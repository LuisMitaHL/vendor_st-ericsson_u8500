/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   volctrl_api.c
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifdef _NMF_MPC_
#include <libeffects/mpc/libvolctrlapi.nmf>
#include "volctrl_api.h"
#include "volctrl_defines.h"
#include "effects_dsp_index.h"
#include <dbc.h>
#include <omxeventext.idt>

//OST Traces specific headers
#include <fsm/generic/include/FSM.h>
#include <trace.idt>

#ifndef _SIMU_
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "libeffects_mpc_libvolctrlapi_volctrl_apiTraces.h"
#endif
#else // _SIMU_*/
#include "libeffects/mpc/include/audiolibs_trace.h"
#endif // _SIMU_

////////////////////////////////////////////////////////////
//			Internal wrapper structures declaration
////////////////////////////////////////////////////////////

typedef struct VolCtrlWrpCtx_t {
    void *                  pTraceObject;
    t_memory_bank           nMemoryBank;
    VOLCTRL_LOCAL_STRUCT_T  sVolData;
    VolCtrlVolumeRampInfo_t *pVolumeRampInfo;
    unsigned short          bMute;
    signed int              nBalance;
    signed int              nVolume;
    signed int              nRampStartVolume;
    signed int              nRampEndVolume;
    signed int              nRampChannels;
    unsigned int            nRampDuration;
    unsigned short          bRampTerminate;
    unsigned int            isRampActivated[MAX_CH];
    void                    (*process)(void* pEffectCtx, Buffer_p inputBuf, Buffer_p outputBuf);
    void                    (*eventHandler)(OMX_EVENTTYPE event, unsigned long data1, unsigned long data2, void *pEffectCtx);
} VolCtrlWrpCtx_t;

///////////////////////////////////////////////////////////////////////////////////

static void processBufferMono(void* pEffectCtx, Buffer_p inputBuf, Buffer_p outputBuf)
{
    VolCtrlWrpCtx_t *pVolCtrlCtx = (VolCtrlWrpCtx_t*) pEffectCtx;
    MMshort *left_in  = inputBuf->data;
    MMshort *left_out = outputBuf->data;
    unsigned int size = inputBuf->filledLen;
    process_one_channel_no_cross_gain(left_in, left_out, GLL, size, 1, &pVolCtrlCtx->sVolData);
}

static void processBufferStereo(void* pEffectCtx, Buffer_p inputBuf, Buffer_p outputBuf)
{
    VolCtrlWrpCtx_t *pVolCtrlCtx = (VolCtrlWrpCtx_t*) pEffectCtx;
    MMshort *left_in  = inputBuf->data;
    MMshort *left_out = outputBuf->data;
    MMshort *right_in = inputBuf->data + 1;
    MMshort *right_out= outputBuf->data + 1;
    unsigned int size = inputBuf->filledLen;
    process_one_channel_no_cross_gain(left_in, left_out, GLL, size, 2, &pVolCtrlCtx->sVolData);
    process_one_channel_no_cross_gain(right_in, right_out, GRR, size, 2, &pVolCtrlCtx->sVolData);
}

