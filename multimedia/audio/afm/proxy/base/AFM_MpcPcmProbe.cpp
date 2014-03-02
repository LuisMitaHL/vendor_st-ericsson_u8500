/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   AFM_MpcPcmPorbe.cpp
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "cm/inc/cm_macros.h"
#include "ENS_Nmf.h"
#include "AFM_MpcPcmProbe.h"
#include "AFMNmfMpc_ProcessingComp.h"
#include "ENS_Component.h"
#include "ENS_Port.h"
#include "AFM_Component.h"
#include "AFM_Utils.h"


#ifdef MMPROBE_ENABLED
// MMProbe framework
#include "t_mm_probe.h"
#include "r_mm_probe.h"
#endif

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "afm_proxy_base_AFM_MpcPcmProbeTraces.h"
#endif

OMX_ERRORTYPE
AFM_MpcPcmProbe::instantiate(OMX_U32 priority) {
    OMX_ERRORTYPE error;

    // instantiate pcmdump component
    error = ENS::instantiateNMFComponent(mENSComponent.getNMFDomainHandle(),
                                         "pcmdump",
                                         "pcmdump",
                                         &mNmfDump,
                                         priority);
    if (error != OMX_ErrorNone) return error;

    for (OMX_U32 omxPortIdx = 0; omxPortIdx < mENSComponent.getPortCount(); omxPortIdx++)
    {
        AFM_PcmPort * port = static_cast<AFM_PcmPort *>(mENSComponent.getPort(omxPortIdx));
        if (port->isMpc())
        {
            OMX_BOOL needsToBind = OMX_FALSE;
            mNmfPortHdl[omxPortIdx].nmfHdl = mAFMProcessingComp.getNmfHandle(omxPortIdx);
            // check if this handle needs to be linked to the pcmdump
            // it the OMX cpompoennt has different nmf compoennts per
            // port, then bind everyone, else bind it once for all
            if (omxPortIdx == 0)
            {
                //very first handle, so needs to be bound
                needsToBind = OMX_TRUE;
            }
            else
            {
              // check for previous handles, if this is a new handle
                for (OMX_U32 i = 0; i <= omxPortIdx; i++)
                {
                    if (mNmfPortHdl[i].nmfHdl != mNmfPortHdl[omxPortIdx].nmfHdl)
                    {
                        needsToBind = OMX_TRUE;
                        break;
                    }
                }
            }

            if (needsToBind)
            {
                OstTraceFiltStatic2(TRACE_DEBUG, "AFM_PROXY: AFM_MpcPcmProbe::instantiate: bind nmf handle 0x%x (omxPortIdx %d)", static_cast<TraceObject*>(&mENSComponent), mNmfPortHdl[omxPortIdx].nmfHdl, omxPortIdx);

                error = ENS::bindComponent(mNmfPortHdl[omxPortIdx].nmfHdl,
                                           "pcmdump",
                                           mNmfDump,
                                           "probe");
                if (error != OMX_ErrorNone) return error;

                // bind the configure interface
                error = ENS::bindComponentFromHost(mNmfPortHdl[omxPortIdx].nmfHdl,
                                                   "configure_probe",
                                                   &mIconfigure[omxPortIdx],
                                                   mENSComponent.getPortCount());
                if (error != OMX_ErrorNone) return error;

                // bind the probe complete interface
                error = ENS::bindComponentToHost(mENSComponent.getOMXHandle(),
                                                 mNmfPortHdl[omxPortIdx].nmfHdl,
                                                 "complete",
                                                 static_cast<NMF::InterfaceDescriptor *>(this),
                                                 mENSComponent.getPortCount());
                if (error != OMX_ErrorNone) return error;

                mNmfPortHdl[omxPortIdx].bIsBoundToPcmDump = OMX_TRUE;
            }
            else
            {
                OstTraceFiltStatic2(TRACE_DEBUG, "AFM_PROXY: AFM_MpcPcmProbe::instantiate: skip the binding of nmf handle 0x%x (omxPortIdx %d)", static_cast<TraceObject*>(&mENSComponent), mNmfPortHdl[omxPortIdx].nmfHdl, omxPortIdx);
            }
        }
    }

    error = ENS::bindComponent(mNmfDump,
                               "osttrace",
                               mAFMProcessingComp.getOstTrace(),
                               "osttrace");
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}

