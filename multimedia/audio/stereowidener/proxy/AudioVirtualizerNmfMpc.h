/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _AUDIOVIRTUALIZER_NMF_MPC_H_
#define _AUDIOVIRTUALIZER_NMF_MPC_H_

#include "AFMNmfMpc_PcmProcessing.h"

#include "host/stereowidener/nmfil/effect/configure.hpp"
/// @defgroup audiovirtualizer
/// @ingroup  afmpcmprocessing
/// @{

// Audio Virtualizer factory method used when registering the AudioVirtualizer
// Derived from AFMNmfMpc_PcmProcessing
class AudioVirtualizerNmfMpc : public AFMNmfMpc_PcmProcessing
{
    public:
        AudioVirtualizerNmfMpc(ENS_Component &enscomp): 
            AFMNmfMpc_PcmProcessing(enscomp) {
                virtualizerEffect = 0;
                mNmfStereoWidenerLib = 0;
                mNmfAudiotablesLib = 0;
                mNmfMalloc = 0;
                mStereowidenerParamsMpc.iChannelsIn  = 0;
                mStereowidenerParamsMpc.iMemoryPreset = 0;
                mStereowidenerParamsMpc.iSamplingFreq = 0;
                mConfigMpc.iEnable = 0;
                mConfigMpc.iHeadphoneMode = 0;
                mConfigMpc.iInternalExternalSpeaker = 0;
                mConfigMpc.iOutputMode  = 0;
                mConfigMpc.iStrength = 0;

            }

        /// Constructor
        virtual OMX_ERRORTYPE  construct(void);
        virtual OMX_ERRORTYPE  destroy(void);
      
        virtual OMX_ERRORTYPE instantiateAlgo(void);
        virtual OMX_ERRORTYPE startAlgo(void);
        virtual OMX_ERRORTYPE stopAlgo(void);
        virtual OMX_ERRORTYPE configureAlgo(void);
        virtual OMX_ERRORTYPE deInstantiateAlgo(void);

        virtual	OMX_ERRORTYPE applyConfig(OMX_INDEXTYPE nConfigIndex,OMX_PTR pComponentConfigStructure);

    private:
        //handle of the NMF component which provides the effect interface

        OMX_S32                                      virtualizerEffect;
    //StereoWidener library
        t_cm_instance_handle                         mNmfStereoWidenerLib;
        t_cm_instance_handle                         mNmfAudiotablesLib;
        t_cm_instance_handle                         mNmfMalloc;
        Imalloc_setheap                              mISetHeap;
        Istereowidener_nmfil_effect_configure        mIconfigure;
        StereowidenerParams_t                        mStereowidenerParamsMpc;
        StereowidenerConfig_t                        mConfigMpc;

        OMX_ERRORTYPE instantiateStereoWidener(void);
        OMX_ERRORTYPE deInstantiateStereoWidener(void);
        OMX_ERRORTYPE startStereoWidener(void);
        OMX_ERRORTYPE stopStereoWidener(void);
        
        void                                         registerStubsAndSkels(void);
        void                                         unregisterStubsAndSkels(void);
};
/// @}
#endif //  _AUDIOVIRTUALIZER_NMF_MPC_H_
