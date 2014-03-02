/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
/**
 * @file ADbg_cnh1605196.c
 *
 * @brief Test cases for CNH1605196 module.
 * This file consist of test cases for automatic testing functions from
 * CNH1605196 module.
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
static void ADbg_Do_Flash_ProcessFileImpl(ADbg_Case_t *Case_p);
static void ADbg_Do_Flash_DumpAreaImpl(ADbg_Case_t *Case_p);
static void ADbg_Do_Flash_ListDevicesImpl(ADbg_Case_t *Case_p);
static void ADbg_Do_Flash_EraseAreaImpl(ADbg_Case_t *Case_p);

/*******************************************************************************
 * The test case definitions
 ******************************************************************************/
static uint8 Parameters1[6] = {5, SHORT, DOUBLEWORD, CHARPTR, CHARPTR, WORD};
ADBG_CASE_DEFINE(Test_Do_Flash_ProcessFileImpl, 1, 3, ADbg_Do_Flash_ProcessFileImpl, Parameters1);

static uint8 Parameters2[8] = {7, SHORT, CHARPTR, DOUBLEWORD, DOUBLEWORD, CHARPTR, WORD, WORD};
ADBG_CASE_DEFINE(Test_Do_Flash_DumpAreaImpl, 2, 3, ADbg_Do_Flash_DumpAreaImpl, Parameters2);

static uint8 Parameters3[3] = {2, SHORT, WORD};
ADBG_CASE_DEFINE(Test_Do_Flash_ListDevicesImpl, 3, 3, ADbg_Do_Flash_ListDevicesImpl, Parameters3);

static uint8 Parameters4[6] = {5, SHORT, CHARPTR, DOUBLEWORD, DOUBLEWORD, WORD};
ADBG_CASE_DEFINE(Test_Do_Flash_EraseAreaImpl, 4, 3, ADbg_Do_Flash_EraseAreaImpl, Parameters4);

/*******************************************************************************
 * The test case suite definition
 ******************************************************************************/
ADBG_SUITE_DEFINE_BEGIN(TestCase_CNH1605196, 0, 3)

ADBG_SUITE_ENTRY(Test_Do_Flash_ProcessFileImpl)
ADBG_SUITE_ENTRY(Test_Do_Flash_DumpAreaImpl)
ADBG_SUITE_ENTRY(Test_Do_Flash_ListDevicesImpl)
ADBG_SUITE_ENTRY(Test_Do_Flash_EraseAreaImpl)

ADBG_SUITE_DEFINE_END();

/*******************************************************************************
 * Case runner
 ******************************************************************************/
void Do_CNH1605196_Module_Test_Run(ADbg_MainModule_t *MainModule_p)
{
    ADbg_Module_t *Module_p = Do_ADbg_Module_Create(TestCase_CNH1605196, NULL, MainModule_p->Command_p, MainModule_p->Result_p);
    Do_ADbg_InModule_Command(Module_p);
    Do_ADbg_Module_Destroy(Module_p);
}

/*******************************************************************************
 * Test cases
 ******************************************************************************/

/**
 * This function will test function:
 * Do_Flash_ProcessFileImpl
 * Function used in: Test_Do_Flash_ProcessFileImpl
 *
 * @param [in] Case_p          Pointer
 * @return void
 */
static void ADbg_Do_Flash_ProcessFileImpl(ADbg_Case_t *Case_p)
{
    uint16 Session = 0;
    uint64 Length = 0;
    char *Type_p = NULL;
    uint32 TypeLength = 0;
    char *SourcePath_p = NULL;
    uint32 PathLength = 0;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Do_ADbg_GetDataVar(sizeof(uint16), &Var_p, &Session);
    Do_ADbg_GetDataVar(sizeof(uint64), &Var_p, &Length);

    TypeLength = get_uint32_string_le((void **)&Var_p);
    Type_p = (char *)Do_ADbg_GetDataPointer(TypeLength, (void **)&Var_p);

    PathLength = get_uint32_string_le((void **)&Var_p);
    SourcePath_p = (char *)Do_ADbg_GetDataPointer(PathLength, (void **)&Var_p);

    Do_ADbg_GetDataVar(sizeof(uint32), &Var_p, &ExpectedResult);

    (void)Do_Flash_ProcessFileImpl(Session, Length, Type_p, SourcePath_p);

    BUFFER_FREE(Type_p);
    BUFFER_FREE(SourcePath_p);
}

