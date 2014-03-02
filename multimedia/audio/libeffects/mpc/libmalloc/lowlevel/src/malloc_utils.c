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
#include "fsm/generic/include/FSM.h"

#ifndef _SIMU_
// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "libeffects_mpc_libmalloc_lowlevel_src_malloc_utilsTraces.h"
#endif
#endif

/*
 * This function updates the "tmp_ptr" with the address
 * of the given memory bank and return 16_BITS_MEM_BANK
 * if "mem" indicates a 16 bits memory bank.
 */
unsigned int * mattach_to_mem_bank(Mem_Bloc mem, Mem_Struct ** tmp_ptr)
{
    switch(mem & MEM_BLOC_MASK){
#ifdef EXTMEM_ALLOC
    case EXT_MEM24:
      *tmp_ptr = heap_Ext24_ptr;
    break;
  case EXT_MEM16:
    *tmp_ptr = heap_Ext16_ptr;
    return (unsigned int *)MEM_BANK_16BITS;
#endif //EXTMEM_ALLOC

#ifdef ESRAM_ALLOC
  case ESRAM_24:
    *tmp_ptr = heap_Esram24_ptr;
    break;
  case ESRAM_16:
    *tmp_ptr = heap_Esram16_ptr;
    return (unsigned int *)MEM_BANK_16BITS;
#endif //ESRAM_ALLOC

  case X_MEM:
  case Y_MEM:
    *tmp_ptr = heap_ptr;
    break;
  default:
    *tmp_ptr = NULL;
    break;
  }

  return (unsigned int *)MEM_BANK_24BITS;
}

/*
 * Calculate the free memory size for a given memory (X, Y , SDRAM or ESRAM).
 */
size_t mget_mem_size_left(Mem_Bloc mem)
{
    size_t size, size_in_24b_mode;
#if defined(EXTMEM_ALLOC) || defined(ESRAM_ALLOC)
    Mem_Struct *tmp_ptr2;
#endif
    unsigned int bool_16bits;
    Mem_Struct *tmp_ptr = NULL;

    bool_16bits = (unsigned int)mattach_to_mem_bank(mem, &tmp_ptr);

    size = 0;

#if defined(EXTMEM_ALLOC) || defined(ESRAM_ALLOC)
    if (bool_16bits)
    {
        while(tmp_ptr != NULL)
        {
            if((tmp_ptr->status & MEM_STATUS_MASK) == MEM_FREE)
            {
#ifndef __mode16__ //in 24 bits DCU mode
                /* read a size_t data in 24 bits mode from 16 bits external memory.
                 * THE DIRECT AFFECTATION IS INCORRECT,
                 * espacially when the given size is greater than 2^16 !!!!
                 */
                size_in_24b_mode = *(unsigned int *)&tmp_ptr->size & 0xFFFF;
                size_in_24b_mode = (size_in_24b_mode << 16) | (*((unsigned int *)&tmp_ptr->size + 1) & 0xFFFFU);
                size += size_in_24b_mode;
#else //in 16 bits DCU mode
                size += tmp_ptr->size;
#endif //__mode16__
            }
            tmp_ptr2 = tmp_ptr->prev;
            //!! do "tmp_ptr->next & 0xFFFF", this is because the sign extension when 16 bits -> 24 bits
            tmp_ptr = (Mem_Struct *)(((unsigned long)tmp_ptr2 << 16) | ( (unsigned long)tmp_ptr->next & 0xFFFFU));
        }
        return size;
    }
    else
#endif //defined(EXTMEM_ALLOC) || defined(ESRAM_ALLOC)
    {
        while(tmp_ptr != NULL)
        {
            if((tmp_ptr->status & MEM_STATUS_MASK) == MEM_FREE &&
               ((tmp_ptr->status & MEM_BLOC_MASK) == mem))
                size += tmp_ptr->size;
            tmp_ptr = tmp_ptr->next;
        }
        return size;
    }
}


