/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*! \file ste_anm_ahi.c
*   \brief ANM implementation of AHI

    Implementation of the AHI
    defined in Android and used by the AudioFlinger.
*/

#define ANM_LOG_FILENAME "anm_ahi"
#include "ste_hal_anm_dbg.h"

#include <assert.h>
#include <cutils/str_parms.h>

#include "ste_hal_anm_ahi.h"
#include "ste_hal_anm_util.h"
#include <ste_adm_client.h>

/* How to translate the [0,1] volume range given from Android to millibells.
 * The translation is linear and the max volume is 0 dB. This value shoudn't
 * be too low (even though the mixer can handle -100 dB) otherwise the lower
 * part of the volume 'knobs' will be experienced as pointless (no sound).
 */
#ifndef  ANM_VOICECALL_VOLUME_FLOOR
 #define ANM_VOICECALL_VOLUME_FLOOR (-2500)
#endif

struct node *ahi_params=NULL;
struct node *ap_params=NULL;

#ifdef CHECK_MEMORY
void* CALLOC(const char *file, int line, const char*func, int num, int size)
{
    void *ptr = calloc(num,size);
    ALOG_INFO("CALLOC(): Allocated memory %x, num=%d, size=%d from %s %d %s", ptr, num, size, file, line, func);
    return ptr;
}

void FREE(const char *file, int line, const char*func, void *ptr)
{
    ALOG_INFO("FREE(): Free memory %x from %s %d %s", ptr, file, line, func);
    if(ptr) {
        free(ptr);
    }
}
#endif

static int convert_to_adm_volume(float volume, int floor)
{
    // 0.0 --> ANM_VOLUME_FLOOR
    // 1.0 --> 0
    // linear
    return (int) ((1-volume)*floor);
}

/**
* Implementation of create method defined in AHI, just returns
* a pointer to our implementation of AHI.
*/
struct ste_audio_hw_device* create_ste_audio_hardware(void)
{
    struct ste_audio_hw_device *ahi;
    ste_anm_debug_setup_log();
    ahi = anm_calloc(1, sizeof(struct ste_audio_hw_device));
    ALOG_INFO("create_ste_audio_hardware(): Creating Audio Hardware Interface (%x)\n", ahi);
    /* Initialise the variabls of ste_audio_hw_device */
    ste_audio_hardware_init(ahi);

    return ahi;
}

void ste_adev_release(hw_device_t* device)
{
    struct ste_audio_hw_device *ahi = (struct ste_audio_hw_device *)device;
    int num_inputs, num_outputs, i;
    ALOG_INFO("ste_adev_release(): device=%x\n", device);

    num_outputs = ahi_list_count(ahi->mOutputs);
    num_inputs  = ahi_list_count(ahi->mInputs);

    for (i = 0; i < num_outputs; i++) {
        ahi_list_del(&ahi->mOutputs, ahi_list_get_nth_node(ahi->mOutputs, 0));
    }
    ahi->mOutputs = NULL;

    for (i = 0; i < num_inputs; i++) {
        ahi_list_del(&ahi->mInputs, ahi_list_get_nth_node(ahi->mInputs, 0));
    }
    ahi->mInputs = NULL;
}

/* Check if already initialized */
int ste_adev_init_check(const struct audio_hw_device *dev)
{
    struct  ste_audio_hw_device *ahi = (struct ste_audio_hw_device *) dev;

    if (ahi_list_count(ahi->mOutputs) > 0 || ahi_list_count(ahi->mInputs) > 0) {
        ALOG_INFO_VERBOSE("ste_adev_init_check(): Already initialized!\n");
        return ALREADY_EXISTS;
    }

    ALOG_INFO_VERBOSE("ste_adev_init_check(): OK\n");
    return NO_ERROR;
}

