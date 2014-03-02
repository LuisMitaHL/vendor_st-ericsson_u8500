/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*! \file ste_anm_ahiinlib.cc
*   \brief ANM implementation of AHI Input interface.

    Implements the Input specific parts of AudioHardwareInterface defined in
    Android and used by the AudioFlinger. The input specific parts consists of
    the methods defined in AudioStreamIn.
*/

#define ANM_LOG_FILENAME "anm_ahi_input"
#include "ste_anm_dbg.h"

#include <sys/types.h>
#include <errno.h>

#include <cstdio>

#include "ste_anm_ahi.h"
#include "ste_anm_ap.h"
#include <ste_adm_client.h>

/**
* The buffer size provided to Android must be an integer multiple of
* this number of samples. If this criteria is not met, Android (Gingerbread)
* stops sending buffers, and fails to call ANM::standby().
* In stereo mode, it fails if the largest power-of-2 factor of the buffer size
* is 32 or less, and the problem has not been reproduced if it is 64 or more.
* This means a granularity of 16 samples (2 channels, 2 bytes per sample)
* is needed.
*
* Buffer sizes tested:
*  NOK: 5644 bytes (factors 2 2 17 83)             (multiple of 4)
*  NOK: 5664 bytes (factors 2 2 2 2 2 3 59)        (multiple of 32)
*  OK:  5696 bytes (factors 2 2 2 2 2 2 89)        (multiple of 64)
*  OK:  5632 bytes (factors 2 2 2 2 2 2 2 2 2 11)  (multiple of 512)
*/
#ifndef BUFFER_GRANULARITY_IN_SAMPLES
#define BUFFER_GRANULARITY_IN_SAMPLES (16)
#endif

/**
* Buffer duration in ms, to be used for input
*/
#ifndef INPUT_BUFFER_DURATION
#define INPUT_BUFFER_DURATION (64)
#endif

/**
* The default PCM format returned to interface client.
*/
#ifndef DEFAULT_PCM_FORMAT
#define DEFAULT_PCM_FORMAT (AudioSystem::PCM_16_BIT)
#endif

/**
* The default channel configuration returned to interface client.
*/
#ifndef DEFAULT_CHANNELS
#define DEFAULT_CHANNELS AudioSystem::CHANNEL_IN_MONO
#endif

/**
* The default buffer size returned to interface client.
*/
#ifndef DEFAULT_BUFFER_SIZE
#define DEFAULT_BUFFER_SIZE (20 * 240)
#endif

/**
* The default sample rate returned to interface client.
* This needs to be configurable at a customer level since AudioFlinger
* will up/downmix to this sample rate automatically.
*/
#ifndef DEFAULT_SAMPLE_RATE
#define DEFAULT_SAMPLE_RATE (8000)
#endif

using namespace android_audio_legacy;
using android::List;

namespace android_audio_legacy {
/**
* Typedef to clean up vector iterations
*/
typedef android::List<const char*>::iterator cviter;
}

namespace {
    /**
    * Build vector with device identifiers from routing bitfield.
    * @param: The vector to fill with devices.
    * @param: The routing bitfield.
    */
    void refreshDeviceList(List <const char*>&v, uint32_t routes);
}

status_t AudioStreamInANM::setup(
    int *format, uint32_t *channels, uint32_t *sampleRate)
{
    int err;

    ALOG_INFO_VERBOSE("setup(): format %d, channels %x (%d), "
        "samplerate %d\n", *format, *channels, AudioSystem::popCount(*channels),
        *sampleRate);

    /* First of all, connect to ADM */
    if (this->setupADMConnection() < 0) {
        ALOG_ERR("setup(): Failed to setup ADM Connection\n");
        return NO_INIT;
    }

    /* Check input values */
    if (*format == 0) {
        *format = DEFAULT_PCM_FORMAT;
        ALOG_INFO_VERBOSE("setup(): Format is 0 - update to %x", *format);
    }
    if (*channels == 0) {
        *channels = DEFAULT_CHANNELS;
        ALOG_INFO_VERBOSE("setup(): Channels is 0 - update to %x (%d)",
            *channels, AudioSystem::popCount(*channels));
    }
    if (*sampleRate == 0) {
        *sampleRate = DEFAULT_SAMPLE_RATE;
        ALOG_INFO_VERBOSE("setup(): Sample rate is 0 - update to %d", *sampleRate);
    }

    mFormat = *format;
    mChannels = *channels;
    mSampleRate = *sampleRate;

    return NO_ERROR;
}

