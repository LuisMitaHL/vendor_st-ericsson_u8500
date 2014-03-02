/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   resample_api.c
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifdef _NMF_MPC_ //SRC wrapper is only implemented for OMX Mixer/Splitter DSP components
#include <libeffects/mpc/libresamplingapi.nmf>
#include "effects_dsp_index.h"
#include "resample_api.h"
#include <dbc.h>

//OST Traces specific headers
#include <fsm/generic/include/FSM.h>
#include <trace.idt>
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "libeffects_mpc_libresamplingapi_resample_apiTraces.h"
#endif

////////////////////////////////////////////////////////////
//			Internal wrapper structures declaration
////////////////////////////////////////////////////////////

typedef struct ResampleWrpCtx_t {
    void *              pTraceObject;
    t_memory_bank       nMemoryBank;
    SRC_QUALITY_t       srcMode;
    ResampleContext     sResampleCtx;
    unsigned int        outputChannels;
    Buffer_t            outputBuffer;
    void *              pHeap;
} ResampleWrpCtx_t;

///////////////////////////////////////////////////////////////////////////////////

static bool isLowLatencySRC(t_sample_freq inputFreq, t_sample_freq outputFreq) {
    if ((inputFreq == FREQ_8KHZ || inputFreq == FREQ_16KHZ || inputFreq == FREQ_48KHZ)
            && (outputFreq == FREQ_8KHZ || outputFreq == FREQ_16KHZ || outputFreq == FREQ_48KHZ)) {
        return true;
    } else {
        return false;
    }
}

///////////////////////////////////////////////////////////////////////////////////

t_EffectError Resample_construct(EffectInit_t *pInitParameters, void** pEffectCtx)
{
    TRACE_t * this = (TRACE_t *) pInitParameters->pTraceObject;
    ResampleWrpCtx_t *pSrcCtx = NULL;
    t_ExtendedAllocParams pAllocParams;

    pAllocParams.bank = pInitParameters->nMemoryBank;
    pAllocParams.trace_p = this;

    pSrcCtx = (ResampleWrpCtx_t *) vmalloc(sizeof(ResampleWrpCtx_t), &pAllocParams);
    if(pSrcCtx == NULL) return EFFECT_ERROR;
    *pEffectCtx= (void*)pSrcCtx;

    pSrcCtx->pTraceObject   = this;
    pSrcCtx->nMemoryBank    = pInitParameters->nMemoryBank;

    pSrcCtx->srcMode = SRC_STANDARD_QUALITY;
    pSrcCtx->outputBuffer.data = NULL;
    pSrcCtx->outputBuffer.allocLen = 0;

    OstTraceFiltInst1(TRACE_DEBUG,"SRCmpc::construct (ctx=0x%x)", (unsigned int)pSrcCtx);

    return EFFECT_ERROR_NONE;
}

t_EffectError Resample_open(void *pEffectCtx, EffectOpen_t *pOpenParameters, t_EffectProcessMode *processMode)
{
    ResampleWrpCtx_t *pSrcCtx = (ResampleWrpCtx_t *) pEffectCtx;
    TRACE_t * this = pSrcCtx->pTraceObject;
    SRC_QUALITY_t srcMode = SRC_STANDARD_QUALITY;
    PcmSettings_t *pInputPcmSettings = NULL;
    PcmSettings_t *pOutputPcmSettings = NULL;
    int status, heapsize, size;
    void *buf;
    t_ExtendedAllocParams pAllocParams;

    ASSERT(pOpenParameters!=NULL);

    pAllocParams.bank = pSrcCtx->nMemoryBank;
    pAllocParams.trace_p = this;

    pInputPcmSettings = &pOpenParameters->sInputPcmSettings;
    pOutputPcmSettings = &pOpenParameters->sOutputPcmSettings;

    OstTraceFiltInst4(TRACE_DEBUG,"SRCmpc::open (ctx=0x%x) : inputFreq=%d, outputFreq=%d, nbCh=%d", (unsigned int)pSrcCtx, pInputPcmSettings->nSamplingRate, pOutputPcmSettings->nSamplingRate, pOutputPcmSettings->nChannels); 

    size = pOutputPcmSettings->nChannels*pOutputPcmSettings->nBlockSize;

    if(pSrcCtx->srcMode == AFM_SRC_MODE_CUSTOM1)
    {
        srcMode = SRC_LOW_RIPPLE;
    }
    else if (isLowLatencySRC(pInputPcmSettings->nSamplingRate, pOutputPcmSettings->nSamplingRate))
    {
        srcMode = SRC_LOW_LATENCY_IN_MSEC;
        size = pInputPcmSettings->nBlockSize;
    }
    else
    {
        srcMode = SRC_STANDARD_QUALITY;
    }

    resample_calc_max_heap_size_fixin_fixout(pInputPcmSettings->nSamplingRate, pOutputPcmSettings->nSamplingRate,
                srcMode, &heapsize, size, pOutputPcmSettings->nChannels, 0);

    buf = vmalloc(heapsize, &pAllocParams);

    pSrcCtx->pHeap = buf;

    status = resample_x_init_ctx_low_mips_fixin_fixout(
                buf, heapsize, &pSrcCtx->sResampleCtx, pInputPcmSettings->nSamplingRate,
                pOutputPcmSettings->nSamplingRate, srcMode, pOutputPcmSettings->nChannels, size);

    buf = vmalloc(pOutputPcmSettings->nBlockSize * pOutputPcmSettings->nChannels, &pAllocParams);

    pSrcCtx->outputChannels = pOutputPcmSettings->nChannels;
    pSrcCtx->outputBuffer.data = buf;
    pSrcCtx->outputBuffer.allocLen = pOutputPcmSettings->nBlockSize;

    *processMode = EFFECT_NOT_INPLACE; //SRC is always working in not inplace mode

    return EFFECT_ERROR_NONE;
}

