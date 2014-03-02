/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   teq_api.c
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifdef _NMF_MPC_
#include <libeffects/mpc/libteqapi.nmf>
#include "effects_dsp_index.h"
#include "dbc.h"
#include "audiolibs_common.h"
#include "config.idt"

#define TRANSDUCER_EQUALIZER_NMFIL_COMMON_SRC_CODE
#include "libeffects/libtransducer_equalizer/include/effect_common.h"

//OST Traces specific headers
#include <fsm/generic/include/FSM.h>
#include <trace.idt>
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "libeffects_mpc_libteqapi_teq_apiTraces.h"
#endif

#define TRANSDUCER_EQUALIZER_NB_MEM_PRESET  6
#define SAMPLES_BUF_SIZE                    960

////////////////////////////////////////////////////////////
//			Internal wrapper structures declaration
////////////////////////////////////////////////////////////

typedef struct TeqWrpCtx_t {
    void *                                  pTraceObject;
    t_memory_bank                           nMemoryBank;
    TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T     *pTrEqStruct;
    TransducerEqualizerConfig_t             *pTrEqConfig;
    transducer_equalizer_smooth_config_t    *pTrEqSmoothConfig;
    MMshort                                 *pSamplesBuf;
} TeqWrpCtx_t;

///////////////////////////////////////////////////////////////////////////////////

