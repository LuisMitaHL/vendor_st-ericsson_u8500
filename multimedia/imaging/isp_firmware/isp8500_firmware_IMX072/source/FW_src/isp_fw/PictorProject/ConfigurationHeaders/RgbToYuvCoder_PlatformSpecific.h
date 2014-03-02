/* 
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved. 
 * This code is ST-Ericsson proprietary and confidential. 
 * Any use of the code for whatever purpose is subject to 
 * specific written permission of ST-Ericsson SA. 
 */ 
 
/**
 \file RgbToYuvCoder_PlatformSpecific.h
 \brief This file is NOT a part of the module release code.
	 		All inputs needed by the RGB to YUV coder module that can only
	 		be resolved at the project level (at integration time)
	 		are met through this file.
	 		It is the responsibility of the integrator to generate
	 		this file at integration time and meet all the input
	 		dependencies.


 \note      The following sections define the dependencies that must be
           met by the system integrator. The way these dependencies
           have been resolved here are just for example. These dependencies
           must be appropriately resolved based on the platform being used.
 \ingroup RgbToYuvCoder
*/
#ifndef _RGBTOYUVCODER_PLATFORMSPECIFIC_H_
#   define _RGBTOYUVCODER_PLATFORMSPECIFIC_H_

#   include "Platform.h"

/// No of H/W pipes supported
#   define RGBTOYUVCODER_NO_OF_HARDWARE_PIPE_IN_ISP    (2)

/// ISP8500 give 10 bit RGB data, so maximum data will be in 1023 range i.e. pipe excursion
#   define RGBTOYUVCODER_MAX_INPUT_FROM_PIPE   (1023)

/// Default Output format for Pipe
#   define RGBTOYUVCODER_DEFAULT_YCBCR_TRANSFORM   (Transform_e_YCbCr_JFIF)

/// DEFAULT contrast and saturation value, 100 = 100%, 120 = 120%
#   define RGBTOYUVCODER_DEFAULT_VALUE_100_PERCENT (100)

/// DEFAULT Maximum contrast value, 100 = 100%, 120 = 120%
#   define RGBTOYUVCODER_DEFAULT_MAX_CONTRAST  (2 * RGBTOYUVCODER_DEFAULT_VALUE_100_PERCENT)

/// DEFAULT Maximum saturation value, 100 = 100%, 120 = 120%
#   define RGBTOYUVCODER_DEFAULT_MAX_SATURATION    (2 * RGBTOYUVCODER_DEFAULT_VALUE_100_PERCENT)

/// Default Black level value, 0.0 fades image to absolute black. 1.0 disables fade to black functionality
#   define RGBTOYUVCODER_DEFAULT_BLACK_VALUE   (0.0)

/// Lower threshold for fade to black
#   define RGBTOYUVCODER_DEFAULT_LOW_THRESHOLD (500224.0)

/// Higher threshold for fade to black
#   define RGBTOYUVCODER_DEFAULT_HIGH_THRESHOLD    (900096.0)
#endif // _RGBTOYUVCODER_PLATFORMSPECIFIC_H_

