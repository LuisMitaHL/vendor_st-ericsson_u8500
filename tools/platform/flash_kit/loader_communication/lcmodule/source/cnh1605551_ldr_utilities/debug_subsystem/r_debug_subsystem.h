/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
#ifndef _R_DEBUG_SUBSYTEM_H_
#define _R_DEBUG_SUBSYTEM_H_

/**
 *  @addtogroup ldr_utilities
 *  @{
 *    @addtogroup debug_subsystem
 *    @{
 *      This module handles putting the debug messages into debug queue.
 *      @li Inserting messages in debug queue is re-entrant process.
 *      @li Reading from debug queue is performed by call of PC side.
 *      @li This module provide circular storage of printout messages. This
 *          means that in cases of queue overflowing the oldest messages will be
 *          overwritten by the newest messages.
 *      @li For Windows Loader this feature is currently not supported.
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_basicdefinitions.h"
#include "t_debug_subsystem.h"
#ifdef CFG_ENABLE_LOADER_TYPE
#include "error_codes.h"
#include "t_communication_service.h"
#endif

/*******************************************************************************
 * Defines
 ******************************************************************************/

#ifndef CFG_ENABLE_LOADER_TYPE
#define printf(...) lcm_printf(__VA_ARGS__)
#else
/**
 * This define replace printf function with ddebug_printf variadic macro.
 */
#define printf ddebug_printf
#endif /*CFG_ENABLE_LOADER_TYPE*/

/**
 * Function-like macro. This macro will write messages in debug queue. Also this
 * macro take care about initialization of debug queue.
 */
#define ddebug_printf(...) debug_printf(__VA_ARGS__)

/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/
#ifndef CFG_ENABLE_LOADER_TYPE
void lcm_printf(const char *format, ...);
#else
/*
 * Initialization of Debug communication device.
 *
 * Used communication devices UART (specified with UART_DEBUG_PORT).
 *
 * @return none.
 */
void Do_Loader_DebugoutInit(void);

/*
 * Function for getting the debug communication device.
 *
 * @param [out] DebugDevice_pp    Pointer to the initialized debug communication
 *                                device.
 *
 * @retval E_SUCCESS              After successful execution.
 * @retval E_GENERAL_FATAL_ERROR  If debug communication device is not
 *                                initialized.
 */
ErrorCode_e Do_GetDebugCommunicationDevice(CommunicationDevice_t **DebugDevice_pp);

/**
 * Calls function 'circular_printf'. If debug queue is not initialized this
 * function takes care for initialization and than redirect input parameters to
 * 'circular_printf' function.
 *
 * @param [in] *format The string constant format provides a description of the
 *                     output, with place holders marked by "%" escape
 *                     characters, to specify both the relative location and the
 *                     type of output that the function should produce.(Standard
 *                     definition for library 'printf' function).
 * @return             None.
 */
void debug_printf(const char *format, ...);

/**
 * This function collect all messages from debug queue and copy them inside one
 * memory space (buffer). First printout message in resulting buffer will be
 * first printout that is accepted and last message in buffer is last message
 * received by the time that this function is called.
 *
 * @param[in, out]  Data_pp Point to pointer that should be initialized to point
 *                          on outgoing buffer.
 *
 * @return                 Number of characters successfully readied from debug
 *                         queue.
 */
uint32 Do_ReadDebugQueue(void **Data_pp);

/**
 * This function destroys debug queue including all memory spaces allocated for
 * debug messages(printouts).
 *
 * @param[in]  Queue_pp Pointer to the debug queue structure to destroy.
 * @return none.
 */
void Do_DestroyDebugQueue(void **const Queue_pp);
#endif /*CFG_ENABLE_LOADER_TYPE*/

/** @} */
/** @} */
#endif /*_R_DEBUG_SUBSYTEM_H_*/
