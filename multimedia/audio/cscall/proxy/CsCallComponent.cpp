/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   CsCallComponent.cpp
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "CsCallComponent.h"
#include "CsCallNmfHost_ProcessingComp.h"
#include "CsCall_PcmPort.h"
#include "CsCall_ControlPort.h"
#include "CsCall_defines.h"
#include "cscall_config.h"

#include "AFM_Index.h"
#include "AFM_Types.h"
#include "audio_chipset_api_index.h"
#include "AFM_Symbian_AudioExt.h"
#include "OMX_IndexExt.h"
#include "OMX_CoreExt.h"

#include "OMX_Symbian_ExtensionNames_Ste.h"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cscall_proxy_CsCallComponentTraces.h"
#endif

#define clearMem(VAR) {char *ptr=(char*)(&VAR);for(unsigned int i=0;i<sizeof(VAR);i++) *(ptr++)=0;}

CsCallComponent::CsCallComponent():AFM_Component() {
  clearMem(mAudioConfigSpeechLoops);
  clearMem(mAudioSpeechTimingReq);
  clearMem(mAudioSpeechCodecReq);

  mAudioCodecEnabled_cb    = OMX_FALSE;
  mAudioCodecDisabled_cb   = OMX_FALSE;
  mAudioSampleRate16khz_cb = OMX_TRUE;
  mAudioSampleRate8khz_cb  = OMX_TRUE;

  //default settings for AFM_AUDIO_CONFIG_CSCALL_UPLINK_TIMING_REQ
  mAudioSpeechTimingReq.nSize                = sizeof(AFM_AUDIO_CONFIG_CSCALL_UPLINK_TIMING_REQ);
  getOmxIlSpecVersion(&mAudioSpeechTimingReq.nVersion);
  mAudioSpeechTimingReq.nDeliveryTime        = 30000; //30 ms
  mAudioSpeechTimingReq.nModemProcessingTime = 0;
  mAudioSpeechTimingReq.nCause               = 0;

  //default settings for AFM_AUDIO_CONFIG_CSCALL_TIMING_REQ_MARGINS
  mAudioTimingReqMargins.nSize    = sizeof(AFM_AUDIO_CONFIG_CSCALL_TIMING_REQ_MARGINS);
  getOmxIlSpecVersion(&mAudioTimingReqMargins.nVersion);
  mAudioTimingReqMargins.nbMargin = 10000;
  mAudioTimingReqMargins.wbMargin = 10000;

  //default settings for AFM_AUDIO_CONFIG_CSCALL_DATA_WAIT_TIME
  mAudioDataWaitTime.nSize = sizeof(AFM_AUDIO_CONFIG_CSCALL_DATA_WAIT_TIME);
  getOmxIlSpecVersion(&(mAudioDataWaitTime.nVersion));
  mAudioDataWaitTime.threshold             = 30000;
  mAudioDataWaitTime.targetWaitTime        = 3000;
  mAudioDataWaitTime.waitBeforeReconfigure = 5;

  //default settings for AFM_AUDIO_CONFIG_CSCALL_SPEECH_CODEC_REQ
  mAudioSpeechCodecReq.nSize                       = sizeof(AFM_AUDIO_CONFIG_CSCALL_SPEECH_CODEC_REQ);
  getOmxIlSpecVersion(&mAudioSpeechCodecReq.nVersion);
  mAudioSpeechCodecReq.nNetworkType                = AFM_AUDIO_NETWORK_GSM;
  mAudioSpeechCodecReq.nCodingType                 = OMX_AUDIO_CodingAMR;
  mAudioSpeechCodecReq.encoderconfig.bAAC          = OMX_FALSE;
  mAudioSpeechCodecReq.encoderconfig.bNsynch       = OMX_FALSE;
  mAudioSpeechCodecReq.encoderconfig.nSFN          = 0;
  mAudioSpeechCodecReq.encoderconfig.nBandModeType = OMX_AUDIO_AMRBandModeNB3;
  mAudioSpeechCodecReq.encoderconfig.bDTX          = OMX_TRUE;
  mAudioSpeechCodecReq.encoderconfig.bNoData       = OMX_TRUE;
  mAudioSpeechCodecReq.encoderconfig.bNSCB         = OMX_TRUE;
  mAudioSpeechCodecReq.nBandModeTypeDecoder        = OMX_AUDIO_AMRBandModeNB3;
  mAudioSpeechCodecReq.codecInitialization         = OMX_FALSE;

  //default settings for AFM_AUDIO_CONFIG_CSCALL_VOICE_CALL_STATUS
  mAudioVoiceCallStatus.nSize      = sizeof(AFM_AUDIO_CONFIG_CSCALL_VOICE_CALL_STATUS);
  getOmxIlSpecVersion(&mAudioVoiceCallStatus.nVersion);
  mAudioVoiceCallStatus.bConnected = OMX_TRUE;

  //default settings for OMX_SYMBIAN_AUDIO_CONFIG_SPEECHLOOPSTYPE
  mAudioConfigSpeechLoops.nSize                  = sizeof(OMX_SYMBIAN_AUDIO_CONFIG_SPEECHLOOPSTYPE);
  getOmxIlSpecVersion(&mAudioConfigSpeechLoops.nVersion);
  mAudioConfigSpeechLoops.bLoopUplinkDownlink    = OMX_FALSE;
  mAudioConfigSpeechLoops.bLoopUplinkDownlinkAlg = OMX_FALSE;
  mAudioConfigSpeechLoops.bLoopDownlinkUplink    = OMX_FALSE;
  mAudioConfigSpeechLoops.bLoopDownlinkUplinkAlg = OMX_FALSE;

  //default settings for AFM_AUDIO_CONFIG_MODEM_LOOPBACK
  mAudioModemLoopBackConfig.nSize            = sizeof(AFM_AUDIO_CONFIG_MODEM_LOOPBACK);
  getOmxIlSpecVersion(&mAudioModemLoopBackConfig.nVersion);
  mAudioModemLoopBackConfig.nLoopBackMode    = AFM_MODEM_LOOP_OFF;
  mAudioModemLoopBackConfig.nSpeechCodec     = OMX_AUDIO_CodingAMR;
  mAudioModemLoopBackConfig.nAMRrate         = OMX_AUDIO_AMRBandModeNB0;

  //default settings for AFM_PARAM_CSCALLMODETYPE
  mCscallModeType.nSize=sizeof(AFM_PARAM_CSCALLMODETYPE);
  getOmxIlSpecVersion(&(mCscallModeType.nVersion));
  mCscallModeType.nULRealTimeMode = OMX_TRUE;
  mCscallModeType.nDLRealTimeMode = OMX_TRUE;

#if defined(MAI_PROTOCOL)
  mCscallModeType.eProtocol = AFM_AUDIO_PROTOCOL_MAI;
#elif defined(ISI_TD_PROTOCOL)
  mCscallModeType.eProtocol = AFM_AUDIO_PROTOCOL_ISI_TD;
#else
  mCscallModeType.eProtocol = AFM_AUDIO_PROTOCOL_ISI;
#endif

  //default settings for OMX_SYMBIAN_AUDIO_CONFIG_DECODERSTATUSTYPE
  mDecoderStatus.nSize = sizeof(OMX_SYMBIAN_AUDIO_CONFIG_DECODERSTATUSTYPE);
  getOmxIlSpecVersion(&(mDecoderStatus.nVersion));
  mDecoderStatus.nCoding = OMX_AUDIO_CodingUnused;
  mDecoderStatus.nBitrate = 0;

  //default settings for OMX_SYMBIAN_AUDIO_CONFIG_ENCODERSTATUSTYPE
  mEncoderStatus.nSize = sizeof(OMX_SYMBIAN_AUDIO_CONFIG_ENCODERSTATUSTYPE);
  getOmxIlSpecVersion(&(mEncoderStatus.nVersion));
  mEncoderStatus.nCoding = OMX_AUDIO_CodingUnused;
  mEncoderStatus.nBitrate = 0;
  mEncoderStatus.bDtx = OMX_FALSE;
  mEncoderStatus.bAudioActivityControl = OMX_FALSE;
  mEncoderStatus.bNsync = OMX_FALSE;

  //default settings for OMX_SYMBIAN_AUDIO_CONFIG_UPLINKTIMINGTYPE
  mUplinkTiming.nSize = sizeof(OMX_SYMBIAN_AUDIO_CONFIG_UPLINKTIMINGTYPE);
  getOmxIlSpecVersion(&(mUplinkTiming.nVersion));
  mUplinkTiming.nDeliveryTime        = 0x80000000;
  mUplinkTiming.nModemProcessingTime = 0x80000000;

  //default settings for OMX_SYMBIAN_AUDIO_CONFIG_UPLINKTIMINGINFOTYPE
  mUplinkTimingInfo.nSize = sizeof(OMX_SYMBIAN_AUDIO_CONFIG_UPLINKTIMINGINFOTYPE);
  getOmxIlSpecVersion(&(mUplinkTimingInfo.nVersion));
  mUplinkTimingInfo.nMaxTime     = 0x80000000;
  mUplinkTimingInfo.nAverageTime = 0x80000000;
  mUplinkTimingInfo.nMinTime     = 0x80000000;

  //default settings for OMX_SYMBIAN_AUDIO_CONFIG_UPLINKDELAYINFOTYPE
  mUplinkDelayInfo.nSize = sizeof(OMX_SYMBIAN_AUDIO_CONFIG_UPLINKDELAYINFOTYPE);
  getOmxIlSpecVersion(&(mUplinkDelayInfo.nVersion));
  mUplinkDelayInfo.nMaxDelay     = 0x80000000;
  mUplinkDelayInfo.nAverageDelay = 0x80000000;
  mUplinkDelayInfo.nMinDelay     = 0x80000000;
}


