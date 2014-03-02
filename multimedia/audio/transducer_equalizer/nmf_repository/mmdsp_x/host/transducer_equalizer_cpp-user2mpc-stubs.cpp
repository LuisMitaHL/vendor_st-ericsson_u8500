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

/* Generated C++ stub for 'transducer_equalizer.nmfil.effect.configure'
   Generator:tools.generator.legacy.cpp.user.stub */
#include <host/transducer_equalizer/nmfil/effect/configure.hpp>

class transducer_equalizer_nmfil_effect_configure: public transducer_equalizer_nmfil_effect_configureDescriptor
{
  public:
    t_cm_bf_host2mpc_handle    host2mpcId;

  public:
    virtual void setParameter(TransducerEqualizerParams_t StaticParams) {
      t_uint16 _xyuv_data[5];
      /* StaticParams <TransducerEqualizerParams_t> marshalling */
      _xyuv_data[0+0] = (t_uint16)StaticParams.iMemoryPreset;
      _xyuv_data[0+1] = (t_uint16)StaticParams.istereo;
      _xyuv_data[0+2] = (t_uint16)StaticParams.inb_alloc_biquad_cells_per_channel;
      _xyuv_data[0+3] = (t_uint16)StaticParams.inb_alloc_FIR_coefs_per_channel;
      _xyuv_data[0+4] = (t_uint16)StaticParams.ialert_on;
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 5*2, 0);
    }
    virtual void setConfig(t_uint32 DynamicParamsFifoDspAddress) {
      t_uint16 _xyuv_data[2];
      /* DynamicParamsFifoDspAddress <t_uint32> marshalling */
      _xyuv_data[0] = (t_uint16)DynamicParamsFifoDspAddress;
      _xyuv_data[0+1] = (t_uint16)(DynamicParamsFifoDspAddress >> 16);
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 2*2, 1);
    }
};
static t_cm_bf_host2mpc_handle* INITstub_transducer_equalizer_nmfil_effect_configure(t_nmf_interface_desc* itf) {
  transducer_equalizer_nmfil_effect_configure *stub = new transducer_equalizer_nmfil_effect_configure();
  if(stub == 0x0)
    return 0x0;
  *(NMF::InterfaceReference*)itf = stub;
  return &stub->host2mpcId;
}

static t_nmf_stub_function transducer_equalizer_cpp_stub_functions[] = {
  {"transducer_equalizer.nmfil.effect.configure", &INITstub_transducer_equalizer_nmfil_effect_configure},
};

/*const */t_nmf_stub_register transducer_equalizer_cpp_stub_register = {
    NMF_BC_IN_CPP,
    sizeof(transducer_equalizer_cpp_stub_functions)/sizeof(t_nmf_stub_function),
    transducer_equalizer_cpp_stub_functions
};
