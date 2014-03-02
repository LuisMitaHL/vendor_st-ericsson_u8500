/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*! \file ste_anm_ahioutlib.cc
*   \brief ANM implementation of AHI Output interface.

    Implements the Output specific parts of AudioHardwareInterface defined in
    Android and used by the AudioFlinger. The output specific parts consists of
    the methods defined in AudioStreamOut.
*/

#define ANM_LOG_FILENAME "anm_ahi_output"
#include "ste_anm_dbg.h"

#include <utils/List.h>

#include <sys/types.h>
#include <errno.h>

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
* Buffer duration in ms, to be used in low power mode.
* This is the size used for buffers to ADM.
*/
#ifndef LOW_POWER_BUFFER_DURATION
#define LOW_POWER_BUFFER_DURATION (32)
#endif

/**
* Buffer duration in ms, to be used to AudioFlinger
*/
#ifndef AUDIO_FLINGER_BUFFER_DURATION
#define AUDIO_FLINGER_BUFFER_DURATION (16)
#endif


/**
* Number of buffers per write to ADM
*/
#ifndef NUMBER_OF_BUFFERS_PER_WRITE
#define NUMBER_OF_BUFFERS_PER_WRITE (1)
#endif

/**
* The default sample rate returned to interface client.
* This needs to be configurable at a customer level since AudioFlinger
* will up/downmix to this sample rate automatically.
*/
#ifndef DEFAULT_SAMPLE_RATE
#define DEFAULT_SAMPLE_RATE (48000)
#endif

/**
* The default channel configuration returned to interface client.
*/
#define DEFAULT_CHANNELS AudioSystem::CHANNEL_OUT_STEREO

/**
* The default PCM format returned to interface client.
*/
#ifndef DEFAULT_PCM_FORMAT
#define DEFAULT_PCM_FORMAT (AudioSystem::PCM_16_BIT)
#endif

/**
* The default HW latency returned to interface client.
*/
#define DEFAULT_LATENCY (100)


/**
* The buffer size for coded formats (e.g. AC3 or DTS)
*/
#ifndef CODED_FORMAT_BUFFER_SIZE
#define CODED_FORMAT_BUFFER_SIZE (4096)
#endif

using namespace std;
using namespace android_audio_legacy;
using android::List;

/*********************** Declaration of File scope functions *******************
*
*
*******************************************************************************/

namespace android_audio_legacy {
/**
* Typedef to clean up vector iterations
*/
typedef android::List<const char*>::iterator cviter;
}

static int MIN(int x, int y) { return x<y ? x : y; }

void* AudioStreamOutANM::open_dev_thread(void* param)
{
    AudioStreamOutANM* out = (AudioStreamOutANM*)param;
    ALOG_INFO("ENTER open_dev_thread(): (this=%p)", out);
    int err = 0;
    int newADMConnID = ste_adm_client_connect();
    ALOG_INFO("open_dev_thread(): New ADM connection has fd = %d", newADMConnID);

    List<const char*> deviceList;
    out->refreshDeviceList(deviceList, out->mNewDevices);
    for (cviter it = deviceList.begin(); it != deviceList.end(); ++it) {
        int num_bufs = 3;
        int bufsz = out->calc_buffer_size();
        char *bufp;

        ALOG_INFO("open_dev_thread(): Open device %s in ADM connection %d\n", *it, newADMConnID);
        err = ste_adm_client_open_device(newADMConnID, *it,
            out->sampleRate(), out->get_adm_format(), NULL,
            &bufp, bufsz, num_bufs);
        if (err != STE_ADM_RES_OK) {
            ALOG_ERR("open_dev_thread(): Failed to open device %s, err = %d\n", *it, err);
            break;
        }

        assert(out->mNewAdmBufSharedMem == NULL || out->mNewAdmBufSharedMem == bufp);
        assert(out->mNewAdmBufSize      == 0    || out->mNewAdmBufSize      == num_bufs);
        assert(out->mNewAdmNumBufs      == 0    || out->mNewAdmNumBufs      == num_bufs);

        out->mNewAdmBufSharedMem = bufp;
        out->mNewAdmBufSize      = bufsz;
        out->mNewAdmNumBufs      = num_bufs;
    }

    if (err == STE_ADM_RES_OK) {
        /* Set new ADM connection since all new devices are opened */
        out->mNewADMConnID = newADMConnID;
    } else {
        /* Failed to open device, close the new connection */
        ste_adm_client_disconnect(newADMConnID);
        if (out->mNewAdmBufSharedMem != NULL) {
            munmap(out->mNewAdmBufSharedMem, out->mNewAdmBufSize * out->mNewAdmNumBufs);
            out->mNewAdmBufSharedMem = NULL;
            out->mNewAdmBufSize      = 0;
            out->mNewAdmNumBufs      = 0;
        }
    }

    out->mChangingDevice = false;
    pthread_mutex_unlock(&(out->mMutex));

    if (err == STE_ADM_RES_OK) {
        /* Now that the mutex has been released, check if sink latency
           shall be notified to client */
        if (out->mLatencyReportOutput != 0) {
            out->notifySinkLatency(out->mLatencyReportOutput);
            out->mLatencyReportOutput = 0;
        }
    }

    ALOG_INFO("LEAVE open_dev_thread():");
    return NULL;
}

