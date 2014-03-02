/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#define ADM_LOG_FILENAME "dev"
#include "ste_adm_dev.h"
#include "ste_adm_dbg.h"
#include "ste_adm_client.h" // ste_adm_res_t
#include "ste_adm_msg.h"    // ADM_MAX_DEVICE_NAME_LENGTH
#include <string.h>
#include "ste_adm_config.h"
#include "ste_adm_db.h" // adm_db_chain_type_t - put elsewhere? ChainId?
#include "ste_adm_dev_omx.h"
#include "ste_adm_dev_omx_graph.h"
#include "ste_adm_srv.h"
#include "ste_adm_omx_tool.h"
#include <limits.h>
#include "audio_hw_control_chipset_api.h"
#include "audio_sourcesink_chipset_api.h"
#include "audio_chipset_api_index.h"
#include "OMX_CoreExt.h"
#include "AFM_Index.h"
#include "AFM_Types.h"
#include "ste_adm_omx_core.h"
#include "ste_adm_dev_omx_util.h"
#ifdef ADM_MMPROBE
#include "t_mm_probe.h"
#endif

static int MIN(int a, int b) { return (a<b) ? a : b; }
static int MAX(int a, int b) { return (a>b) ? a : b; }


// TODO: Rename other files:
// ste_adm_omx.c       -> ste_adm_dev_omx.c
// ste_adm_omx_graph.c -> ste_adm_dev_omx_graph.c


typedef struct
{
    char name[ADM_MAX_DEVICE_NAME_LENGTH+1]; // if name[0] == 0, entry is invalid

    // if != -1, the graph of the specified handle is being changed
    // by the low-prio thread and can become invalid at any time. Thus,
    // the component / port for this handle should not be returned to the user.
    int  locked_logical_handle;

    dev_omx_graph_t  graph;
    int lpa_mode;
} device_t;

typedef struct
{
    char src[ADM_MAX_DEVICE_NAME_LENGTH+1]; // if name[0] == 0, entry is invalid
    char dst[ADM_MAX_DEVICE_NAME_LENGTH+1]; // if name[0] == 0, entry is invalid
} device_to_device_t;

static device_t   g_dev[ADM_NUM_PHY_DEV];
static device_to_device_t   g_dev_hw[ADM_NUM_PHY_DEV];
ste_adm_tty_mode_t g_tty_mode;

#ifdef ADM_MMPROBE

#define MAX_NBR_OF_PROBES_PER_DEVICE 59
#define MAX_NBR_OF_PROBE_DEVICES 15

const char* probe_comp2str(pcm_probe_comp_t pcm_probe_comp)
{
    switch (pcm_probe_comp) {
    case PCM_PROBE_IO:               return "PCM_PROBE_IO";
    case PCM_PROBE_COMMON_EFFECT:    return "PCM_PROBE_COMMON_EFFECT";
    case PCM_PROBE_COMMON_MIX_SPLIT: return "PCM_PROBE_COMMON_MIX_SPLIT";
    case PCM_PROBE_VOICE_EFFECT:     return "PCM_PROBE_VOICE_EFFECT";
    case PCM_PROBE_APP_EFFECT:       return "PCM_PROBE_APP_EFFECT";
    case PCM_PROBE_APP_MIX_SPLIT:    return "PCM_PROBE_APP_MIX_SPLIT";
    default:                         return "<INVALID>";
    }
}

typedef struct
{
    const char          *device_name;
    int                 base_probe_id; // The first id for this dev
    int                 pcm_probe_state[MAX_NBR_OF_PROBES_PER_DEVICE];
} pcm_probe_info_t;

static pcm_probe_info_t g_pcm_probe[MAX_NBR_OF_PROBE_DEVICES] =
{
    {STE_ADM_DEVICE_STRING_EARP,    MM_PROBE_EARPIECE_SNK,  {0}},
    {STE_ADM_DEVICE_STRING_SPEAKER, MM_PROBE_SPEAKER_SNK,   {0}},
    {STE_ADM_DEVICE_STRING_MIC,     MM_PROBE_MIC_SRC,       {0}},
    {STE_ADM_DEVICE_STRING_HSOUT,   MM_PROBE_HSETOUT_SNK,   {0}},
    {STE_ADM_DEVICE_STRING_HSIN,    MM_PROBE_HSETIN_SRC,    {0}},
    {STE_ADM_DEVICE_STRING_USBOUT,  MM_PROBE_USBOUT_SNK,    {0}},
    {STE_ADM_DEVICE_STRING_USBIN,   MM_PROBE_USBIN_SRC,     {0}},
    {STE_ADM_DEVICE_STRING_FMTX,    MM_PROBE_FMTX_SNK,      {0}},
    {STE_ADM_DEVICE_STRING_FMRX,    MM_PROBE_FMRX_SRC,      {0}},
    {STE_ADM_DEVICE_STRING_BTOUT,   MM_PROBE_BTOUT_SNK,     {0}},
    {STE_ADM_DEVICE_STRING_BTIN,    MM_PROBE_BTIN_SRC,      {0}},
    {STE_ADM_DEVICE_STRING_BTWBOUT, MM_PROBE_BTWBOUT_SNK,   {0}},
    {STE_ADM_DEVICE_STRING_BTWBIN,  MM_PROBE_BTWBIN_SRC,    {0}},
    {STE_ADM_DEVICE_STRING_A2DP,    MM_PROBE_A2DP_SNK,      {0}},
    {STE_ADM_DEVICE_STRING_HDMI,    MM_PROBE_HDMI_SNK,      {0}}
};
#endif // ADM_MMPROBE

// log_idx = 0     - vc
// log_idx = [1,8] - app, on corresponding port
//
// phys_handle: index in g_dev

static void update_latency_settings(device_t *dev);
static void g_lpa_event_callback(void* callback_param, il_comp_t hComponent,
        OMX_EVENTEXTTYPE eEvent,
        OMX_U32 Data1, OMX_U32 Data2, OMX_PTR pEventData);
static void dev_notify_lpa_changes(device_t *dev);

static int make_handle(int phys_handle_part, int log_handle_part)
{
    return (phys_handle_part << 8) | log_handle_part;
}

static int get_phys_handle_part(device_t* dev_p)
{
    int phys_index = dev_p - &g_dev[0];
    ADM_ASSERT(phys_index >= 0 && phys_index < ADM_NUM_PHY_DEV);
    return phys_index + 1;
}

static int get_logical_handle(int handle)
{
    return handle & 0xFF;
}

static device_t* get_dev_by_handle(int handle)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    ADM_ASSERT(handle >= 1);
    handle = (handle >> 8) - 1;
    if (!g_dev[handle].name[0]) {
        return 0;
    }
    return &g_dev[handle];
}

const char* dev_get_name(int handle)
{
    device_t* dev_p = get_dev_by_handle(handle);
    if (dev_p) {
        return dev_p->name;
    }

    return NULL;
}

void dev_init()
{
    memset(g_dev, 0, sizeof(g_dev));
    memset(g_dev_hw, 0, sizeof(g_dev_hw));
    g_tty_mode = STE_ADM_TTY_MODE_OFF;
}


static device_t* get_dev_by_name(const char* name)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    ADM_ASSERT(name && *name);
    int i;
    for (i=0 ; i < ADM_NUM_PHY_DEV ; i++) {
        if (strcmp(g_dev[i].name, name) == 0) {
            return &g_dev[i];
        }
    }

    return NULL;
}

static int can_alloc_dev()
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    int i;
    for (i=0 ; i < ADM_NUM_PHY_DEV ; i++) {
        if (g_dev[i].name[0] == 0) {
            return 1;
        }
    }

    return 0;
}

