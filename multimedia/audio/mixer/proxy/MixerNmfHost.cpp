/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   MixerNmfHost.cpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "MixerNmfHost.h"
#include "HostBindings.h"
#include "MixerInputPort.h"
#include "audio_chipset_api_index.h"

#include "AFM_Utils.h"

#include "OstTraceDefinitions_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "mixer_proxy_MixerNmfHostTraces.h"
#endif
#include "OpenSystemTrace_ste.h"


OMX_ERRORTYPE MixerNmfHost::construct(void)
{
    OMX_ERRORTYPE error;

    error = AFMNmfHost_ProcessingComp::construct();
    if (error != OMX_ErrorNone) return error;
    
    return OMX_ErrorNone;
}

OMX_ERRORTYPE MixerNmfHost::destroy(void)
{
    OMX_ERRORTYPE error;

    error = AFMNmfHost_ProcessingComp::destroy();
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}
OMX_ERRORTYPE MixerNmfHost::retrieveConfig(
        OMX_INDEXTYPE nConfigIndex,  
        OMX_PTR pComponentConfigStructure)
{
    switch (nConfigIndex) {
        case OMX_IndexConfigAudioVolumeRamp:
        {
#if 0
            CHECK_STRUCT_SIZE_AND_VERSION(pComponentConfigStructure,OMX_SYMBIAN_AUDIO_CONFIG_VOLUMERAMPTYPE);
            
            OMX_SYMBIAN_AUDIO_CONFIG_VOLUMERAMPTYPE *pRampVolume =
            static_cast<OMX_SYMBIAN_AUDIO_CONFIG_VOLUMERAMPTYPE *>(pComponentConfigStructure);

            Mixer *proxy = (Mixer*)&mENSComponent;
            
            MixerInputPort * inport =  static_cast<MixerInputPort *>(proxy->getPort(pRampVolume->nPortIndex));

            OMX_SYMBIAN_AUDIO_CONFIG_VOLUMERAMPTYPE *actualValue = inport->getConfigVolumeRamp();

            if(inport->isEnabled())
            {
                *pRampVolume = *actualValue;
            }
            else{
                return OMX_ErrorNotReady;
            }
#endif
            return OMX_ErrorNone;
            
        }
        default:
            return AFMNmfHost_ProcessingComp::retrieveConfig(
                       nConfigIndex, pComponentConfigStructure);
    }

}    

