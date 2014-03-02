/*
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ite_testenv_utils.h"
#include "cli.h"
#include "ite_isp_ip_tests.h"
#include "ite_host2sensor.h"
#include "ite_nmf_Rx_Testpattern.h"

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
#include "ite_colormatrix.h"
#include "ite_grab.h"
#include "ite_vpip.h"
#include "ite_framerate.h"
#include "ite_sia_buffer.h"

//#include "ite_irq_handler.h"
//#include "ite_ske.h"
#include "ite_buffer_management.h"
#include "grab_types.idt.h"

#include "ite_nmf_standard_functions.h"
#include "ite_nmf_framerate.h"
#include "ite_nmf_framerate_tests.h"
#include "ite_debug.h"

#include <test/api/test.h>
#include <cm/inc/cm_macros.h>

#include <los/api/los_api.h>
#include "ite_main.h"

//#include <ilos/api/ilos_api.h>
#include "sia.h"
#include "pictor_full.h"
#include "sia_register_fct.h"
#include "hi_register_acces.h"
#include "ite_nreg_datapath_tests.h"

volatile t_uint32           Read_Val = 0xFFFF;
extern ts_sia_usecase       usecase;

// Sensor Or Rx Input Selection. Default Value is Sensor0
extern InputImageSource_te  g_InputImageSource;

void                        ITE_StreamLR (tps_sia_usecase p_sia_usecase);
void                        ITE_StreamHR (tps_sia_usecase p_sia_usecase);

void                        ITE_testSFX (enum e_grabFormat grbformat);
void                        ITE_testSFX_StreamLR (tps_sia_usecase p_sia_usecase);

//void ITE_setAdsoc_PK_Ctrl_0_1_PE(t_uint32 e_Flag_Adsoc_PK_Ctrl_0, t_uint32 e_Flag_Adsoc_PK_Ctrl_1);
//void ITE_setAdsoc_PK_Ctrl_0_1_u8_Adsoc_PK_Emboss_PE(t_uint32 e_Flag_Adsoc_PK_Ctrl_0_u8_Adsoc_PK_Emboss, t_uint32 e_Flag_Adsoc_PK_Ctrl_1_u8_Adsoc_PK_Emboss);
//void ITE_testAdsoc_Ctrl_PE(t_uint32 e_Flag_Adsoc_PK_Ctrl_0, t_uint32 e_Flag_Adsoc_PK_Ctrl_1);
//void ITE_testAdsoc_Emboss_PE(t_uint32 e_Flag_Adsoc_PK_Ctrl_0_u8_Adsoc_PK_Emboss, t_uint32 e_Flag_Adsoc_PK_Ctrl_1_u8_Adsoc_PK_Emboss);
void                        ITE_set_SolarisControl_0_1_PE (
                            t_uint32    e_Flag_SolarisControl_0,
                            t_uint32    e_Flag_SolarisControl_1);
void                        ITE_set_NegativeControl_0_1_PE (
                            t_uint32    e_Flag_NegativeControl_0,
                            t_uint32    e_Flag_NegativeControl_1);
void                        ITE_testSolaris_Ctrl_PE (
                            t_uint32    e_Flag_SolarisControl_0,
                            t_uint32    e_Flag_SolarisControl_1);
void                        ITE_testNegative_Ctrl_PE (
                            t_uint32    e_Flag_NegativeControl_0,
                            t_uint32    e_Flag_NegativeControl_1);
void                        ITE_NMF_Toggle_System_Coin (void);

//Pixel Order
void                        ITE_testPixelOrder (enum e_grabFormat grbformat);
void                        ITE_testPixelOrder_StreamLR (tps_sia_usecase p_sia_usecase);

///Babylon
void                        ITE_testBabylonManual (enum e_grabFormat grbformat);
void                        ITE_testBabylon_StreamLR (tps_sia_usecase p_sia_usecase);
void                        ITE_setBabylonDefault_Ctrl_PE (
                            t_uint32    e_Flag_BabylonEnable_default,
                            t_uint32    e_Flag_u8_ZipperKill_default,
                            t_uint32    e_Flag_u8_Flat_Threshold_default);
void                        ITE_testBabylonDefault_Ctrl_PE (
                            t_uint32    e_Flag_BabylonEnable_default,
                            t_uint32    e_Flag_u8_ZipperKill_default,
                            t_uint32    e_Flag_u8_Flat_Threshold_default);

///Scorpio
void                        ITE_testScorpio (enum e_grabFormat grbformat);
void                        ITE_testScorpio_StreamLR (tps_sia_usecase p_sia_usecase);
void                        ITE_setScorpioDefault_Ctrl_PE (
                            t_uint32    e_Flag_ScorpioEnable_default,
                            t_uint32    e_ScorpioMode_default,
                            t_uint32    e_Flag_u8_CoringLevel_Ctrl_default,
                            t_uint32    e_Flag_u8_CoringLevel_Status);
void                        ITE_testScorpioDefault_Ctrl_PE (
                            t_uint32    e_Flag_ScorpioEnable_default,
                            t_uint32    e_ScorpioMode_default,
                            t_uint32    e_Flag_u8_CoringLevel_Ctrl_default,
                            t_uint32    e_Flag_u8_CoringLevel_Status);

///RSO Manual
void                        ITE_testRSOManual (enum e_grabFormat grbformat);
void                        ITE_testRSOManual_StreamLR (tps_sia_usecase p_sia_usecase);
void                        ITE_setRSOManualDefault_Ctrl_PE (t_uint32 e_Flag_EnableRSO, t_uint32 e_RSO_Mode_Control);
void                        ITE_setRSOManualDefault_Color_Ctrl_PE (
                            t_uint32    u32_XCoefGr_default,
                            t_uint32    u32_YCoefGr_default,
                            t_uint32    u32_XCoefR_default,
                            t_uint32    u32_YCoefR_default,
                            t_uint32    u32_XCoefGb_default,
                            t_uint32    u32_YCoefGb_default,
                            t_uint32    u32_XCoefB_default,
                            t_uint32    u32_YCoefB_default);
void                        ITE_setRSOManualDefault_DcTerm_Ctrl_PE (
                            t_uint32    u16_DcTermGr_default,
                            t_uint32    u16_DcTermR_default,
                            t_uint32    u16_DcTermB_default,
                            t_uint32    u16_DcTermGb_default,
                            t_uint32    u16_XSlantOrigin_default,
                            t_uint32    u16_YSlantOrigin_default);

void                        ITE_testRSOManualDefault_Ctrl_PE (t_uint32 e_Flag_EnableRSO);
void                        ITE_testRSOManualDefault_Color_Ctrl_PE (
                            t_uint32    u32_XCoefGr_default,
                            t_uint32    u32_YCoefGr_default,
                            t_uint32    u32_XCoefR_default,
                            t_uint32    u32_YCoefR_default,
                            t_uint32    u32_XCoefGb_default,
                            t_uint32    u32_YCoefGb_default,
                            t_uint32    u32_XCoefB_default,
                            t_uint32    u32_YCoefB_default);
void                        ITE_testRSOManualDefault_DcTerm_Ctrl_PE (
                            t_uint32    u16_DcTermGr_default,
                            t_uint32    u16_DcTermR_default,
                            t_uint32    u16_DcTermB_default,
                            t_uint32    u16_DcTermGb_default,
                            t_uint32    u16_XSlantOrigin_default,
                            t_uint32    u16_YSlantOrigin_default);

//Binning Repair
void                        ITE_testBinningRepair (enum e_grabFormat grbformat);
void                        ITE_testBinningRepairManual_StreamLR (tps_sia_usecase p_sia_usecase);
void                        ITE_setBinningRepairManualDefault_Ctrl_PE (
                            t_uint32    e_Flag_BinningRepairEnable_Default,
                            t_uint32    e_Flag_H_Jog_Enable_Default,
                            t_uint32    e_Flag_V_Jog_Enable_Default,
                            t_uint32    e_BinningRepairMode_Default,
                            t_uint32    u8_Coeff_00_Default,
                            t_uint32    u8_Coeff_01_Default,
                            t_uint32    u8_Coeff_10_Default,
                            t_uint32    u8_Coeff_11_Default,
                            t_uint32    u8_Coeff_shift_Default,
                            t_uint32    u8_BinningRepair_factor_Default);
void                        ITE_testBinningRepairManualDefault_Ctrl_PE (
                            t_uint32    e_Flag_BinningRepairEnable_Default,
                            t_uint32    e_Flag_H_Jog_Enable_Default,
                            t_uint32    e_Flag_V_Jog_Enable_Default,
                            t_uint32    u8_Coeff_00_Default,
                            t_uint32    u8_Coeff_01_Default,
                            t_uint32    u8_Coeff_10_Default,
                            t_uint32    u8_Coeff_11_Default,
                            t_uint32    u8_Coeff_shift_Default,
                            t_uint32    u8_BinningRepair_factor_Default);

void                        ITE_setBinningRepairManualDefault22_Color_Ctrl_PE (
                            t_uint32    u8_Coeff22_00,
                            t_uint32    u8_Coeff22_01,
                            t_uint32    u8_Coeff22_10,
                            t_uint32    u8_Coeff22_11,
                            t_uint32    u8_Coeff22_shift);
void                        ITE_testBinningRepairManualDefault22_Color_Ctrl_PE (
                            t_uint32    u8_Coeff22_00,
                            t_uint32    u8_Coeff22_01,
                            t_uint32    u8_Coeff22_10,
                            t_uint32    u8_Coeff22_11,
                            t_uint32    u8_Coeff22_shift);
void                        ITE_setBinningRepairManualDefault44_Color_Ctrl_PE (
                            t_uint32    u8_Coeff44_00,
                            t_uint32    u8_Coeff44_01,
                            t_uint32    u8_Coeff44_10,
                            t_uint32    u8_Coeff44_11,
                            t_uint32    u8_Coeff44_shift);
void                        ITE_testBinningRepairManualDefault44_Color_Ctrl_PE (
                            t_uint32    u8_Coeff44_00,
                            t_uint32    u8_Coeff44_01,
                            t_uint32    u8_Coeff44_10,
                            t_uint32    u8_Coeff44_11,
                            t_uint32    u8_Coeff44_shift);

//Colour Matrix
void                        ITE_testColourMatrix (enum e_grabFormat grbformat);
void                        ITE_ColourMatrix_Run (void);
t_bool                      ITE_CM_ReadHW_Compare (t_bool b_PipeNumber);
void                        ITE_ColourMatrix (void);
void                        ITE_SetupColourMatrix (t_uint8 u8_Pipe);
t_bool                      ITE_IsPipe0Enabled (void);
t_bool                      ITE_IsPipe1Enabled (void);

// Luma Test
void                        ITE_ProgramControlCount (void);
void                        ITE_WaitOnControlCount (void);
void                        ITE_LumaOffsetTest (enum e_grabFormat grbformat);

//Adsoc
//void ITE_testAdsoc(enum e_grabFormat grbformat);
//void ITE_testAdsocManual_StreamLR(tps_sia_usecase p_sia_usecase);
//void ITE_setAdsocDefault_Ctrl_PE(t_uint32 e_Flag_Adsoc_PK_Enable_Ctrl,t_uint32 e_AdsocMode_Ctrl,t_uint32 e_Flag_Adsoc_PK_AdaptiveSharpening_Enable_Ctrl,t_uint32 u8_Adsoc_PK_Emboss_Select_Ctrl,
//t_uint32 u8_Adsoc_PK_Flipper_Ctrl,t_uint32 u8_Adsoc_PK_GrayBack_Ctrl);
//void ITE_setAdsocDefault_Params_PE(t_uint32 u8_Adsoc_PK_Coring_Level, t_uint32 u8_Adsoc_PK_OverShoot_Gain_Bright,t_uint32 u8_Adsoc_PK_OverShoot_Gain_Dark,t_uint32 u8_Adsoc_PK_Gain);
//void ITE_testAdsocDefault_Ctrl_PE(t_uint32 e_Flag_Adsoc_PK_Enable_Ctrl,t_uint32 e_AdsocMode_Ctrl,t_uint32 e_Flag_Adsoc_PK_AdaptiveSharpening_Enable_Ctrl,t_uint32 u8_Adsoc_PK_Emboss_Select_Ctrl,
//t_uint32 u8_Adsoc_PK_Flipper_Ctrl,t_uint32 u8_Adsoc_PK_GrayBack_Ctrl);
//void ITE_testAdsocDefault_Params_PE (t_uint32 u8_Adsoc_PK_Coring_Level, t_uint32 u8_Adsoc_PK_OverShoot_Gain_Bright,t_uint32 u8_Adsoc_PK_OverShoot_Gain_Dark,t_uint32 u8_Adsoc_PK_Gain);
//Mirror Flip
void                        ITE_testMirrorFlip (enum e_grabFormat grbformat);
void                        ITE_testMirrorFlip_StreamLR (tps_sia_usecase p_sia_usecase);
void                        ITE_setMirrorFlipPE (t_uint32 e_Flag_mirror, t_uint32 e_Flag_flip);
void                        ITE_testMirrorFlipPE (t_uint32 e_Flag_mirror, t_uint32 e_Flag_flip);

//Self Test
void                        ITE_GiveStringNameFromModeEnum (e_sensormode SensorMode, char *name);
void                        ITE_NMF_SelfTest_HR (enum e_grabFormat grbformat);
void                        ITE_NMF_SelfTest_LR (enum e_grabFormat grbformat);
void                        ITE_NMF_SelfTest_BMS (void);
void                        ITE_NMF_SelfTest_Result (e_sensormode SensorMode, char *SensorModeName, char *Pipe);

extern void                 ITE_testStreamBMS (tps_sia_usecase p_sia_usecase, e_resolution sizebms);
t_uint32                    testCount = 0;

typedef struct
{
    float       CE_CMFloatRedInRed;
    float       CE_CMFloatGreenInRed;
    float       CE_CMFloatBlueInRed;
    float       CE_CMFloatRedInGreen;
    float       CE_CMFloatGreenInGreen;
    float       CE_CMFloatBlueInGreen;
    float       CE_CMFloatRedInBlue;
    float       CE_CMFloatGreenInBlue;
    float       CE_CMFloatBlueInBlue;
    t_uint16    u16_OffsetRed;
    t_uint16    u16_OffsetGreen;
    t_uint16    u16_OffsetBlue;
} CE_ColourMatrix_pageElementvalue;

//CE_ColourMatrix_pageElementvalue CE_ColourMatrix_PE_0_value, CE_ColourMatrix_PE_1_value;
CE_ColourMatrix_pageElementvalue    CE_ColourMatrix_PE_0_value =
{
    200,    //CE_ColourMatrix_PE_0_value.CE_CMFloatRedInRed
    300,    //CE_ColourMatrix_PE_0_value.CE_CMFloatGreenInRed
    400,    //CE_ColourMatrix_PE_0_value.CE_CMFloatBlueInRed
    500,    //CE_ColourMatrix_PE_0_value.CE_CMFloatRedInGreen
    600,    //CE_ColourMatrix_PE_0_value.CE_CMFloatGreenInGreen
    350,    //CE_ColourMatrix_PE_0_value.CE_CMFloatBlueInGreen
    400,    //CE_ColourMatrix_PE_0_value.CE_CMFloatRedInBlue
    450,    //CE_ColourMatrix_PE_0_value.CE_CMFloatGreenInBlue
    500,    //CE_ColourMatrix_PE_0_value.CE_CMFloatBlueInBlue
    50,     //CE_ColourMatrix_PE_0_value.u16_OffsetRed
    100,    //CE_ColourMatrix_PE_0_value.u16_OffsetGreen
    150     //CE_ColourMatrix_PE_0_value.u16_OffsetBlue
};

CE_ColourMatrix_pageElementvalue    CE_ColourMatrix_PE_1_value =
{
    100,    //CE_ColourMatrix_PE_1_value.CE_CMFloatRedInRed
    200,    //CE_ColourMatrix_PE_1_value.CE_CMFloatGreenInRed
    300,    //CE_ColourMatrix_PE_1_value.CE_CMFloatBlueInRed
    300,    //CE_ColourMatrix_PE_1_value.CE_CMFloatRedInGreen
    400,    //CE_ColourMatrix_PE_1_value.CE_CMFloatGreenInGreen
    450,    //CE_ColourMatrix_PE_1_value.CE_CMFloatBlueInGreen
    500,    //CE_ColourMatrix_PE_1_value.CE_CMFloatRedInBlue
    250,    //CE_ColourMatrix_PE_1_value.CE_CMFloatGreenInBlue
    300,    //CE_ColourMatrix_PE_1_value.CE_CMFloatBlueInBlue
    40,     //CE_ColourMatrix_PE_1_value.u16_OffsetRed
    50,     //CE_ColourMatrix_PE_1_value.u16_OffsetGreen
    50      //CE_ColourMatrix_PE_1_value.u16_OffsetBlue
};

t_uint16                            u16_0_OffsetRed;
t_uint16                            u16_0_OffsetGreen;
t_uint16                            u16_0_OffsetBlue;

//ColourMatrixMode_te  ColourMatrixMode;

/* -----------------------------------------------------------------------
FUNCTION : C_ite_ip_tests_cmd
PURPOSE  : Command to execute IP tests
------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_ip_tests_cmd(
int     a_nb_args,
char    **ap_args)
{
    if (a_nb_args == 3)
    {
        ITE_ip_tests(ap_args[1], ap_args[2]);
    }
    else
    {
        CLI_disp_error("Not correct command arguments\n");
    }


    return (CMD_COMPLETE);
}


CMD_COMPLETION
C_ite_Flip_Mirror_cmd(
int     a_nb_args,
char    **ap_args)
{
 UNUSED(ap_args);
    if (a_nb_args == 1)
    {
        ITE_testMirrorFlip(GRBFMT_YUV422_RASTER_INTERLEAVED);
    }
    else
    {
        CLI_disp_error("Not correct command arguments\n");
    }


    return (CMD_COMPLETE);
}


/* -----------------------------------------------------------------------
FUNCTION : ITE_ip_tests
PURPOSE  : Test different datapath (LR,HR,BMS) , check ouput frame automaticaly and framerates
------------------------------------------------------------------------ */
void
ITE_ip_tests(
char    *ap_test_id,
char    *ap_grabvpip_options)
{
    t_uint8 u8_index;

    /* HCL Services, alloc, Log, IT, xti ....*/
    // start
    UNUSED(u8_index);

    ITE_InitUseCase(ap_test_id, ap_grabvpip_options, &usecase);

    //instantiate NMF network and boot xP70
    ITE_Start_Env(&usecase, ap_grabvpip_options, 0, 0);

    ITE_testFirmwareRevision();

    //Perform all tests
    {
        //ITE_NMF_SelfTest_BMS();
        ITE_NMF_SelfTest_HR(GRBFMT_YUV422_RASTER_INTERLEAVED);
        ITE_NMF_SelfTest_LR(GRBFMT_YUV422_RASTER_INTERLEAVED);
        ITE_testMirrorFlip(GRBFMT_YUV422_RASTER_INTERLEAVED);
        ITE_testPixelOrder(GRBFMT_YUV422_RASTER_INTERLEAVED);
        ITE_testBabylonManual(GRBFMT_YUV422_RASTER_INTERLEAVED);
        ITE_testScorpio(GRBFMT_YUV422_RASTER_INTERLEAVED);
        ITE_testRSOManual(GRBFMT_YUV422_RASTER_INTERLEAVED);
        ITE_testBinningRepair(GRBFMT_YUV422_RASTER_INTERLEAVED);
        ITE_LumaOffsetTest(GRBFMT_YUV422_RASTER_INTERLEAVED);

        //ITE_testAdsoc(GRBFMT_YUV422_RASTER_INTERLEAVED);
        ITE_testColourMatrix(GRBFMT_YUV422_RASTER_INTERLEAVED);
        ITE_testSFX(GRBFMT_YUV422_RASTER_INTERLEAVED);  //solaris and negative control fails
    }


    //ITE_Stop_Env();
}


//Seperate test case has been written for Adsoc

/* -----------------------------------------------------------------------
FUNCTION : ITE_testAdsoc
PURPOSE  : Test Adsoc manual module
------------------------------------------------------------------------ */

