/* 
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved. 
 * This code is ST-Ericsson proprietary and confidential. 
 * Any use of the code for whatever purpose is subject to 
 * specific written permission of ST-Ericsson SA. 
 */ 
 
/**
 \file      HostCommsPlatformSpecific.h
 \brief     This file is NOT a part of the module release code.
            All inputs needed by the host comms module that are paltform
            dependent (like the total number of interrupts instantiated
            in the core) are met through this file.
            It is the responsibility of the integrator to generate
            this file at integration time and meet all the platform
            specific dependencies.
            This file must be present in incldue path of the
            integrated project during compilation of code.

 \note      The following sections define the dependencies that must be
            met by the system integrator. The way these dependencies
            have been resolved here are just for example. These dependencies
            must be appropriately resolved based on the platform being used.

 \ingroup HostComms
*/
#ifndef HOSTCOMMSPLATFORMSPECIFIC_
#   define HOSTCOMMSPLATFORMSPECIFIC_

#   include "Platform.h"

/// Specifies the number of bits used for encoding
/// the page number. The number of pages that the
/// address encoding scheme will support will be
/// 2^HOST_COMMS_PAGE_NUMBER_WIDTH
#   define HOST_COMMS_PAGE_NUMBER_WIDTH    9

/// Specifies the number of bits used for encoding
/// the byte offset. This parameter will signify
/// the maximum size of a page which will be
/// 2^HOST_COMMS_PAGE_OFFSET_WIDTH
#   define HOST_COMMS_PAGE_OFFSET_WIDTH    6
#endif /* HOSTCOMMSPLATFORMSPECIFIC_ */

