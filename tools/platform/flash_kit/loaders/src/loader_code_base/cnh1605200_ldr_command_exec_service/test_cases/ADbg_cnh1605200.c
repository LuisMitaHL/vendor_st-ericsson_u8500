/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/

/**
 * @file ADbg_cnh1605200.c
 *
 * @brief Test cases for CNH1605200 module.
 *
 * This file consist of test cases for autometic testing functions from
 * CNH1605200 module.
 */

/*
 * @addtogroup ADbg
 * @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "r_adbg.h"
#include "r_adbg_assert.h"
#include "r_adbg_case.h"
#include "r_adbg_module.h"
#include "r_adbg_command.h"
#include "command_audit.h"
#include "commands_impl.h"
#include "r_serialization.h"
#include "r_command_exec_service.h"
#include "r_memory_utils.h"

/*******************************************************************************
 * The test case functions declarations
 ******************************************************************************/
// Command_audit
static void ADbg_CES_AuditCommand(ADbg_Case_t *Case_p);

// Command_exec_service
static void ADbg_Do_CES_ActiveApplications(ADbg_Case_t *Case_p);
static void ADbg_Do_CES_Init(ADbg_Case_t *Case_p);
static void ADbg_Do_CES_CommandExecutionPoll(ADbg_Case_t *Case_p);
static void ADbg_Do_CES_RegisterRepeatCommand(ADbg_Case_t *Case_p);

// Running_commands
static void ADbg_CES_InitializeExecContextQueue(ADbg_Case_t *Case_p);
static void ADbg_CES_InitializeCommandExecutionContext(ADbg_Case_t *Case_p);
static void ADbg_CES_AddCommand(ADbg_Case_t *Case_p);
static void ADbg_CES_DequeueNextCommandToExecute(ADbg_Case_t *Case_p);
static void ADbg_CES_IsCommandRegistered(ADbg_Case_t *Case_p);
static void ADbg_CES_EnqueueCommandToExecute(ADbg_Case_t *Case_p);


/*******************************************************************************
 * The test case definitions
 ******************************************************************************/
// Command_audit
static uint8 Parameters1_1[3] = {2, WORDPTR, WORD};
ADBG_CASE_DEFINE(Test_CES_AuditCommand, 1, 6, ADbg_CES_AuditCommand, Parameters1_1);

// Command_exec_service
static uint8 Parameters2_1[2] = {1, WORD};
ADBG_CASE_DEFINE(Test_Do_CES_ActiveApplications, 2, 6, ADbg_Do_CES_ActiveApplications, Parameters2_1);
static uint8 Parameters2_2[1] = {0};
ADBG_CASE_DEFINE(Test_Do_CES_Init, 3, 6, ADbg_Do_CES_Init, Parameters2_2);
static uint8 Parameters2_3[1] = {0};
ADBG_CASE_DEFINE(Test_Do_CES_CommandExecutionPoll, 4, 6, ADbg_Do_CES_CommandExecutionPoll, Parameters2_3);
static uint8 Parameters2_4[3] = {2, WORDPTR, WORD};
ADBG_CASE_DEFINE(Test_Do_CES_RegisterRepeatCommand, 5, 6, ADbg_Do_CES_RegisterRepeatCommand, Parameters2_4);

// Running_commands
static uint8 Parameters3_1[1] = {0};
ADBG_CASE_DEFINE(Test_CES_InitializeExecContextQueue, 6, 6, ADbg_CES_InitializeExecContextQueue, Parameters3_1);
static uint8 Parameters3_2[4] = {3, WORDPTR, WORDPTR, WORD};
ADBG_CASE_DEFINE(Test_CES_InitializeCommandExecutionContext, 7, 6, ADbg_CES_InitializeCommandExecutionContext, Parameters3_2);
static uint8 Parameters3_3[3] = {2, WORDPTR, WORD};
ADBG_CASE_DEFINE(Test_CES_AddCommand, 8, 6, ADbg_CES_AddCommand, Parameters3_3);
static uint8 Parameters3_4[2] = {1, WORD};
ADBG_CASE_DEFINE(Test_CES_DequeueNextCommandToExecute, 9, 6, ADbg_CES_DequeueNextCommandToExecute, Parameters3_4);
static uint8 Parameters3_5[4] = {3, BYTE, BYTE, BYTE};
ADBG_CASE_DEFINE(Test_CES_IsCommandRegistered, 10, 6, ADbg_CES_IsCommandRegistered, Parameters3_5);
static uint8 Parameters3_6[2] = {1, WORDPTR};
ADBG_CASE_DEFINE(Test_CES_EnqueueCommandToExecute, 11, 6, ADbg_CES_EnqueueCommandToExecute, Parameters3_6);

