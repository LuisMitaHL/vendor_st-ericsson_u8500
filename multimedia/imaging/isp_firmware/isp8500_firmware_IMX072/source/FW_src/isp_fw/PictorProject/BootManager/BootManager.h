/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 \defgroup  BootManager Boot Manager

 \details   This module performs all the initialisations that are required
            to be done in context of a BOOT command to the Host Interface.
            The host must ensure that the inputs to all the modules that
            are initialised in context of BOOT command are valid before
            issuing a BOOT command.

 \par       The following modules are initialised in context of the BOOT command:
            - Master I2c
*/
#ifndef BOOTMANAGER_H_
#   define BOOTMANAGER_H_

#   include "SensorManager.h"

/**
 \if INCLUDE_IN_HTML_ONLY
 \file      BootManager.h

 \brief     This file exposes an interface to allow other modules of the device
            to access the Boot Manager module.

 \ingroup   BootManager
 \endif
*/
extern void BootManager_Boot ( void ) TO_EXT_DDR_PRGM_MEM;
#endif /*BOOTMANAGER_H_*/

