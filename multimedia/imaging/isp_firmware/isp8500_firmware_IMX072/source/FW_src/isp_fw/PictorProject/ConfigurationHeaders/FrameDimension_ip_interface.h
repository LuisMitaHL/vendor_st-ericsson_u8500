/* 
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved. 
 * This code is ST-Ericsson proprietary and confidential. 
 * Any use of the code for whatever purpose is subject to 
 * specific written permission of ST-Ericsson SA. 
 */ 
 
/**
 \file FrameDimension_ip_interface.h
 \brief This file is NOT a part of the module release code.
        All inputs needed by the frame dimension module that can only
        be resolved at the project level (at integration time) are met
        through this file.
        It is the responsibility of the integrator to generate
        this file at integration time and meet all the input
        dependencies

 \ingroup Frame Dimension
*/
#ifndef FRAMEDIMENSION_IP_INTERFACE_H_
#   define FRAMEDIMENSION_IP_INTERFACE_H_

#   include "Platform.h"
#   include "videotiming_op_interface.h"
#   include "Zoom_OPInterface.h"
#   include "SystemConfig.h"
#   include "MissingModules.h"
#   include "GenericFunctions.h"
#   include "ExpSensor.h"
#   include "HostInterface.h"
#   include "smia_sensor_memorymap_defs.h"
#   include "FrameRate_op_interface.h"
#   include "lla_abstraction.h"
#   include "FrameRate_op_interface.h"

/// It is vital for the frame dimension manager to know whether the sensor
/// is streaming in order to ensure, at that point of time, whether the current
/// coarse exposure reported by the receiver bank register and the applied coarse
/// exposure reported by the exposure block are small enough to be accomodated in-
/// side the new frame length.




/// CCP raw format
#   define FD_GET_CCP_RAW_FORMAT() VideoTiming_GetCsiRawFormat()

/// Get sensor bits per system clock


#   define FD_CEILING_OF_M_BY_N(x, y)                          GenericFunctions_CeilingOfMByN(x, y)

/// Interface to provide the ceiling of a float value


/// Interface to get the minimum of two values
#   define FrameDimension_Min(a, b)    min(a, b)

/// Interface to get the maximum of two values
#   define FrameDimension_Max(a, b)    max(a, b)

/// Interface to clip a value between max and min values
#   define FrameDimension_Clip(value, min, max)    clip(value, min, max)






#endif /*FRAMEDIMENSION_IP_INTERFACE_H_*/

