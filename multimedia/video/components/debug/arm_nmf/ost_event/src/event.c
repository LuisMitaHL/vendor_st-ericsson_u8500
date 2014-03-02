/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <debug/arm_nmf/ost_event.nmf>

//#ifdef __SYMBIAN32__
//#else
#include <string.h>

#include <los/api/los_api.h>

typedef struct my_timer
{
	t_uint32	n;
	t_uint32	max;
	t_uint32	stop;
	t_uint32	start;
	t_uint32	sum;
	char		*name;
}
timer_t;

#define TIMER_ARRAY_SIZE	10

static	timer_t	timer[TIMER_ARRAY_SIZE];



t_construct_error METH(construct)()
{
	memset(timer,0,sizeof(timer[TIMER_ARRAY_SIZE]));
	return CONSTRUCT_OK;
}



void METH(OstTraceEventStart0)(t_uint32 aTraceName,char	* aEventName)
{
	timer[aTraceName].start=LOS_getSystemTime();
	timer[aTraceName].name= aEventName;
}

void METH(OstTraceEventStop)(t_uint32 aTraceName,char *aEventName)
{
	t_uint32	d;
	timer[aTraceName].stop=LOS_getSystemTime();
	d=timer[aTraceName].stop-timer[aTraceName].start;
	if (d<0) d= 0xffffffffU+d+1;
	if (d>timer[aTraceName].max) 
	{
		//LOS_Log("Event %d %s delta new max %d\n",timer[aTraceName].n,timer[aTraceName].name,d);
		timer[aTraceName].max=d;
	}
	timer[aTraceName].sum+=d;
	timer[aTraceName].n++;
}



void DisplayStatus()
{
	t_uint32	i;

	for(i=0;i<TIMER_ARRAY_SIZE;i++)
	{
		if (timer[i].name)
		{
			LOS_Log("Event %s occur %d times\n",timer[i].name,timer[i].n);
			LOS_Log("\tmax %d\n",timer[i].max);
			LOS_Log("\tcumul %d\n",timer[i].sum);
			if (timer[i].n) LOS_Log("\taverage %d\n",timer[i].sum/timer[i].n);
		}


	}
}

void METH(destroy)(void)
{
	DisplayStatus();

}

