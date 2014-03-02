/***********************************************************************
 * $Copyright Ericsson AB 2009 $
 **********************************************************************/
/**
 * \addtogroup ldr_emulator
 * @{
 *      \addtogroup UART
 *      \brief Emulated UART functions for R15 emulator
 *      @{
 */

#ifdef WIN32
#include <windows.h>
#endif //WIN32
#include <stdio.h>
#include "c_system.h"
#include "t_basicdefinitions.h"

#include "asic_uart_macro.h"
#include "asic_uart_map.h"
#include "t_hl1uart.h"
#include "r_hl1uart.h"
#include "r_hl1intcon.h"
#include "r_hl1pmux.h"
#include "r_hl1sys.h"
//#include "t_hl1cdetect.h"
//#include "r_hl1cdetect.h"
#include <stddef.h>
#include <string.h>

//#include "jmptable.h"
#include "t_hl1uart_internal.h"
#include "r_uart_fifo.h"
#include "r_debug.h"

#define DECLARE_UART_HANDLES
#include "t_emul_threads.h"
#define DEF

#include "t_emulator.h"
/* ---------------------------------------------------------------------------
 * Local definitions and types
 * ---------------------------------------------------------------------------
 */

/* The maxium number of supported UARTs. */
#define HL1UART_NBR_OF_UARTS   NO_OF_HL1UART_DEVICES   // TODO

typedef struct {
    HL1UART_Callback_t CallbackFunc;
    const uint8 *Data_p;
    uint32 NrBytes;
} IrqCallback_t;

typedef struct {
    uint8 *Data_p;
    uint8 *Overflow_p;
    uint32 OverflowReadCntr;
    uint32 OverflowWriteCntr;
    uint32 OverflowSize;
    uint32 BytesOverflow;
    uint32 BytesToReceive;
    uint32 BytesReceived;
    IrqCallback_t Callback;
} RxBufferMgmt_t;

typedef struct {
    const uint8 *Data_p;
    uint32 BytesToSend;
    uint32 BytesSent;
    IrqCallback_t Callback;
} TxBufferMgmt_t;

typedef struct {
    uint32 UartOffset;
    RxBufferMgmt_t RxMgmt;
    TxBufferMgmt_t TxMgmt;
} UartMem_t;

typedef struct {
    uint8 UartCableInserted;   // Each bit represents a physical UART device
    UartMem_t *UartMem[HL1UART_NBR_OF_UARTS];
} Uarts_t;

// This is zero because we don't use Cable detect in the UART emulator
#define CDETECT_MEM() (0)


/* ---------------------------------------------------------------------------
 * Global variables
 * ---------------------------------------------------------------------------
 */

Uarts_t *Uarts_p;
extern volatile uint32 UARTBytesToSend;
/* ---------------------------------------------------------------------------
 * Function prototypes
 * ---------------------------------------------------------------------------
 */

static boolean CheckDevice(const HL1UART_Device_t UDev);
void    UART_IRQ_Handler(const HL1UART_Device_t UDev);
//static void    UART_CD_IRQ_Handler(const HL1CDETECT_Device_t Device, const boolean Attach);
static boolean UART_EnableInterrupts(const HL1UART_Device_t UDev);
static boolean UART_DisableInterrupts(const HL1UART_Device_t UDev);
static void    UART_InternalGetData(const HL1UART_Device_t UDev, RxBufferMgmt_t *const RxMgmt);
static void    UART_InternalDeviceInit(void *const Data_p, const uint32 Size, const HL1UART_Device_t UDev);
static boolean UART_CheckOkToInit(const HL1UART_Device_t UDev);
static void RestoreInterrupts(uint16 Interrupt_Mask);

static void UART_IRQ_Handler_0(void);
static void UART_IRQ_Handler_1(void);
static void UART_IRQ_Handler_2(void);
static void UART_IRQ_Handler_3(void);
static void UART_IRQ_Handler_4(void);

/* ---------------------------------------------------------------------------
 * Local data
 * ---------------------------------------------------------------------------
 */
static uint16 InterruptMask = 0;

static const HL1UART_Device_t UART_InvCdDevs[] = {
    HL1UART_DEVICE_0,
    HL1UART_DEVICE_1,
    HL1UART_DEVICE_2,
    HL1UART_DEVICE_3,
    HL1UART_DEVICE_4
};
static const uint32 UART_BRSettings[] = {
    UART_BR_26_MHZ_1625000,
    UART_BR_26_MHZ_921600,
    UART_BR_26_MHZ_812500,
    UART_BR_26_MHZ_460800,
    UART_BR_26_MHZ_230400,
    UART_BR_26_MHZ_115200,
    UART_BR_26_MHZ_57600,
    UART_BR_26_MHZ_38400,
    UART_BR_26_MHZ_28800,
    UART_BR_26_MHZ_19200,
    UART_BR_26_MHZ_14400,
    UART_BR_26_MHZ_9600,
    UART_BR_26_MHZ_4800,
    UART_BR_26_MHZ_2400,
    UART_BR_26_MHZ_1200
};


/* ---------------------------------------------------------------------------
 * Global function definitions
 * ---------------------------------------------------------------------------
 */

//******************************************************************************
/**
     \brief Returns the minimum number of bytes needed to reserve for HL1UART.

     \return      uint32                 The bytes needed to reserve for HL1UART
*/
//******************************************************************************
uint32 DEF(R_Do_HL1UART_GetMemoryNeeded)(void)
{
    return (sizeof(Uarts_t) + CDETECT_MEM() + 3);
}


//******************************************************************************
/**
     \brief Initialize the UART driver. Must be called before any other HL1UART
            functions (if compiled with run-time patching support, then the jump
            table needs to be initialized first.)

            Data_p is a reserved data area free for use by HL1UART. Size is the
            number of bytes available. To get the minimum size of reserved area
            need by HL1UART, call R_Do_HL1UART_GetMemoryNeeded. NOTE: The
            interrupt controller driver HL1INTCON must be initialized before
            this functions is called.

     \param[in]   Data_p                   A reserved word aligned data area
     \param[in]   Size                  The number of bytes available

     \return      boolean                 TRUE if successful init, else FALSE
*/
//******************************************************************************
boolean DEF(R_Do_HL1UART_Init)(void *const Data_p, const uint32 Size)
{
    if ((Size < (sizeof(Uarts_t) + CDETECT_MEM() + 3)) ||
            (((uint32)Data_p & 3) != 0)) {
        return FALSE;
    }

    Uarts_p = (Uarts_t *)Data_p;
    memset(Uarts_p, 0, sizeof(Uarts_t));

    return TRUE;
}


