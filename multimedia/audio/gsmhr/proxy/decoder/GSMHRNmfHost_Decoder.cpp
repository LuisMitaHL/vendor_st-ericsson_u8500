/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "GSMHRNmfHost_Decoder.h"
#include "GSMHRDec_BitstreamPort.h"
#include "AFM_PcmPort.h"

inline OMX_ERRORTYPE
GSMHRNmfHost_Decoder::construct(void)
{
    mAlgo = (GSMHRHostDecAlgo*) new GSMHRHostDecAlgo();
    if (mAlgo == 0) {
      return OMX_ErrorInsufficientResources;
    }
    
    return (AFMNmfHost_Decoder::construct());
}

inline OMX_ERRORTYPE 
GSMHRNmfHost_Decoder::destroy(void)
{
    return (AFMNmfHost_Decoder::destroy());
}

OMX_ERRORTYPE 
GSMHRNmfHost_Decoder::instantiateAlgo(void) {

    OMX_ERRORTYPE error;
    error = mAlgo->instantiate(0, NULL);
    
    mNmfil = (NMF::Composite*) mAlgo->getNmfHandle();

    return error;
}

inline OMX_ERRORTYPE 
GSMHRNmfHost_Decoder::deInstantiateAlgo(void) {

  OMX_ERRORTYPE error = mAlgo->deInstantiate();

  delete mAlgo;

  return error;
}

inline OMX_ERRORTYPE 
GSMHRNmfHost_Decoder::startAlgo(void) {
    return (mAlgo->start());
}

inline OMX_ERRORTYPE 
GSMHRNmfHost_Decoder::stopAlgo(void) {
    return(mAlgo->stop());
}

OMX_ERRORTYPE 
GSMHRNmfHost_Decoder::configureAlgo(void) {
    
    OMX_ERRORTYPE error;
    GsmhrDecParams_t gsmhrNmfParams;
    GsmhrDecConfig_t gsmhrNmfConfig;
    OMX_AUDIO_PARAM_GSMHRTYPE gsmhrOmxParams;

    GSMHRDec_BitstreamPort * BSPort = static_cast<GSMHRDec_BitstreamPort *>(mENSComponent.getPort(0));

    gsmhrOmxParams.nSize           = sizeof(OMX_AUDIO_PARAM_GSMHRTYPE);
    getOmxIlSpecVersion(&gsmhrOmxParams.nVersion);

    error = BSPort->getParameter(OMX_IndexParamAudioGsm_HR, &gsmhrOmxParams);
    if (error != OMX_ErrorNone) return error;

    gsmhrNmfParams.memory_preset     = (t_memory_preset)0;
    gsmhrNmfConfig.epayload_format   = HR_MB_MODEM_PAYLOAD;//default MB payload

    error = mAlgo->setParameter(gsmhrNmfParams);
    error = mAlgo->setConfig(gsmhrNmfConfig); //not using setConfig from OMX client 
    
    if (error != OMX_ErrorNone) return error;
    return OMX_ErrorNone;

    //return (mAlgo->setParameter(gsmhrNmfParams));
}

