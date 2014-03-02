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
#include <string.h>

#include "t_adbg.h"
#include "command_ids.h"
#include "r_debug_macro.h"

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/

ADbg_IntFunction_t *Do_ADbg_IntFunction_Create(void)
{
    ADbg_IntFunction_t *IntFunction_p = NULL;
    IntFunction_p = (ADbg_IntFunction_t *)malloc(sizeof(ADbg_IntFunction_t));
    ASSERT(NULL != IntFunction_p);
    return IntFunction_p;
}

void Do_ADbg_IntFunction_Destroy(ADbg_IntFunction_t *IntFunction_p)
{
    free(IntFunction_p);
    IntFunction_p = NULL;
}

void Do_ADbg_IntFunction_Init(ADbg_Result_t      *Result_p,
                              ADbg_Command_t     *Command_p,
                              ADbg_Recovery_t    *Recovery_p,
                              ADbg_IntFunction_t *IntFunction_p)
{
    IntFunction_p->Command_p = Command_p;
    IntFunction_p->Result_p = Result_p;
    IntFunction_p->Recovery_p = Recovery_p;
}

void Do_ADbg_SetAndRecoveryCondition(void               *Var_p,
                                     void               *VarBackup_p,
                                     uint8               Size,
                                     ADbg_IntFunction_t *IntFunction_p)
{
    switch (IntFunction_p->Command_p->Command) {
    case COMMAND_ADBG_SETPRECONDITION:

        if (IntFunction_p->Command_p->RecoveryFlag == TRUE) {
            if (IntFunction_p->Recovery_p->SetPrecondition != TRUE) {
                memcpy(VarBackup_p, Var_p, Size);
                IntFunction_p->Recovery_p->SetPrecondition = TRUE;
            } else {
                IntFunction_p->Result_p->ErrorValue = E_PRECONDITION_IS_ALREADY_SET;
            }
        }

        memcpy(Var_p, IntFunction_p->Command_p->Data_p, Size);
        break;
    case COMMAND_ADBG_RECOVERYCONDITION:

        if (IntFunction_p->Recovery_p->SetPrecondition == TRUE) {
            memcpy(Var_p, VarBackup_p, Size);
            IntFunction_p->Recovery_p->SetPrecondition = FALSE;
        } else {
            IntFunction_p->Result_p->ErrorValue = E_PRECONDITION_IS_NOT_SET;
        }

        break;
    default:
        break;
    }
}

/* @} */
