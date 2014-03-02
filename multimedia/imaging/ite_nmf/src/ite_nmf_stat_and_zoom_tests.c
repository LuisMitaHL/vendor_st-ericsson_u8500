/*
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*/

/* Includes */
#include "cli.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ite_nmf_stat_and_zoom_tests.h"
#include "hi_register_acces.h"
#include "pictor.h"
#include "ite_testenv_utils.h"
#include "ite_nmf_standard_functions.h"
#include "ite_nmf_statistic_functions.h"
#include "ite_boardinfo.h"
#include "ite_platform.h"
#include "VhcElementDefs.h"
#include "ite_FocusControl_Module.h"

//for NMF
#include "ite_sia_interface_data.h"
#include "ite_init.h"
#include "ite_sia_init.h"
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
#include <test/api/test.h>

#include <los/api/los_api.h>
#include "ite_main.h"
#include "sia.h"
#include "ite_nmf_headers.h"
#include "ite_debug.h" 
#include "ite_nmf_zoom_functions.h"

/* Prototypes */
static void                             Toggle_System_Coin (Flag_te cancelStats);
static void                             Toggle_AF_Coin (Flag_te cancelStats);
static void                             Check_SystemCoin_Equalled (void);
static void                             Check_AFCoin_Equalled (void);
static void                             Wait_AECStats_Complete (void);
static t_uint32                         Get_AECStats_Validity (void);
static t_uint32                         Get_AFStats_Validity (void);
static void                             Print_AEC_TestResult (Flag_te cancelStats);
static void                             Print_AF_TestResult (Flag_te cancelStats);

static void                             Initialize_Zoom (volatile float MaxDZ);
static t_uint32                         Send_Zoom_And_Stats_Command (float DZ, t_uint32 order);
static void                             Check_ZoomParams_Absorbed (void);
static void                             Wait_AFStats_Complete (void);

static void                             Wait_LensMove_Complete (void);
static void                             Toggle_LensMove_Coin (Flag_te cancelStats);
extern void                     ITE_NMF_GlaceHistoExpStatsRequestAndWait (void);


TAlgoError ITE_NMF_ZoomTest_ForstatsOnly(float   MaxDZ);


/* Data Structures */
t_cmd_list                              ite_cmd_list_statandzoom[] =
{
    { "StatAndZoomHelp", C_ite_dbg_StatAndZoomHelp_cmd, "StatAndZoomHelp: StatAndZoomHelp\n" },
    { "StatTest", C_ite_dbg_StatTest_cmd, "StatTest: StatTest\n" },
    { "AECStatCancelTest", C_ite_dbg_AECStatCancelTest_cmd, "AECStatCancelTest: AECStatCancelTest\n" },
    { "AFStatCancelTest", C_ite_dbg_AFStatCancelTest_cmd, "AFStatCancelTest: AFStatCancelTest\n" },
    { "StatAndZoomTest", C_ite_dbg_StatAndZoomTest_cmd, "StatAndZoomTest: StatAndZoomTest\n" },
    { "FocusCancelTest", C_ite_dbg_FocusCancelTest_cmd, "FocusCancelTest: FocusCancelTest\n" },
    { "zoomtestforstatsonlyTest", C_ite_dbg_zoomtestforstatsonlyTest_cmd, "zoomtestforstatsonlyTest: zoomtestforstatsonlyTest\n" }
};

extern volatile tps_siaFocusStatbuffer  pFocusStatsBuffer;
extern StatsBuffersStorage_t    *g_pStatsBufferStorage;
extern t_uint8                     flagDZdenied ;
int                                     g_TestIdentifier = 0;

/* Exported Interfaces */

/* -----------------------------------------------------------------------
FUNCTION : Init_STAT_AND_ZOOM_CmdList
PURPOSE  :
------------------------------------------------------------------------ */
void
Init_STAT_AND_ZOOM_CmdList(void)
{
    CLI_register_interface(
    "STAT_AND_ZOOM",
    sizeof(ite_cmd_list_statandzoom) / (sizeof(ite_cmd_list_statandzoom[0])),
    ite_cmd_list_statandzoom,
    1);
}