static void setVolumeConfig(VolCtrlWrpCtx_t *pVolCtrlCtx)
{
    TRACE_t * this = (TRACE_t *)pVolCtrlCtx->pTraceObject;
    t_sint16 leftVolumemdB, rightVolumemdB;
    t_uint16 gll, glr, grl, grr;
    int i;

    if (pVolCtrlCtx->bMute) {
        leftVolumemdB   = VOLCTRL_VOLUME_MUTE;
        rightVolumemdB  = VOLCTRL_VOLUME_MUTE;
        OstTraceFiltInst1(TRACE_DEBUG,"volctrlmpc::setVolumeConfig volume is muted! (ctx=0x%x)", (unsigned int)pVolCtrlCtx);
    } else {
        leftVolumemdB  = pVolCtrlCtx->nBalance <= 0 ?
                (pVolCtrlCtx->nVolume) :
                (pVolCtrlCtx->nVolume + pVolCtrlCtx->nBalance*(VOLCTRL_VOLUME_MIN - pVolCtrlCtx->nVolume)/VOLCTRL_BALANCE_ALLRIGHT);
        rightVolumemdB = pVolCtrlCtx->nBalance <= 0 ?
                (pVolCtrlCtx->nVolume + pVolCtrlCtx->nBalance*(VOLCTRL_VOLUME_MIN - pVolCtrlCtx->nVolume)/VOLCTRL_BALANCE_ALLLEFT) :
                (pVolCtrlCtx->nVolume);
        OstTraceFiltInst3(TRACE_DEBUG,"volctrlmpc::setVolumeConfig leftVolumemdB=%d rightVolumemdB=%d (ctx=0x%x)", leftVolumemdB, rightVolumemdB, (unsigned int)pVolCtrlCtx);
    }

    gll = (128<<8); //offset
    gll += (leftVolumemdB<<8)/100; //care to take into account values < 1dB
    glr = 0;
    grl = 0;
    grr = (128<<8); //offset
    grr += (rightVolumemdB<<8)/100; //care to take into account values < 1dB

    /* reset gain */
    for (i = 0; i < 4; i++) {
        pVolCtrlCtx->sVolData.gain[i] = 0;
        pVolCtrlCtx->sVolData.duration[i] = 0;
    }

    pVolCtrlCtx->sVolData.new_config = VOLCTRL_IMMEDIAT_CMD_PENDING;

    pVolCtrlCtx->sVolData.gain_cmd[1][GLL] = gll - VOLCTRL_OFFSET_GAIN;
    pVolCtrlCtx->sVolData.gain_cmd[1][GLR] = glr - VOLCTRL_OFFSET_GAIN;
    pVolCtrlCtx->sVolData.gain_cmd[1][GRL] = grl - VOLCTRL_OFFSET_GAIN;
    pVolCtrlCtx->sVolData.gain_cmd[1][GRR] = grr - VOLCTRL_OFFSET_GAIN;

    // Update pVolCtrlCtx->pVolumeRampInfo fields for OMX_GetConfig command
    // dont use current_gaindB => update based on current_gaindB will be done upon process
    // use gain_cmd, in case no process runs (idle for instance) in order to have coherent getconfig after setconfig
    pVolCtrlCtx->pVolumeRampInfo->gGLL= gll;
    pVolCtrlCtx->pVolumeRampInfo->gGRR= grr;

}

