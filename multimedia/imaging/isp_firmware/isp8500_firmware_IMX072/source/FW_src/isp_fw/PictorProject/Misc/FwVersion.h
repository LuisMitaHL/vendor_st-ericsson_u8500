/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 \if        INCLUDE_IN_HTML_ONLY
 \file      FwVersion.h
 \brief     The file holds information like the device ID and the firmware version numbers.
 \ingroup   Miscellaneous
 \endif

*/
#ifndef _FW_VERSION_H_
#   define _FW_VERSION_H_

# include "Platform.h"
# include "DeviceParameter.h"
# include "cam_drv_version.h"


/// Specifies the ID of the device
#define DEVICE_ID   8500

/// Specifies the major firmware version number Incremented With Compatibility break
#define FIRMWARE_VERSION_MAJOR  7

/// Specifies the minor firmware version number Incremented With Feature addition w/o any break in compatibility

#define FIRMWARE_VERSION_MINOR  37

/// Specifies the micro firmware version number Incremented WithBug fixes
#define FIRMWARE_VERSION_MICRO  0

/// Specifies the customer information about the sensor
#define LOW_LEVEL_API_SENSOR      (USE_LLA_CUSTOMER)

/// Default Silicon Revision No
#define DEFAULT_SILICON_VERSION                         (SiliconVersion_e_8500v1)

#endif // _FW_VERSION_H_

