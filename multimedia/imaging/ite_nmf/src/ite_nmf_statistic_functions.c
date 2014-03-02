/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include "ImgConfig.h"
#include "cli.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "ite_nmf_statistic_functions.h"
#include "ite_nmf_standard_functions.h"

#include "ite_testenv_utils.h"

#include "ite_boardinfo.h"
#include "ite_platform.h"
#include "VhcElementDefs.h"

//for NMF
#include "ite_sia_interface_data.h"
#include "ite_init.h"
#include "ite_sia_init.h"

//#include "ite_alloc.h"
//#include "ite_display.h"
#include "ite_event.h"
#include "ite_pageelements.h"
#include "ite_grab.h"
#include "ite_vpip.h"
#include "ite_sia_buffer.h"
#include "ite_host2sensor.h"
#include "ite_sensorinfo.h"
#include "ite_debug.h"
#include "ite_framerate.h"
#include "ite_nmf_focuscontrol_functions.h"
#include "ite_ispblocks_tests.h"
#include "ite_FocusControl_Module.h"
//#include "ite_nmf_tuning_functions.h"
#include <cm/inc/cm_macros.h>
#include <test/api/test.h>
#include <los/api/los_api.h>
#include "ite_main.h"
#include "sia.h"
#include "ite_event.h"

#define ISP_REGION_IO   0xE0000000
#define MIN_DG          1.0
#define MAX_DG          7.99

extern unsigned long long       ITE_NMF_GlaceAndHistoExpReqNoWait (void);
extern void                     ITE_NMF_GlaceHistoExpStatsRequestAndWait (void);
extern int g_Store_IMAGES ;

/* function quick desciption:
 *
 *   StatsBuffersStorage_t* ITE_NMF_createStatsBuffer(tps_siaMetaDatabuffer pBufferMetaData)
 *         create a Metadata buffer describing a StatsBuffersStorage_t buffer
 *         initialise all the structure. write all buffer with 0xff value.
 *         and write PE Glace_Control_p_GlaceStats_Byte0  with good value
 *
 *   void ITE_NMF_freeStatsBuffer(tps_siaMetaDatabuffer pBufferMetaData)
 *        free Metadata buffer
 *
 *   void ITE_NMF_UpdateGlaceParam(void)
 *        write all PE for glace according to Glace_uc value
 *        toggle ParamUpdate count
 *        toggle ControlUpdate Count
 *
 *   void ITE_NMF_GlaceStatsTest(char *pipe)
 *    create a Metadata buffer
 *    UpdateParameter for Glace
 *    launch required pipe runing
 *    wait for notification: ISP_GLACE_STATS_READY
 *
 *   void ITE_NMF_GlaceStatsTestRequestAndWait(void)
 *       toggle ControlUpdatecount PE
 *       and wait for notification: ISP_GLACE_STATS_READY
 *
 *   void ITE_NMF_GlaceStatsTestStop()
 *       stop streaming
 *       free buffer LR/HR or BMS
 *       free metadatbuffer
 *
 *
 */
volatile ts_siaMetaDatabuffer   StatsBuffer;
volatile tps_siaMetaDatabuffer  pStatsBuffer = (tps_siaMetaDatabuffer) & StatsBuffer;

volatile ts_siaMetaDatabuffer   TraceBuffer;
volatile tps_siaMetaDatabuffer  pTraceBuffer = (tps_siaMetaDatabuffer) & TraceBuffer;

volatile ts_siaNVMbuffer        NVMBuffer;
volatile tps_siaNVMbuffer       pNVMStatsBuffer = (tps_siaNVMbuffer) & NVMBuffer;

volatile ts_siaMetaDatabuffer   OutpuModeBuffer;
volatile tps_siaMetaDatabuffer  pOutpuModeBuffer = (tps_siaMetaDatabuffer) & OutpuModeBuffer;

volatile ts_siaGammaLUTbuffer   GammaLUTBuffer;
volatile tps_siaGammaLUTbuffer  pGammaLUTBuffer = (tps_siaGammaLUTbuffer) & GammaLUTBuffer;

//tps_siaFocusStatbuffer pFocusStatsBuffer;
ts_stat_test_usecase            Glace_uc = { 72, 54, 100, 100, 0, 0, 128, 128, 128, 1.0, 1.0, 1.0, 0 };
ts_stat_test_usecase            Histo_uc = { 72, 54, 100, 100, 0, 0, 128, 128, 128, 1.0, 1.0, 1.0, 0 };

tps_stat_test_usecase           pGlace_uc = &Glace_uc;
tps_stat_test_usecase           pHisto_uc = &Histo_uc;

StatsBuffersStorage_t           *pStore;

ts_frameParamStatus             FrameParamStatus;
tps_frameParamStatus            pFrameParamStatus = &FrameParamStatus;

extern ts_sia_usecase           usecase;

extern volatile t_uint32        g_grabLR_count;
extern t_bool                   TraceGrabLREvent;
extern StatsBuffersStorage_t    *g_pStatsBufferStorage;
extern t_uint32                 g_frameparam_counter;

unsigned long long
ITE_NMF_WaitStatReady(
enum e_ispctlInfo   ispctlInfo)
{
    union u_ITE_Event   event;

    do
    {
        event = ITE_GetStatsEvent();
        ITE_traceISPCTLevent(event);
        if (event.ispctlInfo.info_id == ITE_EVT_ISPCTL_ERROR)
        {
            break;
        }

    } while (!(event.ispctlInfo.info_id == ispctlInfo));

    return (LOS_getSystemTime());
}


unsigned long long
ITE_NMF_GlaceAndHistoReqNoWait(void)
{
    if (Coin_e_Heads == ITE_readPE(SystemConfig_Status_e_Coin_Glace_Histogram_Status_Byte0))
    {
        ITE_writePE(SystemSetup_e_Coin_Glace_Histogram_Ctrl_debug_Byte0, Coin_e_Tails);
    }
    else
    {
        ITE_writePE(SystemSetup_e_Coin_Glace_Histogram_Ctrl_debug_Byte0, Coin_e_Heads);
    }


    return (LOS_getSystemTime());
}


void
ITE_NMF_GlaceHistoStatsTestRequestAndWait(void)
{
    unsigned long long  time1,
                        time2;
    time1 = ITE_NMF_GlaceAndHistoReqNoWait();
    time2 = ITE_NMF_WaitStatReady(ISP_GLACE_STATS_READY);
    LOS_Log("Time between Stat Request and Event received: %llu\n", time2 - time1);
}


//Function for glacehistoexp reqowait
unsigned long long
ITE_NMF_GlaceAndHistoExpReqNoWait(void)
{
    if (Coin_e_Heads == ITE_readPE(SystemConfig_Status_e_Coin_Status_Byte0))
    {
        ITE_writePE(SystemSetup_e_Coin_Ctrl_Byte0, Coin_e_Tails);
    }
    else
    {
        ITE_writePE(SystemSetup_e_Coin_Ctrl_Byte0, Coin_e_Heads);
    }


    return (LOS_getSystemTime());
}


//Function for glacehistoexp request cmd
void
ITE_NMF_GlaceHistoExpStatsRequestAndWait(void)
{
    unsigned long long  time1,
                        time2;
    time1 = ITE_NMF_GlaceAndHistoExpReqNoWait();
    time2 = ITE_NMF_WaitStatReady(ISP_GLACE_STATS_READY);
    LOS_Log(
    "Time between Stat Request and Event received: %llu frame number:%d\n",
    time2 - time1,
    pFrameParamStatus->u32_frame_counter);
}


CMD_COMPLETION
C_ite_dbg_createstatbuff_cmd(
int     a_nb_args,
char    **ap_args)
{
    if (ap_args[1] == NULL)
    {
        ap_args[1] = "";    //For Linux, strcmp doesn't support NULL parameter
    }


    if (0 == strcmp(ap_args[1], "help"))
    {
        CLI_disp_error("syntax: createstatbuff \n");
    }
    else
    {
        if (a_nb_args == 1)
        {
            pStore = g_pStatsBufferStorage;
        }
        else
        {
            CLI_disp_error("Not correct command arguments\n");
        }
    }


    return (CMD_COMPLETE);
}


CMD_COMPLETION
C_ite_dbg_test_glacerobustness_cmd(
int     a_nb_args,
char    **ap_args)
{
    if (ap_args[1] == NULL)
    {
        ap_args[1] = "";    //For Linux, strcmp doesn't support NULL parameter
    }


    if (0 == strcmp(ap_args[1], "help"))
    {
        CLI_disp_error("syntax: glacerobustness \n");
    }
    else
    {
        if (a_nb_args == 2)
        {
          if (0== strcmp(ap_args[1],"enable"))\
          {
              g_Store_IMAGES = 1;
              LOS_Log("STE_NOIDA  Store_IMAGES %d \n",g_Store_IMAGES);
          }
          if (0== strcmp(ap_args[1],"disable"))
        {
            g_Store_IMAGES = 0;
            LOS_Log("STE_NOIDA  Store_IMAGES %d \n",g_Store_IMAGES);
          }
          
            ITE_NMF_test_glacerobustness();
        }
        else
        {
            CLI_disp_error("Not correct command arguments\n");
        }
    }


    return (CMD_COMPLETE);
}


CMD_COMPLETION
C_ite_dbg_test_SensorParams_cmd(
int     a_nb_args,
char    **ap_args)
{
    if (ap_args[1] == NULL)
    {
        ap_args[1] = "";    //For Linux, strcmp doesn't support NULL parameter
    }


    if (0 == strcmp(ap_args[1], "help"))
    {
        CLI_disp_error("syntax: sensorparams <loop_count> \n");
    }
    else
    {
        if (a_nb_args == 2)
        {
            ITE_NMF_test_SensorParams(ap_args[1]);
        }
        else
        {
            CLI_disp_error("Not correct command arguments\n");
        }
    }


    return (CMD_COMPLETE);
}


CMD_COMPLETION
C_ite_dbg_test_ISP_Params_cmd(
int     a_nb_args,
char    **ap_args)
{
    if (ap_args[1] == NULL)
    {
        ap_args[1] = "";    //For Linux, strcmp doesn't support NULL parameter
    }


    if (0 == strcmp(ap_args[1], "help"))
    {
        CLI_disp_error("syntax: DG_test <loop_count> \n");
    }
    else
    {
        if (a_nb_args == 2)
        {
            ITE_DigitalGainCheck(ap_args[1]);
        }
        else
        {
            CLI_disp_error("Not correct command arguments\n");
        }
    }


    return (CMD_COMPLETE);
}


CMD_COMPLETION
C_ite_dbg_test_glace_and_focus_stats_cmd(
int     a_nb_args,
char    **ap_args)
{
    int ret =EOK;
    if (ap_args[1] == NULL)
    {
        ap_args[1] = "";    //For Linux, strcmp doesn't support NULL parameter
    }


    if (0 == strcmp(ap_args[1], "help"))
    {
        CLI_disp_error("syntax: glacefocusstats \n");
    }
    else
    {
        if (a_nb_args == 2)
        {
            ret =ITE_NMF_test_glacefocusstats(ap_args[1]);
            if(ret != EOK)
            {
                CLI_disp_error("Not correct command arguments C_ite_dbg_test_glace_and_focus_stats_cmd\n");
                goto END;
            }

        }
        else
        {
            CLI_disp_error("Not correct command arguments\n");
        }
    }

    END:
    return (CMD_COMPLETE);
}


CMD_COMPLETION
C_ite_dbg_test_glace_and_focus_stats_robustness_cmd(
int     a_nb_args,
char    **ap_args)
{
    int ret = EOK;
    if (ap_args[1] == NULL)
    {
        ap_args[1] = "";    //For Linux, strcmp doesn't support NULL parameter
    }


    if (0 == strcmp(ap_args[1], "help"))
    {
        CLI_disp_error("syntax: glacefocusstatsrobustness \n");
    }
    else
    {
        if (a_nb_args == 2)
        {
            ret = ITE_NMF_test_glacefocusstats_robustness(ap_args[1]);
            if(ret != EOK)
            {
                 CLI_disp_error("Not correct command arguments C_ite_dbg_test_glace_and_focus_stats_robustness_cmd\n");
                 goto END;
            }
        }
        else
        {
             CLI_disp_error("Not correct command arguments\n");
             goto END;
        }
    }

    END:
    return (CMD_COMPLETE);
}


/************************************ glace *****************************************************************/
void
ITE_NMF_UpdateGlaceParam(void)
{
    t_uint8         glaceParamCount;

    volatile float  temp;

    temp = ( float ) (Glace_uc.HSizeFraction) / ( float ) 100;
    ITE_writePE(Glace_Control_f_HBlockSizeFraction_Byte0, *( volatile t_uint32 * ) &temp);
    temp = ( float ) (Glace_uc.VSizeFraction) / ( float ) 100;
    ITE_writePE(Glace_Control_f_VBlockSizeFraction_Byte0, *( volatile t_uint32 * ) &temp);
    temp = ( float ) (Glace_uc.HROIFraction) / ( float ) 100;
    ITE_writePE(Glace_Control_f_HROIStartFraction_Byte0, *( volatile t_uint32 * ) &temp);
    temp = ( float ) (Glace_uc.VROIFraction) / ( float ) 100;
    ITE_writePE(Glace_Control_f_VROIStartFraction_Byte0, *( volatile t_uint32 * ) &temp);
    ITE_writePE(Glace_Control_u8_RedSaturationLevel_Byte0, Glace_uc.RedSatLevel);   // NB: sat limit may be up to 16b
    ITE_writePE(Glace_Control_u8_GreenSaturationLevel_Byte0, Glace_uc.GreenSatLevel);
    ITE_writePE(Glace_Control_u8_BlueSaturationLevel_Byte0, Glace_uc.BlueSatLevel);
    ITE_writePE(Glace_Control_u8_HGridSize_Byte0, Glace_uc.gridWidth);
    ITE_writePE(Glace_Control_u8_VGridSize_Byte0, Glace_uc.gridHeight);
    ITE_writePE(Glace_Control_e_GlaceOperationMode_Control_Byte0, GlaceOperationMode_e_Once);
    ITE_writePE(Glace_Control_e_GlaceDataSource_Byte0, GlaceDataSource_e_PostLensShading);

    glaceParamCount = ITE_readPE(Glace_Status_u8_ParamUpdateCount_Byte0);
    ITE_writePE(Glace_Control_u8_ParamUpdateCount_Byte0, ++glaceParamCount);
}


