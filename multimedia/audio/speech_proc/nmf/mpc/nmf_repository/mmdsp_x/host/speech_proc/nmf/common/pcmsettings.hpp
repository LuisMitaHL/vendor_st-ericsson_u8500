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

/* 'speech_proc.nmf.common.pcmsettings' interface */
#if !defined(speech_proc_nmf_common_pcmsettings_IDL)
#define speech_proc_nmf_common_pcmsettings_IDL

#include <cpp.hpp>
#include <host/samplefreq.idt.h>


class speech_proc_nmf_common_pcmsettingsDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void newFormat(t_sample_freq sample_freq, t_uint16 chans_nb, t_uint16 sample_size, t_uint16 ref_chans_nb) = 0;
};

class Ispeech_proc_nmf_common_pcmsettings: public NMF::InterfaceReference {
  public:
    Ispeech_proc_nmf_common_pcmsettings(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void newFormat(t_sample_freq sample_freq, t_uint16 chans_nb, t_uint16 sample_size, t_uint16 ref_chans_nb) {
      ((speech_proc_nmf_common_pcmsettingsDescriptor*)reference)->newFormat(sample_freq, chans_nb, sample_size, ref_chans_nb);
    }
};

#endif
