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

/* Generated C++ stub for 'jpegenc.mpc.api.ddep'
   Generator:tools.generator.legacy.cpp.user.stub */
#include <host/jpegenc/mpc/api/ddep.hpp>

class jpegenc_mpc_api_ddep: public jpegenc_mpc_api_ddepDescriptor
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
    virtual void setParameter(t_uint16 channelId, t_t1xhv_algo_id algoId, ts_ddep_buffer_descriptor ddep_sec_jpeg_param_desc) {
      t_uint16 _xyuv_data[11];
      /* channelId <t_uint16> marshalling */
      _xyuv_data[0] = (t_uint16)channelId;
      /* algoId <t_t1xhv_algo_id> marshalling */
      _xyuv_data[1] = (t_uint16)((unsigned int)algoId & 0xFFFFU);
      _xyuv_data[1+1] = (t_uint16)((unsigned int)algoId >> 16);
      /* ddep_sec_jpeg_param_desc <ts_ddep_buffer_descriptor> marshalling */
      _xyuv_data[3+0] = (t_uint16)ddep_sec_jpeg_param_desc.nSize;
      _xyuv_data[3+0+1] = (t_uint16)(ddep_sec_jpeg_param_desc.nSize >> 16);
      _xyuv_data[3+2] = (t_uint16)ddep_sec_jpeg_param_desc.nPhysicalAddress;
      _xyuv_data[3+2+1] = (t_uint16)(ddep_sec_jpeg_param_desc.nPhysicalAddress >> 16);
      _xyuv_data[3+4] = (t_uint16)ddep_sec_jpeg_param_desc.nLogicalAddress;
      _xyuv_data[3+4+1] = (t_uint16)(ddep_sec_jpeg_param_desc.nLogicalAddress >> 16);
      _xyuv_data[3+6] = (t_uint16)ddep_sec_jpeg_param_desc.nMpcAddress;
      _xyuv_data[3+6+1] = (t_uint16)(ddep_sec_jpeg_param_desc.nMpcAddress >> 16);
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 11*2, 1);
    }
    virtual void setNeeds(t_uint16 channelId, ts_ddep_buffer_descriptor bbm_desc, ts_ddep_buffer_descriptor debugBuffer_desc, t_uint32 temp_buff_address) {
      t_uint16 _xyuv_data[19];
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
      /* temp_buff_address <t_uint32> marshalling */
      _xyuv_data[17] = (t_uint16)temp_buff_address;
      _xyuv_data[17+1] = (t_uint16)(temp_buff_address >> 16);
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 19*2, 2);
    }
    virtual void disableFWCodeexection(t_bool value) {
      t_uint16 _xyuv_data[1];
      /* value <t_bool> marshalling */
      _xyuv_data[0] = (t_uint16)value;
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 1*2, 3);
    }
};
static t_cm_bf_host2mpc_handle* INITstub_jpegenc_mpc_api_ddep(t_nmf_interface_desc* itf) {
  jpegenc_mpc_api_ddep *stub = new jpegenc_mpc_api_ddep();
  if(stub == 0x0)
    return 0x0;
  *(NMF::InterfaceReference*)itf = stub;
  return &stub->host2mpcId;
}

static t_nmf_stub_function video_jpegenc_cpp_stub_functions[] = {
  {"jpegenc.mpc.api.ddep", &INITstub_jpegenc_mpc_api_ddep},
};

/*const */t_nmf_stub_register video_jpegenc_cpp_stub_register = {
    NMF_BC_IN_CPP,
    sizeof(video_jpegenc_cpp_stub_functions)/sizeof(t_nmf_stub_function),
    video_jpegenc_cpp_stub_functions
};