void AudioStreamInANM::setMute(bool state)
{
    ALOG_INFO_VERBOSE("setMute(): %s", state ? "TRUE" : "FALSE");

    for (cviter it = mDeviceList.begin(); it != mDeviceList.end(); ++it) {
        //In case of VoIP, we don't need to set the app volume since it
        //  is handled by the voice call graph.
        if (!strcmp(*it, STE_ADM_DEVICE_STRING_VOIPIN)) {
            ALOG_INFO_VERBOSE("setMute(): Device is %s, no need to "
                "modify app volume.", *it);
            continue;
        }

        if (state) {
            if (ste_adm_client_get_app_volume(*it, &mMuteAppVol)){
                ALOG_INFO_VERBOSE("setMute(): ste_adm_client_get_app_volume() "
                    "failed probably because %s is not opened yet\n", *it);
            }
            if(ste_adm_client_set_app_volume(*it, INT_MIN)) {
                ALOG_INFO_VERBOSE("setMute(): ste_adm_client_set_app_volume() "
                    "failed probably because %s is not opened yet\n", *it);
            }
        } else {
            mMuteAppVol = this->getDefaultMicVol();
            if (ste_adm_client_set_app_volume(*it, mMuteAppVol)) {
                ALOG_INFO_VERBOSE("setMute(): ste_adm_client_set_app_volume() "
                    "failed probably because %s is not opened yet\n", *it);
            }
        }
    }

    mMuted = state;
}

int AudioStreamInANM::getDefaultMicVol()
{
    ALOG_INFO_VERBOSE("getDefaultMicVol()");
    return mMuteAppVol;
}

/********************* Implementation of AudioStreamIn ************************
* We implement the methods in the order they are listed in
* AudioStreamIn.
*
* BEGIN
*******************************************************************************/

AudioStreamInANM::AudioStreamInANM() :
    StreamADMBase(),
        mMuted(false)
{
    ALOG_INFO("AudioStreamInANM(): Created, %p", this);
}

AudioStreamInANM::~AudioStreamInANM()
{
    ALOG_INFO("AudioStreamInANM(): Destroyed");
}

/* Part of AudioStreamIn interface */
status_t AudioStreamInANM::setParameters(const String8& keyValuePairs)
{
    AudioParameter param = AudioParameter(keyValuePairs);
    String8 key = String8(AudioParameter::keyRouting);
    status_t status = NO_ERROR;
    int devices = 0;
    int output = 0;

    ALOG_INFO("setParameters(): %s (this=%p)",
        keyValuePairs.string(), this);

    /* Check for requested routing changes */
    if (param.getInt(key, devices) == NO_ERROR) {
        char buf[400];
        ALOG_INFO_VERBOSE("setParameters(): Got new routing %08X %s\n", devices,
            this->devices2str(buf, (AudioSystem::audio_devices) devices));
        status = this->changeDevice(0, devices);
    } else {
        ALOG_INFO_VERBOSE("setParameters(): Unhandled key %s, forward to StreamADMBase::setParameters\n", keyValuePairs.string());
        status = StreamADMBase::setParameters(keyValuePairs);
    }

    return status;
}
/* Part of AudioStreamIn interface */
String8 AudioStreamInANM::getParameters(const String8& key)
{
    return StreamADMBase::getParameters(key);
}

/* Part of AudioStreamIn interface */
uint32_t AudioStreamInANM::sampleRate() const
{
    ALOG_INFO_VERBOSE("sampleRate(): %d Hz\n", mSampleRate);
    return mSampleRate;
}

size_t AudioStreamInANM::calc_buffer_size() const
{
    size_t size = DEFAULT_BUFFER_SIZE;
    size_t channels = AudioSystem::popCount(mChannels);

    if (mSampleRate != 0 && channels != 0) {
        size = mSampleRate * INPUT_BUFFER_DURATION / 1000 * channels * 2;
        size_t granularity = BUFFER_GRANULARITY_IN_SAMPLES * 2 * channels;
        size = (size / granularity) * granularity;
    }
    return size;
}

/* Return the input buffer size allowed by input device */
/* Part of AudioStreamIn interface */
size_t AudioStreamInANM::bufferSize() const
{
    size_t bufsz = calc_buffer_size();
    ALOG_INFO_VERBOSE("bufferSize(): %d bytes\n", bufsz);
    return bufsz;
}

/* Return the number of audio input channels */
/* Part of AudioStreamIn interface */
uint32_t AudioStreamInANM::channels() const
{
    ALOG_INFO_VERBOSE("channels(): %d\n", mChannels);
    return mChannels;
}

/*
 * Return audio format, default is AudioSystem:PCM_16_BIT
 * Part of AudioStreamIn interface
 */
int AudioStreamInANM::format() const
{
    ALOG_INFO_VERBOSE("format(): %d\n", mFormat);
    return mFormat;
}

/*
 * Return the list of devices.
 */
