/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * \file    steaec.h
 * \brief   STE Auto Exposure Control interface
 * \author  ST-Ericsson Image Quality Team, Agrate (Italy)
 */

#ifndef STEAEC_H_
#define STEAEC_H_

#include "ste3a_aec_types.h"
#include "ste3a_aec_debug.h"
#include "ste3a_cmal_types.h"
#include "ste3a_fgal_types.h"
#include "ste3a_ppal_types.h"
#include "ste3a_stats_types.h"


#define STEAEC_STATS_HIST_BINS_MAX_COUNT		256
#define STEAEC_STATS_HIST_CHANNELS_MAX_COUNT	3
#define STEAEC_STATS_GRID_SIZEX_MAX_COUNT		80
#define STEAEC_STATS_GRID_SIZEY_MAX_COUNT		60
#define STEAEC_STATS_GRID_CHANNELS_MAX_COUNT	4


struct steaec_stats_status
{
	ste3a_histograms_stats_status	hist;
	ste3a_grid_stats_status			grid;
};

struct ste3a_histograms_stats_sample
{
	ste3a_histstats_val values[STEAEC_STATS_HIST_BINS_MAX_COUNT * STEAEC_STATS_HIST_CHANNELS_MAX_COUNT];
};

struct ste3a_grid_stats_sample
{
	ste3a_gridstats_val values[STEAEC_STATS_GRID_SIZEX_MAX_COUNT * STEAEC_STATS_GRID_SIZEY_MAX_COUNT * STEAEC_STATS_GRID_CHANNELS_MAX_COUNT];
};

struct steaec_stats_sample
{
	ste3a_histograms_stats_sample hist;
	ste3a_grid_stats_sample       grid;
};

struct steaec_exposure_input_noflash
{
	ste3a_cammod_exposure_status	cammod;
	steaec_stats_sample				stats;

	ste3a_bool						valid;
	ste3a_timestamp					timestamp;
};

struct steaec_exposure_input_flash
{
	ste3a_cammod_exposure_status	cammod;
	ste3a_flash_exposure_status		flash;
	steaec_stats_sample				stats;

	ste3a_bool						valid;
	ste3a_timestamp					timestamp;
};

struct steaec_exposure_output_noflash
{
	ste3a_light_level				light_level;
	ste3a_normalized_pixel_level	unexposed_black_point;
	ste3a_exposure					requested_exposure_logical;
	ste3a_exposure					requested_exposure_physical;
	ste3a_exposure					damped_exposure_logical;
	ste3a_exposure					damped_exposure_physical;
	ste3a_exposure					actual_exposure;
	ste3a_exposure					residual_exposure;
	ste3a_sensitivity				sensitivity;

	ste3a_bool						valid;
	ste3a_timestamp					timestamp;
};

struct steaec_exposure_output_flash
{
	ste3a_light_level				light_level;
	ste3a_normalized_pixel_level	unexposed_black_point;
	ste3a_exposure					requested_exposure_logical;
	ste3a_exposure					requested_exposure_physical;
	ste3a_exposure					actual_exposure;
	ste3a_exposure					residual_exposure;
	ste3a_sensitivity				sensitivity;

	ste3a_energy					requested_energy;
	ste3a_energy					actual_energy;
	ste3a_gain						residual_gain;

	ste3a_bool						valid;
	ste3a_timestamp					timestamp;
};

struct steaec_exposure_status_noflash
{
	steaec_exposure_input_noflash	input;
	steaec_exposure_output_noflash	output;

	ste3a_bool						fast_coldstart;
};

struct steaec_exposure_status_flash
{
	steaec_exposure_input_flash		input;
	steaec_exposure_output_flash	output;
};

enum steaec_exposure_status_source
{
	steaec_exposure_status_source_none,
	steaec_exposure_status_source_noflash,
	steaec_exposure_status_source_flash
};

struct steaec_exposure_status
{
	steaec_exposure_status_noflash	noflash;
	steaec_exposure_status_flash  	flash;

	steaec_exposure_status_source	last;
};

struct steaec_stats_descriptor
{
	ste3a_histograms_stats_descriptor	hist;
	ste3a_grid_stats_descriptor			grid;
};

enum steaec_weighting_mode
{
	steaec_weighting_mode_static,
	steaec_weighting_mode_dynamic
};

struct steaec_obj
{
	ste3a_common_callbacks	com_cbs;
	ste3a_aec_callbacks		aec_cbs;

	steaec_stats_descriptor ext_stats_descriptor;
	steaec_stats_descriptor int_stats_descriptor;

	ste3a_aec_tuning tuning;

	ste3a_aec_intermediates intermediates;

	steaec_exposure_status active_status;
	steaec_exposure_status saved_status;
	steaec_exposure_status temp_status;

