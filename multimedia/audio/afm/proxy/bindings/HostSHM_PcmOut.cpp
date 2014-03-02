/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   HostSHM_PcmOut.cpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "HostSHM_PcmOut.h"
#include "AFM_PcmPort.h"
#include "AFM_Utils.h"
#include "ENS_Wrapper_Services.h"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "afm_proxy_bindings_HostSHM_PcmOutTraces.h"
#endif

OMX_ERRORTYPE 
HostSHM_PcmOut::allocateBuffer() {
    AFM_PARAM_PCMLAYOUTTYPE pcmLayout;
    OMX_AUDIO_PARAM_PCMMODETYPE pcmSettings;
    AFM_PcmPort & port = static_cast<AFM_PcmPort &>(mPort);
    
    pcmLayout   = port.getPcmLayout();
    pcmSettings = port.getPcmSettings();

    mBufferSize = pcmLayout.nBlockSize * (pcmLayout.nChannels?pcmLayout.nChannels:pcmSettings.nChannels) * (pcmLayout.nBitsPerSample/8);

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
HostSHM_PcmOut::freeBuffer() {
  delete [] mBuffer.pBuffer;
  return OMX_ErrorNone;
}


OMX_ERRORTYPE HostSHM_PcmOut::instantiateBindingComponent(void) {
  OMX_ERRORTYPE error;
  t_nmf_error nmf_error;

    mNmfHandle = hst_composite_shmpcmoutCreate();
    if (mNmfHandle == 0) return OMX_ErrorInsufficientResources;

    // Set the priority
    ((hst_composite_shmpcmout*)mNmfHandle)->host_priority_level=getPortPriorityLevel();

    if (mNmfHandle->construct() != NMF_OK) return OMX_ErrorInsufficientResources;
    
    nmf_error = mNmfHandle->bindFromUser("configure", 1, &mIconfigure);
    if (nmf_error != NMF_OK) return OMX_ErrorInsufficientResources;
   
    nmf_error = mNmfHandle->bindFromUser("fillthisbuffer", mPort.getBufferCountActual(), mIfillThisBuffer);
    if (nmf_error != NMF_OK) return OMX_ErrorInsufficientResources;

    nmf_error = EnsWrapper_bindToUser(mOMXHandle, mNmfHandle, "outputport", mDemptyThisBuffer, mPort.getBufferCountActual());
    if (nmf_error != NMF_OK) return OMX_ErrorInsufficientResources;

    error = allocateBuffer();
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}


OMX_ERRORTYPE HostSHM_PcmOut::deInstantiateBindingComponent(void) {
  OMX_ERRORTYPE error;
  t_nmf_error nmf_error;

    error = freeBuffer();
    if (error != OMX_ErrorNone) return error;

    nmf_error = mNmfHandle->unbindFromUser("configure");
    if (nmf_error != NMF_OK) return OMX_ErrorInsufficientResources;
   
    nmf_error = mNmfHandle->unbindFromUser("fillthisbuffer");
    if (nmf_error != NMF_OK) return OMX_ErrorInsufficientResources;

    nmf_error = EnsWrapper_unbindToUser(mOMXHandle, mNmfHandle, "outputport");
    if (nmf_error != NMF_OK) return OMX_ErrorInsufficientResources;

    if (mNmfHandle->destroy() != NMF_OK) return OMX_ErrorInsufficientResources;

    hst_composite_shmpcmoutDestroy((hst_composite_shmpcmout*&)mNmfHandle);

    return OMX_ErrorNone;
}


OMX_ERRORTYPE HostSHM_PcmOut::configure(void) {
    ShmPcmConfig_t              ShmConfig;
    OMX_AUDIO_PARAM_PCMMODETYPE pcmSettings;
    AFM_PARAM_PCMLAYOUTTYPE     pcmLayout;
    AFM_PcmPort &               port = static_cast<AFM_PcmPort &>(mPort);


    pcmSettings = port.getPcmSettings();
    pcmLayout   = port.getPcmLayout();

    DBC_ASSERT((pcmSettings.nBitPerSample == 16) || (pcmSettings.nBitPerSample == 32));

    if (pcmLayout.nChannels) {
        DBC_ASSERT(pcmSettings.nChannels == pcmLayout.nChannels);
    }

    ShmConfig.nb_buffer          = port.getBufferCountActual();
    ShmConfig.swap_bytes         = pcmSettings.eEndian == OMX_EndianBig ? SWAP_16 : NO_SWAP;
    ShmConfig.bitsPerSampleIn    = pcmLayout.nBitsPerSample;
    ShmConfig.bitsPerSampleOut   = pcmSettings.nBitPerSample;
    ShmConfig.channelsIn         = pcmSettings.nChannels;
    ShmConfig.channelsOut        = pcmSettings.nChannels;
    ShmConfig.sampleFreq         = AFM::sampleFreq(pcmSettings.nSamplingRate);
    ShmConfig.hostMpcSyncEnabled = 0;
    ShmConfig.isSynchronized     = false;

    OstTraceFiltInst3(TRACE_ALWAYS, "AFM_PROXY: HostSHM_PcmOut::configure (port=%d) (nBufferSize=%d) (hostMpcSyncEnabled=%d)",mPort.getPortIndex(), mBufferSize, ShmConfig.hostMpcSyncEnabled);
    OstTraceFiltInst4(TRACE_ALWAYS, "AFM_PROXY: HostSHM_PcmOut::configure (nb_buffer=%d) (swap_bytes=%d) (nChansIn=%d) (nBitPerSampleIn=%d)",ShmConfig.nb_buffer, ShmConfig.swap_bytes, ShmConfig.channelsIn, ShmConfig.bitsPerSampleIn);
    OstTraceFiltInst4(TRACE_ALWAYS, "AFM_PROXY: HostSHM_PcmOut::configure (sampleFreq=%d) (nChansOut=%d) (nBitPerSampleOut=%d) (endianness=%d)", pcmSettings.nSamplingRate, ShmConfig.channelsOut, ShmConfig.bitsPerSampleOut, pcmSettings.eEndian);

    mIconfigure.setParameter(ShmConfig, (void *) &mBuffer, mBufferSize);

    return OMX_ErrorNone;
}


