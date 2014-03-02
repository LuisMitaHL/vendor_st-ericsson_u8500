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

/* 'grab.api.alert' interface */
#if !defined(grab_api_alert_IDL)
#define grab_api_alert_IDL

#include <cm/inc/cm_type.h>
#include <host/grab_types.idt.h>


typedef struct sIgrab_api_alert {
  void* THIS;
  t_cm_error (*info)(void* THIS, enum e_grabInfo info_id, t_uint16 buffer_id, t_uint16 frame_id, t_uint32 timestamp);
  t_cm_error (*error)(void* THIS, enum e_grabError error_id, t_uint16 data, t_uint16 buffer_id, t_uint32 timestamp);
  t_cm_error (*debug)(void* THIS, enum e_grabDebug debug_id, t_uint16 buffer_id, t_uint16 data1, t_uint16 data2, t_uint32 timestamp);
} Igrab_api_alert;

typedef struct {
  void* THIS;
  void (*info)(void* THIS, enum e_grabInfo info_id, t_uint16 buffer_id, t_uint16 frame_id, t_uint32 timestamp);
  void (*error)(void* THIS, enum e_grabError error_id, t_uint16 data, t_uint16 buffer_id, t_uint32 timestamp);
  void (*debug)(void* THIS, enum e_grabDebug debug_id, t_uint16 buffer_id, t_uint16 data1, t_uint16 data2, t_uint32 timestamp);
} CBgrab_api_alert;

#endif
