/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   dmaout_bt.c
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
  #include "audio_audiocodec_mpc_dmaout_bt_src_dmaout_btTraces.h"
#endif

#include <audiocodec/mpc/dmaout_bt.nmf>
#include "dma_common.h"

#define TOTAL_NB_PORTS              2
#define NB_INPUT_PORTS              1

#define INPUT_PORT_INDEX            0
#define OUTPUT_PORT_INDEX           1


//#define DEBUG_TRACE     1


//=============================================================================
//                  Global Variables
//=============================================================================
portStatus_t  mMasterPortBuffers;
portStatus_t  mFeedbackPortBuffers;

//-----------------------------------------------------------------
// Internal states variables
bool        mIsRunning            = false;       //!< indicates if transfer is hapenning
static bool mPendingEvent         = false;       //!< is a process scheduled on input port ?
static bool mPendingFeedbackEvent = false;       //!< is a process scheduled on output port ?

//--------------------------------------------------------------
// HW related variable
static volatile long EXTMMIO * mDmaParam_buffer     = NULL;
static t_uint16                mDmaEndOfTranferIT   = 0;     //!< End of Dma transfer interrupt level
static bool                    mRequestSent         = false; //!< indicates if transfer have started      
static t_uint16                mSamplerate          = 0;
static t_uint16                mBlockSize           = 0;

//-----------------------------------------------------------------
// Buffers
doubleBuffer_t mDoubleBuffer;          //!< Ping Pong buffer used in normal mode
BufferFlag_e   memo_flags;             //!< memorize input port buffer flags to restore them in output port
bool           mIsMasterPortEnabled;   //!< indicates if master port is enabled      
bool           mIsFeedbackPortEnabled; //!< indicates if reference port is enabled 

//-------------------------------------------------
// Sampled Played Counter for audio/video synchro
t_uint48 * mSamplesPlayedPtr = 0;
t_uint48 mTimeStamp          = 0; 

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
    
    ASSERT(mSamplesPlayedPtr!=0);
    for (i = 0; i < NB_INPUT_PORTS; i++) {
        mSamplesPlayedPtr[i] = 0;
    }

    mDoubleBuffer.filledBuffers     = 0;
    mDoubleBuffer.writeIndex        = 0;
    mDoubleBuffer.readIndex         = 0;

    mTimeStamp                      = 0;
    mRequestSent                    = false;
   
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
    PRECONDITION(config.sample_count_buffer_size == (NB_INPUT_PORTS * sizeof(t_uint48)) );
    
		mSamplerate = config.samplerate;
    if (mSamplerate == 16000) {
        mBlockSize = BT_BLOCKSIZE_5MS_WB;  // 16 kHz
    } else {
        mBlockSize = BT_BLOCKSIZE_5MS_NB;  // 8 kHz
    }

    ASSERT(config.buffer_size       == (mBlockSize * 2)); //Ping pong buffer

    mDoubleBuffer.buffer[0]   = config.buffer;
    mDoubleBuffer.buffer[1]   = mDoubleBuffer.buffer[0] + mBlockSize;
    mDoubleBuffer.phy_addr[0] = getArmPhysicalAddr(mDoubleBuffer.buffer[0]);
    mDoubleBuffer.phy_addr[1] = getArmPhysicalAddr(mDoubleBuffer.buffer[1]);
	
    mSamplesPlayedPtr         = (t_uint48 *)config.sample_count_buffer; //!< sampled played shared memory with ARM proxy
    
    //! Get Hw configuration
    mDmaParam_buffer   = getLogicalParameterPtr(getLogicalChannel(MSP0 , MSP_TX));
    mDmaEndOfTranferIT = AUDIO_MSP_GetInterruptLevel(MSP0, MSP_TX);

    mMasterPortBuffers.isRunning         = false;
    mMasterPortBuffers.weHaveTheBuffer   = false;
    mMasterPortBuffers.buffer            = NULL;
    mFeedbackPortBuffers.isRunning       = false;
    mFeedbackPortBuffers.weHaveTheBuffer = false;
    mFeedbackPortBuffers.buffer          = NULL;    

