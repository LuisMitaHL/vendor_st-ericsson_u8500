/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 \defgroup GPIOManager GPIO Manager Module
 \details GPIO manager provides an abstraction layer for the GPIO hardware.
*/

/**
 \if INCLUDE_IN_HTML_ONLY
 \file GPIOManager.h
 \brief declare data structure and functions used in GPIO Manager module
 \ingroup GPIOManager
 \endif
*/
#ifndef GPIOMANAGER_H_
#define GPIOMANAGER_H_


#   include "GPIOManager_OpInterface.h"
#	include "PictorhwReg.h"
#	include "GPIOManager_ip_interface.h"
#   include "SystemConfig.h"

/**
 \enum FlashGPO_te
 \brief ISP FW client shall take care that only GPIO 0 to 5
		can be used to implement flash functionality.Flash
		GPIOs are mapped on paired GPIO.
 \ingroup GPIO Manager
*/

#if 0
typedef enum
{
    /// Main Flash GPO on GPIO 0
	/// Pre flash on GPIO 1
    FlashGPO_e_GPIO_0,
    FlashGPO_e_GPIO_1,
    /// Main Flash GPO on GPIO 2
	/// Pre flash on GPIO 3
    FlashGPO_e_GPIO_2,
    FlashGPO_e_GPIO_3,
    /// Main Flash GPO on GPIO 4
	/// Pre flash on GPIO 5
    FlashGPO_e_GPIO_4,
    FlashGPO_e_GPIO_5,
	/// Main Flash GPO on GPIO 6
	/// Pre flash on GPIO 7
    FlashGPO_e_GPIO_6,
    FlashGPO_e_GPIO_7
} FlashGPO_te;
#endif



#define GPIO_INPUT    0
#define GPIO_OUTPUT   1

// Intelligent GPIOs

//GPIO 0
#define Gpio_0_map	0
#define Gpio_0_type	GPIO_INPUT
  
//GPIO 1
#define Gpio_1_map	1
#define Gpio_1_type	GPIO_INPUT
  
//GPIO 2
#define Gpio_2_map	2
#define Gpio_3_type	GPIO_INPUT
  
//GPIO 3
#define Gpio_3_map	3
#define Gpio_2_type	GPIO_INPUT

  
//GPIO 4
#define Gpio_4_map	4
#define Gpio_4_type	GPIO_INPUT
  
//GPIO 5
#define Gpio_5_map	5
#define Gpio_5_type	GPIO_INPUT

//GPIO 6
#define Gpio_6_map	6
#define Gpio_6_type	GPIO_INPUT
  
//GPIO 7
#define Gpio_7_map	7
#define Gpio_7_type	GPIO_INPUT

// GPIOs 8 to 15 are not connected externally to Nomadik chip.
//GPIO 8
#define Gpio_8_map	8
#define Gpio_8_type	GPIO_INPUT
  
//GPIO 9
#define Gpio_9_map	9
#define Gpio_9_type	GPIO_INPUT
  
//GPIO 10
#define Gpio_10_map	10
#define Gpio_10_type	GPIO_INPUT
  
//GPIO 11
#define Gpio_11_map	11
#define Gpio_11_type	GPIO_INPUT
  
//GPIO 12
#define Gpio_12_map	12
#define Gpio_12_type	GPIO_INPUT
  
//GPIO 13
#define Gpio_13_map	13
#define Gpio_13_type	GPIO_INPUT

//GPIO 14
#define Gpio_14_map	14
#define Gpio_14_type	GPIO_INPUT
  
//GPIO 15
#define Gpio_15_map	15  
#define Gpio_15_type	GPIO_INPUT


#endif// GPIOMANAGER_H_