int ste_adev_set_voice_volume(struct audio_hw_device *dev, float volume)
{
    struct ste_audio_hw_device *ahi = (struct ste_audio_hw_device *) dev;
    int adm_volume;

    if ((ahi->mModemType == ADM_FAT_MODEM) || (ahi->mModemType == ADM_FULL_FAT_MODEM)) {
        /* volume=8.0 is max, volume=0.0 is min. ADM wants input in index. */
        if (volume < 0.0) {
            ALOG_ERR("setVoiceVolume(%f): Bad value\n",volume);
            return BAD_VALUE;
        }

        adm_volume = (int)volume;

        ALOG_INFO_VERBOSE("setVoiceVolume(%d)\n",
            adm_volume);
    } else {
        /* volume=1.0 is max, volume=0.0 is min. ADM wants input in mB. */
        if (volume < 0.0 || volume > 1.0) {
            ALOG_ERR("setVoiceVolume(%f): Bad value, range is [0.0,1.0]\n",volume);
            return BAD_VALUE;
        }

        adm_volume = convert_to_adm_volume(volume, ANM_VOICECALL_VOLUME_FLOOR);

        ALOG_INFO_VERBOSE("setVoiceVolume(%f): %d mB\n",
            volume, adm_volume);
    }

    if (ste_adm_client_set_cscall_downstream_volume(adm_volume)) {
        ALOG_ERR("ste_adev_set_voice_volume(): "
            "ste_adm_client_set_cscall_downstream_volume failed\n");
        return UNKNOWN_ERROR;
    }

    return NO_ERROR;
}

/* @todo: Set master volume for all other cases than voice call */
int ste_adev_set_master_volume(struct audio_hw_device *dev, float volume)
{
    ALOG_INFO_VERBOSE("ste_adev_set_master_volume(%f)\n", volume);
    return NO_ERROR;
}

int ste_adev_get_master_volume(struct audio_hw_device *dev, float *volume)
{
    /* todo */
    return -ENOSYS;
}

int ste_adev_set_mic_mute(struct audio_hw_device *dev, bool state)
{
    struct ste_audio_hw_device *ahi = (struct ste_audio_hw_device *) dev;
    int i;
    struct audio_stream_in_anm *stream_in;

    ALOG_INFO_VERBOSE("ste_adev_set_mic_mute(%s)\n", state ? "TRUE" : "FALSE");

    if (state) {
        if(ste_adm_client_get_cscall_upstream_volume(&ahi->mMuteCsVol)){
            ALOG_ERR("ste_adev_set_mic_mute(): "
                "ste_adm_client_get_cscall_upstream_volume() failed\n");
            return UNKNOWN_ERROR;
        }
        if(ste_adm_client_set_cscall_upstream_volume(INT_MIN)) {
            ALOG_ERR("ste_adev_set_mic_mute(): "
                "ste_adm_client_set_cscall_upstream_volume() failed\n");
            return UNKNOWN_ERROR;
        }
    } else {
        if (ste_adm_client_set_cscall_upstream_volume(ahi->mMuteCsVol)) {
            ALOG_ERR("ste_adev_set_mic_mute(): "
                "ste_adm_client_set_cscall_upstream_volume() failed\n");
            return UNKNOWN_ERROR;
        }
    }

    /* Set mic mute state for all opened inputs */
    for (i = 0; i < ahi_list_count(ahi->mInputs); i++) {
        stream_in = (struct audio_stream_in_anm *)ahi_list_get_nth_node(ahi->mInputs, i);
        ste_audio_stream_in_set_mute((const struct audio_stream *)stream_in, state);
    }

    ahi->mMuted = state;
    return NO_ERROR;
}

int ste_adev_get_mic_mute(const struct audio_hw_device *dev, bool *state)
{
    struct ste_audio_hw_device *ahi = (struct ste_audio_hw_device *) dev;
    *state = ahi->mMuted;
    ALOG_INFO_VERBOSE("ste_adev_get_mic_mute(): state = %s\n", ahi->mMuted ? "TRUE" : "FALSE");
    return NO_ERROR;
}

