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

/* 'mpeg2dec.mpc.api.ddep' interface */
#if !defined(mpeg2dec_mpc_api_ddep_IDL)
#define mpeg2dec_mpc_api_ddep_IDL

#include <cpp.hpp>
#include <host/vfm_common.idt.h>
#include <host/t1xhv_common.idt.h>
#include <host/t1xhv_vdc_mpeg2.idt.h>
#include <host/omxcommand.idt.h>


class mpeg2dec_mpc_api_ddepDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void setConfig(t_uint16 channelId, t_uint8 outputPortBufferCountActual, t_uint8 errorMapFlag) = 0;
    virtual void setParameter(t_uint16 channelId, t_t1xhv_algo_id algoId, ts_ddep_buffer_descriptor ddep_vdc_mpeg2_param_desc) = 0;
    virtual void setNeeds(t_uint16 channelId, ts_ddep_buffer_descriptor bbm_desc, ts_ddep_buffer_descriptor debugBuffer_desc) = 0;
    virtual void sendCommand_sequential(OMX_COMMANDTYPE cmd, t_uword param) = 0;
    virtual void disableFWCodeexection(t_bool value) = 0;
};

class Impeg2dec_mpc_api_ddep: public NMF::InterfaceReference {
  public:
    Impeg2dec_mpc_api_ddep(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void setConfig(t_uint16 channelId, t_uint8 outputPortBufferCountActual, t_uint8 errorMapFlag) {
      ((mpeg2dec_mpc_api_ddepDescriptor*)reference)->setConfig(channelId, outputPortBufferCountActual, errorMapFlag);
    }
    void setParameter(t_uint16 channelId, t_t1xhv_algo_id algoId, ts_ddep_buffer_descriptor ddep_vdc_mpeg2_param_desc) {
      ((mpeg2dec_mpc_api_ddepDescriptor*)reference)->setParameter(channelId, algoId, ddep_vdc_mpeg2_param_desc);
    }
    void setNeeds(t_uint16 channelId, ts_ddep_buffer_descriptor bbm_desc, ts_ddep_buffer_descriptor debugBuffer_desc) {
      ((mpeg2dec_mpc_api_ddepDescriptor*)reference)->setNeeds(channelId, bbm_desc, debugBuffer_desc);
    }
    void sendCommand_sequential(OMX_COMMANDTYPE cmd, t_uword param) {
      ((mpeg2dec_mpc_api_ddepDescriptor*)reference)->sendCommand_sequential(cmd, param);
    }
    void disableFWCodeexection(t_bool value) {
      ((mpeg2dec_mpc_api_ddepDescriptor*)reference)->disableFWCodeexection(value);
    }
};

#endif
