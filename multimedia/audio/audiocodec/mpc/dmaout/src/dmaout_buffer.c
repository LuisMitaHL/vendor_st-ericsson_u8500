/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   dmaout_buffer.c
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/

#include "dmaout.h"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
  #include "audio_audiocodec_mpc_dmaout_src_dmaout_bufferTraces.h"
#endif

static t_uint48 mLastFilteredTimestamp;

static void consumeRingBuffer (void);

static inline void toggleRingSlotWrite(void){
    mRingBuffer.writeSlotIndex = (mRingBuffer.writeSlotIndex + 1) % NB_SLOTS_IN_RING_BUFFER;

    MASK_EOT_IT;
    mRingBuffer.filledSlots++;
    UNMASK_EOT_IT;

    if ((mRingBuffer.filledSlots >= NB_SLOTS_IN_ONE_BURST) && ( (mRingBuffer.writeSlotIndex % NB_SLOTS_IN_ONE_BURST) == 0)) {
        MASK_EOT_IT;
        mRingBuffer.filledBuffers++;
        UNMASK_EOT_IT;

#ifdef DEBUG_TRACE
        OstTraceInt1(TRACE_DEBUG, "dmaout: toggleRingSlotWrite increment filledBuffers %d %s", mRingBuffer.filledBuffers);
#endif
    }

}

static inline void produceJitterBuffer(void){
    MASK_EOT_IT;
    JitterBuffer.filledslots++;
    JitterBuffer.write_index = (JitterBuffer.write_index + 1) % JitterBuffer.length;
    UNMASK_EOT_IT;
}

void returnMasterPort(unsigned int portIdx) {
    
    Buffer_p buf;
    ASSERT(mSamplesPlayedPtr!=0);
	// update Samples Played memory zone
    mSamplesPlayedPtr[portIdx] ++;

#ifdef LATENCY_CONTROL
    // telephony latency control
    // TODO add test to check portidx connected to telephony graph
    OstTraceInt4(TRACE_RESERVED, "dmaout_buffer: latency control (dmaout, input) (timestamp = 0x%x 0x%x 0x%x us) (port %d)", mMasterPortBuffers[portIdx].buffer->nTimeStamph, (unsigned int)(mMasterPortBuffers[portIdx].buffer->nTimeStampl >> 24),(unsigned int)(mMasterPortBuffers[portIdx].buffer->nTimeStampl & 0xffffffu), portIdx);
#endif

#ifdef DEBUG_TRACE
    OstTraceInt1(TRACE_DEBUG, "dmaout_buffer: returnMasterPort: index=%d", portIdx);
#endif
    
    buf=mMasterPortBuffers[portIdx].buffer;
    buf->filledLen = 0;
    
    inputport[portIdx].fillThisBuffer(buf);
    mMasterPortBuffers[portIdx].weHaveTheBuffer = false;
    mMasterPortBuffers[portIdx].buffer          = NULL;
}

void returnFeedbackPort(unsigned int portIdx) {

#ifdef DEBUG_TRACE
    OstTraceInt1(TRACE_DEBUG, "dmaout_buffer: returnFeedbackPort: index=%d", portIdx);
#endif

    outputport[portIdx].emptyThisBuffer(mFeedbackPortBuffers[portIdx].buffer);

    mFeedbackPortBuffers[portIdx].weHaveTheBuffer = false;
    mFeedbackPortBuffers[portIdx].buffer          = NULL;
}

