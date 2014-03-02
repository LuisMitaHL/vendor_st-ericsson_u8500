/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \file   uplink.cpp
 * \brief  Interfaced between encoder libraries and other NMF components. Encoder wrapper takes PCM data from Rate-UL component, encode it with encoder chosen by network and send it to protocol handler.
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include <cscall/nmf/host/uplink.nmf>
#include <armnmf_dbc.h>
#include <string.h>
#include "cscall_time.h"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cscall_nmf_host_uplink_src_uplinkTraces.h"
#endif

// MMProbe framework
#ifdef MMPROBE_ENABLED
#include "t_mm_probe.h"
#include "r_mm_probe.h"
#endif

// size of the bitstream word (configured through bs-init)
// TODO FIXME: Set to 8 bits when the libbitstream supports it!
#define BS_INIT_SIZE_IN_BITS 32
#define BS_WORD_SIZE_IN_BYTES (BS_INIT_SIZE_IN_BITS/8)
#define MAX_VALID_WAIT_TIME_US (100000)


///////////////////////////////////////////
//  Constructor
///////////////////////////////////////////
cscall_nmf_host_uplink::cscall_nmf_host_uplink (void)
{
  memset(&mInterfaceFE,       0, sizeof(CODEC_INTERFACE_T));
  memset(&mEncoderBuffer,     0, sizeof(OMX_BUFFERHEADERTYPE));
  memset(&mCscallCodecConfig, 0, sizeof(Cscall_Codec_Config_t));

  mCodecInUse               = CODEC_NONE;
  mCodecIdx                 = AMR_IDX;
  mCodecInitialized         = false;
  mSRCInitialized           = false;
  mRealTimeInput            = true;
  mInputSampleFreq          = FREQ_UNKNOWN;
  mCodecSampleFreq          = FREQ_UNKNOWN;
  mNbFreeBuffer             = NB_BUFFER;
  mReadBufferIdx            = 0;
  mWriteBufferIdx           = 0;
  mBufferSampleSize         = 0;
  mRingBuffer             = &mRingnExtraBuffer[0];
  for(int i=0; i<NB_BUFFER; i++)
  {
	mBufferFlags[i]             = 0;
	mBufferTimeStamps[i]        = 0;
    mLastReceivedBufferTime[i]  = 0;
  }
  mMaxTimeReached                    = 0;
  mSumWaitTime                       = 0;
  mMaxWaitTime.threshold             = 5000;
  mMaxWaitTime.targetWaitTime        = 1000;
  mMaxWaitTime.waitBeforeReconfigure = 10;
  mULFakeGeneratedFrameNb              = 0;
  mNbFrameReceived                   = 0;
  mNbFrameSent                       = 0;
  mNbInputBufferTrashed              = 0;
  mMissingULFrameNb                  = 0;
  mOutBufferReset                    = false;
  mLostULFrameNb                     = 0;
}


//////////////////////////////////////////////
//  Component FSM
/////////////////////////////////////////////
void cscall_nmf_host_uplink::process()
{
  if (mPorts[INPUT_PORT].queuedBufferCount() == 0)
  {
	return;
  }

  if(mRealTimeInput || mNbFreeBuffer)
  {
	OMX_BUFFERHEADERTYPE *bufIn = mPorts[INPUT_PORT].getBuffer(0);
#ifdef MMPROBE_ENABLED
  mmprobe_probe(MM_PROBE_TX_CS_CALL_INPUT, bufIn->pBuffer, bufIn->nFilledLen);
#endif

    if (mOutBufferReset)
    {
        OstTraceFiltInst1(TRACE_WARNING,"Cscall/uplink: WARNING!! process() receive UL-frame, reset counter: (mMissingULFrameNb=%d)",mMissingULFrameNb);
        mOutBufferReset = false;
        mMissingULFrameNb = 0;
    }

	if(mInputSampleFreq == mCodecSampleFreq)
	{
	  copyBuffer(bufIn);
	}
	else
	{
	  convertSampleRate(bufIn);
	}

	mLastReceivedBufferTime[mWriteBufferIdx] = getTime();

	mWriteBufferIdx = (mWriteBufferIdx + 1) % NB_BUFFER;
	if(mNbFreeBuffer > 0)
	{
	  mNbFreeBuffer --;
    }
	else
	{
	  mNbInputBufferTrashed++;
	  OstTraceFiltInst4 (TRACE_WARNING, "Cscall/Uplink: overwritting one frame (mNbInputBufferTrashed=%d) (NB_BUFFER = %d) (mNbFrameReceived=%d) (mNbFrameSent=%d)", mNbInputBufferTrashed, NB_BUFFER, mNbFrameReceived, mNbFrameSent);
	  mReadBufferIdx = (mReadBufferIdx + 1) % NB_BUFFER;
    }

    if (mNbFrameReceived == 0) {
        OstTraceFiltInst4 (TRACE_ALWAYS, "Cscall/Uplink: First Frame received reset (mULFakeGeneratedFrameNb=%d) (mNbInputBufferTrashed=%d) (mNbFrameReceived=%d) (mNbFrameSent=%d)",mULFakeGeneratedFrameNb,mNbInputBufferTrashed, mNbFrameReceived, mNbFrameSent);
        mULFakeGeneratedFrameNb = 0;
    }
    mNbFrameReceived++;
    OstTraceFiltInst4 (TRACE_DEBUG, "Cscall/Uplink: Frame received (mNbFrameReceived=%d) (mNbFrameSent=%d) (mULFakeGeneratedFrameNb=%d) (mNbInputBufferTrashed=%d)", mNbFrameReceived, mNbFrameSent,mULFakeGeneratedFrameNb, mNbInputBufferTrashed);
	mPorts[INPUT_PORT].dequeueAndReturnBuffer();
  }
}

void cscall_nmf_host_uplink::reset()
{
  OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/Uplink: reset() called ");

  if(mCodecInitialized) encoderClose();
  srcClose();
  resetRingBuffer();
}

