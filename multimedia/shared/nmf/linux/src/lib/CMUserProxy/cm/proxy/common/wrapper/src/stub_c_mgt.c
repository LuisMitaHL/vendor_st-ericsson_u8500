/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
#include <cm/proxy/api/cm_proxy.h>
#include <cm/proxy/common/communication/inc/hoststubs_mgt.h>
#include <cm/proxy/common/wrapper/inc/stub.h>
#include <cm/proxy/inc/osal.h>

EXPORT_SHARED t_cm_error CM_UnbindComponentFromUser(
        t_cm_interface_desc* itfref) {
    t_cm_error error;
#ifdef CM_API_TIMING
    unsigned long long start = OSAL_GetTime();
    func[20].name = __func__;
#endif

    if(itfref == NULL || itfref->THIS == 0)
        return CM_INVALID_PARAMETER;

    error = CM_UnbindComponentFromUserR((t_cm_bf_host2mpc_handle)itfref->THIS);

#ifdef CM_API_TIMING
    func[20].time += OSAL_GetTime() - start;
    func[20].call_count++;
#endif
    return error;
}

EXPORT_SHARED t_cm_error CM_BindComponentFromUser(
        const t_cm_instance_handle server,
        const char* providedItfServerName,
        t_uint32 fifosize,
        t_cm_interface_desc* itfref) {
    t_cm_error error;
#ifdef CM_API_TIMING
    unsigned long long start = OSAL_GetTime();
    func[21].name = __func__;
#endif
    error = CM_BindComponentFromUserR(
            server,
            providedItfServerName,
            fifosize,
            CM_MM_MPC_TCM24_X /*CM_MM_MPC_SDRAM24*/,
            (t_nmf_interface_desc*)itfref,
            NMF_BC_IN_C);

#ifdef CM_API_TIMING
    func[21].time += OSAL_GetTime() - start;
    func[21].call_count++;
#endif
    return error;
}

EXPORT_SHARED t_cm_error CM_BindComponentFromUserEx(
        const t_cm_instance_handle server,
        const char* providedItfServerName,
        t_uint32 fifosize,
        t_cm_mpc_memory_type eventMemType,
        t_cm_interface_desc* itfref) {
    t_cm_error error;
#ifdef CM_API_TIMING
    unsigned long long start = OSAL_GetTime();
    func[22].name = __func__;
#endif
    error = CM_BindComponentFromUserR(
            server,
            providedItfServerName,
            fifosize,
            eventMemType,
            (t_nmf_interface_desc*)itfref,
            NMF_BC_IN_C);
#ifdef CM_API_TIMING
    func[22].time += OSAL_GetTime() - start;
    func[22].call_count++;
#endif
    return error;
}

EXPORT_SHARED t_cm_error CM_BindComponentToUser(
    const t_nmf_channel channel,
    const t_cm_instance_handle client,
    const char* requiredItfClientName,
    void* clientContext,
    t_uint32 fifosize)
{
    t_cm_error error;
#ifdef CM_API_TIMING
    unsigned long long start = OSAL_GetTime();
    func[23].name = __func__;
#endif
    error= CM_BindComponentToUserR(
            channel,
            client,
            requiredItfClientName,
            clientContext,
            fifosize,
            NMF_BC_IN_C);
#ifdef CM_API_TIMING
    func[23].time += OSAL_GetTime() - start;
    func[23].call_count++;
#endif
    return error;
}

EXPORT_SHARED t_cm_error CM_UnbindComponentToUser(
    const t_nmf_channel channel,
    const t_cm_instance_handle client,
    const char* requiredItfClientName,
    void **clientContext)
{
    t_cm_error error;
#ifdef CM_API_TIMING
    unsigned long long start = OSAL_GetTime();
    func[24].name = __func__;
#endif
    error = CM_UnbindComponentToUserR(
            channel,
            client,
            requiredItfClientName,
            clientContext);
#ifdef CM_API_TIMING
    func[24].time += OSAL_GetTime() - start;
    func[24].call_count++;
#endif
    return error;
}