OMX_ERRORTYPE
AFM_MpcPcmProbe::deInstantiate(void) {
    OMX_ERRORTYPE error;

    for (OMX_U32 omxPortIdx = 0; omxPortIdx < mENSComponent.getPortCount(); omxPortIdx++)
    {
        if (mNmfPortHdl[omxPortIdx].bIsBoundToPcmDump == OMX_TRUE)
        {
            OstTraceFiltStatic2(TRACE_DEBUG, "AFM_PROXY: AFM_MpcPcmProbe::deInstantiate Unbind nmf handle %d (omxPortIdx %d)", static_cast<TraceObject*>(&mENSComponent), mNmfPortHdl[omxPortIdx].nmfHdl, omxPortIdx);

            error = ENS::unbindComponentToHost(mENSComponent.getOMXHandle(),
                                               mNmfPortHdl[omxPortIdx].nmfHdl,
                                               "complete");
            if (error != OMX_ErrorNone) return error;

            error = ENS::unbindComponentFromHost(&mIconfigure[omxPortIdx]);
            if (error != OMX_ErrorNone) return error;

            error = ENS::unbindComponent(mNmfPortHdl[omxPortIdx].nmfHdl, "pcmdump");
            if (error != OMX_ErrorNone) return error;
        }
    }

    error = ENS::unbindComponent(mNmfDump, "osttrace");
    if (error != OMX_ErrorNone) return error;

    error = ENS::destroyNMFComponent(mNmfDump);
    if (error != OMX_ErrorNone) return error;

    // check for any enabled enabled probe,
    // to free the shared buffers

    for (OMX_U32 i = 0; i < mENSComponent.getPortCount(); i++)
    {
        PcmProbe_t * probe = getProbeConfig(i);

        if (probe->bEnable)
        {
          error = ENS::freeMpcMemory(probe->SharedBufMemHdl);
          if (error != OMX_ErrorNone) return error;
        }
    }

    return OMX_ErrorNone;
}

OMX_ERRORTYPE
AFM_MpcPcmProbe::start(void) {
    return ENS::startNMFComponent(mNmfDump);
}

OMX_ERRORTYPE
AFM_MpcPcmProbe::stop(void) {
    return ENS::stopNMFComponent(mNmfDump);
}


OMX_ERRORTYPE AFM_MpcPcmProbe::configure(void)
{
    for (OMX_U32 i = 0; i< mENSComponent.getPortCount(); i++)
    {
        AFM_Component *proxy = static_cast<AFM_Component*>(&mENSComponent);
        AFM_AUDIO_CONFIG_PCM_PROBE * probe = proxy->getProbeConfig(i);
        // called when we switch from loaded to idle
        if (probe->bEnable)
        {
            // if any probe point is enabled, then actually enable it!
            applyConfig(probe);
        }
    }
    return OMX_ErrorNone;
}

