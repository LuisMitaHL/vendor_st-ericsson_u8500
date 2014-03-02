/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 \if INCLUDE_IN_HTML_ONLY
 \file 	DeviceParameter.c

 \brief The file contains the Device Parameter page.

 \ingroup Miscellaneous
 \endif
*/
#include "DeviceParameter.h"
#include "FwVersion.h"
#include "cam_drv.h"
#include "cam_drv_version.h"


DeviceParameters_ts g_DeviceParameters =
{
    DEVICE_ID,
    FIRMWARE_VERSION_MAJOR,
    FIRMWARE_VERSION_MINOR,
    FIRMWARE_VERSION_MICRO,
    LOW_LEVEL_API_SENSOR,
    CAM_DRV_API_VERSION_MAJOR,
    CAM_DRV_API_VERSION_MINOR,
    CAM_DRV_DRIVER_VERSION_MAJOR,
    CAM_DRV_DRIVER_VERSION_MINOR,
    DEFAULT_SILICON_VERSION
};

