/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
#ifndef _R_COMMAND_EXEC_SERVICE_H_
#define _R_COMMAND_EXEC_SERVICE_H_

/**
 * \addtogroup ldr_command_exec_service Loader Command Execution Service
 *
 * The purpose of CES is to perform certain execution of received long-time
 * running commands. This module must verify consistency and availability
 * between commands, in order to provide valid execution of loader
 * functionality and improve parallelism. Another purpose of CES is to provide
 * a way to check every received command before its execution.
 *
 * This module is used for execution of long-term commands. The commands must
 * be registered from the application layer. All commands should be grouped by
 * their functionality in command groups or applications. All applications and
 * commands (Command global context CommandInformation_t) are registered on
 * compile time. Command global context holds all information about a command,
 * especially function pointer that points to the executed function.
 *
 * CES module contains command execution queue, used for registering commands
 * that were received in the loader. ExecutionContext_t is the structure
 * required for command storage in the queue. Commands are put and executed
 * from the queue by the FIFO principle. A command has to be coded on the state
 * machine way. Each state is executed sequentially. The execute time for each
 * state must be limited.
 *
 * The Command Execution Control is the central part of CES module. It consists
 * of polling functions that supports loader command manipulating
 * functionalities like verifying, initiating, executing and monitoring. All
 * the functionalities mention above could be implanted as a function
 * positioned in the same or other sub modules in the CES framework. As well,
 * it provides allocation space for the application register structures and
 * command register structures.
 *
 *  @{
 *
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_command_exec_service.h"
#include "t_command_protocol.h"

/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/

/**
 *  Function that manages commands. It starts, switches, and stops
 *  the commands. Before calling this function the CES must be
 *  initialized properly.
 *
 *  @return Nothing
 */
void Do_CES_CommandExecutionPoll(void);

/**
 *  Register the long running command.
 *
 * * @param [in] CmdData_p       Pointer to the command data.
 *
 *  @retval   E_SUCCESS The command registered successfully.
 */
ErrorCode_e Do_CES_RegisterRepeatCommand(CommandData_t *CmdData_p);

/**
 *  Initializes the Comand Execution Service.
 *  Must be called before using any other function from the module.
 *
 *  @return   None.
 */
void Do_CES_Init(void);

/**
 *  Returns number of applications implemented in the current loader.
 *
 *  @retval The number of applications implemented in the current loader.
 */
uint32 Do_CES_ActiveApplications(void);

/**
 *  Register the long running command.
 *
 *  @param [in] CmdData_p       Pointer to the command data.
 *  @param [in] ReturnValue     Return value for error exit.
 *
 */
void Do_CES_SetErrorHandlingData(CommandData_t *CmmData_p, ErrorCode_e ReturnValue);


/** @} */
#endif /*_R_COMMAND_EXEC_SERVICE_H_*/
