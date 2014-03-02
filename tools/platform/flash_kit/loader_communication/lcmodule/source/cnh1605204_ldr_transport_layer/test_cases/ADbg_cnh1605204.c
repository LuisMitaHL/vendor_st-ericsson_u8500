/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/

/**
 * @file ADbg_cnh<ModuleNumber>.c
 *
 * @brief Test cases for CNH1605204 module.
 *
 * This file consist of test cases for autometic testing functions from
 * CNH1605204 module.
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
#ifndef CFG_ENABLE_LOADER_TYPE
#include "r_a2_protocol.h"
#endif // CFG_ENABLE_LOADER_TYPE
#include "r_bulk_protocol.h"
#include "r_command_protocol.h"
#include "r_r15_transport_layer.h"
#include <stdlib.h>

/*******************************************************************************
 * The test case functions declarations
 ******************************************************************************/
#ifndef CFG_ENABLE_LOADER_TYPE
static void ADbg_Do_A2_Command_Send(ADbg_Case_t *Case_p);
#endif // CFG_ENABLE_LOADER_TYPE

static void ADbg_Do_R15_Bulk_OpenSession(ADbg_Case_t *Case_p);
static void ADbg_Do_R15_Bulk_CreateVector(ADbg_Case_t *Case_p);
static void ADbg_Do_R15_Bulk_DestroyVector(ADbg_Case_t *Case_p);
static void ADbg_Do_R15_Bulk_StartSession(ADbg_Case_t *Case_p);
static void ADbg_Do_R15_Bulk_GetStatusSession(ADbg_Case_t *Case_p);
static void ADbg_Do_R15_Bulk_CloseSession(ADbg_Case_t *Case_p);
static void ADbg_R15_Bulk_Process(ADbg_Case_t *Case_p);

static void ADbg_Do_R15_Command_ResetSessionCounters(ADbg_Case_t *Case_p);
static void ADbg_Do_R15_Command_Send(ADbg_Case_t *Case_p);
static void ADbg_R15_Command_Process(ADbg_Case_t *Case_p);

static void ADbg_R15_Transport_Initialize(ADbg_Case_t *Case_p);
static void ADbg_R15_Transport_Shutdown(ADbg_Case_t *Case_p);
static void ADbg_R15_Transport_Poll(ADbg_Case_t *Case_p);
static void ADbg_R15_Transport_Send(ADbg_Case_t *Case_p);
static void ADbg_R15_SetProtocolTimeouts(ADbg_Case_t *Case_p);
static void ADbg_R15_GetProtocolTimeouts(ADbg_Case_t *Case_p);

/*******************************************************************************
 * The test case definitions
 ******************************************************************************/
#ifndef CFG_ENABLE_LOADER_TYPE
// A2_protocol
static uint8 Parameters1_1[4] = {3, WORDPTR, WORDPTR, WORD};
ADBG_CASE_DEFINE(Test_Do_A2_Command_Send, 1, 8, ADbg_Do_A2_Command_Send, Parameters1_1);
#endif // CFG_ENABLE_LOADER_TYPE

