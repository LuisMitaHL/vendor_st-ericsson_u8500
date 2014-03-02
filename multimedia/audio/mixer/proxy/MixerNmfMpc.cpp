/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   MixerNmfMpc.cpp
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "cm/inc/cm_macros.h"
#include "audio_chipset_api_index.h"
#include "audio_av_sync_chipset_api.h"
#include "ENS_Nmf.h"
#include "AFM_nmf.h"
#include "AFM_Utils.h"
#include "OMX_CoreExt.h"
#include <math.h>

#include "MixerInputPort.h"

#include "MixerNmfMpc.h"

#include "AudioEffect.h"
#include "AudioEffectMpc.h"
#include "AudioEffectMpcUtils.h"
#include "OMX_STE_AudioEffectsExt.h"
#include "AudioEffectMpcConfigs.h"
#include "libeffects/include/effects_dsp_index.h"
#include "libeffects/include/effects_dsp_types.h"

#include "OstTraceDefinitions_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "mixer_proxy_MixerNmfMpcTraces.h"
#endif
#include "OpenSystemTrace_ste.h"

#define MIXER_INPUT_PORT_SRC_NB_CONFIGS 1
#define MIXER_INPUT_PORT_VOLUME_NB_CONFIGS 4

MixerNmfMpc::MixerNmfMpc(ENS_Component &enscomp)
    : AFMNmfMpc_ProcessingComp(enscomp)
{
    mNmfEffectsLib      = 0;
    mNmfSyncLib         = 0;
    mDspAddr            = 0;
    mSPHandle           = 0;

    for (unsigned int idx = 0; idx < MIXER_INPUT_PORT_COUNT; idx++) {
        mVolumeRampInfoPtr[idx] = NULL;
        mVolumeRampInfoHandle[idx] = 0;
    }
}


void MixerNmfMpc::registerStubsAndSkels(void) {
    CM_REGISTER_STUBS_SKELS(mixer_cpp);
}


void MixerNmfMpc::unregisterStubsAndSkels(void) {
    CM_UNREGISTER_STUBS_SKELS(mixer_cpp);
}

OMX_ERRORTYPE MixerNmfMpc::construct(void)
{
    OMX_ERRORTYPE error;

    registerStubsAndSkels();

    error = AFMNmfMpc_ProcessingComp::construct();
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}

OMX_ERRORTYPE MixerNmfMpc::destroy(void)
{
    OMX_ERRORTYPE error;

    error = AFMNmfMpc_ProcessingComp::destroy();
    if (error != OMX_ErrorNone) return error;

    unregisterStubsAndSkels();

    return OMX_ErrorNone;
}

const char* MixerNmfMpc::suffixItf[]=
{ "", "[0]", "[1]", "[2]", "[3]", "[4]", "[5]", "[6]", "[7]" };

t_cm_instance_handle MixerNmfMpc::getNmfHandle(OMX_U32 portIdx) const {
    DBC_ASSERT(portIdx < mENSComponent.getPortCount());
    return (t_cm_instance_handle) mNmfMain;
}

const char * MixerNmfMpc::getNmfSuffix(OMX_U32 portIdx) const {
    DBC_ASSERT(portIdx < mENSComponent.getPortCount());
    return suffixItf[portIdx];
}

OMX_ERRORTYPE MixerNmfMpc::instantiateLibraries(void) {
    OMX_ERRORTYPE error;

    error = ENS::instantiateNMFComponent(mENSComponent.getNMFDomainHandle(),
            "libeffects.mpc.libeffects", "libeffects", &mNmfEffectsLib, getPortPriorityLevel(0));
    if (error != OMX_ErrorNone) return error;

    error = ENS::instantiateNMFComponent(mENSComponent.getNMFDomainHandle(),
            "misc.synchronisation", "synchronisation", &mNmfSyncLib, getPortPriorityLevel(0));
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponent(mNmfEffectsLib, "osttrace", mOstTrace, "osttrace");
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponent(mNmfMain, "osttrace", mOstTrace, "osttrace");
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponent(mNmfMain, "malloc", mNmfEffectsLib, "libmalloc");
    if(error != OMX_ErrorNone) return error;

    error = ENS::bindComponent(mNmfMain, "libmixer", mNmfEffectsLib, "libmixer");
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponent(mNmfMain, "libeffectsregister", mNmfEffectsLib, "libeffectsregister");
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponent(mNmfMain, "synchronisation", mNmfSyncLib, "synchronisation");
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponent(mNmfMain, "genericfsm", mNmfGenericFsmLib, "genericfsm");
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponent(mNmfMain, "componentfsm", mNmfComponentFsmLib, "componentfsm");
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}

OMX_ERRORTYPE MixerNmfMpc::deInstantiateLibraries(void) {
    OMX_ERRORTYPE error;

    error = ENS::unbindComponent(mNmfMain, "componentfsm");
    if (error != OMX_ErrorNone) return error;

    error = ENS::unbindComponent(mNmfMain, "genericfsm");
    if (error != OMX_ErrorNone) return error;

    error = ENS::unbindComponent(mNmfMain, "libmixer");
    if (error != OMX_ErrorNone) return error;

    error = ENS::unbindComponent(mNmfMain, "malloc");
    if (error != OMX_ErrorNone) return error;

    error = ENS::unbindComponent(mNmfMain, "libeffectsregister");
    if (error != OMX_ErrorNone) return error;

    error = ENS::unbindComponent(mNmfMain, "osttrace");
    if (error != OMX_ErrorNone) return error;

    error = ENS::unbindComponent(mNmfMain, "synchronisation");
    if (error != OMX_ErrorNone) return error;

    error = ENS::unbindComponent(mNmfEffectsLib, "osttrace");
    if (error != OMX_ErrorNone) return error;

    error = ENS::destroyNMFComponent(mNmfEffectsLib);
    if (error != OMX_ErrorNone) return error;

    error = ENS::destroyNMFComponent(mNmfSyncLib);
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}

OMX_ERRORTYPE MixerNmfMpc::instantiateMain(void) {
    OMX_ERRORTYPE error;

    AFM_MpcHeapMgr * mHeapMgr = AFM_MpcHeapMgr::getHandle(mENSComponent.getNMFDomainHandle(), mOstTrace);
    if(mHeapMgr == NULL) {
        OstTraceFiltInst0(TRACE_ERROR,"MixerNmfMpc::instantiateMain AFM_MpcHeapMgr::getHandle failed!");
        return OMX_ErrorInsufficientResources;
    }

    error = ENS::instantiateNMFComponent(mENSComponent.getNMFDomainHandle(),
            "mixer.nmfil.wrapper", "mixer", &mNmfMain, getPortPriorityLevel(0));
    if (error != OMX_ErrorNone) return error;

    error = instantiateLibraries();
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponentAsynchronous(
            mNmfMain, "me", mNmfMain, "postevent", mENSComponent.getPortCount());
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponentFromHostEx(mNmfMain, "configure",
            &mIconfigure, mENSComponent.getPortCount()*10);
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponentToHost(mENSComponent.getOMXHandle(), mNmfMain, "setConfigApplied",
             (mixer_nmfil_wrapper_setConfigAppliedDescriptor *)this, mENSComponent.getPortCount());
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponentFromHostEx(mNmfMain, "samplesplayed",
            &mIsamplesplayed, mENSComponent.getPortCount());
    if (error != OMX_ErrorNone) return error;

    error = initSamplesPlayed();
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}


OMX_ERRORTYPE MixerNmfMpc::deInstantiateMain(void) {
    OMX_ERRORTYPE error;

    if (mSPHandle) {
        ENS::freeMpcMemory(mSPHandle);
    }

    for (unsigned int portIndex = 1; portIndex < MIXER_INPUT_PORT_COUNT+1; portIndex++) {
        freeVolumeRampInfoStructure(portIndex);
    }

    error = ENS::unbindComponentFromHost(&mIsamplesplayed);
    if (error != OMX_ErrorNone) return error;

    error = ENS::unbindComponentFromHost( &mIconfigure);
    if (error != OMX_ErrorNone) return error;

    error = ENS::unbindComponentToHost(mENSComponent.getOMXHandle(), mNmfMain, "setConfigApplied");
    if (error != OMX_ErrorNone) return error;

    error = ENS::unbindComponentAsynchronous(mNmfMain, "me");
    if (error != OMX_ErrorNone) return error;

    error = deInstantiateLibraries();
    if (error != OMX_ErrorNone) return error;

    error = ENS::destroyNMFComponent(mNmfMain);
    if (error != OMX_ErrorNone) return error;

    mHeapMgr->freeHandle();

    return OMX_ErrorNone;
}

