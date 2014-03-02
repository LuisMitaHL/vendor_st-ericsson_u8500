/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef __INC_ITE_INIT_H
#define __INC_ITE_INIT_H

#include <inc/type.h>

/*--------------------------------------------------------------------------*
 * Global variables                                                         *
 *--------------------------------------------------------------------------*/
/* IMPORT DECLARATIONS FROM SCATTER FILE THAT DEFINE MEMORY PAGES*/

/*--------------------------------------------------------------------------*
 * Types                                                                    *
 *--------------------------------------------------------------------------*/
typedef enum
{
    IDX_SDRAM_SHARE_BANK1 = 0x0,
    IDX_ESRAM_BANKS,
    IDX_SDRAM_SHARE_BANK2
} t_8820nmf_idx_memory_page;


#ifdef __cplusplus
extern "C"
{
#endif


/*--------------------------------------------------------------------------*
 * Functions                                                                *
 *--------------------------------------------------------------------------*/
void VAL_RemovePageFromCache(t_8820nmf_idx_memory_page idx_page_to_remove);
void ITE_InitializeComponentManager( void );


#ifdef __cplusplus
}
#endif

#endif /* __INC_ITE_INIT_H */
