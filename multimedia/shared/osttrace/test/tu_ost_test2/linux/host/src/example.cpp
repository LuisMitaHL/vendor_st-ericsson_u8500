/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <los/api/los_api.h>
#include "proxy.hpp"
#include <stdlib.h>

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "osttrace_test_tu_ost_test2_linux_host_src_exampleTraces.h"
#endif




/***************************************************************************/
/*
 * brief : main
 *
 * This example execute an addition on two values and a remove one on the result
 */
/***************************************************************************/
int main(int argc, const char* argv[])
{
  int trace_mode = 0;

  if (argc > 1) {
    trace_mode = atoi(argv[1]);
    LOS_Log("trace_mode: %d\n", trace_mode);
  }

  OSTTraceInit();

  CProxy MyProxy(0x44332211, 0xFFFF, trace_mode);

  LOS_Log("Starting OST Example Test !!!\n");

  OstTraceInt0 (TRACE_FLOW,  "Starting OST Example Test");
  OstTraceInt0 (TRACE_ERROR,  "Testing trace group error");
  OstTraceInt0 (TRACE_WARNING,  "Testing trace group warning");

  CHECK_NMF_ERROR("Init", MyProxy.Init());

  LOS_ThreadCreate(NMF_DSP_CallBackThread, &MyProxy,
      4096, LOS_USER_URGENT_PRIORITY,
      "NMF_MPC_2_HOST_CB_Thread");


  CHECK_NMF_ERROR("Construct", MyProxy.Construct());

  OstTraceInt0 (TRACE_FLOW,  "Enable Trace for Proxy");
  CHECK_NMF_ERROR("Enable Trace", MyProxy.ConfigureExtraZones(0xFFFF));
  MyProxy.SVA_running = TRUE;
  MyProxy.SIA_running = TRUE;
  CHECK_NMF_ERROR("Execute Test", MyProxy.Execute());

  OstTraceInt0 (TRACE_FLOW,  "Disable Trace for Proxy");
  CHECK_NMF_ERROR("Disable Trace", MyProxy.ConfigureExtraZones(0x0000));
  MyProxy.SVA_running = TRUE;
  MyProxy.SIA_running = TRUE;
  CHECK_NMF_ERROR("Execute Test", MyProxy.Execute());

  LOS_Sleep(1000);
  CHECK_NMF_ERROR("Unconstruct", MyProxy.UnConstruct());

  OstTraceInt0 (TRACE_FLOW,  "Terminating OST Example Test");

  OSTTraceDeInit();

  LOS_Log("Terminating OST Example Test !!!\n");

  return 0;
}


#ifdef DISABLE_LOG

void $Sub$$LOS_Log(const char* fmt, ...) 
{
  OST_Printf(fmt);
}
#endif
