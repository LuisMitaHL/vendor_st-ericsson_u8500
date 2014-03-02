/* 
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved. 
 * This code is ST-Ericsson proprietary and confidential. 
 * Any use of the code for whatever purpose is subject to 
 * specific written permission of ST-Ericsson SA. 
 */ 
 
/*
\file Adsoc_ip_interface.h
 */

#ifndef ADSOC_IP_INTERFACE_H_
#define ADSOC_IP_INTERFACE_H_

#include "Platform.h"
#include "PictorhwReg.h"
#include "Adsoc_platform_specific.h"
#include "Damper.h"
#include "FrameDimension_op_interface.h"

/// No of Pipes in ISP
#define ADSOC_NO_OF_HARDWARE_PIPE_IN_ISP 			NO_OF_HARDWARE_PIPE_IN_ISP

/// specifies the damper num for Adsoc hw block

/// ip interface to enable/disable the adsoc block
#define Set_Pipe0_ADSOC_PK_ENABLE(adsoc_enable)                Set_ISP_CE0_RADIAL_ADSOC_PK_ISP_CE0_RADIAL_ADSOC_PK_ENABLE(adsoc_enable)

/// ip interface to enable/disable adaptive sharpness
#define Set_Pipe0_ADSOC_PK_ADSHARP_EN(adsoc_adsharp_en)        Set_ISP_CE0_RADIAL_ADSOC_PK_ISP_CE0_RADIAL_ADSOC_PK_ADSHARP_EN(adsoc_adsharp_en)

/// ip interface to set adsoc coring level
#define Set_Pipe0_ADSOC_PK_CORING_LEVEL(coring_level)          Set_ISP_CE0_RADIAL_ADSOC_PK_ISP_CE0_RADIAL_ADSOC_PK_CORING_LEVEL(coring_level)

/// ip interface to set adsoc overshoot bright/dark gain ctrl
#define Set_Pipe0_ADSOC_PK_OSHOOT_CTRL(gain_bright, gain_dark) Set_ISP_CE0_RADIAL_ADSOC_PK_ISP_CE0_RADIAL_ADSOC_PK_OSHOOT_CTRL(gain_bright, gain_dark)

/// ip interface to t adsoc effects (Emboss, Flipper, GrayBack)
#define Set_Pipe0_ADSOC_PK_EFFECTS(emboss, flipper, grayback)  Set_ISP_CE0_RADIAL_ADSOC_PK_ISP_CE0_RADIAL_ADSOC_PK_EFFECTS(emboss, flipper, grayback)

/// ip interface to set adsoc gain
#define Set_Pipe0_ADSOC_PK_GAIN(adsoc_gain)                    Set_ISP_CE0_RADIAL_ADSOC_PK_ISP_CE0_RADIAL_ADSOC_PK_GAIN(adsoc_gain)


/// ip interface to enable/disable the adsoc block
#define Set_Pipe1_ADSOC_PK_ENABLE(adsoc_enable)                Set_ISP_CE1_RADIAL_ADSOC_PK_ISP_CE1_RADIAL_ADSOC_PK_ENABLE(adsoc_enable)

/// ip interface to enable/disable adaptive sharpness
#define Set_Pipe1_ADSOC_PK_ADSHARP_EN(adsoc_adsharp_en)        Set_ISP_CE1_RADIAL_ADSOC_PK_ISP_CE1_RADIAL_ADSOC_PK_ADSHARP_EN(adsoc_adsharp_en)

/// ip interface to set adsoc coring level
#define Set_Pipe1_ADSOC_PK_CORING_LEVEL(coring_level)          Set_ISP_CE1_RADIAL_ADSOC_PK_ISP_CE1_RADIAL_ADSOC_PK_CORING_LEVEL(coring_level)

/// ip interface to set adsoc overshoot bright/dark gain ctrl
#define Set_Pipe1_ADSOC_PK_OSHOOT_CTRL(gain_bright, gain_dark) Set_ISP_CE1_RADIAL_ADSOC_PK_ISP_CE1_RADIAL_ADSOC_PK_OSHOOT_CTRL(gain_bright, gain_dark)

/// ip interface to set adsoc effects (Emboss, Flipper, GrayBack)
#define Set_Pipe1_ADSOC_PK_EFFECTS(emboss, flipper, grayback)  Set_ISP_CE1_RADIAL_ADSOC_PK_ISP_CE1_RADIAL_ADSOC_PK_EFFECTS(emboss, flipper, grayback)

/// ip interface to set adsoc gain
#define Set_Pipe1_ADSOC_PK_GAIN(adsoc_gain)                    Set_ISP_CE1_RADIAL_ADSOC_PK_ISP_CE1_RADIAL_ADSOC_PK_GAIN(adsoc_gain)


/// set adsoc radial peaking H-Offset
#define Set_Pipe0_ADSOC_RP_LENS_CENTRE_HOFFSET(h_offset)   Set_ISP_CE0_RADIAL_ADSOC_RP_ISP_CE0_RADIAL_ADSOC_RP_HOFFSET(h_offset)

 /// set adsoc radial peaking V -Offset
#define Set_Pipe0_ADSOC_RP_LENS_CENTRE_VOFFSET(v_offset)    Set_ISP_CE0_RADIAL_ADSOC_RP_ISP_CE0_RADIAL_ADSOC_RP_VOFFSET(v_offset)

/// set adsoc radial peaking X scale value
#define Set_Pipe0_ADSOC_RP_SCALE_X(x_scale)          Set_ISP_CE0_RADIAL_ADSOC_RP_ISP_CE0_RADIAL_ADSOC_RP_SCALE_X(x_scale)

