/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * \file    steaeci.h
 * \brief   STE Auto Exposure Control internal functions declarations
 * \author  ST-Ericsson Image Quality Team, Agrate (Italy)
 */

#ifndef STEAECI_H_
#define STEAECI_H_


struct steaeci_channel_averages
{
	ste3a_normalized_pixel_level channel[4];
};

enum steaeci_metering_phase
{
	steaeci_metering_phase_fast_coldstart,
	steaeci_metering_phase_normal,
	steaeci_metering_phase_fast_mode,
	steaeci_metering_phase_pre_flashes
};

ste3a_bool steaeci_copy_stats(		const	steaec_stats_descriptor					& src_descriptor,
									const	ste3a_histstats_val						* src_hist_values,
									const	ste3a_gridstats_val						* src_grid_values,
									const	steaec_stats_descriptor					& dst_descriptor,
											ste3a_histstats_val						* dst_hist_values,
											ste3a_gridstats_val						* dst_grid_values);

void steaeci_initial_exposure(		const	ste3a_aec_tuning						& tuning,
											ste3a_aec_intermediates 				& intermediates,
											steaeci_channel_averages				& unexposed_channel_averages,
											ste3a_normalized_pixel_level			& next_unexposed_black_point,
											ste3a_exposure							& next_requested_exposure_physical,
											ste3a_exposure							& next_requested_exposure_logical,
											ste3a_exposure							& next_damped_exposure_physical,
											ste3a_exposure							& next_damped_exposure_logical,
											ste3a_light_level						& light_level);

void steaeci_compute_exposure(		const	ste3a_aec_tuning						& tuning,
									const	steaec_weighting_mode					  weighting_mode,
									const	ste3a_float								  sweights[STEAEC_STATS_GRID_SIZEX_MAX_COUNT][STEAEC_STATS_GRID_SIZEY_MAX_COUNT],
									const	ste3a_float								  dweights[STEAEC_STATS_GRID_SIZEX_MAX_COUNT][STEAEC_STATS_GRID_SIZEY_MAX_COUNT],
									const	ste3a_float								  fweights[STEAEC_STATS_GRID_SIZEX_MAX_COUNT][STEAEC_STATS_GRID_SIZEY_MAX_COUNT],
									const	ste3a_float								  bweights[STEAEC_STATS_GRID_SIZEX_MAX_COUNT][STEAEC_STATS_GRID_SIZEY_MAX_COUNT],
									const	ste3a_cammod_exposure_status			& cammod_status,
									const	steaec_stats_descriptor					& stats_descriptor,
									const	steaec_stats_sample						& stats_sample,
									const	ste3a_isp_cg_config						& isp_cg_config,
									const	ste3a_gain								  black_point_evolution_factor,
									const	ste3a_normalized_pixel_level			& prev_unexposed_black_point,
											ste3a_aec_intermediates 				& intermediates,
											steaeci_channel_averages				& unexposed_channel_averages,
											ste3a_normalized_pixel_level			& next_unexposed_black_point,
											ste3a_exposure							& next_requested_exposure_logical,
											ste3a_exposure							& next_requested_exposure_physical,
											ste3a_light_level						& light_level);

void steaeci_damp_exposure(			const	ste3a_aec_tuning						& tuning,
									const	ste3a_timestamp							  prev_timestamp,
									const	ste3a_exposure							  prev_requested_exposure_logical,
									const	ste3a_exposure							  prev_requested_exposure_physical,
									const	ste3a_exposure							  prev_damped_exposure_logical,
									const	ste3a_exposure							  prev_damped_exposure_physical,
									const	ste3a_timestamp							  next_timestamp,
									const	ste3a_exposure							  next_requested_exposure_logical,
									const	ste3a_exposure							  next_requested_exposure_physical,
									const	steaeci_metering_phase					  metering_phase,
											ste3a_aec_intermediates 				& intermediates,
											ste3a_exposure							& next_damped_exposure_logical,
											ste3a_exposure							& next_damped_exposure_physical);

