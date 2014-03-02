/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
#include <ee/callBack.nmf>
#include <ee/api/priority.idt>
//#include <ee/scheduler.nmf>
#include <inc/nmf-def.h>
extern  IMPORT_SHARED t_uint32 nmfSchedulerWait(void);
typedef struct {
    t_list_link channelListElem; //Must be the first param
    t_uint32 refCounter;
    t_uint32 bindRefCounter;
    hSem sem;
    t_queue service;
    t_queue message;
    t_nmf_notify cbNotify;
    void *cbNotifyContextHandler;
    hList serviceCallbackList;
} t_ee_channel;

typedef struct _t_ee_service {
    t_list_link serviceListElem; //Must be the first param
    t_uint32 refCounter;
    t_nmf_serviceCallback handler;
    void *contextHandler;
    t_uint32 isFoundWhenRemove;
    struct _t_ee_service *pElemFound;
} t_ee_service;

/* globals */
static hSem channelListLock;
static hList channelListHandle;
static t_ee_channel *pSharedChannel = 0;

/* private functions */
static void *myMemcpy(void *s1, const void *s2, t_uint32 size);
static void myMemZero(void *s, t_uint32 size);
static t_nmf_error createChannel(t_nmf_channel_flag flags, t_nmf_channel *channel);
static t_nmf_error destroyChannel(t_nmf_channel channel);
static t_ee_service *createService(t_nmf_serviceCallback handler, void *contextHandler);
static t_bool matchingServiceAdd(t_list_link *_pElem, void *args);
static t_bool matchingServiceRemove(t_list_link *_pElem, void *args);
static void executeServiceDispatching(t_list_link *_pElem, t_msg_service_header *pMsg);
static void executeService(t_list_link *_pElem, t_msg_service_header *pMsg);

/* typedef for call back */
typedef void (*reacCb)(t_callback_event *pEvent, void *cbClass);

void METH(EE_GetVersion)(t_uint32 *version) {
    *version = NMF_VERSION;
}

t_nmf_error METH(EE_SetMode)(t_ee_cmd_id aCmdID, t_sint32 aParam)
{
    switch(aCmdID)
    {
    case EE_CMD_TRACE_ON:
        nmfTraceSetMode(TRUE);
        break;
    case EE_CMD_TRACE_OFF:
        nmfTraceSetMode(FALSE);
        break;
    }

    return NMF_OK;
}

/* implement ee.api.out.itf */
t_nmf_error METH(EE_CreateChannel)(t_nmf_channel_flag flags, t_nmf_channel *channel)
{
    t_nmf_error res = NMF_OK;
    
    /* sanity check */
    if (channel == 0)
        return NMF_INVALID_PARAMETER;
    if (flags != NMF_CHANNEL_SHARED && flags != NMF_CHANNEL_PRIVATE)
        return NMF_INVALID_PARAMETER;
    /* add channel */
    sem.P(channelListLock);
    if (flags == NMF_CHANNEL_SHARED && pSharedChannel != 0)
    {
        pSharedChannel->refCounter++;
        *channel = (t_nmf_channel) pSharedChannel;
    }
    else
    {
        res = createChannel(flags, channel);
        if (res == NMF_OK) {list.add(channelListHandle, (t_list_link *) *channel);}
    }
    sem.V(channelListLock);
    
    return res;
}

t_nmf_error METH(EE_FlushChannel)(t_nmf_channel channel, t_bool *isFlushMessageGenerated)
{
    t_ee_channel *pChannel = (t_ee_channel *) channel;
    t_nmf_notify cbNotify = pChannel->cbNotify;
    void *cbNotifyContextHandler = pChannel->cbNotifyContextHandler;
    t_nmf_error res = NMF_OK;
    
    /* sanity check */
    if (isFlushMessageGenerated == 0)
        return NMF_INVALID_PARAMETER;
    /* generate message is needed */
    *isFlushMessageGenerated = FALSE;
    sem.P(channelListLock);
    if (list.isElem(channelListHandle, (t_list_link *) pChannel)) {
        if (pChannel->refCounter == 1) {
            *isFlushMessageGenerated = TRUE;
            sem.V(pChannel->sem);
            if (cbNotify) {(*cbNotify)(cbNotifyContextHandler);}
        }
    } else {
        res = NMF_INVALID_PARAMETER;
    }
    sem.V(channelListLock);
    
    return res;
}

