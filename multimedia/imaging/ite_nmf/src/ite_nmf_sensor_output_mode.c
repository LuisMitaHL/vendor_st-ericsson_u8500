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

#include "ite_nmf_nvm_functions.h"
#include "ite_nmf_statistic_functions.h"
#include "ite_testenv_utils.h"
#include "ite_nmf_standard_functions.h"

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
#include "ite_nmf_headers.h"
#include "nvm.h"

//#include "ite_nmf_tuning_functions.h"
#include <cm/inc/cm_macros.h>

#include <los/api/los_api.h>
#include "ite_main.h"
#include "sia.h"
#include "ite_nmf_sensor_output_mode.h"

extern tps_siaMetaDatabuffer    pOutpuModeBuffer;

#define CMD_ERROR       (CMD_COMPLETION) 1
#define ISP_REGION_IO   0xE0000000
#define ARM_REGION_IO   0x01000000

#define MAX_RETRIES     50

/* -----------------------------------------------------------------------
   FUNCTION : ITE_NMF_getSensorOutputModeDataSize
   PURPOSE  :
   ------------------------------------------------------------------------ */
t_uint32
ITE_NMF_getSensorOutputModeDataSize(void)
{
    return (ITE_readPE(ReadLLAConfig_Status_u32_sensor_Output_Mode_data_size_Byte0));
}


/* -----------------------------------------------------------------------
   FUNCTION : ITE_NMF_writeSensorOutputModeBufferAddress
   PURPOSE  :
   ------------------------------------------------------------------------ */
void
ITE_NMF_writeSensorOutputModeBufferAddress(
t_uint32    address)
{
    ITE_writePE(ReadLLAConfig_Control_ptr32_Sensor_Output_Mode_Data_Address_Byte0, address);
}


/* -----------------------------------------------------------------------
   FUNCTION : ITE_NMF_toggleSensorOutputModeControlCoin
   PURPOSE  :
   ------------------------------------------------------------------------ */
RESULT_te
ITE_NMF_toggleSensorOutputModeControlCoin(void)
{
    t_uint32    SensorOutputModeSatus = ITE_readPE(ReadLLAConfig_Status_e_Coin_ReadLLAConfigStatus_Byte0);
    t_uint32    SensorOutputModeControl = ITE_readPE(ReadLLAConfig_Control_e_Coin_ReadLLAConfigControl_Byte0);

    union u_ITE_Event   event;

    LOS_Log(
    "SensorOutputModeSatus = %d\t, SensorOutputModeControl = %d\t",
    SensorOutputModeSatus,
    SensorOutputModeControl);

    // Refresh event
    ITE_RefreshEventCount(Event0_Count_u16_EVENT0_30_Sensor_Output_Mode_Export_Notification_Byte0);

    if (Coin_e_Heads == SensorOutputModeSatus)
    {
        ITE_writePE(ReadLLAConfig_Control_e_Coin_ReadLLAConfigControl_Byte0, Coin_e_Tails);
    }
    else
    {
        ITE_writePE(ReadLLAConfig_Control_e_Coin_ReadLLAConfigControl_Byte0, Coin_e_Heads);
    }

    //ITE_NMF_WaitEvent(ISP_SENSOR_OUTPUT_MODE_EXPORT_NOTIFICATION);

    do
    {
        event = ITE_WaitEvent(ISP_SENSOR_OUTPUT_MODE_EXPORT_NOTIFICATION_EVENTS);
        ITE_traceISPCTLevent(event);
        if (event.type == ITE_EVT_ISPCTL_ERROR)
        {
            LOS_Log("\nITE_EVT_ISPCTL_ERROR \n");
            break;
        }
    } while (!(event.ispctlInfo.info_id == ISP_SENSOR_OUTPUT_MODE_EXPORT_NOTIFICATION));

    ITE_GetEventCount(Event0_Count_u16_EVENT0_30_Sensor_Output_Mode_Export_Notification_Byte0);

    // What is Success criterion? US 5/26
    return (SUCCESS);
}


/* -----------------------------------------------------------------------
   FUNCTION : ITE_NMF_checkSensorOutputModeStatusCoin
   PURPOSE  :
   ------------------------------------------------------------------------ */