/**
 * This function will test function:
 * Do_Flash_DumpAreaImpl
 * Function used in: Test_Do_Flash_DumpAreaImpl
 *
 * @param [in] Case_p          Pointer
 * @return void
 */
static void ADbg_Do_Flash_DumpAreaImpl(ADbg_Case_t *Case_p)
{
    uint16 Session = 0;
    char *Path_p = NULL;
    uint32 PathLength = 0;
    uint64 Start = 0;
    uint64 Length = 0;
    char *TargetPath_p = NULL;
    uint32 TargetPathLength = 0;
    uint8 *Var_p = NULL;
    uint32 RedundantArea = 0;

    Var_p = Case_p->Command_p->Data_p;
    Do_ADbg_GetDataVar(sizeof(uint16), &Var_p, &Session);

    PathLength = get_uint32_string_le((void **)&Var_p);
    Path_p = (char *)Do_ADbg_GetDataPointer(PathLength, (void **)&Var_p);

    Do_ADbg_GetDataVar(sizeof(uint64), &Var_p, &Start);
    Do_ADbg_GetDataVar(sizeof(uint64), &Var_p, &Length);

    TargetPathLength = get_uint32_string_le((void **)&Var_p);
    TargetPath_p = (char *)Do_ADbg_GetDataPointer(TargetPathLength, (void **)&Var_p);

    Do_ADbg_GetDataVar(sizeof(uint32), &Var_p, &ExpectedResult);
    Do_ADbg_GetDataVar(sizeof(uint32), &Var_p, &RedundantArea);


    (void)Do_Flash_DumpAreaImpl(Session, Path_p, Start, Length, TargetPath_p, RedundantArea);

    BUFFER_FREE(Path_p);
    BUFFER_FREE(TargetPath_p);
}

/**
 * This function will test function:
 * Do_Flash_ListDevicesImpl
 * Function used in: Test_Do_Flash_ListDevicesImpl
 *
 * @param [in] Case_p          Pointer
 * @return void
 */
static void ADbg_Do_Flash_ListDevicesImpl(ADbg_Case_t *Case_p)
{
    uint16 Session = 0;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Do_ADbg_GetDataVar(sizeof(uint16), &Var_p, &Session);

    Do_ADbg_GetDataVar(sizeof(uint32), &Var_p, &ExpectedResult);

    (void)Do_Flash_ListDevicesImpl(Session);
}

/**
 * This function will test function:
 * Do_Flash_EraseAreaImpl
 * Function used in: Test_Do_Flash_EraseAreaImpl
 *
 * @param [in] Case_p          Pointer
 * @return void
 */
static void ADbg_Do_Flash_EraseAreaImpl(ADbg_Case_t *Case_p)
{
    uint16 Session = 0;
    char *Path_p = NULL;
    uint32 PathLength = 0;
    uint64 Start = 0;
    uint64 Length = 0;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Do_ADbg_GetDataVar(sizeof(uint16), &Var_p, &Session);

    PathLength = get_uint32_string_le((void **)&Var_p);
    Path_p = (char *)Do_ADbg_GetDataPointer(PathLength, (void **)&Var_p);

    Do_ADbg_GetDataVar(sizeof(uint64), &Var_p, &Start);
    Do_ADbg_GetDataVar(sizeof(uint64), &Var_p, &Length);

    Do_ADbg_GetDataVar(sizeof(uint32), &Var_p, &ExpectedResult);

    (void)Do_Flash_EraseAreaImpl(Session, Path_p, Start, Length);

    BUFFER_FREE(Path_p);
}

/*@}*/
