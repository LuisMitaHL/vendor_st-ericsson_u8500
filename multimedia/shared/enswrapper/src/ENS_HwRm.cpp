/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * \file    ENS_HwRm.cpp
 * \brief   Implement the Hardware Ressource management API used by the ENS components
 */


#include "ENS_HwRm.h"
#include "ENS_RME_Wrapper.h"
#include "ENS_Wrapper.h"
#include "ENS_DBC.h"

#define LOG_TAG "ENS_RME"
#include "linux_utils.h"

#define DEBUG_RME_PRINTF(...)			\
    if (ENS_Wrapper::RME_trace_enabled) {	\
	ALOGD(__VA_ARGS__);			\
    }

/********************************************************************************
 * Synchronous calls from ENS to RME
 ********************************************************************************/

RM_STATUS_E ENS_HwRm::HWRM_Register(const OMX_HANDLETYPE comphdl,
				    const RM_CBK_T *pcbks)
{
    /*
     * Warning !! Context is not holding wrapper global lock when calling this function !!
     * 
     * Called from ENS_Component::setOMXHandleAndRegisterRm direclty from the 
     * client thread by the OMX Core loader.
     */ 
    OMX_COMPONENTTYPE *omx_comp = (OMX_COMPONENTTYPE *) comphdl;
    ENS_Wrapper_p wrapper = (ENS_Wrapper_p) omx_comp->pComponentPrivate;

    DEBUG_RME_PRINTF("HWRM_Register executing RME - ID=%ld, h=0x%08x\n",
		     wrapper->mID, (unsigned int) comphdl);
    
    /* To ensure getInstance is called by only one thread and that
       refCount increment is atomic, acquire wrapper global lock */
    WRAPPER_LOCK(omx_comp);
    
    ENS_RME_Wrapper * rmeWrapper = ENS_RME_Wrapper::getInstance();
    rmeWrapper->refCount++;

    WRAPPER_UNLOCK(omx_comp);

    wrapper->setRMEcbks(pcbks);
       
    // We register to the RME using the unique ID as comphdl may have the same value
    // even if dealing with a different component instance (previous instance being deleted
    // but a new one allocated on the same address location)
    return rmeWrapper->RME_Register((OMX_PTR) wrapper->mID, wrapper->getRMEwrappercbks());
}

RM_STATUS_E ENS_HwRm::HWRM_Unregister(const OMX_PTR comphdl)
{
    RM_STATUS_E error;
    OMX_COMPONENTTYPE *omx_comp = (OMX_COMPONENTTYPE *) comphdl;
    ENS_Wrapper_p wrapper = (ENS_Wrapper_p) omx_comp->pComponentPrivate; 

    DEBUG_RME_PRINTF("HWRM_Unregister executing RME - ID=%ld, h=0x%08x\n",
		     wrapper->mID, (unsigned int) comphdl);
    
    ENS_RME_Wrapper * rmeWrapper = ENS_RME_Wrapper::getInstance();
    rmeWrapper->refCount--;

    // Release wrapper global lock as:
    //   + RME is processing Async messages in the RME thread
    //   + it maybe waiting on Wrapper lock to call Component RME CB
    //   + this sync will make the RME call a internal lock maybe already held by RME CB thread
    WRAPPER_UNLOCK(omx_comp);

    error = rmeWrapper->RME_Unregister((OMX_PTR) wrapper->mID);

    // Getting back the lock
    WRAPPER_LOCK(omx_comp);

    // When we return from this HWRM_Unregister function, the ENS_Component will be
    // destroyed so we remove the component from the list of active/valid components
    ENS_Wrapper::RemoveHandleFromActiveList((OMX_COMPONENTTYPE *) comphdl);
    
    // if we have unregistered last OMX component, kill the RME wrapper
    if(rmeWrapper->refCount == 0) {
	DEBUG_RME_PRINTF("HWRM_Unregister - ID=%ld, h=0x%08x - unregistered last component"
			 " -> killing RME wrapper\n",
			 wrapper->mID, (unsigned int) comphdl);
	
	// Post a message to the RME thread to request exit
	CMP2RME_Message * msg = rmeWrapper->getFreeCMP2RMEmsg();
	DBC_ASSERT(msg != NULL);
        DEBUG_RME_PRINTF("Sending MSG_CMP2RME_Request_Thread_Exit to RME_Thread");
	msg->type = MSG_CMP2RME_Request_Thread_Exit;
	rmeWrapper->postCMP2RMEmsg(msg);

	// Wait for the RME thread to notify it has exited
        DEBUG_RME_PRINTF("Waiting for ENS-RME thread to exit");
	LOS_SemaphoreWait(rmeWrapper->RME_exit_sem);
        DEBUG_RME_PRINTF("Got RME_Thread exit notification");
        DEBUG_RME_PRINTF("Waiting for ENS-RME-CB thread to exit");
	LOS_SemaphoreWait(rmeWrapper->RMECallBack_exit_sem);

	// CleanUP RME and associated resources
	ENS_RME_Wrapper::delInstance();
    }
    return error;
}


