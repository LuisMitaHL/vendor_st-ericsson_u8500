/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   CsCallNmfHost_ProcessingComp.cpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "CsCallComponent.h"
#include "CsCallNmfHost_ProcessingComp.h"
#include "CsCall_PcmPort.h"
#include "CsCall_defines.h"
#include "OMX_IndexExt.h"
#include "OMX_CoreExt.h"
#include "audio_chipset_api_index.h"
#include "audio_hw_control_chipset_api.h"


// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cscall_proxy_CsCallNmfHost_ProcessingCompTraces.h"
#endif

#if defined (__SYMBIAN32__) || defined (__ARM_LINUX)
#include "audiomodem_helper.hpp"
#endif

#include "CsCall_ControlPort.h"

CsCallNmfHost_ProcessingComp::CsCallNmfHost_ProcessingComp(ENS_Component &enscomp) : AFMNmfHost_ProcessingComp(enscomp)
{
  cscallComp = static_cast<CsCallComponent *>(&enscomp);

  mSpeechCodec = CODEC_NONE;
  mIsRunning   = false;
  pUlTimer = NULL;
  pDlTimer = NULL;
  pTimerMReP = NULL;

  mIconfigurePH = new Icscall_nmf_host_protocolhandler_configure;

  for (int i=0; i<NB_UL_MODEM_BUFFER ;i++)
  {
    omxULBuffers[i] = NULL;

    uplinkBuffers[i].nSize = sizeof(OMX_BUFFERHEADERTYPE);
    getOmxIlSpecVersion(&uplinkBuffers[i].nVersion);
    uplinkBuffers[i].pBuffer              = 0;
    uplinkBuffers[i].nAllocLen            = 0;
    uplinkBuffers[i].nFilledLen           = 0;
    uplinkBuffers[i].nOffset              = 0;
    uplinkBuffers[i].pAppPrivate          = 0;
    uplinkBuffers[i].pPlatformPrivate     = 0;
    uplinkBuffers[i].hMarkTargetComponent = 0;
    uplinkBuffers[i].pMarkData            = 0;
    uplinkBuffers[i].nTickCount           = 0;
    uplinkBuffers[i].nTimeStamp           = 0;
    uplinkBuffers[i].nFlags               = 0;
    uplinkBuffers[i].pOutputPortPrivate   = 0;
    uplinkBuffers[i].nInputPortIndex      = 0;
    uplinkBuffers[i].nOutputPortIndex     = 0;
    uplinkBuffers[i].pInputPortPrivate    = 0;
    uplinkBuffers[i].nInputPortIndex      = 0;
    uplinkBuffers[i].nOutputPortIndex     = 0;
  }

  for (int i=0; i<NB_DL_MODEM_BUFFER ;i++)
  {
    omxDLBuffers[i] = NULL;

    downlinkBuffers[i].nSize = sizeof(OMX_BUFFERHEADERTYPE);
    getOmxIlSpecVersion(&downlinkBuffers[i].nVersion);
    downlinkBuffers[i].pBuffer              = 0;
    downlinkBuffers[i].nAllocLen            = 0;
    downlinkBuffers[i].nFilledLen           = 0;
    downlinkBuffers[i].nOffset              = 0;
    downlinkBuffers[i].pAppPrivate          = 0;
    downlinkBuffers[i].pPlatformPrivate     = 0;
    downlinkBuffers[i].hMarkTargetComponent = 0;
    downlinkBuffers[i].pMarkData            = 0;
    downlinkBuffers[i].nTickCount           = 0;
    downlinkBuffers[i].nTimeStamp           = 0;
    downlinkBuffers[i].nFlags               = 0;
    downlinkBuffers[i].pOutputPortPrivate   = 0;
    downlinkBuffers[i].nInputPortIndex      = 0;
    downlinkBuffers[i].nOutputPortIndex     = 0;
    downlinkBuffers[i].pInputPortPrivate    = 0;
    downlinkBuffers[i].nInputPortIndex      = 0;
    downlinkBuffers[i].nOutputPortIndex     = 0;
  }
}

CsCallNmfHost_ProcessingComp::~CsCallNmfHost_ProcessingComp()
{
  delete mIconfigurePH;
}


void CsCallNmfHost_ProcessingComp::InitiateModemLoopBackMode()
{
  AFM_AUDIO_CONFIG_MODEM_LOOPBACK modemLoopbackModeReq;
  Cscall_ModemLoop_Config_t loop_config;

  modemLoopbackModeReq = cscallComp->getConfigModemLoopMode();

  // 1) prepare the data...
  // 1a) Loop back mode..
  switch(modemLoopbackModeReq.nLoopBackMode)
  {
    case AFM_MODEM_LOOP_OFF : loop_config.modem_loop_type = MODEM_LOOP_OFF;
      break;
    case AFM_MODEM_LOOP_RAW : loop_config.modem_loop_type = MODEM_LOOP_RAW;
      break;
    case AFM_MODEM_LOOP_SPEECH_CODEC :
      loop_config.modem_loop_type = MODEM_LOOP_SPEECH_CODEC;
      break;
    default                          :
      loop_config.modem_loop_type = MODEM_LOOP_OFF;
      break;
  }
  // 1b) Codec in use
  switch(modemLoopbackModeReq.nSpeechCodec)
  {
    case OMX_AUDIO_CodingAMR    : if (modemLoopbackModeReq.nAMRrate <= OMX_AUDIO_AMRBandModeNB7)
      loop_config.codec_on_modem_side = CODEC_AMR_NB;
    else
      loop_config.codec_on_modem_side = CODEC_AMR_WB;
      break;
    case OMX_AUDIO_CodingGSMFR  : loop_config.codec_on_modem_side = CODEC_GSM_FR;
      break;
    case OMX_AUDIO_CodingGSMEFR : loop_config.codec_on_modem_side = CODEC_EFR;
      break;
    case OMX_AUDIO_CodingGSMHR  : loop_config.codec_on_modem_side = CODEC_GSM_HR;
      break;
    default                      :
      loop_config.codec_on_modem_side = CODEC_AMR_NB;
      break;
  }
  // 1c) rate in use
  if (modemLoopbackModeReq.nAMRrate <= OMX_AUDIO_AMRBandModeNB7)
  {
    loop_config.amr_rate = modemLoopbackModeReq.nAMRrate - OMX_AUDIO_AMRBandModeNB0;
  }
  else
  {
    loop_config.amr_rate = modemLoopbackModeReq.nAMRrate - OMX_AUDIO_AMRBandModeWB0;
  }

  OstTraceFiltInst3 (TRACE_ALWAYS, "Cscall/proxy: InitiateModemLoopBackMode() with loop_type = %d codec = %d rate = %d",loop_config.modem_loop_type,loop_config.codec_on_modem_side,loop_config.amr_rate );

  if (mIconfigurePH != NULL)
    mIconfigurePH->setModemLoopbackMode(&loop_config);

}

