/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   TEQNmfMpc.h
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _TEQ_NMF_MPC_H_
#define _TEQ_NMF_MPC_H_

#include "AFMNmfMpc_PcmProcessing.h"

#include "host/transducer_equalizer/nmfil/effect/configure.hpp"
#include "TransducerEqualizer.h"


/// @defgroup transducerequalizer
/// @ingroup  afmpcmprocessing
/// @{

/// Concrete class implementing a TransducerEqualizer component
/// Derived from AFMNmfMpc_PcmProcessing
class TEQNmfMpc : public AFMNmfMpc_PcmProcessing
{
    public:
        TEQNmfMpc(ENS_Component &enscomp): AFMNmfMpc_PcmProcessing(enscomp)
        {
            mNmfEffectsLib = 0;
            mNmfMalloc = 0;
        }

        /// Constructor
        virtual OMX_ERRORTYPE construct(void);
        virtual OMX_ERRORTYPE destroy(void);

        virtual OMX_ERRORTYPE instantiateAlgo(void);
        virtual OMX_ERRORTYPE startAlgo(void);
        virtual OMX_ERRORTYPE stopAlgo(void);
        virtual OMX_ERRORTYPE configureAlgo(void);
        virtual OMX_ERRORTYPE deInstantiateAlgo(void);

        virtual	OMX_ERRORTYPE applyConfig(OMX_INDEXTYPE nConfigIndex, OMX_PTR pComponentConfigStructure);

    protected:
        virtual OMX_ERRORTYPE doSpecificSendCommand(OMX_COMMANDTYPE eCmd, OMX_U32 nData, OMX_BOOL &bDeferredCmd);

    private:
        OMX_ERRORTYPE configureAlgo_port_enabled(TransducerEqualizer *proxy);
        OMX_ERRORTYPE setProxyParams            (TransducerEqualizer *proxy);


        //handle of the NMF component which provides the effect interface
        t_cm_instance_handle                         mNmfEffectsLib;
        t_cm_instance_handle                         mNmfMalloc;
        Imalloc_setheap                              mISetHeap;
        Itransducer_equalizer_nmfil_effect_configure mIconfigure;

        bool                                         configureAlgoRequested;
        bool                                         setParamDone;
        bool                                         doConfigureAlgo;
        int                                          paramsNbChannels;
        int                                          nSamplingRate;
        int                                          nChannels;
        int                                          last_nSamplingRate;
        int                                          last_nChannels;

        void                                         registerStubsAndSkels(void);
        void                                         unregisterStubsAndSkels(void);

        t_cm_memory_handle                           ConfigStructHandle;
        t_uint32                                     ConfigStructDspAddress;
        TransducerEqualizerConfigStructFifo_t        *p_TransducerEqualizerConfigStructFifo;
};
/// @}
#endif //  _TEQ_NMF_MPC_H_
