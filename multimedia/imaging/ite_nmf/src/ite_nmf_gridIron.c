/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include <ite_nmf_gridiron.h>

#define VER_FLIP_VALUE                  (Flag_e_FALSE)
#define HOR_FLIP_VALUE                  (Flag_e_FALSE)
#define K_REFERENCE_CAST_0              (450.247636)
#define K_REFERENCE_CAST_1              (363.504180)
#define K_REFERENCE_CAST_2              (274.047684)
#define K_REFERENCE_CAST_3              (146.198830)
#define K_TEST_LIVE_CAST_1              (274.047684)
#define K_TEST_LIVE_CAST_2              (275.271629)
#define K_TEST_LIVE_CAST_3              (276.405330)
#define K_INDEX_LIVE_CAST_PROGRAMMED    (0)
#define K_INDEX_LIVE_CAST_EXPECTED      (1)

static t_uint32 g_u32_counter_gridiron_conf = 0;
static t_uint32 g_u32_counter_gridiron_test = 0;

extern void     ITE_NMF_GlaceHistoExpStatsRequestAndWait (void);

/*
t_cmd_list  ite_cmd_list_gridiron[] =
{
    { "ConfigureGridIron", C_ite_dbg_ConfigureGridIron_cmd, "ConfigureGridIron : Configures grid iron \n" }
    { "TestGridIron", C_ite_dbg_TestGridIron_cmd, "TestGridIron : Tests GridIron \n" }
};
*/
void
Enable_Gridiron(
t_bool  enableBit)
{
    ITE_writePE(GridironControl_e_Flag_Enable_Byte0, enableBit);
}