static device_t* add_dev(device_t* template_p)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    int i;
    for (i=0 ; i < ADM_NUM_PHY_DEV ; i++) {
        if (g_dev[i].name[0] == 0) {
            g_dev[i] = *template_p;
            ADM_ASSERT(g_dev[i].name[0] != 0);
            return &g_dev[i];
        }
    }

    return 0;
}

static void dealloc_device(device_t* dev_p)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    dev_p->name[0] = 0;
}


int dev_get_endpoint(int handle, il_comp_t* comp_p, OMX_U32* port_p)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    device_t* dev_p = get_dev_by_handle(handle);
    int log_handle = get_logical_handle(handle);
    if (dev_p->locked_logical_handle == log_handle) {
        ALOG_INFO("dev_get_endpoint for %X NOK; logical handle locked\n", handle);
        return 0;
    }

    // TODO? No error handling in get_endpoint, only asserts.
    *comp_p = NULL;
    dev_omx_get_endpoint(&dev_p->graph, log_handle, comp_p, port_p);
    ADM_ASSERT(*comp_p != NULL);

    return 1;
}

ste_adm_res_t dev_get_endpoint_echoref(int handle, il_comp_t* comp_p, OMX_U32* port_p)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    ste_adm_res_t res;

    device_t* dev_p = get_dev_by_handle(handle);
    int log_handle = get_logical_handle(handle);
    if (dev_p->locked_logical_handle == log_handle) {
        ALOG_INFO_VERBOSE("dev_get_endpoint for %X NOK; logical handle locked\n", handle);
        return STE_ADM_RES_INTERNAL_ERROR;
    }

    res = dev_omx_get_endpoint_echoref(&dev_p->graph, comp_p, port_p);

    return res;
}




/**
*
* vc_params       - if NULL, open app device, otherwise open vc dev, as
*                   specified.
*
* comp_id_p       - the job component ID the port to connect to will have. Use
*                   to link to other job commands.
*
* port_p          - the IL port index of the port of the device, after it is
*                   opened.
*
*/
ste_adm_res_t dev_open(const char* name, int vc, struct dev_params* params_p, int* dev_handle)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    //
    // PREPARE
    //

    device_t old_dev_state;
    device_t new_dev_state;
    device_t* real_dev_p;

    ALOG_INFO(" >>> ENTER dev_open(%s, vc=%d, alloc_buffers=%d, format=%X)\n", name, vc, params_p->alloc_buffers, params_p->format);
    /* "Normal" devices like Headset, Earpiece, Speaker only support PCM format in mono or stereo.
       5.1 is supported as well for all devices since the mixer supports channel conversion.
       The dedicated format, from STE_ADM_FORMAT_AC3 to STE_ADM_FORMAT_WMA_PRO are only supported by the HDMI device.
    */
    if ((params_p->format >= STE_ADM_FORMAT_FIRST_CODED) &&
        (strcmp(name, STE_ADM_DEVICE_STRING_HDMI))) {
        ALOG_ERR("This encoded audio format is only supported on HDMI devices\n");
        return STE_ADM_RES_NO_SUCH_DEVICE;
    }

    struct dev_params tmp_params;

    memcpy(&tmp_params, params_p, sizeof(struct dev_params));

    real_dev_p = get_dev_by_name(name);
    if (!real_dev_p) {
        if (!can_alloc_dev()) {
            ALOG_ERR("Device limit reached\n");
            return STE_ADM_RES_DEV_LIMIT_REACHED;
        }
        memset(&new_dev_state, 0, sizeof(new_dev_state));
        new_dev_state.locked_logical_handle = -1; // no locked handle
        new_dev_state.lpa_mode = 0;
        ADM_ASSERT(strlen(name) <= ADM_MAX_DEVICE_NAME_LENGTH);
        ADM_ASSERT(strlen(name) >= 1);
        strcpy(new_dev_state.name, name);
    } else {
        old_dev_state = *real_dev_p;
        new_dev_state = old_dev_state;
    }



    //
    // EXECUTE
    //

    // NOTE: HW handler needs to read state, so we stay as a normal (non-worker) thread for a while..
    // open/close dev is serialized, so they should nto be closed from the other thread.
    SRV_DBG_ASSERT_IS_SERIALIZED_REQUEST;

    ste_adm_res_t retval = STE_ADM_RES_OK;

    // Set last device that has been requested to be opened
    il_tool_io_set_last_opened_dev(name);

    // If this is the first time we open the device, run the hardware
    // handler on it.
    if (!real_dev_p) {
        retval = dev_rescan_hw_handler(name, FADESPEED_FAST);
    }

    srv_become_worker_thread();

    if (retval == STE_ADM_RES_OK && !real_dev_p) {
        retval = adm_db_io_info(name, &new_dev_state.graph.is_input);
    }

    int log_handle_part;
    if (retval == STE_ADM_RES_OK) {
        ALOG_INFO("Device %s is %s\n", name, new_dev_state.graph.is_input ? "INPUT" : "OUTPUT");

        const char *tmp_name = name;
        if (strcmp(name, STE_ADM_DEVICE_STRING_HSIN) == 0 || strcmp(name, STE_ADM_DEVICE_STRING_HSOUT) == 0) {
            switch (g_tty_mode) {
                case STE_ADM_TTY_MODE_FULL:
                    tmp_params.name_in = STE_ADM_DEVICE_STRING_TTYIN;
                    tmp_params.name_out = STE_ADM_DEVICE_STRING_TTYOUT;
                    if (new_dev_state.graph.is_input) {
                        tmp_name = STE_ADM_DEVICE_STRING_TTYIN;
                    }
                    else {
                        tmp_name = STE_ADM_DEVICE_STRING_TTYOUT;
                    }
                break;
                case STE_ADM_TTY_MODE_HCO:
                    tmp_params.name_in = STE_ADM_DEVICE_STRING_TTYIN;
                    tmp_params.name_out = STE_ADM_DEVICE_STRING_HSOUT;
                    if (new_dev_state.graph.is_input) {
                        tmp_name = STE_ADM_DEVICE_STRING_TTYIN;
                    }
                break;
                case STE_ADM_TTY_MODE_VCO:
                    tmp_params.name_in = STE_ADM_DEVICE_STRING_HSIN;
                    tmp_params.name_out = STE_ADM_DEVICE_STRING_TTYOUT;
                    if (!new_dev_state.graph.is_input) {
                        tmp_name = STE_ADM_DEVICE_STRING_TTYOUT;
                    }
                break;
                default:
                break;
            }

            if (g_tty_mode == STE_ADM_TTY_MODE_OFF) {
                ALOG_INFO("TTY not enabled\n");
            }
            else {
                ALOG_INFO("TTY enabled mode %d. Using device name: %s\n", g_tty_mode, tmp_name);
            }
        }

        retval = dev_omx_setup_graph(&new_dev_state.graph,
                                                    tmp_name,
                                                    vc,
                                                    &tmp_params,
                                                    &log_handle_part);
    }

    srv_become_normal_thread();

    if (retval == STE_ADM_RES_OK && !real_dev_p && !new_dev_state.graph.is_input)
    {
        il_comp_t sink = dev_omx_get_sink(&new_dev_state.graph);
        il_tool_subscribe_events(sink, g_lpa_event_callback, NULL, 0);

        update_latency_settings(&new_dev_state);
    }

    //
    // COMMIT / REVERT
    //

    if (retval == STE_ADM_RES_OK) {
        if (!real_dev_p) {
            // Device did not exist; add it with the newly created state
            real_dev_p  = add_dev(&new_dev_state);
        } else {
            // Device already existed; set it to the new state
            *real_dev_p = new_dev_state;
        }
        *dev_handle = make_handle(get_phys_handle_part(real_dev_p),
                                    log_handle_part);
        ALOG_INFO("open dev OK, device handle: %X\n", *dev_handle);
    } else if (retval == STE_ADM_RES_UNRECOVERABLE_ERROR) {
        // Anything could have happened, nothing to do.. set ADM zombie flag?
        ALOG_ERR("Unrecoverable error, undefined state...\n");
    } else {
        ALOG_INFO("open dev FAILED, reverting\n");
        // Nothing to do
    }

    dev_notify_active_device_changes();

    ALOG_INFO(" >>> LEAVE dev_open(%s, %d) --> %X %d\n", name, vc, *dev_handle, retval);
    return retval;
}