static void allocateAndInitializeTEQcoeffs(TeqWrpCtx_t *pTeqCtx)
{
    /* first a table for the effect must be defined. This table gather the memory preset
       supported by the effect .
       In our case we support 6 memory preset.
    */
    static MEMORY_PRESET_T EXTMEM const transducer_equalizer_mem_preset_table[TRANSDUCER_EQUALIZER_NB_MEM_PRESET] =
    {
        MEM_ALL_TCM,
        MEM_ALL_DDR,
        MEM_ALL_ESRAM,
        MEM_MIX_ESRAM_TCM,
        MEM_MIX_DDR_TCM_1,
        MEM_MIX_ESRAM_DDR
    };

    /* For each zone allocated, a look up table must be defined to determine in
       which memory the zone must be allocated according the memory preset parameter
    */
    static MEMORY_TYPE_T EXTMEM const transducer_equalizer_malloc_biquad_coef[TRANSDUCER_EQUALIZER_NB_MEM_PRESET] =
    {
        MEM_XTCM,
        MEM_DDR24,
        MEM_ESR24,
        MEM_XTCM,
        MEM_XTCM,
        MEM_ESR24
    };

    static MEMORY_TYPE_T EXTMEM const transducer_equalizer_malloc_FIR_coef[TRANSDUCER_EQUALIZER_NB_MEM_PRESET] =
    {
        MEM_XTCM,
        MEM_DDR24,
        MEM_ESR24,
        MEM_XTCM,
        MEM_XTCM,
        MEM_ESR24
    };

    TRACE_t * this = pTeqCtx->pTraceObject;
    int index_mem_preset, i, nb_channels;
    t_ExtendedAllocParams pAllocParams;
    TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *pTrEqStruct = pTeqCtx->pTrEqStruct;

    nb_channels = pTrEqStruct->stereo_config ? 2 : 1;

    for(index_mem_preset = TRANSDUCER_EQUALIZER_NB_MEM_PRESET - 1; index_mem_preset > 0; index_mem_preset--)
    {
        if(transducer_equalizer_mem_preset_table[index_mem_preset] == pTrEqStruct->memory_preset)
        {
            break;
        }
    }

    pAllocParams.trace_p = this;

    if(pTrEqStruct->nb_alloc_biquad_cells_per_channel > 0)
    {
        pAllocParams.bank = transducer_equalizer_malloc_biquad_coef[index_mem_preset];
        pTrEqStruct->p_alloc_biquad_coef = (void *) vmalloc(nb_channels * pTrEqStruct->nb_alloc_biquad_cells_per_channel * sizeof(t_biquad_cell), &pAllocParams);

        pAllocParams.bank = MEM_YTCM;
        pTrEqStruct->p_biquad_mem = (MMshort YMEM*) vmalloc_y(nb_channels * 2 * (pTrEqStruct->nb_alloc_biquad_cells_per_channel + 1) * sizeof(MMshort), &pAllocParams);
        ASSERT(pTrEqStruct->p_alloc_biquad_coef != NULL);
        ASSERT(pTrEqStruct->p_biquad_mem        != NULL);
    }

    if(pTrEqStruct->nb_alloc_FIR_coefs_per_channel > 0)
    {
        MMshort YMEM *p_FIR_mem = NULL;

        pAllocParams.bank = transducer_equalizer_malloc_FIR_coef[index_mem_preset];
        pTrEqStruct->p_FIR_coef = (MMshort *) vmalloc(nb_channels * pTrEqStruct->nb_alloc_FIR_coefs_per_channel * sizeof(MMshort), &pAllocParams);

        pAllocParams.bank = MEM_YTCM;
        p_FIR_mem = (MMshort YMEM *) vmalloc_y(nb_channels * pTrEqStruct->nb_alloc_FIR_coefs_per_channel * sizeof(MMshort), &pAllocParams);
        ASSERT(pTrEqStruct->p_FIR_coef != NULL);
        ASSERT(p_FIR_mem != NULL);
        pTrEqStruct->p_FIR_mem = winit_circ_ymem_ptr(p_FIR_mem, p_FIR_mem, p_FIR_mem + nb_channels * pTrEqStruct->nb_alloc_FIR_coefs_per_channel);
    }

    pTrEqStruct->alloc_done = TRUE;

    // default filter settings (flat response)
    if(!pTrEqStruct->stereo_config)
    {
        pTrEqStruct->same_biquad_l_r = TRUE;
        pTrEqStruct->same_FIR_l_r    = TRUE;
    }
    if(pTrEqStruct->same_biquad_l_r)
    {
        pTrEqStruct->p_biquad_coef_st = (t_biquad_cell_st *) NULL;
        pTrEqStruct->p_biquad_coef    = (t_biquad_cell    *) pTrEqStruct->p_alloc_biquad_coef;
        for(i = 0; i < pTrEqStruct->nb_alloc_biquad_cells_per_channel; i++)
        {
            pTrEqStruct->p_biquad_coef[i].b_exp = 0;
            pTrEqStruct->p_biquad_coef[i].b0    = 0x7FFFFF;
            pTrEqStruct->p_biquad_coef[i].b1    = 0;
            pTrEqStruct->p_biquad_coef[i].b2    = 0;
            pTrEqStruct->p_biquad_coef[i].a1    = 0;
            pTrEqStruct->p_biquad_coef[i].a2    = 0;
        }
    }
    else
    {
        pTrEqStruct->p_biquad_coef_st = (t_biquad_cell_st *) pTrEqStruct->p_alloc_biquad_coef;
        pTrEqStruct->p_biquad_coef    = (t_biquad_cell    *) NULL;
        for(i = 0; i < pTrEqStruct->nb_alloc_biquad_cells_per_channel; i++)
        {
            pTrEqStruct->p_biquad_coef_st[i].b_exp_l = 0;
            pTrEqStruct->p_biquad_coef_st[i].b_exp_r = 0;
            pTrEqStruct->p_biquad_coef_st[i].b0_l    = 0x7FFFFF;
            pTrEqStruct->p_biquad_coef_st[i].b0_r    = 0x7FFFFF;
            pTrEqStruct->p_biquad_coef_st[i].b1_l    = 0;
            pTrEqStruct->p_biquad_coef_st[i].b1_r    = 0;
            pTrEqStruct->p_biquad_coef_st[i].b2_l    = 0;
            pTrEqStruct->p_biquad_coef_st[i].b2_r    = 0;
            pTrEqStruct->p_biquad_coef_st[i].a1_l    = 0;
            pTrEqStruct->p_biquad_coef_st[i].a1_r    = 0;
            pTrEqStruct->p_biquad_coef_st[i].a2_l    = 0;
            pTrEqStruct->p_biquad_coef_st[i].a2_r    = 0;
        }
    }

    if(pTrEqStruct->nb_alloc_FIR_coefs_per_channel > 0)
    {
        if(pTrEqStruct->same_FIR_l_r)
        {
            pTrEqStruct->p_FIR_coef[0] = 0x7FFFFF;
            for(i = 1; i < pTrEqStruct->nb_alloc_FIR_coefs_per_channel; i++)
            {
                pTrEqStruct->p_FIR_coef[i] = 0;
            }
        }
        else
        {
            pTrEqStruct->p_FIR_coef[0] = 0x7FFFFF;
            pTrEqStruct->p_FIR_coef[1] = 0x7FFFFF;
            for(i = 2; i < 2 * pTrEqStruct->nb_alloc_FIR_coefs_per_channel; i++)
            {
                pTrEqStruct->p_FIR_coef[i] = 0;
            }
        }
    }

    if(pTrEqStruct->config_received)
    {
        common_setConfig(pTrEqStruct, pTeqCtx->pTrEqConfig, pTeqCtx->pTrEqSmoothConfig);
    }
    else
    {
        common_reset(pTrEqStruct);
        transducer_equalizer_set_filters_pointer(pTrEqStruct);
    }

    OstTraceFiltInst2(TRACE_DEBUG,"teqmpc::allocation of TEQ coeffs done (ctx=0x%x) : nbCh=%d", (unsigned int)pTeqCtx, nb_channels);
}

