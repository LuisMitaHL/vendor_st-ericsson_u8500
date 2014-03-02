/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \file   downlink.cpp
 * \brief  Interfaced between decoder libraries and other NMF components. Decoder wrapper take encoded data from protocol handler component, decode it with decoder chosen by network and send pcm data to Rate-DL component.
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include <cscall/nmf/host/downlink.nmf>
#include <armnmf_dbc.h>
#include <string.h>

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cscall_nmf_host_downlink_src_downlinkTraces.h"
#endif

#define LOG_TAG "CSCALL_DL"
#include "linux_utils.h"

// MMProbe framwork
#ifdef MMPROBE_ENABLED
#include "t_mm_probe.h"
#include "r_mm_probe.h"
#endif

// size of the bitstream word (configured through bs-init)
// TODO FIXME: Set to 8 bits when the libbitstream supports it!
#define BS_INIT_SIZE_IN_BITS  32
#define BS_WORD_SIZE_IN_BYTES (BS_INIT_SIZE_IN_BITS/8)
#define SIZE_SYNC_BUFF        24

// 3 consecutive fake frame generated, with ramp down to linear gain. Then mute
#define CSCALL_MAX_CONSECUTIVE_FAKE_FRAME 10
#define CSCALL_DL_START_RAMP                  3
//#define EXP_RAMPS
#ifdef EXP_RAMPS
// = LIN_GAIN_WANTED^(1/(NB_SAMPLES*(CSCALL_MAX_CONSECUTIVE_FAKE_FRAME-CSCALL_DL_START_RAMP)))
#define CSCALL_GAIN_COEF_NB 0.997118714
#define CSCALL_GAIN_COEF_WB 0.998560571
#endif


//////////////////////////////////////////////
//  Constructor
/////////////////////////////////////////////
cscall_nmf_host_downlink::cscall_nmf_host_downlink (void)
{
  memset(&mBSRead,            0, sizeof(BS_STRUCT_T));
  memset(&mCscallCodecConfig, 0, sizeof(Cscall_Codec_Config_t));

  mCodecCtxInUse                 = 0;
  mCodexCtx[0].codecInUse        = CODEC_NONE;
  mCodexCtx[0].codecIdx          = AMR_IDX;
  memset(&(mCodexCtx[0].interfaceFE),0, sizeof(CODEC_INTERFACE_T));
  mCodexCtx[0].codecInitialized  = CODEC_NOT_INITIALIZED;
  mCodexCtx[0].codecSampleFreq   = FREQ_UNKNOWN;

  mCodexCtx[1].codecInUse        = CODEC_NONE;
  mCodexCtx[1].codecIdx          = AMR_IDX;
  memset(&(mCodexCtx[1].interfaceFE),0, sizeof(CODEC_INTERFACE_T));
  mCodexCtx[1].codecInitialized  = CODEC_NOT_INITIALIZED;
  mCodexCtx[1].codecSampleFreq   = FREQ_UNKNOWN;

  mSRCInitialized   = false;
  mRealTimeOutput   = true;
  mDroppedFrameNb   = 0;

  mOutputSampleFreq = FREQ_UNKNOWN;

  mDLFakeFrameGenerated = 0;
  mConsecutiveFakeFrameSent = 0;

  mDecoderBufferHeader.nSize                = 0;
  mDecoderBufferHeader.nVersion.nVersion    = 0;
  mDecoderBufferHeader.pBuffer              = mDecoderBuffer;
  mDecoderBufferHeader.nAllocLen            = sizeof(mDecoderBuffer);
  mDecoderBufferHeader.nFilledLen           = 0;
  mDecoderBufferHeader.nOffset              = 0;
  mDecoderBufferHeader.pAppPrivate          = 0;
  mDecoderBufferHeader.pPlatformPrivate     = 0;
  mDecoderBufferHeader.pInputPortPrivate    = 0;
  mDecoderBufferHeader.pOutputPortPrivate   = 0;
  mDecoderBufferHeader.hMarkTargetComponent = 0;
  mDecoderBufferHeader.pMarkData            = 0;
  mDecoderBufferHeader.nTickCount           = 0;
  mDecoderBufferHeader.nTimeStamp           = 0;
  mDecoderBufferHeader.nFlags               = 0;
  mDecoderBufferHeader.nOutputPortIndex     = 0;
  mDecoderBufferHeader.nInputPortIndex      = 0;

  mSrcBufferHeader.nSize                = 0;
  mSrcBufferHeader.nVersion.nVersion    = 0;
  mSrcBufferHeader.pBuffer              = mSrcBuffer;
  mSrcBufferHeader.nAllocLen            = sizeof(mSrcBuffer);
  mSrcBufferHeader.nFilledLen           = 0;
  mSrcBufferHeader.nOffset              = 0;
  mSrcBufferHeader.pAppPrivate          = 0;
  mSrcBufferHeader.pPlatformPrivate     = 0;
  mSrcBufferHeader.pInputPortPrivate    = 0;
  mSrcBufferHeader.pOutputPortPrivate   = 0;
  mSrcBufferHeader.hMarkTargetComponent = 0;
  mSrcBufferHeader.pMarkData            = 0;
  mSrcBufferHeader.nTickCount           = 0;
  mSrcBufferHeader.nTimeStamp           = 0;
  mSrcBufferHeader.nFlags               = 0;
  mSrcBufferHeader.nOutputPortIndex     = 0;
  mSrcBufferHeader.nInputPortIndex      = 0;

}


//////////////////////////////////////////////
//  Component FSM
/////////////////////////////////////////////
/**
 * Standard processing when decoder wrapper is in executing state
 * */
void cscall_nmf_host_downlink::process(void)
{
  if(mPorts[INPUT_PORT].queuedBufferCount() != 0)
  {
	// process input buffer
	OMX_BUFFERHEADERTYPE *bufIn = mPorts[INPUT_PORT].getBuffer(0);

	if(mCodexCtx[mCodecCtxInUse].codecInitialized == CODEC_NOT_INITIALIZED)
	{
	  OstTraceFiltInst0 (TRACE_WARNING, "Cscall/Downlink: decoder not initialized. Trash input buffer.");
	  mPorts[INPUT_PORT].dequeueAndReturnBuffer();
	  return;
	}

	if(mRealTimeOutput || availableFreeBuffer() || (mOutputSampleFreq == FREQ_UNKNOWN))
	{
	  bool                  use_output = false;
	  OMX_BUFFERHEADERTYPE *bufOut     = getDecoderBuffer(mCodecCtxInUse, &use_output);

	  if(!availableFreeBuffer())
	  {
		mDroppedFrameNb ++;
		OstTraceFiltInst1 (TRACE_WARNING, "Cscall/Downlink: overwritting one frame (total %d).", mDroppedFrameNb);
	  }

	  decodeFrame(bufIn, bufOut, mCodecCtxInUse);

	  if((mOutputSampleFreq != FREQ_UNKNOWN) &&
              (mCodexCtx[mCodecCtxInUse].codecSampleFreq != mOutputSampleFreq))
	  {
		ARMNMF_DBC_ASSERT(mSRCInitialized);
		bufOut = getSrcBuffer(&use_output); // input buffer is always mDecoderBufferHeader
		convertSampleRate(bufOut);
	  }

#ifdef MMPROBE_ENABLED
    mmprobe_probe(MM_PROBE_RX_CS_CALL_OUTPUT, bufOut->pBuffer, bufOut->nFilledLen);
#endif
	  mPorts[INPUT_PORT].dequeueAndReturnBuffer();
	  if(use_output) {
        returnOutputBuffer();
      }
	}
  }

  if(mPorts[OUTPUT_PORT].queuedBufferCount() != 0)
  {
	// process ouput buffer
	OMX_BUFFERHEADERTYPE *bufIn  = getLatestBuffer();
	OMX_BUFFERHEADERTYPE *bufOut = mPorts[OUTPUT_PORT].getBuffer(0);

	if(bufIn)
	{
	  copyBuffer(bufIn,bufOut);
      returnOutputBuffer();
	}
  }
}

