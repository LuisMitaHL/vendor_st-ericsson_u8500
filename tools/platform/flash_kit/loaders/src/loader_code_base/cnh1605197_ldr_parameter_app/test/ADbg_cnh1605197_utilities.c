/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/

/*
 * @file ADbg_cnh1605197_utilities.c
 *
 * @brief Interface functions for CNH1605197 module.
 *
 * This file consist of interface functions for setting preconditions
 * for automatic testing functions from CNH1605197 module.
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

/*******************************************************************************
 * The interface functions declarations
 ******************************************************************************/
//group1
static void ADbg_IntGroup1_CNH1605197_Group(ADbg_Module_t *Module_p); //Interface group
/** functions used for set the global variables */
static void ADbg_IntFunction1(ADbg_IntFunction_t *IntFunction_p); //Interface function

//group2
static void ADbg_IntGroup2_CNH1605197_Group(ADbg_Module_t *Module_p);
/** functions used for set the static variables */
static void ADbg_IntFunction2(ADbg_IntFunction_t *IntFunction_p);

//group3
static void ADbg_IntGroup3_CNH1605197_Group(ADbg_Module_t *Module_p);
/** functions used to get access to the static functions */
static void ADbg_IntFunction3(ADbg_IntFunction_t *IntFunction_p);

/*******************************************************************************
 * The interface group definitions
 ******************************************************************************/
//group1
ADBG_INTERFACE_GROUP_DEFINE(Test_IntGroup1_CNH1605197_Group, 1, 4, ADbg_IntGroup1_CNH1605197_Group);  //(InterfaceGroupName, InterfaceGroupId, ModuleId, InterfaceFunctionRunner)
//group2
ADBG_INTERFACE_GROUP_DEFINE(Test_IntGroup2_CNH1605197_Group, 2, 4, ADbg_IntGroup2_CNH1605197_Group);
//group3
ADBG_INTERFACE_GROUP_DEFINE(Test_IntGroup3_CNH1605197_Group, 3, 4, ADbg_IntGroup3_CNH1605197_Group);

/*******************************************************************************
 * The interface function definitions
 ******************************************************************************/
//group1 used for set the global variables
static uint8 Parameters1[2] = {1};
static uint8 Backup1[1];
ADBG_INTERFACE_FUNCTION_DEFINE(Test_IntFunction1, 1, 1, 4, ADbg_IntFunction1, Parameters1, Backup1);

//group2 used for set the static variables
static uint8 Parameters2[1] = {1};
static uint8 Backup2[1];
ADBG_INTERFACE_FUNCTION_DEFINE(Test_IntFunction2, 1, 2, 4, ADbg_IntFunction2, Parameters2, Backup2);

//group3 used to get access to the static functions
static uint8 Parameters3[1] = {1};
static uint8 Backup3[1];
ADBG_INTERFACE_FUNCTION_DEFINE(Test_IntFunction3, 1, 3, 4, ADbg_IntFunction3, Parameters3, Backup3);

/*******************************************************************************
 * The interface group suite definition
 ******************************************************************************/
ADBG_INTERFACE_GROUP_SUITE_DEFINE_BEGIN(IntGroup_CNH1605197, 0, 4)   //(TestIntGroup_CNH<ModuleNumber>, 0, ModuleId)

ADBG_INTERFACE_GROUP_SUITE_ENTRY(Test_IntGroup1_CNH1605197_Group)    //InterfaceGroupName
ADBG_INTERFACE_GROUP_SUITE_ENTRY(Test_IntGroup2_CNH1605197_Group)
ADBG_INTERFACE_GROUP_SUITE_ENTRY(Test_IntGroup3_CNH1605197_Group)

ADBG_INTERFACE_GROUP_SUITE_DEFINE_END();

/*******************************************************************************
 * The interface functions suites definition
 ******************************************************************************/
//group1 used for set the global variables
ADBG_INTERFACE_FUNCTION_SUITE_DEFINE_BEGIN(IntGroup1_CNH1605197, 0, 1, 4)  //(TestIntFunctionSuiteName, 0, IntGroupId, ModuleId)

ADBG_INTERFACE_FUNCTION_SUITE_ENTRY(Test_IntFunction1)    //InterfaceFunctionName

