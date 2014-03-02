/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   AFMNmfMpc_PcmProcessing.cpp
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "cm/inc/cm_macros.h"
#include "AFMNmfMpc_PcmProcessing.h"
#include "AFM_PcmProcessing.h"
#include "ENS_Nmf.h"
#include "AFM_Utils.h"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "afm_proxy_base_AFMNmfMpc_PcmProcessingTraces.h"
#endif
EXPORT_C OMX_ERRORTYPE
AFMNmfMpc_PcmProcessing::construct(void)
{
    OMX_ERRORTYPE error;

    setPriorityLevel((OMX_U32)NMF_SCHED_NORMAL);

    error = AFMNmfMpc_ProcessingComp::construct();
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}

EXPORT_C OMX_ERRORTYPE
AFMNmfMpc_PcmProcessing::instantiateMain(void) {
    OMX_ERRORTYPE error;

    // support only notInplace mode
    // as disabling port when port sharing are used
    // is not always correctly handled => modification needed at ENS level .
    mNmfBufferSupplier[1] = OMX_BufferSupplyInput;

    MEMORY_TRACE_ENTER2("AFMNmfMpc_PcmProcessing::instantiateAlgo (%s) @0x%08X", (const char*)mENSComponent.getName(), (unsigned int)&mENSComponent);

    error = instantiateAlgo();
    if (error != OMX_ErrorNone) return error;

    MEMORY_TRACE_LEAVE2("AFMNmfMpc_PcmProcessing::instantiateAlgo (%s) @0x%08X", (const char*)mENSComponent.getName(), (unsigned int)&mENSComponent);

    error = ENS::instantiateNMFComponent(
                mENSComponent.getNMFDomainHandle(), "pcmprocessings.wrapper",
                "wrapper", &mNmfMain, getPriorityLevel());
    if (error != OMX_ErrorNone) return error;

    error = ENS::instantiateNMFComponent(
            mENSComponent.getNMFDomainHandle(), "misc.synchronisation", "synchronisation",
            &mNmfSyncLib, getPriorityLevel());
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponent(
            mNmfMain, "synchronisation", mNmfSyncLib, "synchronisation");
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponent(
            (t_cm_instance_handle)mNmfMain, "osttrace", mOstTrace, "osttrace");
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponent(
                mNmfMain, "genericfsm", mNmfGenericFsmLib, "genericfsm");
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponent(
                mNmfMain, "componentfsm", mNmfComponentFsmLib, "componentfsm");
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponentAsynchronous(
                mNmfMain, "me", mNmfMain, "postevent", 2);
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponent(
                mNmfMain, "effect", mNmfil, "effect");
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponentFromHostEx(mNmfMain, "configure",
                                       &mIconfigure, 1);
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}

EXPORT_C OMX_ERRORTYPE
AFMNmfMpc_PcmProcessing::deInstantiateMain(void) {
    OMX_ERRORTYPE error;

    error = ENS::unbindComponent(mNmfMain, "effect");
    if (error != OMX_ErrorNone) return error;

    error = ENS::unbindComponentFromHost(&mIconfigure);
    if (error != OMX_ErrorNone) return error;

    error = ENS::unbindComponent(
                mNmfMain, "componentfsm");
    if (error != OMX_ErrorNone) return error;

    error = ENS::unbindComponent(mNmfMain, "genericfsm");
    if (error != OMX_ErrorNone) return error;

    error = ENS::unbindComponentAsynchronous(mNmfMain, "me");
    if (error != OMX_ErrorNone) return error;

    error = deInstantiateAlgo();
    if (error != OMX_ErrorNone) return error;

	error = ENS::unbindComponent(mNmfMain, "osttrace");
    if (error != OMX_ErrorNone) return error;

    error = ENS::unbindComponent(mNmfMain, "synchronisation");
    if (error != OMX_ErrorNone) return error;

    error = ENS::destroyNMFComponent(mNmfSyncLib);
    if (error != OMX_ErrorNone) return error;

    error = ENS::destroyNMFComponent(mNmfMain);
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}



EXPORT_C OMX_ERRORTYPE
AFMNmfMpc_PcmProcessing::startMain(void) {
    OMX_ERRORTYPE error;

    error = ENS::startNMFComponent(mNmfMain);
    if (error != OMX_ErrorNone) return error;

    error = startAlgo();
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}


