/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef __SYMBIAN32__
#include <los/api/los_api.h>
#ifdef WORKSTATION
#define CHECK_NMF_ERROR(myMsg,myExpr)                                   \
{                                                                     \
  t_nmf_error myError;                                                \
  if (                                                                \
      (myError = (myExpr)) != CM_OK                                   \
     ) \
  {                                                                 \
    LOS_Log("Error: %s (error = %d)\n", (void*)(myMsg), myError); \
    LOS_Exit();                                                   \
  }                                                                 \
}
#endif
#else
#include <e32debug.h>
#define CHECK_NMF_ERROR(myMsg,myExpr)                                   \
{                                                                     \
  t_nmf_error myError;                                                \
  if (                                                                \
      (myError = (myExpr)) != CM_OK                                   \
     ) \
  {                                                                 \
    RDebug::Printf("Error: %s (error = %d)\n", (void*)(myMsg), myError); \
    return myError;                                            \
  }                                                                 \
}
#endif


#include "proxy.hpp"

#include "common.h"

#define MY_OSTTRACE_LOG_TAG "PROXY"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "osttrace_test_tu_ost_test1_common_host_src_proxyTraces.h"
#endif

#include "ST_TU_OST_COMMON_TEST1_top.h"
#define MY_SIZE	12
#define MY_BIG_SIZE_8 512

#define MY_BIG_SIZE_32 (MY_BIG_SIZE_8 / sizeof(unsigned int))


// Constants
const t_uint32 GAP_WITH_A_VALUE = 15;


