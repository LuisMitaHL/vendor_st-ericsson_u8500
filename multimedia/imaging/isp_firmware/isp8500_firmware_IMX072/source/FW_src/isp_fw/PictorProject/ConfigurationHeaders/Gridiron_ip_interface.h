/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 \file 	Gridiron_ip_interface.h

 \brief This file is NOT a part of the Gridiron module release code.
 		All inputs needed by the Gridiron module that can only
	 	be resolved at the project level (at integration time)
	 	are met through this file.
	 	It is the responsibility of the integrator to generate
	 	this file at integration time and meet all the input
	 	dependencies.
	 	For the current code release, there are no input dependencies
	 	to be met. However this file must be present (even if it is blank)
	 	in incldue path of the integrated project during compilation of code.

 \ingroup Gridiron
*/
#ifndef _GRIDIRON_IP_INTERFACE_H_
#define _GRIDIRON_IP_INTERFACE_H_


#   include "Gridiron_PlatformSpecific.h"
#   include "Gridiron_op_interface.h"
#   include "FrameDimension_op_interface.h"
#   include "lla_abstraction.h"

/**********************************************************************************/

/*  Exported prepocessing macro                                                   */

/**********************************************************************************/

/// Interface to Set GRIDIRON_LOGRIDPITCH
#   define Set_Gridiron_LOGRID_PITCH(Value)    Set_ISP_GRIDIRON_ISP_GRIDIRON_CTRL_loggridpitch(Value)

/// Interface to Set GRIDIRON_CR_HSTART
#   define Set_Gridiron_CR_HSTART(Value)   Set_ISP_GRIDIRON_ISP_GRIDIRON_CR_START_cr_h_start(Value)

/// Interface to Set GRIDIRON_CR_VSTART
#   define Set_Gridiron_CR_VSTART(Value)   Set_ISP_GRIDIRON_ISP_GRIDIRON_CR_START_cr_v_start(Value)

/// Interface to Set GRIDIRON_IM_HSIZE
#   define Set_Gridiron_IM_HSIZE(Value)    Set_ISP_GRIDIRON_ISP_GRIDIRON_IM_SIZE_im_h_size(Value)

/// Interface to Set GRIDIRON_IM_VSIZE
#   define Set_Gridiron_IM_VSIZE(Value)    Set_ISP_GRIDIRON_ISP_GRIDIRON_IM_SIZE_im_v_size(Value)

/// Interface to Set SENSOR_HSIZE
#   define Set_Gridiron_SENSOR_HSIZE(Value)    Set_ISP_GRIDIRON_ISP_GRIDIRON_SENSOR_SIZE_sensor_h_size(Value)

/// Interface to Set SENSOR_HSCALE
#   define Set_Gridiron_SENSOR_HSCALE(Value)   Set_ISP_GRIDIRON_ISP_GRIDIRON_SCALE_hscale(Value)

/// Interface to Set SENSOR_VSCALE
#   define Set_Gridiron_SENSOR_VSCALE(Value)   Set_ISP_GRIDIRON_ISP_GRIDIRON_SCALE_vscale(Value)

/// Interface to Set FPHASE
#   define Set_Gridiron_FPHASE(Value)  Set_ISP_GRIDIRON_ISP_GRIDIRON_LIVE_CAST_PHASE_fphase(Value)

/// Interface to Get ImageHSize from Sensor in Scaled Domain
#   define Get_IM_HSIZE()  FrameDimension_GetCurrentOPXOutputSize()

/// Interface to Get ImageVSize from Sensor in Scaled Domain
#   define Get_IM_VSIZE()  FrameDimension_GetCurrentOPYOutputSize()



/// Interface to Set EnCast0
#   define Set_EnCast0(Value)                                   \
    if (Value)                                                  \
    {                                                           \
        Set_ISP_GRIDIRON_ISP_GRIDIRON_CTRL_en_cast0__ENABLE();  \
    }                                                           \
    else                                                        \
    {                                                           \
        Set_ISP_GRIDIRON_ISP_GRIDIRON_CTRL_en_cast0__DISABLE(); \
    }


/// Interface to Set EnCast1
#   define Set_EnCast1(Value)                                   \
    if (Value)                                                  \
    {                                                           \
        Set_ISP_GRIDIRON_ISP_GRIDIRON_CTRL_en_cast1__ENABLE();  \
    }                                                           \
    else                                                        \
    {                                                           \
        Set_ISP_GRIDIRON_ISP_GRIDIRON_CTRL_en_cast1__DISABLE(); \
    }


/// Interface to Set EnCast2
#   define Set_EnCast2(Value)                                   \
    if (Value)                                                  \
    {                                                           \
        Set_ISP_GRIDIRON_ISP_GRIDIRON_CTRL_en_cast2__ENABLE();  \
    }                                                           \
    else                                                        \
    {                                                           \
        Set_ISP_GRIDIRON_ISP_GRIDIRON_CTRL_en_cast2__DISABLE(); \
    }


/// Interface to Set EnCast1
#   define Set_EnCast3(Value)                                   \
    if (Value)                                                  \
    {                                                           \
        Set_ISP_GRIDIRON_ISP_GRIDIRON_CTRL_en_cast3__ENABLE();  \
    }                                                           \
    else                                                        \
    {                                                           \
        Set_ISP_GRIDIRON_ISP_GRIDIRON_CTRL_en_cast3__DISABLE(); \
    }





/// Interface to Disable Mem_init
#   define Set_MEM_INIT_DISABLE()  Set_ISP_GRIDIRON_ISP_GRIDIRON_CTRL_mem_init__B_0x0()

/// Interface to Set super sensitive normal
#   define Set_SUPERSENSITIVE_NORMAL()  Set_ISP_GRIDIRON_ISP_GRIDIRON_CTRL_super_sensitive__NORMAL()



/// Interface to Enable PIX_ORDER
#   define Set_PIX_ORDER_ENABLE()  Set_ISP_GRIDIRON_ISP_GRIDIRON_PIX_ORDER_wr_pix_order__ENABLE()

/// Interface to Disable PIX_ORDER
#   define Set_PIX_ORDER_DISABLE()  Set_ISP_GRIDIRON_ISP_GRIDIRON_PIX_ORDER_wr_pix_order__DISABLE()

#define GridIron_Sensor_PreScale_Factor()    (FrameDimension_GetCurrentPreScaleFactor())


/**********************************************************************************/

/*  Exported VARIABLES                                                            */

/**********************************************************************************/

/**********************************************************************************/

/*  Exported FUNCTIONS                                                            */

/**********************************************************************************/
#endif // _GRIDIRON_IP_INTERFACE_H_
