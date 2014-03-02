/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   protocolhandler.cpp
 * \brief  The Protocol Handler is responsible for sending Encoded data to the modem, via the SHM driver.  Each frame is encoded into an ISI message, - ISI is the protocol agreed by the modem and APE. It must decode these messages and act upon them.  For example, messages containing audio frames are unpacked and the audio data is passed to the next OpenMAX component in the CS call graph.  When control messages are received the Protocol Handler can make changes to the configuration of the audio  for example if the sample rate switches during a call due to a handover from narrowband to wideband.  Audio resynchronisation is managed by Protocol Handler as well.
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include <cscall/nmf/host/protocolhandler.nmf>
#include "cscall_time.h"
#include "ENS_Component.h"
#include <string.h>

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cscall_nmf_host_protocolhandler_src_protocolhandlerTraces.h"
#endif

#define LOG_TAG "CSCALL_PH"
#include "linux_utils.h"

// MMPROBE traces
#ifdef MMPROBE_ENABLED
#include "t_mm_probe.h"
#include "r_mm_probe.h"
#define MMPROBE_TRACE_DATA(dir,buff) \
    if (mmprobe_status(dir)) {\
        SpeechData_t sd; \
        sd.system = mNetwork; \
        sd.codec = mCodecInUse; \
        sd.dtx = (short)mCscallCodecStatus.encDtx;			  \
        memcpy(sd.data, buff->pBuffer, buff->nFilledLen + (buff->nFilledLen%4)); \
        mmprobe_probe(dir, &sd, buff->nFilledLen + (buff->nFilledLen%4) + 3*sizeof(short));\
    }
#else
#define MMPROBE_TRACE_DATA(dir,buff)
#endif


#define CSCALL_20MS_FRAME_VALUE_IN_US 20000 // 20ms corresponds to 20000 us
#define CSCALL_2G_FRAME_AVERAGE       20000
#define CSCALL_ACCEPTABLE_DL_JITTER    2000
#define CSCALL_ACCEPTABLE_UL_JITTER    2000
#define CSCALL_DL_SECURITY_MARGIN      7000
#define CSCALL_MIN_DL_CONSECUTIVE_FRAME 10000


/**
 * Constructor
 * */
cscall_nmf_host_protocolhandler::cscall_nmf_host_protocolhandler(void)
{
  codecStatusReset();

  mDecoderBufferHeader.nSize = sizeof(OMX_BUFFERHEADERTYPE);
  getOmxIlSpecVersion(&mDecoderBufferHeader.nVersion);
  mDecoderBufferHeader.pBuffer              = 0;
  mDecoderBufferHeader.nAllocLen            = 0;
  mDecoderBufferHeader.nFilledLen           = 0;
  mDecoderBufferHeader.nOffset              = 0;
  mDecoderBufferHeader.pAppPrivate          = 0;
  mDecoderBufferHeader.pPlatformPrivate     = 0;
  mDecoderBufferHeader.hMarkTargetComponent = 0;
  mDecoderBufferHeader.pMarkData            = 0;
  mDecoderBufferHeader.nTickCount           = 0;
  mDecoderBufferHeader.nTimeStamp           = 0;
  mDecoderBufferHeader.nFlags               = 0;
  mDecoderBufferHeader.pOutputPortPrivate   = 0;
  mDecoderBufferHeader.nInputPortIndex      = 0;
  mDecoderBufferHeader.nOutputPortIndex     = 0;
  mDecoderBufferHeader.pInputPortPrivate    = 0;
  mDecoderBufferHeader.nInputPortIndex      = 0;
  mDecoderBufferHeader.nOutputPortIndex     = 0;

  mEncoderBufferHeader.nSize = sizeof(OMX_BUFFERHEADERTYPE);
  getOmxIlSpecVersion(&mEncoderBufferHeader.nVersion);
  mEncoderBufferHeader.pBuffer              = 0;
  mEncoderBufferHeader.nAllocLen            = 0;
  mEncoderBufferHeader.nFilledLen           = 0;
  mEncoderBufferHeader.nOffset              = 0;
  mEncoderBufferHeader.pAppPrivate          = 0;
  mEncoderBufferHeader.pPlatformPrivate     = 0;
  mEncoderBufferHeader.hMarkTargetComponent = 0;
  mEncoderBufferHeader.pMarkData            = 0;
  mEncoderBufferHeader.nTickCount           = 0;
  mEncoderBufferHeader.nTimeStamp           = 0;
  mEncoderBufferHeader.nFlags               = 0;
  mEncoderBufferHeader.pOutputPortPrivate   = 0;
  mEncoderBufferHeader.nInputPortIndex      = 0;
  mEncoderBufferHeader.nOutputPortIndex     = 0;
  mEncoderBufferHeader.pInputPortPrivate    = 0;
  mEncoderBufferHeader.nInputPortIndex      = 0;
  mEncoderBufferHeader.nOutputPortIndex     = 0;

  mProtocol               = 0;
  mNetwork                = NETWORK_NONE;
  mCodecInUse             = CODEC_NONE;
  mDownlinkBufferCount    = 0;
  mNbDropFrame            = 0;
  mNbULFrame              = 0;
  mNbDLFrame              = 0;
  mNbTimingReport         = 0;
  mNbCodingFormat         = 0;
  mUplinkBufferCount      = 0;
  mWaitStatus             = 0;
  memset(&mCodecReq,0,sizeof(mCodecReq));
  memset(&mCscallCodecConfig,0,sizeof(Cscall_Codec_Config_t));
  mNewCodecReqReceived         = NO_NEW_CODEC_REQ_PENDING;
  mNewCodecReqApplied_decoder  = 0;
  mNewCodecReqApplied_encoder  = 0;
  mPropagateStartTime          = false;
  mDLTimeStamp                 = 0;
  mLastReceivedDownlinkMessage = 0;
  mLastSentDownlinkMessage     = 0;
  mTimerStarted                = false;
  mLastReceivedTimingReport    = 0;
  mTimingReportTime            = 0;
  mModemAdjustment             = 1500;
  mPatternDetectionNeeded      = 0;
  mMargins.nbMargin            = 0;
  mMargins.wbMargin            = 0;
  mCurrentMargin               = 0;
  mGSM445                      = -1;
  mNbDlFrameDidNotUpdateUl     = 0;
  mDLTimerExpired              = 0;
  mUseLoopBackMode             = false;

  for (int i=0; i<CODEC_BUFFER_SIZE_IN_BYTE ;i++)
  {
    mDecoderBuffer[i] = 0;
    mEncoderBuffer[i] = 0;
  }

  mUplinkTimingInfo = NULL;
  mNextDeliveryTime = 0;

  mAlpha            = 1.0/500.0; // 10seconds
};


////////////////////////////////////////////
// Component interface
////////////////////////////////////////////
/**
 * Reset protocolhandler and every codec
 */
void cscall_nmf_host_protocolhandler::reset()
{
  mWaitStatus                 = 0;
  mCodecInUse                 = CODEC_NONE;
  mNetwork                    = NETWORK_NONE;
  mNewCodecReqReceived        = NO_NEW_CODEC_REQ_PENDING;
  mNewCodecReqApplied_decoder = 0;
  mNewCodecReqApplied_encoder = 0;
  mPropagateStartTime         = false;
  mDLTimeStamp                = 0;
  mNbDlFrameDidNotUpdateUl    = 0;
  codecStatusReset();
  setWaitOn(DOWNLINK_PORT);
  setWaitOn(ENCODER_PORT);
}

/**
 * Called when a protocolhandler port is disabled (Empty function)
 *
 * \param portIdx is the index of the disabled port
 */
void cscall_nmf_host_protocolhandler::disablePortIndication(t_uint32 portIdx)
{
  OstTraceFiltInst1 (TRACE_ALWAYS, "Cscall/Protocolhandler: disablePortIndication() called (portIdx=%d)",portIdx);
}

