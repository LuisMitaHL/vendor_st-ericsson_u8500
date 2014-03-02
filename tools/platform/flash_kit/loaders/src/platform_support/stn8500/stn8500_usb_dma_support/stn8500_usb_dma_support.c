/***********************************************************************
 * $Copyright ST-Ericsson 2010$
 **********************************************************************/

/**
 * @file   stn8500_usb_dma_support.c
 * @brief  Contains functions for enabling, initiating and controlling DMA
 *         powered USB transfer
 * @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <string.h>
#include "error_codes.h"
#include "dma_services.h"
#include "r_debug_macro.h"
#include "memory_mapping.h"
#include "stn8500_usb_dma_support.h"
#include "r_time_utilities.h"
#include "r_mmu.h"

#define USB_DEV_0_TXFIFO(i) (USB_REG_BASE_ADDR + 32 + (4 * (i % (USB_MAX_NUM_ENDPOINTS / 2))))
#define DMA_MAX_SIZE        (64 * 1024)

//configuring DMA channels that are HW dependent
#if STE_PLATFORM_NUM == 8500
#define DMA_TX_CHAN DMA_CHAN_5
#define DMA_RX_CHAN DMA_CHAN_4
#else

#if STE_PLATFORM_NUM == 5500
#define DMA_TX_CHAN DMA_CHAN_0
#define DMA_RX_CHAN DMA_CHAN_1
#else

#ifndef _DISABLE_DMA //If DMA is ENABLED, and platform is not u8500 or u5500, generate error
#error Unknown platform, DMA channels can not be configured. Please disable DMA.
#else
#warning There is no DMA support for this platform!.
#endif

#endif //STE_PLATFORM_NUM == 5500
#endif //STE_PLATFORM_NUM == 8500

static DMARecord_t DMARxRecord;
static DMARecord_t DMATxRecord;

/*******************************************************************************
 * Declaration of file local functions
 ******************************************************************************/

/***************************************************************************//**
* @brief Initializes DMA services and sets up USB callbacks
*
*******************************************************************************/
static void InitDMA(void);

/***************************************************************************//**
* @brief Opens DMA Tx channel
*
* Opens DMA channel for sending data over USB
*
* @return ErrorCode_e Forwards the response from DMA controller
*
*******************************************************************************/
static ErrorCode_e OpenDMATxChannel(void);

/***************************************************************************//**
* @brief Closes DMA Tx channel
*
* Closes DMA channel opened for sending data over USB
*
* @return ErrorCode_e Forwards the response from DMA controller
*
*******************************************************************************/
static ErrorCode_e CloseDMATxChannel(void);

/***************************************************************************//**
* @brief Opens DMA Rx channel
*
* Opens DMA channel for receiving data over USB
*
* @return ErrorCode_e Forwards the response from DMA controller
*
*******************************************************************************/
static ErrorCode_e OpenDMARxChannel(void);

/***************************************************************************//**
* @brief Closes DMA Rx channel
*
* Closes DMA channel opened for receiving data over USB
*
* @return ErrorCode_e Forwards the response from DMA controller
*
*******************************************************************************/
static ErrorCode_e CloseDMARxChannel(void);

/***************************************************************************//**
* @brief Stops DMA transfer
*
* Stops DMA transfer on both Rx and Tx channel
*
* @return ErrorCode_e E_FAILED_TO_STOP_DMA on failure, E_SUCCESS otherwise
*
*******************************************************************************/
static ErrorCode_e StopDMATransfer(void);

/***************************************************************************//**
* @brief DMA callback function
*
* Callback function that is triggered on end of each DMA transfer
* The same callback function is called on end of RX and Tx transfer
*
* @param[in]  ChannelType   Type of the channel that triggered the DMA interrupt
* @param[in]  LineNum       The line number on which the interrupt has been triggered
*
*
* @remark This function does not return any value, since it is called by the DMA
*         ISR and should be of type t_dma_callback_fct, even though an error can occur
*         while executing it
*
*******************************************************************************/
static void DMA_Callback(t_dma_chan_type ChannelType, t_dma_line_num LineNum);
static void RX_DMA_Callback(void);
static void TX_DMA_Callback(void);

