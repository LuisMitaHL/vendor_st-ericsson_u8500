/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   dmain.c
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/

// OST framework
#include "usefull.h"
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
  #include "audio_audiocodec_mpc_dmain_src_dmainTraces.h"
#endif

#include "dmain.h"


#define MAX_TDM_SLOTS (8)
#define DMAIN_BLOCK_TRANSFER_COUNT (5)
#define DMAIN_PORT_BUFFER_MS (5)
#define DMAIN_WATERMARK_MS (8)

//=============================================================================
//                  Global Variables
//=============================================================================

struct DmaInPortInfo{
    unsigned int tdmslot;
    unsigned int channel;
    unsigned int nb_channel;
};
typedef struct DmaInPortInfo DmaInPortInfo;

DmaInPortInfo mAccessoryInfo    [TOTAL_NB_PORTS];
portStatus_t  mMasterPortBuffers[TOTAL_NB_PORTS];
//-----------------------------------------------------------------
// Internal states variables

static bool xrun = false;
static volatile bool         mIsRxRunning  = false;       //!< indicates if transfer is hapenning

static bool  mPendingEvent = false;       //!< is a process scheduled on input port ?

//--------------------------------------------------------------
// HW related variable
static volatile long EXTMMIO * mDmaParam_buffer     = NULL;
static t_uint16                mNbMspChannelEnabled = 0;    //!< Note that this must be useless on DB8500v2. On v2 only 8 channels.
static t_uint16                mDmaEndOfTranferIT   = 0;    //!< End of Dma transfer interrupt level

//-----------------------------------------------------------------
// Buffers
JitterBuffer_t JitterBuffer;
//-------------------------------------------------
// Sampled Played Counter for audio/video synchro
t_uint48 * mSamplesPlayedPtr = 0;
t_uint48 * mTimeStampPtr     = 0;


//=============================================================================
//               Reset / Init Function
//=============================================================================
//-----------------------------------------------------------------------------
//! \brief Initialise Omx port related information
//-----------------------------------------------------------------------------
static void initPortInfo(void){
    int i;

    // assign default msp slot index
    mAccessoryInfo[MULTIMIC_PORTIDX].channel = MULTIMIC_MSPSLOT;
    mAccessoryInfo[MULTIMIC_PORTIDX].tdmslot = MULTIMIC_MSPSLOT;
    
    mAccessoryInfo[AVSOURCE_PORTIDX].channel = AVSOURCE_MSPSLOT;
    mAccessoryInfo[AVSOURCE_PORTIDX].tdmslot = AVSOURCE_MSPSLOT;
    
    mAccessoryInfo[FMRX_PORTIDX    ].channel = FMRX_MSPSLOT;
    mAccessoryInfo[FMRX_PORTIDX    ].tdmslot = FMRX_MSPSLOT;
    
    // assign number of channels
    mAccessoryInfo[MULTIMIC_PORTIDX].nb_channel = MULTIMIC_NB_CHANNEL;
    mAccessoryInfo[AVSOURCE_PORTIDX].nb_channel = AVSOURCE_NB_CHANNEL;
    mAccessoryInfo[FMRX_PORTIDX    ].nb_channel = FMRX_NB_CHANNEL;
    
    for (i = 0; i < TOTAL_NB_PORTS; i++) {
        mMasterPortBuffers[i].isRunning       = false;
        mMasterPortBuffers[i].weHaveTheBuffer = false;
        mMasterPortBuffers[i].buffer          = NULL;
        mMasterPortBuffers[i].data_droppped   = true;
    }
}

//-----------------------------------------------------------------------------
//! \brief  Global variables initialization
//!
//! Reset variables used by PingPong buffer.
//! Must be called before starting first dma transfer
//!
//-----------------------------------------------------------------------------
static void reset(void) {
    int i;

    JitterBuffer.filledslots = 0;
    JitterBuffer.write_index = 0;
    JitterBuffer.read_index  = 0;
    
    ASSERT(mSamplesPlayedPtr!=0);
    
    //TODO: modify NB_INPUT_PORTS when SetSamplesPlayedAddr will provide size
    for (i = 0; i < TOTAL_NB_PORTS; i++) {
        mSamplesPlayedPtr[i] = 0;
    }

    mTimeStampPtr = get_timestamp_address();
}

