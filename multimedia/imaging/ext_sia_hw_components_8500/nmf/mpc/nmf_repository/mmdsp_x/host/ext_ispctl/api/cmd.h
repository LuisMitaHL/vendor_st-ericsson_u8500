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

/* 'ext_ispctl.api.cmd' interface */
#if !defined(ext_ispctl_api_cmd_IDL)
#define ext_ispctl_api_cmd_IDL

#include <cm/inc/cm_type.h>
#include <host/ext_ispctl_types.idt.h>


typedef struct sIext_ispctl_api_cmd {
  void* THIS;
  t_cm_error (*readPageElement)(void* THIS, t_uint16 addr, t_uint8 client_id);
  t_cm_error (*subscribeEvent)(void* THIS, t_uint32 info_events, t_uint16 error_events, t_uint32 debug_events, t_uint8 client_id);
  t_cm_error (*unsubscribeEvent)(void* THIS, t_uint32 info_events, t_uint16 error_events, t_uint32 debug_events, t_uint8 client_id);
} Iext_ispctl_api_cmd;

typedef struct {
  void* THIS;
  void (*readPageElement)(void* THIS, t_uint16 addr, t_uint8 client_id);
  void (*subscribeEvent)(void* THIS, t_uint32 info_events, t_uint16 error_events, t_uint32 debug_events, t_uint8 client_id);
  void (*unsubscribeEvent)(void* THIS, t_uint32 info_events, t_uint16 error_events, t_uint32 debug_events, t_uint8 client_id);
} CBext_ispctl_api_cmd;

#endif