// Bulk_protocol
static uint8 Parameters3_1[5] = {4, WORDPTR, SHORT, WORD, WORD};
ADBG_CASE_DEFINE(Test_Do_R15_Bulk_OpenSession, 2, 8, ADbg_Do_R15_Bulk_OpenSession, Parameters3_1);
static uint8 Parameters3_2[7] = {6, WORDPTR, WORD, WORD, WORD, WORDPTR, WORDPTR};
ADBG_CASE_DEFINE(Test_Do_R15_Bulk_CreateVector, 3, 8, ADbg_Do_R15_Bulk_CreateVector, Parameters3_2);
static uint8 Parameters3_3[5] = {4, WORDPTR, WORDPTR, BYTE, WORD};
ADBG_CASE_DEFINE(Test_Do_R15_Bulk_DestroyVector, 4, 8, ADbg_Do_R15_Bulk_DestroyVector, Parameters3_3);
static uint8 Parameters3_5[5] = {4, WORDPTR, WORDPTR, DOUBLEWORD, WORD};
ADBG_CASE_DEFINE(Test_Do_R15_Bulk_StartSession, 6, 8, ADbg_Do_R15_Bulk_StartSession, Parameters3_5);
static uint8 Parameters3_6[3] = {2, WORDPTR, WORD};
ADBG_CASE_DEFINE(Test_Do_R15_Bulk_GetStatusSession, 7, 8, ADbg_Do_R15_Bulk_GetStatusSession, Parameters3_6);
static uint8 Parameters3_7[4] = {3, WORDPTR, WORDPTR, WORD};
ADBG_CASE_DEFINE(Test_Do_R15_Bulk_CloseSession, 8, 8, ADbg_Do_R15_Bulk_CloseSession, Parameters3_7);
static uint8 Parameters3_8[4] = {3, WORDPTR, WORDPTR, WORD};
ADBG_CASE_DEFINE(Test_R15_Bulk_Process, 9, 8, ADbg_R15_Bulk_Process, Parameters3_8);

// Command_protocol
static uint8 Parameters4_1[3] = {2, WORDPTR, WORD};
ADBG_CASE_DEFINE(Test_Do_R15_Command_ResetSessionCounters, 10, 8, ADbg_Do_R15_Command_ResetSessionCounters, Parameters4_1);
static uint8 Parameters4_2[4] = {3, WORDPTR, WORDPTR, WORD};
ADBG_CASE_DEFINE(Test_Do_R15_Command_Send, 11, 8, ADbg_Do_R15_Command_Send, Parameters4_2);
static uint8 Parameters4_3[4] = {3, WORDPTR, WORDPTR, WORD};
ADBG_CASE_DEFINE(Test_R15_Command_Process, 12, 8, ADbg_R15_Command_Process, Parameters4_3);

//Protrom_protocol

// R15_transport_layer
static uint8 Parameters7_1[3] = {2, WORDPTR, WORD};
ADBG_CASE_DEFINE(Test_R15_Transport_Initialize, 13, 8, ADbg_R15_Transport_Initialize, Parameters7_1);
static uint8 Parameters7_2[3] = {2, WORDPTR, WORD};
ADBG_CASE_DEFINE(Test_R15_Transport_Shutdown, 14, 8, ADbg_R15_Transport_Shutdown, Parameters7_2);
static uint8 Parameters7_3[3] = {2, WORDPTR, WORD};
ADBG_CASE_DEFINE(Test_R15_Transport_Poll, 15, 8, ADbg_R15_Transport_Poll, Parameters7_3);
static uint8 Parameters7_4[4] = {3, WORDPTR, VOIDPTR, WORD};
ADBG_CASE_DEFINE(Test_R15_Transport_Send, 16, 8, ADbg_R15_Transport_Send, Parameters7_4);
static uint8 Parameters7_5[4] = {3, WORDPTR, VOIDPTR, WORD};
ADBG_CASE_DEFINE(Test_R15_SetProtocolTimeouts, 17, 8, ADbg_R15_SetProtocolTimeouts, Parameters7_5);
static uint8 Parameters7_6[4] = {3, WORDPTR, VOIDPTR, WORD};
ADBG_CASE_DEFINE(Test_R15_GetProtocolTimeouts, 18, 8, ADbg_R15_GetProtocolTimeouts, Parameters7_6);

/*******************************************************************************
* The test case suite definition
******************************************************************************/
ADBG_SUITE_DEFINE_BEGIN(TestCase_CNH1605204, 0, 8)

#ifndef CFG_ENABLE_LOADER_TYPE
ADBG_SUITE_ENTRY(Test_Do_A2_Command_Send)
#endif // CFG_ENABLE_LOADER_TYPE

ADBG_SUITE_ENTRY(Test_Do_R15_Bulk_OpenSession)
ADBG_SUITE_ENTRY(Test_Do_R15_Bulk_CreateVector)
ADBG_SUITE_ENTRY(Test_Do_R15_Bulk_DestroyVector)
ADBG_SUITE_ENTRY(Test_Do_R15_Bulk_GetStatusSession)
ADBG_SUITE_ENTRY(Test_Do_R15_Bulk_CloseSession)
ADBG_SUITE_ENTRY(Test_R15_Bulk_Process)