static void deallocateTEQcoeffs(TeqWrpCtx_t *pTeqCtx)
{
    /* first a table for the effect must be defined. This table gather the memory preset
       supported by the effect .
       In our case we support 6 memory preset.
    */
    static MEMORY_PRESET_T EXTMEM const transducer_equalizer_mem_preset_table[TRANSDUCER_EQUALIZER_NB_MEM_PRESET] =
    {
        MEM_ALL_TCM,
        MEM_ALL_DDR,
        MEM_ALL_ESRAM,
        MEM_MIX_ESRAM_TCM,
        MEM_MIX_DDR_TCM_1,
        MEM_MIX_ESRAM_DDR
    };

    /* For each zone allocated, a look up table must be defined to determine in
       which memory the zone must be allocated according the memory preset parameter
    */
    static MEMORY_TYPE_T EXTMEM const transducer_equalizer_malloc_biquad_coef[TRANSDUCER_EQUALIZER_NB_MEM_PRESET] =
    {
        MEM_XTCM,
        MEM_DDR24,
        MEM_ESR24,
        MEM_XTCM,
        MEM_XTCM,
        MEM_ESR24
    };

    static MEMORY_TYPE_T EXTMEM const transducer_equalizer_malloc_FIR_coef[TRANSDUCER_EQUALIZER_NB_MEM_PRESET] =
    {
        MEM_XTCM,
        MEM_DDR24,
        MEM_ESR24,
        MEM_XTCM,
        MEM_XTCM,
        MEM_ESR24
    };

    TRACE_t * this = pTeqCtx->pTraceObject;
    int index_mem_preset;
    t_ExtendedAllocParams pAllocParams;
    TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *pTrEqStruct = pTeqCtx->pTrEqStruct;

    ASSERT(pTrEqStruct->alloc_done == TRUE);

    for(index_mem_preset = TRANSDUCER_EQUALIZER_NB_MEM_PRESET - 1; index_mem_preset > 0; index_mem_preset--)
    {
        if(transducer_equalizer_mem_preset_table[index_mem_preset] == pTrEqStruct->memory_preset)
        {
            break;
        }
    }

    pAllocParams.trace_p = this;

    if(pTrEqStruct->nb_alloc_biquad_cells_per_channel > 0)
    {
        pAllocParams.bank = transducer_equalizer_malloc_biquad_coef[index_mem_preset];
        vfree(pTrEqStruct->p_alloc_biquad_coef, &pAllocParams);
        pTrEqStruct->p_alloc_biquad_coef = NULL;

        pAllocParams.bank = MEM_YTCM;
        vfree(pTrEqStruct->p_biquad_mem, &pAllocParams);
        pTrEqStruct->p_biquad_mem = NULL;
    }

    if(pTrEqStruct->nb_alloc_FIR_coefs_per_channel > 0)
    {
        pAllocParams.bank = transducer_equalizer_malloc_FIR_coef[index_mem_preset];
        vfree(pTrEqStruct->p_FIR_coef, &pAllocParams);
        pTrEqStruct->p_FIR_coef = NULL;

        pAllocParams.bank = MEM_YTCM;
        vfree(pTrEqStruct->p_FIR_mem, &pAllocParams);
        pTrEqStruct->p_FIR_mem = NULL;
    }

    pTrEqStruct->alloc_done = FALSE;

    OstTraceFiltInst1(TRACE_DEBUG,"teqmpc::deallocation of TEQ coeffs done (ctx=0x%x)", (unsigned int)pTeqCtx);
}

