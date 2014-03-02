/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ite_testenv_utils.h"
#include "cli.h"
#include "ite_datapath_tests.h"

#include "ite_boardinfo.h"
#include "ite_platform.h"
#include "VhcElementDefs.h"

#include "ite_sia_interface_data.h"
#include "ite_sia_bootcmd.h"
#include "ite_init.h"
#include "ite_sia_init.h"

#include "ite_display.h"
#include "ite_event.h"
#include "ite_pageelements.h"
#include "ite_colormatrix.h"
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
#include "ite_main.h"

#include "ite_host2sensor.h"
#include "ite_nmf_Rx_Testpattern.h"

#include <los/api/los_api.h>

extern struct s_grabParams  grabparamsHR;
extern struct s_grabParams  grabparamsLR;
extern volatile t_uint32    g_grabHR_infinite;
extern volatile t_uint32    g_grabLR_infinite;
extern ts_siapicturebuffer  GrabBufferHR[];
extern ts_siapicturebuffer  LCDBuffer;
extern ts_siapicturebuffer  GamBuffer;

extern InputImageSource_te  g_InputImageSource;

extern ts_sia_usecase       usecase;
extern void                 ITE_BMS_Abort (void);

/* -----------------------------------------------------------------------
   FUNCTION : C_ite_datapath_LR_cmd
   PURPOSE  : Command for LR ouput pipe stimulation
   ------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_datapath_LR_cmd(
int     a_nb_args,
char    **ap_args)
{
    if (a_nb_args == 3)
    {
        ITE_LRDatapath(ap_args[1], ap_args[2]);
    }
    else
    {
        CLI_disp_error("Not correct command arguments\n");
    }


    return (CMD_COMPLETE);
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_datapath_LR_cmd
   PURPOSE  : Command for LR ouput pipe stimulation
   ------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_datapath_LR_SENSOR_TUNNING_cmd(
int     a_nb_args,
char    **ap_args)
{
    if (a_nb_args == 3)
    {
        ITE_LRDatapath_SENSOR_TUNNING(ap_args[1], ap_args[2]);
    }
    else
    {
        CLI_disp_error("Not correct command arguments\n");
    }


    return (CMD_COMPLETE);
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_datapath_HR_cmd
   PURPOSE  : Command for HR ouput pipe stimulation
   ------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_datapath_HR_cmd(
int     a_nb_args,
char    **ap_args)
{
    if (a_nb_args == 3)
    {
        ITE_HRDatapath(ap_args[1], ap_args[2]);
    }
    else
    {
        CLI_disp_error("Not correct command arguments\n");
    }


    return (CMD_COMPLETE);
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_datapath_HR_SmiaPP_cmd
   PURPOSE  : Command for HR ouput pipe stimulation
   ------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_datapath_HR_SmiaPP_cmd(
int     a_nb_args,
char    **ap_args)
{
    if (a_nb_args == 3)
    {
        ITE_HRDatapath_SmiaPP(ap_args[1], ap_args[2]);
    }
    else
    {
        CLI_disp_error("Not correct command arguments\n");
    }


    return (CMD_COMPLETE);
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_datapath_HR_SmiaPP_cmd
   PURPOSE  : Command for HR ouput pipe stimulation
   ------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_datapath_HR_SENSOR_TUNNING_Cmd(
int     a_nb_args,
char    **ap_args)
{
    if (a_nb_args == 3)
    {
        ITE_HRDatapath_SmiaPP(ap_args[1], ap_args[2]);
    }
    else
    {
        CLI_disp_error("Not correct command arguments\n");
    }


    return (CMD_COMPLETE);
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_datapath_LRHR_cmd
   PURPOSE  : Command for LR//HR ouput pipe stimulation
   ------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_datapath_LRHR_cmd(
int     a_nb_args,
char    **ap_args)
{
    if (a_nb_args == 3)
    {
        ITE_LRHRDatapath(ap_args[1], ap_args[2]);
    }
    else
    {
        CLI_disp_error("Not correct command arguments\n");
    }


    return (CMD_COMPLETE);
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_datapath_BMS_cmd
   PURPOSE  : Command for BMS ouput pipe stimulation
   ------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_datapath_BMS_cmd(
int     a_nb_args,
char    **ap_args)
{
    if (a_nb_args == 3)
    {
        ITE_BMSDatapath(ap_args[1], ap_args[2]);
    }
    else
    {
        CLI_disp_error("Not correct command arguments\n");
    }


    return (CMD_COMPLETE);
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_FLASH_cmd
   PURPOSE  : Command for BMS ouput pipe stimulation
   ------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_datapath_BMS_FLASH_cmd(
int     a_nb_args,
char    **ap_args)
{
    if (a_nb_args == 3)
    {
        ITE_FLASH_BMSDatapath(ap_args[1], ap_args[2]);
    }
    else
    {
        CLI_disp_error("Not correct command arguments\n");
    }


    return (CMD_COMPLETE);
}


/* -----------------------------------------------------------------------
   FUNCTION : ITE_LRDatapath
   PURPOSE  : Put sensor in color bar test mode, stream on LR pipe and check ouput frame automaticaly
   ------------------------------------------------------------------------ */
