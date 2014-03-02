/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   AFM_PcmPort.cpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "AFM_PcmPort.h"
#include "AFM_Types.h"
#include "AFM_Index.h"
#include "audio_chipset_api_index.h"
#include "AFM_Symbian_AudioExt.h"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "afm_proxy_base_AFM_PcmPortTraces.h"
#endif

void 
AFM_PcmPort::fillDefaultPcmSettings(
        OMX_U32 nChannels, OMX_U32 nSampleRate, OMX_U32 nBitPerSample) {

    mPcmSettings.nSize          = sizeof(OMX_AUDIO_PARAM_PCMMODETYPE);
    getOmxIlSpecVersion(&mPcmSettings.nVersion);
    mPcmSettings.nPortIndex     = getPortIndex();
    mPcmSettings.eNumData       = OMX_NumericalDataSigned;
    mPcmSettings.eEndian        = OMX_EndianLittle;
    mPcmSettings.bInterleaved   = OMX_TRUE;
    mPcmSettings.ePCMMode       = OMX_AUDIO_PCMModeLinear;
    mPcmSettings.nBitPerSample  = nBitPerSample;

    mPcmSettings.nChannels      = nChannels;
    mPcmSettings.nSamplingRate  = nSampleRate;

    for (int i = 0; i < OMX_AUDIO_MAXCHANNELS; i++) {
        mPcmSettings.eChannelMapping[i] = OMX_AUDIO_ChannelNone;
    }

    if (nChannels == 1) {
    	 mPcmSettings.eChannelMapping[0] = OMX_AUDIO_ChannelCF;
    }    	
    else if (nChannels == 2) {
        mPcmSettings.eChannelMapping[0] = OMX_AUDIO_ChannelLF;
        mPcmSettings.eChannelMapping[1] = OMX_AUDIO_ChannelRF;
    }
    else { DBC_ASSERT(0); }

    DBC_ASSERT(checkFWSupportedPcmSettings(mPcmSettings) == OMX_ErrorNone);
}


OMX_ERRORTYPE
AFM_PcmPort::checkChannelMapping(
		const OMX_AUDIO_PARAM_PCMMODETYPE &pcmSettings) {

    if (pcmSettings.nChannels == 1) //mono
    {
        if (pcmSettings.eChannelMapping[0] != OMX_AUDIO_ChannelCF) {
            return OMX_ErrorUnsupportedSetting;
        }
    }
    else if(pcmSettings.nChannels == 2) //stereo
    {
        if (pcmSettings.eChannelMapping[0] != OMX_AUDIO_ChannelLF
                || pcmSettings.eChannelMapping[1] != OMX_AUDIO_ChannelRF) {
            return OMX_ErrorUnsupportedSetting;
        }
    }

    return OMX_ErrorNone;
}

OMX_ERRORTYPE
AFM_PcmPort::checkFWSupportedPcmSettings(
        const OMX_AUDIO_PARAM_PCMMODETYPE &pcmSettings) {
    
    if (pcmSettings.bInterleaved != OMX_TRUE) {
        // only interleaved supported by FW as of now
        return OMX_ErrorUnsupportedSetting;
    }

    if (pcmSettings.eNumData != OMX_NumericalDataSigned) {
        return OMX_ErrorUnsupportedSetting;
    }

    if ((pcmSettings.nBitPerSample != 16)&&(pcmSettings.nBitPerSample != 32)) {
        return OMX_ErrorUnsupportedSetting;
    }

    if (pcmSettings.ePCMMode != OMX_AUDIO_PCMModeLinear) {
        return OMX_ErrorUnsupportedSetting;
    }
   
	// check that the channel mapping is supported by the FW
    OMX_ERRORTYPE error = checkChannelMapping(pcmSettings);
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}

