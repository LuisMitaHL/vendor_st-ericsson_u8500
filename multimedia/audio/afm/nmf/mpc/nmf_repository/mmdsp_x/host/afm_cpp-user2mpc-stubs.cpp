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

/* Generated C++ stub for 'pcmsettings'
   Generator:tools.generator.legacy.cpp.user.stub */
#include <host/pcmsettings.hpp>

class pcmsettings: public pcmsettingsDescriptor
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
static t_cm_bf_host2mpc_handle* INITstub_pcmsettings(t_nmf_interface_desc* itf) {
  pcmsettings *stub = new pcmsettings();
  if(stub == 0x0)
    return 0x0;
  *(NMF::InterfaceReference*)itf = stub;
  return &stub->host2mpcId;
}

/* Generated C++ stub for 'bindings.pcmadapter.configure'
   Generator:tools.generator.legacy.cpp.user.stub */
#include <host/bindings/pcmadapter/configure.hpp>

class bindings_pcmadapter_configure: public bindings_pcmadapter_configureDescriptor
{
  public:
    t_cm_bf_host2mpc_handle    host2mpcId;

  public:
    virtual void setParameter(PcmAdapterParam_t params) {
      t_uint16 _xyuv_data[13];
      /* params <PcmAdapterParam_t> marshalling */
      _xyuv_data[0+0] = (t_uint16)((unsigned int)params.pBuffer & 0xFFFFU);
      _xyuv_data[0+0+1] = (t_uint16)((unsigned int)params.pBuffer >> 16);
      _xyuv_data[0+2] = (t_uint16)params.nBufferSize;
      _xyuv_data[0+2+1] = (t_uint16)(params.nBufferSize >> 16);
      _xyuv_data[0+4] = (t_uint16)params.nInputBlockSize;
      _xyuv_data[0+5] = (t_uint16)params.nChansIn;
      _xyuv_data[0+6] = (t_uint16)params.nOutputBlockSize;
      _xyuv_data[0+7] = (t_uint16)params.nChansOut;
      _xyuv_data[0+8] = (t_uint16)params.nNbBuffersIn;
      _xyuv_data[0+9] = (t_uint16)params.nNbBuffersOut;
      _xyuv_data[0+10] = (t_uint16)params.nBitPerSampleIn;
      _xyuv_data[0+11] = (t_uint16)params.nBitPerSampleOut;
      _xyuv_data[0+12] = (t_uint16)params.nSampleRate;
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 13*2, 0);
    }
};
static t_cm_bf_host2mpc_handle* INITstub_bindings_pcmadapter_configure(t_nmf_interface_desc* itf) {
  bindings_pcmadapter_configure *stub = new bindings_pcmadapter_configure();
  if(stub == 0x0)
    return 0x0;
  *(NMF::InterfaceReference*)itf = stub;
  return &stub->host2mpcId;
}

/* Generated C++ stub for 'bindings.shm.configure'
   Generator:tools.generator.legacy.cpp.user.stub */
#include <host/bindings/shm/configure.hpp>

class bindings_shm_configure: public bindings_shm_configureDescriptor
{
  public:
    t_cm_bf_host2mpc_handle    host2mpcId;

  public:
    virtual void setParameter(ShmConfig_t config) {
      t_uint16 _xyuv_data[3];
      /* config <ShmConfig_t> marshalling */
      _xyuv_data[0+0] = (t_uint16)config.nb_buffer;
      _xyuv_data[0+1] = (t_uint16)((unsigned int)config.swap_bytes & 0xFFFFU);
      _xyuv_data[0+1+1] = (t_uint16)((unsigned int)config.swap_bytes >> 16);
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 3*2, 0);
    }
};
static t_cm_bf_host2mpc_handle* INITstub_bindings_shm_configure(t_nmf_interface_desc* itf) {
  bindings_shm_configure *stub = new bindings_shm_configure();
  if(stub == 0x0)
    return 0x0;
  *(NMF::InterfaceReference*)itf = stub;
  return &stub->host2mpcId;
}

/* Generated C++ stub for 'bindings.shmpcm.configure'
   Generator:tools.generator.legacy.cpp.user.stub */
#include <host/bindings/shmpcm/configure.hpp>

