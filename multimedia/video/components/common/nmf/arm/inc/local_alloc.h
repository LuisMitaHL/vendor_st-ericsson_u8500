/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _LOCAL_ALLOC_H
#define _LOCAL_ALLOC_H

#ifdef __cplusplus
extern "C" {
#endif


#ifdef NO_VFM
#include <malloc.h>
#define mallocCM(_size,_align)  malloc(_size)
#define mallocCachedCM(_size,_align)  malloc(_size)
#define mallocCM_MPC(_size,_align)  malloc(_size)
#define VFM_GetPhysical(_context,a)	a
#define VFM_GetLogical(_context,a)	a
#define	VFM_CloseMemoryList(a)
#define VFM_CreateMemoryList(a)		0
#else
#define VFM_MEMORY_ALLOW_CMALLOC
#include "VFM_Memory.h"

#define malloc(_size)               VFM_Alloc(vfm_memory_ctxt, (_size), VFM_MEM_NEW, 0, 0, __LINE__, (t_uint8 *)__FILE__)
#define mallocCM(_size,_align)   	VFM_Alloc(vfm_memory_ctxt, (_size), VFM_MEM_HWBUFFER_NONDSP, _align, 0, __LINE__, (t_uint8 *)__FILE__)
#define mallocCachedCM(_size,_align)   	VFM_Alloc(vfm_memory_ctxt, (_size), VFM_MEM_CACHED_HWBUFFER_NONDSP, _align, 0, __LINE__, (t_uint8 *)__FILE__)
#define mallocCM_MPC(_size, _align)	VFM_Alloc(vfm_memory_ctxt, (_size), VFM_MEM_CM_MPC_ESRAM16_ALLOC, _align, 0, __LINE__, (t_uint8 *)__FILE__)
#ifdef MALLOC_DEBUG
#define free(_pt)       				printf("Free at %s %d\n",__FILE__,__LINE__);VFM_Free(vfm_memory_ctxt, (t_uint8 *)(_pt))
#else
#define free(_pt)       				{if(_pt) {VFM_Free(vfm_memory_ctxt, (t_uint8 *)(_pt));}}
#endif
#endif



#define realloc(_size)  ERROR
#define calloc(_size)   ERROR



	

#ifdef __cplusplus
}
#endif

#endif
