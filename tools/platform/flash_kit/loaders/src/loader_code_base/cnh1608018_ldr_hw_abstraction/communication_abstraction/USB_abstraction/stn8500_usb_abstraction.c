/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
/*
 *  @addtogroup ldr_hw_abstraction
 *  @{
 *    @addtogroup USB_abstraction
 *    @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "c_system.h"
#include <stdlib.h>

#include "r_debug.h"
#include "r_debug_macro.h"
#include "r_memory_utils.h"
#include "string.h"
#include "t_basicdefinitions.h"
#include "stddef.h"
#include "t_communication_abstraction.h"
#include "r_USB_abstraction.h"
#include "t_stn8500_usb_abstraction.h"
#include "r_r15_network_layer.h"
#include "r_comm_devices.h"
#include "usb_services.h"
#include "usb.h"
#include "gic.h"
#include "memory_mapping.h"
#include "r_time_utilities.h"
#include "stn8500_usb_dma_support.h"

/*******************************************************************************
 * File scope types, constants and variables
 ******************************************************************************/
#define USB_DEVICES          1

/**
 * Declarations of USB device descriptor.
 */
const CommDevDescr_t USB_DevDescriptor = {
    DEV_USB,
    Do_DevUSBInit,
    Do_DevUSBShutDown,
    NULL,
    NULL,
    0x0
};

/**
 * Declaration of USB device driver.
 */
static USB_Driver_Data_t USBROMDriverData;

/** Interconnection memory for callback functions for USB. */
static CallbackEntry CallbackListUSB[CALLBACKS_PER_DEVICE *USB_DEVICES];

/*
 * Buffer and variables for USB extra data
 */
static uint8  USB_Buffer[512]    = {0};
static uint32 USB_Buffered_Bytes =  0;
static uint32 USB_Buffer_Offset  =  0;

/*******************************************************************************
 * Declaration of file local functions
 ******************************************************************************/
static boolean     Wrap_USB_GetDataNoWait(uint8 *const Data_p, const uint32 NrBytes);
static uint32      Wrap_USB_CheckRxBytesLeft(void);
static boolean     Wrap_USB_PutDataNoWait(uint8 *const Data_p, const uint32 NrBytes);
static uint32      Wrap_USB_CheckTxBytesLeft(void);
static void        FillStructureUSB(USB_Driver_Data_t *const USB_Driver_Data_p);
static ErrorCode_e TxCallbackUSB(const uint8 *const Data_p, const uint32 NrBytes);
static ErrorCode_e RxCallbackUSB(const uint8 *const Data_p, const uint32 NrBytes);
static ErrorCode_e CommCancelUSB(void *Param_p);
static ErrorCode_e SendDataNoWaitUSB(void *Data_p, uint32 SizeInBytes, CommunicationCallback_t Callback_fn, void *Param_p);
static ErrorCode_e GetDataNoWaitUSB(void *Data_p, uint32 SizeInBytes, CommunicationCallback_t Callback_fn, void *Param_p);
static uint32      Read_USB_Buffer(uint8 *const Data_p, const uint32 Number_Of_Bytes);
static void        Store_USB_Buffer(uint8 *Extra_Data_Buffer, uint32 Number_Of_Bytes, t_usb_endp_num USB_EndPoint);

/*******************************************************************************
 * Definition of internal functions
 ******************************************************************************/

