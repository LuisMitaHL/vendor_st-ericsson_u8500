/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   spl_api.h
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _spl_api_h_
#define _spl_api_h_

//AFM_AUDIO_PARAM_SPLTYPE
typedef struct SplParamType_t {
    t_uint24 nType;             /**< Limiter type: 0 sound pressure, 1 peak */
    t_uint24 nAttackTime;       /**< Gain adaptation attack time in peak limiter in ms */
    t_uint24 nReleaseTime;      /**< Gain adaptation release time in peak limiter in ms*/
} SplParamType_t;

//OMX_AUDIO_CONFIG_SPLLIMITTYPE
typedef struct SpllimitConfigType_t {
    t_uint24 bEnable;
    t_uint24 eMode;
    t_sint48 nSplLimit;
    t_sint48 nGain;
} SpllimitConfigType_t;

//OMX_AUDIO_CONFIG_VOLUMETYPE
typedef struct SplVolume_t {
    t_sint24    nValue;
    t_sint24    nMin;
    t_sint24    nMax;
} SplVolume_t;

typedef struct SplVolCtrlConfigVolume_t {
    t_uint24    bLinear;
    SplVolume_t sVolume;
} SplVolCtrlConfigVolume_t;

#endif //_spl_api_h_

