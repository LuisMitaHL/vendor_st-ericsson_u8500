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

//for X, Y EXTMEM24, ESRAM24 free
int Internal_Free1(Mem_Struct *tmp_ptr){
  /*
   * Forward merge
   */
    if(tmp_ptr->next != NULL)
    {
        // Merge when blocs intertwines
        if(((tmp_ptr->next->status & MEM_STATUS_MASK) == MEM_FREE)&&
           ((Mem_Struct*)tmp_ptr->next == (Mem_Struct*)((int*)tmp_ptr + tmp_ptr->size) ))
        {
            tmp_ptr->size += tmp_ptr->next->size;
            tmp_ptr->next = tmp_ptr->next->next;
            if(tmp_ptr->next != NULL)
                tmp_ptr->next->prev = tmp_ptr;
#ifdef Y_ALLOC
            else if((tmp_ptr->status & MEM_BLOC_MASK) == Y_MEM)
              last_Y_ptr = tmp_ptr;
#endif //Y_ALLOC
        }
    }
    /*
     * Backward	merge
     */
    if(tmp_ptr->prev != NULL)// Cas du 1er elt a prendre en compte.
    {
        // Merge when blocs intertwines
        if(((tmp_ptr->prev->status  & MEM_STATUS_MASK) == MEM_FREE) &&
           ((Mem_Struct*)((int*)tmp_ptr->prev + tmp_ptr->prev->size) == (Mem_Struct*)tmp_ptr))
        {
            tmp_ptr = tmp_ptr->prev;
            tmp_ptr->size += tmp_ptr->next->size;
            tmp_ptr->next = tmp_ptr->next->next;
            if(tmp_ptr->next != NULL)
            {
                tmp_ptr->next->prev = tmp_ptr;
            }
#ifdef Y_ALLOC
            else
            {
                if((tmp_ptr->status & MEM_BLOC_MASK) == Y_MEM)
                    last_Y_ptr = tmp_ptr;
            }
#endif //Y_ALLOC
        }
    }
    return FREE_SUCCESS;
}

#if defined(EXTMEM_ALLOC) || defined(ESRAM_ALLOC)
//for EXTMEM16, ESRAM16 free
int Internal_Free2(Mem_Struct *tmp_ptr, Mem_Bloc bloc){
    Mem_Struct * tmp_ptr2;
    Mem_Struct * tmp_ptr3;
    size_t size_in_24b_mode, size_in_24b_mode_2;

    /* read a size_t data in 24 bits mode from 16 bits external memory.
     * THE DIRECT AFFECTATION IS INCORRECT,
     * especially when the given size is greater than 2^16 !!!!
   */
    size_in_24b_mode = *((unsigned int *)&tmp_ptr->size) & 0xFFFFU;
    size_in_24b_mode = (size_in_24b_mode << 16) | (*(((unsigned int *)&tmp_ptr->size) + 1) & 0xFFFFU);

    /*
     * Forward merge.
     * Contrary to X, Y EXTMEM24, EXTMEM16 is organized by a uni-direction chain.
     * The fields "prev" and "next" are used to store the address of next block.
     */
    tmp_ptr2 = tmp_ptr->prev;
    //!! do "tmp_ptr->next & 0xFFFF", this is because the sign extension when 16 bits -> 24 bits
    tmp_ptr2 = (Mem_Struct *)(((unsigned long)tmp_ptr2 << 16) | ((unsigned long)tmp_ptr->next & 0xFFFFU));
    if (tmp_ptr2 != NULL){ //not the last block
        if ((tmp_ptr2->status & MEM_STATUS_MASK) == MEM_FREE){
            tmp_ptr->prev = tmp_ptr2->prev;
            tmp_ptr->next = tmp_ptr2->next;

            //like : tmp_ptr->size += size_in_24b_mode;
            /* read a size_t data in 24 bits mode from 16 bits external memory.
             * THE DIRECT AFFECTATION IS INCORRECT,
             * espacially when the given size is greater than 2^16 !!!!
             */
            size_in_24b_mode_2 = *((unsigned int *)&tmp_ptr2->size) & 0xFFFFU;
            size_in_24b_mode_2 = (size_in_24b_mode_2 << 16) | (*(((unsigned int *)&tmp_ptr2->size) + 1) & 0xFFFFU);
            size_in_24b_mode += size_in_24b_mode_2;

            /*stock a size_t data in 24 bits mode to 16 bits external memory.
             * THE DIRECT AFFECTATION IS INCORRECT,
             * espacially when the given size is greater than 2^16 !!!!
             */
            *((unsigned int *)&tmp_ptr->size) = size_in_24b_mode >> 16;
            *(((unsigned int *)&tmp_ptr->size) + 1) = size_in_24b_mode & 0xFFFFU;
        }
    }

    /*
     * Backward merge.
     * Contrary to X, Y EXTMEM24, EXTMEM16 and ESRAM16 are organized by a uni-direction chain.
     * The fields "prev" and "next" are used to store the address of next block.
     * Need to find the previous block!
     */
#ifdef EXTMEM_ALLOC
    if (tmp_ptr != heap_Ext16_ptr )
#endif
    {
#ifdef ESRAM_ALLOC
        if (tmp_ptr != heap_Esram16_ptr)
#endif
        {
            //tmp_ptr is not the first block.
            //find the tmp_ptr's previous block.
            switch(bloc){
#ifdef EXTMEM_ALLOC
            case EXT_MEM16:
                tmp_ptr2 = heap_Ext16_ptr;
                break;
#endif
#ifdef ESRAM_ALLOC
            case ESRAM_16:
                tmp_ptr2 = heap_Esram16_ptr;
                break;
#endif
            default:
                return FREE_FAILED;
                break;
            }
            while ((int *)tmp_ptr2 + tmp_ptr2->size < tmp_ptr){
                tmp_ptr3 = tmp_ptr2->prev;
                //!! do "tmp_ptr2->next & 0xFFFF", this is because the sign extension when 16 bits -> 24 bits
                tmp_ptr2 = (Mem_Struct *)(((unsigned long)tmp_ptr3 << 16) | ((unsigned long)tmp_ptr2->next & 0xFFFFU));
            }
            if ((int *)tmp_ptr2 + tmp_ptr2->size > tmp_ptr)
                return FREE_FAILED;

            //we are now at tmp_ptr's previous block.
            if ((tmp_ptr2->status & MEM_STATUS_MASK) == MEM_FREE){
                tmp_ptr2->prev = tmp_ptr->prev;
                tmp_ptr2->next = tmp_ptr->next;

                //like : tmp_ptr2->size += tmp_ptr->size;
                /* read a size_t data in 24 bits mode from 16 bits external memory.
                 * THE DIRECT AFFECTATION IS INCORRECT,
                 * espacially when the given size is greater than 2^16 !!!!
                 */
                size_in_24b_mode_2 = *((unsigned int *)&tmp_ptr2->size) & 0xFFFFU;
                size_in_24b_mode_2 = (size_in_24b_mode_2 << 16) | (*(((unsigned int *)&tmp_ptr2->size) + 1) & 0xFFFFU);
                size_in_24b_mode_2 += size_in_24b_mode;
                /*stock a size_t data in 24 bits mode to 16 bits external memory.
                 * THE DIRECT AFFECTATION IS INCORRECT,
                 * espacially when the given size is greater than 2^16 !!!!
                 */
                *((unsigned int *)&tmp_ptr2->size) = size_in_24b_mode_2 >> 16;
                *(((unsigned int *)&tmp_ptr2->size) + 1) = size_in_24b_mode_2 & 0xFFFFU;

            }
        }
    }
    return FREE_SUCCESS;
}
#endif //defined(EXTMEM_ALLOC) || defined(ESRAM_ALLOC

