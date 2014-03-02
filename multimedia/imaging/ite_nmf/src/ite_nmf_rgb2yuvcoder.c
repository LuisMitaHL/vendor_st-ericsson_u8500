/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include <ite_nmf_rgb2yuvcoder.h>

static t_uint32         g_u32_counter_rgb2yuvCoder_conf = 0;
static t_uint32         g_u32_counter_rgb2yuvCoder_test = 0;

static t_uint32         g_u32_counter_rgb2yuvCoder_customMatrix_conf = 0;
static t_uint32         g_u32_counter_rgb2yuvCoder_sigRange_conf = 0;

extern ts_sia_usecase   usecase;
extern void             ITE_NMF_GlaceHistoExpStatsRequestAndWait (void);

#   define FLOOR_MIN	 -256
#   define FLOOR_MAX	 256

/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_Test_RGB2YUVCoder_cmd
   PURPOSE  : To tests funcionality of RGB2YUVCoder ISP FW module
   ------------------------------------------------------------------------ */
const float
    RgbToYuvStockMatrix_REC709[9] = { 0.2126, 0.7152, 0.0722, -0.1146, -0.3854, 0.5, 0.5, -0.4542, -0.0458 };

// <Hem> As requested by Nicola MARINELLI LumaMidpointTimes2/ChromaMidpointTimes2  should be 256.
const OutputSignalRange FULL_REC709_SignalRange = { 255, 256, 255, 256 };

const float             RgbToYuvStockMatrix_JFIF[9] = { 0.299, 0.587, 0.114, -0.169, -0.331, 0.5, 0.5, -0.419, -0.081 };

// <Hem> As requested by Nicola MARINELLI LumaMidpointTimes2/ChromaMidpointTimes2  should be 256.
const OutputSignalRange FULL_JFIF_SignalRange = { 255, 256, 255, 256 };

// custom case :
float                   Custom_RgbToYuvStockMatrix[NO_OF_HW_PIPES][9] =
{
    { 0.2126, 0.7152, 0.0722, -0.1146, -0.3854, 0.5, 0.5, -0.4542, -0.0458 },
    { 0.2126, 0.7152, 0.0722, -0.1146, -0.3854, 0.5, 0.5, -0.4542, -0.0458 }
};

OutputSignalRange       Custom_SignalRange[NO_OF_HW_PIPES] = { { 255, 255, 255, 255 }, { 255, 255, 255, 255 } };

// float f_RgbToYuvStockMatrix[9];
// <Hem> Is there some reason why below 2 constants are differring by 1 in ISP FW
// <Hem> as we rely on contrast/saturation algorithms of ISP FW. Let us assume them to be corrrect.
#define K_MAX_INPUT_FROM_PIPE   (1023)
#define F_MATRIX_SCALER         (1024)

CMD_COMPLETION
C_ite_dbg_Configure_RGB2YUVCoder_cmd(
int     a_nb_args,
char    **ap_args)
{
    char        mess[256];
    t_uint32    u32_pipe = 0;
    t_uint32    u32_transform_type = 0;
    t_uint32    u32_contrast = 0;
    t_uint32    u32_max_contrast = 0;
    t_uint32    u32_color_saturation = 0;
    t_uint32    u32_max_color_saturation = 0;

    sprintf(mess, "RGB2YUVCoder_%d", ( int ) g_u32_counter_rgb2yuvCoder_conf);
    g_u32_counter_rgb2yuvCoder_conf++;

    MMTE_TEST_START(mess, "/ite_nmf/test_results_nreg/rgb2yuvcoder", "rgb2yuvcoder");

    sprintf(mess, "configuring rgb2yuvcoder");
    mmte_testNext(mess);

    switch (a_nb_args)
    {
        case 7:     // test_rgb2yuvcoder  <pipe_no.> <Transform_Type> <Contrast> <MaxContrast> <ColourSaturation> <MaxColourSaturation>
            {
                LOS_Log("6 command line arguments\n");
                u32_pipe = atoi(ap_args[1]);
                u32_transform_type = atoi(ap_args[2]);
                u32_contrast = atoi(ap_args[3]);
                u32_max_contrast = atoi(ap_args[4]);
                u32_color_saturation = atoi(ap_args[5]);
                u32_max_color_saturation = atoi(ap_args[6]);
                if
                (
                    0 != ConfigureRGB2YUVcoder(
                        u32_pipe,
                        u32_transform_type,
                        u32_contrast,
                        u32_max_contrast,
                        u32_color_saturation,
                        u32_max_color_saturation)
                )
                {
                    sprintf(mess, "rgb2yuvcoder configuration failed\n");
                    mmte_testComment(mess);
                    MMTE_TEST_FAILED();
                    return (CMD_ERR_ARGS);
                }


                break;
            }


        default:    // wrong syntax : print on stdout how to use test framework for rgb2yuvcoder
            {
                LOS_Log(
                "syntax : test_rgb2yuvcoder <e_Transform_Type> <u8_Contrast> <u8_MaxContrast> <u8_ColourSaturation> <u8_MaxColourSaturation>\n");
                break;
            }
    }


    sprintf(mess, "rgb2yuvcoder configuration done\n");
    mmte_testComment(mess);
    MMTE_TEST_PASSED();

    return (CMD_COMPLETE);
}


