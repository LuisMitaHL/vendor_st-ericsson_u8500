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
#include "r_adbg_int_function.h"
#include "r_debug_macro.h"

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/

ADbg_IntGroup_t *Do_ADbg_IntGroup_Create(ADbg_IntFunctionDefinition_t **const TestIntFunction_pp,
        ADbg_Command_t                      *Command_p,
        ADbg_Result_t                       *Result_p)
{
    ADbg_IntGroup_t *IntGroup_p = NULL;
    IntGroup_p = (ADbg_IntGroup_t *)malloc(sizeof(ADbg_IntGroup_t));
    ASSERT(NULL != IntGroup_p);

    if (IntGroup_p != NULL) {
        IntGroup_p->IntFunctionCounter = 0;
        IntGroup_p->TestIntFunction_pp = TestIntFunction_pp;
        IntGroup_p->Command_p = Command_p;
        IntGroup_p->Result_p = Result_p;
    }

    return IntGroup_p;
}

void Do_ADbg_IntGroup_Destroy(ADbg_IntGroup_t *IntGroup_p)
{
    free(IntGroup_p);
    IntGroup_p = NULL;
}

ADbg_IntFunctionDefinition_t *Do_ADbg_FindIntFunction(ADbg_IntGroup_t *IntGroup_p)
{
    ADbg_IntFunctionDefinition_t **TestIntFunction_pp = IntGroup_p->TestIntFunction_pp;
    uint32 IntFunctionId = IntGroup_p->Command_p->IntFunctionId;

    while (*TestIntFunction_pp != NULL) {
        if ((*TestIntFunction_pp)->IntFunctionId == IntFunctionId) {
            return *TestIntFunction_pp;
        }

        TestIntFunction_pp++;
    }

    return NULL;
}

void Do_ADbg_InIntGroup_ListInterface(ADbg_IntGroup_t *IntGroup_p)
{
    ADbg_IntFunctionDefinition_t **TestIntFunction_pp = &IntGroup_p->TestIntFunction_pp[0];

    TestIntFunction_pp++;

    if (*TestIntFunction_pp != NULL) {
        uint8 *IntFunctionCounter_p = IntGroup_p->Result_p->Data_p;
        IntGroup_p->Result_p->Data_p++;
        IntGroup_p->Result_p->Size++;

        while (*TestIntFunction_pp != NULL) {
            IntGroup_p->TestIntFunction_pp = TestIntFunction_pp;
            Do_CopyIntFunction_List_Result(IntGroup_p);

            TestIntFunction_pp++;
            IntGroup_p->IntFunctionCounter++;
        }

        *IntFunctionCounter_p = IntGroup_p->IntFunctionCounter;
    } else {
        IntGroup_p->Result_p->ErrorValue = E_INT_FUNCTION_LIST_EMPTY;
    }
}

void Do_ADbg_RunIntFunction(ADbg_IntFunctionDefinition_t *TestIntFunction_p,
                            ADbg_IntGroup_t              *IntGroup_p)
{
    ADbg_IntFunction_t *IntFunction_p = NULL;

    if (TestIntFunction_p != NULL) {
        IntFunction_p = TestIntFunction_p->Interface.Create();

        if (IntFunction_p != NULL) {
            ADbg_Recovery_t *Recovery_p = &(TestIntFunction_p->Recovery);
            //    Initialize data
            Do_ADbg_IntFunction_Init(IntGroup_p->Result_p, IntGroup_p->Command_p, Recovery_p, IntFunction_p);

            //    Run IntFunction
            TestIntFunction_p->Interface.Run((void *)IntFunction_p);

            //    Release memory space for IntFunction
            TestIntFunction_p->Interface.Destroy((void *)IntFunction_p);
        }
    } else {
        IntGroup_p->Result_p->ErrorValue = E_INT_FUNCTION_NOT_FOUND;
    }
}

/* @} */
