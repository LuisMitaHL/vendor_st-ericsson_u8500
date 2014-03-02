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

/* Generated C++ stub for 'amr.nmfil.decoder.configure'
   Generator:tools.generator.legacy.cpp.user.stub */
#include <host/amr/nmfil/decoder/configure.hpp>

class amr_nmfil_decoder_configure: public amr_nmfil_decoder_configureDescriptor
{
  public:
    t_cm_bf_host2mpc_handle    host2mpcId;

  public:
    virtual void setParameter(AmrDecParams_t params) {
      t_uint16 _xyuv_data[4];
      /* params <AmrDecParams_t> marshalling */
      _xyuv_data[0+0] = (t_uint16)params.bNoHeader;
      _xyuv_data[0+1] = (t_uint16)params.bErrorConcealment;
      _xyuv_data[0+2] = (t_uint16)((unsigned int)params.memory_preset & 0xFFFFU);
      _xyuv_data[0+2+1] = (t_uint16)((unsigned int)params.memory_preset >> 16);
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 4*2, 0);
    }
    virtual void setConfig(AmrDecConfig_t params) {
      t_uint16 _xyuv_data[3];
      /* params <AmrDecConfig_t> marshalling */
      _xyuv_data[0+0] = (t_uint16)((unsigned int)params.ePayloadFormat & 0xFFFFU);
      _xyuv_data[0+0+1] = (t_uint16)((unsigned int)params.ePayloadFormat >> 16);
      _xyuv_data[0+2] = (t_uint16)params.bEfr_on;
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 3*2, 1);
    }
};
static t_cm_bf_host2mpc_handle* INITstub_amr_nmfil_decoder_configure(t_nmf_interface_desc* itf) {
  amr_nmfil_decoder_configure *stub = new amr_nmfil_decoder_configure();
  if(stub == 0x0)
    return 0x0;
  *(NMF::InterfaceReference*)itf = stub;
  return &stub->host2mpcId;
}

/* Generated C++ stub for 'amr.nmfil.encoder.configure'
   Generator:tools.generator.legacy.cpp.user.stub */
#include <host/amr/nmfil/encoder/configure.hpp>

class amr_nmfil_encoder_configure: public amr_nmfil_encoder_configureDescriptor
{
  public:
    t_cm_bf_host2mpc_handle    host2mpcId;

  public:
    virtual void setParameter(AmrEncParam_t params) {
      t_uint16 _xyuv_data[3];
      /* params <AmrEncParam_t> marshalling */
      _xyuv_data[0+0] = (t_uint16)params.bNoHeader;
      _xyuv_data[0+1] = (t_uint16)((unsigned int)params.memory_preset & 0xFFFFU);
      _xyuv_data[0+1+1] = (t_uint16)((unsigned int)params.memory_preset >> 16);
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 3*2, 0);
    }
    virtual void setConfig(AmrEncConfig_t config) {
      t_uint16 _xyuv_data[5];
      /* config <AmrEncConfig_t> marshalling */
      _xyuv_data[0+0] = (t_uint16)config.nBitRate;
      _xyuv_data[0+1] = (t_uint16)config.bDtxEnable;
      _xyuv_data[0+2] = (t_uint16)((unsigned int)config.ePayloadFormat & 0xFFFFU);
      _xyuv_data[0+2+1] = (t_uint16)((unsigned int)config.ePayloadFormat >> 16);
      _xyuv_data[0+4] = (t_uint16)config.bEfr_on;
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 5*2, 1);
    }
};
static t_cm_bf_host2mpc_handle* INITstub_amr_nmfil_encoder_configure(t_nmf_interface_desc* itf) {
  amr_nmfil_encoder_configure *stub = new amr_nmfil_encoder_configure();
  if(stub == 0x0)
    return 0x0;
  *(NMF::InterfaceReference*)itf = stub;
  return &stub->host2mpcId;
}

static t_nmf_stub_function amr_cpp_stub_functions[] = {
  {"amr.nmfil.decoder.configure", &INITstub_amr_nmfil_decoder_configure},
  {"amr.nmfil.encoder.configure", &INITstub_amr_nmfil_encoder_configure},
};

/*const */t_nmf_stub_register amr_cpp_stub_register = {
    NMF_BC_IN_CPP,
    sizeof(amr_cpp_stub_functions)/sizeof(t_nmf_stub_function),
    amr_cpp_stub_functions
};
