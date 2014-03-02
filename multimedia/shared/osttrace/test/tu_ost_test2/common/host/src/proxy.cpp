/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#include "proxy.hpp"
#define MY_OSTTRACE_LOG_TAG "PROXY"
#define LOG_TAG MY_OSTTRACE_LOG_TAG

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "osttrace_test_tu_ost_test2_common_host_src_proxyTraces.h"
#endif

//#include "ST_OSTTEST_COMMON_top.h"

static t_los_sem_id waitingForMpcFlushMessage_sem;


EXPORT_C CProxy::CProxy(t_uint32 handle, t_uint16 trace, t_uint32 tm) : CTrace(handle, trace)
{
  OstTraceFiltInst0 (TRACE_FLOW,  "CProxy::CProxy");

  cb = (CProxycb*)NULL;
  cbChannel=0;
  SVA_running = FALSE;
  SIA_running = FALSE;
  trace_mode=tm;
  LOS_Log("trace_mode: %d\n", trace_mode);

}

// NMF Call Back thread function
EXPORT_C void NMF_DSP_CallBackThread(void * args) 
{

  CProxy * proxy = (CProxy *) args;
  LOS_Log("NMF-DSP CallBackThread - channel=%d, thread_id=%d\n", (int) proxy->cbChannel, LOS_GetCurrentId());

  while(1) {
    char *buffer;
    void *itfref=0;
#ifdef __DEBUG__
    LOS_Log("NMF-DSP CallBackThread - Waiting for NMF message ...\n");
#endif
    t_nmf_error error = CM_GetMessage(proxy->cbChannel, &itfref, &buffer, TRUE);
    if (error == NMF_FLUSH_MESSAGE ) {
      LOS_Log("NMF-DSP CallBackThread flush message\n");
      LOS_SemaphoreNotify(waitingForMpcFlushMessage_sem);
      return ;
    } else
      NMF_ASSERT(CM_OK == error);
#ifdef __DEBUG__
    LOS_Log("NMF-DSP CallBackThread - Got NMF message\n");
#endif
    if (itfref) {
#ifdef __DEBUG__
      LOS_Log("NMF-DSP CallBackThread - Executing NMF CB message\n");
#endif
      // Execute the call back
      CM_ExecuteMessage(itfref, buffer);
#ifdef __DEBUG__
      // Release component lock
      LOS_Log("NMF-DSP CallBackThread - Executing NMF CB message done\n");
#endif
    } else {
      // NMF service message (panic, ...)
      CM_ExecuteMessage(0, buffer);
    }

  }
}



/***************************************************************************/
/*
 * brief : output_dec_method
 *
 * Output method of the dec_one
 * Display message to CLCD
 */
/***************************************************************************/
void CProxycb::oper(t_uint32 id)
{

  OstTraceFiltInst1 (TRACE_FLOW,  "Receive End of processing on MPC %d", id);

  if (id == 2) {
    LOS_Log("Calling oper !!! from SVA\n");
    pProxyHandle->SVA_running = FALSE;
  }
  else if (id == 3) {
    LOS_Log("Calling oper !!! from SIA\n");
    pProxyHandle->SIA_running = FALSE;
  }
  else
  {
    LOS_Log("Unknown MPC ID %d", id);
    OstTraceInt1 (TRACE_ERROR,  "Unknown MPC ID %d", id);
  }

}