OMX_ERRORTYPE MixerNmfHost::applyConfig(
    OMX_INDEXTYPE nConfigIndex,  
    OMX_PTR pComponentConfigStructure)
{
    switch (nConfigIndex) {
        
        case OMX_IndexConfigAudioPortpause:
        {
            OMX_SYMBIAN_AUDIO_CONFIG_PORTPAUSETYPE * pConfig = 
            (OMX_SYMBIAN_AUDIO_CONFIG_PORTPAUSETYPE*) pComponentConfigStructure;
            OstTraceFiltInst2(TRACE_DEBUG,"MixerNmfHost::applyConfig nPortIndex=%d bIsPaused=%d",pConfig->nPortIndex, pConfig->bIsPaused);            
            mIconfigure.setPausedInputPort(pConfig->nPortIndex, pConfig->bIsPaused);

            return OMX_ErrorNone;
        }

        case OMX_IndexConfigAudioVolume:
        {
            OMX_AUDIO_CONFIG_VOLUMETYPE * pVolume = 
            static_cast<OMX_AUDIO_CONFIG_VOLUMETYPE *>(pComponentConfigStructure);
            if (pVolume->bLinear) return OMX_ErrorBadParameter;
            mIconfigure.setVolumeInputPort(pVolume->nPortIndex, (t_sint16) pVolume->sVolume.nValue);
            return OMX_ErrorNone;
        }
    
        case OMX_IndexConfigAudioMute:
        {
            OMX_AUDIO_CONFIG_MUTETYPE * pMute = 
            static_cast<OMX_AUDIO_CONFIG_MUTETYPE *>(pComponentConfigStructure);
            mIconfigure.setMuteInputPort(pMute->nPortIndex, (BOOL) pMute->bMute);
            return OMX_ErrorNone;
        }

        case OMX_IndexConfigAudioBalance:
        {
            OMX_AUDIO_CONFIG_BALANCETYPE * pBalance = 
            static_cast<OMX_AUDIO_CONFIG_BALANCETYPE *>(pComponentConfigStructure);
            mIconfigure.setBalanceInputPort(pBalance->nPortIndex, (t_sint16) pBalance->nBalance);
            return OMX_ErrorNone;
        }

        case OMX_IndexConfigAudioVolumeRamp:
        {
            OMX_SYMBIAN_AUDIO_CONFIG_VOLUMERAMPTYPE *pRampVolume =
            static_cast<OMX_SYMBIAN_AUDIO_CONFIG_VOLUMERAMPTYPE *>(pComponentConfigStructure);
            mIconfigure.setVolumeRampInputPort(
                    pRampVolume->nPortIndex, 
                    pRampVolume->sStartVolume.nValue,
                    pRampVolume->sEndVolume.nValue,
                    pRampVolume->nChannel,
                    pRampVolume->nRampDuration / 10,
                    pRampVolume->bRampTerminate);
                    
            return OMX_ErrorNone;
        }

        default:
            return AFMNmfHost_ProcessingComp::applyConfig(
                nConfigIndex, pComponentConfigStructure);
	}
}
static const char* suffixItf[]=
{ "", "[0]", "[1]", "[2]", "[3]", "[4]", "[5]", "[6]", "[7]" };
const char * MixerNmfHost::getNmfSuffix(OMX_U32 portidx) const{
    return suffixItf[portidx];
}

OMX_ERRORTYPE MixerNmfHost::instantiateInputPort(int idx) {
    t_nmf_error   nmf_error;
    ENS_String<20> volctrlitf("volctrl");
    volctrlitf.concat(suffixItf[idx+INPUT_PORT_IDX]);
    ENS_String<20> eventhandleritf("eventhandler");
    eventhandleritf.concat(suffixItf[idx+INPUT_PORT_IDX]);
    
    mNmfVolctrl[idx] = volctrl_nmfil_host_effectWrappedCreate();
    if (mNmfVolctrl[idx] == NULL) { return OMX_ErrorInsufficientResources; };
    (static_cast<volctrl_nmfil_host_effectWrapped*>(mNmfVolctrl[idx]))->priority=getPortPriorityLevel(OUTPUT_PORT_IDX);
    if (mNmfVolctrl[idx]->construct() != NMF_OK) return OMX_ErrorInsufficientResources;
	
    nmf_error = mNmfVolctrl[idx]->bindFromUser("configure", 8, &mIconfigureVolCtrl);
    if (nmf_error != NMF_OK) return OMX_ErrorInsufficientResources;

    nmf_error = mNmfMain->bindComponent(volctrlitf, mNmfVolctrl[idx], "volctrl");
    if (nmf_error != NMF_OK) return OMX_ErrorInsufficientResources;

    return OMX_ErrorNone;
}

OMX_ERRORTYPE MixerNmfHost::deInstantiateInputPort(int idx) {
    t_nmf_error   nmf_error;
    ENS_String<20> volctrlitf("volctrl");
    volctrlitf.concat(suffixItf[idx+INPUT_PORT_IDX]);
    ENS_String<20> eventhandleritf("eventhandler");
    eventhandleritf.concat(suffixItf[idx+INPUT_PORT_IDX]);

    nmf_error = mNmfMain->unbindComponent(volctrlitf, mNmfVolctrl[idx], "volctrl");
    if (nmf_error != NMF_OK) return OMX_ErrorInsufficientResources;

    nmf_error = mNmfVolctrl[idx]->unbindFromUser("configure");
    if (nmf_error != NMF_OK) return OMX_ErrorInsufficientResources;

    if (mNmfVolctrl[idx]->destroy() != NMF_OK) return OMX_ErrorInsufficientResources;
    
    volctrl_nmfil_host_effectWrappedDestroy((volctrl_nmfil_host_effectWrapped*&)mNmfVolctrl[idx]);

    
    return OMX_ErrorNone;
}

