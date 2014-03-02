/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "cm/inc/cm_macros.h"
#include "PcmSplitterNmfMpc.h"
#include "ENS_Nmf.h"
#include "AFM_nmf.h"
#include "AFM_Utils.h"
#include "PcmSplitter_PcmPort.h"
#include "OMX_CoreExt.h"

#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "pcm_splitter_proxy_PcmSplitterNmfMpcTraces.h"
#endif

PcmSplitterNmfMpc::PcmSplitterNmfMpc(ENS_Component &enscomp)
    : AFMNmfMpc_ProcessingComp(enscomp)
{
    for (int i = 0; i < NB_OUTPUT; i++) {
        mNmfVolctrl[i] = 0;
        mHeap[i] = 0;
        mHeapSize[i] = 0;
    }

    mNmfEffectsLib = 0;

}

void PcmSplitterNmfMpc::registerStubsAndSkels(void) {
    CM_REGISTER_STUBS(pcm_splitter_cpp);
}


void PcmSplitterNmfMpc::unregisterStubsAndSkels(void) {
    CM_UNREGISTER_STUBS(pcm_splitter_cpp);
}


OMX_ERRORTYPE PcmSplitterNmfMpc::construct(void)
{
    OMX_ERRORTYPE error;

    registerStubsAndSkels();

    error = AFMNmfMpc_ProcessingComp::construct();
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}

OMX_ERRORTYPE PcmSplitterNmfMpc::destroy(void)
{
    OMX_ERRORTYPE error;

    error = AFMNmfMpc_ProcessingComp::destroy();
    if (error != OMX_ErrorNone) return error;

    unregisterStubsAndSkels();

    return OMX_ErrorNone;
}

