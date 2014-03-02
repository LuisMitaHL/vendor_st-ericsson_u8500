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

/* Generated host stub for 'audiolibs.common.dummy' */
#include <host/audiolibs/common/dummy.h>

static t_cm_bf_host2mpc_handle* INITstub_audiolibs_common_dummy(t_nmf_interface_desc* itf) {
  return (t_cm_bf_host2mpc_handle*)&((Iaudiolibs_common_dummy *)itf)->THIS;
}

static t_nmf_stub_function audiolibs_stub_functions[] = {
  {"audiolibs.common.dummy", &INITstub_audiolibs_common_dummy},
};

/*const */t_nmf_stub_register audiolibs_stub_register = {
    NMF_BC_IN_C,
    sizeof(audiolibs_stub_functions)/sizeof(t_nmf_stub_function),
    audiolibs_stub_functions
};