static void setVolumeRampConfig(VolCtrlWrpCtx_t *pVolCtrlCtx)
{
    TRACE_t * this = (TRACE_t *)pVolCtrlCtx->pTraceObject;
    unsigned short startVolume = 0; // Start from
    unsigned short endVolume = 0;   // End volume
    unsigned int alpha = 0;         // Ramp Duration in 10 µs
    unsigned short channel = 0;     // Select Left(1)/Right(2) or both(3)

    OstTraceFiltInst4(TRACE_DEBUG,"volctrlmpc::setVolumeRampConfig rampStartVol=%d, rampEndVol=%d, nCh=%d (ctx=0x%x)", pVolCtrlCtx->nRampStartVolume, pVolCtrlCtx->nRampEndVolume, pVolCtrlCtx->nRampChannels, (unsigned int)pVolCtrlCtx);
    OstTraceFiltInst2(TRACE_DEBUG,"volctrlmpc::setVolumeRampConfig rampDuration=%d, bRampTerminate=%d", pVolCtrlCtx->nRampDuration, pVolCtrlCtx->bRampTerminate);

    startVolume    = (128 << 8);
    startVolume    += (pVolCtrlCtx->nRampStartVolume << 8) / 100;
    endVolume      = (128 << 8);
    endVolume      += (pVolCtrlCtx->nRampEndVolume << 8) / 100;
    alpha          = pVolCtrlCtx->nRampDuration;

    if (pVolCtrlCtx->nRampChannels == OMX_ALL) {
        channel = 3;
    }
    else if (pVolCtrlCtx->nRampChannels == OMX_AUDIO_ChannelCF
            || pVolCtrlCtx->nRampChannels == OMX_AUDIO_ChannelLF) {
        channel = 1;
    }
    else if (pVolCtrlCtx->nRampChannels == OMX_AUDIO_ChannelRF) {
        channel = 2;
    }
    else {
        ASSERT(0);
    }

    if ((channel&1)!=0) //left channel setRampConfig
    {
        if (pVolCtrlCtx->bRampTerminate==0)
        {
            pVolCtrlCtx->sVolData.new_config = VOLCTRL_IMMEDIAT_CMD_PENDING+1;
            pVolCtrlCtx->sVolData.gain_cmd[1][GLL]	= startVolume - VOLCTRL_OFFSET_GAIN;
            pVolCtrlCtx->sVolData.gain_cmd[0][GLL]	= endVolume - VOLCTRL_OFFSET_GAIN;
            pVolCtrlCtx->sVolData.duration[GLL]		= alpha;
            pVolCtrlCtx->sVolData.configL  =1;
            if(pVolCtrlCtx->isRampActivated[GLL] == 1){
                // Communication with proxy to indicate that the previous ramp is ending
                OstTraceFiltInst1(TRACE_DEBUG,"volctrlmpc::setVolumeRampConfig -> callback previous ramp (GLL) is ending! (ctx=0x%x)", (unsigned int)pVolCtrlCtx);
                pVolCtrlCtx->eventHandler(OMX_DSP_EventIndexSettingChanged,GLL, 0x1, pVolCtrlCtx);
            }
            pVolCtrlCtx->isRampActivated[GLL] = 1;
        }
        pVolCtrlCtx->sVolData.terminate[GLL] = pVolCtrlCtx->bRampTerminate;

        // Update pVolCtrlCtx->pVolumeRampInfo fields for OMX_GetConfig command
        pVolCtrlCtx->pVolumeRampInfo->gGLL = startVolume;
    }

    if ((channel&2)!=0) //right channel setRampConfig
    {
        if (pVolCtrlCtx->bRampTerminate==0)
        {
            pVolCtrlCtx->sVolData.new_config = VOLCTRL_IMMEDIAT_CMD_PENDING+1;
            pVolCtrlCtx->sVolData.gain_cmd[1][GRR]	= startVolume - VOLCTRL_OFFSET_GAIN;
            pVolCtrlCtx->sVolData.gain_cmd[0][GRR]	= endVolume - VOLCTRL_OFFSET_GAIN;
            pVolCtrlCtx->sVolData.duration[GRR]		= alpha;
            pVolCtrlCtx->sVolData.configR=1;
            if(pVolCtrlCtx->isRampActivated[GRR] == 1){
                // Communication with proxy to indicate that the previous ramp is ending
                OstTraceFiltInst1(TRACE_DEBUG,"volctrlmpc::setVolumeRampConfig -> callback previous ramp (GRR) is ending! (ctx=0x%x)", (unsigned int)pVolCtrlCtx);
                pVolCtrlCtx->eventHandler(OMX_DSP_EventIndexSettingChanged,GRR, 0x1, pVolCtrlCtx);
            }
            pVolCtrlCtx->isRampActivated[GRR] = 1;
        }
        pVolCtrlCtx->sVolData.terminate[GRR] = pVolCtrlCtx->bRampTerminate;

        // Update pVolCtrlCtx->pVolumeRampInfo fields for OMX_GetConfig command
        pVolCtrlCtx->pVolumeRampInfo->gGRR= startVolume;
    }

}


///////////////////////////////////////////////////////////////////////////////////

