/*****************************************************************************/
/**
*  (c) ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file   TEQ_AudioEffect.cpp
* \brief  Proxy part of transducer equalizer effect included in
* Mixer OMX component
* \author ST-Ericsson
*/
/*****************************************************************************/

#include "TEQ_AudioEffect.h"
#include "OMX_STE_AudioEffectsExt.h"
#include "TEQMpc_AudioEffect.h"
#include "AFM_MemoryPreset.h"
#include "ENS_IOMX.h"
#include "AFM_Types.h"
#include "TEQ_SHAI.h"
#include <host/libeffects/libtransducer_equalizer/include/config.idt.h>

#include "OstTraceDefinitions_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "audio_transducer_equalizer_proxy_TEQ_AudioEffectTraces.h"
#endif
#include "OpenSystemTrace_ste.h"

#define LOG_TAG "TEQ_AudioEffect"
#include "linux_utils.h"

OMX_ERRORTYPE teqCreateMethod(AudioEffect **ppAudioEffect, ENS_Component &enscomp, const AudioEffectParams &sParams)
{
    OMX_ERRORTYPE error  = OMX_ErrorNone;

    TEQ_AudioEffect *pTeqEffect = new TEQ_AudioEffect(sParams, enscomp);
    if (pTeqEffect == 0) {
        LOGE("TEQ_AudioEffect::teqCreateMethod : TEQ_AudioEffect can not be created at position %d on portId %d (OMX_ErrorInsufficientResources)\n", (OMX_U16)sParams.mPosition, (OMX_U16)sParams.mPortIndex);
        return OMX_ErrorInsufficientResources;
    }
    *ppAudioEffect = pTeqEffect;

    TEQMpc_AudioEffect *pTeqMpcEffect = new TEQMpc_AudioEffect(*pTeqEffect, enscomp);
    if (pTeqMpcEffect == 0) {
        LOGE("TEQ_AudioEffect::teqCreateMethod : TEQ_AudioEffect can not be created at position %d on portId %d (OMX_ErrorInsufficientResources)\n", (OMX_U16)sParams.mPosition, (OMX_U16)sParams.mPortIndex);
        delete pTeqEffect;
        *ppAudioEffect = NULL;
        return OMX_ErrorInsufficientResources;
    }

    pTeqEffect->setCoreSpecificPart(pTeqMpcEffect);

    //2 configuration indexes are supported :AFM_IndexParamTransducerEqualizer,AFM_IndexConfigTransducerEqualizer
    OMX_INDEXTYPE configIndexesTable[2];
    configIndexesTable[0]    = (OMX_INDEXTYPE)AFM_IndexParamTransducerEqualizer;
    configIndexesTable[1]    = (OMX_INDEXTYPE)AFM_IndexConfigTransducerEqualizer;
    pTeqEffect->fillConfigIndexesTable(2, configIndexesTable);

    return error;
}

int TEQ_AudioEffect::instance = -1;