int ste_adev_set_parameters(struct audio_hw_device *dev, const char *kv_pairs)
{
    struct  ste_audio_hw_device *ahi = (struct ste_audio_hw_device *) dev;
    struct str_parms *parms;
    char value[32];
    int value_int;
    int ret;
    ste_adm_tty_mode_t tty_mode=STE_ADM_TTY_MODE_OFF;
    char tty_mode_string[4][30]={"STE_ADM_TTY_MODE_OFF","STE_ADM_TTY_MODE_FULL","STE_ADM_TTY_MODE_HCO","STE_ADM_TTY_MODE_VCO"};

    ALOG_INFO("ENTER ste_adev_set_parameters() %s", kv_pairs);

    parms = str_parms_create_str(kv_pairs);

    ret = str_parms_get_str(parms, PARAM_KEY_BTWB, value, sizeof(value));
     if (ret >= 0) {
         ahi->mBtWb = (strcmp(value, "on") == 0) ? 1:0;
    }

    ret = str_parms_get_str(parms, PARAM_KEY_BTNREC, value, sizeof(value));
    if (ret >= 0) {
         ahi->mBtNrEc = (strcmp(value, "on") == 0) ? 1:0;
    }

    ret = str_parms_get_int(parms, PARAM_KEY_AP_REF, &value_int);
    if (ret >= 0) {
        ahi->mAudioPolicyManager = value_int;
    }

    if (((ahi->mModemType == ADM_FAT_MODEM) ||(ahi->mModemType == ADM_FULL_FAT_MODEM)) && (str_parms_get_str(parms, PARAM_KEY_TTY_MODE, value, sizeof(value)))) {
         if(strcmp(value, "tty_full") == 0) {
            tty_mode = STE_ADM_TTY_MODE_FULL;
        }
        else if(strcmp(value, "tty_vco") == 0) {
            tty_mode = STE_ADM_TTY_MODE_VCO;
        }
        else if(strcmp(value, "tty_hco") == 0) {
            tty_mode = STE_ADM_TTY_MODE_HCO;
        }
        else {
            tty_mode = STE_ADM_TTY_MODE_OFF;
        }

        if (ste_adm_client_set_cscall_tty_mode(tty_mode)) {
            ALOG_ERR("ste_adev_set_parameters(): ste_adm_client_set_cscall_tty_mode failed\n");
            str_parms_destroy(parms);
            return UNKNOWN_ERROR;
        }

        ALOG_INFO("ste_adev_set_parameters(): ste_adm_client_set_cscall_tty_mode(%s) SUCCESS\n", tty_mode_string[tty_mode]);
    }
    ALOG_INFO("LEAVE ste_adev_set_parameters");

    str_parms_destroy(parms);
    return NO_ERROR;
}

char* ste_adev_get_parameters(const struct audio_hw_device *dev, const char *keys)
{
    struct  ste_audio_hw_device *ahi = (struct ste_audio_hw_device *) dev;
    struct str_parms *parms;
    struct str_parms *ahi_parms;
    char value[32], *value_str = NULL;
    int ret;

    ALOG_INFO_VERBOSE("ENTER ste_adev_get_parameters(): ");

    parms = str_parms_create_str(keys);
    ahi_parms = str_parms_create();
    ret = str_parms_get_str(parms, PARAM_KEY_BTWB, value, sizeof(value));
    if (ret >= 0) {
        if(ahi->mBtWb)
            strcpy(value, "on");
        else
            strcpy(value, "off");
        ret = str_parms_add_str(ahi_parms, PARAM_KEY_BTWB, value);
    }

    ret = str_parms_get_str(parms, PARAM_KEY_BTNREC, value, sizeof(value));
    if (ret >= 0) {
        if(ahi->mBtNrEc)
            strcpy(value, "on");
        else
            strcpy(value, "off");
        ret = str_parms_add_str(ahi_parms, PARAM_KEY_BTNREC, value);
    }

    ret = str_parms_get_str(parms, PARAM_KEY_EC_SUPPORTED, value, sizeof(value));
    if (ret >= 0) {
        ret = str_parms_add_str(ahi_parms, PARAM_KEY_EC_SUPPORTED, "true");
    }

    ret = str_parms_get_str(parms, PARAM_KEY_AP_REF, value, sizeof(value));
    if (ret >= 0) {
        ret = str_parms_add_int(ahi_parms, PARAM_KEY_AP_REF, ahi->mAudioPolicyManager);
    }

    value_str = str_parms_to_str(ahi_parms);
    str_parms_dump(ahi_parms);

    str_parms_destroy(ahi_parms);
    str_parms_destroy(parms);
    ALOG_INFO_VERBOSE("LEAVE ste_adev_get_parameters(): %s", value_str);
    return value_str;
}

