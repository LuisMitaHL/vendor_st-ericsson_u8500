/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   dmain_bt.c
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
  #include "audio_audiocodec_mpc_dmain_bt_src_dmain_btTraces.h"
#endif

#include <audiocodec/mpc/dmain_bt.nmf>
#include "dma_common.h"

#define INPUT_PORT_INDEX      0
#define TOTAL_NB_PORTS        1

//=============================================================================
//                  Global Variables
//=============================================================================
portStatus_t  mMasterPortBuffers;

//-----------------------------------------------------------------
// Internal states variables
bool         mIsRunning    = false;       //!< indicates if transfer is hapenning
static bool  mPendingEvent = false;       //!< is a process scheduled on input port ?

//--------------------------------------------------------------
// HW related variable
static volatile long EXTMMIO * mDmaParam_buffer     = NULL;
static t_uint16                mDmaEndOfTranferIT   = 0;    //!< End of Dma transfer interrupt level
static t_uint16                mSamplerate          = 0;
static t_uint16                mBlockSize           = 0;

//-----------------------------------------------------------------
// Buffers
doubleBuffer_t mDoubleBuffer; //!< Ping Pong buffer used in normal mode

//-------------------------------------------------
// Sampled Played Counter for audio/video synchro
t_uint48 * mSamplesPlayedPtr = 0;

//=============================================================================
//               Reset / Init Function
//=============================================================================

//-----------------------------------------------------------------------------
//! \brief  Global variables initialization
//!
//! Reset variables used by PingPong buffer.
//! Must be called before starting first dma transfer
//!
//-----------------------------------------------------------------------------
static void reset(void) {
    int i;
    
    // TODO factorize with dmain
    mDoubleBuffer.filledBuffers = 0;
    mDoubleBuffer.writeIndex    = 0;
    mDoubleBuffer.readIndex     = 0;
    
    ASSERT(mSamplesPlayedPtr!=0);
    for (i = 0; i < TOTAL_NB_PORTS; i++) {
        mSamplesPlayedPtr[i] = 0;
    }

}

//=============================================================================
//                  Configuration
//=============================================================================
//-----------------------------------------------------------------------------
//! \brief Set parameter 
//!
//! Get configuration from proxy. Buffers, Dma configuration  
//! It also retrieve HW information and initialize this
//! NMF component.
//! This function is called by the proxy just after instantiation, and before
//! going in executing state.
//!
//! \param config configuration given by proxy
//!
//-----------------------------------------------------------------------------
void METH(setParameter)(DmaConfig_t config,t_uint16 target) {
    PRECONDITION(config.sample_count_buffer_size == (TOTAL_NB_PORTS * sizeof(t_uint48)) );
    
		mSamplerate = config.samplerate;
    if (mSamplerate == 16000) {
        mBlockSize = BT_BLOCKSIZE_5MS_WB;   // 16 kHz
    } else {
        mBlockSize = BT_BLOCKSIZE_5MS_NB;   // 8 kHz
    }

    //ASSERT(config.buffer_size       == (mBlockSize * 2)); //Ping pong buffer

    mDoubleBuffer.buffer[0]   = config.buffer;
    mDoubleBuffer.buffer[1]   = mDoubleBuffer.buffer[0] + mBlockSize;
    mDoubleBuffer.phy_addr[0] = getArmPhysicalAddr(mDoubleBuffer.buffer[0]);
    mDoubleBuffer.phy_addr[1] = getArmPhysicalAddr(mDoubleBuffer.buffer[1]);
    
    mSamplesPlayedPtr         = (t_uint48 *)config.sample_count_buffer; //!< sampled played shared memory with ARM proxy

    //! Get Hw configuration
    mDmaParam_buffer   = getLogicalParameterPtr(getLogicalChannel(MSP0 , MSP_RX));
    mDmaEndOfTranferIT = AUDIO_MSP_GetInterruptLevel(MSP0, MSP_RX);
   
    mMasterPortBuffers.isRunning       = false;
    mMasterPortBuffers.weHaveTheBuffer = false;
    mMasterPortBuffers.buffer          = NULL;

#ifdef DEBUG_TRACE      
    OstTraceInt1(TRACE_DEBUG, "dmain_bt: config - samplerate   = %d", config.samplerate);
    OstTraceInt1(TRACE_DEBUG, "dmain_bt: config - buffer_size  = %d", config.buffer_size);
#endif 
}