void* AudioStreamOutANM::close_dev_thread(void* param)
{
    AudioStreamOutANM* out = (AudioStreamOutANM*)param;
    ALOG_INFO("ENTER close_dev_thread(): fd to close is %d (this=%p)", out->mOldADMConnID, out);
    pthread_mutex_lock(&(out->mMutex));

    (void) ste_adm_client_drain(out->mOldADMConnID);

    List<const char*> deviceList;
    out->refreshDeviceList(deviceList, out->mOldDevices);
    for (cviter it = deviceList.begin(); it != deviceList.end(); ++it) {
        ALOG_INFO("close_dev_thread(): Close device %s in ADM connection %d\n", *it, out->mOldADMConnID);
        int err = ste_adm_close_device(out->mOldADMConnID, *it);
        if (err != STE_ADM_RES_OK) {
            ALOG_ERR("close_dev_thread(): Failed to close device %s! err=%d\n", *it, err);
        }
    }

    ALOG_INFO("close_dev_thread(): Disconnect from ADM, ID = %d", out->mOldADMConnID);
    ste_adm_client_disconnect(out->mOldADMConnID);
    out->mOldADMConnID = -1;
    out->mOldDevices = 0;

    pthread_mutex_unlock(&(out->mMutex));
    ALOG_INFO("LEAVE close_dev_thread(): pthread_self=%X", (int) pthread_self());
    return NULL;
}

AudioStreamOutANM::AudioStreamOutANM(): StreamADMBase(),
    mNewDevices(0), mChangingDevice(false), mNewADMConnID(0),
    mNewAdmNumBufs(0), mNewAdmBufSize(0), mNewAdmBufSharedMem(0),
    mOldDevices(0), mOldADMConnID(0), mCurBufIdx(0),
    mDevices(0), mLatency(0), mLatencyReportOutput(0),
    mLpaMode(0), mCurBufOffset(0)
{
    ALOG_INFO("AudioStreamOutANM(): Created, %p\n", this);
    int retval = pthread_mutex_init(&mMutex, 0);
    if (retval != 0) {
        ALOG_ERR("AudioStreamOutANM(): pthread_mutex_init failed, error = %d\n", retval);
    }
}

/********************* Implementation of AudioStreamOut ************************
*
*
*******************************************************************************/
AudioStreamOutANM::~AudioStreamOutANM()
{
    status_t status;
    ALOG_INFO("AudioStreamOutANM(): Destroyed\n");

    /* Go to standby (drain and close devices) */
    status = this->standby();

    if (status != NO_ERROR) {
        ALOG_ERR("AudioStreamOutANM(): Failed to go to standby mode, continue anyway\n");
    }

    if (this->closeADMConnection()) {
        ALOG_ERR("AudioStreamOutANM(): Failed to close ADM connection!\n");
    }

    pthread_mutex_destroy(&mMutex);
}


/* React to live parameter changes routing, sample rate etc... */
/* Part of AudioStreamOut interface */
status_t AudioStreamOutANM::setParameters(const String8& keyValuePairs)
{
    AudioParameter param = AudioParameter(keyValuePairs);
    String8 key = String8(AudioParameter::keyRouting);
    String8 latencyKey = String8("ste_sink_latency");
    String8 keyStandby = String8(PARAM_KEY_STREAM_STANDBY);
    status_t status = NO_ERROR;
    int devices = 0;
    int output = 0;
    int standby = 0;

    ALOG_INFO("setParameters(): %s (this=%p)",
        keyValuePairs.string(), this);

    /* Check for requested routing changes */
    if (param.getInt(key, devices) == NO_ERROR) {
        char buf[400];
        ALOG_INFO_VERBOSE("setParameters(): Got new routing %08X: %s\n", devices,
            this->devices2str(buf, (AudioSystem::audio_devices) devices));
        status = this->changeDevice(0, devices);
    } else  if (param.getInt(latencyKey, output) == NO_ERROR) {
        if (mChangingDevice) {
            /* Cannot retrieve sink latency until output has been opened. */
            mLatencyReportOutput = output;
            return status;
        }
        notifySinkLatency(output);
    } else if (param.getInt(keyStandby, standby) == NO_ERROR) {
        status = this->standby();
    } else {
        ALOG_INFO_VERBOSE("setParameters(): Unhandled key %s, forward to StreamADMBase::setParameters\n", keyValuePairs.string());
        status = StreamADMBase::setParameters(keyValuePairs);
    }

    return status;
}