//******************************************************************************
/**
     \brief Re-initialize the UART driver. Use this function if a new instance
            of HL1UART firmware is loaded to update interrupt vectors and data
            pointers. NOTE: The interrupt controller driver HL1INTCON must be
            initialized before this functions is called.

     \param[in]   Data_p                   A reserved word aligned data area
     \param[in]   Size                  The number of bytes available

     \return      boolean                 TRUE if successful init, else FALSE
*/
//******************************************************************************
boolean DEF(R_Do_HL1UART_ReInit)(void *const Data_p, const uint32 Size)
{
    int i;
    Uarts_t *Uarts_old_p;

    if ((Size < (sizeof(Uarts_t) + CDETECT_MEM() + 3)) ||
            (((uint32)Data_p & 3) != 0)) {
        return FALSE;
    }

    Uarts_old_p = Uarts_p;
    Uarts_p = (Uarts_t *)Data_p;

    for (i = 0; i < HL1UART_NBR_OF_UARTS; i++) {
        Uarts_p->UartMem[i] = Uarts_old_p->UartMem[i];
    }

    return TRUE;
}


//******************************************************************************
/**
     \brief De-initialize the UART driver.

     \return      boolean                 TRUE if successful deinit, else FALSE
*/
//******************************************************************************
boolean DEF(R_Do_HL1UART_DeInit)(void)
{
    Uarts_p = NULL;

    return TRUE;
}


//******************************************************************************
/**
     \brief Returns the minimum number of bytes needed to reserve for UART
            device UDev.

     \param[in]   UDev       The UART device UDev.
     \return      uint32                 The bytes needed to reserve for the
                                         UART device UDev.
*/
//******************************************************************************
uint32 DEF(R_Do_HL1UART_GetDeviceMemoryNeeded)(const HL1UART_Device_t UDev)
{
    if (!CheckDevice(UDev)) {
        return 0;
    }

    return sizeof(UartMem_t);
}


//******************************************************************************
/**
     \brief Initialize the UART device UDev. Before calling this function, the
            basic data initialization must be performed by calling
            R_Do_HL1UART_Init.

            Data_p is a reserved word aligned data area free for use by the UART
            device. Size is the number of bytes available. To get the minimum
            size of reserved area needed by the UART device call
            R_Do_HL1UART_GetDeviceMemoryNeeded. If Size is greater than the
            minimum required size, all extra bytes are used as an overflow
            buffer. The driver does not use any hardware or software flow
            control mechanisms.

     \param[in]   Data_p                   A reserved word aligned data area
     \param[in]   Size                  The number of bytes available
     \param[in]   UDev        The UART device

     \return      boolean                 TRUE if successful init, else FALSE
*/
//******************************************************************************
boolean DEF(R_Do_HL1UART_DeviceInit)(void *const Data_p, const uint32 Size, const HL1UART_Device_t UDev)
{
    if ((!CheckDevice(UDev)) || (Size < sizeof(UartMem_t)) || (((uint32)Data_p & 3) != 0)
            || (NULL == Uarts_p) || !UART_CheckOkToInit(UDev)) {
        return FALSE;
    }

    UART_InternalDeviceInit(Data_p, Size, UDev);

    /* Disable UART. */
    WaitForSingleObject(UART_Handles_p->UART_EnableInterrupts_h, INFINITE);
    //ResetEvent(UART_Handles_p->UART_EnableInterrupts_h);

    /* Create Fifo object for Rx_p */
    CreateFifoObject(&Fifo[UDev].Rx_p);
    Fifo[UDev].Rx_p->Init(Fifo[UDev].Rx_p, HL1UART_RX_FIFO_LEVEL);

    /* Create Fifo object for Tx_p */
    CreateFifoObject(&Fifo[UDev].Tx_p);
    Fifo[UDev].Tx_p->Init(Fifo[UDev].Tx_p, HL1UART_TX_FIFO_LEVEL);

    /* Enable UART interrupts */
    UART_EnableInterrupts(UDev);

    /* Set the baud rate */

    /* Enable UART */
    SetEvent(UART_Handles_p->UART_EnableInterrupts_h);

    return TRUE;
}


//******************************************************************************
/**
     \brief Re-initialize the UART device UDev. Before calling this function,
            the basic data re-initialization must be performed by calling
            R_Do_HL1UART_ReInit.

            Data_p is a reserved word aligned data area free for use by the UART
            device. Size is the number of bytes available. To get the minimum
            size of reserved area needed by the UART device call
            R_Do_HL1UART_GetDeviceMemoryNeeded. If Size is greater than the
            minimum required size, all extra bytes are used as an overflow
            buffer. The driver does not use any hardware or software flow
            control mechanisms.

     \param[in]   Data_p                   A reserved word aligned data area
     \param[in]   Size                  The number of bytes available
     \param[in]   UDev        The UART device

     \return      boolean                 TRUE if successful init, else FALSE
*/
//******************************************************************************
boolean DEF(R_Do_HL1UART_DeviceReInit)(void *const Data_p, const uint32 Size, const HL1UART_Device_t UDev)
{
    boolean RetVal = FALSE;

    if ((!CheckDevice(UDev)) || (Size < sizeof(UartMem_t)) || (((uint32)Data_p % 4) > 0) ||
            (NULL == Uarts_p) || !UART_CheckOkToInit(UDev)) {
        return FALSE;
    }

    UART_InternalDeviceInit(Data_p, Size, UDev);

    /* Enable UART interrupts */
    UART_EnableInterrupts(UDev);

    return TRUE;
}


