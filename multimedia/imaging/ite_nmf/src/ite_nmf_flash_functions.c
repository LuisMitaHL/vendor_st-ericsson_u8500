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

#include "ite_nmf_standard_functions.h"
#include "ite_nmf_flash_functions.h"
#include "hi_register_acces.h"
#include "pictor.h"
#include "ite_testenv_utils.h"
#include "ite_debug.h"
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

#include <cm/inc/cm_macros.h>
#include <test/api/test.h>

#include <los/api/los_api.h>
#include "ite_main.h"
#include "sia.h"
#include "ite_nmf_headers.h"
#include "ite_sia_bootcmd.h"

/* Defines */
#define DMA_GRAB_OK_OFFSET          0x1FE0
#define DMA_FRAME_ID_OFFSET         (DMA_GRAB_OK_OFFSET + 4)
#define NO_OF_FLASH_CONFIGURATIONS  5
#define TEST_DMA_GRAB_OK

/* Static Functions */
static Flag_te      Is_Flash_Supported (void);
static int         Init_UseCase (void);
static void         ConfigureSystemConfigParams (t_uint32 timeus, t_uint32 AGain);
static void         Configure_Flash (
                    t_uint32    s32_DelayFromStartPoint,
                    t_uint32    u32_StrobeLength_us,
                    t_uint32    u8_FrameCount,
                    t_uint32    u8_MaximumNumberOfFrames,
                    t_uint32    e_StrobeStartPoint_Frame);
static RESULT_te    Verify_Params_Absorbed (
                    Flag_te isCoinToggledWhileStreaming,
                    Flag_te enableFlash);
static void         Send_System_Params (
                    Flag_te toggleCoin, 
                    Flag_te enableFlash);
static RESULT_te    Flash_enable_pre_streaming(char* datapath);

/* Static Variables */
extern t_uint8      ITE_Event_Log_flag;

extern tps_siaMetaDatabuffer    pStatsBuffer;

t_cmd_list          ite_cmd_list_flash[] =
{
    { "FlashHelp", C_ite_dbg_FlashHelp_cmd, "FlashHelp: FlashHelp\n" },
    { "TestFlashSupport", C_ite_dbg_TestFlashSupport_cmd, "TestFlashSupport: TestFlashSupport \n" },
    { "FlashSanityTest", C_ite_dbg_FlashSanityTest_cmd, "FlashSanityTest: FlashSanityTest \n" },
    { "ConfigureFlash", C_ite_dbg_ConfigureFlash_cmd, "ConfigureFlash: ConfigureFlash \n" },
    { "VerifyFlashTriggered", C_ite_dbg_VerifyFlashTriggered_cmd,"VerifyFlashTriggered: VerifyFlashTriggered flashmode\n" },
    { "Flash_enable", C_ite_dbg_Flash_enable_pre_streaming_cmd,"Flash_enable:Flash_enable_pre_streaming\n"}
#if FLASH_DRIVER_INCLUDE
     ,{ "Flash_TestLitFrame", C_ite_dbg_Flash_TestLitFrame_cmd,"Flash_TestLitFrame: Test for flash lit frame \n"}
#endif
};

/* -----------------------------------------------------------------------
FUNCTION : Init_Flash_ITECmdList
PURPOSE  :
------------------------------------------------------------------------ */
void
Init_Flash_ITECmdList(void)
{
    CLI_register_interface(
    "Flash_ITE_CMB",
    sizeof(ite_cmd_list_flash) / (sizeof(ite_cmd_list_flash[0])),
    ite_cmd_list_flash,
    1);
}