OMX_ERRORTYPE MixerNmfMpc::startMain(void) {
    OMX_ERRORTYPE error;

    error = ENS::startNMFComponent(mNmfEffectsLib);
    if (error != OMX_ErrorNone) return error;

    error = ENS::startNMFComponent(mNmfSyncLib);
    if (error != OMX_ErrorNone) return error;

    error = ENS::startNMFComponent(mNmfMain);
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}


OMX_ERRORTYPE MixerNmfMpc::stopMain(void) {
    OMX_ERRORTYPE error;

    error = ENS::stopNMFComponent(mNmfEffectsLib);
    if (error != OMX_ErrorNone) return error;

    error = ENS::stopNMFComponent(mNmfSyncLib);
    if (error != OMX_ErrorNone) return error;

    error = ENS::stopNMFComponent(mNmfMain);
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}

void
MixerNmfMpc::fillOutputPortParam(OutputPortParam_t  &outputportparam) {
    OMX_AUDIO_PARAM_PCMMODETYPE pcmSettings;
    AFM_PARAM_PCMLAYOUTTYPE pcmLayout;
    ENS_Port *port = static_cast<ENS_Port *>(mENSComponent.getPort(0));

    pcmSettings = static_cast<AFM_PcmPort *>(mENSComponent.getPort(0))->getPcmSettings();
    pcmLayout   = static_cast<AFM_PcmPort *>(mENSComponent.getPort(0))->getPcmLayout();

    outputportparam.nSamplingRate   = AFM::sampleFreq(pcmSettings.nSamplingRate);
    outputportparam.nChannels       = pcmLayout.nChannels;
    outputportparam.nBlockSize      = pcmLayout.nBlockSize;
    outputportparam.bDisabled       = !(port->isEnabled());
}

void MixerNmfMpc::fillInputPortParam(int idx, InputPortParam_t  &inputportparam) {
    Mixer *proxy = (Mixer*)&mENSComponent;
    OMX_AUDIO_PARAM_PCMMODETYPE pcmSettings;
    AFM_PARAM_PCMLAYOUTTYPE pcmLayout;
    MixerInputPort * port =  static_cast<MixerInputPort *>(proxy->getPort(idx+1));

    pcmSettings = port->getPcmSettings();
    pcmLayout   = port->getPcmLayout();

    inputportparam.nMemoryBank          = MEM_XTCM;

    // Parameters
    inputportparam.nSamplingRate        = AFM::sampleFreq(pcmSettings.nSamplingRate);
    inputportparam.nChannels            = pcmLayout.nChannels;
    inputportparam.nBlockSize           = pcmLayout.nBlockSize;

    // Configs
    inputportparam.bDisabled            = !(port->isEnabled());
    inputportparam.bPaused              = port->isPaused();
}

OMX_ERRORTYPE MixerNmfMpc::fillPortEffectsConfigsToBeCommitted(OMX_U32 nPortIndex, OMX_U32 *effectsConfigStructDspAddress) {
    Mixer *proxy = (Mixer*)&mENSComponent;
    OMX_U32 specificEffectConfigTableArmAddress = 0;
    MixerPort * port =  static_cast<MixerPort *>(mENSComponent.getPort(nPortIndex));
    OMX_ERRORTYPE err;

    OMX_U32 nbConfigsToCommit = port->getNbConfigsToCommit();
    if(nbConfigsToCommit == 0) {
        *effectsConfigStructDspAddress = 0;
        return OMX_ErrorNone;
    }

    OstTraceFiltInst2(TRACE_DEBUG,"MixerNmfMpc::fillPortConfigToBeCommitted %d configs will be sent to MPC on port %d", nbConfigsToCommit, nPortIndex);

    err = allocAndFillDSPEffectsConfigStruct(nbConfigsToCommit, effectsConfigStructDspAddress, &specificEffectConfigTableArmAddress);
    if(err != OMX_ErrorNone) return err;

    for(unsigned int i = 0; i < nbConfigsToCommit; i++) {
        err = allocAndFillDspConfigStruct(port->getPortIndex(), port->getConfigToCommitInfo(i), (ARMSpecificEffectConfig_t *)specificEffectConfigTableArmAddress);
        if(err != OMX_ErrorNone) {
            OstTraceFiltInst2(TRACE_ERROR,"MixerNmfMpc:: an error (0x%x) has occured in fillPortConfigToBeCommitted portId=%d", err, port->getPortIndex());
            DBC_ASSERT(0);
        }
        specificEffectConfigTableArmAddress += sizeof(ARMSpecificEffectConfig_t);
    }
    port->resetNbConfigsToCommit();

    return OMX_ErrorNone;
}

OMX_U32 MixerNmfMpc::getNbConfigsToApply(OMX_U32 nPortIndex) const {

    MixerPort * port =  static_cast<MixerPort *>(mENSComponent.getPort(nPortIndex));
    OMX_U32 nbConfigsToApply = 0;

    if(nPortIndex != OUTPUT_PORT_IDX) {
        //InputPort configuration(SRC + volctrl)
        if(isSrcNeeded(nPortIndex)) nbConfigsToApply += MIXER_INPUT_PORT_SRC_NB_CONFIGS; //SRC configuration
        nbConfigsToApply += MIXER_INPUT_PORT_VOLUME_NB_CONFIGS; //volctrl configuration
    }

    ENS_ListItem *pEffectItem = port->getEffectNodeList()->getFirst();
    while (pEffectItem != NULL) {
        AudioEffect *pAudioEffect = static_cast<AudioEffect *>(pEffectItem->getData());
        nbConfigsToApply += 1; //Effect insertion on DSP side
        nbConfigsToApply += pAudioEffect->getNbConfigs(); //Effect configurations on DSP side
        pEffectItem = pEffectItem->getNext();
    }
    return nbConfigsToApply;
}


