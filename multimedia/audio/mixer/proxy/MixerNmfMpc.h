/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   MixerNmfMpc.h
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _MIXER_NMFMPC_H_
#define _MIXER_NMFMPC_H_

#include "AFMNmfMpc_ProcessingComp.h"

#include "Mixer.h"
#include "AFM_MpcHeapMgr.h"

#include "AudioEffectMpcConfigs.h"

#include "host/mixer/nmfil/wrapper/setConfigApplied.hpp"
#include "host/mixer/nmfil/wrapper/configure.hpp"
#include "host/misc/samplesplayed.hpp"

/// Concrete class implementing a Mixer processing component
class MixerNmfMpc: public AFMNmfMpc_ProcessingComp , public mixer_nmfil_wrapper_setConfigAppliedDescriptor{
    public:
        MixerNmfMpc(ENS_Component &enscomp);

        virtual OMX_ERRORTYPE construct(void);
        virtual OMX_ERRORTYPE destroy(void);

        virtual	OMX_ERRORTYPE applyConfig(
                OMX_INDEXTYPE nConfigIndex,
                OMX_PTR pComponentConfigStructure);

        virtual OMX_ERRORTYPE retrieveConfig(
                OMX_INDEXTYPE nConfigIndex,
                OMX_PTR pComponentConfigStructure);

        virtual void newConfigApplied(t_uint32 configARMAddress);

        virtual PcmProbe_t * getProbeConfig(OMX_U32 portIdx, OMX_U32 positionIdx) const;

    protected:
        virtual OMX_ERRORTYPE   doSpecificSendCommand(OMX_COMMANDTYPE eCmd, OMX_U32 nData, OMX_BOOL &bDeferredCmd);
        virtual OMX_ERRORTYPE   doSpecificEventHandler_cb(OMX_EVENTTYPE event, OMX_U32 nData1, OMX_U32 nData2, OMX_BOOL & deferEventHandler);

        virtual OMX_ERRORTYPE   instantiateMain(void);
        virtual OMX_ERRORTYPE   startMain(void);
        virtual OMX_ERRORTYPE   stopMain(void);
        virtual OMX_ERRORTYPE   configureMain(void);
        virtual OMX_ERRORTYPE   deInstantiateMain(void);

        virtual t_cm_instance_handle getNmfHandle(OMX_U32 portIdx) const;
        virtual const char * getNmfSuffix(OMX_U32 portIdx) const;
        virtual bool         supportsPcmProbe() const { return true; };
        virtual OMX_U32      getNmfPortIndex(OMX_U32 omxPortIdx) const { return omxPortIdx; };


    private:

        t_cm_instance_handle	        mNmfEffectsLib;
        t_cm_instance_handle	        mNmfSyncLib;

        //Host to Dsp NMF interfaces
        Imixer_nmfil_wrapper_configure	mIconfigure;

        AFM_MpcHeapMgr *                mHeapMgr;

        static const char *suffixItf[MIXER_INPUT_PORT_COUNT+1];

        // SamplesPlayed stuff
        t_cm_memory_handle              mSPHandle;
        Imisc_samplesplayed             mIsamplesplayed;
        OMX_U32                         mDspAddr;

        // Specific volctrl stuff
        ARMVolCtrlVolumeRampInfo_t *    mVolumeRampInfoPtr[MIXER_INPUT_PORT_COUNT];
        t_cm_memory_handle              mVolumeRampInfoHandle[MIXER_INPUT_PORT_COUNT];

        // private methods
        void            registerStubsAndSkels(void);
        void            unregisterStubsAndSkels(void);
        OMX_ERRORTYPE   instantiateLibraries(void);
        OMX_ERRORTYPE   deInstantiateLibraries(void);
        void            fillInputPortParam(int idx, InputPortParam_t  &inputportparam);
        void            fillOutputPortParam(OutputPortParam_t  &outputportparam);
        OMX_ERRORTYPE   fillPortConfig(OMX_U32 nPortIndex, OMX_U32 *mixerConfigStructDspAddress);
        OMX_ERRORTYPE   fillPortEffectsConfigsToBeCommitted(OMX_U32 nPortIndex, OMX_U32 *effectsConfigStructDspAddress);
        OMX_ERRORTYPE   allocAndFillDSPEffectsConfigStruct(OMX_U32 nbConfigs, OMX_U32 *mixerConfigStructDspAddress, OMX_U32 *specificEffectConfigTableArmAddress);
        OMX_ERRORTYPE   allocAndFillDspConfigStruct(OMX_U32 nPortIndex, ConfigInfo_t *pConfigInfo, ARMSpecificEffectConfig_t *pEffectConfig);

        // Specific samplesPlayed methods
        OMX_ERRORTYPE   initSamplesPlayed(void);
        OMX_ERRORTYPE   retrieveProcessedDataAmount(OMX_SYMBIAN_AUDIO_CONFIG_PROCESSEDDATAAMOUNTTYPE *pConfig);

        // Specific volctrl methods
        OMX_ERRORTYPE   initVolumeRampInfoStructure(OMX_U32 portIdx);
        void inline     freeVolumeRampInfoStructure(OMX_U32 portIdx);
        OMX_S32         convertBalanceIfNeeded(OMX_S32 balance);
        OMX_ERRORTYPE   retrieveVolumeRampConfig(OMX_SYMBIAN_AUDIO_CONFIG_VOLUMERAMPTYPE *pVolumeRamp);

        OMX_U32 getNbConfigsToApply(OMX_U32 nPortIndex) const;

        inline OMX_BOOL isSrcNeeded(OMX_U32 nPortIndex) const {
            DBC_ASSERT(nPortIndex != OUTPUT_PORT_IDX);
            AFM_PcmPort *pInputPcmPort  = static_cast<AFM_PcmPort *>(mENSComponent.getPort(nPortIndex));
            AFM_PcmPort *pOutputPcmPort  = static_cast<AFM_PcmPort *>(mENSComponent.getPort(OUTPUT_PORT_IDX));
            return ((pInputPcmPort->getPcmSettings().nSamplingRate != pOutputPcmPort->getPcmSettings().nSamplingRate)?OMX_TRUE:OMX_FALSE);
        }
};

#endif //_MIXER_NMFMPC_H_