OMX_VERSIONTYPE CsCallComponent::getVersion(void) const
{
  OMX_VERSIONTYPE version;

  version.s.nVersionMajor = CSCALL_MAJOR;
  version.s.nVersionMinor = CSCALL_MINOR;
  version.s.nRevision     = CSCALL_REVISION;
  version.s.nStep         = 0;

  return version;
}

OMX_ERRORTYPE CsCallComponent::construct()
{
  OMX_ERRORTYPE error;

  error = AFM_Component::construct(3, 1, true);
  if (error != OMX_ErrorNone) return error;

  OMX_AUDIO_PARAM_PCMMODETYPE defaultPcmSettings;

  defaultPcmSettings.eNumData       = OMX_NumericalDataSigned;
  defaultPcmSettings.eEndian        = OMX_EndianLittle;
  defaultPcmSettings.bInterleaved   = OMX_TRUE;
  defaultPcmSettings.ePCMMode       = OMX_AUDIO_PCMModeLinear;
  defaultPcmSettings.nBitPerSample  = 16;

  defaultPcmSettings.nChannels      = 1;
  defaultPcmSettings.nSamplingRate  = CSCALL_DEFAULT_SAMPLING_RATE;

  defaultPcmSettings.eChannelMapping[0] = OMX_AUDIO_ChannelCF;
  defaultPcmSettings.nPortIndex     = CSCALL_INPUT_PORT_INDEX;

  // Role
  ENS_String<25> role = "CS_sink_source";
  role.put((OMX_STRING)mRoles[0]);
  setActiveRole(mRoles[0]);

  error = createCsCallPcmPort(CSCALL_INPUT_PORT_INDEX, OMX_DirInput, defaultPcmSettings);
  if (error != OMX_ErrorNone) return error;

  defaultPcmSettings.nPortIndex = CSCALL_OUTPUT_PORT_INDEX;

  error = createCsCallPcmPort(CSCALL_OUTPUT_PORT_INDEX, OMX_DirOutput, defaultPcmSettings);
  if (error != OMX_ErrorNone) return error;

  error = createCsCallControlPort(CSCALL_CONTROL_PORT_INDEX, OMX_DirOutput);
  if (error != OMX_ErrorNone) return error;

  return OMX_ErrorNone;
}

