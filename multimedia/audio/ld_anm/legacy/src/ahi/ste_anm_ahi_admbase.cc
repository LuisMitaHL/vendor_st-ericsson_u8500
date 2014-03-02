/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*! \file stream_adm_base.cc
*   \brief Common base class for I/O interfaces.

    Provides common functionality for I/O interfaces, manages ADM connection
    etc.
*/

#define ANM_LOG_FILENAME "anm_ahi_admbase"
#include "ste_anm_dbg.h"

#include <sys/types.h>
#include <errno.h>

#include "ste_anm_ahi.h"
#include "ste_anm_ap.h"
#include <ste_adm_client.h>

#include <media/AudioSystem.h>
#include <utils/List.h>

using namespace std;
using namespace android_audio_legacy;

namespace android_audio_legacy {
/**
* Typedef to clean up vector iterations
*/
typedef android::List<const char*>::iterator cviter;
}

StreamADMBase::StreamADMBase() :
    mADMConnectionID(-1), mStandby(true),
    mStarted(false), mStandbyPending(false),
    mChannels(0), mSampleRate(0), mFormat(0), mAdmNumBufs(0),
    mAdmBufSize(0), mAdmBufSharedMem(0)
{
    ALOG_INFO("StreamADMBase(): Created, %p\n", this);

    int retval = pthread_mutex_init(&mMutex, 0);
    if (retval != 0) {
        ALOG_ERR("StreamADMBase(): pthread_mutex_init failed, error = %d\n", retval);
    }
}

/* Dtor */
StreamADMBase::~StreamADMBase()
{
    ALOG_INFO("StreamADMBase(): Destroyed, %p\n", this);
    if (mADMConnectionID >= 0) {
        ALOG_INFO("StreamADMBase(): Disconnect from ADM, ID=%d\n", mADMConnectionID);
        ste_adm_client_disconnect(mADMConnectionID);
    }
    pthread_mutex_destroy(&mMutex);
}

/* setupConnection */
int StreamADMBase::setupADMConnection()
{
    mADMConnectionID = ste_adm_client_connect();
    ALOG_INFO("setupADMConnection(): Connect to ADM, ID=%d\n",
        mADMConnectionID);
    return mADMConnectionID;
}

/* closeConnection */
int StreamADMBase::closeADMConnection()
{
    ALOG_INFO("closeADMConnection(): Disconnect from ADM, ID=%d\n", mADMConnectionID);
    (void) ste_adm_client_disconnect(mADMConnectionID);
    mADMConnectionID = -1;
    return 0;
}

char* StreamADMBase::devices2str(char* buf, AudioSystem::audio_devices dev)
{
    sprintf(buf, "%s%s%s%s%s%s%s%s%s%s%s%s" "%s%s%s%s%s%s%s%s%s",
        dev & AudioSystem::DEVICE_OUT_EARPIECE ? "EARPIECE " : "",
        dev & AudioSystem::DEVICE_OUT_SPEAKER ? "SPEAKER " : "",
        dev & AudioSystem::DEVICE_OUT_WIRED_HEADSET ? "OUT_HEADSET " : "",
        dev & AudioSystem::DEVICE_OUT_WIRED_HEADPHONE ? "HEADPHONE " : "",
        dev & AudioSystem::DEVICE_OUT_BLUETOOTH_SCO ? "BT_SCO " : "",
        dev & AudioSystem::DEVICE_OUT_BLUETOOTH_SCO_HEADSET ? "BT_SCO_HEADSET " : "",
        dev & AudioSystem::DEVICE_OUT_BLUETOOTH_SCO_CARKIT ? "BT_SCO_CARKIT " : "",
        dev & AudioSystem::DEVICE_OUT_BLUETOOTH_A2DP ? "A2DP " : "",
        dev & AudioSystem::DEVICE_OUT_BLUETOOTH_A2DP_HEADPHONES ? "A2DP_HEADPHONES " : "",
        dev & AudioSystem::DEVICE_OUT_BLUETOOTH_A2DP_SPEAKER ? "A2DP_SPEAKER " : "",
        dev & AudioSystem::DEVICE_OUT_AUX_DIGITAL ? "AUX_DIGITAL " : "",
        dev & AudioSystem::DEVICE_OUT_DEFAULT ? "OUT_DEFAULT " : "",

        dev & AudioSystem::DEVICE_IN_COMMUNICATION ? "COMMUNICATION " : "",
        dev & AudioSystem::DEVICE_IN_AMBIENT ? "AMBIENT " : "",
        dev & AudioSystem::DEVICE_IN_BUILTIN_MIC ? "MIC " : "",
        dev & AudioSystem::DEVICE_IN_BLUETOOTH_SCO_HEADSET ? "BT_SCO_HEADSET " : "",
        dev & AudioSystem::DEVICE_IN_WIRED_HEADSET ? "IN_HEADSET " : "",
        dev & AudioSystem::DEVICE_IN_AUX_DIGITAL ? "AUX " : "",
        dev & AudioSystem::DEVICE_IN_VOICE_CALL ? "VOICECALL " : "",
        dev & AudioSystem::DEVICE_IN_BACK_MIC ? "BACK_MIC " : "",
        dev & AudioSystem::DEVICE_IN_DEFAULT ? "IN_DEFAULT " : "");
    return buf;
}

