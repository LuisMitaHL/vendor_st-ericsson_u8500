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

/* 'mpeg4enc.mpc.api.ddep' interface */
#if !defined(mpeg4enc_mpc_api_ddep_IDL)
#define mpeg4enc_mpc_api_ddep_IDL

#include <cpp.hpp>
#include <host/vfm_common.idt.h>
#include <host/t1xhv_common.idt.h>
#include <host/t1xhv_vec_mpeg4.idt.h>


class mpeg4enc_mpc_api_ddepDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void setConfig(t_uint16 channelId) = 0;
    virtual void setParameter(t_uint16 channelId, t_t1xhv_algo_id algoId, ts_ddep_buffer_descriptor ddep_vec_mpeg_param_desc) = 0;
    virtual void setNeeds(t_uint16 channelId, ts_ddep_buffer_descriptor bbm_desc, ts_ddep_buffer_descriptor debugBuffer_desc) = 0;
    virtual void setCropForStab(t_uint32 crop_left_offset, t_uint32 crop_top_offset, t_uint32 overscan_width, t_uint32 overscan_height) = 0;
    virtual void disableFWCodeexection(t_bool value) = 0;
};

class Impeg4enc_mpc_api_ddep: public NMF::InterfaceReference {
  public:
    Impeg4enc_mpc_api_ddep(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void setConfig(t_uint16 channelId) {
      ((mpeg4enc_mpc_api_ddepDescriptor*)reference)->setConfig(channelId);
    }
    void setParameter(t_uint16 channelId, t_t1xhv_algo_id algoId, ts_ddep_buffer_descriptor ddep_vec_mpeg_param_desc) {
      ((mpeg4enc_mpc_api_ddepDescriptor*)reference)->setParameter(channelId, algoId, ddep_vec_mpeg_param_desc);
    }
    void setNeeds(t_uint16 channelId, ts_ddep_buffer_descriptor bbm_desc, ts_ddep_buffer_descriptor debugBuffer_desc) {
      ((mpeg4enc_mpc_api_ddepDescriptor*)reference)->setNeeds(channelId, bbm_desc, debugBuffer_desc);
    }
    void setCropForStab(t_uint32 crop_left_offset, t_uint32 crop_top_offset, t_uint32 overscan_width, t_uint32 overscan_height) {
      ((mpeg4enc_mpc_api_ddepDescriptor*)reference)->setCropForStab(crop_left_offset, crop_top_offset, overscan_width, overscan_height);
    }
    void disableFWCodeexection(t_bool value) {
      ((mpeg4enc_mpc_api_ddepDescriptor*)reference)->disableFWCodeexection(value);
    }
};

#endif
