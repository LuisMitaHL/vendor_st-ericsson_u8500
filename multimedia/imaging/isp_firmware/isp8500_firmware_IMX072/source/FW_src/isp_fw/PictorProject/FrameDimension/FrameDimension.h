/* 
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved. 
 * This code is ST-Ericsson proprietary and confidential. 
 * Any use of the code for whatever purpose is subject to 
 * specific written permission of ST-Ericsson SA. 
 */ 
 
/**
\defgroup FrameDimension Frame Dimension Module
\details Frame dimension manager provides, to the top level firmware
modules, a simplified view of sensor frame dimension management.
It also provides support for scaling and sub-sampling in the
sensor.
*/

/**
\if INCLUDE_IN_HTML_ONLY
\file FrameDimension.h
\brief declare data structure and functions used in frame dimension module
\ingroup Frame Dimension
\endif
*/
#ifndef FRAMEDIMENSION_H_
#   define FRAMEDIMENSION_H_

#   include "Platform.h"
#   include "FrameDimension_ip_interface.h"
#   include "FrameDimension_op_interface.h"

/**
\enum 		SensorProfile_te
\brief 	Specifies the sensor profile
\ingroup 	Frame Dimension
*/
typedef enum
{
    /// SMIA profile 0 sensor
    SensorProfile_e_Profile0,

    /// SMIA profile 1 sensor
    SensorProfile_e_Profile1,

    /// SMIA profile 2 sensor
    SensorProfile_e_Profile2
} SensorProfile_te;

/**
\enum SensorPrescaleType_te
\brief Sensor prescale types: Scaling or Subsampling
\ingroup Frame Dimension
*/
typedef enum
{
    /// Sensor Subsample
    SensorPrescaleType_e_SensorSubsample,

    /// Sensor scaling
    SensorPrescaleType_e_SensorScale,
} SensorPrescaleType_te;

/**
\enum FrameDimensionMode_te
\brief Current frame dimension values can either be calculated automatically
by the firmware or entered by the Host.
\ingroup Frame Dimension
*/
typedef enum
{
    /// The host will enter the current frame dimension values.
    FrameDimensionProgMode_e_Manual,

    /// Firmware will calculate the current frame dimension values.
    FrameDimensionProgMode_e_Auto
} FrameDimensionProgMode_te;

/**
\enum PixelCode_te
\brief The pixel code defines the type of pixel data, embedded, dummy, black, dark, visible or manufacturer specific.
\ingroup Frame Dimension
*/
typedef enum
{
    PixelCode_e_Illegal0,
    PixelCode_e_EmbeddedData,
    PixelCode_e_DummyPixelData,
    PixelCode_e_BlackPixelData,
    PixelCode_e_DarkPixelData,
    PixelCode_e_VisiblePixelData,
    PixelCode_e_Reserved6,
    PixelCode_e_Reserved7,
    PixelCode_e_ManufacturerSpecificPixelType0,
    PixelCode_e_ManufacturerSpecificPixelType1,
    PixelCode_e_ManufacturerSpecificPixelType2,
    PixelCode_e_ManufacturerSpecificPixelType3,
    PixelCode_e_ManufacturerSpecificPixelType4,
    PixelCode_e_ManufacturerSpecificPixelType5,
    PixelCode_e_ManufacturerSpecificPixelType6,
    PixelCode_e_Illegal15
} PixelCode_te;


#   define FrameDimension_Ceiling(f_Value) GenericFunctions_Ceiling(f_Value)

/// Interface to get the number of cloumns lost in the ISP
#   define FrameDimension_GetISPColumnLoss() \
        (COLUMNS_LOST_AT_LEFT_EDGE_DUE_TO_COLOUR_INTERPOLATION + COLUMNS_LOST_AT_RIGHT_EDGE_DUE_TO_COLOUR_INTERPOLATION)

/// Interface to get the number of rows lost in the ISP
#   define FrameDimension_GetISPRowLoss() \
        (ROWS_LOST_AT_TOP_EDGE_DUE_TO_COLOUR_INTERPOLATION + ROWS_LOST_AT_BOTTOM_EDGE_DUE_TO_COLOUR_INTERPOLATION)

#   define SYNC_CODE_WIDTH_BITS    32
#   define CHECKSUM_WIDTH_BITS     32
#endif /*FRAMEDIMENSION_H_*/

