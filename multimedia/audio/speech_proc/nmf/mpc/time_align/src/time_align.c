/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   time_align.c
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include <speech_proc/nmf/mpc/time_align.nmf>
#include "libeffects/libresampling/include/resample.h"
#include "time_align_common.h"
#include "fsm/component/include/Component.inl"
#include <stdbool.h>
#include <dbc.h>

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "speech_proc_nmf_mpc_time_align_src_time_alignTraces.h"
#endif

////////////////////////////////////////////////////////////
//          Global Variables
////////////////////////////////////////////////////////////
// component fsm anf ports fifo
static void *       mFifoIn[1];
static void *       mFifoRefIn[1];
static void *       mFifoOut[1];
static Port         mPorts[3];
static Component    mComponent;

// src variables
static void *           mHeap;
static t_uint16         mHeapSize;
static ResampleContext  mResampleContext;

// Uplink input port
static t_uint24        mIsConfigured = 0;
static t_sample_freq   mSampleFreq   = FREQ_UNKNOWN;
static t_uint24        mChansNb      = 0;
static t_uint24        mOutputPart   = 0;

// Reference input port
static t_uint24        mIsRefConfigured = 0;
static t_sample_freq   mRefSampleFreq   = FREQ_UNKNOWN;
static t_uint24        mRefChansNb      = 1;     //always send reference data to processing lib (even if it is only zeros)
static t_bool          mRealTimeReference = TRUE;  // set to false only for testing
static t_ref_status    mRefStatus       = REFERENCE_INACTIVE;  // Dynamic flag.
static t_uint48        mTimeStamp;      // timestamp of the last sample received on reference port in us. (i.e. time stamp of the last buffer + buffer duration)
static t_align_status  mAlignmentStatus = TIME_ALIGNMENT_NOT_STARTED;

// ring Buffer
static t_sword *       mRingBuffer;
static t_sword *       mRingWritePtr;
static t_sword *       mRingReadPtr;
static t_uint24        mRingBufferSize;
static t_uint24        mRingUsedSize;  // in samples
static t_uint24        mRingFreeSize;  // in samples

static t_uint24        mSingleInputBufferSize; // in samples for UL_INPUT_BUFFER_DURATION ms per channel
static t_uint24        mOutputBufferSize;      // in samples for PROCESSING_BUFFER_DURATION ms for one channel
static t_uint24        mRefUs2samples;
static t_uint24        mRefSamplesIn2Ms;
static t_uint24        mRefSamplesOut2Ms;


// internals functions
static void   reconfigureSrc(void);
static t_bool processRefBuffer(Buffer_p buffer);
static t_bool referenceAvailable(void);
static void   packOutputBuffer(Buffer_p pInputBuf, Buffer_p pOutputBuf);
static void   readFromRingBuffer(t_sword *dst, t_uint24 size, t_uint48 timestamp);
static void   completeOutputBufferWithZeroPadding(t_sword * ptr, t_uint24 offset);

////////////////////////////////////////////////////////////
//   COMPONENT methods
////////////////////////////////////////////////////////////
static void process(Component *this)
{
  Buffer_p  pInputBuf   = 0;
  Buffer_p  pOutputBuf  = 0;
  int       nbInputBuf  = Port_queuedBufferCount(&mPorts[INPUT_PORT]);
  int       nbRefBuf    = Port_queuedBufferCount(&mPorts[REFERENCE_PORT]);
  int       nbOutputBuf = Port_queuedBufferCount(&mPorts[OUTPUT_PORT]);

  OstTraceFiltInst3(TRACE_DEBUG, "Time align enter: nbInputBuf = %d, nbRefBuf = %d, nbOutputBuf = %d", nbInputBuf, nbRefBuf, nbOutputBuf);

  if ((nbRefBuf)&&
	  ((mRealTimeReference) ||
	   (mRingFreeSize > 0)))
  {
    nbRefBuf --;
	if(!mIsRefConfigured)
	{
	  Port_dequeueAndReturnBuffer(&mPorts[REFERENCE_PORT]);
	}
	else
	{
	  Buffer_p pRefBuf = 0;
	  pRefBuf = Port_getBuffer(&mPorts[REFERENCE_PORT], 0);
	  if(processRefBuffer(pRefBuf))
	  {
		Port_dequeueAndReturnBuffer(&mPorts[REFERENCE_PORT]);
	  }
	  else
	  {
		nbRefBuf++;
	  }
	}
  }

  if (mIsConfigured &&
      nbInputBuf    &&
      nbOutputBuf   &&
      referenceAvailable())
  {
    pInputBuf  = Port_getBuffer(&mPorts[INPUT_PORT],0);
    pOutputBuf = Port_getBuffer(&mPorts[OUTPUT_PORT],0);

    packOutputBuffer(pInputBuf, pOutputBuf);

    Port_dequeueAndReturnBuffer(&mPorts[INPUT_PORT]);
    nbInputBuf --;

    if(mOutputPart == (NB_OUTPUT_PART - 1))
    {
      if (pOutputBuf->flags & BUFFERFLAG_EOS)
      {
        proxy.eventHandler(OMX_EventBufferFlag, INPUT_PORT, pOutputBuf->flags);
      }

      OstTraceFiltInst3 (TRACE_DEBUG, "Speech_proc/time_align: latency control (time_align, output) (timestamp = 0x%x 0x%x 0x%x us)", pOutputBuf->nTimeStamph, (unsigned int)(pOutputBuf->nTimeStampl >> 24), (unsigned int)(pOutputBuf->nTimeStampl & 0xffffffu));
      Port_dequeueAndReturnBuffer(&mPorts[OUTPUT_PORT]);
      nbOutputBuf --;
      mOutputPart = 0;
    }
    else
    {
      mOutputPart ++;
    }
  }

  OstTraceFiltInst3(TRACE_DEBUG, "Time align exit: nbInputBuf = %d, nbRefBuf = %d, nbOutputBuf = %d", nbInputBuf, nbRefBuf, nbOutputBuf);
}

