/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   updownmix.c
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifdef _NMF_MPC_
#include <libeffects/mpc/libupdownmix.nmf>
#include <dbc.h>

//OST Traces specific headers
#include <fsm/generic/include/FSM.h>
#include <trace.idt>
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "libeffects_mpc_libupdownmix_updownmixTraces.h"
#endif

#define MAXITERCOUNT 512    // 2^n such as 2^n<=1023 (if MAXITERCOUNT=1023, compiler adds a divide which is not optimized)

////////////////////////////////////////////////////////////
//			Internal wrapper structures declaration
////////////////////////////////////////////////////////////

typedef struct UpDownmixWrpCtx_t {
    void *          pTraceObject;
    t_memory_bank   nMemoryBank;
    Buffer_t        outputBuffer;
    void            (*process)(Buffer_p pInputBuf, Buffer_p pOutputBuf);
} UpDownmixWrpCtx_t;

////////////////////////////////////////////////////////////

static inline void stereo2mono(Buffer_p pInputBuf, Buffer_p pOutputBuf) {
    int i, n;
    t_sword *p1, *p2;

    ASSERT((pInputBuf->filledLen % 2) == 0);

    pInputBuf->filledLen /= 2;
    p1 = p2 = pInputBuf->data;
#ifdef __flexcc2__
#pragma loop maxitercount(MAXITERCOUNT)
#endif
    for(n = (int) pInputBuf->filledLen; n > MAXITERCOUNT; n -= MAXITERCOUNT)
    {
        for (i = 0; i < MAXITERCOUNT; i++) {
            *p2++ = waddsat(*p1 >> 1, *(p1 + 1) >> 1);
            p1 += 2;
        }
    }
#ifdef __flexcc2__
#pragma loop maxitercount(MAXITERCOUNT)
#endif
    for (i = 0; i < n; i++) {
        *p2++ = waddsat(*p1 >> 1, *(p1 + 1) >> 1);
        p1 += 2;
    }
}

static inline void mono2stereo(Buffer_p pInputBuf, Buffer_p pOutputBuf) {
    int i, n;
    t_sword *p1, *p2, smpl;

    p1 = pInputBuf->data;
    p2 = pOutputBuf->data;
#ifdef __flexcc2__
#pragma loop maxitercount(MAXITERCOUNT)
#endif
    for(n = (int) pInputBuf->filledLen; n > MAXITERCOUNT; n -= MAXITERCOUNT)
    {
        for (i = 0; i < MAXITERCOUNT; i++) {
            smpl  = *p1++;
            *p2++ = smpl;
            *p2++ = smpl;
        }
    }
#ifdef __flexcc2__
#pragma loop maxitercount(MAXITERCOUNT)
#endif
    for (i = 0; i < n; i++) {
        smpl  = *p1++;
        *p2++ = smpl;
        *p2++ = smpl;
    }

    //Update filledLen, flags and timestamps
    pOutputBuf->filledLen = pInputBuf->filledLen * 2;
    pOutputBuf->flags = pInputBuf->flags;
	pOutputBuf->nTimeStamph = pInputBuf->nTimeStamph;
	pOutputBuf->nTimeStampl = pInputBuf->nTimeStampl;
}

static inline void fillBufferWithZeroes(Buffer_p buf)
{
    int i;

    for (i = buf->filledLen; i < buf->allocLen; i++) {
        buf->data[i] = 0;
    }
}

///////////////////////////////////////////////////////////////////////////////////

t_EffectError Updownmix_construct(EffectInit_t *pInitParameters, void** pEffectCtx)
{
    TRACE_t * this = (TRACE_t *) pInitParameters->pTraceObject;
    UpDownmixWrpCtx_t *pUpDownmixCtx = NULL;
    t_ExtendedAllocParams pAllocParams;

    pAllocParams.bank = pInitParameters->nMemoryBank;
    pAllocParams.trace_p = this;

    pUpDownmixCtx = (UpDownmixWrpCtx_t *) vmalloc(sizeof(UpDownmixWrpCtx_t), &pAllocParams);
    if(pUpDownmixCtx == NULL) return EFFECT_ERROR;
    *pEffectCtx= (void*)pUpDownmixCtx;

    pUpDownmixCtx->pTraceObject             = this;
    pUpDownmixCtx->nMemoryBank              = pInitParameters->nMemoryBank;
    pUpDownmixCtx->outputBuffer.data        = NULL;
    pUpDownmixCtx->outputBuffer.allocLen    = 0;

    OstTraceFiltInst1(TRACE_DEBUG,"updownmixmpc::Updownmix_init (ctx=0x%x)", (unsigned int)pUpDownmixCtx);

    return EFFECT_ERROR_NONE;
}