/* -----------------------------------------------------------------------
FUNCTION : C_ite_dbg_StatAndZoomHelp_cmd
PURPOSE  : Displays StatAndZoom Test options for the user.
------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_StatAndZoomHelp_cmd(
int     a_nb_args,
char    **ap_args)
{
    UNUSED(ap_args);
    if (a_nb_args == 1)
    {
        CLI_disp_msg("\n");

        CLI_disp_msg("StatAndZoomHelp: Displays this Help \n");
        CLI_disp_msg("StatAndZoomHelp[Syntax]: StatAndZoomHelp \n\n");

        CLI_disp_msg("StatTest: Toggles system coin and waits for glace event. \n");
        CLI_disp_msg("StatTest[Syntax]: StatTest <HR|LR|BMS> 0\n");
        CLI_disp_msg("StatTest[Example]: StatTest BMS 0\n\n");

        CLI_disp_msg("AECStatCancelTest: Toggles system coin and issues Rx Stop. Checks whether stats are invalid \n");
        CLI_disp_msg("AECStatCancelTest[Syntax]: AECStatCancelTest <HR|LR|BMS> <cancel_flag=<0|1>> \n");
        CLI_disp_msg("AECStatCancelTest[Syntax]: AECStatCancelTest BMS 0 \n\n");

        CLI_disp_msg("AFStatCancelTest: Toggles system coin and issues Rx Stop. Checks whether stats are invalid \n");
        CLI_disp_msg("AFStatCancelTest[Syntax]: AFStatCancelTest <HR|LR> <cancel_flag=<0|1>> \n");
        CLI_disp_msg("AFStatCancelTest[Syntax]: AFStatCancelTest LR 0 \n\n");

        CLI_disp_msg("StatAndZoomTest: Sends Stats and Zoom commands together. If zoom arguements are given... \n");
        CLI_disp_msg(
        "...prior to Stat arguements, then Zoom coin is toggled prior to Stats arguement, and vice-versa. \n");
        CLI_disp_msg(
        "StatAndZoomTest[Syntax]: StatAndZoomTest <HR|LR> <stop|run> <[<zoom zoom_factor> <stat>] | [<stat> <zoom zoom_factor>]> \n");
        CLI_disp_msg("StatAndZoomTest[Example1]: StatAndZoomTest HR stop stat zoom 2 \n");
        CLI_disp_msg("StatAndZoomTest[Example2]: StatAndZoomTest LR run zoom 3 stat \n\n");
    }
    else
    {
        CLI_disp_error("Not correct command arguments\n");
        return (CMD_ERR_ARGS);
    }


    return (CMD_COMPLETE);
}


/* -----------------------------------------------------------------------
FUNCTION : C_ite_dbg_StatTest_cmd
PURPOSE  :
------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_StatTest_cmd(
int     a_nb_args,
char    **ap_args)
{
    char                mess[256],
                        fname[256],
                        pathname[256];
    enum e_grabPipeID   IspOutSelect = GRBPID_PIPE_CAM; //default, unused value
    sprintf(fname, "StatTest");
    sprintf(mess, "_%s_%s_%i", ap_args[1], ap_args[2], g_TestIdentifier++);
    strcat(fname, mess);

    memset(g_out_path, 0, KlogDirectoryLentgh * sizeof(char));
    sprintf(pathname, "/ite_nmf/test_results_nreg/FrameParamStatus");

    MMTE_TEST_START(fname, pathname, "Testing for StatAndZoom_help");

    sprintf(mess, "StatTest");
    MMTE_TEST_NEXT(mess);

    if (a_nb_args == 1)
    {
        CLI_disp_msg("StatTest: Toggles system coin and waits for glace event. \n");
        CLI_disp_msg("StatTest[Syntax]: StatTest <HR|LR|BMS> 0\n");
        CLI_disp_msg("StatTest[Example]: StatTest BMS 0\n\n");
    }
    else if (a_nb_args == 3)
    {
        if (0 == strcmp(ap_args[1], "HR"))
        {
            IspOutSelect = GRBPID_PIPE_HR;
            ITE_HR_Prepare(&usecase, INFINITY);
            ITE_dbg_usagemode("video");
            ITE_HR_Start(INFINITY);
        }
        else if (0 == strcmp(ap_args[1], "LR"))
        {
            IspOutSelect = GRBPID_PIPE_LR;
            ITE_LR_Prepare(&usecase, INFINITY);
            ITE_dbg_usagemode("vf");
            ITE_LR_Start(INFINITY);
        }
        else if (0 == strcmp(ap_args[1], "BMS"))
        {
            IspOutSelect = GRBPID_PIPE_RAW_OUT;
            ITE_BMS_Prepare(&usecase, INFINITY);
            ITE_dbg_usagemode("still");
            ITE_BMS_Start(1);
        }


        Toggle_System_Coin(Flag_e_TRUE);
        Wait_AECStats_Complete();
        Check_SystemCoin_Equalled();

        if (1 == Get_AECStats_Validity())
        {
            MMTE_TEST_COMMENT("Stats VALID.\n");
            MMTE_TEST_PASSED();
        }
        else
        {
            MMTE_TEST_COMMENT("Test Failed As Stats INVALID.\n");
            MMTE_TEST_FAILED();
        }


        if (GRBPID_PIPE_HR == IspOutSelect)
        {
            ITE_HR_Stop();
            ITE_HR_Free();
        }
        else if (GRBPID_PIPE_LR == IspOutSelect)
        {
            ITE_LR_Stop();
            ITE_LR_Free();
        }
        else if (GRBPID_PIPE_RAW_OUT == IspOutSelect)
        {
            ITE_BMS_Stop();
            ITE_BMS_Free();
        }
    }
    else
    {
        MMTE_TEST_COMMENT("Not correct command arguments\n");
        MMTE_TEST_FAILED();
        memset(g_out_path, 0, KlogDirectoryLentgh * sizeof(char));
        return (CMD_ERR_ARGS);
    }


    memset(g_out_path, 0, KlogDirectoryLentgh * sizeof(char));
    return (CMD_COMPLETE);
}


/* -----------------------------------------------------------------------
FUNCTION : C_ite_dbg_AECStatCancelTest_cmd
PURPOSE  :
------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_AECStatCancelTest_cmd(
int     a_nb_args,
char    **ap_args)
{
    char    mess[256],
            fname[256],
            pathname[256];
    Flag_te isCancel = Flag_e_TRUE;
    float           exp_time,fps;
    t_uint16        minfps= 1.0;

    sprintf(fname, "AECStatCancelTest");
    sprintf(mess, "_%s_%s_%i", ap_args[1], ap_args[2], g_TestIdentifier++);
    strcat(fname, mess);

    memset(g_out_path, 0, KlogDirectoryLentgh * sizeof(char));
    sprintf(pathname, "/ite_nmf/test_results_nreg/FrameParamStatus");

    MMTE_TEST_START(fname, pathname, "Testing for StatCancelTest");

    sprintf(mess, "StatCancelTest");
    MMTE_TEST_NEXT(mess);

    if (a_nb_args == 1)
    {
        CLI_disp_msg("AECStatCancelTest: Toggles system coin and issues Rx Stop. Checks whether stats are invalid \n");
        CLI_disp_msg("AECStatCancelTest[Syntax]: AECStatCancelTest <HR|LR|BMS> <cancel_flag=<0|1>> \n");
        CLI_disp_msg("AECStatCancelTest[Syntax]: AECStatCancelTest BMS 0 \n\n");
    }
    else if (a_nb_args >= 2)
    {
        if (a_nb_args == 3)
        {
            isCancel = (Flag_te) ITE_ConvToInt32(ap_args[2]);
        }


        if (0 == strcmp(ap_args[1], "HR"))
        {
            ITE_HR_Prepare(&usecase, INFINITY);
            ITE_dbg_usagemode("video");
            ITE_HR_Start(INFINITY);

//Applying random exposure and enabling flash to simulate usecase where pipe is stopped before stats is exported
            if (Flag_e_TRUE == isCancel)
             {

               fps = ITE_FrameRateProgrammed();
               exp_time = ite_rand_FloatRange((1.0 / fps),(float) minfps) * 1000000;
               LOS_Log("\nExposure Time(us):  %f \n", exp_time);
               ITE_writePE(Exposure_DriverControls_u32_TargetExposureTime_us_Byte0,(t_uint32)exp_time);
               ITE_writePE(FlashControl_e_Flag_FlashMode_Byte0, (t_uint32) Flag_e_TRUE);
             }


            Toggle_System_Coin(isCancel);

            ITE_HR_Stop();

            Wait_AECStats_Complete();

            Print_AEC_TestResult(isCancel);
            Check_SystemCoin_Equalled();

            ITE_HR_Free();
        }
        else if (0 == strcmp(ap_args[1], "LR"))
        {
            ITE_LR_Prepare(&usecase, INFINITY);
            ITE_dbg_usagemode("vf");
            ITE_LR_Start(INFINITY);

//Applying random exposure and enabling flash to simulate usecase where pipe is stopped before stats is exported
            if (Flag_e_TRUE == isCancel)
            {
               fps = ITE_FrameRateProgrammed();
               exp_time = ite_rand_FloatRange((1.0 / fps),( float ) minfps) * 1000000;
               LOS_Log("\nExposure Time(us):  %f \n", exp_time);
               ITE_writePE(Exposure_DriverControls_u32_TargetExposureTime_us_Byte0,(t_uint32)exp_time);
               ITE_writePE(FlashControl_e_Flag_FlashMode_Byte0, (t_uint32) Flag_e_TRUE);
             }

            Toggle_System_Coin(isCancel);

            ITE_LR_Stop();

            Wait_AECStats_Complete();

            Print_AEC_TestResult(isCancel);
            Check_SystemCoin_Equalled();

            ITE_LR_Free();
        }
        else if (0 == strcmp(ap_args[1], "BMS"))
        {
            ITE_BMS_Prepare(&usecase, INFINITY);
            ITE_dbg_usagemode("still");
            ITE_BMS_Start(1);

            Toggle_System_Coin(isCancel);

            ITE_BMS_Stop();

            Wait_AECStats_Complete();

            Print_AEC_TestResult(isCancel);
            Check_SystemCoin_Equalled();

            ITE_BMS_Free();
        }
    }
    else
    {
        MMTE_TEST_COMMENT("Not correct command arguments\n");
        MMTE_TEST_FAILED();
        memset(g_out_path, 0, KlogDirectoryLentgh * sizeof(char));
        return (CMD_ERR_ARGS);
    }


    memset(g_out_path, 0, KlogDirectoryLentgh * sizeof(char));
    return (CMD_COMPLETE);
}


/* -----------------------------------------------------------------------
FUNCTION : C_ite_dbg_AFStatCancelTest_cmd
PURPOSE  :
------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_AFStatCancelTest_cmd(
int     a_nb_args,
char    **ap_args)
{
    char    mess[256],
            fname[256],
            pathname[256];
    Flag_te isCancel = Flag_e_TRUE;

    sprintf(fname, "AFStatCancelTest");
    sprintf(mess, "_%s_%s_%i", ap_args[1], ap_args[2], g_TestIdentifier++);
    strcat(fname, mess);

    memset(g_out_path, 0, KlogDirectoryLentgh * sizeof(char));
    sprintf(pathname, "/ite_nmf/test_results_nreg/FrameParamStatus");

    MMTE_TEST_START(fname, pathname, "Testing for StatCancelTest");

    sprintf(mess, "StatCancelTest");
    MMTE_TEST_NEXT(mess);

    if (a_nb_args == 1)
    {
        CLI_disp_msg("AFStatCancelTest: Toggles system coin and issues Rx Stop. Checks whether stats are invalid \n");
        CLI_disp_msg("AFStatCancelTest[Syntax]: AFStatCancelTest <HR|LR> <cancel_flag=<0|1>> \n");
        CLI_disp_msg("AFStatCancelTest[Syntax]: AFStatCancelTest LR 0 \n\n");
    }
    else if (a_nb_args >= 2)
    {
        if (a_nb_args == 3)
        {
            isCancel = (Flag_te) ITE_ConvToInt32(ap_args[2]);
        }


        if (0 == strcmp(ap_args[1], "HR"))
        {
            ITE_HR_Prepare(&usecase, INFINITY);
            ITE_dbg_usagemode("video");
            ITE_HR_Start(INFINITY);

            AFStats_HostZoneSetup();
            ITE_NMF_FocusStatsPrepare();
            Toggle_AF_Coin(isCancel);
            ITE_HR_Stop();

            Wait_AFStats_Complete();

            Print_AF_TestResult(isCancel);
            Check_AFCoin_Equalled();

            ITE_HR_Free();
        }
        else if (0 == strcmp(ap_args[1], "LR"))
        {
            ITE_LR_Prepare(&usecase, INFINITY);
            ITE_dbg_usagemode("vf");
            ITE_LR_Start(INFINITY);

            AFStats_HostZoneSetup();
            ITE_NMF_FocusStatsPrepare();
            Toggle_AF_Coin(isCancel);
            ITE_LR_Stop();

            Wait_AFStats_Complete();

            Print_AF_TestResult(isCancel);
            Check_AFCoin_Equalled();

            ITE_LR_Free();
        }
    }
    else
    {
        MMTE_TEST_COMMENT("Not correct command arguments\n");
        MMTE_TEST_FAILED();
        memset(g_out_path, 0, KlogDirectoryLentgh * sizeof(char));
        return (CMD_ERR_ARGS);
    }


    memset(g_out_path, 0, KlogDirectoryLentgh * sizeof(char));
    return (CMD_COMPLETE);
}


/* -----------------------------------------------------------------------
FUNCTION : C_ite_dbg_TestStatAndZoom_cmd
PURPOSE  :
------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_StatAndZoomTest_cmd(
int     a_nb_args,
char    **ap_args)
{
    char                mess[256],
                        fname[256],
                        pathname[256];
    enum e_grabPipeID   IspOutSelect = GRBPID_PIPE_CAM; //default, unused value
    sprintf(fname, "StatAndZoomTest");
    sprintf(mess, "_%s_%s_%s_%s_%s", ap_args[1], ap_args[2], ap_args[3], ap_args[4], ap_args[5]);
    strcat(fname, mess);

    memset(g_out_path, 0, KlogDirectoryLentgh * sizeof(char));
    sprintf(pathname, "/ite_nmf/test_results_nreg/FrameParamStatus");

    MMTE_TEST_START(fname, pathname, "Testing for StatAndZoom_help");
    MMTE_TEST_NEXT(mess);

    if (a_nb_args == 1)
    {
        CLI_disp_msg("StatAndZoomTest: Sends Stats and Zoom commands together. If zoom arguements are given... \n");
        CLI_disp_msg(
        "...prior to Stat arguements, then Zoom coin is toggled prior to Stats arguement, and vice-versa. \n");
        CLI_disp_msg(
        "StatAndZoomTest[Syntax]: StatAndZoomTest <HR|LR> <stop|run> <[<zoom zoom_factor> <stat>] | [<stat> <zoom zoom_factor>]> \n");
        CLI_disp_msg("StatAndZoomTest[Example1]: StatAndZoomTest HR stop stat zoom 2 \n");
        CLI_disp_msg("StatAndZoomTest[Example2]: StatAndZoomTest LR run zoom 3 stat \n");
    }
    else if (a_nb_args == 6)
    {
        Initialize_Zoom(5.0);

        if (0 == strcmp(ap_args[1], "HR"))
        {
            IspOutSelect = GRBPID_PIPE_HR;
            ITE_HR_Prepare(&usecase, INFINITY);
            ITE_dbg_usagemode("video");

            if (0 == strcmp(ap_args[2], "run"))
            {
                ITE_HR_Start(INFINITY);
            }
        }
        else if (0 == strcmp(ap_args[1], "LR"))
        {
            IspOutSelect = GRBPID_PIPE_LR;
            ITE_LR_Prepare(&usecase, INFINITY);
            ITE_dbg_usagemode("vf");

            if (0 == strcmp(ap_args[2], "run"))
            {
                ITE_LR_Start(INFINITY);
            }
        }
        else if (0 == strcmp(ap_args[1], "BMS"))
        {
            IspOutSelect = GRBPID_PIPE_RAW_OUT;
            ITE_BMS_Prepare(&usecase, INFINITY);
            ITE_dbg_usagemode("still");

            if (0 == strcmp(ap_args[2], "run"))
            {
                ITE_BMS_Start(1);
            }
        }


        //For Send_Zoom_And_Stats_Command(float DZ, t_uint32 order),
        //order 0: first frame on RUN. Don't explicitly toggle coins.
        //order 1: first zoom, then stats
        //order 2: first stats, then zoom
        if (0 == strcmp(ap_args[2], "stop"))
        {
            Send_Zoom_And_Stats_Command(2, 0);

            if (GRBPID_PIPE_HR == IspOutSelect)
            {
                ITE_dbg_usagemode("video");
                ITE_HR_Start(INFINITY);
            }
            else if (GRBPID_PIPE_LR == IspOutSelect)
            {
                ITE_dbg_usagemode("vf");
                ITE_LR_Start(INFINITY);
            }
            else if (GRBPID_PIPE_RAW_OUT == IspOutSelect)
            {
                ITE_dbg_usagemode("still");
                ITE_BMS_Start(1);
            }
        }
        else if (0 == strcmp(ap_args[2], "run"))
        {
            if ((0 == strcmp(ap_args[3], "zoom")) && (0 == strcmp(ap_args[5], "stat")))
            {
                Send_Zoom_And_Stats_Command(2, 1);
            }
            else if ((0 == strcmp(ap_args[3], "stat")) && (0 == strcmp(ap_args[4], "zoom")))
            {
                Send_Zoom_And_Stats_Command(2, 2);
            }


            //As zoom will complete prior to stats request,
            //check zoom before stats, irrespective of the order.
            ITE_NMF_DZ_WaitZoomComplete();
            Wait_AECStats_Complete();
        }


        //In case the commands were given from STOP state, just check whether
        //commands are absorbed. This is because the zoom and stats event complete
        //notifications are currently disabled in ite_event.c
        Check_ZoomParams_Absorbed();
        Check_SystemCoin_Equalled();

        if (1 == Get_AECStats_Validity())
        {
            MMTE_TEST_COMMENT("Stats VALID.\n");
            MMTE_TEST_PASSED();
        }
        else
        {
            MMTE_TEST_COMMENT("Test Failed As Stats INVALID.\n");
            MMTE_TEST_FAILED();
        }


        if (GRBPID_PIPE_HR == IspOutSelect)
        {
            ITE_HR_Stop();
            ITE_HR_Free();
        }
        else if (GRBPID_PIPE_LR == IspOutSelect)
        {
            ITE_LR_Stop();
            ITE_LR_Free();
        }
        else if (GRBPID_PIPE_RAW_OUT == IspOutSelect)
        {
            ITE_BMS_Stop();
            ITE_BMS_Free();
        }
    }
    else
    {
        MMTE_TEST_COMMENT("Not correct command arguments\n");
        MMTE_TEST_FAILED();
        memset(g_out_path, 0, KlogDirectoryLentgh * sizeof(char));
        return (CMD_ERR_ARGS);
    }


    memset(g_out_path, 0, KlogDirectoryLentgh * sizeof(char));
    return (CMD_COMPLETE);
}


#define MAX_ITERATIONS  10

/* -----------------------------------------------------------------------
FUNCTION : C_ite_dbg_FocusCancelTest_cmd
PURPOSE  :
------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_FocusCancelTest_cmd(
int     a_nb_args,
char    **ap_args)
{
    char    mess[256],
            fname[256],
            pathname[256];
    Flag_te isCancel = Flag_e_TRUE;
    int     temp_i;

    sprintf(fname, "FocusCancelTest");
    sprintf(mess, "_%s_%s_%i", ap_args[1], ap_args[2], g_TestIdentifier++);
    strcat(fname, mess);

    memset(g_out_path, 0, KlogDirectoryLentgh * sizeof(char));
    sprintf(pathname, "/ite_nmf/test_results_nreg/FrameParamStatus");

    MMTE_TEST_START(fname, pathname, "Testing for FocusCancelTest");

    sprintf(mess, "FocusCancelTest");
    MMTE_TEST_NEXT(mess);

    if (a_nb_args == 1)
    {
        CLI_disp_msg("FocusCancelTest: Toggles system coin and issues Rx Stop. Checks whether stats are invalid \n");
        CLI_disp_msg("FocusCancelTest[Syntax]: FocusCancelTest <HR|LR> <cancel_flag=<0|1>> \n");
        CLI_disp_msg("FocusCancelTest[Syntax]: FocusCancelTest LR 0 \n\n");
    }
    else if (a_nb_args >= 2)
    {
        if (a_nb_args == 3)
        {
            isCancel = (Flag_te) ITE_ConvToInt32(ap_args[2]);
        }


        if (0 == strcmp(ap_args[1], "HR"))
        {
            for (temp_i = 0; temp_i < MAX_ITERATIONS; temp_i++)
            {
                ITE_HR_Prepare(&usecase, INFINITY);
                ITE_dbg_usagemode("video");
                ITE_HR_Start(INFINITY);

                AFStats_HostZoneSetup();
                ITE_NMF_FocusStatsPrepare();
                Toggle_LensMove_Coin(isCancel);
                ITE_HR_Stop();

                Wait_LensMove_Complete();

                ITE_HR_Free();
            }
        }
        else if (0 == strcmp(ap_args[1], "LR"))
        {
            ITE_LR_Prepare(&usecase, INFINITY);
            ITE_dbg_usagemode("vf");
            ITE_LR_Start(INFINITY);

            AFStats_HostZoneSetup();
            ITE_NMF_FocusStatsPrepare();
            Toggle_LensMove_Coin(isCancel);
            ITE_LR_Stop();

            Wait_LensMove_Complete();

            ITE_LR_Free();
        }
    }
    else
    {
        MMTE_TEST_COMMENT("Not correct command arguments\n");
        MMTE_TEST_FAILED();
        memset(g_out_path, 0, KlogDirectoryLentgh * sizeof(char));
        return (CMD_ERR_ARGS);
    }


    MMTE_TEST_PASSED();
    return (CMD_COMPLETE);
}

CMD_COMPLETION
C_ite_dbg_zoomtestforstatsonlyTest_cmd(
int     a_nb_args,
char    **ap_args)
{
    TAlgoError ret = EErrorNone;
    if (ap_args[1] == NULL)
    {
        ap_args[1] = "";
    }


    if (0 == strcmp(ap_args[1], "help"))
    {
        CLI_disp_error("syntax:  zoomtestforstatsonlyTest <MaxDZ> \n");
    }
    else
    {
        if (a_nb_args == 2)
        {
            ret = ITE_NMF_ZoomTest_ForstatsOnly( ITE_ConvToFloat(ap_args[1]))	;
            if (ret != EErrorNone)
            {
                 CLI_disp_error("Not correct command arguments in C_ite_dbg_zoomtestforstatsonlyTest_cmd\n");
                 return (CMD_COMPLETE);
            }
        }
        else
        {
            CLI_disp_error("Not correct command arguments\n");
        }
    }

    return (CMD_COMPLETE);
}


/*
* ********************
* Stats Internal Functions
* ********************
*/
static void
Toggle_System_Coin(
Flag_te cancelStats)
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


    ITE_RefreshEventCount(Event0_Count_u16_EVENT0_17_GLACE_STATS_READY_Byte0);

    ITE_writePE(SystemSetup_e_Flag_AecStatsCancel_Byte0, cancelStats);
    ITE_writePE(SystemSetup_e_Coin_Ctrl_Byte0, !control_coin);
}