#ifndef BC_BREAK
AFM_API_EXPORT AFM_PcmPort::AFM_PcmPort(
        OMX_U32 nIndex,
        OMX_DIRTYPE eDir,
        OMX_U32 nBufferSizeMin,
        const OMX_AUDIO_PARAM_PCMMODETYPE &defaultPcmSettings,
        ENS_Component &enscomp) 	 
    : AFM_Port(nIndex, eDir, nBufferSizeMin, OMX_AUDIO_CodingPCM, enscomp) 	 
{ 	 
    mPcmLayout.nSize            = sizeof(AFM_PARAM_PCMLAYOUTTYPE);
    mPcmLayout.nPortIndex       = getPortIndex();
    mPcmLayout.nBlockSize       = 0;
    mPcmLayout.nChannels        = 0;
    mPcmLayout.nMaxChannels     = 0;
    mPcmLayout.nBitsPerSample   = 0;
    mPcmLayout.nNbBuffers       = 0;   

    mPcmSettings                = defaultPcmSettings;
    mPcmSettings.nSize          = sizeof(OMX_AUDIO_PARAM_PCMMODETYPE);
    getOmxIlSpecVersion(&mPcmSettings.nVersion);
    mPcmSettings.nPortIndex     = getPortIndex();

    mIsSynchronized             = OMX_TRUE;
}

#endif

AFM_API_EXPORT AFM_PcmPort::AFM_PcmPort(
        OMX_U32 nIndex,
        OMX_DIRTYPE eDir,
        OMX_U32 nChannels,
        OMX_U32 nSampleRate,
        OMX_U32 nBitPerSample,
        ENS_Component &enscomp) 	 
    : AFM_Port(nIndex, eDir, DEFAULT_MINBUFFERSIZE_PCM, OMX_AUDIO_CodingPCM, enscomp) 	 
{ 	 
    fillDefaultPcmSettings(nChannels, nSampleRate, nBitPerSample);
    
    mPcmLayout.nSize            = sizeof(AFM_PARAM_PCMLAYOUTTYPE);
    mPcmLayout.nPortIndex       = getPortIndex();
    mPcmLayout.nBlockSize       = 0;
    mPcmLayout.nChannels        = 0;
    mPcmLayout.nMaxChannels     = 0;
    mPcmLayout.nBitsPerSample   = 0;
    mPcmLayout.nNbBuffers       = 0;

    mIsSynchronized             = OMX_TRUE;
}

AFM_API_EXPORT OMX_ERRORTYPE AFM_PcmPort::setParameter(
        OMX_INDEXTYPE nParamIndex,
        OMX_PTR pComponentParameterStructure) 
{
    switch (nParamIndex) {
        case AFM_IndexParamForceStandardTunneling:
        {
            forceStandardTunneling();

            return OMX_ErrorNone;
        }

        case OMX_IndexParamAudioPcm:
        {
            CHECK_STRUCT_SIZE_AND_VERSION(pComponentParameterStructure,OMX_AUDIO_PARAM_PCMMODETYPE);
            OMX_AUDIO_PARAM_PCMMODETYPE *pcmsettings = 
                (OMX_AUDIO_PARAM_PCMMODETYPE *) pComponentParameterStructure;

            // check that the PCM settings are supported by the FW
            OMX_ERRORTYPE error = checkFWSupportedPcmSettings(*pcmsettings);
            if (error != OMX_ErrorNone) return error;

            // check that the PCM settings are supported by this specific port
            error = checkPcmSettings(*pcmsettings); 
            if (error != OMX_ErrorNone) return error;
            
            mPcmSettings = *pcmsettings;

            OstTraceFiltInst3(TRACE_DEBUG,"AFM_PcmPort::setParameter OMX_IndexParamAudioPcm nPortIndex=%d eEndian=%d bInterleaved=%d",mPcmSettings.nPortIndex,mPcmSettings.eEndian,mPcmSettings.bInterleaved);
            OstTraceFiltInst3(TRACE_DEBUG,"AFM_PcmPort::setParameter OMX_IndexParamAudioPcm nBitPerSample=%d nChannels=%d nSamplingRate=%d",mPcmSettings.nBitPerSample,mPcmSettings.nChannels,mPcmSettings.nSamplingRate);

            return OMX_ErrorNone;
        }

        case OMX_IndexParamAudioSynchronized:
        {
            CHECK_STRUCT_SIZE_AND_VERSION(pComponentParameterStructure,OMX_SYMBIAN_AUDIO_PARAM_SYNCHRONIZEDTYPE);
            OMX_SYMBIAN_AUDIO_PARAM_SYNCHRONIZEDTYPE *pParams = 
                (OMX_SYMBIAN_AUDIO_PARAM_SYNCHRONIZEDTYPE *) pComponentParameterStructure;

            mIsSynchronized = pParams->bIsSynchronized;

            return OMX_ErrorNone;
        }

        default:
            return AFM_Port::setParameter(
                nParamIndex, pComponentParameterStructure);
    }
}