unsigned long long
ITE_NMF_GlaceReqNoWait(void)
{
    t_uint8 glaceControlCount;

    glaceControlCount = ITE_readPE(Glace_Status_u8_ControlUpdateCount_Byte0);
    ITE_writePE(Glace_Control_u8_ControlUpdateCount_debug_Byte0, (++glaceControlCount) & 1);
    return (LOS_getSystemTime());
}


unsigned long long
ITE_NMF_AFReqNoWait(void)
{
    t_uint8 focusControlCoinCount;

    focusControlCoinCount = ITE_readPE(FocusControl_Status_e_Coin_Status_Byte0);
    ITE_writePE(FocusControl_Controls_e_Coin_Control_Byte0, (++focusControlCoinCount) & 1);

    return (LOS_getSystemTime());
}


void
ITE_NMF_UpdateGain(void)
{
    volatile float      temp;
    Coin_te             coin;

    // WB manualmode
    temp = Glace_uc.RedManualGain;
    ITE_writePE(WhiteBalanceControl_f_RedManualGain_Byte0, *( volatile t_uint32 * ) &temp);

    temp = Glace_uc.GreenManualGain;
    ITE_writePE(WhiteBalanceControl_f_GreenManualGain_Byte0, *( volatile t_uint32 * ) &temp);
    temp = Glace_uc.BlueManualGain;
    ITE_writePE(WhiteBalanceControl_f_BlueManualGain_Byte0, *( volatile t_uint32 * ) &temp);

    // AB Modif for 0.36.0  ITE_writePE(WhiteBalanceControl_e_WhiteBalance_Mode_Byte0,WhiteBalance_Mode_e_MANUAL_RGB);
    // AB Modif for 0.36.0  ITE_writePE(Exposure_AlgorithmControls_e_Exposure_AlgorithmMode_Control_Byte0,Exposure_AlgorithmMode_e_DIRECT_MANUAL);
    ITE_writePE(SystemSetup_e_SystemMeteringMode_Control_Byte0, SystemMeteringMode_e_SINGLE_STEP);

    ITE_RefreshEventCount(Event0_Count_u16_EVENT0_19_EXPOSURE_AND_WB_PARAMETERS_UDPATE_Byte0);
    ITE_RefreshEventCount(Event0_Count_u16_EVENT0_17_GLACE_STATS_READY_Byte0);

    if (Coin_e_Heads == ITE_readPE(SystemConfig_Status_e_Coin_Status_Byte0))
    {
        ITE_writePE(SystemSetup_e_Coin_Ctrl_Byte0, Coin_e_Tails);
        coin = Coin_e_Tails;
    }
    else
    {
        ITE_writePE(SystemSetup_e_Coin_Ctrl_Byte0, Coin_e_Heads);
        coin = Coin_e_Heads;
    }

    /* wait IT (ISP_GLACE_STATS_READY) */
    ITE_NMF_WaitStatReady(ISP_GLACE_STATS_READY);
    ITE_GetEventCount(Event0_Count_u16_EVENT0_19_EXPOSURE_AND_WB_PARAMETERS_UDPATE_Byte0);
    ITE_GetEventCount(Event0_Count_u16_EVENT0_17_GLACE_STATS_READY_Byte0);

    // while (coin != ITE_readPE(SystemConfig_Status_e_Coin_Status_Byte0));
    temp = ( float ) coin;  // __NO_WARNING__
}


void
ITE_NMF_UpdateAnalogGain(
t_uint32    AGain)
{
    // Analog gain update
    ITE_writePE(Exposure_DriverControls_u16_TargetAnalogGain_x256_Byte0, AGain);
    ITE_NMF_GlaceHistoExpStatsRequestAndWait();
}


void
ITE_NMF_UpdateDigitalGain(
volatile float  DGain)
{
    // Digital gain update
    ITE_writePE(WhiteBalanceControl_f_RedManualGain_Byte0, *( volatile t_uint32 * ) &DGain);
    ITE_writePE(WhiteBalanceControl_f_GreenManualGain_Byte0, *( volatile t_uint32 * ) &DGain);
    ITE_writePE(WhiteBalanceControl_f_BlueManualGain_Byte0, *( volatile t_uint32 * ) &DGain);

    ITE_NMF_GlaceHistoExpStatsRequestAndWait();
}


unsigned long long
ITE_NMF_UpdateExposureTime(
t_uint32    timeus)
{
    //Control exposure time :
    ITE_writePE(Exposure_DriverControls_u32_TargetExposureTime_us_Byte0, timeus);
    ITE_NMF_GlaceHistoExpStatsRequestAndWait();
    return (0);             //__NO_WARNING__
}


void
ITE_NMF_UpdateAnalogGainAndExposureTime(
t_uint16    AGain,
t_uint32    timeus)
{
        // Analog gain update
    if(AGain!=0xFF)
        {
        LOS_Log("Applying Analog Gain : %d\n", AGain);
        ITE_writePE(Exposure_DriverControls_u16_TargetAnalogGain_x256_Byte0, (t_uint32)AGain);
        }


        // Control exposure time
    if(timeus!=0xFFFF)
        {
        LOS_Log("Applying Exposure Time : %d\n", timeus);
        ITE_writePE(Exposure_DriverControls_u32_TargetExposureTime_us_Byte0, timeus);
        }

}


void
ITE_NMF_GlaceStatsTestRequestAndWait(void)
{
    unsigned long long  time1,
                        time2;
    time1 = ITE_NMF_GlaceReqNoWait();
    time2 = ITE_NMF_WaitStatReady(ISP_GLACE_STATS_READY);
    LOS_Log(
    "Time between Stat Request and Event received: %llu frame count %d\n",
    time2 - time1,
    pFrameParamStatus->u32_frame_counter);
}


CMD_COMPLETION
C_ite_dbg_SetGlaceBlockFraction_cmd(
int     a_nb_args,
char    **ap_args)
{
    if (0 == strcmp(ap_args[1], "help"))
    {
        CLI_disp_error(
        "syntax: glaceblock <HSizeFraction Value> <VSizeFraction Value> <HROIFraction Value> <VROIFraction Value>\n");
    }
    else
    {
        if (a_nb_args == 5)
        {
            ITE_NMF_SetGlaceBlockFraction(
            ITE_ConvToInt32(ap_args[1]),
            ITE_ConvToInt32(ap_args[2]),
            ITE_ConvToInt32(ap_args[3]),
            ITE_ConvToInt32(ap_args[4]));
        }
        else
        {
            CLI_disp_error("Not correct command arguments\n");
        }
    }


    return (CMD_COMPLETE);
}


CMD_COMPLETION
C_ite_dbg_SetHistoBlockFraction_cmd(
int     a_nb_args,
char    **ap_args)
{
    if (ap_args[1] == NULL)
    {
        ap_args[1] = "";    //For Linux, strcmp doesn't support NULL parameter
    }


    if (0 == strcmp(ap_args[1], "help"))
    {
        CLI_disp_error(
        "syntax: histoblock <HSizeFraction Value> <VSizeFraction Value> <HROIFraction Value> <VROIFraction Value>\n");
    }
    else
    {
        if (a_nb_args == 5)
        {
            ITE_NMF_SetHistoBlockFraction(
            ITE_ConvToInt32(ap_args[1]),
            ITE_ConvToInt32(ap_args[2]),
            ITE_ConvToInt32(ap_args[3]),
            ITE_ConvToInt32(ap_args[4]));
        }
        else
        {
            CLI_disp_error("Not correct command arguments\n");
        }
    }


    return (CMD_COMPLETE);
}


void
ITE_NMF_SetGlaceBlockFraction(
t_uint8 HSizeFraction,
t_uint8 VSizeFraction,
t_uint8 HROIFraction,
t_uint8 VROIFraction)
{
    Glace_uc.HSizeFraction = HSizeFraction;
    Glace_uc.VSizeFraction = VSizeFraction;
    Glace_uc.HROIFraction = HROIFraction;
    Glace_uc.VROIFraction = VROIFraction;
}


void
ITE_NMF_SetHistoBlockFraction(
t_uint8 HSizeFraction,
t_uint8 VSizeFraction,
t_uint8 HROIFraction,
t_uint8 VROIFraction)
{
    Histo_uc.HSizeFraction = HSizeFraction;
    Histo_uc.VSizeFraction = VSizeFraction;
    Histo_uc.HROIFraction = HROIFraction;
    Histo_uc.VROIFraction = VROIFraction;
}


CMD_COMPLETION
C_ite_dbg_SetGlaceGrid_cmd(
int     a_nb_args,
char    **ap_args)
{
    if (0 == strcmp(ap_args[1], "help"))
    {
        CLI_disp_error("syntax: glacegridsize <gridWidth Value> <gridHeight Value>\n");
    }
    else
    {
        if (a_nb_args == 3)
        {
            ITE_NMF_SetGlaceGrid(ITE_ConvToInt8(ap_args[1]), ITE_ConvToInt8(ap_args[2]));
        }
        else
        {
            CLI_disp_error("Not correct command arguments\n");
        }
    }


    return (CMD_COMPLETE);
}


void
ITE_NMF_SetGlaceGrid(
t_uint8 gridWidth,
t_uint8 gridHeight)
{
    Glace_uc.gridWidth = gridWidth;
    Glace_uc.gridHeight = gridHeight;
}


CMD_COMPLETION
C_ite_dbg_SetGlaceSatLevel_cmd(
int     a_nb_args,
char    **ap_args)
{
    if (0 == strcmp(ap_args[1], "help"))
    {
        CLI_disp_error("syntax: glacesat <Red Sat Value> <Green Sat Value> <Blue Sat Value>\n");
    }
    else
    {
        if (a_nb_args == 4)
        {
            ITE_NMF_SetGlaceSatLevel(
            ITE_ConvToInt8(ap_args[1]),
            ITE_ConvToInt8(ap_args[2]),
            ITE_ConvToInt8(ap_args[3]));
        }
        else
        {
            CLI_disp_error("Not correct command arguments\n");
        }
    }


    return (CMD_COMPLETE);
}


void
ITE_NMF_SetGlaceSatLevel(
t_uint8 RedSatLevel,
t_uint8 GreenSatLevel,
t_uint8 BlueSatLevel)
{
    Glace_uc.RedSatLevel = RedSatLevel;
    Glace_uc.GreenSatLevel = GreenSatLevel;
    Glace_uc.BlueSatLevel = BlueSatLevel;
}


CMD_COMPLETION
C_ite_dbg_SetStatWBManualMode_cmd(
int     a_nb_args,
char    **ap_args)
{
    if (0 == strcmp(ap_args[1], "help"))
    {
        CLI_disp_error("syntax: glacewbgain <Red manual Value> <Green manual Value> <Blue manual Value>\n");
    }
    else
    {
        if (a_nb_args == 4)
        {
            ITE_NMF_SetStatWBManualMode(
            ITE_ConvToFloat(ap_args[1]),
            ITE_ConvToFloat(ap_args[2]),
            ITE_ConvToFloat(ap_args[3]));
        }
        else
        {
            CLI_disp_error("Not correct command arguments\n");
        }
    }


    return (CMD_COMPLETE);
}


void
ITE_NMF_SetStatWBManualMode(
float   RedManualGain,
float   GreenManualGain,
float   BlueManualGain)
{
    Glace_uc.RedManualGain = RedManualGain;
    Glace_uc.GreenManualGain = GreenManualGain;
    Glace_uc.BlueManualGain = BlueManualGain;
}


CMD_COMPLETION
C_ite_dbg_GlaceDump_cmd(
int     a_nb_args,
char    **ap_args)
{
    if (ap_args[1] == NULL)
    {
        ap_args[1] = "";    //For Linux, strcmp doesn't support NULL parameter
    }


    if (0 == strcmp(ap_args[1], "help"))
    {
        CLI_disp_error("syntax: glacedump \n");
    }
    else
    {
        if (a_nb_args == 1)
        {
            ITE_NMF_DumpGlaceBuffer();
        }
        else
        {
            CLI_disp_error("Not correct command arguments\n");
        }
    }


    return (CMD_COMPLETE);
}


void
ITE_NMF_ResetISPBufferPointers(void)
{
    ITE_writePE(Glace_Control_ptrGlace_Statistics_Byte0, (t_uint32) (0));

    ITE_writePE(HistStats_Ctrl_ptru32_HistRAddr_Byte0, (t_uint32) (0));
    ITE_writePE(HistStats_Ctrl_ptru32_HistGAddr_Byte0, (t_uint32) (0));
    ITE_writePE(HistStats_Ctrl_ptru32_HistBAddr_Byte0, (t_uint32) (0));

    ITE_writePE(FrameParamStatus_ptru32_SensorParametersTargetAddress_Byte0, (t_uint32) (0));
}


