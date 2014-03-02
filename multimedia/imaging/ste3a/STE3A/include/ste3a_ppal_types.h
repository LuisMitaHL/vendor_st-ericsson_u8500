/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * \file    ste3a_ppal_types.h
 * \brief   STE3A pixel pipe abstraction types
 * \author  ST-Ericsson Image Quality Team, Agrate (Italy)
 */

#ifndef STE3A_PPAL_TYPES_H_
#define STE3A_PPAL_TYPES_H_

#include "ste3a_types.h"

#define STE3A_PPAL_TONEMAPPING_LUT_SIZE		(256 + 1)

struct ste3a_isp_cg_config													// ISP channel gains stage descriptor
{
	ste3a_gain						mul_rr;									//		digital gain for the red channel
	ste3a_gain						mul_gr;									//		digital gain for the green/red channel
	ste3a_gain						mul_gb;									//		digital gain for the green/blue channel
	ste3a_gain						mul_bb;									//		digital gain for the blue channel
};

struct ste3a_isp_dg_config													// ISP digital gain stage descriptor
{
	ste3a_gain						mul;									//		digital gain
};

struct ste3a_isp_cm_config													// ISP colour matrix stage descriptor
{
	ste3a_color_matrix				mul;									//		color matrix
};

struct ste3a_isp_er_config													// ISP exposure recovery descriptor
{
	ste3a_gain						mul;									//		exposure recovery gain
};

struct ste3a_isp_cs_config													// ISP contrast stretch descriptor
{
	ste3a_normalized_pixel_level	pre_sub;								//		offset (1st step)
	ste3a_gain						post_mul;								//		gain (2nd step)
};

struct ste3a_isp_tm_config													// ISP tone mapping descriptor
{
	ste3a_normalized_pixel_level	lut[STE3A_PPAL_TONEMAPPING_LUT_SIZE];	//		tone mapping gains table
};

struct ste3a_isp_gc_config													// ISP gamma coding descriptor
{
	ste3a_float gamma_mul_r;
	ste3a_float gamma_mul_g;
	ste3a_float gamma_mul_b;
};
#endif /* STE3A_PPAL_TYPES_H_ */
