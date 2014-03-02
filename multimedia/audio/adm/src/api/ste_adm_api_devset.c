/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#define ADM_LOG_FILENAME "devset"

#include "ste_adm_msg.h"
#include "ste_adm_dbg.h"
#include "ste_adm_dev.h"
#include "ste_adm_srv.h"
#include "ste_adm_client.h"
#include "ste_adm_config.h"
#include "ste_adm_omx_tool.h"
#include "ste_adm_omx_io.h"
#include "ste_adm_api_devset.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "OMX_CoreExt.h"
#include "ste_adm_api_cscall.h"
#include "ste_adm_platform_adaptions.h" // IL component names
#include "ste_adm_db.h"
#include "OMX_Symbian_AudioExt_Ste.h"
#include "audio_chipset_api_index.h"
#include "AFM_Index.h"
#include "AFM_Types.h"

#if defined(ADM_DBG_X86)
#include "ste_adm_hw_handler.h"
#else
#include "alsactrl_hwh.h"
#endif

static void devapi_dtor(void* client_p);
static void devapi_close(msg_device_t* msg_p, void** client_pp);
static int devapi_open(msg_device_t* msg_p, void** client_pp);
static void devapi_feed(msg_data_t* msg_p, void** client_pp);
static void devapi_datareq(msg_data_t* msg_p, void** client_pp);
static void devapi_drain(msg_base_t* msg_p, void** client_pp);

struct topapi_vtbl
{
    int (*open)(msg_device_t* msg_p, void** session_p);
    void (*close)(msg_device_t* msg_p, void** session_p);
    void (*drain)(msg_base_t* msg_p, void** session_p);
    void (*feed)(msg_data_t* msg_p, void** session_p);
    void (*datareq)(msg_data_t* msg_p, void** session_p);
    void (*dtor)(void* session_p);
};

static const struct topapi_vtbl topapi_vtbls[] =
{
    {
        csapi_dict_in_open,     csapi_dict_in_close,      NULL,
        NULL,                   csapi_dict_in_datareq,    csapi_dict_in_dtor
    },
    {
        csapi_dict_out_open,    csapi_dict_out_close,     csapi_dict_out_drain,
        csapi_dict_out_feed,    NULL,                     csapi_dict_out_dtor
    },

    {
        csapi_voip_in_open,     csapi_voip_in_close,      NULL,
        NULL,                   csapi_voip_in_datareq,    csapi_voip_in_dtor
    },
    {
        csapi_voip_out_open,    csapi_voip_out_close,     csapi_voip_out_drain,
        csapi_voip_out_feed,    NULL,                     csapi_voip_out_dtor
    },

    {
        devapi_open,            devapi_close,             devapi_drain,
        devapi_feed,            devapi_datareq,           devapi_dtor
    },
};

typedef struct
{
    srv_session_t srv_session;
    const struct topapi_vtbl* vtbl;
    void* session_p;
} topapi_session_t;


static void topapi_dtor(srv_session_t* dtor_param)
{
    topapi_session_t* topsession_p = (topapi_session_t*) dtor_param;
    if (topsession_p && topsession_p->srv_session.api_group == API_GROUP_DEV && topsession_p->session_p) {
        topsession_p->vtbl->dtor(topsession_p->session_p);
    }
    free(topsession_p);
}

void topapi_open(msg_device_t* msg_p,
                 srv_session_t** client_pp)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

    topapi_session_t* topsession_p;
    if (*client_pp == NULL) {
        *client_pp = calloc(1, sizeof(topapi_session_t));
        if (*client_pp == NULL) {
            ALOG_ERR("devapi_open: malloc failed\n");
            msg_p->base.result = STE_ADM_RES_ERR_MALLOC;
            srv_send_reply(&msg_p->base);
            return;
        }
        topsession_p = (topapi_session_t*) *client_pp;
        topsession_p->srv_session.api_group = API_GROUP_DEV;
        topsession_p->srv_session.dtor_fp   = topapi_dtor;
        topsession_p->srv_session.dtor_must_be_serialized = 1;
    } else {
        topsession_p = (topapi_session_t*) *client_pp;
    }


    if (topsession_p->session_p) {
        if (topsession_p->vtbl->open(msg_p, &topsession_p->session_p)) {
            return;
        }
    } else {
        const struct topapi_vtbl* cur_vtbl  = topapi_vtbls;
        const struct topapi_vtbl* last_vtbl = topapi_vtbls + sizeof(topapi_vtbls)/sizeof(topapi_vtbls[0]);
        for ( ; cur_vtbl != last_vtbl ; cur_vtbl++) {
            if (cur_vtbl->open(msg_p, &topsession_p->session_p)) {
                topsession_p->vtbl = cur_vtbl;
                return;
            }
        }
    }

    ALOG_ERR("topapi_open: STE_ADM_RES_NO_SUCH_DEVICE '%s'\n", msg_p->name);
    msg_p->base.result = STE_ADM_RES_NO_SUCH_DEVICE;
    srv_send_reply(&msg_p->base);
}

void topapi_close(msg_device_t* msg_p,
                 srv_session_t** client_pp)
{
    topapi_session_t* topsession_p = (topapi_session_t*) *client_pp;
    if (topsession_p && topsession_p->srv_session.api_group == API_GROUP_DEV && topsession_p->session_p) {
        topsession_p->vtbl->close(msg_p, &topsession_p->session_p);
    } else {
        ALOG_ERR("topapi_close: STE_ADM_RES_INCORRECT_STATE\n");
        msg_p->base.result = STE_ADM_RES_INCORRECT_STATE;
        srv_send_reply(&msg_p->base);
    }
}

void topapi_feed(msg_data_t* msg_p,
                 srv_session_t** client_pp)
{
    topapi_session_t* topsession_p = (topapi_session_t*) *client_pp;
    if (topsession_p && topsession_p->srv_session.api_group == API_GROUP_DEV &&
        topsession_p->vtbl->feed && topsession_p->session_p)
    {
        topsession_p->vtbl->feed(msg_p, &topsession_p->session_p);
    } else {
        ALOG_ERR("topapi_feed: STE_ADM_RES_INCORRECT_STATE\n");
        msg_p->base.result = STE_ADM_RES_INCORRECT_STATE;
        srv_send_reply(&msg_p->base);
    }
}