/**
 * Reset decoder wrapper and codec currently in use
 * */
void cscall_nmf_host_downlink::reset()
{
  OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/Downlink: reset() called ");

  if(mCodexCtx[0].codecInitialized) decoderClose(0);
  if(mCodexCtx[1].codecInitialized) decoderClose(1);
  srcClose();
}

/**
 * Called when a protocolhandler port is disabled
 * \param portIdx is the index of the disabled port
 * */
void METH(disablePortIndication)(t_uint32 portIdx)
{
  OstTraceFiltInst1 (TRACE_ALWAYS, "Cscall/Downlink: disablePortIndication() called (portIdx=%d)",portIdx);
  if(portIdx == OUTPUT_PORT)
  {
    mConsecutiveFakeFrameSent = CSCALL_MAX_CONSECUTIVE_FAKE_FRAME;
	mOutputSampleFreq = FREQ_UNKNOWN;
	srcClose();
	if(mPorts[INPUT_PORT].queuedBufferCount())
	{
	  // we need to flush the pending buffer because it may block EOS buffer
	  // since we cannot return buffer here (because we are already dispatching event),
	  // re-schedule process event to process it
	  // NOTE: If there is a buffer pending it means that mRealTimeOutput=0, and setting mOutputSampleFreq to FREQ_UNKNOWN
	  // disactivate mRealTimeOutput=0 behaviour (so when reschuled, this time the buffer will be processed)
	  scheduleProcessEvent();
	}
  }
}

/**
 * Called when a port is enabled (Empty fonction)
 * \param portIdx is the index of the enabled port
 * */
void METH(enablePortIndication)(t_uint32 portIdx)
{
  OstTraceFiltInst1 (TRACE_ALWAYS, "Cscall/Downlink: enablePortIndication() called (portIdx=%d)",portIdx);
  // Note srcOpen is done on setSampleFreq which is always called before an enable port
}

/**
 * Called when a port is flushed (Empty fonction)
 * \param portIdx is the index of the flushed port
 * */
void METH(flushPortIndication)(t_uint32 portIdx)
{
  OstTraceFiltInst1 (TRACE_ALWAYS, "Cscall/Downlink: flushPortIndication() called (portIdx=%d)",portIdx);
}


////////////////////////////////////////////////////////////
//          Provided Interfaces
////////////////////////////////////////////////////////////
/**
 * Initialisation of the state-machine
 * */
void METH(fsmInit)(fsmInit_t initFsm)
{
  // trace init (mandatory before port init)
  if (initFsm.traceInfoAddr){
	setTraceInfo(initFsm.traceInfoAddr, initFsm.id1);
    if (mFakeFrameGenerator) {
        mFakeFrameGenerator->setTraceInfo(initFsm.traceInfoAddr, initFsm.id1);
    }
  }
  OstTraceFiltInst1 (TRACE_ALWAYS, "Cscall/Downlink: fsmInit() called (id=%d)",initFsm.id1);


  mPorts[INPUT_PORT].init(InputPort,  false, false, 0, 0, 1, &inputport,   INPUT_PORT,
						  (initFsm.portsDisabled & (1 << (INPUT_PORT)))!=0, (initFsm.portsTunneled & (1 << (INPUT_PORT)))!=0, this);

  mPorts[OUTPUT_PORT].init(OutputPort,false ,false, 0, 0, 1, &outputport, OUTPUT_PORT,
						   (initFsm.portsDisabled & (1 << (OUTPUT_PORT)))!=0, (initFsm.portsTunneled & (1 << (OUTPUT_PORT)))!=0, this);

  init(2, mPorts, &proxy, &me, false);
}


/**
 * Set tunnel status
 * \param portIdx is the port to be tunneled or not
 * \param isTunneled is a boolean (prot tunneled or not)
 * */
void METH(setTunnelStatus)(t_sint16 portIdx, t_uint16 isTunneled) {
  OstTraceFiltInst2 (TRACE_ALWAYS, "Cscall/Downlink: setTunnelStatus() called (portIdx=%d) (isTunneled=%d)",portIdx,isTunneled);
  mPorts[portIdx].setTunnelStatus(isTunneled);
}


/**
 * Configure codec currently in use
 **/
