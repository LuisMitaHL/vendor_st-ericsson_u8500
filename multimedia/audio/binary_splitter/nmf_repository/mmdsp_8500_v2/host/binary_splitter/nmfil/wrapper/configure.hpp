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

/* 'binary_splitter.nmfil.wrapper.configure' interface */
#if !defined(binary_splitter_nmfil_wrapper_configure_IDL)
#define binary_splitter_nmfil_wrapper_configure_IDL

#include <cpp.hpp>
#include <host/bool.idt.h>
#include <host/samplefreq.idt.h>

typedef struct t_xyuv_t_channelConfig {
  t_uint16 nChans[3];
} t_channelConfig;

typedef struct t_xyuv_t_OutChannelConfig {
  t_uint16 nOutPutChannel[16];
} t_OutChannelConfig;


class binary_splitter_nmfil_wrapper_configureDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void setParameter(t_sample_freq freq, t_channelConfig chConfig, t_uint16 blocksize) = 0;
    virtual void setOutputChannelConfig(t_uint16 portIdx, t_OutChannelConfig OutChannelConfig) = 0;
    virtual void setTrace(t_uint32 addr) = 0;
};

class Ibinary_splitter_nmfil_wrapper_configure: public NMF::InterfaceReference {
  public:
    Ibinary_splitter_nmfil_wrapper_configure(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void setParameter(t_sample_freq freq, t_channelConfig chConfig, t_uint16 blocksize) {
      ((binary_splitter_nmfil_wrapper_configureDescriptor*)reference)->setParameter(freq, chConfig, blocksize);
    }
    void setOutputChannelConfig(t_uint16 portIdx, t_OutChannelConfig OutChannelConfig) {
      ((binary_splitter_nmfil_wrapper_configureDescriptor*)reference)->setOutputChannelConfig(portIdx, OutChannelConfig);
    }
    void setTrace(t_uint32 addr) {
      ((binary_splitter_nmfil_wrapper_configureDescriptor*)reference)->setTrace(addr);
    }
};

#endif
