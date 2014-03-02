/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
#include <cm/os/api/cm_os.h>
#include <cm/proxy/api/cm_proxy.h>
#include <cm/proxy/inc/osal.h>

#include <cm/proxy/common/communication/inc/hoststubs_mgt.h>
#include <cm/proxy/common/communication/inc/communication_internal.h>
#include <cm/proxy/common/repository/inc/repository.h>
#include <inc/nmf-limits.h>

t_cm_error CM_BindComponentFromUserR(
        const t_cm_instance_handle server,
        const char* providedItfServerName,
        t_uint32 fifosize,
        t_cm_mpc_memory_type eventMemType,
        t_nmf_interface_desc* itfref,
        t_nmf_bc_coding_style codingStyle)
{
    t_cm_error error;

    // Sanity check
    if ((server == 0) || (providedItfServerName == NULL) || (itfref == NULL)
        || (fifosize == 0) || (fifosize > MAX_COMMUNICATION_FIFO_SIZE))
            return CM_INVALID_PARAMETER;

    {
        t_nmf_init_stub_fct initStubFct;
        t_cm_bf_host2mpc_handle *host2mpc;
        char components[1][MAX_INTERFACE_TYPE_NAME_LENGTH];
        t_nmf_osal_fs_file_handle files[1];
        void *datas[1];
        t_cm_size sizes[1];
        char providetype[MAX_INTERFACE_TYPE_NAME_LENGTH];

        if((error = CM_OS_GetRequiredComponentFiles(BIND_FROMUSER, 0, NULL, server, providedItfServerName,
                components, 1, providetype, NULL)) != CM_OK) {
            return error;
        }

        /* Get Host Stub information */
        if ((error = cm_HSM_getItfStubInitFct(codingStyle, providetype, &initStubFct)) != CM_OK)
        {
            OSAL_Log("Stubs functions (%s in %s) not registered\n", (void*)providetype, codingStyle == NMF_BC_IN_C ? "C" : "C++", 0);
            return error;
        }

        /* Allocate host2dsp binding factory information */
        host2mpc = initStubFct(itfref);
        if(host2mpc == 0x0) {
            return CM_NO_MORE_MEMORY;
        }

        if((error = cm_openComponents(1, components, files, datas, sizes, 0)) != CM_OK)
            return CM_BINDING_COMPONENT_NOT_FOUND;

        error = CM_OS_BindComponentFromCMCore(server,
                providedItfServerName,
                fifosize,
                eventMemType,
                host2mpc,
                (const char*)datas[0], sizes[0]);

        cm_closeComponents(1, files);
    }

    return error;
}

t_cm_error CM_UnbindComponentFromUserR(t_cm_bf_host2mpc_handle host2mpc)
{
    t_cm_error error;

    error = CM_OS_UnbindComponentFromCMCore(host2mpc);

    return error;
}

t_cm_error CM_BindComponentToUserR(
        const t_nmf_channel channel,
        const t_cm_instance_handle client,
        const char* requiredItfClientName,
        void* clientContext,
        t_uint32 fifosize,
        t_nmf_bc_coding_style       codingStyle)
{
    t_cm_error error;
    t_channel_entry *entry = (t_channel_entry *)channel;

    /* Sanity check */
    if ((client == 0) || (requiredItfClientName == NULL) ||
            (fifosize == 0) || (fifosize > MAX_COMMUNICATION_FIFO_SIZE)
            || (entry == NULL))
        return CM_INVALID_PARAMETER;

    {
        t_nmf_dsp_arm_this *skelthis;
        char components[1][MAX_INTERFACE_TYPE_NAME_LENGTH];
        t_nmf_osal_fs_file_handle files[1];
        void *datas[1];
        t_cm_size sizes[1];
        char requiretype[MAX_INTERFACE_TYPE_NAME_LENGTH];
        t_uint32 methodNumber;

        if((error = CM_OS_GetRequiredComponentFiles(BIND_TOUSER, client, requiredItfClientName, 0, NULL,
                components, 1, requiretype, &methodNumber)) != CM_OK) {
            return error;
        }

        /* Create skeleton context */
        skelthis = (t_nmf_dsp_arm_this*)OSAL_Alloc(sizeof(t_nmf_dsp_arm_this));
        if(skelthis == 0) {
            return CM_NO_MORE_MEMORY;
        }

        /* Get Host Skeleton information */
        if ((error = cm_HSM_getItfSkelInitFct(codingStyle, requiretype, &skelthis->jumpMethods)) != CM_OK)
        {
            OSAL_Log("Skels functions (%s in %s) not registered\n", (void*)requiretype, codingStyle == NMF_BC_IN_C ? "C" : "C++", 0);
            OSAL_Free(skelthis);
            return error;
        }

        if((error = cm_openComponents(1, components, files, datas, sizes, 0)) != CM_OK)
        {
            OSAL_Free(skelthis);
            return CM_BINDING_COMPONENT_NOT_FOUND;
        }

        if((error = CM_OS_BindComponentToCMCore(
                entry->channel,
                client, requiredItfClientName,
                fifosize,
                (t_nmf_mpc2host_handle)skelthis,
                (const char*)datas[0], sizes[0],
                &skelthis->mpc2hostId)) != CM_OK)
        {
            cm_closeComponents(1, files);
            OSAL_Free(skelthis);
            return error;
        }

        skelthis->clientContext = clientContext;

        cm_closeComponents(1, files);
    }

    return CM_OK;
}

