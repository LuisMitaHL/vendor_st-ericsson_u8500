/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "cm/inc/cm_macros.h"
#include "BinarySplitterNmfMpc.h"
#include "BinarySplitter_PcmPort.h"
#include "ENS_Nmf.h"
#include "AFM_nmf.h"
#include "AFM_Utils.h"

#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "binary_splitter_proxy_BinarySplitterNmfMpcTraces.h"
#endif

BinarySplitterNmfMpc::BinarySplitterNmfMpc(ENS_Component &enscomp)
        : AFMNmfMpc_ProcessingComp(enscomp)
{
    OMX_U8 i = 0;
    mBlockSize = DEFAULT_PCM_BLOCKSIZE;
    mNmfSyncLib = 0;

    for (i = 0; i < (NB_INPUT_PORTS + NB_OUTPUT_PORTS); i++)
    {
        mChannelConfig.nChans[i] = 0;
    }

    for (i = 0; i < OMX_AUDIO_MAXCHANNELS; i++)
    {
        mOutChannelConfig.nOutPutChannel[i] = 0;
    }
}

void BinarySplitterNmfMpc::registerStubsAndSkels(void)
{
    CM_REGISTER_STUBS(binary_splitter_cpp);
}


void BinarySplitterNmfMpc::unregisterStubsAndSkels(void)
{
    CM_UNREGISTER_STUBS(binary_splitter_cpp);
}

OMX_ERRORTYPE BinarySplitterNmfMpc::construct(void)
{
    OMX_ERRORTYPE error;

    registerStubsAndSkels();

    error = AFMNmfMpc_ProcessingComp::construct();
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}

OMX_ERRORTYPE BinarySplitterNmfMpc::destroy(void)
{
    OMX_ERRORTYPE error;

    error = AFMNmfMpc_ProcessingComp::destroy();
    if (error != OMX_ErrorNone) return error;

    unregisterStubsAndSkels();

    return OMX_ErrorNone;
}

OMX_ERRORTYPE BinarySplitterNmfMpc::applyConfig(
    OMX_INDEXTYPE nConfigIndex,
    OMX_PTR pComponentConfigStructure)
{
    OMX_U32 i;
    switch (nConfigIndex) {
    case OMX_IndexConfigAudioChannelSwitchBoard:
    {
        OMX_SYMBIAN_AUDIO_CONFIG_CHANNELSWITCHBOARDTYPE * pConfig =
            (OMX_SYMBIAN_AUDIO_CONFIG_CHANNELSWITCHBOARDTYPE *) pComponentConfigStructure;
        ENS_Port *outport = static_cast<ENS_Port *>(mENSComponent.getPort(pConfig->nPortIndex));
        if (outport->isEnabled())
        {
            for (i = 0; i<OMX_AUDIO_MAXCHANNELS; i++)
            {
                mOutChannelConfig.nOutPutChannel[i] = pConfig->nOutputChannel[i];
            }
            mIconfigure.setOutputChannelConfig(pConfig->nPortIndex, mOutChannelConfig);
        }
        return OMX_ErrorNone;
    }

    default:
        return AFMNmfMpc_ProcessingComp::applyConfig(
                   nConfigIndex, pComponentConfigStructure);
    }
}

t_cm_instance_handle BinarySplitterNmfMpc::getNmfHandle(OMX_U32 portIdx) const {
    DBC_ASSERT(portIdx < mENSComponent.getPortCount());
    return  mNmfMain;
}

const char *  BinarySplitterNmfMpc::getNmfSuffix(OMX_U32 portIdx) const {
    if (portIdx == 0)
        return "[0]";
    else if (portIdx == 1)
        return "[0]";
    else if (portIdx == 2)
        return "[1]";

    return "";
}

OMX_ERRORTYPE BinarySplitterNmfMpc::instantiateMain(void) {
    OMX_ERRORTYPE error;

    MEMORY_TRACE_ENTER2("BinarySplitterNmfMpc::instantiateMain (%s) @0x%08X", (const char*)mENSComponent.getName(), (unsigned int)&mENSComponent);

    error = ENS::instantiateNMFComponent(
                mENSComponent.getNMFDomainHandle(), "binary_splitter.nmfil.wrapper", "binarysplitter", &mNmfMain, getPortPriorityLevel(0));
    if (error != OMX_ErrorNone) return error;

    error = ENS::instantiateNMFComponent(
                mENSComponent.getNMFDomainHandle(), "misc.synchronisation", "synchronisation",
                &mNmfSyncLib, getPortPriorityLevel(0));
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponent(mNmfMain, "osttrace", mOstTrace, "osttrace");
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponent(
                mNmfMain, "synchronisation", mNmfSyncLib, "synchronisation");
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponent(
                mNmfMain, "genericfsm", mNmfGenericFsmLib, "genericfsm");
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponent(
                mNmfMain, "componentfsm", mNmfComponentFsmLib, "componentfsm");
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponentAsynchronous(
                mNmfMain, "me", mNmfMain, "postevent", mENSComponent.getPortCount());
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponentFromHostEx(mNmfMain, "configure",
                                         &mIconfigure, mENSComponent.getPortCount());
    if (error != OMX_ErrorNone) return error;

    MEMORY_TRACE_LEAVE2("BinarySplitterNmfMpc::instantiateMain (%s) @0x%08X", (const char*)mENSComponent.getName(), (unsigned int)&mENSComponent);

    return OMX_ErrorNone;
}

