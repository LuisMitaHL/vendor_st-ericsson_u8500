/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include "ste_adm_msg.h"
#include "ste_adm_dbg.h"
#include "ste_adm_dev.h"
#include "ste_adm_srv.h"
#include "ste_adm_omx_core.h"
#include "ste_adm_omx_tool.h"
#include <stdlib.h>
#include <string.h>


struct d2dsession
{
    srv_session_t srv_session;
    int src_dev_handle;
    int dst_dev_handle;
};

struct hw_d2dsession
{
    srv_session_t srv_session;
    int dev_2_dev_handle;
    int closed;
};

static void d2d_dtor(struct srv_session_s* param)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    SRV_DBG_ASSERT_IS_SERIALIZED_REQUEST;

    struct d2dsession* session_p = (struct d2dsession*) param;
    ALOG_STATUS("d2d_dtor - disconnecting devices\n");

    if (session_p == NULL)
    {
        return;
    }

    session_p->srv_session.dtor_fp = NULL;

    il_comp_t src_comp;
    il_comp_t dst_comp;
    OMX_U32 src_port;
    OMX_U32 dst_port;

    if (!dev_get_endpoint(session_p->src_dev_handle, &src_comp, &src_port)) {
        ALOG_ERR("D2D API: dev_get_endpoint (%d) failed, error %d\n", session_p->src_dev_handle);
        return;
    }

    if (!dev_get_endpoint(session_p->dst_dev_handle, &dst_comp, &dst_port)) {
        ALOG_ERR("D2D API: dev_get_endpoint (ds) failed\n", session_p->dst_dev_handle);
        return;
    }

    srv_become_worker_thread();

    /* Disconnect the two devices */
    il_tool_save_state();

    if(il_tool_disconnect(src_comp, src_port, dst_comp, dst_port) != STE_ADM_RES_OK)
    {
      ALOG_ERR("D2D API: il_tool_disconnect failed\n");
      il_tool_revert();
    }

    srv_become_normal_thread();

    if (session_p->dst_dev_handle != 0)
    {
        if (dev_close(session_p->dst_dev_handle, 0) != STE_ADM_RES_OK)
        {
            ALOG_ERR("Failed to close device (%d)\n", session_p->dst_dev_handle);
        }
    }

    if (session_p->src_dev_handle != 0)
    {
        if (dev_close(session_p->src_dev_handle, 0) != STE_ADM_RES_OK)
        {
            ALOG_ERR("Failed to close device (%d)\n", session_p->src_dev_handle);
        }
    }

    free(session_p);
}

static void d2d_hwdtor(struct srv_session_s* param)
{
    int ret;
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    SRV_DBG_ASSERT_IS_SERIALIZED_REQUEST;

    struct hw_d2dsession* session_p = (struct hw_d2dsession*) param;
    ALOG_STATUS("hw d2d_hwdtor - disconnecting devices\n");

    if (session_p == NULL)
    {
        return;
    }

    session_p->srv_session.dtor_fp = NULL;
    if(session_p->closed < 1){
        ALOG_INFO("%s: destructor called calling dev_close_d2d\n", __func__);
        ret = dev_close_d2d(&session_p->dev_2_dev_handle);
        if (ret != STE_ADM_RES_OK)
        {
            ALOG_ERR("D2D API: dev_close_d2d (%d) failed, error %d\n", ret, session_p->dev_2_dev_handle);
        }
    }

    free(session_p);
}