void topapi_drain(msg_base_t* msg_p,
                  srv_session_t** client_pp)
{
    topapi_session_t* topsession_p = (topapi_session_t*) *client_pp;
    if (topsession_p && topsession_p->srv_session.api_group == API_GROUP_DEV &&
        topsession_p->vtbl->drain && topsession_p->session_p)
    {
        topsession_p->vtbl->drain(msg_p, &topsession_p->session_p);
    } else {
        ALOG_ERR("topapi_drain: STE_ADM_RES_INCORRECT_STATE\n");
        msg_p->result = STE_ADM_RES_INCORRECT_STATE;
        srv_send_reply(msg_p);
    }
}

void topapi_datareq(msg_data_t* msg_p,
                 srv_session_t** client_pp)
{
    topapi_session_t* topsession_p = (topapi_session_t*) *client_pp;
    if (topsession_p && topsession_p->srv_session.api_group == API_GROUP_DEV &&
        topsession_p->vtbl->datareq && topsession_p->session_p)
    {
        topsession_p->vtbl->datareq(msg_p, &topsession_p->session_p);
    } else {
        ALOG_ERR("topapi_datareq: STE_ADM_RES_INCORRECT_STATE\n");
        msg_p->base.result = STE_ADM_RES_INCORRECT_STATE;
        srv_send_reply(&msg_p->base);
    }
}


#define MAX_NO_DEV_HANDLES 2

struct devsession
{
    int dev_handle[MAX_NO_DEV_HANDLES];
    int lpa_subscription_handle[MAX_NO_DEV_HANDLES];

    il_comp_t split_handle; // used for output devices to enable
                            // playback on multiple devices.
    il_comp_t lpa_sink;
    il_comp_t lpa_spl;
    msg_base_t* drain_msg_p;
};

static void lpa_test_setup(struct devsession* dev);
static void lpa_test_destroy(struct devsession* dev);
static int is_lpa_test_mode();


typedef struct
{
    char* dev[MAX_NO_DEV_HANDLES];
    int samplerate;
    ste_adm_format_t format;
    int bufsz;
    int num_bufs;
    int shm_fd;
} reload_dev_info;

static void devapi_dtor(void* client_p)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    int res = STE_ADM_RES_OK;

    struct devsession* session_p = (struct devsession*) client_p;
    ALOG_STATUS("devapi_dtor - close all open devices in current session\n");

    if (is_lpa_test_mode()) {
        ALOG_STATUS("devapi_dtor - is_lpa_test_mode() active\n");
        if (session_p->lpa_sink) lpa_test_destroy(session_p);
        free(session_p);
        return;
    }

    OMX_U32 i;
    for (i = 0; i < MAX_NO_DEV_HANDLES; i++) {
        if (session_p->dev_handle[i] != 0) {
            if (session_p->split_handle) {
                il_comp_t comp;
                OMX_U32 port;
                if (!dev_get_endpoint(session_p->dev_handle[i], &comp, &port)) {
                    ADM_ASSERT(0);
                }
                srv_become_worker_thread();
                IL_TOOL_DISCONNECT(session_p->split_handle, i+1, comp, port);
                srv_become_normal_thread();
                dev_unsubscribe_lpa_mode(session_p->lpa_subscription_handle[i]);
                session_p->lpa_subscription_handle[i] = 0;
                dev_close(session_p->dev_handle[i], 0);
            } else {
                dev_close(session_p->dev_handle[i], 1);
            }
        }
        session_p->dev_handle[i] = 0;
    }

    if (session_p->split_handle) {
        il_comp_t tmp_split = session_p->split_handle;
        session_p->split_handle = NULL;
        srv_become_worker_thread();
        IL_TOOL_DISABLE_PORT_WITH_IO(tmp_split, 0);
        IL_TOOL_ENTER_STATE(OMX_StateIdle,   tmp_split);
        IL_TOOL_ENTER_STATE(OMX_StateLoaded, tmp_split);
        IL_TOOL_DESTROY(&tmp_split);
        srv_become_normal_thread();
    }
cleanup:
    if (res != STE_ADM_RES_OK) {
        srv_become_normal_thread();
    }

    free(session_p);
}

static void devapi_close(msg_device_t* msg_p, void** client_pp)
{
    int res = STE_ADM_RES_OK;

    ALOG_STATUS("Close device '%s'\n", msg_p->name);

    struct devsession* session_p = (struct devsession*) *client_pp;

    if (is_lpa_test_mode()) {
        ALOG_STATUS("devapi_close - is_lpa_test_mode() active\n");
        if (session_p->lpa_sink) lpa_test_destroy(session_p);
        session_p->lpa_sink = 0;
        srv_send_reply((msg_base_t*) msg_p);
        return;
    }

    int i;
    for (i = 0; i < MAX_NO_DEV_HANDLES; i++) {
        if (session_p->dev_handle[i] != 0 &&
            strcmp(msg_p->name, dev_get_name(session_p->dev_handle[i])) == 0)
        {
            if (session_p->split_handle) {
                il_comp_t comp;
                OMX_U32 port;
                if (!dev_get_endpoint(session_p->dev_handle[i], &comp, &port)) {
                    ADM_ASSERT(0);
                }
                srv_become_worker_thread();
                IL_TOOL_DISCONNECT(session_p->split_handle, i+1, comp, port);
                srv_become_normal_thread();
                dev_unsubscribe_lpa_mode(session_p->lpa_subscription_handle[i]);
                session_p->lpa_subscription_handle[i] = 0;
                dev_close(session_p->dev_handle[i], 0);
            } else {
                dev_close(session_p->dev_handle[i], 1);
            }
            session_p->dev_handle[i] = 0;
            break;
        }
    }

    if (i == MAX_NO_DEV_HANDLES) {
        // Device not found in list of dev handles.
        ALOG_ERR("devapi_close: Specified device is not open\n");
        msg_p->base.result = STE_ADM_RES_INVALID_PARAMETER;
        srv_send_reply((msg_base_t*) msg_p);
        return;
    }

    int last_device_closed = 1;
    for (i = 0; i < MAX_NO_DEV_HANDLES; i++) {
        if (session_p->dev_handle[i] != 0) {
            last_device_closed = 0;
            break;
        }
    }

    if (session_p->split_handle && last_device_closed) {
        ALOG_INFO("Releasing splitter; no longer used\n");
        il_comp_t tmp_split = session_p->split_handle;
        session_p->split_handle = NULL;
        srv_become_worker_thread();
        IL_TOOL_DISABLE_PORT_WITH_IO(tmp_split, 0);
        IL_TOOL_ENTER_STATE(OMX_StateIdle,   tmp_split);
        IL_TOOL_ENTER_STATE(OMX_StateLoaded, tmp_split);
        IL_TOOL_DESTROY(&tmp_split);
        srv_become_normal_thread();
    }

    if (last_device_closed) {
        free(*client_pp);
        *client_pp = NULL;
    }

    ALOG_INFO("CloseDevice(%s) OK\n", msg_p->name);

cleanup:
    if (res != STE_ADM_RES_OK) {
        srv_become_normal_thread();
    }
    srv_send_reply((msg_base_t*) msg_p);
}


