/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#define ADM_LOG_FILENAME "api_cscall"

#include "ste_adm_dbg.h"
#include "ste_adm_cscall_omx.h"
#include "ste_adm_dev.h"
#include "ste_adm_srv.h"
#include "ste_adm_omx_tool.h"
#include "ste_adm_msg.h"
#include "ste_adm_dev_omx_util.h"
#include "ste_adm_client.h"
#include "ste_adm_db.h"
#include "ste_adm_api_cscall.h"
#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include "ste_adm_platform_adaptions.h"
#include "ste_adm_api_modem.h"
#include "ste_adm_util.h"

#if defined(ADM_DBG_X86)
#include "ste_adm_hw_handler.h"
#include "audio_hwctrl_interface_alsa.h"
#else
#include "alsactrl_hwh.h"
#include "alsactrl_alsa.h"
#endif


#ifndef ADM_DBG_X86
  #include "cutils/properties.h"
#else
  #define PROPERTY_VALUE_MAX 96
  #include <stdlib.h>
#endif

#define STE_ADM_CSCALL_MAX_VOIP_SAMPLERATE (48000)
#define STE_ADM_CSCALL_MAX_VOIP_CHANNELS (2)

/*Device identifier string for Modem Downlink */
#define STE_ADM_DEVICE_STRING_MODEMDL "MODEM_DL"
/*Device identifier string for Modem Uplink */
#define STE_ADM_DEVICE_STRING_MODEMUL "MODEM_UL"

typedef enum
{
  CSCALL_STATE_INVALID,
  CSCALL_STATE_DISABLED,
  CSCALL_STATE_DISCONNECTED,
  CSCALL_STATE_DISCONNECTING,
  CSCALL_STATE_CONNECTED,
  CSCALL_STATE_RETRY,
  CSCALL_STATE_ERROR_FATAL
} cscall_state_t;

// Separate vc volume and mute settings are needed at ADM level
// since two different higher layers in Android that are unaware
// of each other set mute and volume independently...

// Voicecall upstream mute. Applied on the splitter port at
// the beginning of the VC chain. This setting should survive when
// the voicecall device is closed, so it needs to be saved.
static int g_vc_upstream_mute   = 0;

// Voicecall downstream mute. Applied on the mixer port at
// the end of the VC chain. This setting should survive when
// the voicecall device is closed, so it needs to be saved.
static int g_vc_downstream_mute = 0;

// Voicecall upstream volume. Applied on the splitter port at
// the beginning of the VC chain. This setting should survive when
// the voicecall device is closed, so it needs to be saved.
static int g_vc_upstream_volume      = 0;

// Voicecall upstream volume. Applied on the splitter port at
// the beginning of the VC chain. This setting should survive when
// the voicecall device is closed, so it needs to be saved.
static int g_vc_downstream_volume    = 0;
static int g_vc_downstream_min_volume    = 0;
static int g_vc_downstream_max_volume    = 1;

static char g_cur_indev[STE_ADM_MAX_DEVICE_NAME_LENGTH+1];
static char g_cur_outdev[STE_ADM_MAX_DEVICE_NAME_LENGTH+1];
static int g_freq_known = 0; // 0: sample rate unknown, != 0 sample rate known
static int g_codec_enabled = 0;
static int g_loopback_mode = 0;
static cscall_state_t g_state = CSCALL_STATE_INVALID;
static int g_in_dev_handle = 0;
static int g_dict_play_dev_handle = 0;
static int g_out_dev_handle = 0;
static int g_dict_play_active = 0;
static int g_dict_rec_active = 0;
static int g_voip_active = 0;

static ste_adm_res_t connect_graph();
static ste_adm_res_t connect_omx_graph();
static ste_adm_res_t connect_d2d();
static ste_adm_res_t disconnect_graph();
static ste_adm_res_t disconnect_omx_graph();
static ste_adm_res_t disconnect_d2d();
static void reroute_live(const char* new_indev, const char* new_outdev, int indev_changed, int outdev_changed);
static void codec_enabled();
static void codec_disabled();
static void samplerate_updated();
static int rescan();
static void disconnect_dict_rec();
static ste_adm_res_t connect_dict_rec();
static ste_adm_res_t connect_dict_play(const char* outdev, int only_app_part);
static ste_adm_res_t disconnect_dict_play(int only_app_part);
static ste_adm_res_t csapi_apply_vc_up_vol_mute();
static ste_adm_res_t csapi_apply_vc_down_vol_mute();
static ste_adm_res_t csapi_apply_vc_vol_mute();
static void voip_out_io_idle_callback(void* param);

static ste_adm_tty_mode_t g_tty_mode = 0;

static int g_dict_rec_num_bufs;
static int g_dict_rec_bufsz;
static int g_dict_rec_format;
static int g_dict_rec_samplerate;
static int g_dict_play_num_bufs;
static int g_dict_play_bufsz;
static int g_dict_play_num_ch;
static int g_dict_play_sr;

static int is_graph_using_d2d(const char* indev,const char * outdev)
{
    // D2D connection is only used in case of FULL_FAT_MODEM configuration
    // except in following cases (standard graph creation) :
    // - VoIP context
    // - Usage of BT devices
    if (adm_get_modem_type()!=ADM_FULL_FAT_MODEM) return 0;
    if (g_voip_active) return 0;

    // Config FULL_FAT and VoIP not requested :
    if (!strcmp(indev,STE_ADM_DEVICE_STRING_BTIN) ||
        !strcmp(indev,STE_ADM_DEVICE_STRING_BTWBIN) ||
        !strcmp(outdev,STE_ADM_DEVICE_STRING_BTOUT) ||
        !strcmp(outdev,STE_ADM_DEVICE_STRING_BTWBOUT) ) {
            return 0;
    }

    return 1;
}
int csapi_connection_conditions(void)
{
    switch(adm_get_modem_type()) {
        //   For Slim or Half Slim Modem , conditions MUST be :
        //     - Samplerate known (comming froom cscall component)
        //     - Codec Enable (comming froom cscall component)
        //     - DeviceIn DeviceOut Connection request (comming froom application)

        case ADM_SLIM_MODEM:
        case ADM_HALF_SLIM_MODEM:
            return g_cur_indev[0] && ((g_freq_known && g_codec_enabled) || g_voip_active);


        //   For Fat Modem , conditions MUST be :
        //     - DeviceIn DeviceOut Connection request (comming froom application)

        case ADM_FAT_MODEM:
        case ADM_FULL_FAT_MODEM:
            return (g_cur_indev[0]);
        default :
            ALOG_ERR("Bad Modem Type");
            return 0;
    }
    return 0;
}

struct dict_session_s
{
    int is_finished;

    // Will be false after for openDevice -> CloseDevice sequence.
    // (CloseDevice keeps the session open()
    int device_open;

    msg_base_t* drain_msg_p;
    // g_state == CSCALL_STATE_DISCONNECTED, is_finished = 0  --> waiting for cscall
    // g_state == CSCALL_STATE_CONNECTED,    is_finished = 0  --> dict session active
    // g_state == CSCALL_STATE_DISCONNECTED, is_finished = 1  --> old session
    // g_state == CSCALL_STATE_CONNECTED,    is_finished = 1  --> old session, not actiev
    int busy;
    int padding[4];
};

struct voip_session_s
{
    int samplerate;
    int channels;
    msg_base_t* drain_msg_p;
    int busy;
};

static struct dict_session_s  g_dict_play_session;
static struct dict_session_s  g_dict_rec_session;
static struct voip_session_s  g_voip_in_session;
static struct voip_session_s  g_voip_out_session;


#ifndef ADM_EXCLUDE_CSCALL
static struct srv_session_s   g_setdevices_session;
#endif


static const char* cscall_state2str(cscall_state_t state)
{
    switch (state) {
        case CSCALL_STATE_INVALID:       return "CSCALL_STATE_INVALID";
        case CSCALL_STATE_DISABLED:      return "CSCALL_STATE_DISABLED";
        case CSCALL_STATE_DISCONNECTED:  return "CSCALL_STATE_DISCONNECTED";
        case CSCALL_STATE_DISCONNECTING: return "CSCALL_STATE_DISCONNECTING";
        case CSCALL_STATE_CONNECTED:     return "CSCALL_STATE_CONNECTED";
        case CSCALL_STATE_RETRY:         return "CSCALL_STATE_RETRY";
        case CSCALL_STATE_ERROR_FATAL:   return "CSCALL_STATE_ERROR_FATAL";
        default:                         return "<unknown>";
    }
}

//
//  rescan() function investigates the environment, and updates the state
//  to become inline with the environment (open/close voicecall,
//  connect/disconnect dictaphone devices). Call rescan() whenever the
//  environment has changed.
//
//  Environment (set by user or modem events):
//     g_freq_known                               VC sample rate known (if != 0)
//     g_codec_enabled                            VC codec active (if != 0)
//     g_cur_indev[0]                             VC requested by user (if != 0)
//     g_dict_rec_session.srv_session.dtor_fp     Dict rec session connected to ADM (if != NULL)
//
//  State (set by rescan() based on environment):
//     g_state                                    Voicecall state (conected or not connected)
//     g_dict_rec_active                          Dictaphone record graph active
//
//  Combined environment and state:
//     g_dict_rec_session.is_finished             Dict session finished since VC gone.
//                                                - Set by rescan() when closing VC graph
//                                                - Cleared by user starting a new dictaphone session
//
//
//  Voicecall graph becomes connected when all the following is true:
//  - VC sample rate is known
//  - VC codec active
//  - VC requested by user
//
//  Voicecall graph is torn down when any of the above crierias is no longer met.
//
//
//  Dictaphone playback is connected when the following criterias are met:
//  - VC graph connected
//  - Dict rec session connected to ADM
//  - Dict rec session is not in state finished
//
// Dictaphone playback is torn down when any of the above crierias is no longer met.
//
// Dictaphone recording is controlled analogous to dictaphone playback.
//
// While the graph is being torn down, g_state is CSCALL_STATE_DISCONNECTING.
//
void csapi_init()
{
#ifdef ADM_EXCLUDE_CSCALL
    ALOG_INFO("Disable CSCALL functionality in ADM");
    g_state = CSCALL_STATE_DISABLED;
#else
    g_cur_indev[0]  = 0;
    g_cur_outdev[0] = 0;

    g_loopback_mode = 0;

    memset(&g_dict_play_session, 0, sizeof(g_dict_play_session));
    memset(&g_dict_rec_session,  0, sizeof(g_dict_rec_session));
    memset(&g_voip_in_session,  0, sizeof(g_voip_in_session));
    memset(&g_voip_out_session,  0, sizeof(g_voip_out_session));
    g_setdevices_session.dtor_fp = NULL;


    ste_adm_res_t res = adm_cscall_omx_init_cscall(codec_enabled,
                                                   codec_disabled,
                                                   samplerate_updated);

    if (res != STE_ADM_RES_OK) {
        ALOG_ERR("CSCall init failed --> state CSCALL_STATE_DISABLED. Modem problems?\n");
        g_state = CSCALL_STATE_DISABLED;
    } else {
        ALOG_INFO("CSCall init ok, in state CSCALL_STATE_DISCONNECTED\n");
        g_state = CSCALL_STATE_DISCONNECTED;
    }
#endif
}


// Destroys cscall state. Currently this is only supported for debugging
// (to be able to run Valgrind), and is only expected to work if the cscall
// graph is already torn down.
void csapi_destroy()
{
    adm_cscall_omx_destroy_cscall();
}

// The TTY API (an 'API filter' at end of this file) sits above this call.
static ste_adm_res_t csapi_set_devices_internal(const char* indev, const char* outdev, srv_session_t** client_pp)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    SRV_DBG_ASSERT_IS_SERIALIZED_REQUEST;

    (void) client_pp;

    // Either both in and out active, or neither in nor out active
    ADM_ASSERT( (indev[0] && outdev[0]) || (!indev[0] && !outdev[0]) );

    if (g_state == CSCALL_STATE_ERROR_FATAL) {
        ALOG_ERR("SetDevices: CSCALL_STATE_ERROR_FATAL due to earlier, unrecoverable error\n");
        return STE_ADM_RES_UNRECOVERABLE_ERROR;
    }

    // Check what devices are changed; do not touch those that have
    // not changed.
    int indev_changed  = 0;
    int outdev_changed = 0;
    if (indev[0] && strcmp(g_cur_indev, indev)) {
        indev_changed = 1;
    }
    if (outdev[0] && strcmp(g_cur_outdev, outdev)) {
        outdev_changed = 1;
    }

    if ((indev_changed || outdev_changed) && g_state == CSCALL_STATE_CONNECTED) {
        ALOG_STATUS("CSCall SetDevices: [%s,%s] Live cscall reroute from [%s,%s]\n", g_cur_indev, g_cur_outdev, indev, outdev);
        reroute_live(indev, outdev, indev_changed, outdev_changed);
    } else if (g_state == CSCALL_STATE_CONNECTED && !indev[0]) {
        ALOG_STATUS("CSCall SetDevices: [NULL, NULL] Disconnect requested (might wait for events from modem)\n");
    } else if (g_state != CSCALL_STATE_CONNECTED && indev[0]) {
        ALOG_STATUS("CSCall SetDevices: [%s,%s] (currently not connected)\n", indev, outdev);
    } else {
        ALOG_STATUS("CSCall SetDevices: [%s,%s] %s", indev, outdev, cscall_state2str(g_state));
    }

    // Caller is responsible for validating lengths
    ADM_ASSERT(strlen(indev) <= STE_ADM_MAX_DEVICE_NAME_LENGTH);
    ADM_ASSERT(strlen(outdev) <= STE_ADM_MAX_DEVICE_NAME_LENGTH);

    strcpy(g_cur_indev, indev);
    strcpy(g_cur_outdev, outdev);

    // This will do nothing if call is already up, but the devices
    // have been re-routed. No difference is detected.
    while(rescan());

    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

    // Since time of actual voice call graph construcion / teardown also is
    // controlled by modem events and thus asynchronous, there isn't much to do
    // than to return OK. The graph changes may happen later.
    return STE_ADM_RES_OK;
}

static void codec_enabled()
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    SRV_DBG_ASSERT_IS_SERIALIZED_REQUEST;
    g_codec_enabled = 1;
    while(rescan());

    // update modem vc state
    if (g_freq_known != 0)
        csapi_notify_vc_state_change();
}

static void codec_disabled()
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    SRV_DBG_ASSERT_IS_SERIALIZED_REQUEST;
    g_codec_enabled = 0;
    g_freq_known    = 0;
    while(rescan());

    // update modem vc state
    csapi_notify_vc_state_change();
}

static void samplerate_updated()
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    SRV_DBG_ASSERT_IS_SERIALIZED_REQUEST;
    g_freq_known = 1;
    while(rescan());

    // update modem vc state
    if (g_codec_enabled != 0)
        csapi_notify_vc_state_change();
}