static void
Toggle_AF_Coin(
Flag_te cancelStats)
{
    Coin_te control_coin,
            status_coin;

    LOS_Log("\nToggle AF Coin.\n");

    control_coin = (Coin_te) ITE_readPE(AFStats_Controls_e_Coin_AFStatsExportCmd_Byte0);
    status_coin = (Coin_te) ITE_readPE(AFStats_Status_e_Coin_AFStatsExportStatus_Byte0);

    if (control_coin != status_coin)
    {
        LOS_Log("Coins not in proper state to execute the command.\n");
        LOS_Log("Status and Control Coins should be equal before executing the command.\n");
        return;
    }


    ITE_RefreshEventCount(Event0_Count_u16_EVENT0_20_AF_STATS_READY_Byte0);

    //[PM]Toggling AF coin and giving single frame request for stats doesnot always reproduce the use case,
    //since sensor stop takes ~2 frames, and stats command is almost always complete prior to that. But there is
    //no other option, as continuous mode is right now not supported in the Cancel feature.
    ITE_writePE(AFStats_Controls_e_Flag_AFStatsCancel_Byte0, cancelStats);
    ITE_writePE(AFStats_Controls_e_AFStats_HostCmd_Ctrl_Byte0, AFStats_HostCmd_e_REQ_STATS_ONCE);
    ITE_writePE(AFStats_Controls_e_Coin_AFStatsExportCmd_Byte0, !control_coin);
}


