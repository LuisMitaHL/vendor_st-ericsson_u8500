/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
/**
 * @file ADbg_cnh1605195_utilities.c
 *
 * @brief Interface functions for CNH1605195 module.
 * This file consist of interface functions for setting preconditions
 * for automatic testing functions from CNH1605195 module.
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
#include "r_adbg_int_function.h"
#include "r_adbg_int_group.h"
#include "r_adbg_command.h"
#include "r_adbg_module.h"

/*******************************************************************************
 * The interface functions declarations
 ******************************************************************************/
//group1
static void ADbg_IntGroup1_CNH1605195_Group(ADbg_Module_t *Module_p);
/** functions used for set the global variables */
static void ADbg_IntFunction11_1(ADbg_IntFunction_t *IntFunction_p);

//group2
static void ADbg_IntGroup2_CNH1605195_Group(ADbg_Module_t *Module_p);
/** functions used for set the static variables */
static void ADbg_IntFunction22_1(ADbg_IntFunction_t *IntFunction_p);

//group3
static void ADbg_IntGroup3_CNH1605195_Group(ADbg_Module_t *Module_p);
/** functions used to get access to the static functions */
static void ADbg_IntFunction33_1(ADbg_IntFunction_t *IntFunction_p);


/*******************************************************************************
 * The interface group definitions
 ******************************************************************************/
//group1
ADBG_INTERFACE_GROUP_DEFINE(Test_IntGroup1_CNH1605195_Group, 1, 1, ADbg_IntGroup1_CNH1605195_Group);  //(InterfaceGroupName, InterfaceGroupId, ModuleId, InterfaceFunctionRunner)
//group2
ADBG_INTERFACE_GROUP_DEFINE(Test_IntGroup2_CNH1605195_Group, 2, 1, ADbg_IntGroup2_CNH1605195_Group);
//group3
ADBG_INTERFACE_GROUP_DEFINE(Test_IntGroup3_CNH1605195_Group, 3, 1, ADbg_IntGroup3_CNH1605195_Group);
/*******************************************************************************
 * The interface function definitions
 ******************************************************************************/
//group1 used for set the global variables
static uint8 Parameters11_1[1] = {1};
static uint8 Backup11_1[1];
ADBG_INTERFACE_FUNCTION_DEFINE(Test_IntFunction11_1, 1, 1, 1, ADbg_IntFunction11_1, Parameters11_1, Backup11_1);

//group2 used for set the static variables
static uint8 Parameters22_1[1] = {1};
static uint8 Backup22_1[1];
ADBG_INTERFACE_FUNCTION_DEFINE(Test_IntFunction22_1, 1, 2, 1, ADbg_IntFunction22_1, Parameters22_1, Backup22_1);

//group3 used to get access to the static functions
static uint8 Parameters33_1[1] = {1};
static uint8 Backup33_1[1];
ADBG_INTERFACE_FUNCTION_DEFINE(Test_IntFunction33_1, 1, 3, 1, ADbg_IntFunction33_1, Parameters33_1, Backup33_1);

/*******************************************************************************
 * The interface group suite definition
 ******************************************************************************/
ADBG_INTERFACE_GROUP_SUITE_DEFINE_BEGIN(IntGroup_CNH1605195, 0, 1)  //(TestIntGroup_CNH<ModuleNumber>, 0, ModuleId)

ADBG_INTERFACE_GROUP_SUITE_ENTRY(Test_IntGroup1_CNH1605195_Group)    //InterfaceGroupName
ADBG_INTERFACE_GROUP_SUITE_ENTRY(Test_IntGroup2_CNH1605195_Group)
ADBG_INTERFACE_GROUP_SUITE_ENTRY(Test_IntGroup3_CNH1605195_Group)

ADBG_INTERFACE_GROUP_SUITE_DEFINE_END();

/*******************************************************************************
 * The interface functions suites definition
 ******************************************************************************/
//group1 used for set the global variables
ADBG_INTERFACE_FUNCTION_SUITE_DEFINE_BEGIN(IntGroup1_CNH1605195, 0, 1, 1)  //(TestIntFunctionSuiteName, 0, IntGroupId, ModuleId)

ADBG_INTERFACE_FUNCTION_SUITE_ENTRY(Test_IntFunction11_1)    //InterfaceFunctionName

ADBG_INTERFACE_FUNCTION_SUITE_DEFINE_END();

//group2 used for set the static variables
ADBG_INTERFACE_FUNCTION_SUITE_DEFINE_BEGIN(IntGroup2_CNH1605195, 0, 2, 1)

ADBG_INTERFACE_FUNCTION_SUITE_ENTRY(Test_IntFunction22_1)

ADBG_INTERFACE_FUNCTION_SUITE_DEFINE_END();

//group3 used to get access to the static functions
ADBG_INTERFACE_FUNCTION_SUITE_DEFINE_BEGIN(IntGroup3_CNH1605195, 0, 3, 1)

ADBG_INTERFACE_FUNCTION_SUITE_ENTRY(Test_IntFunction33_1)

ADBG_INTERFACE_FUNCTION_SUITE_DEFINE_END();

/*******************************************************************************
 * Interface group runner
 ******************************************************************************/
void Do_CNH1605195_Module_Interface_Run(ADbg_MainModule_t *MainModule_p)
{
    ADbg_Module_t *Module_p = Do_ADbg_Module_Create(NULL, IntGroup_CNH1605195, MainModule_p->Command_p, MainModule_p->Result_p);
    Do_ADbg_InModule_Command(Module_p);
    Do_ADbg_Module_Destroy(Module_p);
}

/*******************************************************************************
 * Interface functions from group1 runner
 ******************************************************************************/
static void ADbg_IntGroup1_CNH1605195_Group(ADbg_Module_t *Module_p)
{
    ADbg_IntGroup_t *IntGroup1_p = Do_ADbg_IntGroup_Create(IntGroup1_CNH1605195, Module_p->Command_p, Module_p->Result_p);
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
static void ADbg_IntFunction11_1(ADbg_IntFunction_t *IntFunction_p)
{
    int Var;
    void *Var_p = &Var;

    Do_ADbg_SetAndRecoveryCondition(Var_p, Backup11_1, sizeof(int), IntFunction_p);
}

/*******************************************************************************
 * Interface functions from group2 runner
 ******************************************************************************/
static void ADbg_IntGroup2_CNH1605195_Group(ADbg_Module_t *Module_p)
{
    ADbg_IntGroup_t *IntGroup2_p = Do_ADbg_IntGroup_Create(IntGroup2_CNH1605195, Module_p->Command_p, Module_p->Result_p);
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
static void ADbg_IntFunction22_1(ADbg_IntFunction_t *IntFunction_p)
{
    static int Var;
    void *Var_p = &Var;

    Do_ADbg_SetAndRecoveryCondition(Var_p, Backup22_1, sizeof(int), IntFunction_p);
}

/*******************************************************************************
 * Interface functions from group3 runner
 ******************************************************************************/
static void ADbg_IntGroup3_CNH1605195_Group(ADbg_Module_t *Module_p)
{
    ADbg_IntGroup_t *IntGroup3_p = Do_ADbg_IntGroup_Create(IntGroup3_CNH1605195, Module_p->Command_p, Module_p->Result_p);
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
static void ADbg_IntFunction33_1(ADbg_IntFunction_t *IntFunction_p)
{
    int Var;
    void *Var_p = &Var;

    Do_ADbg_SetAndRecoveryCondition(Var_p, Backup33_1, sizeof(int), IntFunction_p);
}

/*@}*/
