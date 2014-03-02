/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   SHMin.cpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "cm/inc/cm_macros.h"
#include "SHMin.h"
#include "ENS_Nmf.h"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "afm_proxy_bindings_SHMinTraces.h"
#endif

OMX_ERRORTYPE SHMin::instantiateBindingComponent(void) {
    OMX_ERRORTYPE                   error;
    OMX_U32 priority = getPortPriorityLevel();

    MEMORY_TRACE_ENTER("SHMin::instantiateBindingComponent");

    OstTraceFiltInst1(TRACE_ALWAYS, "AFM_PROXY: SHMin:: instantiateBindingComponent: SHMin instantiated with priority %d",priority);
    error = ENS::instantiateNMFComponent(
            getNMFDomainHandle(), "bindings.shm.shmin", "SHMin", 
            &mNmfHandle, priority);
    if (error != OMX_ErrorNone) return error;

	error = ENS::bindComponent(
           mNmfHandle, "osttrace", mOstTrace, "osttrace");
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponentFromHostEx(
            mNmfHandle, "configure", &mIconfigure, 1);
    if (error != OMX_ErrorNone) return error;
   
    error = ENS::bindComponent(
            mNmfSharedBuffer, "mpc", mNmfHandle, "emptythisbuffer");
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponent(
            mNmfHandle, "inputport", mNmfSharedBuffer, "fillthisbuffer");
    if (error != OMX_ErrorNone) return error;

    MEMORY_TRACE_LEAVE("SHMin::instantiateBindingComponent");

    return OMX_ErrorNone;
}

OMX_ERRORTYPE SHMin::deInstantiateBindingComponent(void) {
    OMX_ERRORTYPE error;

    error = ENS::unbindComponentFromHost(&mIconfigure);
    if (error != OMX_ErrorNone) return error;

    error = ENS::unbindComponent(mNmfSharedBuffer, "mpc");
    if (error != OMX_ErrorNone) return error;

    error = ENS::unbindComponent(mNmfHandle, "inputport");
    if (error != OMX_ErrorNone) return error;

	error = ENS::unbindComponent(mNmfHandle, "osttrace");
    if (error != OMX_ErrorNone) return error;

    error = ENS::destroyNMFComponent(mNmfHandle);
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}

OMX_ERRORTYPE SHMin::configure(void) {
    ShmConfig_t                     ShmConfig = {0, NO_SWAP};
    OMX_PARAM_PORTDEFINITIONTYPE    portdef;

    portdef.nSize           = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
    getOmxIlSpecVersion(&portdef.nVersion);    
    mPort.getParameter(OMX_IndexParamPortDefinition, &portdef);

    ShmConfig.nb_buffer     = portdef.nBufferCountActual;
    ShmConfig.swap_bytes    = SWAP_16;

    OstTraceFiltInst2(TRACE_ALWAYS, "AFM_PROXY: SHMin::configure (nb_buffer=%d) (swap_bytes=%d)",ShmConfig.nb_buffer,ShmConfig.swap_bytes);
    mIconfigure.setParameter(ShmConfig);

    return OMX_ErrorNone;
}

                 
