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

/* Generated C++ skeleton for 'mpeg4dec.mpc.api.ddep' */
#include <host/mpeg4dec/mpc/api/ddep.hpp>
static void sk_mpeg4dec_mpc_api_ddep_setConfig(t_nmf_interface_desc* itfref, t_uint16* _xyuv_data) {
  mpeg4dec_mpc_api_ddepDescriptor *self = (mpeg4dec_mpc_api_ddepDescriptor *)itfref;
  t_uint16 error_reporting;
  t_uint16 valueDecision;
  t_uint32 tempValue;
    /* error_reporting <t_uint16> marshalling */
  error_reporting = ((t_uint16)_xyuv_data[0]);
    /* valueDecision <t_uint16> marshalling */
  valueDecision = ((t_uint16)_xyuv_data[1]);
    /* tempValue <t_uint32> marshalling */
  tempValue = ((t_uint32)_xyuv_data[2] | ((t_uint32)_xyuv_data[2+1] << 16));
  /* Server calling */
  self->setConfig(error_reporting, valueDecision, tempValue);
}

static void sk_mpeg4dec_mpc_api_ddep_setParameter(t_nmf_interface_desc* itfref, t_uint16* _xyuv_data) {
  mpeg4dec_mpc_api_ddepDescriptor *self = (mpeg4dec_mpc_api_ddepDescriptor *)itfref;
    int ___i0, ___j0;
  t_uint32 deblocking_param_struct[10];
  t_t1xhv_algo_id algoId;
  ts_ddep_buffer_descriptor ddep_vdc_mpeg4_param_desc;
  void* mpc_vfm_mem_ctxt;
    /* deblocking_param_struct <t_uint32*> marshalling */
  for(___i0 = ___j0 = 0; ___i0 < 10; ___i0++) {
    deblocking_param_struct[___i0] = ((t_uint32)_xyuv_data[0+___j0] | ((t_uint32)_xyuv_data[0+___j0+1] << 16));
    ___j0 += 2;
  }
    /* algoId <t_t1xhv_algo_id> marshalling */
  algoId = (t_t1xhv_algo_id)((t_uint16)_xyuv_data[20] | ((t_uint16)_xyuv_data[20+1] << 16));
    /* ddep_vdc_mpeg4_param_desc <ts_ddep_buffer_descriptor> marshalling */
  ddep_vdc_mpeg4_param_desc.nSize = ((t_uint32)_xyuv_data[22+0] | ((t_uint32)_xyuv_data[22+0+1] << 16));
  ddep_vdc_mpeg4_param_desc.nPhysicalAddress = ((t_uint32)_xyuv_data[22+2] | ((t_uint32)_xyuv_data[22+2+1] << 16));
  ddep_vdc_mpeg4_param_desc.nLogicalAddress = ((t_uint32)_xyuv_data[22+4] | ((t_uint32)_xyuv_data[22+4+1] << 16));
  ddep_vdc_mpeg4_param_desc.nMpcAddress = ((t_uint32)_xyuv_data[22+6] | ((t_uint32)_xyuv_data[22+6+1] << 16));
    /* mpc_vfm_mem_ctxt <void*> marshalling */
  mpc_vfm_mem_ctxt = (void*)((t_uint16)_xyuv_data[30] | ((t_uint16)_xyuv_data[30+1] << 16));
  /* Server calling */
  self->setParameter(deblocking_param_struct, algoId, ddep_vdc_mpeg4_param_desc, mpc_vfm_mem_ctxt);
}

