/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*! \file ste_anm_ap.c
*   \brief ST-Ericsson Audio Policy Manager

    ST-Ericsson implementation of the Audio Policy Manager
    to be used by Audio Flinger in Android framework.
*/
#define ANM_LOG_FILENAME "anm_ap"
#include "ste_hal_anm_dbg.h"
#include "ste_hal_anm_util.h"
#include "ste_hal_anm_ap.h"
#include "ste_anm_ext_hal.h"
#include <ste_adm_client.h>
#include <unistd.h>
#include <math.h>

/*Audio Policy Structures */
struct ste_audio_ap_module {
    struct audio_policy_module module;
};

struct ste_audio_ap_device {
    struct audio_policy_device device;
};

audio_io_handle_t ste_anm_ap_get_input(struct audio_policy *pol, audio_source_t source,
                            uint32_t samplingRate, audio_format_t format, uint32_t channels,
                            audio_in_acoustics_t acoustics);

#define DEFAULT_INPUT_SAMPLE_RATE       8000
#define DEFAULT_OUTPUT_SAMPLE_RATE      44100
#define DEFAULT_INPUT_CHANNELS          AUDIO_CHANNEL_IN_MONO
#define DEFAULT_OUTPUT_CHANNELS         AUDIO_CHANNEL_OUT_STEREO
#define DEFAULT_PCM_FORMAT              AUDIO_FORMAT_PCM_16_BIT
#define MAX_CSCALL_SAMPLE_RATE          16000
#define AUDIO_SOURCE_VOICE_CALL_NO_RECORD (AUDIO_SOURCE_MAX+1)
#define LOCK_MUTEX(mutex) \
{ \
    if (&(mutex) == &audio_pol_anm->mMutexForceUse) {\
        ALOG_INFO_VERBOSE("LOCK MUTEX FORCEUSE, Line=%d", __LINE__);\
    } else if (&(mutex) == &audio_pol_anm->mMutexCallDevices) {\
        ALOG_INFO_VERBOSE("LOCK MUTEX CALLDEVICES, Line=%d", __LINE__);\
    } else if (&(mutex) == &audio_pol_anm->mMutexCallStatus) {\
        ALOG_INFO_VERBOSE("LOCK MUTEX CALLSTATUS, Line=%d", __LINE__);\
    } else if (&(mutex) == &audio_pol_anm->mMutexVC) {\
        ALOG_INFO_VERBOSE("LOCK MUTEX VC, Line=%d", __LINE__);\
    } else if (&(mutex) == &audio_pol_anm->mMutexIO) {\
        ALOG_INFO_VERBOSE("LOCK MUTEX IO, Line=%d", __LINE__);\
    } else if (&(mutex) == &audio_pol_anm->mMutexDevState) {\
        ALOG_INFO_VERBOSE("LOCK MUTEX DEVICE STATE, Line=%d", __LINE__);\
    } else {\
        ALOG_INFO_VERBOSE("LOCK MUTEX, Line=%d", __LINE__);\
    }\
    pthread_mutex_lock(&(mutex));\
}
#define UNLOCK_MUTEX(mutex) \
{ \
    pthread_mutex_unlock(&(mutex));\
    if (&(mutex) == &audio_pol_anm->mMutexForceUse) {\
        ALOG_INFO_VERBOSE("UNLOCK MUTEX FORCEUSE, Line=%d", __LINE__);\
    } else if (&(mutex) == &audio_pol_anm->mMutexCallDevices) {\
        ALOG_INFO_VERBOSE("UNLOCK MUTEX CALLDEVICES, Line=%d", __LINE__);\
    } else if (&(mutex) == &audio_pol_anm->mMutexCallStatus) {\
        ALOG_INFO_VERBOSE("UNLOCK MUTEX CALLSTATUS, Line=%d", __LINE__);\
    } else if (&(mutex) == &audio_pol_anm->mMutexVC) {\
        ALOG_INFO_VERBOSE("UNLOCK MUTEX VC, Line=%d", __LINE__);\
    } else if (&(mutex) == &audio_pol_anm->mMutexIO) {\
        ALOG_INFO_VERBOSE("UNLOCK MUTEX IO, Line=%d", __LINE__);\
    } else if (&(mutex) == &audio_pol_anm->mMutexDevState) {\
        ALOG_INFO_VERBOSE("UNLOCK MUTEX DEVICE STATE, Line=%d", __LINE__);\
    } else {\
        ALOG_INFO_VERBOSE("UNLOCK MUTEX, Line=%d", __LINE__);\
    }\
}

#define DEVLIST_MAX_NO_DEVICES 16

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
 * Default available input and output devices
 * ---------------------------------------------------------------------------*/
uint32_t defaultOutputDevices = AUDIO_DEVICE_OUT_DEFAULT  |
                                AUDIO_DEVICE_OUT_EARPIECE |
                                AUDIO_DEVICE_OUT_SPEAKER  ;

uint32_t defaultInputDevices  = AUDIO_DEVICE_IN_DEFAULT       |
                                             AUDIO_DEVICE_IN_BUILTIN_MIC   |
                                             AUDIO_DEVICE_IN_FM_RX            |
                                             AUDIO_DEVICE_IN_VOICE_CALL    ;

/* ---------------------------------------------------------------------------
 * Output Device Prio Lists
 * ---------------------------------------------------------------------------*/
uint32_t mediaDevices[DEVLIST_MAX_NO_DEVICES] = {
     AUDIO_DEVICE_OUT_AUX_DIGITAL,
     AUDIO_DEVICE_OUT_BLUETOOTH_A2DP,
     AUDIO_DEVICE_OUT_BLUETOOTH_A2DP_HEADPHONES,
     AUDIO_DEVICE_OUT_FM_TX,
     AUDIO_DEVICE_OUT_WIRED_HEADPHONE,
     AUDIO_DEVICE_OUT_WIRED_HEADSET,
     AUDIO_DEVICE_OUT_DGTL_DOCK_HEADSET,
     AUDIO_DEVICE_OUT_USB_ACCESSORY,
     AUDIO_DEVICE_OUT_USB_DEVICE,
     AUDIO_DEVICE_OUT_SPEAKER,
     AUDIO_DEVICE_OUT_EARPIECE,
    0
};

uint32_t mediaInCallDevices[DEVLIST_MAX_NO_DEVICES] = {
    AUDIO_DEVICE_OUT_AUX_DIGITAL,
    AUDIO_DEVICE_OUT_BLUETOOTH_A2DP,
    AUDIO_DEVICE_OUT_BLUETOOTH_A2DP_HEADPHONES,
    AUDIO_DEVICE_OUT_FM_TX,
    AUDIO_DEVICE_OUT_WIRED_HEADPHONE,
    AUDIO_DEVICE_OUT_WIRED_HEADSET,
    AUDIO_DEVICE_OUT_EARPIECE,
    0
};

uint32_t sonificationDevices[DEVLIST_MAX_NO_DEVICES] = {
    AUDIO_DEVICE_OUT_SPEAKER | AUDIO_DEVICE_OUT_BLUETOOTH_A2DP_HEADPHONES,
    AUDIO_DEVICE_OUT_SPEAKER | AUDIO_DEVICE_OUT_BLUETOOTH_A2DP,
    AUDIO_DEVICE_OUT_SPEAKER | AUDIO_DEVICE_OUT_WIRED_HEADPHONE,
    AUDIO_DEVICE_OUT_SPEAKER | AUDIO_DEVICE_OUT_WIRED_HEADSET,
    AUDIO_DEVICE_OUT_SPEAKER | AUDIO_DEVICE_OUT_USB_ACCESSORY,
    AUDIO_DEVICE_OUT_SPEAKER | AUDIO_DEVICE_OUT_USB_DEVICE,
    AUDIO_DEVICE_OUT_SPEAKER | AUDIO_DEVICE_OUT_DGTL_DOCK_HEADSET,
    AUDIO_DEVICE_OUT_SPEAKER,
    0
};

uint32_t sonificationInCallDevices[DEVLIST_MAX_NO_DEVICES] = {
    AUDIO_DEVICE_OUT_SPEAKER | AUDIO_DEVICE_OUT_WIRED_HEADPHONE,
    AUDIO_DEVICE_OUT_SPEAKER | AUDIO_DEVICE_OUT_WIRED_HEADSET,
    AUDIO_DEVICE_OUT_SPEAKER,
    0
};

uint32_t phoneDevices[DEVLIST_MAX_NO_DEVICES] = {
    AUDIO_DEVICE_OUT_BLUETOOTH_A2DP,
    AUDIO_DEVICE_OUT_BLUETOOTH_A2DP_HEADPHONES,
    AUDIO_DEVICE_OUT_WIRED_HEADPHONE,
    AUDIO_DEVICE_OUT_WIRED_HEADSET,
    AUDIO_DEVICE_OUT_EARPIECE,
    0
};

uint32_t phoneInCallDevices[DEVLIST_MAX_NO_DEVICES] = {
    AUDIO_DEVICE_OUT_WIRED_HEADPHONE,
    AUDIO_DEVICE_OUT_WIRED_HEADSET,
    AUDIO_DEVICE_OUT_EARPIECE,
    0
};

/* ---------------------------------------------------------------------------
 * Input Device Prio Lists
 * ---------------------------------------------------------------------------*/

uint32_t phoneInputDevices[DEVLIST_MAX_NO_DEVICES] = {
    AUDIO_DEVICE_IN_WIRED_HEADSET,
    AUDIO_DEVICE_IN_BUILTIN_MIC,
    0
};

uint32_t phoneInCallRecordDevices[DEVLIST_MAX_NO_DEVICES] = {
    AUDIO_DEVICE_IN_VOICE_CALL,
    0
};

uint32_t camcorderInputDevices[DEVLIST_MAX_NO_DEVICES] = {
    AUDIO_DEVICE_IN_WIRED_HEADSET,
    AUDIO_DEVICE_IN_BUILTIN_MIC,
    0
};

uint32_t micStereoInputDevices[DEVLIST_MAX_NO_DEVICES] = {
    AUDIO_DEVICE_IN_WIRED_HEADSET,
    AUDIO_DEVICE_IN_BUILTIN_MIC,
    0
};

uint32_t micMonoInputDevices[DEVLIST_MAX_NO_DEVICES] = {
    AUDIO_DEVICE_IN_WIRED_HEADSET,
    AUDIO_DEVICE_IN_BUILTIN_MIC,
    0
};

uint32_t fmInputDevices[DEVLIST_MAX_NO_DEVICES] = {
    AUDIO_DEVICE_IN_FM_RX,
    0
};

/* ---------------------------------------------------------------------------
 * Strategy priority is used to select the preferred device
 * configuration when a device is used by several strategies.
 * ---------------------------------------------------------------------------*/
const int strategyPriority[NUM_STRATEGIES+1]=
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
const int sourcePriority[AUDIO_SOURCE_MAX+3]=
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

#ifdef STE_VIDEO_CALL
    3, // AUDIO_SOURCE_VIDEO_CALL
#endif
    5, // AUDIO_SOURCE_VOICE_CALL_NO_RECORD
    0  // AUDIO_SOURCE_UNKNOWN mapped to lowest prio
};



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
    AUDIO_DEVICE_OUT_WIRED_HEADPHONE,
    AUDIO_DEVICE_OUT_WIRED_HEADSET,
    AUDIO_DEVICE_OUT_SPEAKER,
    AUDIO_DEVICE_OUT_DGTL_DOCK_HEADSET,
    AUDIO_DEVICE_OUT_USB_ACCESSORY,
    AUDIO_DEVICE_OUT_USB_DEVICE,
    0
};

struct ste_audio_policy_manager {
    struct audio_policy       policy;
    audio_policy_anm *policy_anm;
};

/* ---------------------------------------------------------------------------
 * Type to string mapping functions
 * ---------------------------------------------------------------------------*/

const char* device2str(audio_devices_t dev)
{
    switch ((int)dev) {
        /* Output devices */
        case AUDIO_DEVICE_OUT_EARPIECE:          return "DEVICE_OUT_EARPIECE";
        case AUDIO_DEVICE_OUT_SPEAKER:           return "DEVICE_OUT_SPEAKER";
        case AUDIO_DEVICE_OUT_WIRED_HEADSET:     return "DEVICE_OUT_WIRED_HEADSET";
        case AUDIO_DEVICE_OUT_DGTL_DOCK_HEADSET: return "DEVICE_OUT_DGTL_DOCK_HEADSET";
        case AUDIO_DEVICE_OUT_USB_ACCESSORY:     return "DEVICE_OUT_USB_ACCESSORY";
        case AUDIO_DEVICE_OUT_USB_DEVICE:        return "DEVICE_OUT_USB_DEVICE";
        case AUDIO_DEVICE_OUT_WIRED_HEADPHONE:   return "DEVICE_OUT_WIRED_HEADPHONE";
        case AUDIO_DEVICE_OUT_SPEAKER|AUDIO_DEVICE_OUT_WIRED_HEADSET: return "DEVICE_OUT_SPEAKER, DEVICE_OUT_WIRED_HEADSET";
        case AUDIO_DEVICE_OUT_SPEAKER|AUDIO_DEVICE_OUT_WIRED_HEADPHONE: return "DEVICE_OUT_SPEAKER, DEVICE_OUT_WIRED_HEADPHONE";
        case AUDIO_DEVICE_OUT_BLUETOOTH_SCO:     return "DEVICE_OUT_BLUETOOTH_SCO";
        case AUDIO_DEVICE_OUT_BLUETOOTH_SCO_HEADSET: return "DEVICE_OUT_BLUETOOTH_SCO_HEADSET";
        case AUDIO_DEVICE_OUT_BLUETOOTH_SCO_CARKIT:  return "DEVICE_OUT_BLUETOOTH_SCO_CARKIT";
        case AUDIO_DEVICE_OUT_BLUETOOTH_A2DP:    return "DEVICE_OUT_BLUETOOTH_A2DP";
        case AUDIO_DEVICE_OUT_BLUETOOTH_A2DP_HEADPHONES: return "DEVICE_OUT_BLUETOOTH_A2DP_HEADPHONES";
        case AUDIO_DEVICE_OUT_BLUETOOTH_A2DP_SPEAKER:    return "DEVICE_OUT_BLUETOOTH_A2DP_SPEAKER";
        case AUDIO_DEVICE_OUT_AUX_DIGITAL:       return "DEVICE_OUT_AUX_DIGITAL";
        case AUDIO_DEVICE_OUT_FM_TX:             return "DEVICE_OUT_FM_TX";
        case AUDIO_DEVICE_OUT_DEFAULT:           return "DEVICE_OUT_DEFAULT";
        case AUDIO_DEVICE_OUT_ALL:               return "DEVICE_OUT_ALL";
        case AUDIO_DEVICE_OUT_ALL_A2DP:          return "DEVICE_OUT_ALL_A2DP";
        /* Input devices */
        case AUDIO_DEVICE_IN_COMMUNICATION:      return "DEVICE_IN_COMMUNICATION";
        case AUDIO_DEVICE_IN_AMBIENT:            return "DEVICE_IN_AMBIENT";
        case AUDIO_DEVICE_IN_BUILTIN_MIC:        return "DEVICE_IN_BUILTIN_MIC";
        case AUDIO_DEVICE_IN_BLUETOOTH_SCO_HEADSET:  return "DEVICE_IN_BLUETOOTH_SCO_HEADSET";
        case AUDIO_DEVICE_IN_WIRED_HEADSET:      return "DEVICE_IN_WIRED_HEADSET";
        case AUDIO_DEVICE_IN_AUX_DIGITAL:        return "DEVICE_IN_AUX_DIGITAL";
        case AUDIO_DEVICE_IN_VOICE_CALL:         return "DEVICE_IN_VOICE_CALL";
        case AUDIO_DEVICE_IN_BACK_MIC:           return "DEVICE_IN_BACK_MIC";
        case AUDIO_DEVICE_IN_FM_RX:              return "DEVICE_IN_FM_RX";
        case AUDIO_DEVICE_IN_DEFAULT:            return "DEVICE_IN_DEFAULT";
        case AUDIO_DEVICE_IN_ALL:                return "DEVICE_IN_ALL";
        default:                                 return "";
    }
}

const char* state2str(audio_policy_dev_state_t state)
{
    switch (state) {
        case AUDIO_POLICY_DEVICE_STATE_UNAVAILABLE: return "DEVICE_STATE_UNAVAILABLE";
        case AUDIO_POLICY_DEVICE_STATE_AVAILABLE:   return "DEVICE_STATE_AVAILABLE";
        default:                                    return "<INVALID>";
    }
}

const char* mode2str(audio_mode_t mode)
{
    switch (mode) {
        case AUDIO_MODE_CURRENT:             return "MODE_CURRENT";
        case AUDIO_MODE_NORMAL:              return "MODE_NORMAL";
        case AUDIO_MODE_RINGTONE:            return "MODE_RINGTONE";
        case AUDIO_MODE_IN_CALL:             return "MODE_IN_CALL";
        case AUDIO_MODE_IN_COMMUNICATION:    return "MODE_IN_COMMUNICATION";
        case AUDIO_MODE_INVALID:             return "MODE_INVALID";
        default:                             return "<INVALID>";
    }
}

const char* stream2str(audio_stream_type_t stream)
{
    switch (stream) {
        case AUDIO_STREAM_DEFAULT:                  return "DEFAULT";
        case AUDIO_STREAM_VOICE_CALL:               return "VOICE_CALL";
        case AUDIO_STREAM_SYSTEM:                   return "SYSTEM";
        case AUDIO_STREAM_RING:                     return "RING";
        case AUDIO_STREAM_MUSIC:                    return "MUSIC";
        case AUDIO_STREAM_ALARM:                    return "ALARM";
        case AUDIO_STREAM_NOTIFICATION:             return "NOTIFICATION";
        case AUDIO_STREAM_BLUETOOTH_SCO:            return "BLUETOOTH_SCO";
        case AUDIO_STREAM_ENFORCED_AUDIBLE:         return "ENFORCED_AUDIBLE";
        case AUDIO_STREAM_DTMF:                     return "DTMF";
        case AUDIO_STREAM_TTS:                      return "TTS";
#ifdef STE_VIDEO_CALL
        case AUDIO_STREAM_VIDEO_CALL:               return "VIDEO_CALL";
#endif
        default:                                    return "<INVALID>";
    }
}

const char* strategy2str(routing_strategy strategy)
{
    switch (strategy) {
        case STRATEGY_MEDIA:         return "STRATEGY_MEDIA";
        case STRATEGY_PHONE:         return "STRATEGY_PHONE";
        case STRATEGY_SONIFICATION:  return "STRATEGY_SONIFICATION";
        case STRATEGY_DTMF:          return "STRATEGY_DTMF";
        default:                     return "<INVALID>";
    }
}

const char* force_use2str(audio_policy_force_use_t force_use)
{
    switch (force_use) {
        case AUDIO_POLICY_FORCE_FOR_COMMUNICATION:        return "FOR_COMMUNICATION";
        case AUDIO_POLICY_FORCE_FOR_MEDIA:                return "FOR_MEDIA";
        case AUDIO_POLICY_FORCE_FOR_RECORD:               return "FOR_RECORD";
        case AUDIO_POLICY_FORCE_FOR_DOCK:                 return "FOR_DOCK";
        default:                                          return "<INVALID>";
    }
}

const char* forced_config2str(audio_policy_forced_cfg_t forced_config)
{
    switch (forced_config) {
        case AUDIO_POLICY_FORCE_NONE:               return "FORCE_NONE";
        case AUDIO_POLICY_FORCE_SPEAKER:            return "FORCE_SPEAKER";
        case AUDIO_POLICY_FORCE_HEADPHONES:         return "FORCE_HEADPHONES";
        case AUDIO_POLICY_FORCE_BT_SCO:             return "FORCE_BT_SCO";
        case AUDIO_POLICY_FORCE_BT_A2DP:            return "FORCE_BT_A2DP";
        case AUDIO_POLICY_FORCE_WIRED_ACCESSORY:    return "FORCE_WIRED_ACCESSORY";
        case AUDIO_POLICY_FORCE_BT_CAR_DOCK:        return "FORCE_BT_CAR_DOCK";
        case AUDIO_POLICY_FORCE_BT_DESK_DOCK:       return "FORCE_BT_DESK_DOCK";
        case AUDIO_POLICY_FORCE_NO_BT_A2DP:         return "FORCE_NO_BT_A2DP";
        default:                                    return "<INVALID>";
    }
}

const char* source2str(int source)
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
#ifdef STE_VIDEO_CALL
        case AUDIO_SOURCE_VIDEO_CALL:               return "AUDIO_SOURCE_VIDEO_CALL";
#endif
        case AUDIO_SOURCE_VOICE_COMMUNICATION:      return "AUDIO_SOURCE_VOICE_COMMUNICATION";
        default:                                    return "<INVALID>";
    }
}

const char* voiptype2str(int voiptype)
{
    switch (voiptype) {
        case VOIP_INACTIVE:          return "VOIP_INACTIVE";
        case VOIP_VIDEO_CALL:        return "VOIP_VIDEO_CALL";
        case VOIP_VOICE_CALL:        return "VOIP_VOICE_CALL";
        case VOIP_IN_COMMUNICATION:  return "VOIP_IN_COMMUNICATION";
        default:                     return "<INVALID>";
    }
}


routing_strategy getStrategy(audio_stream_type_t stream)
{
    routing_strategy strategy;

    switch (stream) {

        case AUDIO_STREAM_VOICE_CALL:
#ifdef STE_VIDEO_CALL
        case AUDIO_STREAM_VIDEO_CALL:
#endif
        case AUDIO_STREAM_BLUETOOTH_SCO:
            strategy = STRATEGY_PHONE;
            break;
           case AUDIO_STREAM_RING:
        case AUDIO_STREAM_NOTIFICATION:
        case AUDIO_STREAM_ALARM:
        case AUDIO_STREAM_ENFORCED_AUDIBLE:
            strategy = STRATEGY_SONIFICATION;
            break;
           case AUDIO_STREAM_DTMF:
            strategy = STRATEGY_DTMF;
            break;
           case AUDIO_STREAM_MUSIC:
        case AUDIO_STREAM_SYSTEM:
        case AUDIO_STREAM_TTS:
        default:
            strategy = STRATEGY_MEDIA;
            break;
    }

    return strategy;
}


bool is_primary_device(uint32_t device)
{
    if((device & (AUDIO_DEVICE_OUT_EARPIECE |
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
                                AUDIO_DEVICE_IN_DEFAULT)) == 0) {
        return false;
    }
    return true;
}

float linear_to_log(int volume)
{
    const float dBPerStep = 0.5f;
    const float dBConvert = -dBPerStep * 2.302585093f / 20.0f;
    return volume ? exp((float)(100 - volume) * dBConvert) : 0;
}

float compute_volume(
    audio_policy_anm *audio_pol_anm,
    int stream, int index, uint32_t device)
{
    float volume = 1.0;

    stream_descriptor_t streamDesc = audio_pol_anm->mStreams[stream];

    /* Force max volume if stream cannot be muted */
    if (!streamDesc.mCanBeMuted) index = streamDesc.mIndexMax;

    int indexRelative = index - streamDesc.mIndexMin;
    int indexRange = streamDesc.mIndexMax - streamDesc.mIndexMin;

    if (stream == AUDIO_STREAM_VOICE_CALL ||
        stream == AUDIO_STREAM_BLUETOOTH_SCO){
        volume = (float)indexRelative / (float)indexRange;
    } else {
        int volInt = (100 * indexRelative) / indexRange;

        // For DTMF index zero don't mean that volume should be zero.
        // Setting volInt to lowest possible value
        if( (volInt <= 0) && (stream == AUDIO_STREAM_DTMF) ) {
            volInt = (100 * 1) / indexRange;
        }

        volume = linear_to_log(volInt);
    }

    return volume;
}

uint32_t get_max_effects_cpu_load()
{
    return MAX_EFFECTS_CPU_LOAD;
}

uint32_t get_max_effects_memory()
{
    return MAX_EFFECTS_MEMORY;
}

/* ---------------------------------------------------------------------------
 * Top level and actual device functions
 * ---------------------------------------------------------------------------*/
uint32_t get_top_level_device_index(audio_policy_anm *audio_pol_anm, uint32_t device)
{
    /* Only 1 device is allowed as input parameter */
    assert(popcount(device) == 1);

    switch (device) {
    /* Output devices */
    case AUDIO_DEVICE_OUT_EARPIECE:                  return DEV_OUT_INDEX_EARPIECE;
    case AUDIO_DEVICE_OUT_SPEAKER:                   return DEV_OUT_INDEX_SPEAKER;
    case AUDIO_DEVICE_OUT_WIRED_HEADSET:             return DEV_OUT_INDEX_HEADSET;
    case AUDIO_DEVICE_OUT_WIRED_HEADPHONE:           return DEV_OUT_INDEX_HEADPHONE;
    case AUDIO_DEVICE_OUT_BLUETOOTH_A2DP:            return DEV_OUT_INDEX_A2DP;
    case AUDIO_DEVICE_OUT_BLUETOOTH_A2DP_SPEAKER:    return DEV_OUT_INDEX_A2DP;
    case AUDIO_DEVICE_OUT_BLUETOOTH_A2DP_HEADPHONES: return DEV_OUT_INDEX_A2DP;
    case AUDIO_DEVICE_OUT_FM_TX:                     return DEV_OUT_INDEX_FMTX;
    case AUDIO_DEVICE_OUT_AUX_DIGITAL:               return DEV_OUT_INDEX_HDMI;
    case AUDIO_DEVICE_OUT_DGTL_DOCK_HEADSET:         return DEV_OUT_INDEX_USB;
    case AUDIO_DEVICE_OUT_USB_DEVICE:                return DEV_OUT_INDEX_USB;
    case AUDIO_DEVICE_OUT_USB_ACCESSORY:             return DEV_OUT_INDEX_USB;
    case AUDIO_DEVICE_OUT_BLUETOOTH_SCO:
    case AUDIO_DEVICE_OUT_BLUETOOTH_SCO_HEADSET:
    case AUDIO_DEVICE_OUT_BLUETOOTH_SCO_CARKIT:
        if (is_btwb_supported(audio_pol_anm)) {
            return DEV_OUT_INDEX_BTWB;
        } else {
            return DEV_OUT_INDEX_BT;
        }
    case AUDIO_DEVICE_OUT_DEFAULT:
        if (audio_pol_anm->mVoIPActive) {
            return DEV_OUT_INDEX_VOIP;
        } else {
            return DEV_OUT_INDEX_DEFAULT;
        }
    /* Input devices */
    case AUDIO_DEVICE_IN_BUILTIN_MIC:    return DEV_IN_INDEX_MIC;
    case AUDIO_DEVICE_IN_WIRED_HEADSET:  return DEV_IN_INDEX_HEADSET;
    case AUDIO_DEVICE_IN_FM_RX:    return DEV_IN_INDEX_FMRX;
    case AUDIO_DEVICE_IN_VOICE_CALL:     return DEV_IN_INDEX_VOICE_CALL;
    case AUDIO_DEVICE_IN_BLUETOOTH_SCO_HEADSET:
        if (is_btwb_supported(audio_pol_anm)) {
            return DEV_IN_INDEX_BTWB;
        } else {
            return DEV_IN_INDEX_BT;
        }
    case AUDIO_DEVICE_IN_DEFAULT:
        if (audio_pol_anm->mVoIPActive) {
            return DEV_IN_INDEX_VOIP;
        } else {
            return DEV_IN_INDEX_DEFAULT;
        }
    default:
        ALOG_ERR("get_top_level_device_index() Unknown device! %d", device);
        return 0;
    }
}

