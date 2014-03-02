/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * \file    ste3a_awb_types.h
 * \brief   STE3A AWB types
 * \author  ST-Ericsson Image Quality Team, Agrate (Italy)
 */

#ifndef STE3A_AWB_TYPES_H_
#define STE3A_AWB_TYPES_H_

#include "ste3a_types.h"
#include "ste3a_calibration_types.h"

// algorithm interface constants and types (must be exposed in a public header file)
#define STE3A_AWB_N_OF_RUNTIME_CONTEXT	2									// number of runtime tuning contexts (low-light, high-light)
#define	STE3A_AWB_N_OF_DISCRETE_ILL		6									// number of discrete illuminants
#define STE3A_AWB_N_OF_ILL_CLASSES		(STE3A_AWB_N_OF_DISCRETE_ILL + 1)	// number of illuminant classes
// number of blackbody boundary illuminants (two)
#define	STE3A_AWB_N_OF_LOCUS_ILL		2
// total number of illuminants (blackbody + discrete)
#define STE3A_AWB_N_OF_ILLUMINANTS		(STE3A_AWB_N_OF_LOCUS_ILL + STE3A_AWB_N_OF_DISCRETE_ILL)
#define STE3A_AWB_N_OF_SURFACES			8									// number of surfaces (including sky and default)
#define STE3A_AWB_N_OF_SENS				(STE3A_CALIBRATION_N_OF_ILL_MAX * 2)	// max number of nvm sensitivities

typedef float ste3a_chromaticity;							// chromaticity value					[pure number ]

enum ste3a_computation_mode									// traces verbosity levels
{
	ste3a_computation_mode_null = STE3A_ENUM_BASE,			//		no computation
	ste3a_computation_mode_phase1,							//		compute only phase1
	ste3a_computation_mode_phase2,							//		compute only phase2 (maps surface-illuminant classifications)
	ste3a_computation_mode_phase1_phase2					//		compute both phase1 and phase2
};

enum ste3a_kelvin_constraint_method							// traces verbosity levels
{
	ste3a_kelvin_constraint_method_null = STE3A_ENUM_BASE,	//		no constraint
	ste3a_kelvin_constraint_method_gcmg,					//		GainAWB -> ColorMatrix -> GainConstrained
	ste3a_kelvin_constraint_method_ggcm						//		GainAWB -> GainConstrained -> ColorMatrix
};

// algorithm tuning (this data structure will be initialized from hand-made tuning files)
struct ste3a_awb_tuning
{
	// runtime contexts tuning (low light and high light)
	ste3a_light_level				scenes_light_level_thr_pair[STE3A_AWB_N_OF_RUNTIME_CONTEXT];	// 0x0000 scene illuminance levels (for low-light and high-light contexts)
	ste3a_index						runtime_subject_idx_pair[STE3A_AWB_N_OF_RUNTIME_CONTEXT];		// 0x0008 scene subjects selection (for low-light and high-light contexts)
	ste3a_index						runtime_light_idx_pair[STE3A_AWB_N_OF_RUNTIME_CONTEXT];			// 0x0010 scene lights selection (for low-light and high-light contexts)
	// convergence control
	ste3a_time						decay_time;														// 0x0018 temporal low-pass filter
	ste3a_float						damping_max_ratio;												// 0x001C temporal low-pass filter max speed
	ste3a_float						stability_thr;													// 0x0020 abs(GainSmoothed / GainTaget - 1) threshold
	// speed vs. accuracy
	ste3a_float						subsampling_factor;												// 0x0024 use a subset of statistic points
	ste3a_computation_mode			computation_mode;												// 0x0028 computation mode (NULL, Phase1, Phase2, Phase1 + phase2)
	// camera module calibration
	ste3a_gain						sensor_tilts[3];												// 0x002C sensor calibration data
	// reliability control
	ste3a_percentage				min_covered_area_percent;										// 0x0038 minimum covered area needed for reliability
	ste3a_time						mired_decay_time;												// 0x003C temporal low-pass filter for mired extimation
	// temporal subsampling
	ste3a_time						computation_period;												// 0x0040
	// method to apply the constraint due to kelvin range
	ste3a_kelvin_constraint_method	kelvin_constraint_method;										// 0x0044
	// convergence control fcs (Fast Cold Start)
	ste3a_time						fcs_decay_time;													// 0x0048 temporal low-pass filter
	ste3a_float						fcs_damping_max_ratio;											// 0x004C temporal low-pass filter max speed
	ste3a_float						fcs_stability_thr;												// 0x0050 abs(GainSmoothed / GainTaget - 1) threshold
	ste3a_time						fcs_mired_decay_time;											// 0x0054 temporal low-pass filter for mired extimation
	ste3a_time						fcs_computation_period;											// 0x0058
	ste3a_float						fcs_subsampling_factor;											// 0x005C use a subset of statistic points
	ste3a_bool						fast_coldstart_enable;											// 0x0060
	ste3a_time						fcs_forced_stability_delay_time;								// 0x0064
	ste3a_time						forced_stability_delay_time;									// 0x0068
	// convergence control flash use case 
	ste3a_time						flash_decay_time;												// 0x006C temporal low-pass filter flash use case
	ste3a_time						flash_mired_decay_time;											// 0x0070 temporal low-pass filter for mired extimation flash use case
																									// 0x0074 #### next free offset ####
};

struct ste3a_rb_chromaticity
{
	ste3a_chromaticity				r;
	ste3a_chromaticity				b;
};

struct ste3a_awb_scene_info
{
	ste3a_kelvin					color_temperature;
	ste3a_float						illuminant_class_scores[STE3A_AWB_N_OF_ILL_CLASSES];
	ste3a_rb_chromaticity			calibrated_central_chromaticity;						
};


#endif /* STE3A_AWB_TYPES_H_ */