/* -----------------------------------------------------------------------
FUNCTION : C_ite_dbg_FlashHelp_cmd
PURPOSE  : Displays Flash Test options for the user.
------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_FlashHelp_cmd(
int     a_nb_args,
char    **ap_args)
{
    char    mess[256];
    UNUSED(a_nb_args);
    UNUSED(ap_args);

    MMTE_TEST_START("FlashHelp", "/ite_nmf/test_results_nreg/flash_tests", "Testing for Flash_help");
    sprintf(mess, "Testing for Flash_help");
    MMTE_TEST_NEXT(mess);

    CLI_disp_msg("\n");

    CLI_disp_msg("TestFlashSupport: Checks whether Flash is suppported by the firmware \n");
    CLI_disp_msg("TestFlashSupport[Syntax]: TestFlashSupport \n\n");

    CLI_disp_msg("FlashSanityTest:  Performs Flash sanity test with default parameters \n");
    CLI_disp_msg("FlashSanityTest[Syntax]: FlashSanityTest \n\n");

    CLI_disp_msg("ConfigureFlash:   Configures the Flash with user given arguments\n");
    CLI_disp_msg("If 1st parameter is <wait>, verifies also for flash trigger. \n");
    CLI_disp_msg(
    "ConfigureFlash[Syntax]: ConfigureFlash [<wait>|<nowait>] s32_DelayFromStartPoint u32_StrobeLength_us u8_FrameCount u8_MaximumNumberOfFrames e_StrobeStartPoint_Frame\n\n");
    CLI_disp_msg("ConfigureFlash[Example]: ConfigureFlash wait -30 500 1 10 0\n\n");

    CLI_disp_msg("VerifyFlashTriggered:  Verifes whether flash triggered. \n");
    CLI_disp_msg("VerifyFlashTriggered[Syntax]: VerifyFlashTriggered \n\n");


    MMTE_TEST_SKIPPED();
    return (CMD_COMPLETE);
}


/* -----------------------------------------------------------------------
FUNCTION : C_ite_dbg_TestFlashSupport_cmd
PURPOSE  :  Function to check the flash is supported or not
------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_TestFlashSupport_cmd(
int     a_nb_args,
char    **ap_args)
{
    char    mess[256];
    UNUSED(ap_args);
    MMTE_TEST_START("TestFlashSupport", "/ite_nmf/test_results_nreg/flash_tests", "Testing for Flash_SUPPORT");

    sprintf(mess, "Testing for Flash_SUPPORT");
    MMTE_TEST_NEXT(mess);
    if (a_nb_args != 1)
    {
        CLI_disp_error("TestFlashSupport: Checks whether Flash is suppported by the firmware \n");
        CLI_disp_error("TestFlashSupport[Command]: TestFlashSupport \n\n");
        CLI_disp_error("Not correct command arguments\n");
        MMTE_TEST_SKIPPED();

        return (CMD_ERR_ARGS);
    }


    if (Flag_e_FALSE == Is_Flash_Supported())
    {
        snprintf(mess, sizeof(mess), "Flash NOT Supported by the device! \n");
        MMTE_TEST_COMMENT(mess);


        MMTE_TEST_SKIPPED();
    }
    else
    {
        snprintf(mess, sizeof(mess), "Flash Supported by the device! \n");
        MMTE_TEST_COMMENT(mess);


        MMTE_TEST_PASSED();
    }


    return (CMD_COMPLETE);
}


/* -----------------------------------------------------------------------
FUNCTION : C_ite_dbg_FlashSanityTest_cmd
PURPOSE  : Function to Test default Flash support
------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_FlashSanityTest_cmd(
int     a_nb_args,
char    **ap_args)
{
    t_uint32    dataPathSelCount = 0,
                streamStatus = 0,
                configCount = 0,
                iLoop = 0,
                testCount = 0;
    int ret =EOK;
    char        mess[256];
    RESULT_te   result = SUCCESS;
    Flag_te     flashMode;
    UNUSED(ap_args);

    MMTE_TEST_START("TestFlashSanity", "/ite_nmf/test_results_nreg/flash_tests", "Test Flash_sanity");

    if (a_nb_args != 1)
    {
        CLI_disp_error("FlashSanityTest:  Performs Flash sanity test with default parameters \n");
        CLI_disp_error("FlashSanityTest[Syntax]: FlashSanityTest \n\n");

        MMTE_TEST_SKIPPED();
        return (CMD_ERR_ARGS);
    }

    if (Flag_e_FALSE == Is_Flash_Supported())
    {
        MMTE_TEST_SKIPPED();
        goto END;
    }

    ret = Init_UseCase();

    if(ret != EOK)
    {
        CLI_disp_error("Not correct command arguments C_ite_dbg_FlashSanityTest_cmd\n");
        goto END;
    }


    //Loop - 0 : For various flash configurations
    for (configCount = 0; configCount < NO_OF_FLASH_CONFIGURATIONS; configCount++)
    {
        //Loop - 1: For LR(0) HR(1) BMS(2) LRHR(3) LRBMS(4) HRBMS(5) LRHRBMS(6) datapaths
        //[TBD] Currently looping from 0 through 3 only,
        //as (4),(5) and (6) are not supported by the Ite_Nmf APIs
        for (dataPathSelCount = 0; dataPathSelCount <= 3; dataPathSelCount++)
        {
            //Inner Loop - 2 : For Toggling Flashmode
            for (iLoop = 0; iLoop <= 1; iLoop++)
            {
                //Inner Loop - 3 : For STOP(0) and RUN(1) States
                for (streamStatus = 0; streamStatus <= 1; streamStatus++)
                {
                    snprintf(mess, sizeof(mess), "[Test %ld] Next iteration for flash sanity test\n", ++testCount);
                    MMTE_TEST_NEXT(mess);

                    flashMode = (Flag_te)iLoop;
                    
                    snprintf(mess, sizeof(mess), "[Usecase] Datapath=%ld Flashmode=%d FW_state_during_Flash_Configuration=%ld\n", dataPathSelCount, flashMode, streamStatus);
                    MMTE_TEST_COMMENT(mess);

                    //reset result
                    result = SUCCESS;

                    if (((0 == dataPathSelCount) || (4 == dataPathSelCount)) 
                     && (1 == streamStatus))
                    {
                        LOS_Log("Starting LR \n");
                        ITE_LR_Prepare(&usecase, INFINITY);
                        ITE_LR_Start(INFINITY);
                        MMTE_TEST_COMMENT("LR STARTED \n");
                    }


                    if (((1 == dataPathSelCount) || (5 == dataPathSelCount)) 
                     && (1 == streamStatus))
                    {
                        LOS_Log("Starting HR \n");
                        ITE_HR_Prepare(&usecase, INFINITY);
                        ITE_HR_Start(INFINITY);
                        MMTE_TEST_COMMENT("HR STARTED \n");
                    }


                    if (((3 == dataPathSelCount) || (6 == dataPathSelCount)) 
                     && (1 == streamStatus))
                    {
                        LOS_Log("Starting LRHR \n");
                        ITE_LRHR_Prepare(&usecase, INFINITY);
                        ITE_LRHR_Start(INFINITY);
                        MMTE_TEST_COMMENT("LRHR STARTED \n");
                    }


                    if
                    (
                        (
                            (2 == dataPathSelCount)
                        ||  (4 == dataPathSelCount)
                        ||  (5 == dataPathSelCount)
                        ||  (6 == dataPathSelCount)
                        )
                    &&  (1 == streamStatus)
                    )
                    {
                        LOS_Log("Starting BMS \n");
                        ITE_BMS_Prepare(&usecase, INFINITY);
                        ITE_BMS_Start(1);
                        MMTE_TEST_COMMENT("BMS STARTED \n");
                    }


                    //workaround for CSI2 or GRAB errors
                    LOS_Sleep(50);

                    //Configure Exposure params, to check simulataneous exposure + flash use case
                    if (testCount % 2 == 0)
                    {
                        //set some random values
                        ConfigureSystemConfigParams(((testCount + configCount + 1) * 1000), ((configCount + 1) * 512));
                    }

                    //Configure Flash
                    Configure_Flash(
                    (configCount + 1) * 10,
                    (configCount + 1) * 100,
                    1,
                    10 - configCount,
                    configCount % 2);

                    //Toggle Coin
                    if (1 == streamStatus)
                    {
                        Send_System_Params(Flag_e_TRUE, flashMode);
                    }
                    
                    if (((0 == dataPathSelCount) || (4 == dataPathSelCount)) 
                     && (0 == streamStatus))
                    {
                        Send_System_Params(Flag_e_TRUE, flashMode);
                        
                        LOS_Log("Starting LR \n");
                        ITE_LR_Prepare(&usecase, INFINITY);
                        ITE_LR_Start(INFINITY);
                        MMTE_TEST_COMMENT("LR STARTED \n");
                    }


                    if (((1 == dataPathSelCount) || (5 == dataPathSelCount)) 
                     && (0 == streamStatus))
                    {
                        Send_System_Params(Flag_e_TRUE, flashMode);
                        
                        LOS_Log("Starting HR \n");
                        ITE_HR_Prepare(&usecase, INFINITY);
                        ITE_HR_Start(INFINITY);
                        MMTE_TEST_COMMENT("HR STARTED \n");
                    }


                    if (((3 == dataPathSelCount) || (6 == dataPathSelCount)) 
                     && (0 == streamStatus))
                    {
                        Send_System_Params(Flag_e_TRUE, flashMode);
                        
                        LOS_Log("Starting LRHR \n");
                        ITE_LRHR_Prepare(&usecase, INFINITY);
                        ITE_LRHR_Start(INFINITY);
                        MMTE_TEST_COMMENT("LRHR STARTED \n");
                    }


                    if
                    (
                        (
                            (2 == dataPathSelCount)
                        ||  (4 == dataPathSelCount)
                        ||  (5 == dataPathSelCount)
                        ||  (6 == dataPathSelCount)
                        )
                    &&  (0 == streamStatus)
                    )
                    {
                        //ite_bms_start will implicitly toggle system coin
                        Send_System_Params(Flag_e_TRUE, flashMode);
                        
                        LOS_Log("Starting BMS \n");
                        ITE_BMS_Prepare(&usecase, INFINITY);
                        ITE_BMS_Start_Extention(1,"nowait","toggle");
                        //ITE_BMS_Start(1);
                        MMTE_TEST_COMMENT("BMS STARTED \n");
                    }


                    //workaround for CSI2 or GRAB errors
                    LOS_Sleep(50);
                    
                    if (SUCCESS == result)
                    {
                        result |= (Verify_Params_Absorbed(streamStatus, flashMode));
                    }

                    if ((0 == dataPathSelCount) || (4 == dataPathSelCount))
                    {
                        ITE_LR_Stop();
                        ITE_LR_Free();
                        LOS_Log("LR STOPPED\n\n");
                    }


                    if ((1 == dataPathSelCount) || (5 == dataPathSelCount))
                    {
                        ITE_HR_Stop();
                        ITE_HR_Free();
                        LOS_Log("HR STOPPED\n\n");
                    }


                    if ((3 == dataPathSelCount) || (6 == dataPathSelCount))
                    {
                        ITE_LRHR_Stop();
                        ITE_LRHR_Free();
                        LOS_Log("LRHR STOPPED\n\n");
                    }


                    if (
                            (2 == dataPathSelCount)
                        ||  (4 == dataPathSelCount)
                        ||  (5 == dataPathSelCount)
                        ||  (6 == dataPathSelCount)
                        )
                    {
                        ITE_BMS_Stop();
                        ITE_BMS_Free();
                        LOS_Log("BMS STOPPED\n\n");
                    }


                    //delay must here, for stop latency
                    LOS_Sleep(50);

                    if (SUCCESS == result)
                    {
                        MMTE_RESULT_PASSED();
                    }
                    else
                    {
                        MMTE_RESULT_FAILED();
                    }
                }
            }
        }
    }


    MMTE_TEST_END();
    
    Send_System_Params(Flag_e_FALSE, Flag_e_FALSE);  //cleanup
    END:
    return (CMD_COMPLETE);
}


/* -----------------------------------------------------------------------
FUNCTION : C_ite_dbg_ConfigureFlash_cmd
PURPOSE  : Function to configure user given flash parameters
------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_ConfigureFlash_cmd(
int     a_nb_args,
char    **ap_args)
{
    t_uint32    s32_DelayFromStartPoint,
                u32_StrobeLength_us,
                u8_FrameCount,
                u8_MaximumNumberOfFrames,
                e_StrobeStartPoint_Frame;
    char        mess[256], fname[256], pathname[256];
    RESULT_te   result = SUCCESS;

    sprintf(fname, "ConfigureFlash_");
    sprintf(mess, "%s_%s_%s_%s_%s_%s", ap_args[1],ap_args[2],ap_args[3],ap_args[4],ap_args[5],ap_args[6]);
    strcat(fname, mess);
      
    memset(g_out_path,0,KlogDirectoryLentgh*sizeof(char));
    sprintf(pathname,"/ite_nmf/test_results_nreg/flash_tests");

    MMTE_TEST_START(fname, pathname,"Testing for Flash_configure");

    if (a_nb_args == 7)
    {
        if (Flag_e_FALSE == Is_Flash_Supported())
        {
            MMTE_TEST_SKIPPED();
            return (CMD_COMPLETE);
        }

        sprintf(mess, "Testing for Flash_configure");
        MMTE_TEST_NEXT(mess);


        s32_DelayFromStartPoint = ITE_ConvToInt32(ap_args[2]);
        u32_StrobeLength_us = ITE_ConvToInt32(ap_args[3]);
        u8_FrameCount = ITE_ConvToInt32(ap_args[4]);
        u8_MaximumNumberOfFrames = ITE_ConvToInt32(ap_args[5]);
        e_StrobeStartPoint_Frame = ITE_ConvToInt32(ap_args[6]);

        Configure_Flash(
        s32_DelayFromStartPoint,
        u32_StrobeLength_us,
        u8_FrameCount,
        u8_MaximumNumberOfFrames,
        e_StrobeStartPoint_Frame);

        Send_System_Params(Flag_e_TRUE, Flag_e_TRUE);

        if (0 == strcmp(ap_args[1], "wait"))
        {
            result |= (Verify_Params_Absorbed(Flag_e_TRUE, Flag_e_TRUE));
        }        
    }
    else
    {
        CLI_disp_error("Not correct command arguments\n");

        CLI_disp_error("ConfigureFlash:   Configures the Flash with user given arguments\n");
        CLI_disp_error(
        "ConfigureFlash[Syntax]: ConfigureFlash [<wait>|<nowait>] s32_DelayFromStartPoint u32_StrobeLength_us u8_FrameCount u8_MaximumNumberOfFrames e_StrobeStartPoint_Frame\n\n");
        CLI_disp_error("ConfigureFlash[Example]: ConfigureFlash wait -30 500 1 10 0\n\n");

        MMTE_TEST_SKIPPED();
        return (CMD_ERR_ARGS);
    }

    if (SUCCESS == result)
    {
        MMTE_RESULT_PASSED();
    }
    else
    {
        MMTE_RESULT_FAILED();
    }

    if (0 == strcmp(ap_args[1], "wait"))
    {
        MMTE_TEST_END();
        Send_System_Params(Flag_e_FALSE, Flag_e_FALSE);  //cleanup
    }

    return (CMD_COMPLETE);
}


/* -----------------------------------------------------------------------
FUNCTION : C_ite_dbg_VerifyFlashTriggered_cmd
PURPOSE  : Function to check flash is triggred or not
------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_VerifyFlashTriggered_cmd(
int     a_nb_args,
char    **ap_args)
{
    char        mess[256];
    RESULT_te   result = SUCCESS;
    UNUSED(ap_args);

    sprintf(mess, "Testing for VerifyFlashTriggered");
    MMTE_TEST_NEXT(mess);

    if (Flag_e_FALSE == Is_Flash_Supported())
    {   
        MMTE_TEST_SKIPPED();
        return (CMD_COMPLETE);
    }

    if (a_nb_args == 1)
    {
        result |= (Verify_Params_Absorbed(Flag_e_FALSE, Flag_e_TRUE));
    }

    if (SUCCESS == result)
    {
        MMTE_TEST_PASSED();
    }
    else
    {
        MMTE_TEST_FAILED();
    }

    Send_System_Params(Flag_e_FALSE, Flag_e_FALSE);  //cleanup
    return (CMD_COMPLETE);
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_Flash_enable_pre_streaming_cmd
   PURPOSE  : enable flash before streaming
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_Flash_enable_pre_streaming_cmd(int a_nb_args, char ** ap_args)
{
    
    if (ap_args[1]==NULL) ap_args[1]=""; //For Linux, strcmp doesn't support NULL parameter
    
    if (0== strcmp(ap_args[1],"help")) {
        CLI_disp_error("syntax: Flash_enable\n");}
    else {
        if (a_nb_args == 2)
        {Flash_enable_pre_streaming(ap_args[1]);}
        else
        {CLI_disp_error("Not correct command arguments\n"); }
    }
    return CMD_COMPLETE;
}


/* -----------------------------------------------------------------------
FUNCTION :  Is_Flash_Supported
PURPOSE  :  Function to check the flash is supported or not.
------------------------------------------------------------------------ */
static Flag_te
Is_Flash_Supported(void)
{
    t_uint32    Flash_Support;
    //char        comment[256];

    Flash_Support = ITE_readPE(FlashStatus_u8_MaxStrobesPerFrame_Byte0);
    if (Flash_Support > 0)
    {
        return (Flag_e_TRUE);
    }

    return (Flag_e_FALSE);
}