static void resetRingBuffer(void)
{
  mRingWritePtr = mRingBuffer;
  mRingReadPtr  = mRingBuffer;
  mRingUsedSize = 0;
  mRingFreeSize = mRingBufferSize;
}

static void resetTimeAlignment(void)
{
  TRACE_t * this = (TRACE_t *)&mComponent;
  OstTraceFiltInst0(TRACE_DEBUG, "Reset mRefStatus and mAlignmentStatus");
  mTimeStamp       = 0;
  mRefStatus       = REFERENCE_INACTIVE;
  mAlignmentStatus = TIME_ALIGNMENT_NOT_STARTED;
}

static void resetReferencePort(void)
{
  mRefSampleFreq    = FREQ_UNKNOWN;
  mRefChansNb       = 1;
  mIsRefConfigured  = 0;
}

static void resetInputPort(void)
{
  // do not reset mSampleFreq, as it can still be used by processRefBuffer
  // (in case mSampleFreq == mRefSampleFreq)
  mIsConfigured	   = 0;
  mChansNb		   = 0;
  mOutputPart	   = 0;
  mAlignmentStatus = TIME_ALIGNMENT_NOT_STARTED;
}

static void reset(Component *this)
{
  resetRingBuffer();
  resetTimeAlignment();
  // do NOT call resetInputPort and resetReferencePort as reset
  // will be called AFTER setConfig if port are enabled when going to idle !!
}

static void disablePortIndication(t_uint32 portIdx)
{
  if(portIdx == REFERENCE_PORT)
  {
    resetRingBuffer();
    resetTimeAlignment();
    resetReferencePort();
  }
  else if(portIdx == INPUT_PORT)
  {
    resetInputPort();
  }
  else if(portIdx == OMX_ALL)
  {
    resetRingBuffer();
    resetTimeAlignment();
    resetReferencePort();
    resetInputPort();
  }
}

static void enablePortIndication(t_uint32 portIdx)
{
}

static void flushPortIndication(t_uint32 portIdx)
{
}


////////////////////////////////////////////////////////////
// Provided Interfaces
////////////////////////////////////////////////////////////
void METH(setParameter)(t_uint24 shared_buffer_header, t_uint24 ring_buffer, t_uint24 heap,
						t_uint24 heap_size, t_uint16 real_time_ref, t_uint24 ring_buffer_size)
{
  mFifoOut[0]        = (void *)shared_buffer_header;
  mHeapSize          = heap_size;
  mHeap              = (void *)heap;
  mRingBuffer        = (t_sword *)ring_buffer;
  mRealTimeReference = (t_bool)real_time_ref;
  mRingBufferSize    = ring_buffer_size;
}

