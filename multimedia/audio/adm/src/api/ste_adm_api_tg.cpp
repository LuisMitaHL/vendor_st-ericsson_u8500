/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

//
// Temporarily use the Android tone generator until the
// same functionality is available on STELP level. The tone generator
// runs in a separate thread. It can not run in the ADM thread,
// since the tone generator calls will end up calling ADM again.
//

#ifdef __cplusplus
extern "C" {
#endif
#include "ste_adm_msg.h"
#include "ste_adm_srv.h"
#include "ste_adm_dbg.h"
#ifdef __cplusplus
}
#endif

#include <assert.h>
#include <stdio.h>
#include <unistd.h>

#ifndef ADM_DISABLE_TONEGEN
#include "hardware/audio_effect.h"
#include "hardware_legacy/AudioSystemLegacy.h"
#include "media/ToneGenerator.h"
#include "media/AudioSystem.h"
#include "cutils/properties.h"
#endif


#include "ste_adm_api_tg.h"


#ifndef ADM_DISABLE_TONEGEN

static int g_tg_pipe[2] = { -1, -1 };
static int g_tg_initalized = 0;

static void* tg_thread(void* args);

/*
* Initialize tone generator. Returns 0 if ok, != 0 otherwise.
* Must be called before any other functions in
* this file.
*/
static int adm_tg_init()
{
    ALOG_STATUS("adm_tg_init: Initializing tonegen.\n");
    if (pipe(g_tg_pipe)) {
        ALOG_ERR("adm_tg_init: pipe() failed\n");
        return -1;
    }

    pthread_t thread;
    if (pthread_create(&thread, NULL, tg_thread, NULL)) {
        ALOG_ERR("adm_tg_init: pthread_create() failed\n");
        return -1;
    }

    g_tg_initalized = 1;
    ALOG_STATUS("adm_tg_init: Done TG uses thread %d\n", thread);
    return 0;
}

// Returns TRUE if the requested tone standard is the active tone standard
// in the system.
static int is_standard_active(ste_adm_comfort_tone_standard_t wanted_standard)
{
    char prop[PROPERTY_VALUE_MAX];
    property_get("gsm.operator.iso-country", prop, "");

    ste_adm_comfort_tone_standard_t current_standard;
    if (strcmp(prop, "jp") == 0)
        current_standard = STE_ADM_COMFORT_TONE_STANDARD_JAPAN;
    else if (strcmp(prop, "ca") == 0 || strcmp(prop, "us") == 0)
        current_standard = STE_ADM_COMFORT_TONE_STANDARD_ANSI;
    else
        current_standard = STE_ADM_COMFORT_TONE_STANDARD_CEPT;

    if (wanted_standard != current_standard) {
        ALOG_ERR("Requested standard %d differs from standard specified in gsm.operator.iso-country (%s)\n", wanted_standard, prop);
        return 0;
    }

    return 1;
}

static int translate_tone_type(int* android_tone_type, ste_adm_comfort_tone_t tone_type)
{
    switch (tone_type) {
    case STE_ADM_COMFORT_TONE_DIAL:
        *android_tone_type = android::ToneGenerator::TONE_SUP_DIAL;
        return 0;
    case STE_ADM_COMFORT_TONE_SUBSCRIBER_BUSY:
        *android_tone_type = android::ToneGenerator::TONE_SUP_BUSY;
        return 0;
    case STE_ADM_COMFORT_TONE_CONGESTION:
        *android_tone_type = android::ToneGenerator::TONE_SUP_CONGESTION;
        return 0;
    case STE_ADM_COMFORT_TONE_RADIO_PATH_ACKNOWLEDGE:
        *android_tone_type = android::ToneGenerator::TONE_SUP_RADIO_ACK;
        return 0;
    case STE_ADM_COMFORT_TONE_RADIO_PATH_NOT_AVAILABLE:
        *android_tone_type = android::ToneGenerator::TONE_SUP_RADIO_NOTAVAIL;
        return 0;
    case STE_ADM_COMFORT_TONE_ERROR_SPECIAL_INFORMATION:
        *android_tone_type = android::ToneGenerator::TONE_SUP_ERROR;
        return 0;
    case STE_ADM_COMFORT_TONE_CALL_WAITING:
        *android_tone_type = android::ToneGenerator::TONE_SUP_CALL_WAITING;
        return 0;
    case STE_ADM_COMFORT_TONE_RINGING:
        *android_tone_type = android::ToneGenerator::TONE_SUP_RINGTONE;
        return 0;
    default:
        ALOG_ERR("Unknown tone type %d\n", tone_type);
        return -1;
    }
}


