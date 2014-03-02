#ifndef INCLUSION_GUARD_R_EMUL_THREADS_H_
#define INCLUSION_GUARD_R_EMUL_THREADS_H_
/***********************************************************************
 * $Copyright Ericsson AB 2008 $
 **********************************************************************/

/**
 * \addtogroup ldr_emulator
 * \brief Utility functions for R15 emulator
 * @{
 */

/*************************************************************************
* Includes
*************************************************************************/
#ifdef WIN32
#include <windows.h>
#endif //WIN32
#include "t_emul_threads.h"

/**
 *  Thread for activating the UART IRQ handler.
 *
 *  @param[in]  Param_p is a pointer to a structure conating information about
 *              events.
 *  @return see \c ErrorCode_e for detailed description of the error codes.
 */
DWORD WINAPI UART_IRQ_HandlerThread(LPVOID Param_p);

/**
 *  Thread for activating the Timer Handler.
 *
 *  @param[in]  Param_p is a pointer to a structure conating information about
 *              timer events.
 *  @return see \c ErrorCode_e for detailed description of the error codes.
 */
DWORD WINAPI Do_Timers_HandlerThread(LPVOID Param_p);

/**
 *  This function is started to run as separate thread. It simulates the
 *  work of UART.
 *
 *  @param[in]  Param_p is a pointer to a structure conating information about
 *              UART events.
 *  @return see \c ErrorCode_e for detailed description of the error codes.
 */
DWORD WINAPI UART_Listener(LPVOID Param_p);


/**
 *  This function terminates all created threads to simulate power shut down
 *  on hardware level.
 *  @return none.
 */

void PowerShutDown(void);

/*@}*/
#endif /*INCLUSION_GUARD_R_EMUL_THREADS_H_*/
