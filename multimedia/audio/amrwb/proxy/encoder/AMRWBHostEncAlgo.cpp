/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "AMRWBHostEncAlgo.h"
#include "amrwb/nmfil/host/composite/encalgo.hpp"
#include "AMRWBEnc_BitstreamPort.h"

OMX_ERRORTYPE AMRWBHostEncAlgo::instantiate(OMX_U32 domainId, OMX_U32 priority) {
    
    t_nmf_error   nmf_error;

    setNMFDomainHandle(domainId); // Unused here
    mMyPriority = priority; // Unused here (No way to set priority at executing state in NMF 1.8)
    
    mNmfMain = (void*)amrwb_nmfil_host_composite_encalgoCreate();
    if (((NMF::Composite*)mNmfMain) == NULL) { return OMX_ErrorInsufficientResources; };
    ((amrwb_nmfil_host_composite_encalgo *)(mNmfMain))->priority = mMyPriority;
    if (((NMF::Composite*)mNmfMain)->construct() != NMF_OK) return OMX_ErrorInsufficientResources;
    
    nmf_error = ((NMF::Composite*)mNmfMain)->getInterface("configure", &mIConfig);
    if (nmf_error != NMF_OK) return OMX_ErrorInsufficientResources;

    return OMX_ErrorNone;
}

OMX_ERRORTYPE AMRWBHostEncAlgo::deInstantiate() {

    if (((NMF::Composite*)mNmfMain)->destroy() != NMF_OK) return OMX_ErrorInsufficientResources;
    
    amrwb_nmfil_host_composite_encalgoDestroy((amrwb_nmfil_host_composite_encalgo* &)(mNmfMain));

    return OMX_ErrorNone;
}

inline OMX_ERRORTYPE AMRWBHostEncAlgo::start() {

    ((NMF::Composite*)mNmfMain)->start();
    return OMX_ErrorNone;
}

inline OMX_ERRORTYPE AMRWBHostEncAlgo::stop() {

    ((NMF::Composite*)mNmfMain)->stop();
    return OMX_ErrorNone;
}

OMX_ERRORTYPE AMRWBHostEncAlgo::setParameter(AmrwbEncParam_t amrwbNmfParams) {
    mIConfig.setParameter(amrwbNmfParams);
    return OMX_ErrorNone;
}

OMX_ERRORTYPE AMRWBHostEncAlgo::setConfig(AmrwbEncConfig_t amrwbNmfConfig) {
    mIConfig.setConfig(amrwbNmfConfig);
    return OMX_ErrorNone;
}