/***************************************************************************//**
* @brief Sets USB configuration
*
* Sets the USB configuration parameters prior initiating a new transfer
*
* @param[in]  Buffer_p      Pointer to the data buffer which need to be transfered
* @param[in]  Size          Size of Buffer_p
* @param[in]  Mode          Mode of the transfer (can be IT or DMA)
* @param[out] USB_Config_p  USB configuration structure
*
********************************************************************************/
static void SetUSBConfig(uint8 *Buffer_p, t_size Size, t_usb_mode Mode, t_usb_dev_xfer_config *USB_Config_p);

/***************************************************************************//**
* @brief Sets DMA callback functions
*
* Sets the callback functions triggered on end of each transfer (DMA and IT, if any)
*
* @param[in]  RxCallbackUSB      Callback function to be triggered on end of Rx transfer
* @param[in]  TxCallbackUSB      Callback function to be triggered on end of Tx transfer
*
********************************************************************************/
static ErrorCode_e SetDMACallbackFunctions(CallbackUSB_fn RxCallbackUSB, CallbackUSB_fn TxCallbackUSB);

static t_dma_channel_config  p_tx_chan_desc;
static t_dma_pipe_config     p_tx_pipe_desc;

static t_dma_channel_config  p_rx_chan_desc;
static t_dma_pipe_config     p_rx_pipe_desc;

static CallbackFunctions_t CallbackFunctions;

static volatile boolean IsDMAInit = FALSE;

/*******************************************************************************
 * Definition of internal functions
 ******************************************************************************/
static ErrorCode_e SetDMACallbackFunctions(CallbackUSB_fn RxCallbackUSB, CallbackUSB_fn TxCallbackUSB)
{
    ErrorCode_e ReturnValue = E_SUCCESS;

    VERIFY((NULL != RxCallbackUSB) && (NULL != TxCallbackUSB), E_INVALID_INPUT_PARAMETER);

    CallbackFunctions.RxCallbackUSB_fn = RxCallbackUSB;
    CallbackFunctions.TxCallbackUSB_fn = TxCallbackUSB;

ErrorExit:
    return ReturnValue;
}

static ErrorCode_e StopDMATransfer(void)
{
    ErrorCode_e ReturnValue = E_SUCCESS;
    t_dma_error dma_error;

    dma_error = DMA_TransferActivate(p_rx_chan_desc, DMA_STOP);
    VERIFY(DMA_OK == dma_error, E_FAILED_TO_STOP_DMA);

    dma_error = DMA_TransferActivate(p_tx_chan_desc, DMA_STOP);
    VERIFY(DMA_OK == dma_error, E_FAILED_TO_STOP_DMA);

ErrorExit:
    return ReturnValue;
}

static ErrorCode_e OpenDMATxChannel(void)
{
    ErrorCode_e ReturnValue = E_SUCCESS;
    t_dma_error dma_error;

    C_(printf("Opening DMA Tx Channel between Memory and Peripheral\n");)

    dma_error = DMA_OpenChannel(DMA_TX_CHAN, &p_tx_chan_desc);
    VERIFY(DMA_OK == dma_error, E_FAILED_TO_OPEN_DMA_CHANNEL);

    dma_error = DMA_SetChannelType(DMA_STANDARD, DMA_NONSECURE, &p_tx_chan_desc);
    VERIFY(DMA_OK == dma_error, E_FAILED_TO_SET_DMA_CHANNEL_TYPE);

    dma_error = DMA_SetChannelModeOption(DMA_PHYSICAL, DMA_BASIC, &p_tx_chan_desc);
    VERIFY(DMA_OK == dma_error, E_FAILED_TO_SET_DMA_CHANNEL_MODE);

    dma_error = DMA_OpenPipe(p_tx_chan_desc, DMA_MEMORY_SRC, DMA_USB_OTG_IEP2_DEVICE, &p_tx_pipe_desc);
    VERIFY(DMA_OK == dma_error, E_FAILED_TO_OPEN_DMA_PIPE);

    dma_error = DMA_SetTCInterruptLogic(p_tx_pipe_desc, DMA_EOT, FALSE, TRUE);
    VERIFY(DMA_OK == dma_error, E_FAILED_TO_SET_DMA_IT_LOGIC);

ErrorExit:
    return ReturnValue;
}