static void transition1_processBuffer(TeqWrpCtx_t *pTeqCtx, t_sword **p_p_inbuf, t_sword **p_p_outbuf, int nb_samples)
{
    TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *pTrEqStruct = pTeqCtx->pTrEqStruct;
    transducer_equalizer_smooth_config_t *pSmoothConfig = pTeqCtx->pTrEqSmoothConfig;
    t_sword *p_inbuf  = *p_p_inbuf;
    t_sword *p_outbuf = *p_p_outbuf;
    int     size, i, shift_div_transition;
    MMshort *p1, *p2;
    MMshort coef_num_smooth_input, coef_num_smooth_output, coef_div_transition;
    MMlong  tmp;

    size = (pTrEqStruct->stereo_config ? nb_samples * 2 : nb_samples);
    if(p_inbuf == p_outbuf)
    {
        ASSERT(size <= SAMPLES_BUF_SIZE);
        p1 = pTeqCtx->pSamplesBuf;
        p2 = p_inbuf;
#pragma loop force_hwloop
        for(i = size; i > 0; i--)
        {
            *p1++ = *p2++;
        }
        p1 = p_outbuf;
        p2 = pTeqCtx->pSamplesBuf;
    }
    else
    {
        p1 = p_outbuf;
        p2 = p_inbuf;
    }

    transducer_equalizer_filter(size,
                                p_inbuf,
                                p_outbuf,
                                pTrEqStruct);

    coef_num_smooth_input  = pSmoothConfig->smooth_transition_duration1 - pSmoothConfig->smooth_transition_counter1;
    coef_num_smooth_output = pSmoothConfig->smooth_transition_counter1;
    shift_div_transition   = pSmoothConfig->shift_div_transition1;
    coef_div_transition    = pSmoothConfig->coef_div_transition1;
    if(pTrEqStruct->stereo_config)
    {
        MMshort transition_gain_mant_l = pSmoothConfig->transition_gain_mant_l;
        MMshort transition_gain_exp_l  = pSmoothConfig->transition_gain_exp_l;
        MMshort transition_gain_mant_r = pSmoothConfig->transition_gain_mant_r;
        MMshort transition_gain_exp_r  = pSmoothConfig->transition_gain_exp_r;

#pragma loop force_hwloop
        for(i = nb_samples; i > 0; i--)
        {
            tmp   = wL_msl(wL_imul(wfmulr(coef_num_smooth_input, transition_gain_mant_l), *p2++), transition_gain_exp_l);
            tmp   = wL_addsat(tmp, wL_imul(coef_num_smooth_output, *p1));
            tmp   = wL_addsat(wL_msl(wL_fmul(coef_div_transition, wround_L(tmp)), shift_div_transition + 24), wL_msl(wL_fmul(coef_div_transition, wextract_l(tmp)), shift_div_transition));
            *p1++ = wround_L(tmp);
            tmp   = wL_msl(wL_imul(wfmulr(coef_num_smooth_input, transition_gain_mant_r), *p2++), transition_gain_exp_r);
            tmp   = wL_addsat(tmp, wL_imul(coef_num_smooth_output, *p1));
            tmp   = wL_addsat(wL_msl(wL_fmul(coef_div_transition, wround_L(tmp)), shift_div_transition + 24), wL_msl(wL_fmul(coef_div_transition, wextract_l(tmp)), shift_div_transition));
            *p1++ = wround_L(tmp);
            coef_num_smooth_input++;
            coef_num_smooth_output--;
        }
    }
    else
    {
        MMshort transition_gain_mant = pSmoothConfig->transition_gain_mant_l;
        MMshort transition_gain_exp  = pSmoothConfig->transition_gain_exp_l;

#pragma loop force_hwloop
        for(i = nb_samples; i > 0; i--)
        {
            tmp   = wL_msl(wL_imul(wfmulr(coef_num_smooth_input, transition_gain_mant), *p2++), transition_gain_exp);
            tmp   = wL_addsat(tmp, wL_imul(coef_num_smooth_output, *p1));
            tmp   = wL_addsat(wL_msl(wL_fmul(coef_div_transition, wround_L(tmp)), shift_div_transition + 24), wL_msl(wL_fmul(coef_div_transition, wextract_l(tmp)), shift_div_transition));
            *p1++ = wround_L(tmp);
            coef_num_smooth_input++;
            coef_num_smooth_output--;
        }
    }
    pSmoothConfig->smooth_transition_counter1 -= nb_samples;
    *p_p_inbuf                               += size;
    *p_p_outbuf                              += size;
}


static void transition2_processBuffer(TeqWrpCtx_t *pTeqCtx, t_sword **p_p_inbuf, t_sword **p_p_outbuf, int nb_samples)
{
    TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *pTrEqStruct = pTeqCtx->pTrEqStruct;
    transducer_equalizer_smooth_config_t *pSmoothConfig = pTeqCtx->pTrEqSmoothConfig;
    t_sword *p_inbuf  = *p_p_inbuf;
    t_sword *p_outbuf = *p_p_outbuf;
    int     size, i, shift_div_transition;
    MMshort *p1, *p2;
    MMshort coef_num_smooth_input, coef_num_smooth_output, coef_div_transition;
    MMlong  tmp;


    size = (pTrEqStruct->stereo_config ? nb_samples * 2 : nb_samples);
    if(p_inbuf == p_outbuf)
    {
        ASSERT(size <= SAMPLES_BUF_SIZE);
        p1 = pTeqCtx->pSamplesBuf;
        p2 = p_inbuf;
#pragma loop force_hwloop
        for(i = size; i > 0; i--)
        {
            *p1++ = *p2++;
        }
        p1 = p_outbuf;
        p2 = pTeqCtx->pSamplesBuf;
    }
    else
    {
        p1 = p_outbuf;
        p2 = p_inbuf;
    }

    transducer_equalizer_filter(size,
                                p_inbuf,
                                p_outbuf,
                                pTrEqStruct);

    coef_num_smooth_input  = pSmoothConfig->smooth_transition_counter2;
    coef_num_smooth_output = pSmoothConfig->smooth_transition_duration2 - pSmoothConfig->smooth_transition_counter2;
    shift_div_transition   = pSmoothConfig->shift_div_transition2;
    coef_div_transition    = pSmoothConfig->coef_div_transition2;
    if(pTrEqStruct->stereo_config)
    {
        MMshort transition_gain_mant_l = pSmoothConfig->transition_gain_mant_l;
        MMshort transition_gain_exp_l  = pSmoothConfig->transition_gain_exp_l;
        MMshort transition_gain_mant_r = pSmoothConfig->transition_gain_mant_r;
        MMshort transition_gain_exp_r  = pSmoothConfig->transition_gain_exp_r;

#pragma loop force_hwloop
        for(i = nb_samples; i > 0; i--)
        {
            tmp   = wL_msl(wL_imul(wfmulr(coef_num_smooth_input, transition_gain_mant_l), *p2++), transition_gain_exp_l);
            tmp   = wL_addsat(tmp, wL_imul(coef_num_smooth_output, *p1));
            tmp   = wL_addsat(wL_msl(wL_fmul(coef_div_transition, wround_L(tmp)), shift_div_transition + 24), wL_msl(wL_fmul(coef_div_transition, wextract_l(tmp)), shift_div_transition));
            *p1++ = wround_L(tmp);
            tmp   = wL_msl(wL_imul(wfmulr(coef_num_smooth_input, transition_gain_mant_r), *p2++), transition_gain_exp_r);
            tmp   = wL_addsat(tmp, wL_imul(coef_num_smooth_output, *p1));
            tmp   = wL_addsat(wL_msl(wL_fmul(coef_div_transition, wround_L(tmp)), shift_div_transition + 24), wL_msl(wL_fmul(coef_div_transition, wextract_l(tmp)), shift_div_transition));
            *p1++ = wround_L(tmp);
            coef_num_smooth_input--;
            coef_num_smooth_output++;
        }
    }
    else
    {
        MMshort transition_gain_mant = pSmoothConfig->transition_gain_mant_l;
        MMshort transition_gain_exp  = pSmoothConfig->transition_gain_exp_l;

#pragma loop force_hwloop
        for(i = nb_samples; i > 0; i--)
        {
            tmp   = wL_msl(wL_imul(wfmulr(coef_num_smooth_input, transition_gain_mant), *p2++), transition_gain_exp);
            tmp   = wL_addsat(tmp, wL_imul(coef_num_smooth_output, *p1));
            tmp   = wL_addsat(wL_msl(wL_fmul(coef_div_transition, wround_L(tmp)), shift_div_transition + 24), wL_msl(wL_fmul(coef_div_transition, wextract_l(tmp)), shift_div_transition));
            *p1++ = wround_L(tmp);
            coef_num_smooth_input--;
            coef_num_smooth_output++;
        }
    }
    pSmoothConfig->smooth_transition_counter2 -= nb_samples;
    *p_p_inbuf                               += size;
    *p_p_outbuf                              += size;
}