OMX_ERRORTYPE MixerNmfMpc::fillPortConfig(OMX_U32 nPortIndex, OMX_U32 *effectsConfigStructDspAddress) {
    Mixer *proxy = (Mixer*)&mENSComponent;
    OMX_U32 specificEffectConfigTableArmAddress = 0;
    MixerPort * port =  static_cast<MixerPort *>(mENSComponent.getPort(nPortIndex));
    OMX_ERRORTYPE err;

    OMX_U32 nbConfigsToApply = getNbConfigsToApply(nPortIndex);
    if(nbConfigsToApply == 0) {
        *effectsConfigStructDspAddress = 0;
        return OMX_ErrorNone;
    }

    OstTraceFiltInst2(TRACE_DEBUG,"MixerNmfMpc::fillPortConfig %d configs will be sent to MPC on port %d", nbConfigsToApply, nPortIndex);

    err = allocAndFillDSPEffectsConfigStruct(nbConfigsToApply, effectsConfigStructDspAddress, &specificEffectConfigTableArmAddress);
    if(err != OMX_ErrorNone) return err;

    if(nPortIndex != OUTPUT_PORT_IDX) {
        //Send SRC+Volctrl configurations to MPC in case of input port
        ConfigInfo_t sConfigInfo; //Only nConfigIndex field is used for SRC/Volctrl configurations
        if(isSrcNeeded(nPortIndex)){
            sConfigInfo.nConfigIndex = AFM_IndexParamSrcMode;
            err = allocAndFillDspConfigStruct(nPortIndex, &sConfigInfo, (ARMSpecificEffectConfig_t *)specificEffectConfigTableArmAddress);
            if(err != OMX_ErrorNone) {
                OstTraceFiltInst2(TRACE_ERROR,"MixerNmfMpc:: an error (0x%x) has occured in fillPortConfig when configuring SRC (AFM_IndexParamSrcMode) on portId=%d", err, nPortIndex);
                DBC_ASSERT(0);
            }
            specificEffectConfigTableArmAddress += sizeof(ARMSpecificEffectConfig_t);
        }
        sConfigInfo.nConfigIndex = OMX_IndexConfigAudioVolume;
        err = allocAndFillDspConfigStruct(nPortIndex, &sConfigInfo, (ARMSpecificEffectConfig_t *)specificEffectConfigTableArmAddress);
        if(err != OMX_ErrorNone) {
            OstTraceFiltInst2(TRACE_ERROR,"MixerNmfMpc:: an error (0x%x) has occured in fillPortConfig when configuring volctrl (OMX_IndexConfigAudioVolume) on portId=%d", err, nPortIndex);
            DBC_ASSERT(0);
        }
        specificEffectConfigTableArmAddress += sizeof(ARMSpecificEffectConfig_t);
        sConfigInfo.nConfigIndex = OMX_IndexConfigAudioMute;
        err = allocAndFillDspConfigStruct(nPortIndex, &sConfigInfo, (ARMSpecificEffectConfig_t *)specificEffectConfigTableArmAddress);
        if(err != OMX_ErrorNone) {
            OstTraceFiltInst2(TRACE_ERROR,"MixerNmfMpc:: an error (0x%x) has occured in fillPortConfig when configuring volctrl (OMX_IndexConfigAudioMute) on portId=%d", err, nPortIndex);
            DBC_ASSERT(0);
        }
        specificEffectConfigTableArmAddress += sizeof(ARMSpecificEffectConfig_t);
        sConfigInfo.nConfigIndex = OMX_IndexConfigAudioBalance;
        err = allocAndFillDspConfigStruct(nPortIndex, &sConfigInfo, (ARMSpecificEffectConfig_t *)specificEffectConfigTableArmAddress);
        if(err != OMX_ErrorNone) {
            OstTraceFiltInst2(TRACE_ERROR,"MixerNmfMpc:: an error (0x%x) has occured in fillPortConfig when configuring volctrl (OMX_IndexConfigAudioBalance) on portId=%d", err, nPortIndex);
            DBC_ASSERT(0);
        }
        specificEffectConfigTableArmAddress += sizeof(ARMSpecificEffectConfig_t);
        sConfigInfo.nConfigIndex = OMX_IndexConfigAudioVolumeRamp;
        err = allocAndFillDspConfigStruct(nPortIndex, &sConfigInfo, (ARMSpecificEffectConfig_t *)specificEffectConfigTableArmAddress);
        if(err != OMX_ErrorNone) {
            OstTraceFiltInst2(TRACE_ERROR,"MixerNmfMpc:: an error (0x%x) has occured in fillPortConfig when configuring volctrl (OOMX_IndexConfigAudioVolumeRamp) on portId=%d", err, nPortIndex);
            DBC_ASSERT(0);
        }
        specificEffectConfigTableArmAddress += sizeof(ARMSpecificEffectConfig_t);
    }

    //Send Effect configurations to MPC
    ENS_ListItem *pEffectItem = port->getEffectNodeList()->getFirst();
    while (pEffectItem != NULL) {
        AudioEffect *pAudioEffect = static_cast<AudioEffect *>(pEffectItem->getData());
        //Effect insertion on DSP side (OMX_IndexConfigAudioEffectAddPosition)
        ConfigInfo_t sConfigInfo;
        sConfigInfo.nConfigIndex = (OMX_INDEXTYPE)OMX_IndexConfigAudioEffectAddPosition;
        sConfigInfo.nPosition = pAudioEffect->getPosition();
        err = allocAndFillDspConfigStruct(nPortIndex, &sConfigInfo, (ARMSpecificEffectConfig_t *)specificEffectConfigTableArmAddress);
        if(err != OMX_ErrorNone) {
            OstTraceFiltInst3(TRACE_ERROR,"MixerNmfMpc:: an error (0x%x) has occured in fillPortConfig when adding effect at positionId=%d on portId=%d", err, pAudioEffect->getPosition(), nPortIndex);
            DBC_ASSERT(0);
        }
        specificEffectConfigTableArmAddress += sizeof(ARMSpecificEffectConfig_t);
        for(unsigned int i = 0; i < pAudioEffect->getNbConfigs(); i++) {
            //Effect configuration on DSP side (OMX_IndexConfigAudioEffectConfigure)
            ConfigInfo_t sConfigInfo;
            sConfigInfo.nConfigIndex = (OMX_INDEXTYPE)OMX_IndexConfigAudioEffectConfigure;
            sConfigInfo.nPosition = pAudioEffect->getPosition();
            sConfigInfo.nEffectSpecificIndex = pAudioEffect->getConfigIndex(i);
            err = allocAndFillDspConfigStruct(nPortIndex, &sConfigInfo, (ARMSpecificEffectConfig_t *)specificEffectConfigTableArmAddress);
            if(err != OMX_ErrorNone) {
                OstTraceFiltInst3(TRACE_ERROR,"MixerNmfMpc:: an error (0x%x) has occured in fillPortConfig when configuring effect at positionId=%d on portId=%d", err, pAudioEffect->getPosition(), port->getPortIndex());
                DBC_ASSERT(0);
            }
            specificEffectConfigTableArmAddress += sizeof(ARMSpecificEffectConfig_t);
        }
        pEffectItem = pEffectItem->getNext();
    }

    return OMX_ErrorNone;
}

OMX_ERRORTYPE MixerNmfMpc::configureMain(void) {
    MixerParam_t mixerParam;
    OMX_U32 effectsConfigStructDspAddress = 0;
    OMX_ERRORTYPE error = OMX_ErrorNone;

    //Don't configure DSP part if output port is disabed : will be configured at enablePort
    if(!mENSComponent.getPort(0)->isEnabled()) return OMX_ErrorNone;

    fillOutputPortParam(mixerParam.outputPortParam);

    for (unsigned int portIndex = 1; portIndex < mENSComponent.getPortCount(); portIndex++) {
        fillInputPortParam(portIndex-1, mixerParam.inputPortParam[portIndex-1]);
    }

    mixerParam.traceAddr  = getDspAddr();

    mIconfigure.setParameter(mixerParam);

    mIsamplesplayed.SetSamplesPlayedAddr((void *)mDspAddr);

    //Configure DSP enabled input ports
    for(unsigned int portIndex = 0; portIndex < MIXER_INPUT_PORT_COUNT+1; portIndex++) {
        //Don't configure DSP part if input port is disabed : will be configured at enablePort
        MixerPort * port =  static_cast<MixerPort *>(mENSComponent.getPort(portIndex));
        if(!port->isEnabled()) continue;
        if(portIndex!=0) {
            //If input port then initialize mVolumeRampInfoPtr to get volumeramp informations
            error = initVolumeRampInfoStructure(portIndex);
            if(error != OMX_ErrorNone) return error;
        }
        //Fill Port dynamic configurations
        error = fillPortConfig(portIndex, &effectsConfigStructDspAddress);
        if(error != OMX_ErrorNone) {
            OstTraceFiltInst2(TRACE_ERROR,"Mixer fillPortConfig portId=%d return error (%d)", portIndex, error);
            return error;
        } else if(effectsConfigStructDspAddress != 0) {
            //don't need to call configureEffects method when all mixer ports are disabled or if
            //IL Client is using default configurations.
            mIconfigure.configureEffects(effectsConfigStructDspAddress);
        }
    }

    return OMX_ErrorNone;
}


OMX_ERRORTYPE MixerNmfMpc::doSpecificSendCommand(
        OMX_COMMANDTYPE eCmd,
        OMX_U32 nData,
        OMX_BOOL &bDeferredCmd)
{
    if (eCmd == OMX_CommandPortEnable) {
        if (nData == 0) {
            configureMain();
        } else {
            if(mENSComponent.getPort(0)->isEnabled()) {
                //Initialize DSP input port only if output port is enabled
                InputPortParam_t inputportparam;
                OMX_ERRORTYPE error = OMX_ErrorNone;
                OMX_U32 effectsConfigStructDspAddress = 0;
                fillInputPortParam(nData-1, inputportparam);
                mIconfigure.setInputPortParameter(nData, inputportparam);
                //Initialize mVolumeRampInfoPtr to get volumeramp informations
                DBC_ASSERT(initVolumeRampInfoStructure(nData) == OMX_ErrorNone);
                //Fill InputPort configurations
                DBC_ASSERT(fillPortConfig(nData, &effectsConfigStructDspAddress) == OMX_ErrorNone);
                if(effectsConfigStructDspAddress != 0) {
                    mIconfigure.configureEffects(effectsConfigStructDspAddress);
                }
            }
        }
    }

    bDeferredCmd = OMX_FALSE;

    return AFMNmfMpc_ProcessingComp::doSpecificSendCommand(eCmd, nData, bDeferredCmd);
}


