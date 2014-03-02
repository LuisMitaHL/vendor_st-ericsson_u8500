/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   spl_api.c
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifdef _NMF_MPC_
#include <libeffects/mpc/libsplapi.nmf>
#include "spl_api.h"
#include "effects_dsp_index.h"
#include "dbc.h"
#include "audiotables.h"

//OST Traces specific headers
#include <fsm/generic/include/FSM.h>
#include <trace.idt>
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "libeffects_mpc_libsplapi_spl_apiTraces.h"
#endif

////////////////////////////////////////////////////////////
//			Internal wrapper structures declaration
////////////////////////////////////////////////////////////

typedef struct SplWrpCtx_t {
    void *              pTraceObject;
    t_memory_bank       nMemoryBank;
    SPL_LOCAL_STRUCT_T *pSplStruct;
    SplStream_t *       pSplStream;
    SplParam_t *        pSplParam;
    SplTuning_t *       pSplTuning;
    SplConfig_t *       pSplConfig;
} SplWrpCtx_t;

///////////////////////////////////////////////////////////////////////////////////

#ifdef SPL_API_MPC_DEBUG
static void printProcessBufferDebugInfo(SplWrpCtx_t *pSplCtx, MMshort *p_outputData, unsigned int channel_size)
{
    static int last_cpt = 0, cpt = 0, sample_max = 0;
    int        i, j;

    for(i = (int) channel_size; i > 0; i--)
    {
        for(j = (int) pSplCtx->pSplStream->NumberChannel; j > 0; j--)
        {
            sample_max = wmax(sample_max, wabssat(*p_outputData++));
        }
        cpt++;
        if((cpt - last_cpt) == pSplCtx->pSplStream->SampleFreq)
        {
            NmfPrint2(0, "splmpc : processBuffer cpt=%d - sample_max=%d\n", cpt, sample_max);
            sample_max = 0;
            last_cpt   = cpt;
        }
    }
}
#endif //SPL_API_MPC_DEBUG

static void printSetConfigDebugInfo(SplWrpCtx_t *pSplCtx)
{
    TRACE_t * this = pSplCtx->pTraceObject;
    OstTraceFiltInst3(TRACE_DEBUG,"splmpc::applyConfig (ctx=0x%x) : splConfig.Enable = %d, splConfig.SPL_Mode = %d", (unsigned int)pSplCtx, pSplCtx->pSplConfig->Enable, pSplCtx->pSplConfig->SPL_Mode);
    if(pSplCtx->pSplConfig->Threshold < 0)
    {
        OstTraceFiltInst2(TRACE_DEBUG,"splmpc::applyConfig (ctx=0x%x) : splConfig.Threshold = -%d", (unsigned int)pSplCtx, -pSplCtx->pSplConfig->Threshold);
    }
    else
    {
        OstTraceFiltInst2(TRACE_DEBUG,"splmpc::applyConfig (ctx=0x%x) : splConfig.Threshold    = %d", (unsigned int)pSplCtx, pSplCtx->pSplConfig->Threshold);
    }
    if(pSplCtx->pSplConfig->SPL_UserGain < 0)
    {
        OstTraceFiltInst2(TRACE_DEBUG,"splmpc::applyConfig (ctx=0x%x) : splConfig.SPL_UserGain = -%d", (unsigned int)pSplCtx, -pSplCtx->pSplConfig->SPL_UserGain);
    }
    else
    {
        OstTraceFiltInst2(TRACE_DEBUG,"splmpc::applyConfig (ctx=0x%x) : splConfig.SPL_UserGain = %d", (unsigned int)pSplCtx, pSplCtx->pSplConfig->SPL_UserGain);
    }
    if(pSplCtx->pSplConfig->PeakL_Gain < 0)
    {
        OstTraceFiltInst2(TRACE_DEBUG,"splmpc::applyConfig (ctx=0x%x) : splConfig.PeakL_Gain   = -%d", (unsigned int)pSplCtx, -pSplCtx->pSplConfig->PeakL_Gain);
    }
    else
    {
        OstTraceFiltInst2(TRACE_DEBUG,"splmpc::applyConfig (ctx=0x%x) : splConfig.PeakL_Gain   = %d", (unsigned int)pSplCtx, pSplCtx->pSplConfig->PeakL_Gain);
    }
}