OMX_U32 ENS_HwRm::HWRM_Get_DefaultDDRDomain(OMX_IN RM_NMFD_E nDomainType)
{
    DEBUG_RME_PRINTF("HWRM_Get_DefaultDDRDomain(%d)\n", (int) nDomainType);
    ENS_RME_Wrapper * rmeWrapper = ENS_RME_Wrapper::getInstance();
    return rmeWrapper->RME_Get_DefaultDDRDomain(nDomainType);
}

/********************************************************************************
 * Asynchronous calls from ENS to RME
 * -- We send the wrapper ID to the RME --
 ********************************************************************************/

OMX_ERRORTYPE ENS_HwRm::HWRM_Notify_NewStatesOMXRES(OMX_IN const OMX_PTR pCompHdl, 
						    OMX_IN OMX_STATETYPE eOMXILState,
						    OMX_IN RM_RESOURCESTATE_E eResourceState,
						    OMX_IN OMX_U32 nCounter)
{
    ENS_RME_Wrapper * rmeWrapper = ENS_RME_Wrapper::getInstance();    
    ENS_Wrapper_p wrapper = (ENS_Wrapper_p) ((OMX_COMPONENTTYPE *) pCompHdl)->pComponentPrivate; 
    
    DEBUG_RME_PRINTF("Posting HWRM_Notify_NewStatesOMXRES() - ID=%d, h=0x%08x\n",
		     (unsigned int) wrapper->mID, (unsigned int) pCompHdl);
    
    CMP2RME_Message * msg = rmeWrapper->getFreeCMP2RMEmsg();
    if(msg == NULL)
	return OMX_ErrorNone;
    
    msg->type = MSG_CMP2RME_Notify_NewStatesOMXRES;
    msg->pCompHdl = (OMX_PTR) wrapper->mID;
    msg->args.sNotify_NewStatesOMXRES.eOMXILState = eOMXILState;
    msg->args.sNotify_NewStatesOMXRES.eResourceState = eResourceState;
    msg->args.sNotify_NewStatesOMXRES.nCounter = nCounter;
    
    rmeWrapper->postCMP2RMEmsg(msg);
    
    return OMX_ErrorNone;
}


OMX_ERRORTYPE ENS_HwRm::HWRM_Notify_NewPolicyCfg(OMX_IN const OMX_PTR pCompHdl,
						 OMX_IN OMX_U32 nGroupPriority,
						 OMX_IN OMX_U32 nGroupID, 
						 OMX_IN OMX_U32 eQoSbitmap)
{
    ENS_RME_Wrapper * rmeWrapper = ENS_RME_Wrapper::getInstance();    
    ENS_Wrapper_p wrapper = (ENS_Wrapper_p) ((OMX_COMPONENTTYPE *) pCompHdl)->pComponentPrivate; 

    DEBUG_RME_PRINTF("Posting HWRM_Notify_NewPolicyCfg() - ID=%d, h=0x%08x\n",
		     (unsigned int) wrapper->mID, (unsigned int) pCompHdl);
    
    CMP2RME_Message * msg = rmeWrapper->getFreeCMP2RMEmsg();
    if(msg == NULL)
	return OMX_ErrorNone;
    
    msg->type = MSG_CMP2RME_Notify_NewPolicyCfg;
    msg->pCompHdl = (OMX_PTR) wrapper->mID;
    msg->args.sNotify_NewPolicyCfg.nGroupPriority = nGroupPriority;
    msg->args.sNotify_NewPolicyCfg.nGroupID = nGroupID;
    msg->args.sNotify_NewPolicyCfg.eQoSbitmap = eQoSbitmap;
    
    rmeWrapper->postCMP2RMEmsg(msg);
    
    return OMX_ErrorNone;
}


