/*! \file ste_anm_ap.h
*   \brief Header for STE Audio Policy

    This header defines the derived class from Android AudioPolicyInterface that is used
    in the STE Android port to control audio policy.
*/

/*******************************************************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \brief   Header of Audio Policy Manager of ANM to be used by AudioFlinger in Android framework.
 */
/********************************************************************************************************************/

#ifndef ANDROID_AUDIO_POLICY_MANAGER_ANM_H
#define ANDROID_AUDIO_POLICY_MANAGER_ANM_H

#include <cutils/log.h>
#include <stdio.h>

#include <stdint.h>
#include <sys/types.h>

#include <utils/Errors.h>
#include <utils/KeyedVector.h>
#include <hardware_legacy/AudioPolicyInterface.h>
#include <hardware_legacy/AudioSystemLegacy.h>
#include <media/AudioParameter.h>
#include <utils/threads.h>
#include <ste_adm_client.h>
#include <linux_utils.h>

using android::KeyedVector;

namespace android_audio_legacy
{

// ----------------------------------------------------------------------------
#ifdef STD_A2DP_MNGT
#define MAX_DEVICE_ADDRESS_LEN 20
#endif

class AudioPolicyManagerANM: public AudioPolicyInterface
{

public:
    AudioPolicyManagerANM(AudioPolicyClientInterface *clientInterface);
    virtual ~AudioPolicyManagerANM();

    /* AudioPolicyInterface */

    /* Set/Get state of a device, i.e. if the device is available or not */
    virtual status_t setDeviceConnectionState(AudioSystem::audio_devices device,
        AudioSystem::device_connection_state state, const char *device_address);
    virtual AudioSystem::device_connection_state getDeviceConnectionState(
        AudioSystem::audio_devices device, const char *device_address);

    /* Set/Get force use configuration, see AudioSystem::force_use and
     * AudioSystem::forced_config for valid values */
    virtual void setForceUse(AudioSystem::force_use usage,
        AudioSystem::forced_config config);
    virtual AudioSystem::forced_config getForceUse(
        AudioSystem::force_use usage);

    /* Set phone state, see AudioSystem::audio_mode for valid values */
    virtual void setPhoneState(int state);
    virtual void setRingerMode(uint32_t mode, uint32_t mask);
    virtual void setSystemProperty(const char* property, const char* value);
    virtual status_t initCheck();

    /* Get output for specified stream type */
    virtual audio_io_handle_t getOutput(
        AudioSystem::stream_type stream, uint32_t samplingRate, uint32_t format,
        uint32_t channels, AudioSystem::output_flags flags = AudioSystem::OUTPUT_FLAG_INDIRECT);
    /* startOutput indicates that the specified output is in use */
    virtual status_t startOutput(
        audio_io_handle_t output, AudioSystem::stream_type stream, int session = 0);
    /* stopOutput indicates that the specified output is not in use anymore */
    virtual status_t stopOutput(
        audio_io_handle_t output, AudioSystem::stream_type stream, int session = 0);
    /* Release resources associated with the specified output */
    virtual void releaseOutput(audio_io_handle_t output);

    /* Get input for specified input source */
    virtual audio_io_handle_t getInput(
        int inputSource, uint32_t samplingRate, uint32_t format,
        uint32_t channels, AudioSystem::audio_in_acoustics acoustics,
        audio_input_clients *inputClient);
    /* startInput indicates that the specified input is in use */
    virtual status_t startInput(audio_io_handle_t input);
    /* stopInput indicates that the specified input is not in use anymore */
    virtual status_t stopInput(audio_io_handle_t input);
    /* Release resources associated with the specified input */
    virtual void releaseInput(audio_io_handle_t input);

    /* Set/Get volume for a specified stream type */
    virtual void initStreamVolume(
        AudioSystem::stream_type stream, int indexMin, int indexMax);
    virtual status_t setStreamVolumeIndex(
        AudioSystem::stream_type stream, int index);
    virtual status_t getStreamVolumeIndex(
        AudioSystem::stream_type stream, int *index);