OMX_ERRORTYPE MixerNmfHost::instantiateMain(void) {
    OMX_ERRORTYPE error;
    t_nmf_error nmf_error;

    mNmfMain = mixer_nmfil_host_wrapperWrappedCreate();

    if (mNmfMain == NULL) return OMX_ErrorInsufficientResources;
    (static_cast<mixer_nmfil_host_wrapperWrapped*>(mNmfMain))->priority=getPortPriorityLevel(OUTPUT_PORT_IDX);

    nmf_error = mNmfMain->construct();
    if (nmf_error != NMF_OK) return OMX_ErrorInsufficientResources;

    nmf_error = mNmfMain->bindAsynchronous("me", mENSComponent.getPortCount(), mNmfMain, "postevent");
    if (nmf_error != NMF_OK) return OMX_ErrorInsufficientResources;

    nmf_error = mNmfMain->bindFromUser("configure", mENSComponent.getPortCount()*10, &mIconfigure);
    if (nmf_error != NMF_OK) return OMX_ErrorInsufficientResources;


    for (unsigned int idx = INPUT_PORT_IDX; idx < mENSComponent.getPortCount(); idx++) {
        ENS_Port *inport = static_cast<ENS_Port *>(mENSComponent.getPort(idx));

        if (inport->isEnabled()) {
            error = instantiateInputPort(idx-INPUT_PORT_IDX);
            if (error != OMX_ErrorNone) return error;
        }
    }

    return OMX_ErrorNone;
}

OMX_ERRORTYPE MixerNmfHost::deInstantiateMain(void) {
    OMX_ERRORTYPE error;
    t_nmf_error nmf_error;

    for (unsigned int idx = INPUT_PORT_IDX; idx < mENSComponent.getPortCount(); idx++) {
        ENS_Port *inport = static_cast<ENS_Port *>(mENSComponent.getPort(idx));

        if (inport->isEnabled()) {
            error = deInstantiateInputPort(idx-INPUT_PORT_IDX);
            if (error != OMX_ErrorNone) return error;
        }
    }

    nmf_error = mNmfMain->unbindAsynchronous("me", mNmfMain, "postevent");
    if (nmf_error != NMF_OK) return OMX_ErrorInsufficientResources;

    nmf_error = mNmfMain->unbindFromUser("configure");
    if (nmf_error != NMF_OK) return OMX_ErrorInsufficientResources;

    nmf_error = mNmfMain->destroy();
    if (nmf_error != NMF_OK) return OMX_ErrorInsufficientResources;

    mixer_nmfil_host_wrapperWrappedDestroy((mixer_nmfil_host_wrapperWrapped*&)mNmfMain);

    return OMX_ErrorNone;
}

OMX_ERRORTYPE MixerNmfHost::startMain(void) {
    mNmfMain->start();

    return OMX_ErrorNone;
}


OMX_ERRORTYPE MixerNmfHost::stopMain(void) {
  
    mNmfMain->stop();
    return OMX_ErrorNone;
}

