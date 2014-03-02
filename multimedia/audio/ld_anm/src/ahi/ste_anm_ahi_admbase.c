/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*! \file stream_adm_base.cc
*   \brief Common base structures for I/O interfaces.

    Provides common functionality for I/O interfaces, manages ADM connection
    etc.
*/

#define ANM_LOG_FILENAME "anm_ahi_admbase"

#include "ste_hal_anm_dbg.h"
#include "ste_hal_anm_ahi.h"
#include "ste_hal_anm_util.h"
#include <ste_adm_client.h>
#include <sys/mman.h>

int admbase_init(struct stream_adm_base* adm)
{
    ALOG_INFO("admbase_init(): stream_adm_base=%p\n", adm);
    adm->mADMConnectionID = -1;
    adm->mDeviceList = NULL;
    adm->mStandby = true;
    adm->mStarted = false;
    adm->mStandbyPending= false;
    adm->mChannels =0;
    adm->mSampleRate = 0;
    adm->mFormat = 0;
    adm->mAdmNumBufs = 0;
    adm->mAdmBufSize = 0;
    adm->mAdmBufSharedMem = 0;

    int retval = pthread_mutex_init(&adm->mMutex, 0);
    if (retval != 0) {
        ALOG_ERR("admbase_init(): pthread_mutex_init failed, error = %d\n", retval);
        return retval;
    }

    return retval;
}
void admbase_deinit(struct stream_adm_base *adm)
{
    ALOG_INFO("admbase_deinit(): stream_adm_base=%p\n", adm);
    if (adm->mADMConnectionID >= 0) {
        ALOG_INFO("admbase_deinit(): Disconnect from ADM, ID=%d\n", adm->mADMConnectionID);
        ste_adm_client_disconnect(adm->mADMConnectionID);
    }
    pthread_mutex_destroy(&adm->mMutex);
}


/* setupConnection */
int admbase_setup_adm_connection(struct stream_adm_base *adm)
{
    adm->mADMConnectionID = ste_adm_client_connect();
    ALOG_INFO("admbase_setup_adm_connection(): Connect to ADM, ID=%d\n",
        adm->mADMConnectionID);
    return adm->mADMConnectionID;
}

/* closeConnection */
int admbase_close_adm_connection(struct stream_adm_base *adm)
{
    ALOG_INFO("admbase_close_adm_connection(): Disconnect from ADM, ID=%d\n", adm->mADMConnectionID);
    (void) ste_adm_client_disconnect(adm->mADMConnectionID);
    adm->mADMConnectionID = -1;
    return 0;
}

char* admbase_devices_2_str(struct stream_adm_base *adm, char* buf, audio_devices_t dev)
{

    sprintf(buf, "%s%s%s%s%s%s%s%s%s%s%s%s" "%s%s%s%s%s%s%s%s%s",
        dev & AUDIO_DEVICE_OUT_EARPIECE ? "DEVICE_OUT_EARPIECE " : "",
        dev & AUDIO_DEVICE_OUT_SPEAKER ? "DEVICE_OUT_SPEAKER " : "",
        dev & AUDIO_DEVICE_OUT_WIRED_HEADSET ? "DEVICE_OUT_WIRED_HEADSET " : "",
        dev & AUDIO_DEVICE_OUT_WIRED_HEADPHONE ? "DEVICE_OUT_WIRED_HEADPHONE " : "",
        dev & AUDIO_DEVICE_OUT_BLUETOOTH_SCO ? "DEVICE_OUT_BLUETOOTH_SCO " : "",
        dev & AUDIO_DEVICE_OUT_BLUETOOTH_SCO_HEADSET ? "DEVICE_OUT_BLUETOOTH_SCO_HEADSET " : "",
        dev & AUDIO_DEVICE_OUT_BLUETOOTH_SCO_CARKIT ? "DEVICE_OUT_BLUETOOTH_SCO_CARKIT " : "",
        dev & AUDIO_DEVICE_OUT_BLUETOOTH_A2DP ? "DEVICE_OUT_BLUETOOTH_A2DP " : "",
        dev & AUDIO_DEVICE_OUT_BLUETOOTH_A2DP_HEADPHONES ? "DEVICE_OUT_BLUETOOTH_A2DP_HEADPHONES " : "",
        dev & AUDIO_DEVICE_OUT_BLUETOOTH_A2DP_SPEAKER ? "DEVICE_OUT_BLUETOOTH_A2DP_SPEAKER " : "",
        dev & AUDIO_DEVICE_OUT_AUX_DIGITAL ? "DEVICE_OUT_AUX_DIGITAL " : "",
        dev & AUDIO_DEVICE_OUT_DEFAULT ? "DEVICE_OUT_DEFAULT " : "",

        dev & AUDIO_DEVICE_IN_COMMUNICATION ? "DEVICE_IN_COMMUNICATION " : "",
        dev & AUDIO_DEVICE_IN_AMBIENT ? "DEVICE_IN_AMBIENT " : "",
        dev & AUDIO_DEVICE_IN_BUILTIN_MIC ? "DEVICE_IN_BUILTIN_MIC " : "",
        dev & AUDIO_DEVICE_IN_BLUETOOTH_SCO_HEADSET ? "DEVICE_IN_BLUETOOTH_SCO_HEADSET " : "",
        dev & AUDIO_DEVICE_IN_WIRED_HEADSET ? "DEVICE_IN_WIRED_HEADSET " : "",
        dev & AUDIO_DEVICE_IN_AUX_DIGITAL ? "DEVICE_IN_AUX_DIGITAL " : "",
        dev & AUDIO_DEVICE_IN_VOICE_CALL ? "DEVICE_IN_VOICE_CALL " : "",
        dev & AUDIO_DEVICE_IN_BACK_MIC ? "DEVICE_IN_BACK_MIC " : "",
        dev & AUDIO_DEVICE_IN_DEFAULT ? "DEVICE_IN_DEFAULT " : "");
    return buf;
}