/// set adsoc radial peaking Y scale value
#define Set_Pipe0_ADSOC_RP_SCALE_Y(y_scale)          Set_ISP_CE0_RADIAL_ADSOC_RP_ISP_CE0_RADIAL_ADSOC_RP_SCALE_Y(y_scale)

/// set adsoc radial peaking polycoeff_0
#define Set_Pipe0_ADSOC_RP_POLYCOEF0(polycoef0)   Set_ISP_CE0_RADIAL_ADSOC_RP_ISP_CE0_RADIAL_ADSOC_RP_POLYCOEF0(polycoef0)

/// set adsoc radial peaking polycoeff_1
#define Set_Pipe0_ADSOC_RP_POLYCOEF1(polycoef1)    Set_ISP_CE0_RADIAL_ADSOC_RP_ISP_CE0_RADIAL_ADSOC_RP_POLYCOEF1(polycoef1)

/// set adsoc radial peaking COF shift
#define Set_Pipe0_ADSOC_RP_COF_SHIFT(cof_shift)      Set_ISP_CE0_RADIAL_ADSOC_RP_ISP_CE0_RADIAL_ADSOC_RP_COF_SHIFT(cof_shift)

/// set adsoc radial peaking COF shift out
#define Set_Pipe0_ADSOC_RP_COF_OUT_SHIFT(cof_out_shift)    Set_ISP_CE0_RADIAL_ADSOC_RP_ISP_CE0_RADIAL_ADSOC_RP_OUT_SHIFT(cof_out_shift)

/// set adsoc radial peaking RP unity
#define Set_Pipe0_ADSOC_RP_UNITY(unity)    Set_ISP_CE0_RADIAL_ADSOC_RP_ISP_CE0_RADIAL_ADSOC_RP_UNITY(unity)

/// Enable/disable Radial adsoc hw block
#define Set_Pipe0_ADSOC_RP_ENABLE(enable_flag)         Set_ISP_CE0_RADIAL_ADSOC_RP_ISP_CE0_RADIAL_ADSOC_RP_ENABLE(enable_flag)

/// set adsoc radial peaking H-Offset
#define Set_Pipe1_ADSOC_RP_LENS_CENTRE_HOFFSET(h_offset)   Set_ISP_CE1_RADIAL_ADSOC_RP_ISP_CE1_RADIAL_ADSOC_RP_HOFFSET(h_offset)

 /// set adsoc radial peaking V -Offset
#define Set_Pipe1_ADSOC_RP_LENS_CENTRE_VOFFSET(v_offset)    Set_ISP_CE1_RADIAL_ADSOC_RP_ISP_CE1_RADIAL_ADSOC_RP_VOFFSET(v_offset)

/// set adsoc radial peaking X scale value
#define Set_Pipe1_ADSOC_RP_SCALE_X(x_scale)          Set_ISP_CE1_RADIAL_ADSOC_RP_ISP_CE1_RADIAL_ADSOC_RP_SCALE_X(x_scale)

/// set adsoc radial peaking Y scale value
#define Set_Pipe1_ADSOC_RP_SCALE_Y(y_scale)          Set_ISP_CE1_RADIAL_ADSOC_RP_ISP_CE1_RADIAL_ADSOC_RP_SCALE_Y(y_scale)

/// set adsoc radial peaking polycoeff_0
#define Set_Pipe1_ADSOC_RP_POLYCOEF0(polycoef0)   Set_ISP_CE1_RADIAL_ADSOC_RP_ISP_CE1_RADIAL_ADSOC_RP_POLYCOEF0(polycoef0)

/// set adsoc radial peaking polycoeff_1
#define Set_Pipe1_ADSOC_RP_POLYCOEF1(polycoef1)    Set_ISP_CE1_RADIAL_ADSOC_RP_ISP_CE1_RADIAL_ADSOC_RP_POLYCOEF1(polycoef1)

/// set adsoc radial peaking COF shift
#define Set_Pipe1_ADSOC_RP_COF_SHIFT(cof_shift)      Set_ISP_CE1_RADIAL_ADSOC_RP_ISP_CE1_RADIAL_ADSOC_RP_COF_SHIFT(cof_shift)

/// set adsoc radial peaking COF shift out
#define Set_Pipe1_ADSOC_RP_COF_OUT_SHIFT(cof_out_shift)    Set_ISP_CE1_RADIAL_ADSOC_RP_ISP_CE1_RADIAL_ADSOC_RP_OUT_SHIFT(cof_out_shift)

/// set adsoc radial peaking RP unity
#define Set_Pipe1_ADSOC_RP_UNITY(unity)    Set_ISP_CE1_RADIAL_ADSOC_RP_ISP_CE1_RADIAL_ADSOC_RP_UNITY(unity)

/// Enable/disable Radial adsoc hw block
#define Set_Pipe1_ADSOC_RP_ENABLE(enable_flag)         Set_ISP_CE1_RADIAL_ADSOC_RP_ISP_CE1_RADIAL_ADSOC_RP_ENABLE(enable_flag)

/// Interface to Get CR_HSTART in Sensor
#define Get_ADSOC_RP_SENSOR_HSTART()             FrameDimension_GetCurrentVTXAddrStart()

/// Interface to Get CR_VSTART in Sensor
#define Get_ADSOC_RP_SENSOR_VSTART()                FrameDimension_GetCurrentVTYAddrStart()

/// Interface to Get X-Scale in Sensor
#   define Get_ADSOC_RP_SCALE_X() FrameDimension_GetCurrentPreScaleFactor()

/// Interface to Get Y-Scale in Sensor
#   define Get_ADSOC_RP_SCALE_Y() FrameDimension_GetCurrentPreScaleFactor()

#endif /* ADSOC_IP_INTERFACE_H_ */
