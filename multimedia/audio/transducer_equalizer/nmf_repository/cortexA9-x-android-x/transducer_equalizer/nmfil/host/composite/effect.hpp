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

/* 'transducer_equalizer_nmfil_host_composite_effect' header library */
#if !defined(transducer_equalizer_nmfil_host_composite_effect_NMF)
#define transducer_equalizer_nmfil_host_composite_effect_NMF

#include <inc/type.h>
#include <cpp.hpp>
#ifndef HOST_ONLY
#include <cm/inc/cm.hpp>
#endif
#include <hst/pcmprocessings/effect.hpp>
#include <transducer_equalizer/nmfil/host/effect/configure.hpp>
#include <ee/api/trace.hpp>

class transducer_equalizer_nmfil_host_composite_effect: public NMF::Composite {
  public:
  protected:
    virtual ~transducer_equalizer_nmfil_host_composite_effect() {} // Protected, use transducer_equalizer_nmfil_host_composite_effect() instead !!!
};

/*
 * Component Factory
 */
class transducer_equalizer_nmfil_host_effect;
IMPORT_NMF_COMPONENT transducer_equalizer_nmfil_host_composite_effect* transducer_equalizer_nmfil_host_composite_effectCreate(transducer_equalizer_nmfil_host_effect *pComp=0x0);
IMPORT_NMF_COMPONENT void transducer_equalizer_nmfil_host_composite_effectDestroy(transducer_equalizer_nmfil_host_composite_effect*& instance);

#endif