/*void ITE_testAdsoc(enum e_grabFormat grbformat)
{
char testComment[200];
char lrformatName[16];
char sensorName[16];
char resolutionName[16];
char pathname[200];
e_resolution sizelr = SUBQCIF;

  if (usecase.sensor==0)
  sprintf(sensorName,"Cam0");
  else
  sprintf(sensorName,"Cam1");

    usecase.LR_GrbFormat = grbformat;
    ITE_GiveStringFormat("LR", lrformatName);
    LOS_Log("\n --> Stream on LR pipe with GRBFMT_%s format\n",lrformatName);

      ITE_GiveStringNameFromEnum(sizelr, resolutionName);
      LOS_Log("\n -----> %s resolution\n",resolutionName);
      usecase.LR_XSize = ITE_GiveXSizeFromEnum(PIPE_LR, sizelr,grbformat);
      usecase.LR_YSize = ITE_GiveYSizeFromEnum(PIPE_LR, sizelr,grbformat);
      usecase.LR_resolution = ITE_GiveResolution(resolutionName);
      sprintf(testComment,"%s_test_IP_Adsoc_tests_%s_%s",sensorName,lrformatName,resolutionName);
      //perform a "cd" to destination repository for test log
      sprintf(pathname,IMAGING_PATH"/ite_nmf/test_results_nreg/IP_Adsoc_tests_%s",lrformatName);
      strcat(g_out_path,pathname);
      LOS_Log("Current path: >%s< \n",(char *)g_out_path);
      mmte_testStart(testComment," test pipe LR (buffer + fr) YUV422 interleaved format", g_out_path);

        ITE_testAdsocManual_StreamLR(&usecase);

          mmte_testEnd();
          //clean  g_out_path
          memset ( g_out_path, 0, KlogDirectoryLentgh*sizeof (char) );
          }
*/

//Seperate test case has been written for Adsoc

/* -----------------------------------------------------------------------
FUNCTION : ITE_testAdsocManual_StreamLR
PURPOSE  : Test Adsoc manual module on LR
------------------------------------------------------------------------ */

/*void ITE_testAdsocManual_StreamLR(tps_sia_usecase p_sia_usecase)
{
t_uint32 e_Flag_Adsoc_PK_Enable_Ctrl = Flag_e_TRUE,
e_AdsocMode_Ctrl = AdsocMode_e_Manual,
e_Flag_Adsoc_PK_AdaptiveSharpening_Enable_Ctrl=Flag_e_FALSE,
u8_Adsoc_PK_Emboss_Select_Ctrl=5,
u8_Adsoc_PK_Flipper_Ctrl=2,
u8_Adsoc_PK_GrayBack_Ctrl=3;

  t_uint32 u8_Adsoc_PK_Coring_Level = 12,u8_Adsoc_PK_OverShoot_Gain_Bright=2,u8_Adsoc_PK_OverShoot_Gain_Dark=3,u8_Adsoc_PK_Gain=4;



        ITE_writePE(HostFrameConstraints_e_Flag_GuaranteeStaticFlickerFrameLength_Byte0,Flag_e_FALSE);

          // prepare buffers/LR pipe and Start streaming on LR pipe
          ITE_StreamLR(p_sia_usecase);

            LOS_Log("\n1secs stream on LR pipe\n");
            ITE_HR_Stop();
            ITE_HR_Free();
            // prepare buffers/HR pipe and Start streaming on HR pipe
            ITE_StreamHR(p_sia_usecase);

              LOS_Log("\n1secs stream on HR pipe\n");

                LOS_Sleep(500);
                ITE_NMF_Toggle_System_Coin();
                // Set Adsoc default Control page elements
                ITE_setAdsocDefault_Ctrl_PE(e_Flag_Adsoc_PK_Enable_Ctrl, e_AdsocMode_Ctrl,e_Flag_Adsoc_PK_AdaptiveSharpening_Enable_Ctrl,u8_Adsoc_PK_Emboss_Select_Ctrl,u8_Adsoc_PK_Flipper_Ctrl,
                u8_Adsoc_PK_GrayBack_Ctrl);
                ITE_NMF_Toggle_System_Coin();
                // Set Adsoc Default Params page elements
                ITE_setAdsocDefault_Params_PE(u8_Adsoc_PK_Coring_Level, u8_Adsoc_PK_OverShoot_Gain_Bright,u8_Adsoc_PK_OverShoot_Gain_Dark,u8_Adsoc_PK_Gain);
                ITE_NMF_Toggle_System_Coin();
                // Test Adsoc default Control page elements
                ITE_testAdsocDefault_Ctrl_PE(e_Flag_Adsoc_PK_Enable_Ctrl, e_AdsocMode_Ctrl,e_Flag_Adsoc_PK_AdaptiveSharpening_Enable_Ctrl,u8_Adsoc_PK_Emboss_Select_Ctrl,u8_Adsoc_PK_Flipper_Ctrl,
                u8_Adsoc_PK_GrayBack_Ctrl);
                ITE_NMF_Toggle_System_Coin();
                // Test Adsoc Default Params page elements
                ITE_testAdsocDefault_Params_PE(u8_Adsoc_PK_Coring_Level, u8_Adsoc_PK_OverShoot_Gain_Bright,u8_Adsoc_PK_OverShoot_Gain_Dark,u8_Adsoc_PK_Gain);

                  ITE_LR_Stop();
                  ITE_LR_Free();

                    //ITE_HR_Stop();
                    //ITE_HR_Free();

                      }
                      */

//Seperate test case has been written for Adsoc

/* -----------------------------------------------------------------------
FUNCTION : ITE_setAdsocDefault_Ctrl_PE
PURPOSE  : Set Adsoc default Control page elements
------------------------------------------------------------------------ */

/*__inline void ITE_setAdsocDefault_Ctrl_PE(t_uint32 e_Flag_Adsoc_PK_Enable_Ctrl,t_uint32 e_AdsocMode_Ctrl,t_uint32 e_Flag_Adsoc_PK_AdaptiveSharpening_Enable_Ctrl,t_uint32 u8_Adsoc_PK_Emboss_Select_Ctrl,
t_uint32 u8_Adsoc_PK_Flipper_Ctrl,t_uint32 u8_Adsoc_PK_GrayBack_Ctrl)

  {
  ITE_writePE(Adsoc_PK_Ctrl_0_e_Flag_Adsoc_PK_Enable_Byte0,e_Flag_Adsoc_PK_Enable_Ctrl);
  ITE_writePE(Adsoc_PK_Ctrl_1_e_Flag_Adsoc_PK_Enable_Byte0,e_Flag_Adsoc_PK_Enable_Ctrl);

    // ITE_writePE(Adsoc_PK_Ctrl_0_e_AdsocMode_Byte0,e_AdsocMode_Ctrl);
    //ITE_writePE(Adsoc_PK_Ctrl_1_e_AdsocMode_Byte0,e_AdsocMode_Ctrl);

      ITE_writePE(Adsoc_PK_Ctrl_0_e_Flag_Adsoc_PK_AdaptiveSharpening_Enable_Byte0,e_Flag_Adsoc_PK_AdaptiveSharpening_Enable_Ctrl);
      ITE_writePE(Adsoc_PK_Ctrl_1_e_Flag_Adsoc_PK_AdaptiveSharpening_Enable_Byte0,e_Flag_Adsoc_PK_AdaptiveSharpening_Enable_Ctrl);

        ITE_writePE(Adsoc_PK_Ctrl_0_u8_Adsoc_PK_Emboss_Effect_Ctrl_Byte0,u8_Adsoc_PK_Emboss_Select_Ctrl);
        ITE_writePE(Adsoc_PK_Ctrl_1_u8_Adsoc_PK_Emboss_Effect_Ctrl_Byte0,u8_Adsoc_PK_Emboss_Select_Ctrl);

          ITE_writePE(Adsoc_PK_Ctrl_0_u8_Adsoc_PK_Flipper_Ctrl_Byte0,u8_Adsoc_PK_Flipper_Ctrl);
          ITE_writePE(Adsoc_PK_Ctrl_1_u8_Adsoc_PK_Flipper_Ctrl_Byte0,u8_Adsoc_PK_Flipper_Ctrl);

            ITE_writePE(Adsoc_PK_Ctrl_0_u8_Adsoc_PK_GrayBack_Ctrl_Byte0,u8_Adsoc_PK_GrayBack_Ctrl);
            ITE_writePE(Adsoc_PK_Ctrl_1_u8_Adsoc_PK_GrayBack_Ctrl_Byte0,u8_Adsoc_PK_GrayBack_Ctrl);

              LOS_Sleep(500);
              }
*/

//Seperate test case has been written for Adsoc

/* -----------------------------------------------------------------------
FUNCTION : ITE_setAdsocDefault_Params_PE
PURPOSE  : Set Adsoc default Params page elements
------------------------------------------------------------------------ */

/*__inline void ITE_setAdsocDefault_Params_PE(t_uint32 u8_Adsoc_PK_Coring_Level, t_uint32 u8_Adsoc_PK_OverShoot_Gain_Bright,t_uint32 u8_Adsoc_PK_OverShoot_Gain_Dark,t_uint32 u8_Adsoc_PK_Gain)
{
ITE_writePE(Adsoc_PK_ParamsCtrl_0_u8_Adsoc_PK_Coring_Level_Byte0,u8_Adsoc_PK_Coring_Level);
ITE_writePE(Adsoc_PK_ParamsCtrl_1_u8_Adsoc_PK_Coring_Level_Byte0,u8_Adsoc_PK_Coring_Level);

  ITE_writePE(Adsoc_PK_ParamsCtrl_0_u8_Adsoc_PK_OverShoot_Gain_Bright_Byte0,u8_Adsoc_PK_OverShoot_Gain_Bright);
  ITE_writePE(Adsoc_PK_ParamsCtrl_1_u8_Adsoc_PK_OverShoot_Gain_Bright_Byte0,u8_Adsoc_PK_OverShoot_Gain_Bright);

    ITE_writePE(Adsoc_PK_ParamsCtrl_0_u8_Adsoc_PK_OverShoot_Gain_Dark_Byte0,u8_Adsoc_PK_OverShoot_Gain_Dark);
    ITE_writePE(Adsoc_PK_ParamsCtrl_1_u8_Adsoc_PK_OverShoot_Gain_Dark_Byte0,u8_Adsoc_PK_OverShoot_Gain_Dark);

      ITE_writePE(Adsoc_PK_ParamsCtrl_0_u8_Adsoc_PK_Gain_Byte0,u8_Adsoc_PK_Gain);
      ITE_writePE(Adsoc_PK_ParamsCtrl_1_u8_Adsoc_PK_Gain_Byte0,u8_Adsoc_PK_Gain);

        LOS_Sleep(500);
}*/

//Seperate test case has been written for Adsoc

/* -----------------------------------------------------------------------
FUNCTION : ITE_testAdsocDefault_Ctrl_PE
PURPOSE  : check Adsoc Default Control status page elements and decide if tests pass or fail
----------------------------------------------------------------------------------- */

/*void ITE_testAdsocDefault_Ctrl_PE(t_uint32 e_Flag_Adsoc_PK_Enable_Ctrl,t_uint32 e_AdsocMode_Ctrl,t_uint32 e_Flag_Adsoc_PK_AdaptiveSharpening_Enable_Ctrl,t_uint32 u8_Adsoc_PK_Emboss_Select_Ctrl,
t_uint32 u8_Adsoc_PK_Flipper_Ctrl,t_uint32 u8_Adsoc_PK_GrayBack_Ctrl)

  {
  char mess[256];
  t_uint32 pk_effects_Pipe0,emboss_Pipe0,flipper_Pipe0,grayBack_Pipe0;
  t_uint32 pk_effects_Pipe1,emboss_Pipe1,flipper_Pipe1,grayBack_Pipe1;

    snprintf(mess, sizeof(mess), "Test %d :check for Adsoc Default Control status ",testCount);
    mmte_testNext(mess);

      pk_effects_Pipe0    = IspRegRead_Value((ISP_CE0_RADIAL_ADSOC_PK_EFFECTS_OFFSET), (long*)&Read_Val, 1);
      emboss_Pipe0        =  pk_effects_Pipe0 & 0x7;
      flipper_Pipe0       = (pk_effects_Pipe0 >> 8)& 0x3;
      grayBack_Pipe0      = (pk_effects_Pipe0 >> 16)& 0x3;

        pk_effects_Pipe1 = IspRegRead_Value((ISP_CE1_RADIAL_ADSOC_PK_EFFECTS_OFFSET), (long*)&Read_Val, 1);
        emboss_Pipe1        =  pk_effects_Pipe1 & 0x7;
        flipper_Pipe1       = (pk_effects_Pipe1 >> 8)& 0x3;
        grayBack_Pipe1      = (pk_effects_Pipe1 >> 16)& 0x3;


            if (  (e_Flag_Adsoc_PK_Enable_Ctrl ==IspRegRead_Value((ISP_CE0_RADIAL_ADSOC_PK_ENABLE_OFFSET), (long*)&Read_Val, 1))
            &&
            (e_Flag_Adsoc_PK_Enable_Ctrl ==IspRegRead_Value((ISP_CE1_RADIAL_ADSOC_PK_ENABLE_OFFSET), (long*)&Read_Val, 1))
            &&
            (e_Flag_Adsoc_PK_AdaptiveSharpening_Enable_Ctrl ==IspRegRead_Value((ISP_CE0_RADIAL_ADSOC_PK_ADSHARP_EN_OFFSET), (long*)&Read_Val, 1))
            &&
            (e_Flag_Adsoc_PK_AdaptiveSharpening_Enable_Ctrl ==IspRegRead_Value((ISP_CE1_RADIAL_ADSOC_PK_ADSHARP_EN_OFFSET), (long*)&Read_Val, 1))
            &&
            (u8_Adsoc_PK_Emboss_Select_Ctrl ==emboss_Pipe0)
            &&
            (u8_Adsoc_PK_Emboss_Select_Ctrl ==emboss_Pipe1)
            &&
            (u8_Adsoc_PK_Flipper_Ctrl ==flipper_Pipe0)
            &&
            (u8_Adsoc_PK_Flipper_Ctrl ==flipper_Pipe1)
            &&
            (u8_Adsoc_PK_GrayBack_Ctrl ==grayBack_Pipe0)
            &&
            (u8_Adsoc_PK_GrayBack_Ctrl ==grayBack_Pipe1)
            )
            {
            //test passed
            LOS_Log("\nTEST PASS: Adsoc Default Control values as expected\n");
            mmte_testResult(TEST_PASSED);
            }
            else
            {
            //test failed
            LOS_Log("\nTEST FAIL: Adsoc Default Control values NOT as expected\n");
            mmte_testResult(TEST_FAILED);
            }
            testCount++;
            }
*/

//Seperate test case has been written for Adsoc

/* -----------------------------------------------------------------------
FUNCTION : ITE_testAdsocDefault_Params_PE
PURPOSE  : check Adsoc Default Params status page elements and decide if tests pass or fail
----------------------------------------------------------------------------------- */

/*void ITE_testAdsocDefault_Params_PE (t_uint32 u8_Adsoc_PK_Coring_Level, t_uint32 u8_Adsoc_PK_OverShoot_Gain_Bright,t_uint32 u8_Adsoc_PK_OverShoot_Gain_Dark,t_uint32 u8_Adsoc_PK_Gain)

  {
  char mess[256];
  t_uint32 ov_ctrl_Pipe0,ov_bright_Pipe0,ov_dark_Pipe0;
  t_uint32 ov_ctrl_Pipe1,ov_bright_Pipe1,ov_dark_Pipe1;

    snprintf(mess, sizeof(mess), "Test %d :check for Adsoc Default Params status ",testCount);
    mmte_testNext(mess);

      ov_ctrl_Pipe0 = IspRegRead_Value((ISP_CE0_RADIAL_ADSOC_PK_OSHOOT_CTRL_OFFSET), (long*)&Read_Val, 1);
      ov_bright_Pipe0 =  ov_ctrl_Pipe0 & 0x1f;
      ov_dark_Pipe0 = (ov_ctrl_Pipe0 >> 8) & 0x1f;


          ov_ctrl_Pipe1 = IspRegRead_Value((ISP_CE1_RADIAL_ADSOC_PK_OSHOOT_CTRL_OFFSET), (long*)&Read_Val, 1);
          ov_bright_Pipe1 =  ov_ctrl_Pipe1 & 0x1f;
          ov_dark_Pipe1  = (ov_ctrl_Pipe1 >> 8) & 0x1f;



                if (  (u8_Adsoc_PK_Coring_Level ==IspRegRead_Value((ISP_CE0_RADIAL_ADSOC_PK_CORING_LEVEL_OFFSET), (long*)&Read_Val, 1))
                &&
                (u8_Adsoc_PK_Coring_Level ==IspRegRead_Value((ISP_CE1_RADIAL_ADSOC_PK_CORING_LEVEL_OFFSET), (long*)&Read_Val, 1))
                &&
                (u8_Adsoc_PK_OverShoot_Gain_Bright ==ov_bright_Pipe0)
                &&
                (u8_Adsoc_PK_OverShoot_Gain_Bright ==ov_bright_Pipe1)
                &&
                (u8_Adsoc_PK_OverShoot_Gain_Dark ==ov_dark_Pipe0)
                &&
                (u8_Adsoc_PK_OverShoot_Gain_Dark ==ov_dark_Pipe1)
                &&
                (u8_Adsoc_PK_Gain ==IspRegRead_Value((ISP_CE0_RADIAL_ADSOC_PK_GAIN_OFFSET), (long*)&Read_Val, 1))
                &&
                (u8_Adsoc_PK_Gain ==IspRegRead_Value((ISP_CE1_RADIAL_ADSOC_PK_GAIN_OFFSET), (long*)&Read_Val, 1))

                  )
                  {
                  //test passed
                  LOS_Log("\nTEST PASS: Adsoc Default Params values as expected\n");
                  mmte_testResult(TEST_PASSED);
                  }
                  else
                  {
                  //test failed
                  LOS_Log("\nTEST FAIL: Adsoc Default Params values NOT as expected\n");
                  mmte_testResult(TEST_FAILED);
                  }
                  testCount++;
                  }

*/

/* -----------------------------------------------------------------------
FUNCTION : ITE_testBinningRepair
PURPOSE  : Test BinningRepair manual module
------------------------------------------------------------------------ */
void
ITE_testBinningRepair(
enum e_grabFormat   grbformat)
{
    char            testComment[200];
    char            lrformatName[16];
    char            sensorName[16];
    char            resolutionName[16];
    char            pathname[200];
    e_resolution    sizelr = SUBQCIF;

    if (usecase.sensor == 0)
    {
        snprintf(sensorName, sizeof(sensorName), "Cam0");
    }
    else
    {
        snprintf(sensorName, sizeof(sensorName), "Cam1");
    }

    usecase.LR_GrbFormat = grbformat;
    ITE_GiveStringFormat("LR", lrformatName);
    LOS_Log("\n --> Stream on LR pipe with GRBFMT_%s format\n", lrformatName);

    ITE_GiveStringNameFromEnum(sizelr, resolutionName);
    LOS_Log("\n -----> %s resolution\n", resolutionName);
    usecase.LR_XSize = ITE_GiveXSizeFromEnum(PIPE_LR, sizelr, grbformat);
    usecase.LR_YSize = ITE_GiveYSizeFromEnum(PIPE_LR, sizelr, grbformat);
    usecase.LR_resolution = ITE_GiveResolution(resolutionName);

    snprintf(testComment, sizeof(testComment), "%s_test_IP_BinningRepair_tests_%s_%s",
             sensorName, lrformatName, resolutionName);

    /* Perform a "cd" to destination repository for test log */
    snprintf(pathname, sizeof(pathname),
             IMAGING_PATH "/ite_nmf/test_results_nreg/IP_BinningRepair_tests_%s",
             lrformatName);
    strcat(g_out_path, pathname);
    LOS_Log("Current path: >%s< \n", ( char * ) g_out_path);
    init_test_results_nreg(g_out_path);
    mmte_testStart(testComment,
                   "test pipe LR (buffer + fr) YUV422 interleaved format",
                   g_out_path);

    ITE_testBinningRepairManual_StreamLR(&usecase);

    mmte_testEnd();

    /* Clear g_out_path */
    memset(g_out_path, 0, KlogDirectoryLentgh * sizeof(char));
}

