/*******************************************************************************
 * $Copyright Ericsson AB 2009 $
 ******************************************************************************/

/*
 * @addtogroup ADbg
 * @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <string.h>

#include "t_adbg.h"
#include "r_serialization.h"

/*******************************************************************************
 * Declaration of file local functions
 ******************************************************************************/
/**
 * @brief Function that copies string to buffer for payload.
 *
 * @param [in]     String_p is pointer to string that should be copied to
 *                 general response payload.
 * @param [in/out] Result_p is pointer to structure for preparing
 *                 general response payload.
 * @return         None.
 *
 * @remark         It dosen’t check the input parameters.
 */
static void Do_CopyString_Result(char          *String_p,
                                 ADbg_Result_t *Result_p);

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/

void Do_ADbg_Result_Init(uint8 *StartData_p, ADbg_Result_t *Result_p)
{
    Result_p->Size = 0;
    Result_p->Data_p = StartData_p;
    Result_p->StartData_p = StartData_p;
    Result_p->ErrorValue = E_SUCCESS;
}

void Do_CopyIntFunction_List_Result(ADbg_IntGroup_t *IntGroup_p)
{
    const ADbg_IntFunctionDefinition_t *TestIntFunction_p = *(IntGroup_p->TestIntFunction_pp);

    //copy IntFunction name
    Do_CopyString_Result(TestIntFunction_p->Name, IntGroup_p->Result_p);
    //copy IntFunctionId
    *(IntGroup_p->Result_p->Data_p) = (uint8)(TestIntFunction_p->IntFunctionId);
    IntGroup_p->Result_p->Data_p++;
    IntGroup_p->Result_p->Size++;
    //copy parameters
    memcpy(IntGroup_p->Result_p->Data_p, TestIntFunction_p->Parameters_p, *(TestIntFunction_p->Parameters_p) + 1);
    IntGroup_p->Result_p->Data_p = IntGroup_p->Result_p->Data_p + *(TestIntFunction_p->Parameters_p) + 1;
    IntGroup_p->Result_p->Size = IntGroup_p->Result_p->Size + *(TestIntFunction_p->Parameters_p) + 1;
}

void Do_CopyIntGroup_List_Result(const ADbg_IntGroupDefinition_t *TestIntGroup_p,
                                 ADbg_Module_t             *Module_p)
{
    //copy IntGroup name
    Do_CopyString_Result(TestIntGroup_p->Name, Module_p->Result_p);
    //copy IntGroupId
    *(Module_p->Result_p->Data_p) = TestIntGroup_p->IntGroupId;
    Module_p->Result_p->Data_p++;
    Module_p->Result_p->Size++;
}

void Do_CopyCase_List_Result(ADbg_Module_t *Module_p)
{
    const ADbg_CaseDefinition_t *TestCase_p = *(Module_p->TestCase_pp);

    //copy Case name
    Do_CopyString_Result(TestCase_p->Name, Module_p->Result_p);
    //copy CaseId
    *(Module_p->Result_p->Data_p) = TestCase_p->CaseId;
    Module_p->Result_p->Data_p++;
    Module_p->Result_p->Size++;
    //copy parameters
    memcpy(Module_p->Result_p->Data_p, TestCase_p->Parameters_p, *(TestCase_p->Parameters_p) + 1);
    Module_p->Result_p->Data_p = Module_p->Result_p->Data_p + *(TestCase_p->Parameters_p) + 1;
    Module_p->Result_p->Size = Module_p->Result_p->Size + *(TestCase_p->Parameters_p) + 1;
}

void Do_CopyModule_List_Result(const ADbg_ModuleDefinition_t *TestModule_p,
                               ADbg_MainModule_t       *MainModule_p)
{
    //copy Module name
    Do_CopyString_Result(TestModule_p->Name, MainModule_p->Result_p);
    //copy ModuleId
    *MainModule_p->Result_p->Data_p = TestModule_p->ModuleId;
    MainModule_p->Result_p->Data_p++;
    MainModule_p->Result_p->Size++;
}

/*******************************************************************************
 * Definition of file local functions
 ******************************************************************************/

static void Do_CopyString_Result(char          *String_p,
                                 ADbg_Result_t *Result_p)
{
    uint32 StringLen = 0;

    StringLen = strlen(String_p);
    put_string((void **)&Result_p->Data_p, String_p, StringLen);
    Result_p->Size += sizeof(uint32) + StringLen;
}

/* @} */
