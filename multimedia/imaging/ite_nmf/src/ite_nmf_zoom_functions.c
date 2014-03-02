/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include "cli.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ite_nmf_zoom_functions.h"
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
#include "ite_display.h"
#include "ite_event.h"
#include "ite_pageelements.h"
#include "ite_grab.h"
#include "ite_vpip.h"
#include "ite_sia_buffer.h"
#include "ite_host2sensor.h"
#include "ite_sensorinfo.h"

#include <test/api/test.h>
#include <cm/inc/cm_macros.h>

#include <los/api/los_api.h>
#include "ite_main.h"
#include "sia.h"
#include "algotypedefs.h"
#include "algointerface.h"
#include "ite_debug.h"

#define ISP_REGION_IO   0xE0000000
volatile t_uint32 g_zoom_chgover = 0;

/* function quick desciption:
 *
 *
 *
 */
extern ts_sia_usecase       usecase;
extern ts_bmlinfo           BmlInfo;

#ifdef TIMELOG
extern ts_sia_eventtimelog  eventtimelog;
#endif
t_uint8                     flagDZdenied = 0;
int g_Store_IMAGES  ;
int g_LRcount, g_HRcount ;

float
ITE_NMF_returnMaxDZforCurrentUsecase(void)
{
    float   MaxDZ;

    switch (usecase.HR_resolution)
    {
        case (VGA):
            MaxDZ = 20.61;
            break;

        case (VGAPLUS):
            MaxDZ = 12.33;
            break;

        case (HD):
            MaxDZ = 5.28;
            break;

        case (FULLHD):
            MaxDZ = 2.36;
            break;

        case (SUBQCIF):
        case (SUBQCIFPLUS):
        case (QCIF):
        case (QCIFPLUS):
        case (CIF):
        case (CIFPLUS):
        case (QQVGA):
        case (QVGA):
        case (QVGAPLUS):
        case (NTSC):
        case (PAL):
        case (SXGA):
        case (SVGA):
        case (WVGA):
        case (QHD):
        case (XGA):
        case (UXGA):
        case (TWOMEGA):
        case (THREEMEGA):
        case (FOURMEGA):
        case (FIVEMEGA):
        case (FIVEMEGA_W):
        case (EIGHTMEGA):
        case (EIGHTMEGA_W):
            MaxDZ = 6;
            break;

        default:
            MaxDZ = 1;
            LOS_Log_Err("Resolution MaxDZ not found\n");
            break;
    }


    return (MaxDZ);
}


void
ITE_NMF_DZ_Init(
volatile float  MaxDZ)
{
    usecase.MaxDZ = MaxDZ;
    ITE_writePE(Zoom_Params_f_ZoomRange_Byte0, *( volatile t_uint32 * ) &MaxDZ);
    ITE_writePE(Zoom_Control_s16_CenterOffsetX_Byte0, 0);
    ITE_writePE(Zoom_Control_s16_CenterOffsetY_Byte0, 0);
}


t_uint8
ITE_NMF_DZ_SendZoomCmd(
ZoomCommand_te  ZoomCmd)
{
    t_uint8 DZCmdCount;

    DZCmdCount = ITE_readPE(Zoom_CommandStatus_u8_CommandCount_Byte0);
    DZCmdCount = (DZCmdCount + 1) % 256;
    ITE_writePE(Zoom_CommandControl_e_ZoomCommand_Byte0, ZoomCmd);
    ITE_writePE(Zoom_CommandControl_u8_CommandCount_Byte0, DZCmdCount);
    return (DZCmdCount);
}


void
ITE_NMF_DZ_CheckCmdAbsorbed(
t_uint8 DZCmdCount)
{
    // wait command absorbed
    do
    {
        LOS_Sleep(5);
    } while (ITE_readPE(Zoom_CommandStatus_u8_CommandCount_Byte0) != DZCmdCount);
}


int
ITE_NMF_DZ_WaitZoomComplete(void)
{
    t_uint16            ChgOver = 0;
    union u_ITE_Event   event;



    do
    {
    	   event = ITE_GetZoomEvent();
        ITE_traceISPCTLevent(event);
        if (event.ispctlInfo.info_id == ITE_EVT_ISPCTL_ERROR)
        {
            break;
        }


        if (event.ispctlInfo.info_id == ISP_ZOOM_CONFIG_REPROGRAM_REQUIRED)
        {
            ChgOver++;
            LOS_Log("ChangeOver appear :%d \n", ChgOver);
            g_zoom_chgover++;
        }


        if (event.ispctlInfo.info_id == ISP_ZOOM_CONFIG_REQUEST_DENIED)
        {
            LOS_Log("Warning: Zoom Request is denied!\n");
            flagDZdenied = 1;
        }


        if (event.ispctlInfo.info_id == ISP_ZOOM_SET_OUT_OF_RANGE)
        {
            LOS_Log("Warning: Zoom Request is Out Of Range!\n");
        }
    } while (!(event.ispctlInfo.info_id == ISP_ZOOM_STEP_COMPLETE));


    if (event.type == ITE_EVT_ISPCTL_ERROR)
    {
        return (event.ispctlError.error_id);
    }
    else
    {
        // If ChgOver we must wait for ISP Streaming Notification
        if (ChgOver)
        {
            do
            {
                event = ITE_WaitEvent(ITE_EVT_ISPCTL_EVENTS);
                ITE_traceISPCTLevent(event);
                if (event.type == ITE_EVT_ISPCTL_ERROR)
                {
                    break;
                }
            } while (!((event.type == ITE_EVT_ISPCTL_INFO) && (event.ispctlInfo.info_id == ISP_STREAMING)));

            if (event.type == ITE_EVT_ISPCTL_ERROR)
            {
                return (event.ispctlError.error_id);
            }
            else
            {
                return (0);
            }
        }
        else
        {
            return (0);
        }
    }
}


int
ITE_NMF_DZ_SetFovX(
volatile float  FOVX)
{
    int     result;
    t_uint8 DZCmdCount;

    ITE_writePE(Zoom_Control_f_SetFOVX_Byte0, *( volatile t_uint32 * ) &FOVX);
    DZCmdCount = ITE_NMF_DZ_SendZoomCmd(ZoomCommand_e_SetFOV);
    ITE_NMF_DZ_CheckCmdAbsorbed(DZCmdCount);

    result = ITE_NMF_DZ_WaitZoomComplete();

    return (result);
}


int
ITE_NMF_DZ_SetCenter(
t_sint16    Xoffset,
t_sint16    Yoffset)
{
    int     result;
    t_uint8 DZCmdCount;

    ITE_writePE(Zoom_Control_s16_CenterOffsetX_Byte0, Xoffset);
    ITE_writePE(Zoom_Control_s16_CenterOffsetY_Byte0, Yoffset);
    DZCmdCount = ITE_NMF_DZ_SendZoomCmd(ZoomCommand_e_SetCenter);
    ITE_NMF_DZ_CheckCmdAbsorbed(DZCmdCount);
    result = ITE_NMF_DZ_WaitZoomComplete();
    return (result);
}


void
ITE_NMF_DZ_ZoomSet(
float   DZ,
t_bool  toggle_bit)
{
    volatile t_uint32   MaxFOVX;
    volatile t_uint32   MaxDZ;
    volatile float      FOVX;
    t_uint8             DZCmdCount;

    // Check if DZ < or = MaxDZ
    MaxDZ = ITE_readPE(Zoom_Params_f_ZoomRange_Byte0);

    if (DZ > (*( volatile float * ) &MaxDZ))
    {
        LOS_Log("DZ Value greater than MaxDZ factor\n");
    }
    else
    {
        usecase.CurrentDZ = DZ;
        MaxFOVX = ITE_readPE(Zoom_Status_f_MaxAvailableFOVXAtCurrentCenter_Byte0);

        if (DZ == 1)
        {
            FOVX = 0;   // Full field Of view
        }
        else
        {
            FOVX = (*( volatile float * ) &MaxFOVX) / DZ;
        }

        LOS_Log("MaxFOVx = %f Set FOVX %f\n", (*( volatile float * ) &MaxFOVX), FOVX);

        ITE_writePE(Zoom_Control_f_SetFOVX_Byte0, *( volatile t_uint32 * ) &FOVX);

        if (Flag_e_TRUE == toggle_bit)
        {
            DZCmdCount = ITE_NMF_DZ_SendZoomCmd(ZoomCommand_e_SetFOV);
            ITE_NMF_DZ_CheckCmdAbsorbed(DZCmdCount);
            ITE_NMF_DZ_WaitZoomComplete();
        }
        else
        {
            ITE_writePE(Zoom_CommandControl_e_ZoomCommand_Byte0, ZoomCommand_e_SetFOV);
        }
    }
}


