/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef  __ITE_NMF_GRIDIRON_H__
#define  __ITE_NMF_GRIDIRON_H__

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

#define ENABLE_GRIDIRON_DEBUG_TRACES (0)

CMD_COMPLETION C_ite_dbg_ConfigureGridIron_StaticPrams_cmd(int     a_nb_args, char    **ap_args);
CMD_COMPLETION C_ite_dbg_TestGridIronLiveCast_cmd(int     a_nb_args, char    **ap_args);

#if ENABLE_GRIDIRON_DEBUG_TRACES
CMD_COMPLETION C_ite_dbg_PrintGridIron_cmd( int     a_nb_args, char    **ap_args);
#endif

void Print_Gridiron_Parameters();
void Configure_Gridiron_StaticPrarams(t_uint32 pixel_order, t_uint32 reference_cast_count);
int TestGridironLiveCast();
void Enable_Gridiron(t_bool enableBit);
void GetExpectedParameterValues(t_uint32 reference_cast_count, float reference_cast_0, float reference_cast_1, float reference_cast_2, float reference_cast_3, float ptr_live_cast, 
                                                                  t_uint32 *ptr_enable_cast_0, t_uint32 *ptr_enable_cast_1, t_uint32 *ptr_enable_cast_2, t_uint32 *ptr_enable_cast_3, t_uint32 *ptr_phase, float *ptr_status_live_cast);

#endif        // __ITE_NMF_GRIDIRON_H__

