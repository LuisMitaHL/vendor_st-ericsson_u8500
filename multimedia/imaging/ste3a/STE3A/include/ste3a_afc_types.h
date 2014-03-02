/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * \file    ste3a_afc_types.h
 * \brief   STE3A AFC types
 * \author  ST-Ericsson Image Quality Team, Agrate (Italy)
 */

#ifndef STE3A_AFC_TYPES_H_
#define STE3A_AFC_TYPES_H_

#include "ste3a_types.h"
#include "ste3a_cmal_types.h"
#include "ste3a_stats_types.h"


#define STE3A_AFC_DISPDIST_MAX_POINTS		4

enum ste3a_afc_mode											// af search mode
{
	ste3a_afc_mode_single = STE3A_ENUM_BASE,				//		single shot (for stills)
	ste3a_afc_mode_continuous_free_running,					//		continuous focus (free running, for viewo/viewfinder before half-pressed event)
	ste3a_afc_mode_continuous_auto_locking					//		continuous focus (auto locking, for viewo/viewfinder after half-pressed event)
};

enum ste3a_afc_state										// afc state machine states
{
	xx_manual_k = STE3A_ENUM_BASE,							//		65536 - AF not running, lens moved by an external actor, position known
	xx_manual_u,											//		65537 - AF not running, lens moved by an external actor, position unknown
	xx_locked_k,											//		65538 - AF not running, lens moved by the AF algorithm, position known (best focus)
	xx_failed_k,											//		65539 - AF not running, lens moved by the AF algorithm, position known (recovery)
	xx_aborted_k,											//		65540 - AF not running, lens moved by the AF algorithm, position known (recovery)
	xx_aborted_u,											//		65541 - AF not running, lens moved by an external actor, position unknown
	ss_starting_k,											//		65542 - internal state, never returned
	ss_starting_u,											//		65543 - internal state, never returned
	cf_starting_k,											//		65544 - internal state, never returned
	ca_starting_k,											//		65545 - internal state, never returned
	cf_starting_u,											//		65546 - internal state, never returned
	ca_starting_u,											//		65547 - internal state, never returned
	ss_searching_k,											//		65548 - AF running in single search (auto-locking) mode, lens moved by the AF algorithm, position known 
	cf_searching_k,											//		65549 - AF running in continuous (free running) mode, lens moved by the AF algorithm, position known
	ca_searching_k,											//		65550 - AF running in continuous (auto-locking) mode, lens moved by the AF algorithm, position known
	cf_tracking_k,											//		65551 - AF running in continuous (free running) mode, lens moved by the AF algorithm, position known
	ca_tracking_k,											//		65552 - AF running in continuous (auto-locking) mode, lens moved by the AF algorithm, position known
	cf_pausing_good_focus_k,								//		65553 - AF paused in continuous (free running) mode, lens moved by the AF algorithm, position known, good focus
	cf_pausing_bad_focus_k,									//		65554 - AF paused in continuous (free running) mode, lens moved by the AF algorithm, position known, bad focus
	ca_pausing_good_focus_k,								//		65555 - internal state, never returned
	ca_pausing_bad_focus_k,									//		65556 - internal state, never returned
	ca_holding_good_focus_k,								//		65557 - AF holding in continuous (auto-locking) mode, lens moved by the AF algorithm, position known, good focus
	ca_holding_bad_focus_k									//		65558 - AF holding in continuous (auto-locking) mode, lens moved by the AF algorithm, position known, bad focus
};

enum ste3a_depth_priority									// depth priority modes (when zones used)
{
	ste3a_depth_priority_none = STE3A_ENUM_BASE,			//		no depth priority
	ste3a_depth_priority_farthest,							//		priority to farthest
	ste3a_depth_priority_nearest							//		priority to nearest
};

enum ste3a_afc_interpolation								// interpolation method
{
	ste3a_afc_interpolation_none = STE3A_ENUM_BASE,			//		no interpolation (sampling)
	ste3a_afc_interpolation_cubic							//		cubic interpolation
};

#define STE3A_AFC_RULE_MAX_POINTS		4					// maximum number of nodes in a afc rule definition (piecewise linear function)

