/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file     dl_wrapper.cpp
 * \brief    NMF wrapper for downlink algorithms
 * \author   ST-Ericsson
 */
/*****************************************************************************/
#include <speech_proc/nmf/host/dl_wrapper.nmf>
#include "speech_proc_config.h"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "speech_proc_nmf_host_dl_wrapper_src_dl_wrapperTraces.h"
#endif

#define MAX_OST_SIZE 56 // TO BE REMOVED

//
// PROVIDED INTERFACES
//
void METH(start)(void)
{
  mProcessingCtx    = 0 ;
  mAlgo             = SP_DOWNLINK_NO_ALGO;
  mInExecutingState = false;
  mIsConfigured     = false;
  mSampleFreqIn     = FREQ_UNKNOWN;
  mSampleFreqOut    = FREQ_UNKNOWN;
  mGetSidetoneGain  = true;
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

  mPorts[INPUT_PORT].init (InputPort, false, false, 0, 0, NB_MAX_BUFFER, &inputport,   INPUT_PORT,
                           (initFsm.portsDisabled & (1<<INPUT_PORT)), (initFsm.portsTunneled & (1<<INPUT_PORT)),  this);

  mPorts[OUTPUT_PORT].init(OutputPort,false ,false, 0, 0, NB_MAX_BUFFER, &outputport, OUTPUT_PORT,
                           (initFsm.portsDisabled & (1<<OUTPUT_PORT)), (initFsm.portsTunneled & (1<<OUTPUT_PORT)), this);

  init(2, mPorts, &proxy, &me, invalid_state);
}

void METH(setTunnelStatus)(t_sint16 portIdx, t_uint16 isTunneled)
{
  mPorts[portIdx].setTunnelStatus((bool)isTunneled);
}


void METH(initLib)(t_uint16 bypass)
{
  unsigned int version = 0;
  speech_proc_error_t error;

  // Init processing lib
  if(bypass){ mAlgo = SP_DOWNLINK_NO_ALGO; }
  else      { mAlgo = SP_DOWNLINK_ALGO; }

  error = speech_proc_init(mAlgo, &mProcessingCtx, &version);
  ARMNMF_DBC_ASSERT(error == SP_ERROR_NONE);
  context.newLibraryContext(mProcessingCtx,(t_uint16)SP_DOWNLINK_ALGO, (t_uint32)version);
}


