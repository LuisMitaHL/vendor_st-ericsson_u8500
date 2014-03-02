/* 
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved. 
 * This code is ST-Ericsson proprietary and confidential. 
 * Any use of the code for whatever purpose is subject to 
 * specific written permission of ST-Ericsson SA. 
 */ 
 
/**
 \file videotiming_PlatformSpecific.h
 \brief This file is NOT a part of the module release code.
	 		All inputs needed by the video timing module that can only
	 		be resolved at the project level (at integration time)
	 		are met through this file.
	 		It is the responsibility of the integrator to generate
	 		this file at integration time and meet all the input
	 		dependencies

 \ingroup VideoTiming
*/
#ifndef _VT_PLATFORM_SPECIFIC_H_
#   define _VT_PLATFORM_SPECIFIC_H_

#   include "Platform.h"

/// Default values for Host Inputs
#   define VIDEO_TIMING_DEFAULT_MAX_RATE_Mbps              (640.0)
#   define VIDEO_TIMING_DEFAULT_OP_CLK_DERATING_FRAC       (1.0)

#   define VIDEO_TIMING_DEFAULT_CSI_RAW_FORMAT             (0x0A0A)

#   define VIDEO_TIMING_DEFAULT_VIDEO_TIMING_MODE          (VideoTimingMode_e_VideoTimingMode_Automatic)
#   define VIDEO_TIMING_DEFAULT_OP_CLK_DERATING_MODE       (DeratingRoundingMode_e_ROUND_UP)

#   define VIDEO_TIMING_DEFAULT_SENSOR_BITS_PER_SYS_CLK    (SensorBitsPerSystemClock_e_DATA_CLOCK)

#   define VIDEO_TIMING_DEFAULT_DERATE_VT_Clk_PROFILE_0    (Flag_e_FALSE)
#   define VIDEO_TIMING_DEFAULT_VSYNC_POLARITY             (Polarity_e_Polarity_ActiveLow)
#   define VIDEO_TIMING_DEFAULT_HSYNC_POLARITY             (Polarity_e_Polarity_ActiveLow)


#endif // _VT_PLATFORM_SPECIFIC_H_

