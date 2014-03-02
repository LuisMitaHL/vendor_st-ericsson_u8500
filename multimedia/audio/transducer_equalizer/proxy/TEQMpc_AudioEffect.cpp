
/**
*  (c) ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file   TEQMpc_AudioEffect.cpp
* \brief  Specific DSP part of teq effect included in
* Mixer OMX component
* \author ST-Ericsson
*/
/*****************************************************************************/
#include "TEQMpc_AudioEffect.h"
#include "TEQ_AudioEffect.h"
#include "OMX_STE_AudioEffectsExt.h"
#include "AudioEffectMpcUtils.h"
#include "ENS_Nmf.h"
#include "AFM_MemoryPreset.h"
#include "libeffects/include/effects_dsp_index.h"
#include "TEQ_SHAI.h"
#include "TEQMpcConfigs.h"

#include "OstTraceDefinitions_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "audio_transducer_equalizer_proxy_TEQMpc_AudioEffectTraces.h"
#endif
#include "OpenSystemTrace_ste.h"

#define MAX_RAM_X (NB_MAX_TRANSDUCER_EQUALIZER_BIQUAD_CELLS * 6 + NB_MAX_TRANSDUCER_EQUALIZER_FIR_COEFS)
#define MAX_RAM_Y (4 * (NB_MAX_TRANSDUCER_EQUALIZER_BIQUAD_CELLS / 2 + 1) + NB_MAX_TRANSDUCER_EQUALIZER_FIR_COEFS)

//TransducerEqualizer Memory need
static t_afm_mempreset memory_presets[]=
{
    //                                        TCM        YTCM       DDR24 DDR16       ESR24 ESR16
    { MEM_PRESET_ALL_TCM,            {{ MAX_RAM_X,  MAX_RAM_Y,          0,    0,          0,    0 }} },
    { MEM_PRESET_ALL_ESRAM,          {{         0,  MAX_RAM_Y,          0,    0,  MAX_RAM_X,    0 }} },
    { MEM_PRESET_ALL_DDR,            {{         0,  MAX_RAM_Y,  MAX_RAM_X,    0,          0,    0 }} },
    { MEM_PRESET_MIX_DDR_TCM_1,      {{ MAX_RAM_X,  MAX_RAM_Y,          0,    0,          0,    0 }} },
    { MEM_PRESET_MIX_ESRAM_DDR,      {{         0,  MAX_RAM_Y,          0,    0,  MAX_RAM_X,    0 }} },
    { MEM_PRESET_MIX_ESRAM_TCM,      {{ MAX_RAM_X,  MAX_RAM_Y,          0,    0,          0,    0 }} }
};

