/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifdef LOCAL_OST_TRACE_EVENT
#include <string.h>
#include "types.h"
#include "host_types.h"

#include <ost_event.h>
#include <los/api/los_api.h>

typedef struct my_timer
{
	t_uint32	n;
	t_uint32	max;
	t_uint32	max_idx;
	t_uint32	stop;
	t_uint32	start;
	t_uint32	sum;
	char		*name;
}
timer_t;

#define TIMER_ARRAY_SIZE	20

static	timer_t	timer[TIMER_ARRAY_SIZE];
static	t_uint32	offset;
#ifdef OSTLOG
#define LOGMAX	10000
t_uint64	logTime[LOGMAX+1];
t_uint32	logEvent[LOGMAX+1];
t_uint32	logwrap,logcounter;
#endif


#ifdef WORKSTATION
#define	LOS_getSystemTime(a)	0
#endif
extern "C"
{
void ILOS_sxaTimer90KhzStart(void);
}
void MeasureEventInit(void)
{
	 memset(timer,0,sizeof(timer[TIMER_ARRAY_SIZE]));
	 MeasureEventStart0(TRACE_EVENT_CALIBRATION_STOP,"Calibration");
	 MeasureEventStop(TRACE_EVENT_CALIBRATION_STOP,"Calibration");
 	 MeasureEventStart0(TRACE_EVENT_CALIBRATION_STOP,"Calibration");
	 MeasureEventStop(TRACE_EVENT_CALIBRATION_STOP,"Calibration");
	 offset=timer[TRACE_EVENT_CALIBRATION_START].sum/2;
#ifdef OSTLOG
	 logcounter=0;
	 logwrap=0;
#endif
	 ILOS_sxaTimer90KhzStart();
}

#ifdef OSTLOG

void	OstLog(t_uint32	aTraceName,t_uint64 d,t_uint32 start)
{
		logTime[logcounter]	= d;
		logEvent[logcounter++]=(aTraceName<<1)+start	;
		if (logcounter==LOGMAX) 
		{
			logcounter=0;
			logwrap=1;
		}
}
#endif

void MeasureEventStart0(t_uint32 aTraceName,char	* aEventName)
{
	t_uint64	d;
	d=LOS_getSystemTime();
	timer[aTraceName].start=d;
	timer[aTraceName].name= aEventName;
#ifdef OSTLOG
	OstLog(aTraceName,d,0);
#endif
}

void MeasureEventStop(t_uint32 aTraceName,char *aEventName)
{
	t_uint64	d;
		
	d=LOS_getSystemTime();
	
	timer[aTraceName].stop=d;
#ifdef OSTLOG
	OstLog(aTraceName,d,1);
#endif
	if (timer[aTraceName].stop>timer[aTraceName].start)
	{
		d=	timer[aTraceName].stop-timer[aTraceName].start;
	}
	else
	{
		d=	(1ULL<<32)+timer[aTraceName].stop-timer[aTraceName].start;
		
	};

	//Offset corection
	d-=offset;

	if (d>timer[aTraceName].max) 
	{
		//LOS_Log("Event %d %s delta new max %d\n",timer[aTraceName].n,timer[aTraceName].name,d);
		timer[aTraceName].max=d;
		timer[aTraceName].max_idx=	timer[aTraceName].n;

	}
	timer[aTraceName].sum+=d;
	timer[aTraceName].n++;
}



void MeasureEventDisplayStatus(void)
{
	t_uint32	i;

#ifdef OSTLOG
	t_uint32	idx;
	t_uint64	d;
	
	d=logTime[0];
	
	for(i=0;i<logcounter;i++)
	{
		d=logTime[i]-d;
		idx=logEvent[i]>>1;
		LOS_Log("%s %d	%lld  %lld	 %s\n",(logEvent[i] & 1)?"<":">",i,logTime[i],d,timer[idx].name);	
		d=logTime[i];
	}

	if (logwrap)
	{
		d=logTime[logcounter];
		for(i=logcounter;i<LOGMAX;i++)
		{
			d=logTime[i]-d;
			idx=logEvent[i]>>1;
		LOS_Log("%s %d	%lld  %lld	 %s\n",(logEvent[i] & 1)?"<":">",i,logTime[i],d,timer[idx].name);	
			d=logTime[i];
		}
	}


#endif


	for(i=0;i<TIMER_ARRAY_SIZE;i++)
	{
		if (timer[i].name)
		{
			LOS_Log("Event %s occur %d times\n",timer[i].name,timer[i].n);
			LOS_Log("\tmax %f ms at occurence %d\n",timer[i].max/1000.0,timer[i].max_idx);
			LOS_Log("\tcumul %f ms\n",timer[i].sum/1000.0);
			if (timer[i].n) LOS_Log("\taverage %f ms\n",(float)timer[i].sum/timer[i].n/1000.0);
		}
	}



}


void	MeasureDelta(t_uint32 aTraceName,char *aEventName,t_uint32 dd)
{
	t_uint64	d=dd;

	timer[aTraceName].name= aEventName;
#ifdef OSTLOG
	t_uint64	t;

	t=LOS_getSystemTime();

	 // time stamp of first decoded frame
	OstLog(aTraceName,t-dd,0);
	OstLog(aTraceName,t,1);
#endif

	if (d>timer[aTraceName].max) 
	{
		timer[aTraceName].max=d;
		timer[aTraceName].max_idx=	timer[aTraceName].n;

	}
	timer[aTraceName].sum+=d;
	timer[aTraceName].n++;
}


#endif
