/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * Includes							       
 */
#include "dma_api.h"

/*****************************************************************************/
/**
 * \brief 	Stop dma process and wait they are finished
 * \author 	Loic Habrial
 * \param   dma_dir DMA dir register for process abort
 * \param   dma_enr DMA enr register, process curently running
 * \param   dma_stop DMA process to stop
 * \param   dma_prog DMA process to wait end
 *
 * Write DMA abort register to stop DMA processes and wait process
 * 
 */
/*****************************************************************************/
void dma_stop_wait_process(t_uint16 dma_dir_l,
                           t_uint16 dma_dir_h,
                           t_uint16 dma_enr_l,
                           t_uint16 dma_enr_h,
                           t_uint32 dma_stop,
                           t_uint32 dma_prog)                                 
{

    /* Write 1 if process = 0 has no effect */
    DMA_SET_REG_32(dma_dir_l, dma_dir_h, dma_stop);

    /* if this is called with no dma_stop, the first while won't work !! */
    dma_stop = 1;

    /* Wait for DMA end of process... */
    while(dma_stop != 0)
    {
        /* Read ENR register to verify that process not finished */
        dma_stop = DMA_GET_REG_32(dma_enr_l, dma_enr_h);
        dma_stop = dma_prog & dma_stop;
    }

    return;
}