void get_actual_output_device(audio_policy_anm *audio_pol_anm, uint32_t device, routing_strategy strategy, const char **actual)
{
    uint32_t index = 0;
    assert(actual);
    *actual = NULL;
    /* Only 1 device is allowed as input parameter */
    assert(popcount(device) == 1);

    index = get_top_level_device_index(audio_pol_anm, device);

    switch (strategy) {
        case STRATEGY_MEDIA:
            *actual = actualMediaOutputDevices[index];
             break;
        case STRATEGY_SONIFICATION:
            *actual = actualSonificationOutputDevices[index];
            break;
        case STRATEGY_PHONE:
        case STRATEGY_DTMF:
            if(!audio_pol_anm->mVoIPActive)
                *actual = actualDefaultOutputDevices[index];
            else
                *actual = actualDefaultVoIPOutputDevices[index];
            break;
        default :
            ALOG_ERR("get_actual_output_device() Unknown strategy! %d", strategy);
            break;
    }

    /* Special case for BT headsets supporting
     * noise reduction (NR) and echo cancelling (EC). */
    switch (index) {
        case DEV_OUT_INDEX_BTWB:
            if (is_bt_nr_ec_supported(audio_pol_anm)) {
                if(!audio_pol_anm->mVoIPActive)
                    *actual = DEVICE_ACTUAL_BTWBOUT_NO_NR_EC;
                else
                    *actual = DEVICE_ACTUAL_BTWBOUT_NO_NR_EC_VOIP;
            }
            break;
        case DEV_OUT_INDEX_BT:
            if (is_bt_nr_ec_supported(audio_pol_anm)) {
                if(!audio_pol_anm->mVoIPActive)
                    *actual = DEVICE_ACTUAL_BTOUT_NO_NR_EC;
                else
                    *actual = DEVICE_ACTUAL_BTOUT_NO_NR_EC_VOIP;
            }
            break;
        default:
            break;
    }

    ALOG_INFO_VERBOSE("get_actual_output_device(%s, %s) returned actual out device %s",
        device2str((audio_devices_t)device), strategy2str(strategy), *actual ? *actual : "NULL");
}

void get_actual_input_device(audio_policy_anm *audio_pol_anm, uint32_t device, int source, const char **actual)
{
    uint32_t index = 0;
    assert(actual);
    *actual = NULL;
    /* Only 1 device is allowed as input parameter */
    assert(popcount(device) == 1);

    index = get_top_level_device_index(audio_pol_anm, device);

    switch (source) {
    case AUDIO_SOURCE_DEFAULT:
    case AUDIO_SOURCE_MIC:
    case AUDIO_SOURCE_VOICE_UPLINK:
    case AUDIO_SOURCE_VOICE_DOWNLINK:
    case AUDIO_SOURCE_VOICE_CALL:
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
        if (device == AUDIO_DEVICE_IN_BUILTIN_MIC) {
            uint32_t vcOutDev = get_output_device_for_strategy(audio_pol_anm, (audio_io_handle_t)NULL, STRATEGY_PHONE, AUDIO_FORMAT_PCM_16_BIT
#ifdef USE_CACHE_MECHANISM
        , false
#endif
        );
        switch (vcOutDev) {
            case AUDIO_DEVICE_OUT_EARPIECE:
                if(!audio_pol_anm->mVoIPActive)
                    *actual = DEVICE_ACTUAL_MIC_EARPIECE;
                else
                    *actual = DEVICE_ACTUAL_MIC_EARPIECE_VOIP;
                break;
            case AUDIO_DEVICE_OUT_SPEAKER:
                if(!audio_pol_anm->mVoIPActive)
                    *actual = DEVICE_ACTUAL_MIC_SPEAKER;
                else
                    *actual = DEVICE_ACTUAL_MIC_SPEAKER_VOIP;
                break;
            case AUDIO_DEVICE_OUT_WIRED_HEADPHONE:
                if(!audio_pol_anm->mVoIPActive)
                    *actual = DEVICE_ACTUAL_MIC_HSOUT;
                else
                    *actual = DEVICE_ACTUAL_MIC_HSOUT_VOIP;
                break;
            default:
                if(!audio_pol_anm->mVoIPActive)
                    *actual = actualDefaultInputDevices[index];
                else
                    *actual = actualDefaultVoIPInputDevices[index];
                break;
            }
        } else {
            if(!audio_pol_anm->mVoIPActive)
                *actual = actualDefaultInputDevices[index];
            else
                *actual = actualDefaultVoIPInputDevices[index];
        }
        break;
    default :
        ALOG_ERR("get_actual_input_device() Unknown source! %d", source);
        break;
    }

    /* Special case for BT headsets supporting
     * noise reduction (NR) and echo cancelling (EC). */
    switch (index) {
        case DEV_IN_INDEX_BTWB:
            if (is_bt_nr_ec_supported(audio_pol_anm)) {
                if (!audio_pol_anm->mVoIPActive)
                    *actual = DEVICE_ACTUAL_BTWBIN_NO_NR_EC;
                else
                    *actual = DEVICE_ACTUAL_BTWBIN_NO_NR_EC_VOIP;
            }
            break;
        case DEV_IN_INDEX_BT:
            if (is_bt_nr_ec_supported(audio_pol_anm)) {
                if(!audio_pol_anm->mVoIPActive)
                    *actual = DEVICE_ACTUAL_BTIN_NO_NR_EC;
                else
                    *actual = DEVICE_ACTUAL_BTIN_NO_NR_EC_VOIP;
            }
            break;
        default:
            break;
    }

    ALOG_INFO_VERBOSE("get_actual_input_device(%s, %s) returned actual in device %s",
        device2str((audio_devices_t)device), source2str(source), *actual ? *actual : "NULL");
}

void update_top_level_device_map(const char *toplevel, const char *actual, const char *toplevel2, const char *actual2)
{
    assert(toplevel);
    assert(actual);

    if (!toplevel2) {
        ALOG_INFO_VERBOSE("update_top_level_device_map(): toplevel=%s, actual=%s", toplevel, actual);
    } else {
        assert(actual2);
        ALOG_INFO_VERBOSE("update_top_level_device_map(): toplevel=%s, actual=%s, toplevel2=%s, actual2=%s", toplevel, actual, toplevel2, actual2);
    }

    if (strcmp(actual, "") == 0) {
        ALOG_INFO_VERBOSE("update_top_level_device_map(): actual is empty! Do not update.");
        return;
    }

    if (actual2) {
        if (strcmp(actual2, "") == 0) {
            ALOG_INFO_VERBOSE("update_top_level_device_map(): actual2 is empty! Do not update.");
            return;
        }
    }

    ste_adm_set_toplevel_map_live(toplevel, actual, toplevel2, actual2);
}

bool is_btwb_supported(audio_policy_anm *audio_pol_anm) {
    bool isBtWbSupport = false; /* Bluetooth Wideband (16kHz) support */


    char *keyValuePairs = audio_pol_anm->mp_client_interface->get_parameters(audio_pol_anm->mService, 0 , PARAM_KEY_BTWB);
    struct str_parms *pointerParam = str_parms_create_str(keyValuePairs);
    char value[10];
    int ret = str_parms_get_str(pointerParam, PARAM_KEY_BTWB, value, sizeof(value));
    if (ret >= 0) {
         isBtWbSupport = strncmp(value, "on", 2)==0?true:false;
    }
    str_parms_destroy(pointerParam);
    return isBtWbSupport;
}

bool is_bt_nr_ec_supported(audio_policy_anm *audio_pol_anm) {
    bool isBtNrEcSupport = false; /* Noise reduction (NR) and echo cancelling (EC) support */

    char *keyValuePairs = audio_pol_anm->mp_client_interface->get_parameters(audio_pol_anm->mService, 0 , PARAM_KEY_BTNREC);
    struct str_parms *pointerParam = str_parms_create_str(keyValuePairs);
    char value[10];
    int ret = str_parms_get_str(pointerParam, PARAM_KEY_BTNREC, value, sizeof(value));
    if (ret >= 0) {
         isBtNrEcSupport = strncmp(value, "on", 2)==0?true:false;
    }
    str_parms_destroy(pointerParam);
    return isBtNrEcSupport;
}

/* ---------------------------------------------------------------------------
 * Output Device Routing
 * ---------------------------------------------------------------------------*/
int get_ref_count(ste_audio_output_descriptor_t *output_descr,audio_stream_type_t stream){
    return(output_descr->mRefCount[stream]);
}

int set_ref_count(ste_audio_output_descriptor_t *output_descr,audio_stream_type_t stream,int value){
    output_descr->mRefCount[stream]=value;
    ALOG_INFO_VERBOSE("set_ref_count(output %d,stream %s) return %d",output_descr->output_id,stream2str(stream),output_descr->mRefCount[stream]);
    return(output_descr->mRefCount[stream]);
}

int update_ref_count(ste_audio_output_descriptor_t *output_descr,audio_stream_type_t stream,int value){
    output_descr->mRefCount[stream]+=value;
    ALOG_INFO_VERBOSE("update_ref_count(output %d,stream %s) of %d => return %d",output_descr->output_id,stream2str(stream),value,output_descr->mRefCount[stream]);
    return(output_descr->mRefCount[stream]);
}

int get_strategy_ref_count(ste_audio_output_descriptor_t *output_descr){
    return(output_descr->mStrategyRefCount);
}

int set_strategy_ref_count(ste_audio_output_descriptor_t *output_descr,int value){
    output_descr->mStrategyRefCount=value;
    ALOG_INFO_VERBOSE("set_strategy_ref_count(output %d) return %d",output_descr->output_id,output_descr->mStrategyRefCount);
    return(output_descr->mStrategyRefCount);
}

int update_strategy_ref_count(ste_audio_output_descriptor_t *output_descr,int value,audio_output_flags_t flags){
    if(flags != AUDIO_OUTPUT_FLAG_NONE) {
        int previous_count = output_descr->mStrategyRefCount;
        output_descr->mStrategyRefCount+=value;
        if( output_descr->mStrategyRefCount <=0) {
            ALOG_ERR("update_strategy_ref_count : StrategyRefCount Change from %d to %d   !!! should never be equal or less than 0",
                        previous_count, output_descr->mStrategyRefCount);
            output_descr->mStrategyRefCount = 1;
        }
        ALOG_INFO_VERBOSE("update_strategy_ref_count(output %d) return %d",output_descr->output_id,output_descr->mStrategyRefCount);
    }
    return(output_descr->mStrategyRefCount);
}

struct ap_store_log {
    int nb_output;
    ste_audio_output_descriptor_t tab_descr[20];
    audio_io_handle_t tab_output[20];
};

static struct ap_store_log store_log;

void dump_list_output(audio_policy_anm *audio_pol_anm){
    int i=0;
    int j=0;
    bool display_log=false;
    ste_audio_output_descriptor_t *descr;
    audio_io_handle_t output;
    int indexChange=0;
    bool desc_device_changed=false;
    bool desc_strategy_count_changed=false;
    bool desc_strategy_changed=false;
    bool desc_duplication_changed=false;
    bool desc_output_changed=false;
    int nb_output_changed=0;

    nb_output_changed= ahi_list_count(audio_pol_anm->mOutputs) - store_log.nb_output ;

    if(nb_output_changed!=0) {
        display_log=true;
    }

    for (i = 0; i < ahi_list_count(audio_pol_anm->mOutputs); i++) {
        descr = ahi_list_get_nth_node(audio_pol_anm->mOutputs, i);
        if(descr) {
            if(    ( descr->mDevice != store_log.tab_descr[i].mDevice )   ||
                    (descr->mStrategyRefCount != store_log.tab_descr[i].mStrategyRefCount) ||
                    (descr->mStrategy != store_log.tab_descr[i].mStrategy) ||
                    (memcmp(descr->duplication_info.duplicated_output,
                            store_log.tab_descr[i].duplication_info.duplicated_output,
                            sizeof(audio_io_handle_t)*NUM_DUP_OUTPUT)!=0) ) {
                display_log=true;
                break;
            }
        }
        else {
            ALOG_WARN("dump_list_output() : output descriptor is null");
        }
        output = ahi_list_get_nth_key(audio_pol_anm->mOutputs, i);
        if(output != store_log.tab_output[i]) {
            display_log=true;
            break;
        }
    }

    if(display_log) {
        ALOG_INFO("-------BEGIN  Outputs List -(* = updated)-------");
        if(nb_output_changed != 0) {
            ALOG_INFO(" UPDATE : Number of Output changed : %d",nb_output_changed);
        }
        for (i = 0; i < ahi_list_count(audio_pol_anm->mOutputs); i++) {
            descr = ahi_list_get_nth_node(audio_pol_anm->mOutputs, i);
            if(descr) {
                desc_device_changed=false;
                desc_strategy_count_changed=false;
                desc_strategy_changed=false;
                desc_duplication_changed=false;
                desc_output_changed=false;

                if(descr->mDevice != store_log.tab_descr[i].mDevice) {
                    desc_device_changed=true;
                }
                if(descr->mStrategyRefCount != store_log.tab_descr[i].mStrategyRefCount) {
                    desc_strategy_count_changed=true;
                }
                if(descr->mStrategy != store_log.tab_descr[i].mStrategy) {
                    desc_strategy_changed=true;
                }
                if(memcmp(descr->duplication_info.duplicated_output,store_log.tab_descr[i].duplication_info.duplicated_output,sizeof(audio_io_handle_t)*NUM_DUP_OUTPUT)!=0){
                    desc_duplication_changed=true;
                }
                output = ahi_list_get_nth_key(audio_pol_anm->mOutputs, i);
                if(output != store_log.tab_output[i]) {
                    desc_output_changed=true;
                }
                memcpy(&(store_log.tab_descr[i]),descr,sizeof(ste_audio_output_descriptor_t));
                output = ahi_list_get_nth_key(audio_pol_anm->mOutputs, i);
                store_log.tab_output[i]=output;
                ALOG_INFO("| %s output %d : %s ",(descr->duplication_info.is_duplicated_output   ? "DUPLICATED" : ""),output,((desc_output_changed )? "\t\t(NEW)" : "" ));
                ALOG_INFO("| ==> Devices [%08x] %s %s",descr->mDevice,device2str(descr->mDevice), ((desc_device_changed)? "(*)"  : "" ));
                ALOG_INFO("| ==> mStrategyRefCount %d %s", descr->mStrategyRefCount, ((desc_strategy_count_changed)? "(*)"  : "" ));
                ALOG_INFO("| ==> mStrategy %s %s",strategy2str(descr->mStrategy), ((desc_strategy_changed)? "(*)"  : "" ));
                if(descr->duplication_info.is_duplicated_output) {
                    ALOG_INFO("| ==> Link to Primary output %d and External output %d",descr->duplication_info.primary_output,descr->duplication_info.external_output);
                }
                else {
                    bool DupInfoToDisplay=false;
                    for(j=0;j<NUM_DUP_OUTPUT;j++){
                        if(descr->duplication_info.duplicated_output[j]) {
                            DupInfoToDisplay=true;
                            break;
                        }
                    }
                    if(DupInfoToDisplay  ) {
                        ALOG_INFO("| ==> Duplication table %s",(desc_duplication_changed ? "(*)"  : "" ));
                        for(j=0;j<NUM_DUP_OUTPUT;j++){
                            if(descr->duplication_info.duplicated_output[j]) {
                                ALOG_INFO("| * Used by duplicated output %d",descr->duplication_info.duplicated_output[j]);
                            }
                        }
                    }
                }
                ALOG_INFO("|");
            }
            else {
                ALOG_WARN("dump_list_output() : output descriptor is null");
            }
        }
        ALOG_INFO("------- END Outputs List --------");
        for (i = ahi_list_count(audio_pol_anm->mOutputs); i < 20; i++) {
            memset(&(store_log.tab_descr[i]),0,sizeof(ste_audio_output_descriptor_t ));
            store_log.tab_output[i]=0;
        }
        store_log.nb_output=ahi_list_count(audio_pol_anm->mOutputs);
    }
}


uint32_t get_possible_output_device(audio_policy_anm *audio_pol_anm,
                                  audio_io_handle_t output, uint32_t format)
{
    uint32_t mask = 0;
    int i = 0;

    // Step 1: find devices that can handle the specified format
    if (is_supported_coded_format(format)) {
        mask = AUDIO_DEVICE_OUT_AUX_DIGITAL;
    } else if (audio_is_linear_pcm(format)) {
        mask = AUDIO_DEVICE_OUT_ALL;
    }

    // Step 2: clear busy devices (that can not handle another connection
    // in the given format). Do not clear own device, if we already have one.
    for (i = 0; i < ahi_list_count(audio_pol_anm->mOutputs); i++) {
        ste_audio_output_descriptor_t *cur_descr = ahi_list_get_nth_node(audio_pol_anm->mOutputs, i);
        audio_io_handle_t cur_output     = ahi_list_get_nth_key(audio_pol_anm->mOutputs, i);
        if (output != cur_output && anm_out_desc_ref_count(cur_descr) >  0) {

            if (!audio_is_linear_pcm(cur_descr->mFormat)) {
                mask &= ~(cur_descr->mDevice);
            }

            if (audio_is_linear_pcm(cur_descr->mFormat)) {
                if (!audio_is_linear_pcm(format)) {
                    mask &= ~(cur_descr->mDevice);
                }
            }
        }
    }

    ALOG_INFO_VERBOSE("get_possible_output_device(): format=%X --> mask=%X", format, mask);
    return mask;
}

uint32_t get_preferred_output_device(
    audio_policy_anm *audio_pol_anm,
    audio_io_handle_t output,
    routing_strategy strategy,
    uint32_t format)
{
    uint32_t device = 0;
    uint32_t *deviceList = NULL;
    int i = 0;

    switch (audio_pol_anm->mPhoneState) {

    case AUDIO_MODE_IN_CALL:
    case AUDIO_MODE_IN_COMMUNICATION:

        switch (strategy) {

        case STRATEGY_PHONE:
            deviceList = phoneInCallDevices;
            break;

        case STRATEGY_SONIFICATION:
            deviceList = sonificationInCallDevices;
            break;

        case STRATEGY_MEDIA:
            deviceList = mediaInCallDevices;
            break;

        case STRATEGY_DTMF:
            deviceList = phoneInCallDevices;
            break;

        default:
            ALOG_ERR("get_preferred_output_device(): Invalid strategy %d", strategy);
            break;
        }

        break;

    case AUDIO_MODE_RINGTONE:
        switch (strategy) {

        case STRATEGY_PHONE:
            if(audio_pol_anm->mVoIPActive)
                deviceList = phoneInCallDevices;
            else
                deviceList = phoneDevices;
            break;

        case STRATEGY_SONIFICATION:
            deviceList = sonificationInCallDevices;
            break;

        case STRATEGY_MEDIA:
            deviceList = mediaDevices;
            break;

        case STRATEGY_DTMF:
            if(audio_pol_anm->mVoIPActive)
                deviceList = phoneInCallDevices;
            else
                deviceList = phoneDevices;
            break;

        default:
            ALOG_ERR("get_preferred_output_device(): Invalid strategy %d", strategy);
            break;
        }

        break;

    case AUDIO_MODE_NORMAL:
    default:

        switch (strategy) {

        case STRATEGY_PHONE:
            if(audio_pol_anm->mVoIPActive)
                deviceList = phoneInCallDevices;
            else
                deviceList = phoneDevices;
            break;

        case STRATEGY_SONIFICATION:
            deviceList = sonificationDevices;
            break;

        case STRATEGY_MEDIA:
            deviceList = mediaDevices;
            break;

        case STRATEGY_DTMF:
            if(audio_pol_anm->mVoIPActive)
                deviceList = phoneInCallDevices;
            else
                deviceList = phoneDevices;
            break;

        default:
            ALOG_ERR("get_preferred_output_device(): Invalid strategy %d", strategy);
            break;
        }

        break;
    }

    uint32_t possible = get_possible_output_device(audio_pol_anm, output, format);
    for (i = 0; i < DEVLIST_MAX_NO_DEVICES; i++){
        device = audio_pol_anm->mAvailableOutputDevices & deviceList[i] & possible;
        if ((device != 0) && (audio_pol_anm->mForceUse[AUDIO_POLICY_FORCE_FOR_MEDIA] == AUDIO_POLICY_FORCE_NO_BT_A2DP)) {
             device &=  ~AUDIO_DEVICE_OUT_ALL_A2DP;
             //ALOG_INFO ("get_preferred_output_device(): after removing A2DP device = 0x%x %s ",device,device2str((audio_devices_t)device));
        }
        if (device != 0) {
            if (popcount(device) ==
                popcount(deviceList[i])) {
                /* Preferred device is found */
                break;
            }
        }

        if (deviceList[i] == 0) {
            ALOG_ERR("get_preferred_output_device(): No preferred device is available!");
            break;
        }
        device = 0;
    }

    return device;
}

uint32_t get_forced_output_device(
    audio_policy_anm                   *audio_pol_anm,
    routing_strategy strategy)
{
    uint32_t device = 0;
    bool forcedDevAvailable = true;

    switch (audio_pol_anm->mPhoneState) {

    /* MODE_IN_CALL, MODE_IN_COMMUNICATION */
    case AUDIO_MODE_IN_CALL:
    case AUDIO_MODE_IN_COMMUNICATION:

        switch (strategy) {
        case STRATEGY_PHONE:
        case STRATEGY_DTMF:

            switch (audio_pol_anm->mForceUse[AUDIO_POLICY_FORCE_FOR_COMMUNICATION]) {
            case AUDIO_POLICY_FORCE_BT_SCO:
                device = audio_pol_anm->mAvailableOutputDevices & AUDIO_DEVICE_OUT_BLUETOOTH_SCO_CARKIT;
                if (device) break;
                device = audio_pol_anm->mAvailableOutputDevices & AUDIO_DEVICE_OUT_BLUETOOTH_SCO_HEADSET;
                if (device) break;
                device = audio_pol_anm->mAvailableOutputDevices & AUDIO_DEVICE_OUT_BLUETOOTH_SCO;
                if (device) break;
                forcedDevAvailable = false;
                break;
            case AUDIO_POLICY_FORCE_WIRED_ACCESSORY:
                device = audio_pol_anm->mAvailableOutputDevices & AUDIO_DEVICE_OUT_WIRED_HEADSET;
                if (device) break;
                device = audio_pol_anm->mAvailableOutputDevices & AUDIO_DEVICE_OUT_WIRED_HEADPHONE;
                if (device) break;
                forcedDevAvailable = false;
                break;
            case AUDIO_POLICY_FORCE_SPEAKER:
                device = audio_pol_anm->mAvailableOutputDevices & AUDIO_DEVICE_OUT_SPEAKER;
                if (device) break;
                forcedDevAvailable = false;
                break;
            default:
                break;
            }
            break;

        case STRATEGY_MEDIA:

            switch (audio_pol_anm->mForceUse[AUDIO_POLICY_FORCE_FOR_MEDIA]) {
            case AUDIO_POLICY_FORCE_BT_SCO:
                device = audio_pol_anm->mAvailableOutputDevices & AUDIO_DEVICE_OUT_BLUETOOTH_SCO_CARKIT;
                if (device) break;
                device = audio_pol_anm->mAvailableOutputDevices & AUDIO_DEVICE_OUT_BLUETOOTH_SCO_HEADSET;
                if (device) break;
                device = audio_pol_anm->mAvailableOutputDevices & AUDIO_DEVICE_OUT_BLUETOOTH_SCO;
                if (device) break;
                forcedDevAvailable = false;
                break;
            case AUDIO_POLICY_FORCE_SPEAKER:
                device = audio_pol_anm->mAvailableOutputDevices & AUDIO_DEVICE_OUT_SPEAKER;
                if (device) break;
                forcedDevAvailable = false;
                break;
            default:
                break;
            }

            if(device)break;

            switch (audio_pol_anm->mForceUse[AUDIO_POLICY_FORCE_FOR_COMMUNICATION]) {
            case AUDIO_POLICY_FORCE_BT_SCO:
                device = audio_pol_anm->mAvailableOutputDevices & AUDIO_DEVICE_OUT_BLUETOOTH_SCO_CARKIT;
                if (device) break;
                device = audio_pol_anm->mAvailableOutputDevices & AUDIO_DEVICE_OUT_BLUETOOTH_SCO_HEADSET;
                if (device) break;
                device = audio_pol_anm->mAvailableOutputDevices & AUDIO_DEVICE_OUT_BLUETOOTH_SCO;
                if (device) break;
                forcedDevAvailable = false;
                break;
            case AUDIO_POLICY_FORCE_WIRED_ACCESSORY:
                device = audio_pol_anm->mAvailableOutputDevices & AUDIO_DEVICE_OUT_WIRED_HEADSET;
                if (device) break;
                device = audio_pol_anm->mAvailableOutputDevices & AUDIO_DEVICE_OUT_WIRED_HEADPHONE;
                if (device) break;
                forcedDevAvailable = false;
                break;
            case AUDIO_POLICY_FORCE_SPEAKER:
                device = audio_pol_anm->mAvailableOutputDevices & AUDIO_DEVICE_OUT_SPEAKER;
                if (device) break;
                forcedDevAvailable = false;
                break;
            default:
                break;
            }
            break;

        case STRATEGY_SONIFICATION:
        default:

            switch (audio_pol_anm->mForceUse[AUDIO_POLICY_FORCE_FOR_COMMUNICATION]) {
            case AUDIO_POLICY_FORCE_BT_SCO:
                device = audio_pol_anm->mAvailableOutputDevices & AUDIO_DEVICE_OUT_BLUETOOTH_SCO_CARKIT;
                if (device) break;
                device = audio_pol_anm->mAvailableOutputDevices & AUDIO_DEVICE_OUT_BLUETOOTH_SCO_HEADSET;
                if (device) break;
                device = audio_pol_anm->mAvailableOutputDevices & AUDIO_DEVICE_OUT_BLUETOOTH_SCO;
                if (device) break;
                forcedDevAvailable = false;
                break;
            case AUDIO_POLICY_FORCE_WIRED_ACCESSORY:
                device = audio_pol_anm->mAvailableOutputDevices & AUDIO_DEVICE_OUT_WIRED_HEADSET;
                if (device) break;
                device = audio_pol_anm->mAvailableOutputDevices & AUDIO_DEVICE_OUT_WIRED_HEADPHONE;
                if (device) break;
                forcedDevAvailable = false;
                break;
            default:
                break;
            }

            if (device) {
                /* STRATEGY_SONIFICATIION should always be played on Speaker */
                device = device | (audio_pol_anm->mAvailableOutputDevices & AUDIO_DEVICE_OUT_SPEAKER);
            }

            break;

        }
        break;

    /* MODE_NORMAL, MODE_RINTONE */
    case AUDIO_MODE_RINGTONE:
    case AUDIO_MODE_NORMAL:
    default:
        switch (strategy) {
        case STRATEGY_PHONE:

            switch (audio_pol_anm->mForceUse[AUDIO_POLICY_FORCE_FOR_COMMUNICATION]) {
            case AUDIO_POLICY_FORCE_BT_SCO:
                device = audio_pol_anm->mAvailableOutputDevices & AUDIO_DEVICE_OUT_BLUETOOTH_SCO_CARKIT;
                if (device) break;
                device = audio_pol_anm->mAvailableOutputDevices & AUDIO_DEVICE_OUT_BLUETOOTH_SCO_HEADSET;
                if (device) break;
                device = audio_pol_anm->mAvailableOutputDevices & AUDIO_DEVICE_OUT_BLUETOOTH_SCO;
                if (device) break;
                forcedDevAvailable = false;
                break;
            case AUDIO_POLICY_FORCE_WIRED_ACCESSORY:
                device = audio_pol_anm->mAvailableOutputDevices & AUDIO_DEVICE_OUT_WIRED_HEADSET;
                if (device) break;
                device = audio_pol_anm->mAvailableOutputDevices & AUDIO_DEVICE_OUT_WIRED_HEADPHONE;
                if (device) break;
                forcedDevAvailable = false;
                break;
            case AUDIO_POLICY_FORCE_SPEAKER:
                device = audio_pol_anm->mAvailableOutputDevices & AUDIO_DEVICE_OUT_SPEAKER;
                if (device) break;
                forcedDevAvailable = false;
                break;
            default:
                break;
            }
            break;

        case STRATEGY_MEDIA:
        case STRATEGY_DTMF:

            switch (audio_pol_anm->mForceUse[AUDIO_POLICY_FORCE_FOR_MEDIA]) {
            case AUDIO_POLICY_FORCE_SPEAKER:
                // when not in a phone call, phone strategy should route STREAM_VOICE_CALL to
                // A2DP speaker when forcing to speaker output
                if(ext_hal_is_available(audio_pol_anm->ext_hal,AUDIO_DEVICE_OUT_BLUETOOTH_A2DP_SPEAKER)){
                    if (!is_in_call(audio_pol_anm)) {
                        device = audio_pol_anm->mAvailableOutputDevices & AUDIO_DEVICE_OUT_BLUETOOTH_A2DP_SPEAKER;
                        if (device) break;
                    }
                }
                device = audio_pol_anm->mAvailableOutputDevices & AUDIO_DEVICE_OUT_SPEAKER;
                if (device) break;
                forcedDevAvailable = false;
                break;
            default:
                break;
            }
            break;

        case STRATEGY_SONIFICATION:
        default:
            break;
        }
        break;
    }

    if (device) {
        ALOG_INFO_VERBOSE("get_forced_output_device(): Forced device for strategy %s is %x %s",
            strategy2str(strategy), device,
            device2str((audio_devices_t)device));
    } else {
        if (!forcedDevAvailable) {
            ALOG_WARN("get_forced_output_device(): No forced device available!");
        }
        ALOG_INFO_VERBOSE("get_forced_output_device(): No forced device for strategy %s",
            strategy2str(strategy));
    }
    return device;
}