static boolean Wrap_USB_GetDataNoWait(uint8 *const Data_p, const uint32 NrBytes)
{
    t_usb_dev_xfer_config USB_Config;
    uint32 Read_Buffered_Bytes = 0;
    uint32 AlignmentLength = 512;
    uint32 Rem = 0;
    uint32 BytesLeft = 0;
    uint32 BytesInRxFIFO = 0;

#ifdef _DISABLE_DMA
    /*
     * set alignment length to 0xFFFFFFFF, so that the whole
     * transfer through USB can go in IT mode
     */
    AlignmentLength = (uint32) - 1;
#endif

    /* Check for buffered data! */
    Read_Buffered_Bytes = Read_USB_Buffer(Data_p, NrBytes);

    if (Read_Buffered_Bytes == NrBytes) {
        /* Transfer finished! */
        (void)RxCallbackUSB(NULL, 0);
    } else {
        BytesLeft = NrBytes - Read_Buffered_Bytes;

        USB_Config.auto_set_or_clear = (t_bool)TRUE;
        USB_Config.send_zero_byte = (t_bool)TRUE;
        USB_Config.p_xfer_buffer = Data_p  + Read_Buffered_Bytes;

        if (BytesLeft < AlignmentLength) {
#ifdef _DISABLE_DMA
            USB_Config.xfer_size = BytesLeft;
            USB_Config.xfer_mode = USB_IT_MODE;

            if (USB_dev_TransferInitiate(USB_ENDPNUM_1_OUT, USB_Config) != USB_OK) {
                return FALSE;
            }

#else
            BytesInRxFIFO = usb_NrOfBytesInRxFIFO(USB_ENDPNUM_1_OUT);

            A_(printf("stn8500_usb_abstraction.c (%d): BytesInRxFIFO %d\n", __LINE__, BytesInRxFIFO);)
            A_(printf("stn8500_usb_abstraction.c (%d): BytesLeft %d\n", __LINE__, BytesLeft);)

            USB_Config.xfer_size = BytesLeft;

            if (BytesLeft == BytesInRxFIFO) {
                usb_ExternalReadFromFifo(USB_ENDPNUM_1_OUT, USB_Config.p_xfer_buffer, USB_Config.xfer_size);
            } else {
                usb_ExternalReadFromFifo(USB_ENDPNUM_1_OUT, USB_Config.p_xfer_buffer, USB_Config.xfer_size);
                USB_dev_FlushTransferFifo(USB_ENDPNUM_1_OUT);
            }

            (void)RxCallbackUSB(NULL, 0);
#endif
        } else {
            Rem = BytesLeft % AlignmentLength;

            OpenDMARxRecord(Data_p + Read_Buffered_Bytes, BytesLeft, Rem);

            USB_Config.xfer_size = BytesLeft - Rem;
            USB_Config.xfer_mode = USB_DMA_MODE;
            USB_Config.dma_req_mode = USB_DMA_REQ_MODE_1;

            InitDMARxTransfer(USB_Config.p_xfer_buffer, USB_Config.xfer_size);

            if (USB_dev_TransferInitiate(USB_ENDPNUM_1_OUT, USB_Config) != USB_OK) {
                return FALSE;
            }
        }
    }

    return TRUE;
}

static uint32 Read_USB_Buffer(uint8 *const Data_p, const uint32 Number_Of_Bytes)
{
    uint32 Bytes_To_Read = 0;

    if (USB_Buffered_Bytes > 0) {
        if (USB_Buffered_Bytes < Number_Of_Bytes) {
            /*
             *
             * Read the available amount of USB buffered data
             * and return the number of bytes read
             *
             */
            Bytes_To_Read = USB_Buffered_Bytes;
        } else {
            Bytes_To_Read = Number_Of_Bytes;
        }

        memcpy(Data_p, USB_Buffer + USB_Buffer_Offset, Bytes_To_Read);

        USB_Buffer_Offset  += Bytes_To_Read;
        USB_Buffered_Bytes -= Bytes_To_Read;
    }

    return Bytes_To_Read;
}

static void Store_USB_Buffer(uint8 *Extra_Data_Buffer, uint32 Number_Of_Bytes, t_usb_endp_num USB_EndPoint)
{
    USB_Buffer_Offset  = 0;
    USB_Buffered_Bytes = 0;

    if ((Extra_Data_Buffer != NULL) && (Number_Of_Bytes <= 512) && (USB_EndPoint == USB_ENDPNUM_1_OUT)) {
        memcpy(USB_Buffer, Extra_Data_Buffer, Number_Of_Bytes);
        USB_Buffered_Bytes = Number_Of_Bytes;
    }
}