void AFM_MpcPcmProbe::pcmdump_complete(PcmDumpCompleteInfo_t sCompleteInfo)
{
    PcmProbe_t * probe = NULL;

    probe = mAFMProcessingComp.getProbeConfig(sCompleteInfo.omx_port_idx, sCompleteInfo.effect_position_idx);
    if (probe == NULL)
    {
        OstTraceFiltStatic3(TRACE_ERROR, "AFM_PROXY: AFM_MpcPcmProbe::pcmdump_complete pcmProbe config is NULL portIdx=%d effectPositionIdx=%d portCount=%d", static_cast<TraceObject*>(&mENSComponent), sCompleteInfo.omx_port_idx, sCompleteInfo.effect_position_idx, mENSComponent.getPortCount());
        DBC_ASSERT(0);
    }

    if (probe->bEnable == OMX_FALSE)
    {
        OstTraceFiltStatic2(TRACE_WARNING, "AFM_PROXY: AFM_MpcPcmProbe::pcmdump_complete ignored since pcmProbe is disabled portIdx=%d effectPositionIdx=%d", static_cast<TraceObject*>(&mENSComponent), sCompleteInfo.omx_port_idx, sCompleteInfo.effect_position_idx);
        return;
    }

    OMX_U32 halfBufSize = probe->nBufferSize/2;
    OMX_U32 probedSize = (sCompleteInfo.offset == REGULAR_COMPLETE_CALL)?halfBufSize:(sCompleteInfo.offset*2);

    OMX_U32 addr = ((OMX_U32)probe->pSharedBufAddr) + (sCompleteInfo.bufferIdx?halfBufSize:0);

    OstTraceFiltStatic4(TRACE_DEBUG, "AFM_PROXY: AFM_MpcPcmProbe::pcmprobe_complete portIdx: %d, bufferIdx: %d, offset: %d, effectPositionIdx=%d", static_cast<TraceObject*>(&mENSComponent), sCompleteInfo.omx_port_idx, sCompleteInfo.bufferIdx, sCompleteInfo.offset, sCompleteInfo.effect_position_idx);

    OstTraceFiltStatic3(TRACE_DEBUG, "AFM_PROXY: AFM_MpcPcmProbe::pcmprobe_complete call to mmprobe: probe: %d, buf: 0x%x, size: %d", static_cast<TraceObject*>(&mENSComponent), probe->nProbeIdx, addr, probedSize);
#ifdef MMPROBE_ENABLED
    DataFormat_t format;
    AFM_PcmPort * port = static_cast<AFM_PcmPort *>(mENSComponent.getPort(sCompleteInfo.omx_port_idx));
    AFM_PARAM_PCMLAYOUTTYPE pcmLayout = port->getPcmLayout();
    OMX_AUDIO_PARAM_PCMMODETYPE pcmSettings = port->getPcmSettings();

    format.SampleRate = AFM::sampleFreq(pcmSettings.nSamplingRate);
    format.NoChannels = pcmLayout.nChannels;
    format.Interleave = MM_PROBE_INTERLEAVED;
    //    format.Resolution = (pcmLayout.nBitsPerSample == 24)?MM_PROBE_PCM_FORMAT_S24_LE:MM_PROBE_FORMAT_S16_LE;
    format.Resolution = MM_PROBE_FORMAT_S16_LE;
    // pass the buffer to mmprobe
    int res = mmprobe_probe_V2(probe->nProbeIdx,
                               (void*) addr,
                               probedSize,
                               &format);

    // check for any mmprobe return error,
    // (two much data sent, ...)
    if (res != 0)
    {
        OstTraceFiltStatic1(TRACE_ERROR, "AFM_PROXY: AFM_MpcPcmProbe::pcmprobe_complete ERROR !! call to mmprobe returned error %d", static_cast<TraceObject*>(&mENSComponent), res);
    }
#endif
    if (sCompleteInfo.offset != REGULAR_COMPLETE_CALL)
    {
        // means this is the last call to the callback probe in case of probe disabling..
        // free the shared buffer
        OstTraceFiltStatic1(TRACE_DEBUG, "AFM_PROXY: AFM_MpcPcmProbe::pcmprobe_complete free buffers for probe %d", static_cast<TraceObject*>(&mENSComponent), probe->nProbeIdx);

        OMX_ERRORTYPE error = ENS::freeMpcMemory(probe->SharedBufMemHdl);
        if (error != OMX_ErrorNone)
        {
          OstTraceFiltStatic1(TRACE_ERROR, "AFM_PROXY: AFM_MpcPcmProbe::pcmdump_complete error 0x%x while freeing shared buffer", static_cast<TraceObject*>(&mENSComponent), error);
        }
    }
}