static ErrorCode_e CloseDMATxChannel(void)
{
    ErrorCode_e ReturnValue = E_SUCCESS;
    t_dma_error dma_error;

    dma_error = DMA_ClosePipe(p_tx_chan_desc, p_tx_pipe_desc);
    VERIFY(DMA_OK == dma_error, E_FAILED_TO_CLOSE_DMA_PIPE);

    dma_error = DMA_CloseChannel(p_tx_chan_desc);
    VERIFY(DMA_OK == dma_error, E_FAILED_TO_CLOSE_DMA_CHANNEL);

ErrorExit:
    return ReturnValue;
}

static ErrorCode_e OpenDMARxChannel(void)
{
    ErrorCode_e ReturnValue = E_SUCCESS;
    t_dma_error dma_error;

    C_(printf("Opening DMA Rx Channel between Memory and Peripheral\n");)

    dma_error = DMA_OpenChannel(DMA_RX_CHAN, &p_rx_chan_desc);
    VERIFY(DMA_OK == dma_error, E_FAILED_TO_OPEN_DMA_CHANNEL);

    dma_error = DMA_SetChannelType(DMA_STANDARD, DMA_NONSECURE, &p_rx_chan_desc);
    VERIFY(DMA_OK == dma_error, E_FAILED_TO_SET_DMA_CHANNEL_TYPE);

    dma_error = DMA_SetChannelModeOption(DMA_PHYSICAL, DMA_BASIC, &p_rx_chan_desc);
    VERIFY(DMA_OK == dma_error, E_FAILED_TO_SET_DMA_CHANNEL_MODE);

    dma_error = DMA_OpenPipe(p_rx_chan_desc, DMA_USB_OTG_OEP1_DEVICE, DMA_MEMORY_DEST, &p_rx_pipe_desc);
    VERIFY(DMA_OK == dma_error, E_FAILED_TO_OPEN_DMA_PIPE);

    dma_error = DMA_SetTCInterruptLogic(p_rx_pipe_desc, DMA_EOT, TRUE, TRUE);
    VERIFY(DMA_OK == dma_error, E_FAILED_TO_SET_DMA_IT_LOGIC);

ErrorExit:
    return ReturnValue;
}

static ErrorCode_e CloseDMARxChannel(void)
{
    ErrorCode_e ReturnValue = E_SUCCESS;
    t_dma_error dma_error;

    dma_error = DMA_ClosePipe(p_rx_chan_desc, p_rx_pipe_desc);
    VERIFY(DMA_OK == dma_error, E_FAILED_TO_CLOSE_DMA_PIPE);

    dma_error = DMA_CloseChannel(p_rx_chan_desc);
    VERIFY(DMA_OK == dma_error, E_FAILED_TO_CLOSE_DMA_CHANNEL);

ErrorExit:
    return ReturnValue;
}

static void InitDMA(void)
{
    SER_DMA_RegisterCallback(DMA_Callback);
    SER_DMA_Init(0);
}

