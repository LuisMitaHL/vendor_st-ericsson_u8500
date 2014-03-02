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

/* 'resource_manager.api.mtf_resource' interface */
#if !defined(resource_manager_api_mtf_resource_IDL)
#define resource_manager_api_mtf_resource_IDL

#include <cm/inc/cm_type.h>


typedef struct sIresource_manager_api_mtf_resource {
  void* THIS;
  t_cm_error (*mtfWrite)(void* THIS, t_uint32 dstDataAddress, t_uint16 Size, t_uint32 srcDataAddress);
  t_cm_error (*mtfRead)(void* THIS, t_uint32 srcDataAddress, t_uint16 Size, t_uint32 dstDataAddress);
} Iresource_manager_api_mtf_resource;

typedef struct {
  void* THIS;
  void (*mtfWrite)(void* THIS, t_uint32 dstDataAddress, t_uint16 Size, t_uint32 srcDataAddress);
  void (*mtfRead)(void* THIS, t_uint32 srcDataAddress, t_uint16 Size, t_uint32 dstDataAddress);
} CBresource_manager_api_mtf_resource;

#endif