static void
Toggle_LensMove_Coin(
Flag_te cancelStats)
{
    ITE_writePE(AFStats_Controls_e_Flag_AFStatsCancel_Byte0, cancelStats);
    ITE_writePE(AFStats_Controls_e_AFStats_HostCmd_Ctrl_Byte0, AFStats_HostCmd_e_REQ_STATS_ONCE);

    //By default, move to macro at first.
    ITE_writePE(
    FocusControl_Controls_e_FocusControl_LensCommand_Control_Byte0,
    FocusControl_LensCommand_e_LA_CMD_GOTO_MACRO_NEAR_END);
    ITE_writePE(FocusControl_Controls_e_Flag_StatsWithLensMove_Control_Byte0, Flag_e_FALSE);
    ITE_RefreshEventCount(Event0_Count_u16_EVENT0_21_FLA_DRIVER_LENS_STOP_Byte0);

    //Toggle focus coin
    LOS_Log("First moving to Macro..\n");
    ITE_writePE(
    FocusControl_Controls_e_Coin_Control_Byte0,
    !((Coin_te) ITE_readPE(FocusControl_Status_e_Coin_Status_Byte0)));
    ITE_NMF_waitForLensStop();
    ITE_GetEventCount(Event0_Count_u16_EVENT0_21_FLA_DRIVER_LENS_STOP_Byte0);
    LOS_Log("Movement to Macro complete!\n");

    //CLEAR NOTIFICATION :FLADRIVER STOP : IF ANY
    ITE_CleanEvent(ITE_EVT_ISPCTL_FOCUS_EVENTS);

    //LOS_Sleep(500);
    LOS_Log("Now moving to Infinity..\n");
    ITE_writePE(
    FocusControl_Controls_e_FocusControl_LensCommand_Control_Byte0,
    FocusControl_LensCommand_e_LA_CMD_GOTO_INFINITY_FAR_END);
    ITE_writePE(FocusControl_Controls_e_Flag_StatsWithLensMove_Control_Byte0, Flag_e_TRUE);

    ITE_RefreshEventCount(Event0_Count_u16_EVENT0_21_FLA_DRIVER_LENS_STOP_Byte0);

    //Now Toggle focus coin and immediately give STOP command
    ITE_writePE(
    FocusControl_Controls_e_Coin_Control_Byte0,
    !((Coin_te) ITE_readPE(FocusControl_Status_e_Coin_Status_Byte0)));

    //ITE_writePE(HostInterface_Control_e_HostInterfaceCommand_User_Byte0, HostInterfaceCommand_e_STOP);
}