void
MixerNmfHost::fillOutputPortParam(HostOutputPortParam_t  &outputportparam) {
    Mixer *proxy = (Mixer*)&mENSComponent;
    OMX_AUDIO_PARAM_PCMMODETYPE &pcmSettings = (static_cast<AFM_PcmPort *>(proxy->getPort(OUTPUT_PORT_IDX)))->getPcmSettings();
    const AFM_PARAM_PCMLAYOUTTYPE &pcmLayout= (static_cast<AFM_PcmPort *>(proxy->getPort(OUTPUT_PORT_IDX)))->getPcmLayout();
    
    
    unsigned int i;

    outputportparam.nSamplingRate = AFM::sampleFreq(pcmSettings.nSamplingRate);
    outputportparam.nChannels     = pcmLayout.nChannels;
    outputportparam.nBlockSize    = pcmLayout.nBlockSize;
    outputportparam.nBitsPerSample = pcmSettings.nBitPerSample;
    
    for(i=0;i<MAXCHANNELS_SUPPORT;i++){
        outputportparam.channel_type[i] = CHANNEL_NONE;
    }
    for(i=0;i<pcmSettings.nChannels;i++){    
        switch (pcmSettings.eChannelMapping[i]) {
                case OMX_AUDIO_ChannelLF:
                    outputportparam.channel_type[i] = CHANNEL_LF;
                    break;
                case OMX_AUDIO_ChannelRF:
                    outputportparam.channel_type[i] = CHANNEL_RF;
                    break;
                case OMX_AUDIO_ChannelCF:
                    outputportparam.channel_type[i] = CHANNEL_CF;
                    break;
                case OMX_AUDIO_ChannelLS:
                    outputportparam.channel_type[i] = CHANNEL_LS;
                    break;
                case OMX_AUDIO_ChannelRS:
                    outputportparam.channel_type[i] = CHANNEL_RS;
                    break;
                case OMX_AUDIO_ChannelLFE:
                    outputportparam.channel_type[i] = CHANNEL_LFE;
                    break;
                case OMX_AUDIO_ChannelCS:
                    outputportparam.channel_type[i] = CHANNEL_CS;
                    break;
                case OMX_AUDIO_ChannelLR:
                    outputportparam.channel_type[i] = CHANNEL_LR;
                    break;
                case OMX_AUDIO_ChannelRR:
                    outputportparam.channel_type[i] = CHANNEL_RR;
                    break;
                default:
                    outputportparam.channel_type[i] = CHANNEL_NONE;
                    break;
            }
    }
    
}

void
MixerNmfHost::fillInputPortParam(int idx, HostInputPortParam_t  &inputportparam) {
    Mixer *proxy = (Mixer*)&mENSComponent;
    OMX_AUDIO_PARAM_PCMMODETYPE &pcmSettings = (static_cast<AFM_PcmPort *>(proxy->getPort(idx+INPUT_PORT_IDX)))->getPcmSettings();
    const AFM_PARAM_PCMLAYOUTTYPE &pcmLayout= (static_cast<AFM_PcmPort *>(proxy->getPort(idx+INPUT_PORT_IDX)))->getPcmLayout();
    
    MixerInputPort * port =  static_cast<MixerInputPort *>(proxy->getPort(idx+INPUT_PORT_IDX));
    unsigned int i;

    

    // Parameters
    inputportparam.nSamplingRate        = AFM::sampleFreq(pcmSettings.nSamplingRate);
    inputportparam.nChannels            = pcmSettings.nChannels;
    inputportparam.nBlockSize           = pcmLayout.nBlockSize;
    inputportparam.nSrcMode             = port->getSrcMode();

    // Configs
    inputportparam.bPaused              = port->isPaused();
    inputportparam.bMute                = port->getConfigMute()->bMute;
    inputportparam.nBalance             = port->getConfigBalance()->nBalance;
    inputportparam.nVolume              = port->getConfigVolume()->sVolume.nValue;

    if (port->getConfigVolumeRamp()->nRampDuration) {
        inputportparam.nVolume       = port->getConfigVolumeRamp()->sStartVolume.nValue;
    }

    inputportparam.nRampEndVolume       = port->getConfigVolumeRamp()->sEndVolume.nValue;
    inputportparam.nRampChannels        = port->getConfigVolumeRamp()->nChannel;
    inputportparam.nRampDuration        = port->getConfigVolumeRamp()->nRampDuration / 10; 
    inputportparam.bRampTerminate       = port->getConfigVolumeRamp()->bRampTerminate;
    for(i=0;i<MAXCHANNELS_SUPPORT;i++){
        inputportparam.channel_type[i] = CHANNEL_NONE;
    }
    for(i=0;i<pcmSettings.nChannels;i++){    
        switch (pcmSettings.eChannelMapping[i]) {
               case OMX_AUDIO_ChannelLF:
                    inputportparam.channel_type[i] = CHANNEL_LF;
                    break;
                case OMX_AUDIO_ChannelRF:
                    inputportparam.channel_type[i] = CHANNEL_RF;
                    break;
                case OMX_AUDIO_ChannelCF:
                    inputportparam.channel_type[i] = CHANNEL_CF;
                    break;
                case OMX_AUDIO_ChannelLS:
                    inputportparam.channel_type[i] = CHANNEL_LS;
                    break;
                case OMX_AUDIO_ChannelRS:
                    inputportparam.channel_type[i] = CHANNEL_RS;
                    break;
                case OMX_AUDIO_ChannelLFE:
                    inputportparam.channel_type[i] = CHANNEL_LFE;
                    break;
                case OMX_AUDIO_ChannelCS:
                    inputportparam.channel_type[i] = CHANNEL_CS;
                    break;
                case OMX_AUDIO_ChannelLR:
                    inputportparam.channel_type[i] = CHANNEL_LR;
                    break;
                case OMX_AUDIO_ChannelRR:
                    inputportparam.channel_type[i] = CHANNEL_RR;
                    break;
                default:
                    inputportparam.channel_type[i] = CHANNEL_NONE;
                    break;
            }
    }
}

