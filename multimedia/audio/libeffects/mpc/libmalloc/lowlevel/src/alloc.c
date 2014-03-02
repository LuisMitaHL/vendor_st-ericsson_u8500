/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

#include "malloc.h"
#include <libeffects/mpc/libmalloc.nmf>

Mem_Struct * heap_ptr_tab[NB_MEMORY_SPACE];

unsigned int mem_Size_Left[NB_MEMORY_SPACE];
unsigned int mem_Size_Init[NB_MEMORY_SPACE];

#define __double_accesses__

static Mem_Struct *Mem_Find(Mem_Struct * begin_addr, size_t size, Mem_Bloc Bloc)
{
  Mem_Struct *tmp_ptr = begin_addr;

  while((tmp_ptr->status & MEM_BLOC_MASK) == Bloc && tmp_ptr != NULL)
  {
    if (tmp_ptr->custom != CUSTOM_CHECKSUM)
    {
      NmfPrint1(0, "MMDSP heap integrity check failed [0x%p]\n", tmp_ptr);
      return NULL;
    }

    if((tmp_ptr->status & MEM_STATUS_MASK) == MEM_FREE)
    {
      if (size <= tmp_ptr->size * sizeof(int))
        break;
    }
    tmp_ptr = tmp_ptr->next;
  }

  if((tmp_ptr->status & MEM_BLOC_MASK) != Bloc)
     tmp_ptr = NULL;

  return(tmp_ptr);
}

#ifdef Y_ALLOC
static Mem_Struct *Y_Find(size_t size)
{
  if (last_Y_ptr != NULL) //If Y mem exists in the target. (e.g. it is not the case for STn8815/video)
  {
    Mem_Struct *tmp_ptr = last_Y_ptr;

    while((tmp_ptr->status & MEM_BLOC_MASK) == Y_MEM)
    {
      if((tmp_ptr->status & MEM_STATUS_MASK) == MEM_FREE)
      {
        if (size <= tmp_ptr->size * sizeof(int))
          break;
      }
      tmp_ptr = tmp_ptr->prev;
    }

    if((tmp_ptr->status & MEM_BLOC_MASK) == Y_MEM)
      return tmp_ptr;
    else //X_MEM
      return NULL;
  }
  else
    return NULL;
}
#endif // Y_ALLOC


#if defined(EXTMEM_ALLOC) || defined(ESRAM_ALLOC)
static Mem_Struct * Ext16_Find(Mem_Struct * begin_addr, size_t size)
{
  Mem_Struct *tmp_ptr2;
  Mem_Struct *tmp_ptr = begin_addr;
  size_t size_in_24b_mode;

  while(tmp_ptr != NULL)
    {
      if((tmp_ptr->status & MEM_STATUS_MASK) == MEM_FREE)
        {
#ifndef __mode16__ //in 24 bits DCU mode
          /* read a size_t data in 24 bits mode from 16 bits external memory.
           * THE DIRECT AFFECTATION IS INCORRECT,
           * espacially when the given size is greater than 2^16 !!!!
           */
          size_in_24b_mode = *(unsigned int *)&tmp_ptr->size & 0xFFFFU;
          size_in_24b_mode = (size_in_24b_mode << 16) | (*((unsigned int *)&tmp_ptr->size + 1) & 0xFFFFU);
          if (size <= size_in_24b_mode * sizeof(int))
#else //in 16 bits DCU mode
            if (size <= tmp_ptr->size * sizeof(int))
#endif //__mode16__
              break;
        }
      tmp_ptr2 = tmp_ptr->prev;
      //!! do "tmp_ptr->next & 0xFFFFU", this is because the sign extension when 16 bits -> 24 bits
      tmp_ptr = (Mem_Struct *)(((unsigned long)tmp_ptr2 << 16) | ((unsigned long)tmp_ptr->next & 0xFFFFU));
    }
  return tmp_ptr;
}
#endif //defined(EXTMEM_ALLOC) || defined(ESRAM_ALLOC)