/* Return a pointer to this AudioStreamOutANM instance */
String8 AudioStreamOutANM::getParameters(const String8& key)
{
    AudioParameter param = AudioParameter();
    if(key == PARAM_KEY_STREAM_REF) {
        int ptr = (int)this;
        param.addInt(key, ptr);
        ALOG_INFO_VERBOSE("getParameters(): Getting AHI pointer with value 0x%x", ptr);
    } else {
        return StreamADMBase::getParameters(key);
    }
    return param.toString();
}

/* Return audio sample rate in Hz - default is 48000 Hz */
/* Part of AudioStreamOut interface */
uint32_t AudioStreamOutANM::sampleRate() const
{
    ALOG_INFO_VERBOSE("sampleRate(): %d Hz\n",
        mSampleRate);
    return mSampleRate;
}

/* Calculate the buffersize used to ADM */
size_t AudioStreamOutANM::calc_buffer_size() const
{
    if (get_adm_format() >= STE_ADM_FORMAT_FIRST_CODED) {
        ALOG_INFO_VERBOSE("calc_buffer_size(): Coded format, using %d bytes buffers\n", CODED_FORMAT_BUFFER_SIZE);
        return CODED_FORMAT_BUFFER_SIZE;
    }

    unsigned int size = 4096; // power of two --> should meet granularity requirements easily
    unsigned int channels = AudioSystem::popCount(mChannels);
    if (mSampleRate != 0 && channels != 0) {
        size = mSampleRate * LOW_POWER_BUFFER_DURATION / 1000 * channels * 2;
        unsigned int granularity = BUFFER_GRANULARITY_IN_SAMPLES * 2 * channels;
        size = (size / granularity) * granularity;
    }

    return size;
}

/* Return size of output buffer */
/* Part of AudioStreamOut interface */
size_t AudioStreamOutANM::bufferSize() const
{
    unsigned int size = 4096; // power of two --> should meet granularity requirements easily
    unsigned int channels = AudioSystem::popCount(mChannels);
    if (mSampleRate != 0 && channels != 0) {
        size = mSampleRate * AUDIO_FLINGER_BUFFER_DURATION / 1000 * channels * 2;
        unsigned int granularity = BUFFER_GRANULARITY_IN_SAMPLES * 2 * channels;
        size = (size / granularity) * granularity;
    }
    ALOG_INFO_VERBOSE("bufferSize(): %d bytes (%d, %d), %d Hz\n",
        size * NUMBER_OF_BUFFERS_PER_WRITE,
        size, NUMBER_OF_BUFFERS_PER_WRITE, mSampleRate);
    return size * NUMBER_OF_BUFFERS_PER_WRITE;
};

/*
 * Return audio channels bitmask
 * Part of AudioStreamOut interface
 */
uint32_t AudioStreamOutANM::channels() const
{
    uint32_t channels = mChannels;
    ALOG_INFO_VERBOSE("channels(): 0x%08X, %d\n",
        channels, AudioSystem::popCount(channels));
    return channels;
}

/*
 * return audio format in 8bit or 16bit PCM format -
 * eg. AudioSystem:PCM_16_BIT
 * Part of AudioStreamOut interface
 */
int AudioStreamOutANM::format() const
{
    ALOG_INFO_VERBOSE("format(): %d\n", mFormat);
    return mFormat;
}

/*
 * return the audio hardware driver latency in milli seconds.
 * Part of AudioStreamOut interface
 */
uint32_t AudioStreamOutANM::latency() const
{
    // Temporary solution to work around a problem in stagefright.
    uint32_t buf_comp = 2 * 1000 * bufferSize() / (mSampleRate * AudioSystem::popCount(mChannels) * 2);
    ALOG_INFO_VERBOSE("latency(): mLatency=%d buf_comp=%u Total=%d\n",
        mLatency, buf_comp, mLatency + buf_comp);

    return mLatency + buf_comp;
}

/*
 * Use this method in situations where audio mixing is done in the
 * hardware. This method serves as a direct interface with hardware,
 * allowing you to directly set the volume as apposed to via the framework.
 * This method might produce multiple PCM outputs or hardware accelerated
 * codecs, such as MP3 or AAC.
 * Part of AudioStreamOut interface
 */
status_t AudioStreamOutANM::setVolume(float left, float right)
{
    /** @todo: Implement TODO */
    return NO_ERROR;
}