/*******************************************************************************
* The test case suite definition
******************************************************************************/
ADBG_SUITE_DEFINE_BEGIN(TestCase_CNH1605200, 0, 6)
// Command_audit
ADBG_SUITE_ENTRY(Test_CES_AuditCommand)

// Command_exec_service
ADBG_SUITE_ENTRY(Test_Do_CES_ActiveApplications)
ADBG_SUITE_ENTRY(Test_Do_CES_Init)
ADBG_SUITE_ENTRY(Test_Do_CES_CommandExecutionPoll)
ADBG_SUITE_ENTRY(Test_Do_CES_RegisterRepeatCommand)

// Running_commands
ADBG_SUITE_ENTRY(Test_CES_InitializeExecContextQueue)
ADBG_SUITE_ENTRY(Test_CES_InitializeCommandExecutionContext)
ADBG_SUITE_ENTRY(Test_CES_AddCommand)
ADBG_SUITE_ENTRY(Test_CES_DequeueNextCommandToExecute)
ADBG_SUITE_ENTRY(Test_CES_IsCommandRegistered)
ADBG_SUITE_ENTRY(Test_CES_EnqueueCommandToExecute)

ADBG_SUITE_DEFINE_END();

/*******************************************************************************
 * Case runner
 ******************************************************************************/
void Do_CNH1605200_Module_Test_Run(ADbg_MainModule_t *MainModule_p)
{
    ADbg_Module_t *Module_p = Do_ADbg_Module_Create(TestCase_CNH1605200, NULL, MainModule_p->Command_p, MainModule_p->Result_p);
    Do_ADbg_InModule_Command(Module_p);
    Do_ADbg_Module_Destroy(Module_p);
}
/*******************************************************************************
 * Test cases
 ******************************************************************************/