static void
Check_SystemCoin_Equalled(void)
{
    Coin_te control_coin,
            status_coin;

    LOS_Log("\nChecking AEC Coin.\n");

    do
    {
        /* Check the status coin */
        control_coin = (Coin_te) ITE_readPE(SystemSetup_e_Coin_Ctrl_Byte0);
        status_coin = (Coin_te) ITE_readPE(SystemConfig_Status_e_Coin_Status_Byte0);
    } while (status_coin != control_coin);

    if (control_coin == status_coin)
    {
        LOS_Log("Coin toggled!\n");
    }
}


static void
Check_AFCoin_Equalled(void)
{
    Coin_te control_coin,
            status_coin;

    LOS_Log("\nChecking AF Coin.\n");

    do
    {
        /* Check the status coin */
        control_coin = (Coin_te) ITE_readPE(AFStats_Controls_e_Coin_AFStatsExportCmd_Byte0);
        status_coin = (Coin_te) ITE_readPE(AFStats_Status_e_Coin_AFStatsExportStatus_Byte0);
    } while (status_coin != control_coin);

    if (control_coin == status_coin)
    {
        LOS_Log("Coin toggled!\n");
    }
}


static void
Wait_AECStats_Complete(void)
{
    LOS_Log("\nChecking glace event.. \n");
    ITE_NMF_WaitStatReady(ISP_GLACE_STATS_READY);
    ITE_GetEventCount(Event0_Count_u16_EVENT0_17_GLACE_STATS_READY_Byte0);
    LOS_Log("1 glace event(s) received.\n");
}


