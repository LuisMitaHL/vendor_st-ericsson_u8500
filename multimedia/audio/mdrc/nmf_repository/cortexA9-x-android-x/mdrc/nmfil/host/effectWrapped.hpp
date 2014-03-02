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

/* 'mdrc_nmfil_host_effectWrapped' header library */
#if !defined(mdrc_nmfil_host_effectWrapped_NMF)
#define mdrc_nmfil_host_effectWrapped_NMF

#include <inc/type.h>
#include <cpp.hpp>
#ifndef HOST_ONLY
#include <cm/inc/cm.hpp>
#endif
#include <mdrc/nmfil/host/effect/configure.hpp>
#include <hst/pcmprocessings/effect.hpp>
#include <ee/api/trace.hpp>

class mdrc_nmfil_host_effectWrapped: public NMF::Composite {
  public:
    t_sint32 priority;
  protected:
    virtual ~mdrc_nmfil_host_effectWrapped() {} // Protected, use mdrc_nmfil_host_effectWrapped() instead !!!
};

/*
 * Component Factory
 */
class mdrc_nmfil_host_effect;
IMPORT_NMF_COMPONENT mdrc_nmfil_host_effectWrapped* mdrc_nmfil_host_effectWrappedCreate(mdrc_nmfil_host_effect *pComp=0x0);
IMPORT_NMF_COMPONENT void mdrc_nmfil_host_effectWrappedDestroy(mdrc_nmfil_host_effectWrapped*& instance);

#endif
