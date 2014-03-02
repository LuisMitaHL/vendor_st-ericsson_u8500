/*******************************************************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \brief   Header of Audio Policy Manager of ANM to be used by AudioFlinger in Android framework.
 */
/********************************************************************************************************************/

/*! \file ste_hal_anm_ap.h
*   \brief Header for STE Audio Policy

    This header defines the audio_policy that is used
    in the STE Android port to control audio policy.
*/


#ifndef STE_HAL_AUDIO_POLICY_MANAGER_ANM_H
#define STE_HAL_AUDIO_POLICY_MANAGER_ANM_H

#include <system/audio.h>
#include <system/audio_policy.h>
#include <hardware/audio_policy.h>
#include <hardware/audio.h>
#include <utils/Timers.h>
#include <linux_utils.h>

#include <ste_adm_client.h>
#include "ste_hal_anm_ahi.h"
#include "ste_hal_anm_util.h"
#include "ste_hal_anm_dbg.h"

#define MAX_OUTPUT_DISCRIPTOR 20
#define MAX_INPUT_DISCRIPTOR 20

#define PARAM_KEY_STREAM_STATE      "ste_stream_state" //AHI

/* Output device indexes */
typedef enum _dev_out_index {
    DEV_OUT_INDEX_EARPIECE,
    DEV_OUT_INDEX_SPEAKER,
    DEV_OUT_INDEX_HEADSET,
    DEV_OUT_INDEX_HEADPHONE,
    DEV_OUT_INDEX_A2DP,
    DEV_OUT_INDEX_FMTX,
    DEV_OUT_INDEX_HDMI,
    DEV_OUT_INDEX_BT,
    DEV_OUT_INDEX_BTWB,
    DEV_OUT_INDEX_VOIP,
    DEV_OUT_INDEX_USB, /*USB analog headset*/
    DEV_OUT_INDEX_DEFAULT,
    NUM_DEV_OUT_INDEXES
} dev_out_index;

/* Input device indexes */
typedef enum _dev_in_index {
    DEV_IN_INDEX_MIC,
    DEV_IN_INDEX_HEADSET,
    DEV_IN_INDEX_BT,
    DEV_IN_INDEX_BTWB,
    DEV_IN_INDEX_FMRX,
    DEV_IN_INDEX_VOIP,
    DEV_IN_INDEX_VOICE_CALL,
    DEV_IN_INDEX_DEFAULT,
    NUM_DEV_IN_INDEXES
} dev_in_index;

typedef enum _routing_strategy {
    STRATEGY_MEDIA,
    STRATEGY_PHONE,
    STRATEGY_SONIFICATION,
    STRATEGY_DTMF,
    NUM_STRATEGIES
} routing_strategy;

typedef enum _voip_type {
    VOIP_INACTIVE,
    VOIP_VIDEO_CALL,
    VOIP_VOICE_CALL,
    VOIP_IN_COMMUNICATION
} voip_type;

// Maximum CPU load allocated to audio effects in 0.1 MIPS (ARMv5TE, 0 WS memory) units
static const uint32_t MAX_EFFECTS_CPU_LOAD = 1000;
// Maximum memory allocated to audio effects in KB
static const uint32_t MAX_EFFECTS_MEMORY = 512;

/* Descriptor for audio outputs.
 * Used to maintain current configuration of each opened audio output
 * and keep track of the usage of this output by each audio stream type.
 */

#ifdef ALLOW_DUPLICATION
#define NUM_DUP_OUTPUT 5

typedef struct duplicated_info{
    bool is_duplicated_output;    // Just to note that this output is a duplicated output
    audio_io_handle_t duplicated_output[NUM_DUP_OUTPUT];  // Just to keep reference on duplicated output (0 if is_duplicated_output=true)
    audio_io_handle_t primary_output;   // Previously named mId1 => reference to the primary output (0 if is_duplicated_output=false)
    audio_io_handle_t external_output;   // reference to the external output (0 if is_duplicated_output=false)
    struct ste_audio_output_desc *primary_desc;    // description of primary output (0 if is_duplicated_output=false)
    struct ste_audio_output_desc *external_desc;    // description of secondary output (0 if is_duplicated_output=false)
}duplicated_info_t;
#endif


