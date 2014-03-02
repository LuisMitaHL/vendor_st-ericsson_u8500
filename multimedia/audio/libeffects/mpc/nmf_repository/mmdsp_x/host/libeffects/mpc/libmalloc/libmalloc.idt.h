/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*****************************************************************************/
/**
 * This is a Nomadik Multiprocessing Framework generated file, please do not modify it.
 */

/* Generated libeffects/mpc/libmalloc/libmalloc.idt defined type */
#if !defined(__LIBMALLOC_H_)
#define __LIBMALLOC_H_

#include <host/libeffects/mpc/include/memorybank.idt.h>

typedef enum t_xyuv_Mem_Error {
  MEM_SUCCESS=0,
  MEM_INVALID_ADDRESS=-1,
  MEM_DISABLED_ALLOC_TYPE=-2,
  MEM_INTEGRITY_FAILED=-3} Mem_Error;

typedef enum t_xyuv_e_Sia_Heap_Debug {
  SIA_HEAP_DEBUG_LEVEL_ERR,
  SIA_HEAP_DEBUG_LEVEL_INFO,
  SIA_HEAP_DEBUG_LEVEL_DEBUG} e_Sia_Heap_Debug;

typedef struct t_xyuv_t_ExtendedAllocParams {
  t_memory_bank bank;
  void* trace_p;
} t_ExtendedAllocParams;

#endif