static uint32 Wrap_USB_CheckRxBytesLeft(void)
{
    t_usb_xfer_status USB_Status;

    if (IsDMARxRecordOpened()) {
        return 1;
    }

    (void)USB_dev_GetTransferStatus(USB_ENDPNUM_1_OUT, &USB_Status);

    if (USB_Status == USB_TRANSFER_ON_GOING) {
        return 1;  /* Bytes left... */
    }

    return 0;  /* No more bytes left... */
}

static boolean Wrap_USB_PutDataNoWait(uint8 *const Data_p, const uint32 NrBytes)
{
    t_usb_dev_xfer_config USB_Config;
    uint32 AlignmentSize = 512;
    uint32 Rem = 0;

#ifdef _DISABLE_DMA
    /*
     * set alignment length to 0xFFFFFFFF, so that the whole
     * transfer through USB can go in IT mode
     */
    AlignmentSize = (uint32) - 1;
#endif

    USB_Config.send_zero_byte = (t_bool)TRUE;
    USB_Config.auto_set_or_clear = (t_bool)TRUE;
    USB_Config.p_xfer_buffer = Data_p;

    if (NrBytes < AlignmentSize) {
        USB_Config.xfer_size = NrBytes;
        USB_Config.xfer_mode = USB_IT_MODE;

        if (USB_dev_TransferInitiate(USB_ENDPNUM_2_IN, USB_Config) != USB_OK) {
            return FALSE;
        }
    } else {
        Rem = NrBytes % AlignmentSize;
        OpenDMATxRecord(Data_p, NrBytes, Rem);

        USB_Config.xfer_mode = USB_DMA_MODE;
        USB_Config.dma_req_mode = USB_DMA_REQ_MODE_1;
        USB_Config.xfer_size = NrBytes - Rem;

        InitDMATxTransfer(USB_Config.p_xfer_buffer, USB_Config.xfer_size);

        if (USB_dev_TransferInitiate(USB_ENDPNUM_2_IN, USB_Config) != USB_OK) {
            return FALSE;
        }
    }

    return TRUE;
}

static uint32  Wrap_USB_CheckTxBytesLeft(void)
{
    t_usb_xfer_status USB_Status;

    if (IsDMATxRecordOpened()) {
        return 1;
    }

    (void)USB_dev_GetTransferStatus(USB_ENDPNUM_2_IN, &USB_Status);

    if (USB_Status != USB_TRANSFER_ENDED) {
        return 1;  /* Bytes left... */
    }

    return 0;  /* No more bytes left... */
}

static void FillStructureUSB(USB_Driver_Data_t *const USB_Driver_Data_p)
{
    USB_Driver_Data_p->RxNoWait_p    = Wrap_USB_GetDataNoWait;
    USB_Driver_Data_p->CheckRxLeft_p = Wrap_USB_CheckRxBytesLeft;
    USB_Driver_Data_p->TxNoWait_p    = Wrap_USB_PutDataNoWait;
    USB_Driver_Data_p->CheckTxLeft_p = Wrap_USB_CheckTxBytesLeft;
}


/**
 * Creates a device descriptor for setting up communication over the USB
 * interface using the specified speed.
 *
 * @param [in] HighSpeed  TRUE if the device should be configured for HighSpeed
 *                        when init_fn_p is called.
 * @retval                A pointer to a device instance on success or NULL on
 *                        failure.
*/
CommDev_t *Do_GetUSBDescriptor(const boolean HighSpeed)
{
    CommDev_t *Ret_p = NULL;
    USB_InternalData_t *Par_Data_p = NULL;

    Ret_p = (CommDev_t *) malloc(sizeof(CommDev_t));

    if (Ret_p != NULL) {
        Par_Data_p = (USB_InternalData_t *) malloc(sizeof(USB_InternalData_t));

        if (Par_Data_p != NULL) {
            Par_Data_p->HighSpeed   = HighSpeed;
            Par_Data_p->Tx.Buffer_p = NULL;
            Par_Data_p->Rx.Buffer_p = NULL;
            Par_Data_p->Rx.Received = 0;
            Par_Data_p->Rx.Length   = 0;

            Ret_p->Descriptor_p = (CommDevDescr_t *)&USB_DevDescriptor;
            Ret_p->Descriptor_p->Parameters = (void *)Par_Data_p;
        } else {
            BUFFER_FREE(Ret_p);
        }
    }

    return Ret_p;
}