static void devapi_lpa_mode_cb (void* param, int lpa_mode)
{
    struct devsession* session_p = (struct devsession*) param;

    OMX_U32 i;
    for (i = 0; i < MAX_NO_DEV_HANDLES; i++) {
        if (session_p->dev_handle[i] != 0) {
            if (session_p->split_handle) {
                OMX_AUDIO_PARAM_SYNCHRONIZEDTYPE synchronized_settings;
                IL_TOOL_INIT_CONFIG_STRUCT(synchronized_settings);
                synchronized_settings.nPortIndex = 0;
                synchronized_settings.bIsSynchronized = lpa_mode;
                il_tool_SetConfig(session_p->split_handle, OMX_IndexParamAudioSynchronized, &synchronized_settings);
            }
        }
    }
}

static int devapi_open(msg_device_t* msg_p, void** client_pp)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    int res = STE_ADM_RES_OK;
    int session_was_allocated = 0;

    ALOG_STATUS("Open device '%s', %d Hz, format=%X, %d x %d bytes bufs\n", msg_p->name, msg_p->samplerate, msg_p->format, msg_p->num_bufs, msg_p->bufsz);

    struct devsession* session_p;
    if (*client_pp == NULL) {
        // New session
        session_p = calloc(1, sizeof(struct devsession));
        if (!session_p) {
            ALOG_ERR("devapi_open: calloc failed\n");
            msg_p->base.result = STE_ADM_RES_ERR_MALLOC;
            srv_send_reply(&msg_p->base);
            return 1;
        }

        session_was_allocated = 1;
        *client_pp =  session_p;
    }

    session_p = (struct devsession*) *client_pp;
    struct dev_params p;
    memset(&p, 0, sizeof(p));
    p.samplerate = msg_p->samplerate;
    p.format = msg_p->format;
    p.shm_fd = -1;

    if (is_lpa_test_mode()) {
        ALOG_STATUS("devapi_open - is_lpa_test_mode() active\n");
        lpa_test_setup(session_p);
        int fd_to_xfer;
        il_tool_get_buffers(session_p->lpa_spl, 0, &fd_to_xfer, NULL, NULL);
        srv_send_reply_and_fd(&msg_p->base, fd_to_xfer);
        return 1;
    }

    int is_input = 0;
    if (adm_db_io_info(msg_p->name, &is_input) != STE_ADM_RES_OK) {
        ALOG_ERR("Failed to get direction of %s from db!\n", msg_p->name);
        if (session_was_allocated) {
            free(*client_pp);
            *client_pp = NULL;
        }
        return 0;
    }

    int no_more_devices_allowed = 0;
    int is_worker_thread = 0;
    OMX_U32 i = 0;
    if (is_input || msg_p->format >= STE_ADM_FORMAT_FIRST_CODED) {
        if (session_p->dev_handle[i] == 0) {
            // For non-splitter devices buffers should be allocated on the device
            // endpoint, for use with FillThisBuffer/EmptyThisBuffer.
            p.alloc_buffers = 1;
            p.bufsz         = msg_p->bufsz;
            p.num_bufs      = msg_p->num_bufs;
            res = dev_open(msg_p->name, 0, &p, &session_p->dev_handle[i]);
            if (res != STE_ADM_RES_OK) {
                 goto cleanup;
            }
        } else {
            no_more_devices_allowed = 1;
        }
    } else {
        for (i = 0; i < MAX_NO_DEV_HANDLES; i++) {
            if (session_p->dev_handle[i] == 0) {
                // For output devices buffers should be allocated on the splitter
                // that enables playing one audio stream on multiple devices.
                p.alloc_buffers = 0;
                res = dev_open(msg_p->name, 0, &p, &session_p->dev_handle[i]);
                if (res != STE_ADM_RES_OK) {
                    goto cleanup;
                }
                // Connect the output device to splitter output.
                il_comp_t comp;
                OMX_U32 port;
                if (!dev_get_endpoint(session_p->dev_handle[i], &comp, &port)) {
                    ADM_ASSERT(0);
                }
                srv_become_worker_thread();
                is_worker_thread = 1;
                if (!session_p->split_handle) {
                    // Initialize the splitter used to enable playback on multiple devices
                    IL_TOOL_CREATE(&session_p->split_handle, adaptions_comp_name(ADM_ARM_SPLITTER_NAME), "devset_split");

                    OMX_AUDIO_PARAM_PCMMODETYPE pcm_mode;
                    IL_TOOL_INIT_CONFIG_STRUCT(pcm_mode);
                    pcm_mode.nPortIndex = port;
                    IL_TOOL_GET_PARAM(comp, OMX_IndexParamAudioPcm, &pcm_mode);

                    OMX_U32 j;
                    for (j=0; j<=MAX_NO_DEV_HANDLES; j++) {
                        pcm_mode.nPortIndex = j;
                        IL_TOOL_SET_PARAM(session_p->split_handle, OMX_IndexParamAudioPcm, &pcm_mode);
                    }
                    IL_TOOL_DISABLE_ALL_PORTS(session_p->split_handle);

                    //
                    // Set all splitter outputs as synchronized, so nothing is sent on only
                    // one of the outputs
                    //
                    OMX_SYMBIAN_AUDIO_PARAM_SYNCHRONIZEDTYPE synch_cfg;
                    IL_TOOL_INIT_CONFIG_STRUCT(synch_cfg);
                    synch_cfg.bIsSynchronized = OMX_TRUE;

                    for (j=1; j<=MAX_NO_DEV_HANDLES; j++) {
                        synch_cfg.nPortIndex = j;
                        IL_TOOL_SET_PARAM(session_p->split_handle,
                                          OMX_IndexParamAudioSynchronized, &synch_cfg);
                    }

                    IL_TOOL_ENTER_STATE(OMX_StateIdle, session_p->split_handle);
                    IL_TOOL_ENTER_STATE(OMX_StateExecuting, session_p->split_handle);

                    IL_TOOL_SET_PORT_BUFFER_CONFIG(session_p->split_handle, 0, msg_p->num_bufs, msg_p->bufsz);
                    IL_TOOL_SET_PORT_SYNCHRONIZED(session_p->split_handle, 0, 0); // generate silence on underrun
                    IL_TOOL_ENABLE_PORT_WITH_IO(session_p->split_handle, 0);
                }

                IL_TOOL_CONNECT_AND_ENABLE_WITH_SRC_CFG(session_p->split_handle, i+1, comp, port);
                srv_become_normal_thread();
                is_worker_thread = 0;

                if (dev_subscribe_lpa_mode(devapi_lpa_mode_cb, session_p->dev_handle[i],
                                           session_p,
                                           &session_p->lpa_subscription_handle[i]) != STE_ADM_RES_OK) {
                    ALOG_ERR("DEV API: dev_subscribe_lpa_mode failed!");
                }

                break;
            }
        }
        if (i == MAX_NO_DEV_HANDLES) {
            // Did not find a free dev handle
            no_more_devices_allowed = 1;
        }
    }

    if (no_more_devices_allowed) {
        // Max number of devices are already opened.
        ALOG_ERR("No more devices may be opened in current session\n");
        res = STE_ADM_RES_DEV_PER_SESSION_LIMIT_REACHED;
    }

    int fd_to_xfer;
    if (res == STE_ADM_RES_OK) {
        if (is_input || msg_p->format >= STE_ADM_FORMAT_FIRST_CODED) {
            il_comp_t comp;
            OMX_U32 port;
            if (!dev_get_endpoint(session_p->dev_handle[0], &comp, &port)) {
                ADM_ASSERT(0);
            }
            il_tool_get_buffers(comp, port, &fd_to_xfer, NULL, NULL);
        } else {
            il_tool_get_buffers(session_p->split_handle, 0, &fd_to_xfer, NULL, NULL);
        }
    }

