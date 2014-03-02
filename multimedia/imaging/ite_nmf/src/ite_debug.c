/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdio.h>
#include <stdlib.h>
//#include <string.h>

#include "ite_testenv_utils.h"
#include "cli.h"
#include "ite_debug.h"
#include "ite_main.h"
#include "pictor_full.h"
#include "ite_nreg_datapath_tests.h"
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
#include "ite_grab.h"
#include "ite_vpip.h"
//#include "ite_ske.h"
#include "ite_framerate.h"

#include "grab_types.idt.h"
#include "ite_nmf_standard_functions.h"
#include "ite_nmf_statistic_functions.h"
#include "ite_buffer_management.h"
#include "ite_host2sensor.h"
#include "ite_nmf_Rx_Testpattern.h"
#include "ite_convf900.h"
#include "ite_nmf_focuscontrol_functions.h"

extern void ITE_PutTestCrossSensor(float FOVX,enum e_grabPipeID pipe);
extern float ITE_GiveDesiredFOVX(int Step);

#include <test/api/test.h>
#include <cm/inc/cm_macros.h>

#include <los/api/los_api.h>
#include "ite_sia_buffer.h"

extern ts_sia_usecase usecase;

extern volatile t_uint32        g_grabLR_countglobal;
extern volatile t_uint32        g_grabHR_countglobal;

extern ts_siapicturebuffer LCDBuffer;
extern t_uint32                g_test_perf_status;
extern tps_siaMetaDatabuffer    pStatsBuffer;
extern tps_siaNVMbuffer         pNVMStatsBuffer;
extern tps_siaMetaDatabuffer    pTraceBuffer;
extern tps_siaFocusStatbuffer   pFocusStatsBuffer;
extern tps_siaMetaDatabuffer    pOutpuModeBuffer;

extern ts_siaFocusStatbuffer    AfFrameStatusBuffer;
extern tps_siaFocusStatbuffer   pAfFrameStatusBuffer;
extern tps_siaGammaLUTbuffer    pGammaLUTBuffer;

extern char                     g_sensor_select[20];
extern int                      g_Store_IMAGES;

// Sensor Or Rx Input Selection. Default Value is Sensor0
extern InputImageSource_te      g_InputImageSource;

//FW Trace property
extern TraceProp_ts     g_TraceProp;

