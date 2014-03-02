/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "GSMFRNmfHost_Decoder.h"
#include "GSMFRDec_BitStreamPort.h"
#include "AFM_PcmPort.h"

inline OMX_ERRORTYPE
GSMFRNmfHost_Decoder::construct(void)
{
    mAlgo = (GSMFRHostDecAlgo*) new GSMFRHostDecAlgo();
    if (mAlgo == 0) {
      return OMX_ErrorInsufficientResources;
    }
    
    return (AFMNmfHost_Decoder::construct());
}

inline OMX_ERRORTYPE 
GSMFRNmfHost_Decoder::destroy(void)
{
    return (AFMNmfHost_Decoder::destroy());
}

OMX_ERRORTYPE 
GSMFRNmfHost_Decoder::instantiateAlgo(void) {

    OMX_ERRORTYPE error;
    error = mAlgo->instantiate(0, 0);
    
    mNmfil = (NMF::Composite*) mAlgo->getNmfHandle();

    return error;
}

inline OMX_ERRORTYPE 
GSMFRNmfHost_Decoder::deInstantiateAlgo(void) {

  OMX_ERRORTYPE error = mAlgo->deInstantiate();

  delete mAlgo;

  return error;
}

inline OMX_ERRORTYPE 
GSMFRNmfHost_Decoder::startAlgo(void) {
    return (mAlgo->start());
}

inline OMX_ERRORTYPE 
GSMFRNmfHost_Decoder::stopAlgo(void) {
    return(mAlgo->stop());
}

OMX_ERRORTYPE 
GSMFRNmfHost_Decoder::configureAlgo(void) {
    
    OMX_ERRORTYPE error;
    NmfGsmfrDecParams_t gsmfrNmfParams;
    NmfGsmfrDecConfig_t gsmfrNmfConfig;
    OMX_AUDIO_PARAM_GSMFRTYPE gsmfrOmxParams;

    //AFM_PcmPort *pcmPort = static_cast<AFM_PcmPort *>(mENSComponent.getPort(1));
    GSMFRDec_BitstreamPort * BSPort = static_cast<GSMFRDec_BitstreamPort *>(mENSComponent.getPort(0));

    gsmfrOmxParams.nSize           = sizeof(OMX_AUDIO_PARAM_GSMFRTYPE);
    getOmxIlSpecVersion(&gsmfrOmxParams.nVersion);

    error = BSPort->getParameter(OMX_IndexParamAudioGsm_FR, &gsmfrOmxParams);
    if (error != OMX_ErrorNone) return error;

    gsmfrNmfConfig.bDtx              = gsmfrOmxParams.bDTX;//not used properly(as param not config) from OMX,fix me!
    gsmfrNmfParams.memory_preset     = (t_memory_preset)0;
    
    gsmfrNmfConfig.epayload_format   = FR_MB_MODEM_PAYLOAD;//default MB payload

    error = mAlgo->setParameter(gsmfrNmfParams);
    error = mAlgo->setConfig(gsmfrNmfConfig); //not using setConfig from OMX client 
    
    if (error != OMX_ErrorNone) return error;
    return OMX_ErrorNone;

    //return (mAlgo->setParameter(gsmfrNmfParams));
}
