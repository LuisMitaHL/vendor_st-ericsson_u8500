/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/

/*
 * @addtogroup ldr_command_exec_service
 * @{
 *  @addtogroup  running_commands
 *  @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "command_execution_control.h"
#include <stdlib.h>
#include <string.h>

#include "r_debug.h"
#include "r_debug_macro.h"
#include "r_basicdefinitions.h"
#include "error_codes.h"
#include "r_command_exec_service.h"
#include "t_communication_service.h"
#include "r_main.h"
#include "r_queue.h"

/*******************************************************************************
 * File scope types, constants and variables
 ******************************************************************************/
uint32 NumberOfRunningCmds = 0;         /* Indicates number of running commands */
void *ExecContextQueue_p;               /* Pointer on execution context queue */

/*******************************************************************************
 * Definition of external constants and variables
 ******************************************************************************/
extern ApplicationInfo_t ActiveApplications[];  /*  */

/*******************************************************************************
 * Declaration of file local functions
 ******************************************************************************/

/*
 *  Parsing input parameters in order to adjust Command Information
 *  structure that hold data for command execution. Populate all
 *  the issues of the execution context constellations structures
 *
 *  @param [in]  CmdData_p     Pointer to command data structure.
 *                             This structure is rearranged in the transport layer and it carries
 *                             the data which are imported for each command, separately.
 *  @param [out] ExecContext_p Pointer on command execution context. This
 *                             structure must be rearranged base on input CmdData_p.
 *  @return      @ref E_GENERAL_FATAL_ERROR General Failure. Unknown error.
 *  @return      @ref E_INVALID_INPUT_PARAMETER The expected value into the function was incorrect.
 *  @return      @ref E_SUCCESS The function completed successfully.
 *  @return      @ref E_UNSUPPORTED_CMD The loader does not support the requested command.
 *  @return      @ref E_UNSUPPORTED_GROUP The loader does not support the requested group.
 */
static ErrorCode_e  CES_FillExecutionContext(const CommandData_t *CmdData_p, ExecutionContext_t *ExecContext_p);

/*
 *  Searches all registered commands in all applications and returns pointer
 *  to structure that matches CommandNumber and ApplicationNumber. If command
 *  is not found function returns appropriate error.
 *
 *  @param [in] CommandNumber     number for the command ID.
 *  @param [in] ApplicationNumber number for the application ID where
 *  @param [out] CommandInfo_pp   Pointer to command register structures.
 *                                corresponded command belongs.
 *  @return      @ref E_SUCCESS The function completed successfully.
 *  @return      @ref E_UNSUPPORTED_CMD The loader does not support the requested command.
 *  @return      @ref E_UNSUPPORTED_GROUP The loader does not support the requested group.
 */
static ErrorCode_e CES_GetCommandInformation(const uint8 CmdNumber, const uint8 CmdGroup, CommandInformation_t **CommandInfo_p);

static boolean MatchCommands(void *Value1_p, void *Value2_p);
/*******************************************************************************
 * Definition of internal functions
 ******************************************************************************/
static ErrorCode_e  CES_FillExecutionContext(const CommandData_t *CmdData_p, ExecutionContext_t *ExecContext_p)
{
    ErrorCode_e ReturnValue             = E_GENERAL_FATAL_ERROR;
    CommandInformation_t *CmdInfo_p    = NULL;

    /*
     *  Sanity check of the size
     */
    VERIFY(NULL != CmdData_p, E_INVALID_INPUT_PARAMETER);

    /* GetCommandInformation searches over entire command group context application
     * register context and returns structure with matches index if match is found.
     * If command or command group is not found function returns error code.
     */
    ReturnValue = CES_GetCommandInformation(CmdData_p->CommandNr, CmdData_p->ApplicationNr, &CmdInfo_p);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    /* fill Execution context structures  */
    ExecContext_p-> Command_p    = CmdInfo_p;
    ExecContext_p->LocalState_p  = NULL;
    ExecContext_p->Running       = TRUE;

    ExecContext_p->Received = *CmdData_p;

    ExecContext_p->Result.Status = E_SUCCESS;
    ExecContext_p->Result.Response_p = &(ExecContext_p->Received.Payload);

    ExecContext_p->Progress = 0;

ErrorExit:
    return ReturnValue;
}

static ErrorCode_e CES_GetCommandInformation(const uint8 CommandNumber, const uint8 ApplicationNumber, CommandInformation_t **CommandInfo_pp)
{
    unsigned int ApplicationIndex;
    unsigned int CommandIndex;
    /*
    *  Sanity check of the size
    */
    //2do think more about conditions

    for (ApplicationIndex = 0; ApplicationIndex < Do_CES_ActiveApplications() ; ApplicationIndex++) {
        if (ActiveApplications[ApplicationIndex].ApplicationNumber  == ApplicationNumber) {
            for (CommandIndex = 0; CommandIndex < ActiveApplications[ApplicationIndex].ApplicationLength; CommandIndex++) {
                if ((ActiveApplications[ApplicationIndex].ListOfCommands_p)[CommandIndex].CmdNumber == CommandNumber) {
                    *CommandInfo_pp = (((ActiveApplications + ApplicationIndex)->ListOfCommands_p) + CommandIndex);
                    return E_SUCCESS;
                }
            }

            return  E_UNSUPPORTED_CMD;
        }
    }

    return E_UNSUPPORTED_GROUP;
}

