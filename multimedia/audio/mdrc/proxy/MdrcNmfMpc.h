/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _MDRC_NMF_MPC_H_
#define _MDRC_NMF_MPC_H_

#include "AFMNmfMpc_PcmProcessing.h"

#include "host/mdrc/nmfil/effect/configure.hpp"
#include "libeffects/libmdrc5b/include/mdrc5b_filters_design.h"

/// @defgroup mdrc
/// @ingroup  afmpcmprocessing
/// @{

// Mdrc factory method used when registering the Mdrc
// Derived from AFMNmfMpc_PcmProcessing
class MdrcNmfMpc : public AFMNmfMpc_PcmProcessing
{
public:
    MdrcNmfMpc(ENS_Component &enscomp):
            AFMNmfMpc_PcmProcessing(enscomp) {
                mNmfMalloc = 0;
                mNmfEffectsLib = 0;
                mMdrcParamsMpc.mChannels = 0;
                mMdrcParamsMpc.mSamplingFreq = 0;
                }

    /// Constructor
    virtual OMX_ERRORTYPE construct(void);
    virtual OMX_ERRORTYPE destroy(void);

    virtual OMX_ERRORTYPE instantiateAlgo(void);
    virtual OMX_ERRORTYPE startAlgo(void);
    virtual OMX_ERRORTYPE stopAlgo(void);
    virtual OMX_ERRORTYPE configureAlgo(void);
    virtual OMX_ERRORTYPE deInstantiateAlgo(void);

    virtual OMX_ERRORTYPE applyConfig(OMX_INDEXTYPE nConfigIndex,OMX_PTR pComponentConfigStructure);

private:
    //handle of the NMF component which provides the effect interface

    //Mdrc library
    t_cm_instance_handle                         mNmfEffectsLib;
    t_cm_instance_handle                         mNmfMalloc;
    Imalloc_setheap                              mISetHeap;
    Imdrc_nmfil_effect_configure                 mIconfigure;
    MdrcParams_t                                 mMdrcParamsMpc;

    t_cm_memory_handle                           ConfigStructHandle;
    t_uint32                                     ConfigStructDspAddress;
    MdrcConfigStructFifo_t                       *p_MdrcConfigStructFifo;

    void                                         registerStubsAndSkels(void);
    void                                         unregisterStubsAndSkels(void);
};
/// @}
#endif //  _MDRC_NMF_MPC_H_
