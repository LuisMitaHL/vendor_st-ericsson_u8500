/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   HostPcmAdapter.cpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "HostPcmAdapter.h"
#include "hst/bindings/pcmadapterWrapped.hpp"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "afm_proxy_bindings_HostPcmAdapterTraces.h"
#endif

HostPcmAdapter::HostPcmAdapter(AFM_Port &port, AFM_Controller &controller, 
        OMX_HANDLETYPE omxhandle, OMX_BOOL isOutputPort) : HostBindings(port, controller, omxhandle), mBufferPcm(0)
{
    AFM_PcmPort *tunneledPort = static_cast<AFM_PcmPort *>(port.getENSTunneledPort());
    AFM_PcmPort *aPort = static_cast<AFM_PcmPort *>(&port);
    AFM_PARAM_PCMLAYOUTTYPE inputPortPcmLayout,outputPortPcmLayout; 
    OMX_AUDIO_PARAM_PCMMODETYPE pcmSettings = aPort->getPcmSettings();
    
    if (isOutputPort == OMX_TRUE) {
        inputPortPcmLayout = tunneledPort->getPcmLayout();
        outputPortPcmLayout = aPort->getPcmLayout();
    } else {
        inputPortPcmLayout = aPort->getPcmLayout();
        outputPortPcmLayout = tunneledPort->getPcmLayout();
    }

    mParams.nInputBlockSize = outputPortPcmLayout.nBlockSize;
    mParams.nOutputBlockSize = inputPortPcmLayout.nBlockSize;


    if(outputPortPcmLayout.nChannels == 0) {
        mParams.nChansIn            = pcmSettings.nChannels;
    } else {
        mParams.nChansIn            = outputPortPcmLayout.nChannels;
    }

    mParams.nChansOut = inputPortPcmLayout.nChannels;
    mParams.nBitPerSampleIn = outputPortPcmLayout.nBitsPerSample;
    mParams.nBitPerSampleOut = inputPortPcmLayout.nBitsPerSample;
    
    mParams.pBuffer             = NULL;
    mParams.nBufferSize         = 0;

    mParams.nSampleFreq  = pcmSettings.nSamplingRate;
}


int HostPcmAdapter::gcd(int a, int b) {
    int tmp;

    while (b != 0) {
        tmp = b;
        b = a % b;
        a = tmp;
    }
    return a;
}


int HostPcmAdapter::computeBufferSize(void) {
    int producedSize = MAX(mParams.nInputBlockSize * mParams.nChansIn * (mParams.nBitPerSampleIn/8),
                            mParams.nInputBlockSize * mParams.nChansOut * (mParams.nBitPerSampleOut/8)); 
    
    // We first do the conversion for updating the number of bits per sample => Multiply by 2 in order to have enough
    // room to make the conversion from 16 to 32 bits per sample
    if ((mParams.nBitPerSampleOut > mParams.nBitPerSampleIn) && (mParams.nChansOut < mParams.nChansIn)) {
        producedSize = producedSize*2;
    }

    int consumedSize = mParams.nOutputBlockSize * mParams.nBitPerSampleOut/8 * mParams.nChansOut;

    if ((mParams.nOutputBlockSize % mParams.nInputBlockSize) == 0)
    {
             mParams.nBufferSize = producedSize + consumedSize - (mParams.nInputBlockSize * mParams.nChansOut * (mParams.nBitPerSampleOut/8));
    }
    else
    {
        mParams.nBufferSize = producedSize + consumedSize -gcd(producedSize,consumedSize);

        if (mParams.nChansIn > mParams.nChansOut) {
            // Here we take the maximum number of bytes per sample as we do the conversion of byte per sample first
            mParams.nBufferSize +=
                mParams.nInputBlockSize * MAX(mParams.nBitPerSampleIn/8,mParams.nBitPerSampleOut/8) * (mParams.nChansIn - mParams.nChansOut);
        } else if (mParams.nBitPerSampleIn > mParams.nBitPerSampleOut) {
            mParams.nBufferSize += 
                mParams.nInputBlockSize * mParams.nChansIn * 2;
        }
    }
    OstTraceFiltInst4(TRACE_ALWAYS, "AFM_PROXY: HostPcmAdapter::computeBufferSize (nInputBlockSize=%d) (nOutputBlockSize=%d) (nChansIn=%d) (nChansOut=%d)",mParams.nInputBlockSize,mParams.nOutputBlockSize,mParams.nChansIn,mParams.nChansOut);
    OstTraceFiltInst2(TRACE_ALWAYS, "AFM_PROXY: HostPcmAdapter::computeBufferSize (nBitPerSampleIn=%d) (nBitPerSampleOut=%d)",mParams.nBitPerSampleIn,mParams.nBitPerSampleOut);

    OstTraceFiltInst1(TRACE_ALWAYS, "AFM_PROXY: HostPcmAdapter::computeBufferSize (nBufferSize=%d)",mParams.nBufferSize);

    return mParams.nBufferSize;
}