/**
 * The function prepares all data structures that is going to be used by the
 * device. The function also registers interrupt callbacks that the underlying
 * driver might have available.
 *
 * @param [in] Ctx_p       A pointer to a device context. The device context is
 *                         initialized by the function.
 * @param [in] Callback_p  Pointer to Callback functions that will be
 *                         called when data is received or transmitted.
 * @retval            None.
 */
ErrorCode_e Do_DevUSBInit(CommDevContext_t *const Ctx_p, void *const Callback_p)
{
    ErrorCode_e           ReturnValue = E_INVALID_INPUT_PARAMETERS;
    USB_InternalData_t   *USBInternalData_p = (USB_InternalData_t *)(Ctx_p->Data_p);
    t_callback_fct        Temp_Callback_p = ((USBDevCallBack_t *)Callback_p)->USB_Callback_Fn_p;
    t_usb_dev_config      HCLUsbConfig;
    t_usb_dev_port_config USB_Port_Config = {0};
    t_usb_endp_config     EndPoint_Config = {0};
    t_usb_error           USB_RetVal = USB_ERROR;
    uint32                Counter = 0;

    Ctx_p->DevState              = DEV_ERROR;
    USBInternalData_p->IntData_p = NULL;

    /* Initialize USB callback list */
    memset(CallbackListUSB, 0, CALLBACKS_PER_DEVICE * USB_DEVICES * sizeof(CallbackEntry));

    SER_USB_Init(0);

    (void)SER_USB_RegisterCallback(Temp_Callback_p, NULL);

    for (Counter = 0; Counter < USB_MAX_NUM_ENDPOINTS; Counter++) {
        HCLUsbConfig.fifosize[Counter] = (t_usb_fifo_size)0;
    }

    HCLUsbConfig.fifosize[USB_ENDPNUM_0_IN]  = USB_FIFO_SIZE_64;
    HCLUsbConfig.fifosize[USB_ENDPNUM_0_OUT] = USB_FIFO_SIZE_64;
    HCLUsbConfig.fifosize[USB_ENDPNUM_1_OUT] = USB_FIFO_SIZE_512;
    HCLUsbConfig.fifosize[USB_ENDPNUM_2_IN]  = USB_FIFO_SIZE_512;

    HCLUsbConfig.power_mode                  = USB_SELF_POWERED;
    HCLUsbConfig.remote_wakeup_supported     = (t_bool)TRUE;
    HCLUsbConfig.speed                       = USB_HIGH_SPEED;

    USB_RetVal = USB_dev_SetConfiguration(&HCLUsbConfig);
    VERIFY_NO_DEBUG(USB_RetVal == USB_OK, E_FAILED_TO_INIT_COM_DEVICE);

    USB_Port_Config.resume_request       = USB_CLEAR;
    USB_Port_Config.soft_connect_request = USB_SET;

    USB_RetVal = USB_dev_SetPortConfig(USB_Port_Config);
    VERIFY_NO_DEBUG(USB_RetVal == USB_OK, E_FAILED_TO_INIT_COM_DEVICE);

    EndPoint_Config.endp_num             = USB_ENDPNUM_0_OUT;
    EndPoint_Config.endp_type            = USB_CONTROL_XFER;
    EndPoint_Config.maxpacketsize        = 64;
    EndPoint_Config.enable_double_buffer = (t_bool)FALSE;
    USB_RetVal = USB_dev_ConfigureEndpoint(EndPoint_Config);

    EndPoint_Config.endp_num             = USB_ENDPNUM_0_IN;
    EndPoint_Config.endp_type            = USB_CONTROL_XFER;
    EndPoint_Config.maxpacketsize        = 64;
    EndPoint_Config.enable_double_buffer = (t_bool)FALSE;
    USB_RetVal = USB_dev_ConfigureEndpoint(EndPoint_Config);

    EndPoint_Config.endp_num             = USB_ENDPNUM_2_IN;
    EndPoint_Config.endp_type            = USB_BULK_XFER;
    EndPoint_Config.fifo_size            = USB_FIFO_SIZE_512;
    EndPoint_Config.maxpacketsize        = 512;
    EndPoint_Config.enable_double_buffer = (t_bool)FALSE;
    EndPoint_Config.enable_nyet          = (t_bool)FALSE;
    EndPoint_Config.enable_multipacket   = (t_bool)FALSE;
    USB_RetVal = USB_dev_ConfigureEndpoint(EndPoint_Config);
    VERIFY_NO_DEBUG(USB_RetVal == USB_OK, E_FAILED_TO_INIT_COM_DEVICE);

    EndPoint_Config.endp_num             = USB_ENDPNUM_1_OUT;
    EndPoint_Config.endp_type            = USB_BULK_XFER;
    EndPoint_Config.fifo_size            = USB_FIFO_SIZE_512;
    EndPoint_Config.maxpacketsize        = 512;
    EndPoint_Config.enable_double_buffer = (t_bool)TRUE;
    EndPoint_Config.enable_nyet          = (t_bool)TRUE;
    EndPoint_Config.enable_multipacket   = (t_bool)FALSE;
    USB_RetVal = USB_dev_ConfigureEndpoint(EndPoint_Config);
    VERIFY_NO_DEBUG(USB_RetVal == USB_OK, E_FAILED_TO_INIT_COM_DEVICE);

    USB_RegisterCallbackForExtraData(Store_USB_Buffer);

    FillStructureUSB(&USBROMDriverData);
    USBInternalData_p->USBDriver = USBROMDriverData;

    Ctx_p->DevState = DEV_UP;

    ReturnValue = E_SUCCESS;

ErrorExit:
    return ReturnValue;
}

