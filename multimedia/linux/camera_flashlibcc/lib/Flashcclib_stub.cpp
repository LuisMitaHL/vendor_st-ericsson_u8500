/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "wrapper.h"

/*
 * Initiate the libary. Read in static parameters from the system.
 * Needs to be called only once at startup time
 *
 * Returns 0 on success, otherwise -1.
*/
uint8_t FlashLED_Init(void)
{
return 0;
}

/*
 * Get the maximum current that shall be used by the chip for Flash,
 * including Calculation.
 * pMaxFlashCurrent = maximum current used, in milliamperes (mA)
 * All calculations done in the context of this function to be used
 * for still main flash
*/
void FlashLED_GetFlashCurrentLevel(uint16_t *const pMaxFlashCurrent)
{
*pMaxFlashCurrent = 100;
}

/*
 * Get the maximum current that shall be used by the chip for AutoFocus.
 * pMaxAutoFocusCurrent - maximum current used, in milliamperes (mA)
 * To be used for AF assistant UC
*/
void FlashLED_GetAutoFocusCurrentLevel(uint16_t *const pMaxAutoFocusCurrent)
{
*pMaxAutoFocusCurrent = 87;
}

/*
 * Get the maximum current that shall be used by the chip for Torch.
 * pMaxTorchCurrent - maximum current used, in milliamperes (mA)
 * To be used for:
 *	 Video UC
 *	 Pre-flash
 *	 Torch UC
*/
void FlashLED_GetTorchCurrentLevel(uint16_t *const pMaxTorchCurrent)
{
*pMaxTorchCurrent = 88;
}

/*
 * Get the color temperature. Production trimed.
 * pFlashChromaPointX - the color temperature in Chromaticity Points,
 *			this is the X Co-ordinate.
 * pFlashChromaPointY - the color temperature in Chromaticity Points,
 *			this is the Y Co-ordinate.
 * Needs to be read only at startup
*/
void FlashLED_GetColorTemperature(float *const pFlashChromaPointX,
				  float *const pFlashChromaPointY)
{
*pFlashChromaPointX = 11.0;
*pFlashChromaPointY = 12.0;
}