void METH(disablePortIndication)(t_uint32 portIdx)
{
  OstTraceFiltInst1 (TRACE_ALWAYS, "Cscall/Uplink: disablePortIndication() called (portIdx=%d)",portIdx);
  if(portIdx == INPUT_PORT)
  {
	srcClose();
    resetRingBuffer();
	mInputSampleFreq = FREQ_UNKNOWN;
	if(mCodecInUse == CODEC_NONE) mCodecSampleFreq = FREQ_UNKNOWN;
  }
}

void METH(enablePortIndication)(t_uint32 portIdx)
{
  OstTraceFiltInst1 (TRACE_ALWAYS, "Cscall/Uplink: enablePortIndication() called (portIdx=%d)",portIdx);
  // Note srcOpen is done on setSampleFreq which is always called before an enable port
}

void METH(flushPortIndication)(t_uint32 portIdx)
{
  OstTraceFiltInst1 (TRACE_ALWAYS, "Cscall/Uplink: flushPortIndication() called (portIdx=%d)",portIdx);
}



////////////////////////////////////////////////////////////
//          Provided Interfaces
////////////////////////////////////////////////////////////
void METH(fsmInit)(fsmInit_t initFsm)
{
  // trace init (mandatory before port init)
  if (initFsm.traceInfoAddr){
    setTraceInfo(initFsm.traceInfoAddr, initFsm.id1);
  }
  OstTraceFiltInst1 (TRACE_ALWAYS, "Cscall/Uplink: fsmInit() called (id=%d)",initFsm.id1);

  mPorts[INPUT_PORT].init (InputPort, false, false, 0, 0, 1, &inputport,   INPUT_PORT,
                           (initFsm.portsDisabled & (1 << (INPUT_PORT)))!=0, (initFsm.portsTunneled & (1 << (INPUT_PORT)))!=0, this);
  mPorts[OUTPUT_PORT].init(OutputPort,false ,false, 0, 0, 1, &outputport, OUTPUT_PORT,
                           (initFsm.portsDisabled & (1 << (OUTPUT_PORT)))!=0, (initFsm.portsTunneled & (1 << (OUTPUT_PORT)))!=0, this);

  init(2, mPorts, &proxy, &me, false);
}

void METH(setTunnelStatus) (t_sint16 portIdx, t_uint16 isTunneled)
{
  OstTraceFiltInst2 (TRACE_ALWAYS, "Cscall/Uplink: setTunnelStatus() called (portIdx=%d) (isTunneled=%d)",portIdx,isTunneled);
  mPorts[portIdx].setTunnelStatus(isTunneled);
}