void METH(setConfigRef)(t_uint16 in_freq, t_uint16 out_freq, t_uint16 nb_channel_ref)
{
  t_bool   reconfigure_src = FALSE;

  if (mRefChansNb != nb_channel_ref) reconfigure_src = TRUE;
  mRefChansNb = nb_channel_ref;

  if(out_freq == 8000)
  {
    if (mSampleFreq != FREQ_8KHZ) reconfigure_src = TRUE;
    mSampleFreq   = FREQ_8KHZ;
    mRefUs2samples = us2samples_8khz * nb_channel_ref;
    mRefSamplesOut2Ms = samples2ms_8khz;
    if (nb_channel_ref == 2) mRefSamplesOut2Ms >>= 1;
  }
  else if(out_freq == 16000)
  {
    if (mSampleFreq != FREQ_16KHZ) reconfigure_src = TRUE;
    mSampleFreq   = FREQ_16KHZ;
    mRefUs2samples = us2samples_16khz * nb_channel_ref;
    mRefSamplesOut2Ms = samples2ms_16khz;
    if (nb_channel_ref == 2) mRefSamplesOut2Ms >>= 1;
  }
  else
    ASSERT(0);

  if(in_freq == 8000)
  {
    if (mRefSampleFreq != FREQ_8KHZ) reconfigure_src = TRUE;
    mRefSampleFreq = FREQ_8KHZ;
    mRefSamplesIn2Ms = samples2ms_8khz;
    if (nb_channel_ref == 2) mRefSamplesIn2Ms >>= 1;
  }
  else if(in_freq == 16000)
  {
    if (mRefSampleFreq != FREQ_16KHZ) reconfigure_src = TRUE;
    mRefSampleFreq = FREQ_16KHZ;
    mRefUs2samples = us2samples_16khz * nb_channel_ref;
    mRefSamplesIn2Ms = samples2ms_16khz;
    if (nb_channel_ref == 2) mRefSamplesIn2Ms >>= 1;
  }
  else if(in_freq == 48000)
  {
    if (mRefSampleFreq != FREQ_48KHZ) reconfigure_src = TRUE;
    mRefSampleFreq = FREQ_48KHZ;
    mRefSamplesIn2Ms = samples2ms_48khz;
    if (nb_channel_ref == 2) mRefSamplesIn2Ms >>= 1;
  }
  else
    ASSERT(0);

  if(reconfigure_src == TRUE)
  {
    reconfigureSrc();
  }

  mIsRefConfigured = 1;
}


void METH(setConfig)(t_uint16 freq, t_uint16 nb_channel)
{
  t_uint16 sample_per_ms = 0;

  mChansNb = nb_channel;

  if(freq == 8000)
  {
    mSampleFreq   = FREQ_8KHZ;
    sample_per_ms = 8;
  }
  else if(freq == 16000)
  {
    mSampleFreq   = FREQ_16KHZ;
    sample_per_ms = 16;
  }
  else
    ASSERT(0);

  // update buffer size
  mSingleInputBufferSize = sample_per_ms * UL_INPUT_BUFFER_DURATION;
  mOutputBufferSize      = (mChansNb + mRefChansNb) * sample_per_ms * PROCESSING_BUFFER_DURATION;

  mIsConfigured = 1;

  if(Port_queuedBufferCount(&mPorts[INPUT_PORT]) &&
     Port_queuedBufferCount(&mPorts[OUTPUT_PORT]))
  {
    Component_scheduleProcessEvent(&mComponent);
  }
}


void METH(fsmInit)(fsmInit_t initFsm)
{
  // trace init (mandatory before trace init)
  FSM_traceInit(&mComponent, initFsm.traceInfoAddr, initFsm.id1);

  Port_init(&mPorts[OUTPUT_PORT], OutputPort, true, false, 0, &mFifoOut, 1, &outputport, OUTPUT_PORT,
            (initFsm.portsDisabled & (1 << (OUTPUT_PORT))), (initFsm.portsTunneled & (1 << (OUTPUT_PORT))), &mComponent);
  Port_init(&mPorts[INPUT_PORT], InputPort, false, false, 0, &mFifoIn, 1, &inputport[INPUT_PORT], INPUT_PORT,
            (initFsm.portsDisabled & (1 << (INPUT_PORT))), (initFsm.portsTunneled & (1 << (INPUT_PORT))), &mComponent);
  Port_init(&mPorts[REFERENCE_PORT], InputPort, false, false, 0, &mFifoRefIn, 1, &inputport[REFERENCE_PORT], REFERENCE_PORT,
            (initFsm.portsDisabled & (1 << (REFERENCE_PORT))), (initFsm.portsTunneled & (1 << (REFERENCE_PORT))), &mComponent);

  mComponent.process               = process;
  mComponent.reset                 = reset;
  mComponent.disablePortIndication = disablePortIndication;
  mComponent.enablePortIndication  = enablePortIndication;
  mComponent.flushPortIndication   = flushPortIndication;

  Component_init(&mComponent, 3, mPorts, &proxy);
}

