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

/* 'jpegdec.mpc.api.ddep' interface */
#if !defined(jpegdec_mpc_api_ddep_IDL)
#define jpegdec_mpc_api_ddep_IDL

#include <cpp.hpp>
#include <host/vfm_common.idt.h>
#include <host/t1xhv_common.idt.h>
#include <host/t1xhv_vdc_jpeg.idt.h>


class jpegdec_mpc_api_ddepDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void setConfig(t_uint16 channelId) = 0;
    virtual void setParameter(t_uint16 channelId, t_t1xhv_algo_id algoId, ts_ddep_buffer_descriptor ddep_sdc_jpeg_param_desc) = 0;
    virtual void setNeeds(t_uint16 channelId, ts_ddep_buffer_descriptor bbm_desc, ts_ddep_buffer_descriptor debugBuffer_desc) = 0;
    virtual void disableFWCodeexection(t_bool value) = 0;
};

class Ijpegdec_mpc_api_ddep: public NMF::InterfaceReference {
  public:
    Ijpegdec_mpc_api_ddep(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void setConfig(t_uint16 channelId) {
      ((jpegdec_mpc_api_ddepDescriptor*)reference)->setConfig(channelId);
    }
    void setParameter(t_uint16 channelId, t_t1xhv_algo_id algoId, ts_ddep_buffer_descriptor ddep_sdc_jpeg_param_desc) {
      ((jpegdec_mpc_api_ddepDescriptor*)reference)->setParameter(channelId, algoId, ddep_sdc_jpeg_param_desc);
    }
    void setNeeds(t_uint16 channelId, ts_ddep_buffer_descriptor bbm_desc, ts_ddep_buffer_descriptor debugBuffer_desc) {
      ((jpegdec_mpc_api_ddepDescriptor*)reference)->setNeeds(channelId, bbm_desc, debugBuffer_desc);
    }
    void disableFWCodeexection(t_bool value) {
      ((jpegdec_mpc_api_ddepDescriptor*)reference)->disableFWCodeexection(value);
    }
};

#endif
