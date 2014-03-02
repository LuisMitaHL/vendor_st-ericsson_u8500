/*                               -*- Mode: C -*-
 * Copyright (C) 2010, ST-Ericsson
 *
 * File name       : apdu_utilities.c
 * Description     : internal utilities
 *
 * Author          : Mats Bergstrom <mats.m.bergstrom@stericsson.com>
 *
 */

// FIXME: Change header to Haiyuans as author.

#include "apdu_utilities.h"


uint8_t                *STE_SAT_Heap_Alloc(unsigned int Size)
{
    uint8_t                *pMem;

    pMem = malloc(Size);

    if (pMem) {
        memset(pMem, 0, Size);
    }
    return pMem;

}

void STE_SAT_Heap_Free(uint8_t * pMem)
{
    free(pMem);
}