/* -----------------------------------------------------------------------
FUNCTION : ITE_testBinningRepairManual_StreamLR
PURPOSE  : Test BinningRepair manual module on LR
------------------------------------------------------------------------ */
void
ITE_testBinningRepairManual_StreamLR(
tps_sia_usecase p_sia_usecase)
{
    t_uint32    e_Flag_BinningRepairEnable_Default = Flag_e_TRUE,
                e_Flag_H_Jog_Enable_Default = Flag_e_TRUE,
                e_Flag_V_Jog_Enable_Default = Flag_e_TRUE;
    t_uint32    e_BinningRepairMode_Default = BinningRepairMode_e_Custom,
                u8_Coeff_00_Default = 9,
                u8_Coeff_01_Default = 9,
                u8_Coeff_10_Default = 9,
                u8_Coeff_11_Default = 9,
                u8_Coeff_shift_Default = 3,
                u8_BinningRepair_factor_Default = 2;

    t_uint32    u8_Coeff22_00 = 1,
                u8_Coeff22_01 = 9,
                u8_Coeff22_10 = 7,
                u8_Coeff22_11 = 49,
                u8_Coeff22_shift = 5;
    t_uint32    u8_Coeff44_00 = 9,
                u8_Coeff44_01 = 39,
                u8_Coeff44_10 = 39,
                u8_Coeff44_11 = 169,
                u8_Coeff44_shift = 7;

    ITE_writePE(HostFrameConstraints_e_Flag_GuaranteeStaticFlickerFrameLength_Byte0, Flag_e_FALSE);

    // prepare buffers/LR pipe and Start streaming on LR pipe
    ITE_StreamLR(p_sia_usecase);

    LOS_Log("\n1secs stream on LR pipe\n");
    LOS_Sleep(500);

    ITE_NMF_Toggle_System_Coin();

    // Set BinningRepairManual default Control page elements
    ITE_setBinningRepairManualDefault_Ctrl_PE(
    e_Flag_BinningRepairEnable_Default,
    e_Flag_H_Jog_Enable_Default,
    e_Flag_V_Jog_Enable_Default,
    e_BinningRepairMode_Default,
    u8_Coeff_00_Default,
    u8_Coeff_01_Default,
    u8_Coeff_10_Default,
    u8_Coeff_11_Default,
    u8_Coeff_shift_Default,
    u8_BinningRepair_factor_Default);
    ITE_NMF_Toggle_System_Coin();

    // Test BinningRepairManual default Control page elements
    ITE_testBinningRepairManualDefault_Ctrl_PE(
    e_Flag_BinningRepairEnable_Default,
    e_Flag_H_Jog_Enable_Default,
    e_Flag_V_Jog_Enable_Default,
    u8_Coeff_00_Default,
    u8_Coeff_01_Default,
    u8_Coeff_10_Default,
    u8_Coeff_11_Default,
    u8_Coeff_shift_Default,
    u8_BinningRepair_factor_Default);
    ITE_NMF_Toggle_System_Coin();

    // Set BinningRepairManual default Control page elements for 2*2
    ITE_setBinningRepairManualDefault22_Color_Ctrl_PE(
    u8_Coeff22_00,
    u8_Coeff22_01,
    u8_Coeff22_10,
    u8_Coeff22_11,
    u8_Coeff22_shift);
    ITE_NMF_Toggle_System_Coin();

    // Test BinningRepairManual default Control page elements for 2*2
    ITE_testBinningRepairManualDefault22_Color_Ctrl_PE(
    u8_Coeff22_00,
    u8_Coeff22_01,
    u8_Coeff22_10,
    u8_Coeff22_11,
    u8_Coeff22_shift);

    ITE_NMF_Toggle_System_Coin();

    // Set BinningRepairManual default Control page elements for 4*4
    ITE_setBinningRepairManualDefault44_Color_Ctrl_PE(
    u8_Coeff44_00,
    u8_Coeff44_01,
    u8_Coeff44_10,
    u8_Coeff44_11,
    u8_Coeff44_shift);
    ITE_NMF_Toggle_System_Coin();

    // Test BinningRepairManual default Control page elements for 4*4
    ITE_testBinningRepairManualDefault44_Color_Ctrl_PE(
    u8_Coeff44_00,
    u8_Coeff44_01,
    u8_Coeff44_10,
    u8_Coeff44_11,
    u8_Coeff44_shift);

    ITE_LR_Stop();
    ITE_LR_Free();
}


/* -----------------------------------------------------------------------
FUNCTION : ITE_setBinningRepairManualDefault_Ctrl_PE
PURPOSE  : Set Default Binning Repair Manual Control page elements
------------------------------------------------------------------------ */
void
ITE_setBinningRepairManualDefault_Ctrl_PE(
t_uint32    e_Flag_BinningRepairEnable_Default,
t_uint32    e_Flag_H_Jog_Enable_Default,
t_uint32    e_Flag_V_Jog_Enable_Default,
t_uint32    e_BinningRepairMode_Default,
t_uint32    u8_Coeff_00_Default,
t_uint32    u8_Coeff_01_Default,
t_uint32    u8_Coeff_10_Default,
t_uint32    u8_Coeff_11_Default,
t_uint32    u8_Coeff_shift_Default,
t_uint32    u8_BinningRepair_factor_Default)
{
    ITE_writePE(BinningRepair_Ctrl_e_Flag_BinningRepairEnable_Byte0, e_Flag_BinningRepairEnable_Default);
    ITE_writePE(BinningRepair_Ctrl_e_Flag_H_Jog_Enable_Byte0, e_Flag_H_Jog_Enable_Default);
    ITE_writePE(BinningRepair_Ctrl_e_Flag_V_Jog_Enable_Byte0, e_Flag_V_Jog_Enable_Default);
    ITE_writePE(BinningRepair_Ctrl_e_BinningRepairMode_Byte0, e_BinningRepairMode_Default);
    ITE_writePE(BinningRepair_Ctrl_u8_Coeff_00_Byte0, u8_Coeff_00_Default);
    ITE_writePE(BinningRepair_Ctrl_u8_Coeff_01_Byte0, u8_Coeff_01_Default);
    ITE_writePE(BinningRepair_Ctrl_u8_Coeff_10_Byte0, u8_Coeff_10_Default);
    ITE_writePE(BinningRepair_Ctrl_u8_Coeff_11_Byte0, u8_Coeff_11_Default);
    ITE_writePE(BinningRepair_Ctrl_u8_Coeff_shift_Byte0, u8_Coeff_shift_Default);
    ITE_writePE(BinningRepair_Ctrl_u8_BinningRepair_factor_Byte0, u8_BinningRepair_factor_Default);
    LOS_Sleep(2000);
}


/* -----------------------------------------------------------------------
FUNCTION : ITE_testBinningRepairManualDefault_Ctrl_PE
PURPOSE  : check Binning Repair Manual Default Control status page elements and decide if tests pass or fail
----------------------------------------------------------------------------------- */
void
ITE_testBinningRepairManualDefault_Ctrl_PE(
t_uint32    e_Flag_BinningRepairEnable_Default,
t_uint32    e_Flag_H_Jog_Enable_Default,
t_uint32    e_Flag_V_Jog_Enable_Default,
t_uint32    u8_Coeff_00_Default,
t_uint32    u8_Coeff_01_Default,
t_uint32    u8_Coeff_10_Default,
t_uint32    u8_Coeff_11_Default,
t_uint32    u8_Coeff_shift_Default,
t_uint32    u8_BinningRepair_factor_Default)
{
    char        mess[256];
    t_uint32    temp1;  // temp2;
    t_uint32    e_h_jog_en,
                e_v_jog_en;
    UNUSED(u8_BinningRepair_factor_Default);

    snprintf(mess, sizeof(mess), "Test %d :check for Binning Repair Manual Default Control status ", ( int ) testCount);
    mmte_testNext(mess);

    //temp1 = ISP_R_R(ISP_BINNING_REPAIR_ENABLE);
    //LOS_Sleep(500);
    //temp2=  ISP_R_R(ISP_BINNING_REPAIR_CONTROL);
    //LOS_Sleep(500);
    temp1 = IspRegRead_Value((ISP_BINNING_REPAIR_CONTROL_OFFSET), ( long * ) &Read_Val, 1);
    e_h_jog_en = (temp1 >> 4) & 0x1;
    e_v_jog_en = (temp1 >> 5) & 0x1;

    if
    (
        (
            e_Flag_BinningRepairEnable_Default == IspRegRead_Value(
                (ISP_BINNING_REPAIR_ENABLE_OFFSET),
                ( long * ) &Read_Val,
                1)
        )
    &&  (e_Flag_H_Jog_Enable_Default == e_h_jog_en)
    &&  (e_Flag_V_Jog_Enable_Default == e_v_jog_en)
    &&  (u8_Coeff_00_Default == IspRegRead_Value((ISP_BINNING_REPAIR_COEF00_OFFSET), ( long * ) &Read_Val, 1))
    &&  (u8_Coeff_01_Default == IspRegRead_Value((ISP_BINNING_REPAIR_COEF01_OFFSET), ( long * ) &Read_Val, 1))
    &&  (u8_Coeff_10_Default == IspRegRead_Value((ISP_BINNING_REPAIR_COEF10_OFFSET), ( long * ) &Read_Val, 1))
    &&  (u8_Coeff_11_Default == IspRegRead_Value((ISP_BINNING_REPAIR_COEF11_OFFSET), ( long * ) &Read_Val, 1))
    &&  (u8_Coeff_shift_Default == IspRegRead_Value((ISP_BINNING_REPAIR_COEF_SFT_OFFSET), ( long * ) &Read_Val, 1))
    )
    {
        //test passed
        LOS_Log("\nTEST PASS: Binning Repair Manual Default Control values as expected\n");
        mmte_testResult(TEST_PASSED);
    }
    else
    {
        //test failed
        LOS_Log("\nTEST FAIL: Binning Repair Manual Default Control values NOT as expected\n");
        mmte_testResult(TEST_FAILED);
    }


    testCount++;
}


/* -----------------------------------------------------------------------
FUNCTION : ITE_setBinningRepairManualDefault22_Color_Ctrl_PE
PURPOSE  : Set Default Binning Repair Manual 2*2 Control page elements
------------------------------------------------------------------------ */
void
ITE_setBinningRepairManualDefault22_Color_Ctrl_PE(
t_uint32    u8_Coeff22_00,
t_uint32    u8_Coeff22_01,
t_uint32    u8_Coeff22_10,
t_uint32    u8_Coeff22_11,
t_uint32    u8_Coeff22_shift)
{
    ITE_writePE(BinningRepair_Ctrl_u8_Coeff_00_Byte0, u8_Coeff22_00);
    ITE_writePE(BinningRepair_Ctrl_u8_Coeff_01_Byte0, u8_Coeff22_01);
    ITE_writePE(BinningRepair_Ctrl_u8_Coeff_10_Byte0, u8_Coeff22_10);
    ITE_writePE(BinningRepair_Ctrl_u8_Coeff_11_Byte0, u8_Coeff22_11);
    ITE_writePE(BinningRepair_Ctrl_u8_Coeff_shift_Byte0, u8_Coeff22_shift);
    LOS_Sleep(500);
}


/* -----------------------------------------------------------------------
FUNCTION : ITE_testBinningRepairManualDefault22_Color_Ctrl_PE
PURPOSE  : check Binning Repair Manual Default 2*2  Control status page elements and decide if tests pass or fail
----------------------------------------------------------------------------------- */
void
ITE_testBinningRepairManualDefault22_Color_Ctrl_PE(
t_uint32    u8_Coeff22_00,
t_uint32    u8_Coeff22_01,
t_uint32    u8_Coeff22_10,
t_uint32    u8_Coeff22_11,
t_uint32    u8_Coeff22_shift)
{
    char    mess[256];

    snprintf(
    mess,
    sizeof(mess),
    "Test %d :check for Binning Repair Manual 2*2 Default Control status ",
    ( int ) testCount);
    mmte_testNext(mess);

    if
    (
        (u8_Coeff22_00 == IspRegRead_Value((ISP_BINNING_REPAIR_COEF00_OFFSET), ( long * ) &Read_Val, 1))
    &&  (u8_Coeff22_01 == IspRegRead_Value((ISP_BINNING_REPAIR_COEF01_OFFSET), ( long * ) &Read_Val, 1))
    &&  (u8_Coeff22_10 == IspRegRead_Value((ISP_BINNING_REPAIR_COEF10_OFFSET), ( long * ) &Read_Val, 1))
    &&  (u8_Coeff22_11 == IspRegRead_Value((ISP_BINNING_REPAIR_COEF11_OFFSET), ( long * ) &Read_Val, 1))
    &&  (u8_Coeff22_shift == IspRegRead_Value((ISP_BINNING_REPAIR_COEF_SFT_OFFSET), ( long * ) &Read_Val, 1))
    )
    {
        //test passed
        LOS_Log("\nTEST PASS: Binning Repair Manual Default 2*2 Control values as expected\n");
        mmte_testResult(TEST_PASSED);
    }
    else
    {
        //test failed
        LOS_Log("\nTEST FAIL: Binning Repair Manual Default 2*2 Control values NOT as expected\n");
        mmte_testResult(TEST_FAILED);
    }


    testCount++;
}


/* -----------------------------------------------------------------------
FUNCTION : ITE_setBinningRepairManualDefault44_Color_Ctrl_PE
PURPOSE  : Set Default Binning Repair Manual 4*4 Control page elements
------------------------------------------------------------------------ */
void
ITE_setBinningRepairManualDefault44_Color_Ctrl_PE(
t_uint32    u8_Coeff44_00,
t_uint32    u8_Coeff44_01,
t_uint32    u8_Coeff44_10,
t_uint32    u8_Coeff44_11,
t_uint32    u8_Coeff44_shift)
{
    ITE_writePE(BinningRepair_Ctrl_u8_Coeff_00_Byte0, u8_Coeff44_00);
    ITE_writePE(BinningRepair_Ctrl_u8_Coeff_01_Byte0, u8_Coeff44_01);
    ITE_writePE(BinningRepair_Ctrl_u8_Coeff_10_Byte0, u8_Coeff44_10);
    ITE_writePE(BinningRepair_Ctrl_u8_Coeff_11_Byte0, u8_Coeff44_11);
    ITE_writePE(BinningRepair_Ctrl_u8_Coeff_shift_Byte0, u8_Coeff44_shift);
    LOS_Sleep(500);
}


/* -----------------------------------------------------------------------
FUNCTION : ITE_testBinningRepairManualDefault_Ctrl_PE
PURPOSE  : check Binning Repair Manual Default 4*4 Control status page elements and decide if tests pass or fail
----------------------------------------------------------------------------------- */
void
ITE_testBinningRepairManualDefault44_Color_Ctrl_PE(
t_uint32    u8_Coeff44_00,
t_uint32    u8_Coeff44_01,
t_uint32    u8_Coeff44_10,
t_uint32    u8_Coeff44_11,
t_uint32    u8_Coeff44_shift)
{
    char    mess[256];
    snprintf(
    mess,
    sizeof(mess),
    "Test %d :check for Binning Repair Manual 4*4  Default Control status ",
    ( int ) testCount);
    mmte_testNext(mess);

    if
    (
        (u8_Coeff44_00 == IspRegRead_Value((ISP_BINNING_REPAIR_COEF00_OFFSET), ( long * ) &Read_Val, 1))
    &&  (u8_Coeff44_01 == IspRegRead_Value((ISP_BINNING_REPAIR_COEF01_OFFSET), ( long * ) &Read_Val, 1))
    &&  (u8_Coeff44_10 == IspRegRead_Value((ISP_BINNING_REPAIR_COEF10_OFFSET), ( long * ) &Read_Val, 1))
    &&  (u8_Coeff44_11 == IspRegRead_Value((ISP_BINNING_REPAIR_COEF11_OFFSET), ( long * ) &Read_Val, 1))
    &&  (u8_Coeff44_shift == IspRegRead_Value((ISP_BINNING_REPAIR_COEF_SFT_OFFSET), ( long * ) &Read_Val, 1))
    )
    {
        //test passed
        LOS_Log("\nTEST PASS: Binning Repair Manual Default 4*4 Control values as expected\n");
        mmte_testResult(TEST_PASSED);
    }
    else
    {
        //test failed
        LOS_Log("\nTEST FAIL: Binning Repair Manual Default 4*4 Control values NOT as expected\n");
        mmte_testResult(TEST_FAILED);
    }


    testCount++;
}


/* -----------------------------------------------------------------------
FUNCTION : ITE_NMF_SelfTest_HR
PURPOSE  :
------------------------------------------------------------------------ */
void
ITE_NMF_SelfTest_HR(
enum e_grabFormat   grbformat)
{
    char            testComment[200];
    char            HrformatName[16];
    char            sensorName[16];
    char            resolutionName[16];
    char            pathname[200];
    e_resolution    sizeHr = SUBQCIF;
    e_sensormode    SensorMode;
    char            SensorModeName[20];
    char            FileName[256];

    if (usecase.sensor == 0)
    {
        sprintf(sensorName, "Cam0");
    }
    else
    {
        sprintf(sensorName, "Cam1");
    }

    usecase.HR_GrbFormat = grbformat;
    ITE_GiveStringFormat("HR", HrformatName);
    LOS_Log("\n --> Stream on HR pipe with GRBFMT_%s format\n", HrformatName);

    ITE_GiveStringNameFromEnum(sizeHr, resolutionName);
    LOS_Log("\n -----> %s resolution\n", resolutionName);
    usecase.HR_XSize = ITE_GiveXSizeFromEnum(PIPE_HR, sizeHr, grbformat);
    usecase.HR_YSize = ITE_GiveYSizeFromEnum(PIPE_HR, sizeHr, grbformat);
    usecase.HR_resolution = ITE_GiveResolution(resolutionName);
    sprintf(testComment, "%s_test_SelfTest_tests_%s_%s",
            sensorName, HrformatName, resolutionName);

    /* Perform a "cd" to destination repository for test log */
    sprintf(pathname,
            IMAGING_PATH "/ite_nmf/test_results_nreg/SelfTest_tests_%s",
            HrformatName);
    strcat(g_out_path, pathname);
    LOS_Log("Current path: >%s< \n", ( char * ) g_out_path);
    init_test_results_nreg(g_out_path);
    mmte_testStart(testComment, "test pipe HR YUV422 interleaved format",
                   g_out_path);

    /* Strart Streaming */
    ITE_writePE(HostFrameConstraints_e_Flag_GuaranteeStaticFlickerFrameLength_Byte0,
                Flag_e_FALSE);

    /* Prepare buffers/HR pipe and Start streaming on Hr pipe */
    //ITE_StreamHR(&usecase);
    //LOS_Log("\n1secs stream on HR pipe\n");

    for (SensorMode = NORMAL_MODE; SensorMode <= SOLIDBAR_GREEN; SensorMode++)
    {
        if ((SensorMode == HORIZ_GREY_SCALE) || (SensorMode == VERT_GREY_SCALE))
        {
            continue;
        }

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
        LOS_Sleep(500);
        LOS_Log("\n1secs stream on HR pipe\n");

        LOS_Log("\n1secs stream on HR pipe\n");
        LOS_Sleep(500);
        ITE_GiveStringNameFromModeEnum(SensorMode, SensorModeName);
        sprintf(FileName, "%s_HR", SensorModeName);

        ITE_NMF_SelfTest_Result(SensorMode, SensorModeName, "HR");
        snprintf(FileName, sizeof(FileName), "%s/HR_YUV422_%s", pathname, SensorModeName);
        ITE_StoreinBMPFile(FileName, &(GrabBufferHR[0]));
        ITE_HR_Stop();
    }

    ITE_HR_Free();
}

/* -----------------------------------------------------------------------
FUNCTION : ITE_NMF_SelfTest_LR
PURPOSE  :
------------------------------------------------------------------------ */
void
ITE_NMF_SelfTest_LR(
enum e_grabFormat   grbformat)
{
    char            testComment[200];
    char            lrformatName[16];
    char            sensorName[16];
    char            resolutionName[16];
    char            pathname[200];
    e_resolution    sizelr = SUBQCIF;
    e_sensormode    SensorMode;
    char            SensorModeName[20];
    char            FileName[256];

    if (usecase.sensor == 0)
    {
        snprintf(sensorName, sizeof(sensorName), "Cam0");
    }
    else
    {
        snprintf(sensorName, sizeof(sensorName), "Cam1");
    }

    usecase.LR_GrbFormat = grbformat;
    ITE_GiveStringFormat("LR", lrformatName);
    LOS_Log("\n --> Stream on LR pipe with GRBFMT_%s format\n", lrformatName);

    ITE_GiveStringNameFromEnum(sizelr, resolutionName);
    LOS_Log("\n -----> %s resolution\n", resolutionName);
    usecase.LR_XSize = ITE_GiveXSizeFromEnum(PIPE_LR, sizelr, grbformat);
    usecase.LR_YSize = ITE_GiveYSizeFromEnum(PIPE_LR, sizelr, grbformat);
    usecase.LR_resolution = ITE_GiveResolution(resolutionName);
    snprintf(testComment, sizeof(testComment), "%s_test_SelfTest_tests_%s_%s",
             sensorName, lrformatName, resolutionName);

    /* Perform a "cd" to destination repository for test log */
    snprintf(pathname, sizeof(pathname),
             IMAGING_PATH "/ite_nmf/test_results_nreg/SelfTest_tests_%s",
             lrformatName);
    strcat(g_out_path, pathname);
    LOS_Log("Current path: >%s< \n", ( char * ) g_out_path);
    init_test_results_nreg(g_out_path);
    mmte_testStart(testComment, " test pipe LR YUV422 interleaved format", g_out_path);