void METH(configureCodec)(CsCallCodec_t codecType, void* pCodecInfo)
{
  // Send config to codec
  Cscall_Codec_Config_t * pCscallCodecInfo = (Cscall_Codec_Config_t *) pCodecInfo;
  OstTraceFiltInst2 (TRACE_ALWAYS, "Cscall/Uplink: configureCodec (current codec=%d) (new codec=%d)", mCodecInUse, codecType);

  if((mCodecInUse != codecType) )
  {
    // First stop and close current codec
    if(mCodecInUse != CODEC_NONE)
	{
      encoderClose();
	  srcClose();
	} else {
      //New call
      mNbFrameReceived                   = 0;
      mNbInputBufferTrashed              = 0;
      resetRingBuffer();
    }

    switch(codecType)
    {
      case CODEC_AMR_NB:
		{
		  mCodecSampleFreq = FREQ_8KHZ;
		  mCodecIdx        = AMR_IDX;

		  mCscallCodecConfig.amrnbEncParam.bNoHeader     = pCscallCodecInfo->amrnbEncParam.bNoHeader;
		  mCscallCodecConfig.amrnbEncParam.memory_preset = pCscallCodecInfo->amrnbEncParam.memory_preset;

		  OstTraceFiltInst2 (TRACE_ALWAYS, "Cscall/Uplink: send setParamter to AMR-NB codec (bNoHeader=%d) (memory_preset=%d)", mCscallCodecConfig.amrnbEncParam.bNoHeader , mCscallCodecConfig.amrnbEncParam.memory_preset);
		  configure_amr.setParameter(mCscallCodecConfig.amrnbEncParam);

		  mCscallCodecConfig.amrnbEncConfig.nBitRate       = pCscallCodecInfo->amrnbEncConfig.nBitRate;
		  mCscallCodecConfig.amrnbEncConfig.bDtxEnable     = pCscallCodecInfo->amrnbEncConfig.bDtxEnable;
		  mCscallCodecConfig.amrnbEncConfig.ePayloadFormat = pCscallCodecInfo->amrnbEncConfig.ePayloadFormat;
		  mCscallCodecConfig.amrnbEncConfig.bEfr_on        = pCscallCodecInfo->amrnbEncConfig.bEfr_on;

		  OstTraceFiltInst2 (TRACE_ALWAYS, "Cscall/Uplink: send Initial setConfig to AMR-NB codec (nBitRate=%d) (bDtxEnable=%d)", mCscallCodecConfig.amrnbEncConfig.nBitRate , mCscallCodecConfig.amrnbEncConfig.bDtxEnable);
		  OstTraceFiltInst2 (TRACE_ALWAYS, "Cscall/Uplink: send Initial setConfig to AMR-NB codec (ePayloadFormat=%d) (bEfr_on=%d)", mCscallCodecConfig.amrnbEncConfig.ePayloadFormat , mCscallCodecConfig.amrnbEncConfig.bEfr_on);
		  configure_amr.setConfig(mCscallCodecConfig.amrnbEncConfig);

		  break;
		}
      case CODEC_EFR:
		{
		  mCodecSampleFreq = FREQ_8KHZ;
		  mCodecIdx        = EFR_IDX;

		  mCscallCodecConfig.amrnbEncParam.bNoHeader     = pCscallCodecInfo->amrnbEncParam.bNoHeader;
		  mCscallCodecConfig.amrnbEncParam.memory_preset = pCscallCodecInfo->amrnbEncParam.memory_preset;

		  OstTraceFiltInst2 (TRACE_ALWAYS, "Cscall/Uplink: send setParamter to EFR codec (bNoHeader=%d) (memory_preset=%d)", mCscallCodecConfig.amrnbEncParam.bNoHeader , mCscallCodecConfig.amrnbEncParam.memory_preset);
		  configure_efr.setParameter(mCscallCodecConfig.amrnbEncParam);

		  mCscallCodecConfig.amrnbEncConfig.nBitRate       = pCscallCodecInfo->amrnbEncConfig.nBitRate;
		  mCscallCodecConfig.amrnbEncConfig.bDtxEnable     = pCscallCodecInfo->amrnbEncConfig.bDtxEnable;
		  mCscallCodecConfig.amrnbEncConfig.ePayloadFormat = pCscallCodecInfo->amrnbEncConfig.ePayloadFormat;
		  mCscallCodecConfig.amrnbEncConfig.bEfr_on        = pCscallCodecInfo->amrnbEncConfig.bEfr_on;

		  OstTraceFiltInst2 (TRACE_ALWAYS, "Cscall/Uplink: send Initial setConfig to EFR codec (nBitRate=%d) (bDtxEnable=%d)", mCscallCodecConfig.amrnbEncConfig.nBitRate , mCscallCodecConfig.amrnbEncConfig.bDtxEnable);
		  OstTraceFiltInst2 (TRACE_ALWAYS, "Cscall/Uplink: send Initial setConfig to EFR codec (ePayloadFormat=%d) (bEfr_on=%d)", mCscallCodecConfig.amrnbEncConfig.ePayloadFormat , mCscallCodecConfig.amrnbEncConfig.bEfr_on);
		  configure_efr.setConfig(mCscallCodecConfig.amrnbEncConfig);

		  break;
		}
      case CODEC_GSM_FR:
		{
		  mCodecSampleFreq = FREQ_8KHZ;
		  mCodecIdx        = FR_IDX;

		  mCscallCodecConfig.gsmfrEncParam.memory_preset = pCscallCodecInfo->gsmfrEncParam.memory_preset;
		  OstTraceFiltInst1 (TRACE_ALWAYS, "Cscall/Uplink: send setParamter to GSMFR codec (memory_preset=%d)", mCscallCodecConfig.gsmfrEncParam.memory_preset);
		  configure_gsmfr.setParameter(mCscallCodecConfig.gsmfrEncParam);

		  mCscallCodecConfig.gsmfrEncConfig.bDtx            = (pCscallCodecInfo->gsmfrEncConfig.bDtx)? 1:0;
		  mCscallCodecConfig.gsmfrEncConfig.epayload_format = pCscallCodecInfo->gsmfrEncConfig.epayload_format;
		  OstTraceFiltInst2 (TRACE_ALWAYS, "Cscall/Uplink: send setConfig to GSMFR codec (ePayloadFormat=%d) (bDtx=%d)", mCscallCodecConfig.gsmfrEncConfig.epayload_format , mCscallCodecConfig.gsmfrEncConfig.bDtx);
		  configure_gsmfr.setConfig(mCscallCodecConfig.gsmfrEncConfig);

		  break;
		}
      case CODEC_GSM_HR:
		{
		  mCodecSampleFreq = FREQ_8KHZ;
		  mCodecIdx        = HR_IDX;

		  mCscallCodecConfig.gsmhrEncParam.memory_preset = pCscallCodecInfo->gsmhrEncParam.memory_preset;
		  OstTraceFiltInst1 (TRACE_ALWAYS, "Cscall/Uplink: send setParamter to GSMHR codec (memory_preset=%d)", mCscallCodecConfig.gsmhrEncParam.memory_preset);
		  configure_gsmhr.setParameter(mCscallCodecConfig.gsmhrEncParam);

		  mCscallCodecConfig.gsmhrEncConfig.bDtx            = pCscallCodecInfo->gsmhrEncConfig.bDtx;
		  mCscallCodecConfig.gsmhrEncConfig.epayload_format = pCscallCodecInfo->gsmhrEncConfig.epayload_format;
		  OstTraceFiltInst2 (TRACE_ALWAYS, "Cscall/Uplink: send setConfig to GSMHR codec (ePayloadFormat=%d) (bDtx=%d)", mCscallCodecConfig.gsmhrEncConfig.epayload_format , mCscallCodecConfig.gsmhrEncConfig.bDtx);
		  configure_gsmhr.setConfig(mCscallCodecConfig.gsmhrEncConfig);

		  break;
		}
	  case CODEC_AMR_WB:
		{
		  mCodecSampleFreq = FREQ_16KHZ;
		  mCodecIdx        = AMRWB_IDX;

		  mCscallCodecConfig.amrwbEncParam.bNoHeader = pCscallCodecInfo->amrwbEncParam.bNoHeader;
		  OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/Uplink: send setParamter to AMR-WB codec");
		  configure_amrwb.setParameter(mCscallCodecConfig.amrwbEncParam);

		  mCscallCodecConfig.amrwbEncConfig.nBitRate         = pCscallCodecInfo->amrwbEncConfig.nBitRate;
		  mCscallCodecConfig.amrwbEncConfig.bDtxEnable       = pCscallCodecInfo->amrwbEncConfig.bDtxEnable;
		  mCscallCodecConfig.amrwbEncConfig.ePayloadFormat   = pCscallCodecInfo->amrwbEncConfig.ePayloadFormat;
		  OstTraceFiltInst3 (TRACE_ALWAYS, "Cscall/Uplink: send Initial setConfig to AMR-WB codec (nBitRate=%d) (bDtxEnable=%d) (ePayloadFormat=%d)", mCscallCodecConfig.amrwbEncConfig.nBitRate , mCscallCodecConfig.amrwbEncConfig.bDtxEnable, mCscallCodecConfig.amrwbEncConfig.ePayloadFormat);
		  configure_amrwb.setConfig(mCscallCodecConfig.amrwbEncConfig);

		  break;
		}
      case CODEC_PCM8:
		{
		  mCodecSampleFreq = FREQ_8KHZ;
		  mCodecIdx        = PCM_IDX;

		  mCscallCodecConfig.pcmEncConfig.sampling_rate = PCM_SamplingRate_8k;
		  mCscallCodecConfig.pcmEncConfig.big_endian_flag = pCscallCodecInfo->pcmEncConfig.big_endian_flag;
		  OstTraceFiltInst2 (TRACE_ALWAYS, "Cscall/Uplink: send setConfig to PCM codec (sampling_rate=%d) (big_endian=%d)", mCscallCodecConfig.pcmEncConfig.sampling_rate,mCscallCodecConfig.pcmEncConfig.big_endian_flag);
		  configure_pcm.setConfig(mCscallCodecConfig.pcmEncConfig);
		  break;
		}
      case CODEC_PCM16:
		{
		  mCodecSampleFreq = FREQ_16KHZ;
		  mCodecIdx        = PCM_IDX;

		  mCscallCodecConfig.pcmEncConfig.sampling_rate = PCM_SamplingRate_16k;
		  mCscallCodecConfig.pcmEncConfig.big_endian_flag = pCscallCodecInfo->pcmEncConfig.big_endian_flag;
		  OstTraceFiltInst2 (TRACE_ALWAYS, "Cscall/Uplink: send setConfig to PCM codec (sampling_rate=%d) (big_endian=%d)", mCscallCodecConfig.pcmEncConfig.sampling_rate,mCscallCodecConfig.pcmEncConfig.big_endian_flag);
		  configure_pcm.setConfig(mCscallCodecConfig.pcmEncConfig);
		  break;
		}
      case CODEC_NONE:
		{
		  if(mInputSampleFreq != FREQ_UNKNOWN)
		  {
			// port is still enabled, so we may still received buffers
			// we could 1. ignore input buffer until codec is not established
			//          2. store them assuming codec will have the sme sample freq. (data will be resetted if sample differs when codec is established)
			// we chose option 2. as it helps to have "timing independant" tests
			mCodecSampleFreq = mInputSampleFreq;
			updateBufferSize();
		  }
		  else
		  {
			mCodecSampleFreq = FREQ_UNKNOWN;
		  }
          OstTraceFiltInst2 (TRACE_ALWAYS, "Cscall/Uplink: end of call (mNbFrameReceived=%d) (mNbFrameSent=%d)", mNbFrameReceived, mNbFrameSent);
          OstTraceFiltInst3 (TRACE_ALWAYS, "Cscall/Uplink: end of call (mULFakeGeneratedFrameNb=%d) (mNbInputBufferTrashed=%d) (mLostULFrameNb=%d)", mULFakeGeneratedFrameNb, mNbInputBufferTrashed,mLostULFrameNb);
          mULFakeGeneratedFrameNb = 0;
          mNbFrameReceived      = 0;
          mNbFrameSent          = 0;
          mNbInputBufferTrashed = 0;
          mMissingULFrameNb     = 0;
          mLostULFrameNb        = 0;
          mOutBufferReset       = false;
		  mCodecIdx             = AMR_IDX;

		  OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/Uplink: No more codec in use");
		  break;
		}
      default:
        ARMNMF_DBC_ASSERT(0);
    }

    // Apply New Codec Config
    mCodecInUse = codecType;

    // Open the codec:
    if(codecType != CODEC_NONE)
    {
      encoderOpen();
	  srcOpen();
    }
  }
  else
  {
    // reconfigure codec
    switch(mCodecInUse)
    {
      case CODEC_AMR_NB:
      case CODEC_EFR:
		{
		  mCscallCodecConfig.amrnbEncConfig.nBitRate         = pCscallCodecInfo->amrnbEncConfig.nBitRate;
		  mCscallCodecConfig.amrnbEncConfig.bDtxEnable       = pCscallCodecInfo->amrnbEncConfig.bDtxEnable;
		  mCscallCodecConfig.amrnbEncConfig.ePayloadFormat   = pCscallCodecInfo->amrnbEncConfig.ePayloadFormat;
		  mCscallCodecConfig.amrnbEncConfig.bEfr_on          = pCscallCodecInfo->amrnbEncConfig.bEfr_on;

		  OstTraceFiltInst2 (TRACE_ALWAYS, "Cscall/Uplink: Reconfiguration .. send setConfig to AMR-NB codec (nBitRate=%d) (bDtxEnable=%d)", mCscallCodecConfig.amrnbEncConfig.nBitRate , mCscallCodecConfig.amrnbEncConfig.bDtxEnable);
		  OstTraceFiltInst2 (TRACE_ALWAYS, "Cscall/Uplink: Reconfiguration .. send setConfig to AMR-NB codec (ePayloadFormat=%d) (bEfr_on=%d)", mCscallCodecConfig.amrnbEncConfig.ePayloadFormat  , mCscallCodecConfig.amrnbEncConfig.bEfr_on  );
		  configure_amr.setConfig(mCscallCodecConfig.amrnbEncConfig);

		  break;
		}
      case CODEC_GSM_FR:
		{
		  mCscallCodecConfig.gsmfrEncConfig.bDtx                      = pCscallCodecInfo->gsmfrEncConfig.bDtx;
		  mCscallCodecConfig.gsmfrEncConfig.epayload_format           = pCscallCodecInfo->gsmfrEncConfig.epayload_format;

		  OstTraceFiltInst2 (TRACE_ALWAYS, "Cscall/Uplink: Reconfiguration .. send setConfig to GSMFR codec (ePayloadFormat=%d) (bDtx=%d)", mCscallCodecConfig.gsmfrEncConfig.epayload_format , mCscallCodecConfig.gsmfrEncConfig.bDtx);
		  configure_gsmfr.setConfig(mCscallCodecConfig.gsmfrEncConfig);

		  break;
		}
      case CODEC_GSM_HR:
		{
		  mCscallCodecConfig.gsmhrEncConfig.bDtx    = pCscallCodecInfo->gsmhrEncConfig.bDtx;
		  mCscallCodecConfig.gsmhrEncConfig.epayload_format           = pCscallCodecInfo->gsmhrEncConfig.epayload_format;

		  OstTraceFiltInst2 (TRACE_ALWAYS, "Cscall/Uplink: Reconfiguration .. send setConfig to GSMHR codec (ePayloadFormat=%d) (bDtx=%d)", mCscallCodecConfig.gsmhrEncConfig.epayload_format , mCscallCodecConfig.gsmhrEncConfig.bDtx);
		  configure_gsmhr.setConfig(mCscallCodecConfig.gsmhrEncConfig);

		  break;
		}
      case CODEC_AMR_WB:
        {
          mCscallCodecConfig.amrwbEncConfig.nBitRate         = pCscallCodecInfo->amrwbEncConfig.nBitRate;
          mCscallCodecConfig.amrwbEncConfig.bDtxEnable       = pCscallCodecInfo->amrwbEncConfig.bDtxEnable;
          mCscallCodecConfig.amrwbEncConfig.ePayloadFormat   = pCscallCodecInfo->amrwbEncConfig.ePayloadFormat;

		  OstTraceFiltInst3 (TRACE_ALWAYS, "Cscall/Uplink: Reconfiguration .. send setConfig to AMR-WB codec (nBitRate=%d) (bDtxEnable=%d) (ePayloadFormat=%d)", mCscallCodecConfig.amrwbEncConfig.nBitRate , mCscallCodecConfig.amrwbEncConfig.bDtxEnable, mCscallCodecConfig.amrwbEncConfig.ePayloadFormat);
          configure_amrwb.setConfig(mCscallCodecConfig.amrwbEncConfig);
        }
        break;
      case CODEC_PCM8:
      case CODEC_PCM16:
		{
		  mCscallCodecConfig.pcmEncConfig.sampling_rate = pCscallCodecInfo->pcmEncConfig.sampling_rate;

		  OstTraceFiltInst1 (TRACE_ALWAYS, "Cscall/Uplink: Reconfiguration .. send setConfig to PCM codec (sampling_rate=%d)", mCscallCodecConfig.pcmEncConfig.sampling_rate);
		  configure_pcm.setConfig(mCscallCodecConfig.pcmEncConfig);
		  break;
		}
      default:
        ARMNMF_DBC_ASSERT(0);
    }
  }
  OstTraceFiltInst1 (TRACE_ALWAYS, "Cscall/Uplink: inform protocohanlder that new codec config has been applied (codecType=%d)",codecType);
  codecApplied.newCodecApplied();
}