AFM_API_EXPORT OMX_ERRORTYPE AFM_PcmPort::getParameter(
        OMX_INDEXTYPE nParamIndex,
        OMX_PTR pComponentParameterStructure) const 
{
    switch (nParamIndex) {
        case AFM_IndexParamForceStandardTunneling:
        {
            AFM_PARAM_FORCE_STANDARD_TUNNELING *tunneling =
                (AFM_PARAM_FORCE_STANDARD_TUNNELING *) pComponentParameterStructure;

            tunneling->bIsStandardTunneling = useStandardTunneling();

            return OMX_ErrorNone;
        }

        case OMX_IndexParamAudioPcm:
        {
            CHECK_STRUCT_SIZE_AND_VERSION(pComponentParameterStructure,OMX_AUDIO_PARAM_PCMMODETYPE);
            OMX_AUDIO_PARAM_PCMMODETYPE *pcmformat = 
                (OMX_AUDIO_PARAM_PCMMODETYPE *) pComponentParameterStructure;
            
            OstTraceFiltInst3(TRACE_DEBUG,"AFM_PcmPort::getParameter OMX_IndexParamAudioPcm nPortIndex=%d eEndian=%d bInterleaved=%d",mPcmSettings.nPortIndex,mPcmSettings.eEndian,mPcmSettings.bInterleaved);
            OstTraceFiltInst3(TRACE_DEBUG,"AFM_PcmPort::getParameter OMX_IndexParamAudioPcm nBitPerSample=%d nChannels=%d nSamplingRate=%d",mPcmSettings.nBitPerSample,mPcmSettings.nChannels,mPcmSettings.nSamplingRate);

            *pcmformat  = mPcmSettings;

            return OMX_ErrorNone;
        }

        case OMX_IndexParamAudioSynchronized:
        {
            CHECK_STRUCT_SIZE_AND_VERSION(pComponentParameterStructure,OMX_SYMBIAN_AUDIO_PARAM_SYNCHRONIZEDTYPE);
            OMX_SYMBIAN_AUDIO_PARAM_SYNCHRONIZEDTYPE *pParams = 
                (OMX_SYMBIAN_AUDIO_PARAM_SYNCHRONIZEDTYPE *) pComponentParameterStructure;

            pParams->bIsSynchronized = mIsSynchronized;

            return OMX_ErrorNone;
        }

        default:
            return AFM_Port::getParameter(
                nParamIndex, pComponentParameterStructure);
    }
}

OMX_ERRORTYPE AFM_PcmPort::fillPcmLayout(void) const
{
    OMX_ERRORTYPE error;
    error = getENSComponent().getParameter(
            (OMX_INDEXTYPE)AFM_IndexParamPcmLayout, &mPcmLayout);
    if(error != OMX_ErrorNone) return error;
    
    return OMX_ErrorNone;
}

AFM_API_EXPORT const AFM_PARAM_PCMLAYOUTTYPE & 
AFM_PcmPort::getPcmLayout() const 
{
    OMX_ERRORTYPE error = fillPcmLayout();
    DBC_ASSERT(error == OMX_ErrorNone);
    DBC_ASSERT(mPcmLayout.nBlockSize != 0);
    return mPcmLayout;
}