int
TestGridironLiveCast(void)
{
    //  f_GridIronLiveCast[index][0] = Value of live cast to program ; f_GridIronLiveCast[index][0] = expected value of live cast
    float       f_GridIronLiveCast[][2] =
    {
        { K_REFERENCE_CAST_0 + 10,
                K_REFERENCE_CAST_0 },
                { K_REFERENCE_CAST_0,
                K_REFERENCE_CAST_0 },
                { K_REFERENCE_CAST_0 - 10,
                K_REFERENCE_CAST_0 - 10 },
                { K_REFERENCE_CAST_1 + 10,
                K_REFERENCE_CAST_1 + 10 },
                { K_REFERENCE_CAST_1,
                K_REFERENCE_CAST_1 },
                { K_REFERENCE_CAST_1 - 10,
                K_REFERENCE_CAST_1 - 10 },
                { K_REFERENCE_CAST_2 + 10,
                K_REFERENCE_CAST_2 + 10 },
                { K_REFERENCE_CAST_2,
                K_REFERENCE_CAST_2 },
                { K_REFERENCE_CAST_2 - 10,
                K_REFERENCE_CAST_2 - 10 },
                { K_REFERENCE_CAST_3 + 10,
                K_REFERENCE_CAST_3 + 10 },
                { K_REFERENCE_CAST_3,
                K_REFERENCE_CAST_3 },
                { K_REFERENCE_CAST_3 - 10,
                K_REFERENCE_CAST_3 },
                { K_TEST_LIVE_CAST_1,
                K_REFERENCE_CAST_2 },

                // because K_TEST_LIVE_CAST_1==K_REFERENCE_CAST_2
        { K_TEST_LIVE_CAST_2,
                K_TEST_LIVE_CAST_2 },
                { K_TEST_LIVE_CAST_3,
                K_TEST_LIVE_CAST_3 },
                };

    t_uint32    u32_StatusLiveCast = 0;
    float       f_StatusLiveCast = 0;
    t_uint32    count = 0;
    t_uint32    failed_Iteration_count = 0;
    char        comment[200];
    t_uint32    status_flag_cast0 = 0;
    t_uint32    status_flag_cast1 = 0;
    t_uint32    status_flag_cast2 = 0;
    t_uint32    status_flag_cast3 = 0;
    t_uint32    status_phase = 0;
    t_uint32    status_log_grid_pitch = 0;
    t_uint32    control_reference_cast_count = 0;
    t_uint32    u32_control_live_cast = 0;
    float       f_control_live_cast = 0;
    t_uint32    expected_flag_cast0 = 0;
    t_uint32    expected_flag_cast1 = 0;
    t_uint32    expected_flag_cast2 = 0;
    t_uint32    expected_flag_cast3 = 0;
    t_uint32    expected_phase = 0;
    float       f_expected_live_cast = 0;

    for (count = 0; count < (sizeof(f_GridIronLiveCast)) / (2 * sizeof(float)); count++)
    {
        ITE_writePE(
        GridironControl_f_LiveCast_Byte0,
        *(( t_uint32 * ) &f_GridIronLiveCast[count][K_INDEX_LIVE_CAST_PROGRAMMED]));

        if
        (
            HostInterfaceHighLevelState_e_RUNNING == ITE_readPE(
                HostInterface_Status_e_HostInterfaceHighLevelState_Byte0)
        )
        {
            ITE_NMF_GlaceHistoExpStatsRequestAndWait();
        }

        while (ITE_readPE(SystemSetup_e_Coin_Ctrl_Byte0) != ITE_readPE(SystemConfig_Status_e_Coin_Status_Byte0))
            ;

        u32_StatusLiveCast = ITE_readPE(GridironStatus_f_LiveCast_Byte0);
        status_flag_cast0 = ITE_readPE(GridironStatus_e_Flag_Cast0_Byte0);
        status_flag_cast1 = ITE_readPE(GridironStatus_e_Flag_Cast1_Byte0);
        status_flag_cast2 = ITE_readPE(GridironStatus_e_Flag_Cast2_Byte0);
        status_flag_cast3 = ITE_readPE(GridironStatus_e_Flag_Cast3_Byte0);
        control_reference_cast_count = ITE_readPE(GridironControl_u8_Active_ReferenceCast_Count_Byte0);

        status_phase = ITE_readPE(GridironStatus_u16_Phase_Byte0);
        status_log_grid_pitch = ITE_readPE(GridironStatus_u8_LogGridPitch_Byte0);

        f_StatusLiveCast = *(( float * ) &u32_StatusLiveCast);

        u32_control_live_cast = ITE_readPE(GridironControl_f_LiveCast_Byte0);
        f_control_live_cast = *(( float * ) &u32_control_live_cast);

        // Stack corruptino occurrs when following function call is done.
        GetExpectedParameterValues(
        control_reference_cast_count,
        K_REFERENCE_CAST_0,
        K_REFERENCE_CAST_1,
        K_REFERENCE_CAST_2,
        K_REFERENCE_CAST_3,
        f_control_live_cast,
        &expected_flag_cast0,
        &expected_flag_cast1,
        &expected_flag_cast2,
        &expected_flag_cast3,
        &expected_phase,
        &f_expected_live_cast);

        sprintf(
        comment,
        "\n\ncount = %d\nprogrammed f_GridIronLiveCast = %f, expected liveCast value = %f, retrieved f_StatusLiveCast = %f\n",
        ( int ) count,
        f_GridIronLiveCast[count][K_INDEX_LIVE_CAST_PROGRAMMED],
        f_GridIronLiveCast[count][K_INDEX_LIVE_CAST_EXPECTED],
        f_StatusLiveCast);

        mmte_testComment(comment);
        sprintf(
        comment,
        "g_GridironStatus.e_Flag_Cast0 = %d, g_GridironStatus.e_Flag_Cast1 = %d, g_GridironStatus.e_Flag_Cast2 = %d, g_GridironStatus.e_Flag_Cast3 = %d, status_phase = %u, status_log_grid_pitch =%u\n\n",
        ( int ) status_flag_cast0,
        ( int ) status_flag_cast1,
        ( int ) status_flag_cast2,
        ( int ) status_flag_cast3,
        ( unsigned int ) status_phase,
        ( unsigned int ) status_log_grid_pitch);
        mmte_testComment(comment);

        if (expected_flag_cast0 != status_flag_cast0)
        {
            sprintf(
            comment,
            "ERR : count = %u, expected_flag_cast0 = %u, status_flag_cast0 = %u\n",
            ( unsigned int ) count,
            ( unsigned int ) expected_flag_cast0,
            ( unsigned int ) status_flag_cast0);
            MMTE_TEST_COMMENT(comment);
        }


        if (expected_flag_cast1 != status_flag_cast1)
        {
            sprintf(
            comment,
            "ERR : count = %u, expected_flag_cast1 = %u, status_flag_cast1 = %u\n",
            ( unsigned int ) count,
            ( unsigned int ) expected_flag_cast1,
            ( unsigned int ) status_flag_cast1);
            MMTE_TEST_COMMENT(comment);
        }


        if (expected_flag_cast2 != status_flag_cast2)
        {
            sprintf(
            comment,
            "ERR : count = %u, expected_flag_cast2 = %u, status_flag_cast2 = %u\n",
            ( unsigned int ) count,
            ( unsigned int ) expected_flag_cast2,
            ( unsigned int ) status_flag_cast2);
            MMTE_TEST_COMMENT(comment);
        }


        if (expected_flag_cast3 != status_flag_cast3)
        {
            sprintf(
            comment,
            "ERR : count = %u, expected_flag_cast3 = %u, status_flag_cast3 = %u\n",
            ( unsigned int ) count,
            ( unsigned int ) expected_flag_cast3,
            ( unsigned int ) status_flag_cast3);
            MMTE_TEST_COMMENT(comment);
        }


        if (expected_phase != status_phase)
        {
            sprintf(
            comment,
            "ERR : count = %u, expected_phase = %u, status_phase = %u\n",
            ( unsigned int ) count,
            ( unsigned int ) expected_phase,
            ( unsigned int ) status_phase);
            MMTE_TEST_COMMENT(comment);
        }


        if (f_expected_live_cast != f_StatusLiveCast)
        {
            sprintf(
            comment,
            "ERR : count = %u, f_expected_live_cast = %f, f_StatusLiveCast = %f\n",
            ( unsigned int ) count,
            f_expected_live_cast,
            f_StatusLiveCast);
            MMTE_TEST_COMMENT(comment);
        }
    }


    LOS_Log("failed_Iteration_count = %u\n", failed_Iteration_count);
    return (failed_Iteration_count);
}