void METH(setTunnelStatus) (t_sint16 portIdx, t_uint16 isTunneled) {
  Port_setTunnelStatus(&mPorts[portIdx], isTunneled);
}

void METH(processEvent)() {
  Component_processEvent(&mComponent);
}

void METH(emptyThisBuffer)(Buffer_p buf, t_uint8 idx)
{
  Component_deliverBuffer(&mComponent, idx, buf);
}

void METH(fillThisBuffer)(Buffer_p buf)
{
  Component_deliverBuffer(&mComponent, OUTPUT_PORT, buf);
}

void METH(sendCommand)(OMX_COMMANDTYPE cmd, t_uword param)
{
  Component_sendCommand(&mComponent, cmd, param);
}

void METH(newFormat)(t_sample_freq freq, t_uint16 chans_nb, t_uint16 sample_bitsize, t_uint8 idx)
{
  PRECONDITION(sample_bitsize == 24);
  if(idx == INPUT_PORT)
  {
    PRECONDITION((mSampleFreq == FREQ_UNKNOWN) || (mSampleFreq == freq));
  }
  else
  {
    PRECONDITION((mRefSampleFreq == FREQ_UNKNOWN) || (mRefSampleFreq == freq));
  }
}


///////////////////////////////////////////////////////////
// Helper function
////////////////////////////////////////////////////////////
// reconfigure SRC
static void reconfigureSrc(void)
{
  int      status;
  t_uint16 required_size;

  // assure we have enough heap
  resample_calc_max_heap_size_fixin_fixout(mRefSampleFreq, mSampleFreq, SRC_MODE, &required_size, 0, mRefChansNb, 0);
  ASSERT(required_size <= mHeapSize);

  // re-init src lib
  status = resample_x_init_ctx_low_mips_fixin_fixout(mHeap, mHeapSize, &mResampleContext, mRefSampleFreq,
                                                     mSampleFreq, SRC_MODE, mRefChansNb, 0);
  POSTCONDITION(status == 0);

  // in case sample freq are the same, no sure if mResampleContext has been updated.
  // just in case update used fields.
  if(mSampleFreq == mRefSampleFreq)
  {
	if(mSampleFreq == FREQ_8KHZ)
	{
	  mResampleContext.fout_khz = 8;
	  mResampleContext.fin_khz  = 8;
	}
	else if (mSampleFreq == FREQ_16KHZ)
	{
	  mResampleContext.fout_khz = 16;
	  mResampleContext.fin_khz  = 16;
	}
	else
	{
	  POSTCONDITION(0);
	}
  }
}

// update used/free size
static inline
void adaptRingUsedSize(t_uint24 delta)
{
  mRingUsedSize += delta;
  mRingFreeSize -= delta;

  if(mRingUsedSize > mRingBufferSize)
  {
    TRACE_t * this = (TRACE_t *)&mComponent;
	if (mAlignmentStatus != TIME_ALIGNMENT_NOT_STARTED)
	  OstTraceFiltInst0(TRACE_WARNING, "Time align : Overflow in reference buffer ");
    mRingUsedSize = mRingBufferSize;
    mRingFreeSize = 0;
    mRingReadPtr  = mRingWritePtr;
  }
}