CMD_COMPLETION
C_ite_dbg_Configure_CustomStockMatrix_cmd(
int     a_nb_args,
char    **ap_args)
{
    char        mess[256];
    float       *f_arr_stock_matrix = NULL;
    t_uint32    u32_pipe_num = 0;

    sprintf(mess, "RGB2YUVCoder_%d", ( int ) g_u32_counter_rgb2yuvCoder_customMatrix_conf);
    g_u32_counter_rgb2yuvCoder_customMatrix_conf++;

    MMTE_TEST_START(mess, "/ite_nmf/test_results_nreg/rgb2yuvcoder", "rgb2yuvcoder");

    sprintf(mess, "configuring rgb2yuvcoder");
    mmte_testNext(mess);

    switch (a_nb_args)
    {
        case 11:    // set_custom_stock_matrix  <pipe_no.> <coef_0> <coef_1> ...    ..       .. <coef_8>
            {
                LOS_Log("10 command line arguments\n");
                u32_pipe_num = atoi(ap_args[1]);
                f_arr_stock_matrix = Custom_RgbToYuvStockMatrix[u32_pipe_num];
                f_arr_stock_matrix[0] = atof(ap_args[2]);
                f_arr_stock_matrix[1] = atof(ap_args[3]);
                f_arr_stock_matrix[2] = atof(ap_args[4]);
                f_arr_stock_matrix[3] = atof(ap_args[5]);
                f_arr_stock_matrix[4] = atof(ap_args[6]);
                f_arr_stock_matrix[5] = atof(ap_args[7]);
                f_arr_stock_matrix[6] = atof(ap_args[8]);
                f_arr_stock_matrix[7] = atof(ap_args[9]);
                f_arr_stock_matrix[8] = atof(ap_args[10]);

                ApplyCustomStockMatrix(u32_pipe_num, f_arr_stock_matrix);
                break;
            }


        default:    // wrong syntax : print on stdout how to use test framework for rgb2yuvcoder
            {
                LOS_Log("syntax : set_custom_stock_matrix  <pipe_no.> <coef_0> <coef_1> ...    ..       .. <coef_8>\n");
                mmte_testComment(mess);
                MMTE_TEST_FAILED();
                return (CMD_ERR_ARGS);

                break;
            }
    }


    sprintf(mess, "rgb2yuvcoder configuration done\n");
    mmte_testComment(mess);
    MMTE_TEST_PASSED();

    return (CMD_COMPLETE);
}


CMD_COMPLETION
C_ite_dbg_Configure_CustomSignalRange_cmd(
int     a_nb_args,
char    **ap_args)
{
    char        mess[256];
    float       *f_arr_stock_matrix = NULL;
    t_uint32    u32_pipe_num = 0;

    sprintf(mess, "RGB2YUVCoder_%d", ( int ) g_u32_counter_rgb2yuvCoder_sigRange_conf);
    g_u32_counter_rgb2yuvCoder_sigRange_conf++;

    MMTE_TEST_START(mess, "/ite_nmf/test_results_nreg/rgb2yuvcoder", "rgb2yuvcoder");

    sprintf(mess, "configuring rgb2yuvcoder");
    mmte_testNext(mess);

    switch (a_nb_args)
    {
        case 6:     // set_custom_signal_range  <pipe_no.> <u16_LumaExcursion> <u16_LumaMidpointTimes2> <u16_ChromaExcursion> <u16_ChromaMidpointTimes2>
            {
                LOS_Log("6 command line arguments\n");
                u32_pipe_num = atoi(ap_args[1]);
                f_arr_stock_matrix = Custom_RgbToYuvStockMatrix[u32_pipe_num];
                LOS_Log("f_arr_stock_matrix : %f\n",f_arr_stock_matrix);
                Custom_SignalRange[u32_pipe_num].u16_LumaExcursion = atoi(ap_args[2]);
                Custom_SignalRange[u32_pipe_num].u16_LumaMidpointTimes2 = atoi(ap_args[3]);
                Custom_SignalRange[u32_pipe_num].u16_ChromaExcursion = atoi(ap_args[4]);
                Custom_SignalRange[u32_pipe_num].u16_ChromaMidpointTimes2 = atoi(ap_args[5]);

                ApplyCustomSignalRange(u32_pipe_num, &Custom_SignalRange[u32_pipe_num]);
                break;
            }


        default:    // wrong syntax : print on stdout how to use test framework for rgb2yuvcoder
            {
                LOS_Log(
                "syntax : set_custom_signal_range  <pipe_no.> <u16_LumaExcursion> <u16_LumaMidpointTimes2> <u16_ChromaExcursion> <u16_ChromaMidpointTimes2>\n");
                mmte_testComment(mess);
                MMTE_TEST_FAILED();
                return (CMD_ERR_ARGS);

                break;
            }
    }


    sprintf(mess, "rgb2yuvcoder configuration done\n");
    mmte_testComment(mess);
    MMTE_TEST_PASSED();

    return (CMD_COMPLETE);
}


CMD_COMPLETION
C_ite_dbg_Test_RGB2YUVCoder_cmd(
int     a_nb_args,
char    **ap_args)
{
    char    mess[256];
    int     s32_result = 0;

    // inclrement identifier count
    // <Hem> TODO : replace this test identifier mechanism by string identifier mechanism
    sprintf(mess, "RGB2YUVCoder_%d", ( int ) g_u32_counter_rgb2yuvCoder_test);
    g_u32_counter_rgb2yuvCoder_test++;

    MMTE_TEST_START(mess, "/ite_nmf/test_results_nreg/rgb2yuvcoder", "Testrgb2yuvcoder");

    sprintf(mess, "Testing for rgb2yuvcoder");
    mmte_testNext(mess);

    switch (a_nb_args)
    {
        case 2:     // test_rgb2yuvcoder <e_Transform_Type> <u8_Contrast> <u8_MaxContrast> <u8_ColourSaturation> <u8_MaxColourSaturation>
            {
                LOS_Log("1 command line arguments\n");
                s32_result = TestRGB2YUVcoder(atoi(ap_args[1]));
                break;
            }


        default:    // wrong syntax : tell how to use test framework for rgb2yuvcoder
            {
                LOS_Log("syntax : test_rgb2yuvcoder\n");
                break;
            }
    }


    if (0 == s32_result)
    {
        sprintf(mess, "rgb2yuvcoder test successfull\n");
        mmte_testComment(mess);
        MMTE_TEST_PASSED();
    }
    else
    {
        sprintf(mess, "rgb2yuvcoder test failed\n");
        mmte_testComment(mess);
        MMTE_TEST_FAILED();
    }


    return (CMD_COMPLETE);
}

CMD_COMPLETION
C_ite_dbg_Test_Brightness_Control_Test_cmd(
int     a_nb_args,
char    **ap_args)
{
if (0 == strcmp(ap_args[1],"help"))
{
  CLI_disp_error("syntax: br_control <PIPE> <step_size>\n");
}
else 
{
if (a_nb_args == 3)
{
 Brightness_Control_Test(ap_args[1],ap_args[2]);
}
else
{
 CLI_disp_error("Not correct command arguments\n");
}
}
   return CMD_COMPLETE;
}

