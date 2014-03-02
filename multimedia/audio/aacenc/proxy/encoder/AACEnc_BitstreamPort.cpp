/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "AACEnc_BitstreamPort.h"
#include "AFM_Types.h"
#include "AFM_Index.h"
#include "AFM_PcmPort.h"

AACEnc_BitstreamPort::AACEnc_BitstreamPort(
        OMX_U32 nIndex, 
        OMX_DIRTYPE eDir,
        OMX_U32 nBufferSizeMin,
        const OMX_AUDIO_PARAM_AACPROFILETYPE &defaultAacSettings,
        ENS_Component &enscomp) : AFM_Port( nIndex, eDir, nBufferSizeMin, OMX_AUDIO_CodingAAC, enscomp), mAacParams(defaultAacSettings)
{
}

OMX_ERRORTYPE 
AACEnc_BitstreamPort::checkFormatCompatibility(
        OMX_HANDLETYPE hTunneledComp, 
        OMX_U32 nTunneledPort) const
{
    return OMX_ErrorNone;
    
}

void AACEnc_BitstreamPort::checkInformationGivenByPcmPort(OMX_AUDIO_PARAM_PCMMODETYPE pcmformat)
{
    OMX_BOOL InfoModified = OMX_FALSE;

    if (pcmformat.nChannels != mAacParams.nChannels){
        mAacParams.nChannels = pcmformat.nChannels;
        InfoModified = OMX_TRUE;
    }

    if (InfoModified == OMX_TRUE){
        getENSComponent().eventHandlerCB(
                OMX_EventPortSettingsChanged, 0, 0, 0);
    }
}

OMX_ERRORTYPE AACEnc_BitstreamPort::CheckOMXParams(OMX_AUDIO_PARAM_AACPROFILETYPE *aacParams)
{
  /*  if (aacParams->nChannels != 1) return OMX_ErrorBadParameter;
    if (aacParams->eAACBandMode < OMX_AUDIO_AACBandModeNB0 || aacParams->eAACBandMode > OMX_AUDIO_AACBandModeNB7) return OMX_ErrorBadParameter;
    if (aacParams->eAACDTXMode & ~1) return OMX_ErrorBadParameter; // Only support DTFModeOff (0) an DTXModeOnVAD1 (1)
    if (aacParams->eAACFrameFormat == OMX_AUDIO_AACFrameFormatIF2 || aacParams->eAACFrameFormat == OMX_AUDIO_AACFrameFormatFSF ||
            aacParams->eAACFrameFormat == OMX_AUDIO_AACFrameFormatRTPPayload)*/
        return OMX_ErrorNone;
   // return OMX_ErrorBadParameter;
}

OMX_ERRORTYPE AACEnc_BitstreamPort::setParameter(
        OMX_INDEXTYPE nParamIndex,
        OMX_PTR pComponentParameterStructure)
{
    OMX_ERRORTYPE error;
    switch (nParamIndex) {
        case OMX_IndexParamAudioAac:
            {
                CHECK_STRUCT_SIZE_AND_VERSION(pComponentParameterStructure,OMX_AUDIO_PARAM_AACPROFILETYPE);
                OMX_AUDIO_PARAM_AACPROFILETYPE *aacParams = (OMX_AUDIO_PARAM_AACPROFILETYPE *)pComponentParameterStructure;
                error = CheckOMXParams(aacParams);
                if (error != OMX_ErrorNone) return error;
                mAacParams      = *aacParams;
                return OMX_ErrorNone;
            }

        default:
            return AFM_Port::setParameter(
                    nParamIndex, pComponentParameterStructure);
    }
}

OMX_ERRORTYPE AACEnc_BitstreamPort::getParameter(
        OMX_INDEXTYPE nParamIndex,
        OMX_PTR pComponentParameterStructure) const
{
    switch (nParamIndex) {
        case OMX_IndexParamAudioAac:
            {
                CHECK_STRUCT_SIZE_AND_VERSION(pComponentParameterStructure,OMX_AUDIO_PARAM_AACPROFILETYPE);
                OMX_AUDIO_PARAM_AACPROFILETYPE *aacparams = (OMX_AUDIO_PARAM_AACPROFILETYPE *)pComponentParameterStructure;
                *aacparams      = mAacParams;
                return OMX_ErrorNone;
            }

        default:
            return AFM_Port::getParameter(nParamIndex, pComponentParameterStructure);
    }
}

