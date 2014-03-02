/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef  __ITE_NMF_RXTESTPATTERN_H__
#define  __ITE_NMF_RXTESTPATTERN_H__



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





void ITE_SetRxTestPattern(RxTestPattern_te ap_pattern);
void ITE_SetSolidColorData(t_uint16 Red, t_uint16 GR, t_uint16 Blue, t_uint16 BG);
void ITE_SetRxCursorPosition(t_uint8 x_pos, t_uint8 x_width, t_uint8 y_pos, t_uint8 y_width);
void ITE_CheckRxPattern(RxTestPattern_te ap_pattern);
void ITE_CheckSolidColorData(t_uint16 Red, t_uint16 GR, t_uint16 Blue, t_uint16 BG);

#endif