TEQ_AudioEffect::TEQ_AudioEffect(const AudioEffectParams &sParams, ENS_Component &enscomp)
    : AudioEffect(sParams, enscomp)
{
    //Default parameters : ONLY PROPRIETARY configuration is supported (AFM_IndexParamTransducerEqualizer,AFM_IndexConfigTransducerEqualizer)

    mTransducerEqualizerParams.nSize                                                      = sizeof(AFM_AUDIO_PARAM_TRANSDUCEREQUALIZERTYPE);
    getOmxIlSpecVersion(&mTransducerEqualizerParams.nVersion);
    mTransducerEqualizerParams.MemoryPreset                                               = MEM_PRESET_ALL_TCM;
    mTransducerEqualizerParams.stereo                                                     = (TREQ_DEFAULT_NB_CHANNEL == 2 ? OMX_TRUE : OMX_FALSE);
    mTransducerEqualizerParams.nb_alloc_biquad_cells_per_channel                          = NB_MAX_TRANSDUCER_EQUALIZER_BIQUAD_CELLS / TREQ_DEFAULT_NB_CHANNEL;
    mTransducerEqualizerParams.nb_alloc_FIR_coefs_per_channel                             = NB_MAX_TRANSDUCER_EQUALIZER_FIR_COEFS    / TREQ_DEFAULT_NB_CHANNEL;

    mTransducerEqualizerConfig.nSize                                                      = sizeof(AFM_AUDIO_CONFIG_TRANSDUCEREQUALIZERTYPE);
    getOmxIlSpecVersion(&mTransducerEqualizerConfig.nVersion);
    mTransducerEqualizerConfig.bEnable                                                    = OMX_TRUE;
    mTransducerEqualizerConfig.same_biquad_l_r                                            = OMX_TRUE;
    mTransducerEqualizerConfig.same_FIR_l_r                                               = OMX_TRUE;
    mTransducerEqualizerConfig.biquad_first                                               = OMX_TRUE;
    mTransducerEqualizerConfig.biquad_gain_exp_l                                          = 1;
    mTransducerEqualizerConfig.biquad_gain_mant_l                                         = 0x40000000;
    mTransducerEqualizerConfig.biquad_gain_exp_r                                          = 1;
    mTransducerEqualizerConfig.biquad_gain_mant_r                                         = 0x40000000;
    mTransducerEqualizerConfig.FIR_gain_exp_l                                             = 1;
    mTransducerEqualizerConfig.FIR_gain_mant_l                                            = 0x40000000;
    mTransducerEqualizerConfig.FIR_gain_exp_r                                             = 1;
    mTransducerEqualizerConfig.FIR_gain_mant_r                                            = 0x40000000;
    mTransducerEqualizerConfig.bAutomaticTransitionGain                                   = OMX_TRUE;
    mTransducerEqualizerConfig.smoothing_delay_ms                                         = 200;
    mTransducerEqualizerConfig.transition_gain_exp_l                                      = 1;
    mTransducerEqualizerConfig.transition_gain_mant_l                                     = 0x40000000;
    mTransducerEqualizerConfig.transition_gain_exp_r                                      = 1;
    mTransducerEqualizerConfig.transition_gain_mant_r                                     = 0x40000000;
    mTransducerEqualizerConfig.nb_biquad_cells_per_channel                                = 0;
    mTransducerEqualizerConfig.nb_FIR_coefs_per_channel                                   = 0;

    for(int i = 0; i < NB_MAX_TRANSDUCER_EQUALIZER_BIQUAD_CELLS; i++)
    {
        mTransducerEqualizerConfig.biquad_cell[i].b_exp                                   = 0;
        mTransducerEqualizerConfig.biquad_cell[i].b0                                      = 0x7FFFFFFF;
        mTransducerEqualizerConfig.biquad_cell[i].b1                                      = 0;
        mTransducerEqualizerConfig.biquad_cell[i].b2                                      = 0;
        mTransducerEqualizerConfig.biquad_cell[i].a1                                      = 0;
        mTransducerEqualizerConfig.biquad_cell[i].a2                                      = 0;
    }

    mTransducerEqualizerConfig.FIR_coef[0]                                                = 0x7FFFFFFF;
    mTransducerEqualizerConfig.FIR_coef[1]                                                = 0x7FFFFFFF;
    for(int i = 2; i < NB_MAX_TRANSDUCER_EQUALIZER_FIR_COEFS; i++)
    {
        mTransducerEqualizerConfig.FIR_coef[i]                                            = 0;
    }

    instance ++;
#if DEBUG_TEQ_SHAI_CONFIG_LEVEL > 0
    open_debug_TEQ_SHAI_file(instance);
#endif // DEBUG_TEQ_SHAI_CONFIG_LEVEL > 0
}

TEQ_AudioEffect::~TEQ_AudioEffect(void) {
#if DEBUG_TEQ_SHAI_CONFIG_LEVEL > 0
    close_debug_TEQ_SHAI_file(instance);
#endif // DEBUG_TEQ_SHAI_CONFIG_LEVEL > 0
    LOGD("close TEQ instance : instance=%d\n", instance);
    instance--;
}