typedef struct ste_audio_output_desc {
    uint32_t mSamplingRate;
    uint32_t mFormat;
    uint32_t mChannels;
    uint32_t mLatency;
    audio_output_flags_t mFlags;
    uint32_t mDevice; /* Current device this output is routed to */
    uint32_t mRefCount[AUDIO_STREAM_CNT];
    uint32_t mStoreRefCount[AUDIO_STREAM_CNT];  //TRACE PURPOSE
    nsecs_t mStopTime[AUDIO_STREAM_CNT];
        /* Number of streams of each stream type using this output */
    uint32_t mStrategyRefCount;
    int mAudioOutputRef;
#ifdef ALLOW_DUPLICATION
    duplicated_info_t duplication_info;
#endif
    routing_strategy mStrategy;
    voip_type mVoIPType;
    bool mVoIPActive;
    bool mSuspended;
    audio_io_handle_t output_id;
} ste_audio_output_descriptor_t;

/* Descriptor for audio inputs.
 * Used to maintain current configuration of each opened audio input
 * and keep track of the usage of this input.
 */

typedef struct ste_audio_input_desc
{
    uint32_t mSamplingRate;
    uint32_t mFormat;
    uint32_t mChannels;
    audio_in_acoustics_t mAcoustics;
    uint32_t mDevice; /* Current device this input is routed to */
    uint32_t mRefCount; /* Number of AudioRecord clients using this input */
    int      mInputSource; /* Input source selected by application */
    voip_type mVoIPType;
    bool mVoIPActive;
} ste_audio_input_desc_t;

/* Stream descriptor used for volume control */
typedef struct stream_desc
{
    void (*dump)(struct stream_desc *streamDescr, char* buffer, size_t size);

    int mIndexMin;      /* min volume index */
    int mIndexMax;      /* max volume index */
    int mIndexCur;      /* current volume index */
    int mMuteCount;     /* mute request counter */
    bool mCanBeMuted;   /* true if the stream can be muted */
} stream_descriptor_t;

/* Used for storing registered effects. */
typedef struct ste_effect_desc
{
    int (*dump)(struct ste_effect_desc *descr,int fd);

    int mIo;                    // input or output the effect is attached to
    routing_strategy mStrategy; // routing strategy the effect is associated to
    int mSession;               // audio session the effect is on
    effect_descriptor_t mDesc;  // effect descriptor
    bool mEnabled;              // enabled state: CPU load being used or not
} ste_effect_descriptor_t;


