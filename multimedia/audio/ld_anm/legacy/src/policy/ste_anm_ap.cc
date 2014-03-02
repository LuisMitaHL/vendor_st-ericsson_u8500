/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*! \file ste_anm_ap.cc
*   \brief ST-Ericsson Audio Policy Manager

    ST-Ericsson implementation of the Audio Policy Manager
    to be used by Audio Flinger in Android framework.
*/
#define ANM_LOG_FILENAME "anm_ap"
#include "ste_anm_dbg.h"

#include "ste_anm_ap.h"
#include "ste_anm_ahi.h"
#include <media/mediarecorder.h>
#include <ste_adm_client.h>

#define DEFAULT_INPUT_SAMPLE_RATE       8000
#define DEFAULT_OUTPUT_SAMPLE_RATE      44100
#define DEFAULT_INPUT_CHANNELS          AudioSystem::CHANNEL_IN_MONO
#define DEFAULT_OUTPUT_CHANNELS         AudioSystem::CHANNEL_OUT_STEREO
#define DEFAULT_PCM_FORMAT              AudioSystem::PCM_16_BIT
#define MAX_CSCALL_SAMPLE_RATE          16000
#define AUDIO_SOURCE_VOICE_CALL_NO_RECORD AUDIO_SOURCE_LIST_END
#define LOCK_MUTEX(mutex) \
{ \
    if (&(mutex) == &mMutexForceUse) {\
        ALOG_INFO_VERBOSE("LOCK MUTEX FORCEUSE, Line=%d", __LINE__);\
    } else if (&(mutex) == &mMutexCallDevices) {\
        ALOG_INFO_VERBOSE("LOCK MUTEX CALLDEVICES, Line=%d", __LINE__);\
    } else if (&(mutex) == &mMutexCallStatus) {\
        ALOG_INFO_VERBOSE("LOCK MUTEX CALLSTATUS, Line=%d", __LINE__);\
    } else if (&(mutex) == &mMutexVC) {\
        ALOG_INFO_VERBOSE("LOCK MUTEX VC, Line=%d", __LINE__);\
    } else if (&(mutex) == &mMutexIO) {\
        ALOG_INFO_VERBOSE("LOCK MUTEX IO, Line=%d", __LINE__);\
    } else if (&(mutex) == &mMutexDevState) {\
        ALOG_INFO_VERBOSE("LOCK MUTEX DEVICE STATE, Line=%d", __LINE__);\
    } else {\
        ALOG_INFO_VERBOSE("LOCK MUTEX, Line=%d", __LINE__);\
    }\
    pthread_mutex_lock(&(mutex));\
}
#define UNLOCK_MUTEX(mutex) \
{ \
    pthread_mutex_unlock(&(mutex));\
    if (&(mutex) == &mMutexForceUse) {\
        ALOG_INFO_VERBOSE("UNLOCK MUTEX FORCEUSE, Line=%d", __LINE__);\
    } else if (&(mutex) == &mMutexCallDevices) {\
        ALOG_INFO_VERBOSE("UNLOCK MUTEX CALLDEVICES, Line=%d", __LINE__);\
    } else if (&(mutex) == &mMutexCallStatus) {\
        ALOG_INFO_VERBOSE("UNLOCK MUTEX CALLSTATUS, Line=%d", __LINE__);\
    } else if (&(mutex) == &mMutexVC) {\
        ALOG_INFO_VERBOSE("UNLOCK MUTEX VC, Line=%d", __LINE__);\
    } else if (&(mutex) == &mMutexIO) {\
        ALOG_INFO_VERBOSE("UNLOCK MUTEX IO, Line=%d", __LINE__);\
    } else if (&(mutex) == &mMutexDevState) {\
        ALOG_INFO_VERBOSE("UNLOCK MUTEX DEVICE STATE, Line=%d", __LINE__);\
    } else {\
        ALOG_INFO_VERBOSE("UNLOCK MUTEX, Line=%d", __LINE__);\
    }\
}

