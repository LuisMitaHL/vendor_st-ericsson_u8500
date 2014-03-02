/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   TEQNmfHost.h
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _TEQ_NMF_HOST_H_
#define _TEQ_NMF_HOST_H_

#include "AFMNmfHost_PcmProcessing.h"

#include "transducer_equalizer/nmfil/host/effect/configure.hpp"
#include "TransducerEqualizer.h"


/// @defgroup transducerequalizer
/// @ingroup  afmpcmprocessing
/// @{

/// Concrete class implementing a TransducerEqualizer component
/// Derived from AFMNmfHost_PcmProcessing
class TEQNmfHost : public AFMNmfHost_PcmProcessing
{
    public:
        TEQNmfHost(ENS_Component &enscomp): AFMNmfHost_PcmProcessing(enscomp) {}

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

    protected:
        virtual OMX_ERRORTYPE doSpecificSendCommand(OMX_COMMANDTYPE eCmd, OMX_U32 nData, OMX_BOOL &bDeferredCmd);

    private:
        OMX_ERRORTYPE configureAlgo_port_enabled(TransducerEqualizer *proxy);
        OMX_ERRORTYPE setProxyParams            (TransducerEqualizer *proxy);


        //handle of the NMF component which provides the effect interface
        Itransducer_equalizer_nmfil_host_effect_configure mIconfigureHost;

        bool                                              configureAlgoRequested;
        bool                                              setParamDone;
        bool                                              doConfigureAlgo;
        int                                               paramsNbChannels;
        int                                               nSamplingRate;
        int                                               nChannels;
        int                                               last_nSamplingRate;
        int                                               last_nChannels;
};
/// @}
#endif //  _TEQ_NMF_HOST_H_
