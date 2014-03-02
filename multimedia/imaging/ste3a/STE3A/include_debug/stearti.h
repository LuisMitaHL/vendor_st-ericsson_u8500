/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * \file    stearti.h
 * \brief   STE Auto Rendition Tuning internal functions declarations
 * \author  ST-Ericsson Image Quality Team, Agrate (Italy)
 */

#ifndef STEARTI_H_
#define STEARTI_H_

#include "nmx.h"
#include "nmxAawb.h"

struct steart_internal
{
	nmxMatrix	grid;							// input data
	nmxMatrix	hist_rgb_in_1;					// intermediates copy of input data
	nmxMatrix	hist_rgb_in;					// intermediates copy of input data
	nmxMatrix	hist_rgb_cg;					// intermediates histograms after color gains
	nmxMatrix	hist_Y_inp;						// intermediates input histogram to nonlinear tone mapping
	nmxMatrix	hist_Y_out;						// intermediates output histogram from nonlinear tone mapping
	nmxMatrix	hist_Y_tmp;						// intermediates internal temporary buffer
	nmxMatrix	lut_out;						// intermediates output lookup-table for tone mapping
	nmxMatrix	lut_tmp;						// intermediates internal temporary buffer
	nmxMatrix	scale;							// intermediates scale for lookup tables
	nmxMatrix	vtemp1;							// intermediates internal temporary buffer
	nmxMatrix	vtemp2;							// intermediates internal temporary buffer
	nmxMatrix	vtemp3;							// intermediates internal temporary buffer
	nmxMatrix	gain_boost;						// intermediates scalar
	nmxMatrix	gamma_out;						// intermediates scalar
	nmxMatrix	scurve_proportion;				// intermediates scalar
};


void stearti_init_internal_structure(
									const	ste3a_art_intermediates	&intermediate,
											steart_internal			&internal );

void stearti_initialize_tuning(				ste3a_art_tuning				& tuning);

void stearti_compute_static_tm(		const	ste3a_art_tuning				& tuning,
											ste3a_count						  gains_count,
											ste3a_gain						* gains_value);

void stearti_compute_dynamic_tm(
									const	ste3a_art_tuning				& tuning,
											steart_internal					& internal,
									const	ste3a_histograms_stats_status	& stats_hist,
											ste3a_isp_cg_config				& isp_cg_config,
											ste3a_isp_dg_config				& isp_dg_config,
											ste3a_isp_er_config				& isp_er_config,
											ste3a_isp_cs_config				& isp_cs_config,
											ste3a_count						  gains_count,
											ste3a_gain						* gains_value);

void stearti_compute_cm(			const	ste3a_art_tuning				& tuning,
									const	ste3a_normalized_snr			  snr,
									const	ste3a_color_matrix				& imat,
											ste3a_color_matrix				& omat);

void stearti_multiply_cm_cm(		const	ste3a_color_matrix				& inner,
									const	ste3a_color_matrix				& outer,
											ste3a_color_matrix				& result);

void stearti_multiply_g_cm(			const	ste3a_gain						& gain,
									const	ste3a_color_matrix				& color_matrix,
											ste3a_color_matrix				& result);

void stearti_add_cm_cm(				const	ste3a_color_matrix				& a1,
									const	ste3a_color_matrix				& a2,
											ste3a_color_matrix				& result);

ste3a_float stearti_apply_rule_cubic(	const	ste3a_float						  input,
										const	ste3a_count						  pcount,
										const	ste3a_float						* pival,
										const	ste3a_float						* poval);

ste3a_float stearti_apply_rule_linear(	const	ste3a_float						  input,
										const	ste3a_count						  pcount,
										const	ste3a_float						* pival,
										const	ste3a_float						* poval);

#endif /* STEARTI_H_ */
