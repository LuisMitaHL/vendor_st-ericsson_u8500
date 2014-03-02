/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef MINIMUM_WEIGHTED_STATISTICS_PLATFORM_SPECIFIC__H_
#define MINIMUM_WEIGHTED_STATISTICS_PLATFORM_SPECIFIC__H_

#include "Platform.h"

/// No of Active lines from sensor or Y size from sensor
#   define MWWB_GetNoOfActiveLinesFromSensor() (600)

// Tilt Gains
#   define MWWB_TILT_GAIN_RED()    (1.0)
#   define MWWB_TILT_GAIN_GREEN1() (1.0)
#   define MWWB_TILT_GAIN_GREEN2() (1.0)
#   define MWWB_TILT_GAIN_BLUE()   (1.0)

// AG TODO: data sheet reference
#   define MAXIMUM_NUMBER_OF_MACRO_PIXELS_SUPPORTED    0x20000

#define MWWB_INCLUDE_MWWB           PLATFORM_MWWB_INCLUDE_MWWB

#endif /*MINIMUM_WEIGHTED_STATISTICS_PLATFORM_SPECIFIC__H_*/

