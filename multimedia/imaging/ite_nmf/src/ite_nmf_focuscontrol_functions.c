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

#include "ite_FocusControl_Module.h"
#include "ite_nmf_standard_functions.h"
#include "ite_nmf_statistic_functions.h"
#include "ite_nmf_focuscontrol_functions.h"

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
#include "ite_nmf_headers.h"
#include "ite_sia_buffer.h"
#include "ite_testenv_utils.h"

#include <cm/inc/cm_macros.h>
#include <test/api/test.h>

#include <los/api/los_api.h>

//#include <ilos/api/ilos_api.h>
#include "ite_main.h"
#include "sia.h"
#include "pictor_full.h"
#include "sia_register_fct.h"
#include "hi_register_acces.h"
#include "ite_sia_bootcmd.h"

#define ISP_REGION_IO   0xE0000000

volatile ts_siaFocusStatbuffer    FocusStatsBuffer;
volatile tps_siaFocusStatbuffer   pFocusStatsBuffer = (tps_siaFocusStatbuffer)&FocusStatsBuffer;
volatile ts_siaFocusStatbuffer    AfFrameStatusBuffer;
volatile tps_siaFocusStatbuffer   pAfFrameStatusBuffer = (tps_siaFocusStatbuffer)&AfFrameStatusBuffer;

//FocusStatsBuffersStorage_t  *pFocusStore;
//FocusStatsBuffersStorage_t* g_pFocusStatsBufferStorage;