t_EffectError Resample_set_parameter(void *pEffectCtx, int index, void *param_struct)
{
    ResampleWrpCtx_t *pSrcCtx = (ResampleWrpCtx_t *) pEffectCtx;

    switch(index) {
        case AFM_DSP_IndexParamSrcMode:
        {
            SrcModeParam_t * pParam = (SrcModeParam_t *)param_struct;
            pSrcCtx->srcMode = pParam->srcMode;
            return EFFECT_ERROR_NONE;
        }
        default:
        return EFFECT_ERROR;
    }
}

t_EffectError Resample_set_config(void *pEffectCtx, int index, void *config_struct)
{
    return EFFECT_ERROR_NONE;
}

t_EffectError Resample_processBuffer(void* pEffectCtx, Buffer_p inputBuf, Buffer_p outputBuf, bool *needInputBuf, bool *filledOutputBuffer)
{
    ResampleWrpCtx_t *pSrcCtx = (ResampleWrpCtx_t *) pEffectCtx;
    TRACE_t * this = pSrcCtx->pTraceObject;

    OstTraceFiltInst4(TRACE_DEBUG,"SRCmpc::Resample_processBuffer input timestamp=%d, input size=%d, output timestamp=%d output size=%d", (unsigned int)(inputBuf->nTimeStampl & 0xFFFFFFFF), inputBuf->filledLen, (unsigned int)(outputBuf->nTimeStampl & 0xFFFFFFFF), outputBuf->filledLen);

    resample_process(&pSrcCtx->sResampleCtx, inputBuf, outputBuf, pSrcCtx->outputChannels, needInputBuf, filledOutputBuffer);

    return EFFECT_ERROR_NONE;
}

Buffer_p Resample_getOutputBuffer(void* pEffectCtx)
{
    ResampleWrpCtx_t *pSrcCtx = (ResampleWrpCtx_t *) pEffectCtx;

    // reset some OMXBUFFERHEADER fields
    pSrcCtx->outputBuffer.nTimeStampl = 0;
    pSrcCtx->outputBuffer.nTimeStamph = 0;
    pSrcCtx->outputBuffer.flags = 0;

    return &pSrcCtx->outputBuffer;
}

void Resample_close(void* pEffectCtx)
{
    ResampleWrpCtx_t *pSrcCtx = (ResampleWrpCtx_t *) pEffectCtx;
    t_ExtendedAllocParams pAllocParams;

    pAllocParams.bank = 0;
    pAllocParams.trace_p = (TRACE_t *)pSrcCtx->pTraceObject;

    vfree(pSrcCtx->outputBuffer.data, &pAllocParams);
    pSrcCtx->outputBuffer.data = NULL;
    pSrcCtx->outputBuffer.allocLen = 0;
   vfree(pSrcCtx->pHeap, &pAllocParams);
    pSrcCtx->pHeap = NULL;
}

void Resample_destroy(void* pEffectCtx)
{
    ResampleWrpCtx_t *pSrcCtx = (ResampleWrpCtx_t *) pEffectCtx;
    t_ExtendedAllocParams pAllocParams;

    pAllocParams.bank = 0;
    pAllocParams.trace_p = (TRACE_t *)pSrcCtx->pTraceObject;

    vfree(pSrcCtx->outputBuffer.data, &pAllocParams);
    vfree(pSrcCtx->pHeap, &pAllocParams);
    vfree(pSrcCtx, &pAllocParams);
}

/*************************************************************************
 *  Structure that defines the effect interface + SRC factory Method
 ************************************************************************/

EffectDescription_t resample_def =
{
    Resample_construct,
    Resample_set_parameter,
    Resample_set_config,
    Resample_open,
    Resample_processBuffer,
    Resample_getOutputBuffer,
    Resample_close,
    Resample_destroy,
};

EffectDescription_t * Resample_factoryMethod(void)
{
    return &resample_def;
}

/*************************************************************************
 *  CONSTRUCT -> SRC registration!
 ************************************************************************/

t_nmf_error METH(construct)(void)
{
    registerEffect("resampling_effect", Resample_factoryMethod);

	return NMF_OK;
}

#endif //_NMF_MPC_