OMX_ERRORTYPE MixerNmfMpc::doSpecificEventHandler_cb(OMX_EVENTTYPE event, OMX_U32 nData1, OMX_U32 nData2, OMX_BOOL & deferEventHandler)
{
    deferEventHandler = OMX_FALSE;

    if(event == (OMX_EVENTTYPE)OMX_EventIndexSettingChanged && nData2 == (OMX_U32)0x1){
        // The audio OMX index are not seen by the DSP
        // We decided to do the conversion here
        nData2 = OMX_IndexConfigAudioVolumeRamp;
        deferEventHandler = OMX_TRUE;
        mENSComponent.eventHandler(event, nData1, nData2);
    }
    else if(event == OMX_EventCmdComplete && nData1 == OMX_CommandPortDisable) {
        if(nData2 == OMX_ALL || nData2 == 0) {
            for (unsigned int portIndex = 1; portIndex < MIXER_INPUT_PORT_COUNT+1; portIndex++) {
                freeVolumeRampInfoStructure(portIndex);
            }
        } else {
            freeVolumeRampInfoStructure(nData2);
        }
    }

    return AFMNmfMpc_ProcessingComp::doSpecificEventHandler_cb(event,nData1,nData2,deferEventHandler);
}

OMX_ERRORTYPE MixerNmfMpc::allocAndFillDSPEffectsConfigStruct(OMX_U32 nbConfigs, OMX_U32 *effectsConfigStructDspAddress, OMX_U32 *specificEffectConfigTableArmAddress) {
    Mixer *proxy = (Mixer*)&mENSComponent;
    ARMEffectsConfig_t   *pMixerConfigMpc = NULL;
    OMX_U32 effectsConfigStructArmAddress = 0, specificEffectConfigTableDspAddress = 0;
    t_cm_memory_handle effectsConfigStructHandle = 0, specificEffectConfigTableHandle = 0;

    OstTraceFiltInst1(TRACE_DEBUG,"MixerNmfMpc::allocAndFillDSPEffectsConfigStruct nbConfigs to be applied=%d", nbConfigs);

    effectsConfigStructHandle = AudioEffectMpcUtils::allocDSPMemoryForSetConfig(mENSComponent.getNMFDomainHandle(),
            sizeof(ARMEffectsConfig_t), &effectsConfigStructArmAddress, effectsConfigStructDspAddress);
    if(!effectsConfigStructHandle) return OMX_ErrorInsufficientResources;

    specificEffectConfigTableHandle = AudioEffectMpcUtils::allocDSPMemoryForSetConfig(mENSComponent.getNMFDomainHandle(),
            nbConfigs*sizeof(ARMEffectsConfig_t), specificEffectConfigTableArmAddress, &specificEffectConfigTableDspAddress);
    if(!specificEffectConfigTableHandle) return OMX_ErrorInsufficientResources;

    pMixerConfigMpc = (ARMEffectsConfig_t *) effectsConfigStructArmAddress;
    pMixerConfigMpc->nbConfigs                                  = nbConfigs;
    pMixerConfigMpc->pEffectConfigTable                         = specificEffectConfigTableDspAddress;
    pMixerConfigMpc->effectsConfigStructArmAddress_high         = effectsConfigStructArmAddress >> 24;
    pMixerConfigMpc->effectsConfigStructArmAddress_low          = effectsConfigStructArmAddress & 0xffffff;
    pMixerConfigMpc->effectsConfigStructHandle_high             = effectsConfigStructHandle >> 24;
    pMixerConfigMpc->effectsConfigStructHandle_low              = effectsConfigStructHandle & 0xffffff;
    pMixerConfigMpc->specificEffectConfigTableArmAddress_high   = (*specificEffectConfigTableArmAddress) >> 24;
    pMixerConfigMpc->specificEffectConfigTableArmAddress_low    = (*specificEffectConfigTableArmAddress) & 0xffffff;
    pMixerConfigMpc->specificEffectConfigTableHandle_high       = specificEffectConfigTableHandle >> 24;
    pMixerConfigMpc->specificEffectConfigTableHandle_low        = specificEffectConfigTableHandle & 0xffffff;

    return OMX_ErrorNone;
}