// process one buffer on reference port :
static t_bool processRefBuffer(Buffer_p buffer)
{
  t_uint24 i;
  t_uint24 nb_input_ms;
  t_uint24 nb_output_samples;
  t_uint48 timestamp   = buffer->nTimeStampl;
  t_sword __circ * dst_ptr;
  TRACE_t * this = (TRACE_t *)&mComponent;

  OstTraceFiltInst2(TRACE_DEBUG, "Time Align: ProcessRefBuffer , timestamp = 0x%x%6x", (t_uint24)((buffer->nTimeStampl>>24)&0xFFFFFFU),  (t_uint24)(buffer->nTimeStampl&0xFFFFFFU));

  // 1. compute input buffer size in ms and output size in samples
  nb_input_ms       = wfmulr((t_uint24)buffer->filledLen, mRefSamplesIn2Ms);
  nb_output_samples = nb_input_ms * mResampleContext.fout_khz * mRefChansNb;

  // 2. If time alignment is in use, data received on reference port may not be contiguous
  //    If so, "lost" data are replaced by 0
  //    TODO : use better values that 0
  if((mRefStatus == REFERENCE_ACTIVE_TIMESTAMP) &&
     (mTimeStamp < timestamp))
  {
    t_uint48 delta_timestamp = timestamp - mTimeStamp;
    t_uint24 delta_sample    = wfmulr((t_uint24)delta_timestamp, mRefUs2samples);

    OstTraceFiltInst3(TRACE_WARNING, "Time align : %d samples lost in reference data flow (buf %d, ring %d) . replaced with 0", delta_sample, (t_uint24)timestamp, (t_uint24)mTimeStamp);

	if(delta_sample > mRingBufferSize) delta_sample = mRingBufferSize;

	if(!mRealTimeReference             &&
	   (delta_sample  >  mRingFreeSize) &&
	   (mRingUsedSize >= (UL_INPUT_BUFFER_DURATION * mResampleContext.fout_khz * mRefChansNb)))
	  return FALSE;

	dst_ptr  = winit_circ_ptr(mRingWritePtr, mRingBuffer,(mRingBuffer + mRingBufferSize));
#pragma loop maxitercount(MAX_LOOP_COUNT)
	for(i=0;i<delta_sample;i++)
	{
	  *dst_ptr++ = 0;
	}
	mRingWritePtr = (t_sword *)dst_ptr;
	adaptRingUsedSize(delta_sample);
	mTimeStamp = timestamp;
  }

  // in non realtime mode, test if we have enough space in ring buffer not to erase not yet read data
  if(!mRealTimeReference && ( nb_output_samples > mRingFreeSize)) return FALSE;

  // 3. do samplerate conversion
  if(mSampleFreq == mRefSampleFreq)
  {
    t_sword *src_ptr = buffer->data;
    dst_ptr  = winit_circ_ptr(mRingWritePtr, mRingBuffer,(mRingBuffer + mRingBufferSize));

    // 3.1.1 simply recopy data if no src needed
#pragma loop maxitercount(MAX_LOOP_COUNT)
    for(i = 0; i < buffer->filledLen; i++)
    {
      *dst_ptr++ = *src_ptr++;
    }

    // 3.1.2 update ring buffer size
    adaptRingUsedSize((t_uint24)buffer->filledLen);

    // 3.1.3 update write buffer for next buffer
    mRingWritePtr = (t_sword *)dst_ptr;
  }
  else
  {
    t_uint16 consumed = 0;
    t_uint16 produced = 0;
    t_uint16 flush    = 0;
	t_uint16 res      = 0;
    t_uint16 remaining_aligned_ms;
    t_uint16 src_size_1 = nb_input_ms; // size in ms
    t_uint16 src_size_2 = 0;           // size in ms

    // 3.2.1 check how many "aligned" size remains in ringbuffer.
    //       if it is not enough then we need to divide samplerateocnverion into 2 calls
    remaining_aligned_ms = mRingBufferSize - ((t_uint24)mRingWritePtr - (t_uint24)mRingBuffer);
    remaining_aligned_ms = wfmulr(remaining_aligned_ms, mRefSamplesOut2Ms);
    if(remaining_aligned_ms < nb_input_ms)
    {
      // need to split samplerateocnverion into 2 calls
      src_size_1 = remaining_aligned_ms;
      src_size_2 = nb_input_ms - src_size_1;
    }

	consumed = src_size_1;
	produced = 0;
	flush    = 0;

	res = (*mResampleContext.processing) (&mResampleContext, buffer->data, mRefChansNb,
										  &consumed, mRingWritePtr, &produced, &flush);

	POSTCONDITION(res == 0);
	POSTCONDITION(consumed == src_size_1);
	POSTCONDITION(produced == src_size_1);

    if(src_size_2)
    {
      t_uint16 offset = src_size_1 * mResampleContext.fin_khz;
	  consumed = src_size_2;
	  produced = 0;
	  flush    = 0;

	  res = (*mResampleContext.processing) (&mResampleContext, &buffer->data[offset], mRefChansNb,
											&consumed, mRingBuffer, &produced, &flush);

	  POSTCONDITION(res == 0);
	  POSTCONDITION(consumed == src_size_2);
	  POSTCONDITION(produced == src_size_2);
    }

    // 3.2.2 update ring buffer size
    adaptRingUsedSize(nb_output_samples);

    // 3.2.3 update write buffer for next buffer
    dst_ptr  = winit_circ_ptr(mRingWritePtr, mRingBuffer,(mRingBuffer + mRingBufferSize));
    dst_ptr += nb_output_samples;
    mRingWritePtr = (t_sword*)dst_ptr;
  }

  // 4. on first reference buffer, select if we will use time align or not
  if(mRefStatus == REFERENCE_INACTIVE)
  {
	mTimeStamp = timestamp;
    if(timestamp || (buffer->flags & BUFFERFLAG_STARTTIME))
      mRefStatus = REFERENCE_ACTIVE_TIMESTAMP;
    else
      mRefStatus = REFERENCE_ACTIVE_NO_TIMESTAMP;
  }

  // 5. update timestamp information
  mTimeStamp = mTimeStamp + (nb_input_ms * MS2TIMESTAMP);

  // 6. send EOS event if needed
  if (buffer->flags & BUFFERFLAG_EOS)
  {
    proxy.eventHandler(OMX_EventBufferFlag, REFERENCE_PORT, BUFFERFLAG_EOS);
	if(!mRealTimeReference) mRefStatus = REFERENCE_INACTIVE;
  }

  return TRUE;
}