//******************************************************************************
/**
     \brief De-initialize the UART device.

     \param[in]   UDev        The UART device to de-initialize

     \return      boolean                 TRUE if successful deinit, else FALSE
*/
//******************************************************************************
boolean DEF(R_Do_HL1UART_DeviceDeInit)(const HL1UART_Device_t UDev)
{
    boolean RetVal = FALSE;

    /* Check that the device is initialized.
     * It is not ok to deinit a device that has not been initialized.
     */
    if (!CheckDevice(UDev) ||
            ((NULL != Uarts_p) && (NULL == Uarts_p->UartMem[UDev]))) {
        return FALSE;
    }

    UART_DisableInterrupts(UDev);

    if (NULL != Uarts_p) {
        Uarts_p->UartMem[UDev] = NULL;
    }

    return TRUE;
}


//******************************************************************************
/**
     \brief Sets a new baudrate on a UART device.

     \param[in]   Baudrate      The new badrate
     \param[in]   UDev        The UART device

     \return      boolean                 Returns TRUE if success, or FALSE if
                                          wrong device or if there is an ongoing
                                          transfer on the UART.
*/
//******************************************************************************
boolean DEF(R_Do_HL1UART_SetBaudrate)(const HL1UART_Baudrate_t Baudrate, const HL1UART_Device_t UDev)
{
    if ((!CheckDevice(UDev)) ||
            Uarts_p->UartMem[UDev]->RxMgmt.BytesToReceive ||
            Uarts_p->UartMem[UDev]->RxMgmt.BytesOverflow  ||
            Uarts_p->UartMem[UDev]->TxMgmt.BytesToSend) {
        return FALSE;
    }

    /* Disable uart. */
    //UART_WR_CR_DISABLE(Offset(UDev));

    /* Set the baud rate. */
    //UART_WR_SET_BAUD_RATE(Offset(UDev), UART_BRSettings[Baudrate]);

    /* Enable uart. */
    //UART_WR_CR_ENABLE(Offset(UDev));

    return TRUE;
}


//******************************************************************************
/**
     \brief Returns if cable is inserted or removed for a UART device.

     \param[in]   UDev        The UART device

     \return      boolean                 Returns TRUE if cable inserted is
                                          detected, FALSE otherwise.
*/
//******************************************************************************
boolean DEF(R_Do_HL1UART_GetCableDetectStatus)(const HL1UART_Device_t UDev)
{
    if (!CheckDevice(UDev)) {
        return FALSE;
    }

    return TRUE;
}


//******************************************************************************
/**
     \brief Retrieves specified amount of data over the UART device. Returns
            when all data has been received.

     \param[in]   Data_p                  Where received data is placed
     \param[in]   NrBytes                  Number of bytes to receive
     \param[in]   UDev        The UART device


     \return      boolean                 TRUE if successful reception,
                                          else FALSE if an error occurred.
*/
//******************************************************************************
boolean DEF(R_Do_HL1UART_GetData)(uint8 *const Data_p, const uint32 NrBytes,  const HL1UART_Device_t UDev)
{
    boolean RetVal;

    RetVal = R_Do_HL1UART_GetDataNoWait(Data_p, NrBytes, UDev);

    if (RetVal) {
        while (R_Do_HL1UART_CheckRxBytesLeft(UDev)) {
            ;
        }
    }

    return RetVal;
}


//******************************************************************************
/**
     \brief Retrieves specified amount of data over the UART device. Function
            returns immediately. The function R_Do_HL1UART_CheckRxBytesLeft
            returns 0 when all data has been received.

     \param[in]   Data_p                  Where received data is placed
     \param[in]   NrBytes                  Number of bytes to receive
     \param[in]   UDev        The UART device


     \return      boolean                 TRUE if ready to receive,
                                          else FALSE if an error occurred.
*/
//******************************************************************************
boolean DEF(R_Do_HL1UART_GetDataNoWait)(uint8 *const Data_p, const uint32 NrBytes, const HL1UART_Device_t UDev)
{
    if (!CheckDevice(UDev) || (NULL == Data_p) || (Uarts_p->UartMem[UDev]->RxMgmt.BytesToReceive != 0)) {
        return FALSE;
    }

    WaitForSingleObject(UART_Handles_p->UART_EnableInterrupts_h, INFINITE);
    //ResetEvent(UART_Handles_p->UART_EnableInterrupts_h);

    Uarts_p->UartMem[UDev]->RxMgmt.Data_p = Data_p;
    Uarts_p->UartMem[UDev]->RxMgmt.BytesToReceive = NrBytes;
    Uarts_p->UartMem[UDev]->RxMgmt.BytesReceived = 0;

    if (Uarts_p->UartMem[UDev]->RxMgmt.Callback.CallbackFunc != NULL) {
        Uarts_p->UartMem[UDev]->RxMgmt.Callback.Data_p  = Data_p;
        Uarts_p->UartMem[UDev]->RxMgmt.Callback.NrBytes = NrBytes;
    }

    UART_InternalGetData(UDev, &(Uarts_p->UartMem[UDev]->RxMgmt));

    /* Fourth - If there are more bytes to receive enable RX FIFO Timeout interrupt. */
    if (Uarts_p->UartMem[UDev]->RxMgmt.BytesToReceive) {
        InterruptMask |= UART_IMSC_RT_ENABLE;
    }

    RestoreInterrupts(InterruptMask);
    return TRUE;
}


//******************************************************************************
/**
     \brief Stops the receiving started by R_Do_HL1UART_GetDataNoWait.

     \param[in]   UDev        The UART device

     \return      boolean                 TRUE if successful stop, else FALSE
*/
//******************************************************************************
boolean DEF(R_Do_HL1UART_CancelGetDataNoWait)(const HL1UART_Device_t UDev)
{
    return R_Do_HL1UART_FlushRxFifo(UDev);
}