void
Configure_Gridiron_StaticPrarams(
t_uint32    pixel_order,
t_uint32    reference_cast_count)
{
    /// Default Value for Cast Position0
    float   f_GridIronCastPosition0 = K_REFERENCE_CAST_0;

    /// Default Value for Cast Position1
    float   f_GridIronCastPosition1 = K_REFERENCE_CAST_1;

    /// Default Value for Cast Position2
    float   f_GridIronCastPosition2 = K_REFERENCE_CAST_2;

    /// Default Value for Cast Position3
    float   f_GridIronCastPosition3 = K_REFERENCE_CAST_3;

    LOS_Log("=====================GRID IRON STATIC CONFIGURAION=====================\n");

    LOS_Log("pixel_order = %d, reference_cast_count = %d\n", pixel_order, reference_cast_count);

    // <Hem> IEEE float conversion result was wrong when rValue of below expression is put in the 2nd argument of the ITE_writePE() being done below.
    // Therefore do casting below
    t_uint32    u32_GridIronCastPosition0 = *(( t_uint32 * ) &f_GridIronCastPosition0);
    t_uint32    u32_GridIronCastPosition1 = *(( t_uint32 * ) &f_GridIronCastPosition1);
    t_uint32    u32_GridIronCastPosition2 = *(( t_uint32 * ) &f_GridIronCastPosition2);
    t_uint32    u32_GridIronCastPosition3 = *(( t_uint32 * ) &f_GridIronCastPosition3);

    // set reference casts
    ITE_writePE(GridironControl_f_CastPosition0_Byte0, u32_GridIronCastPosition0);
    ITE_writePE(GridironControl_f_CastPosition1_Byte0, u32_GridIronCastPosition1);
    ITE_writePE(GridironControl_f_CastPosition2_Byte0, u32_GridIronCastPosition2);
    ITE_writePE(GridironControl_f_CastPosition3_Byte0, u32_GridIronCastPosition3);

    // set active reference cast count to 3
    ITE_writePE(GridironControl_u8_Active_ReferenceCast_Count_Byte0, reference_cast_count);

    // set gridIron grid width and height values
    ITE_writePE(GridironControl_u16_GridWidth_Byte0, 65);
    ITE_writePE(GridironControl_u16_GridHeight_Byte0, 49);

    // set verticle and horizontal flip values
    ITE_writePE(GridironControl_e_Flag_VerFlip_Byte0, VER_FLIP_VALUE);
    ITE_writePE(GridironControl_e_Flag_HorFlip_Byte0, HOR_FLIP_VALUE);

    ITE_writePE(GridironControl_e_PixelOrder_Byte0, pixel_order);

    Enable_Gridiron(Flag_e_TRUE);
    LOS_Log("=====================================================================\n");
}