void METH(signal)()
{
    // It's time to send a new frame on uplink
    OstTraceFiltInst0 (TRACE_DEBUG, "Cscall/Uplink: interrupt: time to encode a new frame and send it to modem");

	if(!mCodecInitialized)
	{
      mLostULFrameNb++;
	  OstTraceFiltInst1 (TRACE_WARNING, "Cscall/Uplink: Codec not yet initialized when Input Port buffer is triggered, on buffer is trashed on input port (mLostULFrameNb=%d) ",mLostULFrameNb);
	  return;
	}
    if(mPorts[OUTPUT_PORT].queuedBufferCount() == 0)
    {
      mLostULFrameNb++;
	  OstTraceFiltInst1 (TRACE_ERROR, "Cscall/Uplink: ERROR!! No buffer Available on OUTPUT Port !!!!! (mLostULFrameNb=%d) ",mLostULFrameNb);
	  return;
    }

	// Encode frame
	OMX_BUFFERHEADERTYPE * bufOut = mPorts[OUTPUT_PORT].dequeueBuffer();
	setupEncoderBuffer();
	encodeFrame(bufOut);

	// return buffer
	returnBufferAsync(OUTPUT_PORT, bufOut);
	OstTraceFiltInst2 (TRACE_DEBUG, "Cscall/Uplink: UL-frame sent latency control (Uplink, output) (timestamp = 0x%x 0x%x us)", (bufOut->nTimeStamp)>>32,(bufOut->nTimeStamp)&0xffffffffu);
    if (mPorts[INPUT_PORT].queuedBufferCount()) scheduleProcessEvent(); // only possible in case of !mRealTimeInput

    mNbFrameSent++;
    OstTraceFiltInst4 (TRACE_DEBUG, "Cscall/Uplink: Frame sent (mNbFrameReceived=%d) (mNbFrameSent=%d)  (mULFakeGeneratedFrameNb=%d) (mNbInputBufferTrashed=%d)", mNbFrameReceived, mNbFrameSent,mULFakeGeneratedFrameNb, mNbInputBufferTrashed);

	// check timing for real time input
	if(mRealTimeInput && mLastReceivedBufferTime[mReadBufferIdx])
	{
	  unsigned long long wait_time;
	  wait_time = getTime() - mLastReceivedBufferTime[mReadBufferIdx] - (unsigned long long)(20000 * (NB_BUFFER - mNbFreeBuffer - 1));
      mLastReceivedBufferTime[mReadBufferIdx] = 0;
	  OstTraceFiltInst1 (TRACE_DEBUG, "Cscall/Uplink: Time waited (%d ms)", (int)(wait_time/1000));
      if (wait_time < MAX_VALID_WAIT_TIME_US) {
          if (wait_time > mMaxWaitTime.threshold)
          {
              mMaxTimeReached++;
              mSumWaitTime += (t_uint32)wait_time;
              reconfigureSource();
          }
          else
          {
              mMaxTimeReached = 0;
              mSumWaitTime = 0;
          }
      }
    }

    if(mNbFreeBuffer != NB_BUFFER) {
	    mReadBufferIdx          = (mReadBufferIdx + 1) % NB_BUFFER;
        mNbFreeBuffer ++;
    }
}