void CsCallNmfHost_ProcessingComp::InitiateSpeechCodecReq()
{
  AFM_AUDIO_CONFIG_CSCALL_SPEECH_CODEC_REQ audioSpeechCodecReq;
  audioSpeechCodecReq = cscallComp->getConfigSpeechCodecReq();
  CsCallCodecReq_t config = {CODEC_AMR_NB, NETWORK_NONE, 0, {0, 0, 0, 0, 0, 0, 0}, {0}};
  //generic
  switch(audioSpeechCodecReq.nCodingType)
  {
    case OMX_AUDIO_CodingAMR:
      if(audioSpeechCodecReq.nBandModeTypeDecoder<9)       
        config.speech_codec = CODEC_AMR_NB;      
      else        
        config.speech_codec = CODEC_AMR_WB;           
      break;
    case OMX_AUDIO_CodingGSMFR:
      config.speech_codec = CODEC_GSM_FR;
      break;
    case OMX_AUDIO_CodingGSMEFR:
      config.speech_codec = CODEC_EFR;
      break;
    case OMX_AUDIO_CodingGSMHR:
      config.speech_codec = CODEC_GSM_HR;
      break;
    case OMX_AUDIO_CodingPCM:
      if(audioSpeechCodecReq.bPCMcodecIn16K == OMX_TRUE)
        config.speech_codec = CODEC_PCM16;
      else
        config.speech_codec = CODEC_PCM8;
      break;
    case OMX_AUDIO_CodingUnused:
    default:
      config.speech_codec = CODEC_NONE;
      break;
  }
  config.network = (CsCallNetwork_t)(audioSpeechCodecReq.nNetworkType);
  config.init    = audioSpeechCodecReq.codecInitialization;
  //encoder
  config.encoder.AAC      = (t_uint8)audioSpeechCodecReq.encoderconfig.bAAC;
  config.encoder.nsync    = (t_uint8)audioSpeechCodecReq.encoderconfig.bNsynch;
  config.encoder.SFN      = (t_uint8)audioSpeechCodecReq.encoderconfig.nSFN;
  config.encoder.amr_rate = (t_uint8)audioSpeechCodecReq.nBandModeTypeDecoder;        
  config.encoder.dtx      = (t_uint8)audioSpeechCodecReq.encoderconfig.bDTX;
  config.encoder.nodata   = (t_uint8)audioSpeechCodecReq.encoderconfig.bNoData;
  config.encoder.NSCB     = (t_uint8)audioSpeechCodecReq.encoderconfig.bNSCB;
  //decoder
  config.decoder.amr_rate = (t_uint8)audioSpeechCodecReq.nBandModeTypeDecoder;

  OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/proxy: InitiateSpeechCodecReq() call audiomodem SpeechCodecReq");
  mIconfigureLoopBack.SpeechCodecReq(config);
}


void CsCallNmfHost_ProcessingComp::InitiateSpeechTimingReq()
{
  AFM_AUDIO_CONFIG_CSCALL_UPLINK_TIMING_REQ audioSpeechTimingReq;
  audioSpeechTimingReq  = cscallComp->getConfigSpeechTimingReq();
  t_uint32 deliveryTime = audioSpeechTimingReq.nDeliveryTime;
  t_uint32 cause        = audioSpeechTimingReq.nCause;
  /*
    TBD
    //at present SpeechTimingRequest supports only delivery time,tbd
    modified to support nModemProcessingTime also
    tuint_32 modemProcessingTime = audioSpeechTimingReq.nModemProcessingTime;
    mIconfigureLoopBack.SpeechTimingRequest(deliveryTime,modemProcessingTime);
  */
  OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/proxy: InitiateSpeechTimingReq() call audiomodem SpeechTimingRequest");
  mIconfigureLoopBack.SpeechTimingRequest(deliveryTime, cause);
}


void CsCallNmfHost_ProcessingComp::InitiateVoiceCallStatus()
{
  AFM_AUDIO_CONFIG_CSCALL_VOICE_CALL_STATUS audioVoiceCallStatus= cscallComp->getConfigVoiceCallStatus();
  OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/proxy: InitiateVoiceCallStatus() call audiomodem VoiceCallStatus");
  mIconfigureLoopBack.VoiceCallStatus((t_uint32)audioVoiceCallStatus.bConnected);
}


void CsCallNmfHost_ProcessingComp::InitiateMRePReq(AFM_AUDIO_CONFIG_CSCALL_MREP_REQ *pComponentConfigStructure)
{
  t_uint32 NextDLTiming = pComponentConfigStructure->nNextDLTiming;
  t_uint16 *pDLData = pComponentConfigStructure->pDLData;
  t_uint32 nOutputULSize = pComponentConfigStructure->nOutputULSize;
  t_uint16 *pULData = pComponentConfigStructure->pULData;

  OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/proxy: InitiateMRePReq() call audiomodem MRePInit");
  mIconfigureLoopBack.MRePInit(NextDLTiming, pDLData, nOutputULSize, pULData);
}


OMX_ERRORTYPE CsCallNmfHost_ProcessingComp::doSpecificSendCommand(OMX_COMMANDTYPE eCmd, OMX_U32 nData, OMX_BOOL &bDeferredCmd)
{
  OstTraceFiltInst2 (TRACE_ALWAYS, "Cscall/proxy: doSpecificSendCommand() (eCmd=0x%x) (nData=0x%x)",eCmd,nData);
  if(eCmd == OMX_CommandPortEnable)
  {
    OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/proxy: doSpecificSendCommand() OMX_CommandPortEnable");

    OMX_AUDIO_PARAM_PCMMODETYPE &pcmSettings_up =
      (static_cast<AFM_PcmPort *> (mENSComponent.getPort(CSCALL_INPUT_PORT_INDEX)))->getPcmSettings();
    OMX_AUDIO_PARAM_PCMMODETYPE &pcmSettings_dl =
      (static_cast<AFM_PcmPort *> (mENSComponent.getPort(CSCALL_OUTPUT_PORT_INDEX)))->getPcmSettings();

    t_sample_freq freq_up = FREQ_8KHZ;
    t_sample_freq freq_dl = FREQ_8KHZ;

    if(pcmSettings_up.nSamplingRate == 16000) freq_up = FREQ_16KHZ;
    if(pcmSettings_dl.nSamplingRate == 16000) freq_dl = FREQ_16KHZ;

    if(nData == OMX_ALL)
    {
      OstTraceFiltInst2 (TRACE_ALWAYS, "Cscall/proxy: doSpecificSendCommand() configure SRC(s) (freq_up=%d) (freq_dl=%d)",(int)freq_up,(int)freq_dl);
      mIconfigureUplink.setSampleFreq(freq_up);
      mIconfigureDownlink.setSampleFreq(freq_dl);
    }
    else if (nData == CSCALL_CONTROL_PORT_INDEX) {
      OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/proxy: doSpecificSendCommand() for CTRL port");
    }
    else if(nData == CSCALL_INPUT_PORT_INDEX)
    {
      OstTraceFiltInst1 (TRACE_ALWAYS, "Cscall/proxy: doSpecificSendCommand() configure SRC-UL (freq_up=%d)",(int)freq_up);
      mIconfigureUplink.setSampleFreq(freq_up);
    }
    else if(nData == CSCALL_OUTPUT_PORT_INDEX)
    {
      OstTraceFiltInst1 (TRACE_ALWAYS, "Cscall/proxy: doSpecificSendCommand() configure SRC-DL (freq_dl)=%d",(int)freq_dl);
      mIconfigureDownlink.setSampleFreq(freq_dl);
    }
  }
  else if (eCmd == OMX_CommandPortDisable)
  {
    if (nData == CSCALL_CONTROL_PORT_INDEX) {
      OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/proxy: doSpecificSendCommand() for CTRL port (Disable port)");
    }
  }


  return AFMNmfHost_ProcessingComp::doSpecificSendCommand(eCmd, nData, bDeferredCmd);
}


