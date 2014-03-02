/*******************************************************************************
 * $Copyright Ericsson AB 2009 $
 ******************************************************************************/
/*
 *  @addtogroup ldr_hw_abstraction
 *  @{
 *    @addtogroup UART_abstraction
 *    @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "c_system.h"
#include <stdlib.h>
#include <string.h>
#include "t_basicdefinitions.h"
#include "t_communication_abstraction.h"
#include "r_UART_abstraction.h"
#include "hcl_defs.h"
#include "uart_irq.h"
#include "uart.h"
#include "services.h"
#include "t_stn8500_uart_abstraction.h"
#include "r_r15_network_layer.h"
#include "memory_mapping.h"
#include "t_communication_service.h"
#include "r_comm_devices.h"
#include "error_codes.h"
#include "r_debug.h"
#include "r_debug_macro.h"
#include "uart_p.h"
#include "r_memory_utils.h"

/*******************************************************************************
 * File scope types, constants and variables
 ******************************************************************************/
#define UART_DEVICES         4

/**
 * Declarations of UART device descriptors.
 */
CommDevDescr_t UART_DevDescriptors[] = {
    {
        DEV_UART0,
        Do_DevUARTInit,
        Do_DevUARTShutDown,
        NULL,
        NULL,
        0x0
    },
    {
        DEV_UART1,
        Do_DevUARTInit,
        Do_DevUARTShutDown,
        NULL,
        NULL,
        0x0
    },
    {
        DEV_UART2,
        Do_DevUARTInit,
        Do_DevUARTShutDown,
        NULL,
        NULL,
        0x0
    }
};

/**
 * Declaration of UART device driver(s).
 */
static UART_Driver_Data_t UARTROMDriverData;

/** Interconnection memory for callback functions for UART. */
static CallbackEntry CallbackListUART[CALLBACKS_PER_DEVICE *UART_DEVICES];

/*******************************************************************************
 * Declaration of file local functions
 ******************************************************************************/
static boolean     Wrap_UART_GetDataNoWait(uint8 *const Data_p, const uint32 NrBytes, const t_uart_device_id UDev);
static uint32      Wrap_UART_CheckRxBytesLeft(const t_uart_device_id UDev);
static boolean     Wrap_UART_PutDataNoWait(uint8 const *const Data_p, const uint32 NrBytes, const t_uart_device_id UDev);
static uint32      Wrap_UART_CheckTxBytesLeft(const t_uart_device_id UDev);
static void        FillStructure(UART_Driver_Data_t *const UART_Driver_Data_p);
static ErrorCode_e TxCallbackUART(const uint8 *const Data_p, const uint32 NrBytes, const t_uart_device_id UDev);
static ErrorCode_e RxCallbackUART(const uint8 *const Data_p, const uint32 NrBytes, const t_uart_device_id UDev);
static ErrorCode_e CommCancelUART(void *Param_p);
static ErrorCode_e SendDataNoWaitUART(void *Data_p, uint32 SizeInBytes, CommunicationCallback_t Callback_fn, void *Param_p);
static ErrorCode_e GetDataNoWaitUART(void *Data_p, uint32 SizeInBytes, CommunicationCallback_t Callback_fn, void *Param_p);
/*******************************************************************************
 * Definition of internal functions
 ******************************************************************************/
static boolean Wrap_UART_GetDataNoWait(uint8 *const Data_p, const uint32 NrBytes, const t_uart_device_id UDev)
{
    uint32 Chars_Received = 0;
    uint32 UART_Status = 0;

    return (0 == UART_ReceivexCharacters(UDev, NrBytes, Data_p, &Chars_Received, &UART_Status));
}

static uint32  Wrap_UART_CheckRxBytesLeft(const t_uart_device_id UDev)
{
    t_uart_it_communication_status UART_Status = {0};

    (void)UART_GetComStatus(UDev, &UART_Status);

    if (UART_Status.rx_comm_status != UART_RECEIVE_ENDED) {
        return 1;  //Bytes left...
    } else {
        return 0;  //No more bytes left...
    }
}

static boolean Wrap_UART_PutDataNoWait(uint8 const *const Data_p, const uint32 NrBytes, const t_uart_device_id UDev)
{
    return (0 == UART_TransmitxCharacters(UDev, NrBytes, (t_uint8 *)Data_p));
}

