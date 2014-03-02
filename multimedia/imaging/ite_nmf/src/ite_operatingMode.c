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
#include "ite_operatingMode.h"
#include "ite_main.h"
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

#include <test/api/test.h>
#include <cm/inc/cm_macros.h>


extern struct s_grabParams grabparamsHR;
extern struct s_grabParams grabparamsLR;
extern volatile t_uint32 g_grabHR_infinite;
extern volatile t_uint32 g_grabLR_infinite;
extern ts_siapicturebuffer GrabBufferLR[];
extern ts_siapicturebuffer GrabBufferHR[];
extern ts_siapicturebuffer LCDBuffer;
extern ts_siapicturebuffer GamBuffer;

extern ts_sia_usecase usecase;

/* -----------------------------------------------------------------------
FUNCTION : C_ite_OperatingMode_cmd
PURPOSE  : Command launch "OMX like" operating mode
------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_OperatingMode_cmd(int a_nb_args, char ** ap_args)
{
    
    if (a_nb_args == 3)
    {
        //ITE_StoreTestHeader(a_nb_args,ap_args);
        ITE_OperatingMode(ap_args[1], ap_args[2]);
    } 
    else
    {
        CLI_disp_msg("Not correct command arguments\n");
    }
    return CMD_COMPLETE;
    
}

/* -----------------------------------------------------------------------
FUNCTION : ITE_OperatingMode
PURPOSE  : launch OMX like operating mode
example of Video Preview and Video capture
followed by an example of Still Preview and Still capture
------------------------------------------------------------------------ */
void ITE_OperatingMode(char * ap_test_id, char * ap_grabvpip_options)
{  
    char out_filename[64];
    t_test_status status=TEST_FAILED;
    
    //disable log pageelement
    ITE_Log_RW_pageelements_disable();
    // start
    
    mmte_testStart("test_OpMode"," test video preview and video record basic", g_out_path);
    
    // example of Video preview and record basic use case
    
    ITE_InitUseCase(ap_test_id, ap_grabvpip_options,&usecase);
    
    usecase.HR_XSize = 640;
    usecase.HR_YSize = 480;
    
    ITE_Start_Env(&usecase,ap_grabvpip_options,0,0);
    
    /*********************************************/
    /*   Start VideoPreview/Video sequence       */
    /*********************************************/
    // message un peu Bidon 
    mmte_testComment("test Video basic use case");
    
    ITE_VideoPreview_Prepare(&usecase);
    ITE_DisplayBuffer(&(GrabBufferLR[0]));
    ITE_VideoPreview_Start(INFINITY);    // probably INFINITY Could be removed
    
    //10 secs viewfinder
    LOS_Log("\n10secs viewfinder then stop VideoPreview mode\n");
    LOS_Sleep(10000);
    
    ITE_VideoPreview_Stop();
    
    //3 secs viewfinder
    LOS_Log("\n3 secs VideoPreview stopped then restart VideoPreview mode\n");
    LOS_Sleep(3000);
    ITE_VideoPreview_Start(INFINITY);    // probably INFINITY Could be removed
    
    //10 secs viewfinder
    LOS_Log("\n10secs VideoPreview then start video record\n");
    LOS_Sleep(10000);
    
    ITE_Video_Prepare(&usecase,INFINITY);
    ITE_Video_Start(INFINITY); 
    
    //10 secs viewfinder
    LOS_Log("\n10secs record then stop video mode\n");
    LOS_Sleep(10000);
    ITE_Video_Stop();
    ITE_Video_Free();
    ITE_VideoPreview_Free();
    
    /*********************************************/
    /*   Start StillPreview/Still sequence       */
    /*********************************************/
    
    // message un peu Bidon 
    mmte_testComment("test Still basic use case");
    
    // example of Still preview and Still basic use case
    usecase.HR_XSize = 2048;
    usecase.HR_YSize = 1536;
    
    LOS_Log("\n start Still Preview mode\n");
    ITE_StillPreview_Prepare(&usecase);
    ITE_DisplayBuffer(&(GrabBufferLR[0]));
    ITE_StillPreview_Start(INFINITY);    // probably INFINITY Could be removed
    
    //10 secs viewfinder
    LOS_Log("\n10secs then stop test\n");
    LOS_Sleep(10000);
    
    ITE_StillPreview_Stop();
    
    //3 secs viewfinder
    LOS_Log("\n3 secs stopped then restart StillPreview mode\n");
    LOS_Sleep(3000);
    
    ITE_StillPreview_Start(INFINITY);    // probably INFINITY Could be removed
    
    //10 secs viewfinder
    LOS_Log("\n10secs then quit StillPreview mode\n");
    LOS_Sleep(10000);
    ITE_StillPreview_Stop();
    
    //10 secs viewfinder
    LOS_Log("\n10secs then capture still on pipe HR\n");
    LOS_Sleep(10000);
    
    ITE_Still_Prepare(&usecase,1);
    //fill buffer with value 0
    //ITE_FillBufferWith(GrabBufferHR[0].logAddress, GrabBufferHR[0].buffersizeByte, 0);
    ITE_Still_Start(1);
    ITE_Still_Stop();  
    
    // test the grab buffer
    mmte_testNext("test grab buffer");
    // test grab buffer
    if(ITE_CheckGrabInBuffer(GrabBufferHR[0].logAddress,GrabBufferHR[0].buffersizeByte, 0)==TRUE) 
    {
        LOS_Log("grab in buffer OK\n",  NULL,NULL,NULL,NULL,NULL,NULL);
        status=TEST_PASSED;
    }
    else
    {
        LOS_Log("error !!!!!!!!! grab in buffer KO\n",  NULL,NULL,NULL,NULL,NULL,NULL);
        status=TEST_FAILED;
    }
    mmte_testResult(status);
    
    //visual check capture frame on LCD
    //ITE_DisplayBuffer(&(GrabBufferHR[0]));
 #if !(defined(__PEPS8500_SIA) || defined(_SVP_) || defined(__ARM_SYMBIAN))
    ITE_ConvertBufferFormatwithB2R2(&(GrabBufferHR[0]),&LCDBuffer);
#endif
    //convert capture frame into .gam file in RGB565 format
    ITE_createSiaPictureBuffer(&GamBuffer,usecase.HR_XSize,usecase.HR_YSize,GRBFMT_R5G6B5,BUF_HR);  
#if !(defined(__PEPS8500_SIA) || defined(_SVP_) || defined(__ARM_SYMBIAN))   
    ITE_ConvertBufferFormatwithB2R2(&(GrabBufferHR[0]),&GamBuffer);
#endif   
    //Write to file in .gam format
    // ITE_WriteRGB16Buffer2GamFile(GamBuffer, usecase.HR_XSize, usecase.HR_YSize, "still_rgb16");
    //dump grabbed frame in RGB16 format
    sprintf(out_filename,"../../frame_rgb16_%d_%d",(int) usecase.HR_XSize,(int) usecase.HR_YSize); //__NO_WARNING__
    ITE_StoreBufferInFile(out_filename,&GamBuffer, ITE_FILE_GAM);   
    ITE_FreeSiaPictureBuffer(&GamBuffer);
    
    
    ITE_Still_Free();
    
    // ITE_StillPreview_Prepare(&usecase);
    ITE_DisplayBuffer(&(GrabBufferLR[0]));
    ITE_StillPreview_Start(INFINITY);
    
    //10 secs viewfinder
    LOS_Log("\n10secs then quit use case\n");
    LOS_Sleep(10000);
    
    ITE_StillPreview_Stop();
    ITE_StillPreview_Free();
    
    ITE_Stop_Env();
    // end test
    
    mmte_testEnd();
    
}