t_uint32
ConfigureRGB2YUVcoder(
t_uint32    u32_pipe,
t_uint32    u32_transform_type,
t_uint32    u32_contrast,
t_uint32    u32_max_contrast,
t_uint32    u32_color_saturation,
t_uint32    u32_max_color_saturation)
{
    LOS_Log("\n>> ConfigureRGB2YUVcoder : pipe no. = %u\n", u32_pipe);

#if ENABLE_RGB2YUVCODER_DEBUG_TRACES 
    // assert(u32_pipe  <= 1);
    LOS_Log("u32_transform_type = %d\n", u32_transform_type);
    LOS_Log("u32_contrast = %d\n", u32_contrast);
    LOS_Log("u32_max_contrast = %d\n", u32_max_contrast);
    LOS_Log("u32_color_saturation = %d\n", u32_color_saturation);
    LOS_Log("u32_max_color_saturation = %d\n", u32_max_color_saturation);
#endif

    if (0 == u32_pipe)
    {
        ITE_writePE(CE_YUVCoderControls_0_e_Transform_Type_Byte0, u32_transform_type);
        ITE_writePE(CE_YUVCoderControls_0_u8_Contrast_Byte0, u32_contrast);
        ITE_writePE(CE_YUVCoderControls_0_u8_MaxContrast_Byte0, u32_max_contrast);
        ITE_writePE(CE_YUVCoderControls_0_u8_ColourSaturation_Byte0, u32_color_saturation);
        ITE_writePE(CE_YUVCoderControls_0_u8_MaxColourSaturation_Byte0, u32_max_color_saturation);
    }
    else if (1 == u32_pipe)
    {
        ITE_writePE(CE_YUVCoderControls_1_e_Transform_Type_Byte0, u32_transform_type);
        ITE_writePE(CE_YUVCoderControls_1_u8_Contrast_Byte0, u32_contrast);
        ITE_writePE(CE_YUVCoderControls_1_u8_MaxContrast_Byte0, u32_max_contrast);
        ITE_writePE(CE_YUVCoderControls_1_u8_ColourSaturation_Byte0, u32_color_saturation);
        ITE_writePE(CE_YUVCoderControls_1_u8_MaxColourSaturation_Byte0, u32_max_color_saturation);
    }
    else
    {
        return (-1);
    }


    // now toggle system coin if we are in streaming state
    if (HostInterfaceHighLevelState_e_RUNNING == ITE_readPE(HostInterface_Status_e_HostInterfaceHighLevelState_Byte0))
    {
        ITE_NMF_GlaceHistoExpStatsRequestAndWait();
    }


    return (0);
    LOS_Log("\n<< ConfigureRGB2YUVcoder\n");
}


t_uint32
TestRGB2YUVcoder(
t_uint32    pipe_no)
{
    t_uint32            u32_transform_type = 0;
    t_uint32            u32_control_contrast = 0;
    t_uint32            u32_control_max_contrast = 0;
    t_uint32            u32_control_color_saturation = 0;
    t_uint32            u32_control_max_color_saturation = 0;
    t_uint32            u32_luma_row_sum = 0;
    float               f_Contrast = 0;
    float               f_Saturation = 0;
    float               f_RgbToYuvStockMatrix[9];
    OutputSignalRange   signalRange;
    int                 arr_s32_computed_output_matrix[9];
    int                 arr_s32_computed_offsets[3];
    int                 arr_s32_status_output_matrix[9];
    int                 arr_s32_status_offsets[3];
    int                 s32_err = 0;

    LOS_Log("\n>> TestRGB2YUVcoder\n");

    if (0 == pipe_no)
    {
        u32_transform_type = ITE_readPE(CE_YUVCoderControls_0_e_Transform_Type_Byte0);
        u32_control_contrast = ITE_readPE(CE_YUVCoderControls_0_u8_Contrast_Byte0);
        u32_control_max_contrast = ITE_readPE(CE_YUVCoderControls_0_u8_MaxContrast_Byte0);
        u32_control_color_saturation = ITE_readPE(CE_YUVCoderControls_0_u8_ColourSaturation_Byte0);
        u32_control_max_color_saturation = ITE_readPE(CE_YUVCoderControls_0_u8_MaxColourSaturation_Byte0);
    }
    else if (1 == pipe_no)
    {
        u32_transform_type = ITE_readPE(CE_YUVCoderControls_1_e_Transform_Type_Byte0);
        u32_control_contrast = ITE_readPE(CE_YUVCoderControls_1_u8_Contrast_Byte0);
        u32_control_max_contrast = ITE_readPE(CE_YUVCoderControls_1_u8_MaxContrast_Byte0);
        u32_control_color_saturation = ITE_readPE(CE_YUVCoderControls_1_u8_ColourSaturation_Byte0);
        u32_control_max_color_saturation = ITE_readPE(CE_YUVCoderControls_1_u8_MaxColourSaturation_Byte0);
    }
    else
    {
        return (-1);
    }


    f_Contrast = ComputeContrast(u32_control_contrast, u32_control_max_contrast);
    f_Saturation = ComputeColorSaturation(u32_control_color_saturation, u32_control_max_color_saturation);
    SetStockMatrixAndSignalRange(pipe_no, u32_transform_type, f_RgbToYuvStockMatrix, &signalRange);
    ApplyExposureAndSaturation(f_Contrast, f_Saturation, f_RgbToYuvStockMatrix, &signalRange);
    u32_luma_row_sum = ComputeLumaRowSum(f_Contrast, &signalRange);

    // maximum possible output of scalar is 1024, so we are using F_MATRIX_SCALER(1024) as multimplicationFactor
    ComputeExpectedCoefficients(
    u32_luma_row_sum,
    &signalRange,
    f_Contrast,
    f_Saturation,
    F_MATRIX_SCALER,
    f_RgbToYuvStockMatrix,
    arr_s32_computed_output_matrix);
    ComputeExpectedOffsetValues(arr_s32_computed_output_matrix, &signalRange, arr_s32_computed_offsets);
    GetStatusOutputAndOffsetMatrix(pipe_no, arr_s32_status_output_matrix, arr_s32_status_offsets);

    if
    (
        0 == CompareComputedAndStatusMatrices(
            arr_s32_computed_output_matrix,
            arr_s32_computed_offsets,
            arr_s32_status_output_matrix,
            arr_s32_status_offsets)
    )
    {
        s32_err = 0;
    }
    else
    {
        s32_err = 1;
    }


    LOS_Log("\n>> TestRGB2YUVcoder : error = %d\n", s32_err);
    return (s32_err);
}


