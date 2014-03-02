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

/* 'pcmsettings' interface */
#if !defined(pcmsettings_IDL)
#define pcmsettings_IDL

#include <cpp.hpp>
#include <host/samplefreq.idt.h>


class pcmsettingsDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void newFormat(t_sample_freq sample_freq, t_uint16 chans_nb, t_uint16 sample_size) = 0;
};

class Ipcmsettings: public NMF::InterfaceReference {
  public:
    Ipcmsettings(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void newFormat(t_sample_freq sample_freq, t_uint16 chans_nb, t_uint16 sample_size) {
      ((pcmsettingsDescriptor*)reference)->newFormat(sample_freq, chans_nb, sample_size);
    }
};

#endif
