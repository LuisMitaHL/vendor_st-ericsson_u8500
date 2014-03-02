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

/* Generated C++ stub for 'mpeg4enc.mpc.api.ddep'
   Generator:tools.generator.legacy.cpp.user.stub */
#include <host/mpeg4enc/mpc/api/ddep.hpp>

class mpeg4enc_mpc_api_ddep: public mpeg4enc_mpc_api_ddepDescriptor
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
    virtual void setParameter(t_uint16 channelId, t_t1xhv_algo_id algoId, ts_ddep_buffer_descriptor ddep_vec_mpeg_param_desc) {
      t_uint16 _xyuv_data[11];
      /* channelId <t_uint16> marshalling */
      _xyuv_data[0] = (t_uint16)channelId;
      /* algoId <t_t1xhv_algo_id> marshalling */
      _xyuv_data[1] = (t_uint16)((unsigned int)algoId & 0xFFFFU);
      _xyuv_data[1+1] = (t_uint16)((unsigned int)algoId >> 16);
      /* ddep_vec_mpeg_param_desc <ts_ddep_buffer_descriptor> marshalling */
      _xyuv_data[3+0] = (t_uint16)ddep_vec_mpeg_param_desc.nSize;
      _xyuv_data[3+0+1] = (t_uint16)(ddep_vec_mpeg_param_desc.nSize >> 16);
      _xyuv_data[3+2] = (t_uint16)ddep_vec_mpeg_param_desc.nPhysicalAddress;
      _xyuv_data[3+2+1] = (t_uint16)(ddep_vec_mpeg_param_desc.nPhysicalAddress >> 16);
      _xyuv_data[3+4] = (t_uint16)ddep_vec_mpeg_param_desc.nLogicalAddress;
      _xyuv_data[3+4+1] = (t_uint16)(ddep_vec_mpeg_param_desc.nLogicalAddress >> 16);
      _xyuv_data[3+6] = (t_uint16)ddep_vec_mpeg_param_desc.nMpcAddress;
      _xyuv_data[3+6+1] = (t_uint16)(ddep_vec_mpeg_param_desc.nMpcAddress >> 16);
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
    virtual void setCropForStab(t_uint32 crop_left_offset, t_uint32 crop_top_offset, t_uint32 overscan_width, t_uint32 overscan_height) {
      t_uint16 _xyuv_data[8];
      /* crop_left_offset <t_uint32> marshalling */
      _xyuv_data[0] = (t_uint16)crop_left_offset;
      _xyuv_data[0+1] = (t_uint16)(crop_left_offset >> 16);
      /* crop_top_offset <t_uint32> marshalling */
      _xyuv_data[2] = (t_uint16)crop_top_offset;
      _xyuv_data[2+1] = (t_uint16)(crop_top_offset >> 16);
      /* overscan_width <t_uint32> marshalling */
      _xyuv_data[4] = (t_uint16)overscan_width;
      _xyuv_data[4+1] = (t_uint16)(overscan_width >> 16);
      /* overscan_height <t_uint32> marshalling */
      _xyuv_data[6] = (t_uint16)overscan_height;
      _xyuv_data[6+1] = (t_uint16)(overscan_height >> 16);
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 8*2, 3);
    }
    virtual void disableFWCodeexection(t_bool value) {
      t_uint16 _xyuv_data[1];
      /* value <t_bool> marshalling */
      _xyuv_data[0] = (t_uint16)value;
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 1*2, 4);
    }
};
static t_cm_bf_host2mpc_handle* INITstub_mpeg4enc_mpc_api_ddep(t_nmf_interface_desc* itf) {
  mpeg4enc_mpc_api_ddep *stub = new mpeg4enc_mpc_api_ddep();
  if(stub == 0x0)
    return 0x0;
  *(NMF::InterfaceReference*)itf = stub;
  return &stub->host2mpcId;
}

static t_nmf_stub_function video_mpeg4enc_cpp_stub_functions[] = {
  {"mpeg4enc.mpc.api.ddep", &INITstub_mpeg4enc_mpc_api_ddep},
};

/*const */t_nmf_stub_register video_mpeg4enc_cpp_stub_register = {
    NMF_BC_IN_CPP,
    sizeof(video_mpeg4enc_cpp_stub_functions)/sizeof(t_nmf_stub_function),
    video_mpeg4enc_cpp_stub_functions
};
