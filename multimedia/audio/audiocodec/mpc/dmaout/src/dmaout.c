/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   dmaout.c
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "dmaout.h"
// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "audio_audiocodec_mpc_dmaout_src_dmaoutTraces.h"
#endif

#include <misc/xbusdma/include/xbusdma.h>
#include "dma_common.h"
#include "dma_registers.h"
#include "msp_registers.h"
#include "prcmu_registers.h"
#include "samplesplayed.idt"
#define NORMAL_TIME_INCREMENT (1000)
#define DMAOUT_JITTERBUFFER_LOW_WATERMARK (3)

//=============================================================================
//                  Global Variables
//=============================================================================
DmaOutPortInfo mAccessoryInfo[NB_INPUT_PORTS];
portStatus_t   mMasterPortBuffers[NB_INPUT_PORTS];
portStatus_t   mFeedbackPortBuffers[NB_OUTPUT_PORTS];
int JitterMargin[NB_INPUT_PORTS];

//-----------------------------------------------------------------
// Internal states variables
static bool log_xrun = true;
static bool xrun = false;
static volatile bool burst_running = false;
static bool Fifo_It_Enabled = false;
static t_uint16 CountdownToBurstSwitch = 0;
static t_uint16 SetupBurstDeviceCtr;
static t_uint16 burst_port_idx;
static volatile t_uint16 SlotsToMoveFromRing = 0;

static LowPowerState reqLowPowerState         = NORMAL_MODE;
LowPowerState actualLowPowerState             = NORMAL_MODE;
static bool          mAbandonLPATran          = false;
static bool          mIsTxRunning             = false;
static bool          mPleaseStopAsap          = false;
static bool          mPleaseDoNotStop         = false;
static bool          mPendingEvent            = false;
static bool          mPendingFeedbackEvent    = false;
t_uint16             mMemorizedPortIdxForIdle = 0;
static void (*mProcess)(void)              = NULL;

//--------------------------------------------------------------
static volatile long EXTMMIO * mDmaParam_buffer      = NULL;
t_uint16                       mNbMspChannelEnabled  = 0;     //!< Note that this must be useless on DB8500v2. On v2 only 8 channels.
t_uint16                       mDmaEndOfTranferIT    = 0;     //!< End of Dma transfer interrupt level
static t_uint16                mFrameCount           = 0;     //!< stores Actual frame count (unit is 8 samples <=> for 48 samples we need to incrment it by 6 )
static t_uint16                mSwitchFrameCount     = 0;     //!< memorize frame count that will be sent to AB8500 to indicate witch frame to switch in burst mode

//-----------------------------------------------------------------

JitterBuffer_t JitterBuffer;
ringBuffer_t   mRingBuffer;

//-------------------------------------------------
t_uint48 * mSamplesPlayedPtr = 0;
t_uint48 * mTimeStampPtr     = 0; //TODO: Can t we use same counter for both time stamp and samples played
t_uint16 mRingbufferXruns = 0;

//=============================================================================
//               Reset / Init Function
//=============================================================================

//-----------------------------------------------------------------------------
//! \brief Initialise Omx port related information
//-----------------------------------------------------------------------------
static void initPortInfo(void){
    int i;

    // assign default msp slot index
    mAccessoryInfo[AVSINK_INPUT_PORT_IDX].mspslot = AV_MSPSLOT;
    mAccessoryInfo[IHF_INPUT_PORT_IDX   ].mspslot = IHF_MSPSLOT;
    mAccessoryInfo[EAR_INPUT_PORT_IDX   ].mspslot = EAR_MSPSLOT;
    mAccessoryInfo[VIBRAL_INPUT_PORT_IDX].mspslot = VIBRAL_MSPSLOT;
    mAccessoryInfo[VIBRAR_INPUT_PORT_IDX].mspslot = VIBRAR_MSPSLOT;
    mAccessoryInfo[FMTX_INPUT_PORT_IDX  ].mspslot = FMTX_MSPSLOT;

    // assign number of channels
    mAccessoryInfo[AVSINK_INPUT_PORT_IDX].nb_channel = AVSINK_NB_CHANNEL;
    mAccessoryInfo[IHF_INPUT_PORT_IDX   ].nb_channel = IHF_NB_CHANNEL;
    mAccessoryInfo[EAR_INPUT_PORT_IDX   ].nb_channel = EAR_NB_CHANNEL;
    mAccessoryInfo[VIBRAL_INPUT_PORT_IDX].nb_channel = VIBRAL_NB_CHANNEL;
    mAccessoryInfo[VIBRAR_INPUT_PORT_IDX].nb_channel = VIBRAR_NB_CHANNEL;
    mAccessoryInfo[FMTX_INPUT_PORT_IDX  ].nb_channel = FMTX_NB_CHANNEL;

    for (i = 0; i < NB_INPUT_PORTS ; i++) {
        mAccessoryInfo[i].flags                 = 0;
    }

    for (i = 0; i < NB_INPUT_PORTS; i++) {
        mMasterPortBuffers[i].isRunning       = false;
        mMasterPortBuffers[i].weHaveTheBuffer = false;
        mMasterPortBuffers[i].muted           = false;
        mMasterPortBuffers[i].mute_cb         = false;
        mMasterPortBuffers[i].ramp_steps      = 0;
        mMasterPortBuffers[i].buffer          = NULL;
        mMasterPortBuffers[i].expandedFragments = 0;
    }
    for (i = 0; i < NB_OUTPUT_PORTS; i++) {
        mFeedbackPortBuffers[i].isRunning       = false;
        mFeedbackPortBuffers[i].weHaveTheBuffer = false;
        mFeedbackPortBuffers[i].data_droppped   = true;
        mFeedbackPortBuffers[i].buffer          = NULL;
        mFeedbackPortBuffers[i].expandedFragments = 0;
    }

}