    // return the strategy corresponding to a given stream type
    virtual uint32_t getStrategyForStream(AudioSystem::stream_type stream);

    // return the strategy corresponding to a given stream type
    virtual uint32_t getDevicesForStream(AudioSystem::stream_type stream);

    // Audio effect management
    virtual audio_io_handle_t getOutputForEffect(effect_descriptor_t *desc);
    virtual status_t registerEffect(effect_descriptor_t *desc,
                                    audio_io_handle_t io,
                                    uint32_t strategy,
                                    int session,
                                    int id);
    virtual status_t unregisterEffect(int id);
    virtual status_t setEffectEnabled(int id, bool enabled);

    virtual bool isStreamActive(int stream, uint32_t inPastMs = 0) const;

    virtual status_t dump(int fd);

    /* Check call status after ADM connection problem,
     * and recover call graph if needed */
    void checkCallStatus();

    /* Update the force use configuration */
    void updateForceUse();


    enum routing_strategy {
        STRATEGY_MEDIA,
        STRATEGY_PHONE,
        STRATEGY_SONIFICATION,
        STRATEGY_DTMF,
        NUM_STRATEGIES
    };

    enum voip_type {
        VOIP_INACTIVE,
        VOIP_VIDEO_CALL,
        VOIP_VOICE_CALL,
        VOIP_IN_COMMUNICATION
    };

private:

    /* Descriptor for audio outputs.
     * Used to maintain current configuration of each opened audio output
     * and keep track of the usage of this output by each audio stream type.
     */
    class AudioOutputDescriptor
    {
    public:
        AudioOutputDescriptor();
        status_t dump(int fd);
        uint32_t device();
        void changeRefCount(AudioSystem::stream_type, int delta);
        bool isUsedByStream(AudioSystem::stream_type stream) {
            return mRefCount[stream] > 0 ? true : false;
        }
        uint32_t refCount();
        uint32_t strategyRefCount(routing_strategy strategy);
        bool isUsedByStrategy(routing_strategy strategy) {
            return (strategyRefCount(strategy) != 0);
        }
        void setAudioOutputRef(int ref) { mAudioOutputRef = ref; }
        int getAudioOutputRef() { return mAudioOutputRef; }

#ifdef STD_A2DP_MNGT
        bool isDuplicated() { return (mOutput1 != NULL && mOutput2 != NULL); }
#endif
        uint32_t mSamplingRate;
        uint32_t mFormat;
        uint32_t mChannels;
        uint32_t mLatency;
        AudioSystem::output_flags mFlags;
        uint32_t mDevice; /* Current device this output is routed to */
        uint32_t mRefCount[AudioSystem::NUM_STREAM_TYPES];
        nsecs_t mStopTime[AudioSystem::NUM_STREAM_TYPES];
            /* Number of streams of each stream type using this output */
        uint32_t mStrategyRefCount;
        int mAudioOutputRef;
#ifdef STD_A2DP_MNGT
        audio_io_handle_t mId1;
        AudioOutputDescriptor *mOutput1;    // used by duplicated outputs: first output
        AudioOutputDescriptor *mOutput2;    // used by duplicated outputs: second output
#endif
        AudioPolicyManagerANM::routing_strategy mStrategy;
        voip_type mVoIPType;
        bool mVoIPActive;
        bool mSuspended;
    };

    /* Descriptor for audio inputs.
     * Used to maintain current configuration of each opened audio input
     * and keep track of the usage of this input.
     */
    class AudioInputDescriptor
    {
    public:
        AudioInputDescriptor();
        status_t dump(int fd);
        uint32_t mSamplingRate;
        uint32_t mFormat;
        uint32_t mChannels;
        AudioSystem::audio_in_acoustics mAcoustics;
        uint32_t mDevice; /* Current device this input is routed to */
        uint32_t mRefCount; /* Number of AudioRecord clients using this input */
        int      mInputSource; /* Input source selected by application */
        uint32_t *mInputClient;
        voip_type mVoIPType;
        bool mVoIPActive;
    };

