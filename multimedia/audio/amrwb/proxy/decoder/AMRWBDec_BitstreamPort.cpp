/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#include "AMRWBDec_BitstreamPort.h"
#include "AFM_Types.h"
#include "AFM_Index.h"
#include "AFM_PcmPort.h"

AMRWBDec_BitstreamPort::AMRWBDec_BitstreamPort(
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
AMRWBDec_BitstreamPort::checkFormatCompatibility(
        OMX_HANDLETYPE hTunneledComp, 
        OMX_U32 nTunneledPort) const
{
    return OMX_ErrorNone;
    
}

OMX_ERRORTYPE AMRWBDec_BitstreamPort::CheckOMXParams(OMX_AUDIO_PARAM_AMRTYPE *amrwbParams)
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

OMX_ERRORTYPE AMRWBDec_BitstreamPort::setParameter(
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
                AFM_PcmPort *outputPort = (AFM_PcmPort *) getENSComponent().getPort(1);

#ifdef ANDRO
                amrwbParams->eAMRFrameFormat = OMX_AUDIO_AMRFrameFormatRTPPayload;
#endif

                error = CheckOMXParams(amrwbParams);
                if (error != OMX_ErrorNone) return error;
 
                mAmrwbParams      = *amrwbParams;
                return outputPort->updatePcmSettings(1, 16000);
            }

        default:
            return AFM_Port::setParameter(
                    nParamIndex, pComponentParameterStructure);
    }
}

OMX_ERRORTYPE AMRWBDec_BitstreamPort::getParameter(
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

OMX_ERRORTYPE AMRWBDec_BitstreamPort::updateAmrSettings(
        OMX_AUDIO_PARAM_AMRTYPE &amrwbParams)
{
    OMX_ERRORTYPE error;

    if ( (mAmrwbParams.nChannels       == amrwbParams.nChannels) &&
         (mAmrwbParams.nBitRate        == amrwbParams.nBitRate) &&
         (mAmrwbParams.eAMRBandMode    == amrwbParams.eAMRBandMode) &&
         (mAmrwbParams.eAMRDTXMode     == amrwbParams.eAMRDTXMode) &&
         (mAmrwbParams.eAMRFrameFormat == amrwbParams.eAMRFrameFormat) ) {

        return OMX_ErrorNone;
    }

    mAmrwbParams = amrwbParams;

    error = updateSettings(OMX_IndexParamAudioAmr, &mAmrwbParams);
    if (error != OMX_ErrorNone) return error; 

    return OMX_ErrorNone;        
}