List<const char*>* AudioStreamInANM::deviceList()
{
    ALOG_INFO_VERBOSE("deviceList()\n");
    return &mDeviceList;
}

/* Set the input gain for the audio driver. This method is for for future use */
/* Part of AudioStreamIn interface */
status_t AudioStreamInANM::setGain(float gain)
{
    ALOG_INFO_VERBOSE("setGain()\n");
    return OK;
}

/* Read audio buffer from input device */
ssize_t AudioStreamInANM::read(void* buffer, ssize_t bytes)
{
    int err = 0;
    int retry_read = 1;

    ALOG_INFO_FL("read(): %d bytes to buffer 0x%08X (this=%p)\n",
        (int) bytes, (uint32_t) buffer, this);

    if (mStandby) {
        ALOG_INFO("read(): Open device since in standby.\n");

        pthread_mutex_lock(&mMutex);
        status_t status = this->openDevices();
        pthread_mutex_unlock(&mMutex);

        if (status != OK) {
            ALOG_ERR("read(): Failed to open input device!\n");
            return JPARKS_BROKE_IT;
        }

        mStandby = false;
    }

    if (bytes == 0 || bytes > mAdmBufSize) {
        ALOG_ERR("read(): Requested to read invalid amount of bytes! bytes=%d mAdmBufSize=%d\n",bytes,mAdmBufSize);
        return BAD_VALUE;
    }


    if (mStandbyPending) {
        ALOG_INFO("read(): mStandbyPending TRUE, resetting\n");
        mStandbyPending = false;
    }

    while (retry_read) {

        if (mAdmBufSharedMem == NULL) {
            ALOG_ERR("read(): output buffer not yet allocated\n");
            return INVALID_OPERATION;
        }

        int bufIdx;
        err = ste_adm_client_receive(mADMConnectionID, &bufIdx);

        if (err == STE_ADM_RES_ERR_MSG_IO) {
            /* connection to ADM has been lost or other connection issues, possible ADM reboot
            try to re-initiate the connection */
            /* try to close but igore error message if there are any */
            int tmp_err;
            for (cviter it = mDeviceList.begin(); it != mDeviceList.end(); ++it) {
                tmp_err = ste_adm_close_device(mADMConnectionID, *it);
                if (tmp_err != STE_ADM_RES_OK) {
                    ALOG_WARN("read(): Lost connection to ADM, trying to close device returned error=%d\n",
                            tmp_err);
                }
            }
            /* disconnect ADM connection */
            tmp_err = ste_adm_client_disconnect(mADMConnectionID);
            if (tmp_err != STE_ADM_RES_OK) {
                ALOG_WARN("read(): Lost connection to ADM, trying to disconnect returned error=%d\n",
                        tmp_err);
            }
            /* open new connection */
            mADMConnectionID = ste_adm_client_connect();
            if (mADMConnectionID < 0) {
                /* failed to re-connect */
                return FAILED_TRANSACTION;
            }
            ALOG_INFO("read(): Re-connected to ADM after lost connection, ID = %d", mADMConnectionID);

            /* Check call status and recover call graph if needed */
            String8 keyValuePairs = android::AudioSystem::getParameters(0, String8(PARAM_KEY_AP_REF));
            AudioParameter apParam = AudioParameter(keyValuePairs);
            int ptr = 0;
            if (apParam.getInt(String8(PARAM_KEY_AP_REF), ptr) == NO_ERROR) {
                ((AudioPolicyManagerANM*)ptr)->checkCallStatus();
            }

            /* re-open all new devices */
            mAdmNumBufs = 3;
            for (cviter it = mDeviceList.begin(); it != mDeviceList.end(); ++it) {
                tmp_err = ste_adm_client_open_device(mADMConnectionID, *it,
                    sampleRate(), get_adm_format(), NULL,
                    &mAdmBufSharedMem, mAdmBufSize, mAdmNumBufs);
                if (tmp_err != STE_ADM_RES_OK) {
                    ALOG_ERR("read(): Re-connect to ADM, failed to open device %s\n", *it);
                }
            }
            ALOG_INFO("read(): Reconnected to ADM server\n");
            ///* return error, second read should hopefully be successful */
            //return FAILED_TRANSACTION;
            continue;
        } else if (err == STE_ADM_RES_DEVICE_RECONFIGURING) {
            LOGE("Failed to receive audio data from ADM (currently " \
                 "reconfiguring device). Waiting some time before trying again.");
            usleep(500000);
            continue;
        } else if (err != STE_ADM_RES_OK) {
            ALOG_ERR("read(): Failed to receive audio data from ADM: %d\n", err);
            return FAILED_TRANSACTION;
        } else {
            memcpy(buffer, mAdmBufSharedMem + mAdmBufSize*bufIdx, bytes);
        }
        retry_read = 0;
    }

    ALOG_INFO_FL("read(): Completed\n");
    return bytes;
}