void fillBurstBufferWithZeros(void) {
    int i, nbSlotToFill = NB_SLOTS_IN_ONE_BURST - (mRingBuffer.filledSlots % NB_SLOTS_IN_ONE_BURST);

    int * buf = mRingBuffer.buffer + mRingBuffer.writeSlotIndex * BLOCKSIZE_STEREO;
#ifdef DEBUG_TRACE
    OstTraceInt3(TRACE_DEBUG, "dmaout: fillBurstBufferWithZeros: mRingBuffer.filledSlots %d, filledBuffers %d, nbSlotToFill %d", mRingBuffer.filledSlots, mRingBuffer.filledBuffers, nbSlotToFill);
#endif
#ifndef DO_NOT_USE_PROVIDED_BUFFERS
    for (i = 0; i < nbSlotToFill * BLOCKSIZE_STEREO; i++) {
        *buf++ = 0;
    }
#endif
    mRingBuffer.filledSlots    += nbSlotToFill;
    mRingBuffer.writeSlotIndex = (nbSlotToFill + mRingBuffer.writeSlotIndex)% NB_SLOTS_IN_RING_BUFFER;
    mRingBuffer.filledBuffers++;
}

Buffer_p getInputBuffer(int portIdx) {
    int i;
    Buffer_p buf;

    if (mMasterPortBuffers[portIdx].weHaveTheBuffer == false)
        return 0;

    buf = mMasterPortBuffers[portIdx].buffer;

    for (i = buf->filledLen; i < buf->allocLen; i++)
        buf->data[i] = 0;

    buf->filledLen = buf->allocLen;

    return buf;
}

static int copyToRingBuffer(int * buf) {
    int * bufout;
    int i;

#ifdef DEBUG_TRACE
    OstTraceInt2(TRACE_DEBUG, "dmaout: copyToRingBuffer writeSlotIndex %d, filledSlots %d", mRingBuffer.writeSlotIndex, mRingBuffer.filledSlots);
#endif

#ifndef DO_NOT_USE_PROVIDED_BUFFERS
    bufout = mRingBuffer.buffer + mRingBuffer.writeSlotIndex * BLOCKSIZE_STEREO;
    for (i = 0; i < BLOCKSIZE_STEREO; i++) {
        bufout[i] = buf[i] >> 4;
    }
#endif

    toggleRingSlotWrite();

    return BLOCKSIZE_STEREO;
}

void fillRingSlotWithZeros(void){
    int * bufout;
    int i;
#ifdef DEBUG_TRACE
    OstTraceInt2(TRACE_DEBUG, "dmaout: fillRingSlotWithZeros writeSlotIndex %d, filledSlots %d", mRingBuffer.writeSlotIndex, mRingBuffer.filledSlots);
#endif
#ifndef DO_NOT_USE_PROVIDED_BUFFERS
    bufout = mRingBuffer.buffer + mRingBuffer.writeSlotIndex * BLOCKSIZE_STEREO;
    for (i = 0; i < BLOCKSIZE_STEREO; i++) {
        bufout[i] = 0;
    }
#endif
    toggleRingSlotWrite();
}

void zeroJitterBufferStripe( int channels, int mspSlot, int shift_in_bits) {
    int i, j,k;
    int *bufout;
    int sample1,sample2;
    
    for (k=0; k<JitterBuffer.length; k++ )
    {
        bufout = JitterBuffer.buffer[k] + mspSlot;

        if (channels == 2) {
            for (i = 0; i < AB_BLOCKSIZE_MONO_1MS; i++) {
                *bufout++ = 0;
                *bufout = 0;
                bufout += mNbMspChannelEnabled - 1;
            }

        } else {
            ASSERT(channels == 1);
            for (i = 0; i < AB_BLOCKSIZE_MONO_1MS; i++) {
                sample1 = 0;
                bufout[mNbMspChannelEnabled*i] = sample1;
            }
        }
    }
}

void zeroJitterBufferSlot(t_uint16 idx) {
	int *zero;
    int i;

    zero = JitterBuffer.buffer[idx];
    for (i = 0; i < AB_BLOCKSIZE_MONO_1MS * mNbMspChannelEnabled; i++) {
        *zero = 0;
        zero++;
    }
}

