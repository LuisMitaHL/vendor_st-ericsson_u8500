/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*! \file ste_anm_ahilib.cc
*   \brief ANM implementation of AHI

    Implementation of the AudioHardwareInterface
    defined in Android and used by the AudioFlinger.
*/

#define ANM_LOG_FILENAME "anm_ahi"
#include "ste_anm_dbg.h"

#include <cstdio>
#include <assert.h>

#include <utils/Log.h>
#include <cutils/properties.h>
#include <dlfcn.h>

#include "ste_anm_ahi.h"
#include <ste_adm_client.h>

/* How to translate the [0,1] volume range given from Android to millibells.
 * The translation is linear and the max volume is 0 dB. This value shoudn't
 * be too low (even though the mixer can handle -100 dB) otherwise the lower
 * part of the volume 'knobs' will be experienced as pointless (no sound).
 */
#ifndef  ANM_VOICECALL_VOLUME_FLOOR
 #define ANM_VOICECALL_VOLUME_FLOOR (-2500)
#endif

static int convert_to_adm_volume(float volume, int floor)
{
    // 0.0 --> ANM_VOLUME_FLOOR
    // 1.0 --> 0
    // linear
    return (int) ((1-volume)*floor);
}

using namespace android_audio_legacy;
/********************* Implementation of AHI ***********************************
* We implement the methods in the order they are listed in
* AudioHardwareInterface.
*
* BEGIN
*******************************************************************************/

namespace android_audio_legacy {
/**
* Implementation of create method defined in AHI, just returns
* a pointer to our implementation of AHI.
*/
AudioHardwareInterface* createAudioHardware(void)
{
    (void) ste_anm_debug_setup_log();
    ALOG_INFO("Creating Audio Hardware Interface\n");
    return new(std::nothrow) AudioHardwareANM();
}
};

/* Constructor */
AudioHardwareANM::AudioHardwareANM() :
    mDLHandle(NULL), mMuted(false), mMuteAppVol(0),
    mBtWb(false), mBtNrEc(false), mAudioPolicyManager(0), mModemType(ADM_UNKNOWN_MODEM_TYPE)
{
    if (ste_adm_client_get_modem_type(&mModemType) != STE_ADM_RES_OK) {
        ALOG_ERR("AudioHardwareANM::AudioHardwareANM(): Impossible to get modem type!\n");
    }
}

/* Destructor */
AudioHardwareANM::~AudioHardwareANM()
{
    ALOG_INFO("AudioHardwareANM(): Destroyed\n");
    for (size_t i = 0; i < mOutputs.size(); i++) {
        delete mOutputs[i];
        mOutputs.removeAt(i);
    }
    mOutputs.clear();
    for (size_t i = 0; i < mInputs.size(); i++) {
        delete mInputs[i];
        mInputs.removeAt(i);
    }
    mInputs.clear();
}

/* Check if already initialized */
status_t AudioHardwareANM::initCheck()
{
    if (mOutputs.size() > 0 || mInputs.size() > 0) {
        ALOG_INFO_VERBOSE("initCheck(): Already initialized!\n");
        return ALREADY_EXISTS;
    }

    ALOG_INFO_VERBOSE("initCheck(): OK\n");
    return NO_ERROR;
}

status_t AudioHardwareANM::setVoiceVolume(float volume)
{
    int adm_volume;

    if ((mModemType == ADM_FAT_MODEM) || (mModemType == ADM_FULL_FAT_MODEM)) {
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
        ALOG_ERR("setVoiceVolume(): "
            "ste_adm_client_set_cscall_downstream_volume failed\n");
        return UNKNOWN_ERROR;
    }

    return NO_ERROR;
}

/* @todo: Set master volume for all other cases than voice call */
status_t AudioHardwareANM::setMasterVolume(float volume)
{
    ALOG_INFO_VERBOSE("setMasterVolume(%f)\n", volume);
    return NO_ERROR;
}

status_t AudioHardwareANM::setMicMute(bool state)
{
    ALOG_INFO_VERBOSE("setMicMute(%s)\n", state ? "TRUE" : "FALSE");

    if (state) {
        if(ste_adm_client_get_cscall_upstream_volume(&mMuteCsVol)){
            ALOG_ERR("setMicMute(): "
                "ste_adm_client_get_cscall_upstream_volume() failed\n");
            return UNKNOWN_ERROR;
        }
        if(ste_adm_client_set_cscall_upstream_volume(INT_MIN)) {
            ALOG_ERR("setMicMute(): "
                "ste_adm_client_set_cscall_upstream_volume() failed\n");
            return UNKNOWN_ERROR;
        }
    } else {
        if (ste_adm_client_set_cscall_upstream_volume(mMuteCsVol)) {
            ALOG_ERR("setMicMute(): "
                "ste_adm_client_set_cscall_upstream_volume() failed\n");
            return UNKNOWN_ERROR;
        }
    }

    /* Set mic mute state for all opened inputs */
    for (size_t i = 0; i < mInputs.size(); i++) {
        mInputs[i]->setMute(state);
    }

    mMuted = state;
    return NO_ERROR;
}