/*
 * This function searches, in the given memory bank "mem",
 * the biggest free bloc and returns its size.
 */
size_t mget_max_free_space(Mem_Bloc mem)
{
#if defined(EXTMEM_ALLOC) || defined(ESRAM_ALLOC)
    Mem_Struct * tmp_ptr2;
#endif
    Mem_Struct * tmp_ptr;
    size_t max_sz, size_in_24b_mode;
    unsigned int bool_16bits;

    bool_16bits = mattach_to_mem_bank(mem, &tmp_ptr);

    max_sz = 0;

#if defined(EXTMEM_ALLOC) || defined(ESRAM_ALLOC)
    if (bool_16bits)
    {
        while (tmp_ptr != NULL)
        {
            if ((tmp_ptr->status & MEM_STATUS_MASK) == MEM_FREE)
            {
#ifndef __mode16__ //in 24 bits DCU mode
              /* read a size_t data in 24 bits mode from 16 bits external memory.
               * THE DIRECT AFFECTATION IS INCORRECT,
               * espacially when the given size is greater than 2^16 !!!!
               */
              size_in_24b_mode = *(unsigned int *)&tmp_ptr->size & 0xFFFF;
              size_in_24b_mode = (size_in_24b_mode << 16) | (*((unsigned int *)&tmp_ptr->size + 1) & 0xFFFFU);
#else //in 16 bits DCU mode
              size_in_24b_mode = tmp_ptr->size;
#endif //__mode16__
              if (size_in_24b_mode > max_sz)
                  max_sz = size_in_24b_mode;
            }
            tmp_ptr2 = tmp_ptr->prev;
            //!! do "tmp_ptr->next & 0xFFFF", this is because the sign extension when 16 bits -> 24 bits
            tmp_ptr = (Mem_Struct *)(((unsigned long)tmp_ptr2 << 16) | ( (unsigned long)tmp_ptr->next & 0xFFFFU));
        }
        return max_sz;
    }
    else
#endif
    {
      while(tmp_ptr != NULL)
      {
          if((tmp_ptr->status & MEM_STATUS_MASK) == MEM_FREE &&
             ((tmp_ptr->status & MEM_BLOC_MASK) == mem))
            if (tmp_ptr->size > max_sz)
              max_sz = tmp_ptr->size;
          tmp_ptr = tmp_ptr->next;
      }
      return max_sz;
    }
}

Mem_Error mcheck_heap_integrity(t_ExtendedAllocParams * params) {

    unsigned int i, start_idx, end_idx;

    TRACE_t * this = (TRACE_t *)params->trace_p;

    if (params->bank == MEM_BANK_COUNT)
    {
        start_idx = 0;
        end_idx = MEM_BANK_COUNT;
    }
    else
    {
        start_idx = params->bank;
        end_idx = start_idx + 1;
    }

    for (i = start_idx; i < end_idx; i++)
    {
        Mem_Struct *tmp_ptr = heap_ptr_tab[i];

        OstTraceFiltInst1(TRACE_DEBUG, "AFM_MPC: libmalloc: mcheck_heap_integrity for heap type %d", i);

        while(tmp_ptr != NULL)
        {
            if (tmp_ptr->custom != CUSTOM_CHECKSUM)
            {
                OstTraceFiltInst1(TRACE_ERROR, "AFM_MPC: libmalloc: mcheck_heap_integrity failed for heap type %d", i);

                return MEM_INTEGRITY_FAILED;
            }

            if((i==MEM_DDR16) || (i==MEM_ESR16)) {
                //!! do "tmp_ptr->next & 0xFFFFU", this is because the sign extension when 16 bits -> 24 bits
                Mem_Struct *tmp_ptr2 = tmp_ptr->prev;
                tmp_ptr2 = (Mem_Struct *)(((unsigned long)tmp_ptr2 << 16) | ((unsigned long)tmp_ptr->next & 0xFFFFU));
                tmp_ptr = tmp_ptr2;
            } else {
                if (i==MEM_YTCM) {
                    tmp_ptr = tmp_ptr->prev;
                    if((tmp_ptr->status & MEM_BLOC_MASK) != Y_MEM) tmp_ptr=NULL;
                } else {
                    tmp_ptr = tmp_ptr->next;
                    if((i==MEM_XTCM)&&((tmp_ptr->status & MEM_BLOC_MASK) != X_MEM)) tmp_ptr=NULL;
                }
            }
        }
    }

    OstTraceFiltInst1(TRACE_DEBUG, "AFM_MPC: libmalloc: mcheck_heap_integrity OK for heap type %d", i);

    return MEM_SUCCESS;
}