void METH(setParameter)(t_bool real_time_input)
{
  mRealTimeInput = (bool)real_time_input;
}

void METH(setSampleFreq)(t_sample_freq sample_freq)
{
  mInputSampleFreq = sample_freq;
  srcClose();
  srcOpen();

  if(mCodecSampleFreq == FREQ_UNKNOWN)
  {
	// If codec is not yet selected when enabling port, assume codec sample freq
	// will be the same as input sample freq (in listening mode, codec will always be selected before enabling port).
	// It will be updated if needed in configureCodec, but it helps for automatics tests
	mCodecSampleFreq = mInputSampleFreq;
	updateBufferSize();
  }
}

void METH(setMaxWaitTime)(UlMaxWaitTime_t maxWaitTime)
{
  mMaxWaitTime = maxWaitTime;
  mMaxTimeReached = 0;
}

////////////////////////////////////////////////////////////
// Encoder methods
///////////////////////////////////////////////////////////
void cscall_nmf_host_uplink::encoderOpen()
{
  if(mCodecInitialized)
  {
    OstTraceFiltInst0 (TRACE_ERROR, "Cscall/Uplink: call encoderOpen() interface but Codec already Initialized !!!!!!!!");
    return;
  }

  OstTraceFiltInst1 (TRACE_ALWAYS, "Cscall/Uplink: call encoderOpen(), mCodecIdx = %d", mCodecIdx);

  // clean codec interface
  memset(&mInterfaceFE, 0, sizeof(CODEC_INTERFACE_T));

  // open encoder
  encoder[mCodecIdx].open(&mInterfaceFE);
  ARMNMF_DBC_POSTCONDITION(mInterfaceFE.codec_local_struct != 0);
  ARMNMF_DBC_POSTCONDITION(mInterfaceFE.codec_config_local_struct != 0);

  // reset encoder
  mInterfaceFE.codec_state.mode                   = UNKNOWN;
  mInterfaceFE.codec_state.output_enable          = false;
  mInterfaceFE.codec_state.bfi                    = false;
#ifndef NO_DTX_FLAG
  mInterfaceFE.codec_state.dtx                    = false;
#endif
  mInterfaceFE.codec_state.first_time             = true;
  mInterfaceFE.sample_struct.samples_interleaved  = true;


  encoder[mCodecIdx].reset(&mInterfaceFE);
  updateBufferSize();

  mCodecInitialized = true;
  return;
}