static int rescan_handle_state_connected()
{
    ADM_ASSERT(g_state == CSCALL_STATE_CONNECTED);

    //
    // Check if the user or the modem has decided that the graph should be closed.
    //
    if (!csapi_connection_conditions()){

        // We will become a worker thread. Unserialized requests may start
        // arriving. Make sure they do not touch the state.
        g_state = CSCALL_STATE_DISCONNECTING;

        if (g_dict_rec_active) {
            ADM_ASSERT(g_dict_rec_session.busy);
            ADM_ASSERT(g_dict_rec_session.device_open);
            ADM_ASSERT(!g_dict_rec_session.is_finished);
            g_dict_rec_active = 0;
            disconnect_dict_rec();
            g_dict_rec_session.is_finished = 1;
        }

        if (g_dict_play_active) {
            ADM_ASSERT(g_dict_play_session.busy);
            ADM_ASSERT(g_dict_play_session.device_open);
            ADM_ASSERT(!g_dict_play_session.is_finished);
            g_dict_play_active = 0;
            disconnect_dict_play(0);
            g_dict_play_session.is_finished = 1;
        }

        ste_adm_res_t res = disconnect_graph();
        if (res != STE_ADM_RES_OK) {
            g_state = CSCALL_STATE_ERROR_FATAL;
            ALOG_STATUS("csapi_rescan disconnect_graph() -> CSCALL_STATE_ERROR_FATAL\n");
        }

        if (g_voip_active) {
            if (!g_voip_in_session.busy ||
                !g_voip_out_session.busy) {
                g_voip_active = 0;
                adm_cscall_omx_disable_voip();
            }
        }
        g_state = CSCALL_STATE_DISCONNECTED;
        ALOG_STATUS("CSCALL_STATE_DISCONNECTED\n");
        return 1;
    }

    //
    //  Is a new dictaphone record session wanted?
    //
    if (g_dict_rec_session.device_open && !g_dict_rec_session.is_finished) {
        // We are connected, we have a dictaphone session that wants to start
        if (!g_dict_rec_active) {
            // ..and dictaphone is not started yet, so start it.
            if (connect_dict_rec()!=STE_ADM_RES_OK){
                ALOG_ERR("Failed to open input dictaphone device\n");
                return 0;
            }
            g_dict_rec_active = 1;
            return 1;
        }
    }

    //
    //  Do we need to close an existing dictaphone record session?
    //
    if (!g_dict_rec_session.device_open) {
        if (g_dict_rec_active) {
            // Dictaphone client disconnected during voicecall.
            // Update state.
            ADM_ASSERT(!g_dict_rec_session.is_finished);
            g_dict_rec_active = 0;
            disconnect_dict_rec();
            g_dict_rec_session.is_finished = 1;
            return 1;
        }
    }

    //
    //  Is a new dictaphone play session wanted?
    //
    if (g_dict_play_session.device_open && !g_dict_play_session.is_finished) {
        // We are connected, we have a dictaphone session that wants to start
        if (!g_dict_play_active) {
            // ..and dictaphone is not started yet, so start it.
            if (connect_dict_play(g_cur_outdev, 0) != STE_ADM_RES_OK) {
                // FUTURE IMPROVEMENT: Do more recovery here - might
                // happen if all port are busy. Reserve a dict port?
                ALOG_ERR("Failed to open output dictaphone device --> CSCALL_STATE_ERROR_FATAL\n");
                g_state = CSCALL_STATE_ERROR_FATAL;
                return 0;
            }

            g_dict_play_active = 1;
            return 1;
        }
    }

    //
    //  Do we need to close an existing dictaphone play session?
    //
    if (!g_dict_play_session.device_open) {
        if (g_dict_play_active) {
            // Dictaphone client disconnected during voicecall.
            // Update state.
            ADM_ASSERT(!g_dict_play_session.is_finished);
            g_dict_play_active = 0;
            disconnect_dict_play(0);
            g_dict_play_session.is_finished = 1;
            return 1;
        }
    }

    csapi_apply_vc_vol_mute();

    // No changes done. No need to re-scan state machine.
    return 0;
}

static int rescan()
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    SRV_DBG_ASSERT_IS_SERIALIZED_REQUEST;

    switch (g_state) {
        case CSCALL_STATE_CONNECTED: {
            // Handle state connected. This includes
            // going to state DISCONNECTED if requested, and
            // turning on/off dictaphone sessions (playback
            // and record) depending on if they are requested
            // by the user.

            //TODO handle result
            return rescan_handle_state_connected();
        }

        case CSCALL_STATE_DISCONNECTED:
            if (csapi_connection_conditions()) {
                ADM_ASSERT(!g_dict_rec_active);
                ADM_ASSERT(!g_dict_play_active);
                ste_adm_res_t res = connect_graph();

                if (res == STE_ADM_RES_OK) {
                    // The main graph is now connected. Dictaphone
                    // is at this point not yet connected.
                    g_state = CSCALL_STATE_CONNECTED;
                    ALOG_STATUS("CSCALL_STATE_CONNECTED\n");
                    //TODO handle result
                    csapi_apply_vc_vol_mute();
                } else if (res == STE_ADM_RES_IL_INSUFFICIENT_RESOURCES) {
                    g_state = CSCALL_STATE_RETRY;
                    ALOG_STATUS("csapi_rescan connect_graph() STE_ADM_RES_IL_INSUFFICIENT_RESOURCES -> CSCALL_STATE_RETRY\n");
                } else if (res == STE_ADM_RES_NO_SUCH_DEVICE) {
                    g_state = CSCALL_STATE_RETRY;
                    ALOG_STATUS("csapi_rescan connect_graph() STE_ADM_RES_NO_SUCH_DEVICE -> CSCALL_STATE_RETRY\n");
                } else {
                    g_state = CSCALL_STATE_ERROR_FATAL;
                    ALOG_STATUS("csapi_rescan connect_graph() -> CSCALL_STATE_ERROR_FATAL\n");
                }

                return 1;
            }
            break;

        case CSCALL_STATE_DISABLED:
        case CSCALL_STATE_ERROR_FATAL:
            break;

        case CSCALL_STATE_RETRY:
            ALOG_INFO("csapi rescan(): In state CSCALL_STATE_RETRY, not doing anything now\n");
            break;

        case CSCALL_STATE_INVALID:
        default:
            ADM_ASSERT(0);
    }

    return 0; // No changes
}

static ste_adm_res_t connect_graph() {
    ALOG_INFO("%s: Enter\n",__func__);
    if (!is_graph_using_d2d(g_cur_indev,g_cur_outdev)) {
        return connect_omx_graph();
    }
    else {
        return connect_d2d();
    }
}

static ste_adm_res_t connect_omx_graph()
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    ste_adm_res_t res;

    /* Get global setting for voice call sample rate (8000, 16000 or auto) */
    const char* data = NULL;
    if (adm_db_global_settings_get(
          ADM_DB_GLOBAL_SETTING_VC_SAMPLERATE, &data) != STE_ADM_RES_OK) {
      ALOG_ERR("Failed to get global setting for voice call samplerate\n");
      res = STE_ADM_RES_DB_INCONSISTENT;
    //  goto cleanup;
    }

    ADM_ASSERT(data!=NULL);
    ALOG_INFO("Global setting for voice call samplerate is: %s\n", data);
    if (strcmp(data, "8000") == 0) {
      /* Global cscall sample rate in database is 8000 */
      if (g_cscall_omx_state.cscall_samplerate != 8000) {
        ALOG_INFO("Network samplerate: %d\n", g_cscall_omx_state.cscall_samplerate);
        ALOG_INFO("Force cscall samplerate to %s\n", data);
        g_cscall_omx_state.cscall_samplerate = 8000;
      }
    } else if (strcmp(data, "16000") == 0) {
      /* Global cscall sample rate in database is 16000 */
      if (g_cscall_omx_state.cscall_samplerate != 16000) {
        ALOG_INFO("Network samplerate: %d\n", g_cscall_omx_state.cscall_samplerate);
        ALOG_INFO("Force cscall samplerate to %s\n", data);
        g_cscall_omx_state.cscall_samplerate = 16000;
      }
    } /* else global cscall sample rate in database is auto */

    if (!g_voip_active) {
        int type = adm_get_modem_type();
        if ((type==ADM_FAT_MODEM) || type==ADM_FULL_FAT_MODEM) {
            ALOG_INFO("Force cscall samplerate to 16000\n");
            g_cscall_omx_state.cscall_samplerate = 16000;
        }
    }

    if(g_tty_mode != STE_ADM_TTY_MODE_OFF){
        switch(g_tty_mode){
            case STE_ADM_TTY_MODE_HCO:
                res = start_async_configure_speechproc_comp(STE_ADM_DEVICE_STRING_TTYIN, g_cur_outdev, NULL);
                break;
            case STE_ADM_TTY_MODE_VCO:
                res = start_async_configure_speechproc_comp(g_cur_indev, STE_ADM_DEVICE_STRING_TTYOUT, NULL);
                break;
            case STE_ADM_TTY_MODE_FULL:
            default:
                res = start_async_configure_speechproc_comp(STE_ADM_DEVICE_STRING_TTYIN, STE_ADM_DEVICE_STRING_TTYOUT, NULL);
                break;
        }
    }
    else{
        res = start_async_configure_speechproc_comp(g_cur_indev, g_cur_outdev, NULL);
    }


    struct dev_params params;
    params.name_in  = g_cur_indev;
    params.name_out = g_cur_outdev;
    params.samplerate = adm_cscall_omx_get_samplerate();
    params.alloc_buffers = 0;
    params.shm_fd = -1;
    sqlite3* db_h;

#ifdef ANDROID
    ALOG_INFO("connect_graph(): [%s %s], %d Hz\n", g_cur_indev, g_cur_outdev, params.samplerate);
 audio_hal_alsa_set_control("Digital Gain Fade Speed Switch", 0, 1);
    adm_db_get_handle(&db_h);
    res = Alsactrl_Hwh_VC(db_h, g_cur_indev, g_cur_outdev, params.samplerate);
    if (res != STE_ADM_RES_OK) {
        return res;
    }
#endif //ANDROID

    params.format = STE_ADM_FORMAT_INVALID; // use device default
    res = dev_open(g_cur_indev, 1, &params, &g_in_dev_handle);
    if (res != STE_ADM_RES_OK) {
        return res;
    }

    res = dev_open(g_cur_outdev, 1, &params, &g_out_dev_handle);
    if (res != STE_ADM_RES_OK) {
        dev_close(g_in_dev_handle, 0);
        return res;
    }

    il_comp_t in_comp,out_comp,echo_comp;
    OMX_U32 in_port,out_port,echo_port;


    dev_get_endpoint(g_in_dev_handle, &in_comp, &in_port);
    dev_get_endpoint(g_out_dev_handle, &out_comp, &out_port);
    dev_get_endpoint_echoref(g_out_dev_handle, &echo_comp, &echo_port);


    if (g_tty_mode != STE_ADM_TTY_MODE_OFF)
    {
        ALOG_INFO("connect_graph(): TTY Enabled, activating using TTY devices.\n");
        res = adm_cscall_omx_activate_cscall(STE_ADM_DEVICE_STRING_TTYIN, STE_ADM_DEVICE_STRING_TTYOUT);
    }
    else
    {
        res = adm_cscall_omx_activate_cscall(g_cur_indev, g_cur_outdev);
    }
    if (res != STE_ADM_RES_OK) {
        dev_close(g_in_dev_handle, 0);
        dev_close(g_out_dev_handle, 0);
        return res;
    }
    wait_end_of_async_configure_speechproc_comp();

    if (!g_voip_active) {
        // Do not connect graph to voicecall input/output for VoIP.
        // This should be handled in VoIP in/out open.
        res = adm_cscall_omx_connect_voicecall_input(in_comp, in_port);
        if (res != STE_ADM_RES_OK) {
            dev_close(g_in_dev_handle, 0);
            dev_close(g_out_dev_handle, 0);
            adm_cscall_omx_deactivate_cscall();
            return res;
        }

        res = adm_cscall_omx_connect_voicecall_output(out_comp, out_port, echo_comp, echo_port);
        if (echo_comp) {
            ALOG_INFO("adm_cscall_omx_connect_voicecall_output %X.%d, echo ref %X.%d\n", out_comp, out_port, echo_comp, echo_port);
        } else {
            ALOG_INFO("adm_cscall_omx_connect_voicecall_output %X.%d, no echo ref\n", out_comp, out_port);
        }

        if (res != STE_ADM_RES_OK) {
            adm_cscall_omx_disconnect_voicecall_input();
            adm_cscall_omx_deactivate_cscall();
            dev_close(g_in_dev_handle, 0);
            dev_close(g_out_dev_handle, 0);
            return res;
        }
    }

    // If VoIP is active, disconnect from common mixer/splitter until
    // VoIP devices are opened to prevent that app chain hangs due to
    // no data consumption in the vc chain.
    if (g_voip_active) {
        dev_disconnect_common(g_in_dev_handle);
        dev_disconnect_common(g_out_dev_handle);
    }

    return STE_ADM_RES_OK;
}

static ste_adm_res_t connect_d2d()
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    ste_adm_res_t res;

    g_cscall_omx_state.cscall_samplerate = 16000;
    ALOG_INFO("%s: [%s %s], %d Hz\n",__func__,g_cur_indev, g_cur_outdev, g_cscall_omx_state.cscall_samplerate);

    if(g_tty_mode != STE_ADM_TTY_MODE_OFF){
        switch(g_tty_mode){
            case STE_ADM_TTY_MODE_HCO:
                res = start_async_configure_speechproc_comp(STE_ADM_DEVICE_STRING_TTYIN, g_cur_outdev, NULL);
                break;
            case STE_ADM_TTY_MODE_VCO:
                res = start_async_configure_speechproc_comp(g_cur_indev, STE_ADM_DEVICE_STRING_TTYOUT, NULL);
                break;
            case STE_ADM_TTY_MODE_FULL:
            default:
                res = start_async_configure_speechproc_comp(STE_ADM_DEVICE_STRING_TTYIN, STE_ADM_DEVICE_STRING_TTYOUT, NULL);
                break;
        }
        ALOG_INFO("%s: TTY Mode:%d\n",__func__,g_tty_mode);
    }
    else {
        res = start_async_configure_speechproc_comp(g_cur_indev, g_cur_outdev, NULL);
    }

    sqlite3* db_h;
    adm_db_get_handle(&db_h);

    res = Alsactrl_Hwh_VC(db_h, g_cur_indev, g_cur_outdev, g_cscall_omx_state.cscall_samplerate);
    if (res != STE_ADM_RES_OK) {
        return res;
    }

    res = dev_open_d2d(STE_ADM_DEVICE_STRING_MODEMUL,g_cur_indev,&g_in_dev_handle);
    if (res != STE_ADM_RES_OK) {
        return res;
    }

    res = dev_open_d2d(STE_ADM_DEVICE_STRING_MODEMDL,g_cur_outdev,&g_out_dev_handle);
    if (res != STE_ADM_RES_OK) {
        dev_close_d2d(&g_in_dev_handle);
        return res;
    }

    wait_end_of_async_configure_speechproc_comp();
    return STE_ADM_RES_OK;
}
static ste_adm_res_t disconnect_graph()
{
    ALOG_INFO("%s: Enter\n",__func__);
    if(!is_graph_using_d2d(g_cur_indev,g_cur_outdev)) {
        return disconnect_omx_graph();
    }
    else {
        return disconnect_d2d();
    }
}

static ste_adm_res_t disconnect_d2d()
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    SRV_DBG_ASSERT_IS_SERIALIZED_REQUEST;
    ALOG_INFO("%s: Enter\n",__func__);
    Alsactrl_Hwh_VC(NULL, NULL, NULL, 0);
    dev_close_d2d(&g_in_dev_handle);
    dev_close_d2d(&g_out_dev_handle);
    ALOG_INFO("%s: Exist\n",__func__);
    return STE_ADM_RES_OK;
}

static ste_adm_res_t disconnect_omx_graph()
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    SRV_DBG_ASSERT_IS_SERIALIZED_REQUEST;

#if defined(ADM_DBG_X86)
    ste_adm_hw_handler_vc(NULL, NULL, NULL, 0);
#else
    Alsactrl_Hwh_VC(NULL, NULL, NULL, 0);
#endif
    // In VoIP mode the input and output are disconnected by close
    // VoIP in/out. Connect to common mixer/splitter again before
    // closing device, since previously disconnected to prevent that
    // app chain hangs due to no data consumption in the vc chain.
    if (g_voip_active && !g_voip_in_session.busy) {
        dev_connect_common(g_in_dev_handle);
    } else {
        adm_cscall_omx_disconnect_voicecall_input();
    }
    if (g_voip_active && !g_voip_out_session.busy) {
        dev_connect_common(g_out_dev_handle);
    } else {
        adm_cscall_omx_disconnect_voicecall_output();
    }

    adm_cscall_omx_deactivate_cscall();

    dev_close(g_in_dev_handle, 0);
    dev_close(g_out_dev_handle, 0);

    return STE_ADM_RES_OK;
}


