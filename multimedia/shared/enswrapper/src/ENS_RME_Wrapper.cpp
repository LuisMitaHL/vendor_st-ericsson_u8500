/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * \file    ENS_RME_Wrapper.cpp
 * \brief   Implement the ENS wrapper to RME interactions
 */


#include "ENS_RME_Wrapper.h"
#include "ENS_Wrapper.h"
#include "ENS_DBC.h"

#define LOG_TAG "ENS_RME"
#include "linux_utils.h"

#define DEBUG_RME_PRINTF(...)			\
    if (ENS_Wrapper::RME_trace_enabled) {	\
        ALOGD(__VA_ARGS__);			\
    }

ENS_RME_Wrapper * ENS_RME_Wrapper::rmeWrapper = (ENS_RME_Wrapper *) NULL;

extern "C" {
    // RME thread function
    void RME_ThreadEntry(void * args) {
        ENS_RME_Wrapper::RME_ThreadEntry(args);
    }
    // RMECallBack thread function
    void RMECallBack_ThreadEntry(void * args) {
        ENS_RME_Wrapper::RMECallBack_ThreadEntry(args);
    }
}


ENS_RME_Wrapper::ENS_RME_Wrapper()
    : mCMP2RMEfreeMsgList(ENS_Queue(RME_MESSAGE_ARRAY_SIZE)),
      pendingCMP2RMEmsgList(ENS_Queue(RME_MESSAGE_ARRAY_SIZE)),
      mRME2CMPfreeMsgList(ENS_Queue(RME_MESSAGE_ARRAY_SIZE)),
      pendingRME2CMPmsgList(ENS_Queue(RME_MESSAGE_ARRAY_SIZE)),
      mRME((RME *) NULL), refCount(0)
{
    DEBUG_RME_PRINTF("Creating ENS_RME_Wrapper 0x%08x", (unsigned int) this);

    for(int i=0;i<RME_MESSAGE_ARRAY_SIZE;i++) {
        mCMP2RMEfreeMsgList.Push((void *) &(mCMP2RME_MessageArray[i]));
    }
    for(int i=0;i<RME_MESSAGE_ARRAY_SIZE;i++) {
        mRME2CMPfreeMsgList.Push((void *) &(mRME2CMP_MessageArray[i]));
    }

    CMP2RME_semaphore = LOS_SemaphoreCreate(0);
    RME2CMP_semaphore = LOS_SemaphoreCreate(0);

    RME_exit_sem = LOS_SemaphoreCreate(0);
    RMECallBack_exit_sem = LOS_SemaphoreCreate(0);
    mRME = RME::getInstance();
}

ENS_RME_Wrapper::~ENS_RME_Wrapper()
{
    DEBUG_RME_PRINTF("Destroying ENS_RME_Wrapper");

    LOS_SemaphoreDestroy(CMP2RME_semaphore);
    LOS_SemaphoreDestroy(RME2CMP_semaphore);
    LOS_SemaphoreDestroy(RME_exit_sem);
    LOS_SemaphoreDestroy(RMECallBack_exit_sem);

    RME::delInstance();
}

void ENS_RME_Wrapper::startRMEthread()
{
    DEBUG_RME_PRINTF("ENS_RME_Wrapper::startRMEthread");
    LOS_ThreadCreate(RME_ThreadEntry, NULL, 0, LOS_USER_NORMAL_PRIORITY, "ENS-RME");
}

void ENS_RME_Wrapper::startRMECallBackThread()
{
    DEBUG_RME_PRINTF("ENS_RME_Wrapper::startRMECallBackThread");
    LOS_ThreadCreate(RMECallBack_ThreadEntry, NULL, 0, LOS_USER_NORMAL_PRIORITY, "ENS-RME-CB");
}

ENS_RME_Wrapper * ENS_RME_Wrapper::getInstance()
{
    if (rmeWrapper == NULL) {
        rmeWrapper = new ENS_RME_Wrapper();
        rmeWrapper->startRMEthread();
        rmeWrapper->startRMECallBackThread();
        DEBUG_RME_PRINTF("ENS_RME_Wrapper::getInstance returning 0x%08x", (unsigned int) rmeWrapper);
    }
    return rmeWrapper;
}