OMX_ERRORTYPE CsCallNmfHost_ProcessingComp::applyConfig(OMX_INDEXTYPE nConfigIndex, OMX_PTR pComponentConfigStructure)
{
  OstTraceFiltInst1 (TRACE_ALWAYS, "Cscall/proxy: applyConfig() (index=0x%x)",nConfigIndex);
  if (pComponentConfigStructure == 0) {
    OstTraceFiltInst0 (TRACE_ERROR, "Cscall/proxy: applyConfig() Null ptr received (error=OMX_ErrorBadParameter) sent to client");
    return OMX_ErrorBadParameter;
  }

  switch (nConfigIndex) {
    case AFM_IndexConfigAmrNbEncoder:
      {
        return OMX_ErrorNone;
      }
    case OMX_Symbian_IndexConfigAudioSpeechLoops:
      {
        //already getting this value in instantiateMain
        return OMX_ErrorNone;
      }
    case AFM_IndexConfigCscallSpeechCodecReq:
      {
        InitiateSpeechCodecReq();
        return OMX_ErrorNone;
      }
    case AFM_IndexConfigCscallSpeechTimingReq:
      {
        InitiateSpeechTimingReq();
        return OMX_ErrorNone;
      }
    case AFM_IndexConfigCscallVoiceCallStatus:
      {
        InitiateVoiceCallStatus();
        return OMX_ErrorNone;
      }
    case AFM_IndexConfigCscallMRePReq:
      {
        InitiateMRePReq(static_cast<AFM_AUDIO_CONFIG_CSCALL_MREP_REQ*>(pComponentConfigStructure));
        return OMX_ErrorNone;
      }
    case AFM_IndexConfigModemLoopBack:
      {
        InitiateModemLoopBackMode();
        return OMX_ErrorNone;
      }

    case OMX_IndexConfigCallbackRequest:
      {
        //everything is done in CsCallComponent::setConfig
        return OMX_ErrorNone;
      }
    case AFM_IndexConfigCscallTimingReqMargins:
      {
        AFM_AUDIO_CONFIG_CSCALL_TIMING_REQ_MARGINS *config = static_cast<AFM_AUDIO_CONFIG_CSCALL_TIMING_REQ_MARGINS*>(pComponentConfigStructure);
        UlTimingReqMargin_t margins = {2000,4000};
        margins.nbMargin = config->nbMargin;
        margins.wbMargin = config->wbMargin;
        OstTraceFiltInst2 (TRACE_ALWAYS, "Cscall/proxy: applyConfig() call setTimingReqMargins on protocolhandler (nbMargin=%d) (wbMargin=%d)", margins.nbMargin, margins.wbMargin);
        mIconfigurePH->setTimingReqMargins(margins);
                
        return OMX_ErrorNone;
      }
    case AFM_IndexConfigCscallDataWaitTime:
      {
        CHECK_STRUCT_SIZE_AND_VERSION(pComponentConfigStructure, AFM_AUDIO_CONFIG_CSCALL_DATA_WAIT_TIME);
        AFM_AUDIO_CONFIG_CSCALL_DATA_WAIT_TIME *config = static_cast<AFM_AUDIO_CONFIG_CSCALL_DATA_WAIT_TIME*>(pComponentConfigStructure);
        UlMaxWaitTime_t maxWaitTime = {5000,1000,5};
        maxWaitTime.threshold = (t_uint16)config->threshold;
        maxWaitTime.targetWaitTime = (t_uint16)config->targetWaitTime;
        maxWaitTime.waitBeforeReconfigure = (t_uint16)config->waitBeforeReconfigure;
        OstTraceFiltInst3 (TRACE_ALWAYS, "Cscall/proxy: applyConfig() call setMaxWaitTime on uplink (threshold=%d) (targetWaitTime=%d) (waitBeforeReconfigure=%d)",maxWaitTime.threshold, maxWaitTime.targetWaitTime, maxWaitTime.waitBeforeReconfigure);
        mIconfigureUplink.setMaxWaitTime(maxWaitTime);
        return OMX_ErrorNone;
      }
    default:
      return ProcessingComponent::applyConfig(nConfigIndex, pComponentConfigStructure);
  }
}

