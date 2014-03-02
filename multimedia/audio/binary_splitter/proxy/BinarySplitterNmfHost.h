/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 * \file   BinarySplitterNmfHost.h
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _BINARYSPLITTER_NMFHOST_H_
#define _BINARYSPLITTER_NMFHOST_H_

#include "AFMNmfHost_ProcessingComp.h"
#include "BinarySplitter.h"
#include "host/binary_splitter/nmfil/host/wrapper/configure.hpp"

/// Concrete class implementing a BinarySplitter processing component
class BinarySplitterNmfHost: public AFMNmfHost_ProcessingComp {
public:

    BinarySplitterNmfHost(ENS_Component &enscomp);

    virtual OMX_ERRORTYPE construct(void);
    virtual OMX_ERRORTYPE destroy(void);
    //virtual OMX_ERRORTYPE doSpecificSendCommand(OMX_COMMANDTYPE eCmd, OMX_U32 nData, OMX_BOOL &bDeferredCmd);

protected:
    NMF::Composite *  mNmfil;

    virtual OMX_ERRORTYPE   instantiateMain(void);
    virtual OMX_ERRORTYPE   startMain(void);
    virtual OMX_ERRORTYPE   stopMain(void);
    virtual OMX_ERRORTYPE   configureMain(void);
    virtual OMX_ERRORTYPE   deInstantiateMain(void);
    virtual OMX_ERRORTYPE   doSpecificSendCommand(OMX_COMMANDTYPE eCmd, OMX_U32 nData, OMX_BOOL &bDeferredCmd);

    virtual NMF::Composite * getNmfHandle(OMX_U32 portIdx) const;
    virtual const char *     getNmfSuffix(OMX_U32 portIdx) const;
    //   virtual OMX_U32       nbBitPerSampleProcessed(); // Should be 16 or 32

private:
    // Libraries
    Ibinary_splitter_nmfil_host_wrapper_configure        mIconfigure;
    t_pcmprocessing_config_host pcmProcConfig;

    OMX_U32 mBlockSize;

};

#endif //_BINARYSPLITTER_NMFHOST_H_

