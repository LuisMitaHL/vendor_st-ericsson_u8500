#include "ite_nmf_histogram_geometry.h"

#include "pictor_full.h"
#include "ite_platform.h"

static char     arr_histogram_configuration_string[200];
static t_uint32 g_u32_Test_Histogram_Geometry_test_counter = 0;
static t_uint32 g_u32_Set_Histogram_Geometry_test_counter = 0;
static t_uint32 g_u32_Get_Histogram_Geometry_Mode_test_counter = 0;

//static t_uint32 g_u32_Set_bms_size_counter = 0;
extern t_uint32 REG_Read_Val;

/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_Set_Histogram_Geometry_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_Set_Histogram_Geometry_cmd(
int     a_nb_args,
char    **ap_args)
{
    char    mess[200];

    sprintf(mess, "setHistogramGeometry_%d", ( int ) g_u32_Set_Histogram_Geometry_test_counter);
    g_u32_Set_Histogram_Geometry_test_counter++;

    MMTE_TEST_START(mess, "/ite_nmf/test_results_nreg/Histogram_geometry", "setHistogramGeometry");

    sprintf(mess, "Set histogram geometry mode");
    mmte_testNext(mess);

    switch (a_nb_args)
    {
        case 6:
            {
                Set_Histogram_Geometry(
                atoi(ap_args[1]),
                atof(ap_args[2]),
                atof(ap_args[3]),
                atof(ap_args[4]),
                atof(ap_args[5]));

                sprintf(
                arr_histogram_configuration_string,
                "mode_%d_size_fraction_%.3f_%.3f_offset_fraction_%.3f_%.3f",
                atoi(ap_args[1]),
                atof(ap_args[2]),
                atof(ap_args[3]),
                atof(ap_args[4]),
                atof(ap_args[5]));

                LOS_Log("test configuration is : %s\n", arr_histogram_configuration_string);
                break;
            }


        default:
            {
                LOS_Log(
                "Syntax: setHistogramGeometry  <mode> <f_hSizeFrac> <f_vSizeFrac> <f_hStartFrac> <f_vStartFrac>, sets value of histogram geometry mode \n");

                break;
            }
    }


    MMTE_TEST_PASSED();
    return (CMD_COMPLETE);
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_Get_Histogram_Geometry_Mode_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_Get_Histogram_Geometry_cmd(
int     a_nb_args,
char    **ap_args)
{
    char    mess[200];
    UNUSED(ap_args);

    sprintf(mess, "GetHistogramGeometryMode_%d", ( int ) g_u32_Get_Histogram_Geometry_Mode_test_counter);
    g_u32_Get_Histogram_Geometry_Mode_test_counter++;

    MMTE_TEST_START(mess, "/ite_nmf/test_results_nreg/Histogram_geometry", "GetHistogramGeometryMode");

    sprintf(mess, "Get histogram geometry mode test");
    mmte_testNext(mess);

    switch (a_nb_args)
    {
        case 1:     { Get_Histogram_Geometry(); break; }


        default:    { LOS_Log("Syntax: getHistogramGeometry , prints value of histogram geometry mode \n"); break; }
    }


    MMTE_TEST_PASSED();
    return (CMD_COMPLETE);
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_Test_Histogram_Geometry_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_Test_Histogram_Geometry_cmd(
int     a_nb_args,
char    **ap_args)
{
    char        mess[200];
    t_uint32    u32_result = 0;
    UNUSED(ap_args);
    /*
        sprintf(mess, "VerifyHistogramGeometry_%d", (int)g_u32_Test_Histogram_Geometry_test_counter);
        */

    /*
        sprintf(arr_histogram_configuration_string, "%d_mode_%d_block_size_%s_%s_ROI_fraction_%s_%s_gridSize_%s_%s", g_u32_Test_Histogram_Geometry_test_counter, ap_args[1], ap_args[2], ap_args[3], ap_args[4],
                                ap_args[5], ap_args[6], ap_args[7]);
        */
    sprintf(mess, "TestHistogramGeometry_%d", ( unsigned int ) g_u32_Test_Histogram_Geometry_test_counter);
    g_u32_Test_Histogram_Geometry_test_counter++;

    MMTE_TEST_START(mess, "/ite_nmf/test_results_nreg/Histogram_geometry", "Test Histogram_geometry_tests");

    sprintf(mess, "Testing for histogram geometry test\n");
    MMTE_TEST_NEXT(mess);

    MMTE_TEST_COMMENT(arr_histogram_configuration_string);

    LOS_Log("******************* Histogram geometry testing start ****************\n");

    switch (a_nb_args)
    {
        case 1:
            {
                u32_result = Test_Histogram_Geometry();
                break;
            }


        default:
            {
                LOS_Log("Syntax: testHistogramGeometry , Tests correctness of histogram geometry related PEs \n");
                break;
            }
    }


    LOS_Log("************************* Histogram geometry testing end**********************\n");

    if (0 == u32_result)
    {
        LOS_Log("Histogram geometry test passed.\n");    
        MMTE_TEST_PASSED();
        return (CMD_COMPLETE);
    }

    LOS_Log("Histogram geometry test failed.\n");    
    MMTE_TEST_FAILED();
    return (CMD_COMPLETE);
}


/* -----------------------------------------------------------------------
   FUNCTION : Get_Histogram_Geometry_Mode
   PURPOSE  :
   ------------------------------------------------------------------------ */
void
Get_Histogram_Geometry(void)
{

    float       f_temp_x = 0;
    float       f_temp_y = 0;

    t_uint32    u32_temp_x = 0;
    t_uint32    u32_temp_y = 0;

    t_uint32    u32_zoom_status_lla_woi_x = 0;
    t_uint32    u32_zoom_status_lla_woi_y = 0;

#if ENABLE_HISTOGRAM_GEOMETRY_DEBUG_TRACES
    t_uint32    u32_histogram_geometry_mode = 0;

    t_uint32    u32_hSize = 0;
    t_uint32    u32_vSize = 0;

    t_uint32    u32_X_startOffset = 0;
    t_uint32    u32_Y_startOffset = 0;

    float       f_hROISizeFrac = 0;
    float       f_vROISizeFrac = 0;

    float       f_hStartFrac = 0;
    float       f_vStartFrac = 0;

    LOS_Log("\n>> Get_Histogram_Geometry\n");

    u32_histogram_geometry_mode = ITE_readPE(HistStats_Status_e_StatisticsFov_Byte0);

    u32_X_startOffset = ITE_readPE(HistStats_Status_u16_HistOffsetX_Byte0);
    u32_Y_startOffset = ITE_readPE(HistStats_Status_u16_HistOffsetY_Byte0);

    u32_hSize = ITE_readPE(HistStats_Status_u16_HistSizeX_Byte0);
    u32_vSize = ITE_readPE(HistStats_Status_u16_HistSizeY_Byte0);

    u32_temp_x = ITE_readPE(HistStats_Ctrl_f_HistSizeRelativeToFOV_X_Byte0);
    f_hROISizeFrac = *(( float * ) &u32_temp_x);

    u32_temp_y = ITE_readPE(HistStats_Ctrl_f_HistSizeRelativeToFOV_Y_Byte0);
    f_vROISizeFrac = *(( float * ) &u32_temp_y);

    u32_temp_x = ITE_readPE(HistStats_Ctrl_f_HistOffsetRelativeToFOV_X_Byte0);
    f_hStartFrac = *(( float * ) &u32_temp_x);

    u32_temp_y = ITE_readPE(HistStats_Ctrl_f_HistOffsetRelativeToFOV_Y_Byte0);
    f_vStartFrac = *(( float * ) &u32_temp_y);

    if (StatisticsFov_e_Sensor == u32_histogram_geometry_mode)
    {
        LOS_Log("histogram geometry mode = StatisticsFov_e_Sensor\n");
    }
    else if (StatisticsFov_e_Master_Pipe == u32_histogram_geometry_mode)
    {
        LOS_Log("histogram geometry mode = StatisticsFov_e_Master_Pipe\n");
    }
    else
    {
        LOS_Log("Invalid histogram geometry mode. Value = %u\n", u32_histogram_geometry_mode);
    }

    LOS_Log("f_hStartFrac = %f, f_vStartFrac = %f\n", f_hStartFrac, f_vStartFrac);
    LOS_Log("f_hROISizeFrac = %f, f_vROISizeFrac = %f\n", f_hROISizeFrac, f_vROISizeFrac);
    LOS_Log("u32_X_startOffset = %u, u32_Y_startOffset = %u\n", u32_X_startOffset, u32_Y_startOffset);
    LOS_Log("u32_hSize = %u, u32_vSize = %u\n", u32_hSize, u32_vSize);

    LOS_Log("Other relevant information\n");

    u32_temp_x = ITE_readPE(DataPathStatus_e_Flag_Pipe0Enable_Byte0);
    if (0 != u32_temp_x)
    {
        LOS_Log("g_DataPathStatus.e_Flag_Pipe0Enable = TRUE\n");
    }


    u32_temp_x = ITE_readPE(DataPathStatus_e_Flag_Pipe1Enable_Byte0);
    if (0 != u32_temp_x)
    {
        LOS_Log("g_DataPathStatus.e_Flag_Pipe1Enable = TRUE\n");
    }


    u32_temp_x = ITE_readPE(DataPathStatus_e_Flag_BayerStore2Enable_Byte0);
    if (0 != u32_temp_x)
    {
        LOS_Log("g_DataPathStatus.e_Flag_BayerStore2Enable = TRUE\n");
    }
#endif

    u32_zoom_status_lla_woi_x = ITE_readPE(Zoom_Status_LLA_u16_woi_resX_Byte0);
    u32_zoom_status_lla_woi_y = ITE_readPE(Zoom_Status_LLA_u16_woi_resY_Byte0);

    u32_temp_x = ITE_readPE(PipeState_0_f_FOVX_Byte0);
    u32_temp_y = ITE_readPE(PipeState_0_f_FOVY_Byte0);
    f_temp_x = *(( float * ) &u32_temp_x);
    f_temp_y = *(( float * ) &u32_temp_y);
    LOS_Log("g_PipeState[0].f_FOVX = %f, g_PipeState[0].f_FOVY = %f\n", f_temp_x, f_temp_y);

    u32_temp_x = ITE_readPE(PipeState_1_f_FOVX_Byte0);
    u32_temp_y = ITE_readPE(PipeState_1_f_FOVY_Byte0);
    f_temp_x = *(( float * ) &u32_temp_x);
    f_temp_y = *(( float * ) &u32_temp_y);
    LOS_Log("g_PipeState[1].f_FOVX = %f, g_PipeState[1].f_FOVY = %f\n", f_temp_x, f_temp_y);

    u32_temp_x = ITE_readPE(Zoom_Status_f_FOVX_Byte0);
    u32_temp_y = ITE_readPE(Zoom_Status_f_FOVY_Byte0);
    f_temp_x = *(( float * ) &u32_temp_x);
    f_temp_y = *(( float * ) &u32_temp_y);
    LOS_Log("g_Zoom_Status.f_FOVX = %f, g_Zoom_Status.f_FOVY = %f\n", f_temp_x, f_temp_y);

    LOS_Log(
    "g_Zoom_Status_LLA.u16_woi_resX = %u, g_Zoom_Status_LLA.u16_woi_resY = %u\n",
    u32_zoom_status_lla_woi_x,
    u32_zoom_status_lla_woi_y);

    LOS_Log("<< Get_Histogram_Geometry\n");
}


/* -----------------------------------------------------------------------
   FUNCTION : Set_Histogram_Geometry
   PURPOSE  :
   ------------------------------------------------------------------------ */
void
Set_Histogram_Geometry(
t_uint32    u32_geometry_mode,
float       f_hSizeFrac,
float       f_vSizeFrac,
float       f_hStartFrac,
float       f_vStartFrac)
{
    t_uint32    u32_hSizeFrac = 0;
    t_uint32    u32_vSizeFrac = 0;
    t_uint32    u32_hROISizeFrac = 0;
    t_uint32    u32_vROISizeFrac = 0;

    LOS_Log("\n>> Set_Histogram_Geometry\n");

    u32_hSizeFrac = *((t_uint32 *) (&f_hSizeFrac));
    u32_vSizeFrac = *((t_uint32 *) (&f_vSizeFrac));

    u32_hROISizeFrac = *((t_uint32 *) (&f_hStartFrac));
    u32_vROISizeFrac = *((t_uint32 *) (&f_vStartFrac));

    ITE_writePE(HistStats_Ctrl_e_StatisticsFov_Byte0, u32_geometry_mode);

    ITE_writePE(HistStats_Ctrl_f_HistSizeRelativeToFOV_X_Byte0, u32_hSizeFrac);
    ITE_writePE(HistStats_Ctrl_f_HistSizeRelativeToFOV_Y_Byte0, u32_vSizeFrac);

    ITE_writePE(HistStats_Ctrl_f_HistOffsetRelativeToFOV_X_Byte0, u32_hROISizeFrac);
    ITE_writePE(HistStats_Ctrl_f_HistOffsetRelativeToFOV_Y_Byte0, u32_vROISizeFrac);

    LOS_Log("<< Set_Histogram_Geometry\n");
}


/* -----------------------------------------------------------------------
   FUNCTION : Test_Histogram_Geometry
   PURPOSE  :
   ------------------------------------------------------------------------ */
t_uint32
Test_Histogram_Geometry(void)
{
    t_uint32    u32_expected_roi_start_x = 0;
    t_uint32    u32_expected_roi_start_y = 0;
    t_uint32    u32_expected_size_x = 0;
    t_uint32    u32_expected_block_size_y = 0;

    t_uint32    u32_err_count = 0;

    LOS_Log("\n>> Test_Histogram_Geometry\n");

    ComputeExpectedHistogramGeometry(
    &u32_expected_roi_start_x,
    &u32_expected_roi_start_y,
    &u32_expected_size_x,
    &u32_expected_block_size_y);

    u32_err_count = CompareComputedAndStatusHistogramGeometry(
        u32_expected_roi_start_x,
        u32_expected_roi_start_y,
        u32_expected_size_x,
        u32_expected_block_size_y);

    LOS_Log("<< Test_Histogram_Geometry\n");
    return (u32_err_count);
}


/* -----------------------------------------------------------------------
   FUNCTION : ComputeExpectedHistogramGeometry
   PURPOSE  :
   ------------------------------------------------------------------------ */
void
ComputeExpectedHistogramGeometry(
t_uint32    *ptr_u32_expected_roi_start_x,
t_uint32    *ptr_u32_expected_roi_start_y,
t_uint32    *ptr_u32_expected_size_x,
t_uint32    *ptr_u32_expected_size_y)
{
    t_uint32    u32_geometry_mode = 0;

    t_uint32    u32_histogram_input_resolution_x = 0;
    t_uint32    u32_histogram_input_resolution_y = 0;

    t_uint32    u32_start_roi_fraction_x = 0;
    t_uint32    u32_start_roi_fraction_y = 0;
    t_uint32    u32_size_fraction_x = 0;
    t_uint32    u32_size_fraction_y = 0;
    float       f_start_roi_fraction_x = 0;
    float       f_start_roi_fraction_y = 0;
    float       f_size_fraction_x = 0;
    float       f_size_fraction_y = 0;

    float    f_pipe_offset_x = 0;
    float    f_pipe_offset_y = 0;

    float       f_prescale = 0;

    t_uint32    u32_zoom_status_lla_woi_x = 0;
    t_uint32    u32_zoom_status_lla_woi_y = 0;

    t_uint32    u32_zoom_status_lla_out_x = 0;
    t_uint32    u32_zoom_status_lla_out_y = 0;

    t_uint32    u32_zoom_status_fov_x = 0;
    t_uint32    u32_zoom_status_fov_y = 0;

    float       f_zoom_status_fov_x = 0;
    float       f_zoom_status_fov_y = 0;

    t_uint32    u32_flag_BMS2_active = 0;
    t_uint32    u32_flag_force_sensor_fov_mode = Flag_e_FALSE;

    LOS_Log("\n>> ComputeExpectedHistogramGeometry\n");

    // read histogram geometry mode
    u32_geometry_mode = ITE_readPE(HistStats_Status_e_StatisticsFov_Byte0);

    u32_start_roi_fraction_x = ITE_readPE(HistStats_Ctrl_f_HistOffsetRelativeToFOV_X_Byte0);
    f_start_roi_fraction_x = (*(( float * ) &u32_start_roi_fraction_x));

    u32_start_roi_fraction_y = ITE_readPE(HistStats_Ctrl_f_HistOffsetRelativeToFOV_Y_Byte0);
    f_start_roi_fraction_y = (*(( float * ) &u32_start_roi_fraction_y));

    u32_size_fraction_x = ITE_readPE(HistStats_Ctrl_f_HistSizeRelativeToFOV_X_Byte0);
    f_size_fraction_x = (*(( float * ) &u32_size_fraction_x));

    u32_size_fraction_y = ITE_readPE(HistStats_Ctrl_f_HistSizeRelativeToFOV_Y_Byte0);
    f_size_fraction_y = (*(( float * ) &u32_size_fraction_y));

    u32_zoom_status_lla_out_x = ITE_readPE(Zoom_Status_LLA_u16_output_resX_Byte0);
    u32_zoom_status_lla_out_y = ITE_readPE(Zoom_Status_LLA_u16_output_resY_Byte0);

    u32_zoom_status_lla_woi_x = ITE_readPE(Zoom_Status_LLA_u16_woi_resX_Byte0);
    u32_zoom_status_lla_woi_y = ITE_readPE(Zoom_Status_LLA_u16_woi_resY_Byte0);

    u32_zoom_status_fov_x = ITE_readPE(Zoom_Status_f_FOVX_Byte0);
    f_zoom_status_fov_x = *(( float * ) (&u32_zoom_status_fov_x));

    u32_zoom_status_fov_y = ITE_readPE(Zoom_Status_f_FOVY_Byte0);
    f_zoom_status_fov_y = *(( float * ) (&u32_zoom_status_fov_y));

    u32_flag_BMS2_active = ITE_readPE(DataPathStatus_e_Flag_BayerStore2Enable_Byte0);

    // sanity check
    if ((f_size_fraction_x + f_start_roi_fraction_x) > 1.0)
    {
        f_start_roi_fraction_x = 1.0 - f_size_fraction_x;
    }


    if ((f_size_fraction_y + f_start_roi_fraction_y) > 1.0)
    {
        f_start_roi_fraction_y = 1.0 - f_size_fraction_y;
    }


    if
    (
        (u32_flag_BMS2_active)
    &&  ((u32_zoom_status_lla_woi_x < f_zoom_status_fov_x) || (u32_zoom_status_lla_woi_y < f_zoom_status_fov_y))
    )
    {
        u32_flag_force_sensor_fov_mode = Flag_e_TRUE;
    }

#if ENABLE_HISTOGRAM_GEOMETRY_DEBUG_TRACES
    LOS_Log("u32_geometry_mode = %u\n", u32_geometry_mode);
    
    LOS_Log(
    "f_start_roi_fraction_x = %f, f_start_roi_fraction_y = %f\n",
    f_start_roi_fraction_x,
    f_start_roi_fraction_y);
    
    LOS_Log("f_size_fraction_x = %f, f_size_fraction_y = %f\n", f_size_fraction_x, f_size_fraction_y);
#endif

    if ((StatisticsFov_e_Sensor == u32_geometry_mode) || (Flag_e_TRUE == u32_flag_force_sensor_fov_mode))
    {
        u32_histogram_input_resolution_x = u32_zoom_status_lla_out_x;
        *ptr_u32_expected_roi_start_x = f_start_roi_fraction_x * u32_histogram_input_resolution_x;
        *ptr_u32_expected_roi_start_x = (*ptr_u32_expected_roi_start_x);

        u32_histogram_input_resolution_y = u32_zoom_status_lla_out_y;
        *ptr_u32_expected_roi_start_y = f_start_roi_fraction_y * u32_histogram_input_resolution_y;
        *ptr_u32_expected_roi_start_y = (*ptr_u32_expected_roi_start_y);
    }
    else if (StatisticsFov_e_Master_Pipe == u32_geometry_mode)
    {
        // A. calculate pre-scale factor
        f_prescale = (( float ) u32_zoom_status_lla_woi_x) / u32_zoom_status_lla_out_x;

        // 1. find fov offsets
        f_pipe_offset_x = (u32_zoom_status_lla_out_x - f_zoom_status_fov_x / f_prescale) / 2;
        f_pipe_offset_y = (u32_zoom_status_lla_out_y - f_zoom_status_fov_y / f_prescale) / 2;

        // 2. compute histogram input resolution taking prescale into consideration
        u32_histogram_input_resolution_x = f_zoom_status_fov_x / f_prescale;
        u32_histogram_input_resolution_y = f_zoom_status_fov_y / f_prescale;

        *ptr_u32_expected_roi_start_x = f_start_roi_fraction_x * u32_histogram_input_resolution_x + f_pipe_offset_x;
        *ptr_u32_expected_roi_start_x = (*ptr_u32_expected_roi_start_x);

        *ptr_u32_expected_roi_start_y = f_start_roi_fraction_y * u32_histogram_input_resolution_y + f_pipe_offset_y;
        *ptr_u32_expected_roi_start_y = (*ptr_u32_expected_roi_start_y);
        
#if ENABLE_HISTOGRAM_GEOMETRY_DEBUG_TRACES
        LOS_Log(
        "u32_zoom_status_lla_woi_x = %u, u32_zoom_status_lla_out_x = %u\n",
        u32_zoom_status_lla_woi_x,
        u32_zoom_status_lla_out_x);
        
        LOS_Log("f_prescale = %f\n", f_prescale);
        
        LOS_Log(
        "u32_histogram_input_resolution_x = %u, u32_histogram_input_resolution_y = %u\n",
        u32_histogram_input_resolution_x,
        u32_histogram_input_resolution_y);
#endif        
    }
    else
    {
        while (1)
        {
            LOS_Log("invalid histogram geometry mode\n");
        }
    }


    *ptr_u32_expected_size_x = f_size_fraction_x * u32_histogram_input_resolution_x;
    *ptr_u32_expected_size_y = f_size_fraction_y * u32_histogram_input_resolution_y;

    *ptr_u32_expected_size_x = (*ptr_u32_expected_size_x) & (~0x1);
    *ptr_u32_expected_size_y = (*ptr_u32_expected_size_y) & (~0x1);

    *ptr_u32_expected_roi_start_x = (*ptr_u32_expected_roi_start_x) & (~0x1);
    *ptr_u32_expected_roi_start_y = (*ptr_u32_expected_roi_start_y) & (~0x1);
    
#if ENABLE_HISTOGRAM_GEOMETRY_DEBUG_TRACES
    LOS_Log(
    "*ptr_u32_expected_roi_start_x = %u, *ptr_u32_expected_roi_start_y = %u\n",
    *ptr_u32_expected_roi_start_x,
    *ptr_u32_expected_roi_start_y);

    LOS_Log(
    "*ptr_u32_expected_size_x = %u, *ptr_u32_expected_size_y = %u\n",
    *ptr_u32_expected_size_x,
    *ptr_u32_expected_size_y);
#endif

    LOS_Log("<< ComputeExpectedHistogramGeometry\n");
}


/* -----------------------------------------------------------------------
   FUNCTION : CompareComputedAndStatusHistogramGeometry
   PURPOSE  :
   ------------------------------------------------------------------------ */
t_uint32
CompareComputedAndStatusHistogramGeometry(
t_uint32    u32_expected_roi_start_x,
t_uint32    u32_expected_roi_start_y,
t_uint32    u32_expected_size_x,
t_uint32    u32_expected_size_y)
{
    t_uint32    u32_status_roi_start_x = 0;
    t_uint32    u32_status_roi_start_y = 0;
    t_uint32    u32_status_size_x = 0;
    t_uint32    u32_status_size_y = 0;

    t_uint32    errorCount = 0;
    char        comment[200];

    LOS_Log("\n>> CompareComputedAndStatusHistogramGeometry\n");

    u32_status_roi_start_x = ITE_readPE(HistStats_Status_u16_HistOffsetX_Byte0);
    u32_status_roi_start_y = ITE_readPE(HistStats_Status_u16_HistOffsetY_Byte0);

    u32_status_size_x = ITE_readPE(HistStats_Status_u16_HistSizeX_Byte0);
    u32_status_size_y = ITE_readPE(HistStats_Status_u16_HistSizeY_Byte0);

    if (u32_expected_roi_start_x != u32_status_roi_start_x)
    {
        errorCount++;
        sprintf(
        comment,
        "ERR : u32_expected_roi_start_x = %u, u32_status_roi_start_x = %u\n",
        ( unsigned int ) u32_expected_roi_start_x,
        ( unsigned int ) u32_status_roi_start_x);
        MMTE_TEST_COMMENT(comment);
    }


    if (u32_expected_roi_start_y != u32_status_roi_start_y)
    {
        errorCount++;
        sprintf(
        comment,
        "ERR : u32_expected_roi_start_y = %u, u32_status_roi_start_y = %u\n",
        ( unsigned int ) u32_expected_roi_start_y,
        ( unsigned int ) u32_status_roi_start_y);
        MMTE_TEST_COMMENT(comment);
    }


    if (u32_expected_size_x != u32_status_size_x)
    {
        errorCount++;
        sprintf(
        comment,
        "ERR : u32_status_size_x = %u, u32_expected_size_x = %u\n",
        ( unsigned int ) u32_status_size_x,
        ( unsigned int ) u32_expected_size_x);
        MMTE_TEST_COMMENT(comment);
    }


    if (u32_expected_size_y != u32_status_size_y)
    {
        errorCount++;
        sprintf(
        comment,
        "ERR : u32_status_size_y = %u, u32_expected_size_y = %u\n",
        ( unsigned int ) u32_status_size_y,
        ( unsigned int ) u32_expected_size_y);
        MMTE_TEST_COMMENT(comment);
    }


    LOS_Log("<< CompareComputedAndStatusHistogramGeometry\n");
    return (errorCount);
}

