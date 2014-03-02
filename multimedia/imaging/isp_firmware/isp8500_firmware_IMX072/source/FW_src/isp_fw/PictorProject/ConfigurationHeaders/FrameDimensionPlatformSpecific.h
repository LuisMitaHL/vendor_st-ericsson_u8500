/* 
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved. 
 * This code is ST-Ericsson proprietary and confidential. 
 * Any use of the code for whatever purpose is subject to 
 * specific written permission of ST-Ericsson SA. 
 */ 
 
/**
 \file 		FrameDimensionPlatformSpecific.h

 \brief		This file is NOT a part of the module release code.
	 		All inputs needed by the frame dimension module that are paltform
	 		dependent (like the default value of page elements)
	 		are met through this file.
	 		It is the responsibility of the integrator to generate
	 		this file at integration time and meet all the platform
	 		specific dependencies.
	 		This file must be present in incldue path of the
	 		integrated project during compilation of code.

 \note		The following sections define the dependencies that must be
 			met by the system integrator. The way these dependencies
 			have been resolved here are just for example. These dependencies
 			must be appropriately resolved based on the platform being used.

 \ingroup 	Frame Dimension
*/
#ifndef FRAME_DIMENSION_PLATFORMSPECIFIC_
#   define FRAME_DIMENSION_PLATFORMSPECIFIC_

#   include "Platform.h"

/// Default value of field of view margin
#   define FRAME_DIMENSION_DEFAULT_FOV_MARGIN  1.0

/// Default value of minimum post scalar line blanking imposed by the host for pipe0
#   define FRAME_DIMENSION_DEFAULT_PIPE0_MINIMUM_INTERLINE_BEYOND_SCALAR   (32)

/// Default value of minimum post scalar line blanking imposed by the host for pipe1
#   define FRAME_DIMENSION_DEFAULT_PIPE1_MINIMUM_INTERLINE_BEYOND_SCALAR   (32)

/// Default value of minimum interframe lines
#   define FRAME_DIMENSION_DEFAULT_MINIMUM_INTERFRAME_LINES    0

/// Default maximum pre scale factor
#   define FRAME_DIMENSION_DEFAULT_MAXIMUM_PRESCALE    3

/// Default value of the AC mains frequency
#   define FRAME_DIMENSION_DEFAULT_MAINS_FREQUENCY_HZ  60

/// Default value of Anti Flicker control
#   define FRAME_DIMENSION_DEFAULT_GAURANTEE_STATIC_FLICKER_FRAME_LENGTH   Flag_e_TRUE

/// Mode of programming of CurrentFrameDimension page
#   define FRAME_DIMENSION_DEFAULT_CURRENT_FRAME_DIM_PROGRAMMING_MODE  FrameDimensionProgMode_e_Auto

/// Flag to specify whether sensor changeover (reprogram) is allowed or not
#   define FRAME_DIMENSION_DEFAULT_ALLOW_CHANGE_OVER   Flag_e_TRUE

/// Specifies the default value of pre scale factor in current frame dimension
#   define FDM_DEFAULT_PRE_SCALE_FACTOR    0

/// Specifies the default value of frame length lines in current frame dimension
#   define FDM_DEFAULT_FRAME_LENGTH_LINES  0

/// Specifies the default value of line length pck in current frame dimension
#   define FDM_DEFAULT_LINE_LENGTH_PCK 0

/// Specifies the default value of x addr start in current frame dimension
#   define FDM_DEFAULT_X_ADDR_START    0

/// Specifies the default value of y addr start in current frame dimension
#   define FDM_DEFAULT_Y_ADDR_START    0

/// Specifies the default value of x addr end in current frame dimension
#   define FDM_DEFAULT_X_ADDR_END  0

/// Specifies the default value of y addr end in current frame dimension
#   define FDM_DEFAULT_Y_ADDR_END  0

/// Specifies the default value of op x output size in current frame dimension
#   define FDM_DEFAULT_OP_X_OUTPUT_SIZE    0

/// Specifies the default value of op y output size in in current frame dimension
#   define FDM_DEFAULT_OP_Y_OUTPUT_SIZE    0

/// Specifies the default value of vt x output size in current frame dimension
#   define FDM_DEFAULT_VT_X_OUTPUT_SIZE    0

/// Specifies the default value of vt y output size in current frame dimension
#   define FDM_DEFAULT_VT_Y_OUTPUT_SIZE    0

/// Specifies the default value of x odd inc in current frame dimension
#   define FDM_DEFAULT_X_ODD_INC   0

/// Specifies the default value of y odd inc in current frame dimension
#   define FDM_DEFAULT_Y_ODD_INC   0

/// Specifies the default value of scalar m in current frame dimension
#   define FDM_DEFAULT_SCALAR_M    0

/// Specifies the default value of non active columns at left edge in current frame dimension
#   define FDM_DEFAULT_NON_ACTIVE_COLUMNS_LEFT_EDGE    0

/// Specifies the default value of non active columns at right edge in current frame dimension
#   define FDM_DEFAULT_NON_ACTIVE_COLUMNS_RIGHT_EDGE   0

/// Specifies the default value of non active rows at top edge in current frame dimension
#   define FDM_DEFAULT_NON_ACTIVE_ROWS_TOP_EDGE    0

/// Specifies the default value of non active rows at bottom edge in current frame dimension
#   define FDM_DEFAULT_NON_ACTIVE_ROWS_BOTTOM_EDGE 0

/// Specifies the default value of status lines in current frame dimension
#   define FDM_DEFAULT_STATUS_LINES    0

/// Specifies the default value of sensor pre scale type in current frame dimension
#   define FDM_DEFAULT_SENSOR_PRESCALE_TYPE    SensorPrescaleType_e_SensorSubsample

/// Specifies the default value of sensor scaling mode in current frame dimension
#   define FDM_DEFAULT_SENSOR_SCALING_MODE FDSensorScalingMode_e_SENSOR_SCALING_HORIZONTAL_AND_VERTICAL
#endif /* FRAME_DIMENSION_PLATFORMSPECIFIC_ */

