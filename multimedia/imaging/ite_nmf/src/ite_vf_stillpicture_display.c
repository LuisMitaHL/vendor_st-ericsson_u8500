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
#include "ite_vf_stillpicture_display.h"

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

#include <test/api/test.h>
#include <cm/inc/cm_macros.h>

#include <los/api/los_api.h>
#include "ite_main.h"

#include "ite_host2sensor.h"
#include "ite_nmf_Rx_Testpattern.h"
#include "ite_debug.h"


extern struct s_grabParams grabparamsHR;
extern struct s_grabParams grabparamsLR;
extern volatile t_uint32 g_grabHR_infinite;
extern volatile t_uint32 g_grabLR_infinite;
extern ts_siapicturebuffer GrabBufferHR[];
extern ts_siapicturebuffer LCDBuffer;
extern ts_siapicturebuffer GamBuffer;

extern ts_sia_usecase usecase;
// Sensor Or Rx Input Selection. Default Value is Sensor0
extern InputImageSource_te g_InputImageSource;


/* -----------------------------------------------------------------------
FUNCTION : ITE_GrabVPIPDisplayCmd
PURPOSE  : Command for Grab Sensor and display use case
------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_VfStillDisplay_cmd(int a_nb_args, char ** ap_args)
{
    
    if (a_nb_args == 3)
    {
        //ITE_StoreTestHeader(a_nb_args,ap_args);
        ITE_VfStillDisplay(ap_args[1], ap_args[2]);
    }
    else
    {
        CLI_disp_error("Not correct command arguments\n");
    }
    return CMD_COMPLETE;
}

/* -----------------------------------------------------------------------
FUNCTION : ITE_RAWDump
PURPOSE  : Function to dump a RAW bayer file
------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_RAWDump_cmd(int a_nb_args, char ** ap_args)
{
    int ret = EOK;
    if (a_nb_args == 3)
    {
        ret = ITE_RAWDump(ap_args[1], ap_args[2]);

        if(ret != EOK)
        {
            CLI_disp_error("Not correct command arguments C_ite_RAWDump_cmd\n");
            goto END;
        }

    }
    else
    {
        CLI_disp_error("Not correct command arguments\n");
        goto END;
    }
    END:
    return CMD_COMPLETE;
}

/* -----------------------------------------------------------------------
FUNCTION : ITE_VfStillDisplay
PURPOSE  : ViewFinderr and Still picture display use case
------------------------------------------------------------------------ */
void ITE_VfStillDisplay(char * ap_test_id, char * ap_grabvpip_options)
{
    char out_filename[64];
    //t_uint16 framerate=30;
    //t_uint8 error=TRUE;
    
    //disable log pageelement
    //MMITE_Log_RW_pageelements_disable();
    ITE_Log_RW_pageelements_enable();
    
    /* HCL Services, alloc, Log, IT, xti ....*/
    //ITE_SKEInit();
    // start
    mmte_testStart("test_LR_datapath"," test pipe LR with sensor in test mode", g_out_path);
    
    ITE_InitUseCase(ap_test_id, ap_grabvpip_options,&usecase);
    
    ITE_Start_Env(&usecase,ap_grabvpip_options,0,0);
    
    // MCDE format is 16bpp RGB565 equivalent to GRBFMT_R5G6B5 in term of bpp
    
    //ITE_createSiaPictureBuffer(&LCDBuffer,usecase.LR_XSize,usecase.LR_YSize,usecase.GRBFMT_YUV422_RASTER_INTERLEAVED,BUF);  
    ITE_createSiaPictureBuffer(&LCDBuffer,usecase.LR_XSize,usecase.LR_YSize,GRBFMT_R5G6B5,BUF);  
    
#ifdef DSI_DISPLAY
    // configure dsi display 
    ITE_configureMcde(&LCDBuffer);
    ITE_TestMcdeForDsi(&LCDBuffer);	
#endif  
    
    //Sensor in test mode
    
    if(g_InputImageSource == InputImageSource_e_Sensor0)
    {
        ITE_SensorMode(COLORBAR);}
    else
    {
        ITE_SetRxTestPattern(RxTestPattern_e_ColourBars);
    }


    ITE_LR_Prepare(&usecase, INFINITY);
    
    ITE_LR_Start(INFINITY);
    
    LOS_Log("\n60secs stream on LR pipe\n");
    
#ifdef DSI_DISPLAY   
    ITE_DisplayBuffer(&(GrabBufferLR[0]));
#endif
    
    LOS_Sleep(60000);
    
#ifdef DSI_DISPLAY    
    ITE_DisplayBufferStop(); 
#endif
    
    ITE_LR_Stop();
    ITE_LR_Free();
    
    ITE_HR_Prepare(&usecase, INFINITY);
    ITE_HR_Start(10);
    
    
#ifdef DSI_DISPLAY  
    ITE_DisplaySingleBuffer(&(GrabBufferHR[0]));
#endif 
    
    LOS_Log("\n1secs stream on HR pipe\n");
    LOS_Sleep(1000);
    
    /********************************************************************/
    //convert capture frame into .gam file in RGB565 format
    ITE_createSiaPictureBuffer(&GamBuffer,usecase.HR_XSize,usecase.HR_YSize,GRBFMT_R5G6B5,BUF_HR);  
    LOS_Sleep(500);  
    
#if !(defined(__ARM_LINUX) || defined(__ARM_SYMBIAN))
    ITE_ConvertBufferFormatwithB2R2(&(GrabBufferHR[0]),&GamBuffer);
#endif
    //dump grabbed frame in RGB16 format
    sprintf(out_filename,"../frame_rgb16_%d_%d",(int) usecase.HR_XSize,(int) usecase.HR_YSize); //__NO_WARNING__
    ITE_StoreBufferInFile(out_filename,&GamBuffer, ITE_FILE_GAM);
    ITE_FreeSiaPictureBuffer(&GamBuffer);
    /*********************************************************************/
    
    ITE_HR_Stop();
    ITE_HR_Free();
    ITE_Stop_Env();
    
    // end test
    mmte_testEnd();
}

