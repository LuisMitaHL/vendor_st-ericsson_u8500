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

/* Generated C++ stub for 'vc1dec.mpc.ddep.api.start_codec'
   Generator:tools.generator.legacy.cpp.user.stub */
#include <host/vc1dec/mpc/ddep/api/start_codec.hpp>

class vc1dec_mpc_ddep_api_start_codec: public vc1dec_mpc_ddep_api_start_codecDescriptor
{
  public:
    t_cm_bf_host2mpc_handle    host2mpcId;

  public:
    virtual void startCodec(t_uint32 addr_in_frame_buffer, t_uint32 addr_out_frame_buffer, t_uint32 addr_internal_buffer, t_uint32 addr_in_bitstream_buffer, t_uint32 addr_out_bitstream_buffer, t_uint32 addr_in_parameters, t_uint32 addr_out_parameters, t_uint32 addr_in_frame_parameters, t_uint32 addr_out_frame_parameters) {
      t_uint16 _xyuv_data[18];
      /* addr_in_frame_buffer <t_uint32> marshalling */
      _xyuv_data[0] = (t_uint16)addr_in_frame_buffer;
      _xyuv_data[0+1] = (t_uint16)(addr_in_frame_buffer >> 16);
      /* addr_out_frame_buffer <t_uint32> marshalling */
      _xyuv_data[2] = (t_uint16)addr_out_frame_buffer;
      _xyuv_data[2+1] = (t_uint16)(addr_out_frame_buffer >> 16);
      /* addr_internal_buffer <t_uint32> marshalling */
      _xyuv_data[4] = (t_uint16)addr_internal_buffer;
      _xyuv_data[4+1] = (t_uint16)(addr_internal_buffer >> 16);
      /* addr_in_bitstream_buffer <t_uint32> marshalling */
      _xyuv_data[6] = (t_uint16)addr_in_bitstream_buffer;
      _xyuv_data[6+1] = (t_uint16)(addr_in_bitstream_buffer >> 16);
      /* addr_out_bitstream_buffer <t_uint32> marshalling */
      _xyuv_data[8] = (t_uint16)addr_out_bitstream_buffer;
      _xyuv_data[8+1] = (t_uint16)(addr_out_bitstream_buffer >> 16);
      /* addr_in_parameters <t_uint32> marshalling */
      _xyuv_data[10] = (t_uint16)addr_in_parameters;
      _xyuv_data[10+1] = (t_uint16)(addr_in_parameters >> 16);
      /* addr_out_parameters <t_uint32> marshalling */
      _xyuv_data[12] = (t_uint16)addr_out_parameters;
      _xyuv_data[12+1] = (t_uint16)(addr_out_parameters >> 16);
      /* addr_in_frame_parameters <t_uint32> marshalling */
      _xyuv_data[14] = (t_uint16)addr_in_frame_parameters;
      _xyuv_data[14+1] = (t_uint16)(addr_in_frame_parameters >> 16);
      /* addr_out_frame_parameters <t_uint32> marshalling */
      _xyuv_data[16] = (t_uint16)addr_out_frame_parameters;
      _xyuv_data[16+1] = (t_uint16)(addr_out_frame_parameters >> 16);
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 18*2, 0);
    }
    virtual void initCodec(t_uint32 param_in) {
      t_uint16 _xyuv_data[2];
      /* param_in <t_uint32> marshalling */
      _xyuv_data[0] = (t_uint16)param_in;
      _xyuv_data[0+1] = (t_uint16)(param_in >> 16);
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 2*2, 1);
    }
    virtual void voidFrame(void) {
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, (t_uint16*)0x0, 0, 2);
    }
    virtual void allowSleep(void) {
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, (t_uint16*)0x0, 0, 3);
    }
    virtual void preventSleep(void) {
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, (t_uint16*)0x0, 0, 4);
    }
};
static t_cm_bf_host2mpc_handle* INITstub_vc1dec_mpc_ddep_api_start_codec(t_nmf_interface_desc* itf) {
  vc1dec_mpc_ddep_api_start_codec *stub = new vc1dec_mpc_ddep_api_start_codec();
  if(stub == 0x0)
    return 0x0;
  *(NMF::InterfaceReference*)itf = stub;
  return &stub->host2mpcId;
}

/* Generated C++ stub for 'vc1dec.mpc.api.mpc_trace_init'
   Generator:tools.generator.legacy.cpp.user.stub */
#include <host/vc1dec/mpc/api/mpc_trace_init.hpp>

class vc1dec_mpc_api_mpc_trace_init: public vc1dec_mpc_api_mpc_trace_initDescriptor
{
  public:
    t_cm_bf_host2mpc_handle    host2mpcId;

  public:
    virtual void traceInit(TraceInfo_t trace_info, t_uint16 id) {
      t_uint16 _xyuv_data[6];
      /* trace_info <TraceInfo_t> marshalling */
      _xyuv_data[0+0] = (t_uint16)trace_info.parentHandle;
      _xyuv_data[0+0+1] = (t_uint16)(trace_info.parentHandle >> 16);
      _xyuv_data[0+2] = (t_uint16)trace_info.traceEnable;
      _xyuv_data[0+3] = (t_uint16)trace_info.dspAddr;
      _xyuv_data[0+3+1] = (t_uint16)(trace_info.dspAddr >> 16);
      /* id <t_uint16> marshalling */
      _xyuv_data[5] = (t_uint16)id;
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 6*2, 0);
    }
};
static t_cm_bf_host2mpc_handle* INITstub_vc1dec_mpc_api_mpc_trace_init(t_nmf_interface_desc* itf) {
  vc1dec_mpc_api_mpc_trace_init *stub = new vc1dec_mpc_api_mpc_trace_init();
  if(stub == 0x0)
    return 0x0;
  *(NMF::InterfaceReference*)itf = stub;
  return &stub->host2mpcId;
}

static t_nmf_stub_function video_vc1dec_ddep_cpp_stub_functions[] = {
  {"vc1dec.mpc.ddep.api.start_codec", &INITstub_vc1dec_mpc_ddep_api_start_codec},
  {"vc1dec.mpc.api.mpc_trace_init", &INITstub_vc1dec_mpc_api_mpc_trace_init},
};

/*const */t_nmf_stub_register video_vc1dec_ddep_cpp_stub_register = {
    NMF_BC_IN_CPP,
    sizeof(video_vc1dec_ddep_cpp_stub_functions)/sizeof(t_nmf_stub_function),
    video_vc1dec_ddep_cpp_stub_functions
};
