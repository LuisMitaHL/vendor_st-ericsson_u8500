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

/* Generated C++ stub for 'speech_proc.nmf.common.configure_time_align'
   Generator:tools.generator.legacy.cpp.user.stub */
#include <host/speech_proc/nmf/common/configure_time_align.hpp>

class speech_proc_nmf_common_configure_time_align: public speech_proc_nmf_common_configure_time_alignDescriptor
{
  public:
    t_cm_bf_host2mpc_handle    host2mpcId;

  public:
    virtual void setParameter(t_uint24 shared_buffer_header, t_uint24 ring_buffer, t_uint24 heap, t_uint24 heap_size, t_uint16 real_time_ref, t_uint24 ring_buffer_size) {
      t_uint16 _xyuv_data[11];
      /* shared_buffer_header <t_uint24> marshalling */
      _xyuv_data[0] = (t_uint16)shared_buffer_header;
      _xyuv_data[0+1] = (t_uint16)(shared_buffer_header >> 16);
      /* ring_buffer <t_uint24> marshalling */
      _xyuv_data[2] = (t_uint16)ring_buffer;
      _xyuv_data[2+1] = (t_uint16)(ring_buffer >> 16);
      /* heap <t_uint24> marshalling */
      _xyuv_data[4] = (t_uint16)heap;
      _xyuv_data[4+1] = (t_uint16)(heap >> 16);
      /* heap_size <t_uint24> marshalling */
      _xyuv_data[6] = (t_uint16)heap_size;
      _xyuv_data[6+1] = (t_uint16)(heap_size >> 16);
      /* real_time_ref <t_uint16> marshalling */
      _xyuv_data[8] = (t_uint16)real_time_ref;
      /* ring_buffer_size <t_uint24> marshalling */
      _xyuv_data[9] = (t_uint16)ring_buffer_size;
      _xyuv_data[9+1] = (t_uint16)(ring_buffer_size >> 16);
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 11*2, 0);
    }
    virtual void setConfig(t_uint16 freq, t_uint16 nb_channel) {
      t_uint16 _xyuv_data[2];
      /* freq <t_uint16> marshalling */
      _xyuv_data[0] = (t_uint16)freq;
      /* nb_channel <t_uint16> marshalling */
      _xyuv_data[1] = (t_uint16)nb_channel;
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 2*2, 1);
    }
    virtual void setConfigRef(t_uint16 in_freq, t_uint16 out_freq, t_uint16 nb_channel_ref) {
      t_uint16 _xyuv_data[3];
      /* in_freq <t_uint16> marshalling */
      _xyuv_data[0] = (t_uint16)in_freq;
      /* out_freq <t_uint16> marshalling */
      _xyuv_data[1] = (t_uint16)out_freq;
      /* nb_channel_ref <t_uint16> marshalling */
      _xyuv_data[2] = (t_uint16)nb_channel_ref;
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 3*2, 2);
    }
};
static t_cm_bf_host2mpc_handle* INITstub_speech_proc_nmf_common_configure_time_align(t_nmf_interface_desc* itf) {
  speech_proc_nmf_common_configure_time_align *stub = new speech_proc_nmf_common_configure_time_align();
  if(stub == 0x0)
    return 0x0;
  *(NMF::InterfaceReference*)itf = stub;
  return &stub->host2mpcId;
}

/* Generated C++ stub for 'speech_proc.nmf.mpc.dsp_port.configure'
   Generator:tools.generator.legacy.cpp.user.stub */
#include <host/speech_proc/nmf/mpc/dsp_port/configure.hpp>

class speech_proc_nmf_mpc_dsp_port_configure: public speech_proc_nmf_mpc_dsp_port_configureDescriptor
{
  public:
    t_cm_bf_host2mpc_handle    host2mpcId;

  public:
    virtual void setParameter(t_uint24 shared_buffer_header) {
      t_uint16 _xyuv_data[2];
      /* shared_buffer_header <t_uint24> marshalling */
      _xyuv_data[0] = (t_uint16)shared_buffer_header;
      _xyuv_data[0+1] = (t_uint16)(shared_buffer_header >> 16);
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 2*2, 0);
    }
};
static t_cm_bf_host2mpc_handle* INITstub_speech_proc_nmf_mpc_dsp_port_configure(t_nmf_interface_desc* itf) {
  speech_proc_nmf_mpc_dsp_port_configure *stub = new speech_proc_nmf_mpc_dsp_port_configure();
  if(stub == 0x0)
    return 0x0;
  *(NMF::InterfaceReference*)itf = stub;
  return &stub->host2mpcId;
}

/* Generated C++ stub for 'speech_proc.nmf.common.hybrid_emptythisbuffer'
   Generator:tools.generator.legacy.cpp.user.stub */