///////////////////////////////////////////////////////////////////////////////////

static void updateTuningParameters(SplWrpCtx_t *pSplCtx, t_sample_freq sampleFreq, unsigned int nbChannels)
{
    int AUDIO_CONST_MEM * AudioTables_sampling_freqs_ptr;
    AudioTables_sampling_freqs_ptr      = fn_AudioTables_sampling_freqs();
    pSplCtx->pSplStream->SampleFreq     = AudioTables_sampling_freqs_ptr[sampleFreq];
    pSplCtx->pSplStream->NumberChannel  = nbChannels;

    /* Sound Pressure Limiter type */
    if(pSplCtx->pSplParam->Type == 0)
    {
        switch(sampleFreq)
        {
            case FREQ_48KHZ:
                pSplCtx->pSplTuning->BlockSize = 48;
                break;
            case FREQ_44_1KHZ:
                pSplCtx->pSplTuning->BlockSize = 44;
                break;
            case FREQ_32KHZ:
                pSplCtx->pSplTuning->BlockSize = 32;
                break;
            case FREQ_24KHZ:
                pSplCtx->pSplTuning->BlockSize = 24;
                break;
            case FREQ_22_05KHZ:
                pSplCtx->pSplTuning->BlockSize = 22;
                break;
            case FREQ_16KHZ:
                pSplCtx->pSplTuning->BlockSize = 16;
                break;
            case FREQ_12KHZ:
            case FREQ_11_025KHZ:
                pSplCtx->pSplTuning->BlockSize = 12;
                break;
            case FREQ_8KHZ:
            case FREQ_7_2KHZ:
                pSplCtx->pSplTuning->BlockSize = 8;
                break;
            default:
                pSplCtx->pSplTuning->BlockSize = 48;
                break;
        }
        pSplCtx->pSplTuning->LookAheadSize = 3 * pSplCtx->pSplTuning->BlockSize;
    }
    else    /* Peak Limiter type */
    {
        switch(sampleFreq)
        {
            case FREQ_64KHZ:
                pSplCtx->pSplTuning->BlockSize = 32;
                break;
            case FREQ_48KHZ:
                pSplCtx->pSplTuning->BlockSize = 24;
                break;
            case FREQ_44_1KHZ:
                pSplCtx->pSplTuning->BlockSize = 22;
                break;
            case FREQ_32KHZ:
                pSplCtx->pSplTuning->BlockSize = 16;
                break;
            case FREQ_24KHZ:
                pSplCtx->pSplTuning->BlockSize = 12;
                break;
            case FREQ_22_05KHZ:
                pSplCtx->pSplTuning->BlockSize = 22;
                break;
            case FREQ_16KHZ:
                pSplCtx->pSplTuning->BlockSize = 8;
                break;
            case FREQ_12KHZ:
            case FREQ_11_025KHZ:
                pSplCtx->pSplTuning->BlockSize = 6;
                break;
            case FREQ_8KHZ:
            case FREQ_7_2KHZ:
                pSplCtx->pSplTuning->BlockSize = 4;
                break;
            default:
                pSplCtx->pSplTuning->BlockSize = 48;
                break;
        }
        pSplCtx->pSplTuning->LookAheadSize = 2 * pSplCtx->pSplTuning->BlockSize;
    }
}

static inline t_EffectError applyConfig(SplWrpCtx_t *pSplCtx)
{
    int error;
    printSetConfigDebugInfo(pSplCtx);
    error = spl_set_config(pSplCtx->pSplStruct, pSplCtx->pSplConfig);
    if(error != 0) return EFFECT_ERROR;
    return EFFECT_ERROR_NONE;
}

///////////////////////////////////////////////////////////////////////////////////

