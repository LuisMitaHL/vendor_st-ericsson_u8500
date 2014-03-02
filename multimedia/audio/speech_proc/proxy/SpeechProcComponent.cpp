/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file     SpeechProcComponent.cpp
 * \brief    OMX part of Speech Proc proxy
 * \author   ST-Ericsson
 */
/*****************************************************************************/
#include "SpeechProcComponent.h"
#include "speech_proc_config.h"


SpeechProcComponent::~SpeechProcComponent()
{
  if(mLibrary)
    delete mLibrary;
}

OMX_ERRORTYPE SpeechProcComponent::construct() {

  OMX_ERRORTYPE error;

#ifdef USE_CONTROL_PORT
  error = AFM_Component::construct(SPEECH_PROC_NB_PORT,1,false);
#else
  error = AFM_Component::construct(SPEECH_PROC_NB_AUDIO_PORT,1,false);
#endif
  if (error != OMX_ErrorNone) return error;

  // Role
  ENS_String<25> role = "speech_processing";
  role.put((OMX_STRING)mRoles[0]);
  setActiveRole(mRoles[0]);

  // Ports
  OMX_AUDIO_PARAM_PCMMODETYPE defaultPcmSettings;

  // Port APB+0
  defaultPcmSettings.eNumData           = OMX_NumericalDataSigned;
  defaultPcmSettings.eEndian            = OMX_EndianLittle;
  defaultPcmSettings.bInterleaved       = OMX_TRUE;
  defaultPcmSettings.ePCMMode           = OMX_AUDIO_PCMModeLinear;
  defaultPcmSettings.eChannelMapping[0] = OMX_AUDIO_ChannelCF;
  defaultPcmSettings.nBitPerSample      = 16;
  defaultPcmSettings.nChannels          = 1;

  defaultPcmSettings.nSamplingRate = 8000;
  defaultPcmSettings.nPortIndex    = INPUT_UPLINK_PORT;
  error = createSpeechProcPcmPort(defaultPcmSettings.nPortIndex, OMX_DirInput,
                                  (OMX_UL_INPUT_DURATION * (defaultPcmSettings.nSamplingRate/1000) *
								   defaultPcmSettings.nChannels * (defaultPcmSettings.nBitPerSample / 2)), // size in Bytes for 20ms
                                  defaultPcmSettings);
  if (error != OMX_ErrorNone) return error;

  // Port APB+1
  defaultPcmSettings.nSamplingRate = 8000;
  defaultPcmSettings.nPortIndex    = INPUT_DOWNLINK_PORT;
  error = createSpeechProcPcmPort(defaultPcmSettings.nPortIndex, OMX_DirInput,
                                  (OMX_DL_INPUT_DURATION * (defaultPcmSettings.nSamplingRate/1000) *
								   defaultPcmSettings.nChannels * (defaultPcmSettings.nBitPerSample / 2)), // size in Bytes for 20ms
                                  defaultPcmSettings);
  if (error != OMX_ErrorNone) return error;

  // Port APB+2
  defaultPcmSettings.nSamplingRate = 8000;
  defaultPcmSettings.nPortIndex    = INPUT_REFERENCE_PORT;
  error = createSpeechProcPcmPort(defaultPcmSettings.nPortIndex, OMX_DirInput,
                                  (OMX_UL_REFERENCE_DURATION * (defaultPcmSettings.nSamplingRate/1000) *
								   defaultPcmSettings.nChannels * (defaultPcmSettings.nBitPerSample / 2)), // size in Bytes for 20ms
                                  defaultPcmSettings);
  if (error != OMX_ErrorNone) return error;

  // Port APB+3
  defaultPcmSettings.nSamplingRate = 8000;
  defaultPcmSettings.nPortIndex    = OUTPUT_UPLINK_PORT;
  error = createSpeechProcPcmPort(defaultPcmSettings.nPortIndex, OMX_DirOutput,
                                  (OMX_UL_OUTPUT_DURATION * (defaultPcmSettings.nSamplingRate/1000) *
								   defaultPcmSettings.nChannels * (defaultPcmSettings.nBitPerSample / 2)), // size in Bytes for 20ms
                                  defaultPcmSettings);
  if (error != OMX_ErrorNone) return error;

  // Port APB+4
  defaultPcmSettings.nSamplingRate = 8000;
  defaultPcmSettings.nPortIndex    = OUTPUT_DOWNLINK_PORT;
  error = createSpeechProcPcmPort(defaultPcmSettings.nPortIndex, OMX_DirOutput,
                                  (OMX_DL_OUTPUT_DURATION * (defaultPcmSettings.nSamplingRate/1000) *
								   defaultPcmSettings.nChannels * (defaultPcmSettings.nBitPerSample / 2)), // size in Bytes for 20ms
                                  defaultPcmSettings);
  if (error != OMX_ErrorNone) return error;

  // Port OPB+0
#ifdef USE_CONTROL_PORT
  error = createSpeechProcControlPort();
  if (error != OMX_ErrorNone) return error;
#endif

  // Instantiate OMX interface for SpeechProc Library
  mLibrary = speech_proc_getOMXInterface();

  if(mLibrary == NULL)
    return OMX_ErrorInsufficientResources;

  mUplinkBypass = false;
  mDownlinkBypass = false;
  mOutputReference = false;
  mRealTimeReferencePort = true;

  return OMX_ErrorNone;
}