void steaeci_compute_flash(			const	ste3a_aec_callbacks						& aec_cbs,
									const	ste3a_aec_tuning						& tuning,
									const	ste3a_exposure							& prev_requested_exposure_logical,
									const	ste3a_exposure							& prev_requested_exposure_physical,
									const	ste3a_energy							& max_energy,
											ste3a_aec_intermediates 				& intermediates,
											ste3a_energy							& next_requested_flash_energy,
											ste3a_energy							& next_actual_flash_energy,
											ste3a_gain								& next_residual_flash_gain,
											ste3a_flash_exposure_config				& next_flash_config,
											ste3a_exposure							& next_requested_exposure_logical,
											ste3a_exposure							& next_requested_exposure_physical);

void steaeci_refine_flash(			const	ste3a_aec_callbacks						& aec_cbs,
									const	ste3a_aec_tuning						& tuning,
									const	steaec_weighting_mode					  weighting_mode,
									const	ste3a_float								  sweights[STEAEC_STATS_GRID_SIZEX_MAX_COUNT][STEAEC_STATS_GRID_SIZEY_MAX_COUNT],
									const	ste3a_float								  dweights[STEAEC_STATS_GRID_SIZEX_MAX_COUNT][STEAEC_STATS_GRID_SIZEY_MAX_COUNT],
									const	ste3a_float								  fweights[STEAEC_STATS_GRID_SIZEX_MAX_COUNT][STEAEC_STATS_GRID_SIZEY_MAX_COUNT],
									const	ste3a_float								  bweights[STEAEC_STATS_GRID_SIZEX_MAX_COUNT][STEAEC_STATS_GRID_SIZEY_MAX_COUNT],
									const	steaec_stats_descriptor					& stats_descriptor,
									const	ste3a_bool								& init_available,
									const	ste3a_cammod_exposure_status			& init_cammod_status,
									const	ste3a_sensitivity						  init_sensitivity,
									const	steaec_stats_sample						& init_stats_sample,
									const	ste3a_exposure							& init_requested_exposure_logical,
									const	ste3a_cammod_exposure_status			& last_cammod_status,
									const	ste3a_sensitivity						  last_sensitivity,
									const	ste3a_flash_exposure_status				& last_flash_status,
									const	steaec_stats_sample						& last_stats_sample,
									const	ste3a_isp_cg_config						& isp_cg_config,
									const	ste3a_energy							& max_energy,
											ste3a_aec_intermediates 				& intermediates,
											steaeci_channel_averages				& unexposed_channel_averages,
											ste3a_normalized_pixel_level			& next_unexposed_black_point,
											ste3a_energy							& next_requested_flash_energy,
											ste3a_energy							& next_actual_flash_energy,
											ste3a_gain								& next_residual_flash_gain,
											ste3a_flash_exposure_config				& next_flash_config,
											ste3a_exposure							& next_requested_exposure_logical,
											ste3a_exposure							& next_requested_exposure_physical,
											ste3a_light_level						& light_level);