void METH(configure)(t_uint16 freq_in, t_uint16 freq_out,
                     t_uint16 nb_channel_main, t_uint16 nb_channel_ref,
                     t_uint16 port_enabled)
{
  ARMNMF_DBC_ASSERT((freq_in == 8000) || (freq_out == 16000));
  ARMNMF_DBC_ASSERT(nb_channel_main == 1);

  // Configure port settings
  mInputChannelSampleSize  = (freq_in / 1000)  * PROCESSING_BUFFER_DURATION; // size in samples
  mOutputChannelSampleSize = (freq_out / 1000) * DL_OUTPUT_BUFFER_DURATION;  // size in samples

  if (freq_in == 8000)  mSampleFreqIn  = FREQ_8KHZ;
  else                  mSampleFreqIn  = FREQ_16KHZ;
  if (freq_out == 8000) mSampleFreqOut = FREQ_8KHZ;
  else                  mSampleFreqOut = FREQ_16KHZ;

  speech_proc_settings_t mode;
  mode.resolution = 16;
  mode.framesize  = (PROCESSING_BUFFER_DURATION / PROCESSING_COUNT);

  speech_proc_port_settings_t inports;
  inports.nb_ports = 1;
  inports.port[SP_MAIN_PORT].enabled     = (port_enabled & (1 << SP_MAIN_PORT));
  inports.port[SP_MAIN_PORT].interleaved = 0;
  inports.port[SP_MAIN_PORT].nb_channels = 1;
  inports.port[SP_MAIN_PORT].samplerate  = freq_in;

  speech_proc_port_settings_t outports;
  outports.nb_ports = 1;
  outports.port[SP_MAIN_PORT].enabled     = (port_enabled & (1 << (SP_MAIN_PORT + inports.nb_ports)));
  outports.port[SP_MAIN_PORT].interleaved = 0;
  outports.port[SP_MAIN_PORT].nb_channels = 1;
  outports.port[SP_MAIN_PORT].samplerate  = freq_out;

  //OstTraceFiltInst3(TRACE_FLOW,"Downlink lib configuration : freq in = %dHz, freq out = %dHz, nb_channel = %d", freq_in, freq_out, nb_channel_main);

  if(mProcessingCtx)
  {
    speech_proc_error_t error;
    error = speech_proc_set_mode(mProcessingCtx, &mode, &inports, &outports);
    ARMNMF_DBC_ASSERT(error == SP_ERROR_NONE);
    uplink.resetProcessingInfo();
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


void METH(newFormat)(t_sample_freq sample_freq, t_uint16 chans_nb, t_uint16 sample_size)
{

}

void METH(fillThisBuffer)(OMX_BUFFERHEADERTYPE_p buffer)
{
  Component::deliverBuffer(OUTPUT_PORT,  buffer);
}


void METH(emptyThisBuffer)(OMX_BUFFERHEADERTYPE_p buffer)
{
  ARMNMF_DBC_ASSERT(((OMX_BUFFERHEADERTYPE *)buffer)->nOffset == 0);
  Component::deliverBufferCheck(INPUT_PORT,  buffer);
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
  ack_uplink.ackProcessingInfo();
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
// COMPONENT methods
//
void speech_proc_nmf_host_dl_wrapper::reset()
{
   while(mInfoPtr[mNextInfoRead])
   {
     OstTraceFiltInst2(TRACE_DEBUG, "dl_wrapper: purge ul info 0x%x at index %d", (int)mInfoPtr[mNextInfoRead], mNextInfoRead);
     speech_proc_set_processing_info(mProcessingCtx, mInfoPtr[mNextInfoRead]);
     mInfoPtr[mNextInfoRead] = (void *)0;
     mNextInfoRead = (mNextInfoRead + 1) % NB_INFO_EXCHANGE;
   }
   ack_uplink.ackProcessingInfo();
}

void speech_proc_nmf_host_dl_wrapper::disablePortIndication(t_uint32 portIdx)
{
  mIsConfigured = false;
  uplink.resetProcessingInfo();
}

void speech_proc_nmf_host_dl_wrapper::enablePortIndication(t_uint32 portIdx)
{

}

void speech_proc_nmf_host_dl_wrapper::flushPortIndication(t_uint32 portIdx)
{

}

void speech_proc_nmf_host_dl_wrapper::sendCommand(OMX_COMMANDTYPE cmd, t_uword param)
{
  Component::sendCommand(cmd, param);

  if (cmd == OMX_CommandStateSet)
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

void speech_proc_nmf_host_dl_wrapper::process(void)
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
    OMX_BUFFERHEADERTYPE    *input  = mPorts[INPUT_PORT].getBuffer(0);
    OMX_BUFFERHEADERTYPE    *output = mPorts[OUTPUT_PORT].getBuffer(0);
    speech_proc_error_t      error;
    void                    *info;

    // process
    ARMNMF_DBC_ASSERT((OMX_U32)(mOutputChannelSampleSize * 2) <= output->nAllocLen);

    // read uplink information
    while(mInfoPtr[mNextInfoRead])
    {
       speech_proc_error_t error;
       OstTraceFiltInst2(TRACE_DEBUG, "dl_wrapper: read ul info 0x%x at index %d", (int)mInfoPtr[mNextInfoRead], mNextInfoRead);
       error = speech_proc_set_processing_info(mProcessingCtx, mInfoPtr[mNextInfoRead]);
       if(error != SP_ERROR_NONE)
       {
          OstTraceFiltInst1(TRACE_ERROR, "dl_wrapper: speech_proc_set_processing_info return error %d", error);
       }
       mInfoPtr[mNextInfoRead] = (void *)0;
       mNextInfoRead = (mNextInfoRead + 1) % NB_INFO_EXCHANGE;
    }
    ack_uplink.ackProcessingInfo();

    if(input->nFilledLen)
    {
      if((OMX_U32)(mInputChannelSampleSize  * 2) != input->nFilledLen)
      {
        OMX_S32 padding_byte = (OMX_S32)(mInputChannelSampleSize  * 2) - (OMX_S32)input->nFilledLen;
        ARMNMF_DBC_ASSERT(padding_byte > 0);

        // pad with zero.
        OMX_U8 *ptr = (OMX_U8 *)((OMX_U32)input->pBuffer + input->nFilledLen);
        for (OMX_S32 i=0;i<padding_byte;i++) { *ptr++ = 0;}

        if(! input->nFlags & OMX_BUFFERFLAG_EOS)
        {
          OstTraceFiltInst1(TRACE_ERROR, "dl_wrapper: need to add %d padding sample(s)", (padding_byte/2));
        }
      }

      traceInputBuffer(input);

      for (int i=0; i< PROCESSING_COUNT; i++)
      {
        mInputBuffer   = (t_sint16 *)((t_uint32)input->pBuffer + input->nOffset + (((mInputChannelSampleSize * 2)/PROCESSING_COUNT) * i));
        mOutputBuffer  = (t_sint16 *)((t_uint32)output->pBuffer + (((mOutputChannelSampleSize * 2)/PROCESSING_COUNT) * i));


        OstTraceFiltInst0(TRACE_FLOW, "start dl process");
        error = speech_proc_process(mProcessingCtx, &mInputBuffer, 1, &mOutputBuffer, 1, (const speech_proc_frame_info_t *)NULL);
        ARMNMF_DBC_ASSERT(error == SP_ERROR_NONE);
        OstTraceFiltInst0(TRACE_FLOW, "end dl process");
      }

      // get information to share with UL library
      if (mInfoFifoFull == FALSE)
      {
        info = speech_proc_get_processing_info(mProcessingCtx);
        if(info)
        {
          mInfoFifoFull = uplink.setProcessingInfo(info);
        }
      }
      else
      {
        OstTraceFiltInst0(TRACE_WARNING, "dl wrapper: cannot send info to UL, fifo full");
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

      output->nFilledLen = mOutputChannelSampleSize * 2;
    }
    else
    {
      // Cscall send empty buffer with OMX_BUFFERFLAG_EOS flag set a the end of the call
      ARMNMF_DBC_ASSERT(input->nFlags & OMX_BUFFERFLAG_EOS);
      output->nFilledLen = 0;
    }

    returnOutputBuffer(output, input->nFlags, input->nTimeStamp);
	returnInputBuffer(input);
  }
}



//
// Helper function
//
void speech_proc_nmf_host_dl_wrapper::traceInputBuffer(OMX_BUFFERHEADERTYPE *input)
{
  //trace buffer
  OMX_U32 size = input->nFilledLen;
  OMX_U8  *ptr = (OMX_U8 *)((OMX_U32)input->pBuffer + input->nOffset);
  OstTraceFiltInst3(TRACE_SP_PORT_DL_IN, "SPEECH_PROC DL input port : size = %dbytes, nb channel = 1, sample freq = %dHz, address = %x", size, (mInputChannelSampleSize*(1000/PROCESSING_BUFFER_DURATION)), (t_uint32)ptr);
  for (OMX_U32 i =0; i < size; i+=MAX_OST_SIZE)
  {
    OstTraceFiltInstData(TRACE_SP_PORT_DL_IN, "SPEECH_PROC DL input port : data = %{int8[]}",(OMX_U8 *)(ptr + i), (size-i > MAX_OST_SIZE) ? MAX_OST_SIZE : size-i);
  }
}

void speech_proc_nmf_host_dl_wrapper::returnInputBuffer(OMX_BUFFERHEADERTYPE *input)
{
  input->nFilledLen = 0;
  input->nOffset    = 0;
  mPorts[INPUT_PORT].dequeueAndReturnBuffer();
}

void speech_proc_nmf_host_dl_wrapper::returnOutputBuffer(OMX_BUFFERHEADERTYPE *output, OMX_U32 flags, OMX_TICKS timestamp)
{
  output->nFlags     = flags;
  output->nTimeStamp = timestamp;
  output->nOffset    = 0;

  if(output->nFlags & OMX_BUFFERFLAG_EOS)
  {
    OstTraceFiltInst1(TRACE_FLOW, "dl_wrapper: OMX_EventBufferFlag on port %d sent to the speechcontroller", OUTPUT_PORT);
    proxy.eventHandler(OMX_EventBufferFlag, OUTPUT_PORT, output->nFlags);
  }

  //trace buffer (except if in TCM, as trace method will make byte access which will crash)
  if (DL_HYBRID_BUFFER_MEMORY != SP_MEM_TCM)
  {
    OMX_U32 size = output->nFilledLen;
    OMX_U8  *ptr = (OMX_U8 *)((OMX_U32)output->pBuffer + output->nOffset);
    OstTraceFiltInst3(TRACE_SP_PORT_DL_OUT, "SPEECH_PROC DL output port : size = %dbytes, nb channel = 1, sample freq = %dHz, address = %x", size, (mOutputChannelSampleSize*(1000/DL_OUTPUT_BUFFER_DURATION)), (t_uint32)ptr);
    for (OMX_U32 i =0; i < size; i+=MAX_OST_SIZE)
    {
      OstTraceFiltInstData(TRACE_SP_PORT_DL_OUT, "SPEECH_PROC DL output port : data = %{int8[]}",(OMX_U8 *)(ptr + i), (size-i > MAX_OST_SIZE) ? MAX_OST_SIZE : size-i);
    }
  }

  OstTraceFiltInst2(TRACE_DEBUG, "Speech_proc/dl_wrapper: speech_proc_nmf_host_dl_wrapper::returnOutputBuffer  latency control (dl_wrapper, output) (timestamp = 0x%x 0x%x us)", (int)(output->nTimeStamp >> 32), (unsigned int)(output->nTimeStamp & 0xffffffffu));

  mPorts[OUTPUT_PORT].dequeueAndReturnBuffer();
}
