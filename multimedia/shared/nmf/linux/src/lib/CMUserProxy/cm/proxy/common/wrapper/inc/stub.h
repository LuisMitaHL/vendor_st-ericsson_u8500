/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
/**
 * \internal
 */
#ifndef CM_PROXY_WRAPPER_STUB_H
#define CM_PROXY_WRAPPER_STUB_H

#include <cm/inc/cm_type.h>
#include <cm/proxy/api/private/stub-requiredapi.h>
#include <cm/engine/component/inc/component_type.h>

PUBLIC t_cm_error CM_BindComponentFromUserR(
        const t_cm_instance_handle  server,
        const char                  * providedItfServerName,
        t_uint32                    fifosize,
        t_cm_mpc_memory_type        eventMemType,
        t_nmf_interface_desc        * itfref,
        t_nmf_bc_coding_style       codingStyle);

PUBLIC t_cm_error CM_UnbindComponentFromUserR(
        t_cm_bf_host2mpc_handle host2mpc);

PUBLIC t_cm_error CM_BindComponentToUserR(
    const t_nmf_channel channel,
    const t_cm_instance_handle client,
    const char* requiredItfClientName,
    void* clientContext,
    t_uint32 fifosize,
    t_nmf_bc_coding_style       codingStyle);

PUBLIC t_cm_error CM_UnbindComponentToUserR(
    const t_nmf_channel channel,
    const t_cm_instance_handle client,
    const char* requiredItfClientName,
    void **clientContext);


#endif