float
ComputeContrast(
t_uint32    u32_contrast,
t_uint32    u32_max_contrast)
{
    LOS_Log("\n>> ComputeContrast\n");

    float   f_contrast = (( float ) u32_contrast) / 100;
    float   f_max_contrast = (( float ) u32_max_contrast) / 100;
    float   f_expected_contrast = (f_contrast > f_max_contrast) ? (f_max_contrast) : (f_contrast);

    LOS_Log("\n<< ComputeContrast  : contrast = %10.4f\n", f_expected_contrast);
    return (f_expected_contrast);
}


float
ComputeColorSaturation(
t_uint32    u32_color_saturation,
t_uint32    u32_max_color_saturation)
{
    LOS_Log("\n>> ComputeColorSaturation\n");

    float   f_color_saturation = (( float ) u32_color_saturation) / 100;
    float   f_max_color_saturation = (( float ) u32_max_color_saturation) / 100;
    float   f_expected_saturation = (f_color_saturation > f_max_color_saturation) ? (f_max_color_saturation) :
        (f_color_saturation);

    LOS_Log("\n<< ComputeColorSaturation  : color_saturation = %10.4f\n", f_expected_saturation);
    return (f_expected_saturation);
}


void
SetStockMatrixAndSignalRange(
t_uint32            u32_pipe_num,
t_uint32            u32_transform_type,
float               f_RgbToYuvStockMatrix[],
OutputSignalRange   *ptr_signal_range)
{
    t_uint32    u32_count = 0;
    LOS_Log(
    "\n>> SetStockMatrixAndSignalRange : u32_pipe_num = %u,  Transform type = %u, \n",
    u32_pipe_num,
    u32_transform_type);

    if (Transform_e_YCbCr_Rec709_FULL_RANGE == u32_transform_type)
    {
        for (u32_count = 0; u32_count < 9; u32_count++)
        {
            f_RgbToYuvStockMatrix[u32_count] = RgbToYuvStockMatrix_REC709[u32_count];
        }


        ptr_signal_range->u16_LumaExcursion = FULL_REC709_SignalRange.u16_LumaExcursion;
        ptr_signal_range->u16_LumaMidpointTimes2 = FULL_REC709_SignalRange.u16_LumaMidpointTimes2;
        ptr_signal_range->u16_ChromaExcursion = FULL_REC709_SignalRange.u16_ChromaExcursion;
        ptr_signal_range->u16_ChromaMidpointTimes2 = FULL_REC709_SignalRange.u16_ChromaMidpointTimes2;
    }
    else if (Transform_e_YCbCr_Custom == u32_transform_type)
    {
        for (u32_count = 0; u32_count < 9; u32_count++)
        {
            f_RgbToYuvStockMatrix[u32_count] = Custom_RgbToYuvStockMatrix[u32_pipe_num][u32_count];
        }


        ptr_signal_range->u16_LumaExcursion = Custom_SignalRange[u32_pipe_num].u16_LumaExcursion;
        ptr_signal_range->u16_LumaMidpointTimes2 = Custom_SignalRange[u32_pipe_num].u16_LumaMidpointTimes2;
        ptr_signal_range->u16_ChromaExcursion = Custom_SignalRange[u32_pipe_num].u16_ChromaExcursion;
        ptr_signal_range->u16_ChromaMidpointTimes2 = Custom_SignalRange[u32_pipe_num].u16_ChromaMidpointTimes2;
    }
    else if (Transform_e_YCbCr_JFIF == u32_transform_type)
    {
        for (u32_count = 0; u32_count < 9; u32_count++)
        {
            f_RgbToYuvStockMatrix[u32_count] = RgbToYuvStockMatrix_JFIF[u32_count];
        }


        ptr_signal_range->u16_LumaExcursion = FULL_JFIF_SignalRange.u16_LumaExcursion;
        ptr_signal_range->u16_LumaMidpointTimes2 = FULL_JFIF_SignalRange.u16_LumaMidpointTimes2;
        ptr_signal_range->u16_ChromaExcursion = FULL_JFIF_SignalRange.u16_ChromaExcursion;
        ptr_signal_range->u16_ChromaMidpointTimes2 = FULL_JFIF_SignalRange.u16_ChromaMidpointTimes2;
    }
    else
    {
        HandleInvalidTransform(u32_transform_type);
    }

#if ENABLE_RGB2YUVCODER_DEBUG_TRACES
    PrintMatrixFloat(f_RgbToYuvStockMatrix, 3, 3);

    LOS_Log(
    "u16_LumaExcursion = %u, u16_LumaMidpointTimes2 = %u\n",
    ptr_signal_range->u16_LumaExcursion,
    ptr_signal_range->u16_LumaMidpointTimes2);
    
    LOS_Log(
    "u16_ChromaExcursion = %u, u16_ChromaMidpointTimes2 = %u\n",
    ptr_signal_range->u16_ChromaExcursion,
    ptr_signal_range->u16_ChromaMidpointTimes2);
#endif    

    LOS_Log("\n<< SetStockMatrixAndSignalRange\n");
}