EXPORT_C CProxy::CProxy(t_uint32 handle, t_uint16 trace) : CTrace(handle, trace)
{
  t_uint32 test1 = 0x5;
  t_uint32 test2 = 0x0700;
  t_uint32 test3 = 0x09000000;
  t_uint32 test4 = 0xBBBBBBBB;

  double t_float1 = 0.123456789;
  double t_float2 = 34567.123456789;
  double t_float3 = 2.0045;
  double t_float4 = 5.0;
  unsigned int i;
  t_uint32 arr_int32[MY_SIZE];
  t_uint16 arr_int16[MY_SIZE];
  t_uint8 arr_int8[MY_SIZE];
  t_uint32 j= 0;
  t_uint8 big_arr_int8[MY_BIG_SIZE_8];
  t_uint32 big_arr_int32[MY_BIG_SIZE_32];

  for ( i=0; i<MY_BIG_SIZE_8; i++) {
    big_arr_int8[i] = i;
  }
  for ( i=0; i<MY_BIG_SIZE_32; i++) {
    big_arr_int32[i] = MY_BIG_SIZE_8+i;
  }

  // Make coverity happy
  for (i=0;i<LOOP_MAX;i++)
  {
    a_table[i]=0;
    b_table[i]=0;
  }
  reac_number = 0;
  cbChannel=0;

#ifndef WORKSTATION
  ex = (tu_ost_test_network*)NULL;
#endif
  ex2 = (tu_ost_test_network2*)NULL;
  cb = (CProxycb*)NULL;


  running = FALSE;

  OstTraceInt1 (TRACE_DEBUG,  "Top Dico ID: 0x%x", KOstST_TU_OST_COMMON_TEST1_top_ID);
  OstTraceInt0 (TRACE_DEBUG,  "Constructor");
  OstTraceInt1 (TRACE_DEBUG,  "Test OsttraceInt1: %x", test1);
  OstTraceInt2 (TRACE_DEBUG,  "Test OsttraceInt2: %x %x", test1, test2);
  OstTraceInt3 (TRACE_DEBUG,  "Test OsttraceInt3: %x %x %x", test1, test2, test3);
  OstTraceInt4 (TRACE_DEBUG,  "Test OsttraceInt4: %x %x %x %x", test1, test2, test3, test4);

  OstTraceFiltInst1 (TRACE_DEBUG,  "Test OstTraceFiltInst1: %x", test1);
  OstTraceFiltInst2 (TRACE_DEBUG,  "Test OstTraceFiltInst2: %x %x", test1, test2);
  OstTraceFiltInst3 (TRACE_DEBUG,  "Test OstTraceFiltInst3: %x %x %x", test1, test2, test3);
  OstTraceFiltInst4 (TRACE_DEBUG,  "Test OstTraceFiltInst4: %x %x %x %x", test1, test2, test3, test4);

  OstTraceFiltStatic1 (TRACE_DEBUG,  "Test OstTraceStaticInst1: %x", this, test1);
  OstTraceFiltStatic2 (TRACE_DEBUG,  "Test OstTraceStaticInst2: %x %x", this, test1, test2);
  OstTraceFiltStatic3 (TRACE_DEBUG,  "Test OstTraceStaticInst3: %x %x %x", this, test1, test2, test3);
  OstTraceFiltStatic4 (TRACE_DEBUG,  "Test OstTraceStaticInst4: %x %x %x %x", this, test1, test2, test3, test4);

  OstTraceFiltInstFloat1 (TRACE_DEBUG,  "Test OstTraceFiltInstFloat1: %g", t_float1);
  OstTraceFiltInstFloat2 (TRACE_DEBUG,  "Test OstTraceFiltInstFloat2: %g %g", t_float1, t_float2);
  OstTraceFiltInstFloat3 (TRACE_DEBUG,  "Test OstTraceFiltInstFloat3: %g %g %g", t_float1, t_float2, t_float3);
  OstTraceFiltInstFloat4 (TRACE_DEBUG,  "Test OstTraceFiltInstFloat4: %g %g %g %g", t_float1, t_float2, t_float3, t_float4);

  OstTraceFiltStaticFloat1 (TRACE_DEBUG,  "Test OstTraceFiltStaticFloat1: %g", this, t_float1);
  OstTraceFiltStaticFloat2 (TRACE_DEBUG,  "Test OstTraceFiltStaticFloat2: %g %g", this, t_float1, t_float2);
  OstTraceFiltStaticFloat3 (TRACE_DEBUG,  "Test OstTraceFiltStaticFloat3: %g %g %g", this, t_float1, t_float2, t_float3);
  OstTraceFiltStaticFloat4 (TRACE_DEBUG,  "Test OstTraceFiltStaticFloat4: %g %g %g %g", this, t_float1, t_float2, t_float3, t_float4);



  for ( i=0, j=0x00FFFFFF; i<MY_SIZE; i++,j++ ) { 
    arr_int32[i] = j; 
  }
  for ( i=0,j=0x00FF; i<MY_SIZE; i++,j++ ) { 
    arr_int16[i] = j;
  }
  for ( i=0; i<MY_SIZE; i++ ) { 
    arr_int8[i] = i; 
  }

  OstTraceArray( TRACE_DEBUG,  "Test OstTraceArray with 32-bit integers: %{int32[]}", (unsigned char*)&arr_int32[0], sizeof(t_uint32)*MY_SIZE );
  OstTraceArray( TRACE_DEBUG,  "Test OstTraceArray with 16-bit integers: %{int16[]}", (unsigned char*)&arr_int16[0], sizeof(t_uint16)*MY_SIZE);
  OstTraceArray( TRACE_DEBUG,  "Test OstTraceArray with  8-bit integers: %{int8[]}",  (unsigned char*)&arr_int8[0],  sizeof(t_uint8)*MY_SIZE);

  OstTraceFiltInstData( TRACE_DEBUG,  "Test OstTraceFiltInstData with 32-bit integers: %{int32[]}", (unsigned char*)&arr_int32[0], sizeof(t_uint32)*MY_SIZE );
  OstTraceFiltInstData( TRACE_DEBUG,  "Test OstTraceFiltInstData with 16-bit integers: %{int16[]}", (unsigned char*)&arr_int16[0], sizeof(t_uint16)*MY_SIZE);
  OstTraceFiltInstData( TRACE_DEBUG,  "Test OstTraceFiltInstData with  8-bit integers: %{int8[]}", (unsigned char*)&arr_int8[0],  sizeof(t_uint8)*MY_SIZE);

  OstTraceFiltStaticData( TRACE_DEBUG,  "Test OstTraceFiltStaticData with 32-bit integers: %{int32[]}", this, (unsigned char*)&arr_int32[0], sizeof(t_uint32)*MY_SIZE );
  OstTraceFiltStaticData( TRACE_DEBUG,  "Test OstTraceFiltStaticData with 16-bit integers: %{int16[]}", this, (unsigned char*)&arr_int16[0], sizeof(t_uint16)*MY_SIZE);
  OstTraceFiltStaticData( TRACE_DEBUG,  "Test OstTraceFiltStaticData with  8-bit integers: %{int8[]}",  this, (unsigned char*)&arr_int8[0],  sizeof(t_uint8)*MY_SIZE);

  OstTraceFiltStaticData( TRACE_DEBUG,  "Test OstTraceFiltStaticData 32-bit integers with size 128: %{int32[]}", this, (unsigned char*)&big_arr_int32[0], 128);
  OstTraceFiltStaticData( TRACE_DEBUG,  "Test OstTraceFiltStaticData 8-bit integers with size 501: %{int8[]}", this, (unsigned char*)&big_arr_int8[0], 501);

}




