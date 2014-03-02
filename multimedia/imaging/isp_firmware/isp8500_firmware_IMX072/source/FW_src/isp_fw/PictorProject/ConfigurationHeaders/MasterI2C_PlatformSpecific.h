/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 \file MasterI2C_PlatformSpecific.h
 \brief This file is NOT a part of the module release code.
            All inputs needed by the MaterI2C module that can only
            be resolved at the project level (at integration time)
            are met through this file.
            It is the responsibility of the integrator to generate
            this file at integration time and meet all the input
            dependencies.


 \note      The following sections define the dependencies that must be
           met by the system integrator. The way these dependencies
           have been resolved here are just for example. These dependencies
           must be appropriately resolved based on the platform being used.
 \ingroup MasterI2C
*/
#ifndef _MASTERI2C_PLATFORM_SPECIFIC_H_
#   define _MASTERI2C_PLATFORM_SPECIFIC_H_

#   include "Platform.h"

/// Default clock values of master i2c registers to allow the master i2c to run @ 375Khz with 20Mhz of host clock
#if 0
// I2C settings for 100 KHz
#   define MASTER_I2C_DEFAULT_COUNT_FALL           (0x10)
#   define MASTER_I2C_DEFAULT_COUNT_RISE           (0x10)
#   define MASTER_I2C_DEFAULT_COUNT_HIGH           (0x20)
#   define MASTER_I2C_DEFAULT_COUNT_BUFFER         (0x20)
#   define MASTER_I2C_DEFAULT_COUNT_HOLD_DATA      (0x10)
#   define MASTER_I2C_DEFAULT_COUNT_SETUP_DATA     (0x10)
#   define MASTER_I2C_DEFAULT_COUNT_HOLD_START     (0x00)
#   define MASTER_I2C_DEFAULT_COUNT_SETUP_START    (0x00)
#   define MASTER_I2C_DEFAULT_COUNT_SETUP_STOP     (0x20)
#else
// I2C settings for 400 KHz
#   define MASTER_I2C_DEFAULT_COUNT_FALL           (0x01)
#   define MASTER_I2C_DEFAULT_COUNT_RISE           (0x02)
#   define MASTER_I2C_DEFAULT_COUNT_HIGH           (0x1A)
#   define MASTER_I2C_DEFAULT_COUNT_BUFFER         (0x30)

#   define MASTER_I2C_DEFAULT_COUNT_HOLD_DATA      (0x0A)
#   define MASTER_I2C_DEFAULT_COUNT_SETUP_DATA     (0x10)

#   define MASTER_I2C_DEFAULT_COUNT_HOLD_START     (0x16)
#   define MASTER_I2C_DEFAULT_COUNT_SETUP_START    (0x16)
#   define MASTER_I2C_DEFAULT_COUNT_SETUP_STOP     (0x16)
#endif

/// u8_MaximumNumberOfGrabAttempts: Default maximum number of grab attempts
#   define MASTER_I2C_DEFAULT_MAX_NUMBER_MASTERI2C_GRAB_ATTEMPTS   (10)

/// Maximum no of bytes that can read or write to the MasterI2C
#   define MASTER_I2C_NO_OF_BYTES_FOR_HOST_TO_MASTERI2C_ACCESS (16)
#   define MASTER_I2C_DEFAULT_DEVICE_ID                        (0x20)
#endif // _MASTERI2C_PLATFORM_SPECIFIC_H_

