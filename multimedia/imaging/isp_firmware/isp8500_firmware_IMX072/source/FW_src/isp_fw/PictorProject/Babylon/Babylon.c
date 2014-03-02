/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
\file Babylon.c
 */
#include "Babylon.h"
#include "Babylon_ip_interface.h"
#include "Babylon_platform_specific.h"

Babylon_Ctrl_ts             g_Babylon_Ctrl =
{
    DEFAULT_BABYLON_ENABLE,
    DEFAULT_BABYLON_ZIPPERKILL,
    DEFAULT_BABYLON_FLAT_THRESHOLD,
    DEFAULT_BABYLON_FLAT_THRESHOLD,
    DEFAULT_BABYLON_MODE
};



void
Babylon_Commit(void)
{
    if (Flag_e_TRUE == g_Babylon_Ctrl.e_Flag_BabylonEnable)
    {
        /// set the zipperkill control
        Set_BABYLON_ZIPPERKILL(g_Babylon_Ctrl.u8_ZipperKill);

        /// set the flat threshold
        Set_BABYLON_FLAT_TH(g_Babylon_Ctrl.u8_Flat_Threshold_Status);
    }


    /// commit the enable/disable of the block
    Set_BABYLON_ENABLE(g_Babylon_Ctrl.e_Flag_BabylonEnable, 0); //enable_babylon,soft_reset
}


/// calculates the values that are to be programmed on to the babylon hardware block
void
Babylon_Update(void)
{
    float_t f_damper_output = 1.0;




    g_Babylon_Ctrl.u8_Flat_Threshold_Status = (uint8_t) (g_Babylon_Ctrl.u8_Flat_Threshold * f_damper_output);
}

