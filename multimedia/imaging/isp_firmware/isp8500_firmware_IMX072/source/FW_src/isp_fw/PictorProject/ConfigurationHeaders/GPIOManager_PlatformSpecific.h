/* 
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved. 
 * This code is ST-Ericsson proprietary and confidential. 
 * Any use of the code for whatever purpose is subject to 
 * specific written permission of ST-Ericsson SA. 
 */ 
 
/**
 \file GPIOManager_PlatformSpecific.h
 \brief This file is NOT a part of the module release code.
	 		All inputs needed by the GPIO Manager module that can only
	 		be resolved at the project level (at integration time)
	 		are met through this file.
	 		It is the responsibility of the integrator to generate
	 		this file at integration time and meet all the input
	 		dependencies.


 \note      The following sections define the dependencies that must be
           met by the system integrator. The way these dependencies
           have been resolved here are just for example. These dependencies
           must be appropriately resolved based on the platform being used.
 \ingroup GPIO Manager
*/
#ifndef _GPIOMANAGER_PLATFORMSPECIFIC_H_
#   define _GPIOMANAGER_PLATFORMSPECIFIC_H_

#	include"GPIOManager.h"

/// Default value of main flash GPO
#define GPIO_MANAGER_MAIN_FLASH_GPO	0

/// Default value of Sensor 0 XShutdown GPO
#define GPIO_MANAGER_SENSOR_0_XSHUTDOWN_GPO	5

/// Default value of Sensor 1 XShutdown GPO
#define GPIO_MANAGER_SENSOR_1_XSHUTDOWN_GPO	6

/// Default FLASH IDP Timer
#   define GPIO_MANAGER_FLASH_IDP_TIMER	4

/// Flash manager timer select
#   define	GPIO_MANAGER_FLASH_TIMERSELECT	0x0


#endif // _GPIOMANAGER_PLATFORMSPECIFIC_H_

