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

/* 'virtual_surround_nmfil_host_composite_effect' header library */
#if !defined(virtual_surround_nmfil_host_composite_effect_NMF)
#define virtual_surround_nmfil_host_composite_effect_NMF

#include <inc/type.h>
#include <cpp.hpp>
#ifndef HOST_ONLY
#include <cm/inc/cm.hpp>
#endif
#include <hst/pcmprocessings/effect.hpp>
#include <virtual_surround/nmfil/host/effect/configure.hpp>
#include <ee/api/trace.hpp>

class virtual_surround_nmfil_host_composite_effect: public NMF::Composite {
  public:
    t_sint32 priority;
  protected:
    virtual ~virtual_surround_nmfil_host_composite_effect() {} // Protected, use virtual_surround_nmfil_host_composite_effect() instead !!!
};

/*
 * Component Factory
 */
class virtual_surround_nmfil_host_effect;
IMPORT_NMF_COMPONENT virtual_surround_nmfil_host_composite_effect* virtual_surround_nmfil_host_composite_effectCreate(virtual_surround_nmfil_host_effect *pComp=0x0);
IMPORT_NMF_COMPONENT void virtual_surround_nmfil_host_composite_effectDestroy(virtual_surround_nmfil_host_composite_effect*& instance);

#endif