/*
 * This function checks if the addr 'mem' is a valid allocated memory address
 */
int check_free_addr(Mem_Bloc bloc, Mem_Struct * addr)
{
    Mem_Struct * start_ptr;
    Mem_Struct * tmp_ptr2;

    if (bloc==X_MEM || bloc==Y_MEM)
        start_ptr = heap_ptr;
#ifdef EXTMEM_ALLOC
    else if (bloc==EXT_MEM24)
        start_ptr = heap_Ext24_ptr;
    else if (bloc==EXT_MEM16)
        start_ptr = heap_Ext16_ptr;
#endif
#ifdef ESRAM_ALLOC
    else if (bloc==ESRAM_24)
        start_ptr = heap_Esram24_ptr;
    else if (bloc==ESRAM_16)
        start_ptr = heap_Esram16_ptr;
#endif
    else
        return INVALIDE_ALLOCATED_MEM_ADDR;

    if (addr == start_ptr)
        return 0;

    //more than one allocated block
    if (bloc==ESRAM_16 || bloc==EXT_MEM16)
    {
        tmp_ptr2 = start_ptr;
        while (tmp_ptr2 != NULL && tmp_ptr2 != addr)
            tmp_ptr2 = (Mem_Struct *)(((unsigned long)(tmp_ptr2->prev) << 16) | ((unsigned long)tmp_ptr2->next & 0xFFFFU));
        if (tmp_ptr2 == NULL) //not find addr in the chain
            return INVALIDE_ALLOCATED_MEM_ADDR;
    }
    else
    {
        if ((addr->next!=NULL && addr->next->prev==addr)
            || (addr->prev != NULL && addr->prev->next==addr))
            return 0;
        else
          return INVALIDE_ALLOCATED_MEM_ADDR;
    }
}


int Mem_Free(void *mem)
{
    Mem_Struct *tmp_ptr;
    int it_mask;
    Mem_Bloc bloc;

    if(mem == NULL)
        return FREE_FAILED;

    tmp_ptr = (Mem_Struct*)((int*)mem - sizeof(Mem_Struct));

    if((tmp_ptr->status & MEM_STATUS_MASK )!= MEM_USED)
    {
        return FREE_FAILED;
    }

    if (tmp_ptr->custom != CUSTOM_CHECKSUM) return MEM_INTEGRITY_FAILED;

    bloc = (tmp_ptr->status & MEM_BLOC_MASK);

    //check the validity of 'mem'
    if (check_free_addr(bloc, tmp_ptr) == INVALIDE_ALLOCATED_MEM_ADDR)
        return FREE_FAILED;
    tmp_ptr->status = MEM_FREE | bloc;

    mem_Size_Left[(bloc>>4)] += tmp_ptr->size;

    switch(bloc){
    case X_MEM:
    case EXT_MEM24:
    case Y_MEM:
#ifdef ESRAM_ALLOC
    case ESRAM_24:
#endif
        Internal_Free1(tmp_ptr);
        break;
#ifdef EXTMEM_ALLOC
    case EXT_MEM16:
        Internal_Free2(tmp_ptr, bloc);
        break;
#endif //EXTMEM_ALLOC
#ifdef ESRAM_ALLOC
    case ESRAM_16:
        Internal_Free2(tmp_ptr, bloc);
        break;
#endif
    default:
      return FREE_FAILED;
    }

    return FREE_SUCCESS;
}