class bindings_shmpcm_configure: public bindings_shmpcm_configureDescriptor
{
  public:
    t_cm_bf_host2mpc_handle    host2mpcId;

  public:
    virtual void setParameter(ShmPcmConfig_t config, void* buffer, t_uint16 output_blocksize) {
      t_uint16 _xyuv_data[14];
      /* config <ShmPcmConfig_t> marshalling */
      _xyuv_data[0+0] = (t_uint16)config.nb_buffer;
      _xyuv_data[0+1] = (t_uint16)((unsigned int)config.swap_bytes & 0xFFFFU);
      _xyuv_data[0+1+1] = (t_uint16)((unsigned int)config.swap_bytes >> 16);
      _xyuv_data[0+3] = (t_uint16)config.bitsPerSampleIn;
      _xyuv_data[0+4] = (t_uint16)config.bitsPerSampleOut;
      _xyuv_data[0+5] = (t_uint16)config.channelsIn;
      _xyuv_data[0+6] = (t_uint16)config.channelsOut;
      _xyuv_data[0+7] = (t_uint16)((unsigned int)config.sampleFreq & 0xFFFFU);
      _xyuv_data[0+7+1] = (t_uint16)((unsigned int)config.sampleFreq >> 16);
      _xyuv_data[0+9] = (t_uint16)config.isSynchronized;
      _xyuv_data[0+10] = (t_uint16)config.hostMpcSyncEnabled;
      /* buffer <void*> marshalling */
      _xyuv_data[11] = (t_uint16)((unsigned int)buffer & 0xFFFFU);
      _xyuv_data[11+1] = (t_uint16)((unsigned int)buffer >> 16);
      /* output_blocksize <t_uint16> marshalling */
      _xyuv_data[13] = (t_uint16)output_blocksize;
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 14*2, 0);
    }
};
static t_cm_bf_host2mpc_handle* INITstub_bindings_shmpcm_configure(t_nmf_interface_desc* itf) {
  bindings_shmpcm_configure *stub = new bindings_shmpcm_configure();
  if(stub == 0x0)
    return 0x0;
  *(NMF::InterfaceReference*)itf = stub;
  return &stub->host2mpcId;
}

/* Generated C++ stub for 'pcmprocessings.wrapper.configure'
   Generator:tools.generator.legacy.cpp.user.stub */
#include <host/pcmprocessings/wrapper/configure.hpp>

class pcmprocessings_wrapper_configure: public pcmprocessings_wrapper_configureDescriptor
{
  public:
    t_cm_bf_host2mpc_handle    host2mpcId;

  public:
    virtual void setParameter(t_pcmprocessing_config config) {
      t_uint16 _xyuv_data[12];
      /* config <t_pcmprocessing_config> marshalling */
      _xyuv_data[0+0] = (t_uint16)((unsigned int)config.processingMode & 0xFFFFU);
      _xyuv_data[0+0+1] = (t_uint16)((unsigned int)config.processingMode >> 16);
      _xyuv_data[0+2+0] = (t_uint16)config.effectConfig.block_size;
      _xyuv_data[0+2+0+1] = (t_uint16)(config.effectConfig.block_size >> 16);
      _xyuv_data[0+2+2+0] = (t_uint16)((unsigned int)config.effectConfig.infmt.freq & 0xFFFFU);
      _xyuv_data[0+2+2+0+1] = (t_uint16)((unsigned int)config.effectConfig.infmt.freq >> 16);
      _xyuv_data[0+2+2+2] = (t_uint16)config.effectConfig.infmt.nof_channels;
      _xyuv_data[0+2+2+3] = (t_uint16)config.effectConfig.infmt.nof_bits_per_sample;
      _xyuv_data[0+2+6+0] = (t_uint16)((unsigned int)config.effectConfig.outfmt.freq & 0xFFFFU);
      _xyuv_data[0+2+6+0+1] = (t_uint16)((unsigned int)config.effectConfig.outfmt.freq >> 16);
      _xyuv_data[0+2+6+2] = (t_uint16)config.effectConfig.outfmt.nof_channels;
      _xyuv_data[0+2+6+3] = (t_uint16)config.effectConfig.outfmt.nof_bits_per_sample;
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 12*2, 0);
    }
};
static t_cm_bf_host2mpc_handle* INITstub_pcmprocessings_wrapper_configure(t_nmf_interface_desc* itf) {
  pcmprocessings_wrapper_configure *stub = new pcmprocessings_wrapper_configure();
  if(stub == 0x0)
    return 0x0;
  *(NMF::InterfaceReference*)itf = stub;
  return &stub->host2mpcId;
}