/*StatsBuffersStorage_t* ITE_NMF_createFocusStatsBuffer(tps_siaMetaDatabuffer pBufferMetaData)
{
    StatsBuffersStorage_t* pStatsBufferStorage = 0;
    int i;
    int size = sizeof(AFStats_Statistics_ts) ;
    if (pBufferMetaData->handle == 0) {
        pBufferMetaData->handle = LOS_Alloc(size, 1024*1024*16,LOS_MAPPING_CREATE_UNCACHED_UNBUFFERED);
        //pBufferMetaData->phyaddress = ILOS_GetPhysicalAddress(pBufferMetaData->handle);
        pBufferMetaData->logAddress = LOS_GetLogicalAddress(pBufferMetaData->handle);
        }
    else LOS_Log("WARNING BufferMetaData already exist!!!!!!\n");

    if(pBufferMetaData->handle != 0)
	{	//init buffer: mandatory for buffer fill test
	  for (i=0; i<size; i++)
	      * ((t_uint8 *) pBufferMetaData->logAddress + i) = 0xff;
	}
    else LOS_Log("!!!!!!!!!!!! Allocate buffer error !!!!!!!!!!!!\n");

    pBufferMetaData->ISPBufferBaseAddress = ITE_NMF_mapStatisticsMemArea(pBufferMetaData->phyaddress);
    pStatsBufferStorage = (StatsBuffersStorage_t *) pBufferMetaData->logAddress;

    return(pStatsBufferStorage);
}

*/
void
ITE_NMF_FocusStatsPrepare(void)
{
    //ITE_NMF_createFocusStatsBuffer(pFocusStatsBuffer);
    //pFocusStore = g_pFocusStatsBufferStorage;
    ITE_writePE(AFStats_Controls_pu32_HostAssignedAddr_Byte0, pFocusStatsBuffer->ISPBufferBaseAddress);
    ITE_writePE(FrameParamStatus_Af_pu32_HostAssignedFrameStatusAddr_Byte0, pAfFrameStatusBuffer->ISPBufferBaseAddress);    
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_focusmanueltest_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_focusmanueltest_cmd(
int     a_nb_args,
char    **ap_args)
{
    if (a_nb_args > 1)
    {
        if (0 == strcmp(ap_args[1], "help"))
        {
            CLI_disp_error("syntax: fmantest <>\n");
        }
    }
    else
    {
        if (a_nb_args == 1)
        {
            FLADriverManualFocusTest();
        }
        else
        {
            CLI_disp_error("Not correct command arguments\n");
        }
    }


    return (CMD_COMPLETE);
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_focuszonesetup_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_focuszonesetup_cmd(
int     a_nb_args,
char    **ap_args)
{
    if (a_nb_args > 1)
    {
        if (0 == strcmp(ap_args[1], "help"))
        {
            CLI_disp_error("syntax: fzset <>\n");
        }
    }
    else
    {
        if (a_nb_args == 1)
        {
            AFStatsZoneSetup();
        }
        else
        {
            CLI_disp_error("Not correct command arguments\n");
        }
    }


    return (CMD_COMPLETE);
}

/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_simulatesw3acmd_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_simulatesw3acmd_cmd(
int     a_nb_args,
char    **ap_args)
{
    if (a_nb_args > 1)
    {
        if (0 == strcmp(ap_args[1], "help"))
        {
            CLI_disp_error("syntax: fsw3a <>\n");
        }
    }
    else
    {
        if (a_nb_args == 1)
        {
            ITE_NMF_FocusStatsPrepare();
            FocusControl_Simulate_SW3A_Test(
            (t_uint32) (pFocusStatsBuffer->ISPBufferBaseAddress),
            (t_uint32) (pFocusStatsBuffer->logAddress));
        }
        else
        {
            CLI_disp_error("Not correct command arguments\n");
        }
    }


    return (CMD_COMPLETE);
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_focusstatsandcmd_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_focusstatsandcmd_cmd(
int     a_nb_args,
char    **ap_args)
{
    if (a_nb_args > 1)
    {
        if (0 == strcmp(ap_args[1], "help"))
        {
            CLI_disp_error("syntax: fsandc <>\n");
        }
    }
    else
    {
        if (a_nb_args == 1)
        {
            ITE_NMF_FocusStatsPrepare();
            FocusControl_FLADMove_and_Stats_Gather(
            (t_uint32) (pFocusStatsBuffer->ISPBufferBaseAddress),
            (t_uint32) (pFocusStatsBuffer->logAddress));
        }
        else
        {
            CLI_disp_error("Not correct command arguments\n");
        }
    }


    return (CMD_COMPLETE);
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_focusprintmem_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_focusprintmem_cmd(
int     a_nb_args,
char    **ap_args)
{
    if (a_nb_args > 1)
    {
        if (0 == strcmp(ap_args[1], "help"))
        {
            CLI_disp_error("syntax: fprintm <>\n");
        }
    }
    else
    {
        if (a_nb_args == 1)
        {
            ITE_NMF_FocusStatsPrepare();
            FocusControl_SpecifyPrintFormat(
            (t_uint32) (pFocusStatsBuffer->ISPBufferBaseAddress),
            (t_uint32) (pFocusStatsBuffer->logAddress));
        }
        else
        {
            CLI_disp_error("Not correct command arguments\n");
        }
    }


    return (CMD_COMPLETE);
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_focusprintmemstats_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_focusprintmemstats_cmd(
int     a_nb_args,
char    **ap_args)
{
    if (a_nb_args > 1)
    {
        if (0 == strcmp(ap_args[1], "help"))
        {
            CLI_disp_error("syntax: fpmemstat <>\n");
        }
    }
    else
    {
        if (a_nb_args == 1)
        {
            ITE_NMF_FocusStatsPrepare();
            FocusControl_PrintBytesExtMemory(
            (t_uint32) (pFocusStatsBuffer->ISPBufferBaseAddress),
            (t_uint32) (pFocusStatsBuffer->logAddress));
        }
        else
        {
            CLI_disp_error("Not correct command arguments\n");
        }
    }


    return (CMD_COMPLETE);
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_focuszonesetupinper_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_focuszonesetupinper_cmd(
int     a_nb_args,
char    **ap_args)
{
    if (a_nb_args > 1)
    {
        if (0 == strcmp(ap_args[1], "help"))
        {
            CLI_disp_error("syntax: fzsetper <>\n");
        }
    }
    else
    {
        if (a_nb_args == 1)
        {
            AFStats_HostZoneSetup();
            LOS_Log("CALLING STATS READY TEST...\n\n");

            //		  ITE_NMF_FocusStatsPrepare();
            //AFSTatsReadyTest(ap_args[1],(t_uint32)(pFocusStatsBuffer->ISPBufferBaseAddress),(t_uint32)(pFocusStatsBuffer->logAddress));
            //LOS_Log("TEST FINISHED: Result Depends on the AFStats Ready Test...\n\n");
        }
        else
        {
            CLI_disp_error("Not correct command arguments\n");
        }
    }


    return (CMD_COMPLETE);
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_focusmovetoinfinity_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_focusmovetoinfinity_cmd(
int     a_nb_args,
char    **ap_args)
{
    if (a_nb_args > 1)
    {
        if (0 == strcmp(ap_args[1], "help"))
        {
            CLI_disp_error("syntax: ftoinf <>\n");
        }
    }
    else
    {
        if (a_nb_args == 1)
        {
            FLADriverMoveToInfinity();
        }
        else
        {
            CLI_disp_error("Not correct command arguments\n");
        }
    }


    return (CMD_COMPLETE);
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_focusmovetomacro_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_focusmovetomacro_cmd(
int     a_nb_args,
char    **ap_args)
{
    if (a_nb_args > 1)
    {
        if (0 == strcmp(ap_args[1], "help"))
        {
            CLI_disp_error("syntax: ftomacro <>\n");
        }
    }
    else
    {
        if (a_nb_args == 1)
        {
            FLADriverMoveToMacro();
        }
        else
        {
            CLI_disp_error("Not correct command arguments\n");
        }
    }


    return (CMD_COMPLETE);
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_focusmovesteptoinfinity_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_focusmovesteptoinfinity_cmd(
int     a_nb_args,
char    **ap_args)
{
    if (a_nb_args > 2)
    {
        if (0 == strcmp(ap_args[1], "help"))
        {
            CLI_disp_error("syntax: fsteptoinf <step value>\n");
        }
    }
    else
    {
        if (a_nb_args == 2)
        {
            FLADriverMoveStepToInfinity(ITE_ConvToInt16(ap_args[1]));
        }
        else
        {
            CLI_disp_error("Not correct command arguments\n");
        }
    }


    return (CMD_COMPLETE);
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_focusmovesteptomacro_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_focusmovesteptomacro_cmd(
int     a_nb_args,
char    **ap_args)
{
    if (a_nb_args > 2)
    {
        if (0 == strcmp(ap_args[1], "help"))
        {
            CLI_disp_error("syntax: fsteptomacro <step value>\n");
        }
    }
    else
    {
        if (a_nb_args == 2)
        {
            FLADriverMoveStepToMacro(ITE_ConvToInt16(ap_args[1]));
        }
        else
        {
            CLI_disp_error("Not correct command arguments\n");
        }
    }


    return (CMD_COMPLETE);
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_focusmovetorest_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_focusmovetorest_cmd(
int     a_nb_args,
char    **ap_args)
{
    if (a_nb_args > 1)
    {
        if (0 == strcmp(ap_args[1], "help"))
        {
            CLI_disp_error("syntax: ftorest <>\n");
        }
    }
    else
    {
        if (a_nb_args == 1)
        {
            FLADriverMoveToRestPos();
        }
        else
        {
            CLI_disp_error("Not correct command arguments\n");
        }
    }


    return (CMD_COMPLETE);
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_focusmovetohorinfinity_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_focusmovetohorinfinity_cmd(
int     a_nb_args,
char    **ap_args)
{
    if (a_nb_args > 1)
    {
        if (0 == strcmp(ap_args[1], "help"))
        {
            CLI_disp_error("syntax: ftohinf <>\n");
        }
    }
    else
    {
        if (a_nb_args == 1)
        {
            FLADriverMoveToHorInfinityPos();
        }
        else
        {
            CLI_disp_error("Not correct command arguments\n");
        }
    }


    return (CMD_COMPLETE);
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_focusmovetohyperfocal_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_focusmovetohyperfocal_cmd(
int     a_nb_args,
char    **ap_args)
{
    if (a_nb_args > 1)
    {
        if (0 == strcmp(ap_args[1], "help"))
        {
            CLI_disp_error("syntax: ftohyp <>\n");
        }
    }
    else
    {
        if (a_nb_args == 1)
        {
            FLADriverMoveToHyperfocal();
        }
        else
        {
            CLI_disp_error("Not correct command arguments\n");
        }
    }


    return (CMD_COMPLETE);
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_focusmovetohormacro_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_focusmovetohormacro_cmd(
int     a_nb_args,
char    **ap_args)
{
    if (a_nb_args > 1)
    {
        if (0 == strcmp(ap_args[1], "help"))
        {
            CLI_disp_error("syntax: ftohmacro <>\n");
        }
    }
    else
    {
        if (a_nb_args == 1)
        {
            FLADriverMoveToHorMacro();
        }
        else
        {
            CLI_disp_error("Not correct command arguments\n");
        }
    }


    return (CMD_COMPLETE);
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_focusmovetotargetpos_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_focusmovetotargetpos_cmd(
int     a_nb_args,
char    **ap_args)
{
    if (a_nb_args > 2)
    {
        if (0 == strcmp(ap_args[1], "help"))
        {
            CLI_disp_error("syntax: ftotargetpos <step value>\n");
        }
    }
    else
    {
        if (a_nb_args == 2)
        {
            FLADriverMoveToTargetPos(ITE_ConvToInt16(ap_args[1]));
        }
        else
        {
            CLI_disp_error("Not correct command arguments\n");
        }
    }


    return (CMD_COMPLETE);
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_focusshifttohostfocussystem_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_focusshifttohostfocussystem_cmd(
int     a_nb_args,
char    **ap_args)
{
    if (a_nb_args > 1)
    {
        if (0 == strcmp(ap_args[1], "help"))
        {
            CLI_disp_error("syntax: fhfs <>\n");
        }
    }
    else
    {
        if (a_nb_args == 1)
        {
            AFStats_HostFocusSystem();
        }
        else
        {
            CLI_disp_error("Not correct command arguments\n");
        }
    }


    return (CMD_COMPLETE);
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_focusstatreadytest_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_focusstatreadytest_cmd(
int     a_nb_args,
char    **ap_args)
{
   Result_te result = Result_e_Success;
   char mess[256];
   MMTE_TEST_START("focusstatreadytest","/ite_nmf/test_results_nreg/focus_tests","Testing for focusstatreadytest");
   snprintf(mess,sizeof(mess),"focusstatreadytest");
   MMTE_TEST_NEXT("focusstatreadytest");

    if (a_nb_args > 2)
    {
        if (0 == strcmp(ap_args[1], "help"))
        {
            CLI_disp_error("syntax: fstattest <filename>\n");
            MMTE_TEST_SKIPPED();
        }
    }
    else
    {
        if (a_nb_args == 2)
        {
            ITE_NMF_FocusStatsPrepare();
            result = AFSTatsReadyTest(ap_args[1],
            (t_uint32) (pFocusStatsBuffer->ISPBufferBaseAddress),
            (t_uint32) (pFocusStatsBuffer->logAddress));

            if(result == Result_e_Success)
             {
               MMTE_TEST_PASSED();
             }
            else
             {
              MMTE_TEST_FAILED();
             }
        }
        else
        {
            CLI_disp_error("Not correct command arguments\n");
            MMTE_TEST_SKIPPED();
        }
    }


    return (CMD_COMPLETE);
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_focusframeidtest_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_focusframeidtest_cmd(
int     a_nb_args,
char    **ap_args)
{
    if (a_nb_args > 1)
    {
        if (0 == strcmp(ap_args[1], "help"))
        {
            CLI_disp_error("syntax: fidtest\n");
        }
    }
    else
    {
        if (a_nb_args == 1)
        {
            ITE_NMF_FocusStatsPrepare();
            AFStats_FrameID_Test(
            (t_uint32) (pFocusStatsBuffer->ISPBufferBaseAddress),
            (t_uint32) (pFocusStatsBuffer->logAddress));
        }
        else
        {
            CLI_disp_error("Not correct command arguments\n");
        }
    }


    return (CMD_COMPLETE);
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_focushostzonewithzoom_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_focushostzonewithzoom_cmd(
int     a_nb_args,
char    **ap_args)
{
    if (a_nb_args > 1)
    {
        if (0 == strcmp(ap_args[1], "help"))
        {
            CLI_disp_error("syntax: fhzwz <>\n");
        }
    }
    else
    {
        if (a_nb_args == 1)
        {
            AFStats_HostZoneWithZoom();
        }
        else
        {
            CLI_disp_error("Not correct command arguments\n");
        }
    }


    return (CMD_COMPLETE);
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_focusonlyzoom_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_focusonlyzoom_cmd(
int     a_nb_args,
char    **ap_args)
{
    if (a_nb_args > 1)
    {
        if (0 == strcmp(ap_args[1], "help"))
        {
            CLI_disp_error("syntax: fozoom <>\n");
        }
    }
    else
    {
        if (a_nb_args == 1)
        {
            AFStats_OnlyZoomTest();
        }
        else
        {
            CLI_disp_error("Not correct command arguments\n");
        }
    }


    return (CMD_COMPLETE);
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_focuszoombug_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_focuszoombug_cmd(
int     a_nb_args,
char    **ap_args)
{
    if (a_nb_args > 1)
    {
        if (0 == strcmp(ap_args[1], "help"))
        {
            CLI_disp_error("syntax: fzbug <>\n");
        }
    }
    else
    {
        if (a_nb_args == 1)
        {
            AFStats_ZoomBugTest();
        }
        else
        {
            CLI_disp_error("Not correct command arguments\n");
        }
    }


    return (CMD_COMPLETE);
}


/* -----------------------------------------------------------------------
	   FUNCTION  : C_ite_dbg_focusbug107051_cmd
	   PURPOSE   :
------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_focusbug107051_cmd(
int     a_nb_args,
char    **ap_args)
{
    if (a_nb_args > 1)
    {
        if (0 == strcmp(ap_args[1], "help"))
        {
            CLI_disp_error("syntax: fzbug <>\n");
        }
    }
    else
    {
        if (a_nb_args == 1)
        {
            ITE_NMF_FocusStatsPrepare();
            FocusControl_BugTest_107051(
            (t_uint32) (pFocusStatsBuffer->ISPBufferBaseAddress),
            (t_uint32) (pFocusStatsBuffer->logAddress));
        }
        else
        {
            CLI_disp_error("Not correct command arguments\n");
        }
    }


    return (CMD_COMPLETE);
}


/* -----------------------------------------------------------------------
	   FUNCTION  : C_ite_dbg_focusabsread_cmd
	   PURPOSE   :
------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_focusabsread_cmd(
int     a_nb_args,
char    **ap_args)
{
    if (a_nb_args > 1)
    {
        if (0 == strcmp(ap_args[1], "help"))
        {
            CLI_disp_error("syntax: fzaread <>\n");
        }
    }
    else
    {
        if (a_nb_args == 1)
        {
            AFStats_WindowConfigReads();
        }
        else
        {
            CLI_disp_error("Not correct command arguments\n");
        }
    }


    return (CMD_COMPLETE);
}


/* -----------------------------------------------------------------------
	   FUNCTION  : C_ite_dbg_focusconfigread_cmd
	   PURPOSE   :
------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_focusconfigread_cmd(
int     a_nb_args,
char    **ap_args)
{
    if (a_nb_args > 1)
    {
        if (0 == strcmp(ap_args[1], "help"))
        {
            CLI_disp_error("syntax: fzcread <>\n");
        }
    }
    else
    {
        if (a_nb_args == 1)
        {
            AFStats_ZoneAbsDimensions();
        }
        else
        {
            CLI_disp_error("Not correct command arguments\n");
        }
    }


    return (CMD_COMPLETE);
}


/* -----------------------------------------------------------------------
	   FUNCTION  : C_ite_dbg_focuswindowsystem_cmd
	   PURPOSE   :
------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_focuswindowsystem_cmd(
int     a_nb_args,
char    **ap_args)
{
    if (a_nb_args > 1)
    {
        if (0 == strcmp(ap_args[1], "help"))
        {
            CLI_disp_error("syntax: fwtest <>\n");
        }
    }
    else
    {
        if (a_nb_args == 1)
        {
            AFStats_AFWindowSystemTest();
        }
        else
        {
            CLI_disp_error("Not correct command arguments\n");
        }
    }


    return (CMD_COMPLETE);
}



/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_focusbugfindingtest_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_focusbugfindingtest_cmd(
int     a_nb_args,
char    **ap_args)
{
    if (a_nb_args > 1)
    {
        if (0 == strcmp(ap_args[1], "help"))
        {
            CLI_disp_error("syntax: fbugft <>\n");
        }
    }
    else
    {
        if (a_nb_args == 1)
        {
            LOS_Log("\n*******************************************\n");
            LOS_Log("\nBUG      FIND       TEST \n");
            LOS_Log("\n*******************************************\n");
            LOS_Log("\n\n\n\n");
            AFStats_BugFindingTest();

            // calling zoom bug test
            LOS_Log("\n----------------------------------------------\n");
            LOS_Log("\nZOOM     BUG      TEST \n");
            LOS_Log("\n----------------------------------------------\n");
            LOS_Log("\n\n\n\n");
            AFStats_ZoomBugTest();

            // calling stats dump test
            LOS_Log("\n----------------------------------------------\n");
            LOS_Log("\nFOCUS     STATS       DUMP       TEST \n");
            LOS_Log("\n----------------------------------------------\n");
            LOS_Log("\n\n\n\n");

            ITE_NMF_FocusStatsPrepare();
            AFSTatsReadyTest(
            "dump.txt",
            (t_uint32) (pFocusStatsBuffer->ISPBufferBaseAddress),
            (t_uint32) (pFocusStatsBuffer->logAddress));

            // Manual focus test
            LOS_Log("\n----------------------------------------------\n");
            LOS_Log("\nMANUAL      FOCUS      TEST \n");
            LOS_Log("\n----------------------------------------------\n");
            LOS_Log("\n\n\n\n");
            FLADriverManualFocusTest();

            // calling stats dump test
            LOS_Log("\n----------------------------------------------\n");
            LOS_Log("\nZONE        SETUP       TEST \n");
            LOS_Log("\n----------------------------------------------\n");
            LOS_Log("\n\n\n\n");

            AFStatsZoneSetup();

            // calling stats and command test together ,
            // calling stats BUG test
            LOS_Log("\n----------------------------------------------\n");
            LOS_Log("\n  			BUG  TEST : 107051     \n");
            LOS_Log("\n----------------------------------------------\n");
            LOS_Log("\n\n\n\n");

            ITE_NMF_FocusStatsPrepare();
            FocusControl_BugTest_107051(
            (t_uint32) (pFocusStatsBuffer->ISPBufferBaseAddress),
            (t_uint32) (pFocusStatsBuffer->logAddress));

            // calling stats dump test
            LOS_Log("\n----------------------------------------------\n");
            LOS_Log("\nMANUAL    COMMAND    AND    THE    STATS    GATHER    TEST \n");
            LOS_Log("\n----------------------------------------------\n");
            LOS_Log("\n\n\n\n");

            ITE_NMF_FocusStatsPrepare();
            FocusControl_FLADMove_and_Stats_Gather(
            (t_uint32) (pFocusStatsBuffer->ISPBufferBaseAddress),
            (t_uint32) (pFocusStatsBuffer->logAddress));

            // calling again the zone setup test
            // calling stats dump test
            LOS_Log("\n----------------------------------------------\n");
            LOS_Log("\nZONE        SETUP       TEST \n");
            LOS_Log("\n----------------------------------------------\n");
            LOS_Log("\n\n\n\n");

            AFStatsZoneSetup();

            //afstats dump test
            // calling stats dump test
            LOS_Log("\n----------------------------------------------\n");
            LOS_Log("\nFOCUS     STATS       DUMP       TEST \n");
            LOS_Log("\n----------------------------------------------\n");
            LOS_Log("\n\n\n\n");

            AFSTatsReadyTest(
            "dump.txt",
            (t_uint32) (pFocusStatsBuffer->ISPBufferBaseAddress),
            (t_uint32) (pFocusStatsBuffer->logAddress));

            // calling complete zoom test .
            // calling stats dump test
            LOS_Log("\n----------------------------------------------\n");
            LOS_Log("\nCOMPLETE ZOOM TEST WITHOUT FOCUS MANAGEMENT");
            LOS_Log("\n----------------------------------------------\n");
            LOS_Log("\n\n\n\n");

            AFStats_OnlyZoomTest();

            // manual fous test
            // Manual focus test
            LOS_Log("\n----------------------------------------------\n");
            LOS_Log("\nMANUAL      FOCUS      TEST \n");
            LOS_Log("\n----------------------------------------------\n");
            LOS_Log("\n\n\n\n");

            FLADriverManualFocusTest();

            // Manual focus test
            LOS_Log("\n----------------------------------------------\n");
            LOS_Log("\n				FRAME ID TEST 					\n");
            LOS_Log("\n----------------------------------------------\n");
            LOS_Log("\n\n\n\n");

            ITE_NMF_FocusStatsPrepare();
            AFStats_FrameID_Test(
            (t_uint32) (pFocusStatsBuffer->ISPBufferBaseAddress),
            (t_uint32) (pFocusStatsBuffer->logAddress));

            // calling host zone setup test .
            LOS_Log("\n----------------------------------------------\n");
            LOS_Log("\nHOST 		ZONE 		SETUP 		TEST\n");
            LOS_Log("\n----------------------------------------------\n");
            LOS_Log("\n\n\n\n");

            AFStats_HostZoneSetup();

            //afstats dump test
            LOS_Log("\n----------------------------------------------\n");
            LOS_Log("\nFOCUS     STATS       DUMP       TEST \n");
            LOS_Log("\n----------------------------------------------\n");
            LOS_Log("\n\n\n\n");

            AFSTatsReadyTest(
            "dump.txt",
            (t_uint32) (pFocusStatsBuffer->ISPBufferBaseAddress),
            (t_uint32) (pFocusStatsBuffer->logAddress));

            // host zones with zoom test
            LOS_Log("\n----------------------------------------------\n");
            LOS_Log("\nHOST     ZONE        WITH 		ZOOM       TEST \n");
            LOS_Log("\n----------------------------------------------\n");
            LOS_Log("\n\n\n\n");

            AFStats_HostZoneWithZoom();

            // complete zoom test
            LOS_Log("\n----------------------------------------------\n");
            LOS_Log("\nONLY ZOOM TEST - NO FOCUS MANAGEMENT TEST \n");
            LOS_Log("\n----------------------------------------------\n");
            LOS_Log("\n\n\n\n");

            AFStats_OnlyZoomTest();

            LOS_Log("\n----------------------------------------------\n");
            LOS_Log("\n    AF  WINDOW SYSTEM TEST\n");
            LOS_Log("\n----------------------------------------------\n");
            LOS_Log("\n\n\n\n");

            AFStats_AFWindowSystemTest();
            LOS_Log("\n************************************************\n");
            LOS_Log("\nBUG 			FINDING 			TEST 		DONE \n");
            LOS_Log("\n************************************************\n");
            LOS_Log("\n\n\n\n");
        }
        else
        {
            CLI_disp_error("Not correct command arguments\n");
        }
    }


    return (CMD_COMPLETE);
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_focushelp_cmd
   PURPOSE  :
   ------------------------------------------------------------------------ */
CMD_COMPLETION
C_ite_dbg_focushelp_cmd(
int     a_nb_args,
char    **ap_args)
{
 UNUSED(ap_args);
    if (a_nb_args == 1)
    {
        CLI_disp_error("\n");
        CLI_disp_error("focusmanualtest         				:Noida Focus test script\n");
        CLI_disp_error(" 	             fmantest 					<>\n");
        CLI_disp_error("focusstatreadytest      				:Noida Stat test script\n");
        CLI_disp_error(" 	             fstattest 					<FILENAME>\n");
        CLI_disp_error("focuszonesetup          				:Init focus zone\n");
        CLI_disp_error("	 	         fzset 						<>\n");
        CLI_disp_error("focuszonesetupinper          			:Init focus zone in per\n");
        CLI_disp_error("	 	         fzsetper 					<>\n");
        CLI_disp_error("focusstatsandcmd          				:Stats and Lens command Together\n");
        CLI_disp_error("	 	         fsandc 					<>\n");
        CLI_disp_error("focusmovetoinfinity     				:Move lens to infinity\n");
        CLI_disp_error("	 	         ftoinf 					<>\n");
        CLI_disp_error("focusmovetomacro        				:Move lens to macro\n");
        CLI_disp_error("	 	         ftomacro 					<>\n");
        CLI_disp_error("focusmovesteptoinfinity 				:Move step value lens to infinity\n");
        CLI_disp_error(" 	             fsteptoinf 				<step value>\n");
        CLI_disp_error("focusmovesteptomacro    				:Move step value lens to macro\n");
        CLI_disp_error("	 	         fsteptomacro 				<step value>\n");
        CLI_disp_error("focusmovetorest    						:Move to Rest position\n");
        CLI_disp_error("				 ftorest 					<>\n");
        CLI_disp_error("focusmovetohorinfinity    				:Move to Hor Infinity position\n");
        CLI_disp_error("				 ftohinf 					<>\n");
        CLI_disp_error("focusmovetohormacro    					:Move to Hor Macro position\n");
        CLI_disp_error("				 ftohmacro 					<>\n");
        CLI_disp_error("focusmovetohyperfocal    				:Move to Hyperfocal  position\n");
        CLI_disp_error("				 ftohyp 					<>\n");
        CLI_disp_error("focusmovetotargetpos    				:Move lens to pos position\n");
        CLI_disp_error(" 	             ftotargetpos 				<pos value>\n");
        CLI_disp_error("focusshifttohostfocussystem				:Select the Host Focus System.\n");
        CLI_disp_error(" 	             fhfs 						<>\n");
        CLI_disp_error("focushostzonewithzoom					:Focus Change With the Zoom System.\n");
        CLI_disp_error(" 	             fhzwz 						<>\n");
        CLI_disp_error("focusonlyzoom							:RUN ZOOM without Focus.\n");
        CLI_disp_error(" 	             fozoom 					<>\n");
        CLI_disp_error("focuszoombug							:zoom bug test.\n");
        CLI_disp_error(" 	             fzbug 						<>\n");
        CLI_disp_error("focusbugfindingtest						:Bug Finding Test.\n");
        CLI_disp_error(" 	             fbugft 					<>\n");
        CLI_disp_error("focusbug107051							:Bug 107051 Test.\n");
        CLI_disp_error(" 	             fb107051 					<>\n");
        CLI_disp_error("focusconfigread 						:Read AF HostZone configuration.\n");
        CLI_disp_error("				 fzcread 					<>\n");
        CLI_disp_error("focuswindowsystem 						:AF window System Test.\n");
        CLI_disp_error("				 fwtest 					<>\n");
        CLI_disp_error("focusabsread 							:READ AF ABSOLUTE VALUES \n");
        CLI_disp_error("				 fzaread 					<>\n");
        CLI_disp_error("focusframeidtest							:READ FRMAE ID OF THE FRAME WHIO's STATS ARE EXPORTED TO MEMORY \n");
        CLI_disp_error("				 fidtest					<>\n");
        CLI_disp_error("focusprintmemstats							:PRINT MEMORY ON CONSOLE WITH AFSTATISTICS EXPORTED \n");
        CLI_disp_error("				 fpmemstat					<>\n");
        CLI_disp_error("focusprintmem							:PRINT MEMORY SPECIFIED BY HOST \n");
        CLI_disp_error("				 fprintm					<>\n");
    }
    else
    {
        CLI_disp_error("Not correct command arguments\n");
    }


    return (CMD_COMPLETE);
}


t_cmd_list  ite_cmd_list_focus[] =
{
    { "focushelp", C_ite_dbg_focushelp_cmd, "focushelp: focushelp\n" },
    { "fmantest", C_ite_dbg_focusmanueltest_cmd, "focusmanualtest: fmantest <FILENAME>\n" },
    { "fstattest", C_ite_dbg_focusstatreadytest_cmd, "focusstatreadytest: fstattest <>\n" },
    { "fzset", C_ite_dbg_focuszonesetup_cmd, "focuszonesetup: fzset <>\n" },
    { "fzsetper", C_ite_dbg_focuszonesetupinper_cmd, "focuszonesetupinper: fzsetper <>\n" },
    { "fsw3a", C_ite_dbg_simulatesw3acmd_cmd, "focussimulatesw3acmd: fsw3a <>\n" },
    { "fsandc", C_ite_dbg_focusstatsandcmd_cmd, "focusstatsandcmd: fsandc <>\n" },
    { "ftoinf", C_ite_dbg_focusmovetoinfinity_cmd, "focusmovetoinfinity: ftoinf <>\n" },
    { "ftomacro", C_ite_dbg_focusmovetomacro_cmd, "focusmovetomacro: ftomacro <>\n" },
    { "fsteptoinf", C_ite_dbg_focusmovesteptoinfinity_cmd, "focusmovesteptoinfinity: fsteptoinf <>\n" },
    { "fsteptomacro", C_ite_dbg_focusmovesteptomacro_cmd, "focusmovesteptomacro: fsteptomacro <>\n" },
    { "ftorest", C_ite_dbg_focusmovetorest_cmd, "focusmovetorest: ftorest <>\n" },
    { "ftohinf", C_ite_dbg_focusmovetohorinfinity_cmd, "focusmovetohorinfinity: ftohinf <>\n" },
    { "ftohmacro", C_ite_dbg_focusmovetohormacro_cmd, "focusmovetohormacro: ftohmacro <>\n" },
    { "ftohyp", C_ite_dbg_focusmovetohyperfocal_cmd, "focusmovetohyperfocal: ftohyp <>\n" },
    { "ftotargetpos", C_ite_dbg_focusmovetotargetpos_cmd, "focusmovetotargetpos: ftotargetpos <>\n" },
    { "fhfs", C_ite_dbg_focusshifttohostfocussystem_cmd, "focusshifttohostfocussystem : fhfs <>\n" },
    { "fhzwz", C_ite_dbg_focushostzonewithzoom_cmd, "focushostzonewithzoom: fhzwz <>\n" },
    { "fozoom", C_ite_dbg_focusonlyzoom_cmd, "focusonlyzoom: fozoom <>\n" },
    { "fzbug", C_ite_dbg_focuszoombug_cmd, "focuszoombug: fzbug <>\n" },
    { "fbugft", C_ite_dbg_focusbugfindingtest_cmd, "focuszoombug: fbugft <>\n" },
    { "fb107051", C_ite_dbg_focusbug107051_cmd, "focusbug107051: fb107051 <>\n" },
    { "fzcread", C_ite_dbg_focusconfigread_cmd, "focusconfigread: fzcread <>\n" },
    { "fwtest", C_ite_dbg_focuswindowsystem_cmd, "focuswindowsystem: fwtest <>\n" },
    { "fzaread", C_ite_dbg_focusabsread_cmd, "focusabsread: fzaread <>\n" },
    { "fidtest", C_ite_dbg_focusframeidtest_cmd, "focusframeidtest: fidtest <>\n" },
    { "fpmemstat", C_ite_dbg_focusprintmemstats_cmd, "focusprintmemstats: fpmemstat <>\n" },
    { "fprintm", C_ite_dbg_focusprintmem_cmd, "focusprintmem: fprintm <>\n"},
};

void
Init_focus_ITECmdList(void)
{
    CLI_register_interface(
    "FOCUS_ITE_CMB",
    sizeof(ite_cmd_list_focus) / (sizeof(ite_cmd_list_focus[0])),
    ite_cmd_list_focus,
    1);
}

