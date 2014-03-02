/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "GSMHRHostEncAlgo.h"
#include "gsmhr/nmfil/host/composite/encalgo.hpp"
#include "GSMHREnc_BitstreamPort.h"

OMX_ERRORTYPE GSMHRHostEncAlgo::instantiate(OMX_U32 domainId, OMX_U32 priority) {
    
    t_nmf_error   nmf_error;

    setNMFDomainHandle(domainId); // Unused here
    mMyPriority = priority; // Unused here (No way to set priority at executing state in NMF 1.8)

    mNmfMain = (void*)gsmhr_nmfil_host_composite_encalgoCreate();
    if (((NMF::Composite*)mNmfMain) == NULL) { return OMX_ErrorInsufficientResources; };
    ((gsmhr_nmfil_host_composite_encalgo *)(mNmfMain))->priority = mMyPriority;
    if (((NMF::Composite*)mNmfMain)->construct() != NMF_OK) return OMX_ErrorInsufficientResources;
    
    nmf_error = ((NMF::Composite*)mNmfMain)->getInterface("configure", &mIConfig);
    if (nmf_error != NMF_OK) return OMX_ErrorInsufficientResources;

    return OMX_ErrorNone;
}

OMX_ERRORTYPE GSMHRHostEncAlgo::deInstantiate() {

    
    if (((NMF::Composite*)mNmfMain)->destroy() != NMF_OK) return OMX_ErrorInsufficientResources;
    
    gsmhr_nmfil_host_composite_encalgoDestroy((gsmhr_nmfil_host_composite_encalgo*&)(mNmfMain));

    return OMX_ErrorNone;
}

inline OMX_ERRORTYPE GSMHRHostEncAlgo::start() {

    ((NMF::Composite*)mNmfMain)->start();
    return OMX_ErrorNone;
}

inline OMX_ERRORTYPE GSMHRHostEncAlgo::stop() {

    ((NMF::Composite*)mNmfMain)->stop();
    return OMX_ErrorNone;
}

OMX_ERRORTYPE GSMHRHostEncAlgo::setParameter(GsmhrEncParams_t gsmhrNmfParams) {

    mIConfig.setParameter(gsmhrNmfParams);
    return OMX_ErrorNone;
}
OMX_ERRORTYPE GSMHRHostEncAlgo::setConfig(GsmhrEncConfig_t gsmhrNmfConfig) {
    mIConfig.setConfig(gsmhrNmfConfig);
    return OMX_ErrorNone;
}