void
ITE_NMF_DZ_Set(
float   DZ)
{
    volatile t_uint32   MaxFOVX;
    volatile t_uint32   MaxDZ;
    volatile float      FOVX;

    // Check if DZ < or = MaxDZ
    MaxDZ = ITE_readPE(Zoom_Params_f_ZoomRange_Byte0);
    if (DZ > (*( volatile float * ) &MaxDZ))
    {
        LOS_Log("DZ Value greater than MaxDZ factor\n");
    }
    else
    {
        usecase.CurrentDZ = DZ;
        MaxFOVX = ITE_readPE(Zoom_Status_f_MaxAvailableFOVXAtCurrentCenter_Byte0);
        if (DZ == 1)
        {
            FOVX = 0;   // Full field Of view
        }
        else
        {
            FOVX = (*( volatile float * ) &MaxFOVX) / DZ;
        }


        LOS_Log("MaxFOVx = %f Set FOVX %f\n", (*( volatile float * ) &MaxFOVX), FOVX);
        ITE_NMF_DZ_SetFovX(FOVX);
    }
}


TAlgoError
ITE_NMF_Pan_tilt_Test(
float   DZ)
{
    TAlgoError          ret = EErrorNone;
    char                mess[256];

    t_sint16            i,
                        j;

    volatile t_uint16   MaxXoffset,
                        MaxYoffset;

    MaxXoffset = ITE_readPE(Zoom_Status_u16_MaximumAbsoluteXCenterShift_Byte0);

    MaxYoffset = ITE_readPE(Zoom_Status_u16_MaximumAbsoluteYCenterShift_Byte0);

    LOS_Log("Max  absolute shift in center for DZ  %f: MaxXoffset: %d,  MaxYoffset: %d \n", DZ, MaxXoffset, MaxYoffset);
    snprintf(
    mess,
    sizeof(mess),
    "Max  absolute shift in center for DZ  %f: MaxXoffset: %d,  MaxYoffset: %d \n",
    DZ,
    MaxXoffset,
    MaxYoffset);        //__NO_WARNING__
    mmte_testComment(mess);

    for (i = (-MaxXoffset); i <= MaxXoffset; i += (1 + (MaxXoffset / 5)))
    {
        for (j = (-MaxYoffset); j <= MaxYoffset; j += (1 + (MaxYoffset / 5)))
        {
            LOS_Log("Pan_tilt Test for Xoffset: %d,  Yoffset: %d \n", i, j);
            snprintf(mess, sizeof(mess), "Pan-Tilt Test  :  Xoffset: %d,  Yoffset: %d  ", i, j);    //__NO_WARNING__
            mmte_testNext(mess);
            ret = ITE_NMF_DZ_SetCenter(i, j);
            if (ret == 0)
            {
                mmte_testResult(TEST_PASSED);
                if (flagDZdenied == 1)
                {
                    snprintf(
                    mess,
                    sizeof(mess),
                    "Pan-Tilt Xofset: %d, Yoffset: %d cannot be absorbed. Set center Denied by ISP",
                    i,
                    j);
                    mmte_testComment(mess);
                }
            }
            else
            {
                mmte_testResult(TEST_FAILED);
            }
        }
    }


    return (ret);
}


TAlgoError
ITE_NMF_DZ_Test(
tps_siapicturebuffer    p_refbuffer,
tps_siapicturebuffer    p_buffer,
float                   DZref,
float                   DZ)
{
    TAlgoMetricParams   aMetricParams;
    TAlgoImageParams    aImageParams;
    TReal64             zoomfactor = 1;
    TAlgoError          ret = EErrorNone;
    volatile TReal64    FOVX,
                        RefFOVX;
    volatile t_uint32   MaxFOVX;
    t_uint8             invert;

    MaxFOVX = ITE_readPE(Zoom_Status_f_MaxAvailableFOVXAtCurrentCenter_Byte0);
    FOVX = (*( volatile float * ) &MaxFOVX) / DZ;
    RefFOVX = (*( volatile float * ) &MaxFOVX) / DZref;

    aMetricParams.iErrorTolerance.iErrorToleranceR = 5;
    aMetricParams.iErrorTolerance.iErrorToleranceG = 5;
    aMetricParams.iErrorTolerance.iErrorToleranceB = 5;
    aMetricParams.iType = ETypeZoom;
    aMetricParams.iTestCoordinates.iStartIndexX = 0;
    aMetricParams.iTestCoordinates.iStartIndexY = 0;
    aMetricParams.iTestCoordinates.iPixelsToGrabX = p_refbuffer->framesizeX;
    aMetricParams.iTestCoordinates.iPixelsToGrabY = p_refbuffer->framesizeY;
    zoomfactor = ((TReal64) RefFOVX / FOVX);
    invert = 0;
    if (zoomfactor < 1)
    {
        zoomfactor = ((TReal64) FOVX / RefFOVX);
        invert = 1;
    }


    aMetricParams.iParams = &zoomfactor;

    aImageParams.iImageWidth = p_refbuffer->framesizeX;
    aImageParams.iImageHeight = p_refbuffer->framesizeY;
    aImageParams.iImageStride = ITE_NMF_ComputeStride(p_refbuffer);
    aImageParams.iImageHeaderLength = 0;
    aImageParams.iImageFormat = ITE_NMF_GiveMetricFormat(p_refbuffer);

    // <Hem> Below 2 defects are resolved by below 3 source code lines:-
    // [FIDO #408250]                 ValidationMetricInterfaceWithBuffers() of ITE crashes for VGA zoom  bml use case.
    // [FIDO #360449]                 Several DigiZoom Tests as part of Non Regression Test Suite are failing
    aImageParams.iImageNbStatusLine = 0;
    aImageParams.iImagePixelOrder = 0;
    aImageParams.iImageSliceHeight = aImageParams.iImageHeight;

    // aImageParams.iBitMaxR = 255;
    // aImageParams.iBitMaxG = 255;
    // aImageParams.iBitMaxB = 255;
    // aImageParams.iParams = ;
    // <Hem> Uncomment below, as zoom metric is not crashing now
    if (invert == 0)
    {
        ret = ValidationMetricInterfaceWithBuffers(
            &aImageParams,
            &aImageParams,
            ( void * ) (p_refbuffer->logAddress),
            ( void * ) (p_buffer->logAddress),
            &aMetricParams);
    }
    else
    {
        ret = ValidationMetricInterfaceWithBuffers(
            &aImageParams,
            &aImageParams,
            ( void * ) (p_buffer->logAddress),
            ( void * ) (p_refbuffer->logAddress),
            &aMetricParams);
    }


    LOS_Log("zoom test result %d \n", ret);
    return (ret);
}


