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

/* Generated C++ stub for 'volctrl.nmfil.effect.configure'
   Generator:tools.generator.legacy.cpp.user.stub */
#include <host/volctrl/nmfil/effect/configure.hpp>

class volctrl_nmfil_effect_configure: public volctrl_nmfil_effect_configureDescriptor
{
  public:
    t_cm_bf_host2mpc_handle    host2mpcId;

  public:
    virtual void setConfig(VolctrlConfig_t config) {
      t_uint16 _xyuv_data[8];
      /* config <VolctrlConfig_t> marshalling */
      _xyuv_data[0+0] = (t_uint16)config.iDownMix;
      _xyuv_data[0+1] = (t_uint16)config.igll;
      _xyuv_data[0+2] = (t_uint16)config.iglr;
      _xyuv_data[0+3] = (t_uint16)config.igrl;
      _xyuv_data[0+4] = (t_uint16)config.igrr;
      _xyuv_data[0+5] = (t_uint16)config.ialpha;
      _xyuv_data[0+5+1] = (t_uint16)(config.ialpha >> 16);
      _xyuv_data[0+7] = (t_uint16)config.iDBRamp;
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 8*2, 0);
    }
    virtual void setRampConfig(VolctrlRampConfig_t config) {
      t_uint16 _xyuv_data[7];
      /* config <VolctrlRampConfig_t> marshalling */
      _xyuv_data[0+0] = (t_uint16)config.iStartVolume;
      _xyuv_data[0+1] = (t_uint16)config.iEndVolume;
      _xyuv_data[0+2] = (t_uint16)config.ialpha;
      _xyuv_data[0+2+1] = (t_uint16)(config.ialpha >> 16);
      _xyuv_data[0+4] = (t_uint16)config.iDBRamp;
      _xyuv_data[0+5] = (t_uint16)config.iChannel;
      _xyuv_data[0+6] = (t_uint16)config.iTerminate;
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 7*2, 1);
    }
};
static t_cm_bf_host2mpc_handle* INITstub_volctrl_nmfil_effect_configure(t_nmf_interface_desc* itf) {
  volctrl_nmfil_effect_configure *stub = new volctrl_nmfil_effect_configure();
  if(stub == 0x0)
    return 0x0;
  *(NMF::InterfaceReference*)itf = stub;
  return &stub->host2mpcId;
}

static t_nmf_stub_function volctrl_cpp_stub_functions[] = {
  {"volctrl.nmfil.effect.configure", &INITstub_volctrl_nmfil_effect_configure},
};

/*const */t_nmf_stub_register volctrl_cpp_stub_register = {
    NMF_BC_IN_CPP,
    sizeof(volctrl_cpp_stub_functions)/sizeof(t_nmf_stub_function),
    volctrl_cpp_stub_functions
};