//-----------------------------------------------------------------------------
//! \brief  Normal state variables initialization
//!
//! Reset variables used in Normal State. Must be called before going in Normal
//! state.
//!
//-----------------------------------------------------------------------------
static void resetNormalState(void){
    JitterBuffer.filledslots = 0;
    JitterBuffer.write_index = 0;
    JitterBuffer.read_index = 0;
    xrun = false;
    log_xrun = true;
}

static void fillJitterBufferWithZeros(void) {
    int *zero;
    int i;

    zero = JitterBuffer.buffer[0];
    for (i = 0; i < AB_BLOCKSIZE_MONO_1MS * mNbMspChannelEnabled * 10; i++) {
        *zero = 0;
        zero++;
    }
}

//-----------------------------------------------------------------------------
//! \brief  Burst state variables initialization
//!
//! Reset variables used in Low power State. Must be called before starting
//! transition to burst.
//!
//-----------------------------------------------------------------------------
static void resetLowPowerState(void){
    SetupBurstDeviceCtr            = 0;
    mRingbufferXruns               = 0;
    mSwitchFrameCount              = 0;
    mAbandonLPATran                = false;
    mRingBuffer.filledBuffers      = 0;
    mRingBuffer.filledSlots        = 0;
    mRingBuffer.readIndex          = 0;
    mRingBuffer.readSlotIndex      = 0;
    mRingBuffer.writeSlotIndex     = 0;
    switchAb8500_FifoWaterMarkHandler(NULL);
}

//-----------------------------------------------------------------------------
//! \brief  Global variables initialization
//!
//! Reset all variables used in Normal mode and Low power State.
//! Must be called before starting first dma transfer, before enabling bit clock
//!
//-----------------------------------------------------------------------------
static void reset(void) {
    int i;

    resetNormalState();
    resetLowPowerState();
    ASSERT(mSamplesPlayedPtr!=0);
    for (i = 0; i < NB_INPUT_PORTS; i++) {
        mSamplesPlayedPtr[i] = 0;
    }

    mTimeStampPtr = get_timestamp_address();
    reqLowPowerState  = NORMAL_MODE;
    mPleaseStopAsap = false;
}

static void computeSlotLimitation(void){
    
    int i;
    int msp_slot_en   = 0;
    
    for ( i=0; i < mNbMspChannelEnabled; i++) {
        msp_slot_en |= 1 << i;
    }
    
    setMspChannelEnableTx(msp_slot_en, mNbMspChannelEnabled);
}

//=============================================================================
//                  Configuration
//=============================================================================

//-----------------------------------------------------------------------------
//! \brief Set parameter
//!
//! Get configuration from proxy. Buffers, Dma configuration, number of
//! enabled MSP channels. It also retrieve HW information and initialize this
//! NMF component.
//! This function is called by the proxy just after instantiation, and before
//! going in executing state.
//!
//! \param config configuration given by proxy
//!
//-----------------------------------------------------------------------------
void METH(setParameter)(DmaConfig_t config, t_uint16 target) {
    unsigned int j;
    int i;
    int *zero;
    volatile long EXTMMIO * relink = NULL;

    PRECONDITION(config.buffer_size == (AB_BLOCKSIZE_MONO_1MS  * config.nb_msp_channel_enabled * JITTERBUFFER_TX_MS));
    PRECONDITION(config.sample_count_buffer_size == (NB_INPUT_PORTS * sizeof(t_uint48)) );

#ifdef DEBUG_TRACE
    OstTraceInt1(TRACE_DEBUG, "Dmaout channels: %u\n", config.nb_msp_channel_enabled);
#endif
    load_prcmu_pointers(target);

    mNbMspChannelEnabled = config.nb_msp_channel_enabled;
    mSamplesPlayedPtr = (t_uint48 *)config.sample_count_buffer;

    JitterBuffer.length = JITTERBUFFER_TX_MS;
    JitterBuffer.buffer[0] = config.buffer;
    JitterBuffer.phy_addr[0] = getArmPhysicalAddr(JitterBuffer.buffer[0]);

    for (i=1; i<JitterBuffer.length; i++) {
        JitterBuffer.buffer[i] = JitterBuffer.buffer[i-1] + AB_BLOCKSIZE_MONO_1MS * mNbMspChannelEnabled;
        JitterBuffer.phy_addr[i] = getArmPhysicalAddr( JitterBuffer.buffer[i]);
    }

    zero = JitterBuffer.buffer[0];
    for (i = 0; i < AB_BLOCKSIZE_MONO_1MS * mNbMspChannelEnabled * JITTERBUFFER_TX_MS; i++) {
        *zero = 0;
        zero++;
    }

    mDmaParam_buffer        = getLogicalParameterPtr(getLogicalChannel(MSP1 , MSP_TX));
    mDmaEndOfTranferIT      = AUDIO_MSP_GetInterruptLevel(MSP1, MSP_TX);

#ifdef DEBUG_TRACE
    OstTraceInt2(TRACE_DEBUG, "dmaout: setParameter buffer=0x%p buffer_size=%d", (int)JitterBuffer.buffer[0], config.buffer_size);
    OstTraceInt1(TRACE_DEBUG, "dmaout: setParameter mspChannelLimit=%d", mNbMspChannelEnabled);
#endif

    initPortInfo();
    mProcess               = DmaOut_Process_NormalMode_NotYetStarted;
}

