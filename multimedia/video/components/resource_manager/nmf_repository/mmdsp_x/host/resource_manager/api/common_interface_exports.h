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

/* 'resource_manager.api.common_interface_exports' interface */
#if !defined(resource_manager_api_common_interface_exports_IDL)
#define resource_manager_api_common_interface_exports_IDL

#include <cm/inc/cm_type.h>
#include <host/t1xhv_common.idt.h>
#include <host/t1xhv_post_process.idt.h>
#include <host/t1xhv_pp.idt.h>
#include <host/t1xhv_retarget.idt.h>
#include <host/t1xhv_vdc.idt.h>
#include <host/t1xhv_vec.idt.h>
#include <host/vfm_common.idt.h>


typedef struct sIresource_manager_api_common_interface_exports {
  void* THIS;
  t_cm_error (*common_interface_exports)(void* THIS);
} Iresource_manager_api_common_interface_exports;

typedef struct {
  void* THIS;
  void (*common_interface_exports)(void* THIS);
} CBresource_manager_api_common_interface_exports;

#endif