/* Generated C++ stub for 'pcmprocessings.mips.configure'
   Generator:tools.generator.legacy.cpp.user.stub */
#include <host/pcmprocessings/mips/configure.hpp>

class pcmprocessings_mips_configure: public pcmprocessings_mips_configureDescriptor
{
  public:
    t_cm_bf_host2mpc_handle    host2mpcId;

  public:
    virtual void setConfig(Mips_configure_t Mips_configure) {
      t_uint16 _xyuv_data[4];
      /* Mips_configure <Mips_configure_t> marshalling */
      _xyuv_data[0+0] = (t_uint16)Mips_configure.delay_us;
      _xyuv_data[0+0+1] = (t_uint16)(Mips_configure.delay_us >> 16);
      _xyuv_data[0+2] = (t_uint16)Mips_configure.cycles_per_second;
      _xyuv_data[0+2+1] = (t_uint16)(Mips_configure.cycles_per_second >> 16);
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 4*2, 0);
    }
};
static t_cm_bf_host2mpc_handle* INITstub_pcmprocessings_mips_configure(t_nmf_interface_desc* itf) {
  pcmprocessings_mips_configure *stub = new pcmprocessings_mips_configure();
  if(stub == 0x0)
    return 0x0;
  *(NMF::InterfaceReference*)itf = stub;
  return &stub->host2mpcId;
}

/* Generated C++ stub for 'malloc.setheap'
   Generator:tools.generator.legacy.cpp.user.stub */
#include <host/malloc/setheap.hpp>

class malloc_setheap: public malloc_setheapDescriptor
{
  public:
    t_cm_bf_host2mpc_handle    host2mpcId;

  public:
    virtual void setHeap(t_memory_bank memory_bank, void* buf, t_uint32 size) {
      t_uint16 _xyuv_data[6];
      /* memory_bank <t_memory_bank> marshalling */
      _xyuv_data[0] = (t_uint16)((unsigned int)memory_bank & 0xFFFFU);
      _xyuv_data[0+1] = (t_uint16)((unsigned int)memory_bank >> 16);
      /* buf <void*> marshalling */
      _xyuv_data[2] = (t_uint16)((unsigned int)buf & 0xFFFFU);
      _xyuv_data[2+1] = (t_uint16)((unsigned int)buf >> 16);
      /* size <t_uint32> marshalling */
      _xyuv_data[4] = (t_uint16)size;
      _xyuv_data[4+1] = (t_uint16)(size >> 16);
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 6*2, 0);
    }
};
static t_cm_bf_host2mpc_handle* INITstub_malloc_setheap(t_nmf_interface_desc* itf) {
  malloc_setheap *stub = new malloc_setheap();
  if(stub == 0x0)
    return 0x0;
  *(NMF::InterfaceReference*)itf = stub;
  return &stub->host2mpcId;
}

/* Generated C++ stub for 'misc.controller.configure'
   Generator:tools.generator.legacy.cpp.user.stub */
#include <host/misc/controller/configure.hpp>

class misc_controller_configure: public misc_controller_configureDescriptor
{
  public:
    t_cm_bf_host2mpc_handle    host2mpcId;

  public:
    virtual void setParameter(t_uint16 portDirections) {
      t_uint16 _xyuv_data[1];
      /* portDirections <t_uint16> marshalling */
      _xyuv_data[0] = (t_uint16)portDirections;
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 1*2, 0);
    }
};
static t_cm_bf_host2mpc_handle* INITstub_misc_controller_configure(t_nmf_interface_desc* itf) {
  misc_controller_configure *stub = new misc_controller_configure();
  if(stub == 0x0)
    return 0x0;
  *(NMF::InterfaceReference*)itf = stub;
  return &stub->host2mpcId;
}

/* Generated C++ stub for 'setmemory'
   Generator:tools.generator.legacy.cpp.user.stub */
