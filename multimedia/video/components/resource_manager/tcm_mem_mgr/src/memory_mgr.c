/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


/**
 * Name:         memory_mgr.c
 * Author:       Maurizio Colombo
 * Date:         23/05/10 12:16
 * Description:  Public functions of TCM memory manager
 *               This module implements a basic malloc/free 
 *               library for a TCM memory pool
**/

#ifdef __T1XHV_NMF_ENV
#include <resource_manager/tcm_mem_mgr.nmf>
#else
#include "standalone.h"
#endif
#include "memory_mgr.h"
#include "linked_lists.h"
 
t_uint32 g_total_heap_size;
 
#ifdef __T1XHV_NMF_ENV

t_uint32 g_tcm_heap[TCM_HEAP_SIZE/2];

#pragma force_dcumode
t_nmf_error METH(construct)(void)
{
  MemMgr_Init((void*)g_tcm_heap,TCM_HEAP_SIZE);
	return NMF_OK;
} /* end of construct() function */

#pragma force_dcumode
void METH(destroy)(void)
{
} /* end of destroy() function */
#endif /* __T1XHV_NMF_ENV */
 
/**
 * Name:         t_addr MemMgr_Malloc(t_uint32 size)       
 * Author:       Maurizio Colombo
 * Description:  malloc function, behaving like the stdlib one
 *               size is expressed in amount of MMDSP 16-bits words
 *               allocated memory is aligned to MM_ALGN 16-bits words
 *               returns 0 if no more memory available
 **/
#pragma force_dcumode
void * METH(MemMgr_Malloc)(t_uint32 size)
{
        t_sint16 ix;
        t_sint16 aux=-1;
        t_uint32 found_size;

        /* ensure alignment of requested size */ 
        if(size & (MM_ALGN-1))
           size += MM_ALGN - (size & (MM_ALGN-1));

        /* look for smallest fit in free list */
        ix = MemMgr_FindSmallestFit(size);
        if(ix==-1) return 0; /* no more memory available ! */
        
        found_size = g_list[ix].end_addr - g_list[ix].start_addr +1;

        /* if the size of the chunk found is bigger than requested size, the chunk
           must be split in two, part transferred to alloc list, part injected back
           into free list */
        if(found_size>size)
           aux = MemMgr_GetUnusedElm();

        /* move from FREE to ALLOC list */ 
        lst_rm_elm(ID_LST_FREE,ix);
        lst_add_elm(ID_LST_ALLOC,ix);

        /* aux can be -1 if found size = requested size 
           or if no more slots are available from unused list */
        if(aux!=-1) 
        {
          /* split the chunk in two, part alloc part free */
          g_list[aux].end_addr = g_list[ix].end_addr;
          g_list[ix].end_addr = g_list[ix].start_addr + size -1;
          g_list[aux].start_addr = g_list[ix].start_addr + size;
          lst_add_elm(ID_LST_FREE,aux);
        }

#ifdef _DEBUG_MEMMGR
        printf("\nAlloc %x (size %d)\n",g_list[ix].start_addr,size);
#endif        
                
        __CHECK_SANITY();
        return (void*)g_list[ix].start_addr;
}

/**
 * Name:         void MemMgr_Free(t_addr addr)       
 * Author:       Maurizio Colombo
 * Description:  free() function, behaving like the stdlib one
 *               the chunk is freed only if the provided address is exactly the same
 *               as the one that was given by malloc()
 *               If the address is not found in the list, nothing happens
 **/
