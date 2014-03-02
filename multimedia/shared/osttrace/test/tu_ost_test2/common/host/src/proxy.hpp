/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _PROXY_HPP_
#define _PROXY_HPP_


#include <inc/type.h>
#include <cm/inc/cm.hpp>
#include <cm/inc/cm_macros.h>


#include "host/tu_ost_test/api/gentrace.hpp"
#include "host/tu_ost_test/api/traceinit.hpp"
#include "host/tu_ost_test/api/uniop.hpp"

// Define size of the communication fifo
#define FIFO_COMS_SIZE  2 // Don't change, hardcoded in .conf

#ifndef __SYMBIAN32__
#define IMPORT_C
#define EXPORT_C
#endif

#ifndef __SYMBIAN32__
#include <los/api/los_api.h>
#else
#include <e32debug.h>
#define LOS_Log RDebug::Printf
#define LOS_GetCurrentId() 0
#define LOS_Exit() return -1
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

extern "C" {
  // NMF Call Back thread function
  IMPORT_C void NMF_DSP_CallBackThread(void * args);
}

class CProxy;

class CTrace
{
  public:	
    CTrace(t_uint32 handle, t_uint16 trace) {aParentHandle=handle;aTraceEnable=trace;}
    t_uint16 getTraceEnable() {return aTraceEnable;}
    t_uint32 getParentHandle() {return aParentHandle;}

    t_uint32 aParentHandle;
    t_uint16 aTraceEnable;

};


class CProxycb: public tu_ost_test_api_uniopDescriptor, public CTrace
{
  virtual void oper(t_uint32 res);
  t_uint32 getId1() {return (t_uint32) this;}


  public:
  CProxycb(void *ProxypHandle, t_uint32 ParentHandle, t_uint16 trace) : CTrace(ParentHandle, trace){pProxyHandle = (CProxy *)ProxypHandle; };
  CProxy *pProxyHandle;
};

class CProxy: public CTrace
{
  public:


    IMPORT_C CProxy(t_uint32 handle, t_uint16 trace, t_uint32 tm);
    IMPORT_C t_nmf_error Init();
    IMPORT_C t_nmf_error Construct();
    IMPORT_C t_nmf_error UnConstruct();
    IMPORT_C t_nmf_error Execute();
    IMPORT_C t_nmf_error CreateExtraZones();
    IMPORT_C t_nmf_error FreeExtraZone(t_cm_memory_handle);
    IMPORT_C t_nmf_error ConfigureExtraZones(t_uint16);


    t_uint32 getId1() {return (t_uint32) this;}

    CProxycb *cb;

    t_nmf_channel cbChannel;

    t_cm_domain_id coreId0;
    t_cm_domain_id coreId1;
    t_cm_instance_handle tu_ost_test1_handle;
    t_cm_instance_handle tu_ost_test2_handle;
    t_cm_instance_handle osttrace_handle_1;
    t_cm_instance_handle osttrace_handle_2;
    Itu_ost_test_api_gentrace gentrace_tu_ost_test1;
    Itu_ost_test_api_gentrace gentrace_tu_ost_test2;
    Itu_ost_test_api_traceinit traceinit_tu_ost_test1;
    Itu_ost_test_api_traceinit traceinit_tu_ost_test2;

    t_cm_memory_handle mHandle1;
    t_cm_memory_handle mHandle2;
    TraceInfoDspCreate_t *mExtraZonePtr1;
    TraceInfoDspCreate_t *mExtraZonePtr2;
    t_cm_system_address SYSmem1;
    t_cm_system_address SYSmem2;
    TRACE_t TraceZone1;
    TRACE_t TraceZone2;
    t_uint32 dspAddr1;
    t_uint32 dspAddr2;

    volatile t_bool SIA_running;
    volatile t_bool SVA_running;
    t_uint32 trace_mode;


  private:
    t_cm_domain_id mpc2domainId(const char* mpcstring); 

};


#endif /* _PROXY_HPP_ */