//******************************************************************************
/**
     \brief Returns number of bytes left for current receive operation
            (started by R_Do_HL1UART_GetDataNoWait). When receive operation
            is finished, this function will return 0.

     \param[in]   UDev        The UART device

     \return      uint32                 Number of bytes left
*/
//******************************************************************************
uint32 DEF(R_Do_HL1UART_CheckRxBytesLeft)(const HL1UART_Device_t UDev)
{
    uint32 RetVal = 0;

    if (!CheckDevice(UDev)) {
        return 0xFFFF;
    }

    /* If the UART reveive FIFO contains characters then UART_InternalGetData is
       called to possibly empty the FIFO. */

    if (!Fifo[UDev].Rx_p->IsEmpty(Fifo[UDev].Rx_p)) {
        WaitForSingleObject(UART_Handles_p->UART_EnableInterrupts_h, INFINITE);
        //ResetEvent(UART_Handles_p->UART_EnableInterrupts_h);

        UART_InternalGetData(UDev, &(Uarts_p->UartMem[UDev]->RxMgmt));

        /* Disable the receive FIFO timeout interrupt when the there are no
         * bytes to recieve and either there is no overflow buffer or the
         * overflow buffer is full.
         *
         * If the recieve FIFO timeout interrupt is not disabled there
         * would be a timeout interrupt every 2^32 * BAUD_RATE^-1 second. */
        if ((0 == Uarts_p->UartMem[UDev]->RxMgmt.BytesToReceive) &&
                (Uarts_p->UartMem[UDev]->RxMgmt.BytesOverflow ==
                 Uarts_p->UartMem[UDev]->RxMgmt.OverflowSize)) {
            /* Disable receive FIFO timeout interrupt. */
            InterruptMask &= ~UART_IMSC_RT_ENABLE;
        }

        /* Restore the UART interrupts. */
        RestoreInterrupts(InterruptMask);
    }

    RetVal = Uarts_p->UartMem[UDev]->RxMgmt.BytesToReceive;

    return RetVal;
}


//******************************************************************************
/**
     \brief Clears the RX FIFO.

     \param[in]   UDev        The UART device

     \return      boolean                 TRUE if success, else FALSE
*/
//******************************************************************************
boolean DEF(R_Do_HL1UART_FlushRxFifo)(const HL1UART_Device_t UDev)
{

    if (!CheckDevice(UDev)) {
        return FALSE;
    }

    /* Disable all UART interrupts. */
    WaitForSingleObject(UART_Handles_p->UART_EnableInterrupts_h, INFINITE);
    //ResetEvent(UART_Handles_p->UART_EnableInterrupts_h);

    Fifo[UDev].Rx_p->Flush(Fifo[UDev].Rx_p);

    /* Empty overflow buffer. */
    Uarts_p->UartMem[UDev]->RxMgmt.OverflowReadCntr  = 0;
    Uarts_p->UartMem[UDev]->RxMgmt.OverflowWriteCntr = 0;
    Uarts_p->UartMem[UDev]->RxMgmt.BytesOverflow     = 0;
    Uarts_p->UartMem[UDev]->RxMgmt.BytesToReceive    = 0;
    Uarts_p->UartMem[UDev]->RxMgmt.BytesReceived     = 0;

    if (0 == Uarts_p->UartMem[UDev]->RxMgmt.OverflowSize) {
        /* Disable receive FIFO timeout interrupt. */
        InterruptMask &= ~UART_IMSC_RT_ENABLE;
    }

    /* Restore the UART interrupts. */
    RestoreInterrupts(InterruptMask);

    return TRUE;
}


//******************************************************************************
/**
     \brief Sends specified amount of data over the UART device.
            Returns when all data has been sent.

     \param[in]   Data_p              Data to be transferred
     \param[in]   NrBytes                  Number of bytes to transfer
     \param[in]   UDev        The UART device

     \return      boolean                 TRUE if successful transmit,
                                          else FALSE if an error occurred
*/
//******************************************************************************
boolean DEF(R_Do_HL1UART_PutData)(const uint8 *const Data_p, const uint32 NrBytes, const HL1UART_Device_t UDev)
{

    if (!CheckDevice(UDev) || (NULL == Data_p) || (Uarts_p->UartMem[UDev]->TxMgmt.BytesToSend != 0)) {
        return FALSE;
    }

    if (!Fifo[UDev].Tx_p->IsEmpty(Fifo[UDev].Tx_p)) {
        A_(printf("hl1uart.c (%d): Error Fifo must be empty something is wrong\n", __LINE__);)
        A_(printf("hl1uart.c (%d): Flushing Fifo in order to continue\n", __LINE__);)
        Fifo[UDev].Tx_p->Flush(Fifo[UDev].Tx_p);
    }

    /* Disable all UART interrupts */
    WaitForSingleObject(UART_Handles_p->UART_EnableInterrupts_h, INFINITE);
    //ResetEvent(UART_Handles_p->UART_EnableInterrupts_h);

    Uarts_p->UartMem[UDev]->TxMgmt.Data_p = Data_p;
    Uarts_p->UartMem[UDev]->TxMgmt.BytesToSend = NrBytes;
    Uarts_p->UartMem[UDev]->TxMgmt.BytesSent = 0;

    /* Fill up the UART transmit FIFO. */
    while (Uarts_p->UartMem[UDev]->TxMgmt.BytesToSend) {
        /* Wait for free space in UART transmit FIFO. */
        while (Fifo[UDev].Tx_p->IsFull(Fifo[UDev].Tx_p)) {
            ;
        }

        Fifo[UDev].Tx_p->Push(Fifo[UDev].Tx_p, (const uint8 *)&Uarts_p->UartMem[UDev]->TxMgmt.Data_p[Uarts_p->UartMem[UDev]->TxMgmt.BytesSent], 1);
        Uarts_p->UartMem[UDev]->TxMgmt.BytesSent++;
        Uarts_p->UartMem[UDev]->TxMgmt.BytesToSend--;
    }

    /* Wait until all bytes have been sent */
    while (!Fifo[UDev].Tx_p->IsEmpty(Fifo[UDev].Tx_p)) {
        ;
    }

    /* Restore UART interrupts */
    RestoreInterrupts(InterruptMask);

    return TRUE;
}