	ste3a_float sweights[STEAEC_STATS_GRID_SIZEX_MAX_COUNT][STEAEC_STATS_GRID_SIZEY_MAX_COUNT];
	ste3a_float dweights[STEAEC_STATS_GRID_SIZEX_MAX_COUNT][STEAEC_STATS_GRID_SIZEY_MAX_COUNT];
	ste3a_float fweights[STEAEC_STATS_GRID_SIZEX_MAX_COUNT][STEAEC_STATS_GRID_SIZEY_MAX_COUNT];
	ste3a_float bweights[STEAEC_STATS_GRID_SIZEX_MAX_COUNT][STEAEC_STATS_GRID_SIZEY_MAX_COUNT];

	steaec_weighting_mode weighting_mode;
};

steaec_obj * steaec_open(		const	ste3a_common_callbacks			& com_cbs,
								const	ste3a_aec_callbacks				& aec_cbs,
								const	steaec_stats_descriptor			& stats_descriptor);

void steaec_close(						steaec_obj						* aec);

void steaec_stats_geometry_histograms(
										steaec_obj						* aec,
								const	ste3a_zoom_factor				  dzoom_factor,
										ste3a_histograms_stats_geometry	& geometry);

void steaec_compute_coldstart_exposure(
										steaec_obj						* aec,
								const	ste3a_light_level				  light_level);

void steaec_reset_coldstart_exposure(	steaec_obj						* aec);

void steaec_update_noflash(				steaec_obj						* aec,
								const	ste3a_timestamp					  current_time,
								const	ste3a_cammod_exposure_status	& cammod_status,
								const	steaec_stats_status				& stats_status);

void steaec_update_flash(				steaec_obj						* aec,
								const	ste3a_timestamp					  current_time,
								const	ste3a_cammod_exposure_status	& cammod_status,
								const	ste3a_flash_exposure_status		& flash_status,
								const	steaec_stats_status				& stats_status);

void steaec_evaluate_noflash(			steaec_obj						* aec,
								const	ste3a_timestamp					  current_time,
								const	ste3a_bool						  fast_mode,
								const	ste3a_isp_cg_config				& isp_cg_config,
										ste3a_cammod_exposure_config	& cammod_config,
										ste3a_isp_dg_config				& isp_dg_config,
										ste3a_isp_er_config				& isp_er_config,
										ste3a_isp_cs_config				& isp_cs_config,
										ste3a_damper_bases				& damper_bases,
										ste3a_aec_scene_info			& scene_info,
										ste3a_bool						& stable,
										ste3a_bool						& flash);

void steaec_evaluate_flash(				steaec_obj						* aec,
								const	ste3a_timestamp					  current_time,
								const	ste3a_aec_flash_mode			  flash_mode,
								const	ste3a_isp_cg_config				& isp_cg_config,
										ste3a_cammod_exposure_config	& cammod_config,
										ste3a_flash_exposure_config		& flash_config,
										ste3a_isp_dg_config				& isp_dg_config,
										ste3a_isp_er_config				& isp_er_config,
										ste3a_isp_cs_config				& isp_cs_config,
										ste3a_damper_bases				& damper_bases,
										ste3a_aec_scene_info			& scene_info,
										ste3a_bool						& stable);

void steaec_evaluate_still_noflash(		steaec_obj						* aec,
								const	ste3a_timestamp					  current_time,
								const	ste3a_cammod_exposure_status	& cammod_status,
								const	ste3a_isp_cg_config				& isp_cg_config,
										ste3a_isp_dg_config				& isp_dg_config,
										ste3a_isp_er_config				& isp_er_config,
										ste3a_isp_cs_config				& isp_cs_config,
										ste3a_damper_bases				& damper_bases,
										ste3a_aec_scene_info			& scene_info);

void steaec_evaluate_still_flash(		steaec_obj						* aec,
								const	ste3a_timestamp					  current_time,
								const	ste3a_cammod_exposure_status	& cammod_status,
								const	ste3a_flash_exposure_status		& flash_status,
								const	ste3a_isp_cg_config				& isp_cg_config,
										ste3a_isp_dg_config				& isp_dg_config,
										ste3a_isp_er_config				& isp_er_config,
										ste3a_isp_cs_config				& isp_cs_config,
										ste3a_damper_bases				& damper_bases,
										ste3a_aec_scene_info			& scene_info);

void steaec_reset(						steaec_obj						* aec);

void steaec_save(						steaec_obj						* aec);

void steaec_restore(					steaec_obj						* aec);

void steaec_swap(						steaec_obj						* aec);

void steaec_set_tuning(					steaec_obj						* aec,
								const	ste3a_aec_tuning				& tuning);

void steaec_get_tuning(					steaec_obj						* aec,
										ste3a_aec_tuning				& tuning);

void steaec_get_intermediates(			steaec_obj						* aec,
										ste3a_aec_intermediates			& intermediates);

void steaec_dynamic_weighting_start(
										steaec_obj						* aec,
								const	ste3a_aec_dynamic_weighting_geometry & geometry);

void steaec_dynamic_weighting_update(
										steaec_obj						* aec,
								const	ste3a_aec_dynamic_weighting_geometry & geometry);

void steaec_dynamic_weighting_stop(		steaec_obj						* aec);

ste3a_bool steaec_playback(				steaec_obj						* aec,
								const	void							* dump,
										void *							& next,
										ste3a_bool						& match);

#endif /* STEAEC_H_ */