static uint32  Wrap_UART_CheckTxBytesLeft(const t_uart_device_id UDev)
{
    t_uart_it_communication_status UART_Status = {0};

    (void)UART_GetComStatus(UDev, &UART_Status);

    if (UART_Status.tx_comm_status == UART_TRANSMIT_ON_GOING) {
        return 1;  //Bytes left...
    } else {
        return 0;  //No more bytes left...
    }
}


static void FillStructure(UART_Driver_Data_t *const UART_Driver_Data_p)
{
    UART_Driver_Data_p->RxNoWait_p    = Wrap_UART_GetDataNoWait;
    UART_Driver_Data_p->CheckRxLeft_p = Wrap_UART_CheckRxBytesLeft;
    UART_Driver_Data_p->TxNoWait_p    = Wrap_UART_PutDataNoWait;
    UART_Driver_Data_p->CheckTxLeft_p = Wrap_UART_CheckTxBytesLeft;
}

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/
/* This is for the crash handler and is hardcoded to use UART2 for now */
#ifdef ENABLE_DEBUG
void DirectPrint(char *Buffer, uint32 Size)
{
#if(UART_DEBUG_PORT==0)
    t_uart_register *uartreg = (t_uart_register *)(UART_0_CTRL_REG_BASE_ADDR);
#endif
#if(UART_DEBUG_PORT==1)
    t_uart_register *uartreg = (t_uart_register *)(UART_1_CTRL_REG_BASE_ADDR);
#endif
#if(UART_DEBUG_PORT==2)
    t_uart_register *uartreg = (t_uart_register *)(UART_2_CTRL_REG_BASE_ADDR);
#endif
    uint32 count = 0;

    for (count = 0; count < Size; count++) {
        while ((uartreg->uartx_fr & UART_FLAG_MASK_TXF) >> UART_FLAG_SHIFT_TXF) {

        }

        uartreg->uartx_dr = (t_uint32) * (Buffer + count);
    }
}
#endif

CommDev_t *Do_GetUARTDescriptor(const CommDevId_t Device, const t_uart_baud_rate Speed)
{
    CommDev_t *Ret_p = NULL;
    UART_InternalData_t *ParData_p = NULL;

    Ret_p = (CommDev_t *) malloc(sizeof(CommDev_t));

    if (NULL == Ret_p) {
        goto ErrorExit;
    }

    ParData_p = (UART_InternalData_t *)malloc(sizeof(UART_InternalData_t));

    if (NULL == ParData_p) {
        BUFFER_FREE(Ret_p);
        goto ErrorExit;
    }

    ParData_p->Speed = Speed;
    ParData_p->Tx.Buffer_p = NULL;
    ParData_p->Tx.Sending  = 0;
    ParData_p->Rx.Buffer_p = NULL;
    ParData_p->Rx.Received = 0;
    ParData_p->Rx.Length = 0;

    if (DEV_UART0 == Device) {
        Ret_p->Descriptor_p = (CommDevDescr_t *)&UART_DevDescriptors[0];
        ParData_p->Device = UART_DEVICE_ID_0;
        Ret_p->Descriptor_p->Parameters = (void *)ParData_p;
    } else if (DEV_UART1 == Device) {
        Ret_p->Descriptor_p = (CommDevDescr_t *)&UART_DevDescriptors[1];
        ParData_p->Device = UART_DEVICE_ID_1;
        Ret_p->Descriptor_p->Parameters = (void *)ParData_p;
    } else if (DEV_UART2 == Device) {
        Ret_p->Descriptor_p = (CommDevDescr_t *)&UART_DevDescriptors[2];
        ParData_p->Device = UART_DEVICE_ID_2;
        Ret_p->Descriptor_p->Parameters = (void *)ParData_p;
    } else { // Illegal device!
        BUFFER_FREE(Ret_p);
        BUFFER_FREE(ParData_p);
    }

ErrorExit:
    return Ret_p;
}