t_EffectError Volctrl_construct(EffectInit_t *pInitParameters, void** pEffectCtx)
{
    TRACE_t * this = (TRACE_t *) pInitParameters->pTraceObject;
    VolCtrlWrpCtx_t *pVolCtrlCtx = NULL;
    t_ExtendedAllocParams pAllocParams;

    pAllocParams.bank = pInitParameters->nMemoryBank;
    pAllocParams.trace_p = this;

    pVolCtrlCtx = (VolCtrlWrpCtx_t *) vmalloc(sizeof(VolCtrlWrpCtx_t), &pAllocParams);
    if(pVolCtrlCtx == NULL) return EFFECT_ERROR;
    *pEffectCtx= (void*)pVolCtrlCtx;

    pVolCtrlCtx->pTraceObject   = this;
    pVolCtrlCtx->nMemoryBank    = pInitParameters->nMemoryBank;
    pVolCtrlCtx->eventHandler   = pInitParameters->eventHandler;
    ASSERT(pVolCtrlCtx->eventHandler != NULL);

    pVolCtrlCtx->sVolData.gain_mask = (GLL_MASK | GRR_MASK);
    pVolCtrlCtx->sVolData.downmix   = 0;
    pVolCtrlCtx->sVolData.db_ramp   = 1;

    /* reset gll and grr to one and glr grl to zero */
    pVolCtrlCtx->sVolData.gain[GLL]          = wL_msl(FORMAT_FLOAT(0.5,MAXVAL), (1 - GUARD_BIT + 24));
    pVolCtrlCtx->sVolData.gain[GRR]          = wL_msl(FORMAT_FLOAT(0.5,MAXVAL), (1 - GUARD_BIT + 24));
    pVolCtrlCtx->sVolData.gain_smoothed[GLL] = pVolCtrlCtx->sVolData.gain[GLL];
    pVolCtrlCtx->sVolData.gain_smoothed[GRR] = pVolCtrlCtx->sVolData.gain[GRR];
    pVolCtrlCtx->sVolData.gain_cmd[0][GLL]   = 0;
    pVolCtrlCtx->sVolData.gain_cmd[0][GRR]   = 0;
    pVolCtrlCtx->sVolData.gain_cmd[1][GLL]   = 0;
    pVolCtrlCtx->sVolData.gain_cmd[1][GRR]   = 0;

    pVolCtrlCtx->sVolData.gain[GLR]          = 0;
    pVolCtrlCtx->sVolData.gain_smoothed[GLR] = pVolCtrlCtx->sVolData.gain[GLR];
    pVolCtrlCtx->sVolData.gain[GRL]          = 0;
    pVolCtrlCtx->sVolData.gain_smoothed[GRL] = pVolCtrlCtx->sVolData.gain[GRL];
    pVolCtrlCtx->sVolData.gain_cmd[0][GLR]   = VOLCTRL_MINIMUM_GAIN;
    pVolCtrlCtx->sVolData.gain_cmd[0][GRL]   = VOLCTRL_MINIMUM_GAIN;
    pVolCtrlCtx->sVolData.gain_cmd[1][GLR]   = VOLCTRL_MINIMUM_GAIN;
    pVolCtrlCtx->sVolData.gain_cmd[1][GRL]   = VOLCTRL_MINIMUM_GAIN;
    pVolCtrlCtx->sVolData.configL       = 1;
    pVolCtrlCtx->sVolData.configR       = 1;
    pVolCtrlCtx->sVolData.terminate[0]  = pVolCtrlCtx->sVolData.terminate[1]=pVolCtrlCtx->sVolData.terminate[2]=pVolCtrlCtx->sVolData.terminate[3]=0;

    pVolCtrlCtx->sVolData.input_mode    = VOLCTRL_INPUT_UNDEFINED;
    pVolCtrlCtx->sVolData.new_config    = 0;

    pVolCtrlCtx->bMute                  = false;
    pVolCtrlCtx->nBalance               = VOLCTRL_BALANCE_CENTER;
    pVolCtrlCtx->nVolume                = VOLCTRL_VOLUME_MAX;

    pVolCtrlCtx->nRampDuration          = 0;

    pVolCtrlCtx->isRampActivated[GLL]   = 0;
    pVolCtrlCtx->isRampActivated[GRR]   = 0;
    pVolCtrlCtx->isRampActivated[GLR]   = 0;
    pVolCtrlCtx->isRampActivated[GRL]   = 0;

    //Structure used by HOST to retrieve volumeRamp informations
    //Must be allocated in HOST/DSP shared memory (will be set by set_parameter method)
    pVolCtrlCtx->pVolumeRampInfo        = NULL;

    OstTraceFiltInst1(TRACE_DEBUG,"volctrlmpc:: construct (ctx=0x%x)", (unsigned int)pVolCtrlCtx);

    return EFFECT_ERROR_NONE;
}

