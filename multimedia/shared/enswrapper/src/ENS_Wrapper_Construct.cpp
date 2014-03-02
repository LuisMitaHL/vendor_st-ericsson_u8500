/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "ENS_WrapperLog.h"
#include "ENS_Wrapper.h"
#include "ENS_Wrapper_Construct.h"
#include "ProcessingComponent.h"

/**
 * Function called from the ENS Component FSM to build the NMF graph.
 * This call meant to delgate to a dedicated thread the time consuming graph build.
 * Wrapper lock is held when this called from ENS.
 */
void ConstructENSProcessingPartAsync(OMX_HANDLETYPE hComponent)
{
    OMX_COMPONENTTYPE *omx_comp = (OMX_COMPONENTTYPE *)hComponent;
    ENS_Wrapper_p wrapper = (ENS_Wrapper_p)omx_comp->pComponentPrivate;

    wrapper->getWrapperConstruct()->ENSProcessingPartAsync(hComponent);
}

OMX_ERRORTYPE ENS_WrapperConstruct::ENSProcessingPartAsync(OMX_HANDLETYPE hComponent)
{
    if (hComponent == NULL) {
        ALOGE("ENS_WrapperConstruct::ENSProcessingPartAsync called with NULL handle !\n");
        return OMX_ErrorBadParameter;
    }
    if (mQueue.Push(hComponent) == 0) {
        WRAPPER_OMX_LOG(hComponent, "ENS_WrapperConstruct::ENSProcessingPartAsync - ok\n");
        LOS_SemaphoreNotify(mSem);
        return OMX_ErrorNone;
    }
    ALOGE("ENS_WrapperConstruct::ENSProcessingPartAsync failed to push\n");
    return OMX_ErrorInsufficientResources;
}

extern "C" {
    void ENS_WrapperConstructThreadEntry(void *args) {
        ENS_WrapperConstruct::Thread((ENS_WrapperConstruct *)args);
    }
}

ENS_WrapperConstruct::ENS_WrapperConstruct()
    : mSem(LOS_SemaphoreCreate(0)), mExitSem(LOS_SemaphoreCreate(0)), mQueue(ENS_Queue(128))
{
    WRAPPER_OMX_LOG(NULL, "Starting ENS_WrapperConstructThread\n");
    LOS_ThreadCreate(ENS_WrapperConstructThreadEntry, (void *)this,
                      0, LOS_USER_NORMAL_PRIORITY, "ENS-Construct");
}

ENS_WrapperConstruct::~ENS_WrapperConstruct()
{
    // Send the stop thread message
    mQueue.Push(NULL);
    LOS_SemaphoreNotify(mSem);
    // Wait for thread to exit
    LOS_SemaphoreWait(mExitSem);
    // Free allocated stuffs
    LOS_SemaphoreDestroy(mSem);
    LOS_SemaphoreDestroy(mExitSem);
}

void ENS_WrapperConstruct::Thread(ENS_WrapperConstruct *instance)
{
    WRAPPER_OMX_LOG(NULL, "ENS_WrapperConstruct::Thread started\n");

    OMX_COMPONENTTYPE *omx_comp = NULL;

    do {
        LOS_SemaphoreWait(instance->mSem);

        omx_comp = (OMX_COMPONENTTYPE *)instance->mQueue.Pop();

        if (omx_comp != NULL) {
            ENS_Wrapper_p wrapper = (ENS_Wrapper_p)omx_comp->pComponentPrivate;

            WRAPPER_OMX_LOG(omx_comp,
                            "ENS_WrapperConstruct::Thread - Start construct %s hComponent=0x%08x",
                            (const char*) wrapper->getENS_Component()->getName(),
                            (unsigned int)omx_comp);

            OMX_ERRORTYPE error = OMX_ErrorInvalidComponent;
            ProcessingComponent * pProcComponent = (ProcessingComponent *) wrapper->getENS_Component()->GetProcessingComponent();
            if (pProcComponent) {
                error = pProcComponent->construct();
            }

            WRAPPER_OMX_LOG(omx_comp,
                            "ENS_WrapperConstruct::Thread - Finished construct %s hComponent=0x%08x",
                            (const char*) wrapper->getENS_Component()->getName(), (unsigned int)omx_comp);

            if (error != OMX_ErrorNone) {
                wrapper->getENS_Component()->eventHandlerCB(OMX_EventError, error, 0, 0);
            }
        }
    } while (omx_comp != NULL);

    LOS_SemaphoreNotify(instance->mExitSem);

    WRAPPER_OMX_LOG(NULL, "ENS_WrapperConstruct::Thread exiting\n");
}

