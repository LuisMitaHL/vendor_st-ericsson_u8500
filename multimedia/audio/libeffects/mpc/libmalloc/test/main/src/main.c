/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

#include <archi.h>
#include <stdio.h>
#include "malloc.h"
#include <stdlib.h>

#include <libeffects/mpc/libmalloc/test/main.nmf>

#include "dsp_mem_map.h"

#define PTRS_ARRAY_SZ 50

unsigned int * ptrs[PTRS_ARRAY_SZ];
unsigned int * ptrs_y[PTRS_ARRAY_SZ];
unsigned int * ptrs_ext[PTRS_ARRAY_SZ];
unsigned int * ptrs_ext16[PTRS_ARRAY_SZ];
unsigned int * ptrs_esram[PTRS_ARRAY_SZ];
unsigned int * ptrs_esram16[PTRS_ARRAY_SZ];

// memory size is in number of words
#define YRAM_SIZE               (((unsigned int)(8 *1024))/sizeof (int))
#define XRAM_START              (U8500_TCM_BASE_ADDRESS + (U8500_TCM_SIZE/2))
#define XRAM_SIZE               (((U8500_TCM_SIZE/2) - YRAM_SIZE)/sizeof (int))
#define YRAM_START              (U8500_TCM_BASE_ADDRESS + U8500_TCM_SIZE - YRAM_SIZE)

#define ESRAM_SIZE              ((unsigned int)(24 * 1024))
#define DDR_SIZE                ((unsigned int)(24 * 1024))

unsigned int Initial_Mem_Size_Tab[NB_MEMORY_SPACE] = {
  XRAM_SIZE,
  YRAM_SIZE,
  DDR_SIZE,
  DDR_SIZE,
  ESRAM_SIZE,
  ESRAM_SIZE
};

extern size_t mget_mem_size_left(Mem_Bloc mem);

int test_free(int nb_free, Mem_Bloc mem_type, int ptr_block_idx);
int test_alloc(int nb_alloc, Mem_Bloc mem_type, size_t * block_size);
int mymain();

void METH(start)(void) {
    mymain();
}