OMX_ERRORTYPE ENS_HwRm::HWRM_Notify_RMConfigApplied(OMX_IN const OMX_PTR pCompHdl, 
						    OMX_IN RM_RC_E eType, 
						    OMX_IN OMX_U32 nData,
						    OMX_IN RM_STATUS_E nErrorCode)
{
    ENS_RME_Wrapper * rmeWrapper = ENS_RME_Wrapper::getInstance();    
    ENS_Wrapper_p wrapper = (ENS_Wrapper_p) ((OMX_COMPONENTTYPE *) pCompHdl)->pComponentPrivate; 
    
    DEBUG_RME_PRINTF("Posting HWRM_Notify_RMConfigApplied() - ID=%d, h=0x%08x\n",
		     (unsigned int) wrapper->mID, (unsigned int) pCompHdl);
    
    CMP2RME_Message * msg = rmeWrapper->getFreeCMP2RMEmsg();
    if(msg == NULL)
	return OMX_ErrorNone;
    
    msg->type = MSG_CMP2RME_Notify_RMConfigApplied;
    msg->pCompHdl = (OMX_PTR) wrapper->mID;
    msg->args.sNotify_RMConfigApplied.eType = eType;
    msg->args.sNotify_RMConfigApplied.nData = nData;
    msg->args.sNotify_RMConfigApplied.nErrorCode = nErrorCode;
    
    rmeWrapper->postCMP2RMEmsg(msg);
    
    return OMX_ErrorNone;
}

OMX_ERRORTYPE ENS_HwRm::HWRM_Notify_Tunnel(OMX_IN const OMX_PTR pCompHdlOut,
					   OMX_IN OMX_U32 nPortIndexOut,
					   OMX_IN const OMX_PTR pCompHdlIn,
					   OMX_IN OMX_U32 nPortIndexIn)
{
    ENS_RME_Wrapper * rmeWrapper = ENS_RME_Wrapper::getInstance();
    ENS_Wrapper_p wrapperOut = (pCompHdlOut ? (ENS_Wrapper_p) ((OMX_COMPONENTTYPE *) pCompHdlOut)->pComponentPrivate : (ENS_Wrapper_p) NULL);
    ENS_Wrapper_p wrapperIn  = (pCompHdlIn  ? (ENS_Wrapper_p) ((OMX_COMPONENTTYPE *) pCompHdlIn)->pComponentPrivate  : (ENS_Wrapper_p) NULL); 
    
    DEBUG_RME_PRINTF("Posting HWRM_Notify_Tunnel() - IDout=%d, hOut=0x%08x - IDin=%d, hIn=0x%08x\n",
		     (unsigned int) (wrapperOut ? wrapperOut->mID : 0), (unsigned int) pCompHdlOut,
		     (unsigned int) (wrapperIn ? wrapperIn->mID : 0), (unsigned int) pCompHdlIn);
    
    CMP2RME_Message * msg = rmeWrapper->getFreeCMP2RMEmsg();
    if(msg == NULL)
	return OMX_ErrorNone;
    
    msg->type = MSG_CMP2RME_Notify_Tunnel;
    msg->pCompHdl = (OMX_PTR) (wrapperOut ? wrapperOut->mID : 0);
    msg->args.sNotify_Tunnel.nPortIndexOut = nPortIndexOut;
    msg->args.sNotify_Tunnel.pCompHdlIn = (OMX_PTR) (wrapperIn ? wrapperIn->mID : 0);
    msg->args.sNotify_Tunnel.nPortIndexIn = nPortIndexIn;
    
    rmeWrapper->postCMP2RMEmsg(msg);
    
    return OMX_ErrorNone;
}

OMX_ERRORTYPE ENS_HwRm::HWRM_Notify_ESRAM_Sharing(OMX_IN OMX_BOOL bEsramResourceRequired)
{
    ENS_RME_Wrapper * rmeWrapper = ENS_RME_Wrapper::getInstance();    
    
    DEBUG_RME_PRINTF("Posting HWRM_Notify_ESRAM_Sharing()\n");
    
    CMP2RME_Message * msg = rmeWrapper->getFreeCMP2RMEmsg();
    if(msg == NULL)
	return OMX_ErrorNone;
    
    msg->type = MSG_CMP2RME_Notify_ESRAM_Sharing;
    msg->args.sNotify_ESRAM_Sharing.bEsramResourceRequired = bEsramResourceRequired;
    
    rmeWrapper->postCMP2RMEmsg(msg);
    
    return OMX_ErrorNone;
}