TAlgoError
ITE_NMF_ZoomTest(
char    *pipe,
float   DZ)
{
    float       latestDZ;
    TAlgoError  ret = EErrorNone,ret_LR =EErrorNone,ret_HR = EErrorNone ;

    volatile int    woi_x=0,woi_y=0,op_x=0,op_y=0;
    char            filenameHR[250];
    char            filenameLR[250];


    ITE_NMF_StoreRefBuffer(pipe);

    latestDZ = usecase.CurrentDZ;
    ITE_NMF_DZ_Set(DZ);
    LOS_Sleep(500);

    if (0 == strcmp(pipe, "LR"))
    {
        ret = ITE_NMF_DZ_Test(&(GrabBufferLR[1]), &(GrabBufferLR[0]), latestDZ, DZ);

        if (g_Store_IMAGES)
        {

            woi_x =ITE_readPE(Zoom_Status_LLA_u16_Ex_woi_resX_Byte0);
            woi_y =ITE_readPE(Zoom_Status_LLA_u16_Ex_woi_resY_Byte0);
            op_x  =ITE_readPE(Zoom_Status_LLA_u16_Ex_output_resX_Byte0);
            op_y  =ITE_readPE(Zoom_Status_LLA_u16_Ex_output_resY_Byte0);

            sprintf(filenameLR,IMAGING_PATH "/LR_%d_%dx%d_%dx%d", g_LRcount++,woi_x,woi_y,op_x,op_y);
            ITE_StoreinBMPFile(filenameLR, &(GrabBufferLR[0]));
        }
    }


    if (0 == strcmp(pipe, "HR"))
    {
        ret = ITE_NMF_DZ_Test(&(GrabBufferHR[1]), &(GrabBufferHR[0]), latestDZ, DZ);
        if (g_Store_IMAGES)
        {

            woi_x =ITE_readPE(Zoom_Status_LLA_u16_Ex_woi_resX_Byte0);
            woi_y =ITE_readPE(Zoom_Status_LLA_u16_Ex_woi_resY_Byte0);
            op_x  =ITE_readPE(Zoom_Status_LLA_u16_Ex_output_resX_Byte0);
            op_y  =ITE_readPE(Zoom_Status_LLA_u16_Ex_output_resY_Byte0);

            sprintf(filenameHR,IMAGING_PATH "/HR_%d_%dx%d_%dx%d", g_HRcount++,woi_x,woi_y,op_x,op_y);
            ITE_StoreinBMPFile(filenameHR, &(GrabBufferHR[0]));
        }
    }

    if (0 == strcmp(pipe, "LRHR"))
    {
        ret_LR = ITE_NMF_DZ_Test(&(GrabBufferLR[1]), &(GrabBufferLR[0]), latestDZ, DZ);
        ret_HR = ITE_NMF_DZ_Test(&(GrabBufferHR[1]), &(GrabBufferHR[0]), latestDZ, DZ);
        if (g_Store_IMAGES)
        {

            woi_x =ITE_readPE(Zoom_Status_LLA_u16_Ex_woi_resX_Byte0);
            woi_y =ITE_readPE(Zoom_Status_LLA_u16_Ex_woi_resY_Byte0);
            op_x  =ITE_readPE(Zoom_Status_LLA_u16_Ex_output_resX_Byte0);
            op_y  =ITE_readPE(Zoom_Status_LLA_u16_Ex_output_resY_Byte0);

            sprintf(filenameHR,IMAGING_PATH "/HR_%d_%dx%d_%dx%d", g_HRcount++,woi_x,woi_y,op_x,op_y);
            ITE_StoreinBMPFile(filenameHR, &(GrabBufferHR[0]));

            sprintf(filenameLR,IMAGING_PATH "/LR_%d_%dx%d_%dx%d", g_LRcount++,woi_x,woi_y,op_x,op_y);
            ITE_StoreinBMPFile(filenameLR, &(GrabBufferLR[0]));
        }
        if((ret_LR && ret_HR)== EErrorNone)
        {
            ret = EErrorNone;
        }
        else
        {
            if (ret_LR != EErrorNone)
            {
                ret = ret_LR;
            }
            else
            {
                ret = ret_HR;
            }
        }
    }

    return (ret);
}


TAlgoError
ITE_NMF_VIDEOZoomTest(
char    *pipe,
float   DZ)
{
    float       latestDZ;
    TAlgoError  ret = EErrorNone;

    ITE_NMF_StoreRefBuffer(pipe);

    latestDZ = usecase.CurrentDZ;
    ITE_NMF_DZ_Set(DZ);
    LOS_Sleep(1000);

    //if (eventtimelog.grabLRerror.index != 0)
    //  while(1);
    // ITE_LR_Stop();
    // ITE_StoreCurrentFOVX();
    ITE_BMS_Prepare(&usecase, INFINITY);
    ITE_BMS_Start(1);
    LOS_Log("\nStream one frame on BMS \n");
    ITE_BMS_Stop();
    ITE_StoreCurrentFOVX();
    LOS_Sleep(1000);
    ITE_HR_Stop();
    LOS_Sleep(1000);
    ITE_BML_Prepare(&usecase, INFINITY);
    ITE_BML_Start(1);
    LOS_Log("\nINject one frame on BML \n");
    LOS_Sleep(1000);
    ITE_BML_Stop();

    if (0 == strcmp(pipe, "LR"))
    {
        ret = ITE_NMF_DZ_Test(&(GrabBufferLR[1]), &(GrabBufferLR[0]), latestDZ, DZ);
    }


    if (0 == strcmp(pipe, "HR"))
    {
        ret = ITE_NMF_DZ_Test(&(GrabBufferHR[1]), &(GrabBufferHR[0]), latestDZ, DZ);
    }


    ITE_HR_Free();
    ITE_HR_Prepare(&usecase, INFINITY);
    ITE_HR_Start(INFINITY);

    return (ret);
}


TAlgoError
ITE_NMF_STILLZoomTest(
char    *pipe,
float   DZ, t_uint32 stripe_count)
{
    volatile float      latestDZ;
    TAlgoError          ret = EErrorNone;
    volatile t_uint32   MaxFOVX;
    volatile float      FOVX;

    ITE_NMF_StoreRefBuffer(pipe);
    MaxFOVX = ITE_readPE(Zoom_Status_f_MaxAvailableFOVXAtCurrentCenter_Byte0);
    if (BmlInfo.Previous_f_SetFOVX == 0)
    {
        latestDZ = 1;
    }
    else
    {
        latestDZ = (*( volatile float * ) &MaxFOVX) / (*( volatile float * ) &(BmlInfo.Previous_f_SetFOVX));
    }


    FOVX = (*( volatile float * ) &MaxFOVX) / DZ;
    LOS_Log("MaxFOVx = %f Set FOVX %f\n", (*( volatile float * ) &MaxFOVX), FOVX);
    if (DZ == 1)
    {
        BmlInfo.Previous_f_SetFOVX = 0; // Full FOV
    }
    else
    {
        BmlInfo.Previous_f_SetFOVX = *( volatile t_uint32 * ) &FOVX;
    }

    if(stripe_count > 0)
    {
          ITE_BML_stripe_Start(1,stripe_count);
    }
    else
    {
    ITE_BML_Start(1);
    }

    LOS_Log("\nINject one frame on BML \n");
    LOS_Sleep(100);

    if (0 == strcmp(pipe, "LR"))
    {
        ret = ITE_NMF_DZ_Test(&(GrabBufferLR[1]), &(GrabBufferLR[0]), latestDZ, DZ);
    }


    if (0 == strcmp(pipe, "HR"))
    {
        ret = ITE_NMF_DZ_Test(&(GrabBufferHR[1]), &(GrabBufferHR[0]), latestDZ, DZ);
    }


    ITE_BML_Stop();
    return (ret);
}


