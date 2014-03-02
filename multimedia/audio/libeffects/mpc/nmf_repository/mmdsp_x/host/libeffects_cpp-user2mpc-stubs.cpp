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

#include <cm/inc/cm.h>
#include <cm/proxy/api/private/stub-requiredapi.h>

/* Generated C++ stub for 'libeffects.mpc.libmalloc.init'
   Generator:tools.generator.legacy.cpp.user.stub */
#include <host/libeffects/mpc/libmalloc/init.hpp>

class libeffects_mpc_libmalloc_init: public libeffects_mpc_libmalloc_initDescriptor
{
  public:
    t_cm_bf_host2mpc_handle    host2mpcId;

  public:
    virtual void minit_intmem(void* p_intmemX_start, t_uint24 sz_intmemX, void* p_intmemY_start, t_uint24 sz_intmemY) {
      t_uint16 _xyuv_data[8];
      /* p_intmemX_start <void*> marshalling */
      _xyuv_data[0] = (t_uint16)((unsigned int)p_intmemX_start & 0xFFFFU);
      _xyuv_data[0+1] = (t_uint16)((unsigned int)p_intmemX_start >> 16);
      /* sz_intmemX <t_uint24> marshalling */
      _xyuv_data[2] = (t_uint16)sz_intmemX;
      _xyuv_data[2+1] = (t_uint16)(sz_intmemX >> 16);
      /* p_intmemY_start <void*> marshalling */
      _xyuv_data[4] = (t_uint16)((unsigned int)p_intmemY_start & 0xFFFFU);
      _xyuv_data[4+1] = (t_uint16)((unsigned int)p_intmemY_start >> 16);
      /* sz_intmemY <t_uint24> marshalling */
      _xyuv_data[6] = (t_uint16)sz_intmemY;
      _xyuv_data[6+1] = (t_uint16)(sz_intmemY >> 16);
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 8*2, 0);
    }
    virtual void minit_extmem(void* p_extmem24_start, t_uint24 sz_extmem24, void* p_extmem16_start, t_uint24 sz_extmem16) {
      t_uint16 _xyuv_data[8];
      /* p_extmem24_start <void*> marshalling */
      _xyuv_data[0] = (t_uint16)((unsigned int)p_extmem24_start & 0xFFFFU);
      _xyuv_data[0+1] = (t_uint16)((unsigned int)p_extmem24_start >> 16);
      /* sz_extmem24 <t_uint24> marshalling */
      _xyuv_data[2] = (t_uint16)sz_extmem24;
      _xyuv_data[2+1] = (t_uint16)(sz_extmem24 >> 16);
      /* p_extmem16_start <void*> marshalling */
      _xyuv_data[4] = (t_uint16)((unsigned int)p_extmem16_start & 0xFFFFU);
      _xyuv_data[4+1] = (t_uint16)((unsigned int)p_extmem16_start >> 16);
      /* sz_extmem16 <t_uint24> marshalling */
      _xyuv_data[6] = (t_uint16)sz_extmem16;
      _xyuv_data[6+1] = (t_uint16)(sz_extmem16 >> 16);
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 8*2, 1);
    }
    virtual void minit_esram(void* p_esram24_start, t_uint24 sz_esram24, void* p_esram16_start, t_uint24 sz_esram16) {
      t_uint16 _xyuv_data[8];
      /* p_esram24_start <void*> marshalling */
      _xyuv_data[0] = (t_uint16)((unsigned int)p_esram24_start & 0xFFFFU);
      _xyuv_data[0+1] = (t_uint16)((unsigned int)p_esram24_start >> 16);
      /* sz_esram24 <t_uint24> marshalling */
      _xyuv_data[2] = (t_uint16)sz_esram24;
      _xyuv_data[2+1] = (t_uint16)(sz_esram24 >> 16);
      /* p_esram16_start <void*> marshalling */
      _xyuv_data[4] = (t_uint16)((unsigned int)p_esram16_start & 0xFFFFU);
      _xyuv_data[4+1] = (t_uint16)((unsigned int)p_esram16_start >> 16);
      /* sz_esram16 <t_uint24> marshalling */
      _xyuv_data[6] = (t_uint16)sz_esram16;
      _xyuv_data[6+1] = (t_uint16)(sz_esram16 >> 16);
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 8*2, 2);
    }
    virtual void release_heaps(void) {
      /* Event sending */
      CM_INTERNAL_PushEventWithSize(host2mpcId, (t_uint16*)0x0, 0, 3);
    }
};
static t_cm_bf_host2mpc_handle* INITstub_libeffects_mpc_libmalloc_init(t_nmf_interface_desc* itf) {
  libeffects_mpc_libmalloc_init *stub = new libeffects_mpc_libmalloc_init();
  if(stub == 0x0)
    return 0x0;
  *(NMF::InterfaceReference*)itf = stub;
  return &stub->host2mpcId;
}

static t_nmf_stub_function libeffects_cpp_stub_functions[] = {
  {"libeffects.mpc.libmalloc.init", &INITstub_libeffects_mpc_libmalloc_init},
};

/*const */t_nmf_stub_register libeffects_cpp_stub_register = {
    NMF_BC_IN_CPP,
    sizeof(libeffects_cpp_stub_functions)/sizeof(t_nmf_stub_function),
    libeffects_cpp_stub_functions
};