OMX_ERRORTYPE CsCallNmfHost_ProcessingComp::instantiateMain() {

  t_nmf_error error;
  OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/proxy: instantiateMain() called");

  t_sint32 priority_ul = getPortPriorityLevel(CSCALL_INPUT_PORT_INDEX);
  t_sint32 priority_dl = getPortPriorityLevel(CSCALL_OUTPUT_PORT_INDEX);

  const OMX_SYMBIAN_AUDIO_CONFIG_SPEECHLOOPSTYPE & speechLoops = cscallComp->getConfigSpeechLoops();
  if(speechLoops.bLoopUplinkDownlinkAlg)
  {
    OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/proxy: instantiateMain() create loopmode composite");
        
    mNmfMain = cscall_nmf_host_composite_cscallLoopULDLAlgCreate();

    OstTraceFiltInst1 (TRACE_ALWAYS, "Cscall/proxy: instantiateMain() priority for ul part of composite loop-back set to %d", priority_ul);
    OstTraceFiltInst1 (TRACE_ALWAYS, "Cscall/proxy: instantiateMain() priority for dl part of composite loop-back set to %d", priority_dl);
    ((cscall_nmf_host_composite_cscallLoopULDLAlg*)mNmfMain)->priority_ul = priority_ul;
    ((cscall_nmf_host_composite_cscallLoopULDLAlg*)mNmfMain)->priority_dl = priority_dl;

    if (mNmfMain->construct() != NMF_OK) {
      NMF_LOG("PANIC - CSCALL Construct Error\n");
      return OMX_ErrorInsufficientResources;
    }
        
    pTimerMReP = timerWrappedCreate();
    OstTraceFiltInst1 (TRACE_ALWAYS, "Cscall/proxy: instantiateMain() priority mRepTimer set to %d", priority_ul);
    pTimerMReP->priority = priority_ul;
    if (pTimerMReP->construct() != NMF_OK) {
      NMF_LOG("PANIC - CSCALL Construct Error\n");
      return OMX_ErrorInsufficientResources;
    }

    error = mNmfMain->bindComponent("timerMReP", pTimerMReP, "timer");
    if (error != NMF_OK)
    {
      NMF_LOG("Error: unable to bind timerMReP !...\n");
      return OMX_ErrorInsufficientResources;
    }
    error = pTimerMReP->bindAsynchronous("alarm", 10, mNmfMain, "alarmMReP", 0);
    if (error != NMF_OK)
    {
      NMF_LOG("Error: unable to bind alarmMReP !...\n");
      return OMX_ErrorInsufficientResources;
    }
  }
  else
  {
#if defined(__SYMBIAN32__) || defined(__ARM_LINUX)
    OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/proxy: instantiateMain() create cscall composite");
    mNmfMain = cscall_nmf_host_composite_cscallCreate();

    OstTraceFiltInst1 (TRACE_ALWAYS, "Cscall/proxy: instantiateMain() priority for ul part of composite set to %d", priority_ul);
    OstTraceFiltInst1 (TRACE_ALWAYS, "Cscall/proxy: instantiateMain() priority for dl part of composite set to %d", priority_dl);
    ((cscall_nmf_host_composite_cscall*)mNmfMain)->priority_ul = priority_ul;
    ((cscall_nmf_host_composite_cscall*)mNmfMain)->priority_dl = priority_dl;

    if (mNmfMain->construct() != NMF_OK) {
      NMF_LOG("PANIC - CSCALL Construct Error\n");
      return OMX_ErrorInsufficientResources;
    }
#else
    NMF_LOG("ONLY LOOP MODE IN THIS TARGET !");
    return OMX_ErrorUndefined;
#endif
  }

  OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/proxy: instantiateMain() create NMF timer");
  pUlTimer = timerWrappedCreate();
  pUlTimer->priority=priority_ul;
  OstTraceFiltInst1 (TRACE_ALWAYS, "Cscall/proxy: instantiateMain() priority timer set to %d", priority_ul);
  if (pUlTimer->construct() != NMF_OK) {
    NMF_LOG("PANIC - CSCALL Construct Error\n");
    return OMX_ErrorInsufficientResources;
  }

  error = mNmfMain->bindComponent("ul_timer", pUlTimer, "timer");
  if (error != NMF_OK)
  {
    NMF_LOG("Error: unable to bind ul timer !...\n");
    return OMX_ErrorInsufficientResources;
  }
  error = pUlTimer->bindAsynchronous("alarm", 10, mNmfMain, "ul_alarm", 0);
  if (error != NMF_OK)
  {
    NMF_LOG("Error: unable to bind ul alarm !...\n");
    return OMX_ErrorInsufficientResources;
  }

  pDlTimer = timerWrappedCreate();
  pDlTimer->priority=priority_dl;
  OstTraceFiltInst1 (TRACE_ALWAYS, "Cscall/proxy: instantiateMain() priority timer set to %d", priority_dl);
  if (pDlTimer->construct() != NMF_OK) {
    NMF_LOG("PANIC - CSCALL Construct Error\n");
    return OMX_ErrorInsufficientResources;
  }

  error = mNmfMain->bindComponent("dl_timer", pDlTimer, "timer");
  if (error != NMF_OK)
  {
    NMF_LOG("Error: unable to bind dl timer !...\n");
    return OMX_ErrorInsufficientResources;
  }
  error = pDlTimer->bindAsynchronous("alarm", 10, mNmfMain, "dl_alarm", 0);
  if (error != NMF_OK)
  {
    NMF_LOG("Error: unable to bind dl alarm !...\n");
    return OMX_ErrorInsufficientResources;
  }

  // bind the codecReq callback...
  error = EnsWrapper_bindToUser(mENSComponent.getOMXHandle(), 
                                mNmfMain, 
                                "networkUpdate", 
                                static_cast<cscall_nmf_host_protocolhandler_network_updateDescriptor*>(this), 4);
  if (error != NMF_OK) return OMX_ErrorInsufficientResources;

  // bind interface to drop data callback
  CsCall_ControlPort * controlPort = static_cast<CsCall_ControlPort *>(mENSComponent.getPort(CSCALL_CONTROL_PORT_INDEX));
  error = EnsWrapper_bindToUser(mENSComponent.getOMXHandle(),mNmfMain,"dropData",static_cast<cscall_nmf_host_uplink_dropDataDescriptor*>(controlPort),2);
  if (error != NMF_OK)
  {
    NMF_LOG("Error: unable to bind uplink dropData interface!...\n");
    return OMX_ErrorUndefined;
  }

  if(speechLoops.bLoopUplinkDownlinkAlg)
  {
    //create interface for configure loopback
    error = mNmfMain->bindFromUser("configure_loopbackmodem", 4, &mIconfigureLoopBack);
    if ( error != NMF_OK )
    {
      NMF_LOG("Error: unable to bind loopback configuration!...\n");
      return OMX_ErrorUndefined;
    }
  }
  // construct the configurations
  error = mNmfMain->bindFromUser("configure_protocol", 6, mIconfigurePH);
  if ( error != NMF_OK )
  {
    NMF_LOG("Error: unable to bind ph configuration!...\n");
    return OMX_ErrorUndefined;
  }

  error = mNmfMain->bindFromUser("configure_uplink", 4, &mIconfigureUplink);
  if ( error != NMF_OK )
  {
    NMF_LOG("Error: unable to bind uplink configuration!...\n");
    return OMX_ErrorUndefined;
  }

  error = mNmfMain->bindFromUser("configure_downlink", 3, &mIconfigureDownlink);
  if ( error != NMF_OK )
  {
    NMF_LOG("Error: unable to bind downlink configuration!...\n");
    return OMX_ErrorUndefined;
  }

#ifdef __SYMBIAN32__
  OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/proxy: open channel for using system clock");
  if (stericsson_cs_audio_open_channel() != OMX_ErrorNone) {
    NMF_LOG("Can not open channel for using system clock !\n");
    OstTraceFiltInst0 (TRACE_ERROR, "Cscall/proxy: Can not openn channel for using system clock !");
    return OMX_ErrorInsufficientResources;
  }
#endif

  return OMX_ErrorNone;
}

