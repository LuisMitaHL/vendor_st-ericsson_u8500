/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * \file    steafc.h
 * \brief   STE Auto Facus Control interface
 * \author  ST-Ericsson Image Quality Team, Agrate (Italy)
 */

#ifndef STEAFC_H_
#define STEAFC_H_

#include "ste3a_afc_types.h"
#include "ste3a_afc_debug.h"
#include "ste3a_cmal_types.h"
#include "ste3a_stats_types.h"
#include "ste3a_calibration_types.h"


#define STEAFC_STATS_SAMPLES_FS_MAX_COUNT	1024
#define STEAFC_STATS_SAMPLES_TR_MAX_COUNT	2

struct steafc_focusstats_sample
{
	ste3a_normalized_sharpness			sharpness[STE3A_AFC_STATS_CHANS_MAX_COUNT * STE3A_AFC_STATS_ZONES_MAX_COUNT];
	ste3a_normalized_brightness			brightness[STE3A_AFC_STATS_CHANS_MAX_COUNT * STE3A_AFC_STATS_ZONES_MAX_COUNT];

	ste3a_normalized_lens_displacement	displacement;

	ste3a_normalized_sharpness			sharpness_noise_threshold;
};

struct steafc_obj
{
	ste3a_common_callbacks	com_cbs;
	ste3a_afc_callbacks		afc_cbs;

	ste3a_afc_tuning tuning;

	ste3a_afc_intermediates intermediates;

	ste3a_afc_state state;

	ste3a_normalized_lens_displacement displacement;

	ste3a_focus_stats_descriptor ext_stats_descriptor;
	ste3a_focus_stats_descriptor int_stats_descriptor;

	ste3a_bool pending_evaluate;

	ste3a_count fs_samples_count;
	ste3a_count tr_samples_count;

	steafc_focusstats_sample fs_samples_array[STEAFC_STATS_SAMPLES_FS_MAX_COUNT];
	steafc_focusstats_sample tr_samples_array[STEAFC_STATS_SAMPLES_TR_MAX_COUNT];

	steafc_focusstats_sample paused_ref;
	steafc_focusstats_sample paused_tst;

	ste3a_bool forced_restart;

	ste3a_cammod_focus_config sequence;
};

steafc_obj * steafc_open(const ste3a_common_callbacks & com_cbs, const ste3a_afc_callbacks & afc_cbs, const ste3a_calibration_data_lens_actuator * calibration_data_ptr);

void steafc_close(steafc_obj * const afc);

void steafc_stats_geometry_focus(steafc_obj * const afc, ste3a_zoom_factor dzoom_factor, ste3a_focus_stats_geometry & geometry);

void steafc_start(steafc_obj * const afc, const ste3a_afc_mode mode, const ste3a_focus_stats_descriptor & stats_descriptor);

void steafc_change_mode(steafc_obj * const afc, const ste3a_afc_mode mode);

void steafc_stop(steafc_obj * const afc);

void steafc_follow(steafc_obj * const afc, const ste3a_logical_lens_displacement displacement);

void steafc_lose(steafc_obj * const afc);

void steafc_update(steafc_obj * const afc, ste3a_timestamp current_time, const ste3a_cammod_focus_status & cammod_status, const ste3a_focus_stats_status & stats_status, ste3a_light_level light_level, ste3a_bool flash_fired);

void steafc_evaluate(steafc_obj * const afc, ste3a_timestamp current_time, ste3a_cammod_focus_config & cammod_config, ste3a_afc_state & state, ste3a_afc_scene_info & scene_info);

void steafc_set_tuning(steafc_obj * afc, const ste3a_afc_tuning & tuning);

void steafc_get_tuning(steafc_obj * afc, ste3a_afc_tuning & tuning);

void steafc_get_intermediates(steafc_obj * afc, ste3a_afc_intermediates & intermediates);

ste3a_bool steafc_locked_zones(steafc_obj * const afc, ste3a_count & zones_count, ste3a_index zones_array[STE3A_AFC_STATS_ZONES_MAX_COUNT]);

ste3a_bool steafc_playback(steafc_obj * afc, const void * dump, void * & next, ste3a_bool & match);

#endif /* STEAFC_H_ */