//
// Change microphone and/or speaker of an existing voicecall graph.
//
static void reroute_live(const char* new_indev, const char* new_outdev, int indev_changed, int outdev_changed)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    SRV_DBG_ASSERT_IS_SERIALIZED_REQUEST;
    ADM_ASSERT(g_state == CSCALL_STATE_CONNECTED);

    sqlite3* db_h;
    int start_async_done,switch_from_d2d=0,switch_from_omx=0;

    ALOG_INFO("reroute_live to [%s %s,%s %s]\n",
        new_indev,  indev_changed  ? "(CHANGED)" : "(not changed)",
        new_outdev, outdev_changed ? "(CHANGED)" : "(not changed)");

    if (indev_changed || outdev_changed) {
        adm_db_get_handle(&db_h);
#if defined(ADM_DBG_X86)
    ste_adm_hw_handler_vc(db_h, new_indev, new_outdev, adm_cscall_omx_get_samplerate());
#else
    Alsactrl_Hwh_VC(db_h, new_indev, new_outdev, adm_cscall_omx_get_samplerate());
#endif
    }

    if (indev_changed) {
        if (g_voip_active && !g_voip_in_session.busy) {
            // Connect to common splitter again before closing, since it has
            // previously been disconnected to prevent that app chain hangs
            // due to no data consumption in the vc chain.
            dev_connect_common(g_in_dev_handle);
            dev_close(g_in_dev_handle, 0);
        } else {
            if(!is_graph_using_d2d(g_cur_indev,g_cur_outdev)) {
                 adm_cscall_omx_disconnect_voicecall_input();
                 dev_close(g_in_dev_handle, 0);
            }else {
                 dev_close_d2d(&g_in_dev_handle);
            }
        }
    }

    if (outdev_changed) {
        if (g_voip_active && !g_voip_out_session.busy) {
            // Connect to common mixer again before closing, since it has
            // previously been disconnected to prevent that app chain hangs
            // due to no data consumption in the vc chain.
            dev_connect_common(g_out_dev_handle);
            dev_close(g_out_dev_handle, 0);
        } else {
            if (!is_graph_using_d2d(g_cur_indev,g_cur_outdev)) {
                adm_cscall_omx_disconnect_voicecall_output();
                dev_close(g_out_dev_handle, 0);
                switch_from_omx = 1;
            } else {
                dev_close_d2d(&g_out_dev_handle);
                switch_from_d2d = 1;
            }
        }
    }

    ALOG_INFO("reroute_live: TTY=%d, dev TTY=%d\n", g_tty_mode, dev_get_tty_mode());
    if (indev_changed && outdev_changed && (dev_get_tty_mode() != g_tty_mode))
    {
        ste_adm_res_t res = dev_set_tty_mode(g_tty_mode);
        if (res != STE_ADM_RES_OK) {
            ALOG_ERR("dev_set_tty_mode failed in reroute_live, not currently handled\n");
            g_state = CSCALL_STATE_ERROR_FATAL;
            return;
        }
    }

    if(g_tty_mode != STE_ADM_TTY_MODE_OFF)
    {
        switch(g_tty_mode){
            case STE_ADM_TTY_MODE_HCO:
                start_async_configure_speechproc_comp(STE_ADM_DEVICE_STRING_TTYIN, new_outdev,&start_async_done);
                break;
            case STE_ADM_TTY_MODE_VCO:
                start_async_configure_speechproc_comp(new_indev, STE_ADM_DEVICE_STRING_TTYOUT,&start_async_done);
                break;
            case STE_ADM_TTY_MODE_FULL:
            default:
                start_async_configure_speechproc_comp(STE_ADM_DEVICE_STRING_TTYIN, STE_ADM_DEVICE_STRING_TTYOUT,&start_async_done);
                break;
        }
    }
    else{
        start_async_configure_speechproc_comp(new_indev, new_outdev,&start_async_done);
    }


    // If the output device has changed and the playback dictaphone
    // is active, we need to close and re-open the connection between
    // the dictaphone splitter output and the application device mixer input.
    int dict_play_involved = 0;
    if (outdev_changed && g_dict_play_active && (adm_get_modem_type()!=ADM_FAT_MODEM)) {
        disconnect_dict_play(1);
        dict_play_involved = 1;
    }

    struct dev_params params;
    params.name_in  = new_indev;
    params.name_out = new_outdev;
    params.samplerate = adm_cscall_omx_get_samplerate();
    params.alloc_buffers = 0;
    params.shm_fd = -1;

    ste_adm_res_t res = STE_ADM_RES_OK;
    if (indev_changed) {
        if (!is_graph_using_d2d(new_indev,new_outdev)) {
            if (switch_from_d2d) {
                res = adm_cscall_omx_activate_cscall(new_indev, new_outdev);
                if (res != STE_ADM_RES_OK) {
                    ALOG_ERR("activate_cscall failed in reroute_live\n");
                    g_state = CSCALL_STATE_ERROR_FATAL;
                    return;
                }
            }
            params.format = STE_ADM_FORMAT_INVALID; // use device default
            res = dev_open(new_indev, 1, &params, &g_in_dev_handle);
        } else {
            if (switch_from_omx) {
                adm_cscall_omx_deactivate_cscall();
            }
            res = dev_open_d2d(STE_ADM_DEVICE_STRING_MODEMUL,new_indev,&g_in_dev_handle);
        }
        if (res == STE_ADM_RES_NO_SUCH_DEVICE) {
            ALOG_ERR("Invalid device during live reroute. Can't recover - %d\n",res);
            g_state = CSCALL_STATE_RETRY;
            return;
        }
        if (res != STE_ADM_RES_OK) {
            ALOG_ERR("dev_open failed in reroute_live, not currently handled - %d\n",res);
            g_state = CSCALL_STATE_ERROR_FATAL;
            return;
        }
    }

    if (outdev_changed && res == STE_ADM_RES_OK) {
        params.format = STE_ADM_FORMAT_MONO;
        if (!is_graph_using_d2d(new_indev,new_outdev))
            res = dev_open(new_outdev, 1, &params, &g_out_dev_handle);
        else {
            res = dev_open_d2d(STE_ADM_DEVICE_STRING_MODEMDL,new_outdev,&g_out_dev_handle);
        }

        if (res == STE_ADM_RES_NO_SUCH_DEVICE) {
            ALOG_ERR("Invalid device during live reroute. Can't recover -%d\n",res);
            adm_cscall_omx_disconnect_voicecall_input();
            dev_close(g_in_dev_handle, 0);
            g_state = CSCALL_STATE_RETRY;
            return;
        }

        if (res != STE_ADM_RES_OK) {
            ALOG_ERR("dev_open failed in reroute_live, not currently handled\n");
            g_state = CSCALL_STATE_ERROR_FATAL;
            return;
        }
    }

    if (res == STE_ADM_RES_OK) {
        if(!start_async_done){
            if(g_tty_mode != STE_ADM_TTY_MODE_OFF)
            {
                switch(g_tty_mode){
                    case STE_ADM_TTY_MODE_HCO:
                        adm_cscall_omx_rescan_speech_proc(STE_ADM_DEVICE_STRING_TTYIN, new_outdev);
                        break;
                    case STE_ADM_TTY_MODE_VCO:
                        adm_cscall_omx_rescan_speech_proc(new_indev, STE_ADM_DEVICE_STRING_TTYOUT);
                        break;
                    case STE_ADM_TTY_MODE_FULL:
                    default:
                        adm_cscall_omx_rescan_speech_proc(STE_ADM_DEVICE_STRING_TTYIN, STE_ADM_DEVICE_STRING_TTYOUT);
                        break;
                }
            }
            else{
                adm_cscall_omx_rescan_speech_proc(new_indev, new_outdev);
            }
        }else {
            wait_end_of_async_configure_speechproc_comp();
        }
        if (indev_changed) {
            if (g_voip_active && !g_voip_in_session.busy) {
                // Disconnect from common splitter until VoIPIn is opened to prevent
                // that app chain hangs due to no data consumption in the vc chain.
                // Connection to voicecall input is handled in VoIPIn open.
                dev_disconnect_common(g_in_dev_handle);
            } else {
                il_comp_t in_comp;
                OMX_U32 in_port;
                if(!is_graph_using_d2d(new_indev,new_outdev)) {
                    dev_get_endpoint(g_in_dev_handle, &in_comp, &in_port);
                    adm_cscall_omx_connect_voicecall_input(in_comp, in_port);
                }
            }
        }

        if (outdev_changed) {
            if (g_voip_active && !g_voip_out_session.busy) {
                // Disconnect from common mixer until VoIPOut is opened to prevent
                // that app chain hangs due to no data consumption in the vc chain.
                // Connection to voicecall output is handled in VoIPOut open.
                dev_disconnect_common(g_out_dev_handle);
            } else {
                il_comp_t out_comp;
                OMX_U32 out_port;
                if (!is_graph_using_d2d(new_indev,new_outdev)) {
                    dev_get_endpoint(g_out_dev_handle, &out_comp, &out_port);
                    il_comp_t echo_comp;
                    OMX_U32 echo_port;
                    dev_get_endpoint_echoref(g_out_dev_handle, &echo_comp, &echo_port);
                    adm_cscall_omx_connect_voicecall_output(out_comp, out_port, echo_comp, echo_port);
                }
            }
        }
    }

    if ((dict_play_involved) && (adm_get_modem_type()!=ADM_FAT_MODEM)){
        connect_dict_play(new_outdev, 1);
    }

    ALOG_INFO("reroute_live to [%s %s,%s %s] COMPLETED\n",
        new_indev,  indev_changed  ? "(CHANGED)" : "(not changed)",
        new_outdev, outdev_changed ? "(CHANGED)" : "(not changed)");
}

void csapi_set_loopback(msg_cscall_loopback_t* msg_p, struct srv_session_s** client_pp)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    SRV_DBG_ASSERT_IS_SERIALIZED_REQUEST;
    (void) client_pp;

    /* Get global setting for loopback delay */
    const char* DelayString = NULL;
    if (adm_db_global_settings_get(
       ADM_DB_GLOBAL_SETTING_LOOPBACK_DELAY, &DelayString) != STE_ADM_RES_OK) {
       ALOG_ERR("Failed to get global setting for loopback delay\n");
    }

#ifndef ADM_DBG_X86
    // Enable loopback delay
    if(DelayString && msg_p->enabled){
       property_set("ste.adm.loopback.delay",DelayString);
    }
    else{
       property_set("ste.adm.loopback.delay","0");
    }
#endif

    if (g_state == CSCALL_STATE_DISCONNECTED) {
        if (g_loopback_mode != msg_p->enabled) {

            msg_p->base.result = cscall_omx_set_loopback_mode(
                                        msg_p->enabled,
                                        msg_p->codec_type);

            if (msg_p->base.result != STE_ADM_RES_OK) {
                ALOG_ERR("Set loopback: failed, --> CSCALL_STATE_ERROR_FATAL");
                g_state = CSCALL_STATE_ERROR_FATAL;
            } else {
                g_loopback_mode = (msg_p->enabled != 0);

                if (g_loopback_mode) {
                    ALOG_STATUS("Loopback enabled, codec_type=%d\n", msg_p->codec_type);
                } else {
                    ALOG_STATUS("Loopback disabled\n");
                }
            }
        } else {
            ALOG_INFO("set_loopback calledin same state as current, not changing\n");
            ALOG_INFO("to change codec, first disable loopback and then re-enable it\n");
        }
    } else {
        ALOG_STATUS("set_loopback called while voicecall not disconnected; not allowed\n");
        msg_p->base.result = STE_ADM_RES_INCORRECT_STATE;
    }
    srv_send_reply(&msg_p->base);
}

// TODO: Check for CSCALL_STATE_ERROR_FATAL (zombie) everywhere

static void dict_in_end()
{
    ADM_ASSERT(g_dict_rec_session.busy);

    il_comp_t comp;
    OMX_U32 port;
    if (!adm_cscall_get_dict_rec_comp(&comp, &port)) {
        ALOG_ERR("dict_in_end: cscall graph not setup!\n");
    }

    (void) adm_cscall_omx_disable_voip_output();

    g_dict_rec_session.device_open = 0;
    while(rescan());
    ADM_ASSERT(!g_dict_rec_active);
    g_dict_rec_session.busy = 0;
}

void csapi_dict_in_dtor(void* client_p)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    SRV_DBG_ASSERT_IS_SERIALIZED_REQUEST;

    ADM_ASSERT(client_p == &g_dict_rec_session);
    ADM_ASSERT(g_dict_rec_session.busy);
    ALOG_STATUS("Dictaphone input session exiting\n");
    dict_in_end();
}

void csapi_dict_in_close(msg_device_t* msg_p, void** client_pp)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    SRV_DBG_ASSERT_IS_SERIALIZED_REQUEST;

    ADM_ASSERT(g_dict_rec_session.busy);

    if (strcmp(msg_p->name, STE_ADM_DEVICE_STRING_DICT_REC) != 0) {
        // Name did not match device handled by this function
        ALOG_ERR("csapi_dict_in_close: device '%s' not handled\n", msg_p->name);
        msg_p->base.result = STE_ADM_RES_NO_SUCH_DEVICE;
        srv_send_reply((msg_base_t*) msg_p);
        return;
    }

    ALOG_STATUS("Dictaphone input session closing\n");
    ADM_ASSERT(*client_pp == &g_dict_rec_session);

    *client_pp = 0;
    dict_in_end();
    srv_send_reply(&msg_p->base);
}

static void dict_out_end()
{
    ADM_ASSERT(g_dict_play_session.busy);

    il_comp_t comp;
    OMX_U32 port;
    if (!adm_cscall_get_dict_play_comp(&comp, &port)) {
        ALOG_ERR("dict_out_end: cscall graph not setup!\n");
    }

    g_dict_play_session.device_open = 0;
    while(rescan());
    ADM_ASSERT(!g_dict_play_active);
    g_dict_play_session.busy = 0;
}

void csapi_dict_out_dtor(void* client_p)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    SRV_DBG_ASSERT_IS_SERIALIZED_REQUEST;

    ADM_ASSERT(client_p == &g_dict_play_session);
    ADM_ASSERT(g_dict_play_session.busy);
    ALOG_STATUS("Dictaphone output session exiting\n");
    dict_out_end();
}

void csapi_dict_out_close(msg_device_t* msg_p, void** client_pp)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    SRV_DBG_ASSERT_IS_SERIALIZED_REQUEST;

    ADM_ASSERT(g_dict_play_session.busy);

    if (strcmp(msg_p->name, STE_ADM_DEVICE_STRING_DICT_PLAY) != 0) {
        // Name did not match device handled by this function
        ALOG_ERR("csapi_dict_out_close: device '%s' not handled\n", msg_p->name);
        msg_p->base.result = STE_ADM_RES_NO_SUCH_DEVICE;
        srv_send_reply((msg_base_t*) msg_p);
        return;
    }

    ALOG_STATUS("Dictaphone output session closing\n");
    ADM_ASSERT(*client_pp == &g_dict_play_session);

    *client_pp = 0;
    dict_out_end();
    srv_send_reply(&msg_p->base);
}


