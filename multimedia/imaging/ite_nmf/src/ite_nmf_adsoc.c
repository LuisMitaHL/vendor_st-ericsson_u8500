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

#include <test/api/test.h>
#include <cm/inc/cm_macros.h>

#include <los/api/los_api.h>
#include "ite_sia_buffer.h"

extern ts_sia_usecase   usecase;

extern void             ITE_NMF_GlaceHistoExpStatsRequestAndWait (void);

extern void             HandleInvalidPipe (t_uint32 u32_pipe_num);

static t_uint32         g_u32_counter_adsoc_conf = 0;
static t_uint32         g_u32_counter_adsoc_test = 0;
static t_uint32         g_u32_counter_RP_conf = 0;
static t_uint32         g_u32_counter_RP_test = 0;

// adsoc peaking
const t_uint32          g_u32_PK_Sharpening_Enable = Flag_e_TRUE;
const t_uint32          g_u32_PK_Coring_Level = 0x02;
const t_uint32          g_u32_PK_OverShoot_Gain_Bright = 0x4;
const t_uint32          g_u32_PK_OverShoot_Gain_Dark = 0x04;
const t_uint32          g_u32_PK_Emboss_Effect_Ctrl = 0x06;
const t_uint32          g_u32_PK_Flipper_Ctrl = 0x01;
const t_uint32          g_u32_PK_GrayBack_Ctrl = 0x02;
const t_uint32          g_u32_PK_Gain = 0x05;

// adsoc radial peaking
const t_uint32          g_u32_RP_Lens_Centre_HOffset = 10;
const t_uint32          g_u32_RP_Lens_Centre_VOffset = 20;
const t_uint32          g_u32_RP_Flag_Adsoc_RP_Enable = Flag_e_TRUE;
const t_uint32          g_u32_RP_Radial_Adsoc_RP_Polycoef0 = 0x04;
const t_uint32          g_u32_RP_Radial_Adsoc_RP_Polycoef1 = 0x06;
const t_uint32          g_u32_RP_Radial_Adsoc_RP_COF_Shift = 0x01;
const t_uint32          g_u32_RP_Radial_Adsoc_RP_Out_COF_Shift = 0x02;
const t_uint32          g_u32_RP_Radial_Adsoc_RP_Unity = 0x05;

static void
Enable_Adsoc(
t_uint32    pipe_no,
t_bool      enableBit)
{
    LOS_Log("\n>> Enable_Adsoc : pipe number = %d, enableBit = %d\n", pipe_no, enableBit);
    if (0 == pipe_no)
    {
        ITE_writePE(Adsoc_PK_Ctrl_0_e_Flag_Adsoc_PK_Enable_Byte0, enableBit);
    }
    else if (1 == pipe_no)
    {
        ITE_writePE(Adsoc_PK_Ctrl_1_e_Flag_Adsoc_PK_Enable_Byte0, enableBit);
    }
    else
    {
        // HandleInvalidPipe(pipe_no);
    }


    if (HostInterfaceHighLevelState_e_RUNNING == ITE_readPE(HostInterface_Status_e_HostInterfaceHighLevelState_Byte0))
    {
        ITE_NMF_GlaceHistoExpStatsRequestAndWait();
    }


    LOS_Log("\n<< Enable_Adsoc");
}