static void DMA_Callback(t_dma_chan_type ChannelType, t_dma_line_num LineNum)
{
    C_(printf("DMA callback for line %d\n", LineNum);)

    switch (LineNum) {
    case DMA_TX_CHAN:
        TX_DMA_Callback();
        break;

    case DMA_RX_CHAN:
        RX_DMA_Callback();
        break;

    default:
        A_(printf("ERROR: Invalid Interrupt Line Number!");)
        break;
    }
}

static void RX_DMA_Callback(void)
{
    t_usb_dev_xfer_config USB_Config;
    static uint32         RxDMACount = 1;
    t_usb_endp_num        EndpNum;

    C_(printf("RX DMA callback!\n");)

    EndpNum = USB_ENDPNUM_1_OUT;


    if (TRUE == DMARxRecord.MoreChunks) {
        /* Initiate the next DMA transfer */
        uint32 RemSize = (DMARxRecord.NrBytes - DMARxRecord.Offset) - DMA_MAX_SIZE * RxDMACount;

        SetUSBConfig(DMARxRecord.Data_p + DMA_MAX_SIZE * RxDMACount, RemSize, USB_DMA_MODE, &USB_Config);

        InitDMARxTransfer(USB_Config.p_xfer_buffer, USB_Config.xfer_size);

        RxDMACount++;

    } else {
        CloseDMATransferRecord(&DMARxRecord);

        MMU_InvalidateL2((uint32)(DMARxRecord.Data_p), DMARxRecord.NrBytes);

        if (DMARxRecord.Offset != 0) {
            uint8 *ITData_p = DMARxRecord.Data_p + (DMARxRecord.NrBytes - DMARxRecord.Offset);
            t_usb_error USBError;

            SetUSBConfig(ITData_p, DMARxRecord.Offset, USB_IT_MODE, &USB_Config);

            USBError = USB_dev_TransferInitiate(EndpNum, USB_Config);

            if (USB_OK != USBError) {
                A_(printf("USB_dev_TransferInitiate Error: %d\n", USBError));
                return;
            }
        } else {
            if (CallbackFunctions.RxCallbackUSB_fn) {
                CallbackFunctions.RxCallbackUSB_fn(NULL, 0);
            }
        }

        RxDMACount = 1;

    }
}

static void TX_DMA_Callback(void)
{
    t_usb_dev_xfer_config USB_Config;
    static uint32         TxDMACount = 1;
    t_usb_endp_num        EndpNum;

    C_(printf("TX DMA callback!\n");)

    EndpNum = USB_ENDPNUM_2_IN;

    if (TRUE == DMATxRecord.MoreChunks) {
        /* Initiate the next DMA transfer */
        uint32 RemSize = (DMATxRecord.NrBytes - DMATxRecord.Offset) - DMA_MAX_SIZE * TxDMACount;

        SetUSBConfig(DMATxRecord.Data_p + DMA_MAX_SIZE * TxDMACount, RemSize, USB_DMA_MODE, &USB_Config);

        InitDMATxTransfer(USB_Config.p_xfer_buffer, USB_Config.xfer_size);

        TxDMACount++;

    } else {
        CloseDMATransferRecord(&DMATxRecord);

        if (DMATxRecord.Offset != 0) {
            uint8 *ITData_p = DMATxRecord.Data_p + (DMATxRecord.NrBytes - DMATxRecord.Offset);
            t_usb_error USBError;

            SetUSBConfig(ITData_p, DMATxRecord.Offset, USB_IT_MODE, &USB_Config);

            USBError = USB_dev_TransferInitiate(EndpNum, USB_Config);

            if (USB_OK != USBError) {
                A_(printf("USB_dev_TransferInitiate Error: %d\n", USBError));
                return;
            }
        } else {
            t_usb_error USB_Error;

            /* Send zero byte to signal end of transfer */
            C_(printf("Send zero byte!\n");)
            USB_Config.auto_set_or_clear = (t_bool)TRUE;
            USB_Config.send_zero_byte = (t_bool)TRUE;
            USB_Config.p_xfer_buffer = NULL;
            USB_Config.xfer_size = 0;
            USB_Config.xfer_mode = USB_IT_MODE;

            USB_Error = USB_dev_TransferInitiate(USB_ENDPNUM_2_IN, USB_Config);

            if (USB_Error != USB_OK) {
                A_(printf("Zero byte failed %d\n", USB_Error);)
                return;
            }
        }

        TxDMACount = 1;
    }
}