//-----------------------------------------------------------------------------
//! \brief Start Low Power mode
//-----------------------------------------------------------------------------
void METH(startLowPowerTransition)(void* buffer_ring, t_uint16 buffer_ring_size) {
#ifdef DEBUG_TRACE
    OstTraceInt2(TRACE_DEBUG, "dmaout: startLowPowerTransition buffer_ring=%X size=%d", (int) buffer_ring, buffer_ring_size);
#endif

    mRingBuffer.buffer = buffer_ring;
    mRingBuffer.phy_addr_for_shift  = getArmPhysicalAddr(mRingBuffer.buffer);
    mRingBuffer.buffer[0] = 0;
    mRingBuffer.buffer[1] = 0;
    mRingBuffer.buffer[2] = 0;
    mRingBuffer.buffer[3] = 0;
    mRingBuffer.buffer[4] = 0;
    mRingBuffer.buffer[5] = 0;
    mRingBuffer.buffer[6] = 0;
    mRingBuffer.buffer[7] = 0;

    mRingBuffer.buffer      = mRingBuffer.buffer + 8;
    mRingBuffer.phy_addr[0] = getArmPhysicalAddr(mRingBuffer.buffer);
    mRingBuffer.phy_addr[1] = getArmPhysicalAddr(mRingBuffer.buffer+BURST_SIZE_STEREO);

    reqLowPowerState = BURST_MODE;
}

//-----------------------------------------------------------------------------
//! \brief Start Normal mode
//-----------------------------------------------------------------------------
void METH(startNormalModeTransition)(){
#ifdef DEBUG_TRACE
    OstTraceInt0(TRACE_DEBUG, "dmaout: startNormalModeTransition");
#endif
    reqLowPowerState = NORMAL_MODE;
}

void METH(mutePort)(t_uint16 port_idx) {

    portStatus_t *p = &mMasterPortBuffers[port_idx];

#ifdef DEBUG_TRACE
    OstTraceInt0(TRACE_DEBUG, "dmaout:mutePort");
#endif
    if (p->isRunning && (mProcess != DmaOut_Process_NormalMode_NotYetStarted))
        start_fading(port_idx, false);
    else
        low_signal.portMuted(port_idx);
}
//=============================================================================
//                Start / Stop Dma Interface
//=============================================================================
//-----------------------------------------------------------------------------
//! \brief internally really stop DMA
//! if a source is running, we maintain dummy TX to count frames for low power
//!
//-----------------------------------------------------------------------------
void internalStopDma(void){
#ifdef DEBUG_TRACE
    OstTraceInt0(TRACE_DEBUG, "dmaout: internalStopDma enter");
#endif
    MASK_EOT_IT;
    releaseTxInterruptHandler(mFrameCount);
    UNMASK_EOT_IT;
    if (!mPleaseDoNotStop){
#ifdef DEBUG_TRACE
        OstTraceInt0(TRACE_DEBUG, "dmaout: internalStopDma call stop_DMA_MSP");
#endif
        mIsTxRunning = false;
        stop_DMA_MSP();
        resetNormalState();
    }
    else{
        // we want to keep dma running
        mPleaseDoNotStop=false;
    }
#ifdef DEBUG_TRACE
    OstTraceInt0(TRACE_DEBUG, "dmaout: internalStopDma done");
#endif
}


bool isRunningGlobally(void){
    int portIdx;
    bool ret = false;
    for (portIdx = 0; portIdx < NB_INPUT_PORTS ; portIdx++) {
        if(mMasterPortBuffers[portIdx].isRunning){
            ret |= true;
        }
    }
    return ret;
}

//-----------------------------------------------------------------------------
//! \brief Start DMA transfer proxy interface
//!
//! This is called by different OMX sinks when they goes in executing state.
//! First sinks that goes in executing starts effectivelly DMA tranfer. Other
//! calls are ignored.
//!
//-----------------------------------------------------------------------------
void startDmaIfNeeded(void){
#ifdef DEBUG_TRACE
    OstTraceInt0(TRACE_DEBUG, "dmaout: startDmaIfNeeded entry");
#endif

    if (mPleaseStopAsap){
        // this case can happen in the case of async stopDmaIfNeeded
        // in this case mIsTxRunning is true but not for a long time.
        // IT handler is not yet called to manage the mPleaseStopAsap and so stop_DMA_MSP is not yet called
        // if nothing is done we will stop dma and new sink that we are starting can be blocked
        // we still need to call portIsStopped controller interface for the first sink to be stopped
#ifdef DEBUG_TRACE
        OstTraceInt0(TRACE_DEBUG, "dmaout: startDmaIfNeeded called while we are stopping dma");
#endif
        // will be managed in the internalStopDma function to avoid stopping dma
        mPleaseDoNotStop = true;
        low_signal.digitalMute(0);
    }

    // if real tx is running(diff from dummy tx for sources) nothing to do
    if(!mIsTxRunning){
        mIsTxRunning = true;

#ifdef DEBUG_TRACE
        OstTraceInt0(TRACE_DEBUG, "dmaout: startDmaIfNeeded switching to executing state");
#endif
        reset();

        if(mNbMspChannelEnabled != 8) {
            computeSlotLimitation();
        }

	    // ASSERT(mNbMspChannelEnabled == 8);

        mProcess               = DmaOut_Process_NormalMode_NotYetStarted;
        low_signal.digitalMute(0);
        start_DMA_MSP();
    }
}

