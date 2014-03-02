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

/* 'mpeg4dec.mpc.api.ddep' interface */
#if !defined(mpeg4dec_mpc_api_ddep_IDL)
#define mpeg4dec_mpc_api_ddep_IDL

#include <cpp.hpp>
#include <host/vfm_common.idt.h>
#include <host/t1xhv_common.idt.h>
#include <host/t1xhv_vdc_mpeg4.idt.h>
#include <host/omxcommand.idt.h>


class mpeg4dec_mpc_api_ddepDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void setConfig(t_uint16 error_reporting, t_uint16 valueDecision, t_uint32 tempValue) = 0;
    virtual void setParameter(t_uint32 deblocking_param_struct[10], t_t1xhv_algo_id algoId, ts_ddep_buffer_descriptor ddep_vdc_mpeg4_param_desc, void* mpc_vfm_mem_ctxt) = 0;
    virtual void setNeeds(t_uint16 error_reporting, t_uint8 BufCountActual, ts_ddep_buffer_descriptor bbm_desc, ts_ddep_buffer_descriptor debugBuffer_desc, t_uint16 disable) = 0;
    virtual void sendCommandX(OMX_COMMANDTYPE cmd, t_uword param) = 0;
    virtual void CopyComplete(t_uint32 bytes_written, t_uint16 InBuffCnt) = 0;
};

class Impeg4dec_mpc_api_ddep: public NMF::InterfaceReference {
  public:
    Impeg4dec_mpc_api_ddep(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void setConfig(t_uint16 error_reporting, t_uint16 valueDecision, t_uint32 tempValue) {
      ((mpeg4dec_mpc_api_ddepDescriptor*)reference)->setConfig(error_reporting, valueDecision, tempValue);
    }
    void setParameter(t_uint32 deblocking_param_struct[10], t_t1xhv_algo_id algoId, ts_ddep_buffer_descriptor ddep_vdc_mpeg4_param_desc, void* mpc_vfm_mem_ctxt) {
      ((mpeg4dec_mpc_api_ddepDescriptor*)reference)->setParameter(deblocking_param_struct, algoId, ddep_vdc_mpeg4_param_desc, mpc_vfm_mem_ctxt);
    }
    void setNeeds(t_uint16 error_reporting, t_uint8 BufCountActual, ts_ddep_buffer_descriptor bbm_desc, ts_ddep_buffer_descriptor debugBuffer_desc, t_uint16 disable) {
      ((mpeg4dec_mpc_api_ddepDescriptor*)reference)->setNeeds(error_reporting, BufCountActual, bbm_desc, debugBuffer_desc, disable);
    }
    void sendCommandX(OMX_COMMANDTYPE cmd, t_uword param) {
      ((mpeg4dec_mpc_api_ddepDescriptor*)reference)->sendCommandX(cmd, param);
    }
    void CopyComplete(t_uint32 bytes_written, t_uint16 InBuffCnt) {
      ((mpeg4dec_mpc_api_ddepDescriptor*)reference)->CopyComplete(bytes_written, InBuffCnt);
    }
};

#endif
