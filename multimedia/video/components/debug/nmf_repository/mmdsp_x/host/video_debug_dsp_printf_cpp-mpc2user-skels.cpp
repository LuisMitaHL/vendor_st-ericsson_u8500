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

/* Generated C++ skeleton for 'debug.mpc.api.msg' */
#include <host/debug/mpc/api/msg.hpp>
static void sk_debug_mpc_api_msg_msg(t_nmf_interface_desc* itfref, t_uint16* _xyuv_data) {
  debug_mpc_api_msgDescriptor *self = (debug_mpc_api_msgDescriptor *)itfref;
    int ___i0, ___j0;
  char tab[80];
    /* tab <char*> marshalling */
  for(___i0 = ___j0 = 0; ___i0 < 80; ___i0++) {
    tab[___i0] = ((char)_xyuv_data[0+___j0]);
    ___j0 += 1;
  }
  /* Server calling */
  self->msg(tab);
}

static t_jump_method JT_debug_mpc_api_msg[] = {
  (t_jump_method)sk_debug_mpc_api_msg_msg,
};

static t_nmf_skel_function video_debug_dsp_printf_cpp_skel_functions[] = {
  {"debug.mpc.api.msg", JT_debug_mpc_api_msg},
};

/*const */t_nmf_skel_register video_debug_dsp_printf_cpp_skel_register = {
    NMF_BC_IN_CPP,
    sizeof(video_debug_dsp_printf_cpp_skel_functions)/sizeof(t_nmf_skel_function),
    video_debug_dsp_printf_cpp_skel_functions
};