OMX_ERRORTYPE MixerNmfMpc::allocAndFillDspConfigStruct(OMX_U32 nPortIndex, ConfigInfo_t *pConfigInfo, ARMSpecificEffectConfig_t *pEffectConfig) {

    Mixer *proxy = (Mixer*)&mENSComponent;
    t_cm_memory_handle configStructHandle = 0;
    OMX_U32 configStructArmAddress = 0, configStructDspAddress = 0;

    switch (pConfigInfo->nConfigIndex) {
        case AFM_IndexParamSrcMode:
            {
                MixerInputPort * inport =  static_cast<MixerInputPort *>(mENSComponent.getPort(nPortIndex));

                configStructHandle = AudioEffectMpcUtils::allocDSPMemoryForSetConfig(mENSComponent.getNMFDomainHandle(),
                        sizeof(ARMParamSrcMode_t), &configStructArmAddress, &configStructDspAddress);
                if(!configStructHandle) return OMX_ErrorInsufficientResources;

                pEffectConfig->portIdx                              = nPortIndex;
                pEffectConfig->effectPosition                       = INPUT_SRC_POSITION;
                pEffectConfig->configIndex                          = AFM_DSP_IndexParamSrcMode;
                pEffectConfig->isStaticConfiguration                = true;
                pEffectConfig->pSpecificConfigStruct                = configStructDspAddress;
                pEffectConfig->specificConfigStructHandle_high      = configStructHandle >> 24;
                pEffectConfig->specificConfigStructHandle_low       = configStructHandle & 0xffffff;

                ARMParamSrcMode_t *pConfigMpc = (ARMParamSrcMode_t *) configStructArmAddress;
                pConfigMpc->srcMode                                 = inport->getSrcMode();

                OstTraceFiltInst2(TRACE_DEBUG,"MixerNmfMpc::allocAndFillDspConfigStruct AFM_IndexParamSrcMode portidx=%d srcMode=%d", nPortIndex, pConfigMpc->srcMode);

                return OMX_ErrorNone;
            }
        case OMX_IndexConfigAudioVolume:
            {
                MixerInputPort * inport =  static_cast<MixerInputPort *>(mENSComponent.getPort(nPortIndex));
                OMX_AUDIO_CONFIG_VOLUMETYPE *pVolume = inport->getConfigVolume();

                configStructHandle = AudioEffectMpcUtils::allocDSPMemoryForSetConfig(mENSComponent.getNMFDomainHandle(),
                        sizeof(ARMVolCtrlConfigVolume_t), &configStructArmAddress, &configStructDspAddress);
                if(!configStructHandle) return OMX_ErrorInsufficientResources;

                pEffectConfig->portIdx                              = nPortIndex;
                pEffectConfig->effectPosition                       = INPUT_VOLCTRL_POSITION;
                pEffectConfig->configIndex                          = OMX_DSP_IndexConfigAudioVolume;
                pEffectConfig->isStaticConfiguration                = false;
                pEffectConfig->pSpecificConfigStruct                = configStructDspAddress;
                pEffectConfig->specificConfigStructHandle_high      = configStructHandle >> 24;
                pEffectConfig->specificConfigStructHandle_low       = configStructHandle & 0xffffff;

                ARMVolCtrlConfigVolume_t * pConfigMpc = (ARMVolCtrlConfigVolume_t *) configStructArmAddress;
                pConfigMpc->bLinear                       = pVolume->bLinear;
                pConfigMpc->sVolume.nValue                = pVolume->sVolume.nValue;
                pConfigMpc->sVolume.nMin                  = pVolume->sVolume.nMin;
                pConfigMpc->sVolume.nMax                  = pVolume->sVolume.nMax;

                OstTraceFiltInst2(TRACE_DEBUG,"MixerNmfMpc::allocAndFillDspConfigStruct OMX_IndexConfigAudioVolume portidx=%d Volume=%d", nPortIndex, pVolume->sVolume.nValue);

                return OMX_ErrorNone;
            }

        case OMX_IndexConfigAudioMute:
            {
                MixerInputPort * inport =  static_cast<MixerInputPort *>(mENSComponent.getPort(nPortIndex));
                OMX_AUDIO_CONFIG_MUTETYPE *pMute = inport->getConfigMute();

                configStructHandle = AudioEffectMpcUtils::allocDSPMemoryForSetConfig(mENSComponent.getNMFDomainHandle(),
                        sizeof(ARMVolCtrlConfigMute_t), &configStructArmAddress, &configStructDspAddress);
                if(!configStructHandle) return OMX_ErrorInsufficientResources;

                pEffectConfig->portIdx                            = pMute->nPortIndex;
                pEffectConfig->effectPosition                     = INPUT_VOLCTRL_POSITION;
                pEffectConfig->configIndex                        = OMX_DSP_IndexConfigAudioMute;
                pEffectConfig->isStaticConfiguration              = false;
                pEffectConfig->pSpecificConfigStruct              = configStructDspAddress;
                pEffectConfig->specificConfigStructHandle_high    = configStructHandle >> 24;
                pEffectConfig->specificConfigStructHandle_low     = configStructHandle & 0xffffff;

                ARMVolCtrlConfigMute_t * pConfigMpc = (ARMVolCtrlConfigMute_t *) configStructArmAddress;
                pConfigMpc->bMute   = pMute->bMute;

                OstTraceFiltInst2(TRACE_DEBUG,"MixerNmfMpc::allocAndFillDspConfigStruct OMX_IndexConfigAudioMute portidx=%d Mute=%d", pMute->nPortIndex, pMute->bMute);

                return OMX_ErrorNone;
            }

        case OMX_IndexConfigAudioBalance:
            {
                MixerInputPort * inport =  static_cast<MixerInputPort *>(mENSComponent.getPort(nPortIndex));
                OMX_AUDIO_CONFIG_BALANCETYPE *pBalance = inport->getConfigBalance();

                configStructHandle = AudioEffectMpcUtils::allocDSPMemoryForSetConfig(mENSComponent.getNMFDomainHandle(),
                        sizeof(ARMVolCtrlConfigBalance_t), &configStructArmAddress, &configStructDspAddress);
                if(!configStructHandle) return OMX_ErrorInsufficientResources;

                pEffectConfig->portIdx                            = pBalance->nPortIndex;
                pEffectConfig->effectPosition                     = INPUT_VOLCTRL_POSITION;
                pEffectConfig->configIndex                        = OMX_DSP_IndexConfigAudioBalance;
                pEffectConfig->isStaticConfiguration              = false;
                pEffectConfig->pSpecificConfigStruct              = configStructDspAddress;
                pEffectConfig->specificConfigStructHandle_high    = configStructHandle >> 24;
                pEffectConfig->specificConfigStructHandle_low     = configStructHandle & 0xffffff;

                ARMVolCtrlConfigBalance_t * pConfigMpc = (ARMVolCtrlConfigBalance_t *) configStructArmAddress;
                //TODO: move balance conversion on DSP side?
                pConfigMpc->nBalance = pBalance->nBalance;
                pConfigMpc->nBalance = convertBalanceIfNeeded(pConfigMpc->nBalance);

                OstTraceFiltInst2(TRACE_DEBUG,"MixerNmfMpc::allocAndFillDspConfigStruct OMX_IndexConfigAudioBalance portidx=%d Balance=%d", pBalance->nPortIndex, pBalance->nBalance);

                return OMX_ErrorNone;
            }

        case OMX_IndexConfigAudioVolumeRamp:
            {
                MixerInputPort * inport =  static_cast<MixerInputPort *>(mENSComponent.getPort(nPortIndex));
                OMX_SYMBIAN_AUDIO_CONFIG_VOLUMERAMPTYPE *pRampVolume = inport->getConfigVolumeRamp();

                configStructHandle = AudioEffectMpcUtils::allocDSPMemoryForSetConfig(mENSComponent.getNMFDomainHandle(),
                        sizeof(ARMVolCtrlConfigVolumeRamp_t), &configStructArmAddress, &configStructDspAddress);
                if(!configStructHandle) return OMX_ErrorInsufficientResources;

                pEffectConfig->portIdx                            = pRampVolume->nPortIndex;
                pEffectConfig->effectPosition                     = INPUT_VOLCTRL_POSITION;
                pEffectConfig->configIndex                        = OMX_DSP_IndexConfigAudioVolumeRamp;
                pEffectConfig->isStaticConfiguration              = false;
                pEffectConfig->pSpecificConfigStruct              = configStructDspAddress;
                pEffectConfig->specificConfigStructHandle_high    = configStructHandle >> 24;
                pEffectConfig->specificConfigStructHandle_low     = configStructHandle & 0xffffff;

                ARMVolCtrlConfigVolumeRamp_t * pConfigMpc = (ARMVolCtrlConfigVolumeRamp_t *) configStructArmAddress;
                pConfigMpc->nChannel                            = pRampVolume->nChannel;
                pConfigMpc->bLinear                             = pRampVolume->bLinear;
                pConfigMpc->sStartVolume.nValue                 = pRampVolume->sStartVolume.nValue;
                pConfigMpc->sStartVolume.nMin                   = pRampVolume->sStartVolume.nMin;
                pConfigMpc->sStartVolume.nMax                   = pRampVolume->sStartVolume.nMax;
                pConfigMpc->sEndVolume.nValue                   = pRampVolume->sEndVolume.nValue;
                pConfigMpc->sEndVolume.nMin                     = pRampVolume->sEndVolume.nMin;
                pConfigMpc->sEndVolume.nMax                     = pRampVolume->sEndVolume.nMax;
                pConfigMpc->nRampDuration                       = pRampVolume->nRampDuration;
                pConfigMpc->bRampTerminate                      = pRampVolume->bRampTerminate;
                pConfigMpc->sCurrentVolume.nValue               = pRampVolume->sCurrentVolume.nValue;
                pConfigMpc->sCurrentVolume.nMin                 = pRampVolume->sCurrentVolume.nMin;
                pConfigMpc->sCurrentVolume.nMax                 = pRampVolume->sCurrentVolume.nMax;
                pConfigMpc->nRampCurrentTime_high               = pRampVolume->nRampCurrentTime >> 24;
                pConfigMpc->nRampCurrentTime_low                = pRampVolume->nRampCurrentTime & 0xffffff;
                pConfigMpc->nRampMinDuration                    = pRampVolume->nRampMinDuration;
                pConfigMpc->nRampMaxDuration                    = pRampVolume->nRampMaxDuration;
                pConfigMpc->nVolumeStep                         = pRampVolume->nVolumeStep;

                OstTraceFiltInst2(TRACE_DEBUG,"MixerNmfMpc::allocAndFillDspConfigStruct OMX_IndexConfigAudioVolumeRamp portidx=%d nRampDuration=%d", pRampVolume->nPortIndex, pConfigMpc->nRampDuration);

                return OMX_ErrorNone;
            }

        case OMX_IndexConfigAudioEffectAddPosition:
            {
                MixerPort * port =  static_cast<MixerPort *>(mENSComponent.getPort(nPortIndex));

                configStructHandle = AudioEffectMpcUtils::allocDSPMemoryForSetConfig(mENSComponent.getNMFDomainHandle(),
                        sizeof(ARMConfigAddEffect_t), &configStructArmAddress, &configStructDspAddress);
                if(!configStructHandle) return OMX_ErrorInsufficientResources;

                AudioEffect *pEffectNode = port->getEffectNode(pConfigInfo->nPosition);

                pEffectConfig->portIdx                            = nPortIndex;
                pEffectConfig->effectPosition                     = pConfigInfo->nPosition;
                pEffectConfig->configIndex                        = OMX_DSP_IndexConfigAudioEffectAddPosition;
                pEffectConfig->isStaticConfiguration              = false;
                pEffectConfig->pSpecificConfigStruct              = configStructDspAddress;
                pEffectConfig->specificConfigStructHandle_high    = configStructHandle >> 24;
                pEffectConfig->specificConfigStructHandle_low     = configStructHandle & 0xffffff;

                ARMConfigAddEffect_t *pConfigMpc = (ARMConfigAddEffect_t *) configStructArmAddress;
                const char *pNameSrc = pEffectNode->getName();
                char *pNameDest = (char *)pConfigMpc->cEffectName;
                //Reset Memory
                for(unsigned int i=0; i<128; i++) {
                    pConfigMpc->cEffectName[i] = 0;
                }
                while(*pNameSrc != '\0') { //Special Name's copy for easy reading on DSP side.
                    if(((int)pNameDest)%4 == 0) {
                        //*pNameDest++ = *pNameSrc++;
                        *pNameDest = *pNameSrc;
                        *pNameDest++;
                        *pNameSrc++;
                    } else {
                        *pNameDest++;
                    }
                }
                *pNameDest='\0';

                OstTraceFiltInst3(TRACE_DEBUG,"MixerNmfMpc::allocAndFillDspConfigStruct OMX_IndexConfigAudioEffectAddPosition portidx=%d position=%d pEffectNode=0x%x", nPortIndex, pEffectNode->getPosition(), (unsigned int)pEffectNode);

                return OMX_ErrorNone;
            }

        case OMX_IndexConfigAudioEffectRemovePosition:
            {
                MixerPort * port =  static_cast<MixerPort *>(mENSComponent.getPort(nPortIndex));

                pEffectConfig->portIdx                            = nPortIndex;
                pEffectConfig->effectPosition                     = pConfigInfo->nPosition;
                pEffectConfig->configIndex                        = OMX_DSP_IndexConfigAudioEffectRemovePosition;
                pEffectConfig->isStaticConfiguration              = false;
                pEffectConfig->pSpecificConfigStruct              = 0;
                pEffectConfig->specificConfigStructHandle_high    = 0;
                pEffectConfig->specificConfigStructHandle_low     = 0;

                OstTraceFiltInst2(TRACE_DEBUG,"MixerNmfMpc::allocAndFillDspConfigStruct OMX_IndexConfigAudioEffectRemovePosition portidx=%d position=%d", nPortIndex, pConfigInfo->nPosition);

                return OMX_ErrorNone;
            }

        case OMX_IndexConfigAudioEffectConfigure:
            {
                MixerPort *port = static_cast<MixerPort *>(mENSComponent.getPort(nPortIndex));
                AudioEffect *pEffectNode = port->getEffectNode(pConfigInfo->nPosition);
                DBC_ASSERT(pEffectNode != NULL);
                AudioEffectMpc &effectNodeMpc = static_cast<AudioEffectMpc &>(pEffectNode->getCoreSpecificPart());
                return effectNodeMpc.allocAndFillDspConfigStruct(pConfigInfo->nEffectSpecificIndex, pEffectConfig);
            }

        default:
            {
                DBC_ASSERT(0);
            }
    }

    return OMX_ErrorNone;
}