// read smaples from ring buffer.
// If time alignment is on, read samples with same timestamp (add 0 if not yet received or already overwritten)
// If time alignment is off, simply read the latest samples
static void readFromRingBuffer(t_sword *dst, t_uint24 size, t_uint48 timestamp)
{
  t_sword __circ *src_circ;
  t_sint24        i,j;
  t_sint24        pre_padding_sample = 0;
  t_sint24        post_padding_sample = 0;
  t_sword        *start_ptr;


  if(mAlignmentStatus == TIME_ALIGNMENT_ON_GOING)
  {
	start_ptr = mRingWritePtr;
    if(timestamp >= mTimeStamp)
    {
      // none of the samples asked have been received, write buffer with full 0
      TRACE_t * this = (TRACE_t *)&mComponent;
      OstTraceFiltInst2(TRACE_WARNING, "Time align : ask data at %d us, whereas we only have samples up to %d us", (t_uint24)timestamp, (t_uint24)mTimeStamp);
      post_padding_sample = size;
      resetRingBuffer();
    }
    else
    {
      t_uint48 delta_timestamp = mTimeStamp - timestamp;
      t_uint24 delta_sample = (t_uint24)wfmulr((t_uint24)delta_timestamp,mRefUs2samples);

      if(delta_sample > mRingUsedSize)
      {
		TRACE_t * this = (TRACE_t *)&mComponent;
        // data asked have already been erased
        pre_padding_sample = (delta_sample - mRingUsedSize);
        delta_sample       = mRingUsedSize;
        if(pre_padding_sample > size)
        {
          pre_padding_sample = size;
        }
		OstTraceFiltInst3(TRACE_WARNING, "Time align : ask data at %d us, whereas we already have samples up to %d us (- %d us)", (t_uint24)timestamp, (t_uint24)mTimeStamp, (t_uint24)delta_timestamp);
      }

      src_circ  = winit_circ_ptr(mRingWritePtr, mRingBuffer,(mRingBuffer + mRingBufferSize));
      src_circ  = src_circ - delta_sample;
      start_ptr = (t_sword *)src_circ;

      // if post_padding_sample > 0, it means that we do not have all requested samples
      post_padding_sample = size - pre_padding_sample - delta_sample;

	  // all samples before the requested timestamp are now useless, mark them as free
	  mRingUsedSize = delta_sample;
	  mRingFreeSize = mRingBufferSize - delta_sample;
    }
  }
  else
  {
    // no time alignement, simply read the oldest samples
    pre_padding_sample = size - mRingUsedSize;
    start_ptr = mRingReadPtr;
  }

  if(pre_padding_sample > 0)
  {
    TRACE_t * this = (TRACE_t *)&mComponent;
    OstTraceFiltInst1(TRACE_DEBUG, "Time align : add %d pre padding samples", pre_padding_sample);
    size -= pre_padding_sample;
  }

  if(post_padding_sample > 0)
  {
    TRACE_t * this = (TRACE_t *)&mComponent;
    OstTraceFiltInst1(TRACE_DEBUG, "Time align : add %d post padding samples", post_padding_sample);
    size -= post_padding_sample;
  }

  src_circ = winit_circ_ptr(start_ptr, mRingBuffer,(mRingBuffer + mRingBufferSize));

  // "de-interleaved" reference data
#pragma loop maxitercount(MAX_NB_REF_CHANNEL)
  for(j=0;j<mRefChansNb;j++)
  {
    src_circ = winit_circ_ptr(start_ptr + j, mRingBuffer,(mRingBuffer + mRingBufferSize));

    // put pre padding at the begging
#pragma loop maxitercount(MAX_LOOP_COUNT)
    for(i=0;i<pre_padding_sample;i+=mRefChansNb)
    {
      *dst++ = 0;
    }

    // samples are stored as 24bit in ring buffer
#pragma loop maxitercount(MAX_LOOP_COUNT)
    for(i=0;i<size;i+=mRefChansNb)
    {
      *dst++ = ((*src_circ)>>8);
      src_circ = (src_circ + mRefChansNb);
    }

    // put post padding at the end
#pragma loop maxitercount(MAX_LOOP_COUNT)
    for(i=0;i<post_padding_sample;i+=mRefChansNb)
    {
      *dst++ = 0;
    }

    dst += ((NB_OUTPUT_PART - 1) * mSingleInputBufferSize); // points on next channel
  }

  src_circ -= (mRefChansNb - 1);

  mRingReadPtr   = (t_sword *)src_circ;
  mRingUsedSize -= size;
  mRingFreeSize += size;
}