void
ITE_LRDatapath(
char    *ap_test_id,
char    *ap_grabvpip_options)
{
    ITE_Log_RW_pageelements_enable();

    ITE_Log_Event_enable();

    mmte_testStart("test_LR_datapath", " test pipe LR with sensor in test mode", g_out_path);

    ITE_InitUseCase(ap_test_id, ap_grabvpip_options, &usecase);

    ITE_Start_Env(&usecase, ap_grabvpip_options, 0, 0);

    //Sensor in test mode

    if (g_InputImageSource == InputImageSource_e_Sensor0)
    {
        ITE_SensorMode(COLORBAR);
    }
    else
    {
        ITE_SetRxTestPattern(RxTestPattern_e_ColourBars);
    }


    ITE_LR_Prepare(&usecase, INFINITY);
    ITE_LR_Start(INFINITY);

    LOS_Log("\n2secs stream on LR pipe\n");
    LOS_Sleep(2000);

    ITE_LR_Stop();

    mmte_testNext("check if colorbar detected in LR buffer");
    if (TestColorBarbuffer(&(GrabBufferLR[0]), COLOR_BAR__DEFAULT_TESTING_POSITION) == 0)
    {
        LOS_Log("\nColor bar successfully detected on GrabBufferLR[0] buffer\n");
        mmte_testResult(TEST_PASSED);
    }
    else
    {
        LOS_Log("\nTEST FAIL: Color bar not properly detected on GrabBufferLR[0] buffer\n");
        mmte_testResult(TEST_FAILED);
    }


    ITE_LR_Free();

    ITE_Stop_Env();

    // end test
    mmte_testEnd();
}


/* -----------------------------------------------------------------------
   FUNCTION : ITE_LRDatapath
   PURPOSE  : Put sensor in color bar test mode, stream on LR pipe and check ouput frame automaticaly
   ------------------------------------------------------------------------ */
void
ITE_LRDatapath_SENSOR_TUNNING(
char    *ap_test_id,
char    *ap_grabvpip_options)
{
    ITE_Log_RW_pageelements_enable();

    ITE_Log_Event_enable();

    mmte_testStart("test_LR_datapath", " test pipe LR with sensor in test mode", g_out_path);

    ITE_InitUseCase(ap_test_id, ap_grabvpip_options, &usecase);

    ITE_Start_Env(&usecase, ap_grabvpip_options, 0, 1);

    if (g_InputImageSource == InputImageSource_e_Sensor0)
    {
        ITE_SensorMode(COLORBAR);
    }
    else
    {
        ITE_SetRxTestPattern(RxTestPattern_e_ColourBars);
    }


    ITE_LR_Prepare(&usecase, INFINITY);
    ITE_LR_Start(INFINITY);

    LOS_Log("\n2secs stream on LR pipe\n");
    LOS_Sleep(2000);

    ITE_LR_Stop();

    mmte_testNext("check if colorbar detected in LR buffer");
    if (TestColorBarbuffer(&(GrabBufferLR[0]), COLOR_BAR__DEFAULT_TESTING_POSITION) == 0)
    {
        LOS_Log("\nColor bar successfully detected on GrabBufferLR[0] buffer\n");
        mmte_testResult(TEST_PASSED);
    }
    else
    {
        LOS_Log("\nTEST FAIL: Color bar not properly detected on GrabBufferLR[0] buffer\n");
        mmte_testResult(TEST_FAILED);
    }


    ITE_LR_Free();

    ITE_Stop_Env();

    // end test
    mmte_testEnd();
}


