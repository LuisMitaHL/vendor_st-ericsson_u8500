/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
#include <cm/proxy/inc/osal.h>
#include <cm/proxy/api/configuration_proxy.h>
#include <cm/proxy/common/repository/inc/repository.h>
#include <cm/proxy/common/communication/inc/communication_internal.h>
#include <cm/proxy/common/utils/inc/list.h>
#include <cm/os/api/configuration_os.h>
#include <cm/os/api/repository_mgt_os.h>

/* Shared channel */
t_channel_entry *shared_channel = NULL;
t_nmf_osal_sync_handle cmProxyLock;

static t_nmf_error cm_createChannelEntry(t_channel_entry **channel, t_os_channel **OSchannel)
{
	t_channel_entry *entry = OSAL_Alloc(sizeof(*entry));

	if (entry == NULL)
		return CM_NO_MORE_MEMORY;

	entry->service_lock = OSAL_CreateLock();
	if (entry->service_lock == 0) {
		OSAL_Free(entry);
		return CM_NO_MORE_MEMORY;
	}

	entry->channel = 0;
	entry->count = 0;
	list_init_entry(&entry->service_list);
	*channel = entry;
	*OSchannel = &entry->channel;

	return CM_OK;
}

static t_nmf_error cm_deleteChannelEntry(t_channel_entry *channel, t_os_channel *OSChannel)
{
	if (!list_empty(&channel->service_list))
		return CM_INVALID_PARAMETER;

	if (OSChannel)
		*OSChannel = channel->channel;
	if (channel == shared_channel)
		shared_channel = NULL;
	OSAL_DestroyLock(channel->service_lock);
	OSAL_Free(channel);
	return CM_OK;
}

static t_nmf_error cm_createServiceEntry(t_service_entry **service_e,
					 t_nmf_serviceCallback service,
					 void *serviceCtx)
{
	t_service_entry *entry = OSAL_Alloc(sizeof(*entry));

	if (entry == NULL)
		return CM_NO_MORE_MEMORY;

	entry->count = 0;
	entry->service = service;
	entry->serviceCtx = serviceCtx;
	*service_e = entry;

	return CM_OK;
}

/* channel->service_lock must be held */
static t_service_entry *cm_getServiceEntry(t_channel_entry *channel,
					   t_nmf_serviceCallback service,
					   void *serviceCtx)
{
	t_list_entry  *entry = channel->service_list.next;

	while (entry != &channel->service_list) {
		t_service_entry  *serv = list_entry(entry, t_service_entry, this_entry);

		if (serv->service == service
		    && serv->serviceCtx == serviceCtx)
			return serv;
		entry = serv->this_entry.next;
	}
	return NULL;
}

/* channel->service_lock must be held */
static t_nmf_error cm_registerService(t_channel_entry *channel,
				      t_nmf_serviceCallback service,
				      void *serviceCtx)
{
	t_service_entry *entry;
	t_nmf_error error;

	entry = cm_getServiceEntry(channel, service, serviceCtx);
	if (entry == NULL) {
		error = cm_createServiceEntry(&entry, service, serviceCtx);
		if (error != CM_OK)
			return error;
		list_add(&entry->this_entry, &channel->service_list);

	}
	channel->count++;
	entry->count++;
	return CM_OK;
}

/* channel->service_lock must be held */
static t_nmf_error cm_unregisterService(t_channel_entry *channel,
					t_nmf_serviceCallback service,
					void *serviceCtx)
{
	t_service_entry  *entry = cm_getServiceEntry(channel,
						     service,
						     serviceCtx);
	if (entry == NULL)
		return CM_INVALID_PARAMETER;

	if (--entry->count == 0) {
		list_del(&entry->this_entry);
		OSAL_Free(entry);
	}
	--channel->count;
	return CM_OK;
}

PUBLIC void CM_INTERNAL_ProxyInit(void)
{
	cmProxyLock = OSAL_CreateLock();
}

