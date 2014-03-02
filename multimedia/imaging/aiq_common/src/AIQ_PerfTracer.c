/****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/

#if ( defined(AIQ_PERFTRACE_ENABLE) && !defined(__ARM_THINK) || defined(__SYMBIAN32__) )

#include "AIQ_CommonTypes.h"
#include "AIQ_PerfTracer.h"
#include "AIQ_PerfInternal.h"

#include <stdio.h>
#include <string.h>

#define AIQ_PERFMAXMESSAGELENGTH   80
#define AIQ_PERFMAXMESSAGENUMBER   20

#define AIQ_PERFMAXCOUNTERS         5

#if defined(WIN32) /* WIN32 is defined by VC6 (at least) */
	/* WIN32 IMPLEMENTATION */
	#include <windows.h>
	typedef __int64 AIQ_TimeValue;

#elif defined(__SYMBIAN32__)
	/* SYMBIAN IMPLEMENTATION */
	#include <e32base.h> /* used for basic types ? */
	#include <hal.h>     /* HW abstraction layer: provides access to counters */
	typedef TUint32 AIQ_TimeValue;

#elif defined(linux) 
	/* LINUX IMPLEMENTATION */
	#include <sys/time.h>
	#include <time.h>
	typedef struct timespec AIQ_TimeValue;

#elif defined(ANDROID) 
	/* ANDROID IMPLEMENTATION  to be validated*/
	#include <sys/time.h>
	#include <time.h>
	typedef struct timespec AIQ_TimeValue;

#else //other OS like Think...
	#error This OS is not managed for this file

#endif /* WIN32 */


typedef struct AIQ_PerfMessage {
   char           message[AIQ_PERFMAXMESSAGELENGTH];
   AIQ_TimeValue  deltaTicks;
} AIQ_PerfMessage;

typedef struct  {
   FILE            *LogFile;
   AIQ_PerfMessage  a_messageSplCtrList[AIQ_PERFMAXMESSAGENUMBER];
   AIQ_PerfMessage  a_messageAccCtrList[AIQ_PERFMAXMESSAGENUMBER];
   AIQ_U32          ui32_nbMessages;
   AIQ_TimeValue    ticksPerSecond;
   AIQ_Bool         b_startCounting[2*AIQ_PERFMAXCOUNTERS];
   AIQ_TimeValue    startCountingTick[2*AIQ_PERFMAXCOUNTERS];
} AIQ_PerfTracerContext;


/* Global variable storing parameters of memory manager */
AIQ_PerfTracerContext perfTracerContext = {NULL}; /* partial initialization */

/****************************************************************************
* 
* 
* 
* 
*****************************************************************************/
AIQ_COMMON_API AIQ_Error AIQINT_InitPerfTrace(char *fileName)
{
   AIQ_U8 i;
   /* Open Output log file */
   if ((fileName != NULL) && (perfTracerContext.LogFile == NULL))
   {
      perfTracerContext.LogFile = fopen(fileName,"a");
      if (perfTracerContext.LogFile == NULL)
      {
         return(AIQ_SYSTEM_ERROR);
      }
   }

   /* Initialize counters */
   for(i=0;i<AIQ_PERFMAXCOUNTERS;i++)
   {
      perfTracerContext.b_startCounting[i] = AIQ_FALSE;
   }
   perfTracerContext.ui32_nbMessages = 0;

   fprintf(perfTracerContext.LogFile, "Comment\tellapsed time in ticks & ms. ticks per second: ");

#ifdef WIN32 /* WIN32 is defined by VC6 (at least) */
   /* WIN32 IMPLEMENTATION */
   QueryPerformanceFrequency((LARGE_INTEGER*)(&(perfTracerContext.ticksPerSecond)));
   fprintf(perfTracerContext.LogFile,"%I64d\n",perfTracerContext.ticksPerSecond);
#elif defined(__SYMBIAN32__)
   /* SYMBIAN IMPLEMENTATION */
   {
      TInt frequency;
      HAL::Get(HALData::EFastCounterFrequency, frequency);
      perfTracerContext.ticksPerSecond = (AIQ_TimeValue)frequency;
      fprintf(perfTracerContext.LogFile,"%d\n",perfTracerContext.ticksPerSecond);
   }
#else
   /* LINUX IMPLEMENTATION */
   fprintf(perfTracerContext.LogFile,"%d\n",1000000000); /* clock_gettime returns value in timespec, i.e. nanosecond unit */  
#endif /* WIN32 */
   
   fflush(perfTracerContext.LogFile);

   return AIQ_NO_ERROR;
}

