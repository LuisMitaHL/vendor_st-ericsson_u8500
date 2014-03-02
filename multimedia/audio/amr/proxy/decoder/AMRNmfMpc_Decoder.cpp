/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved

*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   AMR  Decoder nmf processing class
* \author  ST-Ericsson
*/
/*****************************************************************************/
#include "cm/inc/cm_macros.h"
#include "AMRNmfMpc_Decoder.h"
#include "ENS_Nmf.h"
#include "AMRDecoder.h"
#include "AMRDec_BitstreamPort.h"
#include "AFM_nmf.h"

inline OMX_ERRORTYPE
AMRNmfMpc_Decoder::construct(void)
{
    mAlgo = (AMRDecAlgo*) new AMRDecAlgo();
    if (mAlgo == 0) {
        return OMX_ErrorInsufficientResources;
    }

    registerStubsAndSkels();
        
    return AFMNmfMpc_Decoder::construct();
}

inline OMX_ERRORTYPE 
AMRNmfMpc_Decoder::destroy(void)
{
    OMX_ERRORTYPE error = AFMNmfMpc_Decoder::destroy();
    if (error != OMX_ErrorNone) return error;

    unregisterStubsAndSkels();

    return OMX_ErrorNone;
}

void AMRNmfMpc_Decoder::registerStubsAndSkels(void)
{
    CM_REGISTER_STUBS(amr_cpp);
}

void AMRNmfMpc_Decoder::unregisterStubsAndSkels(void)
{
    CM_UNREGISTER_STUBS(amr_cpp);
}


OMX_ERRORTYPE 
AMRNmfMpc_Decoder::instantiateAlgo(void) {

    OMX_ERRORTYPE error;
    error = mAlgo->instantiate(mENSComponent.getNMFDomainHandle(),getPriorityLevel());
    
    mNmfil = *((t_cm_instance_handle*)mAlgo->getNmfHandle());

    return error;
}

inline OMX_ERRORTYPE 
AMRNmfMpc_Decoder::deInstantiateAlgo(void) {

  OMX_ERRORTYPE error = mAlgo->deInstantiate();

  delete mAlgo;

  return error;
}

inline OMX_ERRORTYPE 
AMRNmfMpc_Decoder::startAlgo(void) {
    return (mAlgo->start());
}

inline OMX_ERRORTYPE 
AMRNmfMpc_Decoder::stopAlgo(void) {
    return(mAlgo->stop());
}

OMX_ERRORTYPE 
AMRNmfMpc_Decoder::configureAlgo(void) {
    
    OMX_ERRORTYPE error;
    AmrDecParams_t amrNmfParams;
    AmrDecConfig_t amrNmfConfig;
    OMX_AUDIO_PARAM_AMRTYPE amrOmxParams;

    AFM_PcmPort *pcmPort = static_cast<AFM_PcmPort *>(mENSComponent.getPort(1));
    AMRDec_BitstreamPort * BSPort = static_cast<AMRDec_BitstreamPort *>(mENSComponent.getPort(0));

    amrOmxParams.nSize           = sizeof(OMX_AUDIO_PARAM_AMRTYPE);
    getOmxIlSpecVersion(&amrOmxParams.nVersion);
    error = BSPort->getParameter(OMX_IndexParamAudioAmr, &amrOmxParams);
    if (error != OMX_ErrorNone) return error;

    amrNmfParams.bNoHeader         = OMX_TRUE;
    amrNmfParams.memory_preset     = (t_memory_preset)(mENSComponent.getMemoryPreset());
    amrNmfParams.bErrorConcealment = pcmPort->getFlagErrorConcealment();
    
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

