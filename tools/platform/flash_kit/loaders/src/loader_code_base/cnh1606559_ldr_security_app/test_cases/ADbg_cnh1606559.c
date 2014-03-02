/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/

/**
 * @file ADbg_cnh1606559.c
 *
 * @brief Test cases for CNH1606559 module.
 *
 * This file consist of test cases for automatic testing functions from
 * CNH1606559 module.
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
#include "string.h"
#include "r_serialization.h"
#include "r_memory_utils.h"

/*******************************************************************************
 * The test case functions declarations
 ******************************************************************************/
static void ADbg_Do_Security_SetDomainImpl(ADbg_Case_t *Case_p);
static void ADbg_Do_Security_GetDomainImpl(ADbg_Case_t *Case_p);
static void ADbg_Do_Security_GetPropertiesImpl(ADbg_Case_t *Case_p);
static void ADbg_Do_Security_SetPropertiesImpl(ADbg_Case_t *Case_p);
static void ADbg_Do_Security_BindPropertiesImpl(ADbg_Case_t *Case_p);
//static void ADbg_Do_Security_StoreSecureObjectImpl(ADbg_Case_t* Case_p);

/*******************************************************************************
 * The test case definitions
 ******************************************************************************/
static uint8 Parameters1[4] = {3, SHORT, WORD, WORD};
ADBG_CASE_DEFINE(Test_Do_Security_SetDomainImpl, 1, 17, ADbg_Do_Security_SetDomainImpl, Parameters1);
static uint8 Parameters2[5] = {4, SHORT, WORD, WORD, WORD};
ADBG_CASE_DEFINE(Test_Do_Security_GetDomainImpl, 2, 17, ADbg_Do_Security_GetDomainImpl, Parameters2);
static uint8 Parameters3[4] = {3, SHORT, WORD, WORD};
ADBG_CASE_DEFINE(Test_Do_Security_GetPropertiesImpl, 3, 17, ADbg_Do_Security_GetPropertiesImpl, Parameters3);
static uint8 Parameters4[7] = {6, SHORT, WORD, WORD, WORD, VOIDPTR, WORD};
ADBG_CASE_DEFINE(Test_Do_Security_SetPropertiesImpl, 4, 17, ADbg_Do_Security_SetPropertiesImpl, Parameters4);
static uint8 Parameters5[3] = {2, SHORT, WORD};
ADBG_CASE_DEFINE(Test_Do_Security_BindPropertiesImpl, 5, 17, ADbg_Do_Security_BindPropertiesImpl, Parameters5);
//static uint8 Parameters6[] = {2, SHORT, CHARPTR, WORD};
//ADBG_CASE_DEFINE(Test_Do_Security_StoreSecureObjectImpl, 6, 17, ADbg_Do_Security_StoreSecureObjectImpl, Parameters6);

/*******************************************************************************
* The test case suite definition
******************************************************************************/
ADBG_SUITE_DEFINE_BEGIN(TestCase_CNH1606559, 0, 17)

ADBG_SUITE_ENTRY(Test_Do_Security_SetDomainImpl)
ADBG_SUITE_ENTRY(Test_Do_Security_GetDomainImpl)
ADBG_SUITE_ENTRY(Test_Do_Security_GetPropertiesImpl)
ADBG_SUITE_ENTRY(Test_Do_Security_SetPropertiesImpl)
ADBG_SUITE_ENTRY(Test_Do_Security_BindPropertiesImpl)
//ADBG_SUITE_ENTRY(Test_Do_Security_StoreSecureObjectImpl)

ADBG_SUITE_DEFINE_END();

/*******************************************************************************
 * Case runner
 ******************************************************************************/
void Do_CNH1606559_Module_Test_Run(ADbg_MainModule_t *MainModule_p)
{
    ADbg_Module_t *Module_p = Do_ADbg_Module_Create(TestCase_CNH1606559, NULL, MainModule_p->Command_p, MainModule_p->Result_p);
    Do_ADbg_InModule_Command(Module_p);
    Do_ADbg_Module_Destroy(Module_p);
}
/*******************************************************************************
 * Test cases
 ******************************************************************************/