/* -----------------------------------------------------------------------
FUNCTION :  Init_UseCase
PURPOSE  :  Function to initialise usecase
------------------------------------------------------------------------ */
static int
Init_UseCase(void)
{
    int ret = EOK;
    //[TBD][Partha]Should be tried for all available resolutions
    ret = ITE_dbg_setusecase("LR", "QVGA", "YUV422RI");
    if(ret != EOK)
    {
         goto END;
    }
    ret = ITE_dbg_setusecase("HR", "VGA", "YUV420MB_D");
    if(ret != EOK)
    {
         goto END;
    }
    ITE_dbg_bmsmode("STILL");
    ITE_dbg_bmsout("BAYERSTORE2");
    //dummy values
    //[TBD][Partha]Should be modified with the values from sensor modes
    ret = ITE_dbg_setusecase("BMS", "VGA", "RAW12");
    if(ret != EOK)
    {
         goto END;
    }
    END:
    return ret;
}


/* -----------------------------------------------------------------------
FUNCTION : Apply exp, wb, analog gain settings.
PURPOSE  : Function to set system config settings.
------------------------------------------------------------------------ */
static void
ConfigureSystemConfigParams(
t_uint32    timeus,
t_uint32    AGain)
{
    LOS_Log("\nConfigure SystemConfigParams now..\n");

    ITE_writePE(Exposure_DriverControls_u32_TargetExposureTime_us_Byte0, timeus);
    ITE_writePE(Exposure_DriverControls_u16_TargetAnalogGain_x256_Byte0, AGain);
}


