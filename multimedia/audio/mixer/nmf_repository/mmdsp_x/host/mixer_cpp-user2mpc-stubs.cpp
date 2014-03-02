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

/* Generated C++ stub for 'mixer.nmfil.wrapper.configure'
   Generator:tools.generator.legacy.cpp.user.stub */
#include <host/mixer/nmfil/wrapper/configure.hpp>

class mixer_nmfil_wrapper_configure: public mixer_nmfil_wrapper_configureDescriptor
{
  public:
    t_cm_bf_host2mpc_handle    host2mpcId;

  public:
    virtual void setParameter(MixerParam_t mixerParam) {
      t_uint16 _xyuv_data[71];
      int ___i0, ___j0;
      /* mixerParam <MixerParam_t> marshalling */
      for(___i0 = ___j0 = 0; ___i0 < 8; ___i0++) {
        _xyuv_data[0+0+___j0+0] = (t_uint16)((unsigned int)mixerParam.inputPortParam[___i0].nSamplingRate & 0xFFFFU);
        _xyuv_data[0+0+___j0+0+1] = (t_uint16)((unsigned int)mixerParam.inputPortParam[___i0].nSamplingRate >> 16);
        _xyuv_data[0+0+___j0+2] = (t_uint16)mixerParam.inputPortParam[___i0].nChannels;
        _xyuv_data[0+0+___j0+3] = (t_uint16)mixerParam.inputPortParam[___i0].nBlockSize;
        _xyuv_data[0+0+___j0+4] = (t_uint16)((unsigned int)mixerParam.inputPortParam[___i0].nMemoryBank & 0xFFFFU);
        _xyuv_data[0+0+___j0+4+1] = (t_uint16)((unsigned int)mixerParam.inputPortParam[___i0].nMemoryBank >> 16);
        _xyuv_data[0+0+___j0+6] = (t_uint16)mixerParam.inputPortParam[___i0].bDisabled;
        _xyuv_data[0+0+___j0+7] = (t_uint16)mixerParam.inputPortParam[___i0].bPaused;
        ___j0 += 8;
      }
      _xyuv_data[0+64+0] = (t_uint16)mixerParam.outputPortParam.nBlockSize;
      _xyuv_data[0+64+1] = (t_uint16)((unsigned int)mixerParam.outputPortParam.nSamplingRate & 0xFFFFU);
      _xyuv_data[0+64+1+1] = (t_uint16)((unsigned int)mixerParam.outputPortParam.nSamplingRate >> 16);
      _xyuv_data[0+64+3] = (t_uint16)mixerParam.outputPortParam.nChannels;
      _xyuv_data[0+64+4] = (t_uint16)mixerParam.outputPortParam.bDisabled;
      _xyuv_data[0+69] = (t_uint16)mixerParam.traceAddr;
      _xyuv_data[0+69+1] = (t_uint16)(mixerParam.traceAddr >> 16);
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 71*2, 0);
    }
    virtual void setInputPortParameter(t_uint16 portIdx, InputPortParam_t inputPortParam) {
      t_uint16 _xyuv_data[9];
      /* portIdx <t_uint16> marshalling */
      _xyuv_data[0] = (t_uint16)portIdx;
      /* inputPortParam <InputPortParam_t> marshalling */
      _xyuv_data[1+0] = (t_uint16)((unsigned int)inputPortParam.nSamplingRate & 0xFFFFU);
      _xyuv_data[1+0+1] = (t_uint16)((unsigned int)inputPortParam.nSamplingRate >> 16);
      _xyuv_data[1+2] = (t_uint16)inputPortParam.nChannels;
      _xyuv_data[1+3] = (t_uint16)inputPortParam.nBlockSize;
      _xyuv_data[1+4] = (t_uint16)((unsigned int)inputPortParam.nMemoryBank & 0xFFFFU);
      _xyuv_data[1+4+1] = (t_uint16)((unsigned int)inputPortParam.nMemoryBank >> 16);
      _xyuv_data[1+6] = (t_uint16)inputPortParam.bDisabled;
      _xyuv_data[1+7] = (t_uint16)inputPortParam.bPaused;
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 9*2, 1);
    }
    virtual void configureEffects(t_uint32 paramDspAddress) {
      t_uint16 _xyuv_data[2];
      /* paramDspAddress <t_uint32> marshalling */
      _xyuv_data[0] = (t_uint16)paramDspAddress;
      _xyuv_data[0+1] = (t_uint16)(paramDspAddress >> 16);
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 2*2, 2);
    }
    virtual void setPausedInputPort(t_uint16 portIdx, BOOL bIsPaused) {
      t_uint16 _xyuv_data[2];
      /* portIdx <t_uint16> marshalling */
      _xyuv_data[0] = (t_uint16)portIdx;
      /* bIsPaused <BOOL> marshalling */
      _xyuv_data[1] = (t_uint16)bIsPaused;
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 2*2, 3);
    }
    virtual void setSamplesPlayedCounter(t_uint16 portIdx, t_uint32 value) {
      t_uint16 _xyuv_data[3];
      /* portIdx <t_uint16> marshalling */
      _xyuv_data[0] = (t_uint16)portIdx;
      /* value <t_uint32> marshalling */
      _xyuv_data[1] = (t_uint16)value;
      _xyuv_data[1+1] = (t_uint16)(value >> 16);
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 3*2, 4);
    }
    virtual void setEffectGetInfoPtr(t_uint16 portIdx, t_uint16 positionId, void* ptr) {
      t_uint16 _xyuv_data[4];
      /* portIdx <t_uint16> marshalling */
      _xyuv_data[0] = (t_uint16)portIdx;
      /* positionId <t_uint16> marshalling */
      _xyuv_data[1] = (t_uint16)positionId;
      /* ptr <void*> marshalling */
      _xyuv_data[2] = (t_uint16)((unsigned int)ptr & 0xFFFFU);
      _xyuv_data[2+1] = (t_uint16)((unsigned int)ptr >> 16);
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 4*2, 5);
    }
};
static t_cm_bf_host2mpc_handle* INITstub_mixer_nmfil_wrapper_configure(t_nmf_interface_desc* itf) {
  mixer_nmfil_wrapper_configure *stub = new mixer_nmfil_wrapper_configure();
  if(stub == 0x0)
    return 0x0;
  *(NMF::InterfaceReference*)itf = stub;
  return &stub->host2mpcId;
}

static t_nmf_stub_function mixer_cpp_stub_functions[] = {
  {"mixer.nmfil.wrapper.configure", &INITstub_mixer_nmfil_wrapper_configure},
};

/*const */t_nmf_stub_register mixer_cpp_stub_register = {
    NMF_BC_IN_CPP,
    sizeof(mixer_cpp_stub_functions)/sizeof(t_nmf_stub_function),
    mixer_cpp_stub_functions
};
