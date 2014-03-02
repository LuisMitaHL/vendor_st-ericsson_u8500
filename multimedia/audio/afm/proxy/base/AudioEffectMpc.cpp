/*****************************************************************************/
/**
*  (c) ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file   SPLMpc_AudioEffect.cpp
* \brief  Specific DSP part of spl effect included in
* OMX Mixer/Splitter components
* \author ST-Ericsson
*/
/*****************************************************************************/
#include "AudioEffectMpc.h"
#include "AFM_Index.h"
#include "AFM_Types.h"
#include "AFM_PcmPort.h"
#include "AFM_MpcPcmProbe.h"
#include "AudioEffectMpcUtils.h"
#include "libeffects/include/effects_dsp_index.h"

#include "OstTraceDefinitions_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "afm_proxy_base_AudioEffectMpcTraces.h"
#endif
#include "OpenSystemTrace_ste.h"

AudioEffectMpc::AudioEffectMpc(
        AudioEffect &audioEffect,
        ENS_Component &enscomp)
    : AudioEffectCoreSpecific(audioEffect, enscomp)
{
    mDspConfigPcmProbe.bEnable = OMX_FALSE;
    setTraceInfo(enscomp.getSharedTraceInfoPtr(),audioEffect.getPortIdx());
}

AudioEffectMpc::~AudioEffectMpc(void) {
    //Check if we need to free shared memory allocated for PcmProbe
    if (mDspConfigPcmProbe.bEnable) {
        DBC_ASSERT(ENS::freeMpcMemory(mDspConfigPcmProbe.SharedBufMemHdl) == OMX_ErrorNone);
    }
}

OMX_ERRORTYPE AudioEffectMpc::allocAndFillDspConfigStruct(OMX_U32 nConfigIndex, ARMSpecificEffectConfig_t *pEffectConfig) {

    switch (nConfigIndex) {

        case ((OMX_U32)AFM_IndexConfigPcmProbe):
            {
                OMX_U32 probeBufferDspAddr = 0;
                OMX_U32 probeDspBufferSize = 0;

                mDspConfigPcmProbe.bEnable      = mAudioEffect.getProbeConfig()->bEnable;
                mDspConfigPcmProbe.nProbeIdx    = mAudioEffect.getProbeConfig()->nProbeIdx;

                if(mAudioEffect.getProbeConfig()->bEnable) {
                    // Allocate the shared buffer for pcm probe
                    // size will be 10 times the natural blocksize
                    // and we need a double buffer
                    t_cm_memory_handle memHdl;
                    AFM_PcmPort * port = static_cast<AFM_PcmPort *>(mENSComponent.getPort(mAudioEffect.getPortIdx()));
                    AFM_PARAM_PCMLAYOUTTYPE pcmLayout = port->getPcmLayout();

                    probeDspBufferSize = pcmLayout.nBlockSize * pcmLayout.nChannels * PCM_PROBE_NB_BUFFER * 2;

                    OMX_U32 bufferSize = probeDspBufferSize * 2;

                    OMX_ERRORTYPE error = ENS::allocMpcMemory(mENSComponent.getNMFDomainHandle(),
                            CM_MM_MPC_SDRAM16,
                            bufferSize,
                            CM_MM_ALIGN_WORD,
                            &memHdl);
                    if (error != OMX_ErrorNone) return error;

                    t_cm_error cm_error = CM_GetMpcMemoryMpcAddress(memHdl, &probeBufferDspAddr);
                    if (cm_error != CM_OK) return error;

                    t_cm_system_address sysAddr;
                    cm_error = CM_GetMpcMemorySystemAddress(memHdl, &sysAddr);
                    if (cm_error != CM_OK) return error;

                    mDspConfigPcmProbe.SharedBufMemHdl = memHdl;
                    // remember the buffer size (half because this is a double buffer)
                    mDspConfigPcmProbe.nBufferSize = bufferSize;
                    mDspConfigPcmProbe.pSharedBufAddr = (void*)sysAddr.logical;

                    OstTraceFiltInst3(TRACE_DEBUG, "AudioEffectMpc::allocAndFillDspConfigStruct (AFM_IndexConfigPcmProbe) probeBufferDspAddr=0x%x, dspBufer size=%d, armBufferSize=%d", probeBufferDspAddr, probeDspBufferSize, bufferSize);
                }
                // in case of probe disabling, the buffer will be freed
                // omce we're sure the dsp does not actually use it

                OMX_U32 configStructArmAddress = 0, configStructDspAddress = 0;

                t_cm_memory_handle configStructHandle = AudioEffectMpcUtils::allocDSPMemoryForSetConfig(mENSComponent.getNMFDomainHandle(), sizeof(ARMConfigPcmProbe_t), &configStructArmAddress, &configStructDspAddress);
                if(!configStructHandle) return OMX_ErrorInsufficientResources;

                pEffectConfig->portIdx                              = mAudioEffect.getPortIdx();
                pEffectConfig->effectPosition                       = mAudioEffect.getPosition();
                pEffectConfig->configIndex                          = AFM_DSP_IndexConfigPcmProbe;
                pEffectConfig->isStaticConfiguration                = false;
                pEffectConfig->pSpecificConfigStruct                = configStructDspAddress;
                pEffectConfig->specificConfigStructHandle_high      = configStructHandle >> 24;
                pEffectConfig->specificConfigStructHandle_low       = configStructHandle & 0xffffff;

                ARMConfigPcmProbe_t *pConfigMpc = (ARMConfigPcmProbe_t *) configStructArmAddress;
                pConfigMpc->bEnable                     = mAudioEffect.getProbeConfig()->bEnable;
                pConfigMpc->buffer                      = probeBufferDspAddr;
                pConfigMpc->buffer_size                 = probeDspBufferSize;

                OstTraceFiltInst3(TRACE_DEBUG,"SPLMpc_AudioEffect::allocAndFillDspConfigStruct AFM_IndexConfigPcmProbe position=%d bEnable=%d nProbeIdx=%d", mAudioEffect.getPosition(), mAudioEffect.getProbeConfig()->bEnable, mAudioEffect.getProbeConfig()->nProbeIdx);

                return OMX_ErrorNone;
            }

        default:
            return OMX_ErrorUnsupportedIndex;
    }
}