// Command_audit
/**
 * This function will test function:
 * CES_AuditCommand.
 * Function used in: Test_CES_AuditCommand.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_CES_AuditCommand(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_SUCCESS;
    uint32 CmdDataLength = 0;
    CommandData_t *CmdData_p;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;

    CmdDataLength = get_uint32_le((void **)&Var_p);
    CmdData_p = Do_ADbg_GetDataPointer(CmdDataLength, (void **)&Var_p);

    Result = CES_AuditCommand(CmdData_p);
    Do_ADbg_Assert((ErrorCode_e)(*Var_p) == Result, Case_p);

    BUFFER_FREE(CmdData_p);
}

// Command_exec_service

/**
 * This function will test function:
 * Do_CES_ActiveApplications.
 * Function used in: Test_Do_CES_ActiveApplications.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_Do_CES_ActiveApplications(ADbg_Case_t *Case_p)
{
    uint32 Result = 0;
    uint32 Return = 0;

    Result = Do_CES_ActiveApplications();
    Do_ADbg_Assert(Return != Result, Case_p);
}

/**
 * This function will test function:
 * Do_CES_Init.
 * Function used in: Test_Do_CES_Init.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_Do_CES_Init(ADbg_Case_t *Case_p)
{
    uint32 Result = 0;
    uint32 Return = 0;

    Do_CES_Init();
    Do_ADbg_Assert(Return == Result, Case_p);
}

/**
 * This function will test function:
 * Do_CES_CommandExecutionPoll.
 * Function used in: Test_Do_CES_CommandExecutionPoll.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_Do_CES_CommandExecutionPoll(ADbg_Case_t *Case_p)
{
    uint32 Result = 0;
    uint32 Return = 0;

    Do_CES_CommandExecutionPoll();
    Do_ADbg_Assert(Return == Result, Case_p);
}

/**
 * This function will test function:
 * Do_CES_RegisterRepeatCommand.
 * Function used in: Test_Do_CES_RegisterRepeatCommand.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_Do_CES_RegisterRepeatCommand(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_SUCCESS;
    uint32 CmdDataLength = 0;
    CommandData_t *CmdData_p;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;

    CmdDataLength = get_uint32_le((void **)&Var_p);
    CmdData_p = Do_ADbg_GetDataPointer(CmdDataLength, (void **)&Var_p);

    Result = Do_CES_RegisterRepeatCommand(CmdData_p);
    Do_ADbg_Assert((ErrorCode_e)(*Var_p) == Result, Case_p);

    BUFFER_FREE(CmdData_p);
}

// Running_commands

/**
 * This function will test function:
 * CES_InitializeExecContextQueue.
 * Function used in: Test_CES_InitializeExecContextQueue.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_CES_InitializeExecContextQueue(ADbg_Case_t *Case_p)
{
    uint32 Result = 0;
    uint32 Return = 0;

    CES_InitializeExecContextQueue();
    Do_ADbg_Assert(Return == Result, Case_p);
}

/**
 * This function will test function:
 * CES_InitializeCommandExecutionContext.
 * Function used in: Test_CES_InitializeCommandExecutionContext.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_CES_InitializeCommandExecutionContext(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_SUCCESS;
    uint32 CmdDataLength = 0;
    CommandData_t *CmdData_p;
    uint32 ExecContextLength = 0;
    ExecutionContext_t *ExecContext_p;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;

    CmdDataLength = get_uint32_le((void **)&Var_p);
    CmdData_p = Do_ADbg_GetDataPointer(CmdDataLength, (void **)&Var_p);

    ExecContextLength = get_uint32_le((void **)&Var_p);
    ExecContext_p = Do_ADbg_GetDataPointer(ExecContextLength, (void **)&Var_p);

    Result = CES_InitializeCommandExecutionContext(CmdData_p, ExecContext_p);
    Do_ADbg_Assert((ErrorCode_e)(*Var_p) == Result, Case_p);

    BUFFER_FREE(CmdData_p);
    BUFFER_FREE(ExecContext_p);

}

/**
 * This function will test function:
 * CES_AddCommand.
 * Function used in: Test_CES_AddCommand.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_CES_AddCommand(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_SUCCESS;
    uint32 ExecContextLength = 0;
    ExecutionContext_t *ExecContext_p;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;

    ExecContextLength = get_uint32_le((void **)&Var_p);
    ExecContext_p = Do_ADbg_GetDataPointer(ExecContextLength, (void **)&Var_p);

    Result = CES_AddCommand(ExecContext_p);
    Do_ADbg_Assert((ErrorCode_e)(*Var_p) == Result, Case_p);

    BUFFER_FREE(ExecContext_p);
}

/**
 * This function will test function:
 * CES_DequeueNextCommandToExecute.
 * Function used in: Test_CES_DequeueNextCommandToExecute.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_CES_DequeueNextCommandToExecute(ADbg_Case_t *Case_p)
{
    ExecutionContext_t *Result_p;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;

    Result_p = CES_DequeueNextCommandToExecute();
    Do_ADbg_Assert((ExecutionContext_t *)(*Var_p) == Result_p, Case_p);
}

/**
 * This function will test function:
 * CES_IsCommandRegistered.
 * Function used in: Test_CES_IsCommandRegistered.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_CES_IsCommandRegistered(ADbg_Case_t *Case_p)
{
    boolean Result = FALSE;
    uint8 CommandNumber = 0;
    uint8 ApplicationNumber = 0;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;

    Do_ADbg_GetDataVar(sizeof(int), &Var_p, &CommandNumber);
    Do_ADbg_GetDataVar(sizeof(int), &Var_p, &ApplicationNumber);

    Result = CES_IsCommandRegistered(CommandNumber, ApplicationNumber);
    Do_ADbg_Assert((boolean)(*Var_p) == Result, Case_p);
}

/**
 * This function will test function:
 * CES_EnqueueCommandToExecute.
 * Function used in: Test_CES_EnqueueCommandToExecute.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_CES_EnqueueCommandToExecute(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_SUCCESS;
    uint32 ExecContextLength = 0;
    ExecutionContext_t *ExecContext_p;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;

    ExecContextLength = get_uint32_le((void **)&Var_p);
    ExecContext_p = Do_ADbg_GetDataPointer(ExecContextLength, (void **)&Var_p);

    CES_EnqueueCommandToExecute(ExecContext_p);
    Do_ADbg_Assert(E_SUCCESS == Result, Case_p);
}
/** @} */