ADBG_SUITE_ENTRY(Test_Do_R15_Command_ResetSessionCounters)
ADBG_SUITE_ENTRY(Test_Do_R15_Command_Send)
ADBG_SUITE_ENTRY(Test_R15_Command_Process)

ADBG_SUITE_ENTRY(Test_R15_Transport_Initialize)
ADBG_SUITE_ENTRY(Test_R15_Transport_Shutdown)
ADBG_SUITE_ENTRY(Test_R15_Transport_Poll)
ADBG_SUITE_ENTRY(Test_R15_Transport_Send)
ADBG_SUITE_ENTRY(Test_R15_SetProtocolTimeouts)
ADBG_SUITE_ENTRY(Test_R15_GetProtocolTimeouts)

ADBG_SUITE_DEFINE_END();

/*******************************************************************************
 * Case runner
 ******************************************************************************/
void Do_CNH1605204_Module_Test_Run(ADbg_MainModule_t *MainModule_p)
{
    ADbg_Module_t *Module_p = Do_ADbg_Module_Create(TestCase_CNH1605204, NULL, MainModule_p->Command_p, MainModule_p->Result_p);
    Do_ADbg_InModule_Command(Module_p);
    Do_ADbg_Module_Destroy(Module_p);
}
/*******************************************************************************
 * Test cases
 ******************************************************************************/
