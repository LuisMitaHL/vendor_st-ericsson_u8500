/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef  __ITE_NMF_HISTOGRAM_GEOMETRY_H__
#define  __ITE_NMF_HISTOGRAM_GEOMETRY_H__

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
//#include "pictor_full.h"
#include "ite_ispblocks_tests.h"

#define ENABLE_HISTOGRAM_GEOMETRY_DEBUG_TRACES (0)

CMD_COMPLETION C_ite_dbg_Test_Histogram_Geometry_cmd(int     a_nb_args, char    **ap_args);
CMD_COMPLETION C_ite_dbg_Set_Histogram_Geometry_cmd(int     a_nb_args, char    **ap_args);
CMD_COMPLETION C_ite_dbg_Get_Histogram_Geometry_cmd(int     a_nb_args, char    **ap_args);

t_uint32 Test_Histogram_Geometry();

void Set_Histogram_Geometry(t_uint32 u32_geometry_mode,
                                                float f_hSizeFrac, float f_vSizeFrac,
                                                float f_hStartFrac, float f_vStartFrac);

void Get_Histogram_Geometry();

void ComputeExpectedHistogramGeometry(t_uint32 *ptr_u32_expected_roi_start_x, t_uint32 *ptr_u32_expected_roi_start_y,
                                                                                    t_uint32 *ptr_u32_expected_size_x, t_uint32 *ptr_u32_expected_size_y);
                                                                    
t_uint32 CompareComputedAndStatusHistogramGeometry(t_uint32 u32_expected_roi_start_x, t_uint32 u32_expected_roi_start_y, 
                                                                     t_uint32 u32_expected_size_x, t_uint32 u32_expected_size_y);
#endif        // __ITE_NMF_HISTOGRAM_GEOMETRY_H__
