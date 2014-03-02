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

#include <cm/proxy/api/private/stub-requiredapi.h>

/* Generated host stub for 'ispctl.api.cmd' */
#include <host/ispctl/api/cmd.h>

static t_cm_error st_ispctl_api_cmd_initISP(void* THIS, t_uint8 client_id) {
  t_cm_bf_host2mpc_handle host2mpcId = (t_cm_bf_host2mpc_handle )THIS;
  t_uint16 _xyuv_data[1];
    /* client_id <t_uint8> marshalling */
  _xyuv_data[0] = (t_uint16)client_id;
  /* Event sending */
  return CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 1*2, 0);
}

static t_cm_error st_ispctl_api_cmd_traceInit(void* THIS, TraceInfo_t trace_info, t_uint16 id) {
  t_cm_bf_host2mpc_handle host2mpcId = (t_cm_bf_host2mpc_handle )THIS;
  t_uint16 _xyuv_data[6];
    /* trace_info <TraceInfo_t> marshalling */
  _xyuv_data[0+0] = (t_uint16)trace_info.parentHandle;
  _xyuv_data[0+0+1] = (t_uint16)(trace_info.parentHandle >> 16);
  _xyuv_data[0+2] = (t_uint16)trace_info.traceEnable;
  _xyuv_data[0+3] = (t_uint16)trace_info.dspAddr;
  _xyuv_data[0+3+1] = (t_uint16)(trace_info.dspAddr >> 16);
    /* id <t_uint16> marshalling */
  _xyuv_data[5] = (t_uint16)id;
  /* Event sending */
  return CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 6*2, 1);
}

static t_cm_error st_ispctl_api_cmd_readPageElement(void* THIS, t_uint16 addr, t_uint8 client_id) {
  t_cm_bf_host2mpc_handle host2mpcId = (t_cm_bf_host2mpc_handle )THIS;
  t_uint16 _xyuv_data[2];
    /* addr <t_uint16> marshalling */
  _xyuv_data[0] = (t_uint16)addr;
    /* client_id <t_uint8> marshalling */
  _xyuv_data[1] = (t_uint16)client_id;
  /* Event sending */
  return CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 2*2, 2);
}

static t_cm_error st_ispctl_api_cmd_WaitExpectedPeValue(void* THIS, t_uint16 addr, t_uint32 value_expected, t_uint32 polling_frequency, t_uint32 timeout, t_uint8 client_id) {
  t_cm_bf_host2mpc_handle host2mpcId = (t_cm_bf_host2mpc_handle )THIS;
  t_uint16 _xyuv_data[8];
    /* addr <t_uint16> marshalling */
  _xyuv_data[0] = (t_uint16)addr;
    /* value_expected <t_uint32> marshalling */
  _xyuv_data[1] = (t_uint16)value_expected;
  _xyuv_data[1+1] = (t_uint16)(value_expected >> 16);
    /* polling_frequency <t_uint32> marshalling */
  _xyuv_data[3] = (t_uint16)polling_frequency;
  _xyuv_data[3+1] = (t_uint16)(polling_frequency >> 16);
    /* timeout <t_uint32> marshalling */
  _xyuv_data[5] = (t_uint16)timeout;
  _xyuv_data[5+1] = (t_uint16)(timeout >> 16);
    /* client_id <t_uint8> marshalling */
  _xyuv_data[7] = (t_uint16)client_id;
  /* Event sending */
  return CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 8*2, 3);
}

static t_cm_error st_ispctl_api_cmd_readListPageElement(void* THIS, ts_PageElement tab_pe[ISPCTL_SIZE_TAB_PE], t_uint16 number_of_pe, t_uint8 client_id) {
  t_cm_bf_host2mpc_handle host2mpcId = (t_cm_bf_host2mpc_handle )THIS;
  t_uint16 _xyuv_data[194];
    int ___i0, ___j0;
    /* tab_pe <ts_PageElement*> marshalling */
  for(___i0 = ___j0 = 0; ___i0 < 64; ___i0++) {
    _xyuv_data[0+___j0+0] = (t_uint16)tab_pe[___i0].pe_addr;
    _xyuv_data[0+___j0+1] = (t_uint16)tab_pe[___i0].pe_data;
    _xyuv_data[0+___j0+1+1] = (t_uint16)(tab_pe[___i0].pe_data >> 16);
    ___j0 += 3;
  }
    /* number_of_pe <t_uint16> marshalling */
  _xyuv_data[192] = (t_uint16)number_of_pe;
    /* client_id <t_uint8> marshalling */
  _xyuv_data[193] = (t_uint16)client_id;
  /* Event sending */
  return CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 194*2, 4);
}

