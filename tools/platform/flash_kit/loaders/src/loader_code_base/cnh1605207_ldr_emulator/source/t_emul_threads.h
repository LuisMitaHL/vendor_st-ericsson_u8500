#ifndef INCLUSION_GUARD_T_EMUL_THREADS_H_
#define INCLUSION_GUARD_T_EMUL_THREADS_H_
/***********************************************************************
 * $Copyright Ericsson AB 2009 $
 **********************************************************************/

/**
 * \addtogroup ldr_emulator
 * @{
 */
/*************************************************************************
* Includes
*************************************************************************/
#ifdef WIN32
#include <windows.h>
#endif //WIN32
#include "t_basicdefinitions.h"
#include "t_hl1uart.h"

/** Number of events defined for the UART */
#define NR_OF_UART_EVENTS      4
/** Number of interrupts defined for the UART */
#define NR_OF_UART_INTERRUPTS  4
/* This is the maximum size for the receive buffer*/
#define MAX_SIZE_OF_RECEIVE_BUFFER  32

/**
 * Structure containing the Number of Events and Interrupts,
 * and array of Events and Interrupts for the UART Device
 */
typedef struct {
    /** Number of UART Events */
    uint32            UART_NrEvents;
    /** Number of UART Interrupts */
    uint32            UART_NrInterrupts;
    /** Array of handles for the UART Events */
    HANDLE            UART_Events_h[NR_OF_UART_EVENTS];
    /** Array of handles for the UART Interrupts */
    HANDLE            UART_Interrupts_h[NR_OF_UART_INTERRUPTS];
    /** Handle for enabling and disabling of the UART Interrupts*/
    HANDLE            UART_EnableInterrupts_h;
    /** Device type, used for determining which device is used*/
    HL1UART_Device_t  UDev;
    /** Shut down the Uart driver */
    boolean                       UART_ShutDown;
} UART_Handles_t;

/**
 * Enumerated type holding indexes of the Events for the UART
 */
typedef enum {
    RX_EVENT_INDEX = 0,
    TX_EVENT_INDEX,
    RT_EVENT_INDEX,
    OE_EVENT_INDEX
} EventIndex_e;

/**
 * Enumerated type holding indexes of the Interrupts for the UART
 */
typedef enum {
    RX_INTR_INDEX = 0,
    TX_INTR_INDEX,
    RT_INTR_INDEX,
    OE_INTR_INDEX
} InterruptIndex_e;

/**
 * Ennumerated type holding the bit positions of the UART Events
 * in the Status register.
 */
typedef enum {
    UART_MIS_RI_EVENT   = 0,
    UART_MIS_CTS_EVENT  = 1,
    UART_MIS_DCD_EVENT  = 2,
    UART_MIS_DSR_EVENT  = 3,
    UART_MIS_RX_EVENT   = 4,
    UART_MIS_TX_EVENT   = 5,
    UART_MIS_RT_EVENT   = 6,
    UART_MIS_FE_EVENT   = 7,
    UART_MIS_PE_EVENT   = 8,
    UART_MIS_BE_EVENT   = 9,
    UART_MIS_OE_EVENT   = 10,
    UART_MIS_XOFF_EVENT = 11,

} UART_Events_e;

/**
 * Structure used for emulation of Timer.
 */
typedef struct {
    /** Handle for the timer Event*/
    HANDLE Timer_Event_h;
    /** Handle for enabling and disabling the timer interrupts*/
    HANDLE Timer_EnableInterrupts_h;
} Timer_Handles_t;

/**
 * Definition of pointer to a structure of type UART_Handles_t.
 * It is used as global variable by all threads in the emulator.
 * Declared in hl1uart.c
 */
#ifdef DECLARE_UART_HANDLES
UART_Handles_t *UART_Handles_p = NULL;
#else //DECLARE_UART_HANDLES
extern UART_Handles_t *UART_Handles_p;
#endif //DECLARE_UART_HANDLES

/**
 * Definition of pointer to a structure of type Timer_Handles_t.
 * It is used as global variable by all threads in the emulator.
 * Declared in timers.c
 */
#ifdef DECLARE_TIMER_HANDLES
Timer_Handles_t *Timer_Handles_p = NULL;
#else //DECLARE_TIMER_HANDLES
extern Timer_Handles_t *Timer_Handles_p;
#endif //DECLARE_TIMER_HANDLES

/*@}*/
#endif /*T_EMUL_THREADS_H_*/
