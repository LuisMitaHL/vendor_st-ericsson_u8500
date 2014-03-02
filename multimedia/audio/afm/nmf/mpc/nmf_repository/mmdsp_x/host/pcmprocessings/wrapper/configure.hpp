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

/* 'pcmprocessings.wrapper.configure' interface */
#if !defined(pcmprocessings_wrapper_configure_IDL)
#define pcmprocessings_wrapper_configure_IDL

#include <cpp.hpp>
#include <host/samplefreq.idt.h>

typedef struct t_xyuv_t_effect_format {
  t_sample_freq freq;
  t_uint16 nof_channels;
  t_uint16 nof_bits_per_sample;
} t_effect_format;

typedef struct t_xyuv_t_effect_config {
  t_uint32 block_size;
  t_effect_format infmt;
  t_effect_format outfmt;
} t_effect_config;

typedef enum t_xyuv_t_pcmprocessing_mode {
  PCMPROCESSING_MODE_INPLACE_PUSH,
  PCMPROCESSING_MODE_INPLACE_PULL,
  PCMPROCESSING_MODE_NOT_INPLACE,
  PCMPROCESSING_MODE_COUNT} t_pcmprocessing_mode;

typedef struct t_xyuv_t_pcmprocessing_config {
  t_pcmprocessing_mode processingMode;
  t_effect_config effectConfig;
} t_pcmprocessing_config;


class pcmprocessings_wrapper_configureDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void setParameter(t_pcmprocessing_config config) = 0;
};

class Ipcmprocessings_wrapper_configure: public NMF::InterfaceReference {
  public:
    Ipcmprocessings_wrapper_configure(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void setParameter(t_pcmprocessing_config config) {
      ((pcmprocessings_wrapper_configureDescriptor*)reference)->setParameter(config);
    }
};

#endif