/* write audio buffer to driver. Returns number of bytes written */
/* Part of AudioStreamOut interface */
ssize_t AudioStreamOutANM::write(const void *buffer, size_t bytes)
{
    int err;
    unsigned int consumed_bytes = 0;

    if (mStandby) {
        ALOG_INFO("write(): Open device since in standby.\n");

        pthread_mutex_lock(&mMutex);
        status_t status = this->openDevices();
        pthread_mutex_unlock(&mMutex);

        if (status != OK) {
            ALOG_ERR("write(): Failed to open output device!\n");
            return BAD_VALUE;
        }
    }

    if (bytes == 0 || bytes > (size_t) mAdmBufSize) {
        ALOG_ERR("write(): Requested to write invalid amount of bytes! (%d, mAdmBufSize=%d)\n", bytes, mAdmBufSize);
        return BAD_VALUE;
    }

    if (mAdmBufSharedMem == NULL) {
        ALOG_ERR("write(): AudioStreamOutANM::write - output buffer not yet allocated\n");
        return INVALID_OPERATION;
    }

    if (mNewADMConnID > 0) {

        pthread_mutex_lock(&mMutex);

        /* Audio has been rerouted, switch to new connection */
        ALOG_INFO("write(): Routing is changed. Old connection=%d, New connection=%d (this=%p)", mADMConnectionID, mNewADMConnID, this);
        mOldADMConnID = mADMConnectionID;
        mADMConnectionID = mNewADMConnID;
        mNewADMConnID = -1;

        munmap(mAdmBufSharedMem, mAdmBufSize * mAdmNumBufs);
        mAdmBufSharedMem = mNewAdmBufSharedMem;
        mAdmBufSize      = mNewAdmBufSize;
        mAdmNumBufs      = mNewAdmNumBufs;
        mCurBufIdx       = 0;


        /* Refresh device list from new routing info */
        refreshDeviceList(mDeviceList, mNewDevices);
        mOldDevices = mDevices;
        mDevices = mNewDevices;
        mNewDevices = 0;

        pthread_mutex_unlock(&mMutex);

        /* Get new latency value */
        mLatency = 0;
        for (cviter it = mDeviceList.begin(); it != mDeviceList.end(); ++it) {
            int latency;
            int res = ste_adm_client_max_out_latency(*it, &latency);
            if (res < 0) {
                ALOG_WARN("write(): Failed to get latency for device %s,res = %d\n", *it, res);
            } else {
                ALOG_INFO("write(): Latency for device %s = %d\n", *it, latency);
                if (latency > mLatency) {
                    mLatency = latency;
                }
            }
        }

        /* Close old devices and connection */
        pthread_t thread;
        pthread_attr_t tattr;
        pthread_attr_init(&tattr);
        pthread_attr_setdetachstate(&tattr, PTHREAD_CREATE_DETACHED);
        if (pthread_create(&thread, &tattr, close_dev_thread, this)) {
            ALOG_ERR("write(): pthread_create() failed\n");
            return 0;
        }
    } else if (mNewDevices != 0) {
        if (!mChangingDevice) {
            ALOG_ERR("write(): Changing to new devices failed, connection %d (this=%p)", mADMConnectionID, this);

            pthread_mutex_lock(&mMutex);

            /* mNewDevices is not 0 and open_dev_thread is not running. This
               means that changing to new devices failed in open_dev_thread */
            ALOG_INFO("write(): mNewDevices = %d", mNewDevices);
            /* Go to standby (drain and close devices) */
            status_t status = this->standby_l();
            if (status != NO_ERROR) {
                ALOG_ERR("write(): Failed to go to standby mode, continue anyway\n");
                mStandby = true;
            }
            ALOG_INFO("write(): Disconnect from ADM, ID = %d", mADMConnectionID);
            ste_adm_client_disconnect(mADMConnectionID);
            mDevices = mNewDevices;
            /* Refresh device list with new devices */
            refreshDeviceList(mDeviceList, mNewDevices);
            mNewDevices = 0;
            mADMConnectionID = ste_adm_client_connect();
            ALOG_INFO("write(): Connect to ADM, ID = %d", mADMConnectionID);

            pthread_mutex_unlock(&mMutex);
        }
    }

    if (mStandbyPending) {
        ALOG_INFO("write(): mStandbyPending TRUE, resetting\n");
        mStandbyPending = false;
    }

    ALOG_INFO_FL("write(): %d bytes from buffer 0x%08X (this=%p)\n",
        (int) bytes, (uint32_t) buffer, this);


    while (consumed_bytes < bytes) {
        int fillLength;
        if (mLpaMode == 0 && get_adm_format() < STE_ADM_FORMAT_FIRST_CODED) {
            fillLength = bytes;
        }
        else {
            fillLength = mAdmBufSize;
        }

        unsigned int transfer_bytes = MIN(MIN(fillLength, mAdmBufSize - mCurBufOffset), bytes - consumed_bytes);

        ALOG_INFO_FL("write(): Copy %d bytes to buffer (%d) 0x%08X at offset %d\n",
                    transfer_bytes, mCurBufIdx, (uint32_t) mAdmBufSharedMem + mCurBufIdx*mAdmBufSize, mCurBufOffset);
        memcpy(mAdmBufSharedMem + mCurBufIdx*mAdmBufSize + mCurBufOffset,
               ((const char*) buffer) + consumed_bytes,
               transfer_bytes);
        mCurBufOffset += transfer_bytes;

        if (mCurBufOffset >= fillLength) {
            int newLpaMode = 0;
            err = ste_adm_client_send(mADMConnectionID, mCurBufIdx, mCurBufOffset, &newLpaMode);
            if(mLpaMode != newLpaMode) {
                ALOG_INFO("write(): LPA mode changed from %d to %d", mLpaMode, newLpaMode);
                mLpaMode = newLpaMode;
            }
            ALOG_INFO_FL("write(): Sent %d bytes to ADM\n", mCurBufOffset);

            mCurBufIdx = (mCurBufIdx + 1) % mAdmNumBufs;
            mCurBufOffset = 0;

            if (err == STE_ADM_RES_ERR_MSG_IO) {

                pthread_mutex_lock(&mMutex);

                /* connection to ADM has been lost or other connection issues, possible ADM reboot
                   try to re-initiate the connection */
                /* try to close but igore error message if there are any */
                int tmp_err;
                for (cviter it = mDeviceList.begin(); it != mDeviceList.end(); ++it) {
                    tmp_err = ste_adm_close_device(mADMConnectionID, *it);
                    if (tmp_err != STE_ADM_RES_OK) {
                        ALOG_WARN("write(): Lost connection to ADM, trying to close device returned error=%d fd=%d\n",
                                  tmp_err, mADMConnectionID);
                    }
                }
                /* disconnect ADM connection */
                tmp_err = ste_adm_client_disconnect(mADMConnectionID);
                mCurBufIdx = 0;
                mCurBufOffset = 0;
                if (tmp_err != STE_ADM_RES_OK) {
                    ALOG_WARN("write(): Lost connection to ADM, trying to disconnect returned error=%d fd=%d\n",
                              tmp_err, mADMConnectionID);
                }
                /* open new connection */
                mADMConnectionID = ste_adm_client_connect();
                if (mADMConnectionID < 0) {
                    /* failed to re-connect */
                    ALOG_ERR("write(): ste_adm_client_connect() failed in write");
                    pthread_mutex_unlock(&mMutex);
                    return FAILED_TRANSACTION;
                }
                ALOG_INFO("write(): Re-connected to ADM after lost connection, ID = %d", mADMConnectionID);

                pthread_mutex_unlock(&mMutex);

                /* Check call status and recover call graph if needed */
                String8 keyValuePairs = android::AudioSystem::getParameters(0, String8(PARAM_KEY_AP_REF));
                AudioParameter apParam = AudioParameter(keyValuePairs);
                int ptr = 0;
                if (apParam.getInt(String8(PARAM_KEY_AP_REF), ptr) == NO_ERROR) {
                    ((AudioPolicyManagerANM*)ptr)->checkCallStatus();
                }

                pthread_mutex_lock(&mMutex);

                /* re-open all new devices */
                mAdmNumBufs = 3;
                if (mNewDevices != 0) {
                    if (!mChangingDevice) {
                        /* mNewDevices is not 0 and open_dev_thread is not running.
                           This means that changing to new devices failed in open_dev_thread */
                        ALOG_INFO("write(): mNewDevices = %d", mNewDevices);
                        mDevices = mNewDevices;
                        /* Refresh device list from new routing info */
                        refreshDeviceList(mDeviceList, mNewDevices);
                        mNewDevices = 0;
                    }
                }

                for (cviter it = mDeviceList.begin(); it != mDeviceList.end(); ++it) {
                    tmp_err = ste_adm_client_open_device(mADMConnectionID, *it,
                    sampleRate(), get_adm_format(), NULL,
                        &mAdmBufSharedMem, mAdmBufSize, mAdmNumBufs);
                    if (tmp_err != STE_ADM_RES_OK) {
                        ALOG_ERR("write(): Re-connect to ADM, failed to open device %s, err=%d\n", *it, tmp_err);
                    }
                }

                pthread_mutex_unlock(&mMutex);

                /* re-connected and re-opened devices to ADM */
                ALOG_INFO("write(): Reconnected to ADM server, new fd=%d\n", mADMConnectionID);
                continue;
            } else if (err != STE_ADM_RES_OK) {
                ALOG_ERR("write(): Failed to send audio data to ADM: %d\n", err);
                return FAILED_TRANSACTION;
            }
        }
        consumed_bytes += transfer_bytes;
    }

    ALOG_INFO_FL("write(): Completed\n");

    return bytes;
}