/***************************************************************************/
/*
 * brief : output_dec_method
 *
 * Output method of the dec_one
 * Display message to CLCD
 */
/***************************************************************************/
void CProxycb::oper(t_uint32 res)
{
  if (pProxyHandle->reac_number>=LOOP_MAX)
  {
#ifdef __SYMBIAN32__
    RDebug::Printf("Error !!! too much reaction\n");
#else
    LOS_Log("Error !!! too much reaction\n");
#endif
    OstTraceFiltInst0 (TRACE_ERROR,  "Error !!! too much reaction");
  }
  else
  {
    t_uint32 data[4];
    t_uint32 *ptr = data;

#ifdef __SYMBIAN32__
    *(ptr)++ = 0x00FF00FF;
#else
    *(ptr)++ = LOS_GetCurrentId();
#endif

#ifndef WORKSTATION
    *(ptr)++ = pProxyHandle->a_table[pProxyHandle->reac_number];
    *(ptr)++ = pProxyHandle->b_table[pProxyHandle->reac_number];
    *(ptr)++ = res;

    OstTraceFiltInst4 (TRACE_DEBUG,  "%x: Hello World (A: 0x%08x, B: 0x%08x, Res: 0x%08x)", data[0], data[1], data[2], data[3]);
#else
    *(ptr)++ = pProxyHandle->b_table[pProxyHandle->reac_number];
    *(ptr)++ = res;

    OstTraceFiltInst3 (TRACE_DEBUG,  "%x: Hello World (A: 0x%08x, Res: 0x%08x)", data[0], data[1], data[2]);
#endif

#ifdef __SYMBIAN32__
    RDebug::Printf("Hello World (A: 0x%08x, B: 0x%08x, Res: 0x%08x)\n",
        (void*)pProxyHandle->a_table[pProxyHandle->reac_number],
        (void*)pProxyHandle->b_table[pProxyHandle->reac_number],(void*)res);
#else
#ifndef WORKSTATION

    LOS_Log("%x: Hello World (A: 0x%08x, B: 0x%08x, Res: 0x%08x)\n", LOS_GetCurrentId(),
        (void*)pProxyHandle->a_table[pProxyHandle->reac_number],
        (void*)pProxyHandle->b_table[pProxyHandle->reac_number],(void*)res);
#else
    LOS_Log("%x: Hello World (A: 0x%08x, Res: 0x%08x)\n", LOS_GetCurrentId(),
        (void*)pProxyHandle->b_table[pProxyHandle->reac_number],(void*)res);
#endif
#endif
    pProxyHandle->reac_number++;
  }
}



void CProxy::PullMessage(t_nmf_channel Channel)
{
  char *msg;
  void *clientContext;
  /* do a blocking call */
  if (EE_GetMessage(Channel, &clientContext, &msg, 1) == NMF_OK) {
    EE_ExecuteMessage(clientContext, msg);
  }
}

