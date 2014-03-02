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

/* Generated C++ skeleton for 'grabctl.api.error' */
#include <host/grabctl/api/error.hpp>
static void sk_grabctl_api_error_error(t_nmf_interface_desc* itfref, t_uint16* _xyuv_data) {
  grabctl_api_errorDescriptor *self = (grabctl_api_errorDescriptor *)itfref;
  enum e_grabError error_id;
  t_uint16 data;
  enum e_grabPipeID pipe_id;
    /* error_id <enum e_grabError> marshalling */
  error_id = (enum e_grabError)((t_uint16)_xyuv_data[0] | ((t_uint16)_xyuv_data[0+1] << 16));
    /* data <t_uint16> marshalling */
  data = ((t_uint16)_xyuv_data[2]);
    /* pipe_id <enum e_grabPipeID> marshalling */
  pipe_id = (enum e_grabPipeID)((t_uint16)_xyuv_data[3] | ((t_uint16)_xyuv_data[3+1] << 16));
  /* Server calling */
  self->error(error_id, data, pipe_id);
}

static t_jump_method JT_grabctl_api_error[] = {
  (t_jump_method)sk_grabctl_api_error_error,
};

/* Generated C++ skeleton for 'grabctl.api.acknowledges' */
#include <host/grabctl/api/acknowledges.hpp>
static void sk_grabctl_api_acknowledges_configured(t_nmf_interface_desc* itfref, t_uint16* _xyuv_data) {
  grabctl_api_acknowledgesDescriptor *self = (grabctl_api_acknowledgesDescriptor *)itfref;
  t_uint16 port_idx;
    /* port_idx <t_uint16> marshalling */
  port_idx = ((t_uint16)_xyuv_data[0]);
  /* Server calling */
  self->configured(port_idx);
}

static void sk_grabctl_api_acknowledges_bufferGrabbed(t_nmf_interface_desc* itfref, t_uint16* _xyuv_data) {
  grabctl_api_acknowledgesDescriptor *self = (grabctl_api_acknowledgesDescriptor *)itfref;
  t_uint16 port_idx;
    /* port_idx <t_uint16> marshalling */
  port_idx = ((t_uint16)_xyuv_data[0]);
  /* Server calling */
  self->bufferGrabbed(port_idx);
}

static void sk_grabctl_api_acknowledges_clockwaitingforstart(t_nmf_interface_desc* itfref, t_uint16* _xyuv_data) {
  grabctl_api_acknowledgesDescriptor *self = (grabctl_api_acknowledgesDescriptor *)itfref;
  t_uint16 port_idx;
    /* port_idx <t_uint16> marshalling */
  port_idx = ((t_uint16)_xyuv_data[0]);
  /* Server calling */
  self->clockwaitingforstart(port_idx);
}

static void sk_grabctl_api_acknowledges_clockrunning(t_nmf_interface_desc* itfref, t_uint16* _xyuv_data) {
  grabctl_api_acknowledgesDescriptor *self = (grabctl_api_acknowledgesDescriptor *)itfref;
  t_uint16 port_idx;
    /* port_idx <t_uint16> marshalling */
  port_idx = ((t_uint16)_xyuv_data[0]);
  /* Server calling */
  self->clockrunning(port_idx);
}

static void sk_grabctl_api_acknowledges_grababorted(t_nmf_interface_desc* itfref, t_uint16* _xyuv_data) {
  grabctl_api_acknowledgesDescriptor *self = (grabctl_api_acknowledgesDescriptor *)itfref;
  t_uint16 port_idx;
    /* port_idx <t_uint16> marshalling */
  port_idx = ((t_uint16)_xyuv_data[0]);
  /* Server calling */
  self->grababorted(port_idx);
}

static t_jump_method JT_grabctl_api_acknowledges[] = {
  (t_jump_method)sk_grabctl_api_acknowledges_configured,
  (t_jump_method)sk_grabctl_api_acknowledges_bufferGrabbed,
  (t_jump_method)sk_grabctl_api_acknowledges_clockwaitingforstart,
  (t_jump_method)sk_grabctl_api_acknowledges_clockrunning,
  (t_jump_method)sk_grabctl_api_acknowledges_grababorted,
};

