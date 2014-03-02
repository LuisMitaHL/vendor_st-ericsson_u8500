/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   MDRCMpcConfigs.h
 * \brief  Redefine OMX structures at 24-bits DSP format
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _MDRC_MPC_CONFIGS_H_
#define _MDRC_MPC_CONFIGS_H_

//AFM_AUDIO_PARAM_MDRC
typedef struct
{
    t_uint32 mChannels;     //STATIC, 0 : mono, 1 : stereo, default 0
    t_uint32 mSamplingFreq; //STATIC,
    t_uint32 mFilterKind;
    t_uint32 mBiquadKind;
    t_uint32 mFirKind;
} ARMMdrcParams_t;

#endif //_MDRC_MPC_CONFIGS_H_