///////////////////////////////////////////////////////////////////////////////////

t_EffectError Teq_construct(EffectInit_t *pInitParameters, void** pEffectCtx)
{
    TRACE_t * this = (TRACE_t *) pInitParameters->pTraceObject;
    TeqWrpCtx_t *pTeqCtx = NULL;
    t_ExtendedAllocParams pAllocParams;

    pAllocParams.bank = pInitParameters->nMemoryBank;
    pAllocParams.trace_p = this;

    pTeqCtx = (TeqWrpCtx_t *) vmalloc(sizeof(TeqWrpCtx_t), &pAllocParams);
    if(pTeqCtx == NULL) return EFFECT_ERROR;
    *pEffectCtx= (void*)pTeqCtx;

    pTeqCtx->pTraceObject   = this;
    pTeqCtx->nMemoryBank    = pInitParameters->nMemoryBank;

    //Allocate teq algorithms structures :
    //TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T is allocated in TCM
    //others are allocated in external memory
    pAllocParams.bank = MEM_XTCM;

    pTeqCtx->pTrEqStruct = (TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *) vmalloc(sizeof(TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T), &pAllocParams);
    if(pTeqCtx->pTrEqStruct == NULL) return EFFECT_ERROR;

    pAllocParams.bank = MEM_DDR24;

    pTeqCtx->pTrEqConfig = (TransducerEqualizerConfig_t *) vmalloc(sizeof(TransducerEqualizerConfig_t), &pAllocParams);
    if(pTeqCtx->pTrEqConfig == NULL) return EFFECT_ERROR;

    pTeqCtx->pTrEqSmoothConfig = (transducer_equalizer_smooth_config_t *) vmalloc(sizeof(transducer_equalizer_smooth_config_t), &pAllocParams);
    if(pTeqCtx->pTrEqSmoothConfig == NULL) return EFFECT_ERROR;

    pTeqCtx->pSamplesBuf = vmalloc(sizeof(MMshort)*SAMPLES_BUF_SIZE, &pAllocParams);
    if(pTeqCtx->pSamplesBuf == NULL) return EFFECT_ERROR;

    common_start(pTeqCtx->pTrEqStruct, pTeqCtx->pTrEqSmoothConfig);

    OstTraceFiltInst1(TRACE_DEBUG,"teqmpc::construct (ctx=0x%x)", (unsigned int)pTeqCtx);

    return EFFECT_ERROR_NONE;
}

t_EffectError Teq_open(void *pEffectCtx, EffectOpen_t *pOpenParameters, t_EffectProcessMode *processMode)
{
    TeqWrpCtx_t *pTeqCtx = (TeqWrpCtx_t*) pEffectCtx;
    TRACE_t * this = pTeqCtx->pTraceObject;
    PcmSettings_t *pInputPcmSettings = NULL;
    PcmSettings_t *pOutputPcmSettings = NULL;
    ASSERT(pOpenParameters!=NULL);

    pInputPcmSettings = &pOpenParameters->sInputPcmSettings;
    pOutputPcmSettings = &pOpenParameters->sOutputPcmSettings;

    OstTraceFiltInst3(TRACE_DEBUG,"teqmpc::open (ctx=0x%x) : nbCh=%d, freq=%d", (unsigned int)pTeqCtx, pInputPcmSettings->nChannels, pInputPcmSettings->nSamplingRate);

    ASSERT(pInputPcmSettings->nSamplingRate == pOutputPcmSettings->nSamplingRate);
    ASSERT(pInputPcmSettings->nChannels == pOutputPcmSettings->nChannels);

    //TEQ coefficients will be initialized when Teq_set_parameter is called with AFM_DSP_IndexParamTeq index.

    *processMode = EFFECT_INPLACE; //TEQ is always working in inplace mode

    return EFFECT_ERROR_NONE;
}

