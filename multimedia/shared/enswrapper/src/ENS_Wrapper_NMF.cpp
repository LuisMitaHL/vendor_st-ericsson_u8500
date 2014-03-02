/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdlib.h>

#include <cpp.hpp>
#include <ee/api/ee.h>

#include <ENS_DBC.h>

#include "OMX_Wrapper.h"
#include "ENS_WrapperLog.h"

t_nmf_channel ENS_Wrapper::mNMF_DSP_CallBackChannel;
t_nmf_channel ENS_Wrapper::mNMF_SMPEE_CallBackChannel;
t_los_sem_id ENS_Wrapper::waitingForMpcFlushMessage_sem;
t_los_sem_id ENS_Wrapper::waitingForHostFlushMessage_sem;

int ENS_Wrapper::NMF_STM_ARM_trace_level = 0;
int ENS_Wrapper::NMF_STM_SIA_trace_level = 0;
int ENS_Wrapper::NMF_STM_SVA_trace_level = 0;

#ifdef ANDROID
#define STE_NMF_AUTOIDLE_PROPERTY_NAME "ste.nmf.autoidle"
#else
#define STE_NMF_AUTOIDLE_PROPERTY_NAME "ste_nmf_autoidle"
#endif

extern "C" {
#ifndef HOST_ONLY
    // NMF on DSP Call Back thread function
    void NMF_DSP_CallBackThread(void * args) {
        ENS_Wrapper::NMF_DSP_CallBackThread(args);
    }
#endif
    // NMF on HOST Call Back thread function
    void NMF_SMPEE_CallBackThread(void * args) {
        ENS_Wrapper::NMF_SMPEE_CallBackThread(args);
    }
}

void ENS_Wrapper::InitNMF()
{
#ifndef HOST_ONLY
    CM_RegisterService(mNMF_DSP_CallBackChannel, NMF_DSP_ServiceCallback, omxComponent);
#endif
}

void ENS_Wrapper::DeInitNMF(OMX_HANDLETYPE hComponent)
{
#ifndef HOST_ONLY
    // We unregister the NMF_DSP_ServiceCallback for this wrapper to ensure not
    // more service messages will be send
    CM_UnregisterService(mNMF_DSP_CallBackChannel, NMF_DSP_ServiceCallback,
                         OMX_HANDLETYPE_TO_COMPONENTTYPE(hComponent));
#endif
}

void ENS_Wrapper::updateNMFTraceLevel(int arm, int sia, int sva, int cm_level)
{
    // CM level
#ifndef HOST_ONLY
    CM_SetMode(CM_CMD_TRACE_LEVEL, cm_level);
#endif

    // ARM on/off
    if(arm == 0 && NMF_STM_ARM_trace_level) {
        EE_SetMode(EE_CMD_TRACE_OFF, 0);
    } else if (arm) {
        EE_SetMode(EE_CMD_TRACE_ON, 0);
    }
    NMF_STM_ARM_trace_level = arm;

    // SIA on/off
    if(sia == 0 && NMF_STM_SIA_trace_level) {
        CM_SetMode(CM_CMD_MPC_TRACE_OFF, SIA_CORE_ID);
        if(NMF_STM_SVA_trace_level == 0)
            CM_SetMode(CM_CMD_TRACE_OFF, 0);
    } else if (sia) {
        CM_SetMode(CM_CMD_MPC_TRACE_ON, SIA_CORE_ID);
        CM_SetMode(CM_CMD_TRACE_ON, 0);
    }
    NMF_STM_SIA_trace_level = sia;

    // SVA on/off
    if(sva == 0 && NMF_STM_SVA_trace_level) {
        CM_SetMode(CM_CMD_MPC_TRACE_OFF, SVA_CORE_ID);
        if(NMF_STM_SIA_trace_level == 0)
            CM_SetMode(CM_CMD_TRACE_OFF, 0);
    } else if (sva) {
        CM_SetMode(CM_CMD_MPC_TRACE_ON, SVA_CORE_ID);
        CM_SetMode(CM_CMD_TRACE_ON, 0);
    }
    NMF_STM_SVA_trace_level = sva;
}