/**
 * Called when a port is enabled (Empty fonction)
 *
 * \param portIdx is the index of the enabled port
 */
void cscall_nmf_host_protocolhandler::enablePortIndication(t_uint32 portIdx)
{
  OstTraceFiltInst1 (TRACE_ALWAYS, "Cscall/Protocolhandler: enablePortIndication() called (portIdx=%d)",portIdx);
}

/**
 * Called when a port is flushed (Empty fonction)
 *
 * \param portIdx is the index of the flushed port
 */
void cscall_nmf_host_protocolhandler::flushPortIndication(t_uint32 portIdx)
{
  OstTraceFiltInst1 (TRACE_ALWAYS, "Cscall/Protocolhandler: flushPortIndication() called (portIdx=%d)",portIdx);
}

/**
 * Main process function.
 * Call each time protocolhandler receives a buffer on any port in executing state.
 *
 */
void cscall_nmf_host_protocolhandler::process(void)
{
  if(isWaitingOn(UPLINK_PORT) &&
     (mPorts[UPLINK_PORT].queuedBufferCount() != 0))
  {
    if(!isWaitingOn(ENCODER_PORT))
    {
      sendCodecFrame();
      if(isWaitingOn(DOWNLINK_PORT)) resetWaitOn(UPLINK_PORT);
    }
    else if(!isWaitingOn(DOWNLINK_PORT))
    {
      ARMNMF_DBC_ASSERT(mPorts[DOWNLINK_PORT].queuedBufferCount() != 0);
      processModemMessage();
      resetWaitOn(UPLINK_PORT);
    }
    else
    {
      ARMNMF_DBC_ASSERT(0);
    }
  }

  if(isWaitingOn(DECODER_PORT) &&
     (mPorts[DECODER_PORT].queuedBufferCount() != 0))
  {
    ARMNMF_DBC_ASSERT(mPorts[DOWNLINK_PORT].queuedBufferCount() != 0);
    processModemMessage();
    resetWaitOn(DECODER_PORT);
  }

  if(isWaitingOn(ENCODER_PORT) &&
     (mPorts[ENCODER_PORT].queuedBufferCount() != 0))
  {
    sendCodecFrame();
  }

  for(int i=0;
      isWaitingOn(DOWNLINK_PORT) &&
          (mPorts[DOWNLINK_PORT].queuedBufferCount() != 0) &&
          processModemMessage() &&
          i < mDownlinkBufferCount;
      i++) ;

  // each buffer had been processed, can now handle dl security timer
  if (mDLTimerExpired){
      if (mCodecInUse != CODEC_NONE) {
          if (mLastSentDownlinkMessage != 0 /* Do not send fake frame when no frame sent*/) {
              int diff = (int)(getTime() - mLastSentDownlinkMessage);
              if (diff > CSCALL_MIN_DL_CONSECUTIVE_FRAME) {
                  mDLTimeStamp += CSCALL_20MS_FRAME_VALUE_IN_US;
                  OstTraceFiltInst2 (TRACE_DEBUG, "Cscall/Protocolhandler: DL-frame sent latency control (protocolhandler, output) (timestamp = 0x%x 0x%x us)", mDLTimeStamp>>32,(mDLTimeStamp)&0xffffffffu);
                  OstTraceFiltInst1 (TRACE_ALWAYS, "Cscall/Protocolhandler: ask Downlink to generate fake frame (diff=%d)",diff);
                  dl_fakeFrame.sendFakeFrame(mDLTimeStamp);
                  mLastSentDownlinkMessage = getTime();
              }
          }
      }
  }
  mDLTimerExpired = false;
}


////////////////////////////////////////////
// NMF interfaces
////////////////////////////////////////////
/////////////////////
// lifecycle.stopper
/////////////////////
void METH(stop)(void)
{
  if(mProtocol)
  {
	delete mProtocol;
	mProtocol = 0;
  }
}

/////////////////
// fsminit
/////////////////
/**
 * Initialisation of the state-machine
 */
void METH(fsmInit)(fsmInit_t initFsm)
{
  bool is_downlink_buffer_provider = true;
  bool is_uplink_buffer_provider = true;

  // trace init (mandatory before port init)
  if (initFsm.traceInfoAddr){
    setTraceInfo(initFsm.traceInfoAddr, initFsm.id1);
	if(mProtocol)
	  mProtocol->setTraceInfo(initFsm.traceInfoAddr, initFsm.id1);
  }
  OstTraceFiltInst1 (TRACE_ALWAYS, "Cscall/Protocolhandler: fsmInit() called (id=%d)",initFsm.id1);

  //Start Encoder port
  OMX_BUFFERHEADERTYPE *BufEnc_list[1] = {&mEncoderBufferHeader};
  mEncoderBufferHeader.pBuffer         = mEncoderBuffer;
  mEncoderBufferHeader.nAllocLen       = CODEC_BUFFER_SIZE_IN_BYTE;
  mEncoderBufferHeader.nFilledLen      = 0;
  mEncoderBufferHeader.nFlags          = 0;
  mEncoderBufferHeader.nOffset         = 0;
  mPorts[ENCODER_PORT].init(InputPort, true, false, 0, BufEnc_list, 1, &inputport, ENCODER_PORT, false, true, this);

  //Start Decoder port
  OMX_BUFFERHEADERTYPE *BufDec_list[1] = {&mDecoderBufferHeader};
  mDecoderBufferHeader.pBuffer         = mDecoderBuffer;
  mDecoderBufferHeader.nAllocLen       = CODEC_BUFFER_SIZE_IN_BYTE;
  mDecoderBufferHeader.nFilledLen      = 0;
  mDecoderBufferHeader.nFlags          = 0;
  mDecoderBufferHeader.nOffset         = 0;
  mPorts[DECODER_PORT].init(OutputPort, true, false, 0, BufDec_list, 1, &outputport, DECODER_PORT, false, true, this);


  //Start Uplink port
  if(mUplinkBufferCount == 0) {
    is_uplink_buffer_provider = false;
    mUplinkBufferCount = 4 ;
  }
  mPorts[UPLINK_PORT].init(OutputPort, is_uplink_buffer_provider, false, 0, mUplinkBuffers, mUplinkBufferCount, &uplink, UPLINK_PORT, false, true, this);

  //Start Downlink port
  if(mDownlinkBufferCount == 0) {
    is_downlink_buffer_provider = false;
    mDownlinkBufferCount = 4 ;
  }
  mPorts[DOWNLINK_PORT].init(InputPort, is_downlink_buffer_provider, false, 0, mDownlinkBuffers, mDownlinkBufferCount, &downlink, DOWNLINK_PORT, false, true, this);

  init(4, mPorts, &proxy, &me, false);
}


/////////////////
// sendcommand
/////////////////
/**
 * Called during a transition of the state-machine.
 * Used to detect a transition to idle state, to cancel requests sent to modem
 */
void METH(sendCommand)(OMX_COMMANDTYPE cmd, t_uword param)
{
  Component::sendCommand(cmd, param);

  /* Detect transitions to idle */
  if ((cmd == OMX_CommandStateSet) && (param == OMX_StateIdle)) {
    if (!audiomodem.IsNullInterface()){
      OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/Protocolhandler: OMX_CommandStateSet OMX_StateIdle received, call audiomodem.cancelRequests()");
      audiomodem.cancelRequests() ;
    }
  }
}


/////////////////////////////////////////////
// cscall.nmf.host.protocolhandler.configure
/////////////////////////////////////////////
/**
 * Select protocol to use
 */
void METH(setProtocol)(CsCallProtocol_t protocol)
{
  mProtocol = Protocol::createProtocol(protocol);

  if(!mProtocol)
  {
	proxy.eventHandler(OMX_EventError, (t_uint32)OMX_ErrorInsufficientResources, 0);
	ARMNMF_DBC_ASSERT(0);
	return;
  }
}



