/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*! \file ste_anm_ahiinlib.cc
*   \brief ANM implementation of AHI Input interface.

    Implements the Input specific parts of AHI defined in
    Android and used by the AudioFlinger. The input specific parts consists of
    the methods defined in audio_stream_in.
*/

#define ANM_LOG_FILENAME "anm_ahi_input"
#include "ste_hal_anm_dbg.h"
#include "ste_hal_anm_ahi.h"

#include <audio_effects/effect_aec.h>
#include <unistd.h>
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
#define INPUT_BUFFER_DURATION (60)
#endif

/**
* The default PCM format returned to interface client.
*/
#ifndef DEFAULT_PCM_FORMAT
#define DEFAULT_PCM_FORMAT AUDIO_FORMAT_PCM_16_BIT
#endif

/**
* The default channel configuration returned to interface client.
*/
#ifndef DEFAULT_CHANNELS
#define DEFAULT_CHANNELS AUDIO_CHANNEL_IN_MONO
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


void ste_audio_stream_in_dump_preprocessing_info(struct audio_stream_in_anm *stream)
{
    struct audio_stream_in_anm *stream_in = (struct audio_stream_in_anm *)stream;
    unsigned int index=0;
    effect_descriptor_t desc;
    ALOG_INFO_VERBOSE("ste_audio_stream_in_dump_preprocessing_info(): %d registered Effects",stream_in->mPreProcInfo.num_preprocessors);
    for(index=0;index<MAX_PREPROCESSORS;index++) {
        if(stream_in->mPreProcInfo.mPreProcEffects[index]) {
            (*stream_in->mPreProcInfo.mPreProcEffects[index])->get_descriptor(
                stream_in->mPreProcInfo.mPreProcEffects[index],
                &desc);
            ALOG_INFO_VERBOSE(" Effect [#%d] : %s",index,desc.name);
        }
    }
    ALOG_INFO_VERBOSE(" Frame size in bytes : %d",stream_in->mPreProcInfo.FrameSizeInBytes);
    ALOG_INFO_VERBOSE(" Number of frames to process 10ms : %d",stream_in->mPreProcInfo.NbFramesPerProcess);
 };

status_t ste_audio_stream_in_setup(const struct audio_stream *stream, struct audio_config *config)
{
    int err;
    struct audio_stream_in_anm *stream_in = (struct audio_stream_in_anm *)stream;
    struct stream_adm_base      *adm_base = &stream_in->adm_base;

    ALOG_INFO_VERBOSE("ste_audio_stream_in_setup(): format %d, channels %x (%d), "
        "samplerate %d\n", config->format, config->channel_mask, popcount(config->channel_mask),
        config->sample_rate);

    /* First of all, connect to ADM */
    if (admbase_setup_adm_connection(adm_base) < 0) {
        ALOG_ERR("ste_audio_stream_in_setup(): Failed to setup ADM Connection\n");
        return NO_INIT;
    }

    /* Check input values */
    if (config->format == 0) {
        config->format = DEFAULT_PCM_FORMAT;
        ALOG_INFO_VERBOSE("ste_audio_stream_in_setup(): Format is 0 - update to %x", config->format);
    }
    if (config->channel_mask == 0) {
        config->channel_mask = DEFAULT_CHANNELS;
        ALOG_INFO_VERBOSE("ste_audio_stream_in_setup(): Channels is 0 - update to %x (%d)",
            config->channel_mask, popcount(config->channel_mask));
    }
    if (config->sample_rate == 0) {
        config->sample_rate = DEFAULT_SAMPLE_RATE;
        ALOG_INFO_VERBOSE("ste_audio_stream_in_setup(): Sample rate is 0 - update to %d", config->sample_rate);
    }

    adm_base->mFormat = config->format;
    adm_base->mChannels = config->channel_mask;
    adm_base->mSampleRate = config->sample_rate;

     return NO_ERROR;
}

