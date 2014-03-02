/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*! \file ste_anm.h
*   \brief Defines for ANM.

    This header defines the derived class from Anroid AHI that needs to be
    implemented in ANM.
*/

/* status_t is based on include/utils/Errors.h for reference it is included
   here */
#if 0
enum {
    OK                = 0,    // Everything's swell.
    NO_ERROR          = 0,    // No errors.

    UNKNOWN_ERROR       = 0x80000000,

    NO_MEMORY           = -ENOMEM,
    INVALID_OPERATION   = -ENOSYS,
    BAD_VALUE           = -EINVAL,
    BAD_TYPE            = 0x80000001,
    NAME_NOT_FOUND      = -ENOENT,
    PERMISSION_DENIED   = -EPERM,
    NO_INIT             = -ENODEV,
    ALREADY_EXISTS      = -EEXIST,
    DEAD_OBJECT         = -EPIPE,
    FAILED_TRANSACTION  = 0x80000002,
    JPARKS_BROKE_IT     = -EPIPE, <<<--LOL
#if !defined(HAVE_MS_C_RUNTIME)
    BAD_INDEX           = -EOVERFLOW,
    NOT_ENOUGH_DATA     = -ENODATA,
    WOULD_BLOCK         = -EWOULDBLOCK,
    TIMED_OUT           = -ETIME,
    UNKNOWN_TRANSACTION = -EBADMSG,
#else
    BAD_INDEX           = -E2BIG,
    NOT_ENOUGH_DATA     = 0x80000003,
    WOULD_BLOCK         = 0x80000004,
    TIMED_OUT           = 0x80000005,
    UNKNOWN_TRANSACTION = 0x80000006,
#endif
};
#endif

#ifndef INCLUSION_GUARD_STE_ANM_H
#define INCLUSION_GUARD_STE_ANM_H

#include <utils/List.h>
#include <hardware_legacy/AudioHardwareBase.h>
#include <ste_adm_client.h> /* for HW params struct */
#include <cstdio>
#include <utils/SortedVector.h>
#include <pthread.h>
#include <linux_utils.h>

/* Key strings for setParameters() / getParameters() */
#define PARAM_KEY_EC_SUPPORTED      "ec_supported"
#define PARAM_KEY_BTWB              "bt_headset_wideband"
#define PARAM_KEY_BTNREC            "bt_headset_nrec"
#define PARAM_KEY_STREAM_STATE      "ste_stream_state"
#define PARAM_KEY_STREAM_REF        "ste_stream_ref"
#define PARAM_KEY_STREAM_STANDBY    "ste_stream_standby"
#define PARAM_KEY_AP_REF            "ste_ap_ref"
#define PARAM_KEY_TTY_MODE          "tty_mode"

using android::List;
using android::SortedVector;

/* We implement inside the android_audio_legacy namespace */
namespace android_audio_legacy {

    /**
    * Common functionality for Input and output interfaces.
    */
    class StreamADMBase
    {
        protected:
            /**
            * Ctor.
            */
            StreamADMBase();
            /**
            * Dtor.
            */
            ~StreamADMBase();

            /** Set up connection to ADM.
            * @return: 0 on success, negative error code on failure.
            */
            int setupADMConnection();

            /** Close connection to ADM
            * @return: 0 on success, negative error code on failure.
            */
            int closeADMConnection();

            /**
             * Convert device id to device string
             */
            char* devices2str(char* buf, AudioSystem::audio_devices dev);

            /**
            * Opens all the devices currently residing in mDeviceList.
            * If not in standby mode, do nothing.
            * @return Android error code.
            */
            status_t openDevices();

            int mADMConnectionID;
            /**< File descriptor holding the ADM connection ID */
            List<const char*> mDeviceList;
            /**< List of currently active devices */
            bool mStandby;
            /**< If we are in standby mode or not */
            bool mStarted;
            /**< true when corresponding input/output is started */
            bool mStandbyPending;
            /**< true when standby has been requested, but mStarted is true */
            int mChannels;
            /**< Mono/Stereo */
            uint32_t mSampleRate;
            /**< SampleRate in Hz */
            int mFormat;
            /**< PCM format */

            int mAdmNumBufs;
            /**< Number of ADM shared memory buffers */
            int mAdmBufSize;
            /**< Size of each ADM shared memory buffer */
            char* mAdmBufSharedMem;
            /**< Pointer to start of ADM shared memory area. Size is mAdmBufSize * mAdmNumBufs */

            pthread_mutex_t mMutex;
            /**< Mutex to protect simultaneous calls to ADM */

            virtual status_t setParameters(const String8& keyValuePairs);
            virtual String8 getParameters(const String8& keys);

            /**< Called by openDevices, after a device has been opened. */
            virtual void postOpenDevice(void) = 0;