status_t AudioHardwareANM::getMicMute(bool* state)
{
    *state = mMuted;
    ALOG_INFO_VERBOSE("getMicMute(): state = %s\n", mMuted ? "TRUE" : "FALSE");
    return NO_ERROR;
}

status_t AudioHardwareANM::setParameters(const String8& keyValuePairs)
{
    AudioParameter param = AudioParameter(keyValuePairs);
    String8 value;
    String8 key;
    ste_adm_tty_mode_t tty_mode=STE_ADM_TTY_MODE_OFF;
    char tty_mode_string[4][30]={"STE_ADM_TTY_MODE_OFF","STE_ADM_TTY_MODE_FULL","STE_ADM_TTY_MODE_HCO","STE_ADM_TTY_MODE_VCO"};

    ALOG_INFO("setParameters(): %s\n", keyValuePairs.string());

    key = String8(PARAM_KEY_BTWB);
    if (param.get(key, value) == NO_ERROR) {
        mBtWb = (value == "on");
    }

    key = String8(PARAM_KEY_BTNREC);
    if (param.get(key, value) == NO_ERROR) {
        mBtNrEc = (value == "on");
    }

    key = String8(PARAM_KEY_AP_REF);
    int ptr = 0;
    if (param.getInt(key, ptr) == NO_ERROR) {
        mAudioPolicyManager = ptr;
    }

    if ((mModemType == ADM_FAT_MODEM)|| (mModemType == ADM_FULL_FAT_MODEM)) {
        key = String8(PARAM_KEY_TTY_MODE);
        if(param.get(key,value) == NO_ERROR) {
            if(value == "tty_full"){
                tty_mode = STE_ADM_TTY_MODE_FULL;
            }
            else if(value == "tty_vco"){
                tty_mode = STE_ADM_TTY_MODE_VCO;
            }
            else if(value == "tty_hco"){
                tty_mode = STE_ADM_TTY_MODE_HCO;
            }
            else{
                tty_mode = STE_ADM_TTY_MODE_OFF;
            }

            /* */
            if (ste_adm_client_set_cscall_tty_mode(tty_mode)) {
                ALOG_ERR("setParameters(): ste_adm_client_set_cscall_tty_mode failed\n");
                return UNKNOWN_ERROR;
            }

            ALOG_INFO("setParameters(): ste_adm_client_set_cscall_tty_mode(%s) SUCCESS\n", tty_mode_string[tty_mode]);
        }
    }
    return NO_ERROR;
}

String8 AudioHardwareANM::getParameters(const String8& keys)
{
    AudioParameter param = AudioParameter(keys);
    AudioParameter ahiParam = AudioParameter();
    String8 keyValuePairs;
    String8 value;
    String8 key;

    key = String8(PARAM_KEY_BTWB);
    if (param.get(key, value) == NO_ERROR) {
        value = mBtWb ? "on" : "off";
        ahiParam.add(key,value);
    }

    key = String8(PARAM_KEY_BTNREC);
    if (param.get(key, value) == NO_ERROR) {
        value = mBtNrEc ? "on" : "off";
        ahiParam.add(key,value);
    }

    key = String8(PARAM_KEY_EC_SUPPORTED);
    if (param.get(key, value) == NO_ERROR) {
        value = "true"; /* Echo cancellation is supported */
        ahiParam.add(key,value);
    }

    key = String8(PARAM_KEY_AP_REF);
    if (param.get(key, value) == NO_ERROR) {
        ahiParam.addInt(key,mAudioPolicyManager);
    }

    keyValuePairs = ahiParam.toString();
    ALOG_INFO_VERBOSE("getParameters(): %s - %s", keys.string(), keyValuePairs.string());

    return keyValuePairs;
}

