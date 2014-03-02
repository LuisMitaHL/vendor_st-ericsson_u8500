/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   msp_dma_ctrlBT.c
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
//#ifdef OST_TRACE_COMPILER_IN_USE
//  #include "audio_audiocodec_mpc_msp_dma_ctrl_bt_src_msp_dma_ctrl_btTraces.h"
//#endif


#include <audiocodec/mpc/msp_dma_ctrl_bt.nmf>
#include <dbc.h>
#include "msp_registers.h"
#include "dma_registers.h"
#include "inc/archi-wrapper.h"
//-------------------------
// Pointer on HW register
static volatile MspConfig EXTMMIO * mspReg = (volatile EXTMMIO MspConfig *) MSP0_MMDSP_BASE_ADDR;

// Start/Stop Tx Dma transfer only if not yet started
static int mStartCount      = 0;


void start_DMA_MSP(t_uint16 samplerate){

    if(mStartCount) {

        return;
    }
    mStartCount++;
    //resetMSP(MSP0); // reset shoud NOT be called EVER
    mspReg->WMRK    = 0x9;
    // Init Rx
    mspReg->IMSC  |= 0x2;     // mask for msp overrun IT
    mspReg->RCF    = 0x2007;  // 1 elements of 32 bits, RDDLY=1 MSB first
    mspReg->DMACR |= 0x1;
    mspReg->MCR   |= 0x1;
    mspReg->RCE0   = 0x3;
    mspReg->GCR   |= 0x202UL;  // enable fifos

    // Init Tx
    mspReg->IMSC  |= 0x20UL;  // mask for msp underrun IT
    mspReg->TCF    = 0x0000007;  // 1 elements of 32 bits, TDDLY=1 MSB first
    mspReg->DMACR |= 0x2UL;
    mspReg->MCR   |= 0x20UL;
    mspReg->TCE0   = 0x3UL;
    //mspReg->IODLY |= 0x00020UL;  // works with or without
    mspReg->GCR   |= 0x155A50UL;  // this is mandatory

    if (samplerate == 8000)
        mspReg->SRG   |= 0x1F004A;   // internal bitclk = 19.2MHz/75 = 256 KHz (needed 256 KHZ = 2x16x8KHz), width = 1 bclk,frame periode = 32 bclk
    else
        mspReg->SRG   = 0x27001D;    //  SCKDIV=30, width = 1 bclk,frame periode = 40 bclk. internal bitclk = 19.2MHz/30/40 = 16 KHz

    // Remap Msp0 Tc interrupt
    // Async_IT[1]  = mmdsp_IT[17] =  MSP0_RX_TC
    // Async_IT[11] = mmdsp_IT[37] =  MSP0_TX_TC
    ITREMAP_interf_reg28 = 37;
    ITREMAP_interf_reg26 = 17;
    ITREMAP_interf_reg17 = 26;
    configureDMA();
    AUDIO_UNMASK_IT(30);
}

void stop_DMA_MSP (void) {
    if(--mStartCount) {
        return;
    }
    AUDIO_MASK_IT(30);
}