int csapi_dict_in_open(msg_device_t* msg_p, void** client_pp)
{
    if (strcmp(msg_p->name, STE_ADM_DEVICE_STRING_DICT_REC) != 0) {
        // Name did not match device handled by this function
        return 0;
    }

    ALOG_STATUS("Open dictaphone record device\n");

    if (msg_p->format != STE_ADM_FORMAT_MONO && msg_p->format != STE_ADM_FORMAT_STEREO) {
        ALOG_ERR("DICT REC: format must be MONO or STEREO (was %d)\n", msg_p->format);
        msg_p->base.result = STE_ADM_RES_INVALID_PARAMETER;
        srv_send_reply((msg_base_t*) msg_p);
        return 1;
    };

    if (g_dict_rec_session.device_open) {
        ALOG_ERR("DICT REC: Already busy\n");
        msg_p->base.result = STE_ADM_RES_DEVICE_ALREADY_OPEN;
        srv_send_reply((msg_base_t*) msg_p);
        return 1;
    }

    if (g_dict_rec_session.busy) {
        ALOG_ERR("DICT REC: Dict already active in other session\n");
        msg_p->base.result = STE_ADM_RES_INCORRECT_STATE;
        srv_send_reply((msg_base_t*) msg_p);
        return 1;
    };

    // Init and register the session
    g_dict_rec_session.busy        = 1;
    g_dict_rec_session.is_finished = 0;
    g_dict_rec_session.device_open = 1;
    g_dict_rec_num_bufs            = msg_p->num_bufs;
    g_dict_rec_bufsz               = msg_p->bufsz;
    g_dict_rec_format              = msg_p->format;
    g_dict_rec_samplerate          = msg_p->samplerate;


    // Go thrugh cscall state. If cscall is up now, dictaphone will be opened.
    while(rescan());

    int fd_to_xfer;
    il_comp_t comp;
    OMX_U32 port;
    if (!adm_cscall_get_dict_rec_comp(&comp, &port)) {
        ALOG_ERR("DICT REC: No active CS call\n");
        msg_p->base.result = STE_ADM_RES_INCORRECT_STATE;
        srv_send_reply((msg_base_t*) msg_p);
        g_dict_rec_session.busy = 0;
        g_dict_rec_session.device_open = 0;
        return 1;
    }
    il_tool_get_buffers(comp, port, &fd_to_xfer, NULL, NULL);


    *client_pp = (struct srv_session_s*) &g_dict_rec_session;
    ALOG_INFO("DICT REC: Open done\n");

    msg_p->actual_samplerate = adm_cscall_omx_get_samplerate();
    srv_send_reply_and_fd(&msg_p->base, fd_to_xfer);
    return 1;
}

int csapi_dict_out_open(msg_device_t* msg_p, void** client_pp)
{
    if (strcmp(msg_p->name, STE_ADM_DEVICE_STRING_DICT_PLAY) != 0) {
        // Name did not match device handled by this function
        return 0;
    }

    ALOG_STATUS("Open dictaphone playback device\n");

    if (msg_p->format != STE_ADM_FORMAT_MONO) {
        ALOG_ERR("DICT PLAY: format must be MONO (was %d)\n", msg_p->format);
        msg_p->base.result = STE_ADM_RES_INVALID_PARAMETER;
        srv_send_reply((msg_base_t*) msg_p);
        return 1;
    };

    if (g_dict_play_session.device_open) {
        ALOG_ERR("DICT PLAY: Already busy\n");
        msg_p->base.result = STE_ADM_RES_DEVICE_ALREADY_OPEN;
        srv_send_reply((msg_base_t*) msg_p);
        return 1;
    }

    if (g_dict_play_session.busy) {
        ALOG_ERR("DICT PLAY: Dict already active in other session\n");
        msg_p->base.result = STE_ADM_RES_INCORRECT_STATE;
        srv_send_reply((msg_base_t*) msg_p);
        return 1;
    };

    // Init and register the session
    g_dict_play_session.busy        = 1;
    g_dict_play_session.is_finished = 0;
    g_dict_play_session.device_open = 1;
    g_dict_play_num_bufs            = msg_p->num_bufs;
    g_dict_play_bufsz               = msg_p->bufsz;
    g_dict_play_num_ch              = msg_p->format;
    g_dict_play_sr                  = msg_p->samplerate;


    // Go thrugh cscall state. If cscall is up now, dictaphone will be opened.
    while(rescan());

    int fd_to_xfer;
    il_comp_t comp;
    OMX_U32 port;
    if (!adm_cscall_get_dict_play_comp(&comp, &port)) {
        ALOG_ERR("DICT PLAY: No active CS call\n");
        msg_p->base.result = STE_ADM_RES_INCORRECT_STATE;
        srv_send_reply((msg_base_t*) msg_p);
        g_dict_play_session.busy = 0;
        g_dict_play_session.device_open = 0;
        return 1;
    }
    il_tool_get_buffers(comp, port, &fd_to_xfer, NULL, NULL);


    *client_pp = (struct srv_session_s*) &g_dict_play_session;
    ALOG_INFO("DICT PLAY: Open done\n");
    msg_p->actual_samplerate = adm_cscall_omx_get_samplerate();
    srv_send_reply_and_fd(&msg_p->base, fd_to_xfer);
    return 1;
}

void csapi_dict_set_mode(msg_cscall_dict_mode_t* msg_p, srv_session_t** client_pp)
{
    (void) client_pp; // No state
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

    ALOG_INFO("csapi_dict_set_mode: %d \n", msg_p->mode);

    ste_adm_res_t res = adm_cscall_omx_set_dict_mode(msg_p->mode);

    if (res != STE_ADM_RES_OK)
    {
        ALOG_ERR("csapi_dict_set_mode: Failed error = %d\n", res);
        msg_p->base.result = res;
    }

    srv_send_reply((msg_base_t*)msg_p);
}

void csapi_dict_set_mute(msg_cscall_dict_mute_t* msg_p, srv_session_t** client_pp)
{
    (void) client_pp;

    ALOG_INFO("csapi_dict_set_mute: %d \n", msg_p->mute);
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

    if (!g_dict_rec_active) {
        ALOG_INFO("csapi_dict_set_mute - STE_ADM_RES_CSCALL_NOT_ACTIVE\n");
        msg_p->base.result = STE_ADM_RES_CSCALL_NOT_ACTIVE;
        srv_send_reply(&msg_p->base);
        return;
    }

    if (g_state == CSCALL_STATE_ERROR_FATAL) {
        ALOG_ERR("csapi_dict_set_mute: CSCALL_STATE_ERROR_FATAL due to earlier, unrecoverable error\n");
        msg_p->base.result = STE_ADM_RES_UNRECOVERABLE_ERROR;
        srv_send_reply(&msg_p->base);
        return;
    }

    if (g_dict_rec_session.is_finished || g_state == CSCALL_STATE_DISCONNECTING) {
        ALOG_INFO("csapi_dict_set_mute: Session finished\n");
        msg_p->base.result = STE_ADM_RES_CSCALL_FINISHED;
        srv_send_reply((msg_base_t*) msg_p);
        return;
    }

    il_comp_t comp;
    OMX_U32 port;
    if (!adm_cscall_get_dict_rec_comp(&comp, &port)) {
        ALOG_ERR("csapi_dict_set_mute: dictaphone graph not setup!\n");
        msg_p->base.result = STE_ADM_RES_ERR_MSG_IO;
        srv_send_reply((msg_base_t*) msg_p);
        return;
    }

    ste_adm_res_t res = adm_cscall_omx_set_dict_mute(msg_p->mute);
    msg_p->base.result = res;

    if (res != STE_ADM_RES_OK)
        ALOG_ERR("csapi_dict_set_mute: Failed error = %d\n", res);

    srv_send_reply((msg_base_t*)msg_p);
}

void csapi_dict_get_mute(msg_cscall_dict_mute_t* msg_p, srv_session_t** client_pp)
{
    (void) client_pp;

    ALOG_INFO("csapi_dict_get_mute\n");
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

    if (!g_dict_rec_active) {
        ALOG_INFO("csapi_dict_get_mute - STE_ADM_RES_CSCALL_NOT_ACTIVE\n");
        msg_p->base.result = STE_ADM_RES_CSCALL_NOT_ACTIVE;
        srv_send_reply(&msg_p->base);
        return;
    }

    if (g_state == CSCALL_STATE_ERROR_FATAL) {
        ALOG_ERR("csapi_dict_get_mute: CSCALL_STATE_ERROR_FATAL due to earlier, unrecoverable error\n");
        msg_p->base.result = STE_ADM_RES_UNRECOVERABLE_ERROR;
        srv_send_reply(&msg_p->base);
        return;
    }

    if (g_dict_rec_session.is_finished || g_state == CSCALL_STATE_DISCONNECTING) {
        ALOG_INFO("csapi_dict_get_mute: Session finished\n");
        msg_p->base.result = STE_ADM_RES_CSCALL_FINISHED;
        srv_send_reply((msg_base_t*) msg_p);
        return;
    }

    il_comp_t comp;
    OMX_U32 port;
    if (!adm_cscall_get_dict_rec_comp(&comp, &port)) {
        ALOG_ERR("csapi_dict_get_mute: dictaphone graph not setup!\n");
        msg_p->base.result = STE_ADM_RES_ERR_MSG_IO;
        srv_send_reply((msg_base_t*) msg_p);
        return;
    }

    ste_adm_res_t res = adm_cscall_omx_get_dict_mute(&msg_p->mute);
    msg_p->base.result = res;

    if (res != STE_ADM_RES_OK)
        ALOG_ERR("csapi_dict_get_mute: Failed error = %d\n", res);

    srv_send_reply((msg_base_t*)msg_p);
}

static void disconnect_dict_rec()
{
    il_comp_t comp;
    OMX_U32 port;
    if (!adm_cscall_get_dict_rec_comp(&comp, &port)) {
        ADM_ASSERT(0);
        return;
    }

    // Mark dictaphone session as finished, so future requests
    // will return STE_ADM_RES_CSCALL_FINISHED.
    g_dict_rec_session.is_finished = 1;
    adm_cscall_omx_disable_dict_rec();
}

static ste_adm_res_t connect_dict_rec()
{
    ste_adm_res_t res = adm_cscall_omx_enable_dict_rec(g_dict_rec_num_bufs, g_dict_rec_bufsz,
                                                       g_dict_rec_samplerate, g_dict_rec_format);
    return res;
}

static ste_adm_res_t connect_dict_play(const char* outdev_name, int only_app_part)
{
    // NOTE: dict play feature affects reroute_live
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

    struct dev_params params;
    params.samplerate = adm_cscall_omx_get_samplerate();
    params.format = STE_ADM_FORMAT_MONO;
    params.alloc_buffers = 0;
    params.shm_fd = -1;

    ALOG_INFO("connect_dict_play() %s",only_app_part?"uplink only\n":"uplink and downlink");

    // Open app mixer connection to play dictaphone on..
    ste_adm_res_t res = dev_open(outdev_name, 0, &params, &g_dict_play_dev_handle);
    if (res != STE_ADM_RES_OK) {
        ALOG_ERR("Failed to open app dict device on %s: %d\n", outdev_name, res);
        return res;
    }

    if (!only_app_part) {
        adm_cscall_omx_enable_dict_play(g_dict_play_num_bufs, g_dict_play_bufsz,g_dict_play_sr, g_dict_play_num_ch);
    }

    il_comp_t app_comp;
    OMX_U32 app_port;
    dev_get_endpoint(g_dict_play_dev_handle, &app_comp, &app_port);


    res = adm_cscall_omx_enable_dict_play_app(app_comp, app_port);
    if (res != STE_ADM_RES_OK) {
        dev_close(g_dict_play_dev_handle, 0);
        g_dict_play_dev_handle = 0;
    }

   return res;
}

static ste_adm_res_t disconnect_dict_play(int only_app_part)
{
    ADM_ASSERT(g_dict_play_dev_handle != 0);

    il_comp_t comp;
    OMX_U32 port;
    if (!adm_cscall_get_dict_play_comp(&comp, &port)) {
        ADM_ASSERT(0);
        return STE_ADM_RES_INTERNAL_ERROR;
    }

    adm_cscall_omx_disable_dict_play_app();

    if (!only_app_part) {
        adm_cscall_omx_disable_dict_play();
    }

    ste_adm_res_t res;
    res = dev_close(g_dict_play_dev_handle, 0);
    if (res != STE_ADM_RES_OK) {
        // Error during disconnect; FATAL
        return res;
    }

    g_dict_play_dev_handle = 0;
    return res;
}

void csapi_dict_in_datareq(msg_data_t* msg_p, void** client_pp)
{
    ADM_ASSERT(g_dict_rec_session.busy);

    (void) client_pp;

    if (!g_dict_rec_active) {
        ALOG_INFO("csapi_dict_in_datareq - STE_ADM_RES_CSCALL_NOT_ACTIVE\n");
        msg_p->base.result = STE_ADM_RES_CSCALL_NOT_ACTIVE;
        srv_send_reply(&msg_p->base);
        return;
    }

    if (g_state == CSCALL_STATE_ERROR_FATAL) {
        ALOG_ERR("csapi_dict_in_datareq: CSCALL_STATE_ERROR_FATAL due to earlier, unrecoverable error\n");
        msg_p->base.result = STE_ADM_RES_UNRECOVERABLE_ERROR;
        srv_send_reply(&msg_p->base);
        return;
    }

    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

    if (g_dict_rec_session.is_finished || g_state == CSCALL_STATE_DISCONNECTING) {
        ALOG_INFO("csapi_dict_in_datareq: Session finished\n");
        msg_p->base.result = STE_ADM_RES_CSCALL_FINISHED;
        srv_send_reply((msg_base_t*) msg_p);
        return;
    }

    il_comp_t comp;
    OMX_U32 port;
    if (!adm_cscall_get_dict_rec_comp(&comp, &port)) {
        ALOG_ERR("csapi_dict_in_datareq: cscall graph not setup!\n");
        msg_p->base.result = STE_ADM_RES_ERR_MSG_IO;
        srv_send_reply((msg_base_t*) msg_p);
        return;
    }

    il_tool_io_request_data(comp, port, msg_p);
}


void csapi_dict_out_feed(msg_data_t* msg_p, void** client_pp)
{
    ADM_ASSERT(g_dict_play_session.busy);

    (void) client_pp;

    ALOG_INFO_VERBOSE("DICT PLAY FEED\n");

    if (!g_dict_play_active) {
        ALOG_INFO("csapi_dict_out_feed - STE_ADM_RES_CSCALL_NOT_ACTIVE\n");
        msg_p->base.result = STE_ADM_RES_CSCALL_NOT_ACTIVE;
        srv_send_reply(&msg_p->base);
        return;
    }

    if (g_state == CSCALL_STATE_ERROR_FATAL) {
        ALOG_ERR("csapi_dict_out_feed: CSCALL_STATE_ERROR_FATAL due to earlier, unrecoverable error\n");
        msg_p->base.result = STE_ADM_RES_UNRECOVERABLE_ERROR;
        srv_send_reply(&msg_p->base);
        return;
    }

    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

    if (g_dict_play_session.is_finished || g_state == CSCALL_STATE_DISCONNECTING) {
        ALOG_INFO("csapi_dict_out_feed: Session finished\n");
        msg_p->base.result = STE_ADM_RES_CSCALL_FINISHED;
        srv_send_reply((msg_base_t*) msg_p);
        return;
    }

    il_comp_t comp;
    OMX_U32 port;
    if (!adm_cscall_get_dict_play_comp(&comp, &port)) {
        ALOG_ERR("csapi_dict_out_feed: cscall graph not setup!\n");
        msg_p->base.result = STE_ADM_RES_ERR_MSG_IO;
        srv_send_reply((msg_base_t*) msg_p);
        return;
    }

    il_tool_io_feed(comp, port, msg_p);
}

// Drain is asynchronous.
static void dict_play_io_idle_callback(void* param);
void csapi_dict_out_drain(msg_base_t* msg_p, void** client_pp)
{
    ADM_ASSERT(g_dict_play_session.busy);

    (void) client_pp;

    ALOG_INFO("csapi_drain\n");;

    if (!g_dict_play_active) {
        ALOG_INFO("csapi_dict_out_drain - STE_ADM_RES_CSCALL_NOT_ACTIVE\n");
        msg_p->result = STE_ADM_RES_CSCALL_NOT_ACTIVE;
        srv_send_reply(msg_p);
        return;
    }

    if (g_state == CSCALL_STATE_ERROR_FATAL) {
        ALOG_ERR("csapi_dict_out_drain: CSCALL_STATE_ERROR_FATAL due to earlier, unrecoverable error\n");
        msg_p->result = STE_ADM_RES_UNRECOVERABLE_ERROR;
        srv_send_reply(msg_p);
        return;
    }

    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

    if (g_dict_play_session.is_finished || g_state == CSCALL_STATE_DISCONNECTING) {
        ALOG_INFO("csapi_dict_out_drain: Session finished\n");
        msg_p->result = STE_ADM_RES_CSCALL_FINISHED;
        srv_send_reply(msg_p);
        return;
    }

    ADM_ASSERT(!g_dict_play_session.drain_msg_p);

    il_comp_t comp;
    OMX_U32 port;
    if (!adm_cscall_get_dict_play_comp(&comp, &port)) {
        ALOG_ERR("csapi_dict_out_drain: cscall graph not setup!\n");
        msg_p->result = STE_ADM_RES_ERR_MSG_IO;
        srv_send_reply(msg_p);
        return;
    }

    if (il_tool_io_is_idle(comp, port)) {
        ALOG_INFO("csapi_dict_out_drain - already idle, sending immediate response\n");
        srv_send_reply(msg_p);
    } else {
        ALOG_INFO("csapi_dict_out_drain - not idle yet, postponing response...\n");
        g_dict_play_session.drain_msg_p = msg_p;
        il_tool_io_subscribe_idle_callback_once(comp, port, dict_play_io_idle_callback, NULL);
        // reply will be sent when IO is idle
    }
}

