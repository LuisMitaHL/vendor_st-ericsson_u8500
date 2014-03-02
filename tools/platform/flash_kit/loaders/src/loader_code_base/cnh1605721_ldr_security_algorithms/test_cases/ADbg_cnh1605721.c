/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/

/**
 * @file ADbg_cnh<ModuleNumber>.c
 *
 * @brief Test cases for CNH1605721 module.
 *
 * This file consist of test cases for autometic testing functions from
 * CNH1605721 module.
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
#include "r_serialization.h"
#include "r_security_algorithms.h"
#include "commands_impl.h"
#include "r_memory_utils.h"

/*******************************************************************************
 * The test case functions declarations
 ******************************************************************************/
static void ADbg_Do_Hash_DeviceShutdown(ADbg_Case_t *Case_p);
static void ADbg_Do_Crypto_AsynchronousHash(ADbg_Case_t *Case_p);
static void ADbg_Do_Crypto_GetHashLength(ADbg_Case_t *Case_p);

/*******************************************************************************
 * The test case definitions
 ******************************************************************************/
//static uint8 ParametersX[NumberOfParameters+1] = {NumberOfParameters(Input+Output), InputPar1,..InputParN, OutputPar}
//ADBG_CASE_DEFINE(TestCaseName, CaseId, ModuleId, TestCaseFunction, Parameters)

static uint8 Parameters1[3] = {2, VOIDPTR, VOIDPTR};
ADBG_CASE_DEFINE(Test_Do_Hash_DeviceShutdown, 1, 14, ADbg_Do_Hash_DeviceShutdown, Parameters1);
static uint8 Parameters2[8] = {7, VOIDPTR, DOUBLEWORD, VOIDPTR, WORD, BYTEPTR, DOUBLEWORD, VOIDPTR};
ADBG_CASE_DEFINE(Test_Do_Crypto_AsynchronousHash, 2, 14, ADbg_Do_Crypto_AsynchronousHash, Parameters2);
static uint8 Parameters3[4] = {3, VOIDPTR, DOUBLEWORD, WORD};
ADBG_CASE_DEFINE(Test_Do_Crypto_GetHashLength, 3, 14, ADbg_Do_Crypto_GetHashLength, Parameters3);

/*******************************************************************************
* The test case suite definition
******************************************************************************/
//ADBG_SUITE_DEFINE_BEGIN(TestCase_CNH1605721, 0, ModuleId)
ADBG_SUITE_DEFINE_BEGIN(TestCase_CNH1605721, 0, 14)

ADBG_SUITE_ENTRY(Test_Do_Hash_DeviceShutdown)
ADBG_SUITE_ENTRY(Test_Do_Crypto_AsynchronousHash)
ADBG_SUITE_ENTRY(Test_Do_Crypto_GetHashLength)

ADBG_SUITE_DEFINE_END();

/*******************************************************************************
 * Case runner
 ******************************************************************************/
void Do_CNH1605721_Module_Test_Run(ADbg_MainModule_t *MainModule_p)
{
    ADbg_Module_t *Module_p = Do_ADbg_Module_Create(TestCase_CNH1605721, NULL, MainModule_p->Command_p, MainModule_p->Result_p);
    Do_ADbg_InModule_Command(Module_p);
    Do_ADbg_Module_Destroy(Module_p);
}
/*******************************************************************************
 * Test cases
 ******************************************************************************/
/**
 * This function will test function:
 * Do_Hash_DeviceShutdown.
 * Function used in: Test_Do_Hash_DeviceShutdown.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_Do_Hash_DeviceShutdown(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_SUCCESS;
    uint32 ObjectLength = 0;
    void *Object_p = NULL;
    uint32 HashDeviceLength = 0;
    void *HashDevice_pp = NULL;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;

    ObjectLength = get_uint32_le((void **)&Var_p);
    Object_p = Do_ADbg_GetDataPointer(ObjectLength, (void **)&Var_p);

    HashDeviceLength = get_uint32_le((void **)&Var_p);
    HashDevice_pp = Do_ADbg_GetDataPointer(HashDeviceLength, (void **)&Var_p);

    Do_Hash_DeviceShutdown(Object_p, &HashDevice_pp);
    Do_ADbg_Assert(E_SUCCESS == Result, Case_p);

    BUFFER_FREE(Object_p);
    BUFFER_FREE(HashDevice_pp);
}

/**
 * This function will test function:
 * Do_Do_Crypto_AsynchronousHash.
 * Function used in: Test_Do_Crypto_AsynchronousHash.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_Do_Crypto_AsynchronousHash(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_SUCCESS;
    uint32 ObjectLength = 0;
    void *Object_p = NULL;
    HashType_e Type;
    uint32 DataLength = 0;
    void *Data_p = NULL;
    uint32 Length = 0;
    uint8 *Hash_p = NULL;
    HashCallback_fn Callback;
    uint32 ParamLength = 0;
    void *Param_p = NULL;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;

    ObjectLength = get_uint32_le((void **)&Var_p);
    Object_p = Do_ADbg_GetDataPointer(ObjectLength, (void **)&Var_p);

    Do_ADbg_GetDataVar(sizeof(HashType_e), &Var_p, &Type);

    DataLength = get_uint32_le((void **)&Var_p);
    Data_p = Do_ADbg_GetDataPointer(DataLength, (void **)&Var_p);

    Do_ADbg_GetDataVar(sizeof(uint32), &Var_p, &Length);

    Hash_p = (uint8 *) Do_ADbg_GetDataPointer(sizeof(uint8), (void **)&Var_p);

    Do_ADbg_GetDataVar(sizeof(HashCallback_fn), &Var_p, &Callback);

    ParamLength = get_uint32_le((void **)&Var_p);
    Param_p = Do_ADbg_GetDataPointer(ParamLength, (void **)&Var_p);

    Do_Crypto_AsynchronousHash(Object_p, Type, Data_p, Length, Hash_p, Callback, Param_p);
    Do_ADbg_Assert(E_SUCCESS == Result, Case_p);

    BUFFER_FREE(Object_p);
    BUFFER_FREE(Data_p);
    BUFFER_FREE(Hash_p);
    BUFFER_FREE(Param_p);

}

/**
 * This function will test function:
 * Do_Crypto_GetHashLength.
 * Function used in: Test_Do_Crypto_GetHashLength.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_Do_Crypto_GetHashLength(ADbg_Case_t *Case_p)
{
    uint32 Result = E_SUCCESS;
    uint32 ExpectedResult = 0xFFFFFFFF;
    uint32 ObjectLength = 0;
    void *Object_p = NULL;
    HashType_e Type;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;

    ObjectLength = get_uint32_le((void **)&Var_p);
    Object_p = Do_ADbg_GetDataPointer(ObjectLength, (void **)&Var_p);

    Do_ADbg_GetDataVar(sizeof(HashType_e), &Var_p, &Type);

    Result = Do_Crypto_GetHashLength(Object_p, Type);
    Do_ADbg_Assert(ExpectedResult != Result, Case_p);

    BUFFER_FREE(Object_p);
}

/* @} */
