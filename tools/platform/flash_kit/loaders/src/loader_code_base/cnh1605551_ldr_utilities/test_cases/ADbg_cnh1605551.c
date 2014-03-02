/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/

/**
 * @file ADbg_cnh1605551.c
 *
 * @brief Test cases for CNH1605551 module.
 *
 * This file consist of test cases for autometic testing functions from
 * CNH1605551 module.
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
#include "r_debug.h"
#include "r_memory_utils.h"
#include <string.h>
#include "r_serialization.h"
#include "t_comm_devices.h"
#include "r_path_utilities.h"
#include "r_comm_devices.h"
#include "t_UART_abstraction.h"
#include "r_UART_abstraction.h"
#include "r_bitset.h"
#include "t_bitset.h"
#include "t_elf_parser.h"
#include "r_elf_parser.h"
//#include "r_communication_abstraction.h"
//#include "t_communication_abstraction.h"


extern int ADbgAssertIndicator;

/*******************************************************************************
 * The test case functions declarations
 ******************************************************************************/
/* functions from bitset.c 1-10 */
static void ADbg_SearchBulkPath(ADbg_Case_t *Case_p);
static void ADbg_ExtractPath(ADbg_Case_t *Case_p);
static void ADbg_Do_GetDeviceSettings(ADbg_Case_t *Case_p);

/* functions from debug_subsystem.c 11-20 */
static void ADbg_Do_BitSet32_Insert(ADbg_Case_t *Case_p);
static void ADbg_Do_ElfParser_Create(ADbg_Case_t *Case_p);
//static void ADbg_Do_GetUARTDescriptor(ADbg_Case_t* Case_p);
//static void ADbg_<TestedFunction3>(ADbg_Case_t* Case_p);

/*******************************************************************************
 * The test case definitions
 ******************************************************************************/
//static uint8 ParametersX[NumberOfParameters+1] = {NumberOfParameters(Input+Output), InputPar1,..InputParN, OutputPar}
//ADBG_CASE_DEFINE(TestCaseName, CaseId, ModuleId, TestCaseFunction, Parameters)

static uint8 Parameters1[3] = {2, BYTEPTR, SHORT};
ADBG_CASE_DEFINE(Test_SearchBulkPath, 1, 6, ADbg_SearchBulkPath, Parameters1);

static uint8 Parameters2[3] = {2, BYTEPTR, BYTEPTR};
ADBG_CASE_DEFINE(Test_ExtractPath, 2, 6, ADbg_ExtractPath, Parameters2);

static uint8 Parameters3[4] = {3, BYTE, BYTE, BYTE};
ADBG_CASE_DEFINE(Test_Do_GetDeviceSettings, 3, 6, ADbg_Do_GetDeviceSettings, Parameters3);

static uint8 Parameters4[4] = {3, WORD, WORD, SHORT};
ADBG_CASE_DEFINE(Test_Do_BitSet32_Insert, 4, 6, ADbg_Do_BitSet32_Insert, Parameters4);

static uint8 Parameters5[3] = {2, WORD, BYTEPTR};
ADBG_CASE_DEFINE(Test_Do_ElfParser_Create, 5, 6, ADbg_Do_ElfParser_Create, Parameters5);

//static uint8 Parameters4[4] = {3, BYTE, BYTE, BYTEPTR};
//ADBG_CASE_DEFINE(Test_Do_GetUARTDescriptor, 4, 6, ADbg_Do_GetUARTDescriptor, Parameters4);

//static uint8 Parameters3[5] = {4, SHORT, DOUBLEWORD, BYTEPTR, SHORT};
//ADBG_CASE_DEFINE(Test_<TestedFunction3>, 3, Y, ADbg_<TestedFunction3>, Parameters3);

/*******************************************************************************
* The test case suite definition
******************************************************************************/
//ADBG_SUITE_DEFINE_BEGIN(TestCase_CNH1605551, 0, ModuleId)
ADBG_SUITE_DEFINE_BEGIN(TestCase_CNH1605551, 0, 6)

//ADBG_SUITE_ENTRY(TestCaseName)
ADBG_SUITE_ENTRY(Test_SearchBulkPath)
ADBG_SUITE_ENTRY(Test_ExtractPath)
ADBG_SUITE_ENTRY(Test_Do_GetDeviceSettings)
ADBG_SUITE_ENTRY(Test_Do_BitSet32_Insert)
ADBG_SUITE_ENTRY(Test_Do_ElfParser_Create)
//ADBG_SUITE_ENTRY(Test_Do_GetUARTDescriptor)
//ADBG_SUITE_ENTRY(Test_<TestedFunction3>)

ADBG_SUITE_DEFINE_END();

/*******************************************************************************
 * Case runner
 ******************************************************************************/
void Do_CNH1605551_Module_Test_Run(ADbg_MainModule_t *MainModule_p)
{
    ADbg_Module_t *Module_p = Do_ADbg_Module_Create(TestCase_CNH1605551, NULL, MainModule_p->Command_p, MainModule_p->Result_p);
    Do_ADbg_InModule_Command(Module_p);
    Do_ADbg_Module_Destroy(Module_p);
}
/*******************************************************************************
 * Test cases
 ******************************************************************************/