void ste_audio_stream_in_set_mute(const struct audio_stream *stream, bool state)
{
    ALOG_INFO_VERBOSE("ste_audio_stream_in_set_mute(): %s", state ? "TRUE" : "FALSE");
    struct audio_stream_in_anm *stream_in = (struct audio_stream_in_anm *)stream;
    struct stream_adm_base       *adm_base  = &stream_in->adm_base;
    struct node *list_elem;
    int i;

    for (i=0; i < list_count(adm_base->mDeviceList); i++) {
        list_elem = list_get_nth(adm_base->mDeviceList, i);

        //In case of VoIP, we don't need to set the app volume since it
        //  is handled by the voice call graph.
        if (!strcmp(list_elem->key, STE_ADM_DEVICE_STRING_VOIPIN)) {
            ALOG_INFO_VERBOSE("ste_audio_stream_in_set_mute(): Device is %s, no need to "
                "modify app volume.", list_elem->key);
            continue;
        }

        if (state) {
            if (ste_adm_client_get_app_volume(list_elem->key, &stream_in->mMuteAppVol)){
                ALOG_INFO_VERBOSE("ste_audio_stream_in_set_mute(): ste_adm_client_get_app_volume() "
                    "failed probably because %s is not opened yet\n", list_elem->key);
            }
            if(ste_adm_client_set_app_volume(list_elem->key, INT_MIN)) {
                ALOG_INFO_VERBOSE("ste_audio_stream_in_set_mute(): ste_adm_client_set_app_volume() "
                    "failed probably because %s is not opened yet\n", list_elem->key);
            }
        } else {
            stream_in->mMuteAppVol = ste_audio_stream_in_get_default_mic_vol(stream_in);
            if (ste_adm_client_set_app_volume(list_elem->key, stream_in->mMuteAppVol)) {
                ALOG_INFO_VERBOSE("ste_audio_stream_in_set_mute(): ste_adm_client_set_app_volume() "
                    "failed probably because %s is not opened yet\n", list_elem->key);
            }
        }
    }

    stream_in->mMuted = state;
}

int ste_audio_stream_in_get_default_mic_vol(struct audio_stream_in_anm *stream_in)
{
    ALOG_INFO_VERBOSE("ste_audio_stream_in_get_default_mic_vol()");
    return stream_in->mMuteAppVol;
}

/********************* Implementation of audio_stream_in ************************
* We implement the methods in the order they are listed in
* audio_stream_in.
*
* BEGIN
*******************************************************************************/
/* Part of audio_stream_in interface */
status_t ste_audio_stream_in_set_parameters(struct audio_stream *stream, const char *kv_pairs)
{
    struct audio_stream_in_anm  *stream_in = (struct audio_stream_in_anm *)stream;
    struct str_parms *parms;
    int value_int;
    int ret;
    status_t status = NO_ERROR;
    int devices = 0;

    parms = str_parms_create_str(kv_pairs);
    str_parms_dump(parms);
    ALOG_INFO("ste_audio_stream_in_set_parameters(): %s (stream=%p)", kv_pairs, stream);

    /* Check for requested routing changes */
    ret = str_parms_get_int(parms, AUDIO_PARAMETER_STREAM_ROUTING, &devices);

    if( ret >= 0 ) {
        char buf[400];
        ALOG_INFO("ste_audio_stream_in_set_parameters(): %s %08X %s\n", AUDIO_PARAMETER_STREAM_ROUTING,
            devices, admbase_devices_2_str(&stream_in->adm_base, buf, (audio_devices_t) devices));
        status = ste_audio_stream_in_change_device(stream, 0, devices);
    } else {
        ALOG_INFO("ste_audio_stream_in_set_parameters(): Unhandled key, forward to admbase_set_parameters\n");
        str_parms_dump(parms);
        status = admbase_set_parameters(&stream_in->adm_base, kv_pairs);
    }
    str_parms_destroy(parms);

    return status;
}

char* ste_audio_stream_in_get_parameters(const struct audio_stream *stream,
                                const char *keys)
{
    struct audio_stream_in_anm *stream_in = (struct audio_stream_in_anm *)stream;
    struct stream_adm_base       *adm_base  = &stream_in->adm_base;
    return admbase_get_parameters(adm_base, keys);
}

uint32_t ste_audio_stream_in_get_sample_rate(const struct audio_stream *stream)
{
    struct audio_stream_in_anm *stream_in = (struct audio_stream_in_anm *)stream;
    struct stream_adm_base       *adm_base  = &stream_in->adm_base;
    ALOG_INFO_VERBOSE("ste_audio_stream_in_get_sample_rate(): %d Hz\n", stream_in->adm_base.mSampleRate);
    return stream_in->adm_base.mSampleRate;
}

