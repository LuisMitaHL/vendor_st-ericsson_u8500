/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
\file Scorpio.c
 */
#include "Scorpio.h"
#include "Scorpio_ip_interface.h"

// local function declaration
void                        Scorpio_DamperUpdate (void);

Scorpio_Ctrl_ts             g_Scorpio_Ctrl = { DEFAULT_SCORPIO_ENABLE, DEFAULT_SCORPIO_MODE, DEFAULT_SCORPIO_CORING_LEVEL, DEFAULT_SCORPIO_CORING_LEVEL };



/// this function programs the recovery engine with appropriate scorpio parameters
void
Scorpio_Commit(void)
{
    /// set required coring level
    Set_SCORPIO_CORING_LEVEL(g_Scorpio_Ctrl.u8_CoringLevel_Status);

    /// enable/disable the hw block
    Set_SCORPIO_ENABLE(g_Scorpio_Ctrl.e_Flag_ScorpioEnable);
}

/// calculates the values that are to be programmed on to the scorpio hardware block
void
Scorpio_Update(void)
{
    float_t f_damper_output = 1.0;


    g_Scorpio_Ctrl.u8_CoringLevel_Status = g_Scorpio_Ctrl.u8_CoringLevel_Ctrl * f_damper_output;
}

