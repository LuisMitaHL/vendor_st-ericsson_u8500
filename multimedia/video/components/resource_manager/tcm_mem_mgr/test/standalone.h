/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


/*************************************
 * type definitions
 *************************************/
#ifndef __MEMMGR_TYPES_H__
#define __MEMMGR_TYPES_H__

#ifndef __T1XHV_NMF_ENV
#include <stdio.h>
#include <stdlib.h>
#define METH(x) (x)
void NMF_PANIC(char * s);
typedef unsigned int      t_uint16;
typedef signed int        t_sint16;
typedef unsigned long     t_uint32;
typedef signed long       t_sint32;


/**
 * Name:         t_addr MemMgr_Malloc(t_uint32 size)       
 * Author:       Maurizio Colombo
 * Description:  malloc function, behaving like the stdlib one
 *               size is expressed in amount of MMDSP 16-bits words
 *               allocated memory is aligned to MM_ALGN 16-bits words
 *               returns 0 if no more memory available
 **/
void *   MemMgr_Malloc(t_uint32 size);

/**
 * Name:         void MemMgr_Free(t_addr addr)       
 * Author:       Maurizio Colombo
 * Description:  free() function, behaving like the stdlib one
 *               the chunk is freed only if the provided address is exactly the same
 *               as the one that was given by malloc()
 *               If the address is not found in the list, nothing happens
 **/
void     MemMgr_Free(void * addr);

/**
 * Name:         void MemMgr_Init(t_addr start_addr, t_uint32 heap_size)      
 * Author:       Maurizio Colombo
 * Description:  initialize the heap, to be called when instantiating the module
 *               heap_start_addr should be aligned to MM_ALGN (the code will force the alignment anyway)
 *               heap_size is expressed in MMDSP 16-bits words 
 **/
void     MemMgr_Init(void * start_addr, t_uint32 heap_size);

/**
 * Name:         t_sint16 MemMgr_GetStatus(t_uint32 * free_mem, t_uint32 * used_mem)     
 * Author:       Maurizio Colombo
 * Description:  gives the amount of free vs allocated memory
 *               if code is compiled with _DEBUG_MEMMGR flag, this function will also
 *               do a sanity check on the heap and on the internal lists
 **/
t_sint16 MemMgr_GetStatus(t_uint32 * free_mem, t_uint32 * used_mem);

#endif

#endif /* __MEMMGR_TYPES_H__ */
