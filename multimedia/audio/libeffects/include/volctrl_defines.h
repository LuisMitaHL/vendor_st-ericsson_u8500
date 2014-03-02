/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   volctrl_defines.h
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _volctrl_defines
#define _volctrl_defines

#define VOLCTRL_VOLUME_MIN  -11400
#define VOLCTRL_VOLUME_MAX  0
#define VOLCTRL_VOLUME_MUTE VOLCTRL_VOLUME_MIN

#define VOLCTRL_BALANCE_ALLRIGHT +100
#define VOLCTRL_BALANCE_ALLLEFT  -100
#define VOLCTRL_BALANCE_CENTER   0

#define VOLCTRL_RAMPDURATION_MAX 10000000
#define VOLCTRL_RAMPDURATION_MIN 1000

#define OMX_AUDIO_ChannelLF     0x1
#define OMX_AUDIO_ChannelRF     0x2
#define OMX_AUDIO_ChannelCF     0x3

#endif //_volctrl_defines