ste_adm_format_t admbase_get_adm_format(struct stream_adm_base *adm)
{
    audio_format_t format;
    format = (audio_format_t)adm->mFormat;
    switch (format) {
        case AUDIO_FORMAT_PCM_16_BIT: {
            int num_channels = popcount(adm->mChannels);
            if (num_channels == 1 || num_channels == 2) {
                return (ste_adm_format_t) num_channels;
            }
            return STE_ADM_FORMAT_INVALID;
        }

        case AUDIO_FORMAT_AC3:     return STE_ADM_FORMAT_AC3;
        case AUDIO_FORMAT_MPEG1:   return STE_ADM_FORMAT_MPEG1;
        case AUDIO_FORMAT_MPEG2:   return STE_ADM_FORMAT_MPEG2;
        case AUDIO_FORMAT_DTS:     return STE_ADM_FORMAT_DTS;
        case AUDIO_FORMAT_ATRAC:   return STE_ADM_FORMAT_ATRAC;

        case AUDIO_FORMAT_OBA:     return STE_ADM_FORMAT_OBA;
        case AUDIO_FORMAT_DDPLUS:  return STE_ADM_FORMAT_DDPLUS;
        case AUDIO_FORMAT_DTS_HD:  return STE_ADM_FORMAT_DTS_HD;
        case AUDIO_FORMAT_MAT:     return STE_ADM_FORMAT_MAT;
        case AUDIO_FORMAT_DST:     return STE_ADM_FORMAT_DST;

        case AUDIO_FORMAT_WMA_PRO: return STE_ADM_FORMAT_WMA_PRO;
        default:      return STE_ADM_FORMAT_INVALID;
   }

}

