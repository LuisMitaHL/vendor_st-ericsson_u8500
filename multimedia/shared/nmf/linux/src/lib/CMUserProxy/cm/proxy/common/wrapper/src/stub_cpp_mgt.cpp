/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
#include <cm/proxy/api/cm_proxy.hpp>
#include <cm/proxy/common/communication/inc/hoststubs_mgt.h>
#include <cm/proxy/common/wrapper/inc/stub.h>
extern "C" {
#include <cm/proxy/inc/osal.h>
}

EXPORT_SHARED t_cm_error CM_BindComponentFromUser(
        const t_cm_instance_handle server,
        const char* providedItfServerName,
        t_uint32 fifosize,
        NMF::InterfaceReference* itfref) {
    t_cm_error error;
#ifdef CM_API_TIMING
    unsigned long long start = OSAL_GetTime();
    func[25].name = __func__;
#endif
    error = CM_BindComponentFromUserR(
            server,
            providedItfServerName,
            fifosize,
            CM_MM_MPC_TCM24_X /*CM_MM_MPC_SDRAM24*/,
            (t_nmf_interface_desc*)itfref,
            NMF_BC_IN_CPP);
#ifdef CM_API_TIMING
    func[25].time += OSAL_GetTime() - start;
    func[25].call_count++;
#endif
    return error;
}

EXPORT_SHARED t_cm_error CM_BindComponentFromUserEx(
        const t_cm_instance_handle server,
        const char* providedItfServerName,
        t_uint32 fifosize,
        t_cm_mpc_memory_type eventMemType,
        NMF::InterfaceReference* itfref) {
    t_cm_error error;
#ifdef CM_API_TIMING
    unsigned long long start = OSAL_GetTime();
    func[26].name = __func__;
#endif
    error = CM_BindComponentFromUserR(
            server,
            providedItfServerName,
            fifosize,
            eventMemType,
            (t_nmf_interface_desc*)itfref,
            NMF_BC_IN_CPP);
#ifdef CM_API_TIMING
    func[26].time += OSAL_GetTime() - start;
    func[26].call_count++;
#endif
    return error;
}

class fake: public NMF::InterfaceDescriptor
{
  public:
    t_cm_bf_host2mpc_handle    host2mpcId;
    virtual void fakeMeth() {}
};

EXPORT_SHARED t_cm_error CM_UnbindComponentFromUser(
        NMF::InterfaceReference* itfref) {
    t_cm_error error;
    fake* f;
#ifdef CM_API_TIMING
    unsigned long long start = OSAL_GetTime();
    func[27].name = __func__;
#endif

    if(itfref == NULL)
        return CM_INVALID_PARAMETER;
    f = (fake*)itfref->getReference();

    error = CM_UnbindComponentFromUserR(f->host2mpcId);

    if(error == CM_OK || error == CM_MPC_NOT_RESPONDING) {
        delete f;
    }

#ifdef CM_API_TIMING
    func[27].time += OSAL_GetTime() - start;
    func[27].call_count++;
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
    func[28].name = __func__;
#endif
    error = CM_BindComponentToUserR(
            channel,
            client,
            requiredItfClientName,
            clientContext,
            fifosize,
            NMF_BC_IN_CPP);
#ifdef CM_API_TIMING
    func[28].time += OSAL_GetTime() - start;
    func[28].call_count++;
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
    func[29].name = __func__;
#endif
    error = CM_UnbindComponentToUserR(
            channel,
            client,
            requiredItfClientName,
            clientContext);
#ifdef CM_API_TIMING
    func[29].time += OSAL_GetTime() - start;
    func[29].call_count++;
#endif
    return error;
}