void d2d_connect_logical(msg_d2d_connect_t* msg_p,
                 srv_session_t** client_pp)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    SRV_DBG_ASSERT_IS_SERIALIZED_REQUEST;

    struct d2dsession* session_p;

    if (*client_pp) {
        ALOG_ERR("d2d: Connection already active\n");
        msg_p->base.result = STE_ADM_RES_INCORRECT_STATE;
        srv_send_reply((msg_base_t*) msg_p);
        return;
    };


    // New session
    session_p = malloc(sizeof(struct d2dsession));
    if (!session_p) {
        ALOG_ERR("d2d: malloc failed\n");
        msg_p->base.result = STE_ADM_RES_ERR_MALLOC;
        srv_send_reply(&msg_p->base);
        return;
    }

    memset(session_p, 0, sizeof(struct d2dsession));
    session_p->srv_session.api_group = API_GROUP_D2D;

    // Register destructor
    session_p->srv_session.dtor_fp   = d2d_dtor;
    session_p->srv_session.dtor_must_be_serialized = 1;
    *client_pp = (srv_session_t*) session_p;



    struct dev_params p;
    p.name_in  = NULL; // Only used for voicecall
    p.name_out = NULL; // Only used for voicecall
    p.samplerate = 48000;
    p.format = STE_ADM_FORMAT_STEREO;
    p.alloc_buffers = 0;
    p.shm_fd = -1;

    msg_p->base.result = dev_open(msg_p->dev_name_src, 0, &p, &session_p->src_dev_handle);
    if (msg_p->base.result == STE_ADM_RES_OK) {
        ALOG_INFO("D2D API: Opened src device, handle = %X\n", session_p->src_dev_handle);
        ADM_ASSERT(session_p->src_dev_handle >= 1);
    } else {
        ALOG_ERR("D2D API: Open device (%s) failed, error %d\n", msg_p->dev_name_dst, msg_p->base.result);
        goto cleanup;
    }

    msg_p->base.result = dev_open(msg_p->dev_name_dst, 0, &p, &session_p->dst_dev_handle);
    if (msg_p->base.result == STE_ADM_RES_OK) {
        ALOG_INFO("D2D API: Opened dst device, handle = %X\n", session_p->src_dev_handle);
        ADM_ASSERT(session_p->src_dev_handle >= 1);
    } else {
        ALOG_ERR("D2D API: Open device (%s) failed, error %d\n", msg_p->dev_name_dst, msg_p->base.result);
        goto cleanup;
    }

    il_comp_t src_comp;
    il_comp_t dst_comp;
    OMX_U32 src_port;
    OMX_U32 dst_port;

    if (!dev_get_endpoint(session_p->src_dev_handle, &src_comp, &src_port)) {
        ALOG_ERR("D2D API: dev_get_endpoint (%s) failed\n", msg_p->dev_name_src);
        msg_p->base.result = STE_ADM_RES_INTERNAL_ERROR;
        goto cleanup;
    }

    if (!dev_get_endpoint(session_p->dst_dev_handle, &dst_comp, &dst_port)) {
        ALOG_ERR("D2D API: dev_get_endpoint (%s) failed\n", msg_p->dev_name_dst);
        msg_p->base.result = STE_ADM_RES_INTERNAL_ERROR;
        goto cleanup;
    }

    srv_become_worker_thread();

    /* Connect the two devices */
    il_tool_save_state();

    // don't generate silence on underrun
    OMX_AUDIO_PARAM_SYNCHRONIZEDTYPE syncparam; \
    IL_TOOL_INIT_CONFIG_STRUCT(syncparam); \
    syncparam.nPortIndex = dst_port;
    syncparam.bIsSynchronized = 1;
    msg_p->base.result = il_tool_SetParameter(dst_comp, OMX_IndexParamAudioSynchronized, &syncparam);
    if (msg_p->base.result != STE_ADM_RES_OK) {
        il_tool_revert();
        srv_become_normal_thread();
        goto cleanup;
    }


    // Collect data for a jitter margin at startup
    ADM_CONFIG_STARTUPJITTERMARGINTYPE jittercfg;
    IL_TOOL_INIT_CONFIG_STRUCT(jittercfg);
    jittercfg.nPortIndex = dst_port;
    jittercfg.startupJitterMargin = 40;
    msg_p->base.result = il_tool_SetConfig(dst_comp, ADM_IndexConfigStartupJitterMargin, &jittercfg);
    if (msg_p->base.result != STE_ADM_RES_OK) {
        il_tool_revert();
        srv_become_normal_thread();
        goto cleanup;
    }

    if(il_tool_connect_and_enable(src_comp, src_port, dst_comp, dst_port,
                    1, 1, 1, OMX_BufferSupplyUnspecified) != STE_ADM_RES_OK)
    {
      ALOG_ERR("D2D API: il_tool_connect_and_enable failed\n");
      il_tool_revert();
      msg_p->base.result = STE_ADM_RES_IL_ERROR;
      srv_become_normal_thread();
      goto cleanup;
    }

    srv_become_normal_thread();

    goto done;
cleanup:
    if (session_p->dst_dev_handle != 0)
    {
        if (dev_close(session_p->dst_dev_handle, 0) != STE_ADM_RES_OK)
        {
            ALOG_ERR("Failed to close device (%d)\n", session_p->dst_dev_handle);
        }
    }

    if (session_p->src_dev_handle != 0)
    {
        if (dev_close(session_p->src_dev_handle, 0) != STE_ADM_RES_OK)
        {
            ALOG_ERR("Failed to close device (%d)\n", session_p->src_dev_handle);
        }
    }

    *client_pp = NULL;
    free(session_p);

done:
    srv_send_reply((msg_base_t*) msg_p);
    return;

}

