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

#include "ite_nmf_sms_tests.h"
#include "hi_register_acces.h"
#include "pictor.h"
#include "ite_testenv_utils.h"
#include "ite_nmf_standard_functions.h"
#include "ite_boardinfo.h"
#include "ite_platform.h"
#include "VhcElementDefs.h"

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
#include "ite_event.h"

#undef MODE_FETCH_LOGIC_1
#undef MODE_FETCH_LOGIC_2
#undef MODE_FETCH_LOGIC_3

//ONLY NEED TO CHANGE HERE, TO CHANGE THE LOGIC
#define MODE_FETCH_LOGIC_3

#define SMS_FPS_STEP_SIZE_X_100  300
//percentage deviation from 1/fps
#define SMS_EXPOSURE_PERCENTAGE_DEVIATION_EXPECTED  0.03

/* Globals */
extern tps_siaMetaDatabuffer pOutpuModeBuffer;
extern ts_sensInfo SensorsInfo[2];
extern t_uint32 ite_sensorselect;

t_cmd_list ite_cmd_list_sms[] =
{
  { "SMSHelp", C_ite_dbg_SMSHelp_cmd, "SMSHelp: SMSHelp\n" },
  { "SMSSanityTest", C_ite_dbg_SMSSanityTest_cmd, "SMSSanityTest: SMSSanityTest\n" },
  { "SMSTest", C_ite_dbg_SMSTest_cmd, "SMSTest: SMSTest <wait|nowait> WOI_x WOI_y OUT_x OUT_y FrameRate_x100 CsiRawFormat\n" }
};

typedef enum e_sms_result {
  SMS_SUCCESS=0,
  SMS_FAILURE=1
}SMS_RESULT_te;

#ifdef SMS_DEBUG
t_uint32  debug_max_exp_by_exp_step_dev = 0;
t_uint32  debug_min_exp_by_exp_step_dev = 0;
#endif

/* Prototypes */
static void Set_SMS_Params(t_uint32 u32_WOI_x,
                           t_uint32 u32_WOI_y,
                           t_uint32 u32_OUT_x,
                           t_uint32 u32_OUT_y,
                           t_uint32 u32_FrameRate_x100,
                           t_uint32 u32_CsiRawFormat);
static t_uint16 Toggle_SMS_Coin(void);
static SMS_RESULT_te Check_SMSParams_Absorbed(t_uint16 smsInterruptCount);
static SMS_RESULT_te Wait_SMS_Complete(t_uint16 smsInterruptCount);



/* Exported Interfaces */

/* -----------------------------------------------------------------------
FUNCTION : Init_SMS_CmdList
PURPOSE  :
------------------------------------------------------------------------ */
void Init_SMS_CmdList()
{
     CLI_register_interface("SMS", sizeof(ite_cmd_list_sms)/(sizeof(ite_cmd_list_sms[0])), ite_cmd_list_sms, 1);
}