// prepare one buffer for the processing library.
// contains mChansNb + mRefChansNb channles not interleaved
static void packOutputBuffer(Buffer_p pInputBuf, Buffer_p pOutputBuf)
{
  t_sword * data_in;
  t_sword * data_out = pOutputBuf->data + (mSingleInputBufferSize * mOutputPart);
  t_uint24  i,j;

  PRECONDITION(pOutputBuf->allocLen >= mOutputBufferSize);
  PRECONDITION((pInputBuf->filledLen == (mChansNb * mSingleInputBufferSize)) ||
               (pInputBuf->flags & BUFFERFLAG_EOS));

  if(pInputBuf->flags & BUFFERFLAG_EOS)
  {
    //complete input buffer with 0 if needed, so that they all have the same size
#pragma loop maxitercount(MAX_LOOP_COUNT)
    for(i=pInputBuf->filledLen; i<(mSingleInputBufferSize * mChansNb); i++)
    {
      pInputBuf->data[i] = 0;
    }
  }

  // copy main input (all channels)
  // input buffers are 24bits
#pragma loop maxitercount(6)
  for(j=0;j<mChansNb;j++)
  {
    data_in   = pInputBuf->data + j;
#pragma loop maxitercount(MAX_LOOP_COUNT)
    for(i=0;i<mSingleInputBufferSize;i++)
    {
      *data_out++ = (*data_in)>>8;
      data_in  += (mChansNb);
    }
    data_out += ((NB_OUTPUT_PART - 1) * mSingleInputBufferSize); // points on next channel
  }

  // copy ref input
  readFromRingBuffer(data_out, (mRefChansNb * mSingleInputBufferSize), pInputBuf->nTimeStampl);

  if(mOutputPart == 0)
  {
    pOutputBuf->flags       = pInputBuf->flags;
    pOutputBuf->nTimeStamph = pInputBuf->nTimeStamph;
    pOutputBuf->nTimeStampl = pInputBuf->nTimeStampl;
  }
  else
  {
    pOutputBuf->flags     |= pInputBuf->flags;
  }

  if(pInputBuf->flags & BUFFERFLAG_EOS)
  {
    completeOutputBufferWithZeroPadding(pOutputBuf->data, mSingleInputBufferSize);
  }

  if (mOutputPart == (NB_OUTPUT_PART - 1))
  {
    pOutputBuf->filledLen = mOutputBufferSize;
  }
}

// complete one output buffer with 0 on all channels
static void completeOutputBufferWithZeroPadding(t_sword *ptr, t_uint24 offset)
{
  t_uint24 nb_padding_samples;
  t_uint24 nb_written_samples;
  t_uint24 i,j;

  nb_written_samples = mOutputPart * mSingleInputBufferSize + offset;
  nb_padding_samples = (NB_OUTPUT_PART * mSingleInputBufferSize - nb_written_samples);

  if(nb_padding_samples <= 0) return;

  ptr += nb_written_samples;
  for(j=0;j<(mChansNb + mRefChansNb);j++)
  {
    for(i=0;i<nb_padding_samples;i++)
    {
      *ptr++ = 0;
    }
    ptr += nb_written_samples;
  }

  mOutputPart = (NB_OUTPUT_PART - 1);
}