    /* Strart Streaming */
    ITE_writePE(HostFrameConstraints_e_Flag_GuaranteeStaticFlickerFrameLength_Byte0,
                Flag_e_FALSE);

    /* Prepare buffers/LR pipe and Start streaming on LR pipe */
    for (SensorMode = NORMAL_MODE; SensorMode <= SOLIDBAR_GREEN; SensorMode++)
    {
        if ((SensorMode == HORIZ_GREY_SCALE) || (SensorMode == VERT_GREY_SCALE))
        {
            continue;
        }

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
        LOS_Sleep(500);
        LOS_Log("\n1secs stream on LR pipe\n");
        ITE_GiveStringNameFromModeEnum(SensorMode, SensorModeName);
        sprintf(FileName, "%s_LR", SensorModeName);

        ITE_NMF_SelfTest_Result(SensorMode, SensorModeName, "LR");
        snprintf(FileName, sizeof(FileName), "%s/LR_YUV422_%s", pathname, SensorModeName);
        ITE_StoreinBMPFile(FileName, &(GrabBufferLR[0]));
        ITE_LR_Stop();
    }

    ITE_LR_Free();
}

/* -----------------------------------------------------------------------
FUNCTION : ITE_NMF_SelfTest_Result
PURPOSE  :
------------------------------------------------------------------------ */
void
ITE_NMF_SelfTest_Result(
e_sensormode    SensorMode,
char            *SensorModeName,
char            *Pipe)
{
    t_uint16        u16_Red,
                    u16_blue,
                    u16_Green_r,
                    u16_Green_b;
    TestPattern_te  e_TestPattern;
    u16_Red = 0;
    u16_blue = 0;
    u16_Green_b = 0;
    u16_Green_r = 0;

    u16_Red = ITE_readPE(TestPattern_Status_u16_test_data_red_Byte0);
    u16_blue = ITE_readPE(TestPattern_Status_u16_test_data_blue_Byte0);
    u16_Green_r = ITE_readPE(TestPattern_Status_u16_test_data_greenR_Byte0);
    u16_Green_b = ITE_readPE(TestPattern_Status_u16_test_data_greenB_Byte0);
    e_TestPattern = ITE_readPE(TestPattern_Status_e_TestPattern_Byte0);

    switch (SensorMode)
    {
        case NORMAL_MODE:
            if (TestPattern_e_Normal == e_TestPattern)
            {
                LOS_Log("\nTest Pass for %s pipe mode %s\n", Pipe, SensorModeName);
            }
            else
            {
                LOS_Log("\nTest Fail for %s pipe mode %s\n", Pipe, SensorModeName);
            }


            break;

        case COLORBAR:
            if (TestPattern_e_SolidColourBars == e_TestPattern)
            {
                LOS_Log("\nTest Pass for %s pipe mode %s\n", Pipe, SensorModeName);
            }
            else
            {
                LOS_Log("\nTest Fail for %s pipe mode %s\n", Pipe, SensorModeName);
            }


            break;

        case FADETOGREY_COLORBAR:
            if (TestPattern_e_SolidColourBarsFade == e_TestPattern)
            {
                LOS_Log("\nTest Pass for %s pipe mode %s\n", Pipe, SensorModeName);
            }
            else
            {
                LOS_Log("\nTest Fail for %s pipe mode %s\n", Pipe, SensorModeName);
            }


            break;

        case PN28:
            if (TestPattern_e_PN9 == e_TestPattern)
            {
                LOS_Log("\nTest Pass for %s pipe mode %s\n", Pipe, SensorModeName);
            }
            else
            {
                LOS_Log("\nTest Fail for %s pipe mode %s\n", Pipe, SensorModeName);
            }


            break;

        case SOLIDBAR_BLACK:
            if
            (
                (u16_Red == 0)
            &&  (u16_blue == 0)
            &&  (u16_Green_b == 0)
            &&  (u16_Green_r == 0)
            &&  (TestPattern_e_SolidColour == e_TestPattern)
            )
            {
                LOS_Log("\nTest Pass for %s pipe mode %s\n", Pipe, SensorModeName);
            }
            else
            {
                LOS_Log("\nTest Fail for %s pipe mode %s\n", Pipe, SensorModeName);
            }


            break;

        case SOLIDBAR_BLUE:
            if
            (
                (u16_Red == 0)
            &&  (u16_blue == 0x03ff)
            &&  (u16_Green_b == 0)
            &&  (u16_Green_r == 0)
            &&  (TestPattern_e_SolidColour == e_TestPattern)
            )
            {
                LOS_Log("\nTest Pass for %s pipe mode %s\n", Pipe, SensorModeName);
            }
            else
            {
                LOS_Log("\nTest Fail for %s pipe mode %s\n", Pipe, SensorModeName);
            }


            break;

        case SOLIDBAR_CYAN:
            if
            (
                (u16_Red == 0)
            &&  (u16_blue == 0x03ff)
            &&  (u16_Green_b == 0x03ff)
            &&  (u16_Green_r == 0x03ff)
            &&  (TestPattern_e_SolidColour == e_TestPattern)
            )
            {
                LOS_Log("\nTest Pass for %s pipe mode %s\n", Pipe, SensorModeName);
            }
            else
            {
                LOS_Log("\nTest Fail for %s pipe mode %s\n", Pipe, SensorModeName);
            }


            break;

        case SOLIDBAR_GREEN:
            if
            (
                (u16_Red == 0)
            &&  (u16_blue == 0)
            &&  (u16_Green_b == 0x03ff)
            &&  (u16_Green_r == 0x03ff)
            &&  (TestPattern_e_SolidColour == e_TestPattern)
            )
            {
                LOS_Log("\nTest Pass for %s pipe mode %s\n", Pipe, SensorModeName);
            }
            else
            {
                LOS_Log("\nTest Fail for %s pipe mode %s\n", Pipe, SensorModeName);
            }


            break;

        case SOLIDBAR_MAGENTA:
            if
            (
                (u16_Red == 0x03ff)
            &&  (u16_blue == 0x03ff)
            &&  (u16_Green_b == 0)
            &&  (u16_Green_r == 0)
            &&  (TestPattern_e_SolidColour == e_TestPattern)
            )
            {
                LOS_Log("\nTest Pass for %s pipe mode %s\n", Pipe, SensorModeName);
            }
            else
            {
                LOS_Log("\nTest Fail for %s pipe mode %s\n", Pipe, SensorModeName);
            }


            break;

        case SOLIDBAR_RED:
            if
            (
                (u16_Red == 0x03ff)
            &&  (u16_blue == 0)
            &&  (u16_Green_b == 0)
            &&  (u16_Green_r == 0)
            &&  (TestPattern_e_SolidColour == e_TestPattern)
            )
            {
                LOS_Log("\nTest Pass for %s pipe mode %s\n", Pipe, SensorModeName);
            }
            else
            {
                LOS_Log("\nTest Fail for %s pipe mode %s\n", Pipe, SensorModeName);
            }


            break;

        case SOLIDBAR_WHITE:
            if
            (
                (u16_Red == 0x03ff)
            &&  (u16_blue == 0x03ff)
            &&  (u16_Green_b == 0x03ff)
            &&  (u16_Green_r == 0x03ff)
            &&  (TestPattern_e_SolidColour == e_TestPattern)
            )
            {
                LOS_Log("\nTest Pass for %s pipe mode %s\n", Pipe, SensorModeName);
            }
            else
            {
                LOS_Log("\nTest Fail for %s pipe mode %s\n", Pipe, SensorModeName);
            }


            break;

        case SOLIDBAR_YELLOW:
            if
            (
                (u16_Red == 0x03ff)
            &&  (u16_blue == 0)
            &&  (u16_Green_b == 0x03ff)
            &&  (u16_Green_r == 0x03ff)
            &&  (TestPattern_e_SolidColour == e_TestPattern)
            )
            {
                LOS_Log("\nTest Pass for %s pipe mode %s\n", Pipe, SensorModeName);
            }
            else
            {
                LOS_Log("\nTest Fail for %s pipe mode %s\n", Pipe, SensorModeName);
            }


            break;

        default:
            break;
    }
}


/* -----------------------------------------------------------------------
FUNCTION : ITE_testStillBMSdatapath
PURPOSE  : Test BMS datapath with raw8 ,  raw12 format
for FFOV : Fast BMS (BAYERSTORE0) and BMS (BAYERSTORE2)
------------------------------------------------------------------------*/
void
ITE_NMF_SelfTest_BMS(void)
{
    e_resolution    sizebms = SUBQCIF;
    e_resolution    maxSizebms = SUBQCIF;
    char            testComment[200];
    char            resolutionName[16];
    char            sensorName[16];
    e_sensormode    SensorMode;
    char            SensorModeName[20];
    char            FileName[256];

    if (usecase.sensor == 0)
    {
        sprintf(sensorName, "Cam0");
    }
    else
    {
        sprintf(sensorName, "Cam1");
    }


    //FAST_RAW12 Test
    usecase.BMS_XSize = 3264;
    usecase.BMS_YSize = 2448;
    usecase.BMS_Mode = BMSSTILL;
    usecase.BMS_output = BAYERSTORE0;
    usecase.BMS_GrbFormat = GRBFMT_FAST_RAW12;
    LOS_Log("\n --> Stream 1 frame on BMS pipe FFOV with GRBFMT_FAST_RAW12 format\n");
    ITE_GiveStringNameFromEnum(maxSizebms, resolutionName);
    LOS_Log("\n -----> %s resolution\n", resolutionName);

    //perform a "cd" to destination repository for test log
    strcat(g_out_path, IMAGING_PATH "/ite_nmf/test_results_nreg/datapath_BMS_RAW12");
    LOS_Log("Current path: >%s< \n", ( char * ) g_out_path);
    init_test_results_nreg(g_out_path);
    sprintf(testComment, "%s_test_nreg_BMS_still_datapath_FASTRAW12_%s", sensorName, resolutionName);
    mmte_testStart(testComment, " test BMS buffer FAST RAW12 format", g_out_path);

    for (SensorMode = NORMAL_MODE; SensorMode <= SOLIDBAR_GREEN; SensorMode++)
    {
        if (SensorMode != COLORBAR)
        //if (( SensorMode == HORIZ_GREY_SCALE) || (SensorMode == VERT_GREY_SCALE))
        {
            continue;
        }


        if (g_InputImageSource == InputImageSource_e_Sensor0)
        {
            ITE_SensorMode(COLORBAR);
        }
        else
        {
            ITE_SetRxTestPattern(RxTestPattern_e_ColourBars);
        }


        ITE_testStreamBMS(&usecase, sizebms);

        LOS_Log("\n1secs stream on BMS pipe\n");
        LOS_Sleep(500);

        ITE_GiveStringNameFromModeEnum(SensorMode, SensorModeName);
        snprintf(
        FileName,
        sizeof(FileName),
        IMAGING_PATH "/imaging/ite_nmf/test_results_nreg/datapath_BMS_RAW8/BMS_R8_%s",
        SensorModeName);
        ITE_StoreinBMPFile(FileName, &(GrabBufferBMS[0]));

        //ITE_BMS_Stop();
    }


    /////////////////////////////////////////////////////////////////////////////////////
    //RAW12 Test
    usecase.BMS_output = BAYERSTORE2;
    usecase.BMS_GrbFormat = GRBFMT_RAW12;
    LOS_Log("\n --> Stream 1 frame on BMS pipe FFOV with GRBFMT_RAW12 format\n");
    sprintf(testComment, "%s_test_nreg_BMS_still_datapath_RAW12_%s", sensorName, resolutionName);
    mmte_testStart(testComment, " test BMS buffer RAW12 format", g_out_path);

    for (SensorMode = NORMAL_MODE; SensorMode <= SOLIDBAR_GREEN; SensorMode++)
    {
        if (SensorMode != COLORBAR)
        //if (( SensorMode == HORIZ_GREY_SCALE) || (SensorMode == VERT_GREY_SCALE))
        {
            continue;
        }


        if (g_InputImageSource == InputImageSource_e_Sensor0)
        {
            ITE_SensorMode(COLORBAR);
        }
        else
        {
            ITE_SetRxTestPattern(RxTestPattern_e_ColourBars);
        }


        ITE_testStreamBMS(&usecase, sizebms);
        LOS_Log("\n1secs stream on BMS pipe\n");
        LOS_Sleep(500);

        ITE_GiveStringNameFromModeEnum(SensorMode, SensorModeName);
        snprintf(
        FileName,
        sizeof(FileName),
        IMAGING_PATH "/imaging/ite_nmf/test_results_nreg/datapath_BMS_RAW8/BMS_R12_%s",
        SensorModeName);
        ITE_StoreinBMPFile(FileName, &(GrabBufferBMS[0]));
        ITE_BMS_Stop();
    }


    /////////////////////////////////////////////////////////////////////////////////////
    //FAST_RAW8 Test
    usecase.BMS_XSize = 3264;
    usecase.BMS_YSize = 2448;
    usecase.BMS_Mode = BMSSTILL;
    usecase.BMS_output = BAYERSTORE0;
    usecase.BMS_GrbFormat = GRBFMT_FAST_RAW8;
    LOS_Log("\n --> Stream 1 frame on BMS pipe FFOV with GRBFMT_FAST_RAW8 format\n");
    ITE_GiveStringNameFromEnum(maxSizebms, resolutionName);
    LOS_Log("\n -----> %s resolution\n", resolutionName);

    //perform a "cd" to destination repository for test log
    strcat(g_out_path, IMAGING_PATH "/imaging/ite_nmf/test_results_nreg/datapath_BMS_RAW8");
    LOS_Log("Current path: >%s< \n", ( char * ) g_out_path);
    init_test_results_nreg(g_out_path);
    sprintf(testComment, "%s_test_nreg_BMS_still_datapath_FASTRAW8_%s", sensorName, resolutionName);
    mmte_testStart(testComment, " test BMS buffer FAST RAW8 format", g_out_path);

    for (SensorMode = NORMAL_MODE; SensorMode <= SOLIDBAR_GREEN; SensorMode++)
    {
        if (SensorMode != COLORBAR)
        //if (( SensorMode == HORIZ_GREY_SCALE) || (SensorMode == VERT_GREY_SCALE))
        {
            continue;
        }


        if (g_InputImageSource == InputImageSource_e_Sensor0)
        {
            ITE_SensorMode(COLORBAR);
        }
        else
        {
            ITE_SetRxTestPattern(RxTestPattern_e_ColourBars);
        }


        ITE_testStreamBMS(&usecase, sizebms);
        LOS_Log("\n1secs stream on BMS pipe\n");
        LOS_Sleep(500);

        ITE_GiveStringNameFromModeEnum(SensorMode, SensorModeName);
        snprintf(
        FileName,
        sizeof(FileName),
        IMAGING_PATH "/imaging/ite_nmf/test_results_nreg/datapath_BMS_RAW8/BMS_R8_%s",
        SensorModeName);
        ITE_StoreinBMPFile(FileName, &(GrabBufferBMS[0]));
        ITE_BMS_Stop();
    }


    /////////////////////////////////////////////////////////////////////////////////////
    //RAW8 Test
    usecase.BMS_output = BAYERSTORE2;
    usecase.BMS_GrbFormat = GRBFMT_RAW8;
    LOS_Log("\n --> Stream 1 frame on BMS pipe FFOV with GRBFMT_RAW8 format\n");
    sprintf(testComment, "%s_test_nreg_BMS_still_datapath_RAW8_%s", sensorName, resolutionName);
    mmte_testStart(testComment, " test BMS buffer RAW8 format", g_out_path);

    for (SensorMode = NORMAL_MODE; SensorMode <= SOLIDBAR_GREEN; SensorMode++)
    {
        if (SensorMode != COLORBAR)
        //if (( SensorMode == HORIZ_GREY_SCALE) || (SensorMode == VERT_GREY_SCALE))
        {
            continue;
        }


        if (g_InputImageSource == InputImageSource_e_Sensor0)
        {
            ITE_SensorMode(COLORBAR);
        }
        else
        {
            ITE_SetRxTestPattern(RxTestPattern_e_ColourBars);
        }


        ITE_testStreamBMS(&usecase, sizebms);
        LOS_Log("\n1secs stream on BMS pipe\n");
        LOS_Sleep(500);

        ITE_GiveStringNameFromModeEnum(SensorMode, SensorModeName);
        snprintf(
        FileName,
        sizeof(FileName),
        IMAGING_PATH "/imaging/ite_nmf/test_results_nreg/datapath_BMS_RAW8/BMS_R8_%s",
        SensorModeName);
        ITE_StoreinBMPFile(FileName, &(GrabBufferBMS[0]));
        ITE_BMS_Stop();
    }


    /////////////////////////////////////////////////////////////////////////////////////
}


/* -----------------------------------------------------------------------
FUNCTION : ITE_GiveStringNameFromModeEnum
PURPOSE  :
------------------------------------------------------------------------ */
void
ITE_GiveStringNameFromModeEnum(
e_sensormode    SensorMode,
char            *name)
{
    switch (SensorMode)
    {
        case (NORMAL_MODE):         sprintf(name, "NORMAL_MODE"); break;
        case (COLORBAR):            sprintf(name, "COLORBAR"); break;
        case (FADETOGREY_COLORBAR): sprintf(name, "FADETOGREY_COLORBAR"); break;
        case (PN28):                sprintf(name, "PN28"); break;
        case (HORIZ_GREY_SCALE):    sprintf(name, "HORIZ_GREY_SCALE"); break;
        case (VERT_GREY_SCALE):     sprintf(name, "VERT_GREY_SCALE"); break;
        case (SOLIDBAR_YELLOW):     sprintf(name, "SOLIDBAR_YELLOW"); break;
        case (SOLIDBAR_CYAN):       sprintf(name, "SOLIDBAR_CYAN"); break;
        case (SOLIDBAR_MAGENTA):    sprintf(name, "SOLIDBAR_MAGENTA"); break;
        case (SOLIDBAR_BLACK):      sprintf(name, "SOLIDBAR_BLACK"); break;
        case (SOLIDBAR_WHITE):      sprintf(name, "SOLIDBAR_WHITE"); break;
        case (SOLIDBAR_RED):        sprintf(name, "SOLIDBAR_RED"); break;
        case (SOLIDBAR_GREEN):      sprintf(name, "SOLIDBAR_GREEN"); break;
        case (SOLIDBAR_BLUE):       sprintf(name, "SOLIDBAR_BLUE"); break;
        default:                    sprintf(name, "ERROR"); break;
    }
}


/* -----------------------------------------------------------------------
FUNCTION : ITE_testColourMatrix
PURPOSE  : Command to execute Colour matrix tests
------------------------------------------------------------------------ */
void
ITE_testColourMatrix(
enum e_grabFormat   grbformat)
{
    char            testComment[200];
    char            lrformatName[16];
    char            sensorName[16];
    char            resolutionName[16];
    char            pathname[200];
    e_resolution    sizelr = SUBQCIF;

    if (usecase.sensor == 0)
    {
        sprintf(sensorName, "Cam0");
    }
    else
    {
        sprintf(sensorName, "Cam1");
    }

    usecase.LR_GrbFormat = grbformat;
    ITE_GiveStringFormat("LR", lrformatName);
    LOS_Log("\n --> Stream on LR pipe with GRBFMT_%s format\n", lrformatName);

    ITE_GiveStringNameFromEnum(sizelr, resolutionName);
    LOS_Log("\n -----> %s resolution\n", resolutionName);
    usecase.LR_XSize = ITE_GiveXSizeFromEnum(PIPE_LR, sizelr, grbformat);
    usecase.LR_YSize = ITE_GiveYSizeFromEnum(PIPE_LR, sizelr, grbformat);
    usecase.LR_resolution = ITE_GiveResolution(resolutionName);
    sprintf(testComment, "%s_test_IP_ColourMatrix_tests_%s_%s",
            sensorName, lrformatName, resolutionName);

    /* Perform a "cd" to destination repository for test log */
    sprintf(pathname,
            IMAGING_PATH "/imaging/ite_nmf/test_results_nreg/IP_ColourMatrix_tests_%s",
            lrformatName);
    strcat(g_out_path, pathname);
    LOS_Log("Current path: >%s< \n", ( char * ) g_out_path);
    init_test_results_nreg(g_out_path);
    mmte_testStart(testComment, " test pipe LR (buffer + fr) YUV422 interleaved format", g_out_path);

    /* Strart Streaming */
    ITE_writePE(HostFrameConstraints_e_Flag_GuaranteeStaticFlickerFrameLength_Byte0,
                Flag_e_FALSE);