t_EffectError Teq_set_parameter(void *pEffectCtx, int index, void *param_struct)
{
    TeqWrpCtx_t *pTeqCtx = (TeqWrpCtx_t*) pEffectCtx;
    TRACE_t * this = pTeqCtx->pTraceObject;

    switch(index) {
        case AFM_DSP_IndexParamTeq:
            {
                TransducerEqualizerParams_t * pTeqParams = (TransducerEqualizerParams_t *)param_struct;

                OstTraceFiltInst2(TRACE_DEBUG,"teqmpc::set_parameter (ctx=0x%x) : memory_preset=%d", (unsigned int)pTeqCtx, pTeqParams->iMemoryPreset);
                OstTraceFiltInst4(TRACE_DEBUG,"teqmpc::set_parameter (ctx=0x%x) : istereo=%d inb_alloc_biquad_cells_per_channel=%d inb_alloc_FIR_coefs_per_channel=%d", (unsigned int)pTeqCtx, pTeqParams->istereo, pTeqParams->inb_alloc_biquad_cells_per_channel, pTeqParams->inb_alloc_FIR_coefs_per_channel);

                PRECONDITION(pTeqParams->iMemoryPreset<= MEM_MIX_ESRAM_OTHER_5);
                pTeqCtx->pTrEqStruct->memory_preset = pTeqParams->iMemoryPreset;

                common_setParameter(pTeqCtx->pTrEqStruct, pTeqParams);

                allocateAndInitializeTEQcoeffs(pTeqCtx);

                return EFFECT_ERROR_NONE;
            }
        default:
            return EFFECT_ERROR;
    }
}

t_EffectError Teq_set_config(void *pEffectCtx, int index, void *config_struct)
{
    TeqWrpCtx_t *pTeqCtx = (TeqWrpCtx_t*) pEffectCtx;
    TRACE_t * this = pTeqCtx->pTraceObject;

    switch(index) {
        case AFM_DSP_IndexConfigTeq:
            {
                t_uint16 * pTeqConfig = (t_uint16 *)config_struct;
                t_uint16 * pLocalTeqConfig = (t_uint16 *)pTeqCtx->pTrEqConfig;
                int i = 0;

                //copy config into local structure -> Use loop instead of
                // *pTeqCtx->pTrEqConfig = *pTeqConfig;
                //because of MMDSP-compilo strange behavior. Indeed sometimes generated code copies config structure in stack before
                //copying it in config structure allocated in context (pTeqCtx->pTrEqConfig).
                //It implies to increase stack size otherwise we get a STACK OVERFLOW DSP Panic.
                //With following loop, stack is not used as intermediate so we don't need to increase stack size.
                for(i=0; i<sizeof(TransducerEqualizerConfig_t); i++) {
                    *pLocalTeqConfig++ = *pTeqConfig++;
                }

                // real setConfig
                common_setConfig(pTeqCtx->pTrEqStruct, pTeqCtx->pTrEqConfig,  pTeqCtx->pTrEqSmoothConfig);

                OstTraceFiltInst2(TRACE_DEBUG,"teqmpc::set_config AFM_DSP_IndexConfigTeq (ctx=0x%x) iEnable=%d", (unsigned int)pTeqCtx, pTeqCtx->pTrEqConfig->iEnable);

                return EFFECT_ERROR_NONE;
            }
        default:
            return EFFECT_ERROR;
    }
}

