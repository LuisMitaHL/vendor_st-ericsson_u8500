/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef SDL_IP_INTERFACE_H_
#define SDL_IP_INTERFACE_H_

/**
 \file      SDL_ip_interface.h

 \brief     This file is NOT a part of the module release code.
            All inputs needed by the scalar module that are paltform
            dependent (like the maximum number of scalar coefficients)
            are met through this file.
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

#include "SDL_PlatformSpecific.h"
#include "Stream.h"
#include "PictorhwReg.h"
#include "EventManager.h"


/// Interface to let the module know if the device is streaming
#define SDL_IsStreaming()   Stream_IsISPInputStreaming()

/// Interface to program the last GIR FlexTF element for SDL
#define SDL_ProgramLastGIR(x)               Set_ISP_FLEXTF_LINEAR_ISP_FLEXTF_LINEAR_LAST_ELT_GREEN_GIR(x)

/// Interface to program the last Red FlexTF element for SDL
#define SDL_ProgramLastRed(x)               Set_ISP_FLEXTF_LINEAR_ISP_FLEXTF_LINEAR_LAST_ELT_RED(x)

/// Interface to program the last Blue FlexTF element for SDL
#define SDL_ProgramLastBlue(x)              Set_ISP_FLEXTF_LINEAR_ISP_FLEXTF_LINEAR_LAST_ELT_BLUE(x)

/// Interface to program the last GIB FlexTF element for SDL
#define SDL_ProgramLastGIB(x)               Set_ISP_FLEXTF_LINEAR_ISP_FLEXTF_LINEAR_LAST_ELT_GIB(x)

/// Interface to program the Pixel Shift
#define SDL_ProgramPixelShift(x)            Set_ISP_FLEXTF_LINEAR_ISP_FLEXTF_LINEAR_PIXELIN_SHIFT_word(x)

/// Interface to enable the FlexTF hardware
//  enable,soft_reset
#define SDL_EnableHW()                      Set_ISP_FLEXTF_LINEAR_ISP_FLEXTF_LINEAR_ENABLE(enable_ENABLE, soft_reset_B_0x0)

/// Interface to disable the FlexTF hardware
//  enable,soft_reset
#define SDL_DisableHW()                     Set_ISP_FLEXTF_LINEAR_ISP_FLEXTF_LINEAR_ENABLE(enable_DISABLE, soft_reset_B_0x0)

#endif /* SDL_IP_INTERFACE_H_ */