void d2d_connect_hw(msg_d2d_connect_t* msg_p,
                 srv_session_t** client_pp)
{

    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    SRV_DBG_ASSERT_IS_SERIALIZED_REQUEST;

    struct hw_d2dsession* session_p;

    if (*client_pp) {
        ALOG_ERR("hw d2d: Connection already active\n");
        msg_p->base.result = STE_ADM_RES_INCORRECT_STATE;
        srv_send_reply((msg_base_t*) msg_p);
        return;
    };

    // New session
    session_p = malloc(sizeof(struct hw_d2dsession));
    if (!session_p) {
        ALOG_ERR("hw d2d: malloc failed\n");
        msg_p->base.result = STE_ADM_RES_ERR_MALLOC;
        srv_send_reply(&msg_p->base);
        return;
    }

    memset(session_p, 0, sizeof(struct hw_d2dsession));
    session_p->srv_session.api_group = API_GROUP_D2D;

    // Register destructor
    session_p->srv_session.dtor_fp   = d2d_hwdtor;
    session_p->srv_session.dtor_must_be_serialized = 1;
    session_p->closed = 0;
    *client_pp = (srv_session_t*) session_p;

    msg_p->base.result = dev_open_d2d(msg_p->dev_name_src, msg_p->dev_name_dst, &session_p->dev_2_dev_handle);
    if (msg_p->base.result == STE_ADM_RES_OK) {
        ALOG_ERR("D2D API: Opened src device & dst device, handle = %X\n", session_p->dev_2_dev_handle);
    } else {
        ALOG_ERR("D2D API: Open device (%s_%s) failed, error %d\n", msg_p->dev_name_src, msg_p->dev_name_dst, msg_p->base.result);
    }

    srv_send_reply((msg_base_t*) msg_p);
    return;
}


void d2d_connect(msg_d2d_connect_t* msg_p,
                 srv_session_t** client_pp)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    SRV_DBG_ASSERT_IS_SERIALIZED_REQUEST;

    if ((msg_p->flags & (STE_ADM_D2D_FLAG_LOGICAL) )== STE_ADM_D2D_FLAG_LOGICAL) {
        d2d_connect_logical(msg_p,client_pp);
    }
    else if ((msg_p->flags & (STE_ADM_D2D_FLAG_HW) )== STE_ADM_D2D_FLAG_HW) {
        d2d_connect_hw(msg_p,client_pp);
    }
    else {
        ALOG_ERR("d2d: unsupported device to device connection requested");
        msg_p->base.result = STE_ADM_RES_INVALID_PARAMETER;
        srv_send_reply((msg_base_t*) msg_p);
        return;
    }
}

void d2d_disconnect_logical(msg_d2d_disconnect_t* msg_p,
                 srv_session_t** client_pp)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    SRV_DBG_ASSERT_IS_SERIALIZED_REQUEST;
    (void) client_pp;

    ALOG_ERR("%s: SYNC disconnect not implemented for d2d logical\n");
    msg_p->base.result = STE_ADM_RES_INVALID_PARAMETER;
    srv_send_reply((msg_base_t*) msg_p);
    return;
}

void d2d_disconnect_hw(msg_d2d_disconnect_t* msg_p,
                 srv_session_t** client_pp)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    SRV_DBG_ASSERT_IS_SERIALIZED_REQUEST;

    ALOG_INFO("%s: called calling destructor\n",__func__);
    msg_p->base.result = dev_close_d2d(&((struct hw_d2dsession*)(*client_pp))->dev_2_dev_handle);

    if (msg_p->base.result != STE_ADM_RES_OK) {
        ALOG_ERR("%s: FAILED error :%d\n", msg_p->base.result);
    }

    ((struct hw_d2dsession*)(*client_pp))->closed = 1;

    srv_send_reply((msg_base_t*) msg_p);
    return;
}


void d2d_disconnect(msg_d2d_disconnect_t* msg_p,
                 srv_session_t** client_pp)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    SRV_DBG_ASSERT_IS_SERIALIZED_REQUEST;

    if ((msg_p->flags & (STE_ADM_D2D_FLAG_LOGICAL) )== STE_ADM_D2D_FLAG_LOGICAL) {
        d2d_disconnect_logical(msg_p,client_pp);
    }
    else if ((msg_p->flags & (STE_ADM_D2D_FLAG_HW) )== STE_ADM_D2D_FLAG_HW) {
        d2d_disconnect_hw(msg_p,client_pp);
    }
    else {
        ALOG_ERR("d2d: unsupported device to device connection requested");
        msg_p->base.result = STE_ADM_RES_INVALID_PARAMETER;
        srv_send_reply((msg_base_t*) msg_p);
        return;
    }
}