//******************************************************************************
/**
     \brief Sends specified amount of data over the UART device. Returns
            immediately. The function R_Do_HL1UART_CheckTxBytesLeft returns 0
            when all data has been sent.

     \param[in]   Data_p              Data to be transferred
     \param[in]   NrBytes                  Number of bytes to transfer
     \param[in]   UDev        The UART device

     \return      boolean                 TRUE if ready to transmit,
                                          else FALSE if an error occurred
*/
//******************************************************************************
boolean DEF(R_Do_HL1UART_PutDataNoWait)(const uint8 *const Data_p, const uint32 NrBytes, const HL1UART_Device_t UDev)
{
    if (!CheckDevice(UDev) || (NULL == Data_p) || (Uarts_p->UartMem[UDev]->TxMgmt.BytesToSend != 0)) {
        return FALSE;
    }

    Uarts_p->UartMem[UDev]->TxMgmt.Data_p = Data_p;
    Uarts_p->UartMem[UDev]->TxMgmt.BytesToSend = NrBytes;
    Uarts_p->UartMem[UDev]->TxMgmt.BytesSent = 0;

    while (0 != UARTBytesToSend) {
        //Waiting UARTListener to send the bytes
        //Going to sleep to let UARTListener to finish it's job faster
#ifdef WIN32
        Sleep(1);
#endif //WIN32
    }

    UARTBytesToSend = NrBytes;

    if (Uarts_p->UartMem[UDev]->TxMgmt.Callback.CallbackFunc != NULL) {
        Uarts_p->UartMem[UDev]->TxMgmt.Callback.Data_p  = Data_p;
        Uarts_p->UartMem[UDev]->TxMgmt.Callback.NrBytes = NrBytes;
    }

    /* Fill up the UART transmit FIFO. */

    while ((!Fifo[UDev].Tx_p->IsFull(Fifo[UDev].Tx_p)) && Uarts_p->UartMem[UDev]->TxMgmt.BytesToSend) {
        Fifo[UDev].Tx_p->Push(Fifo[UDev].Tx_p, (const uint8 *)&Uarts_p->UartMem[UDev]->TxMgmt.Data_p[Uarts_p->UartMem[UDev]->TxMgmt.BytesSent], 1);
        Uarts_p->UartMem[UDev]->TxMgmt.BytesSent++;
        Uarts_p->UartMem[UDev]->TxMgmt.BytesToSend--;
    }

    /* Check if there are more bytes to send. */
    if (Uarts_p->UartMem[UDev]->TxMgmt.BytesToSend) {
        InterruptMask |= UART_IMSC_TX_ENABLE;
        SetEvent(UART_Handles_p->UART_Interrupts_h[TX_INTR_INDEX]);
    } else {
        /* Current receive operation is finished. */
        if (Uarts_p->UartMem[UDev]->TxMgmt.Callback.CallbackFunc != NULL) {
            Uarts_p->UartMem[UDev]->TxMgmt.Callback.CallbackFunc(Uarts_p->UartMem[UDev]->TxMgmt.Callback.Data_p,
                    Uarts_p->UartMem[UDev]->TxMgmt.Callback.NrBytes,
                    UDev);
        }
    }

    return TRUE;
}


//******************************************************************************
/**
     \brief Stops the transmit started by R_Do_HL1UART_PutDataNoWait.

     \param[in]   UDev        The UART device

     \return      boolean                 TRUE if successful stop, else FALSE
*/
//******************************************************************************
boolean DEF(R_Do_HL1UART_CancelPutDataNoWait)(const HL1UART_Device_t UDev)
{
    if (!CheckDevice(UDev)) {
        return FALSE;
    }

    /* Disable all UART interrupts. */
    WaitForSingleObject(UART_Handles_p->UART_EnableInterrupts_h, INFINITE);
    //ResetEvent(UART_Handles_p->UART_EnableInterrupts_h);

    Uarts_p->UartMem[UDev]->TxMgmt.BytesToSend = 0;
    Uarts_p->UartMem[UDev]->TxMgmt.BytesSent   = 0;

    /* Disable transmit FIFO level interrupts. */
    InterruptMask &= ~UART_IMSC_TX_ENABLE;

    Fifo[UDev].Tx_p->Flush(Fifo[UDev].Tx_p);
    UARTBytesToSend = 0;

    /* Restore UART interrupts. */
    RestoreInterrupts(InterruptMask);
    return TRUE;
}


//******************************************************************************
/**
     \brief Returns number of bytes left for current transmit operation
            (started by R_Do_HL1UART_PutDataNoWait). When transmit operation is
            finished, this function will return 0.

     \param[in]   UDev        The UART device

     \return      uint32                  Number of bytes left
*/
//******************************************************************************
uint32 DEF(R_Do_HL1UART_CheckTxBytesLeft)(const HL1UART_Device_t UDev)
{
    uint32 RetVal = 0;

    if (!CheckDevice(UDev)) {
        return 0xFFFF;
    }

    RetVal = Uarts_p->UartMem[UDev]->TxMgmt.BytesToSend;

    return RetVal;
}


//******************************************************************************
/**
     \brief Registers callback functions for data driven use of the non-blocking
            calls (R_Do_HL1UART_GetDataNoWait and R_Do_HL1UART_PutDataNoWait)
            instead of a polling procedure.

            The callback functions have the same arguments as supplied to the
            non-blocking call. NULL is a valid pointer and disables the
            callback. Note that the callback is called from the interrupt
            handler when BytesLeft becomes zero.

     \param[in]   IrqCallbacks_p  The Rx and Tx Irq callback functions.
     \param[in]   UDev         The UART device

     \return      boolean                  TRUE if successful registration,
                                           else FALSE if an error occurred
*/
//******************************************************************************
boolean DEF(R_Do_HL1UART_RegisterIrqCallbacks)(const HL1UART_IrqCallbacks_t*
        const IrqCallbacks_p,
        const HL1UART_Device_t UDev)
{

    Uarts_p->UartMem[UDev]->RxMgmt.Callback.CallbackFunc = IrqCallbacks_p->RxCallback;
    Uarts_p->UartMem[UDev]->RxMgmt.Callback.Data_p       = NULL;
    Uarts_p->UartMem[UDev]->RxMgmt.Callback.NrBytes      = 0;

    Uarts_p->UartMem[UDev]->TxMgmt.Callback.CallbackFunc = IrqCallbacks_p->TxCallback;
    Uarts_p->UartMem[UDev]->TxMgmt.Callback.Data_p       = NULL;
    Uarts_p->UartMem[UDev]->TxMgmt.Callback.NrBytes      = 0;

    return TRUE;
}




