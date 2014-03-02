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

/* 'hst.pcmprocessings.effect' interface */
#if !defined(hst_pcmprocessings_effect_IDL)
#define hst_pcmprocessings_effect_IDL

#include <cpp.hpp>
#include <pcmprocessings/effectconfig.idt.h>
#include <armnmf_buffer.idt.h>

typedef enum t_xyuv_t_effect_reset_reason {
  EFFECT_RESET_REASON_STOP,
  EFFECT_RESET_REASON_EOS,
  EFFECT_RESET_REASON_FLUSH,
  EFFECT_RESET_REASON_COUNT} t_effect_reset_reason;

typedef enum t_xyuv_t_effect_process_type {
  EFFECT_PROCESS_TYPE_NOT_INPLACE,
  EFFECT_PROCESS_TYPE_INPLACE,
  EFFECT_PROCESS_TYPE_COUNT} t_effect_process_type;

typedef struct t_xyuv_t_effect_caps {
  t_effect_process_type proc_type;
} t_effect_caps;

typedef struct t_xyuv_t_effect_process_params {
  t_uint32 size;
  t_effect_process_type proc_type;
} t_effect_process_params;

typedef struct t_xyuv_t_effect_process_not_inplace_params {
  t_effect_process_params base;
  OMX_BUFFERHEADERTYPE_p inbuf_hdr;
  t_bool inbuf_emptied;
  OMX_BUFFERHEADERTYPE_p outbuf_hdr;
  t_bool outbuf_filled;
} t_effect_process_not_inplace_params;

typedef struct t_xyuv_t_effect_process_inplace_params {
  t_effect_process_params base;
  OMX_BUFFERHEADERTYPE_p buf_hdr;
} t_effect_process_inplace_params;


class hst_pcmprocessings_effectDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual t_bool open(const t_host_effect_config* config, t_effect_caps* caps) = 0;
    virtual void reset(t_effect_reset_reason reason) = 0;
    virtual void process(t_effect_process_params* params) = 0;
    virtual void close(void) = 0;
};

class Ihst_pcmprocessings_effect: public NMF::InterfaceReference {
  public:
    Ihst_pcmprocessings_effect(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    t_bool open(const t_host_effect_config* config, t_effect_caps* caps) {
      return ((hst_pcmprocessings_effectDescriptor*)reference)->open(config, caps);
    }
    void reset(t_effect_reset_reason reason) {
      ((hst_pcmprocessings_effectDescriptor*)reference)->reset(reason);
    }
    void process(t_effect_process_params* params) {
      ((hst_pcmprocessings_effectDescriptor*)reference)->process(params);
    }
    void close(void) {
      ((hst_pcmprocessings_effectDescriptor*)reference)->close();
    }
};

#endif