/**
 * The function frees all instance specific memory, flushes transmission data
 * buffers, releases all buffers under the device control and shuts down the
 * device.
 *
 * @param [in] Device_p  A pointer to the device structure.
 * @param [in] CommunicationDevice_p  Pointer to the communication device.
 * @return     None.
 */
void Do_DevUSBShutDown(CommDev_t *Device_p, void *CommunicationDevice_p)
{
    /* TODO: Take down driver before freeing memory. */
    USB_InternalData_t *USB_InternalData = (USB_InternalData_t *)Device_p->Descriptor_p->Parameters;
    uint32 Counter = 0;

    C_(printf("stn8500_usb_abstraction.c(%d): Shutdown USB\n", __LINE__);)

    if (USB_InternalData != NULL) {
        if (GIC_OK != GIC_DisableItLine(GIC_USB_OTG_LINE)) {
            B_(printf("stn8500_usb_abstraction.c(%d): GIC_DisableItLine failed\n", __LINE__);)
        }

        *((uint8 *)(USB_REG_BASE_ADDR + 0x12)) |= 0x20;

        for (Counter = 0; Counter < USB_MAX_NUM_ENDPOINTS; Counter++) {
            USB_dev_SetEndpointStallState(Counter);
            USB_dev_ResetEndpointConfig(Counter);
        }

        // add delay to allow hardware to stall the transfers
        Sleep(10);

        USB_UpdateTransceiver(NULL, USB_0_XCEIVER_SRST);
#ifdef __DB_8500_ED
        // this is only defined for ED hardware
        USB_UpdateTransceiver(NULL, USB_0_XCEIVER_ULPI_DISABLE | USB_0_XCEIVER_XCLK_DISABLE | USB_0_XCEIVER_I2C_OFF);
#endif

        if (USB_InternalData->Rx.Buffer_p != NULL) {
            //R_Do_NL_Buffer_Release_Buffer(USB_internal_data->Rx.Buffer_p);
        }

        if (USB_InternalData->Tx.Buffer_p != NULL) {
            //R_Do_NL_Buffer_Release_Buffer(USB_internal_data->Tx.Buffer_p);
        }

        if (USB_InternalData->IntData_p != NULL) {
            BUFFER_FREE(USB_InternalData->IntData_p);
        }

        BUFFER_FREE(USB_InternalData);

        Device_p->Context.DevState = DEV_DOWN;
        Device_p->Context.Data_p = NULL;
    }
}