static void expandZerosToJitterBuffer( int channels, int mspSlot, int shift_in_bits) {
    int i, idx;
    int *bufout;
    bufout = JitterBuffer.buffer[JitterBuffer.write_index] + mspSlot;

    if (channels == 2) {
        for (i = 0; i < AB_BLOCKSIZE_MONO_1MS; i++) {
            idx=2*i;
            bufout[mNbMspChannelEnabled*i] = 0;
            bufout[mNbMspChannelEnabled*i+1] = 0;
        }
    } else {
        ASSERT(channels == 1);
        for (i = 0; i < AB_BLOCKSIZE_MONO_1MS; i++) {
            bufout[mNbMspChannelEnabled*i] = 0;
        }
    }
}

static void expandToJitterBuffer(int * buf, int channels, int mspSlot, int shift_in_bits) {
    int i;
    int *bufout;
    bufout = JitterBuffer.buffer[JitterBuffer.write_index] + mspSlot;

#ifndef DO_NOT_USE_PROVIDED_BUFFERS
    if (channels == 2) {
        
        for (i = 0; i < AB_BLOCKSIZE_MONO_1MS; i++) {
            *bufout++ = (*buf++) >> shift_in_bits;
            *bufout = (*buf++) >> shift_in_bits;
            bufout += mNbMspChannelEnabled - 1;
        }
    } else {
        ASSERT(channels == 1);
        for (i = 0; i < AB_BLOCKSIZE_MONO_1MS; i++) {
            bufout[mNbMspChannelEnabled*i] = buf[i] >> shift_in_bits;
        }
    }
#endif
}

static void consumeRingBuffer (void) {
    
    ASSERT(mRingBuffer.filledSlots > 0);
    mRingBuffer.filledSlots--;

    if(mRingBuffer.filledBuffers > 0){
        if((mRingBuffer.readSlotIndex % NB_SLOTS_IN_ONE_BURST) == 0){
            mRingBuffer.filledBuffers--;
        }
    }

    mRingBuffer.readSlotIndex = (mRingBuffer.readSlotIndex + 1) % NB_SLOTS_IN_RING_BUFFER;
}

#define RAMP_STEPS (0x400)

void
start_fading(t_uint16 port_idx, bool fade_up) {

    portStatus_t *port= &mMasterPortBuffers[port_idx];

    if (port->muted)
        return;

    if (port->ramp_steps) {
        if ((port->ramp_inc > 0) && !fade_up) {

            port->ramp_steps = RAMP_STEPS - port->ramp_steps;

            if (!port->ramp_steps) {
                port->vol = port->ramp_inc;
                port->ramp_steps = 1;
            }
            port->ramp_inc = (-1) * port->ramp_inc;

        } else {
            return;
        }
    } else {
        port->ramp_steps = RAMP_STEPS;
        port->ramp_inc = 0x8000 / port->ramp_steps;

        if (fade_up) {
            port->vol = 0;
        } else {
            port->vol = 0x8000;
            port->ramp_inc = (-1) * port->ramp_inc;
        }
    }
}