OMX_ERRORTYPE ENS_Wrapper::initNMFWrapperServices()
{
    ALOGI("initNMFWrapperServices");

    // Start a channel to the host EE - must be done prior to any SMPEE call
    t_nmf_error nmf_error = EE_CreateChannel(NMF_CHANNEL_SHARED, &mNMF_SMPEE_CallBackChannel);
    if (nmf_error != NMF_OK){
        ALOGE("initNMFWrapperServices: Error: EE_CreateChannel failed");
        return OMX_ErrorUndefined;
    }

#ifndef HOST_ONLY
    // Start a channel to the CM engine - must be done prior to any CM call
    nmf_error = CM_CreateChannel(NMF_CHANNEL_SHARED, &mNMF_DSP_CallBackChannel);
    if(nmf_error != NMF_OK){
        ALOGE("initNMFWrapperServices: Error: CM_CreateChannel failed");
        return OMX_ErrorUndefined;
    }

    int prop_value;
    GET_PROPERTY(STE_NMF_AUTOIDLE_PROPERTY_NAME, value, "-1");
    prop_value = atoi(value);
    if (prop_value == 1) {
        // Configure CM to AllowSleep
        CM_SetMode(CM_CMD_ULP_MODE_ON, 1);
        ALOGI("initNMFWrapperServices: Enabling CM ULP mode");
    }
    waitingForMpcFlushMessage_sem = LOS_SemaphoreCreate(0);

#endif  // #ifndef HOST_ONLY

    waitingForHostFlushMessage_sem = LOS_SemaphoreCreate(0);

#ifndef HOST_ONLY
    // Start the thread managing the NMF MPC to HOST call backs
    LOS_ThreadCreate(NMF_DSP_CallBackThread, NULL, 0,
                     LOS_USER_URGENT_PRIORITY, "ENS-MPC-CB");
#endif // HOST_ONLY

    // Start the thread managing the NMF MPC to HOST call backs
    LOS_ThreadCreate(NMF_SMPEE_CallBackThread, NULL, 0,
                     LOS_USER_URGENT_PRIORITY, "ENS-SMPEE-CB");

#ifndef HOST_ONLY
    // Any call to CM_SetMode must be done only when a channel is created and not before
    // Force call to CM_SetMode as seems to print by default
    CM_SetMode(CM_CMD_TRACE_LEVEL, ENS_Wrapper::NMF_trace_level);
#endif

    ALOGI("initNMFWrapperServices Done");
    return OMX_ErrorNone;
}

OMX_ERRORTYPE ENS_Wrapper::stopNMFWrapperServices()
{
    ALOGI("stopNMFWrapperServices");

    t_bool isMsgGenerated;
    t_nmf_error nmf_error;

    // Send flush message for HOST
    nmf_error = EE_FlushChannel(mNMF_SMPEE_CallBackChannel, &isMsgGenerated);
    if (nmf_error != NMF_OK) {
        ALOGE("stopENSWrapperServices: EE_FlushChannel failed");
    } else {
        if (isMsgGenerated) {
            LOS_SemaphoreWait(waitingForHostFlushMessage_sem);
        }
    }

    nmf_error = EE_CloseChannel(mNMF_SMPEE_CallBackChannel);
    if (nmf_error != NMF_OK) {
        ALOGE("stopENSWrapperServices: EE_CloseChannel failed");
        // This is very bad as next time around we'll fail getting messages from
        // channel. Most likely some bindings still remains on this channel
        DBC_ASSERT(0);
    }
    mNMF_SMPEE_CallBackChannel = 0;
    LOS_SemaphoreDestroy(waitingForHostFlushMessage_sem);

#ifndef HOST_ONLY
    // Send flush message for MPC
    nmf_error = CM_FlushChannel(mNMF_DSP_CallBackChannel, &isMsgGenerated);
    if (nmf_error == NMF_OK && isMsgGenerated) {
        LOS_SemaphoreWait(waitingForMpcFlushMessage_sem);
    }

    nmf_error = CM_CloseChannel(mNMF_DSP_CallBackChannel);
    if (nmf_error != NMF_OK){
        ALOGE("stopENSWrapperServices: CM_CloseChannel failed");
        return OMX_ErrorUndefined;
    }
    mNMF_DSP_CallBackChannel = 0;
    LOS_SemaphoreDestroy(waitingForMpcFlushMessage_sem);
#endif // HOST_ONLY

    ALOGI("stopNMFWrapperServices Done");
    return OMX_ErrorNone;
}

/********************************************************************************
 * NMF DSP Callbacks management
 ********************************************************************************/

#ifndef HOST_ONLY