static t_cm_error st_ispctl_api_cmd_writePageElement(void* THIS, t_uint16 addr, t_uint32 value, t_uint8 client_id) {
  t_cm_bf_host2mpc_handle host2mpcId = (t_cm_bf_host2mpc_handle )THIS;
  t_uint16 _xyuv_data[4];
    /* addr <t_uint16> marshalling */
  _xyuv_data[0] = (t_uint16)addr;
    /* value <t_uint32> marshalling */
  _xyuv_data[1] = (t_uint16)value;
  _xyuv_data[1+1] = (t_uint16)(value >> 16);
    /* client_id <t_uint8> marshalling */
  _xyuv_data[3] = (t_uint16)client_id;
  /* Event sending */
  return CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 4*2, 5);
}

static t_cm_error st_ispctl_api_cmd_writeListPageElement(void* THIS, ts_PageElement tab_pe[ISPCTL_SIZE_TAB_PE], t_uint16 number_of_pe, t_uint8 client_id) {
  t_cm_bf_host2mpc_handle host2mpcId = (t_cm_bf_host2mpc_handle )THIS;
  t_uint16 _xyuv_data[194];
    int ___i0, ___j0;
    /* tab_pe <ts_PageElement*> marshalling */
  for(___i0 = ___j0 = 0; ___i0 < 64; ___i0++) {
    _xyuv_data[0+___j0+0] = (t_uint16)tab_pe[___i0].pe_addr;
    _xyuv_data[0+___j0+1] = (t_uint16)tab_pe[___i0].pe_data;
    _xyuv_data[0+___j0+1+1] = (t_uint16)(tab_pe[___i0].pe_data >> 16);
    ___j0 += 3;
  }
    /* number_of_pe <t_uint16> marshalling */
  _xyuv_data[192] = (t_uint16)number_of_pe;
    /* client_id <t_uint8> marshalling */
  _xyuv_data[193] = (t_uint16)client_id;
  /* Event sending */
  return CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 194*2, 6);
}

static t_cm_error st_ispctl_api_cmd_updateGridironTable(void* THIS, t_uint32 addr, t_uint16 size, t_uint8 client_id) {
  t_cm_bf_host2mpc_handle host2mpcId = (t_cm_bf_host2mpc_handle )THIS;
  t_uint16 _xyuv_data[4];
    /* addr <t_uint32> marshalling */
  _xyuv_data[0] = (t_uint16)addr;
  _xyuv_data[0+1] = (t_uint16)(addr >> 16);
    /* size <t_uint16> marshalling */
  _xyuv_data[2] = (t_uint16)size;
    /* client_id <t_uint8> marshalling */
  _xyuv_data[3] = (t_uint16)client_id;
  /* Event sending */
  return CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 4*2, 7);
}

static t_cm_error st_ispctl_api_cmd_subscribeEvent(void* THIS, t_uint32 info_events, t_uint16 error_events, t_uint32 debug_events, t_uint8 client_id) {
  t_cm_bf_host2mpc_handle host2mpcId = (t_cm_bf_host2mpc_handle )THIS;
  t_uint16 _xyuv_data[6];
    /* info_events <t_uint32> marshalling */
  _xyuv_data[0] = (t_uint16)info_events;
  _xyuv_data[0+1] = (t_uint16)(info_events >> 16);
    /* error_events <t_uint16> marshalling */
  _xyuv_data[2] = (t_uint16)error_events;
    /* debug_events <t_uint32> marshalling */
  _xyuv_data[3] = (t_uint16)debug_events;
  _xyuv_data[3+1] = (t_uint16)(debug_events >> 16);
    /* client_id <t_uint8> marshalling */
  _xyuv_data[5] = (t_uint16)client_id;
  /* Event sending */
  return CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 6*2, 8);
}