static bool
apply_fading(t_uint16 port_idx, int *buf) {

    int i = 0;
    int j,k;
    bool ret = false;
    t_uint16 vol;
    int channels = mAccessoryInfo[port_idx].nb_channel;
    portStatus_t *port= &mMasterPortBuffers[port_idx];
#ifdef DEBUG_TRACE
    OstTraceInt3(TRACE_DEBUG, "dmaout_buffer.c: entering ramp_steps = %d    , ramp_inc = %d ,  vol = %d ", port->ramp_steps, port->ramp_inc, port->vol);
#endif
    if (port->ramp_steps) {

        vol = port->vol;

        if (port->ramp_steps < AB_BLOCKSIZE_MONO_1MS) {
            j = port->ramp_steps;
            port->ramp_steps = 0;
        } else {
            j = AB_BLOCKSIZE_MONO_1MS;
            port->ramp_steps -= AB_BLOCKSIZE_MONO_1MS;
        }

        for (i = 0; i < j; i++) {
            vol += port->ramp_inc;
            for (k = 0; k < channels; k++) {
                *buf = ((t_sint48)vol * (*buf)) >> 15;
                buf++;
            }
        }
        port->vol = vol;

        if (!port->ramp_steps && (port->ramp_inc < 0)) {
            port->muted = true;
            ret = true;
#ifdef DEBUG_TRACE
            OstTraceInt3(TRACE_DEBUG, "dmaout_buffer.c: reached last ifcase ramp_steps = %d    , ramp_inc = %d ,  vol = %d ", port->ramp_steps, port->ramp_inc, port->vol);
#endif
        }
    }

    if (port->muted)
        for (i; i < AB_BLOCKSIZE_MONO_1MS; i++)
            for (k = 0; k < channels; k++)
                *buf++ = 0;

    return ret;
}
void copySlotFromRingBufferToJitterBuffer(t_uint16 port_idx) {

    if (mRingBuffer.filledSlots == 0) {
        done_fading(port_idx);
        start_fading(port_idx, true);
        fillRingSlotWithZeros();
        mRingbufferXruns++;
    }

    expandToJitterBuffer(mRingBuffer.buffer + mRingBuffer.readSlotIndex * BLOCKSIZE_STEREO, 2, AV_MSPSLOT, 0);
    expandToJitterBuffer(mRingBuffer.buffer + mRingBuffer.readSlotIndex * BLOCKSIZE_STEREO, 2, IHF_MSPSLOT, 0);

    consumeRingBuffer();
    produceJitterBuffer();
#ifdef DEBUG_TRACE
    OstTraceInt2(TRACE_DEBUG, "dmaout: end of copySlotFromRingBufferToDoubleBuffer mRingBuffer.readSlotIndex %d,  mDoubleBuffer.writeIndex  %d ", mRingBuffer.readSlotIndex, JitterBuffer.write_index);
#endif
}

bool processInputBuffersBurst(t_uint16 port_idx) {
    Buffer_p buf = getInputBuffer(port_idx);
    int *data;
    int *fragment;

    if (buf != 0) {
        bool muted;
        data = (buf->data);
        fragment = &data[mMasterPortBuffers[port_idx].expandedFragments * BLOCKSIZE_STEREO];
        muted = apply_fading(port_idx, fragment);
	if (muted) {
	    t_uint16 filledslots;
	    MASK_EOT_IT;
	    filledslots = JitterBuffer.filledslots;
	    filledslots += mRingBuffer.filledSlots;
	    UNMASK_EOT_IT;
	    setup_mute_cb(port_idx, filledslots + 1);
	}
        copyToRingBuffer(fragment);
        mMasterPortBuffers[port_idx].expandedFragments++;

        if (mMasterPortBuffers[port_idx].expandedFragments == 5) {
            returnMasterPort(port_idx);
        }
        return true;
    }
    return false;
}
static int startup_counter[8];

