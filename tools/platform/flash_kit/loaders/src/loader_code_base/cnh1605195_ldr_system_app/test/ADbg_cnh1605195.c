/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
/**
 * @file ADbg_cnh1605195.c
 *
 * @brief Test cases for CNH1605195 module.
 * This file consist of test cases for automatic testing functions from
 * CNH1605195 module.
 */

/**
 *  @addtogroup ADbg
 *  @{
 *
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "r_adbg.h"
#include "r_adbg_assert.h"
#include "r_adbg_case.h"
#include "r_adbg_module.h"
#include "r_adbg_command.h"
#include "r_adbg_applications.h"
#include "t_communication_service.h"
#include "r_debug.h"
#include "commands_impl.h"
#include "string.h"
#include "r_serialization.h"
#include "r_system_applications.h"
#include <stdlib.h>
#include "r_memory_utils.h"

/*******************************************************************************
 * File scope types, constants and variables
 ******************************************************************************/
extern ErrorCode_e ExpectedResult;

/*******************************************************************************
 * The test case functions declarations
 ******************************************************************************/
static void ADbg_Do_System_ShutDownImpl(ADbg_Case_t *Case_p);
static void ADbg_Do_System_SupportedCommandsImpl(ADbg_Case_t *Case_p);
static void ADbg_Do_System_RebootImpl(ADbg_Case_t *Case_p);
static void ADbg_Do_System_AuthenticateImpl(ADbg_Case_t *Case_p);
static void ADbg_Done_System_LoaderStartUpStatusImpl(ADbg_Case_t *Case_p);
static void ADbg_Done_System_GetControlKeysImpl(ADbg_Case_t *Case_p);
static void ADbg_Done_System_AuthenticationChallengeImpl(ADbg_Case_t *Case_p);
static void ADbg_Do_System_CollectDataImpl(ADbg_Case_t *Case_p);
//static void ADbg_Do_System_R15LoaderStarted(ADbg_Case_t* Case_p);
/*******************************************************************************
 * The test case definitions
 ******************************************************************************/
static uint8 Parameters1[3] = {2, SHORT, WORD};
ADBG_CASE_DEFINE(Test_Do_System_ShutDownImpl, 1, 2, ADbg_Do_System_ShutDownImpl, Parameters1);
static uint8 Parameters2[3] = {2, SHORT, WORD};
ADBG_CASE_DEFINE(Test_Do_System_SupportedCommandsImpl, 2, 2, ADbg_Do_System_SupportedCommandsImpl, Parameters2);
static uint8 Parameters5[4] = {3, SHORT, WORD, WORD};
ADBG_CASE_DEFINE(Test_Do_System_RebootImpl, 5, 2, ADbg_Do_System_RebootImpl, Parameters5);
static uint8 Parameters6[4] = {3, SHORT, WORD, WORD};
ADBG_CASE_DEFINE(Test_Do_System_AuthenticateImpl, 6, 2, ADbg_Do_System_AuthenticateImpl, Parameters6);
static uint8 Parameters7[4] = {3, SHORT, WORD, WORD};
ADBG_CASE_DEFINE(Test_Done_System_LoaderStartUpStatusImpl, 7, 2, ADbg_Done_System_LoaderStartUpStatusImpl, Parameters7);
static uint8 Parameters8[16] = {15, SHORT, WORD, CHARPTR, CHARPTR, CHARPTR, CHARPTR, CHARPTR, CHARPTR, CHARPTR, CHARPTR, CHARPTR, CHARPTR, CHARPTR, CHARPTR, WORD};
ADBG_CASE_DEFINE(Test_Done_System_GetControlKeysImpl, 8, 2, ADbg_Done_System_GetControlKeysImpl, Parameters8);
static uint8 Parameters9[6] = {5, SHORT, WORD, WORD, VOIDPTR, WORD};
ADBG_CASE_DEFINE(Test_Done_System_AuthenticationChallengeImpl, 9, 2, ADbg_Done_System_AuthenticationChallengeImpl, Parameters9);
static uint8 Parameters10[4] = {3, SHORT, WORD, WORD};
ADBG_CASE_DEFINE(Test_Do_System_CollectDataImpl, 10, 2, ADbg_Do_System_CollectDataImpl, Parameters10);
//static uint8 Parameters11[4] = {3, WORD, WORD, WORD};
//ADBG_CASE_DEFINE(Test_Do_System_R15LoaderStarted, 11, 2, ADbg_Do_System_R15LoaderStarted, Parameters11);
/*******************************************************************************
 * The test case suite definition
 ******************************************************************************/
ADBG_SUITE_DEFINE_BEGIN(TestCase_CNH1605195, 0, 2)

