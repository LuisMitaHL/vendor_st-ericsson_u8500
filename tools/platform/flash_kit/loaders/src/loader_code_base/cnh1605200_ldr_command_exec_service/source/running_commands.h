/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
#ifndef _RUNNING_COMMANDS_H_
#define _RUNNING_COMMANDS_H_

/*
 * \addtogroup ldr_command_exec_service
 * @{
 *  \addtogroup  running_commands
 *  @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "error_codes.h"
#include "t_communication_service.h"

/*******************************************************************************
 * Types, constants
 ******************************************************************************/

/*
 * The Maximum allowed number of commands that can be executed in parallel.
 *
 */
#define MAX_CONCURRENT_COMMANDS 5

/*
 * Function used for adding certain command into the execution queue,
 * i.e. registering the command for execution
 */
ErrorCode_e CES_AddCommand(ExecutionContext_t *ExecContext_p);

/*
 * Function used to de-queue certain command from the
 * execution queue.
 *
 */
ExecutionContext_t *CES_DequeueNextCommandToExecute(void);

/*
 * Method used for initializing the execution context queue
 * prior its usage
 */
void CES_InitializeExecContextQueue(void);

/*
 * Function used to enqueue command in the execution queue
 */
void CES_EnqueueCommandToExecute(ExecutionContext_t *ExecContext_p);

/*
 *
 */
ErrorCode_e CES_InitializeCommandExecutionContext(const CommandData_t *CmdData_p, ExecutionContext_t *ExecContext_p);

/*
 *
 */
boolean CES_IsCommandRegistered(const uint8 CommandNumber, const uint8 ApplicationNumber);

/**
 * Returns the minimal progress status in percent from all
 * currently executing long running
 * commands.
 *
 * @param [in, out] ProgressStat_p   Command progress status in
 *                                   percent.
 *
 * retval E_SUCCESS                  Successful execution.
 * retval E_INVALID_INPUT_PARAMETERS Invalid input parameters.
 * retval E_FAILED_TO_STORE_IN_FIFO  Failed to enque the command.
 */
ErrorCode_e CES_GetProgressStatus(uint32 *ProgressStat_p);

/* @} */
/* @} */
#endif /*_RUNNING_COMMANDS_H_*/