//-----------------------------------------------------------------------------
//! \brief Stop DMA transfer proxy interface
//!
//! This is called by different OMX sinks when they go into idle state. Last
//! sink that goes in idle stops effectivelly DMA tranfer (if no source is running).
//! Other calls are ignored.
//! returns true if a parallel sink is running
//!
//-----------------------------------------------------------------------------
bool stopDmaIfNeeded(int portIdx){
    bool isSynchronous = true;
#ifdef DEBUG_TRACE
    OstTraceInt0(TRACE_DEBUG, "dmaout: stopDmaIfNeeded entry");
#endif

    //here we check if any parallel sink is still running
    //only last sink will really stop dmaout (if no source is running and needing dummy tx. This is handled by stop_DMA_MSP)
    if(! isRunningGlobally()){
#ifdef DEBUG_TRACE
        OstTraceInt0(TRACE_DEBUG, "dmaout: stopDmaIfNeeded: no other sink is running");
#endif
        low_signal.digitalMute(1);

        // We go first into a known state of the FSM (process = NotYetYetStarted, IT = defaultTx)
        // then we stop dma if no rx is running in parallel (handled in internal stop_DMA_MSP)
        if(mProcess == DmaOut_Process_NormalMode_NotYetStarted){
            internalStopDma();
        }
        else{
            mMemorizedPortIdxForIdle = portIdx;
            reqLowPowerState  = NORMAL_MODE;
            isSynchronous   = false;
            mPleaseStopAsap = true;
        }
    }
    return isSynchronous;
}

static inline void consumeJitterBuffer(void)
{
    /*
    *     JitterBuffer.read_index      k
    *     DMA currently reading        k+1
    *     next_linked_dma_job          k+2
    *
    */
    int next_linked_dma_job = get_next_linked_dma_job(mDmaParam_buffer);
    int slots_advanced = 0;
    if (next_linked_dma_job != 0xFF) {
        unsigned int cur_read_index;
        if (next_linked_dma_job >= 1) {
            cur_read_index = next_linked_dma_job - 1;
        } else {
            cur_read_index = JitterBuffer.length-1;
        }

        slots_advanced = cur_read_index - JitterBuffer.read_index;

        if (slots_advanced < 0) {
            slots_advanced += JitterBuffer.length;
        }

        JitterBuffer.read_index = cur_read_index;

    } else {
        // DMA is stopping, so no new linked job ID available
        JitterBuffer.read_index = (JitterBuffer.read_index+1) % JitterBuffer.length;
        slots_advanced = 1;
    }

    if (JitterBuffer.filledslots == JitterBuffer.length)
        xrun = false;

    if (!xrun) {
        if (slots_advanced <= JitterBuffer.filledslots) {
            JitterBuffer.filledslots -= slots_advanced;
        } else {
            JitterBuffer.filledslots = 0;
        }

        if (!JitterBuffer.filledslots) {
            xrun = true;

            if (log_xrun) {
#ifdef DEBUG_TRACE
			    OstTraceInt0(TRACE_WARNING, "dmaout: XRUN");
#endif
                log_xrun = false;
            }
	    }
    }

    if (xrun || JitterBuffer.filledslots == 1) {
        t_uint16 idx = (JitterBuffer.read_index+1) % JitterBuffer.length;
        zeroJitterBufferSlot(idx);
    }
}

//-----------------------------------------------------------------------------
//! \brief Toggle circular buffer read index and decrement filled buffers
//!
//! This function must only be called inside interrupt when a DMA transfer
//! is finished
//-----------------------------------------------------------------------------
static inline void toggleRingBufferRead(void){
    if (mRingBuffer.filledSlots >= NB_SLOTS_IN_ONE_BURST){
        mRingBuffer.filledSlots -= NB_SLOTS_IN_ONE_BURST;
    }

    mRingBuffer.readIndex = 1 - mRingBuffer.readIndex;

    if(mRingBuffer.filledBuffers > 0){
        mRingBuffer.filledBuffers--;
    }
}

//-----------------------------------------------------------------------------
//! \brief Starts the cyclic DMA transfer
//!
//! This function must only be called inside interrupt when a DMA transfer
//! finishes.
//-----------------------------------------------------------------------------
static inline void StartTransferNormalMode(void){
    start_linked_tx(AB_BLOCKSIZE_MONO_1MS * mNbMspChannelEnabled, JitterBuffer.phy_addr[0], MSP_1_REG_BASE_ADDR, DMA_MSP1_3_EE_OFFSET, mDmaParam_buffer, AB8500_BURSTSIZE);
}

//-----------------------------------------------------------------------------
//! \brief Starts burst mode (32 ms) DMA transfer
//!
//! This function must only be called inside interrupt when a DMA transfer
//! finishes.
//-----------------------------------------------------------------------------
static inline void StartTransferBurst(void){
    
    startTxNoWait(BURST_SIZE_STEREO, mRingBuffer.phy_addr[mRingBuffer.readIndex],
            MSP_1_REG_BASE_ADDR, DMA_MSP1_3_EE_OFFSET, mDmaParam_buffer, AB8500_BURSTSIZE);
    burst_running = true;
}