/*
* Open devices and set HW params if in standby mode. Mutex mMutex MUST be held when
* calling this function.
*
* Note that mMutex is released for a while in case there is an error opening devices. If
* that happens any block protected by mMutex is broken (some other thread might take
* mMutex), but this is OK as long as openDevices() is called at the end of such a
* protected block.
*/
status_t admbase_open_devices(struct stream_adm_base *adm)
{
    int i;
    ste_adm_res_t err = STE_ADM_RES_OK;
    int retries = 10;
    struct node *list_elem;
    audio_policy_anm* audio_policy_service_ops_ptr = NULL;


    while (adm->mStandby && retries > 0) {
        retries--;
        ALOG_INFO_VERBOSE("admbase_open_devices()\n");

        /* Open the devices again */
        for (i=0; i < list_count(adm->mDeviceList); i++) {

            int num_bufs = 3;
            int bufsz = adm->calc_buffer_size(adm);
            char *bufp;
            list_elem = list_get_nth(adm->mDeviceList, i);

            ALOG_INFO("admbase_open_devices(): Open device %s in ADM, ID=%d\n", list_elem->key, adm->mADMConnectionID);
            err = ste_adm_client_open_device(adm->mADMConnectionID, list_elem->key,
                adm->mSampleRate, admbase_get_adm_format((struct stream_adm_base *)adm), NULL,
                &bufp, bufsz, num_bufs);
            if (err != STE_ADM_RES_OK) {
                ALOG_ERR("admbase_open_devices(): Failed to open device %s\n", list_elem->key);
                break;
            } else {
                ALOG_INFO_VERBOSE("admbase_open_devices(): Opened device. bufsz=%d num_bufs=%d\n", bufsz, num_bufs);
            }

            assert(adm->mAdmBufSharedMem == NULL || adm->mAdmBufSharedMem == bufp);
            assert(adm->mAdmBufSize      == 0    || adm->mAdmBufSize      == num_bufs);
            assert(adm->mAdmNumBufs      == 0    || adm->mAdmNumBufs      == num_bufs);
            adm->mAdmBufSharedMem = bufp;
            adm->mAdmBufSize      = bufsz;
            adm->mAdmNumBufs      = num_bufs;

            if (adm->mAdmBufSharedMem == NULL) {
                ALOG_ERR("admbase_open_devices(): adm->mAdmBufSharedMem == NULL after ste_adm_client_open_device\n");
            }

            adm->post_open_device((struct stream_adm_base *)adm); //Should be mapped to either i/p or o/p
        }

        if (err == STE_ADM_RES_ERR_MSG_IO) {
            ALOG_ERR("admbase_open_devices(): Failed to get out of standby mode, I/O error!\n");
            /* connection to ADM has been lost or other connection issues,
               possible ADM reboot try to re-initiate the connection */
            /* try to close but ignore error message if there are any */
            int tmp_err;

            for (i=0; i < list_count(adm->mDeviceList); i++) {
                list_elem = list_get_nth(adm->mDeviceList, i);
                tmp_err = ste_adm_close_device(adm->mADMConnectionID, (list_elem->key));
                    if (tmp_err != STE_ADM_RES_OK) {
                        ALOG_WARN("admbase_open_devices(): Lost connection to ADM, trying to close "
                            "device returned error=%d\n", tmp_err);
                    }
            }
            /* disconnect ADM connection */
            tmp_err = ste_adm_client_disconnect(adm->mADMConnectionID);
            if (tmp_err != STE_ADM_RES_OK) {
                ALOG_WARN("admbase_open_devices(): Lost connection to ADM, trying to disconnect "
                    "returned error=%d\n", tmp_err);
            }
            /* open new connection */
            adm->mADMConnectionID = ste_adm_client_connect();
            ALOG_INFO("admbase_open_devices(): Connect to ADM, ID=%d\n", adm->mADMConnectionID);
            if (adm->mAdmBufSharedMem != NULL) {
                munmap(adm->mAdmBufSharedMem, adm->mAdmBufSize * adm->mAdmNumBufs);
                adm->mAdmBufSharedMem = NULL;
                adm->mAdmBufSize      = 0;
                adm->mAdmNumBufs      = 0;
            }

            /* Release mMutex while calling into AP to avoid possible dead-lock.
            Note that mMutex was not taken in this function. Although, as specified
            in the function header this function assumes that mMutex has been taken
            before it is called */
            pthread_mutex_unlock(&adm->mMutex);

            /* Check call status and recover call graph if needed */
            if (adm->stream_in_anm != NULL) {
                audio_policy_service_ops_ptr = (audio_policy_anm*)(adm->stream_in_anm->dev->mAudioPolicyManager);
            } else {
                audio_policy_service_ops_ptr = (audio_policy_anm*)(adm->stream_out_anm->dev->mAudioPolicyManager);
            }
            ALOG_INFO_VERBOSE("ste_audio_policy_manager ptr=%x", audio_policy_service_ops_ptr);
            if (audio_policy_service_ops_ptr != NULL) {
              audio_policy_service_ops_ptr->check_call_status(audio_policy_service_ops_ptr->apm_ptr);
            }
            pthread_mutex_lock(&adm->mMutex);
        } else if (err != STE_ADM_RES_OK) {
            ALOG_ERR("admbase_open_devices(): Failed to get out of standby mode.\n");
            ALOG_ERR("admbase_open_devices(): Error code %s suggests no use retrying\n", ste_adm_res_to_str(err));
            retries = 0;
        } else {
            ALOG_INFO_VERBOSE("admbase_open_devices(): Successfully left standby mode.\n");
            adm->mStandby = false;
        }
    }

    if (err != STE_ADM_RES_OK){
        ALOG_ERR("admbase_open_devices(): returning UNKNOWN_ERROR\n");
        return UNKNOWN_ERROR;
    }
    else {
        return OK;
    }
}