/* -----------------------------------------------------------------------
FUNCTION : Apply flash settings.
PURPOSE  : Function to set flash settings.
------------------------------------------------------------------------ */
static void
Configure_Flash(
t_uint32    s32_DelayFromStartPoint,
t_uint32    u32_StrobeLength_us,
t_uint32    u8_FrameCount,
t_uint32    u8_MaximumNumberOfFrames,
t_uint32    e_StrobeStartPoint_Frame)
{
    char    comment[256];

    LOS_Log("\nConfigure Flash  now..\n");

    sprintf(
    comment,
    "Configuring  Flash for  s32_DelayFromStartPoint = %ld u32_StrobeLength_us = %ld u8_FrameCount = %ld u8_MaximumNumberOfFrames = %ld e_StrobeStartPoint_Frame = %ld \n",
    s32_DelayFromStartPoint,
    u32_StrobeLength_us,
    u8_FrameCount,
    u8_MaximumNumberOfFrames,
    e_StrobeStartPoint_Frame);
    MMTE_TEST_COMMENT(comment);

    ITE_writePE(FlashControl_s32_DelayFromStartPoint_lines_Byte0, s32_DelayFromStartPoint);
    ITE_writePE(FlashControl_u32_StrobeLength_us_Byte0, u32_StrobeLength_us);
    ITE_writePE(FlashControl_u8_FrameCount_Byte0, u8_FrameCount);
    ITE_writePE(FlashControl_u8_MaxFramesToWaitForFlashTrigger_Byte0, u8_MaximumNumberOfFrames);
    ITE_writePE(FlashControl_e_StrobeStartPoint_Frame_Byte0, e_StrobeStartPoint_Frame);

#if FLASH_DRIVER_INCLUDE
    // ER:408207: Flash driver integration in ITE NMF.
    ITE_FlashDriverConfigure();

#endif
}