/* -----------------------------------------------------------------------
FUNCTION : C_ite_dbg_SMSHelp_cmd
PURPOSE  : Displays SMS Test options for the user.
------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_SMSHelp_cmd(int a_nb_args,char **ap_args)
{
 UNUSED(ap_args);

    if (a_nb_args == 1)
    {
        CLI_disp_msg("\n");

        CLI_disp_msg("SMSHelp: Displays this Help \n");
        CLI_disp_msg("SMSHelp[Syntax]: SMSHelp \n\n");

        CLI_disp_msg("SMSSanityTest: Sets SMS params for all modes and max framerates, one by one. \n");
        CLI_disp_msg("SMSSanityTest[Syntax]: SMSSanityTest\n\n");

        CLI_disp_msg("SMSTest: Sets SMS params, toggles SMS coin and waits SMS Done event. \n");
        CLI_disp_msg("SMSTest[Syntax]: SMSTest <datapath> <wait|nowait> WOI_x WOI_y OUT_x OUT_y FrameRate_x100 CsiRawFormat \n\n");
        CLI_disp_msg("SMSTest[Example]: SMSTest LR wait 3280 2464 1640 1232 2400 0x0A0A \n\n");


    }
    else
    {
        CLI_disp_error("Not correct command arguments\n");
        return (CMD_ERR_ARGS);
    }

    return (CMD_COMPLETE);
}


/* -----------------------------------------------------------------------
FUNCTION : C_ite_dbg_SMSSanityTest_cmd
PURPOSE  :
------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_SMSSanityTest_cmd(int a_nb_args, char ** ap_args)
{
  char    mess[256], comment[256];
  UNUSED(ap_args);
  
  t_uint32    fps_x100 = 0, test_id = 0;
  t_uint16    u32_CsiRawFormat = 0x0A08, mode_id=0, total_modes=0, smsInterruptCount = 0;
  SMS_RESULT_te       result;
  Sensor_Output_Mode_ts* ptr_bmsmodeinfo = (Sensor_Output_Mode_ts*)(pOutpuModeBuffer->logAddress);

  MMTE_TEST_START(
  "SMSSanityTest",
  "/ite_nmf/test_results_nreg/modecheck",
  "SMS Sanity Test Suite");

  if (a_nb_args == 1)
  {
#ifdef MODE_FETCH_LOGIC_1
      //MODE_FETCH_LOGIC_ 1: choose sensor data out format
      //MODE_FETCH_LOGIC_ should be changed in future when ReadLLASensor Settings interface changes.

      switch(SensorsInfo[ite_sensorselect].config.colorDepth)
      {
          case (8)   :u32_CsiRawFormat = 0x0808; break;
          case (10)  :u32_CsiRawFormat = 0x0A0A; break;
          case (12)  :u32_CsiRawFormat = 0x0C0C; break;
          case (108) :u32_CsiRawFormat = 0x0A08; break;
          case (128) :u32_CsiRawFormat = 0x0C08; break;
      }
#endif


#ifdef MODE_FETCH_LOGIC_2
      //MODE_FETCH_LOGIC_2: choose data format from LLAConfigStatus Page
      ts_bmsmodeinfo  bmsmodeinfo;

      //get total no of modes
      total_modes = ITE_readPE(ReadLLAConfig_Status_u16_number_of_modes_Byte0);

      //outer loop for modes
      for (mode_id=0; mode_id < total_modes; mode_id++)
      {
        bmsmodeinfo = ITE_getmode(mode_id);

        switch (bmsmodeinfo.data_format)
        {
            case DataFormat_e_RAW10:

                u32_CsiRawFormat = 0x0A0A;
                break;

            case DataFormat_e_RAW8:

                u32_CsiRawFormat = 0x0808;
                break;

            case DataFormat_e_RAW8_DPCM:

                u32_CsiRawFormat = 0x0A08;
                break;

            case DataFormat_e_RAW10TO6_DPCM:

                u32_CsiRawFormat = 0x0A06;
                break;

        }


        //inner loop for FPS
        for (fps_x100=100; fps_x100 <= bmsmodeinfo.maxfps_x100; fps_x100+=SMS_FPS_STEP_SIZE_X_100)
        {
            test_id++;
            snprintf(mess, sizeof(mess), "Test %ld : Next iteration for SMS sanity test \n", test_id);
            MMTE_TEST_NEXT(mess);

            LOS_Log("\n\n");
            sprintf(
                comment,
                "Requesting SMS Info for Mode %d: woi_sizeX=%d woi_sizeY=%d out_sizeX=%d out_sizeY=%d fps_x100=%ld raw_format=0x0%x \n",
                mode_id, bmsmodeinfo.woi_sizeX, bmsmodeinfo.woi_sizeY, bmsmodeinfo.out_sizeX, bmsmodeinfo.out_sizeY, fps_x100, u32_CsiRawFormat);
            MMTE_TEST_COMMENT(comment);

            Set_SMS_Params(bmsmodeinfo.woi_sizeX,
                           bmsmodeinfo.woi_sizeY,
                           bmsmodeinfo.out_sizeX,
                           bmsmodeinfo.out_sizeY,
                           bmsmodeinfo.maxfps_x100,
                           u32_CsiRawFormat);

            smsInterruptCount = Toggle_SMS_Coin();

            result = Check_SMSParams_Absorbed(smsInterruptCount);

            if (SMS_SUCCESS == result)
            {
                MMTE_RESULT_PASSED();
            }
            else
            {
                MMTE_RESULT_FAILED();
            }

            //This MODE_FETCH_LOGIC_ is to always hit the Max Fps use case
            if ((bmsmodeinfo.maxfps_x100 != fps_x100)
             && (bmsmodeinfo.maxfps_x100 - fps_x100 < SMS_FPS_STEP_SIZE_X_100))
            {
                fps_x100 = bmsmodeinfo.maxfps_x100 - SMS_FPS_STEP_SIZE_X_100;
                LOS_Log("Testing Max FPS use case now for mode %d:...", mode_id);
            }

            LOS_Sleep(500);
        }
      }
#endif

#ifdef MODE_FETCH_LOGIC_3
      //MODE_FETCH_LOGIC_ 3: pick sensor modes (including raw data format) from Sensor_Output_Mode_ts global buffer
      //get total no of modes
      total_modes = ITE_readPE(ReadLLAConfig_Status_u16_number_of_modes_Byte0);

      //outer loop for modes
      for (mode_id=0; mode_id < total_modes; mode_id++)
      {
        if (mode_id > 0)
        {
            ptr_bmsmodeinfo++;
        }

        //inner loop for FPS
        for (fps_x100=100; fps_x100 <= ptr_bmsmodeinfo->u32_max_frame_rate_x100; fps_x100+=SMS_FPS_STEP_SIZE_X_100)
        {
            test_id++;
            snprintf(mess, sizeof(mess), "Test %ld : Next iteration for SMS sanity test \n", test_id);
            MMTE_TEST_NEXT(mess);

            LOS_Log("\n\n");
            sprintf(
                comment,
                "Requesting SMS Info for Mode %d: woi_sizeX=%d woi_sizeY=%d out_sizeX=%d out_sizeY=%d fps_x100=%ld raw_format=0x0%x \n",
                mode_id, ptr_bmsmodeinfo->u32_woi_res_width, ptr_bmsmodeinfo->u32_woi_res_height, ptr_bmsmodeinfo->u32_output_res_width, ptr_bmsmodeinfo->u32_output_res_height, fps_x100, ptr_bmsmodeinfo->u32_data_format);
            MMTE_TEST_COMMENT(comment);

            Set_SMS_Params(ptr_bmsmodeinfo->u32_woi_res_width,
                           ptr_bmsmodeinfo->u32_woi_res_height,
                           ptr_bmsmodeinfo->u32_output_res_width,
                           ptr_bmsmodeinfo->u32_output_res_height,
                           fps_x100,
                           ptr_bmsmodeinfo->u32_data_format);

            smsInterruptCount = Toggle_SMS_Coin();

            result = Check_SMSParams_Absorbed(smsInterruptCount);

            if (SMS_SUCCESS == result)
            {
                MMTE_RESULT_PASSED();
            }
            else
            {
                MMTE_RESULT_FAILED();
            }

            //This logic is to always hit the Max Fps use case
            if ((ptr_bmsmodeinfo->u32_max_frame_rate_x100 != fps_x100)
             && (ptr_bmsmodeinfo->u32_max_frame_rate_x100 - fps_x100 < SMS_FPS_STEP_SIZE_X_100))
            {
                fps_x100 = ptr_bmsmodeinfo->u32_max_frame_rate_x100 - SMS_FPS_STEP_SIZE_X_100;
                LOS_Log("Testing Max FPS use case now for mode %d:...", mode_id);
            }
        }
      }
#endif
  }
  else
  {
      CLI_disp_msg("SMSSanityTest: Sets SMS params for all modes with max framerates, one by one. \n");
      CLI_disp_msg("SMSSanityTest[Syntax]: SMSSanityTest\n\n");

      CLI_disp_error("Not correct command arguments for %x\n",(unsigned int)u32_CsiRawFormat);
      MMTE_TEST_FAILED();
      return (CMD_ERR_ARGS);
  }

  MMTE_TEST_END();

#ifdef SMS_DEBUG
  LOS_Log("\n debug_max_exp_by_exp_step_dev = %ld", debug_max_exp_by_exp_step_dev);
#endif

  return CMD_COMPLETE;
}


/* -----------------------------------------------------------------------
FUNCTION : C_ite_dbg_SMSTest_cmd
PURPOSE  :
------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_SMSTest_cmd(int a_nb_args, char ** ap_args)
{
  char    mess[256], fname[256], comment[256], pathname[256];
  enum e_grabPipeID IspOutSelect = GRBPID_PIPE_CAM; //default, unused value
  t_uint16    u32_WOI_x, u32_WOI_y, u32_OUT_x, u32_OUT_y, u32_CsiRawFormat = 0x0A08, smsInterruptCount = 0;
  t_uint32    u32_FrameRate_x100;
  SMS_RESULT_te       result;

  sprintf(fname, "SMSTest_");
  sprintf(mess, "%s_%s_%s_%s_%s_%s_%s_%s", ap_args[1],ap_args[2],ap_args[3],ap_args[4],ap_args[5],ap_args[6],ap_args[7],ap_args[8]);
  strcat(fname, mess);

  memset(g_out_path,0,KlogDirectoryLentgh*sizeof(char));
  sprintf(pathname,"/ite_nmf/test_results_nreg/modecheck");

  MMTE_TEST_START(fname, pathname, "SMSTest");

  sprintf(mess, "SMSTest");

  MMTE_TEST_NEXT(mess);

  if (a_nb_args == 1)
  {
        CLI_disp_msg("SMSTest: Sets SMS params, toggles SMS coin and waits SMS Done event. \n");
        CLI_disp_msg("SMSTest[Syntax]: SMSTest <datapath> <wait|nowait> WOI_x WOI_y OUT_x OUT_y FrameRate_x100 CsiRawFormat \n\n");
        CLI_disp_msg("SMSTest[Example]: SMSTest LR wait 3280 2464 1640 1232 2400 0x0A0A \n\n");
  }
  else if (a_nb_args == 9)
  {
        if (0 == strcmp(ap_args[2], "wait"))
        {
            if (0 == strcmp(ap_args[1], "HR"))
            {
                IspOutSelect = GRBPID_PIPE_HR;
                ITE_HR_Prepare(&usecase,INFINITY);
                ITE_HR_Start(INFINITY);
            }
            else if (0 == strcmp(ap_args[1], "LR"))
            {
                IspOutSelect = GRBPID_PIPE_LR;
                ITE_LR_Prepare(&usecase,INFINITY);
                ITE_LR_Start(INFINITY);
            }
            else if (0 == strcmp(ap_args[1], "BMS"))
            {
                IspOutSelect = GRBPID_PIPE_RAW_OUT;
                ITE_BMS_Prepare(&usecase, 1);
                ITE_BMS_Start(1);
            }
        }

        u32_WOI_x = ITE_ConvToInt16(ap_args[3]);
        u32_WOI_y = ITE_ConvToInt16(ap_args[4]);
        u32_OUT_x = ITE_ConvToInt16(ap_args[5]);
        u32_OUT_y = ITE_ConvToInt16(ap_args[6]);
        u32_FrameRate_x100 = ITE_ConvToInt32(ap_args[7]);

        if (0 == strcmp(ap_args[8], "0x0808"))
        {
            u32_CsiRawFormat = 0x0808;
        }
        else if ((0 == strcmp(ap_args[8], "0x0a0a")) || (0 == strcmp(ap_args[8], "0x0A0A")))
        {
            u32_CsiRawFormat = 0x0A0A;
        }
        else if ((0 == strcmp(ap_args[8], "0x0c0c")) || (0 == strcmp(ap_args[8], "0x0A0A")))
        {
            u32_CsiRawFormat = 0x0C0C;
        }
        else if ((0 == strcmp(ap_args[8], "0x0a08")) || (0 == strcmp(ap_args[8], "0x0A0A")))
        {
            u32_CsiRawFormat = 0x0A08;
        }

        LOS_Log("\n\n");
        sprintf(
                comment,
                "Requesting Specific SMS Info: woi_sizeX=%d woi_sizeY=%d out_sizeX=%d out_sizeY=%d fps_x100=%ld raw_format=0x0%x \n",
                u32_WOI_x, u32_WOI_y, u32_OUT_x, u32_OUT_y, u32_FrameRate_x100, u32_CsiRawFormat);
        MMTE_TEST_COMMENT(comment);

        Set_SMS_Params(u32_WOI_x, u32_WOI_y, u32_OUT_x, u32_OUT_y, u32_FrameRate_x100, u32_CsiRawFormat);

        smsInterruptCount = Toggle_SMS_Coin();

        if (0 == strcmp(ap_args[2], "nowait"))
        {
            if (0 == strcmp(ap_args[1], "HR"))
            {
                IspOutSelect = GRBPID_PIPE_HR;
                ITE_HR_Prepare(&usecase,INFINITY);
                ITE_HR_Start(INFINITY);
            }
            else if (0 == strcmp(ap_args[1], "LR"))
            {
                IspOutSelect = GRBPID_PIPE_LR;
                ITE_LR_Prepare(&usecase,INFINITY);
                ITE_LR_Start(INFINITY);
            }
            else if (0 == strcmp(ap_args[1], "BMS"))
            {
                IspOutSelect = GRBPID_PIPE_RAW_OUT;
                ITE_BMS_Prepare(&usecase, 1);
                ITE_BMS_Start(1);
            }

        }

        result = Check_SMSParams_Absorbed(smsInterruptCount);

        if (SMS_SUCCESS == result)
        {
            MMTE_TEST_PASSED();
        }
        else
        {   MMTE_TEST_COMMENT("TEST FAILED\n");
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
      CLI_disp_error("Not correct command arguments\n");
      MMTE_TEST_FAILED();
      memset(g_out_path,0,KlogDirectoryLentgh*sizeof(char));
      return (CMD_ERR_ARGS);
  }

#ifdef SMS_DEBUG
  LOS_Log("\n debug_max_exp_by_exp_step_dev = %ld", debug_max_exp_by_exp_step_dev);
#endif

  memset(g_out_path,0,KlogDirectoryLentgh*sizeof(char));
  return CMD_COMPLETE;
}


/*
 * ********************
 * Stats Internal Functions
 * ********************
 */
