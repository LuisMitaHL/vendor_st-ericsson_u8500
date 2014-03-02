/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "AMRWBNmfHost_Encoder.h"
#include "AMRWBEnc_BitstreamPort.h"

#include "audio_chipset_api_index.h"
#include "audio_codecs_chipset_api.h"
inline OMX_ERRORTYPE
AMRWBNmfHost_Encoder::construct(void)
{
    mAlgo = (AMRWBHostEncAlgo*) new AMRWBHostEncAlgo();
    if (mAlgo == 0) {
      return OMX_ErrorInsufficientResources;
    }
    
    return (AFMNmfHost_Encoder::construct());
}

inline OMX_ERRORTYPE 
AMRWBNmfHost_Encoder::destroy(void)
{
    return (AFMNmfHost_Encoder::destroy());
}

OMX_ERRORTYPE 
AMRWBNmfHost_Encoder::instantiateAlgo(void) {

    OMX_ERRORTYPE error;
    error = mAlgo->instantiate(0, NULL);
    
    mNmfil = (NMF::Composite*) mAlgo->getNmfHandle();

    return error;
}

inline OMX_ERRORTYPE 
AMRWBNmfHost_Encoder::deInstantiateAlgo(void) {

  OMX_ERRORTYPE error = mAlgo->deInstantiate();

  delete mAlgo;
  mAlgo = NULL;

  return error;
}

inline OMX_ERRORTYPE 
AMRWBNmfHost_Encoder::startAlgo(void) {
    return (mAlgo->start());
}

inline OMX_ERRORTYPE 
AMRWBNmfHost_Encoder::stopAlgo(void) {
    return(mAlgo->stop());
}

OMX_ERRORTYPE 
AMRWBNmfHost_Encoder::configureAlgo(void) {
    
    OMX_ERRORTYPE error;
    AmrwbEncParam_t amrwbNmfParams;
    AmrwbEncConfig_t amrwbNmfConfig;

    OMX_AUDIO_PARAM_AMRTYPE amrwbOmxParams;

    AMRWBEnc_BitstreamPort * BSPort = static_cast<AMRWBEnc_BitstreamPort *>(mENSComponent.getPort(1));

    amrwbOmxParams.nSize           = sizeof(OMX_AUDIO_PARAM_AMRTYPE);
    getOmxIlSpecVersion(&amrwbOmxParams.nVersion);

    error = BSPort->getParameter(OMX_IndexParamAudioAmr, &amrwbOmxParams);
    if (error != OMX_ErrorNone) return error;

    amrwbNmfParams.bNoHeader         = OMX_TRUE;
  
    switch(amrwbOmxParams.eAMRFrameFormat) {
        case OMX_AUDIO_AMRFrameFormatIF2:
            amrwbNmfConfig.ePayloadFormat = AMRWB_IF2_Payload;
            break;
        case OMX_AUDIO_AMRFrameFormatFSF:
            amrwbNmfConfig.ePayloadFormat = AMRWB_RFC3267_Payload; 
            amrwbNmfParams.bNoHeader = OMX_FALSE;    // assuming that generated bitstream will contain header 
            break;
        case OMX_AUDIO_AMRFrameFormatRTPPayload:
            amrwbNmfConfig.ePayloadFormat = AMRWB_RFC3267_Payload;
            break;
        default:
            return OMX_ErrorBadParameter; // mode not supported
    }
    error = mAlgo->setParameter(amrwbNmfParams);
    if (error != OMX_ErrorNone) return error;

    amrwbNmfConfig.nBitRate = amrwbOmxParams.eAMRBandMode - OMX_AUDIO_AMRBandModeWB0;
    amrwbNmfConfig.bDtxEnable = (amrwbOmxParams.eAMRDTXMode == OMX_AUDIO_AMRDTXModeOnVAD1) ? OMX_AUDIO_AMRDTXModeOnVAD1 : OMX_AUDIO_AMRDTXModeOff;

    return mAlgo->setConfig(amrwbNmfConfig);
}

OMX_ERRORTYPE AMRWBNmfHost_Encoder::applyConfig(OMX_INDEXTYPE nConfigIndex, OMX_PTR pComponentConfigStructure)
{
    switch (nConfigIndex) {
        case OMX_IndexConfigAudioAmrMode:
        {
            CHECK_STRUCT_SIZE_AND_VERSION(pComponentConfigStructure,OMX_AUDIO_CONFIG_AMRMODETYPE);
            OMX_AUDIO_CONFIG_AMRMODETYPE *amrwbConfig = (OMX_AUDIO_CONFIG_AMRMODETYPE *)pComponentConfigStructure;
            AMRWBEnc_BitstreamPort * BSPort = static_cast<AMRWBEnc_BitstreamPort *>(mENSComponent.getPort(1));
            OMX_AUDIO_AMRDTXMODETYPE Dtx = BSPort->getDtx();
            OMX_AUDIO_AMRFRAMEFORMATTYPE eAMRFrameFormat = BSPort->getFrameFormat();
            AmrwbEncConfig_t amrwbNmfConfig;

            if (Dtx & ~1) return OMX_ErrorBadParameter;
            if ((amrwbConfig->eAMRBandMode < OMX_AUDIO_AMRBandModeWB0) || (amrwbConfig->eAMRBandMode > OMX_AUDIO_AMRBandModeWB8)) {
                return OMX_ErrorBadParameter;
            }

            amrwbNmfConfig.nBitRate = amrwbConfig->eAMRBandMode - OMX_AUDIO_AMRBandModeWB0;
            amrwbNmfConfig.bDtxEnable = Dtx;
    switch(eAMRFrameFormat) {
        case OMX_AUDIO_AMRFrameFormatIF2:
            amrwbNmfConfig.ePayloadFormat = AMRWB_IF2_Payload;
            break;
        case OMX_AUDIO_AMRFrameFormatFSF:
            amrwbNmfConfig.ePayloadFormat = AMRWB_RFC3267_Payload; 
            break;
        case OMX_AUDIO_AMRFrameFormatRTPPayload:
            amrwbNmfConfig.ePayloadFormat = AMRWB_RFC3267_Payload;
            break;
        default:
            return OMX_ErrorBadParameter; // mode not supported
    }

            return mAlgo->setConfig(amrwbNmfConfig);
        }
        default:
            return AFMNmfHost_Encoder::applyConfig(
                    nConfigIndex, pComponentConfigStructure);
    }    
}