static void
Configure_Adsoc_Peaking(
t_uint32    pipe_no)
{
    LOS_Log("\n>> Configure_Adsoc_Peaking : pipe number = %d\n", pipe_no);
    if (0 == pipe_no)
    {
        ITE_writePE(Adsoc_PK_Ctrl_0_e_Flag_Adsoc_PK_AdaptiveSharpening_Enable_Byte0, g_u32_PK_Sharpening_Enable);
        ITE_writePE(Adsoc_PK_Ctrl_0_u8_Adsoc_PK_Coring_Level_Byte0, g_u32_PK_Coring_Level);
        ITE_writePE(Adsoc_PK_Ctrl_0_u8_Adsoc_PK_OverShoot_Gain_Bright_Byte0, g_u32_PK_OverShoot_Gain_Bright);
        ITE_writePE(Adsoc_PK_Ctrl_0_u8_Adsoc_PK_OverShoot_Gain_Dark_Byte0, g_u32_PK_OverShoot_Gain_Dark);
        ITE_writePE(Adsoc_PK_Ctrl_0_u8_Adsoc_PK_Emboss_Effect_Ctrl_Byte0, g_u32_PK_Emboss_Effect_Ctrl);
        ITE_writePE(Adsoc_PK_Ctrl_0_u8_Adsoc_PK_Flipper_Ctrl_Byte0, g_u32_PK_Flipper_Ctrl);
        ITE_writePE(Adsoc_PK_Ctrl_0_u8_Adsoc_PK_GrayBack_Ctrl_Byte0, g_u32_PK_GrayBack_Ctrl);
        ITE_writePE(Adsoc_PK_Ctrl_0_u8_Adsoc_PK_Gain_Byte0, g_u32_PK_Gain);
    }
    else if (1 == pipe_no)
    {
        ITE_writePE(Adsoc_PK_Ctrl_1_e_Flag_Adsoc_PK_AdaptiveSharpening_Enable_Byte0, g_u32_PK_Sharpening_Enable);
        ITE_writePE(Adsoc_PK_Ctrl_1_u8_Adsoc_PK_Coring_Level_Byte0, g_u32_PK_Coring_Level);
        ITE_writePE(Adsoc_PK_Ctrl_1_u8_Adsoc_PK_OverShoot_Gain_Bright_Byte0, g_u32_PK_OverShoot_Gain_Bright);
        ITE_writePE(Adsoc_PK_Ctrl_1_u8_Adsoc_PK_OverShoot_Gain_Dark_Byte0, g_u32_PK_OverShoot_Gain_Dark);
        ITE_writePE(Adsoc_PK_Ctrl_1_u8_Adsoc_PK_Emboss_Effect_Ctrl_Byte0, g_u32_PK_Emboss_Effect_Ctrl);
        ITE_writePE(Adsoc_PK_Ctrl_1_u8_Adsoc_PK_Flipper_Ctrl_Byte0, g_u32_PK_Flipper_Ctrl);
        ITE_writePE(Adsoc_PK_Ctrl_1_u8_Adsoc_PK_GrayBack_Ctrl_Byte0, g_u32_PK_GrayBack_Ctrl);
        ITE_writePE(Adsoc_PK_Ctrl_1_u8_Adsoc_PK_Gain_Byte0, g_u32_PK_Gain);
    }
    else
    {
        // HandleInvalidPipe(pipe_no);
    }


    LOS_Log("<< Configure_Adsoc_Peaking\n");
}


static void
Verify_Adsoc_Peaking(
t_uint32    pipe_no)
{
    UNUSED(pipe_no);
    LOS_Log("\n>> Verify_Adsoc_Peaking \n");

    // There is no status page for Adsoc peaking. And ISP FW is not doing any calculation on control PEs of adsoc peaking.
    // Control parameters are directly applied to HW registers.
    // So don't take anything apart from status coin testing.
    LOS_Log("<< Verify_Adsoc_Peaking\n");
}


static void
Enable_Adsoc_Radial_Peaking(
t_uint32    pipe_no,
t_bool      enableBit)
{
    LOS_Log("\n>> Enable_Adsoc_Radial_Peaking : pipe number = %d, enableBit = %d\n", pipe_no, enableBit);
    if (0 == pipe_no)
    {
        ITE_writePE(Adsoc_RP_Ctrl_0_e_Flag_Adsoc_RP_Enable_Byte0, enableBit);
    }


    if (1 == pipe_no)
    {
        ITE_writePE(Adsoc_RP_Ctrl_1_e_Flag_Adsoc_RP_Enable_Byte0, enableBit);
    }


    if (HostInterfaceHighLevelState_e_RUNNING == ITE_readPE(HostInterface_Status_e_HostInterfaceHighLevelState_Byte0))
    {
        ITE_NMF_GlaceHistoExpStatsRequestAndWait();
    }


    LOS_Log("<<Enable_Adsoc_Radial_Peaking\n");
}


