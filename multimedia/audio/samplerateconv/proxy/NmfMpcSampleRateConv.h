/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   NmfMpcSampleRateConv.h
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _NMFMPCSAMPLERATECONV_H_
#define _NMFMPCSAMPLERATECONV_H_

#include "NmfComponent.h"

#include "host/setmemory.hpp"
#include "host/requestmemory.hpp"
#include "host/samplerateconv/nmfil/wrapper/configure.hpp"

/// Concrete class implementing a SRC component
/// Derived from NmfComponent
class NmfMpcSampleRateConv: public NmfComponent ,public requestmemoryDescriptor{
public:
    NmfMpcSampleRateConv(OMX_HANDLETYPE omxHandle,
            OMX_U32 nSampleRateIn,
            OMX_U32 nSampleRateOut,
            OMX_U32 nLowMipsToOut48,
            OMX_U32 nBlockSize,
            OMX_U32 nChannels);

    virtual ~NmfMpcSampleRateConv(void);

    virtual OMX_ERRORTYPE instantiate(OMX_U32 domainId, OMX_U32 priority);
    virtual OMX_ERRORTYPE start(void);
    virtual OMX_ERRORTYPE stop(void);
    virtual OMX_ERRORTYPE deInstantiate(void);
    virtual void requestMemory(t_uint16 size);

    OMX_ERRORTYPE configure(void);

private:
    OMX_HANDLETYPE                          mMainCompHdl;

    OMX_U32                                 mSampleRateIn;
    OMX_U32                                 mSampleRateOut;
    OMX_U32                                 mLowMipsToOut48;
    OMX_U32                                 mBlockSize;
    OMX_U32                                 mChannels;

    t_cm_memory_handle                      mMemory;

    t_cm_instance_handle                    mNmfEffectsLib;
    t_cm_instance_handle                    mNmfSyncLib;
    t_cm_instance_handle                    mOstTrace;

    Isamplerateconv_nmfil_wrapper_configure mIconfigure;
    Isetmemory                              mIsetmemory;

    void registerStubsAndSkels(void);
    void unregisterStubsAndSkels(void);

    // Dsp to Host Callbacks
    static void requestMemory_cb(void *ctxt, t_uint16 size);
};

#endif //_NMFMPCSAMPLERATECONV_H_