ste_adm_res_t dev_close(int handle, int dealloc_buffers)
{
    ALOG_INFO(" >>> ENTER dev_close(%X)\n", handle);

    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    //
    // PREPARE
    //

    device_t old_state;
    device_t new_state;
    device_t* cur_state_p;

    cur_state_p = get_dev_by_handle(handle);
    if (!cur_state_p) {
        return STE_ADM_RES_INTERNAL_ERROR;
    }

    old_state = *cur_state_p;
    new_state = old_state;

    cur_state_p->locked_logical_handle = get_logical_handle(handle);

    il_comp_t comp;
    OMX_U32   port;
    dev_omx_get_endpoint(&cur_state_p->graph, cur_state_p->locked_logical_handle, &comp, &port);

    //
    // EXECUTE
    //
    srv_become_worker_thread();

    int last_device_closed;
    ste_adm_res_t res = dev_omx_close_graph(&new_state.graph,
                                                get_logical_handle(handle),
                                                dealloc_buffers,
                                                &last_device_closed);

    srv_become_normal_thread();

    //
    // COMMIT / REVERT
    //

    if (res == STE_ADM_RES_OK) {
        char old_dev_name[ADM_MAX_DEVICE_NAME_LENGTH+1];
        strcpy(old_dev_name, cur_state_p->name);

        *cur_state_p = new_state;
        if (last_device_closed) {
            dealloc_device(cur_state_p);

            dev_rescan_hw_handler(NULL, FADESPEED_FAST);
        }

        dev_notify_active_device_changes();
    } else {
        *cur_state_p = old_state;
    }

    ALOG_INFO(" >>> LEAVE dev_close(%X)\n", handle);
    return res;
}

ste_adm_res_t dev_connect_common(int handle)
{
    ALOG_INFO(" >>> ENTER dev_connect_common(%X)\n", handle);

    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

    device_t* cur_state_p = get_dev_by_handle(handle);
    if (!cur_state_p) {
        return STE_ADM_RES_INTERNAL_ERROR;
    }

    srv_become_worker_thread();
    int is_vc = (get_logical_handle(handle) == 0);
    ste_adm_res_t res = dev_omx_graph_connect_mixer_splitter_to_app_voice(&cur_state_p->graph, is_vc);
    srv_become_normal_thread();

    ALOG_INFO(" >>> LEAVE dev_connect_common(%X)\n", handle);
    return res;
}

ste_adm_res_t dev_disconnect_common(int handle)
{
    ALOG_INFO(" >>> ENTER dev_disconnect_common(%X)\n", handle);

    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

    device_t* cur_state_p = get_dev_by_handle(handle);
    if (!cur_state_p) {
        return STE_ADM_RES_INTERNAL_ERROR;
    }

    srv_become_worker_thread();
    int is_vc = (get_logical_handle(handle) == 0);
    ste_adm_res_t res = dev_omx_graph_disconnect_mixer_splitter_from_app_voice(&cur_state_p->graph, is_vc);
    srv_become_normal_thread();

    ALOG_INFO(" >>> LEAVE dev_disconnect_common(%X)\n", handle);
    return res;
}

ste_adm_res_t dev_reconfigure_effects(const char* name, const char* name2,
    int vc_in_disconnected, int vc_out_disconnected)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    ste_adm_res_t res = STE_ADM_RES_OK;

    ALOG_INFO(" >>> ENTER dev_reconfigure_effects() %s %s\n",
        name, name2[0] ? name2 : "");

    device_t* dev_p = NULL;
    device_t* dev2_p = NULL;
    dev_p = get_dev_by_name(name);
    if (name2[0]) {
        dev2_p = get_dev_by_name(name2);
    }

    srv_become_worker_thread();
    if (dev_p) {
        res = dev_omx_reconfigure_effects(&dev_p->graph, name, name2,
            dev_p->graph.is_input ? vc_in_disconnected : vc_out_disconnected);
    } else {
        ALOG_INFO("dev_reconfigure_effects: %s not yet opened", name);
    }
    if (res != STE_ADM_RES_OK) {
        ALOG_ERR("dev_reconfigure_effects failed for %s", name);
    } else if (name2[0]) {
        if (dev2_p) {
            res = dev_omx_reconfigure_effects(&dev2_p->graph, name2, name,
                dev2_p->graph.is_input ? vc_in_disconnected : vc_out_disconnected);
            if (res != STE_ADM_RES_OK) {
                ALOG_ERR("dev_reconfigure_effects failed for %s", name2);
            }
        }  else {
            ALOG_INFO("dev_reconfigure_effects: %s not yet opened", name2);
        }
    }

    srv_become_normal_thread();
    ALOG_INFO(" >>> LEAVE dev_reconfigure_effects() %s %s\n",
        name, name2[0] ? name2 : "");
    return res;
}


ste_adm_res_t dev_rescan_effects(ste_adm_effect_chain_type_t chain, const char* name, const char* name2)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    ste_adm_res_t res = STE_ADM_RES_OK;

    ALOG_INFO(" >>> ENTER dev_rescan_effects() %s %s\n",
        name, name2 != NULL ? name2 : "");

    device_t* dev_p = NULL;
    device_t* dev2_p = NULL;
    dev_p = get_dev_by_name(name);
    if (name2 != NULL && name2[0]) {
        dev2_p = get_dev_by_name(name2);
    }

    srv_become_worker_thread();
    if (dev_p) {
        res = dev_omx_rescan_effects(&dev_p->graph, chain, name, name2);
    } else {
        ALOG_INFO("dev_rescan_effects: %s not yet opened", name);
    }
    if (res != STE_ADM_RES_OK) {
        ALOG_ERR("dev_rescan_effects failed for %s", name);
    } else if (name2 != NULL && name2[0]) {
        if (dev2_p) {
            res = dev_omx_rescan_effects(&dev2_p->graph, chain, name2, name);
            if (res != STE_ADM_RES_OK) {
                ALOG_ERR("dev_rescan_effects failed for %s", name2);
            }
        }  else {
            ALOG_INFO("dev_rescan_effects: %s not yet opened", name2);
        }
    }

    srv_become_normal_thread();
    ALOG_INFO(" >>> LEAVE dev_rescan_effects() %s %s\n",
        name, name2 != NULL ? name2 : "");
    return res;
}


/**
*   Returns 1 if an app device is open for the given device name. Else 0 is returned.
*
* name            - Name of the device.
*
*/
int is_app_dev_open(const char* name)
{
    device_t* real_dev_p;

    real_dev_p = get_dev_by_name(name);
    if (NULL != real_dev_p && real_dev_p->graph.app.ref_cnt > 0) {
        return 1;
    }

    return 0;
}

/**
 * Returns 1 if device is in low power mode
 */
int dev_get_lpa_mode(int handle)
{
    device_t* dev = get_dev_by_handle(handle);

    if (dev)
    {
        return dev->lpa_mode;
    }

    return 0;
}

