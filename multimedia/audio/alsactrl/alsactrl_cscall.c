/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */
/*****************************************************************************/

#include "alsactrl_alsa.h"
#include "alsactrl_debug.h"

#include "alsactrl_hwh.h"

void audio_hal_set_msp4_clock(int open)
{
	int ret;

	if (open == 1) {
		LOG_I("MSP4 Clock Enable\n");

        Alsactrl_Hwh_OpenControls();
        ret=audio_hal_alsa_set_control("MSP4 Clock Enable", 0 , 1);

        if(ret < 0)
        {
            LOG_E("ERROR: MSP4 Clock Enable failed\n");
        }
        Alsactrl_Hwh_CloseControls();
    }
    else
    {
        LOG_I("MSP4 Clock Disable\n");

        Alsactrl_Hwh_OpenControls();
        ret=audio_hal_alsa_set_control("MSP4 Clock Enable", 0 , 0);

        if(ret < 0)
        {
            LOG_E("ERROR: MSP4 Clock Disable failed\n");
        }
        Alsactrl_Hwh_CloseControls();
    }
}