/** This method creates and opens the audio hardware output stream */
int ste_adev_open_output_stream(struct audio_hw_device *dev,
                          audio_io_handle_t handle,
                          audio_devices_t devices,
                          audio_output_flags_t flags,
                          struct audio_config *config,
                          struct audio_stream_out **stream_out)
{
    int i=0;
    status_t    status;
    struct ste_audio_hw_device *ahi = (struct ste_audio_hw_device *) dev;

    ste_anm_debug_setup_log();

    ALOG_INFO("ENTER ste_adev_open_output_stream(): format %d, channels 0x%08X (%d), "
        "sample_rate %d, devices=%X\n", config->format, config->channel_mask, popcount(config->channel_mask),
        config->sample_rate, devices);

    struct audio_stream_out_anm *output = (struct audio_stream_out_anm *)anm_calloc(1, sizeof(struct audio_stream_out_anm));
    if (output == NULL) {
        ALOG_ERR("ste_adev_open_output_stream(): Failed to allocate audio_stream_out!\n");
        status = NO_MEMORY;
        return status;
    }

    output->stream_out.common.get_sample_rate     = ste_audio_stream_out_get_sample_rate;
    output->stream_out.common.set_sample_rate     = ste_audio_stream_out_set_sample_rate;
    output->stream_out.common.get_buffer_size     = ste_audio_stream_out_get_buffer_size;
    output->stream_out.common.get_channels        = ste_audio_stream_out_get_channels;
    output->stream_out.common.get_format          = ste_audio_stream_out_get_format;
    output->stream_out.common.set_format          = ste_audio_stream_out_set_format;
    output->stream_out.common.standby             = ste_audio_stream_out_standby;
    output->stream_out.common.dump                = ste_audio_stream_out_dump;
    output->stream_out.common.set_parameters      = ste_audio_stream_out_set_parameters;
    output->stream_out.common.get_parameters      = ste_audio_stream_out_get_parameters;
    output->stream_out.common.add_audio_effect    = ste_audio_stream_out_add_audio_effect;
    output->stream_out.common.remove_audio_effect = ste_audio_stream_out_remove_audio_effect;
    output->stream_out.get_latency                = ste_audio_stream_out_get_latency;
    output->stream_out.set_volume                 = ste_audio_stream_out_set_volume;
    output->stream_out.write                      = ste_audio_stream_out_write;
    output->stream_out.get_render_position        = ste_audio_stream_out_get_render_position;
    output->stream_out.get_next_write_timestamp   = NULL;

    output->dev = ahi;

    status = ste_audio_stream_out_init(output);
    if (status != OK) {
        ALOG_ERR("ste_adev_open_output_stream(): Failed to initialise objects!\n");
        anm_free(output);
        output = NULL;
        goto leave_output;
    }

    status = ste_audio_stream_out_setup((const struct audio_stream *)output, config);
    if (status != OK) {
        ALOG_ERR("ste_adev_open_output_stream(): Failed to store audio settings!\n");
        anm_free(output);
        output = NULL;
    } else {
        status = ste_audio_stream_out_change_device((const struct audio_stream *)output, output->mMode, devices);
        if (status == OK) {
            /* Add to the list of opened outputs */
            ahi_list_add(&ahi->mOutputs, output, 0);
            ALOG_INFO_VERBOSE("ste_adev_open_output_stream(): Successfully set up output stream\n");
        } else {
            ALOG_ERR("ste_adev_open_output_stream(): Failed to setup output stream!\n");
            anm_free(output);
            output = NULL;
        }
    }

leave_output:
    ALOG_INFO("LEAVE ste_adev_open_output_stream(): format %d, channels 0x%08X (%d), "
        "sample_rate %d out %d \n", config->format, config->channel_mask, popcount(config->channel_mask),
        config->sample_rate, output);

    *stream_out = (struct audio_stream_out*)output;
    return OK;
}