/* -----------------------------------------------------------------------
FUNCTION :  Send_System_Params
PURPOSE  :  Function to set flash mode and then toggle system coin
------------------------------------------------------------------------ */
static void
Send_System_Params (
Flag_te toggleCoin, 
Flag_te enableFlash)
{
    Coin_te control_coin,
            status_coin;

    //1. Write Flashmode
    ITE_writePE(FlashControl_e_Flag_FlashMode_Byte0, (t_uint32) enableFlash);

    //2. Toggle System Coin
    control_coin = (Coin_te) ITE_readPE(SystemSetup_e_Coin_Ctrl_Byte0);
    status_coin = (Coin_te) ITE_readPE(SystemConfig_Status_e_Coin_Status_Byte0);

    if (control_coin == status_coin)
    {
        if (Flag_e_TRUE == toggleCoin)
        {
            LOS_Log("\nToggling Coin..\n");
            ITE_writePE(SystemSetup_e_Coin_Ctrl_Byte0, !control_coin);
        }
    }
    else
    {
        LOS_Log("\nWarning:coins in unexpected state. Could be because ite_bms_start() got called.\n");
    }
    
    
    ITE_RefreshEventCount(Event0_Count_u16_EVENT0_19_EXPOSURE_AND_WB_PARAMETERS_UDPATE_Byte0);
    ITE_RefreshEventCount(Event0_Count_u16_EVENT0_17_GLACE_STATS_READY_Byte0);
    ITE_RefreshEventCount(Event3_Count_u16_EVENT3_0_DMA_GRAB_OK_Byte0);
}


