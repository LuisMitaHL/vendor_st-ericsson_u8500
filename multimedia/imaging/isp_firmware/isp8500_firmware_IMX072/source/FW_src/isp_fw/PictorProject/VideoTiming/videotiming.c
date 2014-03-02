/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 \file videotiming.c
 \brief define function required for Video Timing module
 \ingroup VideoTiming
*/
#include "videotiming_op_interface.h"

VideoTimingHostInputs_ts    g_VideoTimingHostInputs =
{
    VIDEO_TIMING_DEFAULT_MAX_RATE_Mbps,
    VIDEO_TIMING_DEFAULT_OP_CLK_DERATING_FRAC,
    VIDEO_TIMING_DEFAULT_CSI_RAW_FORMAT,
    VIDEO_TIMING_DEFAULT_VIDEO_TIMING_MODE,
    VIDEO_TIMING_DEFAULT_SENSOR_BITS_PER_SYS_CLK,
    VIDEO_TIMING_DEFAULT_OP_CLK_DERATING_MODE,
    VIDEO_TIMING_DEFAULT_DERATE_VT_Clk_PROFILE_0,
    VIDEO_TIMING_DEFAULT_VSYNC_POLARITY,
    VIDEO_TIMING_DEFAULT_HSYNC_POLARITY
    };

// video timing output
VideoTimingOutput_ts        g_VideoTimingOutput;

