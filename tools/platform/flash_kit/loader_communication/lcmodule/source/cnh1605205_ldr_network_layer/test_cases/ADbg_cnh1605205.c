/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/

/**
 * @file ADbg_cnh<ModuleNumber>.c
 *
 * @brief Test cases for CNH1605205 module.
 *
 * This file consist of test cases for autometic testing functions from
 * CNH1605205 module.
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
#include "r_a2_header.h"
#include "r_a2_network.h"
#endif // CFG_ENABLE_LOADER_TYPE
#include "r_r15_header.h"
#include "r_r15_network_layer.h"
#include <stdlib.h>

/*******************************************************************************
 * The test case functions declarations
 ******************************************************************************/
#ifndef CFG_ENABLE_LOADER_TYPE
static void ADbg_A2_GetPacketLength(ADbg_Case_t *Case_p);
static void ADbg_A2_IsValidHeader(ADbg_Case_t *Case_p);
static void ADbg_A2_IsReceivedHeader(ADbg_Case_t *Case_p);

static void ADbg_A2_Network_Initialize(ADbg_Case_t *Case_p);
static void ADbg_A2_Network_Shutdown(ADbg_Case_t *Case_p);
static void ADbg_A2_Network_PacketAllocate(ADbg_Case_t *Case_p);
static void ADbg_A2_Network_PacketRelease(ADbg_Case_t *Case_p);
#endif // CFG_ENABLE_LOADER_TYPE

static void ADbg_R15_GetPacketLength(ADbg_Case_t *Case_p);
static void ADbg_R15_IsValidHeader(ADbg_Case_t *Case_p);
static void ADbg_R15_IsReceivedHeader(ADbg_Case_t *Case_p);
static void ADbg_R15_IsValidExtendedHeader(ADbg_Case_t *Case_p);

static void ADbg_R15_Network_Initialize(ADbg_Case_t *Case_p);
static void ADbg_R15_Network_Shutdown(ADbg_Case_t *Case_p);
static void ADbg_R15_Network_CreateUniqueKey(ADbg_Case_t *Case_p);
static void ADbg_R15_Network_PacketAllocate(ADbg_Case_t *Case_p);
static void ADbg_R15_Network_PacketRelease(ADbg_Case_t *Case_p);

/*******************************************************************************
 * The test case definitions
 ******************************************************************************/
#ifndef CFG_ENABLE_LOADER_TYPE
static uint8 Parameters1_1[3] = {2, WORDPTR, WORD};
ADBG_CASE_DEFINE(Test_A2_GetPacketLength, 1, 9, ADbg_A2_GetPacketLength, Parameters1_1);
static uint8 Parameters1_2[3] = {2, VOIDPTR, BYTE};
ADBG_CASE_DEFINE(Test_A2_IsValidHeader, 2, 9, ADbg_A2_IsValidHeader, Parameters1_2);
static uint8 Parameters1_3[3] = {2, VOIDPTR, BYTE};
ADBG_CASE_DEFINE(Test_A2_IsReceivedHeader, 3, 9, ADbg_A2_IsReceivedHeader, Parameters1_3);

static uint8 Parameters2_1[3] = {2, WORDPTR, WORD};
ADBG_CASE_DEFINE(Test_A2_Network_Initialize, 4, 9, ADbg_A2_Network_Initialize, Parameters2_1);
static uint8 Parameters2_2[3] = {2, WORDPTR, WORD};
ADBG_CASE_DEFINE(Test_A2_Network_Shutdown, 5, 9, ADbg_A2_Network_Shutdown, Parameters2_2);
static uint8 Parameters2_3[4] = {3, WORDPTR, WORD, WORDPTR};
ADBG_CASE_DEFINE(Test_A2_Network_PacketAllocate, 6, 9, ADbg_A2_Network_PacketAllocate, Parameters2_3);
static uint8 Parameters2_4[4] = {3, WORDPTR, WORD, WORDPTR};
ADBG_CASE_DEFINE(Test_A2_Network_PacketRelease, 7, 9, ADbg_A2_Network_PacketRelease, Parameters2_4);
#endif // CFG_ENABLE_LOADER_TYPE

static uint8 Parameters3_1[3] = {2, WORDPTR, WORD};
ADBG_CASE_DEFINE(Test_R15_GetPacketLength, 8, 9, ADbg_R15_GetPacketLength, Parameters3_1);
static uint8 Parameters3_2[3] = {2, VOIDPTR, BYTE};
ADBG_CASE_DEFINE(Test_R15_IsValidHeader, 9, 9, ADbg_R15_IsValidHeader, Parameters3_2);
static uint8 Parameters3_3[3] = {2, WORDPTR, BYTE};
ADBG_CASE_DEFINE(Test_R15_IsReceivedHeader, 10, 9, ADbg_R15_IsReceivedHeader, Parameters3_3);
static uint8 Parameters3_4[5] = {4, VOIDPTR, BYTE, BYTE, BYTE};
ADBG_CASE_DEFINE(Test_R15_IsValidExtendedHeader, 11, 9, ADbg_R15_IsValidExtendedHeader, Parameters3_4);

