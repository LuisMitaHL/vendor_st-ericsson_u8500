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

/* Generated C++ skeleton for 'interruptdfc' */
#include <host/interruptdfc.hpp>
static void sk_interruptdfc_interruptDFC(t_nmf_interface_desc* itfref, t_uint16* _xyuv_data) {
  interruptdfcDescriptor *self = (interruptdfcDescriptor *)itfref;
  /* Server calling */
  self->interruptDFC();
}

static t_jump_method JT_interruptdfc[] = {
  (t_jump_method)sk_interruptdfc_interruptDFC,
};

/* Generated C++ skeleton for 'requestmemory' */
#include <host/requestmemory.hpp>
static void sk_requestmemory_requestMemory(t_nmf_interface_desc* itfref, t_uint16* _xyuv_data) {
  requestmemoryDescriptor *self = (requestmemoryDescriptor *)itfref;
  t_uint16 size;
    /* size <t_uint16> marshalling */
  size = ((t_uint16)_xyuv_data[0]);
  /* Server calling */
  self->requestMemory(size);
}

static t_jump_method JT_requestmemory[] = {
  (t_jump_method)sk_requestmemory_requestMemory,
};

/* Generated C++ skeleton for 'pcmdump.complete' */
#include <host/pcmdump/complete.hpp>
static void sk_pcmdump_complete_pcmdump_complete(t_nmf_interface_desc* itfref, t_uint16* _xyuv_data) {
  pcmdump_completeDescriptor *self = (pcmdump_completeDescriptor *)itfref;
  PcmDumpCompleteInfo_t completeInfo;
    /* completeInfo <PcmDumpCompleteInfo_t> marshalling */
  completeInfo.omx_port_idx = ((t_uint8)_xyuv_data[0+0]);
  completeInfo.effect_position_idx = ((t_uint8)_xyuv_data[0+1]);
  completeInfo.bufferIdx = ((t_uint8)_xyuv_data[0+2]);
  completeInfo.offset = ((t_sint24)_xyuv_data[0+3] | ((t_sint24)_xyuv_data[0+3+1] << 16));
  /* Server calling */
  self->pcmdump_complete(completeInfo);
}

static t_jump_method JT_pcmdump_complete[] = {
  (t_jump_method)sk_pcmdump_complete_pcmdump_complete,
};

static t_nmf_skel_function afm_cpp_skel_functions[] = {
  {"interruptdfc", JT_interruptdfc},
  {"requestmemory", JT_requestmemory},
  {"pcmdump.complete", JT_pcmdump_complete},
};

/*const */t_nmf_skel_register afm_cpp_skel_register = {
    NMF_BC_IN_CPP,
    sizeof(afm_cpp_skel_functions)/sizeof(t_nmf_skel_function),
    afm_cpp_skel_functions
};
