/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   dma.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _DMA_H_
#define _DMA_H_

#include <archi.h>

typedef struct {
    int     SREQ;
    int     BREQ;
    int     LSREQ;
    int     LBREQ;
    int     MASKIT;
    int     IT;
    int     AUTO;
    int     LAUTO;

    // virtual mmdspsim registers
    int     eof;
    int     buf1;
    int     buf2;
    int     size;
    int     word_size;
    int     sample_freq;
    int     stereo;
    int     dummy;
} armDmaConfig;


#define MAP_DMA_REGS(channel)   ((MMIO int *) &ARM_DMA0_sreq + (channel) * 0x10)

#define   SET_BIT(a,b) ((a) |=  (1<<(b)))
#define RESET_BIT(a,b) ((a) &= ~(1<<(b)))

#define MASK_IT(it)       { SET_BIT(ITREMAP_interf_itmskh, (it) - 16); \
                          ITREMAP_interf_itmskh; \
                          ITREMAP_interf_itmskh; \
                          ITREMAP_interf_itmskh; }

#define UNMASK_IT(it)     RESET_BIT(ITREMAP_interf_itmskh, (it) - 16)

#endif // _DMA_H_