static uint8 Parameters4_1[3] = {2, WORDPTR, WORD};
ADBG_CASE_DEFINE(Test_R15_Network_Initialize, 12, 9, ADbg_R15_Network_Initialize, Parameters4_1);
static uint8 Parameters4_2[3] = {2, WORDPTR, WORD};
ADBG_CASE_DEFINE(Test_R15_Network_Shutdown, 13, 9, ADbg_R15_Network_Shutdown, Parameters4_2);
static uint8 Parameters4_3[4] = {3, VOIDPTR, BYTE, WORD};
ADBG_CASE_DEFINE(Test_R15_Network_CreateUniqueKey, 14, 9, ADbg_R15_Network_CreateUniqueKey, Parameters4_3);
static uint8 Parameters4_4[4] = {3, WORDPTR, WORD, WORDPTR};
ADBG_CASE_DEFINE(Test_R15_Network_PacketAllocate, 15, 9, ADbg_R15_Network_PacketAllocate, Parameters4_4);
static uint8 Parameters4_5[4] = {3, WORDPTR, WORDPTR, WORD};
ADBG_CASE_DEFINE(Test_R15_Network_PacketRelease, 16, 9, ADbg_R15_Network_PacketRelease, Parameters4_5);

/*******************************************************************************
* The test case suite definition
******************************************************************************/
ADBG_SUITE_DEFINE_BEGIN(TestCase_CNH1605205, 0, 9)

#ifndef CFG_ENABLE_LOADER_TYPE
ADBG_SUITE_ENTRY(Test_A2_GetPacketLength)
ADBG_SUITE_ENTRY(Test_A2_IsValidHeader)
ADBG_SUITE_ENTRY(Test_A2_IsReceivedHeader)

ADBG_SUITE_ENTRY(Test_A2_Network_Initialize)
ADBG_SUITE_ENTRY(Test_A2_Network_Shutdown)
ADBG_SUITE_ENTRY(Test_A2_Network_PacketAllocate)
ADBG_SUITE_ENTRY(Test_A2_Network_PacketRelease)
#endif // CFG_ENABLE_LOADER_TYPE

ADBG_SUITE_ENTRY(Test_R15_GetPacketLength)
ADBG_SUITE_ENTRY(Test_R15_IsValidHeader)
ADBG_SUITE_ENTRY(Test_R15_IsReceivedHeader)
ADBG_SUITE_ENTRY(Test_R15_IsValidExtendedHeader)

ADBG_SUITE_ENTRY(Test_R15_Network_Initialize)
ADBG_SUITE_ENTRY(Test_R15_Network_Shutdown)
ADBG_SUITE_ENTRY(Test_R15_Network_CreateUniqueKey)
ADBG_SUITE_ENTRY(Test_R15_Network_PacketAllocate)
ADBG_SUITE_ENTRY(Test_R15_Network_PacketRelease)

ADBG_SUITE_DEFINE_END();

/*******************************************************************************
 * Case runner
 ******************************************************************************/
void Do_CNH1605205_Module_Test_Run(ADbg_MainModule_t *MainModule_p)
{
    ADbg_Module_t *Module_p = Do_ADbg_Module_Create(TestCase_CNH1605205, NULL, MainModule_p->Command_p, MainModule_p->Result_p);
    Do_ADbg_InModule_Command(Module_p);
    Do_ADbg_Module_Destroy(Module_p);
}
/*******************************************************************************
 * Test cases
 ******************************************************************************/