//=============================================================================
//                Buffers Handling
//=============================================================================
void returnMasterPort(void) {
 
#ifdef DEBUG_TRACE    
    OstTraceInt0(TRACE_DEBUG, "dmain: returnMasterPort");
#endif

    outputport.emptyThisBuffer(mMasterPortBuffers.buffer);
   
    mMasterPortBuffers.weHaveTheBuffer = false;
    mMasterPortBuffers.buffer          = NULL;
}

//-----------------------------------------------------------------------------
//! \brief Copy data to output buffer
//!
//! This function copy data for one accessory and returns associated buffer
//!
//-----------------------------------------------------------------------------
static void CopyDoubleBufferToOutput(Buffer_p outputbuf) {
    int i;
    int * buf = mDoubleBuffer.buffer[mDoubleBuffer.writeIndex];

    for (i=0; i<mBlockSize; i++) {
        outputbuf->data[i] = buf[i] << 8;
    }
    outputbuf->filledLen = outputbuf->allocLen;
    // TODO: we do not have shared time base between BT source and sink, is it an issue ?
    outputbuf->nTimeStampl    =  mSamplesPlayedPtr[0]      & 0xFFFFFFU;
    outputbuf->nTimeStamph    = (mSamplesPlayedPtr[0]>>24) & 0xFFFFFFU;
    outputbuf->byteInLastWord = 2;
    outputbuf->flags          |= BUFFERFLAG_STARTTIME;
}


//-----------------------------------------------------------------------------
//! \brief Returns buffer on one port, null if no buffer in port
//!
//-----------------------------------------------------------------------------
Buffer_p getOutputBuffer() {
#ifdef DEBUG_TRACE
    OstTraceInt1(TRACE_DEBUG, "dmain:getOutputBuffer weHaveTheBuffer=%d", mMasterPortBuffers.weHaveTheBuffer);
#endif
    if (mMasterPortBuffers.weHaveTheBuffer == false) {
        return 0;
    }
    return mMasterPortBuffers.buffer;
}

//-----------------------------------------------------------------------------
//! \brief Extract buffers
//!
//! Iterates on each ports, copy data into buffer if possible and return buffers.
//! Ping Pong buffer index/filled state are updated.
//!
//-----------------------------------------------------------------------------
static void extractOutputBuffers(void) {
    Buffer_p    buf;
    int i;

    buf = getOutputBuffer();
    if (buf) {
        MMDSP_FLUSH_DCACHE();
        CopyDoubleBufferToOutput(buf);
        returnMasterPort();
    }

    MASK_EOT_IT;
    mDoubleBuffer.filledBuffers--;
    UNMASK_EOT_IT;

    mDoubleBuffer.writeIndex = 1 - mDoubleBuffer.writeIndex;
}

//=============================================================================
//                Start / Stop Dma Interface
//=============================================================================

//-----------------------------------------------------------------------------
//! \brief Start DMA transfer proxy interface
//!
//! This is called by different OMX sources when they goes in executing state.
//! First sources that goes in executing starts effectivelly DMA tranfer. Other
//! calls are ignored.
//!
//-----------------------------------------------------------------------------
void startDmaIfNeeded(void) {
    // TODO factorize with dmain
    int i;
    unsigned long arm_buffer;
#ifdef DEBUG_TRACE  
    OstTraceInt0(TRACE_DEBUG, "dmain_bt: startDma entry");
#endif     
    if(mIsRunning){
        return;
    }
#ifdef DEBUG_TRACE 
    OstTraceInt0(TRACE_DEBUG, "dmain_bt: startDma switching to executing state");
#endif     
    mIsRunning = true;
    reset();

    //TODO: remove this in w44
    // Propagate pcmsetting to all connected input
    if(! IS_NULL_INTERFACE(outputsettings, newFormat)){
        if (mSamplerate == 16000) {
            outputsettings.newFormat(FREQ_16KHZ, NB_CHANNEL_MONO, 24);
        } else {
            outputsettings.newFormat(FREQ_8KHZ, NB_CHANNEL_MONO, 24);
        }
    }

    
    start_DMA_MSP(mSamplerate);
   
    startFirstRxTransfer(mBlockSize, mDoubleBuffer.phy_addr[mDoubleBuffer.readIndex], MSP0, DMA_BURST_4);
}

