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

/* Generated C++ stub for 'stereowidener.nmfil.effect.configure'
   Generator:tools.generator.legacy.cpp.user.stub */
#include <host/stereowidener/nmfil/effect/configure.hpp>

class stereowidener_nmfil_effect_configure: public stereowidener_nmfil_effect_configureDescriptor
{
  public:
    t_cm_bf_host2mpc_handle    host2mpcId;

  public:
    virtual void setParameter(StereowidenerParams_t params) {
      t_uint16 _xyuv_data[3];
      /* params <StereowidenerParams_t> marshalling */
      _xyuv_data[0+0] = (t_uint16)params.iMemoryPreset;
      _xyuv_data[0+1] = (t_uint16)params.iSamplingFreq;
      _xyuv_data[0+2] = (t_uint16)params.iChannelsIn;
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 3*2, 0);
    }
    virtual void setConfig(StereowidenerConfig_t config) {
      t_uint16 _xyuv_data[5];
      /* config <StereowidenerConfig_t> marshalling */
      _xyuv_data[0+0] = (t_uint16)config.iEnable;
      _xyuv_data[0+1] = (t_uint16)config.iOutputMode;
      _xyuv_data[0+2] = (t_uint16)config.iHeadphoneMode;
      _xyuv_data[0+3] = (t_uint16)config.iInternalExternalSpeaker;
      _xyuv_data[0+4] = (t_uint16)config.iStrength;
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 5*2, 1);
    }
};
static t_cm_bf_host2mpc_handle* INITstub_stereowidener_nmfil_effect_configure(t_nmf_interface_desc* itf) {
  stereowidener_nmfil_effect_configure *stub = new stereowidener_nmfil_effect_configure();
  if(stub == 0x0)
    return 0x0;
  *(NMF::InterfaceReference*)itf = stub;
  return &stub->host2mpcId;
}

static t_nmf_stub_function stereowidener_cpp_stub_functions[] = {
  {"stereowidener.nmfil.effect.configure", &INITstub_stereowidener_nmfil_effect_configure},
};

/*const */t_nmf_stub_register stereowidener_cpp_stub_register = {
    NMF_BC_IN_CPP,
    sizeof(stereowidener_cpp_stub_functions)/sizeof(t_nmf_stub_function),
    stereowidener_cpp_stub_functions
};
