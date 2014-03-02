/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   CsCallComponent.h
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _CSCALL_COMPONENT_H_
#define _CSCALL_COMPONENT_H_

#include "AFM_Component.h"
#include "cscall_top.h"
#include "AFM_Types.h"
#include "OMX_Symbian_AudioExt_Ste.h"


class CsCallComponent: public AFM_Component {
public:

  CsCallComponent();
  OMX_ERRORTYPE construct();

  virtual OMX_ERRORTYPE getParameter(OMX_INDEXTYPE nParamIndex,
									 OMX_PTR pComponentParameterStructure) const;


  virtual OMX_ERRORTYPE setParameter(OMX_INDEXTYPE nParamIndex,
									 OMX_PTR pComponentParameterStructure);

  virtual OMX_ERRORTYPE setConfig(OMX_INDEXTYPE nIndex,
								  OMX_PTR pComponentConfigStructure);

  virtual OMX_ERRORTYPE getConfig(OMX_INDEXTYPE nIndex,
								  OMX_PTR pComponentConfigStructure) const;

  virtual OMX_U32 getUidTopDictionnary(void) {return KOstcscall_top_ID;}

  virtual OMX_VERSIONTYPE getVersion(void) const;

  const OMX_SYMBIAN_AUDIO_CONFIG_SPEECHLOOPSTYPE & getConfigSpeechLoops(void) const {
	return mAudioConfigSpeechLoops;
  }

  const AFM_AUDIO_CONFIG_CSCALL_SPEECH_CODEC_REQ & getConfigSpeechCodecReq(void) const {
	return mAudioSpeechCodecReq;
  }

  const AFM_AUDIO_CONFIG_MODEM_LOOPBACK & getConfigModemLoopMode(void) const {
	return mAudioModemLoopBackConfig;
  }


  const AFM_AUDIO_CONFIG_CSCALL_UPLINK_TIMING_REQ & getConfigSpeechTimingReq(void) const {
	return mAudioSpeechTimingReq;
  }

  const AFM_AUDIO_CONFIG_CSCALL_VOICE_CALL_STATUS & getConfigVoiceCallStatus(void) const {
	return mAudioVoiceCallStatus;
  }

  const AFM_PARAM_CSCALLMODETYPE & getConfigCscallModeType(void) const {
    return mCscallModeType;
  }

  OMX_BOOL codecOnNotification (void) const {
	return mAudioCodecEnabled_cb;
  }

  OMX_BOOL codecOffNotification (void) const {
	return mAudioCodecDisabled_cb;
  }

  OMX_BOOL SampleRate16kHzNotification (void) const {
    return mAudioSampleRate16khz_cb;
  }

  OMX_BOOL SampleRate8kHzNotification (void) const {
    return mAudioSampleRate8khz_cb;
  }

  void* getTimingStructPtr() {
      return &mUplinkTimingInfo;
  }

  const AFM_AUDIO_CONFIG_CSCALL_TIMING_REQ_MARGINS & getTimingReqMargins(void) const {
      return mAudioTimingReqMargins;
  }

  const AFM_AUDIO_CONFIG_CSCALL_DATA_WAIT_TIME & getDataWaitTime(void) const {
      return mAudioDataWaitTime;
  }

  OMX_ERRORTYPE getExtensionIndex(OMX_STRING cParameterName, OMX_INDEXTYPE* pIndexType) const;

  virtual bool prioritiesSetForAllPorts() const {return false;};

private:

  OMX_ERRORTYPE createCsCallControlPort(OMX_U32 nIndex, OMX_DIRTYPE eDir);
  OMX_ERRORTYPE createCsCallPcmPort(OMX_U32 nIndex, OMX_DIRTYPE eDir,
									const OMX_AUDIO_PARAM_PCMMODETYPE &defaultPcmSettings);

  OMX_BOOL										mAudioCodecEnabled_cb;
  OMX_BOOL										mAudioCodecDisabled_cb;
  OMX_BOOL                                      mAudioSampleRate16khz_cb;
  OMX_BOOL                                      mAudioSampleRate8khz_cb;
  OMX_SYMBIAN_AUDIO_CONFIG_SPEECHLOOPSTYPE      mAudioConfigSpeechLoops;
  AFM_AUDIO_CONFIG_CSCALL_SPEECH_CODEC_REQ		mAudioSpeechCodecReq;
  AFM_AUDIO_CONFIG_CSCALL_UPLINK_TIMING_REQ	    mAudioSpeechTimingReq;
  AFM_AUDIO_CONFIG_CSCALL_TIMING_REQ_MARGINS    mAudioTimingReqMargins;
  AFM_AUDIO_CONFIG_CSCALL_DATA_WAIT_TIME        mAudioDataWaitTime;
  AFM_AUDIO_CONFIG_CSCALL_VOICE_CALL_STATUS     mAudioVoiceCallStatus;
  AFM_PARAM_CSCALLMODETYPE                      mCscallModeType;
  AFM_AUDIO_CONFIG_MODEM_LOOPBACK	  	  	  	mAudioModemLoopBackConfig;

  OMX_SYMBIAN_AUDIO_CONFIG_DECODERSTATUSTYPE    mDecoderStatus;
  OMX_SYMBIAN_AUDIO_CONFIG_ENCODERSTATUSTYPE    mEncoderStatus;

  OMX_SYMBIAN_AUDIO_CONFIG_UPLINKTIMINGTYPE     mUplinkTiming;
  OMX_SYMBIAN_AUDIO_CONFIG_UPLINKTIMINGINFOTYPE mUplinkTimingInfo;
  OMX_SYMBIAN_AUDIO_CONFIG_UPLINKDELAYINFOTYPE  mUplinkDelayInfo;
};

#endif // _CSCALL_COMPONENT_H_