OMX_ERRORTYPE BinarySplitterNmfMpc::startMain(void) {
    OMX_ERRORTYPE error;

    error = ENS::startNMFComponent(mNmfMain);
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}


OMX_ERRORTYPE BinarySplitterNmfMpc::stopMain(void) {
    OMX_ERRORTYPE error;

    error = ENS::stopNMFComponent(mNmfMain);
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}


OMX_ERRORTYPE BinarySplitterNmfMpc::configureMain(void) {
    OMX_U8 i = 0;
    OMX_U8 portcount = (OMX_U8) mENSComponent.getPortCount();

    BinarySplitter *proxy = static_cast<BinarySplitter * >(&mENSComponent);

    OMX_AUDIO_PARAM_PCMMODETYPE &pcmSettingsIn =
        ((AFM_PcmPort *) mENSComponent.getPort(0))->getPcmSettings();
    mChannelConfig.nChans[0] = pcmSettingsIn.nChannels;
    OstTraceFiltInst3(TRACE_API,"BinarySplitterNmfMpc::configureMain portId=0 mBlockSize=%d nchannels=%d frequency=%d",mBlockSize,pcmSettingsIn.nChannels,pcmSettingsIn.nSamplingRate);

    for (i = 1; i < portcount; i++)
    {
        OMX_AUDIO_PARAM_PCMMODETYPE &pcmSettingsOut =
            ((AFM_PcmPort *) mENSComponent.getPort(i))->getPcmSettings();
        mChannelConfig.nChans[i] = pcmSettingsOut.nChannels;
        OstTraceFiltInst4(TRACE_API,"BinarySplitterNmfMpc::configureMain portId=%d mBlockSize=%d nchannels=%d frequency=%d",i,mBlockSize,pcmSettingsOut.nChannels,pcmSettingsOut.nSamplingRate);

    }

    mIconfigure.setParameter(AFM::sampleFreq(pcmSettingsIn.nSamplingRate),
                             mChannelConfig,
                             mBlockSize);

    mIconfigure.setTrace(getDspAddr());

    for (i = 0; i < portcount-1; i++)
    {
        if (proxy->getOutConfig(i) == true)
        {
            applyConfig((OMX_INDEXTYPE)OMX_IndexConfigAudioChannelSwitchBoard, proxy->getAudioSwitchboardConfig(i));
        }
    }

    return OMX_ErrorNone;
}

OMX_ERRORTYPE BinarySplitterNmfMpc::deInstantiateMain(void) {
    OMX_ERRORTYPE error;

    error = ENS::unbindComponent(mNmfMain, "genericfsm");
    if (error != OMX_ErrorNone) return error;

    error = ENS::unbindComponent(mNmfMain, "componentfsm");
    if (error != OMX_ErrorNone) return error;

    error = ENS::unbindComponentAsynchronous(mNmfMain, "me");
    if (error != OMX_ErrorNone) return error;

    error = ENS::unbindComponentFromHost( &mIconfigure);
    if (error != OMX_ErrorNone) return error;

    error = ENS::unbindComponent(mNmfMain, "synchronisation");
    if (error != OMX_ErrorNone) return error;

    error = ENS::destroyNMFComponent(mNmfSyncLib);
    if (error != OMX_ErrorNone) return error;

    error = ENS::unbindComponent(mNmfMain, "osttrace");
    if (error != OMX_ErrorNone) return error;

    error = ENS::destroyNMFComponent(mNmfMain);
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}

OMX_ERRORTYPE BinarySplitterNmfMpc::doSpecificSendCommand(
    OMX_COMMANDTYPE eCmd,
    OMX_U32 nData,
    OMX_BOOL &bDeferredCmd)
{
    switch (eCmd)
    {
    case OMX_CommandPortEnable:
    {
        BinarySplitter *proxy = static_cast<BinarySplitter * >(&mENSComponent);

        OMX_AUDIO_PARAM_PCMMODETYPE &pcmSettingsIn =
            ((AFM_PcmPort *) mENSComponent.getPort(0))->getPcmSettings();
        mChannelConfig.nChans[0] = pcmSettingsIn.nChannels;

        if (nData != 0) {
            OMX_AUDIO_PARAM_PCMMODETYPE pcmSettingsOut = ((AFM_PcmPort *) mENSComponent.getPort(nData))->getPcmSettings();
            mChannelConfig.nChans[nData] = pcmSettingsOut.nChannels;

            if (proxy->getOutConfig(nData-1) == true){
                applyConfig((OMX_INDEXTYPE)OMX_IndexConfigAudioChannelSwitchBoard, proxy->getAudioSwitchboardConfig(nData-1));
            }
        }

        mIconfigure.setParameter(AFM::sampleFreq(pcmSettingsIn.nSamplingRate),
                                 mChannelConfig,
                                 mBlockSize);

        break;
    }

    case OMX_CommandPortDisable:
        if (nData == 1) {
            mNmfBufferSupplier[1] = OMX_BufferSupplyUnspecified;
        }
        break;

    default:
        break;
    }

    return AFMNmfMpc_ProcessingComp::doSpecificSendCommand(eCmd, nData, bDeferredCmd);
}