ErrorCode_e Do_DevUARTInit(CommDevContext_t *const Ctx_p, void *const Callback_p)
{
    ErrorCode_e          ReturnValue        = E_INVALID_INPUT_PARAMETERS;
    UART_InternalData_t *UARTInternalData_p = (UART_InternalData_t *)(Ctx_p->Data_p);
    t_callback_fct       Temp_Callback_p    = ((UARTDevCallBack_t *)Callback_p)->UART_Callback_Fn_p;

    t_uart_config  HCLUartConfig;
    uint8          UART_ID_Mask    = 0;
    t_uart_error   UART_RetVal     = UART_ERROR;
    t_ser_error    UART_SER_RetVal = SERVICE_FAILED;
    static boolean Initialized     = FALSE;

    if (!Initialized) {
        Ctx_p->DevState = DEV_ERROR;
        UARTInternalData_p->IntData_p = NULL;

        switch (UARTInternalData_p->Device) {
        case UART_DEVICE_ID_0:
            UART_SetBaseAddress(UART_DEVICE_ID_0, UART_0_CTRL_REG_BASE_ADDR);
            break;

        case UART_DEVICE_ID_1:
            UART_SetBaseAddress(UART_DEVICE_ID_1, UART_1_CTRL_REG_BASE_ADDR);
            break;

        case UART_DEVICE_ID_2:
            UART_SetBaseAddress(UART_DEVICE_ID_2, UART_2_CTRL_REG_BASE_ADDR);
            break;

        default:
            return E_INVALID_INPUT_PARAMETERS;
        }

        /* Initialize UART callback list */
        memset(CallbackListUART, 0, CALLBACKS_PER_DEVICE * UART_DEVICES * sizeof(CallbackEntry));

        /* UART Device init */
        if (UARTInternalData_p->Debug_Port) {
#ifdef STE_AP8540
            /* Fix for palladium. It might not work on real HW */
            HCLUartConfig.tx_parity_bit   = UART_ODD_PARITY_BIT;
            HCLUartConfig.rx_parity_bit   = UART_ODD_PARITY_BIT;
#else
            HCLUartConfig.tx_parity_bit   = UART_NO_PARITY_BIT;
            HCLUartConfig.rx_parity_bit   = UART_NO_PARITY_BIT;
#endif
        } else {
            HCLUartConfig.tx_parity_bit   = UART_EVEN_PARITY_BIT;
            HCLUartConfig.rx_parity_bit   = UART_EVEN_PARITY_BIT;
        }

        HCLUartConfig.tx_data_bits    = (t_uart_data_bits) UART_DATA_BITS_8;
        HCLUartConfig.rx_data_bits    = (t_uart_data_bits) UART_DATA_BITS_8;
        HCLUartConfig.tx_stop_bits    = (t_uart_stop_bits) UART_ONE_STOP_BIT;
        HCLUartConfig.rx_stop_bits    = (t_uart_stop_bits) UART_ONE_STOP_BIT;
        HCLUartConfig.uart_mode       = UART_NON_MODEM_MODE;
        HCLUartConfig.flow_control    = UART_NO_FLOW_CONTROL;
        HCLUartConfig.receive_enable  = (t_bool)TRUE;
        HCLUartConfig.transmit_enable = (t_bool)TRUE;
        HCLUartConfig.enable_autobaud = (t_bool)FALSE;
        HCLUartConfig.tx_enable_fifo  = HCLUartConfig.rx_enable_fifo = (t_bool)TRUE;
        HCLUartConfig.perr_condition  = UART_PERR_ENDREAD;
        HCLUartConfig.xoff1_char      = 0;
        HCLUartConfig.xon1_char       = 0;
        HCLUartConfig.xoff2_char      = 0;
        HCLUartConfig.xon2_char       = 0;
        HCLUartConfig.tx_trigger      = UART_TX_TRIG_ONE_FOURTH_EMPTY;
        HCLUartConfig.rx_trigger      = UART_RX_TRIG_HALF_FULL;

        /* Baud Rate */
        HCLUartConfig.baud_rate = (t_uart_baud_rate)UARTInternalData_p->Speed;

        UART_ID_Mask = 1UL << UARTInternalData_p->Device;
        SER_UART_Init(UART_ID_Mask);

        UART_RetVal = UART_SetConfiguration(UARTInternalData_p->Device, &HCLUartConfig);
        VERIFY_NO_DEBUG(UART_RetVal == UART_OK, E_FAILED_TO_INIT_COM_DEVICE);

        if (UARTInternalData_p->Debug_Port) {
            UART_RetVal = UART_SetTransferMode(UARTInternalData_p->Device, UART_TX_POLL_MODE | UART_RX_POLL_MODE);
        } else {
            UART_RetVal = UART_SetTransferMode(UARTInternalData_p->Device, UART_TX_POLL_MODE | UART_RX_IT_MODE);
        }

        VERIFY_NO_DEBUG(UART_RetVal == UART_OK, E_FAILED_TO_INIT_COM_DEVICE);

        FillStructure(&UARTROMDriverData);
        UARTInternalData_p->UARTDriver = UARTROMDriverData;

        UART_SER_RetVal = SER_UART_RegisterCallback(UARTInternalData_p->Device, Temp_Callback_p, NULL);
        VERIFY_NO_DEBUG(UART_SER_RetVal == SERVICE_OK, E_FAILED_TO_INIT_COM_DEVICE);

        Ctx_p->DevState = DEV_UP;
    }

    ReturnValue = E_SUCCESS;

ErrorExit:
    return ReturnValue;
}