    /* Stream descriptor used for volume control */
    class StreamDescriptor
    {
    public:
        StreamDescriptor() : mIndexMin(0), mIndexMax(1), mIndexCur(1),
            mMuteCount(0), mCanBeMuted(true) {}

        void dump(char* buffer, size_t size);

        int mIndexMin;      /* min volume index */
        int mIndexMax;      /* max volume index */
        int mIndexCur;      /* current volume index */
        int mMuteCount;     /* mute request counter */
        bool mCanBeMuted;   /* true if the stream can be muted */
    };

    // Used for storing registered effects.
    class EffectDescriptor
    {
    public:

        status_t dump(int fd);

        int mIo;                    // input or output the effect is attached to
        routing_strategy mStrategy; // routing strategy the effect is associated to
        int mSession;               // audio session the effect is on
        effect_descriptor_t mDesc;  // effect descriptor
        bool mEnabled;              // enabled state: CPU load being used or not
    };

    /* Check if the specified format is supported as a coded format (HDMI)
    * by the ADM */
    static bool isSupportedCodedFormat(uint32_t format);

    /* Get routing strategy corresponding to the stream type */
    static routing_strategy getStrategy(AudioSystem::stream_type stream);

    /* Compute volume for stream corresponding to index */
    float computeVolume(int stream, int index, uint32_t device);
    /* Mute or unmute the stream on the specified output */
    void setStreamMute(int stream, bool on, audio_io_handle_t output);

    /* Set VoIP status, the samplerate and number of channels */
    void setVoIPStatus(bool enabled, int inSamplerate,int inChannels, int outSamplerate,int outChannels);
    /* Check if output is VoIP type */
    bool isVoIPOutput(AudioOutputDescriptor *descr, AudioSystem::stream_type stream, bool *inputAvailable, AudioInputDescriptor **inDescr);
    /* Check if input is VoIP type */
    bool isVoIPInput(AudioInputDescriptor *descr, bool *outputAvailable, AudioOutputDescriptor **outDescr);
    /* Check if VoIP type is synced. Synced VoIP type means that the
     * VoIP call graph requires both input and output to be active */
    bool isVoIPSynced(int type);
    /* Close CS call / VoIP call */
    void closeCSCall();

#ifdef STD_A2DP_MNGT
    bool isInCall();

    // true is current platform supports suplication of notifications and ringtones over A2DP output
    virtual bool a2dpUsedForSonification() const { return true; }
    status_t handleA2dpConnection(AudioSystem::audio_devices device,
                                                        const char *device_address);
    status_t handleA2dpDisconnection(AudioSystem::audio_devices device,
                                                        const char *device_address);
    void closeA2dpOutputs();
    // checks and if necessary changes output (a2dp, duplicated or hardware) used for all strategies.
    // must be called every time a condition that affects the output choice for a given strategy is
    // changed: connected device, phone state, force use...
    // Must be called before updateDeviceForStrategy()
    void checkOutputForStrategy(routing_strategy strategy);
    // Same as checkOutputForStrategy() but for a all strategies in order of priority
    void checkOutputForAllStrategies();
    // manages A2DP output suspend/restore according to phone state and BT SCO usage
    void checkA2dpSuspend();
    // updates cache of device used by all strategies (mDeviceForStrategy[])
    // must be called every time a condition that affects the device choice for a given strategy is
    // changed: connected device, phone state, force use...
    // cached values are used by getOutputDeviceForStrategy() if parameter fromCache is true.
     // Must be called after checkOutputForAllStrategies()
    void updateDeviceForStrategy();
#endif

    /* Get preferred output device from the device prio list.
     * Returns the preferred output device(s) from the output device prio list,
     * based on new format to be played, strategy, mPhoneState and currently
     * available output devices.
     */
    uint32_t getPreferredOutputDevice(audio_io_handle_t output,
                 routing_strategy strategy,
                 uint32_t format);

    /* Get forced output device, if any, based on strategy and mForceUse */
    uint32_t getForcedOutputDevice(routing_strategy strategy);