void MixerNmfMpc::newConfigApplied(t_uint32 configARMAddress)
{
    ARMEffectsConfig_t * pMixerConfig = (ARMEffectsConfig_t *) configARMAddress;
    OMX_U32 effectsConfigStructHandle = (pMixerConfig->effectsConfigStructHandle_high << 24) | (pMixerConfig->effectsConfigStructHandle_low & 0xFFFFFF);
    OMX_U32 specificEffectConfigTableArmAddress = (pMixerConfig->specificEffectConfigTableArmAddress_high << 24) | (pMixerConfig->specificEffectConfigTableArmAddress_low & 0xFFFFFF);
    OMX_U32 specificEffectConfigTableHandle = (pMixerConfig->specificEffectConfigTableHandle_high << 24) | (pMixerConfig->specificEffectConfigTableHandle_low & 0xFFFFFF);

    OstTraceFiltInst1(TRACE_DEBUG,"MixerNmfMpc::newConfigApplied, nbConfigs applied=%d", pMixerConfig->nbConfigs);

    for(unsigned int i = 0; i<pMixerConfig->nbConfigs; i++) {
        ARMSpecificEffectConfig_t *pEffectConfig = (ARMSpecificEffectConfig_t *)specificEffectConfigTableArmAddress;
        OMX_U32 specificConfigStructHandle = (pEffectConfig->specificConfigStructHandle_high << 24) | (pEffectConfig->specificConfigStructHandle_low & 0xFFFFFF);
        if(specificConfigStructHandle) {
            //specificConfigStructHandle can be NULL for example for OMX_DSP_IndexConfigAudioEffectRemovePosition or AFM_DSP_IndexConfigNone (no specific DSP structure allocated)
            ENS::freeMpcMemory(specificConfigStructHandle);
        }
        specificEffectConfigTableArmAddress += sizeof(ARMSpecificEffectConfig_t);
    }

    ENS::freeMpcMemory(specificEffectConfigTableHandle);
    ENS::freeMpcMemory(effectsConfigStructHandle);
}

OMX_ERRORTYPE MixerNmfMpc::retrieveConfig(
        OMX_INDEXTYPE nConfigIndex,
        OMX_PTR pComponentConfigStructure)
{
    switch (nConfigIndex) {
        case OMX_IndexConfigAudioProcessedDataAmount:
        {
            CHECK_STRUCT_SIZE_AND_VERSION(pComponentConfigStructure,OMX_SYMBIAN_AUDIO_CONFIG_PROCESSEDDATAAMOUNTTYPE);
            OMX_SYMBIAN_AUDIO_CONFIG_PROCESSEDDATAAMOUNTTYPE * pConfig = (OMX_SYMBIAN_AUDIO_CONFIG_PROCESSEDDATAAMOUNTTYPE *)pComponentConfigStructure;

            return retrieveProcessedDataAmount(pConfig);
        }

        case OMX_IndexConfigAudioVolumeRamp:
        {
            CHECK_STRUCT_SIZE_AND_VERSION(pComponentConfigStructure,OMX_SYMBIAN_AUDIO_CONFIG_VOLUMERAMPTYPE);
            OMX_SYMBIAN_AUDIO_CONFIG_VOLUMERAMPTYPE *pRampVolume =
            static_cast<OMX_SYMBIAN_AUDIO_CONFIG_VOLUMERAMPTYPE *>(pComponentConfigStructure);

            return retrieveVolumeRampConfig(pRampVolume);

        }
        default:
            return AFMNmfMpc_ProcessingComp::retrieveConfig(
                       nConfigIndex, pComponentConfigStructure);
    }

}