/*
 * Initialization of USB communication device.
 *
 * @return Pointer to the initialized USB communication device.
 *
 * @retval NULL if initialization of device fails.
 * @retval Pointer to the initialized communication device.
 *
 */
CommunicationDevice_t *Do_CommDeviceInitUSB(void)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    USBDevCallBack_t CallBack;
    CommunicationDevice_t *CommunicationDevice_p = NULL;
    DeviceParam_t DevParam;

    CommunicationDevice_p = (CommunicationDevice_t *)malloc(sizeof(CommunicationDevice_t));
    ASSERT(NULL != CommunicationDevice_p);

    /* Get Communication device settings */
    Do_GetDeviceSettings(DEV_USB, &DevParam);

    /* Get USB Device descriptor */
    CommunicationDevice_p->Context_p = Do_GetUSBDescriptor(DevParam.HiSpeed);

    if (NULL == CommunicationDevice_p->Context_p) {
        BUFFER_FREE(CommunicationDevice_p);
        goto ErrorExit;
    }

    /* Initialize Device */
    ((CommDev_t *)CommunicationDevice_p->Context_p)->Context.Data_p = ((CommDev_t *)CommunicationDevice_p->Context_p)->Descriptor_p->Parameters;
    CallBack.USB_Callback_Fn_p = (t_callback_fct)CallbackUSB;
    ReturnValue = ((CommDev_t *)CommunicationDevice_p->Context_p)->Descriptor_p->Init_Fn_p(&((CommDev_t *)CommunicationDevice_p->Context_p)->Context, &CallBack);

    if (E_SUCCESS != ReturnValue) {
        BUFFER_FREE(CommunicationDevice_p->Context_p);
        BUFFER_FREE(CommunicationDevice_p);
        goto ErrorExit;
    }

    /* Device drivers */
    ((CommDev_t *)CommunicationDevice_p->Context_p)->Descriptor_p->TxCallback_Fn_p = (void *)((uint32)CallbackUSB);
    ((CommDev_t *)CommunicationDevice_p->Context_p)->Descriptor_p->RxCallback_Fn_p = (void *)((uint32)CallbackUSB);

    CommunicationDevice_p->Read      = GetDataNoWaitUSB;
    CommunicationDevice_p->Write     = SendDataNoWaitUSB;
    CommunicationDevice_p->Cancel    = CommCancelUSB;

#ifndef _DISABLE_DMA
    ReturnValue = InitUSB_DMA(RxCallbackUSB, TxCallbackUSB);
#endif

    if (E_SUCCESS != ReturnValue) {
        BUFFER_FREE(CommunicationDevice_p->Context_p);
        BUFFER_FREE(CommunicationDevice_p);
        goto ErrorExit;
    }

ErrorExit:
    return CommunicationDevice_p;
}

/*
 * Cancel communication via USB device.
 *
 * @param [in] Param_p   Pointer to a communication device structure.
 *
 * @retval E_SUCCESS     If is successfully executed.
 */
static ErrorCode_e CommCancelUSB(void *Param_p)
{
    return E_SUCCESS;
}