EXPORT_C t_nmf_error CProxy::Construct ()
{
  // Use interfaces group
  OstTraceFiltInst0 (TRACE_FLOW,  "Register Stub and Skeleton");

#ifndef WORKSTATION
  CM_REGISTER_STUBS_SKELS(tu_ost_test_common_cpp);
#endif

  OstTraceFiltInst0 (TRACE_FLOW,  "Create EE Channel");
  CHECK_NMF_ERROR("Init channel", EE_CreateChannel(NMF_CHANNEL_SHARED, &cbChannel));

#ifndef WORKSTATION
  CM_SetMode(CM_CMD_TRACE_ON, 0);
  CM_SetMode(CM_CMD_MPC_TRACE_ON, SVA_CORE_ID);
  CM_SetMode(CM_CMD_MPC_PRINT_WARNING, SVA_CORE_ID);
  CM_SetMode(CM_CMD_MPC_PRINT_ERROR, SVA_CORE_ID);
  CM_SetMode(CM_CMD_MPC_PRINT_INFO, SVA_CORE_ID);
  CM_SetMode(CM_CMD_MPC_PRINT_VERBOSE, SVA_CORE_ID);
#endif
  OstTraceFiltInst0 (TRACE_FLOW,  "Create Network");

#ifndef WORKSTATION
  ex = tu_ost_test_networkCreate();

  ex->coreId0 = mpc2domainId("SVA");
  ex->coreId1 = mpc2domainId("SIA");
#endif
  ex2 = tu_ost_test_network2Create();

  OstTraceFiltInst0 (TRACE_FLOW,  "Construct Network");

  /*******************************************************************/
  /* Instantiate Networks                                            */
  /*******************************************************************/
#ifndef WORKSTATION
  CHECK_NMF_ERROR("Construct 'network'", ex->construct());
#endif
  CHECK_NMF_ERROR("Construct 'network2'", ex2->construct());

  OstTraceFiltInst0 (TRACE_FLOW,  "Bind Network");

#ifndef WORKSTATION
  CHECK_NMF_ERROR("Bind user -> 'network'", ex->bindFromUser("input", 2, &input));
  CHECK_NMF_ERROR("Bind user -> 'network'", ex->bindFromUser("configure_arm[0]", 2, &configure_arm[0]));
  CHECK_NMF_ERROR("Bind user -> 'network'", ex->bindFromUser("configure_arm[1]", 2, &configure_arm[1]));
  CHECK_NMF_ERROR("Bind user -> 'network'", ex->bindFromUser("configure_dsp", 2, &configure_dsp));
  CHECK_NMF_ERROR("Bind 'network' -> 'network2'", ex->bindAsynchronous("output", 2, ex2, "input"));
#else
  CHECK_NMF_ERROR("Bind user -> 'network2'", ex2->bindFromUser("input", 2, &input));
#endif

  CHECK_NMF_ERROR("Bind user -> 'network2'", ex2->bindFromUser("configure", 2, &configure_arm[2]));
  cb = new CProxycb(this, aParentHandle, aTraceEnable);
  CHECK_NMF_ERROR("Bind 'network2' -> user", ex2->bindToUser(cbChannel, "output", cb, 2));

  /*******************************************************************/
  /* Start Networks                                                */
  /*******************************************************************/
  OstTraceFiltInst0 (TRACE_FLOW,  "start Network");

#ifndef WORKSTATION
  ex->start();
#endif
  ex2->start();

  return NMF_OK;
}