#pragma force_dcumode
void METH(MemMgr_Free)(void * addr)
{
     t_sint16 ix;
     t_sint16 bot_idx=-1;
     t_sint16 top_idx=-1;
     t_sint16 tmp_ix = gp_start[ID_LST_FREE];                                  
     t_uint32 top_addr; 
     
#ifdef _DEBUG_MEMMGR
     printf("\nFree %x\n",addr);
#endif
     
     /* look for addr in the alloc list */
     ix = MemMgr_FindItem((t_uint32)addr); 
     if(ix==-1)return;  /* if address not found, exit and do nothing */
     
     /* move from alloc to free list */
     lst_rm_elm(ID_LST_ALLOC,ix);
     lst_add_elm(ID_LST_FREE,ix);
          
     /* check if free chunk can be merged with other chunks above and below */
     top_addr = g_list[ix].end_addr;
     tmp_ix = gp_start[ID_LST_FREE];
     do{                   
         if(g_list[tmp_ix].end_addr == ((t_uint32)addr -1))
            bot_idx = tmp_ix;                              
         if(g_list[tmp_ix].start_addr == (top_addr+1))
            top_idx = tmp_ix;                              
         tmp_ix= g_list[tmp_ix].next;                                        
      }while(tmp_ix!=-1);     
      
      /* merge with a chunk below */
      if(bot_idx!=-1)
      {
        lst_rm_elm(ID_LST_FREE,bot_idx);
        g_list[ix].start_addr = g_list[bot_idx].start_addr;
        lst_add_elm(ID_LST_UNUSED,bot_idx);
      }
      /* merge with a chunk above */
      if(top_idx!=-1)
      {
        lst_rm_elm(ID_LST_FREE,top_idx);
        g_list[ix].end_addr = g_list[top_idx].end_addr;
        lst_add_elm(ID_LST_UNUSED,top_idx);
      }     
     
     __CHECK_SANITY();
}
 
/**
 * Name:         void MemMgr_Init(t_addr start_addr, t_uint32 heap_size)      
 * Author:       Maurizio Colombo
 * Description:  initialize the heap, to be called when instantiating the module
 *               heap_start_addr should be aligned to MM_ALGN (the code will force the alignment anyway)
 *               heap_size is expressed in MMDSP 16-bits words 
 **/
void MemMgr_Init(void * heap_start_addr, t_uint32 heap_size)
{
      t_uint16 elm_ix;
      t_uint32 loc_heap_start_addr = (t_uint32)heap_start_addr;

      MemMgr_ListsReset();

      /* ensure alignment of start address and heap size */ 
      if(loc_heap_start_addr & (MM_ALGN-1))
      {
        loc_heap_start_addr += (MM_ALGN - (loc_heap_start_addr & (MM_ALGN-1)));
        heap_size -= (MM_ALGN - (loc_heap_start_addr & (MM_ALGN-1)));  
      }
      heap_size -= (heap_size & (MM_ALGN-1));                   
      
      g_total_heap_size = heap_size;
      elm_ix = gp_start[ID_LST_UNUSED];
      lst_rm_elm(ID_LST_UNUSED,elm_ix);
      lst_add_elm(ID_LST_FREE,elm_ix);
      g_list[elm_ix].start_addr = (t_uint32)loc_heap_start_addr;
      g_list[elm_ix].end_addr = (t_uint32)(loc_heap_start_addr+heap_size-1);
      
      __CHECK_SANITY();
}

/**
 * Name:         t_sint16 MemMgr_GetStatus(t_uint32 * free_mem, t_uint32 * used_mem)     
 * Author:       Maurizio Colombo
 * Description:  gives the amount of free vs allocated memory
 *               if code is compiled with _DEBUG_MEMMGR flag, this function will also
 *               do a sanity check on the heap and on the internal lists
 **/
#pragma force_dcumode
t_sint16 METH(MemMgr_GetStatus)(t_uint32 * free_mem, t_uint32 * used_mem)
{ 
    t_sint16 err=0;
    *free_mem = MemMgr_lst_count_mem(ID_LST_FREE);
    *used_mem = MemMgr_lst_count_mem(ID_LST_ALLOC);
#ifdef _DEBUG_MEMMGR
    printf("Get Status: free = %d, used = %d\n",*free_mem,*used_mem);
    err = MemMgr_CheckSanity(*free_mem,*used_mem);
#endif    
    return err;
}


/**
 * Name:         void MemMgr_DebugScratch(void)     
 * Author:       Maurizio Colombo
 * Description:  This is an helper function, only used for debug. 
 *               When called, it smashes the whole heap with 0xdead 
 *               Purpose is to verify the "scratch" property of the used memory.
 *               Idea is to call it after having freed everything and before doing
 *               a new alloc. This will simulate the fact that another component has
 *               taken the memory and used it.     
 **/ 
#pragma force_dcumode
void METH(MemMgr_DebugScratch)(void)
{
   t_uint16 i;
   for(i=0;i<g_total_heap_size;i++)
     *(t_uint16 *)(g_list[gp_start[ID_LST_FREE]].start_addr + i)=0xdeadU;
}
