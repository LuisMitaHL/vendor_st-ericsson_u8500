/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
#ifndef T_LDR_COMMAND_EXEC_SERVICE_H_
#define T_LDR_COMMAND_EXEC_SERVICE_H_

/**
 * \addtogroup ldr_command_exec_service
 * @{
 *
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "error_codes.h"

/*******************************************************************************
 * Types, constants
 ******************************************************************************/

/**
 * Contains information for each command.
 * Used for initialization and execution of commands.
 */
//******************************************************************************
typedef struct CommandInformation_s {
    ErrorCode_e(*Init)(void *LocalExecutionCtx_p);
    /**< Pointer to initialization
             function (Init). Used to
             initialize global set of command
             parameters. Not mandatory. */
    ErrorCode_e(*Execute)(void *LocalExecutionCtx_p);
    /**< Pointer to function that
              executes command.     */
    uint16                 CmdNumber;   /**< Command number. */
    uint32                 RequiredPermissions;
    /**< Information for command auditing. */
} CommandInformation_t;


/**
 * This number is the same for all command goupes.
 *
 */
#define COMMAND_ERRORHANDLER 255

/**
 * This number represent the total number of parameters in
 * Do_CES_SetErrorHandlingData command.
 *
 */
#define ERROR_HANDLING_DATA_SIZE 2
/*******************************************************************************
 * Global Variables
 ******************************************************************************/

/** @} */
#endif /*T_LDR_COMMAND_EXEC_SERVICE_H_*/