int mymain()
{

    if (init_intmem(XRAM_START, XRAM_SIZE, YRAM_START, YRAM_SIZE) != MEM_SUCCESS)
    {
        printf("Error while initializing TCM");
        return MEM_INVALID_ADDRESS;
    }

    if (init_esram(U8500_ESRAM_24_BASE_ADDRESS, ESRAM_SIZE, U8500_ESRAM_16_BASE_ADDRESS, ESRAM_SIZE))
    {
        printf("Error while initializing ESRAM");
        return MEM_INVALID_ADDRESS;
    }

    if (init_extmem(U8500_DDR_24_BASE_ADDRESS, DDR_SIZE, U8500_DDR_16_BASE_ADDRESS, DDR_SIZE))
    {
        printf("Error while initializing DDR");
        return MEM_INVALID_ADDRESS;
    }


    printf("Available Xmem size is %6d\n", (unsigned int)mget_mem_size_left(X_MEM));
    printf("Available Ymem size is %6d\n", (unsigned int)mget_mem_size_left(Y_MEM));
    printf("Available 24 bits External mem size is %6d\n", (unsigned int)mget_mem_size_left(EXT_MEM24));
    printf("Available 16 bits External mem size is %6d\n", (unsigned int)mget_mem_size_left(EXT_MEM16));
    printf("Available 24 bits Esram size is %6d\n", (unsigned int)mget_mem_size_left(EXT_MEM24));
    printf("Available 16 bits Esram size is %6d\n", (unsigned int)mget_mem_size_left(EXT_MEM16));
    printf("Available 24 bits External mem size is %6d\n", (unsigned int)mget_mem_size_left(EXT_MEM24));
    printf("Available 16 bits External mem size is %6d\n", (unsigned int)mget_mem_size_left(EXT_MEM16));
    printf("Available 24 bits Esram size is %6d\n", (unsigned int)mget_mem_size_left(ESRAM_24));
    printf("Available 16 bits Esram size is %6d\n", (unsigned int)mget_mem_size_left(ESRAM_16));

    /* Test strategy:
       Allocate 5 blocks in x mem,
       then 6 blocks in y mem,
       then 7 ddr24 blocks,
       then 8 ddr16 blocks,
       then 9 esram24 blocks,
       then 10 esram16 blocks */
    {
        int alloc_idx;
        size_t sz_tab[10] = { 11, 15, 64, 22, 840, 20, 78, 96, 46, 100 };

        test_alloc(5, X_MEM, &sz_tab[0]);
        test_alloc(6, Y_MEM, &sz_tab[0]);
        test_alloc(7, EXT_MEM24, &sz_tab[0]);
        test_alloc(8, EXT_MEM16, &sz_tab[0]);
        test_alloc(9, ESRAM_24, &sz_tab[0]);
        test_alloc(10, ESRAM_16, &sz_tab[0]);

    /* at that point free 2 blocks in each memory type (not the first blocks) */

        test_free(1, X_MEM, 1);
        test_free(1, X_MEM, 3);

        test_free(1, Y_MEM, 2);
        test_free(1, Y_MEM, 4);

        test_free(1, EXT_MEM24, 3);
        test_free(1, EXT_MEM24, 5);

        test_free(1, EXT_MEM16, 4);
        test_free(1, EXT_MEM16, 6);

        test_free(1, ESRAM_24, 5);
        test_free(1, ESRAM_24, 7);

        test_free(1, ESRAM_16, 6);
        test_free(1, ESRAM_16, 8);


    /* then allocate more blocks in each mem type, so as to fullfill
       all heaps */
        {
          unsigned int mem_type_idx;
          for ( mem_type_idx =0; mem_type_idx < NB_MEMORY_SPACE; mem_type_idx ++)
          {
              int status;
              do
              {
                  status = test_alloc(1, (mem_type_idx << 4), &sz_tab[4]);
              } while (status == 0);
          }
        }

    /* then check heap integrity before free */
        {
            unsigned int mem_type_idx;
            for ( mem_type_idx =0; mem_type_idx < NB_MEMORY_SPACE; mem_type_idx ++)
            {
                Mem_Error err;
                t_ExtendedAllocParams  params;
                params.bank = mem_type_idx;
                params.trace_p = NULL;
                err = mcheck_heap_integrity(&params);
                if(err != MEM_SUCCESS) {
                    printf("mcheck_heap_integrity FAILED for heap type %d", mem_type_idx);
                    return err;
                }
            }
        }

    /* then free all memory blocks, starting by the blocks in the middle */

        {
          unsigned int mem_type_idx;
          for ( mem_type_idx =0; mem_type_idx < NB_MEMORY_SPACE; mem_type_idx ++)
          {
              unsigned int i = PTRS_ARRAY_SZ/2 ;
              test_free(PTRS_ARRAY_SZ/2, (mem_type_idx << 4), i);
              test_free(PTRS_ARRAY_SZ/2, (mem_type_idx << 4), 0);
          }
        }
    }

    printf("Available Xmem size is %6d\n", (unsigned int)mget_mem_size_left(X_MEM));
    printf("Available Ymem size is %6d\n", (unsigned int)mget_mem_size_left(Y_MEM));
    printf("Available 24 bits External mem size is %6d\n", (unsigned int)mget_mem_size_left(EXT_MEM24));
    printf("Available 16 bits External mem size is %6d\n", (unsigned int)mget_mem_size_left(EXT_MEM16));
    printf("Available 24 bits Esram size is %6d\n", (unsigned int)mget_mem_size_left(EXT_MEM24));
    printf("Available 16 bits Esram size is %6d\n", (unsigned int)mget_mem_size_left(EXT_MEM16));
    printf("Available 24 bits External mem size is %6d\n", (unsigned int)mget_mem_size_left(EXT_MEM24));
    printf("Available 16 bits External mem size is %6d\n", (unsigned int)mget_mem_size_left(EXT_MEM16));
    printf("Available 24 bits Esram size is %6d\n", (unsigned int)mget_mem_size_left(ESRAM_24));
    printf("Available 16 bits Esram size is %6d\n", (unsigned int)mget_mem_size_left(ESRAM_16));

    printf("****** END ******\n");
    exit(0);
}