void processBuffer(unsigned int port_index) {
    Buffer_p buf = getInputBuffer(port_index);
    int *data;
    int *fragment;
    bool insert_silence = true;
    
    
    if(buf) {
#ifdef DEBUG_TRACE
        OstTraceInt2(TRACE_DEBUG, "dmaout : processing buf =0x%x portIdx=%d",buf,port_index);
#endif
        if (startup_counter[port_index] > 0) {
            startup_counter[port_index]--;

            if(!startup_counter[port_index])
                start_fading(port_index, true);
        } else {
            insert_silence = false;
        }
    } else {

        if ( startup_counter[port_index] == 0) {
#ifdef DEBUG_TRACE
            OstTraceInt1(TRACE_WARNING, "dmaout: XRUN: Sink: %u \n",port_index);
#endif
        }
        done_fading(port_index);
        startup_counter[port_index] = 4;

#ifdef DEBUG_TRACE
        OstTraceInt1(TRACE_DEBUG, "dmaout_buffer : inserting jitter margin on portIdx=%d",port_index);
#endif
    }

    if ( !insert_silence) {
        bool muted;
        int bits_to_shift = 4;
	if (port_index == FMTX_INPUT_PORT_IDX) {
            // The FM signal MSB sent to the CG2900 must always be zero
            bits_to_shift = 5;
        }

	data = buf->data;
        fragment = &data[mMasterPortBuffers[port_index].expandedFragments * AB_BLOCKSIZE_MONO_1MS * mAccessoryInfo[port_index].nb_channel];

        muted = apply_fading(port_index, fragment);

	if (muted) {
	    t_uint16 filledslots;
	    MASK_EOT_IT;
	    filledslots = JitterBuffer.filledslots;
	    UNMASK_EOT_IT;
	    setup_mute_cb(port_index, filledslots + 1);
	}

        expandToJitterBuffer( fragment, mAccessoryInfo[port_index].nb_channel, mAccessoryInfo[port_index].mspslot, bits_to_shift);
        
	if ( port_index == IHF_INPUT_PORT_IDX && !mMasterPortBuffers[AVSINK_INPUT_PORT_IDX].isRunning && actualLowPowerState == BURST_MODE) {
            expandToJitterBuffer( fragment, mAccessoryInfo[port_index].nb_channel, mAccessoryInfo[AVSINK_INPUT_PORT_IDX].mspslot, bits_to_shift);
        }

        mMasterPortBuffers[port_index].expandedFragments++;

        if (mMasterPortBuffers[port_index].expandedFragments == 5) {
            returnMasterPort(port_index);
        }

        mAccessoryInfo[port_index].flags         = buf->flags;
    }
    else {
#ifdef DEBUG_TRACE
        OstTraceInt1(TRACE_DEBUG, "dmaout_buffer : expanding zeros on portIdx=%d",port_index);
#endif
        if ( actualLowPowerState == BURST_MODE) {
            expandZerosToJitterBuffer( mAccessoryInfo[IHF_INPUT_PORT_IDX].nb_channel, mAccessoryInfo[IHF_INPUT_PORT_IDX].mspslot, 4);
            expandZerosToJitterBuffer( mAccessoryInfo[AVSINK_INPUT_PORT_IDX].nb_channel, mAccessoryInfo[AVSINK_INPUT_PORT_IDX].mspslot, 4);
        } else {
            expandZerosToJitterBuffer( mAccessoryInfo[port_index].nb_channel, mAccessoryInfo[port_index].mspslot, 4);
        }
    }
}

bool inputdata_available(unsigned int port_index) {
    Buffer_p buf = getInputBuffer(port_index);
    if (buf)
        return true;
    else
        return false;
}

bool processInputBuffers(bool force_output) {
    int i;
    bool all_inputs_ready = true;
    bool all_outputs_ready = true;

    for (i = 0; i < NB_INPUT_PORTS ; i++) {
        if(mMasterPortBuffers[i].isRunning && !inputdata_available(i))
            all_inputs_ready = false;
    }

    for (i = 0; i < NB_OUTPUT_PORTS; i++) {
        if(mFeedbackPortBuffers[i].isRunning && !mFeedbackPortBuffers[i].weHaveTheBuffer)
            all_outputs_ready = false;
    }
    
    if ( force_output || (all_inputs_ready && all_outputs_ready) ) {
        for (i = 0; i < NB_INPUT_PORTS ; i++) {
            if(mMasterPortBuffers[i].isRunning)
                processBuffer(i);
        }
        processOutputBuffers();
        produceJitterBuffer();
    }
    
    return force_output || (all_inputs_ready && all_outputs_ready);
}

