/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   SPLMpcConfigs.h
 * \brief  Redefine OMX structures at 24-bits DSP format
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _SPL_MPC_CONFIGS_H_
#define _SPL_MPC_CONFIGS_H_

//AFM_AUDIO_PARAM_SPLTYPE
typedef struct {
    t_uint32 nType;             /**< Limiter type: 0 sound pressure, 1 peak */
    t_uint32 nAttackTime;       /**< Gain adaptation attack time in peak limiter in ms */
    t_uint32 nReleaseTime;      /**< Gain adaptation release time in peak limiter in ms*/
} ARMParamSpl_t;

//OMX_AUDIO_CONFIG_SPLLIMITTYPE
typedef struct ARMSpllimitConfigType_t {
    t_uint32 bEnable;
    t_uint32 eMode;
    t_uint32 nSplLimit_high;
    t_uint32 nSplLimit_low;
    t_uint32 nGain_high;
    t_uint32 nGain_low;
} ARMSpllimitConfigType_t;

#endif //_SPL_MPC_CONFIGS_H_