void cscall_nmf_host_uplink::encoderClose()
{

  if(!mCodecInitialized)
  {
    OstTraceFiltInst0 (TRACE_ERROR, "Cscall/Uplink: call encoderClose() interface but no Codec Initialized !!!!!!!!");
    return;
  }

  OstTraceFiltInst1 (TRACE_ALWAYS, "Cscall/Uplink: call encoderClose(), mCodecIdx = %d", mCodecIdx);

  encoder[mCodecIdx].close(&mInterfaceFE);

  mCodecInitialized   = false;
  return;
}


void cscall_nmf_host_uplink::encodeFrame(OMX_BUFFERHEADERTYPE * bufOut)
{
  t_uint16    framesize_bits;
  t_uint16    framesize_bytes;
  int         status;

  OstTraceFiltInst0 (TRACE_DEBUG, "Cscall/Uplink: Start encoding");

  // check that the buffer is a 32-bit multiple (since bs_init is 32bits)
  ARMNMF_DBC_ASSERT_MSG((bufOut->nAllocLen % BS_WORD_SIZE_IN_BYTES) == 0, "Buffer size not compliant with bs_init\n");

  bs_init(&mInterfaceFE.stream_struct.bits_struct,
          (unsigned int*)bufOut->pBuffer, (bufOut->nAllocLen/BS_WORD_SIZE_IN_BYTES) + 1, BS_INIT_SIZE_IN_BITS);

  mInterfaceFE.sample_struct.buf_add = (Float *)mEncoderBuffer.pBuffer;

  if (mEncoderBuffer.nFilledLen == 0 && mEncoderBuffer.nFlags & OMX_BUFFERFLAG_EOS)
  {
    OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/Uplink: EOS received");

    bufOut->nFilledLen = 0;
    bufOut->nFlags     = OMX_BUFFERFLAG_EOS;
    bufOut->nTimeStamp = mEncoderBuffer.nTimeStamp;

    OstTraceFiltInst2 (TRACE_DEBUG, "Cscall/Uplink: UL-frame sent (EOS) latency control (encoder_wrapper, output) (timestamp = 0x%x 0x%x us)", (bufOut->nTimeStamp)>>32,(bufOut->nTimeStamp)&0xffffffffu);
    proxy.eventHandler(OMX_EventBufferFlag, OUTPUT_PORT, bufOut->nFlags);
  }
  else
  {
	// Dump input buffer
	{
	  OMX_U8 * ptr  =  (OMX_U8 *)(mEncoderBuffer.pBuffer + mEncoderBuffer.nOffset);
	  OMX_U32 size  = mEncoderBuffer.nFilledLen;
      for (OMX_U32 i = 0; i < size; i+=40)
	  {
		OstTraceFiltInstData(TRACE_CSCALL_UL_PCM_DATA, "Cscall/Uplink: inputBuffer[]  = %{int16[]}", (OMX_U8 *)(ptr + i), (size-i > 40) ? 40 : size-i);
	  }

#ifdef MMPROBE_ENABLED
	  mmprobe_probe(MM_PROBE_TX_SPE_INPUT, mEncoderBuffer.pBuffer, mEncoderBuffer.nFilledLen);
#endif
	}

	// encode Frame
	status = encoder[mCodecIdx].encodeFrame(&mInterfaceFE);

	if (status != RETURN_STATUS_OK)
	{
	  OstTraceFiltInst1 (TRACE_ERROR, "Cscall/Uplink: Error while encoding (status=%d)", status);
	  ARMNMF_DBC_ASSERT_MSG(status < RETURN_STATUS_ERROR, "Cscall/Uplink: encoder return error state");
	}

	framesize_bits = mInterfaceFE.stream_struct.real_size_frame_in_bit;
	ARMNMF_DBC_POSTCONDITION((framesize_bits % 8) == 0);

	framesize_bytes    = framesize_bits/8;
	bufOut->nFilledLen = framesize_bytes;
	bufOut->nTimeStamp = mEncoderBuffer.nTimeStamp;

	if (mEncoderBuffer.nFlags & OMX_BUFFERFLAG_EOS)
	{
	  OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/Uplink: EOS received");
	  bufOut->nFlags = OMX_BUFFERFLAG_EOS;
	  proxy.eventHandler(OMX_EventBufferFlag, OUTPUT_PORT, bufOut->nFlags);
	}

	// Dump output buffer
	{
	  OMX_U8 * ptr  =  (OMX_U8 *)(bufOut->pBuffer + bufOut->nOffset);
	  OMX_U32 size = bufOut->nFilledLen + (bufOut->nFilledLen % 4);
	  OstTraceFiltInst1 (TRACE_CSCALL_UL_BITSTREAM_DATA, "Cscall/Uplink: size of the following outputBuffer is %d bytes",bufOut->nFilledLen);
	  for (OMX_U32 i = 0; i < size; i+=40){
		OstTraceFiltInstData(TRACE_CSCALL_UL_BITSTREAM_DATA, "Cscall/Uplink: outputBuffer[]  = %{int16[]}", (OMX_U8 *)(ptr + i), (size-i > 40) ? 40 : size-i);
	  }
	}

	OstTraceFiltInst2 (TRACE_DEBUG, "Cscall/Uplink: UL-frame sent latency control (encoder_wrapper, output) (timestamp = 0x%x 0x%x us)", (bufOut->nTimeStamp)>>32,(bufOut->nTimeStamp)&0xffffffffu);
  }

  OstTraceFiltInst0 (TRACE_DEBUG, "Cscall/Uplink: End of encoding");
}