/**
 * @brief The function frees all memory that has been allocated for the
 *        specified UART device.
 *
 * The function also flushes transmission data buffers, releases all buffers
 * under the device control and shuts down the device.
 *
 * @param [in] Device_p  Pointer to the device structure.
 * @param [in] CommunicationDevice_p  Pointer to the communication device.
 * @return     None.
 */
void Do_DevUARTShutDown(CommDev_t *Device_p, void *CommunicationDevice_p)
{
    UART_InternalData_t *UARTInternalData_p = (UART_InternalData_t *)Device_p->Descriptor_p->Parameters;

    if (NULL != UARTInternalData_p) {
        BUFFER_FREE(UARTInternalData_p->IntData_p);
        BUFFER_FREE(UARTInternalData_p);

        Device_p->Context.DevState = DEV_DOWN;
        Device_p->Context.Data_p = NULL;
    }
}

/*
 * Initialization of UART communication device.
 *
 * @param [in] Device     Specified UART communication device.
 * @param [in] For_Debug  Set to TRUE if UART is used for debug information. FALSE for communication.
 *
 * @retval NULL if initialization of UART device fails.
 * @retval Pointer to the initialized UART communication device.
 */
CommunicationDevice_t *Do_CommDeviceInitUART(const CommDevId_t Device, boolean For_Debug)
{
    ErrorCode_e            ReturnValue           = E_GENERAL_FATAL_ERROR;
    UARTDevCallBack_t      CallBack;
    CommunicationDevice_t *CommunicationDevice_p = NULL;
    CommDev_t             *DeviceData_p          = NULL;
    CommDev_t             *Device_p;
    DeviceParam_t          DevParam;

    /* Get Communication device settings */
    Do_GetDeviceSettings(Device, &DevParam);

    /* Get UART Device descriptor */
    DeviceData_p = Do_GetUARTDescriptor(DevParam.Device, DevParam.Speed);
    VERIFY_NO_DEBUG(NULL != DeviceData_p, ReturnValue);

    /* Initialize Device */
    DeviceData_p->Context.Data_p = DeviceData_p->Descriptor_p->Parameters;
    ((UART_InternalData_t *)(DeviceData_p->Context.Data_p))->Debug_Port = For_Debug;
    CallBack.UART_Callback_Fn_p  = (t_callback_fct)CallbackUART;

    ReturnValue = DeviceData_p->Descriptor_p->Init_Fn_p(&DeviceData_p->Context, &CallBack);
    VERIFY_NO_DEBUG(E_SUCCESS == ReturnValue, ReturnValue);

    /* Device drivers */
    Device_p = (CommDev_t *)malloc(sizeof(CommDev_t));

    if (NULL == Device_p) {
        goto ErrorExit;
    }

    Device_p->Descriptor_p = DeviceData_p->Descriptor_p;
    Device_p->Descriptor_p->TxCallback_Fn_p = (void *)((uint32)CallbackUART);
    Device_p->Descriptor_p->RxCallback_Fn_p = (void *)((uint32)CallbackUART);
    memcpy(&Device_p->Context, &DeviceData_p->Context, sizeof(CommDevContext_t));

    CommunicationDevice_p = (CommunicationDevice_t *)malloc(sizeof(CommunicationDevice_t));

    if (NULL == CommunicationDevice_p) {
        BUFFER_FREE(Device_p);
        goto ErrorExit;
    }

    CommunicationDevice_p->Read      = GetDataNoWaitUART;
    CommunicationDevice_p->Write     = SendDataNoWaitUART;
    CommunicationDevice_p->Cancel    = CommCancelUART;
    CommunicationDevice_p->Context_p = Device_p;

ErrorExit:
    BUFFER_FREE(DeviceData_p);
    return CommunicationDevice_p;
}