#define NULL_FPS 0.0
/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_help_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_help_cmd(int a_nb_args, char ** ap_args)
{  if (a_nb_args == 1)
      {
       CLI_disp_error("\n");
       CLI_disp_error("LogPE     : disable or enable display read/write PE into terminal \n");
       CLI_disp_error("readPE    : read PE\n");
       CLI_disp_error("writePE   : write PE\n");
       CLI_disp_error("startenv  : instanciate nmf sia component;init usecase; Boot VPIP;\n");
       CLI_disp_error("stopenv   : destroy and unbind sia hw component; stop mcde; free buffers; \n");
       CLI_disp_error("setusecase: init usecase structure (pipe,size,format)\n");
       CLI_disp_error("getusecase: show usecase values\n");
       CLI_disp_error("LR        : prepare/start/stop/free LR mode sequence\n");
       CLI_disp_error("HR        : prepare/start/stop/free HR mode sequence\n");
       CLI_disp_error("LRHR      : prepare/start/stop/free LRHR mode sequence\n");
       CLI_disp_error("bms       : prepare/start/stop/free bms   mode sequence\n");
       CLI_disp_error("bml       : prepare/start/stop/free bml   mode sequence\n");
       CLI_disp_error("bmsmode   : prepare bms STILL or VIDEO mode\n");
       CLI_disp_error("bmsout    : prepare bms BAYERSTORE0 or 1 or 2 output mode\n");
       CLI_disp_error("bmlin     : prepare bml BAYERLOAD0 or 1 input mode\n");
       CLI_disp_error("storeFOVX : store current FOVX into specific usecase structure\n");
       CLI_disp_error("display   : allow to display LR or HR buffer (in infinity or single mode)\n");
       CLI_disp_error("cleanbuf  : fill sia buffer with 0 value\n");
       CLI_disp_error("autofps   : set framerate in mode auto\n");
       CLI_disp_error("manualfps : set framerate in mode manual\n");
       CLI_disp_error("getfps    : report framerate (PE status and calculated)\n");
       CLI_disp_error("colorbartest : check if buffer contain a colorbar\n");
       CLI_disp_error("buffercheck : check if buffer contain a specific color\n");
       CLI_disp_error("sensorwrite  : allow to write sensor register\n");
       CLI_disp_error("sensorread   : allow to read sensor register\n");
       CLI_disp_error("sensormode   : allow to put sensor into specific mode like colorbar, solidbar...\n");
       CLI_disp_error("wait  : do nothing during desired time (sec)\n");
       CLI_disp_error("writebmp  : write a bmp file from a given buffer\n");
       CLI_disp_error("givergb   : return RGB value of a pixel into a buffer\n");
       CLI_disp_error("glacehelp     : describe Glace command\n");
       CLI_disp_error("histohelp     : describe Histo command\n");
      }
   else
      { CLI_disp_error("Not correct command arguments\n"); }
   
    UNUSED(ap_args);
 return CMD_COMPLETE;
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_sensor_select_cmd
   PURPOSE  :  SENSORS SUPPORTED
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_sensor_select_cmd(int a_nb_args, char ** ap_args)
{
    //t_uint32 reg_addr;
if (ap_args[1]==NULL) ap_args[1]="";    //For Linux, strcmp doesn't support NULL parameter

if (0== strcmp(ap_args[1],"help"))
    {
       LOS_Log("syntax: sensorname IMX072\n") ;
    }
 else
    {
     if (a_nb_args == 2)
      {
        snprintf(g_sensor_select,sizeof(g_sensor_select),ap_args[1]);
      }
     else
      {
          LOS_Log("Not correct command arguments\n");
      }
    }
   return CMD_COMPLETE;
}

/* -----------------------------------------------------------------------
   FUNCTION : C_ite_nvm_raw_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_nvm_raw_cmd(int a_nb_args, char ** ap_args)
{
    //t_uint32 reg_addr;
if (ap_args[1]==NULL) ap_args[1]="";    //For Linux, strcmp doesn't support NULL parameter

if (0== strcmp(ap_args[1],"help"))
    {
       LOS_Log("syntax: NVM_RAW enable\n") ;
    }
 else
    {
     if (a_nb_args == 2)
      {
        ite_nvm_raw_data(ap_args[1]);
      }
     else
      {
          LOS_Log("Not correct command arguments\n");
      }
    }
   return CMD_COMPLETE;
}

/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_StoreFOVX_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_StoreFOVX_cmd(int a_nb_args, char ** ap_args)
{if (ap_args[1]==NULL) ap_args[1]="";   //For Linux, strcmp doesn't support NULL parameter
 if (0== strcmp(ap_args[1],"help")) {
    CLI_disp_error("syntax: storeFOVX\n"); }
 else {
   if (a_nb_args == 1)
      {
        ITE_StoreCurrentFOVX();
      }
   else
      { CLI_disp_error("Not correct command arguments\n"); }
   }
 return CMD_COMPLETE;
}

/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_PE_Log_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_PE_Log_cmd(int a_nb_args, char ** ap_args)
{ if (ap_args[1]==NULL) ap_args[1]="";  //For Linux, strcmp doesn't support NULL parameter
if (0== strcmp(ap_args[1],"help")) {
    CLI_disp_error("syntax: LogPE <enable/disable>\n"); }
 else {
   if (a_nb_args == 2)
      {   if (0== strcmp(ap_args[1],"enable"))
              ITE_Log_RW_pageelements_enable();
          if (0== strcmp(ap_args[1],"disable"))
          ITE_Log_RW_pageelements_disable();}
   else
      { CLI_disp_error("Not correct command arguments\n"); }
   }
 return CMD_COMPLETE;
}
/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_Clear_Stats_cmd
   PURPOSE  : Clear Hist and Glace
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_Clear_Stats_cmd(int a_nb_args, char ** ap_args)
{ if (ap_args[1]==NULL) ap_args[1]="";  //For Linux, strcmp doesn't support NULL parameter
if (0== strcmp(ap_args[1],"help")) {
    CLI_disp_error("syntax: Clearstats <glace/histo> \n"); }
 else {
   if (a_nb_args == 2)
      {
      ITE_Clear_Env(ap_args[1]);
      }
   else
      { CLI_disp_error("Not correct command arguments\n"); }
   }
 return CMD_COMPLETE;
}
/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_Display_Events_cmd
   PURPOSE  : Display all events in EventQ
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_Display_Events_cmd(int a_nb_args, char ** ap_args)
{ if (ap_args[1]==NULL) ap_args[1]="";  //For Linux, strcmp doesn't support NULL parameter
if (0== strcmp(ap_args[1],"help")) {
    CLI_disp_error("syntax: Display Events in Event Queue \n"); }
 else {
   if (a_nb_args == 1)
      {
      ITE_DisplayEvent();
    }
   else
      { CLI_disp_error("Not correct command arguments\n"); }
   }
 return CMD_COMPLETE;
}




/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_Store_IMAGE_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_Store_IMAGE_cmd(int a_nb_args, char ** ap_args)
{ if (ap_args[1]==NULL) ap_args[1]="";  //For Linux, strcmp doesn't support NULL parameter
if (0== strcmp(ap_args[1],"help")) {
    CLI_disp_error("syntax: LogPE <enable/disable>\n"); }
 else {
   if (a_nb_args == 2)
      {   if (0== strcmp(ap_args[1],"enable"))
            {
              g_Store_IMAGES = 1;
              LOS_Log("STE_NOIDA  Store_IMAGES %d \n",g_Store_IMAGES);
            }
          if (0== strcmp(ap_args[1],"disable"))
            {
              g_Store_IMAGES = 0;
            LOS_Log("STE_NOIDA  Store_IMAGES %d \n",g_Store_IMAGES);
          }
    }
   else
      { CLI_disp_error("Not correct command arguments\n"); }
   }
 return CMD_COMPLETE;
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_readPE_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_readPE_cmd(int a_nb_args, char ** ap_args)
{t_uint32 value;
if (ap_args[1]==NULL) ap_args[1]="";    //For Linux, strcmp doesn't support NULL parameter
if (0== strcmp(ap_args[1],"help")) {
    CLI_disp_error("syntax: readPE <PE address (0xhex or dec)>\n"); }
 else {
   if (a_nb_args == 2)
      { value = ITE_readPE(ITE_ConvToInt16(ap_args[1]));
    LOS_Log("value read: %x\n",(void *)value);}
   else
      { CLI_disp_error("Not correct command arguments\n"); }
   }
 return CMD_COMPLETE;
}

/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_writePE_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_writePE_cmd(int a_nb_args, char ** ap_args)
{if (ap_args[1]==NULL) ap_args[1]="";   //For Linux, strcmp doesn't support NULL parameter
if (0== strcmp(ap_args[1],"help")) {
    CLI_disp_error("syntax: writePE <PE address (0xhex or dec)> <data (0xhex or dec)>\n"); }
 else {
   if (a_nb_args == 3)
     //{ITE_dbg_writePE(ITE_ConvToInt16(ap_args[1]),ITE_ConvToInt16(ap_args[2]));}
     {ITE_writePE(ITE_ConvToInt16(ap_args[1]),ITE_ConvToInt32(ap_args[2]));}
   else
     {CLI_disp_error("Not correct command arguments\n"); }
   }
   return CMD_COMPLETE;
}

/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_float_readPE_cmd
   PURPOSE  : To Read Float PE's
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_float_readPE_cmd(int     a_nb_args,char    **ap_args)
{
    // For Linux, strcmp doesn't support NULL parameter
    if (ap_args[1] == NULL)
    {
        ap_args[1] = "";
    }


    if (0 == ITE_stricmp(ap_args[1], "help"))
    {
        CLI_disp_error("syntax: float_readPE <PE address (0xhex or dec)>\n");
    }
    else
    {
        if (a_nb_args == 2)
        {
            ITE_float_readPE(ITE_ConvToInt16(ap_args[1]));
        }
        else
        {
            CLI_disp_error("Not correct command arguments\n");
        }
    }


    return (CMD_COMPLETE);
}

/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_startvpip_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_startvpip_cmd(int a_nb_args, char ** ap_args)
{
    //t_uint32 reg_addr;
if (ap_args[1]==NULL) ap_args[1]="";    //For Linux, strcmp doesn't support NULL parameter
if (0== strcmp(ap_args[1],"help")) {
    CLI_disp_error("syntax: startvpip \n"); }
 else {
   if (a_nb_args == 1)
     { STARTVPIP();}
   else
     {CLI_disp_error("Not correct command arguments\n"); }
   }
   return CMD_COMPLETE;
}

/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_stopvpip_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_stopvpip_cmd(int a_nb_args, char ** ap_args)
{if (ap_args[1]==NULL) ap_args[1]="";   //For Linux, strcmp doesn't support NULL parameter
if (0== strcmp(ap_args[1],"help")) {
    CLI_disp_error("syntax: stopvpip \n"); }
 else {
   if (a_nb_args == 1)
     {STOPVPIP();}
   else
     {CLI_disp_error("Not correct command arguments\n"); }
   }
   return CMD_COMPLETE;
}

/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_sleepvpip_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
/*
CMD_COMPLETION C_ite_dbg_sleepvpip_cmd(int a_nb_args, char ** ap_args)
{if (0== strcmp(ap_args[1],"help")) {
    CLI_disp_error("syntax: sleepvpip \n"); }
 else {
   if (a_nb_args == 1)
     {SLEEPVPIP();}
   else
     {CLI_disp_error("Not correct command arguments\n"); }
   }
   return CMD_COMPLETE;
}
*/


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_wakeupvpip_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
/*
CMD_COMPLETION C_ite_dbg_wakeupvpip_cmd(int a_nb_args, char ** ap_args)
{if (0== strcmp(ap_args[1],"help")) {
    CLI_disp_error("syntax: wakeupvpip \n"); }
 else {
   if (a_nb_args == 1)
     {WAKEUPVPIP();}
   else
     {CLI_disp_error("Not correct command arguments\n"); }
   }
   return CMD_COMPLETE;
}
*/

/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_startgrab_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_startgrab_cmd(int a_nb_args, char ** ap_args)
{
    //t_uint32 reg_addr;
if (ap_args[1]==NULL) ap_args[1]="";    //For Linux, strcmp doesn't support NULL parameter
if (0== strcmp(ap_args[1],"help")) {
    CLI_disp_error("syntax: startgrab <LR/HR> <nbframe> \n"); }
 else {
   if (0== strcmp(ap_args[1],"LR")) {
       if (a_nb_args == 2)
         {   ITE_StartGrabNb(GRBPID_PIPE_LR,255);}
       if (a_nb_args == 3)
         {   ITE_StartGrabNb(GRBPID_PIPE_LR,ITE_ConvToInt16(ap_args[2]));}
       }
   if (0== strcmp(ap_args[1],"HR")) {
       if (a_nb_args == 2)
         {   ITE_StartGrabNb(GRBPID_PIPE_HR,255);}
       if (a_nb_args == 3)
         {   ITE_StartGrabNb(GRBPID_PIPE_HR,ITE_ConvToInt16(ap_args[2]));}
       }
   }
   return CMD_COMPLETE;
}

/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_stopgrab_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_stopgrab_cmd(int a_nb_args, char ** ap_args)
{
    //t_uint32 reg_addr;
    UNUSED(a_nb_args);
    
if (ap_args[1]==NULL) ap_args[1]="";    //For Linux, strcmp doesn't support NULL parameter
if (0== strcmp(ap_args[1],"help")) {
    CLI_disp_error("syntax: stopgrab <LR/HR> \n"); }
 else {
   if (0== strcmp(ap_args[1],"LR")) {
       if(ITE_IsGrabRunning(GRBPID_PIPE_LR)) {
          ITE_StopGrab(GRBPID_PIPE_LR);
          ITE_WaitEvent(ITE_EVT_GRABLR_EVENTS);}
       }
   if (0== strcmp(ap_args[1],"HR")) {
       if(ITE_IsGrabRunning(GRBPID_PIPE_HR)) {
          ITE_StopGrab(GRBPID_PIPE_HR);
          ITE_WaitEvent(ITE_EVT_GRABHR_EVENTS);}
       }
   }
   return CMD_COMPLETE;
}

/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_enablepipe_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_enablepipe_cmd(int a_nb_args, char ** ap_args)
{if (ap_args[1]==NULL) ap_args[1]="";   //For Linux, strcmp doesn't support NULL parameter
if (0== strcmp(ap_args[1],"help")) {
    CLI_disp_error("syntax: enablepipe <LR/HR> \n"); }
 else {
   if (a_nb_args == 2) {
       if (0== strcmp(ap_args[1],"LR"))
               ITE_ActivePipe(GRBPID_PIPE_LR, 1);
       if (0== strcmp(ap_args[1],"HR"))
        ITE_ActivePipe(GRBPID_PIPE_HR, 1);
           }
   else
     {CLI_disp_error("Not correct command arguments\n"); }
   }
   return CMD_COMPLETE;
}

/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_testverifbms_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
/*
CMD_COMPLETION C_ite_dbg_testverifbms_cmd(int a_nb_args, char ** ap_args)
{if (ap_args[1]==NULL) ap_args[1]="";   //For Linux, strcmp doesn't support NULL parameter
if (0== strcmp(ap_args[1],"help")) {
    CLI_disp_error("syntax: testverifbms <test_name> <test_options>\n"); }
 else {
   if (a_nb_args == 3)
      {ITE_dbg_veriftestbms(ap_args[1], ap_args[2]);}
   else
      {CLI_disp_error("Not correct command arguments\n"); }
   }
   return CMD_COMPLETE;
}
*/
/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_testverifbml_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
/*
CMD_COMPLETION C_ite_dbg_testverifbml_cmd(int a_nb_args, char ** ap_args)
{if (ap_args[1]==NULL) ap_args[1]="";   //For Linux, strcmp doesn't support NULL parameter
if (0== strcmp(ap_args[1],"help")) {
    CLI_disp_error("syntax: testverifbml <test_name> <test_options>\n"); }
 else {
   if (a_nb_args == 3)
      {ITE_dbg_veriftestbml(ap_args[1], ap_args[2]);}
   else
      {CLI_disp_error("Not correct command arguments\n"); }
   }
   return CMD_COMPLETE;
}
*/
/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_testveriflrhr_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
/*
CMD_COMPLETION C_ite_dbg_testveriflrhr_cmd(int a_nb_args, char ** ap_args)
{if (0== strcmp(ap_args[1],"help")) {
    CLI_disp_error("syntax: testveriflrhr <test_name> <test_options>\n"); }
 else {
   if (a_nb_args == 3)
      {ITE_dbg_veriftestlrhr(ap_args[1], ap_args[2]);}
   else
      {CLI_disp_error("Not correct command arguments\n"); }
   }
   return CMD_COMPLETE;
}
*/

/********************************************************/
/*    USECASE/ENV COMMAND               */
/********************************************************/

/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_startenv_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_startenv_cmd(int a_nb_args, char ** ap_args)
{if (ap_args[1]==NULL) ap_args[1]="";   //For Linux, strcmp doesn't support NULL parameter
if (0== strcmp(ap_args[1],"help")) {
    CLI_disp_error("syntax: startenv <test_name> <test_options>\n"); }
 else {
   if (a_nb_args == 3)
      {ITE_dbg_startenv(ap_args[1], ap_args[2]);}
   else
      {CLI_disp_error("Not correct command arguments\n"); }
   }
   return CMD_COMPLETE;
}

/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_loadfw_cmd
   PURPOSE  : To load Firmware
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_loadfw_cmd(int a_nb_args, char ** ap_args)
{
    if (ap_args [1] == NULL)
        ap_args [1] = "";   //For Linux, strcmp doesn't support NULL parameter

    if (0 == strcmp (ap_args [1], "help")) {
     CLI_disp_error ("syntax: loadFW\n");
    } else {
        if (a_nb_args == 1) {
          //ap_args[1], ap_args[2] are not right arguments to pass because they dont exist.
                ITE_dbg_loadfw (ap_args[1], ap_args[2]);
        } else {
             CLI_disp_error ("Not correct command arguments\n");
     }
    }

    return CMD_COMPLETE;
}

/********************************************************/
/*    USECASE/ENV COMMAND               */
/********************************************************/

/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_startenv_smiapp_smiapp_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_startenv_smiapp_cmd(int a_nb_args, char ** ap_args)
{if (ap_args[1]==NULL) ap_args[1]=""; //For Linux, strcmp doesn't support NULL parameter
if (0== strcmp(ap_args[1],"help")) {
    CLI_disp_error("syntax: startenv <test_name> <test_options>\n"); }
 else {
   if (a_nb_args == 3)
      {ITE_dbg_startenv_smiapp(ap_args[1], ap_args[2]);}
   else
      {CLI_disp_error("Not correct command arguments\n"); }
   }
   return CMD_COMPLETE;
}

/********************************************************/
/*    USECASE/ENV COMMAND               */
/********************************************************/

/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_startenv_smiapp_smiapp_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_startenv_SENSOR_TUNNING_cmd(int a_nb_args, char ** ap_args)
{if (ap_args[1]==NULL) ap_args[1]="";   //For Linux, strcmp doesn't support NULL parameter
if (0== strcmp(ap_args[1],"help")) {
    CLI_disp_error("syntax: startenv <test_name> <test_options>\n"); }
 else {
   if (a_nb_args == 3)
      {ITE_dbg_startenv_SENSOR_TUNNING(ap_args[1], ap_args[2]);}
   else
      {CLI_disp_error("Not correct command arguments\n"); }
   }
   return CMD_COMPLETE;
}
/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_stopenv_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_stopenv_cmd(int a_nb_args, char ** ap_args)
{if (ap_args[1]==NULL) ap_args[1]="";   //For Linux, strcmp doesn't support NULL parameter
if (0== strcmp(ap_args[1],"help")) {
    CLI_disp_error("syntax: stopenv\n"); }
 else {
   if (a_nb_args == 1)
      {ITE_dbg_stopenv();}
   else
      {CLI_disp_error("Not correct command arguments\n"); }
   }
   return CMD_COMPLETE;
}

/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_setusecase_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_setusecase_cmd(int a_nb_args, char ** ap_args)
{

    int ret;
    if (ap_args[1]==NULL)
        ap_args[1]="";   //For Linux, strcmp doesn't support NULL parameter

    if (0== strcmp(ap_args[1],"help"))
    {
        CLI_disp_error("syntax: setusecase <LR/HR/BMS> <SIZE> <FORMAT> \n");
        CLI_disp_error("syntax: <SIZE>  : {SUBQCIF SUBQCIFPLUS QCIF QCIFPLUS CIF\n");
        CLI_disp_error("           QQVGA QVGA QVGAPLUS VGA VGAPLUS}\n");
        CLI_disp_error("           NTSC PAL SVGA XGA SXGA UXGA 3MEGA 4MEGA 5MEGA 8MEGA}\n");
        CLI_disp_error(" (For doing BMS in any specific sensor mode ) MODE0, MODE1, MODE2, ... ,MODEn}\n");
        CLI_disp_error("syntax: <FORMAT>: {YUV422RI YUV420RP_I420 YUV420RP_YU12 YUV422MB YUV420MB_F YUV420MB_D\n");
        CLI_disp_error("           RGB30 RAW8 RAW12 ARGB4444 ARGB1555 RGB565 RGB888 ARGB8888}\n");
        CLI_disp_error("syntax: setusecase default\n");
    }
    else if (0 == ITE_stricmp(ap_args[1],"default"))
    {
        ret =ITE_dbg_setusecase(ap_args[1], 0, 0);
        if(ret != EOK)
        {
            CLI_disp_error("Not correct command arguments\n");
            goto END;
        }

    }
    else
    {
        if (a_nb_args == 4)
        {
            ret = ITE_dbg_setusecase(ap_args[1], ap_args[2], ap_args[3]);
            if(ret != EOK)
            {
                CLI_disp_error("Not correct command arguments\n");
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
    return CMD_COMPLETE;
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_bmsmode_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_bmsmode_cmd(int a_nb_args, char ** ap_args)
{if (ap_args[1]==NULL) ap_args[1]="";   //For Linux, strcmp doesn't support NULL parameter
if (0== strcmp(ap_args[1],"help")) {
    CLI_disp_error("syntax: bmsmode <MODE>\n");
    CLI_disp_error("syntax: <MODE>  : {STILL VIDEO}\n");
    }
 else {
   if (a_nb_args == 2)
      {ITE_dbg_bmsmode(ap_args[1]);}
   else
      {CLI_disp_error("Not correct command arguments\n"); }
   }
   return CMD_COMPLETE;
}

/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_bmsout_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_bmsout_cmd(int a_nb_args, char ** ap_args)
{if (ap_args[1]==NULL) ap_args[1]="";   //For Linux, strcmp doesn't support NULL parameter
if (0== strcmp(ap_args[1],"help")) {
    CLI_disp_error("syntax: bmsout <output>\n");
    CLI_disp_error("syntax: <output>  : {BAYERSTORE0 BAYERSTORE2}\n");
    }
 else {
   if (a_nb_args == 2)
      {ITE_dbg_bmsout(ap_args[1]);}
   else
      {CLI_disp_error("Not correct command arguments\n"); }
   }
   return CMD_COMPLETE;
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_bmlin_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_bmlin_cmd(int a_nb_args, char ** ap_args)
{if (ap_args[1]==NULL) ap_args[1]="";   //For Linux, strcmp doesn't support NULL parameter
if (0== strcmp(ap_args[1],"help")) {
    CLI_disp_error("syntax: bmlin <INPUT>\n");
    CLI_disp_error("syntax: <INPUT>  : {BAYERLOAD0 BAYERLOAD1 RGBLOAD}\n");
    }
 else {
   if (a_nb_args == 2)
      {ITE_dbg_bmlin(ap_args[1]);}
   else
      {CLI_disp_error("Not correct command arguments\n"); }
   }
   return CMD_COMPLETE;
}

/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_getusecase_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_getusecase_cmd(int a_nb_args, char ** ap_args)
{if (ap_args[1]==NULL) ap_args[1]="";   //For Linux, strcmp doesn't support NULL parameter
if (0== strcmp(ap_args[1],"help")) {
    CLI_disp_error("syntax: getusecase\n"); }
 else {
   if (a_nb_args == 1)
      {ITE_dbg_getusecase();}
   else
      {CLI_disp_error("Not correct command arguments\n"); }
 }
   return CMD_COMPLETE;
}

/********************************************************/
/*    SEQUENCE MODE COMMAND             */
/********************************************************/

/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_preview_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_preview_cmd(int a_nb_args, char ** ap_args)
{if (ap_args[1]==NULL) ap_args[1]="";   //For Linux, strcmp doesn't support NULL parameter
if (0== strcmp(ap_args[1],"help")) {
    CLI_disp_error("syntax: preview <still/video> <prepare/start/stop/free>\n"); }
 else {
   if (a_nb_args == 3)
      {ITE_dbg_preview(ap_args[1],ap_args[2],"255");}
   else
   if (a_nb_args == 4)
      {ITE_dbg_preview(ap_args[1],ap_args[2],ap_args[3]);}
   else
      {CLI_disp_error("Not correct command arguments\n"); }
   }
   return CMD_COMPLETE;
}
/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_still_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_still_cmd(int a_nb_args, char ** ap_args)
{if (ap_args[1]==NULL) ap_args[1]="";   //For Linux, strcmp doesn't support NULL parameter
if (0== strcmp(ap_args[1],"help")) {
    CLI_disp_error("syntax: still <prepare/start/stop/free>\n"); }
 else {
   if (a_nb_args == 2)
      {ITE_dbg_still(ap_args[1]);}
   else
      {CLI_disp_error("Not correct command arguments\n"); }
   }
   return CMD_COMPLETE;
}
/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_video_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_video_cmd(int a_nb_args, char ** ap_args)
{if (ap_args[1]==NULL) ap_args[1]="";   //For Linux, strcmp doesn't support NULL parameter
if (0== strcmp(ap_args[1],"help")) {
    CLI_disp_error("syntax: video <prepare/start/stop/free>\n"); }
 else {
   if (a_nb_args == 2)
      {ITE_dbg_video(ap_args[1]);}
   else
      {CLI_disp_error("Not correct command arguments\n"); }
   }
   return CMD_COMPLETE;
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_LR_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_LR_cmd(int a_nb_args, char ** ap_args)
{

  if (ap_args[1]==NULL) ap_args[1]="";  //For Linux, strcmp doesn't support NULL parameter

  if (0== strcmp(ap_args[1],"help"))
  {
    CLI_disp_error("syntax: LR <prepare/start/stop/free>\n");
  }
 else
 {
       if (a_nb_args == 2)
       {

        ITE_dbg_LR(ap_args[1]);


       }
       else
       {CLI_disp_error("Not correct command arguments\n"); }
  }
   return CMD_COMPLETE;
}

/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_HR_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_HR_cmd(int a_nb_args, char ** ap_args)
{

 if (ap_args[1]==NULL) ap_args[1]="";   //For Linux, strcmp doesn't support NULL parameter

 if (0== strcmp(ap_args[1],"help"))
 {
   CLI_disp_error("syntax: HR <prepare/start/stop/free>\n");
 }
 else
 {
     if (a_nb_args == 2)
     {

      ITE_dbg_HR(ap_args[1]);

     }
     else
     {CLI_disp_error("Not correct command arguments\n"); }
  }
   return CMD_COMPLETE;
}

/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_LRHR_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_LRHR_cmd(int a_nb_args, char ** ap_args)
{

 if (ap_args[1]==NULL) ap_args[1]="";   //For Linux, strcmp doesn't support NULL parameter

 if (0== strcmp(ap_args[1],"help"))
 {
   CLI_disp_error("syntax: LRHR <prepare/start/stop/free>\n");
 }
 else
 {
     if (a_nb_args == 2)
     {

      ITE_dbg_LRHR(ap_args[1]);

     }
     else
     {CLI_disp_error("Not correct command arguments\n"); }
  }
   return CMD_COMPLETE;
}

/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_bms_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */

CMD_COMPLETION C_ite_dbg_bms_cmd(int a_nb_args, char ** ap_args)
{
    int nbofframe=1;

    if (ap_args[1]==NULL)
    {
        ap_args[1]="";   //For Linux, strcmp doesn't support NULL parameter
    }
    else
    {
   if (a_nb_args == 2)
        {
            if (0== strcmp(ap_args[1],"help"))
            {
                CLI_disp_error("syntax: bms <prepare/start/stop/free>\n");
                CLI_disp_error("syntax: bms <start> [toggle/notoggle][wait/nowait]\n");
            }
            else
            {
                ITE_dbg_bms(ap_args[1]);
            }
        }
        else if (a_nb_args == 3)
        {
            if(0==(ITE_stricmp(ap_args[2],"wait")))
            {
                ITE_BMS_Start_Extention(nbofframe,ap_args[2],0);
            }
            else if((0== ITE_stricmp(ap_args[2],"toggle")))
            {
                ITE_BMS_Start_Extention(nbofframe,0,ap_args[1]);
            }
   else
            {
                ITE_BMS_Start_Extention(nbofframe,ap_args[1],ap_args[1]);
            }
        }
        else if (a_nb_args == 4)
        {
            ITE_BMS_Start_Extention(nbofframe,ap_args[2],ap_args[3]);
        }
        else
        {
            CLI_disp_error("Not correct command arguments\n");
        }
   }

   return CMD_COMPLETE;
}

/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_bml_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */

CMD_COMPLETION C_ite_dbg_bml_cmd(int a_nb_args, char ** ap_args)
{if (ap_args[1]==NULL) ap_args[1]="";   //For Linux, strcmp doesn't support NULL parameter
if (0== strcmp(ap_args[1],"help")) {
    CLI_disp_error("syntax: bml <prepare/start/stop/free>\n"); }
 else {
   if (a_nb_args == 2)
      {ITE_dbg_bml(ap_args[1]);}
   else if (a_nb_args == 3)
      {
	ITE_dbg_bml_stripe(ap_args[1], ap_args[2]);
      }
   else
      {CLI_disp_error("Not correct command arguments\n"); }
   }
   return CMD_COMPLETE;
}


/********************************************************/
/*    DISPLAY COMMAND                   */
/********************************************************/

/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_dispbuf_cmd
   PURPOSE  :  display buffer LR or HR on LCD
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_dispbuf_cmd(int a_nb_args, char ** ap_args)
{if (ap_args[1]==NULL) ap_args[1]="";   //For Linux, strcmp doesn't support NULL parameter
if (0== strcmp(ap_args[1],"help")) {
    CLI_disp_error("syntax: display <LR/HR> optional: <1 for single step/255 for infinity(by default)>\n"); }
 else {
   if (a_nb_args == 2)    // by default Infinity mode
      {ITE_dbg_dispbuf(ap_args[1],255);}
   else  if (a_nb_args == 3)
      {ITE_dbg_dispbuf(ap_args[1],ITE_ConvToInt16(ap_args[2]));}
      else
      {CLI_disp_error("Not correct command arguments\n"); }
   }
   return CMD_COMPLETE;
}

/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_initpbuf_cmd
   PURPOSE  :  Initialise "VGA" buffer LR or HR on LCD
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_initbuf_cmd(int a_nb_args, char ** ap_args)
{if (ap_args[1]==NULL) ap_args[1]="";   //For Linux, strcmp doesn't support NULL parameter
if (0== strcmp(ap_args[1],"help")) {
    CLI_disp_error("syntax: initbuf <LR/HR>\n"); }
 else {
   if (a_nb_args == 2)
      {ITE_dbg_initbuf(ap_args[1]);}
   else
      {CLI_disp_error("Not correct command arguments\n"); }
   }
   return CMD_COMPLETE;
}

/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_cleanbuf_cmd
   PURPOSE  :  Initialise buffer LR or HR or BMS With 0 value
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_cleanbuf_cmd(int a_nb_args, char ** ap_args)
{if (ap_args[1]==NULL) ap_args[1]="";   //For Linux, strcmp doesn't support NULL parameter
if (0== strcmp(ap_args[1],"help")) {
    CLI_disp_error("syntax: cleanbuf <LR/HR/BMS>\n"); }
 else {
   if (a_nb_args == 2)
      {ITE_dbg_cleanbuf(ap_args[1]);}
   else
      {CLI_disp_error("Not correct command arguments\n"); }
   }
   return CMD_COMPLETE;
}

/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_InitLCDBuf_cmd
   PURPOSE  : create or recreate a LCD Buffer with specific size and format
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_initlcdbuf_cmd(int a_nb_args, char ** ap_args)
{
    int ret= EOK;
    if (ap_args[1]==NULL) ap_args[1]="";   //For Linux, strcmp doesn't support NULL parameter
    if (0== strcmp(ap_args[1],"help"))
    {
        CLI_disp_error("syntax: initlcdbuf <size> <format> \n");
        CLI_disp_error("syntax: <SIZE>  : {SUBQCIF SUBQCIFPLUS QCIF QCIFPLUS CIF\n");
        CLI_disp_error("           QQVGA QVGA QVGAPLUS VGA}\n");
        CLI_disp_error("syntax: <FORMAT>: {YUV422RI ARGB4444 ARGB1555 RGB565 RGB888 ARGB8888}\n");
    }
    else
    {
        if (a_nb_args == 3)
        {
            ret = ITE_dbg_initlcdbuf(ap_args[1],ap_args[2]);
            if(ret != EOK)
            {
                CLI_disp_error("Not correct command arguments C_ite_dbg_initlcdbuf_cmd\n");
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
    return CMD_COMPLETE;
}

/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_showgrabuf_cmd
   PURPOSE  : software convertion and display on LCD RGB888
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_showgrabuf_cmd(int a_nb_args, char ** ap_args)
{if (ap_args[1]==NULL) ap_args[1]="";   //For Linux, strcmp doesn't support NULL parameter
if (0== strcmp(ap_args[1],"help")) {
    CLI_disp_error("syntax: showgrabbuf <pipe> \n");}
 else {
   if (a_nb_args == 2)
   {
   #if !(defined(__PEPS8500_SIA) || defined(_SVP_))
        ITE_dbg_showgrabuf(ap_args[1]);
   #endif
      }
   else
      {CLI_disp_error("Not correct command arguments\n"); }
   }
   return CMD_COMPLETE;
}

/********************************************************/
/*    FRAMERATE COMMAND                 */
/********************************************************/

/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_autoframerate_cmd
   PURPOSE  : set framerate in auto mode args are: min and max fps
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_autoframerate_cmd(int a_nb_args, char ** ap_args)
{if (ap_args[1]==NULL) ap_args[1]="";   //For Linux, strcmp doesn't support NULL parameter
if (0== strcmp(ap_args[1],"help")) {
    CLI_disp_error("syntax: autofps <minfps> <maxfps>\n"); }
 else {
   if (a_nb_args == 3)
      {ITE_dbg_autoframerate(ap_args[1],ap_args[2]);}
   else
      {CLI_disp_error("Not correct command arguments\n"); }
   }
   return CMD_COMPLETE;
}

/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_manualframerate_cmd
   PURPOSE  : set framerate in manual mode args are: desired fps
   ------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_manualframerate_cmd(
int     a_nb_args,
char    **ap_args)
{
    if (ap_args[1] == NULL)
    {
        ap_args[1] = "";    // For Linux, strcmp doesn't support NULL parameter
    }


    if (0 == strcmp(ap_args[1], "help"))
    {
        CLI_disp_error("syntax: manualfps <current_fps> NIL\n");
        CLI_disp_error("syntax: manualfps <current_fps> <max_fps>\n");
        CLI_disp_error("syntax: manualfps NIL  <max_fps>\n");
    }
    else
    {
        if (a_nb_args == 3)
        {
            ITE_dbg_manualframerate(ap_args[1], ap_args[2]);
        }
        else
        {
            CLI_disp_error("Not correct command arguments\n");
        }
    }


    return (CMD_COMPLETE);
}

/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_getframerate_cmd
   PURPOSE  : get framerate VPIP PE reported framerate and calculated fps
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_getframerate_cmd(int a_nb_args, char ** ap_args)
{
if (ap_args[1]==NULL) ap_args[1]="";// For Linux, strcmp doesn't support NULL parameter
if (0== strcmp(ap_args[1],"help")) {
    CLI_disp_error("syntax: getfps LR/HR \n"); }
 else {
   if (a_nb_args == 2)
      {ITE_dbg_getframerate(ap_args[1]);}
   else
      {CLI_disp_error("Not correct command arguments\n"); }
   }
   return CMD_COMPLETE;
}
/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_testframerate_cmd
   PURPOSE  : get framerate VPIP PE reported framerate and calculated fps
   ------------------------------------------------------------------------ */

CMD_COMPLETION C_ite_dbg_testframerate_cmd(int a_nb_args, char ** ap_args)
{
    int ret=EOK;
    if (0 == strcmp(ap_args[1],"help"))
    {
        CLI_disp_error("syntax: testframerate <pipe><resolution><format>\n");
    }
    else
    {
        if (a_nb_args == 4)
        {
            ret = ITE_dbg_testframerate(ap_args[1],ap_args[2],ap_args[3]);
            if(ret != EOK)
            {
                CLI_disp_error("Not correct command arguments in ITE_dbg_testframerate\n");
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
    return CMD_COMPLETE;
}

/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_recordtime_cmd
   PURPOSE  : record and log system time
   ------------------------------------------------------------------------ */
#ifdef TEST_PERFORMANCE
CMD_COMPLETION C_ite_dbg_recordtime_cmd(int a_nb_args, char ** ap_args)
{

 if (0 == strcmp(ap_args[1],"help"))
  {
 CLI_disp_error("syntax: recordtime <START/STOP><usecase><comment><tag>\n");
  }
 else {


        if (a_nb_args == 5)
        {
         ITE_dbg_recordtime(ap_args[1],ap_args[2],ap_args[3],ap_args[4]);
        }
        else if (a_nb_args == 4)
        {
         ITE_dbg_recordtime(" ",ap_args[1],ap_args[2],ap_args[3]);
        }
        else if (a_nb_args == 3)
        {
         ITE_dbg_recordtime(" "," ",ap_args[1],ap_args[2]);
        }
   else
        {
       CLI_disp_error("Not correct command arguments\n");
        }
   }
   return CMD_COMPLETE;
}
#endif


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_colorbartest_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_colorbartest_cmd(int a_nb_args, char ** ap_args)
{if (ap_args[1]==NULL) ap_args[1]="";   //For Linux, strcmp doesn't support NULL parameter
if (0== strcmp(ap_args[1],"help")) {
    CLI_disp_error("syntax: colorbartest <LR/HR/BMS>\n"); }
 else {
   if (a_nb_args == 2)
      {ITE_dbg_colorbar_test(ap_args[1]);}
   else
      {CLI_disp_error("Not correct command arguments\n"); }
   }
   return CMD_COMPLETE;
}
/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_sensoraccess_cmd
   PURPOSE  : sensor access
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_sensoraccess_cmd(int a_nb_args, char ** ap_args)
{if (ap_args[1]==NULL) ap_args[1]="";   //For Linux, strcmp doesn't support NULL parameter
if (0== strcmp(ap_args[1],"help")) {
    CLI_disp_error("syntax: sensorwrite <sensor index> <data> <B>\n"); }
 else {
   if (a_nb_args == 3)
      {//ITE_dbg_sensoraccess(ap_args[1],ap_args[2]);
       ITE_I2CWriteWord2Sensor(ITE_ConvToInt16(ap_args[1]),ITE_ConvToInt16(ap_args[2]));}
   else if (a_nb_args == 4)
      {ITE_I2CWriteByte2Sensor(ITE_ConvToInt16(ap_args[1]),ITE_ConvToInt16(ap_args[2]));}
   else
      {CLI_disp_error("Not correct command arguments\n"); }
   }
   return CMD_COMPLETE;
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_sensorread_cmd
   PURPOSE  : sensor read
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_sensorread_cmd(int a_nb_args, char ** ap_args)
{if (ap_args[1]==NULL) ap_args[1]="";   //For Linux, strcmp doesn't support NULL parameter
if (0== strcmp(ap_args[1],"help")) {
    CLI_disp_error("syntax: sensorread  <index> <B>\n"); }
 else {
   if (a_nb_args == 2)
      {//ITE_dbg_sensorread(ap_args[1]);
      LOS_Log("sensor data = %d \n",ITE_I2CReadWordFromSensor(ITE_ConvToInt16(ap_args[1])));}
   else   if (a_nb_args == 3)
      {LOS_Log("sensor data = %d \n",ITE_I2CReadByteFromSensor(ITE_ConvToInt16(ap_args[1])));}
   else
      {CLI_disp_error("Not correct command arguments\n"); }
   }
   return CMD_COMPLETE;
}

/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_wait_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_wait_cmd(int a_nb_args, char ** ap_args)
{if (ap_args[1]==NULL) ap_args[1]="";   //For Linux, strcmp doesn't support NULL parameter
if (0== strcmp(ap_args[1],"help")) {
    CLI_disp_error("syntax: wait <sec>\n"); }
 else {
   if (a_nb_args == 2)
      {ITE_dbg_wait(ap_args[1]);}
   else
      {CLI_disp_error("Not correct command arguments\n"); }
   }
   return CMD_COMPLETE;
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_sensormode_cmd
   PURPOSE  :  put sensor in mode normal,colorbar,PN28....
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_sensormode_cmd(int a_nb_args, char ** ap_args)
{if (ap_args[1]==NULL) ap_args[1]="";   //For Linux, strcmp doesn't support NULL parameter
if (0== strcmp(ap_args[1],"help")) {
    CLI_disp_error("syntax: sensormode <mode>\n");
        CLI_disp_error("syntax: <mode>  : {NORMAL_MODE COLORBAR FADETOGREY_COLORBAR PN28 HORIZ_GREY_SCALE VERT_GREY_SCALE\n");
    CLI_disp_error("           SOLIDBAR_YELLOW SOLIDBAR_CYAN SOLIDBAR_MAGENTA SOLIDBAR_BLACK\n");
    CLI_disp_error("           SOLIDBAR_WHITE SOLIDBAR_RED SOLIDBAR_BLUE SOLIDBAR_GREEN }\n");}
 else {
   if (a_nb_args == 2)
      {ITE_dbg_sensormode_test(ap_args[1]);}
   else
      {CLI_disp_error("Not correct command arguments\n"); }
   }
   return CMD_COMPLETE;
}

/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_buffercheck_cmd
   PURPOSE  : check if buffer is Red,blue,green,cyan,yellow,black,white,magenta
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_buffercheck_cmd(int a_nb_args, char ** ap_args)
{if (ap_args[1]==NULL) ap_args[1]="";   //For Linux, strcmp doesn't support NULL parameter
if (0== strcmp(ap_args[1],"help")) {
    CLI_disp_error("syntax: buffercheck <LR/HR> <color>\n");
    CLI_disp_error("syntax: <mode>  : {YELLOW CYAN MAGENTA BLACK\n");
    CLI_disp_error("           WHITE RED BLUE GREEN }\n");}
 else {
   if (a_nb_args == 3)
      {ITE_dbg_buffercheck_test(ap_args[1],ap_args[2]);}
   else
      {CLI_disp_error("Not correct command arguments\n"); }
   }
   return CMD_COMPLETE;
}

/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_startstopstress_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_startstopstress_cmd(int a_nb_args, char ** ap_args)
{if (ap_args[1]==NULL) ap_args[1]="";   //For Linux, strcmp doesn't support NULL parameter
if (0== strcmp(ap_args[1],"help")) {
    CLI_disp_error("syntax: startstopstress\n");}
 else {
   if (a_nb_args == 1)
      {ITE_dbg_previewvideo_startstop_test();}
   else
      {CLI_disp_error("Not correct command arguments\n"); }
   }
   return CMD_COMPLETE;
}

/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_writebmp_cmd
   PURPOSE  : convert a buffer into bmp file
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_writebmp_cmd(int a_nb_args, char ** ap_args)
{if (ap_args[1]==NULL) ap_args[1]="";   //For Linux, strcmp doesn't support NULL parameter
if (0== strcmp(ap_args[1],"help")) {
    CLI_disp_error("syntax: writebmp <LR/HR/BMS> <filename> <pixelorder>\n");}
 else {
   if (a_nb_args == 3)
      {ITE_dbg_writebmp(ap_args[1],ap_args[2],"0");}
   if (a_nb_args == 4)
      {ITE_dbg_writebmp(ap_args[1],ap_args[2],ap_args[3]);}
   else
      {CLI_disp_error("Not correct command arguments\n"); }
   }
   return CMD_COMPLETE;
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_iris_cmd
   PURPOSE  : allow to close or open iris shutter
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_iris_cmd(int a_nb_args, char ** ap_args)
{
 UNUSED(a_nb_args);
if (ap_args[1]==NULL) ap_args[1]="";   //For Linux, strcmp doesn't support NULL parameter
if (0== strcmp(ap_args[1],"help")) {
    CLI_disp_error("syntax: iris <OPEN/CLOSE>\n");}
/* mle
 else if (0==strcmp(ap_args[1],"OPEN"))
      {ITE_Shutter(IrisOpen);}
 else if (0==strcmp(ap_args[1],"CLOSE"))
      {ITE_Shutter(IrisClose);}
*/
 else
      {CLI_disp_error("Not correct command arguments\n"); }
   return CMD_COMPLETE;
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_givergbvalue_cmd
   PURPOSE  : print rgb value from x,Y position into a picture buffer
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_givergbvalue_cmd(int a_nb_args, char ** ap_args)
{if (ap_args[1]==NULL) ap_args[1]="";   //For Linux, strcmp doesn't support NULL parameter
if (0== strcmp(ap_args[1],"help")) {
    CLI_disp_error("syntax: givergb <LR/HR/BMS> <Xpos> <Ypos>\n");}
 else {
   if (a_nb_args == 4)
      {ITE_dbg_givergbvalue(ap_args[1],ap_args[2],ap_args[3]);}
   else
      {CLI_disp_error("Not correct command arguments\n"); }
   }
   return CMD_COMPLETE;
}

/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_sleep_mode_xp70_cmd
   PURPOSE  : testsleep modes
   ------------------------------------------------------------------------ */

CMD_COMPLETION C_ite_dbg_sleep_mode_xp70_cmd(int a_nb_args, char ** ap_args)
{
  if (0 == strcmp(ap_args[1],"help"))
  {
    CLI_disp_error("syntax: sleepmode <modeno>\n");
  }
 else {
        if (a_nb_args == 2)
        {
          ITE_dbg_sleepxp70(ap_args[1]);
        }
      else
        {
       CLI_disp_error("Not correct command arguments\n");
        }
   }
   return CMD_COMPLETE;
}

/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_uagemode_cmd
   PURPOSE  : Test different LLCD usage mode as per use case
   ------------------------------------------------------------------------ */

CMD_COMPLETION C_ite_dbg_uagemode_cmd(int a_nb_args, char ** ap_args)
{
    if (0 == strcmp(ap_args[1],"help"))
    {
        CLI_disp_error("syntax: usagemode <vf/af/still/stillnight/stillseq/video/videonight/videohq/videohs>\n");
    }
    else {
        if (a_nb_args == 2)
        {
            ITE_dbg_usagemode(ap_args[1]);
        }
        else
        {
            CLI_disp_error("Not correct command arguments\n");
        }
    }
    return CMD_COMPLETE;
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_mmio_test_cmd
   PURPOSE  : Test MMIO API's
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_mmio_test_cmd(int     a_nb_args,char    **ap_args)
{
    Result_te err = Result_e_Success;

    if (ap_args[1] == NULL)
    {
        ap_args[1] = "";
    }


    if (0 == strcmp(ap_args[1], "help"))
    {
        CLI_disp_error("syntax: sensorread  <index> <B>\n");
    }
    else
    {
        if (a_nb_args == 2)
        {
            err = ITE_PlatformMmioTest(ITE_ConvToInt16(ap_args[1]));
        }
        else if (a_nb_args == 1)
        {
            err = ITE_PlatformMmioTest(0);
        }
        else
        {
            CLI_disp_error("Not correct command arguments\n");
        }

        if (err != Result_e_Success)
        {
            LOS_Log("C_ite_dbg_mmio_test_cmd ITE_PlatformMmioTest ERROR %d:",err);
            ITE_NMF_ASSERT(err);
        }

    }


    return (CMD_COMPLETE);
}
/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_create_all_buffers_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_create_all_buffers_cmd(int a_nb_args, char ** ap_args)
{if (ap_args[1]==NULL) ap_args[1]="";   //For Linux, strcmp doesn't support NULL parameter
if (0== strcmp(ap_args[1],"help")) {
    CLI_disp_error("syntax: createbuffer <BufferName>\n"); }
 else {
   if (a_nb_args == 2)
      {ite_dbg_create_buffers(ap_args[1]);}
   else
      {CLI_disp_error("Not correct command arguments\n"); }
   }
   return CMD_COMPLETE;
}

/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_free_all_buffers_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_free_all_buffers_cmd(int a_nb_args, char ** ap_args)
{if (ap_args[1]==NULL) ap_args[1]="";   //For Linux, strcmp doesn't support NULL parameter
if (0== strcmp(ap_args[1],"help")) {
    CLI_disp_error("syntax: freebuffer <BufferName>\n"); }
 else {
   if (a_nb_args == 2)
      {ite_dbg_free_buffers(ap_args[1]);}
   else
      {CLI_disp_error("Not correct command arguments\n"); }
   }
   return CMD_COMPLETE;
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_rand_floatrange_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_rand_floatrange_cmd(int a_nb_args, char ** ap_args)
{
 float min_no,max_no,result;

if (ap_args[1]==NULL) ap_args[1]="";    //For Linux, strcmp doesn't support NULL parameter
if (0== strcmp(ap_args[1],"help"))
{
    CLI_disp_error("syntax: random <min_no> <max_no>\n");
}
else
 {
   if (a_nb_args == 3)
      {
        min_no = ITE_ConvToFloat(ap_args[1]);
        max_no = ITE_ConvToFloat(ap_args[2]);
        result = ite_rand_FloatRange(min_no,max_no);
        LOS_Log("OUTPUT NO :%f\n",result);
      }
   else
      {
       CLI_disp_error("Not correct command arguments\n");
      }
 }
   return CMD_COMPLETE;

}



float ite_rand_Range(float min_no, float max_no)
{

return (((max_no-min_no)*((float)rand()/RAND_MAX))+min_no);

}


int ite_rand_Range_Mul(int min_no, int max_no,int multiple)
{
int min_quotient,max_quotient,random_quotient;

if(min_no%multiple==0)
    {min_quotient=min_no/multiple;}
else
    {min_quotient=min_no/multiple+1;}

max_quotient=max_no/multiple;

random_quotient=((rand() * LOS_getSystemTime())%(max_quotient-min_quotient))+1;


return (random_quotient*multiple);

}

CMD_COMPLETION C_ite_dbg_set_SensorParams_cmd(int a_nb_args, char ** ap_args)
    {
     t_uint32 exp_time;
     t_uint16 a_gain;

    if (ap_args[1]==NULL) ap_args[1]="";    //For Linux, strcmp doesn't support NULL parameter
    if (0== strcmp(ap_args[1],"help"))
    {
        CLI_disp_error("syntax: set_sensor_param <default/exp_time> <default/a_gain>\n");
    }
    else
     {
       if (a_nb_args == 3)
          {
           if (0 != strcmp(ap_args[1],"default"))
           {
               exp_time=ITE_ConvToInt32(ap_args[1]);
           }
           else
           {
               exp_time = 0xFFFF;
           }

           if (0 != strcmp(ap_args[2],"default"))
           {
               a_gain = ITE_ConvToInt16(ap_args[2]);
           }
           else
           {
               a_gain = 0xFF;
           }


           ITE_NMF_UpdateAnalogGainAndExposureTime(a_gain, exp_time);
          }
       else
          {
           CLI_disp_error("Not correct command arguments\n");
          }
     }
       return CMD_COMPLETE;

    }



CMD_COMPLETION C_ite_dbg_rand_Range_cmd(int a_nb_args, char ** ap_args)
{
 float min_no,max_no,result,multiple;
 char result_random[100];
 
if (ap_args[1]==NULL) ap_args[1]="";    //For Linux, strcmp doesn't support NULL parameter
if (0== strcmp(ap_args[1],"help"))
{
    CLI_disp_error("syntax: gen_random_val <float/int> <min_no> <max_no> [multiple]\n");
}
else
 {
    if (a_nb_args == 4)
      {

        min_no = ITE_ConvToFloat(ap_args[2]);
        max_no = ITE_ConvToFloat(ap_args[3]);
        result = ite_rand_Range(min_no,max_no);

        if (0== strcmp(ap_args[1],"int"))
        {
            LOS_Log("RANDOM GENERATED VALUE :%d\n",(int)result);
            sprintf(result_random,"%d",(int)result);
        }
        if (0== strcmp(ap_args[1],"float"))
        {
            LOS_Log("RANDOM GENERATED VALUE :%f\n",result);
            sprintf(result_random,"%f",result);
        }
        CLI_add_alias ("RANDOM_GENERATED_VALUE",result_random,1);

      }

   else if (a_nb_args == 5)
      {

        min_no = ITE_ConvToFloat(ap_args[2]);
        max_no = ITE_ConvToFloat(ap_args[3]);
        multiple = ITE_ConvToFloat(ap_args[4]);
        result = ite_rand_Range_Mul(min_no,max_no,multiple);

        if (0== strcmp(ap_args[1],"int"))
        {
            LOS_Log("RANDOM GENERATED VALUE :%d\n",(int)result);
            sprintf(result_random,"%d",(int)result);
        }
        if (0== strcmp(ap_args[1],"float"))
        {
            LOS_Log("RANDOM GENERATED VALUE :%f\n",result);
            sprintf(result_random,"%f",result);
        }
        CLI_add_alias ("RANDOM_GENERATED_VALUE",result_random,1);

      }
   else
      {
       CLI_disp_error("Not correct command arguments\n");
      }
 }
   return CMD_COMPLETE;

}

CMD_COMPLETION C_ite_dbg_SetRxTestPattern_cmd(int     a_nb_args, char    **ap_args)
{

    LOS_Log("ENTER Rx Test Pattern Command\n");

    LOS_Log("The Number of Aurguments for Rx Test Pattern  are %x\n", a_nb_args);

    if (ap_args[1]==NULL) ap_args[1]="";    //For Linux, strcmp doesn't support NULL parameter
    if (0== strcmp(ap_args[1],"help"))
    {
       CLI_disp_error("syntax: SetRXTestPattern <RxTestPattern>,<RED>,<GR>,<BLUE>,<BG> \n");
    }
    else
    {
        if((a_nb_args != 2) && ( a_nb_args != 6))
        {
           CLI_disp_error("Not correct command arguments\n");
        }
        else
        {
            if(a_nb_args == 2)
            {
                ITE_RxTestPattern(ap_args[1],0,0,0,0);
            }
            else
            {
                ITE_RxTestPattern(ap_args[1],ap_args[2],ap_args[3],ap_args[4],ap_args[5]);
             }
        }
    }
    return CMD_COMPLETE;

}

CMD_COMPLETION C_ite_dbg_SetCursorPosition_cmd(int     a_nb_args, char    **ap_args)
{

    LOS_Log("ENTER SetCursorPosition\n");

    LOS_Log("The Number of Aurguments are %x\n", a_nb_args);

    if (ap_args[1]==NULL) ap_args[1]="";    //For Linux, strcmp doesn't support NULL parameter
    if (0== strcmp(ap_args[1],"help"))
    {
        CLI_disp_error("syntax: SetCursorPosition <Horz Pos in %><Horz Wd><Vert Pos in %><Vert Wd> \n");
    }
    else
    {
        if(a_nb_args != 5)
        {
           CLI_disp_error("Not correct command arguments\n");
        }
        else
        {
            ITE_RxCursorPosition(ap_args[1], ap_args[2], ap_args[3], ap_args[4]);
        }
    }
    return CMD_COMPLETE;
}


CMD_COMPLETION C_ite_dbg_SetInputModeSelection_cmd(int     a_nb_args, char    **ap_args)
{
    LOS_Log("ENTER SetInputModeSelection \n");

    LOS_Log("The Number of Aurguments are %x\n", a_nb_args);
    if (ap_args[1]==NULL) ap_args[1]="";    //For Linux, strcmp doesn't support NULL parameter
    if (0== strcmp(ap_args[1],"help"))
    {
        CLI_disp_error("syntax: SetInputModeSelection <Rx, Sensor> \n");
    }
    else
    {
        if(a_nb_args != 2)
        {
           CLI_disp_error("Not correct command arguments\n");
        }
        else
        {
            ITE_SetInputModeSelection(ap_args[1]);
        }
    }
    return CMD_COMPLETE;
}

CMD_COMPLETION C_ite_dbg_getresolution_cmd(int     a_nb_args, char    **ap_args)
{   char    resolutionName[16];
    e_resolution    random_int = SUBQCIF;

    if (ap_args[1]==NULL) ap_args[1]="";    //For Linux, strcmp doesn't support NULL parameter
    if (0== strcmp(ap_args[1],"help"))
    {
        CLI_disp_error("syntax: getresolution <resolution enum value>\n");
    }
    else
    {
        if(a_nb_args != 2)
        {
           CLI_disp_error("Not correct command arguments\n");
        }
        else
        {
        random_int=ITE_ConvToInt16(ap_args[1]);
        ITE_GiveStringNameFromEnum(random_int, resolutionName);
        CLI_add_alias ("RESOLUTION",resolutionName,1);
        }
    }
    return CMD_COMPLETE;

}

CMD_COMPLETION C_ite_dbg_Setresolution_cmd(int     a_nb_args, char    **ap_args)
{
     int ret = EOK;
    if (ap_args[1]==NULL) ap_args[1]="";    //For Linux, strcmp doesn't support NULL parameter
    if (0== strcmp(ap_args[1],"help"))
    {
        CLI_disp_error("syntax: Setresolution <LR/HR> <resolution> <format>\n");
    }
    else
    {
        if(a_nb_args != 4)
        {
           CLI_disp_error("Not correct command arguments\n");
        }
        else
        {
         ITE_ConvToUppercase (ap_args[2]);
         LOS_Log("resolution selected :%s\n",ap_args[2]);
           ret =  ITE_dbg_setutresolution(ap_args[1],ap_args[2],ap_args[3]);
            if(ret != EOK)
            {
                CLI_disp_error("Not correct command arguments C_ite_dbg_Setresolution_cmd\n");
                goto END;
            }

        }
    }
    END:
    return CMD_COMPLETE;

}

CMD_COMPLETION C_ite_dbg_ConvToUpper_cmd(int     a_nb_args, char    **ap_args)
{
    if (ap_args[1]==NULL) ap_args[1]="";    //For Linux, strcmp doesn't support NULL parameter
    if (0== strcmp(ap_args[1],"help"))
    {
        LOS_Log("syntax: convu <string>\n");
    }
    else
    {
        if(a_nb_args != 2)
        {
           LOS_Log("Not correct command arguments\n");
    }
    else
    {
            ITE_ConvToUppercase(ap_args[1]);
    }
    }
    return CMD_COMPLETE;

}



/***************************************************************************************/
/***************************************************************************************/
/***************************************************************************************/
/***************************************************************************************/
/***************************************************************************************/
/***************************************************************************************/
/* ------------------------------------------------------------------------------------
   FUNCTION : ITE_dbg_wait
   PURPOSE  :
   ------------------------------------------------------------------------------------ */
void ITE_dbg_wait(char * sec)
{
    LOS_Sleep(1000 * ITE_ConvToInt16(sec));
}
/* ------------------------------------------------------------------------------------
   FUNCTION : ITE_dbg_readPE
   PURPOSE  :
   ------------------------------------------------------------------------------------ */
void ITE_dbg_readPE(t_uint16 AddrPE)
{
LOS_Log("value read: %x\n",ITE_readPE(AddrPE), NULL, NULL, NULL, NULL, NULL);
}

/* ------------------------------------------------------------------------------------
   FUNCTION : ITE_dbg_writePE
   PURPOSE  :
   ------------------------------------------------------------------------------------ */
void ITE_dbg_writePE(t_uint16 AddrPE, t_uint32 data)
{
ITE_writePE(AddrPE, data);
}

/********************************************************/
/*    USECASE/ENV COMMAND               */
/********************************************************/

/* ------------------------------------------------------------------------------------
   FUNCTION : ITE_dbg_startenv
   PURPOSE  :
   ------------------------------------------------------------------------------------ */
void ITE_dbg_startenv(char * ap_test_id, char * ap_grabvpip_options)
{
   //disable log pageelement
   //ITE_Log_RW_pageelements_disable();
    CLI_disp_msg("after PE disable Number of allocted chunk with LOS_Alloc is %d\n", LOS_GetAllocatedChunk());
   /* HCL Services, alloc, Log, IT, xti ....*/
   CLI_disp_msg(" after Init Platform Number of allocted chunk with LOS_Alloc is %d\n", LOS_GetAllocatedChunk());
   ITE_InitUseCase(ap_test_id, ap_grabvpip_options,&usecase);
    CLI_disp_msg(" after Init Usecase Platform Number of allocted chunk with LOS_Alloc is %d\n", LOS_GetAllocatedChunk());
   ITE_Start_Env(&usecase,ap_grabvpip_options, 0,0);
/*
  do {
   ITE_Start_Env(&usecase,ap_grabvpip_options);
   ITE_LR_Prepare(&usecase,INFINITY);
   ITE_LR_Start(INFINITY);
   ITE_LR_Stop();
   LOS_Sleep(1000);
   ITE_LR_Free();
   ITE_Stop_Env();
  } while (1);
*/
}

/* ------------------------------------------------------------------------------------
   FUNCTION : ITE_dbg_startenv_smiapp
   PURPOSE  :
   ------------------------------------------------------------------------------------ */
void ITE_dbg_startenv_smiapp(char * ap_test_id, char * ap_grabvpip_options)
{
   //disable log pageelement
   //ITE_Log_RW_pageelements_disable();
    CLI_disp_msg("after PE disable Number of allocted chunk with LOS_Alloc is %d\n", LOS_GetAllocatedChunk());
   /* HCL Services, alloc, Log, IT, xti ....*/

   usecase.smiapp_powerUp_sequence = 1;
   ITE_InitUseCase(ap_test_id, ap_grabvpip_options,&usecase);
   CLI_disp_msg(" after Init Usecase Platform Number of allocted chunk with LOS_Alloc is %d\n", LOS_GetAllocatedChunk());

   ITE_Start_Env(&usecase,ap_grabvpip_options,1,0);
/*
  do {
   ITE_Start_Env(&usecase,ap_grabvpip_options);
   ITE_LR_Prepare(&usecase,INFINITY);
   ITE_LR_Start(INFINITY);
   ITE_LR_Stop();
   LOS_Sleep(1000);
   ITE_LR_Free();
   ITE_Stop_Env();
  } while (1);
*/
}

/* ------------------------------------------------------------------------------------
   FUNCTION : ITE_dbg_loadfw
   PURPOSE  :
   ------------------------------------------------------------------------------------ */
void ITE_dbg_loadfw(char * ap_test_id, char * ap_grabvpip_options)
{
    //disable log pageelement
    //ITE_Log_RW_pageelements_disable ();
    CLI_disp_msg ("after PE disable Number of allocted chunk with LOS_Alloc is %d\n", LOS_GetAllocatedChunk ());

    // HCL Services, alloc, Log, IT, xti ....
    CLI_disp_msg (" after Init Platform Number of allocted chunk with LOS_Alloc is %d\n", LOS_GetAllocatedChunk ());

    ITE_InitUseCase (ap_test_id, ap_grabvpip_options,&usecase);
    CLI_disp_msg (" after Init Usecase Platform Number of allocted chunk with LOS_Alloc is %d\n", LOS_GetAllocatedChunk ());

    LOS_Log (" Entering LoadFW \n");
    ITE_Load_FW (&usecase);
    CLI_disp_msg (" after Init LoadFW\n");
}

/* ------------------------------------------------------------------------------------
   FUNCTION : ITE_dbg_startenv_SENSOR_TUNNING
   PURPOSE  :
   ------------------------------------------------------------------------------------ */
void ITE_dbg_startenv_SENSOR_TUNNING(char * ap_test_id, char * ap_grabvpip_options)
{
    Result_te err = Result_e_Success;
    //disable log pageelement
    //ITE_Log_RW_pageelements_disable();
    CLI_disp_msg("after PE disable Number of allocted chunk with LOS_Alloc is %d\n", LOS_GetAllocatedChunk());
   /* HCL Services, alloc, Log, IT, xti ....*/

    err = ITE_InitBoard();


    if(err != Result_e_Success)
    {
        LOS_Log("ITE_dbg_startenv_SENSOR_TUNNING ITE_InitBoard ERROR %d:",err);
        ITE_NMF_ASSERT(err);
    }

    CLI_disp_msg(" after Init Platform Number of allocted chunk with LOS_Alloc is %d\n", LOS_GetAllocatedChunk());
   ITE_InitUseCase(ap_test_id, ap_grabvpip_options,&usecase);
    CLI_disp_msg(" after Init Usecase Platform Number of allocted chunk with LOS_Alloc is %d\n", LOS_GetAllocatedChunk());
   ITE_Start_Env(&usecase,ap_grabvpip_options, 0, 1);
}


/* ------------------------------------------------------------------------------------
   FUNCTION : ITE_dbg_stopenv
   PURPOSE  :
   ------------------------------------------------------------------------------------ */
void ITE_dbg_stopenv(void)
{
   ITE_Stop_Env();
}

/* ------------------------------------------------------------------------------------
   FUNCTION : ITE_dbg_setusecase
   PURPOSE  :
   ------------------------------------------------------------------------------------ */
int ITE_dbg_setusecase(char* ap_pipe, char* ap_size, char* ap_format)
{
    int ret = EOK;
    enum e_grabFormat afmt;
    char* pcSizeStr = NULL;

    if (0 == ITE_stricmp(ap_pipe, "LR"))
    {
        char LR_Resolution[100] = "";

        if (ITE_stricmp(ap_size, "MINRES") == 0)
        {
            ITE_GiveStringNameFromEnum(usecase.LR_Min_resolution, LR_Resolution);
            LOS_Log("LR_Video_Min resolution = %s.\n", LR_Resolution);
            pcSizeStr = LR_Resolution;
        }
        else if (ITE_stricmp(ap_size, "MAXRES") == 0)
        {
            ITE_GiveStringNameFromEnum(usecase.LR_Max_resolution, LR_Resolution);
            LOS_Log("LR_Video_Max resolution = %s.\n", LR_Resolution);
            pcSizeStr = LR_Resolution;
        }
        else
        {
            pcSizeStr = ap_size;
        }

        ret = ITE_GiveFormat(PIPE_LR, ap_format, &afmt);
        if (ret != EFMT)
        {
            usecase.LR_GrbFormat = afmt;
            ret = EOK;
        }
        else
        {
            LOS_Log("ITE_GiveFormat gave error in %s and line  %d\n",
                    __FUNCTION__, __LINE__);
            goto END;
        }

        usecase.LR_resolution = ITE_GiveResolution(pcSizeStr);
        usecase.LR_XSize = ITE_GiveXSize(PIPE_LR, pcSizeStr, ap_format);
        usecase.LR_YSize = ITE_GiveYSize(PIPE_LR, pcSizeStr, ap_format);

    }
    else if (0 == ITE_stricmp(ap_pipe,"HR"))
    {
        char HR_Resolution[100] = "";

        if (ITE_stricmp(ap_size, "VIDEOMAX") == 0)
        {
            ITE_GiveStringNameFromEnum(usecase.HR_Video_Max_resolution, HR_Resolution);
            LOS_Log("HR_Video_Max resolution = %s.\n", HR_Resolution);
            pcSizeStr = HR_Resolution;
        }
        else if (ITE_stricmp(ap_size, "STILLMAX") == 0)
        {
            ITE_GiveStringNameFromEnum(usecase.HR_Still_Max_resolution, HR_Resolution);
            LOS_Log("HR_Still_Max resolution = %s.\n", HR_Resolution);
            pcSizeStr = HR_Resolution;
        }
        else
        {
            pcSizeStr = ap_size;
        }

        ret = ITE_GiveFormat(PIPE_HR, ap_format, &afmt);
        if (ret != EFMT)
        {
            usecase.HR_GrbFormat = afmt;
            ret = EOK;
        }
        else
        {
            LOS_Log("ITE_GiveFormat gave error in %s and line  %d\n",
                    __FUNCTION__, __LINE__);
            goto END;
        }
        usecase.HR_resolution = ITE_GiveResolution(pcSizeStr);
        usecase.HR_XSize = ITE_GiveXSize(PIPE_HR, pcSizeStr, ap_format);
        usecase.HR_YSize = ITE_GiveYSize(PIPE_HR, pcSizeStr, ap_format);

    }
    else if (0 == ITE_stricmp(ap_pipe, "BMS"))
    {
        Sensor_Output_Mode_ts* p_Sensor_Output_Mode_Data = NULL;

        ret = ITE_GiveFormat(PIPE_RAW, ap_format, &afmt);
        if (ret != EFMT)
        {
            usecase.BMS_GrbFormat = afmt;
            ret = EOK;
        }
        else
        {
            LOS_Log("ITE_GiveFormat gave error in %s and line  %d\n",
                    __FUNCTION__, __LINE__);
            goto END;
        }

        p_Sensor_Output_Mode_Data = (Sensor_Output_Mode_ts*) pOutpuModeBuffer->logAddress;
        t_uint32 sensorOutputModeDataSize = ITE_readPE(ReadLLAConfig_Status_u32_sensor_Output_Mode_data_size_Byte0);
        t_uint32 sensorOutputModeSturctSize = sizeof(Sensor_Output_Mode_ts);

        if (0 == ITE_strincmp(ap_size, "MODE", 4))
        {
            unsigned int modevalue = 0;

            sscanf(ap_size, "%*s%d", &modevalue);

            if (modevalue < (sensorOutputModeDataSize / sensorOutputModeSturctSize))
            {
                usecase.BMS_woi_X = p_Sensor_Output_Mode_Data[modevalue].u32_woi_res_width;
                usecase.BMS_woi_Y = p_Sensor_Output_Mode_Data[modevalue].u32_woi_res_height;
                usecase.BMS_YSize = p_Sensor_Output_Mode_Data[modevalue].u32_output_res_height;
                usecase.BMS_XSize = p_Sensor_Output_Mode_Data[modevalue].u32_output_res_width;
                usecase.framerate_x100 = p_Sensor_Output_Mode_Data[modevalue].u32_max_frame_rate_x100;
                usecase.data_format = p_Sensor_Output_Mode_Data[modevalue].u32_data_format;
            }
            else
            {
                LOS_Log("Error on Mode value !!!  mode value %d outputmode %d\n",
                        modevalue,
                        (sensorOutputModeDataSize / sensorOutputModeSturctSize));
            }
        }
        else
        {
            unsigned int uiIndex = 0;

            /* Handles the size as 'standard resultion' or 'Width X Height' */
            usecase.BMS_resolution = ITE_GiveResolution(ap_size);
            usecase.BMS_XSize = ITE_GiveXSize(PIPE_RAW, ap_size, ap_format);
            usecase.BMS_YSize = ITE_GiveYSize(PIPE_RAW, ap_size, ap_format);
            usecase.BMS_woi_X = usecase.BMS_XSize;
            usecase.BMS_woi_Y = usecase.BMS_YSize;

            /* Check if the requested size is valid for BMS or not */
            for (uiIndex = 0; uiIndex < (sensorOutputModeDataSize / sensorOutputModeSturctSize); uiIndex++)
            {
                if ((usecase.BMS_woi_X == p_Sensor_Output_Mode_Data[uiIndex].u32_woi_res_width) &&
                    (usecase.BMS_woi_Y == p_Sensor_Output_Mode_Data[uiIndex].u32_woi_res_height) &&
                    (usecase.BMS_XSize == p_Sensor_Output_Mode_Data[uiIndex].u32_output_res_width) &&
                    (usecase.BMS_YSize == p_Sensor_Output_Mode_Data[uiIndex].u32_output_res_height))
                {
                    /* Requested size matches with supported mode, valid BMS
                       size requested
                    */
                    break;
                }
                else
                {
                    uiIndex++;
                }
            }

            if (uiIndex == (sensorOutputModeDataSize / sensorOutputModeSturctSize))
            {
                /* Requested size doesn't match with any supported mode, invalid
                   BMS size requested.
                */
                LOS_Log("ITE_dbg_setusecase: Invalid BMS size %dX%d requested."
                        " Asserting!!!\n", usecase.BMS_XSize, usecase.BMS_YSize);
                ITE_NMF_ASSERT(NULL);
            }
        }
    }
    else if (0 == ITE_stricmp(ap_pipe, "default"))
    {
        char* mode = "none";

        ret = ITE_dbg_setusecase("LR", "VGA", "YUV422RI");
        if (ret != EOK)
        {
            LOS_Log("ITE_dbg_setusecase gave error in %s and line  %d\n",
                    __FUNCTION__, __LINE__);
            goto END;
        }

        ret = ITE_dbg_setusecase("HR", "VGA", "YUV420MB_D");
        if (ret != EOK)
        {
            LOS_Log("ITE_dbg_setusecase gave error in %s and line  %d\n",
                    __FUNCTION__, __LINE__);
            goto END;
        }

        ITE_dbg_bmsmode("STILL");
        ITE_dbg_bmsout("BAYERSTORE2");
        ret = ITE_dbg_setusecase("BMS", "MODE0", "RAW12");
        if (ret != EOK)
        {
            LOS_Log("ITE_dbg_setusecase gave error in %s and line  %d\n",
                    __FUNCTION__, __LINE__);
            goto END;
        }
        ITE_SetFrameRateControl(DEFAULT_FPS, DEFAULT_FPS);
        ITE_dbg_usagemode(mode);
        LOS_Log("USECASE FPS USAGEMODE  -- set to Default !!!  \n");
    }

END:
    return ret;
}


/* ------------------------------------------------------------------------------------
   FUNCTION : ITE_dbg_bmsmode
   PURPOSE  :
   ------------------------------------------------------------------------------------ */
void ITE_dbg_bmsmode(char * ap_format)
{
  if (0== strcmp(ap_format,"STILL"))
      usecase.BMS_Mode = BMSSTILL;
  if (0== strcmp(ap_format,"VIDEO"))
      usecase.BMS_Mode = BMSVIDEO;
}

/* ------------------------------------------------------------------------------------
   FUNCTION : ITE_dbg_bmsout
   PURPOSE  :
   ------------------------------------------------------------------------------------ */
void ITE_dbg_bmsout(char* ap_format)
{
    if (0 == strcmp(ap_format,"BAYERSTORE0"))
    {
        usecase.BMS_output = BAYERSTORE0;
    }
    else if (0 == strcmp(ap_format,"BAYERSTORE1"))
    {
        usecase.BMS_output = BAYERSTORE1;
        LOS_Log("Error Config not usable\n");
    }
    else if (0 == strcmp(ap_format,"BAYERSTORE2"))
    {
        usecase.BMS_output = BAYERSTORE2;
    }

    if ((usecase.BMS_output == BAYERSTORE0) && (usecase.BMS_GrbFormat == GRBFMT_RAW8))
    {
        usecase.BMS_GrbFormat = GRBFMT_FAST_RAW8;
    }
    else if ((usecase.BMS_output == BAYERSTORE0) && (usecase.BMS_GrbFormat == GRBFMT_RAW12))
    {
        usecase.BMS_GrbFormat = GRBFMT_FAST_RAW12;
    }
    else if ((usecase.BMS_output == BAYERSTORE2) && (usecase.BMS_GrbFormat == GRBFMT_FAST_RAW8))
    {
        usecase.BMS_GrbFormat = GRBFMT_RAW8;
    }
    else if ((usecase.BMS_output == BAYERSTORE2) && (usecase.BMS_GrbFormat == GRBFMT_FAST_RAW12))
    {
        usecase.BMS_GrbFormat = GRBFMT_RAW12;
    }
}

/* ------------------------------------------------------------------------------------
   FUNCTION : ITE_dbg_bmlin
   PURPOSE  :
   ------------------------------------------------------------------------------------ */
void ITE_dbg_bmlin(char* ap_format)
{
    if (0 == strcmp(ap_format,"BAYERLOAD1"))
    {
        usecase.BML_input = BAYERLOAD1;
    }
    else if (0 == strcmp(ap_format,"BAYERLOAD2"))
    {
        usecase.BML_input = BAYERLOAD2;
    }
    else if (0 == strcmp(ap_format,"RGBLOAD"))
    {
        usecase.BML_input = RGBLOAD;
    }

    if ((usecase.BML_input == BAYERLOAD1) && (usecase.BMS_GrbFormat == GRBFMT_RAW8))
    {
        usecase.BML_GrbFormat = GRBFMT_FAST_RAW8;
    }
    else if ((usecase.BML_input == BAYERLOAD1) && (usecase.BMS_GrbFormat == GRBFMT_RAW12))
    {
        usecase.BML_GrbFormat = GRBFMT_FAST_RAW12;
    }
    else if ((usecase.BML_input == BAYERLOAD2) && (usecase.BMS_GrbFormat == GRBFMT_FAST_RAW8))
    {
        usecase.BML_GrbFormat = GRBFMT_RAW8;
    }
    else if ((usecase.BML_input == BAYERLOAD2) && (usecase.BMS_GrbFormat == GRBFMT_FAST_RAW12))
    {
        usecase.BML_GrbFormat = GRBFMT_RAW12;
    }
    if (usecase.BML_input == RGBLOAD)
    {
        usecase.BML_GrbFormat = GRBFMT_RGB30;
    }
}

/* ------------------------------------------------------------------------------------
   FUNCTION : ITE_dbg_getusecase
   PURPOSE  :
   ------------------------------------------------------------------------------------ */
void ITE_dbg_getusecase(void)
{ char *format;

LOS_Log("usecase LR size X: %d\n",usecase.LR_XSize,NULL,NULL,NULL,NULL,NULL,NULL);
LOS_Log("usecase LR size Y: %d\n",usecase.LR_YSize,NULL,NULL,NULL,NULL,NULL,NULL);

switch( usecase.LR_GrbFormat) {
    case (GRBFMT_YUV422_RASTER_INTERLEAVED): format = "GRBFMT_YUV422_RASTER_INTERLEAVED";break;
    case (GRBFMT_YUV420_RASTER_PLANAR_I420): format = "GRBFMT_YUV420_RASTER_PLANAR_I420";break;
    case (GRBFMT_YUV420_RASTER_PLANAR_YV12): format = "GRBFMT_YUV420_RASTER_PLANAR_YV12";break;
    case (GRBFMT_YUV422_MB_SEMIPLANAR):format = "GRBFMT_YUV422_MB_SEMIPLANAR";break;
    case (GRBFMT_YUV420_MB_SEMIPLANAR_FILTERED):format = "GRBFMT_YUV420_MB_SEMIPLANAR_FILTERED";break;
    case (GRBFMT_YUV420_MB_SEMIPLANAR_DECIMATED):format = "GRBFMT_YUV420_MB_SEMIPLANAR_DECIMATED";break;
    case (GRBFMT_YUV420_RASTER_SEMIPLANAR_NV21):format = "GRBFMT_YUV420_RASTER_SEMIPLANAR_NV21";break;
    case (GRBFMT_YUV420_RASTER_SEMIPLANAR_NV12):format = "GRBFMT_YUV420_RASTER_SEMIPLANAR_NV12";break;
    case (GRBFMT_RGB30):format = "GRBFMT_RGB30";break;
    case (GRBFMT_RAW8):format = "GRBFMT_RAW8";break;
    case (GRBFMT_RAW12):format = "GRBFMT_RAW12";break;
    case (GRBFMT_FAST_RAW8):format = "GRBFMT_FAST_RAW8";break;
    case (GRBFMT_FAST_RAW12):format = "GRBFMT_FAST_RAW12";break;
    case (GRBFMT_BML2_RAW8):format = "GRBFMT_BML2_RAW8";break;
    case (GRBFMT_BML2_RAW12):format = "GRBFMT_BML2_RAW12";break;
    case (GRBFMT_A4R4G4B4):format = "GRBFMT_A4R4G4B4";break;
    case (GRBFMT_A1R5G5B5):format = "GRBFMT_A1R5G5B5";break;
    case (GRBFMT_R5G6B5):format = "GRBFMT_R5G6B5";break;
    case (GRBFMT_R8G8B8):format = "GRBFMT_R8G8B8";break;
    case (GRBFMT_A8R8G8B8):format = "GRBFMT_A8R8G8B8";break;
        default: format = "GRBFMT_R5G6B5";break;
}
LOS_Log("usecase LR format: %s\n",format,NULL,NULL,NULL,NULL,NULL,NULL);
LOS_Log("usecase HR size X: %d\n",usecase.HR_XSize,NULL,NULL,NULL,NULL,NULL,NULL);
LOS_Log("usecase HR size Y: %d\n",usecase.HR_YSize,NULL,NULL,NULL,NULL,NULL,NULL);

switch( usecase.HR_GrbFormat) {
    case (GRBFMT_YUV422_RASTER_INTERLEAVED):format = "GRBFMT_YUV422_RASTER_INTERLEAVED";break;
    case (GRBFMT_YUV420_RASTER_PLANAR_I420):format = "GRBFMT_YUV420_RASTER_PLANAR_I420";break;
    case (GRBFMT_YUV420_RASTER_PLANAR_YV12):format = "GRBFMT_YUV420_RASTER_PLANAR_YV12";break;
    case (GRBFMT_YUV422_MB_SEMIPLANAR):format = "GRBFMT_YUV422_MB_SEMIPLANAR";break;
    case (GRBFMT_YUV420_MB_SEMIPLANAR_FILTERED):format = "GRBFMT_YUV420_MB_SEMIPLANAR_FILTERED";break;
    case (GRBFMT_YUV420_MB_SEMIPLANAR_DECIMATED):format = "GRBFMT_YUV420_MB_SEMIPLANAR_DECIMATED";break;
    case (GRBFMT_YUV420_MB_SWAP_SEMIPLANAR_DECIMATED):format = "GRBFMT_YUV420_MB_SWAP_SEMIPLANAR_DECIMATED";break;
    case (GRBFMT_YUV420_RASTER_SEMIPLANAR_NV21):format = "GRBFMT_YUV420_RASTER_SEMIPLANAR_NV21";break;
    case (GRBFMT_YUV420_RASTER_SEMIPLANAR_NV12):format = "GRBFMT_YUV420_RASTER_SEMIPLANAR_NV12";break;
    case (GRBFMT_RGB30):format = "GRBFMT_RGB30";break;
    case (GRBFMT_RAW8):format = "GRBFMT_RAW8";break;
    case (GRBFMT_RAW12):format = "GRBFMT_RAW12";break;
    case (GRBFMT_FAST_RAW8):format = "GRBFMT_FAST_RAW8";break;
    case (GRBFMT_FAST_RAW12):format = "GRBFMT_FAST_RAW12";break;
    case (GRBFMT_BML2_RAW8):format = "GRBFMT_BML2_RAW8";break;
    case (GRBFMT_BML2_RAW12):format = "GRBFMT_BML2_RAW12";break;
    case (GRBFMT_A4R4G4B4):format = "GRBFMT_A4R4G4B4";break;
    case (GRBFMT_A1R5G5B5):format = "GRBFMT_A1R5G5B5";break;
    case (GRBFMT_R5G6B5):format = "GRBFMT_R5G6B5";break;
    case (GRBFMT_R8G8B8):format = "GRBFMT_R8G8B8";break;
    case (GRBFMT_A8R8G8B8):format = "GRBFMT_A8R8G8B8";break;
}
LOS_Log("usecase HR format: %s\n",format,NULL,NULL,NULL,NULL,NULL,NULL);
LOS_Log("usecase BMS size X: %d\n",usecase.BMS_XSize,NULL,NULL,NULL,NULL,NULL,NULL);
LOS_Log("usecase BMS size Y: %d\n",usecase.BMS_YSize,NULL,NULL,NULL,NULL,NULL,NULL);

switch( usecase.BMS_GrbFormat) {
    case (GRBFMT_RGB30):format = "GRBFMT_RGB30";break;
    case (GRBFMT_RAW8):format = "GRBFMT_RAW8";break;
    case (GRBFMT_RAW12):format = "GRBFMT_RAW12";break;
    case (GRBFMT_FAST_RAW8):format = "GRBFMT_FAST_RAW8";break;
    case (GRBFMT_FAST_RAW12):format = "GRBFMT_FAST_RAW12";break;
    case (GRBFMT_BML2_RAW8):format = "GRBFMT_BML2_RAW8";break;
    case (GRBFMT_BML2_RAW12):format = "GRBFMT_BML2_RAW12";break;
    case (GRBFMT_YUV422_RASTER_INTERLEAVED):
    case (GRBFMT_YUV420_RASTER_PLANAR_I420):
    case (GRBFMT_YUV420_RASTER_PLANAR_YV12):
    case (GRBFMT_YUV422_MB_SEMIPLANAR):
    case (GRBFMT_YUV420_MB_SEMIPLANAR_FILTERED):
    case (GRBFMT_YUV420_MB_SEMIPLANAR_DECIMATED):
    case (GRBFMT_YUV420_MB_SWAP_SEMIPLANAR_DECIMATED):
    case (GRBFMT_YUV420_RASTER_SEMIPLANAR_NV21):
    case (GRBFMT_YUV420_RASTER_SEMIPLANAR_NV12):
    case (GRBFMT_A4R4G4B4):
    case (GRBFMT_A1R5G5B5):
    case (GRBFMT_R5G6B5):
    case (GRBFMT_R8G8B8):
    case (GRBFMT_A8R8G8B8):LOS_Log("Wrong format or not yet set\n");break;
}
LOS_Log("usecase BMS format: %s\n",format,NULL,NULL,NULL,NULL,NULL,NULL);
switch( usecase.BMS_Mode) {
    case (BMSSTILL):format = "BMSSTILL";break;
    case (BMSVIDEO):format = "BMSVIDEO";break;
    }
LOS_Log("usecase BMS Mode: %s\n",format,NULL,NULL,NULL,NULL,NULL,NULL);
switch( usecase.BML_GrbFormat) {
    case (GRBFMT_RGB30):format = "GRBFMT_RGB30";break;
    case (GRBFMT_RAW8):format = "GRBFMT_RAW8";break;
    case (GRBFMT_RAW12):format = "GRBFMT_RAW12";break;
    case (GRBFMT_FAST_RAW8):format = "GRBFMT_FAST_RAW8";break;
    case (GRBFMT_FAST_RAW12):format = "GRBFMT_FAST_RAW12";break;
    case (GRBFMT_BML2_RAW8):format = "GRBFMT_BML2_RAW8";break;
    case (GRBFMT_BML2_RAW12):format = "GRBFMT_BML2_RAW12";break;
    case (GRBFMT_YUV422_RASTER_INTERLEAVED):
    case (GRBFMT_YUV420_RASTER_PLANAR_I420):
    case (GRBFMT_YUV420_RASTER_PLANAR_YV12):
    case (GRBFMT_YUV422_MB_SEMIPLANAR):
    case (GRBFMT_YUV420_MB_SEMIPLANAR_FILTERED):
    case (GRBFMT_YUV420_MB_SEMIPLANAR_DECIMATED):
    case (GRBFMT_YUV420_MB_SWAP_SEMIPLANAR_DECIMATED):
    case (GRBFMT_YUV420_RASTER_SEMIPLANAR_NV21):
    case (GRBFMT_YUV420_RASTER_SEMIPLANAR_NV12):
    case (GRBFMT_A4R4G4B4):
    case (GRBFMT_A1R5G5B5):
    case (GRBFMT_R5G6B5):
    case (GRBFMT_R8G8B8):
    case (GRBFMT_A8R8G8B8):LOS_Log("Wrong format or not yet set\n");break;
}
LOS_Log("usecase BML format: %s\n",format,NULL,NULL,NULL,NULL,NULL,NULL);
}

/********************************************************/
/*    SEQUENCE MODE COMMAND             */
/********************************************************/

/* ---------------------------------
   FUNCTION : ITE_dbg_preview
   PURPOSE  :
   --------------------------------- */
void ITE_dbg_preview(char * ap_type, char * ap_mode, char * ap_streamlength)
{
if (0== strcmp(ap_type,"still")) {
  if (0== strcmp(ap_mode,"prepare")) {
    // WARNING Need to rebuild LCDBuffer because it is the same as GrabLR[0]
    /* ITE_StopMcde();
    if (LCDBuffer.address != NULL) ITE_FreeSiaPictureBuffer(&LCDBuffer);
    ITE_createSiaPictureBuffer(&LCDBuffer,usecase.LR_XSize,usecase.LR_YSize,GRBFMT_R5G6B5,BUF);
    ITE_configureMcde(&LCDBuffer);
    */
    ITE_StillPreview_Prepare(&usecase);}
  if (0== strcmp(ap_mode,"start")) {ITE_StillPreview_Start( ITE_ConvToInt16(ap_streamlength));}
  if (0== strcmp(ap_mode,"stop")) {ITE_StillPreview_Stop();}
  if (0== strcmp(ap_mode,"free")) {ITE_StillPreview_Free();}
  }
if (0== strcmp(ap_type,"video")) {
  if (0== strcmp(ap_mode,"prepare")) {ITE_VideoPreview_Prepare(&usecase);}
  if (0== strcmp(ap_mode,"start")) {ITE_VideoPreview_Start( ITE_ConvToInt16(ap_streamlength));}
  if (0== strcmp(ap_mode,"stop")) {ITE_VideoPreview_Stop();}
  if (0== strcmp(ap_mode,"free")) {ITE_VideoPreview_Free();}
  }
}

/* ---------------------------------
   FUNCTION : ITE_dbg_still
   PURPOSE  :
   --------------------------------- */
void ITE_dbg_still(char * ap_mode)
{ int nbofframe=1;

  if (0== strcmp(ap_mode,"prepare")) {ITE_Still_Prepare(&usecase,nbofframe);}
  if (0== strcmp(ap_mode,"start")) {ITE_Still_Start(nbofframe);}
  if (0== strcmp(ap_mode,"stop")) {ITE_Still_Stop();}
  if (0== strcmp(ap_mode,"free")) {ITE_Still_Free();}
}


/* ---------------------------------
   FUNCTION : ITE_dbg_video
   PURPOSE  :
   --------------------------------- */
void ITE_dbg_video(char * ap_mode)
{
  if (0== strcmp(ap_mode,"prepare")) {ITE_Video_Prepare(&usecase,INFINITY);}
  if (0== strcmp(ap_mode,"start")) {ITE_Video_Start(INFINITY);}
  if (0== strcmp(ap_mode,"stop")) {ITE_Video_Stop();}
  if (0== strcmp(ap_mode,"free")) {ITE_Video_Free();}
}


/* ---------------------------------
   FUNCTION : ITE_dbg_LR
   PURPOSE  :
   --------------------------------- */
void ITE_dbg_LR(char * ap_mode)
{
  if (0== strcmp(ap_mode,"prepare")) {ITE_LR_Prepare(&usecase,INFINITY);}
  if (0== strcmp(ap_mode,"start")) {ITE_LR_Start(INFINITY);}
  if (0== strcmp(ap_mode,"stop")) {ITE_LR_Stop();}
  if (0== strcmp(ap_mode,"abort")) {ITE_LR_Abort();}
  if (0== strcmp(ap_mode,"free")) {ITE_LR_Free();}
}

/* ---------------------------------
   FUNCTION : ITE_dbg_HR
   PURPOSE  :
   --------------------------------- */
void ITE_dbg_HR(char * ap_mode)
{
  if (0== strcmp(ap_mode,"prepare")) {ITE_HR_Prepare(&usecase,INFINITY);}
  if (0== strcmp(ap_mode,"start")) {ITE_HR_Start(INFINITY);}
  if (0== strcmp(ap_mode,"stop")) {ITE_HR_Stop();}
  if (0== strcmp(ap_mode,"abort")) {ITE_HR_Abort();}
  if (0== strcmp(ap_mode,"free")) {ITE_HR_Free();}
}

/* ---------------------------------
   FUNCTION : ITE_dbg_LRHR
   PURPOSE  :
   --------------------------------- */
void ITE_dbg_LRHR(char * ap_mode)
{
  if (0== strcmp(ap_mode,"prepare")) {ITE_LRHR_Prepare(&usecase,INFINITY);}
  if (0== strcmp(ap_mode,"start")) {ITE_LRHR_Start(INFINITY);}
  if (0== strcmp(ap_mode,"stop")) {ITE_LRHR_Stop();}
  if (0== strcmp(ap_mode,"abort")) {ITE_LRHR_Abort();}
  if (0== strcmp(ap_mode,"free")) {ITE_LRHR_Free();}
}

/*Here we first  write the  PERIPH_CTRL_CTRL_OFFSET to 0x1 (for sleeping fetching is still enabled). For each mode we see the
2 LSBs of PERIPH_CTRL_STATUS_OFFSET register. So, 0th bit is set to 1 when it is in sleep mode and 1st bit tells the mode it enters
i.e. we get modebit 1  when in mode 0, modebit=3 in mode 1,modebit=5 in mode 2 and modebit= 7 in mode 3
We also make use of two new wrapper functions that is used to read and write from/to isp registers [SK]*/

void ITE_dbg_sleepxp70(char *modeno)
{
   t_bool setwakeupbit = FALSE;
   t_uint16 modes;
   t_uint8 sleepbit,wakeupbit;
   volatile t_uint8 modebit=0;
   char mess[256],pathname[256],filename[256];
   modes=ITE_ConvToInt16(modeno);


 sprintf(filename, "sleepmodetest");
 sprintf(mess,"_%s",modeno);
 strcat(filename,mess);
 sprintf(pathname,IMAGING_PATH"/ite_nmf/test_results_nreg/sleepmodetest");
 strcat(g_out_path,pathname);
 init_test_results_nreg(g_out_path);

 if ((modes ==0)||(modes ==1)||(modes ==2)||(modes ==3))
    {
         sleepbit = 0x01;
         IspRegWrite_Wrapper(PERIPH_CTRL_CTRL_OFFSET,(long*)&sleepbit,1);
    }


   switch (modes) {

    case 0:

              mmte_testStart(filename,"sleepmodetest",g_out_path);
              sprintf(mess,"Testing for Sleep mode %s",modeno);
              mmte_testNext(mess);
              ITE_writePE(SystemSetup_e_IdleMode_Ctrl_Byte0,IdleMode_e_IDLE_MODE_0);
              LOS_Log("\nEntering Idle mode 0.\n");
              LOS_Log("\nGoing to Sleep..\n");
              ITE_writePE(HostInterface_Control_e_HostInterfaceCommand_User_Byte0,HostInterfaceCommand_e_SLEEP);
              if (0 != usecase.smiapp_powerUp_sequence)
              {
                   ITE_Do_SmiaPP_PowerDown_Sequence();
              }
              LOS_Log("\nWaiting for 2 secs before wakeup..\n");
             while (modebit == 0)
               {
                 IspRegRead_Wrapper (PERIPH_CTRL_STATUS_OFFSET,(long*)&modebit,1);
                 LOS_Sleep(20);
               }

              LOS_Log("\nmodebit is set : %lu\n",modebit);
              LOS_Sleep(2000);
              sprintf(mess,"mode 0\n");
              mmte_testComment(mess);
              if(modebit == 1)
                 {

                    LOS_Log("\nTEST PASSED\n");
                    mmte_testResult(TEST_PASSED);
                    mmte_testEnd();
                    memset(g_out_path,0,KlogDirectoryLentgh*sizeof(char));
                 }
              else
               {
                      LOS_Log("\nTEST FAILED\n");
                      mmte_testResult(TEST_FAILED);
                      mmte_testEnd();
                      memset(g_out_path,0,KlogDirectoryLentgh*sizeof(char));
               }
             break ;

    case 1:
             mmte_testStart(filename,"sleepmodetest",g_out_path);
             sprintf(mess,"Testing for Sleep mode %s",modeno);
             mmte_testNext(mess);
             ITE_writePE(SystemSetup_e_IdleMode_Ctrl_Byte0,IdleMode_e_IDLE_MODE_1);
             LOS_Log("\nEntering Idle mode 1.\n");
             LOS_Log("\nGoing to Sleep..\n");
             ITE_writePE(HostInterface_Control_e_HostInterfaceCommand_User_Byte0,HostInterfaceCommand_e_SLEEP);
             if (0 != usecase.smiapp_powerUp_sequence)
             {
                  ITE_Do_SmiaPP_PowerDown_Sequence();
             }
             while (modebit == 0)
              {
                IspRegRead_Wrapper (PERIPH_CTRL_STATUS_OFFSET,(long*)&modebit,1);
                LOS_Sleep(20);
              }

             LOS_Log("\nmodebit is set : %lu\n",modebit);
             LOS_Log("\nWaiting for 2 secs before wakeup..\n");
             LOS_Sleep(2000);
             sprintf(mess,"mode 1\n");
             mmte_testComment(mess);

             if(modebit == 3)
                 {

                    LOS_Log("\nTEST PASSED\n");
                    mmte_testResult(TEST_PASSED);
                    mmte_testEnd();
                    memset(g_out_path,0,KlogDirectoryLentgh*sizeof(char));
                 }
              else
               {
                      LOS_Log("\nTEST FAILED\n");
                      mmte_testResult(TEST_FAILED);
                      mmte_testEnd();
                      memset(g_out_path,0,KlogDirectoryLentgh*sizeof(char));
               }
             break ;

    case 2:
             mmte_testStart(filename,"sleepmodetest",g_out_path);
             sprintf(mess,"Testing for Sleep mode %s",modeno);
             mmte_testNext(mess);
             ITE_writePE(SystemSetup_e_IdleMode_Ctrl_Byte0,IdleMode_e_IDLE_MODE_2);
             LOS_Log("\nEntering Idle mode 2\n");
             LOS_Log("\nGoing to Sleep..\n");
             ITE_writePE(HostInterface_Control_e_HostInterfaceCommand_User_Byte0,HostInterfaceCommand_e_SLEEP);
             if (0 != usecase.smiapp_powerUp_sequence)
             {
                  ITE_Do_SmiaPP_PowerDown_Sequence();
             }
             while (modebit == 0)
              {
                IspRegRead_Wrapper (PERIPH_CTRL_STATUS_OFFSET,(long*)&modebit,1);
                LOS_Sleep(20);
              }
             LOS_Log("\nmodebit is set : %lu\n",modebit);
             LOS_Log("\nWaiting for 2 secs before wakeup..\n");
             LOS_Sleep(2000);
             sprintf(mess,"mode 2\n");
             mmte_testComment(mess);
             if(modebit == 5)
                 {

                    LOS_Log("\nTEST PASSED\n");
                    mmte_testResult(TEST_PASSED);
                    mmte_testEnd();
                    memset(g_out_path,0,KlogDirectoryLentgh*sizeof(char));
                 }
              else
               {
                      LOS_Log("\nTEST FAILED\n");
                      mmte_testResult(TEST_FAILED);
                      mmte_testEnd();
                      memset(g_out_path,0,KlogDirectoryLentgh*sizeof(char));
               }
             break ;

    case 3:
             mmte_testStart(filename,"sleepmodetest",g_out_path);
             sprintf(mess,"Testing for Sleep mode %s",modeno);
             mmte_testNext(mess);
             ITE_writePE(SystemSetup_e_IdleMode_Ctrl_Byte0,IdleMode_e_IDLE_MODE_3);
             LOS_Log("\nEntering Idle mode 3\n");
             LOS_Log("\nGoing to Sleep..\n");
             ITE_writePE(HostInterface_Control_e_HostInterfaceCommand_User_Byte0,HostInterfaceCommand_e_SLEEP);
             if (0 != usecase.smiapp_powerUp_sequence)
             {
                  ITE_Do_SmiaPP_PowerDown_Sequence();
             }
             while (modebit == 0)
              {
                IspRegRead_Wrapper (PERIPH_CTRL_STATUS_OFFSET,(long*)&modebit,1);
                LOS_Sleep(20);
              }
             LOS_Log("\nmodebit is set : %lu\n",modebit);
             LOS_Log("\nWaiting for 2 secs before wakeup..\n");
             LOS_Sleep(2000);
             setwakeupbit = TRUE;
             sprintf(mess,"mode 3\n");
             mmte_testComment(mess);
             if(modebit == 7)
                 {

                    LOS_Log("\nTEST PASSED\n");
                    mmte_testResult(TEST_PASSED);
                    mmte_testEnd();
                    memset(g_out_path,0,KlogDirectoryLentgh*sizeof(char));
                 }
              else
               {
                      LOS_Log("\nTEST FAILED\n");
                      mmte_testResult(TEST_FAILED);
                      mmte_testEnd();
                      memset(g_out_path,0,KlogDirectoryLentgh*sizeof(char));
               }
             break ;

   default:
             ITE_writePE(SystemSetup_e_IdleMode_Ctrl_Byte0,IdleMode_e_IDLE_MODE_NONE);
             LOS_Log("\nNo Idle mode entered.\n");
             LOS_Log("\nGoing to Sleep..\n");
             ITE_writePE(HostInterface_Control_e_HostInterfaceCommand_User_Byte0,HostInterfaceCommand_e_SLEEP);
             LOS_Log("\nWaiting for 2 secs before wakeup..\n");
             LOS_Sleep(2000);
             break ;
    }

   if (setwakeupbit == TRUE)
  {
   wakeupbit = 0x03 ;
   IspRegWrite_Wrapper (PERIPH_CTRL_CTRL_OFFSET,(long*)&wakeupbit,1);
  }


  ITE_writePE(HostInterface_Control_e_HostInterfaceCommand_User_Byte0,HostInterfaceCommand_e_WAKEUP);
  if (0 != usecase.smiapp_powerUp_sequence)
  {
    ITE_Do_SmiaPP_PowerUp_Sequence();
  }
}

void ITE_dbg_usagemode(char *modeno)
{
    //    usagemode: usagemode <vf/af/still/stillnight/stillseq/video/videonight/videohq/videohs/none>\n"

    LLD_USAGE_MODE_te e_usage_mode = CAM_DRV_USAGE_MODE_UNDEFINED;

    if (0== strcmp(modeno,"vf"))
    {
        LOS_Log(" Usage Mode vf");
        e_usage_mode = LLD_USAGE_MODE_VF;
    }

    if (0== strcmp(modeno,"af"))
    {
        LOS_Log(" Usage Mode af");
        e_usage_mode = LLD_USAGE_MODE_AF;
    }

    if (0== strcmp(modeno,"still"))
    {
        LOS_Log(" Usage Mode still");
        e_usage_mode = LLD_USAGE_MODE_STILL_CAPTURE;
    }

    if (0== strcmp(modeno,"stillnight"))
    {
        LOS_Log(" Usage Mode stillnight");
        e_usage_mode = LLD_USAGE_MODE_NIGHT_STILL_CAPTURE;
    }

    if (0== strcmp(modeno,"stillseq"))
    {
        LOS_Log(" Usage Mode stillseq");
        e_usage_mode = LLD_USAGE_MODE_STILL_SEQ_CAPTURE;
    }

    if (0== strcmp(modeno,"video"))
    {
        LOS_Log(" Usage Mode video");
        e_usage_mode = LLD_USAGE_MODE_VIDEO_CAPTURE;
    }

    if (0== strcmp(modeno,"videonight"))
    {
        LOS_Log(" Usage Mode videonight");
        e_usage_mode = LLD_USAGE_MODE_NIGHT_VIDEO_CAPTURE;
    }

    if (0== strcmp(modeno,"videohq"))
    {
        LOS_Log(" Usage Mode: high quality video videohq");
        e_usage_mode = LLD_USAGE_MODE_HQ_VIDEO_CAPTURE;
    }

    if (0== strcmp(modeno,"videohs"))
    {
        LOS_Log(" Usage Mode: high speed video videohs");
        e_usage_mode = LLD_USAGE_MODE_HS_VIDEO_CAPTURE;
    }

    if (0== strcmp(modeno,"none"))
    {
        LOS_Log(" Usage Mode: undefined none");
        e_usage_mode = CAM_DRV_USAGE_MODE_UNDEFINED;
    }

    ITE_writePE(RunMode_Control_e_LLD_USAGE_MODE_usagemode_Byte0,e_usage_mode);
}

/* ---------------------------------
   FUNCTION : ITE_dbg_bms
   PURPOSE  :
   --------------------------------- */

void ITE_dbg_bms(char * ap_mode)
{ int nbofframe=1;

  if (0== strcmp(ap_mode,"prepare")) {ITE_BMS_Prepare(&usecase,nbofframe);}
  if (0== strcmp(ap_mode,"start")) {ITE_BMS_Start(nbofframe);}
  if (0== strcmp(ap_mode,"stop")) {ITE_BMS_Stop();}
  if (0== strcmp(ap_mode,"abort")) {ITE_BMS_Abort();}
  if (0== strcmp(ap_mode,"free")) {ITE_BMS_Free();}
}


/* ---------------------------------
   FUNCTION : ITE_dbg_bml
   PURPOSE  :
   --------------------------------- */

void ITE_dbg_bml(char * ap_mode)
{ int nbofframe=1;

  if (0== strcmp(ap_mode,"prepare")) {ITE_BML_Prepare(&usecase,nbofframe);}
  if (0== strcmp(ap_mode,"start")) {ITE_BML_Start(nbofframe);}
  if (0== strcmp(ap_mode,"stop")) {ITE_BML_Stop();}
  if (0== strcmp(ap_mode,"free")) {ITE_BML_Free();}
}
/* ---------------------------------
   FUNCTION : ITE_dbg_bml_stripe
   PURPOSE  : // STE_NOIDA BML_STRIPE_ADD
   --------------------------------- */
void ITE_dbg_bml_stripe(char * ap_mode, char * ap_mode1)
{

 int nbofframe=1;
  if (0!= strcmp(ap_mode,"stripe"))
  {
      CLI_disp_error("syntax: bml <prepare/start/stop/free>\n");
   }
  else
  {
      if (0== strcmp(ap_mode1,"prepare")) {ITE_BML_stripe_Prepare(&usecase,nbofframe);}
      if (0== strcmp(ap_mode1,"start")) {ITE_BML_stripe_Start(nbofframe,2);}
      if (0== strcmp(ap_mode1,"stop")) {ITE_BML_Stop();}
      if (0== strcmp(ap_mode1,"free")) {ITE_BML_Free();}
  }
}
/********************************************************/
/*    DISPLAY COMMAND                   */
/********************************************************/

/* ---------------------------------
   FUNCTION : ITE_dbg_dispbuf
   PURPOSE  :
   --------------------------------- */
void ITE_dbg_dispbuf(char * ap_pipe, int mode)
{ if (mode == 255) { // infinity mode display
           if (0== strcmp(ap_pipe,"LR")) {ITE_DisplayBuffer(&(GrabBufferLR[0]));}
           if (0== strcmp(ap_pipe,"HR")) {ITE_DisplayBuffer(&(GrabBufferHR[0]));}
           }
  else { // single mode display
           if (0== strcmp(ap_pipe,"LR")) {ITE_DisplaySingleBuffer(&(GrabBufferLR[0]));}
           if (0== strcmp(ap_pipe,"HR")) {ITE_DisplaySingleBuffer(&(GrabBufferHR[0]));}
       }
}

/* ---------------------------------
   FUNCTION : ITE_dbg_initbuf
   PURPOSE  :
   --------------------------------- */
void ITE_dbg_initbuf(char * ap_pipe)
{
  if (0== strcmp(ap_pipe,"LR")) {ITE_Initialize_SiaPictureBuffer(&(GrabBufferLR[0]));}
  if (0== strcmp(ap_pipe,"HR")) {ITE_Initialize_SiaPictureBuffer(&(GrabBufferHR[0]));}
}

/* ---------------------------------
   FUNCTION : ITE_dbg_cleanbuf
   PURPOSE  :
   --------------------------------- */
void ITE_dbg_cleanbuf(char * ap_pipe)
{
  if (0== strcmp(ap_pipe,"LR")) {ITE_CleanBuffer(&(GrabBufferLR[0]));}
  if (0== strcmp(ap_pipe,"HR")) {ITE_CleanBuffer(&(GrabBufferHR[0]));}
  if (0== strcmp(ap_pipe,"BMS")) {ITE_CleanBuffer(&(GrabBufferBMS[0]));}
}

#if !(defined(__PEPS8500_SIA) || defined(_SVP_))
void ITE_dbg_showgrabuf(char * ap_pipe)
{
  if (0== strcmp(ap_pipe,"LR")) {ITE_StoreinLCDBuffer(&(GrabBufferLR[0]));}
  if (0== strcmp(ap_pipe,"HR")) {ITE_StoreinLCDBuffer(&(GrabBufferHR[0]));}
}
#endif

int ITE_dbg_initlcdbuf(char* ap_size, char* ap_format)
{
    int ret = EOK;
    enum e_grabFormat afmt;

    ret = ITE_GiveFormat(PIPE_LR, ap_format, &afmt);
    if (ret != EFMT)
    {
        ITE_createLCDBuffer(ITE_GiveXSize(PIPE_LR, ap_size, ap_format),
                            ITE_GiveYSize(PIPE_LR, ap_size, ap_format),
                            afmt);
        ret = EOK;
        goto END;
    }
    else
    {
        ret = ITE_GiveFormat(PIPE_HR, ap_format, &afmt);
        if (ret != EFMT)
        {
            ITE_createLCDBuffer(ITE_GiveXSize(PIPE_HR, ap_size, ap_format),
                                ITE_GiveYSize(PIPE_HR, ap_size, ap_format),
                                afmt);
            ret = EOK;
            goto END;
        }
        else
        {
            LOS_Log("ITE_GiveFormat gave error in %s and line  %d\n",__FUNCTION__,__LINE__);
            goto END;
        }
    }

END:
    return ret;
}

/********************************************************/
/*    FRAMERATE COMMAND                 */
/********************************************************/

/* -----------------------------------------------------------
   FUNCTION : ITE_dbg_autoframerate (char * min, char * max)
   FUNCTION : ITE_dbg_manualframerate(char * fps)
   FUNCTION : ITE_dbg_getframerate(char * ap_pipe)
   ----------------------------------------------------------- */
void ITE_dbg_autoframerate(char * min, char * max)
{
   ITE_SetFrameRateControl(ITE_ConvToInt16(min),ITE_ConvToInt16(max));
}


void
ITE_dbg_manualframerate(
char    *curr_fps,
char    *max_fps)
{
    float      fps_set;
    t_uint8    u8_vfps_flag;

    u8_vfps_flag = ITE_readPE(VariableFrameRateControl_e_Flag_Byte0);

    // When only curr_fps needs to be set
    if (0 == ITE_stricmp(max_fps, "NIL"))
    {
        if (u8_vfps_flag == Flag_e_TRUE)
        {
            fps_set = ITE_float_readPE(VariableFrameRateControl_f_MaximumFrameRate_Hz_Byte0);
            ITE_SetFrameRateControl(ITE_ConvToFloat(curr_fps), fps_set);
        }
        else
        {
            // Only fixed fps being set other argument is dummy.
            ITE_SetFrameRateControl(ITE_ConvToFloat(curr_fps), NULL_FPS);
        }
    }


    // When only max_fps needs to be set
    else if (0 == ITE_stricmp(curr_fps, "NIL"))
    {
        // Check if vatriable frame rate mode is set
        if (u8_vfps_flag == Flag_e_TRUE)
        {
            fps_set = ITE_float_readPE(VariableFrameRateControl_f_CurrentFrameRate_Hz_Byte0);
            ITE_SetFrameRateControl(fps_set, ITE_ConvToFloat(max_fps));
        }
        else
        {
            // Only fixed fps
            CLI_disp_error("\nVARIABLE FPS DISABLED! MAX FPS CANNOT BE SET IN THIS MODE\n");
            ITE_NMF_ASSERT(u8_vfps_flag);
        }
    }
    else
    {
        if (u8_vfps_flag == Flag_e_TRUE)
        {
            // Both curr_fps and max_fps is being set
            ITE_SetFrameRateControl(ITE_ConvToFloat(curr_fps), ITE_ConvToFloat(max_fps));
        }
        else
        {
            CLI_disp_error("\nVARIABLE FPS DISABLED! MAX FPS CANNOT BE SET IN THIS MODE\n");
            ITE_NMF_ASSERT(u8_vfps_flag);
        }
    }
}
t_bool ITE_dbg_frameratedata(char * ap_pipe)
{
    float               f_page_elementMSB,
                        vt_pixel_clk;
    char                mess[256];
    t_bool              test;
    volatile t_uint16   count1 = 0,
                        count2 = 0;
    float               flout;
    unsigned long long  timestart = 0,
                        timestop = 0;
    volatile t_uint32   bufferCount2,
                        bufferCount1;
    volatile t_uint16   active_x,
                        active_y,
                        line_length,
                        frame_length,
                        woi_sizeX,
                        woi_sizeY,
                        u16_MaxFOV_X,
                        u16_MaxFOV_Y;

    float               expected_fps_float, f_vt_pixel_clk;
    float *             ptr_float_vt_pixel_clk;

    //unsigned long timestart,timestop;
    f_page_elementMSB = ITE_FrameRateProgrammed();

    if (0 == strcmp(ap_pipe, "LR"))
    {
        timestart = LOS_getSystemTime();
        count1 = ITE_readPE(PipeStatus_1_u8_FramesStreamedOutOfPipeLastRun_Byte0);
        bufferCount1 = g_grabLR_countglobal;
        LOS_Sleep(1000);
        count2 = ITE_readPE(PipeStatus_1_u8_FramesStreamedOutOfPipeLastRun_Byte0);
        bufferCount2 = g_grabLR_countglobal;
        timestop = LOS_getSystemTime();
    }


    if (0 == strcmp(ap_pipe, "HR"))
    {
        timestart = LOS_getSystemTime();
        count1 = ITE_readPE(PipeStatus_0_u8_FramesStreamedOutOfPipeLastRun_Byte0);
        bufferCount1 = g_grabHR_countglobal;
        LOS_Sleep(1000);
        count2 = ITE_readPE(PipeStatus_0_u8_FramesStreamedOutOfPipeLastRun_Byte0);
        bufferCount2 = g_grabHR_countglobal;
        timestop = LOS_getSystemTime();
    }

    expected_fps_float = ITE_FrameRateProgrammed();

    if (count2 < count1)
    {
        count2 = count2 + 255;
    }


    flout = ( float ) (count2 - count1);

    if
    (
        (f_page_elementMSB <= 1.0)
    &&  (( float ) (bufferCount2 - bufferCount1) - (f_page_elementMSB) <= 1.0)
    )
    {
        test = TRUE;
        sprintf(mess, "TEST PASSED Frames Streamed :%f Frame Rate Expected:%f\n", flout, expected_fps_float);
        mmte_testComment(mess);
        LOS_Log("TEST PASSED Frames Streamed :%f Frame Rate Expected:%f\n", flout, expected_fps_float);
    }
    else
    {
        test = FALSE;
        sprintf(mess, "TEST FAILED Frames Streamed :%f Frame Rate Expected:%f\n", flout, expected_fps_float);
        mmte_testComment(mess);
        LOS_Log("TEST FAILED Frames Streamed :%f Frame Rate Expected:%f\n", flout, expected_fps_float);
    }


    sprintf(mess, "FRAME AFTER CHECK BUFFER COUNT IS :%d\n", ( int ) (bufferCount2 - bufferCount1));
    mmte_testComment(mess);
    LOS_Log("FRAME AFTER CHECK BUFFER COUNT IS :%d\n", ( int ) (bufferCount2 - bufferCount1));

    active_x = ITE_readPE(Zoom_Status_LLA_u16_Ex_output_resX_Byte0);
    active_y = ITE_readPE(Zoom_Status_LLA_u16_Ex_output_resY_Byte0);
    woi_sizeX = ITE_readPE(Zoom_Status_LLA_u16_Ex_woi_resX_Byte0);
    woi_sizeY = ITE_readPE(Zoom_Status_LLA_u16_Ex_woi_resY_Byte0);

    LOS_Log("\nactive_x is: %i\nactive_y is :%i", active_x, active_y);
    LOS_Log("\nwoi_sizeX is: %i\nwoi_sizeY is :%i", woi_sizeX, woi_sizeY);
    sprintf(mess, "WOI: %d X %d Output: %d X %d \n",woi_sizeX, woi_sizeY, active_x, active_y);
    mmte_testComment(mess);

    line_length = ITE_readPE(CurrentFrameDimension_u16_VTLineLengthPck_Byte0);
    frame_length = ITE_readPE(CurrentFrameDimension_u16_VTFrameLengthLines_Byte0);

    LOS_Log("\nLine Length is : %i\nFrame Length is is :%i\n", line_length, frame_length);
    sprintf(mess, "\nLine Length is : %i\nFrame Length is is :%i\n",line_length, frame_length);

    vt_pixel_clk = ITE_readPE(VideoTimingOutput_f_VTPixelClockFrequency_Mhz_Byte0);
    ptr_float_vt_pixel_clk = (float *)&vt_pixel_clk;
    f_vt_pixel_clk = *ptr_float_vt_pixel_clk;

    LOS_Log("\nvt_pixel_clk is : %f\n",f_vt_pixel_clk);
    sprintf(mess,"\nvt_pixel_clk: %f\n", f_vt_pixel_clk);


    u16_MaxFOV_X = ITE_readPE(FrameDimensionStatus_u16_MaximumUsableSensorFOVX_Byte0);
    u16_MaxFOV_Y = ITE_readPE(FrameDimensionStatus_u16_MaximumUsableSensorFOVY_Byte0);

    LOS_Log("\nMaxUsable FOV X: %d, Y: %d \n", u16_MaxFOV_X, u16_MaxFOV_Y);
    sprintf(mess, "\nMaxUsable FOV X: %d, Y: %d \n",u16_MaxFOV_X, u16_MaxFOV_Y);
    mmte_testComment(mess);


    LOS_Log("Framerate Status = %f fr/sec \n", expected_fps_float);

    sprintf(
    mess,
    "Expected FPS= %f,Applied FPS= %f,Calculated FPS = %f\n",
    expected_fps_float,
    f_page_elementMSB,
    flout);

    mmte_testComment(mess);

    sprintf(mess,"ACTIVE_X = %i, ACTIVE_Y = %i, LINE_LENGTH = %i, FRAME_LENGTH = %i",active_x,active_y,line_length,frame_length);

    mmte_testComment(mess);

    LOS_Log("computed timestart %llu timestop %llu\n\n", timestart, timestop);

    return (test);
}

float ITE_dbg_getframerate(char * ap_pipe)
{
    volatile float      f_page_elementMSB;
    volatile t_uint16   count1 = 0,
                        count2 = 0;
    float               flout;
    unsigned long long  timestart = 0,
                        timestop = 0;
    t_uint32            bufferCount2,
                        bufferCount1;

    f_page_elementMSB = ITE_FrameRateProgrammed();
    LOS_Log(
    "Read VPIP PE Framerate Status = %f fr/sec \n",
    f_page_elementMSB);

    if (0 == strcmp(ap_pipe, "LR"))
    {
        timestart = LOS_getSystemTime();
        count1 = ITE_readPE(PipeStatus_1_u8_FramesStreamedOutOfPipeLastRun_Byte0);
        bufferCount1 = g_grabLR_countglobal;
        LOS_Sleep(1000);
        count2 = ITE_readPE(PipeStatus_1_u8_FramesStreamedOutOfPipeLastRun_Byte0);
        bufferCount2 = g_grabLR_countglobal;
        timestop = LOS_getSystemTime();
    }


    if (0 == strcmp(ap_pipe, "HR"))
    {
        timestart = LOS_getSystemTime();
        count1 = ITE_readPE(PipeStatus_0_u8_FramesStreamedOutOfPipeLastRun_Byte0);
        bufferCount1 = g_grabHR_countglobal;
        LOS_Sleep(1000);
        count2 = ITE_readPE(PipeStatus_0_u8_FramesStreamedOutOfPipeLastRun_Byte0);
        bufferCount2 = g_grabHR_countglobal;
        timestop = LOS_getSystemTime();
    }


    if (count2 < count1)
    {
        count2 = count2 + 255;
    }


    flout = ( float ) (count2 - count1);

    LOS_Log("computed timestart %llu timestop %llu fps %f\n\n", timestart, timestop, flout, NULL, NULL, NULL);
    LOS_Log("framecount start %d framecount stop %d \n\n", count1, count2, NULL, NULL, NULL, NULL);
    LOS_Log("buffercount per second : %d \n\n", (bufferCount2 - bufferCount1), NULL, NULL, NULL, NULL);

    return (flout);
}

int ITE_dbg_testframerate(char* ap_pipe_test, char* ap_size, char* ap_format)
{
    char mess[256], sensorname[256], filename[256], pathname[256];
    t_bool check;
    t_uint32 error = 0;
    int ret = EOK;

    if (usecase.sensor == 0)
    {
        sprintf(sensorname, "Cam0_");
    }
    else
    {
        sprintf(sensorname, "Cam1_");
    }

    snprintf(filename, sizeof(filename), "%stestframerate_%s_%s_%s", sensorname,
             ap_pipe_test, ap_size, ap_format);

    snprintf(pathname, sizeof(pathname), IMAGING_PATH"/ite_nmf/test_results_nreg/testframerate");
    strcat(g_out_path, pathname);

    init_test_results_nreg(g_out_path);
    mmte_testStart(filename, "testframerate", g_out_path);

    snprintf(mess, sizeof(mess), "Testing for %s PIPE IN %s RESOLUTION AND FORMAT IS %s",
             ap_pipe_test, ap_size, ap_format);
    mmte_testNext(mess);

    ret = ITE_dbg_setusecase(ap_pipe_test, ap_size, ap_format);
    if (ret != EOK)
    {
        LOS_Log("ITE_dbg_setusecase gave error in %s and line  %d\n",
                __FUNCTION__, __LINE__);
        goto END;
    }

    if (0 == strcmp(ap_pipe_test, "HR"))
    {
        usecase.HR_XSize = ITE_GiveXSize(PIPE_HR, ap_size, ap_format);
        usecase.HR_YSize = ITE_GiveYSize(PIPE_HR, ap_size, ap_format);

        LOS_Log("Resolution : %d X %d\n", (int)usecase.HR_XSize, (int)usecase.HR_YSize);
        sprintf(mess, "Resolution : %d X %d\n", (int)usecase.HR_XSize, (int)usecase.HR_YSize);
        mmte_testComment(mess);
        LOS_Sleep(1000);

        /*if (g_InputImageSource == InputImageSource_e_Sensor0)
        {
            ITE_SensorMode(COLORBAR);
        }
        else
        {
            ITE_SetRxTestPattern(RxTestPattern_e_ColourBars);
        }*/

        ITE_HR_Prepare(&usecase, INFINITY);

#ifdef TEST_PERFORMANCE
        RECORD_SYSTEM_TIME("START", "HR_DATAPATH", "START_HR_PIPE", MAX_HR_START_TIME);
#endif
        error = ITE_HR_Start(INFINITY);

#ifdef TEST_PERFORMANCE
        RECORD_SYSTEM_TIME("STOP", "HR_DATAPATH", "START_HR_PIPE", MAX_HR_START_TIME);
#endif

        if (error != 0)
        {
            sprintf(mess, "Error returned by ISP\n");
            mmte_testComment(mess);
            LOS_Log("\nTEST FAILED\n");
            mmte_testResult(TEST_FAILED);
        }

        LOS_Sleep(500);
        usecase.sensormode = NORMAL_MODE;
        LOS_Log("\n1secs stream on HR pipe\n");
        LOS_Sleep(500);
        /* ITE_testcolorBarHR(); */
    }
    else if (0 == strcmp(ap_pipe_test, "LR"))
    {
        usecase.LR_XSize = ITE_GiveXSize(PIPE_LR, ap_size, ap_format);
        usecase.LR_YSize = ITE_GiveYSize(PIPE_LR, ap_size, ap_format);

        LOS_Log("Resolution : %d X %d\n", (int)usecase.LR_XSize, (int)usecase.LR_YSize);
        sprintf(mess, "Resolution : %d X %d\n", (int)usecase.LR_XSize, (int)usecase.LR_YSize);
        mmte_testComment(mess);
        LOS_Sleep(1000);

        /*if (g_InputImageSource == InputImageSource_e_Sensor0)
        {
            ITE_SensorMode(COLORBAR);
        }
        else
        {
            ITE_SetRxTestPattern(RxTestPattern_e_ColourBars);
        }*/

        ITE_LR_Prepare(&usecase, INFINITY);
        error = ITE_LR_Start(INFINITY);
        if (error != 0)
        {
            sprintf(mess, "Error returned by ISP\n");
            mmte_testComment(mess);
            LOS_Log("\nTEST FAILED\n");
            mmte_testResult(TEST_FAILED);
        }

        LOS_Sleep(500);
        usecase.sensormode = NORMAL_MODE;
        LOS_Log("\n1secs stream on LR pipe\n");
        LOS_Sleep(500);
        /* ITE_testcolorBarLR(); */
    }

    check = ITE_dbg_frameratedata(ap_pipe_test);

    if (0 == strcmp(ap_pipe_test, "HR"))
    {
        ITE_HR_Stop();
        ITE_HR_Free();
    }
    else if (0 == strcmp(ap_pipe_test, "LR"))
    {
        ITE_LR_Stop();
        ITE_LR_Free();
    }

#ifdef TEST_PERFORMANCE
    if (TRUE == g_test_perf_status)
    {
        LOS_Log("\nTest PASSED in Performance\n");
        mmte_testComment("\nTest PASSED in Performance\n");
    }
    else
    {
        LOS_Log("\nTest FAILED in Performance\n");
        mmte_testComment("\nTest FAILED in Performance\n");
    }
#endif

    if (check == TRUE)
    {
        mmte_testResult(TEST_PASSED);
    }
    else
    {
        mmte_testResult(TEST_FAILED);
    }

    mmte_testEnd();
    memset(g_out_path, 0, KlogDirectoryLentgh * sizeof(char));

    LOS_Sleep(1000);

END:
    return ret;
}

#ifdef TEST_PERFORMANCE
void ITE_dbg_recordtime(char * type, char * usecase,char * comment, char * tag)
{
 t_uint32 tag_val;

     tag_val = ITE_ConvToInt32(tag);

     RECORD_SYSTEM_TIME(type, usecase, comment, (int)tag_val);

}
#endif

/********************************************************/
/*    DIGIZOOM COMMAND                  */
/********************************************************/
/* mle
void ITE_dbg_dz_prepare(void)
{
   ITE_DZ_PREPARE();
}
void ITE_dbg_dz_step(char * step)
{
   ITE_DZ_STEP(ITE_ConvToInt16(step));
}
void ITE_dbg_dz_reset(void)
{
   ITE_DZ_RESET();
}
void ITE_dbg_dz_test(char * ap_pipe, char * step)
{
  if (0== strcmp(ap_pipe,"LR")) {
     ITE_PutTestCrossSensor(ITE_GiveDesiredFOVX(ITE_ConvToInt16(step)),GRBPID_PIPE_LR);
     }
  if (0== strcmp(ap_pipe,"HR")) {
     ITE_PutTestCrossSensor(ITE_GiveDesiredFOVX(ITE_ConvToInt16(step)),GRBPID_PIPE_HR);
     }
}
*/
void ITE_dbg_colorbar_test(char * ap_pipe)
{t_uint16 result =0;
  if (0== strcmp(ap_pipe,"LR")) {
     result = TestColorBarbuffer(&(GrabBufferLR[0]), COLOR_BAR__DEFAULT_TESTING_POSITION);
     }
  if (0== strcmp(ap_pipe,"HR")) {
     result = TestColorBarbuffer(&(GrabBufferHR[0]), COLOR_BAR__DEFAULT_TESTING_POSITION);
     }
  if (0== strcmp(ap_pipe,"BMS")) {
     result = TestColorBarbuffer(&(GrabBufferBMS[0]), COLOR_BAR__DEFAULT_TESTING_POSITION);
     }
  if (result == 0) LOS_Log("ColorBarTest Passed\n");
  if (result == 1) LOS_Log("ColorBarTest Failed\n");
}

void ITE_dbg_sensormode_test(char *ap_mode)
{

    char mess[256],pathname[256],filename[256];
    sprintf(filename, "SensorTestPattern");
    sprintf(mess,"_%s",ap_mode);
    strcat(filename,mess);
    sprintf(pathname,IMAGING_PATH"/ite_nmf/test_results_nreg/SensorTestPattern");
    strcat(g_out_path,pathname);
    init_test_results_nreg(g_out_path);
    mmte_testStart(filename,"SensorTestPattern",g_out_path);
    sprintf(mess,"Configuring the SensorTestPattern %s",ap_mode);
    mmte_testNext(mess);

  if (0== strcmp(ap_mode,"NORMAL_MODE")) {
     ITE_SensorMode(NORMAL_MODE);}
  if (0== strcmp(ap_mode,"COLORBAR")) {
     ITE_SensorMode(COLORBAR);}
  if (0== strcmp(ap_mode,"FADETOGREY_COLORBAR")) {
     ITE_SensorMode(FADETOGREY_COLORBAR);}
  if (0== strcmp(ap_mode,"PN28")) {
     ITE_SensorMode(PN28);}
  if (0== strcmp(ap_mode,"HORIZ_GREY_SCALE")) {
     ITE_SensorMode(HORIZ_GREY_SCALE);}
  if (0== strcmp(ap_mode,"VERT_GREY_SCALE")) {
     ITE_SensorMode(VERT_GREY_SCALE);}
  if (0== strcmp(ap_mode,"SOLIDBAR_YELLOW")) {
     ITE_SensorMode(SOLIDBAR_YELLOW);}
  if (0== strcmp(ap_mode,"SOLIDBAR_CYAN")) {
     ITE_SensorMode(SOLIDBAR_CYAN);}
  if (0== strcmp(ap_mode,"SOLIDBAR_MAGENTA")) {
     ITE_SensorMode(SOLIDBAR_MAGENTA);}
  if (0== strcmp(ap_mode,"SOLIDBAR_BLACK")) {
     ITE_SensorMode(SOLIDBAR_BLACK);}
  if (0== strcmp(ap_mode,"SOLIDBAR_WHITE")) {
     ITE_SensorMode(SOLIDBAR_WHITE);}
  if (0== strcmp(ap_mode,"SOLIDBAR_RED")) {
     ITE_SensorMode(SOLIDBAR_RED);}
  if (0== strcmp(ap_mode,"SOLIDBAR_BLUE")) {
     ITE_SensorMode(SOLIDBAR_BLUE);}
  if (0== strcmp(ap_mode,"SOLIDBAR_GREEN")) {
     ITE_SensorMode(SOLIDBAR_GREEN);}

    mmte_testResult(TEST_PASSED);
    mmte_testEnd();
    memset(g_out_path,0,KlogDirectoryLentgh*sizeof(char));
}

void ITE_dbg_buffercheck_test(char * ap_pipe,char *ap_mode)
{e_basic_color color;
  if (0== strcmp(ap_mode,"YELLOW")) {
     color = YELLOW_COLOR_E;}
  else if (0== strcmp(ap_mode,"CYAN")) {
     color = CYAN_COLOR_E;}
  else if (0== strcmp(ap_mode,"MAGENTA")) {
     color = MAGENTA_COLOR_E;}
  else if (0== strcmp(ap_mode,"BLACK")) {
     color = BLACK_COLOR_E;}
  else if (0== strcmp(ap_mode,"WHITE")) {
     color = WHITE_COLOR_E;}
  else if (0== strcmp(ap_mode,"RED")) {
     color = RED_COLOR_E;}
  else if (0== strcmp(ap_mode,"BLUE")) {
     color = BLUE_COLOR_E;}
  else if (0== strcmp(ap_mode,"GREEN")) {
     color = GREEN_COLOR_E;}
  else {color = GREEN_COLOR_E;
    LOS_Log("Warning Color type error in Bufferceck_test function\n");
       }
  if (0== strcmp(ap_pipe,"LR")) {
     TestColoredbuffer(&(GrabBufferLR[0]), color);
     }
  if (0== strcmp(ap_pipe,"HR")) {
     TestColoredbuffer(&(GrabBufferHR[0]), color);
     }
  if (0== strcmp(ap_pipe,"BMS")) {
     TestColoredbuffer(&(GrabBufferBMS[0]), color);
     }
}

void ITE_dbg_sensoraccess(char * index, char *data)
{
      ITE_I2CWriteWord2Sensor(ITE_ConvToInt16(index),ITE_ConvToInt16(data));
}

void ITE_dbg_sensorread(char * index)
{
     LOS_Log("sensor data = %d \n",ITE_I2CReadWordFromSensor(ITE_ConvToInt16(index)));
}


void ITE_dbg_writebmp(char *ap_pipe,char *filename,char *pelorder)
{
     char pathname[256];
     snprintf(pathname,sizeof(pathname),IMAGING_PATH"/%s",filename);

     if (0== strcmp(ap_pipe,"LR")) {
     if (0== strcmp(pelorder,"0")) GrabBufferLR[0].pelorder=0;
     if (0== strcmp(pelorder,"1")) GrabBufferLR[0].pelorder=1;
     if (0== strcmp(pelorder,"2")) GrabBufferLR[0].pelorder=2;
     if (0== strcmp(pelorder,"3")) GrabBufferLR[0].pelorder=3;
     ITE_StoreinBMPFile(pathname,&(GrabBufferLR[0]));
     }
  if (0== strcmp(ap_pipe,"HR")) {
     if (0== strcmp(pelorder,"0")) GrabBufferHR[0].pelorder=0;
     if (0== strcmp(pelorder,"1")) GrabBufferHR[0].pelorder=1;
     if (0== strcmp(pelorder,"2")) GrabBufferHR[0].pelorder=2;
     if (0== strcmp(pelorder,"3")) GrabBufferHR[0].pelorder=3;
     ITE_StoreinBMPFile(pathname,&(GrabBufferHR[0]));
     }
  if (0== strcmp(ap_pipe,"BMS")) {
     /*if (0== strcmp(pelorder,"0")) GrabBufferBMS[0].pelorder=0;
     if (0== strcmp(pelorder,"1")) GrabBufferBMS[0].pelorder=1;
     if (0== strcmp(pelorder,"2")) GrabBufferBMS[0].pelorder=2;
     if (0== strcmp(pelorder,"3")) GrabBufferBMS[0].pelorder=3;*/

     // Update Buffer Pixel Order with PixelOrder page element
     GrabBufferBMS[0].pelorder = ITE_readPE(SystemConfig_Status_e_PixelOrder_Byte0);
     ITE_StoreinBMPFile(pathname,&(GrabBufferBMS[0]));
     }
}

void ITE_dbg_previewvideo_startstop_test(void)
{int i;
  ITE_VideoPreview_Prepare(&usecase);
  ITE_DisplayBuffer(&(GrabBufferLR[0]));
  for (i=0;i<100;i++) {
    ITE_VideoPreview_Start(255);
    LOS_Sleep(1000);
    ITE_VideoPreview_Stop();
    }
  ITE_VideoPreview_Free();
}



void ITE_dbg_givergbvalue(char *ap_pipe,char *Xpos,char *Ypos)
{
ts_PelRGB888 color;
  if (0== strcmp(ap_pipe,"LR")) {
     ITE_GetRGBvalue(&(GrabBufferLR[0]),ITE_ConvToInt32(Xpos),
             ITE_ConvToInt32(Ypos),&color);
     LOS_Log("Pel: Red: %x Green: %x Blue: %x\n",color.RedValue,color.GreenValue,color.BlueValue);
     }
  if (0== strcmp(ap_pipe,"HR")) {
     ITE_GetRGBvalue(&(GrabBufferHR[0]),ITE_ConvToInt32(Xpos),
             ITE_ConvToInt32(Ypos),&color);
     LOS_Log("Pel: Red: %x Green: %x Blue: %x\n",color.RedValue,color.GreenValue,color.BlueValue);
     }
  if (0== strcmp(ap_pipe,"BMS")) {
     ITE_GetRGBvalue(&(GrabBufferBMS[0]),ITE_ConvToInt32(Xpos),
             ITE_ConvToInt32(Ypos),&color);
     LOS_Log("Pel: Red: %x Green: %x Blue: %x\n",color.RedValue,color.GreenValue,color.BlueValue);
     }
}

void ite_dbg_create_buffers(char *buffer_name)
{
    if (0 == strcmp(buffer_name,"OutpuModeBuffer"))
    ITE_NMF_create_Streaming_Config_Data_Buffer(pOutpuModeBuffer);

    else if (0 == strcmp(buffer_name,"FocusStatsBuffer"))
    ITE_NMF_createFocusStatsBuffer(pFocusStatsBuffer, sizeof(AFStats_Statistics_ts));

    else if (0 == strcmp(buffer_name,"AfFrameStatusBuffer"))
    ITE_NMF_createFocusStatsBuffer(pAfFrameStatusBuffer,sizeof(FrameParamStatus_Af_ts));

    else if(0 == strcmp(buffer_name,"StatsBuffer"))
    ITE_NMF_createStatsBuffer(pStatsBuffer);

    else if(0 == strcmp(buffer_name,"NVMStatsBuffer"))
    ITE_NMF_createNVMBuffer(pNVMStatsBuffer);

    else if(0 == strcmp(buffer_name,"TraceBuffer"))
    ITE_NMF_createTraceBuffer(pTraceBuffer);

    else if(0 == strcmp(buffer_name,"all"))
    {
      ITE_NMF_create_Streaming_Config_Data_Buffer(pOutpuModeBuffer);
      ITE_NMF_createFocusStatsBuffer(pFocusStatsBuffer,sizeof(AFStats_Statistics_ts));
      ITE_NMF_createFocusStatsBuffer(pAfFrameStatusBuffer,sizeof(FrameParamStatus_Af_ts));
      ITE_NMF_createStatsBuffer(pStatsBuffer);
      ITE_NMF_createNVMBuffer(pNVMStatsBuffer);
      ITE_NMF_createTraceBuffer(pTraceBuffer);
    }
    else
    LOS_Log("ENTER CORRECT PARAMETER NAME\n");

}

void ite_dbg_free_buffers(char *buffer_name)
{
    if (0 == strcmp(buffer_name,"OutpuModeBuffer"))
    ITE_NMF_freeSensorOutputModeBuffer(pOutpuModeBuffer);

    else if (0 == strcmp(buffer_name,"FocusStatsBuffer"))
    ITE_NMF_freeFocusStatsBuffer(pFocusStatsBuffer);

    else if (0 == strcmp(buffer_name,"AfFrameStatusBuffer"))
    ITE_NMF_freeFocusStatsBuffer(pAfFrameStatusBuffer);

    else if(0 == strcmp(buffer_name,"StatsBuffer"))
    ITE_NMF_freeStatsBuffer(pStatsBuffer);

    else if(0 == strcmp(buffer_name,"NVMStatsBuffer"))
    ITE_NMF_freeNVMBuffer(pNVMStatsBuffer);

    else if(0 == strcmp(buffer_name,"TraceBuffer"))
    ITE_NMF_freeTraceBuffer(pTraceBuffer);

    else if(0 == strcmp(buffer_name,"GammaBuffer"))
    ITE_NMF_freeGammaBuffer(pGammaLUTBuffer);

    else if(0 == strcmp(buffer_name,"all"))
     {
       ITE_NMF_freeSensorOutputModeBuffer(pOutpuModeBuffer);
       ITE_NMF_freeStatsBuffer(pStatsBuffer);
       ITE_NMF_freeFocusStatsBuffer(pFocusStatsBuffer);
       ITE_NMF_freeFocusStatsBuffer(pAfFrameStatusBuffer);
       ITE_NMF_freeNVMBuffer(pNVMStatsBuffer);
       ITE_NMF_freeTraceBuffer(pTraceBuffer);
       ITE_NMF_freeGammaBuffer(pGammaLUTBuffer);
     }
    else
    LOS_Log("ENTER CORRECT PARAMETER NAME\n");
}

void ite_nvm_raw_data(char* enable)
{
 if (0 == strcmp(enable,"enable"))
  {
    g_nvm_raw_data = 1;
  }
 else if(0 == strcmp(enable,"disable"))
  {
    g_nvm_raw_data = 0;
    LOS_Log("NVM RAW DATA disabled!!\n");
  }
 else
  {
   LOS_Log("To enable NVM RAW DATA type NVM_RAW enable\n");
  }
}

float ite_rand_FloatRange(float min_no, float max_no)
{
return ((max_no-min_no)*((float)rand()/RAND_MAX))+min_no;
}

void ITE_RxTestPattern(char * ap_mode, char *Red, char *GR, char *Blue, char *BG)
{

    char mess[256],pathname[256],filename[256];
    sprintf(filename, "RxTestPattern");
    sprintf(mess,"_%s",ap_mode);
    strcat(filename,mess);
    sprintf(pathname,IMAGING_PATH"/ite_nmf/test_results_nreg/RxTestPattern");
    strcat(g_out_path,pathname);
    init_test_results_nreg(g_out_path);
    mmte_testStart(filename,"RxTestPattern",g_out_path);
    sprintf(mess,"Configuring the RxTestPattern %s",ap_mode);
    mmte_testNext(mess);

    if (0== strcmp(ap_mode,"NORMAL_MODE")) {
     ITE_SetRxTestPattern(RxTestPattern_e_Normal);}
    if (0== strcmp(ap_mode,"COLORBAR")) {
     ITE_SetRxTestPattern(RxTestPattern_e_ColourBars);}
    if (0== strcmp(ap_mode,"FADETOGREY_COLORBAR")) {
     ITE_SetRxTestPattern(RxTestPattern_e_GraduatedColourBars);}
    if (0== strcmp(ap_mode,"DIAGONAL_GREY_SCALE")) {
     ITE_SetRxTestPattern(RxTestPattern_e_DiagonalGreyscale);}
    if (0== strcmp(ap_mode,"PN9")) {
     ITE_SetRxTestPattern(RxTestPattern_e_PsuedoRandom);}
    if (0== strcmp(ap_mode,"HORIZ_GREY_SCALE")) {
     ITE_SetRxTestPattern(RxTestPattern_e_HorizontalGreyscale);}
    if (0== strcmp(ap_mode,"VERT_GREY_SCALE")) {
     ITE_SetRxTestPattern(RxTestPattern_e_VerticalGreyscale);}
    if (0== strcmp(ap_mode,"SOLIDBAR")) {
     sprintf(mess, "Inside Solid Bar Rx Test Pattern Configuration \n");
     mmte_testComment(mess);
     sprintf(mess, "R,GR,B,BG values %d %d %d %d \n",ITE_ConvToInt16(Red), ITE_ConvToInt16(GR), ITE_ConvToInt16(Blue), ITE_ConvToInt16(BG));
     mmte_testComment(mess);
     LOS_Log("The Value of R,GR,B,BG  are %d, %d,%d,%d\n", ITE_ConvToInt16(Red), ITE_ConvToInt16(GR), ITE_ConvToInt16(Blue), ITE_ConvToInt16(BG));
     ITE_SetRxTestPattern(RxTestPattern_e_SolidColour);
     ITE_SetSolidColorData (ITE_ConvToInt16(Red), ITE_ConvToInt16(GR), ITE_ConvToInt16(Blue), ITE_ConvToInt16(BG));}

    mmte_testResult(TEST_PASSED);
    mmte_testEnd();
    memset(g_out_path,0,KlogDirectoryLentgh*sizeof(char));
}


void ITE_RxCursorPosition (char *x_pos, char *x_width, char *y_pos, char *y_width)
{
    ITE_SetRxCursorPosition (ITE_ConvToInt8(x_pos), ITE_ConvToInt8(x_width), ITE_ConvToInt8(y_pos), ITE_ConvToInt8(y_width));
}


void ITE_SetInputModeSelection(char *inp_sel)
{

    if (0== strcmp(inp_sel,"Sensor")) {
        LOS_Log(" Sensor Image Source Selected");
        g_InputImageSource = InputImageSource_e_Sensor0;}
    if (0== strcmp(inp_sel,"Rx")) {
        LOS_Log(" Rx Image Source Selected");
        g_InputImageSource = InputImageSource_e_Rx;}

}

int ITE_dbg_setutresolution(char* ap_pipe, char* ap_size, char* ap_format)
{
    int ret = EOK;
    enum e_grabFormat afmt;

    if (0 == strcmp(ap_pipe, "LR"))
    {
        ret = ITE_GiveFormat(PIPE_LR, ap_format, &afmt);
        if (ret != EFMT)
        {
            usecase.LR_GrbFormat = afmt;
            ret = EOK;
        }
        else
        {
            LOS_Log("ITE_GiveFormat gave error in %s and line  %d\n",
                    __FUNCTION__, __LINE__);
            goto END;
        }
        usecase.LR_XSize = ITE_GiveXSize(PIPE_LR, ap_size, ap_format);
        usecase.LR_YSize = ITE_GiveYSize(PIPE_LR, ap_size, ap_format);
    }
    else if (0 == strcmp(ap_pipe, "HR"))
    {
        ret = ITE_GiveFormat(PIPE_HR, ap_format, &afmt);
        if (ret != EFMT)
        {
            usecase.HR_GrbFormat = afmt;
            ret = EOK;
        }
        else
        {
            LOS_Log("ITE_GiveFormat gave error in %s and line  %d\n",
                    __FUNCTION__, __LINE__);
            goto END;
        }
        usecase.HR_XSize = ITE_GiveXSize(PIPE_HR, ap_size, ap_format);
        usecase.HR_YSize = ITE_GiveYSize(PIPE_HR, ap_size, ap_format);
    }

END:
    return ret;
}

//ER 427679,new command added for setting ISP FW trace parameters
/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_SetTraceProperty_cmd
   PURPOSE  : This command, set the ISP FW trace parameters,
   Arguments: Tace mechanism (none, XTI, Memort Dumps),
                     LogLevel
                     Enable/Disable
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_SetTraceProperty_cmd (int a_nb_args, char ** ap_args)
{
    CMD_COMPLETION cmdRetVal = CMD_COMPLETE;

    if (ap_args[1]==NULL) ap_args[1]=""; //For Linux, strcmp doesn't support NULL parameter

    //Help message for new command
    if (0== strcmp(ap_args[1],"help"))
    {
        LOS_Log("syntax:setTraceProp <Trace mechanism> <LogLevel> <Enable>\n<Trace mechanism>\n\t0 - None\n\t1 - XTI Trace\n\t2 - Memory Dumps\n");
        LOS_Log("<LogLevel>\n\t - Log level (in hex)\n");
        LOS_Log("<Enable>\n\t 0- Disable\n\t 1 - Enable\n");

        goto OUT;
    }

    //If memory dump is selected as the trace mechanismm, then its mandatory to provide the log level and enable input also
    if(0 == strcmp(ap_args[1], "2"))
    {
         if(a_nb_args != 4 )/*4 argument in case of memory dumps */
        {
            CLI_disp_error("Not correct command arguments\n");
            LOS_Log("syntax:setTraceProp <Trace mechanism> <LogLevel> <Enable>\n<Trace mechanism>\n\t0 - None\n\t1 - XTI Trace\n\t2 - Memory Dumps\n");
            LOS_Log("<LogLevel>\n\t - Log level (in hex)\n");
            LOS_Log("<Enable>\n\t 0- Disable\n\t 1 - Enable\n");

            cmdRetVal = CMD_ERR_ARGS;
            goto OUT;

        }
        else
        {
            //Correct argument has been passed ...
            g_TraceProp.traceMechanism = TraceMsgOutput_e_MEMORY_DUMP;
            g_TraceProp.traceLogLevel = ITE_ConvToInt16(ap_args[2]);
            g_TraceProp.traceEnable = ITE_ConvToInt8(ap_args[3]);
        }
    }
    else if(0 == strcmp(ap_args[1], "0"))
    {
        //Trace mechanism selected as None ...
        g_TraceProp.traceMechanism = TraceMsgOutput_e_NOMSGS;
    }
    else if(1 == strcmp(ap_args[1], "0"))
    {
        //XTI is selected as trace mechanism ...
        g_TraceProp.traceMechanism = TraceMsgOutput_e_XTI;
    }
    else
    {
        CLI_disp_error("Not correct command arguments\n");
        cmdRetVal = CMD_ERR_ARGS;
        goto OUT;
    }

OUT:
 return cmdRetVal;
}


//This function will be called when setGammaCurve command will be given in the ite_nmf.
//This command expect one argument that is the value of gamma curve which can take 3 values.
//0-> standard; 1-> custom; 2-> disable
CMD_COMPLETION C_ite_dbg_SetGammaCurve_cmd (int a_nb_args, char ** ap_args)
{
    CMD_COMPLETION cmdRetVal = CMD_COMPLETE;
    if (ap_args[1]==NULL)
        ap_args[1]=""; //For Linux, strcmp doesn't support NULL parameter

    if (a_nb_args > 2)
        CLI_disp_error("syntax: One one argument needed\n");

    if (0== strcmp(ap_args[1],"help"))
    {
        CLI_disp_error("syntax: This command expect one argument from following\n");
        CLI_disp_error("1.standard  2.custom 3.disable\n");
    }
    else if ((0 == strcmp(ap_args[1],"standard")) || (0 == strcmp(ap_args[1],"custom")) || (0 == strcmp(ap_args[1],"disable")))
    {
        LOS_Log("\n>>>The value of curve is %s",ap_args[1]);
        ITE_SetGammaCurve(ap_args[1]);
    }
    else
    {
        CLI_disp_error("Not a valid gamma curve\n");
    }



    return cmdRetVal;
}


// ER 447175 - To test the behaviour of SystemSetup_e_Flag_Request_ValidFrameNotification_Ctrl flag
/* ------------------------------------------------------------------------------------------------------
   FUNCTION : C_ite_dbg_TestValidFrameFlag_cmd

   PURPOSE  : The function will test the implementation of SystemSetup_e_Flag_Request_ValidFrameNotification_Ctrl flag.
                     Host set the value of e_Flag_Request_ValidFrameNotification_Ctrl to either true or flase
                     TRUE -> Firmware will send valid frame notification to host
                     FALSE -> Firmware won't send valid frame notification to host.

   Script  :  ./ite_nmf/script/common/testValidFrameFlag.ite

   Prerequisite  : The command should be run when firmware is in RUNNING state.

   Arguments: None.
   ----------------------------------------------------------------------------------------------------- */
CMD_COMPLETION C_ite_dbg_TestValidFrameFlag_cmd (int a_nb_args, char ** ap_args)
{
    CMD_COMPLETION cmdRetVal = CMD_COMPLETE;

    if (ap_args[1]==NULL)
        ap_args[1] = "";    // For Linux, strcmp doesn't support NULL parameter

    if (a_nb_args > 1)
    {
        CLI_disp_error("syntax: NO argument needed\n");
        cmdRetVal = CMD_ERR_ARGS;
    }
    else
    {
        ITE_TestValidFrameFlag();
    }

    return cmdRetVal;
}