/****************************************************************************
* 
* 
* 
* 
*****************************************************************************/
AIQ_COMMON_API AIQ_Error AIQINT_StartPerfCounter(AIQ_U8 id)
{
   if(perfTracerContext.b_startCounting[id] == AIQ_TRUE)
   {
      return AIQ_BAD_STATE;
   }

   perfTracerContext.b_startCounting[id] = AIQ_TRUE;

#ifdef WIN32 /* WIN32 is defined by VC6 (at least) */
   /* WIN32 IMPLEMENTATION */   
   QueryPerformanceCounter((LARGE_INTEGER*)(&(perfTracerContext.startCountingTick[id])));
#elif defined(__SYMBIAN32__)
/* SYMBIAN IMPLEMENTATION */
   perfTracerContext.startCountingTick[id] = User::FastCounter();
#else
/* LINUX IMPLEMENTATION */
   clock_gettime(CLOCK_REALTIME,&(perfTracerContext.startCountingTick[id]));
#endif /* WIN32 */
   

   return AIQ_NO_ERROR;
}

/****************************************************************************
* 
* 
* 
* 
*****************************************************************************/
AIQ_COMMON_API AIQ_Error AIQINT_StopPerfCounter(AIQ_U8 id, char* description)
{
   AIQ_TimeValue stopCountingTick;
   AIQ_TimeValue deltaCountingTicks;

#ifdef WIN32 /* WIN32 is defined by VC6 (at least) */
   /* WIN32 IMPLEMENTATION */
   QueryPerformanceCounter((LARGE_INTEGER*)(&stopCountingTick));
   deltaCountingTicks = stopCountingTick - perfTracerContext.startCountingTick[id];
   if(deltaCountingTicks < 0)
   {
      /* a rollover occured, we assumed only one! */
      deltaCountingTicks += 0xFFFFFFFFFFFFFFFFi64;
   }

#elif defined(__SYMBIAN32__)
/* SYMBIAN IMPLEMENTATION */
   stopCountingTick = User::FastCounter();
   if(stopCountingTick >= perfTracerContext.startCountingTick[id])
   {
      deltaCountingTicks = stopCountingTick - perfTracerContext.startCountingTick[id];
   }
   else
   {
      /* a rollover occured, we assumed only one! */
      deltaCountingTicks = stopCountingTick - perfTracerContext.startCountingTick[id] + 0xFFFFFFFF;
   }

#else
/* LINUX IMPLEMENTATION */
   clock_gettime(CLOCK_REALTIME,&stopCountingTick);
   if (stopCountingTick.tv_nsec < perfTracerContext.startCountingTick[id].tv_nsec) 
   {
      long nsec = (perfTracerContext.startCountingTick[id].tv_nsec - stopCountingTick.tv_nsec) / 1000000000 + 1;
      perfTracerContext.startCountingTick[id].tv_nsec -= 1000000000 * nsec;
      perfTracerContext.startCountingTick[id].tv_sec += nsec;
   }
   if (stopCountingTick.tv_nsec - perfTracerContext.startCountingTick[id].tv_nsec > 1000000000) 
   {
      int nsec = (stopCountingTick.tv_nsec - perfTracerContext.startCountingTick[id].tv_nsec) / 1000000000;
      perfTracerContext.startCountingTick[id].tv_nsec += 1000000000 * nsec;
      perfTracerContext.startCountingTick[id].tv_sec -= nsec;
   }
   
   deltaCountingTicks.tv_sec  = stopCountingTick.tv_sec  - perfTracerContext.startCountingTick[id].tv_sec;
   deltaCountingTicks.tv_nsec = stopCountingTick.tv_nsec - perfTracerContext.startCountingTick[id].tv_nsec;
#endif /* WIN32 */
   
   if(perfTracerContext.b_startCounting[id] == AIQ_FALSE)
   {
      return AIQ_BAD_STATE;
   }

   strncpy(perfTracerContext.a_messageSplCtrList[perfTracerContext.ui32_nbMessages].message,
           description, AIQ_PERFMAXMESSAGELENGTH-1);
   perfTracerContext.a_messageSplCtrList[perfTracerContext.ui32_nbMessages].deltaTicks = deltaCountingTicks;
   perfTracerContext.ui32_nbMessages++;
   perfTracerContext.b_startCounting[id] = AIQ_FALSE;

   if(perfTracerContext.ui32_nbMessages >= AIQ_PERFMAXMESSAGENUMBER)
   {
      AIQINT_FlushPerfTrace();
   }
   return AIQ_NO_ERROR;
}


