/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
#include <ee/onCMkernel/distribution.nmf>
#include <cm/inc/cm.h>
#include <ee/api/priority.idt>

static t_nmf_channel channel[MAX_SCHEDULER_PRIORITY_NUMBER];
static t_nmf_channel serviceChannel;
static void svcCallback(void *contextHandler, t_nmf_service_type serviceType, t_nmf_service_data *serviceData);
static void MessagePuller(void* args);
static void ServiceMessagePuller(void* args);
static hSem syncChannelCloseSem[MAX_SCHEDULER_PRIORITY_NUMBER];
static hSem syncServiceChannelCloseSem;

/* implement ee.api.threadStartRoutine.itf */
void METH(routine)(void *args)
{
    MessagePuller(args);
}

void METH(Sroutine)(void *args)
{
    ServiceMessagePuller(args);
}

/* implement ee.api.distribution.itf */
t_uint32 METH(notifyStart)(t_uint32 priority)
{
    t_cm_error error;
    
    /* sanity check */
    if (channel[priority])
        NMF_PANIC("distribution notifyStart : distribution at priority %d already started\n", priority);
    /* create channel + syncChannelCloseSem */
    error = CM_CreateChannel(NMF_CHANNEL_PRIVATE, &channel[priority]);
    if (error != CM_OK)
        goto error;
    syncChannelCloseSem[priority] = sem.create(0);
    if (!syncChannelCloseSem[priority])
        goto error;
    
    return 1;
error:
    if (channel[priority]) {
        CM_CloseChannel(channel[priority]);
        channel[priority] = 0;
    }
    return 0;
}

void METH(notifyStop)(t_uint32 priority)
{
    t_cm_error error;
    t_bool isFlushMessageGenerated;
    
    /* sanity check */
    if (!channel[priority])
        NMF_PANIC("distribution notifyStop : distribution at priority %d not started\n", priority);
    /* stop thread */
    error = CM_FlushChannel(channel[priority], &isFlushMessageGenerated);
    if (error == CM_OK && isFlushMessageGenerated)
        sem.P(syncChannelCloseSem[priority]);
    /* destroy channel + syncChannelCloseSem */
    if (syncChannelCloseSem[priority]) {
        sem.destroy(syncChannelCloseSem[priority]);
        syncChannelCloseSem[priority] = 0;
    }
    if (channel[priority]) {
        CM_CloseChannel(channel[priority]);
        channel[priority] = (t_nmf_channel) 0;
    }
}

t_uint32 METH(getDistributionChannel)(t_uint32 priority)
{
    return channel[priority];
}

/* implement ee.api.init.itf */
t_sint32 METH(init)()
{
    t_cm_error error = NMF_NO_MORE_MEMORY;
        
    // Create a synchronization semaphore for service channel close.
    syncServiceChannelCloseSem = sem.create(0);
    if (!syncServiceChannelCloseSem)
        goto error;
    
    // Create a service channel
    error = CM_CreateChannel(NMF_CHANNEL_PRIVATE, &serviceChannel);
    if (error != CM_OK)
        goto error;
    
    // Attach a service callback so we can dispatch mpc service message
    error = CM_RegisterService(serviceChannel, svcCallback, 0);
    if (error != CM_OK)
        goto error;
    
    return 0;
error:
    if (syncServiceChannelCloseSem) {
        sem.destroy(syncServiceChannelCloseSem);
        syncServiceChannelCloseSem = (hSem) 0;
    }
    if (serviceChannel) {
        CM_CloseChannel(serviceChannel);
        serviceChannel = (t_nmf_channel) 0;
    }
    
    return (t_sint32) error;
}

void METH(destroy)()
{
    t_cm_error error;
    t_bool isFlushMessageGenerated;
    
    CM_UnregisterService(serviceChannel, svcCallback, 0);
    error = CM_FlushChannel(serviceChannel, &isFlushMessageGenerated);
    /* In case of error then we don't block, but we do our best to destroy everythink */
    if (error == CM_OK && isFlushMessageGenerated) {
        sem.P(syncServiceChannelCloseSem);
    }
    if (syncServiceChannelCloseSem) {
        sem.destroy(syncServiceChannelCloseSem);
        syncServiceChannelCloseSem = 0;
    }
    if (serviceChannel) {
        CM_CloseChannel(serviceChannel);
        serviceChannel = (t_nmf_channel) 0;
    }
}

/* private functions */
static void MessagePuller(void* args)
{
    t_cm_error error = CM_OK;
    t_uint32 priority = (t_uint32) args;
    
    while(error != CM_FLUSH_MESSAGE) {
        char                        *buffer;
        void                        *itfref; // No wrapping in ee, then itfref is the clientContext
        
        error = CM_GetMessage(channel[priority], &itfref, &buffer, TRUE);
        if (error == CM_OK)
            CM_ExecuteMessage((t_cm_interface_desc*)itfref, buffer);
    }
    
    sem.V(syncChannelCloseSem[priority]);
}

static void ServiceMessagePuller(void* args)
{
    t_cm_error error = CM_OK;
    
    while(error != CM_FLUSH_MESSAGE) {
        char                        *buffer;
        void                        *itfref; // No wrapping in ee, then itfref is the clientContext
        
        error = CM_GetMessage(serviceChannel, &itfref, &buffer, TRUE);
        if (error == CM_OK)
            CM_ExecuteMessage((t_cm_interface_desc*)itfref, buffer);
    }
    
    NMF_LOG("Leaving service thread\n");
    
    sem.V(syncServiceChannelCloseSem);
}

static void svcCallback(void *contextHandler, t_nmf_service_type serviceType, t_nmf_service_data *serviceData)
{
    t_msg_service svcMsg;

    /* NOTE : message is allocated on stack since it will be copy for each channel by nmfPushServiceMessage() */
    /* copy message in svcMsg */
    svcMsg.header.msg.type = EE_CB_SVC_MSG;
    svcMsg.header.serviceType = serviceType;
    svcMsg.header.messageSize = sizeof(t_msg_service);
    svcMsg.data = *serviceData;

    /* give it to ee to it can dispatch on channel */
    nmfPushServiceMessage(&svcMsg.header);
}
