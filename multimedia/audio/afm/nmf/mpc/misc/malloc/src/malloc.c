/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   malloc.c
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include <malloc.nmf> 
#include <dbc.h> 
#include <stdlib.h> 

////////////////////////////////////////////////////////////

void *malloc_core(t_heap_info *heap, t_uint32 size)
{
  void *ptr;
  
  PRECONDITION(heap->Start);
  
  if(size & 0x1){
	size = size+1;
  }
  
  if( heap->SizeAvailable >= size && heap->SizeAllocated + size <= heap->SizeTotal)
  {
	ptr = (void *)((t_sword *)heap->Start + heap->SizeAllocated);
    
    POSTCONDITION((t_sword *)ptr + size <= (t_sword *)heap->Start + heap->SizeTotal);
    
	heap->SizeAllocated += size;
	heap->SizeAvailable -= size;
	
	return (ptr);
  }
  else
  {
	POSTCONDITION(0);
	return NULL;
  }
}

void YMEM *malloc_core_y(t_heap_info *heap, t_uint32 size)
{
    int       address;
    void YMEM *ptr;


    PRECONDITION(heap->Start);

    if(size & 0x1)
    {
        size = size + 1;
    }

    if((heap->SizeAvailable >= size) && (heap->SizeAllocated + size <= heap->SizeTotal))
    {
        address = (int) ((t_sword *) heap->Start + heap->SizeAllocated);
        ptr     = (void YMEM *) address;

        heap->SizeAllocated += size;
        heap->SizeAvailable -= size;

        return ptr;
    }
    else
    {
        POSTCONDITION(0);
        return (void YMEM *) NULL;
    }
}


asm unsigned long mode16tomode24_ul(t_uint32 value) {
    asli @{value}.0, #8,  @{}.0
    zero16 @{value}.1, @{}.1
    L_lsri @{}, #8, @{}
}

// malloc call from 24 bit mode 
void *misc_malloc(t_heap_info *heap, t_uint32 size)
{
    return malloc_core(heap, size);
}

// malloc call from 16 bit mode
#pragma force_dcumode 
void *misc_malloc_mode16(t_heap_info *heap, t_uint32 size)
{
    size = mode16tomode24_ul(size);
    return malloc_core(heap, size);
}