void METH(emptyThisBuffer)(Buffer_p buffer, t_uint8 portIdx) {
    PRECONDITION(buffer->filledLen == buffer->allocLen
            || (buffer->flags & BUFFERFLAG_EOS)
            || buffer->filledLen == 0);

#ifdef DEBUG_TRACE
    OstTraceInt2(TRACE_DEBUG, "dmaout:emptyThisBuffer  portIdx=%d buffer->allocLen=%d", portIdx, buffer->allocLen);
#endif
    ASSERT(buffer->allocLen == (5*48*2) || buffer->allocLen == (5*48));

    mMasterPortBuffers[portIdx].weHaveTheBuffer = true;
    mMasterPortBuffers[portIdx].buffer          = buffer;
    mMasterPortBuffers[portIdx].expandedFragments = 0;

    if(mMasterPortBuffers[portIdx].isRunning){
        scheduleProcessEvent();
    }
    else{
        returnMasterPort(portIdx);
    }
}

void METH(fillThisBuffer)(Buffer_p buffer, t_uint8 portIdx) {
#ifdef DEBUG_TRACE
    OstTraceInt2(TRACE_DEBUG, "dmaout:fillThisBuffer  portIdx=%d buffer->allocLen=%d", portIdx, buffer->allocLen);
#endif
    mFeedbackPortBuffers[portIdx].weHaveTheBuffer = true;
    mFeedbackPortBuffers[portIdx].buffer          = buffer;
    mFeedbackPortBuffers[portIdx].expandedFragments = 0;

    scheduleProcessEvent();

    if(! mFeedbackPortBuffers[portIdx].isRunning){
        returnFeedbackPort(portIdx);
    }
}

//////////////////////////
// Feedback Port
//////////////////////////
Buffer_p getOutputBuffer(int portIdx) {
#ifdef DEBUG_TRACE
    OstTraceInt2(TRACE_DEBUG, "dmaout:getOutputBuffer portIdx=%d weHaveTheBuffer=%d", portIdx, mFeedbackPortBuffers[portIdx].weHaveTheBuffer);
#endif
    if (mFeedbackPortBuffers[portIdx].weHaveTheBuffer == false) {
        return 0;
    }
    return mFeedbackPortBuffers[portIdx].buffer;
}

static int copyFromJitterBufferToFeedback(int * feedbackbuf, int channels, int mspSlot, int shift_in_bits) {
    int i, idx;
    int sample1,sample2;
    int * bufout = JitterBuffer.buffer[JitterBuffer.write_index] + mspSlot;

    if (channels == 2) {
        for (i = 0; i < AB_BLOCKSIZE_MONO_1MS; i++) {
            idx=2*i;
            feedbackbuf[idx]   = bufout[mNbMspChannelEnabled*i]   << shift_in_bits;
            feedbackbuf[idx+1] = bufout[mNbMspChannelEnabled*i+1] << shift_in_bits;
        }
    } else {
        ASSERT(channels == 1);
        for (i = 0; i < AB_BLOCKSIZE_MONO_1MS; i++) {
            feedbackbuf[i] = bufout[mNbMspChannelEnabled*i] << shift_in_bits;
        }
    }
    return channels*AB_BLOCKSIZE_MONO_1MS;
}

