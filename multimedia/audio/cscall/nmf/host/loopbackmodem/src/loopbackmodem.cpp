/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   loopbackmodem.cpp
 * \brief  Fake modem driver, used in loop mode. Instantiated in place of the audiomodem driver.
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "cscall/nmf/host/loopbackmodem.nmf"
#include <string.h>
#include "cscall_time.h"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cscall_nmf_host_loopbackmodem_src_loopbackmodemTraces.h"
#endif

#define MREP_ACTIVATED     (1)
#define MREP_TIMESTAMP_END (0xFFFFFFFF)

t_nmf_error METH(construct)()
{
  toIdle          = 0 ;
  mLoopBack       = 0;
  mLoopBackStatus = WAIT_CODEC_REQ;
  nMRePActivation = 0;
  memset(&mCodecConfig, 0, sizeof(CsCallCodecReq_t));
  mGSM445pattern  = 0;
  return NMF_OK ;
}


void METH(destroy)()
{
  OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/loopbackmodem: loopbackmodem destroy");
  if(mLoopBack != 0) delete mLoopBack;
}

///////////////////////////////
// Component functions
//////////////////////////////
void cscall_nmf_host_loopbackmodem::process()
{

  if (nMRePActivation != MREP_ACTIVATED)
  {
	OMX_BUFFERHEADERTYPE *to_requeue[NB_UL_MODEM_BUFFER];
	t_uint32              nb_buffer_to_requeue = 0;
	
	// Checks if buffers available on uplink port
	while (mPorts[UPLINK_PORT].queuedBufferCount())
	{
	  OMX_BUFFERHEADERTYPE * bufuplink = mPorts[UPLINK_PORT].getBuffer(0);
	  
	  ARMNMF_DBC_ASSERT(mLoopBack);
	  if(! mLoopBack) return;
	  CsCallMessage_t msg_type = mLoopBack->getMsgType(bufuplink);
	  
	  if(msg_type == MSG_SPEECH_DATA_UPLINK)
	  {
		// speech_data uplink messages will be recopied to downlink in signal function.
		to_requeue[nb_buffer_to_requeue] = mPorts[UPLINK_PORT].dequeueBuffer();
		nb_buffer_to_requeue++;
		if (mLoopBackStatus == WAIT_UL_FRAME1)
		{
		  mLoopBackStatus = WAIT_UL_FRAME2;
		}
		else if (mLoopBackStatus == WAIT_UL_FRAME2)
		{
		  mLoopBackStatus = ON_GOING;
		}
	  }
	  else
	  {
		if((msg_type == MSG_CODING_FORMAT_RESPONSE) &&
		   (mCodecConfig.speech_codec != CODEC_NONE))
		{
		  mLoopBackStatus = WAIT_UL_FRAME1;
		  // clear previous speech_data_uplink messages (if any) as they do not have the right codec
		  while(nb_buffer_to_requeue)
		  {
			nb_buffer_to_requeue --;
			mPorts[UPLINK_PORT].returnBuffer(to_requeue[nb_buffer_to_requeue]);
		  }
		  
		  // start timer
		  if (mCodecConfig.network == NETWORK_GSM)
		  {
		    OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/loopbackmodem: DL Process date initialization, GSM mode");
		    setNextDLProcessDate(20000,0); //abitrary value
		  }
		  else // WCDMA
	      {
	        OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/loopbackmodem: DL Process date initialization, WCDMA periodic mode");
	        setNextDLProcessDate(20000,20000); //20ms period for WCDMA. Will not be modified anymore
	      }
		}
		else
		{
		  OstTraceFiltInst1 (TRACE_ALWAYS, "Cscall/loopbackmodem: received unhandle messsage of type %d", msg_type);
		}
		mPorts[UPLINK_PORT].dequeueAndReturnBuffer();
	  }
	}
	
	// requeue uplink_speech_data message, will be processed in signal
	while(nb_buffer_to_requeue)
	{
	  nb_buffer_to_requeue --;
	  mPorts[UPLINK_PORT].requeueBuffer(to_requeue[nb_buffer_to_requeue]);
	}
	
	if((mLoopBackStatus == WAIT_DL_BUFFER) ||
	   (mLoopBackStatus == WAIT_UL_FRAME_NOW))
	{
	  t_uint32 timing = 0;
	  mLoopBackStatus = ON_GOING;
	  signalProcess(true, &timing);
	  if (mCodecConfig.network == NETWORK_GSM)
	  {
	    // in case of WCDMA network, the timer has a 20ms period, no need to reconfigure it each frame.
	    setNextDLProcessDate(timing,0);
	  }
	}
  }
  else  // Modem replay mode
  {
	while (mPorts[UPLINK_PORT].queuedBufferCount())
	{
	  OMX_BUFFERHEADERTYPE * bufuplink = mPorts[UPLINK_PORT].getBuffer(0);
	  
	  OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/loopbackmodem: Process: Received UL Data from protocolHandler");
	  // Copies buffer in memory for future file I/O extraction.
	  if (pMRePBufferOutput != NULL)
	  {
		t_uint16 msgLength = bufuplink->nFilledLen;
		memcpy( (void *)pMRePBufferOutput, (const void *)bufuplink->pBuffer,msgLength); // memcpy length in bytes
		pMRePBufferOutput += msgLength;
		// Circular copy management: -640 is the worst case for pcm @16kHz in bytes
		if ((t_uint32)(pMRePBufferOutput - pMRePBufferOutputInitial) >= (nMRePOutputSize-640))
		{
		  pMRePBufferOutput = pMRePBufferOutputInitial;
		}
	  }
	  // And dequeue buffer for protocol handler...
	  mPorts[UPLINK_PORT].dequeueAndReturnBuffer();
	}
  }
  
}/*process*/