ste_adm_format_t StreamADMBase::get_adm_format(void) const
{
    switch (mFormat) {
        case AudioSystem::PCM_16_BIT: {
            int num_channels = AudioSystem::popCount(mChannels);
            if (num_channels == 1 || num_channels == 2) {
                return (ste_adm_format_t) num_channels;
            }
            return STE_ADM_FORMAT_INVALID;
        }

        case AudioSystem::AC3:     return STE_ADM_FORMAT_AC3;
        case AudioSystem::MPEG1:   return STE_ADM_FORMAT_MPEG1;
        case AudioSystem::MPEG2:   return STE_ADM_FORMAT_MPEG2;
        case AudioSystem::DTS:     return STE_ADM_FORMAT_DTS;
        case AudioSystem::ATRAC:   return STE_ADM_FORMAT_ATRAC;

        case AudioSystem::OBA:     return STE_ADM_FORMAT_OBA;
        case AudioSystem::DDPLUS:  return STE_ADM_FORMAT_DDPLUS;
        case AudioSystem::DTS_HD:  return STE_ADM_FORMAT_DTS_HD;
        case AudioSystem::MAT:     return STE_ADM_FORMAT_MAT;
        case AudioSystem::DST:     return STE_ADM_FORMAT_DST;

        case AudioSystem::WMA_PRO: return STE_ADM_FORMAT_WMA_PRO;
        default:                   return STE_ADM_FORMAT_INVALID;
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
status_t StreamADMBase::openDevices()
{
    ste_adm_res_t err = STE_ADM_RES_OK;
    int retries = 10;
    while (mStandby && retries > 0) {
        retries--;
        ALOG_INFO_VERBOSE("openDevices()\n");

        /* Open the devices again */
        for (cviter it = mDeviceList.begin(); it != mDeviceList.end(); ++it) {
            int num_bufs = 3;
            int bufsz = calc_buffer_size();
            char *bufp;

            ALOG_INFO("openDevices(): Open device %s in ADM, ID=%d\n", *it, mADMConnectionID);
            err = ste_adm_client_open_device(mADMConnectionID, *it,
                mSampleRate, get_adm_format(), NULL,
                &bufp, bufsz, num_bufs);
            if (err != STE_ADM_RES_OK) {
                ALOG_ERR("openDevices(): Failed to open device %s\n", *it);
                break;
            } else {
                ALOG_INFO_VERBOSE("openDevices(): Opened device. bufsz=%d num_bufs=%d\n", bufsz, num_bufs);
                mStarted = true;
            }

            assert(mAdmBufSharedMem == NULL || mAdmBufSharedMem == bufp);
            assert(mAdmBufSize      == 0    || mAdmBufSize      == num_bufs);
            assert(mAdmNumBufs      == 0    || mAdmNumBufs      == num_bufs);

            mAdmBufSharedMem = bufp;
            mAdmBufSize      = bufsz;
            mAdmNumBufs      = num_bufs;

            if (mAdmBufSharedMem == NULL) {
                ALOG_ERR("openDevices(): mAdmBufSharedMem == NULL after ste_adm_client_open_device\n");
            }

            postOpenDevice();
        }

        if (err == STE_ADM_RES_ERR_MSG_IO) {
            ALOG_ERR("openDevices(): Failed to get out of standby mode, I/O error!\n");
            /* connection to ADM has been lost or other connection issues,
               possible ADM reboot try to re-initiate the connection */
            /* try to close but ignore error message if there are any */
            int tmp_err;
            for (cviter it = mDeviceList.begin(); it != mDeviceList.end(); ++it) {
                tmp_err = ste_adm_close_device(mADMConnectionID, *it);
                if (tmp_err != STE_ADM_RES_OK) {
                    ALOG_WARN("openDevices(): Lost connection to ADM, trying to close "
                        "device returned error=%d\n", tmp_err);
                }
            }
            /* disconnect ADM connection */
            tmp_err = ste_adm_client_disconnect(mADMConnectionID);
            if (tmp_err != STE_ADM_RES_OK) {
                ALOG_WARN("openDevices(): Lost connection to ADM, trying to disconnect "
                    "returned error=%d\n", tmp_err);
            }
            /* open new connection */
            mADMConnectionID = ste_adm_client_connect();
            ALOG_INFO("openDevices(): Connect to ADM, ID=%d\n", mADMConnectionID);
            if (mAdmBufSharedMem != NULL) {
                munmap(mAdmBufSharedMem, mAdmBufSize * mAdmNumBufs);
                mAdmBufSharedMem = NULL;
                mAdmBufSize      = 0;
                mAdmNumBufs      = 0;
            }

            /* Release mMutex while calling into AP to avoid possible dead-lock.
               Note that mMutex was not taken in this function. Although, as specified
               in the function header this function assumes that mMutex has been taken
               before it is called */
            pthread_mutex_unlock(&mMutex);

            /* Check call status and recover call graph if needed */
            String8 keyValuePairs = android::AudioSystem::getParameters(0, String8(PARAM_KEY_AP_REF));
            AudioParameter apParam = AudioParameter(keyValuePairs);
            int ptr = 0;
            if (apParam.getInt(String8(PARAM_KEY_AP_REF), ptr) == NO_ERROR) {
                ((AudioPolicyManagerANM*)ptr)->checkCallStatus();
            }

            pthread_mutex_lock(&mMutex);

        } else if (err != STE_ADM_RES_OK) {
            ALOG_ERR("openDevices(): Failed to get out of standby mode.\n");
            ALOG_ERR("   Error code %s suggests no use retrying\n", ste_adm_res_to_str(err));
            retries = 0;
        } else {
            ALOG_INFO_VERBOSE("openDevices(): Successfully left standby mode.\n");
            mStandby = false;
        }
    }

    if (err != STE_ADM_RES_OK){
        ALOG_ERR("openDevices(): returning UNKNOWN_ERROR\n");
        return UNKNOWN_ERROR;
    }
    else {
        return OK;
    }
}

status_t StreamADMBase::setParameters(const String8& keyValuePairs)
{
    status_t status;

    pthread_mutex_lock(&mMutex);

    /* Check for requested routing changes */
    AudioParameter param = AudioParameter(keyValuePairs);
    const String8 key = String8(PARAM_KEY_STREAM_STATE);
    int ste_stream_state;
    if (param.getInt(key, ste_stream_state) == NO_ERROR) {
        ALOG_INFO("setParameters(): ste_stream_state=%d\n", ste_stream_state);

        if (ste_stream_state == 1) {
            // Stream started
            status = OK;
            if (!mStarted) {
                if (openDevices() == OK) {
                    mStarted = true;
                } else {
                    ALOG_ERR("setParameters(): openDevices() failed\n");
                    status = UNKNOWN_ERROR;
                }
            }
        } else if (ste_stream_state == 0) {
            // Stream stopped
            mStarted = false;
            if (mStandbyPending) {
                ALOG_INFO("setParameters(): calling standby() to update status\n");
                standby_imp();
            }
            status = OK;
        } else {
            ALOG_ERR("setParameters(): Unknown ste_stream_state %d\n", ste_stream_state);
            status = BAD_VALUE;
        }
    } else {
        ALOG_INFO("setParameters(): Unknown key %s\n", keyValuePairs.string());
        status = BAD_VALUE;
    }

    pthread_mutex_unlock(&mMutex);

    return status;
}

String8 StreamADMBase::getParameters(const String8& key)
{
    AudioParameter param = AudioParameter();
    if (key == PARAM_KEY_STREAM_STATE) {
        param.addInt(key, mStarted ? 1 : 0);
        ALOG_INFO_VERBOSE("getParameters(): ste_stream_state=%d\n", mStarted);
    }
    return param.toString();
}

/* Mutex mMutex MUST be held when calling this function */
status_t StreamADMBase::standby_imp()
{
    ALOG_INFO("standby_imp(): mStandby=%d mStarted=%d mStandbyPending=%d\n",
              mStandby, mStarted, mStandbyPending);

    /* If not already in standby mode, drain and close the active devices */
    if (!mStandby) {
        if (mStarted) {
            ALOG_INFO("standby_imp(): standby requested but io_handle "
                "not stopped --> mStandbyPending=true\n");
            mStandbyPending = true;
        } else {
            preCloseDevice();

            for (cviter it = mDeviceList.begin(); it != mDeviceList.end(); ++it) {
                ALOG_INFO("standby_imp(): Close device %s in ADM, ID=%d\n", *it, mADMConnectionID);
                int err = ste_adm_close_device(mADMConnectionID, *it);
                if (err != STE_ADM_RES_OK) {
                    ALOG_ERR("standby_imp(): Failed to close device!\n");
                }
            }

            mStarted = false;
            mStandby = true;
            mStandbyPending = false;

            munmap(mAdmBufSharedMem, mAdmBufSize * mAdmNumBufs);
            mAdmBufSharedMem = NULL;
            mAdmBufSize      = 0;
            mAdmNumBufs      = 0;
        }
    }

    return OK;
}