OMX_ERRORTYPE CsCallComponent::createCsCallPcmPort(OMX_U32 nIndex, OMX_DIRTYPE eDir,
                                                   const OMX_AUDIO_PARAM_PCMMODETYPE &defaultPcmSettings)
{
  OMX_U32 bufferSizeMin;

  if(defaultPcmSettings.nSamplingRate == 8000)
    bufferSizeMin = 8 * 2;
  else
    bufferSizeMin = 16 * 2;

  if(nIndex == CSCALL_OUTPUT_PORT_INDEX)
	bufferSizeMin = bufferSizeMin * OMX_OUTPUT_BUFFER_DURATION;
  else
	bufferSizeMin = bufferSizeMin * OMX_INPUT_BUFFER_DURATION;


  ENS_Port * port = new CsCall_PcmPort(nIndex, eDir, bufferSizeMin,
                                       defaultPcmSettings, *this);
  if (port == 0)
  {
    return OMX_ErrorInsufficientResources;
  }

  (static_cast<AFM_Port*>(port))->setPortPriorityLevel((eDir == OMX_DirInput)? OMX_PriorityTelephonyUL_2 : OMX_PriorityTelephonyDL);

  addPort(port);

  return OMX_ErrorNone;
}

OMX_ERRORTYPE CsCallComponent::createCsCallControlPort(OMX_U32 nIndex, OMX_DIRTYPE eDir)
{
  ENS_Port * port = new CsCall_ControlPort(nIndex, eDir, *this);
  if (port == 0)
  {
    return OMX_ErrorInsufficientResources;
  }

  addPort(port);

  return OMX_ErrorNone;
}

OMX_ERRORTYPE  CsCallComponent::setParameter(OMX_INDEXTYPE nParamIndex,
                                             OMX_PTR pComponentParameterStructure)
{
  if (pComponentParameterStructure == 0) {
    return OMX_ErrorBadParameter;
  }

  switch (nParamIndex) {
    case AFM_IndexParamCscall:
      {
        CHECK_STRUCT_SIZE_AND_VERSION(pComponentParameterStructure,AFM_PARAM_CSCALLMODETYPE);
        AFM_PARAM_CSCALLMODETYPE * csCallModeType = static_cast<AFM_PARAM_CSCALLMODETYPE *>(pComponentParameterStructure);
        mCscallModeType = *csCallModeType;
        return OMX_ErrorNone;
      }

    default:
      return AFM_Component::setParameter(nParamIndex, pComponentParameterStructure);
  }
}