// test if requested reference is available
static t_bool  referenceAvailable(void)
{
  Buffer_p  pInputBuf        = Port_getBuffer(&(mComponent.ports[INPUT_PORT]),0);
  t_uint48  buffer_timestamp = pInputBuf->nTimeStampl;
  t_uint32  buffer_flags     = pInputBuf->flags;
  TRACE_t * this             = (TRACE_t *)&mComponent;

  if (mRefStatus == REFERENCE_INACTIVE)
  {
    // when no data have been received yet on reference port
    return TRUE;
  }
  else if((!buffer_timestamp &&  !(buffer_flags & BUFFERFLAG_STARTTIME))
          ||
          (mRefStatus == REFERENCE_ACTIVE_NO_TIMESTAMP))
  {
    // time stamp not set on input and/or reference port
    // simply check if amount of data available in ring buffer is enough
    // => no time alignment possible
    PRECONDITION(mAlignmentStatus != TIME_ALIGNMENT_ON_GOING);
    if(mAlignmentStatus == TIME_ALIGNMENT_NOT_STARTED)
    {
      OstTraceFiltInst4(TRACE_DEBUG, "No time alignment because : reference status %d, buffer timestamp %d %d - flags %d",mRefStatus,(t_uint24)(buffer_timestamp>>24), (t_uint24)(buffer_timestamp), (t_uint24)buffer_flags);
    }
    mAlignmentStatus = TIME_ALIGNMENT_NOT_AVAILABLE;

    if(mRealTimeReference || (mRingUsedSize >= (mSingleInputBufferSize * mRefChansNb)))
    {
      return TRUE;
    }
    else
    {
      //OstTraceFiltInst2(TRACE_DEBUG, "Reference Buffer size = %d (samples), size asked = %d (samples)", mRingUsedSize, (mSingleInputBufferSize * mRefChansNb));
      return FALSE;
    }
  }
  else if (!mRealTimeReference)
  {
	t_uint24 buffer_size_us;
	buffer_size_us = wfmulr((t_uint24)pInputBuf->filledLen, mRefSamplesOut2Ms);
	buffer_size_us = buffer_size_us * MS2TIMESTAMP;

	if(mAlignmentStatus == TIME_ALIGNMENT_NOT_STARTED)
	{
	  OstTraceFiltInst4(TRACE_DEBUG, "Alignment (no realtime) starts with reference timestamp %d %d, buffer timestamp %d %d", (t_uint24)(mTimeStamp>>24), (t_uint24)(mTimeStamp),(t_uint24)(buffer_timestamp>>24), (t_uint24)(buffer_timestamp));
	  mAlignmentStatus = TIME_ALIGNMENT_ON_GOING;
	}

	// Use time alignement but also wait for data ==> only for test
    if((buffer_timestamp + buffer_size_us) > mTimeStamp)
    {
      //OstTraceFiltInst3(TRACE_DEBUG, "timestamp available = %d (us), timestamp asked = %d (us), mRingFreeSize = %d",(t_uint24)mTimeStamp, (t_uint24)(buffer_timestamp + buffer_size_us), mRingFreeSize);
	  if(mRingFreeSize < (REF_INPUT_BUFFER_DURATION * mResampleContext.fout_khz * mRefChansNb))
	  {
		// In non realtime mode, we do not overwritte data in ring buffer.
		// Then if there is some gap on ul we may need to "flush" data in ring buffer
		if(buffer_timestamp >= mTimeStamp)
		{
		  // all the data already present in ring buffer are too old compareed to want is asked
		  resetRingBuffer();
		}
		else
		{
		  // some data are still usefull
		  t_uint48 nb_usefull_timestamp = mTimeStamp - buffer_timestamp;
		  t_uint24 nb_usefull_samples   = (t_uint24)wfmulr((t_uint24)nb_usefull_timestamp,mRefUs2samples);
		  mRingFreeSize = mRingBufferSize - nb_usefull_samples;
		  mRingUsedSize = nb_usefull_samples;
		}
		Component_scheduleProcessEvent(&mComponent);
	  }
	  return FALSE;
    }
    else
    {
      return TRUE;
    }
  }
  else
  {
    // Use time alignment.
    // Assume that if data are not available on reference port we will receive them too late, so they will be replaced with 0 in readFromRingBuffer function
    if(mAlignmentStatus == TIME_ALIGNMENT_NOT_STARTED)
    {
      OstTraceFiltInst4(TRACE_DEBUG, "Alignment starts with reference timestamp %d %d, buffer timestamp %d %d", (t_uint24)(mTimeStamp>>24), (t_uint24)(mTimeStamp),(t_uint24)(buffer_timestamp>>24), (t_uint24)(buffer_timestamp));
      mAlignmentStatus = TIME_ALIGNMENT_ON_GOING;
    }
	else if((mAlignmentStatus == TIME_ALIGNMENT_NOT_AVAILABLE) &&
			(mRefStatus == REFERENCE_ACTIVE_TIMESTAMP))
	{
	  OstTraceFiltInst4(TRACE_DEBUG, "Activate Alignment after some buffer with no alignment, with reference timestamp %d %d, buffer timestamp %d %d", (t_uint24)(mTimeStamp>>24), (t_uint24)(mTimeStamp),(t_uint24)(buffer_timestamp>>24), (t_uint24)(buffer_timestamp));
	  mAlignmentStatus = TIME_ALIGNMENT_ON_GOING;
	}
    return TRUE;
  }
}


