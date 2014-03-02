/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   HostSHMout.cpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "HostSHMout.h"
#include "ENS_Wrapper_Services.h"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "afm_proxy_bindings_HostSHMoutTraces.h"
#endif

OMX_ERRORTYPE HostSHMout::instantiateBindingComponent(void) {
  t_nmf_error nmf_error;

    mNmfHandle = hst_composite_shmoutCreate();
    if (mNmfHandle == 0) return OMX_ErrorInsufficientResources;
    
    // Set the priority
    ((hst_composite_shmout*)mNmfHandle)->host_priority_level=getPortPriorityLevel();
    
    if (mNmfHandle->construct() != NMF_OK) return OMX_ErrorInsufficientResources;

    nmf_error = mNmfHandle->bindFromUser("configure", 1, &mIconfigure);
    if (nmf_error != NMF_OK) return OMX_ErrorInsufficientResources;
   
    nmf_error = mNmfHandle->bindFromUser("fillthisbuffer", mPort.getBufferCountActual(), mIfillThisBuffer);
    if (nmf_error != NMF_OK) return OMX_ErrorInsufficientResources;

    nmf_error = EnsWrapper_bindToUser(mOMXHandle, mNmfHandle, "outputport", mDemptyThisBuffer, mPort.getBufferCountActual());
    if (nmf_error != NMF_OK) return OMX_ErrorInsufficientResources;

    return OMX_ErrorNone;
}


OMX_ERRORTYPE HostSHMout::deInstantiateBindingComponent(void) {
  t_nmf_error nmf_error;

    nmf_error = mNmfHandle->unbindFromUser("configure");
    if (nmf_error != NMF_OK) return OMX_ErrorUndefined;

    nmf_error = mNmfHandle->unbindFromUser("fillthisbuffer");
    if (nmf_error != NMF_OK) return OMX_ErrorUndefined;

    nmf_error = EnsWrapper_unbindToUser(mOMXHandle, mNmfHandle, "outputport");
    if (nmf_error != NMF_OK) return OMX_ErrorUndefined;

    if (mNmfHandle->destroy() != NMF_OK) return OMX_ErrorInsufficientResources;

    hst_composite_shmoutDestroy((hst_composite_shmout*&)mNmfHandle);

    return OMX_ErrorNone;
}


OMX_ERRORTYPE HostSHMout::configure(void) {
    ShmConfig_t                     ShmConfig = {0, NO_SWAP};
    OMX_PARAM_PORTDEFINITIONTYPE    portdef;

    portdef.nSize           = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
    getOmxIlSpecVersion(&portdef.nVersion);

    mPort.getParameter(OMX_IndexParamPortDefinition, &portdef);

    ShmConfig.nb_buffer     = portdef.nBufferCountActual;
    ShmConfig.swap_bytes    = SWAP_32;
    
    OstTraceFiltInst2(TRACE_ALWAYS, "AFM_PROXY: HostSHMout::configure (nb_buffer=%d) (swap_bytes=%d)",ShmConfig.nb_buffer,ShmConfig.swap_bytes);
    mIconfigure.setParameter(ShmConfig);

    return OMX_ErrorNone;
}