/* -----------------------------------------------------------------------
   FUNCTION : ITE_HRDatapath
   PURPOSE  : Put sensor in color bar test mode, stream on HR pipe and check ouput frame automaticaly
   ------------------------------------------------------------------------ */
void
ITE_HRDatapath(
char    *ap_test_id,
char    *ap_grabvpip_options)
{
    //char out_filename[64];
    //t_uint16 framerate=30;
    //t_uint8 error=TRUE;
    //disable log pageelement
    //MMITE_Log_RW_pageelements_disable();
    ITE_Log_RW_pageelements_enable();

    ITE_Log_Event_enable();

    /* HCL Services, alloc, Log, IT, xti ....*/
    // ITE_SKEInit();
    // start
    mmte_testStart("test_HR_datapath", " test pipe HR with sensor in test mode", g_out_path);

    ITE_InitUseCase(ap_test_id, ap_grabvpip_options, &usecase);

    ITE_Start_Env(&usecase, ap_grabvpip_options, 0, 0);

    if (g_InputImageSource == InputImageSource_e_Sensor0)
    {
        ITE_SensorMode(COLORBAR);
    }
    else
    {
        ITE_SetRxTestPattern(RxTestPattern_e_ColourBars);
    }


    ITE_HR_Prepare(&usecase, INFINITY);
    ITE_HR_Start(INFINITY);

    LOS_Log("\n2secs stream on HR pipe\n");
    LOS_Sleep(2000);

    ITE_HR_Stop();

    mmte_testNext("check if colorbar detected in HR buffer");
    if (TestColorBarbuffer(&(GrabBufferHR[0]), COLOR_BAR__DEFAULT_TESTING_POSITION) == 0)
    {
        LOS_Log("\nColor bar successfully detected on GrabBufferHR[0] buffer\n");
        mmte_testResult(TEST_PASSED);
    }
    else
    {
        LOS_Log("\nTEST FAIL: Color bar not properly detected on GrabBufferHR[0] buffer\n");
        mmte_testResult(TEST_FAILED);
    }


    ITE_HR_Free();

    ITE_Stop_Env();

    mmte_testEnd();
}


/* -----------------------------------------------------------------------
   FUNCTION : ITE_HRDatapath
   PURPOSE  : Put sensor in color bar test mode, stream on HR pipe and check ouput frame automaticaly
   ------------------------------------------------------------------------ */
void
ITE_HRDatapath_SmiaPP(
char    *ap_test_id,
char    *ap_grabvpip_options)
{
    ITE_Log_RW_pageelements_enable();

    ITE_Log_Event_enable();
    mmte_testStart("test_HR_datapath", " test pipe HR with sensor in test mode", g_out_path);

    ITE_InitUseCase(ap_test_id, ap_grabvpip_options, &usecase);

    ITE_Start_Env(&usecase, ap_grabvpip_options, 1, 0);

    if (g_InputImageSource == InputImageSource_e_Sensor0)
    {
        ITE_SensorMode(COLORBAR);
    }
    else
    {
        ITE_SetRxTestPattern(RxTestPattern_e_ColourBars);
    }


    ITE_HR_Prepare(&usecase, INFINITY);
    ITE_HR_Start(INFINITY);

    LOS_Log("\n2secs stream on HR pipe\n");
    LOS_Sleep(2000);

    ITE_HR_Stop();

    mmte_testNext("check if colorbar detected in HR buffer");
    if (TestColorBarbuffer(&(GrabBufferHR[0]), COLOR_BAR__DEFAULT_TESTING_POSITION) == 0)
    {
        LOS_Log("\nColor bar successfully detected on GrabBufferHR[0] buffer\n");
        mmte_testResult(TEST_PASSED);
    }
    else
    {
        LOS_Log("\nTEST FAIL: Color bar not properly detected on GrabBufferHR[0] buffer\n");
        mmte_testResult(TEST_FAILED);
    }


    ITE_HR_Free();

    ITE_Stop_Env();
    mmte_testEnd();
}