#if ENABLE_GRIDIRON_DEBUG_TRACES
void
Print_Gridiron_Parameters(void)
{


    t_uint32    status_live_cast = 0;
    t_uint32    status_hscale = 0;
    t_uint32    status_vscale = 0;
    t_uint32    status_hscale_x256 = 0;
    t_uint32    status_vscale_x256 = 0;
    t_uint32    status_crop_hstart = 0;
    t_uint32    status_crop_vstart = 0;
    t_uint32    status_image_hsize = 0;
    t_uint32    status_image_vsize = 0;
    t_uint32    status_sensor_hsize = 0;
    t_uint32    status_log_grid_pitch = 0;
    t_uint32    status_flag_cast0 = 0;
    t_uint32    status_flag_cast1 = 0;
    t_uint32    status_flag_cast2 = 0;
    t_uint32    status_flag_cast3 = 0;
    t_uint32    status_phase = 0;
    t_uint32    status_enable_flag = 0;
    t_uint32    status_flag_vert_flip = 0;
    t_uint32    status_horiz_flip = 0;
    t_uint32    status_reference_cast_count = 0;

    t_uint32    control_live_cast = 0;
    t_uint32    control_refrence_cast_0 = 0;
    t_uint32    control_refrence_cast_1 = 0;
    t_uint32    control_refrence_cast_2 = 0;
    t_uint32    control_refrence_cast_3 = 0;
    t_uint32    control_grid_width = 0;
    t_uint32    control_grid_height = 0;
    t_uint32    conrol_enable = 0;
    t_uint32    control_ver_flip = 0;
    t_uint32    control_hor_flip = 0;
    t_uint32    control_reference_cast_count = 0;
    t_uint32    control_pixel_order = 0;

    if (Flag_e_TRUE == ITE_readPE(GridironStatus_e_Flag_Enable_Byte0))
    {
        LOS_Log("GridIron is enabled\n");
        status_live_cast = ITE_readPE(GridironStatus_f_LiveCast_Byte0);
        status_hscale = ITE_readPE(GridironStatus_f_Sensor_HScale_Byte0);
        status_vscale = ITE_readPE(GridironStatus_f_Sensor_VScale_Byte0);
        status_hscale_x256 = ITE_readPE(GridironStatus_u16_Sensor_HScale_x256_Byte0);
        status_vscale_x256 = ITE_readPE(GridironStatus_u16_Sensor_VScale_x256_Byte0);
        status_crop_hstart = ITE_readPE(GridironStatus_u16_Crop_HStart_Byte0);
        status_crop_vstart = ITE_readPE(GridironStatus_u16_Crop_VStart_Byte0);
        status_image_hsize = ITE_readPE(GridironStatus_u16_Image_HSize_Byte0);
        status_image_vsize = ITE_readPE(GridironStatus_u16_Image_VSize_Byte0);
        status_sensor_hsize = ITE_readPE(GridironStatus_u16_Sensor_HSize_Byte0);
        status_log_grid_pitch = ITE_readPE(GridironStatus_u8_LogGridPitch_Byte0);
        status_flag_cast0 = ITE_readPE(GridironStatus_e_Flag_Cast0_Byte0);
        status_flag_cast1 = ITE_readPE(GridironStatus_e_Flag_Cast1_Byte0);
        status_flag_cast2 = ITE_readPE(GridironStatus_e_Flag_Cast2_Byte0);
        status_flag_cast3 = ITE_readPE(GridironStatus_e_Flag_Cast3_Byte0);
        status_phase = ITE_readPE(GridironStatus_u16_Phase_Byte0);
        status_enable_flag = ITE_readPE(GridironStatus_e_Flag_Enable_Byte0);
        status_flag_vert_flip = ITE_readPE(GridironStatus_e_Flag_VerFlip_Byte0);
        status_horiz_flip = ITE_readPE(GridironStatus_e_Flag_HorFlip_Byte0);
        status_reference_cast_count = ITE_readPE(GridironControl_u8_Active_ReferenceCast_Count_Byte0);

        control_live_cast = ITE_readPE(GridironControl_f_LiveCast_Byte0);
        control_refrence_cast_0 = ITE_readPE(GridironControl_f_CastPosition0_Byte0);
        control_refrence_cast_1 = ITE_readPE(GridironControl_f_CastPosition1_Byte0);
        control_refrence_cast_2 = ITE_readPE(GridironControl_f_CastPosition2_Byte0);
        control_refrence_cast_3 = ITE_readPE(GridironControl_f_CastPosition3_Byte0);
        control_grid_width = ITE_readPE(GridironControl_u16_GridWidth_Byte0);
        control_grid_height = ITE_readPE(GridironControl_u16_GridHeight_Byte0);
        conrol_enable = ITE_readPE(GridironControl_e_Flag_Enable_Byte0);
        control_ver_flip = ITE_readPE(GridironControl_e_Flag_VerFlip_Byte0);
        control_hor_flip = ITE_readPE(GridironControl_e_Flag_HorFlip_Byte0);
        control_reference_cast_count = ITE_readPE(GridironControl_u8_Active_ReferenceCast_Count_Byte0);
        control_pixel_order = ITE_readPE(GridironControl_e_PixelOrder_Byte0);


        LOS_Log("Gridiron control parameters\n");
        LOS_Log("Ctrl : GridironControl.f_LiveCast = %f\n", *(( float * ) &control_live_cast));
        LOS_Log("Ctrl : GridironControl.f_RefCast_0 = %f\n", *(( float * ) &control_refrence_cast_0));
        LOS_Log("Ctrl : GridironControl.f_RefCast_1 = %f\n", *(( float * ) &control_refrence_cast_1));
        LOS_Log("Ctrl : GridironControl.f_RefCast_2 = %f\n", *(( float * ) &control_refrence_cast_2));
        LOS_Log("Ctrl : GridironControl.f_RefCast_3 = %f\n", *(( float * ) &control_refrence_cast_3));
        LOS_Log("Ctrl : GridironControl.u32_Grid_Width = %u\n", control_grid_width);
        LOS_Log("Ctrl : GridironControl.u32_Grid_Height = %u\n", control_grid_height);
        LOS_Log("Ctrl : GridironControl.e_Enable= %u\n", conrol_enable);
        LOS_Log("Ctrl : GridironControl.u8_ver_flip= %u\n", control_ver_flip);
        LOS_Log("Ctrl : GridironControl.u8_hor_flip= %u\n", control_hor_flip);
        LOS_Log("Ctrl : GridironControl.u8_ref_cast_count= %u\n", control_reference_cast_count);
        LOS_Log("Ctrl : GridironControl.e_pixel_order= %u\n", control_pixel_order);

        LOS_Log("Gridiron status parameters\n");
        LOS_Log("Status : GridironStatus.f_LiveCast = %f\n", *(( float * ) &status_live_cast));
        LOS_Log("Status : GridironStatus.f_Sensor_HScale = %f\n", *(( float * ) &status_hscale));
        LOS_Log("Status : GridironStatus.f_Sensor_VScale = %f\n", *(( float * ) &status_vscale));
        LOS_Log("Status : GridironStatus.u16_Sensor_HScale_x256 = %d\n", status_hscale_x256);
        LOS_Log("Status : GridironStatus.u16_Sensor_VScale_x256 = %d\n", status_vscale_x256);
        LOS_Log("Status : GridironStatus.u16_Crop_HStart = %d\n", status_crop_hstart);
        LOS_Log("Status : GridironStatus.u16_Crop_VStart = %d\n", status_crop_vstart);
        LOS_Log("Status : GridironStatus.u16_Image_HSize = %d\n", status_image_hsize);
        LOS_Log("Status : GridironStatus.u16_Image_VSize = %d\n", status_image_vsize);
        LOS_Log("Status : GridironStatus.u16_Sensor_HSize = %d\n", status_sensor_hsize);
        LOS_Log("Status : GridironStatus.u8_LogGridPitch = %d\n", status_log_grid_pitch);
        LOS_Log("Status : GridironStatus.e_Flag_Cast0 = %d\n", status_flag_cast0);
        LOS_Log("Status : GridironStatus.e_Flag_Cast1 = %d\n", status_flag_cast1);
        LOS_Log("Status : GridironStatus.e_Flag_Cast2 = %d\n", status_flag_cast2);
        LOS_Log("Status : GridironStatus.e_Flag_Cast3 = %d\n", status_flag_cast3);
        LOS_Log("Status : GridironStatus.u16_Phase = %d\n", status_phase);
        LOS_Log("Status : GridironStatus.e_Flag_Enable = %d\n", status_enable_flag);
        LOS_Log("Status : GridironStatus.e_Flag_VerFlip = %d\n", status_flag_vert_flip);
        LOS_Log("Status : GridironStatus.e_Flag_HorFlip = %d\n", status_horiz_flip);
        LOS_Log("Status : GridironStatus.u8_Active_ReferenceCast_Count = %d\n", status_reference_cast_count);

        /// <Hem> Print current mode information
        LOS_Log("Sensor mode information\n");
        LOS_Log("Zoom_Status_LLA.u16_woi_resX = %d\n", ITE_readPE(Zoom_Status_LLA_u16_woi_resX_Byte0));
        LOS_Log("Zoom_Status_LLA.u16_woi_resY = %d\n", ITE_readPE(Zoom_Status_LLA_u16_woi_resY_Byte0));
        LOS_Log("Zoom_Status_LLA.u16_output_resX = %d\n", ITE_readPE(Zoom_Status_LLA_u16_output_resX_Byte0));
        LOS_Log("Zoom_Status_LLA.u16_output_resY = %d\n", ITE_readPE(Zoom_Status_LLA_u16_output_resY_Byte0));
    }
    else
    {
        LOS_Log("Gridiron: disabled.\n");
    }

}