static void dict_play_io_idle_callback(void* param)
{
    (void) param;
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

    ADM_ASSERT(g_dict_play_session.drain_msg_p);
    srv_send_reply(g_dict_play_session.drain_msg_p);
    g_dict_play_session.drain_msg_p = NULL;
}

static ste_adm_res_t voip_in_end()
{
    ADM_ASSERT(g_voip_active);
    ADM_ASSERT(g_voip_in_session.busy);

    ste_adm_res_t res = STE_ADM_RES_OK;
    if (g_state == CSCALL_STATE_CONNECTED && g_voip_in_session.busy) {
        il_comp_t comp;
        OMX_U32 port;
        if (!adm_cscall_get_voip_in_comp(&comp, &port)) {
            ALOG_ERR("voip_in_end: cscall graph not setup!\n");
            res = STE_ADM_RES_INCORRECT_STATE;
        }

        (void) adm_cscall_omx_disable_voip_input();
    }

    g_voip_in_session.busy = 0;

    return res;
}

void csapi_voip_in_dtor(void* client_p)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    SRV_DBG_ASSERT_IS_SERIALIZED_REQUEST;

    ADM_ASSERT(client_p == &g_voip_in_session);
    ADM_ASSERT(g_voip_active);
    ADM_ASSERT(g_voip_in_session.busy);
    ALOG_STATUS("VoIP input session exiting\n");
    voip_in_end();
}

void csapi_voip_in_close(msg_device_t* msg_p, void** client_pp)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    SRV_DBG_ASSERT_IS_SERIALIZED_REQUEST;

    ADM_ASSERT(g_voip_active);
    ADM_ASSERT(g_voip_in_session.busy);


    if (strcmp(msg_p->name, STE_ADM_DEVICE_STRING_VOIPIN) != 0) {
        // Name did not match device handled by this function
        ALOG_ERR("csapi_voip_in_close: device '%s' not handled\n", msg_p->name);
        msg_p->base.result = STE_ADM_RES_NO_SUCH_DEVICE;
        srv_send_reply((msg_base_t*) msg_p);
        return;
    }

    if (!g_voip_in_session.busy) {
        ALOG_ERR("csapi_voip_in_close called when voip in session is not busy\n");
        msg_p->base.result = STE_ADM_RES_INCORRECT_STATE;
        srv_send_reply((msg_base_t*) msg_p);
        return;
    }

    // Disconnect from common splitter to prevent that app chain hangs
    // due to no data consumption in the vc chain.
    msg_p->base.result = dev_disconnect_common(g_in_dev_handle);
    if (msg_p->base.result != STE_ADM_RES_OK) {
        ALOG_ERR("voip_in_close: disconnect from common chain failed!\n");
        srv_send_reply((msg_base_t*) msg_p);
        return;
    }

    adm_cscall_omx_disconnect_voicecall_input();

    ALOG_STATUS("VoIP input session closing\n");
    ADM_ASSERT(*client_pp == &g_voip_in_session);

    *client_pp = 0;
    msg_p->base.result = voip_in_end();
    srv_send_reply(&msg_p->base);
}

static ste_adm_res_t voip_out_end()
{
    ADM_ASSERT(g_voip_active);
    ADM_ASSERT(g_voip_out_session.busy);

    ste_adm_res_t res = STE_ADM_RES_OK;
    if (g_state == CSCALL_STATE_CONNECTED && g_voip_out_session.busy) {
        il_comp_t comp;
        OMX_U32 port;
        if (!adm_cscall_get_voip_out_comp(&comp, &port)) {
            ALOG_ERR("voip_out_end: cscall graph not setup!\n");
            res = STE_ADM_RES_INCORRECT_STATE;
        }

        (void) adm_cscall_omx_disable_voip_output();
    }

    g_voip_out_session.busy = 0;

    return res;
}

void csapi_voip_out_dtor(void* client_p)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    SRV_DBG_ASSERT_IS_SERIALIZED_REQUEST;

    ADM_ASSERT(client_p == &g_voip_out_session);
    ALOG_STATUS("VoIP output session exiting\n");
    voip_out_end();
}

void csapi_voip_out_close(msg_device_t* msg_p, void** client_pp)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    SRV_DBG_ASSERT_IS_SERIALIZED_REQUEST;

    ADM_ASSERT(g_voip_active);
    ADM_ASSERT(g_voip_out_session.busy);

    if (strcmp(msg_p->name, STE_ADM_DEVICE_STRING_VOIPOUT) != 0) {
        // Name did not match device handled by this function
        ALOG_ERR("csapi_voip_out_close: device '%s' not handled\n", msg_p->name);
        msg_p->base.result = STE_ADM_RES_NO_SUCH_DEVICE;
        srv_send_reply((msg_base_t*) msg_p);
        return;
    }

    if (!g_voip_out_session.busy) {
        ALOG_ERR("csapi_voip_out_close called when vopi out session is not busy\n");
        msg_p->base.result = STE_ADM_RES_INCORRECT_STATE;
        srv_send_reply((msg_base_t*) msg_p);
        return;
    }

    // Disconnect from common mixer to prevent that app chain hangs
    // due to no data consumption in the vc chain.
    msg_p->base.result = dev_disconnect_common(g_out_dev_handle);
    if (msg_p->base.result != STE_ADM_RES_OK) {
        ALOG_ERR("voip_out_close: connect to common chain failed!\n");
        srv_send_reply((msg_base_t*) msg_p);
        return;
    }

    adm_cscall_omx_disconnect_voicecall_output();

    ALOG_STATUS("VoIP output session closing\n");
    ADM_ASSERT(*client_pp == &g_voip_out_session);

    msg_p->base.result = voip_out_end();
    srv_send_reply(&msg_p->base);
    *client_pp = 0;
}


int csapi_voip_in_open(msg_device_t* msg_p, void** client_pp)
{
    ste_adm_res_t res;

    if (strcmp(msg_p->name, STE_ADM_DEVICE_STRING_VOIPIN) != 0) {
        // Name did not match device handled by this function
        return 0;
    }

    ALOG_STATUS("Open VoIP input device\n");

    if (g_voip_in_session.busy) {
        ALOG_ERR("csapi_voip_in_open: VoIP in already active\n");
        msg_p->base.result = STE_ADM_RES_INCORRECT_STATE;
        srv_send_reply((msg_base_t*) msg_p);
        return 1;
    }

    if (!g_voip_active) {
        ALOG_ERR("csapi_voip_in_open: VoIP mode not enabled\n");
        msg_p->base.result = STE_ADM_RES_INCORRECT_STATE;
        srv_send_reply((msg_base_t*) msg_p);
        return 1;
    }

    if (msg_p->format != (ste_adm_format_t)g_voip_in_session.channels) {
        ALOG_ERR("Voip input: format must be %d (was %d)\n", g_voip_in_session.channels, msg_p->format);
        msg_p->base.result = STE_ADM_RES_INVALID_PARAMETER;
        srv_send_reply((msg_base_t*) msg_p);
        return 1;
    }

    // Connect to common splitter, since it was disconnected when call
    // graph was setup to prevent that app chain hangs due to no data
    // consumption in the vc chain.
    msg_p->base.result = dev_connect_common(g_in_dev_handle);
    if (msg_p->base.result != STE_ADM_RES_OK) {
        ALOG_ERR("voip_in_open: connect to common chain failed!\n");
        srv_send_reply((msg_base_t*) msg_p);
        return 1;
    }

    // Connect graph with voicecall input
    il_comp_t in_comp;
    OMX_U32 in_port;
    dev_get_endpoint(g_in_dev_handle, &in_comp, &in_port);

    res = adm_cscall_omx_connect_voicecall_input(in_comp, in_port);
    if (res != STE_ADM_RES_OK) {
        ALOG_ERR("csapi_voip_in_open: could not connect to voicecall input\n");
        msg_p->base.result = STE_ADM_RES_INCORRECT_STATE;
        srv_send_reply((msg_base_t*) msg_p);
        return 1;
    }

    msg_p->actual_samplerate = g_voip_in_session.samplerate;

    int fd_to_xfer;
    il_comp_t comp;
    OMX_U32 port;
    if (!adm_cscall_get_voip_in_comp(&comp, &port)) {
        ALOG_ERR("csapi_voip_in_open: cscall graph not setup!\n");
        msg_p->base.result = STE_ADM_RES_INCORRECT_STATE;
        srv_send_reply((msg_base_t*) msg_p);
        return 1;
    }

    msg_p->base.result = adm_cscall_omx_enable_voip_input(msg_p->num_bufs, msg_p->bufsz);
    if (msg_p->base.result == STE_ADM_RES_OK) {
        il_tool_get_buffers(comp, port, &fd_to_xfer, NULL, NULL);
    }

    ALOG_INFO("csapi_voip_in_open: Open done\n");
    *client_pp = (struct srv_session_s*) &g_voip_in_session;
    g_voip_in_session.busy = 1;
    srv_send_reply_and_fd((msg_base_t*) msg_p, fd_to_xfer);
    return 1;
}

int csapi_voip_out_open(msg_device_t* msg_p, void** client_pp)
{
    ste_adm_res_t res;

    if (strcmp(msg_p->name, STE_ADM_DEVICE_STRING_VOIPOUT) != 0) {
        // Name did not match device handled by this function
        return 0;
    }

    ALOG_STATUS("Open VoIP output device\n");

    if (g_voip_out_session.busy) {
        ALOG_ERR("csapi_voip_out_open: VoIP out already active\n");
        msg_p->base.result = STE_ADM_RES_INCORRECT_STATE;
        srv_send_reply((msg_base_t*) msg_p);
        return 1;
    }

    if (!g_voip_active) {
        ALOG_ERR("csapi_voip_out_open: VoIP mode not enabled\n");
        msg_p->base.result = STE_ADM_RES_INCORRECT_STATE;
        srv_send_reply((msg_base_t*) msg_p);
        return 1;
    }

    if (msg_p->format != (ste_adm_format_t)g_voip_out_session.channels) {
        ALOG_ERR("Voip output: format must be %d (was %d)\n", g_voip_out_session.channels, msg_p->format);
        msg_p->base.result = STE_ADM_RES_INVALID_PARAMETER;
        srv_send_reply((msg_base_t*) msg_p);
        return 1;
    }

    // Connect to common mixer, since it was disconnected when call
    // graph was setup to prevent that app chain hangs due to no data
    // consumption in the vc chain.
    msg_p->base.result = dev_connect_common(g_out_dev_handle);
    if (msg_p->base.result != STE_ADM_RES_OK) {
        ALOG_ERR("voip_out_open: connect to common chain failed!\n");
        srv_send_reply((msg_base_t*) msg_p);
        return 1;
    }

    // Connect voice graph with voicecall output and echo reference port
    il_comp_t out_comp;
    OMX_U32 out_port;
    dev_get_endpoint(g_out_dev_handle, &out_comp, &out_port);

    il_comp_t echo_comp;
    OMX_U32 echo_port;
    dev_get_endpoint_echoref(g_out_dev_handle, &echo_comp, &echo_port);

    res = adm_cscall_omx_connect_voicecall_output(out_comp, out_port, echo_comp, echo_port);
    if (echo_comp) {
        ALOG_INFO("adm_cscall_omx_connect_voicecall_output %X.%d, echo ref %X.%d\n", out_comp, out_port, echo_comp, echo_port);
    } else {
        ALOG_INFO("adm_cscall_omx_connect_voicecall_output %X.%d, no echo ref\n", out_comp, out_port);
    }
    if (res != STE_ADM_RES_OK) {
        ALOG_ERR("voip_out_open: could not connect graph to voicecall output\n");
        msg_p->base.result = STE_ADM_RES_INCORRECT_STATE;
        srv_send_reply((msg_base_t*) msg_p);
        return 1;
    }

    msg_p->actual_samplerate = g_voip_out_session.samplerate;

    int fd_to_xfer;
    il_comp_t comp;
    OMX_U32 port;
    if (!adm_cscall_get_voip_out_comp(&comp, &port)) {
        ALOG_ERR("voip_out_open: cscall graph not setup!\n");
        msg_p->base.result = STE_ADM_RES_INCORRECT_STATE;
        srv_send_reply((msg_base_t*) msg_p);
        return 1;
    }

    msg_p->base.result = adm_cscall_omx_enable_voip_output(msg_p->num_bufs, msg_p->bufsz);
    if (msg_p->base.result == STE_ADM_RES_OK) {
        il_tool_get_buffers(comp, port, &fd_to_xfer, NULL, NULL);
    }

    ALOG_INFO("voip_out_open: Open done\n");
    *client_pp = (struct srv_session_s*) &g_voip_out_session;
    g_voip_out_session.busy = 1;
    srv_send_reply_and_fd((msg_base_t*) msg_p, fd_to_xfer);
    return 1;
}


void csapi_voip_in_datareq(msg_data_t* msg_p, void** client_pp)
{
    ADM_ASSERT(g_voip_active);
    ADM_ASSERT(g_voip_in_session.busy);

    (void) client_pp;
    if (g_state == CSCALL_STATE_ERROR_FATAL) {
        ALOG_ERR("voip_feed: CSCALL_STATE_ERROR_FATAL due to earlier, unrecoverable error\n");
        msg_p->base.result = STE_ADM_RES_UNRECOVERABLE_ERROR;
        srv_send_reply(&msg_p->base);
        return;
    }

    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

    if (g_state == CSCALL_STATE_DISCONNECTING || g_state == CSCALL_STATE_DISCONNECTED) {
        ALOG_INFO("voip_feed: Session finished\n");
        msg_p->base.result = STE_ADM_RES_CSCALL_FINISHED;
        srv_send_reply((msg_base_t*) msg_p);
        return;
    }

    il_comp_t comp;
    OMX_U32 port;
    if (!adm_cscall_get_voip_in_comp(&comp, &port)) {
        ALOG_ERR("voip_datareq: cscall graph not setup!\n");
        msg_p->base.result = STE_ADM_RES_ERR_MSG_IO;
        srv_send_reply((msg_base_t*) msg_p);
        return;
    }

    il_tool_io_request_data(comp, port, msg_p);
}

void csapi_voip_out_feed(msg_data_t* msg_p, void** client_pp)
{
    ADM_ASSERT(g_voip_active);
    ADM_ASSERT(g_voip_out_session.busy);

    (void) client_pp;
    if (g_state == CSCALL_STATE_ERROR_FATAL) {
        ALOG_ERR("voip_feed: CSCALL_STATE_ERROR_FATAL due to earlier, unrecoverable error\n");
        msg_p->base.result = STE_ADM_RES_UNRECOVERABLE_ERROR;
        srv_send_reply(&msg_p->base);
        return;
    }

    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

    if (g_state == CSCALL_STATE_DISCONNECTING || g_state == CSCALL_STATE_DISCONNECTED) {
        ALOG_INFO("voip_feed: Session finished\n");
        msg_p->base.result = STE_ADM_RES_CSCALL_FINISHED;
        srv_send_reply((msg_base_t*) msg_p);
        return;
    }

    il_comp_t comp;
    OMX_U32 port;
    if (!adm_cscall_get_voip_out_comp(&comp, &port)) {
        ALOG_ERR("voip_feed: cscall graph not setup!\n");
        msg_p->base.result = STE_ADM_RES_ERR_MSG_IO;
        srv_send_reply((msg_base_t*) msg_p);
        return;
    }

    il_tool_io_feed(comp, port, msg_p);
}