void ste_adev_close_output_stream(struct audio_hw_device *dev,
                                struct audio_stream_out* out)
{
    ALOG_INFO("ste_adev_close_output_stream(): output %x", (unsigned int)out);
    struct audio_stream_out_anm *output = (struct audio_stream_out_anm *) out;
    struct ste_audio_hw_device *ahi = (struct ste_audio_hw_device *) dev;
    ste_audio_stream_out_deinit(out);
    ahi_list_del(&ahi->mOutputs, output);

    if(output) {
        anm_free(output);
    }
}

/** This method creates and opens the audio hardware input stream */
int ste_adev_open_input_stream(struct audio_hw_device *dev,
                               audio_io_handle_t handle,
                               audio_devices_t devices,
                               struct audio_config *config,
                               struct audio_stream_in **stream_in)
{
    status_t status;
    int i = 0;
    struct ste_audio_hw_device *ahi = (struct ste_audio_hw_device *) dev;

    ste_anm_debug_setup_log();

    ALOG_INFO("ENTER ste_adev_open_input_stream(): "
        "format %d, channels 0x%08X (%d), sample_rate %d\n",
        config->format, config->channel_mask, popcount(config->channel_mask), config->sample_rate);

    struct audio_stream_in_anm *input = anm_calloc(1, sizeof(struct audio_stream_in_anm));
    if (input == NULL) {
        ALOG_ERR("ste_adev_open_input_stream(): Failed to allocate audio_stream_in_anm!\n");
        status = NO_MEMORY;
        return NO_INIT;
    }

    input->stream_in.common.get_sample_rate     = ste_audio_stream_in_get_sample_rate;
    input->stream_in.common.set_sample_rate     = ste_audio_stream_in_set_sample_rate;
    input->stream_in.common.get_buffer_size     = ste_audio_stream_in_get_buffer_size;
    input->stream_in.common.get_channels        = ste_audio_stream_in_get_channels;
    input->stream_in.common.get_format          = ste_audio_stream_in_get_format;
    input->stream_in.common.set_format          = ste_audio_stream_in_set_format;
    input->stream_in.common.standby             = ste_audio_stream_in_standby;
    input->stream_in.common.dump                = ste_audio_stream_in_dump;
    input->stream_in.common.set_parameters      = ste_audio_stream_in_set_parameters;
    input->stream_in.common.get_parameters      = ste_audio_stream_in_get_parameters;
    input->stream_in.common.add_audio_effect    = ste_audio_stream_in_add_audio_effect;
    input->stream_in.common.remove_audio_effect = ste_audio_stream_in_remove_audio_effect;
    input->stream_in.set_gain                   = ste_audio_stream_in_set_gain;
    input->stream_in.read                       = ste_audio_stream_in_read;
    input->stream_in.get_input_frames_lost      = ste_audio_stream_in_get_input_frames_lost;

    input->dev = ahi;

    /* Init audio_stream_in_anm */
    status = ste_audio_stream_in_init(input);
    if (status != OK) {
        ALOG_ERR("ste_adev_open_input_stream(): Failed to initialise objects!\n");
        anm_free(input);
        input = NULL;
        goto leave_input;
    }

    status = ste_audio_stream_in_setup((const struct audio_stream *)input, config);
    ste_audio_stream_in_set_mute((const struct audio_stream *)input, input->mMuted);

    if (status != OK) {
        ALOG_ERR("ste_adev_open_input_stream(): Failed to store audio settings!\n");
        anm_free(input);
        input = NULL;
    } else {
        status = ste_audio_stream_in_change_device((const struct audio_stream *)input, input->mMode, devices);
        if (status == OK) {
            /* Add to the list of opened inputs */
            ahi_list_add(&ahi->mInputs, input, 0);
            ALOG_INFO_VERBOSE("ste_adev_open_input_stream(): Successfully set up input stream\n");
        } else {
            ALOG_ERR("ste_adev_open_input_stream(): Failed to setup input stream!\n");
            anm_free(input);
            input = NULL;
        }
    }

leave_input:
    ALOG_INFO("LEAVE ste_adev_open_input_stream(): "
        "format %d, channels 0x%08X (%d), sample_rate %d\n",
        config->format, config->channel_mask, popcount(config->channel_mask), config->sample_rate);