cleanup:
    msg_p->base.result = res;
    if (is_worker_thread) {
        srv_become_normal_thread();
    }

    if (msg_p->base.result == STE_ADM_RES_OK) {
        ALOG_STATUS("API LEAVE: Opened new device '%s', handle = %X\n", msg_p->name, session_p->dev_handle[i]);
        // If it goes through open graph, it should be OK.
        msg_p->actual_samplerate = msg_p->samplerate;
    } else {
        ALOG_ERR("DEV API: Open device failed, error %d\n", msg_p->base.result);
        // Keep session; we allow several open output devices per session,
        // so there might already exist open output devices. For input devices,
        // it makes sense to keep the session so we can retry OpenDevice.
    }
    if (res != STE_ADM_RES_OK && session_was_allocated) {
        free(*client_pp);
        *client_pp = NULL;
    }
    if (res == STE_ADM_RES_NO_SUCH_DEVICE) {
        return 0;
    } else if (res == STE_ADM_RES_OK) {
        srv_send_reply_and_fd(&msg_p->base, fd_to_xfer);
    } else {
        srv_send_reply(&msg_p->base);
    }

    return 1;
}


static void devapi_feed(msg_data_t* msg_p, void** client_pp)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

    struct devsession* session_p = (struct devsession*) *client_pp;

    if (is_lpa_test_mode()) {
        il_tool_io_feed(session_p->lpa_spl, 0, msg_p);
        return;
    }


    // OK if: we have a splitter, or the first device is open even
    // if we don't have a splitter (coded data scenario)
    if (!(session_p->dev_handle[0] >= 1 || session_p->split_handle))
    {
        ALOG_ERR("devapi_feed - STE_ADM_RES_INCORRECT_STATE\n");
        msg_p->base.result = STE_ADM_RES_INCORRECT_STATE;
        srv_send_reply(&msg_p->base);
        return;
    }

    msg_p->lpa_mode = 0;
    int i;
    for (i = 0; i < MAX_NO_DEV_HANDLES; i++) {
        if (session_p->dev_handle[i] != 0) {
            msg_p->lpa_mode = dev_get_lpa_mode(session_p->dev_handle[i]);
            if (msg_p->lpa_mode) {
                break;
            }
        }
    }

    if (session_p->split_handle) {
        il_tool_io_feed(session_p->split_handle, SPLITTER_INPUT_PORT, msg_p);
    } else {
        il_comp_t comp;
        OMX_U32 port;
        if (!dev_get_endpoint(session_p->dev_handle[0], &comp, &port)) {
            // This should only fail if the device is closed, and that should
            // be caught by the above check (session_p->dev_handle[0] >= 1)
            ADM_ASSERT(0);
        }
        il_tool_io_feed(comp, port, msg_p);
    }
    // response signal will be sent by io_feed, now or
    // later (after EmptyBufferDone)
}

static void devapi_datareq(msg_data_t* msg_p, void** client_pp)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

    struct devsession* session_p = (struct devsession*) *client_pp;
    if (session_p->dev_handle[0] <= 0) {
        ALOG_ERR("devapi_datareq - STE_ADM_RES_INCORRECT_STATE\n");
        msg_p->base.result = STE_ADM_RES_INCORRECT_STATE;
        srv_send_reply(&msg_p->base);
        return;
    }

    il_comp_t comp;
    OMX_U32 port;
    if (!dev_get_endpoint(session_p->dev_handle[0], &comp, &port)) {
        // If this happens the device is currently closing down
        // (locked_logical_handle != -1). If the client is requesting data when
        // the device is closing down this happens due to a reconfiguring.
        ADM_ASSERT(0);
        msg_p->base.result = STE_ADM_RES_DEVICE_RECONFIGURING;
        srv_send_reply(&msg_p->base);
        return;
    }

    il_tool_io_request_data(comp, port, msg_p);
}


// Drain is asynchronous.
static void io_idle_callback(void* param);
static void devapi_drain(msg_base_t* msg_p, void** client_pp)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

    struct devsession* session_p = (struct devsession*) *client_pp;

    if (is_lpa_test_mode()) {
        ALOG_STATUS("devapi_drain - is_lpa_test_mode() active..\n");
        srv_send_reply(msg_p);
        return;
    }


    if ( !(session_p->dev_handle[0] >= 1 || session_p->split_handle)) {
        ALOG_ERR("devapi_drain - STE_ADM_RES_INCORRECT_STATE\n");
        msg_p->result = STE_ADM_RES_INCORRECT_STATE;
        srv_send_reply(msg_p);
        return;
    }
    ADM_ASSERT(!session_p->drain_msg_p);

    // reply will be sent by io_idle_callback() when IO is idle
    session_p->drain_msg_p = msg_p;

    il_comp_t comp;
    OMX_U32 port;
    if (session_p->split_handle) {
        comp = session_p->split_handle;
        port = SPLITTER_INPUT_PORT;
    } else {
        if (!dev_get_endpoint(session_p->dev_handle[0], &comp, &port)) {
            // This should only fail if the device is closed, and that should
            // be caught by the above check (session_p->dev_handle[0] >= 1)
            ADM_ASSERT(0);
        }
    }

    ALOG_INFO("devapi_drain, draining %X.%d\n", (int) il_tool_raw_handle(comp), port);
    il_tool_io_eos(comp, port);
    il_tool_io_subscribe_idle_callback_once(comp, port, io_idle_callback, session_p);
}