// Drain is asynchronous.
void csapi_voip_out_drain(msg_base_t* msg_p, void** client_pp)
{
    ADM_ASSERT(g_voip_active);
    ADM_ASSERT(g_voip_out_session.busy);

    (void) client_pp;

    il_comp_t comp;
    OMX_U32 port;
    if (!adm_cscall_get_voip_out_comp(&comp, &port)) {
        ALOG_ERR("voip_out_drain: cscall graph not setup!\n");
        msg_p->result = STE_ADM_RES_ERR_MSG_IO;
        srv_send_reply(msg_p);
        return;
    }

    if (il_tool_io_is_idle(comp, port)) {
        ALOG_INFO("voip_out_drain - already idle, sending immediate response\n");
        srv_send_reply(msg_p);
    } else {
        ALOG_INFO("voip_out_drain - not idle yet, postponing response...\n");
        g_voip_out_session.drain_msg_p = msg_p;
        il_tool_io_subscribe_idle_callback_once(comp, port, voip_out_io_idle_callback, NULL);
        // reply will be sent when IO is idle
    }
}

static void voip_out_io_idle_callback(void* param)
{
    ADM_ASSERT(g_voip_active);
    ADM_ASSERT(g_voip_out_session.busy);

    (void) param;
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

    ADM_ASSERT(g_voip_out_session.drain_msg_p);
    srv_send_reply(g_voip_out_session.drain_msg_p);
    g_voip_out_session.drain_msg_p = NULL;
}

///////////////////////////////////////////////////////////////////////////////
//
// Mute and volumes
//
///////////////////////////////////////////////////////////////////////////////
//
//
//
//
//
//
//
//
///////////////////////////////////////////////////////////////////////////////

static ste_adm_res_t csapi_apply_vc_up_vol_mute()
{
    if (g_state == CSCALL_STATE_CONNECTED)
    {
        int up_vol = g_vc_upstream_mute?INT_MIN:g_vc_upstream_volume;

        ALOG_INFO("csapi_apply_vc_up_vol_mute: Setting upstream_vol on %s to %d\n", g_cur_indev, up_vol);

        ste_adm_res_t res = dev_set_volume(g_cur_indev, 1, up_vol, 1);

        if (res != STE_ADM_RES_OK)
        {
            ALOG_ERR("csapi_apply_vc_up_vol_mute: failed for %s error = %d\n", g_cur_indev, res);
            return res;
        }
    }
    else
    {
        ALOG_INFO("csapi_apply_vc_up_vol_mute: Not connected\n");
    }

    return STE_ADM_RES_OK;
}

static ste_adm_res_t csapi_apply_vc_down_vol_mute()
{
    if (g_state == CSCALL_STATE_CONNECTED)
    {
        if (adm_get_modem_type() == ADM_FAT_MODEM) {
            if (g_vc_downstream_volume < g_vc_downstream_min_volume) {
                 g_vc_downstream_volume = g_vc_downstream_min_volume;
            }

            if (g_vc_downstream_volume > g_vc_downstream_max_volume) {
                 g_vc_downstream_volume = g_vc_downstream_max_volume;
            }

            if (g_voip_active) {
                float volume = ((float)(g_vc_downstream_volume-g_vc_downstream_min_volume));
                volume /= g_vc_downstream_max_volume-g_vc_downstream_min_volume;
                int down_vol = (int)((1-volume)*-2500);

                ALOG_INFO("csapi_apply_vc_down_vol_mute: Setting downsream_vol on %s to %d (%f)\n", g_cur_outdev, down_vol, volume);

                ste_adm_res_t res = dev_set_volume(g_cur_outdev, 1, down_vol, 1);

                if (res != STE_ADM_RES_OK) {
                    ALOG_ERR("csapi_apply_vc_down_vol_mute: failed for %s error = %d\n", g_cur_outdev, res);
                    return res;
                }
            } else {
                int down_vol = g_vc_downstream_mute?0:(g_vc_downstream_volume*8+(g_vc_downstream_max_volume-g_vc_downstream_min_volume)/2)/(g_vc_downstream_max_volume-g_vc_downstream_min_volume);

                ALOG_INFO("csapi_apply_vc_down_vol_mute: Setting downsream_vol on modem to %d\n", down_vol);

                ste_adm_res_t res = modem_set_volume(down_vol);

                if (res != STE_ADM_RES_OK) {
                    ALOG_ERR("csapi_apply_vc_down_vol_mute: failed to apply modem volume, error= %d\n", res);
                    return res;
                }
            }
        } else {
            int down_vol = g_vc_downstream_mute?INT_MIN:g_vc_downstream_volume;

            ALOG_INFO("csapi_apply_vc_down_vol_mute: Setting downsream_vol on %s to %d\n", g_cur_outdev, down_vol);

            ste_adm_res_t res = dev_set_volume(g_cur_outdev, 1, down_vol, 1);

            if (res != STE_ADM_RES_OK) {
                ALOG_ERR("csapi_apply_vc_down_vol_mute: failed for %s error = %d\n", g_cur_outdev, res);
                return res;
            }
        }
    }
    else
    {
        ALOG_INFO("csapi_apply_vc_down_vol_mute: Not connected\n");
    }

    return STE_ADM_RES_OK;
}

static ste_adm_res_t csapi_apply_vc_vol_mute()
{
    if (is_graph_using_d2d(g_cur_indev,g_cur_outdev)) 
        return STE_ADM_RES_OK;
    if (g_state == CSCALL_STATE_CONNECTED)
    {
        int up_vol = g_vc_upstream_mute?INT_MIN:g_vc_upstream_volume;
        int down_vol = g_vc_downstream_mute?INT_MIN:g_vc_downstream_volume;

        ALOG_INFO("csapi_apply_vc_vol_mute: Setting upstream_vol on %s to %d, downstream_vol on %s %d\n", g_cur_indev, up_vol, g_cur_outdev, down_vol);

        ste_adm_res_t res = csapi_apply_vc_up_vol_mute();

        if (res != STE_ADM_RES_OK)
        {
            ALOG_ERR("csapi_apply_vc_vol_mute: failed for %s error = %d\n", g_cur_indev, res);
            return res;
        }

        res = csapi_apply_vc_down_vol_mute();

        if (res != STE_ADM_RES_OK)
        {
            ALOG_ERR("csapi_apply_vc_vol_mute: failed for %s error = %d\n", g_cur_outdev, res);
            return res;
        }

    }
    else
    {
        ALOG_INFO("csapi_apply_vc_vol_mute: Not connected\n");
    }

    return STE_ADM_RES_OK;
}

void csapi_set_upstream_mute(msg_cscall_mute_t* msg_p, srv_session_t** client_pp)
{
    (void) client_pp; // No state
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

    ALOG_INFO("csapi_set_upstream_mute: mute = %d\n", msg_p->mute);

    g_vc_upstream_mute = msg_p->mute;
    ste_adm_res_t res = csapi_apply_vc_up_vol_mute();

    if (res != STE_ADM_RES_OK)
    {
        ALOG_ERR("csapi_set_upstream_mute: Failed error = %d\n", res);
        msg_p->base.result = res;
    }

    srv_send_reply((msg_base_t*)msg_p);
}

void csapi_set_downstream_mute(msg_cscall_mute_t* msg_p, srv_session_t** client_pp)
{
    (void) client_pp; // No state
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

    ALOG_INFO("csapi_set_downstream_mute: mute = %d\n", msg_p->mute);

    g_vc_downstream_mute = msg_p->mute;
    ste_adm_res_t res = csapi_apply_vc_down_vol_mute();

    if (res != STE_ADM_RES_OK)
    {
        ALOG_ERR("csapi_set_downstream_mute: Failed error = %d\n", res);
        msg_p->base.result = res;
    }
    srv_send_reply((msg_base_t*)msg_p);
}

void csapi_get_upstream_mute(msg_cscall_mute_t* msg_p, srv_session_t** client_pp)
{
    (void) client_pp; // No state
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

    msg_p->mute = g_vc_upstream_mute;

    ALOG_INFO("csapi_get_upstream_mute: mute = %d\n", msg_p->mute);

    srv_send_reply((msg_base_t*)msg_p);
}

void csapi_get_downstream_mute(msg_cscall_mute_t* msg_p, srv_session_t** client_pp)
{
    (void) client_pp; // No state
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

    msg_p->mute = g_vc_downstream_mute;

    ALOG_INFO("csapi_get_downstream_mute: mute = %d\n", msg_p->mute);

    srv_send_reply((msg_base_t*)msg_p);
}

void csapi_set_upstream_volume(msg_cscall_volume_t* msg_p, srv_session_t** client_pp)
{
    (void) client_pp; // No state
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

    ALOG_INFO("csapi_set_upstream_volume: volume = %d\n", msg_p->volume);

    g_vc_upstream_volume = msg_p->volume;
    ste_adm_res_t res = csapi_apply_vc_up_vol_mute();

    if (res != STE_ADM_RES_OK)
    {
        ALOG_ERR("csapi_set_upstream_volume: Failed error = %d\n", res);
        msg_p->base.result = res;
    }

    srv_send_reply((msg_base_t*)msg_p);
}

void csapi_set_downstream_volume(msg_cscall_volume_t* msg_p, srv_session_t** client_pp)
{
    (void) client_pp; // No state
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

    ALOG_INFO("csapi_set_downstream_volume: volume = %d\n", msg_p->volume);

    g_vc_downstream_volume = msg_p->volume;
    ste_adm_res_t res = csapi_apply_vc_down_vol_mute();

    if (res != STE_ADM_RES_OK)
    {
        ALOG_ERR("csapi_set_downstream_volume: Failed error = %d\n", res);
        msg_p->base.result = res;
    }
    srv_send_reply((msg_base_t*)msg_p);
}

void csapi_init_downstream_volume(msg_cscall_init_volume_t* msg_p, srv_session_t** client_pp)
{
    (void) client_pp; // No state
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

    ALOG_INFO("csapi_init_downstream_volume: min = %d, max = %d\n", msg_p->min, msg_p->max);

    if (msg_p->min >= msg_p->max) {
        msg_p->base.result = STE_ADM_RES_INVALID_PARAMETER;
        ALOG_ERR("csapi_init_downstream_volume: Failed error = %d\n", msg_p->base.result);
    } else {
        g_vc_downstream_min_volume = msg_p->min;
        g_vc_downstream_max_volume = msg_p->max;

        if (g_vc_downstream_volume < g_vc_downstream_min_volume) {
             g_vc_downstream_volume = g_vc_downstream_min_volume;
        }

        if (g_vc_downstream_volume > g_vc_downstream_max_volume) {
             g_vc_downstream_volume = g_vc_downstream_max_volume;
        }
    }

    srv_send_reply((msg_base_t*)msg_p);
}

void csapi_get_upstream_volume(msg_cscall_volume_t* msg_p, srv_session_t** client_pp)
{
    (void) client_pp; // No state
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

    msg_p->volume = g_vc_upstream_volume;

    ALOG_INFO("csapi_get_upstream_volume: volume = %d\n", msg_p->volume);

    srv_send_reply((msg_base_t*)msg_p);
}

void csapi_get_downstream_volume(msg_cscall_volume_t* msg_p, srv_session_t** client_pp)
{
    (void) client_pp; // No state
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

    msg_p->volume = g_vc_downstream_volume;

    ALOG_INFO("csapi_get_downstream_volume: volume = %d\n", msg_p->volume);

    srv_send_reply((msg_base_t*)msg_p);
}



///////////////////////////////////////////////////////////////////////////////
//
// TTY layer
//
///////////////////////////////////////////////////////////////////////////////
//
// Allows users to override ADM behavior. The unfortunate reason this is needed
// is that AT sends the enable/disable TTY commands, and AT is in the STELP
// layer and does not want to communicate with Android. Android in turn does
// not want to communicate with AT. Thus, the ADM needs to be the point
// that joins these two independent unsynchronized APIs.
//
// if (tty active) AND (user has requested) voicecall
//    request voicecall using TTY devices
//
///////////////////////////////////////////////////////////////////////////////



void csapi_set_devices(msg_cscall_devices_t* msg_p, srv_session_t** client_pp)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    SRV_DBG_ASSERT_IS_SERIALIZED_REQUEST;

    if ( (msg_p->indev[0]  && !msg_p->outdev[0]) ||
         (!msg_p->indev[0] && msg_p->outdev[0]))
    {
        ALOG_ERR("Partial cscall not supported ('%s', '%s')\n", msg_p->indev, msg_p->outdev);
        msg_p->base.result = STE_ADM_RES_PARTIAL_CSCALL_NOT_SUPPORTED;
        srv_send_reply(&msg_p->base);
        return;
    }

    if (g_state == CSCALL_STATE_DISABLED) {
        ALOG_ERR("csapi_set_devices: CSCall disabled (likely due to modem problems)\n");
        msg_p->base.result = STE_ADM_RES_CSCALL_DISABLED;
        srv_send_reply(&msg_p->base);
        return;
    }

    if (!msg_p->indev[0]) {
        if (g_voip_in_session.busy || g_voip_out_session.busy) {
            ALOG_ERR("Call can't be ended while voip device is active! in=%d out=%d\n", g_voip_in_session.busy, g_voip_out_session.busy);
            msg_p->base.result = STE_ADM_RES_INCORRECT_STATE;
            srv_send_reply(&msg_p->base);
            return;
        }
    }
    int type = adm_get_modem_type();
    if ((type==ADM_FAT_MODEM) || (type==ADM_FULL_FAT_MODEM)){
        if(modem_get_state()!=ADM_MODEM_STATE_ON) {
            ALOG_ERR("csapi_set_devices: modem is not ON\n");
            msg_p->base.result = STE_ADM_RES_CSCALL_DISABLED;
            srv_send_reply(&msg_p->base);
            return;
        }
    }

    if (g_state == CSCALL_STATE_RETRY) {
        // It makes sense for the user if it starts working after a re-route request.
        ALOG_INFO("csapi_set_devices: In state CSCALL_STATE_RETRY, changing to state CSCALL_STATE_DISCONNECTED\n");
        g_state = CSCALL_STATE_DISCONNECTED;
    }


    msg_p->base.result = csapi_set_devices_internal(msg_p->indev, msg_p->outdev, client_pp);
    srv_send_reply((msg_base_t*) msg_p);
}


void csapi_set_tty(msg_cscall_tty_t* msg_p, struct srv_session_s** client_pp)
{
    (void) client_pp; // No state

    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    SRV_DBG_ASSERT_IS_SERIALIZED_REQUEST;

    ALOG_STATUS("csapi_set_tty mode: %d\n", msg_p->tty);

    if (g_tty_mode != msg_p->tty) {
        g_tty_mode = msg_p->tty;

        if (g_cur_indev[0]) {
            // Voicecall requested, so change to request TTY
            reroute_live(g_cur_indev, g_cur_outdev, 1, 1);
        }
        else
        {
            ste_adm_res_t res = dev_set_tty_mode(g_tty_mode);
            if (res != STE_ADM_RES_OK) {
                ALOG_ERR("csapi_set_tty: dev_set_tty_mode failed\n");
                g_state = CSCALL_STATE_ERROR_FATAL;
                return;
            }
        }
    }

    srv_send_reply((msg_base_t*) msg_p);
}



void csapi_get_modem_type(msg_get_modem_type_t* msg_p, struct srv_session_s** client_pp)
{
    (void) client_pp; // No state
    msg_p->modem_type = adm_get_modem_type();
    srv_send_reply((msg_base_t*) msg_p);
}