uint32_t get_output_device_for_strategy(
    audio_policy_anm                   *audio_pol_anm,
    audio_io_handle_t                       output,
    routing_strategy strategy,
    uint32_t                                format
#ifdef USE_CACHE_MECHANISM
    , bool                                  fromCache
#endif
    ) {

    uint32_t device = 0;

#ifdef USE_CACHE_MECHANISM
    if (fromCache && audio_is_linear_pcm(format)) {
        ALOG_INFO_VERBOSE("get_output_device_for_strategy() from cache strategy %s, device %s (%x)",
                strategy2str(strategy),
                device2str((audio_devices_t)audio_pol_anm->mDeviceForStrategy[strategy]),
                audio_pol_anm->mDeviceForStrategy[strategy]);
        return audio_pol_anm->mDeviceForStrategy[strategy];
    }
#endif

    /* First check if device usage is forced for this strategy */
    device = get_forced_output_device(audio_pol_anm, strategy);
    if (device) {
        ALOG_INFO_VERBOSE("get_output_device_for_strategy(): %s, output %d, Forced device: %x %s",
            strategy2str(strategy), output, device, device2str((audio_devices_t)device));
    } else {
        /* Device usage is not forced, get the device from device table */
        device = get_preferred_output_device(audio_pol_anm, output, strategy, format);
        ALOG_INFO_VERBOSE("get_output_device_for_strategy(): %s, output %d, Preferred device: %x %s",
            strategy2str(strategy), output, device, device2str((audio_devices_t)device));
    }

    return device;
}

routing_strategy get_output_strategy(audio_policy_anm *audio_pol_anm, audio_io_handle_t output)
{
    routing_strategy strategy =
        STRATEGY_PHONE;
    ste_audio_output_descriptor_t *descr = ahi_list_get_key_value(audio_pol_anm->mOutputs, output);
    if (descr) {
        strategy = descr->mStrategy;
    } else {
        ALOG_ERR("get_output_strategy(): No output descriptor for output %d!", output);
    }
    return strategy;
}

uint32_t get_output_device(audio_policy_anm *audio_pol_anm, audio_io_handle_t output
#ifdef USE_CACHE_MECHANISM
    , bool fromCache
#endif
)
{

    uint32_t device = 0;
    ste_audio_output_descriptor_t *descr = ahi_list_get_key_value(audio_pol_anm->mOutputs, output);
    if (!descr) {
        ALOG_ERR("get_output_device(): No output descriptor for output %d!", output);
    } else {
        if (descr->mVoIPActive) {
            /* Set device as VoIP device */
            device = AUDIO_DEVICE_OUT_DEFAULT;
        } else {
            /* Get the stream type used by this output */
            routing_strategy strategy =
                get_output_strategy(audio_pol_anm, output);
            /* Get output device for strategy */
            device = get_output_device_for_strategy(audio_pol_anm, output, strategy, descr->mFormat
#ifdef USE_CACHE_MECHANISM
            , fromCache
#endif
            );
        }
    }

    return device;
}

void update_output_config(audio_policy_anm *audio_pol_anm, ste_audio_output_descriptor_t *descr)
{
    size_t i=0x1;
    ALOG_INFO_VERBOSE("ENTER update_output_config(): device=%s", device2str((audio_devices_t)descr->mDevice));
    /* Check if this output is active */
    if (anm_out_desc_ref_count(descr)) {
        /* Loop through the out devices (the first 16 bits in audio_devices) */
        for (i=0x1 ; i<= 0x8000 ; i=i<<1 ) {
            if (descr->mDevice & i) {
                if(!ext_hal_is_external_device(audio_pol_anm->ext_hal,i, NULL)){
                    uint32_t indexTopLevel = get_top_level_device_index(audio_pol_anm, i);
                    if (strategyPriority[descr->mStrategy] > strategyPriority[audio_pol_anm->mCurrentStrategy[indexTopLevel]]) {
                        /* Update current strategy for device */
                        audio_pol_anm->mCurrentStrategy[indexTopLevel] = descr->mStrategy;
                        /* Update top level device map for device */
                        const char *devTopLevel = topLevelOutputDevices[indexTopLevel];
                        const char *devActual = NULL;
                        get_actual_output_device(audio_pol_anm, i, descr->mStrategy, &devActual);
                        update_top_level_device_map(devTopLevel,devActual,NULL, NULL);
                    }
                }
            }
        }
    }

    ALOG_INFO_VERBOSE("LEAVE update_output_config(): device=%s", device2str((audio_devices_t)descr->mDevice));
}

void update_output_routing(
    audio_policy_anm *audio_pol_anm
#ifdef USE_CACHE_MECHANISM
    , bool fromCache
#endif
)
{
    ALOG_INFO_VERBOSE("ENTER update_output_routing(): %d outputs", ahi_list_count(audio_pol_anm->mOutputs));
    int i = 0;
    for (i = 0; i < ahi_list_count(audio_pol_anm->mOutputs); i++) {

        /* Get next output */
        audio_io_handle_t output = ahi_list_get_nth_key(audio_pol_anm->mOutputs, i);
        /* Get output descriptor for the output */
        ste_audio_output_descriptor_t *descr = ahi_list_get_key_value(audio_pol_anm->mOutputs, output);

        /* Get output device, based on current conditions */
        uint32_t device = get_output_device(audio_pol_anm, output
#ifdef USE_CACHE_MECHANISM
            , fromCache
#endif
            );
        if (!device) {
            ALOG_ERR("update_output_routing(): No selected device for output %d!", output);
            return;
        }

        /* Check constraints for using device in current scenario */
        check_hw_resources(audio_pol_anm, &device);

#ifdef USE_CACHE_MECHANISM
        if (!fromCache) {
            check_output_for_all_strategies(audio_pol_anm);
            //Just to parse if suspend state of external devices have to be updated
            ext_hal_update_suspend(audio_pol_anm->ext_hal);
            update_device_for_strategy(audio_pol_anm);
        }
#endif //#if defined USE_CACHE_MECHANISM
#ifdef ALLOW_DUPLICATION
        if (!(anm_out_desc_is_duplicated(descr)))
#endif //#ifdef ALLOW_DUPLICATION
        {
            dump_list_output(audio_pol_anm);
            do_update_routing(audio_pol_anm, output, device);
        }
    }
}

#ifdef USE_CACHE_MECHANISM
void update_device_for_strategy(audio_policy_anm *audio_pol_anm)
{
    int i = 0;
    uint32_t previous_cache[NUM_STRATEGIES];
    memcpy(previous_cache,audio_pol_anm->mDeviceForStrategy,sizeof(uint32_t)*NUM_STRATEGIES);

    ALOG_INFO_VERBOSE("update_device_for_strategy():");
    for (i = 0; i < NUM_STRATEGIES; i++) {
        /*if (i != audio_policy_anm::STRATEGY_SPEECH_PROC)*/
        audio_pol_anm->mDeviceForStrategy[i] = get_output_device_for_strategy(audio_pol_anm, (audio_io_handle_t)NULL, (routing_strategy)i,
                                                        AUDIO_FORMAT_PCM_16_BIT,false);
    }

    if(memcmp(previous_cache,audio_pol_anm->mDeviceForStrategy,sizeof(uint32_t)*NUM_STRATEGIES)!=0) {
        ALOG_INFO_VERBOSE("--------- BEGIN Cache table -----------");
        for (i = 0; i < NUM_STRATEGIES; i++) {
            ALOG_INFO_VERBOSE(" %s => device [0x%x] %s",strategy2str(i),(unsigned int)(audio_pol_anm->mDeviceForStrategy[i]),device2str(audio_pol_anm->mDeviceForStrategy[i]));
        }
        ALOG_INFO_VERBOSE("--------- END Cache table -----------");
    }
}
#endif


bool is_in_call(audio_policy_anm *audio_pol_anm)
{
    return ((audio_pol_anm->mPhoneState == AUDIO_MODE_IN_CALL) ||
           ( audio_pol_anm->mPhoneState == AUDIO_MODE_IN_COMMUNICATION) || audio_pol_anm->mVoIPActive);
}


#ifdef ALLOW_DUPLICATION
audio_io_handle_t open_duplicated_outputs(audio_policy_anm *audio_pol_anm,audio_io_handle_t external_output,ste_audio_output_descriptor_t ** dup_output_desc){
    audio_io_handle_t primary_output = 0;
    audio_io_handle_t duplicated_output = 0;

    if(    audio_pol_anm==NULL ||
            external_output <= 0 ||
            dup_output_desc == NULL )
        return EINVAL;

    ste_audio_output_descriptor_t * primary_descr = NULL;
    ste_audio_output_descriptor_t *external_desc=ahi_list_get_key_value(audio_pol_anm->mOutputs, external_output);
    ste_audio_output_descriptor_t *duplicated_desc = NULL;
    int i=0;
    routing_strategy strategy = STRATEGY_SONIFICATION;
    ALOG_INFO("open_duplicated_outputs for external output %d",external_output);
    if(external_desc==NULL){
        ALOG_WARN("No description found for external output %d, go out",external_output);
        return EINVAL;
    }
    for (i = 0; i < ahi_list_count(audio_pol_anm->mOutputs); i++) {
        primary_output = ahi_list_get_nth_key(audio_pol_anm->mOutputs, i);
        primary_descr = ahi_list_get_key_value(audio_pol_anm->mOutputs, primary_output);
        if(primary_descr==NULL){
            ALOG_WARN("No description found for primary output %d, go out",primary_output);
            return EINVAL;
        }
        if (get_strategy_ref_count(primary_descr)>=1 && primary_descr->mStrategy==strategy) {
            update_strategy_ref_count(primary_descr,1,AUDIO_OUTPUT_FLAG_PRIMARY);
            ALOG_INFO("open_duplicated_outputs :  Reused existing Primary output %d for strategy %s, mStrategyRefCount = %d",
                primary_output, strategy2str((routing_strategy)strategy),primary_descr->mStrategyRefCount);
            break;
        }
    }
   duplicated_output = audio_pol_anm->mp_client_interface->open_duplicate_output(audio_pol_anm->mService, external_output, primary_output);
    // add duplicated output descriptor
    duplicated_desc = (ste_audio_output_descriptor_t*) anm_calloc(sizeof(ste_audio_output_descriptor_t),1);
    init_audio_output_descriptor(duplicated_desc);
    // Update duplicated output information
    duplicated_desc->duplication_info.is_duplicated_output=true;
    duplicated_desc->output_id=duplicated_output;
    //Store information regarding external output
    duplicated_desc->duplication_info.external_output=external_output;
    duplicated_desc->duplication_info.external_desc=external_desc;
    //Store information regarding primary output
    duplicated_desc->duplication_info.primary_desc=primary_descr;
    duplicated_desc->duplication_info.primary_output=primary_output;
    //Store information regarding duplicated output
    duplicated_desc->mSamplingRate = external_desc->mSamplingRate;
    duplicated_desc->mFormat = external_desc->mFormat;
    duplicated_desc->mChannels = external_desc->mChannels;
    duplicated_desc->mLatency = external_desc->mLatency;
    duplicated_desc->mDevice = external_desc->mDevice;
    duplicated_desc->mStrategy = STRATEGY_SONIFICATION;
    set_strategy_ref_count(duplicated_desc,1);
    //Update primary and external outputs to indicate that it's now also used by a duplicated output
    primary_descr->duplication_info.is_duplicated_output=false;
    for(i=0;i<NUM_DUP_OUTPUT;i++){
         if(primary_descr->duplication_info.duplicated_output[i]==0) {
            primary_descr->duplication_info.duplicated_output[i]=duplicated_output;
            break;
        }
    }
    external_desc->duplication_info.is_duplicated_output=false;
    external_desc->duplication_info.duplicated_output[0]=duplicated_output;
    //Then store this new duplicated output in the list of ANM output but also in the private table
    for(i=0;i<NUM_DUP_OUTPUT;i++){
         if(audio_pol_anm->duplicated_output[i]==0) {
            audio_pol_anm->duplicated_output[i]=duplicated_output;
            break;
        }
    }
    ALOG_INFO("open_duplicated_outputs : new duplicated output %d for strategy %s with External Output %d and Primary Output %d", \
        duplicated_output, strategy2str( (routing_strategy) strategy), external_output, primary_output);
    *dup_output_desc=duplicated_desc;
    return(duplicated_output);
}


void close_duplicated_outputs(audio_policy_anm *audio_pol_anm,audio_io_handle_t external_output)
{
    ALOG_INFO("close_duplicated_outputs() ");

    int i = 0;
    struct str_parms *param;
    ste_audio_output_descriptor_t *duplicated_desc=NULL;
    ste_audio_output_descriptor_t *primary_descr=NULL;
    ste_audio_output_descriptor_t *external_desc=NULL;
    audio_io_handle_t duplicated_output;
    audio_io_handle_t primary_output;
    uint32_t nb_strategy_ref=0;

    for(i=0;i<NUM_DUP_OUTPUT;i++){
        if(audio_pol_anm->duplicated_output[i]) {
            duplicated_desc = ahi_list_get_key_value(audio_pol_anm->mOutputs, audio_pol_anm->duplicated_output[i]);
            if(duplicated_desc){
                if(duplicated_desc->duplication_info.is_duplicated_output) {
                    if(duplicated_desc->duplication_info.external_output == external_output) {
                        primary_descr = duplicated_desc->duplication_info.primary_desc;
                        external_desc = duplicated_desc->duplication_info.external_desc;
                        duplicated_output = audio_pol_anm->duplicated_output[i];
                        ALOG_INFO("Closing duplicated output %d, comig from external output %d",duplicated_output,external_output);
                        break;
                    }
                }
                else
                {
                    //Strange
                    ALOG_WARN(" Incoherent entry in duplicated output table");
                    return;
                }
            }
            else {
                ALOG_WARN("close_duplicated_outputs() : output descriptor is null");
            }
        }
    }
    if(duplicated_desc==NULL|| primary_descr==NULL ||external_desc==NULL)
        return;

    // As all active tracks on duplicated output will be deleted,
    // and as they were also referenced on hardware output, the reference
    // count for their stream type must be adjusted accordingly on
    // hardware output.

    for (i = 0; i < (int)AUDIO_STREAM_CNT; i++) {
        int refCount = get_ref_count(duplicated_desc, i);
        anm_out_desc_change_ref_count(primary_descr, (audio_stream_type_t)i,-refCount);
    }

    nb_strategy_ref = get_strategy_ref_count(duplicated_desc);
    audio_pol_anm->mp_client_interface->close_output(audio_pol_anm->mService, duplicated_output);

    anm_free(ahi_list_get_key_value(audio_pol_anm->mOutputs, duplicated_output));
    ahi_list_del_key(&(audio_pol_anm->mOutputs), duplicated_output);

    for(i=0;i<NUM_DUP_OUTPUT;i++) {
        if(audio_pol_anm->duplicated_output[i] == duplicated_output)
            audio_pol_anm->duplicated_output[i] = 0;

        if(duplicated_output == external_desc->duplication_info.duplicated_output[i])
            external_desc->duplication_info.duplicated_output[i] = 0;

        if(duplicated_output == primary_descr->duplication_info.duplicated_output[i])
                primary_descr->duplication_info.duplicated_output[i] = 0;
    }
    routing_strategy strategy = STRATEGY_SONIFICATION;
    for (i = 0; i < ahi_list_count(audio_pol_anm->mOutputs); i++) {
        primary_output = ahi_list_get_nth_key(audio_pol_anm->mOutputs, i);
        primary_descr = ahi_list_get_key_value(audio_pol_anm->mOutputs, primary_output);
        if (get_strategy_ref_count(primary_descr)>=1 && primary_descr->mStrategy==strategy) {
            update_strategy_ref_count(primary_descr,-1*nb_strategy_ref,AUDIO_OUTPUT_FLAG_PRIMARY);
            break;
        }
    }
    ALOG_INFO("close_duplicated_outputs() return");
}
#endif

void check_output_for_strategy(audio_policy_anm *audio_pol_anm, routing_strategy strategy)
{
    uint32_t previous_device = get_output_device_for_strategy(audio_pol_anm, (audio_io_handle_t)NULL, strategy, AUDIO_FORMAT_PCM_16_BIT
#ifdef USE_CACHE_MECHANISM
       , true
#endif
       );
    uint32_t next_device  = get_output_device_for_strategy(audio_pol_anm, (audio_io_handle_t)NULL, (audio_io_handle_t)strategy, AUDIO_FORMAT_PCM_16_BIT
#ifdef USE_CACHE_MECHANISM
        , false
#endif
        );
    bool external_output_was_used = ext_hal_is_external_device(audio_pol_anm->ext_hal,(audio_devices_t)(previous_device & ~AUDIO_DEVICE_OUT_SPEAKER),NULL);
    bool external_output_is_used = ext_hal_is_external_device(audio_pol_anm->ext_hal,(audio_devices_t)(next_device & ~AUDIO_DEVICE_OUT_SPEAKER),NULL);

    audio_io_handle_t src_output = 0;
    audio_io_handle_t dst_output = 0;
    audio_io_handle_t output = 0;
    char buffer[20];
    ste_audio_output_descriptor_t *descr = NULL;
    int i = 0;
    ALOG_INFO_VERBOSE("=========check_output_for_strategy (strategy %s)=================",strategy2str(strategy));

    // Reuse output if already opened for this strategy
    for (i = 0; i < ahi_list_count(audio_pol_anm->mOutputs); i++) {
        descr = ahi_list_get_nth_node(audio_pol_anm->mOutputs, i);
        if(descr) {
            if (get_strategy_ref_count(descr)>=1 && descr->mStrategy==strategy) {
                output = ahi_list_get_nth_key(audio_pol_anm->mOutputs, i);
                ALOG_INFO_VERBOSE("Reuse output %d because already opened for the strategy %s",output, strategy2str(strategy));
                break;
            }
        }
        else {
            ALOG_WARN("check_output_for_strategy() : output descriptor is null");
        }
    }
    src_output = ext_hal_get_io_handle(audio_pol_anm->ext_hal,(audio_devices_t)(previous_device & ~AUDIO_DEVICE_OUT_SPEAKER));
    dst_output = ext_hal_get_io_handle(audio_pol_anm->ext_hal,(audio_devices_t)(next_device & ~AUDIO_DEVICE_OUT_SPEAKER));

    if(!external_output_was_used && external_output_is_used) {
        bool duplicated_is_used = ext_hal_used_for_sonification(     audio_pol_anm->ext_hal,(audio_devices_t)(next_device & ~AUDIO_DEVICE_OUT_SPEAKER)) &&
                                                                                                external_output_is_used &&
                                                                                                (popcount(next_device) == 2);
        src_output = output;
        if (duplicated_is_used) {
            ste_audio_output_descriptor_t * external_desc = NULL;
            external_desc= ext_hal_get_output_desc(audio_pol_anm->ext_hal, dst_output);
            if(external_desc){
                dst_output = external_desc->duplication_info.duplicated_output[0];
                //previous line should be improved ; it works up to now because only one duplicated output can use an external device...
            }
            else {
                ALOG_WARN("check_output_for_strategy() : output descriptor is null");
            }
        }
    }

    if(external_output_was_used && !external_output_is_used) {
        bool duplicated_is_used = ext_hal_used_for_sonification(     audio_pol_anm->ext_hal,(audio_devices_t)(previous_device & ~AUDIO_DEVICE_OUT_SPEAKER)) &&
                                                                                                external_output_was_used &&
                                                                                                (popcount(next_device) == 2);
        dst_output = output;
        if (duplicated_is_used) {
            ste_audio_output_descriptor_t *external_desc = NULL;
            external_desc= ext_hal_get_output_desc(audio_pol_anm->ext_hal, src_output);
            if(external_desc){
                src_output = external_desc->duplication_info.duplicated_output[0];
                //previous line should be improved ; it works up to now because only one duplicated output can use an external device...
            }
            else {
                ALOG_WARN("check_output_for_strategy() : output descriptor is null");
            }
        }
    }

    descr =  ahi_list_get_key_value(audio_pol_anm->mOutputs, src_output);
    sprintf(buffer," ");
    if(descr) {
        if(descr->duplication_info.is_duplicated_output)
            sprintf(buffer,"Duplicated output");
    }
    ALOG_INFO_VERBOSE("   prevDevice [0x%x => %s][%s device][output : %d]%s",
            (unsigned int)previous_device,
            device2str(previous_device),
            (external_output_was_used?"External":"Primary"),
            (src_output==-1?output:src_output),
            buffer);
    descr =  ahi_list_get_key_value(audio_pol_anm->mOutputs, dst_output);
    sprintf(buffer," ");
    if(descr) {
        if(descr->duplication_info.is_duplicated_output)
            sprintf(buffer,"Duplicated output");
    }
    ALOG_INFO_VERBOSE("   curDevice [0x%x => %s][%s device][output : %d]%s",
            (unsigned int)next_device,
            device2str(next_device),
            (external_output_is_used?"External":"Primary"),
            (dst_output==-1?output:dst_output),
            buffer);

    if (src_output > 0 && dst_output > 0) {
        if(src_output == dst_output) {
            ALOG_INFO_VERBOSE("srcOutput = dstOutput = %d    => Go out ",src_output);
            return;
        }
        // Move effects associated to this strategy from previous output to new output
        for (i = 0; i < ahi_list_count(audio_pol_anm->mEffects); i++) {
            ste_effect_descriptor_t *desc = (ste_effect_descriptor_t *)ahi_list_get_nth_node(audio_pol_anm->mEffects,i);
            if(desc){
                if (desc->mSession != AUDIO_SESSION_OUTPUT_STAGE &&
                    desc->mStrategy == strategy &&
                    desc->mIo == src_output) {
                    ALOG_INFO_VERBOSE("check_output_for_strategy(%s) moving effect %d to output %d",strategy2str(strategy),
                            ahi_list_get_key_index(audio_pol_anm->mEffects, i), dst_output);
                    audio_pol_anm->mp_client_interface->move_effects(audio_pol_anm->mService, desc->mSession, src_output, dst_output);
                    desc->mIo = dst_output;
                }
            }
            else {
                ALOG_WARN("check_output_for_strategy() : output descriptor is null");
            }
        }
        // Move tracks associated to this strategy from previous output to new output
        for (i = 0; i < (int)AUDIO_STREAM_CNT; i++) {
            if (getStrategy((audio_stream_type_t)i) == strategy) {
                ALOG_INFO_VERBOSE("check_output_for_strategy(%s) Inform AudioFlinger to move from output %d to output %d", strategy2str(strategy),src_output, dst_output);
                audio_pol_anm->mp_client_interface->set_stream_output(audio_pol_anm->mService, (audio_stream_type_t)i, dst_output);
            }
        }
    }
    else {
        ALOG_INFO_VERBOSE("srcOutput => %s output = %d ; dstOutput => %s output =  %d",(src_output <=0 ? "Invalid" : " valid"),src_output,(dst_output <=0 ? "Invalid" : " valid"),dst_output);
    }
    ALOG_INFO_VERBOSE("==========================");
}

void check_output_for_all_strategies(audio_policy_anm *audio_pol_anm)
{
    check_output_for_strategy(audio_pol_anm, STRATEGY_PHONE);
    check_output_for_strategy(audio_pol_anm, STRATEGY_SONIFICATION);
    check_output_for_strategy(audio_pol_anm, STRATEGY_MEDIA);
    check_output_for_strategy(audio_pol_anm, STRATEGY_DTMF);
}

void do_update_routing(audio_policy_anm *audio_pol_anm, audio_io_handle_t output, uint32_t device)
{
        ALOG_INFO_VERBOSE("do_update_routing(): output %d (device = %s)",
            output, device2str((audio_devices_t)device));

        /* Get output descriptor for the output */
        ste_audio_output_descriptor_t *descr = ahi_list_get_key_value(audio_pol_anm->mOutputs, output);
        if (descr) {
            bool updateRouting = true;
            if (device == (uint32_t)descr->mDevice) {
                /* Preferred device is the same - do not update routing
                 * but update of output config must be checked */
                ALOG_INFO_VERBOSE("do_update_routing(): Preferred device is the same for output %d (device = %s)",
                    output, device2str((audio_devices_t)device));
                updateRouting = false;
            }
            {
                audio_devices_t external_devices = ext_hal_get_external_devices(audio_pol_anm->ext_hal);
                if(ext_hal_is_external_io_handle(audio_pol_anm->ext_hal,output)){
                    //only keep external devices
                    device &= external_devices;
                }
                else {
                    //remove external devices
                    device &= ~external_devices;
                }
            }

            if (device == 0) {
                ALOG_INFO_VERBOSE("updateOutputRouting(): no device for output %d (device = 0)",
                    output);
                updateRouting = false;
            }
            /* Update the output descriptor with the preferred device,
             * and then check if device config needs to be updated */
            uint32_t oldDevice = descr->mDevice;
            descr->mDevice = device;
            update_output_config(audio_pol_anm, descr);

            if (updateRouting) {
                /* Preferred device has changed - update routing */
                ALOG_INFO("update_output_routing(): Preferred Primary device for output %d has changed from %s to %s",
                    output,
                    device2str((audio_devices_t)oldDevice),
                    (device?device2str((audio_devices_t)device):"No device"));

                struct str_parms *param = str_parms_create_str("routing");
                str_parms_add_int(param,"routing",(int)device);
                audio_pol_anm->mp_client_interface->set_parameters(audio_pol_anm->mService, output, str_parms_to_str(param), 0);
                str_parms_destroy(param);

                // Update sink latency since routing has changed and another sink
                //   may be used (e.g. swith from headset to A2DP device)
                struct str_parms *latencyParam = str_parms_create_str(PARAM_KEY_SINK_LATENCY);
                str_parms_add_int(latencyParam, PARAM_KEY_SINK_LATENCY, output);
                audio_pol_anm->mp_client_interface->set_parameters(audio_pol_anm->mService, output, str_parms_to_str(latencyParam), 0);
                str_parms_destroy(latencyParam);
            }
        } else {
            ALOG_ERR("do_update_routing(): No output descriptor for output %d!", output);
        }

        ALOG_INFO_VERBOSE("LEAVE do_update_routing(): %d outputs", ahi_list_count(audio_pol_anm->mOutputs));
}