struct ste3a_afc_tuning										// afc tuning parameters
{
	ste3a_count							search_pcount;												//	0x0000 number of points in the search rule
	ste3a_normalized_lens_displacement	search_pdisplacement[STE3A_CMAL_FOCUS_SEQUENCE_MAX_STEPS];	//	0x0004 search rule, array of displacements to be sequenced during the search phase: MUST be monotonous (either ascending or descending)
	ste3a_normalized_lens_displacement	tracking_step_size;											//	0x0104 step size for the tracking phase (absolute value)
	ste3a_float							restart_brgt_avgrt_threshold;								//	0X0108 brightness averages ratio threshld for restarting tracking
	ste3a_float							restart_shrp_avgrt_threshold;								//	0X010C sharpnesses averages ratio threshld for restarting tracking
	ste3a_float							restart_brgt_ergrt_threshold;								//	0X0110 brightness energies ratio threshld for restarting tracking
	ste3a_float							restart_shrp_ergrt_threshold;								//	0X0114 sharpnesses energies ratio threshld for restarting tracking
	ste3a_normalized_correlation		restart_brgt_xcorr_threshold;								//	0X0118 brightness normalized xcorrelation coeff. threshld for restarting tracking
	ste3a_normalized_correlation		restart_shrp_xcorr_threshold;								//	0X011C sharpnesses normalized xcorrelation coeff. threshld for restarting tracking
	ste3a_normalized_lens_displacement	recovery_position;											//	0x0120 position selected in case the AF algorithm fails
	ste3a_count							sharpness_noise_pcount;										//	0x0124 sharpness noise rule: points count
	ste3a_exposure						sharpness_noise_plightlevel[STE3A_AFC_RULE_MAX_POINTS];		//	0x0128 sharpness noise rule: light levels array
	ste3a_normalized_sharpness			sharpness_noise_pthreshold[STE3A_AFC_RULE_MAX_POINTS];		//	0x0138 sharpness noise rule: thresholds array
	ste3a_count							zones_count;												//	0x0148 number of zones
	ste3a_norm_length					zones_left  [STE3A_AFC_STATS_ZONES_MAX_COUNT];				//	0x014C starting x positions
	ste3a_norm_length					zones_top   [STE3A_AFC_STATS_ZONES_MAX_COUNT];				//	0x0174 starting y positions
	ste3a_norm_length					zones_width [STE3A_AFC_STATS_ZONES_MAX_COUNT];				//	0x019C x sizes
	ste3a_norm_length					zones_height[STE3A_AFC_STATS_ZONES_MAX_COUNT];				//	0x01C4 y sizes
	ste3a_bool							local_evaluation;											//	0x01EC zones evaluated singularly (otherwise averaged)
	ste3a_depth_priority				local_depth_priority;										//	0x01F0 depth priority mode, considered only when zones evaluated singularly
	ste3a_count							dispdist_pcount;											//	0x01F4 subject distance computation curve
	ste3a_normalized_lens_displacement	dispdist_pdisplacement[STE3A_AFC_DISPDIST_MAX_POINTS];		//	0x01F8 "
	ste3a_distance						dispdist_pdistance[STE3A_AFC_DISPDIST_MAX_POINTS];			//	0x0208 "
	ste3a_bool							force_restart_on_caf_mode_change;							//	0x0218 type of transition between free-running and auto-locking CAF
	ste3a_afc_interpolation				interpolation;												//	0x021C interpolation method for the sharpness function
	ste3a_bool							early_stop_enable;											//	0x0220 early stop functionality enable flag
	ste3a_float							es_sharpness_ratio_threshold;								//	0x0224 current/abs_peak sharpness ratio for early stop trigger
	ste3a_normalized_lens_displacement	es_displacement_difference_threshold;						//	0x0228 current/rel_peak displacement difference for early stop trigger
	ste3a_float							continue_brgt_avgrt_threshold;								//	0X022C brightness averages ratio threshld for continuing tracking
	ste3a_float							continue_shrp_avgrt_threshold;								//	0X0230 sharpnesses averages ratio threshld for continuing tracking
	ste3a_float							continue_brgt_ergrt_threshold;								//	0X0234 brightness energies ratio threshld for continuing tracking
	ste3a_float							continue_shrp_ergrt_threshold;								//	0X0238 sharpnesses energies ratio threshld for continuing tracking
	ste3a_normalized_correlation		continue_brgt_xcorr_threshold;								//	0X023C brightness normalized xcorrelation coeff. threshld for continuing tracking
	ste3a_normalized_correlation		continue_shrp_xcorr_threshold;								//	0X0240 sharpnesses normalized xcorrelation coeff. threshld for continuing tracking
																									//	0x0244 #### next free offset ####
};

struct ste3a_afc_scene_info									// afc scene information
{
	ste3a_distance		subject_distance;					//		focused subject distance
};

struct ste3a_afc_callbacks									// afc callbacks pointers wrapper
{
	ste3acb_cmal_set_LA_model	* ptr_cmal_set_LA_model;	//		lens actuator model setup, cannot be NULL
	ste3acb_cmal_convert_LA_ln	* ptr_cmal_convert_LA_ln;	//		logical (HW) to normalized (SW) displacement conversion, cannot be NULL
	ste3acb_cmal_convert_LA_nl	* ptr_cmal_convert_LA_nl;	//		normalized (SW) to logical (HW) displacement conversion, cannot be NULL
	ste3acb_cmal_range_LA_ninf	* ptr_cmal_range_LA_ninf;	//		normalized mechanical stop (inf) position, cannot be NULL
	ste3acb_cmal_range_LA_nmac	* ptr_cmal_range_LA_nmac;	//		normalized mechanical stop (mac) position, cannot be NULL

	void						* cmal_context;				//		pointer to callback's implementation context
};

#endif /* STE3A_AFC_TYPES_H_ */
