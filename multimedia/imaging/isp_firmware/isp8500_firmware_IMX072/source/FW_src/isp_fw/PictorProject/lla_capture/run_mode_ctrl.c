/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 \file 		run_mode_ctrl.c
 \brief
 \ingroup   runmode
*/
#include "run_mode_ctrl.h"

RunMode_Control_ts  g_RunMode_Control =
{
    StreamMode_e_VF,
    SensorModeVF_e_STILL,
    SensorModeCapture_e_STILL,
    Flag_e_FALSE,
    Coin_e_Tails,
    CAM_DRV_USAGE_MODE_UNDEFINED,
};

RunMode_Status_ts   g_RunMode_Status = { StreamMode_e_VF, Coin_e_Tails };