/* ---------------------------------------------------------------------------
 * Input Device Routing
 * ---------------------------------------------------------------------------*/
uint32_t get_preferred_input_device(
    audio_policy_anm *audio_pol_anm,
    int source, int channels)
{
    uint32_t device = 0;
    uint32_t *deviceList = NULL;
    int i = 0;

    switch (audio_pol_anm->mPhoneState) {
    case AUDIO_MODE_IN_CALL:
    case AUDIO_MODE_IN_COMMUNICATION:

        switch (source) {
        case AUDIO_SOURCE_DEFAULT:
        case AUDIO_SOURCE_MIC:
        case AUDIO_SOURCE_VOICE_CALL_NO_RECORD:
        case AUDIO_SOURCE_CAMCORDER:
        case AUDIO_SOURCE_VOICE_RECOGNITION:
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
            ALOG_ERR("get_preferred_input_device(): No preferred device list, input source = %d", source);
            deviceList = NULL;
            break;
        }
        break;

    case AUDIO_MODE_RINGTONE:
    case AUDIO_MODE_NORMAL:
    default:

        switch (source) {
        case AUDIO_SOURCE_VOICE_CALL_NO_RECORD:
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
            ALOG_ERR("get_preferred_input_device(): No preferred device list, input source = %d", source);
            deviceList = NULL;
            break;
        }
        break;
    }

    for (i =0; i < DEVLIST_MAX_NO_DEVICES; i++){

        device = audio_pol_anm->mAvailableInputDevices & deviceList[i];
        if (device != 0) {
            if (popcount(device) ==
                popcount(deviceList[i])) {
                /* Preferred device is found */
                break;
            }
        }

        if (deviceList[i] == 0) {
            ALOG_ERR("get_preferred_input_device(): No preferred device is available!");
            break;
        }
        device = 0;
    }

    return device;
}

uint32_t get_forced_input_device(audio_policy_anm *audio_pol_anm, int source)
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

#ifdef STE_VIDEO_CALL
    case AUDIO_SOURCE_VIDEO_CALL:
#endif
    case AUDIO_SOURCE_VOICE_COMMUNICATION:
        switch (audio_pol_anm->mForceUse[AUDIO_POLICY_FORCE_FOR_COMMUNICATION]) {
        case AUDIO_POLICY_FORCE_BT_SCO:
            device = audio_pol_anm->mAvailableInputDevices & AUDIO_DEVICE_IN_BLUETOOTH_SCO_HEADSET;
            if (device) break;
            forcedDevAvailable = false;
            break;
        case AUDIO_POLICY_FORCE_WIRED_ACCESSORY:
            device = audio_pol_anm->mAvailableInputDevices & AUDIO_DEVICE_IN_WIRED_HEADSET;
            if (device) break;
            forcedDevAvailable = false;
            break;
        case AUDIO_POLICY_FORCE_SPEAKER:
            device = audio_pol_anm->mAvailableInputDevices & AUDIO_DEVICE_IN_BUILTIN_MIC;
            if (device) break;
            forcedDevAvailable = false;
            break;
        default:
            break;
        }
        break;
    case AUDIO_SOURCE_VOICE_UPLINK:
    case AUDIO_SOURCE_VOICE_DOWNLINK:
    case AUDIO_SOURCE_FM_RADIO_RX:
        break;

    default:
        ALOG_WARN("get_forced_input_device(): invalid input source %d", source);
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
            switch (audio_pol_anm->mForceUse[AUDIO_POLICY_FORCE_FOR_RECORD]) {

            case AUDIO_POLICY_FORCE_BT_SCO:
                device = audio_pol_anm->mAvailableInputDevices & AUDIO_DEVICE_IN_BLUETOOTH_SCO_HEADSET;
                if (device) break;
                forcedDevAvailable = false;
                break;
            case AUDIO_POLICY_FORCE_WIRED_ACCESSORY:
                device = audio_pol_anm->mAvailableInputDevices & AUDIO_DEVICE_IN_WIRED_HEADSET;
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
        case AUDIO_SOURCE_VOICE_CALL:

#ifdef STE_VIDEO_CALL
        case AUDIO_SOURCE_VIDEO_CALL:
#endif
        case AUDIO_SOURCE_VOICE_COMMUNICATION:
        case AUDIO_SOURCE_FM_RADIO_RX:
            break;

        default:
            ALOG_WARN("get_forced_input_device(): invalid input source %d", source);
            device = 0;
            break;
        }
    }


    if (device) {
        ALOG_INFO_VERBOSE("get_forced_input_device(): Forced device for input source %s is "
            "%x %s", source2str(source), device,
            device2str((audio_devices_t)device));
    } else {
        if (!forcedDevAvailable) {
            ALOG_WARN("get_forced_input_device(): No forced device available!");
        }
        ALOG_INFO_VERBOSE("get_forced_input_device(): No forced device for input source %s",
            source2str(source));
    }
    return device;
}

uint32_t get_input_device_for_source(
    audio_policy_anm *audio_pol_anm,
    int source, int channels) {

    uint32_t device = 0;
    /* First check if device usage is forced for this input source */
    device = get_forced_input_device(audio_pol_anm, source);
    if (device) {
        ALOG_INFO_VERBOSE("get_input_device_for_source(): %s, Forced device: %x %s",
            source2str(source), device, device2str((audio_devices_t)device));
    } else {
        /* Device usage is not forced, get the device from device table */
        device = get_preferred_input_device(audio_pol_anm, source, channels);
        ALOG_INFO_VERBOSE("get_input_device_for_source(): %s, Preferred device: %x %s",
            source2str(source), device, device2str((audio_devices_t)device));
    }

    return device;
}

uint32_t get_input_device(audio_policy_anm *audio_pol_anm, audio_io_handle_t input) {

    uint32_t device = 0;
    ste_audio_input_desc_t *descr = ahi_list_get_key_value(audio_pol_anm->mInputs, input);
    if (descr) {
        if (descr->mVoIPActive) {
            /* Set device as VoIP device */
            device = AUDIO_DEVICE_IN_DEFAULT;
        } else {
            /* Get input device for this source and number of channels */
            device = get_input_device_for_source(
                audio_pol_anm,
                descr->mInputSource, popcount(descr->mChannels));
        }
    } else {
        ALOG_ERR("get_input_device(): No input descriptor for input %d!", input);
    }

    return device;
}

void update_input_config(audio_policy_anm *audio_pol_anm, ste_audio_input_desc_t *descr)
{
    size_t i = 0;
    ALOG_INFO_VERBOSE("ENTER update_input_config(): device=%s", device2str((audio_devices_t)descr->mDevice));

    /* Check if this input is active */
    if (descr->mRefCount) {

        /* Loop through the in devices (the last 16 bits in audio_devices) */
        for (i=0x10000 ; i>0 && i<=0x80000000 ; i=i<<1 ) {
            if (descr->mDevice & i) {
                uint32_t indexTopLevel = get_top_level_device_index(audio_pol_anm, i);
                if (sourcePriority[descr->mInputSource] > sourcePriority[audio_pol_anm->mCurrentSource[indexTopLevel]]) {
                    /* Update current source for device */
                    audio_pol_anm->mCurrentSource[indexTopLevel] = descr->mInputSource;
                    /* Update top level device map for device */
                    const char *devTopLevel = topLevelInputDevices[indexTopLevel];
                    const char *devActual = NULL;
                    get_actual_input_device(audio_pol_anm, i, descr->mInputSource, &devActual);
                    update_top_level_device_map(devTopLevel,devActual,NULL, NULL);
                }
            }
        }
    }

    ALOG_INFO_VERBOSE("LEAVE update_input_config(): device=%s", device2str((audio_devices_t)descr->mDevice));
}

void update_input_routing(audio_policy_anm *audio_pol_anm)
{
    int i = 0;
    ALOG_INFO_VERBOSE("ENTER update_input_routing(): %d inputs", ahi_list_count(audio_pol_anm->mInputs));

    for (i = 0; i < ahi_list_count(audio_pol_anm->mInputs); i++) {

        /* Get next input */
        audio_io_handle_t input = ahi_list_get_nth_key(audio_pol_anm->mInputs, i);

        /* Get input device, based on current conditions */
        uint32_t device = get_input_device(audio_pol_anm, input);
        if (!device) {
            ALOG_ERR("update_input_routing(): No selected device for input %d!", input);
            continue;
        }

        /* Check constraints for using device in current scenario */
        check_hw_resources(audio_pol_anm, &device);

        /* Get input descriptor for the input */
        ste_audio_input_desc_t *descr = ahi_list_get_key_value(audio_pol_anm->mInputs, input);
        if (descr) {
            bool updateRouting = true;
            if (device == (uint32_t)descr->mDevice) {
                /* Preferred device is the same - do not update routing
                 * but update of input config must be checked */
                ALOG_INFO_VERBOSE("update_input_routing(): Preferred device is the same for input %d (device = %s)",
                    input, device2str((audio_devices_t)device));
                updateRouting = false;
            }

            /* Update the input descriptor with the preferred device,
             * and then check if device config needs to be updated */
            uint32_t oldDevice = descr->mDevice;
            descr->mDevice = device;
            update_input_config(audio_pol_anm, descr);

            if (updateRouting) {
                /* Preferred device has changed - update routing */
                ALOG_INFO("update_input_routing(): Preferred device for input %d has changed from %s to %s",
                    input,
                    device2str((audio_devices_t)oldDevice),
                    device2str((audio_devices_t)device));

                struct str_parms *param = str_parms_create_str("routing");
                str_parms_add_int(param,"routing", (int)device);
                audio_pol_anm->mp_client_interface->set_parameters(audio_pol_anm->mService, input, str_parms_to_str(param), 0);
                str_parms_destroy(param);
            }
        } else {
            ALOG_ERR("update_input_routing(): No input descriptor for input %d!", input);
        }
    }

    ALOG_INFO_VERBOSE("LEAVE update_input_routing(): %d inputs", ahi_list_count(audio_pol_anm->mInputs));
}

void check_hw_resources(audio_policy_anm *audio_pol_anm, uint32_t *device)
{
    /* When earpiece is used as active device, there are constraints on
       which other devices that are allowed to be used simultaneously */
    bool earpieceInUse = false;
    int i = 0;

    /* Check if earpiece is used in any output */
    for (i = 0; i < ahi_list_count(audio_pol_anm->mOutputs); i++) {
        ste_audio_output_descriptor_t *descr = ahi_list_get_nth_node(audio_pol_anm->mOutputs, i);
        if (anm_out_desc_ref_count(descr) > 0) {
            audio_io_handle_t output = ahi_list_get_nth_key(audio_pol_anm->mOutputs, i);
            /* Get output device, based on current conditions */
            uint32_t outputDevice = get_output_device(audio_pol_anm, output
#ifdef USE_CACHE_MECHANISM
            , false
#endif
            );
            if (outputDevice & AUDIO_DEVICE_OUT_EARPIECE) {
                earpieceInUse = true;
                break;
            }
        }
    }

    /* Check if earpiece is used in cscall */
    if (!earpieceInUse && (audio_pol_anm->mPhoneState == AUDIO_MODE_IN_CALL || audio_pol_anm->mVoIPActive)) {
        if (get_output_device_for_strategy(audio_pol_anm, (audio_io_handle_t)NULL, STRATEGY_PHONE, AUDIO_FORMAT_PCM_16_BIT
#ifdef USE_CACHE_MECHANISM
        , false
#endif
        ) &
        AUDIO_DEVICE_OUT_EARPIECE ) {
            earpieceInUse = true;
        }
    }

    if (earpieceInUse) {
        for (i = 0; i < DEVLIST_MAX_NO_DEVICES && constraintsEarpiece[i] != 0; i++) {
            if (*device & constraintsEarpiece[i]) {
                ALOG_INFO_VERBOSE("check_hw_resources(): %s not allowed to be used with %s, use %s instead",
                    device2str((audio_devices_t)constraintsEarpiece[i]),
                    device2str(AUDIO_DEVICE_OUT_EARPIECE),
                    device2str(AUDIO_DEVICE_OUT_EARPIECE));
                *device = AUDIO_DEVICE_OUT_EARPIECE;
                break;
            }
        }
    }
}

/* ---------------------------------------------------------------------------
 * CS Call Routing
 * ---------------------------------------------------------------------------*/
void update_cscall_routing(struct audio_policy *pol)
{
    audio_policy_anm *audio_pol_anm = to_apm_anm(pol);
    uint32_t inputDevice  = get_input_device_for_source(audio_pol_anm, AUDIO_SOURCE_VOICE_CALL_NO_RECORD, 1);
    uint32_t outputDevice = get_output_device_for_strategy(audio_pol_anm, (audio_io_handle_t)NULL, STRATEGY_PHONE,
                             AUDIO_FORMAT_PCM_16_BIT
#ifdef USE_CACHE_MECHANISM
        , false
#endif
        );
    const char *inTopLevel = NULL;
    const char *inActual = NULL;
    const char *outTopLevel = NULL;
    const char *outActual = NULL;
    uint32_t inTopLevelIndex;
    uint32_t outTopLevelIndex;

    ALOG_INFO_VERBOSE("ENTER update_cscall_routing(): %s, %s",
        device2str((audio_devices_t)inputDevice),
        device2str((audio_devices_t)outputDevice));

    if (audio_pol_anm->fd_adm == -1) {
        // connect to ADM
        audio_pol_anm->fd_adm = ste_adm_client_connect();
        ALOG_INFO_VERBOSE("update_cscall_routing(): Connected to ADM, fd = %d\n", audio_pol_anm->fd_adm);
    }

    LOCK_MUTEX(audio_pol_anm->mMutexCallDevices);
    inTopLevelIndex = get_top_level_device_index(audio_pol_anm, inputDevice);
    inTopLevel = topLevelInputDevices[inTopLevelIndex];
    get_actual_input_device(audio_pol_anm, inputDevice, AUDIO_SOURCE_VOICE_CALL_NO_RECORD, &inActual);
    outTopLevelIndex = get_top_level_device_index(audio_pol_anm, outputDevice);
    outTopLevel = topLevelOutputDevices[outTopLevelIndex];
    get_actual_output_device(audio_pol_anm, outputDevice, STRATEGY_PHONE, &outActual);

    /* Update the current source and strategy since ongoing call has highest prio */
    audio_pol_anm->mCurrentSource[inTopLevelIndex] = AUDIO_SOURCE_VOICE_CALL_NO_RECORD;
    audio_pol_anm->mCurrentStrategy[outTopLevelIndex] = STRATEGY_PHONE;
    /* Update top level map and set cscall devices */
    update_top_level_device_map(inTopLevel, inActual, outTopLevel, outActual);
    if ((audio_pol_anm->mModemType == ADM_FAT_MODEM) || (audio_pol_anm->mModemType == ADM_FULL_FAT_MODEM)) {
        switch (outputDevice) {
        case AUDIO_DEVICE_OUT_BLUETOOTH_SCO:
        case AUDIO_DEVICE_OUT_BLUETOOTH_SCO_HEADSET:
            ste_adm_client_init_cscall_downstream_volume(audio_pol_anm->mStreams[AUDIO_STREAM_BLUETOOTH_SCO].mIndexMin, audio_pol_anm->mStreams[AUDIO_STREAM_BLUETOOTH_SCO].mIndexMax);
            break;

        default:
            ste_adm_client_init_cscall_downstream_volume(audio_pol_anm->mStreams[AUDIO_STREAM_VOICE_CALL].mIndexMin, audio_pol_anm->mStreams[AUDIO_STREAM_VOICE_CALL].mIndexMax);
            break;
        }
    }
    ste_adm_set_cscall_devices(audio_pol_anm->fd_adm, inTopLevel, outTopLevel);

    /* update volume for speech */
    switch (outputDevice) {

    ALOG_INFO_VERBOSE("update_cscall_routing(): Update Volume index for %s\n", device2str((audio_devices_t)outputDevice));
    case AUDIO_DEVICE_OUT_BLUETOOTH_SCO:
    case AUDIO_DEVICE_OUT_BLUETOOTH_SCO_HEADSET:
        if (audio_pol_anm->mStreams[AUDIO_STREAM_BLUETOOTH_SCO].mMuteCount == 0) {
            ste_anm_ap_set_stream_volume_index(pol, AUDIO_STREAM_BLUETOOTH_SCO, audio_pol_anm->mStreams[AUDIO_STREAM_BLUETOOTH_SCO].mIndexCur);
        }
        break;

    default:
        if (audio_pol_anm->mStreams[AUDIO_STREAM_VOICE_CALL].mMuteCount == 0) {
            ste_anm_ap_set_stream_volume_index(pol, AUDIO_STREAM_VOICE_CALL, audio_pol_anm->mStreams[AUDIO_STREAM_VOICE_CALL].mIndexCur);
        }
        break;
    }
    UNLOCK_MUTEX(audio_pol_anm->mMutexCallDevices);
    ALOG_INFO_VERBOSE("LEAVE update_cscall_routing(): %s, %s",
        device2str((audio_devices_t)inputDevice),
        device2str((audio_devices_t)outputDevice));
}

void close_cs_call(audio_policy_anm *audio_pol_anm)
{
    ALOG_INFO("close_cs_call()");
    int i=0;

    /* Do nothing if ADM session is already closed */
    if (audio_pol_anm->fd_adm != -1) {
        LOCK_MUTEX(audio_pol_anm->mMutexCallDevices);
        ste_adm_set_cscall_devices(audio_pol_anm->fd_adm, NULL, NULL);
        UNLOCK_MUTEX(audio_pol_anm->mMutexCallDevices);
        /* disconnect ADM connection */
        ALOG_INFO_VERBOSE("close_cs_call(): Disconnecting from ADM, fd = %d\n", audio_pol_anm->fd_adm);
        ste_adm_client_disconnect(audio_pol_anm->fd_adm);
        audio_pol_anm->fd_adm = -1;

        /* Invalidate the current source/strategy if voice call*/
        for (i = 0; i<NUM_DEV_OUT_INDEXES; i++) {
            if (audio_pol_anm->mCurrentStrategy[i] == STRATEGY_PHONE) {
                audio_pol_anm->mCurrentStrategy[i] = STRATEGY_UNKNOWN;
            }
        }
        for (i = 0; i<NUM_DEV_IN_INDEXES; i++) {
            if (audio_pol_anm->mCurrentSource[i] == AUDIO_SOURCE_VOICE_CALL_NO_RECORD) {
                audio_pol_anm->mCurrentSource[i] = AUDIO_SOURCE_UNKNOWN;
            }
        }
    } else {
        ALOG_INFO("close_cs_call(): Already closed!");
    }
}

void check_call_status(struct audio_policy *policy)
{
    audio_policy_anm *audio_pol_anm = to_apm_anm(policy);
    int i = 0;

    LOCK_MUTEX(audio_pol_anm->mMutexCallStatus);
    ALOG_INFO("check_call_status()");
    if (audio_pol_anm->mCSCallActive) {
        ALOG_INFO("check_call_status(): Voice call is ongoing, but cannot recover call graph");
    } else if (audio_pol_anm->mVoIPActive) {
        ALOG_INFO("check_call_status(): VoIP call is ongoing, recover call graph");
        ste_audio_output_descriptor_t *outDescr = NULL;
        ste_audio_input_desc_t  *inDescr = NULL;
        close_cs_call(audio_pol_anm);
        set_voip_status(policy, false, 0,0,0,0);
        update_input_routing(audio_pol_anm);
        update_output_routing(audio_pol_anm
#ifdef USE_CACHE_MECHANISM
            , false
#endif
        );
        // Check for any existing active VoIP output
        for (i = 0; i < ahi_list_count(audio_pol_anm->mOutputs); i++) {
            audio_io_handle_t output = ahi_list_get_nth_key(audio_pol_anm->mOutputs, i);
            outDescr = ahi_list_get_key_value(audio_pol_anm->mOutputs, output);
            if (outDescr != NULL) {
                if (outDescr->mVoIPActive) {
                    // Active VoIP output found
                    break;
                }
            }
        }
        // Check for any existing active VoIP input
        for (i = 0; i < ahi_list_count(audio_pol_anm->mInputs); i++) {
            audio_io_handle_t input = ahi_list_get_nth_key(audio_pol_anm->mInputs, i);
            inDescr = ahi_list_get_key_value(audio_pol_anm->mInputs, input);
            if (inDescr != NULL) {
                if (inDescr->mVoIPActive) {
                    // Active VoIP input found
                    break;
                }
            }
        }

        bool synced = is_voip_synched(outDescr->mVoIPType);

        if (outDescr && inDescr) {
            if (outDescr->mVoIPActive && inDescr->mVoIPActive) {
                set_voip_status(policy, true, inDescr->mSamplingRate, inDescr->mChannels, outDescr->mSamplingRate, outDescr->mChannels);
                update_cscall_routing(policy);
                update_input_routing(audio_pol_anm);
                update_output_routing(audio_pol_anm
#ifdef USE_CACHE_MECHANISM
                    , false
#endif
                );
            }
        } else if (!synced) {
            if (outDescr) {
                if (outDescr->mVoIPActive) {
                    set_voip_status(policy, true, 0, 0, outDescr->mSamplingRate, outDescr->mChannels);
                    update_cscall_routing(policy);
                    update_input_routing(audio_pol_anm);
                    update_output_routing(audio_pol_anm
#ifdef USE_CACHE_MECHANISM
                        , false
#endif
                    );
                }
            }
            if (inDescr) {
                if (inDescr->mVoIPActive) {
                    set_voip_status(policy, true, inDescr->mSamplingRate, inDescr->mChannels, 0, 0);
                    update_cscall_routing(policy);
                    update_input_routing(audio_pol_anm);
                    update_output_routing(audio_pol_anm
#ifdef USE_CACHE_MECHANISM
                        , false
#endif
                    );
                }
            }
        }
    } else {
        ALOG_INFO("check_call_status(): No ongoing call to recover");
    }
    UNLOCK_MUTEX(audio_pol_anm->mMutexCallStatus);
}

void vc_open_request(struct audio_policy *pol)
{
    audio_policy_anm *audio_pol_anm = to_apm_anm(pol);
    ste_adm_vc_modem_status_t lModemStatus;
    LOCK_MUTEX(audio_pol_anm->mMutexVC);

    if ((audio_pol_anm->mModemType == ADM_FAT_MODEM)|| (audio_pol_anm->mModemType == ADM_FULL_FAT_MODEM)) {
        lModemStatus = STE_ADM_VC_MODEM_STATUS_CONNECTED;
    } else {
        lModemStatus = audio_pol_anm->mModemStatus;
    }

    ALOG_INFO("vc_open_request(): phoneState=%s, modemStatus=%s",
        mode2str((audio_mode_t)audio_pol_anm->mPhoneState), lModemStatus ? "CONNECTED" : "DISCONNECTED");

    /*  Only open VC if both state is IN_CALL and modem is connected */
    if (audio_pol_anm->mPhoneState == AUDIO_MODE_IN_CALL &&
        lModemStatus == STE_ADM_VC_MODEM_STATUS_CONNECTED &&
        audio_pol_anm->mCSCallActive == false &&
        audio_pol_anm->mVoIPActive == false) {
        ALOG_INFO("vc_open_request(): Open voice call");
        audio_pol_anm->mCSCallActive = true;
        update_cscall_routing(pol);
    } else if (audio_pol_anm->mCSCallActive) {
        ALOG_INFO("vc_open_request(): Voice call already opened, do nothing");
    } else if (audio_pol_anm->mVoIPActive) {
        ALOG_INFO("vc_open_request(): VoIP active, do nothing");
    } else {
        ALOG_INFO("vc_open_request(): Requirements not fulfilled, do nothing");
    }

    UNLOCK_MUTEX(audio_pol_anm->mMutexVC);
}

void vc_close_request(audio_policy_anm *audio_pol_anm)
{
    ste_adm_vc_modem_status_t lModemStatus;

    LOCK_MUTEX(audio_pol_anm->mMutexVC);

    if ((audio_pol_anm->mModemType == ADM_FAT_MODEM)|| (audio_pol_anm->mModemType == ADM_FULL_FAT_MODEM)) {
        lModemStatus = STE_ADM_VC_MODEM_STATUS_DISCONNECTED;
    } else {
        lModemStatus = audio_pol_anm->mModemStatus;
    }

    ALOG_INFO("vc_close_request(): phoneState=%s, modemStatus=%s",
        mode2str((audio_mode_t)audio_pol_anm->mPhoneState), lModemStatus ? "CONNECTED" : "DISCONNECTED");

    if (audio_pol_anm->mPhoneState != AUDIO_MODE_IN_CALL &&
        lModemStatus == STE_ADM_VC_MODEM_STATUS_DISCONNECTED &&
        audio_pol_anm->mCSCallActive == true) {
        ALOG_INFO("vc_close_request(): Close voice call");
        audio_pol_anm->mCSCallActive = false;
        close_cs_call(audio_pol_anm);
    } else if (!audio_pol_anm->mCSCallActive) {
        ALOG_INFO("vc_close_request(): Voice call not opened, do nothing");
    } else {
        ALOG_INFO("vc_close_request(): Voice call requirements for closing not fulfilled, do nothing");
    }

    UNLOCK_MUTEX(audio_pol_anm->mMutexVC);
}

void subscribe_modem_status(struct audio_policy *policy)
{
    ALOG_INFO_VERBOSE("subscribe_modem_status()");
    pthread_t thread;
    pthread_attr_t tattr;
    pthread_attr_init(&tattr);
    pthread_attr_setdetachstate(&tattr, PTHREAD_CREATE_DETACHED);
    if (pthread_create(&thread, &tattr, thread_subscribe_modem_status, policy)) {
        ALOG_ERR("subscribe_modem_status(): pthread_create() failed\n");
    }
}

void* thread_subscribe_modem_status(void *param)
{
    struct audio_policy *policy = (struct audio_policy *)param;
    audio_policy_anm *ap = to_apm_anm(param);
    ste_adm_res_t res;
    int adm_id;

    while ((adm_id = ste_adm_client_connect()) < 0) {
        ALOG_ERR("thread_subscribe_modem_status(): Failed to connect to ADM");
        usleep(3 * 1000 * 1000);
    }

    while (1) {
        res = ste_adm_client_request_modem_vc_state(adm_id);
        if (res != STE_ADM_RES_OK) {
            ALOG_ERR("thread_subscribe_modem_status(): ste_adm_client_request_modem_vc_state failed with %d", res);
        }

        if (res == STE_ADM_RES_OK) {
            res = ste_adm_client_read_modem_vc_state(adm_id, &ap->mModemStatus);
            if (res != STE_ADM_RES_OK) {
                ALOG_ERR("thread_subscribe_modem_status(): ste_adm_client_read_modem_vc_state failed with %d", res);
            } else {
                ALOG_INFO("thread_subscribe_modem_status(): ADM Modem Status = %s", ap->mModemStatus == STE_ADM_VC_MODEM_STATUS_CONNECTED ? "CONNECTED" : "DISCONNECTED");
                if (ap->mModemStatus == STE_ADM_VC_MODEM_STATUS_CONNECTED) {
                    vc_open_request(policy);
                } else {
                    vc_close_request(ap);
                }
            }
        }

        if (res != STE_ADM_RES_OK) {
            ste_adm_client_disconnect(adm_id);
            while ((adm_id = ste_adm_client_connect()) < 0) {
                ALOG_ERR("thread_subscribe_modem_status(): Failed to connect to ADM\n");
                usleep(3 * 1000 * 1000);
            }
        }
    }

    return 0;
}

/* ---------------------------------------------------------------------------
 * VoIP functions
 * ---------------------------------------------------------------------------*/
