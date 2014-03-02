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

/* Generated host skeleton for 'ispctl.api.alert' */
#include <host/ispctl/api/alert.h>
static void sk_ispctl_api_alert_info(t_nmf_interface_desc* itfref, t_uint16* _xyuv_data) {
  Iispctl_api_alert *self = (Iispctl_api_alert *)itfref;
  enum e_ispctlInfo info;
  t_uint32 value;
  t_uint32 timestamp;
    /* info <enum e_ispctlInfo> marshalling */
  info = (enum e_ispctlInfo)((t_uint16)_xyuv_data[0] | ((t_uint16)_xyuv_data[0+1] << 16));
    /* value <t_uint32> marshalling */
  value = ((t_uint32)_xyuv_data[2] | ((t_uint32)_xyuv_data[2+1] << 16));
    /* timestamp <t_uint32> marshalling */
  timestamp = ((t_uint32)_xyuv_data[4] | ((t_uint32)_xyuv_data[4+1] << 16));
  /* Server calling */
  self->info(self->THIS, info, value, timestamp);
}

static void sk_ispctl_api_alert_infoList(t_nmf_interface_desc* itfref, t_uint16* _xyuv_data) {
  Iispctl_api_alert *self = (Iispctl_api_alert *)itfref;
    int ___i0, ___j0;
  enum e_ispctlInfo info;
  ts_PageElement listvalue[ISPCTL_SIZE_TAB_PE];
  t_uint16 nb_of_pe;
  t_uint32 timestamp;
    /* info <enum e_ispctlInfo> marshalling */
  info = (enum e_ispctlInfo)((t_uint16)_xyuv_data[0] | ((t_uint16)_xyuv_data[0+1] << 16));
    /* listvalue <ts_PageElement*> marshalling */
  for(___i0 = ___j0 = 0; ___i0 < 64; ___i0++) {
    listvalue[___i0].pe_addr = ((t_uint16)_xyuv_data[2+___j0+0]);
    listvalue[___i0].pe_data = ((t_uint32)_xyuv_data[2+___j0+1] | ((t_uint32)_xyuv_data[2+___j0+1+1] << 16));
    ___j0 += 3;
  }
    /* nb_of_pe <t_uint16> marshalling */
  nb_of_pe = ((t_uint16)_xyuv_data[194]);
    /* timestamp <t_uint32> marshalling */
  timestamp = ((t_uint32)_xyuv_data[195] | ((t_uint32)_xyuv_data[195+1] << 16));
  /* Server calling */
  self->infoList(self->THIS, info, listvalue, nb_of_pe, timestamp);
}

static void sk_ispctl_api_alert_error(t_nmf_interface_desc* itfref, t_uint16* _xyuv_data) {
  Iispctl_api_alert *self = (Iispctl_api_alert *)itfref;
  enum e_ispctlError error_id;
  t_uint32 data;
  t_uint32 timestamp;
    /* error_id <enum e_ispctlError> marshalling */
  error_id = (enum e_ispctlError)((t_uint16)_xyuv_data[0] | ((t_uint16)_xyuv_data[0+1] << 16));
    /* data <t_uint32> marshalling */
  data = ((t_uint32)_xyuv_data[2] | ((t_uint32)_xyuv_data[2+1] << 16));
    /* timestamp <t_uint32> marshalling */
  timestamp = ((t_uint32)_xyuv_data[4] | ((t_uint32)_xyuv_data[4+1] << 16));
  /* Server calling */
  self->error(self->THIS, error_id, data, timestamp);
}

static void sk_ispctl_api_alert_debug(t_nmf_interface_desc* itfref, t_uint16* _xyuv_data) {
  Iispctl_api_alert *self = (Iispctl_api_alert *)itfref;
  enum e_ispctlDebug debug_id;
  t_uint16 data1;
  t_uint16 data2;
  t_uint32 timestamp;
    /* debug_id <enum e_ispctlDebug> marshalling */
  debug_id = (enum e_ispctlDebug)((t_uint16)_xyuv_data[0] | ((t_uint16)_xyuv_data[0+1] << 16));
    /* data1 <t_uint16> marshalling */
  data1 = ((t_uint16)_xyuv_data[2]);
    /* data2 <t_uint16> marshalling */
  data2 = ((t_uint16)_xyuv_data[3]);
    /* timestamp <t_uint32> marshalling */
  timestamp = ((t_uint32)_xyuv_data[4] | ((t_uint32)_xyuv_data[4+1] << 16));
  /* Server calling */
  self->debug(self->THIS, debug_id, data1, data2, timestamp);
}