/**
*   Returns a list with all open devices.
*
* device_list            - Out parameter with the list.
* size                   - In/out parameter. Takes size of vector as input and returns size of content.
*
*/
int dev_get_active_devices(active_device_t* device_list, int* size)
{
    if (*size < ADM_NUM_PHY_DEV)
    {
        ALOG_ERR("dev_get_active_devices: Size too small.\n");
        *size=0;
        return STE_ADM_RES_INTERNAL_ERROR;
    }

    int i;
    int nbr_active = 0;
    for (i = 0; i < ADM_NUM_PHY_DEV; i++)
    {
        if(g_dev[i].name[0] != 0)
        {
            const char *actual_dev = NULL;
            ste_adm_res_t res = adm_db_toplevel_mapping_get(g_dev[i].name, &actual_dev);
            if ( res == STE_ADM_RES_OK)
            {
                strcpy(device_list[nbr_active].dev_name, actual_dev);
            }
            else
            {
                ALOG_ERR("dev_get_active_devices: adm_db_toplevel_mapping_get failed %d.\n", res);
                *size=0;
                return STE_ADM_RES_INTERNAL_ERROR;
            }
            device_list[nbr_active].app_active = (g_dev[i].graph.app.ref_cnt > 0);
            device_list[nbr_active].voice_active = (g_dev[i].graph.voice.ref_cnt > 0);
            device_list[nbr_active].voicecall_samplerate = -1;
            nbr_active++;
        }
    }
    *size = nbr_active;

    return STE_ADM_RES_OK;
}



///////////////////////////////////////////////////////////////////////////////
//
// Handling of subscription of LPA events
//
//

static void g_lpa_event_callback(void* callback_param, il_comp_t hComponent,
        OMX_EVENTEXTTYPE eEvent,
        OMX_U32 Data1, OMX_U32 Data2, OMX_PTR pEventData)
{
    (void) callback_param;
    (void) pEventData;
    (void) Data1;

    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

    if (eEvent == OMX_EventIndexSettingChanged && Data2 == AFM_IndexLowPowerRenderer) {
        ALOG_INFO("g_lpa_event_callback received settings changed event");

        int i;
        for (i = 0; i < ADM_NUM_PHY_DEV; i++) {
            if (g_dev[i].name[0]) {
                if(dev_omx_get_sink(&g_dev[i].graph) == hComponent &&  g_dev[i].locked_logical_handle == -1) {
                    break;
                }
            }
        }

        if (i != ADM_NUM_PHY_DEV) {
            update_latency_settings(&g_dev[i]);
            dev_notify_lpa_changes(&g_dev[i]);
        }
        else {
            ALOG_ERR("g_lpa_event_callback: Got LPA mode changed from %X, but no device was found.", (int)hComponent);
        }
    }
    else {
        ALOG_INFO("g_lpa_event_callback received unhandled event");
    }
}


///////////////////////////////////////////////////////////////////////////////
//
// Handling of subscription of active devices
//
//

// These are most likely going to be very rare..
#define MAX_ACTIVE_DEVICE_SUBSCRIPTIONS 4

static struct active_device_subscription
{
    dev_active_device_cb_fp_t   cb_fp;
    void*                       param;
} g_active_device_subscriptions[MAX_ACTIVE_DEVICE_SUBSCRIPTIONS];


void dev_notify_active_device_changes()
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    int i;
    for (i=0 ; i < MAX_ACTIVE_DEVICE_SUBSCRIPTIONS ; i++) {
        if (g_active_device_subscriptions[i].cb_fp) {

            g_active_device_subscriptions[i].cb_fp(g_active_device_subscriptions[i].param);
        }
    }
}


ste_adm_res_t dev_subscribe_active_devices(dev_active_device_cb_fp_t cb_fp,
                                     void* param,
                                     int* subscription_handle_p)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

    ADM_ASSERT(cb_fp);

    int i;
    for (i=0 ; i < MAX_ACTIVE_DEVICE_SUBSCRIPTIONS ; i++) {
        if (!g_active_device_subscriptions[i].cb_fp) {

            g_active_device_subscriptions[i].cb_fp = cb_fp;
            g_active_device_subscriptions[i].param = param;
            *subscription_handle_p = i;
            return STE_ADM_RES_OK;
        }
    }

    return STE_ADM_RES_ALL_IN_USE;
}

void dev_unsubscribe_active_devices(int handle)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    ADM_ASSERT(handle >= 0 && handle < MAX_ACTIVE_DEVICE_SUBSCRIPTIONS);
    ADM_ASSERT(g_active_device_subscriptions[handle].cb_fp);
    g_active_device_subscriptions[handle].cb_fp = NULL;
}

///////////////////////////////////////////////////////////////////////////////
//
// Handling of subscription of LPA mode changes
//
//

// These are most likely going to be very rare..
#define MAX_LPA_SUBSCRIPTIONS 20

static struct lpa_subscription
{
    dev_lpa_mode_cb_fp_t   cb_fp;
    int                    dev_handle;
    void*                  param;
} g_lpa_subscriptions[MAX_LPA_SUBSCRIPTIONS];


static void dev_notify_lpa_changes(device_t *dev)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

    int dev_phys_handle = get_phys_handle_part(dev);

    int i;
    for (i=0 ; i < MAX_LPA_SUBSCRIPTIONS ; i++) {
        if (g_lpa_subscriptions[i].cb_fp && ((g_lpa_subscriptions[i].dev_handle >> 8) == dev_phys_handle)) {
            g_lpa_subscriptions[i].cb_fp(g_lpa_subscriptions[i].param, dev->lpa_mode);
        }
    }
}


ste_adm_res_t dev_subscribe_lpa_mode(dev_lpa_mode_cb_fp_t cb_fp,
                                     int dev_handle,
                                     void* param,
                                     int* subscription_handle_p)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

    ADM_ASSERT(cb_fp);

    int i;
    for (i=0 ; i < MAX_LPA_SUBSCRIPTIONS ; i++) {
        if (!g_lpa_subscriptions[i].cb_fp) {

            g_lpa_subscriptions[i].cb_fp = cb_fp;
            g_lpa_subscriptions[i].dev_handle = dev_handle;
            g_lpa_subscriptions[i].param = param;
            *subscription_handle_p = i + 1;

            dev_notify_lpa_changes(get_dev_by_handle(dev_handle));

            return STE_ADM_RES_OK;
        }
    }

    subscription_handle_p = 0;
    return STE_ADM_RES_ALL_IN_USE;
}

void dev_unsubscribe_lpa_mode(int handle)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    if (handle != 0) {
        ADM_ASSERT(handle > 0 && handle < MAX_LPA_SUBSCRIPTIONS);
        ADM_ASSERT(g_lpa_subscriptions[handle - 1].cb_fp);
        g_lpa_subscriptions[handle - 1].cb_fp = NULL;
    }
}


///////////////////////////////////////////////////////////////////////////////
//
// TTY mode
//
//


ste_adm_tty_mode_t  dev_get_tty_mode()
{
    return g_tty_mode;
}

ste_adm_res_t dev_set_tty_mode(ste_adm_tty_mode_t tty_mode)
{
    ALOG_INFO("dev_set_tty_mode: new mode = %d, current mode = %d\n", tty_mode, g_tty_mode);
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

    int i;
    for (i=0 ; i < ADM_NUM_PHY_DEV ; i++) {
        if (g_dev[i].name[0] != 0 && g_dev[i].graph.voice.ref_cnt != 0) {
            ALOG_ERR("dev_set_tty_mode: Can't enable/disable TTY while voice device is open. Device %s, index %d\n", g_dev[i].name, i);
            return STE_ADM_RES_INCORRECT_STATE;
        }
    }

    ste_adm_res_t res = STE_ADM_RES_OK;

    if (g_tty_mode != tty_mode)
    {
        g_tty_mode = tty_mode;

        device_t *hset_out = get_dev_by_name(STE_ADM_DEVICE_STRING_HSOUT);
        device_t *hset_in = get_dev_by_name(STE_ADM_DEVICE_STRING_HSIN);

        srv_become_worker_thread();

        if (hset_out)
        {
            res = dev_omx_switch_tty_mode_on_app_dev(&(hset_out->graph), g_tty_mode);
        }

        if (hset_in)
        {
            res = dev_omx_switch_tty_mode_on_app_dev(&(hset_in->graph), g_tty_mode);
        }

        srv_become_normal_thread();

        dev_rescan_hw_handler(NULL, FADESPEED_FAST);
    }

    return STE_ADM_RES_OK;
}