OMX_ERRORTYPE CsCallNmfHost_ProcessingComp::startMain() {

  const OMX_SYMBIAN_AUDIO_CONFIG_SPEECHLOOPSTYPE & speechLoops = cscallComp->getConfigSpeechLoops();

  OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/proxy: startMain() start composite and timers");
  mNmfMain->start();  
  pUlTimer->start();
  pDlTimer->start();
  if(speechLoops.bLoopUplinkDownlinkAlg)
  {
    pTimerMReP->start();
  }

  return OMX_ErrorNone;
}

OMX_ERRORTYPE CsCallNmfHost_ProcessingComp::stopMain() {

  const OMX_SYMBIAN_AUDIO_CONFIG_SPEECHLOOPSTYPE & speechLoops = cscallComp->getConfigSpeechLoops();

  OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/proxy: stopMain() stop composite and timers");
  mNmfMain->stop_flush();  
  pUlTimer->stop_flush();
  pDlTimer->stop_flush();
  if(speechLoops.bLoopUplinkDownlinkAlg)
  {
    pTimerMReP->stop_flush();
  }
  return OMX_ErrorNone;
}


OMX_ERRORTYPE CsCallNmfHost_ProcessingComp::configureMain() {

  const AFM_PARAM_CSCALLMODETYPE & csCallModeType = cscallComp->getConfigCscallModeType();
  const OMX_SYMBIAN_AUDIO_CONFIG_SPEECHLOOPSTYPE & speechLoops = cscallComp->getConfigSpeechLoops();
  const AFM_AUDIO_CONFIG_CSCALL_TIMING_REQ_MARGINS & timingReqMargins = cscallComp->getTimingReqMargins();
  const AFM_AUDIO_CONFIG_CSCALL_DATA_WAIT_TIME & DataWaitTime = cscallComp->getDataWaitTime();
  if(speechLoops.bLoopUplinkDownlinkAlg)
  {
    unsigned int size;

    size = 1000; // Worse case (PCM data_16K + header)
    for (int i=0; i<NB_DL_MODEM_BUFFER ;i++)
	{
      omxDLBuffers[i] = &downlinkBuffers[i];        
      downlinkBuffers[i].pBuffer = (OMX_U8 *) new char[size];
      downlinkBuffers[i].nAllocLen = size;
      downlinkBuffers[i].nFilledLen = 0;
    }
    for (int i=0; i<NB_UL_MODEM_BUFFER ;i++)
	{
      omxULBuffers[i] = &uplinkBuffers[i];  
      uplinkBuffers[i].pBuffer = (OMX_U8 *) new char[size];
      uplinkBuffers[i].nAllocLen = size;
      uplinkBuffers[i].nFilledLen = 0;
    }           
    mIconfigurePH->setModemBuffer(NB_UL_MODEM_BUFFER, omxULBuffers, NB_DL_MODEM_BUFFER, omxDLBuffers);
    mIconfigureLoopBack.SetProtocol((CsCallProtocol_t)csCallModeType.eProtocol);
  } 
#ifdef __SYMBIAN32__  
  else
  {
    // Get modem buffers
    audiomodem_helper * helper = audiomodem_helper::getHandle() ;
    if (helper == NULL) {
      NMF_PANIC("PANIC - audiomodem_helper::getHandle()\n");
      return OMX_ErrorUndefined;
    }
	
    for (int i=0; i<NB_DL_MODEM_BUFFER ;i++)
	{
      omxDLBuffers[i] = &downlinkBuffers[i];
    }
    for (int i=0; i<NB_UL_MODEM_BUFFER ;i++)
	{
      omxULBuffers[i] = &uplinkBuffers[i];
    }

    //coverity[naming_error]
    helper->allocateTxBuffersL((OMX_BUFFERHEADERTYPE **)omxULBuffers, NB_UL_MODEM_BUFFER) ;
	//coverity[naming_error]
    helper->allocateRxBuffersL((OMX_BUFFERHEADERTYPE **)omxDLBuffers, NB_DL_MODEM_BUFFER) ; 

    mIconfigurePH->setModemBuffer(NB_UL_MODEM_BUFFER, omxULBuffers, NB_DL_MODEM_BUFFER, omxDLBuffers);
  }
#elif __ARM_LINUX
  else
  {
    audiomodem_helper * amh = audiomodem_helper::getHandle();
	
    ModemBufferList * ulList = amh->getUplinkModemBufferList();
    ModemBufferList * dlList = amh->getDownlinkModemBufferList();

    unsigned int ulBuffercount = ulList->getBufferCount();
    unsigned int dlBuffercount = dlList->getBufferCount();

    for (unsigned int i=0; i<dlBuffercount ;i++)  omxDLBuffers[i] = dlList->getBuffer(i);
    for (unsigned int i=0; i<ulBuffercount ;i++)  omxULBuffers[i] = ulList->getBuffer(i);

    // configure the protocol handler
    mIconfigurePH->setModemBuffer((t_uint16)ulBuffercount,
                                  omxULBuffers,
                                  (t_uint16)dlBuffercount,
                                  omxDLBuffers);
  }
#else
  else
  {
    NMF_LOG("Error: only LOOP mode supported in OSI!...\n");
    return OMX_ErrorUndefined;
  }
#endif
  mIconfigurePH->initTimeMeasurement((static_cast<CsCallComponent &>(mENSComponent)).getTimingStructPtr(), 10000);
  mIconfigurePH->setProtocol((CsCallProtocol_t)csCallModeType.eProtocol);
    
  // configure uplink
  OstTraceFiltInst1(TRACE_ALWAYS, "Cscall/proxy: configureMain() call setParameter on uplink(realTimeinput=%d)", csCallModeType.nULRealTimeMode);
  mIconfigureUplink.setParameter((t_bool)csCallModeType.nULRealTimeMode);

  UlMaxWaitTime_t maxWaitTime = {5000,1000,5};
  maxWaitTime.threshold = (t_uint16)DataWaitTime.threshold;
  maxWaitTime.targetWaitTime = (t_uint16)DataWaitTime.targetWaitTime;
  maxWaitTime.waitBeforeReconfigure = (t_uint16)DataWaitTime.waitBeforeReconfigure;
  OstTraceFiltInst3(TRACE_ALWAYS, "Cscall/proxy: configureMain() call setMaxWaitTime on uplink (threshold=%d) (targetWaitTime=%d) (waitBeforeReconfigure=%d)",maxWaitTime.threshold, maxWaitTime.targetWaitTime, maxWaitTime.waitBeforeReconfigure);
  mIconfigureUplink.setMaxWaitTime(maxWaitTime);

  UlTimingReqMargin_t margins = {2000,4000};
  margins.nbMargin = timingReqMargins.nbMargin;
  margins.wbMargin = timingReqMargins.wbMargin;
  OstTraceFiltInst2 (TRACE_ALWAYS, "Cscall/proxy: configureMain() call setTimingReqMargins on uplink (nbMargin=%d) (wbMargin=%d)", timingReqMargins.nbMargin, timingReqMargins.wbMargin);
  mIconfigurePH->setTimingReqMargins(margins);
  if(speechLoops.bLoopUplinkDownlinkAlg)
  {
	Cscall_ModemLoop_Config_t config = {MODEM_LOOP_SPEECH_CODEC, CODEC_NONE, 0};
    OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/proxy: configureMain() call setModemLoopbackMode");
	mIconfigurePH->setModemLoopbackMode(&config);
  }
  
  // Only static configuration is supported today : Cs-SinkSource ports must be
  // configured with sample freq value which will be used during the call
  // If ports are not enabled, sample freq configuration will be done when enabling the port
  AFM_PcmPort *input_port = static_cast<AFM_PcmPort *>(mENSComponent.getPort(CSCALL_INPUT_PORT_INDEX));
  if(input_port->isEnabled())
  {
	OMX_AUDIO_PARAM_PCMMODETYPE &pcmSettings_up = input_port->getPcmSettings();
	if (pcmSettings_up.nSamplingRate == 16000) 
	{
	  OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/proxy: configureMain() call setInputSampleFreq on uplink (input_freq=FREQ_16KHZ)");
	  mIconfigureUplink.setSampleFreq(FREQ_16KHZ);
	}
	else
	{
	  OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/proxy: configureMain() call setInputSampleFreq on uplink (input_freq=FREQ_8KHZ)");
	  mIconfigureUplink.setSampleFreq(FREQ_8KHZ);
	}
  }


  // configure downlink
  mIconfigureDownlink.setParameter((t_bool)csCallModeType.nDLRealTimeMode);
  mIconfigureDownlink.setProtocol((CsCallProtocol_t)csCallModeType.eProtocol);
  
  AFM_PcmPort *output_port = static_cast<AFM_PcmPort *>(mENSComponent.getPort(CSCALL_OUTPUT_PORT_INDEX));
  // If ports are not enabled, sample freq configuration will be done when enabling the port
  if(output_port->isEnabled())
  {
	OMX_AUDIO_PARAM_PCMMODETYPE &pcmSettings_dl = output_port->getPcmSettings();
	
	if (pcmSettings_dl.nSamplingRate == 16000) 
	{
	  OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/proxy: configureMain() call setSampleFreq on downlink (input_freq=FREQ_16KHZ)");
	  mIconfigureDownlink.setSampleFreq(FREQ_16KHZ);
	}
	else
	{
	  OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/proxy: configureMain() call setSampleFreq on downlink (input_freq=FREQ_8KHZ)");
	  mIconfigureDownlink.setSampleFreq(FREQ_8KHZ);
	}
  }
  
  return OMX_ErrorNone;
}