size_t ste_audio_stream_in_calc_buffer_size(struct stream_adm_base *adm_base)
{
    size_t granularity;
    size_t size = DEFAULT_BUFFER_SIZE;
    size_t channels = popcount(adm_base->mChannels);
    ALOG_INFO_VERBOSE("ste_audio_stream_in_calc_buffer_size():adm_base->mSampleRate= %d Hz, channels=%d\n", adm_base->mSampleRate, channels);
    if (adm_base->mSampleRate != 0 && channels != 0) {
        size = adm_base->mSampleRate * INPUT_BUFFER_DURATION / 1000 * channels * 2;
        granularity = BUFFER_GRANULARITY_IN_SAMPLES * 2 * channels;
        size = (size / granularity) * granularity;
    }
    ALOG_INFO_VERBOSE("ste_audio_stream_in_calc_buffer_size():size %d \n", size);
    return size;
}

/* Return the input buffer size allowed by input device */
size_t ste_audio_stream_in_get_buffer_size(const struct audio_stream *stream)
{
    struct audio_stream_in_anm *stream_in = (struct audio_stream_in_anm *)stream;
    struct stream_adm_base       *adm_base  = &stream_in->adm_base;
    size_t bufsz = ste_audio_stream_in_calc_buffer_size(adm_base);
    ALOG_INFO_VERBOSE("ste_audio_stream_in_get_buffer_size(): %d bytes\n", bufsz);
    return bufsz;
}

/* Return the number of audio input channels */
audio_channel_mask_t ste_audio_stream_in_get_channels(const struct audio_stream *stream)
{
    struct audio_stream_in_anm *stream_in = (struct audio_stream_in_anm *)stream;
    struct stream_adm_base       *adm_base  = &stream_in->adm_base;

    ALOG_INFO_VERBOSE("ste_audio_stream_in_get_channels(): %d\n", adm_base->mChannels);
    return (audio_channel_mask_t) adm_base->mChannels;
}

audio_format_t  ste_audio_stream_in_get_format(const struct audio_stream *stream)
{
    struct audio_stream_in_anm *stream_in = (struct audio_stream_in_anm *)stream;
    struct stream_adm_base       *adm_base  = &stream_in->adm_base;

    ALOG_INFO_VERBOSE("ste_audio_stream_in_get_format(): %d\n", adm_base->mFormat);
    return (audio_format_t)adm_base->mFormat;
}

/* Set the input gain for the audio driver. This method is for for future use */
int ste_audio_stream_in_set_gain(struct audio_stream_in *stream, float gain)
{
    ALOG_INFO_VERBOSE("ste_audio_stream_in_set_gain()\n");
    return OK;
}

