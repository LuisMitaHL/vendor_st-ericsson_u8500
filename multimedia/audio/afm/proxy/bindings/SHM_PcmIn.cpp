/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   SHM_PcmIn.cpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "cm/inc/cm_macros.h"
#include "SHM_PcmIn.h"
#include "ENS_Nmf.h"
#include "AFM_Utils.h"
#include "AFM_PcmPort.h"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "afm_proxy_bindings_SHM_PcmInTraces.h"
#endif

OMX_ERRORTYPE 
SHM_PcmIn::allocateBuffer() {
    OMX_ERRORTYPE error;
    AFM_PARAM_PCMLAYOUTTYPE pcmLayout;
    AFM_PcmPort & port = static_cast<AFM_PcmPort &>(mPort);
    
    pcmLayout   = port.getPcmLayout();

    mBufferSize = pcmLayout.nBlockSize * pcmLayout.nChannels;

    error = ENS::allocMpcMemory(
            getNMFDomainHandle(), mPort.getBindingMpcMemoryType24(),
            mBufferSize, CM_MM_ALIGN_WORD, &mBuffer);

    if (error == OMX_ErrorInsufficientResources &&
        mPort.getBindingMpcMemoryType24() != CM_MM_MPC_SDRAM24) {
        OstTraceFiltInst2(TRACE_DEBUG, "SHM_PcmIn::allocateBuffer failed to allocate %d units of memory type %d, trying SDRAM", mBufferSize, mPort.getBindingMpcMemoryType24());

        error = ENS::allocMpcMemory(
                getNMFDomainHandle(), CM_MM_MPC_SDRAM24,
                mBufferSize, CM_MM_ALIGN_WORD, &mBuffer);
    }


    return error;
}

OMX_ERRORTYPE
SHM_PcmIn::freeBuffer() {
    return ENS::freeMpcMemory(mBuffer);
}

OMX_ERRORTYPE SHM_PcmIn::instantiateBindingComponent(void) {
    OMX_ERRORTYPE error;
    OMX_U32 priority = getPortPriorityLevel();

    MEMORY_TRACE_ENTER("SHM_PcmIn::instantiateBindingComponent");

    OstTraceFiltInst1(TRACE_ALWAYS, "AFM_PROXY: SHM_PcmIn:: instantiateBindingComponent: SHM_PcmIn instantiated with priority %d",priority);
    error = ENS::instantiateNMFComponent(
            getNMFDomainHandle(), "bindings.shmpcm.shmin", "SHM_PcmIn", 
            &mNmfHandle, priority);
    if (error != OMX_ErrorNone) return error;

	error = ENS::bindComponent(
           mNmfHandle, "osttrace", mOstTrace, "osttrace");
    if (error != OMX_ErrorNone) return error;

    OstTraceFiltInst1(TRACE_ALWAYS, "AFM_PROXY: SHM_PcmIn:: instantiateBindingComponent: synchronisation instantiated with priority %d",priority);
    error = ENS::instantiateNMFComponent(
            getNMFDomainHandle(), "misc.synchronisation", "synchronisation", 
            &mNmfSyncLib, priority);
    if (error != OMX_ErrorNone) return error;
    
    error = ENS::bindComponent(
            mNmfHandle, "synchronisation", mNmfSyncLib, "synchronisation");
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponentFromHostEx(
            mNmfHandle, "configure", &mIconfigure, 1);
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponent(
            mNmfSharedBuffer, "mpc", mNmfHandle, "emptythisbuffer");
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponent(
            mNmfHandle, "inputport", mNmfSharedBuffer, "fillthisbuffer");
    if (error != OMX_ErrorNone) return error;
    
    error = allocateBuffer();
    if (error != OMX_ErrorNone) return error;

    MEMORY_TRACE_LEAVE("SHM_PcmIn::instantiateBindingComponent");

    return OMX_ErrorNone;
}


OMX_ERRORTYPE SHM_PcmIn::deInstantiateBindingComponent(void) {
    OMX_ERRORTYPE error;

    error = freeBuffer();
    if (error != OMX_ErrorNone) return error;

    error = ENS::unbindComponentFromHost(&mIconfigure);
    if (error != OMX_ErrorNone) return error;
    
    error = ENS::unbindComponent(mNmfSharedBuffer, "mpc");
    if (error != OMX_ErrorNone) return error;

    error = ENS::unbindComponent(mNmfHandle, "inputport");
    if (error != OMX_ErrorNone) return error;

	error = ENS::unbindComponent(mNmfHandle, "osttrace");
    if (error != OMX_ErrorNone) return error;

	error = ENS::unbindComponent(mNmfHandle, "synchronisation");
    if (error != OMX_ErrorNone) return error;

    error = ENS::destroyNMFComponent(mNmfSyncLib);
    if (error != OMX_ErrorNone) return error;

    error = ENS::destroyNMFComponent(mNmfHandle);
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}


OMX_ERRORTYPE SHM_PcmIn::configure(void) {
    ShmPcmConfig_t              ShmConfig;
    OMX_AUDIO_PARAM_PCMMODETYPE pcmSettings;
    AFM_PARAM_PCMLAYOUTTYPE     pcmLayout;
    t_uint32                    dsp_address;
    AFM_PcmPort &               port = static_cast<AFM_PcmPort &>(mPort);

    pcmSettings = port.getPcmSettings();
    pcmLayout   = port.getPcmLayout();
    
    DBC_ASSERT((pcmSettings.nBitPerSample == 16) || (pcmSettings.nBitPerSample == 32));

    CM_GetMpcMemoryMpcAddress(mBuffer, &dsp_address);
    
    ShmConfig.nb_buffer          = mPort.getBufferCountActual();
    ShmConfig.swap_bytes         = pcmSettings.eEndian == OMX_EndianBig ? SWAP_16 : NO_SWAP;
    ShmConfig.bitsPerSampleIn    = pcmSettings.nBitPerSample;
    ShmConfig.bitsPerSampleOut   = pcmLayout.nBitsPerSample;
    ShmConfig.channelsIn         = pcmSettings.nChannels;
    ShmConfig.channelsOut        = pcmLayout.nChannels;
    ShmConfig.sampleFreq         = AFM::sampleFreq(pcmSettings.nSamplingRate);
    ShmConfig.hostMpcSyncEnabled = mPort.getHostMpcSyncModeEnabled();
    ShmConfig.isSynchronized     = ((port.isSynchronized() == OMX_TRUE) ? true : false);

    OstTraceFiltInst3(TRACE_ALWAYS, "AFM_PROXY: SHM_PcmIn::configure (port=%d) (nBufferSize=%d) (hostMpcSyncEnabled=%d)", mPort.getPortIndex(), mBufferSize, ShmConfig.hostMpcSyncEnabled);
    OstTraceFiltInst4(TRACE_ALWAYS, "AFM_PROXY: SHM_PcmIn::configure (nb_buffer=%d) (swap_bytes=%d) (nChansOut=%d) (nBitPerSampleOut=%d)",ShmConfig.nb_buffer, ShmConfig.swap_bytes, ShmConfig.channelsOut, ShmConfig.bitsPerSampleOut);

    OstTraceFiltInst4(TRACE_ALWAYS, "AFM_PROXY: SHM_PcmIn::configure (sampleFreq=%d) (nChansIn=%d) (nBitPerSampleIn=%d) (endianness=%d)", pcmSettings.nSamplingRate, ShmConfig.channelsIn, ShmConfig.bitsPerSampleIn, pcmSettings.eEndian);

    mIconfigure.setParameter(ShmConfig, (void *) dsp_address, mBufferSize); 

    return OMX_ErrorNone;
}
