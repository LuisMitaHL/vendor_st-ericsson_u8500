/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file     ul_wrapper.cpp
 * \brief    NMF wrapper for uplink algorithms
 * \author   ST-Ericsson
 */
/*****************************************************************************/
#include <speech_proc/nmf/host/ul_wrapper.nmf>
#include "string.h"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "speech_proc_nmf_host_ul_wrapper_src_ul_wrapperTraces.h"
#endif

#define MAX_OST_SIZE 56 // To be removed when limitation removed on osttrace lib side
//
// PROVIDED INTERFACES
//
void METH(start)(void)
{
  mProcessingCtx    = 0;
  mAlgo             = SP_UPLINK_NO_ALGO;
  mInExecutingState = false;
  mIsConfigured     = false;
  mSampleFreqIn     = FREQ_UNKNOWN;
  mSampleFreqOut    = FREQ_UNKNOWN;
  mGetSidetoneGain  = true;
  mOutputReference  = false;
  mInfoFifoFull     = FALSE;
  resetProcessingInfo();
}

void METH(stop)(void)
{
  speech_proc_error_t error;
  error = speech_proc_close(mProcessingCtx);
  ARMNMF_DBC_ASSERT(error == SP_ERROR_NONE);
}


void METH(fsmInit)(fsmInit_t initFsm)
{
  bool invalid_state = true;

  if (mProcessingCtx != 0)
  {
    if(speech_proc_open(mProcessingCtx)==SP_ERROR_NONE)
    {
      invalid_state = false;
    }
  }

  // trace init (mandatory before port init)
  if (initFsm.traceInfoAddr)
  {
    setTraceInfo(initFsm.traceInfoAddr, initFsm.id1);
  }

  mPorts[INPUT_PORT].init(InputPort, false, false, 0, 0, 1, &inputport, INPUT_PORT,
                          (initFsm.portsDisabled & (1<<INPUT_PORT)), (initFsm.portsTunneled & (1<<INPUT_PORT)), this);

  mPorts[OUTPUT_PORT].init(OutputPort,false ,false, 0, 0, 1, &outputport, OUTPUT_PORT,
                           (initFsm.portsDisabled & (1<<OUTPUT_PORT)), (initFsm.portsTunneled & (1<<OUTPUT_PORT)),this);

  init(2, mPorts, &proxy, &me, invalid_state);
}


void METH(setTunnelStatus)(t_sint16 portIdx, t_uint16 isTunneled)
{
  mPorts[portIdx].setTunnelStatus((bool)isTunneled);
}

void METH(initLib)(t_uint16 bypass)
{
  unsigned int version = 0;
  speech_proc_error_t error ;

  if(bypass & 0xFF00)
  {
	bypass &= 0x00FF;
	mOutputReference = true;
  }

  if(bypass) { mAlgo = SP_UPLINK_NO_ALGO; }
  else       { mAlgo = SP_UPLINK_ALGO; }

  error = speech_proc_init(mAlgo, &mProcessingCtx, &version);

  ARMNMF_DBC_ASSERT(error == SP_ERROR_NONE);
  context.newLibraryContext(mProcessingCtx,SP_UPLINK_ALGO, (t_uint32)version);
}

