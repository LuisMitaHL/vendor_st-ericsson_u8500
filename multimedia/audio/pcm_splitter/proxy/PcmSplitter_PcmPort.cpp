/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "PcmSplitter_PcmPort.h"
#include "AFM_Types.h"
#include "AFM_Index.h"
#include "ENS_Nmf.h"
#include "AFM_nmf.h"
#include "AFM_Utils.h"
#include "PcmSplitter.h"

PcmSplitter_PcmPort::PcmSplitter_PcmPort(
    OMX_U32 nIndex,
    OMX_DIRTYPE eDir,
    OMX_U32 nBufferSizeMin,
    const OMX_AUDIO_PARAM_PCMMODETYPE &defaultPcmSettings,
    ENS_Component &enscomp)
        : AFM_PcmPort(
            nIndex, eDir, nBufferSizeMin, defaultPcmSettings, enscomp),
        mIsSynchronized(OMX_FALSE),mIsPaused(OMX_FALSE)
{
    for (OMX_U32 i = 0; i < OMX_AUDIO_MAXCHANNELS; i++) {
        mChannelSwitchBoard[i] = i;
    }
}

OMX_ERRORTYPE 
PcmSplitter_PcmPort::checkChannelSwitchBoardConfig(const OMX_U32 *channelSwitchBoard) const {
    DBC_PRECONDITION(getPortIndex() != 0);
    const OMX_AUDIO_PARAM_PCMMODETYPE &pcmSettingsIn = static_cast<const AFM_PcmPort *>(getENSComponent().getPort(0))->getPcmSettings();
    for (OMX_U32 i = 0; i < getPcmSettings().nChannels; i++) {
        if (channelSwitchBoard[i] >= pcmSettingsIn.nChannels) return OMX_ErrorUnsupportedSetting;
    }
    return OMX_ErrorNone;
}

OMX_ERRORTYPE 
PcmSplitter_PcmPort::setChannelSwitchBoard(OMX_U32 *channelSwitchBoard) {
    OMX_ERRORTYPE error = checkChannelSwitchBoardConfig(channelSwitchBoard);
    if (error != OMX_ErrorNone) return error;
    
    for (OMX_U32 i = 0; i < getPcmSettings().nChannels; i++) {
        mChannelSwitchBoard[i] = channelSwitchBoard[i];
    }

    return OMX_ErrorNone;
}

void
PcmSplitter_PcmPort::getChannelSwitchBoard(OMX_U32 *channelSwitchBoard) const {
    for (OMX_U32 i = 0; i < getPcmSettings().nChannels; i++) {
        channelSwitchBoard[i] = mChannelSwitchBoard[i];
    }
}

OMX_ERRORTYPE PcmSplitter_PcmPort::setParameter(
    OMX_INDEXTYPE nParamIndex,
    OMX_PTR pComponentParameterStructure)
{
    switch (nParamIndex) {
        case OMX_IndexParamAudioSynchronized:
            {
                CHECK_STRUCT_SIZE_AND_VERSION(pComponentParameterStructure,OMX_SYMBIAN_AUDIO_PARAM_SYNCHRONIZEDTYPE);
                OMX_SYMBIAN_AUDIO_PARAM_SYNCHRONIZEDTYPE *pParams =
                    (OMX_SYMBIAN_AUDIO_PARAM_SYNCHRONIZEDTYPE *) pComponentParameterStructure;

                mIsSynchronized = pParams->bIsSynchronized;

                return OMX_ErrorNone;
            }

        case OMX_IndexParamAudioPcm:
        {
            CHECK_STRUCT_SIZE_AND_VERSION(pComponentParameterStructure,OMX_AUDIO_PARAM_PCMMODETYPE);
            OMX_AUDIO_PARAM_PCMMODETYPE *pcmsettings =(OMX_AUDIO_PARAM_PCMMODETYPE *) pComponentParameterStructure;

            if (pcmsettings->nPortIndex != 0) {
                return AFM_PcmPort::setParameter(nParamIndex, pComponentParameterStructure);
            }

            // if the input number of bits per sample is changed, the setParameter command is accepted only if we are in loaded state. If we are in other states; components are already instantiated
            OMX_AUDIO_PARAM_PCMMODETYPE &actualPcmSettings = getPcmSettings();
            if( actualPcmSettings.nBitPerSample == pcmsettings->nBitPerSample){
                return AFM_PcmPort::setParameter(nParamIndex, pComponentParameterStructure);  
            } else {
                OMX_STATETYPE state;
                OMX_ERRORTYPE error;
                ENS_Component&    myENSComponent=getENSComponent();

                error = myENSComponent.GetState(&myENSComponent,&state);
                if (error != OMX_ErrorNone) return error;

                if(state!= OMX_StateLoaded)
                {
                    OMX_BOOL hasOutputEnabled = OMX_FALSE;

                    for(OMX_U32 idx=1; idx<myENSComponent.getPortCount(); idx++) {
                        if(((ENS_Port *) myENSComponent.getPort(idx))->isEnabled()){
                            hasOutputEnabled = OMX_TRUE;
                        }
                    } 

                    if(hasOutputEnabled == OMX_TRUE){
                        return OMX_ErrorInvalidState;
                    }
                    else{
                        return AFM_PcmPort::setParameter(nParamIndex, pComponentParameterStructure);  
                    }
                }
            }
            
        }

        default:
            return AFM_PcmPort::setParameter(nParamIndex, pComponentParameterStructure);
    }
}

OMX_ERRORTYPE PcmSplitter_PcmPort::getParameter(
    OMX_INDEXTYPE nParamIndex,
    OMX_PTR pComponentParameterStructure) const
{
    switch (nParamIndex) {
        case OMX_IndexParamAudioSynchronized:
            {
                CHECK_STRUCT_SIZE_AND_VERSION(pComponentParameterStructure,OMX_SYMBIAN_AUDIO_PARAM_SYNCHRONIZEDTYPE);
                OMX_SYMBIAN_AUDIO_PARAM_SYNCHRONIZEDTYPE *pParams =
                    (OMX_SYMBIAN_AUDIO_PARAM_SYNCHRONIZEDTYPE *) pComponentParameterStructure;

                pParams->bIsSynchronized = mIsSynchronized;

                return OMX_ErrorNone;
            }

        default:
            return AFM_PcmPort::getParameter(
                    nParamIndex, pComponentParameterStructure);
    }
}


OMX_ERRORTYPE PcmSplitter_PcmPort::checkPcmSettings(
        const OMX_AUDIO_PARAM_PCMMODETYPE &pcmSettings) const
{
    switch (pcmSettings.nSamplingRate) {
        case 8000:
        case 11025:
        case 12000:
        case 16000:
        case 22050:
        case 24000:
        case 32000:
        case 44100:
        case 48000:
            break;
        default:
            return OMX_ErrorUnsupportedSetting;
    }

    if (pcmSettings.nChannels != 1 && pcmSettings.nChannels != 2) {
        return OMX_ErrorUnsupportedSetting;
    }

    if (getPortIndex() != 0) {
	  checkChannelSwitchBoardConfig(mChannelSwitchBoard);
    }

    return OMX_ErrorNone;
}
