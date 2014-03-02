/*
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*/

/*
* \file    Img_EnsWrapper_Rme.cpp
* \brief   Implement the ENS wrapper to RME interactions
*/


#include "Img_EnsWrapper_Shared.h"
#include "osal_semaphore.h"
#include "osal_mutex.h"
#include "osal_thread.h"

// RMe defines
#include "rme_types.h"
#include "rme.h"

//OMx definitions
#include "OMX_Component.h"

// ENS
#include "ImgEns_Component.h"

#include "Img_EnsQueue.h"
#include "Img_EnsWrapper_Rme.h"
#include "Img_EnsWrapper_Construct.h"

#include "Img_EnsWrapper.h"
#include "Img_EnsWrapper_Rme.h"

#define LOG_TAG "Img_EnsWrapper_Rme"
#include "linux_utils.h"

#define IMGWRAPPER_DEBUG_RME_PRINTF(...)                 \
	if (Img_EnsWrapper::RME_trace_enabled) {  \
	LOGD(__VA_ARGS__);                    \
	}

Img_EnsWrapper_Rme * Img_EnsWrapper_Rme::rmeWrapper = (Img_EnsWrapper_Rme *) NULL;

extern "C" {
	// RME thread function
	void RME_ThreadEntry(void * args) 
	{
		Img_EnsWrapper_Rme::RME_ThreadEntry(args);
	}
	// RMECallBack thread function
	void RMECallBack_ThreadEntry(void * args) 
	{
		Img_EnsWrapper_Rme::RMECallBack_ThreadEntry(args);
	}
}


Img_EnsWrapper_Rme::Img_EnsWrapper_Rme()
	: mCMP2RMEfreeMsgList  (Img_EnsQueue(IMG_RME_MESSAGE_ARRAY_SIZE))
	, pendingCMP2RMEmsgList(Img_EnsQueue(IMG_RME_MESSAGE_ARRAY_SIZE))
	, mRME2CMPfreeMsgList  (Img_EnsQueue(IMG_RME_MESSAGE_ARRAY_SIZE))
	, pendingRME2CMPmsgList(Img_EnsQueue(IMG_RME_MESSAGE_ARRAY_SIZE))
	, mRME                 ((RME *) NULL)
	, refCount             (0)
{
	IMGWRAPPER_DEBUG_RME_PRINTF("Creating Img_EnsWrapper_Rme 0x%08x\n", (unsigned int) this);

	for(int i=0;i<IMG_RME_MESSAGE_ARRAY_SIZE;i++) 
	{
		mCMP2RMEfreeMsgList.Push((void *) &(mCMP2RME_MessageArray[i]));
	}
	for(int i=0;i<IMG_RME_MESSAGE_ARRAY_SIZE;i++) 
	{
		mRME2CMPfreeMsgList.Push((void *) &(mRME2CMP_MessageArray[i]));
	}

	m_Semaphore_ComponentToRme = OSAL_SemaphoreCreate(0);
	RME2CMP_semaphore          = OSAL_SemaphoreCreate(0);

	m_Semaphore_RmeExit         = OSAL_SemaphoreCreate(0);
	m_Semaphore_RmeCallbackExit = OSAL_SemaphoreCreate(0);
	mRME = RME::getInstance();
}

Img_EnsWrapper_Rme::~Img_EnsWrapper_Rme()
{
	IMGWRAPPER_DEBUG_RME_PRINTF("Destroying Img_EnsWrapper_Rme\n");

	OSAL_SemaphoreDestroy(m_Semaphore_ComponentToRme);
	OSAL_SemaphoreDestroy(RME2CMP_semaphore);
	OSAL_SemaphoreDestroy(m_Semaphore_RmeExit);
	OSAL_SemaphoreDestroy(m_Semaphore_RmeCallbackExit);

	RME::delInstance();
}

void Img_EnsWrapper_Rme::startRMEthread()
{
	IMGWRAPPER_DEBUG_RME_PRINTF("Img_EnsWrapper_Rme::startRMEthread\n");
	OSAL_ThreadCreate(RME_ThreadEntry, NULL,
		IMG_RME_THREAD_STACK_SIZE, OSAL_THREAD_NORMAL_PRIORITY,
		"ENS-RME");
}

void Img_EnsWrapper_Rme::startRMECallBackThread()
{
	IMGWRAPPER_DEBUG_RME_PRINTF("Img_EnsWrapper_Rme::startRMECallBackThread\n");
	OSAL_ThreadCreate(RMECallBack_ThreadEntry, NULL,
		IMG_RME_THREAD_STACK_SIZE, OSAL_THREAD_NORMAL_PRIORITY,
		"ENS-RME-CB");
}

