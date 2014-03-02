/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


 
#ifndef __LINKED_LISTS_H__ 
#define __LINKED_LISTS_H__ 

#ifdef _DEBUG_MEMMGR
#define __CHECK_SANITY()                                                          \
{                                                                                 \
  t_uint32 free_mem, used_mem;                                                    \
  if(MemMgr_GetStatus(&free_mem,&used_mem))                                       \
  {                                                                               \
     NMF_PANIC("*** MEMMGR : Error while checking sanity ****\n");                \
  }                                                                               \
}
#else
#define __CHECK_SANITY()
#endif

typedef struct
{
  t_uint32       start_addr;              /* memory descriptor start address */
  t_uint32       end_addr;                /* memory descriptor end address   */
  t_sint16         next;                    /* index of next element in llist  */
  t_sint16         prev;                    /* index of prev element in llist  */
} t_llist_elm;

typedef enum
{
  ID_LST_FREE,
  ID_LST_ALLOC,
  ID_LST_UNUSED
} t_lst_id;

extern EXTMEM t_sint16     gp_start[3];     
extern EXTMEM t_llist_elm  g_list[LST_MAX_ELM_NBR];


/* -1 indicates first/last of a linked list */
#define LLST_IS_FIRST(a)  (g_list[a].prev==-1)
#define LLST_IS_LAST(a)   (g_list[a].next==-1)

/**
 * add an element to the specified linked list
 * always add it at the beginning of the list
 **/
#define lst_add_elm(lst_id, elm_idx)                             \
{                                                                \
  g_list[elm_idx].next = gp_start[lst_id];                       \
  g_list[elm_idx].prev = -1;                                     \
  if(!LLST_IS_LAST(elm_idx))  g_list[g_list[elm_idx].next].prev = elm_idx;    \
  gp_start[lst_id] = elm_idx;                                    \
}

/**
 * remove an element from the specified linked list
 *  to be done before adding to another list 
 **/
#define lst_rm_elm(lst_id, elm_idx)                              \
{                                                                \
  if(LLST_IS_FIRST(elm_idx))                                     \
  {                                                              \
    gp_start[lst_id] = g_list[elm_idx].next;                     \
  }                                                              \
  else                                                           \
  {                                                              \
    g_list[g_list[elm_idx].prev].next = g_list[elm_idx].next;    \
  }                                                              \
  if(!LLST_IS_LAST(elm_idx))                                     \
  {                                                              \
    g_list[g_list[elm_idx].next].prev = g_list[elm_idx].prev;    \
  }                                                              \
}

/* internal functions */
t_uint16 MemMgr_lst_count_elms(t_lst_id lst_id);
void     MemMgr_ListsReset(void);
t_sint16 MemMgr_ListsCheck(void);
t_sint16 MemMgr_FindItem(t_uint32 addr);
t_sint16 MemMgr_FindSmallestFit(t_uint32 size);
void     MemMgr_PrintList(char *s, t_lst_id lst_id);
t_sint16 MemMgr_CheckSanity(t_uint32 free_mem, t_uint32 used_mem);
t_sint16 MemMgr_GetUnusedElm(void);
t_uint32 MemMgr_lst_count_mem(t_lst_id lst_id);
void     MemMgr_Init(void * start_addr, t_uint32 heap_size);



#endif /* __LINKED_LISTS_H__ */
