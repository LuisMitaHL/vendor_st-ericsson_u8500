/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/

/*
 * @file ADbg_cnh1605197.c
 *
 * @brief Test cases for CNH1605197 module.
 *
 * This file consist of test cases for automatic testing functions from
 * CNH1605197 module.
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
#include "commands_impl.h"
#include "string.h"
#include "r_serialization.h"
#include "r_memory_utils.h"

/*******************************************************************************
 * File scope types, constants and variables
 ******************************************************************************/
extern ErrorCode_e ExpectedResult;

/*******************************************************************************
 * The test case functions declarations
 ******************************************************************************/
static void ADbg_Do_ParameterStorage_ReadGlobalDataUnitImpl(ADbg_Case_t *Case_p);
static void ADbg_Do_ParameterStorage_WriteGlobalDataUnitImpl(ADbg_Case_t *Case_p);
static void ADbg_Do_ParameterStorage_ReadGlobalDataSetImpl(ADbg_Case_t *Case_p);
static void ADbg_Do_ParameterStorage_WriteGlobalDataSetImpl(ADbg_Case_t *Case_p);
static void ADbg_Do_ParameterStorage_EraseGlobalDataSetImpl(ADbg_Case_t *Case_p);

/*******************************************************************************
 * The test case definitions
 ******************************************************************************/
static uint8 Parameters1[5] = {4, SHORT, CHARPTR, WORD, WORD};
ADBG_CASE_DEFINE(Test_Do_ParameterStorage_ReadGlobalDataUnitImpl, 1, 4, ADbg_Do_ParameterStorage_ReadGlobalDataUnitImpl, Parameters1);

//xsassto This Case is exception, first parametar is ExpectedResult
static uint8 Parameters2[7] = {6, WORD, SHORT, CHARPTR, WORD, WORD, VOIDPTR};
ADBG_CASE_DEFINE(Test_Do_ParameterStorage_WriteGlobalDataUnitImpl, 2, 4, ADbg_Do_ParameterStorage_WriteGlobalDataUnitImpl, Parameters2);
static uint8 Parameters3[5] = {4, SHORT, CHARPTR, CHARPTR, WORD};
ADBG_CASE_DEFINE(Test_Do_ParameterStorage_ReadGlobalDataSetImpl, 3, 4, ADbg_Do_ParameterStorage_ReadGlobalDataSetImpl, Parameters3);
static uint8 Parameters4[6] = {5, SHORT, CHARPTR, DOUBLEWORD, CHARPTR, WORD};
ADBG_CASE_DEFINE(Test_Do_ParameterStorage_WriteGlobalDataSetImpl, 4, 4, ADbg_Do_ParameterStorage_WriteGlobalDataSetImpl, Parameters4);
static uint8 Parameters5[4] = {3, SHORT, CHARPTR, WORD};
ADBG_CASE_DEFINE(Test_Do_ParameterStorage_EraseGlobalDataSetImpl, 5, 4, ADbg_Do_ParameterStorage_EraseGlobalDataSetImpl, Parameters5);

/*******************************************************************************
 * The test case suite definition
 ******************************************************************************/
ADBG_SUITE_DEFINE_BEGIN(TestCase_CNH1605197, 0, 4)

ADBG_SUITE_ENTRY(Test_Do_ParameterStorage_ReadGlobalDataUnitImpl)
ADBG_SUITE_ENTRY(Test_Do_ParameterStorage_WriteGlobalDataUnitImpl)
ADBG_SUITE_ENTRY(Test_Do_ParameterStorage_ReadGlobalDataSetImpl)
ADBG_SUITE_ENTRY(Test_Do_ParameterStorage_WriteGlobalDataSetImpl)
ADBG_SUITE_ENTRY(Test_Do_ParameterStorage_EraseGlobalDataSetImpl)

ADBG_SUITE_DEFINE_END();
/*******************************************************************************
 * Case runner
 ******************************************************************************/
void Do_CNH1605197_Module_Test_Run(ADbg_MainModule_t *MainModule_p)
{
    ADbg_Module_t *Module_p = Do_ADbg_Module_Create(TestCase_CNH1605197, NULL, MainModule_p->Command_p, MainModule_p->Result_p);
    Do_ADbg_InModule_Command(Module_p);
    Do_ADbg_Module_Destroy(Module_p);
}
/*******************************************************************************
 * Test cases
 ******************************************************************************/