t_cm_error CM_UnbindComponentToUserR(
	const t_nmf_channel channel,
        const t_cm_instance_handle client,
        const char* requiredItfClientName,
        void **clientContext)
{
    t_nmf_dsp_arm_this *skelthis;
    t_cm_error error;
    t_channel_entry *entry = (t_channel_entry *)channel;

    /* Sainty check */
    if ((client == 0) || (requiredItfClientName == NULL))
	    return CM_INVALID_PARAMETER;

    error = CM_OS_UnbindComponentToCMCore(
	    entry->channel, client, requiredItfClientName,
            (t_nmf_mpc2host_handle*)&skelthis);

    if(error == CM_OK || error == CM_MPC_NOT_RESPONDING) {
        // Force clean in case of MPC not responding error
        if (clientContext != NULL)
            *clientContext = skelthis->clientContext;
        OSAL_Free(skelthis);
    }

    return error;
}

PUBLIC EXPORT_SHARED void CM_ExecuteMessage(
        t_nmf_interface_desc *      itfref,
        char *                      message) {
    t_nmf_message *msg = (t_nmf_message *)message;

    switch (msg->osMsg.type)
    {
    case MSG_INTERFACE: {
	    // Call skeleton to call user callback
	    t_interface_data *itf = &msg->osMsg.data.itf;
	    t_nmf_dsp_arm_this *skelthis = (t_nmf_dsp_arm_this *)itf->THIS;

	    skelthis->jumpMethods[itf->methodIndex](itfref, (t_event_params_handle)itf->params);
	    break;
    }
    case MSG_SERVICE: {
	    // Call each registered service routine
	    t_channel_entry *channel = (t_channel_entry *)msg->channel;
	    t_service_data *srv = &msg->osMsg.data.srv;
	    t_service_entry *srvCall;

	    OSAL_Lock(channel->service_lock);
	    srvCall = list_entry(channel->service_list.next, t_service_entry, this_entry);
	    while (&srvCall->this_entry != &channel->service_list) {
		    srvCall->service(srvCall->serviceCtx, srv->type, (void *)&srv->data);
		    srvCall = list_entry(srvCall->this_entry.next, t_service_entry, this_entry);
	    }
	    OSAL_Unlock(channel->service_lock);
	    break;
    }
    }
}

PUBLIC EXPORT_SHARED t_cm_error CM_GetMessage(
        t_nmf_channel               channel,
        void                        **clientContext,
        char                        **message,
        t_bool                      blockable)
{
    t_channel_entry *entry = (t_channel_entry *)channel;
    t_cm_error error;
    t_nmf_message *msg = (t_nmf_message*)entry->msg;

    /* sanity check */
    if ((clientContext == 0) || (entry == NULL))
	    return CM_INVALID_PARAMETER;

    /* fill message*/
    /* coverity[overrun-buffer-arg : FALSE] */
    error = CM_OS_GetMessage(entry->channel, &msg->osMsg, sizeof(entry->msg)-sizeof(msg->channel), blockable);

    /* if error == CM_NO_MORE_MEMORY => should realloc a bigger size and try again */
    if (error != CM_OK) {
	    return error;
    }

    msg->channel = channel;
    if (msg->osMsg.type == MSG_INTERFACE)
    {
	t_nmf_dsp_arm_this *skelthis = (t_nmf_dsp_arm_this *)msg->osMsg.data.itf.THIS;
	*clientContext = (void*) skelthis->clientContext;
    }

    *message = (char *)msg;
    return error;
}

#ifdef CM_API_TIMING
struct func_entry func[NB_CM_API];
#endif /* CM_API_TIMING */

/*!
 * Prints all timing about all traced CM API and reset the counters
 */
EXPORT_SHARED void CM_PrintNmfUsage(void) {
#ifdef CM_API_TIMING
	int i;
	unsigned int time = 0;
	for (i=0; i<NB_CM_API; i++) {
		if (func[i].name) {
			OSAL_Log("NMF: %s called %d times for %u us",
				 (void *)func[i].name,
				 (void *)func[i].call_count,
				 (void *)func[i].time);
			OSAL_Log(" (%d us per call)\n",
				 (void *)(func[i].time/func[i].call_count), 0, 0);
			time += func[i].time;
			func[i].name = NULL;
			func[i].time = 0;
			func[i].call_count = 0;
		}
	}
	OSAL_Log("NMF: Total NMF time: %u us\n", (void *)time, 0, 0);
#endif /* CM_API_TIMING */
}