#ifdef DEBUG_TRACE      
    OstTraceInt1(TRACE_DEBUG, "dmaout_bt: config - samplerate   = %d", config.samplerate);
    OstTraceInt1(TRACE_DEBUG, "dmaout_bt: config - buffer_size  = %d", config.buffer_size);
#endif 
}

//=============================================================================
//                Start / Stop Dma Interface
//=============================================================================

//-----------------------------------------------------------------------------
//! \brief Start DMA transfer proxy interface
//!
//! This is called by different OMX sinks when they goes in executing state.
//! First sinks that goes in executing starts effectivelly DMA tranfer. Other
//! calls are ignored.
//!
//-----------------------------------------------------------------------------
void startDmaIfNeeded(void) {
#ifdef DEBUG_TRACE     
    OstTraceInt0(TRACE_DEBUG, "dmaout_bt: startDma entry");
#endif 
    if(mIsRunning){
        return;
    }
#ifdef DEBUG_TRACE 
    OstTraceInt0(TRACE_DEBUG, "dmaout_bt: startDma switching to executing state");
#endif    
    reset();
    
    start_DMA_MSP(mSamplerate);

    mIsRunning = true;
}

//-----------------------------------------------------------------------------
//! \brief Stop DMA transfer proxy interface
//!
//! This is called by different OMX sinks when they goes in idle state. First 
//! sinks that goes in idle stops effectivelly DMA tranfer. Other calls are
//! ignored.
//!
//-----------------------------------------------------------------------------
void stopDmaIfNeeded(void) {
#ifdef DEBUG_TRACE      
    OstTraceInt0(TRACE_DEBUG, "dmaout_bt: stopDma entry");
#endif 
    if(!mIsRunning){
        return;
    }
#ifdef DEBUG_TRACE  
    OstTraceInt0(TRACE_DEBUG, "dmaout_bt: stopDma switching to idle state");
#endif
    mIsRunning = false;

    stopTxTransfer(MSP0);
}

//=============================================================================
//                Buffers Handling
//=============================================================================

//-----------------------------------------------------------------------------
//! \brief Toggle double buffer read index and decrement filled buffers
//!
//! This function must only be called inside interrupt when a DMA transfer
//! is finished
//!
//-----------------------------------------------------------------------------
static inline void toggleDoubleRead(void){
    // TODO factorize with dmoaut
    if(mDoubleBuffer.filledBuffers > 0){
        // A buffer has been sent
        mDoubleBuffer.filledBuffers--;
        mDoubleBuffer.readIndex = 1 - mDoubleBuffer.readIndex;
    } else{
#ifdef DEBUG_TRACE
        OstTraceInt0(TRACE_DEBUG, "dmaout_bt: ERROR nbfilledBuffer == 0 in DMA end of transfer IT");
        //TODO : what happens if buffer shortage ??
        //This happed some time in low power because process is not called ... very strange ASSERT(0);
#endif
    }
}

static inline void fillBufferWithZeros(void) {
    int i;
    int *bufout = mDoubleBuffer.buffer[mDoubleBuffer.writeIndex] ;

    for (i = 0; i < mBlockSize; i++) {
        bufout[i] = 0;
    }
}

volatile int wp_readback;
static inline void expandToDoubleBuffer(int * buf) {
    int i;
    int *bufout;
    bufout = mDoubleBuffer.buffer[mDoubleBuffer.writeIndex];
    for (i = 0; i < mBlockSize; i++) {
        bufout[i] = buf[i] >> 8;
    }
    wp_readback = bufout[(mBlockSize-1)];
}

static inline int copyFromDoubleBufferToFeedback(int * feedbackbuf) {
    int i;
    int * bufout = mDoubleBuffer.buffer[mDoubleBuffer.readIndex];

    for (i = 0; i < mBlockSize; i++) {
        feedbackbuf[i] = bufout[i] << 8;
    }
    return mBlockSize;
}

static inline Buffer_p getOutputBuffer() {
#ifdef DEBUG_TRACE    
    OstTraceInt1(TRACE_DEBUG, "dmaout_bt:getOutputBuffer weHaveTheBuffer=%d", mFeedbackPortBuffers.weHaveTheBuffer);
#endif
    if (mFeedbackPortBuffers.weHaveTheBuffer == false) {
        return 0;
    }
    return mFeedbackPortBuffers.buffer;
}

