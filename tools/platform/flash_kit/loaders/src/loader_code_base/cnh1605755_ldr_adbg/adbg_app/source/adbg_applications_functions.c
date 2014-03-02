/*******************************************************************************
 * $Copyright Ericsson AB 2009 $
 ******************************************************************************/

/*
 * @addtogroup ADbg
 * @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdlib.h>
#include <string.h>

#include "r_basicdefinitions.h"
#include "r_debug.h"
#include "r_debug_macro.h"
#include "r_main.h"
#include "r_adbg_main_module.h"
#include "r_adbg_command.h"
#include "r_adbg_result.h"
#include "command_ids.h"
#include "commands_impl.h"
#include "commands.h"

/*******************************************************************************
 * File scope types, constants and variables
 ******************************************************************************/
ErrorCode_e ExpectedResult = E_SUCCESS;
uint16 SessionCopy = 1;

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/
/**
 * @brief Function that executes List Case command.
 *
 * @param [in]     Session is session number.
 * @return         @ref E_SUCCESS – Success ending.
 *
 * @return         @ref E_ALLOCATE_FAILED - the CPU failed to allocate memory.
 *
 * @remark         see \c ErrorCode_e for detail description of the error codes.
 */
ErrorCode_e Do_ADbg_ListCaseImpl(uint16 Session)
{
    ErrorCode_e ReturnValue = E_SUCCESS;
    ADbg_MainModule_t ADbg_MainModule;
    ADbg_Command_t ADbg_Command;
    ADbg_Result_t ADbg_Result;
    uint32 PayloadSize = 0;
    uint8 *PayloadData_p  = NULL;

    B_(printf("Do_Loader_ADbg_ListCase......COMMAND_ADBG_LIST_CASE(%x)\n\n", ReturnValue);)

    PayloadSize = BYTE + ADBG_NUMBER_OF_MODULES_MAX * \
                  ((ADBG_STRING_LENGTH_MAX + BYTE + BYTE) + ADBG_NUMBER_OF_CASES_MAX * \
                   (ADBG_STRING_LENGTH_MAX + BYTE + ADBG_AVERAGE_NUMBER_OF_PARAMETERS));

    PayloadData_p  = (uint8 *)malloc(PayloadSize);
    ASSERT(NULL != PayloadData_p);

    Do_ADbg_Command_Init(COMMAND_ADBG_LISTCASE, 0, 0, 0, 0, FALSE, NULL, &ADbg_Command);
    Do_ADbg_Result_Init(PayloadData_p, &ADbg_Result);
    Do_ADbg_MainModule_Init(MainModule_pp, &ADbg_Command, &ADbg_Result, &ADbg_MainModule);

    Do_ADbg_InAllModules(&ADbg_MainModule);

    /** Fill palyoad with Error_Code */
    PayloadSize = ADbg_Result.Size;
    ReturnValue = ADbg_Result.ErrorValue;

    ReturnValue = Done_ADbg_ListCase(Session, ReturnValue, PayloadSize, (void *)PayloadData_p);
    ASSERT(E_SUCCESS == ReturnValue);

    if (NULL != PayloadData_p) {
        free(PayloadData_p);
    }

    return ReturnValue;
}

/**
 * @brief Function that executes List Interface command.
 *
 * @param [in]     Session is session number.
 * @return         @ref E_SUCCESS – Success ending.
 *
 * @return         @ref E_ALLOCATE_FAILED - the CPU failed to allocate memory.
 *
 * @remark         see \c ErrorCode_e for detail description of the error codes.
 */
