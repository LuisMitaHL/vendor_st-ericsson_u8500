/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 \defgroup  Miscellaneous Miscellaneous
 \details   This module has all miscellaneous entities related
            to the firmware C code entry point, page element list
            and fimware version numbers.
*/
#ifndef DEVICEPARAMETER_H_
#   define DEVICEPARAMETER_H_

/**
 \if    INCLUDE_IN_HTML_ONLY
 \file      DeviceParameter.h

 \brief     This file contains definition of the device parameters page.

 \ingroup   Miscellaneous
 \endif
*/
#   include "Platform.h"

typedef enum
{
    /// version 8500V1 of silicon
    SiliconVersion_e_8500v1,

    /// version 8500V2 of silicon
    SiliconVersion_e_8500v2,

    /// version 9540V1 of silicon
    SiliconVersion_e_9540v1,

    /// version 850V1 of silicon
    SiliconVersion_e_8540v1

}SiliconVersion_te;

/// The page contains configuration information of 8500 firmware and hardware.
typedef struct
{
    /// device id : e.g. 8500
    uint32_t    u32_DeviceId;

    /// Firmware major version number
    uint32_t     u32_FirmwareVersionMajor;

    /// Firmware minor version number
    uint32_t     u32_FirmwareVersionMinor;

    /// Firmware micro version number
    uint32_t     u32_FirmwareVersionMicro;

    /// Sensor model to which low level api belong
    uint32_t    u32_LLA_Sensor;

    /// Major release version of low level api integrated fw
    uint32_t    u32_LLA_MajorVersion;

    /// Minor release version of low level api integrated fw
    uint32_t    u32_LLA_MinorVersion;

    /// Major release version of low level api integrated fw
    uint32_t    u32_LLCD_MajorVersion ;

    /// Minor release version of low level api integrated fw
    uint32_t    u32_LLCD_MinorVersion ;

   /// Version of the Silicon - 1 for V1, 2 for V2
   uint8_t    e_SiliconVersion;

} DeviceParameters_ts;

extern DeviceParameters_ts g_DeviceParameters;

#define Is_8540v1()          (SiliconVersion_e_8540v1 == g_DeviceParameters.e_SiliconVersion)
#define Is_8500v1()          (SiliconVersion_e_8500v1 == g_DeviceParameters.e_SiliconVersion)
#define Is_8500v2()          (SiliconVersion_e_8500v2 == g_DeviceParameters.e_SiliconVersion)
#define Is_9540v1()          (SiliconVersion_e_9540v1 == g_DeviceParameters.e_SiliconVersion)

#endif /*DEVICEPARAMETER_H_*/