OMX_ERRORTYPE AFM_MpcPcmProbe::applyConfig(AFM_AUDIO_CONFIG_PCM_PROBE *pConfig)
{
    OMX_ERRORTYPE error;
    CHECK_STRUCT_SIZE_AND_VERSION(pConfig,
                                  AFM_AUDIO_CONFIG_PCM_PROBE);
    OMX_BOOL bEnable;
    OMX_U32 portIdx;
    OMX_U32 dspAddr = 0;
    OMX_U32 dspBufferSize = 0;

    bEnable = pConfig->bEnable;
    portIdx = pConfig->nPortIndex;

    OstTraceFiltStatic3(TRACE_OMX_API, "AFM_PROXY: AFM_MpcPcmProbe::applyConfig AFM_AUDIO_CONFIG_PCM_PROBE: enable: %d, portIndex: %d probeIdx: %d", static_cast<TraceObject*>(&mENSComponent), bEnable, portIdx, pConfig->nProbeIdx);

    PcmProbe_t * probe = getProbeConfig(portIdx);
    if (probe == NULL) return OMX_ErrorBadParameter;

    if (bEnable == probe->bEnable)
    {
        // it seems we're required not to change anything, so do not change...
        OstTraceFiltStatic0(TRACE_WARNING, "AFM_PROXY: AFM_MpcPcmProbe::applyConfig AFM_AUDIO_CONFIG_PCM_PROBE: no change: early exit", static_cast<TraceObject*>(&mENSComponent));
        return OMX_ErrorNone;
    }

    probe->bEnable = bEnable;
    probe->nProbeIdx = pConfig->nProbeIdx;

    AFM_PcmPort * port = static_cast<AFM_PcmPort *>(mENSComponent.getPort(portIdx));
    AFM_PARAM_PCMLAYOUTTYPE pcmLayout = port->getPcmLayout();

    if (bEnable)
    {
        // Allocate the shared buffer for pcm probe
        // size will be 10 times the natural blocksize
        // and we need a double buffer
        t_cm_memory_handle memHdl;
        dspBufferSize = pcmLayout.nBlockSize * pcmLayout.nChannels * PCM_PROBE_NB_BUFFER * 2;

        //        OMX_U32 bufferSize = dspBufferSize * ((pcmLayout.nBitsPerSample >= 24)?4:2);
        OMX_U32 bufferSize = dspBufferSize * 2;

        error = ENS::allocMpcMemory(mENSComponent.getNMFDomainHandle(),
                                    CM_MM_MPC_SDRAM16,
                                    bufferSize,
                                    CM_MM_ALIGN_WORD,
                                    &memHdl);
        if (error != OMX_ErrorNone) return error;

        t_cm_error cm_error = CM_GetMpcMemoryMpcAddress(memHdl, &dspAddr);
        if (cm_error != CM_OK) return error;

        t_cm_system_address sysAddr;
        cm_error = CM_GetMpcMemorySystemAddress(memHdl, &sysAddr);
        if (cm_error != CM_OK) return error;

        probe->SharedBufMemHdl = memHdl;
        // remember the buffer size (half because this is a double buffer)
        probe->nBufferSize = bufferSize;
        probe->pSharedBufAddr = (void*)sysAddr.logical;

        OstTraceFiltStatic3(TRACE_DEBUG, "AFM_PROXY: AFM_MpcPcmProbe::applyConfig buffers instantiated: dspAddr: 0x%x, dspBufer size: %d, armBufferSize: %d", static_cast<TraceObject*>(&mENSComponent), dspAddr, dspBufferSize, bufferSize);
    }
    // in case of probe disabling, the buffer will be freed
    // omce we're sure the dsp does not actually use it

    OMX_U32 nmfPortIdx = mAFMProcessingComp.getNmfPortIndex(portIdx);
    OMX_U32 i;

    OstTraceFiltStatic4(TRACE_DEBUG, "AFM_PROXY:  AFM_MpcPcmProbe::applyConfig probeid: %d, portId %d, enable: %d, nmfPortIdx: %d", static_cast<TraceObject*>(&mENSComponent), probe->nProbeIdx, portIdx, bEnable, nmfPortIdx);
    OstTraceFiltStatic2(TRACE_DEBUG, "AFM_PROXY:  AFM_MpcPcmProbe::applyConfig dspAddr: 0x%x, dspBufferSize %d", static_cast<TraceObject*>(&mENSComponent), dspAddr, dspBufferSize);

    // check which configure we need to call
    for (i = 0; i< mENSComponent.getPortCount(); i++)
    {
      if ( (mNmfPortHdl[i].nmfHdl == mNmfPortHdl[portIdx].nmfHdl) &&
           mNmfPortHdl[i].bIsBoundToPcmDump ) {
        break;
      }
    }

    if (i == mENSComponent.getPortCount())
    {
        OstTraceFiltStatic1(TRACE_ERROR, "AFM_PROXY:  AFM_MpcPcmProbe::applyConfig ERROR !! Not able to find configure interface bound to host for port %d", static_cast<TraceObject*>(&mENSComponent), portIdx);
        DBC_ASSERT(0);
    }

    // configure the main component!
    PcmDumpConfigure_t sConfigure;
    sConfigure.nmf_port_idx         = nmfPortIdx;
    sConfigure.omx_port_idx         = portIdx;
    sConfigure.effect_position_idx  = 0; /*we configure only probe for OMX ports*/
    sConfigure.enable               = bEnable;
    sConfigure.buffer               = (void*)dspAddr;
    sConfigure.buffer_size          = dspBufferSize;
    mIconfigure[i].configure_pcmprobe(sConfigure);
    return OMX_ErrorNone;
}

OMX_ERRORTYPE AFM_MpcPcmProbe::retrieveConfig(AFM_AUDIO_CONFIG_PCM_PROBE * pConfig)
{
    OMX_U32 portIdx = pConfig->nPortIndex;

    PcmProbe_t * probe = getProbeConfig(portIdx);

    if (probe == NULL) return OMX_ErrorBadParameter;

    pConfig->bEnable = probe->bEnable;
    pConfig->nProbeIdx = probe->nProbeIdx;
    return OMX_ErrorNone;
}


PcmProbe_t * AFM_MpcPcmProbe::getProbeConfig(OMX_U32 portIdx) const
{
    if (portIdx >= mENSComponent.getPortCount())
    {
        OstTraceFiltStatic2(TRACE_ERROR, "AFM_PROXY: AFM_MpcPcmProbe::getProbeConfig ERROR !! portIdx is out of bounds: %d vs %d", static_cast<TraceObject*>(&mENSComponent), portIdx, mENSComponent.getPortCount());
        return NULL;
    }
    else
    {
      return &mConfigPcmProbe[portIdx];
    }
}