/* -----------------------------------------------------------------------
FUNCTION : Verify_Params_Absorbed.
PURPOSE  : Function to verify flash is triggred or not.
           Performs the following checks:
           1. Checks System Coin Status
           2. Exp Notification
           3. Glace Notification
           4. DMA OK Notification (If BMS2 enabled)
           5. Compare DMA OK Frame Id with FrameParamStatus.u32_frame_counter
           6. FrameParamStatus.u32_flash_fired (if FlashMode True)
           7. FlashStatus.u8_FlashFiredFrameCount (if FlashMode True)
------------------------------------------------------------------------ */
static RESULT_te
Verify_Params_Absorbed(
Flag_te isCoinToggledWhileStreaming,
Flag_te enableFlash)
{
    Coin_te     control_coin, status_coin;
    t_uint32    frameCount;
    char        comment[256];
    RESULT_te   result = SUCCESS;
    //t_uint32  grabFrameId=0, statsFrameId=0;
    t_sint16    err;

     ITE_NMF_WaitStatReady(ISP_GLACE_STATS_READY);   
   
   //1. Checks System Coin Status
   LOS_Log("\nChecking coin status.. \n");
   
        control_coin = (Coin_te) ITE_readPE(SystemSetup_e_Coin_Ctrl_Byte0);
        status_coin = (Coin_te) ITE_readPE(SystemConfig_Status_e_Coin_Status_Byte0);

   if (status_coin != control_coin)
   {
    result |= FAILURE;
   }


    //2. Exp Notification
    LOS_Log("\nChecking exposure event.. \n");

    err = ITE_GetEventCount(Event0_Count_u16_EVENT0_19_EXPOSURE_AND_WB_PARAMETERS_UDPATE_Byte0);
    if (err == 0)
    {
        sprintf(comment, "1 Exposure event(s) received.\n");
        MMTE_TEST_COMMENT(comment);        

        result |= SUCCESS;
    }
    else
    {
        sprintf(comment, "Test will fail as no of Exposure events received != 1\n");
        MMTE_TEST_COMMENT(comment);

        result |= FAILURE;
    }

    //If it's not BMS0 and for BMS pre-run case, glace event has been already checked
    if(!((Flag_e_TRUE == ITE_readPE(DataPathStatus_e_Flag_BayerStore0Enable_Byte0)) ||
        ((Flag_e_FALSE == isCoinToggledWhileStreaming) &&
        (Flag_e_TRUE == ITE_readPE(DataPathStatus_e_Flag_BayerStore2Enable_Byte0)))))
    {

        //3. Glace Notification
        LOS_Log("\nChecking glace event.. \n");
        err = ITE_GetEventCount(Event0_Count_u16_EVENT0_17_GLACE_STATS_READY_Byte0);
        if (err == 0)
        {
            sprintf(comment, "1 glace event(s) received.\n");
            MMTE_TEST_COMMENT(comment);

            result |= SUCCESS;
        }
        else
        {
            sprintf(comment, "Test will fail as no of glace events received != 1\n");
            MMTE_TEST_COMMENT(comment);
            result |= FAILURE;
        }
    }


    //4. DMA OK Notification
    if (
         (
           ( (Flag_e_TRUE == (ITE_readPE(DataPathStatus_e_Flag_BayerStore2Enable_Byte0))) || (Flag_e_TRUE == (ITE_readPE(DataPathStatus_e_Flag_Pipe0Enable_Byte0))))
            &&
           (GrabMode_e_NORMAL == (ITE_readPE(SystemSetup_e_GrabMode_Ctrl_Byte0)))
         )
         ||
         (
           GrabMode_e_FORCE_OK == (ITE_readPE(SystemSetup_e_GrabMode_Ctrl_Byte0))
         )
       )
    {
        LOS_Log("\nChecking DMA OK Notification.. \n");
        err = ITE_GetEventCount(Event3_Count_u16_EVENT3_0_DMA_GRAB_OK_Byte0);
        
        if (err == 0)
        {
            sprintf(comment, "1 DMA OK event(s) received.\n");
            MMTE_TEST_COMMENT(comment);
            
            result |= SUCCESS;
        }
        else
        {
            sprintf(comment, "Test will fail as no of DMA OK events received != 1\n");
            MMTE_TEST_COMMENT(comment);   
        
            result |= FAILURE;
        }



        //5. Compare DMA OK Frame Id with FrameParamStatus.u32_frame_counter
#if 0   //Enable this iff MMIO supports IspVectorRead_Wrapper
        if (SUCCESS == result)
        {
            IspVectorRead_Wrapper(( unsigned long ) DMA_FRAME_ID_OFFSET, ( const long int * ) (&grabFrameId), 1);
            statsFrameId = ITE_readPE(FrameParamStatus_u32_frame_counter_Byte0);

            if (grabFrameId == statsFrameId)
            {
                MMTE_TEST_COMMENT("Stats Received for the expected frame!\n");
                result |= SUCCESS;
            }
            else
            {
                MMTE_TEST_COMMENT("Mismatch in frame ids of BMS out and Stats: \n");
                sprintf(comment, "DMA Frame Id = %ld\n", grabFrameId);
                MMTE_TEST_COMMENT(comment);
                sprintf(comment, "Statistics Frame Id = %ld\n", statsFrameId);
                MMTE_TEST_COMMENT(comment);
                result |= FAILURE;
            }
        }


#endif
    }


    if (Flag_e_TRUE == enableFlash)
    {
        MMTE_TEST_COMMENT("FlashMode TRUE! \n");
        LOS_Log("\nChecking Flash related status Params.. \n");

        //6. FrameParamStatus.u32_flash_fired (if FlashMode True)
        frameCount = ITE_readPE(FrameParamStatus_u32_flash_fired_Byte0);
        sprintf(comment, "FrameParamStatus Flash Fired = %ld\n", frameCount);
        MMTE_TEST_COMMENT(comment);

        if (1 == frameCount)
        {
            result |= SUCCESS;
        }
        else
        {
            sprintf(comment, "Test will fail as (FrameParamStatus Flash Fired != 1)\n");
            MMTE_TEST_COMMENT(comment);   

            result |= FAILURE;
        }


        //7. FlashStatus.u8_FlashFiredFrameCount (if FlashMode True)
        frameCount = (ITE_readPE(FlashStatus_u8_FlashFiredFrameCount_Byte0));
        sprintf(comment, "FlashStatus FlashFiredFrameCount = %ld\n", frameCount);
        MMTE_TEST_COMMENT(comment);

        if (1 == frameCount)
        {
            result |= SUCCESS;
        }
        else
        {
            sprintf(comment, "Test will fail as (FlashStatus FlashFiredFrameCount != 1)\n");
            MMTE_TEST_COMMENT(comment);   
        
            result |= FAILURE;
        }
    }
    else
    {
        MMTE_TEST_COMMENT("FlashMode FALSE \n");
    }


    return (result);
}


