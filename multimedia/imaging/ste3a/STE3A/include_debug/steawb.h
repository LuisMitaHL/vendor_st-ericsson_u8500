/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * \file    steawb.h
 * \brief   STE Auto White Balance interface
 * \author  ST-Ericsson Image Quality Team, Agrate (Italy)
 */

// use tab-stop = 4

#ifndef STEAWB_H_
#define STEAWB_H_

#include "ste3a_awb_types.h"
#include "ste3a_awb_debug.h"
#include "ste3a_stats_types.h"
#include "steawbi.h"



// dimensions of input statistics components
struct steawb_stats_descriptor
{
	ste3a_grid_stats_descriptor	vv_grid; // width, height
	ste3a_grid_stats_descriptor	si_grid; // width, height
};

struct steawb_stats_status
{
	ste3a_grid_stats_status       grid;
};

struct ste3a_awb_gains										// 4-channels WB gains
{
	ste3a_gain	rr_gain;									//		red
	ste3a_gain	gr_gain;									//		green red
	ste3a_gain	gb_gain;									//		green blue
	ste3a_gain	bb_gain;									//		blue
};

// algorithm scene information output
struct ste3a_awb_results
{
	ste3a_awb_gains			gains;
	ste3a_color_matrix		color_matrix;
	ste3a_awb_scene_info	scene_info;
};

struct steawb_obj
{
	ste3a_common_callbacks	com_cbs;			// pointers function to malloc and free
	steawb_stats_descriptor	stats_descriptor;	// input statistics for awb algorithm
	ste3a_awb_tuning		tuning;				// tuning parameters
	ste3a_awb_camera_data	camera_data;		// camera module characterization data
	ste3a_awb_intermediates	intermediates;		// data for computing and debugging
	ste3a_awb_intermediates	saved_intermediates;// saved data for computing and debugging
	ste3a_awb_intermediates	reset_intermediates;// for reset method
	ste3a_awb_intermediates	tmp_intermediates;	// for swap method
	steawb_internal			internal;			// internal data
	ste3a_awb_results		output;				// output of the algorithm
	ste3a_awb_results		saved_output;		// saved output of the algorithm
	ste3a_awb_results		reset_output;		// for reset method
	ste3a_awb_results		tmp_output;			// for swap method
};


steawb_obj*	steawb_open(const ste3a_common_callbacks& com_cbs,
	const	steawb_stats_descriptor&	stats_setup,
	const	void						* tuning,
			ste3a_count					tuning_size,
	const	ste3a_calibration_data		* calibration_data);

void		steawb_close(steawb_obj* awb);

void		steawb_update(steawb_obj* awb,
	const ste3a_count current_time,
	const ste3a_acquisition_mode acq_mode,
	const ste3a_scene_status & scene_status,
	const ste3a_grid_stats_status & grid_status);

void		steawb_evaluate(steawb_obj* awb,
	const ste3a_count current_time,
	ste3a_awb_results & awb_results,
	ste3a_bool & stable);

void		steawb_reset(steawb_obj * awb);

void		steawb_save(steawb_obj * awb);

void		steawb_restore(steawb_obj * awb);

void		steawb_swap(steawb_obj * awb);

void		steawb_set_tuning(steawb_obj* awb,
	const ste3a_awb_tuning & tuning);

void		steawb_get_tuning(steawb_obj* awb,
	ste3a_awb_tuning & tuning);

void		steawb_get_intermediates(steawb_obj* awb,
	ste3a_awb_intermediates & intermediates);

ste3a_bool	steawb_playback(steawb_obj* awb,
	const void * dump,
	void * & next,
	ste3a_bool & match);

#endif // STEAWB_H_