ErrorCode_e Do_ADbg_ListInterfaceImpl(uint16 Session)
{
    ErrorCode_e ReturnValue = E_SUCCESS;
    ADbg_MainModule_t ADbg_MainModule;
    ADbg_Command_t ADbg_Command;
    ADbg_Result_t ADbg_Result;
    uint8 *PayloadData_p  = NULL;
    uint32 PayloadSize = 0;

    B_(printf("Do_Loader_ADbg_ListInterface......COMMAND_ADBG_LIST_INTERFACE(%x)\n\n", ReturnValue);)

    PayloadSize = BYTE + ADBG_NUMBER_OF_MODULES_MAX * (BYTE + BYTE + ADBG_NUMBER_OF_INT_GROUPS_MAX * \
                  (ADBG_STRING_LENGTH_MAX + BYTE + BYTE + ADBG_NUMBER_OF_INT_FUNCTIONS_MAX * \
                   (ADBG_STRING_LENGTH_MAX + BYTE + ADBG_AVERAGE_NUMBER_OF_PARAMETERS)));

    PayloadData_p  = (uint8 *)malloc(PayloadSize);
    ASSERT(NULL != PayloadData_p);

    Do_ADbg_Command_Init(COMMAND_ADBG_LISTINTERFACE, 0, 0, 0, 0, FALSE, NULL, &ADbg_Command);
    Do_ADbg_Result_Init(PayloadData_p, &ADbg_Result);
    Do_ADbg_MainModule_Init(MainModule_pp, &ADbg_Command, &ADbg_Result, &ADbg_MainModule);

    Do_ADbg_InAllModules(&ADbg_MainModule);

    /** Fill palyoad with Error_Code */
    PayloadSize = ADbg_Result.Size;
    ReturnValue = ADbg_Result.ErrorValue;

    ReturnValue = Done_ADbg_ListInterface(Session, ReturnValue, PayloadSize, (void *)PayloadData_p);
    ASSERT(E_SUCCESS == ReturnValue);

    if (NULL != PayloadData_p) {
        free(PayloadData_p);
    }

    return ReturnValue;
}

/**
 * @brief Function that executes Set Precondition command.
 *
 * @param [in]     Session is session number.
 * @param [in]     ModuleId is module Id.
 * @param [in]     IntGroupId is interface group Id.
 * @param [in]     IntFunctionId is interface function Id.
 * @param [in]     RecoveryFlag is TRUE if condition should be saved.
 * @param [in]     Precondition_p is pointer to buffer for parameters.
 * @return         @ref E_SUCCESS – Success ending.
 *
 * @return         @ref E_ALLOCATE_FAILED - the CPU failed to allocate memory.
 *
 * @remark         see \c ErrorCode_e for detail description of the error codes.
 */
ErrorCode_e Do_ADbg_SetPreconditionImpl(uint16  Session,
                                        const uint32 ModuleId,
                                        const uint32 IntGroupId,
                                        const uint32 IntFunctionId,
                                        const uint32 RecoveryFlag,
                                        const uint32 PreconditionLength,
                                        void   *Precondition_p)
{
    ErrorCode_e ReturnValue = E_SUCCESS;
    ADbg_MainModule_t ADbg_MainModule;
    ADbg_Command_t ADbg_Command;
    ADbg_Result_t ADbg_Result;
    const ADbg_ModuleDefinition_t *TestModule_p;

    B_(printf("Do_Loader_ADbg_SetPrecondition......COMMAND_ADBG_SET_PRECONDITION(%x)\n\n", ReturnValue);)

    Do_ADbg_Command_Init(COMMAND_ADBG_SETPRECONDITION, ModuleId, 0, IntGroupId, IntFunctionId, RecoveryFlag, Precondition_p, &ADbg_Command);
    Do_ADbg_Result_Init(NULL, &ADbg_Result);
    Do_ADbg_MainModule_Init(MainModule_pp, &ADbg_Command, &ADbg_Result, &ADbg_MainModule);

    TestModule_p = Do_ADbg_FindModule(&ADbg_MainModule);

    if (NULL == TestModule_p) {
        ADbg_Result.ErrorValue = E_MODULE_NOT_FOUND;
        goto ErrorExit;
    }

    Do_ADbg_InOneModule(TestModule_p, &ADbg_MainModule);

ErrorExit:
    /** Fill palyoad with Error_Code */
    ReturnValue = ADbg_Result.ErrorValue;

    ReturnValue = Done_ADbg_SetPrecondition(Session, ReturnValue);
    ASSERT(E_SUCCESS == ReturnValue);

    return ReturnValue;
}

/**
 * @brief Function that executes Recovery Condition command.
 *
 * @param [in]     Session is session number.
 * @param [in]     ModuleId is module Id.
 * @param [in]     IntGroupId is interface group Id.
 * @param [in]     IntFunctionId is interface function Id.
 * @return         @ref E_SUCCESS – Success ending.
 *
 * @return         @ref E_ALLOCATE_FAILED - the CPU failed to allocate memory.
 *
 * @remark         see \c ErrorCode_e for detail description of the error codes.
 */
