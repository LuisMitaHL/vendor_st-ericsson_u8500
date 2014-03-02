/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "AMRWBEnc_BitstreamPort.h"
#include "AFM_Types.h"
#include "AFM_Index.h"
#include "AFM_PcmPort.h"

AMRWBEnc_BitstreamPort::AMRWBEnc_BitstreamPort(
        OMX_U32 nIndex, 
        OMX_DIRTYPE eDir,
        OMX_U32 nBufferSizeMin,
        const OMX_AUDIO_PARAM_AMRTYPE &defaultAmrSettings,
        ENS_Component &enscomp)
        : AFM_Port(
            nIndex, eDir, nBufferSizeMin, OMX_AUDIO_CodingAMR, enscomp),
          mAmrwbParams(defaultAmrSettings)
{
}

OMX_ERRORTYPE 
AMRWBEnc_BitstreamPort::checkFormatCompatibility(
        OMX_HANDLETYPE hTunneledComp, 
        OMX_U32 nTunneledPort) const
{
    return OMX_ErrorNone;
    
}

void AMRWBEnc_BitstreamPort::checkInformationGivenByPcmPort(OMX_AUDIO_PARAM_PCMMODETYPE pcmformat)
{
    OMX_BOOL InfoModified = OMX_FALSE;

    if (pcmformat.nChannels != mAmrwbParams.nChannels){
        mAmrwbParams.nChannels = pcmformat.nChannels;
        InfoModified = OMX_TRUE;
    }

    if (InfoModified == OMX_TRUE){
        getENSComponent().eventHandlerCB(
                OMX_EventPortSettingsChanged, 0, 0, 0);
    }
}

OMX_ERRORTYPE AMRWBEnc_BitstreamPort::CheckOMXParams(OMX_AUDIO_PARAM_AMRTYPE *amrwbParams)
{
    if (amrwbParams->nChannels != 1) return OMX_ErrorBadParameter;
    if (amrwbParams->eAMRBandMode < OMX_AUDIO_AMRBandModeWB0 || amrwbParams->eAMRBandMode > OMX_AUDIO_AMRBandModeWB8) return OMX_ErrorBadParameter;
    if (amrwbParams->eAMRDTXMode & ~1) return OMX_ErrorBadParameter; // Only support DTXModeOff (0) an DTXModeOnVAD1 (1)
    if (amrwbParams->eAMRFrameFormat == OMX_AUDIO_AMRFrameFormatIF2 ||
            amrwbParams->eAMRFrameFormat == OMX_AUDIO_AMRFrameFormatFSF ||
            amrwbParams->eAMRFrameFormat == OMX_AUDIO_AMRFrameFormatRTPPayload)
        return OMX_ErrorNone;
    return OMX_ErrorBadParameter;
}

OMX_ERRORTYPE AMRWBEnc_BitstreamPort::setParameter(
        OMX_INDEXTYPE nParamIndex,
        OMX_PTR pComponentParameterStructure)
{
    OMX_ERRORTYPE error;
    switch (nParamIndex) {
        case OMX_IndexParamAudioAmr:
            {
                CHECK_STRUCT_SIZE_AND_VERSION(pComponentParameterStructure,OMX_AUDIO_PARAM_AMRTYPE);
                OMX_AUDIO_PARAM_AMRTYPE *amrwbParams =
                    (OMX_AUDIO_PARAM_AMRTYPE *)pComponentParameterStructure;

#ifdef ANDRO
#ifdef FORCE_RTP_WHEN_FSF
                if ( amrwbParams->eAMRFrameFormat == OMX_AUDIO_AMRFrameFormatFSF ) {
                    amrwbParams->eAMRFrameFormat = OMX_AUDIO_AMRFrameFormatRTPPayload;
                }
#endif

                if((amrwbParams->eAMRFrameFormat == OMX_AUDIO_AMRFrameFormatFSF) || (amrwbParams->eAMRFrameFormat == OMX_AUDIO_AMRFrameFormatRTPPayload))
                {
                    amrwbParams->eAMRDTXMode = OMX_AUDIO_AMRDTXModeOff;
                }
#endif

                error = CheckOMXParams(amrwbParams);
                if (error != OMX_ErrorNone) return error;

                mAmrwbParams      = *amrwbParams;
                return OMX_ErrorNone;
            }

        default:
            return AFM_Port::setParameter(
                    nParamIndex, pComponentParameterStructure);
    }
}

OMX_ERRORTYPE AMRWBEnc_BitstreamPort::getParameter(
        OMX_INDEXTYPE nParamIndex,
        OMX_PTR pComponentParameterStructure) const
{
    switch (nParamIndex) {
        case OMX_IndexParamAudioAmr:
            {
                CHECK_STRUCT_SIZE_AND_VERSION(pComponentParameterStructure,OMX_AUDIO_PARAM_AMRTYPE);
                OMX_AUDIO_PARAM_AMRTYPE *amrwbparams =
                    (OMX_AUDIO_PARAM_AMRTYPE *)pComponentParameterStructure;

                *amrwbparams      = mAmrwbParams;

                return OMX_ErrorNone;
            }

        default:
            return AFM_Port::getParameter(
                    nParamIndex, pComponentParameterStructure);
    }
}

OMX_ERRORTYPE AMRWBEnc_BitstreamPort::setBitRate(OMX_AUDIO_AMRBANDMODETYPE OmxBandMode)
{
    if ((OmxBandMode < OMX_AUDIO_AMRBandModeWB0) || (OmxBandMode > OMX_AUDIO_AMRBandModeWB8)) return OMX_ErrorBadParameter;
    mAmrwbParams.eAMRBandMode = OmxBandMode;
    return OMX_ErrorNone;
}

OMX_AUDIO_AMRDTXMODETYPE AMRWBEnc_BitstreamPort::getDtx()
{
    return mAmrwbParams.eAMRDTXMode;
}
OMX_AUDIO_AMRFRAMEFORMATTYPE AMRWBEnc_BitstreamPort::getFrameFormat()
{
    return mAmrwbParams.eAMRFrameFormat;
}