void METH(configureCodec)(CsCallCodec_t codecType, void* pCodecInfo)
{
  Cscall_Codec_Config_t * pCscallCodecInfo = (Cscall_Codec_Config_t *) pCodecInfo;

  OstTraceFiltInst2 (TRACE_ALWAYS, "Cscall/Downlink: configureCodec() (current codec=%d) (new codec=%d)", mCodexCtx[mCodecCtxInUse].codecInUse, codecType);

  if(mPorts[INPUT_PORT].queuedBufferCount() != 0)
  {
	// coverity[check_return]
	OstTraceFiltInst1 (TRACE_WARNING, "Cscall/Downlink: configureCodec() received but (nb=%d) buffer are on INPUT_PORT", mPorts[INPUT_PORT].queuedBufferCount());
  }

  if((mCodexCtx[mCodecCtxInUse].codecInUse != codecType) )
  {
	// First stop and close current codec
	if(mCodexCtx[mCodecCtxInUse].codecInitialized != CODEC_NOT_INITIALIZED)
	{
	  mCodexCtx[mCodecCtxInUse].codecInitialized = CODEC_TERMINATING;
      mCodecCtxInUse = 1-mCodecCtxInUse;
      if (mCodexCtx[mCodecCtxInUse].codecInitialized != CODEC_NOT_INITIALIZED) {
          // Two codec changes without receiving and dl frame, close the oldest decoder
          decoderClose(mCodecCtxInUse);
          srcClose();
      }
	} else {
        mFakeFrameGenerator->setConfiguration(codecType, pCscallCodecInfo);
    }

	// Apply New Codec Config
	mCodexCtx[mCodecCtxInUse].codecInUse = codecType;

	switch(mCodexCtx[mCodecCtxInUse].codecInUse)
	{
	  case CODEC_AMR_NB:

		{
		  mCodexCtx[mCodecCtxInUse].codecSampleFreq = FREQ_8KHZ;
		  mCodexCtx[mCodecCtxInUse].codecIdx        = AMR_IDX;

		  mCscallCodecConfig.amrnbDecParam.bNoHeader         = pCscallCodecInfo->amrnbDecParam.bNoHeader;
		  mCscallCodecConfig.amrnbDecParam.memory_preset     = pCscallCodecInfo->amrnbDecParam.memory_preset;
		  mCscallCodecConfig.amrnbDecParam.bErrorConcealment = pCscallCodecInfo->amrnbDecParam.bErrorConcealment;
		  OstTraceFiltInst2 (TRACE_ALWAYS, "Cscall/Downlink: send setParamter to AMR-NB codec (memory_preset=%d) (bErrorConcealment=%d)", mCscallCodecConfig.amrnbDecParam.memory_preset , mCscallCodecConfig.amrnbDecParam.bErrorConcealment);
		  configure_amr.setParameter(mCscallCodecConfig.amrnbDecParam);

		  mCscallCodecConfig.amrnbDecConfig.ePayloadFormat = pCscallCodecInfo->amrnbDecConfig.ePayloadFormat;
		  mCscallCodecConfig.amrnbDecConfig.bEfr_on	     = pCscallCodecInfo->amrnbDecConfig.bEfr_on;
		  OstTraceFiltInst2 (TRACE_ALWAYS, "Cscall/Downlink: send setConfig to AMR-NB codec (ePayloadFormat=%d) (bEfr_on=%d)", mCscallCodecConfig.amrnbDecConfig.ePayloadFormat , mCscallCodecConfig.amrnbDecConfig.bEfr_on);
		  configure_amr.setConfig(mCscallCodecConfig.amrnbDecConfig);

		  break;
		}

        case CODEC_EFR:
		{
		  mCodexCtx[mCodecCtxInUse].codecSampleFreq = FREQ_8KHZ;
		  mCodexCtx[mCodecCtxInUse].codecIdx        = EFR_IDX;

		  mCscallCodecConfig.amrnbDecParam.bNoHeader         = pCscallCodecInfo->amrnbDecParam.bNoHeader;
		  mCscallCodecConfig.amrnbDecParam.memory_preset     = pCscallCodecInfo->amrnbDecParam.memory_preset;
		  mCscallCodecConfig.amrnbDecParam.bErrorConcealment = pCscallCodecInfo->amrnbDecParam.bErrorConcealment;
		  OstTraceFiltInst2 (TRACE_ALWAYS, "Cscall/Downlink: send setParamter to EFR codec (memory_preset=%d) (bErrorConcealment=%d)", mCscallCodecConfig.amrnbDecParam.memory_preset , mCscallCodecConfig.amrnbDecParam.bErrorConcealment);
		  configure_efr.setParameter(mCscallCodecConfig.amrnbDecParam);

		  mCscallCodecConfig.amrnbDecConfig.ePayloadFormat = pCscallCodecInfo->amrnbDecConfig.ePayloadFormat;
		  mCscallCodecConfig.amrnbDecConfig.bEfr_on	     = pCscallCodecInfo->amrnbDecConfig.bEfr_on;
		  OstTraceFiltInst2 (TRACE_ALWAYS, "Cscall/Downlink: send setConfig to EFR codec (ePayloadFormat=%d) (bEfr_on=%d)", mCscallCodecConfig.amrnbDecConfig.ePayloadFormat , mCscallCodecConfig.amrnbDecConfig.bEfr_on);
		  configure_efr.setConfig(mCscallCodecConfig.amrnbDecConfig);

		  break;
		}

	  case CODEC_GSM_FR:
		{
		  mCodexCtx[mCodecCtxInUse].codecSampleFreq = FREQ_8KHZ;
		  mCodexCtx[mCodecCtxInUse].codecIdx        = FR_IDX;

		  mCscallCodecConfig.gsmfrDecParam.memory_preset = pCscallCodecInfo->gsmfrDecParam.memory_preset;
		  OstTraceFiltInst1 (TRACE_ALWAYS, "Cscall/Downlink: send setParamter to GSMFR codec (memory_preset=%d)", mCscallCodecConfig.gsmfrDecParam.memory_preset);
		  configure_gsmfr.setParameter(mCscallCodecConfig.gsmfrDecParam);

		  mCscallCodecConfig.gsmfrDecConfig.bDtx            = pCscallCodecInfo->gsmfrDecConfig.bDtx;
		  mCscallCodecConfig.gsmfrDecConfig.epayload_format = pCscallCodecInfo->gsmfrDecConfig.epayload_format;
		  OstTraceFiltInst2 (TRACE_ALWAYS, "Cscall/Downlink: send setConfig to GSMFR codec (ePayloadFormat=%d) (bDtx=%d)", mCscallCodecConfig.gsmfrDecConfig.epayload_format , mCscallCodecConfig.gsmfrDecConfig.bDtx);
		  configure_gsmfr.setConfig(mCscallCodecConfig.gsmfrDecConfig);

		  break;
		}
	  case CODEC_GSM_HR:
		{
		  mCodexCtx[mCodecCtxInUse].codecSampleFreq = FREQ_8KHZ;
		  mCodexCtx[mCodecCtxInUse].codecIdx        = HR_IDX;

		  mCscallCodecConfig.gsmhrDecParam.memory_preset = pCscallCodecInfo->gsmhrDecParam.memory_preset;
		  OstTraceFiltInst1 (TRACE_ALWAYS, "Cscall/Downlink: send setParamter to GSMHR codec (memory_preset=%d)", mCscallCodecConfig.gsmhrDecParam.memory_preset);
		  configure_gsmhr.setParameter(mCscallCodecConfig.gsmhrDecParam);

		  mCscallCodecConfig.gsmhrDecConfig.epayload_format = pCscallCodecInfo->gsmhrDecConfig.epayload_format;
		  OstTraceFiltInst1 (TRACE_ALWAYS, "Cscall/Downlink: send setConfig to GSMHR codec (ePayloadFormat=%d)", mCscallCodecConfig.gsmhrDecConfig.epayload_format);
		  configure_gsmhr.setConfig(mCscallCodecConfig.gsmhrDecConfig);

		  break;
		}
	  case CODEC_AMR_WB:
		{
		  mCodexCtx[mCodecCtxInUse].codecSampleFreq = FREQ_16KHZ;
		  mCodexCtx[mCodecCtxInUse].codecIdx        = AMRWB_IDX;

		  mCscallCodecConfig.amrwbDecParam.bNoHeader          = pCscallCodecInfo->amrwbDecParam.bNoHeader;
		  mCscallCodecConfig.amrwbDecParam.bErrorConcealment  = pCscallCodecInfo->amrwbDecParam.bErrorConcealment;
		  OstTraceFiltInst1 (TRACE_ALWAYS, "Cscall/Downlink: send setParamter to AMR-WB codec (bErrorConcealment=%d)", mCscallCodecConfig.amrwbDecParam.bErrorConcealment);
		  configure_amrwb.setParameter(mCscallCodecConfig.amrwbDecParam);

		  mCscallCodecConfig.amrwbDecConfig.ePayloadFormat    = pCscallCodecInfo->amrwbDecConfig.ePayloadFormat;
		  OstTraceFiltInst1 (TRACE_ALWAYS, "Cscall/Downlink: send setConfig to AMR-WB codec (ePayloadFormat=%d)", mCscallCodecConfig.amrwbDecConfig.ePayloadFormat);
		  configure_amrwb.setConfig(mCscallCodecConfig.amrwbDecConfig);

		  break;
		}
	  case CODEC_PCM8:
		{
		  mCodexCtx[mCodecCtxInUse].codecSampleFreq = FREQ_8KHZ;
		  mCodexCtx[mCodecCtxInUse].codecIdx        = PCM_IDX;

		  mCscallCodecConfig.pcmDecConfig.sampling_rate = PCM_SamplingRate_8k;
		  mCscallCodecConfig.pcmDecConfig.big_endian_flag = pCscallCodecInfo->pcmDecConfig.big_endian_flag;
		  OstTraceFiltInst2 (TRACE_ALWAYS, "Cscall/Downlink: send setConfig to PCM codec (sampling_rate=%d) (big_endian=%d)", mCscallCodecConfig.pcmDecConfig.sampling_rate,mCscallCodecConfig.pcmDecConfig.big_endian_flag);

		  configure_pcm.setConfig(mCscallCodecConfig.pcmDecConfig);

		  break;
		}
	  case CODEC_PCM16:
		{
		  mCodexCtx[mCodecCtxInUse].codecSampleFreq = FREQ_16KHZ;
		  mCodexCtx[mCodecCtxInUse].codecIdx        = PCM_IDX;

		  mCscallCodecConfig.pcmDecConfig.sampling_rate = PCM_SamplingRate_16k;
		  mCscallCodecConfig.pcmDecConfig.big_endian_flag = pCscallCodecInfo->pcmDecConfig.big_endian_flag;
		  OstTraceFiltInst2 (TRACE_ALWAYS, "Cscall/Downlink: send setConfig to PCM codec (sampling_rate=%d) (big_endian=%d)", mCscallCodecConfig.pcmDecConfig.sampling_rate,mCscallCodecConfig.pcmDecConfig.big_endian_flag);

		  configure_pcm.setConfig(mCscallCodecConfig.pcmDecConfig);

		  break;
		}
	  case CODEC_NONE:
		{
		  mDroppedFrameNb  = 0;
          decoderClose(1-mCodecCtxInUse);
          srcClose();
          OstTraceFiltInst1 (TRACE_ALWAYS, "Cscall/Downlink: end of call (mDLFakeFrameGenerated=%d)", mDLFakeFrameGenerated);
          OstTraceFiltInst3 (TRACE_ALWAYS, "Cscall/Downlink: end of call (mCodexCtx[0].codecInitialized=%d) (mCodexCtx[1].codecInitialized=%d) (mSRCInitialized=%d)", mCodexCtx[0].codecInitialized, mCodexCtx[1].codecInitialized, mSRCInitialized);
          mDLFakeFrameGenerated  = 0;
          mConsecutiveFakeFrameSent = 0;
          mCodecCtxInUse = 0;
          mFakeFrameGenerator->setConfiguration(CODEC_NONE, &mCscallCodecConfig);
          if(mPorts[INPUT_PORT].queuedBufferCount())
            {
                scheduleProcessEvent();
            }
		  break;
		}

	  default:
		ARMNMF_DBC_ASSERT(0);
	}

	// Open the codec:
	// FIXME: Codec need to be configured first, we should be able to avoid this.
	if(codecType != CODEC_NONE)
	{
	  decoderOpen(mCodecCtxInUse);
	  srcOpen();
	}

  }
  else
  {
	// reconfigure codec
	switch(mCodexCtx[mCodecCtxInUse].codecInUse)
	{
	  case CODEC_AMR_NB:

		{
		  mCscallCodecConfig.amrnbDecConfig.ePayloadFormat = pCscallCodecInfo->amrnbDecConfig.ePayloadFormat;
		  mCscallCodecConfig.amrnbDecConfig.bEfr_on	     = pCscallCodecInfo->amrnbDecConfig.bEfr_on;
		  OstTraceFiltInst2 (TRACE_ALWAYS, "Cscall/Downlink: Reconfiguration .. send setConfig to AMR-NB codec (ePayloadFormat=%d) (bEfr_on=%d)", mCscallCodecConfig.amrnbDecConfig.ePayloadFormat , mCscallCodecConfig.amrnbDecConfig.bEfr_on);
		  configure_amr.setConfig(mCscallCodecConfig.amrnbDecConfig);

		  break;
		}
	  case CODEC_EFR:
        {
		  mCscallCodecConfig.amrnbDecConfig.ePayloadFormat = pCscallCodecInfo->amrnbDecConfig.ePayloadFormat;
		  mCscallCodecConfig.amrnbDecConfig.bEfr_on	     = pCscallCodecInfo->amrnbDecConfig.bEfr_on;
		  OstTraceFiltInst2 (TRACE_ALWAYS, "Cscall/Downlink: Reconfiguration .. send setConfig to EFR codec (ePayloadFormat=%d) (bEfr_on=%d)", mCscallCodecConfig.amrnbDecConfig.ePayloadFormat , mCscallCodecConfig.amrnbDecConfig.bEfr_on);
		  configure_efr.setConfig(mCscallCodecConfig.amrnbDecConfig);

		  break;
		}

	  case CODEC_GSM_FR:
		{
		  mCscallCodecConfig.gsmfrDecConfig.bDtx            = pCscallCodecInfo->gsmfrDecConfig.bDtx;
		  mCscallCodecConfig.gsmfrDecConfig.epayload_format = pCscallCodecInfo->gsmfrDecConfig.epayload_format;
		  OstTraceFiltInst2 (TRACE_ALWAYS, "Cscall/Downlink: Reconfiguration .. send setConfig to GSMFR codec (ePayloadFormat=%d) (bDtx=%d)", mCscallCodecConfig.gsmfrDecConfig.epayload_format , mCscallCodecConfig.gsmfrDecConfig.bDtx);
		  configure_gsmfr.setConfig(mCscallCodecConfig.gsmfrDecConfig);

		  break;
		}
	  case CODEC_GSM_HR:
		{
		  mCscallCodecConfig.gsmhrDecConfig.epayload_format = pCscallCodecInfo->gsmhrDecConfig.epayload_format;
		  OstTraceFiltInst1 (TRACE_ALWAYS, "Cscall/Downlink: Reconfiguration .. send setConfig to GSMHR codec (ePayloadFormat=%d)", mCscallCodecConfig.gsmhrDecConfig.epayload_format);
		  configure_gsmhr.setConfig(mCscallCodecConfig.gsmhrDecConfig);

		  break;
		}
	  case CODEC_AMR_WB:
		{
		  mCscallCodecConfig.amrwbDecConfig.ePayloadFormat = pCscallCodecInfo->amrwbDecConfig.ePayloadFormat;
		  OstTraceFiltInst1 (TRACE_ALWAYS, "Cscall/Downlink: Reconfiguration .. send setPayload to AMR-WB codec (ePayloadFormat=%d)", mCscallCodecConfig.amrwbDecConfig.ePayloadFormat);
		  configure_amrwb.setConfig(mCscallCodecConfig.amrwbDecConfig);

		  break;
		}
	  case CODEC_PCM8:
	  case CODEC_PCM16:
		{
		  mCscallCodecConfig.pcmDecConfig.sampling_rate = pCscallCodecInfo->pcmDecConfig.sampling_rate;
		  OstTraceFiltInst1 (TRACE_ALWAYS, "Cscall/Downlink: send setConfig to PCM codec (sampling_rate=%d)", mCscallCodecConfig.pcmDecConfig.sampling_rate);
		  configure_pcm.setConfig(mCscallCodecConfig.pcmDecConfig);

		  break;
		}
	  default:
		ARMNMF_DBC_ASSERT(0);
	}
    mFakeFrameGenerator->setConfiguration(codecType, &mCscallCodecConfig);
  }
  OstTraceFiltInst1 (TRACE_ALWAYS, "Cscall/Downlink: inform protocolhandler that new codec config has been applied (codecType=%d)",codecType);
  codecApplied.newCodecApplied();
}