/*******************************************************************************
 * Definition of internal functions
 ******************************************************************************/
/*
 * Cancel communication via UART device.
 *
 * @param [in] Param_p   Pointer to a communication device structure.
 *
 * @retval E_SUCCESS     If is successfully executed.
 */
static ErrorCode_e CommCancelUART(void *Param_p)
{
    return E_SUCCESS;
}

/*
 * UART callback function.
 *
 * @param [in] Params_p     Used UART channel.
 * @param [in] IRQ_Status_p UART IRQ status.
 *
 * @retval E_INVALID_INPUT_PARAMETERS If input parameters are not correct.
 * @retval E_SUCCESS                  If is successfully executed.
 */
void CallbackUART(void *Params_p, void *IRQ_Status_p)
{
    t_uart_it_communication_status UART_Status = {0};
    t_uart_device_id               UDev;

#if STE_PLATFORM_NUM == 8500
    UDev = UART_DEVICE_ID_2;
#else
    UDev = UART_DEVICE_ID_0;
#endif

    (void)UART_GetComStatus(UDev, &UART_Status);

    if ((UART_Status.rx_comm_status == UART_RECEIVE_ENDED) && (*(uint32 *)IRQ_Status_p & (UART_IRQ_SRC_RXTO | UART_IRQ_SRC_RX))) {
        /* Call RX Callback function */
        (void)RxCallbackUART(NULL, 0, UDev);
    }

    if ((UART_Status.tx_comm_status != UART_TRANSMIT_ON_GOING) && (*(uint32 *)IRQ_Status_p & UART_IRQ_SRC_TX)) {
        /* Call TX Callback function */
        (void)TxCallbackUART(NULL, 0, UDev);
    }
}

/*
 * UART transmitter callback function.
 *
 * @param [in] Data_p  Pointer to the transmitted buffer.
 * @param [in] NrBytes Number of data for transmitting.
 * @param [in] UDev    Specified UART device.
 *
 * @retval E_INVALID_INPUT_PARAMETERS If input parameters are not correct.
 * @retval E_SUCCESS                  If is successfully executed.
 */
static ErrorCode_e TxCallbackUART(const uint8 *const Data_p, const uint32 NrBytes, const t_uart_device_id UDev)
{
    ErrorCode_e ReturnValue = E_INVALID_INPUT_PARAMETERS;

    VERIFY_NO_DEBUG(NULL != CallbackListUART[UDev].TxCallback_fn, E_INVALID_INPUT_PARAMETERS);
    CallbackListUART[UDev].TxCallback_fn((void *)Data_p, CallbackListUART[UDev].TxBytes, CallbackListUART[UDev].Param_p);

    ReturnValue = E_SUCCESS;

ErrorExit:
    return ReturnValue;
}

/*
 * UART receiver callback function.
 *
 * @param [in] Data_p  Pointer to the received data.
 * @param [in] NrBytes Number of data for receiving.
 * @param [in] UDev    Specified UART device.
 *
 * @retval E_INVALID_INPUT_PARAMETERS If input parameters are not correct.
 * @retval E_SUCCESS                  If is successfully executed.
 */
static ErrorCode_e RxCallbackUART(const uint8 *const Data_p, const uint32 NrBytes, const t_uart_device_id UDev)
{
    ErrorCode_e ReturnValue = E_INVALID_INPUT_PARAMETERS;
    uint32 Device = UDev;

    VERIFY_NO_DEBUG(NULL != CallbackListUART[Device].RxCallback_fn, E_INVALID_INPUT_PARAMETERS);
    CallbackListUART[Device].RxCallback_fn((void *)Data_p, CallbackListUART[Device].RxBytes, CallbackListUART[Device].Param_p);

    ReturnValue = E_SUCCESS;

ErrorExit:
    return ReturnValue;
}