static t_cm_error st_ispctl_api_cmd_unsubscribeEvent(void* THIS, t_uint32 info_events, t_uint16 error_events, t_uint32 debug_events, t_uint8 client_id) {
  t_cm_bf_host2mpc_handle host2mpcId = (t_cm_bf_host2mpc_handle )THIS;
  t_uint16 _xyuv_data[6];
    /* info_events <t_uint32> marshalling */
  _xyuv_data[0] = (t_uint16)info_events;
  _xyuv_data[0+1] = (t_uint16)(info_events >> 16);
    /* error_events <t_uint16> marshalling */
  _xyuv_data[2] = (t_uint16)error_events;
    /* debug_events <t_uint32> marshalling */
  _xyuv_data[3] = (t_uint16)debug_events;
  _xyuv_data[3+1] = (t_uint16)(debug_events >> 16);
    /* client_id <t_uint8> marshalling */
  _xyuv_data[5] = (t_uint16)client_id;
  /* Event sending */
  return CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 6*2, 9);
}

static t_cm_error st_ispctl_api_cmd_allowSleep(void* THIS) {
  t_cm_bf_host2mpc_handle host2mpcId = (t_cm_bf_host2mpc_handle )THIS;
  /* Event sending */
  return CM_INTERNAL_PushEventWithSize(host2mpcId, (t_uint16*)0x0, 0, 10);
}

static t_cm_error st_ispctl_api_cmd_preventSleep(void* THIS) {
  t_cm_bf_host2mpc_handle host2mpcId = (t_cm_bf_host2mpc_handle )THIS;
  /* Event sending */
  return CM_INTERNAL_PushEventWithSize(host2mpcId, (t_uint16*)0x0, 0, 11);
}

static t_cm_bf_host2mpc_handle* INITstub_ispctl_api_cmd(t_nmf_interface_desc* itf) {
  ((Iispctl_api_cmd *)itf)->initISP = st_ispctl_api_cmd_initISP;
  ((Iispctl_api_cmd *)itf)->traceInit = st_ispctl_api_cmd_traceInit;
  ((Iispctl_api_cmd *)itf)->readPageElement = st_ispctl_api_cmd_readPageElement;
  ((Iispctl_api_cmd *)itf)->WaitExpectedPeValue = st_ispctl_api_cmd_WaitExpectedPeValue;
  ((Iispctl_api_cmd *)itf)->readListPageElement = st_ispctl_api_cmd_readListPageElement;
  ((Iispctl_api_cmd *)itf)->writePageElement = st_ispctl_api_cmd_writePageElement;
  ((Iispctl_api_cmd *)itf)->writeListPageElement = st_ispctl_api_cmd_writeListPageElement;
  ((Iispctl_api_cmd *)itf)->updateGridironTable = st_ispctl_api_cmd_updateGridironTable;
  ((Iispctl_api_cmd *)itf)->subscribeEvent = st_ispctl_api_cmd_subscribeEvent;
  ((Iispctl_api_cmd *)itf)->unsubscribeEvent = st_ispctl_api_cmd_unsubscribeEvent;
  ((Iispctl_api_cmd *)itf)->allowSleep = st_ispctl_api_cmd_allowSleep;
  ((Iispctl_api_cmd *)itf)->preventSleep = st_ispctl_api_cmd_preventSleep;
  return (t_cm_bf_host2mpc_handle*)&((Iispctl_api_cmd *)itf)->THIS;
}

/* Generated host stub for 'ispctl.api.cfg' */
#include <host/ispctl/api/cfg.h>

static t_cm_bf_host2mpc_handle* INITstub_ispctl_api_cfg(t_nmf_interface_desc* itf) {
  return (t_cm_bf_host2mpc_handle*)&((Iispctl_api_cfg *)itf)->THIS;
}

/* Generated host stub for 'api.set_debug' */
#include <host/api/set_debug.h>

static t_cm_error st_api_set_debug_setDebug(void* THIS, t_uint16 debug_mode, t_uint16 param1, t_uint16 param2) {
  t_cm_bf_host2mpc_handle host2mpcId = (t_cm_bf_host2mpc_handle )THIS;
  t_uint16 _xyuv_data[3];
    /* debug_mode <t_uint16> marshalling */
  _xyuv_data[0] = (t_uint16)debug_mode;
    /* param1 <t_uint16> marshalling */
  _xyuv_data[1] = (t_uint16)param1;
    /* param2 <t_uint16> marshalling */
  _xyuv_data[2] = (t_uint16)param2;
  /* Event sending */
  return CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 3*2, 0);
}