void 
setup_mute_cb(t_uint16 port_idx, t_uint16 slots) {

    MASK_EOT_IT;
    mMasterPortBuffers[port_idx].time_stamp = *mTimeStampPtr;
    UNMASK_EOT_IT;
 
    mMasterPortBuffers[port_idx].time_diff_cb = NORMAL_TIME_INCREMENT * (slots+1);
    mMasterPortBuffers[port_idx].mute_cb = true;
}

static void
send_mute_resp(t_uint16 port_idx) {

    t_uint16 i,j;
    portStatus_t *p = &mMasterPortBuffers[port_idx];
    low_signal.portMuted(port_idx);
    p->mute_cb = false;

    /** If this is the only port running apply digital mute. */
    j = 0;
    for (i = 0; i < NB_INPUT_PORTS; i++) {
        if(mMasterPortBuffers[i].isRunning && !mMasterPortBuffers[i].muted)
            j++;
    }

    if (!j)
        low_signal.digitalMute(1);
}

static void
check_fire_mute_cb() {
    int i;
    t_uint48 cur,diff;

    MASK_EOT_IT;
    cur = *mTimeStampPtr;
    UNMASK_EOT_IT;

    for (i = 0; i < NB_INPUT_PORTS; i++) {
        if (mMasterPortBuffers[i].mute_cb) {

            if (cur > mMasterPortBuffers[i].time_stamp)
                diff = cur - mMasterPortBuffers[i].time_stamp;
            else
                diff = mMasterPortBuffers[i].time_stamp - cur;

            if (diff >= mMasterPortBuffers[i].time_diff_cb)
                send_mute_resp(i);
        }
    }
}

void
done_fading(t_uint16 port_idx) {

    portStatus_t *p = &mMasterPortBuffers[port_idx];
    if (p->mute_cb || (p->ramp_steps && (p->ramp_inc < 0))) {
        p->muted = true;
        send_mute_resp(port_idx);
    }
    p->ramp_steps = 0;
}
//=============================================================================
//                             NORMAL MODE
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

void METH(processEvent)(void){
    ENTER_CRITICAL_SECTION;
    mPendingEvent = false;
    mProcess();
    EXIT_CRITICAL_SECTION;

}

/* todo: remove */
void METH(feedback_processEvent)(void){
    mPendingFeedbackEvent = false;
}

static inline void Process_NormalMode(void) {
    t_uint16 slots,i,j,k;
    bool only_one_input = true;

    check_fire_mute_cb();

    j = 0;
    for (i = 0; i < NB_INPUT_PORTS; i++) {
        if( mMasterPortBuffers[i].isRunning)
            j++;
    }
    if (j>1)
        only_one_input = false;

    MASK_EOT_IT;
    slots = JitterBuffer.filledslots;
    UNMASK_EOT_IT;

    if (xrun) {
        /** Underrun has occurred, reconstruct the jitter buffer! */
        fillJitterBufferWithZeros();

        MASK_EOT_IT;
        JitterBuffer.filledslots = JitterBuffer.length;
        JitterBuffer.write_index = JitterBuffer.read_index;
        UNMASK_EOT_IT;

#ifdef DEBUG_TRACE
        OstTraceInt2(TRACE_DEBUG, "dmaout WARN: Normal process: UNDERRUN! write_index %d, jitter_buffer.filledslots %d", JitterBuffer.write_index, JitterBuffer.filledslots);
#endif
        for (i = 0; i < NB_INPUT_PORTS; i++) {
            if( mMasterPortBuffers[i].isRunning)
                done_fading(i);
        }
    } else {
        /** Fill the jitter buffer by muxing the streams from all devices. If the
         * jitterbuffer falls below the low watermark then force muxing of the streams.
         */
        i = JitterBuffer.length - slots;
        j = 0;
        while ( i-- && processInputBuffers(false)) {
            j++;
        }

        if (j)
            log_xrun = true;

        MASK_EOT_IT;
        slots = JitterBuffer.filledslots;
        UNMASK_EOT_IT;

        if ( slots < DMAOUT_JITTERBUFFER_LOW_WATERMARK && !only_one_input) {
            processInputBuffers(true);
            log_xrun = true;
#ifdef DEBUG_TRACE
            OstTraceInt2(TRACE_WARN, "dmaout: Normal process WARN: 1 slot(s) was muxed by force! write_index %d, jitter_buffer.filledslots %d", JitterBuffer.write_index, JitterBuffer.filledslots);
#endif
        }
    }
    DmaOut_nokToSleep();
}

void DmaOut_Process_NormalMode_NotYetStarted(void) {
#ifdef DEBUG_TRACE
    OstTraceInt0(TRACE_DEBUG, "dmaout: DmaOut_Process_NormalMode_NotYetStarted");
#endif

    mProcess = DmaOut_Process_NormalMode_WithFeedback;
    MASK_EOT_IT;
    mFrameCount  = overrideTxInterruptHandler(IT_Handler_Normal_Mode_First_It);
    UNMASK_EOT_IT;

    DmaOut_nokToSleep();
}

