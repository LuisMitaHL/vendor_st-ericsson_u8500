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

/* 'speech_proc.nmf.common.configure_time_align' interface */
#if !defined(speech_proc_nmf_common_configure_time_align_IDL)
#define speech_proc_nmf_common_configure_time_align_IDL

#include <cpp.hpp>


class speech_proc_nmf_common_configure_time_alignDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void setParameter(t_uint24 shared_buffer_header, t_uint24 ring_buffer, t_uint24 heap, t_uint24 heap_size, t_uint16 real_time_ref, t_uint24 ring_buffer_size) = 0;
    virtual void setConfig(t_uint16 freq, t_uint16 nb_channel) = 0;
    virtual void setConfigRef(t_uint16 in_freq, t_uint16 out_freq, t_uint16 nb_channel_ref) = 0;
};

class Ispeech_proc_nmf_common_configure_time_align: public NMF::InterfaceReference {
  public:
    Ispeech_proc_nmf_common_configure_time_align(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void setParameter(t_uint24 shared_buffer_header, t_uint24 ring_buffer, t_uint24 heap, t_uint24 heap_size, t_uint16 real_time_ref, t_uint24 ring_buffer_size) {
      ((speech_proc_nmf_common_configure_time_alignDescriptor*)reference)->setParameter(shared_buffer_header, ring_buffer, heap, heap_size, real_time_ref, ring_buffer_size);
    }
    void setConfig(t_uint16 freq, t_uint16 nb_channel) {
      ((speech_proc_nmf_common_configure_time_alignDescriptor*)reference)->setConfig(freq, nb_channel);
    }
    void setConfigRef(t_uint16 in_freq, t_uint16 out_freq, t_uint16 nb_channel_ref) {
      ((speech_proc_nmf_common_configure_time_alignDescriptor*)reference)->setConfigRef(in_freq, out_freq, nb_channel_ref);
    }
};

#endif
