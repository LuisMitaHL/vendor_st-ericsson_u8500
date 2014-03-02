/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   AFMNmfHost_PcmProcessing.cpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "AFMNmfHost_PcmProcessing.h"
#include "hst/composite/pp_wrp.hpp"
#include "AFM_PcmProcessing.h"
#include "AFM_Utils.h"
#include "AFM_PcmPort.h"


AFMNmfHost_PcmProcessing::AFMNmfHost_PcmProcessing(ENS_Component &enscomp) :AFMNmfHost_ProcessingComp(enscomp) {

  mPcmProcConfig.processingMode          = PCMPROCESSING_MODE_HOST_COUNT; 
  mPcmProcConfig.effectConfig.block_size = 0;
  
  mPcmProcConfig.effectConfig.infmt.freq                   = FREQ_UNKNOWN;
  mPcmProcConfig.effectConfig.infmt.nof_channels           = 0;
  mPcmProcConfig.effectConfig.infmt.nof_bits_per_sample    = 0;
  mPcmProcConfig.effectConfig.infmt.interleaved            = OMX_TRUE;
  
  mPcmProcConfig.effectConfig.outfmt.freq                   = FREQ_UNKNOWN;
  mPcmProcConfig.effectConfig.outfmt.nof_channels           = 0;
  mPcmProcConfig.effectConfig.outfmt.nof_bits_per_sample    = 0;
    mPcmProcConfig.effectConfig.outfmt.interleaved            = OMX_TRUE;
    
    for(int i = 0; i < OMX_AUDIO_MAXCHANNELS; i++) {
      mPcmProcConfig.effectConfig.infmt.channel_mapping[i]   = CHANNEL_NONE;
      mPcmProcConfig.effectConfig.outfmt.channel_mapping[i] = CHANNEL_NONE;
    }

  mNmfil  = 0;
}

OMX_ERRORTYPE
AFMNmfHost_PcmProcessing::construct(void)
{
    OMX_ERRORTYPE error;

    mNmfil          = 0;
    
    //    setPriorityLevel((OMX_U32)NMF_SCHED_BACKGROUND);
    
    error = AFMNmfHost_ProcessingComp::construct();
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}

OMX_ERRORTYPE 
AFMNmfHost_PcmProcessing::instantiateMain(void) {
  t_nmf_error   nmf_error;
  OMX_ERRORTYPE error;

    error = instantiateAlgo();
    if (error != OMX_ErrorNone) return error;

    mNmfMain = hst_composite_pp_wrpCreate();
    if (mNmfMain == NULL) return OMX_ErrorInsufficientResources;
    ((hst_composite_pp_wrp*)mNmfMain)->priority = getPriorityLevel();
    if (mNmfMain->construct() != NMF_OK) return OMX_ErrorInsufficientResources;

    nmf_error = mNmfMain->bindFromUser("configure", 1, &mIconfigure);
    if (nmf_error != NMF_OK) { 	NMF_LOG("Error: unable to bind main configure!...[%d]\n", nmf_error); return OMX_ErrorInsufficientResources; }

    nmf_error = mNmfMain->bindComponent("effect", mNmfil, "effect");
    if (nmf_error != NMF_OK) { 	NMF_LOG("Error: unable to bind main effect!...[%d]\n", nmf_error); return OMX_ErrorInsufficientResources; }

    return OMX_ErrorNone;
}

OMX_ERRORTYPE 
AFMNmfHost_PcmProcessing::deInstantiateMain(void) {
    OMX_ERRORTYPE error;
    t_nmf_error   nmf_error;

    nmf_error = mNmfMain->unbindComponent("effect", mNmfil, "effect");
    if (nmf_error != NMF_OK) { 	NMF_LOG("Error: unable to unbind main effect!...[%d]\n", nmf_error); return OMX_ErrorInsufficientResources; }

    nmf_error = mNmfMain->unbindFromUser("configure");
    if (nmf_error != NMF_OK) { 	NMF_LOG("Error: unable to unbind main configure!...[%d]\n", nmf_error); return OMX_ErrorInsufficientResources; }

    error = deInstantiateAlgo();
    if (error != OMX_ErrorNone) return error;

    if (mNmfMain->destroy() != NMF_OK) return OMX_ErrorInsufficientResources;

    hst_composite_pp_wrpDestroy((hst_composite_pp_wrp*&)mNmfMain);

    return OMX_ErrorNone;
}



OMX_ERRORTYPE 
AFMNmfHost_PcmProcessing::startMain(void) {
    mNmfMain->start();

    return startAlgo();
}


OMX_ERRORTYPE 
AFMNmfHost_PcmProcessing::stopMain(void) {
    mNmfMain->stop_flush();

    return stopAlgo();
}


OMX_ERRORTYPE 
AFMNmfHost_PcmProcessing::configureMain(void) {
    OMX_ERRORTYPE error;

    error = configureAlgo();
    if (error != OMX_ErrorNone) return error;

    AFM_PcmProcessing* proxy = static_cast<AFM_PcmProcessing*>(&mENSComponent);
    AFM_PcmPort* outputPort = (AFM_PcmPort*) proxy->getPort(1);
    AFM_PcmPort* inputPort = (AFM_PcmPort*) proxy->getPort(0);

    mPcmProcConfig.processingMode          = PCMPROCESSING_MODE_HOST_NOT_INPLACE; 
    mPcmProcConfig.effectConfig.block_size = proxy->getBlockSize();

    mPcmProcConfig.effectConfig.infmt.freq                   = AFM::sampleFreq(inputPort->getPcmSettings().nSamplingRate);
    mPcmProcConfig.effectConfig.infmt.nof_channels           = inputPort->getPcmLayout().nChannels;
    mPcmProcConfig.effectConfig.infmt.nof_bits_per_sample    = inputPort->getPcmLayout().nBitsPerSample;
    mPcmProcConfig.effectConfig.infmt.interleaved            = OMX_TRUE;

    mPcmProcConfig.effectConfig.outfmt.freq                   = AFM::sampleFreq(outputPort->getPcmSettings().nSamplingRate);
    mPcmProcConfig.effectConfig.outfmt.nof_channels           = outputPort->getPcmLayout().nChannels;
    mPcmProcConfig.effectConfig.outfmt.nof_bits_per_sample    = outputPort->getPcmLayout().nBitsPerSample;
    mPcmProcConfig.effectConfig.outfmt.interleaved            = OMX_TRUE;

    for(int i = 0; i < OMX_AUDIO_MAXCHANNELS; i++) {
        mPcmProcConfig.effectConfig.infmt.channel_mapping[i]   = static_cast<t_channel_type>(inputPort->getPcmSettings().eChannelMapping[i]);
        mPcmProcConfig.effectConfig.outfmt.channel_mapping[i] = static_cast<t_channel_type>(outputPort->getPcmSettings().eChannelMapping[i]);
    }

    mIconfigure.setParameter(mPcmProcConfig);

    return OMX_ErrorNone;
}

