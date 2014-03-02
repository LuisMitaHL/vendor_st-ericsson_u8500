/******************************************************************************
* C STMicroelectronics
* Reproduction and Communication of this document is strictly prohibited
* unless specifically authorized in writing by STMicroelectronics.
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ite_testenv_utils.h"
#include "cli.h"
#include "ite_nmf_stress_tests.h"

#include "ite_boardinfo.h"
#include "ite_platform.h"
#include "VhcElementDefs.h"

//for NMF
#include "ite_sia_interface_data.h"
#include "ite_sia_bootcmd.h"
#include "ite_init.h"
#include "ite_sia_init.h"

//#include "ite_alloc.h"
#include "ite_display.h"
#include "ite_event.h"
#include "ite_pageelements.h"
#include "ite_grab.h"
#include "ite_vpip.h"
#include "ite_framerate.h"
#include "ite_sia_buffer.h"
#include "ite_buffer_management.h"
#include "grab_types.idt.h"

#include "ite_nmf_standard_functions.h"
#include "ite_nmf_framerate.h"

#include <test/api/test.h>
#include <cm/inc/cm_macros.h>

#include <los/api/los_api.h>

extern struct s_grabParams  grabparamsHR;
extern struct s_grabParams  grabparamsLR;
extern volatile t_uint32    g_grabHR_infinite;
extern volatile t_uint32    g_grabLR_infinite;
extern ts_siapicturebuffer  GrabBufferHR[];
extern ts_siapicturebuffer  LCDBuffer;
extern ts_siapicturebuffer  GamBuffer;

extern ts_sia_usecase       usecase;

//private functions
t_uint8                     ITE_launchGrabHRthenLRwithLatency (t_uint32 nbFrames, t_uint32 Xms);
t_uint8                     ITE_launchStartStopVPIPwithLatency (t_uint32 Xms);

/* -----------------------------------------------------------------------
FUNCTION : C_ite_StressTests_cmd
PURPOSE  : Command for Stress tests
------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_StressTests_cmd(
int     a_nb_args,
char    **ap_args)
{
    if (a_nb_args == 3)
    {
        //ITE_StoreTestHeader(a_nb_args,ap_args);
        ITE_StressTests(ap_args[1], ap_args[2]);
    }
    else
    {
        CLI_disp_error("Not correct command arguments\n");
    }


    return (CMD_COMPLETE);
}


/* -----------------------------------------------------------------------
FUNCTION : ITE_WBTests
PURPOSE  : Test WB feature
------------------------------------------------------------------------ */
void
ITE_StressTests(
char    *ap_test_id,
char    *ap_grabvpip_options)
{
    char        test_description[64];
    t_uint32    latency;
    t_uint8     launchError = 0;

    //disable log pageelement
    ITE_Log_RW_pageelements_disable();

    ITE_InitUseCase(ap_test_id, ap_grabvpip_options, &usecase);
    sprintf(
    test_description,
    "grab_stress_XLR%d_YLR%d_XHR%d_YHR%d",
    ( int ) usecase.LR_XSize,
    ( int ) usecase.LR_YSize,
    ( int ) usecase.HR_XSize,
    ( int ) usecase.HR_YSize);

    //perform a "cd" to destination repository for test log
    strcat(g_out_path,"/ite_nmf/test_results_nreg/robustness");
    LOS_Log("Current path: >%s< \n", ( char * ) g_out_path);
    init_test_results_nreg(g_out_path);
    mmte_testStart(ap_test_id, test_description, g_out_path);

    ITE_Start_Env(&usecase, ap_grabvpip_options, 0, 0);

    LOS_Log(
    "Use case: LR_XSize=%d LR_YSize=%d HR_XSize=%d HR_YSize=%d \n\n",
    usecase.LR_XSize,
    usecase.LR_YSize,
    usecase.HR_XSize,
    usecase.HR_YSize,
    NULL,
    NULL);

    //Configure Pipe LR and Pipe HR: both pipe work with one only buffer
    ITE_LRHR_Prepare(&usecase, 0);

#ifdef DSI_DISPLAY
    // configure dsi display
    ITE_configureMcde(&(GrabBufferLR[0]));
    ITE_TestMcdeForDsi(&(GrabBufferLR[0]));
#endif

    //Launch LR grab in an infinite manner
    ITE_LR_Start(INFINITY);
    ITE_HR_Start(INFINITY);

    mmte_testNext("check START/STOP VPIP robustness");
    for (latency = 1; latency < 100; latency++)
    {
        launchError += ITE_launchStartStopVPIPwithLatency(latency);
    }


    if (launchError == FALSE)
    {
        LOS_Log("STOP/START VPIP OK\n\n", NULL, NULL, NULL, NULL, NULL, NULL);
        mmte_testResult(TEST_PASSED);
    }
    else
    {
        LOS_Log("WARNING: Error occured during START/STOP VPIP\n\n", NULL, NULL, NULL, NULL, NULL, NULL);
        mmte_testResult(TEST_FAILED);
    }


    ITE_StopGrab(GRBPID_PIPE_LR);

    ITE_WaitEventTimeout(ITE_EVT_GRABLR_EVENTS, 2000);
    ITE_StopGrab(GRBPID_PIPE_HR);
    ITE_WaitEventTimeout(ITE_EVT_GRABHR_EVENTS, 2000);

    mmte_testNext("check START Grab LR//HR robustness");
    for (latency = 1; latency < 100; latency++)
    {
        launchError += ITE_launchGrabHRthenLRwithLatency(1, latency);
    }


    if (launchError == FALSE)
    {
        LOS_Log("HR//LR Grab with latency OK\n\n", NULL, NULL, NULL, NULL, NULL, NULL);
        mmte_testResult(TEST_PASSED);
    }
    else
    {
        LOS_Log("WARNING: Error occured during HR//LR Grab\n\n", NULL, NULL, NULL, NULL, NULL, NULL);
        mmte_testResult(TEST_FAILED);
    }


    ITE_LRHR_Stop();
    ITE_LRHR_Free();

    ITE_Stop_Env();

    mmte_testEnd();

    //clean  g_out_path
    memset(g_out_path, 0, KlogDirectoryLentgh * sizeof(char));
}