static void
Wait_AFStats_Complete(void)
{
    LOS_Log("\nChecking AF event.. \n");
    ITE_NMF_waitForAFStatsReady ();
    ITE_GetEventCount(Event0_Count_u16_EVENT0_20_AF_STATS_READY_Byte0);
    LOS_Log("1 AF event(s) received.\n");
}


static void
Wait_LensMove_Complete(void)
{
    //Wait for lens stop
    ITE_NMF_waitForLensStop();
    ITE_GetEventCount(Event0_Count_u16_EVENT0_21_FLA_DRIVER_LENS_STOP_Byte0);
    LOS_Log("Movement to Infinity complete!\n");
}


/*
 *****************************************
 * Return Value: 1 => Valid , 0 => Invalid
 *****************************************
 */
static t_uint32
Get_AECStats_Validity(void)
{
    LOS_Log("\nChecking AECStats Validity \n");

    return (!(ITE_readPE(FrameParamStatus_Extn_u32_StatsInvalid_Byte0)));
}


/*
 *****************************************
 * Return Value: 1 => Valid , 0 => Invalid
 *****************************************
 */
static t_uint32
Get_AFStats_Validity(void)
{
    LOS_Log("\nChecking AFStats Validity \n");
    return (ITE_readPE(FrameParamStatus_Af_u32_AfStatsValid_Byte0));
}


static void
Print_AEC_TestResult(
Flag_te cancelStats)
{
    LOS_Log("\nPrinting AEC Test Results \n");

    //cancel request
    if (Flag_e_TRUE == cancelStats)
    {
        MMTE_TEST_COMMENT("Stats Cancel Request set to TRUE.\n");

        if (1 == Get_AECStats_Validity())
        {
            MMTE_TEST_COMMENT("Test Failed as Stats VALID.\n");
            MMTE_TEST_FAILED();
        }
        else
        {
            MMTE_TEST_COMMENT("Stats INVALID as expected.\n");
            MMTE_TEST_PASSED();
        }
    }


    //complete request
    else
    {
        MMTE_TEST_COMMENT("Stats Cancel Request set to FALSE; so Stats are to be completed before STOP.\n");

        if (1 == Get_AECStats_Validity())
        {
            MMTE_TEST_COMMENT("Stats VALID as expected.\n");
            MMTE_TEST_PASSED();
        }
        else
        {
            MMTE_TEST_COMMENT("Test Failed as Stats INVALID.\n");
            MMTE_TEST_FAILED();
        }
    }
}


static void
Print_AF_TestResult(
Flag_te cancelStats)
{
    t_uint32    u32_Valid_flag = 0;
    char        comment[256];

    LOS_Log("\nPrinting AF Test Results \n");

    //cancel request
    if (Flag_e_TRUE == cancelStats)
    {
        MMTE_TEST_COMMENT("AF Stats Cancel Request set to TRUE.\n");

        if (1 == Get_AFStats_Validity())
        {
            MMTE_TEST_COMMENT("AF Stats VALID even though Cancel flag was enabled.\n");
            MMTE_TEST_COMMENT(
            "This means that AF stats export was complete before STOP command was received by the FW.\n");
            MMTE_TEST_COMMENT("So lets check the valid bit in stats exported memory..\n");

            //4byte * 10 elements per zone * 10 zones + 4 byte valid flag
            u32_Valid_flag = (*(( volatile unsigned char * ) ((t_uint32) (pFocusStatsBuffer->logAddress) + (4 * 10 * 10))));

            //Valid Byte = 1=> Valid
            //Valid Byte = 0 => Invalid
            if (u32_Valid_flag == 1)
            {
                snprintf(comment, sizeof(comment), "Valid stats exported in memory also!\n");
                MMTE_TEST_COMMENT(comment);

                MMTE_TEST_PASSED();
            }
            else
            {
                snprintf(comment, sizeof(comment), "Invalid stats exported in memory. Test case will FAIL.\n");
                MMTE_TEST_COMMENT(comment);

                MMTE_TEST_FAILED();
            }
        }
        else
        {
            MMTE_TEST_COMMENT("AF Stats INVALID as expected.\n");
            MMTE_TEST_PASSED();
        }
    }


    //complete request
    else
    {
        MMTE_TEST_COMMENT("AF Stats Cancel Request set to FALSE; so Stats are to be completed before STOP.\n");

        if (1 == Get_AFStats_Validity())
        {
            MMTE_TEST_COMMENT("AF Stats VALID as expected.\n");
            MMTE_TEST_PASSED();
        }
        else
        {
            MMTE_TEST_COMMENT("Test Failed as AF tats INVALID.\n");
            MMTE_TEST_FAILED();
        }
    }
}