static void SetUSBConfig(uint8 *Buffer_p, t_size Size, t_usb_mode Mode, t_usb_dev_xfer_config *USB_Config_p)
{
    USB_Config_p->send_zero_byte = (t_bool)TRUE;
    USB_Config_p->auto_set_or_clear = (t_bool)TRUE;
    USB_Config_p->p_xfer_buffer = Buffer_p;
    USB_Config_p->xfer_size = Size;
    USB_Config_p->xfer_mode = Mode;
    USB_Config_p->dma_req_mode = USB_DMA_REQ_MODE_1;
}

ErrorCode_e InitUSB_DMA(CallbackUSB_fn RxCallbackUSB, CallbackUSB_fn TxCallbackUSB)
{
    ErrorCode_e ReturnValue = E_SUCCESS;

    VERIFY(FALSE == IsDMAInit, ReturnValue);

    InitDMA();

    ReturnValue = SetDMACallbackFunctions(RxCallbackUSB, TxCallbackUSB);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    ReturnValue = OpenDMARxChannel();
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    ReturnValue = OpenDMATxChannel();
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    memset(&DMARxRecord, 0, sizeof(DMARxRecord));
    memset(&DMATxRecord, 0, sizeof(DMATxRecord));

    IsDMAInit = TRUE;

ErrorExit:
    return ReturnValue;
}

ErrorCode_e ShutdownUSB_DMA(void)
{
    ErrorCode_e ReturnValue = E_SUCCESS;

    VERIFY(TRUE == IsDMAInit, ReturnValue);

    ReturnValue = StopDMATransfer();
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    ReturnValue = CloseDMARxChannel();
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    ReturnValue = CloseDMATxChannel();
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    IsDMAInit = FALSE;

ErrorExit:
    return ReturnValue;
}

ErrorCode_e InitDMATxTransfer(uint8 *const Data_p, uint32 NrBytes)
{
    ErrorCode_e ReturnValue = E_SUCCESS;
    t_dma_error dma_error;

    C_(printf("Initiate DMA Tx transfer with %uB of data\n", NrBytes);)

    if (NrBytes > DMA_MAX_SIZE) {
        DMATxRecord.MoreChunks = TRUE;
        NrBytes = DMA_MAX_SIZE;
    } else {
        DMATxRecord.MoreChunks = FALSE;
    }

    //    dma_error = DMA_TransferActivate(p_tx_chan_desc, DMA_STOP);
    //    VERIFY(DMA_OK == dma_error, E_FAILED_TO_STOP_DMA);

    dma_error = DMA_ConfigSrcDevice(p_tx_pipe_desc, (t_uint32)Data_p, DMA_WORD_WIDTH, DMA_BURST_SIZE_16, 4, NrBytes, 0);
    VERIFY(DMA_OK == dma_error, E_FAILED_TO_CONFIG_SRC_DMA_DEVICE);

    dma_error = DMA_UpdateSrcMemoryBuffer(p_tx_chan_desc, p_tx_pipe_desc);
    VERIFY(DMA_OK == dma_error, E_FAILED_TO_UPDATE_DMA_SRC_MEMORY_BUFFER);

    dma_error = DMA_ConfigDestDevice(p_tx_pipe_desc, (t_physical_address)USB_DEV_0_TXFIFO(USB_ENDPNUM_2_IN), DMA_WORD_WIDTH, DMA_BURST_SIZE_16, 0, NrBytes, 0);
    VERIFY(DMA_OK == dma_error, E_FAILED_TO_CONFIG_DEST_DMA_DEVICE);

    dma_error = DMA_UpdateDestMemoryBuffer(p_tx_chan_desc, p_tx_pipe_desc);
    VERIFY(DMA_OK == dma_error, E_FAILED_TO_UPDATE_DMA_DEST_MEMORY_BUFFER);

    dma_error = DMA_TransferActivate(p_tx_chan_desc, DMA_RUN);
    VERIFY(DMA_OK == dma_error, E_FAILED_TO_RUN_DMA);

ErrorExit:
    return ReturnValue;
}

