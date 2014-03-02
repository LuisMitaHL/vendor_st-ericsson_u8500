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

/* Generated C++ stub for 'dmaout.low_power'
   Generator:tools.generator.legacy.cpp.user.stub */
#include <host/dmaout/low_power.hpp>

class dmaout_low_power: public dmaout_low_powerDescriptor
{
  public:
    t_cm_bf_host2mpc_handle    host2mpcId;

  public:
    virtual void startLowPowerTransition(void* buffer_ring, t_uint16 buffer_ring_size) {
      t_uint16 _xyuv_data[3];
      /* buffer_ring <void*> marshalling */
      _xyuv_data[0] = (t_uint16)((unsigned int)buffer_ring & 0xFFFFU);
      _xyuv_data[0+1] = (t_uint16)((unsigned int)buffer_ring >> 16);
      /* buffer_ring_size <t_uint16> marshalling */
      _xyuv_data[2] = (t_uint16)buffer_ring_size;
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 3*2, 0);
    }
    virtual void startNormalModeTransition(void) {
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, (t_uint16*)0x0, 0, 1);
    }
    virtual void mutePort(t_uint16 port_idx) {
      t_uint16 _xyuv_data[1];
      /* port_idx <t_uint16> marshalling */
      _xyuv_data[0] = (t_uint16)port_idx;
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 1*2, 2);
    }
};
static t_cm_bf_host2mpc_handle* INITstub_dmaout_low_power(t_nmf_interface_desc* itf) {
  dmaout_low_power *stub = new dmaout_low_power();
  if(stub == 0x0)
    return 0x0;
  *(NMF::InterfaceReference*)itf = stub;
  return &stub->host2mpcId;
}

/* Generated C++ stub for 'common.configure'
   Generator:tools.generator.legacy.cpp.user.stub */
#include <host/common/configure.hpp>

class common_configure: public common_configureDescriptor
{
  public:
    t_cm_bf_host2mpc_handle    host2mpcId;

  public:
    virtual void setParameter(DmaConfig_t config, t_uint16 target) {
      t_uint16 _xyuv_data[11];
      /* config <DmaConfig_t> marshalling */
      _xyuv_data[0+0] = (t_uint16)((unsigned int)config.buffer & 0xFFFFU);
      _xyuv_data[0+0+1] = (t_uint16)((unsigned int)config.buffer >> 16);
      _xyuv_data[0+2] = (t_uint16)config.buffer_size;
      _xyuv_data[0+3] = (t_uint16)((unsigned int)config.buffer_ring & 0xFFFFU);
      _xyuv_data[0+3+1] = (t_uint16)((unsigned int)config.buffer_ring >> 16);
      _xyuv_data[0+5] = (t_uint16)config.samplerate;
      _xyuv_data[0+6] = (t_uint16)((unsigned int)config.sample_count_buffer & 0xFFFFU);
      _xyuv_data[0+6+1] = (t_uint16)((unsigned int)config.sample_count_buffer >> 16);
      _xyuv_data[0+8] = (t_uint16)config.sample_count_buffer_size;
      _xyuv_data[0+9] = (t_uint16)config.nb_msp_channel_enabled;
      /* target <t_uint16> marshalling */
      _xyuv_data[10] = (t_uint16)target;
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 11*2, 0);
    }
};
static t_cm_bf_host2mpc_handle* INITstub_common_configure(t_nmf_interface_desc* itf) {
  common_configure *stub = new common_configure();
  if(stub == 0x0)
    return 0x0;
  *(NMF::InterfaceReference*)itf = stub;
  return &stub->host2mpcId;
}

/* Generated C++ stub for 'msp_dma_ctrl_ab8500.configure'
   Generator:tools.generator.legacy.cpp.user.stub */
#include <host/msp_dma_ctrl_ab8500/configure.hpp>

class msp_dma_ctrl_ab8500_configure: public msp_dma_ctrl_ab8500_configureDescriptor
{
  public:
    t_cm_bf_host2mpc_handle    host2mpcId;

  public:
    virtual void setParameter(void* bufferTx, t_uint16 bufferSizeInSample, t_uint16 nbMspChannelEnabled, t_uint16 target) {
      t_uint16 _xyuv_data[5];
      /* bufferTx <void*> marshalling */
      _xyuv_data[0] = (t_uint16)((unsigned int)bufferTx & 0xFFFFU);
      _xyuv_data[0+1] = (t_uint16)((unsigned int)bufferTx >> 16);
      /* bufferSizeInSample <t_uint16> marshalling */
      _xyuv_data[2] = (t_uint16)bufferSizeInSample;
      /* nbMspChannelEnabled <t_uint16> marshalling */
      _xyuv_data[3] = (t_uint16)nbMspChannelEnabled;
      /* target <t_uint16> marshalling */
      _xyuv_data[4] = (t_uint16)target;
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 5*2, 0);
    }
};
static t_cm_bf_host2mpc_handle* INITstub_msp_dma_ctrl_ab8500_configure(t_nmf_interface_desc* itf) {
  msp_dma_ctrl_ab8500_configure *stub = new msp_dma_ctrl_ab8500_configure();
  if(stub == 0x0)
    return 0x0;
  *(NMF::InterfaceReference*)itf = stub;
  return &stub->host2mpcId;
}

static t_nmf_stub_function audiocodec_cpp_stub_functions[] = {
  {"dmaout.low_power", &INITstub_dmaout_low_power},
  {"common.configure", &INITstub_common_configure},
  {"msp_dma_ctrl_ab8500.configure", &INITstub_msp_dma_ctrl_ab8500_configure},
};

/*const */t_nmf_stub_register audiocodec_cpp_stub_register = {
    NMF_BC_IN_CPP,
    sizeof(audiocodec_cpp_stub_functions)/sizeof(t_nmf_stub_function),
    audiocodec_cpp_stub_functions
};
