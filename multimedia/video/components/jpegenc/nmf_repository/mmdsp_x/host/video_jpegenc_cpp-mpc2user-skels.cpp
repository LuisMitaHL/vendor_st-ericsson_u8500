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

/* Generated C++ skeleton for 'jpegenc.mpc.api.portSettings' */
#include <host/jpegenc/mpc/api/portSettings.hpp>
static void sk_jpegenc_mpc_api_portSettings_portSettings(t_nmf_interface_desc* itfref, t_uint16* _xyuv_data) {
  jpegenc_mpc_api_portSettingsDescriptor *self = (jpegenc_mpc_api_portSettingsDescriptor *)itfref;
  t_uint32 size;
    /* size <t_uint32> marshalling */
  size = ((t_uint32)_xyuv_data[0] | ((t_uint32)_xyuv_data[0+1] << 16));
  /* Server calling */
  self->portSettings(size);
}

static t_jump_method JT_jpegenc_mpc_api_portSettings[] = {
  (t_jump_method)sk_jpegenc_mpc_api_portSettings_portSettings,
};

static t_nmf_skel_function video_jpegenc_cpp_skel_functions[] = {
  {"jpegenc.mpc.api.portSettings", JT_jpegenc_mpc_api_portSettings},
};

/*const */t_nmf_skel_register video_jpegenc_cpp_skel_register = {
    NMF_BC_IN_CPP,
    sizeof(video_jpegenc_cpp_skel_functions)/sizeof(t_nmf_skel_function),
    video_jpegenc_cpp_skel_functions
};