    /* Prepare buffers/LR pipe and Start streaming on LR pipe */
    ITE_StreamLR(&usecase);
    LOS_Log("\n1secs stream on LR pipe\n");

    ITE_HR_Stop();
    ITE_HR_Free();
    ITE_StreamHR(&usecase);
    LOS_Log("\n1secs stream on HR pipe\n");

    LOS_Sleep(500);

    ITE_NMF_Toggle_System_Coin();

    /* Program/write the page elements */
    ITE_ColourMatrix_Run();
    ITE_ColourMatrix();

    /* Stop Streaming LR */
    ITE_LR_Stop();
    ITE_LR_Free();

    /* Stop Streaming HR */
    //ITE_HR_Stop();
    //ITE_HR_Free();
}

void
ITE_ColourMatrix_Run(void)
{
    if (Flag_e_TRUE == ITE_IsPipe0Enabled())
    {
        ITE_SetupColourMatrix(0);
    }


    if (Flag_e_TRUE == ITE_IsPipe1Enabled())
    {
        ITE_SetupColourMatrix(1);
    }
}


/* -----------------------------------------------------------------------
FUNCTION : ITE_SetupColourMatrix
PURPOSE  : Setup the colour matrix
------------------------------------------------------------------------ */
void
ITE_SetupColourMatrix(
t_uint8 u8_Pipe)
{
    volatile float  temp;
    Coin_te         status_coin;
    // ColourMatrixMode = ColourMatrixMode_e_Manual;
    if (0 == u8_Pipe)
    {
        temp = ( float ) CE_ColourMatrix_PE_0_value.CE_CMFloatRedInRed / ( float ) 100;
        ITE_writePE(CE_ColourMatrixFloat_0_f_RedInRed_Byte0, *( volatile t_uint32 * ) &temp);

        temp = ( float ) CE_ColourMatrix_PE_0_value.CE_CMFloatGreenInRed / ( float ) 100;
        ITE_writePE(CE_ColourMatrixFloat_0_f_GreenInRed_Byte0, *( volatile t_uint32 * ) &temp);

        temp = ( float ) CE_ColourMatrix_PE_0_value.CE_CMFloatBlueInRed / ( float ) 100;
        ITE_writePE(CE_ColourMatrixFloat_0_f_BlueInRed_Byte0, *( volatile t_uint32 * ) &temp);

        //*******************************************************************************************
        temp = ( float ) CE_ColourMatrix_PE_0_value.CE_CMFloatRedInGreen / ( float ) 100;
        ITE_writePE(CE_ColourMatrixFloat_0_f_RedInGreen_Byte0, *( volatile t_uint32 * ) &temp);

        temp = ( float ) CE_ColourMatrix_PE_0_value.CE_CMFloatGreenInGreen / ( float ) 100;
        ITE_writePE(CE_ColourMatrixFloat_0_f_GreenInGreen_Byte0, *( volatile t_uint32 * ) &temp);

        temp = ( float ) CE_ColourMatrix_PE_0_value.CE_CMFloatBlueInGreen / ( float ) 100;
        ITE_writePE(CE_ColourMatrixFloat_0_f_BlueInGreen_Byte0, *( volatile t_uint32 * ) &temp);

        //********************************************************************************************
        temp = ( float ) CE_ColourMatrix_PE_0_value.CE_CMFloatRedInBlue / ( float ) 100;
        ITE_writePE(CE_ColourMatrixFloat_0_f_RedInBlue_Byte0, *( volatile t_uint32 * ) &temp);

        temp = ( float ) CE_ColourMatrix_PE_0_value.CE_CMFloatGreenInBlue / ( float ) 100;
        ITE_writePE(CE_ColourMatrixFloat_0_f_GreenInBlue_Byte0, *( volatile t_uint32 * ) &temp);

        temp = ( float ) CE_ColourMatrix_PE_0_value.CE_CMFloatBlueInBlue / ( float ) 100;
        ITE_writePE(CE_ColourMatrixFloat_0_f_BlueInBlue_Byte0, *( volatile t_uint32 * ) &temp);

        //********************************************************************************************
        ITE_writePE(CE_ColourMatrixCtrl_0_s16_Offset_R_Byte0, CE_ColourMatrix_PE_0_value.u16_OffsetRed);
        ITE_writePE(CE_ColourMatrixCtrl_0_s16_Offset_G_Byte0, CE_ColourMatrix_PE_0_value.u16_OffsetGreen);
        ITE_writePE(CE_ColourMatrixCtrl_0_s16_Offset_B_Byte0, CE_ColourMatrix_PE_0_value.u16_OffsetBlue);

        //ITE_writePE(CE_ColourMatrixCtrl_0_e_ColourMatrixMode_Byte0, ColourMatrixMode);
        LOS_Sleep(2000);
        status_coin = (Coin_te) ITE_readPE(SystemConfig_Status_e_Coin_Status_Byte0);
        ITE_writePE(SystemSetup_e_Coin_Ctrl_Byte0, !status_coin);
    }
    else    // for pipe 1
    {
        temp = ( float ) CE_ColourMatrix_PE_1_value.CE_CMFloatRedInRed / ( float ) 100;
        ITE_writePE(CE_ColourMatrixFloat_1_f_RedInRed_Byte0, *( volatile t_uint32 * ) &temp);

        temp = ( float ) CE_ColourMatrix_PE_1_value.CE_CMFloatGreenInRed / ( float ) 100;
        ITE_writePE(CE_ColourMatrixFloat_1_f_GreenInRed_Byte0, *( volatile t_uint32 * ) &temp);

        temp = ( float ) CE_ColourMatrix_PE_1_value.CE_CMFloatBlueInRed / ( float ) 100;
        ITE_writePE(CE_ColourMatrixFloat_1_f_BlueInRed_Byte0, *( volatile t_uint32 * ) &temp);

        //*******************************************************************************************
        temp = ( float ) CE_ColourMatrix_PE_1_value.CE_CMFloatRedInGreen / ( float ) 100;
        ITE_writePE(CE_ColourMatrixFloat_1_f_RedInGreen_Byte0, *( volatile t_uint32 * ) &temp);

        temp = ( float ) CE_ColourMatrix_PE_1_value.CE_CMFloatGreenInGreen / ( float ) 100;
        ITE_writePE(CE_ColourMatrixFloat_1_f_GreenInGreen_Byte0, *( volatile t_uint32 * ) &temp);

        temp = ( float ) CE_ColourMatrix_PE_1_value.CE_CMFloatBlueInGreen / ( float ) 100;
        ITE_writePE(CE_ColourMatrixFloat_1_f_BlueInGreen_Byte0, *( volatile t_uint32 * ) &temp);

        //********************************************************************************************
        temp = ( float ) CE_ColourMatrix_PE_1_value.CE_CMFloatRedInBlue / ( float ) 100;
        ITE_writePE(CE_ColourMatrixFloat_1_f_RedInBlue_Byte0, *( volatile t_uint32 * ) &temp);

        temp = ( float ) CE_ColourMatrix_PE_1_value.CE_CMFloatGreenInBlue / ( float ) 100;
        ITE_writePE(CE_ColourMatrixFloat_1_f_GreenInBlue_Byte0, *( volatile t_uint32 * ) &temp);

        temp = ( float ) CE_ColourMatrix_PE_1_value.CE_CMFloatBlueInBlue / ( float ) 100;
        ITE_writePE(CE_ColourMatrixFloat_1_f_BlueInBlue_Byte0, *( volatile t_uint32 * ) &temp);

        //********************************************************************************************
        ITE_writePE(CE_ColourMatrixCtrl_1_s16_Offset_R_Byte0, CE_ColourMatrix_PE_1_value.u16_OffsetRed);
        ITE_writePE(CE_ColourMatrixCtrl_1_s16_Offset_G_Byte0, CE_ColourMatrix_PE_1_value.u16_OffsetGreen);
        ITE_writePE(CE_ColourMatrixCtrl_1_s16_Offset_B_Byte0, CE_ColourMatrix_PE_1_value.u16_OffsetBlue);
        status_coin = (Coin_te) ITE_readPE(SystemConfig_Status_e_Coin_Status_Byte0);
        ITE_writePE(SystemSetup_e_Coin_Ctrl_Byte0, !status_coin);
    }


    LOS_Sleep(2000);
}


/* -----------------------------------------------------------------------
FUNCTION : ITE_IsPipe0Enabled
PURPOSE  : Check if pipe 0 is enabled or not
------------------------------------------------------------------------ */
t_bool
ITE_IsPipe0Enabled(void)
{
    return (ITE_readPE(DataPathStatus_e_Flag_Pipe0Enable_Byte0));
}


/* -----------------------------------------------------------------------
FUNCTION : ITE_IsPipe1Enabled
PURPOSE  : Check if pipe 1 is enabled or not
------------------------------------------------------------------------ */
t_bool
ITE_IsPipe1Enabled(void)
{
    return (ITE_readPE(DataPathStatus_e_Flag_Pipe1Enable_Byte0));
}


/* -----------------------------------------------------------------------
FUNCTION : ITE_ColourMatrix
PURPOSE  : Check if the written hw value's are actuallu taken into effect or not
------------------------------------------------------------------------ */
void
ITE_ColourMatrix(void)
{
    t_bool  b_Flag = Flag_e_TRUE;

    //Colour Matrix0 Test
    if (Flag_e_TRUE == ITE_IsPipe0Enabled())
    {
        ITE_NMF_Toggle_System_Coin();
        b_Flag = ITE_CM_ReadHW_Compare(0);
    }


    if (Flag_e_TRUE == b_Flag)
    {
        //test passed
        LOS_Log("\nTEST PASS: Colour Matrix values as expected for Pipe 0\n");
        mmte_testResult(TEST_PASSED);
    }
    else
    {
        //test failed
        LOS_Log("\nTEST FAIL: Colour Matrix values not as expected for Pipe 0\n");
        mmte_testResult(TEST_FAILED);
    }


    //Colour Matrix1 Test
    if (Flag_e_TRUE == ITE_IsPipe1Enabled())
    {
        ITE_NMF_Toggle_System_Coin();
        b_Flag = ITE_CM_ReadHW_Compare(1);
    }


    if (Flag_e_TRUE == b_Flag)
    {
        //test passed
        LOS_Log("\nTEST PASS: Colour Matrix values as expected for Pipe 1\n");
        mmte_testResult(TEST_PASSED);
    }
    else
    {
        //test failed
        LOS_Log("\nTEST FAIL: Colour Matrix values not as expected for Pipe 1\n");
        mmte_testResult(TEST_FAILED);
    }
}


/* -----------------------------------------------------------------------
FUNCTION : ITE_CM_ReadHW_Compare
PURPOSE  : Read the Hardware value programmed in to the device
------------------------------------------------------------------------ */
t_bool
ITE_CM_ReadHW_Compare(
t_bool  b_PipeNumber)
{
    ///////////////// for pipe 1  /////////////////////////////////////////////
    if (b_PipeNumber)
    {
        if ((((t_uint32) (((CE_ColourMatrix_PE_1_value.CE_CMFloatRedInRed) / ( float ) 100) * 1024)) & 0x3FFF) != IspRegRead_Value((ISP_CE1_MATRIX_RCOF00_OFFSET), ( long * ) &Read_Val, 1))
        {
            return (Flag_e_FALSE);
        }


        if ((((t_uint32) (((CE_ColourMatrix_PE_1_value.CE_CMFloatGreenInRed) / ( float ) 100) * 1024)) & 0x3FFF) != IspRegRead_Value((ISP_CE1_MATRIX_RCOF01_OFFSET), ( long * ) &Read_Val, 1))
        {
            return (Flag_e_FALSE);
        }


        if ((((t_uint32) (((CE_ColourMatrix_PE_1_value.CE_CMFloatBlueInRed) / ( float ) 100) * 1024)) & 0x3FFF) != IspRegRead_Value((ISP_CE1_MATRIX_RCOF02_OFFSET), ( long * ) &Read_Val, 1))
        {
            return (Flag_e_FALSE);
        }


        //***********************************************************************************************************
        if ((((t_uint32) (((CE_ColourMatrix_PE_1_value.CE_CMFloatRedInGreen) / ( float ) 100) * 1024)) & 0x3FFF) != IspRegRead_Value((ISP_CE1_MATRIX_RCOF10_OFFSET), ( long * ) &Read_Val, 1))
        {
            return (Flag_e_FALSE);
        }


        if ((((t_uint32) (((CE_ColourMatrix_PE_1_value.CE_CMFloatGreenInGreen) / ( float ) 100) * 1024)) & 0x3FFF) != IspRegRead_Value((ISP_CE1_MATRIX_RCOF11_OFFSET), ( long * ) &Read_Val, 1))
        {
            return (Flag_e_FALSE);
        }


        if ((((t_uint32) (((CE_ColourMatrix_PE_1_value.CE_CMFloatBlueInGreen) / ( float ) 100) * 1024)) & 0x3FFF) != IspRegRead_Value((ISP_CE1_MATRIX_RCOF12_OFFSET), ( long * ) &Read_Val, 1))
        {
            return (Flag_e_FALSE);
        }


        //*********************************************************************************************************
        if ((((t_uint32) (((CE_ColourMatrix_PE_1_value.CE_CMFloatRedInBlue) / ( float ) 100) * 1024)) & 0x3FFF) != IspRegRead_Value((ISP_CE1_MATRIX_RCOF20_OFFSET), ( long * ) &Read_Val, 1))
        {
            return (Flag_e_FALSE);
        }


        if ((((t_uint32) (((CE_ColourMatrix_PE_1_value.CE_CMFloatGreenInBlue) / ( float ) 100) * 1024)) & 0x3FFF) != IspRegRead_Value((ISP_CE1_MATRIX_RCOF21_OFFSET), ( long * ) &Read_Val, 1))
        {
            return (Flag_e_FALSE);
        }


        if ((((t_uint32) (((CE_ColourMatrix_PE_1_value.CE_CMFloatBlueInBlue) / ( float ) 100) * 1024)) & 0x3FFF) != IspRegRead_Value((ISP_CE1_MATRIX_RCOF22_OFFSET), ( long * ) &Read_Val, 1))
        {
            return (Flag_e_FALSE);
        }
    }


    ///////////////// for pipe 0  /////////////////////////////////////////////
    else
    {
        if ((((t_uint32) (((CE_ColourMatrix_PE_0_value.CE_CMFloatRedInRed) / ( float ) 100) * 1024)) & 0x3FFF) != IspRegRead_Value((ISP_CE0_MATRIX_RCOF00_OFFSET), ( long * ) &Read_Val, 1))
        {
            return (Flag_e_FALSE);
        }


        if ((((t_uint32) (((CE_ColourMatrix_PE_0_value.CE_CMFloatGreenInRed) / ( float ) 100) * 1024)) & 0x3FFF) != IspRegRead_Value((ISP_CE0_MATRIX_RCOF01_OFFSET), ( long * ) &Read_Val, 1))
        {
            return (Flag_e_FALSE);
        }


        if ((((t_uint32) (((CE_ColourMatrix_PE_0_value.CE_CMFloatBlueInRed) / ( float ) 100) * 1024)) & 0x3FFF) != IspRegRead_Value((ISP_CE0_MATRIX_RCOF02_OFFSET), ( long * ) &Read_Val, 1))
        {
            return (Flag_e_FALSE);
        }


        //***********************************************************************************************************
        if ((((t_uint32) (((CE_ColourMatrix_PE_0_value.CE_CMFloatRedInGreen) / ( float ) 100) * 1024)) & 0x3FFF) != IspRegRead_Value((ISP_CE0_MATRIX_RCOF10_OFFSET), ( long * ) &Read_Val, 1))
        {
            return (Flag_e_FALSE);
        }


        if ((((t_uint32) (((CE_ColourMatrix_PE_0_value.CE_CMFloatGreenInGreen) / ( float ) 100) * 1024)) & 0x3FFF) != IspRegRead_Value((ISP_CE0_MATRIX_RCOF11_OFFSET), ( long * ) &Read_Val, 1))
        {
            return (Flag_e_FALSE);
        }


        if ((((t_uint32) (((CE_ColourMatrix_PE_0_value.CE_CMFloatBlueInGreen) / ( float ) 100) * 1024)) & 0x3FFF) != IspRegRead_Value((ISP_CE0_MATRIX_RCOF12_OFFSET), ( long * ) &Read_Val, 1))
        {
            return (Flag_e_FALSE);
        }


        //*********************************************************************************************************
        if ((((t_uint32) (((CE_ColourMatrix_PE_0_value.CE_CMFloatRedInBlue) / ( float ) 100) * 1024)) & 0x3FFF) != IspRegRead_Value((ISP_CE0_MATRIX_RCOF20_OFFSET), ( long * ) &Read_Val, 1))
        {
            return (Flag_e_FALSE);
        }


        if ((((t_uint32) (((CE_ColourMatrix_PE_0_value.CE_CMFloatGreenInBlue) / ( float ) 100) * 1024)) & 0x3FFF) != IspRegRead_Value((ISP_CE0_MATRIX_RCOF21_OFFSET), ( long * ) &Read_Val, 1))
        {
            return (Flag_e_FALSE);
        }


        if ((((t_uint32) (((CE_ColourMatrix_PE_0_value.CE_CMFloatBlueInBlue) / ( float ) 100) * 1024)) & 0x3FFF) != IspRegRead_Value((ISP_CE0_MATRIX_RCOF22_OFFSET), ( long * ) &Read_Val, 1))
        {
            return (Flag_e_FALSE);
        }
    }


    return (Flag_e_TRUE);
}


/* -----------------------------------------------------------------------
FUNCTION : ITE_testRSOManual
PURPOSE  : Test RSO manual module
------------------------------------------------------------------------ */
void
ITE_testRSOManual(
enum e_grabFormat   grbformat)
{
    char            testComment[200];
    char            lrformatName[16];
    char            sensorName[16];
    char            resolutionName[16];
    char            pathname[200];
    e_resolution    sizelr = SUBQCIF;

    if (usecase.sensor == 0)
    {
        sprintf(sensorName, "Cam0");
    }
    else
    {
        sprintf(sensorName, "Cam1");
    }

    usecase.LR_GrbFormat = grbformat;
    ITE_GiveStringFormat("LR", lrformatName);
    LOS_Log("\n --> Stream on LR pipe with GRBFMT_%s format\n", lrformatName);

    ITE_GiveStringNameFromEnum(sizelr, resolutionName);
    LOS_Log("\n -----> %s resolution\n", resolutionName);
    usecase.LR_XSize = ITE_GiveXSizeFromEnum(PIPE_LR, sizelr, grbformat);
    usecase.LR_YSize = ITE_GiveYSizeFromEnum(PIPE_LR, sizelr, grbformat);
    usecase.LR_resolution = ITE_GiveResolution(resolutionName);
    sprintf(testComment, "%s_test_IP_RSO manual_tests_%s_%s",
            sensorName, lrformatName, resolutionName);

    /* Perform a "cd" to destination repository for test log */
    sprintf(pathname,
            IMAGING_PATH "/ite_nmf/test_results_nreg/IP_RSO_manual_tests_%s",
            lrformatName);
    strcat(g_out_path, pathname);
    LOS_Log("Current path: >%s< \n", ( char * ) g_out_path);
    init_test_results_nreg(g_out_path);
    mmte_testStart(testComment, " test pipe LR (buffer + fr) YUV422 interleaved format",
                   g_out_path);

    ITE_testRSOManual_StreamLR(&usecase);

    mmte_testEnd();

    /* Clear g_out_path */
    memset(g_out_path, 0, KlogDirectoryLentgh * sizeof(char));
}

