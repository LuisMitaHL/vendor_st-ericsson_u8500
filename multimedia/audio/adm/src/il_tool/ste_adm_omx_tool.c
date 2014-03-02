/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#define ADM_LOG_FILENAME "omx_tool"

#include "ste_adm_omx_tool.h"
#include "ste_adm_util.h"
#include "ste_adm_platform_adaptions.h"
#include "ste_adm_dbg.h"
#include "ste_adm_omx_log.h"
#include "ste_adm_omx_io.h"
#include "ste_adm_srv.h"
#include <stdlib.h>
#include "ENS_Index.h"
#include "AFM_Core.h"
#include "OMX_CoreExt.h"
#include "OMX_STE_AudioEffectsExt.h"
#include <errno.h>
#include <string.h>
#include <sys/time.h>
#include <sys/resource.h>


#define OSI_TRACE_ERROR     (1<<0)
#define OSI_TRACE_WARNING   (1<<1)
#define OSI_TRACE_FLOW      (1<<2)
#define OSI_TRACE_DEBUG     (1<<3)
#define OSI_TRACE_API       (1<<4)
#define OSI_TRACE_OMX_API   (1<<5)
#define OSI_TRACE_BUFFER    (1<<6)


#define RETURN_IF_FAIL(__f) \
    { \
        ste_adm_res_t r = __f; \
        if (r != STE_ADM_RES_OK) { \
            ALOG_ERR("Error %d in '%s'\n", r, # __f); \
            return r; \
        } \
    }

// Last opened device
static char g_last_opened_dev[ADM_MAX_DEVICE_NAME_LENGTH];


typedef struct
{
    OMX_U32 nSize;                  /**< size of the structure in bytes */
    OMX_VERSIONTYPE nVersion;       /**< OMX specification version information */
} IL_Base_t;


// FUTURE IMPROVEMENT: Get rid of this
extern adm_sem_t g_adm_event_sem;
static int g_adm_state_transition_timeout;

static int is_fatal_error(OMX_ERRORTYPE error);
static int il_tool_cmd_in_progress(il_comp_t comp);

// TODO: Discuss how errors are reset after occuring
static OMX_ERRORTYPE il_tool_last_error(il_comp_t comp, OMX_U32* Data2_p);

void il_tool_enable_trace(il_comp_t handle, OMX_U16 traceEnable, OMX_U32 parentHandle)
{
    ENS_CONFIG_TRACESETTINGTYPE trace_cfg;
    IL_TOOL_INIT_CONFIG_STRUCT(trace_cfg);

    trace_cfg.nParentHandle = parentHandle;
    trace_cfg.nTraceEnable  = traceEnable;

    ste_adm_res_t res = il_tool_log_SetConfig(handle, ENS_IndexConfigTraceSetting, &trace_cfg);
    if (res != STE_ADM_RES_OK) {
        ALOG_ERR("0X%x ENS_IndexConfigTraceSettin FAILED 0x%x\n ", handle, res);
    } else {
        ALOG_INFO("0X%x ENS_IndexConfigTraceSettin OK\n ", handle);
    }
}

static int is_supplier(il_comp_t src_comp, OMX_U32 src_port)
{
    OMX_PARAM_BUFFERSUPPLIERTYPE bufsup;
    IL_TOOL_INIT_CONFIG_STRUCT(bufsup);
    bufsup.nPortIndex = src_port;
    il_tool_GetParameter(src_comp, OMX_IndexParamCompBufferSupplier, &bufsup);
    return bufsup.eBufferSupplier == OMX_BufferSupplyOutput;
}

void il_tool_subscribe_events(il_comp_t comp_h, il_tool_callback_t cb, void* cb_param, int must_be_serialized)
{
    pthread_mutex_lock(&comp_h->mutex);
    ADM_ASSERT(comp_h->callback_state.event_callback_fp == NULL ||
           comp_h->callback_state.event_callback_fp == cb);

    comp_h->callback_state.event_callback_fp = cb;
    comp_h->callback_state.event_callback_param = cb_param;
    comp_h->callback_state.event_callback_must_be_serialized = must_be_serialized;
    pthread_mutex_unlock(&comp_h->mutex);
}

void il_tool_unsubscribe_events(il_comp_t comp_h)
{
    pthread_mutex_lock(&comp_h->mutex);
    ADM_ASSERT(comp_h->callback_state.event_callback_fp != NULL);
    comp_h->callback_state.event_callback_fp = NULL;
    pthread_mutex_unlock(&comp_h->mutex);

    srv_read_and_drop_il_messages(comp_h, OMX_ALL, 0, 0);
}

void ste_adm_omx_tool_handle_event(il_comp_t hComponent,
        OMX_EVENTTYPE eEvent,
        OMX_U32 Data1, OMX_U32 Data2,
        OMX_PTR pEventData)
{
    pthread_mutex_lock(&hComponent->mutex);
    il_tool_callback_t cb_fb    = hComponent->callback_state.event_callback_fp;
    void*              cb_param = hComponent->callback_state.event_callback_param;
    pthread_mutex_unlock(&hComponent->mutex);

    if (cb_fb) {
        cb_fb(cb_param, hComponent, eEvent, Data1, Data2, pEventData);
    }
}

int ste_adm_omx_tool_handle_event_must_be_serialized(il_comp_t hComponent)
{
    pthread_mutex_lock(&hComponent->mutex);
    int val = hComponent->callback_state.event_callback_must_be_serialized;
    pthread_mutex_unlock(&hComponent->mutex);

    return val;
}

void il_tool_event_cb(il_comp_t hComponent,
        OMX_EVENTTYPE eEvent,
        OMX_U32 Data1, OMX_OUT OMX_U32 Data2,
        OMX_PTR pEventData)
{
    ADM_ASSERT(hComponent != NULL);
    (void) pEventData;

#if 0
    {
        struct sched_param sched_param;
        int    sched_policy;
        int    nice_prio;

        pthread_getschedparam(pthread_self(), &sched_policy, &sched_param);
        ALOG_STATUS("SCHED_OTHER=%d, SCHED_FIFO=%d, SCHED_RR=%d\n", SCHED_OTHER, SCHED_FIFO, SCHED_RR);
        ALOG_STATUS("IL EVENT callback thread info: getpriority=%d pthread.policy=%d pthread.sched_priority=%d\n",
                    getpriority(PRIO_PROCESS, gettid()), sched_policy, sched_param.sched_priority);
    }
#endif

    if (eEvent == OMX_EventCmdComplete) {
        if (Data1 == OMX_CommandPortEnable ||
            Data1 == OMX_CommandPortDisable ||
            Data1 == OMX_CommandStateSet)
            {
                // Unless an error has occured (which may have aborted the
                // current command), a command must be in progress
                // TODO if (il_data->cmd_in_progress) {
                adm_sem_post(&g_adm_event_sem);
                // TODO     il_data->cmd_in_progress = 0;
                // TODO } else {
                    // TODO ALOG_ERR("Unexpected OMX_EventCmdComplete received, last_err=%d\n", il_data->last_error);
                // TODO }
            }
    }

    if (eEvent == OMX_EventError) {
        if ((AFM_ERRORTYPE) Data1 == AFM_ErrorDspPanic) {
            ALOG_ERR("AFM_ErrorDspPanic received\n");
            ste_adm_debug_reboot();
        }
        if (is_fatal_error(Data1)) {
            pthread_mutex_lock(&hComponent->mutex);
            adm_il_tool_state_t* il_data = &hComponent->callback_state;
            if (il_data->cmd_in_progress) {
                adm_sem_post(&g_adm_event_sem);
                il_data->cmd_in_progress  = 0;
                il_data->last_error       = Data1;
                il_data->last_error_Data2 = Data2;
            }
            pthread_mutex_unlock(&hComponent->mutex);
        }
    }

    int handled_event = 0;
    if ((OMX_EVENTEXTTYPE)eEvent == OMX_EventIndexSettingChanged) {
        pthread_mutex_lock(&hComponent->mutex);

        adm_il_tool_state_t* il_data = &hComponent->callback_state;
        if (il_data->waiting_for_event && il_data->waiting_for_event_idx == (int) Data2) {
            il_data->waiting_for_event = 0;
            handled_event = 1;
            adm_sem_post(&g_adm_event_sem);
        }

        pthread_mutex_unlock(&hComponent->mutex);
    }
    if (!handled_event && eEvent != OMX_EventCmdComplete) {
        srv_send_il_event_msg(hComponent, eEvent,
        Data1, Data2, pEventData);
    }

}

void il_tool_cb_empty_buffer_done(
                            il_comp_t           component_handle,
                            OMX_BUFFERHEADERTYPE*    buffer_done_p)
{
#if 0
    {
        struct sched_param sched_param;
        int    sched_policy;
        int    nice_prio;

        pthread_getschedparam(pthread_self(), &sched_policy, &sched_param);
        ALOG_STATUS("SCHED_OTHER=%d, SCHED_FIFO=%d, SCHED_RR=%d\n", SCHED_OTHER, SCHED_FIFO, SCHED_RR);
        ALOG_STATUS("IL ETB callback thread info: getpriority=%d pthread.policy=%d pthread.sched_priority=%d\n",
                    getpriority(PRIO_PROCESS, gettid()), sched_policy, sched_param.sched_priority);
    }
#endif

    ADM_ASSERT(component_handle != NULL);
    ADM_ASSERT(buffer_done_p != NULL);

    srv_send_il_buffer_msg(component_handle, buffer_done_p, CB_TYPE_BUFFER_EMPTIED);
}

void il_tool_cb_fill_buffer_done(
                            il_comp_t          component_handle,
                            OMX_OUT OMX_BUFFERHEADERTYPE*   buffer_done_p)
{
    ADM_ASSERT(component_handle != NULL);
    ADM_ASSERT(buffer_done_p != NULL);

    srv_send_il_buffer_msg(component_handle, buffer_done_p, CB_TYPE_BUFFER_FILLED);
}


int il_tool_is_dsp_component(il_comp_t comp)
{
    char component_name[OMX_MAX_STRINGNAME_SIZE];
    OMX_VERSIONTYPE componentVersion;
    OMX_VERSIONTYPE specVersion;

    if (STE_ADM_RES_OK == il_tool_GetComponentVersion(comp,
              component_name, &componentVersion, &specVersion, NULL))
    {
        return adaptions_is_dsp_component(component_name);
    }

    ALOG_ERR("il_tool_is_dsp_component: il_tool_GetComponentVersion failed for comp %X\n", (int) il_tool_raw_handle(comp));
    return 0;
}

ste_adm_res_t il_tool_create_component(il_comp_t* comp_hp, const char* name, const char* dbg_type)
{
    ADM_ASSERT(comp_hp != NULL);
    ADM_ASSERT(name    != NULL);

    // GetHandle sometimes takes time
    SRV_DBG_ASSERT_IS_WORKER_THREAD;

    adm_il_appdata_t* app_data_p = malloc(sizeof(*app_data_p));
    if (!app_data_p) {
        ALOG_ERR("Out of memory allocating %d bytes\n", sizeof(*app_data_p));
        return STE_ADM_RES_ERR_MALLOC;
    }

    memset(app_data_p, 0, sizeof(*app_data_p));
    il_tool_io_init(app_data_p->bufinfo);
    *comp_hp = app_data_p;
    app_data_p->callback_state.last_error = OMX_ErrorNone;


    app_data_p->callback_state.event_callback_fp = NULL;


    int retval = pthread_mutex_init(&app_data_p->mutex, NULL);
    if (retval != 0) {
        ALOG_ERR("pthread_mutex_init failed, errno = %d\n", errno);
        free(app_data_p);
        return STE_ADM_RES_ERR_MALLOC;
    }

    ste_adm_res_t res = il_tool_log_CreateComponent(name, app_data_p);
    if (res != STE_ADM_RES_OK)
    {
        pthread_mutex_destroy(&app_data_p->mutex);
        free(app_data_p);
        ALOG_ERR("Error: il_tool_log_CreateComponent returned %d\n", res);
        return res;
    }

    if (ste_adm_debug_omx_trace_enabled(name, dbg_type)) {
        ALOG_INFO("Enabling OST traces for '%s', '%s'\n", name, dbg_type);
        OMX_HANDLETYPE omx_h = il_tool_raw_handle(*comp_hp);

        // Enable all OSI traces except buffer traces (they are very verbose)
        il_tool_enable_trace(*comp_hp, (OMX_U16) 65535 & ~OSI_TRACE_BUFFER, (OMX_U32) (omx_h));
    }

    res = adm_adaptions_setup_comp(*comp_hp, name);


    if (res != STE_ADM_RES_OK) {
        ALOG_ERR("adm_adaptions_setup_comp failed for %s, err=%X\n", name, res);
        il_tool_log_FreeHandle(*comp_hp);
        pthread_mutex_destroy(&app_data_p->mutex);
        free(app_data_p);
        *comp_hp = NULL;
        return res;
    }
    return STE_ADM_RES_OK;
}


// TODO: Note that il_tool_destroy_component and io dealloc/alloc runs in
// worker thread! Thus, they, compared to everything else, access ADM state
// from the 'other' thread.
void il_tool_destroy_component(il_comp_t* comp_pp)
{
    SRV_DBG_ASSERT_IS_WORKER_THREAD;
    ADM_ASSERT(comp_pp  != NULL);
    ADM_ASSERT(*comp_pp != NULL);

    int res;
    res = pthread_mutex_destroy(&(*comp_pp)->mutex);
    ADM_ASSERT(res == 0);
    if (res != 0) {
        ALOG_ERR("pthread_mutex_destroy failed, errno=%d\n", errno);
    }

    // This will call OMX_FreeHandle() in the end. OMX_FreeHandle() has to be done while
    // holding the queue mutex, which is why this is done by ste_adm_srv.c
    srv_read_and_drop_il_messages(*comp_pp, OMX_ALL, 0, 1);

    memset(*comp_pp, 0xAE, sizeof(*comp_pp));
    free(*comp_pp);
    *comp_pp = NULL;
}

/**
* enabled=1 io_fd=-1    --> enable port, do not attach io
* enabled=1 io_fd!=-1   --> enable port, attach io to specified fd
*
* enabled=0 io_fd=-1    --> disable port, do not dettach io
* enabled=0 io_fd==0    --> disable port, dettach io
*
*/
ste_adm_res_t il_tool_change_port_state(il_comp_t comp_h, OMX_U32 port, int enabled, int alloc_bufs)
{
    SRV_DBG_ASSERT_IS_WORKER_THREAD;
    ADM_ASSERT(comp_h != NULL);
    ADM_ASSERT(alloc_bufs == 1 || alloc_bufs == 0);

    // Check if port is already in desired state; this is OK for this call.
    OMX_PARAM_PORTDEFINITIONTYPE port_def;
    port_def.nSize             = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
    port_def.nVersion.nVersion = OMX_VERSION;
    port_def.nPortIndex        = port;


    ste_adm_res_t res = il_tool_GetParameter(comp_h,
            OMX_IndexParamPortDefinition, &port_def);
    if (res != STE_ADM_RES_OK) {
        ALOG_ERR("OMX_GetParameter failed\n");
        return res;
    }

    if ((enabled && port_def.bEnabled) || (!enabled && !port_def.bEnabled)) {
        ALOG_INFO("il_tool_change_port_state: Port %d already in requested state\n", port);
        return STE_ADM_RES_OK;
    }

    if (enabled) {
        res = il_tool_SendCommand(comp_h, OMX_CommandPortEnable,
                                         port, NULL);
    } else {
        if (alloc_bufs) {
            il_tool_io_move_towards_idle(comp_h, port);
        }

        res = il_tool_SendCommand(comp_h, OMX_CommandPortDisable,
                                         port, NULL);

        if (alloc_bufs) {
            il_tool_io_wait_until_idle(comp_h, port);
        }
    }

    if (res != STE_ADM_RES_OK) {
        ALOG_ERR("OMX_SendCommand failed\n");
        return res;
    }

    if (alloc_bufs) {
        if (enabled) {
            if (il_tool_io_alloc(comp_h, port, -1)) {
                ALOG_ERR("Failed to allocate io\n");
                return STE_ADM_RES_IL_UNKNOWN_ERROR;
            }
        } else {
            if (il_tool_io_dealloc(comp_h, port)) {
                ALOG_ERR("Problems during buffer deallocation\n");
                return STE_ADM_RES_IL_UNKNOWN_ERROR;
            }
        }
    }

    ADM_SEM_WAIT_WITH_PRINT("il_tool_change_port_state comp=%p port=%d target_state=%s alloc_bufs=%d", il_tool_raw_handle(comp_h), port,
            enabled ? "Enabled" : "Disabled", alloc_bufs);


    // The component must not be busy now (since the
    // semaphore has been taken). Check that no error has
    // occured.
    ADM_ASSERT(!il_tool_cmd_in_progress(comp_h));
    if (il_tool_last_error(comp_h, NULL) != OMX_ErrorNone) {
        ALOG_ERR("Component has received error event\n");
        return STE_ADM_RES_IL_UNKNOWN_ERROR; // FUTURE IMPROVEMENT: Propagate error from event
    }

    return STE_ADM_RES_OK;
}

ste_adm_res_t il_tool_disable_all_except(il_comp_t comp_h,
    OMX_U32* ports_to_keep_enabled, int num_port_to_keep)
{
    SRV_DBG_ASSERT_IS_WORKER_THREAD;
    static const OMX_INDEXTYPE port_type_index[4] = {
        OMX_IndexParamAudioInit,   OMX_IndexParamImageInit,
        OMX_IndexParamVideoInit,   OMX_IndexParamOtherInit
    };

    int type_idx;
    for (type_idx=0 ; type_idx < 4 ; type_idx++) {
        OMX_PORT_PARAM_TYPE port_param;
        port_param.nSize             = sizeof(OMX_PORT_PARAM_TYPE);
        port_param.nVersion.nVersion = OMX_VERSION;

        ste_adm_res_t res = il_tool_GetParameter(comp_h,
                    port_type_index[type_idx], &port_param);
        if (res != STE_ADM_RES_OK)
            return res;

        OMX_U32 first_port = port_param.nStartPortNumber;
        OMX_U32 last_port  = port_param.nStartPortNumber + port_param.nPorts;

        // Go through all ports ([first_port,last_port]) and disable them
        // unless they are in the ports_to_keep_enabled list.
        OMX_U32 port_idx;
        for (port_idx=first_port ; port_idx < last_port ; port_idx++) {
            int i;
            for (i=0 ; i < num_port_to_keep ; i++) {
                if (port_idx == ports_to_keep_enabled[i])
                    break;
            }

            if (i == num_port_to_keep) {
                // 0(third argument): disable port
                // -1(fourth argument): no IO
                res = il_tool_change_port_state(comp_h, port_idx, 0, 0);
                if (res != STE_ADM_RES_OK) {
                    return res;
                }
            }
        }
    }

    return STE_ADM_RES_OK;
}

ste_adm_res_t il_tool_connect_and_enable(il_comp_t src_comp,
                                         OMX_U32 src_port, il_comp_t dst_comp,
                                         OMX_U32 dst_port, int enable_as_well, int propagate_cfg,
                                         int use_dst_cfg, OMX_BUFFERSUPPLIERTYPE supplier)
{
    ste_adm_res_t res;

    SRV_DBG_ASSERT_IS_WORKER_THREAD;
    ADM_ASSERT(src_comp != NULL);
    ADM_ASSERT(dst_comp != NULL);

    // The components must not be busy
    ADM_ASSERT(!il_tool_cmd_in_progress(dst_comp));
    ADM_ASSERT(!il_tool_cmd_in_progress(src_comp));


    //
    // Propagate PCM settings...
    //
    if (propagate_cfg) {
        OMX_AUDIO_PARAM_PCMMODETYPE pcm_params;
        IL_TOOL_INIT_CONFIG_STRUCT(pcm_params);

        // Which should settings be retrieved from?
        il_comp_t cfg_src       = use_dst_cfg ? dst_comp : src_comp;
        OMX_U32   cfg_src_port  = use_dst_cfg ? dst_port : src_port;
        il_comp_t cfg_dst       = use_dst_cfg ? src_comp : dst_comp;
        OMX_U32   cfg_dst_port  = use_dst_cfg ? src_port : dst_port;


        pcm_params.nPortIndex = cfg_src_port;
        res = il_tool_GetParameter(cfg_src,
                OMX_IndexParamAudioPcm, &pcm_params);
        if (res != STE_ADM_RES_OK)
            return res;


        pcm_params.nPortIndex = cfg_dst_port;
        res = il_tool_SetParameter(cfg_dst, OMX_IndexParamAudioPcm, &pcm_params);
        if (res != STE_ADM_RES_OK)
            return res;
    }

    //
    // Setup the tunnel
    //
    res = il_tool_SetupTunnel(src_comp, src_port, dst_comp, dst_port);
    if (res != STE_ADM_RES_OK)
        return res;

    //
    // Change buffer provider
    //
    if (supplier != OMX_BufferSupplyUnspecified) {
        OMX_PARAM_BUFFERSUPPLIERTYPE bufsup;
        IL_TOOL_INIT_CONFIG_STRUCT(bufsup);
        bufsup.eBufferSupplier = supplier;
        bufsup.nPortIndex = dst_port;
        res = il_tool_SetParameter(dst_comp, OMX_IndexParamCompBufferSupplier, &bufsup);
        if (res != STE_ADM_RES_OK)
            return res;
    }

    //
    // If requested to do so, we enable the ports (and wait until
    // that operation is complete).
    //
    if (!enable_as_well)
        return STE_ADM_RES_OK;

    int src_is_supplier = is_supplier(src_comp, src_port);

    // The order in which we enable the ports is critical! It must first
    // be the component which receives the buffers, then the other.
    il_comp_t sup_comp = src_is_supplier ? src_comp : dst_comp;
    OMX_U32 sup_port   = src_is_supplier ? src_port : dst_port;
    il_comp_t rec_comp = src_is_supplier ? dst_comp : src_comp;
    OMX_U32 rec_port   = src_is_supplier ? dst_port : src_port;

    res = il_tool_SendCommand(rec_comp, OMX_CommandPortEnable,
                                     rec_port, NULL);
    if (res != STE_ADM_RES_OK)
        return res;


    res = il_tool_SendCommand(sup_comp, OMX_CommandPortEnable,
                                     sup_port, NULL);
    if (res != STE_ADM_RES_OK)
        return res;


    // Wait for two semaphore 'clicks'
    int i;
    for (i=0 ; i<2 ; i++) {
        ADM_SEM_WAIT_WITH_PRINT("il_tool_connect_and_enable %p.%d --> %p.%d sup=%p enable=%d dst_cfg=%d %d/2",
            il_tool_raw_handle(src_comp), src_port, il_tool_raw_handle(dst_comp), dst_port, sup_comp, enable_as_well, use_dst_cfg, i+1);
    }


    // The components must not be busy now (since the
    // semaphore has been taken)
    ADM_ASSERT(!il_tool_cmd_in_progress(dst_comp));
    ADM_ASSERT(!il_tool_cmd_in_progress(src_comp));

    if (il_tool_last_error(dst_comp, NULL) != OMX_ErrorNone) {
        ALOG_ERR("Destcomponent received error event\n");
        return STE_ADM_RES_IL_UNKNOWN_ERROR;
    }

    if (il_tool_last_error(src_comp, NULL) != OMX_ErrorNone) {
        ALOG_ERR("Srccomponent received error event\n");
        return STE_ADM_RES_IL_UNKNOWN_ERROR;
    }

    return STE_ADM_RES_OK;
}

ste_adm_res_t il_tool_disconnect(il_comp_t src_comp, OMX_U32 src_port, il_comp_t dst_comp, OMX_U32 dst_port)
{
    SRV_DBG_ASSERT_IS_WORKER_THREAD;
    ADM_ASSERT(src_comp != NULL);
    ADM_ASSERT(dst_comp != NULL);

    // The components must not be busy
    ADM_ASSERT(!il_tool_cmd_in_progress(dst_comp));
    ADM_ASSERT(!il_tool_cmd_in_progress(src_comp));

    //GLOBAL_SEM_ACTIVATE

    // The order in which we disable the ports is critical! It must first
    // be the component which sends the buffers, then the other.
    int src_is_supplier = is_supplier(src_comp, src_port);

    il_comp_t sup_comp = src_is_supplier ? src_comp : dst_comp;
    OMX_U32 sup_port   = src_is_supplier ? src_port : dst_port;
    il_comp_t rec_comp = src_is_supplier ? dst_comp : src_comp;
    OMX_U32 rec_port   = src_is_supplier ? dst_port : src_port;

    ste_adm_res_t res;
    res = il_tool_SendCommand(sup_comp, OMX_CommandPortDisable,
                                     sup_port, NULL);
    if (res != STE_ADM_RES_OK)
        return res;

    res = il_tool_SendCommand(rec_comp, OMX_CommandPortDisable,
                                     rec_port, NULL);
    if (res != STE_ADM_RES_OK)
        return res;


    // Wait for two semaphore 'clicks'
    int i;
    for (i=0 ; i<2 ; i++) {
        ADM_SEM_WAIT_WITH_PRINT("il_tool_disconnect %p.%d --> %p.%d, supplier %p", il_tool_raw_handle(src_comp), src_port, il_tool_raw_handle(dst_comp), dst_port, sup_comp);
    }

    // The components must not be busy now (since the
    // semaphore has been taken)
    ADM_ASSERT(!il_tool_cmd_in_progress(sup_comp));
    ADM_ASSERT(!il_tool_cmd_in_progress(rec_comp));

    if (il_tool_last_error(sup_comp, NULL) != OMX_ErrorNone) {
        ALOG_ERR("Destcomponent received error event\n");
        return STE_ADM_RES_IL_UNKNOWN_ERROR;
    }

    if (il_tool_last_error(rec_comp, NULL) != OMX_ErrorNone) {
        ALOG_ERR("Srccomponent received error event\n");
        return STE_ADM_RES_IL_UNKNOWN_ERROR;
    }

    return STE_ADM_RES_OK;
}


ste_adm_res_t il_tool_enter_state(OMX_STATETYPE target_state, il_comp_t* components, int num_components)
{
    SRV_DBG_ASSERT_IS_WORKER_THREAD;
    ADM_ASSERT(components != NULL);
    ADM_ASSERT(num_components > 0);
    ADM_ASSERT(target_state == OMX_StateLoaded ||
           target_state == OMX_StateIdle   ||
           target_state == OMX_StateExecuting);

    int i;
    for (i=0 ; i < num_components ; i++) {
        ADM_ASSERT(components[i] != NULL);
        ADM_ASSERT(!il_tool_cmd_in_progress(components[i]));

        components[i]->callback_state.cmd_in_progress = 1;

        ste_adm_res_t res = il_tool_SendCommand(components[i],
                                         OMX_CommandStateSet,
                                         target_state, NULL);
        if (res != STE_ADM_RES_OK) {
            return res;
        }
    }

    // Wait for semaphore 'clicks' from all components
    int sem_timed_out = 0;
    for (i=0 ; i<num_components && !sem_timed_out ; i++) {
        if (g_adm_state_transition_timeout == 0) {
            ADM_SEM_WAIT_WITH_PRINT("il_tool_enter_state: state=%d %d/%d\n", target_state, i+1, num_components);
        } else {
            ALOG_INFO("Waiting for semaphore, with timeout %d\n", g_adm_state_transition_timeout);
            int semres = adm_sem_wait(&g_adm_event_sem, g_adm_state_transition_timeout);
            if (semres) {
                sem_timed_out = 1;
                ALOG_ERR("Semaphore timed out. g_adm_reboot_on_error == 0\n");
            }
        }
    }

    // Needed until IL tool used in all code
    for (i=0 ; i<num_components ; i++) {
        components[i]->callback_state.cmd_in_progress = 0;
    }

    if (sem_timed_out) {
        return STE_ADM_RES_IL_UNKNOWN_ERROR;
    }

    // The components must not be busy now (since the
    // semaphore has been taken). Check that no error has
    // occured.
    for (i=0 ; i<num_components ; i++) {
        ADM_ASSERT(!il_tool_cmd_in_progress(components[i]));

        if (il_tool_last_error(components[i], NULL) != OMX_ErrorNone) {
            return STE_ADM_RES_IL_UNKNOWN_ERROR;
        }
    }

    return STE_ADM_RES_OK;
}

ste_adm_res_t il_tool_set_buffer_time(il_comp_t comp, OMX_U32 port, unsigned int buffer_time, unsigned buffer_count)
{
    OMX_PARAM_PORTDEFINITIONTYPE portdef;
    IL_TOOL_INIT_CONFIG_STRUCT(portdef);
    portdef.nPortIndex = port;
    RETURN_IF_FAIL(il_tool_GetParameter(comp, OMX_IndexParamPortDefinition, &portdef));

    if (buffer_time > 0) {
        OMX_AUDIO_PARAM_PCMMODETYPE pcmmode;
        IL_TOOL_INIT_CONFIG_STRUCT(pcmmode);
        pcmmode.nPortIndex = port;
        RETURN_IF_FAIL(il_tool_GetParameter(comp, OMX_IndexParamAudioPcm, &pcmmode));

        unsigned int bytes = 2 * pcmmode.nChannels * pcmmode.nSamplingRate * buffer_time / 1000;
        portdef.nBufferSize = bytes;
    }

    if (buffer_count > 0) {
        portdef.nBufferCountActual = buffer_count;
    }

    return il_tool_SetParameter(comp, OMX_IndexParamPortDefinition, &portdef);
}





// TODO: Join with il_tool_last_error?
OMX_ERRORTYPE il_tool_get_last_error(il_comp_t comp)
{
    ADM_ASSERT(comp != NULL);
    return il_tool_last_error(comp, NULL);
}

// Set last opened device
void il_tool_io_set_last_opened_dev(const char* last_dev)
{
    ADM_ASSERT(strlen(last_dev) < ADM_MAX_DEVICE_NAME_LENGTH);
    strcpy(g_last_opened_dev, last_dev);
}
// Get last opened device
const char* il_tool_io_get_last_opened_dev()
{
    return g_last_opened_dev;
}

ste_adm_res_t il_tool_set_config_and_wait_config_event(il_comp_t comp, OMX_INDEXTYPE nIndex, void* cfgData, OMX_INDEXTYPE nEventIndex)
{
    SRV_DBG_ASSERT_IS_WORKER_THREAD;

    adm_il_tool_state_t* il_data = &comp->callback_state;
    ADM_ASSERT(il_data->waiting_for_event == 0);

    il_data->waiting_for_event = 1;
    il_data->waiting_for_event_idx = (int) nIndex;

    ste_adm_res_t res = il_tool_SetConfig(comp, nIndex, cfgData);
    if (res != STE_ADM_RES_OK) {
        il_data->waiting_for_event = 0;
        return res;
    }

    ADM_SEM_WAIT_WITH_PRINT("il_tool_wait_config_event comp=%X index=%X\n", il_tool_raw_handle(comp), (int) nEventIndex);
    ADM_ASSERT(il_data->waiting_for_event == 0);

    return STE_ADM_RES_OK;
}




static int is_fatal_error(OMX_ERRORTYPE error)
{
    switch (error) {
        // Errors that are ignored
        case OMX_ErrorUnderflow:
        case OMX_ErrorOverflow:
        case OMX_ErrorPortUnpopulated:
            return 0;

        // General, fatal errors.
        case OMX_ErrorUndefined:
        case OMX_ErrorHardware:
        case OMX_ErrorResourcesLost:
        case OMX_ErrorTimeout:
        case OMX_ErrorResourcesPreempted:
        case OMX_ErrorPortUnresponsiveDuringAllocation:
        case OMX_ErrorPortUnresponsiveDuringDeallocation:
        case OMX_ErrorPortUnresponsiveDuringStop:
        case OMX_ErrorDynamicResourcesUnavailable:
        case OMX_ErrorInsufficientResources:
            return 1;

        // Errors likely due to ADM-internal error
        // We abort an operation when any of these are encountered...
        case OMX_ErrorInvalidState:
        case OMX_ErrorPortsNotCompatible:
        case OMX_ErrorNoMore:
        case OMX_ErrorSameState:
        case OMX_ErrorIncorrectStateTransition:
        case OMX_ErrorIncorrectStateOperation:
        case OMX_ErrorBadPortIndex:
            return 1;

        // Should not come from callback in the ADM
        // Likely indicates a bug in ADM or IL.
        // We abort an operation when any of these are encountered...
        case OMX_ErrorInvalidComponentName:
        case OMX_ErrorComponentNotFound:
        case OMX_ErrorInvalidComponent:
        case OMX_ErrorBadParameter:
        case OMX_ErrorUnsupportedSetting:
        case OMX_ErrorNotImplemented:
        case OMX_ErrorStreamCorrupt:
        case OMX_ErrorVersionMismatch:
        case OMX_ErrorNotReady:
        case OMX_ErrorMbErrorsInFrame:
        case OMX_ErrorFormatNotDetected:
        case OMX_ErrorSeperateTablesUsed:
        default:
            return 1;
    }
}

OMX_HANDLETYPE il_tool_raw_handle(const il_comp_t comp)
{
    return comp->callback_state.omx_handle;
}

static int il_tool_cmd_in_progress(il_comp_t comp)
{
    (void) comp;
    return 0; // TODO
}

static OMX_ERRORTYPE il_tool_last_error(il_comp_t comp, OMX_U32* Data2_p)
{
    OMX_ERRORTYPE err;

    pthread_mutex_lock(&comp->mutex);
    adm_il_tool_state_t* il_data = &comp->callback_state;
    err = il_data->last_error;
    if (Data2_p) *Data2_p = il_data->last_error_Data2;
    pthread_mutex_unlock(&comp->mutex);
    return err;
}

void il_tool_save_state()
{}

ste_adm_res_t il_tool_revert()
{
    return STE_ADM_RES_INTERNAL_ERROR;
}

void il_tool_set_state_transition_timeout(int timeout)
{
    g_adm_state_transition_timeout = timeout;
}



static ste_adm_res_t il_result_to_adm_result(OMX_ERRORTYPE omx_result)
{
    if (omx_result == OMX_ErrorNone) {
        return STE_ADM_RES_OK;
    }


    // TODO
    return STE_ADM_RES_INTERNAL_ERROR;
}




ste_adm_res_t il_tool_CreateComponent(const char* name,
    il_comp_t* comp_hp)
{
    SRV_DBG_ASSERT_IS_WORKER_THREAD;

    adm_il_appdata_t* il_data = malloc(sizeof(adm_il_appdata_t));
    if (!il_data) {
        return STE_ADM_RES_ERR_MALLOC;
    }

    memset(il_data, 0, sizeof(adm_il_appdata_t));

    ste_adm_res_t res = il_result_to_adm_result(il_tool_log_CreateComponent(name, il_data));
    if (res != STE_ADM_RES_OK) {
        free(il_data);
    } else {
        *comp_hp = il_data;
    }
    return res;
}

ste_adm_res_t il_tool_SetConfig(
    il_comp_t comp_h,
    OMX_INDEXTYPE nIndex,
    const void* pComponentConfigStructure)
{
    return il_result_to_adm_result(il_tool_log_SetConfig(comp_h, nIndex, pComponentConfigStructure));
}

ste_adm_res_t il_tool_SetParameter(
    il_comp_t comp_h,
    OMX_INDEXTYPE nIndex,
    const void* pComponentParameterStructure)
{
    return il_result_to_adm_result(il_tool_log_SetParameter(comp_h, nIndex, pComponentParameterStructure));
}

ste_adm_res_t il_tool_SendCommand(
    il_comp_t comp_h,
    OMX_COMMANDTYPE Cmd,
    OMX_U32 nParam1,
    OMX_PTR pCmdData)
{
    SRV_DBG_ASSERT_IS_WORKER_THREAD;

    // In the future, handling of save_state() and revert() will take place here.
    return il_result_to_adm_result(il_tool_log_SendCommand(comp_h, Cmd, nParam1, pCmdData));
}

ste_adm_res_t il_tool_GetParameter(
    il_comp_t hComponent,
    OMX_INDEXTYPE nParamIndex,
    OMX_PTR pComponentParameterStructure)
{
    return il_result_to_adm_result(il_tool_log_GetParameter(hComponent, nParamIndex, pComponentParameterStructure));
}

ste_adm_res_t il_tool_GetConfig(
    il_comp_t hComponent,
    OMX_INDEXTYPE nIndex,
    OMX_PTR pComponentConfigStructure)
{
    return il_result_to_adm_result(il_tool_log_GetConfig(hComponent, nIndex, pComponentConfigStructure));
}

ste_adm_res_t il_tool_GetComponentVersion(
    il_comp_t hComponent,
    OMX_STRING pComponentName,
    OMX_VERSIONTYPE* pComponentVersion,
    OMX_VERSIONTYPE* pSpecVersion,
    OMX_UUIDTYPE* pComponentUUID)
{
    return il_result_to_adm_result(il_tool_log_GetComponentVersion(hComponent, pComponentName, pComponentVersion, pSpecVersion, pComponentUUID));
}

ste_adm_res_t il_tool_GetState(
    il_comp_t hComponent,
    OMX_STATETYPE* pState)
{
    return il_result_to_adm_result(il_tool_log_GetState(hComponent, pState));
}

ste_adm_res_t il_tool_SetupTunnel(
    il_comp_t hOutput,
    OMX_U32 nPortOutput,
    il_comp_t hInput,
    OMX_U32 nPortInput)
{
    return il_result_to_adm_result(il_tool_log_SetupTunnel(hOutput, nPortOutput, hInput, nPortInput));
}

ste_adm_res_t il_tool_GetExtensionIndex(
    il_comp_t hComponent,
    const char* cParameterName,
    OMX_INDEXTYPE* pIndexType)
{
    OMX_ERRORTYPE omx_res = il_tool_log_GetExtensionIndex(hComponent, cParameterName, pIndexType);
    if (omx_res == OMX_ErrorUnsupportedIndex || omx_res == OMX_ErrorNotImplemented) {
        return STE_ADM_RES_UNKNOWN_IL_INDEX_NAME;
    } else {
        return il_result_to_adm_result(omx_res);
    }
}

ste_adm_res_t il_tool_commit_mixer_effects(il_comp_t comp_h, OMX_U32 port)
{
    SRV_DBG_ASSERT_IS_WORKER_THREAD;
    ADM_ASSERT(comp_h != NULL);

    OMX_AUDIO_CONFIG_EFFECTCOMMITTYPE commit;
    IL_TOOL_INIT_CONFIG_STRUCT(commit);

    commit.nPortIndex        = port;

    ste_adm_res_t res = il_tool_SetConfig(comp_h, OMX_IndexConfigAudioEffectCommit, &commit);

    return res;
}


ste_adm_res_t il_tool_setConfig_mixer_effect(
        il_comp_t comp_h,
        int position,
        OMX_U32 port,
        OMX_INDEXTYPE nIndex,
        const void* pComponentConfigStructure)
{
    OMX_AUDIO_CONFIG_EFFECTCONFIGURETYPE cfg;
    IL_TOOL_INIT_CONFIG_STRUCT(cfg);

    ADM_ASSERT(offsetof(OMX_AUDIO_CONFIG_EFFECTCONFIGURETYPE, sConfig) + ((IL_Base_t*)pComponentConfigStructure)->nSize <=
            sizeof(OMX_AUDIO_CONFIG_EFFECTCONFIGURETYPE));

    memcpy(&cfg.sConfig, pComponentConfigStructure,
            ((IL_Base_t*)pComponentConfigStructure)->nSize);


    cfg.base.nPortIndex = port;
    cfg.base.nPositionIndex = position;
    cfg.base.nEffectSpecificIndex = nIndex;

    ste_adm_res_t res = il_tool_SetConfig(comp_h, OMX_IndexConfigAudioEffectConfigure, &cfg);

    return res;
}


ste_adm_res_t il_tool_add_mixer_effect(
        il_comp_t comp_h,
        int position,
        OMX_U32 port,
        const char* name)
{
    SRV_DBG_ASSERT_IS_WORKER_THREAD;
    ADM_ASSERT(comp_h != NULL);

    OMX_AUDIO_CONFIG_EFFECTADDPOSITIONTYPE add_cfg;
    IL_TOOL_INIT_CONFIG_STRUCT(add_cfg);

    add_cfg.nPortIndex        = port;
    add_cfg.nPositionIndex    = position;
    add_cfg.cEffectName       = (OMX_STRING)adaptions_get_mixer_internal_effect_name(name);

    ste_adm_res_t res = il_tool_SetConfig(comp_h, OMX_IndexConfigAudioEffectAddPosition, &add_cfg);

    return res;
}

ste_adm_res_t il_tool_remove_mixer_effect(
        il_comp_t comp_h,
        int position,
        OMX_U32 port)
{
    SRV_DBG_ASSERT_IS_WORKER_THREAD;
    ADM_ASSERT(comp_h != NULL);

    OMX_AUDIO_CONFIG_EFFECTREMOVEPOSITIONTYPE remove_cfg;
    IL_TOOL_INIT_CONFIG_STRUCT(remove_cfg);

    remove_cfg.nPortIndex        = port;
    remove_cfg.nPositionIndex    = position;

    ste_adm_res_t res = il_tool_SetConfig(comp_h, OMX_IndexConfigAudioEffectRemovePosition, &remove_cfg);

    return res;
}