OMX_ERRORTYPE CsCallComponent::setConfig(OMX_INDEXTYPE nIndex,
                                         OMX_PTR pComponentConfigStructure)
{
  if (pComponentConfigStructure == 0) {
    return OMX_ErrorBadParameter;
  }

  switch (nIndex) {

    case OMX_Symbian_IndexConfigAudioSpeechLoops:
      {
        OMX_SYMBIAN_AUDIO_CONFIG_SPEECHLOOPSTYPE *config = static_cast<OMX_SYMBIAN_AUDIO_CONFIG_SPEECHLOOPSTYPE*>(pComponentConfigStructure);
        mAudioConfigSpeechLoops = *config;
        OstTraceFiltInst2 (TRACE_ALWAYS, "Cscall/proxy: setConfig OMX_IndexConfigSpeechLoops (bLoopUplinkDownlink=%d) (bLoopUplinkDownlinkAlg=%d)",config->bLoopUplinkDownlink,config->bLoopUplinkDownlinkAlg);
        OstTraceFiltInst2 (TRACE_ALWAYS, "Cscall/proxy: setConfig OMX_IndexConfigSpeechLoops (bLoopDownlinkUplink=%d) (bLoopDownlinkUplinkAlg=%d)",config->bLoopDownlinkUplink,config->bLoopDownlinkUplinkAlg);

        return OMX_ErrorNone;
      }
    case AFM_IndexConfigCscallSpeechCodecReq:
      {
        AFM_AUDIO_CONFIG_CSCALL_SPEECH_CODEC_REQ *config = static_cast<AFM_AUDIO_CONFIG_CSCALL_SPEECH_CODEC_REQ*>(pComponentConfigStructure);
        mAudioSpeechCodecReq = *config;
        OstTraceFiltInst2 (TRACE_ALWAYS, "Cscall/proxy: setConfig AFM_IndexConfigCscallSpeechCodecReq (nNetworkType=%d) (nCodingType=%d)",config->nNetworkType,config->nCodingType);
        OstTraceFiltInst1 (TRACE_ALWAYS, "Cscall/proxy: setConfig AFM_IndexConfigCscallSpeechCodecReq (nBandModeTypeDecoder=%d)",config->nBandModeTypeDecoder);

        return OMX_ErrorNone;
      }
    case AFM_IndexConfigCscallSpeechTimingReq:
      {
        AFM_AUDIO_CONFIG_CSCALL_UPLINK_TIMING_REQ *config = static_cast<AFM_AUDIO_CONFIG_CSCALL_UPLINK_TIMING_REQ*>(pComponentConfigStructure);
        mAudioSpeechTimingReq = *config;
        OstTraceFiltInst2 (TRACE_ALWAYS, "Cscall/proxy: setConfig AFM_IndexConfigCscallSpeechTimingReq (nDeliveryTime=%dus) (nModemProcessingTime=%dus)",config->nDeliveryTime,config->nModemProcessingTime);

        return OMX_ErrorNone;
      }
    case AFM_IndexConfigCscallVoiceCallStatus:
      {
        AFM_AUDIO_CONFIG_CSCALL_VOICE_CALL_STATUS *config = static_cast<AFM_AUDIO_CONFIG_CSCALL_VOICE_CALL_STATUS*>(pComponentConfigStructure);
        mAudioVoiceCallStatus = *config;
        OstTraceFiltInst1 (TRACE_ALWAYS, "Cscall/proxy: setConfig AFM_IndexConfigCscallVoiceCallStatus (bConnected=%d)",config->bConnected);

        return OMX_ErrorNone;
      }
    case AFM_IndexConfigCscallMRePReq:
      {
        OMX_STATETYPE state;
        state = omxState();
        if (state == OMX_StateExecuting)
        {
          OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/proxy: setConfig AFM_IndexConfigCscallMRePReq");
          return OMX_ErrorNone;
        }
        else
        {
          return OMX_ErrorNotReady;
        }
      }
    case OMX_IndexConfigCallbackRequest:
      {
        CHECK_STRUCT_SIZE_AND_VERSION(pComponentConfigStructure,OMX_CONFIG_CALLBACKREQUESTTYPE);
        OMX_CONFIG_CALLBACKREQUESTTYPE *config = static_cast<OMX_CONFIG_CALLBACKREQUESTTYPE*>(pComponentConfigStructure);
        OstTraceFiltInst2 (TRACE_ALWAYS, "Cscall/proxy: setConfig OMX_IndexConfigCallbackRequest (index=0x%x) (enable=%d)", config->nIndex, config->bEnable);
        switch(config->nIndex) {
          case OMX_Symbian_IndexConfigAudioCodecEnabled:
            mAudioCodecEnabled_cb = config->bEnable;
            break;
          case OMX_Symbian_IndexConfigAudioCodecDisabled:
            mAudioCodecDisabled_cb = config->bEnable;
            break;
          case OMX_Symbian_IndexConfigAudioSampleRate16khz:
            mAudioSampleRate16khz_cb = config->bEnable;
            break;
          case  OMX_Symbian_IndexConfigAudioSampleRate8khz:
            mAudioSampleRate8khz_cb = config->bEnable;
            break;
          default:
            return OMX_ErrorUnsupportedSetting;
        }
        return OMX_ErrorNone;
      }
    case OMX_Symbian_IndexConfigAudioEncoderStatus:
      {
        OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/proxy: setConfig OMX_Symbian_IndexConfigAudioEncoderStatus");
        CHECK_STRUCT_SIZE_AND_VERSION(pComponentConfigStructure, OMX_SYMBIAN_AUDIO_CONFIG_ENCODERSTATUSTYPE);
        mEncoderStatus = *(static_cast<OMX_SYMBIAN_AUDIO_CONFIG_ENCODERSTATUSTYPE *>(pComponentConfigStructure));
        return OMX_ErrorNone;
      }
    case OMX_Symbian_IndexConfigAudioDecoderStatus:
      {
        OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/proxy: setConfig OMX_Symbian_IndexConfigAudioDecoderStatus");
        CHECK_STRUCT_SIZE_AND_VERSION(pComponentConfigStructure, OMX_SYMBIAN_AUDIO_CONFIG_DECODERSTATUSTYPE);
        mDecoderStatus = *(static_cast<OMX_SYMBIAN_AUDIO_CONFIG_DECODERSTATUSTYPE *>(pComponentConfigStructure));
        return OMX_ErrorNone;
      }
    case OMX_Symbian_IndexConfigAudioUplinkTiming:
      {
        OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/proxy: setConfig OMX_Symbian_IndexConfigAudioUplinkTiming, nothing to do");
        return OMX_ErrorNone;
      }
    case OMX_Symbian_IndexConfigAudioUplinkTimingInfo:
      {
        OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/proxy: setConfig OMX_Symbian_IndexConfigAudioUplinkTimingInfo, nothing to do");
        return OMX_ErrorNone;
      }
    case OMX_Symbian_IndexConfigAudioUplinkDelayInfo:
      {
        OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/proxy: setConfig OMX_Symbian_IndexConfigAudioUplinkDelayInfo, nothing to do");
        return OMX_ErrorNone;
      }
    case AFM_IndexConfigModemLoopBack:
      {
        CHECK_STRUCT_SIZE_AND_VERSION(pComponentConfigStructure, AFM_AUDIO_CONFIG_MODEM_LOOPBACK);
        AFM_AUDIO_CONFIG_MODEM_LOOPBACK *config = static_cast<AFM_AUDIO_CONFIG_MODEM_LOOPBACK*>(pComponentConfigStructure);
        mAudioModemLoopBackConfig = *config;
        OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/proxy: setConfig AFM_IndexConfigModemLoopBack");

        return OMX_ErrorNone;
      }
    case AFM_IndexConfigCscallTimingReqMargins:
      {
        OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/proxy: setConfig AFM_IndexConfigCscallTimingReqMargins");
        CHECK_STRUCT_SIZE_AND_VERSION(pComponentConfigStructure, AFM_AUDIO_CONFIG_CSCALL_TIMING_REQ_MARGINS);
        AFM_AUDIO_CONFIG_CSCALL_TIMING_REQ_MARGINS *config = static_cast<AFM_AUDIO_CONFIG_CSCALL_TIMING_REQ_MARGINS*>(pComponentConfigStructure);
        if (config->nbMargin >= (OMX_U32)20000) {
          return OMX_ErrorBadParameter;
        }
        if (config->wbMargin >= (OMX_U32)20000) {
          return OMX_ErrorBadParameter;
        }
        mAudioTimingReqMargins = *config;
        return OMX_ErrorNone;
      }
    case AFM_IndexConfigCscallDataWaitTime:
      {
        OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/proxy: setConfig AFM_IndexConfigCscallDataWaitTime");
        CHECK_STRUCT_SIZE_AND_VERSION(pComponentConfigStructure, AFM_AUDIO_CONFIG_CSCALL_DATA_WAIT_TIME);
        AFM_AUDIO_CONFIG_CSCALL_DATA_WAIT_TIME *config = static_cast<AFM_AUDIO_CONFIG_CSCALL_DATA_WAIT_TIME*>(pComponentConfigStructure);
        if (config->targetWaitTime > 20000) {
          return OMX_ErrorBadParameter;
        }
        mAudioDataWaitTime = *config;
        return OMX_ErrorNone;
      }

    default:
      return AFM_Component::setConfig(nIndex, pComponentConfigStructure);
  }

}