ErrorCode_e InitDMARxTransfer(uint8 *const Data_p, uint32 NrBytes)
{
    ErrorCode_e ReturnValue = E_SUCCESS;
    t_dma_error dma_error;

    C_(printf("DMA Rx initiate with %dB of data (%p)\n", NrBytes, Data_p);)

    if (NrBytes > DMA_MAX_SIZE) {
        DMARxRecord.MoreChunks = TRUE;
        NrBytes = DMA_MAX_SIZE;
    } else {
        DMARxRecord.MoreChunks = FALSE;
    }

    //dma_error = DMA_TransferActivate(p_rx_chan_desc, DMA_STOP);
    //VERIFY(DMA_OK == dma_error, E_FAILED_TO_STOP_DMA);

    dma_error = DMA_ConfigSrcDevice(p_rx_pipe_desc, (t_physical_address)USB_DEV_0_TXFIFO(USB_ENDPNUM_1_OUT), DMA_WORD_WIDTH, DMA_BURST_SIZE_16, 0, NrBytes, 0);
    VERIFY(DMA_OK == dma_error, E_FAILED_TO_CONFIG_SRC_DMA_DEVICE);

    dma_error = DMA_UpdateSrcMemoryBuffer(p_rx_chan_desc, p_rx_pipe_desc);
    VERIFY(DMA_OK == dma_error, E_FAILED_TO_UPDATE_DMA_SRC_MEMORY_BUFFER);

    dma_error = DMA_ConfigDestDevice(p_rx_pipe_desc, (t_uint32)Data_p, DMA_WORD_WIDTH, DMA_BURST_SIZE_16, 4, NrBytes, 0);
    VERIFY(DMA_OK == dma_error, E_FAILED_TO_CONFIG_DEST_DMA_DEVICE);

    dma_error = DMA_UpdateDestMemoryBuffer(p_rx_chan_desc, p_rx_pipe_desc);
    VERIFY(DMA_OK == dma_error, E_FAILED_TO_UPDATE_DMA_DEST_MEMORY_BUFFER);

    dma_error = DMA_TransferActivate(p_rx_chan_desc, DMA_RUN);
    VERIFY(DMA_OK == dma_error, E_FAILED_TO_RUN_DMA);

ErrorExit:
    return ReturnValue;
}

void OpenDMARxRecord(uint8 *const Data_p, const uint32 NrBytes, const uint32 Offset)
{
    DMARxRecord.Data_p   = Data_p;
    DMARxRecord.NrBytes  = NrBytes;
    DMARxRecord.Offset   = Offset;
    DMARxRecord.IsOpened = TRUE;
}

void OpenDMATxRecord(uint8 *const Data_p, const uint32 NrBytes, const uint32 Offset)
{
    DMATxRecord.Data_p   = Data_p;
    DMATxRecord.NrBytes  = NrBytes;
    DMATxRecord.Offset   = Offset;
    DMATxRecord.IsOpened = TRUE;
}

boolean IsDMARxRecordOpened()
{
    return DMARxRecord.IsOpened;
}
boolean IsDMATxRecordOpened()
{
    return DMATxRecord.IsOpened;
}

void CloseDMATransferRecord(DMARecord_t *pTransferRecord)
{
    pTransferRecord->IsOpened = FALSE;
}
