/* 
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved. 
 * This code is ST-Ericsson proprietary and confidential. 
 * Any use of the code for whatever purpose is subject to 
 * specific written permission of ST-Ericsson SA. 
 */ 
 
/**
 \defgroup FrameRate Frame Rate Module
 \brief Frame rate module calculate the frame length required to achieve max possible frame rate by the system.
        Module take inputs from exposure module and frame dimesion.
*/

/**
 \file FrameRate_op_interface.h
 \brief  This file is a part of the frame rate module release code and provide an
         interface to the module. All functionalities offered by the module are
         available through this file. The file also declare the page elements
         to be used in virtual register list.

 \ingroup FrameRate
*/
#ifndef _FRAME_RATE_OP_INTERFACE_H_
#   define _FRAME_RATE_OP_INTERFACE_H_

#   include "Platform.h"

/**
 \struct FrameRateMode_te
 \brief  Lists the moes in which frame rate module can operate
 \ingroup FrameRate
*/
typedef enum
{
	/// manual mode
	FrameRateMode_e_SINGLE_STEP,

	/// auto mode
	FrameRateMode_e_CONTINUOUS
} FrameRateMode_te;


/**
 \struct FrameRateControl_ts
 \brief  Control pages for Frame rate module
 \ingroup FrameRate
*/
typedef struct
{
    /// Low threshold for system gain. \n
    /// [DEFAULT]: 1.0
    float_t f_SystemGainThresholdLow;

    /// High threshold for system gain. \n
    /// [DEFAULT]: 2.0
    float_t f_SystemGainThresholdHigh;

    /// Minimum frame rate threshold \n
    /// [DEFAULT]: 1.0 fps
    float_t f_UserMinimumFrameRate_Hz;

    /// Maximum frame rate threshold \n
    /// This PE is effective only when value of g_VariableFrameRateStatus.e_Flag is Flag_e_FALSE
    /// [DEFAULT]: 30.0  fps
    float_t f_UserMaximumFrameRate_Hz;

    /// Cold start frame rate \n
    /// [DEFAULT]: 15.0 fps
    float_t f_ColdStartFrameRate;

    /// Relative change in frame length that should be made whenever a change
    /// is required. The change factor should be good enough to encorporate one
    /// flicker free bunch, otherwise gains frame length will not be absorbed by
    /// the system. \n
    /// [MIN]: .01 [MAX]: 1.0 \n
    /// [DEFAULT]: .25
    float_t f_RelativeChange;

    /// Controls the link between exposure implied minimum frame rate from UserMinimumFrameRate or
    /// user maximum integration time. If e_Flag_LinkMinFrameRateAndExposureMaxUserIntegrationTime = e_TRUE
    /// i.e. Frame rate and user max integration are linked, then frame rate can drop till it can support user maximum
    /// integration lines.\n
    /// If Both are not linked (e_FALSE), frame rate can drop more than user maximum integration lines supported. In this scenerio
    /// exposure will use only maximum user integration lines and rest of frame will not be used for integration.\n
    /// [DEFAULT]: e_TRUE
    uint8_t e_Flag_LinkMinFrameRateAndExposureMaxUserIntegrationTime;

    /// The mode in which frame rate has to be run.
    /// [DEFAULT]: FrameRateMode_e_SINGLE_STEP
    uint8_t     e_FrameRateMode;

    /// control coin used for setting min frame rate & max frame rate atomic
    /// [DEFAULT]: Coin_e_Tails
    uint8_t e_Coin_FrameRateChange_Ctrl;
} FrameRateControl_ts;

/**
 \struct FrameRateStatus_ts
 \brief  Status page element for frame rate module
 \ingroup FrameRate
*/
typedef struct
{
    /// Implied gain of system, desired exposure / integration time.
    /// Implied gain will be anlogue gain and digital gain calculated by exposure.
    float_t     f_ImpliedGain;

    /// Desired frame rate as calculated by frame rate module and it is always clipped between minimum and maximum frame rate.
    float_t     f_DesiredFrameRate_Hz;

    /// The frame length corresponding to f_UserMaximumFrameRate_Hz.
    uint16_t    u16_MinimumFrameLength_lines;

    /// The frame length corresponding to f_UserMinimumFrameRate_Hz.
    uint16_t    u16_MaximumFrameLength_lines;

    /// The change in the frame length (in number of lines) that would be done if
    /// at all required. It has been derived directly from f_RelativeChange and
    /// current frame length.
    uint16_t    u16_FrameLengthChange_lines;

    /// The frame length desired to achieve f_DesiredFrameRate_Hz.
    uint16_t    u16_DesiredFrameLength_lines;

    /// status coin used for setting min frame rate & max frame rate atomic
    uint8_t     e_Coin_FrameRateChange_Status;
} FrameRateStatus_ts;

/**
 \struct VariableFrameRateControl_ts
 \brief  Control page for variable frame rate
 \ingroup FrameRate
*/
typedef struct
{
    /// Current frame rate \n
    /// [DEFAULT]: 30.0 fps
    float_t f_CurrentFrameRate_Hz;

    /// Maximum frame rate threshold \n
    /// [DEFAULT]: 30.0  fps
    float_t f_MaximumFrameRate_Hz;

    /// status coin used for setting min frame rate & max frame rate atomic
    /// [DEFAULT]: Flag_e_FALSE  
    uint8_t     e_Flag;   
} VariableFrameRateControl_ts;


/**
 \struct VariableFrameRateStatus_ts
 \brief  Status page for variable frame rate
 \ingroup FrameRate
*/
typedef struct
{
    /// Current frame rate \n
    /// [DEFAULT]: 30.0 fps
    float_t f_CurrentFrameRate_Hz;

    /// Maximum frame rate threshold \n
    /// [DEFAULT]: 30.0  fps
    float_t f_MaximumFrameRate_Hz;

    /// status coin used for setting min frame rate & max frame rate atomic
    /// [DEFAULT]: Flag_e_FALSE  
    uint8_t     e_Flag;   
} VariableFrameRateStatus_ts;

/************************ Exported Page elements *********************/
///  g_FrameRateControl NON_MODE_STATIC_PAGE READ_WRITE_PAGE
extern FrameRateControl_ts  g_FrameRateControl;

///  g_FrameRateStatus  NON_MODE_STATIC_PAGE READ_ONLY_PAGE
extern FrameRateStatus_ts   g_FrameRateStatus;

extern VariableFrameRateControl_ts   g_VariableFrameRateControl;

extern VariableFrameRateStatus_ts   g_VariableFrameRateStatus;

/// returns value of frame rate asked by HOST
/// if not valid sensor will not stream
float FrameRate_GetAskedFrameRate();

/// returns value of maximum frame rate set by host
/// if not valid sensor will not stream
float FrameRate_GetMaximumFrameRate();    
#endif //_FRAME_RATE_OP_INTERFACE_H_

