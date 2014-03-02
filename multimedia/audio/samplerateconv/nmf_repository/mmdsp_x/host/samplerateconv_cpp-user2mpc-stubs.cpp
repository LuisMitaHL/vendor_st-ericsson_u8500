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

/* Generated C++ stub for 'samplerateconv.nmfil.wrapper.configure'
   Generator:tools.generator.legacy.cpp.user.stub */
#include <host/samplerateconv/nmfil/wrapper/configure.hpp>

class samplerateconv_nmfil_wrapper_configure: public samplerateconv_nmfil_wrapper_configureDescriptor
{
  public:
    t_cm_bf_host2mpc_handle    host2mpcId;

  public:
    virtual void setParameter(t_sample_freq input_freq, t_sample_freq output_freq, t_uint16 lowmips_to_out48, t_uint16 conv_type, t_uint16 blocksize, t_uint16 channels, t_uint16 force_reconfiguration) {
      t_uint16 _xyuv_data[9];
      /* input_freq <t_sample_freq> marshalling */
      _xyuv_data[0] = (t_uint16)((unsigned int)input_freq & 0xFFFFU);
      _xyuv_data[0+1] = (t_uint16)((unsigned int)input_freq >> 16);
      /* output_freq <t_sample_freq> marshalling */
      _xyuv_data[2] = (t_uint16)((unsigned int)output_freq & 0xFFFFU);
      _xyuv_data[2+1] = (t_uint16)((unsigned int)output_freq >> 16);
      /* lowmips_to_out48 <t_uint16> marshalling */
      _xyuv_data[4] = (t_uint16)lowmips_to_out48;
      /* conv_type <t_uint16> marshalling */
      _xyuv_data[5] = (t_uint16)conv_type;
      /* blocksize <t_uint16> marshalling */
      _xyuv_data[6] = (t_uint16)blocksize;
      /* channels <t_uint16> marshalling */
      _xyuv_data[7] = (t_uint16)channels;
      /* force_reconfiguration <t_uint16> marshalling */
      _xyuv_data[8] = (t_uint16)force_reconfiguration;
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 9*2, 0);
    }
};
static t_cm_bf_host2mpc_handle* INITstub_samplerateconv_nmfil_wrapper_configure(t_nmf_interface_desc* itf) {
  samplerateconv_nmfil_wrapper_configure *stub = new samplerateconv_nmfil_wrapper_configure();
  if(stub == 0x0)
    return 0x0;
  *(NMF::InterfaceReference*)itf = stub;
  return &stub->host2mpcId;
}

static t_nmf_stub_function samplerateconv_cpp_stub_functions[] = {
  {"samplerateconv.nmfil.wrapper.configure", &INITstub_samplerateconv_nmfil_wrapper_configure},
};

/*const */t_nmf_stub_register samplerateconv_cpp_stub_register = {
    NMF_BC_IN_CPP,
    sizeof(samplerateconv_cpp_stub_functions)/sizeof(t_nmf_stub_function),
    samplerateconv_cpp_stub_functions
};
