/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   NmfMpcSampleRateConv.cpp
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "cm/inc/cm_macros.h"
#include "AFM_Utils.h"
#include "ENS_Nmf.h"
#include "AFM_nmf.h"
#include "NmfMpcSampleRateConv.h"

NmfMpcSampleRateConv::NmfMpcSampleRateConv(
        OMX_HANDLETYPE omxHandle,
        OMX_U32 nSampleRateIn,
        OMX_U32 nSampleRateOut,
        OMX_U32 nLowMipsToOut48,
        OMX_U32 nBlockSize,
        OMX_U32 nChannels)
    : NmfComponent()
{
    mMainCompHdl    = omxHandle;
    mSampleRateIn   = nSampleRateIn;
    mSampleRateOut  = nSampleRateOut;
    mLowMipsToOut48 = nLowMipsToOut48;
    mBlockSize      = nBlockSize;
    mChannels       = nChannels;

    mMemory = 0;
    mNmfEffectsLib = 0;
    mNmfSyncLib = 0;
    mOstTrace = 0;

    registerStubsAndSkels();
}

NmfMpcSampleRateConv::~NmfMpcSampleRateConv() {
    unregisterStubsAndSkels();
}

void NmfMpcSampleRateConv::registerStubsAndSkels(void)
{
    CM_REGISTER_STUBS(samplerateconv_cpp);
}


void NmfMpcSampleRateConv::unregisterStubsAndSkels(void)
{
    CM_UNREGISTER_STUBS(samplerateconv_cpp);
}


void
NmfMpcSampleRateConv::requestMemory(t_uint16 size) {
    t_uint32 dspAddr;

//    MEMORY_TRACE_ENTER("NmfMpcSampleRateConv::requestMemory");

    OMX_ERRORTYPE error = ENS::allocMpcMemory(getNMFDomainHandle(), 
            CM_MM_MPC_TCM24, size, CM_MM_ALIGN_WORD, &mMemory);
    DBC_ASSERT(error == OMX_ErrorNone);

    CM_GetMpcMemoryMpcAddress(mMemory, &dspAddr);

    mIsetmemory.setMemory((void *) dspAddr, size);

//    MEMORY_TRACE_LEAVE("NmfMpcSampleRateConv::requestMemory");
}


OMX_ERRORTYPE NmfMpcSampleRateConv::instantiate(OMX_U32 domainId, OMX_U32 priority) {
    OMX_ERRORTYPE error;

    setNMFDomainHandle(domainId);
    mMyPriority = priority;

    error = ENS::instantiateNMFComponent(
        getNMFDomainHandle(), "osttrace.mmdsp",
        "OSTTRACE", &mOstTrace, mMyPriority);
    if (error != OMX_ErrorNone) return error;

    error = ENS::instantiateNMFComponent(
            getNMFDomainHandle(), "libeffects.mpc.libeffects",
            "libeffects", &mNmfEffectsLib, mMyPriority);
    if (error != OMX_ErrorNone) return error;

    error = ENS::instantiateNMFComponent(
            getNMFDomainHandle(), "samplerateconv.nmfil.wrapper",
            "src", (t_cm_instance_handle *)&mNmfMain, mMyPriority);
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponent(mNmfEffectsLib, "osttrace", mOstTrace, "osttrace");
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponent((t_cm_instance_handle)mNmfMain, "osttrace", mOstTrace, "osttrace");
    if (error != OMX_ErrorNone) return error;

    error = ENS::instantiateNMFComponent(
            getNMFDomainHandle(), "misc.synchronisation", "synchronisation",
            &mNmfSyncLib, mMyPriority);
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponent(
            (t_cm_instance_handle)mNmfMain, "synchronisation", mNmfSyncLib, "synchronisation");
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponent(
            (t_cm_instance_handle)mNmfMain, "libSRC", mNmfEffectsLib, "libresampling");
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponentAsynchronous(
            (t_cm_instance_handle)mNmfMain, "me", (t_cm_instance_handle)mNmfMain, "postevent", 2);
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponentFromHostEx((t_cm_instance_handle)mNmfMain, "configure",
                         &mIconfigure, 1);
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponentFromHostEx((t_cm_instance_handle)mNmfMain, "setmemory",
                         &mIsetmemory, 1);
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponentToHost(
				     mMainCompHdl, (t_cm_instance_handle)mNmfMain, "afm",  static_cast<requestmemoryDescriptor*>(this), 1+1);
    if (error != OMX_ErrorNone) return error;

        return OMX_ErrorNone;
}