            /**< Called just before closing a device */
            virtual void preCloseDevice(void) = 0;

            /**
            * Calculate wanted buffer size
            */
            virtual size_t calc_buffer_size() const = 0;

            /**
             * Put the audio hardware output into standby mode. Returns
             * status based on include/utils/Errors.h
             * @return Android error code.
             */
            status_t standby_imp();

            /**
            * Translates the audio format specifier in mFormat and mChannels to
            * the ADM representation.
            */
            ste_adm_format_t get_adm_format(void) const;
    };

    /**
    * ANM implementation class for the output device specific class in AHI.
    * Inherits from AudioStreamOut defined in AHI.
    */
    class AudioStreamOutANM : public AudioStreamOut, public StreamADMBase
    {
        public:
            /**
            * Ctor
            */
            AudioStreamOutANM();
            /**
            * Dtor
            */
            ~AudioStreamOutANM();
            /*******************************************************************
            *
            * Beginning of inherited methods from AudioStreamOut.
            *
            *******************************************************************/

            /**
            * [Part of AudioStreamOut interface]
            * Return audio sampling rate in Hz.
            * @return Sample rate of the device in Hz.
            */
            uint32_t sampleRate() const;

            /**
            * [Part of AudioStreamOut interface]
            * Returns size of output buffer in bytes.
            * @return Size of the output buffer in bytes.
            */
            size_t bufferSize() const;

            /**
             * [Part of AudioStreamOut interface]
             * Returns the output channel mask.
             * Acceptable values are 1 (mono) or 2 (stereo)
             * @return 1 = mono, 2 = stereo.
             */
            uint32_t channels() const;

            /**
             * [Part of AudioStreamOut interface]
             * return audio format in 8bit or 16bit PCM format -
             * eg. AudioSystem:PCM_16_BIT
             * @return The PCM format in Android domain, see AudioSystem.h.
             */
            int format() const;

            /**
             * [Part of AudioStreamOut interface]
             * return the audio hardware driver latency in milli seconds.
             * @return HW latency in milli seconds.
             */
            uint32_t latency() const;

            /**
             * [Part of AudioStreamOut interface]
             * Use this method in situations where audio mixing is done in the
             * hardware. This method serves as a direct interface with hardware,
             * allowing you to directly set the volume as apposed to via the
             * framework. This method might produce multiple PCM outputs or
             * hardware accelerated codecs, such as MP3 or AAC.
             * @param volume Normalized volume factor.
             * @return Android error code.
             */
            status_t setVolume(float left, float right);

            /**
            * [Part of AudioStreamOut interface]
            * Write audio buffer to driver. Returns number of bytes written.
            * @param buffer Pointer to audio buffer.
            * @param bytes The number of bytes in the audio buffer.
            * @return The number of bytes written to the device.
            */
            ssize_t write(const void* buffer, size_t bytes);

            /**
            * [Part of AudioStreamOut interface]
            * dump the state of the audio output device
            * @param fd File descriptor to dump state into.
            * @param args @todo
            * @return Android error code.
            */
            status_t dump(int fd, const Vector<String16>& args);


            /**
             * [Part of AudioStreamOut interface]
             * Return the number of audio frames written by the audio dsp to
             * DAC since the output has exited standby
             * @param dspFrames Number of frames
             */
            status_t getRenderPosition(uint32_t *dspFrames);
            /*******************************************************************
            *
            * End of inherited methods from AudioStreamOut.
            *
            *******************************************************************/
            /**
            * Change active device routing.
            * @param mode Routing mode, see AudioSystem.h
            * @param routes Bit mask containing the routes to be used,
            *               see AudioSystem.h
            * @return Android error code.
            */
            status_t changeDevice(int mode, uint32_t routes);

            /**
            * Setup the output device, can't be done in
            * constructor since we need to indicate success or failure and we
            * don't like exceptions.
            *
            * @param format PCM format in Android domain.
            * @param channels The number of channels.
            * @param sampleRate Sample rate of incoming audio data in Hz [1/s].
            * @return Android error code.
            */
            status_t setup(
                int *format, uint32_t *channels, uint32_t *sampleRate);

            status_t setParameters(const String8& keyValuePairs);
            String8 getParameters(const String8& keys);

            void postOpenDevice(void);
            void preCloseDevice(void);

            /**
            * [Part of AudioStreamOut interface]
            * Put the audio hardware output into standby mode. Returns
            * status based on include/utils/Errors.h
            */
            status_t standby();