/*
 * USB callback function.
 *
 * @param [in] Params_p     Used USB channel.
 * @param [in] IRQ_Status_p USB IRQ status.
 *
 * @retval E_INVALID_INPUT_PARAMETERS If input parameters are not correct.
 * @retval E_SUCCESS                  If is successfully executed.
 */
void CallbackUSB(void *Params_p, void *IRQ_Status_p)
{
    t_usb_xfer_status USB_Status = USB_TRANSFER_OTHER_ERROR;
    t_usb_event       USB_Events;

    USB_Events = *(t_usb_event *)IRQ_Status_p;

    if (USB_Events.endp & USB_IRQ_SRC_IN_EP2) {
        (void)USB_dev_GetTransferStatus(USB_ENDPNUM_2_IN, &USB_Status);

        if (USB_Status == USB_TRANSFER_ENDED) {
            (void)TxCallbackUSB(NULL, 0);
        }
    }

    if (USB_Events.endp & USB_IRQ_SRC_OUT_EP1) {
        (void)USB_dev_GetTransferStatus(USB_ENDPNUM_1_OUT, &USB_Status);

        if (USB_Status != USB_TRANSFER_ON_GOING) {
            (void)RxCallbackUSB(NULL, 0);
        }
    }
}

/*
 * USB transmitter callback function.
 *
 * @param [in] Data_p  Pointer to the transmitted buffer.
 * @param [in] NrBytes Number of data for transmitting.
 *
 * @retval E_INVALID_INPUT_PARAMETERS If input parameters are not correct.
 * @retval E_SUCCESS                  If is successfully executed.
 */
static ErrorCode_e TxCallbackUSB(const uint8 *const Data_p, const uint32 NrBytes)
{
    ErrorCode_e ReturnValue = E_INVALID_INPUT_PARAMETERS;
    uint32      Device;

    if (!IsDMATxRecordOpened()) {
        Device = USB_DEVICES;
        VERIFY_NO_DEBUG(NULL != CallbackListUSB[Device].TxCallback_fn, E_INVALID_INPUT_PARAMETERS);

        CallbackListUSB[Device].TxCallback_fn((void *)Data_p, CallbackListUSB[Device].TxBytes, CallbackListUSB[Device].Param_p);
    } else {
        B_(printf("stn8500_usb_abstraction.c(%d): TX DMA Opened!\n", __LINE__);)
    }

    ReturnValue = E_SUCCESS;

ErrorExit:
    return ReturnValue;
}

/*
 * USB receiver callback function.
 *
 * @param [in] Data_p  Pointer to the received data.
 * @param [in] NrBytes Number of data for receiving.
 *
 * @retval E_INVALID_INPUT_PARAMETERS If input parameters are not correct.
 * @retval E_SUCCESS                  If is successfully executed.
 */
static ErrorCode_e RxCallbackUSB(const uint8 *const Data_p, const uint32 NrBytes)
{
    ErrorCode_e ReturnValue = E_INVALID_INPUT_PARAMETERS;
    uint32      Device      = USB_DEVICES;

    if (!IsDMARxRecordOpened()) {
        VERIFY_NO_DEBUG(NULL != CallbackListUSB[Device].RxCallback_fn, E_INVALID_INPUT_PARAMETERS);
        CallbackListUSB[Device].RxCallback_fn((void *)Data_p, CallbackListUSB[Device].RxBytes, CallbackListUSB[Device].Param_p);
    } else {
        B_(printf("stn8500_usb_abstraction.c(%d): RX DMA Opened!\n", __LINE__);)
    }

    ReturnValue = E_SUCCESS;

ErrorExit:
    return ReturnValue;
}

