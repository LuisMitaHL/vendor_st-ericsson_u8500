/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 \defgroup  Sensor_Tuning

 \details   The Sensor_Tuning module provides an interface to set the tuning parameters
            in the device.
*/

/**
 \if        INCLUDE_IN_HTML_ONLY
 \file      Sensor_Tuning.c

 \brief     This file performs all sensor tuning handling corresponding to user input.

 \ingroup   Sensor_Tuning
 \endif
*/
#include "Sensor_Tuning.h"
#include "EventManager.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "PictorProject_Sensor_Tuning_Sensor_TuningTraces.h"
#endif
#   include "Profiler.h"

#define DEFAULT_SENSOR_TUNING_DATA_BUFFER            (0)
#define DEFAULT_SELECTED_SUB_BLOCK_ID_INDEX (0)
#define DEFAULT_COIN_CONTROL                (Coin_e_Heads)
#define DEFAULT_TOTAL_SUB_BLOCK_IDS_COUNT   (0)
#define DEFAULT_CURRENT_SUB_BLOCK_ID_VALUE  (0)
#define DEFAULT_COIN_STATUS                 (Coin_e_Heads)
#define NVM_DETAILS_DEFAULT_COIN_VALUE      (Coin_e_Heads)
#define SENSOR_TUNING_PROGRAMMED_FLAG_DEFAULT_VALUE  (Flag_e_FALSE)
#define SENSOR_TUNING_ENABLE_BEFORE_BOOT        (Flag_e_FALSE)
#define DEFAULT_NVM_DATA_ADDRESS            (0)
#define DEFAULT_NVM_DATA_SIZE               (0)
#define DEFAULT_NVM_EXPORT_TYPE             (TypeNVMExport_e_Parsed)

volatile Sensor_Tuning_Control_ts   g_Sensor_Tuning_Control =
{
    DEFAULT_SENSOR_TUNING_DATA_BUFFER,
    DEFAULT_NVM_DATA_ADDRESS,
    DEFAULT_SELECTED_SUB_BLOCK_ID_INDEX,
    DEFAULT_COIN_CONTROL,
    NVM_DETAILS_DEFAULT_COIN_VALUE,
    SENSOR_TUNING_PROGRAMMED_FLAG_DEFAULT_VALUE,
    SENSOR_TUNING_ENABLE_BEFORE_BOOT,
    DEFAULT_NVM_EXPORT_TYPE
};

volatile Sensor_Tuning_Status_ts    g_Sensor_Tuning_Status =
{
    DEFAULT_TOTAL_SUB_BLOCK_IDS_COUNT,
    DEFAULT_CURRENT_SUB_BLOCK_ID_VALUE,
    DEFAULT_NVM_DATA_SIZE,
    DEFAULT_COIN_STATUS,
    NVM_DETAILS_DEFAULT_COIN_VALUE
};

void
sensor_tuning_Read_sensor_tuningConfig(void)
{
    if (g_Sensor_Tuning_Status.e_Coin_SubBlock_Status != g_Sensor_Tuning_Control.e_Coin_SubBlock_Control)
    {
        if (g_Sensor_Tuning_Status.u16_TotalSubBlockIdsCount > g_Sensor_Tuning_Control.u16_SelectedSubBlockIdIndex)
        {
            g_Sensor_Tuning_Status.u16_CurrentSubBlockIdValue = g_config_ids.identifiers[g_Sensor_Tuning_Control.u16_SelectedSubBlockIdIndex];
            g_Sensor_Tuning_Status.e_Coin_SubBlock_Status = g_Sensor_Tuning_Control.e_Coin_SubBlock_Control;
        }
    }
}


void
NVM_Export(void)
{
    if
    (
        (g_Sensor_Tuning_Control.e_Coin_NVM__Control != g_Sensor_Tuning_Status.e_Coin_NVM_Status)
    &&  (NULL != g_Sensor_Tuning_Control.u32_NVM_Data_Address)
    &&  (0 != g_Sensor_Tuning_Status.u16_NVM_Data_Size)
    )
    {
#ifdef PROFILER_USING_XTI
    OstTraceInt0(TRACE_USER8, "start_xti_profiler_nvm");
    CpuCycleProfiler_ts *ptr_CPUProfilerData = NULL;
    CpuCycleProfiler_ts cpu_profiler_data = DEFAULT_VALUES_CPUCYCLEPROFILER;
    ptr_CPUProfilerData = &cpu_profiler_data;
    START_PROFILER();
#endif
        cam_drv_get_parsed_nvm(( CAM_DRV_NVM_T * ) g_Sensor_Tuning_Control.u32_NVM_Data_Address);

        g_Sensor_Tuning_Status.e_Coin_NVM_Status = g_Sensor_Tuning_Control.e_Coin_NVM__Control;

        // send NVM data export notifications
        EventManager_NVM_Export_Notify();
#ifdef PROFILER_USING_XTI
    Profiler_Update(ptr_CPUProfilerData);
    OstTraceInt1(TRACE_USER8, "stop_xti_profiler_nvm : %d ",ptr_CPUProfilerData->u32_CurrentCycles);
#endif
    }
}

