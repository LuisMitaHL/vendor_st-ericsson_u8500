/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   AFMNmfHost_Encoder.cpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "AFMNmfHost_Encoder.h"
#include "hst/composite/enc_fa_wrp.hpp"

AFM_API_EXPORT OMX_ERRORTYPE
AFMNmfHost_Encoder::construct(void)
{
    OMX_ERRORTYPE error;

    mNmfil          = 0;
    
    //    setPriorityLevel((OMX_U32)NMF_SCHED_BACKGROUND);
    (static_cast<AFM_Port *>(mENSComponent.getPort(0)))->setPortPriorityLevel(OMX_PriorityBackground);
    (static_cast<AFM_Port *>(mENSComponent.getPort(1)))->setPortPriorityLevel(OMX_PriorityBackground);
     
    error = AFMNmfHost_ProcessingComp::construct();
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}

AFM_API_EXPORT OMX_ERRORTYPE 
AFMNmfHost_Encoder::instantiateMain(void) {
  t_nmf_error   nmf_error;
  OMX_ERRORTYPE error;

    error = instantiateAlgo();
    if (error != OMX_ErrorNone) return error;

    mNmfMain = hst_composite_enc_fa_wrpCreate();
    if (mNmfMain == NULL) return OMX_ErrorInsufficientResources;
    ((hst_composite_enc_fa_wrp*)mNmfMain)->priority = getPriorityLevel();
    if (mNmfMain->construct() != NMF_OK) return OMX_ErrorInsufficientResources;

    nmf_error = mNmfMain->bindFromUser("configure", 1, &mIconfigure);
    if (nmf_error != NMF_OK) { 	NMF_LOG("Error: unable to bind main configure!...[%d]\n", nmf_error); return OMX_ErrorInsufficientResources; }

    nmf_error = mNmfMain->bindComponent("encoder", mNmfil, "encoder");
    if (nmf_error != NMF_OK) { 	NMF_LOG("Error: unable to bind main encoder!...[%d]\n", nmf_error); return OMX_ErrorInsufficientResources; }

    return OMX_ErrorNone;
}

AFM_API_EXPORT OMX_ERRORTYPE 
AFMNmfHost_Encoder::deInstantiateMain(void) {
    OMX_ERRORTYPE error;
    t_nmf_error   nmf_error;

    nmf_error = mNmfMain->unbindComponent("encoder", mNmfil, "encoder");
    if (nmf_error != NMF_OK) { 	NMF_LOG("Error: unable to unbind main encoder!...[%d]\n", nmf_error); return OMX_ErrorInsufficientResources; }

    nmf_error = mNmfMain->unbindFromUser("configure");
    if (nmf_error != NMF_OK) { 	NMF_LOG("Error: unable to unbind main configure!...[%d]\n", nmf_error); return OMX_ErrorInsufficientResources; }

    error = deInstantiateAlgo();
    if (error != OMX_ErrorNone) return error;

    if (mNmfMain->destroy() != NMF_OK) return OMX_ErrorInsufficientResources;

    hst_composite_enc_fa_wrpDestroy((hst_composite_enc_fa_wrp*&)mNmfMain);

    return OMX_ErrorNone;
}



AFM_API_EXPORT OMX_ERRORTYPE 
AFMNmfHost_Encoder::startMain(void) {
    mNmfMain->start();

    return startAlgo();
}


AFM_API_EXPORT OMX_ERRORTYPE 
AFMNmfHost_Encoder::stopMain(void) {
    mNmfMain->stop_flush();

    return stopAlgo();
}


AFM_API_EXPORT OMX_ERRORTYPE 
AFMNmfHost_Encoder::configureMain(void) {
    OMX_ERRORTYPE error;

    error = configureAlgo();
    if (error != OMX_ErrorNone) return error;

    mIconfigure.setParameter();

    return OMX_ErrorNone;
}