static void Set_SMS_Params(t_uint32 u32_WOI_x,
                           t_uint32 u32_WOI_y,
                           t_uint32 u32_OUT_x,
                           t_uint32 u32_OUT_y,
                           t_uint32 u32_FrameRate_x100,
                           t_uint32 u32_CsiRawFormat)
{
    LOS_Log("\nConfiguring SMS  now..\n");

    ITE_writePE(SMS_Control_u32_FrameRate_x100_Byte0, u32_FrameRate_x100);
    ITE_writePE(SMS_Control_u16_WOI_X_size_Byte0, u32_WOI_x);
    ITE_writePE(SMS_Control_u16_WOI_Y_size_Byte0, u32_WOI_y);
    ITE_writePE(SMS_Control_u16_X_size_Byte0, u32_OUT_x);
    ITE_writePE(SMS_Control_u16_Y_size_Byte0, u32_OUT_y);
    ITE_writePE(SMS_Control_u16_CsiRawFormat_Byte0, u32_CsiRawFormat);

    return;
}


static t_uint16 Toggle_SMS_Coin()
{
  Coin_te control_coin, status_coin;
  t_uint16 smsInterruptCount = 0;

  LOS_Log("\nToggling SMS Coin.\n");

  control_coin = (Coin_te)ITE_readPE(SMS_Control_e_Coin_Ctrl_Byte0);
  status_coin  = (Coin_te)ITE_readPE(SMS_Status_e_Coin_Status_Byte0);

  if (control_coin != status_coin)
  {
    LOS_Log("Coins not in proper state to execute the command.\n");
    LOS_Log("Status and Control Coins should be equal before executing the command.\n");
    return 0xFFFF; //error
  }

  ITE_RefreshEventCount(Event0_Count_u16_EVENT0_29_SMS_DONE_Byte0); 

  //toggle coin
  ITE_writePE(SMS_Control_e_Coin_Ctrl_Byte0, !control_coin);

  return smsInterruptCount;
}