/**
 * To receive the buffer used for communication with the modem
 *
 * \param uplinkBufferCount is the number of uplink buffers
 * \param uplinkBuffer is the table of buffers used for uplink
 * \param downlinkBufferCount is the number of uplink buffers
 * \param downlinkBuffer is a table of buffers used for downlink
 */
void METH(setModemBuffer)(t_uint16 uplinkBufferCount,   OMX_BUFFERHEADERTYPE_p *uplinkBuffer,
						  t_uint16 downlinkBufferCount, OMX_BUFFERHEADERTYPE_p *downlinkBuffer)
{
  ARMNMF_DBC_ASSERT(uplinkBufferCount<=NB_UL_MODEM_BUFFER);
  ARMNMF_DBC_ASSERT(downlinkBufferCount<=NB_DL_MODEM_BUFFER);

  mUplinkBufferCount = uplinkBufferCount;
  for(int i=0;i<mUplinkBufferCount;i++)
  {
    mUplinkBuffers[i] = (OMX_BUFFERHEADERTYPE *)uplinkBuffer[i];
  }

  mDownlinkBufferCount = downlinkBufferCount;
  for(int i=0;i<mDownlinkBufferCount;i++)
  {
    mDownlinkBuffers[i] = (OMX_BUFFERHEADERTYPE *)downlinkBuffer[i];
  }
}

/**
 * Set pointer for uplink timing measurement (type OMX_SYMBIAN_AUDIO_CONFIG_UPLINKTIMINGINFOTYPE, writed by protocolhandler and readed by the proxy)
 */
void METH(initTimeMeasurement)(void *ptr, t_uint32 duration_ms)
{
  mUplinkTimingInfo = static_cast<OMX_SYMBIAN_AUDIO_CONFIG_UPLINKTIMINGINFOTYPE *>(ptr);
  mAlpha = 1.0/(duration_ms/20.0);
}

/**
 * When uplink timing is read, need to reset it
 */
void METH(resetTimeMeasurement)(void)
{
  if (mUplinkTimingInfo) {
    mUplinkTimingInfo->nMaxTime     = 0x80000000;
    mUplinkTimingInfo->nAverageTime = 0x80000000;
    mUplinkTimingInfo->nMinTime     = 0x80000000;
  }
}

/*
 * Set Encoder Margin
 */
void METH(setTimingReqMargins)(UlTimingReqMargin_t margins)
{
  mMargins = margins;
  chooseCurrentMargin();
}


///////////////////////////////////////////////////
// cscall.nmf.host.protocolhandler.newCodecApplied
//////////////////////////////////////////////////
/**
 * The newCodecReq has been applied by the encoder
 */
void METH(encoder_newCodecApplied)(void)
{
  if (mNewCodecReqReceived) {
    mNewCodecReqApplied_encoder++;
    OstTraceFiltInst1 (TRACE_ALWAYS, "Cscall/Protocolhandler: Received newCodecApplied from encoder (mNewCodecReqApplied_encoder=%d)",mNewCodecReqApplied_encoder);
    newCodecApplied();
  } else {
    OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/Protocolhandler: Received newCodecApplied from encoder, but was not waiting for it");
  }
}

///////////////////////////////////////////////////
// cscall.nmf.host.protocolhandler.newCodecApplied
//////////////////////////////////////////////////
/**
 * The newCodecReq has been applied by the decoder
 *
 */
void METH(decoder_newCodecApplied)(void)
{
  if (mNewCodecReqReceived) {
    mNewCodecReqApplied_decoder++;
    OstTraceFiltInst1 (TRACE_ALWAYS, "Cscall/Protocolhandler: Received newCodecApplied from decoder (mNewCodecReqApplied_decoder=%d)", mNewCodecReqApplied_decoder);
    newCodecApplied();
  } else {
    OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/Protocolhandler: Received newCodecApplied from decoder, but was not waiting for it");
  }
}
/////////////////////
//// timer.api.alarm
/////////////////////
void METH(signal)() {
    OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/Protocolhandler: Downlink security timer expired");
    mDLTimerExpired = true;
    scheduleProcessEvent();
}

////////////////////////////////////
// protocol
///////////////////////////////////
/**
 * creates the correct protocol class able to handle the message
 */
void cscall_nmf_host_protocolhandler::autoDetectProtocol(OMX_BUFFERHEADERTYPE *msg)
{
  mProtocol = Protocol::autoDetectProtocol(msg);

  if(!mProtocol)
  {
	proxy.eventHandler(OMX_EventError, (t_uint32)OMX_ErrorInsufficientResources, 0);
	ARMNMF_DBC_ASSERT(0);
	return;
  }

  mProtocol->setTraceInfo(getTraceInfoPtr(), getId1());
}

////////////////////////////////////
// Codec / network
///////////////////////////////////
/**
 * Reset codec status
 */
void cscall_nmf_host_protocolhandler::codecStatusReset(void)
{
  // Default CodecStatus
  mCscallCodecStatus.NextSFN      = 0;
  mCscallCodecStatus.RequestedSFN = -1;
  mCscallCodecStatus.Nsync        = -1;
  mCscallCodecStatus.encDtx       = 0;

}

/**
 * We are waiting for wrappers to use the new codec requested, is it time to raise proxy event, and transmit again speech frames ?
 * */
void cscall_nmf_host_protocolhandler::newCodecApplied()
{
  if (mNewCodecReqReceived != NO_NEW_CODEC_REQ_PENDING)
  {
    if (mNewCodecReqApplied_decoder && mNewCodecReqApplied_encoder)
	{
      OstTraceFiltInst1 (TRACE_ALWAYS, "Cscall/Protocolhandler: newCodecReq applied by both decoder and encoder (mNewCodecReqReceived=%d)", mNewCodecReqReceived);
      OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/Protocolhandler: Raise newCodecReq event to proxy");
      network_update.newCodecReq(mCodecReq);
      if (mCodecReq.speech_codec == CODEC_NONE)
	  {
        endOfCall();
      }
      scheduleProcessEvent();
      mNewCodecReqReceived = NO_NEW_CODEC_REQ_PENDING;
      mNewCodecReqApplied_decoder = 0;
      mNewCodecReqApplied_encoder = 0;
    }
  }
}


/**
 * Reset all variable at the end of the call, send some status traces
 */
void cscall_nmf_host_protocolhandler::endOfCall(void)
{

  // coverity[check_return]
  OstTraceFiltInst2(TRACE_DEBUG, "Cscall/Protocolhandler: endOfCall() buffers in protocolhandler ports (encoder: %d, decoder: %d)", mPorts[ENCODER_PORT].queuedBufferCount(), mPorts[DECODER_PORT].queuedBufferCount());
  OstTraceFiltInst2(TRACE_DEBUG, "Cscall/Protocolhandler: endOfCall() Is waiting on encoder port: %d, decoder port %d", isWaitingOn(ENCODER_PORT)?1:0, isWaitingOn(DECODER_PORT)?1:0);
  // coverity[check_return]
  OstTraceFiltInst2(TRACE_DEBUG, "Cscall/Protocolhandler: endOfCall() buffers in protocolhandler ports (modem uplink: %d, modem downlink: %d)", mPorts[UPLINK_PORT].queuedBufferCount(), mPorts[DOWNLINK_PORT].queuedBufferCount());
  OstTraceFiltInst2(TRACE_DEBUG, "Cscall/Protocolhandler: endOfCall() Is waiting on modem uplink port: %d, modem downlink port %d", isWaitingOn(UPLINK_PORT)?1:0, isWaitingOn(DOWNLINK_PORT)?1:0);
  OstTraceFiltInst3 (TRACE_ALWAYS,"Cscall/Protocolhandler: endOfCall()  (mNbULFrame=%d) (mNbDLFrame=%d) (mNbDropFrame=%d)", mNbULFrame,mNbDLFrame,mNbDropFrame);
  OstTraceFiltInst2 (TRACE_ALWAYS,"Cscall/Protocolhandler: endOfCall()  (mNbTimingReport=%d) (mNbCodingFormat=%d)", mNbTimingReport,mNbCodingFormat);
  mNbDropFrame  = 0;
  mNbULFrame    = 0;
  mNbDLFrame    = 0;
  mNbTimingReport         = 0;
  mNbCodingFormat         = 0;
  codecStatusReset();
  mCodecInUse             = CODEC_NONE;
  mNetwork                = NETWORK_NONE;
  memset(&mCodecReq,0,sizeof(mCodecReq));
  mNewCodecReqReceived    = 0;
  mNewCodecReqApplied_decoder = 0;
  mNewCodecReqApplied_encoder = 0;
  mNbDlFrameDidNotUpdateUl    = 0;
}

