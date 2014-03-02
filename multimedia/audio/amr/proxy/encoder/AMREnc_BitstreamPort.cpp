/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved

*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   AMR Encoder Bitstream port
* \author  ST-Ericsson
*/
/*****************************************************************************/

#include "AMREnc_BitstreamPort.h"
#include "AFM_Types.h"
#include "AFM_Index.h"
#include "AFM_PcmPort.h"

AMREnc_BitstreamPort::AMREnc_BitstreamPort(
        OMX_U32 nIndex, 
        OMX_DIRTYPE eDir,
        OMX_U32 nBufferSizeMin,
        const OMX_AUDIO_PARAM_AMRTYPE &defaultAmrSettings,
        bool isHost,
        ENS_Component &enscomp)
        : AFM_Port(
            nIndex, eDir, nBufferSizeMin, OMX_AUDIO_CodingAMR, enscomp),
          mAmrParams(defaultAmrSettings)
{
    setSupportedProprietaryComm(isHost? AFM_HOST_PROPRIETARY_COMMUNICATION:AFM_MPC_PROPRIETARY_COMMUNICATION);
    //Now already done in AFM
    //setMpc(!isHost);
    //setPortPriorityLevel(OMX_PriorityBackground);
}

OMX_ERRORTYPE 
AMREnc_BitstreamPort::checkFormatCompatibility(
        OMX_HANDLETYPE hTunneledComp, 
        OMX_U32 nTunneledPort) const
{
    return OMX_ErrorNone;
    
}

void AMREnc_BitstreamPort::checkInformationGivenByPcmPort(OMX_AUDIO_PARAM_PCMMODETYPE pcmformat)
{
    OMX_BOOL InfoModified = OMX_FALSE;

    if (pcmformat.nChannels != mAmrParams.nChannels){
        mAmrParams.nChannels = pcmformat.nChannels;
        InfoModified = OMX_TRUE;
    }

    if (InfoModified == OMX_TRUE){
        getENSComponent().eventHandlerCB(
                OMX_EventPortSettingsChanged, 0, 0, 0);
    }
}

OMX_ERRORTYPE AMREnc_BitstreamPort::CheckOMXParams(OMX_AUDIO_PARAM_AMRTYPE *amrParams)
{
    if (amrParams->nChannels != 1) return OMX_ErrorBadParameter;
    if (amrParams->eAMRBandMode < OMX_AUDIO_AMRBandModeNB0 || amrParams->eAMRBandMode > OMX_AUDIO_AMRBandModeNB7) return OMX_ErrorBadParameter;
    if ((amrParams->eAMRDTXMode & ~1) && (amrParams->eAMRDTXMode != OMX_AUDIO_AMRDTXModeOnAuto)) return OMX_ErrorBadParameter; // Only support DTFModeOff (0) and DTXModeOnVAD1 (1)
    if (amrParams->eAMRFrameFormat == OMX_AUDIO_AMRFrameFormatIF2 ||
            amrParams->eAMRFrameFormat == OMX_AUDIO_AMRFrameFormatFSF ||
            amrParams->eAMRFrameFormat == OMX_AUDIO_AMRFrameFormatRTPPayload)
        return OMX_ErrorNone;
    return OMX_ErrorBadParameter;
}

OMX_ERRORTYPE AMREnc_BitstreamPort::setParameter(
        OMX_INDEXTYPE nParamIndex,
        OMX_PTR pComponentParameterStructure)
{
    OMX_ERRORTYPE error;
    switch (nParamIndex) {
        case OMX_IndexParamAudioAmr:
            {
                CHECK_STRUCT_SIZE_AND_VERSION(pComponentParameterStructure,OMX_AUDIO_PARAM_AMRTYPE);
                OMX_AUDIO_PARAM_AMRTYPE *amrParams =
                    (OMX_AUDIO_PARAM_AMRTYPE *)pComponentParameterStructure;

#ifdef FORCE_RTP_WHEN_FSF
                if ( amrParams->eAMRFrameFormat == OMX_AUDIO_AMRFrameFormatFSF ) {
                    amrParams->eAMRFrameFormat = OMX_AUDIO_AMRFrameFormatRTPPayload;
                }
#endif
#ifdef FORCE_DTXOFF
				if((amrParams->eAMRFrameFormat == OMX_AUDIO_AMRFrameFormatFSF) || (amrParams->eAMRFrameFormat == OMX_AUDIO_AMRFrameFormatRTPPayload)) 
				{
                amrParams->eAMRDTXMode = OMX_AUDIO_AMRDTXModeOff;
				}
#endif
                error = CheckOMXParams(amrParams);
                if (error != OMX_ErrorNone) return error;

                mAmrParams      = *amrParams;
                return OMX_ErrorNone;
            }

        default:
            return AFM_Port::setParameter(
                    nParamIndex, pComponentParameterStructure);
    }
}

OMX_ERRORTYPE AMREnc_BitstreamPort::getParameter(
        OMX_INDEXTYPE nParamIndex,
        OMX_PTR pComponentParameterStructure) const
{
    switch (nParamIndex) {
        case OMX_IndexParamAudioAmr:
            {
                CHECK_STRUCT_SIZE_AND_VERSION(pComponentParameterStructure,OMX_AUDIO_PARAM_AMRTYPE);
                OMX_AUDIO_PARAM_AMRTYPE *amrparams =
                    (OMX_AUDIO_PARAM_AMRTYPE *)pComponentParameterStructure;

                *amrparams      = mAmrParams;

                return OMX_ErrorNone;
            }

        default:
            return AFM_Port::getParameter(
                    nParamIndex, pComponentParameterStructure);
    }
}

OMX_ERRORTYPE AMREnc_BitstreamPort::setBitRate(OMX_AUDIO_AMRBANDMODETYPE OmxBandMode)
{
    if ((OmxBandMode < OMX_AUDIO_AMRBandModeNB0) || (OmxBandMode > OMX_AUDIO_AMRBandModeNB7)) return OMX_ErrorBadParameter;
    mAmrParams.eAMRBandMode = OmxBandMode;
    return OMX_ErrorNone;
}

OMX_AUDIO_AMRDTXMODETYPE AMREnc_BitstreamPort::getDtx()
{
    return mAmrParams.eAMRDTXMode;
}

OMX_AUDIO_AMRFRAMEFORMATTYPE AMREnc_BitstreamPort::getFrameFormat()
{
    return mAmrParams.eAMRFrameFormat;
}

