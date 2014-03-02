/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   HostSHM_PcmIn.cpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "HostSHM_PcmIn.h"
#include "AFM_Utils.h"
#include "AFM_PcmPort.h"
#include "ENS_Wrapper_Services.h"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "afm_proxy_bindings_HostSHM_PcmInTraces.h"
#endif

OMX_ERRORTYPE 
HostSHM_PcmIn::allocateBuffer() {
    AFM_PARAM_PCMLAYOUTTYPE pcmLayout;
    AFM_PcmPort & port = static_cast<AFM_PcmPort &>(mPort);
    
    pcmLayout   = port.getPcmLayout();

    mBufferSize = pcmLayout.nBlockSize * pcmLayout.nChannels * (pcmLayout.nBitsPerSample/8);

    memset(&mBuffer, 0, sizeof(OMX_BUFFERHEADERTYPE));

    mBuffer.pBuffer = (OMX_U8*) new char[mBufferSize];

    if( mBuffer.pBuffer == NULL ) {
      return OMX_ErrorInsufficientResources;
    }

    mBuffer.nAllocLen = mBufferSize;
    mBuffer.nSize = sizeof(OMX_BUFFERHEADERTYPE);
    return OMX_ErrorNone;
}

OMX_ERRORTYPE
HostSHM_PcmIn::freeBuffer() {
  delete [] mBuffer.pBuffer;
  return OMX_ErrorNone;
}

OMX_ERRORTYPE HostSHM_PcmIn::instantiateBindingComponent(void) {
    t_nmf_error nmf_error;
    OMX_ERRORTYPE error;

    mNmfHandle = hst_composite_shmpcminCreate();
    if (mNmfHandle == 0) return OMX_ErrorInsufficientResources;
    
    // Set the priority
    ((hst_composite_shmpcmin*)mNmfHandle)->host_priority_level=getPortPriorityLevel();

    if (mNmfHandle->construct() != NMF_OK) return OMX_ErrorInsufficientResources;
    
    nmf_error = mNmfHandle->bindFromUser("configure", 1, &mIconfigure);
    if (nmf_error != NMF_OK) return OMX_ErrorInsufficientResources;
    
    nmf_error = mNmfHandle->bindFromUser("emptythisbuffer", mPort.getBufferCountActual(), mIemptyThisBuffer);
    if (nmf_error != NMF_OK) return OMX_ErrorInsufficientResources;
    
    nmf_error = EnsWrapper_bindToUser(mOMXHandle, mNmfHandle, "inputport", mDfillThisBuffer, mPort.getBufferCountActual());
    if (nmf_error != NMF_OK) return OMX_ErrorInsufficientResources;

    error = allocateBuffer();
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}


OMX_ERRORTYPE HostSHM_PcmIn::deInstantiateBindingComponent(void) {
  OMX_ERRORTYPE error;
  t_nmf_error nmf_error;

    error = freeBuffer();
    if (error != OMX_ErrorNone) return error;

    nmf_error = mNmfHandle->unbindFromUser("configure");
    if (nmf_error != NMF_OK) return OMX_ErrorInsufficientResources;
    
    nmf_error = mNmfHandle->unbindFromUser("emptythisbuffer");
    if (nmf_error != NMF_OK) return OMX_ErrorInsufficientResources;
    
    nmf_error = EnsWrapper_unbindToUser(mOMXHandle, mNmfHandle, "inputport");
    if (nmf_error != NMF_OK) return OMX_ErrorInsufficientResources;

    if (mNmfHandle->destroy() != NMF_OK) return OMX_ErrorInsufficientResources;

    hst_composite_shmpcminDestroy((hst_composite_shmpcmin*&)mNmfHandle);

    return OMX_ErrorNone;
}


OMX_ERRORTYPE HostSHM_PcmIn::configure(void) {
    ShmPcmConfig_t              ShmConfig;
    OMX_AUDIO_PARAM_PCMMODETYPE pcmSettings;
    AFM_PARAM_PCMLAYOUTTYPE     pcmLayout;
    AFM_PcmPort &               port = static_cast<AFM_PcmPort &>(mPort);

    pcmSettings = port.getPcmSettings();
    pcmLayout   = port.getPcmLayout();
    
    DBC_ASSERT((pcmSettings.nBitPerSample == 16) || (pcmSettings.nBitPerSample == 32));

    ShmConfig.nb_buffer          = mPort.getBufferCountActual();
    ShmConfig.swap_bytes         = pcmSettings.eEndian == OMX_EndianBig ? SWAP_16 : NO_SWAP;
    ShmConfig.bitsPerSampleIn    = pcmSettings.nBitPerSample;
    ShmConfig.bitsPerSampleOut   = pcmLayout.nBitsPerSample;
    ShmConfig.channelsIn         = pcmSettings.nChannels;
    ShmConfig.channelsOut        = pcmLayout.nChannels;
    ShmConfig.sampleFreq         = AFM::sampleFreq(pcmSettings.nSamplingRate);
    ShmConfig.hostMpcSyncEnabled = 0;
    ShmConfig.isSynchronized     = ((port.isSynchronized() == OMX_TRUE) ? true : false);

    OstTraceFiltInst4(TRACE_ALWAYS, "AFM_PROXY: HostSHM_PcmIn::configure (port=%d) (nBufferSize=%d) (hostMpcSyncEnabled=%d) (ShmConfig.isSynchronized=%d)", mPort.getPortIndex(), mBufferSize, ShmConfig.hostMpcSyncEnabled,ShmConfig.isSynchronized);
    OstTraceFiltInst4(TRACE_ALWAYS, "AFM_PROXY: HostSHM_PcmIn::configure (nb_buffer=%d) (swap_bytes=%d) (nChansOut=%d) (nBitPerSampleOut=%d)",ShmConfig.nb_buffer, ShmConfig.swap_bytes, ShmConfig.channelsOut, ShmConfig.bitsPerSampleOut);
    OstTraceFiltInst4(TRACE_ALWAYS, "AFM_PROXY: HostSHM_PcmIn::configure (sampleFreq=%d) (nChansIn=%d) (nBitPerSampleIn=%d) (endianness=%d)", pcmSettings.nSamplingRate, ShmConfig.channelsIn, ShmConfig.bitsPerSampleIn, pcmSettings.eEndian);


    mIconfigure.setParameter(ShmConfig, (void *) &mBuffer, mBufferSize);
           
    return OMX_ErrorNone;
}                 