static void sk_mpeg4dec_mpc_api_ddep_setNeeds(t_nmf_interface_desc* itfref, t_uint16* _xyuv_data) {
  mpeg4dec_mpc_api_ddepDescriptor *self = (mpeg4dec_mpc_api_ddepDescriptor *)itfref;
  t_uint16 error_reporting;
  t_uint8 BufCountActual;
  ts_ddep_buffer_descriptor bbm_desc;
  ts_ddep_buffer_descriptor debugBuffer_desc;
  t_uint16 disable;
    /* error_reporting <t_uint16> marshalling */
  error_reporting = ((t_uint16)_xyuv_data[0]);
    /* BufCountActual <t_uint8> marshalling */
  BufCountActual = ((t_uint8)_xyuv_data[1]);
    /* bbm_desc <ts_ddep_buffer_descriptor> marshalling */
  bbm_desc.nSize = ((t_uint32)_xyuv_data[2+0] | ((t_uint32)_xyuv_data[2+0+1] << 16));
  bbm_desc.nPhysicalAddress = ((t_uint32)_xyuv_data[2+2] | ((t_uint32)_xyuv_data[2+2+1] << 16));
  bbm_desc.nLogicalAddress = ((t_uint32)_xyuv_data[2+4] | ((t_uint32)_xyuv_data[2+4+1] << 16));
  bbm_desc.nMpcAddress = ((t_uint32)_xyuv_data[2+6] | ((t_uint32)_xyuv_data[2+6+1] << 16));
    /* debugBuffer_desc <ts_ddep_buffer_descriptor> marshalling */
  debugBuffer_desc.nSize = ((t_uint32)_xyuv_data[10+0] | ((t_uint32)_xyuv_data[10+0+1] << 16));
  debugBuffer_desc.nPhysicalAddress = ((t_uint32)_xyuv_data[10+2] | ((t_uint32)_xyuv_data[10+2+1] << 16));
  debugBuffer_desc.nLogicalAddress = ((t_uint32)_xyuv_data[10+4] | ((t_uint32)_xyuv_data[10+4+1] << 16));
  debugBuffer_desc.nMpcAddress = ((t_uint32)_xyuv_data[10+6] | ((t_uint32)_xyuv_data[10+6+1] << 16));
    /* disable <t_uint16> marshalling */
  disable = ((t_uint16)_xyuv_data[18]);
  /* Server calling */
  self->setNeeds(error_reporting, BufCountActual, bbm_desc, debugBuffer_desc, disable);
}

static void sk_mpeg4dec_mpc_api_ddep_sendCommandX(t_nmf_interface_desc* itfref, t_uint16* _xyuv_data) {
  mpeg4dec_mpc_api_ddepDescriptor *self = (mpeg4dec_mpc_api_ddepDescriptor *)itfref;
  OMX_COMMANDTYPE cmd;
  t_uword param;
    /* cmd <OMX_COMMANDTYPE> marshalling */
  cmd = (OMX_COMMANDTYPE)((t_uint16)_xyuv_data[0] | ((t_uint16)_xyuv_data[0+1] << 16));
    /* param <t_uword> marshalling */
  param = ((t_uword)_xyuv_data[2] | ((t_uword)_xyuv_data[2+1] << 16));
  /* Server calling */
  self->sendCommandX(cmd, param);
}

static void sk_mpeg4dec_mpc_api_ddep_CopyComplete(t_nmf_interface_desc* itfref, t_uint16* _xyuv_data) {
  mpeg4dec_mpc_api_ddepDescriptor *self = (mpeg4dec_mpc_api_ddepDescriptor *)itfref;
  t_uint32 bytes_written;
  t_uint16 InBuffCnt;
    /* bytes_written <t_uint32> marshalling */
  bytes_written = ((t_uint32)_xyuv_data[0] | ((t_uint32)_xyuv_data[0+1] << 16));
    /* InBuffCnt <t_uint16> marshalling */
  InBuffCnt = ((t_uint16)_xyuv_data[2]);
  /* Server calling */
  self->CopyComplete(bytes_written, InBuffCnt);
}

static t_jump_method JT_mpeg4dec_mpc_api_ddep[] = {
  (t_jump_method)sk_mpeg4dec_mpc_api_ddep_setConfig,
  (t_jump_method)sk_mpeg4dec_mpc_api_ddep_setParameter,
  (t_jump_method)sk_mpeg4dec_mpc_api_ddep_setNeeds,
  (t_jump_method)sk_mpeg4dec_mpc_api_ddep_sendCommandX,
  (t_jump_method)sk_mpeg4dec_mpc_api_ddep_CopyComplete,
};