OMX_ERRORTYPE  SpeechProcComponent::setParameter(OMX_INDEXTYPE nParamIndex,
                                                 OMX_PTR pComponentParameterStructure) {
  OMX_ERRORTYPE error;

  if (pComponentParameterStructure == 0) {
    return OMX_ErrorBadParameter;
  }

  // check if index is handled by processing library
  error = mLibrary->checkParameter(nParamIndex,pComponentParameterStructure);
  if(error != OMX_ErrorUnsupportedIndex)
    return error;

  // if not, propagate to upper layer
  switch (nParamIndex) {
	case AFM_IndexParamSpeechProc :
	  {
		AFM_AUDIO_PARAM_SPEECH_PROC *param = (AFM_AUDIO_PARAM_SPEECH_PROC *)pComponentParameterStructure;

		mUplinkBypass = (bool)param->bBypassUplink;
		mDownlinkBypass = (bool)param->bBypassDownlink;
		mRealTimeReferencePort = (bool)param->bRealTimeReference;
		mOutputReference = (bool)param->bOutputReference;

		return OMX_ErrorNone;
	  }
    default:
      return AFM_Component::setParameter(nParamIndex, pComponentParameterStructure);
  }
}

OMX_ERRORTYPE SpeechProcComponent::setConfig(OMX_INDEXTYPE nIndex,
                                             OMX_PTR pComponentConfigStructure)
{
  OMX_ERRORTYPE error;

  if (pComponentConfigStructure == 0) {
    return OMX_ErrorBadParameter;
  }

  // check if index is handled by processing library
  error = mLibrary->checkConfig(nIndex,pComponentConfigStructure);
  if(error != OMX_ErrorUnsupportedIndex)
    return error;

  // if not, propagate to upper layer
  switch (nIndex) {
    default:
      return AFM_Component::setConfig(nIndex, pComponentConfigStructure);
  }
}


OMX_ERRORTYPE SpeechProcComponent::getParameter(OMX_INDEXTYPE nParamIndex,
                                                OMX_PTR pComponentParameterStructure) const
{
  OMX_ERRORTYPE error;

  // check if index is handled by processing library
  error = mLibrary->getOMXParamOrConfig(nParamIndex, pComponentParameterStructure);
  if(error != OMX_ErrorUnsupportedIndex)
    return error;

  // if not, process it or propagate to upper layer
  switch (nParamIndex) {
	case AFM_IndexParamSpeechProc:
	  {
		AFM_AUDIO_PARAM_SPEECH_PROC *param = (AFM_AUDIO_PARAM_SPEECH_PROC *)pComponentParameterStructure;

		param->bBypassUplink = (OMX_BOOL)mUplinkBypass;
		param->bBypassDownlink = (OMX_BOOL)mDownlinkBypass;
		param->bRealTimeReference = (OMX_BOOL)mRealTimeReferencePort;
		param->bOutputReference = (OMX_BOOL)mOutputReference;

		return OMX_ErrorNone;
	  }
    case AFM_IndexParamPcmLayout:
      {
        AFM_PARAM_PCMLAYOUTTYPE *pcmlayout = (AFM_PARAM_PCMLAYOUTTYPE *) pComponentParameterStructure;
        if(pcmlayout->nPortIndex > SPEECH_PROC_NB_AUDIO_PORT) return OMX_ErrorBadPortIndex;

        const AFM_PcmPort *port = static_cast<const AFM_PcmPort *>(getPort(pcmlayout->nPortIndex));
        const OMX_AUDIO_PARAM_PCMMODETYPE  &pcmformat = port->getPcmSettings();

        pcmlayout->nChannels       = pcmformat.nChannels;
        pcmlayout->nMaxChannels    = pcmformat.nChannels;
        pcmlayout->nNbBuffers      = 2;
        pcmlayout->nBitsPerSample  = 16;

        if(pcmformat.nPortIndex == INPUT_UPLINK_PORT)
        {
          pcmlayout->nBlockSize = (pcmformat.nSamplingRate/1000) * UL_INPUT_BUFFER_DURATION;
        }
        else if (pcmformat.nPortIndex == INPUT_REFERENCE_PORT)
        {
          pcmlayout->nBlockSize = (pcmformat.nSamplingRate/1000) * REF_INPUT_BUFFER_DURATION;
#ifdef FORCE_MONO_REFERENCE
          pcmlayout->nChannels    = 1;
          pcmlayout->nMaxChannels = 1;
#endif
        }
        else if (pcmformat.nPortIndex == OUTPUT_DOWNLINK_PORT)
        {
          pcmlayout->nBlockSize = (pcmformat.nSamplingRate/1000) * DL_OUTPUT_BUFFER_DURATION;
        }
        else
        {
          pcmlayout->nBlockSize = (pcmformat.nSamplingRate/1000) * PROCESSING_BUFFER_DURATION;
        }

#ifdef USE_HYBRID_COMPONENT
        // For hybrid component dsp port are 24bits
        if((pcmformat.nPortIndex == OUTPUT_DOWNLINK_PORT)||
           (pcmformat.nPortIndex == INPUT_UPLINK_PORT) ||
           (pcmformat.nPortIndex == INPUT_REFERENCE_PORT))
        {
          pcmlayout->nBitsPerSample  = 24;
        }
#else
		// ARM version of SRC works on 32bit samples
		if(pcmformat.nPortIndex == INPUT_REFERENCE_PORT)
        {
          pcmlayout->nBitsPerSample  = 32;
        }
#endif
        return OMX_ErrorNone;
      }
    default:
      return AFM_Component::getParameter(nParamIndex, pComponentParameterStructure);
  }
}


