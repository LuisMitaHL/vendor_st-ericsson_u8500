/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * \file    ste3a_calibration_types.h
 * \brief   STE3A calibration types
 * \author  ST-Ericsson Image Quality Team, Agrate (Italy)
 */

#ifndef STE3A_CALIBRATION_TYPES_H_
#define STE3A_CALIBRATION_TYPES_H_

#include "ste3a_types.h"
#include "ste3a_cmal_types.h"

#define STE3A_CALIBRATION_N_OF_ILL_MAX		5

struct ste3a_ch_sensitivity
{
	ste3a_float	cie_x;
	ste3a_float	cie_y;
	ste3a_float	rr;
	ste3a_float	gr;
	ste3a_float	gb;
	ste3a_float	bb;
};

struct ste3a_ch_sensitivities
{
	ste3a_count          count;
	ste3a_ch_sensitivity data[STE3A_CALIBRATION_N_OF_ILL_MAX];
};

struct ste3a_calibration_data_lens_actuator
{
	ste3a_logical_lens_displacement mec_inf;
	ste3a_logical_lens_displacement nom_inf;
	ste3a_logical_lens_displacement nom_mac;
	ste3a_logical_lens_displacement mec_mac;
};

struct ste3a_calibration_data
{
	bool									valid;
	ste3a_ch_sensitivities					sensitivity;
	ste3a_calibration_data_lens_actuator	lens_actuator;
};

#endif /* STE3A_CALIBRATION_TYPES_H_ */

