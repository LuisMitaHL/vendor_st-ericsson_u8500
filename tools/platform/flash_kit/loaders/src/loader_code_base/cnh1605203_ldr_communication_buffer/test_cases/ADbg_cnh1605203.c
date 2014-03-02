/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/

/**
 * @file ADbg_cnh1605203.c
 *
 * @brief Test cases for CNH1605203 module.
 *
 * This file consist of test cases for autometic testing functions from
 * CNH1605203 module.
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
#include "commands_impl.h"
#include "r_serialization.h"
#include "r_communication_buffer.h"
#include <string.h>
#include "r_memory_utils.h"

/*******************************************************************************
 * The test case functions declarations
 ******************************************************************************/
static void ADbg_Do_Buffer_BuffersInit(ADbg_Case_t *Case_p);
static void ADbg_Do_Buffer_BufferAllocate(ADbg_Case_t *Case_p);
static void ADbg_Do_Buffer_BufferRelease(ADbg_Case_t *Case_p);
static void ADbg_Do_Buffer_BuffersAvailable(ADbg_Case_t *Case_p);
static void ADbg_Do_Buffer_BuffersDeinit(ADbg_Case_t *Case_p);

/*******************************************************************************
 * The test case definitions
 ******************************************************************************/
static uint8 Parameters1[3] = {2, VOIDPTR, WORD};
ADBG_CASE_DEFINE(Test_Do_Buffer_BuffersInit, 1, 7, ADbg_Do_Buffer_BuffersInit, Parameters1);
static uint8 Parameters2[3] = {2, VOIDPTR, VOIDPTR};
ADBG_CASE_DEFINE(Test_Do_Buffer_BufferAllocate, 2, 7, ADbg_Do_Buffer_BufferAllocate, Parameters2);
static uint8 Parameters3[5] = {4, VOIDPTR, VOIDPTR, WORD, WORD};
ADBG_CASE_DEFINE(Test_Do_Buffer_BufferRelease, 3, 7, ADbg_Do_Buffer_BufferRelease, Parameters3);
static uint8 Parameters4[3] = {2, SHORT, SHORT};
ADBG_CASE_DEFINE(Test_Do_Buffer_BuffersAvailable, 4, 7, ADbg_Do_Buffer_BuffersAvailable, Parameters4);
static uint8 Parameters5[3] = {2, SHORT, SHORT};
ADBG_CASE_DEFINE(Test_Do_Buffer_BuffersDeinit, 5, 7, ADbg_Do_Buffer_BuffersDeinit, Parameters5);


/*******************************************************************************
* The test case suite definition
******************************************************************************/
ADBG_SUITE_DEFINE_BEGIN(TestCase_CNH1605203, 0, 7)

ADBG_SUITE_ENTRY(Test_Do_Buffer_BuffersInit)
ADBG_SUITE_ENTRY(Test_Do_Buffer_BufferAllocate)
ADBG_SUITE_ENTRY(Test_Do_Buffer_BufferRelease)
ADBG_SUITE_ENTRY(Test_Do_Buffer_BuffersAvailable)
ADBG_SUITE_ENTRY(Test_Do_Buffer_BuffersDeinit)

ADBG_SUITE_DEFINE_END();

/*******************************************************************************
 * Case runner
 ******************************************************************************/
void Do_CNH1605203_Module_Test_Run(ADbg_MainModule_t *MainModule_p)
{
    ADbg_Module_t *Module_p = Do_ADbg_Module_Create(TestCase_CNH1605203, NULL, MainModule_p->Command_p, MainModule_p->Result_p);
    Do_ADbg_InModule_Command(Module_p);
    Do_ADbg_Module_Destroy(Module_p);
}
/*******************************************************************************
 * Test cases
 ******************************************************************************/
