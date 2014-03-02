/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   MdrcNmfHost.h
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _MDRC_NMF_HOST_H_
#define _MDRC_NMF_HOST_H_

#include "AFMNmfHost_PcmProcessing.h"


#include "mdrc/nmfil/host/effect/configure.hpp"
#include "mdrc/nmfil/host/effectWrapped.hpp"
#include "Mdrc.h"
#include "libeffects/libmdrc5b/include/mdrc5b_filters_design.h"


/// @defgroup mdrc
/// @ingroup  afmpcmprocessing
/// @{

/// Concrete class implementing a Mdrc component
/// Derived from AFMNmfHost_PcmProcessing
class MdrcNmfHost : public AFMNmfHost_PcmProcessing
{
    public:
        MdrcNmfHost(ENS_Component &enscomp): AFMNmfHost_PcmProcessing(enscomp) {
                mMdrcParamsHost.mChannels = 0;
                mMdrcParamsHost.mSamplingFreq = 0;
        }

        /// Constructor
        virtual OMX_ERRORTYPE construct(void);
        virtual OMX_ERRORTYPE destroy(void);

        virtual OMX_ERRORTYPE instantiateAlgo(void);
        virtual OMX_ERRORTYPE startAlgo(void);
        virtual OMX_ERRORTYPE stopAlgo(void);
        virtual OMX_ERRORTYPE configureAlgo(void);
        virtual OMX_ERRORTYPE deInstantiateAlgo(void);
        virtual OMX_U32       nbBitPerSampleProcessed(void);

        virtual OMX_ERRORTYPE applyConfig(OMX_INDEXTYPE nConfigIndex,OMX_PTR pComponentConfigStructure);

    private:
        //handle of the NMF component which provides the effect interface
        Imdrc_nmfil_host_effect_configure                 mIconfigureHost;
        MdrcParams_t                                      mMdrcParamsHost;
};
/// @}
#endif //  _MDRC_NMF_HOST_H_