typedef struct audio_policy_manager_anm_struct
{
    /* Check call status after ADM connection problem, and recover call graph if needed */
    void (*check_call_status) (struct audio_policy *policy);

    /* Audio Policy client interface */
    struct audio_policy_service_ops *mp_client_interface;
    void *mService;
#ifdef ALLOW_DUPLICATION
    audio_io_handle_t duplicated_output[NUM_DUP_OUTPUT]; // duplicated output handler: outputs to hardware and external output
#endif

    /* Lists if output and input descriptors */
    node_audio_stream_t *mOutputs;
    node_audio_stream_t *mInputs;

    /* Bit field of all available output and input devices */
    uint32_t mAvailableOutputDevices;
    uint32_t mAvailableInputDevices;

    int mPhoneState;      /* Current phone state */
    uint32_t mRingerMode; /* Current ringer mode */
    bool mCSCallActive;   /* Current CS call status, active or not */
    bool mVoIPActive;     /* Current VoIP call status, active or not. */
    ste_adm_modem_type_t mModemType;     /* modem type */

    /* Current forced use configurations */
    audio_policy_forced_cfg_t mForceUse[AUDIO_POLICY_FORCE_CFG_CNT];
    /* Queued forced use configurations */
    audio_policy_forced_cfg_t mForceUseQueued[AUDIO_POLICY_FORCE_CFG_CNT];
    /* Stream descriptors for volume control */
    stream_descriptor_t mStreams[AUDIO_STREAM_CNT];

#ifdef USE_CACHE_MECHANISM
    uint32_t mDeviceForStrategy[NUM_STRATEGIES];
#endif
    /* Mutex to synchronize ForceUse requests */
    pthread_mutex_t mMutexForceUse;
    /* true if update of force use is ongoing */
    bool mForceUseOngoing;
    /* ADM handle, for CSCall control */
    int fd_adm;
    /* Mutex to protect simultaneous set CS call devices to ADM */
    pthread_mutex_t mMutexCallDevices;
    /* Mutex to protect simultaneous check call status from AHI */
    pthread_mutex_t mMutexCallStatus;
    /* Mutex to protect simultaneous calls to input/output functions */
    pthread_mutex_t mMutexIO;
    /* Mutex to protect device connection notifications */
    pthread_mutex_t mMutexDevState; /* Mutex to handle device connection notifications */

    uint32_t mTotalEffectsCpuLoad; // current CPU load used by effects
    uint32_t mTotalEffectsMemory;  // current memory used by effects
    node_audio_stream_t *mEffects;

    /* Current used strategy for an output device */
    routing_strategy mCurrentStrategy[NUM_DEV_OUT_INDEXES];
    #define STRATEGY_UNKNOWN NUM_STRATEGIES
    /* Current used source for an input device */
    int mCurrentSource[NUM_DEV_IN_INDEXES];
    #define AUDIO_SOURCE_UNKNOWN (AUDIO_SOURCE_CNT+1)

    /* Mutex to protect voice call open or close */
    pthread_mutex_t mMutexVC;
    /* Modem status from ADM */
    ste_adm_vc_modem_status_t mModemStatus;
    void *apm_ptr;
    void * ext_hal;

} audio_policy_anm;

/* Check if the specified format is supported as a coded format (HDMI) by the ADM */
static bool is_supported_coded_format(uint32_t format);

/* Compute volume for stream corresponding to index */
float compute_volume(audio_policy_anm *audio_pol_anm, int stream, int index, uint32_t device);

/* Set VoIP status, the samplerate and number of channels */
void set_voip_status(struct audio_policy *pol, bool enabled, int inSamplerate,int inChannels, int outSamplerate,int outChannels);
/* Check if output is VoIP type */
bool is_voip_output(audio_policy_anm *audio_pol_anm, ste_audio_output_descriptor_t *descr, audio_stream_type_t stream, bool *inputAvailable, ste_audio_input_desc_t **inDescr);
/* Check if input is VoIP type */
bool is_voip_input(audio_policy_anm *audio_pol_anm, ste_audio_input_desc_t *descr, bool *outputAvailable, ste_audio_output_descriptor_t **outDescr);
/* Check if VoIP type is synced. Synced VoIP type means that the
 * VoIP call graph requires both input and output to be active */
bool is_voip_synched(int type);
/* Close CS call / VoIP call */
void close_cs_call(audio_policy_anm *audio_pol_anm);
bool is_in_call(audio_policy_anm *audio_pol_anm);

// checks and if necessary changes output (a2dp, duplicated or hardware) used for all strategies.
// must be called every time a condition that affects the output choice for a given strategy is
// changed: connected device, phone state, force use...
// Must be called before update_device_for_strategy()
void check_output_for_strategy(audio_policy_anm *audio_pol_anm, routing_strategy strategy);
// Same as check_output_for_strategy() but for a all strategies in order of priority
void check_output_for_all_strategies(audio_policy_anm *audio_pol_anm);

#ifdef USE_CACHE_MECHANISM
void update_device_for_strategy(audio_policy_anm *audio_pol_anm);
#endif

/* Get preferred output device from the device prio list.
* Returns the preferred output device(s) from the output device prio list,
* based on new format to be played, strategy, mPhoneState and currently
* available output devices.
*/
uint32_t get_preferred_output_device(audio_policy_anm *audio_pol_anm, audio_io_handle_t output,
                                     routing_strategy strategy, uint32_t format);

