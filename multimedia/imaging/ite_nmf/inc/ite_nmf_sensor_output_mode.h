/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __SENSOR_OUTPUT_MODE__
#define __SENSOR_OUTPUT_MODE__

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
#include "ite_sia_interface_data.h"
#include "ite_init.h"
#include "ite_sia_init.h"
#include "ite_display.h"
#include "ite_event.h"
#include "ite_pageelements.h"
#include "ite_grab.h"
#include "ite_vpip.h"
#include "ite_sia_buffer.h"
#include "ite_host2sensor.h"
#include "ite_sensorinfo.h"
#include <cm/inc/cm_macros.h>
#include <los/api/los_api.h>
#include "ite_main.h"
#include "sia.h"


t_uint32 ITE_NMF_getSensorOutputModeDataSize();
void ITE_NMF_writeSensorOutputModeBufferAddress();
void ITE_NMF_checkSensorOutputModeStatusCoin();
void ITE_NMF_TestSensorOutputMode();
RESULT_te ITE_NMF_toggleSensorOutputModeControlCoin();
t_uint32 ITE_NMF_mapSensorOutputModeMemArea(t_uint32 physicalAddr);
RESULT_te ITE_NMF_Wait_For_SensorOutputMode_Notification();
extern void ITE_NMF_freeSensorOutputModeBuffer(tps_siaMetaDatabuffer pBufferMetaData);
void ITE_NMF_Get_SensorOutputMode_Data();
CMD_COMPLETION ITE_NMF_Print_SensorOutputMode_Data(unsigned int   *SensorOutputModeBufferPtr);
CMD_COMPLETION C_ite_dbg_SensorOutputModehelp_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_Print_SensorOutputMode_Data_cmd(int a_nb_args, char ** ap_args);

 #endif 
 /*__SENSOR_OUTPUT_MODE__*/