OMX_ERRORTYPE TEQMpc_AudioEffect::allocAndFillDspConfigStruct(OMX_U32 nConfigIndex, ARMSpecificEffectConfig_t *pEffectConfig) {

    switch (nConfigIndex) {

        case ((OMX_U32)AFM_IndexParamTransducerEqualizer):
            {
                AFM_AUDIO_PARAM_TRANSDUCEREQUALIZERTYPE *pTeqParams = ((TEQ_AudioEffect*)&mAudioEffect)->getTeqParams();
                OMX_U32 configStructArmAddress = 0, configStructDspAddress = 0;

                OMX_U32 nb_channels = (pTeqParams->stereo != 0 ? 2 : 1);
                if(nb_channels != mAudioEffect.getNbChannels()) {
                    OstTraceFiltInst3(TRACE_ERROR,"TEQMpc_AudioEffect::allocAndFillDspConfigStruct (AFM_AUDIO_PARAM_TRANSDUCEREQUALIZERTYPE) nPosition=%d, stereo=%d whereas nbChannels of dataflow is %d ", mAudioEffect.getPosition(), pTeqParams->stereo, mAudioEffect.getNbChannels());
                    return OMX_ErrorBadParameter;
                }

                t_cm_memory_handle configStructHandle = AudioEffectMpcUtils::allocDSPMemoryForSetConfig(mENSComponent.getNMFDomainHandle(), sizeof(ARMParamTeq_t), &configStructArmAddress, &configStructDspAddress);
                if(!configStructHandle) return OMX_ErrorInsufficientResources;

                pEffectConfig->portIdx                              = mAudioEffect.getPortIdx();
                pEffectConfig->effectPosition                       = mAudioEffect.getPosition();
                pEffectConfig->configIndex                          = AFM_DSP_IndexParamTeq;
                pEffectConfig->isStaticConfiguration                = true;
                pEffectConfig->pSpecificConfigStruct                = configStructDspAddress;
                pEffectConfig->specificConfigStructHandle_high      = configStructHandle >> 24;
                pEffectConfig->specificConfigStructHandle_low       = configStructHandle & 0xffffff;

                ARMParamTeq_t *pConfigMpc = (ARMParamTeq_t *) configStructArmAddress;
                pConfigMpc->iMemoryPreset                           = memory_presets[mENSComponent.getMemoryPreset()].mempreset;
                pConfigMpc->istereo                                 = pTeqParams->stereo;
                pConfigMpc->inb_alloc_biquad_cells_per_channel      = pTeqParams->nb_alloc_biquad_cells_per_channel;
                pConfigMpc->inb_alloc_FIR_coefs_per_channel         = pTeqParams->nb_alloc_FIR_coefs_per_channel;
                pConfigMpc->ialert_on                               = false;

                OstTraceFiltInst4(TRACE_DEBUG,"TEQMpc_AudioEffect::allocAndFillDspConfigStruct AFM_IndexParamTransducerEqualizer position=%d stereo=%d nb_alloc_biquad_cells_per_channel=%d nb_alloc_FIR_coefs_per_channel=%d", mAudioEffect.getPosition(), pTeqParams->stereo, pTeqParams->nb_alloc_biquad_cells_per_channel, pTeqParams->nb_alloc_FIR_coefs_per_channel);

                return OMX_ErrorNone;
            }

        case ((OMX_U32)AFM_IndexConfigTransducerEqualizer):
            {
                OMX_U32 configStructArmAddress = 0, configStructDspAddress = 0;

                t_cm_memory_handle configStructHandle = AudioEffectMpcUtils::allocDSPMemoryForSetConfig(mENSComponent.getNMFDomainHandle(), sizeof(ARMTransducerEqualizerConfig_t), &configStructArmAddress, &configStructDspAddress);
                if(!configStructHandle) return OMX_ErrorInsufficientResources;

                pEffectConfig->portIdx                              = mAudioEffect.getPortIdx();
                pEffectConfig->effectPosition                       = mAudioEffect.getPosition();
                pEffectConfig->configIndex                          = AFM_DSP_IndexConfigTeq;
                pEffectConfig->isStaticConfiguration                = false;
                pEffectConfig->pSpecificConfigStruct                = configStructDspAddress;
                pEffectConfig->specificConfigStructHandle_high      = configStructHandle >> 24;
                pEffectConfig->specificConfigStructHandle_low       = configStructHandle & 0xffffff;

                OstTraceFiltInst1(TRACE_DEBUG,"TEQMpc_AudioEffect::allocAndFillDspConfigStruct AFM_IndexConfigTransducerEqualizer position=%d", mAudioEffect.getPosition());

                return fillDSPTEQConfig((ARMTransducerEqualizerConfig_t *) configStructArmAddress);
            }

        default:
            return AudioEffectMpc::allocAndFillDspConfigStruct(nConfigIndex, pEffectConfig);
    }
}