/**
 * This function will test function:
 * Do_Security_SetDomainImpl.
 * Function used in: Test_Do_Security_SetDomainImpl.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_Do_Security_SetDomainImpl(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_GENERAL_FATAL_ERROR;
    uint16 Session = 0;
    uint32 Domain = 0;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Do_ADbg_GetDataVar(sizeof(uint16), &Var_p, &Session);
    Do_ADbg_GetDataVar(sizeof(uint32), &Var_p, &Domain);

    Result = Do_Security_SetDomainImpl(Session, Domain);
    Do_ADbg_Assert((ErrorCode_e)(*Var_p) == Result, Case_p);
}

/**
 * This function will test function:
 * Do_Security_GetDomainImpl.
 * Function used in: Test_Do_Security_GetDomainImpl.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_Do_Security_GetDomainImpl(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_GENERAL_FATAL_ERROR;
    uint16 Session = 0;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Do_ADbg_GetDataVar(sizeof(uint16), &Var_p, &Session);

    Result = Do_Security_GetDomainImpl(Session);
    Do_ADbg_Assert((ErrorCode_e)(*Var_p) == Result, Case_p);
}

/**
 * This function will test function:
 * Do_Security_GetPropertiesImpl.
 * Function used in: Test_Do_Security_GetPropertiesImpl.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_Do_Security_GetPropertiesImpl(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_GENERAL_FATAL_ERROR;
    uint16 Session = 0;
    uint32 Unit_id = 0;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Do_ADbg_GetDataVar(sizeof(uint16), &Var_p, &Session);
    Do_ADbg_GetDataVar(sizeof(uint32), &Var_p, &Unit_id);

    Result = Do_Security_GetPropertiesImpl(Session, Unit_id);
    Do_ADbg_Assert((ErrorCode_e)(*Var_p) == Result, Case_p);
}

/**
 * This function will test function:
 * Do_Security_SetPropertiesImpl.
 * Function used in: Test_Do_Security_SetPropertiesImpl.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_Do_Security_SetPropertiesImpl(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_GENERAL_FATAL_ERROR;
    uint16 Session = 0;
    uint32 Unit_id = 0;
    uint32 DataLength = 0;
    void *Data_p = NULL;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Do_ADbg_GetDataVar(sizeof(uint16), &Var_p, &Session);
    Do_ADbg_GetDataVar(sizeof(uint32), &Var_p, &Unit_id);
    Do_ADbg_GetDataVar(sizeof(uint32), &Var_p, &DataLength);

    Data_p = Do_ADbg_GetDataPointer(DataLength, (void **)&Var_p);

    Result = Do_Security_SetPropertiesImpl(Session, Unit_id, DataLength, Data_p);
    Do_ADbg_Assert((ErrorCode_e)(*Var_p) == Result, Case_p);

    BUFFER_FREE(Data_p);
}

/**
 * This function will test function:
 * Do_Security_BindPropertiesImpl.
 * Function used in: Test_Do_Security_BindPropertiesImpl.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
static void ADbg_Do_Security_BindPropertiesImpl(ADbg_Case_t *Case_p)
{
    ErrorCode_e Result = E_GENERAL_FATAL_ERROR;
    uint16 Session = 0;
    uint8 *Var_p = NULL;

    Var_p = Case_p->Command_p->Data_p;
    Do_ADbg_GetDataVar(sizeof(uint16), &Var_p, &Session);

    Result = Do_Security_BindPropertiesImpl(Session);
    Do_ADbg_Assert((ErrorCode_e)(*Var_p) == Result, Case_p);
}


/**
 * This function will test function:
 * Do_Security_StoreSecureObjectImpl.
 * Function used in: Test_Do_Security_StoreSecureObjectImpl.
 *
 * @param [in] Case_p is pointer.
 * @return     void.
 */
/*static void ADbg_Do_Security_StoreSecureObjectImpl(ADbg_Case_t* Case_p)
{
  ErrorCode_e Result = E_GENERAL_FATAL_ERROR;
  uint16 Session = 0;
  char *SourcePath_p = NULL;
  uint32 PathLength = 0;
  uint32 SecureObjectDestination = 0;
  uint8* Var_p = NULL;

  Var_p = Case_p->Command_p->Data_p;
  Do_ADbg_GetDataVar(sizeof(uint16), &Var_p, &Session);

  PathLength = get_uint32_string_le((void **)&Var_p);
  SourcePath_p = (char*)Do_ADbg_GetDataPointer(PathLength, (void **)&Var_p);

  Do_ADbg_GetDataVar(sizeof(uint32), &Var_p, &SecureObjectDestination);

  Result = Do_Security_StoreSecureObjectImpl(Session, SourcePath_p, SecureObjectDestination);
  Do_ADbg_Assert((ErrorCode_e)(*Var_p) == Result, Case_p);

  BUFFER_FREE(SourcePath_p);
}

*/

/* @}*/