t_EffectError Teq_processBuffer(void* pEffectCtx, Buffer_p inputBuf, Buffer_p outputBuf, bool *needInputBuf, bool *filledOutputBuffer)
{
    TeqWrpCtx_t *pTeqCtx = (TeqWrpCtx_t*) pEffectCtx;
    TRACE_t * this = pTeqCtx->pTraceObject;
    TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *pTrEqStruct = pTeqCtx->pTrEqStruct;
    transducer_equalizer_smooth_config_t *pSmoothConfig = pTeqCtx->pTrEqSmoothConfig;
    int nb_samples, nb_channels_FIR, i;
    t_sword *p_inbuf = inputBuf->data;
    t_sword *p_outbuf = outputBuf->data;
    MMshort *p1, *p2, *p3;
    MMshort coef_smooth_FIR;
    t_uint16 size = inputBuf->filledLen;

    if(pTrEqStruct->chan_nb == -1)
    {
        // no data has been processed : last chan_nb must be less or equal to allocated chan_nb
        // if so, set chan_nb (and offset) to this value and set new config
        if(pTrEqStruct->new_chan_nb == -1)
        {
            pTrEqStruct->new_chan_nb = pTrEqStruct->chan_nb = pTrEqStruct->offset = pTrEqStruct->alloc_chan_nb;
        }
        else
        {
            ASSERT(pTrEqStruct->new_chan_nb == pTrEqStruct->alloc_chan_nb);
        }
    }

    if(!pTrEqStruct->enable)
    {
        if(p_inbuf != p_outbuf)
        {
#pragma loop force_hwloop
            for(i = 0; i < size; i++)
            {
                p_outbuf[i] = p_inbuf[i];
            }
        }
    }
    else if(!pTrEqStruct->smooth_config_change)
    {
        // no configuration smoothing
        transducer_equalizer_filter(size,
                                    p_inbuf,
                                    p_outbuf,
                                    pTrEqStruct);
    }
    else
    {
        if(pTrEqStruct->stereo_config)
        {
            nb_samples = (size >> 1);
        }
        else
        {
            nb_samples = size;
        }

        if(pSmoothConfig->smooth_transition_counter1 > 0)
        {
            if(pSmoothConfig->smooth_transition_counter1 > nb_samples)
            {
                // first transitionpTrEqStruct
                transition1_processBuffer(pTeqCtx, &p_inbuf, &p_outbuf, nb_samples);
            }
            else
            {
                // end of first transition
                nb_samples -= pSmoothConfig->smooth_transition_counter1;
                transition1_processBuffer(pTeqCtx, &p_inbuf, &p_outbuf, pSmoothConfig->smooth_transition_counter1);

                common_copy_new_config(pTrEqStruct, pSmoothConfig);
                common_reset(pTrEqStruct);
                transducer_equalizer_set_filters_pointer(pTrEqStruct);

                if(pSmoothConfig->smooth_transition_counter2 == 0)
                {
                    // no second transition : means that transducer_equalizer is now disabled
                    if(p_inbuf != p_outbuf)
                    {
#pragma loop force_hwloop
                        for(i = (pTrEqStruct->stereo_config ? nb_samples * 2 : nb_samples); i > 0; i--)
                        {
                            *p_outbuf++ = *p_inbuf++;
                        }
                    }
                    pTrEqStruct->smooth_config_change = FALSE;
                }
                else
                {
                    if(pSmoothConfig->smooth_transition_counter2 > nb_samples)
                    {
                        // second transition
                        transition2_processBuffer(pTeqCtx, &p_inbuf, &p_outbuf, nb_samples);
                    }
                    else
                    {
                        // end of second transition
                        nb_samples -= pSmoothConfig->smooth_transition_counter2;
                        transition2_processBuffer(pTeqCtx, &p_inbuf, &p_outbuf, pSmoothConfig->smooth_transition_counter2);

                        transducer_equalizer_filter((pTrEqStruct->stereo_config ? nb_samples * 2 : nb_samples),
                                                    p_inbuf,
                                                    p_outbuf,
                                                    pTrEqStruct);
                        pTrEqStruct->smooth_config_change = FALSE;
                    }
                }
            }
        }
        else if(pSmoothConfig->smooth_transition_counter2 > 0)
        {
            if(pSmoothConfig->smooth_transition_counter2 > nb_samples)
            {
                // second transition
                transition2_processBuffer(pTeqCtx, &p_inbuf, &p_outbuf, nb_samples);
            }
            else
            {
                // end of second transition
                nb_samples -= pSmoothConfig->smooth_transition_counter2;
                transition2_processBuffer(pTeqCtx, &p_inbuf, &p_outbuf, pSmoothConfig->smooth_transition_counter2);

                transducer_equalizer_filter((pTrEqStruct->stereo_config ? nb_samples * 2 : nb_samples),
                                            p_inbuf,
                                            p_outbuf,
                                            pTrEqStruct);
                pTrEqStruct->smooth_config_change = FALSE;
            }
        }
        else
        {
            if(pSmoothConfig->smooth_FIR_transition_counter > 0)
            {
                nb_channels_FIR = ((pTrEqStruct->stereo_config && !pTrEqStruct->same_FIR_l_r) ? 2 : 1);
                while((pSmoothConfig->smooth_FIR_transition_counter > 0) && (nb_samples > 0))
                {
                    coef_smooth_FIR = fract_ratio(pSmoothConfig->smooth_FIR_transition_counter, pSmoothConfig->smooth_FIR_transition_duration);
                    p1              = pTrEqStruct->p_FIR_coef;
                    p2              = pSmoothConfig->new_FIR_coef;
                    p3              = pSmoothConfig->old_FIR_coef;
#pragma loop force_hwloop
                    for(i = nb_channels_FIR * pTrEqStruct->nb_FIR_coefs_per_channel; i > 0; i--)
                    {
                        *p1 = waddsat(*p2, wfmulr(coef_smooth_FIR, wsubsat(*p3, *p2)));
                        p1++;
                        p2++;
                        p3++;
                    }

                    size = (pSmoothConfig->smooth_FIR_transition_counter > pSmoothConfig->FIR_smoothing_delay_granularity ? pSmoothConfig->FIR_smoothing_delay_granularity : pSmoothConfig->smooth_FIR_transition_counter);
                    size = (size > nb_samples ? nb_samples : size);
                    pSmoothConfig->smooth_FIR_transition_counter -= size;
                    nb_samples                                  -= size;

                    size = (pTrEqStruct->stereo_config ? size * 2 : size);
                    transducer_equalizer_filter(size,
                                                p_inbuf,
                                                p_outbuf,
                                                pTrEqStruct);
                    p_inbuf  += size;
                    p_outbuf += size;
                }
                if(pSmoothConfig->smooth_FIR_transition_counter == 0)
                {
                    // end of smooth FIR transition
                    p1 = pTrEqStruct->p_FIR_coef;
                    p2 = pSmoothConfig->save_new_FIR_coef;
#pragma loop force_hwloop
                    for(i = nb_channels_FIR * pSmoothConfig->new_nb_FIR_coefs_per_channel; i > 0; i--)
                    {
                        *p1++ = *p2++;
                    }
                    pTrEqStruct->nb_FIR_coefs_per_channel = pSmoothConfig->new_nb_FIR_coefs_per_channel;
                    pTrEqStruct->same_FIR_l_r             = pSmoothConfig->new_same_FIR_l_r;
                    pTrEqStruct->FIR_gain_exp_l           = pSmoothConfig->new_FIR_gain_exp_l;
                    pTrEqStruct->FIR_gain_mant_l          = pSmoothConfig->new_FIR_gain_mant_l;
                    pTrEqStruct->FIR_gain_exp_r           = pSmoothConfig->new_FIR_gain_exp_r;
                    pTrEqStruct->FIR_gain_mant_r          = pSmoothConfig->new_FIR_gain_mant_r;

                    transducer_equalizer_set_filters_pointer(pTrEqStruct);

                    if(nb_samples > 0)
                    {
                        size = (pTrEqStruct->stereo_config ? nb_samples * 2 : nb_samples);
                        transducer_equalizer_filter(size,
                                                    p_inbuf,
                                                    p_outbuf,
                                                    pTrEqStruct);
                    }

                    pTrEqStruct->smooth_config_change = FALSE;
                }
            }
            else
            {
                transducer_equalizer_filter(size,
                                            p_inbuf,
                                            p_outbuf,
                                            pTrEqStruct);

                pTrEqStruct->smooth_config_change = FALSE;
            }
        }

        if(!pTrEqStruct->smooth_config_change)
        {
            pSmoothConfig->old_average_gain_exp_l  = pSmoothConfig->new_average_gain_exp_l;
            pSmoothConfig->old_average_gain_mant_l = pSmoothConfig->new_average_gain_mant_l;
            pSmoothConfig->old_average_gain_exp_r  = pSmoothConfig->new_average_gain_exp_r;
            pSmoothConfig->old_average_gain_mant_r = pSmoothConfig->new_average_gain_mant_r;
        }
    }

    return EFFECT_ERROR_NONE;
}

