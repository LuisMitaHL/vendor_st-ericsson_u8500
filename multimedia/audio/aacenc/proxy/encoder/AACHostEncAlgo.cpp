/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "AACHostEncAlgo.h"
#include "aacenc/nmfil/host/composite/encalgo.hpp"
#include "AACEnc_BitstreamPort.h"

OMX_ERRORTYPE AACHostEncAlgo::instantiate(OMX_U32 domainId, OMX_U32 priority) {
    
    t_nmf_error   nmf_error;

    setNMFDomainHandle(domainId); // Unused here
    mMyPriority = priority; // Unused here (No way to set priority at executing state in NMF 1.8)
    
    mNmfMain = (void*)aacenc_nmfil_host_composite_encalgoCreate();
    if (((NMF::Composite*)mNmfMain) == NULL) { return OMX_ErrorInsufficientResources; };
    ((aacenc_nmfil_host_composite_encalgo *)(mNmfMain))->priority=mMyPriority;
    if (((NMF::Composite*)mNmfMain)->construct() != NMF_OK) return OMX_ErrorInsufficientResources;
    
    nmf_error = ((NMF::Composite*)mNmfMain)->getInterface("configure", &mIConfig);
    if (nmf_error != NMF_OK) return OMX_ErrorInsufficientResources;

    return OMX_ErrorNone;
}

OMX_ERRORTYPE AACHostEncAlgo::deInstantiate() {

    if (((NMF::Composite*)mNmfMain)->destroy() != NMF_OK) return OMX_ErrorInsufficientResources;
    
    aacenc_nmfil_host_composite_encalgoDestroy((aacenc_nmfil_host_composite_encalgo*&)(mNmfMain));

    return OMX_ErrorNone;
}

inline OMX_ERRORTYPE AACHostEncAlgo::start() {

    ((NMF::Composite*)mNmfMain)->start();
    return OMX_ErrorNone;
}

inline OMX_ERRORTYPE AACHostEncAlgo::stop() {

    ((NMF::Composite*)mNmfMain)->stop();
    return OMX_ErrorNone;
}

OMX_ERRORTYPE AACHostEncAlgo::setParameter(AacEncParam_t aacNmfParams) {
    mIConfig.setParameter(aacNmfParams);
    return OMX_ErrorNone;
}
/*
OMX_ERRORTYPE AACHostEncAlgo::setConfig(AacEncConfig_t aacNmfConfig) {
    mIConfig.setConfig(aacNmfConfig);
    return OMX_ErrorNone;
}
*/