/* ---------------------------------------------------------------------------
 * Local function definitions
 * ---------------------------------------------------------------------------
 */

/* ---------------------------------------------------------------------------
 *        Name: CheckDevice
 *  Parameters: const HL1UART_Device_t UDev
 *     Returns: boolean
 * Description: Returns TRUE if device exists, FALSE otherwise.
 * ---------------------------------------------------------------------------
 */
static boolean CheckDevice(const HL1UART_Device_t UDev)
{
    return (UDev < HL1UART_NBR_OF_UARTS);
}

/* ---------------------------------------------------------------------------
 *        Name: UART_IRQ_Handler_n
 *  Parameters: none
 *     Returns: none
 * Description: UART interrupt handlers for each device.
 * ---------------------------------------------------------------------------
 */
static void UART_IRQ_Handler_0(void)
{
    UART_IRQ_Handler(HL1UART_DEVICE_0);
}

static void UART_IRQ_Handler_1(void)
{
    UART_IRQ_Handler(HL1UART_DEVICE_1);
}

static void UART_IRQ_Handler_2(void)
{
    UART_IRQ_Handler(HL1UART_DEVICE_2);
}

static void UART_IRQ_Handler_3(void)
{
    UART_IRQ_Handler(HL1UART_DEVICE_3);
}

static void UART_IRQ_Handler_4(void)
{
    UART_IRQ_Handler(HL1UART_DEVICE_4);
}


/* ---------------------------------------------------------------------------
 *        Name: UART_IRQ_Handler
 *  Parameters: HL1UART_Device_t UDev
 *     Returns: none
 * Description: Common UART interrupt handler.
 * ---------------------------------------------------------------------------
 */
void UART_IRQ_Handler(const HL1UART_Device_t UDev)
{
    volatile uint16 Status;

    /* Get the interrupt status. */
    Status = UART_EventStatus;

    /* Disable all UART interrupts. */
    WaitForSingleObject(UART_Handles_p->UART_EnableInterrupts_h, INFINITE);
    //ResetEvent(UART_Handles_p->UART_EnableInterrupts_h);

    /* Handle Receive FIFO Level interrupts, Receive FIFO Timeout
       interrupts and Receive FIFO Overrun Error interrupts. */
    if ((Status & UART_MIS_RX_IND) ||
            (Status & UART_MIS_RT_IND) ||
            (Status & UART_MIS_OE_IND)) {
        UART_InternalGetData(UDev, &(Uarts_p->UartMem[UDev]->RxMgmt));

        /* Fourth case - there is data still left in the UART receive
         * FIFO but there is no overflow error interrupt. */
        if ((!Fifo[UDev].Rx_p->IsEmpty(Fifo[UDev].Rx_p)) && (Status & UART_MIS_RX_IND) && !(Status & UART_MIS_OE_IND)) {
            /* Clear RX FIFO Level Interrupt. */
            ResetEvent(UART_Handles_p->UART_Events_h[RX_EVENT_INDEX]);
        }

        /* Fifth case - the UART receive FIFO has overflowed (overrun error
          * interrupt. */
        if (Status & UART_MIS_OE_IND) {
            /* Received data is lost when there is an overrun error
             * and the overflow buffer is full. */
            while (!Fifo[UDev].Rx_p->IsEmpty(Fifo[UDev].Rx_p)) {
                uint8 temp;
                /* Throw away the bytes in the UART recieve FIFO. */
                Fifo[UDev].Rx_p->Pop(Fifo[UDev].Rx_p, &temp, 1);
            }

            /* Clear the overrun error interrupt. */
            ResetEvent(UART_Handles_p->UART_Events_h[OE_EVENT_INDEX]);

            /* Put the receive buffer in a consistent state to prevent lock-ups. */
            Uarts_p->UartMem[UDev]->RxMgmt.OverflowReadCntr = 0;
            Uarts_p->UartMem[UDev]->RxMgmt.OverflowWriteCntr = 0;
            Uarts_p->UartMem[UDev]->RxMgmt.BytesOverflow = 0;
            Uarts_p->UartMem[UDev]->RxMgmt.BytesToReceive = 0;
        }


        /* Disable the receive FIFO timeout interrupt when the there are no
         * bytes to recieve and either there is no overflow buffer or the
         * overflow buffer is full.
         *
         * If the recieve FIFO timeout interrupt is not disabled there
         * would be a timeout interrupt every 2^32 * BAUD_RATE^-1 second. */
        if ((0 == Uarts_p->UartMem[UDev]->RxMgmt.BytesToReceive) &&
                (Uarts_p->UartMem[UDev]->RxMgmt.BytesOverflow ==
                 Uarts_p->UartMem[UDev]->RxMgmt.OverflowSize)) {
            /* Disable receive FIFO timeout interrupt. */
            InterruptMask &= ~UART_IMSC_RT_ENABLE;
        }
    } /*  if ((Status & UART_UARTMIS_RXMIS) || (Status & UART_UARTMIS_RTMIS) || (Status & UART_UARTMIS_OEMIS))*/



    /* Handle Transmit FIFO Level interrupts */
    if (Status & UART_MIS_TX_IND) {
        /* Fill up the transmit FIFO. */
        while ((!Fifo[UDev].Tx_p->IsFull(Fifo[UDev].Tx_p)) && Uarts_p->UartMem[UDev]->TxMgmt.BytesToSend) {
            Fifo[UDev].Tx_p->Push(Fifo[UDev].Tx_p, &Uarts_p->UartMem[UDev]->TxMgmt.Data_p[Uarts_p->UartMem[UDev]->TxMgmt.BytesSent], 1);
            Uarts_p->UartMem[UDev]->TxMgmt.BytesSent++;
            Uarts_p->UartMem[UDev]->TxMgmt.BytesToSend--;
        }

        /* If all bytes are sent - disable the transmit FIFO level
         * interrupt. */
        if (Uarts_p->UartMem[UDev]->TxMgmt.BytesToSend == 0) {
            InterruptMask = InterruptMask & ~UART_IMSC_TX_ENABLE;

            /* Call callback */
            if (Uarts_p->UartMem[UDev]->TxMgmt.Callback.CallbackFunc != NULL) {
                Uarts_p->UartMem[UDev]->TxMgmt.Callback.CallbackFunc(Uarts_p->UartMem[UDev]->TxMgmt.Callback.Data_p,
                        Uarts_p->UartMem[UDev]->TxMgmt.Callback.NrBytes,
                        UDev);
            }
        } else {
            /* Clear the transmit FIFO level interrupt. */
            ResetEvent(UART_Handles_p->UART_Events_h[TX_EVENT_INDEX]);
        }
    }

    /* Restore the UART interrupts. */
    RestoreInterrupts(InterruptMask);
}