////////////////////////////////////
// trace functions
///////////////////////////////////
/**
 * Dump buffers sent to modem
 */
void cscall_nmf_host_protocolhandler::traceULMsg(OMX_BUFFERHEADERTYPE *buffer)
{
  // Dump sent msg buffer
  OMX_U8 * ptr  =  (OMX_U8 *)(buffer->pBuffer + buffer->nOffset);
  OMX_U32 size  = buffer->nFilledLen;

  OstTraceFiltInst2(TRACE_CSCALL_UL_MODEM_MESSAGE, "Cscall/Protocolhandler: sent ul_msg - message type = %d (%d bytes)", mProtocol->getMsgType(buffer), size);
  for (OMX_U32 i = 0; i < size; i+=50)
  {
      OstTraceFiltInstData(TRACE_CSCALL_UL_MODEM_MESSAGE, "Cscall/Protocolhandler: sent ul_msg[]  = %{int8[]}", (OMX_U8 *)(ptr + i), (size-i > 50) ? 50 : size-i);
  }
}


/**
 * Dump buffers received from modem
 */
void cscall_nmf_host_protocolhandler::traceDLMsg(OMX_BUFFERHEADERTYPE *buffer)
{
  // Dump received msg buffer
  OMX_U8 * ptr  = (OMX_U8 *)(buffer->pBuffer + buffer->nOffset);
  OMX_U32 size  = mProtocol->getMsgLength(buffer);

  OstTraceFiltInst2(TRACE_CSCALL_DL_MODEM_MESSAGE, "Cscall/Protocolhandler: received dl_msg - message type = %d (%d bytes)", mProtocol->getMsgType(buffer), size);
  for (OMX_U32 i = 0; i < size; i+=50)
  {
      OstTraceFiltInstData(TRACE_CSCALL_DL_MODEM_MESSAGE, "Cscall/Protocolhandler: received dl_msg[]  = %{int8[]}", (OMX_U8 *)(ptr + i), (size-i > 50) ? 50 : size-i);
  }
}


////////////////////////////////////
// Main functions
///////////////////////////////////
/**
 * Read next message on downlink port, and call the corresponding process function
 * if enough buffer are available on uplink port (depending of message type)
 *
 */
bool cscall_nmf_host_protocolhandler::processModemMessage(void)
{
  OMX_BUFFERHEADERTYPE *buffer = mPorts[DOWNLINK_PORT].getBuffer(0);
  CsCallMessage_t       msg_type;
  t_uint16              nb_answer;
  OMX_BUFFERHEADERTYPE *answers[MAX_NB_ANSWER];
  bool                  message_processed = false;

  if(!mProtocol)
  {
	autoDetectProtocol(buffer);
	if(!mProtocol) return false;
  }

  msg_type  = mProtocol->getMsgType(buffer);
  nb_answer = mProtocol->getNbAnswer(msg_type);

  if(mPorts[UPLINK_PORT].queuedBufferCount() < nb_answer)
  {
	resetWaitOn(DOWNLINK_PORT);
	setWaitOn(UPLINK_PORT);
	return false;
  }
  else
  {
	int i;
	for(i=0;i<nb_answer;i++)
	{
	  answers[i] = mPorts[UPLINK_PORT].getBuffer(i);
	  memset(answers[i]->pBuffer, 0, answers[i]->nAllocLen);
	}
	for(;i<MAX_NB_ANSWER;i++)
	{
	  answers[i] = 0;
	}
  }

  switch(msg_type)
  {
	case MSG_MODEM_INFORMATION_RESPONSE:
	  {
		message_processed = informationResponse(buffer, answers);
		break;
	  }
	case MSG_CODING_FORMAT_REQUEST:
	  {
		OMX_BUFFERHEADERTYPE *decoder = 0;
		if(mPorts[DECODER_PORT].queuedBufferCount())
		  decoder = mPorts[DECODER_PORT].getBuffer(0);

        mNbCodingFormat++;
        message_processed = configureCodec(buffer, answers, decoder);
		break;
	  }
	case MSG_DOWNLINK_TIMING_CONFIGURATION:
	  {
		message_processed = configureDLTiming(buffer, answers);
		break;
	  }
	case MSG_TIMING_REPORT:
      {
        mNbTimingReport++;
		message_processed = configureULTiming(buffer, answers);
		break;
	  }
	case MSG_SPEECH_DATA_DOWNLINK:
	  {
        mNbDLFrame++;
		OstTraceFiltInst1 (TRACE_ALWAYS, "Cscall/Protocolhandler: downlink frame received (mNbDLFrame=%d)",mNbDLFrame);
        downlinkMessageReceived(buffer->nTimeStamp);
		if (mPorts[DECODER_PORT].queuedBufferCount() != 0)
		{
		  message_processed = transmitCodecFrame(buffer, mPorts[DECODER_PORT].getBuffer(0));
		}
		else
		{
		  // drop message
		  mNbDropFrame++;
		  OstTraceFiltInst2 (TRACE_WARNING, "Cscall/Protocolhandler: WARNING!! Drop downlink frame as no buffer available on DECODER_PORT (mNbDropFrame=%d) (mNbDLFrame=%d)",mNbDropFrame,mNbDLFrame);
		  message_processed = true;
		}
		break;
	  }
	case MSG_VOICE_CALL_STATUS:
	  {
		message_processed = voiceCallStatus(buffer, answers);
		break;
	  }
	case MSG_TEST_AUDIO_LOOP_REQUEST:
	  {
		// The modem ask to the APE to set a loop
		message_processed = configureApeLoop(buffer, answers);
		break;
	  }
	case MSG_TEST_MODEM_LOOP_REQUEST:
	case MSG_MODEM_INFORMATION_REQUEST:
	case MSG_CODING_FORMAT_RESPONSE:
	case MSG_UPLINK_TIMING_CONFIGURATION:
	case MSG_SPEECH_DATA_UPLINK:
	case MSG_TIMING_REPORT_RESPONSE:
	case MSG_TEST_AUDIO_LOOP_RESPONSE:
	  {
		OstTraceFiltInst1 (TRACE_ERROR, "Cscall/Protocolhandler: incorrect received Message (msg=%d)",msg_type);
		message_processed = true;
		break;
	  }
	default:
		OstTraceFiltInst1 (TRACE_ERROR, "Cscall/Protocolhandler: unknown msg received  (msg=%d)",msg_type);
		message_processed = true;
		break;
  }

  if(message_processed)
  {
	// return downlink buffer
	traceDLMsg(buffer);
	mPorts[DOWNLINK_PORT].dequeueAndReturnBuffer();
	setWaitOn(DOWNLINK_PORT);

	// send uplink buffer to modem
	for(int i=0; i<MAX_NB_ANSWER; i++)
	{
	  if(answers[i] == 0) break; //stop on first message set to null
      answers[i]->nTimeStamp = 0;
	  traceULMsg(answers[i]);
	  mPorts[UPLINK_PORT].dequeueAndReturnBuffer();
	}
  }
  return message_processed;
}