/* Get forced output device, if any, based on strategy and mForceUse */
uint32_t get_forced_output_device(audio_policy_anm *audio_pol_anm, routing_strategy strategy);

/* Get mask for devices that can play the specified format on a new
* output, taking into consideration the current set of open devices */
uint32_t get_possible_output_device(audio_policy_anm *audio_pol_anm,
                                    audio_io_handle_t output, uint32_t format);

/* Get the output device that should be used for a specific strategy.
 *
 * First, this function checks the forced output device (using
 * get_forced_output_device).
 * Secondly, if no forced output device, the output device from the device
 * prio list is retrieved (using get_preferred_output_device)
 */
uint32_t get_output_device_for_strategy(audio_policy_anm *audio_pol_anm, audio_io_handle_t output,
                                        routing_strategy strategy, uint32_t format
#ifdef USE_CACHE_MECHANISM
    , bool fromCache
#endif
);

/* Get the routing stategy for an output */
routing_strategy get_output_strategy(audio_policy_anm *audio_pol_anm, audio_io_handle_t output);

/* Get the output device that should be used for a specific output.
 *
 * This function retrieves the strategy for this output, and then uses
 * get_output_device_for_strategy() to get the output device.
 */

uint32_t get_output_device(audio_policy_anm *audio_pol_anm, audio_io_handle_t output
#ifdef USE_CACHE_MECHANISM
        , bool fromCache
#endif
           );

/* Get the output device (using get_output_device) that should be used for
 * all different outputs, and if the device has changed update the device
 * routing.
 */
void update_output_routing(audio_policy_anm *audio_pol_anm
#ifdef USE_CACHE_MECHANISM
        , bool fromCache
#endif
           );

void do_update_routing(audio_policy_anm *audio_pol_anm, audio_io_handle_t output, uint32_t device);

/* Update the configuration used by the devices. Different device
 * configurations are applied for different strategies */
void update_output_config(audio_policy_anm *audio_pol_anm, ste_audio_output_descriptor_t *descr);

/* Get preferred input device from the device prio list.
 * Returns the preferred input device(s) from the input device prio list,
 * based on source, mPhoneState, mono/stereo and currently available input
 * devices.
 */
uint32_t get_preferred_input_device(audio_policy_anm *audio_pol_anm, int source, int channels);

/* Get forced input device, if any, based on source and mForceUse */
uint32_t get_forced_input_device(audio_policy_anm *audio_pol_anm, int source);

/* Get the input device that should be used for a specific source.
 *
 * First, this function checks the forced input device (using
 * get_forced_input_device).
 * Secondly, if no forced input device, the input device from the device
 * prio list is retrieved (using get_preferred_input_device)
 */
uint32_t get_input_device_for_source(audio_policy_anm *audio_pol_anm, int source, int channels);

/* Get the source for an input */
int get_input_source(audio_policy_anm *audio_pol_anm, audio_io_handle_t input);

/* Get the input device that should be used for a specific input.
 *
 * This function retrieves the source for this input, and then uses
 * get_input_device_for_source() to get the input device.
 */
uint32_t get_input_device(audio_policy_anm *audio_pol_anm, audio_io_handle_t input);

/* Get the input device (using get_input_device) that should be used for
 * all different inputs, and if the device has changed update the device
 * routing.
 */
void update_input_routing(audio_policy_anm *audio_pol_anm);

/* Update the configuration used by the devices. Different device
 * configurations are applied for different input sources */
void update_input_config(audio_policy_anm *audio_pol_anm, ste_audio_input_desc_t *descr);

/* Check HW resources and other constraints for using device.
If device is not allowed to be used in the current scenario,
it will be updated to the preferred allowed device */
void check_hw_resources(audio_policy_anm *audio_pol_anm, uint32_t *device);

/* Get preferred input and output device for cscall and update routing. */
void update_cscall_routing(struct audio_policy *pol);

