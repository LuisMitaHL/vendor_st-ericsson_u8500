/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/**
* \file    wrapper_api.h
* \brief   
* \author  ST-Ericsson
*
*/
#ifndef __WRAPPER_API_H__
#define __WRAPPER_API_H__

#include <stdint.h>

/*
 * Initiate the libary. Read in static parameters from the system.
 * Needs to be called only once at startup time
 *
 * Returns 0 on success, otherwise -1.
*/
uint8_t FlashLED_Init(void);

/*
 * Get the maximum current that shall be used by the chip for Flash,
 * including Calculation.
 * pMaxFlashCurrent = maximum current used, in milliamperes (mA)
 * All calculations done in the context of this function to be used
 * for still main flash
*/
void FlashLED_GetFlashCurrentLevel(uint16_t *const pMaxFlashCurrent);

/*
 * Get the maximum current that shall be used by the chip for AutoFocus.
 * pMaxAutoFocusCurrent - maximum current used, in milliamperes (mA)
 * To be used for AF assistant UC
*/
void FlashLED_GetAutoFocusCurrentLevel(uint16_t *const pMaxAutoFocusCurrent);

/*
 * Get the maximum current that shall be used by the chip for Torch.
 * pMaxTorchCurrent - maximum current used, in milliamperes (mA)
 * To be used for:
 *	 Video UC
 *	 Pre-flash
 *	 Torch UC
*/
void FlashLED_GetTorchCurrentLevel(uint16_t *const pMaxTorchCurrent);

/*
 * Get the color temperature. Production trimed.
 * pFlashChromaPointX - the color temperature in Chromaticity Points,
 *			this is the X Co-ordinate.
 * pFlashChromaPointY - the color temperature in Chromaticity Points,
 *			this is the Y Co-ordinate.
 * Needs to be read only at startup
*/
void FlashLED_GetColorTemperature(float *const pFlashChromaPointX,
				  float *const pFlashChromaPointY);

#endif /* __WRAPPER_API_H__ */