OMX_ERRORTYPE CsCallComponent::getParameter( OMX_INDEXTYPE nParamIndex,
                                             OMX_PTR pComponentParameterStructure) const
{

  switch (nParamIndex) {
    case AFM_IndexParamPcmLayout:
      {
        AFM_PARAM_PCMLAYOUTTYPE *pcmlayout
          = static_cast<AFM_PARAM_PCMLAYOUTTYPE *> (pComponentParameterStructure);

        if (pcmlayout->nPortIndex > 1) {
          return OMX_ErrorBadPortIndex;
        }

        OMX_AUDIO_PARAM_PCMMODETYPE &pcmSettings =
          ((AFM_PcmPort *) getPort(pcmlayout->nPortIndex))->getPcmSettings();

        if(pcmlayout->nPortIndex == CSCALL_INPUT_PORT_INDEX)
		{
		  pcmlayout->nBlockSize = (pcmSettings.nSamplingRate/1000) * INPUT_BUFFER_DURATION;
        }
		else
		{
		  pcmlayout->nBlockSize = (pcmSettings.nSamplingRate/1000) * OUTPUT_BUFFER_DURATION;
        }

        pcmlayout->nChannels       = 1;
        pcmlayout->nMaxChannels    = 1;
        pcmlayout->nBitsPerSample  = 16;
        pcmlayout->nNbBuffers      = 2;

        return OMX_ErrorNone;
      }
    case AFM_IndexParamCscall:
      {
        AFM_PARAM_CSCALLMODETYPE * cscallPcmModeType = static_cast<AFM_PARAM_CSCALLMODETYPE *>(pComponentParameterStructure);
        *cscallPcmModeType = mCscallModeType;
        return OMX_ErrorNone;
      }

    default:
      return AFM_Component::getParameter(nParamIndex, pComponentParameterStructure);
  }
}


