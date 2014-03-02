/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "AMRWBHostDecAlgo.h"
#include "amrwb/nmfil/host/composite/decalgo.hpp"
#include "AMRWBDec_BitstreamPort.h"

OMX_ERRORTYPE AMRWBHostDecAlgo::instantiate(OMX_U32 domainId, OMX_U32 priority) {
    
    t_nmf_error   nmf_error;

    setNMFDomainHandle(domainId); // Unused here
    mMyPriority = priority; // Unused here (No way to set priority at executing state in NMF 1.8)
    
    mNmfMain = (void*)amrwb_nmfil_host_composite_decalgoCreate();
    if (((NMF::Composite*)mNmfMain) == NULL) { return OMX_ErrorInsufficientResources; };
    ((amrwb_nmfil_host_composite_decalgo *)(mNmfMain))->priority = mMyPriority;
    if (((NMF::Composite*)mNmfMain)->construct() != NMF_OK) return OMX_ErrorInsufficientResources;
    
    nmf_error = ((NMF::Composite*)mNmfMain)->getInterface("configure", &mIConfig);
    if (nmf_error != NMF_OK) return OMX_ErrorInsufficientResources;

    return OMX_ErrorNone;
}

OMX_ERRORTYPE AMRWBHostDecAlgo::deInstantiate() {

    if (((NMF::Composite*)mNmfMain)->destroy() != NMF_OK) return OMX_ErrorInsufficientResources;
    
    amrwb_nmfil_host_composite_decalgoDestroy((amrwb_nmfil_host_composite_decalgo* &)mNmfMain);

    return OMX_ErrorNone;
}

inline OMX_ERRORTYPE AMRWBHostDecAlgo::start() {

    ((NMF::Composite*)mNmfMain)->start();
    return OMX_ErrorNone;
}

inline OMX_ERRORTYPE AMRWBHostDecAlgo::stop() {

    ((NMF::Composite*)mNmfMain)->stop();
    return OMX_ErrorNone;
}

OMX_ERRORTYPE AMRWBHostDecAlgo::setParameter(AmrwbDecParams_t amrwbNmfParams) {
    mIConfig.setParameter(amrwbNmfParams);
    return OMX_ErrorNone;
}

OMX_ERRORTYPE AMRWBHostDecAlgo::setConfig(AmrwbDecConfig_t amrwbNmfConfig) {
    mIConfig.setConfig(amrwbNmfConfig);
    return OMX_ErrorNone;
}
