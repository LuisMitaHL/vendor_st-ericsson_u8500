/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
#ifndef _COMMAND_AUDIT_H_
#define _COMMAND_AUDIT_H_

/*
 *\addtogroup ldr_command_exec_service
 * @{
 *  \addtogroup  command_audit
 *
 * The purpose of command auditing is to provide way to check every received
 * command before execution. Commands must be registered from the application
 * layer. The purpose of authentication is ability to perform certain execution
 * of received commands. This module must verify command consistency and
 * command availability to provide valid execution loader functionality as well
 * to improve parallelism.
 *
 *  @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "running_commands.h"

/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/

/*
 * Audit command.
 *
 * Checks scope of command, Checks command availability.
 *
 * @param [in] CmdData_p                  Pointer to command data structure.
 *
 * @retval  E_INVALID_INPUT_PARAMETER     CmdData_p is NULL.
 * @retval  E_OVERLOAD_COMMAND_TABLE      Too many commands registered.
 * @retval  E_COMMAND_ALREADY_REGISTERED  Command was already registered.
 * @retval  E_SUCCESS                     The function completed successfully,
 *                                        that means the command can be run.
 */
//******************************************************************************
ErrorCode_e CES_AuditCommand(const CommandData_t *const CmdData_p);

/* @} */
/* @} */
#endif /*_COMMAND_AUDIT_H_*/