/* -----------------------------------------------------------------------
FUNCTION : ITE_testRSOManual_StreamLR
PURPOSE  : Test RSO manual module on LR
------------------------------------------------------------------------ */
void
ITE_testRSOManual_StreamLR(
tps_sia_usecase p_sia_usecase)
{
    t_uint32    e_Flag_EnableRSO = Flag_e_TRUE,
                e_RSO_Mode_Control = Flag_e_FALSE;
    t_uint32    u32_XCoefGr_default = 20,
                u32_YCoefGr_default = 21,
                u32_XCoefR_default = 22,
                u32_YCoefR_default = 23,
                u32_XCoefGb_default = 26;
    t_uint32    u32_YCoefGb_default = 27,
                u32_XCoefB_default = 24,
                u32_YCoefB_default = 25,
                u16_DcTermGr_default = 28,
                u16_DcTermR_default = 29;
    t_uint32    u16_DcTermB_default = 30,
                u16_DcTermGb_default = 31,
                u16_XSlantOrigin_default = 32,
                u16_YSlantOrigin_default = 33;

    ITE_writePE(HostFrameConstraints_e_Flag_GuaranteeStaticFlickerFrameLength_Byte0, Flag_e_FALSE);

    // prepare buffers/LR pipe and Start streaming on LR pipe
    ITE_StreamLR(p_sia_usecase);

    LOS_Log("\n1secs stream on LR pipe\n");
    LOS_Sleep(500);

    // Set RSOManual default Control page elements
    ITE_NMF_Toggle_System_Coin();
    ITE_setRSOManualDefault_Ctrl_PE(e_Flag_EnableRSO, e_RSO_Mode_Control);
    ITE_NMF_Toggle_System_Coin();
    ITE_setRSOManualDefault_Color_Ctrl_PE(
    u32_XCoefGr_default,
    u32_YCoefGr_default,
    u32_XCoefR_default,
    u32_YCoefR_default,
    u32_XCoefGb_default,
    u32_YCoefGb_default,
    u32_XCoefB_default,
    u32_YCoefB_default);
    ITE_NMF_Toggle_System_Coin();
    ITE_setRSOManualDefault_DcTerm_Ctrl_PE(
    u16_DcTermGr_default,
    u16_DcTermR_default,
    u16_DcTermB_default,
    u16_DcTermGb_default,
    u16_XSlantOrigin_default,
    u16_YSlantOrigin_default);

    // check RSOManual Control page elements
    ITE_NMF_Toggle_System_Coin();
    ITE_testRSOManualDefault_Ctrl_PE(e_Flag_EnableRSO);
    ITE_NMF_Toggle_System_Coin();
    ITE_testRSOManualDefault_Color_Ctrl_PE(
    u32_XCoefGr_default,
    u32_YCoefGr_default,
    u32_XCoefR_default,
    u32_YCoefR_default,
    u32_XCoefGb_default,
    u32_YCoefGb_default,
    u32_XCoefB_default,
    u32_YCoefB_default);
    ITE_NMF_Toggle_System_Coin();
    ITE_testRSOManualDefault_DcTerm_Ctrl_PE(
    u16_DcTermGr_default,
    u16_DcTermR_default,
    u16_DcTermB_default,
    u16_DcTermGb_default,
    u16_XSlantOrigin_default,
    u16_YSlantOrigin_default);

    ITE_LR_Stop();
    ITE_LR_Free();
}


/* -----------------------------------------------------------------------
FUNCTION : ITE_setRSOManualDefault_Ctrl_PE
PURPOSE  : Set Default RSOManual Control page elements
------------------------------------------------------------------------ */
void
ITE_setRSOManualDefault_Ctrl_PE(
t_uint32    e_Flag_EnableRSO,
t_uint32    e_RSO_Mode_Control)
{
    ITE_writePE(RSO_Control_e_Flag_EnableRSO_Byte0, e_Flag_EnableRSO);
    ITE_writePE(RSO_Control_e_RSO_Mode_Control_Byte0, e_RSO_Mode_Control);
    LOS_Sleep(2000);
}


/* -----------------------------------------------------------------------
FUNCTION : ITE_testRSOManualDefault_Color_Ctrl_PE
PURPOSE  : Set Default RSOManual Color Control page elements
------------------------------------------------------------------------ */
void
ITE_setRSOManualDefault_Color_Ctrl_PE(
t_uint32    u32_XCoefGr_default,
t_uint32    u32_YCoefGr_default,
t_uint32    u32_XCoefR_default,
t_uint32    u32_YCoefR_default,
t_uint32    u32_XCoefGb_default,
t_uint32    u32_YCoefGb_default,
t_uint32    u32_XCoefB_default,
t_uint32    u32_YCoefB_default)
{
    ITE_writePE(RSO_DataCtrl_u32_XCoefGr_Byte0, u32_XCoefGr_default);
    ITE_writePE(RSO_DataCtrl_u32_YCoefGr_Byte0, u32_YCoefGr_default);
    ITE_writePE(RSO_DataCtrl_u32_XCoefR_Byte0, u32_XCoefR_default);
    ITE_writePE(RSO_DataCtrl_u32_YCoefR_Byte0, u32_YCoefR_default);
    ITE_writePE(RSO_DataCtrl_u32_XCoefGb_Byte0, u32_XCoefGb_default);
    ITE_writePE(RSO_DataCtrl_u32_YCoefGb_Byte0, u32_YCoefGb_default);
    ITE_writePE(RSO_DataCtrl_u32_XCoefB_Byte0, u32_XCoefB_default);
    ITE_writePE(RSO_DataCtrl_u32_YCoefB_Byte0, u32_YCoefB_default);
    LOS_Sleep(1000);
}


/* -----------------------------------------------------------------------
FUNCTION : ITE_testRSOManualDefault_DcTerm_Ctrl_PE
PURPOSE  : Set Default RSOManual DcTerm Control page elements
------------------------------------------------------------------------ */
void
ITE_setRSOManualDefault_DcTerm_Ctrl_PE(
t_uint32    u16_DcTermGr_default,
t_uint32    u16_DcTermR_default,
t_uint32    u16_DcTermB_default,
t_uint32    u16_DcTermGb_default,
t_uint32    u16_XSlantOrigin_default,
t_uint32    u16_YSlantOrigin_default)
{
    ITE_writePE(RSO_DataCtrl_u16_DcTermGr_Byte0, u16_DcTermGr_default);
    ITE_writePE(RSO_DataCtrl_u16_DcTermR_Byte0, u16_DcTermR_default);
    ITE_writePE(RSO_DataCtrl_u16_DcTermB_Byte0, u16_DcTermB_default);
    ITE_writePE(RSO_DataCtrl_u16_DcTermGb_Byte0, u16_DcTermGb_default);
    ITE_writePE(RSO_DataCtrl_u16_XSlantOrigin_Byte0, u16_XSlantOrigin_default);
    ITE_writePE(RSO_DataCtrl_u16_YSlantOrigin_Byte0, u16_YSlantOrigin_default);
    LOS_Sleep(1000);
}


/* -----------------------------------------------------------------------
FUNCTION : ITE_testRSOManualDefault_Ctrl_PE
PURPOSE  : check RSOManual Control status page elements and decide if tests pass or fail
----------------------------------------------------------------------------------- */
void
ITE_testRSOManualDefault_Ctrl_PE(
t_uint32    e_Flag_EnableRSO)
{
    char    mess[256];

    snprintf(mess, sizeof(mess), "Test %d :check for RSOManual Control status ", ( int ) testCount);
    mmte_testNext(mess);

    if ((e_Flag_EnableRSO == IspRegRead_Value((ISP_RSO_SLANT_CTRL_OFFSET), ( long * ) &Read_Val, 1)))
    {
        //test passed
        LOS_Log("\nTEST PASS: RSOManual Control values as expected\n");
        mmte_testResult(TEST_PASSED);
    }
    else
    {
        //test failed
        LOS_Log("\nTEST FAIL: RSOManual Control values NOT as expected\n");
        mmte_testResult(TEST_FAILED);
    }


    testCount++;
}


/* -----------------------------------------------------------------------
FUNCTION : ITE_testRSOManualDefault_Color_Ctrl_PE
PURPOSE  : check RSOManualDefault_Color Control status page elements and decide if tests pass or fail
----------------------------------------------------------------------------------- */
void
ITE_testRSOManualDefault_Color_Ctrl_PE(
t_uint32    u32_XCoefGr_default,
t_uint32    u32_YCoefGr_default,
t_uint32    u32_XCoefR_default,
t_uint32    u32_YCoefR_default,
t_uint32    u32_XCoefGb_default,
t_uint32    u32_YCoefGb_default,
t_uint32    u32_XCoefB_default,
t_uint32    u32_YCoefB_default)
{
    char    mess[256];

    snprintf(mess, sizeof(mess), "Test %d :check for RSOManualDefault_Color Control status ", ( int ) testCount);
    mmte_testNext(mess);

    if
    (
        (u32_XCoefGr_default == IspRegRead_Value((ISP_RSO_X_COEF_GR_OFFSET), ( long * ) &Read_Val, 1))
    &&  (u32_YCoefGr_default == IspRegRead_Value((ISP_RSO_Y_COEF_GR_OFFSET), ( long * ) &Read_Val, 1))
    &&  (u32_XCoefR_default == IspRegRead_Value((ISP_RSO_X_COEF_RR_OFFSET), ( long * ) &Read_Val, 1))
    &&  (u32_YCoefR_default == IspRegRead_Value((ISP_RSO_Y_COEF_RR_OFFSET), ( long * ) &Read_Val, 1))
    &&  (u32_XCoefGb_default == IspRegRead_Value((ISP_RSO_X_COEF_GB_OFFSET), ( long * ) &Read_Val, 1))
    &&  (u32_YCoefGb_default == IspRegRead_Value((ISP_RSO_Y_COEF_GB_OFFSET), ( long * ) &Read_Val, 1))
    &&  (u32_XCoefB_default == IspRegRead_Value((ISP_RSO_X_COEF_BB_OFFSET), ( long * ) &Read_Val, 1))
    &&  (u32_YCoefB_default == IspRegRead_Value((ISP_RSO_Y_COEF_BB_OFFSET), ( long * ) &Read_Val, 1))
    )
    {
        //test passed
        LOS_Log("\nTEST PASS: RSOManualDefault_Color Control values as expected\n");
        mmte_testResult(TEST_PASSED);
    }
    else
    {
        //test failed
        LOS_Log("\nTEST FAIL: RSOManualDefault_Color Control values NOT as expected\n");
        mmte_testResult(TEST_FAILED);
    }


    testCount++;
}


/* -----------------------------------------------------------------------
FUNCTION : ITE_testRSOManualDefault_DcTerm_Ctrl_PE
PURPOSE  : check RSOManualDefault_DcTerm Control status page elements and decide if tests pass or fail
----------------------------------------------------------------------------------- */
void
ITE_testRSOManualDefault_DcTerm_Ctrl_PE(
t_uint32    u16_DcTermGr_default,
t_uint32    u16_DcTermR_default,
t_uint32    u16_DcTermB_default,
t_uint32    u16_DcTermGb_default,
t_uint32    u16_XSlantOrigin_default,
t_uint32    u16_YSlantOrigin_default)
{
    char    mess[256];

    snprintf(mess, sizeof(mess), "Test %d :check for RSOManualDefault_DcTerm Control status ", ( int ) testCount);
    mmte_testNext(mess);

    if
    (
        (u16_DcTermGr_default == IspRegRead_Value((ISP_RSO_DC_TERM_GR_OFFSET), ( long * ) &Read_Val, 1))
    &&  (u16_DcTermR_default == IspRegRead_Value((ISP_RSO_DC_TERM_RR_OFFSET), ( long * ) &Read_Val, 1))
    &&  (u16_DcTermB_default == IspRegRead_Value((ISP_RSO_DC_TERM_BB_OFFSET), ( long * ) &Read_Val, 1))
    &&  (u16_DcTermGb_default == IspRegRead_Value((ISP_RSO_DC_TERM_GB_OFFSET), ( long * ) &Read_Val, 1))
    &&  (u16_XSlantOrigin_default == IspRegRead_Value((ISP_RSO_X_SLANT_ORIGIN_OFFSET), ( long * ) &Read_Val, 1))
    &&  (u16_YSlantOrigin_default == IspRegRead_Value((ISP_RSO_Y_SLANT_ORIGIN_OFFSET), ( long * ) &Read_Val, 1))
    )
    {
        //test passed
        LOS_Log("\nTEST PASS: RSOManualDefault_DcTerm Control values as expected\n");
        mmte_testResult(TEST_PASSED);
    }
    else
    {
        //test failed
        LOS_Log("\nTEST FAIL: RSOManualDefault_DcTerm Control values NOT as expected\n");
        mmte_testResult(TEST_FAILED);
    }


    testCount++;
}


/* -----------------------------------------------------------------------
FUNCTION : ITE_testScorpio
PURPOSE  : Test Scorpio module
------------------------------------------------------------------------ */
void
ITE_testScorpio(
enum e_grabFormat   grbformat)
{
    char            testComment[200];
    char            lrformatName[16];
    char            sensorName[16];
    char            resolutionName[16];
    char            pathname[200];
    e_resolution    sizelr = SUBQCIF;

    if (usecase.sensor == 0)
    {
        sprintf(sensorName, "Cam0");
    }
    else
    {
        sprintf(sensorName, "Cam1");
    }

    usecase.LR_GrbFormat = grbformat;
    ITE_GiveStringFormat("LR", lrformatName);
    LOS_Log("\n --> Stream on LR pipe with GRBFMT_%s format\n", lrformatName);

    ITE_GiveStringNameFromEnum(sizelr, resolutionName);
    LOS_Log("\n -----> %s resolution\n", resolutionName);
    usecase.LR_XSize = ITE_GiveXSizeFromEnum(PIPE_LR, sizelr, grbformat);
    usecase.LR_YSize = ITE_GiveYSizeFromEnum(PIPE_LR, sizelr, grbformat);
    usecase.LR_resolution = ITE_GiveResolution(resolutionName);
    sprintf(testComment, "%s_test_IP_Scorpio_tests_%s_%s",
            sensorName, lrformatName, resolutionName);

    /* Perform a "cd" to destination repository for test log */
    sprintf(pathname,
            IMAGING_PATH "/ite_nmf/test_results_nreg/IP_Scorpio_tests_%s",
            lrformatName);
    strcat(g_out_path, pathname);
    LOS_Log("Current path: >%s< \n", ( char * ) g_out_path);
    init_test_results_nreg(g_out_path);
    mmte_testStart(testComment,
                   "test pipe LR (buffer + fr) YUV422 interleaved format",
                   g_out_path);

    ITE_testScorpio_StreamLR(&usecase);

    mmte_testEnd();

    /* Clear g_out_path */
    memset(g_out_path, 0, KlogDirectoryLentgh * sizeof(char));
}

/* -----------------------------------------------------------------------
FUNCTION : ITE_testScorpio_StreamLR
PURPOSE  : Test Scorpio module on LR
------------------------------------------------------------------------ */
void
ITE_testScorpio_StreamLR(
tps_sia_usecase p_sia_usecase)
{
    t_uint32    e_Flag_ScorpioEnable_default = Flag_e_TRUE,
                e_ScorpioMode_default = ScorpioMode_e_Manual,
                e_Flag_u8_CoringLevel_Ctrl_default = 11,
                e_Flag_u8_CoringLevel_Status = 11;
    ITE_writePE(HostFrameConstraints_e_Flag_GuaranteeStaticFlickerFrameLength_Byte0, Flag_e_FALSE);

    // prepare buffers/LR pipe and Start streaming on LR pipe
    ITE_StreamLR(p_sia_usecase);

    LOS_Log("\n1secs stream on LR pipe\n");
    LOS_Sleep(500);

    ITE_NMF_Toggle_System_Coin();

    // Set Scorpio default Control page elements
    ITE_setScorpioDefault_Ctrl_PE(
    e_Flag_ScorpioEnable_default,
    e_ScorpioMode_default,
    e_Flag_u8_CoringLevel_Ctrl_default,
    e_Flag_u8_CoringLevel_Status);

    ITE_NMF_Toggle_System_Coin();

    // check Scorpio Control page elements
    ITE_testScorpioDefault_Ctrl_PE(
    e_Flag_ScorpioEnable_default,
    e_ScorpioMode_default,
    e_Flag_u8_CoringLevel_Ctrl_default,
    e_Flag_u8_CoringLevel_Status);

    ITE_LR_Stop();
    ITE_LR_Free();
}


/* -----------------------------------------------------------------------
FUNCTION : ITE_setScorpioDefault_Ctrl_PE
PURPOSE  : Set Default Babylon Control page elements
------------------------------------------------------------------------ */
void
ITE_setScorpioDefault_Ctrl_PE(
t_uint32    e_Flag_ScorpioEnable_default,
t_uint32    e_ScorpioMode_default,
t_uint32    e_Flag_u8_CoringLevel_Ctrl_default,
t_uint32    e_Flag_u8_CoringLevel_Status)
{
    ITE_writePE(Scorpio_Ctrl_e_Flag_ScorpioEnable_Byte0, e_Flag_ScorpioEnable_default);
    ITE_writePE(Scorpio_Ctrl_e_ScorpioMode_Byte0, e_ScorpioMode_default);
    ITE_writePE(Scorpio_Ctrl_u8_CoringLevel_Ctrl_Byte0, e_Flag_u8_CoringLevel_Ctrl_default);
    ITE_writePE(Scorpio_Ctrl_u8_CoringLevel_Status_Byte0, e_Flag_u8_CoringLevel_Status);
    LOS_Sleep(2000);
}


/* -----------------------------------------------------------------------
FUNCTION : ITE_testScorpioDefault_Ctrl_PE
PURPOSE  : check Scorpio Control status page elements and decide if tests pass or fail
----------------------------------------------------------------------------------- */
void
ITE_testScorpioDefault_Ctrl_PE(
t_uint32    e_Flag_ScorpioEnable_default,
t_uint32    e_ScorpioMode_default,
t_uint32    e_Flag_u8_CoringLevel_Ctrl_default,
t_uint32    e_Flag_u8_CoringLevel_Status)
{
    char    mess[256];

    snprintf(mess, sizeof(mess), "Test %d :check for Scorpio Control status ", ( int ) testCount);
    mmte_testNext(mess);

    UNUSED(e_Flag_ScorpioEnable_default);
    UNUSED(e_ScorpioMode_default);

    // there is a difference between what we write to the page elements and what we observe in the corresponding HW register
    // Value '3' is used to compare for register ISP_SCORPIO_ENABLE in accordace with Pictor Functional Specification Page 292 (24.18)
    if
    (
        (3 == IspRegRead_Value((ISP_SCORPIO_ENABLE_OFFSET), ( long * ) &Read_Val, 1))
    &&  (
            e_Flag_u8_CoringLevel_Ctrl_default == IspRegRead_Value(
                (ISP_SCORPIO_CORING_LVL_REQ_OFFSET),
                ( long * ) &Read_Val,
                1)
        )
    &&  (
            e_Flag_u8_CoringLevel_Status == IspRegRead_Value(
                (ISP_SCORPIO_CORING_LVL_STATUS_OFFSET),
                ( long * ) &Read_Val,
                1)
        )
    )
    {
        //test passed
        LOS_Log("\nTEST PASS: Scorpio Control values as expected\n");
        mmte_testResult(TEST_PASSED);
    }
    else
    {
        //test failed
        LOS_Log("\nTEST FAIL: Scorpio Control values NOT as expected\n");
        mmte_testResult(TEST_FAILED);
    }


    testCount++;
}


/* -----------------------------------------------------------------------
FUNCTION : ITE_testBabylonManual
PURPOSE  : Test Babylon module
------------------------------------------------------------------------ */
void
ITE_testBabylonManual(
enum e_grabFormat   grbformat)
{
    char            testComment[200];
    char            lrformatName[16];
    char            sensorName[16];
    char            resolutionName[16];
    char            pathname[200];
    e_resolution    sizelr = SUBQCIF;

    if (usecase.sensor == 0)
    {
        sprintf(sensorName, "Cam0");
    }
    else
    {
        sprintf(sensorName, "Cam1");
    }

    usecase.LR_GrbFormat = grbformat;
    ITE_GiveStringFormat("LR", lrformatName);
    LOS_Log("\n --> Stream on LR pipe with GRBFMT_%s format\n", lrformatName);

    ITE_GiveStringNameFromEnum(sizelr, resolutionName);
    LOS_Log("\n -----> %s resolution\n", resolutionName);
    usecase.LR_XSize = ITE_GiveXSizeFromEnum(PIPE_LR, sizelr, grbformat);
    usecase.LR_YSize = ITE_GiveYSizeFromEnum(PIPE_LR, sizelr, grbformat);
    usecase.LR_resolution = ITE_GiveResolution(resolutionName);
    sprintf(testComment, "%s_test_IP_Babylon_tests_%s_%s",
            sensorName, lrformatName, resolutionName);

    /* Perform a "cd" to destination repository for test log */
    sprintf(pathname,
            IMAGING_PATH "/ite_nmf/test_results_nreg/IP_Babylon_tests_%s",
            lrformatName);
    strcat(g_out_path, pathname);
    LOS_Log("Current path: >%s< \n", ( char * ) g_out_path);
    init_test_results_nreg(g_out_path);
    mmte_testStart(testComment,
                   "test pipe LR (buffer + fr) YUV422 interleaved format",
                   g_out_path);

    ITE_testBabylon_StreamLR(&usecase);

    mmte_testEnd();

    /* Clear  g_out_path */
    memset(g_out_path, 0, KlogDirectoryLentgh * sizeof(char));
}