static t_cm_error st_api_set_debug_readRegister(void* THIS, t_uint16 reg_adress) {
  t_cm_bf_host2mpc_handle host2mpcId = (t_cm_bf_host2mpc_handle )THIS;
  t_uint16 _xyuv_data[1];
    /* reg_adress <t_uint16> marshalling */
  _xyuv_data[0] = (t_uint16)reg_adress;
  /* Event sending */
  return CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 1*2, 1);
}

static t_cm_bf_host2mpc_handle* INITstub_api_set_debug(t_nmf_interface_desc* itf) {
  ((Iapi_set_debug *)itf)->setDebug = st_api_set_debug_setDebug;
  ((Iapi_set_debug *)itf)->readRegister = st_api_set_debug_readRegister;
  return (t_cm_bf_host2mpc_handle*)&((Iapi_set_debug *)itf)->THIS;
}

/* Generated host stub for 'grab.api.cmd' */
#include <host/grab/api/cmd.h>

static t_cm_error st_grab_api_cmd_traceInit(void* THIS, TraceInfo_t trace_info, t_uint16 id) {
  t_cm_bf_host2mpc_handle host2mpcId = (t_cm_bf_host2mpc_handle )THIS;
  t_uint16 _xyuv_data[6];
    /* trace_info <TraceInfo_t> marshalling */
  _xyuv_data[0+0] = (t_uint16)trace_info.parentHandle;
  _xyuv_data[0+0+1] = (t_uint16)(trace_info.parentHandle >> 16);
  _xyuv_data[0+2] = (t_uint16)trace_info.traceEnable;
  _xyuv_data[0+3] = (t_uint16)trace_info.dspAddr;
  _xyuv_data[0+3+1] = (t_uint16)(trace_info.dspAddr >> 16);
    /* id <t_uint16> marshalling */
  _xyuv_data[5] = (t_uint16)id;
  /* Event sending */
  return CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 6*2, 0);
}

static t_cm_error st_grab_api_cmd_abort(void* THIS, enum e_grabPipeID pipe, t_uint16 client_id) {
  t_cm_bf_host2mpc_handle host2mpcId = (t_cm_bf_host2mpc_handle )THIS;
  t_uint16 _xyuv_data[3];
    /* pipe <enum e_grabPipeID> marshalling */
  _xyuv_data[0] = (t_uint16)((unsigned int)pipe & 0xFFFFU);
  _xyuv_data[0+1] = (t_uint16)((unsigned int)pipe >> 16);
    /* client_id <t_uint16> marshalling */
  _xyuv_data[2] = (t_uint16)client_id;
  /* Event sending */
  return CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 3*2, 1);
}

