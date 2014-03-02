/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


/**
 * Name:         linked_lists.c
 * Author:       Maurizio Colombo
 * Date:         23/05/10 12:16
 * Description:  internal functions to handle linked lists
**/

#ifdef __T1XHV_NMF_ENV
#include <resource_manager/tcm_mem_mgr.nmf>
#else
#include "standalone.h"
#endif
#include "memory_mgr.h"
#include "linked_lists.h"
 
/**
 * start indexes of the three linked lists 
 *    "free" is the linked list of free memory descriptors
 *    "alloc" is the linked list of allocated memory descriptors
 *    "unused" is the linked list of unused descriptors
 **/
EXTMEM t_sint16     gp_start[3];     
EXTMEM t_llist_elm  g_list[LST_MAX_ELM_NBR];
extern EXTMEM t_uint32 g_total_heap_size;

/**
 * Name:         void MemMgr_ListsReset(void)      
 * Author:       Maurizio Colombo
 * Description:  Initialize lists
 *               after execution, free & alloc lists are empty
 *               unused list contains all elements of table
 **/
void MemMgr_ListsReset(void)
{
    t_sint16 i;
    gp_start[ID_LST_FREE]  = -1;
    gp_start[ID_LST_ALLOC] = -1;
    gp_start[ID_LST_UNUSED] = -1;
    for(i=0;i<LST_MAX_ELM_NBR;i++)
       lst_add_elm(ID_LST_UNUSED,i);          
}

/**
 * Name:         t_sint16 MemMgr_FindItem(t_addr addr)     
 * Author:       Maurizio Colombo
 * Description:  Look for the specified address in the Alloc list
 *               return -1 if not found
 **/
t_sint16 MemMgr_FindItem(t_uint32 addr)
{
  t_sint16 res=-1;
  t_sint16 ix = gp_start[ID_LST_ALLOC];
  if(ix==-1)return -1;                                  
  do{                   
    if(g_list[ix].start_addr == addr)
      res=ix;
    ix= g_list[ix].next;                                        
  }  while(ix!=-1);     
  return res;
}

/**
 * Name:         t_sint16 MemMgr_GetUnusedElm(void)     
 * Author:       Maurizio Colombo
 * Description:  Get an index from unused list, remove that index from unused list
 *               return -1 if no more elements available
 **/
t_sint16 MemMgr_GetUnusedElm(void)
{
  t_sint16 ix = gp_start[ID_LST_UNUSED];
  if(ix!=-1)
    lst_rm_elm(ID_LST_UNUSED,ix);
  return ix;
}

/**
 * Name:         t_sint16 MemMgr_FindSmallestFit(t_uint32 size)   
 * Author:       Maurizio Colombo
 * Description:  look for smallest chunk in free list that fits requested size
 *               return -1 if free list is empty or if no chunk has been found 
 *               with sufficient size
 **/
t_sint16 MemMgr_FindSmallestFit(t_uint32 size)
{
  t_sint16 res=-1;
  t_uint32 min=0x3FFFFFFFUL;
  t_uint32 tmp_size;
  t_sint16 ix = gp_start[ID_LST_FREE];
  if(ix==-1)return -1;                                  
  do{                   
    tmp_size = g_list[ix].end_addr - g_list[ix].start_addr + 1;
    if((size <= tmp_size)&&(tmp_size<min))
    {
      min = tmp_size;
      res = ix;
    }                         
    ix= g_list[ix].next;                                        
  }  while(ix!=-1);     
  return res;
}

/**
 * Name:         t_uint32 MemMgr_lst_count_mem(t_lst_id lst_id)    
 * Author:       Maurizio Colombo
 * Description:  returns total amount of memory accumulated on the specified list
 **/
t_uint32 MemMgr_lst_count_mem(t_lst_id lst_id)                                   
{                                                                
  t_uint32 cnt=0;                                                
  t_sint16 ix = gp_start[lst_id];
  if(ix==-1)return 0;                                  
  do{                                                              
    cnt+=(g_list[ix].end_addr - g_list[ix].start_addr + 1);                                                       
    ix= g_list[ix].next;                                         
  }  while(ix!=-1);      
  return cnt;                                                    
}

#ifdef _DEBUG_MEMMGR          
/**
 * Name:         t_sint16 MemMgr_ListsCheck(void)  
 * Author:       Maurizio Colombo
 * Description:  print free/alloc lists contents and checks that the total amount of elements
 *               in the three lists is equal to the elements table size (this means that each 
 *               element of the elements table must belong to one and only one list)
 **/
t_sint16 MemMgr_ListsCheck(void)
{
     MemMgr_PrintList("Free",ID_LST_FREE);
     MemMgr_PrintList("Alloc",ID_LST_ALLOC);
     if((MemMgr_lst_count_elms(ID_LST_FREE)
        +MemMgr_lst_count_elms(ID_LST_ALLOC)
        +MemMgr_lst_count_elms(ID_LST_UNUSED))!=LST_MAX_ELM_NBR)
        return -1;
     return 0;
}

/**
 * Name:         t_uint16 MemMgr_lst_count_elms(t_lst_id lst_id)  
 * Author:       Maurizio Colombo
 * Description:  returns the number of elements belonging to specified list
 **/
t_uint16 MemMgr_lst_count_elms(t_lst_id lst_id)                                   
{                                                                
  t_uint16 cnt=0;                                                
  t_sint16 ix = gp_start[lst_id];
  if(ix==-1)return 0;                                  
  do{                                                              
    cnt++;                                                       
    ix= g_list[ix].next;                                         
  }  while(ix!=-1);      
  return cnt;                                                    
}

/**
 * Name:         void MemMgr_PrintList(char *s, t_lst_id lst_id)  
 * Author:       Maurizio Colombo
 * Description:  print the specified list content
 **/
void MemMgr_PrintList(char *s, t_lst_id lst_id)
{                                        
  t_sint16 ix = gp_start[lst_id];
  if(ix==-1)return;                                  
  printf("List %s : ",s,lst_id);
  do{                                                              
    printf("(%d,%x,%x,[%d]) ",ix,g_list[ix].start_addr,g_list[ix].end_addr,g_list[ix].prev);                                                       
    ix= g_list[ix].next;                                         
  }  while(ix!=-1); 
  printf("\n");       
}

/**
 * Name:         t_sint16 MemMgr_CheckSanity(t_uint32 free_mem, t_uint32 used_mem)
 * Author:       Maurizio Colombo
 * Description:  global sanity check
 *                check on lists 
 *                check that free+used memory is same as total heap size
 **/
t_sint16 MemMgr_CheckSanity(t_uint32 free_mem, t_uint32 used_mem)
{
     if((free_mem+used_mem != g_total_heap_size) || (MemMgr_ListsCheck()))
            return -1;     
     return 0;
}
#endif
