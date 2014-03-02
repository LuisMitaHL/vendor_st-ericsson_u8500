/* 
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved. 
 * This code is ST-Ericsson proprietary and confidential. 
 * Any use of the code for whatever purpose is subject to 
 * specific written permission of ST-Ericsson SA. 
 */ 
 
#ifndef SDL_PLATFORMSPECIFIC_H_
#define SDL_PLATFORMSPECIFIC_H_

/**
 \file      sdl_PlatformSpecific.h

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

 \ingroup   SDL
*/
#include "Platform.h"
#include "SDL_op_interface.h"

/// Specifies the base address of the SDL GIR FlexTF memories in the STXP70 address space
#define SDL_GIR_MEM_BASE    (0x90000000 + 0x2DC04 - 0x4000) // the 0x4000 corresponds to the "base address" of the offset 0x24404 in T1 domain

/// Specifies the base address of the SDL Red FlexTF memories in the STXP70 address space
#define SDL_RED_MEM_BASE    (0x90000000 + 0x2E004 - 0x4000) // the 0x4000 corresponds to the "base address" of the offset 0x24404 in T1 domain

/// Specifies the base address of the SDL Blue FlexTF memories in the STXP70 address space
#define SDL_BLUE_MEM_BASE   (0x90000000 + 0x2E404 - 0x4000) // the 0x4000 corresponds to the "base address" of the offset 0x24404 in T1 domain

/// Specifies the base address of the SDL GIB FlexTF memories in the STXP70 address space
#define SDL_GIB_MEM_BASE    (0x90000000 + 0x2E804 - 0x4000) // the 0x4000 corresponds to the "base address" of the offset 0x24404 in T1 domain

/// Specifies the width of the input pixel in the sensor data linearization FlexTF block
#define SDL_INPUT_PIXEL_WIDTH               (12)

/// Specifies the number of bits required to encode depth of the SDL FlexTF LUT
/// e.g. if there are 256 entries in the LUT, then this value should be 8 (2^8 == 256)
#define SDL_MEM_LUT_DEPTH_LOG_BASE_2        (8)


/// DEFAULT_ISP_FLEXTF_LINEAR_LAST_ELT: default value of last look up entry. The value is programmed in register, not LUT memory
#define DEFAULT_ISP_FLEXTF_LINEAR_LAST_ELT  (256*(1 << (SDL_INPUT_PIXEL_WIDTH - SDL_MEM_LUT_DEPTH_LOG_BASE_2)))

/// ISP_FLEXTF_LINEAR_PIXELIN_SHIFT: default pixel in shift value
#define DEFAULT_ISP_FLEXTF_LINEAR_PIXEL_SHIFT  (0)

/// Specifies the default value of SDL mode control
#define SDL_CONTROL_DEFAULT_MODE_CONTROL    (SDL_Disable)

/// Specifies the default value of SDL mode status
#define SDL_STATUS_DEFAULT_MODE_STATUS      (SDL_Disable)

/// Specifies the default value of SDL update pending status
#define SDL_STATUS_DEFAULT_UPDATE_PENDING   (Flag_e_FALSE)

#endif /*SDL_PLATFORMSPECIFIC_H_*/

