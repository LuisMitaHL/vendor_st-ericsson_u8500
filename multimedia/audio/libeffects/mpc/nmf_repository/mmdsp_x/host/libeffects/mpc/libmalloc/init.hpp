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

/* 'libeffects.mpc.libmalloc.init' interface */
#if !defined(libeffects_mpc_libmalloc_init_IDL)
#define libeffects_mpc_libmalloc_init_IDL

#include <cpp.hpp>
#include <host/libeffects/mpc/libmalloc/libmalloc.idt.h>


class libeffects_mpc_libmalloc_initDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void minit_intmem(void* p_intmemX_start, t_uint24 sz_intmemX, void* p_intmemY_start, t_uint24 sz_intmemY) = 0;
    virtual void minit_extmem(void* p_extmem24_start, t_uint24 sz_extmem24, void* p_extmem16_start, t_uint24 sz_extmem16) = 0;
    virtual void minit_esram(void* p_esram24_start, t_uint24 sz_esram24, void* p_esram16_start, t_uint24 sz_esram16) = 0;
    virtual void release_heaps(void) = 0;
};

class Ilibeffects_mpc_libmalloc_init: public NMF::InterfaceReference {
  public:
    Ilibeffects_mpc_libmalloc_init(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void minit_intmem(void* p_intmemX_start, t_uint24 sz_intmemX, void* p_intmemY_start, t_uint24 sz_intmemY) {
      ((libeffects_mpc_libmalloc_initDescriptor*)reference)->minit_intmem(p_intmemX_start, sz_intmemX, p_intmemY_start, sz_intmemY);
    }
    void minit_extmem(void* p_extmem24_start, t_uint24 sz_extmem24, void* p_extmem16_start, t_uint24 sz_extmem16) {
      ((libeffects_mpc_libmalloc_initDescriptor*)reference)->minit_extmem(p_extmem24_start, sz_extmem24, p_extmem16_start, sz_extmem16);
    }
    void minit_esram(void* p_esram24_start, t_uint24 sz_esram24, void* p_esram16_start, t_uint24 sz_esram16) {
      ((libeffects_mpc_libmalloc_initDescriptor*)reference)->minit_esram(p_esram24_start, sz_esram24, p_esram16_start, sz_esram16);
    }
    void release_heaps(void) {
      ((libeffects_mpc_libmalloc_initDescriptor*)reference)->release_heaps();
    }
};

#endif
