/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef  __ITE_NMF_GLACE_GEOMETRY_H__
#define  __ITE_NMF_GLACE_GEOMETRY_H__

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

#define ENABLE_GLACE_GEOMETRY_DEBUG_TRACES (0)

CMD_COMPLETION C_ite_dbg_Test_Glace_Geometry_cmd(int     a_nb_args, char    **ap_args);
CMD_COMPLETION C_ite_dbg_Set_Glace_Geometry_cmd(int     a_nb_args, char    **ap_args);
CMD_COMPLETION C_ite_dbg_Get_Glace_Geometry_cmd(int     a_nb_args, char    **ap_args);
CMD_COMPLETION C_ite_dbg_Set_BMS_Size_cmd(int     a_nb_args, char    **ap_args);

t_uint32 Test_Glace_Geometry();

void Set_Glace_Geometry(t_uint32 u32_geometry_mode, float f_hBlockSizeFrac, float f_vBlockSizeFrac, 
                                                    float f_hROIStartFrac, float f_vROIStartFrac, 
                                                    t_uint32 u32_hGridSize, t_uint32 u32_vGridSize);

void Get_Glace_Geometry();

void         ComputeExpectedGlaceGeometry(t_uint32 *ptr_u32_expected_roi_start_x, t_uint32 *ptr_u32_expected_roi_start_y, 
                                                                    t_uint32 *ptr_u32_expected_grid_size_x, t_uint32 *ptr_u32_expected_grid_size_y,
                                                                    t_uint32 *ptr_u32_expected_block_size_x, t_uint32 *ptr_u32_expected_block_size_y);
                                                                    
t_uint32 CompareComputedAndStatusGlaceGeometry(t_uint32 u32_expected_roi_start_x, t_uint32 u32_expected_roi_start_y, 
                                                                    t_uint32 u32_expected_grid_size_x, t_uint32 u32_expected_grid_size_y,
                                                                    t_uint32 u32_expected_block_size_x, t_uint32 u32_expected_block_size_y);
void Set_bms_size(t_uint32 u32_woi_x, t_uint32 u32_woi_y, t_uint32 u32_out_res_x, t_uint32 u32_out_res_y);
#endif        // __ITE_NMF_GLACE_GEOMETRY_H__
