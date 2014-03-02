/* 
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved. 
 * This code is ST-Ericsson proprietary and confidential. 
 * Any use of the code for whatever purpose is subject to 
 * specific written permission of ST-Ericsson SA. 
 */ 
 
#ifndef EXPOSURE_STATISTICS_PLATFORM_SPECIFIC_H_
#define EXPOSURE_STATISTICS_PLATFORM_SPECIFIC_H_

#   include "exposure_statistics_op_interface.h"
#   include "Platform.h"

#   define DEFAULT_MEAN_ENERGY        (1.0)
#   define DEFAULT_SUM_OF_ZONES       (48)
#   define DEFAULT_STATS_VALID        (Flag_e_FALSE)
#   define DEFAULT_INTERRUPT_COUNT    (0)

#   define DEFAULT_ZONE_GAINS_1_16     (0xffff)
#   define DEFAULT_ZONE_GAINS_17_32    (0xffff)
#   define DEFAULT_ZONE_GAINS_33_48    (0xffff)
#   define DEFAULT_ZONE_OFFSET_X       (0x0)
#   define DEFAULT_ZONE_OFFSET_Y       (0x0)
#   define DEFAULT_ZONE_SIZE_X         (0x100)
#   define DEFAULT_ZONE_SIZE_Y         (0x100)
#   define DEFAULT_ACC_THRESHOLD_LO    (0)
#   define DEFAULT_ACC_THRESHOLD_HI    (0xff)
#   define DEFAULT_EXP_STATISICS_MODE  (ExpStatisticsMode_e_Auto)
#   define DEFAULT_ACC_MODE            (AccMode_e_FULL_PIXELVAL)
#   define DEFAULT_ACC_TYPE            (AccType_e_ACC_ZONED)
#   define DEFAULT_ACC_COLOR           (AccColor_e_GIR)
#   define DEFAULT_ACC_SRC             (AccSrc_e_SRC_IDP_2)
#   define DEFAULT_ACC_STAGGERED       (Flag_e_FALSE)
#   define DEFAULT_EXP_STATS_ENABLE    (Flag_e_TRUE)

/// Sub sampling factor applied by accumulator in X direction
#define SAMPLING_FACTOR_IN_PIXELS_X    (4)

/// Sub sampling factor applied by accumulator in Y direction
#define SAMPLING_FACTOR_IN_PIXELS_Y    (4)

/// Pixel pipe width
#define PIXEL_SIZE_BITS                (8)

/// No of bits accumulated by accumulator
#define PIXEL_SIZE_BITS_ACC            (6)

#endif /*EXPOSURE_STATISTICS_DEVICE_SPECIFIC_H_*/
