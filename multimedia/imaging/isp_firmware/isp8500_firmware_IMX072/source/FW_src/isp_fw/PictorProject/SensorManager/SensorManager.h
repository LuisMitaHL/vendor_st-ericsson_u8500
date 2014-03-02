/* 
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved. 
 * This code is ST-Ericsson proprietary and confidential. 
 * Any use of the code for whatever purpose is subject to 
 * specific written permission of ST-Ericsson SA. 
 */ 
 
/**
 \defgroup SensorManager Sensor Manager
 \details  Sensor manager module manage sensor contraint reads, sensor information from a SMIA compliant sensor
*/

/**
 \if INCLUDE_IN_HTML_ONLY
 \file      SensorManager.h

 \brief     The file implement sensor manager structures

 \ingroup   SensorManager
 \endif
*/
#ifndef _SENSOR_MANAGER_H_
#   define _SENSOR_MANAGER_H_

#   include "Platform.h"
#   include "SystemConfig.h"
#   include "ExpSensor.h"

/**
 \struct SensorInformation_ts
 \brief  Status page element contain sensor specific information. All the values are set to 0 as default.
         The page element values are only valid when e_Flag_Available == e_TRUE
 \ingroup SensorManager
*/
typedef struct
{
    /// Sensor model ID / 16-bit Sensor model number \n
    /// [DEFAULT]: 0x0000
    uint16_t    u16_model_id;

    /// Revision number or Silicon Revision Number \n
    /// [DEFAULT]: 0x00
    uint8_t     u8_revision_number;

    /// Manufacturer ID \n
    /// [DEFAULT]: 0x00
    uint8_t     u8_manufacturer_id;

    /// SMIA version \n
    /// [DEFAULT]: 0x00
    uint8_t     u8_smia_version;

    /// Status if firmware is able to contact sensor \n
    /// e_TRUE: Communication successtull\n
    /// e_FALSE: Communication with sensor failed or some error \n
    /// [DEFAULT]: e_FALSE
    uint8_t     e_Flag_Available;

    //OTP
    //serial_ID (6 bytes)
    uint8_t             u8_serial_id_0;
    uint8_t		u8_serial_id_1;
    uint8_t		u8_serial_id_2;
    uint8_t		u8_serial_id_3;
    uint8_t		u8_serial_id_4;
    uint8_t		u8_serial_id_5;
	
} SensorInformation_ts;

// The PE has been moved to the SystemConfig.h File in SystemSetup_ts Page.
/*
typedef struct
{
	/// Number of microseconds after which the sensor will output good
	/// clocks after given the start sensor command
	/// i.e., after how many us later does the sensor start(good clocks).\n
	float_t    f_SensorStartDelay_us;

	/// Number of microseconds after which the sensor will output no
	/// clocks after given the stop sensor command
	/// i.e., after how many us later does the sensor stop(no clocks).\n
	///float_t    f_SensorStopDelay_us;
} SensorInputParameters_ts;
*/

extern SensorInformation_ts g_SensorInformation;
#endif // _SENSOR_MANAGER_H_

