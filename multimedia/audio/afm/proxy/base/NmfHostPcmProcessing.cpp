/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   NmfHostPcmProcessing.cpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "NmfHostPcmProcessing.h"
#include "hst/composite/pp_wrp.hpp"
#include "AFM_Utils.h"

//#define _PRINT_NMFHOSTPCMPROCESSING_FMT_

#ifndef __SYMBIAN32__ 
#include <stdio.h>
void NmfHostPcmProcessing::print_fmt(t_host_effect_format * fmt) {

#ifdef _PRINT_NMFHOSTPCMPROCESSING_FMT_

  printf("\t\tfreq.: %d\n", (int)AFM::OMX_SamplingRate(fmt->freq));
  printf("\t\tnof_channels.: %d\n", fmt->nof_channels);
  printf("\t\tnof_bits_per_sample: %d\n", fmt->nof_bits_per_sample);
  printf("\t\theadroom.: %d\n", fmt->headroom);
  printf("\t\tinterleaved.: %d\n", fmt->interleaved);

#endif // _PRINT_NMFHOSTPCMPROCESSING_FMT_

}
#endif


AFM_API_EXPORT NmfHostPcmProcessing::NmfHostPcmProcessing(
        //coverity[pass_by_value]
        t_host_pcmprocessing_config config): NmfComponent(), mNmfAlgo(0)
{
#ifndef __SYMBIAN32__ 

#ifdef _PRINT_NMFHOSTPCMPROCESSING_FMT_

  printf("NmfHostPcmProcessing::NmfHostPcmProcessing() print config:\n");
  printf("\tprocessingMode: %d\n", config.processingMode);
  printf("\tblocksize: %d\n", (int)config.effectConfig.block_size);

  printf("\tinfmt:\n");
  print_fmt(&config.effectConfig.infmt);
  printf("\toutfmt:\n");
  print_fmt(&config.effectConfig.outfmt);

#endif // _PRINT_NMFHOSTPCMPROCESSING_FMT_

#endif

  mConfig = config;
}

AFM_API_EXPORT OMX_ERRORTYPE NmfHostPcmProcessing::instantiate(OMX_U32 domainId, OMX_U32 priority) {
    OMX_ERRORTYPE error;
    t_nmf_error   nmf_error;

    setNMFDomainHandle(domainId);
    mMyPriority = priority;

    error = instantiateAlgo();
    if (error != OMX_ErrorNone) return error;

    mNmfMain = hst_composite_pp_wrpCreate();
    ((hst_composite_pp_wrp*)mNmfMain)->priority=mMyPriority;
    if (mNmfMain == NULL) return OMX_ErrorInsufficientResources;

    if (((NMF::Composite *)mNmfMain)->construct() != NMF_OK) return OMX_ErrorInsufficientResources;

    nmf_error = ((NMF::Composite *)mNmfMain)->bindFromUser("configure", 1, &mIconfigure);
    if (nmf_error != NMF_OK) { 	NMF_LOG("Error: unable to bind main configure!...[%d]\n", nmf_error); return OMX_ErrorInsufficientResources; }

    nmf_error = ((NMF::Composite *)mNmfMain)->bindComponent("effect", mNmfAlgo, "effect");
    if (nmf_error != NMF_OK) { 	NMF_LOG("Error: unable to bind main effect!...[%d]\n", nmf_error); return OMX_ErrorInsufficientResources; }

    return OMX_ErrorNone;
}


AFM_API_EXPORT OMX_ERRORTYPE NmfHostPcmProcessing::start(void) {

  ((NMF::Composite *)mNmfMain)->start();

    return startAlgo();
}


AFM_API_EXPORT OMX_ERRORTYPE NmfHostPcmProcessing::stop(void) {

    ((NMF::Composite *)mNmfMain)->stop();

    return stopAlgo();
}


AFM_API_EXPORT OMX_ERRORTYPE NmfHostPcmProcessing::configure(void) {

    mIconfigure.setParameter(mConfig);

    return configureAlgo();
}


AFM_API_EXPORT OMX_ERRORTYPE NmfHostPcmProcessing::deInstantiate(void) {
    OMX_ERRORTYPE error;
    t_nmf_error   nmf_error;

    nmf_error = ((NMF::Composite *)mNmfMain)->unbindComponent("effect", mNmfAlgo, "effect");
    if (nmf_error != NMF_OK) { 	NMF_LOG("Error: unable to unbind main effect!...[%d]\n", nmf_error); return OMX_ErrorInsufficientResources; }

    nmf_error = ((NMF::Composite *)mNmfMain)->unbindFromUser("configure");
    if (nmf_error != NMF_OK) { 	NMF_LOG("Error: unable to unbind main configure!...[%d]\n", nmf_error); return OMX_ErrorInsufficientResources; }

    error = deInstantiateAlgo();
    if (error != OMX_ErrorNone) return error;

    if (((NMF::Composite *)mNmfMain)->destroy() != NMF_OK) return OMX_ErrorInsufficientResources;

    hst_composite_pp_wrpDestroy((hst_composite_pp_wrp*&)mNmfMain);

    return OMX_ErrorNone;
}

