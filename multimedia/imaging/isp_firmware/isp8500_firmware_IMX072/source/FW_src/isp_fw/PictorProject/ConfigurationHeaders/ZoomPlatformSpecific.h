/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 \file      ZoomPlatformSpecific.h

 \brief     This file is NOT a part of the module release code.
            All inputs needed by the scalar module that are paltform
            dependent (like the maximum number of scalar coefficients)
            are met through this file.
            It is the responsibility of the integrator to generate
            this file at integration time and meet all the platform
            specific dependencies.
            This file must be present in incldue path of the
            integrated project during compilation of code.

 \note      The following sections define the dependencies that must be
            met by the system integrator. The way these dependencies
            have been resolved here are just for example. These dependencies
            must be appropriately resolved based on the platform being used.

 \ingroup   Zoom
*/
#ifndef ZOOMPLATFORMSPECIFIC_
#   define ZOOMPLATFORMSPECIFIC_

#   include "Platform.h"

/// Specifies the number of pipes in the device
#   define PIPE_COUNT  NO_OF_HARDWARE_PIPE_IN_ISP

/// Specifies the default step applicable per zoom in or out step. It specifies the percentage by which the field of view will increase or decrease for each zoom step.
#   define DEFAULT_ZOOM_STEP   0.2  // 2%

/// Specifies the default value of field of view from which the device will start streaming
#   define DEFAULT_SET_FOV_X   0.0

/// Specifies the default X offset from the center of the imaging array for the current field of view
#   define DEFAULT_CENTER_OFFSET_X 0

/// Specifies the default Y offset from the center of the imaging array for the current field of view
#   define DEFAULT_CENTER_OFFSET_Y 0

/// Specifies the default value of the auto zoom control
#   define DEFAULT_AUTO_ZOOM_CONTROL   Flag_e_TRUE

/// Specifies the default value of the dynamic resolution update
#   define DEFAULT_PERFORM_DYNAMIC_RESOLUTION_UPDATE   Flag_e_TRUE

/// Specifies the default zoom range
#   define DEFAULT_ZOOM_RANGE  8.0

/// Specifies the default value of zoom control coin
#   define DEFAULT_ZOOM_CONTROL_COUNT  0

/// Specifies the default value of zoom command control
#   define DEFAULT_ZOOM_COMMAND_CONTROL    ZoomCommand_e_None

/// Specifies the default value of zoom status coin
#   define DEFAULT_ZOOM_STATUS_COUNT   0

/// Specifies the default value of zoom command status
#   define DEFAULT_ZOOM_COMMAND_STATUS ZoomCommand_e_None

/// Specifies the default Command status element
#   define DEFAULT_ZOOM_COMMAND_STATUS_STATUS  ZoomCmdStatus_e_OK

/// Specifies the default request status element
#   define DEFAULT_ZOOM_REQUEST_STATUS ZoomRequestStatus_e_None

/// Specifies the depth of the LUT
#define ZOOM_DYNAMIC_OUTPUT_RESOLUTION_CHANGE_LUT_DEPTH 8



/// Specifies max DZ factor
#define DEFAULT_Max_DZ    (1.0)


#endif /* ZOOMPLATFORMSPECIFIC_ */