/*
 * Put the audio hardware output into standby mode. Returns
 * status based on include/utils/Errors.h
 */
void AudioStreamOutANM::preCloseDevice()
{
    ALOG_INFO_VERBOSE("preCloseDevice\n");

    ALOG_INFO_VERBOSE("preCloseDevice(): Trying to drain devices\n");
    (void) ste_adm_client_drain(mADMConnectionID);
    ALOG_INFO_VERBOSE("preCloseDevice(): Drain devices done\n");
}

/* dump the state of the audio output device */
/* Part of AudioStreamOut interface */
status_t AudioStreamOutANM::dump(int fd, const Vector < String16 > &args)
{
    ALOG_INFO_VERBOSE("dump\n");
    return NO_ERROR;
}

/********************* Implementation of additional member functions ***********
*
*
*******************************************************************************/

status_t AudioStreamOutANM::changeDevice(int mode, uint32_t routes)
{
    status_t status = OK;

    ALOG_INFO("changeDevice(): "
        "routes (0x%08X) mStandby (%d) (this=%p)\n", routes, mStandby, this);

    pthread_mutex_lock(&mMutex);

    if (!mStandby) {
        if (routes != mDevices) {
            ALOG_INFO_VERBOSE("changeDevice(): after mutex, routes (0x%08X)", routes);

            mChangingDevice = true;

            /* Check if there is already a new connection, which has not been closed */
            if (mNewADMConnID > 0) {
                ALOG_INFO("changeDevice(): Close previous unused ADM connection");
                (void) ste_adm_client_drain(mNewADMConnID);
                List<const char*> deviceList;
                refreshDeviceList(deviceList, mNewDevices);
                for (cviter it = deviceList.begin(); it != deviceList.end(); ++it) {
                    ALOG_INFO("changeDevice(): Close device %s in ADM connection %d\n", *it, mNewADMConnID);
                    int err = ste_adm_close_device(mNewADMConnID, *it);
                    if (err != STE_ADM_RES_OK) {
                        ALOG_ERR("changeDevice(): Failed to close device %s!\n", *it);
                    }
                }
                ALOG_INFO("changeDevice(): Disconnect from ADM, ID = %d", mNewADMConnID);
                ste_adm_client_disconnect(mNewADMConnID);
                mNewADMConnID = -1;
            }

            int devicesToClose = mDevices & ~(mDevices & routes);
            int devicesToOpen = routes & ~(mDevices);

            /* Open A2DP devices in separate thread since it takes very long time.
               Don't open in separate thread if the previous device was BT-SCO since BT-SCO and A2DP can't be open simultaneously.
               Same solution when a headset or headphone is plugged during a playback on speaker to avoid video frames loss. */
            if (((devicesToClose == AudioSystem::DEVICE_OUT_SPEAKER) && (devicesToOpen == AudioSystem::DEVICE_OUT_WIRED_HEADSET)) ||
                ((devicesToClose == AudioSystem::DEVICE_OUT_SPEAKER) && (devicesToOpen == AudioSystem::DEVICE_OUT_WIRED_HEADPHONE)) ||
                ((routes & AudioSystem::DEVICE_OUT_ALL_A2DP) &&
                !(mDevices & AudioSystem::DEVICE_OUT_ALL_A2DP) &&
                !(mDevices & (AudioSystem::DEVICE_OUT_BLUETOOTH_SCO | AudioSystem::DEVICE_OUT_BLUETOOTH_SCO_HEADSET | AudioSystem::DEVICE_OUT_BLUETOOTH_SCO_CARKIT)))) {
                /* Open the device to which audio should be routed */
                mNewDevices = routes;
                pthread_t thread;
                pthread_attr_t tattr;
                pthread_attr_init(&tattr);
                pthread_attr_setdetachstate(&tattr, PTHREAD_CREATE_DETACHED);
                if (pthread_create(&thread, &tattr, open_dev_thread, this)) {
                    ALOG_ERR("changeDevice(): pthread_create() failed\n");
                    mChangingDevice = false;
                    pthread_mutex_unlock(&mMutex);
                    return NO_INIT;
                }
                // keep the mutex; will be unlocked by open_dev_thread
            }
            else {
                ALOG_INFO_VERBOSE("changeDevice(): routes = %Xh, devicesToClose = %Xh, devicesToOpen = %Xh. Closing old.", routes, devicesToClose,devicesToOpen);

                (void) ste_adm_client_drain(mADMConnectionID);

                List<const char*> deviceList;
                refreshDeviceList(deviceList, devicesToClose);
                for (cviter it = deviceList.begin(); it != deviceList.end(); ++it) {
                    ALOG_INFO("changeDevice(): Close device %s in ADM connection %d\n", *it, mADMConnectionID);
                    int err = ste_adm_close_device(mADMConnectionID, *it);
                    if (err != STE_ADM_RES_OK) {
                        ALOG_ERR("changeDevice(): Failed to close device %s!\n", *it);
                    }
                }

                ALOG_INFO_VERBOSE("changeDevice(): Old device closed, opening new device");

                int err = 0;
                int retries = 10;
                do {
                    List<const char*> deviceList;
                    refreshDeviceList(deviceList, devicesToOpen);
                    for (cviter it = deviceList.begin(); it != deviceList.end(); ++it) {
                        int num_bufs = 3;
                        int bufsz = calc_buffer_size();
                        char *bufp;

                        ALOG_INFO("changeDevice(): Open device %s in ADM connection %d\n", *it, mADMConnectionID);
                        err = ste_adm_client_open_device(mADMConnectionID, *it,
                            sampleRate(), get_adm_format(), NULL,
                            &bufp, bufsz, num_bufs);
                        if (err != STE_ADM_RES_OK) {
                            ALOG_ERR("changeDevice(): Failed to open device %s\n", *it);
                            break;
                        }

                        assert(mAdmBufSharedMem == NULL || mAdmBufSharedMem == bufp);
                        assert(mAdmBufSize      == 0    || mAdmBufSize      == num_bufs);
                        assert(mAdmNumBufs      == 0    || mAdmNumBufs      == num_bufs);

                        mAdmBufSharedMem = bufp;
                        mAdmBufSize      = bufsz;
                        mAdmNumBufs      = num_bufs;
                    }

                    mDevices = routes;
                    refreshDeviceList(mDeviceList, routes);

                    if (err != STE_ADM_RES_OK) {
                        /* Failed to open device, close the connection */
                        int tmp_err;
                        for (cviter it = deviceList.begin(); it != deviceList.end(); ++it) {
                            tmp_err = ste_adm_close_device(mADMConnectionID, *it);
                            if (tmp_err != STE_ADM_RES_OK) {
                                ALOG_WARN("changeDevice(): Lost connection to ADM, trying to close "
                                    "device returned error=%d\n", tmp_err);
                            }
                        }
                        ste_adm_client_disconnect(mADMConnectionID);
                        ALOG_INFO("changeDevice(): Connect to ADM, ID = %d", mADMConnectionID);
                        if (mAdmBufSharedMem != NULL) {
                            munmap(mAdmBufSharedMem, mAdmBufSize * mAdmNumBufs);
                            mAdmBufSharedMem = NULL;
                            mAdmBufSize      = 0;
                            mAdmNumBufs      = 0;
                        }

                        /* Open new connection */
                        mADMConnectionID = ste_adm_client_connect();
                    }
                    retries--;
                } while (err != STE_ADM_RES_OK && retries > 0);

                if (err == STE_ADM_RES_OK) {
                    ALOG_INFO_VERBOSE("changeDevice(): Opened new device");
                }
                else {
                    ALOG_ERR("changeDevice(): Failed to open devices");
                    status = UNKNOWN_ERROR;
                }

                mChangingDevice = false;
            }
        }
    } else {
        /* Refresh device list from new routing info */
        refreshDeviceList(mDeviceList, routes);
        mDevices = routes;
    }
    pthread_mutex_unlock(&mMutex);

    return status;
}


