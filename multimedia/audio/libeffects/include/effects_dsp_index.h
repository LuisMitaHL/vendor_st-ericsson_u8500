/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   effect_dsp_index.h
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _effect_dsp_index
#define _effect_dsp_index

typedef enum {
    AFM_DSP_IndexConfigNone,
    OMX_DSP_ParamSetEffectInfoPtr,
    OMX_DSP_IndexConfigAudioEffectAddPosition,
    OMX_DSP_IndexConfigAudioEffectRemovePosition,
    //For PcmProbe support
    AFM_DSP_IndexConfigPcmProbe,
    //Specific Volctrl DSP indexes
    OMX_DSP_IndexConfigAudioVolume,
    OMX_DSP_IndexConfigAudioBalance,
    OMX_DSP_IndexConfigAudioMute,
    OMX_DSP_IndexConfigAudioVolumeRamp,
    //Specific SRC DSP indexes
    AFM_DSP_IndexParamSrcMode,
    //Specific SPL DSP indexes
    AFM_DSP_IndexParamSpl,
    OMX_DSP_IndexConfigSpllimit,
    OMX_DSP_IndexStartEffectSpecific,
    //Specific TEQ DSP indexes
    AFM_DSP_IndexParamTeq,
    AFM_DSP_IndexConfigTeq,
    //Specific MDRC DSP indexes
    AFM_DSP_IndexParamMdrc,
    AFM_DSP_IndexConfigMdrc
} t_EffectDSPIndexes;

#endif //_effect_dsp_index