/* -----------------------------------------------------------------------
FUNCTION : ITE_RAWDump
PURPOSE  : Function to dump a RAW bayer file
------------------------------------------------------------------------ */
int  ITE_RAWDump(char * ap_test_id, char * ap_grabvpip_options)
{
    int ret = EOK;
    char out_filename[100];
    char pathname[256],filename[256];
    char BMSresolution[16];
    static t_uint16 filenumber = 0;
    UNUSED(ap_test_id);
    UNUSED(ap_grabvpip_options);
    
    LOS_Log("ITE_RAWDump: RAW8 and RAW12 buffer still saved to Buffer\n");

    sprintf(filename, "RAWDump");
    sprintf(pathname,IMAGING_PATH"/ite_nmf/test_results_nreg");
    memset(g_out_path, 0, KlogDirectoryLentgh * sizeof(char));
    strcat(g_out_path,pathname);
 
    mmte_testStart(filename, "ITE_RAWDump: RAW8 and RAW12 buffer stills test", g_out_path);

    ITE_dbg_bmsmode("STILL");
    ret = ITE_dbg_setusecase("BMS", BMSresolution, "RAW8");
    if(ret != EOK)
    {
        LOS_Log("ITE_dbg_setusecase gave error in %s and line  %d\n",__FUNCTION__,__LINE__);
        goto END;
    }

    ITE_dbg_bmsout("BAYERSTORE2");

    LOS_Log("Streaming for 5 seconds on LS Pipe\n");
    ITE_LR_Prepare(&usecase, INFINITY);   
    ITE_LR_Start(INFINITY);  
    LOS_Sleep(5000);   
    ITE_LR_Stop();
    ITE_LR_Free();
	
    ITE_BMS_Prepare(&usecase, INFINITY);
    ITE_BMS_Start(1);
    LOS_Log("\nStream one frame on BMS \n");
    ITE_BMS_Stop(); 
    LOS_Log("\n After BMS Stop\n");
    
    LOS_Sleep(500);
    // In RAW8, Last 8 column's are to be discarded, and first 2 rows.
    // Therefore adding 8, 2 to the filename
    snprintf(out_filename,sizeof(out_filename),IMAGING_PATH"/frame_%d_raw8_%d_%d_",
            (int) (filenumber), (int) (usecase.BMS_XSize+8),(int) (usecase.BMS_YSize+2)) ;
    LOS_Log("\nSaving BMS RAW8 buffer\n");
    ITE_StoreBufferInFile(out_filename,&GrabBufferBMS[0], ITE_FILE_RAW);

    LOS_Sleep(500);
    ITE_BMS_Free(); 
 
    //-------------RAW 12------------//   
    LOS_Log("Taking 12bits still\n");
 
    ITE_dbg_bmsmode("STILL");
    ITE_dbg_setusecase("BMS", BMSresolution, "RAW12");
    ITE_dbg_bmsout("BAYERSTORE2");

    ITE_LR_Prepare(&usecase, INFINITY);
    ITE_LR_Start(INFINITY);  
    LOS_Sleep(5000);   
    ITE_LR_Stop();
    ITE_LR_Free();
	
    ITE_BMS_Prepare(&usecase, INFINITY);
    ITE_BMS_Start(1);
    LOS_Log("\nStream one frame on BMS \n");
    ITE_BMS_Stop(); 
    LOS_Log("\n After BMS Stop\n");
    
    LOS_Sleep(500);
    // In Y axis First two lines are to be discarded. Therefore adding 2 to the file size
    snprintf(out_filename,sizeof(out_filename),IMAGING_PATH"/frame_%d_raw12_%d_%d_",
            (int) (filenumber), (int) (usecase.BMS_XSize),(int) (usecase.BMS_YSize+2)) ;
    LOS_Log("\nSaving BMS RAW12 buffer\n");
    ITE_StoreBufferInFile(out_filename,&GrabBufferBMS[0], ITE_FILE_RAW);

    LOS_Sleep(500);
    ITE_BMS_Free(); 
    
    filenumber++;
 
    // end test
    mmte_testResult(TEST_PASSED);
    memset(g_out_path, 0, KlogDirectoryLentgh * sizeof(char));
    END:
    return ret;
}