/* ---------------------------------------------------------------------------
 *        Name: UART_EnableInterrupts
 *  Parameters: const HL1UART_Device_t UDev
 *     Returns: boolean
 * Description: Enables the UART interrupts.
 * ---------------------------------------------------------------------------
 */
static boolean UART_EnableInterrupts(const HL1UART_Device_t UDev)
{
    /* Enable UART recieve FIFO level interrupt. */
    InterruptMask |= UART_IMSC_RX_ENABLE;
    SetEvent(UART_Handles_p->UART_Interrupts_h[RX_INTR_INDEX]);

    if (Uarts_p->UartMem[UDev]->RxMgmt.OverflowSize > 0) {
        /* Enable UART receive FIFO timeout interrupt. */
        InterruptMask |= UART_IMSC_RT_ENABLE;
        SetEvent(UART_Handles_p->UART_Interrupts_h[RT_INTR_INDEX]);
    }

    /* Enable UART receive FIFO overrun interrupt. */
    InterruptMask |= UART_IMSC_OE_ENABLE;
    SetEvent(UART_Handles_p->UART_Interrupts_h[OE_INTR_INDEX]);

    return TRUE;
}

/* ---------------------------------------------------------------------------
 *        Name: UART_DisableInterrupts
 *  Parameters: const HL1UART_Device_t UDev
 *     Returns: boolean
 * Description: Disables the UART interrupts.
 * ---------------------------------------------------------------------------
 */
static boolean UART_DisableInterrupts(const HL1UART_Device_t UDev)
{
    if (NULL != Uarts_p) {
        if (NULL != Uarts_p->UartMem[UDev]) {
            /* Disable UART transmit FIFO level interrupt. */
            InterruptMask &= ~UART_IMSC_TX_ENABLE;
            ResetEvent(UART_Handles_p->UART_Interrupts_h[TX_INTR_INDEX]);

            /* Disable UART recieve FIFO level interrupt. */
            InterruptMask &= ~UART_IMSC_RX_ENABLE;
            ResetEvent(UART_Handles_p->UART_Interrupts_h[RX_INTR_INDEX]);

            /* Disable UART receive FIFO timeout interrupt. */
            InterruptMask &= ~UART_IMSC_RT_ENABLE;
            ResetEvent(UART_Handles_p->UART_Interrupts_h[RT_INTR_INDEX]);

            /* Disable UART receive FIFO overrun error interrupt. */
            InterruptMask &= ~UART_IMSC_OE_ENABLE;
            ResetEvent(UART_Handles_p->UART_Interrupts_h[OE_INTR_INDEX]);
        }
    }

    return TRUE;
}


/* ---------------------------------------------------------------------------
 *        Name:  UART_InternalGetData
 *  Parameters: const HL1UART_Device_t UDev
 *     Returns: none
 * Description: Internal function that is called from either
 *              R_Do_HL1UART_GetDataNoWait or UART_IRQ_Handler. The
 *              function reads characters from the UART receive FIFO
 *              and stores the characters in the overflow buffer or in
 *              data_p.
 * ---------------------------------------------------------------------------
 */
static void UART_InternalGetData(const HL1UART_Device_t UDev, RxBufferMgmt_t *const RxMgmt)
{
    uint32 BytesToCopy;

    if (RxMgmt->BytesOverflow > RxMgmt->BytesToReceive) {
        BytesToCopy = RxMgmt->BytesToReceive;
    } else {
        BytesToCopy = RxMgmt->BytesOverflow;
    }

    /* Handle the case where the OverflowReadCntr wraps around. */
    if (BytesToCopy && (RxMgmt->OverflowReadCntr + BytesToCopy > RxMgmt->OverflowSize)) {
        BytesToCopy = RxMgmt->OverflowSize - RxMgmt->OverflowReadCntr;
        memcpy((uint8 *) & (RxMgmt->Data_p[RxMgmt->BytesReceived]),
               (uint8 *) & (RxMgmt->Overflow_p[RxMgmt->OverflowReadCntr]), BytesToCopy);

        /* Update the number of bytes we have received. */
        RxMgmt->BytesReceived += BytesToCopy;

        /* Wrap around read counter. */
        RxMgmt->OverflowReadCntr = 0;

        /* Update the number of bytes in the overflow buffer. */
        RxMgmt->BytesOverflow -= BytesToCopy;

        /* Update the number of bytes left to receive. */
        RxMgmt->BytesToReceive -= BytesToCopy;

        //BytesToCopy = RxMgmt->BytesToReceive;

        if (RxMgmt->BytesOverflow > RxMgmt->BytesToReceive) {
            BytesToCopy = RxMgmt->BytesToReceive;
        } else {
            BytesToCopy = RxMgmt->BytesOverflow;
        }
    }

    /* Then handle the rest of the bytes. */
    if (BytesToCopy) {
        memcpy((uint8 *) & (RxMgmt->Data_p[RxMgmt->BytesReceived]),
               (uint8 *) & (RxMgmt->Overflow_p[RxMgmt->OverflowReadCntr]), BytesToCopy);

        /* Update the number of bytes we have received. */
        RxMgmt->BytesReceived += BytesToCopy;

        RxMgmt->OverflowReadCntr += BytesToCopy;

        /* Update the number of bytes in the overflow buffer. */
        RxMgmt->BytesOverflow -= BytesToCopy;

        /* Update the number of bytes left to receive. */
        RxMgmt->BytesToReceive -= BytesToCopy;
    }

    /* Second -  Empty the UART Receive FIFO and put the received data in Data_p. */
    while ((!Fifo[UDev].Rx_p->IsEmpty(Fifo[UDev].Rx_p)) && RxMgmt->BytesToReceive) {
        Fifo[UDev].Rx_p->Pop(Fifo[UDev].Rx_p, &RxMgmt->Data_p[RxMgmt->BytesReceived], 1);
        RxMgmt->BytesReceived++;
        RxMgmt->BytesToReceive--;
    }

    /* Third - Empty the UART Receive FIFO and put the received data in
    * the overflow buffer. */
    while (!Fifo[UDev].Rx_p->IsEmpty(Fifo[UDev].Rx_p) &&
            RxMgmt->BytesToReceive == 0 &&
            (RxMgmt->BytesOverflow <
             RxMgmt->OverflowSize)) {
        Fifo[UDev].Rx_p->Pop(Fifo[UDev].Rx_p, &RxMgmt->Overflow_p[RxMgmt->OverflowWriteCntr], 1);
        RxMgmt->OverflowWriteCntr++;
        RxMgmt->BytesOverflow++;

        /* Wrap around the write counter. */
        if (RxMgmt->OverflowWriteCntr >= RxMgmt->OverflowSize) {
            RxMgmt->OverflowWriteCntr = 0;
        }
    }

    if (RxMgmt->BytesToReceive == 0) {
        if ((RxMgmt->Callback.CallbackFunc != NULL) && (RxMgmt->Callback.Data_p != NULL)) {
            RxMgmt->Callback.CallbackFunc(RxMgmt->Callback.Data_p, RxMgmt->Callback.NrBytes, UDev);
            RxMgmt->Callback.Data_p = NULL;
        }
    }
}