EXPORT_C t_nmf_error CProxy::Init()
{

  t_uint32 nmfversion;

  OstTraceFiltInst0 (TRACE_FLOW,  "CProxy::Init");

  // Print NMF version
  CM_GetVersion(&nmfversion);
  LOS_Log("NMF Version %d-%d-%d\n",
      (void*)VERSION_MAJOR(nmfversion),
      (void*)VERSION_MINOR(nmfversion),
      (void*)VERSION_PATCH(nmfversion));

  if (NMF_VERSION != nmfversion) {
    LOS_Log("Error: Incompatible API version %d != %d\n", (void*)NMF_VERSION, (void*)nmfversion);
    LOS_Exit();
  }
  CM_CreateChannel(NMF_CHANNEL_SHARED, &cbChannel);

  CM_SetMode(CM_CMD_TRACE_ON, 0);

  CM_SetMode(CM_CMD_MPC_TRACE_ON, SVA_CORE_ID);
  CM_SetMode(CM_CMD_MPC_PRINT_WARNING, SVA_CORE_ID);
  CM_SetMode(CM_CMD_MPC_PRINT_ERROR, SVA_CORE_ID);
  CM_SetMode(CM_CMD_MPC_PRINT_INFO, SVA_CORE_ID);
  CM_SetMode(CM_CMD_MPC_PRINT_VERBOSE, SVA_CORE_ID);

  CM_SetMode(CM_CMD_MPC_TRACE_ON, SIA_CORE_ID);
  CM_SetMode(CM_CMD_MPC_PRINT_WARNING, SIA_CORE_ID);
  CM_SetMode(CM_CMD_MPC_PRINT_ERROR, SIA_CORE_ID);
  CM_SetMode(CM_CMD_MPC_PRINT_INFO, SIA_CORE_ID);
  CM_SetMode(CM_CMD_MPC_PRINT_VERBOSE, SIA_CORE_ID);

  return NMF_OK;

}

EXPORT_C t_nmf_error CProxy::Construct()
{

  waitingForMpcFlushMessage_sem = LOS_SemaphoreCreate(0);

  coreId0 = mpc2domainId("SVA");
  coreId1 = mpc2domainId("SIA");

  CM_REGISTER_STUBS_SKELS(tu_ost_test_common_cpp);

  cb = new CProxycb(this, aParentHandle, aTraceEnable);

  /*******************************************************************/
  /* Instantiate Components                                          */
  /*******************************************************************/
  CHECK_NMF_ERROR("tu_ost_test: comp1 component instantiation",
      CM_InstantiateComponent("tu_ost_test.comp1", coreId0, NMF_SCHED_NORMAL, "tu_ost_test.comp1", &tu_ost_test1_handle)
      );

  CHECK_NMF_ERROR("osttrace component instantiation",
      CM_InstantiateComponent("osttrace.mmdsp", coreId0, NMF_SCHED_NORMAL, "osttrace", &osttrace_handle_1)
      );
  /*******************************************************************/
  /* Bind from/to host                                               */
  /*******************************************************************/
  CHECK_NMF_ERROR("Bind gentrace interface",
      CM_BindComponentFromUser(tu_ost_test1_handle, "gentrace", FIFO_COMS_SIZE, &gentrace_tu_ost_test1)
      );

  CHECK_NMF_ERROR("Bind traceinit interface",
      CM_BindComponentFromUser(tu_ost_test1_handle, "traceinit", FIFO_COMS_SIZE, &traceinit_tu_ost_test1)
      );

  CHECK_NMF_ERROR("Bind uniop interface",
      CM_BindComponentToUser(cbChannel, tu_ost_test1_handle, "output", cb, FIFO_COMS_SIZE)
      );


  // Bind Components
  CHECK_NMF_ERROR("Bind tu_ost_test->osttrace component",
      CM_BindComponent(tu_ost_test1_handle, "osttrace", osttrace_handle_1, "osttrace")
      );

  // Start Components
  CHECK_NMF_ERROR("Start 'tu_ost_test'", CM_StartComponent(tu_ost_test1_handle));
  CHECK_NMF_ERROR("Start 'osttrace'", CM_StartComponent(osttrace_handle_1));

  /*******************************************************************/
  /* Instantiate Components                                          */
  /*******************************************************************/
  CHECK_NMF_ERROR("tu_ost_test2 component instantiation",
      CM_InstantiateComponent("tu_ost_test.comp2", coreId1, NMF_SCHED_NORMAL, "tu_ost_test.comp2", &tu_ost_test2_handle)
      );

  CHECK_NMF_ERROR("osttrace component instantiation",
      CM_InstantiateComponent("osttrace.mmdsp", coreId1, NMF_SCHED_NORMAL, "osttrace", &osttrace_handle_2)
      );
  /*******************************************************************/
  /* Bind from/to host                                               */
  /*******************************************************************/
  CHECK_NMF_ERROR("Bind gentrace interface",
      CM_BindComponentFromUser(tu_ost_test2_handle, "gentrace", FIFO_COMS_SIZE, &gentrace_tu_ost_test2)
      );

  CHECK_NMF_ERROR("Bind traceinit interface",
      CM_BindComponentFromUser(tu_ost_test2_handle, "traceinit", FIFO_COMS_SIZE, &traceinit_tu_ost_test2)
      );

  CHECK_NMF_ERROR("Bind uniop interface",
      CM_BindComponentToUser(cbChannel, tu_ost_test2_handle, "output", cb, FIFO_COMS_SIZE)
      );

  // Bind Components
  CHECK_NMF_ERROR("Bind tu_ost_test->osttrace component",
      CM_BindComponent(tu_ost_test2_handle, "osttrace", osttrace_handle_2, "osttrace")
      );

  // Start Components
  CHECK_NMF_ERROR("Start 'tu_ost_test'", CM_StartComponent(tu_ost_test2_handle));
  CHECK_NMF_ERROR("Start 'osttrace'", CM_StartComponent(osttrace_handle_2));


  CreateExtraZones();



  return NMF_OK;

}


