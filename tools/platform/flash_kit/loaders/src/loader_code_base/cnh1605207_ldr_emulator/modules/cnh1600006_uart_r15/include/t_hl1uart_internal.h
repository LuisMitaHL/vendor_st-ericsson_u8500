#ifndef INCLUSION_GUARD_T_HL1UART_INTERNAL_H_
#define INCLUSION_GUARD_T_HL1UART_INTERNAL_H_
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
#include "c_system_v2.h"
#include "t_basicdefinitions.h"
#include "t_hl1uart.h"
#include "t_uart_fifo.h"
#ifdef WIN32
#include <windows.h>
#endif //WIN32

/* ---------------------------------------------------------------------------
 * Global definitions
 * ---------------------------------------------------------------------------
 */

#ifdef DECLARE_CRITICAL_SECTIONS
CRITICAL_SECTION RxMgmt_cs;
CRITICAL_SECTION TxMgmt_cs;
CRITICAL_SECTION fifo_cs;
CRITICAL_SECTION IrqCallbacks_cs;
#else //DECLARE_CRITICAL_SECTIONS
extern CRITICAL_SECTION RxMgmt_cs;
extern CRITICAL_SECTION TxMgmt_cs;
extern CRITICAL_SECTION fifo_cs;
extern CRITICAL_SECTION IrqCallbacks_cs;
#endif //DECLARE_CRITICAL_SECTIONS

typedef struct {
    Fifo_t *Rx_p;
    Fifo_t *Tx_p;
} Fifo_RxTx;

#ifndef DECLARE_FIFO
extern Fifo_RxTx Fifo[NO_OF_HL1UART_DEVICES];
#else
Fifo_RxTx Fifo[NO_OF_HL1UART_DEVICES] = {
    {NULL, NULL}, {NULL, NULL}, {NULL, NULL},
    {NULL, NULL}, {NULL, NULL}, {NULL, NULL}
};
#endif

#ifdef DECLARE_UART_EVENT_STATUS
volatile uint16 UART_EventStatus;
#else //DECLARE_UART_EVENT_STATUS
extern volatile uint16 UART_EventStatus;
#endif //DECLARE_UART_EVENT_STATUS

/**     @}*/
/**@}*/
#endif //INCLUSION_GUARD_T_HL1UART_INTERNAL_H_