static void io_idle_callback(void* param)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    struct devsession* session_p = (struct devsession*) param;

    ALOG_INFO("Drain complete, sending reply msg\n");
    ADM_ASSERT(session_p->drain_msg_p);
    srv_send_reply(session_p->drain_msg_p);
    session_p->drain_msg_p = NULL;
}


void devapi_get_sink_latency(msg_sink_latency_t* msg_p, srv_session_t** client_pp)
{
    (void) client_pp;
    ALOG_INFO("devapi_get_sink_latency: device = %s\n", msg_p->dev_name);

    ste_adm_res_t res = dev_get_sink_latency(msg_p->dev_name, &msg_p->latencyMs);

    if (res != STE_ADM_RES_OK) {
        ALOG_ERR("devapi_get_sink_latency failed. Error = %d\n", res);
    }

    msg_p->base.result = res;
    srv_send_reply(&msg_p->base);
}

///////////////////////////////////////////////////////////////////////////////
//
//  PCM probe
//  =========================================================================
//
//
//
//
//
//
///////////////////////////////////////////////////////////////////////////////

void devapi_set_pcm_probe(msg_set_pcm_probe_t* msg_p, srv_session_t** client_pp)
{
    (void) client_pp;
    ste_adm_res_t res = STE_ADM_RES_OK;
    ALOG_INFO("devapi_set_pcm_probe: id = %d, status = %s\n", msg_p->probe_id, msg_p->enabled ? "enabled" : "disabled");
#ifdef ADM_MMPROBE
    res = dev_set_pcm_probe(msg_p->probe_id, msg_p->enabled);
    if (res != STE_ADM_RES_OK) {
        ALOG_ERR("devapi_set_pcm_probe failed. Error = %d\n", res);
    }
#else
    ALOG_WARN("devapi_set_pcm_probe: mmprobe not enabled!");
#endif // ADM_MMPROBE
    msg_p->base.result = res;
    srv_send_reply(&msg_p->base);
}

///////////////////////////////////////////////////////////////////////////////
//
//  Volumes and mute
//  =========================================================================
//
//
//
//
//
//
///////////////////////////////////////////////////////////////////////////////

void devapi_set_app_vol(msg_app_volume_t* msg_p, srv_session_t** client_pp)
{
    (void) client_pp;
    ALOG_STATUS("Set application volume: device = %s, volume = %d mB\n", msg_p->dev_name, msg_p->volume);

    ste_adm_res_t res = dev_set_volume(msg_p->dev_name, 0, msg_p->volume, 0);

    if (res != STE_ADM_RES_OK)
    {
        ALOG_ERR("devapi_set_app_vol failed. Error = %d\n", res);
        msg_p->base.result = res;
    }

    srv_send_reply(&msg_p->base);
}


void devapi_get_app_vol(msg_app_volume_t* msg_p, srv_session_t** client_pp)
{
    (void) client_pp;
    ALOG_INFO("devapi_get_app_vol: device = %s\n", msg_p->dev_name);

    ste_adm_res_t res = dev_get_volume(msg_p->dev_name, 0, &msg_p->volume);

    if (res != STE_ADM_RES_OK)
    {
        ALOG_ERR("devapi_get_app_vol failed. Error = %d\n", res);
        msg_p->base.result = res;
    }

    ALOG_INFO("devapi_get_app_vol: device = %s, volume = %d\n", msg_p->dev_name, msg_p->volume);
    srv_send_reply(&msg_p->base);
}




///////////////////////////////////////////////////////////////////////////////
//
//  Set/Get IL Config
//  =========================================================================
//
//
//
//
//
//
///////////////////////////////////////////////////////////////////////////////


struct get_active_devices_state
{
    srv_session_t srv_session;
    int subscription_handle;
    int devices_changed; // Set to 1 if device notification has occurred since last call. Initialized to 1.
    msg_get_active_devices_t* deferred_msg_p;
};

static void devapi_get_active_devics_cb(void* param)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    ALOG_INFO("devapi_get_active_devics_cb: Active devices changed.\n");

    struct get_active_devices_state* state_p = (struct get_active_devices_state*) param;


    if (state_p->deferred_msg_p)
    {
        ALOG_INFO("devapi_get_active_devics_cb: deferred_msg_p exists.\n");
        int size = ADM_NUM_PHY_DEV;
        ste_adm_res_t res = dev_get_active_devices(state_p->deferred_msg_p->device_list, &size);
        if (res != STE_ADM_RES_OK) {
            ALOG_ERR("devapi_get_active_devics_cb: dev_get_active_devices failed: %d\n", res);
            state_p->deferred_msg_p->nbr_devices = 0;
        }
        else
        {
            ALOG_INFO("devapi_get_active_devics_cb: got %d devices.\n", size);
            int i;
            for (i = 0; i < size; i++)
            {
                if(state_p->deferred_msg_p->device_list[i].voice_active)
                {
                    state_p->deferred_msg_p->device_list[i].voicecall_samplerate = csapi_get_samplerate();
                }
            }
            state_p->deferred_msg_p->nbr_devices = size;
        }
        state_p->deferred_msg_p->base.result = res;
        srv_send_reply(&state_p->deferred_msg_p->base);
        state_p->deferred_msg_p = NULL;
    }
    else
    {
        ALOG_INFO("devapi_get_active_devics_cb: No queued request\n");
        state_p->devices_changed = 1;
    }

}

static void devapi_get_active_devices_close(srv_session_t* param)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    struct get_active_devices_state* state_p = (struct get_active_devices_state*) param;

    dev_unsubscribe_active_devices(state_p->subscription_handle);

    memset(state_p, 0x98, sizeof(*state_p));
    free(state_p);
}

