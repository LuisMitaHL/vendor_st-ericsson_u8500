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

/* Generated host skeleton for 'resource_manager.api.inform_resource_status' */
#include <host/resource_manager/api/inform_resource_status.h>
static void sk_resource_manager_api_inform_resource_status_informResourceStatus(t_nmf_interface_desc* itfref, t_uint16* _xyuv_data) {
  Iresource_manager_api_inform_resource_status *self = (Iresource_manager_api_inform_resource_status *)itfref;
  t_t1xhv_resource_status status;
  t_t1xhv_resource resource;
    /* status <t_t1xhv_resource_status> marshalling */
  status = (t_t1xhv_resource_status)((t_uint16)_xyuv_data[0] | ((t_uint16)_xyuv_data[0+1] << 16));
    /* resource <t_t1xhv_resource> marshalling */
  resource = (t_t1xhv_resource)((t_uint16)_xyuv_data[2] | ((t_uint16)_xyuv_data[2+1] << 16));
  /* Server calling */
  self->informResourceStatus(self->THIS, status, resource);
}

static t_jump_method JT_resource_manager_api_inform_resource_status[] = {
  (t_jump_method)sk_resource_manager_api_inform_resource_status_informResourceStatus,
};

static t_nmf_skel_function video_resource_manager_skel_functions[] = {
  {"resource_manager.api.inform_resource_status", JT_resource_manager_api_inform_resource_status},
};

/*const */t_nmf_skel_register video_resource_manager_skel_register = {
    NMF_BC_IN_C,
    sizeof(video_resource_manager_skel_functions)/sizeof(t_nmf_skel_function),
    video_resource_manager_skel_functions
};
