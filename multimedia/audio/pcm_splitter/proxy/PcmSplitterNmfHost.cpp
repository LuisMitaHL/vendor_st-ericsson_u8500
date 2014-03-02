/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "cm/inc/cm_macros.h"
#include "PcmSplitterNmfHost.h"
#include "ENS_Nmf.h"
#include "AFM_nmf.h"
#include "AFM_Utils.h"
#include "PcmSplitter_PcmPort.h"
#include "OMX_CoreExt.h"

#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "pcm_splitter_proxy_PcmSplitterNmfHostTraces.h"
#endif

PcmSplitterNmfHost::PcmSplitterNmfHost(ENS_Component &enscomp)
    : AFMNmfHost_ProcessingComp(enscomp)
{
    for (int i = 0; i < NB_OUTPUT; i++) {
        mNmfVolctrl[i] = 0;
    }
}

OMX_ERRORTYPE PcmSplitterNmfHost::construct(void)
{
    OMX_ERRORTYPE error;

    error = AFMNmfHost_ProcessingComp::construct();
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}

OMX_ERRORTYPE PcmSplitterNmfHost::destroy(void)
{
    OMX_ERRORTYPE error;

    error = AFMNmfHost_ProcessingComp::destroy();
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}

OMX_ERRORTYPE PcmSplitterNmfHost::applyConfig(
    OMX_INDEXTYPE nConfigIndex,
    OMX_PTR pComponentConfigStructure)
{
    OstTraceFiltInst0(TRACE_FLOW,"PcmSplitterNmfHost proxy : applyConfig");
    switch (nConfigIndex) {

        case OMX_IndexConfigAudioChannelSwitchBoard:
        {
            OMX_SYMBIAN_AUDIO_CONFIG_CHANNELSWITCHBOARDTYPE * pConfig =
                (OMX_SYMBIAN_AUDIO_CONFIG_CHANNELSWITCHBOARDTYPE *) pComponentConfigStructure;
            t_uint16 channelSwitchBoard[NMF_AUDIO_MAXCHANNELS];

            for (int i = 0; i < NMF_AUDIO_MAXCHANNELS; i++) {
                channelSwitchBoard[i] = pConfig->nOutputChannel[i];

                OstTraceFiltInst3(TRACE_FLOW,"PcmSplitterNmfHost::applyConfig portidx=%d channelSwitchBoard[%d]=%d",pConfig->nPortIndex,i,channelSwitchBoard[i]);
            }

            mIconfigure.setChannelSwitchBoardOutputPort(pConfig->nPortIndex, channelSwitchBoard);
            return OMX_ErrorNone;
        }

        case OMX_IndexConfigAudioPortpause:
        {
            OMX_SYMBIAN_AUDIO_CONFIG_PORTPAUSETYPE * pConfig =
                (OMX_SYMBIAN_AUDIO_CONFIG_PORTPAUSETYPE*) pComponentConfigStructure;
            mIconfigure.setPausedOutputPort(pConfig->nPortIndex, pConfig->bIsPaused);
            return OMX_ErrorNone;
        }

        case OMX_IndexConfigAudioVolume:
        {
            OMX_AUDIO_CONFIG_VOLUMETYPE * pVolume =
                static_cast<OMX_AUDIO_CONFIG_VOLUMETYPE *>(pComponentConfigStructure);
            if (pVolume->nPortIndex == 0) return OMX_ErrorNone; // FIXME
            if (pVolume->bLinear) return OMX_ErrorBadParameter;
            mIconfigure.setVolumeOutputPort(pVolume->nPortIndex, (t_sint16) pVolume->sVolume.nValue);
            return OMX_ErrorNone;
        }

        case OMX_IndexConfigAudioMute:
        {
            OMX_AUDIO_CONFIG_MUTETYPE * pMute =
                static_cast<OMX_AUDIO_CONFIG_MUTETYPE *>(pComponentConfigStructure);
            if (pMute->nPortIndex == 0) return OMX_ErrorNone; // FIXME
            mIconfigure.setMuteOutputPort(pMute->nPortIndex, (BOOL) pMute->bMute);
            return OMX_ErrorNone;
        }

        case OMX_IndexConfigAudioBalance:
        {
            OMX_AUDIO_CONFIG_BALANCETYPE * pBalance =
                static_cast<OMX_AUDIO_CONFIG_BALANCETYPE *>(pComponentConfigStructure);
            if (pBalance->nPortIndex == 0) return OMX_ErrorNone; // FIXME
            mIconfigure.setBalanceOutputPort(pBalance->nPortIndex, (t_sint16) pBalance->nBalance);
            return OMX_ErrorNone;
        }

        case OMX_IndexConfigAudioVolumeRamp:
        {
            OMX_SYMBIAN_AUDIO_CONFIG_VOLUMERAMPTYPE *pRampVolume =
                    static_cast<OMX_SYMBIAN_AUDIO_CONFIG_VOLUMERAMPTYPE *>(pComponentConfigStructure);

            if (pRampVolume->nPortIndex == 0) return OMX_ErrorNone; // FIXME

            mIconfigure.setVolumeRampOutputPort(
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

static const char* suffixItf[]= { "", "[0]", "[1]" };

const char * PcmSplitterNmfHost::getNmfSuffix(OMX_U32 portidx) const{
    return suffixItf[portidx];
}

OMX_ERRORTYPE PcmSplitterNmfHost::instantiateOutputPort(int idx) {
    t_nmf_error nmf_error;
    ENS_String<20> volctrlitf("volctrl");
    volctrlitf.concat(getNmfSuffix(idx+1));

    mNmfVolctrl[idx] = volctrl_nmfil_host_effectWrappedCreate();
    if (mNmfVolctrl[idx] == NULL) { return OMX_ErrorInsufficientResources; };
    (static_cast<volctrl_nmfil_host_effectWrapped*>(mNmfVolctrl[idx]))->priority=getPortPriorityLevel(idx+1);
    if (mNmfVolctrl[idx]->construct() != NMF_OK) return OMX_ErrorInsufficientResources;
	
    nmf_error = mNmfVolctrl[idx]->bindFromUser("configure", 8, &mIconfigureVolCtrl);
    if (nmf_error != NMF_OK) return OMX_ErrorInsufficientResources;

    nmf_error = mNmfMain->bindComponent(volctrlitf, mNmfVolctrl[idx], "volctrl");
    if (nmf_error != NMF_OK) return OMX_ErrorInsufficientResources;

    return OMX_ErrorNone;
}

OMX_ERRORTYPE PcmSplitterNmfHost::deInstantiateOutputPort(int idx) {
    t_nmf_error nmf_error;
    ENS_String<20> volctrlitf("volctrl");
    volctrlitf.concat(suffixItf[idx+1]);

    nmf_error = mNmfMain->unbindComponent(volctrlitf, mNmfVolctrl[idx], "volctrl");
    if (nmf_error != NMF_OK) return OMX_ErrorInsufficientResources;
    
    nmf_error = mNmfVolctrl[idx]->unbindFromUser("configure");
    if (nmf_error != NMF_OK) return OMX_ErrorInsufficientResources;

    if (mNmfVolctrl[idx]->destroy() != NMF_OK) return OMX_ErrorInsufficientResources;
    
    volctrl_nmfil_host_effectWrappedDestroy((volctrl_nmfil_host_effectWrapped*&)mNmfVolctrl[idx]);

    return OMX_ErrorNone;
}

OMX_ERRORTYPE PcmSplitterNmfHost::instantiateMain(void) {
    OMX_ERRORTYPE error;
    t_nmf_error nmf_error;

    mNmfMain = pcm_splitter_nmfil_host_wrapperWrappedCreate();

    if (mNmfMain == NULL) return OMX_ErrorInsufficientResources;
    (static_cast<pcm_splitter_nmfil_host_wrapperWrapped*>(mNmfMain))->priority=getPortPriorityLevel(INPUT_PORT_IDX);

    nmf_error = mNmfMain->construct();
    if (nmf_error != NMF_OK) return OMX_ErrorInsufficientResources;

    nmf_error = mNmfMain->bindAsynchronous("me", mENSComponent.getPortCount(), mNmfMain, "postevent");
    if (nmf_error != NMF_OK) return OMX_ErrorInsufficientResources;

    nmf_error = mNmfMain->bindFromUser("configure", mENSComponent.getPortCount()*10, &mIconfigure);
    if (nmf_error != NMF_OK) return OMX_ErrorInsufficientResources;



    for (unsigned int idx = 1; idx < mENSComponent.getPortCount(); idx++) {
        ENS_Port *outport = static_cast<ENS_Port *>(mENSComponent.getPort(idx));

        if (outport->isEnabled()) {
            error = instantiateOutputPort(idx-1);
            if (error != OMX_ErrorNone) return error;
        }
    }

    return OMX_ErrorNone;
}

OMX_ERRORTYPE PcmSplitterNmfHost::deInstantiateMain(void) {
    OMX_ERRORTYPE error;
    t_nmf_error nmf_error;

    for (unsigned int idx = 1; idx < mENSComponent.getPortCount(); idx++) {
        ENS_Port *outport = static_cast<ENS_Port *>(mENSComponent.getPort(idx));

        if (outport->isEnabled()) {
            error = deInstantiateOutputPort(idx-1);
            if (error != OMX_ErrorNone) return error;
        }
    }

    nmf_error = mNmfMain->unbindAsynchronous("me", mNmfMain, "postevent");
    if (nmf_error != NMF_OK) return OMX_ErrorInsufficientResources;

    nmf_error = mNmfMain->unbindFromUser("configure");
    if (nmf_error != NMF_OK) return OMX_ErrorInsufficientResources;

    nmf_error = mNmfMain->destroy();
    if (nmf_error != NMF_OK) return OMX_ErrorInsufficientResources;

    pcm_splitter_nmfil_host_wrapperWrappedDestroy((pcm_splitter_nmfil_host_wrapperWrapped*&)mNmfMain);

    return OMX_ErrorNone;
}

OMX_ERRORTYPE PcmSplitterNmfHost::startMain(void) {
    mNmfMain->start();

    return OMX_ErrorNone;
}


OMX_ERRORTYPE PcmSplitterNmfHost::stopMain(void) {
    mNmfMain->stop();

    return OMX_ErrorNone;
}

void
PcmSplitterNmfHost::fillInputPortParam(HostInputPortParam_t  &inputportparam) {
    OMX_AUDIO_PARAM_PCMMODETYPE pcmSettings;
    AFM_PARAM_PCMLAYOUTTYPE pcmLayout;
    
    pcmSettings = static_cast<AFM_PcmPort *>(mENSComponent.getPort(0))->getPcmSettings();
    pcmLayout   = static_cast<AFM_PcmPort *>(mENSComponent.getPort(0))->getPcmLayout();

    inputportparam.nSamplingRate = AFM::sampleFreq(pcmSettings.nSamplingRate);
    inputportparam.nChannels     = pcmLayout.nChannels;
    inputportparam.nBlockSize    = pcmLayout.nBlockSize;
    inputportparam.nBitsPerSample= pcmSettings.nBitPerSample;
}

void
PcmSplitterNmfHost::fillOutputPortParam(int idx, HostOutputPortParam_t  &outputportparam) {
    PcmSplitter *proxy = (PcmSplitter*)&mENSComponent;
    OMX_AUDIO_PARAM_PCMMODETYPE pcmSettings;
    OMX_AUDIO_PARAM_PCMMODETYPE pcmSettingsIn;
    AFM_PARAM_PCMLAYOUTTYPE pcmLayout;
    PcmSplitter_PcmPort * port =  static_cast<PcmSplitter_PcmPort *>(proxy->getPort(idx+1));
    PcmSplitter_PcmPort * inport =  static_cast<PcmSplitter_PcmPort *>(proxy->getPort(0));

    pcmSettings = port->getPcmSettings();
    pcmLayout   = port->getPcmLayout();


    pcmSettingsIn = inport->getPcmSettings();

    // Parameters
    outputportparam.nSamplingRate        = AFM::sampleFreq(pcmSettings.nSamplingRate);
    outputportparam.nChannels            = pcmLayout.nChannels;
    outputportparam.nBlockSize           = pcmLayout.nBlockSize;

    // Configs
    OMX_U32 channelSwitchBoard[NMF_AUDIO_MAXCHANNELS];
    port->getChannelSwitchBoard(channelSwitchBoard);
    for (int i = 0; i < NMF_AUDIO_MAXCHANNELS; i++) {
        outputportparam.nChannelSwitchBoard[i] = channelSwitchBoard[i];
        OstTraceFiltInst3(TRACE_FLOW,"PcmSplitterNmfHost::fillOutputPortParam portidx=%d outputportparam.nChannelSwitchBoard[%d]=%d",idx+1,i,channelSwitchBoard[i]);
    }
    if((pcmSettingsIn.nChannels == 1) && (pcmSettings.nChannels == 2))
    {
        OstTraceFiltInst1(TRACE_DEBUG,"fillOutputPortParam portIdx=%d we are forcing upmix ",idx+1);
        // In this case we need to do upmix (1->2) conversion 
        // with channel switchboard
        outputportparam.nChannelSwitchBoard[0] = 0; //Ouput Left = Input
        outputportparam.nChannelSwitchBoard[1] = 0; //Ouput Right = Input
        OstTraceFiltInst1(TRACE_FLOW,"PcmSplitterNmfHost::fillOutputPortParam portidx=%d outputportparam.nChannelSwitchBoard[0]=0 outputportparam.nChannelSwitchBoard[1]=0 ",idx+1);
    }
    
    outputportparam.bPaused              = port->isPaused();
    outputportparam.bMute                = proxy->getConfigMute(idx+1)->bMute;
    outputportparam.nBalance             = proxy->getConfigBalance(idx+1)->nBalance;
    outputportparam.nVolume              = proxy->getConfigVolume(idx+1)->sVolume.nValue;

    if (proxy->getConfigVolumeRamp(idx+1)->nRampDuration) {
        outputportparam.nVolume         = proxy->getConfigVolumeRamp(idx+1)->sStartVolume.nValue;
    }

    outputportparam.nRampEndVolume       = proxy->getConfigVolumeRamp(idx+1)->sEndVolume.nValue;
    outputportparam.nRampChannels        = proxy->getConfigVolumeRamp(idx+1)->nChannel;
    outputportparam.nRampDuration        = proxy->getConfigVolumeRamp(idx+1)->nRampDuration / 10; 
    outputportparam.bRampTerminate       = proxy->getConfigVolumeRamp(idx+1)->bRampTerminate;
}


OMX_ERRORTYPE PcmSplitterNmfHost::configureMain(void) {
    HostSplitterParam_t splitterParam;

    fillInputPortParam(splitterParam.inputPortParam);

    for (unsigned int i = 1; i < mENSComponent.getPortCount(); i++) {
        fillOutputPortParam(i-1, splitterParam.outputPortParam[i-1]);
    }

    splitterParam.traceAddr  = getDspAddr();

    mIconfigure.setParameter(splitterParam);
     
    return OMX_ErrorNone;
}




OMX_ERRORTYPE PcmSplitterNmfHost::doSpecificSendCommand(
    OMX_COMMANDTYPE eCmd,
    OMX_U32 nData,
    OMX_BOOL &bDeferredCmd)
{
    if (eCmd == OMX_CommandPortEnable) {
        if (nData == 0) {
            configureMain();
        } else {
            HostOutputPortParam_t outputportparam;
            instantiateOutputPort(nData-1);
            fillOutputPortParam(nData-1, outputportparam);
            mIconfigure.setOutputPortParameter(nData, outputportparam);
        }
    }

    bDeferredCmd = OMX_FALSE;

    return AFMNmfHost_ProcessingComp::doSpecificSendCommand(eCmd, nData, bDeferredCmd);
}

OMX_ERRORTYPE PcmSplitterNmfHost::disablePortNotification(OMX_U32 nPortIndex)
{
    if (nPortIndex != 0) {
        deInstantiateOutputPort(nPortIndex-1);
    }

    return OMX_ErrorNone;
}

OMX_ERRORTYPE PcmSplitterNmfHost::doSpecificEventHandler_cb(OMX_EVENTTYPE event, OMX_U32 nData1, OMX_U32 nData2, OMX_BOOL & deferEventHandler)
{
    if(event == (OMX_EVENTTYPE)OMX_EventIndexSettingChanged && nData2 == (OMX_U32)0x1){
        // The audio OMX index are not seen by the DSP
        // We decided to do the conversion here
        nData2 = OMX_IndexConfigAudioVolumeRamp;
        deferEventHandler = OMX_TRUE;
        mENSComponent.eventHandler(event, nData1, nData2);
    }
    else {
        deferEventHandler = OMX_FALSE;
    }

    return AFMNmfHost_ProcessingComp::doSpecificEventHandler_cb(event,nData1,nData2,deferEventHandler);
}