/**
 *
 */
void METH(setParameter)(t_bool real_time_output)
{
  mRealTimeOutput = (bool)real_time_output;
}


/**
 *
 */
void METH(setSampleFreq)(t_sample_freq sample_freq)
{
  if(mOutputSampleFreq == FREQ_UNKNOWN)
  {
    mDecoderBufferHeader.nFilledLen = 0;
	mDecoderBufferHeader.nFlags     = 0;
	mSrcBufferHeader.nFilledLen     = 0;
	mSrcBufferHeader.nFlags         = 0;
  }
  mOutputSampleFreq = sample_freq;
  srcClose();
  srcOpen();
}

void METH(setProtocol)(CsCallProtocol_t protocol)
{
  mFakeFrameGenerator = FakeFrameGenerator::createFakeFrameGenerator(protocol);
  if(!mFakeFrameGenerator)
  {
	proxy.eventHandler(OMX_EventError, (t_uint32)OMX_ErrorInsufficientResources, 0);
	ARMNMF_DBC_ASSERT(0);
	return;
  }
}

/////////////////////
// lifecycle.stopper
/////////////////////
void METH(stop)(void)
{
  if(mFakeFrameGenerator)
  {
	delete mFakeFrameGenerator;
	mFakeFrameGenerator = 0;
  }
}
/**
 * Send fake decoded frame
 * */
