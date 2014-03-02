/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   AFMNmfHost_Decoder.cpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "AFM_Decoder.h"
#include "AFMNmfHost_Decoder.h"
#ifdef USE_FA_WRP
#include "hst/composite/dec_fa_wrp.hpp"
#else
#include "hst/composite/dec_st_wrp.hpp"
#endif

#define BUFFER_SIZE_MULTIPLE_IN_BYTES 4 // must be a power of 2
// TODO: make this a global variable....
//#define SAMPLE_SIZE_IN_BYTES 2
AFM_API_EXPORT OMX_ERRORTYPE
AFMNmfHost_Decoder::construct(void)
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
AFMNmfHost_Decoder::instantiateMain(void) {
  t_nmf_error   nmf_error;
  OMX_ERRORTYPE error;

    error = instantiateAlgo();

    if (error != OMX_ErrorNone) return error;
#ifdef USE_FA_WRP
    mNmfMain = hst_composite_dec_fa_wrpCreate();
    if (mNmfMain == NULL) return OMX_ErrorInsufficientResources;
    ((hst_composite_dec_fa_wrp*)mNmfMain)->priority = getPriorityLevel();
#else
    mNmfMain = hst_composite_dec_st_wrpCreate();
    if (mNmfMain == NULL) return OMX_ErrorInsufficientResources;
    ((hst_composite_dec_st_wrp*)mNmfMain)->priority = getPriorityLevel();
#endif

    if (mNmfMain->construct() != NMF_OK) return OMX_ErrorInsufficientResources;

    nmf_error = mNmfMain->bindFromUser("configure", 1, &mIconfigure);
    if (nmf_error != NMF_OK) { 	NMF_LOG("Error: unable to bind main configure!...[%d]\n", nmf_error); return OMX_ErrorInsufficientResources; }

    nmf_error = mNmfMain->bindComponent("decoder", mNmfil, "decoder");
    if (nmf_error != NMF_OK) { 	NMF_LOG("Error: unable to bind main decoder!...[%d]\n", nmf_error); return OMX_ErrorInsufficientResources; }

    return OMX_ErrorNone;
}

AFM_API_EXPORT OMX_ERRORTYPE 
AFMNmfHost_Decoder::deInstantiateMain(void) {
    OMX_ERRORTYPE error;
    t_nmf_error   nmf_error;

    nmf_error = mNmfMain->unbindComponent("decoder", mNmfil, "decoder");
    if (nmf_error != NMF_OK) { 	NMF_LOG("Error: unable to unbind main decoder!...[%d]\n", nmf_error); return OMX_ErrorInsufficientResources; }

    nmf_error = mNmfMain->unbindFromUser("configure");
    if (nmf_error != NMF_OK) { 	NMF_LOG("Error: unable to unbind main configure!...[%d]\n", nmf_error); return OMX_ErrorInsufficientResources; }

    error = deInstantiateAlgo();
    if (error != OMX_ErrorNone) return error;

    if (mNmfMain->destroy() != NMF_OK) return OMX_ErrorInsufficientResources;

    if (bufInPtr)
      delete [] bufInPtr;

    if (bufOutPtr)
      delete [] bufOutPtr;

    bufInPtr = NULL;
    bufOutPtr = NULL;

#ifdef USE_FA_WRP
    hst_composite_dec_fa_wrpDestroy((hst_composite_dec_fa_wrp*&)mNmfMain);
#else
    hst_composite_dec_st_wrpDestroy((hst_composite_dec_st_wrp*&)mNmfMain);
#endif

    return OMX_ErrorNone;
}



AFM_API_EXPORT OMX_ERRORTYPE 
AFMNmfHost_Decoder::startMain(void) {
    mNmfMain->start();

    return startAlgo();
}


AFM_API_EXPORT OMX_ERRORTYPE 
AFMNmfHost_Decoder::stopMain(void) {
    mNmfMain->stop_flush();

    return stopAlgo();
}


AFM_API_EXPORT OMX_ERRORTYPE 
AFMNmfHost_Decoder::configureMain(void) {
    OMX_ERRORTYPE error;
    unsigned int payLoad = 0;
#ifdef __SYMBIAN32__
    payLoad = 1;
#endif //__SYMBIAN32__ 

    error = configureAlgo();
    if (error != OMX_ErrorNone) return error;

#ifndef USE_FA_WRP
    int max_bitstream_frame_size_in_bytes;
    int max_frame_size_in_word, max_frame_size_in_bytes;
    AFM_Decoder *afmdec = (AFM_Decoder *)&mENSComponent;
    // round the buffer size to the desired multiple
    max_bitstream_frame_size_in_bytes = ((afmdec->getMaxFrameSize() / 8) + 1 + BUFFER_SIZE_MULTIPLE_IN_BYTES - 1) 
      & ~(BUFFER_SIZE_MULTIPLE_IN_BYTES-1);

    max_frame_size_in_word = afmdec->getSampleFrameSize() * afmdec->getMaxChannels();
    // round the buffer size to the desired multiple
    max_frame_size_in_bytes = ((max_frame_size_in_word * (afmdec->getSampleBitSize() / 8)) + BUFFER_SIZE_MULTIPLE_IN_BYTES - 1) 
      & ~(BUFFER_SIZE_MULTIPLE_IN_BYTES-1);

    bufInPtr = (OMX_U8*) new char[max_bitstream_frame_size_in_bytes];
    if (!bufInPtr) return OMX_ErrorInsufficientResources;

    bufOutPtr = (OMX_U8*) new char[max_frame_size_in_bytes];
    if (!bufOutPtr) return OMX_ErrorInsufficientResources;

    mIconfigure.setParameter(bufInPtr, max_bitstream_frame_size_in_bytes, bufOutPtr, max_frame_size_in_bytes, payLoad);
#else
    mIconfigure.setParameter();
#endif

    return OMX_ErrorNone;
}