t_nmf_error METH(EE_CloseChannel)(t_nmf_channel channel)
{
    t_ee_channel *pChannel = (t_ee_channel *) channel;
    t_nmf_error res = NMF_OK;
    
    /* sanity check */
    if (channel == 0)
        return NMF_INVALID_PARAMETER;
    sem.P(channelListLock);
    //check no more service registering before closing
    if (list.isElem(channelListHandle, (t_list_link *) pChannel))
    {
        if (pChannel == pSharedChannel && pChannel->refCounter > 1)
        {
            // shared channel will not be destroyed since still more than one reference to it
            pChannel->refCounter--;
        }
        else
        {
            // not shared channel or shared channel but this is the last reference
            if (list.getNumberOfElem(pChannel->serviceCallbackList) == 0 &&
                pChannel->bindRefCounter == 0)
            {
                list.remove(channelListHandle, (t_list_link *) channel);
                destroyChannel(channel);
            }
            else {res = NMF_INVALID_PARAMETER;}
        }
    }
    else {res= NMF_INVALID_PARAMETER;}
    sem.V(channelListLock);
    
    return res;
}

t_nmf_error METH(EE_RegisterService)(t_nmf_channel channel, t_nmf_serviceCallback handler, void *contextHandler)
{
    t_ee_channel *pChannel = (t_ee_channel *) channel;
    t_ee_service *pToAdd;
    t_nmf_error res = NMF_OK;
    
    /* sanity check */
    if (channel == 0)
        return NMF_INVALID_PARAMETER;
    if (handler == 0)
        return NMF_INVALID_PARAMETER;
    sem.P(channelListLock);
    if (list.isElem(channelListHandle, (t_list_link *) pChannel))
    {
        pToAdd = createService(handler, contextHandler);
        if (pToAdd == 0) {return NMF_NO_MORE_MEMORY;}
        if (list.addNotMatching(pChannel->serviceCallbackList, (t_list_link *) pToAdd, (void *) &matchingServiceAdd, (void *) pToAdd))
        {
            //So we found that (handler, contextHandler) was already register so we destroy structure
            //not insert. Note that reference counter was incremented by matching function.
            allocator.free((void *) pToAdd);
        }
    }
    else {res = NMF_INVALID_PARAMETER;}
    sem.V(channelListLock);
    
    return res;
}

t_nmf_error METH(EE_UnregisterService)(t_nmf_channel channel, t_nmf_serviceCallback handler, void *contextHandler)
{
    t_ee_channel *pChannel = (t_ee_channel *) channel;
    t_ee_service toRemove;
    t_nmf_error res = NMF_OK;

    /* sanity check */
    if (channel == 0)
        return NMF_INVALID_PARAMETER;
    if (handler == 0)
        return NMF_INVALID_PARAMETER;
    sem.P(channelListLock);
    if (list.isElem(channelListHandle, (t_list_link *) pChannel))
    {
        toRemove.handler = handler;
        toRemove.contextHandler = contextHandler;
        toRemove.isFoundWhenRemove = 0;
        list.removeMatching(pChannel->serviceCallbackList, (void *) &matchingServiceRemove, &toRemove);
        if (toRemove.isFoundWhenRemove)
        {
            if (toRemove.pElemFound->refCounter == 0) {allocator.free((void *) toRemove.pElemFound);}
        }
        else {res = NMF_INVALID_PARAMETER;}
    }
    else {res = NMF_INVALID_PARAMETER;}
    sem.V(channelListLock);
    
    return res;
}

t_nmf_error METH(EE_RegisterNotify)(t_nmf_channel channel, t_nmf_notify notify, void *contextHandler)
{
    t_ee_channel *pChannel = (t_ee_channel *) channel;
    t_nmf_error res = NMF_OK;
    
    /* sanity check */
    if (channel == 0)
        return NMF_INVALID_PARAMETER;
    sem.P(channelListLock);
    if (list.isElem(channelListHandle, (t_list_link *) pChannel))
    {
        pChannel->cbNotify = notify;
        pChannel->cbNotifyContextHandler = contextHandler;
    }
    else {res = NMF_INVALID_PARAMETER;}
    sem.V(channelListLock);
    
    return res;
}

t_nmf_error METH(EE_GetMessage)(t_nmf_channel channel, void **clientContext, char **message, t_bool blockable)
{
    t_ee_channel *pChannel = (t_ee_channel *) channel;
    t_msg_service_header *pService;
    t_callback_event *pEvent;
    
    /* sanity check */
    if (channel == 0 || clientContext == 0 || message == 0)
        return NMF_INVALID_PARAMETER;
    /* so try to get message */
    if (blockable) {sem.P(pChannel->sem);}
    else if (sem.tryP(pChannel->sem) != 0) {return NMF_NO_MESSAGE;}
    /* so we are sure we have a message so get the most important one */
    /* first try so see if we have a service message */
    pService = (t_msg_service_header *) nmfQueuePop(pChannel->service);
    if (pService) {*message = (char *) pService;}
    else
    {
        /* so it should be a regular one */
        pEvent = (t_callback_event *) nmfQueuePop(pChannel->message);
        if (pEvent) {
            *message = (char *) pEvent;
            *clientContext = pEvent->clientContext;
        } else {
            /* up has been done on semaphore without appending new message so it's a flush message */
            return NMF_FLUSH_MESSAGE;
        }
    }
    
    return NMF_OK;
}

