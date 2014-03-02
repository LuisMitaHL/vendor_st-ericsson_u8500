/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef  __ITE_NMF_EXPOSURE_H__
#define  __ITE_NMF_EXPOSURE_H__

#include "cli.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ite_nmf_sensor_tunning_functions.h"
#include "ite_nmf_statistic_functions.h"
#include "ite_testenv_utils.h"

#include "ite_boardinfo.h"
#include "ite_platform.h"
#include "VhcElementDefs.h"

//for NMF
#include "ite_sia_interface_data.h"
#include "ite_init.h"
#include "ite_sia_init.h"
//#include "ite_alloc.h"
#include "ite_display.h"
#include "ite_event.h"
#include "ite_pageelements.h"
#include "ite_grab.h"
#include "ite_vpip.h"
#include "ite_sia_buffer.h"
#include "ite_host2sensor.h"
#include "ite_sensorinfo.h"


//#include "ite_nmf_tuning_functions.h"

#include <cm/inc/cm_macros.h>

#include <los/api/los_api.h>
#include "ite_main.h"
#include "sia.h"

CMD_COMPLETION C_ite_dbg_Test_MaxExposureTime_cmd(int     a_nb_args, char    **ap_args);
CMD_COMPLETION C_ite_dbg_ToggleSystemCoin_cmd(int     a_nb_args, char    **ap_args);
t_uint32 TestMaxExposureTime();
void ToggleSystemCoin();
CMD_COMPLETION C_ite_dbg_Set_ExposureTime_cmd(int     a_nb_args, char    **ap_args);
CMD_COMPLETION C_ite_dbg_Get_ExposureTime_cmd(int     a_nb_args, char    **ap_args);

void SetExposureTime(t_uint32 u32_exposure_time_us);
void GetExposureTime();

#endif        // __ITE_NMF_EXPOSURE_H__

