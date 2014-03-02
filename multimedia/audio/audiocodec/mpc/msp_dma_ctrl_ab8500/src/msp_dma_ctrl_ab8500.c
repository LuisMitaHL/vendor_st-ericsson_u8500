/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   msp_dma_ctrl_ab8500.c
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"


#ifdef OST_TRACE_COMPILER_IN_USE
  #include "audio_audiocodec_mpc_msp_dma_ctrl_ab8500_src_msp_dma_ctrl_ab8500Traces.h"
#endif

#include <audiocodec/mpc/msp_dma_ctrl_ab8500.nmf>
#include <dbc.h>
#include "gpio_registers.h"
#include "prcc_registers.h"
#include "msp_registers.h"
#include "prcmu_registers.h"
#include "dma_registers.h"
#include "audiocodec.h"
#include "inc/archi-wrapper.h"

#define TIME_INCREMENT (1000)
#define NB_SLOTS_IN_JITTER  (JITTERBUFFER_TX_MS)
// store if component has been configured
static int mIsConfigured    = 0;
// store nb frame send to AB8500. We need to maintain this information for burst transition
static t_uint16 mFrameCount = 0;

static t_uint48 timestamp   = 20000;
// Start/Stop Tx Dma transfer only if not yet started
static int mStartCount      = 0;
// Limit nb Msp channel, default is 8, possible value is 2, 4, 8
static int mNbMspChannelEnabled = 0;

static int mTarget = 0;

//-------------------------
// Pointer on HW register
static volatile MspConfig EXTMMIO * mspRegTx = (volatile EXTMMIO MspConfig *) MSP1_MMDSP_BASE_ADDR;
static volatile MspConfig EXTMMIO * mspRegRx = (volatile EXTMMIO MspConfig *) MSP3_MMDSP_BASE_ADDR;

//---------------------
// Default Tx buffer
// This buffer used to play 0 during transition. Note that tx is always started before rx
static int *         mTxBuffer;
static unsigned int  mTxBufferSize;
static unsigned long mTxBufferArmAddr;

static long lcpa = 0;

static inline void resetDefaultTxBuffer(void){
    int i;
    for(i = 0; i < mTxBufferSize * NB_SLOTS_IN_JITTER; i++){
        mTxBuffer[i] = 0;
    }
}

//---------------------------------
// Transfer parameter management 
//---------------------------------
static volatile long EXTMMIO * param_buffer_tx = NULL;
    
//---------------------------------
// interrupt Handling
//---------------------------------
unsigned int mTxInterruptLevel = 0;
unsigned int myIndex = 0;

enum stop_running {
    STOPPED,
    RUNNING
};

void configureMspRegs(enum stop_running isRunningTx){
    // Init Rx
    mspRegRx->IMSC  |= 0x2UL;
    mspRegRx->RCF    = 0x000A03DUL;
    mspRegRx->DMACR |= 0x1UL;
    mspRegRx->MCR   |= 0x1UL;
    mspRegRx->RCE0   = 0xFFUL;
    mspRegRx->GCR = 0xC0022UL;
    mspRegRx->IODLY = 0x00020UL;
    mspRegRx->WMRK   = 0x2UL;
    // Init Tx
    mspRegTx->IMSC  |= 0x20UL;
    mspRegTx->TCF    = 0x0000A03DUL;
    mspRegTx->DMACR |= 0x2UL;
    mspRegTx->MCR   |= 0x20UL;
    mspRegTx->TCE0   = 0xFFUL;
    mspRegTx->IODLY = 0x00020UL;
    mspRegTx->GCR = 0xC2200UL;
    mspRegTx->WMRK  = 0x10UL;

    if(isRunningTx == RUNNING){
        mspRegTx->GCR   |= 0x100UL;
    }

    // Due to ER 400572
    if (1 == mTarget) {
        mspRegTx->IODLY = 0x0UL;
        mspRegTx->GCR &= ~0x2000UL;
    }
}

t_uint32* get_timestamp_address(void) {
    return &timestamp;
}

t_uint16 msp_dma_getIndex() {
    return myIndex;
}

// Default Tx interrupt handler used as "dummy" Tx to maintain framecount
void DefaultItTx(void){
    mFrameCount=(mFrameCount+6) & 0xFF;
    timestamp += TIME_INCREMENT;
}

// current Tx interrupt handler, will be overrided by dmaout.
static void (*mItHandlerTx)(void)        = DefaultItTx ;


void METH(IT29handler)(void) {
    myIndex = (myIndex + 1) % NB_SLOTS_IN_JITTER;
    mItHandlerTx();
}

// returns current Frame Count
// should be called inside MASK/UNMASK IT
#pragma lock
t_uint16 overrideTxInterruptHandler(void * handler){
    mItHandlerTx = handler;
    return mFrameCount; 
}



