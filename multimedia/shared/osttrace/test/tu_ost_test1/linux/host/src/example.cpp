/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include <stdio.h>

#include <los/api/los_api.h>
#include "proxy.hpp"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "osttrace_test_tu_ost_test1_linux_host_src_exampleTraces.h"
#endif

#define MY_BIG_SIZE_8 512

#define MY_BIG_SIZE_32 (MY_BIG_SIZE_8 / sizeof(unsigned int))


//static unsigned int aLocalTraceEnable = 3;
//static unsigned int *aTraceEnable = &aLocalTraceEnable;

void ExecuteThread(void *args)
{
  CProxy * proxy = (CProxy *) args;

  LOS_Log("Create Thread for proxy instance: 0x%08x \n", (void*) proxy );
  proxy->Execute();

  LOS_ThreadExit();
}



/***************************************************************************/
/*
 * brief : main
 *
 * This example execute an addition on two values and a remove one on the result
 */
/***************************************************************************/
int main(int argc, const char* argv[])
{
  t_uint8 big_arr_int8[MY_BIG_SIZE_8];
  t_uint32 big_arr_int32[MY_BIG_SIZE_32];
  unsigned int i;
  double t_float1=3.14345454545;
  double t_float2=3456.456754345;
  double t_float3=567895.4567435;
  double t_float4=0.456784545;



  for ( i=0; i<MY_BIG_SIZE_8; i++) {
    big_arr_int8[i] = i;
  }
  for ( i=0; i<MY_BIG_SIZE_32; i++) {
    big_arr_int32[i] = MY_BIG_SIZE_8+i;
  }

  OSTTraceInit();

  OstTraceFloat1 (TRACE_FLOW,  "Test float using OstTraceFloat1: %f", t_float1);
  OstTraceFloat2 (TRACE_FLOW,  "Test float using OstTraceFloat2: %f %f", t_float1, t_float2);
  OstTraceFloat3 (TRACE_FLOW,  "Test float using OstTraceFloat3: %f %f %f", t_float1, t_float2, t_float3);
  OstTraceFloat4 (TRACE_FLOW,  "Test float using OstTraceFloat4: %f %f %f %f", t_float1, t_float2, t_float3, t_float4);

  OstTraceArray( TRACE_DEBUG,  "Test OstTraceArray 32-bit integers with size 128: %{int32[]}", (unsigned char*)&big_arr_int32[0], 128);
  OstTraceArray( TRACE_DEBUG,  "Test OstTraceArray 8-bit integers with size 125: %{int8[]}", (unsigned char*)&big_arr_int8[0], 125);
  OstTraceArray( TRACE_DEBUG,  "Test OstTraceArray 8-bit integers with size 401: %{int8[]}", (unsigned char*)&big_arr_int8[0], 401); 
  OstTraceArray( TRACE_DEBUG,  "Test OstTraceArray 8-bit integers with size 501: %{int8[]}", (unsigned char*)&big_arr_int8[0], 501); 


  CProxy MyProxy1(1, 0xFFFF);
  CProxy MyProxy2(2, 0xFFFF);


  OST_Printf("Starting TEST_OST_2 !!! at time: %ld\n", LOS_getSystemTime);
  LOS_Log("Starting TEST_OST_2 !!!\n");

  OstTraceInt0 (TRACE_FLOW,  "Starting OST Example Test");

#if defined (WORKSTATION)
  MyProxy1.Construct();
  OstTraceInt0 (TRACE_FLOW,  "Enable Trace for Proxy1");
  MyProxy1.Configure(0xFFFF);
  MyProxy1.running = TRUE;
  LOS_ThreadCreate(ExecuteThread, (void*)&MyProxy1, 4096, LOS_USER_NORMAL_PRIORITY, "Proxy1");
  while (MyProxy1.running);	
  MyProxy1.UnConstruct();	

  MyProxy2.Construct();
  OstTraceInt0 (TRACE_FLOW,  "Disable Trace for Proxy2");
  MyProxy2.Configure(0x0000);
  MyProxy2.running = TRUE;
  LOS_ThreadCreate(ExecuteThread, (void*)&MyProxy2, 4096, LOS_USER_NORMAL_PRIORITY, "Proxy2");
  while (MyProxy2.running);	
  MyProxy2.UnConstruct();

#else
  MyProxy1.Construct();
  OstTraceInt0 (TRACE_FLOW,  "Enable Trace for Proxy1");
  MyProxy1.Configure(0xFFFF);
  MyProxy1.running = TRUE;
  LOS_ThreadCreate(ExecuteThread, (void*)&MyProxy1, 4096, LOS_USER_NORMAL_PRIORITY, "Proxy1");
  while (MyProxy1.running);
  MyProxy1.UnConstruct();

  MyProxy2.Construct();
  OstTraceInt0 (TRACE_FLOW,  "Disable Trace for Proxy2");
  MyProxy2.Configure(0x0000);
  MyProxy2.running = TRUE;
  LOS_ThreadCreate(ExecuteThread, (void*)&MyProxy2, 4096, LOS_USER_NORMAL_PRIORITY, "Proxy2");
  while (MyProxy2.running);
  MyProxy2.UnConstruct();
#endif
  OstTraceInt0 (TRACE_FLOW,  "Terminating OST Example Test");

  OstTraceInt0 (TRACE_WARNING,  "Test Warning traces");
  OstTraceInt0 (MY_TRACE_GROUP_1,  "This is a trace from group MY_TRACE_GROUP_1");

  OSTTraceDeInit();

  LOS_Log("Terminating TEST_OST_2 !!!\n");

  return 0;
}


#ifdef DISABLE_LOG

void $Sub$$LOS_Log(const char* fmt, ...) 
{
  OST_Printf(fmt);
}
#endif