static SMS_RESULT_te Check_SMSParams_Absorbed(t_uint16 smsInterruptCount)
{
  char comment[256];
  Coin_te control_coin, status_coin;
  SMS_RESULT_te       result = SMS_SUCCESS;
  volatile t_uint32 LineLength_pck, FrameLength_lines, Min_Exposure_us, Max_Exposure_us, ExposureStep_us, Data_Readout_us;
  float    f_FrameTime_us ,f_line_deviation;

    /* Check the status coin */
    control_coin = (Coin_te)ITE_readPE(SMS_Control_e_Coin_Ctrl_Byte0);
    status_coin  = (Coin_te)ITE_readPE(SMS_Status_e_Coin_Status_Byte0);

  if (status_coin != control_coin)
    {
     result |= SMS_FAILURE;
    }

  LOS_Log("\nChecking SMS Params absorbed. \n");
  result = Wait_SMS_Complete(smsInterruptCount);
  if (SMS_SUCCESS == result)
  {

      LineLength_pck = ITE_readPE(SMS_Status_u32_LineLength_pck_Byte0);
      FrameLength_lines =ITE_readPE(SMS_Status_u32_FrameLength_lines_Byte0);
      Min_Exposure_us = ITE_readPE(SMS_Status_u32_Min_ExposureTime_us_Byte0);
      Max_Exposure_us = ITE_readPE(SMS_Status_u32_Max_ExposureTime_us_Byte0);
      ExposureStep_us = ITE_readPE(SMS_Status_u32_ExposureQuantizationStep_us_Byte0);
      Data_Readout_us = ITE_readPE(SMS_Status_u32_ActiveData_ReadoutTime_us_Byte0);

      MMTE_TEST_COMMENT("SMS Absorbed. Following are the reported paramteres: \n");
      sprintf(comment, "LineLength_pck(in pck) = %ld \n", LineLength_pck);
      MMTE_TEST_COMMENT(comment);
      sprintf(comment, "FrameLength_lines(in lines) = %ld \n", FrameLength_lines);
      MMTE_TEST_COMMENT(comment);
      sprintf(comment, "Min_Exposure_usTime(in us) = %ld \n", Min_Exposure_us);
      MMTE_TEST_COMMENT(comment);
      sprintf(comment, "Max_Exposure_usTime(in us) = %ld \n", Max_Exposure_us);
      MMTE_TEST_COMMENT(comment);
      sprintf(comment, "ExposureQuantizationStep(in us) = %ld \n", ExposureStep_us);
      MMTE_TEST_COMMENT(comment);
      sprintf(comment, "ActiveData_Readout_usTime(in us) = %ld \n", Data_Readout_us);
      MMTE_TEST_COMMENT(comment);

      LOS_Log("\nChecking Params Received.\n");

      //[CHECK 1]Exposure time must be "little" less than 1/framerate
      f_FrameTime_us = (1.0 / ( ITE_readPE(SMS_Control_u32_FrameRate_x100_Byte0) / 100.0 )) * 1000000.0;
      sprintf(comment, "f_FrameTime_us theoretical calulated(in us) = %f \n", f_FrameTime_us);
      MMTE_TEST_COMMENT(comment);

      f_line_deviation = (f_FrameTime_us - (float)Max_Exposure_us)/(float)ExposureStep_us ;

      if ( (Max_Exposure_us > f_FrameTime_us) || (Max_Exposure_us < (f_FrameTime_us * (1 - SMS_EXPOSURE_PERCENTAGE_DEVIATION_EXPECTED))) )
      {
          MMTE_TEST_COMMENT("Possible Error: Exposure time is not little less than Frame Time(i.e., 1/framerate) \n");
          sprintf(comment, "Margin Deviation chosen (in %%) = %f\n", (float)SMS_EXPOSURE_PERCENTAGE_DEVIATION_EXPECTED * 100);
          MMTE_TEST_COMMENT(comment);
          sprintf(comment, "Computed Frame Time (in us) = %f\n", f_FrameTime_us);
          MMTE_TEST_COMMENT(comment);
          result |= SMS_FAILURE;
      }

      //[CHECK 2]
      //max exposure/Exposure step must be integer.
      //Exposure time is in integer and hence we loose some precision so we check line deviation
      //line deviation between (f_FrameTime_us/exposuretime step ) and (Max_Exposure_us /exposure time step) should be less than 2.05 lines
     if ( f_line_deviation >= K_Tolerancelines)
      {
          sprintf(comment,"Error: f_line_deviation is greater than K_TolerancePercentage : %f  \n",f_line_deviation);
          MMTE_TEST_COMMENT(comment);
          result |= SMS_FAILURE;

#ifdef SMS_DEBUG
          if ((Max_Exposure_us % ExposureStep_us) > debug_max_exp_by_exp_step_dev)
               debug_max_exp_by_exp_step_dev = Max_Exposure_us % ExposureStep_us;
#endif
      }

      //[CHECK 3]min exposure/Exposure step must be integer.
      if ((Min_Exposure_us % ExposureStep_us) != 0)
      {
          MMTE_TEST_COMMENT("Possible Error: Min_Exposure_us/exposure step is not an integer \n");
          result |= SMS_FAILURE;

#ifdef SMS_DEBUG
          if ((Min_Exposure_us % ExposureStep_us) > debug_min_exp_by_exp_step_dev)
               debug_min_exp_by_exp_step_dev = Min_Exposure_us % ExposureStep_us;
#endif
      }

  }

  return result;
}