OMX_ERRORTYPE SpeechProcComponent::getConfig(OMX_INDEXTYPE nIndex,
                                             OMX_PTR pComponentConfigStructure) const
{
  OMX_ERRORTYPE error;

  // check if index is handled by processing library
  error = mLibrary->getOMXParamOrConfig(nIndex, pComponentConfigStructure);
  if(error != OMX_ErrorUnsupportedIndex)
    return error;

  // if not process it or propagate to upper layer
  switch (nIndex) {
    default:
      return AFM_Component::getConfig(nIndex, pComponentConfigStructure);
  }
}

OMX_ERRORTYPE SpeechProcComponent::getExtensionIndex(OMX_STRING cParameterName,
                                                     OMX_INDEXTYPE* pIndexType) const
{
  OMX_ERRORTYPE error;

  if (ENS_String<ENS_COMPONENT_NAME_MAX_LENGTH>(cParameterName)
	  == "AFM.Param.SpeechProc")
  {
	*pIndexType = (OMX_INDEXTYPE)AFM_IndexParamSpeechProc;
	return OMX_ErrorNone;
  }

  // check if extension is handled by processing library
  error = mLibrary->getExtensionIndex(cParameterName, (OMX_U32 *)pIndexType);
  if(error == OMX_ErrorNone) return error;

  // if not process it or propagate to upper layer
  return AFM_Component::getExtensionIndex(cParameterName, pIndexType);
}

OMX_VERSIONTYPE SpeechProcComponent::getVersion(void) const
{
  OMX_VERSIONTYPE version;

  version.s.nVersionMajor = SPEECH_PROC_MAJOR;
  version.s.nVersionMinor = SPEECH_PROC_MINOR;
  version.s.nRevision     = SPEECH_PROC_REVISION;
  version.s.nStep         = 0;

  // put some usefull information in step
#ifndef USE_HYBRID_COMPONENT
  version.s.nStep += (1<<0);
#endif

  if(UL_HYBRID_BUFFER_MEMORY == SP_MEM_TCM)
	version.s.nStep += (1<<1);

  if (DL_HYBRID_BUFFER_MEMORY == SP_MEM_TCM)
	version.s.nStep += (1<<2);

  if (SRC_MEMORY == CM_MM_MPC_TCM24)
	version.s.nStep += (1<<3);

  if (RING_BUFFER_MEMORY == CM_MM_MPC_TCM24)
	version.s.nStep += (1<<4);

#if PROCESSING_BUFFER_DURATION == 20
  version.s.nStep += (1<<5);
#endif

#ifdef FORCE_MONO_REFERENCE
  version.s.nStep += (1<<6);
#endif

#ifdef USE_CONTROL_PORT
  version.s.nStep += (1<<7);
#endif

  return version;
}


// private function
OMX_ERRORTYPE SpeechProcComponent::createSpeechProcPcmPort(OMX_U32 nIndex, OMX_DIRTYPE eDir, OMX_U32 nBufferSizeMin,
                                                           const OMX_AUDIO_PARAM_PCMMODETYPE &defaultPcmSettings)
{
  ENS_Port * port = new SpeechProc_PcmPort(nIndex, eDir, nBufferSizeMin, defaultPcmSettings, *this);
  if (port == 0) return OMX_ErrorInsufficientResources;

  addPort(port);
  return OMX_ErrorNone;
}

OMX_ERRORTYPE SpeechProcComponent::createSpeechProcControlPort(void)
{
  ENS_Port * port = new SpeechProc_ControlPort(*this);

  if (port == 0) return OMX_ErrorInsufficientResources;

  addPort(port);

  return OMX_ErrorNone;
}