#include <host/setmemory.hpp>

class setmemory: public setmemoryDescriptor
{
  public:
    t_cm_bf_host2mpc_handle    host2mpcId;

  public:
    virtual void setMemory(void* buf, t_uint16 size) {
      t_uint16 _xyuv_data[3];
      /* buf <void*> marshalling */
      _xyuv_data[0] = (t_uint16)((unsigned int)buf & 0xFFFFU);
      _xyuv_data[0+1] = (t_uint16)((unsigned int)buf >> 16);
      /* size <t_uint16> marshalling */
      _xyuv_data[2] = (t_uint16)size;
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 3*2, 0);
    }
};
static t_cm_bf_host2mpc_handle* INITstub_setmemory(t_nmf_interface_desc* itf) {
  setmemory *stub = new setmemory();
  if(stub == 0x0)
    return 0x0;
  *(NMF::InterfaceReference*)itf = stub;
  return &stub->host2mpcId;
}

/* Generated C++ stub for 'misc.samplesplayed'
   Generator:tools.generator.legacy.cpp.user.stub */
#include <host/misc/samplesplayed.hpp>

class misc_samplesplayed: public misc_samplesplayedDescriptor
{
  public:
    t_cm_bf_host2mpc_handle    host2mpcId;

  public:
    virtual void SetSamplesPlayedAddr(void* Addr) {
      t_uint16 _xyuv_data[2];
      /* Addr <void*> marshalling */
      _xyuv_data[0] = (t_uint16)((unsigned int)Addr & 0xFFFFU);
      _xyuv_data[0+1] = (t_uint16)((unsigned int)Addr >> 16);
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 2*2, 0);
    }
};
static t_cm_bf_host2mpc_handle* INITstub_misc_samplesplayed(t_nmf_interface_desc* itf) {
  misc_samplesplayed *stub = new misc_samplesplayed();
  if(stub == 0x0)
    return 0x0;
  *(NMF::InterfaceReference*)itf = stub;
  return &stub->host2mpcId;
}

/* Generated C++ stub for 'decoders.streamed.configure'
   Generator:tools.generator.legacy.cpp.user.stub */
#include <host/decoders/streamed/configure.hpp>

class decoders_streamed_configure: public decoders_streamed_configureDescriptor
{
  public:
    t_cm_bf_host2mpc_handle    host2mpcId;

  public:
    virtual void setParameter(void* buffer_in, t_uint16 size_in, void* buffer_out, t_uint16 size_out, t_uint16 payLoad) {
      t_uint16 _xyuv_data[7];
      /* buffer_in <void*> marshalling */
      _xyuv_data[0] = (t_uint16)((unsigned int)buffer_in & 0xFFFFU);
      _xyuv_data[0+1] = (t_uint16)((unsigned int)buffer_in >> 16);
      /* size_in <t_uint16> marshalling */
      _xyuv_data[2] = (t_uint16)size_in;
      /* buffer_out <void*> marshalling */
      _xyuv_data[3] = (t_uint16)((unsigned int)buffer_out & 0xFFFFU);
      _xyuv_data[3+1] = (t_uint16)((unsigned int)buffer_out >> 16);
      /* size_out <t_uint16> marshalling */
      _xyuv_data[5] = (t_uint16)size_out;
      /* payLoad <t_uint16> marshalling */
      _xyuv_data[6] = (t_uint16)payLoad;
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 7*2, 0);
    }
};
static t_cm_bf_host2mpc_handle* INITstub_decoders_streamed_configure(t_nmf_interface_desc* itf) {
  decoders_streamed_configure *stub = new decoders_streamed_configure();
  if(stub == 0x0)
    return 0x0;
  *(NMF::InterfaceReference*)itf = stub;
  return &stub->host2mpcId;
}

/* Generated C++ stub for 'encoders.streamed.configure'
   Generator:tools.generator.legacy.cpp.user.stub */
#include <host/encoders/streamed/configure.hpp>

class encoders_streamed_configure: public encoders_streamed_configureDescriptor
{
  public:
    t_cm_bf_host2mpc_handle    host2mpcId;