void METH(reset)() {}
void METH(disablePortIndication)(t_uint32 portIdx) {}
void METH(enablePortIndication)(t_uint32 portIdx)  {}
void METH(flushPortIndication)(t_uint32 portIdx)   {}



///////////////////////////////
// NMF Interfaces
//////////////////////////////
//////////////////////////////
// fsminit
/////////////////////////////
void METH(fsmInit)(fsmInit_t initFsm)
{
  OMX_BUFFERHEADERTYPE * mBufIn_list[NB_UL_MODEM_BUFFER] ;
  OMX_BUFFERHEADERTYPE * mBufOut_list[NB_DL_MODEM_BUFFER];

  for (int i=0; i< NB_UL_MODEM_BUFFER; i++) {
	mBufIn_list[i] = &mBufIn[i] ;
	mBufIn[i].pInputPortPrivate = (OMX_PTR)i ;
  }
  for (int i=0; i< NB_DL_MODEM_BUFFER; i++) {
	mBufOut_list[i] = &mBufOut[i] ;
	mBufOut[i].pOutputPortPrivate = (OMX_PTR)i ;
  }

  // trace init (mandatory before port init)
  if (initFsm.traceInfoAddr){
	setTraceInfo(initFsm.traceInfoAddr, initFsm.id1);
  }

  OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/loopbackmodem: fsminit()");
  
  mPorts[UPLINK_PORT].init(InputPort, false,false,0,mBufIn_list,NB_UL_MODEM_BUFFER, &inputport, UPLINK_PORT, false,true, this) ;
  mPorts[DOWNLINK_PORT].init(OutputPort,false,false,0,mBufOut_list,NB_DL_MODEM_BUFFER, &outputport, DOWNLINK_PORT, false,true, this);
  init(2, mPorts, &proxy, &me, false) ;
}


//////////////////////////////
// sendcommand
//////////////////////////////
void METH(sendCommand)(OMX_COMMANDTYPE cmd, t_uword param)
{
  /* Detect transitions to idle */
  if ((cmd == OMX_CommandStateSet) && (param == OMX_StateIdle)) {
	toIdle = 1 ;
  }
  else if (toIdle) NMF_PANIC("loopbackmodem :: You must destroy this component after a transition to IDLE\n");
  Component::sendCommand(cmd, param) ;
}


