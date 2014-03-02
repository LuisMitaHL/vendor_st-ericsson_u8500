/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/

/*
 * @addtogroup ldr_utilities
 * @{
 *   @addtogroup bitset
 *   @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "r_bitset.h"
#include "r_debug.h"
#include "r_debug_macro.h"
#include <stdio.h>
#include <stdlib.h>

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/
BitSet32_t Do_BitSet32_EmptySet()
{
    return 0;
}

BitSet32_t Do_BitSet32_UniversalSet()
{
    return ~0;
}

boolean Do_BitSet32_Insert(BitSet32_t *Set_p, int V)
{
    uint32 Encoded = 1;

    /* Can't insert items outside the accepted range */
    if (!ISVALID(V)) {
        return FALSE;
    }

    Encoded <<= V;

    /* Can't insert items already present */
    if ((*Set_p & Encoded) != 0) {
        return FALSE;
    }

    *Set_p |= Encoded;

    return TRUE;
}

boolean Do_BitSet32_Remove(BitSet32_t *Set_p, int V)
{
    uint32 Encoded = 1;

    /* We know these values are not in the set */
    if (!ISVALID(V)) {
        return FALSE;
    }

    Encoded <<= V;

    /* Value not present in the set */
    if ((*Set_p & Encoded) != Encoded) {
        return FALSE;
    }

    *Set_p ^= Encoded;

    return TRUE;
}

boolean Do_BitSet32_Contains(BitSet32_t Set, int V)
{
    uint32 Encoded = 1;

    /* We know these values are not in the set */
    if (!ISVALID(V)) {
        return FALSE;
    }

    Encoded <<= V;

    return (Set & Encoded) == Encoded;
}

BitSet32_t Do_BitSet32_Union(BitSet32_t Set1, BitSet32_t Set2)
{
    return Set1 | Set2;
}

BitSet32_t Do_BitSet32_Intersection(BitSet32_t Set1, BitSet32_t Set2)
{
    return Set1 & Set2;
}

BitSet32_t Do_BitSet32_Difference(BitSet32_t Set1, BitSet32_t Set2)
{
    return (Set1 ^ Set2) & Set1;
}

int Do_BitSet32_GetNext(BitSet32_t Set, int Last)
{
    uint32 n = 0;

    if (Last != -1 && !ISVALID(Last)) {
        return -1;
    }

    for (n = Last + 1; n < BITCOUNT; n++) {
        if (Do_BitSet32_Contains(Set, n)) {
            return n;
        }
    }

    return -1;
}

int Do_BitSet32_GetPrevious(BitSet32_t Set, int Last)
{
    int n = 0;

    if (Last != 32 && !ISVALID(Last)) {
        return -1;
    }

    for (n = Last - 1; n >= 0; n--) {
        if (Do_BitSet32_Contains(Set, n)) {
            return n;
        }
    }

    return -1;
}

C_(void Do_BitSet32_Print(BitSet32_t BitSet, char *Names[], int NrOfNames)
{
    int i = 0;

    for (i = Do_BitSet32_GetNext(BitSet, -1); i != -1; i = Do_BitSet32_GetNext(BitSet, i)) {
        if ((i > NrOfNames) || (NULL == Names)) {
            printf("Element[%d] = %d\n", i, i);
        } else {
            printf("Element[%d] = %s\n", i, Names[i]);
        }
    }
})

/* @} */
/* @} */