#ifndef CFG_ENABLE_LOADER_TYPE
/**
 * This function will test function:
 * A2_GetPacketLength.
 * Function used in: Test_A2_GetPacketLength.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_A2_GetPacketLength(ADbg_Case_t *Case_p)
{
    uint32 Result = 0;
    A2_Header_t *Header_p;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Header_p = Do_ADbg_GetDataPointer(sizeof(A2_Header_t *), (void **)&Var_p);

    Result = A2_GetPacketLength(Header_p);
    Do_ADbg_Assert((uint32)(*Var_p) == Result, Case_p);

    if (NULL != Header_p) {
        free(Header_p);
    }
}

/**
 * This function will test function:
 * A2_IsValidHeader.
 * Function used in: Test_A2_IsValidHeader.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_A2_IsValidHeader(ADbg_Case_t *Case_p)
{
    boolean Result = TRUE;
    void *Data_p = NULL;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Data_p = Do_ADbg_GetDataPointer(sizeof(void *), (void **)&Var_p);

    Result = A2_IsValidHeader(Data_p);
    Do_ADbg_Assert((boolean)(*Var_p) == Result, Case_p);

    if (NULL != Data_p) {
        free(Data_p);
    }
}

/**
 * This function will test function:
 * A2_IsReceivedHeader.
 * Function used in: Test_A2_IsReceivedHeader.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_A2_IsReceivedHeader(ADbg_Case_t *Case_p)
{
    boolean Result = TRUE;
    A2_Inbound_t *In_p;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    In_p = Do_ADbg_GetDataPointer(sizeof(A2_Inbound_t *), (void **)&Var_p);

    Result = A2_IsReceivedHeader(In_p);
    Do_ADbg_Assert((boolean)(*Var_p) == Result, Case_p);

    if (NULL != In_p) {
        free(In_p);
    }
}

/**
 * This function will test function:
 * A2_Network_Initialize.
 * Function used in: Test_A2_Network_Initialize.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_A2_Network_Initialize(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_SUCCESS;
    Communication_t *Communication_p;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Communication_p = Do_ADbg_GetDataPointer(sizeof(Communication_t *), (void **)&Var_p);

    Result = A2_Network_Initialize(Communication_p);
    Do_ADbg_Assert((ErrorCode_e)(*Var_p) == Result, Case_p);

    if (NULL != Communication_p) {
        free(Communication_p);
    }
}

/**
 * This function will test function:
 * A2_Network_Shutdown.
 * Function used in: Test_A2_Network_Shutdown.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_A2_Network_Shutdown(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_SUCCESS;
    Communication_t *Communication_p;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Communication_p = Do_ADbg_GetDataPointer(sizeof(Communication_t *), (void **)&Var_p);

    Result = A2_Network_Shutdown(Communication_p);
    Do_ADbg_Assert((ErrorCode_e)(*Var_p) == Result, Case_p);

    if (NULL != Communication_p) {
        free(Communication_p);
    }
}

/**
 * This function will test function:
 * A2_Network_PacketAllocate.
 * Function used in: Test_A2_Network_PacketAllocate.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_A2_Network_PacketAllocate(ADbg_Case_t *Case_p)
{
    A2_PacketMeta_t *Result = NULL;
    Communication_t *Communication_p;
    int BufferSize = 0;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Communication_p = Do_ADbg_GetDataPointer(sizeof(Communication_t *), (void **)&Var_p);

    Do_ADbg_GetDataVar(sizeof(int), &Var_p, &BufferSize);

    Result = A2_Network_PacketAllocate(Communication_p, BufferSize);
    Do_ADbg_Assert((A2_PacketMeta_t *)(*Var_p) != Result, Case_p);

    if (NULL != Communication_p) {
        free(Communication_p);
    }
}

/**
 * This function will test function:
 * A2_Network_PacketRelease.
 * Function used in: Test_A2_Network_PacketRelease.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_A2_Network_PacketRelease(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_SUCCESS;
    Communication_t *Communication_p;
    A2_PacketMeta_t *Meta_p;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Communication_p = Do_ADbg_GetDataPointer(sizeof(Communication_t *), (void **)&Var_p);
    Meta_p = Do_ADbg_GetDataPointer(sizeof(A2_PacketMeta_t *), (void **)&Var_p);

    Result = A2_Network_PacketRelease(Communication_p, Meta_p);
    Do_ADbg_Assert((ErrorCode_e)(*Var_p) == Result, Case_p);

    if (NULL != Communication_p) {
        free(Communication_p);
    }

    if (NULL != Meta_p) {
        free(Meta_p);
    }
}
#endif // CFG_ENABLE_LOADER_TYPE

/**
 * This function will test function:
 * R15_GetPacketLength.
 * Function used in: Test_R15_GetPacketLength.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_R15_GetPacketLength(ADbg_Case_t *Case_p)
{
    uint32 Result = 0;
    R15_Header_t *Header_p;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Header_p = Do_ADbg_GetDataPointer(sizeof(R15_Header_t *), (void **)&Var_p);

    Result = R15_GetPacketLength(Header_p);
    Do_ADbg_Assert((uint32)(*Var_p) == Result, Case_p);

    if (NULL != Header_p) {
        free(Header_p);
    }
}

/**
 * This function will test function:
 * R15_IsValidHeader.
 * Function used in: Test_R15_IsValidHeader.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_R15_IsValidHeader(ADbg_Case_t *Case_p)
{
    boolean Result = FALSE;
    void *Data_p = NULL;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Data_p = Do_ADbg_GetDataPointer(sizeof(void *), (void **)&Var_p);

    Result = R15_IsValidHeader(Data_p);
    Do_ADbg_Assert((boolean)(*Var_p) != Result, Case_p);

    if (NULL != Data_p) {
        free(Data_p);
    }
}

/**
 * This function will test function:
 * R15_IsReceivedHeader.
 * Function used in: Test_R15_IsReceivedHeader.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_R15_IsReceivedHeader(ADbg_Case_t *Case_p)
{
    boolean Result = FALSE;
    R15_Inbound_t *In_p;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    In_p = Do_ADbg_GetDataPointer(sizeof(R15_Inbound_t *), (void **)&Var_p);

    Result = R15_IsReceivedHeader(In_p);
    Do_ADbg_Assert((boolean)(*Var_p) != Result, Case_p);

    if (NULL != In_p) {
        free(In_p);
    }
}

/**
 * This function will test function:
 * R15_IsValidExtendedHeader.
 * Function used in: Test_R15_IsValidExtendedHeader.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_R15_IsValidExtendedHeader(ADbg_Case_t *Case_p)
{
    boolean Result = FALSE;
    void *Data_p = NULL;
    uint8 Length = 0;
    uint8 Checksum = 0;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Data_p = Do_ADbg_GetDataPointer(sizeof(void *), (void **)&Var_p);

    Do_ADbg_GetDataVar(sizeof(uint8), &Var_p, &Length);
    Do_ADbg_GetDataVar(sizeof(uint8), &Var_p, &Checksum);

    Result = R15_IsValidExtendedHeader(Data_p, Length, Checksum);
    Do_ADbg_Assert((boolean)(*Var_p) != Result, Case_p);

    if (NULL != Data_p) {
        free(Data_p);
    }
}

/**
 * This function will test function:
 * R15_Network_Initialize.
 * Function used in: Test_R15_Network_Initialize.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_R15_Network_Initialize(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_SUCCESS;
    Communication_t *Communication_p;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Communication_p = Do_ADbg_GetDataPointer(sizeof(Communication_t *), (void **)&Var_p);

    Result = R15_Network_Initialize(Communication_p);
    Do_ADbg_Assert((ErrorCode_e)(*Var_p) == Result, Case_p);

    if (NULL != Communication_p) {
        free(Communication_p);
    }
}

/**
 * This function will test function:
 * R15_Network_Shutdown.
 * Function used in: Test_R15_Network_Shutdown.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_R15_Network_Shutdown(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_SUCCESS;
    Communication_t *Communication_p;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Communication_p = Do_ADbg_GetDataPointer(sizeof(Communication_t *), (void **)&Var_p);

    Result = R15_Network_Shutdown(Communication_p);
    Do_ADbg_Assert((ErrorCode_e)(*Var_p) == Result, Case_p);

    if (NULL != Communication_p) {
        free(Communication_p);
    }
}

/**
 * This function will test function:
 * R15_Network_CreateUniqueKey.
 * Function used in: Test_R15_Network_CreateUniqueKey.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_R15_Network_CreateUniqueKey(ADbg_Case_t *Case_p)
{
    uint32 Result = 0;
    void *Packet_p = NULL;
    uint8 ExternalKey = 0;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Packet_p = Do_ADbg_GetDataPointer(sizeof(void *), (void **)&Var_p);

    Result = R15_Network_CreateUniqueKey(Packet_p, ExternalKey);
    Do_ADbg_Assert((uint32)(*Var_p) != Result, Case_p);

    if (NULL != Packet_p) {
        free(Packet_p);
    }
}

/**
 * This function will test function:
 * R15_Network_PacketAllocate.
 * Function used in: Test_R15_Network_PacketAllocate.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_R15_Network_PacketAllocate(ADbg_Case_t *Case_p)
{
    PacketMeta_t *Result = NULL;
    Communication_t *Communication_p;
    uint8 BufferSize = 0;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Communication_p = Do_ADbg_GetDataPointer(sizeof(Communication_t *), (void **)&Var_p);

    Result = R15_Network_PacketAllocate(Communication_p, BufferSize);
    Do_ADbg_Assert((PacketMeta_t *)(*Var_p) != Result, Case_p);

    if (NULL != Communication_p) {
        free(Communication_p);
    }
}

/**
 * This function will test function:
 * R15_Network_PacketRelease.
 * Function used in: Test_R15_Network_PacketRelease.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_R15_Network_PacketRelease(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_SUCCESS;
    Communication_t *Communication_p;
    PacketMeta_t *Meta_p;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Communication_p = Do_ADbg_GetDataPointer(sizeof(Communication_t *), (void **)&Var_p);
    Meta_p = Do_ADbg_GetDataPointer(sizeof(PacketMeta_t *), (void **)&Var_p);

    Result = R15_Network_PacketRelease(Communication_p, Meta_p);
    Do_ADbg_Assert((ErrorCode_e)(*Var_p) == Result, Case_p);

    if (NULL != Meta_p) {
        free(Meta_p);
    }
}

/** @} */