uint32_t get_max_effects_cpu_load();
uint32_t get_max_effects_memory();

void ste_ap_init(audio_policy_anm *audio_pol_anm, struct audio_policy *policy,
                 struct audio_policy_service_ops *clientInterface, void *service);

void deinit_audio_policy_manager(audio_policy_anm *audio_pol_anm);

int set_effect_enabled(audio_policy_anm *audio_pol_anm, ste_effect_descriptor_t *pDesc, bool enabled);

/* Returns true if wideband (16kHz) is supported by BT HS */
bool is_btwb_supported();

/* Returns true if noise reduction (NR) and echo cancelling (EC) is supported by BT HS */
bool is_bt_nr_ec_supported();

/* Force use thread, used to handle multiple fast force use changes */
void *thread_force_use(void* param);

/* Get the ADM top level device index corresponding to device */
uint32_t get_top_level_device_index(audio_policy_anm *audio_pol_anm, uint32_t device);
/* Get the actual device, i.e. the entry in the Device table of the
 * ADM database to which the top level device should be mapped */
void get_actual_input_device(audio_policy_anm *audio_pol_anm, uint32_t device, int source, const char **actual);
void get_actual_output_device(audio_policy_anm *audio_pol_anm, uint32_t device, routing_strategy strategy, const char **actual);
/* Update the DeviceMap table of the ADM database with actual device */
void update_top_level_device_map(const char *toplevel, const char *actual, const char *toplevel2, const char *actual2);

audio_policy_anm *to_apm_anm(struct audio_policy *pol);

/* Voice call open/close handling */
void     vc_open_request(struct audio_policy *pol);
void     vc_close_request(audio_policy_anm *audio_pol_anm);
void     subscribe_modem_status(struct audio_policy *policy);

/* Update the force use configuration */
void     update_force_use(struct audio_policy *pol);
void*    thread_subscribe_modem_status(void *param);
void     init_audio_input_descriptor(ste_audio_input_desc_t *inputDesc);
void     init_stream_descriptor(stream_descriptor_t *streamDescr);
void     init_effect_descriptor(ste_effect_descriptor_t *descr);
void     init_audio_output_descriptor(ste_audio_output_descriptor_t *output_descr);

void     anm_out_desc_set_audio_output_ref(ste_audio_output_descriptor_t *outputDescr, int ref);
uint32_t anm_out_desc_device(ste_audio_output_descriptor_t *output_descr);
void     anm_out_desc_change_ref_count(ste_audio_output_descriptor_t *output_descr, audio_stream_type_t stream, int delta);
uint32_t anm_out_desc_ref_count(ste_audio_output_descriptor_t *output_descr);
uint32_t anm_out_desc_strategy_ref_count(ste_audio_output_descriptor_t *output_descr, routing_strategy strategy);
int      anm_out_desc_dump(ste_audio_output_descriptor_t *output_descr,int fd);
bool     anm_out_desc_is_used_by_stream(ste_audio_output_descriptor_t *outputDescr, audio_stream_type_t stream);
bool     anm_out_desc_is_used_by_strategy(ste_audio_output_descriptor_t *outputDescr,routing_strategy strategy);
int      anm_out_desc_get_audio_output_ref(ste_audio_output_descriptor_t *outputDescr);

#ifdef ALLOW_DUPLICATION
bool     anm_out_desc_is_duplicated(ste_audio_output_descriptor_t *outputDescr);
#endif
int      dump_effect_descriptor(ste_effect_descriptor_t *descr,int fd);

/* Policy Functions */
int      ste_anm_ap_set_effect_enabled(struct audio_policy *pol, int id, bool enabled);
int      ste_anm_ap_set_stream_volume_index(struct audio_policy *pol, audio_stream_type_t stream, int index);

/* Input Functions */
int      anm_in_desc_dump(ste_audio_input_desc_t *inputDesc, int fd);

#endif //STE_HAL_AUDIO_POLICY_MANAGER_ANM_H