EXPORT_C t_nmf_error CProxy::UnConstruct ()
{
  t_nmf_error nmfError;

  /*******************************************************************/
  /* Stop Networks                                                   */
  /*******************************************************************/
  OstTraceFiltInst0 (TRACE_FLOW,  "stop Network");
  ex2->stop();

#ifndef WORKSTATION
  ex->stop();
#endif

  delete cb;
  /*******************************************************************/
  /* Unbind Networks                                                 */
  /*******************************************************************/
  OstTraceFiltInst0 (TRACE_FLOW,  "UnBind Network");

  CHECK_NMF_ERROR("UnBind 'network2' -> user", ex2->unbindToUser(cbChannel, "output", 0x0));
#ifndef WORKSTATION
  CHECK_NMF_ERROR("UnBind 'network' -> 'network2'", ex->unbindAsynchronous("output", ex2, "input"));
  CHECK_NMF_ERROR("UnBind user -> 'network'", ex->unbindFromUser("input"));
  CHECK_NMF_ERROR("UnBind user -> 'network'", ex->unbindFromUser("configure_arm[0]"));
  CHECK_NMF_ERROR("UnBind user -> 'network'", ex->unbindFromUser("configure_arm[1]"));
  CHECK_NMF_ERROR("UnBind user -> 'network'", ex->unbindFromUser("configure_dsp"));
#else
  CHECK_NMF_ERROR("UnBind user -> 'network2'", ex2->unbindFromUser("input"));
#endif
  CHECK_NMF_ERROR("UnBind user -> 'network2'", ex2->unbindFromUser("configure"));

  /*******************************************************************/
  /* Destroy Networks                                                */
  /*******************************************************************/
  OstTraceFiltInst0 (TRACE_FLOW,  "Destroy Network");
  CHECK_NMF_ERROR("Destroy 'network2'", ex2->destroy());
#ifndef WORKSTATION
  CHECK_NMF_ERROR("Destroy 'network'", ex->destroy());
#endif
  tu_ost_test_network2Destroy(ex2);
#ifndef WORKSTATION
  tu_ost_test_networkDestroy(ex);
#endif

  OstTraceFiltInst0 (TRACE_FLOW,  "Close EE Channel");
  nmfError = EE_CloseChannel(cbChannel);
  if (nmfError != NMF_OK) 
  {
#ifdef __SYMBIAN32__
    NMF_LOG("Unable to Close channel\n");
#else
    NMF_PANIC("Unable to Close channel\n");
#endif
  }
#ifndef WORKSTATION
  CM_UNREGISTER_STUBS_SKELS(tu_ost_test_common_cpp);
#endif
  return NMF_OK;
}

EXPORT_C t_nmf_error CProxy::Execute()
{
  // Counter for the input loop
  int loop_counter = 0;

  reac_number = 0;

  OstTraceFiltInst0 (TRACE_FLOW,  "Executing....");

  for (loop_counter = 0; loop_counter < LOOP_MAX; loop_counter++)
  {
    t_uint16 a = loop_counter;
    t_uint16 b = loop_counter + GAP_WITH_A_VALUE;

    // Infinite loop until reaction is not done
    while ((loop_counter - reac_number) >= FIFO_COMS_SIZE)
      PullMessage(cbChannel);

    // Save datas
    a_table[loop_counter] = a;
    b_table[loop_counter] = b;

#ifndef WORKSTATION
    // Give datas to adder
    input.add(a,b);
#else
    input.oper(b);
#endif
  }


  /*******************************************************************/
  /* Waiting for the end the test: All callbacks received            */
  /*******************************************************************/
  while (reac_number < LOOP_MAX) 
  {
    PullMessage(cbChannel);
  }
  running = FALSE;
  OstTraceFiltInst0 (TRACE_FLOW,  "Execution Over !!");

  return NMF_OK;


}


EXPORT_C t_nmf_error CProxy::Configure(t_uint16 aTraceFilter)
{
  int i;
#ifndef WORKSTATION
  int  init=0;
#else
  int  init=2;
#endif
  OstTraceFiltInst0 (TRACE_FLOW,  "Configure Trace filter and Parent Handle !!");

  aTraceEnable = aTraceFilter;
  (cb->aTraceEnable) = aTraceFilter;

  for (i=init;i<CONFIGURE_ARM_ITF;i++)
  {
    configure_arm[i].ConfigureTrace(aTraceEnable);
    configure_arm[i].ConfigureParentHandle((t_uint32)this);
  }
#ifndef WORKSTATION
  configure_dsp.ConfigureTrace(aTraceEnable);
  configure_dsp.ConfigureParentHandle((t_uint32)this);
#endif
  return NMF_OK;

}

