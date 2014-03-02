/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
#ifndef _R_MAIN_H_
#define _R_MAIN_H_

/**
 *  @addtogroup ldr_main
 *  @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#ifndef CFG_ENABLE_PROLOGUE
#include "error_codes.h"
#ifdef WIN32
#include <windows.h>
#endif //WIN32
#endif //#ifndef CFG_ENABLE_PROLOGUE
//******************************************************************************
/**
 *  Program execution starts from this function.
 */
//******************************************************************************
//#ifdef WIN32
//int main();
//#endif //WIN32

#ifdef WIN32
DWORD WINAPI Main_Thread(LPVOID lpParam);
#else //WIN32
int main(void);
#endif //WIN32

#ifndef CFG_ENABLE_PROLOGUE
/**
 *  Process handler.
 *
 *  It executes Do_TL_Process and Do_CES_CommandExecutionPoll.
 *
 *  @return none.
 */
void Do_Process(void);

/**
 *  Loader internal error handler.
 *
 *  All the internal errors shall end up here.
 *  The ME must be shut down and ETX will be locked down.
 *
 *  @param [in] IntError This is the internal error code.
 *
 *  @return none.
 */
void Do_LoaderInternalErrorHandler(const ErrorCode_e IntError);

/**
 * The function stalls loader execution.
 * It is used in internal error handling mechanism,
 * and wherever intentional infinite loop is needed
 *
*/
void StallExecution(void);

#endif //#ifndef CFG_ENABLE_PROLOGUE
/** @} */
#endif //_R_MAIN_H_
