/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * \file    ste3a_aec_types.h
 * \brief   STE3A AEC types
 * \author  ST-Ericsson Image Quality Team, Agrate (Italy)
 */

#ifndef STE3A_AEC_TYPES_H_
#define STE3A_AEC_TYPES_H_

#include "ste3a_types.h"
#include "ste3a_cmal_types.h"
#include "ste3a_fgal_types.h"
#include "ste3a_ppal_types.h"


typedef	uint32_t	ste3a_aec_weights_row;					// row of weight indexes

#define STE3A_AEC_RULE_MAX_POINTS		4					// maximum number of nodes in a aec rule definition (piecewise linear function)
#define STE3A_AEC_BITS_PER_WEIGHT		2
#define STE3A_AEC_WEIGHTS_ROW_SIZE		(sizeof(ste3a_aec_weights_row) * 8 / STE3A_AEC_BITS_PER_WEIGHT)
#define STE3A_AEC_WEIGHTS_COL_SIZE		STE3A_AEC_WEIGHTS_ROW_SIZE
#define STE3A_AEC_WEIGHTS_TBL_SIZE		(1 << STE3A_AEC_BITS_PER_WEIGHT)
#define STE3A_AEC_DW_ZONES_MAX_ZONES	10

struct ste3a_aec_tuning										// aec tuning parameters
{
	ste3a_exposure_metering_mode	metering_mode;								// 0x0000 metering mode
	ste3a_normalized_pixel_level	cl_target;									// 0x0004 target brightness level
	ste3a_exposure_comp				compensation;								// 0x0008 exposure compensation
	ste3a_exposure					default_coldstart;							// 0x000C default stating exposure
	ste3a_float						damper_max_ratio;							// 0x0010 aec damper, max ratio between consecutive exposure levels
	ste3a_time						damper_decay_time		;					// 0x0014 aec damper, decay time for reducing the error to 10%
	ste3a_float						damper_convergence_factor;					// 0x0018 aec damper, convergence factor
	ste3a_float						stability_factor;							// 0x001C reported stability tolearnce
	ste3a_exposure_priority_mode	priority_mode;								// 0x0020 aec priority mode (time, aperture, ...)
	ste3a_exposure_rule_driver		ir_driver;									// 0x0024 iris rule driver
	ste3a_count						ir_pcount;									// 0x0028 iris rule nodes
	ste3a_exposure					ir_pexposure[STE3A_AEC_RULE_MAX_POINTS];	// 0x002C "
	ste3a_gain						ir_pgain[STE3A_AEC_RULE_MAX_POINTS];		// 0x003C "
	ste3a_exposure_rule_driver		nr_driver;									// 0x004C neutral density filter rule driver
	ste3a_count						nr_pcount;									// 0x0050 neutral density filter rule nodes
	ste3a_exposure					nr_pexposure[STE3A_AEC_RULE_MAX_POINTS];	// 0x0054 "
	ste3a_gain						nr_pgain[STE3A_AEC_RULE_MAX_POINTS];		// 0x0064 "
	ste3a_exposure_rule_driver		tr_driver;									// 0x0074 exposure time rule driver
	ste3a_count						tr_pcount;									// 0x0078 exposure time rule nodes
	ste3a_exposure					tr_pexposure[STE3A_AEC_RULE_MAX_POINTS];	// 0x007C "
	ste3a_time						tr_ptime[STE3A_AEC_RULE_MAX_POINTS];		// 0x008C "
	ste3a_time						tr_min;										// 0x009C minimum exposure time (saturation threshold)
	ste3a_time						tr_max;										// 0x00A0 maximum exposure time (saturation threshold)
	ste3a_time						tr_quantum;									// 0x00A4 exposure time quantization step (e.g. for antiflickering, 0 means no quantization)
	ste3a_exposure_rule_driver		ar_driver;									// 0x00A8 analog gain rule driver
	ste3a_count						ar_pcount;									// 0x00AC analog gain rule nodes
	ste3a_exposure					ar_pexposure[STE3A_AEC_RULE_MAX_POINTS];	// 0x00B0 "
	ste3a_gain						ar_pgain[STE3A_AEC_RULE_MAX_POINTS];		// 0x00C0 "
	ste3a_exposure_rule_driver		dr_driver;									// 0x00D0 digital gain rule driver
	ste3a_count						dr_pcount;									// 0x00D4 digital gain rule nodes
	ste3a_exposure					dr_pexposure[STE3A_AEC_RULE_MAX_POINTS];	// 0x00D8 "
	ste3a_gain						dr_pgain[STE3A_AEC_RULE_MAX_POINTS];		// 0x00E8 "
	ste3a_exposure					flash_exposure_threshold;					// 0x00F8 exposure threshold for flash trigger
	ste3a_count						pff_en_pcount;								// 0x00FC preflash first energy attenuation rule nodes
	ste3a_exposure					pff_en_pexposure[STE3A_AEC_RULE_MAX_POINTS];// 0x0100 "
	ste3a_gain						pff_en_pgain[STE3A_AEC_RULE_MAX_POINTS];	// 0x0110 "
	ste3a_count						pff_ex_pcount;								// 0x0120 preflash first exposure attenuation rule nodes
	ste3a_exposure					pff_ex_pexposure[STE3A_AEC_RULE_MAX_POINTS];// 0x0124 "
	ste3a_gain						pff_ex_pgain[STE3A_AEC_RULE_MAX_POINTS];	// 0x0134 "
	ste3a_gain						light_meter_r_gain;							// 0x0144 red channel gain to be used for light level estimation
	ste3a_gain						light_meter_g_gain;							// 0x0148 green channel gain to be used for light level estimation
	ste3a_gain						light_meter_b_gain;							// 0x014C blue channel gain to be used for light level estimation
	ste3a_sensitivity				light_meter_sensitivity;					// 0x0150 exposure to luminance conversion factor (previous gains applied)
	ste3a_aec_weights_row			weights_map[STE3A_AEC_WEIGHTS_COL_SIZE];	// 0x0154 coded weights map (one row for each entry)
	ste3a_float						weights_tbl[STE3A_AEC_WEIGHTS_TBL_SIZE];	// 0x0194 weight index to weight value table
	ste3a_bool						anti_clipping_enable;						// 0x01A4 anti clipping enable flag
	ste3a_float						acl_allowed_clip_ratio;						// 0x01A8 allowed clipping ratio
	ste3a_float						acl_maximum_attenuation;					// 0x01AC maximum exposure attenuation (physical / logical)
	ste3a_bool						black_stretch_enable;						// 0x01B0 black stretch enable flag
	ste3a_float						bst_percentile;								// 0x01B4 percentile for black point identification
	ste3a_normalized_pixel_level	bst_full_strecth_max;						// 0x01B8 maximum black level that can be fully stretched
	ste3a_normalized_pixel_level	bst_partial_stretch_max;					// 0x01BC maximum black level that can be partially stretched
	ste3a_gain						bst_evolution_factor;						// 0x01C0 evolution factor for temporal smoothing
	ste3a_count						reserved;									// 0x01C4 
	ste3a_bool						adaptive_compensation_enable;				// 0x01C8 auto exposure compensation enable
	ste3a_count						aco_pcount;									// 0x01CC auto exposure compensation rule nodes
	ste3a_light_level				aco_plightlevel[STE3A_AEC_RULE_MAX_POINTS];	// 0x01D0 "
	ste3a_gain						aco_pgain[STE3A_AEC_RULE_MAX_POINTS];		// 0x01E0"
	ste3a_gain						er_maximum_gain;							// 0x01F0 maximum exposure recovery gain
	ste3a_float						dynamic_weighting_weight;					// 0x01F4 weight for dynamic vs static weighting weighting
	ste3a_sensitivity				vv_sensitivity;								// 0x01F8 sensor baseline ISO sensitivity (unitary gains) when in video/viewfinder mode
	ste3a_exposure_rule_driver		fr_driver;									// 0x01FC fading gain rule driver
	ste3a_count						fr_pcount;									// 0x0200 fading gain rule nodes
	ste3a_exposure					fr_pexposure[STE3A_AEC_RULE_MAX_POINTS];	// 0x0204 "
	ste3a_gain						fr_pgain[STE3A_AEC_RULE_MAX_POINTS];		// 0x0214 "
	ste3a_power						flash_max_power_p;							// 0x0224 maximum pre-flash power
	ste3a_power						flash_max_power_m;							// 0x0228 maximum main-flash power
	ste3a_time						flash_max_duration_p;						// 0x022C maximum pre-flash duration
	ste3a_time						flash_max_duration_m;						// 0x0230 maximum main-flash duration
	ste3a_bool						adaptive_flash_power;						// 0x0234 adaptive flash power enable
	ste3a_sensitivity				si_sensitivity;								// 0x0238 sensor baseline ISO sensitivity (unitary gains) when in still capture mode
	ste3a_count						dcs_pcount;									// 0x023C dynamic coldstart rule nodes
	ste3a_light_level				dcs_plightlevel[STE3A_AEC_RULE_MAX_POINTS];	// 0x0240 ""
	ste3a_inv_exposure				dcs_pinvexposure[STE3A_AEC_RULE_MAX_POINTS];// 0x0250 ""
	ste3a_bool						fast_coldstart_enable;						// 0x0260 fast coldstart enable
	ste3a_gain						fcs_max_igain;								// 0x0264 fast coldstart rule
	ste3a_gain						fcs_max_ngain;								// 0x0268 "
	ste3a_time						fcs_min_etime;								// 0x026C "
	ste3a_time						fcs_mid_etime;								// 0x0270 "
	ste3a_time						fcs_max_etime;								// 0x0274 "
	ste3a_gain						fcs_max_again;								// 0x0278 "
	ste3a_gain						fcs_max_dgain;								// 0x027C "
	ste3a_float						fcs_damper_max_ratio;						// 0x0280 fast coldstart aec damper, max ratio between consecutive exposure levels
	ste3a_time						fcs_damper_decay_time		;				// 0x0284 fast coldstart aec damper, decay time for reducing the error to 10%
	ste3a_float						fcs_damper_convergence_factor;				// 0x0288 fast coldstart aec damper, convergence factor
	ste3a_float						fcs_stability_factor;						// 0x028C fast coldstart reported stability tolearnce
	ste3a_gain						pf_max_igain;								// 0x0290 pre-flashes rule
	ste3a_gain						pf_max_ngain;								// 0x0294 "
	ste3a_time						pf_min_etime;								// 0x0298 "
	ste3a_time						pf_mid_etime;								// 0x029C "
	ste3a_time						pf_max_etime;								// 0x02A0 "
	ste3a_gain						pf_max_again;								// 0x02A4 "
	ste3a_gain						pf_max_dgain;								// 0x02A8 "
	ste3a_float						pf_stability_factor;						// 0x02AC pre-flashes reported stability tolearnce
	ste3a_bool						fast_mode_enable;							// 0x02B0 fast mode enable
	ste3a_gain						fm_max_igain;								// 0x02B4 fast mode rule
	ste3a_gain						fm_max_ngain;								// 0x02B8 "
	ste3a_time						fm_min_etime;								// 0x02BC "
	ste3a_time						fm_mid_etime;								// 0x02C0 "
	ste3a_time						fm_max_etime;								// 0x02C4 "
	ste3a_gain						fm_max_again;								// 0x02C8 "
	ste3a_gain						fm_max_dgain;								// 0x02CC "
	ste3a_bool						backlight_detection_enable;					// 0x02D0 backlight detection enable
	ste3a_aec_weights_row			bd_weights_map[STE3A_AEC_WEIGHTS_COL_SIZE];	// 0x02D4 backlight detection coded weights map (one row for each entry)
	ste3a_float						bd_weights_tbl[STE3A_AEC_WEIGHTS_TBL_SIZE];	// 0x0314 backlight detection weight index to weight value table
	ste3a_float						bd_mapped_unmapped_ratio_zero_compensation;	// 0x0324 backlight detection 
	ste3a_float						bd_mapped_unmapped_ratio_full_compensation;	// 0x0328 backlight detection 
																				// 0x032C #### next free offset ####
};

