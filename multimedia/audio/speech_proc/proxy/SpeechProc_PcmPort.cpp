/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file     SpeechProc_PcmPort.cpp
 * \brief    OMX part of Speech Proc proxy (port)
 * \author   ST-Ericsson
 */
/*****************************************************************************/
#include "SpeechProc_PcmPort.h"
#include "speech_proc_omx_interface.h"
#include "speech_proc_config.h"

SpeechProc_PcmPort::SpeechProc_PcmPort(OMX_U32 nIndex, OMX_DIRTYPE eDir, OMX_U32 nBufferSizeMin,
                                       const OMX_AUDIO_PARAM_PCMMODETYPE &defaultPcmSettings,
                                       ENS_Component &enscomp)
  : AFM_PcmPort(nIndex, eDir, nBufferSizeMin, defaultPcmSettings, enscomp)
{
  switch(nIndex)
  {
    // Mpc port for Hybrid component
    case INPUT_UPLINK_PORT:
    case INPUT_REFERENCE_PORT:
    case OUTPUT_DOWNLINK_PORT:
#ifdef USE_HYBRID_COMPONENT
      setMpc(true);
      setSupportedProprietaryComm(AFM_HYBRIDMPC_PROPRIETARY_COMMUNICATION);
      break;
#endif
      // Host Port
    case INPUT_DOWNLINK_PORT:
    case OUTPUT_UPLINK_PORT:
      setMpc(false);
#ifdef USE_HYBRID_COMPONENT
      setSupportedProprietaryComm(AFM_HYBRIDHOST_PROPRIETARY_COMMUNICATION);
#else
      setSupportedProprietaryComm(AFM_HOST_PROPRIETARY_COMMUNICATION);
#endif
      break;

    default:
      /* error */
      break;
  }
}


OMX_ERRORTYPE SpeechProc_PcmPort::setParameter(OMX_INDEXTYPE nParamIndex,
                                               OMX_PTR pComponentParameterStructure)
{
  OMX_ERRORTYPE error = OMX_ErrorNone;

  switch (nParamIndex)
  {
    case OMX_IndexParamAudioPcm:
      {
        OMX_AUDIO_PARAM_PCMMODETYPE *new_format = (OMX_AUDIO_PARAM_PCMMODETYPE *) pComponentParameterStructure;
		OMX_U32 previous_buffer_size = mParamPortDefinition.nBufferSize;

		// will call checkPcmSettings
        // but we cannot call updateSampleFreq from checkPcmSettings
		// because of const, that's why we call it after
        error = AFM_PcmPort::setParameter(nParamIndex, pComponentParameterStructure);
		if(error != OMX_ErrorNone) return error;

        // update buffer Size if needed
		error = updateSampleFreq(new_format->nSamplingRate, new_format->nChannels,
								 new_format->nBitPerSample, previous_buffer_size);

        return error;
      }
    default:
      return AFM_PcmPort::setParameter(nParamIndex, pComponentParameterStructure);
  }
}


OMX_ERRORTYPE SpeechProc_PcmPort::checkPcmSettings(const OMX_AUDIO_PARAM_PCMMODETYPE &pcmSettings) const
{
  // check nb channels
  if(pcmSettings.nPortIndex == INPUT_REFERENCE_PORT)
  {
    if(pcmSettings.nChannels > MAX_NB_OMX_REF_CHANNEL) return OMX_ErrorUnsupportedSetting;
  }
  else if (pcmSettings.nPortIndex == INPUT_UPLINK_PORT)
  {
    if(pcmSettings.nChannels > MAX_NB_INPUT_CHANNEL) return OMX_ErrorUnsupportedSetting;
  }
  else if (pcmSettings.nChannels != 1)
  {
    return OMX_ErrorUnsupportedSetting;
  }

  // check sample frequency
  if((pcmSettings.nPortIndex != INPUT_REFERENCE_PORT) &&
     (pcmSettings.nSamplingRate != 8000) && (pcmSettings.nSamplingRate != 16000))
    return OMX_ErrorUnsupportedSetting;

  return OMX_ErrorNone;
}


OMX_ERRORTYPE SpeechProc_PcmPort::updateSampleFreq(OMX_U32 samplerate, OMX_U32 nb_channel,
												   OMX_U32 bit_per_sample, OMX_U32 previous_buffer_size)
{
  OMX_U32 portIndex = getPortIndex();
  OMX_U32 bufferSize;

  if(portIndex == INPUT_UPLINK_PORT)
  {
    bufferSize = OMX_UL_INPUT_DURATION;
  }
  else if (portIndex == INPUT_REFERENCE_PORT)
  {
    bufferSize = OMX_UL_REFERENCE_DURATION;
  }
  else if (portIndex == OUTPUT_UPLINK_PORT)
  {
    bufferSize = OMX_UL_OUTPUT_DURATION;
  }
  else if (portIndex == OUTPUT_DOWNLINK_PORT)
  {
    bufferSize = OMX_DL_OUTPUT_DURATION;
  }
  else
  {
    bufferSize = OMX_DL_INPUT_DURATION;
  }

  bufferSize = nb_channel * bufferSize * (samplerate/1000) * (bit_per_sample/8);

  // Currently, updateSettings will call a "basic" setParameter and as nBufferSize
  // filed is read only, the setParameter function will NOT update nBufferSize
  // that why we do it directly here.
  // Also we always re-write the value as it may have been modified by AFM layer
  mParamPortDefinition.nBufferSize = bufferSize;

  if(bufferSize != previous_buffer_size)
  {
	// update port buffer size (OMX_PARAM_PORTDEFINITIONTYPE)
	OMX_PARAM_PORTDEFINITIONTYPE paramPortDefinition;
	paramPortDefinition.nSize                    = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
	paramPortDefinition.nVersion.s.nVersionMajor = OMX_VERSION_MAJOR;
	paramPortDefinition.nVersion.s.nVersionMinor = OMX_VERSION_MINOR;
	paramPortDefinition.nVersion.s.nRevision     = OMX_VERSION_REVISION;
	paramPortDefinition.nVersion.s.nStep         = OMX_VERSION_STEP;
	getParameter(OMX_IndexParamPortDefinition, (OMX_PTR)&paramPortDefinition);
	paramPortDefinition.nBufferSize = bufferSize;

	return updateSettings(OMX_IndexParamPortDefinition, (OMX_PTR)&paramPortDefinition);
  }

  return OMX_ErrorNone;
}
