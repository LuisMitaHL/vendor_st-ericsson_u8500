/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   PcmAdapter.cpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "cm/inc/cm_macros.h"
#include "PcmAdapter.h"
#include "ENS_Nmf.h"
#include "AFM_Utils.h"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "afm_proxy_bindings_PcmAdapterTraces.h"
#endif

PcmAdapter::PcmAdapter(AFM_PcmPort &port, AFM_Controller &controller, OMX_U32 priority)
  : MpcBindings(port, controller, priority), mNmfSyncLib(0), mBufferPcm(0)
{
    OMX_AUDIO_PARAM_PCMMODETYPE pcmSettings;
    AFM_PARAM_PCMLAYOUTTYPE pcmLayoutIn, pcmLayoutOut;

    DBC_ASSERT(port.getTunneledComponent() && !port.useStandardTunneling());

    ENS_Component_p tunneledComponent = 
        ENS::getEnsComponent(port.getTunneledComponent());

    AFM_PcmPort_p tunneledPort  
        = (AFM_PcmPort_p) tunneledComponent->getPort(port.getTunneledPort());

    pcmSettings   = port.getPcmSettings();

    if(port.getDirection()== OMX_DirOutput) {
        pcmLayoutIn     = port.getPcmLayout();
        pcmLayoutOut    = tunneledPort->getPcmLayout();
    } 
    else {
        pcmLayoutOut    = port.getPcmLayout();
        pcmLayoutIn     = tunneledPort->getPcmLayout();
    }

    mParams.nInputBlockSize     = pcmLayoutIn.nBlockSize;
    mParams.nOutputBlockSize    = pcmLayoutOut.nBlockSize;

    if(pcmLayoutIn.nChannels == 0) {
        mParams.nChansIn            = pcmSettings.nChannels;
    } else {
        mParams.nChansIn            = pcmLayoutIn.nChannels;
    }

    mParams.nChansOut           = pcmLayoutOut.nChannels;
    mParams.nNbBuffersIn        = pcmLayoutIn.nNbBuffers;
    mParams.nNbBuffersOut       = pcmLayoutOut.nNbBuffers;
    mParams.nBitPerSampleIn     = pcmLayoutIn.nBitsPerSample;
    mParams.nBitPerSampleOut    = pcmLayoutOut.nBitsPerSample;
    mParams.nBufferSize         = 0; 
    mParams.pBuffer             = 0; 
    mParams.nSampleRate         = AFM::sampleFreq(pcmSettings.nSamplingRate);
}

int PcmAdapter::gcd(int a, int b) {
    int tmp;

    while (b != 0) {
        tmp = b;
        b = a % b;
        a = tmp;
    }

    return a;
}

int 
PcmAdapter::computeBufferSize(void) {
    mParams.nBufferSize = (mParams.nInputBlockSize + mParams.nOutputBlockSize
             - gcd(mParams.nInputBlockSize, mParams.nOutputBlockSize))  * mParams.nChansOut;

    if (mParams.nChansIn > mParams.nChansOut) {
        mParams.nBufferSize += (t_uint24) (mParams.nInputBlockSize 
                * (mParams.nChansIn - mParams.nChansOut));
    }

    OstTraceFiltInst4(TRACE_ALWAYS, "AFM_PROXY: PcmAdapter::computeBufferSize (nInputBlockSize=%d) (nOutputBlockSize=%d) (nChansIn=%d) (nChansOut=%d)",mParams.nInputBlockSize,mParams.nOutputBlockSize,mParams.nChansIn,mParams.nChansOut);
    OstTraceFiltInst1(TRACE_ALWAYS, "AFM_PROXY: PcmAdapter::computeBufferSize (nBufferSize=%d)",mParams.nBufferSize);

    return mParams.nBufferSize;
}