TAlgoError
ITE_NMF_ZoomTest_FullRange(
char    *pipe,
float   MaxDZ)
{
    t_sint16    i;
    t_uint16    zoomstep = 10;
    t_uint16    maxstep = zoomstep;
    float       DZ;
    char        testComment[200];
    char        sensorName[16];
    char        formatName[16];
    char        resolutionName[16];
    char        pathname[200];
    char        mess[256];
    TAlgoError  ret = EErrorNone;
    t_uint32    Num = 0;

    if (usecase.sensor == 0)
    {
        sprintf(sensorName, "Cam0");
    }
    else
    {
        sprintf(sensorName, "Cam1");
    }


    ITE_GiveStringFormat(pipe, formatName);
    if (0 == strcmp(pipe, "LR"))
    {
        ITE_GiveStringNameFromEnum(usecase.LR_resolution, resolutionName);
        LOS_Log(">>>>>>>>>>>>>>>Zoom Test: Pipe LR, Resolution: %s <<<<<<<<<<<<<<<\n", resolutionName);
    }


    if (0 == strcmp(pipe, "HR"))
    {
        ITE_GiveStringNameFromEnum(usecase.HR_resolution, resolutionName);
        LOS_Log(">>>>>>>>>>>>>>>Zoom Test: Pipe HR, Resolution: %s <<<<<<<<<<<<<<<\n", resolutionName);
    }


    sprintf(testComment, "%s_test_nreg_%s_DZ_%s_%s", sensorName, pipe, formatName, resolutionName);

    //perform a "cd" to destination repository for test log
    sprintf(pathname, IMAGING_PATH "/ite_nmf/test_results_nreg/DZ_STREAMING");
    strcat(g_out_path, pathname);
    LOS_Log("Current path: >%s< \n", ( char * ) g_out_path);
    init_test_results_nreg(g_out_path);
    mmte_testStart(testComment, " test DZ all step", g_out_path);
    snprintf(mess, sizeof(mess), "MaxDZ %f for this usecase", MaxDZ);
    mmte_testComment(mess);

    for (i = 0; i <= zoomstep; i++)
    {
        DZ = 1 + (i * ((MaxDZ - 1) / zoomstep));
        LOS_Log("ZoomTest for DZ = %f\n", DZ);
        snprintf(mess, sizeof(mess), "Test %d :check DZ Value %f ", ( int ) Num, DZ);   //__NO_WARNING__
        mmte_testNext(mess);
        ret = ITE_NMF_ZoomTest(pipe, DZ);
        if (ret == 0)
        {
            mmte_testResult(TEST_PASSED);
            if (flagDZdenied == 1)
            {
                snprintf(mess, sizeof(mess), "DZ Step %f cannot be absorbed DZ Denied by ISP", DZ);
                mmte_testComment(mess);
            }
        }
        else
        {
            if (flagDZdenied == 1)
            {
                snprintf(mess, sizeof(mess), "DZ Step %f cannot be absorbed DZ Denied by ISP", DZ);
                mmte_testComment(mess);
                mmte_testResult(TEST_PASSED);
                maxstep = i;
                break;
            }
            else
            {
                mmte_testResult(TEST_FAILED);
            }
        }


        flagDZdenied = 0;
        Num++;
        LOS_Sleep(500);
    }


    for (i = maxstep - 1; i >= 0; i--)
    {
        DZ = 1 + (i * ((MaxDZ - 1) / zoomstep));
        LOS_Log("ZoomTest for DZ = %f\n", DZ);
        snprintf(mess, sizeof(mess), "Test %d :check DZ Value %f ", ( int ) Num, DZ);   //__NO_WARNING__
        mmte_testNext(mess);
        ret = ITE_NMF_ZoomTest(pipe, DZ);
        if (ret == 0)
        {
            mmte_testResult(TEST_PASSED);
            if (flagDZdenied == 1)
            {
                snprintf(mess, sizeof(mess), "DZ Step %f cannot be absorbed DZ Denied by ISP", DZ);
                mmte_testComment(mess);
            }
        }
        else
        {
            if (flagDZdenied == 1)
            {
                snprintf(mess, sizeof(mess), "DZ Step %f cannot be absorbed DZ Denied by ISP", DZ);
                mmte_testComment(mess);
                mmte_testResult(TEST_PASSED);

                //break;
            }
            else
            {
                mmte_testResult(TEST_FAILED);
            }
        }


        flagDZdenied = 0;
        Num++;
        LOS_Sleep(500);
    }


    mmte_testEnd();

    //clean  g_out_path
    memset(g_out_path, 0, KlogDirectoryLentgh * sizeof(char));
    return (ret);   //__NO_WARNING__
}


TAlgoError
ITE_NMF_VIDEOZoomTest_FullRange(
char    *pipe,
float   MaxDZ)
{
    t_sint16    i;
    t_uint16    zoomstep = 10;
    t_uint16    maxstep = zoomstep;
    float       DZ;
    char        testComment[200];
    char        sensorName[16];
    char        formatName[16];
    char        resolutionName[16];
    char        pathname[200];
    char        mess[256];
    TAlgoError  ret = EErrorNone;
    t_uint32    Num = 0;

    if (usecase.sensor == 0)
    {
        sprintf(sensorName, "Cam0");
    }
    else
    {
        sprintf(sensorName, "Cam1");
    }


    ITE_GiveStringFormat(pipe, formatName);
    if (0 == strcmp(pipe, "LR"))
    {
        ITE_GiveStringNameFromEnum(usecase.LR_resolution, resolutionName);
    }


    if (0 == strcmp(pipe, "HR"))
    {
        ITE_GiveStringNameFromEnum(usecase.HR_resolution, resolutionName);
    }


    sprintf(testComment, "%s_test_nreg_%s_DZ_VIDEO_%s_%s", sensorName, pipe, formatName, resolutionName);

    //perform a "cd" to destination repository for test log
    sprintf(pathname, IMAGING_PATH "/ite_nmf/test_results_nreg/DZ_VIDEOBML");
    strcat(g_out_path, pathname);
    LOS_Log("Current path: >%s< \n", ( char * ) g_out_path);
    init_test_results_nreg(g_out_path);
    mmte_testStart(testComment, " test DZ all step", g_out_path);

    for (i = 0; i < zoomstep; i++)
    {
        DZ = 1 + (i * ((MaxDZ - 1) / zoomstep));
        LOS_Log("ZoomTest for DZ = %f\n", DZ);
        snprintf(mess, sizeof(mess), "Test %d :check DZ Value %f ", ( int ) Num, DZ);   //__NO_WARNING__
        mmte_testNext(mess);
        ret = ITE_NMF_VIDEOZoomTest(pipe, DZ);
        if (ret == 0)
        {
            mmte_testResult(TEST_PASSED);
        }
        else
        {
            mmte_testResult(TEST_FAILED);
        }


        if (flagDZdenied == 1)
        {
            snprintf(mess, sizeof(mess), "DZ Step %f cannot be absorbed DZ Denied by ISP", DZ);
            mmte_testComment(mess);
            maxstep = i;
            break;
        };
        flagDZdenied = 0;
        Num++;
        LOS_Sleep(500);
    }


    for (i = maxstep - 1; i >= 0; i--)
    {
        DZ = 1 + (i * ((MaxDZ - 1) / zoomstep));
        LOS_Log("ZoomTest for DZ = %f\n", DZ);
        snprintf(mess, sizeof(mess), "Test %d :check DZ Value %f ", ( int ) Num, DZ);   //__NO_WARNING__
        mmte_testNext(mess);
        LOS_Log("ZoomTest for DZ = %f\n", DZ);
        ret = ITE_NMF_VIDEOZoomTest(pipe, DZ);
        if (ret == 0)
        {
            mmte_testResult(TEST_PASSED);
        }
        else
        {
            mmte_testResult(TEST_FAILED);
        }


        if (flagDZdenied == 1)
        {
            snprintf(mess, sizeof(mess), "DZ Step %f cannot be absorbed DZ Denied by ISP", DZ);
            mmte_testComment(mess);

            //break;
        };
        flagDZdenied = 0;
        Num++;
        LOS_Sleep(500);
    }


    mmte_testEnd();

    //clean  g_out_path
    memset(g_out_path, 0, KlogDirectoryLentgh * sizeof(char));
    return (ret);       //__NO_WARNING__
}


void
ITE_NMF_StoreRefBuffer(
char    *pipe)
{
    if (0 == strcmp(pipe, "LR"))
    {
        ITE_createSiaPictureBuffer(
        &(GrabBufferLR[1]),
        usecase.LR_XSize,
        usecase.LR_YSize,
        usecase.LR_GrbFormat,
        BUF_LR);
        ITE_DuplicateBuffer(&(GrabBufferLR[0]), &(GrabBufferLR[1]));
    }


    if (0 == strcmp(pipe, "HR"))
    {
        ITE_createSiaPictureBuffer(
        &(GrabBufferHR[1]),
        usecase.HR_XSize,
        usecase.HR_YSize,
        usecase.HR_GrbFormat,
        BUF_HR);
        ITE_DuplicateBuffer(&(GrabBufferHR[0]), &(GrabBufferHR[1]));
    }


    if (0 == strcmp(pipe, "BMS"))
    {
        ITE_createSiaPictureBuffer(
        &(GrabBufferBMS[1]),
        usecase.BMS_XSize,
        (usecase.BMS_YSize) + (GrabBufferBMS[0].nonactivelines),
        usecase.BMS_GrbFormat,
        BUF_BMS);
        ITE_DuplicateBuffer(&(GrabBufferBMS[0]), &(GrabBufferBMS[1]));
    }
    if (0 == strcmp(pipe, "LRHR"))
    {
        ITE_createSiaPictureBuffer(
        &(GrabBufferLR[1]),
        usecase.LR_XSize,
        usecase.LR_YSize,
        usecase.LR_GrbFormat,
        BUF_LR);
        ITE_DuplicateBuffer(&(GrabBufferLR[0]), &(GrabBufferLR[1]));

        ITE_createSiaPictureBuffer(
        &(GrabBufferHR[1]),
        usecase.HR_XSize,
        usecase.HR_YSize,
        usecase.HR_GrbFormat,
        BUF_HR);
        ITE_DuplicateBuffer(&(GrabBufferHR[0]), &(GrabBufferHR[1]));

    }
}