t_EffectError Updownmix_open(void *pEffectCtx, EffectOpen_t *pOpenParameters, t_EffectProcessMode *processMode)
{
    UpDownmixWrpCtx_t *pUpDownmixCtx = (UpDownmixWrpCtx_t*) pEffectCtx;
    TRACE_t * this = pUpDownmixCtx->pTraceObject;
    PcmSettings_t *pInputPcmSettings = NULL;
    PcmSettings_t *pOutputPcmSettings = NULL;
    t_ExtendedAllocParams pAllocParams;

    if(pOpenParameters==NULL) return EFFECT_ERROR;

    pAllocParams.bank = pUpDownmixCtx->nMemoryBank;
    pAllocParams.trace_p = this;

    pInputPcmSettings = &pOpenParameters->sInputPcmSettings;
    pOutputPcmSettings = &pOpenParameters->sOutputPcmSettings;

    OstTraceFiltInst4(TRACE_DEBUG,"updownmixmpc::Updownmix_open (ctx=0x%x) : nbChIn=%d, nbChOut=%d, freq=%d", (unsigned int)pUpDownmixCtx, pInputPcmSettings->nChannels, pOutputPcmSettings->nChannels, pInputPcmSettings->nSamplingRate);

    if(pInputPcmSettings->nSamplingRate != pOutputPcmSettings->nSamplingRate) return EFFECT_ERROR;

    if (pInputPcmSettings->nChannels == 2 && pOutputPcmSettings->nChannels == 1) { //Process Downmix will be inplace
        *processMode = EFFECT_INPLACE;
        pUpDownmixCtx->process = stereo2mono;
    }
    else if (pInputPcmSettings->nChannels == 1 && pOutputPcmSettings->nChannels == 2) { //Process Upmix will not be inplace
        *processMode = EFFECT_NOT_INPLACE;
        pUpDownmixCtx->process = mono2stereo;
        //Output Buffer allocation since process is not inplace
        pUpDownmixCtx->outputBuffer.data = vmalloc(pOutputPcmSettings->nBlockSize * pOutputPcmSettings->nChannels, &pAllocParams);
        pUpDownmixCtx->outputBuffer.allocLen = pOutputPcmSettings->nBlockSize;
    }
    else {
        return EFFECT_ERROR;
    }

    return EFFECT_ERROR_NONE;
}

t_EffectError Updownmix_set_parameter(void *pEffectCtx, int index, void *param_struct)
{
    return EFFECT_ERROR;
}

t_EffectError Updownmix_set_config(void *pEffectCtx, int index, void *config_struct)
{
    return EFFECT_ERROR;
}

t_EffectError Updownmix_processBuffer(void* pEffectCtx, Buffer_p inputBuf, Buffer_p outputBuf, bool *needInputBuf, bool *filledOutputBuffer)
{
    UpDownmixWrpCtx_t *pUpDownmixCtx = (UpDownmixWrpCtx_t*) pEffectCtx;
    TRACE_t * this = (TRACE_t *)pUpDownmixCtx->pTraceObject;

    if (inputBuf->flags & BUFFERFLAG_EOS) {
        fillBufferWithZeroes(inputBuf);
    }

    pUpDownmixCtx->process(inputBuf, outputBuf);

    *needInputBuf = true;
    *filledOutputBuffer = true;

    return EFFECT_ERROR_NONE;
}

Buffer_p Updownmix_getOutputBuffer(void* pEffectCtx)
{
    UpDownmixWrpCtx_t *pUpDownmixCtx = (UpDownmixWrpCtx_t*) pEffectCtx;
    return &pUpDownmixCtx->outputBuffer;
}

void Updownmix_close(void* pEffectCtx)
{
    UpDownmixWrpCtx_t *pUpDownmixCtx = (UpDownmixWrpCtx_t*) pEffectCtx;
    t_ExtendedAllocParams pAllocParams;

    pAllocParams.bank = 0;
    pAllocParams.trace_p = (TRACE_t *)pUpDownmixCtx->pTraceObject;

    vfree(pUpDownmixCtx->outputBuffer.data, &pAllocParams);
    pUpDownmixCtx->outputBuffer.data = NULL;
    pUpDownmixCtx->outputBuffer.allocLen = 0;
}

void Updownmix_destroy(void* pEffectCtx)
{
    UpDownmixWrpCtx_t *pUpDownmixCtx = (UpDownmixWrpCtx_t*) pEffectCtx;
    t_ExtendedAllocParams pAllocParams;

    pAllocParams.bank = 0;
    pAllocParams.trace_p = (TRACE_t *)pUpDownmixCtx->pTraceObject;

    vfree(pUpDownmixCtx->outputBuffer.data, &pAllocParams);
    vfree(pUpDownmixCtx, &pAllocParams);
}

/*************************************************************************
 *  Structure that defines the effect interface + updownmix factory Method
 ************************************************************************/

EffectDescription_t Updownmix_def =
{
    Updownmix_construct,
    Updownmix_set_parameter,
    Updownmix_set_config,
    Updownmix_open,
    Updownmix_processBuffer,
    Updownmix_getOutputBuffer,
    Updownmix_close,
    Updownmix_destroy,
};

static EffectDescription_t * Updownmix_factoryMethod(void)
{
    return &Updownmix_def;
}

/*************************************************************************
 *  CONSTRUCT -> updownmix registration!
 ************************************************************************/

t_nmf_error METH(construct)(void)
{
    registerEffect("updownmix_effect", Updownmix_factoryMethod);

	return NMF_OK;
}

#endif