ssize_t ste_audio_stream_in_preprocessing(struct audio_stream_in *stream, void* buffer, size_t bytes,char* inputBuffer)
{
    struct audio_stream_in_anm *stream_in = (struct audio_stream_in_anm *)stream;
    struct stream_adm_base     *adm_base  = &stream_in->adm_base;

    unsigned int index;
    size_t total_frames_read=0;
    size_t remaining_frames_to_process=0;
    size_t frames_requested=0;
    unsigned int IntermediateBufferSize=0;
    audio_buffer_t in_buf;
    audio_buffer_t out_buf;
    char * inputBufferPtr=NULL;
    char * outputBufferPtr = NULL;
    int ret=0;

    ste_audio_stream_in_dump_preprocessing_info(stream_in);
    if(    (stream_in->mPreProcInfo.num_preprocessors == 0)  ||
            (inputBuffer == NULL ) ) {
        return -1;
    }

    if(stream_in->mPreProcInfo.data_beforePreProc) {
        fwrite(inputBuffer,bytes,1,stream_in->mPreProcInfo.data_beforePreProc);
    }

    for (index = 0; index < stream_in->mPreProcInfo.num_preprocessors; index++) {
        ALOG_INFO_FL("Process Effect [%d] index",index);
        // number of frames coming from ADM in one read
        total_frames_read= bytes/stream_in->mPreProcInfo.FrameSizeInBytes;
        //number of frames remaining to process
        remaining_frames_to_process = total_frames_read;
        //By default, input buffer is the one filled by ADM, and output buffer is the one provided by client
        inputBufferPtr = inputBuffer;
        outputBufferPtr = buffer;
        //Size in bytes of the 10ms buffer
        IntermediateBufferSize = stream_in->mPreProcInfo.FrameSizeInBytes*stream_in->mPreProcInfo.NbFramesPerProcess;
        // Number of frames to fill 10ms buffer
        frames_requested = stream_in->mPreProcInfo.NbFramesPerProcess;

        while(remaining_frames_to_process>0) {
            //Indicate number of frames available in input buffer
            in_buf.frameCount=frames_requested;
            //Indicate number of frames waited in output buffer
            out_buf.frameCount=frames_requested;
            in_buf.raw = (void*)inputBufferPtr;
            out_buf.raw = (void*)outputBufferPtr;
	    if(stream_in->mPreProcInfo.mPreProcEffects[index] != NULL) {
            ret=(*stream_in->mPreProcInfo.mPreProcEffects[index])->process(
                    stream_in->mPreProcInfo.mPreProcEffects[index],
                    &in_buf,
                    &out_buf);
            } else {
                  ALOG_WARN("PreProcessing  no according effect processor!");
                  return -1;
            }
            if(ret) {
                ALOG_WARN("PreProcessing return %d status => abort preprocessing",ret);
                return (-1);
            }
            if(out_buf.frameCount != frames_requested) {
                ALOG_WARN("PreProcessing didn't update every frames (remains %d frames when %d frames were requested) => abort preprocessing",frames_requested-out_buf.frameCount,frames_requested);
                return (-1);
            }
            outputBufferPtr+=IntermediateBufferSize;
            inputBufferPtr+=IntermediateBufferSize;
            remaining_frames_to_process-=frames_requested;
            if(remaining_frames_to_process < frames_requested) {
                frames_requested=remaining_frames_to_process;
                IntermediateBufferSize = frames_requested*stream_in->mPreProcInfo.FrameSizeInBytes;
            }
        }
        if((index+1) < stream_in->mPreProcInfo.num_preprocessors) {
            //if several consecutive effects have to be executed, use previous output buffer as next input buffer
            ALOG_INFO_FL("Exchange buffers",index);
            memcpy(inputBuffer,buffer,bytes);
        }
    }
    if(stream_in->mPreProcInfo.data_afterPreProc) {
        fwrite(buffer,bytes,1,stream_in->mPreProcInfo.data_afterPreProc);
    }
    return(bytes);
}