static RESULT_te Flash_enable_pre_streaming(char* datapath)
{
    t_uint32    frameCount=0;
    char        comment[256],fname[256],pathname[256],mess[256];  
    sprintf(fname, "Flash_pre_streaming");
      
    memset(g_out_path,0,KlogDirectoryLentgh*sizeof(char));
    sprintf(pathname,"/ite_nmf/test_results_nreg/flash_tests");

    MMTE_TEST_START(fname, pathname,"Testing for Flash_configure");
  
    Send_System_Params(Flag_e_FALSE, Flag_e_TRUE);  //set flashmode to TRUE
    
    if (0 == strcmp(datapath,"LR"))
    {
        ITE_LR_Prepare(&usecase, INFINITY);
        ITE_LR_Start(INFINITY);
    }
    else if (0 == strcmp(datapath,"HR"))
    { 
        ITE_HR_Prepare(&usecase, INFINITY);
        ITE_HR_Start(INFINITY);
    }
    else if (0 == strcmp(datapath,"BMS"))
    { 
        ITE_BMS_Prepare(&usecase, INFINITY);
        ITE_BMS_Start(INFINITY);
    }
    else  //LRHR
    {
        ITE_LRHR_Prepare(&usecase, INFINITY);
        ITE_LRHR_Start(INFINITY);
    }

    sprintf(mess, "Testing for Flash_pre_streaming_%s\n",datapath);
    MMTE_TEST_NEXT(mess);

    //let first few frames pass..
    LOS_Sleep(1000);

    //currently the implementation in firmware is such that this pe will be updated
    //iff the first frame is a flashlit frame, for this usecase.
    frameCount = ITE_readPE(FlashStatus_u8_FlashFiredFrameCount_Byte0);

    if (0 < frameCount)
    {
        sprintf(comment, "Flash Fired in first Frame!!\n");
        MMTE_TEST_COMMENT(comment);

        if (0 == strcmp(datapath,"BMS"))
        {
          sprintf(comment, "DMA Grab Notify Frame ID:%d\n", (int)ITE_readPE(GrabNotify_u32_DMA_GRAB_Frame_Id_Byte0) );
          MMTE_TEST_COMMENT(comment);
        }

        MMTE_TEST_PASSED();
    }
    else
    {
        sprintf(comment, "Test will fail because first frame is not flash lit.\n");
        MMTE_TEST_COMMENT(comment);
        MMTE_TEST_FAILED();
    }
    
     
    if (0 == strcmp(datapath,"LR"))
    {
        ITE_LR_Stop();
        ITE_LR_Free();
    }
    else if (0 == strcmp(datapath,"HR"))
    { 
        ITE_HR_Stop();
        ITE_HR_Free();
    }
    else if (0 == strcmp(datapath,"BMS"))
    { 
        ITE_BMS_Stop();
        ITE_BMS_Free();
    }
    else  //LRHR
    {
        ITE_LRHR_Stop();
        ITE_LRHR_Free();
    }

    Send_System_Params(Flag_e_FALSE, Flag_e_FALSE);  //cleanup
    return (CMD_COMPLETE);

}


#if FLASH_DRIVER_INCLUDE

//Extern definitions used for stats analysis
extern StatsBuffersStorage_t            *g_pStatsBufferStorage;