OMX_ERRORTYPE CsCallComponent::getConfig( OMX_INDEXTYPE nIndex,
                                          OMX_PTR pComponentConfigStructure) const
{
  switch (nIndex) {
    case OMX_Symbian_IndexConfigAudioSpeechLoops:
      {
        OMX_SYMBIAN_AUDIO_CONFIG_SPEECHLOOPSTYPE *config = static_cast<OMX_SYMBIAN_AUDIO_CONFIG_SPEECHLOOPSTYPE*>(pComponentConfigStructure);
        *config = mAudioConfigSpeechLoops;
        OstTraceFiltInst2 (TRACE_ALWAYS, "Cscall/proxy: getConfig OMX_IndexConfigSpeechLoops (bLoopUplinkDownlink=%d) (bLoopUplinkDownlinkAlg=%d)",config->bLoopUplinkDownlink,config->bLoopUplinkDownlinkAlg);
        OstTraceFiltInst2 (TRACE_ALWAYS, "Cscall/proxy: getConfig OMX_IndexConfigSpeechLoops (bLoopDownlinkUplink=%d) (bLoopDownlinkUplinkAlg=%d)",config->bLoopDownlinkUplink,config->bLoopDownlinkUplinkAlg);

        return OMX_ErrorNone;
      }
    case AFM_IndexConfigCscallSpeechCodecReq:
      {
        AFM_AUDIO_CONFIG_CSCALL_SPEECH_CODEC_REQ *config = static_cast<AFM_AUDIO_CONFIG_CSCALL_SPEECH_CODEC_REQ*>(pComponentConfigStructure);
        *config = mAudioSpeechCodecReq;
        OstTraceFiltInst2 (TRACE_ALWAYS, "Cscall/proxy: getConfig AFM_IndexConfigCscallSpeechCodecReq (nNetworkType=%d) (nCodingType=%d)",config->nNetworkType,config->nCodingType);
        OstTraceFiltInst1 (TRACE_ALWAYS, "Cscall/proxy: getConfig AFM_IndexConfigCscallSpeechCodecReq (nBandModeTypeDecoder=%d)",config->nBandModeTypeDecoder);

        return OMX_ErrorNone;
      }
    case AFM_IndexConfigModemLoopBack:
      {
        AFM_AUDIO_CONFIG_MODEM_LOOPBACK *config = static_cast<AFM_AUDIO_CONFIG_MODEM_LOOPBACK*>(pComponentConfigStructure);
        *config = mAudioModemLoopBackConfig;
        OstTraceFiltInst3 (TRACE_ALWAYS, "Cscall/proxy: getConfig AFM_IndexConfigModemLoopBack (nLoopBackMode = %d nSpeechCodec = %d nAMRrate = %d",config -> nLoopBackMode, config -> nSpeechCodec, config -> nAMRrate );

        return OMX_ErrorNone;
      }
    case AFM_IndexConfigCscallSpeechTimingReq:
      {
        AFM_AUDIO_CONFIG_CSCALL_UPLINK_TIMING_REQ *config = static_cast<AFM_AUDIO_CONFIG_CSCALL_UPLINK_TIMING_REQ*>(pComponentConfigStructure);
        *config = mAudioSpeechTimingReq;
        OstTraceFiltInst2 (TRACE_ALWAYS, "Cscall/proxy: getConfig AFM_IndexConfigCscallSpeechTimingReq (nDeliveryTime=%d) (nModemProcessingTime=%d)",config->nDeliveryTime,config->nModemProcessingTime);

        return OMX_ErrorNone;
      }
    case AFM_IndexConfigCscallVoiceCallStatus:
      {
        AFM_AUDIO_CONFIG_CSCALL_VOICE_CALL_STATUS *config = static_cast<AFM_AUDIO_CONFIG_CSCALL_VOICE_CALL_STATUS*>(pComponentConfigStructure);
        *config = mAudioVoiceCallStatus;
        OstTraceFiltInst1 (TRACE_ALWAYS, "Cscall/proxy: getConfig AFM_IndexConfigCscallVoiceCallStatus (bConnected=%d)",config->bConnected);

        return OMX_ErrorNone;
      }
    case OMX_IndexConfigCallbackRequest:
      {
        CHECK_STRUCT_SIZE_AND_VERSION(pComponentConfigStructure,OMX_CONFIG_CALLBACKREQUESTTYPE);
        OMX_CONFIG_CALLBACKREQUESTTYPE *config = static_cast<OMX_CONFIG_CALLBACKREQUESTTYPE*>(pComponentConfigStructure);
        OstTraceFiltInst1 (TRACE_ALWAYS, "Cscall/proxy: getConfig OMX_IndexConfigCallbackRequest (index=0x%x)", config->nIndex);
        switch(config->nIndex) {
          case OMX_Symbian_IndexConfigAudioCodecEnabled:
            config->bEnable = mAudioCodecEnabled_cb;
            break;
          case OMX_Symbian_IndexConfigAudioCodecDisabled:
            config->bEnable = mAudioCodecDisabled_cb;
            break;
          case OMX_Symbian_IndexConfigAudioSampleRate16khz:
            config->bEnable = mAudioSampleRate16khz_cb;
            break;
          case  OMX_Symbian_IndexConfigAudioSampleRate8khz:
            config->bEnable = mAudioSampleRate8khz_cb;
            break;
          default:
            return OMX_ErrorUnsupportedSetting;
        }
        return OMX_ErrorNone;
      }
    case OMX_Symbian_IndexConfigAudioEncoderStatus:
      {
        OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/proxy: getConfig OMX_Symbian_IndexConfigAudioEncoderStatus");
        CHECK_STRUCT_SIZE_AND_VERSION(pComponentConfigStructure,OMX_SYMBIAN_AUDIO_CONFIG_ENCODERSTATUSTYPE);
        *(static_cast<OMX_SYMBIAN_AUDIO_CONFIG_ENCODERSTATUSTYPE *>(pComponentConfigStructure)) = mEncoderStatus;
        return OMX_ErrorNone;
      }
    case OMX_Symbian_IndexConfigAudioDecoderStatus:
      {
        OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/proxy: getConfig OMX_Symbian_IndexConfigAudioDecoderStatus");
        CHECK_STRUCT_SIZE_AND_VERSION(pComponentConfigStructure,OMX_SYMBIAN_AUDIO_CONFIG_DECODERSTATUSTYPE);
        *(static_cast<OMX_SYMBIAN_AUDIO_CONFIG_DECODERSTATUSTYPE *>(pComponentConfigStructure)) = mDecoderStatus;
        return OMX_ErrorNone;
      }
    case OMX_Symbian_IndexConfigAudioUplinkTiming:
      {
        OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/proxy: getConfig OMX_Symbian_IndexConfigAudioUplinkTiming");
        CHECK_STRUCT_SIZE_AND_VERSION(pComponentConfigStructure, OMX_SYMBIAN_AUDIO_CONFIG_UPLINKTIMINGINFOTYPE);
        *(static_cast<OMX_SYMBIAN_AUDIO_CONFIG_UPLINKTIMINGTYPE*>(pComponentConfigStructure)) = mUplinkTiming;
        return OMX_ErrorNone;
      }
    case OMX_Symbian_IndexConfigAudioUplinkTimingInfo:
      {
        OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/proxy: getConfig OMX_Symbian_IndexConfigAudioUplinkTimingInfo");
        CHECK_STRUCT_SIZE_AND_VERSION(pComponentConfigStructure,OMX_SYMBIAN_AUDIO_CONFIG_UPLINKTIMINGINFOTYPE);
        *(static_cast<OMX_SYMBIAN_AUDIO_CONFIG_UPLINKTIMINGINFOTYPE*>(pComponentConfigStructure)) = mUplinkTimingInfo;
        (static_cast<const CsCallNmfHost_ProcessingComp &>(getProcessingComponent())).resetTimingStruct();
        return OMX_ErrorNone;
      }
    case OMX_Symbian_IndexConfigAudioUplinkDelayInfo:
      {
        OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/proxy: getConfig OMX_Symbian_IndexConfigAudioUplinkDelayInfo");
        CHECK_STRUCT_SIZE_AND_VERSION(pComponentConfigStructure,OMX_SYMBIAN_AUDIO_CONFIG_UPLINKDELAYINFOTYPE);
        *(static_cast<OMX_SYMBIAN_AUDIO_CONFIG_UPLINKDELAYINFOTYPE*>(pComponentConfigStructure)) = mUplinkDelayInfo;
        return OMX_ErrorNone;
      }
    case AFM_IndexConfigCscallTimingReqMargins:
      {
        OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/proxy: getConfig AFM_IndexConfigCscallTimingReqMargins");
        CHECK_STRUCT_SIZE_AND_VERSION(pComponentConfigStructure,AFM_AUDIO_CONFIG_CSCALL_TIMING_REQ_MARGINS);
        *(static_cast<AFM_AUDIO_CONFIG_CSCALL_TIMING_REQ_MARGINS*>(pComponentConfigStructure)) = mAudioTimingReqMargins;
        return OMX_ErrorNone;
      }
    case AFM_IndexConfigCscallDataWaitTime:
      {
        OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/proxy: getConfig AFM_IndexConfigCscallDataWaitTime");
        CHECK_STRUCT_SIZE_AND_VERSION(pComponentConfigStructure,AFM_AUDIO_CONFIG_CSCALL_DATA_WAIT_TIME);
        *(static_cast<AFM_AUDIO_CONFIG_CSCALL_DATA_WAIT_TIME*>(pComponentConfigStructure)) = mAudioDataWaitTime;
        return OMX_ErrorNone;
      }
    default:
      return AFM_Component::getConfig(nIndex, pComponentConfigStructure);
  }
}