    *stream_in = (struct audio_stream_in *)input;
    return OK;
}

void ste_adev_close_input_stream(struct audio_hw_device *dev,
                           struct audio_stream_in *in)
{
    struct ste_audio_hw_device *ahi = (struct ste_audio_hw_device *) dev;
    ALOG_INFO("ste_adev_close_input_stream(): input %x", (unsigned int)in);

    ste_audio_stream_in_deinit(in);
    ahi_list_del(&ahi->mInputs, (struct audio_stream *)in);
    if(in) {
        anm_free(in);
    }
    else {
        ALOG_INFO("ste_adev_close_input_stream(): not free input %x", (unsigned int)in);
    }
}

int ste_adev_dump(const struct audio_hw_device *dev, int fd)
{
    ALOG_INFO_VERBOSE("ste_adev_dump()");
    return NO_ERROR;
}

size_t ste_adev_get_input_buffer_size(const struct audio_hw_device *dev, const struct audio_config *config)
{
    size_t size          = 0;
    uint32_t sample_rate = config->sample_rate;
    int format           = config->format;
    int channel_count    = popcount(config->channel_mask);


    if (format != AUDIO_FORMAT_PCM_16_BIT) {
        ALOG_WARN("ste_adev_get_input_buffer_size(): bad format: %d", format);
        return 0;
    }
    if (channel_count != 1 && channel_count != 2) {
        ALOG_WARN("ste_adev_get_input_buffer_size(): bad channel count: %d", channel_count);
        return 0;
    }

    size = sample_rate * channel_count * 2 * 20 / 1000;
    ALOG_INFO_VERBOSE("ste_adev_get_input_buffer_size(): sample_rate %d, format %x, channels %d - "
        "returned buffer size = %d", sample_rate, format, channel_count, size);
    return size;
}


void ste_audio_hardware_init(struct  ste_audio_hw_device *ahi)
{
    ALOG_INFO("ste_audio_hardware_init(): Created\n");

    ahi->set_parameters        = ste_adev_set_parameters;
    ahi->get_parameters        = ste_adev_get_parameters;
    ahi->dump                  = ste_adev_dump;
    ahi->active_input =NULL;
    ahi->active_output=NULL;
    ahi->mInputs     = NULL;
    ahi->mOutputs    = NULL;
    ahi->mDLHandle   = NULL;
    ahi->mMuted      = false;
    ahi->mMuteCsVol  = 0;
    ahi->mBtWb       = false;
    ahi->mBtNrEc     = false;
    ahi->mAudioPolicyManager = 0;
    ahi->mModemType = ADM_UNKNOWN_MODEM_TYPE;

    if (ste_adm_client_get_modem_type(&ahi->mModemType) != STE_ADM_RES_OK) {
        ALOG_ERR("audio_hardware_anm(): Impossible to get modem type!\n");
    }
}

static uint32_t ste_adev_get_supported_devices(const struct audio_hw_device *dev)
{
    ALOG_INFO_VERBOSE("ste_adev_get_supported_devices(): adev = %x", dev);
    return (/* OUT */
            AUDIO_DEVICE_OUT_EARPIECE |
            AUDIO_DEVICE_OUT_SPEAKER |
            AUDIO_DEVICE_OUT_WIRED_HEADSET |
            AUDIO_DEVICE_OUT_WIRED_HEADPHONE |
            AUDIO_DEVICE_OUT_BLUETOOTH_SCO |
            AUDIO_DEVICE_OUT_BLUETOOTH_SCO_HEADSET |
            AUDIO_DEVICE_OUT_BLUETOOTH_SCO_CARKIT |
#ifndef STD_A2DP_MNGT
            AUDIO_DEVICE_OUT_BLUETOOTH_A2DP |
            AUDIO_DEVICE_OUT_BLUETOOTH_A2DP_HEADPHONES |
            AUDIO_DEVICE_OUT_BLUETOOTH_A2DP_SPEAKER |
#endif
#ifdef STE_USB_MNGT
            AUDIO_DEVICE_OUT_ALL_USB |
#endif
            AUDIO_DEVICE_OUT_AUX_DIGITAL |
            AUDIO_DEVICE_OUT_FM_TX |
            AUDIO_DEVICE_OUT_ANLG_DOCK_HEADSET |
            AUDIO_DEVICE_OUT_DGTL_DOCK_HEADSET |
            AUDIO_DEVICE_OUT_ALL_SCO |
            AUDIO_DEVICE_OUT_DEFAULT |
            /* IN */
            AUDIO_DEVICE_IN_COMMUNICATION |
            AUDIO_DEVICE_IN_AMBIENT |
            AUDIO_DEVICE_IN_BUILTIN_MIC |
            AUDIO_DEVICE_IN_BLUETOOTH_SCO_HEADSET |
            AUDIO_DEVICE_IN_WIRED_HEADSET |
            AUDIO_DEVICE_IN_AUX_DIGITAL |
            AUDIO_DEVICE_IN_BACK_MIC |
            AUDIO_DEVICE_IN_FM_RX |
            AUDIO_DEVICE_IN_ALL_SCO |
            AUDIO_DEVICE_IN_VOICE_CALL |
            AUDIO_DEVICE_IN_DEFAULT);
}