void METH(configure)(t_uint16 freq_in, t_uint16 freq_out,
                     t_uint16 nb_channel_main, t_uint16 nb_channel_ref,
                     t_uint16 port_enabled)
{
  ARMNMF_DBC_ASSERT((freq_in == 8000)  || (freq_in == 16000));
  ARMNMF_DBC_ASSERT((freq_out == 8000) || (freq_out == 16000));
  ARMNMF_DBC_ASSERT((nb_channel_main + nb_channel_ref) <= MAX_NB_CHANNEL);

  // Configure port settings
  mChansNb                 = nb_channel_main;
  mRefChansNb              = nb_channel_ref;
  mInputChannelSampleSize  = (freq_in  / 1000) * PROCESSING_BUFFER_DURATION;  // size in sample
  mOutputChannelSampleSize = (freq_out / 1000) * PROCESSING_BUFFER_DURATION;  // size in sample
  if (freq_in == 8000)  mSampleFreqIn  = FREQ_8KHZ;
  else                  mSampleFreqIn  = FREQ_16KHZ;
  if (freq_out == 8000) mSampleFreqOut = FREQ_8KHZ;
  else                  mSampleFreqOut = FREQ_16KHZ;

  speech_proc_settings_t mode;
  mode.resolution = 16;
  mode.framesize  = (PROCESSING_BUFFER_DURATION / PROCESSING_COUNT);

  speech_proc_port_settings_t   inports;
  inports.nb_ports                            = 2;
  inports.port[SP_MAIN_PORT].enabled          = (port_enabled & (1 << SP_MAIN_PORT));
  inports.port[SP_MAIN_PORT].interleaved      = 0;
  inports.port[SP_MAIN_PORT].nb_channels      = nb_channel_main;
  inports.port[SP_MAIN_PORT].samplerate       = freq_in;
  inports.port[SP_REFERENCE_PORT].enabled     = (port_enabled & (1 << SP_REFERENCE_PORT));
  inports.port[SP_REFERENCE_PORT].interleaved = 0;
  inports.port[SP_REFERENCE_PORT].nb_channels = nb_channel_ref;
  inports.port[SP_REFERENCE_PORT].samplerate  = freq_in;

  speech_proc_port_settings_t outports;
  outports.nb_ports = 1;
  outports.port[SP_MAIN_PORT].enabled     = (port_enabled & (1 << (SP_MAIN_PORT + inports.nb_ports)));
  outports.port[SP_MAIN_PORT].interleaved = 0;
  outports.port[SP_MAIN_PORT].nb_channels = 1;
  outports.port[SP_MAIN_PORT].samplerate  = freq_out;

  if(mProcessingCtx)
  {
    speech_proc_error_t error;
    error = speech_proc_set_mode(mProcessingCtx, &mode, &inports, &outports);
    ARMNMF_DBC_ASSERT(error == SP_ERROR_NONE);
    downlink.resetProcessingInfo();
  }

  mIsConfigured  = true;

  // As buffer may be received before configure (since we wait for both OMX port to be enabled before configuring)
  if(mInExecutingState)
    scheduleProcessEvent();
}


void METH(setConfig)(t_uint32 index, void *config_struct)
{
  speech_proc_error_t error;
  error = speech_proc_set_config(mProcessingCtx, index, config_struct);
  ARMNMF_DBC_ASSERT(error == SP_ERROR_NONE);
}


void METH(setParameter)(t_uint32 index, void *config_struct)
{
  speech_proc_error_t error;
  error = speech_proc_set_parameter(mProcessingCtx ,index, config_struct);
  ARMNMF_DBC_ASSERT(error == SP_ERROR_NONE);
}

void METH(fillThisBuffer) (OMX_BUFFERHEADERTYPE_p buffer)
{
  ((OMX_BUFFERHEADERTYPE *)buffer)->nOffset = 0;
  Component::deliverBufferCheck(OUTPUT_PORT,  buffer);
}

void METH(resetProcessingInfo)(void)
{
  t_uint16 i;

  mNextInfoRead = 0;
  mNextInfoWrite = 0;
  for (i=0; i<NB_INFO_EXCHANGE; i++)
  {
     mInfoPtr[i] = (void *)0;
  }
  ack_downlink.ackProcessingInfo();
}

t_bool METH(setProcessingInfo)(void *info)
{
  ARMNMF_DBC_ASSERT(mInfoPtr[mNextInfoWrite] == NULL);
  mInfoPtr[mNextInfoWrite] = info;
  mNextInfoWrite = (mNextInfoWrite + 1) % NB_INFO_EXCHANGE;

  return (mNextInfoWrite == mNextInfoRead);
}

void METH(ackProcessingInfo)(void)
{
  mInfoFifoFull = FALSE;
}

//
// COMPONENT method
//
void speech_proc_nmf_host_ul_wrapper::reset()
{
  mNbFrame = 0;
  while(mInfoPtr[mNextInfoRead])
  {
    OstTraceFiltInst2(TRACE_DEBUG, "ul_wrapper: purge dl info 0x%x at index %d", (int)mInfoPtr[mNextInfoRead], mNextInfoRead);
    speech_proc_set_processing_info(mProcessingCtx, mInfoPtr[mNextInfoRead]);
    mInfoPtr[mNextInfoRead] = (void *)0;
    mNextInfoRead = (mNextInfoRead + 1) % NB_INFO_EXCHANGE;
  }
  ack_downlink.ackProcessingInfo();
}

void speech_proc_nmf_host_ul_wrapper::disablePortIndication(t_uint32 portIdx)
{
  mIsConfigured = false;
  downlink.resetProcessingInfo();
}

void speech_proc_nmf_host_ul_wrapper::enablePortIndication(t_uint32 portIdx)
{

}

void speech_proc_nmf_host_ul_wrapper::flushPortIndication(t_uint32 portIdx)
{
}

void speech_proc_nmf_host_ul_wrapper::sendCommand(OMX_COMMANDTYPE cmd, t_uword param)
{
  Component::sendCommand(cmd, param);

  if(cmd == OMX_CommandStateSet)
  {
    if(((OMX_STATETYPE)param) == OMX_StateExecuting)
    {
      mInExecutingState = true;
    }
    else
    {
      mInExecutingState = false;
    }
  }
}