void devapi_get_active_devices(msg_get_active_devices_t* msg_p, srv_session_t** client_pp)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

    ALOG_INFO("devapi_get_active_devices:\n");
    struct get_active_devices_state* state_p;

    if (*client_pp == NULL) {
        ALOG_INFO("devapi_get_active_devices: Allocating new state.\n");
        state_p = malloc(sizeof(struct get_active_devices_state));
        if (!state_p) {
            ALOG_ERR("devapi_get_active_devices: malloc failed\n");
            msg_p->base.result = STE_ADM_RES_ERR_MALLOC;
            srv_send_reply(&msg_p->base);
            return;
        }

        state_p->srv_session.dtor_fp = devapi_get_active_devices_close;
        state_p->srv_session.dtor_must_be_serialized = 0;
        state_p->srv_session.api_group = API_GROUP_ATT;
        state_p->devices_changed = 1;
        state_p->deferred_msg_p = NULL;
        *client_pp = &state_p->srv_session;

        ste_adm_res_t res = dev_subscribe_active_devices(devapi_get_active_devics_cb,
                                                         state_p,
                                                         &state_p->subscription_handle);
        if (res != STE_ADM_RES_OK) {
            msg_p->base.result = res;
            ALOG_ERR("devapi_get_active_devices: dev_subscribe_active_devices failed: %d\n", res);
            srv_send_reply(&msg_p->base);
            return;
        }

    } else {
        ALOG_INFO("devapi_get_active_devices: state exists, using old state\n");
        state_p = (struct get_active_devices_state*) *client_pp;
        ADM_ASSERT(state_p->deferred_msg_p == NULL);
    }


    if (state_p->devices_changed) {
        ALOG_INFO("devapi_get_active_devices: devices changed\n");
        state_p->devices_changed = 0;
        int size = ADM_NUM_PHY_DEV;
        ste_adm_res_t res = dev_get_active_devices(msg_p->device_list, &size);
        if (res != STE_ADM_RES_OK) {
            ALOG_ERR("devapi_get_active_devices: dev_get_active_devices failed: %d\n", res);
            msg_p->nbr_devices = 0;
        }
        else
        {
            int i;
            for (i = 0; i < size; i++)
            {
                if(msg_p->device_list[i].voice_active)
                {
                    msg_p->device_list[i].voicecall_samplerate = csapi_get_samplerate();
                }
            }
            msg_p->nbr_devices = size;
        }
        msg_p->base.result = res;
    } else {
        state_p->deferred_msg_p = msg_p;
        ALOG_INFO("devapi_get_active_devices: Waiting for changes\n");
        return;
    }

    ALOG_INFO("devapi_get_active_devices: sending reply\n");
    srv_send_reply(&msg_p->base);
}
void devapi_reload_settings(msg_base_t* msg_p, srv_session_t** client_pp)
{
    (void) client_pp;
    int res = STE_ADM_RES_OK;
    struct client_info* client_tbl;
    int max_clients = 0;
    int i, j;

    ALOG_STATUS("Reload device settings");

    srv_get_clients(&max_clients, &client_tbl);
    reload_dev_info* dev_tbl = malloc((size_t) max_clients * sizeof(reload_dev_info));
    if (dev_tbl == NULL) {
        ALOG_ERR("devapi_reload_settings: malloc failed\n");
        msg_p->result = STE_ADM_RES_ERR_MALLOC;
        srv_send_reply(msg_p);
        return;
    }
    memset(dev_tbl, 0, (size_t) max_clients * sizeof(reload_dev_info));

    /* Close all open devices */
    for (i=0 ; i < max_clients ; i++) {
        if (client_tbl[i].fd != -1 &&
            client_tbl[i].client_state_p != NULL &&
            client_tbl[i].client_state_p->api_group == API_GROUP_DEV &&
            ((topapi_session_t*)client_tbl[i].client_state_p)->vtbl != NULL &&
            ((topapi_session_t*)client_tbl[i].client_state_p)->vtbl->open == devapi_open &&
            ((topapi_session_t*)client_tbl[i].client_state_p)->session_p)
        {
            topapi_session_t* topsession_p = (topapi_session_t*) (client_tbl[i].client_state_p);
            struct devsession* session_p = (struct devsession*) topsession_p->session_p;
            for (j = 0; j < MAX_NO_DEV_HANDLES; j++) {
                dev_tbl[i].dev[j] = NULL;
                if (session_p->dev_handle[j] != 0)
                {
                    ALOG_INFO("Close device %s to reload settings from db",
                        dev_get_name(session_p->dev_handle[j]));

                    dev_tbl[i].dev[j] = malloc(strlen(dev_get_name(session_p->dev_handle[j])) + 1);
                    if (dev_tbl[i].dev[j] == NULL) {
                        ALOG_ERR("devapi_reload_settings: malloc failed\n");
                        msg_p->result = STE_ADM_RES_ERR_MALLOC;
                        goto cleanup;
                    }
                    strcpy(dev_tbl[i].dev[j], dev_get_name(session_p->dev_handle[j]));

                    il_comp_t comp;
                    OMX_U32 port;
                    if (!dev_get_endpoint(session_p->dev_handle[j], &comp, &port)) {
                        ADM_ASSERT(0);
                    }

                    /* Get the PCM settings, needed when reopening the devices */
                    srv_become_worker_thread();
                    OMX_AUDIO_PARAM_PCMMODETYPE pcm_mode;
                    IL_TOOL_INIT_CONFIG_STRUCT(pcm_mode);
                    pcm_mode.nPortIndex = port;
                    IL_TOOL_GET_PARAM(comp, OMX_IndexParamAudioPcm, &pcm_mode);
                    dev_tbl[i].samplerate = (int) pcm_mode.nSamplingRate;
                    dev_tbl[i].format = (ste_adm_format_t) pcm_mode.nChannels;
                    srv_become_normal_thread();

                    if (session_p->split_handle) {
                        srv_become_worker_thread();
                        IL_TOOL_DISCONNECT(session_p->split_handle, (OMX_U32) j+1, comp, port);
                        srv_become_normal_thread();
                        dev_close(session_p->dev_handle[j], 0);
                    } else {

                        /* Get buffer size and minimum buffer count, needed when
                           reopening a device not connected to a splitter */
                        srv_become_worker_thread();
                        OMX_PARAM_PORTDEFINITIONTYPE port_def;
                        IL_TOOL_INIT_CONFIG_STRUCT(port_def);
                        port_def.nPortIndex = port;
                        IL_TOOL_GET_PARAM(comp, OMX_IndexParamPortDefinition, &port_def);
                        dev_tbl[i].bufsz = port_def.nBufferSize;
                        dev_tbl[i].num_bufs = port_def.nBufferCountActual;
                        srv_become_normal_thread();

                        /* Get shared memory file descriptor, needed when
                           reopening a device not connected to a splitter
                           (ADM client will use the same shared memory when
                           reopening the device)*/
                        int fd_to_dup;
                        il_tool_get_buffers(comp, port, &fd_to_dup, NULL, NULL);
                        dev_tbl[i].shm_fd = dup(fd_to_dup);

                        dev_close(session_p->dev_handle[j], 1);
                    }
                    session_p->dev_handle[j] = 0;
                }
            }
        }
    }

    /* Reload cscall settings */
    ALOG_INFO("Reload cscall settings");
    msg_p->result = csapi_reload_device_settings();
    if (msg_p->result != STE_ADM_RES_OK) {
        goto cleanup;
    }

    /* Reopen all devices to reload settings */
    for (i=0 ; i < max_clients ; i++) {
        if (client_tbl[i].fd != -1 &&
            client_tbl[i].client_state_p != NULL &&
            ((topapi_session_t*)client_tbl[i].client_state_p)->vtbl != NULL &&
            ((topapi_session_t*)client_tbl[i].client_state_p)->vtbl->open == devapi_open &&
            ((topapi_session_t*)client_tbl[i].client_state_p)->session_p)
        {
            topapi_session_t* topsession_p = (topapi_session_t*) (client_tbl[i].client_state_p);
            struct devsession* session_p = (struct devsession*) topsession_p->session_p;

            int is_input = 0;
            int reopen_device = 0;
            struct dev_params p;
            p.name_in  = NULL; // Only used for voicecall
            p.name_out = NULL; // Only used for voicecall
            p.samplerate = dev_tbl[i].samplerate;
            p.format = dev_tbl[i].format;
            p.shm_fd = -1;


            for (j = 0; j < MAX_NO_DEV_HANDLES; j++) {
                if (dev_tbl[i].dev[j] != NULL) {
                    if (adm_db_io_info(dev_tbl[i].dev[j], &is_input) != STE_ADM_RES_OK) {
                        ALOG_ERR("Failed to get direction of %s from db!\n", dev_tbl[i].dev[j]);
                        msg_p->result = STE_ADM_RES_NO_SUCH_COMPONENT;
                        goto cleanup;
                    }
                    /* found device to be reopened */
                    reopen_device = 1;
                    break;
                }
            }

            if (reopen_device) {
                if (is_input) {
                    if (session_p->dev_handle[0] == 0) {
                        p.alloc_buffers = 1;
                        p.bufsz = dev_tbl[i].bufsz;
                        p.num_bufs = dev_tbl[i].num_bufs;
                        p.shm_fd = dev_tbl[i].shm_fd; // Use the same shared memory as
                                                      // the device that was closed
                        ALOG_INFO("Open device %s to reload settings from db", dev_tbl[i].dev[0]);
                        msg_p->result = dev_open(dev_tbl[i].dev[0], 0, &p, &session_p->dev_handle[0]);
                    }
                } else {
                    for (j = 0; j < MAX_NO_DEV_HANDLES; j++) {
                        if (session_p->dev_handle[j] == 0 && dev_tbl[i].dev[j] != NULL) {
                            p.alloc_buffers = 0;
                            ALOG_INFO("Open device %s to reload settings from db", dev_tbl[i].dev[j]);
                            msg_p->result = dev_open(dev_tbl[i].dev[j], 0, &p, &session_p->dev_handle[j]);
                            if (msg_p->result == STE_ADM_RES_OK) {
                                /* Connect the output device to splitter output. */
                                il_comp_t comp;
                                OMX_U32 port;
                                if (!dev_get_endpoint(session_p->dev_handle[j], &comp, &port)) {
                                    ADM_ASSERT(0);
                                }
                                srv_become_worker_thread();
                                IL_TOOL_CONNECT_AND_ENABLE_WITH_SRC_CFG(session_p->split_handle, (OMX_U32) j+1, comp, port);
                                srv_become_normal_thread();
                            }
                        }
                    }
                }
            }
        }
    }

cleanup:

    /* Free the device names */
    for (i=0 ; i < max_clients ; i++) {
        for (j = 0; j < MAX_NO_DEV_HANDLES; j++) {
            if (dev_tbl[i].dev[j] != NULL) {
                free(dev_tbl[i].dev[j]);
            }
        }
    }
    free(dev_tbl);

    if (res != STE_ADM_RES_OK) {
        msg_p->result = res;
    }
    if (msg_p->result == STE_ADM_RES_OK) {
        ALOG_STATUS("Reloaded device settings successfully!");
    } else {
        ALOG_ERR("Failed to reload device settings!");
    }
    srv_send_reply(msg_p);
}



