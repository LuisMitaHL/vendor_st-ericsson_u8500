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

/* 'noise_reduction_nmfil_host_effectWrapped' header library */
#if !defined(noise_reduction_nmfil_host_effectWrapped_NMF)
#define noise_reduction_nmfil_host_effectWrapped_NMF

#include <inc/type.h>
#include <cpp.hpp>
#ifndef HOST_ONLY
#include <cm/inc/cm.hpp>
#endif
#include <hst/pcmprocessings/effect.hpp>
#include <noise_reduction/nmfil/host/effect/configure.hpp>
#include <ee/api/trace.hpp>

class noise_reduction_nmfil_host_effectWrapped: public NMF::Composite {
  public:
    t_sint32 priority;
  protected:
    virtual ~noise_reduction_nmfil_host_effectWrapped() {} // Protected, use noise_reduction_nmfil_host_effectWrapped() instead !!!
};

/*
 * Component Factory
 */
class noise_reduction_nmfil_host_effect;
IMPORT_NMF_COMPONENT noise_reduction_nmfil_host_effectWrapped* noise_reduction_nmfil_host_effectWrappedCreate(noise_reduction_nmfil_host_effect *pComp=0x0);
IMPORT_NMF_COMPONENT void noise_reduction_nmfil_host_effectWrappedDestroy(noise_reduction_nmfil_host_effectWrapped*& instance);

#endif
