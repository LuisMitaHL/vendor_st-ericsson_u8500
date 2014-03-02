/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "ENS_Wrapper_Services.h"
#include "ENS_Wrapper.h"

ENS_ASIC_ID EnsWrapper_getASIC_ID()
{
    // The CPU ID is computed when starting ENSwrapper context
    return ENS_Wrapper::ASIC_ID;
}

ENS_IOMX*  EnsWrapper_getEnsComponent(OMX_HANDLETYPE hComp)
{
    OMX_COMPONENTTYPE *omx_comp = (OMX_COMPONENTTYPE *) hComp;
    ENS_Wrapper_p wrapper = (ENS_Wrapper_p) omx_comp->pComponentPrivate;

    return wrapper->getENS_Component();
}

#ifndef HOST_ONLY
t_cm_error EnsWrapper_local_cm_CreateFullContext(OMX_PTR& aCtx,
                                                 t_nmf_channel_flag flag)
{
    //Not use since we use only one shared channel
    aCtx = (OMX_PTR)ENS_WRAPPER_LOCAL_HANDLE_ID;
    return CM_OK;
}

t_cm_error EnsWrapper_local_cm_DestroyFullContext(OMX_PTR& aCtx)
{
    //Not use since we use only one shared channel
    aCtx = (OMX_PTR)ENS_WRAPPER_LOCAL_HANDLE_ID;
    return CM_OK;
}

t_cm_error EnsWrapper_local_cm_bindComponentToUser(OMX_PTR aCtx,
                                                   t_cm_instance_handle component,
                                                   const char *itfname,
                                                   NMF::InterfaceDescriptor *itfref,
                                                   t_uint32 fifosize)
{
    return ENS_Wrapper::cm_bindComponentToUser(aCtx, component, itfname, itfref, fifosize);
}

t_cm_error EnsWrapper_local_cm_unbindComponentToUser(OMX_PTR aCtx,
                                                     const t_cm_instance_handle client,
                                                     const char* itfname)
{
    return ENS_Wrapper::cm_unbindComponentToUser(aCtx, client,itfname);
}

t_cm_error EnsWrapper_cm_bindComponentToUser(OMX_HANDLETYPE hComp,
                                             t_cm_instance_handle component,
                                             const char *itfname,
                                             NMF::InterfaceDescriptor *itfref,
                                             t_uint32 fifosize)
{
    return ENS_Wrapper::cm_bindComponentToUser(hComp, component, itfname, itfref, fifosize);
}

t_cm_error EnsWrapper_cm_unbindComponentToUser(OMX_HANDLETYPE hComp,
                                               const t_cm_instance_handle client,
                                               const char* requiredItfClientName)
{
    return ENS_Wrapper::cm_unbindComponentToUser(hComp, client, requiredItfClientName);
}
#endif //HOST_ONLY

t_nmf_error EnsWrapper_bindToUser(OMX_HANDLETYPE hComp,
                                  NMF::Composite *component,
                                  const char *itfname,
                                  NMF::InterfaceDescriptor *itfref,
                                  t_uint32 fifosize)
{
    OMX_COMPONENTTYPE *omx_comp = (OMX_COMPONENTTYPE *) hComp;
    ENS_Wrapper_p wrapper = (ENS_Wrapper_p) omx_comp->pComponentPrivate;

    return wrapper->bindToUser(component, itfname, itfref, fifosize);
}

t_nmf_error EnsWrapper_unbindToUser(OMX_HANDLETYPE hComp,
                                    NMF::Composite *component,
                                    const char *itfname)
{
    OMX_COMPONENTTYPE *omx_comp = (OMX_COMPONENTTYPE *) hComp;
    ENS_Wrapper_p wrapper = (ENS_Wrapper_p) omx_comp->pComponentPrivate;

    return wrapper->unbindToUser(component, itfname);
}

