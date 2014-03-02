/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   ENS_Nmf.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _ENS_NMF_H_
#define _ENS_NMF_H_

#include "ENS_Redefine_Class.h"

#include "OMX_Core.h"
#include "ENS_Component.h"
#include <cm/inc/cm.hpp>

namespace ENS {

    /// @brief Get the MPC address of a memory handle
    /// @param [in] memoryHandle the memory handle of a MPCMemory
    /// @return the address that can be used on MPC side
    /// @allmedia
    inline t_uint32 GetMPCAddress(t_cm_memory_handle memoryHandle) {
        t_uint32 mpcaddr;
        CM_GetMpcMemoryMpcAddress(memoryHandle, &mpcaddr);
        return mpcaddr;
    }

    /// @brief Get the pointer to the ENS Component of a memory handle
    inline ENS_Component * getEnsComponent(OMX_HANDLETYPE omxcomp) {
        return (ENS_Component *)EnsWrapper_getEnsComponent(omxcomp);
    }

    /// @brief Get the pointer to the ENS Port of a memory handle
    inline ENS_Port * getEnsPort(OMX_HANDLETYPE omxcomp, OMX_U32 nPortIndex) {
        return getEnsComponent(omxcomp)->getPort(nPortIndex);
    }

    ENS_API_IMPORT OMX_ERRORTYPE OMX_Error(t_cm_error cm_error);

    ENS_API_IMPORT OMX_ERRORTYPE allocMpcMemory(
            OMX_U32                 domainId,
            t_cm_mpc_memory_type  	memType,
            t_cm_size  	            size,
            t_cm_memory_alignment  	memAlignment,
            t_cm_memory_handle *  	pHandle);

    ENS_API_IMPORT OMX_ERRORTYPE freeMpcMemory(t_cm_memory_handle handle);

    ENS_API_IMPORT OMX_ERRORTYPE instantiateNMFComponent(
                    OMX_U32 domainId, 
                    const char * type, 
                    const char * localName, 
                    t_cm_instance_handle * pHandle,
                    t_nmf_ee_priority priority = NMF_SCHED_NORMAL);

    ENS_API_IMPORT OMX_ERRORTYPE destroyNMFComponent(
                    t_cm_instance_handle handle);
        
    ENS_API_IMPORT OMX_ERRORTYPE startNMFComponent(
                    t_cm_instance_handle handle);
        
    ENS_API_IMPORT OMX_ERRORTYPE stopNMFComponent(
                    t_cm_instance_handle handle);

    ENS_API_IMPORT OMX_ERRORTYPE localCreateFullContext(OMX_PTR& aCtx,
            t_nmf_channel_flag flag = NMF_CHANNEL_SHARED);

    ENS_API_IMPORT OMX_ERRORTYPE localDestroyFullContext(OMX_PTR& aCtx);

    ENS_API_IMPORT OMX_ERRORTYPE localBindComponentToUser(
            OMX_PTR aCtx,
            t_cm_instance_handle component,
            const char *itfname,
            NMF::InterfaceDescriptor *itfref,
            t_uint32 fifosize);

    ENS_API_IMPORT OMX_ERRORTYPE localUnbindComponentToUser(
            OMX_PTR aCtx,
            const t_cm_instance_handle client,
            const char* itfname
            );

    ENS_API_IMPORT OMX_ERRORTYPE bindComponentFromHost(
                    t_cm_instance_handle component, 
                    const char *itfname, 
		    NMF::InterfaceReference *itfref,
                    t_uint32 fifosize);

    ENS_API_IMPORT OMX_ERRORTYPE bindComponentFromHostEx(
                    t_cm_instance_handle component, 
                    const char *itfname, 
		    NMF::InterfaceReference *itfref,
                    t_uint32 fifosize);

    ENS_API_IMPORT OMX_ERRORTYPE bindComponentToHost(
                    OMX_HANDLETYPE hComponent,
                    t_cm_instance_handle component, 
                    const char *itfname, 
                    NMF::InterfaceDescriptor *itfref,
                    t_uint32 fifosize);

    ENS_API_IMPORT OMX_ERRORTYPE bindComponentToVoid(
                    t_cm_instance_handle component, 
                    const char *itfname);

    ENS_API_IMPORT OMX_ERRORTYPE bindComponentAsynchronous(
                    t_cm_instance_handle client, 
                    const char *client_itfname, 
                    t_cm_instance_handle server, 
                    const char *server_itfname, 
                    t_uint32 fifosize);

    ENS_API_IMPORT OMX_ERRORTYPE bindComponentAsynchronousEx(
                    t_cm_instance_handle client, 
                    const char *client_itfname, 
                    t_cm_instance_handle server, 
                    const char *server_itfname, 
                    t_uint32 fifosize);

    ENS_API_IMPORT OMX_ERRORTYPE bindComponent(
                    t_cm_instance_handle client, 
                    const char *client_itfname, 
                    t_cm_instance_handle server, 
                    const char *server_itfname);

    ENS_API_IMPORT OMX_ERRORTYPE unbindComponent(
                    t_cm_instance_handle client, 
                    const char *client_itfname);

    ENS_API_IMPORT OMX_ERRORTYPE unbindComponentToHost(
                    OMX_HANDLETYPE hComponent,
                    t_cm_instance_handle client, 
                    const char *client_itfname);

    ENS_API_IMPORT OMX_ERRORTYPE unbindComponentFromHost(
                    NMF::InterfaceReference *itfref);
        
    ENS_API_IMPORT OMX_ERRORTYPE unbindComponentToVoid(
                    t_cm_instance_handle component, 
                    const char *itfname);
        
    ENS_API_IMPORT OMX_ERRORTYPE unbindComponentAsynchronous(
                    t_cm_instance_handle client, 
                    const char *client_itfname);

    // Deprecated method. Will be removed
    void memcpy(char *dst, char *src, int size);
}
#endif // _ENS_NMF_H_
