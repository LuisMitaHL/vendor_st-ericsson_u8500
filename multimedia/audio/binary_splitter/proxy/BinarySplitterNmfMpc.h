/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _BINARYSPLITTER_NMFMPC_H_
#define _BINARYSPLITTER_NMFMPC_H_

#include "AFMNmfMpc_ProcessingComp.h"
#include "BinarySplitter.h"
#include "host/binary_splitter/nmfil/wrapper/configure.hpp"

/// Concrete class implementing a Splitter processing component
class BinarySplitterNmfMpc: public AFMNmfMpc_ProcessingComp {
public:

    BinarySplitterNmfMpc(ENS_Component &enscomp);

    virtual OMX_ERRORTYPE construct(void);
    virtual OMX_ERRORTYPE destroy(void);
    virtual OMX_ERRORTYPE doSpecificSendCommand(OMX_COMMANDTYPE eCmd, OMX_U32 nData, OMX_BOOL &bDeferredCmd);
    virtual	OMX_ERRORTYPE applyConfig(
        OMX_INDEXTYPE nConfigIndex,
        OMX_PTR pComponentConfigStructure);

protected:

    virtual OMX_ERRORTYPE   instantiateMain(void);
    virtual OMX_ERRORTYPE   startMain(void);
    virtual OMX_ERRORTYPE   stopMain(void);
    virtual OMX_ERRORTYPE   configureMain(void);
    virtual OMX_ERRORTYPE   deInstantiateMain(void);

    virtual t_cm_instance_handle getNmfHandle(OMX_U32 portIdx) const;
    virtual const char *  getNmfSuffix(OMX_U32 portIdx) const;

private:
    OMX_U32                         mBlockSize;
    t_channelConfig                 mChannelConfig;
    t_OutChannelConfig              mOutChannelConfig;

    // Libraries
    t_cm_instance_handle            mNmfSyncLib;

    //Host to Dsp NMF interfaces
    Ibinary_splitter_nmfil_wrapper_configure mIconfigure;

    void registerStubsAndSkels(void);
    void unregisterStubsAndSkels(void);
};

#endif //_BINARYSPLITTER_NMFMPC_H_

