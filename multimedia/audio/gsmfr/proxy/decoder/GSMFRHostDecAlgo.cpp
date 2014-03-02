/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "GSMFRHostDecAlgo.h"
#include "gsmfr/nmfil/host/composite/decalgo.hpp"
#include "GSMFRDec_BitStreamPort.h"

OMX_ERRORTYPE GSMFRHostDecAlgo::instantiate(OMX_U32 domainId, OMX_U32 priority) {
    t_nmf_error   nmf_error;

    setNMFDomainHandle(domainId); // Unused here
    mMyPriority = priority; // Unused here (No way to set priority at executing state in NMF 1.8)
    
    mNmfMain = (void*)gsmfr_nmfil_host_composite_decalgoCreate();
    if (((NMF::Composite*)mNmfMain) == NULL) { return OMX_ErrorInsufficientResources; };
    ((gsmfr_nmfil_host_composite_decalgo *)(mNmfMain))->priority = mMyPriority;
    if (((NMF::Composite*)mNmfMain)->construct() != NMF_OK) return OMX_ErrorInsufficientResources;
    
    nmf_error = ((NMF::Composite*)mNmfMain)->getInterface("configure", &mIConfig);
    if (nmf_error != NMF_OK) return OMX_ErrorInsufficientResources;

    return OMX_ErrorNone;
}

OMX_ERRORTYPE GSMFRHostDecAlgo::deInstantiate() {

    
    if (((NMF::Composite*)mNmfMain)->destroy() != NMF_OK) return OMX_ErrorInsufficientResources;
    
    gsmfr_nmfil_host_composite_decalgoDestroy((gsmfr_nmfil_host_composite_decalgo*&)mNmfMain);

    return OMX_ErrorNone;
}

inline OMX_ERRORTYPE GSMFRHostDecAlgo::start() {

    ((NMF::Composite*)mNmfMain)->start();
    return OMX_ErrorNone;
}

inline OMX_ERRORTYPE GSMFRHostDecAlgo::stop() {

    ((NMF::Composite*)mNmfMain)->stop();
    return OMX_ErrorNone;
}

OMX_ERRORTYPE GSMFRHostDecAlgo::setParameter(NmfGsmfrDecParams_t gsmfrNmfParams) {
    mIConfig.setParameter(gsmfrNmfParams);
    return OMX_ErrorNone;
}
OMX_ERRORTYPE GSMFRHostDecAlgo::setConfig(NmfGsmfrDecConfig_t gsmfrNmfConfig) {
    mIConfig.setConfig(gsmfrNmfConfig);
    return OMX_ErrorNone;
}
