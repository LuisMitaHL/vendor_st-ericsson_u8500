/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   NmfHostSampleRateConv.cpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "NmfHostSampleRateConv.h"
#include "AFM_Utils.h"
#include <armnmf_dbc.h>
#include "samplerateconv/nmfil/host/effectWrapped.hpp"

NmfHostSampleRateConv::NmfHostSampleRateConv(t_host_pcmprocessing_config &config)
:NmfHostPcmProcessing(config)
{
  mLowMipsToOut48 = 0;
}

NmfHostSampleRateConv::~NmfHostSampleRateConv() {
}

OMX_ERRORTYPE NmfHostSampleRateConv::instantiateAlgo(void) {
    t_nmf_error   nmf_error;

    mNmfAlgo = samplerateconv_nmfil_host_effectWrappedCreate();
    if (mNmfAlgo == NULL) { return OMX_ErrorInsufficientResources; };
    ((samplerateconv_nmfil_host_effectWrapped*)mNmfAlgo)->priority = mMyPriority;
    if (mNmfAlgo->construct() != NMF_OK) return OMX_ErrorInsufficientResources;

    nmf_error = mNmfAlgo->bindFromUser("configure", 8, &mIconfigure);
    if (nmf_error != NMF_OK) return OMX_ErrorInsufficientResources;

    return OMX_ErrorNone;
}


OMX_ERRORTYPE NmfHostSampleRateConv::startAlgo(void) {
    mNmfAlgo->start();
    return OMX_ErrorNone;
}


OMX_ERRORTYPE NmfHostSampleRateConv::stopAlgo(void) {
    mNmfAlgo->stop();
    return OMX_ErrorNone;
}

OMX_ERRORTYPE NmfHostSampleRateConv::applyConfig(OMX_INDEXTYPE nConfigIndex,  
                                         OMX_PTR pComponentConfigStructure)
{

  return OMX_ErrorNone; 
}

OMX_ERRORTYPE NmfHostSampleRateConv::configureAlgo(void) {
  mIconfigure.setParameter(mLowMipsToOut48);
  
  return OMX_ErrorNone;
}


OMX_ERRORTYPE NmfHostSampleRateConv::deInstantiateAlgo(void) {

    if (mNmfAlgo->unbindFromUser("configure") != NMF_OK) return OMX_ErrorInsufficientResources;

    if (mNmfAlgo->destroy() != NMF_OK) return OMX_ErrorInsufficientResources;
    
    samplerateconv_nmfil_host_effectWrappedDestroy((samplerateconv_nmfil_host_effectWrapped*&)mNmfAlgo);

    return OMX_ErrorNone;
}