namespace android_audio_legacy
{

#define DEVLIST_MAX_NO_DEVICES 16
/* ---------------------------------------------------------------------------
 * Default available input and output devices
 * ---------------------------------------------------------------------------*/
uint32_t defaultOutputDevices = AudioSystem::DEVICE_OUT_DEFAULT  |
                                AudioSystem::DEVICE_OUT_EARPIECE |
                                AudioSystem::DEVICE_OUT_SPEAKER  ;

uint32_t defaultInputDevices  = AudioSystem::DEVICE_IN_DEFAULT       |
                                AudioSystem::DEVICE_IN_BUILTIN_MIC   |
                                AudioSystem::DEVICE_IN_FM_RADIO_RX   |
                                AudioSystem::DEVICE_IN_VOICE_CALL    ;

/* ---------------------------------------------------------------------------
 * Output Device Prio Lists
 * ---------------------------------------------------------------------------*/
uint32_t mediaDevices[DEVLIST_MAX_NO_DEVICES] = {
    AudioSystem::DEVICE_OUT_AUX_DIGITAL,
    AudioSystem::DEVICE_OUT_BLUETOOTH_A2DP,
    AudioSystem::DEVICE_OUT_BLUETOOTH_A2DP_HEADPHONES,
    AudioSystem::DEVICE_OUT_FM_RADIO_TX,
    AudioSystem::DEVICE_OUT_WIRED_HEADPHONE,
    AudioSystem::DEVICE_OUT_WIRED_HEADSET,
    AudioSystem::DEVICE_OUT_ANLG_DOCK_HEADSET,
    AudioSystem::DEVICE_OUT_SPEAKER,
    AudioSystem::DEVICE_OUT_EARPIECE,
    0
};

uint32_t mediaInCallDevices[DEVLIST_MAX_NO_DEVICES] = {
    AudioSystem::DEVICE_OUT_AUX_DIGITAL,
    AudioSystem::DEVICE_OUT_FM_RADIO_TX,
    AudioSystem::DEVICE_OUT_WIRED_HEADPHONE,
    AudioSystem::DEVICE_OUT_WIRED_HEADSET,
    AudioSystem::DEVICE_OUT_EARPIECE,
    0
};

uint32_t sonificationDevices[DEVLIST_MAX_NO_DEVICES] = {
    AudioSystem::DEVICE_OUT_SPEAKER | AudioSystem::DEVICE_OUT_BLUETOOTH_A2DP_HEADPHONES,
    AudioSystem::DEVICE_OUT_SPEAKER | AudioSystem::DEVICE_OUT_BLUETOOTH_A2DP,
    AudioSystem::DEVICE_OUT_SPEAKER | AudioSystem::DEVICE_OUT_ANLG_DOCK_HEADSET,
    AudioSystem::DEVICE_OUT_SPEAKER | AudioSystem::DEVICE_OUT_WIRED_HEADPHONE,
    AudioSystem::DEVICE_OUT_SPEAKER | AudioSystem::DEVICE_OUT_WIRED_HEADSET,
    AudioSystem::DEVICE_OUT_SPEAKER,
    0
};

uint32_t sonificationInCallDevices[DEVLIST_MAX_NO_DEVICES] = {
    AudioSystem::DEVICE_OUT_SPEAKER | AudioSystem::DEVICE_OUT_WIRED_HEADPHONE,
    AudioSystem::DEVICE_OUT_SPEAKER | AudioSystem::DEVICE_OUT_WIRED_HEADSET,
    AudioSystem::DEVICE_OUT_SPEAKER,
    0
};

uint32_t phoneDevices[DEVLIST_MAX_NO_DEVICES] = {
    AudioSystem::DEVICE_OUT_BLUETOOTH_A2DP,
    AudioSystem::DEVICE_OUT_BLUETOOTH_A2DP_HEADPHONES,
    AudioSystem::DEVICE_OUT_WIRED_HEADPHONE,
    AudioSystem::DEVICE_OUT_WIRED_HEADSET,
    AudioSystem::DEVICE_OUT_EARPIECE,
    0
};

uint32_t phoneInCallDevices[DEVLIST_MAX_NO_DEVICES] = {
    AudioSystem::DEVICE_OUT_WIRED_HEADPHONE,
    AudioSystem::DEVICE_OUT_WIRED_HEADSET,
    AudioSystem::DEVICE_OUT_EARPIECE,
    0
};

/* ---------------------------------------------------------------------------
 * Input Device Prio Lists
 * ---------------------------------------------------------------------------*/

uint32_t phoneInputDevices[DEVLIST_MAX_NO_DEVICES] = {
    AudioSystem::DEVICE_IN_WIRED_HEADSET,
    AudioSystem::DEVICE_IN_BUILTIN_MIC,
    0
};

uint32_t phoneInCallRecordDevices[DEVLIST_MAX_NO_DEVICES] = {
    AudioSystem::DEVICE_IN_VOICE_CALL,
    0
};

uint32_t camcorderInputDevices[DEVLIST_MAX_NO_DEVICES] = {
    AudioSystem::DEVICE_IN_WIRED_HEADSET,
    AudioSystem::DEVICE_IN_BUILTIN_MIC,
    0
};

uint32_t micStereoInputDevices[DEVLIST_MAX_NO_DEVICES] = {
    AudioSystem::DEVICE_IN_WIRED_HEADSET,
    AudioSystem::DEVICE_IN_BUILTIN_MIC,
    0
};

uint32_t micMonoInputDevices[DEVLIST_MAX_NO_DEVICES] = {
    AudioSystem::DEVICE_IN_WIRED_HEADSET,
    AudioSystem::DEVICE_IN_BUILTIN_MIC,
    0
};

uint32_t fmInputDevices[DEVLIST_MAX_NO_DEVICES] = {
    AudioSystem::DEVICE_IN_FM_RADIO_RX,
    0
};

/* ---------------------------------------------------------------------------
 * Strategy priority is used to select the preferred device
 * configuration when a device is used by several strategies.
 * ---------------------------------------------------------------------------*/
const int strategyPriority[AudioPolicyManagerANM::NUM_STRATEGIES+1]=
{
    1,  // STRATEGY_MEDIA,
    5,  // STRATEGY_PHONE,
    3,  // STRATEGY_SONIFICATION,
    1,  // STRATEGY_DTMF,
    0   // STRATEGY_UNKNOWN mapped to lowest prio
};

/* ---------------------------------------------------------------------------
 * Source priority is used to select the preferred device
 * configuration when a device is used by several input sources.
 * ---------------------------------------------------------------------------*/
const int sourcePriority[AUDIO_SOURCE_LIST_END+2]=
{
    1, // AUDIO_SOURCE_DEFAULT
    1, // AUDIO_SOURCE_MIC
    3, // AUDIO_SOURCE_VOICE_UPLINK
    3, // AUDIO_SOURCE_VOICE_DOWNLINK
    3, // AUDIO_SOURCE_VOICE_CALL
    3, // AUDIO_SOURCE_CAMCORDER
    3, // AUDIO_SOURCE_VOICE_RECOGNITION
    3, // AUDIO_SOURCE_VOICE_COMMUNICATION
    3, // AUDIO_SOURCE_FM_RADIO_RX
    3, // AUDIO_SOURCE_SPEECH_PROC

#ifdef STE_VIDEO_CALL
    3, // AUDIO_SOURCE_VIDEO_CALL
#endif
    5, // AUDIO_SOURCE_VOICE_CALL_NO_RECORD
    0  // AUDIO_SOURCE_UNKNOWN mapped to lowest prio
};

/* ---------------------------------------------------------------------------
 * Device configurations, i.e. the actual devices.
 * Must exist in Device table of the ADM database
 * ---------------------------------------------------------------------------*/
#define DEVICE_ACTUAL_A2DP                     "REF_A2DP"
#define DEVICE_ACTUAL_BTIN                     "REF_BTIn"
#define DEVICE_ACTUAL_BTIN_NO_NR_EC            "REF_BTIn_No_NR_EC"
#define DEVICE_ACTUAL_BTOUT                    "REF_BTOut"
#define DEVICE_ACTUAL_BTOUT_NO_NR_EC           "REF_BTOut_No_NR_EC"
#define DEVICE_ACTUAL_BTWBIN                   "REF_BTWBIn"
#define DEVICE_ACTUAL_BTWBIN_NO_NR_EC          "REF_BTWBIn_No_NR_EC"
#define DEVICE_ACTUAL_BTWBOUT                  "REF_BTWBOut"
#define DEVICE_ACTUAL_BTWBOUT_NO_NR_EC         "REF_BTWBOut_No_NR_EC"
#define DEVICE_ACTUAL_EARPIECE                 "REF_Earpiece"
#define DEVICE_ACTUAL_EARPIECE_MEDIA           "REF_Earpiece_Media"
#define DEVICE_ACTUAL_EARPIECE_SONIFICATION    "REF_Earpiece_Sonification"
#define DEVICE_ACTUAL_FMRX                     "REF_FMIn"
#define DEVICE_ACTUAL_FMTX                     "REF_FMOut"
#define DEVICE_ACTUAL_HDMI                     "REF_HDMI"
#define DEVICE_ACTUAL_HSIN                     "REF_HSetIn"
#define DEVICE_ACTUAL_HSIN_CAMCORDER           "REF_HSetIn_Camcorder"
#define DEVICE_ACTUAL_HSOUT                    "REF_HSetOut"
#define DEVICE_ACTUAL_HSOUT_MEDIA              "REF_HSetOut_Media"
#define DEVICE_ACTUAL_HSOUT_SONIFICATION       "REF_HSetOut_Sonification"
#define DEVICE_ACTUAL_MIC                      "REF_Mic"
#define DEVICE_ACTUAL_MIC_EARPIECE             "REF_Mic_Earpiece"
#define DEVICE_ACTUAL_MIC_CAMCORDER            "REF_Mic_Camcorder"
#define DEVICE_ACTUAL_MIC_HSOUT                "REF_Mic_HSetOut"
#define DEVICE_ACTUAL_MIC_SPEAKER              "REF_Mic_Speaker"
#define DEVICE_ACTUAL_SPEAKER                  "REF_Speaker"
#define DEVICE_ACTUAL_SPEAKER_MEDIA            "REF_Speaker_Media"
#define DEVICE_ACTUAL_SPEAKER_SONIFICATION     "REF_Speaker_Sonification"
#define DEVICE_ACTUAL_VOIPIN                   ""
#define DEVICE_ACTUAL_VOIPOUT                  ""
#define DEVICE_ACTUAL_USBOUT                   "REF_USBOut"
#define DEVICE_ACTUAL_VOICE_CALL               ""

#define DEVICE_ACTUAL_BTIN_VOIP                     "REF_BTInVoIP"
#define DEVICE_ACTUAL_BTIN_NO_NR_EC_VOIP            "REF_BTInVoIP_No_NR_EC"
#define DEVICE_ACTUAL_BTOUT_VOIP                    "REF_BTOutVoIP"
#define DEVICE_ACTUAL_BTOUT_NO_NR_EC_VOIP           "REF_BTOutVoIP_No_NR_EC"
#define DEVICE_ACTUAL_BTWBIN_VOIP                   "REF_BTWBInVoIP"
#define DEVICE_ACTUAL_BTWBIN_NO_NR_EC_VOIP          "REF_BTWBInVoIP_No_NR_EC"
#define DEVICE_ACTUAL_BTWBOUT_VOIP                  "REF_BTWBOutVoIP"
#define DEVICE_ACTUAL_BTWBOUT_NO_NR_EC_VOIP         "REF_BTWBOutVoIP_No_NR_EC"
#define DEVICE_ACTUAL_EARPIECE_VOIP                 "REF_EarpieceVoIP"
#define DEVICE_ACTUAL_HSIN_VOIP                     "REF_HSetInVoIP"
#define DEVICE_ACTUAL_HSOUT_VOIP                    "REF_HSetOutVoIP"
#define DEVICE_ACTUAL_MIC_VOIP                      "REF_MicVoIP"
#define DEVICE_ACTUAL_MIC_EARPIECE_VOIP             "REF_Mic_EarpieceVoIP"
#define DEVICE_ACTUAL_MIC_HSOUT_VOIP                "REF_Mic_HSetOutVoIP"
#define DEVICE_ACTUAL_MIC_SPEAKER_VOIP              "REF_Mic_SpeakerVoIP"
#define DEVICE_ACTUAL_SPEAKER_VOIP                  "REF_SpeakerVoIP"
#define DEVICE_ACTUAL_USBOUT_VOIP                   "REF_USBOutVoIP"



/* ---------------------------------------------------------------------------
 * Top level ADM devices
 * ---------------------------------------------------------------------------*/
const char* topLevelOutputDevices[] = {
    STE_ADM_DEVICE_STRING_EARP,     // DEV_OUT_INDEX_EARPIECE
    STE_ADM_DEVICE_STRING_SPEAKER,  // DEV_OUT_INDEX_SPEAKER
    STE_ADM_DEVICE_STRING_HSOUT,    // DEV_OUT_INDEX_HEADSET
    STE_ADM_DEVICE_STRING_HSOUT,    // DEV_OUT_INDEX_HEADPHONE
    STE_ADM_DEVICE_STRING_A2DP,     // DEV_OUT_INDEX_A2DP
    STE_ADM_DEVICE_STRING_FMTX,     // DEV_OUT_INDEX_FMTX
    STE_ADM_DEVICE_STRING_HDMI,     // DEV_OUT_INDEX_HDMI
    STE_ADM_DEVICE_STRING_BTOUT,    // DEV_OUT_INDEX_BT
    STE_ADM_DEVICE_STRING_BTWBOUT,  // DEV_OUT_INDEX_BTWB
    STE_ADM_DEVICE_STRING_VOIPOUT,  // DEV_OUT_INDEX_VOIP
    STE_ADM_DEVICE_STRING_USBOUT,   // DEV_OUT_INDEX_USB
    STE_ADM_DEVICE_STRING_SPEAKER,  // DEV_OUT_INDEX_DEFAULT
};

const char* topLevelInputDevices[] = {
    STE_ADM_DEVICE_STRING_MIC,      // DEV_IN_INDEX_MIC
    STE_ADM_DEVICE_STRING_HSIN,     // DEV_IN_INDEX_HEADSET
    STE_ADM_DEVICE_STRING_BTIN,     // DEV_IN_INDEX_BT
    STE_ADM_DEVICE_STRING_BTWBIN,   // DEV_IN_INDEX_BTWB
    STE_ADM_DEVICE_STRING_FMRX,     // DEV_IN_INDEX_FMRX
    STE_ADM_DEVICE_STRING_VOIPIN,   // DEV_IN_INDEX_VOIP
    STE_ADM_DEVICE_STRING_DICT_REC, // DEV_IN_INDEX_VOICE_CALL
    STE_ADM_DEVICE_STRING_MIC,      // DEV_IN_INDEX_DEFAULT
};

/* ---------------------------------------------------------------------------
 * Actual ADM devices
 * ---------------------------------------------------------------------------*/
const char* actualDefaultOutputDevices[] = {
    DEVICE_ACTUAL_EARPIECE,                // DEV_OUT_INDEX_EARPIECE
    DEVICE_ACTUAL_SPEAKER,                 // DEV_OUT_INDEX_SPEAKER
    DEVICE_ACTUAL_HSOUT,                   // DEV_OUT_INDEX_HEADSET
    DEVICE_ACTUAL_HSOUT,                   // DEV_OUT_INDEX_HEADPHONE
    DEVICE_ACTUAL_A2DP,                    // DEV_OUT_INDEX_A2DP
    DEVICE_ACTUAL_FMTX,                    // DEV_OUT_INDEX_FMTX
    DEVICE_ACTUAL_HDMI,                    // DEV_OUT_INDEX_HDMI
    DEVICE_ACTUAL_BTOUT,                   // DEV_OUT_INDEX_BT
    DEVICE_ACTUAL_BTWBOUT,                 // DEV_OUT_INDEX_BTWB
    DEVICE_ACTUAL_VOIPOUT,                 // DEV_OUT_INDEX_VOIP
    DEVICE_ACTUAL_USBOUT,                  // DEV_OUT_INDEX_USB
    DEVICE_ACTUAL_SPEAKER,                 // DEV_OUT_INDEX_DEFAULT
};

const char* actualDefaultVoIPOutputDevices[] = {
    DEVICE_ACTUAL_EARPIECE_VOIP,           // DEV_OUT_INDEX_EARPIECE
    DEVICE_ACTUAL_SPEAKER_VOIP,            // DEV_OUT_INDEX_SPEAKER
    DEVICE_ACTUAL_HSOUT_VOIP,              // DEV_OUT_INDEX_HEADSET
    DEVICE_ACTUAL_HSOUT_VOIP,              // DEV_OUT_INDEX_HEADPHONE
    DEVICE_ACTUAL_A2DP,                    // DEV_OUT_INDEX_A2DP
    DEVICE_ACTUAL_FMTX,                    // DEV_OUT_INDEX_FMTX
    DEVICE_ACTUAL_HDMI,                    // DEV_OUT_INDEX_HDMI
    DEVICE_ACTUAL_BTOUT_VOIP,              // DEV_OUT_INDEX_BT
    DEVICE_ACTUAL_BTWBOUT_VOIP,            // DEV_OUT_INDEX_BTWB
    DEVICE_ACTUAL_VOIPOUT,                 // DEV_OUT_INDEX_VOIP
    DEVICE_ACTUAL_USBOUT_VOIP,             // DEV_OUT_INDEX_USB
    DEVICE_ACTUAL_SPEAKER_VOIP,            // DEV_OUT_INDEX_DEFAULT
};


const char* actualMediaOutputDevices[] = {
    DEVICE_ACTUAL_EARPIECE_MEDIA,          // DEV_OUT_INDEX_EARPIECE
    DEVICE_ACTUAL_SPEAKER_MEDIA,           // DEV_OUT_INDEX_SPEAKER
    DEVICE_ACTUAL_HSOUT_MEDIA,             // DEV_OUT_INDEX_HEADSET
    DEVICE_ACTUAL_HSOUT_MEDIA,             // DEV_OUT_INDEX_HEADPHONE
    DEVICE_ACTUAL_A2DP,                    // DEV_OUT_INDEX_A2DP
    DEVICE_ACTUAL_FMTX,                    // DEV_OUT_INDEX_FMTX
    DEVICE_ACTUAL_HDMI,                    // DEV_OUT_INDEX_HDMI
    DEVICE_ACTUAL_BTOUT,                   // DEV_OUT_INDEX_BT
    DEVICE_ACTUAL_BTWBOUT,                 // DEV_OUT_INDEX_BTWB
    DEVICE_ACTUAL_VOIPOUT,                 // DEV_OUT_INDEX_VOIP
    DEVICE_ACTUAL_USBOUT,                  // DEV_OUT_INDEX_USB
    DEVICE_ACTUAL_SPEAKER_MEDIA,           // DEV_OUT_INDEX_DEFAULT
};

const char* actualSonificationOutputDevices[] = {
    DEVICE_ACTUAL_EARPIECE,                // DEV_OUT_INDEX_EARPIECE
    DEVICE_ACTUAL_SPEAKER_SONIFICATION,    // DEV_OUT_INDEX_SPEAKER
    DEVICE_ACTUAL_HSOUT_SONIFICATION,      // DEV_OUT_INDEX_HEADSET
    DEVICE_ACTUAL_HSOUT_SONIFICATION,      // DEV_OUT_INDEX_HEADPHONE
    DEVICE_ACTUAL_A2DP,                    // DEV_OUT_INDEX_A2DP
    DEVICE_ACTUAL_FMTX,                    // DEV_OUT_INDEX_FMTX
    DEVICE_ACTUAL_HDMI,                    // DEV_OUT_INDEX_HDMI
    DEVICE_ACTUAL_BTOUT,                   // DEV_OUT_INDEX_BT
    DEVICE_ACTUAL_BTWBOUT,                 // DEV_OUT_INDEX_BTWB
    DEVICE_ACTUAL_VOIPOUT,                 // DEV_OUT_INDEX_VOIP
    DEVICE_ACTUAL_USBOUT,                  // DEV_OUT_INDEX_USB
    DEVICE_ACTUAL_SPEAKER_SONIFICATION,    // DEV_OUT_INDEX_DEFAULT
};

const char* actualDefaultInputDevices[] = {
    DEVICE_ACTUAL_MIC,                     // DEV_IN_INDEX_MIC
    DEVICE_ACTUAL_HSIN,                    // DEV_IN_INDEX_HEADSET
    DEVICE_ACTUAL_BTIN,                    // DEV_IN_INDEX_BT
    DEVICE_ACTUAL_BTWBIN,                  // DEV_IN_INDEX_BTWB
    DEVICE_ACTUAL_FMRX,                    // DEV_IN_INDEX_FMRX
    DEVICE_ACTUAL_VOIPIN,                  // DEV_IN_INDEX_VOIP
    DEVICE_ACTUAL_VOICE_CALL,              // DEV_IN_INDEX_VOICE_CALL
    DEVICE_ACTUAL_MIC,                     // DEV_IN_INDEX_DEFAULT
};

const char* actualDefaultVoIPInputDevices[] = {
    DEVICE_ACTUAL_MIC_VOIP,                // DEV_IN_INDEX_MIC
    DEVICE_ACTUAL_HSIN_VOIP,               // DEV_IN_INDEX_HEADSET
    DEVICE_ACTUAL_BTIN_VOIP,               // DEV_IN_INDEX_BT
    DEVICE_ACTUAL_BTWBIN_VOIP,             // DEV_IN_INDEX_BTWB
    DEVICE_ACTUAL_FMRX,                    // DEV_IN_INDEX_FMRX
    DEVICE_ACTUAL_VOIPIN,                  // DEV_IN_INDEX_VOIP
    DEVICE_ACTUAL_VOICE_CALL,              // DEV_IN_INDEX_VOICE_CALL
    DEVICE_ACTUAL_MIC_VOIP,                // DEV_IN_INDEX_DEFAULT
};


const char* actualCamcorderInputDevices[] = {
    DEVICE_ACTUAL_MIC_CAMCORDER,           // DEV_IN_INDEX_MIC
    DEVICE_ACTUAL_HSIN_CAMCORDER,          // DEV_IN_INDEX_HEADSET
    DEVICE_ACTUAL_BTIN,                    // DEV_IN_INDEX_BT
    DEVICE_ACTUAL_BTWBIN,                  // DEV_IN_INDEX_BTWB
    DEVICE_ACTUAL_FMRX,                    // DEV_IN_INDEX_FMRX
    DEVICE_ACTUAL_VOIPIN,                  // DEV_IN_INDEX_VOIP
    DEVICE_ACTUAL_MIC,                     // DEV_IN_INDEX_DEFAULT
};

/* ---------------------------------------------------------------------------
 * Output Device Constraint List
 *
 * The constraint list for a specific device contains devices that are not
 * allowed to be used simultaneously as the device for which the list is valid.
 * ---------------------------------------------------------------------------*/
uint32_t constraintsEarpiece[DEVLIST_MAX_NO_DEVICES] = {
    AudioSystem::DEVICE_OUT_WIRED_HEADPHONE,
    AudioSystem::DEVICE_OUT_WIRED_HEADSET,
    AudioSystem::DEVICE_OUT_SPEAKER,
    AudioSystem::DEVICE_OUT_ANLG_DOCK_HEADSET,
    0
};

/* ---------------------------------------------------------------------------
 * Type to string mapping functions
 * ---------------------------------------------------------------------------*/

static const char* device2str(AudioSystem::audio_devices dev)
{
    switch (dev) {
    /* Output devices */
    case AudioSystem::DEVICE_OUT_EARPIECE:          return "DEVICE_OUT_EARPIECE";
    case AudioSystem::DEVICE_OUT_SPEAKER:           return "DEVICE_OUT_SPEAKER";
    case AudioSystem::DEVICE_OUT_WIRED_HEADSET:     return "DEVICE_OUT_WIRED_HEADSET";
    case AudioSystem::DEVICE_OUT_ANLG_DOCK_HEADSET: return "DEVICE_OUT_ANLG_DOCK_HEADSET";
    case AudioSystem::DEVICE_OUT_WIRED_HEADPHONE:   return "DEVICE_OUT_WIRED_HEADPHONE";
    case AudioSystem::DEVICE_OUT_SPEAKER|AudioSystem::DEVICE_OUT_WIRED_HEADSET: return "DEVICE_OUT_SPEAKER, DEVICE_OUT_WIRED_HEADSET";
    case AudioSystem::DEVICE_OUT_SPEAKER|AudioSystem::DEVICE_OUT_WIRED_HEADPHONE: return "DEVICE_OUT_SPEAKER, DEVICE_OUT_WIRED_HEADPHONE";
    case AudioSystem::DEVICE_OUT_BLUETOOTH_SCO:     return "DEVICE_OUT_BLUETOOTH_SCO";
    case AudioSystem::DEVICE_OUT_BLUETOOTH_SCO_HEADSET: return "DEVICE_OUT_BLUETOOTH_SCO_HEADSET";
    case AudioSystem::DEVICE_OUT_BLUETOOTH_SCO_CARKIT:  return "DEVICE_OUT_BLUETOOTH_SCO_CARKIT";
    case AudioSystem::DEVICE_OUT_BLUETOOTH_A2DP:    return "DEVICE_OUT_BLUETOOTH_A2DP";
    case AudioSystem::DEVICE_OUT_BLUETOOTH_A2DP_HEADPHONES: return "DEVICE_OUT_BLUETOOTH_A2DP_HEADPHONES";
    case AudioSystem::DEVICE_OUT_BLUETOOTH_A2DP_SPEAKER:    return "DEVICE_OUT_BLUETOOTH_A2DP_SPEAKER";
    case AudioSystem::DEVICE_OUT_AUX_DIGITAL:       return "DEVICE_OUT_AUX_DIGITAL";
    case AudioSystem::DEVICE_OUT_FM_RADIO_TX:       return "DEVICE_OUT_FM_RADIO_TX";
    case AudioSystem::DEVICE_OUT_DEFAULT:           return "DEVICE_OUT_DEFAULT";
    case AudioSystem::DEVICE_OUT_ALL:               return "DEVICE_OUT_ALL";
    case AudioSystem::DEVICE_OUT_ALL_A2DP:          return "DEVICE_OUT_ALL_A2DP";
    /* Input devices */
    case AudioSystem::DEVICE_IN_COMMUNICATION:      return "DEVICE_IN_COMMUNICATION";
    case AudioSystem::DEVICE_IN_AMBIENT:            return "DEVICE_IN_AMBIENT";
    case AudioSystem::DEVICE_IN_BUILTIN_MIC:        return "DEVICE_IN_BUILTIN_MIC";
    case AudioSystem::DEVICE_IN_BLUETOOTH_SCO_HEADSET:  return "DEVICE_IN_BLUETOOTH_SCO_HEADSET";
    case AudioSystem::DEVICE_IN_WIRED_HEADSET:      return "DEVICE_IN_WIRED_HEADSET";
    case AudioSystem::DEVICE_IN_AUX_DIGITAL:        return "DEVICE_IN_AUX_DIGITAL";
    case AudioSystem::DEVICE_IN_VOICE_CALL:         return "DEVICE_IN_VOICE_CALL";
    case AudioSystem::DEVICE_IN_BACK_MIC:           return "DEVICE_IN_BACK_MIC";
    case AudioSystem::DEVICE_IN_FM_RADIO_RX:        return "DEVICE_IN_FM_RADIO_RX";
    case AudioSystem::DEVICE_IN_DEFAULT:            return "DEVICE_IN_DEFAULT";
    case AudioSystem::DEVICE_IN_ALL:                return "DEVICE_IN_ALL";
    default:                                        return "";
    }
}

static const char* state2str(AudioSystem::device_connection_state state)
{
    switch (state) {
    case AudioSystem::DEVICE_STATE_UNAVAILABLE: return "DEVICE_STATE_UNAVAILABLE";
    case AudioSystem::DEVICE_STATE_AVAILABLE:   return "DEVICE_STATE_AVAILABLE";
    default:                                    return "<INVALID>";
    }
}

static const char* mode2str(AudioSystem::audio_mode mode)
{
    switch (mode) {
    case AudioSystem::MODE_CURRENT:             return "MODE_CURRENT";
    case AudioSystem::MODE_NORMAL:              return "MODE_NORMAL";
    case AudioSystem::MODE_RINGTONE:            return "MODE_RINGTONE";
    case AudioSystem::MODE_IN_CALL:             return "MODE_IN_CALL";
    case AudioSystem::MODE_IN_COMMUNICATION:    return "MODE_IN_COMMUNICATION";
    case AudioSystem::MODE_INVALID:             return "MODE_INVALID";
    default:                                    return "<INVALID>";
    }
}

static const char* stream2str(AudioSystem::stream_type stream)
{
    switch (stream) {
    case AudioSystem::DEFAULT:                  return "DEFAULT";
    case AudioSystem::VOICE_CALL:               return "VOICE_CALL";
    case AudioSystem::SYSTEM:                   return "SYSTEM";
    case AudioSystem::RING:                     return "RING";
    case AudioSystem::MUSIC:                    return "MUSIC";
    case AudioSystem::ALARM:                    return "ALARM";
    case AudioSystem::NOTIFICATION:             return "NOTIFICATION";
    case AudioSystem::BLUETOOTH_SCO:            return "BLUETOOTH_SCO";
    case AudioSystem::ENFORCED_AUDIBLE:         return "ENFORCED_AUDIBLE";
    case AudioSystem::DTMF:                     return "DTMF";
    case AudioSystem::TTS:                      return "TTS";
    case AudioSystem::SPEECH_PROC:              return "SPEECH_PROC";
#ifdef STE_VIDEO_CALL
    case AudioSystem::VIDEO_CALL:               return "VIDEO_CALL";
#endif
    default:                                    return "<INVALID>";
    }
}

static const char* strategy2str(AudioPolicyManagerANM::routing_strategy strategy)
{
    switch (strategy) {
    case AudioPolicyManagerANM::STRATEGY_MEDIA: return "STRATEGY_MEDIA";
    case AudioPolicyManagerANM::STRATEGY_PHONE: return "STRATEGY_PHONE";
    case AudioPolicyManagerANM::STRATEGY_SONIFICATION: return "STRATEGY_SONIFICATION";
    case AudioPolicyManagerANM::STRATEGY_DTMF:  return "STRATEGY_DTMF";
    default:                                    return "<INVALID>";
    }
}

static const char* force_use2str(AudioSystem::force_use force_use)
{
    switch (force_use) {
    case AudioSystem::FOR_COMMUNICATION:        return "FOR_COMMUNICATION";
    case AudioSystem::FOR_MEDIA:                return "FOR_MEDIA";
    case AudioSystem::FOR_RECORD:               return "FOR_RECORD";
    case AudioSystem::FOR_DOCK:                 return "FOR_DOCK";
    default:                                    return "<INVALID>";
    }
}

static const char* forced_config2str(AudioSystem::forced_config forced_config)
{
    switch (forced_config) {
    case AudioSystem::FORCE_NONE:               return "FORCE_NONE";
    case AudioSystem::FORCE_SPEAKER:            return "FORCE_SPEAKER";
    case AudioSystem::FORCE_HEADPHONES:         return "FORCE_HEADPHONES";
    case AudioSystem::FORCE_BT_SCO:             return "FORCE_BT_SCO";
    case AudioSystem::FORCE_BT_A2DP:            return "FORCE_BT_A2DP";
    case AudioSystem::FORCE_WIRED_ACCESSORY:    return "FORCE_WIRED_ACCESSORY";
    case AudioSystem::FORCE_BT_CAR_DOCK:        return "FORCE_BT_CAR_DOCK";
    case AudioSystem::FORCE_BT_DESK_DOCK:       return "FORCE_BT_DESK_DOCK";
    default:                                    return "<INVALID>";
    }
}

static const char* source2str(int source)
{
    switch (source) {
    case AUDIO_SOURCE_DEFAULT:                  return "AUDIO_SOURCE_DEFAULT";
    case AUDIO_SOURCE_MIC:                      return "AUDIO_SOURCE_MIC";
    case AUDIO_SOURCE_VOICE_UPLINK:             return "AUDIO_SOURCE_VOICE_UPLINK";
    case AUDIO_SOURCE_VOICE_DOWNLINK:           return "AUDIO_SOURCE_VOICE_DOWNLINK";
    case AUDIO_SOURCE_VOICE_CALL:               return "AUDIO_SOURCE_VOICE_CALL";
    case AUDIO_SOURCE_VOICE_CALL_NO_RECORD:     return "AUDIO_SOURCE_VOICE_CALL_NO_RECORD";
    case AUDIO_SOURCE_CAMCORDER:                return "AUDIO_SOURCE_CAMCORDER";
    case AUDIO_SOURCE_VOICE_RECOGNITION:        return "AUDIO_SOURCE_VOICE_RECOGNITION";
    case AUDIO_SOURCE_FM_RADIO_RX:              return "AUDIO_SOURCE_FM_RADIO_RX";
    case AUDIO_SOURCE_SPEECH_PROC:              return "AUDIO_SOURCE_SPEECH_PROC";
#ifdef STE_VIDEO_CALL
    case AUDIO_SOURCE_VIDEO_CALL:               return "AUDIO_SOURCE_VIDEO_CALL";
#endif
    case AUDIO_SOURCE_VOICE_COMMUNICATION:      return "AUDIO_SOURCE_VOICE_COMMUNICATION";
    default:                                    return "<INVALID>";
    }
}

static const char* voiptype2str(int voiptype)
{
    switch (voiptype) {
    case AudioPolicyManagerANM::VOIP_INACTIVE:          return "VOIP_INACTIVE";
    case AudioPolicyManagerANM::VOIP_VIDEO_CALL:        return "VOIP_VIDEO_CALL";
    case AudioPolicyManagerANM::VOIP_VOICE_CALL:        return "VOIP_VOICE_CALL";
    case AudioPolicyManagerANM::VOIP_IN_COMMUNICATION:  return "VOIP_IN_COMMUNICATION";
    default:                                            return "<INVALID>";
    }
}

/* ---------------------------------------------------------------------------
 * Top level and actual device functions
 * ---------------------------------------------------------------------------*/
uint32_t AudioPolicyManagerANM::getTopLevelDeviceIndex(uint32_t device)
{
    /* Only 1 device is allowed as input parameter */
    assert(AudioSystem::popCount(device) == 1);

    switch (device) {
    /* Output devices */
    case AudioSystem::DEVICE_OUT_EARPIECE:                  return DEV_OUT_INDEX_EARPIECE;
    case AudioSystem::DEVICE_OUT_SPEAKER:                   return DEV_OUT_INDEX_SPEAKER;
    case AudioSystem::DEVICE_OUT_WIRED_HEADSET:             return DEV_OUT_INDEX_HEADSET;
    case AudioSystem::DEVICE_OUT_WIRED_HEADPHONE:           return DEV_OUT_INDEX_HEADPHONE;
    case AudioSystem::DEVICE_OUT_BLUETOOTH_A2DP:            return DEV_OUT_INDEX_A2DP;
    case AudioSystem::DEVICE_OUT_BLUETOOTH_A2DP_SPEAKER:    return DEV_OUT_INDEX_A2DP;
    case AudioSystem::DEVICE_OUT_BLUETOOTH_A2DP_HEADPHONES: return DEV_OUT_INDEX_A2DP;
    case AudioSystem::DEVICE_OUT_FM_RADIO_TX:               return DEV_OUT_INDEX_FMTX;
    case AudioSystem::DEVICE_OUT_AUX_DIGITAL:               return DEV_OUT_INDEX_HDMI;
    case AudioSystem::DEVICE_OUT_ANLG_DOCK_HEADSET:         return DEV_OUT_INDEX_USB;
    case AudioSystem::DEVICE_OUT_BLUETOOTH_SCO:
    case AudioSystem::DEVICE_OUT_BLUETOOTH_SCO_HEADSET:
    case AudioSystem::DEVICE_OUT_BLUETOOTH_SCO_CARKIT:
        if (isBtWbSupported()) {
            return DEV_OUT_INDEX_BTWB;
        } else {
            return DEV_OUT_INDEX_BT;
        }
    case AudioSystem::DEVICE_OUT_DEFAULT:
        if (mVoIPActive) {
            return DEV_OUT_INDEX_VOIP;
        } else {
            return DEV_OUT_INDEX_DEFAULT;
        }
    /* Input devices */
    case AudioSystem::DEVICE_IN_BUILTIN_MIC:    return DEV_IN_INDEX_MIC;
    case AudioSystem::DEVICE_IN_WIRED_HEADSET:  return DEV_IN_INDEX_HEADSET;
    case AudioSystem::DEVICE_IN_FM_RADIO_RX:    return DEV_IN_INDEX_FMRX;
    case AudioSystem::DEVICE_IN_VOICE_CALL:     return DEV_IN_INDEX_VOICE_CALL;
    case AudioSystem::DEVICE_IN_BLUETOOTH_SCO_HEADSET:
        if (isBtWbSupported()) {
            return DEV_IN_INDEX_BTWB;
        } else {
            return DEV_IN_INDEX_BT;
        }
    case AudioSystem::DEVICE_IN_DEFAULT:
        if (mVoIPActive) {
            return DEV_IN_INDEX_VOIP;
        } else {
            return DEV_IN_INDEX_DEFAULT;
        }
    default:
        ALOG_ERR("getTopLevelDeviceIndex() Unknown device! %d", device);
        return 0;
    }
}

void AudioPolicyManagerANM::getActualOutputDevice(uint32_t device, routing_strategy strategy, const char **actual)
{
    uint32_t index = 0;
    assert(actual);
    *actual = NULL;
    /* Only 1 device is allowed as input parameter */
    assert(AudioSystem::popCount(device) == 1);

    index = getTopLevelDeviceIndex(device);

    switch (strategy) {
    case STRATEGY_MEDIA:
        *actual = actualMediaOutputDevices[index];
         break;
    case STRATEGY_SONIFICATION:
        *actual = actualSonificationOutputDevices[index];
        break;
    case STRATEGY_PHONE:
    case STRATEGY_DTMF:
        if(!mVoIPActive)
            *actual = actualDefaultOutputDevices[index];
        else
            *actual = actualDefaultVoIPOutputDevices[index];
        break;
    default :
        ALOG_ERR("getActualOutputDevice() Unknown strategy! %d", strategy);
        break;
    }

    /* Special case for BT headsets supporting
     * noise reduction (NR) and echo cancelling (EC). */
    switch (index) {
    case DEV_OUT_INDEX_BTWB:
        if (isBtNrEcSupported()) {
            if(!mVoIPActive)
                *actual = DEVICE_ACTUAL_BTWBOUT_NO_NR_EC;
            else
                *actual = DEVICE_ACTUAL_BTWBOUT_NO_NR_EC_VOIP;
        }
        break;
    case DEV_OUT_INDEX_BT:
        if (isBtNrEcSupported()) {
            if(!mVoIPActive)
                *actual = DEVICE_ACTUAL_BTOUT_NO_NR_EC;
            else
                *actual = DEVICE_ACTUAL_BTOUT_NO_NR_EC_VOIP;
        }
        break;
    default:
        break;
    }

    ALOG_INFO_VERBOSE("getActualOutputDevice(%s, %s) returned actual out device %s",
        device2str((AudioSystem::audio_devices)device), strategy2str(strategy), *actual ? *actual : "NULL");
}

void AudioPolicyManagerANM::getActualInputDevice(uint32_t device, int source, const char **actual)
{
    uint32_t index = 0;
    assert(actual);
    *actual = NULL;
    /* Only 1 device is allowed as input parameter */
    assert(AudioSystem::popCount(device) == 1);

    index = getTopLevelDeviceIndex(device);

    switch (source) {
    case AUDIO_SOURCE_DEFAULT:
    case AUDIO_SOURCE_MIC:
    case AUDIO_SOURCE_VOICE_UPLINK:
    case AUDIO_SOURCE_VOICE_DOWNLINK:
    case AUDIO_SOURCE_VOICE_CALL:
    case AUDIO_SOURCE_SPEECH_PROC:
#ifdef STE_VIDEO_CALL
    case AUDIO_SOURCE_VIDEO_CALL:
#endif
    case AUDIO_SOURCE_VOICE_COMMUNICATION:
    case AUDIO_SOURCE_VOICE_RECOGNITION:
    case AUDIO_SOURCE_FM_RADIO_RX:
        *actual = actualDefaultInputDevices[index];
        break;
    case AUDIO_SOURCE_CAMCORDER:
        *actual = actualCamcorderInputDevices[index];
        break;
    case AUDIO_SOURCE_VOICE_CALL_NO_RECORD:
        if (device == AudioSystem::DEVICE_IN_BUILTIN_MIC) {
            uint32_t vcOutDev = getOutputDeviceForStrategy(NULL, STRATEGY_PHONE, AudioSystem::PCM_16_BIT);
            switch (vcOutDev) {
            case AudioSystem::DEVICE_OUT_EARPIECE:
                if(!mVoIPActive)
                    *actual = DEVICE_ACTUAL_MIC_EARPIECE;
                else
                    *actual = DEVICE_ACTUAL_MIC_EARPIECE_VOIP;
                break;
            case AudioSystem::DEVICE_OUT_SPEAKER:
                if(!mVoIPActive)
                    *actual = DEVICE_ACTUAL_MIC_SPEAKER;
                else
                    *actual = DEVICE_ACTUAL_MIC_SPEAKER_VOIP;
                break;
            case AudioSystem::DEVICE_OUT_WIRED_HEADPHONE:
                if(!mVoIPActive)
                    *actual = DEVICE_ACTUAL_MIC_HSOUT;
                else
                    *actual = DEVICE_ACTUAL_MIC_HSOUT_VOIP;
                break;
            default:
                if(!mVoIPActive)
                    *actual = actualDefaultInputDevices[index];
                else
                    *actual = actualDefaultVoIPInputDevices[index];
                break;
            }
        } else {
            if(!mVoIPActive)
                *actual = actualDefaultInputDevices[index];
            else
                *actual = actualDefaultVoIPInputDevices[index];
        }
        break;
    default :
        ALOG_ERR("getActualInputDevice() Unknown source! %d", source);
        break;
    }

    /* Special case for BT headsets supporting
     * noise reduction (NR) and echo cancelling (EC). */
    switch (index) {
    case DEV_IN_INDEX_BTWB:
        if (isBtNrEcSupported()) {
            if(!mVoIPActive)
                *actual = DEVICE_ACTUAL_BTWBIN_NO_NR_EC;
            else
                *actual = DEVICE_ACTUAL_BTWBIN_NO_NR_EC_VOIP;
        }
        break;
    case DEV_IN_INDEX_BT:
        if (isBtNrEcSupported()) {
            if(!mVoIPActive)
                *actual = DEVICE_ACTUAL_BTIN_NO_NR_EC;
            else
                *actual = DEVICE_ACTUAL_BTIN_NO_NR_EC_VOIP;
        }
        break;
    default:
        break;
    }

    ALOG_INFO_VERBOSE("getActualInputDevice(%s, %s) returned actual in device %s",
        device2str((AudioSystem::audio_devices)device), source2str(source), *actual ? *actual : "NULL");
}


void AudioPolicyManagerANM::updateTopLevelDeviceMap(const char *toplevel, const char *actual, const char *toplevel2, const char *actual2)
{
    assert(toplevel);
    assert(actual);

    if (!toplevel2) {
        ALOG_INFO_VERBOSE("updateTopLevelDeviceMap(): toplevel=%s, actual=%s", toplevel, actual);
    } else {
        assert(actual2);
        ALOG_INFO_VERBOSE("updateTopLevelDeviceMap(): toplevel=%s, actual=%s, toplevel2=%s, actual2=%s", toplevel, actual, toplevel2, actual2);
    }

    if (strcmp(actual, "") == 0) {
        ALOG_INFO_VERBOSE("updateTopLevelDeviceMap(): actual is empty! Do not update.");
        return;
    }

    if (actual2) {
        if (strcmp(actual2, "") == 0) {
            ALOG_INFO_VERBOSE("updateTopLevelDeviceMap(): actual2 is empty! Do not update.");
            return;
        }
    }

    ste_adm_set_toplevel_map_live(toplevel, actual, toplevel2, actual2);
}

bool AudioPolicyManagerANM::isBtWbSupported() {
    bool isBtWbSupported = false; /* Bluetooth Wideband (16kHz) support */
    String8 keyValuePairs = android::AudioSystem::getParameters(0, String8(PARAM_KEY_BTWB));
    AudioParameter param = AudioParameter(keyValuePairs);
    String8 value;
    if (param.get(String8(PARAM_KEY_BTWB), value) == NO_ERROR) {
        isBtWbSupported = (value == "on");
    }
    return isBtWbSupported;
}

bool AudioPolicyManagerANM::isBtNrEcSupported() {
    bool isBtNrEcSupported = false; /* Noise reduction (NR) and echo cancelling (EC) support */
    String8 keyValuePairs = android::AudioSystem::getParameters(0, String8(PARAM_KEY_BTNREC));
    AudioParameter param = AudioParameter(keyValuePairs);
    String8 value;
    if (param.get(String8(PARAM_KEY_BTNREC), value) == NO_ERROR) {
        isBtNrEcSupported = (value == "on");
    }
    return isBtNrEcSupported;
}

/* ---------------------------------------------------------------------------
 * Output Device Routing
 * ---------------------------------------------------------------------------*/
uint32_t AudioPolicyManagerANM::getPossibleOutputDevices(
             audio_io_handle_t output, uint32_t format) const
{
    uint32_t mask = 0;

    // Step 1: find devices that can handle the specified format
    if (isSupportedCodedFormat(format)) {
        mask = AudioSystem::DEVICE_OUT_AUX_DIGITAL;
    } else if (AudioSystem::isLinearPCM(format)) {
        mask = AudioSystem::DEVICE_OUT_ALL;
    }

    // Step 2: clear busy devices (that can not handle another connection
    // in the given format). Do not clear own device, if we already have one.
    for (size_t i = 0; i < mOutputs.size(); i++) {
        AudioOutputDescriptor *cur_descr = mOutputs.valueAt(i);
        audio_io_handle_t cur_output     = mOutputs.keyAt(i);
        if (output != cur_output && cur_descr->refCount() >  0) {

            if (!AudioSystem::isLinearPCM(cur_descr->mFormat)) {
                mask &= ~(cur_descr->mDevice);
            }

            if (AudioSystem::isLinearPCM(cur_descr->mFormat)) {
                if (!AudioSystem::isLinearPCM(format)) {
                    mask &= ~(cur_descr->mDevice);
                }
            }
        }
    }

    ALOG_INFO_VERBOSE("getPossibleOutputDevices(): format=%X --> mask=%X", format, mask);
    return mask;
}



uint32_t AudioPolicyManagerANM::getPreferredOutputDevice(
    audio_io_handle_t                       output,
    AudioPolicyManagerANM::routing_strategy strategy,
    uint32_t                                format)
{
    uint32_t device = 0;
    uint32_t *deviceList = NULL;

    switch (mPhoneState) {

    case AudioSystem::MODE_IN_CALL:
    case AudioSystem::MODE_IN_COMMUNICATION:

        switch (strategy) {

        case AudioPolicyManagerANM::STRATEGY_PHONE:
            deviceList = phoneInCallDevices;
            break;

        case AudioPolicyManagerANM::STRATEGY_SONIFICATION:
            deviceList = sonificationInCallDevices;
            break;

        case AudioPolicyManagerANM::STRATEGY_MEDIA:
            deviceList = mediaInCallDevices;
            break;

        case AudioPolicyManagerANM::STRATEGY_DTMF:
            deviceList = phoneInCallDevices;
            break;

        default:
            ALOG_ERR("getPreferredOutputDevice(): Invalid strategy %d", strategy);
            break;
        }

        break;

    case AudioSystem::MODE_RINGTONE:
        switch (strategy) {

        case AudioPolicyManagerANM::STRATEGY_PHONE:
            if(mVoIPActive)
                deviceList = phoneInCallDevices;
            else
                deviceList = phoneDevices;
            break;

        case AudioPolicyManagerANM::STRATEGY_SONIFICATION:
            deviceList = sonificationInCallDevices;
            break;

        case AudioPolicyManagerANM::STRATEGY_MEDIA:
            deviceList = mediaDevices;
            break;

        case AudioPolicyManagerANM::STRATEGY_DTMF:
            if(mVoIPActive)
                deviceList = phoneInCallDevices;
            else
                deviceList = phoneDevices;
            break;

        default:
            ALOG_ERR("getPreferredOutputDevice(): Invalid strategy %d", strategy);
            break;
        }

        break;

    case AudioSystem::MODE_NORMAL:
    default:

        switch (strategy) {

        case AudioPolicyManagerANM::STRATEGY_PHONE:
            if(mVoIPActive)
                deviceList = phoneInCallDevices;
            else
                deviceList = phoneDevices;
            break;

        case AudioPolicyManagerANM::STRATEGY_SONIFICATION:
            deviceList = sonificationDevices;
            break;

        case AudioPolicyManagerANM::STRATEGY_MEDIA:
            deviceList = mediaDevices;
            break;

        case AudioPolicyManagerANM::STRATEGY_DTMF:
            if(mVoIPActive)
                deviceList = phoneInCallDevices;
            else
                deviceList = phoneDevices;
            break;

        default:
            ALOG_ERR("getPreferredOutputDevice(): Invalid strategy %d", strategy);
            break;
        }

        break;
    }

    ALOG_INFO_VERBOSE("getPreferredOutputDevice(): Preferred devices for strategy %s:",
        strategy2str(strategy));
    for (int i = 0; i < DEVLIST_MAX_NO_DEVICES; i++){
        if (deviceList[i] == 0) break;
        ALOG_INFO_VERBOSE("\t%s %x",
            device2str((AudioSystem::audio_devices)deviceList[i]),
            deviceList[i]);
    }

    uint32_t possible = getPossibleOutputDevices(output, format);
    for (int i = 0; i < DEVLIST_MAX_NO_DEVICES; i++){
        device = mAvailableOutputDevices & deviceList[i] & possible;
        if (device != 0) {
            if (AudioSystem::popCount(device) ==
                AudioSystem::popCount(deviceList[i])) {
                /* Preferred device is found */
                break;
            }
        }

        if (deviceList[i] == 0) {
            ALOG_ERR("getPreferredOutputDevice(): No preferred device is available!");
            break;
        }
        device = 0;
    }

    return device;
}

uint32_t AudioPolicyManagerANM::getForcedOutputDevice(
    AudioPolicyManagerANM::routing_strategy strategy)
{
    uint32_t device = 0;
    bool forcedDevAvailable = true;

    switch (mPhoneState) {

    /* MODE_IN_CALL, MODE_IN_COMMUNICATION */
    case AudioSystem::MODE_IN_CALL:
    case AudioSystem::MODE_IN_COMMUNICATION:

        switch (strategy) {
        case AudioPolicyManagerANM::STRATEGY_PHONE:
        case AudioPolicyManagerANM::STRATEGY_DTMF:

            switch (mForceUse[AudioSystem::FOR_COMMUNICATION]) {
            case AudioSystem::FORCE_BT_SCO:
                device = mAvailableOutputDevices & AudioSystem::DEVICE_OUT_BLUETOOTH_SCO_CARKIT;
                if (device) break;
                device = mAvailableOutputDevices & AudioSystem::DEVICE_OUT_BLUETOOTH_SCO_HEADSET;
                if (device) break;
                device = mAvailableOutputDevices & AudioSystem::DEVICE_OUT_BLUETOOTH_SCO;
                if (device) break;
                forcedDevAvailable = false;
                break;
            case AudioSystem::FORCE_WIRED_ACCESSORY:
                device = mAvailableOutputDevices & AudioSystem::DEVICE_OUT_WIRED_HEADSET;
                if (device) break;
                device = mAvailableOutputDevices & AudioSystem::DEVICE_OUT_WIRED_HEADPHONE;
                if (device) break;
                forcedDevAvailable = false;
                break;
            case AudioSystem::FORCE_SPEAKER:
                device = mAvailableOutputDevices & AudioSystem::DEVICE_OUT_SPEAKER;
                if (device) break;
                forcedDevAvailable = false;
                break;
            default:
                break;
            }
            break;

        case AudioPolicyManagerANM::STRATEGY_MEDIA:

            switch (mForceUse[AudioSystem::FOR_MEDIA]) {
            case AudioSystem::FORCE_BT_SCO:
                device = mAvailableOutputDevices & AudioSystem::DEVICE_OUT_BLUETOOTH_SCO_CARKIT;
                if (device) break;
                device = mAvailableOutputDevices & AudioSystem::DEVICE_OUT_BLUETOOTH_SCO_HEADSET;
                if (device) break;
                device = mAvailableOutputDevices & AudioSystem::DEVICE_OUT_BLUETOOTH_SCO;
                if (device) break;
                forcedDevAvailable = false;
                break;
            case AudioSystem::FORCE_SPEAKER:
                device = mAvailableOutputDevices & AudioSystem::DEVICE_OUT_SPEAKER;
                if (device) break;
                forcedDevAvailable = false;
                break;
            default:
                break;
            }

            if(device)break;

            switch (mForceUse[AudioSystem::FOR_COMMUNICATION]) {
            case AudioSystem::FORCE_BT_SCO:
                device = mAvailableOutputDevices & AudioSystem::DEVICE_OUT_BLUETOOTH_SCO_CARKIT;
                if (device) break;
                device = mAvailableOutputDevices & AudioSystem::DEVICE_OUT_BLUETOOTH_SCO_HEADSET;
                if (device) break;
                device = mAvailableOutputDevices & AudioSystem::DEVICE_OUT_BLUETOOTH_SCO;
                if (device) break;
                forcedDevAvailable = false;
                break;
            case AudioSystem::FORCE_WIRED_ACCESSORY:
                device = mAvailableOutputDevices & AudioSystem::DEVICE_OUT_WIRED_HEADSET;
                if (device) break;
                device = mAvailableOutputDevices & AudioSystem::DEVICE_OUT_WIRED_HEADPHONE;
                if (device) break;
                forcedDevAvailable = false;
                break;
            case AudioSystem::FORCE_SPEAKER:
                device = mAvailableOutputDevices & AudioSystem::DEVICE_OUT_SPEAKER;
                if (device) break;
                forcedDevAvailable = false;
                break;
            default:
                break;
            }
            break;

        case AudioPolicyManagerANM::STRATEGY_SONIFICATION:
        default:

            switch (mForceUse[AudioSystem::FOR_COMMUNICATION]) {
            case AudioSystem::FORCE_BT_SCO:
                device = mAvailableOutputDevices & AudioSystem::DEVICE_OUT_BLUETOOTH_SCO_CARKIT;
                if (device) break;
                device = mAvailableOutputDevices & AudioSystem::DEVICE_OUT_BLUETOOTH_SCO_HEADSET;
                if (device) break;
                device = mAvailableOutputDevices & AudioSystem::DEVICE_OUT_BLUETOOTH_SCO;
                if (device) break;
                forcedDevAvailable = false;
                break;
            case AudioSystem::FORCE_WIRED_ACCESSORY:
                device = mAvailableOutputDevices & AudioSystem::DEVICE_OUT_WIRED_HEADSET;
                if (device) break;
                device = mAvailableOutputDevices & AudioSystem::DEVICE_OUT_WIRED_HEADPHONE;
                if (device) break;
                forcedDevAvailable = false;
                break;
            default:
                break;
            }

            if (device) {
                /* STRATEGY_SONIFICATIION should always be played on Speaker */
                device = device | (mAvailableOutputDevices & AudioSystem::DEVICE_OUT_SPEAKER);
            }

            break;

        }
        break;

    /* MODE_NORMAL, MODE_RINTONE */
    case AudioSystem::MODE_RINGTONE:
    case AudioSystem::MODE_NORMAL:
    default:
        switch (strategy) {
        case AudioPolicyManagerANM::STRATEGY_PHONE:

            switch (mForceUse[AudioSystem::FOR_COMMUNICATION]) {
            case AudioSystem::FORCE_BT_SCO:
                device = mAvailableOutputDevices & AudioSystem::DEVICE_OUT_BLUETOOTH_SCO_CARKIT;
                if (device) break;
                device = mAvailableOutputDevices & AudioSystem::DEVICE_OUT_BLUETOOTH_SCO_HEADSET;
                if (device) break;
                device = mAvailableOutputDevices & AudioSystem::DEVICE_OUT_BLUETOOTH_SCO;
                if (device) break;
                forcedDevAvailable = false;
                break;
            case AudioSystem::FORCE_WIRED_ACCESSORY:
                device = mAvailableOutputDevices & AudioSystem::DEVICE_OUT_WIRED_HEADSET;
                if (device) break;
                device = mAvailableOutputDevices & AudioSystem::DEVICE_OUT_WIRED_HEADPHONE;
                if (device) break;
                forcedDevAvailable = false;
                break;
            case AudioSystem::FORCE_SPEAKER:
                device = mAvailableOutputDevices & AudioSystem::DEVICE_OUT_SPEAKER;
                if (device) break;
                forcedDevAvailable = false;
                break;
            default:
                break;
            }
            break;

        case AudioPolicyManagerANM::STRATEGY_MEDIA:
        case AudioPolicyManagerANM::STRATEGY_DTMF:

            switch (mForceUse[AudioSystem::FOR_MEDIA]) {
            case AudioSystem::FORCE_SPEAKER:
#ifdef STD_A2DP_MNGT
                // when not in a phone call, phone strategy should route STREAM_VOICE_CALL to
                // A2DP speaker when forcing to speaker output
                if (!isInCall()) {
                    device = mAvailableOutputDevices & AudioSystem::DEVICE_OUT_BLUETOOTH_A2DP_SPEAKER;
                    if (device) break;
                }
#endif
                device = mAvailableOutputDevices & AudioSystem::DEVICE_OUT_SPEAKER;
                if (device) break;
                forcedDevAvailable = false;
                break;
            default:
                break;
            }
            break;

        case AudioPolicyManagerANM::STRATEGY_SONIFICATION:
        default:
            break;
        }
        break;
    }

    if (device) {
        ALOG_INFO_VERBOSE("getForcedOutputDevice(): Forced device for strategy %s is %x %s",
            strategy2str(strategy), device,
            device2str((AudioSystem::audio_devices)device));
    } else {
        if (!forcedDevAvailable) {
            ALOG_WARN("getForcedOuputDevice(): No forced device available!");
        }
        ALOG_INFO_VERBOSE("getForcedOutputDevice(): No forced device for strategy %s",
            strategy2str(strategy));
    }
    return device;
}

uint32_t AudioPolicyManagerANM::getOutputDeviceForStrategy(
    audio_io_handle_t                       output,
    AudioPolicyManagerANM::routing_strategy strategy,
    uint32_t                                format
#ifdef STD_A2DP_MNGT
    , bool                                  fromCache
#endif
    ) {

    uint32_t device = 0;

#ifdef STD_A2DP_MNGT
    if (fromCache && AudioSystem::isLinearPCM(format)) {
        ALOG_INFO("getOutputDeviceForStrategy() from cache strategy %s, device %s (%x)",
                strategy2str(strategy),
                device2str((AudioSystem::audio_devices)mDeviceForStrategy[strategy]),
                mDeviceForStrategy[strategy]);
        return mDeviceForStrategy[strategy];
    }
#endif

    /* First check if device usage is forced for this strategy */
    device = getForcedOutputDevice(strategy);
    if (device) {
        ALOG_INFO_VERBOSE("getOutputDeviceForStrategy(): Forced device: %x %s", device,
            device2str((AudioSystem::audio_devices)device));
    } else {
        /* Device usage is not forced, get the device from device table */
        device = getPreferredOutputDevice(output, strategy, format);
        ALOG_INFO_VERBOSE("getOutputDeviceForStrategy(): Preferred device: %x %s", device,
            device2str((AudioSystem::audio_devices)device));
    }

    return device;
}

AudioPolicyManagerANM::routing_strategy
    AudioPolicyManagerANM::getOutputStrategy(audio_io_handle_t output)
{
    AudioPolicyManagerANM::routing_strategy strategy =
        AudioPolicyManagerANM::STRATEGY_PHONE;
    AudioOutputDescriptor *descr = mOutputs.valueFor(output);
    if (descr) {
        strategy = descr->mStrategy;
    } else {
        ALOG_ERR("getOutputStrategy(): No output descriptor for output %d!", output);
    }
    return strategy;
}

uint32_t AudioPolicyManagerANM::getOutputDevice(audio_io_handle_t output
#ifdef STD_A2DP_MNGT
    , bool fromCache
#endif
) {

    uint32_t device = 0;
    AudioOutputDescriptor *descr = mOutputs.valueFor(output);
    if (!descr) {
        ALOG_ERR("getOutputDevice(): No output descriptor for output %d!", output);
    } else {
        if (descr->mVoIPActive) {
            /* Set device as VoIP device */
            device = AudioSystem::DEVICE_OUT_DEFAULT;
        } else {
            /* Get the stream type used by this output */
            AudioPolicyManagerANM::routing_strategy strategy =
                getOutputStrategy(output);
            /* Get output device for strategy */
            device = getOutputDeviceForStrategy(output, strategy, descr->mFormat
#ifdef STD_A2DP_MNGT
            , fromCache
#endif
            );
        }
    }

    return device;
}

void AudioPolicyManagerANM::updateOutputConfig(AudioOutputDescriptor *descr)
{
    ALOG_INFO_VERBOSE("ENTER updateOutputConfig(): device=%s", device2str((AudioSystem::audio_devices)descr->mDevice));

    /* Check if this output is active */
    if (descr->refCount()) {

        /* Loop through the out devices (the first 16 bits in audio_devices) */
        for (size_t i=0x1 ; i<= 0x8000 ; i=i<<1 ) {
            if (descr->mDevice & i) {
                uint32_t indexTopLevel = getTopLevelDeviceIndex(i);
                if (strategyPriority[descr->mStrategy] > strategyPriority[mCurrentStrategy[indexTopLevel]]) {
                    /* Update current strategy for device */
                    mCurrentStrategy[indexTopLevel] = descr->mStrategy;
                    /* Update top level device map for device */
                    const char *devTopLevel = topLevelOutputDevices[indexTopLevel];
                    const char *devActual = NULL;
                    getActualOutputDevice(i, descr->mStrategy, &devActual);
                    updateTopLevelDeviceMap(devTopLevel,devActual,NULL, NULL);
                }
            }
        }
    }

    ALOG_INFO_VERBOSE("LEAVE updateOutputConfig(): device=%s", device2str((AudioSystem::audio_devices)descr->mDevice));
}

void AudioPolicyManagerANM::updateOutputRouting(
#ifdef STD_A2DP_MNGT
    bool fromCache
#endif
)
{
    ALOG_INFO_VERBOSE("ENTER updateOutputRouting(): %d outputs", mOutputs.size());

    for (size_t i = 0; i < mOutputs.size(); i++) {

        /* Get next output */
        audio_io_handle_t output = mOutputs.keyAt(i);
        /* Get output descriptor for the output */
        AudioOutputDescriptor *descr = mOutputs.valueFor(output);

        /* Get output device, based on current conditions */
        uint32_t device = getOutputDevice(output
#ifdef STD_A2DP_MNGT
            , fromCache
#endif
            );
        if (!device) {
            ALOG_ERR("LEAVE updateOutputRouting(): No selected device for output %d!", output);
            return;
        }

        /* Check constraints for using device in current scenario */
        checkHwResources(&device);

#ifdef STD_A2DP_MNGT
        if (!fromCache) {
            checkOutputForAllStrategies();
            checkA2dpSuspend();
            updateDeviceForStrategy();
        }

        if (!(descr->isDuplicated()))
#endif
            doOutputRouting(output, device);
    }

    ALOG_INFO_VERBOSE("LEAVE updateOutputRouting()");
}

#ifdef STD_A2DP_MNGT
bool AudioPolicyManagerANM::isInCall()
{
    return (mPhoneState == AudioSystem::MODE_IN_CALL ||
            mPhoneState == AudioSystem::MODE_IN_COMMUNICATION || mVoIPActive);
}

void AudioPolicyManagerANM::updateDeviceForStrategy()
{
    ALOG_INFO("updateDeviceForStrategy()");
    for (int i = 0; i < NUM_STRATEGIES; i++) {
        /*if (i != AudioPolicyManagerANM::STRATEGY_SPEECH_PROC)*/
        mDeviceForStrategy[i] = getOutputDeviceForStrategy(NULL, (routing_strategy)i,
                                                        AudioSystem::PCM_16_BIT, false);
    }
}

status_t AudioPolicyManagerANM::handleA2dpConnection(AudioSystem::audio_devices device,
                                                 const char *device_address)
{
    // when an A2DP device is connected, open an A2DP and a duplicated output
    ALOG_INFO("handleA2dpConnection() opening A2DP output for device %s", device_address);
    AudioOutputDescriptor *outputDesc = new AudioOutputDescriptor();
    outputDesc->mDevice = device;
    outputDesc->mStrategyRefCount = 1;

    /* To Be Removed : Force the strategy to use with a2dp. */
    outputDesc->mStrategy = AudioPolicyManagerANM::STRATEGY_MEDIA;

    mA2dpOutput = mpClientInterface->openOutput(&outputDesc->mDevice,
                                            &outputDesc->mSamplingRate,
                                            &outputDesc->mFormat,
                                            &outputDesc->mChannels,
                                            &outputDesc->mLatency,
                                            outputDesc->mFlags);
    if (mA2dpOutput) {
        // add A2DP output descriptor
        ALOG_INFO("handleA2dpConnection() mA2dpOutput = %d, mStrategyRefCount = %d",
                mA2dpOutput,outputDesc->mStrategyRefCount);
        mOutputs.add(mA2dpOutput, outputDesc);

        //TODO: configure audio effect output stage here

        // If both A2DP and duplicated outputs are open, send device address to A2DP hardware
        // interface
        AudioParameter param;
        param.add(String8("a2dp_sink_address"), String8(device_address));
        mpClientInterface->setParameters(mA2dpOutput, param.toString());
        mA2dpDeviceAddress = String8(device_address, MAX_DEVICE_ADDRESS_LEN);
    } else {
        ALOG_WARN("handleA2dpConnection() could not open A2DP output for device %x", device);
        delete outputDesc;
        return NO_INIT;
    }

    mA2dpSuspended = false;

    if(a2dpUsedForSonification() && (mDuplicatedOutput == 0)) {
        audio_io_handle_t output = 0;
        AudioOutputDescriptor *descr = NULL;
        int strategy = AudioPolicyManagerANM::STRATEGY_SONIFICATION;

        for (size_t i = 0; i < mOutputs.size(); i++) {
            descr = mOutputs.valueAt(i);
            if (descr->mStrategyRefCount>=1 && descr->mStrategy==strategy) {
                descr->mStrategyRefCount++;
                output = mOutputs.keyAt(i);
                ALOG_INFO("handleA2dpConnection() Reused existing output %d for strategy %s, mStrategyRefCount = %d",
                    output, strategy2str((routing_strategy)strategy),descr->mStrategyRefCount);
                break;
            }
        }

        mDuplicatedOutput = mpClientInterface->openDuplicateOutput(mA2dpOutput, output);
        // add duplicated output descriptor
        AudioOutputDescriptor *dupOutputDesc = new AudioOutputDescriptor();
        AudioOutputDescriptor *outputDesc = mOutputs.valueFor(mA2dpOutput);
        dupOutputDesc->mOutput1 = mOutputs.valueFor(output);
        dupOutputDesc->mId1 = output;
        dupOutputDesc->mOutput2 = mOutputs.valueFor(mA2dpOutput);

        ALOG_INFO("created duplicated output %d for strategy %s with a2dpOutput %d and hwOutput %d", \
            mDuplicatedOutput, strategy2str( (routing_strategy) strategy), mDuplicatedOutput, mA2dpOutput, output);

        dupOutputDesc->mSamplingRate = outputDesc->mSamplingRate;
        dupOutputDesc->mFormat = outputDesc->mFormat;
        dupOutputDesc->mChannels = outputDesc->mChannels;
        dupOutputDesc->mLatency = outputDesc->mLatency;
        dupOutputDesc->mDevice = device;
        dupOutputDesc->mStrategy = outputDesc->mStrategy;
        dupOutputDesc->mStrategyRefCount = 1;

        mOutputs.add(mDuplicatedOutput, dupOutputDesc);
    }

    return NO_ERROR;
}

status_t AudioPolicyManagerANM::handleA2dpDisconnection(AudioSystem::audio_devices device,
                                                    const char *device_address)
{
    ALOG_INFO("handleA2dpDisconnection() disconnecting  output");

    if (mA2dpOutput == 0) {
        ALOG_WARN("handleA2dpDisconnection() disconnecting A2DP and no A2DP output!");
        return INVALID_OPERATION;
    }

    if (mA2dpDeviceAddress != device_address) {
        ALOG_WARN("handleA2dpDisconnection() disconnecting unknow A2DP sink address %s",
                device_address);
        return INVALID_OPERATION;
    }

    mA2dpDeviceAddress = "";
    mA2dpSuspended = false;
    return NO_ERROR;
}

void AudioPolicyManagerANM::closeA2dpOutputs()
{
    if (mDuplicatedOutput != 0) {
        ALOG_INFO("closeA2dpOutputs() closing duplicated output (%d)", mDuplicatedOutput);

        AudioOutputDescriptor *dupOutputDesc = mOutputs.valueFor(mDuplicatedOutput);
        AudioOutputDescriptor *hwOutputDesc = dupOutputDesc->mOutput1;
        // As all active tracks on duplicated output will be deleted,
        // and as they were also referenced on hardware output, the reference
        // count for their stream type must be adjusted accordingly on
        // hardware output.
        for (int i = 0; i < (int)AudioSystem::NUM_STREAM_TYPES; i++) {
            int refCount = dupOutputDesc->mRefCount[i];
            hwOutputDesc->changeRefCount((AudioSystem::stream_type)i,-refCount);
        }

        mpClientInterface->closeOutput(mDuplicatedOutput);
        delete mOutputs.valueFor(mDuplicatedOutput);
        mOutputs.removeItem(mDuplicatedOutput);
        mDuplicatedOutput = 0;
    }
    if (mA2dpOutput != 0) {
        ALOG_INFO("closeA2dpOutputs() closing A2DP (%d)", mA2dpOutput);

        AudioParameter param;
        param.add(String8("closing"), String8("true"));
        mpClientInterface->setParameters(mA2dpOutput, param.toString());

        mpClientInterface->closeOutput(mA2dpOutput);
        delete mOutputs.valueFor(mA2dpOutput);
        mOutputs.removeItem(mA2dpOutput);
        mA2dpOutput = 0;
    }
}

void AudioPolicyManagerANM::checkOutputForStrategy(routing_strategy strategy)
{
    uint32_t prevDevice = getOutputDeviceForStrategy(NULL, strategy, AudioSystem::PCM_16_BIT, true);
    uint32_t curDevice = getOutputDeviceForStrategy(NULL, strategy, AudioSystem::PCM_16_BIT);
    bool a2dpWasUsed = AudioSystem::isA2dpDevice((AudioSystem::audio_devices)(prevDevice & ~AudioSystem::DEVICE_OUT_SPEAKER));
    bool a2dpIsUsed = AudioSystem::isA2dpDevice((AudioSystem::audio_devices)(curDevice & ~AudioSystem::DEVICE_OUT_SPEAKER));
    audio_io_handle_t srcOutput = 0;
    audio_io_handle_t dstOutput = 0;
    audio_io_handle_t output = 0;
    AudioOutputDescriptor *descr = NULL;

    // Reuse output if already opened for this strategy
    for (size_t i = 0; i < mOutputs.size(); i++) {
        descr = mOutputs.valueAt(i);
        if (descr->mStrategyRefCount>=1 && descr->mStrategy==strategy) {
            output = mOutputs.keyAt(i);
            break;
        }
    }

    if (a2dpWasUsed && !a2dpIsUsed) {
        bool dupUsed = a2dpUsedForSonification() && a2dpWasUsed && (AudioSystem::popCount(prevDevice) == 2);
        dstOutput = output;
        if (dupUsed) {
            ALOG_INFO("checkOutputForStrategy() moving strategy %d from duplicated", strategy);
            srcOutput = mDuplicatedOutput;
        } else {
            ALOG_INFO("checkOutputForStrategy() moving strategy %d from a2dp", strategy);
            srcOutput = mA2dpOutput;
        }
    }
    if (a2dpIsUsed && !a2dpWasUsed) {
        bool dupUsed = a2dpUsedForSonification() && a2dpIsUsed && (AudioSystem::popCount(curDevice) == 2);
        srcOutput = output;
        if (dupUsed) {
            ALOG_INFO("checkOutputForStrategy() moving strategy %d to duplicated", strategy);
            dstOutput = mDuplicatedOutput;
        } else {
            ALOG_INFO("checkOutputForStrategy() moving strategy %d to a2dp", strategy);
            dstOutput = mA2dpOutput;
        }
    }

    if (srcOutput != 0 && dstOutput != 0) {
        // Move effects associated to this strategy from previous output to new output
        for (size_t i = 0; i < mEffects.size(); i++) {
            EffectDescriptor *desc = mEffects.valueAt(i);
            if (desc->mSession != AudioSystem::SESSION_OUTPUT_STAGE &&
                    desc->mStrategy == strategy &&
                    desc->mIo == srcOutput) {
                ALOG_INFO("checkOutputForStrategy() moving effect %d to output %d",
                        mEffects.keyAt(i), dstOutput);
                mpClientInterface->moveEffects(desc->mSession, srcOutput, dstOutput);
                desc->mIo = dstOutput;
            }
        }
        // Move tracks associated to this strategy from previous output to new output
        for (int i = 0; i < (int)AudioSystem::NUM_STREAM_TYPES; i++) {
            if (getStrategy((AudioSystem::stream_type)i) == strategy) {
                ALOG_INFO("checkOutputForStrategy() moving %d to output %d", srcOutput, dstOutput);
                mpClientInterface->setStreamOutput((AudioSystem::stream_type)i, dstOutput);
            }
        }
    }
}

void AudioPolicyManagerANM::checkOutputForAllStrategies()
{
    checkOutputForStrategy(STRATEGY_PHONE);
    checkOutputForStrategy(STRATEGY_SONIFICATION);
    checkOutputForStrategy(STRATEGY_MEDIA);
    checkOutputForStrategy(STRATEGY_DTMF);
}

void AudioPolicyManagerANM::checkA2dpSuspend()
{
    // suspend A2DP output if:
    //      (NOT already suspended) &&
    //      ((SCO device is connected &&
    //       (forced usage for communication || for record is SCO))) ||
    //      (phone state is ringing || in call)
    //
    // restore A2DP output if:
    //      (Already suspended) &&
    //      ((SCO device is NOT connected ||
    //       (forced usage NOT for communication && NOT for record is SCO))) &&
    //      (phone state is NOT ringing && NOT in call)
    //
    if (mA2dpOutput == 0) {
        return;
    }

    if (mA2dpSuspended) {
        if ((/*(mScoDeviceAddress == "") ||*/
             ((mForceUse[AudioSystem::FOR_COMMUNICATION] != AudioSystem::FORCE_BT_SCO) &&
              (mForceUse[AudioSystem::FOR_RECORD] != AudioSystem::FORCE_BT_SCO))) &&
             ((mPhoneState != AudioSystem::MODE_IN_CALL) &&
              (mPhoneState != AudioSystem::MODE_RINGTONE))) {

            ALOG_INFO("checkA2dpSuspend() restoreOutput %d", mA2dpOutput);
            mpClientInterface->restoreOutput(mA2dpOutput);
            mA2dpSuspended = false;
        }
    } else {
        if ((/*(mScoDeviceAddress != "") &&*/
             ((mForceUse[AudioSystem::FOR_COMMUNICATION] == AudioSystem::FORCE_BT_SCO) ||
              (mForceUse[AudioSystem::FOR_RECORD] == AudioSystem::FORCE_BT_SCO))) ||
             ((mPhoneState == AudioSystem::MODE_IN_CALL) ||
              (mPhoneState == AudioSystem::MODE_RINGTONE))) {

            ALOG_INFO("checkA2dpSuspend() suspendOutput %d", mA2dpOutput);
            mpClientInterface->suspendOutput(mA2dpOutput);
            mA2dpSuspended = true;
        }
    }
}


#endif
void AudioPolicyManagerANM::doOutputRouting(audio_io_handle_t output, uint32_t device)
{
        ALOG_INFO("ENTER doOutputRouting() for %d on device %x", output, device);

        /* Get output descriptor for the output */
        AudioOutputDescriptor *descr = mOutputs.valueFor(output);
        if (descr) {
            bool updateRouting = true;
            if (device == (uint32_t)descr->mDevice) {
                /* Preferred device is the same - do not update routing
                 * but update of output config must be checked */
                ALOG_INFO_VERBOSE("doOutputRouting(): Preferred device is the same for output %d (device = %s)",
                    output, device2str((AudioSystem::audio_devices)device));
                updateRouting = false;
            }

#ifdef STD_A2DP_MNGT
            // filter devices according to output selected
            if (output == mA2dpOutput) {
             device &= AudioSystem::DEVICE_OUT_ALL_A2DP;
            } else {
            device &= ~AudioSystem::DEVICE_OUT_ALL_A2DP;
            }

            if (device == 0) {
                ALOG_INFO_VERBOSE("updateOutputRouting(): no device for output %d (device = 0)",
                    output);
                updateRouting = false;
            }
#endif

            /* Update the output descriptor with the preferred device,
             * and then check if device config needs to be updated */
            uint32_t oldDevice = descr->mDevice;
            descr->mDevice = device;
            updateOutputConfig(descr);

            if (updateRouting) {
                /* Preferred device has changed - update routing */
                ALOG_INFO("doOutputRouting(): Preferred device for output %d has changed from %s to %s",
                    output,
                    device2str((AudioSystem::audio_devices)oldDevice),
                    device2str((AudioSystem::audio_devices)device));
                AudioParameter param = AudioParameter();
                param.addInt(String8(AudioParameter::keyRouting), (int)device);
                mpClientInterface->setParameters(output, param.toString(), 0);

                /* Update sink latency since routing has changed and another sink
                   may be used (e.g. swith from headset to A2DP device) */
                AudioParameter latencyParam = AudioParameter();
                latencyParam.addInt(String8("ste_sink_latency"), output);
                mpClientInterface->setParameters(output, latencyParam.toString(), 0);
            }
        } else {
            ALOG_ERR("doOutputRouting(): No output descriptor for output %d!", output);
        }

        ALOG_INFO_VERBOSE("LEAVE doOutputRouting()");
}

/* ---------------------------------------------------------------------------
 * Input Device Routing
 * ---------------------------------------------------------------------------*/
uint32_t AudioPolicyManagerANM::getPreferredInputDevice(
    int source, int channels)
{
    uint32_t device = 0;
    uint32_t *deviceList = NULL;

    switch (mPhoneState) {
    case AudioSystem::MODE_IN_CALL:
    case AudioSystem::MODE_IN_COMMUNICATION:

        switch (source) {
        case AUDIO_SOURCE_DEFAULT:
        case AUDIO_SOURCE_MIC:
        case AUDIO_SOURCE_VOICE_CALL_NO_RECORD:
        case AUDIO_SOURCE_CAMCORDER:
        case AUDIO_SOURCE_VOICE_RECOGNITION:
        case AUDIO_SOURCE_SPEECH_PROC:
#ifdef STE_VIDEO_CALL
        case AUDIO_SOURCE_VIDEO_CALL:
#endif
        case AUDIO_SOURCE_VOICE_COMMUNICATION:
            deviceList = phoneInputDevices;
            break;
        case AUDIO_SOURCE_VOICE_CALL:
        case AUDIO_SOURCE_VOICE_UPLINK:
        case AUDIO_SOURCE_VOICE_DOWNLINK:
            deviceList = phoneInCallRecordDevices;
            break;
        case AUDIO_SOURCE_FM_RADIO_RX:
            deviceList = fmInputDevices;
            break;
        default:
            ALOG_ERR("getPreferredInputDevice(): No preferred device list, input source = %d", source);
            deviceList = NULL;
            break;
        }
        break;

    case AudioSystem::MODE_RINGTONE:
    case AudioSystem::MODE_NORMAL:
    default:

        switch (source) {
        case AUDIO_SOURCE_VOICE_CALL_NO_RECORD:
        case AUDIO_SOURCE_SPEECH_PROC:
#ifdef STE_VIDEO_CALL
        case AUDIO_SOURCE_VIDEO_CALL:
#endif
        case AUDIO_SOURCE_VOICE_COMMUNICATION:
            deviceList = phoneInputDevices;
            break;
        case AUDIO_SOURCE_CAMCORDER:
            deviceList = camcorderInputDevices;
            break;
        case AUDIO_SOURCE_VOICE_CALL:
        case AUDIO_SOURCE_VOICE_UPLINK:
        case AUDIO_SOURCE_VOICE_DOWNLINK:
            deviceList = phoneInCallRecordDevices;
            break;
        case AUDIO_SOURCE_DEFAULT:
        case AUDIO_SOURCE_VOICE_RECOGNITION:
        case AUDIO_SOURCE_MIC:
            if (channels == 2) {
                deviceList = micStereoInputDevices;
            } else {
                deviceList = micMonoInputDevices;
            }
            break;
        case AUDIO_SOURCE_FM_RADIO_RX:
            deviceList = fmInputDevices;
            break;
        default:
            ALOG_ERR("getPreferredInputDevice(): No preferred device list, input source = %d", source);
            deviceList = NULL;
            break;
        }
        break;
    }

    ALOG_INFO_VERBOSE("getPreferredInputDevice(): Preferred devices for source %s:", source2str(source));
    for (int i = 0; i < DEVLIST_MAX_NO_DEVICES; i++){
        if (deviceList[i] == 0) break;
        ALOG_INFO_VERBOSE("\t%s %x",
            device2str((AudioSystem::audio_devices)deviceList[i]),
            deviceList[i]);
    }


    for (int i = 0; i < DEVLIST_MAX_NO_DEVICES; i++){

        device = mAvailableInputDevices & deviceList[i];
        if (device != 0) {
            if (AudioSystem::popCount(device) ==
                AudioSystem::popCount(deviceList[i])) {
                /* Preferred device is found */
                break;
            }
        }

        if (deviceList[i] == 0) {
            ALOG_ERR("getPreferredInputDevice(): No preferred device is available!");
            break;
        }
        device = 0;
    }

    return device;
}

uint32_t AudioPolicyManagerANM::getForcedInputDevice(int source)
{
    uint32_t device = 0;
    bool forcedDevAvailable = true;

    switch (source) {

    /* Audio input source for forced usage FOR_COMMUNICATION */
    case AUDIO_SOURCE_VOICE_CALL_NO_RECORD:
    case AUDIO_SOURCE_DEFAULT:
    case AUDIO_SOURCE_MIC:
    case AUDIO_SOURCE_VOICE_RECOGNITION:
    case AUDIO_SOURCE_CAMCORDER:
    case AUDIO_SOURCE_SPEECH_PROC:

#ifdef STE_VIDEO_CALL
    case AUDIO_SOURCE_VIDEO_CALL:
#endif
    case AUDIO_SOURCE_VOICE_COMMUNICATION:
        switch (mForceUse[AudioSystem::FOR_COMMUNICATION]) {
        case AudioSystem::FORCE_BT_SCO:
            device = mAvailableInputDevices & AudioSystem::DEVICE_IN_BLUETOOTH_SCO_HEADSET;
            if (device) break;
            forcedDevAvailable = false;
            break;
        case AudioSystem::FORCE_WIRED_ACCESSORY:
            device = mAvailableInputDevices & AudioSystem::DEVICE_IN_WIRED_HEADSET;
            if (device) break;
            forcedDevAvailable = false;
            break;
        case AudioSystem::FORCE_SPEAKER:
            device = mAvailableInputDevices & AudioSystem::DEVICE_IN_BUILTIN_MIC;
            if (device) break;
            forcedDevAvailable = false;
            break;
        default:
            break;
        }
        break;

    case AUDIO_SOURCE_VOICE_CALL:
    case AUDIO_SOURCE_VOICE_UPLINK:
    case AUDIO_SOURCE_VOICE_DOWNLINK:
        break;

    default:
        ALOG_WARN("getForcedInputDevice(): invalid input source %d", source);
        device = 0;
        break;
    }

    if (device == 0) {
        switch (source) {

        /* Audio input sources for forced usage FOR_RECORD */
        case AUDIO_SOURCE_DEFAULT:
        case AUDIO_SOURCE_MIC:
        case AUDIO_SOURCE_VOICE_RECOGNITION:
        case AUDIO_SOURCE_CAMCORDER:
            switch (mForceUse[AudioSystem::FOR_RECORD]) {

            case AudioSystem::FORCE_BT_SCO:
                device = mAvailableInputDevices & AudioSystem::DEVICE_IN_BLUETOOTH_SCO_HEADSET;
                if (device) break;
                forcedDevAvailable = false;
                break;
            case AudioSystem::FORCE_WIRED_ACCESSORY:
                device = mAvailableInputDevices & AudioSystem::DEVICE_IN_WIRED_HEADSET;
                if (device) break;
                forcedDevAvailable = false;
                break;
            default:
                break;
            }
            break;

        case AUDIO_SOURCE_VOICE_CALL_NO_RECORD:
        case AUDIO_SOURCE_VOICE_UPLINK:
        case AUDIO_SOURCE_VOICE_DOWNLINK:
        case AUDIO_SOURCE_SPEECH_PROC:
        case AUDIO_SOURCE_VOICE_CALL:

#ifdef STE_VIDEO_CALL
        case AUDIO_SOURCE_VIDEO_CALL:
#endif
        case AUDIO_SOURCE_VOICE_COMMUNICATION:
            break;

        default:
            ALOG_WARN("getForcedInputDevice(): invalid input source %d", source);
            device = 0;
            break;
        }
    }

    if (device) {
        ALOG_INFO_VERBOSE("getForcedInputDevice(): Forced device for input source %s is "
            "%x %s", source2str(source), device,
            device2str((AudioSystem::audio_devices)device));
    } else {
        if (!forcedDevAvailable) {
            ALOG_WARN("getForcedInputDevice(): No forced device available!");
        }
        ALOG_INFO_VERBOSE("getForcedInputDevice(): No forced device for input source %s",
            source2str(source));
    }
    return device;
}

uint32_t AudioPolicyManagerANM::getInputDeviceForSource(
    int source, int channels) {

    uint32_t device = 0;
    /* First check if device usage is forced for this input source */
    device = getForcedInputDevice(source);
    if (device) {
        ALOG_INFO_VERBOSE("getInputDeviceForSource(): Forced device: %x %s", device,
            device2str((AudioSystem::audio_devices)device));
    } else {
        /* Device usage is not forced, get the device from device table */
        device = getPreferredInputDevice(source, channels);
        ALOG_INFO_VERBOSE("getInputDeviceForSource(): Preferred device: %x %s", device,
            device2str((AudioSystem::audio_devices)device));
    }

    return device;
}

int AudioPolicyManagerANM::getInputSource(audio_io_handle_t input)
{
    int source = 0;
    AudioInputDescriptor *descr = mInputs.valueFor(input);
    if (descr) {
        source = descr->mInputSource;
    } else {
        ALOG_ERR("getInputSource(): No input descriptor for input %d!", input);
    }
    return source;
}

uint32_t AudioPolicyManagerANM::getInputDevice(audio_io_handle_t input) {

    uint32_t device = 0;
    AudioInputDescriptor *descr = mInputs.valueFor(input);
    if (descr) {
        if (descr->mVoIPActive) {
            /* Set device as VoIP device */
            device = AudioSystem::DEVICE_IN_DEFAULT;
        } else {
            /* Get input device for this source and number of channels */
            device = getInputDeviceForSource(
                descr->mInputSource, AudioSystem::popCount(descr->mChannels));
        }
    } else {
        ALOG_ERR("getInputDevice(): No input descriptor for input %d!", input);
    }

    return device;
}

void AudioPolicyManagerANM::updateInputConfig(AudioInputDescriptor *descr)
{
    ALOG_INFO_VERBOSE("ENTER updateInputConfig(): device=%s", device2str((AudioSystem::audio_devices)descr->mDevice));

    /* Check if this input is active */
    if (descr->mRefCount) {

        /* Loop through the in devices (the last 16 bits in audio_devices) */
        for (size_t i=0x10000 ; i>0 && i<=0x80000000 ; i=i<<1 ) {
            if (descr->mDevice & i) {
                uint32_t indexTopLevel = getTopLevelDeviceIndex(i);
                if (sourcePriority[descr->mInputSource] > sourcePriority[mCurrentSource[indexTopLevel]]) {
                    /* Update current source for device */
                    mCurrentSource[indexTopLevel] = descr->mInputSource;
                    /* Update top level device map for device */
                    const char *devTopLevel = topLevelInputDevices[indexTopLevel];
                    const char *devActual = NULL;
                    getActualInputDevice(i, descr->mInputSource, &devActual);
                    updateTopLevelDeviceMap(devTopLevel,devActual,NULL, NULL);
                }
            }
        }
    }

    ALOG_INFO_VERBOSE("LEAVE updateInputConfig(): device=%s", device2str((AudioSystem::audio_devices)descr->mDevice));
}

void AudioPolicyManagerANM::updateInputRouting()
{
    ALOG_INFO_VERBOSE("ENTER updateInputRouting(): %d inputs", mInputs.size());

    for (size_t i = 0; i < mInputs.size(); i++) {

        /* Get next input */
        audio_io_handle_t input = mInputs.keyAt(i);

        /* Get input device, based on current conditions */
        uint32_t device = getInputDevice(input);
        if (!device) {
            ALOG_ERR("updateInputRouting(): No selected device for input %d!", input);
            continue;
        }

        /* Check constraints for using device in current scenario */
        checkHwResources(&device);

        /* Get input descriptor for the input */
        AudioInputDescriptor *descr = mInputs.valueFor(input);
        if (descr) {
            bool updateRouting = true;
            if (device == (uint32_t)descr->mDevice) {
                /* Preferred device is the same - do not update routing
                 * but update of input config must be checked */
                ALOG_INFO_VERBOSE("updateInputRouting(): Preferred device is the same for input %d (device = %s)",
                    input, device2str((AudioSystem::audio_devices)device));
                updateRouting = false;
            }

            /* Update the input descriptor with the preferred device,
             * and then check if device config needs to be updated */
            uint32_t oldDevice = descr->mDevice;
            descr->mDevice = device;
            updateInputConfig(descr);

            if (updateRouting) {
                /* Preferred device has changed - update routing */
                ALOG_INFO("updateInputRouting(): Preferred device for input %d has changed from %s to %s",
                    input,
                    device2str((AudioSystem::audio_devices)oldDevice),
                    device2str((AudioSystem::audio_devices)device));
                AudioParameter param = AudioParameter();
                param.addInt(String8(AudioParameter::keyRouting), (int)device);
                mpClientInterface->setParameters(input, param.toString(), 0);
            }
        } else {
            ALOG_ERR("updateInputRouting(): No input descriptor for input %d!", input);
        }
    }

    ALOG_INFO_VERBOSE("LEAVE updateInputRouting(): %d inputs", mInputs.size());
}

void AudioPolicyManagerANM::checkHwResources(uint32_t *device)
{
    /* When earpiece is used as active device, there are constraints on
       which other devices that are allowed to be used simultaneously */
    bool earpieceInUse = false;

    /* Check if earpiece is used in any output */
    for (size_t i = 0; i < mOutputs.size(); i++) {
        AudioOutputDescriptor *descr = mOutputs.valueAt(i);
        if (descr->refCount() > 0) {
            audio_io_handle_t output = mOutputs.keyAt(i);
            /* Get output device, based on current conditions */
            uint32_t outputDevice = getOutputDevice(output);
            if (outputDevice & AudioSystem::DEVICE_OUT_EARPIECE) {
                earpieceInUse = true;
                break;
            }
        }
    }

    /* Check if earpiece is used in cscall */
    if (!earpieceInUse && (mPhoneState == AudioSystem::MODE_IN_CALL || mVoIPActive)) {
        if (getOutputDeviceForStrategy(NULL, STRATEGY_PHONE, AudioSystem::PCM_16_BIT) &
                                       AudioSystem::DEVICE_OUT_EARPIECE) {
            earpieceInUse = true;
        }
    }

    if (earpieceInUse) {
        for (int i = 0; i < DEVLIST_MAX_NO_DEVICES && constraintsEarpiece[i] != 0; i++) {
            if (*device & constraintsEarpiece[i]) {
                ALOG_INFO_VERBOSE("checkHwResources(): %s not allowed to be used with %s, use %s instead",
                    device2str((AudioSystem::audio_devices)constraintsEarpiece[i]),
                    device2str(AudioSystem::DEVICE_OUT_EARPIECE),
                    device2str(AudioSystem::DEVICE_OUT_EARPIECE));
                *device = AudioSystem::DEVICE_OUT_EARPIECE;
                break;
            }
        }
    }
}

/* ---------------------------------------------------------------------------
 * CS Call Routing
 * ---------------------------------------------------------------------------*/
void AudioPolicyManagerANM::updateCSCallRouting()
{
    uint32_t inputDevice  = getInputDeviceForSource(AUDIO_SOURCE_VOICE_CALL_NO_RECORD, 1);
    uint32_t outputDevice = getOutputDeviceForStrategy(NULL, STRATEGY_PHONE,
                               AudioSystem::PCM_16_BIT);
    const char *inTopLevel = NULL;
    const char *inActual = NULL;
    const char *outTopLevel = NULL;
    const char *outActual = NULL;
    uint32_t inTopLevelIndex;
    uint32_t outTopLevelIndex;

    ALOG_INFO_VERBOSE("ENTER updateCSCallRouting(): %s, %s",
        device2str((AudioSystem::audio_devices)inputDevice),
        device2str((AudioSystem::audio_devices)outputDevice));

    if (fd_adm == -1) {
        // connect to ADM
        fd_adm = ste_adm_client_connect();
        ALOG_INFO_VERBOSE("updateCSCallRouting(): Connected to ADM, fd = %d\n", fd_adm);
    }

    LOCK_MUTEX(mMutexCallDevices);
    inTopLevelIndex = getTopLevelDeviceIndex(inputDevice);
    inTopLevel = topLevelInputDevices[inTopLevelIndex];
    getActualInputDevice(inputDevice, AUDIO_SOURCE_VOICE_CALL_NO_RECORD, &inActual);
    outTopLevelIndex = getTopLevelDeviceIndex(outputDevice);
    outTopLevel = topLevelOutputDevices[outTopLevelIndex];
    getActualOutputDevice(outputDevice, STRATEGY_PHONE, &outActual);

    /* Update the current source and strategy since ongoing call has highest prio */
    mCurrentSource[inTopLevelIndex] = AUDIO_SOURCE_VOICE_CALL_NO_RECORD;
    mCurrentStrategy[outTopLevelIndex] = STRATEGY_PHONE;
    /* Update top level map and set cscall devices */
    updateTopLevelDeviceMap(inTopLevel, inActual, outTopLevel, outActual);
    if ((mModemType == ADM_FAT_MODEM) || (mModemType == ADM_FULL_FAT_MODEM)) {
        switch (outputDevice) {
        case AudioSystem::DEVICE_OUT_BLUETOOTH_SCO:
        case AudioSystem::DEVICE_OUT_BLUETOOTH_SCO_HEADSET:
            ste_adm_client_init_cscall_downstream_volume(mStreams[AudioSystem::BLUETOOTH_SCO].mIndexMin, mStreams[AudioSystem::BLUETOOTH_SCO].mIndexMax);
            break;

        default:
            ste_adm_client_init_cscall_downstream_volume(mStreams[AudioSystem::VOICE_CALL].mIndexMin, mStreams[AudioSystem::VOICE_CALL].mIndexMax);
            break;
        }
    }
    ste_adm_set_cscall_devices(fd_adm, inTopLevel, outTopLevel);

    /* update volume for speech */
    switch (outputDevice) {

    ALOG_INFO_VERBOSE("updateCSCallRouting(): Update Volume index for %s\n", device2str((AudioSystem::audio_devices)outputDevice));
    case AudioSystem::DEVICE_OUT_BLUETOOTH_SCO:
    case AudioSystem::DEVICE_OUT_BLUETOOTH_SCO_HEADSET:
        if (mStreams[AudioSystem::BLUETOOTH_SCO].mMuteCount == 0) {
            setStreamVolumeIndex(AudioSystem::BLUETOOTH_SCO, mStreams[AudioSystem::BLUETOOTH_SCO].mIndexCur);
        }
        break;

    default:
        if (mStreams[AudioSystem::VOICE_CALL].mMuteCount == 0) {
            setStreamVolumeIndex(AudioSystem::VOICE_CALL, mStreams[AudioSystem::VOICE_CALL].mIndexCur);
        }
        break;
    }
    UNLOCK_MUTEX(mMutexCallDevices);
    ALOG_INFO_VERBOSE("LEAVE updateCSCallRouting(): %s, %s",
        device2str((AudioSystem::audio_devices)inputDevice),
        device2str((AudioSystem::audio_devices)outputDevice));
}

void AudioPolicyManagerANM::closeCSCall()
{
    ALOG_INFO("closeCSCall()");

    /* Do nothing if ADM session is already closed */
    if (fd_adm != -1) {
        LOCK_MUTEX(mMutexCallDevices);
        ste_adm_set_cscall_devices(fd_adm, NULL, NULL);
        UNLOCK_MUTEX(mMutexCallDevices);
        /* disconnect ADM connection */
        ALOG_INFO_VERBOSE("closeCSCall(): Disconnecting from ADM, fd = %d\n", fd_adm);
        ste_adm_client_disconnect(fd_adm);
        fd_adm = -1;

        /* Invalidate the current source/strategy if voice call*/
        for (int i=0; i<NUM_DEV_OUT_INDEXES; i++) {
            if (mCurrentStrategy[i] == STRATEGY_PHONE) {
                mCurrentStrategy[i] = STRATEGY_UNKNOWN;
            }
        }
        for (int i=0; i<NUM_DEV_IN_INDEXES; i++) {
            if (mCurrentSource[i] == AUDIO_SOURCE_VOICE_CALL_NO_RECORD) {
                mCurrentSource[i] = AUDIO_SOURCE_UNKNOWN;
            }
        }
    } else {
        ALOG_INFO("closeCSCall(): Already closed!");
    }
}

void AudioPolicyManagerANM::checkCallStatus()
{
    LOCK_MUTEX(mMutexCallStatus);
    ALOG_INFO("checkCallStatus()");
    if (mCSCallActive) {
        ALOG_INFO("checkCallStatus(): Voice call is ongoing, but cannot recover call graph");
    } else if (mVoIPActive) {
        ALOG_INFO("checkCallStatus(): VoIP call is ongoing, recover call graph");
        AudioOutputDescriptor *outDescr = NULL;
        AudioInputDescriptor  *inDescr = NULL;
        closeCSCall();
        setVoIPStatus(false, 0,0,0,0);
        updateInputRouting();
        updateOutputRouting();
        /* Check for any existing active VoIP output */
        for (size_t i = 0; i < mOutputs.size(); i++) {
            audio_io_handle_t output = mOutputs.keyAt(i);
            outDescr = mOutputs.valueFor(output);
            if (outDescr != NULL) {
                if (outDescr->mVoIPActive) {
                    /* Active VoIP output found */
                    break;
                }
            }
        }
        /* Check for any existing active VoIP input */
        for (size_t i = 0; i < mInputs.size(); i++) {
            audio_io_handle_t input = mInputs.keyAt(i);
            inDescr = mInputs.valueFor(input);
            if (inDescr != NULL) {
                if (inDescr->mVoIPActive) {
                    /* Active VoIP input found */
                    break;
                }
            }
        }

        bool synced = isVoIPSynced(outDescr->mVoIPType);

        if (outDescr && inDescr) {
            if (outDescr->mVoIPActive && inDescr->mVoIPActive) {
                setVoIPStatus(true, inDescr->mSamplingRate, inDescr->mChannels, outDescr->mSamplingRate, outDescr->mChannels);
                updateCSCallRouting();
                updateInputRouting();
                updateOutputRouting();
            }
        } else if (!synced) {
            if (outDescr) {
                if (outDescr->mVoIPActive) {
                    setVoIPStatus(true, 0, 0, outDescr->mSamplingRate, outDescr->mChannels);
                    updateCSCallRouting();
                    updateInputRouting();
                    updateOutputRouting();
                }
            }
            if (inDescr) {
                if (inDescr->mVoIPActive) {
                    setVoIPStatus(true, inDescr->mSamplingRate, inDescr->mChannels, 0, 0);
                    updateCSCallRouting();
                    updateInputRouting();
                    updateOutputRouting();
                }
            }
        }
    } else {
        ALOG_INFO("checkCallStatus(): No ongoing call to recover");
    }
    UNLOCK_MUTEX(mMutexCallStatus);
}

void AudioPolicyManagerANM::vcOpenRequest(void)
{
    ste_adm_vc_modem_status_t lModemStatus;

    LOCK_MUTEX(mMutexVC);

    if ((mModemType == ADM_FAT_MODEM) || (mModemType == ADM_FULL_FAT_MODEM)) {
        lModemStatus = STE_ADM_VC_MODEM_STATUS_CONNECTED;
    } else {
        lModemStatus = mModemStatus;
    }

    ALOG_INFO("vcOpenRequest(): phoneState=%s, modemStatus=%s",
        mode2str((AudioSystem::audio_mode)mPhoneState), lModemStatus ? "CONNECTED" : "DISCONNECTED");

    /*  Only open VC if both state is IN_CALL and modem is connected */
    if (mPhoneState == AudioSystem::MODE_IN_CALL &&
        lModemStatus == STE_ADM_VC_MODEM_STATUS_CONNECTED &&
        mCSCallActive == false &&
        mVoIPActive == false) {
        ALOG_INFO("vcOpenRequest(): Open voice call");
        mCSCallActive = true;
        updateCSCallRouting();
    } else if (mCSCallActive) {
        ALOG_INFO("vcOpenRequest(): Voice call already opened, do nothing");
    } else if (mVoIPActive) {
        ALOG_INFO("vcOpenRequest(): VoIP active, do nothing");
    } else {
        ALOG_INFO("vcOpenRequest(): Requirements not fulfilled, do nothing");
    }

    UNLOCK_MUTEX(mMutexVC);
}

void AudioPolicyManagerANM::vcCloseRequest(void)
{
    ste_adm_vc_modem_status_t lModemStatus;

    LOCK_MUTEX(mMutexVC);

    if ((mModemType == ADM_FAT_MODEM) || (mModemType == ADM_FULL_FAT_MODEM)) {
        lModemStatus = STE_ADM_VC_MODEM_STATUS_DISCONNECTED;
    } else {
        lModemStatus = mModemStatus;
    }

    ALOG_INFO("vcCloseRequest(): phoneState=%s, modemStatus=%s",
        mode2str((AudioSystem::audio_mode)mPhoneState), lModemStatus ? "CONNECTED" : "DISCONNECTED");

    if (mPhoneState != AudioSystem::MODE_IN_CALL &&
        lModemStatus == STE_ADM_VC_MODEM_STATUS_DISCONNECTED &&
        mCSCallActive == true) {
        ALOG_INFO("vcCloseRequest(): Close voice call");
        mCSCallActive = false;
        closeCSCall();
    } else if (!mCSCallActive) {
        ALOG_INFO("vcCloseRequest(): Voice call not opened, do nothing");
    } else {
        ALOG_INFO("vcCloseRequest(): Voice call requirements for closing not fulfilled, do nothing");
    }

    UNLOCK_MUTEX(mMutexVC);
}

void AudioPolicyManagerANM::subscribeModemStatus(void)
{
    ALOG_INFO_VERBOSE("subscribeModemStatus()");
    pthread_t thread;
    pthread_attr_t tattr;
    pthread_attr_init(&tattr);
    pthread_attr_setdetachstate(&tattr, PTHREAD_CREATE_DETACHED);
    if (pthread_create(&thread, &tattr, threadSubscribeModemStatus, this)) {
        ALOG_ERR("subscribeModemStatus(): pthread_create() failed\n");
    }
}

void* AudioPolicyManagerANM::threadSubscribeModemStatus(void *param)
{
    AudioPolicyManagerANM *ap = (AudioPolicyManagerANM*) param;
    ste_adm_res_t res;
    int adm_id;

    while ((adm_id = ste_adm_client_connect()) < 0) {
        ALOG_ERR("threadSubscribeModemStatus(): Failed to connect to ADM");
        usleep(3 * 1000 * 1000);
    }

    while (1) {
        res = ste_adm_client_request_modem_vc_state(adm_id);
        if (res != STE_ADM_RES_OK) {
            ALOG_ERR("threadSubscribeModemStatus(): ste_adm_client_request_modem_vc_state failed with %d", res);
        }

        if (res == STE_ADM_RES_OK) {
            res = ste_adm_client_read_modem_vc_state(adm_id, &ap->mModemStatus);
            if (res != STE_ADM_RES_OK) {
                ALOG_ERR("threadSubscribeModemStatus(): ste_adm_client_read_modem_vc_state failed with %d", res);
            } else {
                ALOG_INFO("threadSubscribeModemStatus(): ADM Modem Status = %s", ap->mModemStatus == STE_ADM_VC_MODEM_STATUS_CONNECTED ? "CONNECTED" : "DISCONNECTED");
                if (ap->mModemStatus == STE_ADM_VC_MODEM_STATUS_CONNECTED) {
                    ap->vcOpenRequest();
                } else {
                    ap->vcCloseRequest();
                }
            }
        }

        if (res != STE_ADM_RES_OK) {
            ste_adm_client_disconnect(adm_id);
            while ((adm_id = ste_adm_client_connect()) < 0) {
                ALOG_ERR("threadSubscribeModemStatus(): Failed to connect to ADM\n");
                usleep(3 * 1000 * 1000);
            }
        }
    }

    return 0;
}

/* ---------------------------------------------------------------------------
 * VoIP functions
 * ---------------------------------------------------------------------------*/
void AudioPolicyManagerANM::setVoIPStatus(bool enabled,
     int inSamplerate,int inChannels, int outSamplerate,int outChannels)
{
    int voip_enabled = enabled ? 1 : 0;
    ste_adm_voip_mode_t mode;

    mode.in_samplerate = inSamplerate;
    if (inChannels != AudioSystem::CHANNEL_IN_STEREO) {
        mode.in_channels = 1;
    } else {
        mode.in_channels = 2;
    }

    mode.out_samplerate = outSamplerate;
    if (outChannels != AudioSystem::CHANNEL_OUT_STEREO) {
        mode.out_channels = 1;
    } else {
        mode.out_channels = 2;
    }

    ALOG_INFO("setVoIPStatus(): %s, Input (UL) [%d, %d], Output (DL) [%d, %d]",
        enabled ? "ENABLED" : "DISABLED",
        inSamplerate, mode.in_channels, outSamplerate, mode.out_channels);

    /* Inform ADM about VoIP status */
    ste_adm_client_set_cscall_voip_mode(voip_enabled,&mode);

    mVoIPActive = enabled;

    /* If phone state is MODE_IN_CALL and VoIP is not active, a normal
     * voice call might have been received during the VoIP call. */
    if (mPhoneState == AudioSystem::MODE_IN_CALL && !mVoIPActive) {
        ALOG_INFO_VERBOSE("setVoIPStatus(): phone state is MODE_IN_CALL, request VC open\n");
        vcOpenRequest();
    }
}

bool AudioPolicyManagerANM::isVoIPOutput(AudioOutputDescriptor *descr, AudioSystem::stream_type stream,
    bool *inputAvailable, AudioInputDescriptor **inDescr)
{
    *inputAvailable = false;
    ste_adm_vc_modem_status_t lModemStatus;

    if ((mModemType == ADM_FAT_MODEM) || (mModemType == ADM_FULL_FAT_MODEM)) {
        lModemStatus = STE_ADM_VC_MODEM_STATUS_DISCONNECTED;
    } else {
        lModemStatus = mModemStatus;
    }

    if (mCSCallActive
        || lModemStatus == STE_ADM_VC_MODEM_STATUS_CONNECTED
        ) {
        /* No VoIP output */
        descr->mVoIPType = VOIP_INACTIVE;
        return false;
    }

    switch (stream) {
    case AudioSystem::SPEECH_PROC:
#ifdef STE_VIDEO_CALL
    case AudioSystem::VIDEO_CALL:
#endif
        /* VoIP type VIDEO_CALL */
        descr->mVoIPType = VOIP_VIDEO_CALL;
        /* Check for corresponding input */
        for (size_t i = 0; i < mInputs.size(); i++) {
            audio_io_handle_t input = mInputs.keyAt(i);
            AudioInputDescriptor *tmpDescr = mInputs.valueFor(input);
            if (tmpDescr != NULL) {
                if(tmpDescr->mRefCount) {
                    if (tmpDescr->mVoIPType == VOIP_VIDEO_CALL) {
                        *inputAvailable = true;
                        *inDescr = tmpDescr;
                        break;
                    }
                }
            }
        }
        return true;
    case AudioSystem::VOICE_CALL:
        if (mPhoneState == AudioSystem::MODE_IN_COMMUNICATION) {
            /* VoIP type IN_COMMUNICATION */
            descr->mVoIPType = VOIP_IN_COMMUNICATION;
            /* Check for corresponding input */
            for (size_t i = 0; i < mInputs.size(); i++) {
                audio_io_handle_t input = mInputs.keyAt(i);
                AudioInputDescriptor *tmpDescr = mInputs.valueFor(input);
                if (tmpDescr != NULL) {
                    if(tmpDescr->mRefCount) {
                        if (tmpDescr->mVoIPType == VOIP_IN_COMMUNICATION) {
                            *inputAvailable = true;
                            *inDescr = tmpDescr;
                            break;
                        }
                    }
                }
            }
            return true;
        } else {
            /* VoIP type VOICE_CALL */
            descr->mVoIPType = VOIP_VOICE_CALL;
            /* Check for opened input */
            for (size_t i = 0; i < mInputs.size(); i++) {
                audio_io_handle_t input = mInputs.keyAt(i);
                AudioInputDescriptor *tmpDescr = mInputs.valueFor(input);
                if (tmpDescr != NULL) {
                    if(tmpDescr->mRefCount && tmpDescr->mDevice != AudioSystem::DEVICE_IN_FM_RADIO_RX) {
                        *inputAvailable = true;
                        *inDescr = tmpDescr;
                        break;
                    }
                }
            }

            return true;
        }
    default:
        return false;
    }
}

bool AudioPolicyManagerANM::isVoIPInput(AudioInputDescriptor *descr,
    bool *outputAvailable, AudioOutputDescriptor **outDescr)
{
    *outputAvailable = false;
    ste_adm_vc_modem_status_t lModemStatus;

    if ((mModemType == ADM_FAT_MODEM) || (mModemType == ADM_FULL_FAT_MODEM)) {
        lModemStatus = STE_ADM_VC_MODEM_STATUS_DISCONNECTED;
    } else {
        lModemStatus = mModemStatus;
    }

    if (mCSCallActive
        || lModemStatus == STE_ADM_VC_MODEM_STATUS_CONNECTED
        ) {
        /* No VoIP input */
        descr->mVoIPType = VOIP_INACTIVE;
        return false;
    }


    if (descr->mInputSource == AUDIO_SOURCE_VOICE_COMMUNICATION && mPhoneState == AudioSystem::MODE_IN_COMMUNICATION) {
        /* VoIP type IN_COMMUNICATION */
        descr->mVoIPType = VOIP_IN_COMMUNICATION;
        /* Check for corresponding output */
        for (size_t i = 0; i < mOutputs.size(); i++) {
            audio_io_handle_t output = mOutputs.keyAt(i);
            AudioOutputDescriptor *tmpDescr = mOutputs.valueFor(output);
            if (tmpDescr != NULL) {
                if (tmpDescr->mVoIPType == VOIP_IN_COMMUNICATION ||
                    tmpDescr->mVoIPType == VOIP_VOICE_CALL) {
                    if (tmpDescr->refCount()) {
                        /* Found started VOICE_CALL VoIP output */
                        *outputAvailable = true;
                        /* Make sure the found VoIP output has correct type */
                        if (tmpDescr->mVoIPType == VOIP_VOICE_CALL &&
                            mPhoneState == AudioSystem::MODE_IN_COMMUNICATION) {
                            ALOG_INFO("isVoIPInput(): Updating VoIP type of found VoIP output from %d to %d",
                                tmpDescr->mVoIPType, VOIP_IN_COMMUNICATION);
                            tmpDescr->mVoIPType = VOIP_IN_COMMUNICATION;
                        }
                        *outDescr = tmpDescr;
                        break;
                    }
                }
            }
        }
        return true;
    } else if (descr->mInputSource == AUDIO_SOURCE_SPEECH_PROC
#ifdef STE_VIDEO_CALL
        || descr->mInputSource == AUDIO_SOURCE_VIDEO_CALL
#endif
        ) {
        /* VoIP type VIDEO_CALL */
        descr->mVoIPType = VOIP_VIDEO_CALL;
        /* Check for corresponding output */
        for (size_t i = 0; i < mOutputs.size(); i++) {
            audio_io_handle_t output = mOutputs.keyAt(i);
            AudioOutputDescriptor *tmpDescr = mOutputs.valueFor(output);
            if (tmpDescr != NULL) {
                if (tmpDescr->mVoIPType == VOIP_VIDEO_CALL) {
                    if (tmpDescr->refCount()) {
                        /* Found started VIDEO_CALL VoIP output */
                        *outputAvailable = true;
                        *outDescr = tmpDescr;
                        break;
                    }
                }
            }
        }
        return true;
    } else {
        /* Check VoIP type VOICE_CALL */
        for (size_t i = 0; i < mOutputs.size(); i++) {
            audio_io_handle_t output = mOutputs.keyAt(i);
            AudioOutputDescriptor *tmpDescr = mOutputs.valueFor(output);
            if (tmpDescr != NULL) {
                if (tmpDescr->mVoIPType == VOIP_VOICE_CALL) {
                    if (tmpDescr->refCount()) {
                        /* Found started VOICE_CALL VoIP output */
                        *outputAvailable = true;
                        *outDescr = tmpDescr;
                        descr->mVoIPType = VOIP_VOICE_CALL;
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

bool AudioPolicyManagerANM::isVoIPSynced(int type)
{
    /* Return true if VoIP type is synced.
     * Synced VoIP type means that the VoIP call graph requires both
     * input and output to be active */
    if (type == VOIP_VOICE_CALL ||
        type == VOIP_VIDEO_CALL ||
        type == VOIP_IN_COMMUNICATION) {
        return true;
    } else {
        return false;
    }
}

/* ---------------------------------------------------------------------------
 * AudioPolicyInterface implementation
 * ---------------------------------------------------------------------------*/

status_t AudioPolicyManagerANM::setDeviceConnectionState(
    AudioSystem::audio_devices              device,
    AudioSystem::device_connection_state    state,
    const char*                             device_address)
{
    LOCK_MUTEX(mMutexDevState);

    ALOG_INFO("setDeviceConnectionState(): device %x %s, state %s, address %s\n",
        device, device2str(device), state2str(state), device_address);

    /* Connect/disconnect only 1 device at a time */
    if (AudioSystem::popCount(device) != 1) {
        ALOG_ERR("setDeviceConnectionState(): trying to connect %d devices "
            "at the same time", AudioSystem::popCount(device));
        UNLOCK_MUTEX(mMutexDevState);
        return BAD_VALUE;
    }
#ifdef STD_A2DP_MNGT
    if (strlen(device_address) >= MAX_DEVICE_ADDRESS_LEN) {
        ALOG_ERR("setDeviceConnectionState() invalid address: %s", device_address);
        return BAD_VALUE;
    }
#endif

    /* Handle output devices */
    if (AudioSystem::isOutputDevice(device)) {

        switch (state) {

        /* Handle new output device connection */
        case AudioSystem::DEVICE_STATE_AVAILABLE:

            /* Check if the device is already connected */
            if (mAvailableOutputDevices & device) {
                ALOG_WARN("setDeviceConnectionState(): device %x %s already "
                    "connected", device, device2str(device));
                UNLOCK_MUTEX(mMutexDevState);
                return NO_ERROR;
            }

            /* Register new output device as available */
            mAvailableOutputDevices |= device;

#ifdef STD_A2DP_MNGT
            // handle A2DP device connection
            if (AudioSystem::isA2dpDevice(device)) {
                status_t status = handleA2dpConnection(device, device_address);
                if (status != NO_ERROR) {
                    mAvailableOutputDevices &= ~device;
                    return status;
                }
            }
#endif
            break;

        /* Handle output device disconnection */
        case AudioSystem::DEVICE_STATE_UNAVAILABLE:

            /* Check if the device is connected */
            if (!(mAvailableOutputDevices & device)) {
                ALOG_WARN("setDeviceConnectionState(): device not connected: "
                    "%x %s\n", device, device2str(device));
                UNLOCK_MUTEX(mMutexDevState);
                return NO_ERROR;
            }

            /*  Remove device from available output devices */
            mAvailableOutputDevices &= ~device;

#ifdef STD_A2DP_MNGT
            // handle A2DP device disconnection
            if (AudioSystem::isA2dpDevice(device)) {
                status_t status = handleA2dpDisconnection(device, device_address);
                if (status != NO_ERROR) {
                    mAvailableOutputDevices |= device;
                    return status;
                }
            }
#endif
            break;

        default:
            ALOG_ERR("setDeviceConnectionState(): invalid state: %x\n", state);
            UNLOCK_MUTEX(mMutexDevState);
            return BAD_VALUE;
        }

#ifdef STD_A2DP_MNGT
        checkOutputForAllStrategies();
        // A2DP outputs must be closed after checkOutputForAllStrategies() is executed
        if (state == AudioSystem::DEVICE_STATE_UNAVAILABLE && AudioSystem::isA2dpDevice(device)) {
            closeA2dpOutputs();
        }
        checkA2dpSuspend();
#endif
        /* Update routing to the new preferred device for all outputs */
        updateOutputRouting();

        /* Make sure that, if applicable, the corresponding input device is
         * handled as well.
         */
        switch (device) {
        case AudioSystem::DEVICE_OUT_WIRED_HEADSET:
            device = AudioSystem::DEVICE_IN_WIRED_HEADSET;
            break;
        case AudioSystem::DEVICE_OUT_BLUETOOTH_SCO:
        case AudioSystem::DEVICE_OUT_BLUETOOTH_SCO_HEADSET:
        case AudioSystem::DEVICE_OUT_BLUETOOTH_SCO_CARKIT:
            device = AudioSystem::DEVICE_IN_BLUETOOTH_SCO_HEADSET;
            break;
        default:
            /* No corresponding input device. Update CS call routing */
            if (mCSCallActive || mVoIPActive) {
                updateCSCallRouting();
            }
            UNLOCK_MUTEX(mMutexDevState);
            return NO_ERROR;
        }
    }

    /* Handle input devices */
    if (AudioSystem::isInputDevice(device)) {

        switch (state) {

        /* Handle input device connection */
        case AudioSystem::DEVICE_STATE_AVAILABLE:

            /* Check if the device is already connected */
            if (mAvailableInputDevices & device) {
                ALOG_WARN("setDeviceConnectionState(): device already connected: "
                    "%x %s\n", device, device2str(device));
                UNLOCK_MUTEX(mMutexDevState);
                return INVALID_OPERATION;
            }

            /* Register new input device as available */
            mAvailableInputDevices |= device;

            break;

        /* Handle input device disconnection */
        case AudioSystem::DEVICE_STATE_UNAVAILABLE:

            /* Check if the device is connected */
            if (!(mAvailableInputDevices & device)) {
                ALOG_WARN("setDeviceConnectionState(): device not connected: "
                    "%x %s\n", device, device2str(device));
                UNLOCK_MUTEX(mMutexDevState);
                return INVALID_OPERATION;
            }

            /*  Remove device from available input devices */
            mAvailableInputDevices &= ~device;

            break;

        default:
            ALOG_ERR("setDeviceConnectionState(): invalid state: %x\n", state);
            UNLOCK_MUTEX(mMutexDevState);
            return BAD_VALUE;
        }

        /* Update CS call routing */
        if (mCSCallActive || mVoIPActive) {
            updateCSCallRouting();
        }
        /* Update routing to the new preferred device for all inputs */
        updateInputRouting();

        UNLOCK_MUTEX(mMutexDevState);
        return NO_ERROR;
    }

    ALOG_WARN("setDeviceConnectionState(): invalid device: %x\n", device);
    UNLOCK_MUTEX(mMutexDevState);
    return BAD_VALUE;
}


AudioSystem::device_connection_state
    AudioPolicyManagerANM::getDeviceConnectionState(
        AudioSystem::audio_devices device, const char *device_address)
{
    AudioSystem::device_connection_state state =
        AudioSystem::DEVICE_STATE_UNAVAILABLE;
#ifdef STD_A2DP_MNGT
    String8 address = String8(device_address);
#else
    (void)device_address;
#endif

    if (AudioSystem::isOutputDevice(device)) {
        if (device & mAvailableOutputDevices) {
#ifdef STD_A2DP_MNGT
            if (AudioSystem::isA2dpDevice(device) &&
                address != "" && mA2dpDeviceAddress != address) {
                return state;
            }
#endif
            state = AudioSystem::DEVICE_STATE_AVAILABLE;
        }
    } else if (AudioSystem::isInputDevice(device)) {
        if (device & mAvailableInputDevices) {
            state = AudioSystem::DEVICE_STATE_AVAILABLE;
        }
    }

    ALOG_INFO_VERBOSE("getDeviceConnectionState(): device = %s, state = %s\n",
        device2str(device), state2str(state));
    return state;
}

void AudioPolicyManagerANM::setPhoneState(int state)
{
    if (state < 0 || state >= AudioSystem::NUM_MODES) {
        ALOG_WARN("setPhoneState(): Invalid state %d\n", state);
        return;
    }
    if (state == (int)mPhoneState ) {
        ALOG_WARN("setPhoneState(): Trying to set same state as current state %s\n",
            mode2str((AudioSystem::audio_mode)state));
        return;
    }

    int oldState = (int)mPhoneState;
    /* Update phone state to new state */
    mPhoneState = state;

    ALOG_INFO("setPhoneState(): %s (old state was %s)\n",
        mode2str((AudioSystem::audio_mode)state),
        mode2str((AudioSystem::audio_mode)oldState));

    if (oldState == AudioSystem::MODE_IN_CALL && state != AudioSystem::MODE_IN_CALL) {
        /* Close CS call, since phone state has changed from MODE_IN_CALL */
        vcCloseRequest();
    }

    if (oldState == AudioSystem::MODE_IN_COMMUNICATION && state != AudioSystem::MODE_IN_COMMUNICATION) {
        /* Close VoIP call, since phone state has changed from MODE_IN_COMMUNICATION */

        /* Find VoIP input of type IN_COMMUNICATION, and reroute */
        for (size_t i = 0; i < mInputs.size(); i++) {
            AudioInputDescriptor *tmpDescr = mInputs.valueFor(mInputs.keyAt(i));
            if (tmpDescr != NULL) {
                if (tmpDescr->mVoIPType==VOIP_IN_COMMUNICATION &&
                    tmpDescr->mVoIPActive) {
                        ALOG_INFO("setPhoneState(): reroute input to other device than VoIP device");
                        tmpDescr->mVoIPActive=false;
                        updateInputRouting();
                }
            }
        }

        /* Find VoIP output of type IN_COMMUNICATION, and reroute */
        for (size_t i = 0; i < mOutputs.size(); i++) {
            AudioOutputDescriptor *tmpDescr = mOutputs.valueFor(mOutputs.keyAt(i));
            if (tmpDescr != NULL) {
                if (tmpDescr->mVoIPType==VOIP_IN_COMMUNICATION &&
                    tmpDescr->mVoIPActive) {
                        ALOG_INFO("setPhoneState(): reroute output to other device than VoIP device");
                        tmpDescr->mVoIPActive=false;
                        updateOutputRouting();
                }
            }
        }

        if (mVoIPActive) {
            ALOG_INFO("setPhoneState(): close VoIP call graph");
            closeCSCall();
            setVoIPStatus(false, 0,0,0,0);
        }
    }

    /* Update device routing for cscall, outputs and inputs */
    if (mPhoneState == AudioSystem::MODE_IN_CALL) {
        vcOpenRequest();
    }
    updateOutputRouting();
    updateInputRouting();

    /* update volume for speech */
    if (mStreams[AudioSystem::VOICE_CALL].mMuteCount == 0) {
        setStreamVolumeIndex(AudioSystem::VOICE_CALL, mStreams[AudioSystem::VOICE_CALL].mIndexCur);
    }
}

void AudioPolicyManagerANM::setRingerMode(uint32_t mode, uint32_t mask)
{
    ALOG_INFO("setRingerMode(): mode %x, mask %x\n", mode, mask);
    mRingerMode = mode;
}

void AudioPolicyManagerANM::setForceUse(
    AudioSystem::force_use usage, AudioSystem::forced_config config)
{
    ALOG_INFO("setForceUse(): force_use %d (%s), forced_config %d (%s)\n",
        usage, force_use2str(usage), config, forced_config2str(config));

    LOCK_MUTEX(mMutexForceUse);

    mForceUseQueued[usage] = config;

    if (!mForceUseOngoing) {
        mForceUseOngoing = true;

        pthread_t thread;
        pthread_attr_t tattr;
        pthread_attr_init(&tattr);
        pthread_attr_setdetachstate(&tattr, PTHREAD_CREATE_DETACHED);
        if (pthread_create(&thread, &tattr, threadForceUse, this)) {
            ALOG_ERR("setForceUse(): pthread_create() failed\n");
        }
    }

    UNLOCK_MUTEX(mMutexForceUse);
}

AudioSystem::forced_config AudioPolicyManagerANM::getForceUse(
    AudioSystem::force_use usage)
{
    ALOG_INFO_VERBOSE("getForceUse(): force_use = %d (%s), forced_config = %d (%s)\n",
        usage, force_use2str(usage), mForceUse[usage],
        forced_config2str(mForceUse[usage]));
    return mForceUse[usage];
}

void* AudioPolicyManagerANM::threadForceUse(void* param)
{
    AudioPolicyManagerANM* ap = (AudioPolicyManagerANM*)param;
    ALOG_INFO_VERBOSE("ENTER threadForceUse():");
    ap->updateForceUse();
    ALOG_INFO_VERBOSE("LEAVE threadForceUse():");
    return NULL;
}

void AudioPolicyManagerANM::updateForceUse()
{
    LOCK_MUTEX(mMutexDevState);
    LOCK_MUTEX(mMutexForceUse);
    for (int i = 0; i < AudioSystem::NUM_FORCE_USE; i++) {
        mForceUse[i] = mForceUseQueued[i];
    }
    UNLOCK_MUTEX(mMutexForceUse);

    bool forceUseChanged = true;
    while (forceUseChanged) {
        ALOG_INFO_VERBOSE("updateForceUse(): update routing");
        /* Update device routing for cscall, outputs and inputs */
        if (mCSCallActive || mVoIPActive) {
            updateCSCallRouting();
        }
        updateOutputRouting();
        updateInputRouting();

        forceUseChanged = false;
        LOCK_MUTEX(mMutexForceUse);
        for (int i = 0; i < AudioSystem::NUM_FORCE_USE; i++) {
            if (mForceUseQueued[i] != mForceUse[i]) {
                forceUseChanged = true;
                for (int i = 0; i < AudioSystem::NUM_FORCE_USE; i++) {
                    mForceUse[i] = mForceUseQueued[i];
                }
                break;
            }
        }
        mForceUseOngoing = forceUseChanged;
        UNLOCK_MUTEX(mMutexForceUse);
    }
    UNLOCK_MUTEX(mMutexDevState);
}

void AudioPolicyManagerANM::setSystemProperty(const char* property,
                                              const char* value)
{
    ALOG_INFO_VERBOSE("setSystemProperty(): property %s, value %s\n", property, value);
    if (strcmp(property, "ro.camera.sound.forced") == 0) {
        if (atoi(value)) {
            ALOG_INFO_VERBOSE("setSystemProperty(): ENFORCED_AUDIBLE cannot be muted\n");
            mStreams[AudioSystem::ENFORCED_AUDIBLE].mCanBeMuted = false;
        } else {
            ALOG_INFO_VERBOSE("setSystemProperty(): ENFORCED_AUDIBLE can be muted\n");
            mStreams[AudioSystem::ENFORCED_AUDIBLE].mCanBeMuted = true;
        }
    }
}

bool AudioPolicyManagerANM::isSupportedCodedFormat(uint32_t format)
{
    switch (format) {
        case AudioSystem::AC3:     return true;
        case AudioSystem::MPEG1:   return true;
        case AudioSystem::MPEG2:   return true;
        case AudioSystem::DTS:     return true;
        case AudioSystem::ATRAC:   return true;

        case AudioSystem::OBA:     return true;
        case AudioSystem::DDPLUS:  return true;
        case AudioSystem::DTS_HD:  return true;
        case AudioSystem::MAT:     return true;
        case AudioSystem::DST:     return true;

        case AudioSystem::WMA_PRO: return true;
        default:                   return false;
    }
}

status_t AudioPolicyManagerANM::initCheck()
{
    return NO_ERROR;
}

/* ---------------------------------------------------------------------------
 * Output handling
 * ---------------------------------------------------------------------------*/
audio_io_handle_t AudioPolicyManagerANM::getOutput(
    AudioSystem::stream_type stream,
    uint32_t samplingRate,
    uint32_t format,
    uint32_t channels,
    AudioSystem::output_flags flags)
{
    audio_io_handle_t output = 0;
    AudioOutputDescriptor *descr = NULL;

    (void) ste_anm_debug_setup_log();

    ALOG_INFO("ENTER getOutput(): %s, samplerate=%d, format=%d, channels=%x, flags=%x\n",
        stream2str(stream), samplingRate, format, channels, flags);

    /* Check the format. */
    if (format != 0 &&
        !(AudioSystem::isLinearPCM(format) || isSupportedCodedFormat(format))) {
        ALOG_ERR("getOutput(): Format %X is not supported", format);
        return 0;
    }

    /* Check number of channels */
    if (channels != 0 &&
        channels != AudioSystem::CHANNEL_OUT_MONO &&
        channels != AudioSystem::CHANNEL_OUT_STEREO) {
        ALOG_ERR("getOutput(): Channels=%x is not supported", channels);
        return 0;
    }

    /* Check input values */
    if (format == 0) {
        format = DEFAULT_PCM_FORMAT;
        ALOG_INFO_VERBOSE("getOutput(): Format is 0 - update to %x", format);
    }
    if (channels == 0) {
        channels = DEFAULT_OUTPUT_CHANNELS;
        ALOG_INFO_VERBOSE("getOutput(): Channels is 0 - update to %x", channels);
    }
    if (samplingRate == 0) {
        samplingRate = DEFAULT_OUTPUT_SAMPLE_RATE;
        ALOG_INFO_VERBOSE("getOutput(): Sample rate is 0 - update to %d", samplingRate);
    }

    /* Get the corresponding strategy for this stream type */
    AudioPolicyManagerANM::routing_strategy strategy = getStrategy(stream);

    /* Check if an indirect/shared output is allowed to be used */
    if (flags == AudioSystem::OUTPUT_FLAG_INDIRECT) {
        /* Reuse output if already opened for this strategy */
        for (size_t i = 0; i < mOutputs.size(); i++) {
            descr = mOutputs.valueAt(i);
            if (descr->mStrategyRefCount>=1 && descr->mStrategy==strategy) {
                if (AudioSystem::isLinearPCM(descr->mFormat)) {
                    descr->mStrategyRefCount++;
                    output = mOutputs.keyAt(i);
                    ALOG_INFO("getOutput(): Reused existing output %d for %s, StrategyRefCount = %d",
                        output, strategy2str(strategy),descr->mStrategyRefCount);
                    break;
                }
            }
        }
    }

    uint32_t device = getOutputDeviceForStrategy(NULL, strategy, format
#ifdef STD_A2DP_MNGT
        , true
#endif
        );
    if (device == 0) {
        ALOG_ERR("getOutput() no device found for strategy: %d", strategy);
    }

    if (output == 0) {
        /* Open new hardware output */
        descr = new AudioOutputDescriptor();

        descr->mSamplingRate = samplingRate;
        descr->mFormat = format;
        descr->mChannels = channels;
        descr->mFlags = flags;
        descr->mDevice = device;
        descr->mStrategyRefCount=1;
        descr->mStrategy=strategy;

        output = mpClientInterface->openOutput(&descr->mDevice,
            &descr->mSamplingRate, &descr->mFormat, &descr->mChannels,
            &descr->mLatency, descr->mFlags);

        if (output) {
            ALOG_INFO("getOutput() Opened new output %d for strategy %s, mStrategyRefCount = %d",
                output, strategy2str(strategy),descr->mStrategyRefCount);
            /* Add to the list of available outputs */
            mOutputs.add(output, descr);
        } else {
            ALOG_ERR("getOutput(): Failed to open output for %s, device %x."
                " SR (%d) F (%d) C (%08X) L (%d) FL (%08X)\n",
                strategy2str(strategy), descr->mDevice, descr->mSamplingRate,
                descr->mFormat, descr->mChannels, descr->mLatency,
                descr->mFlags);
            return 0;
        }
    }

#ifdef STD_A2DP_MNGT
    uint32_t a2dpDevice = device & AudioSystem::DEVICE_OUT_ALL_A2DP;
    /* Check if an indirect/shared output is allowed to be used */
    if (flags == AudioSystem::OUTPUT_FLAG_INDIRECT) {
        /* Reuse output if already opened for this strategy */
        // get which output is suitable for the specified stream. The actual routing
        // change will happen when startOutput() will be called
        if (AudioSystem::popCount((AudioSystem::audio_devices)device) != 2) {
            if (a2dpDevice != 0) {
                // if playing on A2DP device, use a2dp output
                if (mA2dpOutput == 0) {
                    ALOG_WARN("getOutput() A2DP device %x selected but A2DP output not opened",
                            device);
                } else {
                    ALOG_INFO("getOutput() using mA2dpOutput = %d for devices %x",
                            mA2dpOutput, device);
                }
                descr->mStrategyRefCount--;
                ALOG_INFO("getOutput() for output = %d,  mStrategyRefCount = %d",
                        output, descr->mStrategyRefCount);
                output = mA2dpOutput;
                descr = mOutputs.valueFor(mA2dpOutput);
                descr->mStrategyRefCount++;
                ALOG_INFO("getOutput() for output = %d,  mStrategyRefCount = %d",
                        output, descr->mStrategyRefCount);
            }
        }
    }
#endif

#ifdef STD_A2DP_MNGT
    /* Check if an indirect/shared output is allowed to be used */
    if (flags == AudioSystem::OUTPUT_FLAG_INDIRECT) {
        /* Reuse output if already opened for this strategy */
        // get which output is suitable for the specified stream. The actual routing change will happen
        // when startOutput() will be called
        if (AudioSystem::popCount((AudioSystem::audio_devices)device) == 2) {
            if (a2dpUsedForSonification() && a2dpDevice != 0) {
                // if playing on 2 devices among which one is A2DP, use duplicated output
                ALOG_INFO("getOutput() using duplicated output");
                if (mA2dpOutput == 0) {
                    ALOG_WARN("getOutput() A2DP device in multiple %x selected but " \
                            "A2DP output not opened", device);
                }
                if (!mDuplicatedOutput) {
                    mDuplicatedOutput = mpClientInterface->openDuplicateOutput(mA2dpOutput, output);
                    // add duplicated output descriptor
                    AudioOutputDescriptor *dupOutputDesc = new AudioOutputDescriptor();
                    AudioOutputDescriptor *outputDesc = mOutputs.valueFor(mA2dpOutput);
                    dupOutputDesc->mOutput1 = mOutputs.valueFor(output);
                    dupOutputDesc->mId1 = output;
                    dupOutputDesc->mOutput2 = mOutputs.valueFor(mA2dpOutput);

                    ALOG_INFO("created duplicated output %d for a2dpOutput %d, hwOutput %d", \
                            mDuplicatedOutput, mA2dpOutput, output);

                    dupOutputDesc->mSamplingRate = outputDesc->mSamplingRate;
                    dupOutputDesc->mFormat = outputDesc->mFormat;
                    dupOutputDesc->mChannels = outputDesc->mChannels;
                    dupOutputDesc->mLatency = outputDesc->mLatency;
                    dupOutputDesc->mDevice = device;
                    dupOutputDesc->mStrategy = outputDesc->mStrategy;
                    mOutputs.add(mDuplicatedOutput, dupOutputDesc);
                    output = mDuplicatedOutput;
                    descr = dupOutputDesc;
                    descr->mStrategyRefCount = 1;
                } else {
                    output = mDuplicatedOutput;
                    descr = mOutputs.valueFor(output);
                    descr->mStrategyRefCount++;
                }
                ALOG_INFO("getOutput() using mDuplicatedOutput = %d for 2 devices %x, " \
                        "mStrategyRefCount = %d", output, device, descr->mStrategyRefCount);
            }
        }
    }
#endif

    ALOG_INFO("LEAVE getOutput(): Returned output %d, %s, %s, "
        "device=%x, SR (%d) F (%d) C (%x) L (%d) FL (%x)\n",
        output, strategy2str(strategy), stream2str(stream), descr->mDevice,
        descr->mSamplingRate, descr->mFormat, descr->mChannels, descr->mLatency,
        descr->mFlags);

    return output;
}

status_t AudioPolicyManagerANM::startOutput(audio_io_handle_t output, AudioSystem::stream_type stream, int session)
{
    ALOG_INFO("ENTER startOutput(): output %d, stream %s\n", output, stream2str(stream));

    LOCK_MUTEX(mMutexDevState);

    ssize_t index = mOutputs.indexOfKey(output);
    if (index < 0) {
        ALOG_WARN("LEAVE startOutput(): Unknown output %d\n", output);
        UNLOCK_MUTEX(mMutexDevState);
        return BAD_VALUE;
    }

    /* Incremenent ref count of this stream on the requested output */
    AudioOutputDescriptor *descr = mOutputs.valueFor(output);
    descr->changeRefCount(stream, 1);
    if (descr->mRefCount[stream]>1) {
        bool inputAvailable = false;
        AudioInputDescriptor *inDescr = NULL;
        AudioOutputDescriptor oldOutDescr = *descr;
        if (isVoIPOutput(descr, stream, &inputAvailable, &inDescr)) {
            if (oldOutDescr.mVoIPType != descr->mVoIPType) {
                ALOG_INFO("LEAVE startOutput(): output already started, "
                    "do nothing but update VoIP type from %s to %s",
                    voiptype2str(oldOutDescr.mVoIPType),
                    voiptype2str(descr->mVoIPType));
                UNLOCK_MUTEX(mMutexDevState);
                return NO_ERROR;
            }
        }
        ALOG_INFO("LEAVE startOutput(): output already started, do nothing");
        UNLOCK_MUTEX(mMutexDevState);
        return NO_ERROR;
    }

    /* Make sure the output is restored when started */
    LOCK_MUTEX(mMutexIO);
    if (descr->mSuspended) {
        ALOG_INFO("startOutput(): output is suspended, restore output");
        mpClientInterface->restoreOutput(output);
        descr->mSuspended=false;
    }
    UNLOCK_MUTEX(mMutexIO);

    /* Check VoIP conditions */
    bool inputAvailable = false;
    AudioInputDescriptor *inDescr = NULL;
    if (isVoIPOutput(descr, stream, &inputAvailable, &inDescr)) {

        ALOG_INFO("startOutput(): VoIP of type=%s\n",
            voiptype2str(descr->mVoIPType));

        /* Check if VoIP input should be synced, i.e. if the call graph
         * should only be setup when both input and output are started */
        bool syncedInput = isVoIPSynced(descr->mVoIPType);

        if (syncedInput && inputAvailable) {
            ALOG_INFO("startOutput(): routing output and input to VoIP devices\n");
            setVoIPStatus(true, inDescr->mSamplingRate, inDescr->mChannels, descr->mSamplingRate, descr->mChannels);

            /* Update VoIP info */
            descr->mVoIPActive = true;
            inDescr->mVoIPActive = true;
            inDescr->mVoIPType = descr->mVoIPType;
            /* Setup the call graph */
            updateCSCallRouting();
            /* Route input to VoIP device */
            updateInputRouting();

        } else if (syncedInput) {
            ALOG_INFO("startOutput(): VoIP output in synced mode, wait for input to switch to VoIP\n");
        } else {
            ALOG_INFO("startOutput(): routing output to VoIP device\n");
            if (inputAvailable) {
                setVoIPStatus(true, inDescr->mSamplingRate, inDescr->mChannels, descr->mSamplingRate, descr->mChannels);
            } else {
                setVoIPStatus(true, 0, 0, descr->mSamplingRate, descr->mChannels);
            }
            /* Update VoIP info */
            descr->mVoIPActive = true;
            /* Setup the call graph */
            updateCSCallRouting();
        }
    }

    /* Update the output routing */
    updateOutputRouting(
#ifdef STD_A2DP_MNGT
        true
#endif
        );

#ifdef STD_A2DP_MNGT
    if (output == mDuplicatedOutput)
    {
        AudioParameter param = AudioParameter();
        param.addInt(String8(PARAM_KEY_STREAM_STATE), 1);
        mpClientInterface->setParameters(descr->mId1, param.toString(), 0);
    }
    else if (output != mA2dpOutput)
#endif
    {
        AudioParameter param = AudioParameter();
        param.addInt(String8(PARAM_KEY_STREAM_STATE), 1);
        /* Get reference to AudioStreamOut in AHI */
        if (descr->getAudioOutputRef() == 0) {
            String8 keyValuePairs = mpClientInterface->getParameters(output, String8(PARAM_KEY_STREAM_REF));
            AudioParameter pointerParam = AudioParameter(keyValuePairs);
            int ptr = 0;
            if (pointerParam.getInt(String8(PARAM_KEY_STREAM_REF), ptr) == NO_ERROR) {
                ALOG_INFO_VERBOSE("startOutput(): AHI returned a pointer: 0x%x", ptr);
                descr->setAudioOutputRef(ptr);
            } else {
            ALOG_INFO_VERBOSE("startOutput(): No pointer returned from AHI.");
            }
        }
        if (descr->getAudioOutputRef() == 0) {
            /* Set parameters via audio policy client interface */
            mpClientInterface->setParameters(output, param.toString(), 0);
        } else {
           /* Set parameters directly using the AHI reference */
           ((AudioStreamOut*)(descr->getAudioOutputRef()))->setParameters(param.toString());
        }

        /* Check if stream was started OK */
        String8 streamState;
        if (descr->getAudioOutputRef() == 0) {
            streamState = mpClientInterface->getParameters(output, String8(PARAM_KEY_STREAM_STATE));
        } else {
            streamState = ((AudioStreamOut*)(descr->getAudioOutputRef()))->getParameters(String8(PARAM_KEY_STREAM_STATE));
        }
        AudioParameter streamStateParam = AudioParameter(streamState);
        int val;
        if (streamStateParam.getInt(String8(PARAM_KEY_STREAM_STATE), val) == NO_ERROR) {
            if (val != 1) {
                ALOG_ERR("LEAVE startOutput(): setParameter for ste_stream_state failed in startOutput()");
                UNLOCK_MUTEX(mMutexDevState);
                return UNKNOWN_ERROR;
            }
        }
    }

    /* apply volume rules for current stream and device if necessary */
    if (mStreams[stream].mMuteCount == 0) {
        setStreamVolumeIndex(stream, mStreams[stream].mIndexCur);
    }

    /* Update latency now when the device has been opened (e.g. delay
     * caused by A2DP device is not known until the device has been opened) */
    AudioParameter latencyParam = AudioParameter();
    latencyParam.addInt(String8("ste_sink_latency"), output);
    if (descr->getAudioOutputRef() == 0) {
        mpClientInterface->setParameters(output, latencyParam.toString(), 0);
    } else {
        ((AudioStreamOut*)(descr->getAudioOutputRef()))->setParameters(latencyParam.toString());
    }

    ALOG_INFO("LEAVE startOutput(): output %d, stream %s\n", output, stream2str(stream));
    UNLOCK_MUTEX(mMutexDevState);

    return NO_ERROR;
}

status_t AudioPolicyManagerANM::stopOutput(audio_io_handle_t output, AudioSystem::stream_type stream, int session)
{
    ALOG_INFO("ENTER stopOutput(): output %d, stream %s\n", output, stream2str(stream));

    LOCK_MUTEX(mMutexDevState);

    ssize_t index = mOutputs.indexOfKey(output);
    if (index < 0) {
        ALOG_WARN("LEAVE stopOutput(): Unknown output %d\n", output);
        UNLOCK_MUTEX(mMutexDevState);
        return BAD_VALUE;
    }

    AudioOutputDescriptor *descr = mOutputs.valueAt(index);

    if (descr->isUsedByStream(stream)) {
        /* Decrement ref count of this stream on the output */
        descr->changeRefCount(stream, -1);
        descr->mStopTime[stream] = systemTime();
    } else {
        ALOG_WARN("LEAVE stopOutput(): Ref count is already 0 for stream %s on output %d\n",
            stream2str(stream), output);
        UNLOCK_MUTEX(mMutexDevState);
        return INVALID_OPERATION;
    }

    if (descr->mRefCount[stream] > 0) {
        ALOG_INFO("LEAVE stopOutput(): Ref count for stream %s is %d, do nothing", stream2str(stream), descr->mRefCount[stream]);
        UNLOCK_MUTEX(mMutexDevState);
        return NO_ERROR;
    }

    /* Check if VoIP is active */
    if (descr->mVoIPActive) {
        ALOG_INFO("stopOutput(): VoIP is active for output %d\n", output);

        /* Check if VoIP input should be synced, i.e. if the call graph
         * should be closed regardless if input is active or not */
        bool syncedInput = isVoIPSynced(descr->mVoIPType);

        /* Check if active VoIP input exists */
        bool inputFound = false;
        AudioInputDescriptor *inDescr;
        for (size_t i = 0; i < mInputs.size(); i++) {
            audio_io_handle_t input = mInputs.keyAt(i);
            AudioInputDescriptor *tmpDescr = mInputs.valueFor(input);
            if (tmpDescr != NULL) {
                if(tmpDescr->mVoIPActive) {
                    inputFound = true;
                    inDescr = tmpDescr;
                    break;
                }
            }
        }

        /* Reroute VoIP output. Check if VoIP input should be
         * rerouted and if VoIP call graph should be closed */
        ALOG_INFO("stopOutput(): synced=%d, activeInput=%d\n", syncedInput, inputFound);
        ALOG_INFO("stopOutput(): reroute VoIP output\n");
        descr->mVoIPActive = false;
        updateOutputRouting();
        if (syncedInput || !inputFound) {
            if (inputFound) {
                ALOG_INFO("stopOutput(): reroute VoIP input\n");
                inDescr->mVoIPActive = false;
                updateInputRouting();
            }
            ALOG_INFO("stopOutput(): close VoIP call graph\n");
            closeCSCall();
            setVoIPStatus(false, 0,0,0,0);
        }
    }

#ifdef STD_A2DP_MNGT
    if (output == mDuplicatedOutput)
    {
        AudioParameter param = AudioParameter();
        param.addInt(String8(PARAM_KEY_STREAM_STATE), 0);
        mpClientInterface->setParameters(descr->mId1, param.toString(), 0);
    }
    else if (output != mA2dpOutput)
#endif
    {
        AudioParameter param = AudioParameter();
        param.addInt(String8(PARAM_KEY_STREAM_STATE), 0);
        if (descr->getAudioOutputRef() == 0) {
            /* Set parameters via audio policy client interface */
            mpClientInterface->setParameters(output, param.toString(), 0);
        } else {
            /* Set parameters directly using the AHI reference */
            ((AudioStreamOut*)(descr->getAudioOutputRef()))->setParameters(param.toString());
        }
    }
    if (!descr->refCount()) {
        bool updateRouting = false;
        /* Loop through the out devices (the first 16 bits in audio_devices) */
        for (size_t i=0x1 ; i<= 0x8000 ; i=i<<1 ) {
            if (descr->mDevice & i) {
                uint32_t indexTopLevel = getTopLevelDeviceIndex(i);
                if (mCurrentStrategy[indexTopLevel] == descr->mStrategy &&
                    !(descr->mStrategy==STRATEGY_PHONE && mPhoneState==AudioSystem::MODE_IN_CALL)) {
                    /* Invalidate current strategy for device */
                    mCurrentStrategy[indexTopLevel] = STRATEGY_UNKNOWN;
                    updateRouting = true;
                }
            }
        }
        if (updateRouting) {
            updateOutputRouting();
        }
    }

    ALOG_INFO("LEAVE stopOutput(): output %d, stream %s\n", output, stream2str(stream));
    UNLOCK_MUTEX(mMutexDevState);

    return NO_ERROR;
}

void AudioPolicyManagerANM::releaseOutput(audio_io_handle_t output)
{
    ALOG_INFO("ENTER releaseOutput(): output %d", output);

    /* Check if the output is a valid output */
    ssize_t index = mOutputs.indexOfKey(output);
    if (index < 0) {
        ALOG_WARN("LEAVE releaseOutput(): Unknown output %d!\n", output);
        return;
    } else {
        AudioOutputDescriptor *descr = mOutputs.valueAt(index);
        AudioPolicyManagerANM::routing_strategy strategy = getOutputStrategy(output);

        if(descr->mStrategyRefCount==1) {
            if(!(descr->isUsedByStrategy(strategy))) {
#ifndef STD_A2DP_MNGT
/* don't close output because of audio output caches at two levels (AudioSystem and AudioPolicyManager)
--> reference counter of AudioPolicyManager and AudioSystem maybe not in sync */
                //mpClientInterface->closeOutput(output);
                //delete mOutputs.valueAt(index);
                //mOutputs.removeItem(output);
                ALOG_INFO("releaseOutput(): don't release output (output is still opened) mStrategyRefCount=0 [output=%d]\n", output);
#else
                if (mDuplicatedOutput == output) {
                    AudioOutputDescriptor *dupOutputDesc = mOutputs.valueFor(mDuplicatedOutput);
                    AudioOutputDescriptor *hwOutputDesc = dupOutputDesc->mOutput1;
                    // As all active tracks on duplicated output will be deleted,
                    // and as they were also referenced on hardware output, the reference
                    // count for their stream type must be adjusted accordingly on
                    // hardware output.
                    for (int i = 0; i < (int)AudioSystem::NUM_STREAM_TYPES; i++) {
                        int refCount = dupOutputDesc->mRefCount[i];
                        hwOutputDesc->changeRefCount((AudioSystem::stream_type)i,-refCount);
                    }
                    updateOutputRouting(true);
                    hwOutputDesc->mStrategyRefCount -= dupOutputDesc->mStrategyRefCount;
                    ALOG_INFO("releaseOutput() for output %d, mStrategyRefCount =%d\n",
                            dupOutputDesc->mId1, hwOutputDesc->mStrategyRefCount);
                }
#endif
            } else {
                    ALOG_INFO("releaseOutput() output still opened, because used by streams");
            }
        } else {
#ifdef STD_A2DP_MNGT
            if (descr->isDuplicated()) {
                AudioOutputDescriptor *dupOutputDesc = descr;
                AudioOutputDescriptor *hwOutputDesc = dupOutputDesc->mOutput1;
                // As all active tracks on duplicated output will be deleted,
                // and as they were also referenced on hardware output, the reference
                // count for their stream type must be adjusted accordingly on
                // hardware output.
                for (int i = 0; i < (int)AudioSystem::NUM_STREAM_TYPES; i++) {
                    int refCount = dupOutputDesc->mRefCount[i];
                    hwOutputDesc->changeRefCount((AudioSystem::stream_type)i,-refCount);
                }
                hwOutputDesc->mStrategyRefCount--;
                ALOG_INFO("releaseOutput() for output %d, mStrategyRefCount =%d\n", dupOutputDesc->mId1, hwOutputDesc->mStrategyRefCount);
            }
#endif
            LOCK_MUTEX(mMutexIO);
            descr->mStrategyRefCount--;
            ALOG_INFO("releaseOutput(): output %d still opened, mStrategyRefCount=%d ", output, descr->mStrategyRefCount);
            if(!(descr->isUsedByStrategy(strategy))) {
                if (descr->mSuspended == true ) {
                    ALOG_INFO("releaseOutput(): output %d is stopped and suspended. Do nothing", output);
                } else {
                    ALOG_INFO("releaseOutput(): output %d is stopped, suspend output and make sure device is in standby", output);
                    mpClientInterface->suspendOutput(output);
                    descr->mSuspended=true;
                    AudioParameter param = AudioParameter();
                    param.addInt(String8(PARAM_KEY_STREAM_STANDBY), 1);
                    mpClientInterface->setParameters(output, param.toString(), 0);
                }
            }
            UNLOCK_MUTEX(mMutexIO);
        }

        if(mOutputs.isEmpty()) {
            ALOG_INFO("releaseOutput(): No more opened outputs");
        }
    }

    updateOutputRouting();

    ALOG_INFO("LEAVE releaseOutput(): output %d", output);
}

/* ---------------------------------------------------------------------------
 * Input handling
 * ---------------------------------------------------------------------------*/
audio_io_handle_t AudioPolicyManagerANM::getInput(
    int source,
    uint32_t samplingRate,
    uint32_t format,
    uint32_t channels,
    AudioSystem::audio_in_acoustics acoustics,
    audio_input_clients* inputClient)
{
    audio_io_handle_t input = 0;
    uint32_t device;

    (void) ste_anm_debug_setup_log();

    ALOG_INFO("ENTER getInput(): %s, samplerate=%d, format=%d, "
        "channels=%x, acoustics=%x, inputClient=%p\n",
        source2str(source), samplingRate, format, channels,
        acoustics, inputClient);

    /* Check the format. Only supports PCM */
    if (format != 0 && !AudioSystem::isLinearPCM(format)) {
        ALOG_ERR("LEAVE getInput(): Format %d is not supported", format);
        return 0;
    }

    /* Check number of channels */
    if (channels != 0 &&
        channels != AudioSystem::CHANNEL_IN_MONO &&
        channels != AudioSystem::CHANNEL_IN_STEREO) {
        ALOG_ERR("LEAVE getInput(): Channels=%x is not supported", channels);
        return 0;
    }

    /* Check input values */
    if (format == 0) {
        format = DEFAULT_PCM_FORMAT;
        ALOG_INFO_VERBOSE("getInput(): Format is 0 - update to %x", format);
    }
    if (channels == 0) {
        channels = DEFAULT_INPUT_CHANNELS;
        ALOG_INFO_VERBOSE("getInput(): Channels is 0 - update to %x", channels);
    }
    if (samplingRate == 0) {
        samplingRate = DEFAULT_INPUT_SAMPLE_RATE;
        ALOG_INFO_VERBOSE("getInput(): Sample rate is 0 - update to %d", samplingRate);
    }

    /* Open new hardware input */
    AudioInputDescriptor *descr = new AudioInputDescriptor();

    descr->mDevice = getInputDeviceForSource(
        source, AudioSystem::popCount(channels));
    if (!descr->mDevice) {
        ALOG_ERR("LEAVE getInput(): No input device for source %d!", source);
        return 0;
    }
    descr->mSamplingRate = samplingRate;
    descr->mFormat = format;
    descr->mChannels = channels;
    descr->mAcoustics = acoustics;
    descr->mRefCount = 0;
    descr->mInputSource = source;
    descr->mInputClient = (uint32_t*)inputClient;

    /* Set recording mode to ADM if necessary */
    if(source == AUDIO_SOURCE_VOICE_CALL || AUDIO_SOURCE_VOICE_UPLINK ||
       source == AUDIO_SOURCE_VOICE_DOWNLINK)
    {
        ste_adm_dictaphone_mode_t mode = STE_ADM_DICTAPHONE_UPLINK_DOWNLINK;

        if(source == AUDIO_SOURCE_VOICE_DOWNLINK) {
            mode = STE_ADM_DICTAPHONE_DOWNLINK;
        }

        if(source == AUDIO_SOURCE_VOICE_UPLINK) {
            mode = STE_ADM_DICTAPHONE_UPLINK;
        }

        ste_adm_client_set_cscall_dictaphone_mode(mode);
    }

    input = mpClientInterface->openInput(
        &descr->mDevice, &descr->mSamplingRate, &descr->mFormat,
        &descr->mChannels, descr->mAcoustics, descr->mInputClient);

    if (input) {
        ALOG_INFO("getInput(): Opened new input %d for %s",
            input, source2str(source));
        /* Add to the list of available inputs */
        mInputs.add(input, descr);
    } else {
        ALOG_ERR("LEAVE getInput(): Failed to open input, %s, device %x,"
            " SR (%d) F (%x) C (%x)\n",
            source2str(source), descr->mDevice, descr->mSamplingRate,
            descr->mFormat, descr->mChannels);
        return 0;
    }

    ALOG_INFO("LEAVE getInput(): Returned input %d, %s, device=%x, "
        "SR (%d) F (%x) C (%x)\n",
        input, source2str(source), descr->mDevice, descr->mSamplingRate,
        descr->mFormat, descr->mChannels);

    return input;
}

status_t AudioPolicyManagerANM::startInput(audio_io_handle_t input)
{
    ALOG_INFO("ENTER startInput(): input %d\n", input);

    LOCK_MUTEX(mMutexDevState);

    ssize_t index = mInputs.indexOfKey(input);
    if (index < 0) {
        ALOG_WARN("LEAVE startInput(): Unknown input %d\n", input);
        UNLOCK_MUTEX(mMutexDevState);
        return BAD_VALUE;
    }

    /* Incremenent ref count of this stream on the requested input */
    AudioInputDescriptor *descr = mInputs.valueFor(input);
    descr->mRefCount++;
    if (descr->mRefCount > 1) {
        ALOG_INFO("LEAVE startInput(): input already started, do nothing");
        UNLOCK_MUTEX(mMutexDevState);
        return NO_ERROR;
    }

    /* Check VoIP conditions */
    bool outputAvailable = false;
    AudioOutputDescriptor *outDescr = NULL;
    if (isVoIPInput(descr, &outputAvailable, &outDescr)) {

        ALOG_INFO("startInput(): VoIP of type=%s\n",
            voiptype2str(descr->mVoIPType));

        /* Check if VoIP output should be synced, i.e. if the call graph
         * should only be setup when both input and output are started */
        bool syncedOutput = isVoIPSynced(descr->mVoIPType);

        if (syncedOutput && outputAvailable) {
            ALOG_INFO("startInput(): routing input and output to VoIP devices\n");
            setVoIPStatus(true, descr->mSamplingRate, descr->mChannels, outDescr->mSamplingRate, outDescr->mChannels);

            /* Update VoIP info */
            descr->mVoIPActive = true;
            outDescr->mVoIPActive = true;
            outDescr->mVoIPType = descr->mVoIPType;
            /* Setup the call graph */
            updateCSCallRouting();
            /* Route output to VoIP device */
            updateOutputRouting();

        } else if (syncedOutput) {
            ALOG_INFO("startInput(): VoIP input in synced mode, wait for output to switch to VoIP\n");
        } else {
            ALOG_INFO("startInput(): routing input to VoIP device\n");
            if (outputAvailable) {
                setVoIPStatus(true, descr->mSamplingRate, descr->mChannels, outDescr->mSamplingRate, outDescr->mChannels);
            } else {
                setVoIPStatus(true, descr->mSamplingRate, descr->mChannels, 0, 0);
            }
            /* Update VoIP info */
            descr->mVoIPActive = true;
            /* Setup the call graph */
            updateCSCallRouting();
        }
    }

    /* Update the input routing */
    updateInputRouting();

    /* Set stream state parameter */
    AudioParameter param = AudioParameter();
    param.addInt(String8(PARAM_KEY_STREAM_STATE), 1);
    mpClientInterface->setParameters(input, param.toString(), 0);
    /* Check if stream was started OK */
    String8 streamState;
    streamState = mpClientInterface->getParameters(input, String8(PARAM_KEY_STREAM_STATE));
    AudioParameter streamStateParam = AudioParameter(streamState);
    int val;
    if (streamStateParam.getInt(String8(PARAM_KEY_STREAM_STATE), val) == NO_ERROR) {
        if (val != 1) {
            ALOG_ERR("LEAVE startInput(): setParameter for ste_stream_state failed for input %d\n", input);
            UNLOCK_MUTEX(mMutexDevState);
            return UNKNOWN_ERROR;
        }
    }

    ALOG_INFO("LEAVE startInput(): input %d\n", input);
    UNLOCK_MUTEX(mMutexDevState);

    return NO_ERROR;
}

status_t AudioPolicyManagerANM::stopInput(audio_io_handle_t input)
{
    ALOG_INFO("ENTER stopInput(): input %d\n", input);

    LOCK_MUTEX(mMutexDevState);

    ssize_t index = mInputs.indexOfKey(input);
    if (index < 0) {
        ALOG_WARN("LEAVE stopInput(): Unknown input %d\n", input);
        UNLOCK_MUTEX(mMutexDevState);
        return BAD_VALUE;
    }

    AudioInputDescriptor *descr = mInputs.valueAt(index);

    if (descr->mRefCount > 0) {
        /* Decrement ref count of the input */
        descr->mRefCount--;
    } else {
        ALOG_WARN("LEAVE stopInput(): input %d already stopped\n", input);
        UNLOCK_MUTEX(mMutexDevState);
        return INVALID_OPERATION;
    }

    if (descr->mRefCount > 0) {
        ALOG_INFO("LEAVE stopInput(): Ref count is %d, do nothing", descr->mRefCount);
        UNLOCK_MUTEX(mMutexDevState);
        return NO_ERROR;
    }

    /* Check if VoIP is active */
    if (descr->mVoIPActive) {
        ALOG_INFO("stopInput(): VoIP is active for input %d\n", input);

        /* Check if VoIP output should be synced, i.e. if the call graph
         * should be closed regardless if output is active or not */
        bool syncedOutput = isVoIPSynced(descr->mVoIPType);

        /* Check if active VoIP output exists */
        bool outputFound = false;
        AudioOutputDescriptor *outDescr;
        for (size_t i = 0; i < mOutputs.size(); i++) {
            audio_io_handle_t output = mOutputs.keyAt(i);
            AudioOutputDescriptor *tmpDescr = mOutputs.valueFor(output);
            if (tmpDescr != NULL) {
                if(tmpDescr->mVoIPActive) {
                    outputFound = true;
                    outDescr = tmpDescr;
                    break;
                }
            }
        }

        /* Reroute VoIP input. Check if VoIP output should be
         * rerouted and if VoIP call graph should be closed */
        ALOG_INFO("stopInput(): synced=%d, activeOutput=%d\n", syncedOutput, outputFound);
        ALOG_INFO("stopInput(): reroute VoIP input\n");
        descr->mVoIPActive = false;
        updateInputRouting();
        if (syncedOutput || !outputFound) {
            if (outputFound) {
                ALOG_INFO("stopInput(): reroute VoIP output\n");
                outDescr->mVoIPActive = false;
                updateOutputRouting();
            }
            ALOG_INFO("stopInput(): close VoIP call graph\n");
            closeCSCall();
            setVoIPStatus(false, 0,0,0,0);
        }
    }

    if (!descr->mRefCount) {
        /* Loop through the in devices (the last 16 bits in audio_devices) */
        for (size_t i=0x10000 ; i>0 && i<=0x80000000 ; i=i<<1 ) {
            if (descr->mDevice & i) {
                uint32_t indexTopLevel = getTopLevelDeviceIndex(i);
                if (mCurrentSource[indexTopLevel] == descr->mInputSource &&
                    !(descr->mInputSource==AUDIO_SOURCE_VOICE_CALL_NO_RECORD && mPhoneState==AudioSystem::MODE_IN_CALL)) {
                    /* Invalidate current source for device */
                    mCurrentSource[indexTopLevel] = AUDIO_SOURCE_UNKNOWN;
                    updateInputRouting();
                }
            }
        }
    }

    /* Set stream state parameter */
    AudioParameter param = AudioParameter();
    param.addInt(String8(PARAM_KEY_STREAM_STATE), 0);
    mpClientInterface->setParameters(input, param.toString(), 0);

    ALOG_INFO("LEAVE stopInput(): input %d\n", input);
    UNLOCK_MUTEX(mMutexDevState);

    return NO_ERROR;
}

void AudioPolicyManagerANM::releaseInput(audio_io_handle_t input)
{
    ALOG_INFO("ENTER releaseInput(): input %d", input);

    /* Check if the output is a valid input */
    ssize_t index = mInputs.indexOfKey(input);
    if (index < 0) {
        ALOG_WARN("LEAVE releaseInput(): Unknown input %d!\n", input);
        return;
    } else {
        AudioInputDescriptor *descr = mInputs.valueAt(index);

        mpClientInterface->closeInput(input,descr->mInputClient);
        delete mInputs.valueAt(index);
        mInputs.removeItem(input);
        ALOG_INFO("releaseInput(): Successfully released input %d\n", input);
        if(mInputs.isEmpty()) {
            ALOG_INFO("releaseInput(): No more opened inputs");
        }
    }

    ALOG_INFO("LEAVE releaseInput(): input %d", input);
}

/* ---------------------------------------------------------------------------
 * Volume handling
 * ---------------------------------------------------------------------------*/
void AudioPolicyManagerANM::initStreamVolume(AudioSystem::stream_type stream,
        int indexMin,
        int indexMax)
{
    ALOG_INFO_VERBOSE("initStreamVolume(): stream %s, min %d, max %d\n",
        stream2str(stream), indexMin, indexMax);
    mStreams[stream].mIndexMin = indexMin;
    mStreams[stream].mIndexMax = indexMax;
}

status_t AudioPolicyManagerANM::setStreamVolumeIndex(
    AudioSystem::stream_type stream, int index)
{
    float volume = 0;
    uint32_t device = 0;

    if ((index < mStreams[stream].mIndexMin) ||
        (index > mStreams[stream].mIndexMax)) {
        return BAD_VALUE;
    }

    ALOG_INFO_VERBOSE("setStreamVolumeIndex(): stream %s, index %d\n",
        stream2str(stream), index);
    mStreams[stream].mIndexCur = index;

    /* Do not change actual stream volume if the stream is muted */
    if (mStreams[stream].mMuteCount != 0) {
        return NO_ERROR;
    }

    if (stream == AudioSystem::VOICE_CALL ||
        stream == AudioSystem::BLUETOOTH_SCO){
        if ((mModemType == ADM_FAT_MODEM) || (mModemType == ADM_FULL_FAT_MODEM)) {
            /* Apply voice call volume */
            ste_adm_client_init_cscall_downstream_volume(mStreams[stream].mIndexMin, mStreams[stream].mIndexMax);
            ALOG_INFO_VERBOSE("setStreamVolumeIndex(): setVoiceVolume() for stream %s, index volume %d\n",
                stream2str(stream), index);
            mpClientInterface->setVoiceVolume(index);
        } else {
            /* Compute and apply voice call volume */
            volume = computeVolume((int)stream, index, device);
            ALOG_INFO_VERBOSE("setStreamVolumeIndex(): setVoiceVolume() for stream %s, volume %f\n",
                stream2str(stream), volume);
            mpClientInterface->setVoiceVolume(volume);
        }
    } else {
        /* Compute and apply stream volume on all outputs */
        for (size_t i = 0; i < mOutputs.size(); i++) {
            AudioOutputDescriptor *outputDesc = mOutputs.valueAt(i);
            device = outputDesc->device();
            volume = computeVolume((int)stream, index, device);
            ALOG_INFO_VERBOSE("setStreamVolumeIndex(): setStreamVolume() for output %d stream %s, volume %f\n",
                mOutputs.keyAt(i), stream2str(stream), volume);
            mpClientInterface->setStreamVolume(stream, volume, mOutputs.keyAt(i));
        }
    }
    return NO_ERROR;
}

status_t AudioPolicyManagerANM::getStreamVolumeIndex(
    AudioSystem::stream_type stream, int *index)
{
    if (index == 0) {
        return BAD_VALUE;
    }
    ALOG_INFO_VERBOSE("getStreamVolumeIndex(): stream %s\n", stream2str(stream));
    *index =  mStreams[stream].mIndexCur;
    return NO_ERROR;
}

status_t AudioPolicyManagerANM::dump(int fd)
{
    const size_t SIZE = 256;
    char buffer[SIZE];
    String8 result;
    snprintf(buffer, SIZE, "\nAudioPolicyManager Dump: %p\n", this);
    result.append(buffer);
#ifdef STD_A2DP_MNGT
    snprintf(buffer, SIZE, " A2DP Output: %d\n", mA2dpOutput);
    result.append(buffer);
    snprintf(buffer, SIZE, " Duplicated Output: %d\n", mDuplicatedOutput);
    result.append(buffer);
    snprintf(buffer, SIZE, " A2DP device address: %s\n", mA2dpDeviceAddress.string());
    result.append(buffer);
#endif
    snprintf(buffer, SIZE, " Output devices: %08x\n", mAvailableOutputDevices);
    result.append(buffer);
    snprintf(buffer, SIZE, " Input devices: %08x\n", mAvailableInputDevices);
    result.append(buffer);
    snprintf(buffer, SIZE, " Phone state: %d\n", mPhoneState);
    result.append(buffer);
    snprintf(buffer, SIZE, " Ringer mode: %d\n", mRingerMode);
    result.append(buffer);
    snprintf(buffer, SIZE, " Force use for communication %d\n",
        mForceUse[AudioSystem::FOR_COMMUNICATION]);
    result.append(buffer);
    snprintf(buffer, SIZE, " Force use for media %d\n",
        mForceUse[AudioSystem::FOR_MEDIA]);
    result.append(buffer);
    snprintf(buffer, SIZE, " Force use for record %d\n",
        mForceUse[AudioSystem::FOR_RECORD]);
    result.append(buffer);
    write(fd, result.string(), result.size());

    snprintf(buffer, SIZE, "\nOutputs dump:\n");
    write(fd, buffer, strlen(buffer));
    for (size_t i = 0; i < mOutputs.size(); i++) {
        snprintf(buffer, SIZE, "- Output %d dump:\n", mOutputs.keyAt(i));
        write(fd, buffer, strlen(buffer));
        mOutputs.valueAt(i)->dump(fd);
    }

    snprintf(buffer, SIZE, "\nInputs dump:\n");
    write(fd, buffer, strlen(buffer));
    for (size_t i = 0; i < mInputs.size(); i++) {
        snprintf(buffer, SIZE, "- Input %d dump:\n", mInputs.keyAt(i));
        write(fd, buffer, strlen(buffer));
        mInputs.valueAt(i)->dump(fd);
    }

    snprintf(buffer, SIZE, "\nStreams dump:\n");
    write(fd, buffer, strlen(buffer));
    snprintf(buffer, SIZE, " Stream  Index Min  Index Max  Index Cur  Mute "
        "Count  Can be muted\n");
    write(fd, buffer, strlen(buffer));
    for (size_t i = 0; i < AudioSystem::NUM_STREAM_TYPES; i++) {
        snprintf(buffer, SIZE, " %02d", i);
        mStreams[i].dump(buffer + 3, SIZE);
        write(fd, buffer, strlen(buffer));
    }

    return NO_ERROR;
}


uint32_t AudioPolicyManagerANM::getStrategyForStream(AudioSystem::stream_type stream)
{
    return (uint32_t)getStrategy(stream);
}

uint32_t AudioPolicyManagerANM::getDevicesForStream(AudioSystem::stream_type stream)
{
    uint32_t devices;
    // By checking the range of stream before calling getStrategy, we avoid
    // getStrategy's behavior for invalid streams.  getStrategy would do a LOGE
    // and then return STRATEGY_MEDIA, but we want to return the empty set.
    if (stream < (AudioSystem::stream_type) 0 || stream >= AudioSystem::NUM_STREAM_TYPES) {
        devices = 0;
    } else {
        uint32_t strategy = getStrategyForStream(stream);
        devices = getOutputDeviceForStrategy(NULL,(AudioPolicyManagerANM::routing_strategy) strategy,AudioSystem::PCM_16_BIT);
    }
    return devices;
}

// Audio effect management
audio_io_handle_t AudioPolicyManagerANM::getOutputForEffect(effect_descriptor_t *desc)
{
    ALOG_INFO_VERBOSE("getOutputForEffect(): %s\n", desc->name);
    // apply simple rule where global effects are attached to the same output as MUSIC streams
    return getOutput(AudioSystem::MUSIC, 0, AudioSystem::FORMAT_DEFAULT, 0, AudioSystem::OUTPUT_FLAG_INDIRECT);
}

status_t AudioPolicyManagerANM::registerEffect(effect_descriptor_t *desc,
                                    audio_io_handle_t io,
                                    uint32_t strategy,
                                    int session,
                                    int id)
{
    ALOG_INFO("registerEffect(): id %d, io %d, strategy %d, session %d, name %s, CPU %d, memory %d",
            id, io, strategy, session, desc->name, desc->cpuLoad, desc->memoryUsage);

    ssize_t index = mOutputs.indexOfKey(io);
    if (index < 0) {
        index = mInputs.indexOfKey(io);
        if (index < 0) {
            ALOG_WARN("registerEffect(): unknown io %d", io);
            return INVALID_OPERATION;
        }
    }
    if (desc->cpuLoad > getMaxEffectsCpuLoad()) {
        ALOG_WARN("registerEffect(): CPU Load limit (%d) exceeded for Fx %s, CPU %f MIPS",
                getMaxEffectsCpuLoad(), desc->name, (float)desc->cpuLoad/10);
        return INVALID_OPERATION;
    }
    if (desc->memoryUsage > getMaxEffectsMemory()) {
        ALOG_WARN("registerEffect(): memory limit (%d) exceeded for Fx %s, Memory %d KB",
                getMaxEffectsMemory(), desc->name, desc->memoryUsage);
        return INVALID_OPERATION;
    }

    ALOG_INFO_VERBOSE("registerEffect(): Registration OK CPU %d, memory %d", desc->cpuLoad, desc->memoryUsage);
    ALOG_INFO_VERBOSE("  total CPU %d, total memory %d", mTotalEffectsCpuLoad, mTotalEffectsMemory);

    EffectDescriptor *pDesc = new EffectDescriptor();
    memcpy (&pDesc->mDesc, desc, sizeof(effect_descriptor_t));
    pDesc->mIo = io;
    pDesc->mStrategy = (routing_strategy)strategy;
    pDesc->mSession = session;
    mEffects.add(id, pDesc);

    return NO_ERROR;
}

status_t AudioPolicyManagerANM::unregisterEffect(int id)
{
    ALOG_INFO_VERBOSE("unregisterEffect(): effect ID %d", id);
    ssize_t index = mEffects.indexOfKey(id);
    if (index < 0) {
        ALOG_WARN("unregisterEffect(): unknown effect ID %d", id);
        return INVALID_OPERATION;
    }

    EffectDescriptor *pDesc = mEffects.valueAt(index);

    setEffectEnabled(pDesc, false);

    ALOG_INFO_VERBOSE("unregisterEffect(): effect %s, ID %d, CPU %d, memory %d",
            pDesc->mDesc.name, id, pDesc->mDesc.cpuLoad, pDesc->mDesc.memoryUsage);
    ALOG_INFO_VERBOSE("  total CPU %d, total memory %d", mTotalEffectsCpuLoad, mTotalEffectsMemory);

    mEffects.removeItem(id);
    delete pDesc;

    return NO_ERROR;
}


status_t AudioPolicyManagerANM::setEffectEnabled(int id, bool enabled)
{
    ssize_t index = mEffects.indexOfKey(id);
    if (index < 0) {
        ALOG_WARN("setEffectEnabled(): unknown effect ID %d", id);
        return INVALID_OPERATION;
    }

    return setEffectEnabled(mEffects.valueAt(index), enabled);
}

status_t AudioPolicyManagerANM::setEffectEnabled(EffectDescriptor *pDesc, bool enabled)
{
    if (enabled == pDesc->mEnabled) {
        ALOG_INFO_VERBOSE("setEffectEnabled(%s) effect already %s",
             enabled?"true":"false", enabled?"enabled":"disabled");
        return INVALID_OPERATION;
    }

    if (enabled) {
        if (mTotalEffectsCpuLoad + pDesc->mDesc.cpuLoad > getMaxEffectsCpuLoad()) {
            ALOG_WARN("setEffectEnabled(true) CPU Load limit exceeded for Fx %s, CPU %f MIPS",
                 pDesc->mDesc.name, (float)pDesc->mDesc.cpuLoad/10);
            return INVALID_OPERATION;
        }
        mTotalEffectsCpuLoad += pDesc->mDesc.cpuLoad;
        ALOG_INFO_VERBOSE("setEffectEnabled(true) total CPU %d", mTotalEffectsCpuLoad);

        if (mTotalEffectsMemory + pDesc->mDesc.memoryUsage > getMaxEffectsMemory()) {
            ALOG_WARN("setEffectEnabled(true) memory limit (%d) exceeded for Fx %s, Memory %d KB",
                getMaxEffectsMemory(), pDesc->mDesc.name, pDesc->mDesc.memoryUsage);
            return INVALID_OPERATION;
        }
        mTotalEffectsMemory += pDesc->mDesc.memoryUsage;
        ALOG_INFO_VERBOSE("setEffectEnabled(true) total Memory %d", mTotalEffectsMemory);
    } else {
        if (mTotalEffectsCpuLoad < pDesc->mDesc.cpuLoad) {
            ALOG_WARN("setEffectEnabled(false) CPU load %d too high for total %d",
                    pDesc->mDesc.cpuLoad, mTotalEffectsCpuLoad);
            pDesc->mDesc.cpuLoad = mTotalEffectsCpuLoad;
        }
        mTotalEffectsCpuLoad -= pDesc->mDesc.cpuLoad;
        ALOG_INFO_VERBOSE("setEffectEnabled(false) total CPU %d", mTotalEffectsCpuLoad);

        if (mTotalEffectsMemory < pDesc->mDesc.memoryUsage) {
            ALOG_WARN("setEffectEnabled(false) memory %d too big for total %d",
                pDesc->mDesc.memoryUsage, mTotalEffectsMemory);
            pDesc->mDesc.memoryUsage = mTotalEffectsMemory;
        }
        mTotalEffectsMemory -= pDesc->mDesc.memoryUsage;
        ALOG_INFO_VERBOSE("setEffectEnabled(false) total Memory %d", mTotalEffectsMemory);
    }
    pDesc->mEnabled = enabled;
    return NO_ERROR;
}

bool AudioPolicyManagerANM::isStreamActive(int stream, uint32_t inPastMs) const
{
    nsecs_t sysTime = systemTime();
    for (size_t i = 0; i < mOutputs.size(); i++) {
        if (mOutputs.valueAt(i)->mRefCount[stream] != 0 ||
                ns2ms(sysTime - mOutputs.valueAt(i)->mStopTime[stream]) < inPastMs) {
            return true;
        }
    }
    return false;
}

/* ---------------------------------------------------------------------------
 * AudioPolicyManagerANM
 * ---------------------------------------------------------------------------*/
extern "C" AudioPolicyInterface* createAudioPolicyManager(
    AudioPolicyClientInterface *clientInterface)
{
    return new AudioPolicyManagerANM(clientInterface);
}

extern "C" void destroyAudioPolicyManager(AudioPolicyInterface *interface)
{
    delete interface;
}

AudioPolicyManagerANM::AudioPolicyManagerANM(
    AudioPolicyClientInterface *clientInterface) :
        mRingerMode(0), mCSCallActive(false), mVoIPActive(false), mForceUseOngoing(false),
        mTotalEffectsCpuLoad(0), mTotalEffectsMemory(0), mModemType(ADM_UNKNOWN_MODEM_TYPE)
#ifdef STD_A2DP_MNGT
      , mA2dpSuspended(false)
#endif
{
    mpClientInterface = clientInterface;

    (void) ste_anm_debug_setup_log();

    ALOG_INFO("AudioPolicyManagerANM(): Created, %p", this);
    fd_adm = -1;

    int retval = pthread_mutex_init(&mMutexForceUse, 0);
    if (retval != 0) {
        ALOG_ERR("AudioPolicyManagerANM(): pthread_mutex_init failed, error = %d", retval);
    }

    retval = pthread_mutex_init(&mMutexCallDevices, 0);
    if (retval != 0) {
        ALOG_ERR("AudioPolicyManagerANM(): pthread_mutex_init failed, error = %d", retval);
    }

    retval = pthread_mutex_init(&mMutexCallStatus, 0);
    if (retval != 0) {
        ALOG_ERR("AudioPolicyManagerANM(): pthread_mutex_init failed, error = %d", retval);
    }

    retval = pthread_mutex_init(&mMutexVC, 0);
    if (retval != 0) {
        ALOG_ERR("AudioPolicyManagerANM(): pthread_mutex_init failed, error = %d", retval);
    }

    retval = pthread_mutex_init(&mMutexDevState, 0);
    if (retval != 0) {
        ALOG_ERR("pthread_mutex_init failed, error = %d\n", retval);
    }

    retval = pthread_mutex_init(&mMutexIO, 0);
    if (retval != 0) {
        ALOG_ERR("AudioPolicyManagerANM(): pthread_mutex_init failed, error = %d", retval);
    }

    for (int i = 0; i < AudioSystem::NUM_FORCE_USE; i++) {
        mForceUse[i] = AudioSystem::FORCE_NONE;
    }

    for (int i=0; i<NUM_DEV_OUT_INDEXES; i++) {
        mCurrentStrategy[i]=STRATEGY_UNKNOWN;
    }
    for (int i=0; i<NUM_DEV_IN_INDEXES; i++) {
        mCurrentSource[i]=AUDIO_SOURCE_UNKNOWN;
    }

    /* Set default input and output devices */
    mAvailableOutputDevices = defaultOutputDevices;
    mAvailableInputDevices  = defaultInputDevices;
#ifdef STD_A2DP_MNGT
    mA2dpOutput = 0;
    mDuplicatedOutput = 0;
    mA2dpDeviceAddress = String8("");
    updateDeviceForStrategy();
    getOutput( AudioSystem::VOICE_CALL, 0, 0, 0, (AudioSystem::output_flags)0);
    getOutput( AudioSystem::DTMF, 0, 0, 0, (AudioSystem::output_flags)0);
    getOutput( AudioSystem::MUSIC, 0, 0, 0, (AudioSystem::output_flags)0);
    if (a2dpUsedForSonification()) {
        getOutput( AudioSystem::RING, 0, 0, 0, (AudioSystem::output_flags)0);
    }
#endif

    ALOG_INFO_VERBOSE("AudioPolicyManagerANM(): "
        "Default available devices: output = %x, input = %x)",
        mAvailableOutputDevices, mAvailableOutputDevices);

    /* Set Audio Policy Manager in AHI */
    AudioParameter param = AudioParameter();
    int ptr = (int)this;
    param.addInt(String8(PARAM_KEY_AP_REF), ptr);
    mpClientInterface->setParameters(0, param.toString(), 0);

    /* Subscribe to ADM Modem Status */
    subscribeModemStatus();

    if (ste_adm_client_get_modem_type(&mModemType) != STE_ADM_RES_OK) {
        ALOG_ERR("AudioPolicyManagerANM::AudioPolicyManagerANM(): Impossible to get modem type!\n");
    }
}

AudioPolicyManagerANM::~AudioPolicyManagerANM()
{
    ALOG_INFO("AudioPolicyManagerANM(): Destroyed");

    for (size_t i = 0; i < mOutputs.size(); i++) {
        mpClientInterface->closeOutput(mOutputs.keyAt(i));
        delete mOutputs.valueAt(i);
    }
    mOutputs.clear();
    for (size_t i = 0; i < mInputs.size(); i++) {
        mpClientInterface->closeInput(mInputs.keyAt(i));
        delete mInputs.valueAt(i);
    }
    mInputs.clear();

    pthread_mutex_destroy(&mMutexForceUse);
    pthread_mutex_destroy(&mMutexCallDevices);
    pthread_mutex_destroy(&mMutexCallStatus);
    pthread_mutex_destroy(&mMutexVC);
    pthread_mutex_destroy(&mMutexIO);
    pthread_mutex_destroy(&mMutexDevState);
}

AudioPolicyManagerANM::routing_strategy AudioPolicyManagerANM::getStrategy(
    AudioSystem::stream_type stream)
{
    AudioPolicyManagerANM::routing_strategy strategy;

    switch (stream) {

    case AudioSystem::VOICE_CALL:
    case AudioSystem::SPEECH_PROC:
#ifdef STE_VIDEO_CALL
    case AudioSystem::VIDEO_CALL:
#endif
    case AudioSystem::BLUETOOTH_SCO:
        strategy = STRATEGY_PHONE;
        break;

    case AudioSystem::RING:
    case AudioSystem::NOTIFICATION:
    case AudioSystem::ALARM:
    case AudioSystem::ENFORCED_AUDIBLE:
        strategy = STRATEGY_SONIFICATION;
        break;

    case AudioSystem::DTMF:
        strategy = STRATEGY_DTMF;
        break;

    case AudioSystem::MUSIC:
    case AudioSystem::SYSTEM:
    case AudioSystem::TTS:
    default:
        strategy = STRATEGY_MEDIA;
        break;
    }

    return strategy;
}

float AudioPolicyManagerANM::computeVolume(
    int stream, int index, uint32_t device)
{
    float volume = 1.0;

    StreamDescriptor &streamDesc = mStreams[stream];

    /* Force max volume if stream cannot be muted */
    if (!streamDesc.mCanBeMuted) index = streamDesc.mIndexMax;

    int indexRelative = index - streamDesc.mIndexMin;
    int indexRange = streamDesc.mIndexMax - streamDesc.mIndexMin;

    if (stream == AudioSystem::VOICE_CALL ||
        stream == AudioSystem::BLUETOOTH_SCO){
        volume = (float)indexRelative / (float)indexRange;
    } else {
        int volInt = (100 * indexRelative) / indexRange;

        // For DTMF index zero don't mean that volume should be zero.
        // Setting volInt to lowest possible value
        if( (volInt <= 0) && (stream == AudioSystem::DTMF) ) {
            volInt = (100 * 1) / indexRange;
        }

        volume = android::AudioSystem::linearToLog(volInt);
    }

    return volume;
}

void AudioPolicyManagerANM::setStreamMute(int stream, bool on,
    audio_io_handle_t output)
{
    ALOG_INFO_VERBOSE("setStreamMute(): stream %d, mute %d, output %d", stream, on, output);

    StreamDescriptor &streamDesc = mStreams[stream];

    if (on) {
        if (streamDesc.mMuteCount++ == 0) {
            if (streamDesc.mCanBeMuted) {
                mpClientInterface->setStreamVolume(
                    (AudioSystem::stream_type)stream, 0, output);
            }
        }
    } else {
        if (streamDesc.mMuteCount == 0) {
            ALOG_WARN("setStreamMute(): unmuting non muted stream!");
            return;
        }
        if (--streamDesc.mMuteCount == 0) {
            uint32_t device = mOutputs.valueFor(output)->mDevice;
            float volume = computeVolume(stream, streamDesc.mIndexCur, device);
            mpClientInterface->setStreamVolume(
                (AudioSystem::stream_type)stream, volume, output);
        }
    }
}

uint32_t AudioPolicyManagerANM::getMaxEffectsCpuLoad()
{
    return MAX_EFFECTS_CPU_LOAD;
}

uint32_t AudioPolicyManagerANM::getMaxEffectsMemory()
{
    return MAX_EFFECTS_MEMORY;
}


/* AudioOutputDescriptor class implementation --------------------------------*/

AudioPolicyManagerANM::AudioOutputDescriptor::AudioOutputDescriptor()
        : mSamplingRate(0), mFormat(0), mChannels(0), mLatency(0),
        mFlags((AudioSystem::output_flags)0), mDevice(0), mStrategyRefCount(0),
        mAudioOutputRef(0), mVoIPActive(false), mSuspended(false)
#ifdef STD_A2DP_MNGT
        , mId1(0), mOutput1(0), mOutput2(0)
#endif
{
    /* Clear usage count for all stream types */
    for (int i = 0; i < AudioSystem::NUM_STREAM_TYPES; i++) {
        mRefCount[i] = 0;
        mStopTime[i] = 0;
    }
}

uint32_t AudioPolicyManagerANM::AudioOutputDescriptor::device()
{
 #ifdef STD_A2DP_MNGT
    if (isDuplicated()) {
        return (mOutput1->mDevice | mOutput2->mDevice);
    }
#endif
    return mDevice;
}

void AudioPolicyManagerANM::AudioOutputDescriptor::changeRefCount(
    AudioSystem::stream_type stream, int delta)
{
#ifdef STD_A2DP_MNGT
    // forward usage count change to attached outputs
    if (isDuplicated()) {
        ALOG_INFO_VERBOSE("changeRefCount() for adm output:");
        mOutput1->changeRefCount(stream, delta);
        ALOG_INFO_VERBOSE("changeRefCount() for a2dp output:");
        mOutput2->changeRefCount(stream, delta);
        ALOG_INFO_VERBOSE("changeRefCount() for duplicated output:");
    }
#endif
    if ((delta + (int)mRefCount[stream]) < 0) {
        ALOG_WARN("changeRefCount(): invalid delta %d for stream %s, refCount %d",
            delta, stream2str(stream), mRefCount[stream]);
        mRefCount[stream] = 0;
        return;
    }
    mRefCount[stream] += delta;
    ALOG_INFO_VERBOSE("changeRefCount(): stream %s, count %d",
        stream2str(stream), mRefCount[stream]);
}

uint32_t AudioPolicyManagerANM::AudioOutputDescriptor::refCount()
{
    uint32_t refcount = 0;
    for (int i = 0; i < (int)AudioSystem::NUM_STREAM_TYPES; i++) {
        refcount += mRefCount[i];
    }
    return refcount;
}

uint32_t AudioPolicyManagerANM::AudioOutputDescriptor::strategyRefCount(
    routing_strategy strategy)
{
    uint32_t refCount = 0;
    for (int i = 0; i < (int)AudioSystem::NUM_STREAM_TYPES; i++) {
        if (getStrategy((AudioSystem::stream_type)i) == strategy) {
            refCount += mRefCount[i];
        }
    }
    return refCount;
}

status_t AudioPolicyManagerANM::AudioOutputDescriptor::dump(int fd)
{
    const size_t SIZE = 256;
    char buffer[SIZE];
    String8 result;

    snprintf(buffer, SIZE, " Sampling rate: %d\n", mSamplingRate);
    result.append(buffer);
    snprintf(buffer, SIZE, " Format: %d\n", mFormat);
    result.append(buffer);
    snprintf(buffer, SIZE, " Channels: %08x\n", mChannels);
    result.append(buffer);
    snprintf(buffer, SIZE, " Latency: %d\n", mLatency);
    result.append(buffer);
    snprintf(buffer, SIZE, " Flags %08x\n", mFlags);
    result.append(buffer);
    snprintf(buffer, SIZE, " Devices %08x\n", mDevice);
    result.append(buffer);
    snprintf(buffer, SIZE, " Stream refCount\n");
    result.append(buffer);
    for (int i = 0; i < AudioSystem::NUM_STREAM_TYPES; i++) {
        snprintf(buffer, SIZE, " %02d     %d\n", i, mRefCount[i]);
        result.append(buffer);
    }
    write(fd, result.string(), result.size());

    return NO_ERROR;
}

/* AudioInputDescriptor class implementation ---------------------------------*/

AudioPolicyManagerANM::AudioInputDescriptor::AudioInputDescriptor()
        : mSamplingRate(0), mFormat(0), mChannels(0),
        mAcoustics((AudioSystem::audio_in_acoustics)0), mDevice(0),
        mRefCount(0), mVoIPActive(false)
{
}

status_t AudioPolicyManagerANM::AudioInputDescriptor::dump(int fd)
{
    const size_t SIZE = 256;
    char buffer[SIZE];
    String8 result;

    snprintf(buffer, SIZE, " Sampling rate: %d\n", mSamplingRate);
    result.append(buffer);
    snprintf(buffer, SIZE, " Format: %d\n", mFormat);
    result.append(buffer);
    snprintf(buffer, SIZE, " Channels: %08x\n", mChannels);
    result.append(buffer);
    snprintf(buffer, SIZE, " Acoustics %08x\n", mAcoustics);
    result.append(buffer);
    snprintf(buffer, SIZE, " Devices %08x\n", mDevice);
    result.append(buffer);
    snprintf(buffer, SIZE, " Ref Count %d\n", mRefCount);
    result.append(buffer);
    write(fd, result.string(), result.size());

    return NO_ERROR;
}

/* StreamDescriptor class implementation -------------------------------------*/

void AudioPolicyManagerANM::StreamDescriptor::dump(char* buffer, size_t size)
{
    snprintf(buffer, size,
        "      %02d         %02d         %02d         %02d          %d\n",
        mIndexMin,
        mIndexMax,
        mIndexCur,
        mMuteCount,
        mCanBeMuted);
}

// --- EffectDescriptor class implementation

status_t AudioPolicyManagerANM::EffectDescriptor::dump(int fd)
{
    const size_t SIZE = 256;
    char buffer[SIZE];
    String8 result;

    snprintf(buffer, SIZE, " IO: %d\n", mIo);
    result.append(buffer);
    snprintf(buffer, SIZE, " Strategy: %d\n", mStrategy);
    result.append(buffer);
    snprintf(buffer, SIZE, " Session: %d\n", mSession);
    result.append(buffer);
    snprintf(buffer, SIZE, " Name: %s\n",  mDesc.name);
    result.append(buffer);
    write(fd, result.string(), result.size());

    return NO_ERROR;
}

}; // namespace android
