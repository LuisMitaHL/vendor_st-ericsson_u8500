/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * \file    ste3a_art_types.h
 * \brief   STE3A ART types
 * \author  ST-Ericsson Image Quality Team, Agrate (Italy)
 */

#ifndef STE3A_ART_TYPES_H_
#define STE3A_ART_TYPES_H_

#include "ste3a_types.h"


#define STE3A_ART_AS_RULE_MAX_POINTS	4
#define STE3A_ART_ST_RULE_MAX_POINTS	16

enum ste3a_color_rendition_intent
{
	ste3a_color_rendition_intent_modify = STE3A_ENUM_BASE,
	ste3a_color_rendition_intent_substitute
};

enum ste3a_art_interpolation								// interpolation method
{
	ste3a_art_interpolation_linear = STE3A_ENUM_BASE,		//		linear interpolation
	ste3a_art_interpolation_cubic							//		cubic interpolation
};

struct ste3a_art_tuning										// art tuning parameters
{
	ste3a_color_rendition_intent	color_rendition_intent;					// 0x0000 color rendition intent (how the following color matrix has to be used)
	ste3a_color_matrix				color_rendition_matrix;					// 0x0004 color rendition matrix (in the sRGB-primaries space)
	ste3a_float						gamma_mul_r;							// 0x0028 gamma multiplier for the red channel
	ste3a_float						gamma_mul_g;							// 0x002C gamma multiplier for the green channel
	ste3a_float						gamma_mul_b;							// 0x0030 gamma multiplier for the blue channel
	ste3a_count						as_pcount;								// 0x0034 automatic saturation rule nodes
	ste3a_float						as_psnr[STE3A_ART_AS_RULE_MAX_POINTS];	// 0x0038 "
	ste3a_float						as_psat[STE3A_ART_AS_RULE_MAX_POINTS];	// 0x0048 "
	ste3a_count						st_pcount;								// 0x0058 static tone mapping rule nodes
	ste3a_float						st_pival[STE3A_ART_ST_RULE_MAX_POINTS];	// 0x005C "
	ste3a_float						st_poval[STE3A_ART_ST_RULE_MAX_POINTS];	// 0x009C "
	ste3a_bool						dynamic_tonemapping_enable;				// 0x00DC dynamic tone mapping
	ste3a_bool						dt_gain_boost_enable;					// 0x00E0 "
	ste3a_bool						dt_adaptive_gamma_enable;				// 0x00E4 "
	ste3a_bool						dt_scurve_enable;						// 0x00E8 "
	ste3a_bool						dt_black_boost_enable;					// 0x00EC "
	ste3a_gain						dt_gain_boost_white_point;				// 0x00F0 "
	ste3a_float						dt_gamma_min;							// 0x00F4 "
	ste3a_float						dt_gamma_max;							// 0x00F8 "
	ste3a_float						dt_scurve_target;						// 0x00FC "
	ste3a_float						dt_scurve_min;							// 0x0100 "
	ste3a_float						dt_scurve_max;							// 0x0104 "
	ste3a_float						dt_black_boost_len;						// 0x0108 "
	ste3a_float						dt_black_boost_exp;						// 0x010C "
	ste3a_art_interpolation			st_interpolation;						// 0x0110 interpolation method for the static tone mapping function
																			// 0x0114 #### next free offset ####
};

#endif /* STE3A_ART_TYPES_H_ */
