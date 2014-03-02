/* 
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved. 
 * This code is ST-Ericsson proprietary and confidential. 
 * Any use of the code for whatever purpose is subject to 
 * specific written permission of ST-Ericsson SA. 
 */ 
 
/**
 \file whitebalance_PlatformSpecific.h
 \brief Specify default values for the platform. The file is not part of release but
        dependencies should be resolved at integration level.
 \details
 \ingroup WhiteBalance
*/
#ifndef _WHITEBALANCE_PLATFORMSPECIFIC_H_
#   define _WHITEBALANCE_PLATFORMSPECIFIC_H_

#   include "whitebalance_opinterface.h"
#   include "Platform.h"

/// Default value of manual red gain
#define WHITEBALANCE_DEFAULT_MANUAL_RED_GAIN		(1.0)

/// Default value of manual green gain
#define WHITEBALANCE_DEFAULT_MANUAL_GREEN_GAIN		(1.0)

/// Default value of manual blue gain
#define WHITEBALANCE_DEFAULT_MANUAL_BLUE_GAIN		(1.0)

#endif //_WHITEBALANCE_PLATFORMSPECIFIC_H_