t_uint8
ITE_launchGrabHRthenLRwithLatency(
t_uint32    nbFrames,
t_uint32    Xms)
{
    t_uint8             errorLR = FALSE;
    t_uint8             errorHR = FALSE;
    ITE_StartGrabNb(GRBPID_PIPE_HR, nbFrames);

    //insert latency between grab commands
    LOS_Sleep(Xms);
    ITE_StartGrabNb(GRBPID_PIPE_LR, nbFrames);

    //ITE_DisplaySingleBuffer(&(GrabBufferHR[0]));
    errorHR = ITE_WaitEventTimeout(ITE_EVT_GRABHR_EVENTS, 2000);
    errorLR = ITE_WaitEventTimeout(ITE_EVT_GRABLR_EVENTS, 2000);

    if ((errorLR == FALSE) && (errorHR == FALSE))
    {
        LOS_Log("Grab OK on HR-LR for a latency= %d\n\n", Xms, NULL, NULL, NULL, NULL, NULL);
    }
    else
    {
        if (errorLR == TRUE)
        {
            LOS_Log("Timeout or Error occured on LR for a latency= %d\n\n", Xms, NULL, NULL, NULL, NULL, NULL);
        }


        if (errorHR == TRUE)
        {
            LOS_Log("Timeout or Error occured on HR for a latency= %d\n\n", Xms, NULL, NULL, NULL, NULL, NULL);
        }
    }


    return (errorLR + errorHR);
}


t_uint8
ITE_launchStartStopVPIPwithLatency(
t_uint32    Xms)
{
    t_uint8             errorVPIP = FALSE;

    errorVPIP = STOPVPIP();
    if (errorVPIP == TRUE)
    {
        LOS_Log("Timeout or Error occured on STOPVPIP for a latency= %d\n\n", Xms, NULL, NULL, NULL, NULL, NULL);
        return (errorVPIP);
    }


    LOS_Sleep(Xms);

    errorVPIP = STARTVPIP();
    if (errorVPIP == TRUE)
    {
        LOS_Log("Timeout or Error occured on STARTVPIP for a latency= %d\n\n", Xms, NULL, NULL, NULL, NULL, NULL);
        return (errorVPIP);
    }


    return (errorVPIP);
}