t_uint32
ApplyExposureAndSaturation(
float               f_contrast,
float               f_color_saturation,
float               f_RgbToYuvStockMatrix[],
OutputSignalRange   *ptr_SignalRange)
{
    float   f_lumaMultiplicationFactor = 0.0;
    float   f_chromaMultiplicationFactor = 0.0;

    LOS_Log("\n>> ApplyExposureAndSaturation\n");

    // <Hem> For the time being assume that ISP FW calculations for contrast and staturation are right one.
    f_lumaMultiplicationFactor = (( float ) ptr_SignalRange->u16_LumaExcursion) * f_contrast / K_MAX_INPUT_FROM_PIPE;
    f_chromaMultiplicationFactor = (( float ) ptr_SignalRange->u16_ChromaExcursion) *
        f_contrast *
        f_color_saturation /
        K_MAX_INPUT_FROM_PIPE;

    // apply contrast on first 2 luma coefficients
    f_RgbToYuvStockMatrix[0] = f_RgbToYuvStockMatrix[0] * f_lumaMultiplicationFactor;
    f_RgbToYuvStockMatrix[1] = f_RgbToYuvStockMatrix[1] * f_lumaMultiplicationFactor;
    f_RgbToYuvStockMatrix[2] = 1 - f_RgbToYuvStockMatrix[0] - f_RgbToYuvStockMatrix[1];

    // aply colour saturation on first 2 chroma coefficients of each row
    f_RgbToYuvStockMatrix[1 * 3 + 0] = f_RgbToYuvStockMatrix[1 * 3 + 0] * f_chromaMultiplicationFactor;
    f_RgbToYuvStockMatrix[1 * 3 + 1] = f_RgbToYuvStockMatrix[1 * 3 + 1] * f_chromaMultiplicationFactor;
    f_RgbToYuvStockMatrix[1 * 3 + 2] = 0 - f_RgbToYuvStockMatrix[1 * 3 + 0] - f_RgbToYuvStockMatrix[1 * 3 + 1];

    f_RgbToYuvStockMatrix[2 * 3 + 0] = f_RgbToYuvStockMatrix[2 * 3 + 0] * f_chromaMultiplicationFactor;
    f_RgbToYuvStockMatrix[2 * 3 + 1] = f_RgbToYuvStockMatrix[2 * 3 + 1] * f_chromaMultiplicationFactor;
    f_RgbToYuvStockMatrix[2 * 3 + 2] = 0 - f_RgbToYuvStockMatrix[2 * 3 + 0] - f_RgbToYuvStockMatrix[2 * 3 + 1];

#if ENABLE_RGB2YUVCODER_DEBUG_TRACES
    LOS_Log(
    "f_lumaMultiplicationFactor = %f, f_chromaMultiplicationFactor = %f\n",
    f_lumaMultiplicationFactor,
    f_chromaMultiplicationFactor);

    PrintMatrixFloat(f_RgbToYuvStockMatrix, 3, 3);
#endif

    LOS_Log("\n<< ApplyExposureAndSaturation\n");
    return (0);
}


t_uint32
ComputeLumaRowSum(
float               f_contrast,
OutputSignalRange   *ptr_SignalRange)
{
    float   f_opLumaExcursinoByIpExcursion = (( float ) ptr_SignalRange->u16_LumaExcursion) / K_MAX_INPUT_FROM_PIPE;
    float   f_lumaRowSum = 0.0;
    LOS_Log("\n>> GetLumaRowSum\n");

    f_lumaRowSum = F_MATRIX_SCALER * f_contrast * f_opLumaExcursinoByIpExcursion;
    LOS_Log("f_opLumaExcursinoByIpExcursion = %f, f_lumaRowSum = %f\n", f_opLumaExcursinoByIpExcursion, f_lumaRowSum);

    LOS_Log("\n<< GetLumaRowSum\n");
    return (GetRounded(f_lumaRowSum));
}


t_uint32
ComputeExpectedCoefficients(
t_uint32            u32_luma_row_sum,
OutputSignalRange   *ptr_SignalRange,
float               f_contrast,
float               f_color_saturation,
t_uint32            u32_scalar_multiplication_factor,
float               f_RgbToYuvStockMatrix[],
int                 *arr_s32_computed_output_matrix)
{
    float   f_lumaMultiplicationFactor = 0.0;
    float   f_chromaMultiplicationFactor = 0.0;

    LOS_Log("\n>> ComputeExpectedCoefficients\n");

    // <Hem> For the time being assume that ISP FW calculations for contrast and staturation are right one.
    f_lumaMultiplicationFactor = ( float ) (ptr_SignalRange->u16_LumaExcursion) * f_contrast / K_MAX_INPUT_FROM_PIPE;
    f_chromaMultiplicationFactor = ( float ) (ptr_SignalRange->u16_ChromaExcursion) *
        f_contrast *
        f_color_saturation /
        K_MAX_INPUT_FROM_PIPE;

    LOS_Log("f_lumaMultiplicationFactor :%f\n",f_lumaMultiplicationFactor);
    LOS_Log("f_chromaMultiplicationFactor :%f\n",f_chromaMultiplicationFactor);

    // apply contrast on first 2 luma coefficients
    arr_s32_computed_output_matrix[0] = GetRounded(f_RgbToYuvStockMatrix[0] * u32_scalar_multiplication_factor);
    arr_s32_computed_output_matrix[1] = GetRounded(f_RgbToYuvStockMatrix[1] * u32_scalar_multiplication_factor);
    arr_s32_computed_output_matrix[2] = u32_luma_row_sum -
        arr_s32_computed_output_matrix[0] -
        arr_s32_computed_output_matrix[1];

    // aply colour saturation on first 2 chroma coefficients of each row
    arr_s32_computed_output_matrix[1 * 3 + 0] = GetRounded(f_RgbToYuvStockMatrix[1 * 3 + 0] * u32_scalar_multiplication_factor);
    arr_s32_computed_output_matrix[1 * 3 + 1] = GetRounded(f_RgbToYuvStockMatrix[1 * 3 + 1] * u32_scalar_multiplication_factor);
    arr_s32_computed_output_matrix[1 * 3 + 2] = 0 -
        arr_s32_computed_output_matrix[1 * 3 + 0] -
        arr_s32_computed_output_matrix[1 * 3 + 1];

    arr_s32_computed_output_matrix[2 * 3 + 0] = GetRounded(f_RgbToYuvStockMatrix[2 * 3 + 0] * u32_scalar_multiplication_factor);
    arr_s32_computed_output_matrix[2 * 3 + 1] = GetRounded(f_RgbToYuvStockMatrix[2 * 3 + 1] * u32_scalar_multiplication_factor);
    arr_s32_computed_output_matrix[2 * 3 + 2] = 0 -
        arr_s32_computed_output_matrix[2 * 3 + 0] -
        arr_s32_computed_output_matrix[2 * 3 + 1];

#if ENABLE_RGB2YUVCODER_DEBUG_TRACES
    PrintMatrixInt(arr_s32_computed_output_matrix, 3, 3);
#endif

    LOS_Log("\n<< ComputeExpectedCoefficients\n");

    return (0);
}