EXPORT_C OMX_ERRORTYPE
AFMNmfMpc_PcmProcessing::stopMain(void) {
    OMX_ERRORTYPE error;

    error = ENS::stopNMFComponent(mNmfMain);
    if (error != OMX_ErrorNone) return error;

    error = stopAlgo();
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}


EXPORT_C OMX_ERRORTYPE
AFMNmfMpc_PcmProcessing::configureMain(void) {
    OMX_ERRORTYPE error;
    t_pcmprocessing_config config;

    error = configureAlgo();
    if (error != OMX_ErrorNone) return error;

    //check if inplace or notInplace mode is selected
    if(!mENSComponent.getPort(1)->isEnabled()){
        mNmfBufferSupplier[1] = OMX_BufferSupplyInput;
        config.processingMode = PCMPROCESSING_MODE_NOT_INPLACE;
        OstTraceFiltInst0 (TRACE_DEBUG, "AFM_PROXY:  AFMNmfMpc_PcmProcessing::configureMain PCMPROCESSING_MODE_NOT_INPLACE (disabled port)\n");
    }
    else if(mNmfBufferSupplier[1] == OMX_BufferSupplyOutput) {
        config.processingMode = PCMPROCESSING_MODE_INPLACE_PUSH;
        OstTraceFiltInst0 (TRACE_DEBUG, "AFM_PROXY:  AFMNmfMpc_PcmProcessing::configureMain PCMPROCESSING_MODE_INPLACE_PUSH (BufferSupplyOutput)\n");
    }
    else if (mNmfBufferSupplier[1] == OMX_BufferSupplyInput) {
        config.processingMode = PCMPROCESSING_MODE_NOT_INPLACE;
        OstTraceFiltInst0 (TRACE_DEBUG, "AFM_PROXY:  AFMNmfMpc_PcmProcessing::configureMain PCMPROCESSING_MODE_NOT_INPLACE (BufferSupplyInput)\n");
    }
    else {
        config.processingMode = PCMPROCESSING_MODE_NOT_INPLACE;
        OstTraceFiltInst0 (TRACE_ERROR, "AFM_PROXY:  AFMNmfMpc_PcmProcessing::configureMain PCMPROCESSING_MODE_NOT_INPLACE (enabled Port but no buffer supplier preference -> Assert !!!)\n");
        DBC_ASSERT(0);
    }

    AFM_PcmProcessing* proxy = static_cast<AFM_PcmProcessing*>(&mENSComponent);
    AFM_PcmPort *inputPort      = static_cast<AFM_PcmPort *>(mENSComponent.getPort(0));
    AFM_PcmPort *outputPort     = static_cast<AFM_PcmPort *>(mENSComponent.getPort(1));

    DBC_ASSERT(inputPort->getPcmSettings().nSamplingRate == outputPort->getPcmSettings().nSamplingRate);
    DBC_ASSERT(inputPort->getPcmLayout().nBitsPerSample == outputPort->getPcmLayout().nBitsPerSample);

    config.effectConfig.block_size                  = proxy->getBlockSize();

    config.effectConfig.infmt.freq                  = AFM::sampleFreq(inputPort->getPcmSettings().nSamplingRate);
    config.effectConfig.infmt.nof_channels          = inputPort->getPcmLayout().nChannels;
    config.effectConfig.infmt.nof_bits_per_sample   = inputPort->getPcmLayout().nBitsPerSample;

    config.effectConfig.outfmt.freq                  = AFM::sampleFreq(outputPort->getPcmSettings().nSamplingRate);
    config.effectConfig.outfmt.nof_channels          = outputPort->getPcmLayout().nChannels;
    config.effectConfig.outfmt.nof_bits_per_sample   = outputPort->getPcmLayout().nBitsPerSample;

    mIconfigure.setParameter(config);

    return OMX_ErrorNone;
}

EXPORT_C OMX_ERRORTYPE
AFMNmfMpc_PcmProcessing::doSpecificSendCommand(
        OMX_COMMANDTYPE eCmd,
        OMX_U32 nData,
        OMX_BOOL &bDeferredCmd)
{
    /*switch (eCmd)
    {
        case OMX_CommandPortDisable:
        {
            if(nData == 1) {
                //mNmfBufferSupplier[1] = OMX_BufferSupplyUnspecified;
            }
        }
        break;

        default:
        break;
    }*/

    return AFMNmfMpc_ProcessingComp::doSpecificSendCommand(eCmd, nData, bDeferredCmd);
}