struct ste3a_aec_dynamic_weighting_zone						//	dynamic weighting zone configuration
{
	ste3a_norm_length	left;								//		starting x position
	ste3a_norm_length	top;								//		starting y position
	ste3a_norm_length	width;								//		x size
	ste3a_norm_length	height;								//		y size
	ste3a_float			weight;								//		relevance, weight (relative to other zones)
};

struct ste3a_aec_dynamic_weighting_geometry					//	dynamic weighting configuration
{
	ste3a_count							zones_count;
	ste3a_aec_dynamic_weighting_zone	zones_array[STE3A_AEC_DW_ZONES_MAX_ZONES];
};

struct ste3a_aec_scene_info									// aec scene information
{
	ste3a_light_level	ambient_light_level;				//		detected ambient light level (Lux)
	ste3a_light_level	overall_light_level;				//		detected overall light level (Lux)
	ste3a_sensitivity	sensitivity;						//		selected sensitivity
};

struct ste3a_aec_callbacks									// aec callbacks pointers wrapper
{
	ste3acb_cmal_closest_IT	* ptr_cmal_closest_IT;			//		integration time quantizer, can be NULL
	ste3acb_cmal_closest_ET	* ptr_cmal_closest_ET;			//		exposure time quantizer, can be NULL
	ste3acb_cmal_closest_AG	* ptr_cmal_closest_AG;			//		analog gain quantizer, can be NULL
	ste3acb_cmal_closest_IR	* ptr_cmal_closest_IR;			//		iris quantizer, can be NULL
	ste3acb_cmal_closest_NF	* ptr_cmal_closest_NF;			//		neutral density filter quantizer, can be NULL
	ste3acb_fgal_closest_TP * ptr_fgal_closest_TP;			//		flash duration and power quantizer, can be NULL

	void					* cmal_context;					//		pointer to callback's implementation context
	void					* fgal_context;					//		pointer to callback's implementation context
	void					* ppal_context;					//		pointer to callback's implementation context
};

enum ste3a_aec_flash_mode									// flash mode
{
	ste3a_aec_flash_mode_pre = STE3A_ENUM_BASE,				//		pre-flash
	ste3a_aec_flash_mode_main								//		main-flash
};

#endif /* STE3A_AEC_TYPES_H_ */
