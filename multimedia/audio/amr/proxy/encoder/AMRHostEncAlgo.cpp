/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved

*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   AMR Host Encoder Proxy Port class
* \author  ST-Ericsson
*/
/*****************************************************************************/

#include "AMRHostEncAlgo.h"
#include "amr/nmfil/host/composite/encalgo.hpp"
#include "AMREnc_BitstreamPort.h"

OMX_ERRORTYPE AMRHostEncAlgo::instantiate(OMX_U32 domainId, OMX_U32 priority) {
    
    t_nmf_error   nmf_error;

    setNMFDomainHandle(domainId); // Unused here
    mMyPriority = priority; // Unused here (No way to set priority at executing state in NMF 1.8)
    
    mNmfMain = (void*)amr_nmfil_host_composite_encalgoCreate();
    ((amr_nmfil_host_composite_encalgo *)(mNmfMain))->priority = mMyPriority;
    if (((NMF::Composite*)mNmfMain) == NULL) { return OMX_ErrorInsufficientResources; };
    
    if (((NMF::Composite*)mNmfMain)->construct() != NMF_OK) return OMX_ErrorInsufficientResources;
    
    nmf_error = ((NMF::Composite*)mNmfMain)->getInterface("configure", &mIConfig);
    if (nmf_error != NMF_OK) return OMX_ErrorInsufficientResources;

    return OMX_ErrorNone;
}

OMX_ERRORTYPE AMRHostEncAlgo::deInstantiate() {

    if (((NMF::Composite*)mNmfMain)->destroy() != NMF_OK) return OMX_ErrorInsufficientResources;
    
    amr_nmfil_host_composite_encalgoDestroy((amr_nmfil_host_composite_encalgo*&)(mNmfMain));

    return OMX_ErrorNone;
}

inline OMX_ERRORTYPE AMRHostEncAlgo::start() {

    ((NMF::Composite*)mNmfMain)->start();
    return OMX_ErrorNone;
}

inline OMX_ERRORTYPE AMRHostEncAlgo::stop() {

    ((NMF::Composite*)mNmfMain)->stop();
    return OMX_ErrorNone;
}

OMX_ERRORTYPE AMRHostEncAlgo::setParameter(AmrEncParam_t amrNmfParams) {
    mIConfig.setParameter(amrNmfParams);
    return OMX_ErrorNone;
}

OMX_ERRORTYPE AMRHostEncAlgo::setConfig(AmrEncConfig_t amrNmfConfig) {
    mIConfig.setConfig(amrNmfConfig);
    return OMX_ErrorNone;
}