void
ITE_SetFOVX_forBMLStill(
volatile float  DZ)
{
    volatile t_uint32   MaxFOVX;
    volatile t_uint32   MaxDZ;
    volatile float      FOVX;

    // Check if DZ < or = MaxDZ
    MaxDZ = ITE_readPE(Zoom_Params_f_ZoomRange_Byte0);
    if (DZ > (*( volatile float * ) &MaxDZ))
    {
        LOS_Log("DZ Value greater than MaxDZ factor\n");
    }
    else
    {
        usecase.CurrentDZ = DZ;
        MaxFOVX = ITE_readPE(Zoom_Status_f_MaxAvailableFOVXAtCurrentCenter_Byte0);
        if (DZ == 1)
        {
            FOVX = 0;   // Full field Of view
        }
        else
        {
            FOVX = (*( volatile float * ) &MaxFOVX) / DZ;
        }


        LOS_Log("MaxFOVx = %f Set FOVX %f\n", (*( volatile float * ) &MaxFOVX), FOVX);
    }
}


CMD_COMPLETION
C_ite_dbg_storebuff_cmd(
int     a_nb_args,
char    **ap_args)
{
    if (ap_args[1] == NULL)
    {
        ap_args[1] = "";
    }


    if (0 == strcmp(ap_args[1], "help"))
    {
        CLI_disp_error("syntax: storebuff <LR/HR/BMS>\n");
    }
    else
    {
        if (a_nb_args == 2)
        {
            ITE_NMF_StoreRefBuffer(ap_args[1]);
        }
        else
        {
            CLI_disp_error("Not correct command arguments\n");
        }
    }


    return (CMD_COMPLETE);
}


CMD_COMPLETION
C_ite_dbg_zoomtest_cmd(
int     a_nb_args,
char    **ap_args)
{
    if (ap_args[1] == NULL)
    {
        ap_args[1] = "";
    }


    if (0 == strcmp(ap_args[1], "help"))
    {
        CLI_disp_error("syntax:  zoomtest <LR/HR/BMS> <DZ>\n");
    }
    else
    {
        if (a_nb_args == 3)
        {
            ITE_NMF_ZoomTest(ap_args[1], ITE_ConvToFloat(ap_args[2]));
        }
        else
        {
            CLI_disp_error("Not correct command arguments\n");
        }
    }


    return (CMD_COMPLETE);
}


CMD_COMPLETION
C_ite_dbg_zoomtestfullrange_cmd(
int     a_nb_args,
char    **ap_args)
{
    if (ap_args[1] == NULL)
    {
        ap_args[1] = "";
    }


    if (0 == strcmp(ap_args[1], "help"))
    {
        CLI_disp_error("syntax:  zoomtestfullrange <LR/HR/BMS> <MaxDZ> <video/still>\n");
    }
    else
    {
        if (a_nb_args == 3)
        {
            ITE_NMF_ZoomTest_FullRange(ap_args[1], ITE_ConvToFloat(ap_args[2]));
        }
        else if (a_nb_args == 4)
        {
            if (0 == strcmp(ap_args[3], "video"))
            {
                ITE_NMF_VIDEOZoomTest_FullRange(ap_args[1], ITE_ConvToFloat(ap_args[2]));
            }
        }
        else
        {
            CLI_disp_error("Not correct command arguments\n");
        }
    }


    return (CMD_COMPLETE);
}


CMD_COMPLETION
C_ite_dbg_zoominit_cmd(
int     a_nb_args,
char    **ap_args)
{
    if (ap_args[1] == NULL)
    {
        ap_args[1] = "";
    }


    if (0 == strcmp(ap_args[1], "help"))
    {
        CLI_disp_error("syntax: zoominit             // for using MaxDZ from SAS document \n");
        CLI_disp_error("syntax: or zoominit <MaxDZ>  // for specific setting\n");
    }
    else
    {
        if (a_nb_args == 2)
        {
            ITE_NMF_DZ_Init(ITE_ConvToFloat(ap_args[1]));
        }
        else if (a_nb_args == 1)
        {
            ITE_NMF_DZ_Init(ITE_NMF_returnMaxDZforCurrentUsecase());
        }
        else
        {
            CLI_disp_error("Not correct command arguments\n");
        }
    }


    return (CMD_COMPLETE);
}


CMD_COMPLETION
C_ite_dbg_zoomset_cmd(
int     a_nb_args,
char    **ap_args)
{

    t_bool toggle = Flag_e_TRUE;  //purposefully initialized to Flag_e_TRUE

    if (ap_args[1] == NULL)
    {
        ap_args[1] = "";
    }


    if (0 == strcmp(ap_args[1], "help"))
    {
        CLI_disp_error("syntax: zoomset <DZ> [toggle|notoggle]\n");
    }
    else
    {
        if ((a_nb_args == 2) || (a_nb_args == 3))
        {
            if (a_nb_args == 3)
            {
                if (0 == strcmp("toggle", ap_args[2]))
                {
                    toggle = Flag_e_TRUE;
                }
                else if (0 == strcmp("notoggle", ap_args[2]))
                {
                    toggle = Flag_e_FALSE;
                }
                else
                {
                    CLI_disp_error("Not correct command arguments\n");
                    return (CMD_ERR_ARGS);
                }
            }

            ITE_NMF_DZ_ZoomSet(ITE_ConvToFloat(ap_args[1]), toggle);
        }
        else
        {
            CLI_disp_error("Not correct command arguments\n");
            return (CMD_ERR_ARGS);
        }
    }


    return (CMD_COMPLETE);
}


CMD_COMPLETION
C_ite_dbg_zoomsetFOV_cmd(
int     a_nb_args,
char    **ap_args)
{
    if (0 == strcmp(ap_args[1], "help"))
    {
        CLI_disp_error("syntax: zoomfov <FOVX>\n");
    }
    else
    {
        if (a_nb_args == 2)
        {
            ITE_NMF_DZ_SetFovX(ITE_ConvToFloat(ap_args[1]));
        }
        else
        {
            CLI_disp_error("Not correct command arguments\n");
        }
    }


    return (CMD_COMPLETE);
}


CMD_COMPLETION
C_ite_dbg_zoomhelp_cmd(
int     a_nb_args,
char    **ap_args)
{
    UNUSED(ap_args);
    if (a_nb_args == 1)
    {
        CLI_disp_error("\n");
        CLI_disp_error("zoominit        : Zoom initialize MaxDZ and center \n");
        CLI_disp_error("        syntax: zoominit             // for using MaxDZ from SAS document \n");
        CLI_disp_error("        syntax: or zoominit <MaxDZ>  // for specific setting\n");
        CLI_disp_error("zoomset     : execute a Zoom Set Command\n");
        CLI_disp_error("        zoomset <DZ> [toggle|notoggle]\n");
        CLI_disp_error("zoomfov     : execute a Zoom Set Command using FOVX\n");
        CLI_disp_error("        zoomfov <FOVX>\n");
        CLI_disp_error("storebuff       : store LR,HR,or BMS[0] buffer into LR,HR or BMS[1]\n");
        CLI_disp_error("         storebuff <LR/HR/BMS>\n");
        CLI_disp_error("zoomtest        : launch zoom metric on [1] (ref) and [0] LR,HR, or BMS buffer\n");
        CLI_disp_error("        zoomtest <LR/HR/BMS> <DZ> \n");
        CLI_disp_error("zoomtestfullrange       : launch 50 step from 1 to MaxDZ tests on LR,HR, or BMS buffer\n");
        CLI_disp_error("        zoomtestfullerange <LR/HR/BMS>\n");
    }
    else
    {
        CLI_disp_error("Not correct command arguments\n");
    }


    return (CMD_COMPLETE);
}