/* -----------------------------------------------------------------------
   FUNCTION : ITE_HRDatapath
   PURPOSE  : Put sensor in color bar test mode, stream on HR pipe and check ouput frame automaticaly
   ------------------------------------------------------------------------ */
void
ITE_HRDatapath_SENSOR_TUNNING(
char    *ap_test_id,
char    *ap_grabvpip_options)
{
    //char out_filename[64];
    ITE_Log_RW_pageelements_enable();

    ITE_Log_Event_enable();

    mmte_testStart("test_HR_datapath", " test pipe HR with sensor in test mode", g_out_path);

    ITE_InitUseCase(ap_test_id, ap_grabvpip_options, &usecase);

    ITE_Start_Env(&usecase, ap_grabvpip_options, 0, 1);
    if (g_InputImageSource == InputImageSource_e_Sensor0)
    {
        ITE_SensorMode(COLORBAR);
    }
    else
    {
        ITE_SetRxTestPattern(RxTestPattern_e_ColourBars);
    }


    ITE_HR_Prepare(&usecase, INFINITY);
    ITE_HR_Start(INFINITY);

    LOS_Log("\n2secs stream on HR pipe\n");
    LOS_Sleep(2000);

    ITE_HR_Stop();

    mmte_testNext("check if colorbar detected in HR buffer");
    if (TestColorBarbuffer(&(GrabBufferHR[0]), COLOR_BAR__DEFAULT_TESTING_POSITION) == 0)
    {
        LOS_Log("\nColor bar successfully detected on GrabBufferHR[0] buffer\n");
        mmte_testResult(TEST_PASSED);
    }
    else
    {
        LOS_Log("\nTEST FAIL: Color bar not properly detected on GrabBufferHR[0] buffer\n");
        mmte_testResult(TEST_FAILED);
    }


    ITE_HR_Free();

    ITE_Stop_Env();

    mmte_testEnd();
}


/* -----------------------------------------------------------------------
   FUNCTION : ITE_LRHRDatapath
   PURPOSE  : Put sensor in color bar test mode, stream on both HR and LR pipes and check ouput frames  automaticaly
   ------------------------------------------------------------------------ */
void
ITE_LRHRDatapath(
char    *ap_test_id,
char    *ap_grabvpip_options)
{
    ITE_Log_RW_pageelements_enable();

    ITE_Log_Event_enable();

    mmte_testStart("test_LRHR_datapath", " test both pipe LR and HR with sensor in test mode", g_out_path);

    ITE_InitUseCase(ap_test_id, ap_grabvpip_options, &usecase);
    ITE_Start_Env(&usecase, ap_grabvpip_options, 0, 0);

    if (g_InputImageSource == InputImageSource_e_Sensor0)
    {
        ITE_SensorMode(COLORBAR);
    }
    else
    {
        ITE_SetRxTestPattern(RxTestPattern_e_ColourBars);
    }


    ITE_LRHR_Prepare(&usecase, INFINITY);
    ITE_LRHR_Start(INFINITY);

    LOS_Log("\n2secs stream on LR and HR pipes\n");
    LOS_Sleep(2000);

    ITE_LRHR_Stop();

    mmte_testNext("check if colorbar detected in LR buffer");
    if (TestColorBarbuffer(&(GrabBufferLR[0]), COLOR_BAR__DEFAULT_TESTING_POSITION) == 0)
    {
        LOS_Log("\nColor bar successfully detected on GrabBufferLR[0] buffer\n");
        mmte_testResult(TEST_PASSED);
    }
    else
    {
        LOS_Log("\nTEST FAIL: Color bar not properly detected on GrabBufferLR[0] buffer\n");
        mmte_testResult(TEST_FAILED);
    }


    mmte_testNext("check if colorbar detected in HR buffer");
    if (TestColorBarbuffer(&(GrabBufferHR[0]), COLOR_BAR__DEFAULT_TESTING_POSITION) == 0)
    {
        LOS_Log("\nColor bar successfully detected on GrabBufferHR[0] buffer\n");
        mmte_testResult(TEST_PASSED);
    }
    else
    {
        LOS_Log("\nTEST FAIL: Color bar not properly detected on GrabBufferHR[0] buffer\n");
        mmte_testResult(TEST_FAILED);
    }


    ITE_LRHR_Free();

    ITE_Stop_Env();

    mmte_testEnd();
}