unsigned int * ptrs_tab[NB_MEMORY_SPACE] = {
    ptrs,
    ptrs_y,
    ptrs_ext,
    ptrs_ext16,
    ptrs_esram,
    ptrs_esram16
};

int nb_alloc_tab[NB_MEMORY_SPACE] = {0, 0, 0, 0, 0, 0};

#define MEM_BLOC_TO_IDX(x) (x>>4)

int test_alloc(int nb_alloc, Mem_Bloc mem_type, size_t * block_size)
{
    unsigned int i;
    size_t left_sz;
    int ptr_idx = -1;
    int mem_type_idx = MEM_BLOC_TO_IDX(mem_type);

    i = 0;

    // search for an available slot in the pointer table
    while (i < PTRS_ARRAY_SZ)
    {
        if (ptrs_tab[mem_type_idx][i] == NULL)
        {
            ptr_idx = i;
            break;
        }
        i++;
    }

    if (i == PTRS_ARRAY_SZ)
    {
        printf("Error: Trying to allocate while no slot is available\n");
        Panic(USER_PANIC, 0);
        return -1;
    }

    i = 0;

    while(i < nb_alloc)
    {
        t_ExtendedAllocParams  params;
        params.bank = mem_type_idx;
        params.trace_p = NULL;

        ptrs_tab[mem_type_idx][ptr_idx] = (unsigned int*) vmalloc(block_size[i], &params);

        left_sz = mget_mem_size_left(mem_type);

        if (ptrs_tab[mem_type_idx][ptr_idx] == NULL)
        {
            printf("Alloc failed [%s]: requested size is %d, available size is %d/%d\n",
                 mem_name_tab[mem_type_idx],
                 (unsigned int)block_size[i],
                 (unsigned int)left_sz,
                 (unsigned int)Initial_Mem_Size_Tab[mem_type_idx]);
            return -1;
        }
        else
        {
            printf("Alloc [%s]: sz %d at 0x%p, available size %d/%d\n",
                   mem_name_tab[mem_type_idx],
                   (unsigned int)block_size[i],
                   ptrs_tab[mem_type_idx][ptr_idx],
                   (unsigned int)left_sz,
                   (unsigned int)Initial_Mem_Size_Tab[mem_type_idx]);
        }
        i++; ptr_idx++;
    }
    return 0;
}

int test_free(int nb_to_free, Mem_Bloc mem_type, int ptr_block_idx) // free no NULL pointers in ptrs[] array.
{
    unsigned int i;
    int free_ret;
    size_t left_sz;
    int mem_type_idx = MEM_BLOC_TO_IDX(mem_type);
    int ptr_idx = -1;

    i = 0;
    while (i < nb_to_free)
    {
        ptr_idx = ptr_block_idx + i;
        if (ptrs_tab[mem_type_idx][ptr_idx] != NULL)
        {
            t_ExtendedAllocParams  params;
            params.bank = mem_type_idx;
            params.trace_p = NULL;

            free_ret = vfree(ptrs_tab[mem_type_idx][ptr_idx], &params);
            left_sz = mget_mem_size_left(mem_type);

            if (free_ret == FREE_SUCCESS)
            {
                printf("free [%s]: at 0x%p [id %d], available size %d/%d\n",
                       mem_name_tab[mem_type_idx],
                       ptrs_tab[mem_type_idx][ptr_idx],
                       ptr_idx,
                       (unsigned int)left_sz,
                       (unsigned int)Initial_Mem_Size_Tab[mem_type_idx]);

                ptrs_tab[mem_type_idx][ptr_idx] = NULL;
            }
            else
            {
                printf("free [%s] FAILED: at 0x%p\n",
                       mem_name_tab[mem_type_idx], 
                       ptrs_tab[mem_type_idx][ptr_idx]);
                return free_ret;
            }
        }
      i++;
    }
    return free_ret;
}