void processFeedbackBuffer(unsigned int portIdx){
    int *data;
    int *fragment;
    
    if(mFeedbackPortBuffers[portIdx].isRunning){
        Buffer_p buf = getOutputBuffer(portIdx);
#ifdef DEBUG_TRACE
        OstTraceInt2(TRACE_DEBUG, "dmaout:processFeedbackBuffer buf=0x%x, portidx=%d",buf,portIdx);
#endif
        if(buf) {
            data = (buf->data);
            fragment = &data[mFeedbackPortBuffers[portIdx].expandedFragments * AB_BLOCKSIZE_MONO_1MS * mAccessoryInfo[portIdx].nb_channel];
            buf->filledLen += copyFromJitterBufferToFeedback(fragment, mAccessoryInfo[portIdx].nb_channel, mAccessoryInfo[portIdx].mspslot, 4);
            
            if (!mFeedbackPortBuffers[portIdx].expandedFragments) {
                buf->flags       = mAccessoryInfo[portIdx].flags & ~BUFFERFLAG_EOS;

                // As of now, always set the STARTTIME flag
                mFeedbackPortBuffers[portIdx].data_droppped = false;
            }

            mFeedbackPortBuffers[portIdx].expandedFragments++;

            if (mFeedbackPortBuffers[portIdx].expandedFragments == 5) {
                t_uint48 clock_fb   = *mTimeStampPtr;

                // might be negative if parts have already been played
                int delay = ((int) JitterBuffer.filledslots - 5) * 1000;
                t_uint48 playout_ts = clock_fb + delay;

                // filter to avoid jitter.. jitter confuses time_align component
                t_uint48 fb_delivered_diff = playout_ts - mLastFilteredTimestamp;

                buf->flags       |= BUFFERFLAG_STARTTIME;
                buf->nTimeStamph  = 0;
                buf->nTimeStampl  = playout_ts;
                if (fb_delivered_diff >= 4000 && fb_delivered_diff <= 6000) {
                    buf->nTimeStampl = mLastFilteredTimestamp + 5000;
                }
                mLastFilteredTimestamp = buf->nTimeStampl;

#ifdef DEBUG_TRACE
                OstTraceInt3(TRACE_DEBUG, "DMAOUT feedback %d (original %d, clock %d) return buffer",(int)  buf->nTimeStampl, (int) playout_ts, (int) clock_fb);
#endif

                returnFeedbackPort(portIdx);
            }
        }
        else
        {
            mFeedbackPortBuffers[portIdx].data_droppped = true;
        }
    }
}

void processOutputBuffers(void){
    processFeedbackBuffer(AVSINK_INPUT_PORT_IDX);
    processFeedbackBuffer(IHF_INPUT_PORT_IDX);
    processFeedbackBuffer(EAR_INPUT_PORT_IDX);
}

void METH(enablePort)(t_uint8 portIdx) {
#ifdef DEBUG_TRACE
    OstTraceInt1(TRACE_DEBUG, "dmaout: enablePort portIdx=%d", portIdx);
#endif
    zeroJitterBufferStripe(mAccessoryInfo[portIdx].nb_channel, mAccessoryInfo[portIdx].mspslot,4);
    mMasterPortBuffers[portIdx].isRunning = true;
    mMasterPortBuffers[portIdx].muted = false;
    startup_counter[portIdx] = 4;
 
    startDmaIfNeeded();

    dma_port_state[portIdx].portIsRunning();

}

void METH(disablePort)(t_uint8 portIdx) {
#ifdef DEBUG_TRACE
    OstTraceInt1(TRACE_DEBUG, "dmaout: disablePort portIdx=%d", portIdx);
 #endif
    mMasterPortBuffers[portIdx].isRunning = false;
    done_fading(portIdx);
    zeroJitterBufferStripe(mAccessoryInfo[portIdx].nb_channel, mAccessoryInfo[portIdx].mspslot,4);
    if(mMasterPortBuffers[portIdx].weHaveTheBuffer){
        returnMasterPort(portIdx);
    }

    if(stopDmaIfNeeded(portIdx)){
        dma_port_state[portIdx].portIsStopped();
    }
    else{
        mMemorizedPortIdxForIdle = portIdx;
    }
}

void METH(feedback_enablePort)(t_uint8 portIdx) {
#ifdef DEBUG_TRACE
    OstTraceInt1(TRACE_DEBUG, "dmaout: feedback_enablePort portIdx=%d", portIdx);
#endif
    mFeedbackPortBuffers[portIdx].isRunning = true;
}

void METH(feedback_disablePort)(t_uint8 portIdx) {
#ifdef DEBUG_TRACE
    OstTraceInt1(TRACE_DEBUG, "dmaout: feedback_disablePort portIdx=%d", portIdx);
#endif
    mFeedbackPortBuffers[portIdx].isRunning = false;

    if(mFeedbackPortBuffers[portIdx].weHaveTheBuffer){
        returnFeedbackPort(portIdx);
    }
}