static t_jump_method JT_ispctl_api_alert[] = {
  (t_jump_method)sk_ispctl_api_alert_info,
  (t_jump_method)sk_ispctl_api_alert_infoList,
  (t_jump_method)sk_ispctl_api_alert_error,
  (t_jump_method)sk_ispctl_api_alert_debug,
};

/* Generated host skeleton for 'grab.api.alert' */
#include <host/grab/api/alert.h>
static void sk_grab_api_alert_info(t_nmf_interface_desc* itfref, t_uint16* _xyuv_data) {
  Igrab_api_alert *self = (Igrab_api_alert *)itfref;
  enum e_grabInfo info_id;
  t_uint16 buffer_id;
  t_uint16 frame_id;
  t_uint32 timestamp;
    /* info_id <enum e_grabInfo> marshalling */
  info_id = (enum e_grabInfo)((t_uint16)_xyuv_data[0] | ((t_uint16)_xyuv_data[0+1] << 16));
    /* buffer_id <t_uint16> marshalling */
  buffer_id = ((t_uint16)_xyuv_data[2]);
    /* frame_id <t_uint16> marshalling */
  frame_id = ((t_uint16)_xyuv_data[3]);
    /* timestamp <t_uint32> marshalling */
  timestamp = ((t_uint32)_xyuv_data[4] | ((t_uint32)_xyuv_data[4+1] << 16));
  /* Server calling */
  self->info(self->THIS, info_id, buffer_id, frame_id, timestamp);
}

static void sk_grab_api_alert_error(t_nmf_interface_desc* itfref, t_uint16* _xyuv_data) {
  Igrab_api_alert *self = (Igrab_api_alert *)itfref;
  enum e_grabError error_id;
  t_uint16 data;
  t_uint16 buffer_id;
  t_uint32 timestamp;
    /* error_id <enum e_grabError> marshalling */
  error_id = (enum e_grabError)((t_uint16)_xyuv_data[0] | ((t_uint16)_xyuv_data[0+1] << 16));
    /* data <t_uint16> marshalling */
  data = ((t_uint16)_xyuv_data[2]);
    /* buffer_id <t_uint16> marshalling */
  buffer_id = ((t_uint16)_xyuv_data[3]);
    /* timestamp <t_uint32> marshalling */
  timestamp = ((t_uint32)_xyuv_data[4] | ((t_uint32)_xyuv_data[4+1] << 16));
  /* Server calling */
  self->error(self->THIS, error_id, data, buffer_id, timestamp);
}

static void sk_grab_api_alert_debug(t_nmf_interface_desc* itfref, t_uint16* _xyuv_data) {
  Igrab_api_alert *self = (Igrab_api_alert *)itfref;
  enum e_grabDebug debug_id;
  t_uint16 buffer_id;
  t_uint16 data1;
  t_uint16 data2;
  t_uint32 timestamp;
    /* debug_id <enum e_grabDebug> marshalling */
  debug_id = (enum e_grabDebug)((t_uint16)_xyuv_data[0] | ((t_uint16)_xyuv_data[0+1] << 16));
    /* buffer_id <t_uint16> marshalling */
  buffer_id = ((t_uint16)_xyuv_data[2]);
    /* data1 <t_uint16> marshalling */
  data1 = ((t_uint16)_xyuv_data[3]);
    /* data2 <t_uint16> marshalling */
  data2 = ((t_uint16)_xyuv_data[4]);
    /* timestamp <t_uint32> marshalling */
  timestamp = ((t_uint32)_xyuv_data[5] | ((t_uint32)_xyuv_data[5+1] << 16));
  /* Server calling */
  self->debug(self->THIS, debug_id, buffer_id, data1, data2, timestamp);
}

static t_jump_method JT_grab_api_alert[] = {
  (t_jump_method)sk_grab_api_alert_info,
  (t_jump_method)sk_grab_api_alert_error,
  (t_jump_method)sk_grab_api_alert_debug,
};

static t_nmf_skel_function imaging_skel_functions[] = {
  {"ispctl.api.alert", JT_ispctl_api_alert},
  {"grab.api.alert", JT_grab_api_alert},
};

/*const */t_nmf_skel_register imaging_skel_register = {
    NMF_BC_IN_C,
    sizeof(imaging_skel_functions)/sizeof(t_nmf_skel_function),
    imaging_skel_functions
};