ADBG_SUITE_ENTRY(Test_Do_System_ShutDownImpl)
ADBG_SUITE_ENTRY(Test_Do_System_SupportedCommandsImpl)
ADBG_SUITE_ENTRY(Test_Do_System_RebootImpl)
ADBG_SUITE_ENTRY(Test_Do_System_AuthenticateImpl)
ADBG_SUITE_ENTRY(Test_Done_System_LoaderStartUpStatusImpl)
ADBG_SUITE_ENTRY(Test_Done_System_GetControlKeysImpl)
ADBG_SUITE_ENTRY(Test_Done_System_AuthenticationChallengeImpl)
ADBG_SUITE_ENTRY(Test_Do_System_CollectDataImpl)
//ADBG_SUITE_ENTRY(Test_Do_System_R15LoaderStarted)

ADBG_SUITE_DEFINE_END();

/*******************************************************************************
 * Case runner
 ******************************************************************************/
void Do_CNH1605195_Module_Test_Run(ADbg_MainModule_t *MainModule_p)
{
    ADbg_Module_t *Module_p = Do_ADbg_Module_Create(TestCase_CNH1605195, NULL, MainModule_p->Command_p, MainModule_p->Result_p);
    Do_ADbg_InModule_Command(Module_p);
    Do_ADbg_Module_Destroy(Module_p);
}
/*******************************************************************************
 * Test cases
 ******************************************************************************/

/**
 * This function will test function:
 * Do_System_ShutDownImpl
 * Function used in: Test_Do_System_ShutDownImpl
 *
 * @param [in] Case_p          Pointer
 * @return void
 */
static void ADbg_Do_System_ShutDownImpl(ADbg_Case_t *Case_p)
{
    uint16 Session = 0;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Do_ADbg_GetDataVar(sizeof(uint16), &Var_p, &Session);

    Do_ADbg_GetDataVar(sizeof(uint32), &Var_p, &ExpectedResult);

    (void)Do_System_ShutDownImpl(Session);
}

/**
 * This function will test function:
 * Do_System_SupportedCommandsImpl
 * Function used in: Test_Do_System_SupportedCommandsImpl
 *
 * @param [in] Case_p          Pointer
 * @return void
 */
static void ADbg_Do_System_SupportedCommandsImpl(ADbg_Case_t *Case_p)
{
    uint16 Session = 0;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Do_ADbg_GetDataVar(sizeof(uint16), &Var_p, &Session);

    Do_ADbg_GetDataVar(sizeof(uint32), &Var_p, &ExpectedResult);

    (void)Do_System_SupportedCommandsImpl(Session);
}

/**
 * This function will test function:
 * Do_System_RebootImpl
 * Function used in: Test_Do_System_RebootImpl
 *
 * @param [in] Case_p          Pointer
 * @return void
 */
static void ADbg_Do_System_RebootImpl(ADbg_Case_t *Case_p)
{
    uint16 Session = 0;
    uint32 Mode = 0;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Do_ADbg_GetDataVar(sizeof(uint16), &Var_p, &Session);
    Do_ADbg_GetDataVar(sizeof(uint32), &Var_p, &Mode);

    Do_ADbg_GetDataVar(sizeof(uint32), &Var_p, &ExpectedResult);

    (void)Do_System_RebootImpl(Session, Mode);
}

/**
 * This function will test function:
 * Do_System_AuthenticateImpl
 * Function used in: Test_Do_System_AuthenticateImpl
 *
 * @param [in] Case_p          Pointer
 * @return void
 */
static void ADbg_Do_System_AuthenticateImpl(ADbg_Case_t *Case_p)
{
    uint16 Session = 0;
    uint32 Type = 0;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Do_ADbg_GetDataVar(sizeof(uint16), &Var_p, &Session);
    Do_ADbg_GetDataVar(sizeof(uint32), &Var_p, &Type);

    Do_ADbg_GetDataVar(sizeof(uint32), &Var_p, &ExpectedResult);

    (void)Do_System_AuthenticateImpl(Session, Type);
}

/**
 * This function will test function:
 * Done_System_LoaderStartUpStatusImpl
 * Function used in: Test_Done_System_LoaderStartUpStatusImpl
 *
 * @param [in] Case_p          Pointer
 * @return void
 */
static void ADbg_Done_System_LoaderStartUpStatusImpl(ADbg_Case_t *Case_p)
{
    uint16 Session = 0;
    ErrorCode_e Status = E_GENERAL_FATAL_ERROR;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Do_ADbg_GetDataVar(sizeof(uint16), &Var_p, &Session);
    Do_ADbg_GetDataVar(sizeof(ErrorCode_e), &Var_p, &Status);

    Do_ADbg_GetDataVar(sizeof(uint32), &Var_p, &ExpectedResult);

    (void)Done_System_LoaderStartUpStatusImpl(Session, Status);
}