void speech_proc_nmf_host_ul_wrapper::process(void)
{
  // Need to test mIsConfigured flag, because library can only be configured when
  // both input and output downlink OMX ports are enabled. And if input port is
  // enabled in executing state, whereas ouputport is still disabled, we may receive
  // input buffer and not be configured
  if(mPorts[INPUT_PORT].queuedBufferCount() &&
     mPorts[OUTPUT_PORT].queuedBufferCount() &&
     mIsConfigured)
  {
    // retreive buffers
    OMX_BUFFERHEADERTYPE        *input  = mPorts[INPUT_PORT].getBuffer(0);
    OMX_BUFFERHEADERTYPE        *output = mPorts[OUTPUT_PORT].getBuffer(0);
    speech_proc_error_t          error;
    void                        *info;

    // process
    ARMNMF_DBC_ASSERT((OMX_U32)(mOutputChannelSampleSize * 2) <= (output->nAllocLen));

    // read downlink information
    while(mInfoPtr[mNextInfoRead])
    {
       speech_proc_error_t error;
       OstTraceFiltInst2(TRACE_DEBUG, "ul wrapper: read dl info 0x%x at index %d", (int)mInfoPtr[mNextInfoRead], mNextInfoRead);
       error = speech_proc_set_processing_info(mProcessingCtx, mInfoPtr[mNextInfoRead]);
       if(error != SP_ERROR_NONE)
       {
          OstTraceFiltInst1(TRACE_ERROR, "ul wrapper: speech_proc_set_processing_info return error %d", error);
       }
       mInfoPtr[mNextInfoRead] = (void *)0;
       mNextInfoRead = (mNextInfoRead + 1) % NB_INFO_EXCHANGE;
    }
    ack_downlink.ackProcessingInfo();

    if((OMX_U32)(mInputChannelSampleSize  * 2 * (mChansNb + mRefChansNb)) == (input->nFilledLen))
    {
      traceInputBuffer(input);

      for(int j=0; j<PROCESSING_COUNT; j++)
      {
        for(int i=0;i<(mChansNb + mRefChansNb);i++)
        {
          mInputBuffers[i] = (t_sint16 *)((t_uint32)input->pBuffer + input->nOffset + (mInputChannelSampleSize * 2 * i) + (((mInputChannelSampleSize * 2)/PROCESSING_COUNT) * j));
        }
        mOutputBuffer = (t_sint16 *)((t_uint32)output->pBuffer + (((mOutputChannelSampleSize * 2)/PROCESSING_COUNT) * j));

        OstTraceFiltInst0(TRACE_FLOW, "start ul process");
        error = speech_proc_process(mProcessingCtx, mInputBuffers, (mChansNb + mRefChansNb), &mOutputBuffer, 1, (const speech_proc_frame_info_t *)NULL);
        ARMNMF_DBC_ASSERT(error == SP_ERROR_NONE);
        OstTraceFiltInst0(TRACE_FLOW, "end ul process");
      }

      // get information to share with DL library
      if (mInfoFifoFull == FALSE)
      {
         info = speech_proc_get_processing_info(mProcessingCtx);
         if(info)
         {
            mInfoFifoFull = downlink.setProcessingInfo(info);
         }
      }
      else
      {
        OstTraceFiltInst0(TRACE_WARNING, "ul wrapper: cannot send info to DL, fifo full");
      }

      // Get sidetonegain
      if(mGetSidetoneGain)
      {
        int gain   = 0;
        int update = 0;
        error = speech_proc_get_sidetone_gain(mProcessingCtx, &gain, &update);
        if((error == SP_ERROR_NONE) && update)
        {
          sidetone.newSidetoneGain(gain);
        }
        else if( error == SP_ERROR_UNSUPPORTED)
        {
          mGetSidetoneGain = false;
        }
      }

	  if(mOutputReference) {copyRefernceToOutput(input->pBuffer, input->nOffset, output->pBuffer);}
      returnOutputBuffer(output,input->nFlags,input->nTimeStamp);
      returnInputBuffer(input);
    }
    else
    {
      OstTraceFiltInst3(TRACE_ERROR, "ul wrapper: Mismatch between config and input Buffer size (mChansNb=%d , mRefChansNb=%d, input->nFilledLen =%d)",mChansNb,mRefChansNb,input->nFilledLen);
      returnInputBuffer(input);
    }
  }
}