/* Read audio buffer from input device */
ssize_t ste_audio_stream_in_read(struct audio_stream_in *stream, void* buffer, size_t bytes)
{
    struct audio_stream_in_anm *stream_in = (struct audio_stream_in_anm *)stream;
    struct stream_adm_base     *adm_base  = &stream_in->adm_base;
    int err = 0;
    int retry_read = 1;
    audio_policy_anm* audio_policy_service_ops_ptr = NULL;

    ALOG_INFO_FL("ste_audio_stream_in_read(): %d bytes to buffer 0x%08X (this=%p)\n",
        (int) bytes, (uint32_t) buffer, stream);

    if (adm_base->mStandby) {
        ALOG_INFO("ste_audio_stream_in_read(): Open device since in standby.\n");

        pthread_mutex_lock(&adm_base->mMutex);
        status_t status = admbase_open_devices(adm_base);
        pthread_mutex_unlock(&adm_base->mMutex);

        if (status != OK) {
            ALOG_ERR("ste_audio_stream_in_read(): Failed to open input device!\n");
            return JPARKS_BROKE_IT;
        }
        adm_base->mStandby = false;
    }

    if (bytes == 0 || (ssize_t)bytes > adm_base->mAdmBufSize) {
        ALOG_ERR("ste_audio_stream_in_read(): Requested to read invalid amount of bytes! bytes=%d adm_base->mAdmBufSize=%d\n",bytes,adm_base->mAdmBufSize);
        return BAD_VALUE;
    }

    if (adm_base->mStandbyPending) {
        ALOG_INFO("ste_audio_stream_in_read(): adm_base->mStandbyPending TRUE, resetting\n");
        adm_base->mStandbyPending = false;
    }

    while (retry_read) {
        if (adm_base->mAdmBufSharedMem == NULL) {
            ALOG_ERR("ste_audio_stream_in_read(): output buffer not yet allocated\n");
            return INVALID_OPERATION;
        }

        int bufIdx;
        err = ste_adm_client_receive(adm_base->mADMConnectionID, &bufIdx);

        if (err == STE_ADM_RES_ERR_MSG_IO) {
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
                    ALOG_WARN("ste_audio_stream_in_read(): Lost connection to ADM, trying to close device returned error=%d\n",
                            tmp_err);
                }
            }
            /* disconnect ADM connection */
            tmp_err = ste_adm_client_disconnect(adm_base->mADMConnectionID);
            if (tmp_err != STE_ADM_RES_OK) {
                ALOG_WARN("ste_audio_stream_in_read(): Lost connection to ADM, trying to disconnect returned error=%d\n",
                        tmp_err);
            }
            /* open new connection */
            adm_base->mADMConnectionID = ste_adm_client_connect();
            if (adm_base->mADMConnectionID < 0) {
                /* failed to re-connect */
                return FAILED_TRANSACTION;
            }

            ALOG_INFO("ste_audio_stream_in_read(): Re-connected to ADM after lost connection, ID = %d", adm_base->mADMConnectionID);
            /* Check call status and recover call graph if needed */
            ALOG_INFO_VERBOSE("ste_audio_policy_manager ptr=%x", stream_in->dev->mAudioPolicyManager);
            if (stream_in->dev->mAudioPolicyManager != 0) {
                audio_policy_service_ops_ptr = (audio_policy_anm*)((audio_policy_anm*)(stream_in->dev->mAudioPolicyManager));
                audio_policy_service_ops_ptr->check_call_status(audio_policy_service_ops_ptr->apm_ptr);
            }

            /* re-open all new devices */
            adm_base->mAdmNumBufs = 3;
            for (i=0; i < list_count(adm_base->mDeviceList); i++) {
                list_elem = list_get_nth(adm_base->mDeviceList, i);
                tmp_err = ste_adm_client_open_device(adm_base->mADMConnectionID, list_elem->key,
                    ste_audio_stream_in_get_sample_rate((const struct audio_stream *)stream), admbase_get_adm_format(adm_base), NULL,
                    &adm_base->mAdmBufSharedMem, adm_base->mAdmBufSize, adm_base->mAdmNumBufs);
                if (tmp_err != STE_ADM_RES_OK) {
                    ALOG_ERR("ste_audio_stream_in_read(): Re-connect to ADM, failed to open device %s\n", list_elem->key);
                }
            }
            ALOG_INFO("ste_audio_stream_in_read(): Reconnected to ADM server\n");
            continue;
        } else if (err == STE_ADM_RES_DEVICE_RECONFIGURING) {
            ALOG_ERR("Failed to receive audio data from ADM (currently " \
                  "reconfiguring device). Waiting some time before trying again.");
            usleep(500000);
            continue;
        } else if (err != STE_ADM_RES_OK) {
            ALOG_ERR("ste_audio_stream_in_read(): Failed to receive audio data from ADM: %d\n", err);
            return FAILED_TRANSACTION;
        } else {
            char* inputBuffer=adm_base->mAdmBufSharedMem + adm_base->mAdmBufSize*bufIdx;
            if(ste_audio_stream_in_preprocessing(stream,buffer,bytes,inputBuffer)<0) {
                memcpy(buffer,inputBuffer, bytes);
            }
        }
        retry_read = 0;
    }

    ALOG_INFO_FL("ste_audio_stream_in_read(): Completed\n");
    return bytes;
}

/* Dump the state of the audio input device */
int ste_audio_stream_in_dump(const struct audio_stream *stream, int fd)
{
    ALOG_INFO_VERBOSE("ste_audio_stream_in_dump()\n");
    return OK;
}

void ste_audio_stream_in_post_open_device(struct stream_adm_base *adm)
{
    struct audio_stream_in_anm *stream_in = adm->stream_in_anm;

    ALOG_INFO_VERBOSE("ste_audio_stream_in_post_open_device(): updating mute status\n");

    if (stream_in->mMuted){
        ALOG_INFO_VERBOSE("ste_audio_stream_in_post_open_device(): Microphone muted");
        ste_audio_stream_in_set_mute((const struct audio_stream*)stream_in, stream_in->mMuted);
    }
}

void ste_audio_stream_in_pre_close_device(struct stream_adm_base *adm)
{
    ALOG_INFO_VERBOSE("ste_audio_stream_in_pre_close_device(): does nothing\n");
}

uint32_t ste_audio_stream_in_get_input_frames_lost(struct audio_stream_in *stream)
{
  /**@todo: implement */
  return 0;
}

