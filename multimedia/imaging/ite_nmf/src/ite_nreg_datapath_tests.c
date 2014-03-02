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
#include "ite_nreg_datapath_tests.h"
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
#include "ite_nmf_zoom_functions.h"
#include "ite_nmf_statistic_functions.h"

#include <test/api/test.h>
#include <cm/inc/cm_macros.h>

#include <los/api/los_api.h>
#include "ite_main.h"
#include "ite_nmf_sensor_output_mode.h"

extern struct s_grabParams              grabparamsHR;
extern struct s_grabParams              grabparamsLR;
extern volatile t_uint32                g_grabHR_infinite;
extern volatile t_uint32                g_grabLR_infinite;
extern ts_siapicturebuffer              GrabBufferHR[];
extern ts_siapicturebuffer              LCDBuffer;
extern ts_siapicturebuffer              GamBuffer;
extern ts_sensInfo                      SensorsInfo[2];

extern ts_sia_usecase                   usecase;
extern t_uint32                         ite_sensorselect;

extern ts_sia_nonregselect              nonregselect;
extern t_uint8                          ITE_Event_Log_flag;
extern ts_bmlinfo                       BmlInfo;

extern t_bool                           TraceGrabLREvent;
extern t_bool                           TraceBMLEvent;

extern StatsBuffersStorage_t            *pStore;
extern ts_siaMetaDatabuffer             StatsBuffer;
extern StatsBuffersStorage_t            *g_pStatsBufferStorage;

extern volatile t_uint32                g_grabLR_count;
extern volatile t_uint32                g_grabHR_count;
extern volatile t_uint32                g_grabBMS_count;
extern volatile t_uint32                g_statsGlaceHisto_count;
extern volatile tps_siaMetaDatabuffer   pOutpuModeBuffer;
extern ts_preboot                       PreBoot;

// Sensor Or Rx Input Selection. Default Value is Sensor0
extern InputImageSource_te              g_InputImageSource;

void                                    ITE_testLRdatapath (enum e_grabFormat grbformat);
Result_te                               ITE_testStreamLR (tps_sia_usecase p_sia_usecase, float framerate);
void                                    ITE_testStreamLR_allFramerate (tps_sia_usecase p_sia_usecase);
void                                    ITE_testHRdatapath (enum e_grabFormat grbformat);
Result_te                               ITE_testStreamHR (tps_sia_usecase p_sia_usecase, float framerate);
void                                    ITE_testStreamHR_allFramerate (tps_sia_usecase p_sia_usecase);
void                                    ITE_testLRHRdatapath (
                                        enum e_grabFormat   lrgrbformat,
                                        enum e_grabFormat   hrgrbformat,
                                        t_uint8             sameAR);
Result_te                               ITE_testStreamLRHR (tps_sia_usecase p_sia_usecase, float framerate);
void                                    ITE_testStreamLRHR_allFramerate (tps_sia_usecase p_sia_usecase);
void                                    ITE_testBMSdatapath (void);
int                                    ITE_testLRHRBMSdatapath (
                                        enum e_grabFormat   lrgrbformat,
                                        enum e_grabFormat   hrgrbformat,
                                        t_uint8             sameAR,
                                        t_uint8             glaceonly,
                                        t_uint8             Zoom,
                                        t_uint8             statinzoom);
void                                    ITE_testStillBMSdatapath (void);
void                                    ITE_testBMLdatapathLRHR (char RAW[6]);
int                                    ITE_testBMLdatapath (
                                        enum e_grabFormat   lrgrbformat,
                                        enum e_grabFormat   hrgrbformat);
void                                    ITE_testStillBMLdatapath (void);
void                                    ITE_testStreamBMS (tps_sia_usecase p_sia_usecase, e_resolution sizebms);

void                                    ITE_testStreamBML (tps_sia_usecase p_sia_usecase, e_resolution sizebml);
int                                    ITE_testVideoBMSdatapath (
                                        e_resolution        sizelr,
                                        enum e_grabFormat   lrgrbformat,
                                        e_resolution        sizehr,
                                        enum e_grabFormat   hrgrbformat);
int                                    ITE_testVideoBMSdatapath_alloutput (
                                        enum e_grabFormat   lrgrbformat,
                                        enum e_grabFormat   hrgrbformat);
Result_te                               ITE_testStreamBMSVideo (
                                        tps_sia_usecase p_sia_usecase,
                                        float           framerate,
                                        t_uint32        BML);
void                                    ITE_testStreamBMSVideo_allframerate (
                                        tps_sia_usecase p_sia_usecase,
                                        t_uint32        BML);
void                                    ITE_testBufferBMS (void);
void                                    ITE_testBufferBMSVideo (void);
int                                    ITE_testVideoBMLdatapath (
                                        e_resolution        sizelr,
                                        enum e_grabFormat   lrgrbformat,
                                        e_resolution        sizehr,
                                        enum e_grabFormat   hrgrbformat);
int                                    ITE_testVideoBMLdatapath_alloutput (
                                        enum e_grabFormat   lrgrbformat,
                                        enum e_grabFormat   hrgrbformat);

int                                    ITE_testZoomstreaming (
                                        enum e_grabFormat   lrgrbformat,
                                        enum e_grabFormat   hrgrbformat,
                                        char                *pipe);
int                                    ITE_bugZoomBMLStill (
                                        enum e_grabFormat   lrgrbformat,
                                        enum e_grabFormat   hrgrbformat);
int                                    ITE_testZoomBMLStill (
                                        enum e_grabFormat   lrgrbformat,
                                        enum e_grabFormat   hrgrbformat,t_uint32 stripe);
void                                    ITE_testZoomBMLStillLRHR (t_uint32 stripe);
void                                    ITE_testZoomBMLStilldatapath (t_uint32 stripe);
void                                    ITE_testZoomBML (tps_sia_usecase p_sia_usecase, t_uint8 maxstep,t_uint32 stripe);

Result_te                               ITE_testcolorBarBMS (void);
Result_te                               ITE_testcolorBarLR (void);
Result_te                               ITE_testcolorBarLRGeneric (void);
Result_te                               ITE_testcolorBarHR (void);
Result_te                               ITE_testcolorBarHRGeneric (void);
void                                    ITE_testBufferHR (void);
void                                    ITE_testBufferLR (void);

extern void                             ITE_NMF_UpdateGlaceParam (void);
extern unsigned long long               ITE_NMF_GlaceAndHistoExpReqNoWait (void);
extern void                             ITE_NMF_GlaceHistoExpStatsRequestAndWait (void);

TAlgoError                              ITE_NMF_Pan_tilt_Test_FullRange (char *pipe, float MaxDZ);
int                                    ITE_testPan_tiltStreaming (
                                        enum e_grabFormat   lrgrbformat,
                                        enum e_grabFormat   hrgrbformat,
                                        char                *Pipe);
t_uint32                                IsModeValidforBML (
                                        Sensor_Output_Mode_ts   *p_Sensor_Output_Mode_Data,
                                        t_uint32                outpuModeCount,
                                        t_uint32                max_woi_X,
                                        t_uint32                max_woi_Y);

t_uint32                                testNum = 0;

/* -----------------------------------------------------------------------
   FUNCTION : C_ite_test_bug97581_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_test_bug97581_cmd(
int     a_nb_args,
char    **ap_args)
{
    if (a_nb_args == 3)
    {
        ITE_test_bug97581(ap_args[1], ap_args[2]);
    }
    else
    {
        CLI_disp_error("Not correct command arguments\n");
    }


    return (CMD_COMPLETE);
}


/* -----------------------------------------------------------------------
   FUNCTION : ITE_test_bug97581
   PURPOSE  :
   ------------------------------------------------------------------------ */
