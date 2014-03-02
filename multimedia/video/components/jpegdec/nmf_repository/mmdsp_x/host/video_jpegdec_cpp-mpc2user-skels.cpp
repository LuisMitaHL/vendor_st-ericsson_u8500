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

/* Generated C++ skeleton for 'jpegdec.mpc.api.ddep' */
#include <host/jpegdec/mpc/api/ddep.hpp>
static void sk_jpegdec_mpc_api_ddep_setConfig(t_nmf_interface_desc* itfref, t_uint16* _xyuv_data) {
  jpegdec_mpc_api_ddepDescriptor *self = (jpegdec_mpc_api_ddepDescriptor *)itfref;
  t_uint16 channelId;
    /* channelId <t_uint16> marshalling */
  channelId = ((t_uint16)_xyuv_data[0]);
  /* Server calling */
  self->setConfig(channelId);
}

static void sk_jpegdec_mpc_api_ddep_setParameter(t_nmf_interface_desc* itfref, t_uint16* _xyuv_data) {
  jpegdec_mpc_api_ddepDescriptor *self = (jpegdec_mpc_api_ddepDescriptor *)itfref;
  t_uint16 channelId;
  t_t1xhv_algo_id algoId;
  ts_ddep_buffer_descriptor ddep_sdc_jpeg_param_desc;
    /* channelId <t_uint16> marshalling */
  channelId = ((t_uint16)_xyuv_data[0]);
    /* algoId <t_t1xhv_algo_id> marshalling */
  algoId = (t_t1xhv_algo_id)((t_uint16)_xyuv_data[1] | ((t_uint16)_xyuv_data[1+1] << 16));
    /* ddep_sdc_jpeg_param_desc <ts_ddep_buffer_descriptor> marshalling */
  ddep_sdc_jpeg_param_desc.nSize = ((t_uint32)_xyuv_data[3+0] | ((t_uint32)_xyuv_data[3+0+1] << 16));
  ddep_sdc_jpeg_param_desc.nPhysicalAddress = ((t_uint32)_xyuv_data[3+2] | ((t_uint32)_xyuv_data[3+2+1] << 16));
  ddep_sdc_jpeg_param_desc.nLogicalAddress = ((t_uint32)_xyuv_data[3+4] | ((t_uint32)_xyuv_data[3+4+1] << 16));
  ddep_sdc_jpeg_param_desc.nMpcAddress = ((t_uint32)_xyuv_data[3+6] | ((t_uint32)_xyuv_data[3+6+1] << 16));
  /* Server calling */
  self->setParameter(channelId, algoId, ddep_sdc_jpeg_param_desc);
}

static void sk_jpegdec_mpc_api_ddep_setNeeds(t_nmf_interface_desc* itfref, t_uint16* _xyuv_data) {
  jpegdec_mpc_api_ddepDescriptor *self = (jpegdec_mpc_api_ddepDescriptor *)itfref;
  t_uint16 channelId;
  ts_ddep_buffer_descriptor bbm_desc;
  ts_ddep_buffer_descriptor debugBuffer_desc;
    /* channelId <t_uint16> marshalling */
  channelId = ((t_uint16)_xyuv_data[0]);
    /* bbm_desc <ts_ddep_buffer_descriptor> marshalling */
  bbm_desc.nSize = ((t_uint32)_xyuv_data[1+0] | ((t_uint32)_xyuv_data[1+0+1] << 16));
  bbm_desc.nPhysicalAddress = ((t_uint32)_xyuv_data[1+2] | ((t_uint32)_xyuv_data[1+2+1] << 16));
  bbm_desc.nLogicalAddress = ((t_uint32)_xyuv_data[1+4] | ((t_uint32)_xyuv_data[1+4+1] << 16));
  bbm_desc.nMpcAddress = ((t_uint32)_xyuv_data[1+6] | ((t_uint32)_xyuv_data[1+6+1] << 16));
    /* debugBuffer_desc <ts_ddep_buffer_descriptor> marshalling */
  debugBuffer_desc.nSize = ((t_uint32)_xyuv_data[9+0] | ((t_uint32)_xyuv_data[9+0+1] << 16));
  debugBuffer_desc.nPhysicalAddress = ((t_uint32)_xyuv_data[9+2] | ((t_uint32)_xyuv_data[9+2+1] << 16));
  debugBuffer_desc.nLogicalAddress = ((t_uint32)_xyuv_data[9+4] | ((t_uint32)_xyuv_data[9+4+1] << 16));
  debugBuffer_desc.nMpcAddress = ((t_uint32)_xyuv_data[9+6] | ((t_uint32)_xyuv_data[9+6+1] << 16));
  /* Server calling */
  self->setNeeds(channelId, bbm_desc, debugBuffer_desc);
}

static void sk_jpegdec_mpc_api_ddep_disableFWCodeexection(t_nmf_interface_desc* itfref, t_uint16* _xyuv_data) {
  jpegdec_mpc_api_ddepDescriptor *self = (jpegdec_mpc_api_ddepDescriptor *)itfref;
  t_bool value;
    /* value <t_bool> marshalling */
  value = ((t_bool)_xyuv_data[0]);
  /* Server calling */
  self->disableFWCodeexection(value);
}

static t_jump_method JT_jpegdec_mpc_api_ddep[] = {
  (t_jump_method)sk_jpegdec_mpc_api_ddep_setConfig,
  (t_jump_method)sk_jpegdec_mpc_api_ddep_setParameter,
  (t_jump_method)sk_jpegdec_mpc_api_ddep_setNeeds,
  (t_jump_method)sk_jpegdec_mpc_api_ddep_disableFWCodeexection,
};