//-----------------------------------------------------------------------------
//! \brief Stop DMA transfer proxy interface
//!
//! This is called by different OMX sources when they goes in idle state. First 
//! sources that goes in idle stops effectivelly DMA tranfer. Other calls are
//! ignored.
//!
//-----------------------------------------------------------------------------
void stopDmaIfNeeded(void) {
    // TODO factorize with dmain
#ifdef DEBUG_TRACE 
    OstTraceInt0(TRACE_DEBUG, "dmain_bt: stopDmaIfNeeded entry");
#endif 
    if(! mIsRunning){
        return;
    }
#ifdef DEBUG_TRACE 
    OstTraceInt0(TRACE_DEBUG, "dmain_bt: stopDmaIfNeeded switching to idle state");
#endif     
    mIsRunning = false;

    stopRxTransfer(MSP0);
    
    while(mDoubleBuffer.filledBuffers){    
        extractOutputBuffers();
    }
}

//=============================================================================
//                             PROCESSES
//=============================================================================

//-----------------------------------------------------------------------------
//! \brief End Of Tranfer Interrupt handler
//!
//! Restart a DMA transfer and update.
//! Ping Pong buffer index/filled state are updated.
//! SampledPlayedCounter is updated.
//! Note that for record SampledPlayed counter represent "real time" not "buffer traffic"
//!
//-----------------------------------------------------------------------------
void METH(IT26handler)(void) {
    // TODO factorize with dmain

    //-------------------------    
    // relaunch the transfer
    mDoubleBuffer.readIndex = 1 - mDoubleBuffer.readIndex;

    startRx(mBlockSize, mDoubleBuffer.phy_addr[mDoubleBuffer.readIndex],
            MSP_0_REG_BASE_ADDR, DMA_MSP0_EE_OFFSET, mDmaParam_buffer, DMA_BURST_4);
    
    if(mDoubleBuffer.filledBuffers <2){
        mDoubleBuffer.filledBuffers++;
    }

	// count how many buffers are received to get reference time for A/V sync (in ms)
    mSamplesPlayedPtr[0] += BT_GRAIN_IN_MS;  	

    scheduleProcessEvent();
}

//=============================================================================
//                      BUFFERS 
//=============================================================================
void METH(fillThisBuffer)(Buffer_p buffer) {   
    mMasterPortBuffers.weHaveTheBuffer = true;
    mMasterPortBuffers.buffer          = buffer;

    if(mMasterPortBuffers.isRunning){
        scheduleProcessEvent();
    }
    else{
        returnMasterPort();
    }
}

void scheduleProcessEvent(void){
    bool need_process = false;

    ENTER_CRITICAL_SECTION;
    if(mPendingEvent == false){
        mPendingEvent = true;
        need_process  = true;
    }
    EXIT_CRITICAL_SECTION;

    if(need_process){
        me.processEvent();
    }
}

//-----------------------------------------------------------------------------
//! \brief Fill buffer on each ports if possible
//!
//-----------------------------------------------------------------------------
void METH(processEvent)() {
    mPendingEvent = false;
    
    if(mDoubleBuffer.filledBuffers > 0) {
        extractOutputBuffers();
    }
}

void METH(enablePort)() {
#ifdef DEBUG_TRACE    
    OstTraceInt0(TRACE_DEBUG, "dmain_bt: enablePort");
#endif 
    mMasterPortBuffers.isRunning = true;
   
    startDmaIfNeeded();

    dma_port_state.portIsRunning();
}

void METH(disablePort)() {
#ifdef DEBUG_TRACE    
    OstTraceInt0(TRACE_DEBUG, "dmain_bt: disablePort");
#endif 
    mMasterPortBuffers.isRunning = false;
    if(mMasterPortBuffers.weHaveTheBuffer){
        returnMasterPort();
    }

    stopDmaIfNeeded();
    //TODO Maybe we need to wait last TC interrupt
    dma_port_state.portIsStopped();
}

void METH(stop)(void) {
    // stop any dma transfer if any
    if(mIsRunning){
        stopRxTransfer(MSP0);
    }
}
