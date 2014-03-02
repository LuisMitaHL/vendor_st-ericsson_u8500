/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   mdrc5b_api.c
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifdef _NMF_MPC_
#include <libeffects/mpc/libmdrc5bapi.nmf>
#include "effects_dsp_index.h"
#include "dbc.h"
#include "mdrc5b.h"

//OST Traces specific headers
#include <fsm/generic/include/FSM.h>
#include <trace.idt>
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "libeffects_mpc_libmdrc5bapi_mdrc5b_apiTraces.h"
#endif

//#define DEBUG_MDRC_DSP_CONFIG //To be defined to dump mdrc DSP config in kernel log

////////////////////////////////////////////////////////////
//			Internal wrapper structures declaration
////////////////////////////////////////////////////////////

typedef struct MdrcWrpCtx_t {
    void *                  pTraceObject;
    t_memory_bank           nMemoryBank;
    bool                    allocDone;
    bool                    configReceived;
    MDRC5B_LOCAL_STRUCT_T * pMdrcStruct;
    Mdrc5bAlgoConfig_t *    pMdrcAlgoConfig;
    MdrcConfig_t *          pMdrcConfig;
} MdrcWrpCtx_t;

///////////////////////////////////////////////////////////////////////////////////

#ifdef DEBUG_MDRC_DSP_CONFIG