void METH(EE_ExecuteMessage)(void *itfref, char *message)
{
    t_ee_msg_header *pMsg = (t_ee_msg_header *) message;
    
    switch (pMsg->type)
    {
        case EE_CB_ITF_MSG:
            {
                t_callback_event *pEvent = (t_callback_event *) message;
                reacCb react = (reacCb)pEvent->pReaction;
                
                (*react)(pEvent, itfref);
            }
            break;
        case EE_CB_SVC_MSG:
            {
                t_msg_service_header *pSvc = (t_msg_service_header *) message;
                t_ee_channel *pChannel = (t_ee_channel *) pSvc->channel;
                
                /* execute message for all registered service callback on this channel */
                list.execute(pChannel->serviceCallbackList , (void *) executeService, (void *) pSvc);
                /* destroy it */
                allocator.free((void *) pSvc);
            }
            break;
        default:
            break;
    }
}

/* implement ee.api.callback */
EXPORT_SHARED t_nmf_error nmfChannelAddRef(t_nmf_channel channel)
{
    t_nmf_error res = NMF_OK;
    t_ee_channel *pChannel = (t_ee_channel *) channel;
    
    sem.P(channelListLock);
    if (list.isElem(channelListHandle, (t_list_link *) pChannel))
    {
        pChannel->bindRefCounter++;
    }
    else {res = NMF_INVALID_PARAMETER;}
    sem.V(channelListLock);
    
    return res;
}

EXPORT_SHARED t_nmf_error nmfChannelRemoveRef(t_nmf_channel channel)
{
    t_nmf_error res = NMF_OK;
    t_ee_channel *pChannel = (t_ee_channel *) channel;
    
    sem.P(channelListLock);
    if (list.isElem(channelListHandle, (t_list_link *) channel) && pChannel->bindRefCounter)
    {
        pChannel->bindRefCounter--;
    }
    else {res = NMF_INVALID_PARAMETER;}
    sem.V(channelListLock);
    
    return res;
}

EXPORT_SHARED void nmfPushMessageInChannel(t_nmf_channel channel, t_queue_link *pEvent)
{
    t_ee_channel *pChannel = (t_ee_channel *) channel;
    t_nmf_notify cbNotify = pChannel->cbNotify;
    void *cbNotifyContextHandler = pChannel->cbNotifyContextHandler;
    
    nmfQueuePush(pChannel->message, pEvent);
    sem.V(pChannel->sem);
    if (cbNotify) {(*cbNotify)(cbNotifyContextHandler);}
}


EXPORT_SHARED t_queue_link* nmfRemoveMessageFromChannel(t_nmf_channel channel)
{
    t_ee_channel *pChannel = (t_ee_channel *) channel;
    t_uint32 retries=0;
    t_queue_link *pMsg=NULL;
    pMsg =nmfQueuePop(pChannel->message);
    if(!pMsg)
    {
        for(retries=0;retries<=30;retries++)
        nmfSchedulerWait();
    }
    return pMsg;
}

EXPORT_SHARED void nmfPushServiceMessage(t_msg_service_header *pMsg)
{
    sem.P(channelListLock);
    list.execute(channelListHandle, (void *) executeServiceDispatching, (void *) pMsg);
    sem.V(channelListLock);
}

/* implement ee.api.init */
t_sint32 METH(init)()
{
    channelListHandle = list.create(0);
    if (channelListHandle == (hList)0) {return NMF_NO_MORE_MEMORY;}
    channelListLock = sem.create(1);
    if (channelListLock == (hSem)0)
    {
        list.destroy(channelListHandle);
        return NMF_NO_MORE_MEMORY;
    }
    
    return (t_sint32) NMF_OK;
}

void METH(destroy)()
{
    list.destroy(channelListHandle);
    channelListHandle = 0;
    sem.destroy(channelListLock);
    channelListLock = 0;
}

