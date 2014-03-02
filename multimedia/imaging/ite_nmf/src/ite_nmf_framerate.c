/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include "ite_nmf_framerate.h"
#include "ssc.h"
#include "ite_vpip.h"
#include "ite_convf900.h"
#include <los/api/los_api.h>
#include "grab_types.idt.h"
#include <inc/typedef.h>
#include "ite_main.h"
#include "cli.h"


/********************************************************/
/*  float  ITE_NMF_ReadFramerate()		        */
/*         					        */
/*  read framerate from Page element     		*/
/*  return Read PE	               		        */
/********************************************************/   
//ported on 8500
t_uint32  ITE_NMF_CheckReadFramerate(float framerate_in)
{
   volatile t_uint32 page_element;
   UNUSED(framerate_in);
   t_uint32 error=FALSE;
   //float flout;
   page_element=ITE_readPE(FrameDimensionStatus_f_CurrentFrameRate_Byte0);
   //ITE_ConvertTo32BitFloat(page_elementMSB, &flout);
   //flout= (float) page_element;
   LOS_Log("Read Framerate from PE = %f fr/sec \n", *(volatile float *)&page_element, NULL, NULL, NULL, NULL, NULL);
   
   //if( (page_element<=((framerate_in)+2)) && (page_element>=((framerate_in)-2)) ) error=page_element; 
   error=page_element;
   
   return error;
}


/****************************************************************/
/*  t_uint8 ITE_NMF_CheckFrameRateCount(enum e_grabPipeID pipe, */
/*  					t_uint16 framerate)     */
/*  count number of frames in 2000ms			        */
/*  return 0 if OK / Measured framerate if error		               		        */
/****************************************************************/
//ported on 8500
t_uint32 ITE_NMF_CheckFrameRateCount(enum e_grabPipeID pipe,float framerate_in)
{
   t_uint32 page_element0,page_element1;
   t_uint32 maesuredFrameRate;
   t_uint32 add=0;
   t_uint32 error=FALSE;
   //unsigned long time0,time1;
   
   //time0=LOS_getSystemTime();
   if (pipe == GRBPID_PIPE_HR) {
      page_element0=ITE_readPE(PipeStatus_0_u8_FramesStreamedOutOfPipeLastRun_Byte0);
      //LOS_Sleep(1950);
	  LOS_Sleep(2000);
	  
	  page_element1=ITE_readPE(PipeStatus_0_u8_FramesStreamedOutOfPipeLastRun_Byte0);
      }
   else {
      page_element0=ITE_readPE(PipeStatus_1_u8_FramesStreamedOutOfPipeLastRun_Byte0);
      //LOS_Sleep(1950);
	  LOS_Sleep(2000);
	  
      page_element1=ITE_readPE(PipeStatus_1_u8_FramesStreamedOutOfPipeLastRun_Byte0);
      }
   //time1=LOS_getSystemTime();
   
   if(page_element1<page_element0) add=255;

   //LOS_Log("time0 %d time1 %d\n",time0,time1);
   //LOS_Log("nb0 %d nb1 %d\n",page_element0,page_element1);
   
   maesuredFrameRate=(t_uint32) ((page_element1+add-page_element0)/2);
   
   LOS_Log("Measure Framerate (int32) = %d fr/sec \n\n", maesuredFrameRate, NULL, NULL, NULL, NULL, NULL);
   LOS_Log("Measure Framerate (float) = %.3f fr/sec \n\n", ((page_element1+add-page_element0)/(float)2), NULL, NULL, NULL, NULL, NULL);
   
    if( (maesuredFrameRate<=((framerate_in)+2)) && (maesuredFrameRate>=((framerate_in)-2)) ) 
		error=0; //Test OK
	else error=maesuredFrameRate; //Test fail , error contains mesured framerate
   
   return error;
}
