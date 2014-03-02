/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*****************************************************************************/
/**
 * This is a Nomadik Multiprocessing Framework generated file, please do not modify it.
 */

#include <cm/inc/cm.h>
#include <cm/proxy/api/private/stub-requiredapi.h>

/* Generated C++ stub for 'shared_emptythisbuffer'
   Generator:tools.generator.legacy.cpp.user.stub */
#include <host/shared_emptythisbuffer.hpp>

class shared_emptythisbuffer: public shared_emptythisbufferDescriptor
{
  public:
    t_cm_bf_host2mpc_handle    host2mpcId;

  public:
    virtual void emptyThisBuffer(t_uint32 buffer) {
      t_uint16 _xyuv_data[2];
      /* buffer <t_uint32> marshalling */
      _xyuv_data[0] = (t_uint16)buffer;
      _xyuv_data[0+1] = (t_uint16)(buffer >> 16);
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 2*2, 0);
    }
};
static t_cm_bf_host2mpc_handle* INITstub_shared_emptythisbuffer(t_nmf_interface_desc* itf) {
  shared_emptythisbuffer *stub = new shared_emptythisbuffer();
  if(stub == 0x0)
    return 0x0;
  *(NMF::InterfaceReference*)itf = stub;
  return &stub->host2mpcId;
}

/* Generated C++ stub for 'shared_fillthisbuffer'
   Generator:tools.generator.legacy.cpp.user.stub */
#include <host/shared_fillthisbuffer.hpp>

class shared_fillthisbuffer: public shared_fillthisbufferDescriptor
{
  public:
    t_cm_bf_host2mpc_handle    host2mpcId;

  public:
    virtual void fillThisBuffer(t_uint32 buffer) {
      t_uint16 _xyuv_data[2];
      /* buffer <t_uint32> marshalling */
      _xyuv_data[0] = (t_uint16)buffer;
      _xyuv_data[0+1] = (t_uint16)(buffer >> 16);
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 2*2, 0);
    }
};
static t_cm_bf_host2mpc_handle* INITstub_shared_fillthisbuffer(t_nmf_interface_desc* itf) {
  shared_fillthisbuffer *stub = new shared_fillthisbuffer();
  if(stub == 0x0)
    return 0x0;
  *(NMF::InterfaceReference*)itf = stub;
  return &stub->host2mpcId;
}

/* Generated C++ stub for 'sendcommand'
   Generator:tools.generator.legacy.cpp.user.stub */
#include <host/sendcommand.hpp>

class sendcommand: public sendcommandDescriptor
{
  public:
    t_cm_bf_host2mpc_handle    host2mpcId;

  public:
    virtual void sendCommand(OMX_COMMANDTYPE cmd, t_uword param) {
      t_uint16 _xyuv_data[4];
      /* cmd <OMX_COMMANDTYPE> marshalling */
      _xyuv_data[0] = (t_uint16)((unsigned int)cmd & 0xFFFFU);
      _xyuv_data[0+1] = (t_uint16)((unsigned int)cmd >> 16);
      /* param <t_uword> marshalling */
      _xyuv_data[2] = (t_uint16)param;
      _xyuv_data[2+1] = (t_uint16)(param >> 16);
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 4*2, 0);
    }
};
static t_cm_bf_host2mpc_handle* INITstub_sendcommand(t_nmf_interface_desc* itf) {
  sendcommand *stub = new sendcommand();
  if(stub == 0x0)
    return 0x0;
  *(NMF::InterfaceReference*)itf = stub;
  return &stub->host2mpcId;
}

/* Generated C++ stub for 'fsminit'
   Generator:tools.generator.legacy.cpp.user.stub */
#include <host/fsminit.hpp>

class fsminit: public fsminitDescriptor
{
  public:
    t_cm_bf_host2mpc_handle    host2mpcId;

  public:
    virtual void fsmInit(fsmInit_t init) {
      t_uint16 _xyuv_data[5];
      /* init <fsmInit_t> marshalling */
      _xyuv_data[0+0] = (t_uint16)init.portsDisabled;
      _xyuv_data[0+1] = (t_uint16)init.portsTunneled;
      _xyuv_data[0+2] = (t_uint16)((unsigned int)init.traceInfoAddr & 0xFFFFU);
      _xyuv_data[0+2+1] = (t_uint16)((unsigned int)init.traceInfoAddr >> 16);
      _xyuv_data[0+4] = (t_uint16)init.id1;
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 5*2, 0);
    }
    virtual void setTunnelStatus(t_sint16 portIdx, t_uint16 isTunneled) {
      t_uint16 _xyuv_data[2];
      /* portIdx <t_sint16> marshalling */
      _xyuv_data[0] = (t_uint16)portIdx;
      /* isTunneled <t_uint16> marshalling */
      _xyuv_data[1] = (t_uint16)isTunneled;
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 2*2, 1);
    }
};
static t_cm_bf_host2mpc_handle* INITstub_fsminit(t_nmf_interface_desc* itf) {
  fsminit *stub = new fsminit();
  if(stub == 0x0)
    return 0x0;
  *(NMF::InterfaceReference*)itf = stub;
  return &stub->host2mpcId;
}

/* Generated C++ stub for 'dummy'
   Generator:tools.generator.legacy.cpp.user.stub */
#include <host/dummy.hpp>

class dummy: public dummyDescriptor
{
  public:
    t_cm_bf_host2mpc_handle    host2mpcId;

  public:
};
static t_cm_bf_host2mpc_handle* INITstub_dummy(t_nmf_interface_desc* itf) {
  dummy *stub = new dummy();
  if(stub == 0x0)
    return 0x0;
  *(NMF::InterfaceReference*)itf = stub;
  return &stub->host2mpcId;
}

static t_nmf_stub_function ens_cpp_stub_functions[] = {
  {"shared_emptythisbuffer", &INITstub_shared_emptythisbuffer},
  {"shared_fillthisbuffer", &INITstub_shared_fillthisbuffer},
  {"sendcommand", &INITstub_sendcommand},
  {"fsminit", &INITstub_fsminit},
  {"dummy", &INITstub_dummy},
};

/*const */t_nmf_stub_register ens_cpp_stub_register = {
    NMF_BC_IN_CPP,
    sizeof(ens_cpp_stub_functions)/sizeof(t_nmf_stub_function),
    ens_cpp_stub_functions
};