/* private methods */
static t_nmf_error createChannel(t_nmf_channel_flag flags, t_nmf_channel *channel)
{
    t_ee_channel *pChannel;
    
    pChannel = (t_ee_channel *) allocator.alloc(sizeof(t_ee_channel));
    if (pChannel == 0) {return NMF_NO_MORE_MEMORY;}
    myMemZero((void *) pChannel, sizeof(t_ee_channel));
    
    pChannel->bindRefCounter = 0;
    pChannel->refCounter = 1;
    pChannel->sem = sem.create(0);
    if (pChannel->sem == 0) {goto error;}
    
    pChannel->serviceCallbackList = list.create(1);
    if (pChannel->serviceCallbackList == 0) {goto error;}

    pChannel->service = nmfQueueCreate();
    if (pChannel->service == 0) {goto error;}

    pChannel->message = nmfQueueCreate();
    if (pChannel->message == 0) {goto error;}
    
    if (flags == NMF_CHANNEL_SHARED) {pSharedChannel = pChannel;}
    *channel = (t_nmf_channel) pChannel;
    
    return NMF_OK;
    
error:
    if (pChannel->service) {nmfQueueDestroy(pChannel->service);}
    if (pChannel->serviceCallbackList) {list.destroy(pChannel->serviceCallbackList);}
    if (pChannel->sem) {sem.destroy(pChannel->sem);}
    allocator.free(pChannel);
    
    return NMF_NO_MORE_MEMORY;
}

static t_nmf_error destroyChannel(t_nmf_channel channel)
{
    t_ee_channel *pChannel = (t_ee_channel *) channel;
    t_msg_service_header *pService;
    
    nmfQueueDestroy(pChannel->message);
    // destroy copy of service message not read before destroying service queue
    while((pService = (t_msg_service_header *) nmfQueuePop(pChannel->service)) != 0)
    {
        allocator.free((void *) pService);
    }
    nmfQueueDestroy(pChannel->service);
    list.destroy(pChannel->serviceCallbackList);
    sem.destroy(pChannel->sem);
    allocator.free(pChannel);
    if (pChannel == pSharedChannel) {pSharedChannel = 0;}
    
    return NMF_OK;
}

static t_ee_service *createService(t_nmf_serviceCallback handler, void *contextHandler)
{
    t_ee_service *pRes;
    
    pRes = allocator.alloc(sizeof(t_ee_channel));
    if (pRes)
    {
        pRes->refCounter = 1;
        pRes->handler = handler;
        pRes->contextHandler = contextHandler;
    }
    
    return pRes;
}

static t_bool matchingServiceAdd(t_list_link *_pElem, void *args)
{
    t_ee_service *pElem = (t_ee_service *) _pElem;
    t_ee_service *pToAdd = (t_ee_service *) args;
    
    if (pElem->handler == pToAdd->handler && pElem->contextHandler == pToAdd->contextHandler)
    {
        pElem->refCounter++;
        return 1;
    }
    
    return 0;
}

static t_bool matchingServiceRemove(t_list_link *_pElem, void *args)
{
    t_ee_service *pElem = (t_ee_service *) _pElem;
    t_ee_service *pToRemove = (t_ee_service *) args;
    
    if (pElem->handler == pToRemove->handler && pElem->contextHandler == pToRemove->contextHandler)
    {
        pToRemove->isFoundWhenRemove = 1;
        pToRemove->pElemFound = pElem;
        pElem->refCounter--;
        if (!pElem->refCounter) {return 1;} //remove from list if reference counter down to zero
    }
    
    return 0;
}

static void executeServiceDispatching(t_list_link *_pElem, t_msg_service_header *pMsg)
{
    t_ee_channel *pChannel = (t_ee_channel *) _pElem;
    t_msg_service_header *pMsgClone;
    
    /* don't dispatch message in case nobody is listening */
    if (list.getNumberOfElem(pChannel->serviceCallbackList) == 0) {return ;}
    
    /* clone message */
    pMsgClone = (t_msg_service_header *) allocator.alloc(pMsg->messageSize);
    if (pMsgClone == (t_msg_service_header *)0) {return ;} /* message is drop ... */
    myMemcpy((void *) pMsgClone, (void *) pMsg, pMsg->messageSize);
    
    /* add additionnal information */
    pMsgClone->channel = (t_nmf_channel) pChannel;
    
    /* push it */
    nmfQueuePush(pChannel->service, (t_queue_link *) pMsgClone);
    sem.V(pChannel->sem);
    if (pChannel->cbNotify) {(*pChannel->cbNotify)(pChannel->cbNotifyContextHandler);}
}

static void executeService(t_list_link *_pElem, t_msg_service_header *pMsg)
{
    t_ee_service *pSvc = (t_ee_service *) _pElem;
    t_nmf_serviceCallback svcCb = (t_nmf_serviceCallback) pSvc->handler;
    
    (*svcCb)(pSvc->contextHandler, pMsg->serviceType, (void *) ((t_uint32) pMsg + sizeof(t_msg_service_header)));
}

/* Use a custom memcpy to avoid dependency from libc (symbian) */
static void *myMemcpy(void *s1, const void *s2, t_uint32 size)
{
    char *pDst = (char *) s1;
    char *pSrc = (char *) s2;
    
    while(size--) {*pDst++ = *pSrc++;}
    
    return s1;
}

static void myMemZero(void *s, t_uint32 size)
{
    char *pToZero = (char *) s;
    
    while(size--) {*pToZero++ = 0;}
}