/* -----------------------------------------------------------------------
   FUNCTION : ITE_BMSDatapath
   PURPOSE  : Put sensor in color bar test mode, stream on BMS pipe and check ouput frame automaticaly
   ------------------------------------------------------------------------ */
void
ITE_BMSDatapath(
char    *ap_test_id,
char    *ap_grabvpip_options)
{
    //char out_filename[64];
    //t_uint16 framerate=30;
    //t_uint8 error=TRUE;
    //disable log pageelement
    //MMITE_Log_RW_pageelements_disable();
    Sensor_Output_Mode_ts   *p_Sensor_Output_Mode_Data = NULL;
    t_uint32                outpuModeCount = 0;
    t_uint32                sensorOutputModeDataSize = 0;
    t_uint32                sensorOutputModeSturctSize = sizeof(Sensor_Output_Mode_ts);
    float fps=0.0;
    ITE_Log_RW_pageelements_enable();

    ITE_Log_Event_enable();

    /* HCL Services, alloc, Log, IT, xti ....*/
    //ITE_SKEInit();
    // start
    mmte_testStart("test_BMSdatapath", " test pipe BMS with sensor in test mode", g_out_path);

    ITE_InitUseCase(ap_test_id, ap_grabvpip_options, &usecase);

    ITE_Start_Env(&usecase, ap_grabvpip_options, 0, 0);

    //Sensor in test mode
    if (g_InputImageSource == InputImageSource_e_Sensor0)
    {
        ITE_SensorMode(COLORBAR);
    }
    else
    {
        ITE_SetRxTestPattern(RxTestPattern_e_ColourBars);
    }

    p_Sensor_Output_Mode_Data = ( Sensor_Output_Mode_ts * ) pOutpuModeBuffer->logAddress;
    sensorOutputModeDataSize = ITE_readPE(ReadLLAConfig_Status_u32_sensor_Output_Mode_data_size_Byte0);

    for (outpuModeCount = 0; outpuModeCount < sensorOutputModeDataSize / sensorOutputModeSturctSize; outpuModeCount++)
    {
        if (p_Sensor_Output_Mode_Data[outpuModeCount].u32_data_format == PreBoot.sensor.uwCsiRawFormat_MSByte)
        {
            LOS_Log("outpuModeCount = %d", outpuModeCount);
            break;
        }
    }


    // Set BMS usecase as par 0th sensor output mode
    usecase.BMS_woi_X = p_Sensor_Output_Mode_Data[outpuModeCount].u32_woi_res_width;
    usecase.BMS_woi_Y = p_Sensor_Output_Mode_Data[outpuModeCount].u32_woi_res_height;

    usecase.BMS_YSize = p_Sensor_Output_Mode_Data[outpuModeCount].u32_output_res_height;
    usecase.BMS_XSize = p_Sensor_Output_Mode_Data[outpuModeCount].u32_output_res_width;

    usecase.framerate_x100 = p_Sensor_Output_Mode_Data[outpuModeCount].u32_max_frame_rate_x100;
    fps = usecase.framerate_x100/100;
    ITE_SetFrameRateControl(fps,fps);
    usecase.data_format = p_Sensor_Output_Mode_Data[outpuModeCount].u32_data_format;

    ITE_BMS_Prepare(&usecase, INFINITY);

    //Grab one frame
    ITE_BMS_Start(1);

    LOS_Log("\nStream 1 frame on BMS\n");
    LOS_Sleep(4000);

    // Record RAW8 grab frame to File
    //ITE_StoreinBMPFile("imageBMS",&(GrabBufferBMS[0]));
    mmte_testNext("check if colorbar detected in BMS buffer");
    if (TestColorBarbuffer(&(GrabBufferBMS[0]), COLOR_BAR__DEFAULT_TESTING_POSITION) == 0)
    {
        LOS_Log("\nColor bar successfully detected on GrabBufferBMS[0] buffer\n");
        mmte_testResult(TEST_PASSED);
    }
    else
    {
        LOS_Log("\nTEST FAIL: Color bar not properly detected on GrabBufferBMS[0] buffer\n");
        mmte_testResult(TEST_FAILED);
    }


    //FIXME: commented for bugged xP70 FW
    ITE_BMS_Stop();

    //Temporary Patch: use of LR
    ITE_LR_Free();

    ITE_BMS_Free();

    ITE_Stop_Env();

    // end test
    mmte_testEnd();
}


