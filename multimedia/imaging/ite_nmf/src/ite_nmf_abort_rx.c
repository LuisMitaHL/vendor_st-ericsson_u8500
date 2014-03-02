/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_set_abort_rx_cmd
   PURPOSE  : To tests funcionality of abort_rx ISP FW module
   ------------------------------------------------------------------------ */
#include <ite_nmf_abort_rx.h>

extern ts_sia_usecase   usecase;

static t_uint32         g_u32_counter_abort_rx_conf = 0;
static t_uint32         g_u32_counter_abort_rx_zoom_streaming = 0;

/*  purpose : set SystemSetup_e_Flag_abortRx_OnStop_Byte0 PE to value specified by HOST
  *  pre-condition : can be done post-boot/run
  *  return-value : none
  *  post-condition :  none
*/
CMD_COMPLETION
C_ite_dbg_set_abort_rx_cmd(
int     a_nb_args,
char    **ap_args)
{
    char    mess[256];

    sprintf(mess, "abort_rx_%d", ( int ) g_u32_counter_abort_rx_conf);
    g_u32_counter_abort_rx_conf++;

    MMTE_TEST_START(mess, "/ite_nmf/test_results_nreg/abort_rx", "abort_rx");

    sprintf(mess, "setting abort-rx to HOST specified value");
    mmte_testNext(mess);

    switch (a_nb_args)
    {
        case 2:
            {
                if(0 == strcasecmp("reset", ap_args[1]))
                {
                    ITE_setAbortRx(0);
                break;
            }
                else if(0 == strcasecmp("set", ap_args[1]))
                {
                    ITE_setAbortRx(1);
                    break;
                }
                else if(0 == strcasecmp("save", ap_args[1]))
                {
                    ITE_setAbortRx(2);
                    break;
                }
                else if(0 == strcasecmp("restore", ap_args[1]))
                {
                    ITE_setAbortRx(3);
                    break;
                }
            }

        default:    // wrong syntax : print on stdout how to use test framework for rgb2yuvcoder
            {
                LOS_Log("syntax : set_abort_rx <reset/set/save/restore> \n");
                break;
            }
    }


    sprintf(mess, "set_abort_rx done\n");
    mmte_testComment(mess);
    MMTE_TEST_PASSED();

    return (0);
}


/*  purpose : Tests zoom streaming for abort-rx for 3 resolution (VGA, HD, FULLHD) on LR and HR
  *  pre-condition : can be done post-boot only
  *  return-value : none
  *  post-condition :  none
*/
CMD_COMPLETION
C_ite_dbg_Test_abort_rx_Zoom_Streaming(
int     a_nb_args,
char    **ap_args)
{

    int ret = EOK;
    char    mess[256];

    sprintf(mess, "abort_rx_zoom_streaming_%d", ( int ) g_u32_counter_abort_rx_zoom_streaming);
    g_u32_counter_abort_rx_zoom_streaming++;

    MMTE_TEST_START(mess, "/ite_nmf/test_results_nreg/abort_rx", "abort_rx");

    sprintf(mess, "doing abort_rx_zoom_streaming testing");
    mmte_testNext(mess);

    switch (a_nb_args)
    {
        case 2:     // test_abort_rx_zoom_streaming <pipe_name>
            {
                ret = ITE_testAbortRxForZoomstreaming(
                GRBFMT_YUV422_RASTER_INTERLEAVED,
                GRBFMT_YUV420_MB_SEMIPLANAR_DECIMATED,
                ap_args[1]);
                if(ret != EOK)
                {
                    CLI_disp_error("Not correct command arguments C_ite_dbg_Test_abort_rx_Zoom_Streaming\n");
                    goto END;
                }
                break;
            }


        default:    // wrong syntax : print on stdout how to use test framework for rgb2yuvcoder
            {
                LOS_Log("syntax : test_abort_rx_zoom_streaming LR/HR\n");
                break;
            }
    }


    sprintf(mess, "abort_rx_zoom_streaming testing done\n");
    mmte_testComment(mess);
    MMTE_TEST_PASSED();
    END:
    return (CMD_COMPLETE);
}


void
ITE_setAbortRx(
t_uint32    u32_flag)
{
    static char u8_Value;
    LOS_Log("\n>> ITE_setAbortRx %d\n", u32_flag);

    switch (u32_flag)
    {
        case 0 : LOS_Log("SET g_FrameParamStatus.e_Flag_abortRx_OnStop \n");
        ITE_writePE(SystemSetup_e_Flag_abortRx_OnStop_Byte0, Flag_e_FALSE);
                 break;
        case 1 : LOS_Log("RESET g_FrameParamStatus.e_Flag_abortRx_OnStop \n");
        ITE_writePE(SystemSetup_e_Flag_abortRx_OnStop_Byte0, Flag_e_TRUE);
                 break;
        case 2 : u8_Value = ITE_readPE(SystemSetup_e_Flag_abortRx_OnStop_Byte0);
                 LOS_Log("SAVE g_FrameParamStatus.e_Flag_abortRx_OnStop \n");
                 break;
        case 3 : LOS_Log("RESTORE g_FrameParamStatus.e_Flag_abortRx_OnStop \n");
                 ITE_writePE(SystemSetup_e_Flag_abortRx_OnStop_Byte0, u8_Value);
                 break;
    }

    LOS_Log("<< ITE_setAbortRx \n");
    return;
}


/* -----------------------------------------------------------------------
   FUNCTION : ITE_testAbortRxForZoomstreaming
   PURPOSE  : Test zoom streaming on LR and HR independantly
   for different output LR HR resolutions
   ------------------------------------------------------------------------ */
int
ITE_testAbortRxForZoomstreaming(
enum e_grabFormat   lrgrbformat,
enum e_grabFormat   hrgrbformat,
char                *Pipe)
{
    e_resolution        lrresolution;
    e_resolution        hrresolution;
    e_resolution        resolutionToTest[] = { VGA,
                        HD,
                        FULLHD };
    char                lrresolutionstring[16];
    char                hrresolutionstring[16];
    char                lrformatstring[16];
    char                hrformatstring[16];
    t_uint16            maxfps;
    volatile t_uint32   MaxDZ;
    volatile t_uint32   MaxFOVX;
    volatile t_uint32   MinFOVX;
    t_uint32            after,
                        before = 0;
    t_uint32            u32_count = 0;
    int ret = EOK;

    LOS_Log("\n>> ITE_testAbortRxForZoomstreaming\n");

    ITE_writePE(SystemSetup_e_Flag_abortRx_OnStop_Byte0, Flag_e_TRUE);

    maxfps = 15;
    if (0 == strcmp(Pipe, "LR"))
    {
        for (u32_count = 0; u32_count < sizeof(resolutionToTest) / sizeof(e_resolution); u32_count++)
        // for (lrresolution = usecase.LR_Min_resolution; lrresolution <= usecase.LR_Max_resolution; lrresolution++)
        {
            lrresolution = resolutionToTest[u32_count];
            LOS_Log("testing abort-rx in zoom streaming for LR pipe : resolution = %u\n", lrresolution);
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
        for (u32_count = 0; u32_count < sizeof(resolutionToTest) / sizeof(e_resolution); u32_count++)
        //for (hrresolution = usecase.HR_Min_resolution; hrresolution <= usecase.HR_Video_Max_resolution; hrresolution++)
        {
            hrresolution = resolutionToTest[u32_count];
            LOS_Log("testing abort-rx in zoom streaming for HR pipe : resolution = %u\n", hrresolution);
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


    LOS_Log("<< ITE_testAbortRxForZoomstreaming\n");
    END:
    return ret;
}