OMX_ERRORTYPE CsCallComponent::getExtensionIndex(OMX_STRING cParameterName,
                                                 OMX_INDEXTYPE* pIndexType) const
{
  if (ENS_String<ENS_COMPONENT_NAME_MAX_LENGTH>(cParameterName)
      == OMX_SYMBIAN_INDEX_CONFIG_AUDIO_SAMPLERATE8KHZ_NAME) {
    *pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigAudioSampleRate8khz;
    OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/proxy: getExtensionIndex OMX_Symbian_IndexConfigAudioSampleRate8khz");

    return OMX_ErrorNone;
  }
  if (ENS_String<ENS_COMPONENT_NAME_MAX_LENGTH>(cParameterName)
      == OMX_SYMBIAN_INDEX_CONFIG_AUDIO_SAMPLERATE16KHZ_NAME) {
    *pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigAudioSampleRate16khz;
    OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/proxy: getExtensionIndex OMX_Symbian_IndexConfigAudioSampleRate16khz");

    return OMX_ErrorNone;
  }
  if (ENS_String<ENS_COMPONENT_NAME_MAX_LENGTH>(cParameterName)
      == OMX_SYMBIAN_INDEX_CONFIG_AUDIO_SPEECHCODECENABLED_NAME) {
    *pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigAudioCodecEnabled;
    OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/proxy: getExtensionIndex OMX_Symbian_IndexConfigAudioCodecEnabled");

    return OMX_ErrorNone;
  }
  if (ENS_String<ENS_COMPONENT_NAME_MAX_LENGTH>(cParameterName)
      == OMX_SYMBIAN_INDEX_CONFIG_AUDIO_SPEECHCODECDISABLED_NAME) {
    *pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigAudioCodecDisabled;
    OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/proxy: getExtensionIndex OMX_Symbian_IndexConfigAudioCodecDisabled");

    return OMX_ErrorNone;
  }

  if (ENS_String<ENS_COMPONENT_NAME_MAX_LENGTH>(cParameterName)
      == OMX_SYMBIAN_INDEX_CONFIG_AUDIO_SPEECHLOOPS_NAME) {
    *pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigAudioSpeechLoops;
    OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/proxy: getExtensionIndex OMX_Symbian_IndexConfigAudioSpeechLoops");

    return OMX_ErrorNone;
  }

  if (ENS_String<ENS_COMPONENT_NAME_MAX_LENGTH>(cParameterName)
      == OMX_SYMBIAN_INDEX_CONFIG_AUDIO_DECODERSTATUS_NAME) {
    *pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigAudioDecoderStatus;
    OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/proxy: getExtensionIndex OMX_Symbian_IndexConfigAudioDecoderStatus");

    return OMX_ErrorNone;
  }

  if (ENS_String<ENS_COMPONENT_NAME_MAX_LENGTH>(cParameterName)
      == OMX_SYMBIAN_INDEX_CONFIG_AUDIO_ENCODERSTATUS_NAME) {
    *pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigAudioEncoderStatus;
    OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/proxy: getExtensionIndex OMX_Symbian_IndexConfigAudioEncoderStatus");

    return OMX_ErrorNone;
  }
  if (ENS_String<ENS_COMPONENT_NAME_MAX_LENGTH>(cParameterName)
      == OMX_SYMBIAN_INDEX_CONFIG_AUDIO_UPLINKTIMIMG_NAME) {
    *pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigAudioUplinkTiming;
    OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/proxy: getExtensionIndex OMX_Symbian_IndexConfigAudioUplinkTiming");

    return OMX_ErrorNone;
  }
  if (ENS_String<ENS_COMPONENT_NAME_MAX_LENGTH>(cParameterName)
      == OMX_SYMBIAN_INDEX_CONFIG_AUDIO_UPLINKTIMINGINFO_NAME) {
    *pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigAudioUplinkTimingInfo;
    OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/proxy: getExtensionIndex OMX_Symbian_IndexConfigAudioUplinkTimingInfo");

    return OMX_ErrorNone;
  }

  if (ENS_String<ENS_COMPONENT_NAME_MAX_LENGTH>(cParameterName)
      == OMX_SYMBIAN_INDEX_CONFIG_AUDIO_UPLINKDELAYINFO_NAME) {
    *pIndexType = (OMX_INDEXTYPE)OMX_Symbian_IndexConfigAudioUplinkDelayInfo;
    OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/proxy: getExtensionIndex OMX_Symbian_IndexConfigAudioUplinkDelayInfo");

    return OMX_ErrorNone;
  }

  if (ENS_String<ENS_COMPONENT_NAME_MAX_LENGTH>(cParameterName)
      == "AFM.AUDIO.CONFIG.CSCALL_TIMING_REQ_MARGINS") {
    *pIndexType = (OMX_INDEXTYPE)AFM_IndexConfigCscallTimingReqMargins;
    OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/proxy: getExtensionIndex AFM_IndexConfigCscallTimingReqMargins");

    return OMX_ErrorNone;
  }

  //What about OMX_IndexConfigCallbackRequest
  //What about NewCodecReq
  //What about Timing Req


  return AFM_Component::getExtensionIndex(cParameterName, pIndexType);
}