t_uint32
ComputeExpectedOffsetValues(
int                 *arr_s32_computed_output_matrix,
OutputSignalRange   *ptr_SignalRange,
int                 *s32_expected_offset)
{
    t_uint32    u32_row_count = 0;
    t_uint32    u32_column_count = 0;
    int         s32_row_sum = 0;

    LOS_Log("\n>> ComputeExpectedOffsetValues\n");

    // compute luma offset
    for (u32_row_count = 0; u32_row_count < 3; u32_row_count++)
    {
        for (u32_column_count = 0; u32_column_count < 3; u32_column_count++)
        {
            s32_row_sum += arr_s32_computed_output_matrix[u32_row_count * 3 + u32_column_count];
        }


        if (0 == u32_row_count)
        {
            s32_expected_offset[u32_row_count] = (( int ) ptr_SignalRange->u16_LumaMidpointTimes2 - s32_row_sum) / 2;
        }
        else
        {
            s32_expected_offset[u32_row_count] = (( int ) ptr_SignalRange->u16_ChromaMidpointTimes2 - s32_row_sum) / 2;
        }

#if ENABLE_RGB2YUVCODER_DEBUG_TRACES 
        LOS_Log(
        "row number = %2d : s32_row_sum = %+d, offset = %+d\n",
        u32_row_count,
        s32_row_sum,
        s32_expected_offset[u32_row_count]);
#endif            

        s32_row_sum = 0;
    }

#if ENABLE_RGB2YUVCODER_DEBUG_TRACES     
    PrintMatrixInt(s32_expected_offset, 3, 1);
#endif

    LOS_Log("\n<< ComputeExpectedOffsetValues\n");
    return (0);
}


/*  Performs rounding off operation for +ve and -ve floating point values */
int
GetRounded(
float   f_Input)
{
    /*~~~~~~~~~~~~~~~~~~~*/
    float   f_Tmp;
    int     s16_Output = 0;

    /*~~~~~~~~~~~~~~~~~~~*/

    // dInput += (dInput < 0)? -0.5:0.5;
    // first check for 0.0
    // if ip is zero return 0.
    if (f_Input != 0.0)
    {
        if (f_Input < 0.0)
        {
            // -ve number
            f_Tmp = -0.5;
        }
        else
        {
            // +ve number
            f_Tmp = 0.5;
        }


        s16_Output = ( int ) (f_Input + f_Tmp);
    }


    return (s16_Output);
}


void
PrintMatrixFloat(
float       f_RgbToYuvStockMatrix[],
t_uint32    u32_row_count,
t_uint32    u32_column_count)
{
    t_uint32    u32_row_num = 0;
    t_uint32    u32_column_num = 0;

    LOS_Log("\n>> PrintMatrix\n");

    // <Hem> We are not using column count
    for (u32_row_num = 0; u32_row_num < u32_row_count; u32_row_num++)
    {
        for (u32_column_num = 0; u32_column_num < u32_column_count; u32_column_num++)
        {
            LOS_Log("%+10.6f      ", f_RgbToYuvStockMatrix[u32_row_num * 3 + u32_column_num]);
        }


        LOS_Log("\n");
    }


    LOS_Log("\n<< PrintMatrix\n");
}


void
PrintMatrixInt(
int         s32_RgbToYuvStockMatrix[],
t_uint32    u32_row_count,
t_uint32    u32_column_count)
{
    t_uint32    u32_row_num = 0;
    t_uint32    u32_column_num = 0;

    LOS_Log("\n>> PrintMatrix\n");

    // <Hem> We are not using column count
    for (u32_row_num = 0; u32_row_num < u32_row_count; u32_row_num++)
    {
        LOS_Log("row number  %d :", u32_row_num);

        for (u32_column_num = 0; u32_column_num < u32_column_count; u32_column_num++)
        {
            LOS_Log("%+10d      ", s32_RgbToYuvStockMatrix[u32_row_num * 3 + u32_column_num]);
        }


        LOS_Log("\n");
    }


    LOS_Log("\n<< PrintMatrix\n");
}


void
GetStatusOutputAndOffsetMatrix(
t_uint32    u32_pipe_num,
int         *s16_status_output_matrix,
int         *s16_status_offset_matrix)
{
    LOS_Log("\n>> GetStatusOutputAndOffsetMatrix\n");

    if (0 == u32_pipe_num)
    {
        s16_status_output_matrix[0] = ( short ) ITE_readPE(CE_OutputCoderMatrix_0_s16_w0_0_Byte0);
        s16_status_output_matrix[1] = ( short ) ITE_readPE(CE_OutputCoderMatrix_0_s16_w0_1_Byte0);
        s16_status_output_matrix[2] = ( short ) ITE_readPE(CE_OutputCoderMatrix_0_s16_w0_2_Byte0);
        s16_status_output_matrix[3] = ( short ) ITE_readPE(CE_OutputCoderMatrix_0_s16_w1_0_Byte0);
        s16_status_output_matrix[4] = ( short ) ITE_readPE(CE_OutputCoderMatrix_0_s16_w1_1_Byte0);
        s16_status_output_matrix[5] = ( short ) ITE_readPE(CE_OutputCoderMatrix_0_s16_w1_2_Byte0);
        s16_status_output_matrix[6] = ( short ) ITE_readPE(CE_OutputCoderMatrix_0_s16_w2_0_Byte0);
        s16_status_output_matrix[7] = ( short ) ITE_readPE(CE_OutputCoderMatrix_0_s16_w2_1_Byte0);
        s16_status_output_matrix[8] = ( short ) ITE_readPE(CE_OutputCoderMatrix_0_s16_w2_2_Byte0);

        s16_status_offset_matrix[0] = ( short ) ITE_readPE(CE_OutputCoderOffsetVector_0_s16_i0_Byte0);
        s16_status_offset_matrix[1] = ( short ) ITE_readPE(CE_OutputCoderOffsetVector_0_s16_i1_Byte0);
        s16_status_offset_matrix[2] = ( short ) ITE_readPE(CE_OutputCoderOffsetVector_0_s16_i2_Byte0);
    }
    else if (1 == u32_pipe_num)
    {
        s16_status_output_matrix[0] = ( short ) ITE_readPE(CE_OutputCoderMatrix_1_s16_w0_0_Byte0);
        s16_status_output_matrix[1] = ( short ) ITE_readPE(CE_OutputCoderMatrix_1_s16_w0_1_Byte0);
        s16_status_output_matrix[2] = ( short ) ITE_readPE(CE_OutputCoderMatrix_1_s16_w0_2_Byte0);
        s16_status_output_matrix[3] = ( short ) ITE_readPE(CE_OutputCoderMatrix_1_s16_w1_0_Byte0);
        s16_status_output_matrix[4] = ( short ) ITE_readPE(CE_OutputCoderMatrix_1_s16_w1_1_Byte0);
        s16_status_output_matrix[5] = ( short ) ITE_readPE(CE_OutputCoderMatrix_1_s16_w1_2_Byte0);
        s16_status_output_matrix[6] = ( short ) ITE_readPE(CE_OutputCoderMatrix_1_s16_w2_0_Byte0);
        s16_status_output_matrix[7] = ( short ) ITE_readPE(CE_OutputCoderMatrix_1_s16_w2_1_Byte0);
        s16_status_output_matrix[8] = ( short ) ITE_readPE(CE_OutputCoderMatrix_1_s16_w2_2_Byte0);

        s16_status_offset_matrix[0] = ( short ) ITE_readPE(CE_OutputCoderOffsetVector_1_s16_i0_Byte0);
        s16_status_offset_matrix[1] = ( short ) ITE_readPE(CE_OutputCoderOffsetVector_1_s16_i1_Byte0);
        s16_status_offset_matrix[2] = ( short ) ITE_readPE(CE_OutputCoderOffsetVector_1_s16_i2_Byte0);
    }
    else
    {
    }


    PrintMatrixInt(s16_status_output_matrix, 1, 9);
    PrintMatrixInt(s16_status_offset_matrix, 1, 3);

    LOS_Log("\n<< GetStatusOutputAndOffsetMatrix\n");
}


