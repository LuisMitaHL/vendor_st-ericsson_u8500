/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/

/**
 * @file ADbg_cnh1606344.c
 *
 * @brief Test cases for CNH1606344 module.
 *
 * This file consist of test cases for autometic testing functions from
 * CNH1606344 module.
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
#include "r_communication_service.h"
#include "r_memory_utils.h"

/*******************************************************************************
 * The test case functions declarations
 ******************************************************************************/
// A2_Family

// Communication_service
static void ADbg_Do_Communication_Initialize(ADbg_Case_t *Case_p);
static void ADbg_Do_Communication_Shutdown(ADbg_Case_t *Case_p);
static void ADbg_Do_Communication_SetFamily(ADbg_Case_t *Case_p);
static void ADbg_Do_Communication_Poll(ADbg_Case_t *Case_p);
static void ADbg_Do_Communication_Send(ADbg_Case_t *Case_p);
static void ADbg_Do_Communication_SetProtocolTimeouts(ADbg_Case_t *Case_p);
static void ADbg_Do_Communication_GetProtocolTimeouts(ADbg_Case_t *Case_p);

// Protrom_family

/*******************************************************************************
 * The test case definitions
 ******************************************************************************/
// A2_Family

// Communication_service
static uint8 Parameters2_1[11] = {10, VOIDPTR, WORDPTR, WORD, WORDPTR, WORDPTR, WORD, WORDPTR, WORDPTR, WORDPTR, WORD};
ADBG_CASE_DEFINE(Test_Do_Communication_Initialize, 1, 15, ADbg_Do_Communication_Initialize, Parameters2_1);
static uint8 Parameters2_2[3] = {2, WORDPTR, WORD};
ADBG_CASE_DEFINE(Test_Do_Communication_Shutdown, 2, 15, ADbg_Do_Communication_Shutdown, Parameters2_2);
static uint8 Parameters2_3[5] = {4, WORDPTR, WORD, WORD, WORD};
ADBG_CASE_DEFINE(Test_Do_Communication_SetFamily, 3, 15, ADbg_Do_Communication_SetFamily, Parameters2_3);
static uint8 Parameters2_4[2] = {1, WORDPTR};
ADBG_CASE_DEFINE(Test_Do_Communication_Poll, 4, 15, ADbg_Do_Communication_Poll, Parameters2_4);
static uint8 Parameters2_5[4] = {3, WORDPTR, VOIDPTR, WORD};
ADBG_CASE_DEFINE(Test_Do_Communication_Send, 5, 15, ADbg_Do_Communication_Send, Parameters2_5);
static uint8 Parameters2_6[4] = {3, WORDPTR, VOIDPTR, WORD};
ADBG_CASE_DEFINE(Test_Do_Communication_SetProtocolTimeouts, 6, 15, ADbg_Do_Communication_SetProtocolTimeouts, Parameters2_6);
static uint8 Parameters2_7[4] = {3, WORDPTR, VOIDPTR, WORD};
ADBG_CASE_DEFINE(Test_Do_Communication_GetProtocolTimeouts, 7, 15, ADbg_Do_Communication_GetProtocolTimeouts, Parameters2_7);

/*******************************************************************************
* The test case suite definition
******************************************************************************/
ADBG_SUITE_DEFINE_BEGIN(TestCase_CNH1606344, 0, 15)

// A2_Family

// Communication_service
ADBG_SUITE_ENTRY(Test_Do_Communication_Initialize)
ADBG_SUITE_ENTRY(Test_Do_Communication_Shutdown)
ADBG_SUITE_ENTRY(Test_Do_Communication_SetFamily)
ADBG_SUITE_ENTRY(Test_Do_Communication_Poll)
ADBG_SUITE_ENTRY(Test_Do_Communication_Send)
ADBG_SUITE_ENTRY(Test_Do_Communication_SetProtocolTimeouts)
ADBG_SUITE_ENTRY(Test_Do_Communication_GetProtocolTimeouts)

ADBG_SUITE_DEFINE_END();

/*******************************************************************************
 * Case runner
 ******************************************************************************/
void Do_CNH1606344_Module_Test_Run(ADbg_MainModule_t *MainModule_p)
{
    ADbg_Module_t *Module_p = Do_ADbg_Module_Create(TestCase_CNH1606344, NULL, MainModule_p->Command_p, MainModule_p->Result_p);
    Do_ADbg_InModule_Command(Module_p);
    Do_ADbg_Module_Destroy(Module_p);
}
/*******************************************************************************
 * Test cases
 ******************************************************************************/
// A2_Family

