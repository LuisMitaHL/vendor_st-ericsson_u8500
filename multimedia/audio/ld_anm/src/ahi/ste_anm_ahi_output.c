/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*! \file ste_anm_ahioutlib.cc
*   \brief ANM implementation of AHI Output interface.

    Implements the Output specific parts of AHI defined in
    Android and used by the AudioFlinger. The output specific parts consists of
    the methods defined in audio_stream_out.
*/

#define ANM_LOG_FILENAME "anm_ahi_output"
#include "ste_hal_anm_dbg.h"
#include "ste_hal_anm_util.h"
#include "ste_hal_anm_ahi.h"
#include "ste_hal_anm_ap.h"
#include <sys/mman.h>
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
#define DEFAULT_CHANNELS AUDIO_CHANNEL_OUT_STEREO

/**
* The default PCM format returned to interface client.
*/
#ifndef DEFAULT_PCM_FORMAT
#define DEFAULT_PCM_FORMAT AUDIO_FORMAT_PCM_16_BIT
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


static int MIN(int x, int y) { return x<y ? x : y; }

void* ste_audio_stream_out_open_dev_thread(void* param)
{
    struct audio_stream_out_anm *out = (struct audio_stream_out_anm*)param;
    struct audio_stream      *stream = (struct audio_stream*)out;
    struct stream_adm_base *adm_base = &out->adm_base;
    int i;
    struct node *list_elem;
    ALOG_INFO("ENTER ste_audio_stream_out_open_dev_thread(): (this=%p)", out);
    int err = 0;
    int newADMConnID = ste_adm_client_connect();
    ALOG_INFO("ste_audio_stream_out_open_dev_thread(): New ADM connection has fd = %d", newADMConnID);

    struct node *deviceList = NULL;
    ste_audio_stream_out_refresh_device_list(&deviceList, out->mNewDevices, out);
    for (i=0; i < list_count(deviceList); i++) {
        list_elem = list_get_nth(deviceList, i);

        int num_bufs = 3;
        int bufsz = adm_base->calc_buffer_size(adm_base);
        char *bufp;

        ALOG_INFO("ste_audio_stream_out_open_dev_thread(): Open device %s in ADM connection %d\n", list_elem->key, newADMConnID);
        err = ste_adm_client_open_device(newADMConnID, list_elem->key,
            ste_audio_stream_out_get_sample_rate(stream), admbase_get_adm_format(&out->adm_base), NULL,
            &bufp, bufsz, num_bufs);
        if (err != STE_ADM_RES_OK) {
            ALOG_ERR("ste_audio_stream_out_open_dev_thread(): Failed to open device %s, err = %d\n", list_elem->key, err);
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
    pthread_mutex_unlock(&(adm_base->mMutex));

    if (err == STE_ADM_RES_OK) {
        /* Now that the mutex has been released, check if sink latency
           shall be notified to client */
        if (out->mLatencyReportOutput != 0) {
            ste_audio_stream_out_notify_sink_latency(stream, out->mLatencyReportOutput);
            out->mLatencyReportOutput = 0;
        }
    }

    ALOG_INFO("LEAVE ste_audio_stream_out_open_dev_thread():");
    return NULL;
}



void* ste_audio_stream_out_close_dev_thread(void* param)
{
    struct audio_stream_out_anm *out = (struct audio_stream_out_anm*)param;
    struct audio_stream      *stream = (struct audio_stream*)out;
    struct stream_adm_base *adm_base = &out->adm_base;
    struct node *list_elem;
    int i;

    ALOG_INFO("ENTER ste_audio_stream_out_close_dev_thread(): fd to close is %d (this=%p)", out->mOldADMConnID, out);
    pthread_mutex_lock(&(adm_base->mMutex));

    (void) ste_adm_client_drain(out->mOldADMConnID);

    struct node *deviceList = NULL;
    ste_audio_stream_out_refresh_device_list(&deviceList, out->mOldDevices, out);
    for (i=0; i < list_count(deviceList); i++) {
        list_elem = list_get_nth(deviceList, i);
        ALOG_INFO("ste_audio_stream_out_close_dev_thread(): Close device %s in ADM connection %d\n", list_elem->key, out->mOldADMConnID);
        int err = ste_adm_close_device(out->mOldADMConnID, list_elem->key);
        if (err != STE_ADM_RES_OK) {
            ALOG_ERR("ste_audio_stream_out_close_dev_thread(): Failed to close device %s! err=%d\n", list_elem->key, err);
        }
    }

    ALOG_INFO("ste_audio_stream_out_close_dev_thread(): Disconnect from ADM, ID = %d", out->mOldADMConnID);
    ste_adm_client_disconnect(out->mOldADMConnID);
    out->mOldADMConnID = -1;
    out->mOldDevices = 0;

    pthread_mutex_unlock(&(adm_base->mMutex));
    ALOG_INFO("LEAVE ste_audio_stream_out_close_dev_thread(): pthread_self=%X", (int) pthread_self());
    return NULL;
}

/* React to live parameter changes routing, sample rate etc... */
status_t ste_audio_stream_out_set_parameters(struct audio_stream *stream, const char *kv_pairs)
{
    struct audio_stream_out_anm  *stream_out = (struct audio_stream_out_anm *)stream;
    struct str_parms *parms;
    int value_int;
    int ret;

    status_t status = NO_ERROR;
    int devices = 0;
    int output = 0;
    int standby = 0;

    ALOG_INFO("ste_audio_stream_out_set_parameters(): %s (stream=%p)", kv_pairs, stream);

    parms = str_parms_create_str(kv_pairs);

    ret = str_parms_get_int(parms, AUDIO_PARAMETER_STREAM_ROUTING, &value_int);
    if (ret >= 0) {
        devices = value_int;
        char buf[400];
        ALOG_INFO("ste_audio_stream_out_set_parameters(): %s 0x%08X %s\n", AUDIO_PARAMETER_STREAM_ROUTING,
            devices, admbase_devices_2_str(&stream_out->adm_base, buf, (audio_devices_t) devices));
        status = ste_audio_stream_out_change_device(stream, 0, devices);
        str_parms_destroy(parms);
        return status;
    }

    ret = str_parms_get_int(parms, PARAM_KEY_SINK_LATENCY, &value_int);
    if (ret >= 0) {
        output = value_int;
        stream_out->mLatencyReportOutput = output;
        str_parms_destroy(parms);
        return status;
    }

    ret = str_parms_get_int(parms, PARAM_KEY_STREAM_STANDBY, &value_int);
    if (ret >= 0) {
        standby = value_int;
        status = ste_audio_stream_out_standby(stream);
        str_parms_destroy(parms);
        return status;
    }
    str_parms_dump(parms);
    str_parms_destroy(parms);
    ALOG_INFO("ste_audio_stream_out_set_parameters(): Unhandled key, forward to admbase_set_parameters\n");
    status = admbase_set_parameters(&stream_out->adm_base, kv_pairs);
    return status;
}

char* ste_audio_stream_out_get_parameters(const struct audio_stream *stream, const char *key)
{
    struct audio_stream_out_anm  *stream_out = (struct audio_stream_out_anm *)stream;
    struct str_parms *parms;
    int ste_stream_state;
    char *str=NULL;
    int ret,ptr;

    ALOG_INFO_VERBOSE("ste_audio_stream_out_get_parameters(): " );

    if(!strcmp(key, PARAM_KEY_STREAM_REF))
    {
        ptr = (int)stream;
        parms = str_parms_create_str(key);
        ret = str_parms_add_int(parms, PARAM_KEY_STREAM_REF, ptr);
        str = str_parms_to_str(parms);
        str_parms_destroy(parms);
        ALOG_INFO_VERBOSE("ste_audio_stream_out_get_parameters(): %s", str);
        return str;
    } else {
        return admbase_get_parameters(&stream_out->adm_base, key);
    }
}

/* Return audio sample rate in Hz - default is 48000 Hz */
uint32_t ste_audio_stream_out_get_sample_rate(const struct audio_stream *stream)
{
    struct audio_stream_out_anm  *stream_out = (struct audio_stream_out_anm *)stream;
    ALOG_INFO_VERBOSE("ste_audio_stream_out_get_sample_rate(): %d Hz\n",
        stream_out->adm_base.mSampleRate);
    return stream_out->adm_base.mSampleRate;
}

/* Calculate the buffersize used to ADM */
size_t ste_audio_stream_out_calc_buffer_size(struct stream_adm_base *adm_base)
{
    if (admbase_get_adm_format(adm_base) >= STE_ADM_FORMAT_FIRST_CODED) {
        ALOG_INFO_VERBOSE("ste_audio_stream_out_calc_buffer_size(): Coded format, using %d bytes buffers\n", CODED_FORMAT_BUFFER_SIZE);
        return CODED_FORMAT_BUFFER_SIZE;
    }

    unsigned int size = 4096; // power of two --> should meet granularity requirements easily
    unsigned int channels = popcount(adm_base->mChannels);
    if (adm_base->mSampleRate != 0 && channels != 0) {
        size = adm_base->mSampleRate * LOW_POWER_BUFFER_DURATION / 1000 * channels * 2;
        unsigned int granularity = BUFFER_GRANULARITY_IN_SAMPLES * 2 * channels;
        size = (size / granularity) * granularity;
    }
    return size;
}

/* Return size of output buffer */
size_t ste_audio_stream_out_get_buffer_size(const struct audio_stream *stream)
{
    struct audio_stream_out_anm  *stream_out = (struct audio_stream_out_anm *)stream;
    unsigned int size = 4096; // power of two --> should meet granularity requirements easily
    unsigned int channels = popcount(stream_out->adm_base.mChannels);
    if (stream_out->adm_base.mSampleRate != 0 && channels != 0) {
        size = stream_out->adm_base.mSampleRate * AUDIO_FLINGER_BUFFER_DURATION / 1000 * channels * 2;
        unsigned int granularity = BUFFER_GRANULARITY_IN_SAMPLES * 2 * channels;
        size = (size / granularity) * granularity;
    }
    ALOG_INFO_VERBOSE("ste_audio_stream_out_get_buffer_size(): %d bytes (%d, %d), %d Hz\n",
        size * NUMBER_OF_BUFFERS_PER_WRITE,
        size, NUMBER_OF_BUFFERS_PER_WRITE, stream_out->adm_base.mSampleRate);
    return size * NUMBER_OF_BUFFERS_PER_WRITE;
}

/*
 * Return audio channels bitmask
 */
audio_channel_mask_t ste_audio_stream_out_get_channels(const struct audio_stream *stream)
{
    struct audio_stream_out_anm  *stream_out = (struct audio_stream_out_anm *)stream;
    audio_channel_mask_t channels =stream_out->adm_base.mChannels;
    ALOG_INFO_VERBOSE("ste_audio_stream_out_get_channels(): 0x%08X, %d\n",
        channels, popcount(channels));
    return channels;
}

/*
 * return audio format in 8bit or 16bit PCM format -
 */
audio_format_t ste_audio_stream_out_get_format(const struct audio_stream *stream)
{
    struct audio_stream_out_anm  *stream_out = (struct audio_stream_out_anm *)stream;
    ALOG_INFO_VERBOSE("ste_audio_stream_out_get_format(): %d\n", stream_out->adm_base.mFormat);
    return (audio_format_t)stream_out->adm_base.mFormat;
}

/*
 * return the audio hardware driver latency in milli seconds.
 * Part of audio_stream_out interface
 */
uint32_t ste_audio_stream_out_get_latency(const struct audio_stream_out *stream)
{
    struct audio_stream_out_anm  *stream_out = (struct audio_stream_out_anm *)stream;
    // Temporary solution to work around a problem in stagefright.
    uint32_t buf_comp = 2 * 1000 * ste_audio_stream_out_get_buffer_size((const struct audio_stream *)stream) / (stream_out->adm_base.mSampleRate * popcount(stream_out->adm_base.mChannels) * 2);
    ALOG_INFO_VERBOSE("ste_audio_stream_out_get_latency(): mLatency=%d buf_comp=%u Total=%d\n",
        stream_out->mLatency, buf_comp, stream_out->mLatency + buf_comp);

    return stream_out->mLatency + buf_comp;
}

/*
 * Use this method in situations where audio mixing is done in the
 * hardware. This method serves as a direct interface with hardware,
 * allowing you to directly set the volume as apposed to via the framework.
 * This method might produce multiple PCM outputs or hardware accelerated
 * codecs, such as MP3 or AAC.
 * Part of audio_stream_out interface
 */
int ste_audio_stream_out_set_volume(struct audio_stream_out *stream, float left, float right)
{
    /** @todo: Implement TODO */
    return NO_ERROR;
}

/* write audio buffer to driver. Returns number of bytes written */
/* Part of audio_stream_out interface */
ssize_t ste_audio_stream_out_write(struct audio_stream_out *stream, const void* buffer, size_t bytes)
{
    int err;
    unsigned int consumed_bytes = 0;
    struct audio_stream_out_anm *stream_out = (struct audio_stream_out_anm *)stream;
    stream_adm_base_t           *adm_base   = &stream_out->adm_base;
    struct node *list_elem;
    int i;
    audio_policy_anm *audio_policy_service_ops_ptr = NULL;

    if (adm_base->mStandby) {
        ALOG_INFO("ste_audio_stream_out_write(): Open device since in standby.\n");

        pthread_mutex_lock(&adm_base->mMutex);
        status_t status = admbase_open_devices(adm_base);
        pthread_mutex_unlock(&adm_base->mMutex);

        if (status != OK) {
            ALOG_ERR("ste_audio_stream_out_write(): Failed to open output device!\n");
            return BAD_VALUE;
        }
    }

    if (bytes == 0 || bytes > (size_t) adm_base->mAdmBufSize) {
        ALOG_ERR("ste_audio_stream_out_write(): Requested to write invalid amount of bytes! (%d, adm_base->mAdmBufSize=%d)\n", bytes, adm_base->mAdmBufSize);
        return BAD_VALUE;
    }

    if (adm_base->mAdmBufSharedMem == NULL) {
        ALOG_ERR("ste_audio_stream_out_write(): StreamOutANM_write - output buffer not yet allocated\n");
        return INVALID_OPERATION;
    }

    if (stream_out->mNewADMConnID > 0) {

        pthread_mutex_lock(&adm_base->mMutex);

        /* Audio has been rerouted, switch to new connection */
        ALOG_INFO("ste_audio_stream_out_write(): Routing is changed. Old connection=%d, New connection=%d (this=%p)", adm_base->mADMConnectionID, stream_out->mNewADMConnID, stream);
        stream_out->mOldADMConnID = stream_out->adm_base.mADMConnectionID;
        stream_out->adm_base.mADMConnectionID = stream_out->mNewADMConnID;
        stream_out->mNewADMConnID = -1;

        munmap(stream_out->adm_base.mAdmBufSharedMem, stream_out->adm_base.mAdmBufSize * stream_out->adm_base.mAdmNumBufs);
        stream_out->adm_base.mAdmBufSharedMem = stream_out->mNewAdmBufSharedMem;
        stream_out->adm_base.mAdmBufSize      = stream_out->mNewAdmBufSize;
        stream_out->adm_base.mAdmNumBufs      = stream_out->mNewAdmNumBufs;
        stream_out->mCurBufIdx                = 0;


        /* Refresh device list from new routing info */
        ste_audio_stream_out_refresh_device_list(&adm_base->mDeviceList, stream_out->mNewDevices, stream_out);
        stream_out->mOldDevices = stream_out->mDevices;
        stream_out->mDevices = stream_out->mNewDevices;
        stream_out->mNewDevices = 0;

        pthread_mutex_unlock(&adm_base->mMutex);

        /* Get new latency value */
        stream_out->mLatency = 0;

        for (i=0; i < list_count(adm_base->mDeviceList); i++) {
            int latency, res;
            list_elem = list_get_nth(adm_base->mDeviceList, i);
            res = ste_adm_client_max_out_latency(list_elem->key, &latency);
            if (res < 0) {
                ALOG_WARN("ste_audio_stream_out_write(): Failed to get latency for device %s,res = %d\n", list_elem->key, res);
            } else {
                ALOG_INFO("ste_audio_stream_out_write(): Latency for device %s = %d\n", list_elem->key, latency);
                if (latency > stream_out->mLatency) {
                    stream_out->mLatency = latency;
                }
            }
        }

        /* Close old devices and connection */
        pthread_t thread;
        pthread_attr_t tattr;
        pthread_attr_init(&tattr);
        pthread_attr_setdetachstate(&tattr, PTHREAD_CREATE_DETACHED);
        if (pthread_create(&thread, &tattr, ste_audio_stream_out_close_dev_thread, stream)) {
            ALOG_ERR("ste_audio_stream_out_write(): pthread_create() failed\n");
            return 0;
        }
    } else if (stream_out->mNewDevices != 0) {
        if (!stream_out->mChangingDevice) {
            ALOG_ERR("ste_audio_stream_out_write(): Changing to new devices failed, connection %d (this=%p)", adm_base->mADMConnectionID, stream);

            pthread_mutex_lock(&adm_base->mMutex);
            /* mNewDevices is not 0 and open_dev_thread is not running. This
               means that changing to new devices failed in open_dev_thread */
            ALOG_INFO("ste_audio_stream_out_write(): mNewDevices = %d", stream_out->mNewDevices);
            /* Go to standby (drain and close devices) */
            status_t status = ste_audio_stream_out_standby_l((struct audio_stream *)stream);
            if (status != NO_ERROR) {
                ALOG_ERR("ste_audio_stream_out_write(): Failed to go to standby mode, continue anyway\n");
                adm_base->mStandby = true;
            }
            ALOG_INFO("ste_audio_stream_out_write(): Disconnect from ADM, ID = %d", adm_base->mADMConnectionID);
            ste_adm_client_disconnect(adm_base->mADMConnectionID);
            stream_out->mDevices = stream_out->mNewDevices;
            /* Refresh device list with new devices */
            ste_audio_stream_out_refresh_device_list(&adm_base->mDeviceList, stream_out->mNewDevices, stream_out);
            stream_out->mNewDevices = 0;
            adm_base->mADMConnectionID = ste_adm_client_connect();
            ALOG_INFO("ste_audio_stream_out_write(): Connect to ADM, ID = %d", adm_base->mADMConnectionID);
            pthread_mutex_unlock(&adm_base->mMutex);
        }
    }

    if (adm_base->mStandbyPending) {
        ALOG_INFO("ste_audio_stream_out_write(): mStandbyPending TRUE, resetting\n");
        adm_base->mStandbyPending = false;
    }

    ALOG_INFO_FL("ste_audio_stream_out_write(): %d bytes from buffer 0x%08X (this=%p)\n",
        (int) bytes, (uint32_t) buffer, stream);

    while (consumed_bytes < bytes) {
        int fillLength;
        if (stream_out->mLpaMode == 0 && admbase_get_adm_format(&stream_out->adm_base) < STE_ADM_FORMAT_FIRST_CODED) {
            fillLength = bytes;
        }
        else {
            fillLength = adm_base->mAdmBufSize;
        }

        unsigned int transfer_bytes = MIN(MIN(fillLength, adm_base->mAdmBufSize - stream_out->mCurBufOffset), bytes - consumed_bytes);

        ALOG_INFO_FL("ste_audio_stream_out_write(): Copy %d bytes to buffer (%d) 0x%08X at offset %d\n",
                    transfer_bytes,stream_out->mCurBufIdx, (uint32_t) adm_base->mAdmBufSharedMem +stream_out->mCurBufIdx*adm_base->mAdmBufSize, stream_out->mCurBufOffset);
        memcpy(adm_base->mAdmBufSharedMem +stream_out->mCurBufIdx*adm_base->mAdmBufSize + stream_out->mCurBufOffset,
               ((const char*) buffer) + consumed_bytes,
               transfer_bytes);
        stream_out->mCurBufOffset += transfer_bytes;

        if (stream_out->mCurBufOffset >= fillLength) {
            int newLpaMode = 0;
            err = ste_adm_client_send(adm_base->mADMConnectionID, stream_out->mCurBufIdx, stream_out->mCurBufOffset, &newLpaMode);
            if(stream_out->mLpaMode != newLpaMode) {
                ALOG_INFO("ste_audio_stream_out_write(): LPA mode changed from %d to %d", stream_out->mLpaMode, newLpaMode);
                stream_out->mLpaMode = newLpaMode;
            }
            ALOG_INFO_FL("ste_audio_stream_out_write(): Sent %d bytes to ADM\n", stream_out->mCurBufOffset);

            stream_out->mCurBufIdx = (stream_out->mCurBufIdx + 1) % adm_base->mAdmNumBufs;
            stream_out->mCurBufOffset = 0;

            if (err == STE_ADM_RES_ERR_MSG_IO) {
                pthread_mutex_lock(&adm_base->mMutex);
                /* connection to ADM has been lost or other connection issues, possible ADM reboot
                   try to re-initiate the connection */
                /* try to close but igore error message if there are any */
                int tmp_err;
                struct node *list_elem;
                int i;
                for (i=0; i < list_count(adm_base->mDeviceList); i++) {
                    list_elem = list_get_nth(adm_base->mDeviceList, i);
                    tmp_err = ste_adm_close_device(adm_base->mADMConnectionID, list_elem->key);
                    if (tmp_err != STE_ADM_RES_OK) {
                        ALOG_WARN("ste_audio_stream_out_write(): Lost connection to ADM, trying to close device returned error=%d fd=%d\n",
                                  tmp_err, adm_base->mADMConnectionID);
                    }
                }
                /* disconnect ADM connection */
               tmp_err = ste_adm_client_disconnect(adm_base->mADMConnectionID);
               stream_out->mCurBufIdx = 0;
                stream_out->mCurBufOffset = 0;
                if (tmp_err != STE_ADM_RES_OK) {
                    ALOG_WARN("ste_audio_stream_out_write(): Lost connection to ADM, trying to disconnect returned error=%d fd=%d\n",
                              tmp_err, adm_base->mADMConnectionID);
                }
                /* open new connection */
                adm_base->mADMConnectionID = ste_adm_client_connect();
                if (adm_base->mADMConnectionID < 0) {
                    /* failed to re-connect */
                    ALOG_ERR("ste_audio_stream_out_write(): ste_adm_client_connect() failed in write");
                    pthread_mutex_unlock(&adm_base->mMutex);
                    return FAILED_TRANSACTION;
                }
                ALOG_INFO("ste_audio_stream_out_write(): Re-connected to ADM after lost connection, ID = %d", adm_base->mADMConnectionID);

                pthread_mutex_unlock(&adm_base->mMutex);

                /* Check call status and recover call graph if needed */
                ALOG_INFO_VERBOSE("ste_audio_policy_manager ptr=%x", stream_out->dev->mAudioPolicyManager);
                if( stream_out->dev->mAudioPolicyManager != 0) {
                    audio_policy_service_ops_ptr = (audio_policy_anm*)(stream_out->dev->mAudioPolicyManager);
                    audio_policy_service_ops_ptr->check_call_status(audio_policy_service_ops_ptr->apm_ptr);
                }

                pthread_mutex_lock(&adm_base->mMutex);

                /* re-open all new devices */
                adm_base->mAdmNumBufs = 3;
                if (stream_out->mNewDevices != 0) {
                    if (!stream_out->mChangingDevice) {
                        /* mNewDevices is not 0 and open_dev_thread is not running.
                           This means that changing to new devices failed in open_dev_thread */
                        ALOG_INFO("ste_audio_stream_out_write(): mNewDevices = %d", stream_out->mNewDevices);
                        stream_out->mDevices = stream_out->mNewDevices;
                        /* Refresh device list from new routing info */
                        ste_audio_stream_out_refresh_device_list(&adm_base->mDeviceList, stream_out->mNewDevices, stream_out);
                        stream_out->mNewDevices = 0;
                    }
                }

                for (i=0; i < list_count(adm_base->mDeviceList); i++) {
                    list_elem = list_get_nth(adm_base->mDeviceList, i);
                    tmp_err = ste_adm_client_open_device(adm_base->mADMConnectionID, list_elem->key,
                    ste_audio_stream_out_get_sample_rate((const struct audio_stream *)stream), admbase_get_adm_format(&stream_out->adm_base), NULL,
                        &adm_base->mAdmBufSharedMem, adm_base->mAdmBufSize, adm_base->mAdmNumBufs);
                    if (tmp_err != STE_ADM_RES_OK) {
                        ALOG_ERR("ste_audio_stream_out_write(): Re-connect to ADM, failed to open device %s, err=%d\n", list_elem->key, tmp_err);
                    }
                }

                pthread_mutex_unlock(&adm_base->mMutex);

                /* re-connected and re-opened devices to ADM */
                ALOG_INFO("ste_audio_stream_out_write(): Reconnected to ADM server, new fd=%d\n", adm_base->mADMConnectionID);
                continue;
            } else if (err != STE_ADM_RES_OK) {
                ALOG_ERR("ste_audio_stream_out_write(): Failed to send audio data to ADM: %d\n", err);
                return FAILED_TRANSACTION;
            }
        }
        consumed_bytes += transfer_bytes;
    }

    ALOG_INFO_FL("ste_audio_stream_out_write(): Completed\n");

    return bytes;
}

/*
 * Put the audio hardware output into standby mode. Returns
 * status based on include/utils/Errors.h
 */
void ste_audio_stream_out_pre_close_device(struct stream_adm_base *adm)
{
    struct audio_stream_out_anm *stream_out_anm_p = adm->stream_out_anm;

    ALOG_INFO_VERBOSE("ste_audio_stream_out_pre_close_device(): Trying to drain devices\n");
    (void) ste_adm_client_drain(stream_out_anm_p->adm_base.mADMConnectionID);
    ALOG_INFO_VERBOSE("ste_audio_stream_out_pre_close_device(): Drain devices done\n");
}

/* dump the state of the audio output device */
int ste_audio_stream_out_dump(const struct audio_stream *stream, int fd)
{
    ALOG_INFO_VERBOSE("ste_audio_stream_out_dump()\n");
    return NO_ERROR;
}

status_t ste_audio_stream_out_change_device(const struct audio_stream *stream, int mode, uint32_t routes)
{
    status_t status = OK;
    struct audio_stream_out_anm *stream_out = (struct audio_stream_out_anm *)stream;
    struct stream_adm_base      *adm_base   = &(stream_out->adm_base);
    struct node *list_elem;
    int i;

    ALOG_INFO("ste_audio_stream_out_change_device(): "
        "routes (0x%08X) mStandby (%d) (stream=%p)\n", routes, adm_base->mStandby, stream);

    pthread_mutex_lock(&adm_base->mMutex);

    if (!adm_base->mStandby) {
        if (routes != stream_out->mDevices) {
            ALOG_INFO_VERBOSE("ste_audio_stream_out_change_device(): after mutex, routes (0x%08X)", routes);

            stream_out->mChangingDevice = true;

            /* Check if there is already a new connection, which has not been closed */
            if (stream_out->mNewADMConnID > 0) {
                ALOG_INFO("ste_audio_stream_out_change_device(): Close previous unused ADM connection");
                (void) ste_adm_client_drain(stream_out->mNewADMConnID);
                struct node *deviceList = NULL;
                ste_audio_stream_out_refresh_device_list(&deviceList, stream_out->mNewDevices, stream_out);

                for (i=0; i < list_count(deviceList); i++) {
                    list_elem = list_get_nth(deviceList, i);
                    ALOG_INFO("ste_audio_stream_out_change_device(): Close device %s in ADM connection %d\n", list_elem->key, stream_out->mNewADMConnID);
                    int err = ste_adm_close_device(stream_out->mNewADMConnID, list_elem->key);
                    if (err != STE_ADM_RES_OK) {
                        ALOG_ERR("ste_audio_stream_out_change_device(): Failed to close device %s!\n", list_elem->key);
                    }
                }
                ALOG_INFO("ste_audio_stream_out_change_device(): Disconnect from ADM, ID = %d", stream_out->mNewADMConnID);
                ste_adm_client_disconnect(stream_out->mNewADMConnID);
                stream_out->mNewADMConnID = -1;
            }

            int devicesToClose = stream_out->mDevices & ~(stream_out->mDevices & routes);
            int devicesToOpen = routes & ~(stream_out->mDevices);
            /* Open A2DP devices in separate thread since it takes very long time.
               Don't open in separate thread if the previous device was BT-SCO since BT-SCO and A2DP can't be open simultaneously.
               Same solution when a headset or headphone is plugged during a playback on speaker to avoid video frames loss. */
            if (((devicesToClose == AUDIO_DEVICE_OUT_SPEAKER) && (devicesToOpen == AUDIO_DEVICE_OUT_WIRED_HEADSET)) ||
                ((devicesToClose == AUDIO_DEVICE_OUT_SPEAKER) && (devicesToOpen == AUDIO_DEVICE_OUT_WIRED_HEADPHONE)) ||
                ((routes & AUDIO_DEVICE_OUT_ALL_A2DP) &&
                !(stream_out->mDevices & AUDIO_DEVICE_OUT_ALL_A2DP) &&
                !(stream_out->mDevices & (AUDIO_DEVICE_OUT_BLUETOOTH_SCO | AUDIO_DEVICE_OUT_BLUETOOTH_SCO_HEADSET | AUDIO_DEVICE_OUT_BLUETOOTH_SCO_CARKIT)))) {
                /* Open the device to which audio should be routed */
                stream_out->mNewDevices = routes;
                pthread_t thread;
                pthread_attr_t tattr;
                pthread_attr_init(&tattr);
                pthread_attr_setdetachstate(&tattr, PTHREAD_CREATE_DETACHED);
                if (pthread_create(&thread, &tattr, ste_audio_stream_out_open_dev_thread, (void*)stream)) {
                    ALOG_ERR("ste_audio_stream_out_change_device(): pthread_create() failed\n");
                    stream_out->mChangingDevice = false;
                    pthread_mutex_unlock(&adm_base->mMutex);
                    return NO_INIT;
                }
                // keep the mutex; will be unlocked by open_dev_thread
            }
            else {
                ALOG_INFO_VERBOSE("ste_audio_stream_out_change_device(): routes = %Xh, devicesToClose = %Xh, devicesToOpen = %Xh. Closing old.", routes, devicesToClose,devicesToOpen);

                (void) ste_adm_client_drain(adm_base->mADMConnectionID);

                struct node *deviceList = NULL;
                ste_audio_stream_out_refresh_device_list(&deviceList, devicesToClose, stream_out);
                for (i=0; i < list_count(deviceList); i++) {
                    list_elem = list_get_nth(deviceList, i);
                    ALOG_INFO("ste_audio_stream_out_change_device(): Close device %s in ADM connection %d\n", list_elem->key, adm_base->mADMConnectionID);
                    int err = ste_adm_close_device(adm_base->mADMConnectionID, list_elem->key);
                    if (err != STE_ADM_RES_OK) {
                        ALOG_ERR("ste_audio_stream_out_change_device(): Failed to close device %s!\n", list_elem->key);
                    }
                }

                ALOG_INFO_VERBOSE("ste_audio_stream_out_change_device(): Old device closed, opening new device");

                int err = 0;
                int retries = 10;
                do {
                    struct node *deviceList = NULL;
                    ste_audio_stream_out_refresh_device_list(&deviceList, devicesToOpen, stream_out);
                    for (i=0; i < list_count(deviceList); i++) {
                        list_elem = list_get_nth(deviceList, i);
                        int num_bufs = 3;
                        int bufsz = adm_base->calc_buffer_size(adm_base);
                        char *bufp;

                        ALOG_INFO("ste_audio_stream_out_change_device(): Open device %s in ADM connection %d\n", list_elem->key, adm_base->mADMConnectionID);
                        err = ste_adm_client_open_device(adm_base->mADMConnectionID, list_elem->key,
                            ste_audio_stream_out_get_sample_rate(stream), admbase_get_adm_format(&stream_out->adm_base), NULL,
                            &bufp, bufsz, num_bufs);
                        if (err != STE_ADM_RES_OK) {
                            ALOG_ERR("ste_audio_stream_out_change_device(): Failed to open device %s\n", list_elem->key);
                            break;
                        }

                        assert(adm_base->mAdmBufSharedMem == NULL || adm_base->mAdmBufSharedMem == bufp);
                        assert(adm_base->mAdmBufSize      == 0    || adm_base->mAdmBufSize      == num_bufs);
                        assert(adm_base->mAdmNumBufs      == 0    || adm_base->mAdmNumBufs      == num_bufs);
                        adm_base->mAdmBufSharedMem = bufp;
                        adm_base->mAdmBufSize      = bufsz;
                        adm_base->mAdmNumBufs      = num_bufs;
                    }

                    stream_out->mDevices = routes;
                    ste_audio_stream_out_refresh_device_list(&adm_base->mDeviceList, routes, stream_out);

                    if (err != STE_ADM_RES_OK) {
                        /* Failed to open device, close the connection */
                        int tmp_err;
                        for (i=0; i < list_count(deviceList); i++) {
                            list_elem = list_get_nth(deviceList, i);
                            tmp_err = ste_adm_close_device(adm_base->mADMConnectionID, list_elem->key);
                            if (tmp_err != STE_ADM_RES_OK) {
                                ALOG_WARN("ste_audio_stream_out_change_device(): Lost connection to ADM, trying to close "
                                    "device returned error=%d\n", tmp_err);
                            }
                        }
                        ste_adm_client_disconnect(adm_base->mADMConnectionID);
                        ALOG_INFO("ste_audio_stream_out_change_device(): Connect to ADM, ID = %d", adm_base->mADMConnectionID);
                        if (adm_base->mAdmBufSharedMem != NULL) {
                            munmap(adm_base->mAdmBufSharedMem, adm_base->mAdmBufSize * adm_base->mAdmNumBufs);
                            adm_base->mAdmBufSharedMem = NULL;
                            ALOG_INFO(" 3 adm_base->mAdmBufSize = %d, set to 0", adm_base->mAdmBufSize);
                            adm_base->mAdmBufSize      = 0;
                            adm_base->mAdmNumBufs      = 0;
                        }

                        /* Open new connection */
                        adm_base->mADMConnectionID = ste_adm_client_connect();
                    }
                    retries--;
                } while (err != STE_ADM_RES_OK && retries > 0);

                if (err == STE_ADM_RES_OK) {
                    ALOG_INFO_VERBOSE("ste_audio_stream_out_change_device(): Opened new device");
                }
                else {
                    ALOG_ERR("ste_audio_stream_out_change_device(): Failed to open devices");
                    status = UNKNOWN_ERROR;
                }

                stream_out->mChangingDevice = false;
            }

        }
    } else {
        /* Refresh device list from new routing info */
        ste_audio_stream_out_refresh_device_list(&adm_base->mDeviceList, routes, stream_out);
        stream_out->mDevices = routes;
    }
    pthread_mutex_unlock(&adm_base->mMutex);

    return status;
}

status_t ste_audio_stream_out_setup(const struct audio_stream *stream, struct audio_config *config)
{
    status_t status = OK;
   struct audio_stream_out_anm *stream_out = (struct audio_stream_out_anm *)stream;

    ALOG_INFO_VERBOSE("ste_audio_stream_out_setup(): samplerate %d, channels %x (%d), "
        "format %d\n", config->sample_rate, config->channel_mask, popcount(config->channel_mask),
        config->format);

    /* First of all, connect to ADM */
    if (admbase_setup_adm_connection(&stream_out->adm_base) < 0) {
        ALOG_ERR("ste_audio_stream_out_setup():Failed to setup ADM Connection\n");
        return NO_INIT;
    }

    /* Check input values */
    if (config->format == 0) {
        config->format = DEFAULT_PCM_FORMAT;
        ALOG_INFO_VERBOSE("ste_audio_stream_out_setup(): Format is 0 - update to %x", config->format);
    }
    if (config->channel_mask == 0) {
        config->channel_mask = DEFAULT_CHANNELS;
        ALOG_INFO_VERBOSE("ste_audio_stream_out_setup(): Channels is 0 - update to %x (%d)",
            config->channel_mask, popcount(config->channel_mask));
    }
    if (config->sample_rate == 0) {
        config->sample_rate = DEFAULT_SAMPLE_RATE;
        ALOG_INFO_VERBOSE("ste_audio_stream_out_setup(): Sample rate is 0 - update to %d", config->sample_rate);
    }

    stream_out->adm_base.mFormat = config->format;
    stream_out->adm_base.mChannels = config->channel_mask;
    stream_out->adm_base.mSampleRate = config->sample_rate;

    return status;
}

/* get_render_position */
status_t ste_audio_stream_out_get_render_position(const struct audio_stream_out *stream, uint32_t *dsp_frames)
{
  /**@todo: implement */
  *dsp_frames=0;
  return NO_ERROR;
}

/**
 * get the local time at which the next write to the audio driver will be presented.
 * The units are microseconds, where the epoch is decided by the local audio HAL.
 */
int ste_audio_stream_out_get_next_write_timestamp(const struct audio_stream_out *stream, int64_t *timestamp)
{
    return NO_ERROR;
}

void ste_audio_stream_out_post_open_device(struct stream_adm_base *adm)
{
    ALOG_INFO_VERBOSE("ste_audio_stream_out_post_open_device(): do nothing\n");
}

int ste_audio_stream_out_standby(struct audio_stream *stream)
{
    status_t status;
    struct audio_stream_out_anm *stream_out = (struct audio_stream_out_anm *)stream;
    struct stream_adm_base *adm_base = &stream_out->adm_base;

    pthread_mutex_lock(&adm_base->mMutex);
    status = ste_audio_stream_out_standby_l(stream);
    pthread_mutex_unlock(&adm_base->mMutex);
    return status;
}

/* Mutex mMutex MUST be held when calling this function */
status_t ste_audio_stream_out_standby_l(struct audio_stream *stream)
{
    status_t status;
    struct audio_stream_out_anm *stream_out = (struct audio_stream_out_anm *)stream;
    struct stream_adm_base *adm_base = &stream_out->adm_base;

    ALOG_INFO("ENTER ste_audio_stream_out_standby_l() (stream=%p)\n", stream);

    status = admbase_standby_imp(adm_base);
    if (status != OK) {
        ALOG_ERR("ste_audio_stream_out_standby_l(): Failed to go to standby!\n");
        return status;
    }

    if (stream_out->mNewADMConnID > 0) {

        /* Switch to new connection */
        stream_out->mOldADMConnID = adm_base->mADMConnectionID;
        adm_base->mADMConnectionID = stream_out->mNewADMConnID;
        stream_out->mNewADMConnID = -1;
        ALOG_INFO("ste_audio_stream_out_standby_l(): Disconnect from ADM, ID = %d", stream_out->mOldADMConnID);
        ste_adm_client_disconnect(stream_out->mOldADMConnID);
        stream_out->mCurBufIdx = 0;
        stream_out->mOldADMConnID = -1;

        /* Refresh device list from new routing info */
        ste_audio_stream_out_refresh_device_list(&adm_base->mDeviceList, stream_out->mNewDevices, stream_out);
        ALOG_INFO("ste_audio_stream_out_standby_l(): new devices = %X", (int) stream_out->mNewDevices);
        stream_out->mDevices =stream_out->mNewDevices;
        stream_out->mNewDevices = 0;
        adm_base->mStandby = 0;

        status = admbase_standby_imp(adm_base);

        if (status != OK) {
            ALOG_ERR("ste_audio_stream_out_standby_l(): Failed to go to standby!\n");
            return status;
        }
    }
    ALOG_INFO("LEAVE ste_audio_stream_out_standby_l()\n");
    return status;
}

void* ste_audio_stream_out_report_latency_thread(void *parms)
{
    audio_io_handle_t output;
    struct kvpair_stream_ptr *ptr = (struct kvpair_stream_ptr *)parms;
    char* kv_pairs = ptr->ptr1;
    struct str_parms *latencyParam;
    char *str = NULL;
    struct audio_stream_out_anm *stream_out = (struct audio_stream_out_anm *)ptr->ptr2;

    ALOG_INFO("ste_audio_stream_out_report_latency_thread()");

    if(ptr) {
        /* Free the memory allocated for parms*/
        anm_free(ptr);
    }
    latencyParam = str_parms_create_str(kv_pairs);
    if(str_parms_get_int(latencyParam, "output", &output) <= 0) {
        str_parms_destroy(latencyParam);
        return NULL;
    }
    str_parms_del(latencyParam, "output");
    str = str_parms_to_str(latencyParam);

    ALOG_INFO_VERBOSE("ste_audio_policy_manager ptr=%x", stream_out->dev->mAudioPolicyManager);
    ((audio_policy_anm*)(stream_out->dev->mAudioPolicyManager))->mp_client_interface->set_parameters(((audio_policy_anm*)(stream_out->dev->mAudioPolicyManager))->mService, output, str, 0);

    str_parms_destroy(latencyParam);

    return NULL;
}

void ste_audio_stream_out_notify_sink_latency(const struct audio_stream *stream, audio_io_handle_t output)
{
    struct audio_stream_out_anm *stream_out = (struct audio_stream_out_anm *)stream;
    struct stream_adm_base        *adm_base = &stream_out->adm_base;
    pthread_t thread;
    pthread_attr_t tattr;
    struct str_parms *latencyParam;
    char value[32], *str = NULL;
    int ret;

    ALOG_INFO("ENTER ste_audio_stream_out_notify_sink_latency()");
    struct kvpair_stream_ptr *ptr = anm_calloc(1, sizeof(struct kvpair_stream_ptr));
    pthread_mutex_lock(&adm_base->mMutex);

    /* Get current devices */
    struct node* deviceList = NULL;
    if (stream_out->mNewDevices == 0) {
        deviceList = stream_out->adm_base.mDeviceList;
    } else {
        /* Routing change ongoing, use the new device list */
        ste_audio_stream_out_refresh_device_list(&deviceList, stream_out->mNewDevices, stream_out);
    }

    /* Get latency for any A2DP device */
    uint32_t sinkLatency = ste_audio_stream_out_get_latency((const struct audio_stream_out *)stream_out);
    struct node *list_elem;
    int i;
    for (i=0; i < list_count(deviceList); i++) {
        list_elem = list_get_nth(deviceList, i);
        if (strcmp("STE_ADM_DEVICE_STRING_A2DP", list_elem->key) == 0) {
            ALOG_INFO("Retrieving sink latency for device %s\n", list_elem->key);

            uint32_t latency;
            int res = ste_adm_client_get_sink_latency(list_elem->key, &latency);
            if (res < 0) {
                ALOG_WARN("Failed to get sink latency for device %s\n", list_elem->key);
                pthread_mutex_unlock(&adm_base->mMutex);
                return;
            }
            sinkLatency += latency;
            break;
        }
    }
    pthread_mutex_unlock(&adm_base->mMutex);

    /* Report sink latency */
    pthread_attr_init(&tattr);
    pthread_attr_setdetachstate(&tattr, PTHREAD_CREATE_DETACHED);

    latencyParam = str_parms_create_str("output");
    ret = str_parms_add_int(latencyParam, "output", output);
    str = str_parms_to_str(latencyParam);
    str_parms_dump(latencyParam);

    str_parms_destroy(latencyParam);
    ptr->ptr1 = str;
    ptr->ptr2 = stream_out;
    if (pthread_create(&thread, &tattr, ste_audio_stream_out_report_latency_thread, (void*)ptr)) {
        ALOG_ERR("pthread_create() failed, no latency will be reported\n");
    }
    ALOG_INFO_VERBOSE("LEAVE ste_audio_stream_out_notify_sink_latency():");
}

/*********************** Implementation of File scope functions ****************
*
*
*******************************************************************************/
void ste_audio_stream_out_refresh_device_list(struct node **device_list, uint32_t routes, struct audio_stream_out_anm* stream_out)
{
    struct node *temp, *device_list_to_del;
    char value[40], key[40], *kvpairs;
    struct str_parms *parms;
    int ret;
    bool isBTWB;

    ALOG_INFO("ste_audio_stream_out_refresh_device_list routes = %d\n", routes);

    temp = *device_list;
    device_list_to_del = *device_list;
    while (device_list_to_del != NULL) {
        temp = device_list_to_del;
        device_list_to_del = device_list_to_del->next;
        anm_free(temp);
    };
    *device_list = NULL;

    if (routes & AUDIO_DEVICE_OUT_EARPIECE) {
        list_add(device_list, STE_ADM_DEVICE_STRING_EARP, 0);
    }

    if (routes & AUDIO_DEVICE_OUT_SPEAKER) {
        list_add(device_list, STE_ADM_DEVICE_STRING_SPEAKER, 0);
    }

    if ((routes & AUDIO_DEVICE_OUT_BLUETOOTH_SCO) ||
        (routes & AUDIO_DEVICE_OUT_BLUETOOTH_SCO_HEADSET) ||
        (routes & AUDIO_DEVICE_OUT_BLUETOOTH_SCO_CARKIT)) {
        isBTWB = false; /* Bluetooth Wideband (16kHz) */
        strcpy(key, PARAM_KEY_BTWB);

        ALOG_INFO_VERBOSE("ste_audio_policy_manager ptr=%x", stream_out->dev->mAudioPolicyManager);
        kvpairs = ((audio_policy_anm*)(stream_out->dev->mAudioPolicyManager))->mp_client_interface->get_parameters(((audio_policy_anm*)(stream_out->dev->mAudioPolicyManager))->mService,0, key);
        parms = str_parms_create_str(kvpairs);
        if (str_parms_get_str(parms, PARAM_KEY_BTWB, value, sizeof(value))) {
            isBTWB = ((!strcmp(value, "on")) ? 1:0);
        }
        if (isBTWB) {
            list_add(device_list, STE_ADM_DEVICE_STRING_BTWBOUT, 0);
        } else {
            list_add(device_list, STE_ADM_DEVICE_STRING_BTOUT, 0);
        }
    }

    if ((routes & AUDIO_DEVICE_OUT_WIRED_HEADSET) ||
        (routes & AUDIO_DEVICE_OUT_WIRED_HEADPHONE)) {
        list_add(device_list, STE_ADM_DEVICE_STRING_HSOUT, 0);
    }

    if ((routes & AUDIO_DEVICE_OUT_BLUETOOTH_A2DP) ||
        (routes & AUDIO_DEVICE_OUT_BLUETOOTH_A2DP_HEADPHONES) ||
        (routes & AUDIO_DEVICE_OUT_BLUETOOTH_A2DP_SPEAKER)) {
        list_add(device_list, STE_ADM_DEVICE_STRING_A2DP, 0);
    }

    if (routes & AUDIO_DEVICE_OUT_FM_TX) {
        list_add(device_list, STE_ADM_DEVICE_STRING_FMTX, 0);
    }

    if (routes & AUDIO_DEVICE_OUT_AUX_DIGITAL) {
        list_add(device_list, STE_ADM_DEVICE_STRING_HDMI, 0);
    }
    if ((routes & AUDIO_DEVICE_OUT_DGTL_DOCK_HEADSET)
#ifdef STE_USB_MNGT
        ||(routes & AUDIO_DEVICE_OUT_USB_DEVICE)
        ||(routes & AUDIO_DEVICE_OUT_USB_ACCESSORY)
#endif
         ){
        list_add(device_list, STE_ADM_DEVICE_STRING_USBOUT, 0);
    }
    if (routes & AUDIO_DEVICE_OUT_DEFAULT) {
        list_add(device_list, STE_ADM_DEVICE_STRING_VOIPOUT, 0);
    }

}

int ste_audio_stream_out_set_sample_rate(struct audio_stream *stream, uint32_t rate)
{
    ALOG_INFO_VERBOSE("ste_audio_stream_out_set_sample_rate()");
    return 0;
}

int ste_audio_stream_out_set_format(struct audio_stream *stream, audio_format_t format)
{
    ALOG_INFO_VERBOSE("ste_audio_stream_out_set_format()");
    return 0;
}

int ste_audio_stream_out_add_audio_effect(const struct audio_stream *stream, effect_handle_t effect)
{
    ALOG_INFO_VERBOSE("ste_audio_stream_out_add_audio_effect()");
    return 0;
}

int ste_audio_stream_out_remove_audio_effect(const struct audio_stream *stream, effect_handle_t effect)
{
    ALOG_INFO_VERBOSE("ste_audio_stream_out_remove_audio_effect()");
    return 0;
}

int ste_audio_stream_out_init(struct audio_stream_out_anm *stream_out)
{

    int status;
    status = admbase_init(&stream_out->adm_base);
    if (status != 0) {
        ALOG_ERR("ste_audio_stream_out_init(): admbase_init failed, error = %d\n", status);
    }

    stream_out->dump = ste_audio_stream_out_dump;
    stream_out->set_parameters = ste_audio_stream_out_set_parameters;
    stream_out->get_parameters = ste_audio_stream_out_get_parameters;

    stream_out->mNewDevices = 0;
    stream_out->mChangingDevice = false;
    stream_out->mNewADMConnID = 0;
    stream_out->mNewAdmNumBufs = 0;
    stream_out->mNewAdmBufSize = 0;
    stream_out->mNewAdmBufSharedMem = 0;
    stream_out->mOldDevices = 0;
    stream_out->mOldADMConnID = 0;
    stream_out->mCurBufIdx = 0;
    stream_out->mDevices = 0;
    stream_out->mLatency = 0;
    stream_out->mLatencyReportOutput = 0;
    stream_out->mLpaMode = 0;
    stream_out->mCurBufOffset = 0;

    /* ADMBase Functions */
    stream_out->adm_base.stream_in_anm = NULL;
    stream_out->adm_base.stream_out_anm = stream_out;
    stream_out->adm_base.post_open_device = ste_audio_stream_out_post_open_device;
    stream_out->adm_base.pre_close_device = ste_audio_stream_out_pre_close_device;
    stream_out->adm_base.calc_buffer_size = ste_audio_stream_out_calc_buffer_size;

    ALOG_INFO("ste_audio_stream_out_init(): stream_out=%p\n", stream_out);
    int retval = pthread_mutex_init(&stream_out->adm_base.mMutex, 0);
    if (retval != 0) {
        pthread_mutex_destroy(&stream_out->adm_base.mMutex);
        ALOG_ERR("ste_audio_stream_out_init(): pthread_mutex_init failed, error = %d\n", retval);
    }
    return retval;
}

void ste_audio_stream_out_deinit(struct audio_stream_out *stream)
{
    status_t status;
    ALOG_INFO("ste_audio_stream_out_deinit(): stream_out=%p\n", stream);

    /* Go to standby (drain and close devices) */
    struct audio_stream_out_anm *stream_out = (struct audio_stream_out_anm *)stream;
    status = ste_audio_stream_out_standby((struct audio_stream *)stream);

    if (status != NO_ERROR) {
        ALOG_ERR("ste_audio_stream_out_deinit(): Failed to go to standby mode, continue anyway\n");
    }

    if (admbase_close_adm_connection(&stream_out->adm_base)) {
        ALOG_ERR("ste_audio_stream_out_deinit(): Failed to close ADM connection!\n");
    }

    admbase_deinit((struct stream_adm_base*)(&stream_out->adm_base));

    pthread_mutex_destroy(&stream_out->adm_base.mMutex);
}