/* Generated C++ skeleton for 'mpeg4dec.mpc.api.emptythisheader' */
#include <host/mpeg4dec/mpc/api/emptythisheader.hpp>
static void sk_mpeg4dec_mpc_api_emptythisheader_emptyThisHeader(t_nmf_interface_desc* itfref, t_uint16* _xyuv_data) {
  mpeg4dec_mpc_api_emptythisheaderDescriptor *self = (mpeg4dec_mpc_api_emptythisheaderDescriptor *)itfref;
  Buffer_p buffer;
    /* buffer <Buffer_p> marshalling */
  buffer = (Buffer_t*)((t_uint16)_xyuv_data[0] | ((t_uint16)_xyuv_data[0+1] << 16));
  /* Server calling */
  self->emptyThisHeader(buffer);
}

static t_jump_method JT_mpeg4dec_mpc_api_emptythisheader[] = {
  (t_jump_method)sk_mpeg4dec_mpc_api_emptythisheader_emptyThisHeader,
};

/* Generated C++ skeleton for 'mpeg4dec.arm_nmf.api.Copy' */
#include <host/mpeg4dec/arm_nmf/api/Copy.hpp>
static void sk_mpeg4dec_arm_nmf_api_Copy_InternalCopy(t_nmf_interface_desc* itfref, t_uint16* _xyuv_data) {
  mpeg4dec_arm_nmf_api_CopyDescriptor *self = (mpeg4dec_arm_nmf_api_CopyDescriptor *)itfref;
  t_uint16 InBuffCnt;
  t_uint32 offset;
    /* InBuffCnt <t_uint16> marshalling */
  InBuffCnt = ((t_uint16)_xyuv_data[0]);
    /* offset <t_uint32> marshalling */
  offset = ((t_uint32)_xyuv_data[1] | ((t_uint32)_xyuv_data[1+1] << 16));
  /* Server calling */
  self->InternalCopy(InBuffCnt, offset);
}

static t_jump_method JT_mpeg4dec_arm_nmf_api_Copy[] = {
  (t_jump_method)sk_mpeg4dec_arm_nmf_api_Copy_InternalCopy,
};

/* Generated C++ skeleton for 'mpeg4dec.mpc.api.fillthisheader' */
#include <host/mpeg4dec/mpc/api/fillthisheader.hpp>
static void sk_mpeg4dec_mpc_api_fillthisheader_fillThisHeader(t_nmf_interface_desc* itfref, t_uint16* _xyuv_data) {
  mpeg4dec_mpc_api_fillthisheaderDescriptor *self = (mpeg4dec_mpc_api_fillthisheaderDescriptor *)itfref;
  Buffer_p buffer;
    /* buffer <Buffer_p> marshalling */
  buffer = (Buffer_t*)((t_uint16)_xyuv_data[0] | ((t_uint16)_xyuv_data[0+1] << 16));
  /* Server calling */
  self->fillThisHeader(buffer);
}

static t_jump_method JT_mpeg4dec_mpc_api_fillthisheader[] = {
  (t_jump_method)sk_mpeg4dec_mpc_api_fillthisheader_fillThisHeader,
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

static t_nmf_skel_function video_mpeg4dec_cpp_skel_functions[] = {
  {"mpeg4dec.mpc.api.ddep", JT_mpeg4dec_mpc_api_ddep},
  {"mpeg4dec.mpc.api.emptythisheader", JT_mpeg4dec_mpc_api_emptythisheader},
  {"mpeg4dec.arm_nmf.api.Copy", JT_mpeg4dec_arm_nmf_api_Copy},
  {"mpeg4dec.mpc.api.fillthisheader", JT_mpeg4dec_mpc_api_fillthisheader},
  {"shared_emptythisbuffer", JT_shared_emptythisbuffer},
  {"shared_fillthisbuffer", JT_shared_fillthisbuffer},
  {"eventhandler", JT_eventhandler},
};

/*const */t_nmf_skel_register video_mpeg4dec_cpp_skel_register = {
    NMF_BC_IN_CPP,
    sizeof(video_mpeg4dec_cpp_skel_functions)/sizeof(t_nmf_skel_function),
    video_mpeg4dec_cpp_skel_functions
};