/**
 * This function will test function:
 * Done_System_GetControlKeysImpl
 * Function used in: Test_Done_System_GetControlKeysImpl
 *
 * @param [in] Case_p          Pointer
 * @return void
 */
static void ADbg_Done_System_GetControlKeysImpl(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_GENERAL_FATAL_ERROR;
    uint32 AssertStatus = 1;
    uint16 Session = 0;
    ErrorCode_e Status = E_GENERAL_FATAL_ERROR;
    char *NLCKLock_p = NULL;
    char *NSLCKLock_p = NULL;
    char *SPLCKLock_p = NULL;
    char *CLCKLock_p = NULL;
    char *PCKLock_p = NULL;
    char *ESLCKLock_p = NULL;
    char *NLCKUnlock_p = NULL;
    char *NSLCKUnlock_p = NULL;
    char *SPLCKUnlock_p = NULL;
    char *CLCKUnlock_p = NULL;
    char *PCKUnlock_p = NULL;
    char *ESLCKUnlock_p = NULL;
    uint32 NLCKLockLength = 0;
    uint32 NSLCKLockLength = 0;
    uint32 SPLCKLockLength = 0;
    uint32 CLCKLockLength = 0;
    uint32 PCKLockLength = 0;
    uint32 ESLCKLockLength = 0;
    uint32 NLCKUnlockLength = 0;
    uint32 NSLCKUnlockLength = 0;
    uint32 SPLCKUnlockLength = 0;
    uint32 CLCKUnlockLength = 0;
    uint32 PCKUnlockLength = 0;
    uint32 ESLCKUnlockLength = 0;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Do_ADbg_GetDataVar(sizeof(uint16), &Var_p, &Session);
    Do_ADbg_GetDataVar(sizeof(ErrorCode_e), &Var_p, &Status);

    NLCKLockLength = get_uint32_string_le((void *)&Var_p);
    NLCKLock_p = (char *)Do_ADbg_GetDataPointer(NLCKLockLength, (void *)&Var_p);

    NSLCKLockLength = get_uint32_string_le((void *)&Var_p);
    NSLCKLock_p = (char *)Do_ADbg_GetDataPointer(NSLCKLockLength, (void *)&Var_p);

    SPLCKLockLength = get_uint32_string_le((void *)&Var_p);
    SPLCKLock_p = (char *)Do_ADbg_GetDataPointer(SPLCKLockLength, (void *)&Var_p);

    CLCKLockLength = get_uint32_string_le((void *)&Var_p);
    CLCKLock_p = (char *)Do_ADbg_GetDataPointer(CLCKLockLength, (void *)&Var_p);

    PCKLockLength = get_uint32_string_le((void *)&Var_p);
    PCKLock_p = (char *)Do_ADbg_GetDataPointer(PCKLockLength, (void *)&Var_p);

    ESLCKLockLength = get_uint32_string_le((void *)&Var_p);
    ESLCKLock_p = (char *)Do_ADbg_GetDataPointer(ESLCKLockLength, (void *)&Var_p);

    NLCKUnlockLength = get_uint32_string_le((void *)&Var_p);
    NLCKUnlock_p = (char *)Do_ADbg_GetDataPointer(NLCKUnlockLength, (void *)&Var_p);

    NSLCKUnlockLength = get_uint32_string_le((void *)&Var_p);
    NSLCKUnlock_p = (char *)Do_ADbg_GetDataPointer(NSLCKUnlockLength, (void *)&Var_p);

    SPLCKUnlockLength = get_uint32_string_le((void *)&Var_p);
    SPLCKUnlock_p = (char *)Do_ADbg_GetDataPointer(SPLCKUnlockLength, (void *)&Var_p);

    CLCKUnlockLength = get_uint32_string_le((void *)&Var_p);
    CLCKUnlock_p = (char *)Do_ADbg_GetDataPointer(CLCKUnlockLength, (void *)&Var_p);

    PCKUnlockLength = get_uint32_string_le((void *)&Var_p);
    PCKUnlock_p = (char *)Do_ADbg_GetDataPointer(PCKUnlockLength, (void *)&Var_p);

    ESLCKUnlockLength = get_uint32_string_le((void *)&Var_p);
    ESLCKUnlock_p = (char *)Do_ADbg_GetDataPointer(ESLCKUnlockLength, (void *)&Var_p);

    Do_ADbg_GetDataVar(sizeof(uint32), &Var_p, &ExpectedResult);

    Result = Done_System_GetControlKeysImpl(Session, Status, NLCKLock_p, NSLCKLock_p,
                                            SPLCKLock_p, CLCKLock_p,
                                            PCKLock_p, ESLCKLock_p,
                                            NLCKUnlock_p, NSLCKUnlock_p,
                                            SPLCKUnlock_p, CLCKUnlock_p,
                                            PCKUnlock_p, ESLCKUnlock_p);

    if (ExpectedResult == Result) {
        AssertStatus = 0;
    }

    ReleaseADbg(AssertStatus);
    BUFFER_FREE(NLCKLock_p);
    BUFFER_FREE(NSLCKLock_p);
    BUFFER_FREE(SPLCKLock_p);
    BUFFER_FREE(CLCKLock_p);
    BUFFER_FREE(PCKLock_p);
    BUFFER_FREE(ESLCKLock_p);
    BUFFER_FREE(NLCKUnlock_p);
    BUFFER_FREE(NSLCKUnlock_p);
    BUFFER_FREE(SPLCKUnlock_p);
    BUFFER_FREE(CLCKUnlock_p);
    BUFFER_FREE(PCKUnlock_p);
    BUFFER_FREE(ESLCKUnlock_p);

}