void ENS_RME_Wrapper::delInstance()
{
    if (rmeWrapper != NULL) {
        DEBUG_RME_PRINTF("ENS_RME_Wrapper::delInstance 0x%08x", (unsigned int) rmeWrapper);
        delete rmeWrapper;
        rmeWrapper = (ENS_RME_Wrapper *)NULL;
    }
}

/********************************************************************************/
// CMP to RME queue

CMP2RME_Message * ENS_RME_Wrapper::getFreeCMP2RMEmsg()
{
    return (CMP2RME_Message *) mCMP2RMEfreeMsgList.Pop();
}

void ENS_RME_Wrapper::putFreeCMP2RMEmsg(CMP2RME_Message *msg)
{
    mCMP2RMEfreeMsgList.Push(msg);
}

void ENS_RME_Wrapper::postCMP2RMEmsg(CMP2RME_Message *msg)
{
    pendingCMP2RMEmsgList.Push(msg);
    // We always notify the semaphore of new message
    LOS_SemaphoreNotify(CMP2RME_semaphore);
}

CMP2RME_Message * ENS_RME_Wrapper::popCMP2RMEmsg()
{
    return (CMP2RME_Message *) pendingCMP2RMEmsgList.Pop();
}

/********************************************************************************/
// RME to CMP queue

RME2CMP_Message * ENS_RME_Wrapper::getFreeRME2CMPmsg()
{
    return (RME2CMP_Message *) mRME2CMPfreeMsgList.Pop();
}
void ENS_RME_Wrapper::putFreeRME2CMPmsg(RME2CMP_Message *msg)
{
    mRME2CMPfreeMsgList.Push(msg);
}

void ENS_RME_Wrapper::postRME2CMPmsg(RME2CMP_Message *msg)
{
    pendingRME2CMPmsgList.Push(msg);
    // We always notify the semaphore of new message
    LOS_SemaphoreNotify(RME2CMP_semaphore);
}

RME2CMP_Message * ENS_RME_Wrapper::popRME2CMPmsg()
{
    return (RME2CMP_Message *) pendingRME2CMPmsgList.Pop();
}

/********************************************************************************/
// RME thread