///////////////////////////////////////////////////////////////////////////////
//
// Reference test code, LPA minimal graph
//
//

#include "AFM_Types.h"
#include "AFM_Index.h"
#include <stdio.h>
#include "OMX_Audio.h"
#include "AFM_Types.h"
#include "AFM_Index.h"
#include "omx_ste_speech_proc.h"
#include "audio_render_chipset_api.h"
#include "audio_sourcesink_chipset_api.h"
#include "OMX_DRCExt.h"
#include "OMX_CoreExt.h"
#include "audio_transducer_chipset_api.h"
#include "OMX_STE_VirtualSurroundExt.h"
#ifdef ANDROID
    #include "cutils/properties.h"
#endif

static void setup_spl(il_comp_t spl);
static void setup_pcm_48s(il_comp_t spl, int port);



static int is_lpa_test_mode()
{
#ifndef ADM_DBG_X86
    static char prop_static_buf[PROPERTY_VALUE_MAX];
    prop_static_buf[0] = 0;

    property_get("ste.debug.adm.minilpa", prop_static_buf, "");
    if (prop_static_buf[0] == '1') return 1;
#endif
    return 0;
}

static int lpa_testhwhandler_pos;
static int lpa_testhw_handler_dev_next(const char** dev_name_pp)
{
    if (lpa_testhwhandler_pos == 0) {
        *dev_name_pp = STE_ADM_DEVICE_STRING_HSOUT;
        lpa_testhwhandler_pos = 1;
        return 0;
    }

    return 1;
}

static int lpa_testhw_handler_dev_next_d2d(const char** src_dev_name_pp, const char** dst_dev_name_pp)
{
    (void) src_dev_name_pp;
    (void) dst_dev_name_pp;
    return 1; // No more entries
}