///////////////////////////////////////////////////////////////////////////////
//
// Device volumes
//
//

ste_adm_res_t dev_set_volume(const char* name, int vc, int volume, int cap_volume)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

    device_t* real_dev_p;
    ste_adm_res_t res;

	ALOG_STATUS("dev_set_volume: device %s,vc:%d,volume:%d,cap_volume:%d\n", name,vc,volume,cap_volume);

    real_dev_p = get_dev_by_name(name);
    if (!real_dev_p) {
        ALOG_ERR("dev_set_volume: device %s is not open\n", name);
        return STE_ADM_RES_NO_SUCH_DEVICE;
    }

    if (real_dev_p->locked_logical_handle != -1 ) {
        ALOG_INFO("dev_set_volume for %s NOK; logical handle locked (device is closing)\n", name);
        return STE_ADM_RES_NO_SUCH_DEVICE;
    }

    OMX_U32 port = 0;
    il_comp_t comp = dev_omx_get_common_mixer_splitter(&real_dev_p->graph, vc, &port);

    if (NULL == comp)
    {
        ALOG_ERR("dev_set_volume: no common mixer/splitter found\n", name);
        return STE_ADM_RES_INCORRECT_STATE;
    }

    OMX_AUDIO_CONFIG_MUTETYPE mute_cfg;
    IL_TOOL_INIT_CONFIG_STRUCT(mute_cfg);
    mute_cfg.nPortIndex = port;
    mute_cfg.bMute             = (volume == INT_MIN);

    res = il_tool_SetConfig(comp, OMX_IndexConfigAudioMute, &mute_cfg);

    if (res != STE_ADM_RES_OK)
    {
        ALOG_ERR("dev_set_volume: il_tool_SetConfig OMX_IndexConfigAudioMute failed: %d\n", res);
        return res;
    }

    if (volume == INT_MIN)
    {
        // We muted, so no reason to set volume as well
        return STE_ADM_RES_OK;
    }

    OMX_AUDIO_CONFIG_VOLUMETYPE volume_cfg;
    IL_TOOL_INIT_CONFIG_STRUCT(volume_cfg);
    volume_cfg.nPortIndex = port;

    res = il_tool_GetConfig(comp, OMX_IndexConfigAudioVolume, &volume_cfg);

    if (res != STE_ADM_RES_OK)
    {
        ALOG_ERR("dev_set_volume: il_tool_GetConfig OMX_IndexConfigAudioVolume failed: %d\n", res);
        return res;
    }

    volume_cfg.sVolume.nValue = volume;

    if (cap_volume)
    {
        volume_cfg.sVolume.nValue = MAX(volume_cfg.sVolume.nMin, volume_cfg.sVolume.nValue);
        volume_cfg.sVolume.nValue = MIN(volume_cfg.sVolume.nMax, volume_cfg.sVolume.nValue);

        if (volume < volume_cfg.sVolume.nMin ||
            volume > volume_cfg.sVolume.nMax) {
            ALOG_WARN("dev_set_volume: Volume %d mB capped to %d mB\n", volume, volume_cfg.sVolume.nValue);
        }
    }

    res = il_tool_SetConfig(comp, OMX_IndexConfigAudioVolume, &volume_cfg);

    if (res != STE_ADM_RES_OK)
    {
        ALOG_ERR("dev_set_volume: il_tool_SetConfig OMX_IndexConfigAudioVolume failed: %d\n", res);
        return res;
    }

    return STE_ADM_RES_OK;
}

ste_adm_res_t dev_get_volume(const char* name, int vc, int *volume)
{
	ALOG_STATUS("dev_get_volume: device %s,vc:%d\n", name,vc);
	
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

    device_t* real_dev_p;
    ste_adm_res_t res;

    real_dev_p = get_dev_by_name(name);
    if (!real_dev_p) {
        ALOG_ERR("dev_get_volume: device %s is not open\n", name);
        return STE_ADM_RES_NO_SUCH_DEVICE;
    }

    if (real_dev_p->locked_logical_handle != -1 ) {
        ALOG_INFO("dev_get_volume for %s NOK; logical handle locked (device is closing)\n", name);
        return STE_ADM_RES_NO_SUCH_DEVICE;
    }

    OMX_U32 port = 0;
    il_comp_t comp = dev_omx_get_common_mixer_splitter(&real_dev_p->graph, vc, &port);

    if (NULL == comp)
    {
        ALOG_ERR("dev_get_volume: no common mixer/splitter found\n", name);
        return STE_ADM_RES_INCORRECT_STATE;
    }

    OMX_AUDIO_CONFIG_VOLUMETYPE volume_cfg;
    IL_TOOL_INIT_CONFIG_STRUCT(volume_cfg);

    volume_cfg.nPortIndex = port;

    res = il_tool_GetConfig(comp, OMX_IndexConfigAudioVolume, &volume_cfg);

    if (res != STE_ADM_RES_OK)
    {
        ALOG_ERR("dev_set_volume: il_tool_GetConfig failed: %d\n", res);
        return res;
    }

    *volume = volume_cfg.sVolume.nValue ;

    return STE_ADM_RES_OK;
}


///////////////////////////////////////////////////////////////////////////////
//
// Device latency
//
//


ste_adm_res_t dev_get_sink_latency(const char* name, uint32_t *latencyMs)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

    device_t* real_dev_p;
    ste_adm_res_t res;

    real_dev_p = get_dev_by_name(name);
    if (!real_dev_p) {
        ALOG_ERR("dev_get_sink_latency: device %s is not open\n", name);
        return STE_ADM_RES_NO_SUCH_DEVICE;
    }

    if (real_dev_p->locked_logical_handle != -1 ) {
        ALOG_INFO("dev_get_sink_latency for %s NOK; logical handle locked (device is closing)\n", name);
        return STE_ADM_RES_NO_SUCH_DEVICE;
    }

    il_comp_t comp = dev_omx_get_sink(&real_dev_p->graph);

    if (NULL == comp)
    {
        ALOG_ERR("dev_get_sink_latency: no sink found\n", name);
        return STE_ADM_RES_INCORRECT_STATE;
    }

    OMX_AUDIO_CONFIG_LATENCYTYPE latency_cfg;
    IL_TOOL_INIT_CONFIG_STRUCT(latency_cfg);

    res = il_tool_GetConfig(comp, OMX_IndexConfigAudioLatency, &latency_cfg);

    if (res != STE_ADM_RES_OK)
    {
        ALOG_ERR("dev_get_sink_latency: il_tool_GetConfig failed: %d\n", res);
        return res;
    }

    *latencyMs = latency_cfg.nLatency;
    return STE_ADM_RES_OK;
}