Img_EnsWrapper_Rme * Img_EnsWrapper_Rme::getInstance()
{
	if(rmeWrapper == NULL) {
		rmeWrapper = new Img_EnsWrapper_Rme();
		rmeWrapper->startRMEthread();
		rmeWrapper->startRMECallBackThread();
	}
	IMGWRAPPER_DEBUG_RME_PRINTF("Img_EnsWrapper_Rme::getInstance returning 0x%08x\n", (unsigned int) rmeWrapper);
	return rmeWrapper;
}

void Img_EnsWrapper_Rme::delInstance()
{
	if(rmeWrapper != NULL) {
		IMGWRAPPER_DEBUG_RME_PRINTF("Img_EnsWrapper_Rme::delInstance 0x%08x\n", (unsigned int) rmeWrapper);
		delete rmeWrapper;
		rmeWrapper = (Img_EnsWrapper_Rme *)NULL;
	}
}

/********************************************************************************/
// CMP to RME queue

CMP2RME_Message * Img_EnsWrapper_Rme::getFreeCMP2RMEmsg()
{
	return (CMP2RME_Message *) mCMP2RMEfreeMsgList.Pop();
}

void Img_EnsWrapper_Rme::putFreeCMP2RMEmsg(CMP2RME_Message * msg)
{
	mCMP2RMEfreeMsgList.Push(msg);
}

void Img_EnsWrapper_Rme::postCMP2RMEmsg(CMP2RME_Message * msg)
{
	pendingCMP2RMEmsgList.Push(msg);
	// We always notify the semaphore of new message
	OSAL_SemaphoreNotify(m_Semaphore_ComponentToRme);
}

CMP2RME_Message * Img_EnsWrapper_Rme::popCMP2RMEmsg()
{
	return (CMP2RME_Message *) pendingCMP2RMEmsgList.Pop();
}

/********************************************************************************/
// RME to CMP queue

RME2CMP_Message * Img_EnsWrapper_Rme::getFreeRME2CMPmsg()
{
	return (RME2CMP_Message *) mRME2CMPfreeMsgList.Pop();
}
void Img_EnsWrapper_Rme::putFreeRME2CMPmsg(RME2CMP_Message * msg)
{
	mRME2CMPfreeMsgList.Push(msg);
}

void Img_EnsWrapper_Rme::postRME2CMPmsg(RME2CMP_Message * msg)
{
	pendingRME2CMPmsgList.Push(msg);
	// We always notify the semaphore of new message
	OSAL_SemaphoreNotify(RME2CMP_semaphore);
}

RME2CMP_Message * Img_EnsWrapper_Rme::popRME2CMPmsg()
{
	return (RME2CMP_Message *) pendingRME2CMPmsgList.Pop();
}

/********************************************************************************/
// RME thread