t_EffectError Spl_construct(EffectInit_t *pInitParameters, void** pEffectCtx)
{
    TRACE_t * this = (TRACE_t *) pInitParameters->pTraceObject;
    SplWrpCtx_t *pSplCtx = NULL;
    t_ExtendedAllocParams pAllocParams;

    pAllocParams.bank = pInitParameters->nMemoryBank;
    pAllocParams.trace_p = this;

    pSplCtx = (SplWrpCtx_t *) vmalloc(sizeof(SplWrpCtx_t), &pAllocParams);
    if(pSplCtx == NULL) return EFFECT_ERROR;
    *pEffectCtx= (void*)pSplCtx;

    pSplCtx->pTraceObject   = this;
    pSplCtx->nMemoryBank    = pInitParameters->nMemoryBank;

    //Allocate spl algorithms structures in external memory
    pAllocParams.bank = MEM_DDR24;

    pSplCtx->pSplStruct = (SPL_LOCAL_STRUCT_T *) vmalloc(sizeof(SPL_LOCAL_STRUCT_T), &pAllocParams);
    if(pSplCtx->pSplStruct == NULL) return EFFECT_ERROR;

    pSplCtx->pSplStream = (SplStream_t *) vmalloc(sizeof(SplStream_t), &pAllocParams);
    if(pSplCtx->pSplStream == NULL) return EFFECT_ERROR;

    pSplCtx->pSplParam = (SplParam_t *) vmalloc(sizeof(SplParam_t), &pAllocParams);
    if(pSplCtx->pSplParam == NULL) return EFFECT_ERROR;

    pSplCtx->pSplTuning = (SplTuning_t *) vmalloc(sizeof(SplTuning_t), &pAllocParams);
    if(pSplCtx->pSplTuning == NULL) return EFFECT_ERROR;

    pSplCtx->pSplConfig = (SplConfig_t *) vmalloc(sizeof(SplConfig_t), &pAllocParams);
    if(pSplCtx->pSplConfig == NULL) return EFFECT_ERROR;

    //Initialization
    pSplCtx->pSplStream->SampleFreq         = 48000;
    pSplCtx->pSplStream->NumberChannel      = 1;

    pSplCtx->pSplParam->Type                = SPL_PEAK_LIMITER;

    //Tuning parameters initialization
    pSplCtx->pSplTuning->PeakClippingMode   = SPL_HARD_CLIPPING;
    pSplCtx->pSplTuning->PowerAttackTime    = 0;
    pSplCtx->pSplTuning->PowerReleaseTime   = 0;
    pSplCtx->pSplTuning->GainAttackTime     = 400000;
    pSplCtx->pSplTuning->GainReleaseTime    = 200000;
    pSplCtx->pSplTuning->GainHoldTime       = 1000;
    pSplCtx->pSplTuning->ThreshAttackTime   = 500;
    pSplCtx->pSplTuning->ThreshReleaseTime  = 200000;
    pSplCtx->pSplTuning->ThreshHoldTime     = 1000;
    pSplCtx->pSplTuning->BlockSize          = 0;
    pSplCtx->pSplTuning->LookAheadSize      = 0;

    pSplCtx->pSplConfig->Enable             = FALSE;
    pSplCtx->pSplConfig->Threshold          = 0;
    pSplCtx->pSplConfig->SPL_Mode           = SPL_LIMITATION;
    pSplCtx->pSplConfig->SPL_UserGain       = 0;
    pSplCtx->pSplConfig->PeakL_Gain         = 0;

    OstTraceFiltInst1(TRACE_DEBUG,"splmpc::construct (ctx=0x%x)", (unsigned int)pSplCtx);

    return EFFECT_ERROR_NONE;
}