/**
 * This function will test function:
 * Done_System_AuthenticationChallengeImpl
 * Function used in: Test_Done_System_AuthenticationChallengeImpl
 *
 * @param [in] Case_p          Pointer
 * @return void
 */
static void ADbg_Done_System_AuthenticationChallengeImpl(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_GENERAL_FATAL_ERROR;
    uint32 AssertStatus = 1;
    uint16 Session = 0;
    ErrorCode_e Status = E_GENERAL_FATAL_ERROR;
    uint32 UpdatedAuthChallengeBlockLength = 0;
    void *UpdatedAuthChallengeBlock_p = NULL;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Do_ADbg_GetDataVar(sizeof(uint16), &Var_p, &Session);
    Do_ADbg_GetDataVar(sizeof(ErrorCode_e), &Var_p, &Status);
    Do_ADbg_GetDataVar(sizeof(uint32), &Var_p, &UpdatedAuthChallengeBlockLength);

    UpdatedAuthChallengeBlock_p = Do_ADbg_GetDataPointer(UpdatedAuthChallengeBlockLength, (void **)&Var_p);

    Do_ADbg_GetDataVar(sizeof(uint32), &Var_p, &ExpectedResult);

    Result = Done_System_AuthenticationChallengeImpl(Session, Status, UpdatedAuthChallengeBlockLength, UpdatedAuthChallengeBlock_p);

    if (ExpectedResult == Result) {
        AssertStatus = 0;
    }

    ReleaseADbg(AssertStatus);

    BUFFER_FREE(UpdatedAuthChallengeBlock_p);
}

/**
 * This function will test function:
 * Do_System_CollectDataImpl
 * Function used in: Test_Do_System_CollectDataImpl
 *
 * @param [in] Case_p          Pointer
 * @return void
 */
static void ADbg_Do_System_CollectDataImpl(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_GENERAL_FATAL_ERROR;
    uint32 AssertStatus = 1;
    uint16 Session = 0;
    uint32 Type = 0;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Do_ADbg_GetDataVar(sizeof(uint16), &Var_p, &Session);
    Do_ADbg_GetDataVar(sizeof(uint32), &Var_p, &Type);

    Do_ADbg_GetDataVar(sizeof(uint32), &Var_p, &ExpectedResult);

    Result = Do_System_CollectDataImpl(Session, Type);

    if (ExpectedResult == Result) {
        AssertStatus = 0;
    }

    ReleaseADbg(AssertStatus);
}

/**
 * This function will test function:
 * Do_System_R15LoaderStarted
 * Function used in: Test_Do_System_R15LoaderStarted
 *
 * @param [in] Case_p          Pointer
 * @return void
 */
/*
static void ADbg_Do_System_R15LoaderStarted(ADbg_Case_t* Case_p)
{
 ErrorCode_e Result = E_SUCCESS;
 ErrorCode_e ErrorCode = E_SUCCESS;
 Communication_t *Communication_p = NULL;
 uint8* Var_p = NULL;

 Var_p = Case_p->Command_p->Data_p;
 Do_ADbg_GetDataVar(sizeof(ErrorCode_e), &Var_p, &ErrorCode);

 Communication_p = (Communication_t *)Do_ADbg_GetDataPointer (sizeof(Communication_t), &Var_p);

 Result = Do_System_R15LoaderStarted(ErrorCode, Communication_p);
 Do_ADbg_Assert((ErrorCode_e)(*Var_p) == Result, Case_p);

 BUFFER_FREE(Communication_p);
}*/

/*@}*/