PUBLIC void CM_INTERNAL_ProxyDestroy(void)
{
	OSAL_DestroyLock(cmProxyLock);
	cmProxyLock = 0;
}

PUBLIC t_nmf_error CM_INTERNAL_Init(void)
{
#ifdef CM_API_TIMING
	{
		int i;
		for (i=0; i<NB_CM_API; i++) {
			func[i].name = NULL;
			func[i].call_count = 0;
			func[i].time = 0;;
		}
	}
#endif
	return CM_OK;
}

PUBLIC void CM_INTERNAL_Destroy(void)
{
}

PUBLIC t_nmf_error CM_INTERNAL_CreateChannel(t_nmf_channel_flag flags,
							   t_nmf_channel *channel,
							   t_os_channel **OSchannel)
{
    t_nmf_error status = CM_OK;
    t_channel_entry *used_channel = NULL;

    if (channel == NULL || OSchannel == NULL)
	    return CM_INVALID_PARAMETER;

    switch (flags) {
    case NMF_CHANNEL_SHARED:
	    /* Use shared channel */
	    if (shared_channel == NULL)
		    status = cm_createChannelEntry(&shared_channel, OSchannel);
	    else
		    *OSchannel = NULL;
	    used_channel = shared_channel;
	    break;
    case NMF_CHANNEL_PRIVATE:
	    /* Create a new channel */
	    status = cm_createChannelEntry(&used_channel, OSchannel);
	    break;
    default:
	    status = CM_INVALID_PARAMETER;
    }

    if (status == CM_OK) {
	    used_channel->count++;
	    *channel = (t_nmf_channel)used_channel;
    }

    return status;
}

PUBLIC t_nmf_error CM_INTERNAL_FlushChannel(t_nmf_channel channel, t_os_channel *OSchannel)
{
    t_channel_entry *entry = (t_channel_entry *)channel;
    t_nmf_error status = CM_OK;
    
    /* sanity check */
    if (entry == NULL || OSchannel == NULL)
        return CM_INVALID_PARAMETER;
    
    /* in case not the last one then don't return valid *OSchannel and in this case no message will be return */
    if (entry->count == 1) 
        *OSchannel = entry->channel;
    else
        *OSchannel = 0;
    
    return status;
}

PUBLIC t_nmf_error CM_INTERNAL_CloseChannel(t_nmf_channel channel, t_os_channel *OSchannel)
{
	t_channel_entry *entry = (t_channel_entry *)channel;
	t_nmf_error status = CM_OK;

	if (entry == NULL)
		return CM_INVALID_PARAMETER;

	if (--entry->count == 0)
		status = cm_deleteChannelEntry(entry, OSchannel);
	else
		*OSchannel = 0;

	return status;
}

PUBLIC EXPORT_SHARED t_nmf_error CM_RegisterService(t_nmf_channel channel,
						    t_nmf_serviceCallback handler,
						    void *ctx)
{
    t_channel_entry *entry = (t_channel_entry *)channel;
    t_nmf_error error;

    if (entry == NULL)
	    return CM_INVALID_PARAMETER;

    if (handler == NULL)
	    return CM_INVALID_PARAMETER;

    OSAL_Lock(entry->service_lock);
    error = cm_registerService(entry, handler, ctx);
    OSAL_Unlock(entry->service_lock);

    return error;
}

PUBLIC EXPORT_SHARED t_nmf_error CM_UnregisterService(t_nmf_channel channel,
						      t_nmf_serviceCallback handler,
						      void *ctx)
{
    t_channel_entry *entry = (t_channel_entry *)channel;
    t_nmf_error error;

    if (entry == NULL)
	    return CM_INVALID_PARAMETER;

    if (handler == NULL)
	    return CM_INVALID_PARAMETER;

    OSAL_Lock(entry->service_lock);
    error = cm_unregisterService(entry, handler, ctx);
    OSAL_Unlock(entry->service_lock);

    return error;
}
