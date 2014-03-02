/* 
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved. 
 * This code is ST-Ericsson proprietary and confidential. 
 * Any use of the code for whatever purpose is subject to 
 * specific written permission of ST-Ericsson SA. 
 */ 
 
/**
 * \file    FLADriverPlatformSpecific.h
 * \brief   Header File containing external function declarations and defines for FLADriver and FLADriver Compiler.
 * \details This File Contains the default values as define for the Control ,status and Low Level parameters
 *          for the focus Lens Actuator Driver .
 *          NVM defines are taken as zero ,as the Actual value for these are taken from the sensor .
 *          The defualt value for the Host programmable params the set if host doesn't specify the
 *          new values , these values May change according to the type of the module used .
 * \ingroup FLADriver
*/
#ifndef FLADRIVERPLATFORMSPECIFIC_H_
#   define FLADRIVERPLATFORMSPECIFIC_H_

#   include "FLADriverSensor.h"


//  Default value for the FLADriver_Status
#   define FLAD_DEFAULT_LOW_LEVEL_PARAMETER_LENS_MIN_POSITION  FLAD_DEFAULT_LOW_LEVEL_PARAMETER_CTRL_MIN_POSITION
#   define FLAD_DEFAULT_LOW_LEVEL_PARAMETER_LENS_MAX_POSITION  FLAD_DEFAULT_LOW_LEVEL_PARAMETER_CTRL_MAX_POSITION
#   define FLAD_DEFAULT_LOW_LEVEL_PARAMETER_LENS_REST_POSITION FLAD_DEFAULT_LOW_LEVEL_PARAMETER_CTRL_REST_POSITION
#   define FLAD_DEFAULT_LOW_LEVEL_PARAMETER_LENS_INFINITY_FAR_END_POSITION \
        FLAD_DEFAULT_LOW_LEVEL_PARAMETER_CTRL_INFINITY_FAR_END_POSITION
#   define FLAD_DEFAULT_LOW_LEVEL_PARAMETER_LENS_INFINITY_HOR_POSITION \
        FLAD_DEFAULT_LOW_LEVEL_PARAMETER_CTRL_INFINITY_HOR_POSITION
#   define FLAD_DEFAULT_LOW_LEVEL_PARAMETER_LENS_HYPERFOCAL_POSITION \
        FLAD_DEFAULT_LOW_LEVEL_PARAMETER_CTRL_HYPERFOCAL_POSITION
#   define FLAD_DEFAULT_LOW_LEVEL_PARAMETER_LENS_HOR_MACRO_POSITION \
        FLAD_DEFAULT_LOW_LEVEL_PARAMETER_CTRL_MACRO_HOR_POSITION
#   define FLAD_DEFAULT_LOW_LEVEL_PARAMETER_LENS_MACRO_NEAR_END_POSITION \
        FLAD_DEFAULT_LOW_LEVEL_PARAMETER_CTRL_MACRO_NEAR_END_POSITION
#   define FLAD_DEFAULT_LOW_LEVEL_PARAMETER_LENS_TOLERANCE FLAD_DEFAULT_LOW_LEVEL_PARAMETER_CTRL_POSITION_TOLERANCE
#   define FLAD_DEFAULT_LOW_LEVEL_PARAMETER_LENS_TARGET_SET_POSITION (0)
#   define FLAD_DEFAULT_LOW_LEVEL_PARAMETER_LENS_MANUAL_STEP_SIZE          FLAD_DEFAULT_LOW_LEVEL_PARAMETER_CTRL_MANUAL_STEP_SIZE
#   define FLAD_DEFAULT_LOW_LEVEL_PARAMETER_LENS_CURRENT_POSITION          0
#   define FLAD_DEFAULT_LOW_LEVEL_PARAMETER_LENS_TARGET_POSITION           0
#   define FLAD_DEFAULT_LOW_LEVEL_PARAMETER_LENS_PREVIOUS_POSITION         0
#   define FLAD_DEFAULT_LOW_LEVEL_PARAMETER_LENS_LAST_STEP_SIZE_EXECUTED   0

#   define FLAD_DEFAULT_CTRL_TIME_LIMIT_MS                                 10
#   define FLAD_DEFAULT_CTRL_RANGE_DEF_RANGE_CTRL                          FLADriver_RangeDef_e_NVM_LEVEL_RANGE

#   define FLAD_DEFAULT_STATUS_CYCLES                                      0
#   define FLAD_DEFAULT_STATUS_LENS_IS_MOVING                              Flag_e_FALSE
#   define FLAD_DEFAULT_STATUS_LIMITS_EXCEEDED                             Flag_e_FALSE
#   define FLAD_DEFAULT_STATUS_LOW_LEVEL_DRIVER_INITIALIZED                Flag_e_FALSE
#   define FLAD_DEFAULT_STATUS_AF_SENSOR_ORIENTATION_TYPE \
        FLADriver_ActuatorOrientation_e_DIRECTION_REVERSED_WRT_TO_STANDARDS
#   define FLAD_DEFAULT_STATUS_RANGE_DEF_RANGE_STATUS  FLADriver_RangeDef_e_NVM_LEVEL_RANGE

#   define FLAD_DEFAULT_NVM_MACRO_POS                  (-1)
#   define FLAD_DEFAULT_NVM_INFINITY_POS               (-1)
#   define FLAD_DEFAULT_NVM_INFINITY_HOR               (-1)
#   define FLAD_DEFAULT_NVM_MACRO_HOR                  (-1)
#   define FLAD_DEFAULT_NVM_HYPERFOCAL_POS             (-1)
#   define FLAD_DEFAULT_NVM_REST_POS                   (-1)
#   define FLAD_DEFAULT_NVM_DATA_PRESENT               Flag_e_TRUE
#   define FLAD_DEFAULT_NVM_ACTUATOR_LENS_PRESENT      Flag_e_TRUE
#   define FLAD_DEFAULT_NVM_POSITION_SENSOR_PRESENT    Flag_e_TRUE
#endif /*FLADRIVERPLATFORMSPECIFIC_H_*/