/* ---------------------------------------------------------------------------
 *        Name: UART_InternalDeviceInit
 *  Parameters: void* const Data_p, const uint32 Size, const HL1UART_Device_t UDev
 *     Returns: none
 * Description: Internal function that is called from either
 *              R_Do_HL1UART_DeviceInit or R_Do_HL1UART_DeviceReInit.
 * ---------------------------------------------------------------------------
 */
static void UART_InternalDeviceInit(void *const Data_p, const uint32 Size, const HL1UART_Device_t UDev)
{
    Uarts_p->UartMem[UDev] = (UartMem_t *)Data_p;

    /**************************************************
     * Wait to see if there is bug in this line.
     **************************************************/
    memset(Uarts_p->UartMem[UDev], 0, sizeof(UartMem_t *));

    Uarts_p->UartMem[UDev]->RxMgmt.OverflowReadCntr  = 0;
    Uarts_p->UartMem[UDev]->RxMgmt.OverflowWriteCntr = 0;
    Uarts_p->UartMem[UDev]->RxMgmt.BytesOverflow     = 0;
    Uarts_p->UartMem[UDev]->RxMgmt.BytesToReceive    = 0;
    Uarts_p->UartMem[UDev]->RxMgmt.BytesReceived     = 0;

    if (Size > sizeof(UartMem_t)) {
        Uarts_p->UartMem[UDev]->RxMgmt.Overflow_p = (uint8 *)Data_p + sizeof(UartMem_t);
        Uarts_p->UartMem[UDev]->RxMgmt.OverflowSize = Size - sizeof(UartMem_t);
    } else {
        Uarts_p->UartMem[UDev]->RxMgmt.Overflow_p = NULL;
        Uarts_p->UartMem[UDev]->RxMgmt.OverflowSize = 0;
    }

    Uarts_p->UartMem[UDev]->TxMgmt.Data_p = NULL;
    Uarts_p->UartMem[UDev]->TxMgmt.BytesToSend = 0;
    Uarts_p->UartMem[UDev]->TxMgmt.BytesSent = 0;
}


/* ---------------------------------------------------------------------------
 *        Name:  UART_CheckOkToInit
 *  Parameters: const HL1UART_Device_t UDev
 *     Returns: boolean
 * Description: Internal function that is called from
 *              R_Do_HL1UART_DeviceInitGetDataNoWait or UART_IRQ_Handler. The
 *              function reads characters from the UART receive FIFO
 *              and stores the characters in the overflow buffer or in
 *              data_p.
 * ---------------------------------------------------------------------------
 */
static boolean UART_CheckOkToInit(const HL1UART_Device_t UDev)
{
    boolean RetVal;

    /* Make sure that conflicting UARTs not are initialized at the same
     * time. */
    if (UDev == HL1UART_DEVICE_0) {
        RetVal = (NULL == Uarts_p->UartMem[HL1UART_DEVICE_5]);
    } else if (UDev == HL1UART_DEVICE_5) {
        RetVal = (NULL == Uarts_p->UartMem[HL1UART_DEVICE_0]);
    } else {
        RetVal = TRUE;
    }

    return RetVal;
}

Uarts_t *GetUarts_p()
{
    return Uarts_p;
}

static void RestoreInterrupts(uint16 Interrupt_Mask)
{
    if (Interrupt_Mask & UART_IMSC_RT_ENABLE) {
        SetEvent(UART_Handles_p->UART_Interrupts_h[RT_INTR_INDEX]);
    } else {
        ResetEvent(UART_Handles_p->UART_Interrupts_h[RT_INTR_INDEX]);
    }

    if (Interrupt_Mask & UART_IMSC_RX_ENABLE) {
        SetEvent(UART_Handles_p->UART_Interrupts_h[RX_INTR_INDEX]);
    } else {
        ResetEvent(UART_Handles_p->UART_Interrupts_h[RX_INTR_INDEX]);
    }

    if (Interrupt_Mask & UART_IMSC_TX_ENABLE) {
        SetEvent(UART_Handles_p->UART_Interrupts_h[TX_INTR_INDEX]);
    } else {
        ResetEvent(UART_Handles_p->UART_Interrupts_h[TX_INTR_INDEX]);
    }

    if (Interrupt_Mask & UART_IMSC_OE_ENABLE) {
        SetEvent(UART_Handles_p->UART_Interrupts_h[OE_INTR_INDEX]);
    } else {
        ResetEvent(UART_Handles_p->UART_Interrupts_h[OE_INTR_INDEX]);
    }

    SetEvent(UART_Handles_p->UART_EnableInterrupts_h);
}
/**     @}*/
/**@}*/