/* Generated C++ skeleton for 'grabctl.api.reportcurves' */
#include <host/grabctl/api/reportcurves.hpp>
static void sk_grabctl_api_reportcurves_reportCurves(t_nmf_interface_desc* itfref, t_uint16* _xyuv_data) {
  grabctl_api_reportcurvesDescriptor *self = (grabctl_api_reportcurvesDescriptor *)itfref;
  t_uint32 h_curve;
  t_uint32 v_curve;
  t_uint32 timestamp;
    /* h_curve <t_uint32> marshalling */
  h_curve = ((t_uint32)_xyuv_data[0] | ((t_uint32)_xyuv_data[0+1] << 16));
    /* v_curve <t_uint32> marshalling */
  v_curve = ((t_uint32)_xyuv_data[2] | ((t_uint32)_xyuv_data[2+1] << 16));
    /* timestamp <t_uint32> marshalling */
  timestamp = ((t_uint32)_xyuv_data[4] | ((t_uint32)_xyuv_data[4+1] << 16));
  /* Server calling */
  self->reportCurves(h_curve, v_curve, timestamp);
}

static t_jump_method JT_grabctl_api_reportcurves[] = {
  (t_jump_method)sk_grabctl_api_reportcurves_reportCurves,
};

/* Generated C++ skeleton for 'ispctl.api.alert' */
#include <host/ispctl/api/alert.hpp>
static void sk_ispctl_api_alert_info(t_nmf_interface_desc* itfref, t_uint16* _xyuv_data) {
  ispctl_api_alertDescriptor *self = (ispctl_api_alertDescriptor *)itfref;
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
  self->info(info, value, timestamp);
}

static void sk_ispctl_api_alert_infoList(t_nmf_interface_desc* itfref, t_uint16* _xyuv_data) {
  ispctl_api_alertDescriptor *self = (ispctl_api_alertDescriptor *)itfref;
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
  self->infoList(info, listvalue, nb_of_pe, timestamp);
}

static void sk_ispctl_api_alert_error(t_nmf_interface_desc* itfref, t_uint16* _xyuv_data) {
  ispctl_api_alertDescriptor *self = (ispctl_api_alertDescriptor *)itfref;
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
  self->error(error_id, data, timestamp);
}

static void sk_ispctl_api_alert_debug(t_nmf_interface_desc* itfref, t_uint16* _xyuv_data) {
  ispctl_api_alertDescriptor *self = (ispctl_api_alertDescriptor *)itfref;
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
  self->debug(debug_id, data1, data2, timestamp);
}

static t_jump_method JT_ispctl_api_alert[] = {
  (t_jump_method)sk_ispctl_api_alert_info,
  (t_jump_method)sk_ispctl_api_alert_infoList,
  (t_jump_method)sk_ispctl_api_alert_error,
  (t_jump_method)sk_ispctl_api_alert_debug,
};

/* Generated C++ skeleton for 'grab.api.alert' */
#include <host/grab/api/alert.hpp>
static void sk_grab_api_alert_info(t_nmf_interface_desc* itfref, t_uint16* _xyuv_data) {
  grab_api_alertDescriptor *self = (grab_api_alertDescriptor *)itfref;
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
  self->info(info_id, buffer_id, frame_id, timestamp);
}

static void sk_grab_api_alert_error(t_nmf_interface_desc* itfref, t_uint16* _xyuv_data) {
  grab_api_alertDescriptor *self = (grab_api_alertDescriptor *)itfref;
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
  self->error(error_id, data, buffer_id, timestamp);
}

static void sk_grab_api_alert_debug(t_nmf_interface_desc* itfref, t_uint16* _xyuv_data) {
  grab_api_alertDescriptor *self = (grab_api_alertDescriptor *)itfref;
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
  self->debug(debug_id, buffer_id, data1, data2, timestamp);
}

static t_jump_method JT_grab_api_alert[] = {
  (t_jump_method)sk_grab_api_alert_info,
  (t_jump_method)sk_grab_api_alert_error,
  (t_jump_method)sk_grab_api_alert_debug,
};

static t_nmf_skel_function imaging_cpp_skel_functions[] = {
  {"grabctl.api.error", JT_grabctl_api_error},
  {"grabctl.api.acknowledges", JT_grabctl_api_acknowledges},
  {"grabctl.api.reportcurves", JT_grabctl_api_reportcurves},
  {"ispctl.api.alert", JT_ispctl_api_alert},
  {"grab.api.alert", JT_grab_api_alert},
};

/*const */t_nmf_skel_register imaging_cpp_skel_register = {
    NMF_BC_IN_CPP,
    sizeof(imaging_cpp_skel_functions)/sizeof(t_nmf_skel_function),
    imaging_cpp_skel_functions
};