/*
* ********************
* Zoom Internal Functions
* ********************
*/
static void
Initialize_Zoom(
volatile float  MaxDZ)
{
    LOS_Log("\nInitializing Zoom.\n");

    ITE_writePE(Zoom_Params_f_ZoomRange_Byte0, *( volatile t_uint32 * ) &MaxDZ);
    ITE_writePE(Zoom_Control_s16_CenterOffsetX_Byte0, 0);
    ITE_writePE(Zoom_Control_s16_CenterOffsetY_Byte0, 0);
}


static t_uint32
Send_Zoom_And_Stats_Command(
float       DZ,
t_uint32    order)
{
    volatile t_uint32   MaxFOVX;
    volatile t_uint32   MaxDZ;
    volatile float      FOVX;
    t_uint8             DZCmdCount = 0xFF;

    LOS_Log("\nPreparing to Send Zoom And Stats Commands\n");

    // Check if DZ < or = MaxDZ
    MaxDZ = ITE_readPE(Zoom_Params_f_ZoomRange_Byte0);
    if (DZ > (*( volatile float * ) &MaxDZ))
    {
        LOS_Log("DZ Value greater than MaxDZ factor\n");
    }
    else
    {
        MaxFOVX = ITE_readPE(Zoom_Status_f_MaxAvailableFOVXAtCurrentCenter_Byte0);
        if (DZ == 1)
        {
            FOVX = 0;           // Full field Of view
        }
        else
        {
            FOVX = (*( volatile float * ) &MaxFOVX) / DZ;
        }


        LOS_Log("MaxFOVx = %f Set FOVX %f\n", (*( volatile float * ) &MaxFOVX), FOVX);

        ITE_writePE(Zoom_Control_f_SetFOVX_Byte0, *( volatile t_uint32 * ) &FOVX);

        //set FOV now
        ITE_writePE(Zoom_CommandControl_e_ZoomCommand_Byte0, ZoomCommand_e_SetFOV);

        //toggle coins now
        DZCmdCount = ITE_readPE(Zoom_CommandStatus_u8_CommandCount_Byte0);
        DZCmdCount = (DZCmdCount + 1) % 256;

        //order 0: first frame on RUN. Don't explicitly toggle coins.
        if (order == 1)         //1: first zoom, then stats
        {
            LOS_Log("\nSending Zoom and Stats commands: first zoom, then stats\n");

            //toggle zoom coin
            ITE_writePE(Zoom_CommandControl_u8_CommandCount_Byte0, DZCmdCount);

            //send stats command (toggle system coin)
            ITE_writePE(SystemSetup_e_Coin_Ctrl_Byte0, !ITE_readPE(SystemConfig_Status_e_Coin_Status_Byte0));
        }
        else if (order == 2)    //2: first stats, then zoom
        {
            LOS_Log("\nSending Zoom and Stats commands: first stats, then zoom\n");

            //send stats command (toggle system coin)
            ITE_writePE(SystemSetup_e_Coin_Ctrl_Byte0, !ITE_readPE(SystemConfig_Status_e_Coin_Status_Byte0));

            //toggle zoom coin
            ITE_writePE(Zoom_CommandControl_u8_CommandCount_Byte0, DZCmdCount);
        }
    }


    return (DZCmdCount);
}


static void
Check_ZoomParams_Absorbed(void)
{
    t_uint8 DZCmdCount = ITE_readPE(Zoom_CommandControl_u8_CommandCount_Byte0);

    LOS_Log("\nChecking whether Zoom command is absorbed\n");

    // wait command absorbed
    //do {LOS_Sleep(5);}
    while (ITE_readPE(Zoom_CommandStatus_u8_CommandCount_Byte0) != DZCmdCount)
        ;

    LOS_Log("\nZoom command absorbed!\n");
}


