/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 \file FLADriverSensor.h
 \brief This file is a part of the release code of the FLADriver module.
        This file contains the sensor specific defines and can change for the different module .
        contains the address for the Hw registers to access the VCM and NVM information .
        Aprt from the HW registers this Also has defines for the Low Level Parameters used ny FLADriver Module
        ,which can change if another sensor is used .so all the sensor / Module specific details in terms of defines
        are stored inside this file .
 \ingroup VideoTiming
*/
#ifndef FLADRIVERSENSOR_H_
#   define FLADRIVERSENSOR_H_

// DAC VALUES ACCORDING TO SENSOR USED
// these defines are needed for the 851 vcm Actuator
#   define FLAD_851VCM_VAR_MAX_DAC_VALUE       511
#   define FLAD_851VCM_VAR_MIN_DAC_VALUE       0
#   define FLAD_851VCM_VAR_POS_ACTUATOR_HOME   0

// these defines are needed for the PIEZO actuator
#   define FLAD_VAR_MAX_DAC_VALUE      1023
#   define FLAD_VAR_MIN_DAC_VALUE      0
#   define FLAD_VAR_POS_ACTUATOR_HOME  0


// define for the control/status Pages .
#   define FLAD_DEFAULT_LOW_LEVEL_PARAMETER_CTRL_MIN_POSITION  FLAD_VAR_MIN_DAC_VALUE
#   define FLAD_DEFAULT_LOW_LEVEL_PARAMETER_CTRL_MAX_POSITION  FLAD_VAR_MAX_DAC_VALUE
#   define FLAD_DEFAULT_LOW_LEVEL_PARAMETER_CTRL_MAX_MIN_DIFF  ( \
        FLAD_DEFAULT_LOW_LEVEL_PARAMETER_CTRL_MAX_POSITION - FLAD_DEFAULT_LOW_LEVEL_PARAMETER_CTRL_MIN_POSITION)
#   define FLAD_DEFAULT_LOW_LEVEL_PARAMETER_CTRL_REST_POSITION FLAD_VAR_POS_ACTUATOR_HOME
#   define FLAD_DEFAULT_LOW_LEVEL_PARAMETER_CTRL_INFINITY_FAR_END_POSITION \
        FLAD_DEFAULT_LOW_LEVEL_PARAMETER_CTRL_MIN_POSITION
#   define FLAD_DEFAULT_LOW_LEVEL_PARAMETER_CTRL_INFINITY_HOR_POSITION ( \
        FLAD_DEFAULT_LOW_LEVEL_PARAMETER_CTRL_MIN_POSITION +             \
            (FLAD_DEFAULT_LOW_LEVEL_PARAMETER_CTRL_MAX_MIN_DIFF / 6))
#   define FLAD_DEFAULT_LOW_LEVEL_PARAMETER_CTRL_HYPERFOCAL_POSITION \
        FLAD_DEFAULT_LOW_LEVEL_PARAMETER_CTRL_INFINITY_HOR_POSITION
#   define FLAD_DEFAULT_LOW_LEVEL_PARAMETER_CTRL_MACRO_HOR_POSITION        ( \
        FLAD_DEFAULT_LOW_LEVEL_PARAMETER_CTRL_MAX_POSITION -                 \
            (FLAD_DEFAULT_LOW_LEVEL_PARAMETER_CTRL_MAX_MIN_DIFF / 6))
#   define FLAD_DEFAULT_LOW_LEVEL_PARAMETER_CTRL_MACRO_NEAR_END_POSITION   FLAD_DEFAULT_LOW_LEVEL_PARAMETER_CTRL_MAX_POSITION
#   define FLAD_DEFAULT_LOW_LEVEL_PARAMETER_CTRL_POSITION_TOLERANCE        0
#   define FLAD_DEFAULT_LOW_LEVEL_PARAMETER_CTRL_TARGET_SET_POSITION       ( \
			(																	 \
				FLAD_DEFAULT_LOW_LEVEL_PARAMETER_CTRL_MAX_POSITION +			 \
				FLAD_DEFAULT_LOW_LEVEL_PARAMETER_CTRL_MIN_POSITION				 \
			) / 																 \
					2)
#   define FLAD_DEFAULT_LOW_LEVEL_PARAMETER_CTRL_MANUAL_STEP_SIZE          30
#   define FLADriver_AFStatsGetStatsWithLensMoveFWStatus()             AFStats_GetStatsWithLensMoveFWStatus()



#endif /*FLADRIVERSENSOR_H_*/