AudioStreamOut* AudioHardwareANM::openOutputStream(
    uint32_t    devices,
    int         *format,
    uint32_t    *channels,
    uint32_t    *sampleRate,
    status_t    *status)
{
    assert(status != NULL);
    assert(format != NULL);
    assert(channels != NULL);
    assert(sampleRate != NULL);

    (void) ste_anm_debug_setup_log();

    ALOG_INFO("ENTER openOutputStream(): format %d, channels 0x%08X (%d), "
        "sampleRate %d, devices=%X\n", *format, *channels, AudioSystem::popCount(*channels),
        *sampleRate, devices);

    AudioStreamOutANM* output = new(std::nothrow) AudioStreamOutANM();
    if (output == NULL) {
        ALOG_ERR("openOutputStream(): Failed to allocate AudioStreamOutANM!\n");
        *status = NO_MEMORY;
        return NULL;
    }

    *status = output->setup(format, channels, sampleRate);

    if (*status != OK) {
        ALOG_ERR("openOutputStream(): Failed to store audio settings!\n");
        delete output;
        output = NULL;
    } else {
        *status = output->changeDevice(mMode, devices);
        if (*status == OK) {
            /* Add to the list of opened outputs */
            mOutputs.add(output);
            ALOG_INFO_VERBOSE("openOutputStream(): Successfully set up output stream\n");
        } else {
            ALOG_ERR("openOutputStream(): Failed to setup output stream!\n");
            delete output;
            output = NULL;
        }
    }

    ALOG_INFO("LEAVE openOutputStream(): format %d, channels 0x%08X (%d), "
        "samplerate %d\n", *format, *channels, AudioSystem::popCount(*channels),
        *sampleRate);

    return output;
}

void AudioHardwareANM::closeOutputStream(AudioStreamOut * out)
{
    ALOG_INFO("closeOutputStream(): output %x", (unsigned int)out);

    ssize_t index = mOutputs.indexOf((AudioStreamOutANM *)out);
    if (index < 0) {
        ALOG_WARN("closeOutputStream(): Unknown output stream %x\n",
            (unsigned int)out);
    } else {
        delete mOutputs[index];
        mOutputs.removeAt(index);
    }
}

AudioStreamIn* AudioHardwareANM::openInputStream(
    uint32_t    devices,
    int         *format,
    uint32_t    *channels,
    uint32_t    *sampleRate,
    status_t    *status,
    AudioSystem::audio_in_acoustics acoustics)
{
    assert(status != NULL);
    assert(format != NULL);
    assert(channels != NULL);
    assert(sampleRate != NULL);

    (void) ste_anm_debug_setup_log();

    ALOG_INFO("ENTER openInputStream(): "
        "format %d, channels 0x%08X (%d), samplerate %d\n",
        *format, *channels, AudioSystem::popCount(*channels), *sampleRate);

    AudioStreamInANM* input = new(std::nothrow) AudioStreamInANM();
    if (input == NULL) {
        ALOG_ERR("openInputStream(): Failed to allocate AudioStreamInANM!\n");
        *status = NO_MEMORY;
        return NULL;
    }

    *status = input->setup(format, channels, sampleRate);
    input->setMute(mMuted);

    if (*status != OK) {
        ALOG_ERR("openInputStream(): Failed to store audio settings!\n");
        delete input;
        input = NULL;
    } else {
        *status = input->changeDevice(mMode, devices);
        if (*status == OK) {
            /* Add to the list of opened inputs */
            mInputs.add(input);
            ALOG_INFO_VERBOSE("openInputStream(): Successfully set up input stream\n");
        } else {
            ALOG_ERR("openInputStream(): Failed to setup input stream!\n");
            delete input;
            input = NULL;
        }
    }

    ALOG_INFO("LEAVE openInputStream(): "
        "format %d, channels 0x%08X (%d), samplerate %d\n",
        *format, *channels, AudioSystem::popCount(*channels), *sampleRate);

    return input;
}

void AudioHardwareANM::closeInputStream(AudioStreamIn *in)
{
    ALOG_INFO("closeInputStream(): input %x", (unsigned int)in);

    ssize_t index = mInputs.indexOf((AudioStreamInANM *)in);
    if (index < 0) {
        ALOG_WARN("closeInputStream(): Unknown input stream %x\n",
            (unsigned int)in);
    } else {
        delete mInputs[index];
        mInputs.removeAt(index);
    }
}

/* Protected methods */
status_t AudioHardwareANM::dump(int fd, const Vector<String16>& args)
{
    ALOG_INFO_VERBOSE("dump()");
    return NO_ERROR;
}

size_t AudioHardwareANM::getInputBufferSize(
    uint32_t sampleRate, int format, int channelCount)
{
    size_t size = 0;

    if (format != AudioSystem::PCM_16_BIT) {
        ALOG_WARN("getInputBufferSize(): bad format: %d", format);
        return 0;
    }
    if (channelCount != 1 && channelCount != 2) {
        ALOG_WARN("getInputBufferSize(): bad channel count: %d", channelCount);
        return 0;
    }

    size = sampleRate * channelCount * 2 * 20 / 1000;
    ALOG_INFO_VERBOSE("getInputBufferSize(): samplerate %d, format %x, channels %d - "
        "returned buffer size = %d", sampleRate, format, channelCount, size);
    return size;
}