//-----------------------------------------------------------------------------
//! \brief Computes Multi channel Msp mask
//!
//! The Ab8500 transmits 8 slots. This function configures the
//! MSP to receive slots that are mapped to sources that are open.
//!
//-----------------------------------------------------------------------------
static void computeSlotLimitation(void){
    int i,j,k;
    int channels_used = 0;
    int msp_slot_en   = 0;
    int nb_bit_to_force = 0;

    /**
     * If a source is open enable the slots that are mapped
     * to that source.
     */

    for (i = 0; i < TOTAL_NB_PORTS ; i++) {
        if (!IS_NULL_INTERFACE(outputport[i],emptyThisBuffer)) {
            int nb_channel = mAccessoryInfo[i].nb_channel;
            int mask = nb_channel == 2 ? 0x3 : 0x1;
            mAccessoryInfo[i].channel = 0;
            msp_slot_en |= mask <<  mAccessoryInfo[i].tdmslot;
        }
    }

    for (i = 0; i < MAX_TDM_SLOTS ; i++) {

        if (channels_used == AB_8500_RX_MSP_SLOTS_MAX)
            msp_slot_en &= !(1 << i);
        else if (msp_slot_en & (1 << i))
            channels_used++;
    }

    /**
     * Audiocodec support 4 different data formats. One, two or
     * AB_8500_RX_MSP_SLOTS_MAX number of slots received per frame.
     * Audiocodec needs to enabe slots until one of the supported
     * formats is hit.
     */

    if ( channels_used > 2)
	mNbMspChannelEnabled = AB_8500_RX_MSP_SLOTS_MAX;
    else
	mNbMspChannelEnabled = channels_used;

    nb_bit_to_force = (mNbMspChannelEnabled-channels_used);
    
    i = 0;
    while ( (i < MAX_TDM_SLOTS) &&  nb_bit_to_force) {
        if (!(msp_slot_en & (1 << i))) {
            msp_slot_en |= 1 << i;
            nb_bit_to_force--;
        }
        i++;
    }

    /**
     * The offset in the received frame has to be calculated for
     * all opened sources.
     */

    j = 0;
    for (i = 0; i < MAX_TDM_SLOTS ; i++) {
        if (msp_slot_en & (1 << i)) {
            for (k = 0 ; k < TOTAL_NB_PORTS; k++) {
                if (mAccessoryInfo[k].tdmslot == i) {
                    mAccessoryInfo[k].channel = j;
                }
            }
            j++;
        }
    }

#ifdef DEBUG_TRACE
    OstTraceInt2(TRACE_DEBUG, "dmain: computeSlotLimitation %d %d", msp_slot_en, mNbMspChannelEnabled);
#endif
    setMspChannelEnableRx(msp_slot_en, mNbMspChannelEnabled);
}