void DmaOut_Process_NormalMode_WithFeedback(void) {
    
    t_uint16 idx = 0;

#ifdef DEBUG_TRACE    
    OstTraceInt0(TRACE_DEBUG, "dmaout: Process NormalMode_WithFeedback");
#endif
    Process_NormalMode();

    if(reqLowPowerState == BURST_MODE && actualLowPowerState == NORMAL_MODE) {
        resetLowPowerState();
        mProcess = DmaOut_Process_PrepareBurstMode;
        while ( !mMasterPortBuffers[idx].isRunning)
            idx++;
        burst_port_idx = idx;
    }
    DmaOut_nokToSleep();
}

void DmaOut_Process_NormalMode_DisableItFifo (void) {

    AUDIO_MASK_IT(PRCMU_FW_IT);
    Fifo_It_Enabled = false;
    ItFiFoConfigurationThroughPrcmu(0);

    Process_NormalMode();

    mProcess = DmaOut_Process_NormalMode_WithFeedback;
#ifdef DEBUG_TRACE
    OstTraceInt0(TRACE_DEBUG, "dmaout: Exit process DisableItFifo");
#endif
    DmaOut_nokToSleep();
}

void DmaOut_Process_PrepareBurstMode(void) {
    t_uint16 slots,i;
    t_uint16 j = 0;

    check_fire_mute_cb();

    while ( !mAbandonLPATran && (mRingBuffer.filledSlots < NB_SLOTS_IN_RING_BUFFER) && processInputBuffersBurst(burst_port_idx));

    MASK_EOT_IT;
    slots = JitterBuffer.filledslots;
    UNMASK_EOT_IT;

    i = JitterBuffer.length - slots;

    if (!mAbandonLPATran  && !mRingBuffer.filledSlots && (slots < DMAOUT_JITTERBUFFER_LOW_WATERMARK)) {
        mAbandonLPATran = true;
        while (mRingBuffer.filledSlots < NB_SLOTS_IN_RING_BUFFER)
            fillRingSlotWithZeros();
    }

    while ( i-- && mRingBuffer.filledSlots) {
        /** Dmaout is not allowed to move the readindex of the ringbuffer without disabling the ITs. */
        MASK_EOT_IT;
        if ( SlotsToMoveFromRing)
            SlotsToMoveFromRing--;
        SetupBurstDeviceCtr++;
        copySlotFromRingBufferToJitterBuffer(burst_port_idx);
        UNMASK_EOT_IT;
    }

    if( SetupBurstDeviceCtr == JITTERBUFFER_TX_MS && burst_port_idx != AVSINK_INPUT_PORT_IDX) {
        SetupBurstDeviceCtr++;
        low_signal.lowPowerModeSetup(burst_port_idx);
    }

    if (mAbandonLPATran) {
        while (mRingBuffer.filledSlots < NB_SLOTS_IN_RING_BUFFER)
            fillRingSlotWithZeros();
    }

#ifdef DEBUG_TRACE
    OstTraceInt0(TRACE_DEBUG, "dmaout: Exit Process PrepareBurstMode");
#endif
    check_fire_mute_cb();
    DmaOut_nokToSleep();
}

void DmaOut_Process_EnterBurstMode (void) {
    low_signal.lowPowerMode( mSwitchFrameCount);
    mProcess = DmaOut_Process_AwaitSwitchToBurstMode;

#ifdef DEBUG_TRACE
    OstTraceInt1(TRACE_DEBUG, "dmaout: Process EnterBurstMode SwitchFrame: %u", mSwitchFrameCount);
#endif
    check_fire_mute_cb();
    DmaOut_nokToSleep();
}

void DmaOut_Process_AwaitSwitchToBurstMode (void) {

    t_uint16 slots,i;
    t_uint16 j = 0;

    check_fire_mute_cb();

    while ( (mRingBuffer.filledSlots < NB_SLOTS_IN_RING_BUFFER) && processInputBuffersBurst(burst_port_idx));

    MASK_EOT_IT;
    slots = JitterBuffer.filledslots;
    UNMASK_EOT_IT;

    i = JitterBuffer.length - slots;

    while ( i-- && SlotsToMoveFromRing) {
        MASK_EOT_IT;
        copySlotFromRingBufferToJitterBuffer(burst_port_idx);
        UNMASK_EOT_IT;
        SlotsToMoveFromRing--;
    }

#ifdef DEBUG_TRACE
    OstTraceInt1(TRACE_DEBUG, "dmaout: Process AwaitSwitchToBurstMode SlotsToMoveFromRing: %u",SlotsToMoveFromRing);
#endif
    DmaOut_nokToSleep();
}

void DmaOut_Process_BurstMode (void) {

    check_fire_mute_cb();

    while ( (mRingBuffer.filledSlots < NB_SLOTS_IN_RING_BUFFER) && processInputBuffersBurst(burst_port_idx));

#ifdef DEBUG_TRACE
    OstTraceInt0(TRACE_DEBUG, "dmaout: Exit Process BurstMode");
#endif
    DmaOut_nokToSleep();
    if (mRingbufferXruns) {
#ifdef DEBUG_TRACE
        OstTraceInt1(TRACE_WARNING, "dmaout LPA tran: XRUN: Inserted %u ms silence during LPA transition\n", mRingbufferXruns);
#endif
        mRingbufferXruns = 0;
        done_fading(burst_port_idx);
    }
}