/** the callback which is called by CM in case of Panic on DSP side */
void ENS_Wrapper::NMF_DSP_ServiceCallback(void *contextHandler,
                                          t_nmf_service_type serviceType,
                                          t_nmf_service_data *serviceData)
{
    OMX_COMPONENTTYPE *comphdl = (OMX_COMPONENTTYPE *) contextHandler;

    // Acquire lock here to not invalidate component once checked
    WRAPPER_LOCK(comphdl);
    // In case the CM_UnregisterService has been made after this message has been
    // retreived we need to check if wrapper is still active
    if(IsComponentActive(comphdl)) {
        if (serviceType == NMF_SERVICE_PANIC) {
            ALOGE("NMF-DSP ServiceCallback - sending 'PANIC' to " OMX_COMPONENT_DEF_STR,
                 OMX_COMPONENT_DEF_ARG(comphdl));
            Dump(LOG_OPENED | DUMP_FATAL, "DSPPanic");

        } else {
            WRAPPER_OMX_LOG(comphdl, "NMF-DSP ServiceCallback - sending '%s' to " OMX_COMPONENT_DEF_STR,
                            (serviceType == NMF_SERVICE_SHUTDOWN ? "SHUTDOWN" : "unkown-service"),
                            OMX_COMPONENT_DEF_ARG(comphdl));
        }

        ((ENS_Wrapper_p) comphdl->pComponentPrivate)->
            ensComponent->NmfPanicCallback(contextHandler, serviceType, serviceData);
    }
    WRAPPER_UNLOCK(comphdl);
}

// NMF Call Back thread function
void ENS_Wrapper::NMF_DSP_CallBackThread(void *)
{
    do {
        char *buffer;
        ENSWrapperCtx *WrapperCtx = 0;
        WRAPPER_DEBUG_LOG("NMF-DSP CallBackThread - Waiting for NMF message ...");
        t_nmf_error error = CM_GetMessage(mNMF_DSP_CallBackChannel, (void **)&WrapperCtx,
                                          &buffer, TRUE);
        if (error == NMF_FLUSH_MESSAGE) {
            WRAPPER_DEBUG_LOG("NMF-DSP CallBackThread flush message");
            LOS_SemaphoreNotify(waitingForMpcFlushMessage_sem);
            return;
        }
        if (error != CM_OK) {
            ALOGE("NMF-DSP CallBackThread - CM_GetMessage returned != CM_OK");
        } else {
            WRAPPER_DEBUG_LOG("NMF-DSP CallBackThread - Got NMF message");
            if (WrapperCtx) {
                // Acquire the component lock to execute code in it
                WRAPPER_LOCK(WrapperCtx->mHandle);
                WRAPPER_DEBUG_LOG("NMF-DSP CallBackThread - Got mutex, executing NMF CB message on"
                                  OMX_COMPONENT_DEF_STR, OMX_COMPONENT_DEF_ARG(WrapperCtx->mHandle));

                // Execute the call back
                CM_ExecuteMessage(WrapperCtx->mItfref, buffer);

                // Release component lock
                WRAPPER_UNLOCK(WrapperCtx->mHandle);
                WRAPPER_DEBUG_LOG("NMF-DSP CallBackThread - mutex released by "
                                  OMX_COMPONENT_DEF_STR, OMX_COMPONENT_DEF_ARG(WrapperCtx->mHandle));
            } else {
                // NMF service message (panic, ...)
                WRAPPER_DEBUG_LOG("NMF-DSP CallBackThread - calling CM_ExecuteMessage(0, buffer)");
                CM_ExecuteMessage(0, buffer);
                WRAPPER_DEBUG_LOG("NMF-DSP CallBackThread - exiting CM_ExecuteMessage(0, buffer)");
            }
        }
    } while(1);
}
#endif // HOST_ONLY

/********************************************************************************
 * NMF SMPEE Callbacks management
 ********************************************************************************/