void cscall_nmf_host_downlink::sendFakeFrame(t_sint64 nTimeStamp) {
    if (mPorts[OUTPUT_PORT].queuedBufferCount() != 0) {
        int CtxToBeUsed = mCodecCtxInUse;
        if (mCodexCtx[CtxToBeUsed].codecInitialized < TRANSFER_STARTED) {
            CtxToBeUsed = 1-mCodecCtxInUse;
            if (mCodexCtx[CtxToBeUsed].codecInitialized < TRANSFER_STARTED) {
                OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/Downlink: No transfer started, cannot generate fake frame, return");
                return;
            }
        }

        OstTraceFiltInst4 (TRACE_ALWAYS, "Cscall/Downlink: Generate fake frame (CtxToBeUsed=%d) (mCodecCtxInUse=%d) (mCodexCtx[CtxToBeUsed].codecInUse=%d) (mCodexCtx[mCodecCtxInUse].codecInUse=%d)", CtxToBeUsed, mCodecCtxInUse, mCodexCtx[CtxToBeUsed].codecInUse,mCodexCtx[mCodecCtxInUse].codecInUse);

        bool use_output = true;
        OMX_BUFFERHEADERTYPE * outputBuffer = getDecoderBuffer(CtxToBeUsed, &use_output);;
        OMX_BUFFERHEADERTYPE * latestBuffer = getLatestBuffer();
        if (latestBuffer) {
	        OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/Downlink: sendFakeFrame will copy latest decoded frame");
            copyBuffer(latestBuffer, outputBuffer);
        } else {
            if (!(mFakeFrameGenerator->codecCanHandleFakeFrame())) {
                OstTraceFiltInst1 (TRACE_ALWAYS, "Cscall/Downlink: Generate fake frame, codec does not support fake frame generation (codec=%d)", mCodexCtx[CtxToBeUsed].codecInUse);
                if (mConsecutiveFakeFrameSent < CSCALL_MAX_CONSECUTIVE_FAKE_FRAME) {
                    int nbSamples = mCodexCtx[CtxToBeUsed].interfaceFE.sample_struct.block_len;

                    OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/Downlink: sendFakeFrame will resend previous frame");
                    outputBuffer->nFilledLen = nbSamples * 2;
                    outputBuffer->nOffset    = 0;
                    outputBuffer->nFlags     = 0;

                    t_sint16 * sample = (t_sint16 *) outputBuffer->pBuffer;
                    if (mConsecutiveFakeFrameSent == CSCALL_DL_START_RAMP) {
                        OstTraceFiltInst0 (TRACE_DEBUG, "Cscall/Downlink: sendFakeFrame, start ramp down");
                        mGain = 1.0;
#ifdef EXP_RAMPS
                        if (nbSamples == 160){
                            mCoefGain = CSCALL_GAIN_COEF_NB;
                        } else {
                            mCoefGain = CSCALL_GAIN_COEF_WB;
                        }
#else
                        mCoefGain = 1.0/((float)(CSCALL_MAX_CONSECUTIVE_FAKE_FRAME-CSCALL_DL_START_RAMP) * (float)nbSamples);
#endif
                    } else {
#ifdef EXP_RAMPS
                        mCoefGain /= mCoefGain;
#else
                        mCoefGain += mCoefGain;
#endif

                    }
                    if (mConsecutiveFakeFrameSent >= CSCALL_DL_START_RAMP) {
                        OstTraceFiltInst0 (TRACE_DEBUG, "Cscall/Downlink: sendFakeFrame, continue ramp down");
                        for (; nbSamples > 0; nbSamples--){
                            *(sample++) *= mGain;
#ifdef EXP_RAMPS
                            mGain *= mCoefGain;
#else
                            mGain -= mCoefGain;
#endif
                        }
                    }
                    mCodexCtx[mCodecCtxInUse].codecInitialized = PH_SENDING_FAKE_FRAME;
                    mConsecutiveFakeFrameSent ++;

                } else {
                    OstTraceFiltInst0 (TRACE_DEBUG, "Cscall/Downlink: sendFakeFrame, generate empty frame");
                    memset(outputBuffer->pBuffer, 0, outputBuffer->nAllocLen);
                    outputBuffer->nFilledLen = mCodexCtx[CtxToBeUsed].interfaceFE.sample_struct.block_len * 2;
                    outputBuffer->nOffset    = 0;
                    outputBuffer->nFlags     = 0;
                }
            } else {
                OMX_BUFFERHEADERTYPE *bufIn = mFakeFrameGenerator->generateFakeFrame();
                bufIn->nTimeStamp = nTimeStamp;

                mCodexCtx[CtxToBeUsed].interfaceFE.codec_state.bfi = 1;
                OstTraceFiltInst1 (TRACE_ALWAYS, "Cscall/Downlink: sendFakeFrame, generate fake frame using decoder (Header sent=%d)", *((int*)bufIn->pBuffer));
                decodeFrame(bufIn, outputBuffer, CtxToBeUsed, true);
            }
            outputBuffer = mPorts[OUTPUT_PORT].dequeueBuffer();
            if (mCodexCtx[CtxToBeUsed].codecSampleFreq != mOutputSampleFreq) {
                if(mSRCInitialized)
                {
                    convertSampleRate(outputBuffer);
                } else {
                    OstTraceFiltInst1 (TRACE_WARNING, "Cscall/Downlink: sendFakeFrame cannot convert sample rate !!!", mDLFakeFrameGenerated);
                }
            }
            outputBuffer->nTimeStamp = nTimeStamp;

            returnBufferAsync(OUTPUT_PORT,outputBuffer);
            mDLFakeFrameGenerated ++;
            OstTraceFiltInst3 (TRACE_WARNING, "Cscall/Downlink: sendFakeFrame (mDLFakeFrameGenerated=%d) (mCodexCtx[CtxToBeUsed].codecInUse=%d) (mCodexCtx[mCodecCtxInUse].codecInUse=%d)",mDLFakeFrameGenerated,  mCodexCtx[CtxToBeUsed].codecInUse,mCodexCtx[mCodecCtxInUse].codecInUse);

        }
    } else {
        OstTraceFiltInst0 (TRACE_ERROR, "Cscall/Downlink: sendFakeFrame could not send frame no buffer available on OUTPUT_PORT");
    }
}

//////////////////////////////////////////////
//  Decoder specific
/////////////////////////////////////////////
/**
 * Return output buffer and reset variables
 * */