OMX_ERRORTYPE CsCallNmfHost_ProcessingComp::deInstantiateMain() {
  t_nmf_error   nmf_error;
  OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/proxy: deInstantiateMain() called");

  nmf_error = EnsWrapper_unbindToUser(mENSComponent.getOMXHandle(), mNmfMain, "networkUpdate");
  if (nmf_error != NMF_OK) {    
    NMF_LOG("Error: unable to unbind networkUpdate!...[%d]\n", nmf_error); 
    return OMX_ErrorInsufficientResources; 
  }

  nmf_error = EnsWrapper_unbindToUser(mENSComponent.getOMXHandle(), mNmfMain, "dropData");
  if (nmf_error != NMF_OK) {    
    NMF_LOG("Error: unable to unbind dropData!...[%d]\n", nmf_error); 
    return OMX_ErrorInsufficientResources; 
  }

  nmf_error = mNmfMain->unbindFromUser("configure_protocol");
  if (nmf_error != NMF_OK) {    
    NMF_LOG("Error: unable to unbind protocol configure!...[%d]\n", nmf_error); 
    return OMX_ErrorInsufficientResources; 
  }

  nmf_error = mNmfMain->unbindFromUser("configure_uplink");
  if (nmf_error != NMF_OK) {    
    NMF_LOG("Error: unable to unbind uplink configure!...[%d]\n", nmf_error); 
    return OMX_ErrorInsufficientResources; 
  }

  nmf_error = mNmfMain->unbindFromUser("configure_downlink");
  if (nmf_error != NMF_OK) {    
    NMF_LOG("Error: unable to unbind downlink configure!...[%d]\n", nmf_error); 
    return OMX_ErrorInsufficientResources; 
  }

  nmf_error = mNmfMain->unbindComponent("ul_timer", pUlTimer, "timer");
  if (nmf_error != NMF_OK) {
    NMF_LOG("Error: unable to unbind timer!...[%d]\n", nmf_error);
    return OMX_ErrorInsufficientResources;
  }

  nmf_error = pUlTimer->unbindAsynchronous("alarm", mNmfMain, "ul_alarm");
  if (nmf_error != NMF_OK) {
    NMF_LOG("Error: unable to unbind alarm!...[%d]\n", nmf_error);
    return OMX_ErrorInsufficientResources;
  }

  nmf_error = mNmfMain->unbindComponent("dl_timer", pDlTimer, "timer");
  if (nmf_error != NMF_OK) {
    NMF_LOG("Error: unable to unbind timer!...[%d]\n", nmf_error);
    return OMX_ErrorInsufficientResources;
  }

  nmf_error = pDlTimer->unbindAsynchronous("alarm", mNmfMain, "dl_alarm");
  if (nmf_error != NMF_OK) {
    NMF_LOG("Error: unable to unbind alarm!...[%d]\n", nmf_error);
    return OMX_ErrorInsufficientResources;
  }

  const OMX_SYMBIAN_AUDIO_CONFIG_SPEECHLOOPSTYPE & speechLoops = cscallComp->getConfigSpeechLoops();

  if(speechLoops.bLoopUplinkDownlinkAlg)
  {
    nmf_error = mNmfMain->unbindComponent("timerMReP", pTimerMReP, "timer");
    if (nmf_error != NMF_OK) {
      NMF_LOG("Error: unable to unbind timerMReP!...[%d]\n", nmf_error);
      return OMX_ErrorInsufficientResources;
    }

    nmf_error = pTimerMReP->unbindAsynchronous("alarm", mNmfMain, "alarmMReP");
    if (nmf_error != NMF_OK) {
      NMF_LOG("Error: unable to unbind alarmMReP!...[%d]\n", nmf_error);
      return OMX_ErrorInsufficientResources;
    }

    nmf_error = mNmfMain->unbindFromUser("configure_loopbackmodem");
    if (nmf_error != NMF_OK) {  
      NMF_LOG("Error: unable to unbind loopbackmodem configure!...[%d]\n", nmf_error); 
      return OMX_ErrorInsufficientResources; 
    }

    for (int i=0; i<NB_DL_MODEM_BUFFER ;i++)
    {
      delete [] downlinkBuffers[i].pBuffer;
    }
	for (int i=0; i<NB_UL_MODEM_BUFFER ;i++)
    {
      delete [] uplinkBuffers[i].pBuffer;
    }

  }  
#ifdef __SYMBIAN32__  
  else {
    // Free modem buffers
    audiomodem_helper * helper = audiomodem_helper::getHandle() ;
    if (helper == NULL) {
      NMF_PANIC("PANIC - audiomodem_helper::getHandle()\n");
      return OMX_ErrorUndefined;
    }

    helper->freeRxBuffers();
    helper->freeTxBuffers();
  }
#endif

  // destroy the main component and timer
  OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/proxy: deInstantiateMain() call destroy() on cscall composite");
  mNmfMain->destroy();
  OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/proxy: deInstantiateMain() call destroy() on NMF Timers");
  pUlTimer->destroy();
  pDlTimer->destroy();

  if(speechLoops.bLoopUplinkDownlinkAlg)
  {
    OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/proxy: deInstantiateMain() destroy loopmode composite");
    cscall_nmf_host_composite_cscallLoopULDLAlgDestroy((cscall_nmf_host_composite_cscallLoopULDLAlg*&)mNmfMain);
    pTimerMReP->destroy();
    timerWrappedDestroy(pTimerMReP);
  }
#if defined(__SYMBIAN32__) || defined(__ARM_LINUX)
  else
  {
    OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/proxy: deInstantiateMain() destroy cscall composite and close ");
    cscall_nmf_host_composite_cscallDestroy((cscall_nmf_host_composite_cscall*&)mNmfMain);
  }
#endif
  OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/proxy: deInstantiateMain() destroy NMF Timers");
  timerWrappedDestroy(pUlTimer);
  timerWrappedDestroy(pDlTimer);

#ifdef __SYMBIAN32__
  OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/proxy: close channel for using system clock");
  stericsson_cs_audio_close_channel();
#endif

  return OMX_ErrorNone;
}