void
ITE_NMF_checkSensorOutputModeStatusCoin(void)
{
    t_uint32    SensorOutputModeSatus = ITE_readPE(ReadLLAConfig_Status_e_Coin_ReadLLAConfigStatus_Byte0);
    t_uint32    SensorOutputModeControl = ITE_readPE(ReadLLAConfig_Control_e_Coin_ReadLLAConfigControl_Byte0);

    while (SensorOutputModeSatus != SensorOutputModeControl)
    {
        SensorOutputModeSatus = ITE_readPE(ReadLLAConfig_Status_e_Coin_ReadLLAConfigStatus_Byte0);
    }


    return;
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_SensorOutputModehelp_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_SensorOutputModehelp_cmd(
int     a_nb_args,
char    **ap_args)
{
  UNUSED(ap_args);
  
    LOS_Log("******************************** SensorOutputMode HELP *****************************\n");

    if (a_nb_args == 1)
    {
        CLI_disp_error("\n");
        CLI_disp_error("TestSensorOutputModeDataSize: Checks size of SensorOutputMode data provided by FW \n");
        CLI_disp_error("TestSensorOutputModeData: Checks SensorOutputMode data provided by FW \n");
    }
    else
    {
        CLI_disp_error("Not correct command arguments\n");
        return (CMD_ERROR);
    }


    LOS_Log("******************************** SensorOutputMode HELP END**************************\n");
    return (CMD_COMPLETE);
}


CMD_COMPLETION
C_ite_dbg_Print_SensorOutputMode_Data_cmd(
int     a_nb_args,
char    **ap_args)
{
    t_uint16    SensorOutputModeDataSize = 0;
    char        mess[256];

    MMTE_TEST_START(
    "TestSensorOpModeData",
    "/ite_nmf/test_results_nreg/SensorOutputMode_tests",
    "Test TestSensorOpModeData");

    sprintf(mess, "Testing for TestSensorOpModeData");
    mmte_testNext(mess);

    LOS_Log("********************* SensorOutputMode DATA SUPPORT TEST **************************\n");

    if (a_nb_args > 1)
    {
        if (0 == strcmp(ap_args[1], "help"))
        {
            CLI_disp_error("TestSensorOutputModeData : Prints SensorOutputMode data provided by ISP FW \n");
            CLI_disp_error("Syntax : TestSensorOutputModeData \n\n");
        }
    }
    else
    {
        SensorOutputModeDataSize = ITE_NMF_getSensorOutputModeDataSize();
        if (0 != SensorOutputModeDataSize)
        {
            LOS_Log("SensorOutputModeDataSize = %d\n", SensorOutputModeDataSize);
            ITE_NMF_Print_SensorOutputMode_Data(( unsigned int * ) (pOutpuModeBuffer->logAddress));
            MMTE_TEST_PASSED();
            return (CMD_COMPLETE);
        }
        else
        {
            LOS_Log("*** SensorOutputMode data not supported by sensor ***\n");
            MMTE_TEST_FAILED();
            return (CMD_ERROR);
        }
    }


    MMTE_TEST_END();
    return (CMD_COMPLETE);
}


void
ITE_NMF_Get_SensorOutputMode_Data(void)
{
    t_uint32                SensorOutputMode_data_size = 0;
    char                    mess[256],
                            comment[256];
    unsigned int            *SensorOutputModeBufferPtr = NULL;
    ts_siaMetaDatabuffer    SensorOutputModeBuffer = { 0,
                            0,
                            0,
                            0,
                            0 };
    Sensor_Output_Mode_ts   *p_Sensor_Output_Mode_Data = ( Sensor_Output_Mode_ts * ) SensorOutputModeBufferPtr;
    int                     data_size = ITE_NMF_getSensorOutputModeDataSize();
    RESULT_te               result = SUCCESS;
    MMTE_TEST_START(
    "SensorOutputMode_Data",
    "/ite_nmf/test_results_nreg/SensorOutputMode_tests",
    "SensorOutputMode_Data check");

    UNUSED(p_Sensor_Output_Mode_Data);
    UNUSED(SensorOutputModeBuffer);
    UNUSED(SensorOutputMode_data_size);

    LOS_Log("data_size = %d\n", data_size);

    snprintf(mess, sizeof(mess), "Testing for TestSensorOutputMode_Data_SUPPORTED");
    mmte_testNext(mess);

    if (0 == data_size)
    {
        LOS_Log("*************** SensorOutputMode  DATA  NOT  SUPPORTED *********************\n");
        MMTE_TEST_COMMENT("FAILED");
    }


    // allocate a buffer..
    SensorOutputModeBufferPtr = ( unsigned int * ) pOutpuModeBuffer->ISPBufferBaseAddress;

    LOS_Log("SensorOutputMode buffer creation done\n");

    // assign data_address value
    ITE_NMF_writeSensorOutputModeBufferAddress((t_uint32) SensorOutputModeBufferPtr);
    LOS_Log("SensorOutputMode buffer address passed to FW\n");

    // toggle SensorOutputMode coin
    LOS_Log("SensorOutputMode coin toggled\n");
    result = ITE_NMF_toggleSensorOutputModeControlCoin();

    if (result == SUCCESS)
    {
        // wait for notification
        result = ITE_NMF_Wait_For_SensorOutputMode_Notification();
        LOS_Log("SensorOutputMode notificatino recievd\n");
    }
    else
    {
        snprintf(comment, sizeof(comment), "SensorOutputMode Notifcation Not Received. Test Failed");
        MMTE_TEST_COMMENT(comment);
    }


    if (result == SUCCESS)
    {
        //Printing all possible modes supported into the buffer
        ITE_NMF_Print_SensorOutputMode_Data(( unsigned int * ) pOutpuModeBuffer->logAddress);

        // now free buffer
        //ITE_NMF_freeSensorOutputModeBuffer(&SensorOutputModeBuffer);
        LOS_Log("******************* Reading SensorOutputMode done ************************\n");
    }
    else
    {
        snprintf(comment, sizeof(comment), "EVENT ERROR ENCOUNTERED ,EVENT COUNT NOT EQUAL TO 1");
        MMTE_TEST_COMMENT(comment);
    }


    MMTE_TEST_PASSED();
}


/* -----------------------------------------------------------------------
   FUNCTION : ITE_NMF_Test_SensorOutputMode_Export_Done
   PURPOSE  :
   ------------------------------------------------------------------------ */
RESULT_te
ITE_NMF_Wait_For_SensorOutputMode_Notification(void)
{
    t_uint16    SensorOutputMoMode_counter = 0;
    t_uint32    SensorOutputMode_control = 0;
    t_uint32    SensorOutputMode_status = 0;

    SensorOutputMode_control = ITE_readPE(ReadLLAConfig_Control_e_Coin_ReadLLAConfigControl_Byte0);
    SensorOutputMode_status = ITE_readPE(ReadLLAConfig_Status_e_Coin_ReadLLAConfigStatus_Byte0);

    while (SensorOutputMode_control != SensorOutputMode_status)
    {
        SensorOutputMode_status = ITE_readPE(ReadLLAConfig_Status_e_Coin_ReadLLAConfigStatus_Byte0);
        SensorOutputMoMode_counter++;
        LOS_Sleep(100);

        // Removing the Infinite Loop
        if (SensorOutputMoMode_counter >= MAX_RETRIES)
        {
            return (FAILURE);
        }
    }


    return (SUCCESS);
}


CMD_COMPLETION
ITE_NMF_Print_SensorOutputMode_Data(
unsigned int    *SensorOutputModeBufferPtr)
{
    char                    mess[256];
    Sensor_Output_Mode_ts   *p_Sensor_Output_Mode_Data = ( Sensor_Output_Mode_ts * ) SensorOutputModeBufferPtr;
    t_uint32                     count = 0;
    t_uint32                     data_size = ITE_NMF_getSensorOutputModeDataSize();
    LOS_Log("data_size = %d\n", data_size);

    snprintf(
    mess,
    sizeof(mess),
    "\nmode,woi_res_width,woi_res_height,output_res_width,output_res_height,data_format,usage_restriction_bitmask,max_frame_rate_x100\n");

    for (count = 0; count < data_size / sizeof(Sensor_Output_Mode_ts); count++)
    {
#if 0
        snprintf(mess, sizeof(mess), "\n========== Mode %u==============\n", count);
        MMTE_TEST_COMMENT(mess);

        snprintf(
        mess,
        sizeof(mess),
        "woi_res_width    = %u X woi_res_height    = %u \n",
        p_Sensor_Output_Mode_Data[count].u32_woi_res_width,
        p_Sensor_Output_Mode_Data[count].u32_woi_res_height);
        MMTE_TEST_COMMENT(mess);

        snprintf(
        mess,
        sizeof(mess),
        "output_res_width = %u X output_res_height = %u\n",
        p_Sensor_Output_Mode_Data[count].u32_output_res_width,
        p_Sensor_Output_Mode_Data[count].u32_output_res_height);
        MMTE_TEST_COMMENT(mess);

        snprintf(mess, sizeof(mess), "data_format = 0x%X\n", p_Sensor_Output_Mode_Data[count].u32_data_format);
        MMTE_TEST_COMMENT(mess);

        snprintf(
        mess,
        sizeof(mess),
        "usage_restriction_bitmask = %u, max_frame_rate_x100 = %u\n",
        p_Sensor_Output_Mode_Data[count].u32_usage_restriction_bitmask,
        p_Sensor_Output_Mode_Data[count].u32_max_frame_rate_x100);
        MMTE_TEST_COMMENT(mess);
#else
        snprintf(mess, sizeof(mess), "%u,",(unsigned int) count);
        MMTE_TEST_COMMENT(mess);

        snprintf(
        mess,
        sizeof(mess),
        "%u,%u,",
        p_Sensor_Output_Mode_Data[count].u32_woi_res_width,
        p_Sensor_Output_Mode_Data[count].u32_woi_res_height);
        MMTE_TEST_COMMENT(mess);

        snprintf(
        mess,
        sizeof(mess),
        "%u,%u,",
        p_Sensor_Output_Mode_Data[count].u32_output_res_width,
        p_Sensor_Output_Mode_Data[count].u32_output_res_height);
        MMTE_TEST_COMMENT(mess);

        snprintf(mess, sizeof(mess), "%X,", p_Sensor_Output_Mode_Data[count].u32_data_format);
        MMTE_TEST_COMMENT(mess);

        snprintf(
        mess,
        sizeof(mess),
        "%u,%u\n",
        p_Sensor_Output_Mode_Data[count].u32_usage_restriction_bitmask,
        p_Sensor_Output_Mode_Data[count].u32_max_frame_rate_x100);
        MMTE_TEST_COMMENT(mess);
#endif
    }


    return (CMD_COMPLETE);
}