void
ITE_NMF_ResetFocusBufferPointers(void)
{
    ITE_writePE(AFStats_Controls_pu32_HostAssignedAddr_Byte0, (t_uint32) (0));
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_glacestattest_cmd
   PURPOSE  : launch basic glace statistic test
   ------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_glacestattest_cmd(
int     a_nb_args,
char    **ap_args)
{
    if (0 == strcmp(ap_args[1], "help"))
    {
        CLI_disp_error("syntax: glace <update/req/reqnowait/waitstatready/freebuff/continous [pipe]> \n");
    }
    else
    {
        if (a_nb_args == 2)
        {
            if (0 == strcmp(ap_args[1], "update"))
            {
                ITE_NMF_UpdateGlaceParam();
            }


            if (0 == strcmp(ap_args[1], "req"))
            {
                ITE_NMF_GlaceStatsTestRequestAndWait();
            }


            if (0 == strcmp(ap_args[1], "freebuff"))
            {
#if 0
                // Glace Crash if someone calls this by mistake!
                ITE_NMF_freeStatsBuffer(pStatsBuffer);
#endif
                ITE_NMF_ResetISPBufferPointers();
            }


            if (0 == strcmp(ap_args[1], "reqnowait"))
            {
                ITE_NMF_GlaceReqNoWait();
            }


            if (0 == strcmp(ap_args[1], "waitstatready"))
            {
                ITE_NMF_WaitStatReady(ISP_GLACE_STATS_READY);
            }

            if (0 == strcmp(ap_args[1], "null"))
            {   // just for robusness test
                ITE_writePE(Glace_Control_ptrGlace_Statistics_Byte0, 0);
            }
        }
        else if(a_nb_args == 3)
         {
           if (0 == strcmp(ap_args[1], "continous"))
            {
             ITE_NMF_Continous_Glace_Test(ap_args[2]);
            }
         }
        else
        {
            CLI_disp_error("Not correct command arguments\n");
        }
    }


    return (CMD_COMPLETE);
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_glacehistostattest_cmd
   PURPOSE  : req new glace and histo statistic
   ------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_glacehistostattest_cmd(
int     a_nb_args,
char    **ap_args)
{
    if (0 == strcmp(ap_args[1], "help"))
    {
        CLI_disp_error("syntax: glacehisto <req/reqnowait/waitstatready> \n");
    }
    else
    {
        if (a_nb_args == 2)
        {
            if (0 == strcmp(ap_args[1], "req"))
            {
                ITE_NMF_GlaceHistoStatsTestRequestAndWait();
            }


            if (0 == strcmp(ap_args[1], "reqnowait"))
            {
                ITE_NMF_GlaceAndHistoReqNoWait();
            }


            if (0 == strcmp(ap_args[1], "waitstatready"))
            {
                ITE_NMF_WaitStatReady(ISP_HISTOGRAM_STATS_READY);
            }
        }
        else
        {
            CLI_disp_error("Not correct command arguments\n");
        }
    }


    return (CMD_COMPLETE);
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_glacehistoexp_cmd
   PURPOSE  : req glscehisto and exp stats
   ------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_glacehistoexp_cmd(
int     a_nb_args,
char    **ap_args)
{
    if (0 == strcmp(ap_args[1], "help"))
    {
        CLI_disp_error("syntax: glacehistoexp<req/reqnowait/waitstatready> \n");
    }
    else
    {
        if (a_nb_args == 2)
        {
            if (0 == strcmp(ap_args[1], "req"))
            {
                ITE_NMF_GlaceHistoExpStatsRequestAndWait();
            }


            if (0 == strcmp(ap_args[1], "reqnowait"))
            {
                ITE_NMF_GlaceAndHistoExpReqNoWait();
            }


            if (0 == strcmp(ap_args[1], "waitstatready"))
            {
                ITE_NMF_WaitStatReady(ISP_GLACE_STATS_READY);
            }
        }
        else
        {
            CLI_disp_error("Not correct command arguments\n");
        }
    }


    return (CMD_COMPLETE);
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_updategaintest_cmd
   PURPOSE  : update Gain and automatically wait new glace and histo return event
   ------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_updategaintest_cmd(
int     a_nb_args,
char    **ap_args)
{
    t_uint16    i;
    if (0 == strcmp(ap_args[1], "help"))
    {
        CLI_disp_error("syntax: updategain req \n");
    }
    else
    {
        if (a_nb_args == 2)
        {
            if (0 == strcmp(ap_args[1], "req"))
            {
                ITE_NMF_UpdateGain();
            }
        }
        else if (a_nb_args == 3)
        {
            if (0 == strcmp(ap_args[1], "req"))
            {
                for (i = 0; i < ITE_ConvToInt16(ap_args[2]); i++)
                {
                    ITE_NMF_UpdateGain();
                    LOS_Log("loop number: %d\n", i);
                }
            }
        }
        else
        {
            CLI_disp_error("Not correct command arguments\n");
        }
    }


    return (CMD_COMPLETE);
}


void
ITE_NMF_DumpGlaceBuffer(void)
{
    t_los_file      *out_fileR;
    t_los_file      *out_fileG;
    t_los_file      *out_fileB;
    t_los_file      *out_fileS;

    char            Red_filename[100] = IMAGING_PATH "/ite_nmf/test_results_nreg/GlaceRed";
    char            Green_filename[100] = IMAGING_PATH "/ite_nmf/test_results_nreg/GlaceGreen";
    char            Blue_filename[100] = IMAGING_PATH "/ite_nmf/test_results_nreg/GlaceBlue";
    char            Sat_filename[100] = IMAGING_PATH "/ite_nmf/test_results_nreg/GlaceSat";

    static t_uint32 Nb = 0;
    char            extension[8] = "";

    snprintf(extension, sizeof(extension), "%d.a", ( int ) Nb); //__NO_WARNING__
    Nb++;
    strcat(Red_filename, extension);
    strcat(Green_filename, extension);
    strcat(Blue_filename, extension);
    strcat(Sat_filename, extension);
    out_fileR = LOS_fopen(Red_filename, "wb+");
    out_fileG = LOS_fopen(Green_filename, "wb+");
    out_fileB = LOS_fopen(Blue_filename, "wb+");
    out_fileS = LOS_fopen(Sat_filename, "wb+");

    // Added for making it re-entrant
    pStore = g_pStatsBufferStorage;
    if (pStore == NULL)
    {
        LOS_Log("ERROR: ITE_NMF_DumpGlaceBuffer: Stats Buffer Null");
        return;
    }


    LOS_fwrite((t_uint8 *) (pStore->pGridR), Glace_uc.gridWidth * Glace_uc.gridHeight, 1, out_fileR);
    LOS_fclose(out_fileR);
    LOS_fwrite((t_uint8 *) (pStore->pGridG), Glace_uc.gridWidth * Glace_uc.gridHeight, 1, out_fileG);
    LOS_fclose(out_fileG);
    LOS_fwrite((t_uint8 *) (pStore->pGridB), Glace_uc.gridWidth * Glace_uc.gridHeight, 1, out_fileB);
    LOS_fclose(out_fileB);
    LOS_fwrite((t_uint8 *) (pStore->pGridS), Glace_uc.gridWidth * Glace_uc.gridHeight, 1, out_fileS);
    LOS_fclose(out_fileS);
    ITE_NMF_Print_frameparamstatus_buffer();
    LOS_Log("output file %s %s %s %s written\n", Red_filename, Green_filename, Blue_filename, Sat_filename);
}


void
ITE_NMF_Print_frameparamstatus_buffer(void)
{
    LOS_Log("ExposureTime_us: %d\n", (pFrameParamStatus->u32_ExposureTime_us));

    LOS_Log("AnalogGain_x256:%d\n", (pFrameParamStatus->u32_AnalogGain_x256));

    LOS_Log("RedGain_x1000:%d\n", (pFrameParamStatus->u32_RedGain_x1000));

    LOS_Log("GreenGain_x1000:%d\n", (pFrameParamStatus->u32_GreenGain_x1000));

    LOS_Log("u32_BlueGain_x1000:%d\n", (pFrameParamStatus->u32_BlueGain_x1000));

    LOS_Log("u32_frame_counter:%d\n", (pFrameParamStatus->u32_frame_counter));

    LOS_Log(" u32_frameRate_x100:%d\n", (pFrameParamStatus->u32_frameRate_x100));
}


void
DisplayAllFrameParamStatus(void)
{
    LOS_Log("u32_ExposureTime_us                     = %d \n", pFrameParamStatus->u32_ExposureTime_us);
    LOS_Log("u32_AnalogGain_x256                     = 0x%X \n", pFrameParamStatus->u32_AnalogGain_x256);
    LOS_Log("u32_RedGain_x1000                       = 0x%X \n", pFrameParamStatus->u32_RedGain_x1000);
    LOS_Log("u32_GreenGain_x1000                     = 0x%X \n", pFrameParamStatus->u32_GreenGain_x1000);
    LOS_Log("u32_BlueGain_x1000                      = 0x%X \n", pFrameParamStatus->u32_BlueGain_x1000);
    LOS_Log("u32_frame_counter                       = %d \n", pFrameParamStatus->u32_frame_counter);
    LOS_Log("u32_frameRate_x100                      = 0x%X \n", pFrameParamStatus->u32_frameRate_x100);
    LOS_Log("u32_flash_fired                         = 0x%X \n", pFrameParamStatus->u32_flash_fired);
    LOS_Log("u32_NDFilter_Transparency_x100          = 0x%X \n", pFrameParamStatus->u32_NDFilter_Transparency_x100);
    LOS_Log("u32_Flag_NDFilter                       = 0x%X \n", pFrameParamStatus->u32_Flag_NDFilter);
    LOS_Log("u32_ExposureQuantizationStep_us         = %d \n", pFrameParamStatus->u32_ExposureQuantizationStep_us);
    LOS_Log("u32_ActiveData_ReadoutTime_us           = %d \n", pFrameParamStatus->u32_ActiveData_ReadoutTime_us);
    LOS_Log("u32_SensorExposureTimeMin_us            = %d \n", pFrameParamStatus->u32_SensorExposureTimeMin_us);
    LOS_Log("u32_SensorExposureTimeMax_us            = %d \n", pFrameParamStatus->u32_SensorExposureTimeMax_us);
    LOS_Log("u32_applied_f_number_x100               = 0x%X \n", pFrameParamStatus->u32_applied_f_number_x_100);
    LOS_Log(
    "u32_SensorParametersAnalogGainMin_x256  = 0x%X \n",
    pFrameParamStatus->u32_SensorParametersAnalogGainMin_x256);
    LOS_Log(
    "u32_SensorParametersAnalogGainMax_x256  = 0x%X \n",
    pFrameParamStatus->u32_SensorParametersAnalogGainMax_x256);
    LOS_Log(
    "u32_SensorParametersAnalogGainStep_x256 = 0x%X \n",
    pFrameParamStatus->u32_SensorParametersAnalogGainStep_x256);
}


void
ITE_NMF_DisplayFrameParamStatus(void)
{
    //   char mess[256];
    static t_uint32 ParamStatusNb = 0;
    t_uint32        rev1,
                    rev2,
                    rev3,
                    rev4,
                    rev5,
                    rev6,
                    rev7,
                    f_param1,
                    f_param2,
                    f_param3;
    char            mess[255],
                    filename[256],
                    pathname[256];
    t_uint32        fout0,
                    fout1;
    t_uint32        diff0 = 0;
    t_uint32        diff1 = 0;
    t_uint32        diff2 = 0;
    t_uint32        diff3 = 0;

    // <Hem> Removing below variable as it is not used.
    // Result_te       result = Result_e_Success;
    sprintf(filename, "FrameParamStatus");
    sprintf(pathname, IMAGING_PATH "/ite_nmf/test_results_nreg/FrameParamStatus");
    strcat(g_out_path, pathname);
    init_test_results_nreg(g_out_path);
    rev1 = ITE_readPE(FrameParamStatus_u32_ExposureTime_us_Byte0);
    rev2 = ITE_readPE(FrameParamStatus_u32_AnalogGain_x256_Byte0);
    rev3 = ITE_readPE(FrameParamStatus_u32_frameRate_x100_Byte0);
    rev4 = ITE_readPE(FrameParamStatus_u32_frame_counter_Byte0);
    rev5 = rev1 + rev2 + rev4;
    sprintf(mess,"_ExposureTime%d_AnalogGain%d_frameRate%d_%drandom_%dcounter",( int ) rev1,( int ) rev2,( int ) rev3 / 100,( int ) rev5,( int ) g_frameparam_counter);

    strcat(filename, mess);

    mmte_testStart(filename, "FrameParamStatus", g_out_path);
    sprintf(mess, "Testing for FrameParamStatus");
    mmte_testNext(mess);

    LOS_Log("Parameters   |   EXPOSURE   |   ANALOG GAIN   |   FRAMERATE \n");
    mmte_testComment("Parameters   |   EXPOSURE   |   ANALOG GAIN   |   FRAMERATE \n");
    rev1 = ITE_readPE(Exposure_DriverControls_u32_TargetExposureTime_us_Byte0);
    rev2 = ITE_readPE(Exposure_DriverControls_u16_TargetAnalogGain_x256_Byte0);
    rev3 = ITE_FrameRateProgrammed();

    LOS_Log("Requested|  %d          |       %d             |    %f         \n", ( int ) rev1, ( int ) rev2,*( float * ) &rev3);
    snprintf(mess,sizeof(mess),"Requested|%d|%d|%f\n", ( int ) rev1, ( int ) rev2,*( float * ) &rev3);
    mmte_testComment(mess);

    rev1 = ITE_readPE(Exposure_ParametersApplied_u32_TotalIntegrationTime_us_Byte0);
    rev2 = ITE_readPE(Exposure_ParametersApplied_u16_AnalogGain_x256_Byte0);
    rev3 = ITE_readPE(FrameDimensionStatus_f_CurrentFrameRate_Byte0);
    LOS_Log("Parameters   |   EXPOSURE   |   ANALOG GAIN   |   FRAMERATE \n");
    LOS_Log("Feedback|%d|%d|%f\n", ( int ) rev1, ( int ) rev2,( float * ) &rev3);
    sprintf(mess,"Feedback|%d|%d|%f\n",( int ) rev1, ( int ) rev2,*( float * ) &rev3);
    mmte_testComment(mess);
    rev3 = ITE_readPE(FrameParamStatus_u32_frameRate_x100_Byte0);
    LOS_Log("Parameters   |   EXPOSURE   |   ANALOG GAIN   |   FRAMERATE |FRAME COUNTER\n");
    LOS_Log("Sensor|%d|%d|%d|%d\n",( int ) pFrameParamStatus->u32_ExposureTime_us,( int ) pFrameParamStatus->u32_AnalogGain_x256,( int )pFrameParamStatus->u32_frameRate_x100, (int)pFrameParamStatus->u32_frame_counter);

    f_param1 = pFrameParamStatus->u32_ExposureTime_us;
    f_param2 = pFrameParamStatus->u32_AnalogGain_x256;
    f_param3 = pFrameParamStatus->u32_frame_counter;
     if
    (
        (f_param1 == rev1)
    &&  (f_param2 == rev2)
    &&  (f_param3 == rev4)
    
    )
    {
       
        LOS_Log("Test case passed \n");
        LOS_Log("\nTEST PASSED\n\n");
        sprintf(mess,
        "Sensor |%d | %d |%d\n",
        ( int ) rev1,
        ( int ) rev2,
        ( int ) rev3);
        mmte_testComment(mess);
        sprintf(
        mess,
        "ExposureTime_us %d = %d , AnalogGain_x256 %d = %d, frame_counter %d = %d\n",
        ( int ) f_param1,
        ( int ) rev1,
        ( int ) f_param2,
        ( int ) rev2,
        ( int ) f_param3,
        ( int ) rev4);
        mmte_testComment(mess);
        LOS_Log(
        "ExposureTime_us expected %d => %d , AnalogGain_x256 %d => %d, frame_counter %d => %d\n",
        ( int ) f_param1,
        ( int ) rev1,
        ( int ) f_param2,
        ( int ) rev2,
        ( int ) f_param3,
        ( int ) rev4);
              
        mmte_testResult(TEST_PASSED);
       
    }
    else
    {
        
        LOS_Log("Test case failed \n");
        LOS_Log("\nTEST FAILED\n\n");
        sprintf(mess,
        "Sensor |%d | %d |%d\n",
        ( int ) rev1,
        ( int ) rev2,
        ( int ) rev3);
        mmte_testComment(mess);
        sprintf(
        mess,
        "ExposureTime_us expected %d => %d , AnalogGain_x256 %d => %d, frame_counter %d => %d\n",
        ( int ) f_param1,
        ( int ) rev1,
        ( int ) f_param2,
        ( int ) rev2,
        ( int ) f_param3,
        ( int ) rev4);
        mmte_testComment(mess);
        LOS_Log(
        "ExposureTime_us expected %d => %d , AnalogGain_x256 %d => %d, frame_counter %d => %d\n",
        ( int ) f_param1,
        ( int ) rev1,
        ( int ) f_param2,
        ( int ) rev2,
        ( int ) f_param3,
        ( int ) rev4);

        mmte_testResult(TEST_FAILED);
        
    }


    mmte_testNext("Testing for FPS");

  
    fout0 = (t_uint32) ITE_dbg_getframerate("LR");
    fout1 = (t_uint32) ITE_dbg_getframerate("HR");

      sprintf(
    mess,
    " expected fps @LRHR => %d , fps received @LR = %d fps received @HR = %d \n",
    ( int ) (rev3 / 100),
    ( int ) fout0,
    ( int ) fout1);
    mmte_testComment(mess);
    
    LOS_Log(
    " expected fps @LRHR => %d , fps received @LR = %d fps received @HR = %d \n",
    ( int ) (rev3 / 100),
    ( int ) fout0,
    ( int ) fout1);

     
    rev6 = ITE_readPE(FrameParamStatus_u32_SensorExposureTimeMin_us_Byte0);
    rev7 = ITE_readPE(FrameParamStatus_u32_SensorExposureTimeMax_us_Byte1);

    LOS_Log("  SensorExposureTimeMin_us       %d         |  SensorExposureTimeMax_us      %d \n",( int ) rev6, ( int ) rev7);
    sprintf(mess, "SensorExposureTimeMin_us %d|SensorExposureTimeMax_us %d \n",( int ) rev6, ( int ) rev7);
    mmte_testComment(mess);

   diff0 = fout0 - (rev3 / 100);
   diff1 = (rev3 / 100) - fout0 ;

   diff2 = fout1 - (rev3 / 100);
   diff3 = (rev3 / 100) - fout1;

    if ((diff0 || diff1 <= 1) && (diff2 || diff3 <= 1))
    {
               
        LOS_Log("\nTEST PASSED\n\n");
        mmte_testComment("TEST PASSED");
        mmte_testResult(TEST_PASSED);
    }
    else
    {
        
        LOS_Log("\nTEST FAILED FRAME RATES NOT MATCHING!!\n\n");
        mmte_testComment("TEST FAILED FRAME RATES NOT MATCHING!!");
        sprintf(mess, "fps @ LR = %d fps @ HR =%d Fps From PE =%d", ( int ) fout0, ( int ) fout1, ( int ) (rev3/100));
        mmte_testComment(mess);
        mmte_testResult(TEST_FAILED);
    }

   

   
    DisplayAllFrameParamStatus();

    mmte_testEnd();
    memset(g_out_path, 0, KlogDirectoryLentgh * sizeof(char));
    ParamStatusNb++;
}



/***************************************************** histo ************************************************************************************/
void
ITE_NMF_UpdateHistoParam(void)
{
    volatile float  temp;

    // Histogram settings
    ITE_writePE(HistStats_Ctrl_e_HistogramMode_Byte0, HistogramMode_e_ONCE);
    ITE_writePE(HistStats_Ctrl_e_HistInputSrc_Byte0, HistInputSrc_e_PostLensShading);

    //// NB: NIPS bug ROI not expressed in %, but in 1/100%
    temp = ( float ) (Histo_uc.HSizeFraction) / ( float ) 100;
    ITE_writePE(HistStats_Ctrl_f_HistSizeRelativeToFOV_X_Byte0, *( volatile t_uint32 * ) &temp);
    temp = ( float ) (Histo_uc.VSizeFraction) / ( float ) 100;
    ITE_writePE(HistStats_Ctrl_f_HistSizeRelativeToFOV_Y_Byte0, *( volatile t_uint32 * ) &temp);
    temp = ( float ) (Histo_uc.HROIFraction) / ( float ) 100;
    ITE_writePE(HistStats_Ctrl_f_HistOffsetRelativeToFOV_X_Byte0, *( volatile t_uint32 * ) &temp);
    temp = ( float ) (Histo_uc.VROIFraction) / ( float ) 100;
    ITE_writePE(HistStats_Ctrl_f_HistOffsetRelativeToFOV_Y_Byte0, *( volatile t_uint32 * ) &temp);

    ITE_writePE(HistStats_Ctrl_u8_HistPixelInputShift_Byte0, 0);    // Implicitly defines the number of bins: 0-shift => 256 bins
}


unsigned long long
ITE_NMF_HistoReqNoWait(void)
{
    t_uint8 HistoControlCount;

    HistoControlCount = ITE_readPE(HistStats_Status_e_CoinStatus_Byte0);
    ITE_writePE(HistStats_Ctrl_e_CoinCtrl_debug_Byte0, (++HistoControlCount) & 1);
    return (LOS_getSystemTime());
}


void
ITE_NMF_HistoStatsTestRequestAndWait(void)
{
    unsigned long long  time1,
                        time2;

    time1 = ITE_NMF_HistoReqNoWait();
    time2 = ITE_NMF_WaitStatReady(ISP_HISTOGRAM_STATS_READY);
    LOS_Log("Time between Stat Request and Event received: %llu\n", time2 - time1);
}


CMD_COMPLETION
C_ite_dbg_HistoDump_cmd(
int     a_nb_args,
char    **ap_args)
{
    if (ap_args[1] == NULL)
    {
        ap_args[1] = "";    //For Linux, strcmp doesn't support NULL parameter
    }


    if (0 == strcmp(ap_args[1], "help"))
    {
        CLI_disp_error("syntax: histodump \n");
    }
    else
    {
        if (a_nb_args == 1)
        {
            ITE_NMF_DumpHistoBuffer();
        }
        else
        {
            CLI_disp_error("Not correct command arguments\n");
        }
    }


    return (CMD_COMPLETE);
}


CMD_COMPLETION
C_ite_dbg_CheckHisto_cmd(
int     a_nb_args,
char    **ap_args)
{
    if (ap_args[1] == NULL)
    {
        ap_args[1] = "";    //For Linux, strcmp doesn't support NULL parameter
    }


    if (0 == strcmp(ap_args[1], "help"))
    {
        CLI_disp_error("syntax: checkhisto \n");
    }
    else
    {
        if (a_nb_args == 1)
        {
            ITE_NMF_CheckHistoBuffer();
        }
        else
        {
            CLI_disp_error("Not correct command arguments\n");
        }
    }


    return (CMD_COMPLETE);
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_histostattest_cmd
   PURPOSE  : launch basic glace statistic test
   ------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_histostattest_cmd(
int     a_nb_args,
char    **ap_args)
{
    if (0 == strcmp(ap_args[1], "help"))
    {
        CLI_disp_error("syntax: histo <update/req/reqnowait/waitstatready/freebuff>\n");
    }
    else
    {
        if (a_nb_args == 2)
        {
            if (0 == strcmp(ap_args[1], "update"))
            {
                ITE_NMF_UpdateHistoParam();
            }


            if (0 == strcmp(ap_args[1], "req"))
            {
                ITE_NMF_HistoStatsTestRequestAndWait();
            }


            if (0 == strcmp(ap_args[1], "freebuff"))
            {
                ITE_NMF_ResetISPBufferPointers();
            }


            if (0 == strcmp(ap_args[1], "reqnowait"))
            {
                ITE_NMF_HistoReqNoWait();
            }


            if (0 == strcmp(ap_args[1], "waitstatready"))
            {
                ITE_NMF_WaitStatReady(ISP_HISTOGRAM_STATS_READY);
            }


            if (0 == strcmp(ap_args[1], "null"))
            {
                ITE_writePE(HistStats_Ctrl_ptru32_HistRAddr_Byte0, (t_uint32) (0));
                ITE_writePE(HistStats_Ctrl_ptru32_HistGAddr_Byte0, (t_uint32) (0));
                ITE_writePE(HistStats_Ctrl_ptru32_HistBAddr_Byte0, (t_uint32) (0));
            }
        }
        else
        {
            CLI_disp_error("Not correct command arguments\n");
        }
    }


    return (CMD_COMPLETE);
}


void
ITE_NMF_CheckHistoBuffer(void)
{
    t_uint32    Rsum = 0;
    t_uint32    Gsum = 0;
    t_uint32    Bsum = 0;
    t_uint32    i;

    for (i = 0; i < 256; i++)
    {
        Rsum = Rsum + (pStore->apHistR[i]);
        Gsum = Gsum + (pStore->apHistG[i]);
        Bsum = Bsum + (pStore->apHistB[i]);
    }


    LOS_Log("Red pixel number: %d\nGreen pixel number: %d\nBlue pixel number: %d\n", Rsum, Gsum, Bsum);
}


void
ITE_NMF_DumpHistoBuffer(void)
{
    t_los_file      *out_fileR;
    t_los_file      *out_fileG;
    t_los_file      *out_fileB;
#ifndef __ARM_SYMBIAN
    char            Red_filename[100] = IMAGING_PATH "/ite_nmf/test_results_nreg/HistoRed";
    char            Green_filename[100] = IMAGING_PATH "/ite_nmf/test_results_nreg/HistoGreen";
    char            Blue_filename[100] = IMAGING_PATH "/ite_nmf/test_results_nreg/HistoBlue";
#else
    char            Red_filename[100] = "F:/imaging/ite_nmf/test_results_nreg/HistoRed";
    char            Green_filename[100] = "F:/imaging/ite_nmf/test_results_nreg/HistoGreen";
    char            Blue_filename[100] = "F:/imaging/ite_nmf/test_results_nreg/HistoBlue";
#endif
    static t_uint32 Nb = 0;
    char            extension[8] = "";

    snprintf(extension, sizeof(extension), "%d.a", ( int ) Nb); //__NO_WARNING__
    Nb++;
    strcat(Red_filename, extension);
    strcat(Green_filename, extension);
    strcat(Blue_filename, extension);
    out_fileR = LOS_fopen(Red_filename, "wb+");
    out_fileG = LOS_fopen(Green_filename, "wb+");
    out_fileB = LOS_fopen(Blue_filename, "wb+");

    LOS_fwrite((t_uint8 *) (pStore->apHistR), 256 * 4, 1, out_fileR);
    LOS_fclose(out_fileR);
    LOS_fwrite((t_uint8 *) (pStore->apHistG), 256 * 4, 1, out_fileG);
    LOS_fclose(out_fileG);
    LOS_fwrite((t_uint8 *) (pStore->apHistB), 256 * 4, 1, out_fileB);
    LOS_fclose(out_fileB);

    LOS_Log("output file %s %s %s  written\n", Red_filename, Green_filename, Blue_filename);
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_GlaceTimeCheck_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_GlaceTimeCheck_cmd(
int     a_nb_args,
char    **ap_args)
{
    if (0 == strcmp(ap_args[1], "help"))
    {
        CLI_disp_error("syntax: glacetimecheck <LR/HR/BMS>\n");
    }
    else
    {
        if (a_nb_args == 2)
        {
            ITE_NMF_GlaceTimeCheck(ap_args[1]);
        }
        else
        {
            CLI_disp_error("Not correct command arguments\n");
        }
    }


    return (CMD_COMPLETE);
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_Update_Exposure_Time_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_Update_Exposure_Time_cmd(
int     a_nb_args,
char    **ap_args)
{
    if (0 == strcmp(ap_args[1], "help"))
    {
        CLI_disp_error("syntax: updatexptime <time>\n");
    }
    else
    {
        if (a_nb_args == 2)
        {
            ITE_NMF_UpdateExposureTime_cmd(ap_args[1]);
        }
        else
        {
            CLI_disp_error("Not correct command arguments\n");
        }
    }


    return (CMD_COMPLETE);
}


/************************************************* histo ******************************************************************/
CMD_COMPLETION
C_ite_dbg_glacehelp_cmd(
int     a_nb_args,
char    **ap_args)
{
    if (ap_args[1] == NULL)
    {
        ap_args[1] = "";        //For Linux, strcmp doesn't support NULL parameter
    }


    if (a_nb_args == 1)
    {
        CLI_disp_error("\n");
        CLI_disp_error("glace         : launch Glace test\n");
        CLI_disp_error("            glace <update/req/reqnowait/waitstatready/freebuff>\n");
        CLI_disp_error("glaceblock    : set glace_uc block size and offset\n");
        CLI_disp_error("        glaceblock <HSizeFraction> <VSizeFraction> <HROIFraction> <VROIFraction>\n");
        CLI_disp_error("glacesat      : set glace_uc R G B saturation Value\n");
        CLI_disp_error("        glacesat <Red Sat Value> <Green Sat Value> <Blue Sat Value>\n");
        CLI_disp_error("glacegridsize : set glace_uc gridSize width and height\n");
        CLI_disp_error("        glacegridsize <gridWidth Value> <gridHeight Value>\n");
        CLI_disp_error("statwbgain   : set glace_uc R G B manual gain\n");
        CLI_disp_error("        statwbgain <Red manual Value> <Green manual Value> <Blue manual Value>\n");
        CLI_disp_error("glacedump     : dump Glace statistic R G B and Sat File\n");
        CLI_disp_error("        glacedump \n");
        CLI_disp_error("glacehisto     : request a new Glace and Histo statistic\n");
        CLI_disp_error("        glacehisto req \n");
        CLI_disp_error("updategain     : Update Manual gains and wait for new Glace and Histo statistic\n");
        CLI_disp_error("        updategain req \n");
    }
    else
    {
        CLI_disp_error("Not correct command arguments\n");
    }


    return (CMD_COMPLETE);
}


CMD_COMPLETION
C_ite_dbg_histohelp_cmd(
int     a_nb_args,
char    **ap_args)
{
    if (ap_args[1] == NULL)
    {
        ap_args[1] = "";        //For Linux, strcmp doesn't support NULL parameter
    }


    if (a_nb_args == 1)
    {
        CLI_disp_error("\n");
        CLI_disp_error("histo         : launch histo test\n");
        CLI_disp_error("            histo <update/req/reqnowait/waitstatready/freebuff>\n");
        CLI_disp_error("histoblock    : set histo_uc block size and offset\n");
        CLI_disp_error("        histo <HSizeFraction> <VSizeFraction> <HROIFraction> <VROIFraction>\n");
        CLI_disp_error("histodump     : dump histo statistic R G B and Sat File\n");
        CLI_disp_error("        histodump \n");
        CLI_disp_error("checkhisto    : display number of each R G B (should match with FOV)\n");
        CLI_disp_error("        checkhisto \n");
    }
    else
    {
        CLI_disp_error("Not correct command arguments\n");
    }


    return (CMD_COMPLETE);
}


t_cmd_list  ite_cmd_list_statistic[] =
{
    { "glacehelp", C_ite_dbg_glacehelp_cmd, "glacehelp: glacehelp\n" },
    { "glace", C_ite_dbg_glacestattest_cmd, "glace: glace <update/req/reqnowait/waitstatready/freebuff>\n" },
    { "glaceblock", C_ite_dbg_SetGlaceBlockFraction_cmd,
            "glaceblock: glaceblock <HSizeFraction Value> <VSizeFraction Value> <HROIFraction Value> <VROIFraction Value>\n"
            },
    { "glacegridsize", C_ite_dbg_SetGlaceGrid_cmd,
            "glacegridsize: glacegridsize <gridWidth Value> <gridHeight Value>\n" },
    { "glacesat", C_ite_dbg_SetGlaceSatLevel_cmd,
            "glacesat: glacesat <Red Sat Value> <Green Sat Value> <Blue Sat Value>\n" },
    { "statwbgain", C_ite_dbg_SetStatWBManualMode_cmd,
            "statwbgain: statwbgain <Red manual Value> <Green manual Value> <Blue manual Value>\n" },
    { "glacedump", C_ite_dbg_GlaceDump_cmd, "glacedump: glacedump \n" },
    { "histohelp", C_ite_dbg_histohelp_cmd, "histohelp: histohelp\n" },
    { "histo", C_ite_dbg_histostattest_cmd, "histo: histo <update/req/reqnowait/waitstatready/freebuff>  \n" },
    { "histoblock", C_ite_dbg_SetHistoBlockFraction_cmd,
            "histoblock: histoblock <HSizeFraction Value> <VSizeFraction Value> <HROIFraction Value> <VROIFraction Value>\n"
            },
    { "histodump", C_ite_dbg_HistoDump_cmd, "histodump: histodump \n" },
    { "checkhisto", C_ite_dbg_CheckHisto_cmd, "checkhisto: checkhisto \n" },
    { "glacehisto", C_ite_dbg_glacehistostattest_cmd, "glacehisto: glacehisto <req/reqnowait/waitstatready> \n" },
    { "glacehistoexp", C_ite_dbg_glacehistoexp_cmd, "glacehistoexp: glacehistoexp<req/reqnowait/waitstatready> \n" },
    { "updategain", C_ite_dbg_updategaintest_cmd, "updategain: updategain req\n" },
    { "createstatbuff", C_ite_dbg_createstatbuff_cmd, "createstatbuff: createstatbuff\n" },
    { "glacerobustness", C_ite_dbg_test_glacerobustness_cmd, "glacerobustness: glacerobustness <enable/disable>\n" },
    { "glacefocusstats", C_ite_dbg_test_glace_and_focus_stats_cmd, "glacefocusstats: glacefocusstats <framerate>\n" },
    { "glacefocusstatsrobustness", C_ite_dbg_test_glace_and_focus_stats_robustness_cmd,
            "glacefocusstatsrobustness: glacefocusstats <framerate>\n" },
    { "glacetimecheck", C_ite_dbg_GlaceTimeCheck_cmd, "glacetimecheck: glacetimecheck <LR/HR/BMS>\n" },
    { "updatexptime", C_ite_dbg_Update_Exposure_Time_cmd, "updatexptime: updatexptime<time>\n" },
};

void
Init_statistic_ITECmdList(void)
{
    CLI_register_interface(
    "STATISTIC_ITE_CMB",
    sizeof(ite_cmd_list_statistic) / (sizeof(ite_cmd_list_statistic[0])),
    ite_cmd_list_statistic,
    1);
}


int
ITE_NMF_test_glacefocusstats(
char    *fps)
{
    t_uint32            iter = 0;
    unsigned long long  time1,
                        time2,
                        time3,
                        time4;
    float               framerate = ITE_ConvToFloat(fps);
    int ret = EOK;

    //To trace grabLRinfo event
    TraceGrabLREvent = TRUE;

    ITE_testFirmwareRevision();

    ret = ITE_dbg_setusecase("LR", "VGA", "YUV422RI");
    if(ret != EOK)
    {
        goto END;
    }

    ret = ITE_dbg_setusecase("HR", "VGA", "YUV420MB_D");
    if(ret != EOK)
    {
        goto END;
    }

    LOS_Log("\n######### Test to evaluate Stat event Frequency versus Framerate \n");
    LOS_Log("######### Selected Framerate is %f fps\n\n", framerate);

    LOS_Log(
    "Goal is to monitor AF Stat ready event and Glace stat ready in parallel with incoming LR grabbed buffer event\n");
    LOS_Log(
    "Expectation is to get an AF event and a glace event per grabbed frame (or 1 glace event per 2 frames worst case for high framerate\n");
    LOS_Log("Loop Methodology:\n");
    LOS_Log("1: Update exposure time\n");
    LOS_Log("2: Request Lens movement (Null inf fact)\n");
    LOS_Log("3: Set System Coin\n");
    LOS_Log("4: Set AF Coin\n");
    LOS_Log("5: Wait for Focus Stats ready\n");
    LOS_Log("6: Wait for Glace Stats ready\n\n");

    //glaceblock <HSizeFraction Value> <VSizeFraction Value> <HROIFraction Value> <VROIFraction Value>\n");
    ITE_NMF_SetGlaceBlockFraction(100, 100, 0, 0);

    ITE_NMF_SetGlaceGrid(72, 54);

    //syntax: glacesat <Red Sat Value> <Green Sat Value> <Blue Sat Value>\n");
    ITE_NMF_SetGlaceSatLevel(255, 255, 255);

    //To work with Glace and histogram
    //
    //To work with Glace only: histogram pointer sets to 0 + allocation for FrameParamStatus structure
    //ITE_NMF_createStatsBuffer(&StatsBuffer);
    pStore = g_pStatsBufferStorage;

    ITE_writePE(HistStats_Ctrl_ptru32_HistRAddr_Byte0, (t_uint32) (0));
    ITE_writePE(HistStats_Ctrl_ptru32_HistGAddr_Byte0, (t_uint32) (0));
    ITE_writePE(HistStats_Ctrl_ptru32_HistBAddr_Byte0, (t_uint32) (0));

    //To work with AF stats: allocation of buffer + init
    ITE_NMF_FocusStatsPrepare();

    //Init AF
    ITE_writePE(AFStats_Controls_e_AFStats_HostCmd_Ctrl_Byte0, AFStats_HostCmd_e_REQ_STATS_ONCE);
    ITE_writePE(FocusControl_Controls_e_Flag_StatsWithLensMove_Control_Byte0, Flag_e_TRUE);

    ITE_writePE(HostFrameConstraints_e_Flag_GuaranteeStaticFlickerFrameLength_Byte0, Flag_e_FALSE);

    //ITE_SetFrameRateControl(5,5);
    ITE_SetFrameRateControl(framerate, framerate);

    ITE_LR_Prepare(&usecase, INFINITY);
    ITE_LR_Start(INFINITY);

    LOS_Log("\nStart VF and sleep 2s\n");
    LOS_Sleep(2);
    LOS_Log("\nEnd of sleep \n");

    //Perform loop updating exposure time gain
    for (iter = 0; iter < 4; iter++)
    {
        LOS_Log("\n\n #####       Set new exposure time : %d ms\n\n", 20 * (iter + 1));

        //Update exposure time :
        ITE_writePE(Exposure_DriverControls_u32_TargetExposureTime_us_Byte0, 20000 * (iter + 1));

        //Update Focus: Request a Null "fake" Lens movement
        ITE_writePE(FLADriver_LLLCtrlStatusParam_u16_ManualStepSize_Byte0, 0);
        ITE_writePE(
        FocusControl_Controls_e_FocusControl_LensCommand_Control_Byte0,
        FocusControl_LensCommand_e_LA_CMD_MOVE_STEP_TO_INFINITY);

        //Set System Coin
        time1 = ITE_NMF_GlaceAndHistoExpReqNoWait();
        LOS_Log(
        "    --> Set_SystemCoin=%.3Lfs ( %9Ld us)\n",
        (time1 / ( double ) 1000000),
        time1,
        NULL,
        NULL,
        NULL,
        NULL);

        //Set Focus Coin
        time2 = ITE_NMF_AFReqNoWait();
        LOS_Log("    --> Set_AFCoin=%.3Lfs ( %9Ld us)\n", (time2 / ( double ) 1000000), time2, NULL, NULL, NULL, NULL);

        //Wait Focus Stats ready
        ITE_NMF_waitForAFStatsReady();
        time3 = LOS_getSystemTime();
        LOS_Log(
        "    --> Got_AFStatReady=%.3Lfs ( %9Ld us)\n",
        (time3 / ( double ) 1000000),
        time3,
        NULL,
        NULL,
        NULL,
        NULL);

        //Wait Glace stats ready
        time4 = ITE_NMF_WaitStatReady(ISP_GLACE_STATS_READY);
        LOS_Log(
        "    --> Got_GlaceStatReady=%.3Lfs ( %9Ld us)\n",
        (time4 / ( double ) 1000000),
        time4,
        NULL,
        NULL,
        NULL,
        NULL);

        ITE_NMF_DisplayFrameParamStatus();

        //Stop & Start the pipe to check not break stat generation
        LOS_Log("    Stop & Start the pipe to check not break stat generation \n", NULL, NULL, NULL, NULL, NULL, NULL);
        ITE_LR_Stop();
        ITE_LR_Start(INFINITY);

        LOS_Log(" \n\n   ", NULL, NULL, NULL, NULL, NULL, NULL);
    }


    //Dump stat buffers in files
    //ITE_NMF_DumpGlaceBuffer();
    //Sudeep: commented as allocation is done only once at (post) BOOT time
    //TODO: find a suitable place to move Free buffers code.
#if 0
    //Free stat buffers
    ITE_NMF_freeStatsBuffer(pStatsBuffer);
    ITE_NMF_ResetISPBufferPointers();

    ITE_NMF_freeFocusStatsBuffer(pFocusStatsBuffer);
    ITE_NMF_ResetFocusBufferPointers();
#endif

    //Stop HR pipe
    ITE_LR_Stop();
    ITE_LR_Free();

    //To Disable trace grabLRinfo event
    TraceGrabLREvent = FALSE;
    END:
    return ret;
}


int
ITE_NMF_test_glacefocusstats_robustness(
char    *fps)
{
    int ret = EOK;
    t_uint32            iter = 0;
    unsigned long long  time1,
                        time2,
                        time3,
                        time4;
    float               framerate = ITE_ConvToFloat(fps);
    //To trace grabLRinfo event
    TraceGrabLREvent = TRUE;

    ITE_testFirmwareRevision();

    ret = ITE_dbg_setusecase("LR", "VGA", "YUV422RI");
    if(ret != EOK)
    {
        goto END;
    }

    ret =ITE_dbg_setusecase("HR", "VGA", "YUV420MB_D");
    if(ret != EOK)
    {
        goto END;
    }

    LOS_Log("\n######### Test to check Stat event are still generated even if pipe Stop at different time \n");
    LOS_Log("######### Selected Framerate is %f fps\n\n", framerate);

    LOS_Log("This robustness test relies on AF+Glace stat test\n");

    //glaceblock <HSizeFraction Value> <VSizeFraction Value> <HROIFraction Value> <VROIFraction Value>\n");
    ITE_NMF_SetGlaceBlockFraction(100, 100, 0, 0);

    ITE_NMF_SetGlaceGrid(72, 54);

    //syntax: glacesat <Red Sat Value> <Green Sat Value> <Blue Sat Value>\n");
    ITE_NMF_SetGlaceSatLevel(255, 255, 255);

    //To work with Glace and histogram
    //
    //To work with Glace only: histogram pointer sets to 0 + allocation for FrameParamStatus structure
    //ITE_NMF_createStatsBuffer(&StatsBuffer);
    pStore = g_pStatsBufferStorage;

    ITE_writePE(HistStats_Ctrl_ptru32_HistRAddr_Byte0, (t_uint32) (0));
    ITE_writePE(HistStats_Ctrl_ptru32_HistGAddr_Byte0, (t_uint32) (0));
    ITE_writePE(HistStats_Ctrl_ptru32_HistBAddr_Byte0, (t_uint32) (0));

    //To work with AF stats: allocation of buffer + init
    ITE_NMF_FocusStatsPrepare();

    //Init AF
    ITE_writePE(AFStats_Controls_e_AFStats_HostCmd_Ctrl_Byte0, AFStats_HostCmd_e_REQ_STATS_ONCE);
    ITE_writePE(FocusControl_Controls_e_Flag_StatsWithLensMove_Control_Byte0, Flag_e_TRUE);

    ITE_writePE(HostFrameConstraints_e_Flag_GuaranteeStaticFlickerFrameLength_Byte0, Flag_e_FALSE);

    //ITE_SetFrameRateControl(5,5);
    ITE_SetFrameRateControl(framerate, framerate);

    ITE_LR_Prepare(&usecase, INFINITY);
    ITE_LR_Start(INFINITY);

    LOS_Log("\nStart VF and sleep 2s\n");
    LOS_Sleep(2);
    LOS_Log("\nEnd of sleep \n");

    //Perform loop updating exposure time gain
    for (iter = 0; iter < 5; iter++)
    {
        LOS_Log("\n\n #####       Set new exposure time : %d ms\n\n", 20 * (iter + 1));

        //Update exposure time :
        ITE_writePE(Exposure_DriverControls_u32_TargetExposureTime_us_Byte0, 20000 * (iter + 1));

        //Update Focus: Request a Null "fake" Lens movement
        ITE_writePE(FLADriver_LLLCtrlStatusParam_u16_ManualStepSize_Byte0, 0);
        ITE_writePE(
        FocusControl_Controls_e_FocusControl_LensCommand_Control_Byte0,
        FocusControl_LensCommand_e_LA_CMD_MOVE_STEP_TO_INFINITY);

        //Set System Coin
        time1 = ITE_NMF_GlaceAndHistoExpReqNoWait();
        LOS_Log(
        "    --> Set_SystemCoin=%.3Lfs ( %9Ld us)\n",
        (time1 / ( double ) 1000000),
        time1,
        NULL,
        NULL,
        NULL,
        NULL);

        if (iter == 3)
        {
            //Stop & Start the pipe to check not break stat generation
            LOS_Log("########## \n");
            LOS_Log(
            "##########    Stop & Start the pipe BEFORE Toggling Focus Coin (after toggle of System coin) \n",
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL);
            LOS_Log("########## \n");
            ITE_LR_Stop();
            ITE_LR_Start(INFINITY);

            LOS_Log(" \n\n   ", NULL, NULL, NULL, NULL, NULL, NULL);
        }


        //Set Focus Coin
        time2 = ITE_NMF_AFReqNoWait();
        LOS_Log("    --> Set_AFCoin=%.3Lfs ( %9Ld us)\n", (time2 / ( double ) 1000000), time2, NULL, NULL, NULL, NULL);

        if (iter == 2)
        {
            //Stop & Start the pipe to check not break stat generation
            LOS_Log("########## \n");
            LOS_Log(
            "##########    Stop & Start the pipe BEFORE receiving Focus stat event \n",
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL);
            LOS_Log("########## \n");
            ITE_LR_Stop();
            ITE_LR_Start(INFINITY);

            LOS_Log(" \n\n   ", NULL, NULL, NULL, NULL, NULL, NULL);
        }


        //Wait Focus Stats ready
        ITE_NMF_waitForAFStatsReady();
        time3 = LOS_getSystemTime();
        LOS_Log(
        "    --> Got_AFStatReady=%.3Lfs ( %9Ld us)\n",
        (time3 / ( double ) 1000000),
        time3,
        NULL,
        NULL,
        NULL,
        NULL);

        if (iter == 1)
        {
            //Stop & Start the pipe to check not break stat generation
            LOS_Log("########## \n");
            LOS_Log(
            "##########    Stop & Start the pipe BEFORE receiving Glace stat event \n",
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL);
            LOS_Log("########## \n");
            ITE_LR_Stop();
            ITE_LR_Start(INFINITY);

            LOS_Log(" \n\n   ", NULL, NULL, NULL, NULL, NULL, NULL);
        }


        //Wait Glace stats ready
        time4 = ITE_NMF_WaitStatReady(ISP_GLACE_STATS_READY);
        LOS_Log(
        "    --> Got_GlaceStatReady=%.3Lfs ( %9Ld us)\n",
        (time4 / ( double ) 1000000),
        time4,
        NULL,
        NULL,
        NULL,
        NULL);

        if (iter == 0)
        {
            //Stop & Start the pipe to check not break stat generation
            LOS_Log("########## \n");
            LOS_Log(
            "##########    Stop & Start the pipe AFTER All stat event received \n",
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL);
            LOS_Log("########## \n");
            ITE_LR_Stop();
            ITE_LR_Start(INFINITY);

            LOS_Log(" \n\n   ", NULL, NULL, NULL, NULL, NULL, NULL);
        }
    }


    //Dump stat buffers in files
    //ITE_NMF_DumpGlaceBuffer();
    //Sudeep: commented as allocation is done only once at (post) BOOT time
    //TODO: find a suitable place to move Free buffers code.
#if 0
    //Free stat buffers
    ITE_NMF_freeStatsBuffer(pStatsBuffer);
    ITE_NMF_ResetISPBufferPointers();
    ITE_NMF_freeFocusStatsBuffer(pFocusStatsBuffer);
    ITE_NMF_ResetFocusBufferPointers();
#endif

    //Stop HR pipe
    ITE_LR_Stop();
    ITE_LR_Free();

    //To Disable trace grabLRinfo event
    TraceGrabLREvent = FALSE;
    END:
    return ret;
}


void
ITE_NMF_UpdateExposureTime_cmd(
char    *exptime)
{
    t_uint32    exposure;
    exposure = ITE_ConvToInt32(exptime);
    LOS_Log("exposure time set is : %d\n", ( int ) exposure);
    ITE_NMF_UpdateExposureTime(exposure);
}


void
ITE_NMF_GlaceTimeCheck(
char    *ap_pipe)
{
    t_uint32            frameid1,
                        frameid2,
                        result,
                        exptime,
                        appexp,
                        minexp,
                        maxexp;
    char                comment[256],
                        filename[256];
    unsigned long long  timestart = 0,
                        timestop = 0;

    MMTE_TEST_START("GlaceTimeCheck", "/ite_nmf/test_results_nreg/FrameParamStatus", "Testing for GlaceTimeCheck");

    pStore = g_pStatsBufferStorage;

    LOS_Log("GLACE TIME CHECK FOR %s PIPE\n", ap_pipe);

    minexp = ITE_readPE(FrameParamStatus_u32_SensorExposureTimeMin_us_Byte0);
    maxexp = ITE_readPE(FrameParamStatus_u32_SensorExposureTimeMax_us_Byte0);

    for (exptime = minexp; exptime <= maxexp; exptime = (exptime + 5000))
    {
        ITE_writePE(Exposure_DriverControls_u32_TargetExposureTime_us_Byte0, exptime);

        frameid1 = ITE_readPE(Interrupts_Count_u16_INT05_SMIARX0_Byte0);
        timestart = LOS_getSystemTime();

        ITE_NMF_GlaceHistoExpStatsRequestAndWait();

        frameid2 = ITE_readPE(Interrupts_Count_u16_INT05_SMIARX0_Byte0);
        timestop = LOS_getSystemTime();

        appexp = ITE_readPE(FrameParamStatus_u32_ExposureTime_us_Byte0);

        result = frameid2 - frameid1;

        snprintf(
        comment,
        sizeof(comment),
        "Timestart:%llu\tTimestop:%llu\nFrameID Start:%d\tFrameID Stop:%d\nFrameID Diff:%d\n Exposure Set:%d\t Exposure Applied:%d\n",
        timestart,
        timestop,
        ( int ) frameid1,
        ( int ) frameid2,
        ( int ) result,
        ( int ) exptime,
        ( int ) appexp);
        MMTE_TEST_COMMENT(comment);

        if (result < 3)
        {
            MMTE_RESULT_PASSED();
        }
        else
        {
            MMTE_RESULT_FAILED();
        }


        ITE_NMF_DumpGlaceBuffer();

        if (0 == strcmp(ap_pipe, "HR"))
        {
            sprintf(filename, IMAGING_PATH "/glacetimecheck_HR");
            ITE_StoreinBMPFile(filename, &(GrabBufferHR[0]));
        }
        else if (0 == strcmp(ap_pipe, "LR"))
        {
            sprintf(filename, IMAGING_PATH "/glacetimecheck_LR");
            ITE_StoreinBMPFile(filename, &(GrabBufferLR[0]));
        }
        else
        {
            sprintf(filename, IMAGING_PATH "/glacetimecheck_BMS");
            ITE_StoreinBMPFile("/glacetimecheck_BMS_", &(GrabBufferBMS[0]));
        }


        LOS_Sleep(500);
    }


    MMTE_TEST_END();
}


void
ITE_NMF_test_glacerobustness(void)
{
    t_uint32        iter = 0;

    char            filename[250];
    static t_uint32 Nb = 0;

    t_uint16        minfps,
                    max_ag,
                    min_ag,
                    ag_applied;


    float           exp_time,
                    rand_fps,
                    dg_r_applied,
                    dg_g_applied,
                    fps,
                    dg_b_applied;

    ITE_testFirmwareRevision();

    //glaceblock <HSizeFraction Value> <VSizeFraction Value> <HROIFraction Value> <VROIFraction Value>\n");
    ITE_NMF_SetGlaceBlockFraction(100, 100, 0, 0);

    ITE_NMF_SetGlaceGrid(72, 54);

    //syntax: glacesat <Red Sat Value> <Green Sat Value> <Blue Sat Value>\n");
    ITE_NMF_SetGlaceSatLevel(255, 255, 255);

    //To work with Glace and histogram
    //To work with Glace only: histogram pointer sets to 0 + allocation for FrameParamStatus structure
    pStore = g_pStatsBufferStorage;
    ITE_writePE(HistStats_Ctrl_ptru32_HistRAddr_Byte0, (t_uint32) g_pStatsBufferStorage->ispHistAddr_R);
    ITE_writePE(HistStats_Ctrl_ptru32_HistGAddr_Byte0, (t_uint32) g_pStatsBufferStorage->ispHistAddr_G);
    ITE_writePE(HistStats_Ctrl_ptru32_HistBAddr_Byte0, (t_uint32) g_pStatsBufferStorage->ispHistAddr_B);

    ITE_NMF_UpdateGlaceParam();

    ITE_LRHR_Prepare(&usecase, INFINITY);
    ITE_LRHR_Start(INFINITY);

    fps = ITE_FrameRateProgrammed();
    minfps = 1;
    max_ag = ITE_readPE(FrameParamStatus_Extn_u32_SensorParametersAnalogGainMax_x256_Byte0);
    min_ag = ITE_readPE(FrameParamStatus_Extn_u32_SensorParametersAnalogGainMin_x256_Byte0);

    //perform loop updating exposure time
    for (iter = 0; iter < 10; iter++)
    {
        LOS_Log("\niter_Exposure= %d\n", iter);
        exp_time = ite_rand_FloatRange(( float ) minfps, (1.0 / ( float ) (fps))) * 1000000;
        LOS_Log("\nExposure Time(us):  %f \n", exp_time);
        ITE_NMF_UpdateExposureTime((t_uint32) (exp_time));
        if (g_Store_IMAGES)
         {
           ITE_NMF_DumpGlaceBuffer();
           ITE_NMF_DumpHistoBuffer();
         }
        ITE_NMF_DisplayFrameParamStatus();
        if (g_Store_IMAGES)
        {
        sprintf(
        filename,
        IMAGING_PATH "/Frame_EXP_%d_HR_%d_%d",
        ( int ) exp_time,
        ( int ) Nb,
        ( int ) g_frameparam_counter);
        Nb++;
        ITE_StoreinBMPFile(filename, &(GrabBufferHR[0]));
        sprintf(
        filename,
        IMAGING_PATH "/Frame_EXP_%d_LR_%d_%d",
        ( int ) exp_time,
        ( int ) Nb,
        ( int ) g_frameparam_counter);
        Nb++;
        ITE_StoreinBMPFile(filename, &(GrabBufferLR[0]));
        g_frameparam_counter++;
    }

    }


    //Perform loop updating analog gain
    for (iter = 0; iter < 10; iter++)
    {
        LOS_Log("\niter= %d\n", iter);
        Nb = 0;

        //set analog gain to 1 and exposure time to 50ms
        ag_applied = ite_rand_FloatRange(( float ) min_ag, ( float ) max_ag);
        LOS_Log("\nAnalog Gainx100: %d\n", ( int ) ag_applied);
        ITE_NMF_UpdateAnalogGain((t_uint32) ag_applied);
        LOS_Sleep(1);
        if (g_Store_IMAGES)
         {
           ITE_NMF_DumpGlaceBuffer();
           ITE_NMF_DumpHistoBuffer();
         }
        ITE_NMF_DisplayFrameParamStatus();
        if (g_Store_IMAGES)
        {
        sprintf(
        filename,
        IMAGING_PATH "/Frame_AG_%d_HR_%d_%d",
        ( int ) ag_applied,
        ( int ) Nb,
        ( int ) g_frameparam_counter);
        Nb++;
        ITE_StoreinBMPFile(filename, &(GrabBufferHR[0]));
        sprintf(
        filename,
        IMAGING_PATH "/Frame_AG_%d_LR_%d_%d",
        ( int ) ag_applied,
        ( int ) Nb,
        ( int ) g_frameparam_counter);
        Nb++;
        ITE_StoreinBMPFile(filename, &(GrabBufferLR[0]));
        g_frameparam_counter++;
    }
    }


    for (iter = 0; iter < 10; iter++)
    {
        LOS_Log("\niter= %d\n", iter);
        Nb = 0;
        dg_r_applied = ite_rand_FloatRange(MIN_DG, MAX_DG);
        ITE_NMF_UpdateDigitalGain(dg_r_applied);
        LOS_Sleep(10);
         if (g_Store_IMAGES)
         {
           ITE_NMF_DumpGlaceBuffer();
           ITE_NMF_DumpHistoBuffer();
         }
        if (g_Store_IMAGES)
        {
        sprintf(
        filename,
        IMAGING_PATH "/Frame_DG_%f_HR_%d_%d",
        dg_r_applied,
        ( int ) iter,
        ( int ) g_frameparam_counter);
        Nb++;
        ITE_StoreinBMPFile(filename, &(GrabBufferHR[0]));
        sprintf(
        filename,
        IMAGING_PATH "/Frame_DG_%f_LR_%d_%d",
        dg_r_applied,
        ( int ) iter,
        ( int ) g_frameparam_counter);
        Nb++;
        ITE_StoreinBMPFile(filename, &(GrabBufferLR[0]));
        g_frameparam_counter++;
    }
    }


    for (iter = 1; iter < 30; iter++)
    {
        rand_fps = ite_rand_FloatRange(1.0, fps);
        ITE_SetFrameRateControl(rand_fps, rand_fps);

        exp_time = ite_rand_FloatRange(( float ) minfps, (1.0 / rand_fps)) * 1000000;
        ITE_writePE(Exposure_DriverControls_u32_TargetExposureTime_us_Byte0, exp_time);

        //ITE_NMF_UpdateExposureTime((t_uint32) exp_time);
        LOS_Log("\nExposure Time :%d\n", ( int ) exp_time);

        ag_applied = ite_rand_FloatRange(( float ) min_ag, ( float ) max_ag);
        ITE_writePE(Exposure_DriverControls_u16_TargetAnalogGain_x256_Byte0, ag_applied);
        LOS_Log("\nAnalog Gain :%d\n", ( int ) ag_applied);

        dg_r_applied = ite_rand_FloatRange(MIN_DG, MAX_DG);
        ITE_writePE(WhiteBalanceControl_f_RedManualGain_Byte0, *( volatile t_uint32 * ) &dg_r_applied);

        dg_g_applied = ite_rand_FloatRange(MIN_DG, MAX_DG);
        ITE_writePE(WhiteBalanceControl_f_GreenManualGain_Byte0, *( volatile t_uint32 * ) &dg_g_applied);

        dg_b_applied = ite_rand_FloatRange(MIN_DG, MAX_DG);
        ITE_writePE(WhiteBalanceControl_f_BlueManualGain_Byte0, *( volatile t_uint32 * ) &dg_b_applied);

        //Toggle SystemCoin
        ITE_NMF_GlaceHistoExpStatsRequestAndWait();

        LOS_Sleep(10);

        if (g_Store_IMAGES)
         {
           ITE_NMF_DumpGlaceBuffer();
           ITE_NMF_DumpHistoBuffer();
         };
        ITE_NMF_DisplayFrameParamStatus();
        if (g_Store_IMAGES)
        {
        sprintf(
        filename,
        IMAGING_PATH "/HR_%d_AG_%d_DG_R_%d_DG_G_%d_DG_B_%d_%d_%d",
        ( int ) exp_time,
        ( int ) ag_applied,
        ( int ) dg_r_applied,
        ( int ) dg_g_applied,
        ( int ) dg_b_applied,
        ( int ) g_frameparam_counter,
        ( int ) Nb);
        Nb++;
        ITE_StoreinBMPFile(filename, &(GrabBufferHR[0]));
        sprintf(
        filename,
        IMAGING_PATH "/LR_%d_AG_%d_DG_R_%d_DG_G_%d_DG_B_%d_%d_%d",
        ( int ) exp_time,
        ( int ) ag_applied,
        ( int ) dg_r_applied,
        ( int ) dg_g_applied,
        ( int ) dg_b_applied,
        ( int ) g_frameparam_counter,
        ( int ) Nb);
        Nb++;
        ITE_StoreinBMPFile(filename, &(GrabBufferLR[0]));

        g_frameparam_counter++;
    }
    }


    if(Flag_e_FALSE == ITE_readPE(VariableFrameRateControl_e_Flag_Byte0))
    {
        ITE_writePE(FrameRateControl_f_UserMaximumFrameRate_Hz_Byte0, *(volatile t_uint32 *) &fps);
    }
    else
    {
        ITE_writePE(VariableFrameRateControl_f_CurrentFrameRate_Hz_Byte0, *(volatile t_uint32 *) &fps);
    }

    //Stop LRHR pipe
    ITE_LRHR_Stop();
    ITE_LRHR_Free();
}
void ITE_NMF_Continous_Glace_Test(char *ap_pipe)
{
    volatile t_uint32  count1= 0,count2,glace_count1,glace_count2;
    volatile t_uint32 sleep_duration;
    volatile t_uint8 loop_count;
    float fps;
    char mess[256];

    //glaceblock <HSizeFraction Value> <VSizeFraction Value> <HROIFraction Value> <VROIFraction Value>\n");
    ITE_NMF_SetGlaceBlockFraction(100, 100, 0, 0);

    ITE_NMF_SetGlaceGrid(72, 54);

    //syntax: glacesat <Red Sat Value> <Green Sat Value> <Blue Sat Value>\n");
    ITE_NMF_SetGlaceSatLevel(255, 255, 255);

    //To work with Glace and histogram
    //To work with Glace only: histogram pointer sets to 0 + allocation for FrameParamStatus structure
    pStore = g_pStatsBufferStorage;

    ITE_NMF_UpdateGlaceParam();

    //Setting Glace stats to continuous
    ITE_writePE(Glace_Control_e_GlaceOperationMode_Control_Byte0, GlaceOperationMode_e_Continuous);

    fps = ITE_FrameRateProgrammed();
    sleep_duration = (int)fps *100;

    snprintf(mess,sizeof(mess),"_Continous_Glace_Test_%s_%d",ap_pipe,(int)fps);
    MMTE_TEST_START(mess,"/ite_nmf/test_results_nreg/Continous_Glace_Test","Testing Continous_Glace_Test");

    if (0== strcmp(ap_pipe,"LR"))
     {
     for (loop_count = 0 ; loop_count <= 2 ;loop_count++)
      {
       snprintf(mess,sizeof(mess),"Continous_Glace_Test_%s_%d_%d",ap_pipe,(int)fps,(int)loop_count);
       MMTE_TEST_NEXT(mess);

       sleep_duration = sleep_duration + (loop_count*1000);
       LOS_Log("STARTING LR PIPE AND SLEEP FOR %d SECONDS In Loop %d\n",sleep_duration/1000,loop_count);
       //Count1 is always 0  as PipeStatus_1_u8_FramesStreamedOutOfPipeLastRun_Byte0 resets to zero at pipe start
       glace_count1 = ITE_readPE(Event0_Count_u16_EVENT0_17_GLACE_STATS_READY_Byte0);

     ITE_LR_Prepare(&usecase, INFINITY);
     ITE_LR_Start(INFINITY);
        
       // Pipe streams  for specified seconds.
       LOS_Sleep(sleep_duration);

     ITE_LR_Stop();
     count2 = ITE_readPE(PipeStatus_1_u8_FramesStreamedOutOfPipeLastRun_Byte0);
     //check glace events after pipe streames "frame_count" no of frames
     glace_count2 = ITE_readPE(Event0_Count_u16_EVENT0_17_GLACE_STATS_READY_Byte0);
     ITE_LR_Free();
       //Cleaning all stats events from event queue
       ITE_CleanEvent(ITE_EVT_ISPCTL_STATS_EVENTS);

       sprintf(mess,"Start Frame :%d\n",(int)count1);
       MMTE_TEST_COMMENT(mess);
       sprintf(mess,"End Frame:%d\n",(int)(count2));
       MMTE_TEST_COMMENT(mess);
       sprintf(mess,"Glace Stats starte count: %d\n",(int)glace_count1);
       MMTE_TEST_COMMENT(mess);
       sprintf(mess,"Glace Stats end count: %d\n",(int)glace_count2);
       MMTE_TEST_COMMENT(mess);
       sprintf(mess,"Frames Streamed:%d\n",(int)(count2-count1));
       MMTE_TEST_COMMENT(mess);
       sprintf(mess,"No Of Glace Stats Received: %d\n",(int)(glace_count2 -glace_count1));
       MMTE_TEST_COMMENT(mess);

       //check if frames streamed is same as glace events received
       if ((glace_count2-glace_count1)== (count2-count1))
         {
           LOS_Log("\nTEST PASSED\n");
           MMTE_RESULT_PASSED();
         }
        else
         {
          LOS_Log("\nTEST FAILED\n");
          MMTE_RESULT_FAILED();
         }

      }

     }

    if (0== strcmp(ap_pipe,"HR")) 
     {
      for (loop_count = 0 ; loop_count <= 2 ;loop_count++)
      {
       snprintf(mess,sizeof(mess),"Continous_Glace_Test_%s_%d_%d",ap_pipe,(int)fps,(int)loop_count);
       MMTE_TEST_NEXT(mess);

       sleep_duration = sleep_duration + (loop_count*1000);
       LOS_Log("STARTING HR PIPE AND SLEEP FOR %d SECONDS In Loop %d\n",sleep_duration/1000,loop_count);
       //Count1 is always 0  as PipeStatus_0_u8_FramesStreamedOutOfPipeLastRun_Byte0 resets to zero at pipe start
     glace_count1 = ITE_readPE(Event0_Count_u16_EVENT0_17_GLACE_STATS_READY_Byte0);
       ITE_HR_Prepare(&usecase, INFINITY);
       ITE_HR_Start(INFINITY);

       // Pipe streams  for specified seconds.
       LOS_Sleep(sleep_duration);

       ITE_HR_Stop();
       count2 = ITE_readPE(PipeStatus_0_u8_FramesStreamedOutOfPipeLastRun_Byte0);
     //check glace events after pipe streames "frame_count" no of frames
     glace_count2 = ITE_readPE(Event0_Count_u16_EVENT0_17_GLACE_STATS_READY_Byte0);
     ITE_HR_Free();
    //Cleaning all stats events from event queue
    ITE_CleanEvent(ITE_EVT_ISPCTL_STATS_EVENTS);

    sprintf(mess,"Start Frame :%d\n",(int)count1);
       MMTE_TEST_COMMENT(mess);
    sprintf(mess,"End Frame:%d\n",(int)(count2));
       MMTE_TEST_COMMENT(mess);
    sprintf(mess,"Glace Stats starte count: %d\n",(int)glace_count1);
       MMTE_TEST_COMMENT(mess);
    sprintf(mess,"Glace Stats end count: %d\n",(int)glace_count2);
       MMTE_TEST_COMMENT(mess);
    sprintf(mess,"Frames Streamed:%d\n",(int)(count2-count1));
       MMTE_TEST_COMMENT(mess);
    sprintf(mess,"No Of Glace Stats Received: %d\n",(int)(glace_count2 -glace_count1));
       MMTE_TEST_COMMENT(mess);

    //check if frames streamed is same as glace events received
    if ((glace_count2-glace_count1)== (count2-count1))
      {
        LOS_Log("\nTEST PASSED\n");
           MMTE_RESULT_PASSED();
      }
     else
      {
       LOS_Log("\nTEST FAILED\n");
          MMTE_RESULT_FAILED();
      }

       }

     }
     MMTE_TEST_END();

}



void
ITE_NMF_test_SensorParams(
char    *loop_count)
{
    t_uint16    count,
                iter;

    t_uint32    exp_time[6] = { 1000,
                2000,
                3000,
                5000,
                10000,
                30000 };
    t_uint32    a_gain[6] = { 256,
                512,
                768,
                1024,
                1280,
                1536 };
    t_uint8     iter_values;

    count = ITE_ConvToInt16(loop_count);

    //glaceblock <HSizeFraction Value> <VSizeFraction Value> <HROIFraction Value> <VROIFraction Value>\n");
    ITE_NMF_SetGlaceBlockFraction(100, 100, 0, 0);

    ITE_NMF_SetGlaceGrid(72, 54);

    //syntax: glacesat <Red Sat Value> <Green Sat Value> <Blue Sat Value>\n");
    ITE_NMF_SetGlaceSatLevel(255, 255, 255);

    //To work with Glace and histogram
    //To work with Glace only: histogram pointer sets to 0 + allocation for FrameParamStatus structure
    pStore = g_pStatsBufferStorage;

    ITE_NMF_UpdateGlaceParam();

    //Setting Glace stats to continuous
    ITE_writePE(Glace_Control_e_GlaceOperationMode_Control_Byte0, GlaceOperationMode_e_Continuous);

    ITE_LRHR_Prepare(&usecase, INFINITY);
    ITE_LRHR_Start(INFINITY);

    for (iter = 0; iter < count; iter++)
    {
        for (iter_values = 0; iter_values <= 5; iter_values++)
        {
            ITE_writePE(Exposure_DriverControls_u32_TargetExposureTime_us_Byte0, exp_time[iter_values]);
            LOS_Log("\nExposure Time :%d\n", exp_time[iter_values]);

            ITE_writePE(Exposure_DriverControls_u16_TargetAnalogGain_x256_Byte0, a_gain[iter_values]);
            LOS_Log("\nAnalog Gain :%d\n", a_gain[iter_values]);

            ITE_DisplaySensorParams();
        }
    }


    //Stop LRHR pipe
    ITE_LRHR_Stop();
    ITE_LRHR_Free();
    ITE_CleanEvent(ITE_EVT_ISPCTL_STATS_EVENTS);
}


void
ITE_DisplaySensorParams(void)
{
    volatile t_uint16   rev1,
                        rev2,
                        rev3,
                        rev4,
                        rev5,
                        rev6,
                        rev7,
                        iter;

    char                mess[255],
                        filename[256],
                        pathname[256];

    t_uint32            sensorparams[24];
    union u_ITE_Event   event;


    sprintf(filename, "SensorParamStatus");
    sprintf(pathname, IMAGING_PATH "/ite_nmf/test_results_nreg/SensorParamStatus");
    strcat(g_out_path, pathname);
    init_test_results_nreg(g_out_path);
    ITE_RefreshEventCount(Event1_Count_u16_EVENT1_1_SENSOR_COMMIT_Byte0);

    g_frameparam_counter = ITE_readPE(FrameParamStatus_u32_frame_counter_Byte0);

    g_grabLR_count = ITE_readPE(Event0_Count_u16_EVENT0_17_GLACE_STATS_READY_Byte0);

    //Toggle Sensor Coin
    if (Coin_e_Heads == ITE_readPE(SensorPipeSettings_Status_e_Coin_SensorSettings_Byte0))
    {
        ITE_writePE(SensorPipeSettings_Control_e_Coin_SensorSettings_Byte0, Coin_e_Tails);
    }
    else
    {
        ITE_writePE(SensorPipeSettings_Control_e_Coin_SensorSettings_Byte0, Coin_e_Heads);
    }

    //ITE_NMF_WaitEvent(ISP_SENSOR_COMMIT_NOTIFICATION);
    do
    {
        event = ITE_WaitEvent(ISP_SENSOR_COMMIT_NOTIFICATION_EVENTS);
        ITE_traceISPCTLevent(event);
        if (event.type == ITE_EVT_ISPCTL_ERROR)
        {
            LOS_Log("\nITE_EVT_ISPCTL_ERROR \n");
            break;
         }
    } while (!(event.ispctlInfo.info_id == ISP_SENSOR_COMMIT_NOTIFICATION));

    ITE_GetEventCount(Event1_Count_u16_EVENT1_1_SENSOR_COMMIT_Byte0);

    //ITE_NMF_GlaceHistoExpStatsRequestAndWait();
    ITE_NMF_WaitStatReady(ISP_GLACE_STATS_READY);

    LOS_Sleep(10);

    rev1 = ITE_readPE(Exposure_DriverControls_u32_TargetExposureTime_us_Byte0);
    rev2 = ITE_readPE(Exposure_DriverControls_u16_TargetAnalogGain_x256_Byte0);
    rev3 = ITE_readPE(FrameParamStatus_u32_frameRate_x100_Byte0);
    rev4 = ITE_readPE(Interrupts_Count_u16_INT05_SMIARX_Byte0);
    rev5 = rev1 + rev2 + rev4 + g_frameparam_counter;

    sprintf(
    mess,
    "_ExposureTime%d_AnalogGain%d_frameRate%d_random%d_%d",
    ( int ) rev1,
    ( int ) rev2,
    ( int ) rev3 / 100,
    ( int ) rev5,
    ( int ) g_frameparam_counter);

    strcat(filename, mess);

    mmte_testStart(filename, "SensorParamStatus", g_out_path);
    sprintf(mess, "Testing for SensorParamStatus");
    mmte_testNext(mess);

    LOS_Log("Parameters   |   EXPOSURE   |   ANALOG GAIN   |   FRAMECOUNTER\n");
    mmte_testComment("Parameters   |   EXPOSURE   |   ANALOG GAIN   |   FRAMECOUNTER | GLACE STAT COUNTER\n");

    rev3 = g_frameparam_counter;

    rev4 = ITE_readPE(Exposure_ParametersApplied_u32_TotalIntegrationTime_us_Byte0);
    rev5 = ITE_readPE(Exposure_ParametersApplied_u16_AnalogGain_x256_Byte0);

    while (!((pFrameParamStatus->u32_ExposureTime_us == rev4) && (pFrameParamStatus->u32_AnalogGain_x256 == rev5)))
        ;

    /*wait*/
    {
        rev7 = ITE_readPE(Event0_Count_u16_EVENT0_17_GLACE_STATS_READY_Byte0);
        rev6 = ITE_readPE(FrameParamStatus_u32_frame_counter_Byte0);

        sensorparams[iter] = ITE_readPE(Exposure_DriverControls_u32_TargetExposureTime_us_Byte0);
        iter++;
        sensorparams[iter] = ITE_readPE(Exposure_DriverControls_u16_TargetAnalogGain_x256_Byte0);
        iter++;
        sensorparams[iter] = g_frameparam_counter;
        iter++;
        sensorparams[iter] = rev6;

        LOS_Log("\nTEST PASSED\n\n");
        LOS_Log("Parameters   |   EXPOSURE   |   ANALOG GAIN   |   FRAMECOUNTER | GLACE STAT COUNTER\n");
        LOS_Log(
        "Requested|  %d          |       %d             |    %d    | %d    \n",
        ( int ) rev1,
        ( int ) rev2,
        ( int ) g_frameparam_counter,
        ( int ) g_grabLR_count);
        LOS_Log("Feedback|%d|%d|%d|%d\n", ( int ) rev4, ( int ) rev5, ( int ) rev6, ( int ) rev7);
        LOS_Log(
        "Sensor|%d|%d|%d|%d\n",
        ( int ) pFrameParamStatus->u32_ExposureTime_us,
        ( int ) pFrameParamStatus->u32_AnalogGain_x256,
        ( int ) pFrameParamStatus->u32_frame_counter,
        ( int ) rev7);
        LOS_Log(
        "TargetExposureTime :%d | TargetAnalogGain:%d| Frame_req:%d | Frame_rec:%d",
        ( int ) sensorparams[iter - 3],
        ( int ) sensorparams[iter - 2],
        ( int ) sensorparams[iter - 1],
        ( int ) sensorparams[iter]);

        snprintf(
        mess,
        sizeof(mess),
        "Requested|%d|%d|%d|%d",
        ( int ) rev1,
        ( int ) ( int ) rev2,
        ( int ) g_frameparam_counter,
        ( int ) g_grabLR_count);
        mmte_testComment(mess);
        sprintf(mess, "Feedback|%d|%d|%d|%d", ( int ) rev4, ( int ) rev5, ( int ) rev6, ( int ) rev7);
        mmte_testComment(mess);
        snprintf(
        mess,
        sizeof(mess),
        "Sensor|%d|%d|%d|%d",
        ( int ) pFrameParamStatus->u32_ExposureTime_us,
        ( int ) pFrameParamStatus->u32_AnalogGain_x256,
        ( int ) pFrameParamStatus->u32_frame_counter,
        ( int ) rev7);
        mmte_testComment(mess);
        snprintf(
        mess,
        sizeof(mess),
        "TargetExposureTime :%d | TargetAnalogGain:%d| Frame_req:%d | Frame_rec:%d",
        ( int ) sensorparams[iter - 3],
        ( int ) sensorparams[iter - 2],
        ( int ) sensorparams[iter - 1],
        ( int ) sensorparams[iter]);
        mmte_testComment(mess);
        mmte_testResult(TEST_PASSED);
    }


    if ((pFrameParamStatus->u32_ExposureTime_us != rev4) || (pFrameParamStatus->u32_AnalogGain_x256 != rev5))
    {
        rev7 = ITE_readPE(Event0_Count_u16_EVENT0_17_GLACE_STATS_READY_Byte0);
        rev6 = ITE_readPE(FrameParamStatus_u32_frame_counter_Byte0);

        LOS_Log("Test case failed \n");
        LOS_Log("\nTEST FAILED\n\n");
        LOS_Log("Parameters   |   EXPOSURE   |   ANALOG GAIN   |   FRAMECOUNTER | GLACE STAT COUNTER\n");
        LOS_Log(
        "Requested|  %d          |       %d             |    %d    | %d    \n",
        ( int ) rev1,
        ( int ) rev2,
        ( int ) g_frameparam_counter,
        ( int ) g_grabLR_count);
        LOS_Log("Feedback|%d|%d|%d|%d\n", ( int ) rev4, ( int ) rev5, ( int ) rev6, ( int ) rev7);
        LOS_Log(
        "Sensor|%d|%d|%d|%d\n",
        ( int ) pFrameParamStatus->u32_ExposureTime_us,
        ( int ) pFrameParamStatus->u32_AnalogGain_x256,
        ( int ) pFrameParamStatus->u32_frame_counter,
        ( int ) rev7);
        LOS_Log(
        "TargetExposureTime :%d | TargetAnalogGain:%d| Frame_req:%d | Frame_rec:%d",
        ( int ) sensorparams[iter - 3],
        ( int ) sensorparams[iter - 2],
        ( int ) sensorparams[iter - 1],
        ( int ) sensorparams[iter]);

        snprintf(
        mess,
        sizeof(mess),
        "Requested|%d|%d|%d|%d",
        ( int ) rev1,
        ( int ) ( int ) rev2,
        ( int ) g_frameparam_counter,
        ( int ) g_grabLR_count);
        mmte_testComment(mess);
        sprintf(mess, "Feedback|%d|%d|%d|%d", ( int ) rev4, ( int ) rev5, ( int ) rev6, ( int ) rev7);
        mmte_testComment(mess);
        snprintf(
        mess,
        sizeof(mess),
        "Sensor|%d|%d|%d|%d",
        ( int ) pFrameParamStatus->u32_ExposureTime_us,
        ( int ) pFrameParamStatus->u32_AnalogGain_x256,
        ( int ) pFrameParamStatus->u32_frame_counter,
        ( int ) rev7);
        mmte_testComment(mess);
        snprintf(
        mess,
        sizeof(mess),
        "TargetExposureTime :%d | TargetAnalogGain:%d| Frame_req:%d | Frame_rec:%d",
        ( int ) sensorparams[iter - 3],
        ( int ) sensorparams[iter - 2],
        ( int ) sensorparams[iter - 1],
        ( int ) sensorparams[iter]);
        mmte_testComment(mess);
        mmte_testResult(TEST_FAILED);
    }


    mmte_testEnd();
    memset(g_out_path, 0, KlogDirectoryLentgh * sizeof(char));
}


void
ITE_DigitalGainCheck(
char    *loop_count)
{
    volatile float  DGain;

    t_uint32        count,
                    iter,
                    frame_req,
                    frame_rec,
                    dg_r,
                    dg_g,
                    dg_b,
                    dg_r_app,
                    dg_g_app,
                    dg_b_app;

    char            mess[255],
                    filename[256],
                    pathname[256];
    union u_ITE_Event   event;

    count = ITE_ConvToInt16(loop_count);

    sprintf(pathname, IMAGING_PATH "/ite_nmf/test_results_nreg/PipeParamStatus");
    strcat(g_out_path, pathname);
    init_test_results_nreg(g_out_path);
    //glaceblock <HSizeFraction Value> <VSizeFraction Value> <HROIFraction Value> <VROIFraction Value>\n");
    ITE_NMF_SetGlaceBlockFraction(100, 100, 0, 0);

    ITE_NMF_SetGlaceGrid(72, 54);

    //syntax: glacesat <Red Sat Value> <Green Sat Value> <Blue Sat Value>\n");
    ITE_NMF_SetGlaceSatLevel(255, 255, 255);

    //To work with Glace and histogram
    //To work with Glace only: histogram pointer sets to 0 + allocation for FrameParamStatus structure
    pStore = g_pStatsBufferStorage;

    ITE_NMF_UpdateGlaceParam();

    ITE_writePE(Glace_Control_e_GlaceOperationMode_Control_Byte0, GlaceOperationMode_e_Continuous);

    ITE_LRHR_Prepare(&usecase, INFINITY);
    ITE_LRHR_Start(INFINITY);

    for (iter = 0; iter <= count; iter++)
    {
        DGain = ite_rand_FloatRange(1.0, 7.99);

        sprintf(mess, "_DGain__%d_%d", ( int ) DGain, ( int ) iter);
        sprintf(filename, "PipeParamStatus");
        strcat(filename, mess);
        mmte_testStart(filename, "PipeParamStatus", g_out_path);
        sprintf(mess, "Testing for PipeParamStatus");

        sprintf(mess, "Testing for PipeParamStatus");
        mmte_testNext(mess);

        LOS_Log("DG REQUESTED :%f \n", DGain);

        // Digital gain update
        ITE_writePE(WhiteBalanceControl_f_RedManualGain_Byte0, *( volatile t_uint32 * ) &DGain);
        ITE_writePE(WhiteBalanceControl_f_GreenManualGain_Byte0, *( volatile t_uint32 * ) &DGain);
        ITE_writePE(WhiteBalanceControl_f_BlueManualGain_Byte0, *( volatile t_uint32 * ) &DGain);

        frame_req = ITE_readPE(Interrupts_Count_u16_INT05_SMIARX_Byte0);

        if (Coin_e_Heads == ITE_readPE(SensorPipeSettings_Status_e_Coin_ISPSettings_Byte0))
        {
            ITE_writePE(SensorPipeSettings_Control_e_Coin_ISPSettings_Byte0, Coin_e_Tails);
        }
        else
        {
            ITE_writePE(SensorPipeSettings_Control_e_Coin_ISPSettings_Byte0, Coin_e_Heads);
        }


          do
    {
        event = ITE_WaitEvent(ISP_ISP_COMMIT_NOTIFICATION_EVENTS);
        ITE_traceISPCTLevent(event);
        if (event.type == ITE_EVT_ISPCTL_ERROR)
        {
            LOS_Log("\nITE_EVT_ISPCTL_ERROR \n");
            break;
        }
    } while (!(event.ispctlInfo.info_id == ISP_ISP_COMMIT_NOTIFICATION));

        ITE_NMF_WaitStatReady(ISP_GLACE_STATS_READY);

        if
        (
            ITE_readPE(WhiteBalanceControl_f_RedManualGain_Byte0) == ITE_readPE(
                WhiteBalanceStatus_f_RedGain_Byte0)
        &&  ITE_readPE(WhiteBalanceControl_f_BlueManualGain_Byte0) == ITE_readPE(
                WhiteBalanceStatus_f_BlueGain_Byte0)
        &&  ITE_readPE(WhiteBalanceControl_f_GreenManualGain_Byte0) == ITE_readPE(
                WhiteBalanceStatus_f_GreenGain_Byte0)
        )
        {
            frame_rec = ITE_readPE(Interrupts_Count_u16_INT05_SMIARX_Byte0);

            dg_r = ITE_readPE(WhiteBalanceControl_f_RedManualGain_Byte0);
            dg_g = ITE_readPE(WhiteBalanceControl_f_GreenManualGain_Byte0);
            dg_b = ITE_readPE(WhiteBalanceControl_f_BlueManualGain_Byte0);

            dg_r_app = ITE_readPE(WhiteBalanceStatus_f_RedGain_Byte0);
            dg_g_app = ITE_readPE(WhiteBalanceStatus_f_GreenGain_Byte0);
            dg_b_app = ITE_readPE(WhiteBalanceStatus_f_BlueGain_Byte0);

            LOS_Log("Frame Requested:%d Frame Received:%d\n", ( int ) frame_req, ( int ) frame_rec);

            LOS_Log(
            "DG REQUESTED R :%f  G:%f  B:%f\n",
            (*( float * ) &dg_r),
            (*( float * ) &dg_g),
            (*( float * ) &dg_b));

            LOS_Log(
            "DG APPLIED R :%f  G:%f  B:%f\n",
            (*( float * ) &dg_r_app),
            (*( float * ) &dg_g_app),
            (*( float * ) &dg_b_app));

            LOS_Log("\nTEST PASSED\n");

            sprintf(
            mess,
            "DG REQUESTED R :%f  G:%f  B:%f\n",
            (*( float * ) &dg_r),
            (*( float * ) &dg_g),
            (*( float * ) &dg_b));

            mmte_testComment(mess);

            sprintf(
            mess,
            "DG APPLIED R :%f  G:%f  B:%f\n",
            (*( float * ) &dg_r_app),
            (*( float * ) &dg_g_app),
            (*( float * ) &dg_b_app));

            mmte_testComment(mess);

            sprintf(mess, "Frame Requested:%d Frame Received:%d\n", ( int ) frame_req, ( int ) frame_rec);
            mmte_testComment(mess);
            mmte_testResult(TEST_PASSED);
            mmte_testEnd();
        }
        else
        {
            LOS_Log("Frame Requested:%d Frame Received:%d\n", ( int ) frame_req, ( int ) frame_rec);

            LOS_Log(
            "DG REQUESTED R :%f  G:%f  B:%f\n",
            (*( float * ) &dg_r),
            (*( float * ) &dg_g),
            (*( float * ) &dg_b));

            LOS_Log(
            "DG APPLIED R :%f  G:%f  B:%f\n",
            (*( float * ) &dg_r_app),
            (*( float * ) &dg_g_app),
            (*( float * ) &dg_b_app));

            LOS_Log("\nTEST FAILED\n");

            sprintf(
            mess,
            "DG REQUESTED R :%f  G:%f  B:%f\n",
            (*( float * ) &dg_r),
            (*( float * ) &dg_g),
            (*( float * ) &dg_b));

            mmte_testComment(mess);

            sprintf(
            mess,
            "DG APPLIED R :%f  G:%f  B:%f\n",
            (*( float * ) &dg_r_app),
            (*( float * ) &dg_g_app),
            (*( float * ) &dg_b_app));

            mmte_testComment(mess);

            sprintf(mess, "Frame Requested:%d Frame Received:%d\n", ( int ) frame_req, ( int ) frame_rec);
            mmte_testComment(mess);
            mmte_testResult(TEST_FAILED);
            mmte_testEnd();
        }
    }


    memset(g_out_path, 0, KlogDirectoryLentgh * sizeof(char));

    //Stop LRHR pipe
    ITE_LRHR_Stop();
    ITE_LRHR_Free();
    ITE_CleanEvent(ITE_EVT_ISPCTL_STATS_EVENTS);
}