EXPORT_C t_nmf_error CProxy::UnConstruct()
{
  t_bool isMsgGenerated;

  OstTraceFiltInst0 (TRACE_FLOW,  "CProxy::UnConstruct");

  CHECK_NMF_ERROR("Stop 'tu_ost_test1'", CM_StopComponent(tu_ost_test1_handle));
  CHECK_NMF_ERROR("Stop 'osttrace'", CM_StopComponent(osttrace_handle_1));
  CHECK_NMF_ERROR("Stop 'tu_ost_test2'", CM_StopComponent(tu_ost_test2_handle));
  CHECK_NMF_ERROR("Stop 'osttrace'", CM_StopComponent(osttrace_handle_2));

  CHECK_NMF_ERROR("Unbind 'gentrace' interface", CM_UnbindComponentFromUser(&gentrace_tu_ost_test1));
  CHECK_NMF_ERROR("Unbind 'traceinit' interface", CM_UnbindComponentFromUser(&traceinit_tu_ost_test1));
  CHECK_NMF_ERROR("Unbind 'uniop' interface", CM_UnbindComponentToUser(cbChannel,tu_ost_test1_handle, "output",0));

  CHECK_NMF_ERROR("Unbind 'gentrace' interface", CM_UnbindComponentFromUser(&gentrace_tu_ost_test2));
  CHECK_NMF_ERROR("Unbind 'traceinit' interface", CM_UnbindComponentFromUser(&traceinit_tu_ost_test2));
  CHECK_NMF_ERROR("Unbind 'uniop' interface", CM_UnbindComponentToUser(cbChannel,tu_ost_test2_handle, "output",0));

  CHECK_NMF_ERROR("Unbind 'osttrace' component", CM_UnbindComponent(tu_ost_test1_handle, "osttrace"));
  CHECK_NMF_ERROR("Unbind 'osttrace' component", CM_UnbindComponent(tu_ost_test2_handle, "osttrace"));

  CHECK_NMF_ERROR("DestroyComponent 'tu_ost_test1' component ", CM_DestroyComponent(tu_ost_test1_handle));
  CHECK_NMF_ERROR("DestroyComponent 'osttrace' component ", CM_DestroyComponent(osttrace_handle_1));
  CHECK_NMF_ERROR("DestroyComponent 'tu_ost_test2' component ", CM_DestroyComponent(tu_ost_test2_handle));
  CHECK_NMF_ERROR("DestroyComponent 'osttrace' component ", CM_DestroyComponent(osttrace_handle_2));

  FreeExtraZone(mHandle1);
  FreeExtraZone(mHandle2);


  delete cb;

  CM_UNREGISTER_STUBS_SKELS(tu_ost_test_common_cpp);
  CHECK_NMF_ERROR("FlushChannel", CM_FlushChannel(cbChannel, &isMsgGenerated));
  if (isMsgGenerated)
    LOS_SemaphoreWait(waitingForMpcFlushMessage_sem);

  CHECK_NMF_ERROR("CM_CloseChannel", CM_CloseChannel(cbChannel));
  LOS_SemaphoreDestroy(waitingForMpcFlushMessage_sem);

  return NMF_OK;

}