static void printSetConfigDebugInfo(MdrcWrpCtx_t *pMdrcCtx)
{
    int i;

    NmfPrint0(0, "********** DUMP MDRC DSP config ************\n");
    NmfPrint1(0, "Cfg_Mode                  = 0x%x\n", pMdrcCtx->pMdrcAlgoConfig->Mode);
    NmfPrint1(0, "Cfg_NumBands              = 0x%x\n", pMdrcCtx->pMdrcAlgoConfig->NumBands);
    for(i=0; i<5; i++) {
        NmfPrint2(0, "Cfg_Band%d_Enable          = 0x%x\n", i, pMdrcCtx->pMdrcAlgoConfig->BandCompressors[i].Enable);
        NmfPrint2(0, "Cfg_Band%d_FreqCutoff      = 0x%x\n", i, pMdrcCtx->pMdrcAlgoConfig->BandCompressors[i].FreqCutoff);
        NmfPrint2(0, "Cfg_Band%d_PostGain        = 0x%x\n", i, pMdrcCtx->pMdrcAlgoConfig->BandCompressors[i].PostGain);
        NmfPrint2(0, "Cfg_Band%d_KneePoints      = 0x%x\n", i, pMdrcCtx->pMdrcAlgoConfig->BandCompressors[i].KneePoints);
        NmfPrint2(0, "Cfg_Band%d_DynResp0_0      = 0x%x\n", i, pMdrcCtx->pMdrcAlgoConfig->BandCompressors[i].DynamicResponse[0][0]);
        NmfPrint2(0, "Cfg_Band%d_DynResp0_1      = 0x%x\n", i, pMdrcCtx->pMdrcAlgoConfig->BandCompressors[i].DynamicResponse[0][1]);
        NmfPrint2(0, "Cfg_Band%d_DynResp1_0      = 0x%x\n", i, pMdrcCtx->pMdrcAlgoConfig->BandCompressors[i].DynamicResponse[1][0]);
        NmfPrint2(0, "Cfg_Band%d_DynResp1_1      = 0x%x\n", i, pMdrcCtx->pMdrcAlgoConfig->BandCompressors[i].DynamicResponse[1][1]);
        NmfPrint2(0, "Cfg_Band%d_DynResp2_0      = 0x%x\n", i, pMdrcCtx->pMdrcAlgoConfig->BandCompressors[i].DynamicResponse[2][0]);
        NmfPrint2(0, "Cfg_Band%d_DynResp2_1      = 0x%x\n", i, pMdrcCtx->pMdrcAlgoConfig->BandCompressors[i].DynamicResponse[2][1]);
        NmfPrint2(0, "Cfg_Band%d_DynResp3_0      = 0x%x\n", i, pMdrcCtx->pMdrcAlgoConfig->BandCompressors[i].DynamicResponse[3][0]);
        NmfPrint2(0, "Cfg_Band%d_DynResp3_1      = 0x%x\n", i, pMdrcCtx->pMdrcAlgoConfig->BandCompressors[i].DynamicResponse[3][1]);
        NmfPrint2(0, "Cfg_Band%d_DynResp4_0      = 0x%x\n", i, pMdrcCtx->pMdrcAlgoConfig->BandCompressors[i].DynamicResponse[4][0]);
        NmfPrint2(0, "Cfg_Band%d_DynResp4_1      = 0x%x\n", i, pMdrcCtx->pMdrcAlgoConfig->BandCompressors[i].DynamicResponse[4][1]);
        NmfPrint2(0, "Cfg_Band%d_DynResp5_0      = 0x%x\n", i, pMdrcCtx->pMdrcAlgoConfig->BandCompressors[i].DynamicResponse[5][0]);
        NmfPrint2(0, "Cfg_Band%d_DynResp5_1      = 0x%x\n", i, pMdrcCtx->pMdrcAlgoConfig->BandCompressors[i].DynamicResponse[5][1]);
        NmfPrint2(0, "Cfg_Band%d_DynResp6_0      = 0x%x\n", i, pMdrcCtx->pMdrcAlgoConfig->BandCompressors[i].DynamicResponse[6][0]);
        NmfPrint2(0, "Cfg_Band%d_DynResp6_1      = 0x%x\n", i, pMdrcCtx->pMdrcAlgoConfig->BandCompressors[i].DynamicResponse[6][1]);
        NmfPrint2(0, "Cfg_Band%d_DynResp7_0      = 0x%x\n", i, pMdrcCtx->pMdrcAlgoConfig->BandCompressors[i].DynamicResponse[7][0]);
        NmfPrint2(0, "Cfg_Band%d_DynResp7_1      = 0x%x\n", i, pMdrcCtx->pMdrcAlgoConfig->BandCompressors[i].DynamicResponse[7][1]);
        NmfPrint2(0, "Cfg_Band%d_DynResp8_0      = 0x%x\n", i, pMdrcCtx->pMdrcAlgoConfig->BandCompressors[i].DynamicResponse[8][0]);
        NmfPrint2(0, "Cfg_Band%d_DynResp8_1      = 0x%x\n", i, pMdrcCtx->pMdrcAlgoConfig->BandCompressors[i].DynamicResponse[8][1]);
        NmfPrint2(0, "Cfg_Band%d_DynResp9_0      = 0x%x\n", i, pMdrcCtx->pMdrcAlgoConfig->BandCompressors[i].DynamicResponse[9][0]);
        NmfPrint2(0, "Cfg_Band%d_DynResp9_1      = 0x%x\n", i, pMdrcCtx->pMdrcAlgoConfig->BandCompressors[i].DynamicResponse[9][1]);
        NmfPrint2(0, "Cfg_Band%d_AttackTime      = 0x%x\n", i, pMdrcCtx->pMdrcAlgoConfig->BandCompressors[i].AttackTime);
        NmfPrint2(0, "Cfg_Band%d_ReleaseTime     = 0x%x\n", i, pMdrcCtx->pMdrcAlgoConfig->BandCompressors[i].ReleaseTime);
    }
    NmfPrint1(0, "Cfg_Limiter_Enable        = 0x%x\n", pMdrcCtx->pMdrcAlgoConfig->Limiter.Enable);
    NmfPrint1(0, "Cfg_Limiter_RmsMeas       = 0x%x\n", pMdrcCtx->pMdrcAlgoConfig->Limiter.RmsMeasure);
    NmfPrint1(0, "Cfg_Limiter_Thresh        = 0x%x\n", pMdrcCtx->pMdrcAlgoConfig->Limiter.Thresh);
    NmfPrint1(0, "Ctx_mdrc_filter_kind      = 0x%x\n", pMdrcCtx->pMdrcStruct->mdrc_filter_kind);
    NmfPrint1(0, "Ctx_Mode                  = 0x%x\n", pMdrcCtx->pMdrcStruct->Mode);
    NmfPrint1(0, "Ctx_SampleFreq            = 0x%x\n", pMdrcCtx->pMdrcStruct->SampleFreq);
    NmfPrint1(0, "Ctx_NumMainCh             = 0x%x\n", pMdrcCtx->pMdrcStruct->NumMainCh);
    NmfPrint1(0, "Ctx_DataInOffset          = 0x%x\n", pMdrcCtx->pMdrcStruct->DataInOffset);
    NmfPrint1(0, "Ctx_DataOutOffset         = 0x%x\n", pMdrcCtx->pMdrcStruct->DataOutOffset);
    NmfPrint1(0, "Ctx_ConsumedSamples       = 0x%x\n", pMdrcCtx->pMdrcStruct->ConsumedSamples);
    NmfPrint1(0, "Ctx_ValidSamples          = 0x%x\n", pMdrcCtx->pMdrcStruct->ValidSamples);
    NmfPrint1(0, "Ctx_InBufSamp             = 0x%x\n", pMdrcCtx->pMdrcStruct->InBufSamp);
    NmfPrint1(0, "Ctx_OutRdIdx              = 0x%x\n", pMdrcCtx->pMdrcStruct->OutRdIdx);
    NmfPrint1(0, "Ctx_OutWtIdx              = 0x%x\n", pMdrcCtx->pMdrcStruct->OutWtIdx);
#ifdef MDRC5B_COMPRESSOR_ACTIVE
    NmfPrint1(0, "Ctx_CompressorEnable      = 0x%x\n", pMdrcCtx->pMdrcStruct->CompressorEnable);
    NmfPrint1(0, "Ctx_NumBands              = 0x%x\n", pMdrcCtx->pMdrcStruct->NumBands);
    NmfPrint1(0, "Ctx_Band0_Enable          = 0x%x\n", pMdrcCtx->pMdrcStruct->BandCompressors[0].Enable);
    NmfPrint1(0, "Ctx_Band0_FreqCutoff      = 0x%x\n", pMdrcCtx->pMdrcStruct->BandCompressors[0].FreqCutoff);
    NmfPrint1(0, "Ctx_Band0_PostGain        = 0x%x\n", pMdrcCtx->pMdrcStruct->BandCompressors[0].PostGain);
    NmfPrint1(0, "Ctx_Band0_KneePoints      = 0x%x\n", pMdrcCtx->pMdrcStruct->BandCompressors[0].KneePoints);
    for(i=0; i<5; i++) {
        NmfPrint2(0, "Ctx_Band%d_DynResp0_0      = 0x%x\n", i, pMdrcCtx->pMdrcStruct->BandCompressors[0].DynamicResponse[0][0]);
        NmfPrint2(0, "Ctx_Band%d_DynResp0_1      = 0x%x\n", i, pMdrcCtx->pMdrcStruct->BandCompressors[0].DynamicResponse[0][1]);
        NmfPrint2(0, "Ctx_Band%d_DynResp1_0      = 0x%x\n", i, pMdrcCtx->pMdrcStruct->BandCompressors[0].DynamicResponse[1][0]);
        NmfPrint2(0, "Ctx_Band%d_DynResp1_1      = 0x%x\n", i, pMdrcCtx->pMdrcStruct->BandCompressors[0].DynamicResponse[1][1]);
        NmfPrint2(0, "Ctx_Band%d_DynResp2_0      = 0x%x\n", i, pMdrcCtx->pMdrcStruct->BandCompressors[0].DynamicResponse[2][0]);
        NmfPrint2(0, "Ctx_Band%d_DynResp2_1      = 0x%x\n", i, pMdrcCtx->pMdrcStruct->BandCompressors[0].DynamicResponse[2][1]);
        NmfPrint2(0, "Ctx_Band%d_DynResp3_0      = 0x%x\n", i, pMdrcCtx->pMdrcStruct->BandCompressors[0].DynamicResponse[3][0]);
        NmfPrint2(0, "Ctx_Band%d_DynResp3_1      = 0x%x\n", i, pMdrcCtx->pMdrcStruct->BandCompressors[0].DynamicResponse[3][1]);
        NmfPrint2(0, "Ctx_Band%d_DynResp4_0      = 0x%x\n", i, pMdrcCtx->pMdrcStruct->BandCompressors[0].DynamicResponse[4][0]);
        NmfPrint2(0, "Ctx_Band%d_DynResp4_1      = 0x%x\n", i, pMdrcCtx->pMdrcStruct->BandCompressors[0].DynamicResponse[4][1]);
        NmfPrint2(0, "Ctx_Band%d_DynResp5_0      = 0x%x\n", i, pMdrcCtx->pMdrcStruct->BandCompressors[0].DynamicResponse[5][0]);
        NmfPrint2(0, "Ctx_Band%d_DynResp5_1      = 0x%x\n", i, pMdrcCtx->pMdrcStruct->BandCompressors[0].DynamicResponse[5][1]);
        NmfPrint2(0, "Ctx_Band%d_DynResp6_0      = 0x%x\n", i, pMdrcCtx->pMdrcStruct->BandCompressors[0].DynamicResponse[6][0]);
        NmfPrint2(0, "Ctx_Band%d_DynResp6_1      = 0x%x\n", i, pMdrcCtx->pMdrcStruct->BandCompressors[0].DynamicResponse[6][1]);
        NmfPrint2(0, "Ctx_Band%d_DynResp7_0      = 0x%x\n", i, pMdrcCtx->pMdrcStruct->BandCompressors[0].DynamicResponse[7][0]);
        NmfPrint2(0, "Ctx_Band%d_DynResp7_1      = 0x%x\n", i, pMdrcCtx->pMdrcStruct->BandCompressors[0].DynamicResponse[7][1]);
        NmfPrint2(0, "Ctx_Band%d_DynResp8_0      = 0x%x\n", i, pMdrcCtx->pMdrcStruct->BandCompressors[0].DynamicResponse[8][0]);
        NmfPrint2(0, "Ctx_Band%d_DynResp8_1      = 0x%x\n", i, pMdrcCtx->pMdrcStruct->BandCompressors[0].DynamicResponse[8][1]);
        NmfPrint2(0, "Ctx_Band%d_DynResp9_0      = 0x%x\n", i, pMdrcCtx->pMdrcStruct->BandCompressors[0].DynamicResponse[9][0]);
        NmfPrint2(0, "Ctx_Band%d_DynResp9_1      = 0x%x\n", i, pMdrcCtx->pMdrcStruct->BandCompressors[0].DynamicResponse[9][1]);
        NmfPrint2(0, "Ctx_Band%d_AttackTime      = 0x%x\n", i, pMdrcCtx->pMdrcStruct->BandCompressors[0].AttackTime);
        NmfPrint2(0, "Ctx_Band%d_ReleaseTime     = 0x%x\n", i, pMdrcCtx->pMdrcStruct->BandCompressors[0].ReleaseTime);
    }
    NmfPrint1(0, "Ctx_ChWeight0             = 0x%x\n", pMdrcCtx->pMdrcStruct->ChWeight[0]);
    NmfPrint1(0, "Ctx_ChWeight1             = 0x%x\n", pMdrcCtx->pMdrcStruct->ChWeight[1]);
    NmfPrint1(0, "Ctx_biquad_kind           = 0x%x\n", pMdrcCtx->pMdrcStruct->biquad_kind);
    NmfPrint1(0, "Ctx_FIR_kind              = 0x%x\n", pMdrcCtx->pMdrcStruct->FIR_kind);

    if((pMdrcCtx->pMdrcStruct->mdrc_filter_kind == MDRC_BIQUAD_FILTER) || (pMdrcCtx->pMdrcStruct->mdrc_filter_kind == MDRC_FIR_FILTER))
    {
        NmfPrint1(0, "Ctx_filt_p_sbands_filt    = 0x%x\n", pMdrcCtx->pMdrcStruct->p_mdrc_filters->p_sub_bands_filtering);
        NmfPrint1(0, "Ctx_filt_filter_kind      = 0x%x\n", pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filter_kind);
        NmfPrint1(0, "Ctx_filt_nb_channels      = 0x%x\n", pMdrcCtx->pMdrcStruct->p_mdrc_filters->nb_channels);
        NmfPrint1(0, "Ctx_filt_nb_bands         = 0x%x\n", pMdrcCtx->pMdrcStruct->p_mdrc_filters->nb_bands);

        if(pMdrcCtx->pMdrcStruct->mdrc_filter_kind == MDRC_BIQUAD_FILTER)
        {
            NmfPrint1(0, "Ctx_filt_biq0LPHP_comb    = 0x%x\n", pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[0].biquad_LP_HP_combination);
            NmfPrint1(0, "Ctx_filt_biq0LP_order     = 0x%x\n", pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[0].biquadLP_processing_ctx.biquad_order);

            if(pMdrcCtx->pMdrcStruct->biquad_kind == BUTTERWORTH_BIQUAD)
            {
                if(pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[0].biquadLP_processing_ctx.p_biquad_mem[0] != NULL)
                {
                    t_biquad_cell_Butter YMEM *p_biquad_cell = (t_biquad_cell_Butter YMEM *) pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[0].biquadLP_processing_ctx.p_biquad_cell_void;
                    for(i=0; i<4; i++) {
                        if(pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[0].biquadLP_processing_ctx.biquad_order > i) {
                            NmfPrint2(0, "Ctx_filt_biq0LP_%d_bexp   = 0x%x\n", i, p_biquad_cell->b_exp);
                            NmfPrint2(0, "Ctx_filt_biq0LP_%d_b0     = 0x%x\n", i, p_biquad_cell->b0);
                            NmfPrint2(0, "Ctx_filt_biq0LP_%d_b1     = 0x%x\n", i, p_biquad_cell->b0);
                            NmfPrint2(0, "Ctx_filt_biq0LP_%d_b2     = 0x%x\n", i, p_biquad_cell->b0);
                            NmfPrint2(0, "Ctx_filt_biq0LP_%d_a1     = 0x%x\n", i, p_biquad_cell->a1);
                            NmfPrint2(0, "Ctx_filt_biq0LP_%d_a2     = 0x%x\n", i, p_biquad_cell->a2);
                            p_biquad_cell++;
                        }
                    }
                }
            } else
            {
                if(pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[0].biquadLP_processing_ctx.p_biquad_mem[0] != NULL)
                {
                    t_biquad_cell YMEM *p_biquad_cell = (t_biquad_cell YMEM *) pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[0].biquadLP_processing_ctx.p_biquad_cell_void;
                    for(i=0; i<4; i++) {
                        if(pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[0].biquadLP_processing_ctx.biquad_order > i) {
                            NmfPrint2(0, "Ctx_filt_biq0LP_%d_bexp   = 0x%x\n", i, p_biquad_cell->b_exp);
                            NmfPrint2(0, "Ctx_filt_biq0LP_%d_b0     = 0x%x\n", i, p_biquad_cell->b0);
                            NmfPrint2(0, "Ctx_filt_biq0LP_%d_b1     = 0x%x\n", i, p_biquad_cell->b1);
                            NmfPrint2(0, "Ctx_filt_biq0LP_%d_b2     = 0x%x\n", i, p_biquad_cell->b2);
                            NmfPrint2(0, "Ctx_filt_biq0LP_%d_a1     = 0x%x\n", i, p_biquad_cell->a1);
                            NmfPrint2(0, "Ctx_filt_biq0LP_%d_a2     = 0x%x\n", i, p_biquad_cell->a2);
                            p_biquad_cell++;
                        }
                    }
                }
            }

            NmfPrint1(0, "Ctx_filt_biq0HP_order     = 0x%x\n",pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[0].biquadHP_processing_ctx.biquad_order);

            if(pMdrcCtx->pMdrcStruct->biquad_kind == BUTTERWORTH_BIQUAD)
            {
                if(pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[0].biquadHP_processing_ctx.p_biquad_mem[0] != NULL)
                {
                    t_biquad_cell_Butter YMEM *p_biquad_cell = (t_biquad_cell_Butter YMEM *) pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[0].biquadHP_processing_ctx.p_biquad_cell_void;
                    for(i=0; i<4; i++) {
                        if(pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[0].biquadHP_processing_ctx.biquad_order > i) {
                            NmfPrint2(0, "Ctx_filt_biq0HP_%d_bexp   = 0x%x\n", i, p_biquad_cell->b_exp);
                            NmfPrint2(0, "Ctx_filt_biq0HP_%d_b0     = 0x%x\n", i, p_biquad_cell->b0);
                            NmfPrint2(0, "Ctx_filt_biq0HP_%d_b1     = 0x%x\n", i, -p_biquad_cell->b0);
                            NmfPrint2(0, "Ctx_filt_biq0HP_%d_b2     = 0x%x\n", i, p_biquad_cell->b0);
                            NmfPrint2(0, "Ctx_filt_biq0HP_%d_a1     = 0x%x\n", i, p_biquad_cell->a1);
                            NmfPrint2(0, "Ctx_filt_biq0HP_%d_a2     = 0x%x\n", i, p_biquad_cell->a2);
                            p_biquad_cell++;
                        }
                    }
                }
            }
            else
            {
                if(pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[0].biquadHP_processing_ctx.p_biquad_mem[0] != NULL)
                {
                    t_biquad_cell YMEM *p_biquad_cell = (t_biquad_cell YMEM *) pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[0].biquadHP_processing_ctx.p_biquad_cell_void;
                    for(i=0; i<4; i++) {
                        if(pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[0].biquadHP_processing_ctx.biquad_order > i) {
                            NmfPrint2(0, "Ctx_filt_biq0HP_%d_bexp   = 0x%x\n", i, p_biquad_cell->b_exp);
                            NmfPrint2(0, "Ctx_filt_biq0HP_%d_b0     = 0x%x\n", i, p_biquad_cell->b0);
                            NmfPrint2(0, "Ctx_filt_biq0HP_%d_b1     = 0x%x\n", i, p_biquad_cell->b1);
                            NmfPrint2(0, "Ctx_filt_biq0HP_%d_b2     = 0x%x\n", i, p_biquad_cell->b2);
                            NmfPrint2(0, "Ctx_filt_biq0HP_%d_a1     = 0x%x\n", i, p_biquad_cell->a1);
                            NmfPrint2(0, "Ctx_filt_biq0HP_%d_a2     = 0x%x\n", i, p_biquad_cell->a2);
                            p_biquad_cell++;
                        }
                    }
                }
            }

            NmfPrint1(0, "Ctx_filt_biq1LPHP_comb    = 0x%x\n", pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[1].biquad_LP_HP_combination);
            NmfPrint1(0, "Ctx_filt_biq1LP_order     = 0x%x\n", pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[1].biquadLP_processing_ctx.biquad_order);

            if(pMdrcCtx->pMdrcStruct->biquad_kind == BUTTERWORTH_BIQUAD)
            {
                if(pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[1].biquadLP_processing_ctx.p_biquad_mem[0] != NULL)
                {
                    t_biquad_cell_Butter YMEM *p_biquad_cell = (t_biquad_cell_Butter YMEM *) pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[1].biquadLP_processing_ctx.p_biquad_cell_void;
                    for(i=0; i<4; i++) {
                        if(pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[1].biquadLP_processing_ctx.biquad_order > i) {
                            NmfPrint2(0, "Ctx_filt_biq1LP_%d_bexp   = 0x%x\n", i, p_biquad_cell->b_exp);
                            NmfPrint2(0, "Ctx_filt_biq1LP_%d_b0     = 0x%x\n", i, p_biquad_cell->b0);
                            NmfPrint2(0, "Ctx_filt_biq1LP_%d_b1     = 0x%x\n", i, p_biquad_cell->b0);
                            NmfPrint2(0, "Ctx_filt_biq1LP_%d_b2     = 0x%x\n", i, p_biquad_cell->b0);
                            NmfPrint2(0, "Ctx_filt_biq1LP_%d_a1     = 0x%x\n", i, p_biquad_cell->a1);
                            NmfPrint2(0, "Ctx_filt_biq1LP_%d_a2     = 0x%x\n", i, p_biquad_cell->a2);
                            p_biquad_cell++;
                        }
                    }
                }
            } else
            {
                if(pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[1].biquadLP_processing_ctx.p_biquad_mem[0] != NULL)
                {
                    t_biquad_cell YMEM *p_biquad_cell = (t_biquad_cell YMEM *) pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[1].biquadLP_processing_ctx.p_biquad_cell_void;
                    for(i=0; i<4; i++) {
                        if(pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[1].biquadLP_processing_ctx.biquad_order > i) {
                            NmfPrint2(0, "Ctx_filt_biq1LP_%d_bexp   = 0x%x\n", i, p_biquad_cell->b_exp);
                            NmfPrint2(0, "Ctx_filt_biq1LP_%d_b0     = 0x%x\n", i, p_biquad_cell->b0);
                            NmfPrint2(0, "Ctx_filt_biq1LP_%d_b1     = 0x%x\n", i, p_biquad_cell->b1);
                            NmfPrint2(0, "Ctx_filt_biq1LP_%d_b2     = 0x%x\n", i, p_biquad_cell->b2);
                            NmfPrint2(0, "Ctx_filt_biq1LP_%d_a1     = 0x%x\n", i, p_biquad_cell->a1);
                            NmfPrint2(0, "Ctx_filt_biq1LP_%d_a2     = 0x%x\n", i, p_biquad_cell->a2);
                            p_biquad_cell++;
                        }
                    }
                }
            }

            NmfPrint1(0, "Ctx_filt_biq1HP_order     = 0x%x\n",pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[1].biquadHP_processing_ctx.biquad_order);

            if(pMdrcCtx->pMdrcStruct->biquad_kind == BUTTERWORTH_BIQUAD)
            {
                if(pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[1].biquadHP_processing_ctx.p_biquad_mem[0] != NULL)
                {
                    t_biquad_cell_Butter YMEM *p_biquad_cell = (t_biquad_cell_Butter YMEM *) pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[1].biquadHP_processing_ctx.p_biquad_cell_void;
                    for(i=0; i<4; i++) {
                        if(pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[1].biquadHP_processing_ctx.biquad_order > i) {
                            NmfPrint2(0, "Ctx_filt_biq1HP_%d_bexp   = 0x%x\n", i, p_biquad_cell->b_exp);
                            NmfPrint2(0, "Ctx_filt_biq1HP_%d_b0     = 0x%x\n", i, p_biquad_cell->b0);
                            NmfPrint2(0, "Ctx_filt_biq1HP_%d_b1     = 0x%x\n", i, -p_biquad_cell->b0);
                            NmfPrint2(0, "Ctx_filt_biq1HP_%d_b2     = 0x%x\n", i, p_biquad_cell->b0);
                            NmfPrint2(0, "Ctx_filt_biq1HP_%d_a1     = 0x%x\n", i, p_biquad_cell->a1);
                            NmfPrint2(0, "Ctx_filt_biq1HP_%d_a2     = 0x%x\n", i, p_biquad_cell->a2);
                            p_biquad_cell++;
                        }
                    }
                }
            }
            else
            {
                if(pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[1].biquadHP_processing_ctx.p_biquad_mem[0] != NULL)
                {
                    t_biquad_cell YMEM *p_biquad_cell = (t_biquad_cell YMEM *) pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[1].biquadHP_processing_ctx.p_biquad_cell_void;
                    for(i=0; i<4; i++) {
                        if(pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[1].biquadHP_processing_ctx.biquad_order > i) {
                            NmfPrint2(0, "Ctx_filt_biq1HP_%d_bexp   = 0x%x\n", i, p_biquad_cell->b_exp);
                            NmfPrint2(0, "Ctx_filt_biq1HP_%d_b0     = 0x%x\n", i, p_biquad_cell->b0);
                            NmfPrint2(0, "Ctx_filt_biq1HP_%d_b1     = 0x%x\n", i, p_biquad_cell->b1);
                            NmfPrint2(0, "Ctx_filt_biq1HP_%d_b2     = 0x%x\n", i, p_biquad_cell->b2);
                            NmfPrint2(0, "Ctx_filt_biq1HP_%d_a1     = 0x%x\n", i, p_biquad_cell->a1);
                            NmfPrint2(0, "Ctx_filt_biq1HP_%d_a2     = 0x%x\n", i, p_biquad_cell->a2);
                            p_biquad_cell++;
                        }
                    }
                }
            }

            NmfPrint1(0, "Ctx_filt_biq2LPHP_comb    = 0x%x\n", pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[2].biquad_LP_HP_combination);
            NmfPrint1(0, "Ctx_filt_biq2LP_order     = 0x%x\n", pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[2].biquadLP_processing_ctx.biquad_order);

            if(pMdrcCtx->pMdrcStruct->biquad_kind == BUTTERWORTH_BIQUAD)
            {
                if(pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[2].biquadLP_processing_ctx.p_biquad_mem[0] != NULL)
                {
                    t_biquad_cell_Butter YMEM *p_biquad_cell = (t_biquad_cell_Butter YMEM *) pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[2].biquadLP_processing_ctx.p_biquad_cell_void;
                    for(i=0; i<4; i++) {
                        if(pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[2].biquadLP_processing_ctx.biquad_order > i) {
                            NmfPrint2(0, "Ctx_filt_biq2LP_%d_bexp   = 0x%x\n", i, p_biquad_cell->b_exp);
                            NmfPrint2(0, "Ctx_filt_biq2LP_%d_b0     = 0x%x\n", i, p_biquad_cell->b0);
                            NmfPrint2(0, "Ctx_filt_biq2LP_%d_b1     = 0x%x\n", i, p_biquad_cell->b0);
                            NmfPrint2(0, "Ctx_filt_biq2LP_%d_b2     = 0x%x\n", i, p_biquad_cell->b0);
                            NmfPrint2(0, "Ctx_filt_biq2LP_%d_a1     = 0x%x\n", i, p_biquad_cell->a1);
                            NmfPrint2(0, "Ctx_filt_biq2LP_%d_a2     = 0x%x\n", i, p_biquad_cell->a2);
                            p_biquad_cell++;
                        }
                    }
                }
            } else
            {
                if(pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[2].biquadLP_processing_ctx.p_biquad_mem[0] != NULL)
                {
                    t_biquad_cell YMEM *p_biquad_cell = (t_biquad_cell YMEM *) pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[2].biquadLP_processing_ctx.p_biquad_cell_void;
                    for(i=0; i<4; i++) {
                        if(pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[2].biquadLP_processing_ctx.biquad_order > i) {
                            NmfPrint2(0, "Ctx_filt_biq2LP_%d_bexp   = 0x%x\n", i, p_biquad_cell->b_exp);
                            NmfPrint2(0, "Ctx_filt_biq2LP_%d_b0     = 0x%x\n", i, p_biquad_cell->b0);
                            NmfPrint2(0, "Ctx_filt_biq2LP_%d_b1     = 0x%x\n", i, p_biquad_cell->b1);
                            NmfPrint2(0, "Ctx_filt_biq2LP_%d_b2     = 0x%x\n", i, p_biquad_cell->b2);
                            NmfPrint2(0, "Ctx_filt_biq2LP_%d_a1     = 0x%x\n", i, p_biquad_cell->a1);
                            NmfPrint2(0, "Ctx_filt_biq2LP_%d_a2     = 0x%x\n", i, p_biquad_cell->a2);
                            p_biquad_cell++;
                        }
                    }
                }
            }

            NmfPrint1(0, "Ctx_filt_biq2HP_order     = 0x%x\n",pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[2].biquadHP_processing_ctx.biquad_order);

            if(pMdrcCtx->pMdrcStruct->biquad_kind == BUTTERWORTH_BIQUAD)
            {
                if(pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[2].biquadHP_processing_ctx.p_biquad_mem[0] != NULL)
                {
                    t_biquad_cell_Butter YMEM *p_biquad_cell = (t_biquad_cell_Butter YMEM *) pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[2].biquadHP_processing_ctx.p_biquad_cell_void;
                    for(i=0; i<4; i++) {
                        if(pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[2].biquadHP_processing_ctx.biquad_order > i) {
                            NmfPrint2(0, "Ctx_filt_biq2HP_%d_bexp   = 0x%x\n", i, p_biquad_cell->b_exp);
                            NmfPrint2(0, "Ctx_filt_biq2HP_%d_b0     = 0x%x\n", i, p_biquad_cell->b0);
                            NmfPrint2(0, "Ctx_filt_biq2HP_%d_b1     = 0x%x\n", i, -p_biquad_cell->b0);
                            NmfPrint2(0, "Ctx_filt_biq2HP_%d_b2     = 0x%x\n", i, p_biquad_cell->b0);
                            NmfPrint2(0, "Ctx_filt_biq2HP_%d_a1     = 0x%x\n", i, p_biquad_cell->a1);
                            NmfPrint2(0, "Ctx_filt_biq2HP_%d_a2     = 0x%x\n", i, p_biquad_cell->a2);
                            p_biquad_cell++;
                        }
                    }
                }
            }
            else
            {
                if(pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[2].biquadHP_processing_ctx.p_biquad_mem[0] != NULL)
                {
                    t_biquad_cell YMEM *p_biquad_cell = (t_biquad_cell YMEM *) pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[2].biquadHP_processing_ctx.p_biquad_cell_void;
                    for(i=0; i<4; i++) {
                        if(pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[2].biquadHP_processing_ctx.biquad_order > i) {
                            NmfPrint2(0, "Ctx_filt_biq2HP_%d_bexp   = 0x%x\n", i, p_biquad_cell->b_exp);
                            NmfPrint2(0, "Ctx_filt_biq2HP_%d_b0     = 0x%x\n", i, p_biquad_cell->b0);
                            NmfPrint2(0, "Ctx_filt_biq2HP_%d_b1     = 0x%x\n", i, p_biquad_cell->b1);
                            NmfPrint2(0, "Ctx_filt_biq2HP_%d_b2     = 0x%x\n", i, p_biquad_cell->b2);
                            NmfPrint2(0, "Ctx_filt_biq2HP_%d_a1     = 0x%x\n", i, p_biquad_cell->a1);
                            NmfPrint2(0, "Ctx_filt_biq2HP_%d_a2     = 0x%x\n", i, p_biquad_cell->a2);
                            p_biquad_cell++;
                        }
                    }
                }
            }

            NmfPrint1(0, "Ctx_filt_biq3LPHP_comb    = 0x%x\n", pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[3].biquad_LP_HP_combination);
            NmfPrint1(0, "Ctx_filt_biq3LP_order     = 0x%x\n", pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[3].biquadLP_processing_ctx.biquad_order);

            if(pMdrcCtx->pMdrcStruct->biquad_kind == BUTTERWORTH_BIQUAD)
            {
                if(pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[3].biquadLP_processing_ctx.p_biquad_mem[0] != NULL)
                {
                    t_biquad_cell_Butter YMEM *p_biquad_cell = (t_biquad_cell_Butter YMEM *) pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[3].biquadLP_processing_ctx.p_biquad_cell_void;
                    for(i=0; i<4; i++) {
                        if(pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[3].biquadLP_processing_ctx.biquad_order > i) {
                            NmfPrint2(0, "Ctx_filt_biq3LP_%d_bexp   = 0x%x\n", i, p_biquad_cell->b_exp);
                            NmfPrint2(0, "Ctx_filt_biq3LP_%d_b0     = 0x%x\n", i, p_biquad_cell->b0);
                            NmfPrint2(0, "Ctx_filt_biq3LP_%d_b1     = 0x%x\n", i, p_biquad_cell->b0);
                            NmfPrint2(0, "Ctx_filt_biq3LP_%d_b2     = 0x%x\n", i, p_biquad_cell->b0);
                            NmfPrint2(0, "Ctx_filt_biq3LP_%d_a1     = 0x%x\n", i, p_biquad_cell->a1);
                            NmfPrint2(0, "Ctx_filt_biq3LP_%d_a2     = 0x%x\n", i, p_biquad_cell->a2);
                            p_biquad_cell++;
                        }
                    }
                }
            } else
            {
                if(pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[3].biquadLP_processing_ctx.p_biquad_mem[0] != NULL)
                {
                    t_biquad_cell YMEM *p_biquad_cell = (t_biquad_cell YMEM *) pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[3].biquadLP_processing_ctx.p_biquad_cell_void;
                    for(i=0; i<4; i++) {
                        if(pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[3].biquadLP_processing_ctx.biquad_order > i) {
                            NmfPrint2(0, "Ctx_filt_biq3LP_%d_bexp   = 0x%x\n", i, p_biquad_cell->b_exp);
                            NmfPrint2(0, "Ctx_filt_biq3LP_%d_b0     = 0x%x\n", i, p_biquad_cell->b0);
                            NmfPrint2(0, "Ctx_filt_biq3LP_%d_b1     = 0x%x\n", i, p_biquad_cell->b1);
                            NmfPrint2(0, "Ctx_filt_biq3LP_%d_b2     = 0x%x\n", i, p_biquad_cell->b2);
                            NmfPrint2(0, "Ctx_filt_biq3LP_%d_a1     = 0x%x\n", i, p_biquad_cell->a1);
                            NmfPrint2(0, "Ctx_filt_biq3LP_%d_a2     = 0x%x\n", i, p_biquad_cell->a2);
                            p_biquad_cell++;
                        }
                    }
                }
            }

            NmfPrint1(0, "Ctx_filt_biq3HP_order     = 0x%x\n",pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[3].biquadHP_processing_ctx.biquad_order);

            if(pMdrcCtx->pMdrcStruct->biquad_kind == BUTTERWORTH_BIQUAD)
            {
                if(pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[3].biquadHP_processing_ctx.p_biquad_mem[0] != NULL)
                {
                    t_biquad_cell_Butter YMEM *p_biquad_cell = (t_biquad_cell_Butter YMEM *) pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[3].biquadHP_processing_ctx.p_biquad_cell_void;
                    for(i=0; i<4; i++) {
                        if(pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[3].biquadHP_processing_ctx.biquad_order > i) {
                            NmfPrint2(0, "Ctx_filt_biq3HP_%d_bexp   = 0x%x\n", i, p_biquad_cell->b_exp);
                            NmfPrint2(0, "Ctx_filt_biq3HP_%d_b0     = 0x%x\n", i, p_biquad_cell->b0);
                            NmfPrint2(0, "Ctx_filt_biq3HP_%d_b1     = 0x%x\n", i, -p_biquad_cell->b0);
                            NmfPrint2(0, "Ctx_filt_biq3HP_%d_b2     = 0x%x\n", i, p_biquad_cell->b0);
                            NmfPrint2(0, "Ctx_filt_biq3HP_%d_a1     = 0x%x\n", i, p_biquad_cell->a1);
                            NmfPrint2(0, "Ctx_filt_biq3HP_%d_a2     = 0x%x\n", i, p_biquad_cell->a2);
                            p_biquad_cell++;
                        }
                    }
                }
            }
            else
            {
                if(pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[3].biquadHP_processing_ctx.p_biquad_mem[0] != NULL)
                {
                    t_biquad_cell YMEM *p_biquad_cell = (t_biquad_cell YMEM *) pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[3].biquadHP_processing_ctx.p_biquad_cell_void;
                    for(i=0; i<4; i++) {
                        if(pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[3].biquadHP_processing_ctx.biquad_order > i) {
                            NmfPrint2(0, "Ctx_filt_biq3HP_%d_bexp   = 0x%x\n", i, p_biquad_cell->b_exp);
                            NmfPrint2(0, "Ctx_filt_biq3HP_%d_b0     = 0x%x\n", i, p_biquad_cell->b0);
                            NmfPrint2(0, "Ctx_filt_biq3HP_%d_b1     = 0x%x\n", i, p_biquad_cell->b1);
                            NmfPrint2(0, "Ctx_filt_biq3HP_%d_b2     = 0x%x\n", i, p_biquad_cell->b2);
                            NmfPrint2(0, "Ctx_filt_biq3HP_%d_a1     = 0x%x\n", i, p_biquad_cell->a1);
                            NmfPrint2(0, "Ctx_filt_biq3HP_%d_a2     = 0x%x\n", i, p_biquad_cell->a2);
                            p_biquad_cell++;
                        }
                    }
                }
            }

            NmfPrint1(0, "Ctx_filt_biq4LPHP_comb    = 0x%x\n", pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[4].biquad_LP_HP_combination);
            NmfPrint1(0, "Ctx_filt_biq4LP_order     = 0x%x\n", pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[4].biquadLP_processing_ctx.biquad_order);

            if(pMdrcCtx->pMdrcStruct->biquad_kind == BUTTERWORTH_BIQUAD)
            {
                if(pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[4].biquadLP_processing_ctx.p_biquad_mem[0] != NULL)
                {
                    t_biquad_cell_Butter YMEM *p_biquad_cell = (t_biquad_cell_Butter YMEM *) pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[4].biquadLP_processing_ctx.p_biquad_cell_void;
                    for(i=0; i<4; i++) {
                        if(pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[4].biquadLP_processing_ctx.biquad_order > i) {
                            NmfPrint2(0, "Ctx_filt_biq4LP_%d_bexp   = 0x%x\n", i, p_biquad_cell->b_exp);
                            NmfPrint2(0, "Ctx_filt_biq4LP_%d_b0     = 0x%x\n", i, p_biquad_cell->b0);
                            NmfPrint2(0, "Ctx_filt_biq4LP_%d_b1     = 0x%x\n", i, p_biquad_cell->b0);
                            NmfPrint2(0, "Ctx_filt_biq4LP_%d_b2     = 0x%x\n", i, p_biquad_cell->b0);
                            NmfPrint2(0, "Ctx_filt_biq4LP_%d_a1     = 0x%x\n", i, p_biquad_cell->a1);
                            NmfPrint2(0, "Ctx_filt_biq4LP_%d_a2     = 0x%x\n", i, p_biquad_cell->a2);
                            p_biquad_cell++;
                        }
                    }
                }
            } else
            {
                if(pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[4].biquadLP_processing_ctx.p_biquad_mem[0] != NULL)
                {
                    t_biquad_cell YMEM *p_biquad_cell = (t_biquad_cell YMEM *) pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[4].biquadLP_processing_ctx.p_biquad_cell_void;
                    for(i=0; i<4; i++) {
                        if(pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[4].biquadLP_processing_ctx.biquad_order > i) {
                            NmfPrint2(0, "Ctx_filt_biq4LP_%d_bexp   = 0x%x\n", i, p_biquad_cell->b_exp);
                            NmfPrint2(0, "Ctx_filt_biq4LP_%d_b0     = 0x%x\n", i, p_biquad_cell->b0);
                            NmfPrint2(0, "Ctx_filt_biq4LP_%d_b1     = 0x%x\n", i, p_biquad_cell->b1);
                            NmfPrint2(0, "Ctx_filt_biq4LP_%d_b2     = 0x%x\n", i, p_biquad_cell->b2);
                            NmfPrint2(0, "Ctx_filt_biq4LP_%d_a1     = 0x%x\n", i, p_biquad_cell->a1);
                            NmfPrint2(0, "Ctx_filt_biq4LP_%d_a2     = 0x%x\n", i, p_biquad_cell->a2);
                            p_biquad_cell++;
                        }
                    }
                }
            }

            NmfPrint1(0, "Ctx_filt_biq4HP_order     = 0x%x\n",pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[4].biquadHP_processing_ctx.biquad_order);

            if(pMdrcCtx->pMdrcStruct->biquad_kind == BUTTERWORTH_BIQUAD)
            {
                if(pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[4].biquadHP_processing_ctx.p_biquad_mem[0] != NULL)
                {
                    t_biquad_cell_Butter YMEM *p_biquad_cell = (t_biquad_cell_Butter YMEM *) pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[4].biquadHP_processing_ctx.p_biquad_cell_void;
                    for(i=0; i<4; i++) {
                        if(pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[4].biquadHP_processing_ctx.biquad_order > i) {
                            NmfPrint2(0, "Ctx_filt_biq4HP_%d_bexp   = 0x%x\n", i, p_biquad_cell->b_exp);
                            NmfPrint2(0, "Ctx_filt_biq4HP_%d_b0     = 0x%x\n", i, p_biquad_cell->b0);
                            NmfPrint2(0, "Ctx_filt_biq4HP_%d_b1     = 0x%x\n", i, -p_biquad_cell->b0);
                            NmfPrint2(0, "Ctx_filt_biq4HP_%d_b2     = 0x%x\n", i, p_biquad_cell->b0);
                            NmfPrint2(0, "Ctx_filt_biq4HP_%d_a1     = 0x%x\n", i, p_biquad_cell->a1);
                            NmfPrint2(0, "Ctx_filt_biq4HP_%d_a2     = 0x%x\n", i, p_biquad_cell->a2);
                            p_biquad_cell++;
                        }
                    }
                }
            }
            else
            {
                if(pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[4].biquadHP_processing_ctx.p_biquad_mem[0] != NULL)
                {
                    t_biquad_cell YMEM *p_biquad_cell = (t_biquad_cell YMEM *) pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[4].biquadHP_processing_ctx.p_biquad_cell_void;
                    for(i=0; i<4; i++) {
                        if(pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[4].biquadHP_processing_ctx.biquad_order > i) {
                            NmfPrint2(0, "Ctx_filt_biq4HP_%d_bexp   = 0x%x\n", i, p_biquad_cell->b_exp);
                            NmfPrint2(0, "Ctx_filt_biq4HP_%d_b0     = 0x%x\n", i, p_biquad_cell->b0);
                            NmfPrint2(0, "Ctx_filt_biq4HP_%d_b1     = 0x%x\n", i, p_biquad_cell->b1);
                            NmfPrint2(0, "Ctx_filt_biq4HP_%d_b2     = 0x%x\n", i, p_biquad_cell->b2);
                            NmfPrint2(0, "Ctx_filt_biq4HP_%d_a1     = 0x%x\n", i, p_biquad_cell->a1);
                            NmfPrint2(0, "Ctx_filt_biq4HP_%d_a2     = 0x%x\n", i, p_biquad_cell->a2);
                            p_biquad_cell++;
                        }
                    }
                }
            }
        }
        if(pMdrcCtx->pMdrcStruct->mdrc_filter_kind == MDRC_FIR_FILTER)
        {
            for(i=0; i<5; i++) {

                NmfPrint2(0, "Ctx_filt_FIR%d_kind        = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR_kind);
                NmfPrint2(0, "Ctx_filt_FIR%d_order       = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.order);
                NmfPrint2(0, "Ctx_filt_FIR%d_gain_exp    = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.gain_exp);
                NmfPrint2(0, "Ctx_filt_FIR%d_coef000     = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  0]);
                NmfPrint2(0, "Ctx_filt_FIR%d_coef001     = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  1]);
                NmfPrint2(0, "Ctx_filt_FIR%d_coef002     = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  2]);
                NmfPrint2(0, "Ctx_filt_FIR%d_coef003     = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  3]);
                NmfPrint2(0, "Ctx_filt_FIR%d_coef004     = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  4]);
                NmfPrint2(0, "Ctx_filt_FIR%d_coef005     = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  5]);
                NmfPrint2(0, "Ctx_filt_FIR%d_coef006     = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  6]);
                NmfPrint2(0, "Ctx_filt_FIR%d_coef007     = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  7]);
                NmfPrint2(0, "Ctx_filt_FIR%d_coef008     = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  8]);
                NmfPrint2(0, "Ctx_filt_FIR%d_coef009     = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  9]);
                NmfPrint2(0, "Ctx_filt_FIR%d_coef0010    = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  10]);
                NmfPrint2(0, "Ctx_filt_FIR%d_coef0011    = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  11]);
                NmfPrint2(0, "Ctx_filt_FIR%d_coef0012    = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  12]);
                NmfPrint2(0, "Ctx_filt_FIR%d_coef0013    = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  13]);
                NmfPrint2(0, "Ctx_filt_FIR%d_coef0014    = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  14]);
                NmfPrint2(0, "Ctx_filt_FIR%d_coef0015    = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  15]);
                NmfPrint2(0, "Ctx_filt_FIR%d_coef0016    = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  16]);
                NmfPrint2(0, "Ctx_filt_FIR%d_coef0017    = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  17]);
                NmfPrint2(0, "Ctx_filt_FIR%d_coef0018    = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  18]);
                NmfPrint2(0, "Ctx_filt_FIR%d_coef0019    = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  19]);
                NmfPrint2(0, "Ctx_filt_FIR%d_coef0020    = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  20]);
                NmfPrint2(0, "Ctx_filt_FIR%d_coef0021    = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  21]);
                NmfPrint2(0, "Ctx_filt_FIR%d_coef0022    = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  22]);
                NmfPrint2(0, "Ctx_filt_FIR%d_coef0023    = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  23]);
                NmfPrint2(0, "Ctx_filt_FIR%d_coef0024    = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  24]);
                NmfPrint2(0, "Ctx_filt_FIR%d_coef0025    = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  25]);
                NmfPrint2(0, "Ctx_filt_FIR%d_coef0026    = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  26]);
                NmfPrint2(0, "Ctx_filt_FIR%d_coef0027    = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  27]);
                NmfPrint2(0, "Ctx_filt_FIR%d_coef0028    = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  28]);
                NmfPrint2(0, "Ctx_filt_FIR%d_coef0029    = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  29]);
                NmfPrint2(0, "Ctx_filt_FIR%d_coef0030    = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  30]);
                NmfPrint2(0, "Ctx_filt_FIR%d_coef0031    = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  31]);
                NmfPrint2(0, "Ctx_filt_FIR%d_coef0032    = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  32]);
                NmfPrint2(0, "Ctx_filt_FIR%d_coef0033    = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  33]);
                NmfPrint2(0, "Ctx_filt_FIR%d_coef0034    = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  34]);
                NmfPrint2(0, "Ctx_filt_FIR%d_coef0035    = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  35]);
                NmfPrint2(0, "Ctx_filt_FIR%d_coef0036    = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  36]);
                NmfPrint2(0, "Ctx_filt_FIR%d_coef0037    = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  37]);
                NmfPrint2(0, "Ctx_filt_FIR%d_coef0038    = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  38]);
                NmfPrint2(0, "Ctx_filt_FIR%d_coef0039    = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  39]);
                NmfPrint2(0, "Ctx_filt_FIR%d_coef0040    = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  40]);

                if(pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[0].FIR_kind == OTHER_FIR)
                {
                    NmfPrint2(0, "Ctx_filt_FIR%d_coef0041    = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  41]);
                    NmfPrint2(0, "Ctx_filt_FIR%d_coef0042    = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  42]);
                    NmfPrint2(0, "Ctx_filt_FIR%d_coef0043    = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  43]);
                    NmfPrint2(0, "Ctx_filt_FIR%d_coef0044    = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  44]);
                    NmfPrint2(0, "Ctx_filt_FIR%d_coef0045    = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  45]);
                    NmfPrint2(0, "Ctx_filt_FIR%d_coef0046    = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  46]);
                    NmfPrint2(0, "Ctx_filt_FIR%d_coef0047    = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  47]);
                    NmfPrint2(0, "Ctx_filt_FIR%d_coef0048    = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  48]);
                    NmfPrint2(0, "Ctx_filt_FIR%d_coef0049    = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  49]);
                    NmfPrint2(0, "Ctx_filt_FIR%d_coef0050    = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  50]);
                    NmfPrint2(0, "Ctx_filt_FIR%d_coef0051    = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  51]);
                    NmfPrint2(0, "Ctx_filt_FIR%d_coef0052    = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  52]);
                    NmfPrint2(0, "Ctx_filt_FIR%d_coef0053    = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  53]);
                    NmfPrint2(0, "Ctx_filt_FIR%d_coef0054    = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  54]);
                    NmfPrint2(0, "Ctx_filt_FIR%d_coef0055    = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  55]);
                    NmfPrint2(0, "Ctx_filt_FIR%d_coef0056    = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  56]);
                    NmfPrint2(0, "Ctx_filt_FIR%d_coef0057    = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  57]);
                    NmfPrint2(0, "Ctx_filt_FIR%d_coef0058    = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  58]);
                    NmfPrint2(0, "Ctx_filt_FIR%d_coef0059    = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  59]);
                    NmfPrint2(0, "Ctx_filt_FIR%d_coef0060    = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  60]);
                    NmfPrint2(0, "Ctx_filt_FIR%d_coef0061    = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  61]);
                    NmfPrint2(0, "Ctx_filt_FIR%d_coef0062    = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  62]);
                    NmfPrint2(0, "Ctx_filt_FIR%d_coef0063    = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  63]);
                    NmfPrint2(0, "Ctx_filt_FIR%d_coef0064    = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  64]);
                    NmfPrint2(0, "Ctx_filt_FIR%d_coef0065    = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  65]);
                    NmfPrint2(0, "Ctx_filt_FIR%d_coef0066    = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  66]);
                    NmfPrint2(0, "Ctx_filt_FIR%d_coef0067    = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  67]);
                    NmfPrint2(0, "Ctx_filt_FIR%d_coef0068    = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  68]);
                    NmfPrint2(0, "Ctx_filt_FIR%d_coef0069    = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  69]);
                    NmfPrint2(0, "Ctx_filt_FIR%d_coef0070    = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  70]);
                    NmfPrint2(0, "Ctx_filt_FIR%d_coef0071    = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  71]);
                    NmfPrint2(0, "Ctx_filt_FIR%d_coef0072    = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  72]);
                    NmfPrint2(0, "Ctx_filt_FIR%d_coef0073    = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  73]);
                    NmfPrint2(0, "Ctx_filt_FIR%d_coef0074    = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  74]);
                    NmfPrint2(0, "Ctx_filt_FIR%d_coef0075    = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  75]);
                    NmfPrint2(0, "Ctx_filt_FIR%d_coef0076    = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  76]);
                    NmfPrint2(0, "Ctx_filt_FIR%d_coef0077    = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  77]);
                    NmfPrint2(0, "Ctx_filt_FIR%d_coef0078    = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  78]);
                    NmfPrint2(0, "Ctx_filt_FIR%d_coef0079    = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  79]);
                    NmfPrint2(0, "Ctx_filt_FIR%d_coef0080    = 0x%x\n", i, pMdrcCtx->pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef[  80]);
                }
            }
        }
    }

    NmfPrint1(0, "Ctx_ComPostGaindB0        = 0x%x\n", pMdrcCtx->pMdrcStruct->ComPostGaindB[0]);
    NmfPrint1(0, "Ctx_ComPostGaindB1        = 0x%x\n", pMdrcCtx->pMdrcStruct->ComPostGaindB[1]);
    NmfPrint1(0, "Ctx_ComPostGaindB2        = 0x%x\n", pMdrcCtx->pMdrcStruct->ComPostGaindB[2]);
    NmfPrint1(0, "Ctx_ComPostGaindB3        = 0x%x\n", pMdrcCtx->pMdrcStruct->ComPostGaindB[3]);
    NmfPrint1(0, "Ctx_ComPostGaindB4        = 0x%x\n", pMdrcCtx->pMdrcStruct->ComPostGaindB[4]);

    for(i=0; i<5; i++) {
        NmfPrint2(0, "Ctx_ComThreshOrig%d_0_0   = 0x%x\n", i, pMdrcCtx->pMdrcStruct->ComThreshdBOrig[i][0][0]);
        NmfPrint2(0, "Ctx_ComThreshOrig%d_0_1   = 0x%x\n", i, pMdrcCtx->pMdrcStruct->ComThreshdBOrig[i][0][1]);
        NmfPrint2(0, "Ctx_ComThreshOrig%d_1_0   = 0x%x\n", i, pMdrcCtx->pMdrcStruct->ComThreshdBOrig[i][1][0]);
        NmfPrint2(0, "Ctx_ComThreshOrig%d_1_1   = 0x%x\n", i, pMdrcCtx->pMdrcStruct->ComThreshdBOrig[i][1][1]);
        NmfPrint2(0, "Ctx_ComThreshOrig%d_2_0   = 0x%x\n", i, pMdrcCtx->pMdrcStruct->ComThreshdBOrig[i][2][0]);
        NmfPrint2(0, "Ctx_ComThreshOrig%d_2_1   = 0x%x\n", i, pMdrcCtx->pMdrcStruct->ComThreshdBOrig[i][2][1]);
        NmfPrint2(0, "Ctx_ComThreshOrig%d_3_0   = 0x%x\n", i, pMdrcCtx->pMdrcStruct->ComThreshdBOrig[i][3][0]);
        NmfPrint2(0, "Ctx_ComThreshOrig%d_3_1   = 0x%x\n", i, pMdrcCtx->pMdrcStruct->ComThreshdBOrig[i][3][1]);
        NmfPrint2(0, "Ctx_ComThreshOrig%d_4_0   = 0x%x\n", i, pMdrcCtx->pMdrcStruct->ComThreshdBOrig[i][4][0]);
        NmfPrint2(0, "Ctx_ComThreshOrig%d_4_1   = 0x%x\n", i, pMdrcCtx->pMdrcStruct->ComThreshdBOrig[i][4][1]);
        NmfPrint2(0, "Ctx_ComThreshOrig%d_5_0   = 0x%x\n", i, pMdrcCtx->pMdrcStruct->ComThreshdBOrig[i][5][0]);
        NmfPrint2(0, "Ctx_ComThreshOrig%d_5_1   = 0x%x\n", i, pMdrcCtx->pMdrcStruct->ComThreshdBOrig[i][5][1]);
        NmfPrint2(0, "Ctx_ComThreshOrig%d_6_0   = 0x%x\n", i, pMdrcCtx->pMdrcStruct->ComThreshdBOrig[i][6][0]);
        NmfPrint2(0, "Ctx_ComThreshOrig%d_6_1   = 0x%x\n", i, pMdrcCtx->pMdrcStruct->ComThreshdBOrig[i][6][1]);
        NmfPrint2(0, "Ctx_ComThreshOrig%d_7_0   = 0x%x\n", i, pMdrcCtx->pMdrcStruct->ComThreshdBOrig[i][7][0]);
        NmfPrint2(0, "Ctx_ComThreshOrig%d_7_1   = 0x%x\n", i, pMdrcCtx->pMdrcStruct->ComThreshdBOrig[i][7][1]);
        NmfPrint2(0, "Ctx_ComThreshOrig%d_8_0   = 0x%x\n", i, pMdrcCtx->pMdrcStruct->ComThreshdBOrig[i][8][0]);
        NmfPrint2(0, "Ctx_ComThreshOrig%d_8_1   = 0x%x\n", i, pMdrcCtx->pMdrcStruct->ComThreshdBOrig[i][8][1]);
    }

    for(i=0; i<5; i++) {
        NmfPrint2(0, "Ctx_ComThresh%d_0_0       = 0x%x\n", i, pMdrcCtx->pMdrcStruct->ComThreshdB[0][0][0]);
        NmfPrint2(0, "Ctx_ComThresh%d_0_1       = 0x%x\n", i, pMdrcCtx->pMdrcStruct->ComThreshdB[0][0][1]);
        NmfPrint2(0, "Ctx_ComThresh%d_1_0       = 0x%x\n", i, pMdrcCtx->pMdrcStruct->ComThreshdB[0][1][0]);
        NmfPrint2(0, "Ctx_ComThresh%d_1_1       = 0x%x\n", i, pMdrcCtx->pMdrcStruct->ComThreshdB[0][1][1]);
        NmfPrint2(0, "Ctx_ComThresh%d_2_0       = 0x%x\n", i, pMdrcCtx->pMdrcStruct->ComThreshdB[0][2][0]);
        NmfPrint2(0, "Ctx_ComThresh%d_2_1       = 0x%x\n", i, pMdrcCtx->pMdrcStruct->ComThreshdB[0][2][1]);
        NmfPrint2(0, "Ctx_ComThresh%d_3_0       = 0x%x\n", i, pMdrcCtx->pMdrcStruct->ComThreshdB[0][3][0]);
        NmfPrint2(0, "Ctx_ComThresh%d_3_1       = 0x%x\n", i, pMdrcCtx->pMdrcStruct->ComThreshdB[0][3][1]);
        NmfPrint2(0, "Ctx_ComThresh%d_4_0       = 0x%x\n", i, pMdrcCtx->pMdrcStruct->ComThreshdB[0][4][0]);
        NmfPrint2(0, "Ctx_ComThresh%d_4_1       = 0x%x\n", i, pMdrcCtx->pMdrcStruct->ComThreshdB[0][4][1]);
        NmfPrint2(0, "Ctx_ComThresh%d_5_0       = 0x%x\n", i, pMdrcCtx->pMdrcStruct->ComThreshdB[0][5][0]);
        NmfPrint2(0, "Ctx_ComThresh%d_5_1       = 0x%x\n", i, pMdrcCtx->pMdrcStruct->ComThreshdB[0][5][1]);
        NmfPrint2(0, "Ctx_ComThresh%d_6_0       = 0x%x\n", i, pMdrcCtx->pMdrcStruct->ComThreshdB[0][6][0]);
        NmfPrint2(0, "Ctx_ComThresh%d_6_1       = 0x%x\n", i, pMdrcCtx->pMdrcStruct->ComThreshdB[0][6][1]);
        NmfPrint2(0, "Ctx_ComThresh%d_7_0       = 0x%x\n", i, pMdrcCtx->pMdrcStruct->ComThreshdB[0][7][0]);
        NmfPrint2(0, "Ctx_ComThresh%d_7_1       = 0x%x\n", i, pMdrcCtx->pMdrcStruct->ComThreshdB[0][7][1]);
        NmfPrint2(0, "Ctx_ComThresh%d_8_0       = 0x%x\n", i, pMdrcCtx->pMdrcStruct->ComThreshdB[0][8][0]);
        NmfPrint2(0, "Ctx_ComThresh%d_8_1       = 0x%x\n", i, pMdrcCtx->pMdrcStruct->ComThreshdB[0][8][1]);
    }

    for(i=0; i<5; i++) {
        NmfPrint2(0, "Ctx_ComSlope%d_0          = 0x%x\n", i, pMdrcCtx->pMdrcStruct->ComSlope[i][0]);
        NmfPrint2(0, "Ctx_ComSlope%d_1          = 0x%x\n", i, pMdrcCtx->pMdrcStruct->ComSlope[i][1]);
        NmfPrint2(0, "Ctx_ComSlope%d_2          = 0x%x\n", i, pMdrcCtx->pMdrcStruct->ComSlope[i][2]);
        NmfPrint2(0, "Ctx_ComSlope%d_3          = 0x%x\n", i, pMdrcCtx->pMdrcStruct->ComSlope[i][3]);
        NmfPrint2(0, "Ctx_ComSlope%d_4          = 0x%x\n", i, pMdrcCtx->pMdrcStruct->ComSlope[i][4]);
        NmfPrint2(0, "Ctx_ComSlope%d_5          = 0x%x\n", i, pMdrcCtx->pMdrcStruct->ComSlope[i][5]);
        NmfPrint2(0, "Ctx_ComSlope%d_6          = 0x%x\n", i, pMdrcCtx->pMdrcStruct->ComSlope[i][6]);
        NmfPrint2(0, "Ctx_ComSlope%d_7          = 0x%x\n", i, pMdrcCtx->pMdrcStruct->ComSlope[i][7]);
        NmfPrint2(0, "Ctx_ComSlope%d_8          = 0x%x\n", i, pMdrcCtx->pMdrcStruct->ComSlope[i][8]);
    }

    for(i=0; i<5; i++) {
        NmfPrint2(0, "Ctx_RmsAtCoef%d           = 0x%x\n", i, pMdrcCtx->pMdrcStruct->RmsAtCoef[i]);
    }
    for(i=0; i<5; i++) {
        NmfPrint2(0, "Ctx_RmsReCoef%d           = 0x%x\n", i, pMdrcCtx->pMdrcStruct->RmsReCoef[i]);
    }
    for(i=0; i<5; i++) {
        NmfPrint2(0, "Ctx_ComAtCoef%d           = 0x%x\n", i, pMdrcCtx->pMdrcStruct->ComAtCoef[i]);
    }
    for(i=0; i<5; i++) {
        NmfPrint2(0, "Ctx_ComReCoef%d           = 0x%x\n", i, pMdrcCtx->pMdrcStruct->ComReCoef[i]);
    }
    for(i=0; i<5; i++) {
        NmfPrint2(0, "Ctx_ComHoldRem%d          = 0x%x\n", i, pMdrcCtx->pMdrcStruct->ComHoldRem[i]);
    }
    for(i=0; i<5; i++) {
        NmfPrint2(0, "Ctx_HtSamp%d              = 0x%x\n", i, pMdrcCtx->pMdrcStruct->HtSamp[i]);
    }
#endif // MDRC5B_COMPRESSOR_ACTIVE

#ifdef MDRC5B_LIMITER_ACTIVE
    NmfPrint1(0, "Ctx_Limiter_Enable        = 0x%x\n", pMdrcCtx->pMdrcStruct->Limiter.Enable);
    NmfPrint1(0, "Ctx_Limiter_RmsMeasure    = 0x%x\n", pMdrcCtx->pMdrcStruct->Limiter.RmsMeasure);
    NmfPrint1(0, "Ctx_Limiter_Thresh        = 0x%x\n", pMdrcCtx->pMdrcStruct->Limiter.Thresh);
    NmfPrint1(0, "Ctx_LimiterLaIdx          = 0x%x\n", pMdrcCtx->pMdrcStruct->LimiterLaIdx);
    NmfPrint1(0, "Ctx_LimiterPeakAtCoef     = 0x%x\n", pMdrcCtx->pMdrcStruct->LimiterPeakAtCoef);
    NmfPrint1(0, "Ctx_LimiterPeakReCoef     = 0x%x\n", pMdrcCtx->pMdrcStruct->LimiterPeakReCoef);
    NmfPrint1(0, "Ctx_LimiterAtCoef         = 0x%x\n", pMdrcCtx->pMdrcStruct->LimiterAtCoef);
    NmfPrint1(0, "Ctx_LimiterReCoef         = 0x%x\n", pMdrcCtx->pMdrcStruct->LimiterReCoef);
    NmfPrint1(0, "Ctx_LimiterThreshdB       = 0x%x\n", pMdrcCtx->pMdrcStruct->LimiterThreshdB);
    NmfPrint1(0, "Ctx_LimiterGainMant       = 0x%x\n", pMdrcCtx->pMdrcStruct->LimiterGainMant);
    NmfPrint1(0, "Ctx_LimiterGainExp        = 0x%x\n", pMdrcCtx->pMdrcStruct->LimiterGainExp);
    NmfPrint1(0, "Ctx_LimiterHoldRem        = 0x%x\n", pMdrcCtx->pMdrcStruct->LimiterHoldRem);
    NmfPrint1(0, "Ctx_LimiterHtSamp         = 0x%x\n", pMdrcCtx->pMdrcStruct->LimiterHtSamp);
    NmfPrint1(0, "Ctx_PrevShiftBits         = 0x%x\n", pMdrcCtx->pMdrcStruct->PrevShiftBits);
#endif // MDRC5B_LIMITER_ACTIVE
    NmfPrint1(0, "Ctx_processing            = 0x%x\n", pMdrcCtx->pMdrcStruct->processing);

    NmfPrint0(0, "********************************************\n");
}

#endif // DEBUG_MDRC_DSP_CONFIG

///////////////////////////////////////////////////////////////////////////////////

static int convert_lowhigh2MMshort(t_uint16 low, t_uint16 high)
{
    return ((int) (low & 0xFFFF)) | (((int) high) << 16);
}

static void local_setConfig(MdrcWrpCtx_t *pMdrcCtx)
{
    MDRC5B_LOCAL_STRUCT_T *pMdrcStruct = pMdrcCtx->pMdrcStruct;
    MdrcConfig_t *pMdrcConfig = pMdrcCtx->pMdrcConfig;
    Mdrc5bAlgoConfig_t *pMdrcAlgoConfig = pMdrcCtx->pMdrcAlgoConfig;
    MdrcFiltersConfig_t *p_mdrc_filters_config = &pMdrcConfig->MdrcFiltersConfig;
    t_mdrc_filters      *p_mdrc_filters        = pMdrcStruct->p_mdrc_filters;

    PRECONDITION(pMdrcCtx->allocDone == true);

    // copy the data to local config
    pMdrcAlgoConfig->Mode     = (int) pMdrcConfig->mMode;
    pMdrcAlgoConfig->NumBands = (int) pMdrcConfig->mNumBands;

    if(pMdrcAlgoConfig->Mode & COMPRESSOR)
    {
        int i, j;

        switch(pMdrcStruct->mdrc_filter_kind)
        {
            case MDRC_LEGACY_FILTER:
                break;
            case MDRC_BIQUAD_FILTER:
            case MDRC_FIR_FILTER:
                if(get_mdrc_filter_config(p_mdrc_filters,
                            p_mdrc_filters_config,
                            pMdrcStruct->mdrc_filter_kind,
                            pMdrcStruct->biquad_kind,
                            pMdrcStruct->FIR_kind) != 0)
                {
                    ASSERT(0);
                }
                break;
            default:
                ASSERT(0);
        }

        // copy bands setting into local structure
        for(i = 0; i < pMdrcAlgoConfig->NumBands; i++)
        {
            pMdrcAlgoConfig->BandCompressors[i].Enable       = (int) pMdrcConfig->mBandCompressors[i].bEnable;
            pMdrcAlgoConfig->BandCompressors[i].KneePoints   = (int) pMdrcConfig->mBandCompressors[i].mKneePoints;
            pMdrcAlgoConfig->BandCompressors[i].FreqCutoff   = convert_lowhigh2MMshort(pMdrcConfig->mBandCompressors[i].mFreqCutoff_low,  pMdrcConfig->mBandCompressors[i].mFreqCutoff_high);
            pMdrcAlgoConfig->BandCompressors[i].PostGain     = convert_lowhigh2MMshort(pMdrcConfig->mBandCompressors[i].mPostGain_low,    pMdrcConfig->mBandCompressors[i].mPostGain_high);
            pMdrcAlgoConfig->BandCompressors[i].AttackTime   = convert_lowhigh2MMshort(pMdrcConfig->mBandCompressors[i].mAttackTime_low,  pMdrcConfig->mBandCompressors[i].mAttackTime_high);
            pMdrcAlgoConfig->BandCompressors[i].ReleaseTime  = convert_lowhigh2MMshort(pMdrcConfig->mBandCompressors[i].mReleaseTime_low, pMdrcConfig->mBandCompressors[i].mReleaseTime_high);

            for(j = 0; j < pMdrcAlgoConfig->BandCompressors[i].KneePoints; j++)
            {
                pMdrcAlgoConfig->BandCompressors[i].DynamicResponse[j][0] = ((((int) pMdrcConfig->mBandCompressors[i].mDynamicResponse[j][0]) << 8) >> 8);
                pMdrcAlgoConfig->BandCompressors[i].DynamicResponse[j][1] = ((((int) pMdrcConfig->mBandCompressors[i].mDynamicResponse[j][1]) << 8) >> 8);
            }
        }
        for(; i < MDRC5B_SUBBAND_MAX; i++)
        {
            pMdrcAlgoConfig->BandCompressors[i].Enable = (int) FALSE;
        }
    }

    if(pMdrcAlgoConfig->Mode & LIMITER)
    {
        pMdrcAlgoConfig->Limiter.Enable     = (int) pMdrcConfig->mLimiter.bEnable;
        pMdrcAlgoConfig->Limiter.RmsMeasure = (int) pMdrcConfig->mLimiter.mRmsMeasure;
        pMdrcAlgoConfig->Limiter.Thresh     = convert_lowhigh2MMshort(pMdrcConfig->mLimiter.mThresh_low, pMdrcConfig->mLimiter.mThresh_high);
    }

    ASSERT(applyEffectConfig(pMdrcStruct, pMdrcAlgoConfig) == 0);

#ifdef DEBUG_MDRC_DSP_CONFIG
    printSetConfigDebugInfo(pMdrcCtx);
#endif // DEBUG_MDRC_DSP_CONFIG
}

static void allocateAndInitializeMDRCcoeffs(MdrcWrpCtx_t *pMdrcCtx)
{
    TRACE_t * this = pMdrcCtx->pTraceObject;
    MDRC5B_LOCAL_STRUCT_T *pMdrcStruct = pMdrcCtx->pMdrcStruct;
    t_ExtendedAllocParams pAllocParams;
    void *p_xmem = NULL;
    void YMEM *p_ymem = NULL;
    int i, j;

    PRECONDITION(pMdrcCtx->allocDone == false);

    pAllocParams.trace_p = this;
    pAllocParams.bank = MEM_DDR24;

    pMdrcStruct->p_filter_buffer = (t_filter_buffer *) vmalloc(sizeof(t_filter_buffer), &pAllocParams);
    ASSERT(pMdrcStruct->p_filter_buffer != NULL);

    switch(pMdrcStruct->mdrc_filter_kind)
    {
        case MDRC_LEGACY_FILTER:
            {
                pAllocParams.bank = MEM_YTCM;
                pMdrcStruct->p_filter_coef = (t_filter_coef YMEM *) vmalloc_y(sizeof(t_filter_coef), &pAllocParams);
                ASSERT(pMdrcStruct->p_filter_coef != NULL);

                pAllocParams.bank = MEM_XTCM;
                pMdrcStruct->p_filter_mem = (t_filter_mem *) vmalloc(sizeof(t_filter_mem), &pAllocParams);
                ASSERT(pMdrcStruct->p_filter_mem != NULL);

                pMdrcStruct->p_mdrc_filters = NULL;
            }
            break;

        case MDRC_BIQUAD_FILTER:
            {
                pAllocParams.bank = MEM_DDR24;
                pMdrcStruct->p_mdrc_filters = (t_mdrc_filters *) vmalloc(sizeof(t_mdrc_filters), &pAllocParams);
                ASSERT(pMdrcStruct->p_mdrc_filters != NULL);

                pMdrcStruct->p_filter_coef                    = NULL;
                pMdrcStruct->p_filter_mem                     = NULL;
                pMdrcStruct->p_mdrc_filters->mdrc_filter_kind = pMdrcStruct->mdrc_filter_kind;
                pMdrcStruct->p_mdrc_filters->nb_channels      = pMdrcStruct->NumMainCh;

                for(i = 0; i < MDRC_BANDS_MAX; i++)
                {
                    pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[i].biquadLP_processing_ctx.p_biquad_cell_void = NULL;
                    pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[i].biquadHP_processing_ctx.p_biquad_cell_void = NULL;
                    pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[i].biquadLP_processing_ctx.p_biquad_mem[0]    = NULL;
                    pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[i].biquadLP_processing_ctx.p_biquad_mem[1]    = NULL;
                    pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[i].biquadHP_processing_ctx.p_biquad_mem[0]    = NULL;
                    pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[i].biquadHP_processing_ctx.p_biquad_mem[1]    = NULL;
                }

                for(i = 0; i < MDRC_BANDS_MAX; i++)
                {
                    switch(pMdrcStruct->biquad_kind)
                    {
                        case BUTTERWORTH_BIQUAD:
                            {
                                if(i < MDRC_BANDS_MAX - 1)
                                {
                                    pAllocParams.bank = MEM_YTCM;
                                    p_ymem = vmalloc_y(MDRC_BIQUAD_ORDER_MAX * sizeof(t_biquad_cell_Butter), &pAllocParams);
                                    ASSERT(p_ymem != NULL);
                                    pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[i].biquadLP_processing_ctx.p_biquad_cell_void = p_ymem;
                                }
                                if(i > 0)
                                {
                                    pAllocParams.bank = MEM_YTCM;
                                    p_ymem = vmalloc_y(MDRC_BIQUAD_ORDER_MAX * sizeof(t_biquad_cell_Butter), &pAllocParams);
                                    ASSERT(p_ymem != NULL);
                                    pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[i].biquadHP_processing_ctx.p_biquad_cell_void = p_ymem;
                                }
                            }
                            break;
                        case OTHER_BIQUAD:
                            {
                                if(i < MDRC_BANDS_MAX - 1)
                                {
                                    pAllocParams.bank = MEM_YTCM;
                                    p_ymem = vmalloc_y(MDRC_BIQUAD_ORDER_MAX * sizeof(t_biquad_cell), &pAllocParams);
                                    ASSERT(p_ymem != NULL);
                                    pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[i].biquadLP_processing_ctx.p_biquad_cell_void = p_ymem;
                                }
                                if(i > 0)
                                {
                                    pAllocParams.bank = MEM_YTCM;
                                    p_ymem = vmalloc_y(MDRC_BIQUAD_ORDER_MAX * sizeof(t_biquad_cell), &pAllocParams);
                                    ASSERT(p_ymem != NULL);
                                    pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[i].biquadHP_processing_ctx.p_biquad_cell_void = p_ymem;
                                }
                            }
                            break;
                        default:
                            break;
                    }

                    for(j = 0; j < pMdrcStruct->NumMainCh; j++)
                    {
                        if(i < MDRC_BANDS_MAX - 1)
                        {
                            pAllocParams.bank = MEM_XTCM;
                            p_xmem = vmalloc((2 * MDRC_BIQUAD_ORDER_MAX + 2) * sizeof(MMshort), &pAllocParams);
                            ASSERT(p_xmem != NULL);
                            pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[i].biquadLP_processing_ctx.p_biquad_mem[j] = (MMshort *) p_xmem;
                        }
                        if(i > 0)
                        {
                            pAllocParams.bank = MEM_XTCM;
                            p_xmem = vmalloc((2 * MDRC_BIQUAD_ORDER_MAX + 2) * sizeof(MMshort), &pAllocParams);
                            ASSERT(p_xmem != NULL);
                            pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[i].biquadHP_processing_ctx.p_biquad_mem[j] = (MMshort *) p_xmem;
                        }
                    }
                }
            }
            break;

        case MDRC_FIR_FILTER:
            {
                pAllocParams.bank = MEM_DDR24;
                pMdrcStruct->p_mdrc_filters = (t_mdrc_filters *) vmalloc(sizeof(t_mdrc_filters), &pAllocParams);
                ASSERT(pMdrcStruct->p_mdrc_filters != NULL);

                pMdrcStruct->p_filter_coef                    = NULL;
                pMdrcStruct->p_filter_mem                     = NULL;
                pMdrcStruct->p_mdrc_filters->mdrc_filter_kind = pMdrcStruct->mdrc_filter_kind;
                pMdrcStruct->p_mdrc_filters->nb_channels      = pMdrcStruct->NumMainCh;

                for(i = 0; i < MDRC_BANDS_MAX; i++)
                {
                    pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef = NULL;
                }
                pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.p_FIR_common_mem[0]    = winit_circ_ymem_ptr(NULL, NULL, NULL);
                pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.p_FIR_common_mem[1]    = winit_circ_ymem_ptr(NULL, NULL, NULL);
                pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.p_FIR_common_mem_optim = NULL;

                for(i = 0; i < MDRC_BANDS_MAX; i++)
                {
                    switch(pMdrcStruct->FIR_kind)
                    {
                        case SYMMETRIC_FIR:
                            pAllocParams.bank = MEM_XTCM;
                            p_xmem = vmalloc((MDRC_FIR_ORDER_MAX + 1) / 2 * sizeof(MMshort), &pAllocParams);
                            break;
                        case OTHER_FIR:
                            pAllocParams.bank = MEM_XTCM;
                            p_xmem = vmalloc(MDRC_FIR_ORDER_MAX * sizeof(MMshort), &pAllocParams);
                            break;
                        default:
                            break;
                    }
                    ASSERT(p_xmem != NULL);
                    pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef = (MMshort *) p_xmem;
                }
                for(j = 0; j < pMdrcStruct->NumMainCh; j++)
                {
                    MMshort YMEM *p_FIR_mem = NULL;
                    pAllocParams.bank = MEM_YTCM;
                    p_FIR_mem = (MMshort YMEM *)vmalloc_y((MDRC_FIR_ORDER_MAX + MDRC5B_BLOCK_SIZE) * sizeof(MMshort), &pAllocParams);
                    ASSERT(p_FIR_mem != NULL);
                    pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.p_FIR_common_mem[j] = winit_circ_ymem_ptr(p_FIR_mem, p_FIR_mem, p_FIR_mem + MDRC_FIR_ORDER_MAX + MDRC5B_BLOCK_SIZE);
                }

                pAllocParams.bank = MEM_YTCM;
                p_ymem = vmalloc_y((MDRC_FIR_ORDER_MAX + 1) / 2 * sizeof(MMshort), &pAllocParams);
                ASSERT(p_ymem != NULL);
                pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.p_FIR_common_mem_optim = (MMshort YMEM *) p_ymem;
            }
            break;

        default:
            break;
    }

    pMdrcCtx->allocDone = true;

    OstTraceFiltInst2(TRACE_DEBUG,"mdrcmpc::allocation of MDRC coeffs done (ctx=0x%x) FilterKind=%d", (unsigned int)pMdrcCtx, pMdrcStruct->mdrc_filter_kind);

    if(pMdrcCtx->configReceived) {
        pMdrcCtx->configReceived = false;
        local_setConfig(pMdrcCtx);
    }
}

static void deallocateMDRCcoeffs(MdrcWrpCtx_t *pMdrcCtx)
{
    TRACE_t * this = pMdrcCtx->pTraceObject;
    MDRC5B_LOCAL_STRUCT_T *pMdrcStruct = pMdrcCtx->pMdrcStruct;
    t_ExtendedAllocParams pAllocParams;
    int i, j;

    PRECONDITION(pMdrcCtx->allocDone == true);

    pAllocParams.trace_p = this;

    switch(pMdrcStruct->mdrc_filter_kind)
    {
        case MDRC_LEGACY_FILTER:
            {
                pAllocParams.bank = MEM_YTCM;
                vfree(pMdrcStruct->p_filter_coef, &pAllocParams);
                pMdrcStruct->p_filter_coef = NULL;

                pAllocParams.bank = MEM_XTCM;
                vfree(pMdrcStruct->p_filter_mem, &pAllocParams);
                pMdrcStruct->p_filter_mem = NULL;
            }
            break;

        case MDRC_BIQUAD_FILTER:
            {

                for(i = 0; i < MDRC_BANDS_MAX; i++)
                {
                    switch(pMdrcStruct->biquad_kind)
                    {
                        case BUTTERWORTH_BIQUAD:
                            {
                                if(i < MDRC_BANDS_MAX - 1)
                                {
                                    pAllocParams.bank = MEM_YTCM;
                                    vfree(pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[i].biquadLP_processing_ctx.p_biquad_cell_void, &pAllocParams);
                                    pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[i].biquadLP_processing_ctx.p_biquad_cell_void = NULL;
                                }
                                if(i > 0)
                                {
                                    pAllocParams.bank = MEM_YTCM;
                                    vfree(pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[i].biquadLP_processing_ctx.p_biquad_cell_void, &pAllocParams);
                                    pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[i].biquadLP_processing_ctx.p_biquad_cell_void = NULL;
                                }
                            }
                            break;
                        case OTHER_BIQUAD:
                            {
                                if(i < MDRC_BANDS_MAX - 1)
                                {
                                    pAllocParams.bank = MEM_YTCM;
                                    vfree(pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[i].biquadLP_processing_ctx.p_biquad_cell_void, &pAllocParams);
                                    pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[i].biquadLP_processing_ctx.p_biquad_cell_void = NULL;
                                }
                                if(i > 0)
                                {
                                    pAllocParams.bank = MEM_YTCM;
                                    vfree(pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[i].biquadHP_processing_ctx.p_biquad_cell_void, &pAllocParams);
                                    pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[i].biquadHP_processing_ctx.p_biquad_cell_void = NULL;
                                }
                            }
                            break;
                        default:
                            break;
                    }

                    for(j = 0; j < pMdrcStruct->NumMainCh; j++)
                    {
                        if(i < MDRC_BANDS_MAX - 1)
                        {
                            pAllocParams.bank = MEM_XTCM;
                            vfree(pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[i].biquadLP_processing_ctx.p_biquad_mem[j], &pAllocParams);
                            pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[i].biquadLP_processing_ctx.p_biquad_mem[j] = NULL;
                        }
                        if(i > 0)
                        {
                            pAllocParams.bank = MEM_XTCM;
                            vfree(pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[i].biquadHP_processing_ctx.p_biquad_mem[j], &pAllocParams);
                            pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_biquads[i].biquadHP_processing_ctx.p_biquad_mem[j] = NULL;
                        }
                    }
                }

                pAllocParams.bank = MEM_DDR24;
                vfree(pMdrcStruct->p_mdrc_filters, &pAllocParams);
                pMdrcStruct->p_mdrc_filters = NULL;
            }
            break;

        case MDRC_FIR_FILTER:
            {
                for(i = 0; i < MDRC_BANDS_MAX; i++)
                {
                    pAllocParams.bank = MEM_XTCM;
                    vfree(pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.mdrc_FIR[i].FIR.coef, &pAllocParams);
                }
                for(j = 0; j < pMdrcStruct->NumMainCh; j++)
                {
                    pAllocParams.bank = MEM_YTCM;
                    vfree(pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.p_FIR_common_mem[j], &pAllocParams);
                    pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.p_FIR_common_mem[j] = NULL;
                }

                pAllocParams.bank = MEM_YTCM;
                vfree(pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.p_FIR_common_mem_optim, &pAllocParams);
                pMdrcStruct->p_mdrc_filters->mdrc_filters.mdrc_FIRs.p_FIR_common_mem_optim = NULL;

                pAllocParams.bank = MEM_DDR24;
                vfree(pMdrcStruct->p_mdrc_filters, &pAllocParams);
                pMdrcStruct->p_mdrc_filters = NULL;
            }
            break;

        default:
            break;
    }

    pAllocParams.bank = MEM_DDR24;
    vfree(pMdrcStruct->p_filter_buffer, &pAllocParams);
    pMdrcStruct->p_filter_buffer = NULL;

    pMdrcCtx->allocDone = false;

    OstTraceFiltInst1(TRACE_DEBUG,"mdrcmpc::deallocation of MDRC coeffs done (ctx=0x%x)", (unsigned int)pMdrcCtx);
}

///////////////////////////////////////////////////////////////////////////////////

t_EffectError Mdrc_construct(EffectInit_t *pInitParameters, void** pEffectCtx)
{
    TRACE_t * this = (TRACE_t *) pInitParameters->pTraceObject;
    MdrcWrpCtx_t *pMdrcCtx = NULL;
    t_ExtendedAllocParams pAllocParams;

    pAllocParams.bank = pInitParameters->nMemoryBank;
    pAllocParams.trace_p = this;

    pMdrcCtx = (MdrcWrpCtx_t *) vmalloc(sizeof(MdrcWrpCtx_t), &pAllocParams);
    if(pMdrcCtx == NULL) return EFFECT_ERROR;
    *pEffectCtx= (void*)pMdrcCtx;

    pMdrcCtx->pTraceObject   = this;
    pMdrcCtx->nMemoryBank    = pInitParameters->nMemoryBank;

    //Allocate MDRC algorithms structures in external memory
    pAllocParams.bank = MEM_DDR24;

    pMdrcCtx->pMdrcStruct = (MDRC5B_LOCAL_STRUCT_T *) vmalloc(sizeof(MDRC5B_LOCAL_STRUCT_T), &pAllocParams);
    if(pMdrcCtx->pMdrcStruct == NULL) return EFFECT_ERROR;

    pMdrcCtx->pMdrcAlgoConfig = (Mdrc5bAlgoConfig_t *) vmalloc(sizeof(Mdrc5bAlgoConfig_t), &pAllocParams);
    if(pMdrcCtx->pMdrcAlgoConfig == NULL) return EFFECT_ERROR;

    pMdrcCtx->pMdrcConfig = (MdrcConfig_t *) vmalloc(sizeof(MdrcConfig_t), &pAllocParams);
    if(pMdrcCtx->pMdrcConfig == NULL) return EFFECT_ERROR;

    //Initialization
    pMdrcCtx->allocDone = false;
    pMdrcCtx->configReceived = false;
    pMdrcCtx->pMdrcStruct->mdrc_filter_kind = MDRC_LEGACY_FILTER;
    pMdrcCtx->pMdrcStruct->biquad_kind      = BUTTERWORTH_BIQUAD;
    pMdrcCtx->pMdrcStruct->FIR_kind         = SYMMETRIC_FIR;
    pMdrcCtx->pMdrcStruct->p_mdrc_filters   = NULL;
    pMdrcCtx->pMdrcStruct->p_filter_buffer  = NULL;
    pMdrcCtx->pMdrcStruct->p_filter_coef    = NULL;
    pMdrcCtx->pMdrcStruct->p_filter_mem     = NULL;

    OstTraceFiltInst1(TRACE_DEBUG,"mdrcmpc::construct (ctx=0x%x)", (unsigned int)pMdrcCtx);

    return EFFECT_ERROR_NONE;
}

t_EffectError Mdrc_open(void *pEffectCtx, EffectOpen_t *pOpenParameters, t_EffectProcessMode *processMode)
{
    MdrcWrpCtx_t *pMdrcCtx = (MdrcWrpCtx_t*) pEffectCtx;
    TRACE_t * this = pMdrcCtx->pTraceObject;
    PcmSettings_t *pInputPcmSettings = NULL;
    PcmSettings_t *pOutputPcmSettings = NULL;
    int error;
    ASSERT(pOpenParameters!=NULL);

    pInputPcmSettings = &pOpenParameters->sInputPcmSettings;
    pOutputPcmSettings = &pOpenParameters->sOutputPcmSettings;

    OstTraceFiltInst3(TRACE_DEBUG,"mdrcmpc::open (ctx=0x%x) : nbCh=%d, freq=%d", (unsigned int)pMdrcCtx, pInputPcmSettings->nChannels, pInputPcmSettings->nSamplingRate);

    //Initializze Algo parameters depending on NbChannels and sampling frequency
    ASSERT(pInputPcmSettings->nSamplingRate == pOutputPcmSettings->nSamplingRate);
    ASSERT(pInputPcmSettings->nChannels == pOutputPcmSettings->nChannels);

    //MDRC coefficients will be initialized when Mdrc_set_parameter is called with  index.

    *processMode = EFFECT_INPLACE; //MDRC is always working in inplace mode

    return EFFECT_ERROR_NONE;
}

t_EffectError Mdrc_set_parameter(void *pEffectCtx, int index, void *param_struct)
{
    MdrcWrpCtx_t *pMdrcCtx = (MdrcWrpCtx_t*) pEffectCtx;
    TRACE_t * this = pMdrcCtx->pTraceObject;

    switch(index) {
        case AFM_DSP_IndexParamMdrc:
            {
                MdrcParams_t * pMdrcParams = (MdrcParams_t *)param_struct;
                PRECONDITION((pMdrcParams->mFilterKind == MDRC_LEGACY_FILTER) || (pMdrcParams->mFilterKind == MDRC_BIQUAD_FILTER) || (pMdrcParams->mFilterKind == MDRC_FIR_FILTER));
                PRECONDITION((pMdrcParams->mFilterKind != MDRC_BIQUAD_FILTER) || (pMdrcParams->mBiquadKind == BUTTERWORTH_BIQUAD) || (pMdrcParams->mBiquadKind == OTHER_BIQUAD));
                PRECONDITION((pMdrcParams->mFilterKind != MDRC_FIR_FILTER)    || (pMdrcParams->mFirKind    == SYMMETRIC_FIR)      || (pMdrcParams->mFirKind    == OTHER_FIR));
                PRECONDITION((pMdrcParams->mChannels == 1) || (pMdrcParams->mChannels == 2));

                pMdrcCtx->pMdrcStruct->mdrc_filter_kind = (t_mdrc_filter_kind) pMdrcParams->mFilterKind;
                pMdrcCtx->pMdrcStruct->biquad_kind      = (t_biquad_kind)      pMdrcParams->mBiquadKind;
                pMdrcCtx->pMdrcStruct->FIR_kind         = (t_FIR_kind)         pMdrcParams->mFirKind;
                pMdrcCtx->pMdrcStruct->NumMainCh        = pMdrcParams->mChannels;
                pMdrcCtx->pMdrcStruct->SampleFreq       = pMdrcParams->mSamplingFreq;

                pMdrcCtx->pMdrcStruct->DataInOffset     = pMdrcParams->mChannels;
                pMdrcCtx->pMdrcStruct->DataOutOffset    = pMdrcParams->mChannels;

                allocateAndInitializeMDRCcoeffs(pMdrcCtx);

                return EFFECT_ERROR_NONE;
            }

        default:
        return EFFECT_ERROR;
    }
}

t_EffectError Mdrc_set_config(void *pEffectCtx, int index, void *config_struct)
{
    MdrcWrpCtx_t *pMdrcCtx = (MdrcWrpCtx_t*) pEffectCtx;
    TRACE_t * this = pMdrcCtx->pTraceObject;

    switch(index) {
        case AFM_DSP_IndexConfigMdrc:
            {
                t_uint16 * pMdrcConfig = (t_uint16 *)config_struct;
                t_uint16 * pLocalMdrcConfig = (t_uint16 *)pMdrcCtx->pMdrcConfig;
                int i = 0;

                //copy config into local structure -> Use loop instead of
                // *pMdrcCtx->pMdrcConfig = *pMdrcConfig;
                //because of MMDSP-compilo strange behavior. Indeed sometimes generated code copies config structure in stack before
                //copying it in config structure allocated in context (pMdrcCtx->pMdrcConfig).
                //It implies to increase stack size otherwise we get a STACK OVERFLOW DSP Panic.
                //With following loop, stack is not used as intermediate so we don't need to increase stack size.
                for(i=0; i<sizeof(MdrcConfig_t); i++) {
                    *pLocalMdrcConfig++ = *pMdrcConfig++;
                }

                if(pMdrcCtx->allocDone) {
                    // real setConfig
                    local_setConfig(pMdrcCtx);
                } else {
                    pMdrcCtx->configReceived = true;
                }

                OstTraceFiltInst1(TRACE_DEBUG,"mdrcmpc::set_config AFM_DSP_IndexConfigMdrc (ctx=0x%x)", (unsigned int)pMdrcCtx);

                return EFFECT_ERROR_NONE;
            }
        default:
        return EFFECT_ERROR;
    }
}

t_EffectError Mdrc_processBuffer(void* pEffectCtx, Buffer_p inputBuf, Buffer_p outputBuf, bool *needInputBuf, bool *filledOutputBuffer)
{
    MdrcWrpCtx_t *pMdrcCtx = (MdrcWrpCtx_t*) pEffectCtx;
    TRACE_t * this = (TRACE_t *)pMdrcCtx->pTraceObject;

    mdrc5b_main(pMdrcCtx->pMdrcStruct, inputBuf->data, outputBuf->data, inputBuf->filledLen);

    *needInputBuf = true;
    *filledOutputBuffer = true;

    return EFFECT_ERROR_NONE;
}

Buffer_p Mdrc_getOutputBuffer(void* pEffectCtx)
{
    return NULL; //process inplace
}

void Mdrc_close(void* pEffectCtx)
{
    MdrcWrpCtx_t *pMdrcCtx = (MdrcWrpCtx_t*) pEffectCtx;

    mdrc5b_reset(pMdrcCtx->pMdrcStruct);
    ASSERT(applyEffectConfig(pMdrcCtx->pMdrcStruct, pMdrcCtx->pMdrcAlgoConfig) == 0);

    //Free memory allocated for MDRC coeffs
    deallocateMDRCcoeffs(pMdrcCtx);
}

void Mdrc_destroy(void* pEffectCtx)
{
    t_ExtendedAllocParams pAllocParams;
    MdrcWrpCtx_t *pMdrcCtx = (MdrcWrpCtx_t*) pEffectCtx;

    //Free MDRC algorithms structures in external memory
    pAllocParams.bank = MEM_DDR24;
    pAllocParams.trace_p = (TRACE_t *)pMdrcCtx->pTraceObject;
    vfree(pMdrcCtx->pMdrcStruct, &pAllocParams);
    vfree(pMdrcCtx->pMdrcAlgoConfig, &pAllocParams);
    vfree(pMdrcCtx->pMdrcConfig, &pAllocParams);

    //Free MDRC context allocated in XTCM
    pAllocParams.bank = 0;
    vfree(pMdrcCtx, &pAllocParams);
}

/*************************************************************************
 *  Structure that defines the effect interface + mdrc factory Method
 ************************************************************************/

EffectDescription_t mdrc_def =
{
    Mdrc_construct,
    Mdrc_set_parameter,
    Mdrc_set_config,
    Mdrc_open,
    Mdrc_processBuffer,
    Mdrc_getOutputBuffer,
    Mdrc_close,
    Mdrc_destroy,
};

static EffectDescription_t * Mdrc_factoryMethod(void)
{
    return &mdrc_def;
}

/*************************************************************************
 *  CONSTRUCT -> mdrc registration!
 ************************************************************************/

t_nmf_error METH(construct)(void)
{
    registerEffect("audio_processor.pcm.mdrc", Mdrc_factoryMethod);

	return NMF_OK;
}

#endif