OMX_ERRORTYPE
PcmAdapter::allocatePcmBuffer(void) {
    OMX_ERRORTYPE error;

    mParams.nBufferSize = computeBufferSize();

    error = ENS::allocMpcMemory(
            getNMFDomainHandle(), mPort.getBindingMpcMemoryType24(),
            mParams.nBufferSize, CM_MM_ALIGN_WORD, &mBufferPcm);

    if (error == OMX_ErrorInsufficientResources &&
        mPort.getBindingMpcMemoryType24() != CM_MM_MPC_SDRAM24) {
        OstTraceFiltInst2(TRACE_DEBUG, "PcmAdapter::allocatePcmBuffer failed to allocate %d units of memory type %d, trying SDRAM", mParams.nBufferSize, mPort.getBindingMpcMemoryType24());

        error = ENS::allocMpcMemory(
                getNMFDomainHandle(), CM_MM_MPC_SDRAM24,
                mParams.nBufferSize, CM_MM_ALIGN_WORD, &mBufferPcm);
    }

    if (error != OMX_ErrorNone) return error;

    CM_GetMpcMemoryMpcAddress(mBufferPcm, (t_uint32 *) &mParams.pBuffer);

    return OMX_ErrorNone;
}

OMX_ERRORTYPE
PcmAdapter::freePcmBuffer(void) {
    return ENS::freeMpcMemory(mBufferPcm);
}


OMX_ERRORTYPE PcmAdapter::instantiateBindingComponent(void) 
{
    OMX_ERRORTYPE error;
    OMX_U32 priority = getPortPriorityLevel();
    if(priority == 0){
        OstTraceFiltInst1(TRACE_ALWAYS, "AFM_PROXY: PcmAdapter:: instantiateBindingComponent: pcmadapter instantiated with priority %d",priority+1);
        error = ENS::instantiateNMFComponent(
                getNMFDomainHandle(), "bindings.pcmadapter", 
                "pcmadapter", &mNmfHandle, priority+1);
    }
    else{
        OstTraceFiltInst1(TRACE_ALWAYS, "AFM_PROXY: PcmAdapter:: instantiateBindingComponent: pcmadapter instantiated with priority %d",priority);
        error = ENS::instantiateNMFComponent(
                getNMFDomainHandle(), "bindings.pcmadapter", 
                "pcmadapter", &mNmfHandle, priority); 
    }
    if (error != OMX_ErrorNone) return error;

	error = ENS::bindComponent(
           mNmfHandle, "osttrace", mOstTrace, "osttrace");
    if (error != OMX_ErrorNone) return error;

    OstTraceFiltInst1(TRACE_ALWAYS, "AFM_PROXY: PcmAdapter:: instantiateBindingComponent: synchronisation instantiated with priority %d",priority);
    error = ENS::instantiateNMFComponent(
            getNMFDomainHandle(), "misc.synchronisation", "synchronisation", 
            &mNmfSyncLib, priority);
    if (error != OMX_ErrorNone) return error;
    
    error = ENS::bindComponent(
            mNmfHandle, "synchronisation", mNmfSyncLib, "synchronisation");
    if (error != OMX_ErrorNone) return error;

    error = allocatePcmBuffer();
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponentFromHostEx(mNmfHandle, "configure", 
            &mIconfigure, 1);
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}


OMX_ERRORTYPE PcmAdapter::deInstantiateBindingComponent(void) {
    OMX_ERRORTYPE error;

    error = ENS::unbindComponentFromHost(&mIconfigure);
    if (error != OMX_ErrorNone) return error;

    error = freePcmBuffer();
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

OMX_ERRORTYPE PcmAdapter::configure(void) {

    mIconfigure.setParameter(mParams);

    OstTraceFiltInst4(TRACE_ALWAYS, "AFM_PROXY: PcmAdapter::configure (sampleFreq=%d) (channelNb=%d) (bitPerSample=%d) (endianness=%d)", AFM::OMX_SamplingRate(t_sample_freq(mParams.nSampleRate)), mParams.nChansOut, mParams.nBitPerSampleOut, OMX_EndianBig);
    OstTraceFiltInst2(TRACE_ALWAYS, "AFM_PROXY: PcmAdapter::configure (inputBufSize=%d) (outputBufSize=%d)", mParams.nInputBlockSize, mParams.nOutputBlockSize);

    return OMX_ErrorNone;
}