t_cm_domain_id CProxy::mpc2domainId(const char* mpcstring) 
{
  t_cm_domain_id domainId = 0;
  t_cm_domain_memory my_domain;

  my_domain.esramCode.offset = 0;
  my_domain.esramCode.size = 0;
  my_domain.esramData.offset = 0;
  my_domain.esramData.size = 0;

  my_domain.sdramData.offset = 0x40000;
  my_domain.sdramData.size = 0xFFFFFF;

  my_domain.sdramCode.offset = 0x40000;
  my_domain.sdramCode.size = 0xFFFFFF;


#ifndef WORKSTATION
  if(mpcstring[0] == 'S' && mpcstring[1] == 'V' && mpcstring[2] == 'A' && mpcstring[3] == 0) 
  {
    my_domain.coreId = SVA_CORE_ID;
    if (CM_CreateMemoryDomain(&my_domain, &domainId) != CM_OK) {
      NMF_LOG("Couldn't Create domain\n");
      NMF_ASSERT(0);
    }
  } 
  else if(mpcstring[0] == 'S' && mpcstring[1] == 'I' && mpcstring[2] == 'A' && mpcstring[3] == 0) 
  {
    my_domain.coreId = SIA_CORE_ID;		
    if (CM_CreateMemoryDomain(&my_domain, &domainId) != CM_OK) {
      NMF_LOG("Couldn't Create domain\n");
      NMF_ASSERT(0);
    }

  }
#endif
  return domainId;
}