int
CompareComputedAndStatusMatrices(
int *arr_s32_computed_output_matrix,
int *arr_s32_computed_offsets,
int *arr_s32_status_output_matrix,
int *arr_s32_status_offsets)
{
    t_uint32    u32_error_count = 0;
    int         count = 0;
    char        comment[200];

    LOS_Log("\n>> CompareComputedAndStatusMatrices\n");

    for (count = 0; count < 9; count++)
    {
        if (arr_s32_computed_output_matrix[count] != arr_s32_status_output_matrix[count])
        {
            sprintf(
            comment,
            "ERR -> mismatch for output matrix coefficient for index <%u>: computed value = [%d], status = [%d]\n",
            count,
            arr_s32_computed_output_matrix[count],
            arr_s32_status_output_matrix[count]);
            MMTE_TEST_COMMENT(comment);

            u32_error_count++;
        }
    }


    for (count = 0; count < 3; count++)
    {
        if (arr_s32_computed_offsets[count] != arr_s32_status_offsets[count])
        {
            LOS_Log(
            "ERR -> mismatch for offset matrix for index <%u>: computed value = [%d], status = [%d]\n",
            count,
            arr_s32_computed_offsets[count],
            arr_s32_status_offsets[count]);

            u32_error_count++;
        }
    }


    LOS_Log("\n<< CompareComputedAndStatusMatrices : ERR count = %u\n", u32_error_count);
    return (u32_error_count);
}


void
ApplyCustomStockMatrix(
t_uint32    u32_pipe_num,
float       *f_arr_stock_matrix)
{
    LOS_Log("\n>> ApplyCustomStockMatrix\n");

    if (0 == u32_pipe_num)
    {
        ITE_writePE(CustomStockMatrix_0_f_StockMatrix_0_Byte0, *( volatile t_uint32 * ) &f_arr_stock_matrix[0]);
        ITE_writePE(CustomStockMatrix_0_f_StockMatrix_1_Byte0, *( volatile t_uint32 * ) &f_arr_stock_matrix[1]);
        ITE_writePE(CustomStockMatrix_0_f_StockMatrix_2_Byte0, *( volatile t_uint32 * ) &f_arr_stock_matrix[2]);
        ITE_writePE(CustomStockMatrix_0_f_StockMatrix_3_Byte0, *( volatile t_uint32 * ) &f_arr_stock_matrix[3]);
        ITE_writePE(CustomStockMatrix_0_f_StockMatrix_4_Byte0, *( volatile t_uint32 * ) &f_arr_stock_matrix[4]);
        ITE_writePE(CustomStockMatrix_0_f_StockMatrix_5_Byte0, *( volatile t_uint32 * ) &f_arr_stock_matrix[5]);
        ITE_writePE(CustomStockMatrix_0_f_StockMatrix_6_Byte0, *( volatile t_uint32 * ) &f_arr_stock_matrix[6]);
        ITE_writePE(CustomStockMatrix_0_f_StockMatrix_7_Byte0, *( volatile t_uint32 * ) &f_arr_stock_matrix[7]);
        ITE_writePE(CustomStockMatrix_0_f_StockMatrix_8_Byte0, *( volatile t_uint32 * ) &f_arr_stock_matrix[8]);
    }
    else if (1 == u32_pipe_num)
    {
        ITE_writePE(CustomStockMatrix_1_f_StockMatrix_0_Byte0, *( volatile t_uint32 * ) &f_arr_stock_matrix[0]);
        ITE_writePE(CustomStockMatrix_1_f_StockMatrix_1_Byte0, *( volatile t_uint32 * ) &f_arr_stock_matrix[1]);
        ITE_writePE(CustomStockMatrix_1_f_StockMatrix_2_Byte0, *( volatile t_uint32 * ) &f_arr_stock_matrix[2]);
        ITE_writePE(CustomStockMatrix_1_f_StockMatrix_3_Byte0, *( volatile t_uint32 * ) &f_arr_stock_matrix[3]);
        ITE_writePE(CustomStockMatrix_1_f_StockMatrix_4_Byte0, *( volatile t_uint32 * ) &f_arr_stock_matrix[4]);
        ITE_writePE(CustomStockMatrix_1_f_StockMatrix_5_Byte0, *( volatile t_uint32 * ) &f_arr_stock_matrix[5]);
        ITE_writePE(CustomStockMatrix_1_f_StockMatrix_6_Byte0, *( volatile t_uint32 * ) &f_arr_stock_matrix[6]);
        ITE_writePE(CustomStockMatrix_1_f_StockMatrix_7_Byte0, *( volatile t_uint32 * ) &f_arr_stock_matrix[7]);
        ITE_writePE(CustomStockMatrix_1_f_StockMatrix_8_Byte0, *( volatile t_uint32 * ) &f_arr_stock_matrix[8]);
    }
    else
    {
        HandleInvalidPipe(u32_pipe_num);
    }


    LOS_Log("\n<< ApplyCustomStockMatrix\n");
}