static int ste_adev_set_mode(struct audio_hw_device *dev, audio_mode_t mode)
{
    ALOG_INFO_VERBOSE("ste_adev_set_mode()");
    return 0;
}

static int ste_adev_close(hw_device_t *device)
{
    ALOG_INFO_VERBOSE("ste_adev_close()");

    ste_adev_release(device);
    anm_free(device);
    return 0;
}

static int ste_adev_open(const hw_module_t* module, const char* name,
                     hw_device_t** device)
{
    struct ste_audio_hw_device *adev;
    int ret;

    ALOG_INFO("ENTER ste_adev_open()");

    adev = create_ste_audio_hardware();
    if (!adev)
        return -ENOMEM;

    ALOG_INFO_VERBOSE("created ste_adev_open %p", adev);

    if (strcmp(name, AUDIO_HARDWARE_INTERFACE) != 0)
        return -EINVAL;


    adev->device.common.tag     = HARDWARE_DEVICE_TAG;
    adev->device.common.version = AUDIO_DEVICE_API_VERSION_1_0;
    adev->device.common.module  = (struct hw_module_t *) module;
    adev->device.common.close   = ste_adev_close;

    adev->device.get_supported_devices = ste_adev_get_supported_devices;
    adev->device.init_check            = ste_adev_init_check;
    adev->device.set_voice_volume      = ste_adev_set_voice_volume;
    adev->device.set_master_volume     = ste_adev_set_master_volume;
    adev->device.get_master_volume     = NULL;
    adev->device.set_mode              = ste_adev_set_mode;
    adev->device.set_mic_mute          = ste_adev_set_mic_mute;
    adev->device.get_mic_mute          = ste_adev_get_mic_mute;
    adev->device.set_parameters        = ste_adev_set_parameters;
    adev->device.get_parameters        = ste_adev_get_parameters;
    adev->device.get_input_buffer_size = ste_adev_get_input_buffer_size;
    adev->device.open_output_stream    = ste_adev_open_output_stream;
    adev->device.close_output_stream   = ste_adev_close_output_stream;
    adev->device.open_input_stream     = ste_adev_open_input_stream;
    adev->device.close_input_stream    = ste_adev_close_input_stream;
    adev->device.dump                  = ste_adev_dump;

    *device = &adev->device.common;

    return 0;

err_create_audio_hw:
    anm_free(adev);
    return ret;
}

static struct hw_module_methods_t ste_hal_module_methods = {
    .open = ste_adev_open,
};


struct audio_module HAL_MODULE_INFO_SYM = {
    .common = {
        .tag = HARDWARE_MODULE_TAG,
        .module_api_version = AUDIO_MODULE_API_VERSION_0_1,
        .hal_api_version = HARDWARE_HAL_API_VERSION,
        .id = AUDIO_HARDWARE_MODULE_ID,
        .name = "ST-Ericsson Audio HW HAL",
        .author = "ST-Ericsson",
        .methods = &ste_hal_module_methods,
    },
};