AFM_API_EXPORT OMX_ERRORTYPE
AFM_PcmPort::checkFormatCompatibility(
        OMX_HANDLETYPE hTunneledComp,
        OMX_U32 nTunneledPort) const {

    OMX_ERRORTYPE error;
    const OMX_AUDIO_PARAM_PCMMODETYPE &myPcmSettings = getPcmSettings();
    OMX_AUDIO_PARAM_PCMMODETYPE tunneledPcmSettings;

    tunneledPcmSettings.nSize = sizeof(tunneledPcmSettings);
    tunneledPcmSettings.nPortIndex = nTunneledPort;
    
    tunneledPcmSettings.nSize = sizeof(OMX_AUDIO_PARAM_PCMMODETYPE);
    getOmxIlSpecVersion(&tunneledPcmSettings.nVersion);
    error = OMX_GetParameter(
            hTunneledComp, OMX_IndexParamAudioPcm, &tunneledPcmSettings);
    if (error != OMX_ErrorNone) return error;

    if (myPcmSettings.nChannels != tunneledPcmSettings.nChannels) {
        return OMX_ErrorPortsNotCompatible;
    }
    if (myPcmSettings.eNumData != tunneledPcmSettings.eNumData) {
        return OMX_ErrorPortsNotCompatible;
    }
    if (myPcmSettings.eEndian != tunneledPcmSettings.eEndian) {
        return OMX_ErrorPortsNotCompatible;
    }
    if (myPcmSettings.bInterleaved != tunneledPcmSettings.bInterleaved) {
        return OMX_ErrorPortsNotCompatible;
    }
    if (myPcmSettings.nBitPerSample != tunneledPcmSettings.nBitPerSample) {
        return OMX_ErrorPortsNotCompatible;
    }
    if (myPcmSettings.nSamplingRate != tunneledPcmSettings.nSamplingRate) {
        return OMX_ErrorPortsNotCompatible;
    }
    if (myPcmSettings.ePCMMode != tunneledPcmSettings.ePCMMode) {
        return OMX_ErrorPortsNotCompatible;
    }
    
    for (unsigned int i = 0 ; i < myPcmSettings.nChannels; i++ ){
        if (myPcmSettings.eChannelMapping[i] 
                != tunneledPcmSettings.eChannelMapping[i]) {
            return OMX_ErrorPortsNotCompatible;
        }
    }

    return OMX_ErrorNone;
}


AFM_API_EXPORT OMX_ERRORTYPE
AFM_PcmPort::updatePcmSettings(OMX_U32 nChannels, OMX_U32 nSampleRate) {
    OMX_AUDIO_PARAM_PCMMODETYPE newPcmSettings = getPcmSettings();

    if (newPcmSettings.nChannels == nChannels 
            && newPcmSettings.nSamplingRate == nSampleRate) {
        return OMX_ErrorNone;
    }

    newPcmSettings.nChannels   = nChannels;
    newPcmSettings.nSamplingRate = nSampleRate;

    if (nChannels == 1) {
        newPcmSettings.eChannelMapping[0] = OMX_AUDIO_ChannelCF;
    } 
    else if (nChannels == 2) {
        newPcmSettings.eChannelMapping[0] = OMX_AUDIO_ChannelLF;
        newPcmSettings.eChannelMapping[1] = OMX_AUDIO_ChannelRF;
    }
    else { DBC_ASSERT(0); }

    return updateSettings(OMX_IndexParamAudioPcm, &newPcmSettings);
}



bool operator!=( //FIXME : to be removed!!!!
                const AFM_PARAM_PCMLAYOUTTYPE &pcmlayout1, 
                const AFM_PARAM_PCMLAYOUTTYPE &pcmlayout2) {
  if (pcmlayout1.nBlockSize != pcmlayout2.nBlockSize) {
    return true;
  }
  if (pcmlayout1.nBitsPerSample != pcmlayout2.nBitsPerSample) {
    return true;
  }
  // FIXME check MaxChannels
  if (pcmlayout1.nChannels != pcmlayout2.nChannels) {
    return true;
  }
  return false;
}