/* -----------------------------------------------------------------------
FUNCTION : ITE_testBabylon_StreamLR
PURPOSE  : Test Babylon manual on LR
------------------------------------------------------------------------ */
void
ITE_testBabylon_StreamLR(
tps_sia_usecase p_sia_usecase)
{
    t_uint32    e_Flag_BabylonEnable_default = Flag_e_TRUE,
                e_Flag_u8_ZipperKill_default = 5,
                e_Flag_u8_Flat_Threshold_default = 11;

    //t_uint32 e_Flag_Flat_Threshold_Status_Byte =Flag_e_FALSE, e_Flag_BabylonMode_Byte =Flag_e_FALSE;  //not used currently
    // prepare buffers/LR pipe and Start streaming on LR pipe
    ITE_StreamLR(p_sia_usecase);

    LOS_Log("\n1secs stream on LR pipe\n");
    LOS_Sleep(500);

    ITE_NMF_Toggle_System_Coin();

    // Set Babylon default Control page elements
    ITE_setBabylonDefault_Ctrl_PE(
    e_Flag_BabylonEnable_default,
    e_Flag_u8_ZipperKill_default,
    e_Flag_u8_Flat_Threshold_default);
    ITE_NMF_Toggle_System_Coin();

    // check Babylon Control page elements
    ITE_testBabylonDefault_Ctrl_PE(
    e_Flag_BabylonEnable_default,
    e_Flag_u8_ZipperKill_default,
    e_Flag_u8_Flat_Threshold_default);

    ITE_LR_Stop();
    ITE_LR_Free();
}


/* -----------------------------------------------------------------------
FUNCTION : ITE_setBabylonDefault_Ctrl_PE
PURPOSE  : Set Default Babylon Control page elements
------------------------------------------------------------------------ */
void
ITE_setBabylonDefault_Ctrl_PE(
t_uint32    e_Flag_BabylonEnable_default,
t_uint32    e_Flag_u8_ZipperKill_default,
t_uint32    e_Flag_u8_Flat_Threshold_default)
{
    ITE_writePE(Babylon_Ctrl_e_Flag_BabylonEnable_Byte0, e_Flag_BabylonEnable_default);
    ITE_writePE(Babylon_Ctrl_u8_ZipperKill_Byte0, e_Flag_u8_ZipperKill_default);
    ITE_writePE(Babylon_Ctrl_u8_Flat_Threshold_Byte0, e_Flag_u8_Flat_Threshold_default);
    LOS_Sleep(2000);
}


/* -----------------------------------------------------------------------
FUNCTION : ITE_testBabylonDefault_Ctrl_PE
PURPOSE  : check Babylon Control status page elements and decide if tests pass or fail
----------------------------------------------------------------------------------- */
void
ITE_testBabylonDefault_Ctrl_PE(
t_uint32    e_Flag_BabylonEnable_default,
t_uint32    e_Flag_u8_ZipperKill_default,
t_uint32    e_Flag_u8_Flat_Threshold_default)
{
    char    mess[256];

    //t_uint32 temp, temp1, temp2;
    snprintf(mess, sizeof(mess), "Test %d :check for Babylon Control status ", ( int ) testCount);
    mmte_testNext(mess);

    //LOS_Log(" VALUE %i ,%i,%i",e_Flag_BabylonEnable_default,e_Flag_u8_ZipperKill_default,e_Flag_u8_Flat_Threshold_default);

    /*temp=IspRegRead_Value(ISP_BABYLON_ENABLE);
        temp1=IspRegRead_Value(ISP_BABYLON_ZIPPERKILL);
        temp2=IspRegRead_Value(ISP_BABYLON_FLAT_TH);*/

    //LOS_Log(" VALUE %i ,%i,%i",temp, temp1, temp2);
    if
    (
        (e_Flag_BabylonEnable_default == IspRegRead_Value((ISP_BABYLON_ENABLE_OFFSET), ( long * ) &Read_Val, 1))
    &&  (e_Flag_u8_ZipperKill_default == IspRegRead_Value((ISP_BABYLON_ZIPPERKILL_OFFSET), ( long * ) &Read_Val, 1))
    &&  (e_Flag_u8_Flat_Threshold_default == IspRegRead_Value((ISP_BABYLON_FLAT_TH_OFFSET), ( long * ) &Read_Val, 1))
    )
    {
        //test passed
        LOS_Log("\nTEST PASS: Babylon Control values as expected\n");
        mmte_testResult(TEST_PASSED);
    }
    else
    {
        //test failed
        LOS_Log("\nTEST FAIL: Babylon Control values NOT as expected\n");
        mmte_testResult(TEST_FAILED);
    }


    testCount++;
}


/* -----------------------------------------------------------------------
FUNCTION : ITE_testPixelOrder
PURPOSE  : Test PixelOrder
------------------------------------------------------------------------ */
void
ITE_testPixelOrder(
enum e_grabFormat   grbformat)
{
    char            testComment[200];
    char            lrformatName[16];
    char            sensorName[16];
    char            resolutionName[16];
    char            pathname[200];
    e_resolution    sizelr = SUBQCIF;

    if (usecase.sensor == 0)
    {
        sprintf(sensorName, "Cam0");
    }
    else
    {
        sprintf(sensorName, "Cam1");
    }

    usecase.LR_GrbFormat = grbformat;
    ITE_GiveStringFormat("LR", lrformatName);
    LOS_Log("\n --> Stream on LR pipe with GRBFMT_%s format\n", lrformatName);

    ITE_GiveStringNameFromEnum(sizelr, resolutionName);
    LOS_Log("\n -----> %s resolution\n", resolutionName);
    usecase.LR_XSize = ITE_GiveXSizeFromEnum(PIPE_LR, sizelr, grbformat);
    usecase.LR_YSize = ITE_GiveYSizeFromEnum(PIPE_LR, sizelr, grbformat);
    usecase.LR_resolution = ITE_GiveResolution(resolutionName);
    sprintf(testComment, "%s_test_IP_PixelOrder_tests_%s_%s",
            sensorName, lrformatName, resolutionName);

    /* Perform a "cd" to destination repository for test log */
    sprintf(pathname,
            IMAGING_PATH "/ite_nmf/test_results_nreg/IP_PixelOrder_tests_%s",
            lrformatName);
    strcat(g_out_path, pathname);
    LOS_Log("Current path: >%s< \n", ( char * ) g_out_path);
    init_test_results_nreg(g_out_path);
    mmte_testStart(testComment,
                   "test pipe LR (buffer + fr) YUV422 interleaved format",
                   g_out_path);

    ITE_testPixelOrder_StreamLR(&usecase);

    mmte_testEnd();

    /* Clear  g_out_path */
    memset(g_out_path, 0, KlogDirectoryLentgh * sizeof(char));
}

/* -----------------------------------------------------------------------
FUNCTION : ITE_testPixelOrder_StreamLR
PURPOSE  : Test PixelOrder on LR
------------------------------------------------------------------------ */
void
ITE_testPixelOrder_StreamLR(
tps_sia_usecase p_sia_usecase)
{
    t_uint32    PixelOrderValue;

    // prepare buffers/LR pipe and Start streaming on LR pipe
    ITE_StreamLR(p_sia_usecase);

    LOS_Log("\n1secs stream on LR pipe\n");
    LOS_Sleep(500);

    PixelOrderValue = ITE_readPE(SystemConfig_Status_e_PixelOrder_Byte0);
    LOS_Log("\n --> Pixel Order Value is %d\n", PixelOrderValue);

    ITE_LR_Stop();
    ITE_LR_Free();
}

/* -----------------------------------------------------------------------
FUNCTION : ITE_testSFX
PURPOSE  : Test SFX on LR
------------------------------------------------------------------------ */
void
ITE_testSFX(
enum e_grabFormat   grbformat)
{
    char            testComment[200];
    char            lrformatName[16];
    char            sensorName[16];
    char            resolutionName[16];
    char            pathname[200];
    e_resolution    sizelr = SUBQCIF;

    if (usecase.sensor == 0)
    {
        sprintf(sensorName, "Cam0");
    }
    else
    {
        sprintf(sensorName, "Cam1");
    }

    usecase.LR_GrbFormat = grbformat;
    ITE_GiveStringFormat("LR", lrformatName);
    LOS_Log("\n --> Stream on LR pipe with GRBFMT_%s format\n", lrformatName);

    ITE_GiveStringNameFromEnum(sizelr, resolutionName);
    LOS_Log("\n -----> %s resolution\n", resolutionName);
    usecase.LR_XSize = ITE_GiveXSizeFromEnum(PIPE_LR, sizelr, grbformat);
    usecase.LR_YSize = ITE_GiveYSizeFromEnum(PIPE_LR, sizelr, grbformat);
    usecase.LR_resolution = ITE_GiveResolution(resolutionName);
    sprintf(testComment, "%s_test_IP_SFX_tests_%s_%s",
            sensorName, lrformatName, resolutionName);

    /* Perform a "cd" to destination repository for test log */
    sprintf(pathname,
            IMAGING_PATH "/ite_nmf/test_results_nreg/IP_SFX_tests_%s",
            lrformatName);
    strcat(g_out_path, pathname);
    LOS_Log("Current path: >%s< \n", ( char * ) g_out_path);
    init_test_results_nreg(g_out_path);
    mmte_testStart(testComment,
                   "test pipe LR (buffer + fr) YUV422 interleaved format",
                   g_out_path);

    ITE_testSFX_StreamLR(&usecase);

    mmte_testEnd();

    /* Clear g_out_path */
    memset(g_out_path, 0, KlogDirectoryLentgh * sizeof(char));
}

void
ITE_testSFX_StreamLR(
tps_sia_usecase p_sia_usecase)
{

    t_uint32    e_Flag_SolarisControl_0,
                e_Flag_SolarisControl_1;
    t_uint32    e_Flag_NegativeControl_0,
                e_Flag_NegativeControl_1;

    e_Flag_SolarisControl_0 = SFXSolariseControl_Enable;
    e_Flag_SolarisControl_1 = SFXSolariseControl_Enable;
    e_Flag_NegativeControl_0 = SFXNegativeControl_Enable;
    e_Flag_NegativeControl_1 = SFXNegativeControl_Enable;

    ITE_writePE(HostFrameConstraints_e_Flag_GuaranteeStaticFlickerFrameLength_Byte0, Flag_e_FALSE);

    // prepare buffers/LR pipe and Start streaming on LR pipe
    ITE_StreamLR(p_sia_usecase);

    LOS_Log("\n1secs stream on LR pipe\n");
    ITE_HR_Stop();
    ITE_HR_Free();

    // prepare buffers/HR pipe and Start streaming on HR pipe
    ITE_StreamHR(p_sia_usecase);

    LOS_Log("\n1secs stream on HR pipe\n");

    LOS_Sleep(500);

    // First type of test : set PE and test status PE
    ITE_NMF_Toggle_System_Coin();

    //Set Special effects solaris control
    ITE_set_SolarisControl_0_1_PE(e_Flag_SolarisControl_0, e_Flag_SolarisControl_1);

    // ITE_NMF_Toggle_System_Coin();
    //Set Special effects solaris control
    ITE_set_NegativeControl_0_1_PE(e_Flag_NegativeControl_0, e_Flag_NegativeControl_1);

    //ITE_NMF_Toggle_System_Coin();
    //Set Adsoc control page elements
    // ITE_setAdsoc_PK_Ctrl_0_1_PE (e_Flag_Adsoc_PK_Ctrl_0, e_Flag_Adsoc_PK_Ctrl_1);
    //  ITE_NMF_Toggle_System_Coin();
    //Set Adsoc Emboss page elements
    //ITE_setAdsoc_PK_Ctrl_0_1_u8_Adsoc_PK_Emboss_PE (e_Flag_Adsoc_PK_Ctrl_0_u8_Adsoc_PK_Emboss, e_Flag_Adsoc_PK_Ctrl_1_u8_Adsoc_PK_Emboss);
    ITE_NMF_Toggle_System_Coin();

    // check Adsoc Control page elements
    //ITE_testAdsoc_Ctrl_PE(e_Flag_Adsoc_PK_Ctrl_0, e_Flag_Adsoc_PK_Ctrl_1);
    //ITE_NMF_Toggle_System_Coin();
    // check Adsoc Emboss page elements
    //ITE_testAdsoc_Emboss_PE(e_Flag_Adsoc_PK_Ctrl_0_u8_Adsoc_PK_Emboss, e_Flag_Adsoc_PK_Ctrl_1_u8_Adsoc_PK_Emboss);
    //ITE_NMF_Toggle_System_Coin();
    // check Solaris Control page elements
    ITE_testSolaris_Ctrl_PE(e_Flag_SolarisControl_0, e_Flag_SolarisControl_1);

    //  ITE_NMF_Toggle_System_Coin();
    // check Negative Control page elements
    ITE_testNegative_Ctrl_PE(e_Flag_NegativeControl_0, e_Flag_NegativeControl_1);

    ITE_LR_Stop();
    ITE_LR_Free();

    //ITE_HR_Stop();
    //ITE_HR_Free();
}


/* -----------------------------------------------------------------------
FUNCTION : ITE_set_SolarisControl_0_1_PE
PURPOSE  : Set Solaris Control page elements
------------------------------------------------------------------------ */
void
ITE_set_SolarisControl_0_1_PE(
t_uint32    e_Flag_SolarisControl_0,
t_uint32    e_Flag_SolarisControl_1)
{
    ITE_writePE(SpecialEffects_Control_0_e_SFXSolarisControl_Byte0, e_Flag_SolarisControl_0);
    ITE_writePE(SpecialEffects_Control_1_e_SFXSolarisControl_Byte0, e_Flag_SolarisControl_1);
    LOS_Sleep(500);
}


/* -----------------------------------------------------------------------
FUNCTION : ITE_set_NegativeControl_0_1_PE
PURPOSE  : Set Solaris Control page elements
------------------------------------------------------------------------ */
void
ITE_set_NegativeControl_0_1_PE(
t_uint32    e_Flag_NegativeControl_0,
t_uint32    e_Flag_NegativeControl_1)
{
    ITE_writePE(SpecialEffects_Control_0_e_SFXNegativeControl_Byte0, e_Flag_NegativeControl_0);
    ITE_writePE(SpecialEffects_Control_1_e_SFXNegativeControl_Byte0, e_Flag_NegativeControl_1);
    LOS_Sleep(500);
}


/* -----------------------------------------------------------------------
FUNCTION : ITE_setAdsoc_PK_Ctrl_0_1_PE
PURPOSE  : Set Adsoc Control page elements
------------------------------------------------------------------------ */

/*void ITE_setAdsoc_PK_Ctrl_0_1_PE(t_uint32 e_Flag_Adsoc_PK_Ctrl_0, t_uint32 e_Flag_Adsoc_PK_Ctrl_1)
{
ITE_writePE(Adsoc_PK_Ctrl_0_e_Flag_Adsoc_PK_Enable_Byte0, e_Flag_Adsoc_PK_Ctrl_0);
ITE_writePE(Adsoc_PK_Ctrl_1_e_Flag_Adsoc_PK_Enable_Byte0,e_Flag_Adsoc_PK_Ctrl_1);
LOS_Sleep(500);
}*/

/* -----------------------------------------------------------------------
FUNCTION : ITE_setAdsoc_PK_Ctrl_0_1_u8_Adsoc_PK_Emboss_PE
PURPOSE  : Set Adsoc emboss page elements
------------------------------------------------------------------------ */

/*void ITE_setAdsoc_PK_Ctrl_0_1_u8_Adsoc_PK_Emboss_PE(t_uint32 e_Flag_Adsoc_PK_Ctrl_0_u8_Adsoc_PK_Emboss, t_uint32 e_Flag_Adsoc_PK_Ctrl_1_u8_Adsoc_PK_Emboss)
{
ITE_writePE(Adsoc_PK_Ctrl_0_u8_Adsoc_PK_Emboss_Effect_Ctrl_Byte0, e_Flag_Adsoc_PK_Ctrl_0_u8_Adsoc_PK_Emboss);
ITE_writePE(Adsoc_PK_Ctrl_1_u8_Adsoc_PK_Emboss_Effect_Ctrl_Byte0, e_Flag_Adsoc_PK_Ctrl_1_u8_Adsoc_PK_Emboss);
LOS_Sleep(500);
}
*/

/* -----------------------------------------------------------------------
FUNCTION : ITE_testSolaris_Ctrl_PE
PURPOSE  : check Solaris Control status page elements and decide if tests pass or fail
----------------------------------------------------------------------------------- */
void
ITE_testSolaris_Ctrl_PE(
t_uint32    e_Flag_SolarisControl_0,
t_uint32    e_Flag_SolarisControl_1)
{
    char        mess[256];
    t_uint32    temp,
                temp1;
    temp = IspRegRead_Value((ISP_CE0_SPECIALFX_SOL_ENABLE_OFFSET), ( long * ) &Read_Val, 1);
    temp1 = IspRegRead_Value((ISP_CE1_SPECIALFX_SOL_ENABLE_OFFSET), ( long * ) &Read_Val, 1);

    snprintf(mess, sizeof(mess), "Test %d :check for Solaris Control status ", ( int ) testCount);
    mmte_testNext(mess);
    LOS_Log("\nSolaris Control values %i,%i\n", e_Flag_SolarisControl_0, e_Flag_SolarisControl_1);
    LOS_Log("\nSolaris Control expected values %i,%i\n", temp, temp1);
    if
    (
        (e_Flag_SolarisControl_0 == IspRegRead_Value((ISP_CE0_SPECIALFX_SOL_ENABLE_OFFSET), ( long * ) &Read_Val, 1))
    &&  (e_Flag_SolarisControl_1 == IspRegRead_Value((ISP_CE1_SPECIALFX_SOL_ENABLE_OFFSET), ( long * ) &Read_Val, 1))
    )
    {
        //test passed
        LOS_Log("\nTEST PASS: Solaris Control values as expected\n");
        mmte_testResult(TEST_PASSED);
    }
    else
    {
        //test failed
        LOS_Log("\nTEST FAIL: Solaris Control values NOT as expected\n");
        mmte_testResult(TEST_FAILED);
    }


    testCount++;
}


/* -----------------------------------------------------------------------
FUNCTION : ITE_testNegative_Ctrl_PE
PURPOSE  : check Negative Control status page elements and decide if tests pass or fail
----------------------------------------------------------------------------------- */
void
ITE_testNegative_Ctrl_PE(
t_uint32    e_Flag_NegativeControl_0,
t_uint32    e_Flag_NegativeControl_1)
{
    char        mess[256];
    t_uint32    temp2,
                temp3;

    snprintf(mess, sizeof(mess), "Test %d :check for Solaris Control status ", ( int ) testCount);
    mmte_testNext(mess);
    temp2 = IspRegRead_Value((ISP_CE0_SPECIALFX_NEG_ENABLE_OFFSET), ( long * ) &Read_Val, 1);
    temp3 = IspRegRead_Value((ISP_CE1_SPECIALFX_NEG_ENABLE_OFFSET), ( long * ) &Read_Val, 1);

    LOS_Log("\nNegativeControl values %i,%i\n", e_Flag_NegativeControl_0, e_Flag_NegativeControl_1);
    LOS_Log("\nNegativeControl expected values %i,%i\n", temp2, temp3);

    if
    (
        (
            e_Flag_NegativeControl_0 == IspRegRead_Value(
                (ISP_CE0_SPECIALFX_NEG_ENABLE_OFFSET),
                ( long * ) &Read_Val,
                1)
        )
    &&  (
            e_Flag_NegativeControl_1 == IspRegRead_Value(
                (ISP_CE1_SPECIALFX_NEG_ENABLE_OFFSET),
                ( long * ) &Read_Val,
                1)
        )
    )
    {
        //test passed
        LOS_Log("\nTEST PASS: Negative Control values as expected\n");
        mmte_testResult(TEST_PASSED);
    }
    else
    {
        //test failed
        LOS_Log("\nTEST FAIL: Negative Control values NOT as expected\n");
        mmte_testResult(TEST_FAILED);
    }


    testCount++;
}


/* -----------------------------------------------------------------------
FUNCTION : ITE_testAdsoc_Ctrl_PE
PURPOSE  : check Adsoc Control status page elements and decide if tests pass or fail
----------------------------------------------------------------------------------- */