t_EffectError Volctrl_open(void *pEffectCtx, EffectOpen_t *pOpenParameters, t_EffectProcessMode *processMode)
{
    VolCtrlWrpCtx_t *pVolCtrlCtx = (VolCtrlWrpCtx_t*) pEffectCtx;
    TRACE_t * this = pVolCtrlCtx->pTraceObject;
    PcmSettings_t *pInputPcmSettings = NULL;
    PcmSettings_t *pOutputPcmSettings = NULL;
    ASSERT(pOpenParameters!=NULL);
    ASSERT(pVolCtrlCtx->pVolumeRampInfo != NULL);

    pInputPcmSettings = &pOpenParameters->sInputPcmSettings;
    pOutputPcmSettings = &pOpenParameters->sOutputPcmSettings;

    OstTraceFiltInst3(TRACE_DEBUG,"volctrlmpc:: open (ctx=0x%x) : nbCh=%d, freq=%d", (unsigned int)pVolCtrlCtx, pInputPcmSettings->nChannels, pInputPcmSettings->nSamplingRate);
    ASSERT(pInputPcmSettings->nSamplingRate == pOutputPcmSettings->nSamplingRate);
    ASSERT(pInputPcmSettings->nChannels == pOutputPcmSettings->nChannels);

    volctrl_init(&pVolCtrlCtx->sVolData, pInputPcmSettings->nSamplingRate);

    if (pVolCtrlCtx->nRampDuration != 0) {
        setVolumeRampConfig(pVolCtrlCtx);
    } else {
        setVolumeConfig(pVolCtrlCtx);
    }

    if (pInputPcmSettings->nChannels == 1 && pOutputPcmSettings->nChannels == 1) {
        pVolCtrlCtx->sVolData.input_mode = VOLCTRL_INPUT_MONO;
        pVolCtrlCtx->process = processBufferMono;
    }
    else if (pInputPcmSettings->nChannels == 2 && pOutputPcmSettings->nChannels == 2) {
        pVolCtrlCtx->sVolData.input_mode = VOLCTRL_INPUT_STEREO;
        pVolCtrlCtx->process = processBufferStereo;
    }
    else {
        return EFFECT_ERROR;
    }

    *processMode = EFFECT_INPLACE; //Volctrl is always working in inplace mode

    pVolCtrlCtx->sVolData.new_config |= VOLCTRL_IMMEDIAT_CMD_PENDING;

    return EFFECT_ERROR_NONE;
}

t_EffectError Volctrl_set_parameter(void *pEffectCtx, int index, void *param_struct)
{
    VolCtrlWrpCtx_t *pVolCtrlCtx = (VolCtrlWrpCtx_t*) pEffectCtx;

    switch(index) {
        case OMX_DSP_ParamSetEffectInfoPtr:
        {
            pVolCtrlCtx->pVolumeRampInfo = (VolCtrlVolumeRampInfo_t *)param_struct;
            return EFFECT_ERROR_NONE;
        }
        default:
        return EFFECT_ERROR;
    }
}

