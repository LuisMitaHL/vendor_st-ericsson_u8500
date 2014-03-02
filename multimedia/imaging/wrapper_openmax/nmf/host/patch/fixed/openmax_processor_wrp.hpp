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

/* 'openmax_processor_wrp' header library */
#if !defined(openmax_processor_wrp_NMF)
#define openmax_processor_wrp_NMF

#include <inc/type.h>
#include <cpp.hpp>
#if defined(__STN_8500)
#include <cm/inc/cm.hpp>
#endif
#include <armnmf_emptythisbuffer.hpp>
#include <armnmf_fillthisbuffer.hpp>
#include <sendcommand.hpp>
#include <fsminit.hpp>
#include <openmax_processor/api/Config.hpp>
#include <openmax_processor/api/Param.hpp>
#include <eventhandler.hpp>
#include <openmax_processor/api/ToOMXComponent.hpp>
#include <ee/api/trace.hpp>

class openmax_processor_wrp: public NMF::Composite {
  public:
  protected:
    virtual ~openmax_processor_wrp() {} // Protected, use openmax_processor_wrp() instead !!!
};

/*
 * Component Factory
 */
class openmax_processor;
IMPORT_SHARED openmax_processor_wrp* openmax_processor_wrpCreate(openmax_processor *pComp=NULL);
IMPORT_SHARED void openmax_processor_wrpDestroy(openmax_processor_wrp*& instance);

#endif
