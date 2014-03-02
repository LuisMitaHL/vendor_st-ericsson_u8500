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

/* 'drc_nmfil_host_composite_effect' header library */
#if !defined(drc_nmfil_host_composite_effect_NMF)
#define drc_nmfil_host_composite_effect_NMF

#include <inc/type.h>
#include <cpp.hpp>
#ifndef HOST_ONLY
#include <cm/inc/cm.hpp>
#endif
#include <hst/pcmprocessings/effect.hpp>
#include <drc/nmfil/host/effect/configure.hpp>
#include <ee/api/trace.hpp>

class drc_nmfil_host_composite_effect: public NMF::Composite {
  public:
    t_sint32 priority;
  protected:
    virtual ~drc_nmfil_host_composite_effect() {} // Protected, use drc_nmfil_host_composite_effect() instead !!!
};

/*
 * Component Factory
 */
class drc_nmfil_host_effect;
IMPORT_NMF_COMPONENT drc_nmfil_host_composite_effect* drc_nmfil_host_composite_effectCreate(drc_nmfil_host_effect *pComp=0x0);
IMPORT_NMF_COMPONENT void drc_nmfil_host_composite_effectDestroy(drc_nmfil_host_composite_effect*& instance);

#endif