#include <host/speech_proc/nmf/common/hybrid_emptythisbuffer.hpp>

class speech_proc_nmf_common_hybrid_emptythisbuffer: public speech_proc_nmf_common_hybrid_emptythisbufferDescriptor
{
  public:
    t_cm_bf_host2mpc_handle    host2mpcId;

  public:
    virtual void emptyThisBuffer(t_uint32 buffer) {
      t_uint16 _xyuv_data[2];
      /* buffer <t_uint32> marshalling */
      _xyuv_data[0] = (t_uint16)buffer;
      _xyuv_data[0+1] = (t_uint16)(buffer >> 16);
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 2*2, 0);
    }
};
static t_cm_bf_host2mpc_handle* INITstub_speech_proc_nmf_common_hybrid_emptythisbuffer(t_nmf_interface_desc* itf) {
  speech_proc_nmf_common_hybrid_emptythisbuffer *stub = new speech_proc_nmf_common_hybrid_emptythisbuffer();
  if(stub == 0x0)
    return 0x0;
  *(NMF::InterfaceReference*)itf = stub;
  return &stub->host2mpcId;
}

/* Generated C++ stub for 'speech_proc.nmf.common.hybrid_fillthisbuffer'
   Generator:tools.generator.legacy.cpp.user.stub */
#include <host/speech_proc/nmf/common/hybrid_fillthisbuffer.hpp>

class speech_proc_nmf_common_hybrid_fillthisbuffer: public speech_proc_nmf_common_hybrid_fillthisbufferDescriptor
{
  public:
    t_cm_bf_host2mpc_handle    host2mpcId;

  public:
    virtual void fillThisBuffer(t_uint32 buffer) {
      t_uint16 _xyuv_data[2];
      /* buffer <t_uint32> marshalling */
      _xyuv_data[0] = (t_uint16)buffer;
      _xyuv_data[0+1] = (t_uint16)(buffer >> 16);
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 2*2, 0);
    }
};
static t_cm_bf_host2mpc_handle* INITstub_speech_proc_nmf_common_hybrid_fillthisbuffer(t_nmf_interface_desc* itf) {
  speech_proc_nmf_common_hybrid_fillthisbuffer *stub = new speech_proc_nmf_common_hybrid_fillthisbuffer();
  if(stub == 0x0)
    return 0x0;
  *(NMF::InterfaceReference*)itf = stub;
  return &stub->host2mpcId;
}

/* Generated C++ stub for 'hst.common.pcmsettings'
   Generator:tools.generator.legacy.cpp.user.stub */
#include <host/hst/common/pcmsettings.hpp>

class hst_common_pcmsettings: public hst_common_pcmsettingsDescriptor
{
  public:
    t_cm_bf_host2mpc_handle    host2mpcId;

  public:
    virtual void newFormat(t_sample_freq sample_freq, t_uint16 chans_nb, t_uint16 sample_size) {
      t_uint16 _xyuv_data[4];
      /* sample_freq <t_sample_freq> marshalling */
      _xyuv_data[0] = (t_uint16)((unsigned int)sample_freq & 0xFFFFU);
      _xyuv_data[0+1] = (t_uint16)((unsigned int)sample_freq >> 16);
      /* chans_nb <t_uint16> marshalling */
      _xyuv_data[2] = (t_uint16)chans_nb;
      /* sample_size <t_uint16> marshalling */
      _xyuv_data[3] = (t_uint16)sample_size;
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 4*2, 0);
    }
};
static t_cm_bf_host2mpc_handle* INITstub_hst_common_pcmsettings(t_nmf_interface_desc* itf) {
  hst_common_pcmsettings *stub = new hst_common_pcmsettings();
  if(stub == 0x0)
    return 0x0;
  *(NMF::InterfaceReference*)itf = stub;
  return &stub->host2mpcId;
}

static t_nmf_stub_function speech_proc_cpp_stub_functions[] = {
  {"speech_proc.nmf.common.configure_time_align", &INITstub_speech_proc_nmf_common_configure_time_align},
  {"speech_proc.nmf.mpc.dsp_port.configure", &INITstub_speech_proc_nmf_mpc_dsp_port_configure},
  {"speech_proc.nmf.common.hybrid_emptythisbuffer", &INITstub_speech_proc_nmf_common_hybrid_emptythisbuffer},
  {"speech_proc.nmf.common.hybrid_fillthisbuffer", &INITstub_speech_proc_nmf_common_hybrid_fillthisbuffer},
  {"hst.common.pcmsettings", &INITstub_hst_common_pcmsettings},
};

/*const */t_nmf_stub_register speech_proc_cpp_stub_register = {
    NMF_BC_IN_CPP,
    sizeof(speech_proc_cpp_stub_functions)/sizeof(t_nmf_stub_function),
    speech_proc_cpp_stub_functions
};