    /* Get mask for devices that can play the specified format on a new
    * output, taking into consideration the current set of open devices */
    uint32_t getPossibleOutputDevices(audio_io_handle_t output,
                 uint32_t format) const;

    /* Get the output device that should be used for a specific strategy.
     *
     * First, this function checks the forced output device (using
     * getForcedOutputDevice).
     * Secondly, if no forced output device, the output device from the device
     * prio list is retrieved (using getPreferredOutputDevice)
     */
    uint32_t getOutputDeviceForStrategy(audio_io_handle_t output,
                 routing_strategy strategy,
                 uint32_t format
#ifdef STD_A2DP_MNGT
                 , bool fromCache = false
#endif
                );

    /* Get the routing stategy for an output */
    routing_strategy getOutputStrategy(audio_io_handle_t output);

    /* Get the output device that should be used for a specific output.
     *
     * This function retrieves the strategy for this output, and then uses
     * getOutputDeviceForStrategy() to get the output device.
     */
    uint32_t getOutputDevice(audio_io_handle_t output
#ifdef STD_A2DP_MNGT
                 , bool fromCache = false
#endif
                );

    /* Get the output device (using getOutputDevice) that should be used for
     * all different outputs, and if the device has changed update the device
     * routing.
     */
    void updateOutputRouting(
#ifdef STD_A2DP_MNGT
                 bool fromCache = false
#endif
                );
    void doOutputRouting(audio_io_handle_t output, uint32_t device);

    /* Update the configuration used by the devices. Different device
     * configurations are applied for different strategies */
    void updateOutputConfig(AudioOutputDescriptor *descr);

    /* Get preferred input device from the device prio list.
     * Returns the preferred input device(s) from the input device prio list,
     * based on source, mPhoneState, mono/stereo and currently available input
     * devices.
     */
    uint32_t getPreferredInputDevice(int source, int channels);

    /* Get forced input device, if any, based on source and mForceUse */
    uint32_t getForcedInputDevice(int source);

    /* Get the input device that should be used for a specific source.
     *
     * First, this function checks the forced input device (using
     * getForcedInputDevice).
     * Secondly, if no forced input device, the input device from the device
     * prio list is retrieved (using getPreferredInputDevice)
     */
    uint32_t getInputDeviceForSource(int source, int channels);

    /* Get the source for an input */
    int getInputSource(audio_io_handle_t input);

    /* Get the input device that should be used for a specific input.
     *
     * This function retrieves the source for this input, and then uses
     * getInputDeviceForSource() to get the input device.
     */
    uint32_t getInputDevice(audio_io_handle_t input);

    /* Get the input device (using getInputDevice) that should be used for
     * all different inputs, and if the device has changed update the device
     * routing.
     */
    void updateInputRouting();
    /* Update the configuration used by the devices. Different device
     * configurations are applied for different input sources */
    void updateInputConfig(AudioInputDescriptor *descr);

    /* Check HW resources and other constraints for using device.
       If device is not allowed to be used in the current scenario,
       it will be updated to the preferred allowed device */
    void checkHwResources(uint32_t *device);

    /* Get preferred input and output device for cscall and update routing. */
    void updateCSCallRouting();

    virtual uint32_t getMaxEffectsCpuLoad();
    virtual uint32_t getMaxEffectsMemory();

    status_t setEffectEnabled(EffectDescriptor *pDesc, bool enabled);

    /* Audio Policy client interface */
    AudioPolicyClientInterface *mpClientInterface;
#ifdef STD_A2DP_MNGT
    audio_io_handle_t mA2dpOutput;       // A2DP output handler
    audio_io_handle_t mDuplicatedOutput; // duplicated output handler: outputs to hardware and A2DP
#endif

    /* Lists if output and input descriptors */
    KeyedVector<audio_io_handle_t, AudioOutputDescriptor *> mOutputs;
    KeyedVector<audio_io_handle_t, AudioInputDescriptor *> mInputs;
    /* Bit field of all available output and input devices */
    uint32_t mAvailableOutputDevices;
    uint32_t mAvailableInputDevices;

