/* 
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved. 
 * This code is ST-Ericsson proprietary and confidential. 
 * Any use of the code for whatever purpose is subject to 
 * specific written permission of ST-Ericsson SA. 
 */ 
 
/**
 \file ColourMatrix_PlatformSpecific.h
 \brief This file is NOT a part of the module release code.
        All inputs needed by the colour matrix module that can only
        be resolved at the project level (at integration time)
        are met through this file.
        It is the responsibility of the integrator to generate
        this file at integration time and meet all the input
        dependencies.

 \details The file specify default values for control and status page elements. The values are
          device specific.

 \note      The following sections define the dependencies that must be
           met by the system integrator. The way these dependencies
           have been resolved here are just for example. These dependencies
           must be appropriately resolved based on the platform being used.
 \ingroup ColourMatrix
*/
#ifndef _COLOUR_MATRIX_PLATFORMSPECIFIC_H_
#   define _COLOUR_MATRIX_PLATFORMSPECIFIC_H_

/**
    Default values are device specific
*/

/// As Pipe is 4.10 format so 1.0 will be 1024
#   define COLOURMATRIX_DEFAULT_VALUE_UNITY    (1024)

/// Default value to be programmed in the PIPE
    /// As Pipe is 4.10 format so 0.0 will be 0
#   define COLOURMATRIX_DEFAULT_VALUE_ZERO (0)
#endif // _COLOUR_MATRIX_PLATFORMSPECIFIC_H_