void cscall_nmf_host_downlink::returnOutputBuffer(void)
{
    OstTraceFiltInst1 (TRACE_DEBUG, "Cscall/Downlink: returnOutputBuffer with codec state %d", mCodexCtx[mCodecCtxInUse].codecInitialized);
    mPorts[OUTPUT_PORT].dequeueAndReturnBuffer();
    if (mCodexCtx[mCodecCtxInUse].codecInitialized != CODEC_NOT_INITIALIZED) {
        mCodexCtx[mCodecCtxInUse].codecInitialized = TRANSFER_STARTED;
    }
    mConsecutiveFakeFrameSent = 0;
    if (mCodexCtx[1-mCodecCtxInUse].codecInitialized == CODEC_TERMINATING) {
        decoderClose(1-mCodecCtxInUse);
        mFakeFrameGenerator->setConfiguration(mCodexCtx[mCodecCtxInUse].codecInUse, &mCscallCodecConfig);
        srcClose();
    }
}

/**
 * Open and reset decoder to be used
 * */
void cscall_nmf_host_downlink::decoderOpen(int ctx)
{
  if(mCodexCtx[ctx].codecInitialized)
  {
	OstTraceFiltInst1 (TRACE_ERROR, "Cscall/Downlink: call decoderOpen(%d) interface but Codec already Initialized !!!!!!!!", ctx);
	return;
  }

  OstTraceFiltInst2 (TRACE_ALWAYS, "Cscall/Downlink: call decoderOpen(), mCodexCtx[%d].codecIdx = %d", ctx, mCodexCtx[ctx].codecIdx);

  // clean codec interface
  memset(&(mCodexCtx[ctx].interfaceFE), 0, sizeof(CODEC_INTERFACE_T));

  // open decoder
  decoder[mCodexCtx[ctx].codecIdx].open(&(mCodexCtx[ctx].interfaceFE));
  ARMNMF_DBC_POSTCONDITION(mCodexCtx[ctx].interfaceFE.codec_local_struct != 0);
  ARMNMF_DBC_POSTCONDITION(mCodexCtx[ctx].interfaceFE.codec_config_local_struct != 0);

  // reset decoder
  mCodexCtx[ctx].interfaceFE.codec_state.mode                   = UNKNOWN;
  mCodexCtx[ctx].interfaceFE.codec_state.output_enable          = false;
  mCodexCtx[ctx].interfaceFE.codec_state.bfi                    = false;
#ifndef NO_DTX_FLAG
  mCodexCtx[ctx].interfaceFE.codec_state.dtx                    = false;
#endif
  mCodexCtx[ctx].interfaceFE.codec_state.first_time             = true;
  mCodexCtx[ctx].interfaceFE.sample_struct.samples_interleaved  = true;

  decoder[mCodexCtx[ctx].codecIdx].reset(&(mCodexCtx[ctx].interfaceFE));

  mCodexCtx[ctx].codecInitialized = CODEC_INITIALIZED;
  return;
}

/**
 * Close decode currently in use
 * */
void cscall_nmf_host_downlink::decoderClose(int ctx)
{
  if(!mCodexCtx[ctx].codecInitialized)
  {
	OstTraceFiltInst1 (TRACE_ERROR, "Cscall/Downlink: call decoderClose(%d) interface but no Codec Initialized !!!!!!!!", ctx);
	return;
  }

  OstTraceFiltInst2 (TRACE_ALWAYS, "Cscall/Downlink: call decoderClose(), mCodexCtx[%d].codecIdx = %d", ctx, mCodexCtx[ctx].codecIdx);

  decoder[mCodexCtx[ctx].codecIdx].close(&(mCodexCtx[ctx].interfaceFE));

  mCodexCtx[ctx].codecInitialized = CODEC_NOT_INITIALIZED;
  mCodexCtx[ctx].codecInUse       = CODEC_NONE;
  mCodexCtx[ctx].codecSampleFreq  = FREQ_UNKNOWN;

  return;
}

/**
 * Check if buffer provided seems to be correct for decoder currently in use
 * */
void cscall_nmf_host_downlink::checkSynchro(int ctx)
{
  int           dummy;
  unsigned int  syncbuff_h, syncbuff_l;
  BS_STRUCT_T   mybs;
  AUDIO_MODE_T  algo = UNKNOWN;

  mybs = mBSRead;

  syncbuff_h  = bs_read_bit_arm(&mybs, SIZE_SYNC_BUFF);
  syncbuff_l  = bs_read_bit_arm(&mybs, SIZE_SYNC_BUFF);

  algo = (AUDIO_MODE_T) decoder[mCodexCtx[ctx].codecIdx].checkSynchro(syncbuff_h, syncbuff_l, (t_sint32 *)&dummy);

  if( algo != UNKNOWN )
  {
	mCodexCtx[ctx].interfaceFE.codec_state.mode = algo;
  }
  else
  {
	mCodexCtx[ctx].interfaceFE.codec_state.bfi = true;
  }

  mCodexCtx[ctx].interfaceFE.stream_struct.bits_struct = mBSRead;
}

/**
 * Decode received frame using decoder currently in use
 * */
