/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/

/**
 * @file ADbg_cnh1605203_utilities.c
 *
 * @brief Interface functions for CNH1605203 module.
 *
 * This file consist of interface functions for setting preconditions
 * for automatic testing functions from CNH1605203 module.
 */

/*
 * @addtogroup ADbg
 * @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "r_adbg.h"
#include "r_adbg_int_function.h"
#include "r_adbg_int_group.h"
#include "r_adbg_command.h"
#include "r_adbg_module.h"
#include "ADbg_cnh1605203_utilities.h"

/*******************************************************************************
 * The interface functions declarations
 ******************************************************************************/

static void ADbg_IntGroup1_CNH1605203_Group(ADbg_Module_t *Module_p);
/** Functions used for set the global variables. */
static void ADbg_Test_Function1_CNH1605203(ADbg_IntFunction_t *IntFunction_p);

/*******************************************************************************
 * The interface group definitions
 ******************************************************************************/
//ADBG_INTERFACE_GROUP_DEFINE(InterfaceGroupName, InterfaceGroupId, ModuleId, InterfaceFunctionRunner)
ADBG_INTERFACE_GROUP_DEFINE(Test_IntGroup1_CNH1605203_Group, 1, 7, ADbg_IntGroup1_CNH1605203_Group);

/*******************************************************************************
 * The interface function definitions
 ******************************************************************************/

static uint8 Param1[2] = {1, WORD};
static uint8 Param2[4];
ADBG_INTERFACE_FUNCTION_DEFINE(Test_Function1_CNH1605203, 1, 1, 7, ADbg_Test_Function1_CNH1605203, Param1, Param2);

/*******************************************************************************
 * The interface group suite definition
 ******************************************************************************/
//ADBG_INTERFACE_GROUP_SUITE_DEFINE_BEGIN(TestIntGroup_CNH1605203, 0, ModuleId)
ADBG_INTERFACE_GROUP_SUITE_DEFINE_BEGIN(IntGroup_CNH1605203, 0, 7)

//ADBG_INTERFACE_GROUP_SUITE_ENTRY(InterfaceGroupName)
ADBG_INTERFACE_GROUP_SUITE_ENTRY(Test_IntGroup1_CNH1605203_Group)

ADBG_INTERFACE_GROUP_SUITE_DEFINE_END();

/*******************************************************************************
 * The interface functions suites definition
 ******************************************************************************/
//ADBG_INTERFACE_FUNCTION_SUITE_DEFINE_BEGIN(TestIntFunctionSuiteName, 0, IntGroupId, ModuleId)

ADBG_INTERFACE_FUNCTION_SUITE_DEFINE_BEGIN(IntGroup1_CNH1605203, 0, 1, 7)

ADBG_INTERFACE_FUNCTION_SUITE_ENTRY(Test_Function1_CNH1605203)           //InterfaceFunctionName

ADBG_INTERFACE_FUNCTION_SUITE_DEFINE_END();

/*******************************************************************************
 * Interface group runner
 ******************************************************************************/
void Do_CNH1605203_Module_Interface_Run(ADbg_MainModule_t *MainModule_p)
{
    ADbg_Module_t *Module_p = Do_ADbg_Module_Create(NULL, IntGroup_CNH1605203, MainModule_p->Command_p, MainModule_p->Result_p);
    Do_ADbg_InModule_Command(Module_p);
    Do_ADbg_Module_Destroy(Module_p);
}

/*******************************************************************************
 * Interface functions from group1 runner
 ******************************************************************************/
static void ADbg_IntGroup1_CNH1605203_Group(ADbg_Module_t *Module_p)
{
    ADbg_IntGroup_t *IntGroup1_p = Do_ADbg_IntGroup_Create(IntGroup1_CNH1605203, Module_p->Command_p, Module_p->Result_p);
    Do_ADbg_InIntGroup_Command(IntGroup1_p);
    Do_ADbg_IntGroup_Destroy(IntGroup1_p);
}

/**
 * @brief Interface function.
 *
 * This function...
 *
 * @param [in] IntFunction_p is pointer.
 * @return     void.
 */
static void ADbg_Test_Function1_CNH1605203(ADbg_IntFunction_t *IntFunction_p)
{
    uint32 Param;

    //...
    Do_ADbg_SetAndRecoveryCondition((void *)&Param, Param2, sizeof(uint32), IntFunction_p);
}

/* @} */
