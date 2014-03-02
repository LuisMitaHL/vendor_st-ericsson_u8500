/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "AACNmfHost_Encoder.h"
#include "AACEnc_BitstreamPort.h"
//#include "AFM_PcmPort.h"

inline OMX_ERRORTYPE
AACNmfHost_Encoder::construct(void)
{
    mAlgo = (AACHostEncAlgo*) new AACHostEncAlgo();
    if (mAlgo == 0) {
      return OMX_ErrorInsufficientResources;
    }
    
    return (AFMNmfHost_Encoder::construct());
}

inline OMX_ERRORTYPE 
AACNmfHost_Encoder::destroy(void)
{
    return (AFMNmfHost_Encoder::destroy());
}

OMX_ERRORTYPE 
AACNmfHost_Encoder::instantiateAlgo(void) {

    OMX_ERRORTYPE error;
    error = mAlgo->instantiate(0, NULL);
    
    mNmfil = (NMF::Composite*) mAlgo->getNmfHandle();

    return error;
}

inline OMX_ERRORTYPE 
AACNmfHost_Encoder::deInstantiateAlgo(void) {

  OMX_ERRORTYPE error = mAlgo->deInstantiate();

  delete mAlgo;

  return error;
}

inline OMX_ERRORTYPE 
AACNmfHost_Encoder::startAlgo(void) {
    return (mAlgo->start());
}

inline OMX_ERRORTYPE 
AACNmfHost_Encoder::stopAlgo(void) {
    return(mAlgo->stop());
}

OMX_ERRORTYPE 
AACNmfHost_Encoder::configureAlgo(void) {
    
    OMX_ERRORTYPE error;
    AacEncParam_t aacNmfParams;
    //AacEncConfig_t aacNmfConfig;

    OMX_AUDIO_PARAM_AACPROFILETYPE aacOmxParams;

    AACEnc_BitstreamPort * BSPort = static_cast<AACEnc_BitstreamPort *>(mENSComponent.getPort(1));

    aacOmxParams.nSize           = sizeof(OMX_AUDIO_PARAM_AACPROFILETYPE);
    getOmxIlSpecVersion(&aacOmxParams.nVersion);

    error = BSPort->getParameter(OMX_IndexParamAudioAac, &aacOmxParams);
    if (error != OMX_ErrorNone) return error;
 
  /** Params parameters Set **/  
    aacNmfParams.nChannels            = aacOmxParams.nChannels;                                         
    aacNmfParams.sampleRate           = aacOmxParams.nSampleRate;                                       
    aacNmfParams.nBitRate             = aacOmxParams.nBitRate;  
    aacNmfParams.nBits						    = 16;                                       // need to be 16 bit    
   // aacNmfParams.eAacEncProfile       = (NMF_HEAAC_PROFILE)aacOmxParams.eAACProfile;   
    switch(aacOmxParams.eAACProfile) {
        case OMX_AUDIO_AACObjectLC:
            aacNmfParams.eAacEncProfile = NMF_HEAAC_PROFILE_AACLC;
            break;
        case OMX_AUDIO_AACObjectHE:
            aacNmfParams.eAacEncProfile = NMF_HEAAC_PROFILE_HE;
            break;            
        case OMX_AUDIO_AACObjectHE_PS:
            aacNmfParams.eAacEncProfile = NMF_HEAAC_PROFILE_HE_PS; 
            break;
        default:
            return OMX_ErrorBadParameter; // mode not supported
    }   
    
    //aacNmfParams.eAacEncBsFormat      = (NMF_HEAAC_BS_FORMAT)aacOmxParams.eAACStreamFormat;
   switch(aacOmxParams.eAACStreamFormat) {
        case OMX_AUDIO_AACStreamFormatMP2ADTS:
            aacNmfParams.eAacEncBsFormat = NMF_HEAAC_BSFORMAT_ADTS_MPEG2;
            break;
        case OMX_AUDIO_AACStreamFormatMP4ADTS:
            aacNmfParams.eAacEncBsFormat = NMF_HEAAC_BSFORMAT_ADTS_MPEG4;
            break;            
        case OMX_AUDIO_AACStreamFormatADIF:
            aacNmfParams.eAacEncBsFormat = NMF_HEAAC_BSFORMAT_ADIF; 
            break;
       case OMX_AUDIO_AACStreamFormatRAW:
       case OMX_AUDIO_AACStreamFormatMP4FF:
            aacNmfParams.eAacEncBsFormat = NMF_HEAAC_BSFORMAT_RAW; 
            break;            
        default:
            return OMX_ErrorBadParameter; // mode not supported
    }   
        
    //aacNmfParams.bEncodeMono			    = aacOmxParams.eChannelMode; 
   switch(aacOmxParams.eChannelMode) {
        case OMX_AUDIO_ChannelModeStereo:
            aacNmfParams.bEncodeMono = 0;
            break;
        case OMX_AUDIO_ChannelModeMono:
            aacNmfParams.bEncodeMono = 1;
            break;     
        default:
            return OMX_ErrorBadParameter; // mode not supported
    }     
    error = mAlgo->setParameter(aacNmfParams);
    //if (error != OMX_ErrorNone) return error;
    return error;  
    
    /** Config parameters Set **/   
    //aacNmfConfig.nBitRate   = aacOmxParams.nBitRate;   
    //return mAlgo->setConfig(aacNmfConfig);
}   
                                                                                                      
