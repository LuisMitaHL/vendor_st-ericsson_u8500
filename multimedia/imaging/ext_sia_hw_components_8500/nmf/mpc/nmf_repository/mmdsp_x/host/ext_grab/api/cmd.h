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

/* 'ext_grab.api.cmd' interface */
#if !defined(ext_grab_api_cmd_IDL)
#define ext_grab_api_cmd_IDL

#include <cm/inc/cm_type.h>
#include <host/ext_grab_types.idt.h>


typedef struct sIext_grab_api_cmd {
  void* THIS;
  t_cm_error (*abort)(void* THIS, enum e_grabPipeID pipe, t_uint16 client_id);
  t_cm_error (*execute)(void* THIS, enum e_grabPipeID pipe, struct s_grabParams params, t_uint16 client_id);
  t_cm_error (*subscribeEvents)(void* THIS, t_uint16 info_events, t_uint16 error_events, t_uint32 debug_events, t_uint16 client_id);
  t_cm_error (*unsubscribeEvents)(void* THIS, t_uint16 info_events, t_uint16 error_events, t_uint32 debug_events, t_uint16 client_id);
  t_cm_error (*set_frame_skip)(void* THIS, t_uint16 frameSkip);
} Iext_grab_api_cmd;

typedef struct {
  void* THIS;
  void (*abort)(void* THIS, enum e_grabPipeID pipe, t_uint16 client_id);
  void (*execute)(void* THIS, enum e_grabPipeID pipe, struct s_grabParams params, t_uint16 client_id);
  void (*subscribeEvents)(void* THIS, t_uint16 info_events, t_uint16 error_events, t_uint32 debug_events, t_uint16 client_id);
  void (*unsubscribeEvents)(void* THIS, t_uint16 info_events, t_uint16 error_events, t_uint32 debug_events, t_uint16 client_id);
  void (*set_frame_skip)(void* THIS, t_uint16 frameSkip);
} CBext_grab_api_cmd;

#endif