t_EffectError Spl_open(void *pEffectCtx, EffectOpen_t *pOpenParameters, t_EffectProcessMode *processMode)
{
    SplWrpCtx_t *pSplCtx = (SplWrpCtx_t*) pEffectCtx;
    TRACE_t * this = pSplCtx->pTraceObject;
    PcmSettings_t *pInputPcmSettings = NULL;
    PcmSettings_t *pOutputPcmSettings = NULL;
    int AUDIO_CONST_MEM * AudioTables_sampling_freqs_ptr;
    int error;
    ASSERT(pOpenParameters!=NULL);

    pInputPcmSettings = &pOpenParameters->sInputPcmSettings;
    pOutputPcmSettings = &pOpenParameters->sOutputPcmSettings;

    OstTraceFiltInst3(TRACE_DEBUG,"splmpc::open (ctx=0x%x) : nbCh=%d, freq=%d", (unsigned int)pSplCtx, pInputPcmSettings->nChannels, pInputPcmSettings->nSamplingRate);

    //Initializze Algo parameters depending on NbChannels and sampling frequency
    ASSERT(pInputPcmSettings->nSamplingRate == pOutputPcmSettings->nSamplingRate);
    ASSERT(pInputPcmSettings->nChannels == pOutputPcmSettings->nChannels);
    updateTuningParameters(pSplCtx, pInputPcmSettings->nSamplingRate, pInputPcmSettings->nChannels);

    error = spl_set_parameter(pSplCtx->pSplStruct, pSplCtx->pSplParam, pSplCtx->pSplStream, pSplCtx->pSplTuning);
    if(error != 0) return EFFECT_ERROR;

    error = spl_init(pSplCtx->pSplStruct, pSplCtx->pSplParam, pSplCtx->pSplStream, pSplCtx->pSplTuning);
    if(error != 0) return EFFECT_ERROR;

    applyConfig(pSplCtx);

    *processMode = EFFECT_INPLACE; //Spl is always working in inplace mode

    return EFFECT_ERROR_NONE;
}

t_EffectError Spl_set_parameter(void *pEffectCtx, int index, void *param_struct)
{
    SplWrpCtx_t *pSplCtx = (SplWrpCtx_t*) pEffectCtx;
    TRACE_t * this = pSplCtx->pTraceObject;

    switch(index) {
        case AFM_DSP_IndexParamSpl:
        {
            SplParamType_t * pSplParams = (SplParamType_t *)param_struct;
            pSplCtx->pSplParam->Type = pSplParams->nType;
            if(pSplCtx->pSplParam->Type == 0) {
                pSplCtx->pSplTuning->PeakClippingMode   = 0;
                pSplCtx->pSplTuning->PowerAttackTime    = 1200000;
                pSplCtx->pSplTuning->PowerReleaseTime   = 800000;
                pSplCtx->pSplTuning->GainAttackTime     = 30000;
                pSplCtx->pSplTuning->GainReleaseTime    = 200000;
                pSplCtx->pSplTuning->GainHoldTime       = 10000;
                pSplCtx->pSplTuning->ThreshAttackTime   = 0;
                pSplCtx->pSplTuning->ThreshReleaseTime  = 0;
                pSplCtx->pSplTuning->ThreshHoldTime     = 0;
            } else {
#ifdef OMX_SPL_TUNING_2
                pSplCtx->pSplTuning->PeakClippingMode  = 1;
                pSplCtx->pSplTuning->PowerAttackTime    = 500;
                pSplCtx->pSplTuning->PowerReleaseTime   = 200000;
#else
                pSplCtx->pSplTuning->PeakClippingMode  = 2;
                pSplCtx->pSplTuning->PowerAttackTime    = 0;
                pSplCtx->pSplTuning->PowerReleaseTime   = 0;
#endif
                pSplCtx->pSplTuning->GainAttackTime     = pSplParams->nAttackTime * 1000; //value in microseconds
                pSplCtx->pSplTuning->GainReleaseTime    = pSplParams->nReleaseTime * 1000; //value in microseconds
                pSplCtx->pSplTuning->GainHoldTime       = 1000;
                pSplCtx->pSplTuning->ThreshAttackTime   = 500;
                pSplCtx->pSplTuning->ThreshReleaseTime  = pSplParams->nReleaseTime * 1000; //value in microseconds
                pSplCtx->pSplTuning->ThreshHoldTime     = 1000;
            }
            OstTraceFiltInst4(TRACE_DEBUG,"splmpc::set_parameter (ctx=0x%x) : nType=%d, nAttackTime=%d, nReleaseTime=%d", (unsigned int)pSplCtx, pSplParams->nType, pSplParams->nAttackTime, pSplParams->nReleaseTime);
            return EFFECT_ERROR_NONE;
        }
        default:
        return EFFECT_ERROR;
    }
}