void steaeci_absorb_flash(			const	ste3a_aec_tuning						& tuning,
									const	steaec_weighting_mode					  weighting_mode,
									const	ste3a_float								  sweights[STEAEC_STATS_GRID_SIZEX_MAX_COUNT][STEAEC_STATS_GRID_SIZEY_MAX_COUNT],
									const	ste3a_float								  dweights[STEAEC_STATS_GRID_SIZEX_MAX_COUNT][STEAEC_STATS_GRID_SIZEY_MAX_COUNT],
									const	ste3a_float								  fweights[STEAEC_STATS_GRID_SIZEX_MAX_COUNT][STEAEC_STATS_GRID_SIZEY_MAX_COUNT],
									const	ste3a_float								  bweights[STEAEC_STATS_GRID_SIZEX_MAX_COUNT][STEAEC_STATS_GRID_SIZEY_MAX_COUNT],
									const	steaec_stats_descriptor					& stats_descriptor,
									const	ste3a_bool								& init_available,
									const	ste3a_cammod_exposure_status			& init_cammod_status,
									const	ste3a_sensitivity						  init_sensitivity,
									const	steaec_stats_sample						& init_stats_sample,
									const	ste3a_exposure							& init_requested_exposure_logical,
									const	ste3a_cammod_exposure_status			& last_cammod_status,
									const	ste3a_sensitivity						  last_sensitivity,
									const	ste3a_flash_exposure_status				& last_flash_status,
									const	steaec_stats_sample						& last_stats_sample,
									const	ste3a_sensitivity						  curr_sensitivity,
									const	ste3a_flash_exposure_status				& curr_flash_status,
									const	ste3a_isp_cg_config						& isp_cg_config,
											ste3a_aec_intermediates 				& intermediates,
											steaeci_channel_averages				& unexposed_channel_averages,
											ste3a_normalized_pixel_level			& next_unexposed_black_point,
											ste3a_exposure							& next_requested_exposure_logical,
											ste3a_exposure							& next_requested_exposure_physical,
											ste3a_light_level						& light_level);

void steaeci_split_exposure(		const	ste3a_aec_tuning						& tuning,
									const	ste3a_aec_callbacks						& aec_cbs,
									const	ste3a_isp_cg_config						& isp_cg_config,
									const	ste3a_exposure							  next_damped_exposure_logical,
									const	ste3a_exposure							  next_damped_exposure_physical,
									const	ste3a_sensitivity						  baseline_sensitivity,
									const	steaeci_metering_phase					  metering_phase,
											ste3a_aec_intermediates 				& intermediates,
											ste3a_exposure							& next_actual_exposure,
											ste3a_exposure 							& next_residual_exposure,
											ste3a_cammod_exposure_config			& cammod_config,
											ste3a_isp_dg_config						& isp_dg_config,
											ste3a_gain								& fading_gain,
											ste3a_gain								& average_cg,
											ste3a_sensitivity						& sensitivity);

void steaeci_refine_exposure(		const	ste3a_aec_tuning						& tuning,
									const	ste3a_aec_callbacks						& aec_cbs,
									const	ste3a_isp_cg_config						& isp_cg_config,
									const	ste3a_cammod_exposure_status			& cammod_status,
									const	ste3a_exposure							  next_requested_exposure_logical,
									const	ste3a_exposure							  next_requested_exposure_physical,
									const	ste3a_sensitivity						  baseline_sensitivity,
											ste3a_aec_intermediates 				& intermediates,
											ste3a_exposure							& next_actual_exposure,
											ste3a_exposure 							& next_residual_exposure,
											ste3a_isp_dg_config						& isp_dg_config,
											ste3a_gain								& fading_gain,
											ste3a_gain								& average_cg,
											ste3a_sensitivity						& sensitivity);

void steaeci_split_flash(			const	ste3a_aec_callbacks						& aec_cbs,
									const	ste3a_energy							  next_requested_energy,
											ste3a_energy							& next_actual_energy,
											ste3a_gain								& next_residual_gain,
											ste3a_flash_exposure_config				& flash_config);

ste3a_bool steaeci_decide_flash(	const	ste3a_aec_tuning						& tuning,
									const	ste3a_exposure							  exposure);

ste3a_bool steaeci_decide_stable_noflash(
									const	ste3a_float								  stability_factor,
									const	ste3a_exposure							  prev_requested_exposure,
									const	ste3a_exposure							  prev_damped_exposure,
									const	ste3a_exposure							  next_requested_exposure,
									const	ste3a_exposure							  next_damped_exposure);

