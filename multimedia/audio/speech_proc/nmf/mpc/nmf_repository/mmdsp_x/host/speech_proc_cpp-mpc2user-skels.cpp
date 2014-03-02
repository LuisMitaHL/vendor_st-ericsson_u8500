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

/* Generated C++ skeleton for 'speech_proc.nmf.common.pcmsettings' */
#include <host/speech_proc/nmf/common/pcmsettings.hpp>
static void sk_speech_proc_nmf_common_pcmsettings_newFormat(t_nmf_interface_desc* itfref, t_uint16* _xyuv_data) {
  speech_proc_nmf_common_pcmsettingsDescriptor *self = (speech_proc_nmf_common_pcmsettingsDescriptor *)itfref;
  t_sample_freq sample_freq;
  t_uint16 chans_nb;
  t_uint16 sample_size;
  t_uint16 ref_chans_nb;
    /* sample_freq <t_sample_freq> marshalling */
  sample_freq = (t_sample_freq)((t_uint16)_xyuv_data[0] | ((t_uint16)_xyuv_data[0+1] << 16));
    /* chans_nb <t_uint16> marshalling */
  chans_nb = ((t_uint16)_xyuv_data[2]);
    /* sample_size <t_uint16> marshalling */
  sample_size = ((t_uint16)_xyuv_data[3]);
    /* ref_chans_nb <t_uint16> marshalling */
  ref_chans_nb = ((t_uint16)_xyuv_data[4]);
  /* Server calling */
  self->newFormat(sample_freq, chans_nb, sample_size, ref_chans_nb);
}

static t_jump_method JT_speech_proc_nmf_common_pcmsettings[] = {
  (t_jump_method)sk_speech_proc_nmf_common_pcmsettings_newFormat,
};

/* Generated C++ skeleton for 'speech_proc.nmf.common.hybrid_emptythisbuffer' */
#include <host/speech_proc/nmf/common/hybrid_emptythisbuffer.hpp>
static void sk_speech_proc_nmf_common_hybrid_emptythisbuffer_emptyThisBuffer(t_nmf_interface_desc* itfref, t_uint16* _xyuv_data) {
  speech_proc_nmf_common_hybrid_emptythisbufferDescriptor *self = (speech_proc_nmf_common_hybrid_emptythisbufferDescriptor *)itfref;
  t_uint32 buffer;
    /* buffer <t_uint32> marshalling */
  buffer = ((t_uint32)_xyuv_data[0] | ((t_uint32)_xyuv_data[0+1] << 16));
  /* Server calling */
  self->emptyThisBuffer(buffer);
}

static t_jump_method JT_speech_proc_nmf_common_hybrid_emptythisbuffer[] = {
  (t_jump_method)sk_speech_proc_nmf_common_hybrid_emptythisbuffer_emptyThisBuffer,
};

/* Generated C++ skeleton for 'speech_proc.nmf.common.hybrid_fillthisbuffer' */
#include <host/speech_proc/nmf/common/hybrid_fillthisbuffer.hpp>
static void sk_speech_proc_nmf_common_hybrid_fillthisbuffer_fillThisBuffer(t_nmf_interface_desc* itfref, t_uint16* _xyuv_data) {
  speech_proc_nmf_common_hybrid_fillthisbufferDescriptor *self = (speech_proc_nmf_common_hybrid_fillthisbufferDescriptor *)itfref;
  t_uint32 buffer;
    /* buffer <t_uint32> marshalling */
  buffer = ((t_uint32)_xyuv_data[0] | ((t_uint32)_xyuv_data[0+1] << 16));
  /* Server calling */
  self->fillThisBuffer(buffer);
}

static t_jump_method JT_speech_proc_nmf_common_hybrid_fillthisbuffer[] = {
  (t_jump_method)sk_speech_proc_nmf_common_hybrid_fillthisbuffer_fillThisBuffer,
};

static t_nmf_skel_function speech_proc_cpp_skel_functions[] = {
  {"speech_proc.nmf.common.pcmsettings", JT_speech_proc_nmf_common_pcmsettings},
  {"speech_proc.nmf.common.hybrid_emptythisbuffer", JT_speech_proc_nmf_common_hybrid_emptythisbuffer},
  {"speech_proc.nmf.common.hybrid_fillthisbuffer", JT_speech_proc_nmf_common_hybrid_fillthisbuffer},
};

/*const */t_nmf_skel_register speech_proc_cpp_skel_register = {
    NMF_BC_IN_CPP,
    sizeof(speech_proc_cpp_skel_functions)/sizeof(t_nmf_skel_function),
    speech_proc_cpp_skel_functions
};