/**
 * This function will test function:
 * Do_ParameterStorage_ReadGlobalDataUnitImpl.
 * Function used in: Test_Do_ParameterStorage_ReadGlobalDataUnitImpl.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_Do_ParameterStorage_ReadGlobalDataUnitImpl(ADbg_Case_t *Case_p)
{
    uint16 Session = 0;
    uint32 DevicePathLength = 0;
    char *DevicePath_p = NULL;
    uint32 Unit_id = 0;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Do_ADbg_GetDataVar(sizeof(uint16), &Var_p, &Session);

    DevicePathLength = get_uint32_string_le((void **)&Var_p);
    DevicePath_p = (char *)Do_ADbg_GetDataPointer(DevicePathLength, (void **)&Var_p);

    Do_ADbg_GetDataVar(sizeof(uint32), &Var_p, &Unit_id);
    Do_ADbg_GetDataVar(sizeof(uint32), &Var_p, &ExpectedResult);

    (void)Do_ParameterStorage_ReadGlobalDataUnitImpl(Session, DevicePath_p, Unit_id);
}

/**
 * This function will test function:
 * Do_ParameterStorage_WriteGlobalDataUnitImpl.
 * Function used in: Test_Do_ParameterStorage_WriteGlobalDataUnitImpl.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_Do_ParameterStorage_WriteGlobalDataUnitImpl(ADbg_Case_t *Case_p)
{
    uint16 Session = 0;
    uint32 DevicePathLength = 0;
    char *DevicePath_p = NULL;
    uint32 Unit_id = 0;
    uint32 DataBuffLength = 0;
    void *DataBuff_p = NULL;
    uint8 *Var_p = NULL;

    //xsassto This Case is exception, first parametar is ExpectedResult

    Var_p = Case_p->Command_p->Data_p;
    Do_ADbg_GetDataVar(sizeof(uint32), &Var_p, &ExpectedResult);
    Do_ADbg_GetDataVar(sizeof(uint16), &Var_p, &Session);

    DevicePathLength = get_uint32_string_le((void **)&Var_p);
    DevicePath_p = (char *)Do_ADbg_GetDataPointer(DevicePathLength, (void **)&Var_p);

    Do_ADbg_GetDataVar(sizeof(uint32), &Var_p, &Unit_id);

    Do_ADbg_GetDataVar(sizeof(uint32), &Var_p, &DataBuffLength);
    DataBuff_p = (void *)Do_ADbg_GetDataPointer(DevicePathLength, (void **)&Var_p);

    (void)Do_ParameterStorage_WriteGlobalDataUnitImpl(Session, DevicePath_p, Unit_id, DataBuffLength, DataBuff_p);
    //Do_ADbg_Assert((ErrorCode_e)(*Var_p) == Result, Case_p);

    BUFFER_FREE(DataBuff_p);
}

/**
 * This function will test function:
 * Do_ParameterStorage_ReadGlobalDataSetImpl.
 * Function used in: Test_Do_ParameterStorage_ReadGlobalDataSetImpl.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_Do_ParameterStorage_ReadGlobalDataSetImpl(ADbg_Case_t *Case_p)
{
    uint16 Session = 0;
    uint32 DevicePathLength = 0;
    char *DevicePath_p = NULL;
    uint32 TargetPathLength = 0;
    char *TargetPath_p = NULL;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Do_ADbg_GetDataVar(sizeof(uint16), &Var_p, &Session);

    DevicePathLength = get_uint32_string_le((void **)&Var_p);
    DevicePath_p = (char *)Do_ADbg_GetDataPointer(DevicePathLength, (void **)&Var_p);

    TargetPathLength = get_uint32_string_le((void **)&Var_p);
    TargetPath_p = (char *)Do_ADbg_GetDataPointer(TargetPathLength, (void **)&Var_p);

    Do_ADbg_GetDataVar(sizeof(uint32), &Var_p, &ExpectedResult);

    (void)Do_ParameterStorage_ReadGlobalDataSetImpl(Session, DevicePath_p, TargetPath_p);

    BUFFER_FREE(TargetPath_p);
}

/**
 * This function will test function:
 * Do_ParameterStorage_WriteGlobalDataSetImpl.
 * Function used in: Test_Do_ParameterStorage_WriteGlobalDataSetImpl.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_Do_ParameterStorage_WriteGlobalDataSetImpl(ADbg_Case_t *Case_p)
{
    uint16 Session = 0;
    uint32 DevicePathLength = 0;
    char *DevicePath_p = NULL;
    uint64 DataLength = 0;
    uint32 SourcePathLength = 0;
    char *SourcePath_p = NULL;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Do_ADbg_GetDataVar(sizeof(uint16), &Var_p, &Session);

    DevicePathLength = get_uint32_string_le((void **)&Var_p);
    DevicePath_p = (char *)Do_ADbg_GetDataPointer(DevicePathLength, (void **)&Var_p);

    Do_ADbg_GetDataVar(sizeof(uint64), &Var_p, &DataLength);

    SourcePathLength = get_uint32_string_le((void **)&Var_p);
    SourcePath_p = (char *)Do_ADbg_GetDataPointer(SourcePathLength, (void **)&Var_p);

    Do_ADbg_GetDataVar(sizeof(uint32), &Var_p, &ExpectedResult);

    (void)Do_ParameterStorage_WriteGlobalDataSetImpl(Session, DevicePath_p, DataLength, SourcePath_p);

    BUFFER_FREE(SourcePath_p);
}

/**
 * This function will test function:
 * Do_ParameterStorage_EraseGlobalDataSetImpl.
 * Function used in: Test_Do_ParameterStorage_EraseGlobalDataSetImpl.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_Do_ParameterStorage_EraseGlobalDataSetImpl(ADbg_Case_t *Case_p)
{
    uint16 Session = 0;
    uint32 DevicePathLength = 0;
    char *DevicePath_p = NULL;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Do_ADbg_GetDataVar(sizeof(uint16), &Var_p, &Session);

    DevicePathLength = get_uint32_string_le((void **)&Var_p);
    DevicePath_p = (char *)Do_ADbg_GetDataPointer(DevicePathLength, (void **)&Var_p);

    Do_ADbg_GetDataVar(sizeof(uint32), &Var_p, &ExpectedResult);

    (void)Do_ParameterStorage_EraseGlobalDataSetImpl(Session, DevicePath_p);
}

/* @} */
