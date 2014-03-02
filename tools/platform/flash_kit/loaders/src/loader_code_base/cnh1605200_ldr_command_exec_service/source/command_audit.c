/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/

/*
 * @addtogroup ldr_command_exec_service
 * @{
 *  @addtogroup command_audit
 *  @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdlib.h>

#include "r_debug_macro.h"
#include "command_audit.h"
#include "t_command_exec_service.h"

/*******************************************************************************
 * File scope types, constants and variables
 ******************************************************************************/
extern uint32 NumberOfRunningCmds;

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/
ErrorCode_e CES_AuditCommand(const CommandData_t *const CmdData_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;

    /*
     *  Sanity check of the input command data
     */
    VERIFY(NULL != CmdData_p, E_INVALID_INPUT_PARAMETER);

    /* Checks scope of NrOfRunningCmds */
    if (NumberOfRunningCmds >= MAX_CONCURRENT_COMMANDS) {
        if (NumberOfRunningCmds == MAX_CONCURRENT_COMMANDS) {
            if (CmdData_p->CommandNr != COMMAND_ERRORHANDLER) {
                ReturnValue = E_OVERLOAD_COMMAND_TABLE;
                ASSERT(E_SUCCESS == ReturnValue);
            }
        } else {
            ReturnValue = E_OVERLOAD_COMMAND_TABLE;
            ASSERT(E_SUCCESS == ReturnValue);
        }
    }

    /* checks if command is already registered */
    if (CES_IsCommandRegistered(CmdData_p->CommandNr, CmdData_p->ApplicationNr)) {
        A_(printf("command_audit.c (%d): ** Command is already registered in Execution Context! **\n", __LINE__);)
        return E_COMMAND_ALREADY_REGISTERED;
    }

    ReturnValue = E_SUCCESS;

ErrorExit:
    return ReturnValue;
}

/* @} */
/* @} */
