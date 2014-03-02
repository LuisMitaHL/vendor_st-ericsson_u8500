/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef HOST_ALLOC_H
#define HOST_ALLOC_H

#ifdef NO_VFM

#include <malloc.h>
#define HWBUFF_ALLOC(_size,_align)   	malloc(_size)
#define MPC_ALLOC(_size,_align)   	malloc(_size)
#define MPC_ALLOC_SDRAM(_size,_align)  	malloc(_size)
#define FREE(_pt)       		free(_pt)
#define VFM_GetPhysical(_context,a)	a
#define VFM_GetLogical(_context,a)	a
#define	VFM_CloseMemoryList(a)
#define VFM_CreateMemoryList(a,b)	0

#else

#include "VFM_Memory.h"
#define NEW_ALLOC(_size)		VFM_Alloc(vfm_memory_ctxt, (_size), VFM_MEM_NEW, 0, 0, __LINE__, (t_uint8 *)__FILE__)
#define HWBUFF_ALLOC(_size,_align)   	VFM_Alloc(vfm_memory_ctxt, (_size), VFM_MEM_HWBUFFER_NONDSP, _align, 0, __LINE__, (t_uint8 *)__FILE__)
#define MPC_ALLOC(_size, _align)	VFM_Alloc(vfm_memory_ctxt, (_size), VFM_MEM_CM_MPC_ESRAM16_ALLOC, _align, 0, __LINE__, (t_uint8 *)__FILE__)
#define MPC_ALLOC_SDRAM(_size, _align)	VFM_Alloc(vfm_memory_ctxt, (_size), VFM_MEM_CM_MPC_SDRAM16_ALLOC, _align, 0, __LINE__, (t_uint8 *)__FILE__)
#define FREE(_pt)       		VFM_Free(vfm_memory_ctxt, (t_uint8 *)(_pt))

#endif


#endif
