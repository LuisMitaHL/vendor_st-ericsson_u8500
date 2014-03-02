/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   effect_dsp_types.h
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _effect_dsp_types
#define _effect_dsp_types

//OMX_AUDIO_CONFIG_EFFECTADDPOSITIONTYPE
typedef struct AudioConfigAddEffect_t {
    t_uint24    cEffectName[128]; //contain effect name that can hold 128 characters plus zero termination
} AudioConfigAddEffect_t;

typedef struct AudioConfigPcmProbe_t {
    t_uint24    bEnable;
    t_uint24    buffer;
    t_uint24    buffer_size;
} AudioConfigPcmProbe_t;

#endif //_effect_dsp_types