status_t AudioStreamOutANM::setup(
    int *format, uint32_t *channels, uint32_t *sampleRate)
{
    status_t status = OK;

    ALOG_INFO_VERBOSE("setup(): samplerate %d, channels %x (%d), "
        "format %d\n", *sampleRate, *channels, AudioSystem::popCount(*channels),
        *format);

    /* First of all, connect to ADM */
    if (this->setupADMConnection() < 0) {
        ALOG_ERR("setup():Failed to setup ADM Connection\n");
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

    return status;
}

/* Part of AudioStreamOut interface */
status_t AudioStreamOutANM::getRenderPosition(uint32_t *dspFrames) {
  /**@todo: implement */
  *dspFrames=0;
  return NO_ERROR;
}

void AudioStreamOutANM::postOpenDevice(void)
{
    ALOG_INFO_VERBOSE("postOpenDevice(): do nothing\n");
}

/* Part of AudioStreamOut interface */
status_t AudioStreamOutANM::standby()
{
    status_t status;
    pthread_mutex_lock(&mMutex);
    status = standby_l();
    pthread_mutex_unlock(&mMutex);
    return status;
}

/* Mutex mMutex MUST be held when calling this function */
status_t AudioStreamOutANM::standby_l()
{
    status_t status;

    ALOG_INFO("ENTER standby() (this=%p)\n", this);

    status = standby_imp();
    if (status != OK) {
        ALOG_ERR("standby(): Failed to go to standby!\n");
        return status;
    }

    if (mNewADMConnID > 0) {

        /* Switch to new connection */
        mOldADMConnID = mADMConnectionID;
        mADMConnectionID = mNewADMConnID;
        mNewADMConnID = -1;
        ALOG_INFO("standby(): Disconnect from ADM, ID = %d", mOldADMConnID);
        ste_adm_client_disconnect(mOldADMConnID);
        mCurBufIdx = 0;
        mOldADMConnID = -1;

        /* Refresh device list from new routing info */
        refreshDeviceList(mDeviceList, mNewDevices);
        ALOG_INFO("standby(): new devices = %X", (int) mNewDevices);
        mDevices = mNewDevices;
        mNewDevices = 0;
        mStandby = 0;

        status = standby_imp();

        if (status != OK) {
            ALOG_ERR("standby(): Failed to go to standby!\n");
            return status;
        }
    }
    ALOG_INFO("LEAVE standby()\n");
    return status;
}

void* AudioStreamOutANM::report_latency_thread(void* param)
{
    AudioParameter *latencyParam = (AudioParameter*)param;
    audio_io_handle_t output;

    if (latencyParam->getInt(String8("output"), output) != NO_ERROR) {
        delete latencyParam;
        return NULL;
    }
    latencyParam->remove(String8("output"));

    android::AudioSystem::setParameters(output, latencyParam->toString());

    delete latencyParam;
    return NULL;
}

void AudioStreamOutANM::notifySinkLatency(audio_io_handle_t output)
{
    pthread_mutex_lock(&mMutex);

    /* Get current devices */
    List<const char*> deviceList;
    if (mNewDevices == 0) {
        deviceList = mDeviceList;
    } else {
        /* Routing change ongoing, use the new device list */
        refreshDeviceList(deviceList, mNewDevices);
    }

    /* Get latency for any A2DP device */
    uint32_t sinkLatency = latency();
    for (cviter it = deviceList.begin(); it != deviceList.end(); ++it) {
        if (strcmp(STE_ADM_DEVICE_STRING_A2DP, *it) == 0) {
            ALOG_INFO("Retrieving sink latency for device %s\n", *it);

            uint32_t latency;
            int res = ste_adm_client_get_sink_latency(*it, &latency);
            if (res < 0) {
                ALOG_WARN("Failed to get sink latency for device %s\n", *it);
                pthread_mutex_unlock(&mMutex);
                return;
            }
            sinkLatency += latency;
            break;
        }
    }
    pthread_mutex_unlock(&mMutex);

    /* Report sink latency */
    pthread_t thread;
    pthread_attr_t tattr;
    pthread_attr_init(&tattr);
    pthread_attr_setdetachstate(&tattr, PTHREAD_CREATE_DETACHED);
    AudioParameter *latencyParam = new AudioParameter();
    latencyParam->addInt(String8("output"), output);
    latencyParam->addInt(String8(AudioParameter::keySinkLatency), sinkLatency);
    if (pthread_create(&thread, &tattr, report_latency_thread, latencyParam)) {
        ALOG_ERR("pthread_create() failed, no latency will be reported\n");
    }
}


/*********************** Implementation of File scope functions ****************
*
*
*******************************************************************************/
void AudioStreamOutANM::refreshDeviceList(List <const char*>&v, uint32_t routes)
{
    v.clear();

    if (routes & AudioSystem::DEVICE_OUT_EARPIECE) {
        v.push_back(STE_ADM_DEVICE_STRING_EARP);
    }

    if (routes & AudioSystem::DEVICE_OUT_SPEAKER) {
        v.push_back(STE_ADM_DEVICE_STRING_SPEAKER);
    }

    if ((routes & AudioSystem::DEVICE_OUT_BLUETOOTH_SCO) ||
        (routes & AudioSystem::DEVICE_OUT_BLUETOOTH_SCO_HEADSET) ||
        (routes & AudioSystem::DEVICE_OUT_BLUETOOTH_SCO_CARKIT)) {
        bool isBTWB = false; /* Bluetooth Wideband (16kHz) */
        String8 keyValuePairs = android::AudioSystem::getParameters(0, String8(PARAM_KEY_BTWB));
        AudioParameter param = AudioParameter(keyValuePairs);
        String8 value;
        if (param.get(String8(PARAM_KEY_BTWB), value) == NO_ERROR) {
            isBTWB = (value == "on");
        }
        if (isBTWB) {
            v.push_back(STE_ADM_DEVICE_STRING_BTWBOUT);
        } else {
            v.push_back(STE_ADM_DEVICE_STRING_BTOUT);
        }
    }

    if ((routes & AudioSystem::DEVICE_OUT_WIRED_HEADSET) ||
        (routes & AudioSystem::DEVICE_OUT_WIRED_HEADPHONE)) {
        v.push_back(STE_ADM_DEVICE_STRING_HSOUT);
    }

    if ((routes & AudioSystem::DEVICE_OUT_BLUETOOTH_A2DP) ||
        (routes & AudioSystem::DEVICE_OUT_BLUETOOTH_A2DP_HEADPHONES) ||
        (routes & AudioSystem::DEVICE_OUT_BLUETOOTH_A2DP_SPEAKER)) {
        v.push_back(STE_ADM_DEVICE_STRING_A2DP);
    }

    if (routes & AudioSystem::DEVICE_OUT_FM_RADIO_TX) {
        v.push_back(STE_ADM_DEVICE_STRING_FMTX);
    }

    if (routes & AudioSystem::DEVICE_OUT_AUX_DIGITAL) {
        v.push_back(STE_ADM_DEVICE_STRING_HDMI);
    }
    if (routes & AudioSystem::DEVICE_OUT_ANLG_DOCK_HEADSET) {
        v.push_back(STE_ADM_DEVICE_STRING_USBOUT);
    }

    if (routes & AudioSystem::DEVICE_OUT_DEFAULT) {
        v.push_back(STE_ADM_DEVICE_STRING_VOIPOUT);
    }
}