//
// Helper function
//
void speech_proc_nmf_host_ul_wrapper::traceInputBuffer(OMX_BUFFERHEADERTYPE *input)
{
  //trace buffers (except if in TCM, as trace method will make byte access which will crash)
  if (UL_HYBRID_BUFFER_MEMORY != SP_MEM_TCM)
  {
    OMX_U32 size = mInputChannelSampleSize * 2 * mChansNb;
    OMX_U8  *ptr = (OMX_U8 *)((OMX_U32)input->pBuffer + input->nOffset);
    OstTraceFiltInst4(TRACE_SP_PORT_UL_IN, "SPEECH_PROC UL input port : size = %dbytes, nb channel = %d, sample freq = %dHz, address = %x", size, mChansNb, (mInputChannelSampleSize*(1000/PROCESSING_BUFFER_DURATION)), (t_uint32)ptr);
    for (OMX_U32 i =0; i < size; i+=MAX_OST_SIZE)
    {
      OstTraceFiltInstData(TRACE_SP_PORT_UL_IN, "SPEECH_PROC UL input port : data = %{int8[]}",(OMX_U8 *)(ptr + i), (size-i > MAX_OST_SIZE) ? MAX_OST_SIZE : size-i);
    }

    ptr  = (OMX_U8 *)((OMX_U32)ptr + size);
    size = mInputChannelSampleSize  * 2 * mRefChansNb;
    OstTraceFiltInst4(TRACE_SP_PORT_UL_REF, "SPEECH_PROC UL reference port : size = %dbytes, nb channel = %d, sample freq = %dHz, address = %x", size, mRefChansNb, (mInputChannelSampleSize*(1000/PROCESSING_BUFFER_DURATION)), (t_uint32)ptr);
    for (OMX_U32 i =0; i < size; i+=MAX_OST_SIZE)
    {
      OstTraceFiltInstData(TRACE_SP_PORT_UL_REF, "SPEECH_PROC UL reference port : data = %{int8[]}",(OMX_U8 *)(ptr + i), (size-i > MAX_OST_SIZE) ? MAX_OST_SIZE : size-i);
    }
  }
}

void speech_proc_nmf_host_ul_wrapper::returnInputBuffer(OMX_BUFFERHEADERTYPE *input)
{
  input->nFilledLen = 0;
  input->nOffset    = 0;
  mPorts[INPUT_PORT].dequeueAndReturnBuffer();
}

void speech_proc_nmf_host_ul_wrapper::returnOutputBuffer(OMX_BUFFERHEADERTYPE *output, OMX_U32 flags, OMX_TICKS timestamp)
{
  output->nFlags     = flags;
  output->nTimeStamp = timestamp;
  output->nOffset    = 0;
  output->nFilledLen = mOutputChannelSampleSize * 2;

  if(output->nFlags & OMX_BUFFERFLAG_EOS)
  {
    OstTraceFiltInst1(TRACE_FLOW, "ul_wrapper: OMX_EventBufferFlag on port %d sent to the speechcontroller", OUTPUT_PORT);
    proxy.eventHandler(OMX_EventBufferFlag, OUTPUT_PORT, output->nFlags);
  }

  //trace buffer
  OMX_U32 size = output->nFilledLen;
  OMX_U8  *ptr = (OMX_U8 *)((OMX_U32)output->pBuffer + output->nOffset);
  OstTraceFiltInst3(TRACE_SP_PORT_UL_OUT, "SPEECH_PROC UL output port : size = %dbytes, nb channel = 1, sample freq = %dHz, address = %x", size, (mOutputChannelSampleSize*(1000/PROCESSING_BUFFER_DURATION)), (t_uint32)ptr);
  for (OMX_U32 i =0; i < size; i+=MAX_OST_SIZE)
  {
    OstTraceFiltInstData(TRACE_SP_PORT_UL_OUT, "SPEECH_PROC UL output port : data = %{int8[]}",(OMX_U8 *)(ptr + i), (size-i > MAX_OST_SIZE) ? MAX_OST_SIZE : size-i);
  }

  OstTraceFiltInst2(TRACE_DEBUG, "Speech_proc/ul_wrapper: speech_proc_nmf_host_ul_wrapper::returnOutputBuffer  latency control (ul_wrapper, output) (timestamp = 0x%x 0x%x us)", (int)(output->nTimeStamp >> 32), (unsigned int)(output->nTimeStamp & 0xffffffffu));
  mPorts[OUTPUT_PORT].dequeueAndReturnBuffer();
}

void speech_proc_nmf_host_ul_wrapper::copyRefernceToOutput(OMX_U8* input, OMX_U32 input_offset, OMX_U8 *output)
{
  // skip input data and poitn to reference data
  t_sint16 *reference_data = (t_sint16 *)((t_uint32)input + input_offset + (mInputChannelSampleSize * 2 * mChansNb));

  // recopy only first channel of reference data
  memcpy((void *)output, (const void *)reference_data, (size_t)(mOutputChannelSampleSize * 2));
}
