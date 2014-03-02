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

/* 'ext_ispctl.api.alert' interface */
#if !defined(ext_ispctl_api_alert_IDL)
#define ext_ispctl_api_alert_IDL

#include <cm/inc/cm_type.h>
#include <host/ext_ispctl_types.idt.h>


typedef struct sIext_ispctl_api_alert {
  void* THIS;
  t_cm_error (*info)(void* THIS, enum e_ispctlInfo info, t_uint32 value, t_uint32 timestamp);
  t_cm_error (*error)(void* THIS, enum e_ispctlError error_id, t_uint32 data, t_uint32 timestamp);
  t_cm_error (*debug)(void* THIS, enum e_ispctlDebug debug_id, t_uint16 data1, t_uint16 data2, t_uint32 timestamp);
} Iext_ispctl_api_alert;

typedef struct {
  void* THIS;
  void (*info)(void* THIS, enum e_ispctlInfo info, t_uint32 value, t_uint32 timestamp);
  void (*error)(void* THIS, enum e_ispctlError error_id, t_uint32 data, t_uint32 timestamp);
  void (*debug)(void* THIS, enum e_ispctlDebug debug_id, t_uint16 data1, t_uint16 data2, t_uint32 timestamp);
} CBext_ispctl_api_alert;

#endif