static void lpa_test_setup(struct devsession* dev)
{
    ste_adm_res_t res = STE_ADM_RES_OK;

    srv_become_worker_thread();
    IL_TOOL_CREATE(&dev->lpa_spl, "OMX.ST.AFM.pcmprocessing.spl", "lpa_spl");
    setup_pcm_48s(dev->lpa_spl, 0);
    setup_pcm_48s(dev->lpa_spl, 1);
    setup_spl(dev->lpa_spl);

    IL_TOOL_CREATE(&dev->lpa_sink, "OMX.ST.AFM.NOKIA_AV_SINK", "lpa_sink");
    setup_pcm_48s(dev->lpa_sink, 0);

    OMX_AUDIO_CONFIG_VOLUMETYPE t3;
    IL_TOOL_INIT_CONFIG_STRUCT(t3);
    t3.nPortIndex = 0;
    t3.bLinear = 1;
    t3.sVolume.nValue = 90;
    t3.sVolume.nMin = 0;
    t3.sVolume.nMax = 100;
    IL_TOOL_SET_CONFIG(dev->lpa_sink, OMX_IndexConfigAudioVolume, &t3);


    OMX_AUDIO_CONFIG_MUTETYPE mt;
    IL_TOOL_INIT_CONFIG_STRUCT(mt);
    mt.nPortIndex = 0;
    mt.bMute = 0;
    IL_TOOL_SET_CONFIG(dev->lpa_sink, OMX_IndexConfigAudioMute, &mt);

    //
    // Set the output size to match LowPowerAudio burst size (32 ms @ 48 kHz)
    //
    OMX_PARAM_PORTDEFINITIONTYPE portdef;
    IL_TOOL_INIT_CONFIG_STRUCT(portdef);
    portdef.nPortIndex = 0;
    IL_TOOL_GET_PARAM(dev->lpa_spl, OMX_IndexParamPortDefinition, &portdef);
    portdef.nBufferSize = 32 * 48 * 2 * 2; // 32 ms, 48 kHz, 2 channels a 2 bytes per channel
    portdef.nBufferCountActual = 3;
    IL_TOOL_SET_PARAM(dev->lpa_spl, OMX_IndexParamPortDefinition, &portdef);



    //
    // Activate DSP/ARM buffer synchronization mode
    //
    AFM_PARAM_HOST_MPC_SYNC_TYPE hostsync;
    IL_TOOL_INIT_CONFIG_STRUCT(hostsync);
    hostsync.nPortIndex = 0;
    hostsync.enabled = 1;
    IL_TOOL_SET_PARAM(dev->lpa_spl, AFM_IndexParamHostMpcSync, &hostsync);


    IL_TOOL_DISABLE_PORT(dev->lpa_spl, 1);
    IL_TOOL_DISABLE_PORT(dev->lpa_sink, 0);
    IL_TOOL_CONNECT_SRC_CFG(dev->lpa_spl, 1, dev->lpa_sink, 0);

    // The component MUST be listed in order Source to Sink
    IL_TOOL_DISABLE_PORT(dev->lpa_spl, 0);
    IL_TOOL_ENTER_STATE(OMX_StateIdle, dev->lpa_spl, dev->lpa_sink);
    IL_TOOL_ENTER_STATE(OMX_StateExecuting, dev->lpa_spl, dev->lpa_sink);

    // ASoC
    sqlite3* db_h;
    adm_db_get_handle(&db_h);
    lpa_testhwhandler_pos = 0;
#if defined(ADM_DBG_X86)
    (void)ste_adm_hw_handler(db_h, lpa_testhw_handler_dev_next, lpa_testhw_handler_dev_next_d2d,FADESPEED_FAST);
#else
    (void)Alsactrl_Hwh(db_h, lpa_testhw_handler_dev_next, lpa_testhw_handler_dev_next_d2d,FADESPEED_FAST);
#endif
    IL_TOOL_ENABLE_PORT_WITH_IO(dev->lpa_spl, 0);


cleanup:;
    srv_become_normal_thread();
}

static void lpa_test_destroy(struct devsession* dev)
{
    ste_adm_res_t res = STE_ADM_RES_OK;
    srv_become_worker_thread();
    IL_TOOL_DISCONNECT(dev->lpa_spl, 1, dev->lpa_sink, 0);
    IL_TOOL_DISABLE_PORT_WITH_IO(dev->lpa_spl, 0);

    IL_TOOL_ENTER_STATE(OMX_StateIdle, dev->lpa_spl, dev->lpa_sink);
    IL_TOOL_ENTER_STATE(OMX_StateLoaded, dev->lpa_spl, dev->lpa_sink);

    IL_TOOL_DESTROY(&dev->lpa_spl);
    IL_TOOL_DESTROY(&dev->lpa_sink);

cleanup:;
    srv_become_normal_thread();
}

#include "audio_chipset_api_index.h"

static void setup_spl(il_comp_t spl)
{
    ste_adm_res_t res = STE_ADM_RES_OK;

    AFM_AUDIO_PARAM_SPLTYPE t1;
    IL_TOOL_INIT_CONFIG_STRUCT(t1);

    t1.MemoryPreset = 0;
    t1.nType = 1;
    t1.nAttackTime = 400;
    t1.nReleaseTime = 200;
    IL_TOOL_SET_PARAM(spl, AFM_IndexParamSpl, &t1);

    OMX_AUDIO_CONFIG_SPLLIMITTYPE t2;
    IL_TOOL_INIT_CONFIG_STRUCT(t2);
    t2.nPortIndex = 0;
    t2.bEnable = 1;
    t2.eMode = 0;
    t2.nSplLimit = -600;
    t2.nGain = 0;
    IL_TOOL_SET_CONFIG(spl, OMX_IndexConfigSpllimit, &t2);

    OMX_AUDIO_CONFIG_VOLUMETYPE t3;
    IL_TOOL_INIT_CONFIG_STRUCT(t3);
    t3.nPortIndex = 0;
    t3.bLinear = 0;
    t3.sVolume.nValue = 600;
    t3.sVolume.nMin = 0;
    t3.sVolume.nMax = 600;
    IL_TOOL_SET_CONFIG(spl, OMX_IndexConfigAudioVolume, &t3);

    cleanup:;
}

static void setup_pcm_48s(il_comp_t spl, int port)
{
    ste_adm_res_t res = STE_ADM_RES_OK;

    OMX_AUDIO_PARAM_PCMMODETYPE pcm;
    IL_TOOL_INIT_CONFIG_STRUCT(pcm);

    pcm.nPortIndex = (OMX_U32) port;
    pcm.nChannels = 2;
    pcm.eNumData = 0;
    pcm.eEndian = 1;
    pcm.bInterleaved = 1;
    pcm.nBitPerSample = 16;
    pcm.nSamplingRate = 48000;
    pcm.ePCMMode = 0;
    pcm.eChannelMapping[0] = 1;
    pcm.eChannelMapping[1] = 2;

    IL_TOOL_SET_PARAM(spl, OMX_IndexParamAudioPcm, &pcm);

cleanup:;
}