OMX_ERRORTYPE MixerNmfMpc::applyConfig(
        OMX_INDEXTYPE nConfigIndex,
        OMX_PTR pComponentConfigStructure)
{
    switch (nConfigIndex) {

        case OMX_IndexConfigAudioPortpause:
            {
                OMX_SYMBIAN_AUDIO_CONFIG_PORTPAUSETYPE * pConfig =
                    (OMX_SYMBIAN_AUDIO_CONFIG_PORTPAUSETYPE *) pComponentConfigStructure;

                //Don't apply configuration if output port or corresponding input port are disabled.
                //Configuration will be applied later at enablePort.
                if(!mENSComponent.getPort(0)->isEnabled()) return OMX_ErrorNone;
                if(!mENSComponent.getPort(pConfig->nPortIndex)->isEnabled()) return OMX_ErrorNone;

                mIconfigure.setPausedInputPort(pConfig->nPortIndex, pConfig->bIsPaused);

                return OMX_ErrorNone;
            }

        case OMX_IndexConfigAudioVolume:
        case OMX_IndexConfigAudioMute:
        case OMX_IndexConfigAudioBalance:
        case OMX_IndexConfigAudioVolumeRamp:
            {
                MIXER_CONFIG_PORTBASETYPE * pConfig = (MIXER_CONFIG_PORTBASETYPE *) pComponentConfigStructure;
                OMX_U32 effectsConfigStructDspAddress = 0, specificEffectConfigTableArmAddress = 0;
                OMX_ERRORTYPE err;

                //Don't apply configuration if output port or corresponding input port is disabled.
                //Configuration will be applied later at enablePort.
                if(!mENSComponent.getPort(0)->isEnabled()) return OMX_ErrorNone;
                if(!mENSComponent.getPort(pConfig->nPortIndex)->isEnabled()) return OMX_ErrorNone;

                err = allocAndFillDSPEffectsConfigStruct(1, &effectsConfigStructDspAddress, &specificEffectConfigTableArmAddress);
                if(err != OMX_ErrorNone) return err;

                MixerInputPort * inport =  static_cast<MixerInputPort *>(mENSComponent.getPort(pConfig->nPortIndex));

                ConfigInfo_t sConfigInfo;
                sConfigInfo.nConfigIndex = nConfigIndex;
                err = allocAndFillDspConfigStruct(inport->getPortIndex(), &sConfigInfo, (ARMSpecificEffectConfig_t *)specificEffectConfigTableArmAddress);
                if(err != OMX_ErrorNone) return err;

                mIconfigure.configureEffects(effectsConfigStructDspAddress);

                return OMX_ErrorNone;
            }

        case OMX_IndexConfigAudioProcessedDataAmount:
            {
                OMX_SYMBIAN_AUDIO_CONFIG_PROCESSEDDATAAMOUNTTYPE * pConfig = (OMX_SYMBIAN_AUDIO_CONFIG_PROCESSEDDATAAMOUNTTYPE *)pComponentConfigStructure;
                OMX_S32 PortIndex = pConfig->nPortIndex;

                if ((PortIndex >= OUTPUT_PORT_IDX) && (PortIndex <= MIXER_INPUT_PORT_COUNT))
                {
                    //Don't apply configuration if output port or corresponding input port is disabled.
                    if(!mENSComponent.getPort(0)->isEnabled()) return OMX_ErrorNone;
                    if(!mENSComponent.getPort(pConfig->nPortIndex)->isEnabled()) return OMX_ErrorNone;

                    mIconfigure.setSamplesPlayedCounter(pConfig->nPortIndex, 0/*pConfig->nProcessedDataAmount*/);

                    // XXX make sure the value is written immediately
                    MixerSamplesPlayed_t * SPPtr = (MixerSamplesPlayed_t *)getSamplesPlayedPtr();
                    SPPtr->port[PortIndex].samplesPlayedLH=0;
                    SPPtr->port[PortIndex].samplesPlayedLL=0;

                    return OMX_ErrorNone;
                }
                else
                {
                    return OMX_ErrorBadPortIndex;
                }
            }

        case OMX_IndexConfigAudioEffectAddPosition:
        case OMX_IndexConfigAudioEffectRemovePosition:
        case OMX_IndexConfigAudioEffectConfigure:
            {
                //Config has been stored in port->mConfigToCommitStructTable and will be sent to MPC only at with OMX_IndexConfigAudioEffectCommit index
                return OMX_ErrorNone;
            }

        case OMX_IndexConfigAudioEffectCommit:
            {
                OMX_AUDIO_CONFIG_EFFECTCOMMITTYPE * pConfig = (OMX_AUDIO_CONFIG_EFFECTCOMMITTYPE *)pComponentConfigStructure;

                //Don't configure DSP part if output port is disabed : will be configured at enablePort
                if(!mENSComponent.getPort(0)->isEnabled())   return OMX_ErrorNone;

                if(pConfig->nPortIndex == OMX_ALL) {
                    for(OMX_U32 i = 0; i < MIXER_INPUT_PORT_COUNT+1; i++) {
                        //Don't configure DSP part if requested port is disabled : will be configured at enablePort
                        MixerPort * port =  static_cast<MixerPort *>(mENSComponent.getPort(i));
                        if(!port->isEnabled()) continue;

                        OMX_U32 effectsConfigStructDspAddress = 0;
                        DBC_ASSERT(fillPortEffectsConfigsToBeCommitted(i, &effectsConfigStructDspAddress) == OMX_ErrorNone);
                        if(effectsConfigStructDspAddress != 0) {
                            //Send new configuration to DSP if effectsConfigStructDspAddress is not NULL
                            //effectsConfigStructDspAddress can be NULL is port->getNbConfigsToCommit() is equal to 0.
                            //It can be the case if we get some errors in OMX_SetConfig before commit.
                            mIconfigure.configureEffects(effectsConfigStructDspAddress);
                        }
                    }
                } else {
                    //Don't configure DSP part if requested port is disabled : will be configured at enablePort
                    MixerPort * port =  static_cast<MixerPort *>(mENSComponent.getPort(pConfig->nPortIndex));
                    if(!port->isEnabled()) return OMX_ErrorNone;

                    OMX_U32 effectsConfigStructDspAddress = 0;
                    DBC_ASSERT(fillPortEffectsConfigsToBeCommitted(pConfig->nPortIndex, &effectsConfigStructDspAddress) == OMX_ErrorNone);
                    if(effectsConfigStructDspAddress != 0) {
                        //Send new configuration to DSP if effectsConfigStructDspAddress is not NULL
                        //effectsConfigStructDspAddress can be NULL is port->getNbConfigsToCommit() is equal to 0.
                        //It can be the case if we get some errors in OMX_SetConfig before commit.
                        mIconfigure.configureEffects(effectsConfigStructDspAddress);
                    }
                }
                return OMX_ErrorNone;
            }

        default:
            {
                return AFMNmfMpc_ProcessingComp::applyConfig(nConfigIndex, pComponentConfigStructure);
            }
    }
}

//----------------------------------------------------------------------------------------
//  Samples Played methods
//----------------------------------------------------------------------------------------
OMX_ERRORTYPE MixerNmfMpc::initSamplesPlayed(void) {
    t_cm_error alloc_error;
    t_cm_system_address SYSmem;

    mDspAddr = 0;

    alloc_error = ENS::allocMpcMemory(mENSComponent.getNMFDomainHandle(),
            CM_MM_MPC_SDRAM24, sizeof(MixerSamplesPlayed_t)/4, CM_MM_ALIGN_2WORDS, &mSPHandle);
    if (alloc_error != CM_OK) {
        return OMX_ErrorInsufficientResources;
    }

    alloc_error = CM_GetMpcMemorySystemAddress(mSPHandle, &SYSmem);
    if (alloc_error != CM_OK) {
        return OMX_ErrorUndefined;
    }
    setSamplesPlayedPtr((void *)SYSmem.logical);

    alloc_error = CM_GetMpcMemoryMpcAddress(mSPHandle, &mDspAddr);
    if (alloc_error != CM_OK) {
        return OMX_ErrorUndefined;
    }

    unsigned int i;
    int * Ptr = (int *)SYSmem.logical;
    for (i = 0; i < sizeof(MixerSamplesPlayed_t)/4; i++)
    {
        *Ptr++ = 0;
    }

    return OMX_ErrorNone;
}

OMX_ERRORTYPE MixerNmfMpc::retrieveProcessedDataAmount(OMX_SYMBIAN_AUDIO_CONFIG_PROCESSEDDATAAMOUNTTYPE *pConfig) {
    MixerSamplesPlayed_t * SPPtr = (MixerSamplesPlayed_t *)getSamplesPlayedPtr();
    OMX_TICKS p2;
    OMX_U16 loopcount=2;

    if(!mENSComponent.getPort(pConfig->nPortIndex)->isEnabled()) {
        pConfig->nProcessedDataAmount   = 0;
        return OMX_ErrorNone;
    }

    pConfig->nProcessedDataAmount = (((OMX_S64)SPPtr->port[pConfig->nPortIndex].samplesPlayedLH << 24) |
            (OMX_S64)(SPPtr->port[pConfig->nPortIndex].samplesPlayedLL & 0xffffff));
    p2 = (((OMX_S64)SPPtr->port[pConfig->nPortIndex].samplesPlayedLH << 24) |
            (OMX_S64)(SPPtr->port[pConfig->nPortIndex].samplesPlayedLL & 0xffffff));

    while (pConfig->nProcessedDataAmount != p2 && loopcount)
    {
        pConfig->nProcessedDataAmount = p2;
        p2 = (((OMX_S64)SPPtr->port[pConfig->nPortIndex].samplesPlayedLH << 24)|(OMX_S64)(SPPtr->port[pConfig->nPortIndex].samplesPlayedLL & 0xffffff));
        loopcount--;
    }

    OstTraceFiltInst3(TRACE_DEBUG,"Mixer samples played value for port %d : high part %d low part %d)",pConfig->nPortIndex, (pConfig->nProcessedDataAmount >> 32), (pConfig->nProcessedDataAmount & 0xFFFFFFFF));

    if (pConfig->nProcessedDataAmount == p2){
        return OMX_ErrorNone;
    }
    else{
        OstTraceFiltInst3(TRACE_DEBUG,"Mixer checked value for port %d : high part %d low part %d)",pConfig->nPortIndex, (p2 >> 32), (p2 & 0xFFFFFFFF));
        return OMX_ErrorNotReady;
    }
}

