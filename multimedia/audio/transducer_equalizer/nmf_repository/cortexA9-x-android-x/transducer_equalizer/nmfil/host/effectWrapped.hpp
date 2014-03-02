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

/* 'transducer_equalizer_nmfil_host_effectWrapped' header library */
#if !defined(transducer_equalizer_nmfil_host_effectWrapped_NMF)
#define transducer_equalizer_nmfil_host_effectWrapped_NMF

#include <inc/type.h>
#include <cpp.hpp>
#ifndef HOST_ONLY
#include <cm/inc/cm.hpp>
#endif
#include <transducer_equalizer/nmfil/host/effect/configure.hpp>
#include <hst/pcmprocessings/effect.hpp>
#include <ee/api/trace.hpp>

class transducer_equalizer_nmfil_host_effectWrapped: public NMF::Composite {
  public:
    t_sint32 priority;
  protected:
    virtual ~transducer_equalizer_nmfil_host_effectWrapped() {} // Protected, use transducer_equalizer_nmfil_host_effectWrapped() instead !!!
};

/*
 * Component Factory
 */
class transducer_equalizer_nmfil_host_effect;
IMPORT_NMF_COMPONENT transducer_equalizer_nmfil_host_effectWrapped* transducer_equalizer_nmfil_host_effectWrappedCreate(transducer_equalizer_nmfil_host_effect *pComp=0x0);
IMPORT_NMF_COMPONENT void transducer_equalizer_nmfil_host_effectWrappedDestroy(transducer_equalizer_nmfil_host_effectWrapped*& instance);

#endif
