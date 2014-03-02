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

#include "t_adbg.h"
#include "r_debug_macro.h"

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/

ADbg_Case_t *Do_ADbg_Case_Create(void)
{
    ADbg_Case_t *Case_p = NULL;
    Case_p = (ADbg_Case_t *)malloc(sizeof(ADbg_Case_t));
    ASSERT(NULL != Case_p);
    return Case_p;
}

void Do_ADbg_Case_Destroy(ADbg_Case_t *Case_p)
{
    free(Case_p);
    Case_p = NULL;
}

void Do_ADbg_Case_Init(ADbg_Result_t  *Result_p,
                       ADbg_Command_t *Command_p,
                       ADbg_Case_t    *Case_p)
{
    Case_p->Command_p = Command_p;
    Case_p->Result_p = Result_p;
}

/* @} */
