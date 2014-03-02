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

/* 'sw_3A_wrp' header library */
#if !defined(sw_3A_wrp_NMF)
#define sw_3A_wrp_NMF

#include <inc/type.h>
#include <cpp.hpp>
#ifndef HOST_ONLY
#include <cm/inc/cm.hpp>
#endif
#include <sw_3A/api/control.hpp>
#include <sw_3A/api/sync.hpp>
#include <sw_3A/api/done.hpp>
#include <ee/api/trace.hpp>

class sw_3A_wrp: public NMF::Composite {
  public:
  protected:
    virtual ~sw_3A_wrp() {} // Protected, use sw_3A_wrp() instead !!!
};

/*
 * Component Factory
 */
class sw_3A;
IMPORT_NMF_COMPONENT sw_3A_wrp* sw_3A_wrpCreate(sw_3A *pComp=0x0);
IMPORT_NMF_COMPONENT void sw_3A_wrpDestroy(sw_3A_wrp*& instance);

#endif