int ste_audio_stream_in_add_audio_effect(const struct audio_stream *stream, effect_handle_t effect)
{
    struct audio_stream_in_anm *stream_in = (struct audio_stream_in_anm *)stream;
    struct stream_adm_base     *adm_base  = &stream_in->adm_base;
    effect_descriptor_t desc;
    int status= NO_ERROR;

    ALOG_INFO("Add audio effect [EffectHandle : 0x%x]",effect);

    if (stream_in == NULL ||adm_base==NULL) {
        ALOG_WARN("Bad parameters");
        return BAD_VALUE;
    }

    pthread_mutex_lock(&adm_base->mMutex);
    if(stream_in->mPreProcInfo.num_preprocessors >= MAX_PREPROCESSORS) {
        ALOG_WARN("No more resources");
        status = PERMISSION_DENIED;
        goto exit;
    }

    status = (*effect)->get_descriptor(effect, &desc);
    if (status == 0) {
        if (memcmp(&desc.type, FX_IID_AEC, sizeof(effect_uuid_t)) == 0) {
            ALOG_WARN("external AEC isn't supported ; usage of internal AEC instead");
            status = PERMISSION_DENIED;
            goto exit;
        }
        //Prepare preprocessing information
        if(adm_base->mFormat!=AUDIO_FORMAT_PCM_16_BIT) {
            ALOG_WARN("PreProcessing effect won't be applied: only 16bits format is supported");
            goto exit;
        }
        switch(adm_base->mChannels) {
            case AUDIO_CHANNEL_IN_MONO :
                stream_in->mPreProcInfo.FrameSizeInBytes=2;
                break;
            case AUDIO_CHANNEL_IN_STEREO :
                stream_in->mPreProcInfo.FrameSizeInBytes = 4;
                break;
            default:
                ALOG_WARN("PreProcessing effect won't be applied: only mono or stereo samples are supported");
                goto exit;
        }
        //Preprocessing algorithms only  accept 10ms buffer so compute the number of frames according to this.
        switch(adm_base->mSampleRate) {
            case 8000 : stream_in->mPreProcInfo.NbFramesPerProcess =  80; break;
            case 16000 : stream_in->mPreProcInfo.NbFramesPerProcess = 160; break;
            case 32000 : stream_in->mPreProcInfo.NbFramesPerProcess = 320; break;
            default:
                ALOG_WARN("PreProcessing effects won't be applied: Sample rate %d not supported",adm_base->mSampleRate);
                goto exit;
        }
        ALOG_INFO("Add %s effect",desc.name);
        stream_in->mPreProcInfo.mPreProcEffects[stream_in->mPreProcInfo.num_preprocessors]=effect;
        stream_in->mPreProcInfo.num_preprocessors++;
    }
    ste_audio_stream_in_dump_preprocessing_info(stream_in);
exit:
    pthread_mutex_unlock(&adm_base->mMutex);
    ALOG_INFO_VERBOSE("Add effect return %d",status);
    return status;
}

int ste_audio_stream_in_remove_audio_effect(const struct audio_stream *stream, effect_handle_t effect)
{
    struct audio_stream_in_anm *stream_in = (struct audio_stream_in_anm *)stream;
    struct stream_adm_base     *adm_base  = &stream_in->adm_base;
    effect_descriptor_t desc;
    int status = NO_ERROR;
    unsigned int  i;
    ALOG_INFO("Remove audio effect [EffectHandle : 0x%x]",effect);
    if (stream_in == NULL ||adm_base==NULL) {
        ALOG_WARN("Bad parameters");
        return BAD_VALUE;
    }

    pthread_mutex_lock(&adm_base->mMutex);
    if(stream_in->mPreProcInfo.num_preprocessors < 1 ) {
        goto exit;
    }

    for (i = 0; i < MAX_PREPROCESSORS; i++) {
        if (stream_in->mPreProcInfo.mPreProcEffects[i] == effect) {
            (*effect)->get_descriptor(effect, &desc);
            ALOG_INFO("Remove %s effect",desc.name);
            stream_in->mPreProcInfo.mPreProcEffects[i]=NULL;
            stream_in->mPreProcInfo.num_preprocessors--;
        }
    }
exit:
    ste_audio_stream_in_dump_preprocessing_info(stream_in);
    pthread_mutex_unlock(&adm_base->mMutex);
    ALOG_INFO_VERBOSE("Remove effect return %d",status);
    return status;
}