/* -----------------------------------------------------------------------
FUNCTION : FlashTest_FlashDriver.
PURPOSE  : This function will test flash functionality with flash driver enabled and then check
                  whether flash has been lit or not by analyzing the histogram stats.
------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_Flash_TestLitFrame_cmd(
int     a_nb_args,
char    **ap_args)
{
    t_uint16 i = 0;
    float f_meanRed = 0.0, f_meanBlue = 0.0, f_meanGreen = 0.0;
    float f_PreGreanMean = 0.0, f_PostGreanMean = 0.0;
    RESULT_te   result = SUCCESS;
    volatile StatsBuffersStorage_t           *pStore;
    char datapath[5];

    MMTE_TEST_START("Flash_TestLitFrame", "/ite_nmf/test_results_nreg/flash_tests", "Flash_TestLitFrame");

    if (a_nb_args != 2)
    {
        CLI_disp_error("Flash_TestLitFrame:  Performs Flash test to check lit frame \n");
        CLI_disp_error("Flash_TestLitFrame[Syntax]: Flash_TestLitFrame <LR/HR/LRHR/BMS> \n\n");

        MMTE_TEST_SKIPPED();
        return (CMD_ERR_ARGS);
    }

    if (Flag_e_FALSE == Is_Flash_Supported())
    {
        MMTE_TEST_SKIPPED();
        return (CMD_COMPLETE);
    }

    strcpy(datapath, ap_args[1]);

    //prepare the selected datapath
    if (0 == strcmp(datapath,"LR"))
    {
        ITE_LR_Prepare(&usecase, INFINITY);
    }
    else if (0 == strcmp(datapath,"HR"))
    {
        ITE_HR_Prepare(&usecase, INFINITY);
    }
    else if (0 == strcmp(datapath,"BMS"))
    {
        ITE_BMS_Prepare(&usecase, INFINITY);
    }
    else  //LRHR
    {
        ITE_LRHR_Prepare(&usecase, INFINITY);
    }


    //1. Pass
    //Set the zoom factor as x1
    ITE_writePE(Zoom_Control_f_SetFOVX_Byte0, 0);

    //Set the frame rate = 30dps
    ITE_SetFrameRateControl(30, 30);

    //Program the Glace/Histogram blocks
    ITE_NMF_SetGlaceBlockFraction(100, 100, 0, 0);
    ITE_NMF_SetHistoBlockFraction(100, 100, 0, 0);
    ITE_NMF_SetGlaceGrid(72, 54);
    ITE_NMF_SetGlaceSatLevel(255, 255, 255);

    pStore = g_pStatsBufferStorage;
    ITE_NMF_UpdateGlaceParam();
    ITE_NMF_UpdateHistoParam();

    //set stats request (witout flash) before streaming
    ITE_NMF_GlaceAndHistoReqNoWait();
    ITE_NMF_GlaceAndHistoExpReqNoWait();

    //Start streamind the datapath
    if (0 == strcmp(datapath,"LR"))
    {
        ITE_LR_Start( INFINITY);
    }
    else if (0 == strcmp(datapath,"HR"))
    {
        ITE_HR_Start( INFINITY);
    }
    else if (0 == strcmp(datapath,"BMS"))
    {
        ITE_BMS_Start( INFINITY);
    }
    else  //LRHR
    {
        ITE_LRHR_Start( INFINITY);
    }

    ITE_NMF_WaitStatReady(ISP_GLACE_STATS_READY);

    //Glace and Histo gram stats recvd without flash
    //Calculate the mean energy from R component
    for(i = 0; i < 256; i++)
    {
        f_meanRed  	+=	*((pStore->apHistR) + i) / (256 - i );
        f_meanBlue 	+=	*((pStore->apHistB) + i) / (256 - i );
        f_meanGreen +=	*((pStore->apHistG) + i) / (256 - i ) ;
    }

    f_PreGreanMean = f_meanGreen;
    LOS_Log("\n Histogram mean energy .. \n");
    LOS_Log("\n Red mean energy - %f \n",f_meanRed);
    LOS_Log("\n Blue mean energy - %f \n",f_meanBlue);
    LOS_Log("\n Green mean energy - %f \n",f_meanGreen);

    //2nd Pass.
    //Request for stats with flash this time
    f_meanRed = 0.0;
    f_meanBlue = 0.0;
    f_meanGreen = 0.0;

    memset(g_pStatsBufferStorage->apHistG, 0, 256 * 4);

    if (0 == strcmp(datapath,"LR"))
    {
        ITE_LR_Stop();
    }
    else if (0 == strcmp(datapath,"HR"))
    {
        ITE_HR_Stop();
    }
    else if (0 == strcmp(datapath,"BMS"))
    {
        ITE_BMS_Stop();
    }
    else  //LRHR
    {
        ITE_LRHR_Stop();
    }

    // Fire flash and and re-calculate the energy
     ITE_NMF_GlaceAndHistoReqNoWait();
     Send_System_Params(Flag_e_TRUE, Flag_e_TRUE);


    if (0 == strcmp(datapath,"LR"))
    {
        ITE_LR_Start( INFINITY);
    }
    else if (0 == strcmp(datapath,"HR"))
    {
        ITE_HR_Start( INFINITY);
    }
    else if (0 == strcmp(datapath,"BMS"))
    {
        ITE_BMS_Start( INFINITY);
    }
    else  //LRHR
    {
        ITE_LRHR_Start( INFINITY);
    }

     ITE_NMF_WaitStatReady(ISP_GLACE_STATS_READY);

    //Glace and Histo gram stats recvd with flash
    //Calculate the mean energy from R component
    for(i = 0; i < 256; i++)
    {
        f_meanRed  	+=	*((pStore->apHistR) + i) / (256 - i );
        f_meanBlue 	+=	*((pStore->apHistB) + i) / (256 - i );
        f_meanGreen +=	*((pStore->apHistG) + i) / (256 - i ) ;
    }

    f_PostGreanMean = f_meanGreen;
    LOS_Log("\n Histogram mean energy .. \n");
    LOS_Log("\n Red mean energy - %f \n",f_meanRed);
    LOS_Log("\n Blue mean energy - %f \n",f_meanBlue);
    LOS_Log("\n Green mean energy - %f \n",f_meanGreen);

    f_meanRed = 0.0;
    f_meanBlue = 0.0;
    f_meanGreen = 0.0;

    if((result == SUCCESS) && (f_PostGreanMean >= f_PreGreanMean))
    {
         MMTE_RESULT_PASSED();
    }
    else
    {
        MMTE_RESULT_FAILED();
    }

    //Stop/clear the test
    if (0 == strcmp(datapath,"LR"))
    {
        ITE_LR_Stop();
        ITE_LR_Free();
    }
    else if (0 == strcmp(datapath,"HR"))
    {
        ITE_HR_Stop();
        ITE_HR_Free();
    }
    else if (0 == strcmp(datapath,"BMS"))
    {
        ITE_BMS_Stop();
        ITE_BMS_Free();
    }
    else  //LRHR
    {
        ITE_LRHR_Stop();
        ITE_LRHR_Free();
    }

    MMTE_TEST_END();
    Send_System_Params(Flag_e_FALSE, Flag_e_FALSE);  //cleanup

    return (CMD_COMPLETE);
}
#endif
