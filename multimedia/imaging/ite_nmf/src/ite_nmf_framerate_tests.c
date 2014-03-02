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
#include "ite_nmf_framerate_tests.h"

#include "ite_boardinfo.h"
#include "ite_platform.h"
#include "VhcElementDefs.h"

//for NMF
#include "ite_sia_interface_data.h"
#include "ite_sia_bootcmd.h"
#include "ite_init.h"
#include "ite_sia_init.h"
#include "ite_display.h"
#include "ite_event.h"
#include "ite_pageelements.h"
#include "ite_colormatrix.h"
#include "ite_grab.h"
#include "ite_vpip.h"
#include "ite_framerate.h"
#include "ite_sia_buffer.h"
#include "ite_buffer_management.h"
#include "grab_types.idt.h"

#include "ite_nmf_standard_functions.h"
#include "ite_nmf_framerate.h"

#include <test/api/test.h>
#include <cm/inc/cm_macros.h>

#include <los/api/los_api.h>
#include "ite_main.h"


extern struct s_grabParams grabparamsHR;
extern struct s_grabParams grabparamsLR;
extern volatile t_uint32 g_grabHR_infinite;
extern volatile t_uint32 g_grabLR_infinite;
extern ts_siapicturebuffer GrabBufferHR[];
extern ts_siapicturebuffer LCDBuffer;
extern ts_siapicturebuffer GamBuffer;

extern ts_sia_usecase usecase;
extern ts_sensInfo SensorsInfo[2];
extern t_uint32 ite_sensorselect;
extern t_uint32 testNum;

/* -----------------------------------------------------------------------
   FUNCTION : C_ite_FramerateTests_cmd
   PURPOSE  : Command for testing manual framerate
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_FramerateTests_cmd(int a_nb_args, char ** ap_args)
{

   if (a_nb_args == 3)
   {
      ITE_FramerateTest(ap_args[1], ap_args[2]);
   }
   else
   {
      CLI_disp_error("Not correct command arguments\n");
   }
   return CMD_COMPLETE;
}


/* -----------------------------------------------------------------------
   FUNCTION : ITE_FramerateTest
   PURPOSE  : Test manual framerate
   ------------------------------------------------------------------------ */
//ported on 8500
void ITE_FramerateTest(char * ap_test_id, char * ap_grabvpip_options)
{
   ITE_Log_RW_pageelements_enable();

   ITE_Log_Event_disable();

   mmte_testStart("test_manual_framerate"," test Framerate", g_out_path);

   ITE_InitUseCase(ap_test_id, ap_grabvpip_options,&usecase);

   ITE_Start_Env(&usecase,ap_grabvpip_options,0,0);

   mmte_testComment("Set AntiFlickerExposureControls_fGuaranteeStaticFlickerFrameLength to FALSE to guaranty static framerate is respected");
   ITE_writePE(HostFrameConstraints_e_Flag_GuaranteeStaticFlickerFrameLength_Byte0,Flag_e_FALSE);

   ITE_LR_Prepare(&usecase, INFINITY);

   //ITE_SetFrameRateControl(FrameRateMode_Manual,framerate,5,30);

   ITE_LR_Start(INFINITY);

   //2 secs viewfinder
   LOS_Log("\n2secs stream on LR pipe\n");
   LOS_Sleep(2000);

   #if defined(__PEPS8500_SIA)
   ITE_unitaryTestStaticFramerate(1.0,GRBPID_PIPE_LR);
   ITE_unitaryTestStaticFramerate(2.0,GRBPID_PIPE_LR);
   ITE_unitaryTestStaticFramerate(3.0,GRBPID_PIPE_LR);
   ITE_unitaryTestStaticFramerate(4.0,GRBPID_PIPE_LR);
   ITE_unitaryTestStaticFramerate(5.0,GRBPID_PIPE_LR);
   ITE_unitaryTestStaticFramerate(6.0,GRBPID_PIPE_LR);

   #else

   ITE_unitaryTestStaticFramerate(5.0,GRBPID_PIPE_LR);
   ITE_unitaryTestStaticFramerate(15.0,GRBPID_PIPE_LR);
   ITE_unitaryTestStaticFramerate(30.0,GRBPID_PIPE_LR);
   //check on high framerate only on swordfish sensor

   if IS_TOSH_SENSOR_USED
   //if (SensorsInfo[ite_sensorselect].revision.identity==8510)
   {
	   ITE_unitaryTestStaticFramerate(60.0,GRBPID_PIPE_LR);
	   ITE_unitaryTestStaticFramerate(90.0,GRBPID_PIPE_LR);
	   //not achievable with this config: max is 109fps : change HostRXMaxDataRate to achieve 120fps
	   ITE_unitaryTestStaticFramerate(120.0,GRBPID_PIPE_LR);
   }

   #endif

   ITE_LR_Stop();

   ITE_LR_Free();

   ITE_Stop_Env();

   // end test
   mmte_testEnd();


}

Result_te ITE_unitaryTestStaticFramerate(float manualFr, enum e_grabPipeID pipeId)
{
	   t_uint32 error=1;
	   volatile t_uint32 readPE;
	   char mess[256];
    Result_te result = Result_e_Success;

	   // test frame rate
	   snprintf(mess, sizeof(mess), "Test %d :check static frame rate %f f/s",(int)testNum,manualFr);
	   mmte_testNext(mess);
	   //set suitable framerate
	   //ITE_SetFrameRateControl(manualFr,manualFr);
	   //LOS_Sleep(2000);
	   readPE=ITE_NMF_CheckReadFramerate(manualFr);
	   LOS_Log("check Framerate : expect %f got %f\n", manualFr,*(float *)&readPE);
	   // check framerate
	   error=ITE_NMF_CheckFrameRateCount(pipeId,manualFr);
	   if (error== 0)
	   {
	   	// end test
	   	mmte_testResult(TEST_PASSED);
        result &= Result_e_Success;


	   }
	   else
	   {
	    // end test
		//PE have been processed => float conversion no more available
	   	snprintf(mess, sizeof(mess), "measured frame rate %d f/s",(int) error); //__NO_WARNING__
		mmte_testComment(mess);
		mmte_testResult(TEST_FAILED);
        result &= Result_e_Failure;

	   }

    return result;
}