status_t ste_audio_stream_in_change_device(const struct audio_stream *stream, int mode, uint32_t routes)
{
    status_t status = OK;
    struct audio_stream_in_anm *stream_in = (struct audio_stream_in_anm *)stream;
    struct stream_adm_base       *adm_base  = &stream_in->adm_base;

    ALOG_INFO("ste_audio_stream_in_change_device(): Got change device request, routes (0x%08X) (this=%p)\n",
          routes, stream);

    pthread_mutex_lock(&adm_base->mMutex);
    if (!adm_base->mStandby) {
        /* First go to standby mode, drain pcm and close devices */
        adm_base->mStarted = false;
        status = ste_audio_stream_in_standby_l((struct audio_stream*)stream_in);

        if (status != OK) {
            pthread_mutex_unlock(&adm_base->mMutex);
            return status;
        }

        /* Refresh device list from new routing info */
        ste_audio_stream_in_refresh_device_list(&adm_base->mDeviceList, routes, stream_in);

        /* Open all the devices again, this will leave standby mode */
        status = admbase_open_devices(adm_base);
    }
    else
    {
        /* Refresh device list from new routing info */
        ste_audio_stream_in_refresh_device_list(&adm_base->mDeviceList, routes, stream_in);
    }

    pthread_mutex_unlock(&adm_base->mMutex);

    return status;
}


int ste_audio_stream_in_standby(struct audio_stream *stream)
{
    status_t status;
    struct audio_stream_in_anm *stream_in = (struct audio_stream_in_anm *)stream;
    struct stream_adm_base      *adm_base = &stream_in->adm_base;

    pthread_mutex_lock(&adm_base->mMutex);
    status = ste_audio_stream_in_standby_l(stream);
    pthread_mutex_unlock(&adm_base->mMutex);

    return status;
}

/* Mutex mMutex MUST be held when calling this function */
status_t ste_audio_stream_in_standby_l(struct audio_stream *stream)
{
    status_t status;
    struct audio_stream_in_anm *stream_in = (struct audio_stream_in_anm *)stream;
    struct stream_adm_base      *adm_base = &stream_in->adm_base;
    ALOG_INFO("ENTER ste_audio_stream_in_standby_l() (this=%p) \n", stream);
    status = admbase_standby_imp(adm_base);
    ALOG_INFO("LEAVE ste_audio_stream_in_standby_l()\n");
    return status;
}

/*********************** Implementation of File scope functions ****************
*
*
*******************************************************************************/
void ste_audio_stream_in_refresh_device_list(struct node **device_list, uint32_t routes, struct audio_stream_in_anm* stream_in)
{
    struct node *temp, *device_list_to_del;
    struct str_parms *parms;
    char value[32], *kvpairs;

    ALOG_INFO_VERBOSE("ste_audio_stream_in_refresh_device_list routes = %d\n", routes);
    temp = *device_list;
    device_list_to_del = *device_list;
    while(device_list_to_del != NULL) {
        temp = device_list_to_del;
        device_list_to_del = device_list_to_del->next;
        anm_free(temp);
    };

    *device_list = NULL;

    if (routes & AUDIO_DEVICE_IN_BUILTIN_MIC) {
        list_add(device_list, STE_ADM_DEVICE_STRING_MIC, 0);
    }

    if (routes & AUDIO_DEVICE_IN_WIRED_HEADSET) {
        list_add(device_list, STE_ADM_DEVICE_STRING_HSIN, 0);
    }

    if (routes & AUDIO_DEVICE_IN_VOICE_CALL) {
        list_add(device_list, STE_ADM_DEVICE_STRING_DICT_REC, 0);
    }

    if (routes & AUDIO_DEVICE_IN_BLUETOOTH_SCO_HEADSET) {
        bool isBTWB = false; /* Bluetooth Wideband (16kHz) */
        ALOG_INFO_VERBOSE("ste_audio_policy_manager ptr=%x", stream_in->dev->mAudioPolicyManager);

        kvpairs = ((audio_policy_anm*)(stream_in->dev->mAudioPolicyManager))->mp_client_interface->get_parameters(((audio_policy_anm*)(stream_in->dev->mAudioPolicyManager))->mService, 0, PARAM_KEY_BTWB);

        parms = str_parms_create_str(kvpairs);
        if (str_parms_get_str(parms, PARAM_KEY_BTWB, value, sizeof(value))) {
            isBTWB = ((!strcmp(value, "on")) ? 1:0);
        }
        if (isBTWB) {
            list_add(device_list, STE_ADM_DEVICE_STRING_BTWBIN, 0);
        } else {
            list_add(device_list, STE_ADM_DEVICE_STRING_BTIN, 0);
        }
    }

    if (routes & AUDIO_DEVICE_IN_FM_RX) {
        list_add(device_list, STE_ADM_DEVICE_STRING_FMRX, 0);
    }

    if (routes & AUDIO_DEVICE_IN_DEFAULT) {
        list_add(device_list, STE_ADM_DEVICE_STRING_VOIPIN, 0);
    }
}