ste3a_bool steaeci_decide_stable_noflash(
									const	ste3a_float								  stability_factor,
									const	ste3a_exposure							  prev_requested_exposure,
									const	ste3a_exposure							  prev_damped_exposure,
									const	ste3a_exposure							  prev_actual_exposure,
									const	ste3a_exposure							  next_requested_exposure,
									const	ste3a_exposure							  next_damped_exposure,
									const	ste3a_exposure							  next_actual_exposure);

ste3a_bool steaeci_decide_stable_flash(
									const	ste3a_float								  stability_factor,
									const	ste3a_exposure							  prev_requested_exposure,
									const	ste3a_energy							  prev_requested_energy,
									const	ste3a_exposure							  prev_actual_exposure,
									const	ste3a_energy							  prev_actual_energy,
									const	ste3a_exposure							  next_requested_exposure,
									const	ste3a_energy							  next_requested_energy,
									const	ste3a_exposure							  next_actual_exposure,
									const	ste3a_energy							  next_actual_energy);

ste3a_float steaeci_apply_rule(		const	ste3a_float								  input,
									const	ste3a_count								  pcount,
									const	ste3a_float								* pinput,
									const	ste3a_float								* poutput);

ste3a_gain steaeci_apply_gain_rule(	const	ste3a_exposure							  exposure_original,
									const	ste3a_exposure							  exposure_residual,
									const	ste3a_exposure_rule_driver				  driver,
									const	ste3a_count								  pcount,
									const	ste3a_exposure							* pexposure,
									const	ste3a_gain								* pgain);

ste3a_time steaeci_apply_time_rule(	const	ste3a_exposure							  exposure_original,
									const	ste3a_exposure							  exposure_residual,
									const	ste3a_exposure_rule_driver				  driver,
									const	ste3a_count								  pcount,
									const	ste3a_exposure							* pexposure,
									const	ste3a_time								* ptime,
									const	ste3a_time								  min,
									const	ste3a_time								  max,
									const	ste3a_time								  quantum);

void steaeci_decode_weights(		const	ste3a_aec_weights_row					  weights_map[STE3A_AEC_WEIGHTS_COL_SIZE],
									const	ste3a_float								  weights_tbl[STE3A_AEC_WEIGHTS_TBL_SIZE],
											ste3a_count								  grid_size_x,
											ste3a_count								  grid_size_y,
											ste3a_float								  dweights[STEAEC_STATS_GRID_SIZEX_MAX_COUNT][STEAEC_STATS_GRID_SIZEY_MAX_COUNT],
											ste3a_float								  iweights[STEAEC_STATS_GRID_SIZEX_MAX_COUNT][STEAEC_STATS_GRID_SIZEY_MAX_COUNT]);

void steaeci_compute_weights(		const	ste3a_aec_tuning						& tuning,
									const	ste3a_aec_dynamic_weighting_geometry	& geometry,
											ste3a_count								  grid_size_x,
											ste3a_count								  grid_size_y,
									const	ste3a_float								  static_weights[STEAEC_STATS_GRID_SIZEX_MAX_COUNT][STEAEC_STATS_GRID_SIZEY_MAX_COUNT],
											ste3a_float								  dynamic_weights[STEAEC_STATS_GRID_SIZEX_MAX_COUNT][STEAEC_STATS_GRID_SIZEY_MAX_COUNT]);

void steaeci_compute_damper_bases(	const	ste3a_cammod_exposure_config			& cammod_config,
									const	ste3a_gain								  fading_gain,
									const	ste3a_isp_dg_config						& isp_dg_config,
									const	ste3a_gain								  average_cg,
									const	ste3a_isp_er_config						& isp_er_config,
									const	ste3a_isp_cs_config						& isp_cs_config,
									const	steaeci_channel_averages				& unexposed_channel_averages,
											ste3a_damper_bases						& damper_bases);

void steaeci_reset_damper_bases(			ste3a_damper_bases						& damper_bases);

void steaeci_initialize_tuning(				ste3a_aec_tuning						& tuning);

#endif /* STEAECI_H_ */
