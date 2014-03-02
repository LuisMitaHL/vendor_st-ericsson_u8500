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

/* 'resource_manager.api.inform_resource_status' interface */
#if !defined(resource_manager_api_inform_resource_status_IDL)
#define resource_manager_api_inform_resource_status_IDL

#include <cm/inc/cm_type.h>
#include <host/t1xhv_common.idt.h>
#include <host/t1xhv_resource_manager.idt.h>


typedef struct sIresource_manager_api_inform_resource_status {
  void* THIS;
  t_cm_error (*informResourceStatus)(void* THIS, t_t1xhv_resource_status status, t_t1xhv_resource resource);
} Iresource_manager_api_inform_resource_status;

typedef struct {
  void* THIS;
  void (*informResourceStatus)(void* THIS, t_t1xhv_resource_status status, t_t1xhv_resource resource);
} CBresource_manager_api_inform_resource_status;

#endif