// Communication_service
/**
 * This function will test function:
 * Do_Communication_Initialize.
 * Function used in: Test_Do_Communication_Initialize.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_Do_Communication_Initialize(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_SUCCESS;
    void *Object_p;
    Communication_t **Communication_pp;
    Family_t Family;
    HashDevice_t *HashDevice_p;
    CommunicationDevice_t *CommunicationDevice_p;
    Do_CEH_Call_t CommandCallback_p;
    BuffersInterface_t *Buffers_p;
    TimersInterface_t *Timers_p;
    QueueInterface_t *Queue_p;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Object_p = Do_ADbg_GetDataPointer(sizeof(void *), (void **)&Var_p);
    Communication_pp = Do_ADbg_GetDataPointer(sizeof(Communication_t *), (void **)&Var_p);

    Do_ADbg_GetDataVar(sizeof(Family_t), &Var_p, &Family);

    HashDevice_p = Do_ADbg_GetDataPointer(sizeof(HashDevice_t), (void **)&Var_p);
    CommunicationDevice_p = Do_ADbg_GetDataPointer(sizeof(CommunicationDevice_t), (void **)&Var_p);

    Do_ADbg_GetDataVar(sizeof(Do_CEH_Call_t), &Var_p, &CommandCallback_p);
    Buffers_p = Do_ADbg_GetDataPointer(sizeof(BuffersInterface_t), (void **)&Var_p);
    Timers_p = Do_ADbg_GetDataPointer(sizeof(TimersInterface_t), (void **)&Var_p);
    Queue_p = Do_ADbg_GetDataPointer(sizeof(QueueInterface_t), (void **)&Var_p);

    Result = Do_Communication_Initialize(Object_p, Communication_pp, Family, HashDevice_p, CommunicationDevice_p, CommandCallback_p, Buffers_p, Timers_p, Queue_p);
    Do_ADbg_Assert((ErrorCode_e)(*Var_p) == Result, Case_p);

    BUFFER_FREE(*Communication_pp);
    BUFFER_FREE(HashDevice_p);
    BUFFER_FREE(CommunicationDevice_p);

}

/**
 * This function will test function:
 * Do_Communication_Shutdown.
 * Function used in: Test_Do_Communication_Shutdown.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_Do_Communication_Shutdown(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_SUCCESS;
    Communication_t **Communication_p;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Communication_p = Do_ADbg_GetDataPointer(sizeof(Communication_t), (void **)&Var_p);

    Result = Do_Communication_Shutdown(Communication_p);
    Do_ADbg_Assert((ErrorCode_e)(*Var_p) == Result, Case_p);

    BUFFER_FREE(Communication_p);
}

/**
 * This function will test function:
 * Do_Communication_SetFamily.
 * Function used in: Test_Do_Communication_SetFamily.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_Do_Communication_SetFamily(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_SUCCESS;
    Communication_t *Communication_p;
    Family_t Family;
    uint8 *Var_p = NULL;
    Do_CEH_Call_t CEHCallback = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Communication_p = Do_ADbg_GetDataPointer(sizeof(Communication_t), (void **)&Var_p);

    Do_ADbg_GetDataVar(sizeof(Family_t), &Var_p, &Family);
    Do_ADbg_GetDataVar(sizeof(Do_CEH_Call_t), &Var_p, &CEHCallback);

    Result = Do_Communication_SetFamily(Communication_p, Family, CEHCallback);
    Do_ADbg_Assert((ErrorCode_e)(*Var_p) == Result, Case_p);

    BUFFER_FREE(Communication_p);
}

/**
 * This function will test function:
 * Do_Communication_Poll.
 * Function used in: Test_Do_Communication_Poll.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_Do_Communication_Poll(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_SUCCESS;
    Communication_t *Communication_p;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Communication_p = Do_ADbg_GetDataPointer(sizeof(Communication_t), (void **)&Var_p);

    Do_Communication_Poll(Communication_p);
    Do_ADbg_Assert((ErrorCode_e)(*Var_p) == Result, Case_p);

    BUFFER_FREE(Communication_p);
}

/**
 * This function will test function:
 * Do_Communication_Send.
 * Function used in: Test_Do_Communication_Send.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_Do_Communication_Send(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_SUCCESS;
    Communication_t *Communication_p;
    void *InputData_p = NULL;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Communication_p = Do_ADbg_GetDataPointer(sizeof(Communication_t), (void **)&Var_p);
    InputData_p = Do_ADbg_GetDataPointer(sizeof(void *), (void **)&Var_p);

    Result = Do_Communication_Send(Communication_p, InputData_p);
    Do_ADbg_Assert((ErrorCode_e)(*Var_p) == Result, Case_p);

    BUFFER_FREE(Communication_p);
    BUFFER_FREE(InputData_p);

}

/**
 * This function will test function:
 * Do_Communication_SetProtocolTimeouts.
 * Function used in: Test_Do_Communication_SetProtocolTimeouts.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_Do_Communication_SetProtocolTimeouts(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_SUCCESS;
    Communication_t *Communication_p;
    void *TimeoutData_p = NULL;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Communication_p = Do_ADbg_GetDataPointer(sizeof(Communication_t), (void **)&Var_p);
    TimeoutData_p = Do_ADbg_GetDataPointer(sizeof(void *), (void **)&Var_p);

    Result = Do_Communication_SetProtocolTimeouts(Communication_p, TimeoutData_p);
    Do_ADbg_Assert((ErrorCode_e)(*Var_p) == Result, Case_p);

    BUFFER_FREE(Communication_p);
    BUFFER_FREE(TimeoutData_p);
}

/**
 * This function will test function:
 * Do_Communication_GetProtocolTimeouts.
 * Function used in: Test_Do_Communication_GetProtocolTimeouts.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_Do_Communication_GetProtocolTimeouts(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_SUCCESS;
    Communication_t *Communication_p;
    void *TimeoutData_p = NULL;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Communication_p = Do_ADbg_GetDataPointer(sizeof(Communication_t), (void **)&Var_p);
    TimeoutData_p = Do_ADbg_GetDataPointer(sizeof(void *), (void **)&Var_p);

    Result = Do_Communication_GetProtocolTimeouts(Communication_p, &TimeoutData_p);
    Do_ADbg_Assert((ErrorCode_e)(*Var_p) == Result, Case_p);

    BUFFER_FREE(Communication_p);
    BUFFER_FREE(TimeoutData_p);
}