#endif
/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_TestGridIronLiveCast_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_TestGridIronLiveCast_cmd(
int     a_nb_args,
char    **ap_args)
{
    char    mess[256];
    UNUSED(ap_args);

    sprintf(mess, "TestGridIron_%d", ( int ) g_u32_counter_gridiron_test);
    g_u32_counter_gridiron_test++;

    MMTE_TEST_START(mess, "/ite_nmf/test_results_nreg/GridIron_tests", "TestGridIron");

    sprintf(mess, "Testing for GridIron");
    mmte_testNext(mess);

    if (a_nb_args > 1)
    {
        CLI_disp_error("Syntax:=>   TestGridIron   \n");
    }
    else
    {
        if (a_nb_args == 1)
        {
            if (0 == TestGridironLiveCast())
            {
                sprintf(mess, "GridIron live cast testing completed successfully\n");
                mmte_testComment(mess);
                MMTE_TEST_PASSED();
                return (0);
            }
        }
    }


    sprintf(mess, "GridIron live cast testing not done\n");
    mmte_testComment(mess);
    MMTE_TEST_FAILED();
    return (0);
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_ConfigureGridIron_StaticPrams_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_ConfigureGridIron_StaticPrams_cmd(
int     a_nb_args,
char    **ap_args)
{
    char    mess[256];

    sprintf(mess, "ConfigureGridIron_%d", ( int ) g_u32_counter_gridiron_conf);
    g_u32_counter_gridiron_conf++;

    MMTE_TEST_START(mess, "/ite_nmf/test_results_nreg/GridIron_tests", "Test ConfigureGridIronStatic");

    sprintf(mess, "Testing for GridIron");
    mmte_testNext(mess);

    LOS_Log("******************* GRIDIRON STATIC CONFIGURATION START **********************\n");

    if (a_nb_args != 3)
    {
        if (0 == strcmp(ap_args[1], "help"))
        {
            CLI_disp_error("Syntax: ConfigureGridIronStatic , Configures GridIron interface provided by ISP FW \n");
        }
    }
    else
    {
        Configure_Gridiron_StaticPrarams(ap_args[1][0] - '0', ap_args[2][0] - '0');
        Enable_Gridiron(Flag_e_TRUE);
        LOS_Log("GridIron configuration done\n");
    }


    LOS_Log("**************************** GRIDIRON STATIC CONFIGURATION END************************\n");

    MMTE_TEST_PASSED();
    return (0);
}

#if ENABLE_GRIDIRON_DEBUG_TRACES
/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_PrintGridIron_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_PrintGridIron_cmd(
int     a_nb_args,
char    **ap_args)
{
    char    mess[256];

    MMTE_TEST_START("PrintGridIron", "/ite_nmf/test_results_nreg/GridIron_tests", "Test PrintGridIronStatus");

    sprintf(mess, "Testing for GridIron");
    mmte_testNext(mess);

    LOS_Log("********************* PRINT GRIDIRON STATUS PAGE **********************\n");
    if (a_nb_args > 1)
    {
        if (0 == strcmp(ap_args[1], "help"))
        {
            CLI_disp_error("Syntax: PrintGridIron , Tests GridIron interface provided by ISP FW \n");
        }
    }
    else
    {
        if (a_nb_args == 1)
        {
            Print_Gridiron_Parameters();
            LOS_Log("GridIron Status Page Printing done\n");
        }
    }


    LOS_Log("**************** PRINT GRIDIRON STATUS PAGE COMPLETE *****************\n");
    MMTE_TEST_PASSED();
    return (0);
}
#endif

void
GetExpectedParameterValues(
t_uint32    reference_cast_count,
float       reference_cast_0,
float       reference_cast_1,
float       reference_cast_2,
float       reference_cast_3,
float       ptr_live_cast,
t_uint32    *ptr_enable_cast_0,
t_uint32    *ptr_enable_cast_1,
t_uint32    *ptr_enable_cast_2,
t_uint32    *ptr_enable_cast_3,
t_uint32    *ptr_phase,
float       *ptr_status_live_cast)
{
    t_uint32    log_grid_pitch = 0;
    float       f_ReferenceCast1 = 0;
    float       f_ReferenceCast2 = 0;

    // As log_grid_pitch calculations depend on giriron control parameters and sensor both. Sor read log_grid_pitch from gridiron status register.
    log_grid_pitch = ITE_readPE(GridironStatus_u8_LogGridPitch_Byte0);

    LOS_Log(">> GetExpectedParameterValues \n");

#if ENABLE_GRIDIRON_DEBUG_TRACES
    LOS_Log("reference_cast_count = %u \n", reference_cast_count);
    LOS_Log("reference_cast_0 = %f \n", reference_cast_0);
    LOS_Log("reference_cast_1 = %f \n", reference_cast_1);
    LOS_Log("reference_cast_2 = %f \n", reference_cast_2);
    LOS_Log("reference_cast_3 = %f \n", reference_cast_3);
    LOS_Log("log_grid_pitch = %u \n", log_grid_pitch);
#endif   

    switch (reference_cast_count)
    {
        case 1:
            *ptr_status_live_cast = reference_cast_0;
            f_ReferenceCast1 = f_ReferenceCast2 = *ptr_status_live_cast;
            *ptr_enable_cast_0 = Flag_e_TRUE;
            *ptr_enable_cast_1 = Flag_e_FALSE;
            *ptr_enable_cast_2 = Flag_e_FALSE;
            *ptr_enable_cast_3 = Flag_e_FALSE;

            break;

        case 2:
            if (ptr_live_cast >= reference_cast_0)
            {
                *ptr_status_live_cast = reference_cast_0;
                f_ReferenceCast1 = f_ReferenceCast2 = *ptr_status_live_cast;
                *ptr_enable_cast_0 = Flag_e_TRUE;
                *ptr_enable_cast_1 = Flag_e_FALSE;
                *ptr_enable_cast_2 = Flag_e_FALSE;
                *ptr_enable_cast_3 = Flag_e_FALSE;
            }
            else if (ptr_live_cast <= reference_cast_1)
            {
                *ptr_status_live_cast = reference_cast_1;
                f_ReferenceCast1 = f_ReferenceCast2 = *ptr_status_live_cast;
                *ptr_enable_cast_0 = Flag_e_FALSE;
                *ptr_enable_cast_1 = Flag_e_TRUE;
                *ptr_enable_cast_2 = Flag_e_FALSE;
                *ptr_enable_cast_3 = Flag_e_FALSE;
            }
            else if ((ptr_live_cast < reference_cast_0) && (ptr_live_cast > reference_cast_1))
            {
                *ptr_status_live_cast = ptr_live_cast;
                f_ReferenceCast1 = reference_cast_0;
                f_ReferenceCast2 = reference_cast_1;
                *ptr_enable_cast_0 = Flag_e_TRUE;
                *ptr_enable_cast_1 = Flag_e_TRUE;
                *ptr_enable_cast_2 = Flag_e_FALSE;
                *ptr_enable_cast_3 = Flag_e_FALSE;
            }


            break;

        case 3:
            if (ptr_live_cast >= reference_cast_0)
            {
                *ptr_status_live_cast = reference_cast_0;
                f_ReferenceCast1 = f_ReferenceCast2 = *ptr_status_live_cast;
                *ptr_enable_cast_0 = Flag_e_TRUE;
                *ptr_enable_cast_1 = Flag_e_FALSE;
                *ptr_enable_cast_2 = Flag_e_FALSE;
                *ptr_enable_cast_3 = Flag_e_FALSE;
            }
            else if (ptr_live_cast <= reference_cast_2)
            {
                *ptr_status_live_cast = reference_cast_2;
                f_ReferenceCast1 = f_ReferenceCast2 = *ptr_status_live_cast;
                *ptr_enable_cast_0 = Flag_e_FALSE;
                *ptr_enable_cast_1 = Flag_e_FALSE;
                *ptr_enable_cast_2 = Flag_e_TRUE;
                *ptr_enable_cast_3 = Flag_e_FALSE;
            }
            else if (ptr_live_cast == reference_cast_1)
            {
                *ptr_status_live_cast = reference_cast_1;
                f_ReferenceCast1 = f_ReferenceCast2 = *ptr_status_live_cast;
                *ptr_enable_cast_0 = Flag_e_FALSE;
                *ptr_enable_cast_1 = Flag_e_TRUE;
                *ptr_enable_cast_2 = Flag_e_FALSE;
                *ptr_enable_cast_3 = Flag_e_FALSE;
            }
            else if ((ptr_live_cast < reference_cast_0) && (ptr_live_cast > reference_cast_1))
            {
                *ptr_status_live_cast = ptr_live_cast;
                f_ReferenceCast1 = reference_cast_0;
                f_ReferenceCast2 = reference_cast_1;
                *ptr_enable_cast_0 = Flag_e_TRUE;
                *ptr_enable_cast_1 = Flag_e_TRUE;
                *ptr_enable_cast_2 = Flag_e_FALSE;
                *ptr_enable_cast_3 = Flag_e_FALSE;
            }
            else if ((ptr_live_cast < reference_cast_1) && (ptr_live_cast > reference_cast_2))
            {
                *ptr_status_live_cast = ptr_live_cast;
                f_ReferenceCast1 = reference_cast_1;
                f_ReferenceCast2 = reference_cast_2;
                *ptr_enable_cast_0 = Flag_e_FALSE;
                *ptr_enable_cast_1 = Flag_e_TRUE;
                *ptr_enable_cast_2 = Flag_e_TRUE;
                *ptr_enable_cast_3 = Flag_e_FALSE;
            }


            break;

        case 4:
            if (ptr_live_cast >= reference_cast_0)
            {
                *ptr_status_live_cast = reference_cast_0;
                f_ReferenceCast1 = f_ReferenceCast2 = *ptr_status_live_cast;
                *ptr_enable_cast_0 = Flag_e_TRUE;
                *ptr_enable_cast_1 = Flag_e_FALSE;
                *ptr_enable_cast_2 = Flag_e_FALSE;
                *ptr_enable_cast_3 = Flag_e_FALSE;
            }
            else if (ptr_live_cast <= reference_cast_3)
            {
                *ptr_status_live_cast = reference_cast_3;
                f_ReferenceCast1 = f_ReferenceCast2 = *ptr_status_live_cast;
                *ptr_enable_cast_0 = Flag_e_FALSE;
                *ptr_enable_cast_1 = Flag_e_FALSE;
                *ptr_enable_cast_2 = Flag_e_FALSE;
                *ptr_enable_cast_3 = Flag_e_TRUE;
            }
            else if (ptr_live_cast == reference_cast_1)
            {
                *ptr_status_live_cast = reference_cast_1;
                f_ReferenceCast1 = f_ReferenceCast2 = *ptr_status_live_cast;
                *ptr_enable_cast_0 = Flag_e_FALSE;
                *ptr_enable_cast_1 = Flag_e_TRUE;
                *ptr_enable_cast_2 = Flag_e_FALSE;
                *ptr_enable_cast_3 = Flag_e_FALSE;
            }
            else if (ptr_live_cast == reference_cast_2)
            {
                *ptr_status_live_cast = reference_cast_2;
                f_ReferenceCast1 = f_ReferenceCast2 = *ptr_status_live_cast;
                *ptr_enable_cast_0 = Flag_e_FALSE;
                *ptr_enable_cast_1 = Flag_e_FALSE;
                *ptr_enable_cast_2 = Flag_e_TRUE;
                *ptr_enable_cast_3 = Flag_e_FALSE;
            }
            else if ((ptr_live_cast < reference_cast_0) && (ptr_live_cast > reference_cast_1))
            {
                *ptr_status_live_cast = ptr_live_cast;
                f_ReferenceCast1 = reference_cast_0;
                f_ReferenceCast2 = reference_cast_1;
                *ptr_enable_cast_0 = Flag_e_TRUE;
                *ptr_enable_cast_1 = Flag_e_TRUE;
                *ptr_enable_cast_2 = Flag_e_FALSE;
                *ptr_enable_cast_3 = Flag_e_FALSE;
            }
            else if ((ptr_live_cast < reference_cast_1) && (ptr_live_cast > reference_cast_2))
            {
                *ptr_status_live_cast = ptr_live_cast;
                f_ReferenceCast1 = reference_cast_1;
                f_ReferenceCast2 = reference_cast_2;
                *ptr_enable_cast_0 = Flag_e_FALSE;
                *ptr_enable_cast_1 = Flag_e_TRUE;
                *ptr_enable_cast_2 = Flag_e_TRUE;
                *ptr_enable_cast_3 = Flag_e_FALSE;
            }
            else if ((ptr_live_cast < reference_cast_2) && (ptr_live_cast > reference_cast_3))
            {
                *ptr_status_live_cast = ptr_live_cast;
                f_ReferenceCast1 = reference_cast_2;
                f_ReferenceCast2 = reference_cast_3;
                *ptr_enable_cast_0 = Flag_e_FALSE;
                *ptr_enable_cast_1 = Flag_e_FALSE;
                *ptr_enable_cast_2 = Flag_e_TRUE;
                *ptr_enable_cast_3 = Flag_e_TRUE;
            }


            break;
    }


    if (f_ReferenceCast1 != f_ReferenceCast2)
    {
        // <Hem>
        // FIX ME
        // in following calculations 32 should not be used. Use (1<<log_grid_pitch)
        *ptr_phase = (t_uint32)
            (
                ((f_ReferenceCast1 - ptr_live_cast) / (f_ReferenceCast1 - f_ReferenceCast2)) *
                (1 << log_grid_pitch) +
                0.5
            );
            if ((t_uint32)(1 << log_grid_pitch) <= *ptr_phase)
            {
                LOS_Log("boundary case encored. Expected value of phase is 0.\n");
                *ptr_phase = 0;

                // now set one cast enable flag to 0
                if ((*ptr_enable_cast_0 == *ptr_enable_cast_1) && (*ptr_enable_cast_0 == 1))
                {
                    LOS_Log("phase is o, therefore setting *ptr_enable_cast_1 to 0.\n");
                    *ptr_enable_cast_0 = 0;
                }


                if ((*ptr_enable_cast_1 == *ptr_enable_cast_2) && (*ptr_enable_cast_2 == 1))
                {
                    LOS_Log("phase is o, therefore setting *ptr_enable_cast_2 to 0.\n");
                    *ptr_enable_cast_1 = 0;
                }


                if ((*ptr_enable_cast_2 == *ptr_enable_cast_3) && (*ptr_enable_cast_3 == 1))
                {
                    LOS_Log("phase is o, therefore setting *ptr_enable_cast_3 to 0.\n");
                    *ptr_enable_cast_2 = 0;
                }
            }
    }
    else
    {
        *ptr_phase = 0;
    }


    LOS_Log("Expected value of phase = %u\n", *ptr_phase);
    LOS_Log("<< GetExpectedParameterValues \n");

    return;
}

