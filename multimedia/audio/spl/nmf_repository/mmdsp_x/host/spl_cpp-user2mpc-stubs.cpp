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

/* Generated C++ stub for 'spl.nmfil.effect.configure'
   Generator:tools.generator.legacy.cpp.user.stub */
#include <host/spl/nmfil/effect/configure.hpp>

class spl_nmfil_effect_configure: public spl_nmfil_effect_configureDescriptor
{
  public:
    t_cm_bf_host2mpc_handle    host2mpcId;

  public:
    virtual void setParameter(SPLimiterParams_t StaticParams) {
      t_uint16 _xyuv_data[23];
      /* StaticParams <SPLimiterParams_t> marshalling */
      _xyuv_data[0+0] = (t_uint16)StaticParams.instance;
      _xyuv_data[0+1] = (t_uint16)StaticParams.iSamplingFreq;
      _xyuv_data[0+2] = (t_uint16)StaticParams.iNumberChannel;
      _xyuv_data[0+3] = (t_uint16)StaticParams.iType;
      _xyuv_data[0+4] = (t_uint16)StaticParams.iPeakClippingMode;
      _xyuv_data[0+5] = (t_uint16)StaticParams.iPowerAttackTime_l;
      _xyuv_data[0+6] = (t_uint16)StaticParams.iPowerAttackTime_h;
      _xyuv_data[0+7] = (t_uint16)StaticParams.iPowerReleaseTime_l;
      _xyuv_data[0+8] = (t_uint16)StaticParams.iPowerReleaseTime_h;
      _xyuv_data[0+9] = (t_uint16)StaticParams.iGainAttackTime_l;
      _xyuv_data[0+10] = (t_uint16)StaticParams.iGainAttackTime_h;
      _xyuv_data[0+11] = (t_uint16)StaticParams.iGainReleaseTime_l;
      _xyuv_data[0+12] = (t_uint16)StaticParams.iGainReleaseTime_h;
      _xyuv_data[0+13] = (t_uint16)StaticParams.iGainHoldTime_l;
      _xyuv_data[0+14] = (t_uint16)StaticParams.iGainHoldTime_h;
      _xyuv_data[0+15] = (t_uint16)StaticParams.iThreshAttackTime_l;
      _xyuv_data[0+16] = (t_uint16)StaticParams.iThreshAttackTime_h;
      _xyuv_data[0+17] = (t_uint16)StaticParams.iThreshReleaseTime_l;
      _xyuv_data[0+18] = (t_uint16)StaticParams.iThreshReleaseTime_h;
      _xyuv_data[0+19] = (t_uint16)StaticParams.iThreshHoldTime_l;
      _xyuv_data[0+20] = (t_uint16)StaticParams.iThreshHoldTime_h;
      _xyuv_data[0+21] = (t_uint16)StaticParams.iBlockSize;
      _xyuv_data[0+22] = (t_uint16)StaticParams.iLookAheadSize;
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 23*2, 0);
    }
    virtual void setConfig(SPLimiterConfig_t DynamicParams) {
      t_uint16 _xyuv_data[5];
      /* DynamicParams <SPLimiterConfig_t> marshalling */
      _xyuv_data[0+0] = (t_uint16)DynamicParams.iEnable;
      _xyuv_data[0+1] = (t_uint16)DynamicParams.iThreshold;
      _xyuv_data[0+2] = (t_uint16)DynamicParams.iSPL_Mode;
      _xyuv_data[0+3] = (t_uint16)DynamicParams.iSPL_UserGain;
      _xyuv_data[0+4] = (t_uint16)DynamicParams.iPeakL_Gain;
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 5*2, 1);
    }
};
static t_cm_bf_host2mpc_handle* INITstub_spl_nmfil_effect_configure(t_nmf_interface_desc* itf) {
  spl_nmfil_effect_configure *stub = new spl_nmfil_effect_configure();
  if(stub == 0x0)
    return 0x0;
  *(NMF::InterfaceReference*)itf = stub;
  return &stub->host2mpcId;
}

static t_nmf_stub_function spl_cpp_stub_functions[] = {
  {"spl.nmfil.effect.configure", &INITstub_spl_nmfil_effect_configure},
};

/*const */t_nmf_stub_register spl_cpp_stub_register = {
    NMF_BC_IN_CPP,
    sizeof(spl_cpp_stub_functions)/sizeof(t_nmf_stub_function),
    spl_cpp_stub_functions
};