CMD_COMPLETION
C_ite_dbg_zoomtestbug1_cmd(
int     a_nb_args,
char    **ap_args)
{
    volatile float      DZ;
    volatile t_uint32   MaxDZ;
    volatile t_uint32   MaxFOVX;
    volatile t_uint32   MinFOVX;
    int ret =EOK;


    if (ap_args[1] == NULL)
    {
        ap_args[1] = "";
    }


    if (0 == strcmp(ap_args[1], "help"))
    {
        CLI_disp_error("syntax: zoomtestbug1\n");
    }
    else
    {
        if (a_nb_args == 1)
        {
            ret = ITE_dbg_setusecase("LR", "QVGA", "YUV422RI");
            if(ret != EOK)
            {
                CLI_disp_error("Not correct command arguments C_ite_dbg_zoomtestbug1_cmd\n");
                goto END;
            }

            ret = ITE_dbg_setusecase("HR", "UXGA", "YUV420MB_D");
            if(ret != EOK)
            {
                CLI_disp_error("Not correct command arguments C_ite_dbg_zoomtestbug1_cmd\n");
                goto END;

            }

            ITE_dbg_bmsmode("STILL");

            ret = ITE_dbg_setusecase("BMS", "8MEGA", "RAW8");
            if(ret != EOK)
            {
                CLI_disp_error("Not correct command arguments C_ite_dbg_zoomtestbug1_cmd\n");
                goto END;

            }

            ITE_dbg_bmsout("BAYERSTORE0");
            ITE_dbg_bmlin("BAYERLOAD1");
            ITE_LR_Prepare(&usecase, INFINITY);

            // should be remove when bug will be fixed
            ITE_SetFrameRateControl(2, 2);
            ITE_LR_Start(INFINITY);
            ITE_LR_Stop();
            ITE_SetFrameRateControl(15, 15);

            // until here
            ITE_LR_Start(INFINITY);
            LOS_Sleep(500);
            ITE_LR_Stop();
            ITE_StoreCurrentFOVX();

            ITE_BMS_Prepare(&usecase, INFINITY);
            ITE_BMS_Start(1);
            LOS_Log("\nStream one frame on BMS \n");
            ITE_BMS_Stop();

            ITE_LR_Free();

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
            DZ = 1;
            BmlInfo.Previous_f_SetFOVX = 0;
            ITE_BML_Start(1);
            LOS_Log("\nINject one frame on BML \n");
            LOS_Sleep(500);
            ITE_BML_Stop();

            DZ = 1;
            LOS_Log("ZoomTest for DZ = %f (MaxDZ= %f)\n", DZ, *( volatile float * ) &MaxDZ);
            ITE_NMF_STILLZoomTest("HR", DZ,0);//NO STRIPE
            DZ = 1.55;
            LOS_Log("ZoomTest for DZ = %f (MaxDZ= %f)\n", DZ, *( volatile float * ) &MaxDZ);
            ITE_NMF_STILLZoomTest("HR", DZ,0);//NO STRIPE

            ITE_BMS_Free();
            ITE_BML_Free();
            ITE_HR_Free();
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
C_ite_dbg_zoomtestbug2_cmd(
int     a_nb_args,
char    **ap_args)
{
    volatile float      DZ;
    volatile t_uint32   MaxDZ;
    volatile t_uint32   MaxFOVX;
    volatile t_uint32   MinFOVX;
    volatile float      FOVX;
    int ret  = EOK;

    if (ap_args[1] == NULL)
    {
        ap_args[1] = "";
    }


    if (0 == strcmp(ap_args[1], "help"))
    {
        CLI_disp_error("syntax: zoomtestbug2\n");
    }
    else
    {
        if (a_nb_args == 1)
        {
            ret = ITE_dbg_setusecase("LR", "PAL", "YUV422RI");
            if(ret != EOK)
            {
                CLI_disp_error("Not correct command arguments C_ite_dbg_zoomtestbug2_cmd\n");
                goto END;

            }

            ret = ITE_dbg_setusecase("HR", "PAL", "YUV422RI");
            if(ret != EOK)
            {
                CLI_disp_error("Not correct command arguments C_ite_dbg_zoomtestbug2_cmd\n");
                goto END;

            }

            ITE_dbg_bmsmode("STILL");
            ret = ITE_dbg_setusecase("BMS", "8MEGA", "RAW8");
            if(ret != EOK)
            {
                CLI_disp_error("Not correct command arguments C_ite_dbg_zoomtestbug2_cmd\n");
                goto END;

            }

            ITE_dbg_bmsout("BAYERSTORE0");
            ITE_dbg_bmlin("BAYERLOAD1");
            ITE_LR_Prepare(&usecase, INFINITY);

            // should be remove when bug will be fixed
            ITE_SetFrameRateControl(2, 2);
            ITE_LR_Start(INFINITY);
            ITE_LR_Stop();
            ITE_SetFrameRateControl(15, 15);

            // until here
            ITE_LR_Start(INFINITY);
            LOS_Sleep(500);
            ITE_LR_Stop();
            ITE_StoreCurrentFOVX();

            ITE_BMS_Prepare(&usecase, INFINITY);
            ITE_BMS_Start(1);
            LOS_Log("\nStream one frame on BMS \n");
            ITE_BMS_Stop();

            ITE_LR_Free();

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
            DZ = 1;
            FOVX = (*( volatile float * ) &MaxFOVX) / DZ;
            BmlInfo.Previous_f_SetFOVX = *( volatile t_uint32 * ) &FOVX;
            ITE_BML_Start(1);
            LOS_Log("\nINject one frame on BML \n");
            LOS_Sleep(500);
            ITE_BML_Stop();
            DZ = (*( volatile float * ) &MaxDZ);
            LOS_Log("ZoomTest for DZ = %f (MaxDZ= %f)\n", DZ, *( volatile float * ) &MaxDZ);
            ITE_NMF_STILLZoomTest("HR", DZ,0);//NO STRIPE
            ITE_BMS_Free();
            ITE_BML_Free();
            ITE_HR_Free();

            ret = ITE_dbg_setusecase("LR", "WVGA", "YUV422RI");
            if(ret != EOK)
            {
                CLI_disp_error("Not correct command arguments C_ite_dbg_zoomtestbug2_cmd\n");
                goto END;

            }

            ret = ITE_dbg_setusecase("HR", "WVGA", "YUV422RI");
            if(ret != EOK)
            {
                CLI_disp_error("Not correct command arguments C_ite_dbg_zoomtestbug2_cmd\n");
                goto END;

            }

            ITE_dbg_bmsmode("STILL");
            ret = ITE_dbg_setusecase("BMS", "8MEGA", "RAW8");
            if(ret != EOK)
            {
                CLI_disp_error("Not correct command arguments C_ite_dbg_zoomtestbug2_cmd\n");
                goto END;

            }

            ITE_dbg_bmsout("BAYERSTORE0");
            ITE_dbg_bmlin("BAYERLOAD1");
            ITE_LR_Prepare(&usecase, INFINITY);

            // should be remove when bug will be fixed
            ITE_SetFrameRateControl(2, 2);
            ITE_LR_Start(INFINITY);
            ITE_LR_Stop();
            ITE_SetFrameRateControl(15, 15);

            // until here
            ITE_LR_Start(INFINITY);
            LOS_Sleep(500);
            ITE_LR_Stop();
            ITE_StoreCurrentFOVX();

            ITE_BMS_Prepare(&usecase, INFINITY);
            ITE_BMS_Start(1);
            LOS_Log("\nStream one frame on BMS \n");
            ITE_BMS_Stop();

            ITE_LR_Free();

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
            DZ = 1;
            FOVX = (*( volatile float * ) &MaxFOVX) / DZ;
            BmlInfo.Previous_f_SetFOVX = *( volatile t_uint32 * ) &FOVX;
            ITE_BML_Start(1);
            LOS_Log("\nINject one frame on BML \n");
            LOS_Sleep(500);
            ITE_BML_Stop();
            DZ = (*( volatile float * ) &MaxDZ);
            LOS_Log("ZoomTest for DZ = %f (MaxDZ= %f)\n", DZ, *( volatile float * ) &MaxDZ);
            ITE_NMF_STILLZoomTest("HR", DZ,0);//NO STRIPE
            ITE_BMS_Free();
            ITE_BML_Free();
            ITE_HR_Free();
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
C_ite_dbg_zoomtest_stress_cmd(
int     a_nb_args,
char    **ap_args)
{
  if (ap_args[1] == NULL)
    {
        ap_args[1] = "";
    }


    if (0 == strcmp(ap_args[1], "help"))
    {
        CLI_disp_error("syntax: zoomrange <zoomstart> <zoomend> <zoomstep> <LR/HR/LRHR>\n");
    }
    else if (a_nb_args == 5)
    {
      ITE_Zoom_stress_test(ap_args[1],ap_args[2],ap_args[3],ap_args[4]);
    }
    else
    {
      CLI_disp_error("Not correct command arguments\n");
    }
    return (CMD_COMPLETE);
}


t_cmd_list  ite_cmd_list_dz[] =
{
    { "zoomhelp", C_ite_dbg_zoomhelp_cmd, "zoomhelp: zoomhelp\n" },
    { "zoominit", C_ite_dbg_zoominit_cmd, "zoominit: zoominit <maxDZ> \n" },
    { "zoomset", C_ite_dbg_zoomset_cmd, "zoomset: zoomset <DZ> \n" },
    { "zoomfov", C_ite_dbg_zoomsetFOV_cmd, "zoomfov: zoomfov <FOVX> \n" },
    { "storebuff", C_ite_dbg_storebuff_cmd, "storebuff: storebuff <LR/HR/BMS> \n" },
    { "zoomtest", C_ite_dbg_zoomtest_cmd, "zoomtest: zoomtest <LR/HR/BMS> <DZ> \n" },
    { "zoomtestfullrange", C_ite_dbg_zoomtestfullrange_cmd, "zoomtestfullrange: zoomtestfullrange <LR/HR/BMS> \n" },
    { "zoomtestbug1", C_ite_dbg_zoomtestbug1_cmd, "zoomtestbug1: zoomtestbug1 \n" },
    { "zoomtestbug2", C_ite_dbg_zoomtestbug2_cmd, "zoomtestbug2: zoomtestbug2 \n" },
    { "zoomrange", C_ite_dbg_zoomtest_stress_cmd, "zoomrange <zoomstart> <zoomend> <zoomstep> <PIPE>\n"},
};

void
Init_dz_ITECmdList(void)
{
    CLI_register_interface("DZ_ITE_CMB", sizeof(ite_cmd_list_dz) / (sizeof(ite_cmd_list_dz[0])), ite_cmd_list_dz, 1);
}


TAlgoError
ITE_NMF_Pan_tilt_Test_FullRange(
char    *pipe,
float   MaxDZ)
{
    t_sint16    i;
    t_uint16    zoomstep = 10;
    t_uint16    maxstep = zoomstep;
    float       DZ;
    char        testComment[200];
    char        sensorName[16];
    char        formatName[16];
    char        resolutionName[16];
    char        pathname[200];
    char        mess[256];
    TAlgoError  ret = EErrorNone;
    t_uint32    Num = 0;

    if (usecase.sensor == 0)
    {
        sprintf(sensorName, "Cam0");
    }
    else
    {
        sprintf(sensorName, "Cam1");
    }


    ITE_GiveStringFormat(pipe, formatName);
    if (0 == strcmp(pipe, "LR"))
    {
        ITE_GiveStringNameFromEnum(usecase.LR_resolution, resolutionName);
        LOS_Log(">>>>>>>>>>>>>>>Zoom Test: Pipe LR, Resolution: %s <<<<<<<<<<<<<<<\n", resolutionName);
    }


    if (0 == strcmp(pipe, "HR"))
    {
        ITE_GiveStringNameFromEnum(usecase.HR_resolution, resolutionName);
        LOS_Log(">>>>>>>>>>>>>>>Zoom Test: Pipe HR, Resolution: %s <<<<<<<<<<<<<<<\n", resolutionName);
    }


    sprintf(testComment, "%s_test_nreg_%s_Pan_Tilt_%s_%s", sensorName, pipe, formatName, resolutionName);

    //perform a "cd" to destination repository for test log
    sprintf(pathname, IMAGING_PATH "/ite_nmf/test_results_nreg/Pan_Tilt_STREAMING");
    strcat(g_out_path, pathname);
    LOS_Log("Current path: >%s< \n", ( char * ) g_out_path);
    init_test_results_nreg(g_out_path);
    mmte_testStart(testComment, " test DZ all step", g_out_path);
    snprintf(mess, sizeof(mess), "MaxDZ %f for this usecase", MaxDZ);
    LOS_Log("DEBUG>>>>>:  MaxDZ %f for this usecase: %f\n", MaxDZ);
    mmte_testComment(mess);

    for (i = 0; i <= zoomstep; i++)
    {
        DZ = 1 + (i * ((MaxDZ - 1) / zoomstep));
        LOS_Log("ZoomTest for DZ = %f\n", DZ);
        snprintf(mess, sizeof(mess), "Test %d :check DZ Value %f ", ( int ) Num, DZ);   //__NO_WARNING__
        mmte_testNext(mess);

        // Reset Center offset to 0, 0 before changing DZ
        // This is very important
        ITE_NMF_DZ_SetCenter(0, 0);
        ITE_NMF_DZ_Set(DZ);

        /// Break first time denied is encountered. No further zoom-in is tested
        if (flagDZdenied == 1)
        {
            snprintf(mess, sizeof(mess), "DZ Step %f cannot be absorbed DZ Denied by ISP", DZ);
            mmte_testComment(mess);
            maxstep = i;
            break;
        }


        ret = ITE_NMF_Pan_tilt_Test(DZ);

        if (0 == ret)
        {
            mmte_testResult(TEST_PASSED);
        }
        else
        {
            mmte_testResult(TEST_FAILED);
        }
    }


    flagDZdenied = 0;
    Num++;
    LOS_Sleep(500);

    for (i = maxstep - 1; i >= 0; i--)
    {
        DZ = 1 + (i * ((MaxDZ - 1) / zoomstep));
        LOS_Log("ZoomTest for DZ = %f\n", DZ);
        snprintf(mess, sizeof(mess), "Test %d :check DZ Value %f ", ( int ) Num, DZ);   //__NO_WARNING__
        mmte_testComment(mess);

        // Reset Center offset to 0, 0 before changing DZ
        // This is very important
        ITE_NMF_DZ_SetCenter(0, 0);
        ITE_NMF_DZ_Set(DZ);

        if (flagDZdenied == 1)
        {
            snprintf(mess, sizeof(mess), "DZ Step %f cannot be absorbed DZ Denied by ISP", DZ);

            mmte_testComment(mess);

            flagDZdenied = 0;
        }
        else
        {
            ret = ITE_NMF_Pan_tilt_Test(DZ);

            if (0 == ret)
            {
                mmte_testResult(TEST_PASSED);
            }
            else
            {
                mmte_testResult(TEST_FAILED);
            }
        }


        flagDZdenied = 0;
        Num++;
        LOS_Sleep(500);
    }


    mmte_testEnd();

    //clean  g_out_path
    memset(g_out_path, 0, KlogDirectoryLentgh * sizeof(char));
    return (ret);   //__NO_WARNING__
}


/* -----------------------------------------------------------------------
   FUNCTION : ITE_testZoomstreamingLRorHR
   PURPOSE  : Test zoom streaming on LR and HR independantly
   for different output LR HR resolutions
   ------------------------------------------------------------------------ */
void
ITE_Zoom_stress_test(char *zstart,char *zend,char *zstep,char *Pipe)
{

    volatile t_uint32   MaxDZ;
    float zoomstart,zoomend,zoomstep;


    zoomstart = ITE_ConvToFloat (zstart);
    zoomend   = ITE_ConvToFloat (zend);
    zoomstep  = ITE_ConvToFloat (zstep);


    MaxDZ =zoomend;

    if (0 == strcmp(Pipe, "LR"))
    {
            ITE_LR_Prepare(&usecase, INFINITY);
            ITE_writePE(Zoom_Control_f_SetFOVX_Byte0, 0);
            ITE_LR_Start(INFINITY);
            LOS_Sleep(500);
            ITE_LR_Stop();
            ITE_NMF_DZ_Init(( float ) MaxDZ);
            ITE_LR_Start(INFINITY);
            LOS_Sleep(500);
            ITE_NMF_ZoomTest_with_range(zoomstart,zoomend,zoomstep,"LR",MaxDZ);
            ITE_LR_Stop();
            ITE_LR_Free();

        }



    if (0 == strcmp(Pipe, "HR"))
    {

            ITE_HR_Prepare(&usecase, INFINITY);
            ITE_writePE(Zoom_Control_f_SetFOVX_Byte0, 0);
            ITE_HR_Start(INFINITY);
            LOS_Sleep(500);
            ITE_HR_Stop();
            ITE_NMF_DZ_Init(( float ) MaxDZ);

            ITE_HR_Start(INFINITY);
            LOS_Sleep(500);

            ITE_NMF_ZoomTest_with_range(zoomstart,zoomend,zoomstep,"HR",MaxDZ);
            ITE_HR_Stop();
            ITE_HR_Free();


    }


     if (0 == strcmp(Pipe, "LRHR"))
    {

            ITE_LRHR_Prepare(&usecase, INFINITY);
            ITE_writePE(Zoom_Control_f_SetFOVX_Byte0, 0);
            ITE_LRHR_Start(INFINITY);
            LOS_Sleep(500);
            ITE_LRHR_Stop();
            ITE_NMF_DZ_Init(( float ) MaxDZ);

            ITE_LRHR_Start(INFINITY);
            LOS_Sleep(500);

            ITE_NMF_ZoomTest_with_range(zoomstart,zoomend,zoomstep,"LRHR",MaxDZ);
            ITE_LRHR_Stop();
            ITE_LRHR_Free();


    }
}



void
ITE_NMF_ZoomTest_with_range(float zoomstart,float zoomend,float zoomstep,char *Pipe,float MaxDZ)
{
    float       DZ;
    char        testComment[256];
    char        sensorName[16];
    char        pathname[256];
    char        mess[256];
    TAlgoError  ret = EErrorNone;
    t_uint32    Num = 0,chgover;
    volatile t_uint16       woi_x,woi_y,op_x,op_y;

    if (usecase.sensor == 0)
    {
        sprintf(sensorName, "Cam0");
    }
    else
    {
        sprintf(sensorName, "Cam1");
    }


    LOS_Log("zoomstart:%f\n", ( float ) zoomstart);
    LOS_Log("zoomend:%f\n", ( float ) zoomend);
    LOS_Log("zoomstep:%f\n", ( float ) zoomstep);
    LOS_Log("Pipe:%s\n", Pipe);
    LOS_Log("MaxDZ:%f\n", ( float ) MaxDZ);

    sprintf(pathname, IMAGING_PATH "/ite_nmf/test_results_nreg/Zoom_test");
    sprintf(g_out_path, pathname);
    init_test_results_nreg(g_out_path);
    if(0==strcmp(Pipe,"HR"))
     {
      snprintf(testComment,sizeof(testComment),"%s_zoomtest_%s_%dx%d_%d", sensorName,Pipe,(int)usecase.HR_XSize,( int )usecase.HR_YSize, ( int ) Num);

     }
     else if(0==strcmp(Pipe,"LR"))
     {
      snprintf(testComment,sizeof(testComment),"%s_zoomtest_%s_%dx%d_%d", sensorName, Pipe,( int )usecase.LR_XSize,( int )usecase.LR_YSize, ( int ) Num);

     }
     else
     {
       snprintf(testComment,sizeof(testComment),"%s_zoomtest_%s_%dx%d_LR_%dx%d_HR%d", sensorName,Pipe,(int)usecase.LR_XSize,( int )usecase.LR_YSize,( int )usecase.HR_XSize,( int )usecase.HR_YSize, ( int ) Num);
     }
    //perform a "cd" to destination repository for test log
    mmte_testStart(testComment, "test DZ all step", g_out_path);

          woi_x =ITE_readPE(Zoom_Status_LLA_u16_Ex_woi_resX_Byte0);
          woi_y =ITE_readPE(Zoom_Status_LLA_u16_Ex_woi_resY_Byte0);
          op_x  =ITE_readPE(Zoom_Status_LLA_u16_Ex_output_resX_Byte0);
          op_y  =ITE_readPE(Zoom_Status_LLA_u16_Ex_output_resY_Byte0);

          snprintf(mess, sizeof(mess), "ModeSelected WOI:%dx%d OP:%dx%d",(int)woi_x,(int)woi_y,(int)op_x,(int)op_y);
          mmte_testComment(mess);

    for (DZ = zoomstart; DZ <= zoomend; DZ = DZ + zoomstep)
    {
        chgover = g_zoom_chgover;
        snprintf(mess, sizeof(mess), "Test %d :check DZ Value %f ", ( int ) Num, DZ);    //__NO_WARNING__
        mmte_testNext(mess);
        LOS_Log("Current path: >%s< \n", ( char * ) g_out_path);
        LOS_Log("ZoomTest for DZ = %f\n", DZ);
        snprintf(mess, sizeof(mess), "MaxDZ %f for this usecase", MaxDZ);
        mmte_testComment(mess);
        DZ = DZ * 1000000/1000000 ;
        ret = ITE_NMF_ZoomTest(Pipe, DZ);
        LOS_Log("DZ = %f\n", DZ);
        if ((g_zoom_chgover - chgover)== 1)
        {
          woi_x =ITE_readPE(Zoom_Status_LLA_u16_Ex_woi_resX_Byte0);
          woi_y =ITE_readPE(Zoom_Status_LLA_u16_Ex_woi_resY_Byte0);
          op_x  =ITE_readPE(Zoom_Status_LLA_u16_Ex_output_resX_Byte0);
          op_y  =ITE_readPE(Zoom_Status_LLA_u16_Ex_output_resY_Byte0);

          snprintf(mess, sizeof(mess), "Change over occured WOI:%dx%d OP:%dx%d",(int)woi_x,(int)woi_y,(int)op_x,(int)op_y);
          mmte_testComment(mess);
        }
        if (ret == 0)
        {
            if (flagDZdenied == 1)
            {
                snprintf(mess, sizeof(mess), "DZ Step %f cannot be absorbed DZ Denied by ISP", DZ);
                mmte_testComment(mess);
                mmte_testResult(TEST_FAILED);
                LOS_Log("TEST FAILED\n");
            }

           else
            {

              snprintf(mess, sizeof(mess), "TEST PASSED");
              mmte_testComment(mess);
              mmte_testResult(TEST_PASSED);
              LOS_Log("TEST PASSED\n");           }
        }
        else
        {
            if (flagDZdenied == 1)
            {
                snprintf(mess, sizeof(mess), "DZ Step %f cannot be absorbed DZ Denied by ISP", DZ);
                mmte_testComment(mess);
                mmte_testResult(TEST_FAILED);
                LOS_Log("TEST FAILED\n");
            }
            else
            {
                snprintf(mess, sizeof(mess), "TEST PASSED");
                mmte_testComment(mess);
                mmte_testResult(TEST_PASSED);
                LOS_Log("TEST PASSED\n");
            }
        }


        flagDZdenied = 0;
        Num++;
        LOS_Sleep(500);

    }

   snprintf(mess, sizeof(mess), "Now Testing Zoom From %f to %f", zoomend,zoomstart);
   mmte_testComment(mess);

    for (DZ = zoomend; DZ >= zoomstart; DZ = DZ - zoomstep)
    {
        chgover = g_zoom_chgover;
        snprintf(mess, sizeof(mess), "Test %d :check DZ Value %f ", ( int ) Num, DZ);    //__NO_WARNING__
        mmte_testNext(mess);
        LOS_Log("Current path: >%s< \n", ( char * ) g_out_path);
        LOS_Log("ZoomTest for DZ = %f\n", DZ);
        snprintf(mess, sizeof(mess), "MaxDZ %f for this usecase", MaxDZ);
        mmte_testComment(mess);

        DZ = DZ * 1000000/1000000;
        ret = ITE_NMF_ZoomTest(Pipe, DZ);
        LOS_Log("DZ = %f\n", DZ);

         if ((g_zoom_chgover - chgover)== 1)
        {
          woi_x =ITE_readPE(ReadLLAConfig_Status_u16_woi_res_width_Byte0);
          woi_y =ITE_readPE(ReadLLAConfig_Status_u16_woi_res_height_Byte0);
          op_x  =ITE_readPE(ReadLLAConfig_Status_u16_output_res_width_Byte0);
          op_y  =ITE_readPE(ReadLLAConfig_Status_u16_output_res_height_Byte0);

          snprintf(mess, sizeof(mess), "Change over occured WOI:%dx%d OP:%dx%d",(int)woi_x,(int)woi_y,(int)op_x,(int)op_y);
          mmte_testComment(mess);
        }

        if (ret == 0)
        {
            if (flagDZdenied == 1)
            {
                snprintf(mess, sizeof(mess), "DZ Step %f cannot be absorbed DZ Denied by ISP", DZ);
                mmte_testComment(mess);
                mmte_testResult(TEST_FAILED);
                LOS_Log("TEST FAILED\n");
            }

           else
            {

              snprintf(mess, sizeof(mess), "TEST PASSED");
              mmte_testComment(mess);
              mmte_testResult(TEST_PASSED);
              LOS_Log("TEST PASSED\n");           }
             }
        else
        {
            if (flagDZdenied == 1)
            {
                snprintf(mess, sizeof(mess), "DZ Step %f cannot be absorbed DZ Denied by ISP", DZ);
                mmte_testComment(mess);
                mmte_testResult(TEST_FAILED);
                LOS_Log("TEST FAILED\n");
            }
            else
            {
                snprintf(mess, sizeof(mess), "TEST PASSED");
                mmte_testComment(mess);
                mmte_testResult(TEST_PASSED);
                LOS_Log("TEST PASSED\n");
            }
        }


        flagDZdenied = 0;
        Num++;
        LOS_Sleep(500);

    }

    g_zoom_chgover = 0;
    mmte_testEnd();

    //clean  g_out_path
    memset(g_out_path, 0, KlogDirectoryLentgh * sizeof(char));
}