ADBG_INTERFACE_FUNCTION_SUITE_DEFINE_END();

//group2 used for set the static variables
ADBG_INTERFACE_FUNCTION_SUITE_DEFINE_BEGIN(IntGroup2_CNH1605197, 0, 2, 4)

ADBG_INTERFACE_FUNCTION_SUITE_ENTRY(Test_IntFunction2)

ADBG_INTERFACE_FUNCTION_SUITE_DEFINE_END();

//group3 used to get access to the static functions
ADBG_INTERFACE_FUNCTION_SUITE_DEFINE_BEGIN(IntGroup3_CNH1605197, 0, 3, 4)

ADBG_INTERFACE_FUNCTION_SUITE_ENTRY(Test_IntFunction3)

ADBG_INTERFACE_FUNCTION_SUITE_DEFINE_END();

/*******************************************************************************
 * Interface group runner
 ******************************************************************************/
void Do_CNH1605197_Module_Interface_Run(ADbg_MainModule_t *MainModule_p)
{
    ADbg_Module_t *Module_p = Do_ADbg_Module_Create(NULL, IntGroup_CNH1605197, MainModule_p->Command_p, MainModule_p->Result_p);
    Do_ADbg_InModule_Command(Module_p);
    Do_ADbg_Module_Destroy(Module_p);
}

/*******************************************************************************
 * Interface functions from group1 runner
 ******************************************************************************/
static void ADbg_IntGroup1_CNH1605197_Group(ADbg_Module_t *Module_p)
{
    ADbg_IntGroup_t *IntGroup1_p = Do_ADbg_IntGroup_Create(IntGroup1_CNH1605197, Module_p->Command_p, Module_p->Result_p);
    Do_ADbg_InIntGroup_Command(IntGroup1_p);
    Do_ADbg_IntGroup_Destroy(IntGroup1_p);
}

/**
 * Interface function.
 * This function sets the external variable
 *
 * @param [in] IntFunction_p          Pointer
 * @return void
 */
static void ADbg_IntFunction1(ADbg_IntFunction_t *IntFunction_p)
{
    int Var = 0;
    void *Var_p = &Var;

    Do_ADbg_SetAndRecoveryCondition(Var_p, Backup1, sizeof(int), IntFunction_p);
}

/***********************************************************************
 * Interface functions from group2 runner
 **********************************************************************/
static void ADbg_IntGroup2_CNH1605197_Group(ADbg_Module_t *Module_p)
{
    ADbg_IntGroup_t *IntGroup2_p = Do_ADbg_IntGroup_Create(IntGroup2_CNH1605197, Module_p->Command_p, Module_p->Result_p);
    Do_ADbg_InIntGroup_Command(IntGroup2_p);
    Do_ADbg_IntGroup_Destroy(IntGroup2_p);
}

/**
 * Interface function.
 * This function set the static variable
 *
 * @param [in] IntFunction_p          Pointer
 * @return void
 */
static void ADbg_IntFunction2(ADbg_IntFunction_t *IntFunction_p)
{
    int Var = 0;
    void *Var_p = &Var;

    Do_ADbg_SetAndRecoveryCondition(Var_p, Backup2, sizeof(int), IntFunction_p);
}

/***********************************************************************
 * Interface functions from group3 runner
 **********************************************************************/
static void ADbg_IntGroup3_CNH1605197_Group(ADbg_Module_t *Module_p)
{
    ADbg_IntGroup_t *IntGroup3_p = Do_ADbg_IntGroup_Create(IntGroup3_CNH1605197, Module_p->Command_p, Module_p->Result_p);
    Do_ADbg_InIntGroup_Command(IntGroup3_p);
    Do_ADbg_IntGroup_Destroy(IntGroup3_p);
}

/**
 * Interface function.
 * This function get access to the static function
 *
 *
 * @param [in] IntFunction_p          Pointer
 * @return void
 */
static void ADbg_IntFunction3(ADbg_IntFunction_t *IntFunction_p)
{
    int Var = 0;
    void *Var_p = &Var;

    Do_ADbg_SetAndRecoveryCondition(Var_p, Backup3, sizeof(int), IntFunction_p);
}

/* @} */