void DmaOut_Process_PrepareSleep (void) {

    check_fire_mute_cb();

    while ( (mRingBuffer.filledSlots < NB_SLOTS_IN_ONE_BURST) && processInputBuffersBurst(burst_port_idx));

    if (mRingBuffer.filledSlots >= NB_SLOTS_IN_ONE_BURST) {
#ifdef DEBUG_TRACE
        OstTraceInt0(TRACE_DEBUG, "dmaout: Call allowSleep");
#endif
        DmaOut_okToSleep();
        mProcess = DmaOut_Process_DoNothing;
    } else {
        DmaOut_nokToSleep();
    }

    if(mRingbufferXruns) {
#ifdef DEBUG_TRACE
        OstTraceInt1(TRACE_WARNING, "XRUN: Inserted %u ms silence\n", mRingbufferXruns);
#endif
        mRingbufferXruns = 0;
        done_fading(burst_port_idx);
    }

#ifdef DEBUG_TRACE
    OstTraceInt0(TRACE_DEBUG, "dmaout: Exit Process PrepareSleep");
#endif
}

void DmaOut_Process_DoNothing (void) {

    check_fire_mute_cb();

    if( mRingbufferXruns) {
#ifdef DEBUG_TRACE
        OstTraceInt1(TRACE_WARNING, "XRUN: Inserted %u ms silence\n", mRingbufferXruns);
#endif
        mRingbufferXruns = 0;
        done_fading(burst_port_idx);
    }

#ifdef DEBUG_TRACE
    OstTraceInt0(TRACE_DEBUG, "dmaout: Process DoNothing");
#endif
}

/**
 *  IT handlers
 */

void IT_Handler_Normal_Mode_BackFromBurstMode(void) {

    mFrameCount= (mFrameCount+6) & 0xFF;
    *mTimeStampPtr += NORMAL_TIME_INCREMENT * NB_SLOTS_IN_ONE_BURST;

    overrideTxInterruptHandler(IT_Handler_Normal_Mode_FIFO_Emptied);

    burst_running = false;
    ASSERT(mRingBuffer.filledSlots == 32);

    // Just for debugging. Buffer will be deallocated when low_signal.normalMode()
    mRingBuffer.buffer = 0;
    mRingBuffer.phy_addr_for_shift  = 0;
    mRingBuffer.phy_addr[0] = 0;
    mRingBuffer.phy_addr[1] = 0;

    mProcess = DmaOut_Process_NormalMode_DisableItFifo;

    // This call also signals to RM that we have exited burst mode
    low_signal.normalMode();

    scheduleProcessEvent();
}

void IT_Handler_Normal_Mode_FIFO_Emptied(void) {

    consumeJitterBuffer();
    mFrameCount=(mFrameCount+6) & 0xFF;
    *mTimeStampPtr += NORMAL_TIME_INCREMENT;
    overrideTxInterruptHandler(IT_Handler_Normal_Mode);
    actualLowPowerState = NORMAL_MODE;
    low_signal.fifoEmptied();
    scheduleProcessEvent();
}

#pragma lock
void IT_Handler_Normal_Mode_First_It(void) {

    int index = msp_dma_getIndex();

    JitterBuffer.read_index  = index;
    JitterBuffer.write_index = (index + 1) %  JitterBuffer.length;
    JitterBuffer.filledslots = 1;

    while (JitterBuffer.filledslots < JitterBuffer.length && processInputBuffers(false /* output not forced */));

    mFrameCount=(mFrameCount+6) & 0xFF;
    *mTimeStampPtr += NORMAL_TIME_INCREMENT;

    overrideTxInterruptHandler(IT_Handler_Normal_Mode);
    mProcess = DmaOut_Process_NormalMode_WithFeedback;

    scheduleProcessEvent();
}

void IT_Handler_Normal_Mode(void) {

    consumeJitterBuffer();
    mFrameCount=(mFrameCount+6) & 0xFF;
    *mTimeStampPtr += NORMAL_TIME_INCREMENT;

#ifdef DEBUG_TRACE
    OstTraceInt1(TRACE_DEBUG, "dmaout: IT_Handler_Normal_Mode: mPleaseStopAsap %d", mPleaseStopAsap);
#endif

    /** TODO: This has to go */
    if(mPleaseStopAsap == true) {
        mProcess = DmaOut_Process_DoNothing;
        mPleaseStopAsap = false;
        internalStopDma();
        //if (!mErrorRecovery){
            dma_port_state[mMemorizedPortIdxForIdle].portIsStopped();
        //}
        //else{
            //TODO
            //restart after stop
        //}
    }

    /**
     * If the counter is a non-zero value dmaout intend to move to burstmode.
     */
    if (CountdownToBurstSwitch) {
        CountdownToBurstSwitch--;

        /** Examine the countdown counter. If the counter is one this is the
         * last dma transfer so stop the linked transfer by setting the
         * relink parameters to zero.
         * If the counter is zero set up a dma transfer of size 32 ms.
         */
        if (CountdownToBurstSwitch == 1) {
            stop_linked_tx (mDmaParam_buffer);
            overrideTxInterruptHandler(IT_Handler_Normal_Last_It);
        }
    }

    /**
     * If a move to burst mode is scheduled AB8500 has to be informed on what sample
     * the switch is intended to happen.
     *
     * For simplicity the swith is scheduled so that the readindex of the ringbuffer is zero.
     */

    if ( mProcess == DmaOut_Process_PrepareBurstMode) {

        if ((mRingBuffer.filledSlots == NB_SLOTS_IN_RING_BUFFER) && ( (mRingBuffer.readSlotIndex - JitterBuffer.filledslots) ==NB_SLOTS_IN_ONE_BURST)) {
            mSwitchFrameCount = (mFrameCount + 6 * (NB_SLOTS_IN_ONE_BURST-1)) & 0x0FF;
            CountdownToBurstSwitch =  NB_SLOTS_IN_ONE_BURST;
            SlotsToMoveFromRing = NB_SLOTS_IN_ONE_BURST - JitterBuffer.filledslots;
            mProcess = DmaOut_Process_EnterBurstMode;
        }
    }

    scheduleProcessEvent();
}

