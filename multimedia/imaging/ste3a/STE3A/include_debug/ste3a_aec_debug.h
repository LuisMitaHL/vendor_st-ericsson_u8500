/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * \file    ste3a_aec_debug.h
 * \brief   STE3A AEC debug types
 * \author  ST-Ericsson Image Quality Team, Agrate (Italy)
 */

#ifndef STE3A_AEC_DEBUG_H_
#define STE3A_AEC_DEBUG_H_

#include "ste3a_types.h"


struct ste3a_aec_processed_tuning
{
	ste3a_exposure					coldstart;
	ste3a_time						tr_min;
	ste3a_time						tr_max;	
	ste3a_time						ar_min;
	ste3a_time						ar_max;	
	ste3a_time						dr_min;
	ste3a_time						dr_max;	
};

struct ste3a_aec_intermediates								// aec intermediate values
{
	ste3a_aec_processed_tuning		processed_tuning;			//	preprocessed tuning values
	ste3a_gain						compensation_gain;			//	exposure compensation gain
	ste3a_normalized_pixel_level	cl_target;					//	exposure compensated brightness target
	ste3a_float						clipped_ratio;				//	ratio of clipped pixels
	ste3a_gain						correction_linear;			//	exposure correction (linear)
	ste3a_gain						correction_weighted;		//	exposure correction (weighted)
	ste3a_gain						correction_stretched;		//	exposure correction (stretched)
	ste3a_gain						correction_logical;			//	exposure correction (logical)
	ste3a_gain						correction_physical;		//	exposure correction (physical)
	ste3a_exposure					exposure_requested_logical;	//	requested exposure (logical)
	ste3a_exposure					exposure_requested_physical;//	requested exposure (physical)
	ste3a_exposure					exposure_damped_logical;	//	damped requested exposure (logical)
	ste3a_exposure					exposure_damped_physical;	//	damped requested exposure (physical)
	ste3a_exposure					exposure_actual;			//	quantized damped requested exposure
	ste3a_exposure					exposure_residual;			//	residual exposure (because of quantization or clipping)
	ste3a_gain						igain;						//	quantized damped requested iris gain
	ste3a_gain						ngain;						//	quantized damped requested neutral density filter gain
	ste3a_time						etime;						//	quantized damped requested exposure time
	ste3a_gain						again;						//	quantized damped requested analog gain
	ste3a_gain						dgain;						//	quantized damped requested digital gain
};

#endif /* STE3A_AEC_DEBUG_H_ */