  public:
    virtual void setParameter(void* buffer_in, t_uint16 size_in, void* buffer_out, t_uint16 size_out) {
      t_uint16 _xyuv_data[6];
      /* buffer_in <void*> marshalling */
      _xyuv_data[0] = (t_uint16)((unsigned int)buffer_in & 0xFFFFU);
      _xyuv_data[0+1] = (t_uint16)((unsigned int)buffer_in >> 16);
      /* size_in <t_uint16> marshalling */
      _xyuv_data[2] = (t_uint16)size_in;
      /* buffer_out <void*> marshalling */
      _xyuv_data[3] = (t_uint16)((unsigned int)buffer_out & 0xFFFFU);
      _xyuv_data[3+1] = (t_uint16)((unsigned int)buffer_out >> 16);
      /* size_out <t_uint16> marshalling */
      _xyuv_data[5] = (t_uint16)size_out;
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 6*2, 0);
    }
};
static t_cm_bf_host2mpc_handle* INITstub_encoders_streamed_configure(t_nmf_interface_desc* itf) {
  encoders_streamed_configure *stub = new encoders_streamed_configure();
  if(stub == 0x0)
    return 0x0;
  *(NMF::InterfaceReference*)itf = stub;
  return &stub->host2mpcId;
}

/* Generated C++ stub for 'pcmdump.configure'
   Generator:tools.generator.legacy.cpp.user.stub */
#include <host/pcmdump/configure.hpp>

class pcmdump_configure: public pcmdump_configureDescriptor
{
  public:
    t_cm_bf_host2mpc_handle    host2mpcId;

  public:
    virtual void configure_pcmprobe(PcmDumpConfigure_t sConfigureInfo) {
      t_uint16 _xyuv_data[8];
      /* sConfigureInfo <PcmDumpConfigure_t> marshalling */
      _xyuv_data[0+0] = (t_uint16)sConfigureInfo.nmf_port_idx;
      _xyuv_data[0+1] = (t_uint16)sConfigureInfo.omx_port_idx;
      _xyuv_data[0+2] = (t_uint16)sConfigureInfo.effect_position_idx;
      _xyuv_data[0+3] = (t_uint16)sConfigureInfo.enable;
      _xyuv_data[0+4] = (t_uint16)((unsigned int)sConfigureInfo.buffer & 0xFFFFU);
      _xyuv_data[0+4+1] = (t_uint16)((unsigned int)sConfigureInfo.buffer >> 16);
      _xyuv_data[0+6] = (t_uint16)sConfigureInfo.buffer_size;
      _xyuv_data[0+6+1] = (t_uint16)(sConfigureInfo.buffer_size >> 16);
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 8*2, 0);
    }
};
static t_cm_bf_host2mpc_handle* INITstub_pcmdump_configure(t_nmf_interface_desc* itf) {
  pcmdump_configure *stub = new pcmdump_configure();
  if(stub == 0x0)
    return 0x0;
  *(NMF::InterfaceReference*)itf = stub;
  return &stub->host2mpcId;
}

static t_nmf_stub_function afm_cpp_stub_functions[] = {
  {"pcmsettings", &INITstub_pcmsettings},
  {"bindings.pcmadapter.configure", &INITstub_bindings_pcmadapter_configure},
  {"bindings.shm.configure", &INITstub_bindings_shm_configure},
  {"bindings.shmpcm.configure", &INITstub_bindings_shmpcm_configure},
  {"pcmprocessings.wrapper.configure", &INITstub_pcmprocessings_wrapper_configure},
  {"pcmprocessings.mips.configure", &INITstub_pcmprocessings_mips_configure},
  {"malloc.setheap", &INITstub_malloc_setheap},
  {"misc.controller.configure", &INITstub_misc_controller_configure},
  {"setmemory", &INITstub_setmemory},
  {"misc.samplesplayed", &INITstub_misc_samplesplayed},
  {"decoders.streamed.configure", &INITstub_decoders_streamed_configure},
  {"encoders.streamed.configure", &INITstub_encoders_streamed_configure},
  {"pcmdump.configure", &INITstub_pcmdump_configure},
};

/*const */t_nmf_stub_register afm_cpp_stub_register = {
    NMF_BC_IN_CPP,
    sizeof(afm_cpp_stub_functions)/sizeof(t_nmf_stub_function),
    afm_cpp_stub_functions
};
