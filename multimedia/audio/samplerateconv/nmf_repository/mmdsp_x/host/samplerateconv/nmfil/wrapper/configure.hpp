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

/* 'samplerateconv.nmfil.wrapper.configure' interface */
#if !defined(samplerateconv_nmfil_wrapper_configure_IDL)
#define samplerateconv_nmfil_wrapper_configure_IDL

#include <cpp.hpp>
#include <host/bool.idt.h>
#include <host/samplefreq.idt.h>

typedef enum t_xyuv_t_src_convtype {
  SRC_UNKNOWN,
  SRC_UPSAMPLING,
  SRC_DOWNSAMPLING} t_src_convtype;


class samplerateconv_nmfil_wrapper_configureDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void setParameter(t_sample_freq input_freq, t_sample_freq output_freq, t_uint16 lowmips_to_out48, t_uint16 conv_type, t_uint16 blocksize, t_uint16 channels, t_uint16 force_reconfiguration) = 0;
};

class Isamplerateconv_nmfil_wrapper_configure: public NMF::InterfaceReference {
  public:
    Isamplerateconv_nmfil_wrapper_configure(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void setParameter(t_sample_freq input_freq, t_sample_freq output_freq, t_uint16 lowmips_to_out48, t_uint16 conv_type, t_uint16 blocksize, t_uint16 channels, t_uint16 force_reconfiguration) {
      ((samplerateconv_nmfil_wrapper_configureDescriptor*)reference)->setParameter(input_freq, output_freq, lowmips_to_out48, conv_type, blocksize, channels, force_reconfiguration);
    }
};

#endif
