/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   volctrl_api.h
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _volctrl_api_h_
#define _volctrl_api_h_

typedef struct Volume_t {
    t_sint24    nValue;
    t_sint24    nMin;
    t_sint24    nMax;
} Volume_t;

//OMX_AUDIO_CONFIG_MUTETYPE
typedef struct VolCtrlConfigMute_t {
    t_uint24    bMute;
} VolCtrlConfigMute_t;

//OMX_AUDIO_CONFIG_VOLUMETYPE
typedef struct VolCtrlConfigVolume_t {
    t_uint24    bLinear;
    Volume_t    sVolume;
} VolCtrlConfigVolume_t;

//OMX_AUDIO_CONFIG_BALANCETYPE
typedef struct VolCtrlConfigBalance_t {
    t_sint24    nBalance;
} VolCtrlConfigBalance_t;

//OMX_SYMBIAN_AUDIO_CONFIG_VOLUMERAMPTYPE
//RampDuration fields are defined as OMX_U64 is standard OMX.
//Check if we really need 64bits or if 24 is enough!
typedef struct VolCtrlConfigVolumeRamp_t {
    t_uint24    nChannel;
    t_uint24    bLinear;
    Volume_t    sStartVolume;
    Volume_t    sEndVolume;
    t_uint24    nRampDuration;
    t_uint24    bRampTerminate;
    Volume_t    sCurrentVolume;     //Not used
    t_uint48    nRampCurrentTime;   //Not used
    t_uint24    nRampMinDuration;   //Not used
    t_uint24    nRampMaxDuration;   //Not used
    t_uint24    nVolumeStep;        //Not used
} VolCtrlConfigVolumeRamp_t;

//OMX_DSP_IndexParamSetVolumeRampStructAddr
//Structure used by HOST to retrieve volumeRamp informations
typedef struct VolCtrlVolumeRampInfo_t {
    t_uint24 gGLL;
    t_uint24 gGRR;
    t_uint24 gTimeLeft;
    t_uint24 gTimeRight;
} VolCtrlVolumeRampInfo_t;

#endif //_volctrl_api_h_

