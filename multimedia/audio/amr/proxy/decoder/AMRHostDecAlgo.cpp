/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved

*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   AMR Host Decoder Algo class
* \author  ST-Ericsson
*/
/*****************************************************************************/

#include "AMRHostDecAlgo.h"
#include "amr/nmfil/host/composite/decalgo.hpp"
#include "AMRDec_BitstreamPort.h"

OMX_ERRORTYPE AMRHostDecAlgo::instantiate(OMX_U32 domainId, OMX_U32 priority) {
    
    t_nmf_error   nmf_error;

    setNMFDomainHandle(domainId); // Unused here
    mMyPriority = priority; // Unused here (No way to set priority at executing state in NMF 1.8)
    
    mNmfMain = (void*)amr_nmfil_host_composite_decalgoCreate();
    if (((NMF::Composite*)mNmfMain) == NULL) { return OMX_ErrorInsufficientResources; };
    ((amr_nmfil_host_composite_decalgo *)(mNmfMain))->priority = mMyPriority;
    if (((NMF::Composite*)mNmfMain)->construct() != NMF_OK) return OMX_ErrorInsufficientResources;
    
    nmf_error = ((NMF::Composite*)mNmfMain)->getInterface("configure", &mIConfig);
    if (nmf_error != NMF_OK) return OMX_ErrorInsufficientResources;

    return OMX_ErrorNone;
}

OMX_ERRORTYPE AMRHostDecAlgo::deInstantiate() {

    if (((NMF::Composite*)mNmfMain)->destroy() != NMF_OK) return OMX_ErrorInsufficientResources;
    
    amr_nmfil_host_composite_decalgoDestroy((amr_nmfil_host_composite_decalgo*&)mNmfMain);

    return OMX_ErrorNone;
}

inline OMX_ERRORTYPE AMRHostDecAlgo::start() {

    ((NMF::Composite*)mNmfMain)->start();
    return OMX_ErrorNone;
}

inline OMX_ERRORTYPE AMRHostDecAlgo::stop() {

    ((NMF::Composite*)mNmfMain)->stop();
    return OMX_ErrorNone;
}

OMX_ERRORTYPE AMRHostDecAlgo::setParameter(AmrDecParams_t amrNmfParams) {
    mIConfig.setParameter(amrNmfParams);
    return OMX_ErrorNone;
}

OMX_ERRORTYPE AMRHostDecAlgo::setConfig(AmrDecConfig_t amrNmfConfig) {
    mIConfig.setConfig(amrNmfConfig);
    return OMX_ErrorNone;
}

