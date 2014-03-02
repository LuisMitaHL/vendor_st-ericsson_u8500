/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   NmfMpcPcmProcessing.cpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "cm/inc/cm_macros.h"
#include "ENS_Nmf.h"
#include "NmfMpcPcmProcessing.h"

AFM_API_EXPORT NmfMpcPcmProcessing::NmfMpcPcmProcessing(t_pcmprocessing_config config)
    : NmfComponent(), mNmfSyncLib(0), mNmfAlgo(0), mOstTrace(0)
{
    mConfig = config;
    mCallback = OMX_FALSE;
}

AFM_API_EXPORT OMX_ERRORTYPE NmfMpcPcmProcessing::instantiate(OMX_U32 domainId, OMX_U32 priority) {
    OMX_ERRORTYPE error;

    MEMORY_TRACE_ENTER("NmfMpcPcmProcessing::instantiate");

    setNMFDomainHandle(domainId);
    mMyPriority = priority;

    MEMORY_TRACE_ENTER("NmfMpcPcmProcessing::instantiateAlgo");

    error = instantiateAlgo();
    if (error != OMX_ErrorNone) return error;

    MEMORY_TRACE_LEAVE("NmfMpcPcmProcessing::instantiateAlgo");

    error = ENS::instantiateNMFComponent(
            getNMFDomainHandle(), "pcmprocessings.wrapper", 
            "wrapper", (t_cm_instance_handle *)&mNmfMain, mMyPriority);
    if (error != OMX_ErrorNone) return error;

    error = ENS::instantiateNMFComponent(
            getNMFDomainHandle(), "misc.synchronisation", "synchronisation", 
            &mNmfSyncLib, mMyPriority);
    if (error != OMX_ErrorNone) return error;
    
    error = ENS::bindComponent(
            (t_cm_instance_handle)mNmfMain, "synchronisation", mNmfSyncLib, "synchronisation");
    if (error != OMX_ErrorNone) return error;

    error = ENS::instantiateNMFComponent(
            getNMFDomainHandle(), "osttrace.mmdsp", 
            "OSTTRACE", &mOstTrace, mMyPriority);
    if (error != OMX_ErrorNone) return error; 

    error = ENS::bindComponent(
            (t_cm_instance_handle)mNmfMain, "osttrace", mOstTrace, "osttrace");
    if (error != OMX_ErrorNone) return error;
	
    error = ENS::bindComponent(
		 (t_cm_instance_handle)mNmfMain, "effect", mNmfAlgo, "effect");
    if (error != OMX_ErrorNone) return error;

    t_uint8 requireNb, i;
    t_cm_require_state requireState;
    
    CM_GetComponentRequiredInterfaceNumber((t_cm_instance_handle)mNmfAlgo, &requireNb);

    for(i = 0; i < requireNb; i++) {
        char itfName[MAX_INTERFACE_NAME_LENGTH];
        char itfType[MAX_INTERFACE_TYPE_NAME_LENGTH];
        t_sint16 collectionSize;

        CM_GetComponentRequiredInterface((t_cm_instance_handle)mNmfAlgo, i,
                                         itfName, MAX_INTERFACE_NAME_LENGTH,
                                         itfType, MAX_INTERFACE_TYPE_NAME_LENGTH,
                                         &requireState,
                                         &collectionSize);
        ENS_String<MAX_INTERFACE_NAME_LENGTH> component_itf(itfName);
        ENS_String<MAX_INTERFACE_NAME_LENGTH> required_itf("proxy");
        
        if(component_itf == required_itf)
        {
            error = ENS::bindComponentAsynchronousEx((t_cm_instance_handle)mNmfAlgo, 
                                                     "proxy", (t_cm_instance_handle)mNmfMain, "proxy_callback", 2); // 2 for 2 channel
            if (error != OMX_ErrorNone) return error;

            mCallback = OMX_TRUE;
            break;
            
        }
    }

	error = ENS::bindComponentFromHostEx((t_cm_instance_handle)mNmfMain, "configure", 
			&mIconfigure, 1);
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponentAsynchronous(
            (t_cm_instance_handle)mNmfMain, "me", (t_cm_instance_handle)mNmfMain, "postevent", 2);
    if (error != OMX_ErrorNone) return error;

    MEMORY_TRACE_LEAVE("NmfMpcPcmProcessing::instantiate");

    return OMX_ErrorNone;
}


AFM_API_EXPORT OMX_ERRORTYPE NmfMpcPcmProcessing::start(void) {
    OMX_ERRORTYPE error;
    
    error = ENS::startNMFComponent((t_cm_instance_handle)mNmfMain);
    if (error != OMX_ErrorNone) return error;

    error = startAlgo();
    if (error != OMX_ErrorNone) return error;
    
    return OMX_ErrorNone;
}


AFM_API_EXPORT OMX_ERRORTYPE NmfMpcPcmProcessing::stop(void) {
    OMX_ERRORTYPE error;
    
    error = ENS::stopNMFComponent((t_cm_instance_handle)mNmfMain);
    if (error != OMX_ErrorNone) return error;

    error = stopAlgo();
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}


AFM_API_EXPORT OMX_ERRORTYPE NmfMpcPcmProcessing::configure(void) {
    OMX_ERRORTYPE error;

    mIconfigure.setParameter(mConfig);

    error = configureAlgo();
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}


AFM_API_EXPORT OMX_ERRORTYPE NmfMpcPcmProcessing::deInstantiate(void) {
    OMX_ERRORTYPE error;

    error = ENS::unbindComponentAsynchronous((t_cm_instance_handle)mNmfMain, "me");
    if (error != OMX_ErrorNone) return error;
    
	error = ENS::unbindComponentFromHost(&mIconfigure);
    if (error != OMX_ErrorNone) return error;

    error = ENS::unbindComponent((t_cm_instance_handle)mNmfMain, "effect");
    if (error != OMX_ErrorNone) return error;

    if(mCallback == OMX_TRUE)
    {
        error = ENS::unbindComponentAsynchronous((t_cm_instance_handle)mNmfAlgo, "proxy");
        if (error != OMX_ErrorNone) return error;

        mCallback = OMX_FALSE;
    }

	error = ENS::unbindComponent((t_cm_instance_handle)mNmfMain, "synchronisation");
    if (error != OMX_ErrorNone) return error;

    error = ENS::destroyNMFComponent(mNmfSyncLib);
    if (error != OMX_ErrorNone) return error;

	error = ENS::unbindComponent((t_cm_instance_handle)mNmfMain, "osttrace");
    if (error != OMX_ErrorNone) return error;

    error = ENS::destroyNMFComponent(mOstTrace);
    if (error != OMX_ErrorNone) return error;

    error = ENS::destroyNMFComponent((t_cm_instance_handle)mNmfMain);
    if (error != OMX_ErrorNone) return error;
    
    error = deInstantiateAlgo();
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}