#ifndef CFG_ENABLE_LOADER_TYPE
/**
 * This function will test function:
 * CNH1605204_TestedFunction1.
 * Function used in: Test_CNH1605204_TestedFunction1.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_Do_A2_Command_Send(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_SUCCESS;
    Communication_t *Communication_p;
    CommandData_t *CmdData_p;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Communication_p = Do_ADbg_GetDataPointer(sizeof(Communication_t), (void **)&Var_p);
    CmdData_p = Do_ADbg_GetDataPointer(sizeof(CommandData_t), (void **)&Var_p);

    Result = Do_A2_Command_Send(Communication_p, CmdData_p);
    Do_ADbg_Assert((ErrorCode_e)(*Var_p) == Result, Case_p);

    if (NULL != Communication_p) {
        free(Communication_p);
    }

    if (NULL != CmdData_p) {
        free(CmdData_p);
    }
}
#endif // CFG_ENABLE_LOADER_TYPE

/**
 * This function will test function:
 * Do_R15_Bulk_OpenSession.
 * Function used in: Test_Do_R15_Bulk_OpenSession.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_Do_R15_Bulk_OpenSession(ADbg_Case_t *Case_p)
{
    uint32 Result = BULK_SESSION_OPEN;
    Communication_t *Communication_p;
    uint16 SessionId = 0;
    TL_SessionMode_t Mode;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Communication_p = Do_ADbg_GetDataPointer(sizeof(Communication_t), (void **)&Var_p);

    Do_ADbg_GetDataVar(sizeof(uint16), &Var_p, &SessionId);
    Do_ADbg_GetDataVar(sizeof(TL_SessionMode_t), &Var_p, &Mode);

    Result = Do_R15_Bulk_OpenSession(Communication_p, SessionId, Mode);
    Do_ADbg_Assert((ErrorCode_e)(*Var_p) == Result, Case_p);

    if (NULL != Communication_p) {
        free(Communication_p);
    }
}

/**
 * This function will test function:
 * Do_R15_Bulk_CreateVector.
 * Function used in: Test_Do_R15_Bulk_CreateVector.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_Do_R15_Bulk_CreateVector(ADbg_Case_t *Case_p)
{
    TL_BulkVectorList_t *Result = NULL;
    Communication_t *Communication_p;
    uint32 BulkVector = 0;
    uint32 Length = 0;
    uint32 BuffSize = 0;
    TL_BulkVectorList_t *CreatedBulkVector_p;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Communication_p = Do_ADbg_GetDataPointer(sizeof(Communication_t *), (void **)&Var_p);

    Do_ADbg_GetDataVar(sizeof(uint32), &Var_p, &BulkVector);
    Do_ADbg_GetDataVar(sizeof(uint32), &Var_p, &Length);
    Do_ADbg_GetDataVar(sizeof(uint32), &Var_p, &BuffSize);

    CreatedBulkVector_p = Do_ADbg_GetDataPointer(sizeof(TL_BulkVectorList_t *), (void **)&Var_p);

    Result = Do_R15_Bulk_CreateVector(Communication_p, BulkVector, Length, BuffSize, CreatedBulkVector_p);
    Do_ADbg_Assert((TL_BulkVectorList_t *)(*Var_p) == Result, Case_p);

    if (NULL != Communication_p) {
        free(Communication_p);
    }

    if (NULL != CreatedBulkVector_p) {
        free(CreatedBulkVector_p);
    }
}

/**
 * This function will test function:
 * Do_R15_Bulk_DestroyVector.
 * Function used in: Test_Do_R15_Bulk_DestroyVector.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_Do_R15_Bulk_DestroyVector(ADbg_Case_t *Case_p)
{
    uint32 Result = E_SUCCESS;
    Communication_t *Communication_p;
    TL_BulkVectorList_t *BulkVector_p;
    boolean ReqReleaseBuffer;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Communication_p = Do_ADbg_GetDataPointer(sizeof(Communication_t *), (void **)&Var_p);
    BulkVector_p = Do_ADbg_GetDataPointer(sizeof(TL_BulkVectorList_t *), (void **)&Var_p);

    Do_ADbg_GetDataVar(sizeof(boolean), &Var_p, &ReqReleaseBuffer);

    Result = Do_R15_Bulk_DestroyVector(Communication_p, BulkVector_p, ReqReleaseBuffer);
    Do_ADbg_Assert((uint32)(*Var_p) == Result, Case_p);

    if (NULL != Communication_p) {
        free(Communication_p);
    }

    if (NULL != BulkVector_p) {
        free(BulkVector_p);
    }
}

/**
 * This function will test function:
 * Do_R15_Bulk_StartSession.
 * Function used in: Test_Do_R15_Bulk_StartSession.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_Do_R15_Bulk_StartSession(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_SUCCESS;
    Communication_t *Communication_p;
    TL_BulkVectorList_t *BulkVector_p;
    uint64 Offset = 0;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Communication_p = Do_ADbg_GetDataPointer(sizeof(Communication_t *), (void **)&Var_p);
    BulkVector_p = Do_ADbg_GetDataPointer(sizeof(TL_BulkVectorList_t *), (void **)&Var_p);

    Do_ADbg_GetDataVar(sizeof(uint64), &Var_p, &Offset);

    Result = Do_R15_Bulk_StartSession(Communication_p, BulkVector_p, Offset);
    Do_ADbg_Assert((ErrorCode_e)(*Var_p) == Result, Case_p);

    if (NULL != Communication_p) {
        free(Communication_p);
    }

    if (NULL != BulkVector_p) {
        free(BulkVector_p);
    }
}

/**
 * This function will test function:
 * Do_R15_Bulk_GetStatusSession.
 * Function used in: Test_Do_R15_Bulk_GetStatusSession.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_Do_R15_Bulk_GetStatusSession(ADbg_Case_t *Case_p)
{
    TL_BulkSessionState_t Result = E_SUCCESS;
    TL_BulkVectorList_t *BulkVector_p;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    BulkVector_p = Do_ADbg_GetDataPointer(sizeof(TL_BulkVectorList_t *), (void **)&Var_p);

    Result = Do_R15_Bulk_GetStatusSession(BulkVector_p);
    Do_ADbg_Assert((TL_BulkSessionState_t)(*Var_p) == Result, Case_p);

    if (NULL != BulkVector_p) {
        free(BulkVector_p);
    }
}

/**
 * This function will test function:
 * Do_R15_Bulk_CloseSession.
 * Function used in: Test_Do_R15_Bulk_CloseSession.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_Do_R15_Bulk_CloseSession(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_SUCCESS;
    Communication_t *Communication_p;
    TL_BulkVectorList_t *BulkVector_p;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Communication_p = Do_ADbg_GetDataPointer(sizeof(Communication_t *), (void **)&Var_p);
    BulkVector_p = Do_ADbg_GetDataPointer(sizeof(TL_BulkVectorList_t *), (void **)&Var_p);

    Result = Do_R15_Bulk_CloseSession(Communication_p, BulkVector_p);
    Do_ADbg_Assert((ErrorCode_e)(*Var_p) == Result, Case_p);

    if (NULL != Communication_p) {
        free(Communication_p);
    }

    if (NULL != BulkVector_p) {
        free(BulkVector_p);
    }
}

/**
 * This function will test function:
 * R15_Bulk_Process.
 * Function used in: Test_R15_Bulk_Process.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_R15_Bulk_Process(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_SUCCESS;
    Communication_t *Communication_p;
    PacketMeta_t *Packet_p;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Communication_p = Do_ADbg_GetDataPointer(sizeof(Communication_t *), (void **)&Var_p);
    Packet_p = Do_ADbg_GetDataPointer(sizeof(PacketMeta_t *), (void **)&Var_p);

    Result = R15_Bulk_Process(Communication_p, Packet_p);
    Do_ADbg_Assert((ErrorCode_e)(*Var_p) == Result, Case_p);

    if (NULL != Communication_p) {
        free(Communication_p);
    }

    if (NULL != Packet_p) {
        free(Packet_p);
    }
}

/**
 * This function will test function:
 * Do_R15_Command_ResetSessionCounters.
 * Function used in: Test_Do_R15_Command_ResetSessionCounters.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_Do_R15_Command_ResetSessionCounters(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_SUCCESS;
    Communication_t *Communication_p;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Communication_p = Do_ADbg_GetDataPointer(sizeof(Communication_t *), (void **)&Var_p);

    Result = Do_R15_Command_ResetSessionCounters(Communication_p);
    Do_ADbg_Assert((ErrorCode_e)(*Var_p) == Result, Case_p);

    if (NULL != Communication_p) {
        free(Communication_p);
    }
}

/**
 * This function will test function:
 * Do_R15_Command_Send.
 * Function used in: Test_Do_R15_Command_Send.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_Do_R15_Command_Send(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_SUCCESS;
    Communication_t *Communication_p;
    CommandData_t *CmdData_p;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Communication_p = Do_ADbg_GetDataPointer(sizeof(Communication_t *), (void **)&Var_p);
    CmdData_p = Do_ADbg_GetDataPointer(sizeof(CommandData_t *), (void **)&Var_p);

    Result = Do_R15_Command_Send(Communication_p, CmdData_p);
    Do_ADbg_Assert((ErrorCode_e)(*Var_p) == Result, Case_p);

    if (NULL != Communication_p) {
        free(Communication_p);
    }

    if (NULL != CmdData_p) {
        free(CmdData_p);
    }
}

/**
 * This function will test function:
 * R15_Command_Process.
 * Function used in: Test_R15_Command_Process.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_R15_Command_Process(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_SUCCESS;
    Communication_t *Communication_p;
    PacketMeta_t *Packet_p;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Communication_p = Do_ADbg_GetDataPointer(sizeof(Communication_t *), (void **)&Var_p);
    Packet_p = Do_ADbg_GetDataPointer(sizeof(PacketMeta_t *), (void **)&Var_p);

    Result = R15_Command_Process(Communication_p, Packet_p);
    Do_ADbg_Assert((ErrorCode_e)(*Var_p) == Result, Case_p);

    if (NULL != Communication_p) {
        free(Communication_p);
    }

    if (NULL != Packet_p) {
        free(Packet_p);
    }
}

/**
 * This function will test function:
 * R15_Transport_Initialize.
 * Function used in: Test_R15_Transport_Initialize.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_R15_Transport_Initialize(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_SUCCESS;
    Communication_t *Communication_p;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Communication_p = Do_ADbg_GetDataPointer(sizeof(Communication_t *), (void **)&Var_p);

    Result = R15_Transport_Initialize(Communication_p);
    Do_ADbg_Assert((ErrorCode_e)(*Var_p) == Result, Case_p);

    if (NULL != Communication_p) {
        free(Communication_p);
    }
}

/**
 * This function will test function:
 * R15_Transport_Shutdown.
 * Function used in: Test_R15_Transport_Shutdown.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_R15_Transport_Shutdown(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_SUCCESS;
    Communication_t *Communication_p;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Communication_p = Do_ADbg_GetDataPointer(sizeof(Communication_t *), (void **)&Var_p);

    Result = R15_Transport_Shutdown(Communication_p);
    Do_ADbg_Assert((ErrorCode_e)(*Var_p) == Result, Case_p);

    if (NULL != Communication_p) {
        free(Communication_p);
    }
}

/**
 * This function will test function:
 * R15_Transport_Poll.
 * Function used in: Test_R15_Transport_Poll.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_R15_Transport_Poll(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_SUCCESS;
    Communication_t *Communication_p;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Communication_p = Do_ADbg_GetDataPointer(sizeof(Communication_t *), (void **)&Var_p);

    Result = R15_Transport_Poll(Communication_p);
    Do_ADbg_Assert((ErrorCode_e)(*Var_p) == Result, Case_p);

    if (NULL != Communication_p) {
        free(Communication_p);
    }
}

/**
 * This function will test function:
 * R15_Transport_Send.
 * Function used in: Test_R15_Transport_Send.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_R15_Transport_Send(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_SUCCESS;
    Communication_t *Communication_p;
    void *InputDataIn_p = NULL;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Communication_p = Do_ADbg_GetDataPointer(sizeof(Communication_t *), (void **)&Var_p);
    InputDataIn_p = Do_ADbg_GetDataPointer(sizeof(void *), (void **)&Var_p);

    Result = R15_Transport_Send(Communication_p, InputDataIn_p);
    Do_ADbg_Assert((ErrorCode_e)(*Var_p) == Result, Case_p);

    if (NULL != Communication_p) {
        free(Communication_p);
    }

    if (NULL != InputDataIn_p) {
        free(InputDataIn_p);
    }
}

/**
 * This function will test function:
 * R15_SetProtocolTimeouts.
 * Function used in: Test_R15_SetProtocolTimeouts.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_R15_SetProtocolTimeouts(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_SUCCESS;
    Communication_t *Communication_p;
    void *TimeoutData_p = NULL;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Communication_p = Do_ADbg_GetDataPointer(sizeof(Communication_t *), (void **)&Var_p);
    TimeoutData_p = Do_ADbg_GetDataPointer(sizeof(void *), (void **)&Var_p);

    Result = R15_SetProtocolTimeouts(Communication_p, TimeoutData_p);
    Do_ADbg_Assert((ErrorCode_e)(*Var_p) == Result, Case_p);

    if (NULL != Communication_p) {
        free(Communication_p);
    }

    if (NULL != TimeoutData_p) {
        free(TimeoutData_p);
    }
}

/**
 * This function will test function:
 * R15_GetProtocolTimeouts.
 * Function used in: Test_R15_GetProtocolTimeouts.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_R15_GetProtocolTimeouts(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_SUCCESS;
    Communication_t *Communication_p;
    void *TimeoutData_p = NULL;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Communication_p = Do_ADbg_GetDataPointer(sizeof(Communication_t *), (void **)&Var_p);
    TimeoutData_p = Do_ADbg_GetDataPointer(sizeof(void *), (void **)&Var_p);

    Result = R15_GetProtocolTimeouts(Communication_p, &TimeoutData_p);
    Do_ADbg_Assert((ErrorCode_e)(*Var_p) == Result, Case_p);

    if (NULL != Communication_p) {
        free(Communication_p);
    }

    if (NULL != TimeoutData_p) {
        free(TimeoutData_p);
    }
}

/** @} */
