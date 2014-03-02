/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 \if INCLUDE_IN_HTML_ONLY
 \file  FrameRate.c
 \brief This file is a part of the release code of the Frame rate module.

 \ingroup FrameRate
 \endif
*/
#include "FrameRate.h"

FrameRateControl_ts g_FrameRateControl =
{
    DEFAULT_SYSTEM_GAIN_THRESHOLD_LOW,
    DEFAULT_SYSTEM_GAIN_THRESHOLD_HIGH,
    DEFAULT_FRAME_RATE_MINIMUM,
    DEFAULT_FRAME_RATE_MAXIMUM,
    DEFAULT_FRAME_RATE_COLD_START,
    DEFAULT_REALATIVE_CHANGE,
    DEFAULT_LINK_FRAME_RATE_AND_MAX_USER_INTEGRATION_TIME,
    DEFAULT_FRAME_RATE_MODE,
    DEFAULT_COIN_FRAME_RATE_CHANGE

};

FrameRateStatus_ts  g_FrameRateStatus =
{
    DEFAULT_FRAME_RATE_IMPLIED_GAIN,                     // implied gain
    DEFAULT_FRAME_RATE_DESIRED_FRAME_RATE_HZ,            // desired frame rate
    DEFAULT_FRAME_RATE_MIN_FRAME_LENGTH_LINES,           // minimum frame length: 851
    DEFAULT_FRAME_RATE_MAX_FRAME_LENGTH_LINES,           // maximum frame length: 851
    DEFAULT_FRAME_RATE_FRAME_LENGTH_CHANGE_LINES,        // frame length change lines
    DEFAULT_FRAME_RATE_DESIRED_FRAME_LENGTH_LINES,       // desired frame length lines
    DEFAULT_COIN_FRAME_RATE_CHANGE                       //e_Coin_FrameRateChange_Status
};

VariableFrameRateControl_ts   g_VariableFrameRateControl =
{
        DEFAULT_VARIABLE_FRAMERATE_CONTROL_CURRENT_FRAME_RATE,       // f_CurrentFrameRate_Hz
        DEFAULT_VARIABLE_FRAMERATE_CONTROL_MAXIMUM_FRAME_RATE,      // f_MaximumFrameRate_Hz
        DEFAULT_VARIABLE_FRAMERATE_CONTROL_FLAG                                   // e_Flag
};

VariableFrameRateStatus_ts   g_VariableFrameRateStatus =
{
        DEFAULT_VARIABLE_FRAMERATE_CONTROL_CURRENT_FRAME_RATE,       // f_CurrentFrameRate_Hz
        DEFAULT_VARIABLE_FRAMERATE_CONTROL_MAXIMUM_FRAME_RATE,      // f_MaximumFrameRate_Hz
        DEFAULT_VARIABLE_FRAMERATE_CONTROL_FLAG                                   // e_Flag
};

float FrameRate_GetAskedFrameRate()
{
    if(Flag_e_TRUE == g_VariableFrameRateStatus.e_Flag)
    {
        if(g_VariableFrameRateControl.f_CurrentFrameRate_Hz <= g_VariableFrameRateStatus.f_MaximumFrameRate_Hz)
        {
            return g_VariableFrameRateControl.f_CurrentFrameRate_Hz;
        }
        else
        {
           return g_VariableFrameRateStatus.f_MaximumFrameRate_Hz;
        }
    }
    else
    {
        return g_FrameRateControl.f_UserMaximumFrameRate_Hz;
    }
}

float FrameRate_GetMaximumFrameRate()
{
    if(Flag_e_TRUE == g_VariableFrameRateStatus.e_Flag)
    {
        return g_VariableFrameRateStatus.f_MaximumFrameRate_Hz;
    }
    else
    {
        return g_FrameRateControl.f_UserMaximumFrameRate_Hz;
    }
}