/**
 * @brief Send request for transmitting data via USB communication device.
 *
 * @param [in] Data_p       Data buffer.
 * @param [in] SizeInBytes  Number of bytes to send.
 * @param [in] Callback_fn  Callback function.
 * @param [in] Param_p      Communication parameters.
 *
 * @retval E_INVALID_INPUT_PARAMETERS If input parameters are not correct.
 * @retval E_COM_DEVICE_BUSY          If communication device is busy.
 * @retval E_FAILED_TO_USE_COM_DEVICE If communication device is ready for
 *                                    transmitting but transmitter do not execute
 *                                    correctly.
 * @retval E_SUCCESS                  If is successfully executed.
 */
static ErrorCode_e SendDataNoWaitUSB(void *Data_p, uint32 SizeInBytes, CommunicationCallback_t Callback_fn, void *Param_p)
{
    ErrorCode_e ReturnValue = E_INVALID_INPUT_PARAMETERS;
    uint32      Device;
    CommDev_t  *Device_p = NULL;
    USB_InternalData_t *InternalData_p = NULL;

    /* Check input parameters */
    VERIFY_NO_DEBUG(((NULL != Data_p) && (0 != SizeInBytes)), E_INVALID_INPUT_PARAMETERS);

    Device_p = (CommDev_t *)(((CommunicationDevice_t *)Param_p)->Context_p);
    InternalData_p = ((USB_InternalData_t *)(Device_p->Context.Data_p));

    Device = USB_DEVICES;
    CallbackListUSB[Device].Device_p      = Device_p;
    CallbackListUSB[Device].TxCallback_fn = Callback_fn;
    CallbackListUSB[Device].Param_p       = Param_p;
    CallbackListUSB[Device].TxBytes       = SizeInBytes;

    VERIFY_NO_DEBUG((0 == InternalData_p->USBDriver.CheckTxLeft_p()), E_COM_DEVICE_BUSY);

    VERIFY_NO_DEBUG((TRUE == InternalData_p->USBDriver.TxNoWait_p((uint8 *)Data_p, SizeInBytes)), E_FAILED_TO_USE_COM_DEVICE);

    ReturnValue = E_SUCCESS;
    CallbackListUSB[Device].TxBytes = 0;

ErrorExit:
    return ReturnValue;
}

/**
 * @brief Send request for receiving data via USB communication device.
 *
 * @param [in] Data_p       Data buffer.
 * @param [in] SizeInBytes  Number of bytes to receive.
 * @param [in] Callback_fn  Callback function.
 * @param [in] Param_p      Communication parameters.
 *
 * @retval FALSE If USB driver is not accept the request for receiving.
 * @retval TRUE  If is successfully executed.
 */
static ErrorCode_e GetDataNoWaitUSB(void *Data_p, uint32 SizeInBytes, CommunicationCallback_t Callback_fn, void *Param_p)
{
    ErrorCode_e ReturnValue = E_INVALID_INPUT_PARAMETERS;
    uint32  Device;

    CommDev_t *Device_p = (CommDev_t *)(((CommunicationDevice_t *)Param_p)->Context_p);
    USB_InternalData_t *InternalData_p = ((USB_InternalData_t *)(Device_p->Context.Data_p));

    while (InternalData_p->USBDriver.CheckRxLeft_p() != 0) {
        ;
    }

    Device = USB_DEVICES; //+ InternalData_p->Device;    /* Calculating the location in callback table ( + USB_DEVICES for read) */
    CallbackListUSB[Device].Device_p      = Device_p;    /* Saving parameters for the callback function */
    CallbackListUSB[Device].RxCallback_fn = Callback_fn;
    CallbackListUSB[Device].Param_p       = Param_p;
    CallbackListUSB[Device].RxBytes       = SizeInBytes;

    ReturnValue = E_COM_DEVICE_BUSY;

    if (TRUE == InternalData_p->USBDriver.RxNoWait_p((uint8 *)Data_p, SizeInBytes)) {
        ReturnValue = E_SUCCESS;
    } else {
        A_(printf("USB_abstraction.c (%d): Failed to Read from USB device!\n", __LINE__));
    }

    return ReturnValue;
}

/* @} */
/* @} */