void *Mem_Alloc_Ptr(unsigned int custom, size_t size, int bloc_stack)
{
    size_t  tot_size;
    Mem_Struct *tmp_ptr;
    Mem_Struct *tmp2_ptr;
    Mem_Struct *tmp3_ptr;
    int *init_ptr;       // To reset (set to 0) all the allocated memory.
    size_t init_size, size_in_24b_mode, i;
    Mem_Bloc bloc;
    int * ret_ptr;
    size_t remain_size;

    bloc = bloc_stack & MEM_BLOC_MASK;

#ifdef __double_accesses__
    if ((size & 1) == 1) //alignement
        tot_size = (size * sizeof(int)) + sizeof(Mem_Struct) + 1;
    else
#endif
        tot_size = (size * sizeof(int)) + sizeof(Mem_Struct);

    switch(bloc){
#ifdef Y_ALLOC
    case Y_MEM:
        tmp_ptr = Y_Find(tot_size);
        break;
#endif //Y_ALLOC
#ifdef EXTMEM_ALLOC
    case EXT_MEM24:
        tmp_ptr = Mem_Find(heap_Ext24_ptr, tot_size, bloc);
        break;
    case EXT_MEM16:
        tmp_ptr = Ext16_Find(heap_Ext16_ptr, tot_size);
        break;
#endif //EXTMEM_ALLOC
#ifdef ESRAM_ALLOC
    case ESRAM_24:
        tmp_ptr = Mem_Find(heap_Esram24_ptr, tot_size, bloc);
        break;
    case ESRAM_16:
        tmp_ptr = Ext16_Find(heap_Esram16_ptr, tot_size);
        break;
#endif //ESRAM_ALLOC
    case X_MEM:
        tmp_ptr = Mem_Find(heap_ptr, tot_size, bloc);
        break;
    default:
      return NULL;
    }

    if (tmp_ptr == NULL)
        return (NULL);

    tmp_ptr->status = (MEM_USED | bloc_stack);
    tmp_ptr->custom = custom;

#ifndef __mode16__ //in 24 bits DCU mode
    size_in_24b_mode = 0;
    if (bloc == EXT_MEM16 || bloc == ESRAM_16)
    {
        /* read a size_t data in 24 bits mode from 16 bits external memory.
         * THE DIRECT AFFECTATION IS INCORRECT,
         * espacially when the given size is greater than 2^16 !!!!
         */
        size_in_24b_mode = *((unsigned int *)&tmp_ptr->size) & 0xFFFFU;
        size_in_24b_mode = (size_in_24b_mode << 16) | (*(((unsigned int *)&tmp_ptr->size) + 1) & 0xFFFFU);
    }
    else
        size_in_24b_mode = tmp_ptr->size;

    if((size_in_24b_mode * sizeof(int)) != tot_size) //tmp_ptr->size > tot_size
    {
        remain_size = size_in_24b_mode * sizeof(int)- tot_size;
#else //in 16 bits DCU mode
        if((tmp_ptr->size * sizeof(int)) != tot_size)
        {
            remain_size = (tmp_ptr->size * sizeof(int))- tot_size;
#endif //__mode16__
            if(remain_size >= sizeof(Mem_Struct))
            {
                switch(bloc){
                case X_MEM:
                case EXT_MEM24:
                case ESRAM_24:
                    tmp2_ptr = tmp_ptr->next;
                    tmp_ptr->next = (Mem_Struct*)((int*)tmp_ptr + tot_size);
                    tmp_ptr->next->prev = tmp_ptr;
                    tmp_ptr->next->next = tmp2_ptr;
                    tmp_ptr->next->custom = CUSTOM_CHECKSUM;

                    if (tmp2_ptr != NULL)
                    {
                      tmp2_ptr->prev = tmp_ptr->next;
                    }
                    tmp_ptr->next->status = (MEM_FREE | ( bloc & MEM_BLOC_MASK) );
                    tmp_ptr->next->size = remain_size;
                    tmp_ptr->size = tot_size;
                    break;

                case EXT_MEM16:
                case ESRAM_16:
                    /*EXT_MEM16 maintains a single chained list.
                     * "prev" is used as the higher part (extension) of pointer "next".
                     * i.e. [prev(16bits) next(16bits)] => next(32bits) only 24 low bits are used.
                     */
                  //like : tmp2_ptr = tmp_ptr->next;
                  tmp2_ptr = tmp_ptr->prev;
                  tmp2_ptr = (Mem_Struct *)(((unsigned long)tmp2_ptr << 16) | ((unsigned long)tmp_ptr->next & 0xFFFFU ));
                  tmp3_ptr = (Mem_Struct*)((int*)tmp_ptr + tot_size);

                  //like : tmp_ptr->next = tmp3_ptr;
                  tmp_ptr->prev = (Mem_Struct*)((unsigned long)tmp3_ptr >> 16);
                  tmp_ptr->next = (Mem_Struct*)((unsigned long)tmp3_ptr & 0xFFFFU);

                  //like : tmp3_ptr->next = tmp2_ptr;
                  tmp3_ptr->prev = (Mem_Struct*)((unsigned long)tmp2_ptr >> 16);
                  tmp3_ptr->next = (Mem_Struct*)((unsigned long)tmp2_ptr & 0xFFFFU);
                  tmp3_ptr->status = ( MEM_FREE | ( bloc & MEM_BLOC_MASK) );
                  tmp3_ptr->custom = CUSTOM_CHECKSUM;

#ifndef __mode16__ //in 24bits DCU mode
                  /*stock a size_t data in 24 bits mode to 16 bits external memory.
                   * THE DIRECT AFFECTATION IS INCORRECT,
                   * espacially when the given size is greater than 2^16 !!!!
                   */
                  *((unsigned int *)&tmp3_ptr->size) = remain_size >> 16;
                  *(((unsigned int *)&tmp3_ptr->size) + 1) = remain_size & 0xFFFFU;
                  *((unsigned int *)&tmp_ptr->size) = tot_size >> 16;
                  *(((unsigned int *)&tmp_ptr->size) + 1) = tot_size & 0xFFFFU;
#else //in 16 bits DCU mode
                  tmp3_ptr->size = remain_size;
                  tmp_ptr->size = tot_size;
#endif //__mode16__
                  break;
#ifdef Y_ALLOC
                case Y_MEM:
                  tmp2_ptr = tmp_ptr->next;
                  tmp_ptr->next = (Mem_Struct*)((int*)tmp_ptr + remain_size);
                  tmp_ptr->next->prev = tmp_ptr;
                  tmp_ptr->next->next = tmp2_ptr;

                  tmp_ptr->next->custom = tmp_ptr->custom;
                  tmp_ptr->custom = CUSTOM_CHECKSUM;
                  tmp_ptr->next->status = (MEM_USED | (tmp_ptr->status & (MEM_BLOC_MASK|MEM_STACK_MASK)));
                  tmp_ptr->status = (MEM_FREE | (tmp_ptr->status & (MEM_BLOC_MASK|MEM_STACK_MASK)));
                  tmp_ptr->next->size = tot_size;
                  tmp_ptr->size = remain_size;

                  if (tmp2_ptr != NULL)
                  {
                    tmp2_ptr->prev = tmp_ptr->next;
                  }
                  tmp_ptr = tmp_ptr->next;
                  if( tmp_ptr > last_Y_ptr )
                    last_Y_ptr = tmp_ptr;
                  break;
#endif//Y_ALLOC
                default:
                  return NULL;
                }
            }
            // else we return the whole bloc, even if bigger than requested.
        }

#ifndef __mode16__ //in 24 bits DCU mode
        /* read a size_t data in 24 bits mode from 16 bits external memory.
	 * THE DIRECT AFFECTATION IS INCORRECT,
	 * espacially when the given size is greater than 2^16 !!!!
	 */
        size_in_24b_mode = 0;
        if (bloc == EXT_MEM16 || bloc == ESRAM_16)
        {
            size_in_24b_mode = *((unsigned int *)&tmp_ptr->size) & 0xFFFFU;
            size_in_24b_mode = (size_in_24b_mode << 16) | (*(((unsigned int *)&tmp_ptr->size) + 1) & 0xFFFFU);
            mem_Size_Left[(bloc>>4)] -= size_in_24b_mode;
        }
        else
#endif //__mode16__
        {
          mem_Size_Left[(bloc>>4)] -= tmp_ptr->size;
        }

        ret_ptr = (int*)tmp_ptr + sizeof(Mem_Struct);
        // Add code to initialize to 0 all the requested memory area !!!
        // init_ptr = (unsigned int *)((int*)tmp_ptr + sizeof(Mem_Struct) / sizeof(int));
        init_ptr = ret_ptr;
#ifndef __mode16__ //in 24 bits DCU mode
        if (bloc == EXT_MEM16 || bloc == ESRAM_16)
        {
            init_size = size_in_24b_mode - sizeof(Mem_Struct);
        }
        else
#endif // __mode16__
        {
            init_size = tmp_ptr->size - sizeof(Mem_Struct); //Don't use "tmp_ptr->next - init_ptr" because there could be a "_cst_data2" segment between the last X bloc and the first Y bloc.
        }

        for (i = 0; i < init_size; i++)
        {
            *init_ptr++ = 0;
        }

     return (void *)ret_ptr;
 }


/*
 * Set available X memories area for malloc use.
 * @param pid: the current thread's Id. When in the stand alone mode, the pid is 0.
 */
void Init_X(void * available_mem_addr, size_t init_size)
{
  heap_ptr = (Mem_Struct*)available_mem_addr; //alignement of heap_start is checked at startup.

  heap_ptr->size = init_size;
  heap_ptr->custom = CUSTOM_CHECKSUM;
  heap_ptr->prev = NULL;
  heap_ptr->next = NULL;
  heap_ptr->status = (MEM_FREE | X_MEM | DATA_MEM);

  mem_Size_Left[MEM_XTCM] = heap_ptr->size;
  mem_Size_Init[MEM_XTCM] = init_size;
}


#ifdef Y_ALLOC
  /*
   * Set available Y memories area for malloc use.
   * @param pid: the current thread's Id. When in the stand alone mode, the pid is 0.
   */
void Init_Y(void * available_mem_addr, size_t init_size)
{
    Mem_Struct * tmp_ptr;

    last_Y_ptr = (Mem_Struct*)available_mem_addr; //alignement of heap_start is checked at startup.

    last_Y_ptr->size = init_size;
    last_Y_ptr->custom = CUSTOM_CHECKSUM;
    //find the last bloc in X memory
    tmp_ptr = heap_ptr;

    while (tmp_ptr->next != NULL) {
      tmp_ptr = tmp_ptr->next;
    }

    tmp_ptr->next = last_Y_ptr;
    last_Y_ptr->prev = tmp_ptr;
    last_Y_ptr->next = NULL;
    last_Y_ptr->status = (MEM_FREE | Y_MEM | DATA_MEM);
    mem_Size_Left[MEM_YTCM] = last_Y_ptr->size;
    mem_Size_Init[MEM_YTCM] = init_size;
}

#endif //Y_ALLOC


#if defined(EXTMEM_ALLOC) || defined(ESRAM_ALLOC)
  // used for init extmem24 and esram24
void Init_ext_mem_24(Mem_Struct ** mem_block_ptr, void * available_mem_addr, size_t init_size, Mem_Bloc bloc, unsigned int custom)
{
    if (init_size > sizeof(Mem_Struct)) //if the memory bloc exists and has enough size.
    {
#ifdef __double_accesses__
        /*
         * the block should start at an even address
         */
        if (((unsigned long)available_mem_addr & 1) == 1)
        {
            *mem_block_ptr = (Mem_Struct*)((unsigned long)available_mem_addr + 1);
            (*mem_block_ptr)->size = init_size - 1;
        }
        else
#endif //__double_accesses__
        {
            *mem_block_ptr = (Mem_Struct*)(available_mem_addr);
            (*mem_block_ptr)->size = init_size;
        }
        (*mem_block_ptr)->custom = custom;
        (*mem_block_ptr)->prev = NULL;
        (*mem_block_ptr)->next = NULL;
        (*mem_block_ptr)->status = (MEM_FREE | bloc | DATA_MEM);

        mem_Size_Left[(bloc>>4)] = (*mem_block_ptr)->size;
        mem_Size_Init[(bloc>>4)] = init_size;
      }
}
#endif //defined(EXTMEM_ALLOC) || defined(ESRAM_ALLOC)

#if defined(EXTMEM_ALLOC) || defined(ESRAM_ALLOC)
  //use for init extmem16 and esram16
void Init_ext_mem_16(Mem_Struct ** mem_block_ptr, void * available_mem_addr, size_t init_size, Mem_Bloc bloc, unsigned int custom)
{
    if (init_size > sizeof(Mem_Struct)) //if the memory bloc exists and has enough size.
    {
        size_t actual_init_size;
#ifdef __double_accesses__
        /*
         * the block should start at an even address
         */
        if (((unsigned long)available_mem_addr & 1) == 1)
        {
            *mem_block_ptr = (Mem_Struct*)((unsigned long)available_mem_addr + 1);
            actual_init_size = init_size -1;
        }
        else
#endif //__double_accesses__
        {
            *mem_block_ptr = (Mem_Struct*)(available_mem_addr);
            actual_init_size = init_size;
        }
#ifndef __mode16__ //in 24 bits DCU mode
        //stock a size_t data in 24 bits mode to 16 bits external memory.
        //THE DIRECT AFFECTATION IS INCORRECT, espacially when the given size is greater than 2^16 !!!!
        *((unsigned int *)&(*mem_block_ptr)->size) = actual_init_size >> 16;
        *((unsigned int *)&(*mem_block_ptr)->size + 1) = actual_init_size & 0xFFFFU;
#else  //16 bits DCU mode
        (*mem_block_ptr)->size = actual_init_size;
#endif //__mode16__
        (*mem_block_ptr)->custom = custom;
        (*mem_block_ptr)->prev = NULL; //the prev is used for the next pointer, i.e. [prev(16bits) next(16bits)] => next(32bits)
        (*mem_block_ptr)->next = NULL;
        (*mem_block_ptr)->status = (MEM_FREE | bloc | DATA_MEM);

        mem_Size_Left[(bloc>>4)] = (*mem_block_ptr)->size;
        mem_Size_Init[(bloc>>4)] = actual_init_size;
      }
}
#endif //defined(EXTMEM_ALLOC) || defined(ESRAM_ALLOC)
