/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file     SpeechProcComponent.h
 * \brief    OMX part of Speech Proc proxy
 * \author   ST-Ericsson
 */
/*****************************************************************************/
#ifndef _SPEECHPROCCOMPONENT_H_
#define _SPEECHPROCCOMPONENT_H_

#include "AFM_Component.h"
#include "speech_proc_omx_interface.h"
#include "SpeechProc_PcmPort.h"
#include "SpeechProc_ControlPort.h"
#include "SPEECH_PROC_top.h"


class SpeechProcComponent: public AFM_Component {
public:

  virtual ~SpeechProcComponent();

  OMX_ERRORTYPE construct();

  virtual OMX_ERRORTYPE getParameter(OMX_INDEXTYPE nParamIndex,
									 OMX_PTR pComponentParameterStructure) const;

  virtual OMX_ERRORTYPE setParameter(OMX_INDEXTYPE nParamIndex,
									 OMX_PTR pComponentParameterStructure);

  virtual OMX_ERRORTYPE setConfig(OMX_INDEXTYPE nIndex,
								  OMX_PTR pComponentConfigStructure);

  virtual OMX_ERRORTYPE getConfig(OMX_INDEXTYPE nIndex,
								  OMX_PTR pComponentConfigStructure) const;

  virtual OMX_ERRORTYPE getExtensionIndex(OMX_STRING cParameterName,
										  OMX_INDEXTYPE* pIndexType) const;

  virtual OMX_VERSIONTYPE getVersion(void) const;

  virtual OMX_U32 getUidTopDictionnary(void) { return KOstSPEECH_PROC_top_ID; };

  virtual bool prioritiesSetForAllPorts() const {return false;};

  SpeechProcLibrary *getLibrary(void) const {return mLibrary;};

  bool uplinkBypass(void) {return mUplinkBypass;}

  bool downlinkBypass(void) {return mDownlinkBypass;}

  bool outputReference(void) {return mOutputReference;}

  bool realtimeReference(void) {return mRealTimeReferencePort;}

private:
  OMX_ERRORTYPE createSpeechProcPcmPort(OMX_U32 nIndex,OMX_DIRTYPE eDir,OMX_U32 nBufferSizeMin,
										const OMX_AUDIO_PARAM_PCMMODETYPE &defaultPcmSettings);
  OMX_ERRORTYPE createSpeechProcControlPort(void);

  SpeechProcLibrary *mLibrary;

  // Status only used for testing
  bool mUplinkBypass;
  bool mDownlinkBypass;
  bool mOutputReference;
  bool mRealTimeReferencePort;
};

#endif