TAlgoError
ITE_NMF_ZoomTest_ForstatsOnly(
float   MaxDZ)
{
    t_sint16    i;
    t_uint16    zoomstep = 10;
    t_uint16    maxstep = zoomstep;
    t_uint32    Value = 0;
    float       DZ;
    char        testComment[200];
    char        sensorName[16];
//    char        formatName[16];
//    char        resolutionName[16];
    char        pathname[200];
    char        mess[256];
    TAlgoError  ret = EErrorNone;
    t_uint32    Num = 0;
    e_resolution        lrresolution;
    e_resolution        hrresolution;
    char                lrresolutionstring[16];
    char                hrresolutionstring[16];
    int retval=EOK;

    if (usecase.sensor == 0)
    {
        sprintf(sensorName, "Cam0");
    }
    else
    {
        sprintf(sensorName, "Cam1");
    }

    sprintf(testComment, "%s_test_nreg_DZ_For_Stats_Only", sensorName);

    //perform a "cd" to destination repository for test log
    sprintf(pathname, IMAGING_PATH "/ite_nmf/test_results_nreg/DZ_FOR_STATS_ONLY");
    strcat(g_out_path, pathname);
    LOS_Log("Current path: >%s< \n", ( char * ) g_out_path);
    init_test_results_nreg(g_out_path);
    mmte_testStart(testComment, " test DZ all step", g_out_path);
     snprintf(mess, sizeof(mess), "MaxDZ %f for this usecase", MaxDZ);
     mmte_testComment(mess);

     ITE_NMF_SetGlaceBlockFraction(100, 100, 0, 0);

     ITE_NMF_SetGlaceGrid(72, 54);

     //syntax: glacesat <Red Sat Value> <Green Sat Value> <Blue Sat Value>\n");
     ITE_NMF_SetGlaceSatLevel(255, 255, 255);

     //To work with Glace and histogram
     //To work with Glace only: histogram pointer sets to 0 + allocation for FrameParamStatus structure
     //pStore = g_pStatsBufferStorage;
     ITE_writePE(HistStats_Ctrl_ptru32_HistRAddr_Byte0, (t_uint32) g_pStatsBufferStorage->ispHistAddr_R);
     ITE_writePE(HistStats_Ctrl_ptru32_HistGAddr_Byte0, (t_uint32) g_pStatsBufferStorage->ispHistAddr_G);
     ITE_writePE(HistStats_Ctrl_ptru32_HistBAddr_Byte0, (t_uint32) g_pStatsBufferStorage->ispHistAddr_B);
     ITE_writePE(Glace_Control_ptrGlace_Statistics_Byte0, g_pStatsBufferStorage->ispGlaceStatsAddr);
     ITE_writePE(Glace_Control_e_StatisticsFov_Byte0, StatisticsFov_e_Master_Pipe);

     ITE_writePE(AFStats_Controls_e_AFStats_HostCmd_Ctrl_Byte0, AFStats_HostCmd_e_REQ_STATS_ONCE);
     ITE_writePE(AFStats_Controls_pu32_HostAssignedAddr_Byte0, (t_uint32) (pFocusStatsBuffer->ISPBufferBaseAddress));

     ITE_NMF_UpdateGlaceParam();

     Initialize_Zoom(MaxDZ);

     ITE_writePE(SystemSetup_e_Flag_ZoomdStatsOnly_Byte0 , 1);

     for (hrresolution = usecase.HR_Min_resolution; hrresolution <= usecase.HR_Video_Max_resolution; hrresolution++)
       for (lrresolution = usecase.LR_Min_resolution; lrresolution <= usecase.LR_Max_resolution; lrresolution++)
       {
	      ITE_GiveStringNameFromEnum(hrresolution, hrresolutionstring);
          retval = ITE_dbg_setusecase("HR", hrresolutionstring, "YUV420MB_D");
          if(retval != EOK)
          {
              LOS_Log("ITE_dbg_setusecase gave error in %s and line  %d\n",__FUNCTION__,__LINE__);
              ret =  EErrorArgument;
              goto END;
          }


          ITE_GiveStringNameFromEnum(lrresolution, lrresolutionstring);
          retval =ITE_dbg_setusecase("LR", lrresolutionstring, "YUV422RI");
          if(retval != EOK)
          {
            LOS_Log("ITE_dbg_setusecase gave error in %s and line  %d\n",__FUNCTION__,__LINE__);
            ret =  EErrorArgument;
            goto END;
          }

	   //ITE_GiveStringNameFromEnum(usecase.LR_resolution, resolutionName);
          LOS_Log(">>>>>>>>>>>>>>>Zoom Test: Pipe LR, Resolution: %s <<<<<<<<<<<<<<<\n", lrresolutionstring);


          //ITE_GiveStringNameFromEnum(usecase.HR_resolution, resolutionName);
          LOS_Log(">>>>>>>>>>>>>>>Zoom Test: Pipe HR, Resolution: %s <<<<<<<<<<<<<<<\n", hrresolutionstring);

          snprintf(mess, sizeof(mess), "Zoom Test for stats only: Pipe HR, Resolution: %s  Pipe LR, Resolution: %s ", hrresolutionstring, lrresolutionstring);
          mmte_testComment(mess);

          ITE_LRHR_Prepare(&usecase, INFINITY);

          ITE_LRHR_Start(INFINITY);

	for (i = 0; i <= zoomstep; i++)
    {
        DZ = 1 + (i * ((MaxDZ - 1) / zoomstep));
        LOS_Log("ZoomTest for DZ = %f\n", DZ);
        snprintf(mess, sizeof(mess), "Test %d :check DZ Value %f ", ( int ) Num, DZ);   //__NO_WARNING__
        mmte_testNext(mess);
        //ret = ITE_NMF_ZoomTest(pipe, DZ);
	  ITE_NMF_DZ_Set(DZ);

        mmte_testResult(TEST_PASSED);

        if (flagDZdenied == 1)
        {
            snprintf(mess, sizeof(mess), "DZ Step %f cannot be absorbed DZ Denied by ISP", DZ);
            mmte_testComment(mess);
        }

        ITE_NMF_GlaceHistoExpStatsRequestAndWait();
	 ITE_NMF_DumpGlaceBuffer();
        ITE_NMF_DumpHistoBuffer();

	 Value = !(ITE_readPE(AFStats_Status_e_Coin_AFStatsExportStatus_Byte0));
        ITE_writePE(AFStats_Controls_e_Coin_AFStatsExportCmd_Byte0, Value);
        ITE_NMF_waitForAFStatsReady();

        flagDZdenied = 0;
        Num++;
        //LOS_Sleep(500);
    }


    for (i = maxstep - 1; i >= 0; i--)
    {
        DZ = 1 + (i * ((MaxDZ - 1) / zoomstep));
        LOS_Log("ZoomTest for DZ = %f\n", DZ);
        snprintf(mess, sizeof(mess), "Test %d :check DZ Value %f ", ( int ) Num, DZ);   //__NO_WARNING__
        mmte_testNext(mess);
        //t = ITE_NMF_ZoomTest(pipe, DZ);
        ITE_NMF_DZ_Set(DZ);

        mmte_testResult(TEST_PASSED);

        if (flagDZdenied == 1)
        {
            snprintf(mess, sizeof(mess), "DZ Step %f cannot be absorbed DZ Denied by ISP", DZ);
            mmte_testComment(mess);
        }

        ITE_NMF_GlaceHistoExpStatsRequestAndWait();
	 //ITE_NMF_DumpGlaceBuffer();
        //ITE_NMF_DumpHistoBuffer();

	 Value = !(ITE_readPE(AFStats_Status_e_Coin_AFStatsExportStatus_Byte0));
        ITE_writePE(AFStats_Controls_e_Coin_AFStatsExportCmd_Byte0, Value);
        ITE_NMF_waitForAFStatsReady();

        flagDZdenied = 0;
        Num++;
        //LOS_Sleep(500);
    }

	ITE_LRHR_Stop();
     }

       ITE_writePE(SystemSetup_e_Flag_ZoomdStatsOnly_Byte0 , 0);
	mmte_testEnd();

    //clean  g_out_path
    memset(g_out_path, 0, KlogDirectoryLentgh * sizeof(char));
    END:
    return ret;

}