OMX_ERRORTYPE NmfMpcSampleRateConv::start(void) {
    OMX_ERRORTYPE error;

    error = ENS::startNMFComponent(mNmfEffectsLib);
    if (error != OMX_ErrorNone) return error;

    error = ENS::startNMFComponent(mOstTrace);
    if (error != OMX_ErrorNone) return error;

    error = ENS::startNMFComponent((t_cm_instance_handle)mNmfMain);
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}


OMX_ERRORTYPE NmfMpcSampleRateConv::stop(void) {
    OMX_ERRORTYPE error;

    error = ENS::stopNMFComponent(mNmfEffectsLib);
    if (error != OMX_ErrorNone) return error;

    error = ENS::stopNMFComponent(mOstTrace);
    if (error != OMX_ErrorNone) return error;

    error = ENS::stopNMFComponent((t_cm_instance_handle)mNmfMain);
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}


OMX_ERRORTYPE NmfMpcSampleRateConv::configure(void) {
    t_src_convtype eConversionType;

    if(mSampleRateIn > mSampleRateOut) {
        eConversionType = SRC_DOWNSAMPLING;
    } else if(mSampleRateIn < mSampleRateOut) {
        eConversionType = SRC_UPSAMPLING;
    } else {
        eConversionType = SRC_UNKNOWN;
    }

    mIconfigure.setParameter(
            AFM::sampleFreq(mSampleRateIn),
            AFM::sampleFreq(mSampleRateOut),
            mLowMipsToOut48,
            eConversionType,
            mBlockSize,
            mChannels,
			0);

        return OMX_ErrorNone;
}


OMX_ERRORTYPE NmfMpcSampleRateConv::deInstantiate(void) {
    OMX_ERRORTYPE error;

    error = ENS::freeMpcMemory(mMemory);
    if (error != OMX_ErrorNone) return error;

    error = ENS::unbindComponent((t_cm_instance_handle)mNmfMain, "libSRC");
    if (error != OMX_ErrorNone) return error;

    error = ENS::unbindComponentAsynchronous((t_cm_instance_handle)mNmfMain, "me");
    if (error != OMX_ErrorNone) return error;

    error = ENS::unbindComponentFromHost( &mIconfigure);
    if (error != OMX_ErrorNone) return error;

    error = ENS::unbindComponentFromHost( &mIsetmemory);
    if (error != OMX_ErrorNone) return error;

    error = ENS::unbindComponentToHost(mMainCompHdl, (t_cm_instance_handle)mNmfMain, "afm");
    if (error != OMX_ErrorNone) return error;

	error = ENS::unbindComponent((t_cm_instance_handle)mNmfMain, "synchronisation");
    if (error != OMX_ErrorNone) return error;

    error = ENS::unbindComponent((t_cm_instance_handle)mNmfMain, "osttrace");
    if (error != OMX_ErrorNone) return error;

    error = ENS::unbindComponent(mNmfEffectsLib, "osttrace");
    if (error != OMX_ErrorNone) return error;

    error = ENS::destroyNMFComponent(mNmfSyncLib);
    if (error != OMX_ErrorNone) return error;

    error = ENS::destroyNMFComponent(mNmfEffectsLib);
    if (error != OMX_ErrorNone) return error;

    error = ENS::destroyNMFComponent(mOstTrace);
    if (error != OMX_ErrorNone) return error;

    error = ENS::destroyNMFComponent((t_cm_instance_handle)mNmfMain);
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}

