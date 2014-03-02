/* 
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved. 
 * This code is ST-Ericsson proprietary and confidential. 
 * Any use of the code for whatever purpose is subject to 
 * specific written permission of ST-Ericsson SA. 
 */ 
 
/*
\file Adsoc_platform_specific.h
 */

#ifndef ADSOC_PLATFORM_SPECIFIC_H_
#define ADSOC_PLATFORM_SPECIFIC_H_

#include "Platform.h"
#include "Adsoc.h"

#define  DEFAULT_ADSOC_PK_ENABLE                       (Flag_e_FALSE)
#define  DEFAULT_ADSOC_PK_ADAPTIVE_SHARPENING_ENABLE   (Flag_e_FALSE)
#define  DEFAULT_ADSCO_PK_CORING_LEVEL                 (0)
#define  DEFAULT_ADSOC_PK_OVERSHOOT_GAIN_BRIGHT        (0)
#define  DEFAULT_ADSOC_PK_OVERSHOOT_GAIN_DARK          (0)
#define  DEFAULT_ADSOC_PK_EMBOSS_EFFECT_CTRL           (0)
#define  DEFAULT_ADSOC_PK_FLIPPER_CTRL                 (0)
#define  DEFAULT_ADSOC_PK_GRAYBACK_CTRL                (0)
#define  DEFAULT_ADSOC_PK_GAIN                         (1)


#define  DEFAULT_RADIAL_ADSOC_RP_ENABLE          (Flag_e_FALSE)
#define  DEFAULT_RADIAL_ADSOC_RP_POLYCOEF0           (0)
#define  DEFAULT_RADIAL_ADSOC_RP_POLYCOEF1                 (0)
#define  DEFAULT_RADIAL_ADSOC_RP_COF_SHIFT                (0)
#define  DEFAULT_RADIAL_ADSOC_RP_OUT_COF_SHIFT                    (0)
#define  DEFAULT_RADIAL_ADSOC_RP_UNITY                    (0)


#define  DEFAULT_RADIAL_ADSOC_RP_HOFFSET                       (0)
#define  DEFAULT_RADIAL_ADSOC_RP_VOFFSET                       (0)
#define  DEFAULT_RADIAL_ADSOC_RP_SCALE_X                         (1)
#define  DEFAULT_RADIAL_ADSOC_RP_SCALE_Y        (1)


//#define ADSOC_DEFAULT_OVERSHOOT_GAIN_BRIGHT_PLACEMENT   1
//#define ADSOC_DEFAULT_OVERSHOOT_GAIN_DARK_PLACEMENT     2


#endif /* ADSOC_PLATFORM_SPECIFIC_H_ */