void cscall_nmf_host_uplink::setupEncoderBuffer(void)
{
    if(mNbFreeBuffer == NB_BUFFER)
    {
        // the ring buffer does not contain any frame -> fake frame is generated!
        mULFakeGeneratedFrameNb++;
        mMissingULFrameNb++;

        if (mNbFrameReceived) {
            OstTraceFiltInst4 (TRACE_WARNING,"Cscall/Uplink: WARNING!! fake frame generated: (mULFakeGeneratedFrameNb=%d) (mMissingULFrameNb=%d) (mNbFrameReceived=%d) (mNbFrameSent=%d)", mULFakeGeneratedFrameNb,mMissingULFrameNb,mNbFrameReceived,mNbFrameSent);
        }

        if (!mOutBufferReset && mMissingULFrameNb >=MISSING_UL_FRAME_THRESHOLD)
        {
            OstTraceFiltInst1(TRACE_WARNING,"Cscall/uplink: WARNING!! missing too many UL frames, reset ring buffer: (mMissingULFrameNb=%d)",mMissingULFrameNb);
            resetRingBuffer();
            mOutBufferReset = true;
        }
    }

  // when there is only one slot per buffer, no need to reorder it :)
  mEncoderBuffer.pBuffer = (OMX_U8 *)(mRingBuffer + mReadBufferIdx * mBufferSampleSize);

  // update rest of the header
  mEncoderBuffer.nOffset    = 0;
  mEncoderBuffer.nAllocLen  = mBufferSampleSize * 2;
  mEncoderBuffer.nFilledLen = mBufferSampleSize * 2;
  mEncoderBuffer.nTimeStamp = mBufferTimeStamps[mReadBufferIdx];
  mEncoderBuffer.nFlags     = 0;
  mEncoderBuffer.nFlags |= mBufferFlags[mReadBufferIdx];
  mBufferFlags[mReadBufferIdx]      = 0;
  mBufferTimeStamps[mReadBufferIdx] = 0;
}


void cscall_nmf_host_uplink::updateBufferSize(void)
{
  if(mCodecSampleFreq == FREQ_8KHZ)
  {
	if(mBufferSampleSize != (BUFFER_MS_SIZE * 8))
	{
	  resetRingBuffer();
	}
	mBufferSampleSize = (BUFFER_MS_SIZE * 8);
  }
  else if(mCodecSampleFreq == FREQ_16KHZ)
  {
	if(mBufferSampleSize != (BUFFER_MS_SIZE * 16))
	{
	  resetRingBuffer();
	}
	mBufferSampleSize = (BUFFER_MS_SIZE * 16);
  }
}