static t_uint32
Configure_Adsoc_Radial_Peaking(
t_uint32    pipe_no)
{
    LOS_Log("\n>> Configure_Adsoc_Radial_Peaking : pipe number = %d\n", pipe_no);
    if (0 == pipe_no)
    {
        ITE_writePE(Adsoc_RP_Ctrl_0_u16_Lens_Centre_HOffset_Byte0, g_u32_RP_Lens_Centre_HOffset);
        ITE_writePE(Adsoc_RP_Ctrl_0_u16_Lens_Centre_VOffset_Byte0, g_u32_RP_Lens_Centre_VOffset);
        ITE_writePE(Adsoc_RP_Ctrl_0_e_Flag_Adsoc_RP_Enable_Byte0, g_u32_RP_Flag_Adsoc_RP_Enable);
        ITE_writePE(Adsoc_RP_Ctrl_0_u8_Radial_Adsoc_RP_Polycoef0_Byte0, g_u32_RP_Radial_Adsoc_RP_Polycoef0);
        ITE_writePE(Adsoc_RP_Ctrl_0_u8_Radial_Adsoc_RP_Polycoef1_Byte0, g_u32_RP_Radial_Adsoc_RP_Polycoef1);
        ITE_writePE(Adsoc_RP_Ctrl_0_u8_Radial_Adsoc_RP_COF_Shift_Byte0, g_u32_RP_Radial_Adsoc_RP_COF_Shift);
        ITE_writePE(Adsoc_RP_Ctrl_0_u8_Radial_Adsoc_RP_Out_COF_Shift_Byte0, g_u32_RP_Radial_Adsoc_RP_Out_COF_Shift);
        ITE_writePE(Adsoc_RP_Ctrl_0_u8_Radial_Adsoc_RP_Unity_Byte0, g_u32_RP_Radial_Adsoc_RP_Unity);
    }
    else if (1 == pipe_no)
    {
        ITE_writePE(Adsoc_RP_Ctrl_1_u16_Lens_Centre_HOffset_Byte0, g_u32_RP_Lens_Centre_HOffset);
        ITE_writePE(Adsoc_RP_Ctrl_1_u16_Lens_Centre_VOffset_Byte0, g_u32_RP_Lens_Centre_VOffset);
        ITE_writePE(Adsoc_RP_Ctrl_1_e_Flag_Adsoc_RP_Enable_Byte0, g_u32_RP_Flag_Adsoc_RP_Enable);
        ITE_writePE(Adsoc_RP_Ctrl_1_u8_Radial_Adsoc_RP_Polycoef0_Byte0, g_u32_RP_Radial_Adsoc_RP_Polycoef0);
        ITE_writePE(Adsoc_RP_Ctrl_1_u8_Radial_Adsoc_RP_Polycoef1_Byte0, g_u32_RP_Radial_Adsoc_RP_Polycoef1);
        ITE_writePE(Adsoc_RP_Ctrl_1_u8_Radial_Adsoc_RP_COF_Shift_Byte0, g_u32_RP_Radial_Adsoc_RP_COF_Shift);
        ITE_writePE(Adsoc_RP_Ctrl_1_u8_Radial_Adsoc_RP_Out_COF_Shift_Byte0, g_u32_RP_Radial_Adsoc_RP_Out_COF_Shift);
        ITE_writePE(Adsoc_RP_Ctrl_1_u8_Radial_Adsoc_RP_Unity_Byte0, g_u32_RP_Radial_Adsoc_RP_Unity);
    }
    else
    {
        // HandleInvalidPipe(pipe_no);
    }


    LOS_Log("\n<< Configure_Adsoc_Radial_Peaking\n");
    return (0);
}