static t_cm_error st_grab_api_cmd_execute(void* THIS, enum e_grabPipeID pipe, struct s_grabParams params, t_uint16 client_id) {
  t_cm_bf_host2mpc_handle host2mpcId = (t_cm_bf_host2mpc_handle )THIS;
  t_uint16 _xyuv_data[57];
    int ___i0, ___j0;
    /* pipe <enum e_grabPipeID> marshalling */
  _xyuv_data[0] = (t_uint16)((unsigned int)pipe & 0xFFFFU);
  _xyuv_data[0+1] = (t_uint16)((unsigned int)pipe >> 16);
    /* params <struct s_grabParams> marshalling */
  _xyuv_data[2+0] = (t_uint16)((unsigned int)params.output_format & 0xFFFFU);
  _xyuv_data[2+0+1] = (t_uint16)((unsigned int)params.output_format >> 16);
  _xyuv_data[2+2] = (t_uint16)params.x_window_size;
  _xyuv_data[2+3] = (t_uint16)params.y_window_size;
  _xyuv_data[2+4] = (t_uint16)params.x_window_offset;
  _xyuv_data[2+5] = (t_uint16)params.y_window_offset;
  _xyuv_data[2+6] = (t_uint16)params.x_frame_size;
  _xyuv_data[2+7] = (t_uint16)params.y_frame_size;
  _xyuv_data[2+8] = (t_uint16)params.dest_buf_addr;
  _xyuv_data[2+8+1] = (t_uint16)(params.dest_buf_addr >> 16);
  _xyuv_data[2+10] = (t_uint16)params.buf_id;
  _xyuv_data[2+11] = (t_uint16)params.rotation_cfg;
  _xyuv_data[2+12] = (t_uint16)params.disable_grab_cache;
  _xyuv_data[2+13] = (t_uint16)params.cache_buf_addr;
  _xyuv_data[2+13+1] = (t_uint16)(params.cache_buf_addr >> 16);
  _xyuv_data[2+15] = (t_uint16)params.enable_stab;
  _xyuv_data[2+16] = (t_uint16)params.x_stab_size_lr;
  _xyuv_data[2+17] = (t_uint16)params.y_stab_size_lr;
  _xyuv_data[2+18] = (t_uint16)params.proj_h_buf_addr;
  _xyuv_data[2+18+1] = (t_uint16)(params.proj_h_buf_addr >> 16);
  _xyuv_data[2+20] = (t_uint16)params.proj_v_buf_addr;
  _xyuv_data[2+20+1] = (t_uint16)(params.proj_v_buf_addr >> 16);
  _xyuv_data[2+22] = (t_uint16)params.lr_alpha;
  _xyuv_data[2+23] = (t_uint16)params.keepRatio;
  _xyuv_data[2+24+0] = (t_uint16)((unsigned int)params.bml_params.pixel_order & 0xFFFFU);
  _xyuv_data[2+24+0+1] = (t_uint16)((unsigned int)params.bml_params.pixel_order >> 16);
  _xyuv_data[2+24+2] = (t_uint16)params.bml_params.nb_linetypes;
  for(___i0 = ___j0 = 0; ___i0 < 8; ___i0++) {
    _xyuv_data[2+24+3+___j0] = (t_uint16)((unsigned int)params.bml_params.line_type[___i0] & 0xFFFFU);
    _xyuv_data[2+24+3+___j0+1] = (t_uint16)((unsigned int)params.bml_params.line_type[___i0] >> 16);
    ___j0 += 2;
  }
  for(___i0 = ___j0 = 0; ___i0 < 8; ___i0++) {
    _xyuv_data[2+24+19+___j0] = (t_uint16)params.bml_params.line_type_nb[___i0];
    ___j0 += 1;
  }
  _xyuv_data[2+24+27] = (t_uint16)params.bml_params.interline_size;
  _xyuv_data[2+24+27+1] = (t_uint16)(params.bml_params.interline_size >> 16);
  _xyuv_data[2+24+29] = (t_uint16)params.bml_params.nb_embedded_lines;
    /* client_id <t_uint16> marshalling */
  _xyuv_data[56] = (t_uint16)client_id;
  /* Event sending */
  return CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 57*2, 2);
}

static t_cm_error st_grab_api_cmd_subscribeEvents(void* THIS, t_uint16 info_events, t_uint16 error_events, t_uint32 debug_events, t_uint16 client_id) {
  t_cm_bf_host2mpc_handle host2mpcId = (t_cm_bf_host2mpc_handle )THIS;
  t_uint16 _xyuv_data[5];
    /* info_events <t_uint16> marshalling */
  _xyuv_data[0] = (t_uint16)info_events;
    /* error_events <t_uint16> marshalling */
  _xyuv_data[1] = (t_uint16)error_events;
    /* debug_events <t_uint32> marshalling */
  _xyuv_data[2] = (t_uint16)debug_events;
  _xyuv_data[2+1] = (t_uint16)(debug_events >> 16);
    /* client_id <t_uint16> marshalling */
  _xyuv_data[4] = (t_uint16)client_id;
  /* Event sending */
  return CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 5*2, 3);
}

static t_cm_error st_grab_api_cmd_unsubscribeEvents(void* THIS, t_uint16 info_events, t_uint16 error_events, t_uint32 debug_events, t_uint16 client_id) {
  t_cm_bf_host2mpc_handle host2mpcId = (t_cm_bf_host2mpc_handle )THIS;
  t_uint16 _xyuv_data[5];
    /* info_events <t_uint16> marshalling */
  _xyuv_data[0] = (t_uint16)info_events;
    /* error_events <t_uint16> marshalling */
  _xyuv_data[1] = (t_uint16)error_events;
    /* debug_events <t_uint32> marshalling */
  _xyuv_data[2] = (t_uint16)debug_events;
  _xyuv_data[2+1] = (t_uint16)(debug_events >> 16);
    /* client_id <t_uint16> marshalling */
  _xyuv_data[4] = (t_uint16)client_id;
  /* Event sending */
  return CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 5*2, 4);
}

