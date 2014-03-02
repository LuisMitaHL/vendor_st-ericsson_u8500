/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

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

#define INCLUDE_NVM_CHANGES 1

typedef t_uint16 uint16;
typedef t_uint32 uint32;
typedef t_uint8 uint8;



//void C_ite_dbg_NVMhelp_cmd(void);
CMD_COMPLETION C_ite_dbg_NVMhelp_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_TestNVM_Data_Supported_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_TestNVM_Data_cmd(int a_nb_args, char ** ap_args);
CMD_COMPLETION C_ite_dbg_Test_RAW_NVM_Data_cmd(int a_nb_args, char ** ap_args);

t_uint32 ITE_NMF_getNvmDataSize();
void ITE_NMF_writeNvmBufferAddress();
void ITE_NMF_toggleNvmControlCoin();
void ITE_NMF_checkNvmStatusCoin();
void ITE_NMF_Wait_For_NVM_Notification();
void ITE_NMF_TestNvm();
 t_uint32 ITE_NMF_mapNVMMemArea(t_uint32 physicalAddr);

