/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _TIMED_TASK_
#define _TIMED_TASK_

#include "OMX_Index.h"
#include "OMX_Core.h"
#include "IFM_Types.h"

/*
time_TT_task -  Holds the timing information of timed_tasks
Parameters            data Type       description
---------------------------------------------------------------
current_time           OMX_U16	    Hold the current time if enabled.
								    Default = 0
timeout                OMX_U16      time out value
									Default = 0
timer_running          bool         Timer Status Running/Stop
									Default = 0(stop)

*/

typedef struct time_task
{
	bool timer_running;
	OMX_U16 current_time;
	OMX_U16 timeout;
}OMX_TIME_TT;

typedef enum 
{
	TT_ISP_START_STREAMING_REQ,	//ISP_START_STREAMING
	TT_ISP_STOP_STREAMING_REQ,	//ISP_STOP_STREAMING
	TT_GRAB_CONFIG_REQ,			//configureGrabResolutionFormat - CONFIGURE_SIG
	TT_SW3A_STOP_REQ,			//SW3A_STOP
	TT_MAX
} t_timed_task;

#define TT_TIMEOUT 6000 //In msec multiple of 1000 as timer is running on 1sec tick

#endif /*_TIMED_TASK_*/
