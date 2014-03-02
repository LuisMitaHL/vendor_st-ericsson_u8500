/***********************************************************************
 * $Copyright Ericsson AB 2009 $
 **********************************************************************/
/**
 * \addtogroup ldr_emulator
 *  @{
 *      \addtogroup Utility
 *      \brief Utility functions for R15 emulator
 *      @{
 */
#include "c_system.h"
#ifdef WIN32
#include <windows.h>
#endif //WIN32
#include <assert.h>
#include <stdio.h>
#include "error_codes.h"
#include "r_emul_threads.h"
#include "t_hl1uart.h"
#include "uart.h"
#define DECLARE_UART_EVENT_STATUS
#include "t_hl1uart_internal.h"
#include "r_debug.h"
#include "t_r15_network_layer.h"
#include "t_emulator.h"

/* Macro defining the baud rate of the UART*/
#define UART_BAUD_RATE_115200       115200
/*
 * This variable is used to improve the transmit speed of the UART
 * communication. Instead of sending one byte at a time, bytes are
 * first stored in buffer, and after that they are send to the UART.
 * volatile is used because it can be changed in R_Do_HL1UART_PutDataNoWait.
 */
volatile uint32 UARTBytesToSend = 0;

extern void UART_IRQ_Handler(const HL1UART_Device_t UDev);
extern void Do_Timer_TimersHandler(void);

DWORD WINAPI UART_IRQ_HandlerThread(LPVOID Param_p)
{
    DWORD RetVal;
    UART_Handles_t *Wait_p = NULL;

    if (NULL == Param_p) {
        return E_INVALID_INPUT_PARAMETERS;
    }

    while (1) {
        UART_EventStatus = 0;
        Wait_p = (UART_Handles_t *)Param_p;
        RetVal = WaitForMultipleObjects(Wait_p->UART_NrEvents, Wait_p->UART_Events_h, FALSE, INFINITE);

        switch (RetVal) {
        case WAIT_OBJECT_0 + RX_EVENT_INDEX: {
            UART_EventStatus |= (1 << UART_MIS_RX_EVENT);
        }
        break;

        case WAIT_OBJECT_0 + TX_EVENT_INDEX: {
            UART_EventStatus |= (1 << UART_MIS_TX_EVENT);
        }
        break;

        case WAIT_OBJECT_0 + RT_EVENT_INDEX: {
            UART_EventStatus |= (1 << UART_MIS_RT_EVENT);
        }
        break;

        case WAIT_OBJECT_0 + OE_EVENT_INDEX: {
            UART_EventStatus |= (1 << UART_MIS_OE_EVENT);
        }
        break;

        default: {
            UART_EventStatus = 0;
        }
        break;
        }

        if (0 != UART_EventStatus) {
            UART_IRQ_Handler(Wait_p->UDev);
        } else {
            //TO-DO: Create somekind of log or Fifo to hold errors
            //Important: Don't use a file or any other kind of blocking I/O
            //because this is thread with highest priority and should run for very short time
            //and shouldn't be interrupted
            printf("Unknown Event\n");
        }
    }

    return E_SUCCESS;
}

DWORD WINAPI Do_Timers_HandlerThread(LPVOID Param_p)
{
    DWORD RetVal;
    Timer_Handles_t *Wait_p = NULL;

    if (NULL == Param_p) {
        return E_INVALID_INPUT_PARAMETERS;
    }

    while (1) {
        Wait_p = (Timer_Handles_t *)Param_p;

        RetVal = WaitForSingleObject(Wait_p->Timer_Event_h, INFINITE);

        switch (RetVal) {
        case WAIT_OBJECT_0: {
            Do_Timer_TimersHandler();
            //printf("Timer finished\n");
        }
        break;

        default: {
            printf("Unknown Event\n");
        }
        break;
        }
    }

    return E_SUCCESS;
}