/**
 * This function will test function:
 * SearchBulkPath.
 * Function used in: Test_SearchBulkPath.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_SearchBulkPath(ADbg_Case_t *Case_p)
{
    boolean Result;
    char *Path_p = NULL;
    uint32 PathLength = 0;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;

    PathLength = get_uint32_string_le((void **)&Var_p);
    Path_p = (char *)Do_ADbg_GetDataPointer(PathLength, (void **)&Var_p);

    Result = SearchBulkPath(Path_p);
    Do_ADbg_Assert((ErrorCode_e)(*Var_p) == Result, Case_p);

    BUFFER_FREE(Path_p);
}

/**
 * This function will test function:
 * ExtractPath.
 * Function used in: Test_ExtractPath.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_ExtractPath(ADbg_Case_t *Case_p)
{
    char *Result_p = NULL;
    char *Path_p = NULL;
    uint32 PathLength = 0;
    uint32 Length = 0;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;

    PathLength = get_uint32_string_le((void **)&Var_p);
    Path_p = (char *)Do_ADbg_GetDataPointer(PathLength, (void **)&Var_p);

    Do_ADbg_GetDataVar(sizeof(uint32), &Var_p, &Length);

    Result_p = ExtractPath(Path_p);
    Do_ADbg_Assert((0 == strncmp((const char *)Var_p , (const char *)Result_p, Length)), Case_p);

    BUFFER_FREE(Path_p);
    BUFFER_FREE(Result_p);
}

/**
 * This function will test function:
 * Do_GetDeviceSettings.
 * Function used in: Test_Do_GetDeviceSettings.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_Do_GetDeviceSettings(ADbg_Case_t *Case_p)
{
    DeviceParam_t Result;
    DeviceParam_t DevParam;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;

    Do_ADbg_GetDataVar(sizeof(uint8), &Var_p, &DevParam.Device);
    Do_ADbg_GetDataVar(sizeof(uint8), &Var_p, &DevParam.Speed);
    Do_ADbg_GetDataVar(sizeof(uint8), &Var_p, &DevParam.HiSpeed);

    Result = Do_GetDeviceSettings(DevParam.Device);   //TODO: This function is changed and check this test case!
    Do_ADbg_Assert(((DevParam.Device == Result.Device)
                    && (DevParam.Speed == Result.Speed)
                    && (DevParam.HiSpeed == Result.HiSpeed)), Case_p);

}

/**
 * This function will test function:
 * Do_BitSet32_Insert.
 * Function used in: Test_Do_BitSet32_Insert.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_Do_BitSet32_Insert(ADbg_Case_t *Case_p)
{
    boolean Result;
    BitSet32_t *Set_p = NULL;
    int V;
    int Variable;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;

    Do_ADbg_GetDataVar(sizeof(uint32), &Var_p, &Variable);
    Do_ADbg_GetDataVar(sizeof(uint32), &Var_p, &V);

    Set_p = (BitSet32_t *)&Variable;

    Result = Do_BitSet32_Insert(Set_p, V);

    Do_ADbg_Assert((ErrorCode_e)(*Var_p) == Result, Case_p);
}

/**
 * This function will test function:
 * Do_ElfParser_Create.
 * Function used in: Test_Do_ElfParser_Create.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_Do_ElfParser_Create(ADbg_Case_t *Case_p)
{
    ElfParserContext_t *Result_p;
    uint32 Handle;

    uint8 *Var_p = NULL;
    Var_p = Case_p->Command_p->Data_p;

    Do_ADbg_GetDataVar(sizeof(uint32), &Var_p, &Handle);

    Result_p = (ElfParserContext_t *)Do_ElfParser_Create(Handle);

    Do_ADbg_Assert(((ElfParserContext_t *)(Var_p) == (ElfParserContext_t *)(Result_p)), Case_p);

}


/**
 * This function will test function:
 * Do_GetUARTDescriptor.
 * Function used in: Test_Do_GetUARTDescriptor.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
//static void ADbg_Do_GetUARTDescriptor(ADbg_Case_t* Case_p)
//{
//  CommDev_t* Result_p = NULL;
//  //UART_InternalData_t* ParData_p = NULL;
//  uint8* Var_p = NULL;
//  uint8 Device;
//  uint8 Speed;
//
//  Var_p = Case_p->Command_p->Data_p;
//  //ParData_p = (UART_InternalData_t*) malloc(sizeof(UART_InternalData_t));
//
//  Do_ADbg_GetDataVar(sizeof(uint8), &Var_p, &Device);
//  Do_ADbg_GetDataVar(sizeof(uint8), &Var_p, &Speed);
//
//  //ParData_p->Device = Device;
//  //ParData_p->Speed = Speed;
//
//
//  Result_p = Do_GetUARTDescriptor(Device, Speed);
//  Do_ADbg_Assert((Speed == Result_p->Descriptor_p->Parameters.Speed), Case_p);
//
////  (Device == Result_p->Descriptor_p->Parameters->Device)
//  //BUFFER_FREE(ParData_p);
//}

/** @} */