    int mPhoneState;      /* Current phone state */
    uint32_t mRingerMode; /* Current ringer mode */
    bool mCSCallActive;   /* Current CS call status, active or not */
    bool mVoIPActive;     /* Current VoIP call status, active or not. */
    ste_adm_modem_type_t mModemType;     /* modem type */

    /* Force use thread, used to handle multiple fast force use changes */
    static void* threadForceUse(void* param);
    /* Current forced use configurations */
    AudioSystem::forced_config mForceUse[AudioSystem::NUM_FORCE_USE];
    /* Queued forced use configurations */
    AudioSystem::forced_config mForceUseQueued[AudioSystem::NUM_FORCE_USE];
    /* Stream descriptors for volume control */
    StreamDescriptor mStreams[AudioSystem::NUM_STREAM_TYPES];
#ifdef STD_A2DP_MNGT
    String8 mA2dpDeviceAddress; // A2DP device MAC address
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

    // Maximum CPU load allocated to audio effects in 0.1 MIPS (ARMv5TE, 0 WS memory) units
    static const uint32_t MAX_EFFECTS_CPU_LOAD = 1000;
    // Maximum memory allocated to audio effects in KB
    static const uint32_t MAX_EFFECTS_MEMORY = 512;
    uint32_t mTotalEffectsCpuLoad; // current CPU load used by effects
    uint32_t mTotalEffectsMemory;  // current memory used by effects
    KeyedVector<int, EffectDescriptor *> mEffects;  // list of registered audio effects

    /* Returns true if wideband (16kHz) is supported by BT HS */
    bool isBtWbSupported();

    /* Returns true if noise reduction (NR) and echo cancelling (EC) is supported by BT HS */
    bool isBtNrEcSupported();
#ifdef STD_A2DP_MNGT
    bool    mA2dpSuspended; // true if A2DP output is suspended
#endif

    /* Output device indexes */
    enum dev_out_index {
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
    };
    /* Input device indexes */
    enum dev_in_index {
        DEV_IN_INDEX_MIC,
        DEV_IN_INDEX_HEADSET,
        DEV_IN_INDEX_BT,
        DEV_IN_INDEX_BTWB,
        DEV_IN_INDEX_FMRX,
        DEV_IN_INDEX_VOIP,
        DEV_IN_INDEX_VOICE_CALL,
        DEV_IN_INDEX_DEFAULT,
        NUM_DEV_IN_INDEXES
    };
    /* Get the ADM top level device index corresponding to device */
    uint32_t getTopLevelDeviceIndex(uint32_t device);
    /* Get the actual device, i.e. the entry in the Device table of the
     * ADM database to which the top level device should be mapped */
    void getActualInputDevice(uint32_t device, int source, const char **actual);
    void getActualOutputDevice(uint32_t device, routing_strategy strategy, const char **actual);
    /* Update the DeviceMap table of the ADM database with actual device */
    void updateTopLevelDeviceMap(const char *toplevel, const char *actual, const char *toplevel2, const char *actual2);
    /* Current used strategy for an output device */
    routing_strategy mCurrentStrategy[NUM_DEV_OUT_INDEXES];
    #define STRATEGY_UNKNOWN NUM_STRATEGIES
    /* Current used source for an input device */
    int mCurrentSource[NUM_DEV_IN_INDEXES];
    #define AUDIO_SOURCE_UNKNOWN AUDIO_SOURCE_LIST_END+1

    /* Voice call open/close handling */
    void vcOpenRequest(void);
    void vcCloseRequest(void);
    void subscribeModemStatus(void);
    static void* threadSubscribeModemStatus(void *param);
    /* Mutex to protect voice call open or close */
    pthread_mutex_t mMutexVC;
    /* Modem status from ADM */
    ste_adm_vc_modem_status_t mModemStatus;

};

}; // namespace android_audio_legacy

#endif //ANDROID_AUDIO_POLICY_MANAGER_ANM_H