int ste_audio_stream_in_set_sample_rate(struct audio_stream *stream, uint32_t rate)
{
    ALOG_INFO_VERBOSE("ENTER ste_audio_stream_in_set_sample_rate()");
    return 0;
}

int ste_audio_stream_in_set_format(struct audio_stream *stream, audio_format_t format)
{
    ALOG_INFO_VERBOSE("ENTER ste_audio_stream_in_set_format()");
    return 0;
}

audio_devices_t ste_audio_stream_in_get_device(const struct audio_stream *stream)
{
    ALOG_INFO_VERBOSE("ste_audio_stream_in_get_device()");
    return 0;
}

int ste_audio_stream_in_set_device(struct audio_stream *stream, audio_devices_t device)
{
    ALOG_INFO_VERBOSE("ste_audio_stream_in_set_device()");
    return 0;
}

int ste_audio_stream_in_init(struct audio_stream_in_anm *stream_in)
{
    int status;
    status = admbase_init(&stream_in->adm_base);
    if (status != 0) {
        ALOG_ERR("ste_audio_stream_in_init(): pthread_mutex_init failed, error = %d\n", status);
        return status;
    }

    stream_in->dump            = ste_audio_stream_in_dump;
    stream_in->set_parameters  = ste_audio_stream_in_set_parameters;
    stream_in->get_parameters  = ste_audio_stream_in_get_parameters;
    stream_in->mMuted        = false;
    stream_in->mMuteAppVol   = 0;

    /* stream_adm_base */
    stream_in->adm_base.stream_in_anm    = stream_in;
    stream_in->adm_base.stream_out_anm   = NULL;
    stream_in->adm_base.post_open_device = ste_audio_stream_in_post_open_device;
    stream_in->adm_base.pre_close_device = ste_audio_stream_in_pre_close_device;
    stream_in->adm_base.calc_buffer_size = ste_audio_stream_in_calc_buffer_size;

    memset(&stream_in->mPreProcInfo,0,sizeof(struct ste_preprocessing_info));

#ifdef DUMP_PREPROCESSING_DATA
    if(stream_in->mPreProcInfo.data_beforePreProc!=NULL) {
        fclose(stream_in->mPreProcInfo.data_beforePreProc);
        stream_in->mPreProcInfo.data_beforePreProc=NULL;
    }
    if(stream_in->mPreProcInfo.data_beforePreProc==NULL) {
        char filename[1024] = "/data/data_ori.pcm";
        stream_in->mPreProcInfo.data_beforePreProc=fopen(filename,"ab");
    }
    if(stream_in->mPreProcInfo.data_afterPreProc!=NULL) {
        fclose(stream_in->mPreProcInfo.data_afterPreProc);
        stream_in->mPreProcInfo.data_afterPreProc=NULL;
    }
    if(stream_in->mPreProcInfo.data_afterPreProc==NULL) {
        char filename[1024] = "/data/data_proc.pcm";
        stream_in->mPreProcInfo.data_afterPreProc=fopen(filename,"ab");
    }
#endif
    ALOG_INFO("ste_audio_stream_in_init(): stream_in=%p\n", stream_in);
    return 0;
}

void ste_audio_stream_in_deinit(struct audio_stream_in *stream)
{
    struct audio_stream_in_anm *stream_in = (struct audio_stream_in_anm *)stream;
    ALOG_INFO("ste_audio_stream_in_deinit(): stream_in=%p", stream_in);
#ifdef DUMP_PREPROCESSING_DATA
    if(stream_in->mPreProcInfo.data_beforePreProc!=NULL) {
        fclose(stream_in->mPreProcInfo.data_beforePreProc);
        stream_in->mPreProcInfo.data_beforePreProc=NULL;
    }
    if(stream_in->mPreProcInfo.data_afterPreProc!=NULL) {
        fclose(stream_in->mPreProcInfo.data_afterPreProc);
        stream_in->mPreProcInfo.data_afterPreProc=NULL;
    }
#endif
    admbase_deinit((struct stream_adm_base*)(&stream_in->adm_base));
}
