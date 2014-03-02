/*
* Copyright (C) ST-Ericsson SA 2011. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ImgEns_IOmx.h"
#include "BSD_list.h"
#include "osal_semaphore.h"
#include "osal_mutex.h"
#include "osal_thread.h"

//Omx definitions
#include "OMX_Component.h"

// ENS
#include "ImgEns_Component.h"
#include "ImgEns_Index.h"

#include "Img_EnsWrapper_Shared.h"
#include "Img_EnsQueue.h"
#include "Img_EnsWrapper.h"
#include "Img_EnsWrapper_Log.h"
#include "Img_EnsWrapper_Construct.h"

/**
* Function called from the ENS Component FSM to build the NMF graph.
* This call meant to delgate to a dedicated thread the time consuming graph build.
* Wrapper lock is held when this called from ENS.
*/
void ConstructENSProcessingPartAsync(OMX_HANDLETYPE hComponent)
{
	OMX_COMPONENTTYPE *omx_comp = (OMX_COMPONENTTYPE *)hComponent;
	Img_EnsWrapper * wrapper    = (Img_EnsWrapper *)omx_comp->pComponentPrivate;

	wrapper->getWrapperConstruct()->ENSProcessingPartAsync(hComponent);
}

OMX_ERRORTYPE Img_EnsWrapper_Construct::ENSProcessingPartAsync(OMX_HANDLETYPE hComponent)
{
	if (hComponent == NULL)
	{
		LOGE("Img_EnsWrapper_Construct::ENSProcessingPartAsync called with NULL handle !\n");
		return OMX_ErrorBadParameter;
	}
	if (mQueue.Push(hComponent) == 0)
	{
		IMGWRAPPER_OMX_LOG("Img_EnsWrapper_Construct::ENSProcessingPartAsync - ok\n");
		OSAL_SemaphoreNotify(m_Semaphore_Process);
		return OMX_ErrorNone;
	}
	LOGE("Img_EnsWrapper_Construct::ENSProcessingPartAsync failed to push\n");
	return OMX_ErrorInsufficientResources;
}


extern "C" {
	t_osal_thread_return_type ImgEns_WrapperConstructThreadEntry(void *args)
	{ // Entry point for main ImgEns thread
		Img_EnsWrapper_Construct::Thread((Img_EnsWrapper_Construct *)args);
#ifdef _MSC_VER
		return(0);
#endif
	}
}

Img_EnsWrapper_Construct::Img_EnsWrapper_Construct()
: m_Semaphore_Process(OSAL_SemaphoreCreate(0))
, m_Semaphore_Exit(OSAL_SemaphoreCreate(0))
, mQueue(Img_EnsQueue(128))
{
	IMGWRAPPER_OMX_LOG("Starting ImgEns_WrapperConstructThread\n");
	OSAL_ThreadCreate(ImgEns_WrapperConstructThreadEntry, (void *)this, 0, OSAL_THREAD_NORMAL_PRIORITY, "ENS-Construct");
}

Img_EnsWrapper_Construct::~Img_EnsWrapper_Construct()
{
	// Send the stop thread message
	mQueue.Push(NULL);
	// Wait for thread to exit
	OSAL_SemaphoreNotify(m_Semaphore_Process); // ADDED BY MOMO
	OSAL_SemaphoreWait(m_Semaphore_Exit);
	// Free allocated stuffs
	OSAL_SemaphoreDestroy(m_Semaphore_Process);
	OSAL_SemaphoreDestroy(m_Semaphore_Exit);
}

void Img_EnsWrapper_Construct::Thread(Img_EnsWrapper_Construct *instance)
{
	IMGWRAPPER_OMX_LOG("Img_EnsWrapper_Construct::Thread started\n");
	OMX_HANDLETYPE hComponent = NULL;
	do {
		OSAL_SemaphoreWait(instance->m_Semaphore_Process);
		hComponent = (OMX_HANDLETYPE)instance->mQueue.Pop();
		if (hComponent != NULL)
		{
			OMX_COMPONENTTYPE *omx_comp = (OMX_COMPONENTTYPE *)hComponent;
			Img_EnsWrapper * wrapper = (Img_EnsWrapper *)omx_comp->pComponentPrivate;

			IMGWRAPPER_OMX_LOG("Img_EnsWrapper_Construct::Thread - construct %s hComponent=0x%08x",
				(const char*) wrapper->getImgEns_Component()->getName(),
				(unsigned int) wrapper->getOMX_Component());

			OMX_ERRORTYPE error = wrapper->getImgEns_Component()->construct();

			IMGWRAPPER_OMX_LOG("Img_EnsWrapper_Construct::Thread - construct %s hComponent=0x%08x",
				(const char*) wrapper->getImgEns_Component()->getName(),
				(unsigned int) wrapper->getOMX_Component());

			if (error != OMX_ErrorNone)
			{
				wrapper->getImgEns_Component()->eventHandlerCB(OMX_EventError, error, 0, 0);
			}
		}
	} while (hComponent != NULL);

	OSAL_SemaphoreNotify(instance->m_Semaphore_Exit);

	IMGWRAPPER_OMX_LOG("Img_EnsWrapper_Construct::Thread exiting\n");
}