DWORD WINAPI UART_Listener(LPVOID Param_p)
{
    HANDLE Comm_h;
    char COM[10];
    FILE *cfPtr;
    uint8 *ReceiveData_p = NULL;
    uint8 *TransmitData_p = NULL;
    UART_Handles_t *UART_p = NULL;
    uint32 i = 0;

    if ((cfPtr = fopen("com_port_settings.txt", "rb+")) == NULL) {
        A_(printf("emul_threads.c(%d) Missing comport settings file!\n", __LINE__);)
        Comm_h = AsyncOpen("COM1");
    } else {
        memset(COM, 0x00, 10);

        while (!feof(cfPtr)) {
            fread(&COM, 5, 1, cfPtr);
        }

        Comm_h = AsyncOpen(COM);
    }

    ReceiveData_p = malloc(MAX_SIZE_OF_RECEIVE_BUFFER);
    assert(NULL != ReceiveData_p);
    TransmitData_p = malloc(MAX_SIZE_OF_TRANSMIT_BUFFER);
    assert(NULL != TransmitData_p);

    UART_p = (UART_Handles_t *) Param_p;

    if (Comm_h == NULL || Comm_h == INVALID_HANDLE_VALUE) {
        printf("emul_threads.c(%d): Error failed to open COM port!\n", __LINE__);
    }

    //  Set comm port
    WinSetPortState(Comm_h, UART_BAUD_RATE_115200, 8, 0, 0);  // 115200 baud rate, 8 bits, NOPARITY, ONESTOPBIT
#ifdef WIN32
    Sleep(1);
#endif //WIN32

    UART_p->UART_ShutDown = FALSE;

    while (!UART_p->UART_ShutDown) {
        int Res = 0;

        Res = AsyncRead(Comm_h, ReceiveData_p, MAX_SIZE_OF_RECEIVE_BUFFER, 100);

        if (Res > 0) {
            uint32 i = 0;

            while (Res > 0) {
                if (Fifo[UART_p->UDev].Rx_p->IsFull(Fifo[UART_p->UDev].Rx_p)) {
                    WaitForSingleObject(UART_p->UART_EnableInterrupts_h, INFINITE);

                    if (WAIT_OBJECT_0 == WaitForSingleObject(UART_p->UART_Interrupts_h[OE_INTR_INDEX], 0)) {
                        SetEvent(UART_p->UART_Events_h[OE_EVENT_INDEX]);
                    }

                    SetEvent(UART_p->UART_EnableInterrupts_h);
                } else {
                    WaitForSingleObject(UART_p->UART_EnableInterrupts_h, INFINITE);
                    Fifo[UART_p->UDev].Rx_p->Push(Fifo[UART_p->UDev].Rx_p, &ReceiveData_p[i], 1);
                    i++;
                    Res--;
                    SetEvent(UART_p->UART_EnableInterrupts_h);
                }

                if ((!Fifo[UART_p->UDev].Rx_p->IsEmpty(Fifo[UART_p->UDev].Rx_p)) &&
                        (!Fifo[UART_p->UDev].Rx_p->IsFull(Fifo[UART_p->UDev].Rx_p))) {
                    WaitForSingleObject(UART_p->UART_EnableInterrupts_h, INFINITE);

                    if (WAIT_OBJECT_0 == WaitForSingleObject(UART_p->UART_Interrupts_h[RX_INTR_INDEX], 0)) {
                        SetEvent(UART_p->UART_Events_h[RX_EVENT_INDEX]);
                    }

                    SetEvent(UART_p->UART_EnableInterrupts_h);
                }
            }
        }

        while ((!Fifo[UART_p->UDev].Tx_p->IsEmpty(Fifo[UART_p->UDev].Tx_p)) && (0 != UARTBytesToSend)) {
            UARTBytesToSend--;
            Fifo[UART_p->UDev].Tx_p->Pop(Fifo[UART_p->UDev].Tx_p, &TransmitData_p[i], 1);
            i++;
        }

        if ((0 == UARTBytesToSend) && (i > 0)) {
            AsyncWrite(Comm_h, TransmitData_p, i, INFINITE);
            i = 0;
        }

        WaitForSingleObject(UART_p->UART_EnableInterrupts_h, INFINITE);

        if (WAIT_OBJECT_0 == WaitForSingleObject(UART_p->UART_Interrupts_h[TX_INTR_INDEX], 0)) {
            SetEvent(UART_p->UART_Events_h[TX_EVENT_INDEX]);
        }

        SetEvent(UART_p->UART_EnableInterrupts_h);
    }

    /* close the COM port */
    WinCloseCommPort(Comm_h);
    return E_SUCCESS;
}

void PowerShutDown(void)
{
    ExitProcess(0);
}

/**      @}*/
/**@}*/