//----------------------------------------------------------------------------------------
//  Specific Volctrl methods
//----------------------------------------------------------------------------------------

OMX_ERRORTYPE MixerNmfMpc::initVolumeRampInfoStructure(OMX_U32 portIdx) {
    t_cm_error cmErr;
    t_cm_system_address systemAddress;
    OMX_U32 dspAddress = 0;
    ARMVolCtrlVolumeRampInfo_t *pVolumeRampInfo = NULL;

    cmErr = ENS::allocMpcMemory(mENSComponent.getNMFDomainHandle(),
            CM_MM_MPC_SDRAM24, sizeof(ARMVolCtrlVolumeRampInfo_t)/4, CM_MM_ALIGN_2WORDS, &mVolumeRampInfoHandle[portIdx-1]);
    if (cmErr != CM_OK) {
        return OMX_ErrorInsufficientResources;
    }

    cmErr = CM_GetMpcMemorySystemAddress(mVolumeRampInfoHandle[portIdx-1], &systemAddress);
    if(cmErr != CM_OK)
    {
        ENS::freeMpcMemory(mVolumeRampInfoHandle[portIdx-1]);
        return OMX_ErrorInsufficientResources;
    }

    cmErr = CM_GetMpcMemoryMpcAddress(mVolumeRampInfoHandle[portIdx-1], &dspAddress);
    if(cmErr != CM_OK)
    {
        ENS::freeMpcMemory(mVolumeRampInfoHandle[portIdx-1]);
        return OMX_ErrorInsufficientResources;
    }

    mVolumeRampInfoPtr[portIdx-1] = (ARMVolCtrlVolumeRampInfo_t *) systemAddress.logical;

    //Initialize fields
    mVolumeRampInfoPtr[portIdx-1]->gGLL       = 0;
    mVolumeRampInfoPtr[portIdx-1]->gGRR       = 0;
    mVolumeRampInfoPtr[portIdx-1]->gTimeLeft  = 0;
    mVolumeRampInfoPtr[portIdx-1]->gTimeRight = 0;

    mIconfigure.setEffectGetInfoPtr(portIdx, INPUT_VOLCTRL_POSITION, (void *)dspAddress);

    return OMX_ErrorNone;
}

void inline MixerNmfMpc::freeVolumeRampInfoStructure(OMX_U32 portIdx) {
    if (mVolumeRampInfoHandle[portIdx-1]) {
        ENS::freeMpcMemory(mVolumeRampInfoHandle[portIdx-1]);
        mVolumeRampInfoHandle[portIdx-1] = 0;
        mVolumeRampInfoPtr[portIdx-1] = NULL;
    }
}

OMX_S32 MixerNmfMpc::convertBalanceIfNeeded(OMX_S32 balance)
{
    if(balance==-100 || balance==0 || balance==100 )
    {
        return balance; //No need of logarithmic conversion
    }
    else
    {
        double log_value=0;
        t_sint16 miliBell;
        t_sint16 invert_balance;

        if(balance<0) {
            invert_balance = 100+balance;
            log_value=log10((double)(invert_balance)/(double)(100));
            miliBell = (t_sint16) (2000*log_value);
        } else {
            invert_balance = 100-balance;
            log_value=log10((double)(invert_balance)/(double)(100));
            miliBell = (t_sint16) (-2000*log_value);
        }

        return miliBell;
    }
}

OMX_ERRORTYPE MixerNmfMpc::retrieveVolumeRampConfig(OMX_SYMBIAN_AUDIO_CONFIG_VOLUMERAMPTYPE *pVolumeRamp) {
    Mixer *proxy = (Mixer*)&mENSComponent;
    OMX_U32 portIdx = pVolumeRamp->nPortIndex;
    MixerInputPort * inport =  static_cast<MixerInputPort *>(mENSComponent.getPort(portIdx));
    OMX_SYMBIAN_AUDIO_CONFIG_VOLUMERAMPTYPE *pCurrentValue = inport->getConfigVolumeRamp();

    if (portIdx == OUTPUT_PORT_IDX) return OMX_ErrorBadPortIndex;

    *pVolumeRamp = *pCurrentValue;
    pVolumeRamp->bLinear = OMX_FALSE;  //non linear mode only

    if((!mENSComponent.getPort(portIdx)->isEnabled()) ||
            (!mENSComponent.getPort(OUTPUT_PORT_IDX)->isEnabled())) {
        //Don't read value if port is disabled or if output port is disabled
        return OMX_ErrorNone;
    }

    OMX_U32 gainread, gainreadbis;
    OMX_U32 currentTime, currentTimebis;
    OMX_U16 loopcount=2;
    ARMVolCtrlVolumeRampInfo_t *pVolumeRampInfo = (ARMVolCtrlVolumeRampInfo_t *) mVolumeRampInfoPtr[portIdx-1];

    if((pVolumeRamp->nChannel == OMX_ALL)
            || (pVolumeRamp->nChannel == OMX_AUDIO_ChannelCF)
            || (pVolumeRamp->nChannel == OMX_AUDIO_ChannelLF)) {
        gainread = pVolumeRampInfo->gGLL;
        gainreadbis = pVolumeRampInfo->gGLL;
        currentTime = pVolumeRampInfo->gTimeLeft;
        currentTimebis = pVolumeRampInfo->gTimeLeft;
        while (gainread != gainreadbis && currentTime!=currentTimebis && loopcount)
        {
            //Be sure that DSP value is valid
            gainread = gainreadbis;
            gainreadbis = pVolumeRampInfo->gGLL;
            currentTime = currentTimebis;
            currentTimebis = pVolumeRampInfo->gTimeLeft;
            loopcount--;
        }
    }
    else if (pVolumeRamp->nChannel == OMX_AUDIO_ChannelRF) {
        gainread = pVolumeRampInfo->gGRR;
        gainreadbis = pVolumeRampInfo->gGRR;
        currentTime = pVolumeRampInfo->gTimeRight;
        currentTimebis = pVolumeRampInfo->gTimeRight;
        while (gainread != gainreadbis && currentTime!=currentTimebis && loopcount)
        {
            //Be sure that DSP value is valid
            gainread = gainreadbis;
            gainreadbis = pVolumeRampInfo->gGRR;
            currentTime = currentTimebis;
            currentTimebis = pVolumeRampInfo->gTimeRight;
            loopcount--;
        }
    } else {
        return OMX_ErrorBadParameter;
    }

    if (gainread != gainreadbis || currentTime!=currentTimebis){
        OstTraceFiltInst1(TRACE_WARNING,"Mixer not able to retrieve VolumeRamp Config for port %d",portIdx);
        return OMX_ErrorNotReady;
    }

    OMX_S32 volback = (((OMX_S32)gainread * 100) >> 8) - 12800;
    // check vs absolute ranges (needed in order to cope with rounded values at min/max levels)
    if (volback < pVolumeRamp->sCurrentVolume.nMin) {
        volback = pVolumeRamp->sCurrentVolume.nMin;
    } else if (volback > pVolumeRamp->sCurrentVolume.nMax) {
        volback = pVolumeRamp->sCurrentVolume.nMax;
    }

    pVolumeRamp->sCurrentVolume.nValue  = volback;
    pVolumeRamp->nRampCurrentTime       = (pVolumeRamp->nRampDuration - currentTime*10);

    return OMX_ErrorNone;
}

PcmProbe_t * MixerNmfMpc::getProbeConfig(OMX_U32 portIdx, OMX_U32 nPositionIdx) const {
    if(nPositionIdx == 0) {
        return mPcmProbe.getProbeConfig(portIdx);
    } else {
        MixerPort * port =  static_cast<MixerPort *>(mENSComponent.getPort(portIdx));
        AudioEffect *pEffectNode = port->getEffectNode(nPositionIdx);
        DBC_ASSERT(pEffectNode != NULL);
        AudioEffectMpc &effectNodeMpc = static_cast<AudioEffectMpc &>(pEffectNode->getCoreSpecificPart());
        return effectNodeMpc.getDspProbeConfig();
    }
}