static t_cm_error st_grab_api_cmd_Overscan_Supported(void* THIS, t_uint16 Adaptive_Overscan_legacy) {
  t_cm_bf_host2mpc_handle host2mpcId = (t_cm_bf_host2mpc_handle )THIS;
  t_uint16 _xyuv_data[1];
    /* Adaptive_Overscan_legacy <t_uint16> marshalling */
  _xyuv_data[0] = (t_uint16)Adaptive_Overscan_legacy;
  /* Event sending */
  return CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 1*2, 5);
}

static t_cm_error st_grab_api_cmd_disableDspFineLowPower(void* THIS) {
  t_cm_bf_host2mpc_handle host2mpcId = (t_cm_bf_host2mpc_handle )THIS;
  /* Event sending */
  return CM_INTERNAL_PushEventWithSize(host2mpcId, (t_uint16*)0x0, 0, 6);
}

static t_cm_error st_grab_api_cmd_setBMSAdd(void* THIS, t_uint16 bmsHadd, t_uint16 bmsLadd) {
  t_cm_bf_host2mpc_handle host2mpcId = (t_cm_bf_host2mpc_handle )THIS;
  t_uint16 _xyuv_data[2];
    /* bmsHadd <t_uint16> marshalling */
  _xyuv_data[0] = (t_uint16)bmsHadd;
    /* bmsLadd <t_uint16> marshalling */
  _xyuv_data[1] = (t_uint16)bmsLadd;
  /* Event sending */
  return CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 2*2, 7);
}

static t_cm_error st_grab_api_cmd_setHiddenBMS(void* THIS, t_uint16 hiddenBMS) {
  t_cm_bf_host2mpc_handle host2mpcId = (t_cm_bf_host2mpc_handle )THIS;
  t_uint16 _xyuv_data[1];
    /* hiddenBMS <t_uint16> marshalling */
  _xyuv_data[0] = (t_uint16)hiddenBMS;
  /* Event sending */
  return CM_INTERNAL_PushEventWithSize(host2mpcId, _xyuv_data, 1*2, 8);
}

static t_cm_bf_host2mpc_handle* INITstub_grab_api_cmd(t_nmf_interface_desc* itf) {
  ((Igrab_api_cmd *)itf)->traceInit = st_grab_api_cmd_traceInit;
  ((Igrab_api_cmd *)itf)->abort = st_grab_api_cmd_abort;
  ((Igrab_api_cmd *)itf)->execute = st_grab_api_cmd_execute;
  ((Igrab_api_cmd *)itf)->subscribeEvents = st_grab_api_cmd_subscribeEvents;
  ((Igrab_api_cmd *)itf)->unsubscribeEvents = st_grab_api_cmd_unsubscribeEvents;
  ((Igrab_api_cmd *)itf)->Overscan_Supported = st_grab_api_cmd_Overscan_Supported;
  ((Igrab_api_cmd *)itf)->disableDspFineLowPower = st_grab_api_cmd_disableDspFineLowPower;
  ((Igrab_api_cmd *)itf)->setBMSAdd = st_grab_api_cmd_setBMSAdd;
  ((Igrab_api_cmd *)itf)->setHiddenBMS = st_grab_api_cmd_setHiddenBMS;
  return (t_cm_bf_host2mpc_handle*)&((Igrab_api_cmd *)itf)->THIS;
}

static t_nmf_stub_function imaging_stub_functions[] = {
  {"ispctl.api.cmd", &INITstub_ispctl_api_cmd},
  {"ispctl.api.cfg", &INITstub_ispctl_api_cfg},
  {"api.set_debug", &INITstub_api_set_debug},
  {"grab.api.cmd", &INITstub_grab_api_cmd},
};

/*const */t_nmf_stub_register imaging_stub_register = {
    NMF_BC_IN_C,
    sizeof(imaging_stub_functions)/sizeof(t_nmf_stub_function),
    imaging_stub_functions
};