OMX_ERRORTYPE PcmSplitterNmfMpc::applyConfig(
    OMX_INDEXTYPE nConfigIndex,
    OMX_PTR pComponentConfigStructure)
{
    switch (nConfigIndex) {

        case OMX_IndexConfigAudioChannelSwitchBoard:
        {
            OMX_SYMBIAN_AUDIO_CONFIG_CHANNELSWITCHBOARDTYPE * pConfig =
                (OMX_SYMBIAN_AUDIO_CONFIG_CHANNELSWITCHBOARDTYPE *) pComponentConfigStructure;
            t_uint16 channelSwitchBoard[NMF_AUDIO_MAXCHANNELS];

            for (int i = 0; i < NMF_AUDIO_MAXCHANNELS; i++) {
                channelSwitchBoard[i] = pConfig->nOutputChannel[i];
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
            return AFMNmfMpc_ProcessingComp::applyConfig(
                        nConfigIndex, pComponentConfigStructure);
    }
}


const char* PcmSplitterNmfMpc::suffixItf[]=
{ "", "[0]", "[1]" };

t_cm_instance_handle PcmSplitterNmfMpc::getNmfHandle(OMX_U32 portIdx) const {
    DBC_ASSERT(portIdx < mENSComponent.getPortCount());
    return (t_cm_instance_handle) mNmfMain;
}

const char * PcmSplitterNmfMpc::getNmfSuffix(OMX_U32 portIdx) const {  
    DBC_ASSERT(portIdx < mENSComponent.getPortCount());
    return suffixItf[portIdx];
}


OMX_ERRORTYPE PcmSplitterNmfMpc::instantiateLibraries(void) {
    OMX_ERRORTYPE error;

    error = ENS::instantiateNMFComponent(mENSComponent.getNMFDomainHandle(),
            "libeffects.mpc.libeffects", "libeffects", &mNmfEffectsLib, getPortPriorityLevel(0));
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponent(mNmfEffectsLib, "osttrace", mOstTrace, "osttrace");
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponent(mNmfMain, "osttrace", mOstTrace, "osttrace");
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponent(
            mNmfMain, "libSRC", mNmfEffectsLib, "libresampling");
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponent(
            mNmfMain, "genericfsm", mNmfGenericFsmLib, "genericfsm");
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponent(
            mNmfMain, "componentfsm", mNmfComponentFsmLib, "componentfsm");
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}

OMX_ERRORTYPE PcmSplitterNmfMpc::deInstantiateLibraries(void) {
    OMX_ERRORTYPE error;

    error = ENS::unbindComponent(mNmfMain, "componentfsm");
    if (error != OMX_ErrorNone) return error;

    error = ENS::unbindComponent(mNmfMain, "genericfsm");
    if (error != OMX_ErrorNone) return error;

    error = ENS::unbindComponent(mNmfMain, "libSRC");
    if (error != OMX_ErrorNone) return error;

    error = ENS::unbindComponent(mNmfEffectsLib, "osttrace");
    if (error != OMX_ErrorNone) return error;

    error = ENS::unbindComponent(mNmfMain, "osttrace");
    if (error != OMX_ErrorNone) return error;

    error = ENS::destroyNMFComponent(mNmfEffectsLib);
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}

OMX_ERRORTYPE PcmSplitterNmfMpc::instantiateOutputPort(int idx) {
    OMX_ERRORTYPE error;
    ENS_String<20> volctrlitf("volctrl");
    volctrlitf.concat(suffixItf[idx+1]);
    ENS_String<20> eventhandleritf("eventhandler");
    eventhandleritf.concat(suffixItf[idx+1]);

    allocatePortHeap(idx);

    error = ENS::instantiateNMFComponent(mENSComponent.getNMFDomainHandle(),
            "volctrl.nmfil.effect", "volctrl", &mNmfVolctrl[idx], getPortPriorityLevel(0));
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponent(
            mNmfMain, volctrlitf, mNmfVolctrl[idx], "volctrl");
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponent(
            mNmfVolctrl[idx], "proxy", mNmfMain, eventhandleritf);
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponent(
            mNmfVolctrl[idx], "libvolctrl", mNmfEffectsLib, "libvolctrl");
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}

OMX_ERRORTYPE PcmSplitterNmfMpc::deInstantiateOutputPort(int idx) {
    OMX_ERRORTYPE error;
    ENS_String<20> volctrlitf("volctrl");
    volctrlitf.concat(suffixItf[idx+1]);

    error = ENS::unbindComponent(mNmfVolctrl[idx], "libvolctrl");
    if (error != OMX_ErrorNone) return error;

    error = ENS::unbindComponent(mNmfVolctrl[idx], "proxy");
    if (error != OMX_ErrorNone) return error;

    error = ENS::unbindComponent(mNmfMain, volctrlitf);
    if (error != OMX_ErrorNone) return error;

    error = ENS::destroyNMFComponent(mNmfVolctrl[idx]);
    if (error != OMX_ErrorNone) return error;

    freePortHeap(idx);

    return OMX_ErrorNone;
}

OMX_ERRORTYPE PcmSplitterNmfMpc::instantiateMain(void) {
    OMX_ERRORTYPE error;

    error = ENS::instantiateNMFComponent(mENSComponent.getNMFDomainHandle(),
            "pcm_splitter.nmfil.wrapper", "pcm_splitter", &mNmfMain, getPortPriorityLevel(0));
    if (error != OMX_ErrorNone) return error;

    error = instantiateLibraries();
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponentAsynchronous(
            mNmfMain, "me", mNmfMain, "postevent", mENSComponent.getPortCount());
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponentFromHostEx(mNmfMain, "configure",
            &mIconfigure, mENSComponent.getPortCount()*10);
    if (error != OMX_ErrorNone) return error;

    for (unsigned int idx = 1; idx < mENSComponent.getPortCount(); idx++) {
        ENS_Port *outport = static_cast<ENS_Port *>(mENSComponent.getPort(idx));

        if (outport->isEnabled()) {
            error = instantiateOutputPort(idx-1);
            if (error != OMX_ErrorNone) return error;
        }
    }

    return OMX_ErrorNone;
}

OMX_ERRORTYPE PcmSplitterNmfMpc::deInstantiateMain(void) {
    OMX_ERRORTYPE error;

    for (unsigned int idx = 1; idx < mENSComponent.getPortCount(); idx++) {
        ENS_Port *outport = static_cast<ENS_Port *>(mENSComponent.getPort(idx));

        if (outport->isEnabled()) {
            error = deInstantiateOutputPort(idx-1);
            if (error != OMX_ErrorNone) return error;
        }
    }

    error = ENS::unbindComponentFromHost( &mIconfigure);
    if (error != OMX_ErrorNone) return error;

    error = ENS::unbindComponentAsynchronous(mNmfMain, "me");
    if (error != OMX_ErrorNone) return error;

    error = deInstantiateLibraries();
    if (error != OMX_ErrorNone) return error;

    error = ENS::destroyNMFComponent(mNmfMain);
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}

OMX_ERRORTYPE PcmSplitterNmfMpc::startMain(void) {
    OMX_ERRORTYPE error;

    error = ENS::startNMFComponent(mNmfMain);
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}


OMX_ERRORTYPE PcmSplitterNmfMpc::stopMain(void) {
    OMX_ERRORTYPE error;

    error = ENS::stopNMFComponent(mNmfMain);
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}

void
PcmSplitterNmfMpc::fillInputPortParam(InputPortParam_t  &inputportparam) {
    OMX_AUDIO_PARAM_PCMMODETYPE pcmSettings;
    AFM_PARAM_PCMLAYOUTTYPE pcmLayout;
    
    pcmSettings = static_cast<AFM_PcmPort *>(mENSComponent.getPort(0))->getPcmSettings();
    pcmLayout   = static_cast<AFM_PcmPort *>(mENSComponent.getPort(0))->getPcmLayout();

    inputportparam.nSamplingRate = AFM::sampleFreq(pcmSettings.nSamplingRate);
    inputportparam.nChannels     = pcmLayout.nChannels;
    inputportparam.nBlockSize    = pcmLayout.nBlockSize;
    
    OstTraceFiltInst3(TRACE_DEBUG,"fillInputPortParam nSamplingRate=%d nChannels=%d nBlockSize=%d", inputportparam.nSamplingRate,inputportparam.nChannels,inputportparam.nBlockSize);

}

void
PcmSplitterNmfMpc::fillOutputPortParam(int idx, OutputPortParam_t  &outputportparam) {
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
    outputportparam.pHeap                = mHeap[idx];
    outputportparam.nHeapSize            = mHeapSize[idx];

    // Configs
    OMX_U32 channelSwitchBoard[NMF_AUDIO_MAXCHANNELS];
    port->getChannelSwitchBoard(channelSwitchBoard);
    for (int i = 0; i < NMF_AUDIO_MAXCHANNELS; i++) {
        outputportparam.nChannelSwitchBoard[i] = channelSwitchBoard[i];
    }
    if((pcmSettingsIn.nChannels == 1) && (pcmSettings.nChannels == 2))
    {
        OstTraceFiltInst1(TRACE_DEBUG,"fillOutputPortParam portIdx=%d we are forcing upmix ",idx+1);
        // In this case we need to do upmix (1->2) conversion 
        // with channel switchboard
        outputportparam.nChannelSwitchBoard[0] = 0; //Ouput Left = Input
        outputportparam.nChannelSwitchBoard[1] = 0; //Ouput Right = Input
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

    OstTraceFiltInst4(TRACE_DEBUG,"fillOutputPortParam portIdx=%d nSamplingRate=%d nChannels=%d nBlockSize=%d",idx+1,outputportparam.nSamplingRate,outputportparam.nChannels,outputportparam.nBlockSize );
    OstTraceFiltInst4(TRACE_DEBUG,"fillOutputPortParam bPaused=%d bMute=%d nBalance=%d nVolume=%d", outputportparam.bPaused ,outputportparam.bMute,outputportparam.nBalance,outputportparam.nVolume);

}


OMX_ERRORTYPE PcmSplitterNmfMpc::configureMain(void) {
    SplitterParam_t splitterParam;

    fillInputPortParam(splitterParam.inputPortParam);

    for (unsigned int i = 1; i < mENSComponent.getPortCount(); i++) {
        fillOutputPortParam(i-1, splitterParam.outputPortParam[i-1]);
    }

    splitterParam.traceAddr  = getDspAddr();

    mIconfigure.setParameter(splitterParam);
     
    return OMX_ErrorNone;
}




OMX_ERRORTYPE PcmSplitterNmfMpc::doSpecificSendCommand(
    OMX_COMMANDTYPE eCmd,
    OMX_U32 nData,
    OMX_BOOL &bDeferredCmd)
{
    if (eCmd == OMX_CommandPortEnable) {
        if (nData == 0) {
            configureMain();
        } else {
            OutputPortParam_t outputportparam;
            instantiateOutputPort(nData-1);
            fillOutputPortParam(nData-1, outputportparam);
            mIconfigure.setOutputPortParameter(nData, outputportparam);
        }
    }

    bDeferredCmd = OMX_FALSE;

    return AFMNmfMpc_ProcessingComp::doSpecificSendCommand(eCmd, nData, bDeferredCmd);
}

OMX_ERRORTYPE PcmSplitterNmfMpc::disablePortNotification(OMX_U32 nPortIndex)
{
    if (nPortIndex != 0) {
        deInstantiateOutputPort(nPortIndex-1);
    }

    return OMX_ErrorNone;
}

OMX_ERRORTYPE PcmSplitterNmfMpc::doSpecificEventHandler_cb(OMX_EVENTTYPE event, OMX_U32 nData1, OMX_U32 nData2, OMX_BOOL & deferEventHandler)
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

    return AFMNmfMpc_ProcessingComp::doSpecificEventHandler_cb(event,nData1,nData2,deferEventHandler);
}

//  table of heap size needed by SRC
//  this table is generated with .arch testcase
int PcmSplitterNmfMpc::srcHeapSizes[9][9]=
{
    {14,293,346,376,755,436,496,875,616},
    {290,14,722,752,376,812,872,496,943},
    {316,695,14,336,715,376,798,846,496},
    {316,695,316,14,293,346,376,755,436},
    {692,316,692,290,14,722,752,376,812},
    {316,695,316,316,695,14,336,715,376},
    {316,695,718,316,695,316,14,293,346},
    {692,316,763,692,316,692,290,14,722},
    {316,695,316,316,695,316,316,695,14}
};

int PcmSplitterNmfMpc::srcHeapSizeLowLatency = 135;

int
PcmSplitterNmfMpc::computeHeapSize(int idx) {

    PcmSplitter *proxy = (PcmSplitter*)&mENSComponent;
    OMX_AUDIO_PARAM_PCMMODETYPE pcmSettingsIn, pcmSettingsOut;

    pcmSettingsIn   = static_cast<AFM_PcmPort *>(proxy->getPort(0))->getPcmSettings();
    pcmSettingsOut = static_cast<AFM_PcmPort *>(proxy->getPort(idx+1))->getPcmSettings();
     
    t_sample_freq freqIn = AFM::sampleFreq(pcmSettingsIn.nSamplingRate);
    t_sample_freq freqOut = AFM::sampleFreq(pcmSettingsOut.nSamplingRate);
    
    if (freqIn == freqOut) {
        return 0;
    }

    int srcHeapSize;

    if ((freqIn == FREQ_8KHZ || freqIn == FREQ_16KHZ || freqIn == FREQ_48KHZ)
            && (freqOut == FREQ_8KHZ || freqOut == FREQ_16KHZ || freqOut == FREQ_48KHZ)) {
        srcHeapSize = srcHeapSizeLowLatency;
    } else {
        srcHeapSize = srcHeapSizes[freqIn-FREQ_48KHZ][freqOut-FREQ_48KHZ];
    }
   
    AFM_PARAM_PCMLAYOUTTYPE pcmLayoutIn = static_cast<AFM_PcmPort *>(proxy->getPort(0))->getPcmLayout();
    AFM_PARAM_PCMLAYOUTTYPE pcmLayoutOut = static_cast<AFM_PcmPort *>(proxy->getPort(idx+1))->getPcmLayout();

    return (srcHeapSize + pcmLayoutIn.nBlockSize ) * pcmLayoutOut.nChannels;
}

void
PcmSplitterNmfMpc::allocatePortHeap(int idx) {
    t_uint32 dspAddr;

    mHeapSize[idx] = computeHeapSize(idx);

    OstTraceFiltInst2(TRACE_DEBUG,"allocating %d words for port %d", mHeapSize[idx], idx+1);

    if (mHeapSize[idx] == 0) return;
    
    OMX_ERRORTYPE error = ENS::allocMpcMemory(mENSComponent.getNMFDomainHandle(), 
            CM_MM_MPC_TCM24, mHeapSize[idx], CM_MM_ALIGN_WORD, &mMemory[idx]);
    DBC_ASSERT(error == OMX_ErrorNone);

    CM_GetMpcMemoryMpcAddress(mMemory[idx], &dspAddr);

    mHeap[idx] = (void *) dspAddr;
}

void 
PcmSplitterNmfMpc::freePortHeap(int idx) {
    if (mHeapSize[idx] != 0) {
        ENS::freeMpcMemory(mMemory[idx]);
    }
}
