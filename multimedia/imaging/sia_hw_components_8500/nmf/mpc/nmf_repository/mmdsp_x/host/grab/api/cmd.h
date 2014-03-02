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

/* 'grab.api.cmd' interface */
#if !defined(grab_api_cmd_IDL)
#define grab_api_cmd_IDL

#include <cm/inc/cm_type.h>
#include <host/grab_types.idt.h>
#include <host/trace.idt.h>


typedef struct sIgrab_api_cmd {
  void* THIS;
  t_cm_error (*traceInit)(void* THIS, TraceInfo_t trace_info, t_uint16 id);
  t_cm_error (*abort)(void* THIS, enum e_grabPipeID pipe, t_uint16 client_id);
  t_cm_error (*execute)(void* THIS, enum e_grabPipeID pipe, struct s_grabParams params, t_uint16 client_id);
  t_cm_error (*subscribeEvents)(void* THIS, t_uint16 info_events, t_uint16 error_events, t_uint32 debug_events, t_uint16 client_id);
  t_cm_error (*unsubscribeEvents)(void* THIS, t_uint16 info_events, t_uint16 error_events, t_uint32 debug_events, t_uint16 client_id);
  t_cm_error (*Overscan_Supported)(void* THIS, t_uint16 Adaptive_Overscan_legacy);
  t_cm_error (*disableDspFineLowPower)(void* THIS);
  t_cm_error (*setBMSAdd)(void* THIS, t_uint16 bmsHadd, t_uint16 bmsLadd);
  t_cm_error (*setHiddenBMS)(void* THIS, t_uint16 hiddenBMS);
} Igrab_api_cmd;

typedef struct {
  void* THIS;
  void (*traceInit)(void* THIS, TraceInfo_t trace_info, t_uint16 id);
  void (*abort)(void* THIS, enum e_grabPipeID pipe, t_uint16 client_id);
  void (*execute)(void* THIS, enum e_grabPipeID pipe, struct s_grabParams params, t_uint16 client_id);
  void (*subscribeEvents)(void* THIS, t_uint16 info_events, t_uint16 error_events, t_uint32 debug_events, t_uint16 client_id);
  void (*unsubscribeEvents)(void* THIS, t_uint16 info_events, t_uint16 error_events, t_uint32 debug_events, t_uint16 client_id);
  void (*Overscan_Supported)(void* THIS, t_uint16 Adaptive_Overscan_legacy);
  void (*disableDspFineLowPower)(void* THIS);
  void (*setBMSAdd)(void* THIS, t_uint16 bmsHadd, t_uint16 bmsLadd);
  void (*setHiddenBMS)(void* THIS, t_uint16 hiddenBMS);
} CBgrab_api_cmd;

#endif
