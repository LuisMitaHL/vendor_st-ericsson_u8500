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

/* 'ispctl.api.cmd' interface */
#if !defined(ispctl_api_cmd_IDL)
#define ispctl_api_cmd_IDL

#include <cm/inc/cm_type.h>
#include <host/ispctl_types.idt.h>
#include <host/trace.idt.h>


typedef struct sIispctl_api_cmd {
  void* THIS;
  t_cm_error (*initISP)(void* THIS, t_uint8 client_id);
  t_cm_error (*traceInit)(void* THIS, TraceInfo_t trace_info, t_uint16 id);
  t_cm_error (*readPageElement)(void* THIS, t_uint16 addr, t_uint8 client_id);
  t_cm_error (*WaitExpectedPeValue)(void* THIS, t_uint16 addr, t_uint32 value_expected, t_uint32 polling_frequency, t_uint32 timeout, t_uint8 client_id);
  t_cm_error (*readListPageElement)(void* THIS, ts_PageElement tab_pe[ISPCTL_SIZE_TAB_PE], t_uint16 number_of_pe, t_uint8 client_id);
  t_cm_error (*writePageElement)(void* THIS, t_uint16 addr, t_uint32 value, t_uint8 client_id);
  t_cm_error (*writeListPageElement)(void* THIS, ts_PageElement tab_pe[ISPCTL_SIZE_TAB_PE], t_uint16 number_of_pe, t_uint8 client_id);
  t_cm_error (*updateGridironTable)(void* THIS, t_uint32 addr, t_uint16 size, t_uint8 client_id);
  t_cm_error (*subscribeEvent)(void* THIS, t_uint32 info_events, t_uint16 error_events, t_uint32 debug_events, t_uint8 client_id);
  t_cm_error (*unsubscribeEvent)(void* THIS, t_uint32 info_events, t_uint16 error_events, t_uint32 debug_events, t_uint8 client_id);
  t_cm_error (*allowSleep)(void* THIS);
  t_cm_error (*preventSleep)(void* THIS);
} Iispctl_api_cmd;

typedef struct {
  void* THIS;
  void (*initISP)(void* THIS, t_uint8 client_id);
  void (*traceInit)(void* THIS, TraceInfo_t trace_info, t_uint16 id);
  void (*readPageElement)(void* THIS, t_uint16 addr, t_uint8 client_id);
  void (*WaitExpectedPeValue)(void* THIS, t_uint16 addr, t_uint32 value_expected, t_uint32 polling_frequency, t_uint32 timeout, t_uint8 client_id);
  void (*readListPageElement)(void* THIS, ts_PageElement tab_pe[ISPCTL_SIZE_TAB_PE], t_uint16 number_of_pe, t_uint8 client_id);
  void (*writePageElement)(void* THIS, t_uint16 addr, t_uint32 value, t_uint8 client_id);
  void (*writeListPageElement)(void* THIS, ts_PageElement tab_pe[ISPCTL_SIZE_TAB_PE], t_uint16 number_of_pe, t_uint8 client_id);
  void (*updateGridironTable)(void* THIS, t_uint32 addr, t_uint16 size, t_uint8 client_id);
  void (*subscribeEvent)(void* THIS, t_uint32 info_events, t_uint16 error_events, t_uint32 debug_events, t_uint8 client_id);
  void (*unsubscribeEvent)(void* THIS, t_uint32 info_events, t_uint16 error_events, t_uint32 debug_events, t_uint8 client_id);
  void (*allowSleep)(void* THIS);
  void (*preventSleep)(void* THIS);
} CBispctl_api_cmd;

#endif
