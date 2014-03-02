/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */



#include <los/api/los_api.h>

#if defined (__ARM_LINUX) || defined (TLM_PLATFORM) || defined(__ARM_SYMBIAN)
#define __weak
#ifndef  DEFINE_WEAK_PROTOTYPES
#define __attribute__(...)
#endif
#else
#define __attribute__(...)
#endif

__weak unsigned int g_mmu_offset=0;

/* -----------------------------------------------------------------------
FUNCTION : MMTE_B2R2_GetMemoryOffset
PURPOSE  : return offset between logical and physical memory
------------------------------------------------------------------------ */
/*__weak MMTE_B2R2_GetMmuOffset()
{
	unsigned int physical_addr;
	unsigned int logical_addr;
	
	t_los_memory_handle mem_handle;

        mem_handle = LOS_Alloc(10,256, LOS_MAPPING_CREATE_UNCACHED_UNBUFFERED);


	logical_addr = LOS_GetLogicalAddress(mem_handle);
	g_mmu_offset=logical_addr-physical_addr;
	LOS_Free(mem_handle);
}*/

/* -----------------------------------------------------------------------
FUNCTION : MMTE_B2R2_Alloc
PURPOSE  : Allocate buffer in DDR memory
------------------------------------------------------------------------ */
/*__weak unsigned int MMTE_B2R2_Alloc(unsigned int sizeInBytes, unsigned int alignmentInBytes,t_uint32* p_handle )
{
	unsigned int addr_return;
	t_los_memory_handle mem_handle;

        mem_handle = LOS_Alloc(sizeInBytes,alignmentInBytes, LOS_MAPPING_CREATE_UNCACHED_UNBUFFERED);
	addr_return = LOS_GetLogicalAddress(mem_handle);
	
	*p_handle = (t_uint32)mem_handle;
	
	return addr_return;
} 
*/
/* -----------------------------------------------------------------------
FUNCTION : MMTE_B2R2_Free
PURPOSE  : de-Allocate buffer in DDR memory
------------------------------------------------------------------------ */
__weak void MMTE_B2R2_Free(unsigned int handle)
{
   /* TODO */
   LOS_Free(handle);
}
