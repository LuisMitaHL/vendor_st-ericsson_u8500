/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/

/**
 * @file ADbg_cnh<ModuleNumber>.c
 *
 * @brief Test cases for CNH1605720 module.
 *
 * This file consist of test cases for autometic testing functions from
 * CNH1605720 module.
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
#include <stdlib.h>

/*******************************************************************************
 * The test case functions declarations
 ******************************************************************************/
static void ADbg_CNH1605720_TestedFunction1(ADbg_Case_t *Case_p);

/*******************************************************************************
 * The test case definitions
 ******************************************************************************/
//static uint8 ParametersX[NumberOfParameters+1] = {NumberOfParameters(Input+Output), InputPar1,..InputParN, OutputPar}
//ADBG_CASE_DEFINE(TestCaseName, CaseId, ModuleId, TestCaseFunction, Parameters)

static uint8 Parameters1[3] = {2, SHORT, SHORT};
ADBG_CASE_DEFINE(Test_CNH1605720_TestedFunction1, 1, 13, ADbg_CNH1605720_TestedFunction1, Parameters1);


/*******************************************************************************
* The test case suite definition
******************************************************************************/
//ADBG_SUITE_DEFINE_BEGIN(TestCase_CNH1605720, 0, ModuleId)
ADBG_SUITE_DEFINE_BEGIN(TestCase_CNH1605720, 0, 13)

//ADBG_SUITE_ENTRY(TestCaseName)
ADBG_SUITE_ENTRY(Test_CNH1605720_TestedFunction1)

ADBG_SUITE_DEFINE_END();

/*******************************************************************************
 * Case runner
 ******************************************************************************/
void Do_CNH1605720_Module_Test_Run(ADbg_MainModule_t *MainModule_p)
{
    ADbg_Module_t *Module_p = Do_ADbg_Module_Create(TestCase_CNH1605720, NULL, MainModule_p->Command_p, MainModule_p->Result_p);
    Do_ADbg_InModule_Command(Module_p);
    Do_ADbg_Module_Destroy(Module_p);
}
/*******************************************************************************
 * Test cases
 ******************************************************************************/
/*note: The Test cases shown here are invented examples with purpose to show how
the Test Cases for functions should be written with different input and output parameters.
At the time the templates are going to be used, these examples should be removed and
replaced with the appropriate Cases which will be written. */

/*note: TestedFunction1 is invented function which takes as input just one argument
(unit16 Param1), and returns ErrorCode_e */
/**
 * This function will test function:
 * CNH1605720_TestedFunction1.
 * Function used in: Test_CNH1605720_TestedFunction1.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_CNH1605720_TestedFunction1(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_SUCCESS;
    uint16 Param1 = 0;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Do_ADbg_GetDataVar(sizeof(uint16), &Var_p, &Param1);

    //Result = TestedFunction1(Param1);
    Do_ADbg_Assert((ErrorCode_e)(*Var_p) == Result, Case_p);

}