//////////////////////////////
// timer.api.alarm
//////////////////////////////
void METH(signal)()
{
  t_uint32 timing = 0;
  
  if (nMRePActivation == MREP_ACTIVATED)
  {
	// It's time to send a new frame on DL
	OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/loopbackmodem: MReP signal: time to send DL frames");
	do {
	  timing = MRePProcess();
	} while (timing == 0);
	
	setNextDLProcessDate(timing,0);
  }
  else
  {
	signalProcess(false, &timing);
	
	/* reconfigure the timer only in 2G mode. In 3G, it is in 20 ms periodic mode */
	if (mCodecConfig.network == NETWORK_GSM)
	{
	  setNextDLProcessDate(timing,0);
	}
  }
}


///////////////////////////////////////////
// cscall.nmf.host.loopbackmodem.configure
///////////////////////////////////////////
void METH(SetProtocol)(CsCallProtocol_t protocol)
{
  createLoopBack(protocol);
}

void METH(SpeechCodecReq)(CsCallCodecReq_t config)
{
  mCodecConfig   = config;
  mGSM445pattern = 0;
  
  if((mCodecConfig.speech_codec == CODEC_NONE) &&
	 (mLoopBackStatus != WAIT_CODEC_REQ))
  {
	setNextDLProcessDate(0,0);

    while (mPorts[UPLINK_PORT].queuedBufferCount()) {
      OMX_BUFFERHEADERTYPE * buffer = mPorts[UPLINK_PORT].dequeueBuffer();
      returnBufferAsync(UPLINK_PORT,buffer);
    }
	mLoopBackStatus = WAIT_CODEC_REQ;
  }
  else
  {
	mLoopBackStatus = WAIT_CODEC_RESP;
  }
  
  //preapare the output buffer MSG_CODING_FORMAT_REQUEST
  //check if buffer is available on out port
  if(mPorts[DOWNLINK_PORT].queuedBufferCount())
  {
	ARMNMF_DBC_ASSERT(mLoopBack);
	if(! mLoopBack) return;

	OMX_BUFFERHEADERTYPE * buffer = mPorts[DOWNLINK_PORT].dequeueBuffer();
	mLoopBack->generate_coding_format_request(&config, buffer);
	buffer->nTimeStamp = getTime();
	returnBufferAsync(DOWNLINK_PORT,buffer);
  }
}

void METH(SpeechTimingRequest)(t_uint32 timing, t_uint32 cause)
{
  //preapare the output buffer MSG_TIMING_REPORT
  //check if buffer is available on out port
  if(mPorts[DOWNLINK_PORT].queuedBufferCount())
  {
	ARMNMF_DBC_ASSERT(mLoopBack);
	if(! mLoopBack) return;

	OMX_BUFFERHEADERTYPE * buffer = mPorts[DOWNLINK_PORT].dequeueBuffer();
	mLoopBack->generate_timing_report(timing, cause, buffer);
	buffer->nTimeStamp = getTime();
	returnBufferAsync(DOWNLINK_PORT, buffer);
  }
}

void METH(VoiceCallStatus)(t_uint32 connected)
{
  //preapare the output buffer MSG_VOICE_CALL_STATUS
  //check if buffer is available on out port
  if(mPorts[DOWNLINK_PORT].queuedBufferCount())
  {
	ARMNMF_DBC_ASSERT(mLoopBack);
	if(! mLoopBack) return;
	
	OMX_BUFFERHEADERTYPE * buffer = mPorts[DOWNLINK_PORT].dequeueBuffer();
	mLoopBack->generate_voice_call_status((bool)connected, buffer);
	returnBufferAsync(DOWNLINK_PORT, buffer);
  }
}