status_t admbase_set_parameters(struct stream_adm_base *adm, const char *kv_pairs)
{
    status_t status;
    struct str_parms *parms;
    int ste_stream_state;
    int ret;

    (void)ste_anm_debug_setup_log();

    ALOG_INFO("ENTER admbase_set_parameters(): %s", kv_pairs);

    pthread_mutex_lock(&adm->mMutex);
    parms = str_parms_create_str(kv_pairs);
    ret = str_parms_get_int(parms, PARAM_KEY_STREAM_STATE, &ste_stream_state);
    str_parms_dump(parms);
    if (ret >= 0) {
        if (ste_stream_state == 1) {
            // Stream opened
            status = OK;
            if (!adm->mStarted) {
                if (admbase_open_devices(adm) == OK) {
                    adm->mStarted = true;
                } else {
                    ALOG_ERR("admbase_set_parameters(): open_devices() failed\n");
                    status = UNKNOWN_ERROR;
                }
            }
        } else if (ste_stream_state == 0) {
            // Stream stopped
            adm->mStarted = false;
            if (adm->mStandbyPending) {
                ALOG_INFO("admbase_set_parameters(): standby is pending, "
                    "call standby() since stream is now stopped\n");
                admbase_standby_imp(adm);
            }
            status = OK;
        } else {
            ALOG_ERR("admbase_set_parameters(): Unknown ste_stream_state %d\n", ste_stream_state);
            status = BAD_VALUE;
        }
    } else {
        str_parms_dump(parms);
        ALOG_INFO("admbase_set_parameters(): Unknown key \n");
        status = BAD_VALUE;
    }

    pthread_mutex_unlock(&adm->mMutex);

    ALOG_INFO("LEAVE admbase_set_parameters()");

    return status;
}

char* admbase_get_parameters(struct stream_adm_base *adm, const char *key)
{
    struct str_parms *parms;
    int ste_stream_state;
    int ret;
    char *kv_pairs = NULL;

    if(!strcmp(key, PARAM_KEY_STREAM_STATE)) {
        parms = str_parms_create_str(key);
        ret = str_parms_add_int(parms, PARAM_KEY_STREAM_STATE, adm->mStarted ? 1 : 0);
        kv_pairs = str_parms_to_str(parms);
        str_parms_destroy(parms);
        ALOG_INFO_VERBOSE("admbase_get_parameters(): %s", kv_pairs);
        return kv_pairs;
    }
    ALOG_INFO_VERBOSE("admbase_get_parameters(): Unknown key");
    return strdup("");
}

/* Mutex mMutex MUST be held when calling this function */
status_t admbase_standby_imp(struct stream_adm_base *adm)
{
    int i;
    struct node *list_elem;
    ALOG_INFO("admbase_standby_imp(): adm->mStandby=%d adm->mStarted=%d adm->mStandbyPending=%d\n",
              adm->mStandby, adm->mStarted, adm->mStandbyPending);

    /* If not already in standby mode, drain and close the active devices */
    if (!adm->mStandby) {
        if (adm->mStarted) {
            ALOG_INFO("admbase_standby_imp(): standby requested but io_handle "
                "not stopped --> mStandbyPending=true\n");
            adm->mStandbyPending = true;
        } else {
            adm->pre_close_device((struct stream_adm_base *)adm);

            for (i=0; i < list_count(adm->mDeviceList); i++) {
                list_elem = list_get_nth(adm->mDeviceList, i);

                ALOG_INFO("admbase_standby_imp(): Close device %s in ADM, ID=%d\n", list_elem->key, adm->mADMConnectionID);
                int err = ste_adm_close_device(adm->mADMConnectionID, list_elem->key);
                if (err != STE_ADM_RES_OK) {
                    ALOG_ERR("admbase_standby_imp(): Failed to close device!\n");
                }
            }

            adm->mStarted = false;
            adm->mStandby = true;
            adm->mStandbyPending = false;

            munmap(adm->mAdmBufSharedMem, adm->mAdmBufSize * adm->mAdmNumBufs);
            adm->mAdmBufSharedMem = NULL;
            adm->mAdmBufSize      = 0;
            adm->mAdmNumBufs      = 0;
        }
    }

    return OK;
}
