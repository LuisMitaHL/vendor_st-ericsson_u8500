/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef  __ITE_NMF_RGB2YUVCODER_H__
#define  __ITE_NMF_RGB2YUVCODER_H__

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

#define ENABLE_RGB2YUVCODER_DEBUG_TRACES (0)

typedef struct
{
    /// Difference between Maximum code and minimum Luma values \n
    /// YUV format can give maximum 255 as output, so difference between Max and min
    /// Luma or Y should be 255 \n
    /// RGB format can give maximum 1023 as output (ISP is 10 bit pipe), so difference between Max and min
    /// Luma or Y should be maximum 1023 \n
    /// [DEFAULT]:  255
    t_uint16    u16_LumaExcursion;

    /// Midpoint multiplied by 2 for Luma \n
    /// e.g. MaxY = 220, MinY = 20, So midpoint will be = (20 + 220) = 240 \n
    /// [DEFAULT]:  255
    t_uint16    u16_LumaMidpointTimes2;

    /// Difference between Maximum code and minimum Chroma values \n
    /// YUV format can give maximum 255 as output, so difference between Max and min
    /// chroma or Cb/Cr will be 255 \n
    /// RGB format can give maximum 1023 as output (ISP is 10 bit pipe), so difference between Max and min
    /// should be maximum 1023 \n
    /// [DEFAULT]:  255
    t_uint16    u16_ChromaExcursion;

    /// Midpoint multiplied by 2 for Chroma \n
    /// e.g. Max = 220, Min = 20, So midpoint will be = (20 + 220) = 240 \n
    /// [DEFAULT]:  255
    t_uint16    u16_ChromaMidpointTimes2;
} OutputSignalRange;

#define NO_OF_HW_PIPES (2)


CMD_COMPLETION C_ite_dbg_Configure_RGB2YUVCoder_cmd(int     a_nb_args, char    **ap_args);
CMD_COMPLETION C_ite_dbg_Test_RGB2YUVCoder_cmd(int     a_nb_args, char    **ap_args);
CMD_COMPLETION C_ite_dbg_Configure_CustomStockMatrix_cmd(int     a_nb_args, char    **ap_args);
CMD_COMPLETION C_ite_dbg_Configure_CustomSignalRange_cmd(int     a_nb_args, char    **ap_args);
CMD_COMPLETION C_ite_dbg_Test_Brightness_Control_Test_cmd(int     a_nb_args,char    **ap_args);
t_uint32 ConfigureRGB2YUVcoder(t_uint32 u32_pipe, t_uint32 u32_transform_type, t_uint32 u32_contrast, t_uint32 u32_max_contrast, t_uint32 u32_color_saturation, t_uint32 u32_max_color_saturation);
t_uint32 TestRGB2YUVcoder();
float ComputeContrast(t_uint32 u32_contrast, t_uint32 u32_max_contrast);
float ComputeColorSaturation(t_uint32 u32_color_saturation, t_uint32 u32_max_color_saturation);
void SetStockMatrixAndSignalRange(t_uint32 u32_pipe_num, t_uint32 u32_transform_type, float f_RgbToYuvStockMatrix[], OutputSignalRange *ptr_signal_range);
t_uint32 ApplyExposureAndSaturation(float f_contrast, float f_color_saturation, float f_RgbToYuvStockMatrix[], OutputSignalRange *ptr_SignalRange);
int GetRounded(float f_Input);
void PrintMatrixFloat(float f_RgbToYuvStockMatrix[], t_uint32 u32_row_count, t_uint32 u32_column_count);
void PrintMatrixInt(int s32_RgbToYuvStockMatrix[], t_uint32 u32_row_count, t_uint32 u32_column_count);
t_uint32 ComputeLumaRowSum(float f_contrast, OutputSignalRange *ptr_SignalRange);
t_uint32 ComputeExpectedCoefficients(t_uint32 u32_luma_row_sum, OutputSignalRange* ptr_SignalRange, float f_contrast, float f_color_saturation, 
                                                                        t_uint32 u32_scalar_multiplication_factor, float f_RgbToYuvStockMatrix[], int* s32_output_matrix);
t_uint32 ComputeExpectedOffsetValues(int* arr_s32_output_matrix, OutputSignalRange* ptr_SignalRange, int* s32_expected_offset);
void GetStatusOutputAndOffsetMatrix(t_uint32 u32_pipe_num, int* s16_status_output_matrix, int* s16_status_offset_matrix);

void ApplyCustomStockMatrix(t_uint32 u32_pipe_num , float* f_arr_stock_matrix);
void ApplyCustomSignalRange(t_uint32 u32_pipe_num , OutputSignalRange* ptr_signal_range);
void HandleInvalidPipe(t_uint32 u32_pipe_num);
void HandleInvalidTransform(int s32_transform_type);
int CompareComputedAndStatusMatrices(int *arr_s32_computed_output_matrix, int *arr_s32_computed_offsets, 
     int *arr_s32_status_output_matrix, int *arr_s32_status_offsets);
void Brightness_Control_Test (char *ap_pipe,char *step_size);
void Toggle_System_Coin(void);
#endif        // __ITE_NMF_RGB2YUVCODER_H__