/**
 * process function for MSG_MODEM_INFORMATION_RESPONSE message.
 * simply call protocol specific function
 *
 */
bool cscall_nmf_host_protocolhandler::informationResponse(OMX_BUFFERHEADERTYPE *msg, OMX_BUFFERHEADERTYPE **answer)
{
  OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/Protocolhandler: informationResponse");
  return mProtocol->process_modem_information_response(msg,answer);
}

void cscall_nmf_host_protocolhandler::applyCodecConfig(void)
{
    CsCallCodec_t    previous_codec   = mCodecInUse;
    CsCallNetwork_t  previous_network = mNetwork;

    mCodecInUse = mPendingCodecInUse;
    mNetwork = mPendingNetwork;

    if((mCodecInUse != previous_codec) || (mNetwork != previous_network))
    {
        mNewCodecReqReceived = NEW_CODEC_REQ_PENDING_BLOCKING;
        if(previous_codec == CODEC_NONE) {
            mPropagateStartTime = true;
        }
        mNbDropFrame = 0;
    }
    else
    {
        mNewCodecReqReceived = NEW_CODEC_REQ_PENDING;
    }

    chooseCurrentMargin();
    OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/Protocolhandler: send config to Encoder wrapper");
    configEncoder.configureCodec(mCodecInUse, (void*)&mCscallCodecConfig);
    OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/Protocolhandler: send config to Decoder wrapper");
    configDecoder.configureCodec(mCodecInUse, (void*)&mCscallCodecConfig);

    if ((previous_codec == CODEC_NONE) && mPatternDetectionNeeded)
    {
        // will start timer
        timingReport(mLastReceivedTimingReport, mTimingReportTime);
        resetTimeMeasurement();
    }
}

/**
 * process function for MSG_CODING_FORMAT_RESPONSE message.
 *
 */
bool cscall_nmf_host_protocolhandler::configureCodec(OMX_BUFFERHEADERTYPE *msg, OMX_BUFFERHEADERTYPE **answer, OMX_BUFFERHEADERTYPE *decoder)
{
  // do not process new CODING_FORMAT_REQUEST until codec wrapper processed the previous one
  if(mNewCodecReqReceived == NO_NEW_CODEC_REQ_PENDING)
  {
	bool             process_ok;
	bool             config_updated   = false;
	bool             time_updated     = false;
	t_uint32         time             = 0;

	if(mProtocol->is_end_of_call(msg))
	{
	  if(decoder == 0)
	  {
		OstTraceFiltInst0 (TRACE_WARNING, "Cscall/Protocolhandler: WARNING!! Not able to send EOS, No available Buffer on DECODER_PORT. Reconfiguration kept pending in DL_port queue");
		return false;
	  }

	  // send last empty buffer with EOS flag
	  decoder->nFilledLen = 0;
	  decoder->nFlags |= OMX_BUFFERFLAG_EOS;
	  decoder->nOffset    = 0;
	  decoder->nTimeStamp = mDLTimeStamp + CSCALL_20MS_FRAME_VALUE_IN_US;
	  mPorts[DECODER_PORT].dequeueAndReturnBuffer();
	  OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/Protocolhandler: configureCodec() Send EOS on DECODER_PORT, informs uplink to stop timer");

	  // stop ul and dl timer
	  timingReport(0, 0);

	  OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/Protocolhandler: configureCodec() (codec = none). ###End of Call### ");
	}

	process_ok = mProtocol->process_coding_format_request(msg, answer, &mCscallCodecConfig, &mCscallCodecStatus, &mCodecReq, 
														  &mPendingCodecInUse, &mPendingNetwork, &config_updated, &time, &time_updated);

	// return if message has not been processed
	if(! process_ok)
	{
	  return false;
	}

	// apply new uplilnk timing if needed
	if(time_updated)
	{
	  timingReport(msg->nTimeStamp, time);
	}

	if(config_updated)
	{
	  OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/Protocolhandler: configureCodec(). Apply codec config immediately");
      applyCodecConfig();
	}
    else
    {
	  OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/Protocolhandler: configureCodec(). Delayed codec configuration");
    }
	return true;
  }
  else
  {
	return false;
  }
}


/**
 * process function for MSG_DOWNLINK_TIMING_CONFIGURATION message.
 *
 */
bool cscall_nmf_host_protocolhandler::configureDLTiming(OMX_BUFFERHEADERTYPE *msg, OMX_BUFFERHEADERTYPE **answer)
{
  bool      process_ok;
  bool      time_updated = false;
  t_uint32  time         = 0;

  process_ok = mProtocol->process_downlink_timing_configuration(msg, answer, &time, &time_updated);

  if(!process_ok) return false;

  // apply new uplilnk timing if needed
  if(time_updated)
  {
	timingReport(msg->nTimeStamp, time);
  }

  return true;
}

/**
 * process function for MSG_TIMING_REPORT message.
 *
 */
bool cscall_nmf_host_protocolhandler::configureULTiming(OMX_BUFFERHEADERTYPE *msg, OMX_BUFFERHEADERTYPE **answer)
{
  bool      process_ok;
  bool      time_updated = false;
  t_uint32  time         = 0;
  signed long long timeStamp = msg->nTimeStamp;

  process_ok = mProtocol->process_timing_report(msg, answer,
  		  	  	  	  	  	  mNextDeliveryTime,
  		  	  	  	  	  	  &time,
  		  	  	  	  	  	  &time_updated);

  if(!process_ok) return false;

  // apply new uplilnk timing if needed
  if(time_updated)
  {
	timingReport(timeStamp, time);
    resetTimeMeasurement();
  }

  return true;
}

/**
 * process function for MSG_TEST_MODEM_LOOP_REQUEST message.
 * The aim here is to put the APE in 'loopback' mode
 */
bool cscall_nmf_host_protocolhandler::configureApeLoop(OMX_BUFFERHEADERTYPE *msg, OMX_BUFFERHEADERTYPE **answer)
{
  bool      process_ok;

  process_ok = mProtocol->process_request_for_loop(msg, answer);

  if(!process_ok) return false;

  return true;
}


/**
 * process function for MSG_SPEECH_DATA_DOWNLINK message
 *
 */
bool cscall_nmf_host_protocolhandler::transmitCodecFrame(OMX_BUFFERHEADERTYPE *msg, OMX_BUFFERHEADERTYPE *decoder)
{
  if(mNewCodecReqReceived == NEW_CODEC_REQ_PENDING_BLOCKING)
  {
	mNbDropFrame++;
	OstTraceFiltInst1 (TRACE_WARNING, "Cscall/Protocolhandler: Drop downlink frame as decoder wrapper not ready (total drop = %d)",mNbDropFrame);
  }
  else
  {
	bool  process_ok;

	process_ok = mProtocol->process_speech_data_downlink(msg, decoder);
	if(!process_ok) return false;

	// send buffer to decoder (only if decoder buffer is not empty)
	if(decoder->nFilledLen) {
      int diff=(int)(getTime() - mLastSentDownlinkMessage);
      if ((diff > CSCALL_MIN_DL_CONSECUTIVE_FRAME) || (mLastSentDownlinkMessage == 0) || (diff < 0)) {
        // handle timset stamp
        if(mPropagateStartTime)
        {
          decoder->nTimeStamp = 0;
          decoder->nFlags    |= OMX_BUFFERFLAG_STARTTIME;
          mPropagateStartTime = false;
        }
        else
        {
          decoder->nTimeStamp = mDLTimeStamp + CSCALL_20MS_FRAME_VALUE_IN_US;
        }
        mDLTimeStamp = decoder->nTimeStamp;
        OstTraceFiltInst2 (TRACE_DEBUG, "Cscall/Protocolhandler: DL-frame sent latency control (protocolhandler, output) (timestamp = 0x%x 0x%x us)", (decoder->nTimeStamp)>>32,(decoder->nTimeStamp)&0xffffffffu);
        MMPROBE_TRACE_DATA(MM_PROBE_RX_SPD_INPUT,decoder);
        OstTraceFiltInst2 (TRACE_DEBUG, "Cscall/Protocolhandler: Send encoded frame from modem to decoder (buf=0x%x) (decoder_buffer->nFilledLen=%d)", decoder->pBuffer[0],decoder->nFilledLen);
        mPorts[DECODER_PORT].dequeueAndReturnBuffer();
        mLastSentDownlinkMessage = getTime();
        mDLTimerExpired = false;
      } else {
        OstTraceFiltInst1 (TRACE_WARNING, "Cscall/Protocolhandler: modem frame received too late (diff=%d)",diff);
      }
    }
  }
  return true;
}