/*
 * Send request for sending data via UART communication device.
 *
 * @param [in] Data_p  Pointer to the sending data.
 * @param [in] NrBytes Number of data for sending.
 * @param [in] Callback_fn Call to callback function when finish with sending
 *                         requested data.
 * @param [in] Param_p     External parameters.
 *
 * @retval E_INVALID_INPUT_PARAMETERS If input parameters are not correct.
 * @retval E_COM_DEVICE_BUSY          If communication device is busy.
 * @retval E_FAILED_TO_USE_COM_DEVICE If communication device is ready for
 *                                    transmitting but transmitter do not execute
 *                                    correctly.
 * @retval E_SUCCESS                  If is successfully executed.
 */
static ErrorCode_e SendDataNoWaitUART(void *Data_p, uint32 SizeInBytes, CommunicationCallback_t Callback_fn, void *Param_p)
{
    ErrorCode_e ReturnValue = E_INVALID_INPUT_PARAMETERS;
    UART_InternalData_t *InternalData_p = NULL;
    t_uart_device_id Device;
    CommDev_t *Device_p = (CommDev_t *)(((CommunicationDevice_t *)Param_p)->Context_p);

    /* Check input parameters */
    VERIFY_NO_DEBUG(((NULL != Data_p) && (0 != SizeInBytes)), E_INVALID_INPUT_PARAMETERS);

    /* Check type of communication device */
    InternalData_p = ((UART_InternalData_t *)(Device_p->Context.Data_p));
    ReturnValue = E_COM_DEVICE_BUSY;
    Device = InternalData_p->Device;
    CallbackListUART[Device].Device_p      =  Device_p;    /* Saving parameters for the callback function */
    CallbackListUART[Device].TxCallback_fn = Callback_fn;
    CallbackListUART[Device].Param_p       = Param_p;
    CallbackListUART[Device].TxBytes       = SizeInBytes;

    VERIFY_NO_DEBUG((0 == InternalData_p->UARTDriver.CheckTxLeft_p(Device)), E_COM_DEVICE_BUSY);

    VERIFY_NO_DEBUG((TRUE == InternalData_p->UARTDriver.TxNoWait_p((const uint8 *)Data_p, SizeInBytes, InternalData_p->Device)), E_FAILED_TO_USE_COM_DEVICE);

    if (NULL != Callback_fn) {
        Callback_fn(Data_p, SizeInBytes, Param_p);
    }

    ReturnValue = E_SUCCESS;

ErrorExit:
    return ReturnValue;
}

/*
 * Send request for receiving data via UART communication device.
 *
 * @param [in] Data_p      Pointer to the received data.
 * @param [in] SizeInBytes Number of data for receiving.
 * @param [in] Callback_fn Call to callback function when finish with receiving
 *                         requested data.
 * @param [in] Param_p     External parameters.
 *
 * @retval FALSE If UART driver is not accept the request for receiving.
 * @retval TRUE  If is successfully executed.
 */
static ErrorCode_e GetDataNoWaitUART(void *Data_p, uint32 SizeInBytes, CommunicationCallback_t Callback_fn, void *Param_p)
{
    ErrorCode_e ReturnValue = E_INVALID_INPUT_PARAMETERS;
    t_uart_device_id Device;
    CommDev_t *Device_p = (CommDev_t *)(((CommunicationDevice_t *)Param_p)->Context_p);
    UART_InternalData_t *InternalData_p = ((UART_InternalData_t *)(Device_p->Context.Data_p));

    Device = InternalData_p->Device;
    CallbackListUART[Device].Device_p      =  Device_p;    /* Saving parameters for the callback function */
    CallbackListUART[Device].RxCallback_fn = Callback_fn;
    CallbackListUART[Device].Param_p       = Param_p;
    CallbackListUART[Device].RxBytes       = SizeInBytes;

    ReturnValue = E_COM_DEVICE_BUSY;

    if (TRUE == InternalData_p->UARTDriver.RxNoWait_p((uint8 *)Data_p, SizeInBytes, InternalData_p->Device)) {
        ReturnValue = E_SUCCESS;
    } else {
        A_(printf("UART_abstraction.c (%d): Failed to Read from UART device! Device: %d!\n", __LINE__, Device));
    }

    return ReturnValue;
}

/* @} */
/* @} */