void cscall_nmf_host_downlink::decodeFrame(OMX_BUFFERHEADERTYPE *bufIn, OMX_BUFFERHEADERTYPE *bufOut, int ctx, bool fakeFrame)
{
  OstTraceFiltInst2 (TRACE_DEBUG, "Cscall/Downlink: Start decoding (bufIn->nFilledLen=%d), (ctx=%d)",bufIn->nFilledLen,ctx);

  //if bufout flag is set it means that is has not been propagated
  if(bufOut->nFlags & OMX_BUFFERFLAG_STARTTIME) bufIn->nFlags |= OMX_BUFFERFLAG_STARTTIME;

  // Dump input buffer
  OMX_U8 * ptr = (OMX_U8 *)(bufIn->pBuffer + bufIn->nOffset);
  OMX_U32 size = bufIn->nFilledLen + (bufIn->nFilledLen % 4);
  OstTraceFiltInst1 (TRACE_CSCALL_DL_BITSTREAM_DATA, "Cscall/Downlink: size of the following inputBuffer is %d bytes",bufIn->nFilledLen);
  for (OMX_U32 i = 0; i < size; i+=40){
	OstTraceFiltInstData(TRACE_CSCALL_DL_BITSTREAM_DATA, "Cscall/Downlink: inputBuffer[] = %{int16[]}", (OMX_U8 *)(ptr + i), (size-i > 40) ? 40 : size-i);
  }

  // init pointers on input and ouput buffers
  bs_init(&mBSRead, (unsigned int*)bufIn->pBuffer, (bufIn->nAllocLen/BS_WORD_SIZE_IN_BYTES), BS_INIT_SIZE_IN_BITS);

  mCodexCtx[ctx].interfaceFE.sample_struct.buf_add = (Float*)bufOut->pBuffer;
  decoder[mCodexCtx[ctx].codecIdx].setOutputBuf(&(mCodexCtx[ctx].interfaceFE), (void*)bufOut->pBuffer);

  // process EOS if needed
  if ((bufIn->nFilledLen == 0) && (bufIn->nFlags & OMX_BUFFERFLAG_EOS))
  {
	bufOut->nFilledLen = 0;
	bufOut->nFlags     = OMX_BUFFERFLAG_EOS;

	OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/Downlink: EOS received");
	proxy.eventHandler(OMX_EventBufferFlag, OUTPUT_PORT, OMX_BUFFERFLAG_EOS);
  }
  else
  {
	checkSynchro(ctx);
    if (!fakeFrame)
    {
        mFakeFrameGenerator->setLatestFrame(bufIn);
    }

	// decode the frame
	int status = decoder[mCodexCtx[ctx].codecIdx].decodeFrame(&(mCodexCtx[ctx].interfaceFE));

	if (status != RETURN_STATUS_OK)
	{
	  OstTraceFiltInst3 (TRACE_WARNING, "Cscall/Downlink: Error while decoding (status=%d) (output_enable=%d, output_size=%d)", status, mCodexCtx[ctx].interfaceFE.codec_state.output_enable,  mCodexCtx[ctx].interfaceFE.sample_struct.block_len);
	  ARMNMF_DBC_ASSERT_MSG(status < RETURN_STATUS_ERROR, "Cscall/Downlink: decoder return error state");
	}

	if (mCodexCtx[ctx].interfaceFE.codec_state.output_enable)
	{
      int nbSamples = mCodexCtx[ctx].interfaceFE.sample_struct.block_len;
      bufOut->nFilledLen = nbSamples * 2;

      if ((mCodexCtx[ctx].codecInitialized == PH_SENDING_FAKE_FRAME) && (mConsecutiveFakeFrameSent > CSCALL_DL_START_RAMP)) {
	      OstTraceFiltInst0 (TRACE_DEBUG, "Cscall/Downlink: apply ramp up");

          //Apply gain, return to gain 1
          t_sint16 * sample = (t_sint16 *) bufOut->pBuffer;
#ifdef EXP_RAMPS
          int i;
          for (i=CSCALL_DL_START_RAMP; i<CSCALL_MAX_CONSECUTIVE_FAKE_FRAME;i++) {
              mCoefGain *= mCoefGain;
          }
#else
          mCoefGain *= (float)(CSCALL_MAX_CONSECUTIVE_FAKE_FRAME - CSCALL_DL_START_RAMP);
#endif
          while ((mGain < 1.0) && (nbSamples > 0)) {
              *(sample++) *= mGain;
#ifdef EXP_RAMPS
              mGain /= mCoefGain;
#else
              mGain += mCoefGain;
#endif
              nbSamples --;
          }
          mGain = 1.0;
      }

	  // Propagate Time Stamp and nFlags
	  bufOut->nFlags     = bufIn->nFlags;
	  bufOut->nTimeStamp = bufIn->nTimeStamp;
	  OstTraceFiltInst3 (TRACE_DEBUG, "Cscall/Downlink: DL-frame sent latency control (decoder_wrapper, output) (timestamp = 0x%x 0x%x us) flags=%x", (bufOut->nTimeStamp)>>32,(bufOut->nTimeStamp)&0xffffffffu,bufOut->nFlags);

	  // Dump output buffer
	  OMX_U8 * ptr = (OMX_U8 *)(bufOut->pBuffer);
	  OMX_U32 size = bufOut->nFilledLen;
	  OstTraceFiltInst1 (TRACE_CSCALL_DL_PCM_DATA, "Cscall/Downlink: decoding (bufOut->nFilledLen=%d)",bufOut->nFilledLen);
	  for (OMX_U32 i = 0; i < size; i+=40){
		OstTraceFiltInstData(TRACE_CSCALL_DL_PCM_DATA, "Cscall/Downlink: outputBuffer[]  = %{int16[]}", (OMX_U8 *)(ptr + i), (size-i > 40) ? 40 : size-i);
	  }
#ifdef MMPROBE_ENABLED
	  mmprobe_probe(MM_PROBE_RX_SPD_OUTPUT, bufOut->pBuffer, bufOut->nFilledLen);
#endif
	}

#ifndef NO_DTX_FLAG
	// Reset dtx Flag
	mCodexCtx[ctx].interfaceFE.codec_state.dtx = false;
#endif
	if (mCodexCtx[ctx].interfaceFE.codec_state.bfi)
	{
	  OstTraceFiltInst0 (TRACE_DEBUG, "Cscall/Downlink: bfi was set decoding, reset bfi");
	  mCodexCtx[ctx].interfaceFE.codec_state.bfi = 0;
	}
  }

  OstTraceFiltInst0 (TRACE_DEBUG, "Cscall/Downlink: End of decoding");
}


//////////////////////////////////////////////
//  SRC specific
/////////////////////////////////////////////
void cscall_nmf_host_downlink::srcOpen(void)
{
  int ctxToBeUsed = -1;
  if(getTraceInfoPtr()) {
    OstTraceFiltInst2 (TRACE_DEBUG, "Cscall/Downlink: openSRC() (mSRCInitialized=%d), (mOutputSampleFreq=%d)", mSRCInitialized, mOutputSampleFreq);
  }
  if ((mSRCInitialized) || (mOutputSampleFreq  == FREQ_UNKNOWN)) {
    return;
  }
  if(getTraceInfoPtr()) {
    OstTraceFiltInst2 (TRACE_DEBUG, "Cscall/Downlink: openSRC() (mCodexCtx[0].codecInitialized=%d) (mCodexCtx[0].codecSampleFreq=%d)", mCodexCtx[0].codecInitialized, mCodexCtx[0].codecSampleFreq);
    OstTraceFiltInst2 (TRACE_DEBUG, "Cscall/Downlink: openSRC() (mCodexCtx[1].codecInitialized=%d) (mCodexCtx[1].codecSampleFreq=%d)", mCodexCtx[1].codecInitialized, mCodexCtx[1].codecSampleFreq);
  }
  if ((mCodexCtx[0].codecInitialized != CODEC_NOT_INITIALIZED) && (mCodexCtx[0].codecSampleFreq != mOutputSampleFreq)) {
    ctxToBeUsed = 0;
  }
  if ((mCodexCtx[1].codecInitialized != CODEC_NOT_INITIALIZED) && (mCodexCtx[1].codecSampleFreq != mOutputSampleFreq)) {
    ctxToBeUsed = 1;
  }
  if (ctxToBeUsed != -1)
  {
	t_host_effect_config config;
	int status, heap_size, size;

	config.block_size				  = 20; //this is the size in ms for src in mode 6
	config.infmt.freq				  = mCodexCtx[ctxToBeUsed].codecSampleFreq;
	config.infmt.nof_channels		  = 1;
	config.infmt.nof_bits_per_sample  = 16;
	config.infmt.headroom			  = 0;
	config.infmt.interleaved		  = 1;
	config.outfmt.freq				  = mOutputSampleFreq;
	config.outfmt.nof_channels		  = 1;
	config.outfmt.nof_bits_per_sample = 16;
	config.outfmt.headroom			  = 0;
	config.outfmt.interleaved		  = 1;

	if(getTraceInfoPtr())
	  OstTraceFiltInst3 (TRACE_ALWAYS, "Cscall/Downlink: openSRC() open and configure SRC effect (config.block_size=%d) (input_freq=%d) (output_freq=%d)",config.block_size,config.infmt.freq,config.outfmt.freq);

	size = config.block_size * config.outfmt.nof_channels;

	resample_calc_max_heap_size_fixin_fixout(config.infmt.freq,config.outfmt.freq,
            SRC_LOW_LATENCY_IN_MSEC, &heap_size,
            size, config.outfmt.nof_channels, 0);

	mHeap = malloc(heap_size);
	ARMNMF_DBC_ASSERT(mHeap != NULL);

	// Init Src lib
	status = resample_x_init_ctx_low_mips_fixin_fixout_sample16(
		(char*)mHeap, heap_size, &mResampleContext, config.infmt.freq,
		config.outfmt.freq, SRC_LOW_LATENCY_IN_MSEC, config.outfmt.nof_channels, size);
	ARMNMF_DBC_ASSERT(status == 0);

	mSRCInitialized = true;
  }
}