/****************************************************************************
* 
* 
* 
* 
*****************************************************************************/
AIQ_COMMON_API void      AIQINT_FlushPerfTrace        (void)
{
   AIQ_U8       i;

   for(i=0;i<perfTracerContext.ui32_nbMessages;i++)
   {
#ifdef WIN32
      /* WIN32 IMPLEMENTATION */
      fprintf(perfTracerContext.LogFile,"%s\t%I64d\t%lf\n",
         perfTracerContext.a_messageSplCtrList[i].message,
         perfTracerContext.a_messageSplCtrList[i].deltaTicks,
         perfTracerContext.a_messageSplCtrList[i].deltaTicks*1000/(double)perfTracerContext.ticksPerSecond);
#elif defined(__SYMBIAN32__)
/* SYMBIAN IMPLEMENTATION */
      double fdelta; /* ellapsed time in ms */
      fdelta = perfTracerContext.a_messageSplCtrList[i].deltaTicks*1000/(double)perfTracerContext.ticksPerSecond;

      fprintf(perfTracerContext.LogFile,"%s\t%ld\t%lf\n",
         perfTracerContext.a_messageSplCtrList[i].message,
         perfTracerContext.a_messageSplCtrList[i].deltaTicks,
         fdelta);

#else
/* LINUX IMPLEMENTATION */
      double fdelta; /* ellapsed time in ms */
      fdelta =   perfTracerContext.a_messageSplCtrList[i].deltaTicks.tv_sec  * 1000
               + perfTracerContext.a_messageSplCtrList[i].deltaTicks.tv_nsec / 1000000.0; 

      fprintf(perfTracerContext.LogFile,"%s\t%ld%ld\t%lf\n",
         perfTracerContext.a_messageSplCtrList[i].message,
         perfTracerContext.a_messageSplCtrList[i].deltaTicks.tv_sec,
         perfTracerContext.a_messageSplCtrList[i].deltaTicks.tv_nsec,
         fdelta);
#endif /* WIN32 */

   }
   fflush(perfTracerContext.LogFile);
   perfTracerContext.ui32_nbMessages = 0;

}
/****************************************************************************
* 
* 
* 
* 
*****************************************************************************/
AIQ_COMMON_API void      AIQINT_DeInitPerfTrace       (void)
{
   AIQINT_FlushPerfTrace();
   fclose(perfTracerContext.LogFile);
   perfTracerContext.LogFile = NULL;
}

#endif // #if ( defined(AIQ_PERFTRACE_ENABLE) && !defined(__ARM_THINK) )