void
ITE_test_bug97581(
char    *ap_test_id,
char    *ap_grabvpip_options)
{
    ITE_InitUseCase(ap_test_id, ap_grabvpip_options, &usecase);

    //instantiate NMF network and boot xP70
    ITE_Start_Env(&usecase, ap_grabvpip_options, 0, 0);

    ITE_testFirmwareRevision();

    //FAST_RAW8 Test
    usecase.BMS_XSize = 3264;
    usecase.BMS_YSize = 2448;
    usecase.BMS_Mode = BMSSTILL;
    usecase.BMS_output = BAYERSTORE0;
    usecase.BMS_GrbFormat = GRBFMT_FAST_RAW8;

    ITE_LR_Prepare(&usecase, INFINITY);
    ITE_SetFrameRateControl(15, 15);
    ITE_LR_Start(INFINITY);

    LOS_Log("\n1secs stream on LR pipe\n");
    LOS_Sleep(1000);

    ITE_LR_Stop();
    ITE_LR_Free();

    ITE_Event_Log_flag = 0;

    ITE_BMS_Prepare(&usecase, INFINITY);
    ITE_writePE(ChannelGains_Control_e_Flag_EnableChannelGains_Byte0, Flag_e_FALSE);
    ITE_BMS_Start(1);
    LOS_Log("\nStream one frame on BMS \n");
    LOS_Sleep(1000);

    STOPVPIP();
    STARTVPIP();    // start doesn't work, issue here !!!!!!!!!!!!!!!!
    STOPVPIP();
    STARTVPIP();
    STOPVPIP();
    STARTVPIP();
    STOPVPIP();
    STARTVPIP();
    STOPVPIP();
    STARTVPIP();
    STOPVPIP();
    STARTVPIP();
    STOPVPIP();
    STARTVPIP();
    STOPVPIP();
    STARTVPIP();
    STOPVPIP();
    STARTVPIP();
    STOPVPIP();
    STARTVPIP();
    STOPVPIP();
    STARTVPIP();
    STOPVPIP();
    STARTVPIP();
    STOPVPIP();
    STARTVPIP();
    STOPVPIP();
    STARTVPIP();
    STOPVPIP();
    STARTVPIP();
    STOPVPIP();
    STARTVPIP();
    STOPVPIP();
    STARTVPIP();
    STOPVPIP();
    STARTVPIP();
    STOPVPIP();
    STARTVPIP();

    ITE_BMS_Stop();
    ITE_writePE(ChannelGains_Control_e_Flag_EnableChannelGains_Byte0, Flag_e_TRUE);
    ITE_BMS_Free();

    ITE_Stop_Env();

}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_test_bug98168_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_test_bug98168_cmd(
int     a_nb_args,
char    **ap_args)
{
    if (a_nb_args == 3)
    {
        ITE_test_bug98168(ap_args[1], ap_args[2]);
    }
    else
    {
        CLI_disp_error("Not correct command arguments\n");
    }


    return (CMD_COMPLETE);
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_basic_bml_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_basic_bml_cmd(
int     a_nb_args,
char    **ap_args)
{
    int ret = EOK;
    UNUSED(ap_args);
    if (a_nb_args == 1)
    {
        ret = ITE_testBasicBML();
        if(ret != EOK)
        {
            CLI_disp_error("Not correct command arguments C_ite_dbg_zoomtestbug1_cmd\n");
            goto END;
        }

    }
    else
    {
        CLI_disp_error("Not correct command arguments\n");
        goto END;
    }

    END:
    return (CMD_COMPLETE);
}


/* -----------------------------------------------------------------------
   FUNCTION : ITE_testLRdatapath
   PURPOSE  : Test LR datapath for  YUV422 itld format
   for different resolution and framerates
   ------------------------------------------------------------------------ */
void
ITE_test_bug98168(
char    *ap_test_id,
char    *ap_grabvpip_options)
{
    e_resolution    sizelr;
    volatile float  data;

    /* LR Format always YUV422Interleaved */
    usecase.LR_GrbFormat = GRBFMT_YUV422_RASTER_INTERLEAVED;

    sizelr = VGA;

    usecase.LR_XSize = ITE_GiveXSizeFromEnum(PIPE_LR,
                                             sizelr,
                                             GRBFMT_YUV422_RASTER_INTERLEAVED);
    usecase.LR_YSize = ITE_GiveYSizeFromEnum(PIPE_LR,
                                             sizelr,
                                             GRBFMT_YUV422_RASTER_INTERLEAVED);

    ITE_InitUseCase(ap_test_id, ap_grabvpip_options, &usecase);

    // Instantiate NMF network and boot xP70
    ITE_Start_Env(&usecase, ap_grabvpip_options, 0, 0);

    // LR test
    ITE_writePE(HostFrameConstraints_e_Flag_GuaranteeStaticFlickerFrameLength_Byte0,
                Flag_e_FALSE);

    // Set FOV
    data = 1024;
    ITE_writePE(Zoom_Control_f_SetFOVX_Byte0, *( volatile t_uint32 * ) &data);

    // test ok at 1frs
    ITE_SetFrameRateControl(1, 1);

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
    ITE_LR_Stop();
    ITE_LR_Free();

    ITE_SetFrameRateControl(25, 25);

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
    ITE_LR_Stop();

    if (ITE_CheckGrabInBuffer(GrabBufferLR[0].logAddress, GrabBufferLR[0].buffersizeByte, 0xff) == 1)
    {
        LOS_Log("\nTEST OK: GrabBufferLR[0] buffer was filled with values different from default value\n");
    }
    else
    {
        LOS_Log("\nTEST FAIL: GrabBufferLR[0] buffer not properly filled (too many default values in buffer)\n");
    }

    ITE_LR_Free();
}

/* -----------------------------------------------------------------------
   FUNCTION : C_ite_nreg_datapath_cmd
   PURPOSE  : Command for LR ouput pipe non regression
   ------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_nreg_datapath_cmd(
int     a_nb_args,
char    **ap_args)
{
    int ret= EOK;
    if (a_nb_args == 4)
    {
        ret =ITE_nreg_Datapath(ap_args[1], ap_args[2], ap_args[3]);
        if(ret != EOK)
        {
            CLI_disp_error("Not correct command arguments C_ite_nreg_datapath_cmd\n");
            goto END;
        }

    }
    else
    {
        CLI_disp_error("Not correct command arguments\n");
        goto END;
    }

    END:
    return (CMD_COMPLETE);
}

CMD_COMPLETION
C_ite_nreg_LRdatapath_cmd(
int a_nb_args,
char** ap_args)
{
    int ret = EOK;
    enum e_grabFormat afmt;

    if (a_nb_args == 2)
    {
        ret = ITE_GiveFormat(PIPE_LR, ap_args[1], &afmt);
        if (ret != EFMT)
        {
            ITE_testLRdatapath(afmt);
        }
        else
        {
            LOS_Log("ITE_GiveFormat( gave error in %s and line  %d\n",__FUNCTION__,__LINE__);
            goto END;
        }
    }
    else
    {
        CLI_disp_error("Not correct command arguments\n");
        goto END;
    }

END:
    return (CMD_COMPLETE);
}

CMD_COMPLETION
C_ite_nreg_HRdatapath_cmd(
int a_nb_args,
char** ap_args)
{
    int ret = EOK;
    enum e_grabFormat afmt;

    if (a_nb_args == 2)
    {
        ret = ITE_GiveFormat(PIPE_HR, ap_args[1], &afmt);
        if (ret != EFMT)
        {
            ITE_testHRdatapath(afmt);
        }
        else
        {
            LOS_Log("ITE_GiveFormat( gave error in %s and line  %d\n",__FUNCTION__,__LINE__);
            goto END;
        }
    }
    else
    {
        CLI_disp_error("Not correct command arguments\n");
        goto END;
    }

END:
    return (CMD_COMPLETE);
}

CMD_COMPLETION
C_ite_nreg_LRHRdatapath_cmd(
int a_nb_args,
char** ap_args)
{
    int ret = EOK;
    enum e_grabFormat afmt1, afmt2;

    if (a_nb_args == 3)
    {
        ret = ITE_GiveFormat(PIPE_LR, ap_args[1], &afmt1);
        if (ret != EOK)
        {
            LOS_Log("ITE_GiveFormat gave error in %s and line  %d\n",__FUNCTION__,__LINE__);
            CLI_disp_error("Not correct command arguments in C_ite_nreg_LRHRdatapath_cmd\n");
            goto END;
        }
        ret = ITE_GiveFormat(PIPE_HR, ap_args[2], &afmt2);
        if (ret != EOK)
        {
            LOS_Log("ITE_GiveFormat gave error in %s and line  %d\n",__FUNCTION__,__LINE__);
            CLI_disp_error("Not correct command arguments in C_ite_nreg_LRHRdatapath_cmd 2\n");
            goto END;
        }

        ITE_testLRHRdatapath(afmt1, afmt2, 1);
    }
    else
    {
        CLI_disp_error("Not correct command arguments\n");
        goto END;
    }

END:
    return (CMD_COMPLETE);
}

CMD_COMPLETION
C_ite_nreg_BMSdatapath_cmd(
int     a_nb_args,
char    **ap_args)
{
    UNUSED(ap_args);
    
    if (a_nb_args == 1)
    {
        ITE_testBMSdatapathForSensoroutputModes();
    }
    else
    {
        CLI_disp_error("Not correct command arguments\n");
    }


    return (CMD_COMPLETE);
}

CMD_COMPLETION
C_ite_nreg_VideoBMSdatapath_cmd(
int a_nb_args,
char** ap_args)
{
    int ret = EOK;
    enum e_grabFormat afmt1,afmt2;

    if (a_nb_args == 3)
    {
        ret = ITE_GiveFormat(PIPE_LR, ap_args[1], &afmt1);
        if (ret != EOK)
        {
            LOS_Log("ITE_GiveFormat gave error in %s and line  %d\n",__FUNCTION__,__LINE__);
            CLI_disp_error("Not correct command arguments in C_ite_nreg_VideoBMSdatapath_cmd\n");
            goto END;
        }
        ret = ITE_GiveFormat(PIPE_HR, ap_args[2], &afmt2);
        if (ret != EOK)
        {
            LOS_Log("ITE_GiveFormat gave error in %s and line  %d\n",__FUNCTION__,__LINE__);
            CLI_disp_error("Not correct command arguments in C_ite_nreg_VideoBMSdatapath_cmd 2\n");
            goto END;
        }

        ret = ITE_testVideoBMSdatapath_alloutput(afmt1, afmt2);
        if(ret != EOK)
        {
            CLI_disp_error("Not correct command arguments C_ite_nreg_VideoBMSdatapath_cmd 3\n");
            goto END;
        }
    }
    else
    {
        CLI_disp_error("Not correct command arguments\n");
        goto END;
    }

END:
    return (CMD_COMPLETE);
}

CMD_COMPLETION
C_ite_nreg_LRHRBMSdatapath_cmd(
int a_nb_args,
char** ap_args)
{
    int ret = EOK;
    enum e_grabFormat afmt1,afmt2;

    if (a_nb_args == 7)
    {
        ret = ITE_GiveFormat(PIPE_LR, ap_args[1], &afmt1);
        if (ret != EOK)
        {
            LOS_Log("ITE_GiveFormat gave error in %s and line  %d\n",__FUNCTION__,__LINE__);
            CLI_disp_error("Not correct command arguments in C_ite_nreg_LRHRBMSdatapath_cmd\n");
            goto END;
        }
        ret = ITE_GiveFormat(PIPE_HR, ap_args[2], &afmt2);
        if (ret != EOK)
        {
            LOS_Log("ITE_GiveFormat gave error in %s and line  %d\n",__FUNCTION__,__LINE__);
            CLI_disp_error("Not correct command arguments in C_ite_nreg_LRHRBMSdatapath_cmd 2\n");
            goto END;
        }

        ret = ITE_testLRHRBMSdatapath(afmt1,
                                      afmt2,
                                      atoi(ap_args[3]),
                                      atoi(ap_args[4]),
                                      atoi(ap_args[5]),
                                      atoi(ap_args[6]));
        if(ret != EOK)
        {
            CLI_disp_error("Not correct command arguments C_ite_nreg_LRHRBMSdatapath_cmd\n");
            goto END;
        }
    }
    else
    {
        CLI_disp_error("Not correct command arguments\n");
        goto END;
    }

END:
    return (CMD_COMPLETE);
}

CMD_COMPLETION
C_ite_nreg_BMLdatapath_cmd(
int a_nb_args,
char** ap_args)
{
    int ret = EOK;
    enum e_grabFormat afmt1,afmt2;

    if (a_nb_args == 3)
    {
        ret = ITE_GiveFormat(PIPE_LR, ap_args[1], &afmt1);
        if (ret != EOK)
        {
            LOS_Log("ITE_GiveFormat gave error in %s and line  %d\n",__FUNCTION__,__LINE__);
            CLI_disp_error("Not correct command arguments in C_ite_nreg_BMLdatapath_cmd\n");
            goto END;
        }
        ret = ITE_GiveFormat(PIPE_HR, ap_args[2], &afmt2);
        if (ret != EOK)
        {
            LOS_Log("ITE_GiveFormat gave error in %s and line  %d\n",__FUNCTION__,__LINE__);
            CLI_disp_error("Not correct command arguments in C_ite_nreg_BMLdatapath_cmd 2\n");
            goto END;
        }

        ret = ITE_testBMLdatapathForSensoroutputModes(afmt1, afmt2);
        if (ret != EOK)
        {
            LOS_Log("ITE_testBMLdatapathForSensoroutputModes gave error in %s and line  %d\n",__FUNCTION__,__LINE__);
            CLI_disp_error("Not correct command arguments in C_ite_nreg_BMLdatapath_cmd 3\n");
            goto END;
        }
    }
    else
    {
        CLI_disp_error("Not correct command arguments\n");
        goto END;
    }

END:
    return (CMD_COMPLETE);
}

CMD_COMPLETION
C_ite_nreg_VideoBMLdatapath_cmd(
int a_nb_args,
char** ap_args)
{
    int ret = EOK;
    enum e_grabFormat afmt1,afmt2;

    if (a_nb_args == 3)
    {
        ret = ITE_GiveFormat(PIPE_LR, ap_args[1], &afmt1);
        if (ret != EOK)
        {
            LOS_Log("ITE_GiveFormat gave error in %s and line  %d\n",__FUNCTION__,__LINE__);
            CLI_disp_error("Not correct command arguments in C_ite_nreg_VideoBMLdatapath_cmd\n");
            goto END;
        }
        ret = ITE_GiveFormat(PIPE_HR, ap_args[2], &afmt2);
        if (ret != EOK)
        {
            LOS_Log("ITE_GiveFormat gave error in %s and line  %d\n",__FUNCTION__,__LINE__);
            CLI_disp_error("Not correct command arguments in C_ite_nreg_VideoBMLdatapath_cmd 2\n");
            goto END;
        }

        ret = ITE_testVideoBMLdatapath_alloutput(afmt1, afmt2);
        if(ret != EOK)
        {
            CLI_disp_error("Not correct command arguments C_ite_nreg_VideoBMLdatapath_cmd 3\n");
            goto END;
        }
    }
    else
    {
        CLI_disp_error("Not correct command arguments\n");
        goto END;
    }

END:
    return (CMD_COMPLETE);
}

CMD_COMPLETION
C_ite_nreg_ZoomBMLdatapath_cmd(
int a_nb_args,
char** ap_args)
{
    int ret = EOK;
    enum e_grabFormat afmt1,afmt2;

    if (a_nb_args == 3)
    {
        ret = ITE_GiveFormat(PIPE_LR, ap_args[1], &afmt1);
        if (ret != EOK)
        {
            LOS_Log("ITE_GiveFormat gave error in %s and line  %d\n",__FUNCTION__,__LINE__);
            CLI_disp_error("Not correct command arguments in C_ite_nreg_ZoomBMLdatapath_cmd\n");
            goto END;
        }
        ret = ITE_GiveFormat(PIPE_HR, ap_args[2], &afmt2);
        if (ret != EOK)
        {
            LOS_Log("ITE_GiveFormat gave error in %s and line  %d\n",__FUNCTION__,__LINE__);
            CLI_disp_error("Not correct command arguments in C_ite_nreg_ZoomBMLdatapath_cmd 2\n");
            goto END;
        }

        ret = ITE_testZoomBMLStillForSensoroutputModes(afmt1, afmt2, 0);
        if(ret != EOK)
        {
            CLI_disp_error("Not correct command arguments C_ite_nreg_ZoomBMLdatapath_cmd\n");
            goto END;
        }
    }
    else if (a_nb_args == 4)
    {
        if ((atoi(ap_args[3]) < 0) || (atoi(ap_args[3]) > 2))
        {
            CLI_disp_error("Not correct command arguments\n");
        }
        else
        {
            ret = ITE_GiveFormat(PIPE_LR, ap_args[1], &afmt1);
            if (ret != EOK)
            {
                LOS_Log("ITE_GiveFormat gave error in %s and line  %d\n",__FUNCTION__,__LINE__);
                CLI_disp_error("Not correct command arguments in C_ite_nreg_ZoomBMLdatapath_cmd\n");
                goto END;
            }
            ret = ITE_GiveFormat(PIPE_HR, ap_args[2], &afmt2);
            if (ret != EOK)
            {
                LOS_Log("ITE_GiveFormat gave error in %s and line  %d\n",__FUNCTION__,__LINE__);
                CLI_disp_error("Not correct command arguments in C_ite_nreg_ZoomBMLdatapath_cmd 2\n");
                goto END;
            }

            ITE_testZoomBMLStillForSensoroutputModes(afmt1, afmt2, atoi(ap_args[3]));

            LOS_Log("Data-1 = %s, Data-2 = %s, Data-3 = %s\n", ap_args[1], ap_args[2], ap_args[3]);
        }
    }
    else
    {
        CLI_disp_error("Not correct command arguments\n");
        goto END;
    }

END:
    return (CMD_COMPLETE);
}

CMD_COMPLETION
C_ite_nreg_ZoomStreaming_cmd(
int a_nb_args,
char** ap_args)
{
    int ret = EOK;
    enum e_grabFormat afmt1,afmt2;

    if (a_nb_args == 3)
    {
        ret = ITE_GiveFormat(PIPE_LR, ap_args[1], &afmt1);
        if (ret != EOK)
        {
            LOS_Log("ITE_GiveFormat gave error in %s and line  %d\n",__FUNCTION__,__LINE__);
            CLI_disp_error("Not correct command arguments in C_ite_nreg_ZoomStreaming_cmd\n");
            goto END;
        }
        ret = ITE_GiveFormat(PIPE_HR, ap_args[2], &afmt2);
        if (ret != EOK)
        {
            LOS_Log("ITE_GiveFormat gave error in %s and line  %d\n",__FUNCTION__,__LINE__);
            CLI_disp_error("Not correct command arguments in C_ite_nreg_ZoomStreaming_cmd 2\n");
            goto END;
        }

        ret = ITE_testZoomstreaming(afmt1, afmt2, "LR");
        if (ret != EOK)
        {
            CLI_disp_error("Not correct command arguments C_ite_nreg_ZoomStreaming_cmd\n");
            goto END;
        }
    }
    else
    {
        CLI_disp_error("Not correct command arguments\n");
        goto END;
    }

END:
    return (CMD_COMPLETE);
}

t_uint32
IsModeValidforBML(
Sensor_Output_Mode_ts   *p_Sensor_Output_Mode_Data,
t_uint32                outpuModeCount,
t_uint32                max_woi_X,
t_uint32                max_woi_Y)
{
    t_uint32    u32_mode_valid = FALSE;

		
    if ((p_Sensor_Output_Mode_Data[outpuModeCount].u32_data_format != PreBoot.sensor.uwCsiRawFormat_MSByte) ||
        ((p_Sensor_Output_Mode_Data[outpuModeCount].u32_usage_restriction_bitmask & (0x1<<LLD_USAGE_MODE_STILL_CAPTURE))) // If not a still capture mode, ignore it
	)
    {
        return (FALSE);
    }


    if
    (
        (
            (p_Sensor_Output_Mode_Data[outpuModeCount].u32_woi_res_width == max_woi_X)
        &&  (
                p_Sensor_Output_Mode_Data[outpuModeCount].u32_woi_res_height == p_Sensor_Output_Mode_Data[outpuModeCount].
                    u32_output_res_height
            )
        )
    ||  (
            (p_Sensor_Output_Mode_Data[outpuModeCount].u32_woi_res_height == max_woi_Y)
        &&  (
                p_Sensor_Output_Mode_Data[outpuModeCount].u32_woi_res_width == p_Sensor_Output_Mode_Data[outpuModeCount].
                    u32_output_res_width
            )
        )
    )
    {
        u32_mode_valid = TRUE;
    }


    return (u32_mode_valid);
}


void
ITE_testBMSdatapathForSensoroutputModes(void)
{
    Sensor_Output_Mode_ts   *p_Sensor_Output_Mode_Data = ( Sensor_Output_Mode_ts * ) pOutpuModeBuffer->logAddress;
    t_uint32                sensorOutputModeDataSize = ITE_readPE(
        ReadLLAConfig_Status_u32_sensor_Output_Mode_data_size_Byte0);
    t_uint32                sensorOutputModeSturctSize = sizeof(Sensor_Output_Mode_ts);
    t_uint32                outpuModeCount = 0;
    float fps=0.0;
    LOS_Log("Total number of output modes = %d\n", sensorOutputModeDataSize / sensorOutputModeSturctSize);

    for (outpuModeCount = 0; outpuModeCount < sensorOutputModeDataSize / sensorOutputModeSturctSize; outpuModeCount++)
    {
        LOS_Log("outpuModeCount = %d", outpuModeCount);
        usecase.BMS_woi_X = p_Sensor_Output_Mode_Data[outpuModeCount].u32_woi_res_width;
        usecase.BMS_woi_Y = p_Sensor_Output_Mode_Data[outpuModeCount].u32_woi_res_height;

        usecase.BMS_YSize = p_Sensor_Output_Mode_Data[outpuModeCount].u32_output_res_height;
        usecase.BMS_XSize = p_Sensor_Output_Mode_Data[outpuModeCount].u32_output_res_width;

        usecase.framerate_x100 = p_Sensor_Output_Mode_Data[outpuModeCount].u32_max_frame_rate_x100;
        fps = usecase.framerate_x100/100;
        ITE_SetFrameRateControl(fps,fps);
        usecase.data_format = p_Sensor_Output_Mode_Data[outpuModeCount].u32_data_format;

        ITE_testBMSdatapath();
    }
}


int
ITE_testBMLdatapathForSensoroutputModes(
enum e_grabFormat   lrgrbformat,
enum e_grabFormat   hrgrbformat)
{
    int ret = EOK;
    Sensor_Output_Mode_ts   *p_Sensor_Output_Mode_Data = ( Sensor_Output_Mode_ts * ) pOutpuModeBuffer->logAddress;
    t_uint32                sensorOutputModeDataSize = ITE_readPE(
        ReadLLAConfig_Status_u32_sensor_Output_Mode_data_size_Byte0);
    t_uint32                sensorOutputModeSturctSize = sizeof(Sensor_Output_Mode_ts);
    t_uint32                outpuModeCount = 0;
    t_uint32                max_woi_X = 0;
    t_uint32                max_woi_Y = 0;
    float fps=0.0;
    LOS_Log("Total number of output modes = %d\n", sensorOutputModeDataSize / sensorOutputModeSturctSize);

    // <Hem> Due to time constratints, we are testing only FFOV and Binning resolutions as of now.
    // find max_woi_x, and max_woi_Y for doing this. And test resolutions corresponding to these
    // resolutions only
    for (outpuModeCount = 0; outpuModeCount < sensorOutputModeDataSize / sensorOutputModeSturctSize; outpuModeCount++)
    {
        if (p_Sensor_Output_Mode_Data[outpuModeCount].u32_woi_res_width > max_woi_X)
        {
            max_woi_X = p_Sensor_Output_Mode_Data[outpuModeCount].u32_woi_res_width;
        }


        if (p_Sensor_Output_Mode_Data[outpuModeCount].u32_woi_res_height > max_woi_Y)
        {
            max_woi_Y = p_Sensor_Output_Mode_Data[outpuModeCount].u32_woi_res_height;
        }
    }

    for (outpuModeCount = 0; outpuModeCount < sensorOutputModeDataSize / sensorOutputModeSturctSize; outpuModeCount++)
    {
#if 0
        if
        (
            (
                p_Sensor_Output_Mode_Data[outpuModeCount].u32_woi_res_width == max_woi_X
            ||  p_Sensor_Output_Mode_Data[outpuModeCount].u32_woi_res_height == max_woi_Y
            )
        &&  p_Sensor_Output_Mode_Data[outpuModeCount].u32_data_format == PreBoot.sensor.uwCsiRawFormat_MSByte   //
        )
#else
            if (IsModeValidforBML(p_Sensor_Output_Mode_Data, outpuModeCount, max_woi_X, max_woi_Y))
#endif
            {
                LOS_Log("outpuModeCount = %d", outpuModeCount);
                usecase.BMS_woi_X = p_Sensor_Output_Mode_Data[outpuModeCount].u32_woi_res_width;
                usecase.BMS_woi_Y = p_Sensor_Output_Mode_Data[outpuModeCount].u32_woi_res_height;

                usecase.BMS_YSize = p_Sensor_Output_Mode_Data[outpuModeCount].u32_output_res_height;
                usecase.BMS_XSize = p_Sensor_Output_Mode_Data[outpuModeCount].u32_output_res_width;

                usecase.framerate_x100 = p_Sensor_Output_Mode_Data[outpuModeCount].u32_max_frame_rate_x100;
                fps = usecase.framerate_x100/100;
                ITE_SetFrameRateControl(fps,fps);
                usecase.data_format = p_Sensor_Output_Mode_Data[outpuModeCount].u32_data_format;
                g_InputImageSource = InputImageSource_e_Rx;
                


                ret = ITE_testBMLdatapath(lrgrbformat, hrgrbformat);
                if(ret != EOK)
                {
                    LOS_Log("ITE_testBMLdatapath gave error in %s and line  %d\n",__FUNCTION__,__LINE__);
                    goto END;
                }

            }
    }
    END:
    return ret;
}


int
ITE_testZoomBMLStillForSensoroutputModes(
enum e_grabFormat   lrgrbformat,
enum e_grabFormat   hrgrbformat,t_uint32 stripe)
{
    Sensor_Output_Mode_ts   *p_Sensor_Output_Mode_Data = ( Sensor_Output_Mode_ts * ) pOutpuModeBuffer->logAddress;
    t_uint32                sensorOutputModeDataSize = ITE_readPE(
        ReadLLAConfig_Status_u32_sensor_Output_Mode_data_size_Byte0);
    t_uint32                sensorOutputModeSturctSize = sizeof(Sensor_Output_Mode_ts);
    t_uint32                outpuModeCount = 0;
    t_uint32                max_woi_X = 0;
    t_uint32                max_woi_Y = 0;
    float fps=0.0;
    int ret = EOK;
    LOS_Log("Total number of output modes = %d\n", sensorOutputModeDataSize / sensorOutputModeSturctSize);

    // <Hem> Due to time constratints, we are testing only FFOV and Binning resolutions as of now.
    // find max_woi_x, and max_woi_Y for doing this. And test resolutions corresponding to these
    // resolutions only
    for (outpuModeCount = 0; outpuModeCount < sensorOutputModeDataSize / sensorOutputModeSturctSize; outpuModeCount++)
    {
        if (p_Sensor_Output_Mode_Data[outpuModeCount].u32_woi_res_width > max_woi_X)
        {
            max_woi_X = p_Sensor_Output_Mode_Data[outpuModeCount].u32_woi_res_width;
        }


        if (p_Sensor_Output_Mode_Data[outpuModeCount].u32_woi_res_height > max_woi_Y)
        {
            max_woi_Y = p_Sensor_Output_Mode_Data[outpuModeCount].u32_woi_res_height;
        }
    }


    for (outpuModeCount = 0; outpuModeCount < sensorOutputModeDataSize / sensorOutputModeSturctSize; outpuModeCount++)
    {
#if 0
        if
        (
            (
                p_Sensor_Output_Mode_Data[outpuModeCount].u32_woi_res_width == max_woi_X
            ||  p_Sensor_Output_Mode_Data[outpuModeCount].u32_woi_res_height == max_woi_Y
            )
        &&  p_Sensor_Output_Mode_Data[outpuModeCount].u32_data_format == PreBoot.sensor.uwCsiRawFormat_MSByte   //
        )
#else
            if (IsModeValidforBML(p_Sensor_Output_Mode_Data, outpuModeCount, max_woi_X, max_woi_Y))
#endif
            {
                LOS_Log("outpuModeCount = %d stripe %d \n", outpuModeCount,stripe);
                usecase.BMS_woi_X = p_Sensor_Output_Mode_Data[outpuModeCount].u32_woi_res_width;
                usecase.BMS_woi_Y = p_Sensor_Output_Mode_Data[outpuModeCount].u32_woi_res_height;

                usecase.BMS_YSize = p_Sensor_Output_Mode_Data[outpuModeCount].u32_output_res_height;
                usecase.BMS_XSize = p_Sensor_Output_Mode_Data[outpuModeCount].u32_output_res_width;

                usecase.framerate_x100 = p_Sensor_Output_Mode_Data[outpuModeCount].u32_max_frame_rate_x100;
                fps = usecase.framerate_x100/100;
                ITE_SetFrameRateControl(fps,fps);
                usecase.data_format = p_Sensor_Output_Mode_Data[outpuModeCount].u32_data_format;

                ret =ITE_testZoomBMLStill(lrgrbformat, hrgrbformat,stripe);
                if(ret != EOK)
                {
                    LOS_Log("ITE_testZoomBMLStill gave error in %s and line  %d\n",__FUNCTION__,__LINE__);
                    goto END;
                }

            }
    }
    END:
    return ret;
}


/* -----------------------------------------------------------------------
   FUNCTION : ITE_nreg_Datapath
   PURPOSE  : Test different datapath (LR,HR,BMS) , check ouput frame automaticaly and framerates
   ------------------------------------------------------------------------ */
int
ITE_nreg_Datapath(
char    *ap_test_id,
char    *ap_grabvpip_options,
char    *sensor_tunning)
{
    t_cm_error  err;
    t_uint8     sensor_tunning_preboot;
    int ret =EOK;
    sensor_tunning_preboot = ITE_ConvToInt8(sensor_tunning);
    ITE_InitUseCase(ap_test_id, ap_grabvpip_options, &usecase);

    //instantiate NMF network and boot xP70
    ITE_Start_Env(&usecase, ap_grabvpip_options, 0, sensor_tunning_preboot);

    err = CM_SetMode(CM_CMD_TRACE_ON, 0);
    if (err != CM_OK)
    {
        LOS_Log("CM_SetMode(CM_CMD_TRACE_ON,0): Failed error %d\n", err);
    }


    err = CM_SetMode(CM_CMD_MPC_TRACE_ON, 2);
    if (err != CM_OK)
    {
        LOS_Log("CM_SetMode(CM_CMD_MPC_TRACE_ON, 2): Failed error %d\n", err);
    }


    err = CM_SetMode(CM_CMD_TRACE_LEVEL, 3);
    if (err != CM_OK)
    {
        LOS_Log("CM_SetMode(CM_CMD_TRACE_LEVEL, 3): Failed error %d\n", err);
    }


    ITE_testFirmwareRevision();

    if (nonregselect.lr.nonreg1)
    {
        ITE_dbg_usagemode("vf");
        nonregselect.lr.nonreg1 = 2;
        ITE_testLRdatapath(GRBFMT_R5G6B5);
        ITE_testLRdatapath(GRBFMT_YUV422_RASTER_INTERLEAVED);
        nonregselect.lr.nonreg1 = 3;
    }


    if (nonregselect.hr.nonreg1)
    {
        ITE_dbg_usagemode("video");
        nonregselect.hr.nonreg1 = 2;
        ITE_testHRdatapath(GRBFMT_YUV420_MB_SEMIPLANAR_DECIMATED);
        nonregselect.hr.nonreg1 = 3;
    }


    if (nonregselect.hr.nonreg2)
    {
        ITE_dbg_usagemode("video");
        nonregselect.hr.nonreg2 = 2;
        ITE_testHRdatapath(GRBFMT_YUV420_RASTER_PLANAR_I420);
        nonregselect.hr.nonreg2 = 3;
    }


    if (nonregselect.lrhr.nonreg1)
    {
        ITE_dbg_usagemode("video");
        nonregselect.lrhr.nonreg1 = 2;
        ITE_testLRHRdatapath(GRBFMT_YUV422_RASTER_INTERLEAVED, GRBFMT_YUV420_MB_SEMIPLANAR_DECIMATED, 1);
        nonregselect.lrhr.nonreg1 = 3;
    }


    if (nonregselect.lrhr.nonreg2)
    {
        ITE_dbg_usagemode("video");
        nonregselect.lrhr.nonreg2 = 2;
        ITE_testLRHRdatapath(GRBFMT_YUV422_RASTER_INTERLEAVED, GRBFMT_YUV420_RASTER_PLANAR_I420, 1);
        nonregselect.lrhr.nonreg2 = 3;
    }


    if (nonregselect.lrhr.nonreg3)
    {
        ITE_dbg_usagemode("video");
        nonregselect.lrhr.nonreg3 = 2;
        ITE_testLRHRdatapath(GRBFMT_YUV422_RASTER_INTERLEAVED, GRBFMT_YUV420_RASTER_PLANAR_I420, 0);
        nonregselect.lrhr.nonreg3 = 3;
    }


    if (nonregselect.lrhr.nonreg4)
    {
        ITE_dbg_usagemode("video");
        nonregselect.lrhr.nonreg4 = 2;
        ITE_testLRHRdatapath(GRBFMT_YUV422_RASTER_INTERLEAVED, GRBFMT_YUV420_MB_SEMIPLANAR_DECIMATED, 0);
        nonregselect.lrhr.nonreg4 = 3;
    }


    if (nonregselect.bms.nonreg1)
    {
        ITE_dbg_usagemode("still");
        nonregselect.bms.nonreg1 = 2;
        ITE_testBMSdatapathForSensoroutputModes();
        nonregselect.bms.nonreg1 = 3;
    }


    if (nonregselect.bms.nonreg2)
    {
        ITE_dbg_usagemode("still");
        nonregselect.bms.nonreg2 = 2;
        ret = ITE_testVideoBMSdatapath_alloutput(GRBFMT_YUV422_RASTER_INTERLEAVED, GRBFMT_YUV420_MB_SEMIPLANAR_DECIMATED);
        if(ret != EOK)
        {
            LOS_Log("ITE_testVideoBMSdatapath_alloutput gave error in %s and line  %d\n",__FUNCTION__,__LINE__);
            goto END;
        }

        nonregselect.bms.nonreg2 = 3;
    }


    //[sudeep] not in non reg menu
    if (nonregselect.bms.nonreg3)
    {
        nonregselect.bms.nonreg3 = 2;
        ret = ITE_testLRHRBMSdatapath(GRBFMT_YUV422_RASTER_INTERLEAVED, GRBFMT_YUV420_RASTER_PLANAR_I420, 1, 0, 0, 0);    // sameAR;Glace and histo      ;no zoom;no stat during zoom
        if(ret != EOK)
        {
            LOS_Log("ITE_testLRHRBMSdatapath gave error in %s and line  %d\n",__FUNCTION__,__LINE__);
            goto END;
        }
        ret = ITE_testLRHRBMSdatapath(GRBFMT_YUV422_RASTER_INTERLEAVED, GRBFMT_YUV420_RASTER_PLANAR_I420, 1, 1, 0, 0);    // sameAR;Glace only (no histo);no zoom;no stat during zoom
        if(ret != EOK)
        {
            LOS_Log("ITE_testLRHRBMSdatapath gave error in %s and line  %d\n",__FUNCTION__,__LINE__);
            goto END;
        }
        ret = ITE_testLRHRBMSdatapath(GRBFMT_YUV422_RASTER_INTERLEAVED, GRBFMT_YUV420_RASTER_PLANAR_I420, 1, 1, 1, 0);    // sameAR;Glace only (no histo);zoom   ;no stat during zoom
        if(ret != EOK)
        {
            LOS_Log("ITE_testLRHRBMSdatapath gave error in %s and line  %d\n",__FUNCTION__,__LINE__);
            goto END;
        }
        ret = ITE_testLRHRBMSdatapath(GRBFMT_YUV422_RASTER_INTERLEAVED, GRBFMT_YUV420_RASTER_PLANAR_I420, 1, 0, 1, 0);    // sameAR;Glace and histo      ;zoom   ;no stat during zoom
        if(ret != EOK)
        {
            LOS_Log("ITE_testLRHRBMSdatapath gave error in %s and line  %d\n",__FUNCTION__,__LINE__);
            goto END;
        }
        ret = ITE_testLRHRBMSdatapath(GRBFMT_YUV422_RASTER_INTERLEAVED, GRBFMT_YUV420_RASTER_PLANAR_I420, 1, 1, 1, 1);    // sameAR;Glace only (no histo);zoom   ;stat during zoom
        if(ret != EOK)
        {
            LOS_Log("v gave error in %s and line  %d\n",__FUNCTION__,__LINE__);
            goto END;
        }
        ret = ITE_testLRHRBMSdatapath(GRBFMT_YUV422_RASTER_INTERLEAVED, GRBFMT_YUV420_RASTER_PLANAR_I420, 1, 0, 1, 1);    // sameAR;Glace and histo      ;zoom   ;stat during zoom
        if(ret != EOK)
        {
            LOS_Log("ITE_testLRHRBMSdatapath gave error in %s and line  %d\n",__FUNCTION__,__LINE__);
            goto END;
        }

        nonregselect.bms.nonreg3 = 3;
    }


    //[sudeep] not in non reg menu
    if (nonregselect.bms.nonreg4)
    {
        nonregselect.bms.nonreg4 = 2;
        ret = ITE_testLRHRBMSdatapath(GRBFMT_YUV422_RASTER_INTERLEAVED, GRBFMT_YUV420_RASTER_PLANAR_I420, 0, 0, 0, 0);    // AR difer;Glace and histo      ;no zoom;no stat during zoom
        if(ret != EOK)
        {
            LOS_Log("ITE_testLRHRBMSdatapath gave error in %s and line  %d\n",__FUNCTION__,__LINE__);
            goto END;
        }
        ret = ITE_testLRHRBMSdatapath(GRBFMT_YUV422_RASTER_INTERLEAVED, GRBFMT_YUV420_RASTER_PLANAR_I420, 0, 1, 0, 0);    // AR difer;Glace only (no histo);no zoom;no stat during zoom
        if(ret != EOK)
        {
            LOS_Log("ITE_testLRHRBMSdatapath gave error in %s and line  %d\n",__FUNCTION__,__LINE__);
            goto END;
        }
        ret = ITE_testLRHRBMSdatapath(GRBFMT_YUV422_RASTER_INTERLEAVED, GRBFMT_YUV420_RASTER_PLANAR_I420, 0, 1, 1, 0);    // AR difer;Glace only (no histo);zoom   ;no stat during zoom
        if(ret != EOK)
        {
            LOS_Log("ITE_testLRHRBMSdatapath gave error in %s and line  %d\n",__FUNCTION__,__LINE__);
            goto END;
        }
        ret = ITE_testLRHRBMSdatapath(GRBFMT_YUV422_RASTER_INTERLEAVED, GRBFMT_YUV420_RASTER_PLANAR_I420, 0, 0, 1, 0);    // AR difer;Glace and histo      ;zoom   ;no stat during zoom
        if(ret != EOK)
        {
            LOS_Log("ITE_testLRHRBMSdatapath gave error in %s and line  %d\n",__FUNCTION__,__LINE__);
            goto END;
        }
        ret = ITE_testLRHRBMSdatapath(GRBFMT_YUV422_RASTER_INTERLEAVED, GRBFMT_YUV420_RASTER_PLANAR_I420, 0, 1, 1, 1);    // AR difer;Glace only (no histo);zoom   ;stat during zoom
        if(ret != EOK)
        {
            LOS_Log("ITE_testLRHRBMSdatapath gave error in %s and line  %d\n",__FUNCTION__,__LINE__);
            goto END;
        }
        ret = ITE_testLRHRBMSdatapath(GRBFMT_YUV422_RASTER_INTERLEAVED, GRBFMT_YUV420_RASTER_PLANAR_I420, 0, 0, 1, 1);    // AR difer;Glace and histo      ;zoom   ;stat during zoom
        if(ret != EOK)
        {
            LOS_Log("ITE_testLRHRBMSdatapath gave error in %s and line  %d\n",__FUNCTION__,__LINE__);
            goto END;
        }
        nonregselect.bms.nonreg4 = 3;
    }


    if (nonregselect.bml.nonreg1)
    {
        nonregselect.bml.nonreg1 = 2;
        ret = ITE_testBMLdatapathForSensoroutputModes(GRBFMT_R5G6B5, GRBFMT_YUV420_MB_SEMIPLANAR_DECIMATED);
        if(ret != EOK)
        {
            LOS_Log("ITE_testBMLdatapathForSensoroutputModes gave error in %s and line  %d\n",__FUNCTION__,__LINE__);
            goto END;
        }

        nonregselect.bml.nonreg1 = 3;
    }


    if (nonregselect.bml.nonreg2)
    {
        nonregselect.bml.nonreg2 = 2;
        ITE_testBMLdatapathForSensoroutputModes(GRBFMT_YUV422_RASTER_INTERLEAVED, GRBFMT_YUV420_RASTER_PLANAR_I420);
        nonregselect.bml.nonreg2 = 3;
    }


    if (nonregselect.bml.nonreg3)
    {
        nonregselect.bml.nonreg3 = 2;
        ITE_testBMLdatapathForSensoroutputModes(GRBFMT_R5G6B5, GRBFMT_YUV422_RASTER_INTERLEAVED);
        nonregselect.bml.nonreg3 = 3;
    }


    if (nonregselect.bml.nonreg4)
    {
        nonregselect.bml.nonreg4 = 2;
        ret = ITE_testVideoBMLdatapath_alloutput(GRBFMT_YUV422_RASTER_INTERLEAVED, GRBFMT_YUV420_MB_SEMIPLANAR_DECIMATED);
        if(ret != EOK)
        {
            LOS_Log("ITE_dbg_setusecase gave error in %s and line  %d\n",__FUNCTION__,__LINE__);
            goto END;
        }
        nonregselect.bml.nonreg4 = 3;
    }


    /**
        Zoom Non regression
    */
    if (nonregselect.zoom.nonreg1)
    {
        nonregselect.zoom.nonreg1 = 2;
        ret = ITE_testZoomBMLStillForSensoroutputModes(
        GRBFMT_YUV422_RASTER_INTERLEAVED,
        GRBFMT_YUV420_MB_SEMIPLANAR_DECIMATED,0);// NO_STRIPE
        if(ret != EOK)
        {
            LOS_Log("ITE_dbg_setusecase gave error in %s and line  %d\n",__FUNCTION__,__LINE__);
            goto END;
        }

        nonregselect.zoom.nonreg1 = 3;
    }


    if (nonregselect.zoom.nonreg2)
    {
        nonregselect.zoom.nonreg2 = 2;
        ret = ITE_testZoomstreaming(GRBFMT_YUV422_RASTER_INTERLEAVED, GRBFMT_YUV420_MB_SEMIPLANAR_DECIMATED, "LR");
        if(ret != EOK)
        {
            LOS_Log("ITE_testZoomstreaming gave error in %s and line  %d\n",__FUNCTION__,__LINE__);
            goto END;
        }

        nonregselect.zoom.nonreg2 = 3;
    }


    if (nonregselect.zoom.nonreg3)
    {
        nonregselect.zoom.nonreg3 = 2;
        ret = ITE_bugZoomBMLStill(GRBFMT_YUV422_RASTER_INTERLEAVED, GRBFMT_YUV420_MB_SEMIPLANAR_DECIMATED);
        if(ret != EOK)
        {
            LOS_Log("ITE_bugZoomBMLStill gave error in %s and line  %d\n",__FUNCTION__,__LINE__);
            goto END;
        }

        nonregselect.zoom.nonreg3 = 3;
    }


    //[sudeep] not in non reg menu
    if (nonregselect.zoom.nonreg4)
    {
        nonregselect.zoom.nonreg4 = 2;

        //ITE_testZoomstreaming(GRBFMT_YUV422_RASTER_INTERLEAVED,GRBFMT_YUV422_MB_SEMIPLANAR,"HR");
        ITE_testZoomstreaming(GRBFMT_YUV422_RASTER_INTERLEAVED, GRBFMT_YUV420_MB_SEMIPLANAR_DECIMATED, "HR");

        //ITE_testZoomstreaming(GRBFMT_YUV422_RASTER_INTERLEAVED,GRBFMT_YUV422_RASTER_INTERLEAVED,"HR");
        nonregselect.zoom.nonreg4 = 3;
    }


    // Pan-Tilt non regression
    if (nonregselect.pan_tilt.nonreg1)
    {
        nonregselect.pan_tilt.nonreg1 = 2;

        ret =ITE_testPan_tiltStreaming(GRBFMT_YUV422_RASTER_INTERLEAVED, GRBFMT_YUV420_MB_SEMIPLANAR_DECIMATED, "LR");
        if(ret != EOK)
        {
            LOS_Log("ITE_dbg_setusecase gave error in %s and line  %d\n",__FUNCTION__,__LINE__);
            goto END;
        }


        nonregselect.pan_tilt.nonreg1 = 3;
    }


    if (nonregselect.pan_tilt.nonreg2)
    {
        nonregselect.pan_tilt.nonreg2 = 2;
        ITE_testPan_tiltStreaming(GRBFMT_YUV422_RASTER_INTERLEAVED, GRBFMT_YUV420_MB_SEMIPLANAR_DECIMATED, "HR");
        nonregselect.pan_tilt.nonreg2 = 3;
    }


    ITE_Stop_Env();
    END:
    return ret;
    // end test
}

/* -----------------------------------------------------------------------
   FUNCTION : ITE_testLRdatapath
   PURPOSE  : Test LR datapath for  YUV422 itld format
   for different resolution and framerates
   ------------------------------------------------------------------------ */
void
ITE_testLRdatapath(
enum e_grabFormat   grbformat)
{
    e_resolution        sizelr;
    char                testComment[200];
    char                resolutionName[16];
    char                sensorName[16];
    char                lrformatName[16];
    char                pathname[200];
    volatile t_uint32   maxDZ;

    if (usecase.sensor == 0)
    {
        sprintf(sensorName, "Cam0");
    }
    else
    {
        sprintf(sensorName, "Cam1");
    }

    /* LR Format always YUV422interleaved */
    usecase.LR_GrbFormat = grbformat;
    ITE_GiveStringFormat("LR", lrformatName);

    LOS_Log("\n --> Stream on LR pipe with GRBFMT_%s format\n", lrformatName);

    /* No DZ */
    maxDZ = ITE_readPE(Zoom_Control1_f_MaxDZ_Byte0);
    ITE_writePE(Zoom_Control1_f_MaxDZ_Byte0, 1);

    for (sizelr = usecase.LR_Min_resolution; sizelr <= usecase.LR_Max_resolution; sizelr++)
    {
        ITE_GiveStringNameFromEnum(sizelr, resolutionName);

        LOS_Log("\n -----> %s resolution\n", resolutionName);

        usecase.LR_XSize = ITE_GiveXSizeFromEnum(PIPE_LR, sizelr, grbformat);
        usecase.LR_YSize = ITE_GiveYSizeFromEnum(PIPE_LR, sizelr, grbformat);
        usecase.LR_resolution = ITE_GiveResolution(resolutionName);
        sprintf(testComment, "%s_test_nreg_LR_datapath_%s_%s",
                sensorName, lrformatName, resolutionName);

        /* Perform a "cd" to destination repository for test log */
        sprintf(pathname,
                IMAGING_PATH "/ite_nmf/test_results_nreg/datapath_LR_%s",
                lrformatName);
        strcat(g_out_path, pathname);

        LOS_Log("Current path: >%s< \n", ( char * ) g_out_path);
        init_test_results_nreg(g_out_path);
        mmte_testStart(testComment,
                       "test pipe LR (buffer + fr) YUV422 interleaved format",
                       g_out_path);

#ifdef TEST_PERFORMANCE
        mmte_testNext("test pipe LR\n");
        mmte_testComment("Test passed performace parameters printed\n");
        mmte_testResult(TEST_PASSED);
#endif

        ITE_testStreamLR_allFramerate(&usecase);
    }

    ITE_writePE(Zoom_Control1_f_MaxDZ_Byte0, maxDZ);
}

/* -----------------------------------------------------------------------
   FUNCTION : ITE_testStreamLR
   PURPOSE  : test Streaming on pipe LR using configuration provided through usecase :
   check output buffer and framerate
   ------------------------------------------------------------------------ */
Result_te
ITE_testStreamLR(
tps_sia_usecase p_sia_usecase,
float           framerate)
{
    char        mess[256];
    t_uint32    error = 0;
    Result_te   result = Result_e_Success;

    ITE_writePE(HostFrameConstraints_e_Flag_GuaranteeStaticFlickerFrameLength_Byte0, Flag_e_FALSE);

    ITE_SetFrameRateControl(framerate, framerate);

    if (g_InputImageSource == InputImageSource_e_Sensor0)
    {
        ITE_SensorMode(COLORBAR);
    }
    else
    {
        ITE_SetRxTestPattern(RxTestPattern_e_ColourBars);
    }


    usecase.sensormode = NORMAL_MODE;   // for LLD fw we need to reprogam each time the sensor
    ITE_LR_Prepare(p_sia_usecase, INFINITY);
    error = ITE_LR_Start(INFINITY);
    if (error != 0)
    {
        snprintf(mess, sizeof(mess), "Test :check static frame rate %f f/s", framerate);
        mmte_testNext(mess);
        snprintf(mess, sizeof(mess), "error returned by ISP: %d ", ( int ) error);  //__NO_WARNING__
        mmte_testComment(mess);
        mmte_testResult(TEST_FAILED);
        result = Result_e_Failure;
    }
    else
    {
        result &= ITE_unitaryTestStaticFramerate(framerate, GRBPID_PIPE_LR);
        ITE_LR_Stop();

        //Test Buffer was filled : Test values != 0xFF (default values when allocated)
        result &= ITE_testcolorBarLR();
    }


    ITE_LR_Free();

    //SUDEEP: Changed mmte_testend() to the following code for performance
    if ((result == Result_e_Failure) && (FALSE == g_test_perf_status))
    {
        LOS_Log("\nTest Failed\n");
        mmte_testComment("Test Failed,Test Performance Failed\n");
        result &= Result_e_Failure;
    }
    else if ((result == Result_e_Failure) && (PERFORMANCE_SKIPPED == g_test_perf_status))
    {
        LOS_Log("\nTest Failed Performance Skipped\n");
        mmte_testComment("Test Performance Skipped\n");
        result &= Result_e_Failure;
    }
    else if ((result == Result_e_Success) && (PERFORMANCE_SKIPPED == g_test_perf_status))
    {
        LOS_Log("\nTest Passed Performance Skipped\n");
        mmte_testComment("Test Performance Skipped\n");
        result &= Result_e_Success;
    }
    else if ((result == Result_e_Success) && (TRUE == g_test_perf_status))
    {
        LOS_Log("\nTest Passed,Test Passed in Performance\n");
        mmte_testComment("Test Passed,Test Passed in Performance\n");
        result &= Result_e_Success;
    }
    else
    {
        LOS_Log("\nTest Failed\n");
        mmte_testComment("Test Failed,Test Performance Passed\n");
        result &= Result_e_Failure;
    }


    return (result);
}


/* -----------------------------------------------------------------------
   FUNCTION : ITE_testStreamLR_allFramerate
   PURPOSE  : test Streaming on pipe LR using configuration provided through usecase :
   check output buffer (colorbar) for different framerates=5,10...30
   ------------------------------------------------------------------------ */
void
ITE_testStreamLR_allFramerate(
tps_sia_usecase p_sia_usecase)
{
    Result_te   result = Result_e_Success;

    result &= ITE_testStreamLR(p_sia_usecase, 1.0);
    result &= ITE_testStreamLR(p_sia_usecase, 2.0);
    result &= ITE_testStreamLR(p_sia_usecase, 5.0);
    result &= ITE_testStreamLR(p_sia_usecase, 10.0);
    result &= ITE_testStreamLR(p_sia_usecase, 15.0);
    result &= ITE_testStreamLR(p_sia_usecase, 20.0);
    if (usecase.sensor == 0)
    {
        result &= ITE_testStreamLR(p_sia_usecase, 25.0);
        result &= ITE_testStreamLR(p_sia_usecase, 30.0);
    }


    if (result == Result_e_Failure)
    {
        mmte_testResult(TEST_FAILED);
    }
    else
    {
        mmte_testResult(TEST_PASSED);
    }


    mmte_testEnd();

    //clean  g_out_path
    memset(g_out_path, 0, KlogDirectoryLentgh * sizeof(char));
}


/* -----------------------------------------------------------------------
   FUNCTION : ITE_testStreamHR_allFramerate
   PURPOSE  : test Streaming on pipe HR using configuration provided through usecase :
   check output buffer (colorbar) for different framerates=5,10...30
   ------------------------------------------------------------------------ */
void
ITE_testStreamHR_allFramerate(
tps_sia_usecase p_sia_usecase)
{
    Result_te   result = Result_e_Success;

    result &= ITE_testStreamHR(p_sia_usecase, 1.0);
    result &= ITE_testStreamHR(p_sia_usecase, 2.0);
    result &= ITE_testStreamHR(p_sia_usecase, 5.0);
    result &= ITE_testStreamHR(p_sia_usecase, 7.5);
    result &= ITE_testStreamHR(p_sia_usecase, 10.0);
    result &= ITE_testStreamHR(p_sia_usecase, 15.0);
    result &= ITE_testStreamHR(p_sia_usecase, 20.0);
    if (usecase.sensor == 0)
    {
        result &= ITE_testStreamHR(p_sia_usecase, 25.0);
        result &= ITE_testStreamHR(p_sia_usecase, 30.0);
    }


    if (result == Result_e_Failure)
    {
        mmte_testResult(TEST_FAILED);
    }
    else
    {
        mmte_testResult(TEST_PASSED);
    }


    mmte_testEnd();

    //clean  g_out_path
    memset(g_out_path, 0, KlogDirectoryLentgh * sizeof(char));
}


/* --------------------------------------------------------------------------------------------
   FUNCTION : ITE_testStreamLRHR_allFramerate
   PURPOSE  : test Streaming on pipe LR and HR using configuration provided through usecase :
   check output buffer (colorbar) for different framerates=5,10...30
   -------------------------------------------------------------------------------------------- */
void
ITE_testStreamLRHR_allFramerate(
tps_sia_usecase p_sia_usecase)
{
    Result_te   result = Result_e_Success;

    result &= ITE_testStreamLRHR(p_sia_usecase, 1.0);
    result &= ITE_testStreamLRHR(p_sia_usecase, 2.0);
    result &= ITE_testStreamLRHR(p_sia_usecase, 5.0);
    result &= ITE_testStreamLRHR(p_sia_usecase, 7.5);
    result &= ITE_testStreamLRHR(p_sia_usecase, 10.0);
    result &= ITE_testStreamLRHR(p_sia_usecase, 15.0);
    result &= ITE_testStreamLRHR(p_sia_usecase, 20.0);
    if (usecase.sensor == 0)
    {
        result &= ITE_testStreamLRHR(p_sia_usecase, 25.0);
        result &= ITE_testStreamLRHR(p_sia_usecase, 30.0);
    }


    if (result == Result_e_Failure)
    {
        mmte_testResult(TEST_FAILED);
    }
    else
    {
        mmte_testResult(TEST_PASSED);
    }


    mmte_testEnd();

    //clean  g_out_path
    memset(g_out_path, 0, KlogDirectoryLentgh * sizeof(char));
}

/* -----------------------------------------------------------------------
   FUNCTION : ITE_testHRdatapath
   PURPOSE  : Test HR datapath with YUV420MB ,  YUV420P format
   for different resolution and framerates
   ------------------------------------------------------------------------ */
void
ITE_testHRdatapath(
enum e_grabFormat   grbformat)
{
    e_resolution        sizehr;
    char                testComment[200];
    char                resolutionName[16];
    char                sensorName[16];
    char                hrformatName[16];
    char                pathname[200];
    volatile t_uint32   maxDZ;

    if (usecase.sensor == 0)
    {
        sprintf(sensorName, "Cam0");
    }
    else
    {
        sprintf(sensorName, "Cam1");
    }

    /* HR Format could be YUV420MB and YUV420P */
    usecase.HR_GrbFormat = grbformat;
    ITE_GiveStringFormat("HR", hrformatName);
    LOS_Log("\n --> Stream on HR pipe with %s format\n", hrformatName);

    /* No DZ */
    maxDZ = ITE_readPE(Zoom_Control1_f_MaxDZ_Byte0);
    ITE_writePE(Zoom_Control1_f_MaxDZ_Byte0, 1);

    for (sizehr = usecase.HR_Min_resolution; sizehr <= usecase.HR_Video_Max_resolution; sizehr++)
    {
        ITE_GiveStringNameFromEnum(sizehr, resolutionName);
        LOS_Log("\n -----> %s resolution\n", resolutionName);
        usecase.HR_XSize = ITE_GiveXSizeFromEnum(PIPE_HR, sizehr, grbformat);
        usecase.HR_YSize = ITE_GiveYSizeFromEnum(PIPE_HR, sizehr, grbformat);
        usecase.HR_resolution = ITE_GiveResolution(resolutionName);

        if (usecase.HR_XSize >= (t_uint32)ITE_GiveXSizeFromEnum(PIPE_HR, HD, grbformat))
        {
            ITE_dbg_usagemode("videohq");
        }
        else
        {
            ITE_dbg_usagemode("video");
        }

        sprintf(testComment, "%s_test_nreg_HR_datapath_%s_%s",
                sensorName, hrformatName, resolutionName);

        /* Perform a "cd" to destination repository for test log */
        sprintf(pathname,
                IMAGING_PATH "/ite_nmf/test_results_nreg/datapath_HR_%s",
                hrformatName);
        strcat(g_out_path, pathname);
        LOS_Log("Current path: >%s< \n", ( char * ) g_out_path);
        init_test_results_nreg(g_out_path);
        mmte_testStart(testComment,
                       "test pipe HR (buffer + fr) YUV420 Macrobloc format",
                       g_out_path);

#ifdef TEST_PERFORMANCE
        mmte_testNext("test pipe HR\n");
        mmte_testComment("Test passed performace parameters printed\n");
        mmte_testResult(TEST_PASSED);
#endif

        ITE_testStreamHR_allFramerate(&usecase);
    }

    ITE_writePE(Zoom_Control1_f_MaxDZ_Byte0, maxDZ);
}

/* -----------------------------------------------------------------------
   FUNCTION : ITE_testLRHRdatapath
   PURPOSE  : Test LR and HR datapath
   for different resolution and framerates
   ------------------------------------------------------------------------ */
void
ITE_testLRHRdatapath(
enum e_grabFormat   lrgrbformat,
enum e_grabFormat   hrgrbformat,
t_uint8             sameAR)
{
    e_resolution        sizelr,
                        sizehr;
    char                testComment[200];
    char                lrresolutionName[16];
    char                hrresolutionName[16];
    char                sensorName[16];
    char                lrformatName[16];
    char                hrformatName[16];
    char                pathname[200];
    volatile t_uint32   maxDZ;

    if (usecase.sensor == 0)
    {
        sprintf(sensorName, "Cam0");
    }
    else
    {
        sprintf(sensorName, "Cam1");
    }

    usecase.LR_GrbFormat = lrgrbformat;
    ITE_GiveStringFormat("LR", lrformatName);
    usecase.HR_GrbFormat = hrgrbformat;
    ITE_GiveStringFormat("HR", hrformatName);

    LOS_Log("\n --> Stream on LR pipe with GRBFMT_%s and HR  pipe with GRBFMT_%s format\n",
            lrformatName,
            hrformatName);

    /* No DZ */
    maxDZ = ITE_readPE(Zoom_Control1_f_MaxDZ_Byte0);
    ITE_writePE(Zoom_Control1_f_MaxDZ_Byte0, 1);

    for (sizelr = usecase.LR_Min_resolution; sizelr <= usecase.LR_Max_resolution; sizelr++)
    {
        for (sizehr = usecase.HR_Min_resolution; sizehr <= usecase.HR_Video_Max_resolution; sizehr++)
        {
            if(((sameAR == 1) &&
                (ITE_GiveAspectRatioFromEnum(PIPE_LR, sizelr, lrgrbformat) ==
                 ITE_GiveAspectRatioFromEnum(PIPE_HR, sizehr, hrgrbformat))) ||
               ((sameAR == 0) &&
                (ITE_GiveAspectRatioFromEnum(PIPE_LR, sizelr, lrgrbformat) !=
                 ITE_GiveAspectRatioFromEnum(PIPE_HR, sizehr, hrgrbformat))))
            {
                ITE_GiveStringNameFromEnum(sizelr, lrresolutionName);
                LOS_Log("\n -----> %s LR resolution\n", lrresolutionName);
                ITE_GiveStringNameFromEnum(sizehr, hrresolutionName);
                LOS_Log("\n -----> %s HR resolution\n", hrresolutionName);

                usecase.LR_XSize = ITE_GiveXSizeFromEnum(PIPE_LR, sizelr, lrgrbformat);
                usecase.LR_YSize = ITE_GiveYSizeFromEnum(PIPE_LR, sizelr, lrgrbformat);
                usecase.HR_XSize = ITE_GiveXSizeFromEnum(PIPE_HR, sizehr, hrgrbformat);
                usecase.HR_YSize = ITE_GiveYSizeFromEnum(PIPE_HR, sizehr, hrgrbformat);
                usecase.LR_resolution = ITE_GiveResolution(lrresolutionName);
                usecase.HR_resolution = ITE_GiveResolution(hrresolutionName);

                if (usecase.HR_XSize >= (t_uint32)ITE_GiveXSizeFromEnum(PIPE_HR, HD, hrgrbformat))
                {
                    ITE_dbg_usagemode("videohq");
                }
                else
                {
                    ITE_dbg_usagemode("video");
                }

                sprintf(testComment,
                        "%s_test_nreg_LR_datapath_%s_%s_HR_datapath_%s_%s",
                        sensorName,
                        lrformatName,
                        lrresolutionName,
                        hrformatName,
                        hrresolutionName);

                /* Perform a "cd" to destination repository for test log */
                sprintf(pathname,
                        IMAGING_PATH "/ite_nmf/test_results_nreg/datapath_LR_%s_HR_%s",
                        lrformatName,
                        hrformatName);

                strcat(g_out_path, pathname);
                LOS_Log("Current path: >%s< \n", ( char * ) g_out_path);
                init_test_results_nreg(g_out_path);
                mmte_testStart(testComment,
                               "test pipe LR and HR (buffer + fr) ",
                               g_out_path);

#ifdef TEST_PERFORMANCE
                mmte_testNext("test pipe LRHR\n");
                mmte_testComment("Test passed performace parameters printed\n");
                mmte_testResult(TEST_PASSED);
#endif

                ITE_testStreamLRHR_allFramerate(&usecase);
            }
        }
    }

    ITE_writePE(Zoom_Control1_f_MaxDZ_Byte0, maxDZ);
}

/* -------------------------------------------------------------------------------------------
   FUNCTION : ITE_testStreamLRHR
   PURPOSE  : test Streaming on pipe LR and HR using configuration provided through usecase :
   check output buffer and framerate
   ------------------------------------------------------------------------------------------- */
Result_te
ITE_testStreamLRHR(
tps_sia_usecase p_sia_usecase,
float           framerate)
{
    char        mess[256];
    t_uint32    error = 0;
    Result_te   result = Result_e_Success;

    ITE_writePE(HostFrameConstraints_e_Flag_GuaranteeStaticFlickerFrameLength_Byte0, Flag_e_FALSE);

    ITE_SetFrameRateControl(framerate, framerate);

    if (g_InputImageSource == InputImageSource_e_Sensor0)
    {
        ITE_SensorMode(COLORBAR);
    }
    else
    {
        ITE_SetRxTestPattern(RxTestPattern_e_ColourBars);
    }


    usecase.sensormode = NORMAL_MODE;   // for LLD fw we need to reprogam each time the sensor
    ITE_LRHR_Prepare(p_sia_usecase, INFINITY);
    error = ITE_LRHR_Start(INFINITY);

    if (error != 0)
    {
        snprintf(mess, sizeof(mess), "Test :check static frame rate %f f/s", framerate);
        mmte_testNext(mess);
        snprintf(mess, sizeof(mess), "error returned by ISP: %d ", ( int ) error);  //__NO_WARNING__
        mmte_testComment(mess);
        mmte_testResult(TEST_FAILED);
    }
    else
    {
        result &= ITE_unitaryTestStaticFramerate(framerate, GRBPID_PIPE_HR);
        result &= ITE_unitaryTestStaticFramerate(framerate, GRBPID_PIPE_LR);
        ITE_LRHR_Stop();

        //check if color bar in buffer
        result &= ITE_testcolorBarLR();
        result &= ITE_testcolorBarHR();
    }


    ITE_LRHR_Free();

    //SUDEEP: Changed mmte_testend() to the following code for performance
    if ((result == Result_e_Failure) && (FALSE == g_test_perf_status))
    {
        LOS_Log("\nTest Failed\n");
        mmte_testComment("Test Failed,Test Performance Failed\n");
        result &= Result_e_Failure;
    }
    else if ((result == Result_e_Failure) && (PERFORMANCE_SKIPPED == g_test_perf_status))
    {
        LOS_Log("\nTest Failed, Performance Skipped\n");
        mmte_testComment("Test Failed,Test Performance Skipped\n");
        result &= Result_e_Failure;
    }
    else if ((result == Result_e_Success) && (PERFORMANCE_SKIPPED == g_test_perf_status))
    {
        LOS_Log("\nTest Passed, Performance Skipped\n");
        mmte_testComment("Test Passed,Test Performance Skipped\n");
        result &= Result_e_Success;
    }
    else if ((result == Result_e_Success) && (TRUE == g_test_perf_status))
    {
        LOS_Log("\nTest Passed,Test Passed in Performance\n");
        mmte_testComment("Test Passed,Test Passed in Performance\n");
        result &= Result_e_Success;
    }
    else
    {
        LOS_Log("\nTest Failed\n");
        mmte_testComment("Test Failed,Test Performance Passed\n");
        result &= Result_e_Failure;
    }


    return (result);
}


/* -----------------------------------------------------------------------
   FUNCTION : ITE_testBufferHR
   PURPOSE  : Test if GrabBufferHR[0] buffer was filled;
   use samples in buffer and check different from default values
   use mmte_testResult() API
   ------------------------------------------------------------------------ */
void
ITE_testBufferHR(void)
{
    char    mess[256];

    snprintf(mess, sizeof(mess), "Test %d :check if GrabBufferHR[0] was filled", ( int ) testNum);  //__NO_WARNING__
    mmte_testNext(mess);

    if (ITE_CheckGrabInBuffer(GrabBufferHR[0].logAddress, GrabBufferHR[0].buffersizeByte, 0xff) == 1)
    {
        MMTE_TEST_COMMENT("TEST OK: GrabBufferHR[0] buffer was filled with values different from default value\n");
        PERFORMANCE_RESULT_PASSED();
    }
    else
    {
        MMTE_TEST_COMMENT(
        "TEST FAIL: GrabBufferHR[0] buffer not properly filled (too many default values in buffer)\n");
        PERFORMANCE_RESULT_FAILED();
    }


    testNum++;
}


/* -----------------------------------------------------------------------
   FUNCTION : ITE_testStreamHR
   PURPOSE  : test Streaming on pipe HR using configuration provided through usecase :
   check output buffer and framerate
   ------------------------------------------------------------------------ */
Result_te
ITE_testStreamHR(
tps_sia_usecase p_sia_usecase,
float           framerate)
{
    char        mess[256];
    t_uint32    error = 0;
    Result_te   result = Result_e_Success;

    ITE_SetFrameRateControl(framerate, framerate);

    if (g_InputImageSource == InputImageSource_e_Sensor0)
    {
        ITE_SensorMode(COLORBAR);
    }
    else
    {
        ITE_SetRxTestPattern(RxTestPattern_e_ColourBars);
    }


    usecase.sensormode = NORMAL_MODE;   // for LLD fw we need to reprogam each time the sensor
    ITE_HR_Prepare(p_sia_usecase, INFINITY);
    error = ITE_HR_Start(INFINITY);

    if (error != 0)
    {
        snprintf(mess, sizeof(mess), "Test :check static frame rate %f f/s", framerate);
        mmte_testNext(mess);
        snprintf(mess, sizeof(mess), "error returned by ISP: %d ", ( int ) error);  //__NO_WARNING__
        mmte_testComment(mess);
        mmte_testResult(TEST_FAILED);
    }
    else
    {
        result &= ITE_unitaryTestStaticFramerate(framerate, GRBPID_PIPE_HR);
        ITE_HR_Stop();

        //check if color bar in buffer
        result &= ITE_testcolorBarHR();
    }


    ITE_HR_Free();

    //SUDEEP: Changed mmte_testend() to the following code for performance
    if ((result == Result_e_Failure) && (FALSE == g_test_perf_status))
    {
        LOS_Log("\nTest Failed\n");
        mmte_testComment("Test Failed,Test Performance Failed\n");
        result &= Result_e_Failure;
    }
    else if ((result == Result_e_Failure) && (PERFORMANCE_SKIPPED == g_test_perf_status))
    {
        LOS_Log("\nTest Failed Performance Skipped\n");
        mmte_testComment("Test Performance Skipped\n");
        result &= Result_e_Failure;
    }
    else if ((result == Result_e_Success) && (PERFORMANCE_SKIPPED == g_test_perf_status))
    {
        LOS_Log("\nTest Passed Performance Skipped\n");
        mmte_testComment("Test Performance Skipped\n");
        result &= Result_e_Success;
    }
    else if ((result == Result_e_Success) && (TRUE == g_test_perf_status))
    {
        LOS_Log("\nTest Passed,Test Passed in Performance\n");
        mmte_testComment("Test Passed,Test Passed in Performance\n");
        result &= Result_e_Success;
    }
    else
    {
        LOS_Log("\nTest Failed\n");
        mmte_testComment("Test Failed,Test Performance Passed\n");
        result &= Result_e_Failure;
    }


    return (result);
}


/* -----------------------------------------------------------------------
   FUNCTION : ITE_testBMSdatapath
   PURPOSE  : Test BMS datapath with raw8 ,  raw12 format
   for different resolutions
   ------------------------------------------------------------------------ */
void
ITE_testBMSdatapath(void)
{
    ITE_testStillBMSdatapath();
}

int
ITE_testLRHRBMSdatapath(
enum e_grabFormat   lrgrbformat,
enum e_grabFormat   hrgrbformat,
t_uint8             sameAR,
t_uint8             glaceonly,
t_uint8             Zoom,
t_uint8             statinzoom)
{
    e_resolution            sizelr,
                            sizehr;
    char                    testComment[200];
    char                    lrresolutionName[16];
    char                    hrresolutionName[16];
    char                    sensorName[16];
    char                    lrformatName[16];
    char                    hrformatName[16];
    char                    pathname[200];
    char                    BMSresolution[16];
    int ret = EOK;
    float                   DZ;
    t_uint32                previousgrbcountLR;
    t_uint32                previousgrbcountHR;

    if (usecase.sensor == 0)
    {
        sprintf(sensorName, "Cam0");
    }
    else
    {
        sprintf(sensorName, "Cam1");
    }

    usecase.LR_GrbFormat = lrgrbformat;
    ITE_GiveStringFormat("LR", lrformatName);
    usecase.HR_GrbFormat = hrgrbformat;
    ITE_GiveStringFormat("HR", hrformatName);

    LOS_Log("\n --> Stream on LR pipe with GRBFMT_%s and set HR  pipe with GRBFMT_%s format\n",
            lrformatName,
            hrformatName);

    ITE_dbg_bmsmode("STILL");
    ret = ITE_dbg_setusecase("BMS", BMSresolution, "RAW8");
    if(ret != EOK)
    {
          LOS_Log("ITE_dbg_setusecase gave error in %s and line  %d\n",__FUNCTION__,__LINE__);
          goto END;
    }

    ITE_dbg_bmsout("BAYERSTORE2");

    for (sizelr = usecase.LR_Min_resolution; sizelr <= usecase.LR_Max_resolution; sizelr++)
    {
        for (sizehr = usecase.HR_Min_resolution; sizehr <= usecase.HR_Video_Max_resolution; sizehr++)
        {
            if
            (((sameAR == 1) &&
              (ITE_GiveAspectRatioFromEnum(PIPE_LR, sizelr, lrgrbformat) ==
               ITE_GiveAspectRatioFromEnum(PIPE_HR, sizehr, hrgrbformat))) ||
             ((sameAR == 0) &&
              (ITE_GiveAspectRatioFromEnum(PIPE_LR, sizelr, lrgrbformat) !=
               ITE_GiveAspectRatioFromEnum(PIPE_HR, sizehr, hrgrbformat))))
            {
                ITE_GiveStringNameFromEnum(sizelr, lrresolutionName);
                LOS_Log("\n -----> %s LR resolution\n", lrresolutionName);
                ITE_GiveStringNameFromEnum(sizehr, hrresolutionName);
                LOS_Log("\n -----> %s HR resolution\n", hrresolutionName);

                usecase.LR_XSize = ITE_GiveXSizeFromEnum(PIPE_LR, sizelr, lrgrbformat);
                usecase.LR_YSize = ITE_GiveYSizeFromEnum(PIPE_LR, sizelr, lrgrbformat);
                usecase.HR_XSize = ITE_GiveXSizeFromEnum(PIPE_HR, sizehr, hrgrbformat);
                usecase.HR_YSize = ITE_GiveYSizeFromEnum(PIPE_HR, sizehr, hrgrbformat);
                usecase.LR_resolution = ITE_GiveResolution(lrresolutionName);
                usecase.HR_resolution = ITE_GiveResolution(hrresolutionName);

                sprintf(
                testComment,
                "%s_test_nreg_LR_datapath_%s_%s_HR_datapath_%s_%s",
                sensorName,
                lrformatName,
                lrresolutionName,
                hrformatName,
                hrresolutionName);

                /* Perform a "cd" to destination repository for test log */
                sprintf(pathname,
                        IMAGING_PATH "/ite_nmf/test_results_nreg/datapath_LR_%s_HR_%s_BMS_FFOV",
                        lrformatName,
                        hrformatName);

                strcat(g_out_path, pathname);
                LOS_Log("Current path: >%s< \n", ( char * ) g_out_path);
                init_test_results_nreg(g_out_path);
                mmte_testStart(testComment,
                               "test pipe LR and HR (buffer + fr) ",
                               g_out_path);

                /* Stream first on LR */
                ITE_LR_Prepare(&usecase, INFINITY);
                ITE_HR_Prepare(&usecase, INFINITY);

                ITE_writePE(Zoom_Control_f_SetFOVX_Byte0, 0);
                ITE_SetFrameRateControl(30, 30);

                ITE_LR_Start(INFINITY);
                LOS_Log("\n500 millisecs stream on LR pipe\n");
                LOS_Sleep(500);
                ITE_LR_Stop();
                ITE_StoreCurrentFOVX();

                ITE_NMF_SetGlaceBlockFraction(100, 100, 0, 0);
                ITE_NMF_SetHistoBlockFraction(100, 100, 0, 0);
                ITE_NMF_SetGlaceGrid(72, 54);
                ITE_NMF_SetGlaceSatLevel(255, 255, 255);

                // ITE_NMF_createStatsBuffer(&StatsBuffer);
                pStore = g_pStatsBufferStorage;
                ITE_NMF_UpdateGlaceParam();
                ITE_NMF_UpdateHistoParam();
                if (glaceonly == 1)
                {
                    ITE_writePE(HistStats_Ctrl_ptru32_HistRAddr_Byte0, (t_uint32) (0));
                    ITE_writePE(HistStats_Ctrl_ptru32_HistGAddr_Byte0, (t_uint32) (0));
                    ITE_writePE(HistStats_Ctrl_ptru32_HistBAddr_Byte0, (t_uint32) (0));
                }

                ITE_NMF_GlaceAndHistoExpReqNoWait();
                g_grabLR_count = 0;
                g_statsGlaceHisto_count = 0;
                ITE_LR_Start(INFINITY);
                ITE_NMF_WaitStatReady(ISP_GLACE_STATS_READY);
                ITE_NMF_GlaceHistoExpStatsRequestAndWait();
                if (Zoom == 1)
                {
                    for (DZ = 1.0; DZ < 7.0; DZ++)
                    {
                        previousgrbcountLR = g_grabLR_count;
                        ITE_NMF_DZ_Set(DZ);
                        if (statinzoom == 1)
                        {
                            ITE_NMF_GlaceHistoExpStatsRequestAndWait();
                            LOS_Log("\tNumber of Grabbed frame on LR = %d\n",
                                    g_grabLR_count);
                            if (previousgrbcountLR == g_grabLR_count)
                            {
                                LOS_Log("\tGrab LR stopted!\n");
                            }
                        }
                    }

                    for (DZ = 5.0; DZ >= 1.0; DZ--)
                    {
                        previousgrbcountLR = g_grabLR_count;
                        ITE_NMF_DZ_Set(DZ);
                        if (statinzoom == 1)
                        {
                            ITE_NMF_GlaceHistoExpStatsRequestAndWait();
                            LOS_Log("\t\t\t\tNumber of Grabbed frame on LR = %d\n",
                                    g_grabLR_count);
                            if (previousgrbcountLR == g_grabLR_count)
                            {
                                LOS_Log("\t\t\t\t Grab LR stopted!\n");
                            }
                        }
                    }
                }

                LOS_Sleep(500);
                ITE_LR_Stop();

                LOS_Log("\t\t\t\tNumber of Grabbed frame on LR = %d\n",
                        g_grabLR_count);
                LOS_Log("\t\t\t\tNumber of Collected Stat GlaceHisto = %d\n",
                        g_statsGlaceHisto_count);

                ITE_NMF_GlaceAndHistoExpReqNoWait();
                ITE_LR_Start(INFINITY);
                ITE_NMF_WaitStatReady(ISP_GLACE_STATS_READY);
                ITE_LR_Stop();

                LOS_Log("\t\t\t\tNumber of Grabbed frame on LR = %d\n",
                        g_grabLR_count);
                LOS_Log("\t\t\t\tNumber of Collected Stat GlaceHisto = %d\n",
                        g_statsGlaceHisto_count);

#if 0   /* TODO: <AG>: Review: to be corrected: hem  */
                ITE_BMS_Prepare(&usecase, INFINITY);
                ITE_BMS_Start(1);
                LOS_Log("\nStream one frame on BMS \n");
                ITE_BMS_Stop();
#endif
                g_grabLR_count = 0;
                g_grabHR_count = 0;
                g_statsGlaceHisto_count = 0;

                // LR HR streaming
                ITE_NMF_GlaceAndHistoExpReqNoWait();
                ITE_LRHR_Start(INFINITY);
                ITE_NMF_WaitStatReady(ISP_GLACE_STATS_READY);
                ITE_NMF_GlaceHistoExpStatsRequestAndWait();

                if (Zoom == 1)
                {
                    for (DZ = 1.0; DZ < 7.0; DZ++)
                    {
                        previousgrbcountLR = g_grabLR_count;
                        previousgrbcountHR = g_grabHR_count;
                        ITE_NMF_DZ_Set(DZ);
                        if (statinzoom == 1)
                        {
                            ITE_NMF_GlaceHistoExpStatsRequestAndWait();
                            LOS_Log("\t\t\t\tNumber of Grabbed frame on LR = %d\n",
                                    g_grabLR_count);
                            LOS_Log("\t\t\t\tNumber of Grabbed frame on HR = %d\n",
                                    g_grabHR_count);
                            if (previousgrbcountLR == g_grabLR_count)
                            {
                                LOS_Log("\t\t\t\t Grab LR stopted!\n");
                            }


                            if (previousgrbcountHR == g_grabHR_count)
                            {
                                LOS_Log("\t\t\t\t Grab HR stopted!\n");
                            }
                        }
                    }

                    for (DZ = 5.0; DZ >= 1.0; DZ--)
                    {
                        previousgrbcountLR = g_grabLR_count;
                        previousgrbcountHR = g_grabHR_count;
                        ITE_NMF_DZ_Set(DZ);
                        if (statinzoom == 1)
                        {
                            ITE_NMF_GlaceHistoExpStatsRequestAndWait();
                            LOS_Log("\t\t\t\tNumber of Grabbed frame on LR = %d\n",
                                    g_grabLR_count);
                            LOS_Log("\t\t\t\tNumber of Grabbed frame on HR = %d\n",
                                    g_grabHR_count);
                            if (previousgrbcountLR == g_grabLR_count)
                            {
                                LOS_Log("\t\t\t\t Grab LR stopted!\n");
                            }

                            if (previousgrbcountHR == g_grabHR_count)
                            {
                                LOS_Log("\t\t\t\t Grab HR stopted!\n");
                            }
                        }
                    }
                }

                LOS_Sleep(500);

                ITE_LRHR_Stop();

                LOS_Log("\t\t\t\tNumber of Grabbed frame on LR = %d\n",
                        g_grabLR_count);
                LOS_Log("\t\t\t\tNumber of Grabbed frame on HR = %d\n",
                        g_grabHR_count);
                LOS_Log("\t\t\t\tNumber of Collected Stat GlaceHisto = %d\n",
                        g_statsGlaceHisto_count);

                // HR streaming only
                ITE_LR_Free();

                g_grabLR_count = 0;
                g_grabHR_count = 0;
                g_statsGlaceHisto_count = 0;

                ITE_NMF_GlaceAndHistoExpReqNoWait();
                ITE_HR_Start(INFINITY);
                ITE_NMF_WaitStatReady(ISP_GLACE_STATS_READY);
                ITE_NMF_GlaceHistoExpStatsRequestAndWait();
                if (Zoom == 1)
                {
                    for (DZ = 1.0; DZ < 7.0; DZ++)
                    {
                        previousgrbcountHR = g_grabHR_count;
                        ITE_NMF_DZ_Set(DZ);
                        if (statinzoom == 1)
                        {
                            ITE_NMF_GlaceHistoExpStatsRequestAndWait();
                            LOS_Log("\t\t\t\tNumber of Grabbed frame on HR = %d\n",
                                    g_grabHR_count);
                            if (previousgrbcountHR == g_grabHR_count)
                            {
                                LOS_Log("\t\t\t\t Grab HR stopted!\n");
                            }
                        }
                    }

                    for (DZ = 5.0; DZ >= 1.0; DZ--)
                    {
                        previousgrbcountHR = g_grabHR_count;
                        ITE_NMF_DZ_Set(DZ);
                        if (statinzoom == 1)
                        {
                            ITE_NMF_GlaceHistoExpStatsRequestAndWait();
                            LOS_Log("\t\t\t\tNumber of Grabbed frame on HR = %d\n",
                                    g_grabHR_count);
                            if (previousgrbcountHR == g_grabHR_count)
                            {
                                LOS_Log("\t\t\t\t Grab HR stopted!\n");
                            }
                        }
                    }
                }

                LOS_Sleep(500);
                ITE_HR_Stop();

                LOS_Log("\t\t\t\tNumber of Grabbed frame on HR = %d\n",
                        g_grabHR_count);
                LOS_Log("\t\t\t\tNumber of Collected Stat GlaceHisto = %d\n",
                        g_statsGlaceHisto_count);

                ITE_BMS_Free();
                ITE_HR_Free();
                mmte_testEnd();

                /* Clear g_out_path */
                memset(g_out_path, 0, KlogDirectoryLentgh * sizeof(char));
            }
        }
    }

    // ITE_writePE(Zoom_Control1_f_MaxDZ_Byte0,maxDZ);

END:
    return ret;
}

/* -----------------------------------------------------------------------
   FUNCTION : ITE_testBMLdatapath
   PURPOSE  : Test BML datapath with raw8 ,  raw12 format
   for different output LR HR resolutions
   ------------------------------------------------------------------------ */
int
ITE_testBMLdatapath(
enum e_grabFormat   lrgrbformat,
enum e_grabFormat   hrgrbformat)
{
    e_resolution    lrresolution;
    e_resolution    hrresolution;
    char            lrresolutionstring[16];
    char            hrresolutionstring[16];
    char            lrformatstring[16];
    char            hrformatstring[16];
    char            BMSresolution[16];
    int ret = EOK;
    ITE_dbg_bmsmode("STILL");
    ret = ITE_dbg_setusecase("BMS", BMSresolution, "RAW8");
    if(ret != EOK)
    {
        LOS_Log("ITE_dbg_setusecase gave error in %s and line  %d\n",
                __FUNCTION__,__LINE__);
        goto END;
    }

    ITE_dbg_bmsout("BAYERSTORE2");

    // Sensor in test mode
    if (g_InputImageSource == InputImageSource_e_Sensor0)
    {
        ITE_SensorMode(COLORBAR);
    }
    else
    {
        ITE_writePE(SystemSetup_e_InputImageSource_Byte0, InputImageSource_e_Rx);
        ITE_SetRxTestPattern(RxTestPattern_e_ColourBars);
    }

    ITE_dbg_usagemode("vf");

    // Stream first on LR
    ITE_LR_Prepare(&usecase, INFINITY);
    ITE_LR_Start(INFINITY);
    LOS_Log("\n500msecs stream on LR pipe\n");
    LOS_Sleep(500);

    ITE_LR_Stop();

    ITE_dbg_usagemode("still");
    ITE_BMS_Prepare(&usecase, INFINITY);
    ITE_BMS_Start(1);
    LOS_Log("\nStream one frame on BMS \n");
    ITE_BMS_Stop();

    for (lrresolution = usecase.LR_Min_resolution; lrresolution <= usecase.LR_Max_resolution; lrresolution++)
    {
        for (hrresolution = usecase.HR_Min_resolution; hrresolution <= usecase.HR_Still_Max_resolution; hrresolution++)
        {
            if (ITE_GiveAspectRatioFromEnum(PIPE_LR, lrresolution, lrgrbformat) ==
                ITE_GiveAspectRatioFromEnum(PIPE_HR, hrresolution, hrgrbformat))
            {
                ITE_GiveStringNameFromEnum(lrresolution, lrresolutionstring);
                ITE_GiveStringNameFromEnum(hrresolution, hrresolutionstring);
                ITE_GiveFormatStringFromEnum(lrgrbformat, lrformatstring);
                ITE_GiveFormatStringFromEnum(hrgrbformat, hrformatstring);

                ret = ITE_dbg_setusecase("LR", lrresolutionstring, lrformatstring);
                if(ret != EOK)
                {
                    LOS_Log("ITE_dbg_setusecase gave error in %s and line  %d\n",
                            __FUNCTION__,__LINE__);
                    goto END;
                }
                ret = ITE_dbg_setusecase("HR", hrresolutionstring, hrformatstring);
                if(ret != EOK)
                {
                    LOS_Log("ITE_dbg_setusecase gave error in %s and line  %d\n",
                            __FUNCTION__,__LINE__);
                    goto END;
                }
                ITE_testBMLdatapathLRHR("RAW8");
            }
        }
    }

    ITE_BMS_Free();
    ITE_BML_Free();
    ITE_LR_Free();

    ITE_dbg_bmsmode("STILL");
    ret =ITE_dbg_setusecase("BMS", BMSresolution, "RAW12");
    if(ret != EOK)
    {
        LOS_Log("ITE_dbg_setusecase gave error in %s and line  %d\n",
                __FUNCTION__,__LINE__);
        goto END;
    }
    ITE_dbg_bmsout("BAYERSTORE2");

    // Sensor in test mode
    if (g_InputImageSource == InputImageSource_e_Sensor0)
    {
        ITE_SensorMode(COLORBAR);
    }
    else
    {
        ITE_SetRxTestPattern(RxTestPattern_e_ColourBars);
    }

    // Stream first on LR
    ITE_dbg_usagemode("vf");
    ITE_LR_Prepare(&usecase, INFINITY);
    ITE_LR_Start(INFINITY);
    LOS_Log("\n500msecs stream on LR pipe\n");
    LOS_Sleep(500);
    ITE_LR_Stop();

    ITE_dbg_usagemode("still");
    ITE_BMS_Prepare(&usecase, INFINITY);
    ITE_BMS_Start(1);
    LOS_Log("\nStream one frame on BMS \n");
    ITE_BMS_Stop();

    for (lrresolution = usecase.LR_Min_resolution; lrresolution <= usecase.LR_Max_resolution; lrresolution++)
    {
        for (hrresolution = usecase.HR_Min_resolution; hrresolution <= usecase.HR_Still_Max_resolution; hrresolution++)
        {
            if (ITE_GiveAspectRatioFromEnum(PIPE_LR, lrresolution, lrgrbformat) ==
                ITE_GiveAspectRatioFromEnum(PIPE_HR, hrresolution, hrgrbformat))
            {
                ITE_GiveStringNameFromEnum(lrresolution, lrresolutionstring);
                ITE_GiveStringNameFromEnum(hrresolution, hrresolutionstring);
                ITE_GiveFormatStringFromEnum(lrgrbformat, lrformatstring);
                ITE_GiveFormatStringFromEnum(hrgrbformat, hrformatstring);

                ret = ITE_dbg_setusecase("LR", lrresolutionstring, lrformatstring);
                if(ret != EOK)
                {
                    LOS_Log("ITE_dbg_setusecase gave error in %s and line  %d\n",
                            __FUNCTION__,__LINE__);
                    goto END;
                }
                ret = ITE_dbg_setusecase("HR", hrresolutionstring, hrformatstring);
                if(ret != EOK)
                {
                    LOS_Log("ITE_dbg_setusecase gave error in %s and line  %d\n",
                            __FUNCTION__,__LINE__);
                    goto END;
                }
                ITE_testBMLdatapathLRHR("RAW12");
            }
        }
    }

    ITE_BMS_Free();
    ITE_BML_Free();
    ITE_LR_Free();
    ITE_HR_Free();

END:
    return ret;
}

/* -----------------------------------------------------------------------
   FUNCTION : ITE_testBMLdatapathLRHR
   PURPOSE  : Test BML datapath with raw8 ,  raw12 format
   for different resolutions
   ------------------------------------------------------------------------ */
void
ITE_testBMLdatapathLRHR(
char    RAW[6])
{
    char    pathname[200];

    ITE_LR_Prepare(&usecase, INFINITY);
    ITE_HR_Prepare(&usecase, INFINITY);
    sprintf(pathname, IMAGING_PATH "/ite_nmf/test_results_nreg/datapath_BML_%s", RAW);
    strcat(g_out_path, pathname);
    LOS_Log("Current path: >%s< \n", ( char * ) g_out_path);
    init_test_results_nreg(g_out_path);

    ITE_dbg_bmlin("BAYERLOAD1");    //usecase.BML_input = BAYERLOAD1;
    ITE_testStillBMLdatapath();

    ITE_dbg_bmlin("BAYERLOAD2");
    ITE_testStillBMLdatapath();
}


/* -----------------------------------------------------------------------
   FUNCTION : ITE_testStillBMSdatapath
   PURPOSE  : Test BMS datapath with raw8 ,  raw12 format
   for FFOV : Fast BMS (BAYERSTORE0) and BMS (BAYERSTORE2)
   ------------------------------------------------------------------------ */
void
ITE_testStillBMSdatapath(void)
{
    e_resolution    sizebms = SUBQCIF;  //__NO_WARNING_ not being set before useds;
    char            testComment[200];
    char            resolutionName[80];
    char            sensorName[16];

    if (usecase.sensor == 0)
    {
        sprintf(sensorName, "Cam0");
    }
    else
    {
        sprintf(sensorName, "Cam1");
    }



    //RAW12 Test
    usecase.BMS_Mode = BMSSTILL;

    usecase.BMS_output = BAYERSTORE2;

    usecase.BMS_GrbFormat = GRBFMT_RAW12;

    ITE_GetStringNameForBMSConfiguration(resolutionName);

    LOS_Log("\n -----> %s resolution\n", resolutionName);

    //perform a "cd" to destination repository for test log
    strcat(g_out_path, IMAGING_PATH "/ite_nmf/test_results_nreg/datapath_BMS_RAW12");

    LOS_Log("Current path: >%s< \n", ( char * ) g_out_path);
    init_test_results_nreg(g_out_path);
    LOS_Log("\n --> Stream 1 frame on BMS pipe FFOV with GRBFMT_RAW12 format\n");
    sprintf(testComment, "%s_test_nreg_BMS_still_datapath_RAW12_%s", sensorName, resolutionName);

    mmte_testStart(testComment, " test BMS buffer RAW12 format", g_out_path);
#ifdef TEST_PERFORMANCE
    mmte_testNext(testComment);
    mmte_testComment("Test passed performace parameters printed\n");
    mmte_testResult(TEST_PASSED);
#endif
    ITE_testStreamBMS(&usecase, sizebms);

    mmte_testEnd();

    //clean  g_out_path
    memset(g_out_path, 0, KlogDirectoryLentgh * sizeof(char));

    //RAW8 Test
    usecase.BMS_Mode = BMSSTILL;
    usecase.BMS_output = BAYERSTORE2;
    usecase.BMS_GrbFormat = GRBFMT_RAW8;

    ITE_GetStringNameForBMSConfiguration(resolutionName);

    LOS_Log("\n -----> %s resolution\n", resolutionName);

    //perform a "cd" to destination repository for test log
    strcat(g_out_path, IMAGING_PATH "/ite_nmf/test_results_nreg/datapath_BMS_RAW8");
    LOS_Log("Current path: >%s< \n", ( char * ) g_out_path);
    init_test_results_nreg(g_out_path);
    LOS_Log("\n --> Stream 1 frame on BMS pipe FFOV with GRBFMT_RAW8 format\n");
    sprintf(testComment, "%s_test_nreg_BMS_still_datapath_RAW8_%s", sensorName, resolutionName);
    mmte_testStart(testComment, " test BMS buffer RAW8 format", g_out_path);

#ifdef TEST_PERFORMANCE
    mmte_testNext(testComment);
    mmte_testComment("Test passed performace parameters printed\n");
    mmte_testResult(TEST_PASSED);
#endif
    ITE_testStreamBMS(&usecase, sizebms);

    mmte_testEnd();

    //clean  g_out_path
    memset(g_out_path, 0, KlogDirectoryLentgh * sizeof(char));
}


int
ITE_testVideoBMSdatapath_alloutput(
enum e_grabFormat   lrgrbformat,
enum e_grabFormat   hrgrbformat)
{
    e_resolution    sizelr,
                    sizehr;
    int ret = EOK;

    for (sizelr = usecase.LR_Min_resolution; sizelr <= usecase.LR_Max_resolution; sizelr++)
    {
        for (sizehr = usecase.HR_Min_resolution; sizehr <= usecase.HR_Video_Max_resolution; sizehr++)
        {
            if (ITE_GiveAspectRatioFromEnum(PIPE_LR, sizelr, lrgrbformat) ==
                ITE_GiveAspectRatioFromEnum(PIPE_HR, sizehr, hrgrbformat))
            {
                ret = ITE_testVideoBMSdatapath(sizelr, lrgrbformat, sizehr, hrgrbformat);
                if(ret != EOK)
                {
                    LOS_Log("ITE_testVideoBMSdatapath gave error in %s and line  %d\n",
                            __FUNCTION__, __LINE__);
                    goto END;
                }
            }
        }
    }

END:
    return ret;
}

int
ITE_testVideoBMLdatapath_alloutput(
enum e_grabFormat   lrgrbformat,
enum e_grabFormat   hrgrbformat)
{
    e_resolution    sizelr,
                    sizehr;
    int ret = EOK;

    for (sizelr = usecase.LR_Min_resolution; sizelr <= usecase.LR_Max_resolution; sizelr++)
    {
        for (sizehr = usecase.HR_Min_resolution; sizehr <= usecase.HR_Video_Max_resolution; sizehr++)
        {
            if (ITE_GiveAspectRatioFromEnum(PIPE_LR, sizelr, lrgrbformat) ==
                ITE_GiveAspectRatioFromEnum(PIPE_HR, sizehr, hrgrbformat))
            {
                ret = ITE_testVideoBMLdatapath(sizelr, lrgrbformat, sizehr, hrgrbformat);
                if (ret != EOK)
                {
                    LOS_Log("ITE_testVideoBMLdatapath_alloutput gave error in %s and line  %d\n",
                            __FUNCTION__, __LINE__);
                    goto END;
                }
            }
        }
    }

END:
    return ret;
}

/* -----------------------------------------------------------------------
   FUNCTION : ITE_testVideoBMSdatapath
   PURPOSE  : Grab from CSI on LR//HR (video resolution) and Test BMS datapath with raw8 ,  raw12 format without sensor changeover (video record resolution)
   this for different sensor resolution
   ------------------------------------------------------------------------ */
int
ITE_testVideoBMSdatapath(
e_resolution        sizelr,
enum e_grabFormat   lrgrbformat,
e_resolution        sizehr,
enum e_grabFormat   hrgrbformat)
{
    char        testComment[200];
    char        sensorName[16];
    char        lrresolutionName[16];
    char        hrresolutionName[16];
    char        lrformatName[16];
    char        hrformatName[16];
    t_uint32    BML = 0;
    int ret = EOK;

    if (usecase.sensor == 0)
    {
        sprintf(sensorName, "Cam0");
    }
    else
    {
        sprintf(sensorName, "Cam1");
    }

    usecase.LR_GrbFormat = lrgrbformat;
    ITE_GiveStringFormat("LR", lrformatName);
    usecase.HR_GrbFormat = hrgrbformat;
    ITE_GiveStringFormat("HR", hrformatName);
    ITE_GiveStringNameFromEnum(sizelr, lrresolutionName);
    LOS_Log("\n -----> %s LR resolution\n", lrresolutionName);
    ITE_GiveStringNameFromEnum(sizehr, hrresolutionName);
    LOS_Log("\n -----> %s HR resolution\n", hrresolutionName);
    usecase.LR_XSize = ITE_GiveXSizeFromEnum(PIPE_LR, sizelr, lrgrbformat);
    usecase.LR_YSize = ITE_GiveYSizeFromEnum(PIPE_LR, sizelr, lrgrbformat);
    usecase.HR_XSize = ITE_GiveXSizeFromEnum(PIPE_HR, sizehr, hrgrbformat);
    usecase.HR_YSize = ITE_GiveYSizeFromEnum(PIPE_HR, sizehr, hrgrbformat);
    usecase.LR_resolution = ITE_GiveResolution(lrresolutionName);
    usecase.HR_resolution = ITE_GiveResolution(hrresolutionName);

    // RAW12 Test
    ITE_dbg_bmsmode("VIDEO");
    ret =ITE_dbg_setusecase("BMS", "8MEGA", "RAW12");
    if(ret != EOK)
    {
        LOS_Log("ITE_dbg_setusecase gave error in %s and line  %d\n",
                __FUNCTION__, __LINE__);
        goto END;
    }

    ITE_dbg_bmsout("BAYERSTORE2");

    // Perform a "cd" to destination repository for test log
    strcat(g_out_path,
           IMAGING_PATH "/ite_nmf/test_results_nreg/datapath_BMS_VIDEO_RAW12");
    LOS_Log("Current path: >%s< \n", ( char * ) g_out_path);
    init_test_results_nreg(g_out_path);
    LOS_Log("\n --> Stream 1 frame on BMS pipe FFOV with GRBFMT_RAW12 format\n");

    sprintf(testComment,
            "%s_BMS_video_RAW12_LR_%s_%s_HR_%s_%s",
            sensorName,
            lrformatName,
            lrresolutionName,
            hrformatName,
            hrresolutionName);

    mmte_testStart(testComment, " test BMS buffer RAW12 format", g_out_path);

#ifdef TEST_PERFORMANCE
    mmte_testNext(testComment);
    mmte_testComment("Test passed performace parameters printed\n");
    mmte_testResult(TEST_PASSED);
#endif

    ITE_testStreamBMSVideo_allframerate(&usecase, BML);

    // RAW8 Test
    ITE_dbg_bmsmode("VIDEO");
    ret = ITE_dbg_setusecase("BMS", "8MEGA", "RAW8");
    if(ret != EOK)
    {
        LOS_Log("ITE_dbg_setusecase gave error in %s and line  %d\n",
                __FUNCTION__, __LINE__);
        goto END;
    }

    ITE_dbg_bmsout("BAYERSTORE2");

    // Perform a "cd" to destination repository for test log
    strcat(g_out_path,
           IMAGING_PATH "/ite_nmf/test_results_nreg/datapath_BMS_VIDEO_RAW8");
    LOS_Log("Current path: >%s< \n", ( char * ) g_out_path);
    init_test_results_nreg(g_out_path);
    LOS_Log("\n --> Stream 1 frame on BMS pipe FFOV with GRBFMT_RAW8 format\n");

    sprintf(testComment,
            "%s_BMS_video_RAW8_LR_%s_%s_HR_%s_%s",
            sensorName,
            lrformatName,
            lrresolutionName,
            hrformatName,
            hrresolutionName);

    mmte_testStart(testComment, " test BMS buffer RAW8 format", g_out_path);

#ifdef TEST_PERFORMANCE
    mmte_testNext(testComment);
    mmte_testComment("Test passed performace parameters printed\n");
    mmte_testResult(TEST_PASSED);
#endif

    ITE_testStreamBMSVideo_allframerate(&usecase, BML);

END:
    return ret;
}

/* -----------------------------------------------------------------------
   FUNCTION : ITE_testVideoBMLdatapath
   PURPOSE  : Grab from CSI on LR//HR (video resolution) and Test BMS datapath with raw8 ,  raw12 format without sensor changeover (video record resolution)
   this for different sensor resolution
   ------------------------------------------------------------------------ */
int
ITE_testVideoBMLdatapath(
e_resolution        sizelr,
enum e_grabFormat   lrgrbformat,
e_resolution        sizehr,
enum e_grabFormat   hrgrbformat)
{
    char        testComment[200];
    char        sensorName[16];
    char        lrresolutionName[16];
    char        hrresolutionName[16];
    char        lrformatName[16];
    char        hrformatName[16];
    t_uint32    BML = 1;
    int ret = EOK;

    if (usecase.sensor == 0)
    {
        sprintf(sensorName, "Cam0");
    }
    else
    {
        sprintf(sensorName, "Cam1");
    }

    usecase.LR_GrbFormat = lrgrbformat;
    ITE_GiveStringFormat("LR", lrformatName);
    usecase.HR_GrbFormat = hrgrbformat;

    ITE_GiveStringFormat("HR", hrformatName);
    ITE_GiveStringNameFromEnum(sizelr, lrresolutionName);

    LOS_Log("\n -----> %s LR resolution\n", lrresolutionName);
    ITE_GiveStringNameFromEnum(sizehr, hrresolutionName);
    LOS_Log("\n -----> %s HR resolution\n", hrresolutionName);

    usecase.LR_XSize = ITE_GiveXSizeFromEnum(PIPE_LR, sizelr, lrgrbformat);
    usecase.LR_YSize = ITE_GiveYSizeFromEnum(PIPE_LR, sizelr, lrgrbformat);
    usecase.HR_XSize = ITE_GiveXSizeFromEnum(PIPE_HR, sizehr, hrgrbformat);
    usecase.HR_YSize = ITE_GiveYSizeFromEnum(PIPE_HR, sizehr, hrgrbformat);
    usecase.LR_resolution = ITE_GiveResolution(lrresolutionName);
    usecase.HR_resolution = ITE_GiveResolution(hrresolutionName);

    // RAW12 Test
    ITE_dbg_bmsmode("VIDEO");
    ret = ITE_dbg_setusecase("BMS", "8MEGA", "RAW12");
    if(ret != EOK)
    {
        LOS_Log("ITE_dbg_setusecase gave error in %s and line  %d\n",
                __FUNCTION__, __LINE__);
        goto END;
    }

    ITE_dbg_bmsout("BAYERSTORE2");
    ITE_dbg_bmlin("BAYERLOAD1");
    LOS_Log("\n --> Stream 1 frame on BMS pipe FFOV with GRBFMT_RAW12 format\n");

    // Perform a "cd" to destination repository for test log
    strcat(g_out_path,
           IMAGING_PATH "/ite_nmf/test_results_nreg/datapath_BML_VIDEO_RAW12");
    LOS_Log("Current path: >%s< \n", ( char * ) g_out_path);
    init_test_results_nreg(g_out_path);

    sprintf(testComment,
            "%s_test_nreg_BML_video_RAW12_LR_%s_%s_HR_%s_%s",
            sensorName,
            lrformatName,
            lrresolutionName,
            hrformatName,
            hrresolutionName);

    mmte_testStart(testComment, " test BMS buffer RAW12 format", g_out_path);

#ifdef TEST_PERFORMANCE
    mmte_testNext(testComment);
    mmte_testComment("Test passed performace parameters printed\n");
    mmte_testResult(TEST_PASSED);
#endif

    ITE_testStreamBMSVideo_allframerate(&usecase, BML);

    // RAW8 Test
    ITE_dbg_bmsmode("VIDEO");
    ret = ITE_dbg_setusecase("BMS", "8MEGA", "RAW8");
    if(ret != EOK)
    {
        LOS_Log("ITE_dbg_setusecase gave error in %s and line  %d\n",
                __FUNCTION__, __LINE__);
        goto END;
    }

    ITE_dbg_bmsout("BAYERSTORE2");
    ITE_dbg_bmlin("BAYERLOAD1");
    LOS_Log("\n --> Stream 1 frame on BML pipe FFOV with GRBFMT_RAW8 format\n");

    // Perform a "cd" to destination repository for test log
    strcat(g_out_path,
           IMAGING_PATH "/ite_nmf/test_results_nreg/datapath_BML_VIDEO_RAW8");
    LOS_Log("Current path: >%s< \n", ( char * ) g_out_path);
    init_test_results_nreg(g_out_path);

    sprintf(testComment,
            "%s_test_nreg_BML_video_RAW8_LR_%s_%s_HR_%s_%s",
            sensorName,
            lrformatName,
            lrresolutionName,
            hrformatName,
            hrresolutionName);

    mmte_testStart(testComment, " test BML buffer RAW8 format", g_out_path);

#ifdef TEST_PERFORMANCE
    mmte_testNext(testComment);
    mmte_testComment("Test passed performace parameters printed\n");
    mmte_testResult(TEST_PASSED);
#endif

    ITE_testStreamBMSVideo_allframerate(&usecase, BML);

END:
    return ret;
}

/* -----------------------------------------------------------------------
   FUNCTION : ITE_testStreamBMS
   PURPOSE  : test Streaming on BMS using configuration provided through usecase :
   check output buffer
   ------------------------------------------------------------------------ */
void
ITE_testStreamBMS(
tps_sia_usecase p_sia_usecase,
e_resolution    sizebms)
{
    Result_te   result = Result_e_Success;
    UNUSED(sizebms);
#if STORE_GLACE_STATS_AND_BITMAP
    char        pathname[256];
#endif

    //    char BMSFrameName[200],resolutionName[20];
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
    ITE_dbg_usagemode("vf");

    ITE_writePE(Glace_Control_e_StatisticsFov_Byte0, StatisticsFov_e_Master_Pipe);
    ITE_LR_Start(INFINITY);

#if STORE_GLACE_STATS_AND_BITMAP
    LOS_Log("\ndumping glace buffer and bitmap file\n");
    ITE_NMF_GlaceHistoExpStatsRequestAndWait();
    ITE_NMF_DumpGlaceBuffer();
    snprintf(pathname, sizeof(pathname), IMAGING_PATH "/%s", "LR_");
    ITE_StoreinBMPFile(pathname, &(GrabBufferLR[0]));
#endif
    LOS_Log("\n1secs stream on LR pipe\n");
    LOS_Sleep(1000);

    ITE_LR_Stop();
    ITE_LR_Free();

    ITE_dbg_usagemode("still");
    ITE_BMS_Prepare(p_sia_usecase, INFINITY);

    ITE_writePE(ChannelGains_Control_e_Flag_EnableChannelGains_Byte0, Flag_e_FALSE);

    ITE_BMS_Start(1);

    LOS_Log("\nStream one frame on BMS \n");
    LOS_Sleep(1000);

    result &= ITE_testcolorBarBMS();    // lld
    ITE_BMS_Stop();

    ITE_writePE(ChannelGains_Control_e_Flag_EnableChannelGains_Byte0, Flag_e_TRUE);

    ITE_BMS_Free();

    //SUDEEP: Changed mmte_testend() to the following code for performance
    if ((result == Result_e_Failure) && (FALSE == g_test_perf_status))
    {
        LOS_Log("\nTest Failed\n");
        mmte_testComment("Test Failed,Test Performance Failed\n");
        mmte_testResult(TEST_FAILED);
    }
    else if ((result == Result_e_Failure) && (PERFORMANCE_SKIPPED == g_test_perf_status))
    {
        LOS_Log("\nTest Failed Performance Skipped\n");
        mmte_testComment("Test Performance Skipped\n");
        mmte_testResult(TEST_FAILED);
    }
    else if ((result == Result_e_Success) && (PERFORMANCE_SKIPPED == g_test_perf_status))
    {
        LOS_Log("\nTest Passed Performance Skipped\n");
        mmte_testComment("Test Performance Skipped\n");
        mmte_testResult(TEST_PASSED);
    }
    else if ((result == Result_e_Success) && (TRUE == g_test_perf_status))
    {
        LOS_Log("\nTest Passed,Test Passed in Performance\n");
        mmte_testComment("Test Passed,Test Passed in Performance\n");
        mmte_testResult(TEST_PASSED);
    }
    else
    {
        LOS_Log("\nTest Failed\n");
        mmte_testComment("Test Failed,Test Performance Passed\n");
        mmte_testResult(TEST_FAILED);
    }
}


/* -----------------------------------------------------------------------
   FUNCTION : ITE_testStreamBMSVideo_allframerate
   PURPOSE  : test Streaming on LR, HR and BMS using configuration provided through usecase :
   check output buffers
   ------------------------------------------------------------------------ */
void
ITE_testStreamBMSVideo_allframerate(
tps_sia_usecase p_sia_usecase,
t_uint32        BML)
{
    Result_te   result = Result_e_Success;

    result &= ITE_testStreamBMSVideo(p_sia_usecase, 1.0, BML);
    result &= ITE_testStreamBMSVideo(p_sia_usecase, 7.5, BML);
    result &= ITE_testStreamBMSVideo(p_sia_usecase, 15.0, BML);
    result &= ITE_testStreamBMSVideo(p_sia_usecase, 20.0, BML);
    result &= ITE_testStreamBMSVideo(p_sia_usecase, 25.0, BML);
    result &= ITE_testStreamBMSVideo(p_sia_usecase, 30.0, BML);

    if (result == Result_e_Failure)
    {
        mmte_testResult(TEST_FAILED);
    }
    else
    {
        mmte_testResult(TEST_PASSED);
    }


    mmte_testEnd();

    //clean  g_out_path
    memset(g_out_path, 0, KlogDirectoryLentgh * sizeof(char));
}


/* -----------------------------------------------------------------------
   FUNCTION : ITE_testStreamBMSVideo
   PURPOSE  : test Streaming on LR, HR and BMS using configuration provided through usecase :
   check output buffers
   ------------------------------------------------------------------------ */
Result_te
ITE_testStreamBMSVideo(
tps_sia_usecase p_sia_usecase,
float           framerate,
t_uint32        BML)
{
    char        mess[256];
    Result_te   result = Result_e_Success;

    LOS_Log("\nStream one frame on BMS with %f \n", framerate);
    sprintf(mess, "Stream one frame on BMS with %f \n", framerate);
    mmte_testComment(mess);

    if (g_InputImageSource == InputImageSource_e_Sensor0)
    {
        ITE_SensorMode(COLORBAR);
    }
    else
    {
        ITE_SetRxTestPattern(RxTestPattern_e_ColourBars);
    }


    usecase.sensormode = NORMAL_MODE;   // for LLD fw we need to reprogam each time the sensor; Sudeep commented for testing
    ITE_LRHR_Prepare(p_sia_usecase, INFINITY);

    ITE_SetFrameRateControl(framerate, framerate);

    ITE_LRHR_Start(INFINITY);

    /*
            <AG> TODO: If streaming is not successfull, the test case should not proceed.
          */
    LOS_Log("\n1secs stream on LR and HR pipe before BMS snap with sensor resolution\n");
    LOS_Sleep(1000);

    LOS_Log("\nStream one frame on BMS \n");

    //BMS size setted during BMS prepare : sensor output selected required for  HR pipe
    ITE_BMS_Prepare(p_sia_usecase, INFINITY);

    snprintf(
    mess,
    sizeof(mess),
    "BMS size width %d and height %d\n",
    ( int ) usecase.BMS_XSize,
    ( int ) usecase.BMS_YSize);         //__NO_WARNING__
    MMTE_TEST_COMMENT(mess);

    ITE_BMS_Start(1);
    LOS_Sleep(500);
    ITE_BMS_Stop();

    //Test Buffer was filled : Test values != 0xFF (default values when allocated)
    mmte_testComment("Check BMS LR and HR buffer after BMS");
    result &= ITE_testcolorBarBMS();
    result &= ITE_testcolorBarLR();
    result &= ITE_testcolorBarHR();

    if (BML == 1)
    {
        LOS_Sleep(2000);
        ITE_LRHR_Stop();
        ITE_LRHR_Free();

        ITE_LR_Prepare(p_sia_usecase, INFINITY);
        ITE_BML_Prepare(p_sia_usecase, INFINITY);
        ITE_BML_Start(1);
        ITE_BML_Stop();

        mmte_testComment("Check LR and HR buffer after BMS");
        result &= ITE_testcolorBarLR();
        result &= ITE_testcolorBarHR();
        ITE_LR_Free();
        ITE_HR_Free();
        ITE_BMS_Free();
    }
    else
    {
        ITE_LRHR_Stop();
        ITE_LRHR_Free();
        ITE_BMS_Free();
    }


    //SUDEEP: Changed mmte_testend() to the following code for performance
    if ((result == Result_e_Failure) && (FALSE == g_test_perf_status))
    {
        LOS_Log("\nTest Failed\n");
        mmte_testComment("Test Failed,Test Performance Failed\n");
        result &= Result_e_Failure;
    }
    else if ((result == Result_e_Failure) && (PERFORMANCE_SKIPPED == g_test_perf_status))
    {
        LOS_Log("\nTest Failed Performance Skipped\n");
        mmte_testComment("Test Failed Performance Skipped\n");
        result &= Result_e_Failure;
    }
    else if ((result == Result_e_Success) && (PERFORMANCE_SKIPPED == g_test_perf_status))
    {
        LOS_Log("\nTest Passed Performance Skipped\n");
        mmte_testComment("Test Passed Performance Skipped\n");
        result &= Result_e_Success;
    }
    else if ((result == Result_e_Success) && (TRUE == g_test_perf_status))
    {
        LOS_Log("\nTest Passed,Test Passed in Performance\n");
        mmte_testComment("Test Passed,Test Passed in Performance\n");
        result &= Result_e_Success;
    }
    else
    {
        LOS_Log("\nTest Failed\n");
        mmte_testComment("Test Failed,Test Performance Passed\n");
        result &= Result_e_Failure;
    }


    return (result);
}


/* -----------------------------------------------------------------------
   FUNCTION : ITE_testStreamBML
   PURPOSE  : test Streaming on BML after grabbing frame on BMS , this using configuration provided through usecase :
   ------------------------------------------------------------------------ */
void
ITE_testStreamBML(
tps_sia_usecase p_sia_usecase,
e_resolution    sizebml)
{
    Result_te   result = Result_e_Success;
    UNUSED(sizebml);

    ITE_BML_Prepare(p_sia_usecase, INFINITY);
    ITE_BML_Start(1);
    LOS_Log("\nInject one frame on BML \n");

    if(usecase.LR_GrbFormat == GRBFMT_R5G6B5)
    {
    result &= ITE_testcolorBarLRGeneric();
    }

    ITE_BML_Stop();

    //SUDEEP: Changed mmte_testend() to the following code for performance
    if ((result == Result_e_Failure) && (FALSE == g_test_perf_status))
    {
        LOS_Log("\nTest Failed\n");
        mmte_testComment("Test Failed,Test Performance Failed\n");
        mmte_testResult(TEST_FAILED);
    }
    else if ((result == Result_e_Failure) && (PERFORMANCE_SKIPPED == g_test_perf_status))
    {
        LOS_Log("\nTest Failed Performance Skipped\n");
        mmte_testComment("Test Performance Skipped\n");
        mmte_testResult(TEST_FAILED);
    }
    else if ((result == Result_e_Success) && (PERFORMANCE_SKIPPED == g_test_perf_status))
    {
        LOS_Log("\nTest Passed Performance Skipped\n");
        mmte_testComment("Test Performance Skipped\n");
        mmte_testResult(TEST_PASSED);
    }
    else if ((result == Result_e_Success) && (TRUE == g_test_perf_status))
    {
        LOS_Log("\nTest Passed,Test Passed in Performance\n");
        mmte_testComment("Test Passed,Test Passed in Performance\n");
        mmte_testResult(TEST_PASSED);
    }
    else
    {
        LOS_Log("\nTest Failed\n");
        mmte_testComment("Test Failed,Test Performance Passed\n");
        mmte_testResult(TEST_FAILED);
    }


    mmte_testEnd();

    //clean  g_out_path
    memset(g_out_path, 0, KlogDirectoryLentgh * sizeof(char));
}


/* -----------------------------------------------------------------------
   FUNCTION : ITE_testStillBMLdatapath
   PURPOSE  : Test BML datapath with raw8 ,  raw12 format
   for FFOV : BAYERLOAD1 and BAYERLOAD2
   ------------------------------------------------------------------------ */
void
ITE_testStillBMLdatapath(void)
{
    e_resolution    sizebms = SUBQCIF;  //__NO_WARNING_ not being set before used
    char            testComment[200];
    char            Comment[128];
    char            resolutionName[80];
    char            sensorName[16];
    char            bmlinputName[16];
    char            bmlformatName[16];
    char            LRresolutionName[16];
    char            HRresolutionName[16];
    char            LRFormatName[16];
    char            HRFormatName[16];

    if (usecase.sensor == 0)
    {
        sprintf(sensorName, "Cam0");
    }
    else
    {
        sprintf(sensorName, "Cam1");
    }


    //ITE_GiveStringNameFromEnum(maxSizebms, resolutionName);
    ITE_GetStringNameForBMSConfiguration(resolutionName);

    ITE_GiveStringBMLinput(bmlinputName);
    ITE_GiveStringFormat("BML", bmlformatName);
    ITE_GiveStringResolution("LR", LRresolutionName);
    ITE_GiveStringResolution("HR", HRresolutionName);
    ITE_GiveStringFormat("LR", LRFormatName);
    ITE_GiveStringFormat("HR", HRFormatName);

    LOS_Log(
    "\n --> Inject 1 frame on BML FFOV using %s with %s %s  format\n",
    bmlinputName,
    bmlformatName,
    resolutionName);

    sprintf(
    testComment,
    "%s_nreg_%s_%s_%s_LR_%s_%s_HR_%s_%s",
    sensorName,
    bmlinputName,
    bmlformatName,
    resolutionName,
    LRresolutionName,
    LRFormatName,
    HRresolutionName,
    HRFormatName);

    sprintf(Comment, "test BML buffer %s %s format", bmlinputName, bmlformatName);

    mmte_testStart(testComment, Comment, g_out_path);
#ifdef TEST_PERFORMANCE
    mmte_testNext(testComment);
    mmte_testComment("Test passed performace parameters printed\n");
    mmte_testResult(TEST_PASSED);
#endif
    if
    (
        (usecase.flag_backup_fw == 1)
    &&  ((usecase.HR_resolution == QHD) || (usecase.HR_resolution == WVGA) || (usecase.HR_resolution == HD))
    )
    {   // log test fail
        mmte_testResult(TEST_FAILED);
    }   // ISsue with these two value bug number 93316
    else
    {
        ITE_testStreamBML(&usecase, sizebms);
    }
}


/* -----------------------------------------------------------------------
   FUNCTION : ITE_testZoomstreamingLRorHR
   PURPOSE  : Test zoom streaming on LR and HR independantly
   for different output LR HR resolutions
   ------------------------------------------------------------------------ */
int
ITE_testZoomstreaming(
enum e_grabFormat   lrgrbformat,
enum e_grabFormat   hrgrbformat,
char                *Pipe)
{
    e_resolution        lrresolution;
    e_resolution        hrresolution;
    char                lrresolutionstring[16];
    char                hrresolutionstring[16];
    char                lrformatstring[16];
    char                hrformatstring[16];
    t_uint16            maxfps;
    volatile t_uint32   MaxDZ;
    volatile t_uint32   MaxFOVX;
    volatile t_uint32   MinFOVX;
    t_uint32            after,
                        before                                       = 0;
    int ret = EOK;
    maxfps = 30;
    if (0 == strcmp(Pipe, "LR"))
    {
        for (lrresolution = usecase.LR_Min_resolution; lrresolution <= usecase.LR_Max_resolution; lrresolution++)
        {
            ITE_GiveStringNameFromEnum(lrresolution, lrresolutionstring);
            ITE_GiveFormatStringFromEnum(lrgrbformat, lrformatstring);
            ret = ITE_dbg_setusecase("LR", lrresolutionstring, lrformatstring);
            if(ret != EOK)
            {
                LOS_Log("ITE_dbg_setusecase gave error in %s and line  %d\n",__FUNCTION__,__LINE__);
                goto END;
            }


            ITE_dbg_usagemode("vf");
            ITE_LR_Prepare(&usecase, INFINITY);
            ITE_writePE(Zoom_Control_f_SetFOVX_Byte0, 0);
            ITE_LR_Start(INFINITY);
            LOS_Sleep(500);
            MaxFOVX = ITE_readPE(Zoom_Status_f_MaxAvailableFOVXAtCurrentCenter_Byte0);
            MinFOVX = usecase.LR_XSize / 6;
            MaxDZ = (*( volatile float * ) &MaxFOVX) / MinFOVX;
            ITE_LR_Stop();
            ITE_NMF_DZ_Init(( float ) MaxDZ);
            MaxDZ = ITE_readPE(Zoom_Params_f_ZoomRange_Byte0);
            ITE_SetFrameRateControl(maxfps, maxfps);
            ITE_LR_Start(INFINITY);
            LOS_Sleep(500);
            ITE_NMF_ZoomTest_FullRange("LR", (*( volatile float * ) &MaxDZ));   // MaxDZ
            ITE_LR_Stop();
            ITE_LR_Free();
            after = LOS_GetAllocatedChunk();
            LOS_Log("Not DeAllocated Memory chunk : \t %d - %d = %d\n", after, before, after - before);
        }
    }


    if (0 == strcmp(Pipe, "HR"))
    {
        for (hrresolution = usecase.HR_Min_resolution; hrresolution <= usecase.HR_Video_Max_resolution; hrresolution++)
        {
            ITE_GiveStringNameFromEnum(hrresolution, hrresolutionstring);
            ITE_GiveFormatStringFromEnum(hrgrbformat, hrformatstring);
            ret =ITE_dbg_setusecase("HR", hrresolutionstring, hrformatstring);
            if(ret != EOK)
            {
                LOS_Log("ITE_dbg_setusecase gave error in %s and line  %d\n",__FUNCTION__,__LINE__);
                goto END;
            }


            if (usecase.HR_XSize >= (t_uint32)ITE_GiveXSizeFromEnum(PIPE_HR, HD, hrgrbformat))
            {
                ITE_dbg_usagemode("videohq");
            }
            else
            {
                ITE_dbg_usagemode("video");
            }


            ITE_HR_Prepare(&usecase, INFINITY);
            ITE_writePE(Zoom_Control_f_SetFOVX_Byte0, 0);
            ITE_HR_Start(INFINITY);
            LOS_Sleep(500);
            MaxFOVX = ITE_readPE(Zoom_Status_f_MaxAvailableFOVXAtCurrentCenter_Byte0);
            MinFOVX = usecase.HR_XSize / 6;
            MaxDZ = (*( volatile float * ) &MaxFOVX) / MinFOVX;
            ITE_HR_Stop();
            ITE_NMF_DZ_Init(( float ) MaxDZ);
            MaxDZ = ITE_readPE(Zoom_Params_f_ZoomRange_Byte0);
            ITE_SetFrameRateControl(maxfps, maxfps);
            ITE_HR_Start(INFINITY);
            LOS_Sleep(500);

            ITE_NMF_ZoomTest_FullRange("HR", (*( volatile float * ) &MaxDZ));   // MaxDZ
            ITE_HR_Stop();
            ITE_HR_Free();
            after = LOS_GetAllocatedChunk();
            LOS_Log("Not DeAllocated Memory chunk : \t %d\n", after - before);
        }
    }
    END:
    return ret;

}


int
ITE_bugZoomBMLStill(
enum e_grabFormat   lrgrbformat,
enum e_grabFormat   hrgrbformat)
{
    t_sint16            i;
    volatile float      DZ;
    volatile t_uint32   MaxDZ;
    volatile t_uint32   MaxFOVX;
    volatile t_uint32   MinFOVX;
    TAlgoError          ret = EErrorNone;
    int retval= EOK;
    t_uint8             maxstep = 5;

    UNUSED(lrgrbformat);
    UNUSED(hrgrbformat);

    ret =ITE_dbg_setusecase("LR", "CIF", "YUV422RI");
    if(retval != EOK)
    {
        LOS_Log("ITE_dbg_setusecase gave error in %s and line  %d\n",__FUNCTION__,__LINE__);
        ret = EErrorArgument;
        goto END;
    }

    retval =ITE_dbg_setusecase("HR", "QCIF", "YUV420MB_D");
    if(retval != EOK)
    {
        LOS_Log("ITE_dbg_setusecase gave error in %s and line  %d\n",__FUNCTION__,__LINE__);
        ret = EErrorArgument;
        goto END;
    }

    ITE_dbg_bmsmode("STILL");
    retval =ITE_dbg_setusecase("BMS", "8MEGA", "RAW8");
    if(retval != EOK)
    {
        LOS_Log("ITE_dbg_setusecase gave error in %s and line  %d\n",__FUNCTION__,__LINE__);
        ret = EErrorArgument;
        goto END;
    }

    ITE_dbg_bmsout("BAYERSTORE2");
    ITE_dbg_bmlin("BAYERLOAD1");

    //stream first on LR
    ITE_LR_Prepare(&usecase, INFINITY);
    ITE_writePE(Zoom_Control_f_SetFOVX_Byte0, 0);
    ITE_SetFrameRateControl(15, 15);

    // until here
    ITE_LR_Start(INFINITY);
    LOS_Log("\n500 millisecs stream on LR pipe\n");
    LOS_Sleep(500);
    ITE_LR_Stop();
    ITE_StoreCurrentFOVX();

    ITE_BMS_Free();
    ITE_BML_Free();
    ITE_LR_Free();
    ITE_HR_Free();

    retval =ITE_dbg_setusecase("LR", "CIFPLUS", "YUV422RI");
    if(retval != EOK)
    {
        LOS_Log("ITE_dbg_setusecase gave error in %s and line  %d\n",__FUNCTION__,__LINE__);
        ret = EErrorArgument;
        goto END;
    }

    retval = ITE_dbg_setusecase("HR", "QCIFPLUS", "YUV420MB_D");
    if(retval != EOK)
    {
        LOS_Log("ITE_dbg_setusecase gave error in %s and line  %d\n",__FUNCTION__,__LINE__);
        ret = EErrorArgument;
        goto END;
    }

    ITE_dbg_bmsmode("STILL");
    retval = ITE_dbg_setusecase("BMS", "8MEGA", "RAW8");
    if(retval != EOK)
    {
        LOS_Log("ITE_dbg_setusecase gave error in %s and line  %d\n",__FUNCTION__,__LINE__);
        ret = EErrorArgument;
        goto END;
    }

    ITE_dbg_bmsout("BAYERSTORE2");
    ITE_dbg_bmlin("BAYERLOAD1");

    //stream first on LR
    ITE_LR_Prepare(&usecase, INFINITY);
    ITE_writePE(Zoom_Control_f_SetFOVX_Byte0, 0);
    ITE_SetFrameRateControl(15, 15);

    // until here
    ITE_LR_Start(INFINITY);
    LOS_Log("\n500 millisecs stream on LR pipe\n");
    LOS_Sleep(500);
    ITE_LR_Stop();
    ITE_StoreCurrentFOVX();

    ITE_BMS_Prepare(&usecase, INFINITY);
    ITE_BMS_Start(1);
    LOS_Log("\nStream one frame on BMS \n");
    ITE_BMS_Stop();

    MaxFOVX = ITE_readPE(Zoom_Status_f_MaxAvailableFOVXAtCurrentCenter_Byte0);
    if (usecase.LR_XSize > usecase.HR_XSize)
    {
        MinFOVX = usecase.LR_XSize / 6;
    }
    else
    {
        MinFOVX = usecase.HR_XSize / 6;
    }


    MaxDZ = (*( volatile float * ) &MaxFOVX) / MinFOVX;
    ITE_NMF_DZ_Init(( float ) MaxDZ);

    MaxDZ = ITE_readPE(Zoom_Params_f_ZoomRange_Byte0);

    ITE_BML_Prepare(&usecase, INFINITY);

    // for reference FFOV buffer creation
    BmlInfo.Previous_f_SetFOVX = 0;
    ITE_BML_Start(1);
    LOS_Log("\nINject one frame on BML \n");
    LOS_Sleep(50);
    ITE_BML_Stop();

    for (i = 0; i <= maxstep; i++)
    {
        DZ = 1 + (i * ((*( volatile float * ) &MaxDZ - 1) / maxstep));
        LOS_Log("ZoomTest for DZ = %f (MaxDZ= %f)\n", DZ, *( volatile float * ) &MaxDZ);
        ret = ITE_NMF_STILLZoomTest("HR", DZ,0);//NO STRIPE
    }


    ITE_BMS_Free();
    ITE_BML_Free();
    ITE_LR_Free();
    ITE_HR_Free();

    retval = ITE_dbg_setusecase("LR", "CIF", "YUV422RI");
    if(retval != EOK)
    {
        LOS_Log("ITE_dbg_setusecase gave error in %s and line  %d\n",__FUNCTION__,__LINE__);
        ret = EErrorArgument;
        goto END;
    }

    retval = ITE_dbg_setusecase("HR", "QCIF", "YUV420MB_D");
    if(retval != EOK)
    {
        LOS_Log("ITE_dbg_setusecase gave error in %s and line  %d\n",__FUNCTION__,__LINE__);
        ret = EErrorArgument;
        goto END;
    }

    ITE_dbg_bmsmode("STILL");
    retval = ITE_dbg_setusecase("BMS", "8MEGA", "RAW8");
    if(retval != EOK)
    {
        LOS_Log("ITE_dbg_setusecase gave error in %s and line  %d\n",__FUNCTION__,__LINE__);
        ret = EErrorArgument;
        goto END;
    }

    ITE_dbg_bmsout("BAYERSTORE2");
    ITE_dbg_bmlin("BAYERLOAD1");

    //stream first on LR
    ITE_LR_Prepare(&usecase, INFINITY);
    ITE_writePE(Zoom_Control_f_SetFOVX_Byte0, 0);
    ITE_SetFrameRateControl(15, 15);

    // until here
    ITE_LR_Start(INFINITY);
    LOS_Log("\n500 millisecs stream on LR pipe\n");
    LOS_Sleep(500);
    ITE_LR_Stop();
    ITE_StoreCurrentFOVX();

    ITE_BMS_Prepare(&usecase, INFINITY);
    ITE_BMS_Start(1);
    LOS_Log("\nStream one frame on BMS \n");
    ITE_BMS_Stop();

    MaxFOVX = ITE_readPE(Zoom_Status_f_MaxAvailableFOVXAtCurrentCenter_Byte0);
    if (usecase.LR_XSize > usecase.HR_XSize)
    {
        MinFOVX = usecase.LR_XSize / 6;
    }
    else
    {
        MinFOVX = usecase.HR_XSize / 6;
    }


    MaxDZ = (*( volatile float * ) &MaxFOVX) / MinFOVX;
    ITE_NMF_DZ_Init(( float ) MaxDZ);

    MaxDZ = ITE_readPE(Zoom_Params_f_ZoomRange_Byte0);

    ITE_BML_Prepare(&usecase, INFINITY);

    // for reference FFOV buffer creation
    BmlInfo.Previous_f_SetFOVX = 0;
    ITE_BML_Start(1);
    LOS_Log("\nINject one frame on BML \n");
    LOS_Sleep(50);
    ITE_BML_Stop();

    for (i = 0; i <= maxstep; i++)
    {
        DZ = 1 + (i * ((*( volatile float * ) &MaxDZ - 1) / maxstep));
        LOS_Log("ZoomTest for DZ = %f (MaxDZ= %f)\n", DZ, *( volatile float * ) &MaxDZ);
        ret = ITE_NMF_STILLZoomTest("HR", DZ,0);//NO STRIPE
    }


    ITE_BMS_Free();
    ITE_BML_Free();
    ITE_LR_Free();
    ITE_HR_Free();
    END:
    return ret;
}


/* -----------------------------------------------------------------------
   FUNCTION : ITE_testZoomBMLStill
   PURPOSE  : Test zoom BML Still with raw8 ,  raw12 format
   for different output LR HR resolutions
   ------------------------------------------------------------------------ */
int
ITE_testZoomBMLStill(
enum e_grabFormat   lrgrbformat,
enum e_grabFormat   hrgrbformat,t_uint32 stripe)
{
    e_resolution    lrresolution;
    e_resolution    hrresolution;
    char            lrresolutionstring[16];
    char            hrresolutionstring[16];
    char            lrformatstring[16];
    char            hrformatstring[16];
    char            BMSresolution[16];
    int ret = EOK;
    ITE_GiveStringNameFromEnum(usecase.HR_Video_Max_resolution, BMSresolution);

    // RAW8
    ITE_dbg_bmsmode("STILL");
    ret = ITE_dbg_setusecase("BMS", BMSresolution, "RAW8");
    if(ret != EOK)
    {
        LOS_Log("ITE_dbg_setusecase gave error in %s and line  %d\n",
                __FUNCTION__, __LINE__);
        goto END;
    }

    ITE_dbg_bmsout("BAYERSTORE2");

    ITE_dbg_usagemode("vf");

    // Stream first on LR
    ITE_LR_Prepare(&usecase, INFINITY);
    ITE_writePE(Zoom_Control_f_SetFOVX_Byte0, 0);
    ITE_SetFrameRateControl(15, 15);

    // Until here
    ITE_LR_Start(INFINITY);
    LOS_Log("\n500 millisecs stream on LR pipe\n");
    LOS_Sleep(500);
    ITE_LR_Stop();

    // ITE_DisplayBuffer(&(GrabBufferLR[0]));
    ITE_StoreCurrentFOVX();

    ITE_BMS_Prepare(&usecase, INFINITY);

    ITE_dbg_usagemode("still");
    ITE_BMS_Start(1);
    LOS_Log("\nStream one frame on BMS \n");
    ITE_BMS_Stop();

    for (lrresolution = usecase.LR_Min_resolution; lrresolution <= usecase.LR_Max_resolution; lrresolution++)
    {
        for (hrresolution = usecase.HR_Min_resolution; hrresolution <= usecase.HR_Still_Max_resolution; hrresolution++)
        {
            ITE_GiveStringNameFromEnum(lrresolution, lrresolutionstring);
            ITE_GiveStringNameFromEnum(hrresolution, hrresolutionstring);
            ITE_GiveFormatStringFromEnum(lrgrbformat, lrformatstring);
            ITE_GiveFormatStringFromEnum(hrgrbformat, hrformatstring);
            LOS_Log("LR resolution: %s AR:%f  HR resolution:%s AR:%f\n",
                    lrresolutionstring,
                    ITE_GiveAspectRatioFromEnum(PIPE_LR, lrresolution, lrgrbformat),
                    hrresolutionstring,
                    ITE_GiveAspectRatioFromEnum(PIPE_HR, hrresolution, hrgrbformat));

            /* if (ITE_GiveAspectRatioFromEnum(PIPE_LR, lrresolution,lrgrbformat) ==
                   ITE_GiveAspectRatioFromEnum(PIPE_HR, hrresolution,hrgrbformat)) */
            {
                ret = ITE_dbg_setusecase("LR", lrresolutionstring, lrformatstring);
                if(ret != EOK)
                {
                    LOS_Log("ITE_dbg_setusecase gave error in %s and line  %d\n",
                            __FUNCTION__, __LINE__);
                    goto END;
                }

                ret = ITE_dbg_setusecase("HR", hrresolutionstring, hrformatstring);
                if(ret != EOK)
                {
                    LOS_Log("ITE_dbg_setusecase gave error in %s and line  %d\n",
                            __FUNCTION__, __LINE__);
                    goto END;
                }

                LOS_Log("Zoom BML Still Test: LR_reso %s, HR_reso %s \n",
                        lrresolutionstring,
                        hrresolutionstring);
                ITE_testZoomBMLStillLRHR(stripe);
            }
        }
    }

    ITE_BMS_Free();

    // RAW12
    ITE_dbg_bmsmode("STILL");
    ret = ITE_dbg_setusecase("BMS", BMSresolution, "RAW12");
    if(ret != EOK)
    {
        LOS_Log("ITE_dbg_setusecase gave error in %s and line  %d\n",
                __FUNCTION__, __LINE__);
        goto END;
    }

    ITE_dbg_bmsout("BAYERSTORE2");

    // Stream first on LR
    ITE_LR_Prepare(&usecase, INFINITY);
    ITE_writePE(Zoom_Control_f_SetFOVX_Byte0, 0);
    ITE_SetFrameRateControl(15, 15);

    // Until here
    ITE_dbg_usagemode("vf");
    ITE_LR_Start(INFINITY);
    LOS_Log("\n500 millisecs stream on LR pipe\n");
    LOS_Sleep(500);
    ITE_LR_Stop();

    // ITE_DisplayBuffer(&(GrabBufferLR[0]));
    ITE_StoreCurrentFOVX();

    ITE_dbg_usagemode("still");

    ITE_BMS_Prepare(&usecase, INFINITY);
    ITE_BMS_Start(1);
    LOS_Log("\nStream one frame on BMS \n");
    ITE_BMS_Stop();

    for (lrresolution = usecase.LR_Min_resolution; lrresolution <= usecase.LR_Max_resolution; lrresolution++)
    {
        for (hrresolution = usecase.HR_Min_resolution; hrresolution <= usecase.HR_Still_Max_resolution; hrresolution++)
        {
            ITE_GiveStringNameFromEnum(lrresolution, lrresolutionstring);
            ITE_GiveStringNameFromEnum(hrresolution, hrresolutionstring);
            ITE_GiveFormatStringFromEnum(lrgrbformat, lrformatstring);
            ITE_GiveFormatStringFromEnum(hrgrbformat, hrformatstring);
            LOS_Log("LR resolution: %s AR:%f  HR resolution:%s AR:%f\n",
                    lrresolutionstring,
                    ITE_GiveAspectRatioFromEnum(PIPE_LR, lrresolution, lrgrbformat),
                    hrresolutionstring,
                    ITE_GiveAspectRatioFromEnum(PIPE_HR, hrresolution, hrgrbformat));

            /* if (ITE_GiveAspectRatioFromEnum(PIPE_LR, lrresolution,lrgrbformat) ==
                   ITE_GiveAspectRatioFromEnum(PIPE_HR, hrresolution,hrgrbformat)) */
            {
                ret = ITE_dbg_setusecase("LR", lrresolutionstring, lrformatstring);
                if(ret != EOK)
                {
                    LOS_Log("ITE_dbg_setusecase gave error in %s and line  %d\n",
                            __FUNCTION__, __LINE__);
                    goto END;
                }

                ret = ITE_dbg_setusecase("HR", hrresolutionstring, hrformatstring);
                if(ret != EOK)
                {
                    LOS_Log("ITE_dbg_setusecase gave error in %s and line  %d\n",
                            __FUNCTION__, __LINE__);
                    goto END;
                }

                ITE_testZoomBMLStillLRHR(stripe);
            }
        }
    }

    ITE_BMS_Free();

    // Clear g_out_path
    memset(g_out_path, 0, KlogDirectoryLentgh * sizeof(char));

END:
    return ret;
}

/* -----------------------------------------------------------------------
   FUNCTION : ITE_testZoomBMLStillLRHR
   PURPOSE  : Test BML zoom datapath with raw8 ,  raw12 format
   for different resolutions
   ------------------------------------------------------------------------ */
void
ITE_testZoomBMLStillLRHR(t_uint32 stripe)
{

    /*
            ITE_GiveStringNameFromEnum(usecase.HR_Video_Max_resolution, BMSresolution);

            ITE_dbg_bmsmode("STILL");
            ITE_dbg_setusecase("BMS",BMSresolution,"RAW8");
            ITE_dbg_bmsout("BAYERSTORE2");

            //perform a "cd" to destination repository for test log
            strcat(g_out_path,IMAGING_PATH"/ite_nmf/test_results_nreg/datapath_ZOOM_BML_STILL_RAW8");
            LOS_Log("Current path: >%s< \n",(char *)g_out_path);
          */
    ITE_LR_Prepare(&usecase, INFINITY);
    ITE_HR_Prepare(&usecase, INFINITY);

    ITE_dbg_bmlin("BAYERLOAD1");

    ITE_testZoomBMLStilldatapath(stripe);

    /*
            ITE_dbg_setusecase("BMS",BMSresolution,"RAW8");
            ITE_dbg_bmsout("BAYERSTORE2");
          */
    ITE_dbg_bmlin("BAYERLOAD2");
    ITE_testZoomBMLStilldatapath(stripe);

    //  ITE_BML_Free();
    ITE_LR_Free();
    ITE_HR_Free();

    //clean  g_out_path
    //   memset ( g_out_path, 0, KlogDirectoryLentgh*sizeof (char) );
    /*
            ITE_dbg_bmsmode("STILL");
            ITE_dbg_setusecase("BMS",BMSresolution,"RAW12");
            ITE_dbg_bmsout("BAYERSTORE2");

            //perform a "cd" to destination repository for test log
            strcat(g_out_path,IMAGING_PATH"/ite_nmf/test_results_nreg/datapath_ZOOM_BML_STILL_RAW12");
            LOS_Log("Current path: >%s< \n",(char *)g_out_path);

            ITE_dbg_bmlin("BAYERLOAD1");
            ITE_testZoomBMLStilldatapath();

            ITE_dbg_setusecase("BMS",BMSresolution,"RAW12");
            ITE_dbg_bmsout("BAYERSTORE2");
            ITE_dbg_bmlin("BAYERLOAD2");
            ITE_testZoomBMLStilldatapath();

            //clean  g_out_path
            memset ( g_out_path, 0, KlogDirectoryLentgh*sizeof (char) );
          */
}


/* -----------------------------------------------------------------------
   FUNCTION : ITE_testZoomBMLStilldatapath
   PURPOSE  : Test BML ZOOM in STILL Mode with raw8 ,  raw12 format
   for FFOV : BAYERLOAD1 and BAYERLOAD2
   ------------------------------------------------------------------------ */
void
ITE_testZoomBMLStilldatapath(t_uint32 stripe)
{
    char            testComment[200];
    char            Comment[128];
    char            resolutionName[32];
    char            sensorName[16];
    char            bmlinputName[16];
    char            bmlformatName[16];
    char            LRresolutionName[16];
    char            HRresolutionName[16];
    char            LRFormatName[16];
    char            HRFormatName[16];

    if ((usecase.BMS_GrbFormat == GRBFMT_FAST_RAW8) ||
        (usecase.BMS_GrbFormat == GRBFMT_RAW8))
    {
        //perform a "cd" to destination repository for test log
        strcat(g_out_path, IMAGING_PATH "/ite_nmf/test_results_nreg/datapath_ZOOM_BML_STILL_RAW8");
        LOS_Log("Current path: >%s< \n", ( char * ) g_out_path);
        init_test_results_nreg(g_out_path);
    }
    else
    {
        //perform a "cd" to destination repository for test log
        strcat(g_out_path, IMAGING_PATH "/ite_nmf/test_results_nreg/datapath_ZOOM_BML_STILL_RAW12");
        LOS_Log("Current path: >%s< \n", ( char * ) g_out_path);
        init_test_results_nreg(g_out_path);
    }


    if (usecase.sensor == 0)
    {
        sprintf(sensorName, "Cam0");
    }
    else
    {
        sprintf(sensorName, "Cam1");
    }


    //ITE_GiveStringNameFromEnum(maxSizebms, resolutionName);
    ITE_GetStringNameForBMSResolution(resolutionName);

    //LOS_Log("\n -----> %s resolution\n",resolutionName);
    ITE_GiveStringBMLinput(bmlinputName);
    ITE_GiveStringFormat("BML", bmlformatName);
    ITE_GiveStringResolution("LR", LRresolutionName);
    ITE_GiveStringResolution("HR", HRresolutionName);
    ITE_GiveStringFormat("LR", LRFormatName);
    ITE_GiveStringFormat("HR", HRFormatName);

    //LOS_Log("\n --> Inject 1 frame on BML FFOV using %s with %s %s  format\n",bmlinputName,bmlformatName,resolutionName);
    sprintf(
    testComment,
    "%s_test_nreg_ZOOM_BML_STILL_%s_%s_%s_LR_%s_%s_HR_%s_%s",
    sensorName,
    bmlinputName,
    bmlformatName,
    resolutionName,
    LRresolutionName,
    LRFormatName,
    HRresolutionName,
    HRFormatName);
    sprintf(Comment, "test BML buffer %s %s format", bmlinputName, bmlformatName);
    mmte_testStart(Comment, "test BML buffer %s %s format", g_out_path);
    mmte_testNext(Comment);
    mmte_testComment(testComment);

    ITE_testZoomBML(&usecase, 4, stripe);

    mmte_testEnd();

    //clean  g_out_path
    memset(g_out_path, 0, KlogDirectoryLentgh * sizeof(char));
}


/* -----------------------------------------------------------------------
   FUNCTION : ITE_testZoomBML
   PURPOSE  : test All possible Zoom BML after grabbing frame on BMS , this using configuration provided through usecase :
   ------------------------------------------------------------------------ */
void
ITE_testZoomBML(
tps_sia_usecase p_sia_usecase,
t_uint8         maxstep,t_uint32 stripe_count)
{
    t_sint16            i;
    volatile float      DZ;
    volatile t_uint32   MaxDZ;
    volatile t_uint32   MaxFOVX;
    volatile t_uint32   MinFOVX;
    t_uint32            Num = 0;
    TAlgoError          ret = EErrorNone;
    char                mess[256];

    Num = 0;

    /*
          //stream first on LR
          ITE_LR_Prepare(&usecase, INFINITY);
          ITE_writePE(Zoom_Control_f_SetFOVX_Byte0,0);
          ITE_SetFrameRateControl(15,15);
          // until here
          ITE_LR_Start(INFINITY);
          LOS_Log("\n500 millisecs stream on LR pipe\n");
          LOS_Sleep(500);
          ITE_LR_Stop();
          //   ITE_DisplayBuffer(&(GrabBufferLR[0]));
          ITE_StoreCurrentFOVX();

          ITE_BMS_Prepare(p_sia_usecase, INFINITY);
          ITE_BMS_Start(1);
          LOS_Log("\nStream one frame on BMS \n");
          ITE_BMS_Stop();
          */
    MaxFOVX = ITE_readPE(Zoom_Status_f_MaxAvailableFOVXAtCurrentCenter_Byte0);
    if (usecase.LR_XSize > usecase.HR_XSize)
    {
        MinFOVX = usecase.LR_XSize / 6;
    }
    else
    {
        MinFOVX = usecase.HR_XSize / 6;
    }


    MaxDZ = (*( volatile float * ) &MaxFOVX) / MinFOVX;
    ITE_NMF_DZ_Init(( float ) MaxDZ);

    MaxDZ = ITE_readPE(Zoom_Params_f_ZoomRange_Byte0);

    if(stripe_count > 0)
    {
        ITE_BML_stripe_Prepare(p_sia_usecase, INFINITY);
        // for reference FFOV buffer creation
        BmlInfo.Previous_f_SetFOVX = 0;
        ITE_BML_stripe_Start(1,stripe_count);
    }
    else
    {
        ITE_BML_Prepare(p_sia_usecase, INFINITY);
        // for reference FFOV buffer creation
        BmlInfo.Previous_f_SetFOVX = 0;
        ITE_BML_Start(1);
    }
    LOS_Log("\nINject one frame on BML \n");
    LOS_Sleep(50);
    ITE_BML_Stop();

    //for (i = 0; i<= maxstep ;i++) {
    for (i = 0; i < maxstep; i++)
    {
        DZ = 1 + (i * ((*( volatile float * ) &MaxDZ - 1) / maxstep));
        LOS_Log("ZoomTest for DZ = %f (MaxDZ= %f)\n", DZ, *( volatile float * ) &MaxDZ);
        snprintf(mess, sizeof(mess), "Test %d :check DZ Value %f ", ( int ) Num++, DZ);
        mmte_testNext(mess);
        ret = ITE_NMF_STILLZoomTest("HR", DZ,stripe_count);
        switch (ret)
        {
            case (EErrorNone):
                mmte_testResult(TEST_PASSED);
                break;

            case (EErrorNoMemory):
                mmte_testComment("Buffer cannot be checked due to lack of memory");
                mmte_testResult(TEST_SKIPPED);
                break;

            default:
                mmte_testResult(TEST_FAILED);
                break;
        }
    }


    //    ITE_BMS_Free();
}


/* -----------------------------------------------------------------------
   FUNCTION : ITE_testBufferLR
   PURPOSE  : Test if GrabBufferLR[0] buffer was filled;
   use samples in buffer and check different from default values
   use mmte_testResult() API
   ------------------------------------------------------------------------ */
void
ITE_testBufferLR(void)
{
    char    mess[256];

    //Result_te   result = Result_e_Success;
    snprintf(mess, sizeof(mess), "Test %d :check if GrabBufferLR[0] was filled", ( int ) testNum);  //__NO_WARNING__
    mmte_testNext(mess);

    if (ITE_CheckGrabInBuffer(GrabBufferLR[0].logAddress, GrabBufferLR[0].buffersizeByte, 0xff) == 1)
    {
        MMTE_TEST_COMMENT("TEST OK: GrabBufferLR[0] buffer was filled with values different from default value\n");
        PERFORMANCE_RESULT_PASSED();
    }
    else
    {
        MMTE_TEST_COMMENT(
        "TEST FAIL: GrabBufferLR[0] buffer not properly filled (too many default values in buffer)\n");
        PERFORMANCE_RESULT_FAILED();
    }


    testNum++;
}


/* -----------------------------------------------------------------------
   FUNCTION : ITE_testcolorBarLR
   PURPOSE  : Test if GrabBufferLR[0] buffer contains colorbar;
   use mmte_testResult() API
   ------------------------------------------------------------------------ */
Result_te
ITE_testcolorBarLR(void)
{
    char        mess[256];
    Result_te   result = Result_e_Success;

    snprintf(mess, sizeof(mess), "Test %d :check if GrabBufferLR[0] was filled with colorbar", ( int ) testNum);    //__NO_WARNING__
    mmte_testNext(mess);

    if (TestColorBarbuffer(&GrabBufferLR[0], COLOR_BAR__DEFAULT_TESTING_POSITION) == 0)
    {
        MMTE_TEST_COMMENT("TEST OK: GrabBufferLR[0] buffer contains a colorbar\n");
        mmte_testResult(TEST_PASSED);
        result &= Result_e_Success;
    }
    else
    {
        MMTE_TEST_COMMENT("TEST FAIL: GrabBufferLR[0] buffer not properly filled (does not contain colorbar)\n");
        mmte_testResult(TEST_FAILED);
        result &= Result_e_Failure;
    }


    testNum++;

    return (result);
}


/* -----------------------------------------------------------------------
   FUNCTION : ITE_testcolorBarLRGeneric
   PURPOSE  : Test if GrabBufferLR[0] buffer contains colorbar;
   use mmte_testResult() API
   ------------------------------------------------------------------------ */
Result_te
ITE_testcolorBarLRGeneric(void)
{
    char        mess[256];
    t_uint32    float_number = ITE_readPE(PipeState_1_f_FOVX_Byte0);
    float       fov_x = *(( float * ) (&float_number));
    Result_te   result = Result_e_Success;

    snprintf(
    mess,
    sizeof(mess),
    "Test %u :check if GrabBufferLR[0] was filled with colorbar",
    ( unsigned int ) testNum);
    mmte_testNext(mess);

    if (TestColorBarbufferGeneric(&GrabBufferLR[0], COLOR_BAR__DEFAULT_TESTING_POSITION, usecase.BMS_XSize, fov_x) == 0)
    {
        MMTE_TEST_COMMENT("TEST OK: GrabBufferLR[0] buffer contains a colorbar\n");
        mmte_testResult(TEST_PASSED);
        result &= Result_e_Success;
    }
    else
    {
        MMTE_TEST_COMMENT("TEST FAIL: GrabBufferLR[0] buffer not properly filled (does not contain colorbar)\n");
        mmte_testResult(TEST_FAILED);
        result &= Result_e_Failure;
    }


    testNum++;
    return (result);
}


/* -----------------------------------------------------------------------
   FUNCTION : ITE_testcolorBarHR
   PURPOSE  : Test if GrabBufferHR[0] buffer contains colorbar;
   use mmte_testResult() API
   ------------------------------------------------------------------------ */
Result_te
ITE_testcolorBarHR(void)
{
    char        mess[256];
    Result_te   result = Result_e_Success;

    snprintf(
    mess,
    sizeof(mess),
    "Test %u :check if GrabBufferHR[0] was filled with colorbar",
    ( unsigned int ) testNum);  //__NO_WARNING__
    mmte_testNext(mess);

    if (TestColorBarbuffer(&GrabBufferHR[0], COLOR_BAR__DEFAULT_TESTING_POSITION) == 0)
    {
        MMTE_TEST_COMMENT("TEST OK: GrabBufferHR[0] buffer contains a colorbar\n");
        mmte_testResult(TEST_PASSED);
        result &= Result_e_Success;
    }
    else
    {
        MMTE_TEST_COMMENT("TEST FAIL: GrabBufferHR[0] buffer not properly filled (does not contain colorbar)\n");
        mmte_testResult(TEST_FAILED);
        result &= Result_e_Failure;
    }


    testNum++;

    return (result);
}


/* -----------------------------------------------------------------------
   FUNCTION : ITE_testcolorBarHRGeneric
   PURPOSE  : Test if GrabBufferLR[0] buffer contains colorbar;
   use mmte_testResult() API
   ------------------------------------------------------------------------ */
Result_te
ITE_testcolorBarHRGeneric(void)
{
    char        mess[256];
    t_uint32    float_number = ITE_readPE(PipeState_0_f_FOVX_Byte0);
    float       fov_x = *(( float * ) (&float_number));
    Result_te   result = Result_e_Success;

    snprintf(
    mess,
    sizeof(mess),
    "Test %u :check if GrabBufferHR[0] was filled with colorbar",
    ( unsigned int ) testNum);
    mmte_testNext(mess);

    if (TestColorBarbufferGeneric(&GrabBufferHR[0], COLOR_BAR__DEFAULT_TESTING_POSITION, usecase.BMS_XSize, fov_x) == 0)
    {
        MMTE_TEST_COMMENT("TEST OK: GrabBufferHR[0] buffer contains a colorbar\n");
        mmte_testResult(TEST_PASSED);
        result &= Result_e_Success;
    }
    else
    {
        MMTE_TEST_COMMENT("TEST FAIL: GrabBufferHR[0] buffer not properly filled (does not contain colorbar)\n");
        mmte_testResult(TEST_FAILED);
        result &= Result_e_Failure;
    }


    testNum++;

    return (result);
}


/* -----------------------------------------------------------------------
   FUNCTION : ITE_testcolorBarBMS
   PURPOSE  : Test if GrabBufferBMS[0] buffer contains colorbar;
   use mmte_testResult() API
   ------------------------------------------------------------------------ */
Result_te
ITE_testcolorBarBMS(void)
{
    char        mess[256];
    Result_te   result = Result_e_Success;

    snprintf(mess, sizeof(mess), "Test %d :check if GrabBufferBMS[0] was filled with colorbar", ( int ) testNum);   //__NO_WARNING__
    mmte_testNext(mess);

    if (TestColorBarbuffer(&GrabBufferBMS[0], COLOR_BAR__DEFAULT_TESTING_POSITION) == 0)
    {
        MMTE_TEST_COMMENT("TEST OK: GrabBufferBMS[0] buffer contains a colorbar\n");
        mmte_testResult(TEST_PASSED);
        result &= Result_e_Success;
    }
    else
    {
        MMTE_TEST_COMMENT("TEST FAIL: GrabBufferBMS[0] buffer not properly filled (does not contain colorbar)\n");
        mmte_testResult(TEST_FAILED);
        result &= Result_e_Failure;
    }


    testNum++;

    return (result);
}


/* -----------------------------------------------------------------------
   FUNCTION : ITE_testBufferBMS
   PURPOSE  : Test if GrabBufferBMS[0] buffer was filled;
   use samples in buffer and check different from default values
   use mmte_testResult() API
   ------------------------------------------------------------------------ */
void
ITE_testBufferBMS(void)
{
    char    mess[256];

    snprintf(mess, sizeof(mess), "Test %d :check if GrabBufferBMS[0] was filled", ( int ) testNum); //__NO_WARNING__
    mmte_testNext(mess);

    if (ITE_CheckGrabInBuffer(GrabBufferBMS[0].logAddress, GrabBufferBMS[0].buffersizeByte, 0xff) == 1)
    {
        MMTE_TEST_COMMENT("TEST OK: GrabBufferBMS[0] buffer was filled with values different from default value\n");
        PERFORMANCE_RESULT_PASSED();
    }
    else
    {
        MMTE_TEST_COMMENT(
        "TEST FAIL: GrabBufferBMS[0] buffer not properly filled (too many default values in buffer)\n");
        PERFORMANCE_RESULT_FAILED();
    }


    testNum++;
}


/* -----------------------------------------------------------------------
   FUNCTION : ITE_testColorRedHR
   PURPOSE  : Test if GrabBufferHR[0] buffer contains red solid bar;
   use mmte_testResult() API
   ------------------------------------------------------------------------ */

/*void ITE_testSolidBarBMS()
  {
  char mess[256];

  snprintf(mess, sizeof(mess), "Test %d :check if GrabBufferHR[0] was filled with red solid bar",testNum);
  mmte_testNext(mess);

  if (TestColoredbuffer(&GrabBufferHR[0], RED_COLOR_E) == 0)
  {
  LOS_Log("\nTEST OK: GrabBufferHR[0] buffer contains a red solid bar\n");
  mmte_testResult(TEST_PASSED);
  }
  else
  {
  LOS_Log("\nTEST FAIL: GrabBufferHR[0] buffer not properly filled (does not contain red solid bar)\n");
  mmte_testResult(TEST_FAILED);
  }
  testNum++;
  }*/

/* -----------------------------------------------------------------------
   FUNCTION : ITE_testColorRedLR
   PURPOSE  : Test if GrabBufferLR[0] buffer contains red solid bar;
   use mmte_testResult() API
   ------------------------------------------------------------------------ */

/*void ITE_testColorRedLR()
  {
  char mess[256];

  snprintf(mess, sizeof(mess), "Test %d :check if GrabBufferLR[0] was filled with red solid bar",testNum);
  mmte_testNext(mess);

  if (TestColoredbuffer(&GrabBufferLR[0], RED_COLOR_E) == 0)
  {
  LOS_Log("\nTEST OK: GrabBufferLR[0] buffer contains a red solid bar\n");
  mmte_testResult(TEST_PASSED);
  }
  else
  {
  LOS_Log("\nTEST FAIL: GrabBufferLR[0] buffer not properly filled (does not contain red solid bar)\n");
  mmte_testResult(TEST_FAILED);
  }
  testNum++;
  }*/

/* -----------------------------------------------------------------------
   FUNCTION : ITE_testBasicBML
   PURPOSE  : Test BML datapath with raw8 format
   with specific LR HR resolutions: used for perfs measurment
   ------------------------------------------------------------------------ */
int
ITE_testBasicBML(void)
{
    e_resolution        lrresolution;
    e_resolution        hrresolution;
    char                lrresolutionstring[16];
    char                hrresolutionstring[16];
    char                lrformatstring[16];
    char                hrformatstring[16];
    char                BMSresolution[16];
    int ret = EOK;
    enum e_grabFormat   lrgrbformat = GRBFMT_YUV422_RASTER_INTERLEAVED;

    enum e_grabFormat   hrgrbformat = GRBFMT_YUV420_MB_SEMIPLANAR_DECIMATED;

    //Set Trace ON for BML and LR
    TraceGrabLREvent = (t_bool) TRUE;
    TraceBMLEvent = (t_bool) TRUE;

    ITE_dbg_bmsmode("STILL");
    ret = ITE_dbg_setusecase("BMS", BMSresolution, "RAW8");
    if (ret != EOK)
    {
        LOS_Log("ITE_dbg_setusecase gave error in %s and line  %d\n",__FUNCTION__,__LINE__);
        goto END;
    }

    ITE_dbg_bmsout("BAYERSTORE2");

    if (g_InputImageSource == InputImageSource_e_Sensor0)
    {
        ITE_SensorMode(COLORBAR);
    }
    else
    {
        ITE_SetRxTestPattern(RxTestPattern_e_ColourBars);
    }


    // Stream first on LR
    ITE_LR_Prepare(&usecase, INFINITY);
    ITE_LR_Start(INFINITY);
    LOS_Log("\n500msecs stream on LR pipe\n");
    LOS_Sleep(500);

    ITE_LR_Stop();
    ITE_BMS_Prepare(&usecase, INFINITY);
    ITE_BMS_Start(1);
    LOS_Log("\nStream one frame on BMS \n");
    ITE_BMS_Stop();

    lrresolution = usecase.LR_Max_resolution;
    hrresolution = usecase.HR_Still_Max_resolution;

    if (ITE_GiveAspectRatioFromEnum(PIPE_LR, lrresolution, lrgrbformat) ==
        ITE_GiveAspectRatioFromEnum(PIPE_HR, hrresolution, hrgrbformat))
    {
        ITE_GiveStringNameFromEnum(lrresolution, lrresolutionstring);
        ITE_GiveStringNameFromEnum(hrresolution, hrresolutionstring);
        ITE_GiveFormatStringFromEnum(lrgrbformat, lrformatstring);
        ITE_GiveFormatStringFromEnum(hrgrbformat, hrformatstring);

        ret = ITE_dbg_setusecase("LR", lrresolutionstring, lrformatstring);
        if(ret != EOK)
        {
            LOS_Log("ITE_dbg_setusecase gave error in %s and line  %d\n",
                    __FUNCTION__, __LINE__);
            goto END;
        }

        ret = ITE_dbg_setusecase("HR", hrresolutionstring, hrformatstring);
        if(ret != EOK)
        {
            LOS_Log("ITE_dbg_setusecase gave error in %s and line  %d\n",
                    __FUNCTION__, __LINE__);
            goto END;
        }

        // ITE_testBMLdatapathLRHR("RAW8");
        {
            ITE_LR_Prepare(&usecase, INFINITY);
            ITE_HR_Prepare(&usecase, INFINITY);

            // sprintf(pathname,IMAGING_PATH"/ite_nmf/test_results_nreg/datapath_BML_%s",RAW);
            // strcat(g_out_path,pathname);
            // LOS_Log("Current path: >%s< \n",(char *)g_out_path);
            // usecase.BML_input = BAYERLOAD1;
            ITE_dbg_bmlin("BAYERLOAD1");

            // ITE_testStillBMLdatapath();
            {
                // ITE_testStreamBML(&usecase,sizebms);
                {
                    ITE_BML_Prepare(&usecase, INFINITY);
                    ITE_BML_Start(1);

                    // LOS_Log("\nINject one frame on BML \n");
                    // ITE_testcolorBarHR();
                    // ITE_testcolorBarLR();
                    ITE_BML_Stop();
                }
            }

            ITE_dbg_bmlin("BAYERLOAD2");

            // ITE_testStillBMLdatapath();
            {
                // ITE_testStreamBML(&usecase,sizebms);
                {
                    ITE_BML_Prepare(&usecase, INFINITY);
                    ITE_BML_Start(1);

                    //LOS_Log("\nINject one frame on BML \n");
                    //ITE_testcolorBarHR();
                    //ITE_testcolorBarLR();
                    ITE_BML_Stop();
                }
            }
        }
    }

    ITE_BMS_Free();
    ITE_BML_Free();
    ITE_LR_Free();
    ITE_HR_Free();
    END:
    return ret;
}

/* -----------------------------------------------------------------------
   FUNCTION : ITE_testPan_tiltStreaming
   PURPOSE  : testPan_tiltStreamin
   ------------------------------------------------------------------------ */
int
ITE_testPan_tiltStreaming(
enum e_grabFormat   lrgrbformat,
enum e_grabFormat   hrgrbformat,
char                *Pipe)
{
    e_resolution        lrresolution;
    e_resolution        hrresolution;
    char                lrresolutionstring[16];
    char                hrresolutionstring[16];
    char                lrformatstring[16];
    char                hrformatstring[16];
    t_uint16            maxfps;
    int ret =EOK;
    volatile t_uint32   MaxDZ;
    volatile t_uint32   MaxFOVX;
    volatile t_uint32   MinFOVX;
    t_uint32            after,
                        before           = 0;

    maxfps = 15;
    if (0 == strcmp(Pipe, "LR"))
    {
        for (lrresolution = usecase.LR_Min_resolution; lrresolution <= usecase.LR_Max_resolution; lrresolution++)
        {
            ITE_GiveStringNameFromEnum(lrresolution, lrresolutionstring);
            ITE_GiveFormatStringFromEnum(lrgrbformat, lrformatstring);
            ret = ITE_dbg_setusecase("LR", lrresolutionstring, lrformatstring);
            if(ret != EOK)
            {
                LOS_Log("ITE_dbg_setusecase gave error in %s and line  %d\n",__FUNCTION__,__LINE__);
                goto END;
            }


            ITE_LR_Prepare(&usecase, INFINITY);
            ITE_writePE(Zoom_Control_f_SetFOVX_Byte0, 0);
            ITE_LR_Start(INFINITY);
            LOS_Sleep(500);
            MaxFOVX = ITE_readPE(Zoom_Status_f_MaxAvailableFOVXAtCurrentCenter_Byte0);
            MinFOVX = usecase.LR_XSize / 6;
            MaxDZ = (*( volatile float * ) &MaxFOVX) / MinFOVX;
            LOS_Log("DEBUG>>>>>:  MaxFOVX: %f, MinFOVX: %f ,  MaxDZ: %f\n", MaxFOVX, MinFOVX, MaxDZ);
            ITE_LR_Stop();
            ITE_NMF_DZ_Init(( float ) MaxDZ);
            MaxDZ = ITE_readPE(Zoom_Params_f_ZoomRange_Byte0);
            LOS_Log("DEBUG>>>>>:  MaxFOVX: %f, MinFOVX: %f ,  MaxDZ: %f\n", MaxFOVX, MinFOVX, MaxDZ);
            ITE_SetFrameRateControl(maxfps, maxfps);
            ITE_LR_Start(INFINITY);
            LOS_Sleep(500);
            ITE_NMF_Pan_tilt_Test_FullRange("LR", (*( volatile float * ) &MaxDZ));  // MaxDZ
            ITE_LR_Stop();
            ITE_LR_Free();
            after = LOS_GetAllocatedChunk();
            LOS_Log("Not DeAllocated Memory chunk : \t %d - %d = %d\n", after, before, after - before);
        }
    }


    if (0 == strcmp(Pipe, "HR"))
    {
        for (hrresolution = usecase.HR_Min_resolution; hrresolution <= usecase.HR_Video_Max_resolution; hrresolution++)
        {
            ITE_GiveStringNameFromEnum(hrresolution, hrresolutionstring);
            ITE_GiveFormatStringFromEnum(hrgrbformat, hrformatstring);
            ret = ITE_dbg_setusecase("HR", hrresolutionstring, hrformatstring);
            if(ret != EOK)
            {
                LOS_Log("ITE_dbg_setusecase gave error in %s and line  %d\n",__FUNCTION__,__LINE__);
                goto END;
            }


            ITE_HR_Prepare(&usecase, INFINITY);
            ITE_writePE(Zoom_Control_f_SetFOVX_Byte0, 0);
            ITE_HR_Start(INFINITY);
            LOS_Sleep(5000);
            MaxFOVX = ITE_readPE(Zoom_Status_f_MaxAvailableFOVXAtCurrentCenter_Byte0);
            MinFOVX = usecase.HR_XSize / 6;
            MaxDZ = (*( volatile float * ) &MaxFOVX) / MinFOVX;
            LOS_Log("DEBUG>>>>>:  MaxFOVX: %f, MinFOVX: %f ,  MaxDZ: %f\n", MaxFOVX, MinFOVX, MaxDZ);
            ITE_HR_Stop();
            ITE_NMF_DZ_Init(( float ) MaxDZ);
            MaxDZ = ITE_readPE(Zoom_Params_f_ZoomRange_Byte0);
            ITE_SetFrameRateControl(maxfps, maxfps);
            ITE_HR_Start(INFINITY);
            LOS_Sleep(500);

            ITE_NMF_Pan_tilt_Test_FullRange("HR", (*( volatile float * ) &MaxDZ));  // MaxDZ
            ITE_HR_Stop();
            ITE_HR_Free();
            after = LOS_GetAllocatedChunk();
            LOS_Log("Not DeAllocated Memory chunk : \t %d\n", after - before);
        }
    }
    END:
    return ret;
}

