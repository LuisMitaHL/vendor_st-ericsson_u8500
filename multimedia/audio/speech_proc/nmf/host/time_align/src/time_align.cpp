/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file     time_align.cpp
 * \brief    NMF time alignment component
 * \author   ST-Ericsson
 */
/*****************************************************************************/
#include <speech_proc/nmf/host/time_align.nmf>
#include <string.h>

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "speech_proc_nmf_host_time_align_src_time_alignTraces.h"
#endif


////////////////////////////////////////////////////////////
//   COMPONENT methods
////////////////////////////////////////////////////////////
void speech_proc_nmf_host_time_align::process(void)
{
  int  nbInputBuf  = mPorts[INPUT_PORT].queuedBufferCount();
  int  nbRefBuf    = mPorts[REFERENCE_PORT].queuedBufferCount();
  int  nbOutputBuf = mPorts[OUTPUT_PORT].queuedBufferCount();

  OstTraceFiltInst3(TRACE_DEBUG, "Time align enter: nbInputBuf = %d, nbRefBuf = %d, nbOutputBuf = %d", nbInputBuf, nbRefBuf, nbOutputBuf);

  if(nbRefBuf)
  {
	nbRefBuf --;
	if(!mIsRefConfigured)
	{
	  mPorts[REFERENCE_PORT].dequeueAndReturnBuffer();
	}
	else
	{
	  OMX_BUFFERHEADERTYPE  *pRefBuf = mPorts[REFERENCE_PORT].getBuffer(0);
	  if(processRefBuffer(pRefBuf)) // return false only if mRealTimeReference == true, i.e. only for tests
	  {
		mPorts[REFERENCE_PORT].dequeueAndReturnBuffer();
	  }
	  else
	  {
		nbRefBuf++;
	  }
	}
  }

  if(mIsConfigured &&
     nbInputBuf    &&
     nbOutputBuf   &&
     referenceAvailable())
  {
    // retreive buffers
    OMX_BUFFERHEADERTYPE   *input  = mPorts[INPUT_PORT].getBuffer(0);
    OMX_BUFFERHEADERTYPE   *output = mPorts[OUTPUT_PORT].getBuffer(0);

    packOutputBuffer(input, output);

    mPorts[INPUT_PORT].dequeueAndReturnBuffer();
    nbInputBuf --;

    if(mOutputPart == (NB_OUTPUT_PART - 1))
    {
      if (output->nFlags & OMX_BUFFERFLAG_EOS)
      {
        proxy.eventHandler(OMX_EventBufferFlag, INPUT_PORT, output->nFlags);
      }

      OstTraceFiltInst2 (TRACE_DEBUG, "Speech_proc/time_align: latency control (timestamp = 0x%x 0x%x us)", (int)(output->nTimeStamp >> 32), (unsigned int)(output->nTimeStamp & 0xffffffffu));
      mPorts[OUTPUT_PORT].dequeueAndReturnBuffer();
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


void speech_proc_nmf_host_time_align::resetRingBuffer(void)
{
  mRingWritePtr = mRingBuffer;
  mRingReadPtr  = mRingBuffer;
  mRingUsedSize = 0;
  mRingFreeSize = RING_BUFFER_SIZE;
}

void speech_proc_nmf_host_time_align::resetTimeAlignment(void)
{
  mTimeStamp       = 0;
  mRefStatus       = REFERENCE_INACTIVE;
  mAlignmentStatus = TIME_ALIGNMENT_NOT_STARTED;
}

void speech_proc_nmf_host_time_align::resetReferencePort(void)
{
  mRefSampleFreq   = FREQ_UNKNOWN;
  mRefChansNb      = 1;
  mIsRefConfigured = false;
}

void speech_proc_nmf_host_time_align::resetInputPort()
{
  //mSampleFreq   = FREQ_UNKNOWN;
  mIsConfigured = false;
  mChansNb      = 0;
  mOutputPart   = 0;
  mAlignmentStatus = TIME_ALIGNMENT_NOT_STARTED;
}

void speech_proc_nmf_host_time_align::reset()
{
  resetRingBuffer();
  resetTimeAlignment();
  // do NOT call resetInputPort and resetReferencePort
  // as reset will be called AFTER setConfig if port are enabled when going to idle !!
}

void speech_proc_nmf_host_time_align::disablePortIndication(t_uint32 portIdx)
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

void speech_proc_nmf_host_time_align::enablePortIndication(t_uint32 portIdx)
{
}

void speech_proc_nmf_host_time_align::flushPortIndication(t_uint32 portIdx)
{
}




////////////////////////////////////////////////////////////
// PROVIDED INTERFACES
////////////////////////////////////////////////////////////
void METH(start)(void)
{
  mSampleFreq            = FREQ_UNKNOWN;
  mChansNb               = 0;
  mRefSampleFreq         = FREQ_UNKNOWN;
  mRefChansNb            = 1;
  mSingleInputBufferSize = 0;
  mOutputBufferSize      = 0;
  mRealTimeReference     = true;
  mRingWritePtr          = mRingBuffer;
  mRingReadPtr           = mRingBuffer;
  mOutputPart            = 0;
  mTimeStamp             = 0;
  mRefStatus             = REFERENCE_INACTIVE;
  mAlignmentStatus       = TIME_ALIGNMENT_NOT_STARTED;
  mIsConfigured          = false;
  mIsRefConfigured       = false;
  mHeap                  = (char *)0;
  mRingBufferEnd         = (t_uint32)mRingBuffer + SAMPLE2BYTE(RING_BUFFER_SIZE);
}

void METH(stop)(void)
{
  if(mHeap) delete [] mHeap;
  mHeap = (char *)0;
}


void METH(setParameter)(t_uint24 shared_buffer_header, t_uint24 ring_buffer, t_uint24 heap,
						t_uint24 heap_size, t_uint16 real_time_ref, t_uint24 ring_buffer_size)
{
  mRealTimeReference = (bool)real_time_ref;
  resample_calc_max_heap_size_fixin_fixout(0, 0, SRC_MODE, &mHeapSize,
										   0,MAX_NB_REF_CHANNEL,0);
  mHeap = new char[mHeapSize];
  ARMNMF_DBC_ASSERT(mHeap);
}

void METH(setConfigRef)(t_uint16 in_freq, t_uint16 out_freq, t_uint16 nb_channel_ref)
{
  bool     reconfigure_src = false;

  if (mRefChansNb != nb_channel_ref) reconfigure_src = true;
  mRefChansNb = nb_channel_ref;

  if(out_freq == 8000)
  {
    if (mSampleFreq != FREQ_8KHZ) reconfigure_src = true;
    mSampleFreq   = FREQ_8KHZ;
  }
  else if(out_freq == 16000)
  {
    if (mSampleFreq != FREQ_16KHZ) reconfigure_src = true;
    mSampleFreq   = FREQ_16KHZ;
  }
  else
    ARMNMF_DBC_ASSERT(0);

  if(in_freq == 8000)
  {
    if (mRefSampleFreq != FREQ_8KHZ) reconfigure_src = true;
    mRefSampleFreq = FREQ_8KHZ;
  }
  else if(in_freq == 16000)
  {
    if (mRefSampleFreq != FREQ_16KHZ) reconfigure_src = true;
    mRefSampleFreq = FREQ_16KHZ;
  }
  else if(in_freq == 48000)
  {
    if (mRefSampleFreq != FREQ_48KHZ) reconfigure_src = true;
    mRefSampleFreq = FREQ_48KHZ;
  }
  else
    ARMNMF_DBC_ASSERT(0);

  if(reconfigure_src == true)
  {
    reconfigureSrc();
  }

  mIsRefConfigured = true;
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
    ARMNMF_DBC_ASSERT(0);

  // update buffer size
  mSingleInputBufferSize = sample_per_ms * UL_INPUT_BUFFER_DURATION;
  mOutputBufferSize      = (mChansNb + mRefChansNb) * sample_per_ms * PROCESSING_BUFFER_DURATION * 2;

  mIsConfigured = 1;

  if(mPorts[INPUT_PORT].queuedBufferCount() &&
     mPorts[OUTPUT_PORT].queuedBufferCount())
  {
    scheduleProcessEvent();
  }
}


void METH(fsmInit)(fsmInit_t initFsm)
{
  // trace init (mandatory before port init)
  if (initFsm.traceInfoAddr)
  {
    setTraceInfo(initFsm.traceInfoAddr, initFsm.id1);
  }

  mPorts[INPUT_PORT].init(InputPort, false, false, 0, 0, 1, &inputport[INPUT_PORT], INPUT_PORT,
                          (initFsm.portsDisabled & (1<<INPUT_PORT)), (initFsm.portsTunneled & (1<<INPUT_PORT)), this);

  mPorts[REFERENCE_PORT].init(InputPort, false, false, 0, 0, 1, &inputport[REFERENCE_PORT], REFERENCE_PORT,
                              (initFsm.portsDisabled & (1<<REFERENCE_PORT)), (initFsm.portsTunneled & (1<<REFERENCE_PORT)),this);

  OMX_BUFFERHEADERTYPE *Buf_list[1] = {&mOutputBufferHeader};
  mOutputBufferHeader.pBuffer       = (OMX_U8 *)mOutputBuffer;
  mOutputBufferHeader.nAllocLen     = OUTPUT_BUFFER_SIZE_BYTE;
  mOutputBufferHeader.nFilledLen    = 0;
  mOutputBufferHeader.nFlags        = 0;
  mOutputBufferHeader.nOffset       = 0;
  mPorts[OUTPUT_PORT].init(OutputPort,true ,false, 0, Buf_list, 1, &outputport, OUTPUT_PORT,
                           (initFsm.portsDisabled & (1<<OUTPUT_PORT)), (initFsm.portsTunneled & (1<<OUTPUT_PORT)),this);

  init(3, mPorts, &proxy, &me, false);
}


void METH(setTunnelStatus)(t_sint16 portIdx, t_uint16 isTunneled)
{
  mPorts[portIdx].setTunnelStatus((bool)isTunneled);
}


void METH(emptyThisBuffer) (OMX_BUFFERHEADERTYPE_p buffer, t_uint8 idx)
{
  Component::deliverBufferCheck(idx,  buffer);
}

void METH(sendCommand)(OMX_COMMANDTYPE cmd, t_uword param)
{
  Component::sendCommand(cmd, param);
}

void METH(newFormat)(t_sample_freq sample_freq, t_uint16 chans_nb, t_uint16 sample_size, t_uint8 idx)
{
  if(idx == INPUT_PORT)
  {
    ARMNMF_DBC_ASSERT((mSampleFreq == FREQ_UNKNOWN) || (mSampleFreq == sample_freq));
	ARMNMF_DBC_ASSERT(sample_size == 16);
  }
  else
  {
    ARMNMF_DBC_ASSERT((mRefSampleFreq == FREQ_UNKNOWN) || (mRefSampleFreq == sample_freq));
	ARMNMF_DBC_ASSERT(sample_size == 32);
  }
}


///////////////////////////////////////////////////////////
// Helper function
///////////////////////////////////////////////////////////
// reconfigure SRC
void speech_proc_nmf_host_time_align::reconfigureSrc(void)
{
  int      status;

  // re-init src lib
  memset(&mResampleContext, 0 , sizeof(mResampleContext));
  status = resample_x_init_ctx_low_mips_fixin_fixout(mHeap, mHeapSize, &mResampleContext, mRefSampleFreq,
                                                     mSampleFreq, SRC_MODE, mRefChansNb, 0);
  ARMNMF_DBC_ASSERT(status == 0);

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
	  ARMNMF_DBC_ASSERT(0);
	}
  }
}


// process one buffer on reference port :
bool speech_proc_nmf_host_time_align::processRefBuffer(OMX_BUFFERHEADERTYPE *buffer)
{
  t_uint32  nb_input_ms;
  t_uint32  nb_output_samples;
  OMX_TICKS timestamp = buffer->nTimeStamp;

  OstTraceFiltInst2(TRACE_DEBUG, "Time Align: ProcessRefBuffer , timestamp = 0x%x%x", (t_uint32)(buffer->nTimeStamp >> 32), (t_uint32)buffer->nTimeStamp );

  // 1. compute input buffer size in ms and output size in samples
  nb_input_ms       = buffer->nFilledLen / (mResampleContext.fin_khz * mRefChansNb * 4);
  nb_output_samples = nb_input_ms * mResampleContext.fout_khz * mRefChansNb;

  // 2. If time alignment is in use, data received on reference port may not be contiguous
  //    If so, "lost" data are replaced by 0
  //    TODO : use better values that 0
  if((mRefStatus == REFERENCE_ACTIVE_TIMESTAMP) &&
     (mTimeStamp < timestamp))
  {
    OMX_TICKS delta_timestamp = timestamp - mTimeStamp;
    t_uint16  delta_sample    = ((t_uint32)delta_timestamp / MS2TIMESTAMP) * (mResampleContext.fout_khz * mRefChansNb);

    OstTraceFiltInst3(TRACE_WARNING, "Time align : %d samples lost in reference data flow (buf %d, ring %d) . replaced with 0", delta_sample, (t_uint24)timestamp, (t_uint24)mTimeStamp);

    if(delta_sample > RING_BUFFER_SIZE) delta_sample = RING_BUFFER_SIZE;

	if(!mRealTimeReference             &&
	   (delta_sample  >  mRingFreeSize) &&
	   (mRingUsedSize >= (UL_INPUT_BUFFER_DURATION * mResampleContext.fout_khz * mRefChansNb)))
	  return false;

	ringModuloSet(0, delta_sample);
	mTimeStamp = timestamp;
  }

  // in non realtime mode, test if we have enough space in ring buffer not to erase not yet read data
  if(!mRealTimeReference && ( nb_output_samples > mRingFreeSize)) return false;

  // 3. do samplerate conversion
  if(mSampleFreq == mRefSampleFreq)
  {
    t_sint32 *src_ptr  = (t_sint32 *)buffer->pBuffer;
    t_uint32 nb_sample = BYTE2SAMPLE(buffer->nFilledLen);

    // 3.1.1 simply recopy data if no src needed
    ringModuloWrite(src_ptr, nb_sample);
  }
  else
  {
    int      consumed = 0;
    int      produced = 0;
    int      flush = 0;
    int      res = 0;
    t_uint16 remaining_aligned_ms;
    int     *src_ptr = (int *)buffer->pBuffer;
    t_uint16 src_size_1 = nb_input_ms; // size in ms
    t_uint16 src_size_2 = 0;           // size in ms


    // 3.2.1 check how many "aligned" size remains in ringbuffer.
    //       if it is not enough then we need to divide samplerateocnverion into 2 calls
    remaining_aligned_ms = SAMPLE2BYTE(RING_BUFFER_SIZE) - ((t_uint32)mRingWritePtr - (t_uint32)mRingBuffer);
    remaining_aligned_ms = remaining_aligned_ms / (SAMPLE2BYTE(mResampleContext.fout_khz * mRefChansNb));
    if(remaining_aligned_ms < nb_input_ms)
    {
      // need to split samplerateocnverion into 2 calls
      src_size_1 = remaining_aligned_ms;
      src_size_2 = nb_input_ms - src_size_1;
    }

    consumed = src_size_1;
    produced = 0;
    flush    = 0;

    res = (*mResampleContext.processing) (&mResampleContext, src_ptr, mRefChansNb,
                                          &consumed, (int *)mRingWritePtr, &produced, &flush);

    ARMNMF_DBC_ASSERT(res == 0);
    ARMNMF_DBC_ASSERT(consumed == src_size_1);
    ARMNMF_DBC_ASSERT(produced == src_size_1);

    if(src_size_2)
    {
      src_ptr  = (int *)buffer->pBuffer;
      src_ptr += src_size_1 * mResampleContext.fin_khz;

      consumed = src_size_2;
      produced = 0;
      flush    = 0;

      res = (*mResampleContext.processing) (&mResampleContext, src_ptr, mRefChansNb,
                                            &consumed, (int *)mRingBuffer, &produced, &flush);

      ARMNMF_DBC_ASSERT(res == 0);
	  ARMNMF_DBC_ASSERT(consumed == src_size_2);
	  ARMNMF_DBC_ASSERT(produced == src_size_2);
	}

    // 3.2.2 update ring buffer size
    adaptRingUsedSize(nb_output_samples);

    // 3.2.3 update write buffer for next buffer
    mRingWritePtr += nb_output_samples;
    if((t_uint32)mRingWritePtr >= mRingBufferEnd) mRingWritePtr -= RING_BUFFER_SIZE;
  }

  // 4. on first reference buffer, select if we will time align or not
  if(mRefStatus == REFERENCE_INACTIVE)
  {
	mTimeStamp = timestamp;

    if(timestamp || (buffer->nFlags & OMX_BUFFERFLAG_STARTTIME))
      mRefStatus = REFERENCE_ACTIVE_TIMESTAMP;
    else
      mRefStatus = REFERENCE_ACTIVE_NO_TIMESTAMP;
  }

  // 5. update timestamp information
  mTimeStamp = mTimeStamp + (nb_input_ms * MS2TIMESTAMP);

  // 6. send EOS event if needed
  if (buffer->nFlags & OMX_BUFFERFLAG_EOS)
  {
    proxy.eventHandler(OMX_EventBufferFlag, REFERENCE_PORT, OMX_BUFFERFLAG_EOS);
	if(! mRealTimeReference) mRefStatus = REFERENCE_INACTIVE;
  }

  return true;
}


// read samples from ring buffer.
// If time alignment is on, read samples with same timestamp (add 0 if not yet received or already overwritten)
// If time alignment is off, simply read the latest samples
void speech_proc_nmf_host_time_align::readFromRingBuffer(t_sint16 *dst, t_uint32 size, OMX_TICKS timestamp)
{
  t_sint16  i,j;
  t_sint16  pre_padding_sample = 0;
  t_sint16  post_padding_sample = 0;
  t_sint32 *start_ptr;

  if(mAlignmentStatus == TIME_ALIGNMENT_ON_GOING)
  {
	start_ptr = mRingWritePtr;
    if(timestamp >= mTimeStamp)
    {
      // none of the samples asked have been received, write buffer with full 0
      OstTraceFiltInst2(TRACE_WARNING, "Time align : ask data at %d us, whereas we only have samples up to %d us", (t_uint24)timestamp, (t_uint24)mTimeStamp);
      post_padding_sample = size;
      resetRingBuffer();
    }
    else
    {
      OMX_TICKS delta_timestamp = mTimeStamp - timestamp;
      t_uint16  delta_sample = ((t_uint32)delta_timestamp / MS2TIMESTAMP) * mRefChansNb * mResampleContext.fout_khz;

      if(delta_sample > mRingUsedSize)
      {
        // data asked have already been erased
        pre_padding_sample = (delta_sample - mRingUsedSize);
        delta_sample       = mRingUsedSize;
        if((unsigned)pre_padding_sample > size)
        {
          OstTraceFiltInst1(TRACE_WARNING, "Time align : ask delta of %d samples", delta_sample);
          pre_padding_sample = size;
        }
      }

      start_ptr  = mRingWritePtr;
      start_ptr -= delta_sample;
      if((t_uint32)start_ptr < (t_uint32)mRingBuffer) start_ptr += RING_BUFFER_SIZE;

      // if post_padding_sample > 0, it means that we do not have all requested samples
      post_padding_sample = size - pre_padding_sample - delta_sample;

	  // all samples before the requested timestamp are now useless, mark them as free
	  mRingUsedSize = delta_sample;
	  mRingFreeSize = RING_BUFFER_SIZE - delta_sample;
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
    OstTraceFiltInst1(TRACE_DEBUG, "Time align : add %d pre padding samples", pre_padding_sample);
    size -= pre_padding_sample;
  }

  if(post_padding_sample > 0)
  {
    OstTraceFiltInst1(TRACE_DEBUG, "Time align : add %d post padding samples", post_padding_sample);
    size -= post_padding_sample;
  }

  // "de-interleaved" reference data
  for(j=0;j<mRefChansNb;j++)
  {
    mRingReadPtr = start_ptr + j;

    // put pre padding at the begging
    for(i=pre_padding_sample; i>0; i-=mRefChansNb)
    {
      *dst++ = 0;
    }

    ringModuloRead(dst, size);

    // put post padding at the end
    for(i=post_padding_sample; i>0; i-=mRefChansNb)
    {
      *dst++ = 0;
    }

    dst += ((NB_OUTPUT_PART - 1) * mSingleInputBufferSize); // points on next channel
  }

  mRingUsedSize -= size;
  mRingFreeSize += size;
}



// prepare one buffer for the processing library.
// contains mChansNb + mRefChansNb channles not interleaved
void speech_proc_nmf_host_time_align::packOutputBuffer(OMX_BUFFERHEADERTYPE *pInputBuf, OMX_BUFFERHEADERTYPE *pOutputBuf)
{
  t_sint16 *data_in;
  t_sint16 *data_out = (t_sint16 *)pOutputBuf->pBuffer + (mSingleInputBufferSize * mOutputPart);
  t_uint16  i,j;

  ARMNMF_DBC_ASSERT(pOutputBuf->nAllocLen >= mOutputBufferSize);
  ARMNMF_DBC_ASSERT((pInputBuf->nFilledLen == (unsigned)(mChansNb * mSingleInputBufferSize * 2))||
		    (pInputBuf->nFlags &  OMX_BUFFERFLAG_EOS));

  if(pInputBuf->nFlags & OMX_BUFFERFLAG_EOS)
  {
    //complete input buffer with 0 if needed, so that they all have the same size
    for(i=pInputBuf->nFilledLen; i<(mSingleInputBufferSize * mChansNb * 2); i++)
    {
      pInputBuf->pBuffer[i] = 0;
    }
  }

  // copy main input port
  for(j=0;j<mChansNb;j++)
  {
    data_in = (t_sint16 *)pInputBuf->pBuffer + j;
    for(i=mSingleInputBufferSize; i>0; i--)
    {
      *data_out++ = (*data_in);
      data_in += (mChansNb);
    }

    data_out += ((NB_OUTPUT_PART - 1) * mSingleInputBufferSize); // points on next channel
  }

  // copy ref input
  readFromRingBuffer((t_sint16 *)data_out, (mRefChansNb * mSingleInputBufferSize), pInputBuf->nTimeStamp);

  if(mOutputPart == 0)
  {
    pOutputBuf->nFlags     = pInputBuf->nFlags;
    pOutputBuf->nTimeStamp = pInputBuf->nTimeStamp;
  }
  else
  {
    pOutputBuf->nFlags     |= pInputBuf->nFlags;
  }

  if(pInputBuf->nFlags & OMX_BUFFERFLAG_EOS)
  {
    completeOutputBufferWithZeroPadding((t_sint16 *)pOutputBuf->pBuffer, mSingleInputBufferSize);
  }

  if (mOutputPart == (NB_OUTPUT_PART - 1))
  {
    pOutputBuf->nFilledLen  = mOutputBufferSize;
  }
}

// complete one output buffer with 0 on all channels
void speech_proc_nmf_host_time_align::completeOutputBufferWithZeroPadding(t_sint16 * ptr, t_uint16 offset)
{
  t_uint16 nb_padding_samples;
  t_uint16 nb_written_samples;
  t_uint16 i,j;

  nb_written_samples = mOutputPart * mSingleInputBufferSize + offset;
  nb_padding_samples = (NB_OUTPUT_PART * mSingleInputBufferSize - nb_written_samples);

  if(nb_padding_samples <= 0) return;

  ptr += nb_written_samples;
  for(j=0;j<(mChansNb + mRefChansNb);j++)
  {
    for(i=nb_padding_samples; i>0; i++)
    {
      *ptr++ = 0;
    }
    ptr += nb_written_samples;
  }

  mOutputPart = (NB_OUTPUT_PART - 1);
}


// test if requested reference is available
bool speech_proc_nmf_host_time_align::referenceAvailable(void)
{
  OMX_BUFFERHEADERTYPE *pInputBuf        = mPorts[INPUT_PORT].getBuffer(0);
  OMX_TICKS             buffer_timestamp = pInputBuf->nTimeStamp;
  OMX_U32               buffer_flags     = pInputBuf->nFlags;

  if (mRefStatus == REFERENCE_INACTIVE)
  {
    // when no data have been received yet on reference port
    return TRUE;
  }
  else if((!buffer_timestamp &&  !(buffer_flags & OMX_BUFFERFLAG_STARTTIME))
          ||
          (mRefStatus == REFERENCE_ACTIVE_NO_TIMESTAMP))
  {
    // time stamp not set on input and/or reference port
    // simply check if amount of data available in ring buffer is enough
    // => no time alignment possible
    ARMNMF_DBC_ASSERT(mAlignmentStatus != TIME_ALIGNMENT_ON_GOING);
    if(mAlignmentStatus == TIME_ALIGNMENT_NOT_STARTED)
    {
      OstTraceFiltInst4(TRACE_DEBUG, "No time alignment because : reference status %d, buffer timestamp %d %d - flags %d",mRefStatus,(t_uint32)(buffer_timestamp>>32), (t_uint32)(buffer_timestamp), (t_uint32)buffer_flags);
    }
    mAlignmentStatus = TIME_ALIGNMENT_NOT_AVAILABLE;

    if(mRealTimeReference || (mRingUsedSize >= (t_uint32)(mSingleInputBufferSize * mRefChansNb)))
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
	t_uint32 buffer_size_us;
	buffer_size_us = ((pInputBuf->nFilledLen / 2) / (mResampleContext.fout_khz * mRefChansNb));
	buffer_size_us = buffer_size_us * MS2TIMESTAMP;

	if(mAlignmentStatus == TIME_ALIGNMENT_NOT_STARTED)
	{
	  OstTraceFiltInst4(TRACE_DEBUG, "Alignment (no realtime) starts with reference timestamp %d %d, buffer timestamp %d %d", (t_uint32)(mTimeStamp>>32), (t_uint32)(mTimeStamp),(t_uint32)(buffer_timestamp>>32), (t_uint32)(buffer_timestamp));
	  mAlignmentStatus = TIME_ALIGNMENT_ON_GOING;
	}

    // Use time alignement but also wait for data ==> only for test
    if((buffer_timestamp + buffer_size_us) > mTimeStamp)
    {
	  OstTraceFiltInst3(TRACE_DEBUG, "timestamp available = %d (us), timestamp asked = %d (us), mRingFreeSize = %d",(t_uint32)mTimeStamp, (t_uint32)(buffer_timestamp + buffer_size_us), mRingFreeSize);
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
		  OMX_TICKS nb_usefull_timestamp = mTimeStamp - buffer_timestamp;
		  t_uint16  nb_usefull_samples   = ((t_uint32)nb_usefull_timestamp / MS2TIMESTAMP) * mRefChansNb * mResampleContext.fout_khz;
		  mRingFreeSize = RING_BUFFER_SIZE - nb_usefull_samples;
		  mRingUsedSize = nb_usefull_samples;
		}
		scheduleProcessEvent();
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
      OstTraceFiltInst4(TRACE_DEBUG, "Alignment starts with reference timestamp %d %d, buffer timestamp %d %d", (t_uint32)(mTimeStamp>>32), (t_uint32)(mTimeStamp),(t_uint32)(buffer_timestamp>>32), (t_uint32)(buffer_timestamp));
      mAlignmentStatus = TIME_ALIGNMENT_ON_GOING;
    }
	else if((mAlignmentStatus == TIME_ALIGNMENT_NOT_AVAILABLE) &&
			(mRefStatus == REFERENCE_ACTIVE_TIMESTAMP))
	{
	  OstTraceFiltInst4(TRACE_DEBUG, "Activate Alignment after some buffer with no alignment, with reference timestamp %d %d, buffer timestamp %d %d", (t_uint32)(mTimeStamp>>32), (t_uint32)(mTimeStamp),(t_uint32)(buffer_timestamp>>32), (t_uint32)(buffer_timestamp));
	  mAlignmentStatus = TIME_ALIGNMENT_ON_GOING;
	}
    return TRUE;
  }
}


///////////////////////////////////////
// Ring buffer modulo functions
//////////////////////////////////////
// write nb_sample from src in ringbuffer
// and update ring buffer status (i.e. mRingWritePtr, mRingUsedSize and mRingFreeSize )
// Assume that nb_sample <= RING_BUFFER_SIZE
void speech_proc_nmf_host_time_align::ringModuloWrite(t_sint32 *src, t_uint16 nb_sample)
{
  t_uint16 nb_sample_modulo = 0;
  bool     modulo = false;

  if(((t_uint32)mRingWritePtr + SAMPLE2BYTE(nb_sample)) >= mRingBufferEnd)
  {
    modulo = true;
    nb_sample_modulo = nb_sample - BYTE2SAMPLE((mRingBufferEnd - (t_uint32)mRingWritePtr));
    nb_sample -= nb_sample_modulo;
  }

  memcpy((void *)mRingWritePtr, (void *)src, SAMPLE2BYTE(nb_sample));
  mRingWritePtr += nb_sample;

  if(modulo)
  {
    mRingWritePtr = mRingBuffer;
    src += nb_sample;
    memcpy((void *)mRingWritePtr, (void *)src, SAMPLE2BYTE(nb_sample_modulo));
    mRingWritePtr += nb_sample_modulo;
  }

  adaptRingUsedSize(nb_sample + nb_sample_modulo);
}

// read nb_sample from mRingReadPtr and write then to dst de-interleaved
// and update mRingReadPtr NOT mRingUsedSize and mRingFreeSize
// Assume that nb_sample <= RING_BUFFER_SIZE
void speech_proc_nmf_host_time_align::ringModuloRead(t_sint16 *dst, t_uint16 nb_sample)
{
  t_uint16 i;
  t_uint16 nb_sample_modulo = 0;
  bool     modulo = false;

  if(((t_uint32)mRingReadPtr + SAMPLE2BYTE(nb_sample)) >= mRingBufferEnd)
  {
    modulo = true;
    nb_sample_modulo = nb_sample - BYTE2SAMPLE((mRingBufferEnd - (t_uint32)mRingReadPtr));
    nb_sample -= nb_sample_modulo;
  }

  for(i=nb_sample; i > 0; i--)
  {
    *dst++ = (t_sint16)(((*mRingReadPtr)&0xFFFF0000)>>16);
    mRingReadPtr+=mRefChansNb;
  }

  if(modulo)
  {
    mRingReadPtr = mRingBuffer;
    for(i=nb_sample_modulo; i > 0; i--)
    {
      *dst++ = (t_sint16)(((*mRingReadPtr)&0xFFFF0000)>>16);
      mRingReadPtr+=mRefChansNb;
    }
  }
}

// write nb_sample of data in ringbuffer
// and update ring buffer status (i.e. mRingWritePtr, mRingUsedSize and mRingFreeSize )
// Assume that nb_sample <= RING_BUFFER_SIZE
void speech_proc_nmf_host_time_align::ringModuloSet(t_uint16 data, t_uint16 nb_sample)
{
  t_uint16 nb_sample_modulo = 0;
  bool     modulo = false;

  if(((t_uint32)mRingWritePtr + SAMPLE2BYTE(nb_sample)) >= mRingBufferEnd)
  {
    modulo = true;
    nb_sample_modulo = nb_sample - BYTE2SAMPLE((mRingBufferEnd - (t_uint32)mRingWritePtr));
    nb_sample -= nb_sample_modulo;
  }

  memset((void *)mRingWritePtr, (char)data, SAMPLE2BYTE(nb_sample));
  mRingWritePtr += nb_sample;

  if(modulo)
  {
    mRingWritePtr = mRingBuffer;
    memset((void *)mRingWritePtr, (char)data, SAMPLE2BYTE(nb_sample_modulo));
    mRingWritePtr += nb_sample_modulo;
  }

  adaptRingUsedSize(nb_sample + nb_sample_modulo);
}

// update used/free size
void speech_proc_nmf_host_time_align::adaptRingUsedSize(t_uint16 delta)
{
  mRingUsedSize += delta;
  mRingFreeSize -= delta;

  if(mRingUsedSize > RING_BUFFER_SIZE)
  {
	if( mAlignmentStatus != TIME_ALIGNMENT_NOT_STARTED)
	  OstTraceFiltInst0(TRACE_WARNING, "Time align : Overflow in reference buffer ");
    mRingUsedSize = RING_BUFFER_SIZE;
    mRingFreeSize = 0;
    mRingReadPtr  = mRingWritePtr;
  }
}
