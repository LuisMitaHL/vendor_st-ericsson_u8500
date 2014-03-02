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

/* Generated C++ skeleton for 'mixer.nmfil.wrapper.setConfigApplied' */
#include <host/mixer/nmfil/wrapper/setConfigApplied.hpp>
static void sk_mixer_nmfil_wrapper_setConfigApplied_newConfigApplied(t_nmf_interface_desc* itfref, t_uint16* _xyuv_data) {
  mixer_nmfil_wrapper_setConfigAppliedDescriptor *self = (mixer_nmfil_wrapper_setConfigAppliedDescriptor *)itfref;
  t_uint32 configARMAddress;
    /* configARMAddress <t_uint32> marshalling */
  configARMAddress = ((t_uint32)_xyuv_data[0] | ((t_uint32)_xyuv_data[0+1] << 16));
  /* Server calling */
  self->newConfigApplied(configARMAddress);
}

static t_jump_method JT_mixer_nmfil_wrapper_setConfigApplied[] = {
  (t_jump_method)sk_mixer_nmfil_wrapper_setConfigApplied_newConfigApplied,
};

static t_nmf_skel_function mixer_cpp_skel_functions[] = {
  {"mixer.nmfil.wrapper.setConfigApplied", JT_mixer_nmfil_wrapper_setConfigApplied},
};

/*const */t_nmf_skel_register mixer_cpp_skel_register = {
    NMF_BC_IN_CPP,
    sizeof(mixer_cpp_skel_functions)/sizeof(t_nmf_skel_function),
    mixer_cpp_skel_functions
};