t_EffectError Volctrl_set_config(void *pEffectCtx, int index, void *config_struct)
{
    VolCtrlWrpCtx_t *pVolCtrlCtx = (VolCtrlWrpCtx_t*) pEffectCtx;

    switch(index) {
        case OMX_DSP_IndexConfigAudioMute:
        {
            VolCtrlConfigMute_t * pMuteConfig = (VolCtrlConfigMute_t *)config_struct;
            pVolCtrlCtx->bMute = pMuteConfig->bMute;
            if(pVolCtrlCtx->sVolData.input_mode != VOLCTRL_INPUT_UNDEFINED) {
                setVolumeConfig(pVolCtrlCtx); //called only when volctrl_init has already been called
            }
            return EFFECT_ERROR_NONE;
        }
        case OMX_DSP_IndexConfigAudioBalance:
        {
            VolCtrlConfigBalance_t * pBalanceConfig = (VolCtrlConfigBalance_t *)config_struct;
            pVolCtrlCtx->nBalance = pBalanceConfig->nBalance;
            if(pVolCtrlCtx->sVolData.input_mode != VOLCTRL_INPUT_UNDEFINED) {
                setVolumeConfig(pVolCtrlCtx); //called only when volctrl_init has already been called
            }
            return EFFECT_ERROR_NONE;
        }
        case OMX_DSP_IndexConfigAudioVolume:
        {
            VolCtrlConfigVolume_t * pVolumeConfig = (VolCtrlConfigVolume_t *)config_struct;
            pVolCtrlCtx->nVolume = pVolumeConfig->sVolume.nValue;
            if(pVolCtrlCtx->sVolData.input_mode != VOLCTRL_INPUT_UNDEFINED) {
                setVolumeConfig(pVolCtrlCtx); //called only when volctrl_init has already been called
            }
            return EFFECT_ERROR_NONE;
        }
        case OMX_DSP_IndexConfigAudioVolumeRamp:
        {
            VolCtrlConfigVolumeRamp_t *pRampConfig = (VolCtrlConfigVolumeRamp_t *) config_struct;
            pVolCtrlCtx->nRampStartVolume   = pRampConfig->sStartVolume.nValue;
            pVolCtrlCtx->nRampEndVolume     = pRampConfig->sEndVolume.nValue;
            pVolCtrlCtx->nRampChannels      = pRampConfig->nChannel;
            pVolCtrlCtx->nRampDuration      = pRampConfig->nRampDuration / 10;
            pVolCtrlCtx->bRampTerminate     = pRampConfig->bRampTerminate;
            if(pVolCtrlCtx->sVolData.input_mode != VOLCTRL_INPUT_UNDEFINED) {
                setVolumeRampConfig(pVolCtrlCtx); //called only when volctrl_init has already been called
            }
            return EFFECT_ERROR_NONE;
        }
        default:
        return EFFECT_ERROR;
    }
}

t_EffectError Volctrl_processBuffer(void* pEffectCtx, Buffer_p inputBuf, Buffer_p outputBuf, bool *needInputBuf, bool *filledOutputBuffer)
{
    VolCtrlWrpCtx_t *pVolCtrlCtx = (VolCtrlWrpCtx_t*) pEffectCtx;
    TRACE_t * this = (TRACE_t *)pVolCtrlCtx->pTraceObject;
    int i;

    while (pVolCtrlCtx->sVolData.new_config) {
        volctrl_set_new_config(&pVolCtrlCtx->sVolData);
    }

    pVolCtrlCtx->process(pEffectCtx, inputBuf, outputBuf);

    // Update pVolCtrlCtx->pVolumeRampInfo fields for OMX_GetConfig command
    pVolCtrlCtx->pVolumeRampInfo->gGLL          = pVolCtrlCtx->sVolData.current_gaindB[GLL]+VOLCTRL_OFFSET_GAIN;
    pVolCtrlCtx->pVolumeRampInfo->gGRR          = pVolCtrlCtx->sVolData.current_gaindB[GRR]+VOLCTRL_OFFSET_GAIN;
    pVolCtrlCtx->pVolumeRampInfo->gTimeLeft     = pVolCtrlCtx->sVolData.current_duration[GLL];
    pVolCtrlCtx->pVolumeRampInfo->gTimeRight    = pVolCtrlCtx->sVolData.current_duration[GRR];

    for(i=0;i<MAX_CH;i++){
        if(pVolCtrlCtx->isRampActivated[i]){
            if(pVolCtrlCtx->sVolData.nb_smooth_iter[i] == 0){
                // Ramp terminated
                pVolCtrlCtx->isRampActivated[i] = 0;
                OstTraceFiltInst2(TRACE_DEBUG,"volctrlmpc::Volctrl_processBuffer -> callback previous ramp (%d) is ending! (ctx=0x%x)", i, (unsigned int)pVolCtrlCtx);
                pVolCtrlCtx->eventHandler(OMX_DSP_EventIndexSettingChanged,i, 0x1, pVolCtrlCtx);
            }
        }
    }

    *needInputBuf = true;
    *filledOutputBuffer = true;

    return EFFECT_ERROR_NONE;
}

Buffer_p Volctrl_getOutputBuffer(void* pEffectCtx)
{
    return NULL; //process inplace
}

