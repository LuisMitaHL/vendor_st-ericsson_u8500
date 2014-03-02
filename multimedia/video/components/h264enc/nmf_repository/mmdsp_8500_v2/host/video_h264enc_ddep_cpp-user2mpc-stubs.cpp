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

/* Generated C++ stub for 'h264enc.mpc.api.mpc_trace_init'
   Generator:tools.generator.legacy.cpp.user.stub */
#include <host/h264enc/mpc/api/mpc_trace_init.hpp>

class h264enc_mpc_api_mpc_trace_init: public h264enc_mpc_api_mpc_trace_initDescriptor
{
  public:
    t_cm_bf_host2mpc_handle    host2mpcId;

  public:
    virtual void traceInit(t_uint16 addrH, t_uint16 addrL, t_uint16 id) {
      t_uint16 _xyuv_data[3];
      /* addrH <t_uint16> marshalling */
      _xyuv_data[0] = (t_uint16)addrH;
      /* addrL <t_uint16> marshalling */
      _xyuv_data[1] = (t_uint16)addrL;
      /* id <t_uint16> marshalling */
      _xyuv_data[2] = (t_uint16)id;
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 3*2, 0);
    }
};
static t_cm_bf_host2mpc_handle* INITstub_h264enc_mpc_api_mpc_trace_init(t_nmf_interface_desc* itf) {
  h264enc_mpc_api_mpc_trace_init *stub = new h264enc_mpc_api_mpc_trace_init();
  if(stub == 0x0)
    return 0x0;
  *(NMF::InterfaceReference*)itf = stub;
  return &stub->host2mpcId;
}

/* Generated C++ stub for 'h264enc.mpc.ddep.api.start_codec'
   Generator:tools.generator.legacy.cpp.user.stub */
#include <host/h264enc/mpc/ddep/api/start_codec.hpp>

class h264enc_mpc_ddep_api_start_codec: public h264enc_mpc_ddep_api_start_codecDescriptor
{
  public:
    t_cm_bf_host2mpc_handle    host2mpcId;

  public:
    virtual void startCodec(t_uint32 addr_in_frame_buffer, t_uint32 addr_out_frame_buffer, t_uint32 addr_internal_buffer, t_uint32 addr_in_header_buffer, t_uint32 addr_in_bitstream_buffer, t_uint32 addr_out_bitstream_buffer, t_uint32 addr_in_parameters, t_uint32 addr_out_parameters, t_uint32 addr_in_frame_parameters, t_uint32 addr_out_frame_parameters) {
      t_uint16 _xyuv_data[20];
      /* addr_in_frame_buffer <t_uint32> marshalling */
      _xyuv_data[0] = (t_uint16)addr_in_frame_buffer;
      _xyuv_data[0+1] = (t_uint16)(addr_in_frame_buffer >> 16);
      /* addr_out_frame_buffer <t_uint32> marshalling */
      _xyuv_data[2] = (t_uint16)addr_out_frame_buffer;
      _xyuv_data[2+1] = (t_uint16)(addr_out_frame_buffer >> 16);
      /* addr_internal_buffer <t_uint32> marshalling */
      _xyuv_data[4] = (t_uint16)addr_internal_buffer;
      _xyuv_data[4+1] = (t_uint16)(addr_internal_buffer >> 16);
      /* addr_in_header_buffer <t_uint32> marshalling */
      _xyuv_data[6] = (t_uint16)addr_in_header_buffer;
      _xyuv_data[6+1] = (t_uint16)(addr_in_header_buffer >> 16);
      /* addr_in_bitstream_buffer <t_uint32> marshalling */
      _xyuv_data[8] = (t_uint16)addr_in_bitstream_buffer;
      _xyuv_data[8+1] = (t_uint16)(addr_in_bitstream_buffer >> 16);
      /* addr_out_bitstream_buffer <t_uint32> marshalling */
      _xyuv_data[10] = (t_uint16)addr_out_bitstream_buffer;
      _xyuv_data[10+1] = (t_uint16)(addr_out_bitstream_buffer >> 16);
      /* addr_in_parameters <t_uint32> marshalling */
      _xyuv_data[12] = (t_uint16)addr_in_parameters;
      _xyuv_data[12+1] = (t_uint16)(addr_in_parameters >> 16);
      /* addr_out_parameters <t_uint32> marshalling */
      _xyuv_data[14] = (t_uint16)addr_out_parameters;
      _xyuv_data[14+1] = (t_uint16)(addr_out_parameters >> 16);
      /* addr_in_frame_parameters <t_uint32> marshalling */
      _xyuv_data[16] = (t_uint16)addr_in_frame_parameters;
      _xyuv_data[16+1] = (t_uint16)(addr_in_frame_parameters >> 16);
      /* addr_out_frame_parameters <t_uint32> marshalling */
      _xyuv_data[18] = (t_uint16)addr_out_frame_parameters;
      _xyuv_data[18+1] = (t_uint16)(addr_out_frame_parameters >> 16);
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 20*2, 0);
    }
    virtual void flush(void) {
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, (t_uint16*)0x0, 0, 1);
    }
};
static t_cm_bf_host2mpc_handle* INITstub_h264enc_mpc_ddep_api_start_codec(t_nmf_interface_desc* itf) {
  h264enc_mpc_ddep_api_start_codec *stub = new h264enc_mpc_ddep_api_start_codec();
  if(stub == 0x0)
    return 0x0;
  *(NMF::InterfaceReference*)itf = stub;
  return &stub->host2mpcId;
}

static t_nmf_stub_function video_h264enc_ddep_cpp_stub_functions[] = {
  {"h264enc.mpc.api.mpc_trace_init", &INITstub_h264enc_mpc_api_mpc_trace_init},
  {"h264enc.mpc.ddep.api.start_codec", &INITstub_h264enc_mpc_ddep_api_start_codec},
};

/*const */t_nmf_stub_register video_h264enc_ddep_cpp_stub_register = {
    NMF_BC_IN_CPP,
    sizeof(video_h264enc_ddep_cpp_stub_functions)/sizeof(t_nmf_stub_function),
    video_h264enc_ddep_cpp_stub_functions
};
