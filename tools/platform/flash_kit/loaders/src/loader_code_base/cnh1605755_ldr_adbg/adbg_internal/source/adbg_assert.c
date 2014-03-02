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
#include "r_debug_macro.h"

/*******************************************************************************
 * Definition of static functions
 ******************************************************************************/
/**
 * Function that checks if Data_p is pointing to array zeros, where size is
 * Size. If Data_p is pointer to array of zeros, sets Data_p = NULL.
 *
 * @param [in]     Size is size of buffer for testing.
 * @param [in]     Var_p is pointer to buffer for testing.
 * @return  Size   If Var_p is not pointer to zeros.
 * @retval  0      If Var_p is pointer to zeros.
 *
 * @remark       None.
 */
static boolean IsPointerNULL(uint32 Size, uint8 *Var_p);

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/

void *Do_ADbg_GetDataPointer(uint32 Size, void **Var_pp)
{
    uint8 *Data_p = NULL;
    uint8 *Var_p = *Var_pp;

    if (IsPointerNULL(Size, Var_p) == TRUE) {
        Data_p = (uint8 *)malloc(Size);
        ASSERT(NULL != Data_p);
        memcpy(Data_p, Var_p, Size);
    }

    *(uint8 **)Var_pp += Size;

    return (void *)Data_p;
}

void Do_ADbg_GetDataVar(uint32 Size,
                        uint8 **Var_pp,
                        void  *Param_p)
{
    memcpy(Param_p, *Var_pp, Size);
    *(uint8 **)Var_pp += Size;
}

void Do_ADbg_Assert(boolean      Expression,
                    ADbg_Case_t *Case_p)
{
    if (!Expression) {
        *(Case_p->Result_p->Data_p) = (uint8)ASSERT_FAILED;
        Case_p->Result_p->Size++;
    } else {
        *(Case_p->Result_p->Data_p) = (uint8)ASSERT_OK;
        Case_p->Result_p->Size++;
    }
}

static boolean IsPointerNULL(uint32 Size,
                             uint8 *Var_p)
{
    uint32 i;

    for (i = 0;  i < Size; i++) {
        if (*(Var_p + i) != 0) {
            return TRUE;
        }
    }

    return FALSE;
}

/* @} */
