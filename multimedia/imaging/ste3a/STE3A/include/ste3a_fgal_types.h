/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * \file    ste3a_fgal_types.h
 * \brief   STE3A flash gun abstraction types
 * \author  ST-Ericsson Image Quality Team, Agrate (Italy)
 */

#ifndef STE3A_FGAL_TYPES_H_
#define STE3A_FGAL_TYPES_H_

#include "ste3a_types.h"


struct ste3a_flash_exposure_status							// flashgun device parameters
{
	ste3a_power power;										//		normalized power level
	ste3a_time  time;										//		flash duration
};

struct ste3a_flash_exposure_config							// flashgun device parameters descriptor
{
	ste3a_power power;										//		normalized power
	ste3a_time  time;										//		duration
};

/***** Flash Gun Abstraction Layer callbacks *********************************
*
* requests the quantization of exposure parameters according to the available
* flashgun device (ISP):
*
*   TP = time (duration) and (normalized) power
*
* inputs:
*
* context       context for the called function
* desired_time  desired duration, to be quantized or clipped
* desired_power desired normalized power, to be quantized or clipped
*
* outputs:
*
* lower_time    best approximation of the requested duration, on the lower side
* higher_time   best approximation of the requested duration, on the higher side
* lower_power   best approximation of the requested normalized power, on the lower side
* higher_power  best approximation of the requested normalized power, on the higher side
*
* returns:      true if the requested energy (duration + power) is in the allowed range
*
*****************************************************************************/

typedef ste3a_bool ste3acb_fgal_closest_TP(const void * context, const ste3a_time desired_time, const ste3a_power desired_power, ste3a_time & lower_time, ste3a_power & lower_power, ste3a_time & higher_time, ste3a_power & higher_power);

#endif /* STE3A_FGAL_TYPES_H_ */