void releaseTxInterruptHandler(t_uint16 frame_count){
    AUDIO_MASK_IT(mTxInterruptLevel);
    mItHandlerTx = DefaultItTx;
    mFrameCount  = frame_count;
    AUDIO_UNMASK_IT(mTxInterruptLevel);
    resetDefaultTxBuffer();   
}

//--------------------------------------
// Start/Stop DMA/MSP Transfer
//--------------------------------------
void startFirstTx(void){
    //low_signal.normalMode();

    mFrameCount=(mFrameCount+6) & 0xFF;
    startFirstTxLliTransfer(mTxBufferSize, mTxBufferArmAddr, MSP_AB8500_TX, AB8500_BURSTSIZE);

    while(mspRegTx->FLR & 0x10UL); // wait not Tx Fifo Empty
    // TODO: update when new PRCMUFW allow us to write directly to AB8500 
    bitclock.enable(TRUE);
}

//-----------------------------------
// Start/Stop/configure this singleton
//-----------------------------------
void METH(setParameter)(void * bufferTx, t_uint16 bufferSizeInSample, t_uint16 nbMspChannelEnabled,
    t_uint16 target) {

    mNbMspChannelEnabled = nbMspChannelEnabled;

    // MAINTAIN BOTH COUNTERS ALIGNED !
    param_buffer_tx = getLogicalParameterPtr(getLogicalChannel(MSP_AB8500_TX, MSP_TX));

    mTxBuffer        = bufferTx;
    mTxBufferSize    = 48 * nbMspChannelEnabled;
    mTxBufferArmAddr = getArmPhysicalAddr(mTxBuffer);

    load_prcmu_pointers(target);
    mTarget = target;
    configureMspRegs(STOPPED);
    resetDefaultTxBuffer();

    // Configure DMA common stuff
    configureDMA();
    mIsConfigured = 1;
}

//called with first sink and only and with first source only
void start_DMA_MSP(void){
    if(mStartCount++) {
        return;
    }

    ASSERT(mIsConfigured);
#ifdef DEBUG_TRACE 
    OstTraceInt0(TRACE_DEBUG, "msp_dma_ctrl_ab8500: start_DMA_MSP do it");
#endif

    // Init 
    mItHandlerTx            = DefaultItTx;
    mFrameCount             = 0;
    timestamp               = 20000;
    
    // This has been added because sometime previous transfer was not properly stopped
    // (Logical channel still waiting burstreq from Msp )
    stopTxTransfer(MSP_AB8500_TX);   

    // Start Tx every time, even if only capturer is started (need to maintain frame count for normal to burst transition)
    startFirstTx();
    AUDIO_UNMASK_IT(30);
}

// called with last sink or last source only
void stop_DMA_MSP(void){

    if(--mStartCount){
        return;
    }
#ifdef DEBUG_TRACE 
    OstTraceInt0(TRACE_DEBUG, "msp_dma_ctrl_ab8500: stop_DMA_MSP do it");   
#endif
    AUDIO_MASK_IT(30);
    stopTxTransfer(MSP_AB8500_TX);

    // Waiting till Transmit shift register and fifo is empty
    while(mspRegTx->FLR & 0x8UL);

    // TODO: update when new PRCMUFW allow us to write directly to AB8500 
    bitclock.enable(FALSE);
}

//------------------------------
// Msp stuff 
//------------------------------
void setMspChannelEnableTx(int mask, int nb_slot){
    /* ASSERT(nb_slot == mNbMspChannelEnabled);*/
    mspRegTx->TCE0 = mask; 
}

void setMspChannelEnableRx(int mask, int nb_slot){
    /* ASSERT(nb_slot == mNbMspChannelEnabled); */
    mspRegRx->RCE0 = mask; 
}

void remapInterrupts(void){
    //--------------------------------------
    //   Interrupt remapping
    //--------------------------------------
    // Remap Msp1 Tc interrupt
    // Async_IT[12] = mmdsp_IT[38] =  MSP1_TX_TC
    // Async_IT[5]  = mmdsp_IT[4]  =  MSP1_RX_TC 
    ITREMAP_interf_reg30 = 16; 
    ITREMAP_interf_reg29 = 38; 
    ITREMAP_interf_reg27 =  4;
    ITREMAP_interf_reg15 = 34;
}

void METH(start)(void) {
    resetMSP(MSP_AB8500_TX);
    
    configureMspRegs(STOPPED);
    remapInterrupts();

    // Get Interrupt Level
    mTxInterruptLevel = AUDIO_MSP_GetInterruptLevel(MSP_AB8500_TX, MSP_TX);
    AUDIO_DMA_SetRealTime(getLogicalChannel(MSP_AB8500_TX, MSP_TX), DMA_REALTIME);
    AUDIO_DMA_SetRealTime(getLogicalChannel(MSP_AB8500_RX, MSP_RX), DMA_REALTIME);
}