ErrorCode_e Do_ADbg_RecoveryConditionImpl(uint16  Session,
        const uint32 ModuleId,
        const uint32 IntGroupId,
        const uint32 IntFunctionId)
{
    ErrorCode_e ReturnValue = E_SUCCESS;
    ADbg_MainModule_t ADbg_MainModule;
    ADbg_Command_t ADbg_Command;
    ADbg_Result_t ADbg_Result;
    const ADbg_ModuleDefinition_t *TestModule_p;

    B_(printf("Do_Loader_ADbg_RecoveryCondition......COMMAND_ADBG_RECOVERY_CONDITION(%x)\n\n", ReturnValue);)

    Do_ADbg_Command_Init(COMMAND_ADBG_RECOVERYCONDITION, ModuleId, 0, IntGroupId, IntFunctionId, 0, NULL, &ADbg_Command);
    Do_ADbg_Result_Init(NULL, &ADbg_Result);
    Do_ADbg_MainModule_Init(MainModule_pp, &ADbg_Command, &ADbg_Result, &ADbg_MainModule);

    TestModule_p = Do_ADbg_FindModule(&ADbg_MainModule);
    Do_ADbg_InOneModule(TestModule_p, &ADbg_MainModule);

    /** Fill palyoad with Error_Code */
    ReturnValue = ADbg_Result.ErrorValue;

    ReturnValue = Done_ADbg_RecoveryCondition(Session, ReturnValue);
    ASSERT(E_SUCCESS == ReturnValue);

    return ReturnValue;
}

/**
 * @brief Function that executes Run command.
 *
 * @param [in]     Session is session number.
 * @param [in]     ModuleId is module Id.
 * @param [in]     CaseId is case Id.
 * @param [in]     Precondition_p is pointer to buffer for parameters.
 * @return         @ref E_SUCCESS – Success ending.
 *
 * @return         @ref E_ALLOCATE_FAILED - the CPU failed to allocate memory.
 *
 * @remark        see \c ErrorCode_e for detail description of the error codes.
 */
ErrorCode_e Do_ADbg_RunImpl(uint16  Session,
                            const uint32 ModuleId,
                            const uint32 CaseId,
                            const uint32 PreconditionLength,
                            void *Precondition_p)
{
    ErrorCode_e ReturnValue = E_SUCCESS;
    ADbg_MainModule_t ADbg_MainModule;
    ADbg_Command_t ADbg_Command;
    ADbg_Result_t ADbg_Result;
    const ADbg_ModuleDefinition_t *TestModule_p;
    uint32 AssertStatus = 0;
    SessionCopy = Session;

    B_(printf("Do_Loader_ADbg_Run......COMMAND_ADBG_RUN(%x)\n\n", ReturnValue);)

    Do_ADbg_Command_Init(COMMAND_ADBG_RUN, ModuleId, CaseId, 0, 0, 0, Precondition_p, &ADbg_Command);
    Do_ADbg_Result_Init((uint8 *)&AssertStatus, &ADbg_Result);
    Do_ADbg_MainModule_Init(MainModule_pp, &ADbg_Command, &ADbg_Result, &ADbg_MainModule);

    TestModule_p = Do_ADbg_FindModule(&ADbg_MainModule);

    if (NULL == TestModule_p) {
        ADbg_Result.ErrorValue = E_MODULE_NOT_FOUND;
        goto ErrorExit;
    }

    Do_ADbg_InOneModule(TestModule_p, &ADbg_MainModule);

    IDENTIFIER_NOT_USED(PreconditionLength);
ErrorExit:
    /** Fill payload with Error_Code */
    //ReturnValue = ADbg_Result.ErrorValue;

    //ReturnValue = Done_ADbg_Run(Session, ReturnValue, AssertStatus);
    //ASSERT(E_SUCCESS == ReturnValue);

    return ReturnValue;
}

/**
 * @brief Function that is called from each Done.. function from command_marshal.c
 *
 * @param [in]     Result is error code returned from the function which is tested.
 * @return         ErrorCode_e.
 *
 * @remark         It doesn't check the input parameters.
 */
ErrorCode_e ReleaseADbg_AppFunc(ErrorCode_e Result)
{
    ErrorCode_e ReturnValue = E_SUCCESS;
    uint32 AssertStatus = 1; //1 == Negative assert, 0 == Postitive assert

    if (ExpectedResult == Result) {
        AssertStatus = 0;
    }

    ReturnValue = Done_ADbg_Run(SessionCopy, ReturnValue, AssertStatus);

    return ReturnValue;
}

/**
 * @brief Function that is called from the test cases for functions which are not commands
 *
 * @param [in]     AssertStatus is the result from the tested assert. 0 if expected result is equal with returned result, 1 if not
 * @return         None.
 *
 * @remark         It doesn't check the input parameters.
 */
void ReleaseADbg(uint32 AssertStatus)
{
    ErrorCode_e ReturnValue = E_SUCCESS;

    ReturnValue = Done_ADbg_Run(SessionCopy, ReturnValue, AssertStatus);
}
/* @} */