            int mNewDevices;
            /**< Bit mask of new devices to which audio should be routed */
            bool mChangingDevice;
            /**< true when changing device is ongoing */
            int mNewADMConnID;
            /**< ADM ID where new devices are opened */
            int mNewAdmNumBufs;
            /**< Number of ADM shared memory buffers */
            int mNewAdmBufSize;
            /**< Size of each ADM shared memory buffer */
            char* mNewAdmBufSharedMem;
            /**< Pointer to start of ADM shared memory area. Size is mAdmBufSize * mAdmNumBufs */
            int mOldDevices;
            /**< Bit mask of old devices to be closed */
            int mOldADMConnID;
            /**< ADM ID with old devices to be closed */
            int mCurBufIdx;
            /**< Index of next buffer to play */

        private:
            uint32_t mDevices;
            /**< Bit mask of currently active devices */
            int mLatency;
            /**< Current latency value for this device */
            audio_io_handle_t mLatencyReportOutput;
            /**< Output to report change in sink latency */
            int mLpaMode;
            /**< LPA mode enabled or disabled */
            int mCurBufOffset;
            /**< Offset in current buffer to continue filling at*/

            /**
            * Calculate wanted buffer size
            */
            virtual size_t calc_buffer_size() const;

            static void* open_dev_thread(void* param);
            static void* close_dev_thread(void* param);

            status_t standby_l();

            static void* report_latency_thread(void* param);
            void notifySinkLatency(audio_io_handle_t output);

            void refreshDeviceList(List <const char*>&v, uint32_t routes);
    }; /* class AudioStreamOutANM */

    /**
    * ANM implementation class for the input device specific class in AHI.
    * Inherits from AudioStreamIn defined in AHI.
    */
    class AudioStreamInANM : public AudioStreamIn, public StreamADMBase
    {
        public:
            /**
            * Ctor
            */
            AudioStreamInANM();
            /**
            * Dtor
            */
            virtual ~AudioStreamInANM();

            /**
            * [Part of AudioStreamIn interface]
            * Returns the audio sampling rate.
            */
            uint32_t sampleRate() const;

            /**
            * [Part of AudioStreamIn interface]
            * Return the input buffer size allowed by audio driver.
            * @return Input buffer size.
            */
            size_t bufferSize() const;

            /**
            * [Part of AudioStreamIn interface]
            * Returns the input channel mask.
            * @return 1 = mono, 2 = stereo.
            */
            uint32_t channels() const;

            /**
             * [Part of AudioStreamIn interface]
             * return audio format in 8bit or 16bit PCM format -
             * eg. AudioSystem:PCM_16_BIT
             * @return The PCM format in Android domain, see AudioSystem.h.
             */
            int format() const;

            /*
             * Return the list of devices.
             */
            List<const char*>* deviceList();

            /**
            * [Part of AudioStreamIn interface]
            * Set the input gain for the audio driver.
            * This method is for future use
            * @return Android error code.
            */
            status_t setGain(float gain);

            /**
            * Read audio buffer in from audio driver.
            * @return Number of bytes read. */
            ssize_t read(void* buffer, ssize_t bytes);

            /**
            * [Part of AudioStreamIn interface]
            * Dump the state of the audio input device.
            * @param fd File descriptor to dump state into.
            * @param args @todo
            * @return Android error code.
            */
            status_t dump(int fd, const Vector<String16>& args);

            status_t setParameters(const String8& keyValuePairs);
            String8 getParameters(const String8& keys);

            /**
            * Setup the output device, can't be done in
            * constructor since we need to indicate success or failure and we
            * don't like exceptions.
            * @param format PCM format in Android domain.
            * @param channels The number of channels.
            * @param sampleRate Sample rate of outgoing audio data in Hz [1/s].
            * @return Android error code.
            */
            status_t setup(
                int *format, uint32_t *channels, uint32_t *sampleRate);

            /**
            * Set Mute variable so if the MIC gets muted on open.
            * @param State if true if muted false if not.
            */
            void setMute(bool state);

            /**
            * Get default MIC volume used if the MIC is muted before device is
            * open.
            * @param volume.
            */
            int getDefaultMicVol();

            /**
             * Return the amount of input frames lost in the audio driver since
             * the last call of this function
             */
            unsigned int getInputFramesLost() const;

            status_t addAudioEffect(effect_handle_t effect);
            status_t removeAudioEffect(effect_handle_t effect);

            /**
            * Change active device routing.
            * @param mode Routing mode, see AudioSystem.h
            * @param routes Bit mask containing the routes to be used,
            *               see AudioSystem.h
            * @return Android error code.
            */
            status_t changeDevice(int mode, uint32_t routes);

            void postOpenDevice(void);
            void preCloseDevice(void);

            /**
            * [Part of AudioStreamIn interface]
            */
            status_t standby();

        private:
            bool mMuted;
            int  mMuteAppVol;
            /**< If the mic is muted or not. */

            /**
            * Calculate wanted buffer size
            */
            virtual size_t calc_buffer_size() const;

            status_t standby_l();

            void refreshDeviceList(List <const char*>&v, uint32_t routes);
    }; /* class AudioStreamInANM */


