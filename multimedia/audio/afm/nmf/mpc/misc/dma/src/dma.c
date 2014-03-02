/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   dma.c
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include <misc/dma.nmf>
#include <misc/dma/include/dma.h>
#include <dbc.h>


t_uint16 get_dma_interrupt(t_uint16 dma_channel)
{
    switch (dma_channel) {
        case 0: return 21;
        case 1: return 22;
        case 2: return 23;
        case 3: return 25;
        case 4: return 26;
        case 5: return 27;
        case 6: return 28;
        case 7: return 29;
    }

    ASSERT(0);
    return 0;
}


void enable_dma_interrupt(t_uint16 dma_channel)
{
    t_uint16 IT = get_dma_interrupt(dma_channel);

    SET_BIT(ITREMAP_interf_itmemoh, IT - 16);
    UNMASK_IT(IT);

    MASK_IT(30);
}


void disable_dma_interrupt(t_uint16 dma_channel)
{
    t_uint16 IT = get_dma_interrupt(dma_channel);

    MASK_IT(IT);
    RESET_BIT(ITREMAP_interf_itmemoh, IT - 16);
}


void ack_dma_interrupt(t_uint16 dma_channel)
{
    volatile armDmaConfig MMIO * DmaChannel = MAP_DMA_REGS(dma_channel);

    (void)DmaChannel->IT;
}


void request_dma_transfer(t_uint16 dma_channel)
{
    volatile armDmaConfig MMIO * DmaChannel = MAP_DMA_REGS(dma_channel);

    DmaChannel->BREQ = 1;
}


void cancel_dma_transfer(t_uint16 dma_channel)
{
    volatile armDmaConfig MMIO * DmaChannel = MAP_DMA_REGS(dma_channel);

    DmaChannel->BREQ = 0;
}


void init_dma_channel(t_uint16 dma_channel, void *buffer, t_uint16 buffer_size)
{
    volatile armDmaConfig MMIO * DmaChannel = MAP_DMA_REGS(dma_channel);

    DmaChannel->buf1   = (int) buffer;
    DmaChannel->buf2   = (int) buffer + buffer_size;
    DmaChannel->size   = buffer_size;
    DmaChannel->MASKIT = 2;
    DmaChannel->AUTO   = 1;
}


void reset_dma_channel(t_uint16 dma_channel)
{
    volatile armDmaConfig MMIO * DmaChannel = MAP_DMA_REGS(dma_channel);

    disable_dma_interrupt(dma_channel);

    DmaChannel->SREQ  = 0;
    DmaChannel->BREQ  = 0;
    DmaChannel->LSREQ = 0;
    DmaChannel->LBREQ = 0;
    DmaChannel->MASKIT= 0;
    DmaChannel->AUTO  = 0;
    DmaChannel->LAUTO = 0;
}


void swap_bytes(int *buffer, t_uint32 size)
{
    int i;
    for (i = 0; i < size; i++) {
        buffer[i] = ((buffer[i] & 0xFF) << 8) | ((buffer[i] >> 8) & 0xFF);
    }
}