static boolean MatchCommands(void *Value1_p, void *Value2_p)
{
    ExecutionContext_t *Context1_p = (ExecutionContext_t *)Value1_p;
    ExecutionContext_t *Context2_p = (ExecutionContext_t *)Value2_p;

    if ((Context1_p->Received.CommandNr == Context2_p->Received.CommandNr) &&
            (Context1_p->Received.ApplicationNr  == Context2_p->Received.ApplicationNr)) {
        return TRUE;
    }

    return FALSE;
}

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/
void CES_InitializeExecContextQueue(void)
{
    Do_Fifo_Create(NULL, &ExecContextQueue_p, MAX_CONCURRENT_COMMANDS + 4, NULL);

    (void)Do_Fifo_SetCallback(NULL, ExecContextQueue_p, QUEUE_EMPTY, NULL, NULL);
    (void)Do_Fifo_SetCallback(NULL, ExecContextQueue_p, QUEUE_NONEMPTY, NULL, NULL);
}

ErrorCode_e CES_InitializeCommandExecutionContext(const CommandData_t *CmdData_p, ExecutionContext_t *ExecContext_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    /*
     *  Sanity check of the size
     */
    VERIFY(NULL != CmdData_p, E_INVALID_INPUT_PARAMETER);

    /* Fill Execution context structures  */
    ReturnValue = CES_FillExecutionContext(CmdData_p, ExecContext_p);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

ErrorExit:
    return ReturnValue;
}

ErrorCode_e CES_AddCommand(ExecutionContext_t *ExecContext_p)
{
    ErrorCode_e ReturnValue = E_SUCCESS;
    /*
     *  Sanity check of the size
     */
    VERIFY(NULL != ExecContext_p, E_INVALID_INPUT_PARAMETER);

    /* add command into queue e.g. register command */
    ReturnValue = Do_RFifo_Enqueue(NULL, ExecContextQueue_p, ExecContext_p);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    NumberOfRunningCmds++;

ErrorExit:
    return ReturnValue;
}

ExecutionContext_t *CES_DequeueNextCommandToExecute(void)
{
    return (ExecutionContext_t *)Do_RFifo_Dequeue(NULL, ExecContextQueue_p);
}

boolean CES_IsCommandRegistered(const uint8 CommandNumber, const uint8 ApplicationNumber)
{
    ExecutionContext_t TempContext;
    TempContext.Received.CommandNr = CommandNumber;
    TempContext.Received.ApplicationNr = ApplicationNumber;
    return Do_RFifo_IsMember(NULL, ExecContextQueue_p, &TempContext, MatchCommands);
}

void CES_EnqueueCommandToExecute(ExecutionContext_t *ExecContext_p)
{
    /* add command into queue e.g. register command in the queue*/
    ///  //lint --e(534)
    (void)Do_RFifo_Enqueue(NULL, ExecContextQueue_p, ExecContext_p);
}

/**
 * Get minimal progress status in percent from all the active long running
 * commands.
 *
 * @param [in, out] ProgressStat_p   Command progress status presented in
 *                                   percent.
 *
 * retval E_SUCCESS                  Successful execution.
 * retval E_INVALID_INPUT_PARAMETERS Invalid input parameters.
 * retval E_FAILED_TO_STORE_IN_FIFO  Failed to enque the command.
 */
ErrorCode_e CES_GetProgressStatus(uint32 *ProgressStat_p)
{
#define MAX_PROGRESS      (100)
#define NO_PROGRESS       (0xffffffff)
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    ExecutionContext_t *ExeCtx_p;
    uint32 Elements = 0;
    uint32 i = 0;

    Elements = (uint32)Do_RFifo_GetNrOfElements(NULL, ExecContextQueue_p);

    if (0 == Elements) {
        *ProgressStat_p = NO_PROGRESS;
        return E_SUCCESS;
    }

    *ProgressStat_p = MAX_PROGRESS;

    for (i = 0; i < Elements; i++) {
        ExeCtx_p = (ExecutionContext_t *)Do_RFifo_Dequeue(NULL, ExecContextQueue_p);
        VERIFY(ExeCtx_p != NULL, E_INVALID_INPUT_PARAMETERS);

        *ProgressStat_p = MIN(*ProgressStat_p, ExeCtx_p->Progress);
        VERIFY(MAX_PROGRESS >= *ProgressStat_p, E_GENERAL_FATAL_ERROR);

        ReturnValue = Do_RFifo_Enqueue(NULL, ExecContextQueue_p, ExeCtx_p);
        VERIFY(ReturnValue == E_SUCCESS, ReturnValue);
    }

    ReturnValue = E_SUCCESS;

    return ReturnValue;

ErrorExit:
    *ProgressStat_p = 0;

    return ReturnValue;

#undef NO_PROGRESS
#undef MAX_PROGRESS
}

/* @} */
/* @} */
