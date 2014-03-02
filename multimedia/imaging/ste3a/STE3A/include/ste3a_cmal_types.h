/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * \file    ste3a_cmal_types.h
 * \brief   STE3A camera module abstraction types
 * \author  ST-Ericsson Image Quality Team, Agrate (Italy)
 */

#ifndef STE3A_CMAL_TYPES_H_
#define STE3A_CMAL_TYPES_H_

#include "ste3a_types.h"


typedef uint32_t ste3a_logical_lens_displacement;			// camera module specific, HW units


struct ste3a_cammod_exposure_status							// camera module exposure parameters status
{
	ste3a_time etime;										//		exposure time
	ste3a_gain again;										//		analog gain
	ste3a_gain igain;										//		iris gain
	ste3a_gain ngain;										//		neutral density filter gain
};

struct ste3a_cammod_exposure_config							// camera module exposure parameters descriptor
{
	ste3a_time etime;										//		exposure time
	ste3a_gain again;										//		analog gain
	ste3a_gain igain;										//		iris gain
	ste3a_gain ngain;										//		neutral density filter gain
};

struct ste3a_cammod_focus_status							// camera module lens actuator parameters status
{
	ste3a_bool lens_moving;									//		lens moving flag
};

enum ste3a_lens_sequence_reference							// reference points for lens sequences
{
	ste3a_lens_sequence_reference_cur = STE3A_ENUM_BASE,	//		current position
	ste3a_lens_sequence_reference_inf,						//		infinity-side mechanical stop
	ste3a_lens_sequence_reference_mac						//		macro-side mechanical stop
};

#define STE3A_CMAL_FOCUS_SEQUENCE_MAX_STEPS	64

struct ste3a_cammod_focus_config							// camera module focus parameters descriptor
{
	ste3a_lens_sequence_reference	reference;										//	reference point for lens sequence
	ste3a_count						steps;											//	number of the steps in the sequence (0 means no action is required)
	ste3a_logical_lens_displacement	positions[STE3A_CMAL_FOCUS_SEQUENCE_MAX_STEPS];	//	arrayf of destination positions, one for each step
};

/***** Camera Module Abstraction Layer callbacks (exposure quantization) *****
*
* requests the quantization of exposure parameters according to the available
* camera module:
*
*   IT = integration time (electronic shutter)
*   ET = exposure time (mechanical shutter)
*   AG = analog gain
*   IR = iris relative gain/attenuation
*   NF = neutral density filter relative gain/attenuation
*
* inputs:
*
* context       context for the called function
* desired       desired value, to be quantized or clipped
*
* outputs:
*
* lower         best approximation of the requested value, on the lower side
* higher        best approximation of the requested value, on the higher side
*
* returns:      true if the requested value is in the allowed range
*
*****************************************************************************/

typedef ste3a_bool ste3acb_cmal_closest_IT(const void * context, ste3a_time desired, ste3a_time & lower, ste3a_time & higher);
typedef ste3a_bool ste3acb_cmal_closest_ET(const void * context, ste3a_time desired, ste3a_time & lower, ste3a_time & higher);
typedef ste3a_bool ste3acb_cmal_closest_AG(const void * context, ste3a_gain desired, ste3a_gain & lower, ste3a_gain & higher);
typedef ste3a_bool ste3acb_cmal_closest_IR(const void * context, ste3a_gain desired, ste3a_gain & lower, ste3a_gain & higher);
typedef ste3a_bool ste3acb_cmal_closest_NF(const void * context, ste3a_gain desired, ste3a_gain & lower, ste3a_gain & higher);

/***** Camera Module Abstraction Layer callbacks (lens actuator model setup) **
*
* programs the lens actuator model for transforming HW units (logical) to SW
* units (normalized), and vice-versa
*
* inputs:
*
* context       context for the called function
* mec_inf       HW value for mechanical stop on the infinity side (AKA "start
*               of controllability")
* nom_inf       nominal value for the infinity position
* nom_mac       nominal value for the macro position
* mec_mac       HW value for mechanical stop on the macro side (AKA "end of
*               controllability")
*
*****************************************************************************/

typedef void ste3acb_cmal_set_LA_model(const void * context, ste3a_logical_lens_displacement mec_inf, ste3a_logical_lens_displacement nom_inf, ste3a_logical_lens_displacement nom_mac, ste3a_logical_lens_displacement mec_mac);

/***** Camera Module Abstraction Layer callbacks (lens position conversion) **
*
* transform HW units (logical) to SW units (normalized), and vice-versa
*
* inputs:
*
* context       context for the called function
*
* inputs/outputs:
*
* lvalue        logical position (HW units)
* nvalue        normalized position (SW units)
*
* returns:      true if the requested value is in the physical range
*
*****************************************************************************/

typedef ste3a_bool ste3acb_cmal_convert_LA_ln(const void * context, ste3a_logical_lens_displacement lvalue, ste3a_normalized_lens_displacement & nvalue);
typedef ste3a_bool ste3acb_cmal_convert_LA_nl(const void * context, ste3a_normalized_lens_displacement nvalue, ste3a_logical_lens_displacement & lvalue);

/***** Camera Module Abstraction Layer callbacks (lens calibrated range) *****
*
* return the normalized endpoints (inf/macro) of the physical range, in
* normalized (SW) units and according to calibration data
*
* inputs:
*
* context       context for the called function
*
* outputs:
*
* nvalue        normalized position (SW units)
*
*****************************************************************************/

typedef void ste3acb_cmal_range_LA_ninf(const void * context, ste3a_normalized_lens_displacement & nvalue);
typedef void ste3acb_cmal_range_LA_nmac(const void * context, ste3a_normalized_lens_displacement & nvalue);

#endif /* STE3A_CMAL_TYPES_H_ */