void configureMsp1Gpios(void){
    // Set gpios [33-36] to alternate A (AFS/BA=01)   
    gpioReg->AFSLB &= ~(0xFUL << 1); 
    gpioReg->AFSLA |=  (0xFUL << 1); 
    
    // Maintain Alternate even if IOFORCE=1
    gpioReg->SLPM |=  (0xFUL << 1);
}

void releaseMsp1Gpios(void){
  // Set gpios [34..35] to not alternate (AFS/BA=00)  
   gpioReg->AFSLB &= ~(0x6UL << 1); 
   gpioReg->AFSLA &= ~(0x6UL << 1);

  // Set gpios [34..35] to Input (DIR = 0)
   gpioReg->DIRC =  (0x6UL << 1);

  // pull-up/down disabled  GPIO34 and GPIO35
   gpioReg->PDIS |= (0x6UL << 1);
}

void wake_up_save_configuration(void){
    if (!lcpa)
        lcpa = dmaReg->LCPA;
    releaseMsp1Gpios();
}

void wake_up_reconfiguration(void){
   long pck_bitmask = 0x200 /*gpio*/;
    long kck_bitmask = 0x10 /*Msp1*/;
   
    // Reset de la MSP1
    prcc1Reg->SOFTRST_CLR = 0x10;
    prcc1Reg->SOFTRST_SET = 0x10;
    prcc1Reg->PCKEN |= pck_bitmask;
    configureMsp1Gpios();
            
    //restore PRCC periph1 for Msp1 and Gpio
    pck_bitmask = 0x10 /*Msp1*/ |  0x200 /*gpio*/;
    prcc1Reg->PCKEN |= pck_bitmask;
    prcc1Reg->KCKEN |= kck_bitmask;
    // Sometime writing PRCC register doen t work ! (see on V1 with gpio on ulp clock)
    // ASSERT((prcc1Reg->PCKSR  & pck_bitmask) == pck_bitmask);
    // ASSERT((prcc1Reg->PKCKSR & kck_bitmask) == kck_bitmask);

    // DMA restore global configuration (ralated to PHY2) 
    // Warning: This can be an issue of coherency with CA9, moreover I enable all clocks
    dmaReg->GCC      = 0xFF03UL; 
    dmaReg->PRTYP   |= ~ (0x2UL << 2); 
    dmaReg->PRSM[0] |=   (0x2UL << 2);
    dmaReg->PRMO[0] |=   (0x2UL << 2);
    if (lcpa)
        dmaReg->LCPA = lcpa;

    AUDIO_DMA_SetChannel_Logical(DMA_PHY2);

    configureMspRegs(RUNNING);
    remapInterrupts();
   
    AUDIO_UNMASK_IT(PRCMU_FW_IT); 
    AUDIO_UNMASK_IT(mTxInterruptLevel);
}

//=====================================
// PRCMU
//=====================================

static void (*mITAB8500_FifoWaterMarkHandler)(void)  = NULL ;

//! if NULL, just ignore it fifo
void switchAb8500_FifoWaterMarkHandler(void * handler){
    mITAB8500_FifoWaterMarkHandler = handler;
}

void METH(IT15handler)(void) {
    unsigned int it_status = * PRCMUFW_generate_sia_It; //same register used for IT generation and status

    // Acknoledge IT
    * PRCMUFW_clear_sia_It = 1;

    if(it_status ==  0x1 ){
        if(mITAB8500_FifoWaterMarkHandler != NULL){
            mITAB8500_FifoWaterMarkHandler();
        }
    }
    else if(it_status ==  0x2 ){
        // I2c access acknoledge
    }
#ifdef DEBUG_TRACE
    OstTraceInt0(TRACE_DEBUG, "PRCMU: IT15handler");
#endif
}

void METH(stop)(void) {
  // stop any dma transfer if any
  if (mStartCount) {
      volatile char digif3;
      int i2c_status = 0;

      stopTxTransfer(MSP_AB8500_TX);
      bitclock.enable(FALSE);

      // switch off IF0 to reset the frame counter
      // if Asoc does not swicth off ab within the
      // restart time
      i2c_status = readAb8500RegThroughPrcmu(0xd1B, &digif3);
      if (!i2c_status){
          i2c_status = writeAb8500RegThroughPrcmu(0xd1d,(digif3 & 0xF7));
      }
      i2c_status = readAb8500RegThroughPrcmu(0xd6B, &digif3);
      if (!i2c_status){
          i2c_status = writeAb8500RegThroughPrcmu(0xd6B,(digif3 & 0xFE));
      }
  }
}