void METH(MRePInit)(t_uint32 timing, t_uint16 *pBuffer, t_uint32 outBufferSize, t_uint16 *pBufferOutput)
{
  OstTraceFiltInst2 (TRACE_ALWAYS, "Cscall/loopbackmodem: MRePInit() called (timing=%d) (pDLData = %d)",timing,(t_uint32) pBuffer);
  *pBuffer++;   // First timing value is 0 in the input file...just bypass it
  *pBuffer++;   // twice because timing on 32 bits in fact...
  pMRePBuffer = pBuffer;
  nMRePActivation = MREP_ACTIVATED;
  nMRePTiming = timing;
  nMRePOutputSize = outBufferSize;
  pMRePBufferOutput = pBufferOutput;
  pMRePBufferOutputInitial = pBufferOutput;
  setNextDLProcessDate(timing, 0);
}


///////////////////////////////
// private functions
///////////////////////////////
// Remark : OST traces OK for MReP since always in executing state when MReP functions called
void cscall_nmf_host_loopbackmodem::setNextDLProcessDate(t_uint32 timing, t_uint32 period)
{
  if (timing == 0) {
	timerMReP.stopTimer();
	OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/loopbackmodem: Stop MReP Timer cause NULL timing");
	return;
  }
  
  // Else schedule through alarm or quit in case of MREP_TIMESTAMP_END tag
  if (timing != MREP_TIMESTAMP_END) {
	timerMReP.stopTimer();
	timerMReP.startTimer(timing,period);
	OstTraceFiltInst1 (TRACE_ALWAYS, "Cscall/loopbackmodem: New DL Process date (time=%dus)", timing);
  }
  else
  {
	timerMReP.stopTimer();
	OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/loopbackmodem: Stop MReP Timer");
  }
}


t_uint32 cscall_nmf_host_loopbackmodem::MRePProcess()
{
  t_uint32 timing = MREP_TIMESTAMP_END;
  t_uint16 msgLength;
  t_uint32 *pMRePBuffer32;

  OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/loopbackmodem: MRePProcess: time to send frame to DL protocol handler");

  // First extracts message length
  msgLength = *pMRePBuffer++;

  // Tranfers modem message to DL protocol handler
  // Checks if buffers available on downlink port
  if (mPorts[DOWNLINK_PORT].queuedBufferCount() == 0)
  {
	// Sends error message in case no DL buffer available
	OstTraceFiltInst0 (TRACE_ERROR, "Cscall/loopbackmodem: ERROR!! DL Buffer not available");
	return timing;
  }
  OMX_BUFFERHEADERTYPE * bufdownlink = mPorts[DOWNLINK_PORT].getBuffer(0);
  // Copies the encoder frame to the decoder frame,
  memcpy( (void *)bufdownlink->pBuffer, (const void *)pMRePBuffer,msgLength*2); // memcpy length in bytes
  pMRePBuffer += msgLength;
  bufdownlink->nTimeStamp = getTime();
  OMX_BUFFERHEADERTYPE *bufOut = mPorts[DOWNLINK_PORT].dequeueBuffer();
  bufOut->nFilledLen = msgLength*2;
  bufOut->nOffset    = 0;
  bufOut->nFlags     = 0;
  returnBufferAsync(DOWNLINK_PORT, bufOut);

  // Then gets timing for next DL process scheduling
  pMRePBuffer32 = (t_uint32 *)pMRePBuffer;
  timing = *pMRePBuffer32++;
  pMRePBuffer = (t_uint16 *)pMRePBuffer32;

  return timing;
}