static void update_latency_settings(device_t *dev)
{
    ADM_ASSERT(dev != NULL);

    if (dev_omx_is_nonpcm_device(&dev->graph)) {
        return;
    }

    il_comp_t sink = dev_omx_get_sink(&dev->graph);
    il_comp_t app_mixer = dev_omx_get_app_mixer_splitter(&dev->graph);

    if (sink != NULL && app_mixer != NULL) {
        AFM_CONFIG_LOWPOWERTYPE lpa_cfg;
        IL_TOOL_INIT_CONFIG_STRUCT(lpa_cfg);

        ste_adm_res_t res = il_tool_GetConfig(sink, AFM_IndexLowPowerRenderer, &lpa_cfg);

        if (res != STE_ADM_RES_OK) {
            ALOG_ERR("update_latency_settings: il_tool_GetConfig AFM_IndexLowPowerRenderer failed: %d\n", res);
            return;
        }

        ALOG_INFO("update_latency_settings: setting lpa mode to %d for device %s on %X", lpa_cfg.nLowPower, dev->name, (int)app_mixer);

        dev->lpa_mode = lpa_cfg.nLowPower;

        ADM_CONFIG_LATENCYSETTINGTYPE latency_settings;
        IL_TOOL_INIT_CONFIG_STRUCT(latency_settings);
        latency_settings.nPortIndex = MIXER_PORT_OUTPUT;
        if (lpa_cfg.nLowPower) {
            latency_settings.nbrBuffersToUse = 0;
            latency_settings.bufferFillTime = 0;
        }
        else {
            latency_settings.nbrBuffersToUse = 2;
            latency_settings.bufferFillTime = 10;
        }

        res = il_tool_SetConfig(app_mixer, ADM_IndexConfigLatencySettings, &latency_settings);

        if (res != STE_ADM_RES_OK) {
            ALOG_ERR("update_latency_settings: il_tool_SetConfig ADM_IndexConfigLatencySettings failed on %X: %d\n", (int)app_mixer, res);
            return;
        }
    }
    else {
        ALOG_ERR("update_latency_settings: sink or app mixer not found for dev %s", dev->name);
    }
}

///////////////////////////////////////////////////////////////////////////////
//
// PCM probe
//
//
#ifdef ADM_MMPROBE
static void get_pcm_probe_info(int probe_id, pcm_probe_info_t **pcm_probe_info_p)
{
    int i;
    for (i = 0; i < MAX_NBR_OF_PROBE_DEVICES; i++) {
        if (probe_id >= g_pcm_probe[i].base_probe_id &&
            probe_id < g_pcm_probe[i].base_probe_id + MAX_NBR_OF_PROBES_PER_DEVICE) {
            *pcm_probe_info_p = &(g_pcm_probe[i]);
        }
    }
}

static ste_adm_res_t get_pcm_probe_component(int probe_id, il_comp_t *comp_p, OMX_U32 *port_p, int *mixer_port, int *effect_position)
{
    ste_adm_res_t res = STE_ADM_RES_OK;
    ADM_ASSERT(comp_p);
    ADM_ASSERT(port_p);

    pcm_probe_info_t *pcm_probe_info_p = NULL;
    get_pcm_probe_info(probe_id, &pcm_probe_info_p);

    *mixer_port = -1;
    *effect_position = -1;

    if (pcm_probe_info_p) {
        device_t* dev_p;
        dev_p = get_dev_by_name(pcm_probe_info_p->device_name);
        if (!dev_p) {
            ALOG_INFO("get_pcm_probe_component: device %s is not open\n", pcm_probe_info_p->device_name);
            res = STE_ADM_RES_NO_SUCH_DEVICE;
        } else {
            int probe_offset = probe_id - pcm_probe_info_p->base_probe_id;
            if (probe_offset < PCM_PROBE_COMMON_EFFECT) {
                /* IO (source/sink) probe */
                ALOG_INFO("get_pcm_probe_component: id=%d (%s)", probe_id, probe_comp2str(PCM_PROBE_IO));
                *comp_p = dev_p->graph.common.io_handle;
                *port_p = probe_offset - PCM_PROBE_IO;
            } else if (probe_offset < PCM_PROBE_COMMON_MIX_SPLIT) {
                /* Common effect probe */
                ALOG_INFO("get_pcm_probe_component: id=%d (%s, effect_index=%d)", probe_id, probe_comp2str(PCM_PROBE_COMMON_EFFECT), probe_offset-PCM_PROBE_COMMON_EFFECT);
                if (probe_offset-PCM_PROBE_COMMON_EFFECT < (int)dev_p->graph.common.nbr_of_comp + (int)dev_p->graph.common.nbr_mixer_effects) {
                    if (probe_offset-PCM_PROBE_COMMON_EFFECT >= (int)dev_p->graph.common.nbr_mixer_effects) {
                        /* Effect component */
                        *comp_p = dev_p->graph.common.comp_handles[probe_offset-PCM_PROBE_COMMON_EFFECT-dev_p->graph.common.nbr_mixer_effects];
                    }
                    else {
                        /* Mixer effect */
                        *comp_p = dev_p->graph.common.mixer_splitter_handle;
                        *mixer_port = MIXER_PORT_OUTPUT;
                        *effect_position = probe_offset-PCM_PROBE_COMMON_EFFECT + 1;
                    }
                    *port_p = EFFECT_PORT_OUTPUT;
                } else {
                    ALOG_ERR("get_pcm_probe_component: id=%d is not valid. (Nbr of common effects = %u)", probe_id, dev_p->graph.common.nbr_of_comp + dev_p->graph.common.nbr_mixer_effects);
                    res = STE_ADM_RES_INVALID_PARAMETER;
                }
            } else if (probe_offset < PCM_PROBE_VOICE_EFFECT) {
                /* Common mixer/splitter probe */
                ALOG_INFO("get_pcm_probe_component: id=%d (%s, port=%d)", probe_id, probe_comp2str(PCM_PROBE_COMMON_MIX_SPLIT), probe_offset-PCM_PROBE_COMMON_MIX_SPLIT);
                *comp_p = dev_p->graph.common.mixer_splitter_handle;
                *port_p = probe_offset-PCM_PROBE_COMMON_MIX_SPLIT;
            } else if (probe_offset < PCM_PROBE_APP_EFFECT) {
                /* Voice effect probe */
                if (dev_p->graph.voice.ref_cnt == 0) {
                    ALOG_INFO("get_pcm_probe_component: voice device %s is not open\n", pcm_probe_info_p->device_name);
                    res = STE_ADM_RES_NO_SUCH_DEVICE;
                }
                else {
                    ALOG_INFO("get_pcm_probe_component: id=%d (%s, effect_index=%d)", probe_id, probe_comp2str(PCM_PROBE_VOICE_EFFECT), probe_offset-PCM_PROBE_VOICE_EFFECT);
                    if (probe_offset-PCM_PROBE_VOICE_EFFECT < (int)dev_p->graph.voice.nbr_of_comp + (int)dev_p->graph.voice.nbr_mixer_effects) {
                        if (probe_offset-PCM_PROBE_VOICE_EFFECT < (int)dev_p->graph.voice.nbr_of_comp) {
                            /* Effect component */
                            *comp_p = dev_p->graph.voice.comp_handles[probe_offset-PCM_PROBE_VOICE_EFFECT];
                        }
                        else {
                            /* Mixer effect */
                            *comp_p = dev_p->graph.common.mixer_splitter_handle;
                            *mixer_port = MIXER_PORT_VOICE_INPUT;
                            *effect_position = probe_offset-PCM_PROBE_VOICE_EFFECT + 1 - dev_p->graph.voice.nbr_of_comp;
                        }
                        *port_p = EFFECT_PORT_OUTPUT;
                    } else {
                        ALOG_ERR("get_pcm_probe_component: id=%d is not valid. (Nbr of voice effects = %u)", probe_id, dev_p->graph.voice.nbr_of_comp + dev_p->graph.voice.nbr_mixer_effects);
                        res = STE_ADM_RES_INVALID_PARAMETER;
                    }
                }
            } else if (probe_offset < PCM_PROBE_APP_MIX_SPLIT) {
                /* Application effect probe */
                if (dev_p->graph.app.ref_cnt == 0) {
                    ALOG_INFO("get_pcm_probe_component: app device %s is not open\n", pcm_probe_info_p->device_name);
                    res = STE_ADM_RES_NO_SUCH_DEVICE;
                }
                else {
                    ALOG_INFO("get_pcm_probe_component: id=%d (%s, effect_index=%d)", probe_id, probe_comp2str(PCM_PROBE_APP_EFFECT), probe_offset-PCM_PROBE_APP_EFFECT);
                    if (probe_offset-PCM_PROBE_APP_EFFECT < (int)dev_p->graph.app.nbr_of_comp + (int)dev_p->graph.app.nbr_mixer_effects) {
                        if (probe_offset-PCM_PROBE_APP_EFFECT < (int)dev_p->graph.app.nbr_of_comp) {
                            /* Effect component */
                            *comp_p = dev_p->graph.app.comp_handles[probe_offset-PCM_PROBE_APP_EFFECT];
                        }
                        else {
                            /* Mixer effect */
                            *comp_p = dev_p->graph.common.mixer_splitter_handle;
                            *mixer_port = MIXER_PORT_APP_INPUT;
                            *effect_position = probe_offset-PCM_PROBE_APP_EFFECT + 1 - dev_p->graph.app.nbr_of_comp;
                        }
                        *port_p = EFFECT_PORT_OUTPUT;
                    } else {
                        ALOG_ERR("get_pcm_probe_component: id=%d is not valid. (Nbr of app effects = %u)", probe_id, dev_p->graph.app.nbr_of_comp + dev_p->graph.app.nbr_mixer_effects);
                        res = STE_ADM_RES_INVALID_PARAMETER;
                    }
                }
            } else if (probe_offset < MAX_NBR_OF_PROBES_PER_DEVICE) {
                /* Application mixer/splitter probe */
                if (dev_p->graph.app.ref_cnt == 0) {
                    ALOG_INFO("get_pcm_probe_component: app device %s is not open\n", pcm_probe_info_p->device_name);
                    res = STE_ADM_RES_NO_SUCH_DEVICE;
                }
                else {
                    ALOG_INFO("get_pcm_probe_component: id=%d (%s, port=%d)", probe_id, probe_comp2str(PCM_PROBE_APP_MIX_SPLIT), probe_offset-PCM_PROBE_APP_MIX_SPLIT);
                    *comp_p = dev_p->graph.app.mixer_splitter_handle;
                    *port_p = probe_offset-PCM_PROBE_APP_MIX_SPLIT;
                }
            } else {
                ALOG_ERR("get_pcm_probe_component: id=%d - out of range");
                res = STE_ADM_RES_INVALID_PARAMETER;
            }
        }
    } else {
        ALOG_ERR("get_pcm_probe_component: Unknown probe id = %d", probe_id);
        res = STE_ADM_RES_INVALID_PARAMETER;
    }

    return res;
}