void
ApplyCustomSignalRange(
t_uint32            u32_pipe_num,
OutputSignalRange   *ptr_signal_range)
{
    LOS_Log("\n>> ApplyCustomSignalRange : pipe number = [%u]\n", u32_pipe_num);

    if (0 == u32_pipe_num)
    {
        ITE_writePE(CE_CustomTransformOutputSignalRange_0_u16_LumaExcursion_Byte0, ptr_signal_range->u16_LumaExcursion);
        ITE_writePE(
        CE_CustomTransformOutputSignalRange_0_u16_LumaMidpointTimes2_Byte0,
        ptr_signal_range->u16_LumaMidpointTimes2);
        ITE_writePE(
        CE_CustomTransformOutputSignalRange_0_u16_ChromaExcursion_Byte0,
        ptr_signal_range->u16_ChromaExcursion);
        ITE_writePE(
        CE_CustomTransformOutputSignalRange_0_u16_ChromaMidpointTimes2_Byte0,
        ptr_signal_range->u16_ChromaMidpointTimes2);
    }
    else if (1 == u32_pipe_num)
    {
        ITE_writePE(CE_CustomTransformOutputSignalRange_1_u16_LumaExcursion_Byte0, ptr_signal_range->u16_LumaExcursion);
        ITE_writePE(
        CE_CustomTransformOutputSignalRange_1_u16_LumaMidpointTimes2_Byte0,
        ptr_signal_range->u16_LumaMidpointTimes2);
        ITE_writePE(
        CE_CustomTransformOutputSignalRange_1_u16_ChromaExcursion_Byte0,
        ptr_signal_range->u16_ChromaExcursion);
        ITE_writePE(
        CE_CustomTransformOutputSignalRange_1_u16_ChromaMidpointTimes2_Byte0,
        ptr_signal_range->u16_ChromaMidpointTimes2);
    }
    else
    {
        HandleInvalidPipe(u32_pipe_num);
    }


    LOS_Log("\n<< ApplyCustomSignalRange\n");
}


void
HandleInvalidPipe(
t_uint32    u32_pipe_num)
{
    while (1)
    {
        LOS_Log("ERR : pipe number can not be <%d> \n", u32_pipe_num);
    }
}


void
HandleInvalidTransform(
int s32_transform_type)
{
    while (1)
    {
        LOS_Log("ERR : transform not supported by test case <%d> \n", s32_transform_type);
    }
}


void
Toggle_System_Coin(void)
{
    Coin_te control_coin,
            status_coin;

    LOS_Log("\nToggle System Coin.\n");

    control_coin = (Coin_te) ITE_readPE(SystemSetup_e_Coin_Ctrl_Byte0);
    status_coin = (Coin_te) ITE_readPE(SystemConfig_Status_e_Coin_Status_Byte0);

    if (control_coin != status_coin)
    {
        LOS_Log("Coins not in proper state to execute the command.\n");
        LOS_Log("Status and Control Coins should be equal before executing the command.\n");
        return;
    }


    ITE_writePE(SystemSetup_e_Coin_Ctrl_Byte0, !control_coin);
}


void Brightness_Control_Test (char *ap_pipe,char *step_size)
{

 t_sint32 br_range=FLOOR_MIN;
 t_uint32 br_step;
 
 char filename[255];
  
 br_step = ITE_ConvToInt16(step_size);

 if (0== strcmp(ap_pipe,"LRHR"))
 {
  LOS_Log("\nSTREAMING ON LR AND HR PIPES\n");
  ITE_LRHR_Prepare(&usecase,INFINITY);
  ITE_LRHR_Start(INFINITY);
 }
 else if(0== strcmp(ap_pipe,"LR"))
 {
  LOS_Log("\nSTREAMING ON LR PIPE\n");
  ITE_LR_Prepare(&usecase,INFINITY);
  ITE_LR_Start(INFINITY);
 }
 else
 {
  LOS_Log("\nSTREAMING ON HR PIPE\n");
  ITE_HR_Prepare(&usecase,INFINITY);
  ITE_HR_Start(INFINITY);
 }

 do {
     if (0== strcmp(ap_pipe,"LRHR"))
     {
     ITE_writePE(CE_LumaOffset_0_s16_LumaOffset_Byte0,br_range);
     ITE_writePE(CE_LumaOffset_1_s16_LumaOffset_Byte0,br_range);
     //toggle system coin
     Toggle_System_Coin();
     ITE_NMF_WaitStatReady(ISP_GLACE_STATS_READY);
     sprintf(filename,IMAGING_PATH "/%s_%d_",ap_pipe,(int)br_range);
     ITE_StoreinBMPFile(filename, &(GrabBufferLR[0]));
     ITE_StoreinBMPFile(filename, &(GrabBufferHR[0]));
     }
     else if(0== strcmp(ap_pipe,"LR"))
     {
     LOS_Log("LumaOffset Applied in pipe %s:%d\n",ap_pipe,(int)br_range);
     ITE_writePE(CE_LumaOffset_1_s16_LumaOffset_Byte0,br_range);
     //toggle system coin
     Toggle_System_Coin();
     ITE_NMF_WaitStatReady(ISP_GLACE_STATS_READY);
     sprintf(filename,IMAGING_PATH "/%s_%d_",ap_pipe,(int)br_range);
     ITE_StoreinBMPFile(filename, &(GrabBufferLR[0]));
     }
     else
     {
     LOS_Log("LumaOffset Applied in pipe %s:%d\n",ap_pipe,(int)br_range);
     ITE_writePE(CE_LumaOffset_0_s16_LumaOffset_Byte0,br_range);
     Toggle_System_Coin();
     ITE_NMF_WaitStatReady(ISP_GLACE_STATS_READY);
     sprintf(filename,IMAGING_PATH "/%s_%d_",ap_pipe,(int)br_range);
     ITE_StoreinBMPFile(filename, &(GrabBufferHR[0]));
     }
     br_range=br_range+br_step;
    } while (br_range<FLOOR_MAX);

 if (0== strcmp(ap_pipe,"LRHR"))
 {
 ITE_LRHR_Stop();
 ITE_LRHR_Free();
 }
 else if(0== strcmp(ap_pipe,"LR"))
 {
 ITE_LR_Stop();
 ITE_LR_Free();
 }
 else
 {
 ITE_HR_Stop();
 ITE_HR_Free();
 }
}
