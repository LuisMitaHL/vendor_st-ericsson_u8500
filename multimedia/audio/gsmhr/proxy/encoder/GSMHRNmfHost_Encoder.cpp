/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "GSMHRNmfHost_Encoder.h"
#include "GSMHREnc_BitstreamPort.h"
//#include "AFM_PcmPort.h"

inline OMX_ERRORTYPE
GSMHRNmfHost_Encoder::construct(void)
{
    mAlgo = (GSMHRHostEncAlgo*) new GSMHRHostEncAlgo();
    if (mAlgo == 0) {
      return OMX_ErrorInsufficientResources;
    }
    
    return (AFMNmfHost_Encoder::construct());
}

inline OMX_ERRORTYPE 
GSMHRNmfHost_Encoder::destroy(void)
{
    return (AFMNmfHost_Encoder::destroy());
}

OMX_ERRORTYPE 
GSMHRNmfHost_Encoder::instantiateAlgo(void) {

    OMX_ERRORTYPE error;
    error = mAlgo->instantiate(0, NULL);
    
    mNmfil = (NMF::Composite*) mAlgo->getNmfHandle();

    return error;
}

inline OMX_ERRORTYPE 
GSMHRNmfHost_Encoder::deInstantiateAlgo(void) {

  OMX_ERRORTYPE error = mAlgo->deInstantiate();

  delete mAlgo;

  return error;
}

inline OMX_ERRORTYPE 
GSMHRNmfHost_Encoder::startAlgo(void) {
    return (mAlgo->start());
}

inline OMX_ERRORTYPE 
GSMHRNmfHost_Encoder::stopAlgo(void) {
    return(mAlgo->stop());
}

OMX_ERRORTYPE 
GSMHRNmfHost_Encoder::configureAlgo(void) {
    
    OMX_ERRORTYPE error;
    GsmhrEncParams_t gsmhrNmfParams;
    GsmhrEncConfig_t gsmhrNmfConfig;
    OMX_AUDIO_PARAM_GSMHRTYPE gsmhrOmxParams;

    GSMHREnc_BitstreamPort * BSPort = static_cast<GSMHREnc_BitstreamPort *>(mENSComponent.getPort(1));

    gsmhrOmxParams.nSize           = sizeof(OMX_AUDIO_PARAM_GSMHRTYPE);
    getOmxIlSpecVersion(&gsmhrOmxParams.nVersion);

    error = BSPort->getParameter(OMX_IndexParamAudioGsm_HR, &gsmhrOmxParams);
    if (error != OMX_ErrorNone) return error;

    gsmhrNmfConfig.bDtx         = (t_uint16) gsmhrOmxParams.bDTX;
    gsmhrNmfParams.memory_preset     = (t_memory_preset)0;
    gsmhrNmfConfig.epayload_format   = HR_MB_MODEM_PAYLOAD; //default for MB modem

    error = mAlgo->setParameter(gsmhrNmfParams);
    error = mAlgo->setConfig(gsmhrNmfConfig); //not using setConfig from OMX client 
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}
