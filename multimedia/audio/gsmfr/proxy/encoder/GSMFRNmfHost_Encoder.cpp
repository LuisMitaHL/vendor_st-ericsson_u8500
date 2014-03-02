/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "GSMFRNmfHost_Encoder.h"
#include "GSMFREnc_BitStreamPort.h"
//#include "AFM_PcmPort.h"

inline OMX_ERRORTYPE
GSMFRNmfHost_Encoder::construct(void)
{
    mAlgo = (GSMFRHostEncAlgo*) new GSMFRHostEncAlgo();
    if (mAlgo == 0) {
      return OMX_ErrorInsufficientResources;
    }
    
    return (AFMNmfHost_Encoder::construct());
}

inline OMX_ERRORTYPE 
GSMFRNmfHost_Encoder::destroy(void)
{
    return (AFMNmfHost_Encoder::destroy());
}

OMX_ERRORTYPE 
GSMFRNmfHost_Encoder::instantiateAlgo(void) {

    OMX_ERRORTYPE error;
    error = mAlgo->instantiate(0,0);
    
    mNmfil = (NMF::Composite*) mAlgo->getNmfHandle();

    return error;
}

inline OMX_ERRORTYPE 
GSMFRNmfHost_Encoder::deInstantiateAlgo(void) {

  OMX_ERRORTYPE error = mAlgo->deInstantiate();

  delete mAlgo;

  return error;
}

inline OMX_ERRORTYPE 
GSMFRNmfHost_Encoder::startAlgo(void) {
    return (mAlgo->start());
}

inline OMX_ERRORTYPE 
GSMFRNmfHost_Encoder::stopAlgo(void) {
    return(mAlgo->stop());
}

OMX_ERRORTYPE 
GSMFRNmfHost_Encoder::configureAlgo(void) {
    
    OMX_ERRORTYPE error;
    NmfGsmfrEncParams_t gsmfrNmfParams;
    NmfGsmfrEncConfig_t gsmfrNmfConfig;
    OMX_AUDIO_PARAM_GSMFRTYPE gsmfrOmxParams;

    GSMFREnc_BitstreamPort * BSPort = static_cast<GSMFREnc_BitstreamPort *>(mENSComponent.getPort(1));

    gsmfrOmxParams.nSize           = sizeof(OMX_AUDIO_PARAM_GSMFRTYPE);
    getOmxIlSpecVersion(&gsmfrOmxParams.nVersion);

    error = BSPort->getParameter(OMX_IndexParamAudioGsm_FR, &gsmfrOmxParams);
    if (error != OMX_ErrorNone) return error;

    gsmfrNmfConfig.bDtx              = gsmfrOmxParams.bDTX; //not used properly(as param not config) from OMX,fix me!
    gsmfrNmfConfig.epayload_format   = FR_MB_MODEM_PAYLOAD; //default for MB modem
    
    
    error = mAlgo->setParameter(gsmfrNmfParams);
    error = mAlgo->setConfig(gsmfrNmfConfig); //not using setConfig from OMX client 
    
    if (error != OMX_ErrorNone) return error;
    return OMX_ErrorNone;
    
    //return (mAlgo->setParameter(gsmfrNmfParams));
}