    /**
    *   ANM implementation class for AHI. AudioHardwareBase inherits from
    *   AHI and defines some instance variables and implements some of the AHI
    *   functions. Here we inherit from
    *   AudioHardwareBase and also provide implementation of the other AHI
    *   functionality. We keep the functions as virtual in case this class
    *   should be extended.
    */
    class AudioHardwareANM : public AudioHardwareBase
    {
        public:
            /**
            * Ctor
            */
            AudioHardwareANM();
            /**
            * Dtor
            */
            virtual ~AudioHardwareANM();
            /*******************************************************************
            *         Beginning of virtual functions not included in
            *          AHB but in AHI.
            *******************************************************************/

            /**
            * Check to see if the audio hardware interface has been initialized.
            * return status based on values defined in include/utils/Errors.h
            * @return Android error code.
            */
            status_t initCheck();

            /**
            * Set the audio volume of a voice call.
            * Range is between 0.0 and 1.0 (0 and 8 for FAT modem)
            * @param volume Audio volume factor in range [0.0, 1.0] or [0, 8] for FAT modem.
            * @return Android error code.
            */
            status_t setVoiceVolume(float volume);

            /**
             * Set the audio volume for all audio activities other than voice
             * call. Range between 0.0 and 1.0. If any value other than NO_ERROR
             * is returned, the software mixer will emulate this capability.
            * @param volume Audio volume factor in range [0.0, 1.0].
            * @return Android error code.
             */
            status_t setMasterVolume(float volume);

            /**
            * Set mic mute status.
            * @param state True: Mic mute on False: Mic mute off.
            * @return Android error code.
            */
            status_t setMicMute(bool state);

            /**
            * Get mic mute status.
            * @param state output pointer to which current mic mute status will
            * be written.
            * @return Android error code.
            */
            status_t getMicMute(bool* state);

            /**
             * Set/Get global audio parameters.
             * The function accepts a list of parameters key value pairs in the
             * form: key1=value1;key2=value2;...
             */
            virtual status_t setParameters(const String8& keyValuePairs);
            virtual String8  getParameters(const String8& keys);

            /**
            * Creates and opens the audio hardware output stream.
            * @param format The Android domain PCM format to use.
            * @param channels Number of audio channels.
            * @param sampleRate The audio sampling rate in Hz.
            * @param status Output pointer to Android return code.
            * @return Pointer to an AudioStreamOut instance.
            */
            AudioStreamOut* openOutputStream(uint32_t devices,
                int *format,
                uint32_t *channels,
                uint32_t *sampleRate,
                status_t *status);

            /**
            * Closes the given output stream
            */
            void closeOutputStream(AudioStreamOut * out);


            /**
            * Checks for the valid parmaters for speech record.
            */
            virtual size_t getInputBufferSize(
                uint32_t sampleRate, int format, int channelCount);


            /**
            * Creates and opens the audio hardware input stream
            * @param inputSource todo.
            * @param format The Android domain PCM format to use.
            * @param channels Number of audio channels.
            * @param sampleRate The audio sampling rate in Hz.
            * @param status Output pointer to Android return code.
            * @param acoustics todo.
            * @return Pointer to an AudioStreamIn instance.
            */
            AudioStreamIn*  openInputStream(
                uint32_t devices,
                int *format,
                uint32_t *channels,
                uint32_t *sampleRate,
                status_t *status,
                AudioSystem::audio_in_acoustics acoustics);

            void closeInputStream(AudioStreamIn *in);

        protected:
            /**
            * Dump information about HW.
            * @param fd File descriptor to dump state into.
            * @param args @todo
            * @return Android error code.
            */
            status_t    dump(int fd, const Vector<String16>& args);

            /*******************************************************************
            *         End of virtual functions not included in
            *          AHB but in AHI.
            *******************************************************************/
        private:
            SortedVector<AudioStreamInANM*> mInputs;
            /**< List of opened inputs */
            SortedVector<AudioStreamOutANM*> mOutputs;
            /**< List of opened outputs */
            void *mDLHandle;
            /**< Handle to dynamic library */
            bool mMuted;
            /**< If mic is muted or not  */
            int  mMuteCsVol;
            /**< When mic is muted this contains saved volume for cscall */
            int  mMuteAppVol;
            /**< When mic is muted this contains saved volume for app devices */
            bool mBtWb;
            /**< true if wideband is supported in BT HS */
            bool mBtNrEc;
            /**< true if noise reduction and echo cancelling is supported in BT HS */
            int mAudioPolicyManager;
            /**< audio policy manager reference */
            ste_adm_modem_type_t mModemType;
            /**< modem type */
    }; /* class AudioHardwareANM */
}; /* namespace android_audio_legacy */
#endif //INCLUSION_GUARD_STE_ANM_H