void returnFeedbackPort(void) {
 
#ifdef DEBUG_TRACE    
    OstTraceInt0(TRACE_DEBUG, "dmaout_bt_buffer: returnFeedbackPort");
#endif

    outputport.emptyThisBuffer(mFeedbackPortBuffers.buffer);
   
    mFeedbackPortBuffers.weHaveTheBuffer = false;
    mFeedbackPortBuffers.buffer          = NULL;
}

void processFeedbackBuffer(){
    Buffer_p buf = getOutputBuffer();
#ifdef DEBUG_TRACE_BUFFERS
    OstTraceInt1(TRACE_DEBUG, "dmaout_bt:processFeedbackBuffer buf=0x%x",buf);
#endif

    if(buf) {
        buf->filledLen = copyFromDoubleBufferToFeedback(buf->data);
        buf->nTimeStampl = mTimeStamp & 0xFFFFFFU;
        buf->nTimeStamph = mTimeStamp & 0xFFFFFFU;
        buf->flags       = memo_flags & ~BUFFERFLAG_EOS;
        returnFeedbackPort();
    }
}


Buffer_p getInputBuffer() {
    Buffer_p buf;
#ifdef DEBUG_TRACE    
    OstTraceInt1(TRACE_DEBUG, "dmaout_bt:getInputBuffer weHaveTheBuffer=%d",mMasterPortBuffers.weHaveTheBuffer);
#endif

    if (mMasterPortBuffers.weHaveTheBuffer == false) {
        return 0;
    }

    buf = mMasterPortBuffers.buffer;

#ifdef DEBUG_TRACE    
    OstTraceInt2(TRACE_DEBUG, "dmaout_bt:getInputBuffer filledLen=%d allocLen=%d",buf->filledLen , buf->allocLen);
#endif

    if (buf->flags & BUFFERFLAG_EOS) {
#ifdef DEBUG_TRACE        
        OstTraceInt0(TRACE_DEBUG, "dmaout_bt:getInputBuffer:EOS_FOUND");
#endif
        {
            int i;
            for (i = buf->filledLen; i < buf->allocLen; i++) {
                buf->data[i] = 0;
            }
            return buf;
        }
    }
    if (buf->filledLen == 0) {
        // FIXME need to understand why some buffers without EOS have filledLen == 0
        return 0; 
    }
    return buf;
}

void returnMasterPort(void) {
 
    ASSERT(mSamplesPlayedPtr!=0);
	// update Samples Played memory zone
    mSamplesPlayedPtr[INPUT_PORT_INDEX] ++;

#ifdef DEBUG_TRACE    
    OstTraceInt0(TRACE_DEBUG, "dmaout_bt_buffer: returnMasterPort");
#endif

    inputport.fillThisBuffer(mMasterPortBuffers.buffer);
  
    mMasterPortBuffers.weHaveTheBuffer = false;
    mMasterPortBuffers.buffer          = NULL;
}


static void expandInputBuffers(void) {
    Buffer_p    buf;
    int i;
   static int isok = 0;

    fillBufferWithZeros();
    
    buf = getInputBuffer();
    if (buf) {
        expandToDoubleBuffer(buf->data);
        returnMasterPort();
        memo_flags = buf->flags;
        isok = 1;
    }

    if(isok){
        MASK_EOT_IT;
        mDoubleBuffer.filledBuffers++;
        UNMASK_EOT_IT;
        mDoubleBuffer.writeIndex = 1 - mDoubleBuffer.writeIndex;
    }
}

//=============================================================================
//                Start Dma Transfer
//=============================================================================
static inline void RestartTranfer (int nb_element){
    startTx(nb_element, mDoubleBuffer.phy_addr[mDoubleBuffer.readIndex], MSP_0_REG_BASE_ADDR, DMA_MSP0_EE_OFFSET, mDmaParam_buffer, DMA_BURST_4);
    mTimeStamp += mBlockSize >> 3 ; //TODO: check if we must count in FRAMECOUNT_INCREMENT = 6 for one ms
}