OMX_ERRORTYPE CsCallNmfHost_ProcessingComp::construct()
{
  OMX_ERRORTYPE error;

  error = AFMNmfHost_ProcessingComp::construct();
  if (error != OMX_ErrorNone) return error;

  return OMX_ErrorNone;
}

OMX_ERRORTYPE CsCallNmfHost_ProcessingComp::destroy(void)
{
  OMX_ERRORTYPE error;

  error = AFMNmfHost_ProcessingComp::destroy();
  if (error != OMX_ErrorNone) return error;

  return OMX_ErrorNone;
}



void CsCallNmfHost_ProcessingComp::newCodecReq(CsCallCodecReq_t config) {    

  OstTraceFiltInst1 (TRACE_ALWAYS, "Cscall/proxy: New Codec requested (speech_codec=%d)",(int)config.speech_codec);

  // if the update does not change the algorithm, or the new requested algorithm is null, then do nothing...
  if (config.speech_codec == mSpeechCodec) {
    OstTraceFiltInst1 (TRACE_ALWAYS, "Cscall/proxy:newCodecReq() Nothing to be done as same codec in use",(int)config.speech_codec);
    return;
  }

  if (config.speech_codec == CODEC_NONE) {
    mSpeechCodec = CODEC_NONE;
    if(cscallComp->codecOffNotification() == OMX_TRUE) {
      //Notify client that speech codec is disabled
      OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/proxy: informs client with (event=OMX_EventIndexSettingChanged) (type=OMX_Symbian_IndexConfigAudioCodecDisabled)");
      mENSComponent.eventHandlerCB((OMX_EVENTTYPE)OMX_EventIndexSettingChanged, (OMX_U32)OMX_ALL, 
                                   (OMX_U32)OMX_Symbian_IndexConfigAudioCodecDisabled, 0);
    }
    OMX_SYMBIAN_AUDIO_CONFIG_DECODERSTATUSTYPE decoderStatus;
    OMX_SYMBIAN_AUDIO_CONFIG_ENCODERSTATUSTYPE encoderStatus;

    decoderStatus.nSize = sizeof(OMX_SYMBIAN_AUDIO_CONFIG_DECODERSTATUSTYPE);
    encoderStatus.nSize = sizeof(OMX_SYMBIAN_AUDIO_CONFIG_ENCODERSTATUSTYPE);
    getOmxIlSpecVersion(&decoderStatus.nVersion); 
    getOmxIlSpecVersion(&encoderStatus.nVersion); 

    decoderStatus.nCoding  = encoderStatus.nCoding  = OMX_AUDIO_CodingUnused;
    decoderStatus.nBitrate = encoderStatus.nBitrate = 0;
    encoderStatus.bDtx                  = OMX_FALSE;
    encoderStatus.bAudioActivityControl = OMX_FALSE;
    encoderStatus.bNsync                = OMX_FALSE;

    mENSComponent.setConfig((OMX_INDEXTYPE)OMX_Symbian_IndexConfigAudioDecoderStatus, &decoderStatus);
    mENSComponent.setConfig((OMX_INDEXTYPE)OMX_Symbian_IndexConfigAudioEncoderStatus, &encoderStatus);

#ifdef __SYMBIAN32__
    OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/proxy: Stop use of sysClock");
    stericsson_cs_audio_release_sys_clock();
#endif

    return;
  }

  if(mSpeechCodec == CODEC_NONE)
  {
    // We start a new Call
#ifdef __SYMBIAN32__
    OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/proxy: Start use of sysClock");
    stericsson_cs_audio_use_sys_clock();
#endif
    if(cscallComp->codecOnNotification() == OMX_TRUE) {
      //Notify client that speech codec is enabled
      OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/proxy: informs client with (event=OMX_EventIndexSettingChanged) (type=OMX_Symbian_IndexConfigAudioCodecEnabled)");
      mENSComponent.eventHandlerCB((OMX_EVENTTYPE)OMX_EventIndexSettingChanged, (OMX_U32)OMX_ALL, 
                                   (OMX_U32)OMX_Symbian_IndexConfigAudioCodecEnabled, 0);
    }
  }

  if(cscallComp->SampleRate16kHzNotification() == OMX_TRUE)
  {
    if (((config.speech_codec == CODEC_AMR_WB) || (config.speech_codec == CODEC_PCM16)) &&
        ((mSpeechCodec != CODEC_AMR_WB) && (mSpeechCodec != CODEC_PCM16)))
    {
      OstTraceFiltInst0(TRACE_ALWAYS,"Cscall/proxy: informs client with (event=OMX_EventIndexSettingChanged) (type=OMX_Symbian_IndexConfigAudioSampleRate16khz)");
      mENSComponent.eventHandlerCB((OMX_EVENTTYPE)OMX_EventIndexSettingChanged, (OMX_U32)OMX_ALL, 
                                   (OMX_U32)OMX_Symbian_IndexConfigAudioSampleRate16khz, 0);
    }
  }
  if(cscallComp->SampleRate8kHzNotification() == OMX_TRUE)
  {
    if (((config.speech_codec != CODEC_AMR_WB) && (config.speech_codec != CODEC_PCM16)) &&
        ((mSpeechCodec == CODEC_AMR_WB) || (mSpeechCodec == CODEC_PCM16) || (mSpeechCodec == CODEC_NONE)))
    {
      OstTraceFiltInst0(TRACE_ALWAYS,"Cscall/proxy: informs client with (event=OMX_EventIndexSettingChanged) (type=OMX_Symbian_IndexConfigAudioSampleRate8khz)");
      mENSComponent.eventHandlerCB((OMX_EVENTTYPE)OMX_EventIndexSettingChanged, (OMX_U32)OMX_ALL, 
                                   (OMX_U32)OMX_Symbian_IndexConfigAudioSampleRate8khz, 0);
    }
  }
  mSpeechCodec = config.speech_codec;

  OstTraceFiltInst0 (TRACE_ALWAYS,"Cscall/proxy: Informs ENS_Component");
  OMX_SYMBIAN_AUDIO_CONFIG_DECODERSTATUSTYPE decoderStatus;
  OMX_SYMBIAN_AUDIO_CONFIG_ENCODERSTATUSTYPE encoderStatus;

  decoderStatus.nSize = sizeof(OMX_SYMBIAN_AUDIO_CONFIG_DECODERSTATUSTYPE);
  encoderStatus.nSize = sizeof(OMX_SYMBIAN_AUDIO_CONFIG_ENCODERSTATUSTYPE);
  getOmxIlSpecVersion(&decoderStatus.nVersion); 
  getOmxIlSpecVersion(&encoderStatus.nVersion); 

  switch (config.speech_codec) { // CODEC_NONE never appear here
    case CODEC_GSM_FR:
      decoderStatus.nCoding  = encoderStatus.nCoding  = OMX_AUDIO_CodingGSMFR;
      decoderStatus.nBitrate = encoderStatus.nBitrate = 13200;
      break;
    case CODEC_GSM_HR:
      decoderStatus.nCoding  = encoderStatus.nCoding  = OMX_AUDIO_CodingGSMHR;
      decoderStatus.nBitrate = encoderStatus.nBitrate = 12200;
      break;
    case CODEC_EFR:
      decoderStatus.nCoding  = encoderStatus.nCoding  = OMX_AUDIO_CodingGSMEFR;
      decoderStatus.nBitrate = encoderStatus.nBitrate = 12200;
      break;
    case CODEC_PCM8:
      decoderStatus.nCoding  = encoderStatus.nCoding  = OMX_AUDIO_CodingPCM;
      decoderStatus.nBitrate = encoderStatus.nBitrate = 8000*16;
      break;
    case CODEC_PCM16:
      decoderStatus.nCoding  = encoderStatus.nCoding  = OMX_AUDIO_CodingPCM;
      decoderStatus.nBitrate = encoderStatus.nBitrate = 16000*16;
      break;
    case CODEC_AMR_NB:
    case CODEC_AMR_WB:
      decoderStatus.nCoding  = encoderStatus.nCoding  = OMX_AUDIO_CodingAMR;
      switch (config.decoder.amr_rate){
        case 0: decoderStatus.nBitrate = (config.speech_codec == CODEC_AMR_NB)?  4750: 6600;
          break;
        case 1: decoderStatus.nBitrate = (config.speech_codec == CODEC_AMR_NB)?  5150: 8850;
          break;
        case 2: decoderStatus.nBitrate = (config.speech_codec == CODEC_AMR_NB)?  5900:12650;
          break;
        case 3: decoderStatus.nBitrate = (config.speech_codec == CODEC_AMR_NB)?  6700:14250;
          break;
        case 4: decoderStatus.nBitrate = (config.speech_codec == CODEC_AMR_NB)?  7400:15850;
          break;
        case 5: decoderStatus.nBitrate = (config.speech_codec == CODEC_AMR_NB)?  7950:18250;
          break;
        case 6: decoderStatus.nBitrate = (config.speech_codec == CODEC_AMR_NB)? 10200:19850;
          break;
        case 7: decoderStatus.nBitrate = (config.speech_codec == CODEC_AMR_NB)? 12200:23050;
          break;
        case 8: decoderStatus.nBitrate = (config.speech_codec == CODEC_AMR_NB)?     0:23850;
          break;
      };
      switch (config.encoder.amr_rate){
        case 0: encoderStatus.nBitrate = (config.speech_codec == CODEC_AMR_NB)?  4750: 6600;
          break;
        case 1: encoderStatus.nBitrate = (config.speech_codec == CODEC_AMR_NB)?  5150: 8850;
          break;
        case 2: encoderStatus.nBitrate = (config.speech_codec == CODEC_AMR_NB)?  5900:12650;
          break;
        case 3: encoderStatus.nBitrate = (config.speech_codec == CODEC_AMR_NB)?  6700:14250;
          break;
        case 4: encoderStatus.nBitrate = (config.speech_codec == CODEC_AMR_NB)?  7400:15850;
          break;
        case 5: encoderStatus.nBitrate = (config.speech_codec == CODEC_AMR_NB)?  7950:18250;
          break;
        case 6: encoderStatus.nBitrate = (config.speech_codec == CODEC_AMR_NB)? 10200:19850;
          break;
        case 7: encoderStatus.nBitrate = (config.speech_codec == CODEC_AMR_NB)? 12200:23050;
          break;
        case 8: encoderStatus.nBitrate = (config.speech_codec == CODEC_AMR_NB)?     0:23850;
          break;
      };
	case CODEC_NONE:
	  // CODEC_NONE never appear here
	  break;
  }
  encoderStatus.bDtx                  = (config.encoder.dtx)?OMX_TRUE:OMX_FALSE;
  encoderStatus.bAudioActivityControl = OMX_FALSE;
  encoderStatus.bNsync                = (config.encoder.nsync)?OMX_TRUE:OMX_FALSE;

  mENSComponent.setConfig((OMX_INDEXTYPE)OMX_Symbian_IndexConfigAudioDecoderStatus, &decoderStatus);
  mENSComponent.setConfig((OMX_INDEXTYPE)OMX_Symbian_IndexConfigAudioEncoderStatus, &encoderStatus);

  OstTraceFiltInst0 (TRACE_ALWAYS, "Cscall/proxy: New Codec request fulfilled");
}

void CsCallNmfHost_ProcessingComp::resetTimingStruct() const
{
  mIconfigurePH->resetTimeMeasurement();
}


