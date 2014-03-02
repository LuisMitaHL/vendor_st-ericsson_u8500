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

/* 'hst.common.pcmsettings' interface */
#if !defined(hst_common_pcmsettings_IDL)
#define hst_common_pcmsettings_IDL

#include <cpp.hpp>
#include <host/samplefreq.idt.h>


class hst_common_pcmsettingsDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void newFormat(t_sample_freq sample_freq, t_uint16 chans_nb, t_uint16 sample_size) = 0;
};

class Ihst_common_pcmsettings: public NMF::InterfaceReference {
  public:
    Ihst_common_pcmsettings(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void newFormat(t_sample_freq sample_freq, t_uint16 chans_nb, t_uint16 sample_size) {
      ((hst_common_pcmsettingsDescriptor*)reference)->newFormat(sample_freq, chans_nb, sample_size);
    }
};

#endif
