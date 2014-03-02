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

/* 'ispctl.api.alert' interface */
#if !defined(ispctl_api_alert_IDL)
#define ispctl_api_alert_IDL

#include <cm/inc/cm_type.h>
#include <host/ispctl_types.idt.h>


typedef struct sIispctl_api_alert {
  void* THIS;
  t_cm_error (*info)(void* THIS, enum e_ispctlInfo info, t_uint32 value, t_uint32 timestamp);
  t_cm_error (*infoList)(void* THIS, enum e_ispctlInfo info, ts_PageElement listvalue[ISPCTL_SIZE_TAB_PE], t_uint16 nb_of_pe, t_uint32 timestamp);
  t_cm_error (*error)(void* THIS, enum e_ispctlError error_id, t_uint32 data, t_uint32 timestamp);
  t_cm_error (*debug)(void* THIS, enum e_ispctlDebug debug_id, t_uint16 data1, t_uint16 data2, t_uint32 timestamp);
} Iispctl_api_alert;

typedef struct {
  void* THIS;
  void (*info)(void* THIS, enum e_ispctlInfo info, t_uint32 value, t_uint32 timestamp);
  void (*infoList)(void* THIS, enum e_ispctlInfo info, ts_PageElement listvalue[ISPCTL_SIZE_TAB_PE], t_uint16 nb_of_pe, t_uint32 timestamp);
  void (*error)(void* THIS, enum e_ispctlError error_id, t_uint32 data, t_uint32 timestamp);
  void (*debug)(void* THIS, enum e_ispctlDebug debug_id, t_uint16 data1, t_uint16 data2, t_uint32 timestamp);
} CBispctl_api_alert;

#endif