void ENS_RME_Wrapper::RME_ThreadEntry(void *)
{
    int exitRequested = 0;
        long rme_timeout = 0;
    int timerExpired = 0;
    int ret=0;

    DEBUG_RME_PRINTF("ENS-RME thread - starting");

    do {
        timerExpired = 0;
        if (rme_timeout>0) {
            // Wait for a message to process or timer expiration
            DEBUG_RME_PRINTF("ENS-RME thread waiting message or timer expiration");
            ret=LOS_SemaphoreWaitTimeOut(rmeWrapper->CMP2RME_semaphore,rme_timeout);
            if (ret<0){
                // Timer has expired
                timerExpired = 1;
            }
        } else {
            // Wait for a message to process
            DEBUG_RME_PRINTF("ENS-RME thread waiting message");
            LOS_SemaphoreWait(rmeWrapper->CMP2RME_semaphore);
        }

        if (!timerExpired) {
            // A message has been received
            rme_timeout = 0;
            // Retrieve the message
            CMP2RME_Message *msg = ENS_RME_Wrapper::getInstance()->popCMP2RMEmsg();
            DBC_ASSERT(msg != NULL);

            // Treat the message
            switch(msg->type) {
            case MSG_CMP2RME_Notify_NewStatesOMXRES:
                DEBUG_RME_PRINTF("ENS-RME thread executing Notify_NewStatesOMXRES() - ID=%d",
                                 (unsigned int) msg->pCompHdl);
                rmeWrapper->mRME->Notify_NewStatesOMXRES(msg->pCompHdl,
                                                         msg->args.sNotify_NewStatesOMXRES.eOMXILState,
                                                         msg->args.sNotify_NewStatesOMXRES.eResourceState,
                                                         msg->args.sNotify_NewStatesOMXRES.nCounter);
                break;
            case MSG_CMP2RME_Notify_NewPolicyCfg:
                DEBUG_RME_PRINTF("ENS-RME thread executing Notify_NewPolicyCfg() - ID=%d",
                                 (unsigned int) msg->pCompHdl);
                rmeWrapper->mRME->Notify_NewPolicyCfg(msg->pCompHdl,
                                                      msg->args.sNotify_NewPolicyCfg.nGroupPriority,
                                                      msg->args.sNotify_NewPolicyCfg.nGroupID,
                                                      msg->args.sNotify_NewPolicyCfg.eQoSbitmap);
                break;
            case MSG_CMP2RME_Notify_RMConfigApplied:
                DEBUG_RME_PRINTF("ENS-RME thread executing Notify_RMConfigApplied() - ID=%d",
                                 (unsigned int) msg->pCompHdl);
                rmeWrapper->mRME->Notify_RMConfigApplied(msg->pCompHdl,
                                                         msg->args.sNotify_RMConfigApplied.eType,
                                                         msg->args.sNotify_RMConfigApplied.nData,
                                                         msg->args.sNotify_RMConfigApplied.nErrorCode);
                break;
            case MSG_CMP2RME_Notify_Tunnel:
                DEBUG_RME_PRINTF("ENS-RME thread executing Notify_Tunnel() - outID=%d inID=%d",
                                 (unsigned int) msg->pCompHdl,
                                 (unsigned int) msg->args.sNotify_Tunnel.pCompHdlIn);
                rmeWrapper->mRME->Notify_Tunnel(msg->pCompHdl,
                                                msg->args.sNotify_Tunnel.nPortIndexOut,
                                                msg->args.sNotify_Tunnel.pCompHdlIn,
                                                msg->args.sNotify_Tunnel.nPortIndexIn);
                break;
            case MSG_CMP2RME_Notify_ESRAM_Sharing:
#ifdef ALP_SUPPORT_MIGRATIONDOMAINS
                DEBUG_RME_PRINTF("ENS-RME thread executing Notify_ESRAM_Sharing() - ID=%d",
                                 (unsigned int) msg->pCompHdl);
                rmeWrapper->mRME->Notify_ESRAM_Sharing(msg->args.sNotify_ESRAM_Sharing.
                                                       bEsramResourceRequired);
#endif
                break;
            case MSG_CMP2RME_Request_Thread_Exit:
                DEBUG_RME_PRINTF("ENS-RME thread got MSG_CMP2RME_Request_Thread_Exit");
                exitRequested = 1;
                break;
            default:
                DBC_ASSERT(0);
            }

            // Put back message container as available
            ENS_RME_Wrapper::getInstance()->putFreeCMP2RMEmsg(msg);

            // In case queue is empty call IdleT
            if(rmeWrapper->getNbPendingCMP2RME() == 0) {
                DEBUG_RME_PRINTF("ENS-RME thread executing idleT - cause is empty queue");
                rme_timeout = rmeWrapper->mRME->idleT();
            }
        } else {
            // Timer has expired : call idleT
            DEBUG_RME_PRINTF("ENS-RME thread executing idleT - cause is timeout reached");
            rme_timeout = rmeWrapper->mRME->idleT();
        }

    } while(!exitRequested);

    RME2CMP_Message *RME2CMPmsg = rmeWrapper->getFreeRME2CMPmsg();
    DBC_ASSERT(RME2CMPmsg != NULL);
    RME2CMPmsg->type = MSG_RME2CMP_Request_Thread_Exit;
    rmeWrapper->postRME2CMPmsg(RME2CMPmsg);

    LOS_SemaphoreNotify(rmeWrapper->RME_exit_sem);
    DEBUG_RME_PRINTF("ENS-RME thread - exiting");
}

/********************************************************************************/
// RMECallBack thread

