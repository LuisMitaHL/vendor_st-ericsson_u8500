/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include "BinarySplitterNmfHost.h"
#include "BinarySplitter_PcmPort.h"
#include "AFM_Utils.h"
#include "binary_splitter/nmfil/host/composite/pp_splitter.hpp"

BinarySplitterNmfHost::BinarySplitterNmfHost(ENS_Component &enscomp)
        : AFMNmfHost_ProcessingComp(enscomp) {
    pcmProcConfig.mode = PCMPROCESSING_MODE_COUNT;
    pcmProcConfig.block_size = 0;
    pcmProcConfig.out_freq = FREQ_UNKNOWN;
    pcmProcConfig.out_nof_channels = 0;
    pcmProcConfig.out_nof_bits_per_sample = 0;

    mNmfil      = 0;
    mBlockSize  = 0;
}

OMX_ERRORTYPE BinarySplitterNmfHost::construct(void)
{
    OMX_ERRORTYPE error;

    mBlockSize = DEFAULT_PCM_BLOCKSIZE;

    error = AFMNmfHost_ProcessingComp::construct();
    if (error != OMX_ErrorNone) return error;
    return OMX_ErrorNone;
}

OMX_ERRORTYPE BinarySplitterNmfHost::destroy(void)
{
    OMX_ERRORTYPE error;

    error = AFMNmfHost_ProcessingComp::destroy();
    if (error != OMX_ErrorNone) return error;


    return OMX_ErrorNone;
}

NMF::Composite *  BinarySplitterNmfHost::getNmfHandle(OMX_U32 portIdx) const {
    DBC_ASSERT(portIdx < mENSComponent.getPortCount());
    return  mNmfMain;
}

const char *  BinarySplitterNmfHost::getNmfSuffix(OMX_U32 portIdx) const {
    if (portIdx == 0)
        return "[0]";
    else if (portIdx == 1)
        return "[0]";
    else if (portIdx == 2)
        return "[1]";

    return "";
}

OMX_ERRORTYPE BinarySplitterNmfHost::instantiateMain(void) {
    t_nmf_error   nmf_error;

    mNmfMain = (NMF::Composite*)binary_splitter_nmfil_host_composite_pp_splitterCreate();
    if (mNmfMain == NULL) return OMX_ErrorInsufficientResources;

    ((binary_splitter_nmfil_host_composite_pp_splitter *)mNmfMain)->priority = getPortPriorityLevel(0);

    if (mNmfMain->construct() != NMF_OK) return OMX_ErrorInsufficientResources;

    nmf_error = mNmfMain->bindFromUser("configure", 3, &mIconfigure);
    if (nmf_error != NMF_OK) {
        NMF_LOG("Error: unable to bind main configure!...[%d]\n", nmf_error);
        return OMX_ErrorInsufficientResources;
    }

    return OMX_ErrorNone;
}

OMX_ERRORTYPE BinarySplitterNmfHost::deInstantiateMain(void) {
    t_nmf_error   nmf_error;

    nmf_error = mNmfMain->unbindFromUser("configure");
    if (nmf_error != NMF_OK) {
        NMF_LOG("Error: unable to unbind main configure!...[%d]\n", nmf_error);
        return OMX_ErrorInsufficientResources;
    }

    if (mNmfMain->destroy() != NMF_OK) return OMX_ErrorInsufficientResources;
    binary_splitter_nmfil_host_composite_pp_splitterDestroy((binary_splitter_nmfil_host_composite_pp_splitter*&)mNmfMain);

    return OMX_ErrorNone;

}
OMX_ERRORTYPE BinarySplitterNmfHost::startMain(void) {
    mNmfMain->start();
    return OMX_ErrorNone;
}


OMX_ERRORTYPE BinarySplitterNmfHost::stopMain(void) {
    mNmfMain->stop();

    return OMX_ErrorNone;
}


OMX_ERRORTYPE BinarySplitterNmfHost::configureMain(void) {
    BinarySplitter_PcmPort *pcmPort = (BinarySplitter_PcmPort *) getENSComponent().getPort(0);

    OMX_AUDIO_PARAM_PCMMODETYPE pcmSettingsIn = pcmPort->getPcmSettings();

    memset(&pcmProcConfig, 0, sizeof(t_pcmprocessing_config_host));

    // Set PCM processing config
    pcmProcConfig.mode                    = PCMPROCESSING_MODE_NOT_INPLACE;
    pcmProcConfig.block_size              = mBlockSize;
    pcmProcConfig.out_freq                = AFM::sampleFreq(pcmSettingsIn.nSamplingRate);
    pcmProcConfig.out_nof_channels        = pcmSettingsIn.nChannels;
    pcmProcConfig.out_nof_bits_per_sample = pcmSettingsIn.nBitPerSample;

    mIconfigure.setParameter(pcmProcConfig);

    for (OMX_U32 idx=0; idx<mENSComponent.getPortCount()-1; idx++) {
        BinarySplitter_PcmPort *outputPort = static_cast<BinarySplitter_PcmPort *>(mENSComponent.getPort(idx+1));
        if (outputPort->isEnabled()){
            mIconfigure.setSynchronizedOutputPort(idx, outputPort->isSynchronized());
        }
    }

    return OMX_ErrorNone;
}

OMX_ERRORTYPE BinarySplitterNmfHost::doSpecificSendCommand(
    OMX_COMMANDTYPE eCmd,
    OMX_U32 nData,
    OMX_BOOL &bDeferredCmd)
{
    if (eCmd == OMX_CommandPortEnable) {
        if (nData == 0)
        {
            OMX_AUDIO_PARAM_PCMMODETYPE &pcmSettingsIn =
                ((AFM_PcmPort *) mENSComponent.getPort(0))->getPcmSettings();
            // Set PCM processing config
            pcmProcConfig.mode                    = PCMPROCESSING_MODE_NOT_INPLACE;
            pcmProcConfig.block_size              = DEFAULT_PCM_BLOCKSIZE;
            pcmProcConfig.out_freq                = AFM::sampleFreq(pcmSettingsIn.nSamplingRate);
            pcmProcConfig.out_nof_channels        = pcmSettingsIn.nChannels;
            pcmProcConfig.out_nof_bits_per_sample = pcmSettingsIn.nBitPerSample;

            mIconfigure.setParameter(pcmProcConfig);
        }
        else
        {
            BinarySplitter_PcmPort *outputPort = static_cast<BinarySplitter_PcmPort *>(mENSComponent.getPort(nData));

            mIconfigure.setSynchronizedOutputPort(nData-1, outputPort->isSynchronized());
        }
    }

    bDeferredCmd = OMX_FALSE;

    return AFMNmfHost_ProcessingComp::doSpecificSendCommand(eCmd, nData, bDeferredCmd);
}

