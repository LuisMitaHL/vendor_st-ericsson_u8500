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
#include <stdlib.h>

#include "r_adbg_result.h"
#include "r_adbg_case.h"
#include "r_debug_macro.h"

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/

ADbg_Module_t *Do_ADbg_Module_Create(const ADbg_CaseDefinition_t     **const TestCase_pp,
                                     const ADbg_IntGroupDefinition_t **const TestIntGroup_pp,
                                     ADbg_Command_t                   *Command_p,
                                     ADbg_Result_t                    *Result_p)
{
    ADbg_Module_t *Module_p = NULL;
    Module_p = (ADbg_Module_t *)malloc(sizeof(ADbg_Module_t));
    ASSERT(NULL != Module_p);

    if (Module_p != NULL) {
        Module_p->CaseCounter = 0;
        Module_p->IntGroupCounter = 0;
        Module_p->TestCase_pp = TestCase_pp;
        Module_p->TestIntGroup_pp = TestIntGroup_pp;
        Module_p->Command_p = Command_p;
        Module_p->Result_p = Result_p;
    }

    return Module_p;
}

void Do_ADbg_Module_Destroy(ADbg_Module_t *Module_p)
{
    free(Module_p);
    Module_p = NULL;
}

const ADbg_IntGroupDefinition_t *Do_ADbg_FindIntGroup(ADbg_Module_t *Module_p)
{
    const ADbg_IntGroupDefinition_t **TestIntGroup_pp = Module_p->TestIntGroup_pp;
    uint32 IntGroupId = Module_p->Command_p->IntGroupId;

    while (*TestIntGroup_pp != NULL) {
        if ((*TestIntGroup_pp)->IntGroupId == IntGroupId) {
            return *TestIntGroup_pp;
        }

        TestIntGroup_pp++;
    }

    return NULL;
}

void Do_ADbg_InModule_ListCase(ADbg_Module_t *Module_p)
{
    const ADbg_CaseDefinition_t **TestCase_pp = &Module_p->TestCase_pp[0];

    TestCase_pp++;

    if (*TestCase_pp != NULL) {
        uint8 *CaseCounter_p = Module_p->Result_p->Data_p;
        Module_p->Result_p->Data_p++;
        Module_p->Result_p->Size++;

        while (*TestCase_pp != NULL) {
            Module_p->TestCase_pp = TestCase_pp;
            Do_CopyCase_List_Result(Module_p);

            TestCase_pp++;
            Module_p->CaseCounter++;
        }

        *CaseCounter_p = Module_p->CaseCounter;
    } else {
        Module_p->Result_p->ErrorValue = E_CASE_LIST_EMPTY;
    }
}

void Do_ADbg_InModule_Run(const ADbg_CaseDefinition_t *TestCase_p,
                          ADbg_Module_t         *Module_p)
{
    if (TestCase_p != NULL) {
        ADbg_Case_t *Case_p = TestCase_p->Interface.Create();

        if (Case_p != NULL) {
            //    Initialize data
            Do_ADbg_Case_Init(Module_p->Result_p, Module_p->Command_p, Case_p);

            //    Run Case
            TestCase_p->Interface.Run((void *)Case_p);

            //    Release memory space for Case
            TestCase_p->Interface.Destroy((void *)Case_p);
        }
    } else {
        Module_p->Result_p->ErrorValue = E_CASE_NOT_FOUND;
    }
}

void Do_ADbg_InOneIntGroup(const ADbg_IntGroupDefinition_t *TestIntGroup_p,
                           ADbg_Module_t             *Module_p)
{
    if (TestIntGroup_p != NULL) {
        TestIntGroup_p->Interface.Run(Module_p);
    } else {
        Module_p->Result_p->ErrorValue = E_INT_GROUP_NOT_FOUND;
    }
}

void Do_ADbg_InAllIntGroups(ADbg_Module_t *Module_p)
{
    const ADbg_IntGroupDefinition_t **TestIntGroup_pp = &Module_p->TestIntGroup_pp[0];

    TestIntGroup_pp++;

    if (*TestIntGroup_pp != NULL) {
        uint8 *IntGroupCounter_p = Module_p->Result_p->Data_p;

        Module_p->Result_p->Data_p++;
        Module_p->Result_p->Size++;

        while (*TestIntGroup_pp != NULL) {
            const ADbg_IntGroupDefinition_t *TestIntGroup_p = *TestIntGroup_pp;
            //IntGroup name and Id
            Do_CopyIntGroup_List_Result(TestIntGroup_p, Module_p);

            Do_ADbg_InOneIntGroup(TestIntGroup_p, Module_p);
            TestIntGroup_pp++;
            Module_p->IntGroupCounter++;
        }

        *IntGroupCounter_p = Module_p->IntGroupCounter;
    } else {
        Module_p->Result_p->ErrorValue = E_INT_GROUP_LIST_EMPTY;
    }
}

const ADbg_CaseDefinition_t *Do_ADbg_FindCase(ADbg_Module_t *Module_p)
{
    const ADbg_CaseDefinition_t **TestCase_pp = Module_p->TestCase_pp;
    uint32 CaseId = Module_p->Command_p->CaseId;

    while (*TestCase_pp != NULL) {
        if ((*TestCase_pp)->CaseId == CaseId) {
            return *TestCase_pp;
        }

        TestCase_pp++;
    }

    return NULL;
}

/* @} */