void set_voip_status(struct audio_policy *pol, bool enabled,
     int inSamplerate,int inChannels, int outSamplerate,int outChannels)
{
    int voip_enabled = enabled ? 1 : 0;
    ste_adm_voip_mode_t mode;
    audio_policy_anm *audio_pol_anm = to_apm_anm(pol);

    mode.in_samplerate = inSamplerate;
    if (inChannels != AUDIO_CHANNEL_IN_STEREO) {
        mode.in_channels = 1;
    } else {
        mode.in_channels = 2;
    }

    mode.out_samplerate = outSamplerate;
    if (outChannels != AUDIO_CHANNEL_OUT_STEREO) {
        mode.out_channels = 1;
    } else {
        mode.out_channels = 2;
    }

    ALOG_INFO("set_voip_status(): %s, Input (UL) [%d, %d], Output (DL) [%d, %d]",
        enabled ? "ENABLED" : "DISABLED",
        inSamplerate, mode.in_channels, outSamplerate, mode.out_channels);

    /* Inform ADM about VoIP status */
    ste_adm_client_set_cscall_voip_mode(voip_enabled,&mode);

    audio_pol_anm->mVoIPActive = enabled;

    /* If phone state is MODE_IN_CALL and VoIP is not active, a normal
     * voice call might have been received during the VoIP call. */
    if (audio_pol_anm->mPhoneState == AUDIO_MODE_IN_CALL && !audio_pol_anm->mVoIPActive) {
        ALOG_INFO_VERBOSE("set_voip_status(): phone state is MODE_IN_CALL, request VC open\n");
        vc_open_request(pol);
    }
}