/* Generated C++ skeleton for 'jpegdec.mpc.api.emptythisheader' */
#include <host/jpegdec/mpc/api/emptythisheader.hpp>
static void sk_jpegdec_mpc_api_emptythisheader_emptyThisHeader(t_nmf_interface_desc* itfref, t_uint16* _xyuv_data) {
  jpegdec_mpc_api_emptythisheaderDescriptor *self = (jpegdec_mpc_api_emptythisheaderDescriptor *)itfref;
  Buffer_p buffer;
    /* buffer <Buffer_p> marshalling */
  buffer = (Buffer_t*)((t_uint16)_xyuv_data[0] | ((t_uint16)_xyuv_data[0+1] << 16));
  /* Server calling */
  self->emptyThisHeader(buffer);
}

static t_jump_method JT_jpegdec_mpc_api_emptythisheader[] = {
  (t_jump_method)sk_jpegdec_mpc_api_emptythisheader_emptyThisHeader,
};

/* Generated C++ skeleton for 'jpegdec.mpc.api.fillthisheader' */
#include <host/jpegdec/mpc/api/fillthisheader.hpp>
static void sk_jpegdec_mpc_api_fillthisheader_fillThisHeader(t_nmf_interface_desc* itfref, t_uint16* _xyuv_data) {
  jpegdec_mpc_api_fillthisheaderDescriptor *self = (jpegdec_mpc_api_fillthisheaderDescriptor *)itfref;
  Buffer_p buffer;
    /* buffer <Buffer_p> marshalling */
  buffer = (Buffer_t*)((t_uint16)_xyuv_data[0] | ((t_uint16)_xyuv_data[0+1] << 16));
  /* Server calling */
  self->fillThisHeader(buffer);
}

static t_jump_method JT_jpegdec_mpc_api_fillthisheader[] = {
  (t_jump_method)sk_jpegdec_mpc_api_fillthisheader_fillThisHeader,
};

/* Generated C++ skeleton for 'shared_emptythisbuffer' */
#include <host/shared_emptythisbuffer.hpp>
static void sk_shared_emptythisbuffer_emptyThisBuffer(t_nmf_interface_desc* itfref, t_uint16* _xyuv_data) {
  shared_emptythisbufferDescriptor *self = (shared_emptythisbufferDescriptor *)itfref;
  t_uint32 buffer;
    /* buffer <t_uint32> marshalling */
  buffer = ((t_uint32)_xyuv_data[0] | ((t_uint32)_xyuv_data[0+1] << 16));
  /* Server calling */
  self->emptyThisBuffer(buffer);
}

static t_jump_method JT_shared_emptythisbuffer[] = {
  (t_jump_method)sk_shared_emptythisbuffer_emptyThisBuffer,
};

/* Generated C++ skeleton for 'shared_fillthisbuffer' */
#include <host/shared_fillthisbuffer.hpp>
static void sk_shared_fillthisbuffer_fillThisBuffer(t_nmf_interface_desc* itfref, t_uint16* _xyuv_data) {
  shared_fillthisbufferDescriptor *self = (shared_fillthisbufferDescriptor *)itfref;
  t_uint32 buffer;
    /* buffer <t_uint32> marshalling */
  buffer = ((t_uint32)_xyuv_data[0] | ((t_uint32)_xyuv_data[0+1] << 16));
  /* Server calling */
  self->fillThisBuffer(buffer);
}

static t_jump_method JT_shared_fillthisbuffer[] = {
  (t_jump_method)sk_shared_fillthisbuffer_fillThisBuffer,
};

/* Generated C++ skeleton for 'eventhandler' */
#include <host/eventhandler.hpp>
static void sk_eventhandler_eventHandler(t_nmf_interface_desc* itfref, t_uint16* _xyuv_data) {
  eventhandlerDescriptor *self = (eventhandlerDescriptor *)itfref;
  OMX_EVENTTYPE ev;
  t_uint32 data1;
  t_uint32 data2;
    /* ev <OMX_EVENTTYPE> marshalling */
  ev = (OMX_EVENTTYPE)((t_uint16)_xyuv_data[0] | ((t_uint16)_xyuv_data[0+1] << 16));
    /* data1 <t_uint32> marshalling */
  data1 = ((t_uint32)_xyuv_data[2] | ((t_uint32)_xyuv_data[2+1] << 16));
    /* data2 <t_uint32> marshalling */
  data2 = ((t_uint32)_xyuv_data[4] | ((t_uint32)_xyuv_data[4+1] << 16));
  /* Server calling */
  self->eventHandler(ev, data1, data2);
}

static t_jump_method JT_eventhandler[] = {
  (t_jump_method)sk_eventhandler_eventHandler,
};

static t_nmf_skel_function video_jpegdec_cpp_skel_functions[] = {
  {"jpegdec.mpc.api.ddep", JT_jpegdec_mpc_api_ddep},
  {"jpegdec.mpc.api.emptythisheader", JT_jpegdec_mpc_api_emptythisheader},
  {"jpegdec.mpc.api.fillthisheader", JT_jpegdec_mpc_api_fillthisheader},
  {"shared_emptythisbuffer", JT_shared_emptythisbuffer},
  {"shared_fillthisbuffer", JT_shared_fillthisbuffer},
  {"eventhandler", JT_eventhandler},
};

/*const */t_nmf_skel_register video_jpegdec_cpp_skel_register = {
    NMF_BC_IN_CPP,
    sizeof(video_jpegdec_cpp_skel_functions)/sizeof(t_nmf_skel_function),
    video_jpegdec_cpp_skel_functions
};