/**
 * process function fo MSG_VOICE_CALL_STATUS message
 *
 */
bool cscall_nmf_host_protocolhandler::voiceCallStatus(OMX_BUFFERHEADERTYPE *msg, OMX_BUFFERHEADERTYPE **answer)
{
  bool  process_ok;
  bool  connected = true;
  process_ok = mProtocol->process_voice_call_status(msg, answer, &connected);

  if(!process_ok) return false;

  if(connected == false)
  {
	// TODO mute downlink & uplink
  }
  else
  {
	// TODO unmute downlink & uplink
  }

  return true;
}


/**
 * process new buffer send by encoder
 *
 */
void cscall_nmf_host_protocolhandler::sendCodecFrame(void)
{
  //Restart timer, must always be done
  if (mTimerStarted) {
    mNextDeliveryTime += CSCALL_20MS_FRAME_VALUE_IN_US;
    int nextUlFrame = ( (int)(mNextDeliveryTime - mCurrentMargin - getTime())) % 40000;
    while (nextUlFrame < 0) {
      // TODO FIXME : This should not happen, or a problem still exists somewhere in the compensation logic
	  OstTraceFiltInst1(TRACE_WARNING, "Cscall/Protocolhandler: Next frame should start in the past (nextUlFrame=%d), it seems that there is jitter in dl (nextUlFrame=%d us)", nextUlFrame);
      nextUlFrame += CSCALL_20MS_FRAME_VALUE_IN_US;
      mNextDeliveryTime += CSCALL_20MS_FRAME_VALUE_IN_US;
    }

    ul_timer.startTimer((t_uint32)nextUlFrame,0);
    OstTraceFiltInst1(TRACE_DEBUG, "Cscall/Protocolhandler: sendCodecFrame nextUlFrame should start in %d us", nextUlFrame);
  }

  if(mPorts[UPLINK_PORT].queuedBufferCount() == 0)
  {
	// wait uplink buffer
	resetWaitOn(ENCODER_PORT);
	setWaitOn(UPLINK_PORT);
    OstTraceFiltInst1 (TRACE_WARNING, "Cscall/Protocolhandler: sendCodecFrame() WARNING: no buffer available on UPLINK_PORT (mNbULFrame=%d)",mNbULFrame);

	return;
  }

  OMX_BUFFERHEADERTYPE *encoder_buffer = mPorts[ENCODER_PORT].getBuffer(0);
  OMX_BUFFERHEADERTYPE *modem_buffer   = mPorts[UPLINK_PORT].getBuffer(0);
  bool                  config_updated = false;
  bool                  process_ok     = false;

  if (mNewCodecReqReceived == NEW_CODEC_REQ_PENDING_BLOCKING)
  {
	OstTraceFiltInst1 (TRACE_DEBUG, "Cscall/Protocolhandler: frame received from encoder, but drop it (mNewCodecReqReceived=%d)", mNewCodecReqReceived);
	encoder_buffer->nFilledLen = 0;
	mPorts[ENCODER_PORT].dequeueAndReturnBuffer();
	setWaitOn(ENCODER_PORT);
	return;
  }

  memset(modem_buffer->pBuffer, 0, modem_buffer->nAllocLen);
  process_ok = mProtocol->process_speech_data_uplink(encoder_buffer, modem_buffer, &mCscallCodecConfig,
													 &mCscallCodecStatus,&config_updated);
  modem_buffer->nTimeStamp = encoder_buffer->nTimeStamp;
  if(!process_ok) return;

  if (config_updated)
  {
    applyCodecConfig();
  }

  if (mUplinkTimingInfo)
  {
	// update timing information is asked
	OMX_S32 time=(OMX_S32)(getTime() - mNextDeliveryTime - CSCALL_20MS_FRAME_VALUE_IN_US);
	if (mUplinkTimingInfo->nMaxTime < time)
	{
	  mUplinkTimingInfo->nMaxTime = time;
	}
	if ((mUplinkTimingInfo->nMinTime > time) || (mUplinkTimingInfo->nMinTime == (OMX_S32)0x80000000))
	{
	  mUplinkTimingInfo->nMinTime = time;
	}
	if (mUplinkTimingInfo->nAverageTime == (OMX_S32)0x80000000)
	{
	  mUplinkTimingInfo->nAverageTime = time;
	}
	else
	{
	  mUplinkTimingInfo->nAverageTime = (OMX_S32)((1-mAlpha)*(mUplinkTimingInfo->nAverageTime)) + (OMX_S32)(mAlpha * time);
	}
  }

  // send UL encoded frame
  mNbULFrame++;

  OstTraceFiltInst2 (TRACE_DEBUG, "Cscall/Protocolhandler: UL-frame sent latency control (protocolhandler, input) (timestamp = 0x%x 0x%x us)", (encoder_buffer->nTimeStamp)>>32,(encoder_buffer->nTimeStamp)&0xffffffffu);
  OstTraceFiltInst3 (TRACE_ALWAYS, "Cscall/Protocolhandler: Send encoded frame from encoder to modem (mNbULFrame=%d) (buf=0x%x) (encoder_buffer->nFilledLen=%d)", mNbULFrame, (t_uint32)encoder_buffer->pBuffer, encoder_buffer->nFilledLen);

  // return and trace bufers
  MMPROBE_TRACE_DATA(MM_PROBE_TX_SPE_OUTPUT,encoder_buffer);
  mPorts[ENCODER_PORT].dequeueAndReturnBuffer();

  traceULMsg(modem_buffer);

  mPorts[UPLINK_PORT].dequeueAndReturnBuffer();
  setWaitOn(ENCODER_PORT);
}

/**
 * Select the loop mode if needed
 */
void METH(setModemLoopbackMode)(Cscall_ModemLoop_Config_t * loop_config)
{
  bool process_ok;
  bool traces = (getTraceInfoPtr() != 0);

  if (traces) {
    OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/Protocolhandler: Received setModemLoopbackMode");
  }

  if(!mProtocol)
  {
	proxy.eventHandler(OMX_EventError, (t_uint32)OMX_ErrorInsufficientResources, 0);
	ARMNMF_DBC_ASSERT(0);
	return;
  }
  else
  {
	/* Send a Loop request to the Modem */
    mUseLoopBackMode = true;
	if(mPorts[UPLINK_PORT].queuedBufferCount() < 1)
	{
	  if (traces) {
        OstTraceFiltInst0 (TRACE_WARNING, "Cscall/Protocolhandler: No buffer available on the uplink port. Not possible to send a loop");
      }
	  //ARMNMF_DBC_ASSERT(0);
	}
	else
	{
	  OMX_BUFFERHEADERTYPE *modem_buffer   = mPorts[UPLINK_PORT].dequeueBuffer();
	  if (traces) {
        OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/Protocolhandler: Call mPorts[UPLINK_PORT].dequeueBuffer completed");
      }
	  process_ok = mProtocol->set_modem_loop_mode(modem_buffer,loop_config);
	  if(!process_ok) return;
	  if (traces) {
        traceULMsg(modem_buffer);
      }
	  returnBufferAsync(UPLINK_PORT,modem_buffer);
	  if (traces) {
        OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/Protocolhandler: Call returnBufferAsync() linked to the uplink port completed");
      }
	}
  }
}