void Img_EnsWrapper_Rme::RME_ThreadEntry(void *)
{
	int exitRequested = 0;
	long rme_timeout  = 0;
	int timerExpired  = 0;
	int ret=0;

	IMGWRAPPER_DEBUG_RME_PRINTF("RME_Thread - starting\n");

	do {
		timerExpired = 0;
		if (rme_timeout>0) 
		{
			// Wait for a message to process or timer expiration
			IMGWRAPPER_DEBUG_RME_PRINTF("RME_Thread waiting message or timer expiration\n");
			ret=OSAL_SemaphoreWaitTimeOut(rmeWrapper->m_Semaphore_ComponentToRme,rme_timeout);
			if (ret<0){
				// Timer has expired
				timerExpired = 1;
			}
		} 
		else 
		{
			// Wait for a message to process
			IMGWRAPPER_DEBUG_RME_PRINTF("RME_Thread waiting message\n");
			OSAL_SemaphoreWait(rmeWrapper->m_Semaphore_ComponentToRme);
		}

		if (!timerExpired) 
		{
			// A message has been received
			rme_timeout = 0;
			// Retrieve the message
			CMP2RME_Message * msg = Img_EnsWrapper_Rme::getInstance()->popCMP2RMEmsg();
			IMG_DBC_ASSERT(msg != NULL);

			// Treat the message
			switch(msg->type) 
			{
		case MSG_CMP2RME_Notify_NewStatesOMXRES:
			IMGWRAPPER_DEBUG_RME_PRINTF("RME_Thread executing Notify_NewStatesOMXRES() - ID=%d\n",
				(unsigned int) msg->pCompHdl);
			rmeWrapper->mRME->Notify_NewStatesOMXRES(msg->pCompHdl,
				msg->args.sNotify_NewStatesOMXRES.eOMXILState,
				msg->args.sNotify_NewStatesOMXRES.eResourceState,
				msg->args.sNotify_NewStatesOMXRES.nCounter);
			break;
		case MSG_CMP2RME_Notify_NewPolicyCfg:
			IMGWRAPPER_DEBUG_RME_PRINTF("RME_Thread executing Notify_NewPolicyCfg() - ID=%d\n",
				(unsigned int) msg->pCompHdl);
			rmeWrapper->mRME->Notify_NewPolicyCfg(msg->pCompHdl,
				msg->args.sNotify_NewPolicyCfg.nGroupPriority,
				msg->args.sNotify_NewPolicyCfg.nGroupID,
				msg->args.sNotify_NewPolicyCfg.eQoSbitmap);
			break;
		case MSG_CMP2RME_Notify_RMConfigApplied:
			IMGWRAPPER_DEBUG_RME_PRINTF("RME_Thread executing Notify_RMConfigApplied() - ID=%d\n",
				(unsigned int) msg->pCompHdl);
			rmeWrapper->mRME->Notify_RMConfigApplied(msg->pCompHdl,
				msg->args.sNotify_RMConfigApplied.eType,
				msg->args.sNotify_RMConfigApplied.nData,
				msg->args.sNotify_RMConfigApplied.nErrorCode);
			break;
		case MSG_CMP2RME_Notify_Tunnel:
			IMGWRAPPER_DEBUG_RME_PRINTF("RME_Thread executing Notify_Tunnel() - outID=%d inID=%d\n",
				(unsigned int) msg->pCompHdl,
				(unsigned int) msg->args.sNotify_Tunnel.pCompHdlIn);
			rmeWrapper->mRME->Notify_Tunnel(msg->pCompHdl,
				msg->args.sNotify_Tunnel.nPortIndexOut,
				msg->args.sNotify_Tunnel.pCompHdlIn,
				msg->args.sNotify_Tunnel.nPortIndexIn);
			break;
		case MSG_CMP2RME_Notify_ESRAM_Sharing:
#ifdef ALP_SUPPORT_MIGRATIONDOMAINS
			IMGWRAPPER_DEBUG_RME_PRINTF("RME_Thread executing Notify_ESRAM_Sharing() - ID=%d\n",
				(unsigned int) msg->pCompHdl);
			rmeWrapper->mRME->Notify_ESRAM_Sharing(msg->args.sNotify_ESRAM_Sharing.
				bEsramResourceRequired);
#endif
			break;
		case MSG_CMP2RME_Request_Thread_Exit:
			IMGWRAPPER_DEBUG_RME_PRINTF("RME_Thread got MSG_CMP2RME_Request_Thread_Exit\n");
			exitRequested = 1;
			break;
		default:
			IMG_DBC_ASSERT(0);
			}

			// Put back message container as available
			Img_EnsWrapper_Rme::getInstance()->putFreeCMP2RMEmsg(msg);

			// In case queue is empty call IdleT
			if(rmeWrapper->getNbPendingCMP2RME() == 0) {
				IMGWRAPPER_DEBUG_RME_PRINTF("RME_Thread executing idleT - cause is empty queue\n");
				rme_timeout = rmeWrapper->mRME->idleT();
			}
		} else {
			// Timer has expired : call idleT
			IMGWRAPPER_DEBUG_RME_PRINTF("RME_Thread executing idleT - cause is timeout reached\n");
			rme_timeout = rmeWrapper->mRME->idleT();
		}

	} while(!exitRequested);

	RME2CMP_Message * RME2CMPmsg = rmeWrapper->getFreeRME2CMPmsg();
	IMG_DBC_ASSERT(RME2CMPmsg != NULL);
	RME2CMPmsg->type = MSG_RME2CMP_Request_Thread_Exit;
	rmeWrapper->postRME2CMPmsg(RME2CMPmsg);

	OSAL_SemaphoreNotify(rmeWrapper->m_Semaphore_RmeExit);
	IMGWRAPPER_DEBUG_RME_PRINTF("RME_Thread - exiting\n");
}

/********************************************************************************/
// RMECallBack thread

