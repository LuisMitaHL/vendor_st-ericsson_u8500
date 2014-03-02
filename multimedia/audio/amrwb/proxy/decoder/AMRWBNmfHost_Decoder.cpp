/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "AMRWBNmfHost_Decoder.h"
#include "AMRWBDec_BitstreamPort.h"
#include "AFM_PcmPort.h"

inline OMX_ERRORTYPE
AMRWBNmfHost_Decoder::construct(void)
{
    mAlgo = (AMRWBHostDecAlgo*) new AMRWBHostDecAlgo();
    if (mAlgo == 0) {
      return OMX_ErrorInsufficientResources;
    }
    
    return (AFMNmfHost_Decoder::construct());
}

inline OMX_ERRORTYPE 
AMRWBNmfHost_Decoder::destroy(void)
{
    return (AFMNmfHost_Decoder::destroy());
}

OMX_ERRORTYPE 
AMRWBNmfHost_Decoder::instantiateAlgo(void) {

    OMX_ERRORTYPE error;
    error = mAlgo->instantiate(0, NULL);
    
    mNmfil = (NMF::Composite*) mAlgo->getNmfHandle();

    return error;
}

inline OMX_ERRORTYPE 
AMRWBNmfHost_Decoder::deInstantiateAlgo(void) {

  OMX_ERRORTYPE error = mAlgo->deInstantiate();

  delete mAlgo;

  return error;
}

inline OMX_ERRORTYPE 
AMRWBNmfHost_Decoder::startAlgo(void) {
    return (mAlgo->start());
}

inline OMX_ERRORTYPE 
AMRWBNmfHost_Decoder::stopAlgo(void) {
    return(mAlgo->stop());
}

OMX_ERRORTYPE 
AMRWBNmfHost_Decoder::configureAlgo(void) {
    
    OMX_ERRORTYPE error;
    AmrwbDecParams_t amrwbNmfParams;
    AmrwbDecConfig_t amrwbNmfConfig;
    OMX_AUDIO_PARAM_AMRTYPE amrwbOmxParams;

    AFM_PcmPort *pcmPort = static_cast<AFM_PcmPort *>(mENSComponent.getPort(1));
    AMRWBDec_BitstreamPort * BSPort = static_cast<AMRWBDec_BitstreamPort *>(mENSComponent.getPort(0));

    amrwbOmxParams.nSize           = sizeof(OMX_AUDIO_PARAM_AMRTYPE);
    getOmxIlSpecVersion(&amrwbOmxParams.nVersion);

    error = BSPort->getParameter(OMX_IndexParamAudioAmr, &amrwbOmxParams);
    if (error != OMX_ErrorNone) return error;

    amrwbNmfParams.bNoHeader         = OMX_TRUE;
    amrwbNmfParams.bErrorConcealment = OMX_FALSE;
    amrwbNmfParams.bErrorConcealment = pcmPort->getFlagErrorConcealment();

    switch(amrwbOmxParams.eAMRFrameFormat) {
        case OMX_AUDIO_AMRFrameFormatIF2:
            amrwbNmfConfig.ePayloadFormat = AMRWB_IF2_Payload;
            break;
        case OMX_AUDIO_AMRFrameFormatFSF:
            amrwbNmfConfig.ePayloadFormat = AMRWB_RFC3267_Payload; 
            //amrwbNmfParams.bNoHeader = OMX_FALSE;    // assuming that input bitstream contains no header (temp fix)
            break;
        case OMX_AUDIO_AMRFrameFormatRTPPayload:
            amrwbNmfConfig.ePayloadFormat = AMRWB_RFC3267_Payload;
            break;
        default:
            return OMX_ErrorBadParameter; // mode not supported
    }

    error = mAlgo->setParameter(amrwbNmfParams);
    if (error != OMX_ErrorNone) return error;

    return (mAlgo->setConfig(amrwbNmfConfig));
}