OMX_ERRORTYPE TEQ_AudioEffect::setConfig(OMX_U32 nConfigIndex, OMX_PTR pComponentConfigStructure) {
    switch(nConfigIndex){
        case ((OMX_U32)AFM_IndexParamTransducerEqualizer):
            {
                AFM_AUDIO_PARAM_TRANSDUCEREQUALIZERTYPE *pConfig = (AFM_AUDIO_PARAM_TRANSDUCEREQUALIZERTYPE *) pComponentConfigStructure;

                OMX_U32 nb_channels = (pConfig->stereo != 0 ? 2 : 1);
                if((nb_channels * pConfig->nb_alloc_biquad_cells_per_channel > NB_MAX_TRANSDUCER_EQUALIZER_BIQUAD_CELLS)
                        || (nb_channels * pConfig->nb_alloc_FIR_coefs_per_channel > NB_MAX_TRANSDUCER_EQUALIZER_FIR_COEFS))
                {
                    OstTraceFiltInst4(TRACE_ERROR,"TEQ_AudioEffect::setConfig (AFM_AUDIO_PARAM_TRANSDUCEREQUALIZERTYPE) nPosition=%d, nb_channels=%d, nb_alloc_biquad_cells_per_channel=%d, nb_alloc_FIR_coefs_per_channel=%d", getPosition(), nb_channels, pConfig->nb_alloc_biquad_cells_per_channel, pConfig->nb_alloc_FIR_coefs_per_channel);
                    return OMX_ErrorBadParameter;
                }
                mTransducerEqualizerParams = *pConfig;

                OstTraceFiltInst4(TRACE_DEBUG,"TEQ_AudioEffect::setConfig (AFM_AUDIO_PARAM_TRANSDUCEREQUALIZERTYPE) nPosition=%d nb_channels=%d, nb_alloc_biquad_cells_per_channel=%d, nb_alloc_FIR_coefs_per_channel=%d", getPosition(), nb_channels, pConfig->nb_alloc_biquad_cells_per_channel, pConfig->nb_alloc_FIR_coefs_per_channel);

                return OMX_ErrorNone;
            }

        case ((OMX_U32)AFM_IndexConfigTransducerEqualizer):
            {
                AFM_AUDIO_CONFIG_TRANSDUCEREQUALIZERTYPE *pConfig = (AFM_AUDIO_CONFIG_TRANSDUCEREQUALIZERTYPE *) pComponentConfigStructure;

                if((pConfig->nb_FIR_coefs_per_channel > mTransducerEqualizerParams.nb_alloc_FIR_coefs_per_channel)
                        || (pConfig->nb_biquad_cells_per_channel > mTransducerEqualizerParams.nb_alloc_biquad_cells_per_channel))
                {
                    OstTraceFiltInst3(TRACE_ERROR,"TEQ_AudioEffect::setConfig (AFM_AUDIO_CONFIG_TRANSDUCEREQUALIZERTYPE) nPosition=%d, nb_FIR_coefs_per_channel=%d, nb_biquad_cells_per_channel=%d", getPosition(), pConfig->nb_FIR_coefs_per_channel, pConfig->nb_biquad_cells_per_channel);
                    return OMX_ErrorBadParameter;
                }

                mTransducerEqualizerConfig = *pConfig;

                OstTraceFiltInst3(TRACE_DEBUG,"TEQ_AudioEffect::setConfig (AFM_AUDIO_CONFIG_TRANSDUCEREQUALIZERTYPE) nPosition=%d, nb_FIR_coefs_per_channel=%d, nb_biquad_cells_per_channel=%d", getPosition(), pConfig->nb_FIR_coefs_per_channel, pConfig->nb_biquad_cells_per_channel);

                return OMX_ErrorNone;
            }

        default:
            return AudioEffect::setConfig(nConfigIndex, pComponentConfigStructure);
    }
}

OMX_ERRORTYPE TEQ_AudioEffect::getConfig(OMX_U32 nConfigIndex, OMX_PTR pComponentConfigStructure) const
{
    switch(nConfigIndex)
    {
        case ((OMX_U32)AFM_IndexParamTransducerEqualizer):
            {
                AFM_AUDIO_PARAM_TRANSDUCEREQUALIZERTYPE *pConfig = (AFM_AUDIO_PARAM_TRANSDUCEREQUALIZERTYPE *) pComponentConfigStructure;

                *pConfig = mTransducerEqualizerParams;

                return OMX_ErrorNone;
            }

        case ((OMX_U32)AFM_IndexConfigTransducerEqualizer):
        {
            AFM_AUDIO_CONFIG_TRANSDUCEREQUALIZERTYPE *pConfig = (AFM_AUDIO_CONFIG_TRANSDUCEREQUALIZERTYPE *) pComponentConfigStructure;

            *pConfig = mTransducerEqualizerConfig;

            return OMX_ErrorNone;
        }

        default:
            return AudioEffect::getConfig(nConfigIndex, pComponentConfigStructure);
    }
}