/**
 * This function will test function:
 * Do_Buffer_BuffersInit.
 * Function used in: Test_Do_Buffer_BuffersInit.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_Do_Buffer_BuffersInit(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_GENERAL_FATAL_ERROR;
    uint32 ObjectLength = 0;
    void *Object_p = NULL;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;

    ObjectLength = get_uint32_le((void **)&Var_p);
    Object_p = Do_ADbg_GetDataPointer(ObjectLength, (void **)&Var_p);

    Result = Do_Buffer_BuffersInit(Object_p);
    Do_ADbg_Assert((ErrorCode_e)(*Var_p) == Result, Case_p);

    BUFFER_FREE(Object_p);
}

/**
 * This function will test function:
 * Do_Buffer_BufferAllocate.
 * Function used in: Test_Do_Buffer_BufferAllocate.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_Do_Buffer_BufferAllocate(ADbg_Case_t *Case_p)
{
    void *Result_p = NULL;
    uint32 ObjectLength = 0;
    void *Object_p = NULL;
    int BufferSize = 0;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;

    ObjectLength = get_uint32_le((void **)&Var_p);
    Object_p = Do_ADbg_GetDataPointer(ObjectLength, (void **)&Var_p);

    Do_ADbg_GetDataVar(sizeof(int), &Var_p, &BufferSize);

    Result_p = Do_Buffer_BufferAllocate(Object_p, BufferSize);
    Do_ADbg_Assert((void *)(*Var_p) == Result_p, Case_p);

    BUFFER_FREE(Object_p);
}

/**
 * This function will test function:
 * Do_Buffer_BufferRelease.
 * Function used in: Test_Do_Buffer_BufferRelease.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_Do_Buffer_BufferRelease(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_SUCCESS;
    uint32 ObjectLength = 0;
    void *Object_p = NULL;
    void *Buffer_p = NULL;
    int BufferSize = 0;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;

    ObjectLength = get_uint32_le((void **)&Var_p);
    Object_p = Do_ADbg_GetDataPointer(ObjectLength, (void **)&Var_p);

    BufferSize = get_uint32_le((void **)&Var_p);
    Buffer_p = Do_ADbg_GetDataPointer(BufferSize, (void **)&Var_p);

    Do_ADbg_GetDataVar(sizeof(int), &Var_p, &BufferSize);

    Result = Do_Buffer_BufferRelease(Object_p, Buffer_p, BufferSize);
    Do_ADbg_Assert((ErrorCode_e)(*Var_p) == Result, Case_p);

    BUFFER_FREE(Object_p);
    BUFFER_FREE(Buffer_p);
}

/**
 * This function will test function:
 * Do_Buffer_BuffersAvailable.
 * Function used in: Test_Do_Buffer_BuffersAvailable.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_Do_Buffer_BuffersAvailable(ADbg_Case_t *Case_p)
{
    uint32 Result = 0;
    uint32 Return = 0;
    uint32 ObjectLength = 0;
    void *Object_p = NULL;
    int BufferSize = 0;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;

    ObjectLength = get_uint32_le((void **)&Var_p);
    Object_p = Do_ADbg_GetDataPointer(ObjectLength, (void **)&Var_p);

    Do_ADbg_GetDataVar(sizeof(int), &Var_p, &BufferSize);

    Result = Do_Buffer_BuffersAvailable(Object_p, BufferSize);
    Do_ADbg_Assert(Return != Result, Case_p);

    BUFFER_FREE(Object_p);
}

/**
 * This function will test function:
 * Do_Buffer_BuffersDeinit.
 * Function used in: Test_Do_Buffer_BuffersDeinit.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_Do_Buffer_BuffersDeinit(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_SUCCESS;
    uint32 ObjectLength = 0;
    void *Object_p = NULL;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;

    ObjectLength = get_uint32_le((void **)&Var_p);
    Object_p = Do_ADbg_GetDataPointer(ObjectLength, (void **)&Var_p);

    Do_Buffer_BuffersDeinit(Object_p);
    Do_ADbg_Assert(E_SUCCESS == Result, Case_p);

    BUFFER_FREE(Object_p);
}

/** @} */