t_uint32
Verify_Adsoc_Radial_Peaking(
t_uint32    pipe_no)
{
    LOS_Log("\n>> Verify_Adsoc_Radial_Peaking : pipe number = %d\n", pipe_no);

    t_uint32    u32_status_RP_Scale_X = 0;
    t_uint32    u32_status_RP_Scale_Y = 0;

    //t_uint32 u32_status_RP_HOffset = 0;
    //t_uint32 u32_status_RP_VOffset = 0;
    t_uint32    s32_status_RP_HOffset = 0;
    t_uint32    s32_status_RP_VOffset = 0;
    t_uint32    u32_zoom_status_output_resX = 0;
    t_uint32    u32_zoom_status_output_resY = 0;
    t_uint32    u32_zoom_status_woiX = 0;
    t_uint32    u32_zoom_status_woiY = 0;
    int         s32_sensor_H_start = 0;
    int         s32_sensor_V_start = 0;
    int         s32_expected_RP_H_Offset = 0;
    int         s32_expected_RP_V_Offset = 0;
    t_uint32    u32_sensor_prescale_X = 0;
    t_uint32    u32_sensor_prescale_Y = 0;
    char        comment[200];
    t_uint32    errorCount = 0;

    s32_sensor_H_start = ITE_readPE(CurrentFrameDimension_u16_VTXAddrStart_Byte0);
    s32_sensor_V_start = ITE_readPE(CurrentFrameDimension_u16_VTYAddrStart_Byte0);

    s32_expected_RP_H_Offset = g_u32_RP_Lens_Centre_HOffset - s32_sensor_H_start;
    s32_expected_RP_V_Offset = g_u32_RP_Lens_Centre_VOffset - s32_sensor_V_start;
    u32_zoom_status_woiX = ITE_readPE(Zoom_Status_LLA_u16_woi_resX_Byte0);
    u32_zoom_status_woiY = ITE_readPE(Zoom_Status_LLA_u16_woi_resY_Byte0);
    u32_zoom_status_output_resX = ITE_readPE(Zoom_Status_LLA_u16_output_resX_Byte0);
    u32_zoom_status_output_resY = ITE_readPE(Zoom_Status_LLA_u16_output_resY_Byte0);

    u32_sensor_prescale_X = u32_zoom_status_woiX / u32_zoom_status_output_resX;
    u32_sensor_prescale_Y = u32_zoom_status_woiY / u32_zoom_status_output_resY;
    LOS_Log("u32_zoom_status_woiX = %d, u32_zoom_status_woiY = %d\n", u32_zoom_status_woiX, u32_zoom_status_woiY);
    LOS_Log(
    "u32_zoom_status_output_resX = %d, u32_zoom_status_output_resX = %d\n",
    u32_zoom_status_output_resX,
    u32_zoom_status_output_resY);
    LOS_Log("u32_sensor_prescale_X = %d, u32_sensor_prescale_Y = %d\n", u32_sensor_prescale_X, u32_sensor_prescale_Y);

    LOS_Log(
    "expected H offset = %d, expected V offset = %d, sensor pre-scale X = %d, sensor pre-scale Y = %d",
    s32_expected_RP_H_Offset,
    s32_expected_RP_V_Offset,
    u32_sensor_prescale_X,
    u32_sensor_prescale_Y);

    if (ITE_readPE(SystemSetup_e_Coin_Ctrl_Byte0) != ITE_readPE(SystemConfig_Status_e_Coin_Status_Byte0))
    {
        errorCount++;
        sprintf(comment, "ERR : Inconsistent state of system coin\n");
        MMTE_TEST_COMMENT(comment);
    }
    else
    {
        if (0 == pipe_no)
        {
            LOS_Log("Verifying adsoc radial peaking for pipe pipe <%d>\n", pipe_no);
            u32_status_RP_Scale_X = ITE_readPE(Adsoc_RP_Status_0_u16_Adsoc_RP_Scale_X_Byte0);
            u32_status_RP_Scale_Y = ITE_readPE(Adsoc_RP_Status_0_u16_Adsoc_RP_Scale_Y_Byte0);
            s32_status_RP_HOffset = ( int ) ITE_readPE(Adsoc_RP_Status_0_s16_Adsoc_RP_HOffset_Byte0);
            s32_status_RP_VOffset = ( int ) ITE_readPE(Adsoc_RP_Status_0_s16_Adsoc_RP_VOffset_Byte0);
        }
        else if (1 == pipe_no)
        {
            LOS_Log("Verifying adsoc radial peaking for pipe pipe <%d>\n", pipe_no);
            u32_status_RP_Scale_X = ITE_readPE(Adsoc_RP_Status_1_u16_Adsoc_RP_Scale_X_Byte0);
            u32_status_RP_Scale_Y = ITE_readPE(Adsoc_RP_Status_1_u16_Adsoc_RP_Scale_Y_Byte0);
            s32_status_RP_HOffset = ( int ) ITE_readPE(Adsoc_RP_Status_0_s16_Adsoc_RP_HOffset_Byte0);
            s32_status_RP_VOffset = ( int ) ITE_readPE(Adsoc_RP_Status_0_s16_Adsoc_RP_VOffset_Byte0);
        }
        else
        {
            while (1)
            {
                HandleInvalidPipe(pipe_no);
            }
        }
    }


    if (u32_sensor_prescale_X != u32_status_RP_Scale_X)
    {
        errorCount++;
        sprintf(
        comment,
        "ERR : u32_status_RP_Scale_X = %u, u32_sensor_prescale_X = %u\n",
        ( unsigned int ) u32_status_RP_Scale_X,
        ( unsigned int ) u32_sensor_prescale_X);
        MMTE_TEST_COMMENT(comment);
    }


    if (u32_sensor_prescale_Y != u32_status_RP_Scale_Y)
    {
        errorCount++;
        sprintf(
        comment,
        "ERR : u32_status_RP_Scale_Y = %u, u32_sensor_prescale_Y = %u\n",
        ( unsigned int ) u32_status_RP_Scale_Y,
        ( unsigned int ) u32_sensor_prescale_Y);
        MMTE_TEST_COMMENT(comment);
    }


    if (s32_status_RP_HOffset != (t_uint32)s32_expected_RP_H_Offset)
    {
        errorCount++;
        sprintf(
        comment,
        "ERR : s32_status_RP_HOffset = %u, g_u32_RP_Lens_Centre_HOffset = %u\n",
        ( int ) s32_status_RP_HOffset,
        ( int ) s32_expected_RP_H_Offset);
        MMTE_TEST_COMMENT(comment);
    }


    if (s32_status_RP_VOffset != (t_uint32)s32_expected_RP_V_Offset)
    {
        errorCount++;
        sprintf(
        comment,
        "ERR : s32_status_RP_HOffset = %u, g_u32_RP_Lens_Centre_VOffset = %u\n",
        ( int ) s32_status_RP_VOffset,
        ( int ) s32_expected_RP_V_Offset);
        MMTE_TEST_COMMENT(comment);
    }


    LOS_Log("\n<< Verify_Adsoc_Radial_Peaking : pipe number = %d\n", pipe_no);
    return (errorCount);
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_ConfigureAdsoc_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_ConfigureAdsoc_cmd(
int     a_nb_args,
char    **ap_args)
{
    char    mess[256];

    sprintf(mess, "ConfigureAdsoc_%d", ( int ) g_u32_counter_adsoc_conf);
    g_u32_counter_adsoc_conf++;

    MMTE_TEST_START(mess, "/ite_nmf/test_results_nreg/Adsoc_tests", "Test ConfigureAdsoc");

    sprintf(mess, "Testing for Adsoc peaking");
    mmte_testNext(mess);

    LOS_Log("******************* ADSOC PEAKING CONFIGURATION ****************\n");

    if (a_nb_args > 1)
    {
        if (0 == strcmp(ap_args[1], "help"))
        {
            CLI_disp_error("Syntax: ConfigureAdsoc , Configures Adsoc interface provided by ISP FW \n");
        }


        if (0 == strcmp(ap_args[1], "0"))
        {
            Configure_Adsoc_Peaking(0);
            Enable_Adsoc(0, Flag_e_TRUE);
            LOS_Log("Adsoc peaking configuration done for pipe 0\n");
        }
        else if (0 == strcmp(ap_args[1], "1"))
        {
            Configure_Adsoc_Peaking(1);
            Enable_Adsoc(1, Flag_e_TRUE);
            LOS_Log("Adsoc peaking configuration done for pipe 1\n");
        }
    }


    LOS_Log("************************* ADSOC PEAKING TEST END**********************\n");
    MMTE_TEST_PASSED();
    return (CMD_COMPLETE);
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_ConfigureAdsocRP_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_ConfigureAdsocRP_cmd(
int     a_nb_args,
char    **ap_args)
{
    char    mess[256];

    sprintf(mess, "ConfigureAdsoc_Radial_Peaking_%d", ( int ) g_u32_counter_RP_conf);
    g_u32_counter_RP_conf++;

    MMTE_TEST_START(mess, "/ite_nmf/test_results_nreg/AdsocRP_tests", "Test ConfigureAdsocRP");

    sprintf(mess, "Testing for Adsoc peaking");
    mmte_testNext(mess);

    LOS_Log("************* ADSOC RADIAL PEAKING CONFIGURATION TEST ****************\n");
    if (a_nb_args != 2)
    {
        CLI_disp_error("Syntax: ConfigureAdsocRP , Configures Adsoc interface provided by ISP FW \n");
        return (CMD_ERR_ARGS);
    }


    if (0 == Configure_Adsoc_Radial_Peaking(atoi(ap_args[1])))
    {
        Enable_Adsoc_Radial_Peaking(atoi(ap_args[1]), Flag_e_TRUE);
        LOS_Log("Adsoc radial peaking configuration done for pipe <%d>\n", atoi(ap_args[1]));
    }
    else
    {
        snprintf(mess, sizeof(mess), "Adsoc radial peaking configuration failed for pipe <%d>\n", atoi(ap_args[1]));
        MMTE_TEST_COMMENT(mess);
        MMTE_TEST_FAILED();
        return (CMD_COMPLETE);
    }


    LOS_Log("********************* ADSOC RADIAL PEAKING TEST END*******************\n");
    MMTE_TEST_PASSED();
    return (CMD_COMPLETE);
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_VerityAdsocRP_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_VerityAdsocRP_cmd(
int     a_nb_args,
char    **ap_args)
{
    char    mess[256];

    sprintf(mess, "VerityAdsoc_Radial_Peaking_%d", ( int ) g_u32_counter_RP_test);
    g_u32_counter_RP_test++;

    MMTE_TEST_START(mess, "/ite_nmf/test_results_nreg/Adsoc_tests", "Test VerifyAdsocRP");

    sprintf(mess, "Verifying AdsocRP configuration");
    mmte_testNext(mess);

    LOS_Log("********************* Verify Adsoc radial peaking STATUS PAGE **********************\n");
    if (a_nb_args != 2)
    {
        CLI_disp_error("Syntax: VerifyAdsocRP <pipe_number> \n");
        return (CMD_ERR_ARGS);
    }


    if (0 == Verify_Adsoc_Radial_Peaking(atoi(ap_args[1])))
    {
        snprintf(mess, sizeof(mess), "Adsoc radial peaking test successfull for pipe <%d>\n", atoi(ap_args[1]));
        MMTE_TEST_COMMENT(mess);
        MMTE_TEST_PASSED();
        return (CMD_COMPLETE);
    }
    else
    {
        snprintf(mess, sizeof(mess), "Adsoc radial peaking test failed for pipe <%d>\n", atoi(ap_args[1]));
        MMTE_TEST_COMMENT(mess);
        MMTE_TEST_FAILED();
        return (CMD_COMPLETE);
    }


    LOS_Log("**************** Verify Adsoc radial peaking STATUS PAGE COMPLETE *****************\n");
    return (CMD_COMPLETE);
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_PrintAdsoc_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_VerifyAdsoc_cmd(
int     a_nb_args,
char    **ap_args)
{
    char    mess[256];

    sprintf(mess, "VerityAdsoc_%d", ( int ) g_u32_counter_adsoc_test);
    g_u32_counter_adsoc_test++;

    MMTE_TEST_START(mess, "/ite_nmf/test_results_nreg/Adsoc_tests", "Test PrintAdsocStatus");

    sprintf(mess, "Verifying Adsoc");
    mmte_testNext(mess);

    LOS_Log("**********************************************************************\n");
    LOS_Log("******************* PRINT Adsoc peaking STATUS PAGE ********************\n");
    LOS_Log("**********************************************************************\n");
    if (a_nb_args > 1)
    {
        if (0 == strcmp(ap_args[1], "help"))
        {
            CLI_disp_error("Syntax: PrintAdsocRP , Tests Adsoc interface provided by ISP FW \n");
        }


        if (0 == strcmp(ap_args[1], "0"))
        {
            Verify_Adsoc_Peaking(0);
            LOS_Log("Adsoc peaking Status Page Printing done for pipe 0\n");
        }
        else if (0 == strcmp(ap_args[1], "1"))
        {
            Verify_Adsoc_Peaking(1);
            LOS_Log("Adsoc peaking Status Page Printing done for pipe 1\n");
        }
    }
    else
    {
        CLI_disp_error("Syntax: PrintAdsoc , Tests Adsoc peaking interface provided by ISP FW \n");
    }


    LOS_Log("**********************************************************************\n");
    LOS_Log("****************** PRINT Adsoc peaking STATUS PAGE COMPLETE *******************\n");
    LOS_Log("**********************************************************************\n");
    MMTE_TEST_PASSED();
    return (CMD_COMPLETE);
}

