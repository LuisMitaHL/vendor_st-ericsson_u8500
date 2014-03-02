/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved

*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   AMR  Encoder nmf processing class
* \author  ST-Ericsson
*/
/*****************************************************************************/
#include "cm/inc/cm_macros.h"
#include "AMRNmfMpc_Encoder.h"
#include "ENS_Nmf.h"
#include "AMREncoder.h"
#include "AMREnc_BitstreamPort.h"
#include "AFM_nmf.h"

#include "audio_chipset_api_index.h"
#include "audio_codecs_chipset_api.h"

inline OMX_ERRORTYPE
AMRNmfMpc_Encoder::construct(void)
{
    mAlgo = (AMREncAlgo*) new AMREncAlgo();
    if (mAlgo == 0) {
        return OMX_ErrorInsufficientResources;
    }

    registerStubsAndSkels();
        
    return AFMNmfMpc_Encoder::construct();
}

inline OMX_ERRORTYPE 
AMRNmfMpc_Encoder::destroy(void)
{
    OMX_ERRORTYPE error = AFMNmfMpc_Encoder::destroy();
    if (error != OMX_ErrorNone) return error;

    unregisterStubsAndSkels();

    return OMX_ErrorNone;
}

void AMRNmfMpc_Encoder::registerStubsAndSkels(void)
{
    CM_REGISTER_STUBS(amr_cpp);
}

void AMRNmfMpc_Encoder::unregisterStubsAndSkels(void)
{
    CM_UNREGISTER_STUBS(amr_cpp);
}

OMX_ERRORTYPE 
AMRNmfMpc_Encoder::instantiateAlgo(void) {

    OMX_ERRORTYPE error;
    error = mAlgo->instantiate(mENSComponent.getNMFDomainHandle(), getPriorityLevel());
    
    mNmfil = *((t_cm_instance_handle*)mAlgo->getNmfHandle());

    return error;
}

inline OMX_ERRORTYPE 
AMRNmfMpc_Encoder::deInstantiateAlgo(void) {

  OMX_ERRORTYPE error = mAlgo->deInstantiate();

  delete mAlgo;

  return error;
}

inline OMX_ERRORTYPE 
AMRNmfMpc_Encoder::startAlgo(void) {
    return (mAlgo->start());
}

inline OMX_ERRORTYPE 
AMRNmfMpc_Encoder::stopAlgo(void) {
    return(mAlgo->stop());
}

OMX_ERRORTYPE 
AMRNmfMpc_Encoder::configureAlgo(void) {
    
    OMX_ERRORTYPE error;
    AmrEncParam_t amrNmfParams;
    AmrEncConfig_t amrNmfConfig;

    OMX_AUDIO_PARAM_AMRTYPE amrOmxParams;

    AMREnc_BitstreamPort * BSPort = static_cast<AMREnc_BitstreamPort *>(mENSComponent.getPort(1));
    
    amrOmxParams.nSize           = sizeof(OMX_AUDIO_PARAM_AMRTYPE);
    getOmxIlSpecVersion(&amrOmxParams.nVersion);

    error = BSPort->getParameter(OMX_IndexParamAudioAmr, &amrOmxParams);
    if (error != OMX_ErrorNone) return error;

    amrNmfParams.bNoHeader         = OMX_TRUE;
    amrNmfParams.memory_preset     = (t_memory_preset)(mENSComponent.getMemoryPreset());

    amrNmfConfig.nBitRate = amrOmxParams.eAMRBandMode - OMX_AUDIO_AMRBandModeNB0;
    amrNmfConfig.bDtxEnable = (amrOmxParams.eAMRDTXMode != OMX_AUDIO_AMRDTXModeOff)? OMX_AUDIO_AMRDTXModeOnVAD1:OMX_AUDIO_AMRDTXModeOff;
    amrNmfConfig.bEfr_on           = OMX_FALSE;
    switch(amrOmxParams.eAMRFrameFormat) {
        case OMX_AUDIO_AMRFrameFormatIF2:
            amrNmfConfig.ePayloadFormat = AMR_IF2_Payload;
            break;
        case OMX_AUDIO_AMRFrameFormatFSF:
            amrNmfConfig.ePayloadFormat = AMR_RFC3267_Payload; 
            amrNmfParams.bNoHeader = OMX_FALSE;
            break;
        case OMX_AUDIO_AMRFrameFormatRTPPayload:
            amrNmfConfig.ePayloadFormat = AMR_RFC3267_Payload;
            break;
        default:
            return OMX_ErrorBadParameter; // mode not supported
    }

    error = mAlgo->setParameter(amrNmfParams);
    if (error != OMX_ErrorNone) return error;

    error = mAlgo->setConfig(amrNmfConfig);
    if (error != OMX_ErrorNone) return error;

    return mAlgo->configure();
}


OMX_ERRORTYPE AMRNmfMpc_Encoder::applyConfig(OMX_INDEXTYPE nConfigIndex, OMX_PTR pComponentConfigStructure)
{
    switch (nConfigIndex) {
        case OMX_IndexConfigAudioAmrMode:
        {
            CHECK_STRUCT_SIZE_AND_VERSION(pComponentConfigStructure,OMX_AUDIO_CONFIG_AMRMODETYPE);
            OMX_AUDIO_CONFIG_AMRMODETYPE *amrConfig = (OMX_AUDIO_CONFIG_AMRMODETYPE *)pComponentConfigStructure;
            AMREnc_BitstreamPort * BSPort = static_cast<AMREnc_BitstreamPort *>(mENSComponent.getPort(1));
            OMX_AUDIO_AMRDTXMODETYPE Dtx = BSPort->getDtx();
            OMX_AUDIO_AMRFRAMEFORMATTYPE eAMRFrameFormat = BSPort->getFrameFormat();
            AmrEncConfig_t amrNmfConfig;

            if (Dtx & ~1) return OMX_ErrorBadParameter;
            if ((amrConfig->eAMRBandMode < OMX_AUDIO_AMRBandModeNB0) || (amrConfig->eAMRBandMode > OMX_AUDIO_AMRBandModeNB7)) {
                return OMX_ErrorBadParameter;
            }

            amrNmfConfig.nBitRate = amrConfig->eAMRBandMode - OMX_AUDIO_AMRBandModeNB0;
            amrNmfConfig.bDtxEnable = Dtx;
            amrNmfConfig.bEfr_on    = OMX_FALSE;
            switch(eAMRFrameFormat) {
                case OMX_AUDIO_AMRFrameFormatIF2:
                    amrNmfConfig.ePayloadFormat = AMR_IF2_Payload;
                    break;
                case OMX_AUDIO_AMRFrameFormatFSF:
                    amrNmfConfig.ePayloadFormat = AMR_RFC3267_Payload; 
                    break;
                case OMX_AUDIO_AMRFrameFormatRTPPayload:
                    amrNmfConfig.ePayloadFormat = AMR_RFC3267_Payload;
                    break;
                default:
                    return OMX_ErrorBadParameter; // mode not supported
            }

            return mAlgo->setConfig(amrNmfConfig);
        }
        default:
            return AFMNmfMpc_Encoder::applyConfig(
                    nConfigIndex, pComponentConfigStructure);
    }    
}