void ENS_RME_Wrapper::RMECallBack_ThreadEntry(void *)
{
    int exitRequested = 0;
    RME2CMP_Message *msg;

    DEBUG_RME_PRINTF("ENS-RME-CB thread - starting");

    do {
        // Wait for a message to process
        DEBUG_RME_PRINTF("ENS-RME-CB thread waiting");

        LOS_SemaphoreWait(rmeWrapper->RME2CMP_semaphore);

        msg = ENS_RME_Wrapper::getInstance()->popRME2CMPmsg();

        DEBUG_RME_PRINTF("ENS-RME-CB thread got message");

        DBC_ASSERT(msg != NULL);

        OMX_COMPONENTTYPE *hComponent =
            ENS_Wrapper::GetOMX_COMPONENTTYPEforID((unsigned long) msg->pCompHdl);

        if(msg->type == MSG_RME2CMP_Request_Thread_Exit) {
            DEBUG_RME_PRINTF("ENS-RME-CB thread MSG_RME2CMP_Request_Thread_Exit");
            exitRequested = 1;
        } else {
            if(hComponent) {
                switch(msg->type) {
                case MSG_RME2CMP_CP_Notify_Error:
                    DEBUG_RME_PRINTF("ENS-RME-CB thread MSG_RME2CMP_CP_Notify_Error - ID=%ld",
                                     (unsigned long) msg->pCompHdl);
                    OMX_HANDLETYPE_TO_WRAPPER(hComponent)->
                        RME2CMP_Notify_Error(hComponent,
                                             msg->args.CP_Notify_Error.nError);
                    break;

                case MSG_RME2CMP_CP_Notify_ResReservationProcessed:
                    DEBUG_RME_PRINTF("ENS-RME-CB thread MSG_RME2CMP_CP_Notify_ResReservationProcessed - ID=%ld",
                                     (unsigned long) msg->pCompHdl);
                    OMX_HANDLETYPE_TO_WRAPPER(hComponent)->
                        RME2CMP_Notify_ResReservationProcessed(hComponent,
                            msg->args.CP_Notify_ResReservationProcessed.bResourcesReservGranted);
                    break;

                case MSG_RME2CMP_CP_Cmd_SetRMConfig:
                    DEBUG_RME_PRINTF("ENS-RME-CB thread MSG_RME2CMP_CP_Notify_ResReservationProcessed - ID=%ld",
                                     (unsigned long) msg->pCompHdl);
                    OMX_HANDLETYPE_TO_WRAPPER(hComponent)->
                        RME2CMP_Cmd_SetRMConfig(hComponent,
                                                &(msg->args.CP_Cmd_SetRMConfig.sRMcfg));
                    break;

                case MSG_RME2CMP_CP_Cmd_ReleaseResource:
                    DEBUG_RME_PRINTF("ENS-RME-CB thread MSG_RME2CMP_CP_Cmd_ReleaseResource - ID=%ld",
                                     (unsigned long) msg->pCompHdl);
                    OMX_HANDLETYPE_TO_WRAPPER(hComponent)->
                        RME2CMP_Cmd_ReleaseResource(hComponent,
                                                    msg->args.CP_Cmd_ReleaseResource.bSuspend);
                    break;

                default:
                    DBC_ASSERT(0);
                }
            } else {
                const char *msg_name;
                switch(msg->type) {
                case MSG_RME2CMP_CP_Notify_Error: msg_name = "Notify_Error"; break;
                case MSG_RME2CMP_CP_Notify_ResReservationProcessed: msg_name = "ResReservationProcessed"; break;
                case MSG_RME2CMP_CP_Cmd_SetRMConfig: msg_name = "Cmd_SetRMConfig"; break;
                case MSG_RME2CMP_CP_Cmd_ReleaseResource: msg_name = "Cmd_ReleaseResource"; break;
                default: msg_name = "Unknown";
                }
                ALOGI("ENS-RME-CB thread ignoring RME message '%s' on non active component ID=%ld",
                     msg_name, (unsigned long) msg->pCompHdl);
            }
        }
        // Put back message container as available
        ENS_RME_Wrapper::getInstance()->putFreeRME2CMPmsg(msg);
    } while(!exitRequested);

    // We notify the exit semaphore to notify we are exiting
    LOS_SemaphoreNotify(rmeWrapper->RMECallBack_exit_sem);

    DEBUG_RME_PRINTF("ENS-RME-CB thread - exiting");
}

/********************************************************************************
 * Synchronous calls to RME from ENS (called from ENS_HwRm class implementation)
 ********************************************************************************/

RM_STATUS_E ENS_RME_Wrapper::RME_Register(OMX_IN const OMX_PTR pCompHdl,
                                          OMX_IN const RM_CBK_T* pCompCbks)
{
    return mRME->Register(pCompHdl, pCompCbks);
}


RM_STATUS_E ENS_RME_Wrapper::RME_Unregister(OMX_IN const OMX_PTR pCompHdl)
{
    return mRME->Unregister(pCompHdl);
}


OMX_U32 ENS_RME_Wrapper::RME_Get_DefaultDDRDomain(OMX_IN RM_NMFD_E nDomainType)
{
    return mRME->Get_DefaultDDRDomain(nDomainType);
}