/**
 * Schedule process only if buffer is received on "waiting" port
 */
void cscall_nmf_host_protocolhandler::receiveBuffer(portname port, OMX_BUFFERHEADERTYPE_p buffer)
{
    mPorts[port].queueBuffer((OMX_BUFFERHEADERTYPE *)buffer);

    if((port == DOWNLINK_PORT) && mPorts[DOWNLINK_PORT].queuedBufferCount() > 1){
        // It's natural to have this trace (not really a warning) when the CSALL gets the transition from executing to idle
        OstTraceFiltInst2 (TRACE_WARNING,"Cscall/Protocolhandler: receiveBuffer() call scheduleProcessEvent() (nb_buff queued on DL_Port=%d) (mWaitStatus=0x%x)",mPorts[DOWNLINK_PORT].queuedBufferCount(),mWaitStatus);
    }

    if(isWaitingOn(port))
    {
        scheduleProcessEvent();
    }
}


void cscall_nmf_host_protocolhandler::timingReport(signed long long timestamp, t_uint32 time)
{
  OstTraceFiltInst2(TRACE_ALWAYS, "Cscall/Protocolhandler: timingReport : (timestamp=%d) (timing=%d)",(t_sint32)timestamp, time);

  if (time == 0) {
	// stop uplink processing and reset all variable used for modem sync to default value
	ul_timer.stopTimer();
    if (!mUseLoopBackMode) {
        dl_timer.stopTimer();
    }
	mTimerStarted = false;
	mLastReceivedDownlinkMessage = 0;
    mLastSentDownlinkMessage     = 0;
	mLastReceivedTimingReport = 0;
	mTimingReportTime = 0;
	mModemAdjustment = 1000;
	mPatternDetectionNeeded = 0;
	OstTraceFiltInst0(TRACE_ALWAYS, "Cscall/Protocolhandler: stop ul timer and reset timing variables");
  }
  else
  {
    if (time > 1000000) {
	  OstTraceFiltInst0(TRACE_WARNING, "Cscall/Protocolhandler: Modem asked for next UL frame in more than one second! reset to 1 sec");
      time = 1000000;
    }
	mTimingReportTime         = time;
	mLastReceivedTimingReport = timestamp;
	mPatternDetectionNeeded     = 1;

	//Always set next delivery time!
    mNextDeliveryTime = timestamp + time;

	if((mCodecInUse != CODEC_NONE))
	{
      unsigned long long CanStartOn = getTime() + mCurrentMargin;

	  OstTraceFiltInst4(TRACE_ALWAYS, "Cscall/Protocolhandler: timingReport before adjustment :: (CanStartOn=0x%x%x) (mNextDeliveryTime=0x%x%x)",(t_uint32)(CanStartOn >> 32), (t_uint32)CanStartOn, (t_uint32)(mNextDeliveryTime>>32), (t_uint32)mNextDeliveryTime);
      if(mNextDeliveryTime < CanStartOn)
	  {
		unsigned long long delta = CanStartOn - mNextDeliveryTime;
		delta = delta % CSCALL_20MS_FRAME_VALUE_IN_US;
		mNextDeliveryTime = CanStartOn + (CSCALL_20MS_FRAME_VALUE_IN_US - delta);
      }
      OstTraceFiltInst4(TRACE_ALWAYS, "Cscall/Protocolhandler: timingReport after adjustment :(CanStartOn=0x%x%x) (mNextDeliveryTime=0x%x%x)",(t_uint32)(CanStartOn >> 32), (t_uint32)CanStartOn, (t_uint32)(mNextDeliveryTime>>32), (t_uint32)mNextDeliveryTime);

      if (mTimerStarted) {
          OstTraceFiltInst0(TRACE_ALWAYS, "Cscall/Protocolhandler: timingReport: stop timer");
          ul_timer.stopTimer();
      }

      int nextUlFrame = (int)(mNextDeliveryTime-getTime())-mCurrentMargin;

      if (nextUlFrame < 100) {
          ul_alarm.signal();
      } else {
          ul_timer.startTimer(nextUlFrame, 0); //start timer at call start
      }
      OstTraceFiltInst1(TRACE_ALWAYS, "Cscall/Protocolhandler: timingReport: UL timer started for %d us!", nextUlFrame);
      mTimerStarted = true;
	}
  }
}


/**
 * A dl frame has been received. Transmit it to decoder.
 * If 4/4/5 pattern is not yet synchronized, detect it and call calculateModemAdjustment to compute time difference between dl and ul
 * Also try to update ul timing when possible:
 *     - consecutive dl frames are received in correct framings (include 4/4/5 pattern), otherwise restart 4/4/5 pattern detection
 *     - Updates are "smooth" ie inside [-CSCALL_ACCEPTABLE_UL_JITTER:CSCALL_ACCEPTABLE_UL_JITTER].
 *       If too many dl frames did not update ul, it seems that we had a problem in adjustment computation. So restart pattern detection and adjustment computation.
 */