/* -----------------------------------------------------------------------
   FUNCTION : ITE_FLASH_BMSDatapath
   PURPOSE  : Put sensor in color bar test mode, stream on BMS pipe and check ouput frame automaticaly
   ------------------------------------------------------------------------ */
void
ITE_FLASH_BMSDatapath(
char    *ap_test_id,
char    *ap_grabvpip_options)
{
    Coin_te status_coin;

    ITE_Log_RW_pageelements_enable();

    ITE_Log_Event_enable();
    mmte_testStart("test_BMSdatapath", " test pipe BMS with sensor in test mode", g_out_path);

    ITE_InitUseCase(ap_test_id, ap_grabvpip_options, &usecase);

    ITE_Start_Env(&usecase, ap_grabvpip_options, 0, 0);

    usecase.BMS_Mode = BMSSTILL;
    usecase.BMS_output = BAYERSTORE2;
    usecase.BMS_GrbFormat = GRBFMT_RAW8;


    LOS_Log("flash mode = 1 no coin \n");
    ITE_BMS_Prepare(&usecase, INFINITY);
    ITE_writePE(FlashControl_e_Flag_FlashMode_Byte0, 1);

    //Grab one frame
    ITE_BMS_Start(1);

    LOS_Log("\nStream 1 frame on BMS\n");
    LOS_Sleep(5000);
    status_coin = (Coin_te) ITE_readPE(SystemConfig_Status_e_Coin_Status_Byte0);
    ITE_writePE(SystemSetup_e_Coin_Ctrl_Byte0, !status_coin);

    LOS_Log("\nStream 1 frame on BMS\n");
    LOS_Sleep(5000);
    ITE_BMS_Stop();

    LOS_Log("flash mode = 1  coin=0 \n");
    ITE_BMS_Prepare(&usecase, INFINITY);
    ITE_writePE(FlashControl_e_Flag_FlashMode_Byte0, 1);
    status_coin = (Coin_te) ITE_readPE(SystemConfig_Status_e_Coin_Status_Byte0);
    ITE_writePE(SystemSetup_e_Coin_Ctrl_Byte0, !status_coin);

    //Grab one frame
    ITE_BMS_Start(1);

    LOS_Log("\nStream 1 frame on BMS\n");
    LOS_Sleep(5000);
    ITE_BMS_Stop();

    LOS_Log("flash mode = 1 request 10 frames coin=1 \n");
    ITE_BMS_Prepare(&usecase, INFINITY);
    ITE_writePE(FlashControl_e_Flag_FlashMode_Byte0, 1);
    status_coin = (Coin_te) ITE_readPE(SystemConfig_Status_e_Coin_Status_Byte0);
    ITE_writePE(SystemSetup_e_Coin_Ctrl_Byte0, !status_coin);

    //Grab one frame
    ITE_BMS_Start(10);

    LOS_Log("\nStream 1 frame on BMS\n");
    LOS_Sleep(5000);

    //ITE_writePE(FlashControl_e_Flag_FlashMode_Byte0, 0);
    ITE_BMS_Abort();
    LOS_Sleep(5000);
    ITE_BMS_Stop();

    mmte_testNext("check if colorbar detected in BMS buffer");
    if (TestColorBarbuffer(&(GrabBufferBMS[0]), COLOR_BAR__DEFAULT_TESTING_POSITION) == 0)
    {
        LOS_Log("\nColor bar successfully detected on GrabBufferBMS[0] buffer\n");
        mmte_testResult(TEST_PASSED);
    }
    else
    {
        LOS_Log("\nTEST FAIL: Color bar not properly detected on GrabBufferBMS[0] buffer\n");
        mmte_testResult(TEST_FAILED);
    }


    //Temporary Patch: use of LR
    ITE_LR_Free();

    ITE_BMS_Free();

    ITE_Stop_Env();

    // end test
    mmte_testEnd();
}