/* Dump the state of the audio input device */
/* Part of AudioStreamIn interface */
status_t AudioStreamInANM::dump(int fd, const Vector<String16>& args)
{
    ALOG_INFO_VERBOSE("dump()\n");
    return OK;
}

void AudioStreamInANM::postOpenDevice(void)
{
    ALOG_INFO_VERBOSE("postOpenDevice(): updating mute status\n");

    if (mMuted){
        ALOG_INFO_VERBOSE("postOpenDevice(): Microphone muted");
        setMute(mMuted);
    }
}

void AudioStreamInANM::preCloseDevice()
{
    ALOG_INFO_VERBOSE("preCloseDevice(): does nothing\n");
}

/* Part of AudioStreamIn interface */
unsigned int  AudioStreamInANM::getInputFramesLost() const {
  /**@todo: implement */
  return 0;
}

status_t AudioStreamInANM::addAudioEffect(effect_handle_t effect)
{
    ALOG_ERR("addAudioEffect(): is NOT IMPLEMENTED!!!!! Returning NO_ERROR");
    return NO_ERROR;
}

status_t AudioStreamInANM::removeAudioEffect(effect_handle_t effect)
{
    ALOG_ERR("removeAudioEffect(): is NOT IMPLEMENTED!!!!! Returning NO_ERROR");
    return NO_ERROR;
}

status_t AudioStreamInANM::changeDevice(int mode, uint32_t routes)
{
    status_t status = OK;

    ALOG_INFO("changeDevice(): Got change device request, routes (0x%08X) (this=%p)\n",
          routes, this);

    pthread_mutex_lock(&mMutex);

    if (!mStandby) {
        /* First go to standby mode, drain pcm and close devices */
        mStarted = false;
        status = this->standby_l();

        if (status != OK) {
            pthread_mutex_unlock(&mMutex);
            return status;
        }

        /* Refresh device list from new routing info */
        refreshDeviceList(mDeviceList, routes);

        /* Open all the devices again, this will leave standby mode */
        status = this->openDevices();
    }
    else
    {
        /* Refresh device list from new routing info */
        refreshDeviceList(mDeviceList, routes);
    }

    pthread_mutex_unlock(&mMutex);

    return status;
}

/* Part of AudioStreamIn interface */
status_t AudioStreamInANM::standby()
{
    status_t status;
    pthread_mutex_lock(&mMutex);
    status = standby_l();
    pthread_mutex_unlock(&mMutex);
    return status;
}

/* Mutex mMutex MUST be held when calling this function */
status_t AudioStreamInANM::standby_l()
{
    status_t status;
    ALOG_INFO("ENTER standby() (this=%p)\n", this);
    status = standby_imp();
    ALOG_INFO("LEAVE standby()\n");
    return status;
}

/*********************** Implementation of File scope functions ****************
*
*
*******************************************************************************/
void AudioStreamInANM::refreshDeviceList(List <const char*>&v, uint32_t routes)
{
    v.clear();

    if (routes & AudioSystem::DEVICE_IN_BUILTIN_MIC) {
        v.push_back(STE_ADM_DEVICE_STRING_MIC);
    }

    if (routes & AudioSystem::DEVICE_IN_WIRED_HEADSET) {
        v.push_back(STE_ADM_DEVICE_STRING_HSIN);
    }

    if (routes & AudioSystem::DEVICE_IN_VOICE_CALL) {
        v.push_back(STE_ADM_DEVICE_STRING_DICT_REC);
    }

    if (routes & AudioSystem::DEVICE_IN_BLUETOOTH_SCO_HEADSET) {
        bool isBTWB = false; /* Bluetooth Wideband (16kHz) */
        String8 keyValuePairs = android::AudioSystem::getParameters(0, String8(PARAM_KEY_BTWB));
        AudioParameter param = AudioParameter(keyValuePairs);
        String8 value;
        if (param.get(String8(PARAM_KEY_BTWB), value) == NO_ERROR) {
            isBTWB = (value == "on");
        }
        if (isBTWB) {
            v.push_back(STE_ADM_DEVICE_STRING_BTWBIN);
        } else {
            v.push_back(STE_ADM_DEVICE_STRING_BTIN);
        }
    }

    if (routes & AudioSystem::DEVICE_IN_FM_RADIO_RX) {
        v.push_back(STE_ADM_DEVICE_STRING_FMRX);
    }

    if (routes & AudioSystem::DEVICE_IN_DEFAULT) {
        v.push_back(STE_ADM_DEVICE_STRING_VOIPIN);
    }
}