bool is_voip_output(audio_policy_anm *audio_pol_anm, ste_audio_output_descriptor_t *descr, audio_stream_type_t stream,
    bool *inputAvailable, ste_audio_input_desc_t **inDescr)
{
    int i = 0;
    *inputAvailable = false;
    ste_adm_vc_modem_status_t lModemStatus;

    if ((audio_pol_anm->mModemType == ADM_FAT_MODEM) ||(audio_pol_anm->mModemType == ADM_FULL_FAT_MODEM)) {
        lModemStatus = STE_ADM_VC_MODEM_STATUS_DISCONNECTED;
    } else {
        lModemStatus = audio_pol_anm->mModemStatus;
    }

    if (audio_pol_anm->mCSCallActive
        || lModemStatus == STE_ADM_VC_MODEM_STATUS_CONNECTED
        ) {
        /* No VoIP output */
        descr->mVoIPType = VOIP_INACTIVE;
        return false;
    }

    switch (stream) {
#ifdef STE_VIDEO_CALL
    case AUDIO_STREAM_VIDEO_CALL:
#endif
        /* VoIP type VIDEO_CALL */
        descr->mVoIPType = VOIP_VIDEO_CALL;
        /* Check for corresponding input */
        for (i = 0; i < ahi_list_count(audio_pol_anm->mInputs); i++) {
            audio_io_handle_t input = ahi_list_get_nth_key(audio_pol_anm->mInputs, i);
            ste_audio_input_desc_t *tmpDescr = ahi_list_get_key_value(audio_pol_anm->mInputs, input);
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
    case AUDIO_STREAM_VOICE_CALL:
        if (audio_pol_anm->mPhoneState == AUDIO_MODE_IN_COMMUNICATION) {
            /* VoIP type IN_COMMUNICATION */
            descr->mVoIPType = VOIP_IN_COMMUNICATION;
            /* Check for corresponding input */
            for (i = 0; i < ahi_list_count(audio_pol_anm->mInputs); i++) {
                audio_io_handle_t input = ahi_list_get_nth_key(audio_pol_anm->mInputs, i);
                ste_audio_input_desc_t *tmpDescr = ahi_list_get_key_value(audio_pol_anm->mInputs, input);
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
            for (i = 0; i < ahi_list_count(audio_pol_anm->mInputs); i++) {
                audio_io_handle_t input = ahi_list_get_nth_key(audio_pol_anm->mInputs, i);
                ste_audio_input_desc_t *tmpDescr = ahi_list_get_key_value(audio_pol_anm->mInputs, input);
                if (tmpDescr != NULL) {
                    if(tmpDescr->mRefCount && tmpDescr->mDevice != AUDIO_DEVICE_IN_FM_RX) {
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

bool is_voip_input(audio_policy_anm *audio_pol_anm, ste_audio_input_desc_t *descr,
    bool *outputAvailable, ste_audio_output_descriptor_t **outDescr)
{
    int i = 0;
    *outputAvailable = false;
    ste_adm_vc_modem_status_t lModemStatus;

    if ((audio_pol_anm->mModemType == ADM_FAT_MODEM) || (audio_pol_anm->mModemType == ADM_FULL_FAT_MODEM)) {
        lModemStatus = STE_ADM_VC_MODEM_STATUS_DISCONNECTED;
    } else {
        lModemStatus = audio_pol_anm->mModemStatus;
    }

    if (audio_pol_anm->mCSCallActive
        || lModemStatus == STE_ADM_VC_MODEM_STATUS_CONNECTED
        ) {
        /* No VoIP input */
        descr->mVoIPType = VOIP_INACTIVE;
        return false;
    }


    if (descr->mInputSource == AUDIO_SOURCE_VOICE_COMMUNICATION && audio_pol_anm->mPhoneState == AUDIO_MODE_IN_COMMUNICATION) {
        /* VoIP type IN_COMMUNICATION */
        descr->mVoIPType = VOIP_IN_COMMUNICATION;
        /* Check for corresponding output */
        for (i = 0; i < ahi_list_count(audio_pol_anm->mOutputs); i++) {
            audio_io_handle_t output = ahi_list_get_nth_key(audio_pol_anm->mOutputs, i);
            ste_audio_output_descriptor_t *tmpDescr = ahi_list_get_key_value(audio_pol_anm->mOutputs, output);
            if (tmpDescr != NULL) {
                if (tmpDescr->mVoIPType == VOIP_IN_COMMUNICATION ||
                    tmpDescr->mVoIPType == VOIP_VOICE_CALL) {
                    if (anm_out_desc_ref_count(tmpDescr)) {
                        /* Found started VOICE_CALL VoIP output */
                        *outputAvailable = true;
                        /* Make sure the found VoIP output has correct type */
                        if (tmpDescr->mVoIPType == VOIP_VOICE_CALL &&
                            audio_pol_anm->mPhoneState == AUDIO_MODE_IN_COMMUNICATION) {
                            ALOG_INFO("is_voip_input(): Updating VoIP type of found VoIP output from %d to %d",
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
    }
#ifdef STE_VIDEO_CALL
    else if (descr->mInputSource == AUDIO_SOURCE_VIDEO_CALL) {
        // VoIP type VIDEO_CALL
        descr->mVoIPType = VOIP_VIDEO_CALL;
        // Check for corresponding output
        for (i = 0; i < ahi_list_count(audio_pol_anm->mOutputs); i++) {
            audio_io_handle_t output = ahi_list_get_nth_key(audio_pol_anm->mOutputs, i);
            ste_audio_output_descriptor_t *tmpDescr = ahi_list_get_key_value(audio_pol_anm->mOutputs, output);
            if (tmpDescr != NULL) {
                if (tmpDescr->mVoIPType == VOIP_VIDEO_CALL) {
                    if (anm_out_desc_ref_count(tmpDescr)) {
                        // Found started VIDEO_CALL VoIP output
                        *outputAvailable = true;
                        *outDescr = tmpDescr;
                        break;
                    }
                }
            }
        }
        return true;
    }
#endif
    else {
        /* Check VoIP type VOICE_CALL */
        for (i = 0; i < ahi_list_count(audio_pol_anm->mOutputs); i++) {
            audio_io_handle_t output = ahi_list_get_nth_key(audio_pol_anm->mOutputs, i);
            ste_audio_output_descriptor_t *tmpDescr = ahi_list_get_key_value(audio_pol_anm->mOutputs, output);
            if (tmpDescr != NULL) {
                if (tmpDescr->mVoIPType == VOIP_VOICE_CALL) {
                    if (anm_out_desc_ref_count(tmpDescr)) {
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

bool is_voip_synched(int type)
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

audio_policy_anm *to_apm_anm(struct audio_policy *pol)
{
    struct ste_audio_policy_manager *apm = (struct ste_audio_policy_manager *)pol;
    return apm->policy_anm;
}

/* ---------------------------------------------------------------------------
 * AudioPolicyInterface implementation
 * ---------------------------------------------------------------------------*/

int ste_anm_ap_set_device_connection_state(
    struct audio_policy          *pol,
    audio_devices_t              device,
    audio_policy_dev_state_t     state,
    const char*                  device_address)
{
    audio_policy_anm *audio_pol_anm = to_apm_anm(pol);
    int status=-1;

    LOCK_MUTEX(audio_pol_anm->mMutexDevState);

    ALOG_INFO("ste_anm_ap_set_device_connection_state(): device %x %s, state %s, address %s\n",
        device, device2str(device), state2str(state), device_address);
    /* Connect/disconnect only 1 device at a time */
    if (popcount(device) != 1) {
        ALOG_ERR("ste_anm_ap_set_device_connection_state(): trying to connect %d devices "
            "at the same time", popcount(device));
        UNLOCK_MUTEX(audio_pol_anm->mMutexDevState);
        return EINVAL;
    }

    /* Handle output devices */
    if (audio_is_output_device(device)) {

        switch (state) {

        /* Handle new output device connection */
        case AUDIO_POLICY_DEVICE_STATE_AVAILABLE:

            /* Check if the device is already connected */
            if (audio_pol_anm->mAvailableOutputDevices & device) {
                ALOG_WARN("ste_anm_ap_set_device_connection_state(): device %x %s already "
                    "connected", device, device2str(device));
                UNLOCK_MUTEX(audio_pol_anm->mMutexDevState);
                return 0;
            }
            /* Register new output device as available */
            audio_pol_anm->mAvailableOutputDevices |= device;
            if(ext_hal_is_external_device(audio_pol_anm->ext_hal,device, NULL)){
                audio_io_handle_t external_output=-1;
                ste_audio_output_descriptor_t * external_desc=NULL;
                external_desc = ext_hal_open_output(audio_pol_anm->ext_hal,device,&external_output);
                status = ext_hal_connect_output(audio_pol_anm->ext_hal,device,device_address);
                ALOG_INFO("outputDesc 0x%x ; output %d  ; status %d ",(unsigned int )external_desc,external_output,status);
                if(external_desc && status == 0 && external_output>0) {
                    ALOG_INFO("Add outputDesc 0x%x associated to output %d to the list ",(unsigned int )external_desc,external_output);
                    ahi_list_add(&(audio_pol_anm->mOutputs), external_desc, external_output);
                }
                else {
                    ext_hal_close_output(audio_pol_anm->ext_hal,device,device_address);
                    audio_pol_anm->mAvailableOutputDevices &= ~device;
                    UNLOCK_MUTEX(audio_pol_anm->mMutexDevState);
                    return status;
                }
#ifdef ALLOW_DUPLICATION
                if(ext_hal_used_for_sonification(audio_pol_anm->ext_hal,device)){
                    ste_audio_output_descriptor_t *duplicated_desc=NULL;
                    audio_io_handle_t duplicated_output = open_duplicated_outputs(audio_pol_anm,external_output,&duplicated_desc);
                    if(duplicated_output>0) {
                        ahi_list_add(&(audio_pol_anm->mOutputs), duplicated_desc, duplicated_output);
                    }
                    else {
                        ALOG_ERR("Impossible to create duplicated output for external output %d",external_output);
                        UNLOCK_MUTEX(audio_pol_anm->mMutexDevState);
                        return duplicated_output;
                    }
                }
#endif
            }
            else if( !is_primary_device(device)) {
                ALOG_ERR("device 0x%x is neither a supported primary nor a supported external device",(unsigned int)device);
                audio_pol_anm->mAvailableOutputDevices &= ~device;
                UNLOCK_MUTEX(audio_pol_anm->mMutexDevState);
                return status;
            }
            break;

        /* Handle output device disconnection */
        case AUDIO_POLICY_DEVICE_STATE_UNAVAILABLE:

            /* Check if the device is connected */
            if (!(audio_pol_anm->mAvailableOutputDevices & device)) {
                ALOG_WARN("ste_anm_ap_set_device_connection_state(): device not connected: "
                    "%x %s\n", device, device2str(device));
                UNLOCK_MUTEX(audio_pol_anm->mMutexDevState);
                return 0;
            }

            /*  Remove device from available output devices */
            audio_pol_anm->mAvailableOutputDevices &= ~device;
            if(ext_hal_is_external_device(audio_pol_anm->ext_hal,device, NULL)){
                status = ext_hal_disconnect_output(audio_pol_anm->ext_hal,device,device_address);
                if(status != 0) {
                    audio_pol_anm->mAvailableOutputDevices |= device;
                    UNLOCK_MUTEX(audio_pol_anm->mMutexDevState);
                    return status;
                }
            }
            break;

        default:
            ALOG_ERR("ste_anm_ap_set_device_connection_state(): invalid state: %x\n", state);
            UNLOCK_MUTEX(audio_pol_anm->mMutexDevState);
            return EINVAL;
        }

        check_output_for_all_strategies(audio_pol_anm);
        if(ext_hal_is_external_device(audio_pol_anm->ext_hal,device, NULL)){
            if( state == AUDIO_POLICY_DEVICE_STATE_UNAVAILABLE ) {
                audio_io_handle_t handle=ext_hal_get_io_handle(audio_pol_anm->ext_hal,device);
#ifdef ALLOW_DUPLICATION
                close_duplicated_outputs(audio_pol_anm,handle);
#endif
                // Close output at ExtHAL side
                ext_hal_close_output(audio_pol_anm->ext_hal,device, device_address);
                // Remove reference from ANM
                ahi_list_del_key(&(audio_pol_anm->mOutputs), handle);
            }
        }
        //Just to parse if suspend state of external devices have to be updated
        ext_hal_update_suspend(audio_pol_anm->ext_hal);
        /* Update routing to the new preferred device for all outputs */
        update_output_routing(audio_pol_anm
#ifdef USE_CACHE_MECHANISM
        , false
#endif
            );

        /* Make sure that, if applicable, the corresponding input device is
         * handled as well.
         */
        switch (device) {
        case AUDIO_DEVICE_OUT_WIRED_HEADSET:
            device = AUDIO_DEVICE_IN_WIRED_HEADSET;
            break;
        case AUDIO_DEVICE_OUT_BLUETOOTH_SCO:
        case AUDIO_DEVICE_OUT_BLUETOOTH_SCO_HEADSET:
        case AUDIO_DEVICE_OUT_BLUETOOTH_SCO_CARKIT:
            device = AUDIO_DEVICE_IN_BLUETOOTH_SCO_HEADSET;
            break;
        default:
            /* No corresponding input device. Update CS call routing */
            if (audio_pol_anm->mCSCallActive || audio_pol_anm->mVoIPActive) {
                update_cscall_routing(pol);
            }
            UNLOCK_MUTEX(audio_pol_anm->mMutexDevState);
            return 0;
        }
    }

    /* Handle input devices */
    if (audio_is_input_device(device)) {

        switch (state) {

        /* Handle input device connection */
        case AUDIO_POLICY_DEVICE_STATE_AVAILABLE:

            /* Check if the device is already connected */
            if (audio_pol_anm->mAvailableInputDevices & device) {
                ALOG_WARN("ste_anm_ap_set_device_connection_state(): device already connected: "
                    "%x %s\n", device, device2str(device));
                UNLOCK_MUTEX(audio_pol_anm->mMutexDevState);
                return ENOSYS;
            }

            /* Register new input device as available */
            audio_pol_anm->mAvailableInputDevices |= device;

            break;

        /* Handle input device disconnection */
        case AUDIO_POLICY_DEVICE_STATE_UNAVAILABLE:

            /* Check if the device is connected */
            if (!(audio_pol_anm->mAvailableInputDevices & device)) {
                ALOG_WARN("ste_anm_ap_set_device_connection_state(): device not connected: "
                    "%x %s\n", device, device2str(device));
                UNLOCK_MUTEX(audio_pol_anm->mMutexDevState);
                return ENOSYS;
            }

            /*  Remove device from available input devices */
            audio_pol_anm->mAvailableInputDevices &= ~device;

            break;

        default:
            ALOG_ERR("ste_anm_ap_set_device_connection_state(): invalid state: %x\n", state);
            UNLOCK_MUTEX(audio_pol_anm->mMutexDevState);
            return EINVAL;
        }

        /* Update CS call routing */
        if (audio_pol_anm->mCSCallActive || audio_pol_anm->mVoIPActive) {
            update_cscall_routing(pol);
        }
        /* Update routing to the new preferred device for all inputs */
        update_input_routing(audio_pol_anm);

        UNLOCK_MUTEX(audio_pol_anm->mMutexDevState);
        return 0;
    }

    ALOG_WARN("ste_anm_ap_set_device_connection_state(): invalid device: %x\n", device);
    UNLOCK_MUTEX(audio_pol_anm->mMutexDevState);
    return EINVAL;
}


audio_policy_dev_state_t
    ste_anm_ap_get_device_connection_state(
        const struct audio_policy *pol,
        audio_devices_t device, const char *device_address)
{
    audio_policy_anm *audio_pol_anm = ((struct ste_audio_policy_manager *)pol)->policy_anm;

    audio_policy_dev_state_t state =  AUDIO_POLICY_DEVICE_STATE_UNAVAILABLE;

    if (audio_is_output_device(device)) {
        if (device & audio_pol_anm->mAvailableOutputDevices) {
            state = AUDIO_POLICY_DEVICE_STATE_AVAILABLE;
        }
    } else if (audio_is_input_device(device)) {
        if (device & audio_pol_anm->mAvailableInputDevices) {
            state = AUDIO_POLICY_DEVICE_STATE_AVAILABLE;
        }
    }
    if(ext_hal_is_external_device(audio_pol_anm->ext_hal,device,NULL)){
        state =  ext_hal_get_device_connection_state( audio_pol_anm->ext_hal,device,device_address);
    }

    ALOG_INFO_VERBOSE("ste_anm_ap_get_device_connection_state(): device = %s, state = %s\n",
        device2str(device), state2str(state));
    return state;
}

void ste_anm_ap_set_phone_state(struct audio_policy *pol, int state)
{
    int i = 0;
    audio_policy_anm *audio_pol_anm = to_apm_anm(pol);

    if (state < 0 || state >= AUDIO_MODE_CNT) {
        ALOG_WARN("ste_anm_ap_set_phone_state(): Invalid state %d\n", state);
        return;
    }
    if (state == (int)audio_pol_anm->mPhoneState ) {
        ALOG_WARN("ste_anm_ap_set_phone_state(): Trying to set same state as current state %s\n",
            mode2str((audio_mode_t)state));
        return;
    }

    int oldState = (int)audio_pol_anm->mPhoneState;
    /* Update phone state to new state */
    audio_pol_anm->mPhoneState = state;

    ALOG_INFO("ste_anm_ap_set_phone_state(): %s (old state was %s)\n",
        mode2str((audio_mode_t)state),
        mode2str((audio_mode_t)oldState));

    if (oldState == AUDIO_MODE_IN_CALL && state != AUDIO_MODE_IN_CALL) {
        /* Close CS call, since phone state has changed from MODE_IN_CALL */
        vc_close_request(audio_pol_anm);
    }

    if (oldState == AUDIO_MODE_IN_COMMUNICATION && state != AUDIO_MODE_IN_COMMUNICATION) {
        /* Close VoIP call, since phone state has changed from MODE_IN_COMMUNICATION */

        /* Find VoIP input of type IN_COMMUNICATION, and reroute */
        for (i = 0; i < ahi_list_count(audio_pol_anm->mInputs); i++) {
            ste_audio_input_desc_t *tmpDescr = ahi_list_get_nth_node(audio_pol_anm->mInputs,i);
            if (tmpDescr != NULL) {
                if (tmpDescr->mVoIPType==VOIP_IN_COMMUNICATION &&
                    tmpDescr->mVoIPActive) {
                        ALOG_INFO("ste_anm_ap_set_phone_state(): reroute input to other device than VoIP device");
                        tmpDescr->mVoIPActive=false;
                        update_input_routing(audio_pol_anm);
                }
            }
        }

        /* Find VoIP output of type IN_COMMUNICATION, and reroute */
        for (i = 0; i < ahi_list_count(audio_pol_anm->mOutputs); i++) {
            ste_audio_output_descriptor_t *tmpDescr = ahi_list_get_nth_node(audio_pol_anm->mOutputs, i);
            if (tmpDescr != NULL) {
                if (tmpDescr->mVoIPType==VOIP_IN_COMMUNICATION &&
                    tmpDescr->mVoIPActive) {
                        ALOG_INFO("ste_anm_ap_set_phone_state(): reroute output to other device than VoIP device");
                        tmpDescr->mVoIPActive=false;
                        update_output_routing(audio_pol_anm
#ifdef USE_CACHE_MECHANISM
            , false
#endif
            );
                }
            }
        }

        if (audio_pol_anm->mVoIPActive) {
            ALOG_INFO("ste_anm_ap_set_phone_state(): close VoIP call graph");
            close_cs_call(audio_pol_anm);
            set_voip_status(pol,false, 0,0,0,0);
        }
    }

    /* Update device routing for cscall, outputs and inputs */
    if (audio_pol_anm->mPhoneState == AUDIO_MODE_IN_CALL) {
        vc_open_request(pol);
    }
    update_output_routing(audio_pol_anm
#ifdef USE_CACHE_MECHANISM
            , false
#endif
            );
    update_input_routing(audio_pol_anm);

    /* update volume for speech */
    if (audio_pol_anm->mStreams[AUDIO_STREAM_VOICE_CALL].mMuteCount == 0) {
        ste_anm_ap_set_stream_volume_index(pol, AUDIO_STREAM_VOICE_CALL, audio_pol_anm->mStreams[AUDIO_STREAM_VOICE_CALL].mIndexCur);
    }
}

void ste_anm_ap_set_ringer_mode(struct audio_policy *pol, uint32_t mode, uint32_t mask)
{
    audio_policy_anm *audio_pol_anm = to_apm_anm(pol);
    ALOG_INFO("ste_anm_ap_set_ringer_mode(): mode %x, mask %x\n", mode, mask);
    audio_pol_anm->mRingerMode = mode;
}

void ste_anm_ap_set_force_use(
    struct audio_policy *pol,
    audio_policy_force_use_t usage, audio_policy_forced_cfg_t config)
{
    audio_policy_anm *audio_pol_anm = to_apm_anm(pol);

    ALOG_INFO("ste_anm_ap_set_force_use(): force_use %d (%s), forced_config %d (%s)\n",
        usage, force_use2str(usage), config, forced_config2str(config));

    LOCK_MUTEX(audio_pol_anm->mMutexForceUse);

    audio_pol_anm->mForceUseQueued[usage] = config;

    if (!audio_pol_anm->mForceUseOngoing) {
        audio_pol_anm->mForceUseOngoing = true;

        pthread_t thread;
        pthread_attr_t tattr;
        pthread_attr_init(&tattr);
        pthread_attr_setdetachstate(&tattr, PTHREAD_CREATE_DETACHED);
        if (pthread_create(&thread, &tattr, thread_force_use, pol)) {
            ALOG_ERR("ste_anm_ap_set_force_use(): pthread_create() failed\n");
        }
    }

    UNLOCK_MUTEX(audio_pol_anm->mMutexForceUse);
}

audio_policy_forced_cfg_t ste_anm_ap_get_force_use(
    const struct audio_policy *pol,
    audio_policy_force_use_t usage)
{
    audio_policy_anm *audio_pol_anm = ((struct ste_audio_policy_manager *)pol)->policy_anm;

    ALOG_INFO_VERBOSE("ste_anm_ap_get_force_use(): force_use = %d (%s), forced_config = %d (%s)\n",
        usage, force_use2str(usage), audio_pol_anm->mForceUse[usage],
        forced_config2str(audio_pol_anm->mForceUse[usage]));
    return audio_pol_anm->mForceUse[usage];
}

void* thread_force_use(void* param)
{
    struct audio_policy *pol = (struct audio_policy *)param;
    ALOG_INFO_VERBOSE("ENTER thread_force_use():");
    update_force_use(pol);
    ALOG_INFO_VERBOSE("LEAVE thread_force_use():");
    return NULL;
}

void update_force_use(struct audio_policy *pol)
{
    int i = 0, j;
    audio_policy_anm *audio_pol_anm = to_apm_anm(pol);
    LOCK_MUTEX(audio_pol_anm->mMutexDevState);
    LOCK_MUTEX(audio_pol_anm->mMutexForceUse);
    for (i = 0; i < AUDIO_POLICY_FORCE_USE_CNT; i++) {
        audio_pol_anm->mForceUse[i] = audio_pol_anm->mForceUseQueued[i];
    }
    UNLOCK_MUTEX(audio_pol_anm->mMutexForceUse);

    bool forceUseChanged = true;
    while (forceUseChanged) {
        ALOG_INFO_VERBOSE("update_force_use(): update routing");
        /* Update device routing for cscall, outputs and inputs */
        if (audio_pol_anm->mCSCallActive || audio_pol_anm->mVoIPActive) {
            update_cscall_routing(pol);
        }
        update_output_routing(audio_pol_anm
#ifdef USE_CACHE_MECHANISM
            , false
#endif
            );
        update_input_routing(audio_pol_anm);

        forceUseChanged = false;
        LOCK_MUTEX(audio_pol_anm->mMutexForceUse);
        for (i = 0; i < AUDIO_POLICY_FORCE_USE_CNT; i++) {
            if (audio_pol_anm->mForceUseQueued[i] != audio_pol_anm->mForceUse[i]) {
                forceUseChanged = true;
                for (j = 0; j < AUDIO_POLICY_FORCE_USE_CNT; j++) {
                    audio_pol_anm->mForceUse[j] = audio_pol_anm->mForceUseQueued[j];
                }
                break;
            }
        }
        audio_pol_anm->mForceUseOngoing = forceUseChanged;
        UNLOCK_MUTEX(audio_pol_anm->mMutexForceUse);
    }
    UNLOCK_MUTEX(audio_pol_anm->mMutexDevState);
}

void ste_anm_ap_set_can_mute_enforced_audible(struct audio_policy *pol, bool can_mute)
{
    ALOG_INFO_VERBOSE("ste_anm_ap_set_can_mute_enforced_audible(): can_mute %d\n", can_mute);

    audio_policy_anm *audio_pol_anm = to_apm_anm(pol);

    audio_pol_anm->mStreams[AUDIO_STREAM_ENFORCED_AUDIBLE].mCanBeMuted = can_mute;
}

bool is_supported_coded_format(uint32_t format)
{
    switch (format) {
        case AUDIO_FORMAT_AC3:     return true;
        case AUDIO_FORMAT_MPEG1:   return true;
        case AUDIO_FORMAT_MPEG2:   return true;
        case AUDIO_FORMAT_DTS:     return true;
        case AUDIO_FORMAT_ATRAC:   return true;

        case AUDIO_FORMAT_OBA:     return true;
        case AUDIO_FORMAT_DDPLUS:  return true;
        case AUDIO_FORMAT_DTS_HD:  return true;
        case AUDIO_FORMAT_MAT:     return true;
        case AUDIO_FORMAT_DST:     return true;

        case AUDIO_FORMAT_WMA_PRO: return true;
        default:                   return false;
    }
}

int ste_anm_ap_init_check(const struct audio_policy *pol)
{
    return 0;
}

/* ---------------------------------------------------------------------------
 * Output handling
 * ---------------------------------------------------------------------------*/
audio_io_handle_t ste_anm_ap_get_output(
    struct audio_policy *pol,
    audio_stream_type_t stream,
    uint32_t samplingRate,
    uint32_t format,
    uint32_t channels,
    audio_output_flags_t flags)
{
    audio_policy_anm *audio_pol_anm = to_apm_anm(pol);
    int i = 0;
    audio_io_handle_t output = 0;
    ste_audio_output_descriptor_t *descr = NULL;

    (void) ste_anm_debug_setup_log();

    ALOG_INFO("ENTER ste_anm_ap_get_output(): %s, samplerate=%d, format=%d, channels=%x, flags=%x\n",
        stream2str(stream), samplingRate, format, channels, flags);

    /* Check the format. */
    if (format != AUDIO_FORMAT_DEFAULT &&
        !(audio_is_linear_pcm(format) || is_supported_coded_format(format))) {
        ALOG_ERR("ste_anm_ap_get_output(): Format %X is not supported", format);
        return 0;
    }

    /* Check number of channels */
    if (channels != 0 &&
        channels != AUDIO_CHANNEL_OUT_MONO &&
        channels != AUDIO_CHANNEL_OUT_STEREO &&
        channels != AUDIO_CHANNEL_OUT_QUAD &&
        channels != AUDIO_CHANNEL_OUT_5POINT1 &&
        channels != AUDIO_CHANNEL_OUT_SURROUND &&
        channels != AUDIO_CHANNEL_OUT_7POINT1 &&
        channels != (channels & AUDIO_CHANNEL_OUT_ALL)) {
        ALOG_ERR("ste_anm_ap_get_output(): Channels=%x is not supported", channels);
        return 0;
    }

    /* Check input values */
    if (format == AUDIO_FORMAT_DEFAULT) {
        format = DEFAULT_PCM_FORMAT;
        ALOG_INFO_VERBOSE("ste_anm_ap_get_output(): Format is 0 - update to %x", format);
    }
    if (channels == 0) {
        channels = DEFAULT_OUTPUT_CHANNELS;
        ALOG_INFO_VERBOSE("ste_anm_ap_get_output(): Channels is 0 - update to %x", channels);
    }
    if (samplingRate == 0) {
        samplingRate = DEFAULT_OUTPUT_SAMPLE_RATE;
        ALOG_INFO_VERBOSE("ste_anm_ap_get_output(): Sample rate is 0 - update to %d", samplingRate);
    }

    /* Get the corresponding strategy for this stream type */
    routing_strategy strategy = getStrategy(stream);

    LOCK_MUTEX(audio_pol_anm->mMutexDevState);
    /* Check if an indirect/shared output is allowed to be used */
    if ( !(flags & AUDIO_OUTPUT_FLAG_DIRECT) ) {
        /* Reuse output if already opened for this strategy */
        for (i = 0; i < ahi_list_count(audio_pol_anm->mOutputs); i++) {
            descr = ahi_list_get_nth_node(audio_pol_anm->mOutputs, i);
            if(descr) {
                if (get_strategy_ref_count(descr)>=1 && descr->mStrategy==strategy && descr->mDevice) {
                    if (audio_is_linear_pcm(descr->mFormat)) {
                        output = ahi_list_get_nth_key(audio_pol_anm->mOutputs, i);
                        update_strategy_ref_count(descr,1,flags);
                        ALOG_INFO("ste_anm_ap_get_output(): Reused existing output %d for %s, StrategyRefCount = %d",
                            output, strategy2str(strategy),descr->mStrategyRefCount);
                        break;
                    }
                }
            }
            else {
                ALOG_WARN("ste_anm_ap_get_output() : output descriptor is null");
            }
        }
    }

    uint32_t device = get_output_device_for_strategy(audio_pol_anm, (audio_io_handle_t)NULL, strategy, format
#ifdef USE_CACHE_MECHANISM
        , true
#endif
        );
    UNLOCK_MUTEX(audio_pol_anm->mMutexDevState);
    ALOG_INFO("ste_anm_ap_get_output():  for strategy %s, we get device(s) : 0x%x %s", strategy2str(strategy),(unsigned int)device,device2str(device));
    if (device == 0) {
        ALOG_ERR("ste_anm_ap_get_output():  no device found");
    }

    if (output == 0) {
        /* Open new hardware output */
        descr = (ste_audio_output_descriptor_t*) anm_calloc(sizeof(ste_audio_output_descriptor_t),1);
        init_audio_output_descriptor(descr);

        descr->mSamplingRate = samplingRate;
        descr->mFormat = format;
        descr->mChannels = channels;
        descr->mFlags = flags | AUDIO_OUTPUT_FLAG_PRIMARY;
        descr->mDevice = device;
        set_strategy_ref_count(descr,1);
        descr->mStrategy=strategy;

        output = audio_pol_anm->mp_client_interface->open_output(audio_pol_anm->mService,
            &descr->mDevice,
            &descr->mSamplingRate, &descr->mFormat, &descr->mChannels,
            &descr->mLatency, descr->mFlags);

        if (output) {
            ALOG_INFO("ste_anm_ap_get_output() Opened new output %d for strategy %s, mStrategyRefCount = %d",
                output, strategy2str(strategy),descr->mStrategyRefCount);
            /* Add to the list of available outputs */
            descr->output_id=output;
            ahi_list_add(&(audio_pol_anm->mOutputs), descr, output);
        } else {
            ALOG_ERR("ste_anm_ap_get_output(): Failed to open output for %s, device %x."
                " SR (%d) F (%d) C (%08X) L (%d) FL (%08X)\n",
                strategy2str(strategy), descr->mDevice, descr->mSamplingRate,
                descr->mFormat, descr->mChannels, descr->mLatency,
                descr->mFlags);
            anm_free(descr);
            return 0;
        }
    }

    if ( !(flags & AUDIO_OUTPUT_FLAG_DIRECT) ) {
        if(ext_hal_is_external_device(audio_pol_anm->ext_hal,device,NULL))
        {
            audio_io_handle_t external_output = ext_hal_get_io_handle(audio_pol_anm->ext_hal,device);
            ste_audio_output_descriptor_t * external_desc = ext_hal_get_output_desc(audio_pol_anm->ext_hal,external_output);

            if(external_output<0 || external_desc==NULL){
                ALOG_WARN("getOutput() External device %x selected but output not opened",
                            device);
                return 0;
            }
            if(descr==NULL) {
                ALOG_WARN("getOutput() Impossible to retrieve descriptor from selected output ");
                return 0;
            }
            if (popcount((audio_devices_t)device) < 2) {
                update_strategy_ref_count(descr,-1,flags);
                ALOG_INFO("Output %d  mStrategyRefCount becomes %d",
                        output, descr->mStrategyRefCount);
                descr = ahi_list_get_key_value(audio_pol_anm->mOutputs, external_output);
                if(descr==NULL) {
                    ALOG_WARN("getOutput() Impossible to retrieve descriptor of the xxternal device %x",device);
                    return 0;
                }
                //switch to external output
                output=external_output;
                update_strategy_ref_count(descr,1,flags);
                ALOG_INFO("Output = %d,  mStrategyRefCount becomes %d",
                        output, descr->mStrategyRefCount);
            }
#ifdef ALLOW_DUPLICATION
            else{
                if(ext_hal_used_for_sonification(audio_pol_anm->ext_hal,device)){
                    if(external_desc->duplication_info.duplicated_output[0] == 0) {
                        // Duplicated output not yet created => do it
                        ste_audio_output_descriptor_t *duplicated_desc=NULL;
                        audio_io_handle_t duplicated_output = open_duplicated_outputs(audio_pol_anm,external_output,&duplicated_desc);
                        if(duplicated_output>0) {
                            ahi_list_add(&(audio_pol_anm->mOutputs),duplicated_desc,duplicated_output);
                        }
                        else {
                            ALOG_ERR("Impossible to create duplicated output for external output %d",external_output);
                        }
                    }
                    else {
                        // Duplicated output already exists, switch to it and update usage
                        output = external_desc->duplication_info.duplicated_output[0];
                        descr = ahi_list_get_key_value(audio_pol_anm->mOutputs, output);
                        if(descr) {
                            update_strategy_ref_count(descr,1,flags);
                            update_strategy_ref_count(descr->duplication_info.primary_desc,1,flags);
                            ALOG_INFO("Re-use duplicated output %d, StrategyRefCount = %d",
                                output, descr->mStrategyRefCount);
                        }
                        else {
                            ALOG_WARN("ste_anm_ap_get_output() : output descriptor is null");
                        }
                    }
                }
            }
#endif
        }
    }
    if(descr) {
        ALOG_INFO("LEAVE ste_anm_ap_get_output(): Returned output %d, %s, %s, "
            "device=%x, SR (%d) F (%d) C (%x) L (%d) FL (%x)\n",
            output, strategy2str(strategy), stream2str(stream), descr->mDevice,
            descr->mSamplingRate, descr->mFormat, descr->mChannels, descr->mLatency,
            descr->mFlags);
    }
    else {
        ALOG_ERR("LEAVE ste_anm_ap_get_output(): Output descriptor NULL");
    }
    return output;
}

int ste_anm_ap_start_output(struct audio_policy *pol, audio_io_handle_t output, audio_stream_type_t stream, int session)
{
    ALOG_INFO("ENTER ste_anm_ap_start_output(): output %d, stream %s\n", output, stream2str(stream));

    audio_policy_anm *audio_pol_anm = to_apm_anm(pol);

    LOCK_MUTEX(audio_pol_anm->mMutexDevState);

    ssize_t index = ahi_list_get_key_index(audio_pol_anm->mOutputs, output);
    struct str_parms *streamStateParam = NULL;

    if (index < 0) {
        ALOG_WARN("LEAVE ste_anm_ap_start_output(): Unknown output %d\n", output);
        UNLOCK_MUTEX(audio_pol_anm->mMutexDevState);
        return EINVAL;
    }

    /* Incremenent ref count of this stream on the requested output */
    ste_audio_output_descriptor_t *descr = ahi_list_get_key_value(audio_pol_anm->mOutputs, output);
    anm_out_desc_change_ref_count(descr, stream, 1);
    if (get_ref_count(descr,stream)>1) {
        bool inputAvailable = false;
        ste_audio_input_desc_t *inDescr = NULL;
        ste_audio_output_descriptor_t oldOutDescr = *descr;
        if (is_voip_output(audio_pol_anm, descr, stream, &inputAvailable, &inDescr)) {
            if (oldOutDescr.mVoIPType != descr->mVoIPType) {
                ALOG_INFO("LEAVE ste_anm_ap_start_output(): output already started, "
                    "do nothing but update VoIP type from %s to %s",
                    voiptype2str(oldOutDescr.mVoIPType),
                    voiptype2str(descr->mVoIPType));
                UNLOCK_MUTEX(audio_pol_anm->mMutexDevState);
                return 0;
            }
        }
        ALOG_INFO("LEAVE ste_anm_ap_start_output(): output already started, do nothing");
        UNLOCK_MUTEX(audio_pol_anm->mMutexDevState);
        return 0;
    }

    /* Make sure the output is restored when started */
    LOCK_MUTEX(audio_pol_anm->mMutexIO);
    if (descr->mSuspended) {
        ALOG_INFO("ste_anm_ap_start_output(): output is suspended, restore output");
        audio_pol_anm->mp_client_interface->restore_output(audio_pol_anm->mService, output);
        descr->mSuspended=false;
    }
    UNLOCK_MUTEX(audio_pol_anm->mMutexIO);

    /* Check VoIP conditions */
    bool inputAvailable = false;
    ste_audio_input_desc_t *inDescr = NULL;
    if (is_voip_output(audio_pol_anm, descr, stream, &inputAvailable, &inDescr)) {

        ALOG_INFO("ste_anm_ap_start_output(): VoIP of type=%s\n",
            voiptype2str(descr->mVoIPType));

        /* Check if VoIP input should be synced, i.e. if the call graph
         * should only be setup when both input and output are started */
        bool syncedInput = is_voip_synched(descr->mVoIPType);

        if (syncedInput && inputAvailable) {
            ALOG_INFO("ste_anm_ap_start_output(): routing output and input to VoIP devices\n");
            set_voip_status(pol, true, inDescr->mSamplingRate, inDescr->mChannels, descr->mSamplingRate, descr->mChannels);

            /* Update VoIP info */
            descr->mVoIPActive = true;
            inDescr->mVoIPActive = true;
            inDescr->mVoIPType = descr->mVoIPType;
            /* Setup the call graph */
            update_cscall_routing(pol);
            /* Route input to VoIP device */
            update_input_routing(audio_pol_anm);

        } else if (syncedInput) {
            ALOG_INFO("ste_anm_ap_start_output(): VoIP output in synced mode, wait for input to switch to VoIP\n");
        } else {
            ALOG_INFO("ste_anm_ap_start_output(): routing output to VoIP device\n");
            if (inputAvailable) {
                set_voip_status(pol, true, inDescr->mSamplingRate, inDescr->mChannels, descr->mSamplingRate, descr->mChannels);
            } else {
                set_voip_status(pol, true, 0, 0, descr->mSamplingRate, descr->mChannels);
            }
            /* Update VoIP info */
            descr->mVoIPActive = true;
            /* Setup the call graph */
            update_cscall_routing(pol);
        }
    }

    /* Update the output routing */
    update_output_routing(audio_pol_anm
#ifdef USE_CACHE_MECHANISM
        , true
#endif
        );

#ifdef ALLOW_DUPLICATION
    if(descr->duplication_info.is_duplicated_output) {
        streamStateParam = str_parms_create_str(PARAM_KEY_STREAM_STATE);
        str_parms_add_int(streamStateParam, PARAM_KEY_STREAM_STATE, 1);
        audio_pol_anm->mp_client_interface->set_parameters(audio_pol_anm->mService, descr->duplication_info.primary_output, str_parms_to_str(streamStateParam), 0);
        str_parms_destroy(streamStateParam);
    }
    else
#endif
    if (!ext_hal_is_external_io_handle(audio_pol_anm->ext_hal,output))
    {
        struct str_parms *param = str_parms_create_str(PARAM_KEY_STREAM_STATE);
        str_parms_add_int(param, PARAM_KEY_STREAM_STATE, 1);

        // Get reference to audio_stream_out_anm_t in AHI
        if (anm_out_desc_get_audio_output_ref(descr) == 0) {
            char *keyValuePairs = audio_pol_anm->mp_client_interface->get_parameters(audio_pol_anm->mService, output, PARAM_KEY_STREAM_REF);
            struct str_parms *pointerParam = str_parms_create_str(keyValuePairs);
            int ptr = 0;
            int ret = str_parms_get_int(pointerParam, PARAM_KEY_STREAM_REF, &ptr);
            if (ret >= 0) {
                ALOG_INFO_VERBOSE("ste_anm_ap_start_output(): AHI returned a pointer: 0x%x", ptr);
                audio_stream_out_anm_t *out_stream = (audio_stream_out_anm_t *)ptr;
                struct str_parms *androidAP = str_parms_create_str("struct_audio_policy");
                str_parms_add_int(androidAP, "struct_audio_policy", (int)pol);
                out_stream->set_parameters(&(out_stream->stream_out.common), str_parms_to_str(androidAP));
                str_parms_destroy(androidAP);
                anm_out_desc_set_audio_output_ref(descr, ptr);
            } else {
            ALOG_INFO_VERBOSE("ste_anm_ap_start_output(): No pointer returned from AHI.");
            }
            str_parms_destroy(pointerParam);
            anm_free(keyValuePairs);
        }

        if (anm_out_desc_get_audio_output_ref(descr) == 0) {
            // Set parameters via audio policy client interface
            audio_pol_anm->mp_client_interface->set_parameters(audio_pol_anm->mService, output, str_parms_to_str(param), 0);
        } else {
           // Set parameters directly using the AHI reference
           audio_stream_out_anm_t *out_stream = (audio_stream_out_anm_t *)(anm_out_desc_get_audio_output_ref(descr));
           out_stream->set_parameters(&(out_stream->stream_out.common), str_parms_to_str(param));
        }
        str_parms_destroy(param);
        /* Check if stream was started OK */
        char *streamState = NULL;
        if (anm_out_desc_get_audio_output_ref(descr) == 0) {
            streamState = audio_pol_anm->mp_client_interface->get_parameters(audio_pol_anm->mService, output, PARAM_KEY_STREAM_STATE);
        } else {
            audio_stream_out_anm_t *out_stream = (audio_stream_out_anm_t *)(anm_out_desc_get_audio_output_ref(descr));
            streamState = out_stream->get_parameters(&(out_stream->stream_out.common), PARAM_KEY_STREAM_STATE);
        }
        if(streamState)
        {
            struct str_parms *stateParam = str_parms_create_str(streamState);
            int val;
            int ret = str_parms_get_int(stateParam, PARAM_KEY_STREAM_STATE, &val);
            if(ret >=0){
                if (val != 1) {
                    ALOG_ERR("LEAVE ste_anm_ap_start_output(): setParameter for ste_stream_state failed in ste_anm_ap_start_output()");
                    str_parms_destroy(stateParam);
                    anm_free(streamState);
                    UNLOCK_MUTEX(audio_pol_anm->mMutexDevState);
                    return UNKNOWN_ERROR;
                }
            }
            ALOG_INFO_VERBOSE("ste_anm_ap_start_output(): state : %d \n", val);
            str_parms_destroy(stateParam);
            anm_free(streamState);
        }
    }

    /* apply volume rules for current stream and device if necessary */
    if (audio_pol_anm->mStreams[stream].mMuteCount == 0) {
    ALOG_INFO_VERBOSE("ste_anm_ap_start_output(): setStreamVolumeIndex call");
        ste_anm_ap_set_stream_volume_index(pol, stream, audio_pol_anm->mStreams[stream].mIndexCur);
    }

    /* Update latency now when the device has been opened (e.g. delay
     * caused by A2DP device is not known until the device has been opened) */
    struct str_parms *latencyParam = str_parms_create_str(PARAM_KEY_SINK_LATENCY);
    str_parms_add_int(latencyParam, PARAM_KEY_SINK_LATENCY, (int)output);

    if (anm_out_desc_get_audio_output_ref(descr) == 0) {
        audio_pol_anm->mp_client_interface->set_parameters(audio_pol_anm->mService, output, str_parms_to_str(latencyParam), 0);
    } else {
        audio_stream_out_anm_t *out_stream = (audio_stream_out_anm_t *)(anm_out_desc_get_audio_output_ref(descr));
        out_stream->set_parameters(&(out_stream->stream_out.common), str_parms_to_str(latencyParam));
    }
    str_parms_destroy(latencyParam);

    ALOG_INFO("LEAVE ste_anm_ap_start_output(): output %d, stream %s\n", output, stream2str(stream));
    UNLOCK_MUTEX(audio_pol_anm->mMutexDevState);
    return 0;
}

int ste_anm_ap_stop_output(struct audio_policy *pol, audio_io_handle_t output, audio_stream_type_t stream, int session)
{
    audio_policy_anm *audio_pol_anm = to_apm_anm(pol);
    int i = 0;
    ssize_t index = ahi_list_get_key_index(audio_pol_anm->mOutputs, output);
    ste_audio_output_descriptor_t *descr = ahi_list_get_nth_node(audio_pol_anm->mOutputs, index);

    ALOG_INFO("ENTER ste_anm_ap_stop_output(): output %d, stream %s\n", output, stream2str(stream));

    LOCK_MUTEX(audio_pol_anm->mMutexDevState);

    if (index < 0) {
        ALOG_WARN("LEAVE ste_anm_ap_stop_output(): Unknown output %d\n", output);
        UNLOCK_MUTEX(audio_pol_anm->mMutexDevState);
        return EINVAL;
    }

    if (anm_out_desc_is_used_by_stream(descr, stream)) {
        /* Decrement ref count of this stream on the output */
        anm_out_desc_change_ref_count(descr, stream, -1);
        descr->mStopTime[stream] = systemTime(SYSTEM_TIME_MONOTONIC);
    } else {
        ALOG_WARN("LEAVE ste_anm_ap_stop_output(): Ref count is already 0 for stream %s on output %d\n",
            stream2str(stream), output);
        UNLOCK_MUTEX(audio_pol_anm->mMutexDevState);
        return ENOSYS;
    }

    if (get_ref_count(descr,stream)>0){
        ALOG_INFO("LEAVE ste_anm_ap_stop_output(): Ref count for stream %s is %d, do nothing", stream2str(stream), descr->mRefCount[stream]);
        UNLOCK_MUTEX(audio_pol_anm->mMutexDevState);
        return 0;
    }

    /* Check if VoIP is active */
    if (descr->mVoIPActive) {
        ALOG_INFO("ste_anm_ap_stop_output(): VoIP is active for output %d\n", output);

        /* Check if VoIP input should be synced, i.e. if the call graph
         * should be closed regardless if input is active or not */
        bool syncedInput = is_voip_synched(descr->mVoIPType);

        /* Check if active VoIP input exists */
        bool inputFound = false;
        ste_audio_input_desc_t *inDescr;
        for (i = 0; i < ahi_list_count(audio_pol_anm->mInputs); i++) {
            audio_io_handle_t input = ahi_list_get_nth_key(audio_pol_anm->mInputs, i);
            ste_audio_input_desc_t *tmpDescr = ahi_list_get_key_value(audio_pol_anm->mInputs, input);
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
        ALOG_INFO("ste_anm_ap_stop_output(): synced=%d, activeInput=%d\n", syncedInput, inputFound);
        ALOG_INFO("ste_anm_ap_stop_output(): reroute VoIP output\n");
        descr->mVoIPActive = false;
        update_output_routing(audio_pol_anm
#ifdef USE_CACHE_MECHANISM
            , false
#endif
            );
        if (syncedInput || !inputFound) {
            if (inputFound) {
                ALOG_INFO("ste_anm_ap_stop_output(): reroute VoIP input\n");
                inDescr->mVoIPActive = false;
                update_input_routing(audio_pol_anm);
            }
            ALOG_INFO("ste_anm_ap_stop_output(): close VoIP call graph\n");
            close_cs_call(audio_pol_anm);
            set_voip_status(pol, false, 0,0,0,0);
        }
    }

    struct str_parms *param = str_parms_create_str(PARAM_KEY_STREAM_STATE);
    str_parms_add_int(param,PARAM_KEY_STREAM_STATE, 0);

#ifdef ALLOW_DUPLICATION
    if (descr->duplication_info.is_duplicated_output) {
        audio_pol_anm->mp_client_interface->set_parameters(audio_pol_anm->mService, descr->duplication_info.primary_output, str_parms_to_str(param), 0);
    }
    else
#endif
    if (!ext_hal_is_external_io_handle(audio_pol_anm->ext_hal,output))
    {
        if (anm_out_desc_get_audio_output_ref(descr) == 0) {
            // Set parameters via audio policy client interface
            audio_pol_anm->mp_client_interface->set_parameters(audio_pol_anm->mService, output, str_parms_to_str(param), 0);
        } else {
            // Set parameters directly using the AHI reference
            audio_stream_out_anm_t *out_sream = (audio_stream_out_anm_t *)(anm_out_desc_get_audio_output_ref(descr));
            out_sream->set_parameters(&(out_sream->stream_out.common), str_parms_to_str(param));
        }
    }
    str_parms_destroy(param);

    if (!anm_out_desc_ref_count(descr)) {
        bool updateRouting = false;
        /* Loop through the out devices (the first 16 bits in audio_devices) */
        for (i=0x1 ; i<= 0x8000 ; i=i<<1 ) {
            if (descr->mDevice & i) {
                uint32_t indexTopLevel = get_top_level_device_index(audio_pol_anm, i);
                if (audio_pol_anm->mCurrentStrategy[indexTopLevel] == descr->mStrategy &&
                    !(descr->mStrategy==STRATEGY_PHONE && audio_pol_anm->mPhoneState==AUDIO_MODE_IN_CALL)) {
                    /* Invalidate current strategy for device */
                    audio_pol_anm->mCurrentStrategy[indexTopLevel] = STRATEGY_UNKNOWN;
                    updateRouting = true;
                }
            }
        }
        if (updateRouting) {
            update_output_routing(audio_pol_anm
#ifdef USE_CACHE_MECHANISM
            , false
#endif
            );
        }
    }
    ALOG_INFO("LEAVE ste_anm_ap_stop_output(): output %d, stream %s\n", output, stream2str(stream));
    UNLOCK_MUTEX(audio_pol_anm->mMutexDevState);
    return 0;
}

void ste_anm_ap_release_output(struct audio_policy *pol, audio_io_handle_t output)
{
    ALOG_INFO("ENTER ste_anm_ap_release_output(): output %d", output);

    audio_policy_anm *audio_pol_anm = to_apm_anm(pol);
    /* Check if the output is a valid output */
    ssize_t index = ahi_list_get_key_index(audio_pol_anm->mOutputs, output);
    int i = 0;
    if (index < 0) {
        ALOG_WARN("LEAVE ste_anm_ap_release_output(): Unknown output %d!\n", output);
        return;
    } else {
        ste_audio_output_descriptor_t *descr = ahi_list_get_nth_node(audio_pol_anm->mOutputs, index);
        routing_strategy strategy = get_output_strategy(audio_pol_anm, output);

        if(get_strategy_ref_count(descr)==1) {
            if(!(anm_out_desc_is_used_by_strategy(descr, strategy))) {
#ifndef ALLOW_DUPLICATION
                /* don't close output because of audio output caches at two levels (AudioSystem and AudioPolicyManager)
                --> reference counter of AudioPolicyManager and AudioSystem maybe not in sync */
                //mp_client_interface->closeOutput(audio_pol_anm->mService, output);
                //free(mOutputs.valueAt(index));
                //mOutputs.removeItem(output);
                ALOG_INFO("ste_anm_ap_release_output(): don't release output (output is still opened) mStrategyRefCount=0 [output=%d]\n", output);
#else
                if (descr->duplication_info.is_duplicated_output) {
                    ste_audio_output_descriptor_t *dupOutputDesc = descr;
                    ste_audio_output_descriptor_t *hwOutputDesc = dupOutputDesc->duplication_info.primary_desc;
                    // As all active tracks on duplicated output will be deleted,
                    // and as they were also referenced on hardware output, the reference
                    // count for their stream type must be adjusted accordingly on
                    // hardware output.
                    for (i = 0; i < (int)AUDIO_STREAM_CNT; i++) {
                        int refCount = get_ref_count(dupOutputDesc,i);
                        anm_out_desc_change_ref_count(hwOutputDesc, (audio_stream_type_t)i,-refCount);
                    }
                    update_output_routing(audio_pol_anm
#ifdef USE_CACHE_MECHANISM
                            , true
#endif
                            );
                    update_strategy_ref_count(hwOutputDesc,(-1)*get_strategy_ref_count(dupOutputDesc),AUDIO_OUTPUT_FLAG_PRIMARY);
                    ALOG_INFO("ste_anm_ap_release_output(): release duplicated output, output %d, mStrategyRefCount =%d\n",
                            dupOutputDesc->duplication_info.primary_output, hwOutputDesc->mStrategyRefCount);
                }
#endif
            } else {
                ALOG_INFO("ste_anm_ap_release_output() output still opened, because used by streams");
            }
        } else {
#ifdef ALLOW_DUPLICATION
            if (anm_out_desc_is_duplicated(descr)) {
                ste_audio_output_descriptor_t  *dupOutputDesc = descr;
                ste_audio_output_descriptor_t *hwOutputDesc = descr->duplication_info.primary_desc;
                int i = 0;
                // As all active tracks on duplicated output will be deleted,
                // and as they were also referenced on hardware output, the reference
                // count for their stream type must be adjusted accordingly on
                // hardware output.
                for ( i = 0; i < AUDIO_STREAM_CNT; i++) {
                    int refCount = get_ref_count(dupOutputDesc,i);
                    anm_out_desc_change_ref_count(hwOutputDesc,(audio_stream_type_t)i,-refCount);
                }
                update_strategy_ref_count(hwOutputDesc,-1,AUDIO_OUTPUT_FLAG_PRIMARY);
                ALOG_INFO("releaseOutput() for output %d, mStrategyRefCount =%d\n", dupOutputDesc->duplication_info.primary_output,hwOutputDesc->mStrategyRefCount);
            }
#endif
            LOCK_MUTEX(audio_pol_anm->mMutexIO);
            update_strategy_ref_count(descr,-1,AUDIO_OUTPUT_FLAG_PRIMARY);
            ALOG_INFO("ste_anm_ap_release_output(): output %d still opened, mStrategyRefCount=%d ", output, descr->mStrategyRefCount);
            if(!(anm_out_desc_is_used_by_strategy(descr, strategy))) {
                if (descr->mSuspended == true ) {
                    ALOG_INFO("ste_anm_ap_release_output(): output %d is stopped and suspended. Do nothing", output);
                } else if (strategy == STRATEGY_PHONE && audio_pol_anm->mPhoneState!=AUDIO_MODE_IN_CALL) {
                    /* Suspend output and make sure device is in standby. This is needed for the VoIP scenario,
                       to prevent that a device is kept open, where some applications are starting and stopping
                       an output without sending any audio data. In this case the standby request is not
                       triggered automatically by audio flinger. Only valid for STRATEGY_PHONE, since otherwise
                       there is a risk of forcing the device to standby when e.g. switching between audio tracks. */
                    ALOG_INFO("ste_anm_ap_release_output(): output %d is stopped, suspend output and make sure device is in standby", output);
                    audio_pol_anm->mp_client_interface->suspend_output(audio_pol_anm->mService, output);
                    descr->mSuspended=true;

                    struct str_parms *param = str_parms_create_str(PARAM_KEY_STREAM_STANDBY);
                    str_parms_add_int(param,PARAM_KEY_STREAM_STANDBY, 1);
                    audio_pol_anm->mp_client_interface->set_parameters(audio_pol_anm->mService, output, str_parms_to_str(param), 0);
                    str_parms_destroy(param);
                }
            }
            UNLOCK_MUTEX(audio_pol_anm->mMutexIO);
        }

        if(ahi_list_count(audio_pol_anm->mOutputs) == 0) {
            ALOG_INFO("ste_anm_ap_release_output(): No more opened outputs");
        }
    }

    update_output_routing(audio_pol_anm
#ifdef USE_CACHE_MECHANISM
            , false
#endif
            );

    ALOG_INFO("LEAVE ste_anm_ap_release_output(): output %d", output);
}

/* ---------------------------------------------------------------------------
 * Input handling
 * ---------------------------------------------------------------------------*/
audio_io_handle_t ste_anm_ap_get_input(struct audio_policy *pol, audio_source_t source,
                                       uint32_t samplingRate, audio_format_t format, uint32_t channels,
                                       audio_in_acoustics_t acoustics)
{
    audio_policy_anm *audio_pol_anm = to_apm_anm(pol);
    audio_io_handle_t input = 0;
    uint32_t device;

    (void) ste_anm_debug_setup_log();

    ALOG_INFO("ENTER ste_anm_ap_get_input(): %s, samplerate=%d, format=%d, "
        "channels=%x, acoustics=%x\n",
        source2str(source), samplingRate, format, channels, acoustics);

    /* Check the format. Only supports PCM */
    if (format != AUDIO_FORMAT_DEFAULT && !audio_is_linear_pcm(format)) {
        ALOG_ERR("ste_anm_ap_get_input(): Format %d is not supported", format);
        return 0;
    }

    /* Check number of channels */
    if (channels != 0 &&
        channels != AUDIO_CHANNEL_IN_MONO &&
        channels != AUDIO_CHANNEL_IN_STEREO) {
        ALOG_ERR("ste_anm_ap_get_input(): Channels=%x is not supported", channels);
        return 0;
    }

    /* Check input values */
    if (format == AUDIO_FORMAT_DEFAULT) {
        format = DEFAULT_PCM_FORMAT;
        ALOG_INFO_VERBOSE("ste_anm_ap_get_input(): Format is 0 - update to %x", format);
    }
    if (channels == 0) {
        channels = DEFAULT_INPUT_CHANNELS;
        ALOG_INFO_VERBOSE("ste_anm_ap_get_input(): Channels is 0 - update to %x", channels);
    }
    if (samplingRate == 0) {
        samplingRate = DEFAULT_INPUT_SAMPLE_RATE;
        ALOG_INFO_VERBOSE("ste_anm_ap_get_input(): Sample rate is 0 - update to %d", samplingRate);
    }

    /* Open new hardware input */
    ste_audio_input_desc_t *descr = (ste_audio_input_desc_t *)anm_calloc(1, sizeof(ste_audio_input_desc_t));
    init_audio_input_descriptor(descr);

    descr->mDevice = get_input_device_for_source(
        audio_pol_anm,
        source, popcount(channels));
    if (!descr->mDevice) {
        ALOG_ERR("ste_anm_ap_get_input(): No input device for source %d!", source);
        anm_free(descr);
        return 0;
    }
    descr->mSamplingRate = samplingRate;
    descr->mFormat = format;
    descr->mChannels = channels;
    descr->mAcoustics = acoustics;
    descr->mRefCount = 0;
    descr->mInputSource = source;

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

    input = audio_pol_anm->mp_client_interface->open_input(audio_pol_anm->mService,
        &descr->mDevice, &descr->mSamplingRate, &descr->mFormat,
        &descr->mChannels, descr->mAcoustics);

    if (input) {
        ALOG_INFO("ste_anm_ap_get_input(): Opened new input %d for %s",
            input, source2str(source));
        /* Add to the list of available inputs */
        ahi_list_add(&(audio_pol_anm->mInputs), descr, input);
    } else {
        ALOG_ERR("ste_anm_ap_get_input(): Failed to open input, %s, device %x,"
            " SR (%d) F (%x) C (%x)\n",
            source2str(source), descr->mDevice, descr->mSamplingRate,
            descr->mFormat, descr->mChannels);
        anm_free(descr);
        return 0;
    }

    ALOG_INFO("LEAVE ste_anm_ap_get_input(): Returned input %d, %s, device=%x, "
        "SR (%d) F (%x) C (%x)\n",
        input, source2str(source), descr->mDevice, descr->mSamplingRate,
        descr->mFormat, descr->mChannels);

    return input;
}

int ste_anm_ap_start_input(struct audio_policy *pol, audio_io_handle_t input)
{
    ALOG_INFO("ENTER ste_anm_ap_start_input(): input %d\n", input);
    audio_policy_anm *audio_pol_anm = to_apm_anm(pol);

    LOCK_MUTEX(audio_pol_anm->mMutexDevState);

    ssize_t index = ahi_list_get_key_index(audio_pol_anm->mInputs, input);
    if (index < 0) {
        ALOG_WARN("LEAVE ste_anm_ap_start_input(): Unknown input %d\n", input);
        UNLOCK_MUTEX(audio_pol_anm->mMutexDevState);
        return EINVAL;
    }

    /* Incremenent ref count of this stream on the requested input */
    ste_audio_input_desc_t *descr = ahi_list_get_key_value(audio_pol_anm->mInputs, input);
    descr->mRefCount++;
    if (descr->mRefCount > 1) {
        ALOG_INFO("LEAVE ste_anm_ap_start_input(): input already started, do nothing");
        UNLOCK_MUTEX(audio_pol_anm->mMutexDevState);
        return 0;
    }

    /* Check VoIP conditions */
    bool outputAvailable = false;
    ste_audio_output_descriptor_t *outDescr = NULL;
    if (is_voip_input(audio_pol_anm, descr, &outputAvailable, &outDescr)) {

        ALOG_INFO("ste_anm_ap_start_input(): VoIP of type=%s\n",
            voiptype2str(descr->mVoIPType));

        /* Check if VoIP output should be synced, i.e. if the call graph
         * should only be setup when both input and output are started */
        bool syncedOutput = is_voip_synched(descr->mVoIPType);

        if (syncedOutput && outputAvailable) {
            ALOG_INFO("ste_anm_ap_start_input(): routing input and output to VoIP devices\n");
            set_voip_status(pol, true, descr->mSamplingRate, descr->mChannels, outDescr->mSamplingRate, outDescr->mChannels);

            /* Update VoIP info */
            descr->mVoIPActive = true;
            outDescr->mVoIPActive = true;
            outDescr->mVoIPType = descr->mVoIPType;
            /* Setup the call graph */
            update_cscall_routing(pol);
            /* Route output to VoIP device */
            update_output_routing(audio_pol_anm
#ifdef USE_CACHE_MECHANISM
            , false
#endif
            );
        } else if (syncedOutput) {
            ALOG_INFO("ste_anm_ap_start_input(): VoIP input in synced mode, wait for output to switch to VoIP\n");
        } else {
            ALOG_INFO("ste_anm_ap_start_input(): routing input to VoIP device\n");
            if (outputAvailable) {
                set_voip_status(pol, true, descr->mSamplingRate, descr->mChannels, outDescr->mSamplingRate, outDescr->mChannels);
            } else {
                set_voip_status(pol, true, descr->mSamplingRate, descr->mChannels, 0, 0);
            }
            /* Update VoIP info */
            descr->mVoIPActive = true;
            /* Setup the call graph */
            update_cscall_routing(pol);
        }
    }

    /* Update the input routing */
    update_input_routing(audio_pol_anm);

    /* Set stream state parameter */
    struct str_parms *param = NULL;
    param = str_parms_create_str(PARAM_KEY_STREAM_STATE);
    str_parms_add_int(param, PARAM_KEY_STREAM_STATE, 1);
    audio_pol_anm->mp_client_interface->set_parameters(audio_pol_anm->mService, input, str_parms_to_str(param), 0);
    str_parms_destroy(param);

    /* Check if stream was started OK */
    char *keyValuePairs = audio_pol_anm->mp_client_interface->get_parameters(audio_pol_anm->mService, input, PARAM_KEY_STREAM_STATE);
    struct str_parms *streamStateParam = NULL;
    streamStateParam = str_parms_create_str(keyValuePairs);
    int val;
    int ret = str_parms_get_int(streamStateParam, PARAM_KEY_STREAM_STATE, &val);
    str_parms_destroy(streamStateParam);
    if (ret >= 0) {
        if (val != 1) {
            ALOG_ERR("LEAVE ste_anm_ap_start_input(): setParameter for ste_stream_state failed for input %d\n", input);
            UNLOCK_MUTEX(audio_pol_anm->mMutexDevState);
            return UNKNOWN_ERROR;
        }
    }

    ALOG_INFO("LEAVE ste_anm_ap_start_input(): input %d\n", input);
    UNLOCK_MUTEX(audio_pol_anm->mMutexDevState);

    return 0;
}

int ste_anm_ap_stop_input(struct audio_policy *pol, audio_io_handle_t input)
{
    ALOG_INFO("ENTER ste_anm_ap_stop_input(): input %d\n", input);
    audio_policy_anm *audio_pol_anm = to_apm_anm(pol);

    LOCK_MUTEX(audio_pol_anm->mMutexDevState);

    ssize_t index = ahi_list_get_key_index(audio_pol_anm->mInputs, input);
    int i = 0;
    size_t j = 0;

    if (index < 0) {
        ALOG_WARN("LEAVE ste_anm_ap_stop_input(): Unknown input %d\n", input);
        UNLOCK_MUTEX(audio_pol_anm->mMutexDevState);
        return EINVAL;
    }

    ste_audio_input_desc_t *descr = ahi_list_get_nth_node(audio_pol_anm->mInputs, index);

    if (descr->mRefCount > 0) {
        /* Decrement ref count of the input */
        descr->mRefCount--;
    } else {
        ALOG_WARN("LEAVE ste_anm_ap_stop_input(): input %d already stopped\n", input);
        UNLOCK_MUTEX(audio_pol_anm->mMutexDevState);
        return ENOSYS;
    }

    if (descr->mRefCount > 0) {
        ALOG_INFO("LEAVE ste_anm_ap_stop_input(): Ref count is %d, do nothing", descr->mRefCount);
        UNLOCK_MUTEX(audio_pol_anm->mMutexDevState);
        return 0;
    }

    /* Check if VoIP is active */
    if (descr->mVoIPActive) {
        ALOG_INFO("ste_anm_ap_stop_input(): VoIP is active for input %d\n", input);

        /* Check if VoIP output should be synced, i.e. if the call graph
         * should be closed regardless if output is active or not */
        bool syncedOutput = is_voip_synched(descr->mVoIPType);

        /* Check if active VoIP output exists */
        bool outputFound = false;
        ste_audio_output_descriptor_t *outDescr;
        for (i = 0; i < ahi_list_count(audio_pol_anm->mOutputs); i++) {
            audio_io_handle_t output = ahi_list_get_nth_key(audio_pol_anm->mOutputs, i);
            ste_audio_output_descriptor_t *tmpDescr = ahi_list_get_key_value(audio_pol_anm->mOutputs, output);
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
        ALOG_INFO("ste_anm_ap_stop_input(): synced=%d, activeOutput=%d\n", syncedOutput, outputFound);
        ALOG_INFO("ste_anm_ap_stop_input(): reroute VoIP input\n");
        descr->mVoIPActive = false;
        update_input_routing(audio_pol_anm);
        if (syncedOutput || !outputFound) {
            if (outputFound) {
                ALOG_INFO("ste_anm_ap_stop_input(): reroute VoIP output\n");
                outDescr->mVoIPActive = false;
                update_output_routing(audio_pol_anm
#ifdef USE_CACHE_MECHANISM
            , false
#endif
            );
            }
            ALOG_INFO("ste_anm_ap_stop_input(): close VoIP call graph\n");
            close_cs_call(audio_pol_anm);
            set_voip_status(pol, false, 0,0,0,0);
        }
    }

    if (!descr->mRefCount) {
        /* Loop through the in devices (the last 16 bits in audio_devices) */
        for (j=0x10000 ; j>0 && j<=0x80000000 ; j=j<<1) {
            if (descr->mDevice & j) {
                uint32_t indexTopLevel = get_top_level_device_index(audio_pol_anm, j);
                if (audio_pol_anm->mCurrentSource[indexTopLevel] == descr->mInputSource &&
                    !(descr->mInputSource==AUDIO_SOURCE_VOICE_CALL_NO_RECORD && audio_pol_anm->mPhoneState==AUDIO_MODE_IN_CALL)) {
                    /* Invalidate current source for device */
                    audio_pol_anm->mCurrentSource[indexTopLevel] = AUDIO_SOURCE_UNKNOWN;
                    update_input_routing(audio_pol_anm);
                }
            }
        }
    }

    /* Set stream state parameter */
    struct str_parms *param = NULL;
    param = str_parms_create_str(PARAM_KEY_STREAM_STATE);
    str_parms_add_int(param, PARAM_KEY_STREAM_STATE, 0);
    audio_pol_anm->mp_client_interface->set_parameters(audio_pol_anm->mService, input, str_parms_to_str(param), 0);
    str_parms_destroy(param);

    ALOG_INFO("LEAVE ste_anm_ap_stop_input(): input %d\n", input);

    UNLOCK_MUTEX(audio_pol_anm->mMutexDevState);

    return 0;
}

void ste_anm_ap_release_input(struct audio_policy *pol, audio_io_handle_t input)
{
    ALOG_INFO("ENTER ste_anm_ap_release_input(): input %d", input);

    audio_policy_anm *audio_pol_anm = to_apm_anm(pol);
    /* Check if the output is a valid input */
    ssize_t index = ahi_list_get_key_index(audio_pol_anm->mInputs, input);
    if (index < 0) {
        ALOG_WARN("LEAVE ste_anm_ap_release_input(): Unknown input %d!\n", input);
        return;
    } else {
        ste_audio_input_desc_t *descr = ahi_list_get_nth_node(audio_pol_anm->mInputs, index);

        audio_pol_anm->mp_client_interface->close_input(audio_pol_anm->mService, input);
        anm_free(ahi_list_get_nth_node(audio_pol_anm->mInputs, index));
        ahi_list_del_key(&(audio_pol_anm->mInputs), input);
        ALOG_INFO("ste_anm_ap_release_input(): Successfully released input %d\n", input);
        if(ahi_list_count(audio_pol_anm->mInputs) == 0) {
            ALOG_INFO("ste_anm_ap_release_input(): No more opened inputs");
        }
    }

    ALOG_INFO("LEAVE ste_anm_ap_release_input(): input %d", input);
}

/* ---------------------------------------------------------------------------
 * Volume handling
 * ---------------------------------------------------------------------------*/
void ste_anm_ap_init_stream_volume(struct audio_policy *pol,
        audio_stream_type_t stream,
        int indexMin,
        int indexMax)
{
    ALOG_INFO_VERBOSE("ste_anm_ap_init_stream_volume(): stream %s, min %d, max %d\n",
        stream2str(stream), indexMin, indexMax);
    audio_policy_anm *audio_pol_anm = to_apm_anm(pol);
    audio_pol_anm->mStreams[stream].mIndexMin = indexMin;
    audio_pol_anm->mStreams[stream].mIndexMax = indexMax;
}

int ste_anm_ap_set_stream_volume_index(
    struct audio_policy *pol,
    audio_stream_type_t stream, int index)
{
    ALOG_INFO_VERBOSE("ste_anm_ap_set_stream_volume_index(): ENTER");
    float volume = 0;
    uint32_t device = 0;
    int i = 0;
    audio_policy_anm *audio_pol_anm = to_apm_anm(pol);

    if ((index < audio_pol_anm->mStreams[stream].mIndexMin) ||
        (index > audio_pol_anm->mStreams[stream].mIndexMax)) {
        return EINVAL;
    }

    ALOG_INFO_VERBOSE("ste_anm_ap_set_stream_volume_index(): stream %s, index %d\n",
        stream2str(stream), index);
    audio_pol_anm->mStreams[stream].mIndexCur = index;

    /* Do not change actual stream volume if the stream is muted */
    if (audio_pol_anm->mStreams[stream].mMuteCount != 0) {
        return 0;
    }

    if (stream == AUDIO_STREAM_VOICE_CALL ||
        stream == AUDIO_STREAM_BLUETOOTH_SCO){
        if ((audio_pol_anm->mModemType == ADM_FAT_MODEM) || (audio_pol_anm->mModemType == ADM_FULL_FAT_MODEM)) {
            /* Apply voice call volume */
            ste_adm_client_init_cscall_downstream_volume(audio_pol_anm->mStreams[stream].mIndexMin, audio_pol_anm->mStreams[stream].mIndexMax);
            ALOG_INFO_VERBOSE("setStreamVolumeIndex(): setVoiceVolume() for stream %s, index volume %d\n",
                stream2str(stream), index);
            audio_pol_anm->mp_client_interface->set_voice_volume(audio_pol_anm->mService, index,0);
        } else {
            /* Compute and apply voice call volume */
            volume = compute_volume(audio_pol_anm, (int)stream, index, device);
            ALOG_INFO_VERBOSE("setStreamVolumeIndex(): setVoiceVolume() for stream %s, volume %f\n",
                stream2str(stream), volume);
            audio_pol_anm->mp_client_interface->set_voice_volume(audio_pol_anm->mService, volume,0);
        }
    } else {
        /* Compute and apply stream volume on all outputs */
        for (i = 0; i < ahi_list_count(audio_pol_anm->mOutputs); i++) {
            ste_audio_output_descriptor_t *outputDesc = (ste_audio_output_descriptor_t *) ahi_list_get_nth_node(audio_pol_anm->mOutputs, i);
            device = anm_out_desc_device(outputDesc);
            volume = compute_volume(audio_pol_anm, (int)stream, index, device);
            ALOG_INFO_VERBOSE("ste_anm_ap_set_stream_volume_index(): setStreamVolume() for output %d stream %s, volume %f\n",
                ahi_list_get_nth_key(audio_pol_anm->mOutputs, i), stream2str(stream), volume);
            audio_pol_anm->mp_client_interface->set_stream_volume(audio_pol_anm->mService, stream, volume, ahi_list_get_nth_key(audio_pol_anm->mOutputs, i),0);
        }
    }
    return 0;
}

int ste_anm_ap_get_stream_volume_index(
    const struct audio_policy *pol,
    audio_stream_type_t stream, int *index)
{
    audio_policy_anm *audio_pol_anm = ((struct ste_audio_policy_manager *)pol)->policy_anm;
    if (index == 0) {
        return EINVAL;
    }
    ALOG_INFO_VERBOSE("ste_anm_ap_get_stream_volume_index(): stream %s\n", stream2str(stream));
    *index =  audio_pol_anm->mStreams[stream].mIndexCur;
    return 0;
}

int ste_anm_ap_dump(const struct audio_policy *pol, int fd)
{
    const size_t SIZE = 256;
    char buffer[SIZE];
    char result[1024];
    int i = 0;
    audio_policy_anm *audio_pol_anm = ((struct ste_audio_policy_manager *)pol)->policy_anm;

    memset(result,0,1024);
    snprintf(buffer, SIZE, "\nAudioPolicyManager Dump: %p\n", audio_pol_anm);
    strncat(result, buffer, SIZE);
    for(i=0;i<NUM_DUP_OUTPUT;i++) {
        snprintf(buffer, SIZE, " Duplicated Output[%d] : %d\n", i,audio_pol_anm->duplicated_output[i]);
        strncat(result, buffer, SIZE);
    }
    snprintf(buffer, SIZE, " Output devices: %08x\n", audio_pol_anm->mAvailableOutputDevices);
    strncat(result, buffer, SIZE);
    snprintf(buffer, SIZE, " Input devices: %08x\n", audio_pol_anm->mAvailableInputDevices);
    strncat(result, buffer, SIZE);
    snprintf(buffer, SIZE, " Phone state: %d\n", audio_pol_anm->mPhoneState);
    strncat(result, buffer, SIZE);
    snprintf(buffer, SIZE, " Ringer mode: %d\n", audio_pol_anm->mRingerMode);
    strncat(result, buffer, SIZE);
    snprintf(buffer, SIZE, " Force use for communication %d\n",
        audio_pol_anm->mForceUse[AUDIO_POLICY_FORCE_FOR_COMMUNICATION]);
    strncat(result, buffer, SIZE);
    snprintf(buffer, SIZE, " Force use for media %d\n",
        audio_pol_anm->mForceUse[AUDIO_POLICY_FORCE_FOR_MEDIA]);
    strncat(result, buffer, SIZE);
    snprintf(buffer, SIZE, " Force use for record %d\n",
        audio_pol_anm->mForceUse[AUDIO_POLICY_FORCE_FOR_RECORD]);
    strncat(result, buffer, SIZE);
    write(fd, result, strlen(result));

    snprintf(buffer, SIZE, "\nOutputs dump:\n");
    write(fd, buffer, strlen(buffer));
    for (i = 0; i < ahi_list_count(audio_pol_anm->mOutputs); i++) {
        snprintf(buffer, SIZE, "- Output %d dump:\n", ahi_list_get_nth_key(audio_pol_anm->mOutputs, i));
        write(fd, buffer, strlen(buffer));
        ste_audio_output_descriptor_t *descr = (ste_audio_output_descriptor_t*)ahi_list_get_nth_node(audio_pol_anm->mOutputs, i);
        anm_out_desc_dump(descr, fd);
    }

    snprintf(buffer, SIZE, "\nInputs dump:\n");
    write(fd, buffer, strlen(buffer));
    for (i = 0; i < ahi_list_count(audio_pol_anm->mInputs); i++) {
        snprintf(buffer, SIZE, "- Input %d dump:\n", ahi_list_get_nth_key(audio_pol_anm->mInputs, i));
        write(fd, buffer, strlen(buffer));
        ste_audio_input_desc_t *descr = (ste_audio_input_desc_t *)ahi_list_get_nth_node(audio_pol_anm->mInputs, i);
        anm_in_desc_dump(descr,fd);
    }

    snprintf(buffer, SIZE, "\nStreams dump:\n");
    write(fd, buffer, strlen(buffer));
    snprintf(buffer, SIZE, " Stream  Index Min  Index Max  Index Cur  Mute "
        "Count  Can be muted\n");
    write(fd, buffer, strlen(buffer));
    for (i = 0; i < AUDIO_STREAM_CNT; i++) {
        snprintf(buffer, SIZE, " %02d", i);
        audio_pol_anm->mStreams[i].dump(&audio_pol_anm->mStreams[i],buffer + 3, SIZE);
        write(fd, buffer, strlen(buffer));
    }

    return 0;
}


uint32_t ste_anm_ap_get_strategy_for_stream(const struct audio_policy *pol, audio_stream_type_t stream)
{
    return (uint32_t)getStrategy(stream);
}

uint32_t ste_anm_ap_get_devices_for_stream(const struct audio_policy *pol, audio_stream_type_t stream)
{
    uint32_t devices;
    audio_policy_anm *audio_pol_anm = ((struct ste_audio_policy_manager *)pol)->policy_anm;

    // By checking the range of stream before calling getStrategy, we avoid
    // getStrategy's behavior for invalid streams.  getStrategy would do a LOGE
    // and then return STRATEGY_MEDIA, but we want to return the empty set.
    if (stream < (audio_stream_type_t) 0 || stream >= AUDIO_STREAM_CNT) {
        devices = 0;
    } else {
        uint32_t strategy = ste_anm_ap_get_strategy_for_stream(pol, stream);
        devices = get_output_device_for_strategy(audio_pol_anm, (audio_io_handle_t)NULL,(routing_strategy) strategy,AUDIO_FORMAT_PCM_16_BIT
#ifdef USE_CACHE_MECHANISM
        , false
#endif
        );
    }
    return devices;
}

// Audio effect management
audio_io_handle_t ste_anm_ap_get_output_for_effect(struct audio_policy *pol, effect_descriptor_t *desc)
{
    ALOG_INFO_VERBOSE("ste_anm_ap_get_output_for_effect(): %s\n", desc->name);
    audio_policy_anm *audio_pol_anm = to_apm_anm(pol);
    // apply simple rule where global effects are attached to the same output as MUSIC streams
    return ste_anm_ap_get_output(pol, AUDIO_STREAM_MUSIC, 0, AUDIO_FORMAT_DEFAULT, 0, AUDIO_OUTPUT_FLAG_NONE);
}

int ste_anm_ap_register_effect(struct audio_policy *pol,
                                    effect_descriptor_t *desc,
                                    audio_io_handle_t io,
                                    uint32_t strategy,
                                    int session,
                                    int id)
{
    ALOG_INFO("ste_anm_ap_register_effect(): id %d, io %d, strategy %d, session %d, name %s, CPU %d, memory %d",
            id, io, strategy, session, desc->name, desc->cpuLoad, desc->memoryUsage);

    audio_policy_anm *audio_pol_anm = to_apm_anm(pol);
    ssize_t index = ahi_list_get_key_index(audio_pol_anm->mOutputs, io);
    if (index < 0) {
        index =  ahi_list_get_key_index(audio_pol_anm->mInputs, io);
        if (index < 0) {
            ALOG_WARN("ste_anm_ap_register_effect(): unknown io %d", io);
            return ENOSYS;
        }
    }
    if (desc->cpuLoad > get_max_effects_cpu_load()) {
        ALOG_WARN("ste_anm_ap_register_effect(): CPU Load limit (%d) exceeded for Fx %s, CPU %f MIPS",
                get_max_effects_cpu_load(), desc->name, (float)desc->cpuLoad/10);
        return ENOSYS;
    }
    if (desc->memoryUsage > get_max_effects_memory()) {
        ALOG_WARN("ste_anm_ap_register_effect(): memory limit (%d) exceeded for Fx %s, Memory %d KB",
                get_max_effects_memory(), desc->name, desc->memoryUsage);
        return ENOSYS;
    }

    ALOG_INFO_VERBOSE("ste_anm_ap_register_effect(): Registration OK CPU %d, memory %d", desc->cpuLoad, desc->memoryUsage);
    ALOG_INFO_VERBOSE("  total CPU %d, total memory %d", audio_pol_anm->mTotalEffectsCpuLoad, audio_pol_anm->mTotalEffectsMemory);

    ste_effect_descriptor_t *pDesc = (ste_effect_descriptor_t *)anm_calloc(1, sizeof(ste_effect_descriptor_t));
    init_effect_descriptor(pDesc);
    memcpy (&pDesc->mDesc, desc, sizeof(effect_descriptor_t));
    pDesc->mIo = io;
    pDesc->mStrategy = (routing_strategy)strategy;
    pDesc->mSession = session;
    ahi_list_add(&(audio_pol_anm->mEffects), pDesc, id);

    return 0;
}

int ste_anm_ap_unregister_effect(struct audio_policy *pol, int id)
{
    ALOG_INFO_VERBOSE("ste_anm_ap_unregister_effect(): effect ID %d", id);
    audio_policy_anm *audio_pol_anm = to_apm_anm(pol);
    ssize_t index = ahi_list_get_key_index(audio_pol_anm->mEffects, id);
    if (index < 0) {
        ALOG_WARN("ste_anm_ap_unregister_effect(): unknown effect ID %d", id);
        return ENOSYS;
    }

    ste_effect_descriptor_t *pDesc = ahi_list_get_nth_node(audio_pol_anm->mEffects, index);

    ste_anm_ap_set_effect_enabled(pol, id, false);

    ALOG_INFO_VERBOSE("ste_anm_ap_unregister_effect(): effect %s, ID %d, CPU %d, memory %d",
            pDesc->mDesc.name, id, pDesc->mDesc.cpuLoad, pDesc->mDesc.memoryUsage);
    ALOG_INFO_VERBOSE("  total CPU %d, total memory %d", audio_pol_anm->mTotalEffectsCpuLoad, audio_pol_anm->mTotalEffectsMemory);

    ahi_list_del_key(&(audio_pol_anm->mEffects), id);
    anm_free(pDesc);

    return 0;
}


int ste_anm_ap_set_effect_enabled(struct audio_policy *pol, int id, bool enabled)
{
    audio_policy_anm *audio_pol_anm = to_apm_anm(pol);
    ssize_t index = ahi_list_get_key_index(audio_pol_anm->mEffects, id);
    if (index < 0) {
        ALOG_WARN("ste_anm_ap_set_effect_enabled(): unknown effect ID %d", id);
        return ENOSYS;
    }

    return set_effect_enabled(audio_pol_anm, ahi_list_get_nth_node(audio_pol_anm->mEffects, index), enabled);
}

int set_effect_enabled(audio_policy_anm *audio_pol_anm, ste_effect_descriptor_t *pDesc, bool enabled)
{
    if (enabled == pDesc->mEnabled) {
        ALOG_INFO_VERBOSE("set_effect_enabled(%s) effect already %s",
             enabled?"true":"false", enabled?"enabled":"disabled");
        return ENOSYS;
    }

    if (enabled) {
        if (audio_pol_anm->mTotalEffectsCpuLoad + pDesc->mDesc.cpuLoad > get_max_effects_cpu_load()) {
            ALOG_WARN("set_effect_enabled(true) CPU Load limit exceeded for Fx %s, CPU %f MIPS",
                 pDesc->mDesc.name, (float)pDesc->mDesc.cpuLoad/10);
            return ENOSYS;
        }
        audio_pol_anm->mTotalEffectsCpuLoad += pDesc->mDesc.cpuLoad;
        ALOG_INFO_VERBOSE("set_effect_enabled(true) total CPU %d", audio_pol_anm->mTotalEffectsCpuLoad);

        if (audio_pol_anm->mTotalEffectsMemory + pDesc->mDesc.memoryUsage > get_max_effects_memory()) {
            ALOG_WARN("set_effect_enabled(true) memory limit (%d) exceeded for Fx %s, Memory %d KB",
                get_max_effects_memory(), pDesc->mDesc.name, pDesc->mDesc.memoryUsage);
            return ENOSYS;
        }
        audio_pol_anm->mTotalEffectsMemory += pDesc->mDesc.memoryUsage;
        ALOG_INFO_VERBOSE("set_effect_enabled(true) total Memory %d", audio_pol_anm->mTotalEffectsMemory);
    } else {
        if (audio_pol_anm->mTotalEffectsCpuLoad < pDesc->mDesc.cpuLoad) {
            ALOG_WARN("set_effect_enabled(false) CPU load %d too high for total %d",
                    pDesc->mDesc.cpuLoad, audio_pol_anm->mTotalEffectsCpuLoad);
            pDesc->mDesc.cpuLoad = audio_pol_anm->mTotalEffectsCpuLoad;
        }
        audio_pol_anm->mTotalEffectsCpuLoad -= pDesc->mDesc.cpuLoad;
        ALOG_INFO_VERBOSE("set_effect_enabled(false) total CPU %d", audio_pol_anm->mTotalEffectsCpuLoad);

        if (audio_pol_anm->mTotalEffectsMemory < pDesc->mDesc.memoryUsage) {
            ALOG_WARN("set_effect_enabled(false) memory %d too big for total %d",
                pDesc->mDesc.memoryUsage, audio_pol_anm->mTotalEffectsMemory);
            pDesc->mDesc.memoryUsage = audio_pol_anm->mTotalEffectsMemory;
        }
        audio_pol_anm->mTotalEffectsMemory -= pDesc->mDesc.memoryUsage;
        ALOG_INFO_VERBOSE("set_effect_enabled(false) total Memory %d", audio_pol_anm->mTotalEffectsMemory);
    }
    pDesc->mEnabled = enabled;
    return 0;
}

bool ste_anm_ap_is_stream_active(const struct audio_policy *pol, int stream, uint32_t inPastMs)
{
    audio_policy_anm *audio_pol_anm = to_apm_anm((struct audio_policy *)pol);
    nsecs_t sysTime = systemTime(SYSTEM_TIME_MONOTONIC);
    int i = 0;
    for (i = 0; i < ahi_list_count(audio_pol_anm->mOutputs); i++) {
        ste_audio_output_descriptor_t *descr = ahi_list_get_nth_node(audio_pol_anm->mOutputs, i);
        if(    get_ref_count(descr,stream) != 0  ||
                ns2ms(sysTime - descr->mStopTime[stream]) < inPastMs) {
            return true;
        }
    }
    return false;
}

void ste_ap_init(audio_policy_anm *audio_pol_anm, struct audio_policy *policy,
                 struct audio_policy_service_ops *clientInterface, void *service)
{
    int i = 0;

    policy->set_device_connection_state = ste_anm_ap_set_device_connection_state;
    policy->get_device_connection_state = ste_anm_ap_get_device_connection_state;
    policy->set_phone_state = ste_anm_ap_set_phone_state;
    policy->set_ringer_mode = ste_anm_ap_set_ringer_mode;
    policy->set_force_use = ste_anm_ap_set_force_use;
    policy->get_force_use = ste_anm_ap_get_force_use;
    policy->set_can_mute_enforced_audible = ste_anm_ap_set_can_mute_enforced_audible;
    policy->init_check = ste_anm_ap_init_check;
    policy->get_output = ste_anm_ap_get_output;
    policy->start_output = ste_anm_ap_start_output;
    policy->stop_output = ste_anm_ap_stop_output;
    policy->release_output = ste_anm_ap_release_output;
    policy->get_input = ste_anm_ap_get_input;
    policy->start_input = ste_anm_ap_start_input;
    policy->stop_input = ste_anm_ap_stop_input;
    policy->release_input = ste_anm_ap_release_input;
    policy->init_stream_volume = ste_anm_ap_init_stream_volume;
    policy->set_stream_volume_index = ste_anm_ap_set_stream_volume_index;
    policy->get_stream_volume_index = ste_anm_ap_get_stream_volume_index;
    policy->get_strategy_for_stream = ste_anm_ap_get_strategy_for_stream;
    policy->get_devices_for_stream = ste_anm_ap_get_devices_for_stream;
    policy->get_output_for_effect = ste_anm_ap_get_output_for_effect;
    policy->register_effect = ste_anm_ap_register_effect;
    policy->unregister_effect = ste_anm_ap_unregister_effect;
    policy->set_effect_enabled = ste_anm_ap_set_effect_enabled;
    policy->is_stream_active = ste_anm_ap_is_stream_active;
    policy->dump = ste_anm_ap_dump;

    if(audio_pol_anm == NULL) {
        return;
    }

    audio_pol_anm->mRingerMode = 0;
    audio_pol_anm->mCSCallActive = false;
    audio_pol_anm->mVoIPActive = false;
    audio_pol_anm->mForceUseOngoing = false;
    audio_pol_anm->mTotalEffectsCpuLoad = 0;
    audio_pol_anm->mTotalEffectsMemory = 0;
    audio_pol_anm->mModemType = ADM_UNKNOWN_MODEM_TYPE;
    audio_pol_anm->mp_client_interface = clientInterface;
    audio_pol_anm->mService = service;


    memset(&store_log,0,sizeof(struct ap_store_log));

    (void) ste_anm_debug_setup_log();
    audio_pol_anm->ext_hal = (void*) identify_ext_hal(audio_pol_anm);

    ALOG_INFO("ste_ap_init(): audio_policy_anm=%p", audio_pol_anm);
    audio_pol_anm->fd_adm = -1;

    int retval = pthread_mutex_init(&audio_pol_anm->mMutexForceUse, 0);
    if (retval != 0) {
        ALOG_ERR("ste_ap_init(): pthread_mutex_init failed, error = %d", retval);
    }

    retval = pthread_mutex_init(&audio_pol_anm->mMutexCallDevices, 0);
    if (retval != 0) {
        ALOG_ERR("ste_ap_init(): pthread_mutex_init failed, error = %d", retval);
    }

    retval = pthread_mutex_init(&audio_pol_anm->mMutexCallStatus, 0);
    if (retval != 0) {
        ALOG_ERR("ste_ap_init(): pthread_mutex_init failed, error = %d", retval);
    }

    retval = pthread_mutex_init(&audio_pol_anm->mMutexVC, 0);
    if (retval != 0) {
        ALOG_ERR("ste_ap_init(): pthread_mutex_init failed, error = %d", retval);
    }

    retval = pthread_mutex_init(&audio_pol_anm->mMutexDevState, 0);
    if (retval != 0) {
        ALOG_ERR("pthread_mutex_init failed, error = %d\n", retval);
    }

    retval = pthread_mutex_init(&audio_pol_anm->mMutexIO, 0);
    if (retval != 0) {
        ALOG_ERR("ste_ap_init(): pthread_mutex_init failed, error = %d", retval);
    }

    for (i = 0; i < AUDIO_POLICY_FORCE_USE_CNT; i++) {
        audio_pol_anm->mForceUse[i] = AUDIO_POLICY_FORCE_NONE;
    }

    for (i=0; i<NUM_DEV_OUT_INDEXES; i++) {
        audio_pol_anm->mCurrentStrategy[i]=STRATEGY_UNKNOWN;
    }
    for (i=0; i<NUM_DEV_IN_INDEXES; i++) {
        audio_pol_anm->mCurrentSource[i]=AUDIO_SOURCE_UNKNOWN;
    }

    for(i = 0; i < AUDIO_STREAM_CNT; i++) {
        init_stream_descriptor(&audio_pol_anm->mStreams[i]);
    }

    /* Set default input and output devices */
    audio_pol_anm->mAvailableOutputDevices = defaultOutputDevices;
    audio_pol_anm->mAvailableInputDevices  = defaultInputDevices;
    audio_pol_anm->check_call_status = check_call_status;

    // Initialize each interface with external HALs
    ext_hal_init(audio_pol_anm->ext_hal);

#ifdef USE_CACHE_MECHANISM
    update_device_for_strategy(audio_pol_anm);
#endif

    // In case of A2DP, in standard mechanism, outputs have to be created
    if(ext_hal_is_available(audio_pol_anm->ext_hal, AUDIO_DEVICE_OUT_BLUETOOTH_A2DP)){
        ste_anm_ap_get_output(policy, AUDIO_STREAM_VOICE_CALL, 0, 0, 0, (audio_output_flags_t)0);
        ste_anm_ap_get_output(policy, AUDIO_STREAM_DTMF, 0, 0, 0, (audio_output_flags_t)0);
        ste_anm_ap_get_output(policy, AUDIO_STREAM_MUSIC, 0, 0, 0, (audio_output_flags_t)0);
    }
    if (ext_hal_duplication_is_supported(audio_pol_anm->ext_hal)) {
        ste_anm_ap_get_output(policy, AUDIO_STREAM_RING, 0, 0, 0, (audio_output_flags_t)0);
    }
    dump_list_output(audio_pol_anm);
    ALOG_INFO_VERBOSE("ste_ap_init(): "
        "Default available devices: output = %x, input = %x)",
        audio_pol_anm->mAvailableOutputDevices, audio_pol_anm->mAvailableInputDevices);

    // Set Audio Policy Manager in AHI
    struct str_parms *param = str_parms_create_str(PARAM_KEY_AP_REF);
    int jj=0, ret;
    char *str;
    audio_pol_anm->apm_ptr = (void*)policy; /* Will be used by AHI to call check_call_status function */
    ret = str_parms_add_int(param,PARAM_KEY_AP_REF,(int)audio_pol_anm);
    str = str_parms_to_str(param);
    audio_pol_anm->mp_client_interface->set_parameters(audio_pol_anm->mService, 0, str, 0);
    str_parms_destroy(param);
    /* Subscribe to ADM Modem Status */
    subscribe_modem_status(policy);

    if (ste_adm_client_get_modem_type(&audio_pol_anm->mModemType) != STE_ADM_RES_OK) {
        ALOG_ERR("audio_policy_anm(): Impossible to get modem type!\n");
    }
}

void deinit_audio_policy_manager(audio_policy_anm *audio_pol_anm)
{
    int i = 0;
    ALOG_INFO("deinit_audio_policy_manager(): audio_policy_anm=%p", audio_pol_anm);

    for (i = 0; i < ahi_list_count(audio_pol_anm->mOutputs); i++) {
        audio_pol_anm->mp_client_interface->close_output(audio_pol_anm->mService, ahi_list_get_nth_key(audio_pol_anm->mOutputs, i));
        anm_free(ahi_list_get_nth_node(audio_pol_anm->mOutputs, i));
    }
    ahi_list_clear(&audio_pol_anm->mOutputs);
    for (i = 0; i < ahi_list_count(audio_pol_anm->mInputs); i++) {
        ste_audio_input_desc_t *descr = ahi_list_get_nth_node(audio_pol_anm->mInputs, i);
        audio_pol_anm->mp_client_interface->close_input(audio_pol_anm->mService, ahi_list_get_nth_key(audio_pol_anm->mInputs, i));
        anm_free(descr);
    }
    ahi_list_clear(&audio_pol_anm->mInputs);
// CleanUp each interface with external HALs
    ext_hal_reset(audio_pol_anm->ext_hal);
    pthread_mutex_destroy(&audio_pol_anm->mMutexForceUse);
    pthread_mutex_destroy(&audio_pol_anm->mMutexCallDevices);
    pthread_mutex_destroy(&audio_pol_anm->mMutexCallStatus);
    pthread_mutex_destroy(&audio_pol_anm->mMutexVC);
    pthread_mutex_destroy(&audio_pol_anm->mMutexIO);
    pthread_mutex_destroy(&audio_pol_anm->mMutexDevState);
}

void init_audio_output_descriptor(ste_audio_output_descriptor_t *output_descr)
{
    int i = 0;
    output_descr->mSamplingRate = 0;
    output_descr->mFormat = 0;
    output_descr->mChannels = 0;
    output_descr->mLatency = 0;
    output_descr->mFlags = (audio_output_flags_t)0;
    output_descr->mDevice = 0;
    set_strategy_ref_count(output_descr, 0);
    output_descr->mAudioOutputRef =0;
    output_descr->mVoIPActive = false;
    output_descr->mSuspended = false;
#ifdef ALLOW_DUPLICATION
    memset(&(output_descr->duplication_info),0,sizeof(duplicated_info_t));
#endif
    /* Clear usage count for all stream types */
    for (i = 0; i < AUDIO_STREAM_CNT; i++) {
        set_ref_count(output_descr,i,0);
        output_descr->mStoreRefCount[i]=0; //TRACE PURPOSE
        output_descr->mStopTime[i] = 0;
    }
}

void anm_out_desc_set_audio_output_ref(ste_audio_output_descriptor_t *outputDescr, int ref)
{
    outputDescr->mAudioOutputRef = ref;
}

uint32_t anm_out_desc_device(ste_audio_output_descriptor_t *output_descr)
{
 #ifdef ALLOW_DUPLICATION
    if (anm_out_desc_is_duplicated(output_descr)) {
        return (output_descr->duplication_info.primary_desc->mDevice | output_descr->duplication_info.external_desc->mDevice);
    }
#endif
    return output_descr->mDevice;
}

void display_RefCount(ste_audio_output_descriptor_t *output_descr)
{
    if(memcmp(output_descr->mStoreRefCount,output_descr->mRefCount,sizeof(uint32_t)*AUDIO_STREAM_CNT)==0)
        return;

    ALOG_INFO_VERBOSE("-----BEGIN RefCount table for output %d ------",output_descr->output_id);
    int i = 0;
    for (i = 0; i < (int)AUDIO_STREAM_CNT; i++) {
        ALOG_INFO_VERBOSE("Stream %s : refCount : %d",stream2str(i),output_descr->mRefCount[i]);
    }
    ALOG_INFO_VERBOSE("-----END RefCount table -------",(unsigned int )output_descr);
    memcpy(output_descr->mStoreRefCount,output_descr->mRefCount,sizeof(uint32_t)*AUDIO_STREAM_CNT);
}

void anm_out_desc_change_ref_count(
    ste_audio_output_descriptor_t *output_descr,
    audio_stream_type_t stream, int delta)
{
#ifdef ALLOW_DUPLICATION
    // forward usage count change to attached outputs
    if (anm_out_desc_is_duplicated(output_descr)) {
        ALOG_INFO("anm_out_desc_change_ref_count(%d) for primary output: %d",delta,output_descr->duplication_info.primary_output);
        anm_out_desc_change_ref_count(output_descr->duplication_info.primary_desc, stream, delta);
        ALOG_INFO("anm_out_desc_change_ref_count(%d) for external output:%d",delta,output_descr->duplication_info.external_output);
        anm_out_desc_change_ref_count(output_descr->duplication_info.external_desc, stream, delta);
        ALOG_INFO("anm_out_desc_change_ref_count() for duplicated output:");
    }
#endif
    if ((delta + (int)output_descr->mRefCount[stream]) < 0) {
        ALOG_WARN("anm_out_desc_change_ref_count(): invalid delta %d for stream %s, refCount %d",
            delta, stream2str(stream), output_descr->mRefCount[stream]);
        set_ref_count(output_descr,stream,0);
        return;
    }
    update_ref_count(output_descr,stream,delta);
    ALOG_INFO_VERBOSE("anm_out_desc_change_ref_count(): stream %s, count %d",
        stream2str(stream), output_descr->mRefCount[stream]);
    display_RefCount(output_descr);
}

uint32_t anm_out_desc_ref_count(ste_audio_output_descriptor_t *output_descr)
{
    uint32_t refcount = 0;
    int i = 0;

    for (i = 0; i < (int)AUDIO_STREAM_CNT; i++) {
        refcount += output_descr->mRefCount[i];
    }
    display_RefCount(output_descr);
    return refcount;
}

uint32_t anm_out_desc_strategy_ref_count(
    ste_audio_output_descriptor_t *output_descr,
    routing_strategy strategy)
{
    uint32_t refCount = 0;

    int i = 0;
    for (i = 0; i < (int)AUDIO_STREAM_CNT; i++) {
        if (getStrategy((audio_stream_type_t)i) == strategy) {
            refCount += output_descr->mRefCount[i];
        }
    }
    display_RefCount(output_descr);
    ALOG_INFO("anm_out_desc_strategy_ref_count  for output %d and strategy %s   return refCount = %d ",output_descr->output_id,strategy2str(strategy),refCount);
    return refCount;
}

int anm_out_desc_dump(ste_audio_output_descriptor_t *output_descr,int fd)
{
    const size_t SIZE = 256;
    char buffer[SIZE];
    char result[1024];
    int i = 0;

    memset(result, 0, 1024);
    snprintf(buffer, SIZE, " Sampling rate: %d\n", output_descr->mSamplingRate);
    strncat(result, buffer, SIZE);
    snprintf(buffer, SIZE, " Format: %d\n", output_descr->mFormat);
    strncat(result, buffer, SIZE);
    snprintf(buffer, SIZE, " Channels: %08x\n", output_descr->mChannels);
    strncat(result, buffer, SIZE);
    snprintf(buffer, SIZE, " Latency: %d\n", output_descr->mLatency);
    strncat(result, buffer, SIZE);
    snprintf(buffer, SIZE, " Flags %08x\n", output_descr->mFlags);
    strncat(result, buffer, SIZE);
    snprintf(buffer, SIZE, " Devices %08x\n", output_descr->mDevice);
    strncat(result, buffer, SIZE);
    snprintf(buffer, SIZE, " Stream refCount\n");
    strncat(result, buffer, SIZE);
    for (i = 0; i < AUDIO_STREAM_CNT; i++) {
        snprintf(buffer, SIZE, " %02d     %d\n", i, output_descr->mRefCount[i]);
        strncat(result, buffer, SIZE);
    }
    write(fd, result, strlen(result));

    return 0;
}

bool anm_out_desc_is_used_by_stream(ste_audio_output_descriptor_t *outputDescr, audio_stream_type_t stream)
{
    return outputDescr->mRefCount[stream] > 0 ? true : false;
}

bool anm_out_desc_is_used_by_strategy(ste_audio_output_descriptor_t *outputDescr,routing_strategy strategy)
{
    return (anm_out_desc_strategy_ref_count(outputDescr,strategy) != 0);
}

int anm_out_desc_get_audio_output_ref(ste_audio_output_descriptor_t *outputDescr)
{
    return outputDescr->mAudioOutputRef;
}

#ifdef ALLOW_DUPLICATION
bool anm_out_desc_is_duplicated(ste_audio_output_descriptor_t *outputDescr)
{
    return (outputDescr->duplication_info.is_duplicated_output);
}
#endif

void init_audio_input_descriptor(ste_audio_input_desc_t *inputDesc)
{
    inputDesc->mSamplingRate = 0;
    inputDesc->mFormat = 0;
    inputDesc->mChannels = 0;
    inputDesc->mAcoustics = (audio_in_acoustics_t)0;
    inputDesc->mDevice = 0;
    inputDesc->mRefCount = 0;
    inputDesc->mVoIPActive = false;
}

int anm_in_desc_dump(ste_audio_input_desc_t *inputDesc, int fd)
{
    const size_t SIZE = 256;
    char buffer[SIZE];
    char result[1536];

    memset(result, 0, 1536);
    snprintf(buffer, SIZE, " Sampling rate: %d\n", inputDesc->mSamplingRate);
    strncat(result, buffer, SIZE);
    snprintf(buffer, SIZE, " Format: %d\n", inputDesc->mFormat);
    strncat(result, buffer, SIZE);
    snprintf(buffer, SIZE, " Channels: %08x\n", inputDesc->mChannels);
    strncat(result, buffer, SIZE);
    snprintf(buffer, SIZE, " Acoustics %08x\n", inputDesc->mAcoustics);
    strncat(result, buffer, SIZE);
    snprintf(buffer, SIZE, " Devices %08x\n", inputDesc->mDevice);
    strncat(result, buffer, SIZE);
    snprintf(buffer, SIZE, " Ref Count %d\n", inputDesc->mRefCount);
    strncat(result, buffer, SIZE);
    write(fd, result, strlen(result));

    return 0;
}

void StreamDescriptor_dump(stream_descriptor_t *streamDescr, char* buffer, size_t size)
{
    snprintf(buffer, size,
        "      %02d         %02d         %02d         %02d          %d\n",
        streamDescr->mIndexMin,
        streamDescr->mIndexMax,
        streamDescr->mIndexCur,
        streamDescr->mMuteCount,
        streamDescr->mCanBeMuted);
}

void init_stream_descriptor(stream_descriptor_t *streamDescr)
{
    streamDescr->mIndexMin = 0;
    streamDescr->mIndexMax = 1;
    streamDescr->mIndexCur = 1;
    streamDescr->mMuteCount = 0;
    streamDescr->mCanBeMuted = true;
    streamDescr->dump = StreamDescriptor_dump;
}

void init_effect_descriptor(ste_effect_descriptor_t *descr)
{
    descr->dump = dump_effect_descriptor;
}

int dump_effect_descriptor(ste_effect_descriptor_t *descr,int fd)
{
    const size_t SIZE = 256;
    char buffer[SIZE];
    char result[1024];

    memset(result, 0, 1024);
    snprintf(buffer, SIZE, " IO: %d\n", descr->mIo);
    strncat(result, buffer, SIZE);
    snprintf(buffer, SIZE, " Strategy: %d\n", descr->mStrategy);
    strncat(result, buffer, SIZE);
    snprintf(buffer, SIZE, " Session: %d\n", descr->mSession);
    strncat(result, buffer, SIZE);
    snprintf(buffer, SIZE, " Name: %s\n",  descr->mDesc.name);
    strncat(result, buffer, SIZE);
    write(fd, result, strlen(result));

    return 0;
}



int ste_ap_create(const struct audio_policy_device *device,
                             struct audio_policy_service_ops *aps_ops,
                             void *service,
                             struct audio_policy **ap)
{
    struct ste_audio_policy_manager *apm = NULL;
    int ret;

    *ap = NULL;
    ALOG_INFO("ste_ap_create()");

    if (!service || !aps_ops)
        return -EINVAL;

    apm = (struct ste_audio_policy_manager *)anm_calloc(1, sizeof(struct ste_audio_policy_manager));
    if (!apm)
        return -ENOMEM;

    apm->policy_anm = (audio_policy_anm *)anm_calloc(1, sizeof(audio_policy_anm));
    ALOG_INFO_VERBOSE("ste_anm_ap: %x ", apm->policy_anm);

    ste_ap_init(apm->policy_anm, &apm->policy, aps_ops, service);

    *ap = &apm->policy;

    return 0;
}

static int ste_ap_destroy(const struct audio_policy_device *ap_dev,
                          struct audio_policy *ap)
{
    ALOG_INFO("ste_ap_destroy()");
    audio_policy_anm *audio_pol_anm = to_apm_anm(ap);
    deinit_audio_policy_manager(audio_pol_anm);
    anm_free(audio_pol_anm);
    anm_free((struct ste_audio_policy_manager *)ap);
    return 0;
}

static int ste_ap_dev_close(hw_device_t* device)
{
    ALOG_INFO("ste_ap_dev_close()");
    anm_free(device);
    return 0;
}

static int ste_ap_dev_open(const hw_module_t* module, const char* name,
                           hw_device_t** device)
{
    struct ste_audio_ap_device *dev = NULL;

    *device = NULL;

    ALOG_INFO("ste_ap_dev_open()");

    if (strcmp(name, AUDIO_POLICY_INTERFACE) != 0)
        return -EINVAL;

    dev = (struct ste_audio_ap_device *)anm_calloc(1, sizeof(*dev));
    if (!dev)
        return -ENOMEM;

    dev->device.common.tag = HARDWARE_DEVICE_TAG;
    dev->device.common.version = 0;
    dev->device.common.module = (hw_module_t *)module;
    dev->device.common.close = ste_ap_dev_close;
    dev->device.create_audio_policy = ste_ap_create;
    dev->device.destroy_audio_policy = ste_ap_destroy;

    *device = &dev->device.common;

    return 0;
}

static struct hw_module_methods_t ste_ap_module_methods = {
    .open = ste_ap_dev_open,
};

struct ste_audio_ap_module HAL_MODULE_INFO_SYM = {
    .module = {
        .common = {
            .tag            = HARDWARE_MODULE_TAG,
            .version_major  = 1,
            .version_minor  = 0,
            .id             = AUDIO_POLICY_HARDWARE_MODULE_ID,
            .name           = "ST-Ericsson Audio Policy HAL",
            .author         = "ST-Ericsson",
            .methods        = &ste_ap_module_methods,
        },
    },
};
