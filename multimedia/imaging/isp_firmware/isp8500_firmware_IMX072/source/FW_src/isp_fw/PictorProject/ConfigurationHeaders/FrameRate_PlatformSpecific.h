/* 
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved. 
 * This code is ST-Ericsson proprietary and confidential. 
 * Any use of the code for whatever purpose is subject to 
 * specific written permission of ST-Ericsson SA. 
 */ 
 
/**
 \file FrameRate_PlatformSpecific.h
 \brief This file is NOT a part of the module release code.
        All inputs needed by the Frame Rate module that can only
        be resolved at the project level (at integration time)
        are met through this file.
        It is the responsibility of the integrator to generate
        this file at integration time and meet all the input
        dependencies.

 \note  The following sections define the dependencies that must be
           met by the system integrator. The way these dependencies
           have been resolved here are just for example. These dependencies
           must be appropriately resolved based on the platform being used.
 \ingroup FrameRate
*/
#ifndef _FRAME_RATE_PLATFORM_SPECIFIC_H_
#   define _FRAME_RATE_PLATFORM_SPECIFIC_H_

#   include "Platform.h"

/// Low threshold system gain, system gain will not go below this value
#   define DEFAULT_SYSTEM_GAIN_THRESHOLD_LOW   (1.0)

/// High threshold system gain, system gain will not go above this value
#   define DEFAULT_SYSTEM_GAIN_THRESHOLD_HIGH  (2.0)

/// Default minimum frame rate
#   define DEFAULT_FRAME_RATE_MINIMUM  (1.0)

/// Default maximum frame rate
#   define DEFAULT_FRAME_RATE_MAXIMUM  (30.0)

/// Default frame rate in manual mode
#   define DEFAULT_FRAME_RATE_COLD_START   (1.0)

/// Relative change = 25%
#   define DEFAULT_REALATIVE_CHANGE    (0.25)

/// Default: exposure and frame rate are linked. Frame rate will be caped to user maximum
/// integration time or maximum exposure time possible, whichever is maximum
#   define DEFAULT_LINK_FRAME_RATE_AND_MAX_USER_INTEGRATION_TIME   (Flag_e_FALSE)

/// Default mode in which frame rate has to be run
#   define DEFAULT_FRAME_RATE_MODE                (FrameRateMode_e_SINGLE_STEP)

/// Default coin for using frame rate in manual mode.
#   define DEFAULT_COIN_FRAME_RATE_CHANGE         (Coin_e_Tails)


/// Default Implied gain of system
#   define DEFAULT_FRAME_RATE_IMPLIED_GAIN              (1.0)

/// Default Desired frame rate
#   define DEFAULT_FRAME_RATE_DESIRED_FRAME_RATE_HZ     (30.0)

/// Default frame length corresponding to DEFAULT_FRAME_RATE_MAXIMUM
#   define DEFAULT_FRAME_RATE_MIN_FRAME_LENGTH_LINES    (22)          // minimum frame length: 851

/// Default frame length corresponding to DEFAULT_FRAME_RATE_MINIMUM
#   define DEFAULT_FRAME_RATE_MAX_FRAME_LENGTH_LINES    (65535)       // maximum frame length: 851

/// Default change in lines form current frame length
#   define DEFAULT_FRAME_RATE_FRAME_LENGTH_CHANGE_LINES (0)

/// Default frame length desired to achieve f_DesiredFrameRate_Hz
#   define DEFAULT_FRAME_RATE_DESIRED_FRAME_LENGTH_LINES (0)

/// Default frame rate control current frame rate
#   define DEFAULT_VARIABLE_FRAMERATE_CONTROL_CURRENT_FRAME_RATE    (30)       // maximum frame length: 851

/// Default frame rate control maximum frame rate
#   define DEFAULT_VARIABLE_FRAMERATE_CONTROL_MAXIMUM_FRAME_RATE (30)

/// Default frame rate control enable flage
#   define DEFAULT_VARIABLE_FRAMERATE_CONTROL_FLAG (Flag_e_FALSE)

#endif // _FRAME_RATE_PLATFORM_SPECIFIC_H_