static SMS_RESULT_te Wait_SMS_Complete(t_uint16 smsInterruptCount)
{
    t_sint16 err;

    union u_ITE_Event   event;
    UNUSED(smsInterruptCount);

    LOS_Log("\nWaiting for SMS Notification \n");
#if 0
    union u_ITE_Event   event;

    //Method 1: to be used once SMS notify is implemented in NMF
    do
    {
        event = ITE_WaitEvent(ITE_EVT_SMS_DONE_EVENTS);
        ITE_traceISPCTLevent(event);
        if (event.type == ITE_EVT_ISPCTL_SMS_ERROR)
        {
            break;
        }
    } while (!((event.type == ITE_EVT_ISPCTL_SMS_INFO) && (event.ispctlInfo.info_id == ISP_SMS_READY)));

    LOS_Log("SMS Notification Received!\n");

#endif
    //METHOD 2: Track SMS Interrupt count: Workround till SMS notify is implemented in NMF
#if 0
    t_uint16 currentInterruptCount;

    currentInterruptCount = ITE_readPE(Event0_Count_u16_EVENT0_29_SMS_DONE_Byte0);

    //check interrupt count value
    if (((smsInterruptCount == 0xFFFF) && (currentInterruptCount == 0)) ||
       (currentInterruptCount - smsInterruptCount == 1))
    {
       LOS_Log("SMS Notification Received!\n");
       return SMS_SUCCESS;
    }
    else
    {
       MMTE_TEST_COMMENT("SMS Notification NOT Received!\n");
       return SMS_FAILURE;
    }


#endif
     //ITE_NMF_WaitEvent(ISP_SMS_NOTIFICATION);

  do
    {
        event = ITE_WaitEvent(ISP_SMS_NOTIFICATION_EVENTS);
        ITE_traceISPCTLevent(event);
        if (event.type == ITE_EVT_ISPCTL_ERROR)
        {
            LOS_Log("\nITE_EVT_ISPCTL_ERROR \n");
            break;
            return 1;
        }
    } while (!(event.ispctlInfo.info_id == ISP_SMS_NOTIFICATION));

     err = ITE_GetEventCount(Event0_Count_u16_EVENT0_29_SMS_DONE_Byte0);
     
     if (err == 0)
      {
        return SMS_SUCCESS;
      }
     else
      {
        return SMS_FAILURE;  
      }
 
 
}