EXPORT_C t_nmf_error CProxy::CreateExtraZones() 
{
  t_cm_error error;
  t_uint32 trace_mode_r;

  // Create extra zone for Domain 0
  OstTraceFiltInst1 (TRACE_FLOW,  "CProxy::CreateExtraZone for Domain ID: %d", coreId0);

  // create
  error = CM_AllocMpcMemory(coreId0, CM_MM_MPC_SDRAM16, sizeof(TraceInfoDspCreate_t)/2, CM_MM_ALIGN_2WORDS, &mHandle1);

  if (error != CM_OK) {
    return -1;
  }

  error = CM_GetMpcMemorySystemAddress(mHandle1, &SYSmem1);
  if (error != CM_OK) {
    return -1;
  }
  mExtraZonePtr1 = (TraceInfoDspCreate_t *)SYSmem1.logical;

  error = CM_GetMpcMemoryMpcAddress(mHandle1, &dspAddr1);
  LOS_Log("MpcMemoryMpcAddress: 0x%x\n", dspAddr1);

  if (error != CM_OK) {
    return -1;
  }

  TraceZone1.mTraceInfoPtr = (TraceInfo_t*)dspAddr1;
  TraceZone1.mId1 = 0x9999;

  SVA_running = TRUE;	
  traceinit_tu_ost_test1.traceinit(TraceZone1);
  while (SVA_running);

  // Read trace configuration attribues
  CM_ReadComponentAttribute (osttrace_handle_1, "trace_mode", &trace_mode_r);
  LOS_Log("Read attribue trace_mode for SVA: %d\n", trace_mode_r);

  // Write trace configuration attribues
  CM_WriteComponentAttribute (osttrace_handle_1, "trace_mode", trace_mode);
  LOS_Log("Write attribue trace_mode for SVA: %d\n", trace_mode);

  // Read trace configuration attribues
  CM_ReadComponentAttribute (osttrace_handle_1, "trace_mode", &trace_mode_r);
  LOS_Log("Read attribue trace_mode for SVA: %d\n", trace_mode_r);

  // Create extra zone for Domain 0
  OstTraceFiltInst1 (TRACE_FLOW,  "CProxy::CreateExtraZone for Domain ID: %d", coreId1);

  // create
  error = CM_AllocMpcMemory(coreId1, CM_MM_MPC_SDRAM16, sizeof(TraceInfoDspCreate_t)/2, CM_MM_ALIGN_2WORDS, &mHandle2);

  if (error != CM_OK) {
    return -1;
  }

  error = CM_GetMpcMemorySystemAddress(mHandle2, &SYSmem2);
  if (error != CM_OK) {
    return -1;
  }
  mExtraZonePtr2 = (TraceInfoDspCreate_t *)SYSmem2.logical;

  error = CM_GetMpcMemoryMpcAddress(mHandle2, &dspAddr2);
  LOS_Log("MpcMemoryMpcAddress: 0x%x\n", dspAddr2);

  if (error != CM_OK) {
    return -1;
  }

  TraceZone2.mTraceInfoPtr = (TraceInfo_t*)dspAddr2;
  TraceZone2.mId1 = 0x9999;

  SIA_running = TRUE;	
  traceinit_tu_ost_test2.traceinit(TraceZone2);
  while (SIA_running);

  // Read trace configuration attribues
  CM_ReadComponentAttribute (osttrace_handle_2, "trace_mode", &trace_mode_r);
  LOS_Log("Read attribute trace_mode for SIA: %d\n", trace_mode_r);

  // Write trace configuration attribues
  CM_WriteComponentAttribute (osttrace_handle_2, "trace_mode", trace_mode);
  LOS_Log("Write attribute trace_mode for SIA: %d\n", trace_mode);

  // Read trace configuration attribues
  CM_ReadComponentAttribute (osttrace_handle_2, "trace_mode", &trace_mode_r);
  LOS_Log("Read attribute trace_mode for SIA: %d\n", trace_mode_r);


  ConfigureExtraZones(0xFFFF);

  return NMF_OK;

}


EXPORT_C t_nmf_error CProxy::ConfigureExtraZones(t_uint16 aTraceFilter)
{

  OstTraceFiltInst0 (TRACE_FLOW,  "CProxy::ConfigureExtraZone");

  aTraceEnable = aTraceFilter;
  (cb->aTraceEnable) = aTraceFilter;	

  // initialize
  if (mExtraZonePtr1 != 0) 
  {
    //mExtraZonePtr1->parentHandle = aParentHandle;
    mExtraZonePtr1->parentHandle_L = (aParentHandle & 0xFFFF0000UL) >> 16;
    mExtraZonePtr1->parentHandle_H =  aParentHandle & 0x0000FFFFUL;

    mExtraZonePtr1->traceEnable = aTraceFilter;
  }

  if (mExtraZonePtr2 != 0) 
  {
    //mExtraZonePtr2->parentHandle = aParentHandle;		
    mExtraZonePtr2->parentHandle_L = (aParentHandle & 0xFFFF0000UL) >> 16;
    mExtraZonePtr2->parentHandle_H =  aParentHandle & 0x0000FFFFUL;

    mExtraZonePtr2->traceEnable = aTraceFilter;
  }


  return NMF_OK;

}


EXPORT_C t_nmf_error CProxy::FreeExtraZone(t_cm_memory_handle mHandle) 
{

  OstTraceFiltInst0 (TRACE_FLOW,  "CProxy::FreeExtraZone");

  CHECK_NMF_ERROR("Free MPC MEMORY", CM_FreeMpcMemory(mHandle));

  return NMF_OK;

}



EXPORT_C t_nmf_error CProxy::Execute()
{

  OstTraceFiltInst0 (TRACE_FLOW,  "CProxy::Execute");

  gentrace_tu_ost_test1.gentrace();
  while (SVA_running);

  gentrace_tu_ost_test2.gentrace();
  while (SIA_running);

  return NMF_OK;


}