//////////////////////////////////////////////
//  SRC specific
/////////////////////////////////////////////
void cscall_nmf_host_uplink::srcOpen(void)
{
  if((mInputSampleFreq != FREQ_UNKNOWN)     &&
	 (mCodecInitialized)                    &&
	 (mInputSampleFreq != mCodecSampleFreq))
  {
	t_host_effect_config config;
	int status, heap_size, size;

	config.block_size				  = BUFFER_MS_SIZE; //this is the size in ms for src in mode 6
	config.infmt.freq				  = mInputSampleFreq;
	config.infmt.nof_channels		  = 1;
	config.infmt.nof_bits_per_sample  = 16;
	config.infmt.headroom			  = 0;
	config.infmt.interleaved		  = 1;
	config.outfmt.freq				  = mCodecSampleFreq;
	config.outfmt.nof_channels		  = 1;
	config.outfmt.nof_bits_per_sample = 16;
	config.outfmt.headroom			  = 0;
	config.outfmt.interleaved		  = 1;

	if(getTraceInfoPtr())
	  OstTraceFiltInst3 (TRACE_ALWAYS, "Cscall/Uplink: openSRC() open and configure SRC effect (config.block_size=%d) (input_freq=%d) (output_freq=%d)",config.block_size,config.infmt.freq,config.outfmt.freq);

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


void cscall_nmf_host_uplink::srcClose(void)
{
  if(mSRCInitialized)
  {
	if(getTraceInfoPtr())
	  OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/Uplink: closeSRC() close SRC effect ");
	// close lib
	free(mHeap);
	mSRCInitialized = false;
  }
}


void cscall_nmf_host_uplink::convertSampleRate(OMX_BUFFERHEADERTYPE *bufIn)
{
  OMX_BUFFERHEADERTYPE bufOut;
  int consumed = 0, produced = 0, flush = 0, flush_in = 0, blocksize = BUFFER_MS_SIZE,
      nbchannel = 1, bytePerSample = (16 / 8), *inputbuffer, *outputBuffer;

  bufOut.nAllocLen  = mBufferSampleSize * 2; //size in bytes
  bufOut.nFilledLen = 0;
  bufOut.nOffset    = 0;
  bufOut.pBuffer    = (OMX_U8 *)(mRingBuffer + mWriteBufferIdx * mBufferSampleSize);

  OstTraceFiltInst1 (TRACE_DEBUG, "Cscall/Uplink: convertSampleRate() start processing (bufIn->nFilledLen =%d)",bufIn->nFilledLen);
  if(bufIn->nFilledLen)
  {
	inputbuffer     = (int *)((int)bufIn->pBuffer  + bufIn->nOffset);
	outputBuffer    = (int *)((int)bufOut.pBuffer + bufOut.nOffset);

	consumed=blocksize;
	produced=0;

	flush       = (bufIn->nFlags & OMX_BUFFERFLAG_EOS) ? true : false;
	flush_in    = flush;


	if((*mResampleContext.processing) (
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

	if (flush) {
	   bufOut.nFlags |= OMX_BUFFERFLAG_EOS;
	}
	bufOut.nFilledLen = produced *nbchannel*bytePerSample;
  }
  else
  {
	ARMNMF_DBC_ASSERT(bufIn->nFlags & OMX_BUFFERFLAG_EOS);
	bufOut.nFilledLen = 0;
  }
  OstTraceFiltInst1 (TRACE_DEBUG, "Cscall/Uplink: convertSampleRate() end processing   (bufOut->nFilledLen =%d)",bufOut.nFilledLen);

  // complete slot with 0 if input buffer not full
  if(bufOut.nFilledLen < (unsigned)(mBufferSampleSize * 2))
  {
	ARMNMF_DBC_ASSERT(bufIn->nFlags & OMX_BUFFERFLAG_EOS);
	t_uint16   nb_sample_to_complete = mBufferSampleSize - (bufOut.nFilledLen / 2);
	t_sint16 * dst_ptr               = mRingBuffer + mWriteBufferIdx * mBufferSampleSize + (mBufferSampleSize - nb_sample_to_complete);

	memset((void *)dst_ptr, 0, (nb_sample_to_complete * 2));
  }

  mBufferFlags[mWriteBufferIdx]      = bufIn->nFlags;
  mBufferTimeStamps[mWriteBufferIdx] = bufIn->nTimeStamp;
}


void cscall_nmf_host_uplink::copyBuffer(OMX_BUFFERHEADERTYPE *bufIn)
{
  t_sint16 *dst_ptr = mRingBuffer + mWriteBufferIdx * mBufferSampleSize;

  ARMNMF_DBC_ASSERT(bufIn->nFilledLen <= (unsigned)(mBufferSampleSize * 2));
  memcpy((void *)dst_ptr, (void *)bufIn->pBuffer, bufIn->nFilledLen);

  // complete slot with 0 if input buffer not full
  if(bufIn->nFilledLen < (unsigned)(mBufferSampleSize * 2))
  {
	ARMNMF_DBC_ASSERT(bufIn->nFlags & OMX_BUFFERFLAG_EOS);
	t_uint16 nb_sample_to_complete = mBufferSampleSize - (bufIn->nFilledLen / 2);

	dst_ptr += mBufferSampleSize - nb_sample_to_complete;
	memset((void *)dst_ptr, 0, (nb_sample_to_complete * 2));
  }

  mBufferFlags[mWriteBufferIdx]      = bufIn->nFlags;
  mBufferTimeStamps[mWriteBufferIdx] = bufIn->nTimeStamp;
}


/////////////////////////////////////////////////////
//  Timing and Margins
////////////////////////////////////////////////////
void cscall_nmf_host_uplink::reconfigureSource(t_uint32 drop_time)
{
  OstTraceFiltInst2 (TRACE_DEBUG, "Cscall/Uplink: mMaxTimeReached=%d, mSumWaitTime=%d", mMaxTimeReached, (int)mSumWaitTime);
  if (!drop_time && (mMaxTimeReached >= mMaxWaitTime.waitBeforeReconfigure))
  {
	drop_time = ((mSumWaitTime/mMaxTimeReached) - mMaxWaitTime.targetWaitTime);
  }
  if (drop_time)
  {
	OstTraceFiltInst1 (TRACE_DEBUG, "Cscall/Uplink: Source must be reconfigured, drop %d us", (int)drop_time);
	dropData.drop(drop_time);

	mMaxTimeReached = 0;
	mSumWaitTime = 0;
  }
}


void cscall_nmf_host_uplink::resetRingBuffer()
{
  if(getTraceInfoPtr()) {
    OstTraceFiltInst0 (TRACE_WARNING, "Cscall/Uplink: resetRingBuffer() called reset/zeros ring buffer");
  }
  mRingBuffer = &mRingnExtraBuffer[0];
  memset((void *)mRingBuffer, 0, NB_BUFFER * MAX_BUFFER_SAMPLE_SIZE * 2);
  for(int i=0; i<NB_BUFFER; i++)
  {
    mLastReceivedBufferTime[i] = 0;
  }
  mReadBufferIdx            = 0;
  mWriteBufferIdx           = 0;
  mNbFreeBuffer             = NB_BUFFER;
}
