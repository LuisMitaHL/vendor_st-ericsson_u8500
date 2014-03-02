/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 * \file 	ExposurePlatformSpecific.h
 * \brief 	Header File defining the Platform Specific Dependencies of the Exposure Module.
 * \ingroup ExpCtrl
*/


#ifndef EXPOSUREDEVICESPECIFIC_H_
#   define EXPOSUREDEVICESPECIFIC_H_

#include "Platform.h"


// macros for Compiler Status Page
#   define EXPOSURE_DEFAULT_STATUS_ANALOG_GAIN_PENDING                             (1.0)
#   define EXPOSURE_DEFAULT_STATUS_DIGITAL_GAIN_PENDING                            (1.0)
#   define EXPOSURE_DEFAULT_STATUS_COMPILED_EXPOSURE_TIME_US                       (1000000)  //0
#   define EXPOSURE_DEFAULT_STATUS_TOTAL_INTEGRATION_TIME_US                       (100000)   //0
#   define EXPOSURE_DEFAULT_STATUS_COARSE_INTEGRATION_PENDING_LINES                (50)       //0
#   define EXPOSURE_DEFAULT_STATUS_FINE_INTEGRATION_PENDING_PIXELS                 (150)      //0
#   define EXPOSURE_DEFAULT_STATUS_CODED_ANALOG_GAIN_PENDING                       (1)        //0

// macros for Parameters Applied Page
#   define EXPOSURE_DEFAULT_PARAMETERS_APPLIED_DIGITAL_GAIN                (1.0)
#   define EXPOSURE_DEFAULT_PARAMETERS_APPLIED_TOTAL_INTEGRATION_TIME      (15000)
#   define EXPOSURE_DEFAULT_PARAMETERS_APPLIED_COARSE_INTEGRATION_LINES    (50)
#   define EXPOSURE_DEFAULT_PARAMETERS_APPLIED_FINE_INTEGRATION_PIXELS     (50)
#   define EXPOSURE_DEFAULT_PARAMETERS_APPLIED_CODED_ANALOG_GAIN           (64)

// macros for Error Status page
#   define EPOSURE_DEFAULT_ERROR_STATUS_NUMBER_OF_FORCED_INPUT_PROC_UPDATES    (0)
#   define EPOSURE_DEFAULT_ERROR_STATUS_NUMBER_OF_CONSECUTIVE_DELAYED_FRAMES   (0)
#   define EPOSURE_DEFAULT_ERROR_STATUS_SYNC_ERROR_COUNT                       (0)
#   define EPOSURE_DEFAULT_ERROR_STATUS_FORCE_INPUT_PROC_UPDATION              (Flag_e_FALSE)

// macros for Error Control Page
#   define EXPOSURE_DEFAULT_MAXIMUM_FRAMES_FOR_EXPOSURE_ERROR       (10)

// macros for Driver Manual Control Page
#   define EXPOSURE_DEFAULT_DRIVER_MANUAL_TOTAL_TARGET_EXPOSURE_TIME_US        (10000)
#   define EXPOSURE_DEFAULT_DRIVER_MANUAL_TARGET_EXPOSURE_TIME_US              (5000)
#   define EXPOSURE_DEFAULT_DRIVER_MANUAL_TARGET_ANALOG_GAIN_x256              (512)
#   define EXPOSURE_DEFAULT_DRIVER_MANUAL_APERTURE                             (0)
#   define EXPOSURE_DEFAULT_DRIVER_MANUAL_FLASH_STATE                          (0)
#   define EXPOSURE_DEFAULT_DRIVER_MANUAL_DISTANCE_FROM_CONVERGENCE            (0)
#   define EXPOSURE_DEFAULT_DRIVER_MANUAL_FLAG_ND_FILTER                       (Flag_e_FALSE)
#   define EXPOSURE_DEFAULT_DRIVER_MANUAL_FLAG_AEC_CONVERGED                   (Flag_e_FALSE)


#endif /*EXPOSUREDEVICESPECIFIC_H_*/

