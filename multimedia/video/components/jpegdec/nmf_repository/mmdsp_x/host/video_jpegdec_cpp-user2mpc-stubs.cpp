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

/* Generated C++ stub for 'jpegdec.mpc.api.ddep'
   Generator:tools.generator.legacy.cpp.user.stub */
#include <host/jpegdec/mpc/api/ddep.hpp>

class jpegdec_mpc_api_ddep: public jpegdec_mpc_api_ddepDescriptor
{
  public:
    t_cm_bf_host2mpc_handle    host2mpcId;

  public:
    virtual void setConfig(t_uint16 channelId) {
      t_uint16 _xyuv_data[1];
      /* channelId <t_uint16> marshalling */
      _xyuv_data[0] = (t_uint16)channelId;
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 1*2, 0);
    }
    virtual void setParameter(t_uint16 channelId, t_t1xhv_algo_id algoId, ts_ddep_buffer_descriptor ddep_sdc_jpeg_param_desc) {
      t_uint16 _xyuv_data[11];
      /* channelId <t_uint16> marshalling */
      _xyuv_data[0] = (t_uint16)channelId;
      /* algoId <t_t1xhv_algo_id> marshalling */
      _xyuv_data[1] = (t_uint16)((unsigned int)algoId & 0xFFFFU);
      _xyuv_data[1+1] = (t_uint16)((unsigned int)algoId >> 16);
      /* ddep_sdc_jpeg_param_desc <ts_ddep_buffer_descriptor> marshalling */
      _xyuv_data[3+0] = (t_uint16)ddep_sdc_jpeg_param_desc.nSize;
      _xyuv_data[3+0+1] = (t_uint16)(ddep_sdc_jpeg_param_desc.nSize >> 16);
      _xyuv_data[3+2] = (t_uint16)ddep_sdc_jpeg_param_desc.nPhysicalAddress;
      _xyuv_data[3+2+1] = (t_uint16)(ddep_sdc_jpeg_param_desc.nPhysicalAddress >> 16);
      _xyuv_data[3+4] = (t_uint16)ddep_sdc_jpeg_param_desc.nLogicalAddress;
      _xyuv_data[3+4+1] = (t_uint16)(ddep_sdc_jpeg_param_desc.nLogicalAddress >> 16);
      _xyuv_data[3+6] = (t_uint16)ddep_sdc_jpeg_param_desc.nMpcAddress;
      _xyuv_data[3+6+1] = (t_uint16)(ddep_sdc_jpeg_param_desc.nMpcAddress >> 16);
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 11*2, 1);
    }
    virtual void setNeeds(t_uint16 channelId, ts_ddep_buffer_descriptor bbm_desc, ts_ddep_buffer_descriptor debugBuffer_desc) {
      t_uint16 _xyuv_data[17];
      /* channelId <t_uint16> marshalling */
      _xyuv_data[0] = (t_uint16)channelId;
      /* bbm_desc <ts_ddep_buffer_descriptor> marshalling */
      _xyuv_data[1+0] = (t_uint16)bbm_desc.nSize;
      _xyuv_data[1+0+1] = (t_uint16)(bbm_desc.nSize >> 16);
      _xyuv_data[1+2] = (t_uint16)bbm_desc.nPhysicalAddress;
      _xyuv_data[1+2+1] = (t_uint16)(bbm_desc.nPhysicalAddress >> 16);
      _xyuv_data[1+4] = (t_uint16)bbm_desc.nLogicalAddress;
      _xyuv_data[1+4+1] = (t_uint16)(bbm_desc.nLogicalAddress >> 16);
      _xyuv_data[1+6] = (t_uint16)bbm_desc.nMpcAddress;
      _xyuv_data[1+6+1] = (t_uint16)(bbm_desc.nMpcAddress >> 16);
      /* debugBuffer_desc <ts_ddep_buffer_descriptor> marshalling */
      _xyuv_data[9+0] = (t_uint16)debugBuffer_desc.nSize;
      _xyuv_data[9+0+1] = (t_uint16)(debugBuffer_desc.nSize >> 16);
      _xyuv_data[9+2] = (t_uint16)debugBuffer_desc.nPhysicalAddress;
      _xyuv_data[9+2+1] = (t_uint16)(debugBuffer_desc.nPhysicalAddress >> 16);
      _xyuv_data[9+4] = (t_uint16)debugBuffer_desc.nLogicalAddress;
      _xyuv_data[9+4+1] = (t_uint16)(debugBuffer_desc.nLogicalAddress >> 16);
      _xyuv_data[9+6] = (t_uint16)debugBuffer_desc.nMpcAddress;
      _xyuv_data[9+6+1] = (t_uint16)(debugBuffer_desc.nMpcAddress >> 16);
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 17*2, 2);
    }
    virtual void disableFWCodeexection(t_bool value) {
      t_uint16 _xyuv_data[1];
      /* value <t_bool> marshalling */
      _xyuv_data[0] = (t_uint16)value;
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 1*2, 3);
    }
};
static t_cm_bf_host2mpc_handle* INITstub_jpegdec_mpc_api_ddep(t_nmf_interface_desc* itf) {
  jpegdec_mpc_api_ddep *stub = new jpegdec_mpc_api_ddep();
  if(stub == 0x0)
    return 0x0;
  *(NMF::InterfaceReference*)itf = stub;
  return &stub->host2mpcId;
}

