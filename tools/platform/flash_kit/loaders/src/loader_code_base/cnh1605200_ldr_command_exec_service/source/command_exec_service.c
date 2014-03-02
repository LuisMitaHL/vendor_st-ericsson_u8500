/*******************************************************************************
 * $Copyright ST-Ericsson 2010 $
 ******************************************************************************/

/*
 * @ddtogroup ldr_command_exec_service
 * @{
 *  @addtogroup  command_exec_service
 *  @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "command_execution_control.h"
#include "r_debug.h"
#include "r_debug_macro.h"
#include "r_memory_utils.h"
#include <string.h>

#include "r_main.h"
#include "r_command_exec_service.h"
#include "r_command_protocol.h"
#include "running_commands.h"
#include "command_audit.h"
#include "r_communication_service.h"
#include "r_queue.h"
#include "r_serialization.h"
#include "ces_commands.h"

/*******************************************************************************
 * Definition of external constants and variables
 ******************************************************************************/
extern uint32 NumberOfRunningCmds;
extern ApplicationInfo_t ActiveApplications[];

/*******************************************************************************
 * File scope types, constants and variables
 ******************************************************************************/
void *LocalExecutionCtx_p;
struct CommandInformation_s *CommandRegistration_p;

/*******************************************************************************
 * Declaration of file local functions
 ******************************************************************************/
/*
 *  Execution mechanism sequence.
 *
 *  Calling command execution function. If command doesn’t exist in
 *  execution context function return NULL
 *
 *  @return ExecContext_p point on command execution context.
 */
static ExecutionContext_t *CES_CommandExecution(void);

/*
 *  Populate ApplicationLength member in ActiveApplications
 *
 *  ApplicationLength is length of area for structure
 *  pointed by ListOfCommands_p. It must be determined
 *  for each element in ActiveApplications.
 */
static void CES_PopulateApplicationGroupLength(void);
/*******************************************************************************
 * Definition of internal functions
 ******************************************************************************/
static ExecutionContext_t *CES_CommandExecution(void)
{
    ErrorCode_e ReturnValue  = E_GENERAL_FATAL_ERROR;
    ExecutionContext_t *CmdCtx_p = NULL;

    if (NULL == (CmdCtx_p = CES_DequeueNextCommandToExecute())) {
        return NULL;
    }

    /* Call the command execution function */
    ReturnValue = ((CommandInformation_t *)CmdCtx_p->Command_p)->Execute(CmdCtx_p);
    ASSERT(E_SUCCESS == ReturnValue);

    return CmdCtx_p;
}


/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/
uint32 Do_CES_ActiveApplications(void)
{
    uint32 ApplicationInfoSize = 0;
    uint32 ActiveApplicationsSize = 0;

    ApplicationInfoSize = Get_ApplicationInfoSize();
    ActiveApplicationsSize = Get_ActiveApplicationsSize();

    return ActiveApplicationsSize / ApplicationInfoSize;
}

static void CES_PopulateApplicationGroupLength(void)
{
    unsigned int ApplicationIndex;
    uint32 ApplicationLength;

    for (ApplicationIndex = 0; ApplicationIndex < Do_CES_ActiveApplications() ; ApplicationIndex++) {
        ApplicationLength = 0;

        while ((ActiveApplications[ApplicationIndex].ListOfCommands_p + ApplicationLength)->Execute != NULL) {
            ApplicationLength++;
        }

        ActiveApplications[ApplicationIndex].ApplicationLength = (uint8)ApplicationLength;
    }
}

void Do_CES_Init(void)
{
    CES_PopulateApplicationGroupLength();
    CES_InitializeExecContextQueue();
}

void Do_CES_CommandExecutionPoll(void)
{
    ExecutionContext_t *ExecContext_p = NULL;

    if (0 != NumberOfRunningCmds) {
        ExecContext_p = CES_CommandExecution();

        if (NULL == ExecContext_p) {
            A_(printf("command_exec_service.c (%d): **ERROR Failed to execute next command**\n", __LINE__);)
            goto ErrorExit;
        }

        if (ExecContext_p->Running) {
            CES_EnqueueCommandToExecute(ExecContext_p);
            goto ErrorExit;
        }

        BUFFER_FREE(ExecContext_p->Received.Payload.Data_p);

        BUFFER_FREE(ExecContext_p);

        NumberOfRunningCmds--;
    }

ErrorExit:
    return;
}

ErrorCode_e Do_CES_RegisterRepeatCommand(CommandData_t *CmdData_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    ExecutionContext_t *ExecContext_p = NULL;

    /* CES_AddNewCommand  registers new command in command execution context queue.
     * It guaranties that new command is not already registered and there is free
     * place in command execution context queue.
     * The function checks if new command is supported.
     * When all conditions are satisfied it registers the new command in
     * command execution context queue. CmdData_p must be allocated.
     */

    /*
     *  Sanity check of the size
     */
    VERIFY(NULL != CmdData_p, E_INVALID_INPUT_PARAMETER);

    /* audit command*/
    ReturnValue = CES_AuditCommand(CmdData_p);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    ExecContext_p = (ExecutionContext_t *)malloc(sizeof(ExecutionContext_t));
    ASSERT(NULL != ExecContext_p);

    ReturnValue = CES_InitializeCommandExecutionContext(CmdData_p, ExecContext_p);
    ASSERT(E_SUCCESS == ReturnValue);

    if (ExecContext_p->Running) {
        /* register command*/
        ReturnValue = CES_AddCommand(ExecContext_p);
        ASSERT(E_SUCCESS == ReturnValue);
    } else {
        BUFFER_FREE(ExecContext_p);
    }

    B_(printf("command_exec_service.c (%d): ** Command register successful! **\n", __LINE__);)

ErrorExit:
    //lint -e429
    return ReturnValue;
    //lint +e429

}

void Do_CES_SetErrorHandlingData(CommandData_t *CmmData_p, ErrorCode_e ReturnValue)
{
    uint8 *Data_p = NULL;

    CmmData_p->Payload.Size = ERROR_HANDLING_DATA_SIZE * sizeof(uint32);
    CmmData_p->Payload.Data_p = (uint8 *)malloc(CmmData_p->Payload.Size);
    ASSERT(NULL != CmmData_p->Payload.Data_p);
    memset(CmmData_p->Payload.Data_p, 0x00, CmmData_p->Payload.Size);

    Data_p = CmmData_p->Payload.Data_p;
    put_uint32_le((void **)&Data_p, ReturnValue);
    put_uint32_le((void **)&Data_p, CmmData_p->CommandNr);
    CmmData_p->CommandNr = COMMAND_ERRORHANDLER;
}

/* @} */
/* @} */
