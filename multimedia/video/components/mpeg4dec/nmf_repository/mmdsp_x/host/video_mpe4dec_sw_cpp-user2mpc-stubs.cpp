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

/* Generated C++ stub for 'mpeg4dec.mpc.api.algo_deblock'
   Generator:tools.generator.legacy.cpp.user.stub */
#include <host/mpeg4dec/mpc/api/algo_deblock.hpp>

class mpeg4dec_mpc_api_algo_deblock: public mpeg4dec_mpc_api_algo_deblockDescriptor
{
  public:
    t_cm_bf_host2mpc_handle    host2mpcId;

  public:
    virtual void configureAlgo(t_uint16 Width, t_uint16 Height, t_uint32 PostFilterLevel, t_uint32 MQ_Mode, t_uint32 QP_p, t_uint32 Input_p, t_uint32 Output_p, t_uint32 Temp_p) {
      t_uint16 _xyuv_data[14];
      /* Width <t_uint16> marshalling */
      _xyuv_data[0] = (t_uint16)Width;
      /* Height <t_uint16> marshalling */
      _xyuv_data[1] = (t_uint16)Height;
      /* PostFilterLevel <t_uint32> marshalling */
      _xyuv_data[2] = (t_uint16)PostFilterLevel;
      _xyuv_data[2+1] = (t_uint16)(PostFilterLevel >> 16);
      /* MQ_Mode <t_uint32> marshalling */
      _xyuv_data[4] = (t_uint16)MQ_Mode;
      _xyuv_data[4+1] = (t_uint16)(MQ_Mode >> 16);
      /* QP_p <t_uint32> marshalling */
      _xyuv_data[6] = (t_uint16)QP_p;
      _xyuv_data[6+1] = (t_uint16)(QP_p >> 16);
      /* Input_p <t_uint32> marshalling */
      _xyuv_data[8] = (t_uint16)Input_p;
      _xyuv_data[8+1] = (t_uint16)(Input_p >> 16);
      /* Output_p <t_uint32> marshalling */
      _xyuv_data[10] = (t_uint16)Output_p;
      _xyuv_data[10+1] = (t_uint16)(Output_p >> 16);
      /* Temp_p <t_uint32> marshalling */
      _xyuv_data[12] = (t_uint16)Temp_p;
      _xyuv_data[12+1] = (t_uint16)(Temp_p >> 16);
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 14*2, 0);
    }
    virtual void controlAlgo(t_t1xhv_command command, t_uint16 param) {
      t_uint16 _xyuv_data[3];
      /* command <t_t1xhv_command> marshalling */
      _xyuv_data[0] = (t_uint16)((unsigned int)command & 0xFFFFU);
      _xyuv_data[0+1] = (t_uint16)((unsigned int)command >> 16);
      /* param <t_uint16> marshalling */
      _xyuv_data[2] = (t_uint16)param;
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 3*2, 1);
    }
};
static t_cm_bf_host2mpc_handle* INITstub_mpeg4dec_mpc_api_algo_deblock(t_nmf_interface_desc* itf) {
  mpeg4dec_mpc_api_algo_deblock *stub = new mpeg4dec_mpc_api_algo_deblock();
  if(stub == 0x0)
    return 0x0;
  *(NMF::InterfaceReference*)itf = stub;
  return &stub->host2mpcId;
}

static t_nmf_stub_function video_mpe4dec_sw_cpp_stub_functions[] = {
  {"mpeg4dec.mpc.api.algo_deblock", &INITstub_mpeg4dec_mpc_api_algo_deblock},
};

/*const */t_nmf_stub_register video_mpe4dec_sw_cpp_stub_register = {
    NMF_BC_IN_CPP,
    sizeof(video_mpe4dec_sw_cpp_stub_functions)/sizeof(t_nmf_stub_function),
    video_mpe4dec_sw_cpp_stub_functions
};
