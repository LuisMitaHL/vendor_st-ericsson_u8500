/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   NmfMpcPcmAdapter.cpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "cm/inc/cm_macros.h"
#include "ENS_Nmf.h"
#include "NmfMpcPcmAdapter.h"
#include "AFM_Utils.h"

AFM_API_EXPORT NmfMpcPcmAdapter::NmfMpcPcmAdapter(
        OMX_U32 nInputBlockSize,
        OMX_U32 nOutputBlockSize,
        OMX_U32 nChansIn,
        OMX_U32 nChansOut,
        OMX_U32 nNbBuffersIn,
        OMX_U32 nNbBuffersOut,
        OMX_U32 nBitPerSampleIn,
        OMX_U32 nBitPerSampleOut,
        OMX_U32 nSamplingRate) : NmfComponent(), 
    mNmfSyncLib(0), mBufferPcm(0)
{
    mParams.nInputBlockSize     = nInputBlockSize;
    mParams.nOutputBlockSize    = nOutputBlockSize;
    mParams.nChansIn            = nChansIn;
    mParams.nChansOut           = nChansOut;
    mParams.nNbBuffersIn        = nNbBuffersIn;
    mParams.nNbBuffersOut       = nNbBuffersOut;
    mParams.nBitPerSampleIn     = nBitPerSampleIn;
    mParams.nBitPerSampleOut    = nBitPerSampleOut;
    mParams.nSampleRate         = AFM::sampleFreq(nSamplingRate);

    mParams.pBuffer             = NULL;
    mParams.nBufferSize         = 0;
    mOstTrace                   = 0;
}


int NmfMpcPcmAdapter::gcd(int a, int b) {
    int tmp;

    while (b != 0) {
        tmp = b;
        b = a % b;
        a = tmp;
    }

    return a;
}

int 
NmfMpcPcmAdapter::computeBufferSize(void) {
    mParams.nBufferSize = (mParams.nInputBlockSize + mParams.nOutputBlockSize
             - gcd(mParams.nInputBlockSize, mParams.nOutputBlockSize))  * mParams.nChansOut;

    if (mParams.nChansIn > mParams.nChansOut) {
        mParams.nBufferSize += (t_uint24) (mParams.nInputBlockSize 
                * (mParams.nChansIn - mParams.nChansOut));
    }

    return mParams.nBufferSize;
}


OMX_ERRORTYPE NmfMpcPcmAdapter::allocatePcmBuffer(void) {
    OMX_ERRORTYPE error;
    
    mParams.nBufferSize = computeBufferSize();

    error = ENS::allocMpcMemory(getNMFDomainHandle(), CM_MM_MPC_SDRAM24, 
            mParams.nBufferSize, CM_MM_ALIGN_WORD, &mBufferPcm);
    if (error != OMX_ErrorNone) return error;

    CM_GetMpcMemoryMpcAddress(mBufferPcm, (t_uint32 *) &mParams.pBuffer);

    return OMX_ErrorNone;
}


OMX_ERRORTYPE NmfMpcPcmAdapter::freePcmBuffer() {
    return ENS::freeMpcMemory(mBufferPcm);
}


AFM_API_EXPORT OMX_ERRORTYPE NmfMpcPcmAdapter::instantiate(OMX_U32 domainId, OMX_U32 priority) 
{
    OMX_ERRORTYPE error;

    setNMFDomainHandle(domainId);
    mMyPriority = priority;

    error = ENS::instantiateNMFComponent(
            getNMFDomainHandle(), "bindings.pcmadapter", "pcmadapter", (t_cm_instance_handle *)&mNmfMain, mMyPriority);
    if (error != OMX_ErrorNone) return error;

	error = ENS::instantiateNMFComponent(
            getNMFDomainHandle(), "osttrace.mmdsp", 
            "OSTTRACE", &mOstTrace, mMyPriority);
    if (error != OMX_ErrorNone) return error;

	error = ENS::bindComponent(
            (t_cm_instance_handle)mNmfMain, "osttrace", mOstTrace, "osttrace");
    if (error != OMX_ErrorNone) return error;

    error = ENS::instantiateNMFComponent(
            getNMFDomainHandle(), "misc.synchronisation", "synchronisation", 
            &mNmfSyncLib, mMyPriority);
    if (error != OMX_ErrorNone) return error;
    
    error = ENS::bindComponent(
            (t_cm_instance_handle)mNmfMain, "synchronisation", mNmfSyncLib, "synchronisation");
    if (error != OMX_ErrorNone) return error;

    error = allocatePcmBuffer();
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponentAsynchronous(
            (t_cm_instance_handle)mNmfMain, "me", (t_cm_instance_handle)mNmfMain, "postevent", 2);
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponentFromHostEx((t_cm_instance_handle)mNmfMain, "configure", 
            &mIconfigure, 1);
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}


AFM_API_EXPORT OMX_ERRORTYPE NmfMpcPcmAdapter::start(void) {
    OMX_ERRORTYPE error;

    error = ENS::startNMFComponent((t_cm_instance_handle)mNmfMain);
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}


AFM_API_EXPORT OMX_ERRORTYPE NmfMpcPcmAdapter::stop(void) {
    OMX_ERRORTYPE error;

    error = ENS::stopNMFComponent((t_cm_instance_handle)mNmfMain);
    if (error != OMX_ErrorNone) return error;
  
    return OMX_ErrorNone;
}


AFM_API_EXPORT OMX_ERRORTYPE NmfMpcPcmAdapter::configure(void) {

    mIconfigure.setParameter(mParams);

    return OMX_ErrorNone;
}


AFM_API_EXPORT OMX_ERRORTYPE NmfMpcPcmAdapter::deInstantiate(void) {
    OMX_ERRORTYPE error;

    error = ENS::unbindComponentFromHost(&mIconfigure);
    if (error != OMX_ErrorNone) return error;

    error = ENS::unbindComponentAsynchronous((t_cm_instance_handle)mNmfMain, "me");
    if (error != OMX_ErrorNone) return error;

    error = freePcmBuffer();
    if (error != OMX_ErrorNone) return error;

	error = ENS::unbindComponent((t_cm_instance_handle)mNmfMain, "osttrace");
    if (error != OMX_ErrorNone) return error;

	error = ENS::destroyNMFComponent(mOstTrace);
    if (error != OMX_ErrorNone) return error;

	error = ENS::unbindComponent((t_cm_instance_handle)mNmfMain, "synchronisation");
    if (error != OMX_ErrorNone) return error;

    error = ENS::destroyNMFComponent(mNmfSyncLib);
    if (error != OMX_ErrorNone) return error;

    error = ENS::destroyNMFComponent((t_cm_instance_handle)mNmfMain); 
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}