OMX_ERRORTYPE HostPcmAdapter::allocatePcmBuffer(void) {
    
    mParams.nBufferSize = computeBufferSize();

    mBufferPcm = new OMX_U8[mParams.nBufferSize];//malloc(mParams.nBufferSize);

    if (mBufferPcm == NULL) { return OMX_ErrorInsufficientResources; }
    
    // CGI
    mParams.pBuffer = mBufferPcm;

    return OMX_ErrorNone;
}


OMX_ERRORTYPE HostPcmAdapter::freePcmBuffer() {
    delete [](OMX_U8*)mBufferPcm;
    return OMX_ErrorNone;
}

OMX_ERRORTYPE HostPcmAdapter::instantiateBindingComponent()
{
    OMX_ERRORTYPE error;
    t_nmf_error   nmf_error;

    //setNMFDomainHandle(domainId);
    //mMyPriority = priority;

    mNmfHandle = hst_bindings_pcmadapterWrappedCreate();
    if (mNmfHandle == NULL) return OMX_ErrorInsufficientResources;
    
    // Set the priority
    ((hst_bindings_pcmadapterWrapped*)mNmfHandle)->priority=getPortPriorityLevel();

    if (((NMF::Composite*)mNmfHandle)->construct() != NMF_OK) return OMX_ErrorInsufficientResources;

    error = allocatePcmBuffer();
    if (error != OMX_ErrorNone) return error;

    nmf_error = ((NMF::Composite*)mNmfHandle)->bindAsynchronous("me", 2, (NMF::Composite*)mNmfHandle, "postevent", 2);
    if (nmf_error != NMF_OK) return OMX_ErrorInsufficientResources;

    nmf_error = ((NMF::Composite*)mNmfHandle)->bindFromUser("configure", 1, &mIconfigure);
    if (nmf_error != NMF_OK) return OMX_ErrorInsufficientResources;

    return OMX_ErrorNone;
}


OMX_ERRORTYPE HostPcmAdapter::configure(void) {

    mIconfigure.setParameter(mParams);

    OstTraceFiltInst4(TRACE_ALWAYS, "AFM_PROXY: HostPcmAdapter::configure (sampleFreq=%d) (chanNb=%d) (bitPerSample=%d) (endianness=%d)", mParams.nSampleFreq, mParams.nChansOut, mParams.nBitPerSampleOut, OMX_EndianLittle);
    OstTraceFiltInst2(TRACE_ALWAYS, "AFM_PROXY: HostPcmAdapter::configure (inputBufSize=%d) (outputBufSize=%d)", mParams.nInputBlockSize, mParams.nOutputBlockSize);

    return OMX_ErrorNone;
}


OMX_ERRORTYPE HostPcmAdapter::deInstantiateBindingComponent(void) {
    t_nmf_error   nmf_error;

    freePcmBuffer();

    nmf_error = ((NMF::Composite*)mNmfHandle)->unbindAsynchronous("me", (NMF::Composite*)mNmfHandle, "postevent");
    if (nmf_error != NMF_OK) return OMX_ErrorInsufficientResources;

    nmf_error = ((NMF::Composite*)mNmfHandle)->unbindFromUser("configure");
    if (nmf_error != NMF_OK) return OMX_ErrorInsufficientResources;

    if (((NMF::Composite*)mNmfHandle)->destroy() != NMF_OK) {
      return OMX_ErrorInsufficientResources;
    }

    hst_bindings_pcmadapterWrappedDestroy((hst_bindings_pcmadapterWrapped*&)mNmfHandle);

    return OMX_ErrorNone;
}