void Volctrl_close(void* pEffectCtx)
{
    VolCtrlWrpCtx_t *pVolCtrlCtx = (VolCtrlWrpCtx_t*) pEffectCtx;

    pVolCtrlCtx->sVolData.gain_mask = (GLL_MASK | GRR_MASK);
    pVolCtrlCtx->sVolData.downmix   = 0;
    pVolCtrlCtx->sVolData.db_ramp   = 1;

    /* reset gll and grr to one and glr grl to zero */
    pVolCtrlCtx->sVolData.gain[GLL]          = wL_msl(FORMAT_FLOAT(0.5,MAXVAL), (1 - GUARD_BIT + 24));
    pVolCtrlCtx->sVolData.gain[GRR]          = wL_msl(FORMAT_FLOAT(0.5,MAXVAL), (1 - GUARD_BIT + 24));
    pVolCtrlCtx->sVolData.gain_smoothed[GLL] = pVolCtrlCtx->sVolData.gain[GLL];
    pVolCtrlCtx->sVolData.gain_smoothed[GRR] = pVolCtrlCtx->sVolData.gain[GRR];
    pVolCtrlCtx->sVolData.gain_cmd[0][GLL]   = 0;
    pVolCtrlCtx->sVolData.gain_cmd[0][GRR]   = 0;
    pVolCtrlCtx->sVolData.gain_cmd[1][GLL]   = 0;
    pVolCtrlCtx->sVolData.gain_cmd[1][GRR]   = 0;

    pVolCtrlCtx->sVolData.gain[GLR]          = 0;
    pVolCtrlCtx->sVolData.gain_smoothed[GLR] = pVolCtrlCtx->sVolData.gain[GLR];
    pVolCtrlCtx->sVolData.gain[GRL]          = 0;
    pVolCtrlCtx->sVolData.gain_smoothed[GRL] = pVolCtrlCtx->sVolData.gain[GRL];
    pVolCtrlCtx->sVolData.gain_cmd[0][GLR]   = VOLCTRL_MINIMUM_GAIN;
    pVolCtrlCtx->sVolData.gain_cmd[0][GRL]   = VOLCTRL_MINIMUM_GAIN;
    pVolCtrlCtx->sVolData.gain_cmd[1][GLR]   = VOLCTRL_MINIMUM_GAIN;
    pVolCtrlCtx->sVolData.gain_cmd[1][GRL]   = VOLCTRL_MINIMUM_GAIN;
    pVolCtrlCtx->sVolData.configL       = 1;
    pVolCtrlCtx->sVolData.configR       = 1;
    pVolCtrlCtx->sVolData.terminate[0]  = pVolCtrlCtx->sVolData.terminate[1]=pVolCtrlCtx->sVolData.terminate[2]=pVolCtrlCtx->sVolData.terminate[3]=0;

    pVolCtrlCtx->sVolData.input_mode    = VOLCTRL_INPUT_UNDEFINED;
    pVolCtrlCtx->sVolData.new_config    = 0;
}

void Volctrl_destroy(void* pEffectCtx)
{
    t_ExtendedAllocParams pAllocParams;
    VolCtrlWrpCtx_t *pVolCtrlCtx = (VolCtrlWrpCtx_t*) pEffectCtx;

    pAllocParams.bank = 0;
    pAllocParams.trace_p = (TRACE_t *)pVolCtrlCtx->pTraceObject;

    vfree(pEffectCtx, &pAllocParams);
}

/*************************************************************************
 *  Structure that defines the effect interface + volctrl factory Method
 ************************************************************************/

EffectDescription_t volctrl_def =
{
    Volctrl_construct,
    Volctrl_set_parameter,
    Volctrl_set_config,
    Volctrl_open,
    Volctrl_processBuffer,
    Volctrl_getOutputBuffer,
    Volctrl_close,
    Volctrl_destroy,
};

static EffectDescription_t * Volctrl_factoryMethod(void)
{
    return &volctrl_def;
}

/*************************************************************************
 *  CONSTRUCT -> volctrl registration!
 ************************************************************************/

t_nmf_error METH(construct)(void)
{
    registerEffect("volctrl_effect", Volctrl_factoryMethod);

	return NMF_OK;
}

#endif