void Img_EnsWrapper_Rme::RMECallBack_ThreadEntry(void *)
{
	int exitRequested = 0;
	Img_EnsWrapper * wrapper;
	RME2CMP_Message * msg;

	IMGWRAPPER_DEBUG_RME_PRINTF("RMECallBack_Thread - starting\n");

	do {
		// Wait for a message to process
		IMGWRAPPER_DEBUG_RME_PRINTF("RMECallBack_Thread waiting\n");

		OSAL_SemaphoreWait(rmeWrapper->RME2CMP_semaphore);

		msg = Img_EnsWrapper_Rme::getInstance()->popRME2CMPmsg();

		IMGWRAPPER_DEBUG_RME_PRINTF("RMECallBack_Thread got message\n");

		IMG_DBC_ASSERT(msg != NULL);

		wrapper = Img_EnsWrapper::GetWrapperForID((unsigned long) msg->pCompHdl);

		if(msg->type == MSG_RME2CMP_Request_Thread_Exit) 
		{
			IMGWRAPPER_DEBUG_RME_PRINTF("RMECallBack_Thread MSG_RME2CMP_Request_Thread_Exit\n");
			exitRequested = 1;
		} 
		else 
		{
			if(wrapper) 
			{
				switch(msg->type) 
				{
				case MSG_RME2CMP_CP_Notify_Error:
					IMGWRAPPER_DEBUG_RME_PRINTF("RMECallBack_Thread MSG_RME2CMP_CP_Notify_Error - ID=%ld\n", (unsigned long) msg->pCompHdl);
					wrapper->RME2CMP_Notify_Error(wrapper->getOMX_Component(),
						msg->args.CP_Notify_Error.nError);
					break;

				case MSG_RME2CMP_CP_Notify_ResReservationProcessed:
					IMGWRAPPER_DEBUG_RME_PRINTF("RMECallBack_Thread MSG_RME2CMP_CP_Notify_ResReservationProcessed - ID=%ld\n", (unsigned long) msg->pCompHdl);
					wrapper->RME2CMP_Notify_ResReservationProcessed(wrapper->getOMX_Component(), msg->args.CP_Notify_ResReservationProcessed.bResourcesReservGranted);
					break;

				case MSG_RME2CMP_CP_Cmd_SetRMConfig:
					IMGWRAPPER_DEBUG_RME_PRINTF("RMECallBack_Thread MSG_RME2CMP_CP_Notify_ResReservationProcessed - ID=%ld\n", (unsigned long) msg->pCompHdl);
					wrapper->RME2CMP_Cmd_SetRMConfig(wrapper->getOMX_Component(), &(msg->args.CP_Cmd_SetRMConfig.sRMcfg));
					break;

				case MSG_RME2CMP_CP_Cmd_ReleaseResource:
					IMGWRAPPER_DEBUG_RME_PRINTF("RMECallBack_Thread MSG_RME2CMP_CP_Cmd_ReleaseResource - ID=%ld\n",
						(unsigned long) msg->pCompHdl);
					wrapper->RME2CMP_Cmd_ReleaseResource(wrapper->getOMX_Component(),
						msg->args.CP_Cmd_ReleaseResource.bSuspend);
					break;

				default:
					IMG_DBC_ASSERT(0);
				}
			} 
			else 
			{
				const char * msg_name;
				switch(msg->type) 
				{
				case MSG_RME2CMP_CP_Notify_Error:                   msg_name = "Notify_Error"; break;
				case MSG_RME2CMP_CP_Notify_ResReservationProcessed: msg_name = "ResReservationProcessed"; break;
				case MSG_RME2CMP_CP_Cmd_SetRMConfig:                msg_name = "Cmd_SetRMConfig"; break;
				case MSG_RME2CMP_CP_Cmd_ReleaseResource:            msg_name = "Cmd_ReleaseResource"; break;
				default: msg_name = "Unknown";
				}
				LOGI("RMECallBack_Thread ignoring RME message '%s' on non active component ID=%ld\n", msg_name, (unsigned long) msg->pCompHdl);
			}
		}
		// Put back message container as available
		Img_EnsWrapper_Rme::getInstance()->putFreeRME2CMPmsg(msg);
	} while(!exitRequested);

	// We notify the exit semaphore to notify we are exiting
	OSAL_SemaphoreNotify(rmeWrapper->m_Semaphore_RmeCallbackExit);

	IMGWRAPPER_DEBUG_RME_PRINTF("RMECallBack_Thread - exiting\n");
}

/********************************************************************************
* Synchronous calls to RME from ENS (called from ImgEns_HwRm class implementation)
********************************************************************************/

RM_STATUS_E Img_EnsWrapper_Rme::RME_Register(
	OMX_IN const OMX_PTR pCompHdl,
	OMX_IN const RM_CBK_T* pCompCbks)
{
	return mRME->Register(pCompHdl, pCompCbks);
}


RM_STATUS_E Img_EnsWrapper_Rme::RME_Unregister(OMX_IN const OMX_PTR pCompHdl)
{
	return mRME->Unregister(pCompHdl);
}


OMX_U32 Img_EnsWrapper_Rme::RME_Get_DefaultDDRDomain(OMX_IN RM_NMFD_E nDomainType)
{
	return mRME->Get_DefaultDDRDomain(nDomainType);
}