/*
 * This function dumps information about a given heap
 * like the base poitner, the size at init time, the
 * remaining size, and dumps all memory block information
 */
void mdump_heap_info(t_ExtendedAllocParams * params)
{
    unsigned int i, start_idx, end_idx;
    TRACE_t * this = (TRACE_t *)params->trace_p;

    if (params->bank == MEM_BANK_COUNT)
    {
        start_idx = 0;
        end_idx = MEM_BANK_COUNT;
    }
    else
    {
        start_idx = params->bank;
        end_idx = start_idx + 1;
    }

    for (i = start_idx; i < end_idx; i++)
    {
       Mem_Struct *tmp_ptr = heap_ptr_tab[i];

        OstTraceFiltInst1(TRACE_DEBUG, "AFM_MPC: libmalloc: dump_heap_info for heap type %d", i);
        OstTraceFiltInst2(TRACE_DEBUG, "AFM_MPC: libmalloc: \t heap size: %u, left size:%u", mem_Size_Init[i], mem_Size_Left[i]);

        while(tmp_ptr != NULL)
        {
            if((i==MEM_DDR16) || (i==MEM_ESR16)) {
                Mem_Struct *tmp_ptr2 = tmp_ptr->prev;
                size_t size_in_24b_mode;
                size_in_24b_mode = *(unsigned int *)&tmp_ptr->size & 0xFFFFU;
                size_in_24b_mode = (size_in_24b_mode << 16) | (*((unsigned int *)&tmp_ptr->size + 1) & 0xFFFFU);
                OstTraceFiltInst2(TRACE_DEBUG, "AFM_MPC: libmalloc: \t\tblock 0x%x, size %d", tmp_ptr, (unsigned int)size_in_24b_mode);
                //!! do "tmp_ptr->next & 0xFFFFU", this is because the sign extension when 16 bits -> 24 bits
                tmp_ptr2 = (Mem_Struct *)(((unsigned long)tmp_ptr2 << 16) | ((unsigned long)tmp_ptr->next & 0xFFFFU));
                OstTraceFiltInst3(TRACE_DEBUG, "AFM_MPC: libmalloc: \t\t\tnext 0x%x, status 0x%x, custom 0x%x", tmp_ptr2, tmp_ptr->status, tmp_ptr->custom);
                tmp_ptr = tmp_ptr2;
            } else {
                OstTraceFiltInst3(TRACE_DEBUG, "AFM_MPC: libmalloc: \t\tblock 0x%x, size %d, prev 0x%x", tmp_ptr, (unsigned int)tmp_ptr->size, tmp_ptr->prev);
                OstTraceFiltInst3(TRACE_DEBUG, "AFM_MPC: libmalloc: \t\t\tnext 0x%x, status 0x%x, custom 0x%x", tmp_ptr->next, tmp_ptr->status, tmp_ptr->custom);
                if (i==MEM_YTCM) {
                    tmp_ptr = tmp_ptr->prev;
                    if((tmp_ptr->status & MEM_BLOC_MASK) != Y_MEM) tmp_ptr=NULL;
                } else {
                    tmp_ptr = tmp_ptr->next;
                    if((i==MEM_XTCM)&&((tmp_ptr->status & MEM_BLOC_MASK) != X_MEM)) tmp_ptr=NULL;
                }
            }
        }
    }
}

