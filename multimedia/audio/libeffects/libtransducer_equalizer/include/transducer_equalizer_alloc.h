/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   transducer_equalizer_alloc.h
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _transducer_equalizer_alloc_h_
#define _transducer_equalizer_alloc_h_


//#define  TRANSDUCER_EQUALIZER_VERIF_MALLOC_FREE


#ifdef  TRANSDUCER_EQUALIZER_VERIF_MALLOC_FREE

#define _fwmalloc_h_    // to prevent fwmalloc.h defintions usage
#include "audiolibs_common.h"

typedef struct
{
    void *(*malloc)(size_t size);
} t_malloc_func;

extern t_malloc_func EXTMEM debug_malloctable[BANK_COUNT];
extern void YMEM *debug_malloc_y(size_t size);
extern void debug_free(void *ptr);
#define malloc_y    debug_malloc_y
#define free        debug_free

#define fw_malloc(memory_bank, size) debug_malloctable[(memory_bank)].malloc((size))

#else /* TRANSDUCER_EQUALIZER_VERIF_MALLOC_FREE */

#include "audiolibs_common.h"

extern void YMEM *malloc_y(size_t size);

#endif /*  TRANSDUCER_EQUALIZER_VERIF_MALLOC_FREE */


#endif /* _transducer_equalizer_alloc_h_ */