void cscall_nmf_host_downlink::srcClose(void)
{
  if(getTraceInfoPtr()) {
    OstTraceFiltInst1 (TRACE_DEBUG, "Cscall/Downlink: closeSRC() (mSRCInitialized=%d)", mSRCInitialized);
  }

  if(mSRCInitialized)
  {
	if(getTraceInfoPtr()) {
      OstTraceFiltInst2 (TRACE_DEBUG, "Cscall/Downlink: closeSRC() (mCodexCtx[0].codecInitialized=%d) (mCodexCtx[0].codecSampleFreq=%d)", mCodexCtx[0].codecInitialized, mCodexCtx[0].codecSampleFreq);
      OstTraceFiltInst2 (TRACE_DEBUG, "Cscall/Downlink: closeSRC() (mCodexCtx[1].codecInitialized=%d) (mCodexCtx[1].codecSampleFreq=%d)", mCodexCtx[1].codecInitialized, mCodexCtx[1].codecSampleFreq);
    }
    if ((mOutputSampleFreq == FREQ_UNKNOWN) ||
            (((mCodexCtx[0].codecInitialized == CODEC_NOT_INITIALIZED) || (mCodexCtx[0].codecSampleFreq == mOutputSampleFreq)) &&
            ((mCodexCtx[1].codecInitialized == CODEC_NOT_INITIALIZED) || (mCodexCtx[1].codecSampleFreq == mOutputSampleFreq))) ) {
        if(getTraceInfoPtr()) {
            OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/Downlink: closeSRC() close SRC effect ");
		}
        // We must free the memory allocated during the srcOpen()
		free(mHeap);
        mSRCInitialized = false;
    }
  }
}


void cscall_nmf_host_downlink::convertSampleRate(OMX_BUFFERHEADERTYPE *bufOut)
{
  OMX_BUFFERHEADERTYPE *bufIn = &mDecoderBufferHeader;
  int consumed = 0, produced = 0, flush = 0, flush_in = 0, blocksize = 20,
      nbchannel = 1, bytePerSample = (16 / 8), *inputbuffer, *outputBuffer;

  OstTraceFiltInst1 (TRACE_DEBUG, "Cscall/Downlink: convertSampleRate() start processing (bufIn->nFilledLen =%d)",mDecoderBufferHeader.nFilledLen);
  if(bufIn->nFilledLen)
  {
	inputbuffer  = (int *)((int)bufIn->pBuffer  + bufIn->nOffset);
	outputBuffer = (int *)((int)bufOut->pBuffer + bufOut->nOffset);

	consumed = blocksize;
	produced = 0;

	flush       = (bufIn->nFlags & OMX_BUFFERFLAG_EOS) ? true : false;
	flush_in    = flush;

	if ((*mResampleContext.processing) (
           &mResampleContext,
           (int*)inputbuffer,
           nbchannel,
           &consumed,
           (int*)outputBuffer,
           &produced,
           &flush) != 0)
	{
	   ARMNMF_DBC_ASSERT(FALSE);
	}

	if (flush_in == 0) {
	   ARMNMF_DBC_POSTCONDITION(consumed == blocksize);
	   ARMNMF_DBC_POSTCONDITION(produced == blocksize);
	}

	produced*=(mResampleContext.fout_khz);
	consumed*=(mResampleContext.fin_khz); // FIXME Variable never read, could it be usefull for debugging?

	if (flush) {
	   bufOut->nFlags |= OMX_BUFFERFLAG_EOS;
	}
	bufOut->nFilledLen = produced *nbchannel*bytePerSample;

  }
  else
  {
	ARMNMF_DBC_ASSERT(mDecoderBufferHeader.nFlags & OMX_BUFFERFLAG_EOS);
	bufOut->nFilledLen = 0;
  }
  OstTraceFiltInst1 (TRACE_DEBUG, "Cscall/Downlink: convertSampleRate() end processing   (bufOut->nFilledLen =%d)",bufOut->nFilledLen);

  bufOut->nFlags                  = mDecoderBufferHeader.nFlags;
  bufOut->nTimeStamp              = mDecoderBufferHeader.nTimeStamp;
  mDecoderBufferHeader.nFlags     = 0;
  mDecoderBufferHeader.nFilledLen = 0;
}



//////////////////////////////////////////////
//  Internal buffers
/////////////////////////////////////////////
inline
bool cscall_nmf_host_downlink::availableFreeBuffer(void)
{
  if((mDecoderBufferHeader.nFilledLen == 0) &&
	 (mSrcBufferHeader.nFilledLen == 0))
	return true;

  return false;
}

inline
OMX_BUFFERHEADERTYPE * cscall_nmf_host_downlink::getLatestBuffer(void)
{
  if(mDecoderBufferHeader.nFilledLen ||
	 mDecoderBufferHeader.nFlags)
	return &mDecoderBufferHeader;

  if(mSrcBufferHeader.nFilledLen ||
	 mSrcBufferHeader.nFlags)
	return &mSrcBufferHeader;

  return (OMX_BUFFERHEADERTYPE *)0;
}

inline
OMX_BUFFERHEADERTYPE * cscall_nmf_host_downlink::getDecoderBuffer(int ctx, bool *use_output_buffer)
{
  // if SRC is needed or there is no buffer on output port
  // use mDecoderBufferHeader to decode codec frame
  // Otherwise directly decode inside output buffer
  if((!mPorts[OUTPUT_PORT].queuedBufferCount())||
	 (mCodexCtx[ctx].codecSampleFreq != mOutputSampleFreq) )
  {
	*use_output_buffer = false;
	return &mDecoderBufferHeader;
  }
  else
  {
	*use_output_buffer = true;
	return mPorts[OUTPUT_PORT].getBuffer(0);
  }
}

inline
OMX_BUFFERHEADERTYPE * cscall_nmf_host_downlink::getSrcBuffer(bool *use_output_buffer)
{
  // if no buffer on output port use mSrcBufferHeader to do sampling rate conversion
  // Otherwise directly process inside output buffer
  if(!mPorts[OUTPUT_PORT].queuedBufferCount())
  {
	*use_output_buffer = false;
	return &mSrcBufferHeader;
  }
  else
  {
	*use_output_buffer = true;
	return mPorts[OUTPUT_PORT].getBuffer(0);
  }
}




void cscall_nmf_host_downlink::copyBuffer(OMX_BUFFERHEADERTYPE *bufin, OMX_BUFFERHEADERTYPE *bufout)
{

  OstTraceFiltInst2 (TRACE_DEBUG, "Cscall/Downlink: copyBuffer %d bytes (bufout->nAllocLen=%d)", (int)bufin->nFilledLen, (int)bufout->nAllocLen);
  ARMNMF_DBC_ASSERT(bufout->nAllocLen >= bufin->nFilledLen);

  // copy data
  memcpy((void *)bufout->pBuffer, (void *)bufin->pBuffer, bufin->nFilledLen);

  // update header
  bufout->nFilledLen = bufin->nFilledLen;
  bufout->nOffset    = 0;
  bufout->nFlags     = bufin->nFlags;

  // clear buf in
  bufin->nFilledLen = 0;
  bufin->nFlags     = 0;
}

