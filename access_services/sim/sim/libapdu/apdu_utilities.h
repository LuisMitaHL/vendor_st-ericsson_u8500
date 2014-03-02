/*                               -*- Mode: C -*- 
 * Copyright (C) 2010, ST-Ericsson
 * 
 * File name       : apdu_utilities.h
 * Description     : internal definitions for apdu library.
 * 
 * Author          : Mats Bergstrom <mats.m.bergstrom@stericsson.com>
 * 
 */

#ifndef __apdu_utilities_h__
#define __apdu_utilities_h__ (1)

// FIXME: DOXYGEN!
// FIXME: Change header to Haiyuans as author.


#include <sys/types.h>
#include <stddef.h>
#include <stdlib.h>             // malloc etc
#include <stdio.h>
#include <string.h>

#include <stdint.h>

#define STE_SAT_LOG_INFO       if(0)printf
#define STE_SAT_LOG_WARNING    if(1)printf
#define STE_SAT_LOG_ERROR      if(1)printf


#define STE_SAT_MEM_ALLOCATE(Size)                      STE_SAT_Heap_Alloc( (unsigned int)Size )
#define STE_SAT_MEM_FREE(pMem)                          STE_SAT_Heap_Free( (uint8_t*)pMem )
#define STE_SAT_MEM_COPY                                memcpy


uint8_t                *STE_SAT_Heap_Alloc(unsigned int Size);
void                    STE_SAT_Heap_Free(uint8_t * pMem);

#endif