void cscall_nmf_host_loopbackmodem::signalProcess(bool from_process, t_uint32 *timing)
{
  if (mPorts[DOWNLINK_PORT].queuedBufferCount() == 0)
  {
	mLoopBackStatus = WAIT_DL_BUFFER;
	OstTraceFiltInst0 (TRACE_DEBUG, "Cscall/loopbackmodem: signalProcess() no downlink buffer");
	return;
  }
	
  if(mLoopBackStatus == ON_GOING)
  {
	mLoopBackStatus = WAIT_UL_FRAME_NOW;
	
	OMX_BUFFERHEADERTYPE *to_requeue[NB_UL_MODEM_BUFFER];
	t_uint32              nb_buffer_to_requeue = 0;
	
	while (mPorts[UPLINK_PORT].queuedBufferCount())
	{
	  OMX_BUFFERHEADERTYPE * bufuplink = mPorts[UPLINK_PORT].dequeueBuffer();
	  
	  ARMNMF_DBC_ASSERT(mLoopBack);
	  if(! mLoopBack) return;
	  CsCallMessage_t msg_type = mLoopBack->getMsgType(bufuplink);
	  
	  if(msg_type == MSG_SPEECH_DATA_UPLINK)
	  {
		OMX_BUFFERHEADERTYPE * bufdownlink = mPorts[DOWNLINK_PORT].dequeueBuffer();
		mLoopBack->copy_speech_uplink_to_downlink(bufuplink, bufdownlink);
		bufdownlink->nTimeStamp = getTime();
		
		OstTraceFiltInst0 (TRACE_DEBUG, "Cscall/loopbackmodem: signalProcess() send one downlink buffer");
		if(from_process)
		{
		  mPorts[DOWNLINK_PORT].returnBuffer(bufdownlink);
		  mPorts[UPLINK_PORT].returnBuffer(bufuplink);
		}
		else
		{
		  returnBufferAsync(UPLINK_PORT, bufuplink);
		  returnBufferAsync(DOWNLINK_PORT, bufdownlink);
		}
		mLoopBackStatus = ON_GOING;
		
		// update timing for next frame
		if(mCodecConfig.network == NETWORK_GSM)
		{
		  if(mGSM445pattern) *timing = 18500;
		  else               *timing = 23000;
		  mGSM445pattern = (mGSM445pattern + 1) % 3;
		}
		else
		{
		  *timing = 20000; // downlink frame every 20ms in 3G
		}
		
		break;
	  }
	  else
	  {
		to_requeue[nb_buffer_to_requeue] = bufuplink;
		nb_buffer_to_requeue++;
	  }
	}
	
	// requeue non-uplink_speech_data message, will be processed in process
	while(nb_buffer_to_requeue)
	{
	  nb_buffer_to_requeue --;
	  mPorts[UPLINK_PORT].requeueBuffer(to_requeue[nb_buffer_to_requeue]);
	}
	
	if(mLoopBackStatus == WAIT_UL_FRAME_NOW)
	{
	  OstTraceFiltInst0 (TRACE_DEBUG, "Cscall/loopbackmodem: signalProcess() no uplink buffer");
	}
  }
  else if(mLoopBackStatus != WAIT_CODEC_REQ)
  {
	OMX_BUFFERHEADERTYPE * bufdownlink = mPorts[DOWNLINK_PORT].dequeueBuffer();
	mLoopBack->generate_fake_downlink_frame(&mCodecConfig, bufdownlink);
	bufdownlink->nTimeStamp = getTime();
	OstTraceFiltInst0 (TRACE_DEBUG, "Cscall/loopbackmodem: signalProcess() send one fake downlink buffer");
	if(from_process)
	{
	  mPorts[DOWNLINK_PORT].returnBuffer(bufdownlink);
	}
	else
	{
	  returnBufferAsync(DOWNLINK_PORT, bufdownlink);
	}
	
	// update timing for next frame
	if(mCodecConfig.network == NETWORK_GSM)
	{
	  if(mGSM445pattern) *timing = 18500;
	  else               *timing = 23000;
	  mGSM445pattern = (mGSM445pattern + 1) % 3;
	}
	else
	{
	  *timing = 20000; // downlink frame every 20ms in 3G
	}
  }
}


void cscall_nmf_host_loopbackmodem::createLoopBack(CsCallProtocol_t protocol)
{
  if(mLoopBack==0) mLoopBack = LoopBack::getLoopBack(protocol);
  
  if(!mLoopBack)
  {
	proxy.eventHandler(OMX_EventError, (t_uint32)OMX_ErrorInsufficientResources, 0);
	ARMNMF_DBC_ASSERT(0);
  }
}
