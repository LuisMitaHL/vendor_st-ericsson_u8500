/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 \if        INCLUDE_IN_HTML_ONLY
 \file      Aperture.h

 \brief     This file exposes an interface to allow other modules of the device
            to access the aperture module.

 \ingroup   Aperture
 \endif
*/
#ifndef _APERTURE_H_
#   define _APERTURE_H_

#   include "Platform.h"

/**
 \struct    ApertureConfig_Control_ts
 \brief
 \ingroup
 \endif
*/
typedef struct
{
    /// Select the aperture (format should be f_numberx100 )
    /// supported apertures can be read using g_ApertureConfig_Control.u16_SelectAperture & g_ApertureConfig_Status.u16_f_number_x_100
    /// Toggle following coin to apply the aperture in sensor:  g_SystemSetup.e_Coin_Ctrl.
    uint16_t    u16_requested_f_number_x_100;

    /// Select whiich configuration to read from driver
    /// Possible values 0 - (g_ApertureConfig_Status.u16_number_of_apertures - 1)
    uint16_t    u16_SelectAperture;

    /// Control coin
    /// [NOTE]: The coin should only be used to get aperture values from the driver. Toggling this coin will not apply value in the sensor.
    uint8_t     e_Coin_Ctrl;
} ApertureConfig_Control_ts;

/**
 \struct    ApertureConfig_Status_ts
 \brief
 \ingroup
 \endif
*/
typedef struct
{
    /// Currently applied f_numberx100 in the sensor
    uint16_t    u16_applied_f_number_x_100;

    /// Number of aperture supported by the driver
    /// The read value is populated after BOOT command.
    uint16_t    u16_number_of_apertures;

    /// aperture valuex100 (normally defined as f number)
    /// the value correspond to the aperture number selected in g_ApertureConfig_Control.u16_SelectAperture and coin toggled after that.
    uint16_t    u16_f_number_x_100;

    /// Status coin becomes equal to control coin
    uint8_t     e_Coin_Status;
} ApertureConfig_Status_ts;

// Based on ER342576
#   define DEFAULT_APERTURE_VALUE_X100 (100)

uint8_t LLA_Abstraction_Is_Aperture_Supported ( void ) TO_EXT_DDR_PRGM_MEM;
extern ApertureConfig_Control_ts g_ApertureConfig_Control   TO_EXT_DATA_MEM;
extern ApertureConfig_Status_ts g_ApertureConfig_Status     TO_EXT_DATA_MEM;
extern void ReadApertureConfig ( void ) TO_EXT_DDR_PRGM_MEM;

extern void LLA_Abstraction_Set_Aperture ( void ) TO_EXT_DDR_PRGM_MEM;
#endif // _APERTURE_H_