/**
* dev_get_pcm_probe
*
* device_name     - top level device name
* pcm_probe_comp  - probe component in the omx graph
* index           - specifying the effect position (starting at 0) when
*                   pcm_probe_comp is an effect, otherwise specifying
*                   the port index of pcm_probe_comp.
*
* probe_id        - out parameter specifying probe id.
* probe_enabled   - out parameter specifying probe status (enabled/disabled)
*/
ste_adm_res_t dev_get_pcm_probe(
    const char* device_name, pcm_probe_comp_t pcm_probe_comp, int index,
    int* probe_id, int* probe_enabled)
{
    ste_adm_res_t res = STE_ADM_RES_OK;
    ADM_ASSERT(probe_id);
    ADM_ASSERT(probe_enabled);

    pcm_probe_info_t *pcm_probe_info_p = NULL;

    int i;
    for (i = 0; i < MAX_NBR_OF_PROBE_DEVICES; i++) {
        if (strcmp(device_name, g_pcm_probe[i].device_name) == 0) {
            pcm_probe_info_p = &(g_pcm_probe[i]);
            break;
        }
    }

    if (pcm_probe_info_p) {
        *probe_id = pcm_probe_info_p->base_probe_id + pcm_probe_comp + index;
        *probe_enabled = pcm_probe_info_p->pcm_probe_state[pcm_probe_comp + index];
    } else {
        ALOG_ERR("dev_get_pcm_probe failed for %s, pcm_probe_comp=%s, "
            "index=%d", device_name, probe_comp2str(pcm_probe_comp), index);
        res = STE_ADM_RES_NO_SUCH_DEVICE;
    }

    ALOG_INFO_VERBOSE("dev_get_pcm_probe: "
        "%s, pcm_probe_comp=%s, index=%d, probe_id=%d, probe_enabled=%d",
        device_name, probe_comp2str(pcm_probe_comp), index, *probe_id, *probe_enabled);

    return res;
}

ste_adm_res_t dev_set_pcm_probe(int probe_id, int enabled)
{
    ste_adm_res_t res = STE_ADM_RES_OK;
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

    pcm_probe_info_t *pcm_probe_info_p = NULL;
    get_pcm_probe_info(probe_id, &pcm_probe_info_p);

    ALOG_INFO("dev_set_pcm_probe: id=%d, enabled=%d", probe_id, enabled);

    if (pcm_probe_info_p) {
        if (pcm_probe_info_p->pcm_probe_state[probe_id - pcm_probe_info_p->base_probe_id] != enabled) {
            /* Update probe state in probe list */
            pcm_probe_info_p->pcm_probe_state[probe_id - pcm_probe_info_p->base_probe_id] = enabled;

            il_comp_t comp_h;
            OMX_U32   port_index;
            int mixer_port;
            int effect_position;

            /* Check if device corresponding to probe_id is opened */
            res = get_pcm_probe_component(probe_id, &comp_h, &port_index, &mixer_port, &effect_position);
            if (res == STE_ADM_RES_NO_SUCH_DEVICE) {
                /* Device corresponding to the probe id is not opened */
                res = STE_ADM_RES_OK;
            } else if (res != STE_ADM_RES_OK) {
                ALOG_ERR("dev_set_pcm_probe failed for probe_id = %d", probe_id);
            } else {
                /* Update probe state in opened device */
                ALOG_INFO("dev_set_pcm_probe: %s (id=%d) is opened, update probe state", pcm_probe_info_p->device_name, probe_id);
                AFM_AUDIO_CONFIG_PCM_PROBE pcm_probe_cfg;
                IL_TOOL_INIT_CONFIG_STRUCT(pcm_probe_cfg);
                pcm_probe_cfg.nPortIndex = port_index;
                pcm_probe_cfg.nProbeIdx = probe_id;
                pcm_probe_cfg.bEnable = enabled;

                if (mixer_port == -1) {
                    if (il_tool_SetConfig(comp_h, AFM_IndexConfigPcmProbe, &pcm_probe_cfg) != STE_ADM_RES_OK) {
                        ALOG_WARN("dev_set_pcm_probe: AFM_IndexConfigPcmProbe not supported by comp=%p (%s), port=%u",
                            il_tool_raw_handle(comp_h), pcm_probe_info_p->device_name, port_index);
                    }
                }
                else {
                    if (il_tool_setConfig_mixer_effect(comp_h ,effect_position, mixer_port, AFM_IndexConfigPcmProbe, &pcm_probe_cfg) != STE_ADM_RES_OK) {
                        ALOG_WARN("dev_set_pcm_probe: AFM_IndexConfigPcmProbe not supported by comp=%p (mixer_port=%d, port_index=%d, effect_index=%d)",
                                il_tool_raw_handle(comp_h), mixer_port, EFFECT_PORT_OUTPUT, effect_position);
                    }
                    if (il_tool_commit_mixer_effects(comp_h, mixer_port) != STE_ADM_RES_OK) {
                        ALOG_WARN("dev_set_pcm_probe: Failed to commit AFM_IndexConfigPcmProbe on comp=%p (%s, port_index=%d, effect_index=%d)",
                                il_tool_raw_handle(comp_h), mixer_port, EFFECT_PORT_OUTPUT, effect_position);
                    }
                }
            }
        } else {
            ALOG_INFO("dev_set_pcm_probe: id = %d is already %s, do nothing",
                probe_id, enabled ? "enabled" : "disabled");
        }
    } else {
        ALOG_ERR("dev_set_pcm_probe: Unknown id = %d", probe_id);
        res = STE_ADM_RES_INVALID_PARAMETER;
    }

    return res;
}
#endif // ADM_MMPROBE
//
//
// Hardware handler support
//
//