OMX_ERRORTYPE TEQMpc_AudioEffect::fillDSPTEQConfig(ARMTransducerEqualizerConfig_t *pConfigMpc) {
    AFM_AUDIO_CONFIG_TRANSDUCEREQUALIZERTYPE *pTeqConfig = ((TEQ_AudioEffect*)&mAudioEffect)->getTeqConfig();
    AFM_AUDIO_PARAM_TRANSDUCEREQUALIZERTYPE *pTeqParams = ((TEQ_AudioEffect*)&mAudioEffect)->getTeqParams();
    bool stereo = (mAudioEffect.getNbChannels() > 1);
    int i, nb_channels, nb_channels_biquad, nb_channels_FIR, smoothing_delay, FIR_smoothing_delay_granularity;
    t_float module_avg_l, module_avg_r, module_avg_biquad_l, module_avg_biquad_r, module_avg_FIR_l, module_avg_FIR_r;
    t_float *p_module2;
    int average_gain_mant_l, average_gain_mant_r, average_gain_exp_l, average_gain_exp_r;
    int instance = ((TEQ_AudioEffect*)&mAudioEffect)->getInstance();

#if DEBUG_TEQ_SHAI_CONFIG_LEVEL > 4
    fprintf(debug_TEQ_SHAI_file[instance], "TransducerEqualizer::getEffectConfig_IndexConfigTransducerEqualizer\n");
#endif // DEBUG_TEQ_SHAI_CONFIG_LEVEL > 4

    if(stereo)
    {
        nb_channels = nb_channels_biquad = nb_channels_FIR = 2;
        if(pTeqConfig->same_biquad_l_r)
        {
            nb_channels_biquad = 1;
        }
        if(pTeqConfig->same_FIR_l_r)
        {
            nb_channels_FIR = 1;
        }
    }
    else
    {
        nb_channels = nb_channels_biquad = nb_channels_FIR = 1;
    }

    if(pTeqConfig->bAutomaticTransitionGain)
    {
        if(pTeqConfig->nb_biquad_cells_per_channel > 0)
        {
            p_module2 = NULL;
            for(i = 0; i < pTeqConfig->nb_biquad_cells_per_channel; i++)
            {
#if DEBUG_TEQ_SHAI_CONFIG_LEVEL > 5
                fprintf(debug_TEQ_SHAI_file[instance], "compute module_avg_biquad left cell %d\n", i);
#endif // DEBUG_TEQ_SHAI_CONFIG_LEVEL > 5
                module_avg_biquad(false,
                        pTeqConfig->biquad_cell[nb_channels_biquad * i].b_exp,
                        pTeqConfig->biquad_cell[nb_channels_biquad * i].b0,
                        pTeqConfig->biquad_cell[nb_channels_biquad * i].b1,
                        pTeqConfig->biquad_cell[nb_channels_biquad * i].b2,
                        pTeqConfig->biquad_cell[nb_channels_biquad * i].a1,
                        pTeqConfig->biquad_cell[nb_channels_biquad * i].a2,
                        0,
                        &p_module2,
                        instance);
                if(p_module2 == NULL)
                {
                    return OMX_ErrorInsufficientResources;
                }
            }
            module_avg_biquad_l = module_avg_biquad_r = module_avg_biquad(true, 0, 0, 0, 0, 0, 0, mAudioEffect.getSamplingRate(), &p_module2, instance);
        }
        else
        {
            module_avg_biquad_l = module_avg_biquad_r = 1.0;
        }
        module_avg_FIR_l = module_avg_FIR_r = module_avg_FIR(pTeqConfig->nb_FIR_coefs_per_channel, nb_channels_FIR, pTeqConfig->FIR_coef, mAudioEffect.getSamplingRate());
        module_avg_l = module_avg_biquad_l * module_avg_FIR_l * ((t_float) pTeqConfig->biquad_gain_mant_l) * ((t_float) pTeqConfig->FIR_gain_mant_l) * pow(2.0, pTeqConfig->biquad_gain_exp_l + pTeqConfig->FIR_gain_exp_l - 62);
#if DEBUG_TEQ_SHAI_CONFIG_LEVEL > 4
        fprintf(debug_TEQ_SHAI_file[instance], "module_avg_biquad left  = %1.8f\n", module_avg_biquad_l);
        fprintf(debug_TEQ_SHAI_file[instance], "module_avg_FIR    left  = %1.8f\n", module_avg_FIR_l);
        fprintf(debug_TEQ_SHAI_file[instance], "module_avg        left  = %1.8f\n", module_avg_l);
#endif // DEBUG_TEQ_SHAI_CONFIG_LEVEL > 4
        module_avg_l = (module_avg_l < 0.0 ? -module_avg_l : module_avg_l) * 2147483648.0;
        if(module_avg_l == 0.0)
        {
            average_gain_mant_l = 0;
            average_gain_exp_l  = 0;
        }
        else
        {
            average_gain_exp_l = 0;
            while(module_avg_l > 2147483647.0)
            {
                average_gain_exp_l++;
                module_avg_l *= 0.5;
            }
            while(module_avg_l < 1073741823.0)
            {
                average_gain_exp_l--;
                module_avg_l *= 2.0;
            }
            average_gain_mant_l = (int) floor(module_avg_l);
        }
        if(stereo)
        {
            int biquad_gain_exp_r, biquad_gain_mant_r, FIR_gain_exp_r, FIR_gain_mant_r;

            if(pTeqConfig->same_biquad_l_r)
            {
                biquad_gain_exp_r  = pTeqConfig->biquad_gain_exp_l;
                biquad_gain_mant_r = pTeqConfig->biquad_gain_mant_l;
            }
            else
            {
                if(pTeqConfig->nb_biquad_cells_per_channel > 0)
                {
                    p_module2 = NULL;
                    for(i = 0; i < pTeqConfig->nb_biquad_cells_per_channel; i++)
                    {
#if DEBUG_TEQ_SHAI_CONFIG_LEVEL > 5
                        fprintf(debug_TEQ_SHAI_file[instance], "compute module_avg_biquad right cell %d\n", i);
#endif // DEBUG_TEQ_SHAI_CONFIG_LEVEL > 5
                        module_avg_biquad(false,
                                pTeqConfig->biquad_cell[nb_channels_biquad * i + 1].b_exp,
                                pTeqConfig->biquad_cell[nb_channels_biquad * i + 1].b0,
                                pTeqConfig->biquad_cell[nb_channels_biquad * i + 1].b1,
                                pTeqConfig->biquad_cell[nb_channels_biquad * i + 1].b2,
                                pTeqConfig->biquad_cell[nb_channels_biquad * i + 1].a1,
                                pTeqConfig->biquad_cell[nb_channels_biquad * i + 1].a2,
                                0,
                                &p_module2,
                                instance);
                        if(p_module2 == NULL)
                        {
                            return OMX_ErrorInsufficientResources;
                        }
                    }
                    module_avg_biquad_r = module_avg_biquad(true, 0, 0, 0, 0, 0, 0, mAudioEffect.getSamplingRate(), &p_module2, instance);
                }
                else
                {
                    module_avg_biquad_r = 1.0;
                }
                biquad_gain_exp_r  = pTeqConfig->biquad_gain_exp_r;
                biquad_gain_mant_r = pTeqConfig->biquad_gain_mant_r;
            }
            if(pTeqConfig->same_FIR_l_r)
            {
                FIR_gain_exp_r   = pTeqConfig->FIR_gain_exp_l;
                FIR_gain_mant_r  = pTeqConfig->FIR_gain_mant_l;
            }
            else
            {
                module_avg_FIR_r = module_avg_FIR(pTeqConfig->nb_FIR_coefs_per_channel, nb_channels_FIR, &pTeqConfig->FIR_coef[1], mAudioEffect.getSamplingRate());
                FIR_gain_exp_r   = pTeqConfig->FIR_gain_exp_r;
                FIR_gain_mant_r  = pTeqConfig->FIR_gain_mant_r;
            }
            module_avg_r = module_avg_biquad_r * module_avg_FIR_r * ((t_float) biquad_gain_mant_r) * ((t_float) FIR_gain_mant_r) * pow(2.0, biquad_gain_exp_r + FIR_gain_exp_r - 62);
#if DEBUG_TEQ_SHAI_CONFIG_LEVEL > 4
            fprintf(debug_TEQ_SHAI_file[instance], "module_avg_biquad right = %1.8f\n", module_avg_biquad_r);
            fprintf(debug_TEQ_SHAI_file[instance], "module_avg_FIR    right = %1.8f\n", module_avg_FIR_r);
            fprintf(debug_TEQ_SHAI_file[instance], "module_avg        right = %1.8f\n", module_avg_r);
#endif // DEBUG_TEQ_SHAI_CONFIG_LEVEL > 4

            module_avg_r = (module_avg_r < 0.0 ? -module_avg_r : module_avg_r) * 2147483648.0;
            if(module_avg_r == 0.0)
            {
                average_gain_mant_r = 0;
                average_gain_exp_r  = 0;
            }
            else
            {
                average_gain_exp_r = 0;
                while(module_avg_r > 2147483647.0)
                {
                    average_gain_exp_r++;
                    module_avg_r *= 0.5;
                }
                while(module_avg_r < 1073741823.0)
                {
                    average_gain_exp_r--;
                    module_avg_r *= 2.0;
                }
                average_gain_mant_r = (int) floor(module_avg_r);
            }
        }
        else
        {
            average_gain_mant_r = average_gain_mant_l;
            average_gain_exp_r  = average_gain_exp_l;
        }
    }
    else
    {
        average_gain_exp_l  = pTeqConfig->transition_gain_exp_l;
        average_gain_mant_l = pTeqConfig->transition_gain_mant_l;
        average_gain_exp_r  = pTeqConfig->transition_gain_exp_r;
        average_gain_mant_r = pTeqConfig->transition_gain_mant_r;
    }

    if(mAudioEffect.getSamplingRate() > 0)
    {
        FIR_smoothing_delay_granularity = mAudioEffect.getSamplingRate() / 200;  // 5 ms
        smoothing_delay                 = (mAudioEffect.getSamplingRate() * pTeqConfig->smoothing_delay_ms) / 1000;
    }
    else
    {
        FIR_smoothing_delay_granularity = 0;
        smoothing_delay                 = 0;
    }

    pConfigMpc->iEnable                           = (t_uint16) (pTeqConfig->bEnable != 0);
    pConfigMpc->iautomatic_transition_gain        = (t_uint16) (pTeqConfig->bAutomaticTransitionGain != 0);
    pConfigMpc->iaverage_gain_exp_l               = (t_uint16)  average_gain_exp_l;
    pConfigMpc->iaverage_gain_mant_l_low          = (t_uint16)  average_gain_mant_l;
    pConfigMpc->iaverage_gain_mant_l_high         = (t_uint16) (average_gain_mant_l >> 16);
    pConfigMpc->iaverage_gain_exp_r               = (t_uint16)  average_gain_exp_r;
    pConfigMpc->iaverage_gain_mant_r_low          = (t_uint16)  average_gain_mant_r;
    pConfigMpc->iaverage_gain_mant_r_high         = (t_uint16) (average_gain_mant_r >> 16);
    pConfigMpc->itransition_gain_exp_l            = (t_uint16)  pTeqConfig->transition_gain_exp_l;
    pConfigMpc->itransition_gain_mant_l_low       = (t_uint16)  pTeqConfig->transition_gain_mant_l;
    pConfigMpc->itransition_gain_mant_l_high      = (t_uint16) (pTeqConfig->transition_gain_mant_l >> 16);
    pConfigMpc->itransition_gain_exp_r            = (t_uint16)  pTeqConfig->transition_gain_exp_r;
    pConfigMpc->itransition_gain_mant_r_low       = (t_uint16)  pTeqConfig->transition_gain_mant_r;
    pConfigMpc->itransition_gain_mant_r_high      = (t_uint16) (pTeqConfig->transition_gain_mant_r >> 16);
    pConfigMpc->ismoothing_delay_low              = (t_uint16)  smoothing_delay;
    pConfigMpc->ismoothing_delay_high             = (t_uint16) (smoothing_delay >> 16);
    pConfigMpc->iFIR_smoothing_delay_granularity  = (t_uint16)  FIR_smoothing_delay_granularity;
    pConfigMpc->isame_biquad_l_r                  = (t_uint16) (pTeqConfig->same_biquad_l_r != 0);
    pConfigMpc->isame_FIR_l_r                     = (t_uint16) (pTeqConfig->same_FIR_l_r    != 0);
    pConfigMpc->ibiquad_first                     = (t_uint16) (pTeqConfig->biquad_first    != 0);
    pConfigMpc->ibiquad_gain_exp_l                = (t_uint16)  pTeqConfig->biquad_gain_exp_l;
    pConfigMpc->ibiquad_gain_mant_l_low           = (t_uint16)  pTeqConfig->biquad_gain_mant_l;
    pConfigMpc->ibiquad_gain_mant_l_high          = (t_uint16) (pTeqConfig->biquad_gain_mant_l >> 16);
    pConfigMpc->ibiquad_gain_exp_r                = (t_uint16)  pTeqConfig->biquad_gain_exp_r;
    pConfigMpc->ibiquad_gain_mant_r_low           = (t_uint16)  pTeqConfig->biquad_gain_mant_r;
    pConfigMpc->ibiquad_gain_mant_r_high          = (t_uint16) (pTeqConfig->biquad_gain_mant_r >> 16);
    pConfigMpc->iFIR_gain_exp_l                   = (t_uint16)  pTeqConfig->FIR_gain_exp_l;
    pConfigMpc->iFIR_gain_mant_l_low              = (t_uint16)  pTeqConfig->FIR_gain_mant_l;
    pConfigMpc->iFIR_gain_mant_l_high             = (t_uint16) (pTeqConfig->FIR_gain_mant_l >> 16);
    pConfigMpc->iFIR_gain_exp_r                   = (t_uint16)  pTeqConfig->FIR_gain_exp_r;
    pConfigMpc->iFIR_gain_mant_r_low              = (t_uint16)  pTeqConfig->FIR_gain_mant_r;
    pConfigMpc->iFIR_gain_mant_r_high             = (t_uint16) (pTeqConfig->FIR_gain_mant_r >> 16);
    pConfigMpc->inb_biquad_cells_per_channel      = (t_uint16)  pTeqConfig->nb_biquad_cells_per_channel;
    pConfigMpc->inb_FIR_coefs_per_channel         = (t_uint16)  pTeqConfig->nb_FIR_coefs_per_channel;

    for(i = 0; i < NB_MAX_TRANSDUCER_EQUALIZER_BIQUAD_CELLS; i++)
    {
        pConfigMpc->ibiquad_cells[i].b_exp   = (t_uint16) 0;
        pConfigMpc->ibiquad_cells[i].b0_low  = (t_uint16) 0x7FFF;
        pConfigMpc->ibiquad_cells[i].b0_high = (t_uint16) 0xFFFF;
        pConfigMpc->ibiquad_cells[i].b1_low  = (t_uint16) 0;
        pConfigMpc->ibiquad_cells[i].b1_high = (t_uint16) 0;
        pConfigMpc->ibiquad_cells[i].b2_low  = (t_uint16) 0;
        pConfigMpc->ibiquad_cells[i].b2_high = (t_uint16) 0;
        pConfigMpc->ibiquad_cells[i].a1_low  = (t_uint16) 0;
        pConfigMpc->ibiquad_cells[i].a1_high = (t_uint16) 0;
        pConfigMpc->ibiquad_cells[i].a2_low  = (t_uint16) 0;
        pConfigMpc->ibiquad_cells[i].a2_high = (t_uint16) 0;
    }

    for(i = 0; i < nb_channels * pTeqParams->nb_alloc_biquad_cells_per_channel; i++)
    {
        pConfigMpc->ibiquad_cells[i].b_exp   = (t_uint16)  pTeqConfig->biquad_cell[i].b_exp;
        pConfigMpc->ibiquad_cells[i].b0_low  = (t_uint16)  pTeqConfig->biquad_cell[i].b0;
        pConfigMpc->ibiquad_cells[i].b0_high = (t_uint16) (pTeqConfig->biquad_cell[i].b0 >> 16);
        pConfigMpc->ibiquad_cells[i].b1_low  = (t_uint16)  pTeqConfig->biquad_cell[i].b1;
        pConfigMpc->ibiquad_cells[i].b1_high = (t_uint16) (pTeqConfig->biquad_cell[i].b1 >> 16);
        pConfigMpc->ibiquad_cells[i].b2_low  = (t_uint16)  pTeqConfig->biquad_cell[i].b2;
        pConfigMpc->ibiquad_cells[i].b2_high = (t_uint16) (pTeqConfig->biquad_cell[i].b2 >> 16);
        pConfigMpc->ibiquad_cells[i].a1_low  = (t_uint16)  pTeqConfig->biquad_cell[i].a1;
        pConfigMpc->ibiquad_cells[i].a1_high = (t_uint16) (pTeqConfig->biquad_cell[i].a1 >> 16);
        pConfigMpc->ibiquad_cells[i].a2_low  = (t_uint16)  pTeqConfig->biquad_cell[i].a2;
        pConfigMpc->ibiquad_cells[i].a2_high = (t_uint16) (pTeqConfig->biquad_cell[i].a2 >> 16);
    }

    for(i = 0; i < nb_channels_FIR; i++)
    {
        pConfigMpc->iFIR_coefs[i].coef_low  = (t_uint16) 0x7FFF;
        pConfigMpc->iFIR_coefs[i].coef_high = (t_uint16) 0xFFFF;
    }

    for(; i < NB_MAX_TRANSDUCER_EQUALIZER_FIR_COEFS; i++)
    {
        pConfigMpc->iFIR_coefs[i].coef_low  = (t_uint16) 0;
        pConfigMpc->iFIR_coefs[i].coef_high = (t_uint16) 0;
    }

    for(i = 0; i < nb_channels * pTeqParams->nb_alloc_FIR_coefs_per_channel; i++)
    {
        pConfigMpc->iFIR_coefs[i].coef_low  = (t_uint16)  pTeqConfig->FIR_coef[i];
        pConfigMpc->iFIR_coefs[i].coef_high = (t_uint16) (pTeqConfig->FIR_coef[i] >> 16);
    }

#if DEBUG_TEQ_SHAI_CONFIG_LEVEL > 1
    fprintf(debug_TEQ_SHAI_file[instance], "TransducerEqualizer config :\n");
    fprintf(debug_TEQ_SHAI_file[instance], "bEnable                           = %d\n",     (int) pTeqConfig->bEnable);
    fprintf(debug_TEQ_SHAI_file[instance], "bAutomaticTransitionGain          = %d\n",     (int) pTeqConfig->bAutomaticTransitionGain);
    fprintf(debug_TEQ_SHAI_file[instance], "average_gain_exp_l                = %d\n",     (int) average_gain_exp_l);
    fprintf(debug_TEQ_SHAI_file[instance], "average_gain_mant_l               = 0x%08X\n", (int) average_gain_mant_l);
    fprintf(debug_TEQ_SHAI_file[instance], "average_gain_exp_r                = %d\n",     (int) average_gain_exp_r);
    fprintf(debug_TEQ_SHAI_file[instance], "average_gain_mant_r               = 0x%08X\n", (int) average_gain_mant_r);
    fprintf(debug_TEQ_SHAI_file[instance], "transition_gain_exp_l             = %d\n",     (int) pTeqConfig->transition_gain_exp_l);
    fprintf(debug_TEQ_SHAI_file[instance], "transition_gain_mant_l            = 0x%08X\n", (int) pTeqConfig->transition_gain_mant_l);
    fprintf(debug_TEQ_SHAI_file[instance], "transition_gain_exp_r             = %d\n",     (int) pTeqConfig->transition_gain_exp_r);
    fprintf(debug_TEQ_SHAI_file[instance], "transition_gain_mant_r            = 0x%08X\n", (int) pTeqConfig->transition_gain_mant_r);
    fprintf(debug_TEQ_SHAI_file[instance], "smoothing_delay                   = %d\n",     (int) smoothing_delay);
    fprintf(debug_TEQ_SHAI_file[instance], "same_biquad_l_r                   = %d\n",     (int) pTeqConfig->same_biquad_l_r);
    fprintf(debug_TEQ_SHAI_file[instance], "same_FIR_l_r                      = %d\n",     (int) pTeqConfig->same_FIR_l_r);
    fprintf(debug_TEQ_SHAI_file[instance], "biquad_first                      = %d\n",     (int) pTeqConfig->biquad_first);
    fprintf(debug_TEQ_SHAI_file[instance], "biquad_gain_exp_l                 = %d\n",     (int) pTeqConfig->biquad_gain_exp_l);
    fprintf(debug_TEQ_SHAI_file[instance], "biquad_gain_mant_l                = 0x%08X\n", (int) pTeqConfig->biquad_gain_mant_l);
    fprintf(debug_TEQ_SHAI_file[instance], "biquad_gain_exp_r                 = %d\n",     (int) pTeqConfig->biquad_gain_exp_r);
    fprintf(debug_TEQ_SHAI_file[instance], "biquad_gain_mant_r                = 0x%08X\n", (int) pTeqConfig->biquad_gain_mant_r);
    fprintf(debug_TEQ_SHAI_file[instance], "FIR_gain_exp_l                    = %d\n",     (int) pTeqConfig->FIR_gain_exp_l);
    fprintf(debug_TEQ_SHAI_file[instance], "FIR_gain_mant_l                   = 0x%08X\n", (int) pTeqConfig->FIR_gain_mant_l);
    fprintf(debug_TEQ_SHAI_file[instance], "FIR_gain_exp_r                    = %d\n",     (int) pTeqConfig->FIR_gain_exp_r);
    fprintf(debug_TEQ_SHAI_file[instance], "FIR_gain_mant_r                   = 0x%08X\n", (int) pTeqConfig->FIR_gain_mant_r);
    fprintf(debug_TEQ_SHAI_file[instance], "nb_biquad_cells_per_channel       = %d\n",     (int) pTeqConfig->nb_biquad_cells_per_channel);
    fprintf(debug_TEQ_SHAI_file[instance], "nb_FIR_coefs_per_channel          = %d\n",     (int) pTeqConfig->nb_FIR_coefs_per_channel);
    fprintf(debug_TEQ_SHAI_file[instance], "biquad cells :\n");
    for(i = 0; i < (pTeqConfig->nb_biquad_cells_per_channel * (pTeqConfig->same_biquad_l_r ? 1 : 2)); i++)
    {
        fprintf(debug_TEQ_SHAI_file[instance],
                "  b_exp=%+2d - b0=0x%08X - b1=0x%08X - b2=0x%08X - a1=0x%08X - a2=0x%08X\n",
                (int) pTeqConfig->biquad_cell[i].b_exp,
                (int) pTeqConfig->biquad_cell[i].b0,
                (int) pTeqConfig->biquad_cell[i].b1,
                (int) pTeqConfig->biquad_cell[i].b2,
                (int) pTeqConfig->biquad_cell[i].a1,
                (int) pTeqConfig->biquad_cell[i].a2);
    }
    fprintf(debug_TEQ_SHAI_file[instance], "FIR coefs :\n");
    for(i = 0; i < (pTeqConfig->nb_FIR_coefs_per_channel * (pTeqConfig->same_FIR_l_r ? 1 : 2)); i++)
    {
        if((i % 8) == 0)
        {
            fprintf(debug_TEQ_SHAI_file[instance], " ");
        }
        fprintf(debug_TEQ_SHAI_file[instance], " 0x%08X", (int) pTeqConfig->FIR_coef[i]);
        if((i % 8) == 7)
        {
            fprintf(debug_TEQ_SHAI_file[instance], "\n");
        }
    }
    if(i % 8)
    {
        fprintf(debug_TEQ_SHAI_file[instance], "\n");
    }
    fprintf(debug_TEQ_SHAI_file[instance], "--------------------------------------------------\n");
#endif // DEBUG_TEQ_SHAI_CONFIG_LEVEL > 1

    return OMX_ErrorNone;
}