void ENS_Wrapper::NMF_SMPEE_CallBackThread(void *)
{
    do {
        char *buffer;
        ENSHostWrapperCtx *WrapperCtx = 0;
        WRAPPER_DEBUG_LOG("NMF-ARM CallBackArmThread - Waiting for NMF message ...");
        t_nmf_error error = EE_GetMessage(mNMF_SMPEE_CallBackChannel, (void **)&WrapperCtx,
                                          &buffer, 1 /* blocking */);
        if (error == NMF_FLUSH_MESSAGE) {
            WRAPPER_DEBUG_LOG("NMF-ARM CallBackArmThread flush message");
            LOS_SemaphoreNotify(waitingForHostFlushMessage_sem);
            return;
        }
        if (error != CM_OK) {
            ALOGE("NMF-ARM CallBackThread - EE_GetMessage returned != CM_OK");
        } else {
            WRAPPER_DEBUG_LOG("NMF-ARM CallBackArmThread - Got ARM NMF message");
            if (WrapperCtx) {
                // Acquire the component lock to execute code in it
                WRAPPER_LOCK(WrapperCtx->mWrapper->omxComponent);
                WRAPPER_DEBUG_LOG("NMF-ARM CallBackArmThread - Got mutex, executing NMF CB message on "
                                  OMX_COMPONENT_DEF_STR,
                                  OMX_COMPONENT_DEF_ARG(WrapperCtx->mWrapper->omxComponent));

                // Execute the call back
                EE_ExecuteMessage(WrapperCtx->mItfref, buffer);

                // Release component lock
                WRAPPER_UNLOCK(WrapperCtx->mWrapper->omxComponent);
                WRAPPER_DEBUG_LOG("NMF-ARM CallBackThread - mutex released by "
                                  OMX_COMPONENT_DEF_STR,
                                  OMX_COMPONENT_DEF_ARG(WrapperCtx->mWrapper->omxComponent));
            } else {
                // NMF service message (panic, ...)
                WRAPPER_DEBUG_LOG("NMF-ARM CallBackArmThread - calling EE_ExecuteMessage(0, buffer)");
                EE_ExecuteMessage(0, buffer);
                WRAPPER_DEBUG_LOG("NMF-ARM CallBackArmThread - exiting EE_ExecuteMessage(0, buffer)");
            }
        }
    } while(1);
}

/********************************************************************************
 * NMF DSP context binding
 ********************************************************************************/

#ifndef HOST_ONLY
t_cm_error ENS_Wrapper::cm_bindComponentToUser(OMX_HANDLETYPE hComp,
                                               t_cm_instance_handle component,
                                               const char *itfname,
                                               NMF::InterfaceDescriptor *itfref,
                                               t_uint32 fifosize)
{
    ENSWrapperCtx * wrapperCtx = new ENSWrapperCtx((t_nmf_interface_desc *)itfref,
                                                   (OMX_COMPONENTTYPE *)hComp);

    if(wrapperCtx == 0) {
        return CM_NO_MORE_MEMORY;
    }

    return CM_BindComponentToUser(mNMF_DSP_CallBackChannel, component,
                                  itfname, wrapperCtx, fifosize);
}

t_cm_error ENS_Wrapper::cm_unbindComponentToUser(OMX_HANDLETYPE hComp,
                                                 const t_cm_instance_handle client,
                                                 const char* requiredItfClientName)
{
    ENSWrapperCtx * ctx = 0;
    t_cm_error error = CM_OK;

    error = CM_UnbindComponentToUser(mNMF_DSP_CallBackChannel, client,
                                     requiredItfClientName, (void **)&ctx);

    if(ctx) {
        delete ctx;
    }

    return error;
}

#endif // HOST_ONLY

/********************************************************************************
 * NMF SMPEE context binding
 ********************************************************************************/

t_nmf_error ENS_Wrapper::bindToUser(NMF::Composite *component,
                                    const char *itfname,
                                    NMF::InterfaceDescriptor *itfref,
                                    t_uint32 fifosize)
{
    ENSHostWrapperCtx * wrapperCtx = new ENSHostWrapperCtx(this, itfref);

    if(wrapperCtx == 0) {
        return CM_NO_MORE_MEMORY;
    }

    return component->bindToUser(mNMF_SMPEE_CallBackChannel, itfname, wrapperCtx, fifosize);
}

t_nmf_error ENS_Wrapper::unbindToUser(NMF::Composite *component,
                                      const char *itfname)
{
    ENSHostWrapperCtx * ctx = 0;
    t_cm_error error = CM_OK;

    error = component->unbindToUser(mNMF_SMPEE_CallBackChannel, itfname, (void **)&ctx);

    if(ctx && (ctx != (ENSHostWrapperCtx *)0XDEADBEEF)) {
        delete ctx;
    }

    return error;
}

