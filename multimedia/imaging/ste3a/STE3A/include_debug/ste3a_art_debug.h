/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * \file    ste3a_art_debug.h
 * \brief   STE3A ART debug types
 * \author  ST-Ericsson Image Quality Team, Agrate (Italy)
 */

#ifndef STE3A_ART_DEBUG_H_
#define STE3A_ART_DEBUG_H_

#include "ste3a_types.h"

#define STE3A_ART_LUT_HIST_SIZE		257
#define STE3A_ART_HIST_RGB_CH		3

struct ste3a_art_lut_hist
{
	ste3a_float				vector[STE3A_ART_LUT_HIST_SIZE];
};

struct ste3a_art_rgb_hist
{
	ste3a_art_lut_hist	red;
	ste3a_art_lut_hist	green;
	ste3a_art_lut_hist	blue;
};

struct ste3a_art_intermediates					// art intermediate values
{
	ste3a_art_rgb_hist	hist_rgb_in;			// input histograms
	ste3a_art_rgb_hist	hist_rgb_cg;			// histograms after color gains
	ste3a_art_lut_hist	hist_Y_inp;				// input histogram to nonlinear tone mapping
	ste3a_art_lut_hist	hist_Y_out;				// output histogram from nonlinear tone mapping
	ste3a_art_lut_hist	hist_Y_tmp;				// internal temporary buffer
	ste3a_art_lut_hist	lut_out;				// output lookup-table for tone mapping
	ste3a_art_lut_hist	lut_tmp;				// internal temporary buffer
	ste3a_art_lut_hist	scale;					// scale for lookup tables
	ste3a_art_lut_hist	vtemp1;					// internal temporary buffer
	ste3a_art_lut_hist	vtemp2;					// internal temporary buffer
	ste3a_art_lut_hist	vtemp3;					// internal temporary buffer
	ste3a_gain			gain_boost;				// output from gain boost function
	ste3a_float			gamma_out;				// output from adaptive gamma function
	ste3a_float			scurve_proportion;		// output from scurve function
};

#endif /* STE3A_ART_DEBUG_H_ */