/*void ITE_testAdsoc_Ctrl_PE(t_uint32 e_Flag_Adsoc_PK_Ctrl_0, t_uint32 e_Flag_Adsoc_PK_Ctrl_1)
{
char mess[256];

  snprintf(mess, sizeof(mess), "Test %d :check for Adsoc Control status ",testCount);
  mmte_testNext(mess);


      if (  (e_Flag_Adsoc_PK_Ctrl_0  == ITE_readPE(Adsoc_PK_Ctrl_0_e_Flag_Adsoc_PK_Enable_Byte0))
      &&
      (e_Flag_Adsoc_PK_Ctrl_1  == ITE_readPE(Adsoc_PK_Ctrl_1_e_Flag_Adsoc_PK_Enable_Byte0))
      )
      {
      //test passed
      LOS_Log("\nTEST PASS: Adsoc Control values as expected\n");
      mmte_testResult(TEST_PASSED);
      }
      else
      {
      //test failed
      LOS_Log("\nTEST FAIL: Adsoc Control values NOT as expected\n");
      mmte_testResult(TEST_FAILED);
      }
      testCount++;
      }
*/

/* -----------------------------------------------------------------------
FUNCTION : ITE_testAdsoc_Emboss_PE
PURPOSE  : check Adsoc Emboss status page elements and decide if tests pass or fail
----------------------------------------------------------------------------------- */

/*void ITE_testAdsoc_Emboss_PE(t_uint32 e_Flag_Adsoc_PK_Ctrl_0_u8_Adsoc_PK_Emboss, t_uint32 e_Flag_Adsoc_PK_Ctrl_1_u8_Adsoc_PK_Emboss)
{
char mess[256];


    snprintf(mess, sizeof(mess), "Test %d :check for Adsoc Emboss status ",testCount);
    mmte_testNext(mess);

      if (  (e_Flag_Adsoc_PK_Ctrl_0_u8_Adsoc_PK_Emboss  == ITE_readPE(Adsoc_PK_Ctrl_0_u8_Adsoc_PK_Emboss_Effect_Ctrl_Byte0))
      &&
      (e_Flag_Adsoc_PK_Ctrl_1_u8_Adsoc_PK_Emboss  == ITE_readPE(Adsoc_PK_Ctrl_1_u8_Adsoc_PK_Emboss_Effect_Ctrl_Byte0))
      )
      {
      //test passed
      LOS_Log("\nTEST PASS: Adsoc  Emboss values as expected\n");
      //mmte_testResult(TEST_PASSED);
      }
      else
      {
      //test failed
      LOS_Log("\nTEST FAIL: Adsoc  Emboss values NOT as expected\n");
      mmte_testResult(TEST_FAILED);
      }
      testCount++;
      }
*/

/* -----------------------------------------------------------------------
FUNCTION : ITE_testMirrorFlip
PURPOSE  : Test Mirror/Flip on LR
------------------------------------------------------------------------ */
void
ITE_testMirrorFlip(
enum e_grabFormat   grbformat)
{
    char            testComment[200];
    char            lrformatName[16];
    char            sensorName[16];
    char            resolutionName[16];
    char            pathname[200];
    e_resolution    sizelr = SUBQCIF;

    if (usecase.sensor == 0)
    {
        sprintf(sensorName, "Cam0");
    }
    else
    {
        sprintf(sensorName, "Cam1");
    }

    usecase.LR_GrbFormat = grbformat;
    ITE_GiveStringFormat("LR", lrformatName);
    LOS_Log("\n --> Stream on LR pipe with GRBFMT_%s format\n", lrformatName);

    ITE_GiveStringNameFromEnum(sizelr, resolutionName);
    LOS_Log("\n -----> %s resolution\n", resolutionName);
    usecase.LR_XSize = ITE_GiveXSizeFromEnum(PIPE_LR, sizelr, grbformat);
    usecase.LR_YSize = ITE_GiveYSizeFromEnum(PIPE_LR, sizelr, grbformat);
    usecase.LR_resolution = ITE_GiveResolution(resolutionName);
    sprintf(testComment, "%s_test_IP_MirrorFlip_tests_%s_%s",
            sensorName, lrformatName, resolutionName);

    /* Perform a "cd" to destination repository for test log */
    sprintf(pathname,
            IMAGING_PATH "/ite_nmf/test_results_nreg/IP_MirrorFlip_tests_%s",
            lrformatName);
    strcat(g_out_path, pathname);
    LOS_Log("Current path: >%s< \n", ( char * ) g_out_path);
    init_test_results_nreg(g_out_path);
    mmte_testStart(testComment,
                   "test pipe LR (buffer + fr) YUV422 interleaved format",
                   g_out_path);

    ITE_testMirrorFlip_StreamLR(&usecase);

    mmte_testEnd();

    /* Clear g_out_path */
    memset(g_out_path, 0, KlogDirectoryLentgh * sizeof(char));
}

void
ITE_testMirrorFlip_StreamLR(
tps_sia_usecase p_sia_usecase)
{
    t_uint32    e_Flag_mirror = Flag_e_FALSE,
                e_Flag_flip = Flag_e_FALSE;
    ITE_writePE(HostFrameConstraints_e_Flag_GuaranteeStaticFlickerFrameLength_Byte0, Flag_e_FALSE);

    ITE_dbg_usagemode("vf");

    // prepare buffers/LR pipe and Start streaming on LR pipe
    ITE_StreamLR(p_sia_usecase);

    LOS_Log("\n1secs stream on LR pipe\n");
    LOS_Sleep(500);

    // First type of test : set PE and test status PE
    for (e_Flag_mirror = Flag_e_FALSE; e_Flag_mirror <= Flag_e_TRUE; e_Flag_mirror++)
    {
        for (e_Flag_flip = Flag_e_FALSE; e_Flag_flip <= Flag_e_TRUE; e_Flag_flip++)
        {
            //Set Mirror and Flip page elements
            ITE_setMirrorFlipPE(e_Flag_mirror, e_Flag_flip);

            // check Mirror and Flip page elements
            ITE_testMirrorFlipPE(e_Flag_mirror, e_Flag_flip);
        }
    }


    // Second type of test : stop streaming, set PE, start streaming, test status PE
    for (e_Flag_mirror = Flag_e_FALSE; e_Flag_mirror <= Flag_e_TRUE; e_Flag_mirror++)
    {
        for (e_Flag_flip = Flag_e_FALSE; e_Flag_flip <= Flag_e_TRUE; e_Flag_flip++)
        {
            ITE_LR_Stop();

            //Set Mirror and Flip page elements
            ITE_setMirrorFlipPE(e_Flag_mirror, e_Flag_flip);
            ITE_LR_Start(INFINITY);
            LOS_Sleep(500);

            if (g_InputImageSource == InputImageSource_e_Sensor0)
            {
                ITE_SensorMode(COLORBAR);
            }
            else
            {
                ITE_SetRxTestPattern(RxTestPattern_e_ColourBars);
            }


            // check Mirror and Flip page elements
            ITE_testMirrorFlipPE(e_Flag_mirror, e_Flag_flip);
        }
    }


    ITE_LR_Stop();
    ITE_LR_Free();
}


/* -----------------------------------------------------------------------
FUNCTION : ITE_setMirrorFlipPE
PURPOSE  : Set Mirror Flip page elements
------------------------------------------------------------------------ */
void
ITE_setMirrorFlipPE(
t_uint32    e_Flag_mirror,
t_uint32    e_Flag_flip)
{
    ITE_writePE(SystemSetup_e_Flag_mirror_Byte0, e_Flag_mirror);
    ITE_writePE(SystemSetup_e_Flag_flip_Byte0, e_Flag_flip);
    LOS_Sleep(500);
}


/* -----------------------------------------------------------------------
FUNCTION : ITE_testMirrorFlipPE
PURPOSE  : check Mirror Flip status page elements and decide if tests pass or fail
----------------------------------------------------------------------------------- */
void
ITE_testMirrorFlipPE(
t_uint32    e_Flag_mirror,
t_uint32    e_Flag_flip)
{
    char    mess[256];

    snprintf(mess, sizeof(mess), "Test %d :check for Mirror Flip status ", ( int ) testCount);
    mmte_testNext(mess);

    if
    (
        (e_Flag_mirror == ITE_readPE(SystemConfig_Status_e_Flag_mirror_Byte0))
    &&  (e_Flag_flip == ITE_readPE(SystemConfig_Status_e_Flag_flip_Byte0))
    )
    {
        //test passed
        LOS_Log("\nTEST PASS: Mirror Flip values as expected\n");
        mmte_testResult(TEST_PASSED);
    }
    else
    {
        //test failed
        LOS_Log("\nTEST FAIL: Mirror Flip values NOT as expected\n");
        mmte_testResult(TEST_FAILED);
    }


    testCount++;
}


/* -----------------------------------------------------------------------
FUNCTION : ITE_StreamLR
PURPOSE  : Prepare LR pipe and Start Streaming on LR
----------------------------------------------------------------------------------- */
void
ITE_StreamLR(
tps_sia_usecase p_sia_usecase)
{
    //if backup
    if (p_sia_usecase->flag_backup_fw == 1)
    {
        if (g_InputImageSource == InputImageSource_e_Sensor0)
        {
            ITE_SensorMode(COLORBAR);
        }
        else
        {
            ITE_SetRxTestPattern(RxTestPattern_e_ColourBars);
        }


        LOS_Sleep(500);
        ITE_LR_Prepare(p_sia_usecase, INFINITY);
        ITE_LR_Start(INFINITY);
    }


    // else LLD
    else
    {
        ITE_LR_Prepare(p_sia_usecase, INFINITY);
        ITE_LR_Start(INFINITY);
        LOS_Sleep(500);

        if (g_InputImageSource == InputImageSource_e_Sensor0)
        {
            ITE_SensorMode(COLORBAR);
        }
        else
        {
            ITE_SetRxTestPattern(RxTestPattern_e_ColourBars);
        }
    }
}


/* -----------------------------------------------------------------------
FUNCTION : ITE_StreamHR
PURPOSE  : Prepare HR pipe and Start Streaming on HR
----------------------------------------------------------------------------------- */
void
ITE_StreamHR(
tps_sia_usecase p_sia_usecase)
{
    //if backup
    if (p_sia_usecase->flag_backup_fw == 1)
    {
        if (g_InputImageSource == InputImageSource_e_Sensor0)
        {
            ITE_SensorMode(COLORBAR);
        }
        else
        {
            ITE_SetRxTestPattern(RxTestPattern_e_ColourBars);
        }


        LOS_Sleep(500);
        ITE_HR_Prepare(p_sia_usecase, INFINITY);
        ITE_HR_Start(INFINITY);
    }


    // else LLD
    else
    {
        ITE_HR_Prepare(p_sia_usecase, INFINITY);
        ITE_HR_Start(INFINITY);
        LOS_Sleep(500);
        if (g_InputImageSource == InputImageSource_e_Sensor0)
        {
            ITE_SensorMode(COLORBAR);
        }
        else
        {
            ITE_SetRxTestPattern(RxTestPattern_e_ColourBars);
        }
    }
}


/**********************************************Luma Test Starts Here  **********************************************/

/* -----------------------------------------------------------------------
FUNCTION : ITE_LumaOffsetTest
PURPOSE  : Luma Offset Tests
------------------------------------------------------------------------ */
void
ITE_LumaOffsetTest(
enum e_grabFormat   grbformat)
{
    char            testComment[200];
    char            lrformatName[16];
    char            sensorName[16];
    char            resolutionName[16];
    char            pathname[200];

    t_uint32        luma_offset_pipe_0,
                    luma_offset_pipe_1;
    t_uint32        CE_OutputCoderOffsetVector_Ce0,
                    CE_OutputCoderOffsetVector_Ce1;
    t_uint32        YFloor_Ce0,
                    YFloor_Ce1;
    t_uint32        Diff_Ce0,
                    Diff_Ce1;
    e_resolution    sizelr = SUBQCIF;

    luma_offset_pipe_0 = 10;
    luma_offset_pipe_1 = 20;

    if (usecase.sensor == 0)
    {
        sprintf(sensorName, "Cam0");
    }
    else
    {
        sprintf(sensorName, "Cam1");
    }

    usecase.LR_GrbFormat = grbformat;
    ITE_GiveStringFormat("LR", lrformatName);
    LOS_Log("\n --> Stream on LR pipe with GRBFMT_%s format\n", lrformatName);

    ITE_GiveStringNameFromEnum(sizelr, resolutionName);
    LOS_Log("\n -----> %s resolution\n", resolutionName);
    usecase.LR_XSize = ITE_GiveXSizeFromEnum(PIPE_LR, sizelr, grbformat);
    usecase.LR_YSize = ITE_GiveYSizeFromEnum(PIPE_LR, sizelr, grbformat);
    usecase.LR_resolution = ITE_GiveResolution(resolutionName);
    sprintf(testComment, "%s_test_IP_ColourMatrix_tests_%s_%s",
            sensorName, lrformatName, resolutionName);

    /* Perform a "cd" to destination repository for test log */
    sprintf(pathname,
            IMAGING_PATH "/ite_nmf/test_results_nreg/IP_ColourMatrix_tests_%s",
            lrformatName);
    strcat(g_out_path, pathname);
    LOS_Log("Current path: >%s< \n", ( char * ) g_out_path);
    init_test_results_nreg(g_out_path);
    mmte_testStart(testComment,
                   "test pipe LR (buffer + fr) YUV422 interleaved format",
                   g_out_path);

    /*usecase.HR_GrbFormat = grbformat;
        ITE_GiveStringFormat("HR", lrformatName);
        LOS_Log("\n --> Stream on HR pipe with GRBFMT_%s format\n",lrformatName);

          ITE_GiveStringNameFromEnum(sizelr, resolutionName);
          LOS_Log("\n -----> %s resolution\n",resolutionName);
          usecase.HR_XSize = ITE_GiveXSizeFromEnum(PIPE_HR, sizelr,grbformat);
          usecase.HR_YSize = ITE_GiveYSizeFromEnum(PIPE_HR, sizelr,grbformat);
          usecase.HR_resolution = ITE_GiveResolution(resolutionName);
          sprintf(testComment,"%s_test_IP_ColourMatrix_tests_%s_%s",sensorName,lrformatName,resolutionName);
          //perform a "cd" to destination repository for test log
          sprintf(pathname,"../../test_results_nreg/IP_ColourMatrix_tests_%s",lrformatName);
          strcat(g_out_path,pathname);
          LOS_Log("Current path: >%s< \n",(char *)g_out_path);
        mmte_testStart(testComment," test pipe HR (buffer + fr) YUV422 interleaved format", g_out_path);*/

    //ITE_SetupProto();
    //TODO : SS : check later
    //ITE_Glace_Disable();

    ITE_writePE(HostFrameConstraints_e_Flag_GuaranteeStaticFlickerFrameLength_Byte0,
                Flag_e_FALSE);

    /* Prepare buffers/LR pipe and Start streaming on LR pipe */
    ITE_StreamLR(&usecase);
    LOS_Log("\n1secs stream on LR pipe\n");
    ITE_StreamHR(&usecase);
    LOS_Log("\n1secs stream on HR pipe\n");
    LOS_Sleep(500);

    //ITE_CheckPipesStreaming();
    ITE_writePE(CE_LumaOffset_0_s16_LumaOffset_Byte0, luma_offset_pipe_0);
    ITE_writePE(CE_LumaOffset_1_s16_LumaOffset_Byte0, luma_offset_pipe_1);

    //TODO : SS : check later
    //ITE_Glace_Disable();
    //ITE_CheckPipesStreaming();
    LOS_Sleep(2000);
    CE_OutputCoderOffsetVector_Ce0 = ITE_readPE(CE_OutputCoderOffsetVector_0_s16_i0_Byte0);
    CE_OutputCoderOffsetVector_Ce1 = ITE_readPE(CE_OutputCoderOffsetVector_1_s16_i0_Byte0);

    // TODO : SS : remove this
    //read off the RGB2YUV Y_FLOOR For CE0
    //self.ReadHardwareRegister = self.device.getDeviceAttribute('ReadHardwareRegister')   /////////////////////////////
    YFloor_Ce0 = IspRegRead_Value((ISP_CE0_CODER_YFLOOR_OFFSET), ( long * ) &Read_Val, 1);
    YFloor_Ce1 = IspRegRead_Value((ISP_CE1_CODER_YFLOOR_OFFSET), ( long * ) &Read_Val, 1);

    Diff_Ce0 = YFloor_Ce0 - CE_OutputCoderOffsetVector_Ce0;

    if (ITE_readPE(CE_LumaOffset_0_u8_YFloor_Saturated_Byte0) == Flag_e_FALSE)
    {
        if (Diff_Ce0 != luma_offset_pipe_0)
        {
            LOS_Log("\nLuma Offset Not Applied Successfully for Brightness Support in Ce0 \n");
        }
        else
        {
            LOS_Log("\nLuma Offset Applied Successfully for Brightness Support in Ce0 \n");
        }
    }

    Diff_Ce1 = YFloor_Ce1 - CE_OutputCoderOffsetVector_Ce1;
    if (ITE_readPE(CE_LumaOffset_1_u8_YFloor_Saturated_Byte0) == Flag_e_FALSE)
    {
        if (Diff_Ce1 != luma_offset_pipe_1)
        {
            LOS_Log("\nLuma Offset Not Applied Successfully for Brightness Support in Ce1 \n");
        }
        else
        {
            LOS_Log("\nLuma Offset Applied Successfully for Brightness Support in Ce1 \n");
        }
    }

    //Stop Streaming HR
    //ITE_HR_Stop();
    //ITE_HR_Free();
    //Stop Streaming LR
    ITE_LR_Stop();
    ITE_LR_Free();

    mmte_testEnd();

    /* Clear g_out_path */
    memset(g_out_path, 0, KlogDirectoryLentgh * sizeof(char));
}

/* -----------------------------------------------------------------------
FUNCTION : ITE_SetupProto
PURPOSE  : Setup the Proto
------------------------------------------------------------------------ */
void
ITE_SetupProto(void)
{
    //  WriteHardwareRegister = self.device.getDeviceAttribute('WriteHardwareRegister')    ///////////////////////////
    /*
        WriteHardwareRegister(0x3000, 0x00)
        WriteHardwareRegister(0x3004, 0x01)
        WriteHardwareRegister(0x3008, 0x04)
        */
}


/* -----------------------------------------------------------------------
FUNCTION : ITE_Glace_Disable
PURPOSE  : Disable Glace
------------------------------------------------------------------------ */
void
ITE_Glace_Disable(void)
{
    GlaceOperationMode_te   e_GlaceOperationMode_Control;
    e_GlaceOperationMode_Control = GlaceOperationMode_e_Disable;

    //if (Flag_e_TRUE == IsStreaming())
    {
        ITE_writePE(Glace_Control_e_GlaceOperationMode_Control_Byte0, e_GlaceOperationMode_Control);
        ITE_ProgramControlCount();
        ITE_WaitOnControlCount();
        ITE_writePE(Glace_Status_e_GlaceOperationMode_Status_Byte0, e_GlaceOperationMode_Control);
    }


    //else
    {
        ITE_writePE(Glace_Control_e_GlaceOperationMode_Control_Byte0, e_GlaceOperationMode_Control);
        ITE_writePE(Glace_Status_e_GlaceOperationMode_Status_Byte0, e_GlaceOperationMode_Control);
    }
}


/* -----------------------------------------------------------------------
FUNCTION : ITE_ProgramControlCount
PURPOSE  : Program count control
------------------------------------------------------------------------ */
void
ITE_ProgramControlCount(void)
{
    t_uint16    u8_ControlUpdateCount;
    u8_ControlUpdateCount = ITE_readPE(Glace_Status_u8_ControlUpdateCount_Byte0);
    u8_ControlUpdateCount += 1;
    if (u8_ControlUpdateCount > 255)
    {
        u8_ControlUpdateCount = 0;
    }


    ITE_writePE(Glace_Control_u8_ControlUpdateCount_debug_Byte0, u8_ControlUpdateCount);
}


/* -----------------------------------------------------------------------
FUNCTION : ITE_WaitOnControlCount
PURPOSE  : Wait On Control Count
------------------------------------------------------------------------ */
void
ITE_WaitOnControlCount(void)
{
    t_uint8 u8_ControlUpdateCount;
    u8_ControlUpdateCount = ITE_readPE(Glace_Control_u8_ControlUpdateCount_debug_Byte0);

    while (ITE_readPE(Glace_Status_u8_ControlUpdateCount_Byte0) != u8_ControlUpdateCount)
    {
        LOS_Sleep(1000);    //time.sleep(1.0)
    }
}


void
ITE_NMF_Toggle_System_Coin(void)
{
    if (Coin_e_Heads == ITE_readPE(SensorPipeSettings_Status_e_Coin_SensorSettings_Byte0))
    {
        ITE_writePE(SensorPipeSettings_Control_e_Coin_SensorSettings_Byte0, Coin_e_Tails);
    }
    else
    {
        ITE_writePE(SensorPipeSettings_Control_e_Coin_SensorSettings_Byte0, Coin_e_Heads);
    }


    LOS_Sleep(500);
}