Buffer_p Teq_getOutputBuffer(void* pEffectCtx)
{
    return NULL; //process inplace
}

void Teq_close(void* pEffectCtx)
{
    t_ExtendedAllocParams pAllocParams;
    TeqWrpCtx_t *pTeqCtx = (TeqWrpCtx_t*) pEffectCtx;

    common_reset(pTeqCtx->pTrEqStruct);

    //Free memory allocated for FIR coeffs
    deallocateTEQcoeffs(pTeqCtx);
}

void Teq_destroy(void* pEffectCtx)
{
    t_ExtendedAllocParams pAllocParams;
    TeqWrpCtx_t *pTeqCtx = (TeqWrpCtx_t*) pEffectCtx;
    TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *pTrEqStruct = pTeqCtx->pTrEqStruct;

    //Free TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T allocated in TCM memory
    pAllocParams.bank = MEM_XTCM;
    pAllocParams.trace_p = (TRACE_t *)pTeqCtx->pTraceObject;
    vfree(pTeqCtx->pTrEqStruct, &pAllocParams);

    //Free other teq algorithms structures allocated in external memory
    pAllocParams.bank = MEM_DDR24;
    vfree(pTeqCtx->pTrEqConfig, &pAllocParams);
    vfree(pTeqCtx->pTrEqSmoothConfig, &pAllocParams);
    vfree(pTeqCtx->pSamplesBuf, &pAllocParams);

    //Free teq context allocated
    pAllocParams.bank = pTeqCtx->nMemoryBank;
    vfree(pEffectCtx, &pAllocParams);
}

/*************************************************************************
 *  Structure that defines the effect interface + teq factory Method
 ************************************************************************/

EffectDescription_t teq_def =
{
    Teq_construct,
    Teq_set_parameter,
    Teq_set_config,
    Teq_open,
    Teq_processBuffer,
    Teq_getOutputBuffer,
    Teq_close,
    Teq_destroy,
};

static EffectDescription_t * Teq_factoryMethod(void)
{
    return &teq_def;
}

/*************************************************************************
 *  CONSTRUCT -> teq registration!
 ************************************************************************/

t_nmf_error METH(construct)(void)
{
    registerEffect("audio_processor.pcm.trans_equalizer", Teq_factoryMethod);

	return NMF_OK;
}

#endif