OMX_ERRORTYPE MixerNmfHost::configureMain(void) {
    Mixer *proxy = (Mixer*)&mENSComponent;
    HostMixerParam_t mixerParam;
    memset(&mixerParam, 0, sizeof(HostMixerParam_t));
    fillOutputPortParam(mixerParam.outputPortParam);

    for (unsigned int i = INPUT_PORT_IDX; i < proxy->getPortCount(); i++) {
        fillInputPortParam(i-INPUT_PORT_IDX, mixerParam.inputPortParam[i-INPUT_PORT_IDX]);
    }

    mixerParam.traceAddr  = (t_uint32)getTraceInfoPtr();

    mIconfigure.setParameter(mixerParam);
     
    return OMX_ErrorNone;
}

OMX_ERRORTYPE MixerNmfHost::doSpecificSendCommand(
    OMX_COMMANDTYPE eCmd, 
    OMX_U32 nData, 
    OMX_BOOL &bDeferredCmd)
{
    if (eCmd == OMX_CommandPortEnable) {
        if (nData == OUTPUT_PORT_IDX) {
            configureMain();
        } else {
            HostInputPortParam_t inputportparam;
            instantiateInputPort(nData-INPUT_PORT_IDX);
            fillInputPortParam(nData-INPUT_PORT_IDX, inputportparam);
            mIconfigure.setInputPortParameter(nData, inputportparam);
        }
    }
    bDeferredCmd = OMX_FALSE;

    return AFMNmfHost_ProcessingComp::doSpecificSendCommand(eCmd, nData, bDeferredCmd);
}

OMX_ERRORTYPE MixerNmfHost::disablePortNotification(OMX_U32 nPortIndex)
{
    OMX_ERRORTYPE error = OMX_ErrorNone;
    if (nPortIndex != OUTPUT_PORT_IDX) {
        error = deInstantiateInputPort(nPortIndex-INPUT_PORT_IDX);
    }

    return error;
}

OMX_ERRORTYPE MixerNmfHost::doSpecificEventHandler_cb(OMX_EVENTTYPE event, OMX_U32 nData1, OMX_U32 nData2, OMX_BOOL & deferEventHandler)
{
    deferEventHandler = OMX_FALSE;

    return AFMNmfHost_ProcessingComp::doSpecificEventHandler_cb(event,nData1,nData2,deferEventHandler);
}