static inline SendRequest(unsigned int nb_element) {
    startFirstTxTransfer(nb_element, mDoubleBuffer.phy_addr[mDoubleBuffer.readIndex], MSP0, DMA_BURST_4);
    processFeedbackBuffer();
    mRequestSent = true;
}


//=============================================================================
//                             PROCESSES
//=============================================================================
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

void METH(IT28handler)(void) {
    if (mIsRunning) {
        toggleDoubleRead();
        // relaunch the transfer
        RestartTranfer(mBlockSize);
    } 
    scheduleProcessEvent();
    if(mPendingFeedbackEvent == false){
        mPendingFeedbackEvent = true;
        feedback_me.processEvent();
    }
#ifdef DEBUG_TRACE
     OstTraceInt0(TRACE_DEBUG, "dmaout_bt: it 28 handler");
#endif
}

//=============================================================================
//                      BUFFERS 
//=============================================================================
void METH(feedback_processEvent)(void){
    mPendingFeedbackEvent = false;    
    processFeedbackBuffer();
}

void METH(fillThisBuffer)(Buffer_p buffer) {   
#ifdef DEBUG_TRACE  
    OstTraceInt1(TRACE_DEBUG, "dmaout_bt:fillThisBuffer buffer->allocLen=%d", buffer->allocLen);
#endif   
    mFeedbackPortBuffers.weHaveTheBuffer = true;
    mFeedbackPortBuffers.buffer          = buffer;

    if(! mFeedbackPortBuffers.isRunning){
        returnFeedbackPort();
    }
}

void METH(emptyThisBuffer)(Buffer_p buffer) {
    PRECONDITION(buffer->filledLen == buffer->allocLen 
            || (buffer->flags & BUFFERFLAG_EOS) 
            || buffer->filledLen == 0);

#ifdef DEBUG_TRACE     
    OstTraceInt1(TRACE_DEBUG, "dmaout_bt:emptyThisBuffer buffer->allocLen=%d", buffer->allocLen);
#endif   
    
    mMasterPortBuffers.weHaveTheBuffer = true;
    mMasterPortBuffers.buffer          = buffer;

    if(mMasterPortBuffers.isRunning){
        scheduleProcessEvent();
    }
    else{
        returnMasterPort();
    }
}

void METH(enablePort)() {
#ifdef DEBUG_TRACE    
    OstTraceInt0(TRACE_DEBUG, "dmaout_bt: enablePort");
#endif  
    mMasterPortBuffers.isRunning = true;
   
    startDmaIfNeeded();

    dma_port_state.portIsRunning();
}

void METH(disablePort)() {
#ifdef DEBUG_TRACE     
    OstTraceInt0(TRACE_DEBUG, "dmaout_bt: disablePort");
#endif 
    mMasterPortBuffers.isRunning = false;

    if(mMasterPortBuffers.weHaveTheBuffer){
        returnMasterPort();
    }

    stopDmaIfNeeded();
    //TODO Maybe we need to wait last TC interrupt
    dma_port_state.portIsStopped();  
}

void METH(feedback_enablePort)() {
    mFeedbackPortBuffers.isRunning = true;
}

void METH(feedback_disablePort)() {
    mFeedbackPortBuffers.isRunning = false;

    if(mFeedbackPortBuffers.weHaveTheBuffer){
        returnFeedbackPort();
    }
}

void METH(processEvent)() {
    mPendingEvent = false;
    if (mDoubleBuffer.filledBuffers < 2){
        expandInputBuffers();
    }

    // Check if we need to wait for state executing
    if ( (!mRequestSent) && mDoubleBuffer.filledBuffers == 2) {    
        SendRequest(mBlockSize);
    }
}

//TODO: to be removed
void METH(newFormat)(t_sample_freq sample_freq, t_uint16 chans_nb, t_uint16 sample_size) {
    //Nothing to do
}

void METH(stop)(void) {
    // stop any dma transfer if any
    if(mIsRunning){
        stopTxTransfer(MSP0);
    }
}