static unsigned int setup_jitter_buffer(void) {
    int i;

    JitterBuffer.filledslots = 0;
    JitterBuffer.write_index = 0;
    JitterBuffer.read_index  = 0;

    for ( i=1; i<JitterBuffer.length; i++) {
        JitterBuffer.buffer[i] = JitterBuffer.buffer[i-1] + AB_BLOCKSIZE_MONO_1MS * mNbMspChannelEnabled;
        JitterBuffer.phy_addr[i] = getArmPhysicalAddr(JitterBuffer.buffer[i]);
    }

    switch (mNbMspChannelEnabled) {
        case 1:
            return LLI_RX_TDM_SLOTS_1;
        case 2:
            return LLI_RX_TDM_SLOTS_2;
        default :
            return LLI_RX_TDM_SLOTS_4;
    }
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
void METH(setParameter)(DmaConfig_t config,t_uint16 target) {
    int i;

    PRECONDITION(JITTERBUFFER_RX_MS > DMAIN_WATERMARK_MS);

    mNbMspChannelEnabled = config.nb_msp_channel_enabled;
#ifdef DEBUG_TRACE
    OstTraceInt1(TRACE_DEBUG,"dmain MSP channels: %u\n",mNbMspChannelEnabled);
#endif

    JitterBuffer.length = JITTERBUFFER_RX_MS;
    JitterBuffer.buffer[0]   = config.buffer;
    JitterBuffer.phy_addr[0] = getArmPhysicalAddr(JitterBuffer.buffer[0]);

    for (i=1; i<JitterBuffer.length; i++) {
        JitterBuffer.buffer[i] = JitterBuffer.buffer[i-1] + AB_BLOCKSIZE_MONO_1MS * mNbMspChannelEnabled;
        JitterBuffer.phy_addr[i] = getArmPhysicalAddr(JitterBuffer.buffer[i]);
    }

    mSamplesPlayedPtr         = (t_uint48 *)config.sample_count_buffer;

    mDmaParam_buffer        = getLogicalParameterPtr(getLogicalChannel(MSP_AB8500_RX , MSP_RX));
    mDmaEndOfTranferIT      = AUDIO_MSP_GetInterruptLevel(MSP_AB8500_RX, MSP_RX);

    initPortInfo();
}

//=============================================================================
//                Buffers Handling
//=============================================================================

void returnMasterPort(unsigned int portIdx) {
#ifdef DEBUG_TRACE
    OstTraceInt1(TRACE_DEBUG, "dmain: returnMasterPort: idx=%d", portIdx);
#endif

    outputport[portIdx].emptyThisBuffer(mMasterPortBuffers[portIdx].buffer);
   
    mMasterPortBuffers[portIdx].weHaveTheBuffer = false;
    mMasterPortBuffers[portIdx].buffer          = NULL;
}

//-----------------------------------------------------------------------------
//! \brief Copy data to output buffer
//!
//! This function copy data for one accessory and returns associated buffer
//!
//-----------------------------------------------------------------------------
static void CopyDoubleBufferToOutputAndReturnBuffer(Buffer_p outputbuf, unsigned int portIndex){
    t_uint48 timestamp;
    int i;
    unsigned int channel   = mAccessoryInfo[portIndex].channel;
    int * buf              = JitterBuffer.buffer[JitterBuffer.write_index] + channel;
    unsigned int nbChannel = mAccessoryInfo[portIndex].nb_channel;

    PRECONDITION(outputbuf->allocLen == (DMAIN_PORT_BUFFER_MS*AB_BLOCKSIZE_MONO_1MS*nbChannel));

    if (nbChannel == 2) {
        for (i=0; i<(AB_BLOCKSIZE_MONO_1MS * DMAIN_PORT_BUFFER_MS); i++) {
            outputbuf->data[2*i  ] = (buf[mNbMspChannelEnabled*i]   << 4 );
            outputbuf->data[2*i+1] = (buf[mNbMspChannelEnabled*i+1] << 4 );
        }
    }
    else {
 	ASSERT(nbChannel == 1);
        for (i=0; i<(AB_BLOCKSIZE_MONO_1MS * DMAIN_PORT_BUFFER_MS); i++) {
            outputbuf->data[i] = (buf[mNbMspChannelEnabled*i  ] << 4 );
        }
    }

    timestamp = *mTimeStampPtr;
    timestamp -= DMAIN_PORT_BUFFER_MS * 1000;

    outputbuf->filledLen       = outputbuf->allocLen;
    outputbuf->nTimeStampl     = timestamp;
    outputbuf->nTimeStamph     = 0;
    outputbuf->byteInLastWord  = 2;

    // Always set this for now; otherwise timestamp won't
    // propagate correctly through network.
    outputbuf->flags          |= BUFFERFLAG_STARTTIME;

    mMasterPortBuffers[portIndex].data_droppped = false;
    returnMasterPort(portIndex);
}

static bool allBuffersAvailable () {
    bool all_ports_ready = true;

    if ( mMasterPortBuffers[MULTIMIC_PORTIDX].isRunning && !mMasterPortBuffers[MULTIMIC_PORTIDX].weHaveTheBuffer) {
        all_ports_ready = false;
    }

    if ( mMasterPortBuffers[AVSOURCE_PORTIDX].isRunning && !mMasterPortBuffers[AVSOURCE_PORTIDX].weHaveTheBuffer) {
        all_ports_ready = false;
    }

    if ( mMasterPortBuffers[FMRX_PORTIDX].isRunning && !mMasterPortBuffers[FMRX_PORTIDX].weHaveTheBuffer) {
        all_ports_ready = false;
    }

    return all_ports_ready;
}

//-----------------------------------------------------------------------------
//! \brief Returns buffer on one port, null if no buffer in port
//!
//-----------------------------------------------------------------------------
Buffer_p getOutputBuffer(int portIdx) {
#ifdef DEBUG_TRACE    
    OstTraceInt2(TRACE_DEBUG, "dmain:getOutputBuffer portIdx=%d weHaveTheBuffer=%d", portIdx, mMasterPortBuffers[portIdx].weHaveTheBuffer);
#endif
    if (mMasterPortBuffers[portIdx].weHaveTheBuffer == false) {
        return 0;
    }
    return mMasterPortBuffers[portIdx].buffer;
}

//-----------------------------------------------------------------------------
//! \brief Extract buffers
//!
//! Iterates on each ports, copy data into buffer if possible and return buffers.
//! Ping Pong buffer index/filled state are updated.
//!
//-----------------------------------------------------------------------------
static void extractOutputBuffers(void) {

    Buffer_p buf_multimic = getOutputBuffer(MULTIMIC_PORTIDX);
    Buffer_p buf_avsource = getOutputBuffer(AVSOURCE_PORTIDX);
    Buffer_p buf_fm       = getOutputBuffer(FMRX_PORTIDX);

    if (xrun) {
        mMasterPortBuffers[MULTIMIC_PORTIDX].data_droppped = true;
        mMasterPortBuffers[AVSOURCE_PORTIDX].data_droppped = true;
        mMasterPortBuffers[FMRX_PORTIDX].data_droppped = true;
        xrun = false;
    }

    if(buf_multimic || buf_fm || buf_avsource){
        MMDSP_FLUSH_DCACHE();
    }

    if (buf_multimic) {
        CopyDoubleBufferToOutputAndReturnBuffer(buf_multimic, MULTIMIC_PORTIDX);
    } else {
        if (mMasterPortBuffers[MULTIMIC_PORTIDX].isRunning &&
            mMasterPortBuffers[MULTIMIC_PORTIDX].data_droppped == false) {
        }
        mMasterPortBuffers[MULTIMIC_PORTIDX].data_droppped = true;
    }

    if (buf_avsource) {
        CopyDoubleBufferToOutputAndReturnBuffer(buf_avsource, AVSOURCE_PORTIDX);
    } else {
        if (mMasterPortBuffers[AVSOURCE_PORTIDX].isRunning &&
            mMasterPortBuffers[AVSOURCE_PORTIDX].data_droppped == false) {
        }
        mMasterPortBuffers[AVSOURCE_PORTIDX].data_droppped = true;
    }

    if (buf_fm) {
        CopyDoubleBufferToOutputAndReturnBuffer(buf_fm, FMRX_PORTIDX);
    } else {
        if (mMasterPortBuffers[FMRX_PORTIDX].isRunning &&
            mMasterPortBuffers[FMRX_PORTIDX].data_droppped == false) {
        }
        mMasterPortBuffers[FMRX_PORTIDX].data_droppped = true;
    }

    MASK_EOT_IT;
    if (!JitterBuffer.filledslots) {
        /* xrun */
        JitterBuffer.write_index = JitterBuffer.read_index;
    } else {
        JitterBuffer.filledslots -= DMAIN_PORT_BUFFER_MS;
        JitterBuffer.write_index = (JitterBuffer.write_index + DMAIN_PORT_BUFFER_MS) % JitterBuffer.length;
    }
    UNMASK_EOT_IT;
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
    int i;
#ifdef DEBUG_TRACE  
    OstTraceInt0(TRACE_DEBUG, "dmain: startDmaIfNeeded entry");
#endif
    computeSlotLimitation();    

    if(mIsRxRunning){
        stopRxTransfer(MSP_AB8500_RX);
        start_first_linked_rx_xfer( AB_BLOCKSIZE_MONO_1MS * mNbMspChannelEnabled, JitterBuffer.phy_addr[JitterBuffer.read_index], MSP_AB8500_RX, AB8500_BURSTSIZE,setup_jitter_buffer());
        return;
    }
#ifdef DEBUG_TRACE 
    OstTraceInt0(TRACE_DEBUG, "dmain: startDmaIfNeeded switching to executing state.");
#endif 
    mIsRxRunning = true;

    reset();
    
    start_DMA_MSP();
    start_first_linked_rx_xfer( AB_BLOCKSIZE_MONO_1MS * mNbMspChannelEnabled, JitterBuffer.phy_addr[JitterBuffer.read_index], MSP_AB8500_RX, AB8500_BURSTSIZE,setup_jitter_buffer());
}

//-----------------------------------------------------------------------------
//! \brief Stop DMA transfer proxy interface
//!
//! This is called by different OMX sources when they goes in idle state. First 
//! sources that goes in idle stops effectivelly DMA tranfer. Other calls are
//! ignored.
//!
//-----------------------------------------------------------------------------
bool isRunningGlobally(void){
    int portIdx;
    bool ret = false;
    for (portIdx = 0; portIdx < TOTAL_NB_PORTS ; portIdx++) {
        if(mMasterPortBuffers[portIdx].isRunning){
            ret |= true;
        }
    }
    return ret;
}

void stopDmaIfNeeded(void) {
#ifdef DEBUG_TRACE 
    OstTraceInt0(TRACE_DEBUG, "dmain: stopDmaIfNeeded entry.");
#endif
    
    if (!isRunningGlobally()) {
#ifdef DEBUG_TRACE    
        OstTraceInt1(TRACE_DEBUG, "dmain: stopDmaIfNeeded mIsRxRunning=%d", mIsRxRunning);
#endif
        if(!mIsRxRunning){
            return;
        }
#ifdef DEBUG_TRACE
        OstTraceInt0(TRACE_DEBUG, "dmain: stopDmaIfNeeded switching to idle state.");
#endif        
        mIsRxRunning = false;

        stopRxTransfer(MSP_AB8500_RX);
        stop_DMA_MSP();
    }
    else 
    {
        stopRxTransfer(MSP_AB8500_RX);
        computeSlotLimitation();
        start_first_linked_rx_xfer( AB_BLOCKSIZE_MONO_1MS * mNbMspChannelEnabled, JitterBuffer.phy_addr[JitterBuffer.read_index], MSP_AB8500_RX, AB8500_BURSTSIZE,setup_jitter_buffer());
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
#pragma lock
void METH(IT27handler)(void) {

    JitterBuffer.read_index = (JitterBuffer.read_index + 1) % JitterBuffer.length;
    JitterBuffer.filledslots++;
    
    /* xrun detection */
    if(JitterBuffer.filledslots == JitterBuffer.length) {
        xrun = true;
        JitterBuffer.filledslots = 0;
    }
    // Count how many buffers are received to get reference time for A/V sync (in ms)
    (*mSamplesPlayedPtr)++;
    
    if(mIsRxRunning && JitterBuffer.filledslots >= DMAIN_PORT_BUFFER_MS)
        scheduleProcessEvent();
}

//=============================================================================
//                      BUFFERS 
//=============================================================================
void METH(fillThisBuffer)(Buffer_p buffer, t_uint8 portIdx) {
    mMasterPortBuffers[portIdx].weHaveTheBuffer = true;
    mMasterPortBuffers[portIdx].buffer          = buffer;

    if(mMasterPortBuffers[portIdx].isRunning){
        scheduleProcessEvent();
    }
    else {
        returnMasterPort(portIdx);
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

void METH(processEvent)() {
    t_uint16 filledslots;
    
    mPendingEvent = false;
    
    MASK_EOT_IT;
    filledslots = JitterBuffer.filledslots;
    UNMASK_EOT_IT;
    
    if ( ( (filledslots >= DMAIN_PORT_BUFFER_MS) && allBuffersAvailable() ) ||
        (filledslots >= DMAIN_WATERMARK_MS) ) {
        extractOutputBuffers();
    }
}

void METH(enablePort)(t_uint8 portIdx) {
#ifdef DEBUG_TRACE    
    OstTraceInt1(TRACE_DEBUG, "dmain: enablePort portIdx=%d", portIdx);
#endif 
    mMasterPortBuffers[portIdx].isRunning = true;
    mMasterPortBuffers[portIdx].data_droppped   = true;
    startDmaIfNeeded();

    dma_port_state[portIdx].portIsRunning();
}

void METH(disablePort)(t_uint8 portIdx) {
#ifdef DEBUG_TRACE    
    OstTraceInt1(TRACE_DEBUG, "dmain: disablePort portIdx=%d", portIdx);
#endif 
    mMasterPortBuffers[portIdx].isRunning = false;
    
    if(mMasterPortBuffers[portIdx].weHaveTheBuffer){
        returnMasterPort(portIdx);
    }

    stopDmaIfNeeded();
    dma_port_state[portIdx].portIsStopped();
}

void METH(stop)(void) {
    // stop any dma transfer if any
    if (mIsRxRunning) {
        stopRxTransfer(MSP_AB8500_RX);
    }
}