/* Generated C++ stub for 'jpegdec.mpc.api.emptythisheader'
   Generator:tools.generator.legacy.cpp.user.stub */
#include <host/jpegdec/mpc/api/emptythisheader.hpp>

class jpegdec_mpc_api_emptythisheader: public jpegdec_mpc_api_emptythisheaderDescriptor
{
  public:
    t_cm_bf_host2mpc_handle    host2mpcId;

  public:
    virtual void emptyThisHeader(Buffer_p buffer) {
      t_uint16 _xyuv_data[2];
      /* buffer <Buffer_p> marshalling */
      _xyuv_data[0] = (t_uint16)((unsigned int)buffer & 0xFFFFU);
      _xyuv_data[0+1] = (t_uint16)((unsigned int)buffer >> 16);
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 2*2, 0);
    }
};
static t_cm_bf_host2mpc_handle* INITstub_jpegdec_mpc_api_emptythisheader(t_nmf_interface_desc* itf) {
  jpegdec_mpc_api_emptythisheader *stub = new jpegdec_mpc_api_emptythisheader();
  if(stub == 0x0)
    return 0x0;
  *(NMF::InterfaceReference*)itf = stub;
  return &stub->host2mpcId;
}

/* Generated C++ stub for 'jpegdec.mpc.api.fillthisheader'
   Generator:tools.generator.legacy.cpp.user.stub */
#include <host/jpegdec/mpc/api/fillthisheader.hpp>

class jpegdec_mpc_api_fillthisheader: public jpegdec_mpc_api_fillthisheaderDescriptor
{
  public:
    t_cm_bf_host2mpc_handle    host2mpcId;

  public:
    virtual void fillThisHeader(Buffer_p buffer) {
      t_uint16 _xyuv_data[2];
      /* buffer <Buffer_p> marshalling */
      _xyuv_data[0] = (t_uint16)((unsigned int)buffer & 0xFFFFU);
      _xyuv_data[0+1] = (t_uint16)((unsigned int)buffer >> 16);
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 2*2, 0);
    }
};
static t_cm_bf_host2mpc_handle* INITstub_jpegdec_mpc_api_fillthisheader(t_nmf_interface_desc* itf) {
  jpegdec_mpc_api_fillthisheader *stub = new jpegdec_mpc_api_fillthisheader();
  if(stub == 0x0)
    return 0x0;
  *(NMF::InterfaceReference*)itf = stub;
  return &stub->host2mpcId;
}

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

static t_nmf_stub_function video_jpegdec_cpp_stub_functions[] = {
  {"jpegdec.mpc.api.ddep", &INITstub_jpegdec_mpc_api_ddep},
  {"jpegdec.mpc.api.emptythisheader", &INITstub_jpegdec_mpc_api_emptythisheader},
  {"jpegdec.mpc.api.fillthisheader", &INITstub_jpegdec_mpc_api_fillthisheader},
  {"shared_emptythisbuffer", &INITstub_shared_emptythisbuffer},
  {"shared_fillthisbuffer", &INITstub_shared_fillthisbuffer},
  {"sendcommand", &INITstub_sendcommand},
  {"fsminit", &INITstub_fsminit},
};

/*const */t_nmf_stub_register video_jpegdec_cpp_stub_register = {
    NMF_BC_IN_CPP,
    sizeof(video_jpegdec_cpp_stub_functions)/sizeof(t_nmf_stub_function),
    video_jpegdec_cpp_stub_functions
};
