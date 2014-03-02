/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _fwmalloc_h_
#define _fwmalloc_h_
#include <stdlib.h>
#include "audiolibs_common.h"

#ifdef _NMF_MPC_

#define FW_MEMORY_BANK(name,ptr,idx) MEMORY_TYPE_T name = *(((MEMORY_TYPE_T *)ptr)+idx);
#define fw_malloc(memory_bank, size)     malloc_core((t_heap_info*)itf->heap_info_table[(memory_bank)], (size))
#define fw_malloc_16(memory_bank, size)  misc_malloc_mode16((t_heap_info*)itf->heap_info_table[(memory_bank)], (size))
#define free_from16(ptr) 
#define free(ptr) 

#else // _NMF_MPC_

#ifndef __flexcc2__
#define FW_MEMORY_BANK(name,ptr,idx)
#define fw_malloc(memory_bank, size) malloc(size)
#define fw_malloc_16(memory_bank, size) malloc(size)
#define malloc_y_from16(size) malloc(size)
#define free_from16(ptr) free(ptr)

#else // __flexcc2__

/*-------------
 * tyepdef 
 *-------------*/
typedef struct
{
  void *(*malloc)(size_t size);
}t_malloc_func;
 

extern t_malloc_func const EXTMEM malloctable[BANK_COUNT];
extern t_malloc_func const EXTMEM malloctable_from16[BANK_COUNT];


/* table of malloc functions */
void *malloc_from16(size_t size);
void *malloc_ext_from16(size_t size);
void *malloc_ext16_from16(size_t size);
void *malloc_esram_from16(size_t size);
void *malloc_esram16_from16(size_t size);
void YMEM *malloc_y_from16(size_t size);
void free_from16(void * ptr);

#define FW_MEMORY_BANK(name,ptr,idx) MEMORY_TYPE_T name = *(((MEMORY_TYPE_T *)ptr)+idx);
#define fw_malloc(memory_bank, size) malloctable[(memory_bank)].malloc((size));
#define fw_malloc_16(memory_bank, size) malloctable_from16[(memory_bank)].malloc((size));
#endif // __flexcc2__

#endif // _NMF_MPC_


#endif // _fwmalloc_h_