/*
* See ste_adm_start_comfort_tone for documentation.
*/
void tgapi_start_tone(msg_start_comfort_tone_t* msg_p, srv_session_t** client_pp)
{
    (void) client_pp;
    ALOG_INFO("tgapi_start_tone\n");

    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

    if (!g_tg_initalized)
    {
        if (adm_tg_init() != 0)
        {
            ALOG_ERR("tgapi_start_tone: failed to initialize TG.\n");
            msg_p->base.result = STE_ADM_RES_INTERNAL_ERROR;
            srv_send_reply((msg_base_t*)msg_p);
            return;
        }
    }

    if (!is_standard_active(msg_p->tone_standard)) {
        ALOG_ERR("tgapi_start_tone: start failed; can't play tone of requested standard\n");
        msg_p->base.result = STE_ADM_RES_INVALID_PARAMETER;
        srv_send_reply((msg_base_t*)msg_p);
        return;
    }

    int android_tone_type;
    if (translate_tone_type(&android_tone_type, msg_p->tone_type))
    {
        ALOG_ERR("tgapi_start_tone: Invalid tone type\n");
        msg_p->base.result = STE_ADM_RES_INVALID_PARAMETER;
        srv_send_reply((msg_base_t*)msg_p);
        return;
    }

    if (write(g_tg_pipe[1], &android_tone_type, sizeof(int)) != sizeof(int)) {
        ALOG_ERR("tgapi_start_tone: write() failed\n");
        msg_p->base.result = STE_ADM_RES_INTERNAL_ERROR;
        srv_send_reply((msg_base_t*)msg_p);
        return;
    }

    srv_send_reply((msg_base_t*)msg_p);
}

/*
* See ste_adm_start_comfort_tone for documentation.
*/
void tgapi_stop_tone(msg_base_t* msg_p, srv_session_t** client_pp)
{
    (void) client_pp;
    int no_tone = -1;
    ALOG_INFO("tgapi_stop_tone\n");

    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

    if (!g_tg_initalized)
    {
        ALOG_ERR("tgapi_stop_tone: Not initialized.\n");
        msg_p->result = STE_ADM_RES_INCORRECT_STATE;
        srv_send_reply(msg_p);
    }

    if (write(g_tg_pipe[1], &no_tone, sizeof(int)) != sizeof(int)) {
        ALOG_ERR("tgapi_stop_tone: write() failed\n");
        msg_p->result = STE_ADM_RES_INTERNAL_ERROR;
    }

    srv_send_reply(msg_p);
}




static void* tg_thread(void* args)
{
    (void) args;

    // Arguments to ctor: The type of sound, volume in float
    // (range [0,1]), and if the thread can be called from Java.
    // TODO: Does new throw or return 0?
    ALOG_INFO("Calling android::ToneGenerator ctor\n");
    android::ToneGenerator* tg = new android::ToneGenerator(
            AUDIO_STREAM_NOTIFICATION,
            1.0f,
            false);
    ALOG_INFO("Calling android::ToneGenerator ctor DONE\n");
    if (!tg) return 0;


    int tone_played = -1;
    int tone_to_play;
    while (1) {
        int ret = read(g_tg_pipe[0], &tone_to_play, sizeof(int));
        if (ret != sizeof(int)) {
            ALOG_ERR("read() failed in tone generator thread\n");
            return NULL;
        }

        if (tone_to_play != tone_played) {
            if (tone_played != -1) {
                ALOG_INFO("Calling tg->stopTone();\n");
                tg->stopTone();
                ALOG_INFO("DONE Calling tg->stopTone();\n");
                tone_played = -1;
            }
            if (tone_to_play != -1) {
                ALOG_INFO("Calling tg->startTone();\n");
                if (!tg->startTone((android::ToneGenerator::tone_type)tone_to_play)) {
                    ALOG_ERR("tg->startTone() failed\n");
                }
                ALOG_INFO("DONE Calling tg->startTone();\n");
            }

            tone_played = tone_to_play;
        }
    }
}

#else
void tgapi_start_tone(msg_start_comfort_tone_t* msg_p, srv_session_t** client_pp)
{
    ALOG_INFO("Tonegen requested to play tone, but tonegen is disabled\n");
    (void) client_pp;
    srv_send_reply(&msg_p->base);
}

void tgapi_stop_tone(msg_base_t* msg_p, srv_session_t** client_pp)
{
    ALOG_INFO("Tonegen requested to stop tone, but tonegen is disabled\n");
    (void) client_pp;
    srv_send_reply(msg_p);
}

#endif