void csapi_get_tty(msg_cscall_tty_t* msg_p, struct srv_session_s** client_pp)
{
    (void) client_pp; // No state
    msg_p->tty = g_tty_mode;
    srv_send_reply((msg_base_t*) msg_p);
}

void csapi_set_voip(msg_cscall_voip_t* msg_p, struct srv_session_s** client_pp)
{
    (void) client_pp; // No state

    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    SRV_DBG_ASSERT_IS_SERIALIZED_REQUEST;

    if (g_voip_in_session.busy || g_voip_out_session.busy) {
        ALOG_ERR("voip mode can't be changed while voip device is active! in=%d out=%d\n", g_voip_in_session.busy, g_voip_out_session.busy);
        msg_p->base.result = STE_ADM_RES_INCORRECT_STATE;
        srv_send_reply(&msg_p->base);
        return;
    }

    if (g_state != CSCALL_STATE_DISCONNECTED &&
        g_state != CSCALL_STATE_DISABLED)
    {
        ALOG_ERR("voip mode can't be changed in %s\n", cscall_state2str(g_state));
        msg_p->base.result = STE_ADM_RES_INCORRECT_STATE;
        srv_send_reply(&msg_p->base);
        return;
    }

    ALOG_STATUS("CSCALL VoIP: %s %s\n", msg_p->voip_enabled ? "ENABLE" : "DISABLE", cscall_state2str(g_state));

    if (msg_p->voip_enabled) {
        // Check sample rate
        if (msg_p->in_samplerate != 0) {
            if (msg_p->in_samplerate > STE_ADM_CSCALL_MAX_VOIP_SAMPLERATE) {
                g_voip_in_session.samplerate = STE_ADM_CSCALL_MAX_VOIP_SAMPLERATE;
            } else {
                g_voip_in_session.samplerate = msg_p->in_samplerate;
            }
        }
        if(msg_p->in_channels != 0) {
            if (msg_p->in_channels > STE_ADM_CSCALL_MAX_VOIP_CHANNELS) {
                g_voip_in_session.channels = STE_ADM_CSCALL_MAX_VOIP_CHANNELS;
            } else {
                g_voip_in_session.channels = msg_p->in_channels;
            }
        }
        if (msg_p->out_samplerate != 0) {
            if(msg_p->out_samplerate > STE_ADM_CSCALL_MAX_VOIP_SAMPLERATE) {
                g_voip_out_session.samplerate = STE_ADM_CSCALL_MAX_VOIP_SAMPLERATE;
            } else {
                g_voip_out_session.samplerate = msg_p->out_samplerate;
            }
        }
        if(msg_p->out_channels != 0) {
            if (msg_p->out_channels > STE_ADM_CSCALL_MAX_VOIP_CHANNELS) {
                g_voip_out_session.channels = STE_ADM_CSCALL_MAX_VOIP_CHANNELS;
            } else {
                g_voip_out_session.channels = msg_p->out_channels;
            }
        }

        if (g_voip_in_session.samplerate == 0) {
           g_voip_in_session.samplerate = g_voip_out_session.samplerate;
        }
        if (g_voip_in_session.channels == 0) {
           g_voip_in_session.channels = g_voip_out_session.channels;
        }
        if (g_voip_out_session.samplerate == 0) {
           g_voip_out_session.samplerate = g_voip_in_session.samplerate;
        }
        if (g_voip_out_session.channels == 0) {
           g_voip_out_session.channels = g_voip_in_session.channels;
        }

        adm_cscall_omx_enable_voip(
            g_voip_in_session.samplerate, g_voip_in_session.channels,
            g_voip_out_session.samplerate, g_voip_out_session.channels);
        g_voip_active = 1;
    } else {
        adm_cscall_omx_disable_voip();
        g_voip_active = 0;
        g_voip_in_session.samplerate = 0;
        g_voip_out_session.samplerate = 0;
        g_voip_in_session.channels = 0;
        g_voip_out_session.channels = 0;
    }

    srv_send_reply((msg_base_t*) msg_p);
}


///////////////////////////////////////////////////////////////////////////////
//
// Database rescan
//
///////////////////////////////////////////////////////////////////////////////
//
//
//
//
//
//
//
//
//
//
///////////////////////////////////////////////////////////////////////////////



static unsigned int tv_ms_diff(struct timeval a, struct timeval b)
{
  return (unsigned int) (1000 * (a.tv_sec - b.tv_sec) + (a.tv_usec - b.tv_usec) / 1000);
}


static ste_adm_res_t csapi_remap_verify(const char* toplevel,
                                        const char* toplevel2,
                                        const char* actual,
                                        const char* actual2,
                                        const char* cur_device,
                                        const char* cur_device2)
{
    struct timeval tv_start;
    gettimeofday(&tv_start, NULL);


    int is_input;
    ste_adm_res_t res = adm_db_io_info(toplevel, &is_input);
    if (res != STE_ADM_RES_OK) {
        ALOG_ERR("adm_db_io_info failed in csiapi_remap_verify for %s\n", toplevel);
        return res;
    }

    int is_input2;
    if (toplevel2[0]) {
        ste_adm_res_t res = adm_db_io_info(toplevel2, &is_input2);
        if (res != STE_ADM_RES_OK) {
            ALOG_ERR("adm_db_io_info failed in csiapi_remap_verify for %s\n", toplevel2);
            return res;
        }

        if ( (is_input && is_input2) || (!is_input && !is_input2)) {
            ALOG_ERR("csapi_remap_verify: One device must be input, one output, if two\n");
            return STE_ADM_RES_INVALID_PARAMETER;
        }
    }

    //
    // Get current voicecall devices
    //
    char cur_vc_in_device[STE_ADM_MAX_DEVICE_NAME_LENGTH+1]  = "";
    char cur_vc_out_device[STE_ADM_MAX_DEVICE_NAME_LENGTH+1] = "";
    if (g_cur_indev[0]) {
        ALOG_INFO("g_cur_indev=%s g_cur_outdev=%s\n", g_cur_indev, g_cur_outdev);

        const char* new_indev = NULL;
        const char* new_outdev = NULL;
        if (is_input) {
            new_indev = toplevel;
            new_outdev = toplevel2;
        } else {
            new_indev = toplevel2;
            new_outdev = toplevel;
        }
        if (strcmp(g_cur_indev, new_indev) == 0 ||
            strcmp(g_cur_outdev, new_outdev) == 0) {
            const char* tmp_str;
            res = adm_db_toplevel_mapping_get(g_cur_indev,  &tmp_str);
            if (res != STE_ADM_RES_OK) {
                ALOG_ERR("adm_db_toplevel_mapping_get failed for %s\n", g_cur_indev);
                return res;
            }
            strcpy(cur_vc_in_device, tmp_str);

            res = adm_db_toplevel_mapping_get(g_cur_outdev, &tmp_str);
            if (res != STE_ADM_RES_OK) {
                ALOG_ERR("adm_db_toplevel_mapping_get failed for %s\n", g_cur_outdev);
            }
            strcpy(cur_vc_out_device, tmp_str);
        }
    }


    res = adm_db_remap_verify(is_input, cur_vc_in_device, cur_vc_out_device,
                        adm_cscall_omx_get_samplerate(),
                        cur_device,
                        actual,
                        cur_device2,
                        actual2);

    if (res != STE_ADM_RES_OK) {
        ALOG_INFO("Device configurations are not compatible with live rescan\n");
        ALOG_INFO("Cur device = [%s,%s]   New device=[%s,%s]   VC=[%s,%s] SR=%d\n",
            cur_device, cur_device2, actual, actual2, cur_vc_in_device, cur_vc_out_device, adm_cscall_omx_get_samplerate());
        return res;
    }



    struct timeval tv_end;
    gettimeofday(&tv_end, NULL);
    unsigned int time_used = tv_ms_diff(tv_end, tv_start);
    ALOG_INFO("Verified database configuration in %u ms\n",time_used);
    return STE_ADM_RES_OK;
}

static ste_adm_res_t csapi_remap_rescan(const char* toplevel, const char* toplevel2)
{
    ALOG_INFO("Rescanning application chain for device %s\n", toplevel);
    ste_adm_res_t res = dev_rescan_effects(STE_ADM_APPLICATION_CHAIN, toplevel, NULL);
    if (res != STE_ADM_RES_OK) {
        ALOG_ERR("Error while rescanning application chain for device %s\n", toplevel);
    }

    if (res == STE_ADM_RES_OK) {
        ALOG_INFO("Rescanning common chain for device %s\n", toplevel);
        res = dev_rescan_effects(STE_ADM_COMMON_CHAIN, toplevel, NULL);
        if (res != STE_ADM_RES_OK) {
            ALOG_ERR("Error while rescanning common chain for device %s\n", toplevel);
        }
    }

    if (res == STE_ADM_RES_OK) {
        ALOG_INFO("Rescanning hw handler for device %s\n", toplevel);
        res = dev_rescan_hw_handler(toplevel, FADESPEED_SLOW);
        if (res != STE_ADM_RES_OK) {
            ALOG_ERR("Error while rescanning hw handler for device %s\n", toplevel);
        }
    }

    if (res == STE_ADM_RES_OK && toplevel2[0]) {
        ALOG_INFO("Rescanning application chain for device %s\n", toplevel2);
        ste_adm_res_t res = dev_rescan_effects(STE_ADM_APPLICATION_CHAIN, toplevel2, NULL);
        if (res != STE_ADM_RES_OK) {
            ALOG_ERR("Error while rescanning application chain for device %s\n", toplevel2);
        }

        if (res == STE_ADM_RES_OK) {
            ALOG_INFO("Rescanning common chain for device %s\n", toplevel2);
            res = dev_rescan_effects(STE_ADM_COMMON_CHAIN, toplevel2, NULL);
            if (res != STE_ADM_RES_OK) {
                ALOG_ERR("Error while rescanning common chain for device %s\n", toplevel2);
            }
        }

        if (res == STE_ADM_RES_OK) {
            ALOG_INFO("Rescanning hw handler for device %s\n", toplevel2);
            res = dev_rescan_hw_handler(toplevel2, FADESPEED_SLOW);
            if (res != STE_ADM_RES_OK) {
                ALOG_ERR("Error while rescanning hw handler for device %s\n", toplevel2);
            }
        }
    }

    if (res == STE_ADM_RES_OK && g_cur_indev[0] && g_state == CSCALL_STATE_CONNECTED) {
        if (strcmp(g_cur_indev,  toplevel) == 0 ||
            strcmp(g_cur_outdev, toplevel) == 0 ||
            strcmp(g_cur_indev,  toplevel2) == 0 ||
            strcmp(g_cur_outdev, toplevel2) == 0)
        {
            // Need to rescan both input and output, since VC chain config depends
            // on both input and output mapping
            const char* next_indev = NULL;
            const char* next_outdev = NULL;
            if (strcmp(g_cur_indev, toplevel) == 0 ||
                strcmp(g_cur_outdev, toplevel2) == 0) {
                next_indev = toplevel;
                next_outdev = toplevel2;
            } else {
                next_indev = toplevel2;
                next_outdev = toplevel;
            }

            ALOG_INFO("Rescanning voice chain for device %s\n", next_indev);
            res = dev_rescan_effects(STE_ADM_VOICE_CHAIN, next_indev, next_outdev);
            if (res != STE_ADM_RES_OK) {
                ALOG_ERR("Error while rescanning voice chain for %s in pair [%s,%s]\n", next_indev, next_indev, next_outdev);
            }

            if (res == STE_ADM_RES_OK) {
                ALOG_INFO("Rescanning voice chain for device %s\n", next_outdev);
                res = dev_rescan_effects(STE_ADM_VOICE_CHAIN, next_outdev, next_indev);
                if (res != STE_ADM_RES_OK) {
                    ALOG_ERR("Error while rescanning voice chain for %s in pair [%s,%s]\n", next_outdev, next_indev, next_outdev);
                }
            }

            // Speech proc and HW handler VC are rescanned in reroute_live

        } else {
            ALOG_INFO("csapi_remap_live: VC not affected, not re-scanning speech proc and vc chain\n");
        }
    }

    return res;
}

static ste_adm_res_t csapi_remap_recreate(const char* toplevel, const char* toplevel2)
{
    ste_adm_res_t res = STE_ADM_RES_OK;
    int is_vc = 0;

    if (toplevel2[0]) {
        ALOG_INFO("Recreate effect chains for devices %s and %s\n", toplevel, toplevel2);
        /* For voice call both input and output devices are provided */
        is_vc = 1;
    } else {
        ALOG_INFO("Recreate effect chains for device %s\n", toplevel);
    }

    ALOG_INFO_VERBOSE("g_voip_active=%d, g_voip_out_session.busy=%d, g_voip_in_session.busy=%d\n", g_voip_active, g_voip_out_session.busy, g_voip_in_session.busy);

    int vc_in_disconnected = 0;
    int vc_out_disconnected = 0;
    if (is_vc) {
        if ((strcmp(g_cur_indev, toplevel) == 0 || strcmp(g_cur_indev, toplevel2) == 0) &&
            (g_voip_active && !g_voip_in_session.busy)) {
            /* If VoIP active and VoIP In not busy the voice effect chain
               is already disconnected */
            vc_in_disconnected = 1;
        }
        if ((strcmp(g_cur_outdev, toplevel) == 0 || strcmp(g_cur_outdev, toplevel2) == 0) &&
            (g_voip_active && !g_voip_out_session.busy)) {
            /* If VoIP active and VoIP Out not busy the voice effect chain
               is already disconnected */
            vc_out_disconnected = 1;
        }

        if (g_state == CSCALL_STATE_CONNECTED) {
            // Disconnect from speech_proc
            if (strcmp(g_cur_indev, toplevel) == 0 || strcmp(g_cur_indev, toplevel2) == 0) {
                if (!vc_in_disconnected) {
                    res = adm_cscall_omx_disconnect_voicecall_input();
                    if (res != STE_ADM_RES_OK) {
                        ALOG_ERR("Failed to disconnect voicecall input");
                        return res;
                    }
                }
            }
            if (strcmp(g_cur_outdev, toplevel) == 0 || strcmp(g_cur_outdev, toplevel2) == 0) {
                if (!vc_out_disconnected) {
                    res = adm_cscall_omx_disconnect_voicecall_output();
                    if (res != STE_ADM_RES_OK) {
                        ALOG_ERR("Failed to disconnect voicecall output");
                        return res;
                    }
                }
            }
        }
    }

    // Reconfigure the effect chains of the device
    res = dev_reconfigure_effects(toplevel, toplevel2, vc_in_disconnected, vc_out_disconnected);
    if (res != STE_ADM_RES_OK) {
        ALOG_ERR("Failed to reconfigure effects");
        return res;
    }


    if (is_vc && g_state == CSCALL_STATE_CONNECTED) {

        il_comp_t in_comp;
        OMX_U32 in_port;
        dev_get_endpoint(g_in_dev_handle, &in_comp, &in_port);

        il_comp_t out_comp;
        OMX_U32 out_port;
        dev_get_endpoint(g_out_dev_handle, &out_comp, &out_port);

        il_comp_t echo_comp;
        OMX_U32 echo_port;
        dev_get_endpoint_echoref(g_out_dev_handle, &echo_comp, &echo_port);


        if (g_cur_indev[0]) {
            if (strcmp(g_cur_indev,  toplevel) == 0 ||
                strcmp(g_cur_outdev, toplevel) == 0 ||
                strcmp(g_cur_indev,  toplevel2) == 0 ||
                strcmp(g_cur_outdev, toplevel2) == 0)
            {
                // Need to rescan both input and output, since VC chain config depends
                // on both input and output mapping
                const char* next_indev = NULL;
                const char* next_outdev = NULL;
                if (strcmp(g_cur_indev, toplevel) == 0 ||
                    strcmp(g_cur_outdev, toplevel2) == 0) {
                    next_indev = toplevel;
                    next_outdev = toplevel2;
                } else {
                    next_indev = toplevel2;
                    next_outdev = toplevel;
                }

                // Connect to speech_proc again
                if (strcmp(g_cur_indev, toplevel) == 0 || strcmp(g_cur_indev, toplevel2) == 0) {
                    if (!vc_in_disconnected) {
                        res = adm_cscall_omx_connect_voicecall_input(in_comp, in_port);
                        if (res != STE_ADM_RES_OK) {
                            ALOG_ERR("Failed to connect voicecall input");
                            return res;
                        }
                    }
                }
                if (strcmp(g_cur_outdev, toplevel) == 0 || strcmp(g_cur_outdev, toplevel2) == 0) {
                    if (!vc_out_disconnected) {
                        res = adm_cscall_omx_connect_voicecall_output(out_comp, out_port, echo_comp, echo_port);
                        if (res != STE_ADM_RES_OK) {
                            ALOG_ERR("Failed to connect voicecall output");
                            return res;
                        }
                    }
                }

                // Speech proc and HW handler VC are rescanned in reroute_live

            }
        }
    }

    // Rescan hw handler parameters
    ALOG_INFO_VERBOSE("Rescan hw handler for %s\n", toplevel);
    res = dev_rescan_hw_handler(toplevel, FADESPEED_SLOW);
    if (res != STE_ADM_RES_OK) {
        ALOG_ERR("Failed to rescan hw handler for %s\n", toplevel);
        return res;
    }
    if (toplevel2[0]) {
        ALOG_INFO_VERBOSE("Rescan hw handler for %s\n", toplevel2);
        res = dev_rescan_hw_handler(toplevel2, FADESPEED_SLOW);
        if (res != STE_ADM_RES_OK) {
            ALOG_ERR("Failed to rescan hw handler for %s\n", toplevel2);
            return res;
        }
    }

    return res;
}



