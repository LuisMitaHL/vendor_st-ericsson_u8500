/* 
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved. 
 * This code is ST-Ericsson proprietary and confidential. 
 * Any use of the code for whatever purpose is subject to 
 * specific written permission of ST-Ericsson SA. 
 */ 
 
/**
 \file FrameRate_ip_interface.h
 \brief This file is NOT a part of the module release code.
        All inputs needed by the Frame Rate module that can only
        be resolved at the project level (at integration time)
        are met through this file.
        It is the responsibility of the integrator to generate
        this file at integration time and meet all the input
        dependencies.

 \note  The following sections define the dependencies that must be
           met by the system integrator. The way these dependencies
           have been resolved here are just for example. These dependencies
           must be appropriately resolved based on the platform being used.
 \ingroup FrameRate
*/
#ifndef _FRAME_RATE_IP_INTERFACE_H_
#   define _FRAME_RATE_IP_INTERFACE_H_
#   include "PictorhwReg.h"
#   include "FrameRate_PlatformSpecific.h"
#   include "FrameDimension_op_interface.h"
#endif // _FRAME_RATE_IP_INTERFACE_H_