static int g_hw_handler_busy = 0;
static int g_hw_handler_iter_pos;
static int g_hw_handler_iter_pos_d2d;
static const char* g_hw_handler_new_dev;

// Callback from hardware handler, used to enumerate devices.
static int hw_handler_dev_next(const char** dev_name_pp)
{
    SRV_DBG_ASSERT_IS_WORKER_THREAD;
    SRV_DBG_ASSERT_IS_SERIALIZED_REQUEST;

    ADM_ASSERT(g_hw_handler_busy);

    while (g_hw_handler_iter_pos < ADM_NUM_PHY_DEV) {
        if (g_dev[g_hw_handler_iter_pos].name[0]) {
            if ((g_tty_mode == STE_ADM_TTY_MODE_FULL ||  g_tty_mode == STE_ADM_TTY_MODE_VCO) &&
                strcmp(g_dev[g_hw_handler_iter_pos].name, STE_ADM_DEVICE_STRING_HSOUT) == 0) {
                *dev_name_pp = STE_ADM_DEVICE_STRING_TTYOUT;
                g_hw_handler_iter_pos++;
            }
            else if ((g_tty_mode == STE_ADM_TTY_MODE_FULL ||  g_tty_mode == STE_ADM_TTY_MODE_HCO) &&
                     strcmp(g_dev[g_hw_handler_iter_pos].name, STE_ADM_DEVICE_STRING_HSIN) == 0) {
                     *dev_name_pp = STE_ADM_DEVICE_STRING_TTYIN;
                     g_hw_handler_iter_pos++;
            }
            else {
                *dev_name_pp = g_dev[g_hw_handler_iter_pos++].name;
            }

            return 0; // entry valid
        }
        g_hw_handler_iter_pos++;
    }

    if (g_hw_handler_iter_pos == ADM_NUM_PHY_DEV) {
        if (g_hw_handler_new_dev) {
            if ((g_tty_mode == STE_ADM_TTY_MODE_FULL ||  g_tty_mode == STE_ADM_TTY_MODE_VCO) &&
                strcmp(g_hw_handler_new_dev, STE_ADM_DEVICE_STRING_HSOUT) == 0) {
                *dev_name_pp = STE_ADM_DEVICE_STRING_TTYOUT;
            }
            else if ((g_tty_mode == STE_ADM_TTY_MODE_FULL ||  g_tty_mode == STE_ADM_TTY_MODE_HCO) &&
                     strcmp(g_hw_handler_new_dev, STE_ADM_DEVICE_STRING_HSIN) == 0) {
                     *dev_name_pp = STE_ADM_DEVICE_STRING_TTYIN;
            }
            else {
                *dev_name_pp = g_hw_handler_new_dev;
            }

            g_hw_handler_iter_pos++;
            return 0; // entry valid
        }
    }

    *dev_name_pp = NULL;
    g_hw_handler_iter_pos = 0;
    ALOG_INFO("hw_handler_dev_next: DONE");
    return 1; // No more entries
}

static int hw_handler_dev_next_d2d(const char** src_dev_name_pp, const char** dst_dev_name_pp)
{
    SRV_DBG_ASSERT_IS_WORKER_THREAD;
    SRV_DBG_ASSERT_IS_SERIALIZED_REQUEST;

    ADM_ASSERT(g_hw_handler_busy);

    while (g_hw_handler_iter_pos_d2d < ADM_NUM_PHY_DEV) {
        if (g_dev_hw[g_hw_handler_iter_pos_d2d].src[0] && g_dev_hw[g_hw_handler_iter_pos_d2d].dst[0]) {
            *src_dev_name_pp = g_dev_hw[g_hw_handler_iter_pos_d2d].src;
            *dst_dev_name_pp = g_dev_hw[g_hw_handler_iter_pos_d2d++].dst;
            return 0; // entry valid
        }
        g_hw_handler_iter_pos_d2d++;
    }

    *src_dev_name_pp = NULL;
    *dst_dev_name_pp = NULL;
    g_hw_handler_iter_pos_d2d = 0;
    ALOG_INFO("hw_handler_dev_next_d2d: DONE");
    return 1; // No more entries
}

ste_adm_res_t dev_rescan_hw_handler(const char* new_dev, fadeSpeed_t fadeSpeed)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    SRV_DBG_ASSERT_IS_SERIALIZED_REQUEST;

    ALOG_INFO("dev_rescan_hw_handler\n");

    g_hw_handler_new_dev = new_dev;
    ADM_ASSERT(!g_hw_handler_busy);
    g_hw_handler_iter_pos = 0;
    g_hw_handler_busy = 1;

    sqlite3* db_h;
    srv_become_worker_thread();
    adm_db_get_handle(&db_h);

#if defined(ADM_DBG_X86)
    ste_adm_res_t res = ste_adm_hw_handler(db_h, hw_handler_dev_next, hw_handler_dev_next_d2d, fadeSpeed);
#else
    ste_adm_res_t res = Alsactrl_Hwh(db_h, hw_handler_dev_next, hw_handler_dev_next_d2d, fadeSpeed);
#endif
    srv_become_normal_thread();

    g_hw_handler_new_dev = NULL;
    g_hw_handler_busy = 0;
    return res;
}

ste_adm_res_t dev_rescan_hw_handler_d2d()
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    SRV_DBG_ASSERT_IS_SERIALIZED_REQUEST;

    ALOG_INFO("%s: Enter\n", __func__);

    ADM_ASSERT(!g_hw_handler_busy);
    g_hw_handler_iter_pos_d2d = 0;
    g_hw_handler_busy = 1;

    sqlite3* db_h;
    srv_become_worker_thread();
    adm_db_get_handle(&db_h);
#if defined(ADM_DBG_X86)
    ste_adm_res_t res = ste_adm_hw_handler(db_h, hw_handler_dev_next, hw_handler_dev_next_d2d, FADESPEED_SLOW);
#else
    ste_adm_res_t res = Alsactrl_Hwh(db_h, hw_handler_dev_next, hw_handler_dev_next_d2d, FADESPEED_SLOW);
#endif
    srv_become_normal_thread();

    g_hw_handler_busy = 0;

    return res;
}

ste_adm_res_t dev_open_d2d(const char* src_dev, const char* dst_dev, int* dev_handle)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    int i;
    ALOG_INFO("%s: Enter\n", __func__);
    if(src_dev == NULL || dst_dev == NULL)
    {
       ALOG_ERR("%s: dev_open_d2d ERROR src or dst cant be null\n", __func__);
       return STE_ADM_RES_INVALID_PARAMETER;
    }
    for (i=0 ; i < ADM_NUM_PHY_DEV ; i++){
        if (g_dev_hw[i].src[0] == 0 && g_dev_hw[i].dst[0] == 0) {
            strcpy(g_dev_hw[i].src, src_dev);
            strcpy(g_dev_hw[i].dst, dst_dev);
            ADM_ASSERT(g_dev_hw[i].src[0] != 0 || g_dev_hw[i].dst[0] != 0 );
            *dev_handle = i;
            return dev_rescan_hw_handler_d2d();
        }
    }
    return STE_ADM_RES_DEV_LIMIT_REACHED;
}

ste_adm_res_t dev_close_d2d(int* dev_handle)
{
    if(dev_handle == NULL){
        ALOG_ERR("%s: dev_handle equals NULL !!\n", __func__);
        return STE_ADM_RES_INVALID_PARAMETER;
    }

    g_dev_hw[*dev_handle].src[0] = 0;
    g_dev_hw[*dev_handle].dst[0] = 0;
    *dev_handle = 0;
    return dev_rescan_hw_handler_d2d();
}