void cscall_nmf_host_protocolhandler::downlinkMessageReceived(signed long long timestamp)
{
    t_sint32 diff;

    if(mTimingReportTime==0)
    {
        OstTraceFiltInst0(TRACE_WARNING, "Cscall/Protocolhandler: downlink message received but don't start uplink timer (timing Req not received yet)!");
        return;
    }

    if(mLastReceivedDownlinkMessage == 0)
    {
        mLastReceivedDownlinkMessage = timestamp;
        return; // skip first one
    }
    else
    {
        diff = timestamp - mLastReceivedDownlinkMessage;
        mLastReceivedDownlinkMessage = timestamp;
    }

    /* start dl timer to be sure we will send a frame even if modem doesn't */
    OstTraceFiltInst0(TRACE_DEBUG, "Cscall/Protocolhandler: stop DL security timer");
    if (!mUseLoopBackMode) {
        dl_timer.stopTimer();
    }

    if (!mPatternDetectionNeeded) {
        // Pattern should be synchronized. Check that there was not any problem in dl like frame drop or too high level of jitter.
        // First calculate absolute difference with expected time of dl frame arrival
        int jitter;
        if (mGSM445 > -1) {
            OstTraceFiltInst2(TRACE_DEBUG, "Cscall/Protocolhandler: GSM network (diff=%d) (mGSM445=%d)", (int)diff,(int)mGSM445);
            // diff should be 23ms if mGSM445 = 0, 18.5ms otherwise
            jitter = diff - (23000-(mGSM445?4500:0));
        } else {
            OstTraceFiltInst1(TRACE_DEBUG, "Cscall/Protocolhandler: 3G network (diff=%d)", (int)diff);
            jitter = diff - CSCALL_20MS_FRAME_VALUE_IN_US;
        }

        if (jitter < 0) {
            jitter = -jitter;
        }

        if (jitter > CSCALL_ACCEPTABLE_DL_JITTER) {
            //Diff time from previous frame is not correct, we won't update mNextDeliveryTime but will restart pattern detection (maybe modem did not send all frames)
            OstTraceFiltInst2(TRACE_ALWAYS, "Cscall/Protocolhandler: Downlink periodicity problem detected! Restart framing detection (jitter=%d) Restart timer (timer=%d)", jitter, (mGSM445 > -1)?23000:20000+CSCALL_DL_SECURITY_MARGIN);
            mPatternDetectionNeeded = 1;
            mGSM445                 = (mGSM445 > -1)?0:-1;
            if (!mUseLoopBackMode) {
                dl_timer.startTimer((mGSM445 > -1)?23000:20000+CSCALL_DL_SECURITY_MARGIN,CSCALL_20MS_FRAME_VALUE_IN_US);
            }
            return;
        }
    } else {
        // Try to calculate expected difference between dl receive time and ul expected delivery time
        // If network is GSM we should only recalculate on 23 ms TDMA frame and not 18.5.
        // But we don't want to calculate adjustment if jitter seems too high

        // First calculate difference with expected time of dl frame arrival (compared to 23 ms for GSM network as we want to set adjustment only when 23 ms frame received)
        int jitter = diff;
        if (mGSM445 > -1) {
            jitter -= 23000;
        } else {
            jitter -= CSCALL_20MS_FRAME_VALUE_IN_US;
        }
        if (jitter < 0) {
            jitter = -jitter;
        }
        if (jitter < CSCALL_ACCEPTABLE_DL_JITTER) {
            // Diff is correct: jitter seems correct and we received a 23ms TDMA frame
            // Calculate adjustment
            mPatternDetectionNeeded = !(calculateModemAdjustment(mLastReceivedDownlinkMessage));
        }
    }

    if (!mPatternDetectionNeeded) {
        // Pattern seems ok, no problem detected
        t_sint32 adjustment = mModemAdjustment;
        unsigned long long calculatedDeliveryTime = timestamp;
        int diffTime = 0;

        // compensate GSM 4-4-5 pattern if activated
        if (mGSM445 > -1)
        {
            // Update adjustment in case of GSM network
            //    - add 0 if we received 23ms frame
            //    - add 1.5ms if we received first 18.5ms frame
            //    - add 3ms if we received second 18.5ms frame
            adjustment += mGSM445 * 1500;
            mGSM445 = (mGSM445 + 1) % 3;
            OstTraceFiltInst2(TRACE_DEBUG, "Cscall/Protocolhandler: start DL security timer (timer=%d) (mGSM445=%d)",23000+CSCALL_DL_SECURITY_MARGIN-(mGSM445?4500:0),mGSM445);
            if (!mUseLoopBackMode) {
                dl_timer.startTimer(23000+CSCALL_DL_SECURITY_MARGIN-(mGSM445?4500:0),CSCALL_20MS_FRAME_VALUE_IN_US);
            }
        } else {
            OstTraceFiltInst1(TRACE_DEBUG, "Cscall/Protocolhandler: start DL security timer (timer=%d)",20000+CSCALL_DL_SECURITY_MARGIN);
            if (!mUseLoopBackMode) {
                dl_timer.startTimer(20000+CSCALL_DL_SECURITY_MARGIN,CSCALL_20MS_FRAME_VALUE_IN_US);
            }
        }
        calculatedDeliveryTime += adjustment;

        // Check if the new delivery time we calculated thanks to dl is acceptable or not
        // it means is inside [mNextDeliveryTime-CSCALL_ACCEPTABLE_UL_JITTER:mNextDeliveryTime+CSCALL_ACCEPTABLE_UL_JITTER]

        // both calculatedDeliveryTime and mNextDeliveryTime are unsigned long long, can not do signed diff
        if (calculatedDeliveryTime > mNextDeliveryTime) {
            diffTime = (int)(calculatedDeliveryTime - mNextDeliveryTime);
        } else {
            diffTime = (int)(mNextDeliveryTime - calculatedDeliveryTime);
        }
        if (diffTime < CSCALL_ACCEPTABLE_UL_JITTER) {
            // Jitter seems correct, update mNextDeliveryTime with dl frame time + adjustement
            mNbDlFrameDidNotUpdateUl = 0;
            mNextDeliveryTime = calculatedDeliveryTime;
        } else {
            //Jitter not correct
            mNbDlFrameDidNotUpdateUl ++;
            OstTraceFiltInst2(TRACE_ALWAYS, "Cscall/Protocolhandler: Don't update mNextDeliveryTime because calculated delivery time not in valid uplink acceptable range (diffTime = %d) (mNbDlFrameDidNotUpdateUl=%d)", diffTime, mNbDlFrameDidNotUpdateUl);

        }
        if (mNbDlFrameDidNotUpdateUl > 25) {
            // If we detect the pattern during some irregular dl, we might never update ul delivery time.
            // Need to start again pattern detection to avoid clock drift and avoid timing_req
            // 25 means half a second
            OstTraceFiltInst0(TRACE_ALWAYS, "Cscall/Protocolhandler: It appears that we do not update ul timing anymore! Restart framing detection");

            mNbDlFrameDidNotUpdateUl = 0;
            mPatternDetectionNeeded = 1;
        }

        OstTraceFiltInst3(TRACE_DEBUG, "Cscall/Protocolhandler: downlinkMessageReceived() (adjustment=%d)  (msg_diff=%d)  (GSM445=%d)", adjustment, diff, mGSM445);

    } else {
       OstTraceFiltInst2(TRACE_DEBUG, "Cscall/Protocolhandler: start DL security timer (timer=%d) (mGSM445=%d)",((mGSM445 > -1)?23000:20000)+CSCALL_DL_SECURITY_MARGIN, mGSM445);
       if (!mUseLoopBackMode) {
          dl_timer.startTimer(((mGSM445 > -1)?23000:20000)+CSCALL_DL_SECURITY_MARGIN,CSCALL_20MS_FRAME_VALUE_IN_US);
       }
   }
}

/**
  * Calculate the difference in time between when the next uplink frame should be sent and when we receive dl frame.
  * This difference is calculated:
  *     - at the beggining of the call, when the pattern has been correctly detected in case of GSM network
  *     - When we detected some problem in dl (missing frame for example)
  * This adjustment is used during the whole call: when we receive a frame, the next ul frame should be sent after "adjustment" ms
  * The adjustment is updated if issue detected or new timing_req from modem
  * */
bool cscall_nmf_host_protocolhandler::calculateModemAdjustment(unsigned long long now)
{
  t_sint32 adjustment = (t_sint32)(mNextDeliveryTime - now);
  if (adjustment < 0) {
      // In that case, we received a downlink frame and timer should have already expired but we did not yet receive ul frame ==> UL is in late !!!
      OstTraceFiltInst1(TRACE_DEBUG, "Cscall/Protocolhandler: calculateModemAdjustment(): ul is in late (adjustment = %d)!!!", adjustment);
      return false;
  } else if ((t_uint32)adjustment > CSCALL_20MS_FRAME_VALUE_IN_US + mCurrentMargin) {
      OstTraceFiltInst1(TRACE_DEBUG, "Cscall/Protocolhandler: calculateModemAdjustment(): adjustment too high (adjustment = %d)!!!", adjustment);
	  return false;
  }

  mModemAdjustment = adjustment;
#ifdef ISI_TD_PROTOCOL
  mGSM445 = -1;
#else
  mGSM445 = (mNetwork == NETWORK_GSM)?0:-1;
#endif

  OstTraceFiltInst3(TRACE_DEBUG, "Cscall/Protocolhandler: calculateModemAdjustment(): (adjustment=%d) (mNextDeliveryTime high=%d low=%d)",  mModemAdjustment, (int)((mNextDeliveryTime>>32) & 0xFFFFFFFF), (int)(mNextDeliveryTime & 0xFFFFFFFF));
  OstTraceFiltInst3(TRACE_DEBUG, "Cscall/Protocolhandler: calculateModemAdjustment(): (now high=%d low=%d)",  mModemAdjustment, (int)((now>>32) & 0xFFFFFFFF), (int)(now & 0xFFFFFFFF));
  return true;
}

void cscall_nmf_host_protocolhandler::chooseCurrentMargin(void)
{
  if(mCodecInUse == CODEC_AMR_WB)
  {
	mCurrentMargin = mMargins.wbMargin;
  }
  else if((mCodecInUse == CODEC_PCM8) ||
		  (mCodecInUse == CODEC_PCM16))
  {
	mCurrentMargin = 0;
  }
  else
  {
	mCurrentMargin = mMargins.nbMargin;
  }
}