t_EffectError Spl_set_config(void *pEffectCtx, int index, void *config_struct)
{
    SplWrpCtx_t *pSplCtx = (SplWrpCtx_t*) pEffectCtx;

    switch(index) {
        case OMX_DSP_IndexConfigSpllimit:
        {
            SpllimitConfigType_t * pSplConfig = (SpllimitConfigType_t *)config_struct;
            pSplCtx->pSplConfig->Enable     = pSplConfig->bEnable;
            pSplCtx->pSplConfig->SPL_Mode   = pSplConfig->eMode;
            pSplCtx->pSplConfig->Threshold  = pSplConfig->nSplLimit;
            if(pSplCtx->pSplTuning->BlockSize != 0) {
                return applyConfig(pSplCtx); //config applied only when spl_init has already been called
            }
            return EFFECT_ERROR_NONE;
        }
        case OMX_DSP_IndexConfigAudioVolume:
        {
            SplVolCtrlConfigVolume_t * pVolumeConfig = (SplVolCtrlConfigVolume_t *)config_struct;
            pSplCtx->pSplConfig->SPL_UserGain   = pVolumeConfig->sVolume.nValue;
            pSplCtx->pSplConfig->PeakL_Gain     = pVolumeConfig->sVolume.nValue;
            if(pSplCtx->pSplTuning->BlockSize != 0) {
                return applyConfig(pSplCtx); //config applied only when spl_init has already been called
            }
            return EFFECT_ERROR_NONE;
        }
        default:
        return EFFECT_ERROR;
    }
}

t_EffectError Spl_processBuffer(void* pEffectCtx, Buffer_p inputBuf, Buffer_p outputBuf, bool *needInputBuf, bool *filledOutputBuffer)
{
    SplWrpCtx_t *pSplCtx = (SplWrpCtx_t*) pEffectCtx;
    TRACE_t * this = (TRACE_t *)pSplCtx->pTraceObject;
    t_uint16 channel_size = inputBuf->filledLen;

    if(pSplCtx->pSplStream->NumberChannel == 2)
    {
        channel_size >>= 1;
    }

    spl_main(inputBuf->data, outputBuf->data, channel_size, pSplCtx->pSplStruct);

#ifdef SPL_API_MPC_DEBUG
    printProcessBufferDebugInfo(pSplCtx, outputBuf->data, channel_size);
#endif //SPL_API_MPC_DEBUG

    *needInputBuf = true;
    *filledOutputBuffer = true;

    return EFFECT_ERROR_NONE;
}

Buffer_p Spl_getOutputBuffer(void* pEffectCtx)
{
    return NULL; //process inplace
}

void Spl_close(void* pEffectCtx)
{
    SplWrpCtx_t *pSplCtx = (SplWrpCtx_t*) pEffectCtx;

    spl_reset(pSplCtx->pSplStruct);
}

void Spl_destroy(void* pEffectCtx)
{
    t_ExtendedAllocParams pAllocParams;
    SplWrpCtx_t *pSplCtx = (SplWrpCtx_t*) pEffectCtx;

    //Free spl algorithms structures in external memory
    pAllocParams.bank = MEM_DDR24;
    pAllocParams.trace_p = (TRACE_t *)pSplCtx->pTraceObject;
    vfree(pSplCtx->pSplStruct, &pAllocParams);
    vfree(pSplCtx->pSplStream, &pAllocParams);
    vfree(pSplCtx->pSplParam, &pAllocParams);
    vfree(pSplCtx->pSplTuning, &pAllocParams);
    vfree(pSplCtx->pSplConfig, &pAllocParams);

    //Free spl context allocated in XTCM
    pAllocParams.bank = 0;
    vfree(pEffectCtx, &pAllocParams);
}

/*************************************************************************
 *  Structure that defines the effect interface + spl factory Method
 ************************************************************************/

EffectDescription_t spl_def =
{
    Spl_construct,
    Spl_set_parameter,
    Spl_set_config,
    Spl_open,
    Spl_processBuffer,
    Spl_getOutputBuffer,
    Spl_close,
    Spl_destroy,
};

static EffectDescription_t * Spl_factoryMethod(void)
{
    return &spl_def;
}

/*************************************************************************
 *  CONSTRUCT -> spl registration!
 ************************************************************************/

t_nmf_error METH(construct)(void)
{
    registerEffect("audio_processor.pcm.spl", Spl_factoryMethod);

	return NMF_OK;
}

#endif