#pragma lock
void IT_Handler_Normal_Last_It (void) {
    StartTransferBurst();
    *mTimeStampPtr += NORMAL_TIME_INCREMENT;

    ASSERT(mSwitchFrameCount == mFrameCount);
    ASSERT(JitterBuffer.filledslots == 1);

#ifdef DEBUG_TRACE
    OstTraceInt0(TRACE_DEBUG, "dmaout: Process NormalMode_WithFeedback, last it before burst transition");
#endif
    CountdownToBurstSwitch = 0;
    overrideTxInterruptHandler(IT_Handler_Burst_Mode_First_It);
    actualLowPowerState = BURST_MODE;    

    scheduleProcessEvent();
}

#pragma lock
void IT_Handler_Burst_Mode_First_It(void) {
    toggleRingBufferRead();
    *mTimeStampPtr += NORMAL_TIME_INCREMENT * NB_SLOTS_IN_ONE_BURST;

    overrideTxInterruptHandler(IT_Handler_Burst_Mode);
    mProcess = DmaOut_Process_BurstMode;
    scheduleProcessEvent();

    // Notify proxy that we have entered burst mode
    low_signal.acknowledgeRmForLowPowerMode();
#ifdef DEBUG_TRACE
    OstTraceInt0(TRACE_DEBUG, "dmaout: acknowledge burst mode enter");
#endif

    burst_running = false;
    switchAb8500_FifoWaterMarkHandler(IT_AB8500_Fifo);
    if (!Fifo_It_Enabled) {
#ifdef DEBUG_TRACE
        OstTraceInt0(TRACE_DEBUG, "dmaout: IT_Handler_Burst_Mode_First_It, unmask it");
#endif
        ItFiFoConfigurationThroughPrcmu(1);
        AUDIO_UNMASK_IT(PRCMU_FW_IT);
        Fifo_It_Enabled = true;
    }
}

void IT_Handler_Burst_Mode(void) {
    toggleRingBufferRead();
    *mTimeStampPtr += NORMAL_TIME_INCREMENT * NB_SLOTS_IN_ONE_BURST;

#ifdef DEBUG_TRACE
    OstTraceInt0(TRACE_DEBUG, "dmaout: IT_Handler_Burst_Mode");
#endif
    mProcess = DmaOut_Process_PrepareSleep;
    scheduleProcessEvent();

    burst_running = false;
}

void IT_Handler_BurstMode_Leave(void) {
    toggleRingBufferRead();
    burst_running = false;
    *mTimeStampPtr += NORMAL_TIME_INCREMENT * NB_SLOTS_IN_ONE_BURST;

    scheduleProcessEvent();
}

void METH(IT25handler)(void) {
}

void IT_AB8500_Fifo(void) {
    t_uint16 nslots = 0;
    bool underrun = false;
    
    DmaOut_nokToSleep();    

#ifdef DEBUG_TRACE
    OstTraceInt0(TRACE_DEBUG, "dmaout: IT_AB8500_Fifo");
#endif
    if (CountdownToBurstSwitch == 1) {
        start_last_burst_tx(BURST_SIZE_STEREO, mRingBuffer.phy_addr[mRingBuffer.readIndex],
                MSP_1_REG_BASE_ADDR, DMA_MSP1_3_EE_OFFSET, mDmaParam_buffer, AB8500_BURSTSIZE);

        overrideTxInterruptHandler(IT_Handler_Normal_Mode_BackFromBurstMode);
    }
    else
    {
        StartTransferBurst();
    }

    nslots = mRingBuffer.filledSlots;

    if (nslots < NB_SLOTS_IN_ONE_BURST){
        fillBurstBufferWithZeros();
        mRingbufferXruns = NB_SLOTS_IN_ONE_BURST - nslots;
    }

    mProcess = DmaOut_Process_BurstMode;

    if (CountdownToBurstSwitch) {
#ifdef DEBUG_TRACE
        OstTraceInt1(TRACE_DEBUG, "dmaout: Number of filled slots: %u", nslots);
#endif
        CountdownToBurstSwitch--;
        mProcess = DmaOut_Process_NormalMode_WithFeedback;

        if(!CountdownToBurstSwitch) {
            switchAb8500_FifoWaterMarkHandler(NULL);
        }
    } else if (reqLowPowerState == NORMAL_MODE ) {
        resetNormalState();
        zeroJitterBufferStripe(mAccessoryInfo[AVSINK_INPUT_PORT_IDX].nb_channel, mAccessoryInfo[AVSINK_INPUT_PORT_IDX].mspslot,4);
        zeroJitterBufferStripe(mAccessoryInfo[IHF_INPUT_PORT_IDX].nb_channel, mAccessoryInfo[IHF_INPUT_PORT_IDX].mspslot,4);
        mFrameCount = 0;
        CountdownToBurstSwitch = 2;
        overrideTxInterruptHandler(IT_Handler_BurstMode_Leave);
    }

    scheduleProcessEvent();
}
