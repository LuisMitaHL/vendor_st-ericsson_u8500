/* 
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved. 
 * This code is ST-Ericsson proprietary and confidential. 
 * Any use of the code for whatever purpose is subject to 
 * specific written permission of ST-Ericsson SA. 
 */ 
 
#ifndef GAMMA_PLATFORMSPECIFIC_H_
#define GAMMA_PLATFORMSPECIFIC_H_

/**
 \file      Gamma_PlatformSpecific.h

 \brief     This file is NOT a part of the module release code.
            All inputs needed by the Gamma module that are platform
            dependent are met through this file.
            It is the responsibility of the integrator to generate
            this file at integration time and meet all the platform
            specific dependencies.
            This file must be present in incldue path of the
            integrated project during compilation of code.

 \note      The following sections define the dependencies that must be
            met by the system integrator. The way these dependencies
            have been resolved here are just for example. These dependencies
            must be appropriately resolved based on the platform being used.

 \ingroup   Gamma
*/
#include "Platform.h"

#define NO_OF_LUTBINS                                          128

/// Gamma Lut Table
const   uint16_t Gamma_LutTable[NO_OF_LUTBINS] =            { 0,113,155,186,212,235,255,274,291,307,322,336,350,363,375,387,399,410,421,431,442,451,461,471,480,489,497,506,515,523,531,539,547,554,562,569,577,584,591,598,605,612,619,625,632,638,645,651,657,664,670,676,682,688,694,699,705,711,716,722,728,733,738,744,749,754,760,765,770,775,780,785,790,795,800,805,810,815,820,824,829,834,839,843,848,852,857,861,866,870,875,879,884,888,892,897,901,905,909,913,918,922,926,930,934,938,942,946,950,954,958,962,966,970,974,978,982,986,989,993,997,1001,1004,1008,1012,1016,1019,1023 };

/// Specifies the base address of the Gamma memories in the STXP70 address space
/// Ce0Sharp Green Memory Base Address
#define Gamma_GetCe0Sharp_GIRAddr()             ((0x90000000 + 0x2F004 - 0x4000))
/// Ce0Sharp Red Memory Base Address
#define Gamma_GetCe0Sharp_RedAddr()             ((0x90000000 + 0x2F004 - 0x4000) + 0x200)
/// Ce0Sharp Blue Memory Base Address
#define Gamma_GetCe0Sharp_BlueAddr()            ((0x90000000 + 0x2F004 - 0x4000) + 0x400)

/// Ce0UnSharp Green Memory Base Address
#define Gamma_GetCe0UnSharp_GIRAddr()           ((0x90000000 + 0x2FC04 - 0x4000))
/// Ce0UnSharp Red Memory Base Address
#define Gamma_GetCe0UnSharp_RedAddr()           ((0x90000000 + 0x2FC04 - 0x4000) + 0x200)
/// Ce0UnSharp Blue Memory Base Address
#define Gamma_GetCe0UnSharp_BlueAddr()          ((0x90000000 + 0x2FC04 - 0x4000) + 0x400)

/// Ce1Sharp Green Memory Base Address
#define Gamma_GetCe1Sharp_GIRAddr()             ((0x90000000 + 0x30804 - 0x4000))
/// Ce1Sharp Red Memory Base Address
#define Gamma_GetCe1Sharp_RedAddr()             ((0x90000000 + 0x30804 - 0x4000) + 0x200)
/// Ce1Sharp Blue Memory Base Address
#define Gamma_GetCe1Sharp_BlueAddr()            ((0x90000000 + 0x30804 - 0x4000) + 0x400)

/// Ce1UnSharp Green Memory Base Address
#define Gamma_GetCe1UnSharp_GIRAddr()           ((0x90000000 + 0x31404 - 0x4000))
/// Ce1UnSharp Red Memory Base Address
#define Gamma_GetCe1UnSharp_RedAddr()           ((0x90000000 + 0x31404 - 0x4000) + 0x200)
/// Ce1UnSharp Blue Memory Base Address
#define Gamma_GetCe1UnSharp_BlueAddr()          ((0x90000000 + 0x31404 - 0x4000) + 0x400)

/// Pipe count
#define GAMMA_PIPE_COUNT                        NO_OF_HARDWARE_PIPE_IN_ISP

/// Value for Ce0 Gamma Curve
#define Gamma_Ce0GammaCurve                     (GammaCurve_Disable)
/// Value for Ce0Sharp Last GreenGIR Pixel
#define Gamma_Ce0GammaSharp_LstGreenGIRValue    (1023)
/// Value for Ce0Sharp Last Red Pixel
#define Gamma_Ce0GammaSharp_LstRedValue         (1023)
/// Value for Ce0Sharp Last Blue Pixel
#define Gamma_Ce0GammaSharp_LstBlueValue        (1023)
/// Value for Ce0Sharp Last GreenGIB Pixel
#define Gamma_Ce0GammaSharp_LstGreenGIBValue    (1023)
/// Value for Ce0Sharp PixelInShift
#define Gamma_Ce0GammaSharp_PixelInShiftValue   (0)

/// Value for Ce0UnSharp Last GreenGIR Pixel
#define Gamma_Ce0GammaUnSharp_LstGreenGIRValue  (1023)
/// Value for Ce0UnSharp Last Red Pixel
#define Gamma_Ce0GammaUnSharp_LstRedValue       (1023)
/// Value for Ce0UnSharp Last Blue Pixel
#define Gamma_Ce0GammaUnSharp_LstBlueValue      (1023)
/// Value for Ce0UnSharp Last GreenGIB Pixel
#define Gamma_Ce0GammaUnSharp_LstGreenGIBValue  (1023)
/// Value for Ce0UnSharp PixelInShift
#define Gamma_Ce0GammaUnSharp_PixelInShiftValue  (0)

/// Value for Ce1 Gamma Curve
#define Gamma_Ce1GammaCurve                     (GammaCurve_Disable)
/// Value for Ce1Sharp Last GreenGIR Pixel
#define Gamma_Ce1GammaSharp_LstGreenGIRValue    (1023)
/// Value for Ce1Sharp Last Red Pixel
#define Gamma_Ce1GammaSharp_LstRedValue         (1023)
/// Value for Ce1Sharp Last Blue Pixel
#define Gamma_Ce1GammaSharp_LstBlueValue        (1023)
/// Value for Ce1Sharp Last GreenGIB Pixel
#define Gamma_Ce1GammaSharp_LstGreenGIBValue    (1023)
/// Value for Ce1Sharp PixelInShift
#define Gamma_Ce1GammaSharp_PixelInShiftValue   (0)

/// Value for Ce1UnSharp Last GreenGIR Pixel
#define Gamma_Ce1GammaUnSharp_LstGreenGIRValue  (1023)
/// Value for Ce1UnSharp Last Red Pixel
#define Gamma_Ce1GammaUnSharp_LstRedValue       (1023)
/// Value for Ce1UnSharp Last Blue Pixel
#define Gamma_Ce1GammaUnSharp_LstBlueValue      (1023)
/// Value for Ce1UnSharp Last GreenGIB Pixel
#define Gamma_Ce1GammaUnSharp_LstGreenGIBValue  (1023)
/// Value for Ce1UnSharp PixelInShift
#define Gamma_Ce1GammaUnSharp_PixelInShiftValue (0)

#endif /*GAMMA_PLATFORMSPECIFIC_H_*/

