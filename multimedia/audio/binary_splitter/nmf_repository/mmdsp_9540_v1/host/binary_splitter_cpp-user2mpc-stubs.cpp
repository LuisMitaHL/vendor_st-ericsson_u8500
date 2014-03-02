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

/* Generated C++ stub for 'binary_splitter.nmfil.wrapper.configure'
   Generator:tools.generator.legacy.cpp.user.stub */
#include <host/binary_splitter/nmfil/wrapper/configure.hpp>

class binary_splitter_nmfil_wrapper_configure: public binary_splitter_nmfil_wrapper_configureDescriptor
{
  public:
    t_cm_bf_host2mpc_handle    host2mpcId;

  public:
    virtual void setParameter(t_sample_freq freq, t_channelConfig chConfig, t_uint16 blocksize) {
      t_uint16 _xyuv_data[6];
      int ___i0, ___j0;
      /* freq <t_sample_freq> marshalling */
      _xyuv_data[0] = (t_uint16)((unsigned int)freq & 0xFFFFU);
      _xyuv_data[0+1] = (t_uint16)((unsigned int)freq >> 16);
      /* chConfig <t_channelConfig> marshalling */
      for(___i0 = ___j0 = 0; ___i0 < 3; ___i0++) {
        _xyuv_data[2+0+___j0] = (t_uint16)chConfig.nChans[___i0];
        ___j0 += 1;
      }
      /* blocksize <t_uint16> marshalling */
      _xyuv_data[5] = (t_uint16)blocksize;
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 6*2, 0);
    }
    virtual void setOutputChannelConfig(t_uint16 portIdx, t_OutChannelConfig OutChannelConfig) {
      t_uint16 _xyuv_data[17];
      int ___i0, ___j0;
      /* portIdx <t_uint16> marshalling */
      _xyuv_data[0] = (t_uint16)portIdx;
      /* OutChannelConfig <t_OutChannelConfig> marshalling */
      for(___i0 = ___j0 = 0; ___i0 < 16; ___i0++) {
        _xyuv_data[1+0+___j0] = (t_uint16)OutChannelConfig.nOutPutChannel[___i0];
        ___j0 += 1;
      }
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 17*2, 1);
    }
    virtual void setTrace(t_uint32 addr) {
      t_uint16 _xyuv_data[2];
      /* addr <t_uint32> marshalling */
      _xyuv_data[0] = (t_uint16)addr;
      _xyuv_data[0+1] = (t_uint16)(addr >> 16);
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 2*2, 2);
    }
};
static t_cm_bf_host2mpc_handle* INITstub_binary_splitter_nmfil_wrapper_configure(t_nmf_interface_desc* itf) {
  binary_splitter_nmfil_wrapper_configure *stub = new binary_splitter_nmfil_wrapper_configure();
  if(stub == 0x0)
    return 0x0;
  *(NMF::InterfaceReference*)itf = stub;
  return &stub->host2mpcId;
}

static t_nmf_stub_function binary_splitter_cpp_stub_functions[] = {
  {"binary_splitter.nmfil.wrapper.configure", &INITstub_binary_splitter_nmfil_wrapper_configure},
};

/*const */t_nmf_stub_register binary_splitter_cpp_stub_register = {
    NMF_BC_IN_CPP,
    sizeof(binary_splitter_cpp_stub_functions)/sizeof(t_nmf_stub_function),
    binary_splitter_cpp_stub_functions
};