void csapi_remap(msg_set_toplevel_map_t* msg_p, struct srv_session_s** client_pp)
{
    (void) client_pp; // No state
    if (!msg_p->toplevel2[0]) {
        ALOG_STATUS("Remap %s to %s. %s\n", msg_p->toplevel, msg_p->actual, msg_p->rescan ? "RESCAN " : "");
    } else {
        ALOG_STATUS("Remap [%s,%s] to [%s,%s]. %s\n", msg_p->toplevel, msg_p->toplevel2, msg_p->actual, msg_p->actual2, msg_p->rescan ? "RESCAN " : "");
    }


    srv_become_worker_thread();

    const char* tmp_str;
    char cur_device[STE_ADM_MAX_DEVICE_NAME_LENGTH+1];
    char cur_device2[STE_ADM_MAX_DEVICE_NAME_LENGTH+1] = "";

    // tmp_str is valid until next time adm_db_toplevel_mapping_get is called
    ste_adm_res_t res = adm_db_toplevel_mapping_get(msg_p->toplevel, &tmp_str);
    if (res == STE_ADM_RES_OK) {
        strcpy(cur_device, tmp_str);
    }

    if (res == STE_ADM_RES_OK && msg_p->toplevel2[0]) {
        res = adm_db_toplevel_mapping_get(msg_p->toplevel2, &tmp_str);
        if (res == STE_ADM_RES_OK) {
            strcpy(cur_device2, tmp_str);
        }
    }

    if (res == STE_ADM_RES_OK) {
        if (strcmp(cur_device, msg_p->actual) == 0 &&
            (msg_p->toplevel2[0] == 0 || strcmp(cur_device2, msg_p->actual2) == 0))
        {
            ALOG_INFO("Remapping to same device(s) as currently active, doing nothing\n");
            srv_become_normal_thread();
        } else {
            int compatible_effect_chains = 0;
            if (msg_p->rescan) {
                res = csapi_remap_verify(msg_p->toplevel, msg_p->toplevel2,
                                         msg_p->actual,   msg_p->actual2,
                                         cur_device,      cur_device2);
                if (res == STE_ADM_RES_OK) {
                    compatible_effect_chains = 1;
                }
            }

            res = adm_db_toplevel_mapping_set(msg_p->toplevel, msg_p->actual);
            if (res == STE_ADM_RES_OK && msg_p->toplevel2[0]) {
                res = adm_db_toplevel_mapping_set(msg_p->toplevel2, msg_p->actual2);
            }
            srv_become_normal_thread();
            if (res != STE_ADM_RES_OK) {
                ALOG_ERR("Failed to set toplevel map");
            } else {

                struct timeval tv_start;
                gettimeofday(&tv_start, NULL);

                if (msg_p->rescan) {
                    if (compatible_effect_chains) {
                        ALOG_INFO("Effect chains are compatible, reconfigure current effect chains");
                        res = csapi_remap_rescan(msg_p->toplevel, msg_p->toplevel2);
                        if (res != STE_ADM_RES_OK) {
                            ALOG_ERR("csapi_remap_rescan for %s failed, error = %d", msg_p->toplevel, res);
                        }
                    } else {
                        ALOG_INFO("Effect chains not compatible, new effect chains are connected");
                        res = csapi_remap_recreate(msg_p->toplevel, msg_p->toplevel2);
                        if (res != STE_ADM_RES_OK) {
                            ALOG_ERR("csapi_remap_recreate for %s failed, error = %d", msg_p->toplevel, res);
                        }
                    }
                }

                if (res == STE_ADM_RES_OK) {
                    struct timeval tv_end;
                    gettimeofday(&tv_end, NULL);
                    ALOG_INFO_VERBOSE("Remap (excluding verify) %s to %s completed in %u ms. %s\n",
                        msg_p->toplevel, msg_p->actual, tv_ms_diff(tv_end, tv_start), msg_p->rescan ? "RESCAN " : "");
                }
            }
        }
    } else {
        srv_become_normal_thread();
    }

    dev_notify_active_device_changes();
    msg_p->base.result = res;
    srv_send_reply(&msg_p->base);
}



void csapi_rescan_config(msg_rescan_config_t* msg_p, struct srv_session_s** client_pp)
{
    (void) client_pp; // Stateless request

    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

    char cur_indev[STE_ADM_MAX_DEVICE_NAME_LENGTH+1];
    char cur_outdev[STE_ADM_MAX_DEVICE_NAME_LENGTH+1];
    if (msg_p->chain_type == STE_ADM_VOICE_CHAIN && g_state == CSCALL_STATE_CONNECTED)
    {
        strcpy(cur_indev, g_cur_indev);
        strcpy(cur_outdev, g_cur_outdev);
    }


    struct timeval tv_start;
    gettimeofday(&tv_start, NULL);

    if (msg_p->chain_type != STE_ADM_APPLICATION_CHAIN &&
        msg_p->chain_type != STE_ADM_VOICE_CHAIN &&
        msg_p->chain_type != STE_ADM_COMMON_CHAIN)
    {
        ALOG_ERR("Invalid chain type\n");
        msg_p->base.result = STE_ADM_RES_INVALID_PARAMETER;
        goto cleanup;
    }

    if (msg_p->chain_type == STE_ADM_VOICE_CHAIN && g_state != CSCALL_STATE_CONNECTED) {
        ALOG_ERR("CSCall not active\n");
        msg_p->base.result = STE_ADM_RES_CSCALL_NOT_ACTIVE;
        goto cleanup;
    }

    if (adm_db_extname(msg_p->dev_name) != STE_ADM_RES_OK) {
        ALOG_ERR("Failed to get external name of device %s from db!\n", msg_p->dev_name);
        msg_p->base.result = STE_ADM_RES_NO_SUCH_COMPONENT;
        goto cleanup;
    }

    int is_input;
    if (adm_db_io_info(msg_p->dev_name, &is_input) != STE_ADM_RES_OK) {
        ALOG_ERR("Failed to get direction of %s from db!\n", msg_p->dev_name);
        msg_p->base.result = STE_ADM_RES_NO_SUCH_COMPONENT;
        goto cleanup;
    }
    const char* second_dev = is_input ? cur_outdev : cur_indev;

    if (msg_p->chain_type == STE_ADM_VOICE_CHAIN) {
        msg_p->base.result = dev_rescan_effects(
            STE_ADM_VOICE_CHAIN,
            msg_p->dev_name,
            second_dev);
    } else {
        msg_p->base.result = dev_rescan_effects(msg_p->chain_type, msg_p->dev_name,NULL);
    }



    if (msg_p->base.result != STE_ADM_RES_OK) {
        ALOG_ERR("RescanConfig failed with error %d.", msg_p->base.result);
        goto cleanup;
    }

    struct timeval tv_end;
    gettimeofday(&tv_end, NULL);
    unsigned time_used = tv_ms_diff(tv_end, tv_start);

    ALOG_STATUS("RescanConfig: dev=%s chain_type=%d completed in %u ms\n", msg_p->dev_name,
            msg_p->chain_type, time_used);


cleanup:

    srv_send_reply(&msg_p->base);
}




void csapi_rescan_speechproc(msg_base_t* msg_p, struct srv_session_s** client_pp)
{

    (void) client_pp; // Stateless request

    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    SRV_DBG_ASSERT_IS_SERIALIZED_REQUEST;

    if (g_state != CSCALL_STATE_CONNECTED)
    {
        ALOG_ERR("csapi_rescan_speechproc: No active cscall.\n");
        msg_p->result = STE_ADM_RES_CSCALL_NOT_ACTIVE;
        goto cleanup;
    }

    msg_p->result = adm_cscall_omx_rescan_speech_proc(g_cur_indev, g_cur_outdev);
    if (msg_p->result != STE_ADM_RES_OK)
    {
        ALOG_ERR("csapi_rescan_speechproc: rescan failed.\n");
    }

cleanup:
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    srv_send_reply(msg_p);
}

ste_adm_res_t csapi_reload_device_settings()
{
    ste_adm_res_t res = STE_ADM_RES_OK;
    if (g_state != CSCALL_STATE_CONNECTED) {
        ALOG_INFO("csapi_reload_device_settings: No active cscall.\n");
    } else if (is_app_dev_open(g_cur_indev) || is_app_dev_open(g_cur_outdev)) {
        ALOG_ERR("csapi_reload_device_settings: App dev is open for %s or %s, "
            "can't reload graph.\n", g_cur_indev, g_cur_outdev);
        res = STE_ADM_RES_INCORRECT_STATE;
    } else {
        reroute_live(g_cur_indev, g_cur_outdev, 1, 1);
        res = csapi_apply_vc_vol_mute();  // Restore volume

    }
    return res;
}

int csapi_get_samplerate()
{
    return adm_cscall_omx_get_samplerate();
}

int csapi_cscall_active()
{
    return g_state == CSCALL_STATE_CONNECTED && g_voip_active == 0;
}

#define MAX_MODEM_VC_STATE_SUBSCRIPTIONS 10

static struct modem_vc_state_subscriptions
{
    csapi_modem_vc_state_cb_fp_t cb_fp;
    void*                        param;
} g_modem_vc_state_subscriptions[MAX_MODEM_VC_STATE_SUBSCRIPTIONS];


typedef struct
{
    srv_session_t srv_session;
    int subscription_handle;
    int value_changed;                     // set to 1 if value have changed, init to 1
    msg_modem_vc_state_t* deferred_msg_p;
} modem_vc_state_t;

ste_adm_vc_modem_status_t csapi_current_vc_state(void)
{
    if (g_codec_enabled != 0 && g_freq_known != 0)
        return STE_ADM_VC_MODEM_STATUS_CONNECTED;
    else
        return STE_ADM_VC_MODEM_STATUS_DISCONNECTED;
}

void csapi_notify_vc_state_change(void)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    int i;
    for (i=0 ; i < MAX_MODEM_VC_STATE_SUBSCRIPTIONS ; i++) {
        if (g_modem_vc_state_subscriptions[i].cb_fp) {

            g_modem_vc_state_subscriptions[i].cb_fp(g_modem_vc_state_subscriptions[i].param);
        }
    }
}

void csapi_modem_vc_state_cb(void* param)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    ALOG_INFO("csapi_modem_vc_state_cb: Modem VC state changed.\n");

    modem_vc_state_t *state_p = (modem_vc_state_t*) param;

    if (state_p->deferred_msg_p)
    {
        ALOG_INFO("csapi_modem_vc_state_cb: deferred_msg_p exists.\n");
        state_p->deferred_msg_p->vc_state = csapi_current_vc_state();
        state_p->deferred_msg_p->base.result = STE_ADM_RES_OK;
        srv_send_reply(&state_p->deferred_msg_p->base);
        state_p->deferred_msg_p = NULL;
    }
    else
    {
        ALOG_INFO("csapi_modem_vc_state_cb: No queued request\n");
        state_p->value_changed = 1;
    }
}

void csapi_modem_vc_state_close(srv_session_t* param)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    modem_vc_state_t *state_p = (modem_vc_state_t*) param;

    csapi_unsubscribe_modem_vc_state(state_p->subscription_handle);

    memset(state_p, 0x98, sizeof(*state_p));
    free(state_p);
}


void csapi_request_modem_vc_state(msg_modem_vc_state_t* msg_p, srv_session_t** client_pp)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

    ALOG_INFO("csapi_request_modem_vc_state\n");
    modem_vc_state_t *state_p;

    if (*client_pp == NULL) {
        ALOG_INFO("csapi_request_modem_vc_state: Allocating new state.\n");
        state_p = malloc(sizeof(modem_vc_state_t));
        if (!state_p) {
            ALOG_ERR("csapi_request_modem_vc_state: malloc failed\n");
            msg_p->base.result = STE_ADM_RES_ERR_MALLOC;
            srv_send_reply(&msg_p->base);
            return;
        }

        state_p->srv_session.dtor_fp = csapi_modem_vc_state_close;
        state_p->srv_session.dtor_must_be_serialized = 0;
        state_p->srv_session.api_group = API_GROUP_ATT;
        state_p->value_changed = 1;
        state_p->deferred_msg_p = NULL;
        *client_pp = &state_p->srv_session;

        ste_adm_res_t res = csapi_subscribe_modem_vc_state(csapi_modem_vc_state_cb,
                                                         state_p,
                                                         &state_p->subscription_handle);
        if (res != STE_ADM_RES_OK) {
            msg_p->base.result = res;
            ALOG_ERR("csapi_request_modem_vc_state: csapi_subscribe_modem_vc_state failed: %d\n", res);
            srv_send_reply(&msg_p->base);
            return;
        }

    } else {
        ALOG_INFO("csapi_request_modem_vc_state: state exists, using old state\n");
        state_p = (modem_vc_state_t*) *client_pp;
        ADM_ASSERT(state_p->deferred_msg_p == NULL);
    }

    if (state_p->value_changed) {
        ALOG_INFO("csapi_request_modem_vc_state: devices changed\n");
        state_p->value_changed = 0;

        msg_p->vc_state = csapi_current_vc_state();
        msg_p->base.result = STE_ADM_RES_OK;
    } else {
        state_p->deferred_msg_p = msg_p;
        ALOG_INFO("csapi_request_modem_vc_state: Waiting for changes\n");
        return;
    }

    ALOG_INFO("csapi_request_modem_vc_state: sending reply\n");
    srv_send_reply(&msg_p->base);
}

ste_adm_res_t csapi_subscribe_modem_vc_state(csapi_modem_vc_state_cb_fp_t cb_fp,
                                     void* param,
                                     int* subscription_handle_p)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

    ADM_ASSERT(cb_fp);

    int i;
    for (i=0 ; i < MAX_MODEM_VC_STATE_SUBSCRIPTIONS ; i++) {
        if (!g_modem_vc_state_subscriptions[i].cb_fp) {
            g_modem_vc_state_subscriptions[i].cb_fp = cb_fp;
            g_modem_vc_state_subscriptions[i].param = param;
            *subscription_handle_p = i;
            return STE_ADM_RES_OK;
        }
    }

    return STE_ADM_RES_ALL_IN_USE;
}

void csapi_unsubscribe_modem_vc_state(int handle)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    ADM_ASSERT(handle >= 0 && handle < MAX_MODEM_VC_STATE_SUBSCRIPTIONS);
    ADM_ASSERT(g_modem_vc_state_subscriptions[handle].cb_fp);
    g_modem_vc_state_subscriptions[handle].cb_fp = NULL;
}









