/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*! \file ste_hal_anm_ahi.h
*   \brief Defines for ANM.

*/

/* status_t is based on include/utils/Errors.h for reference it is included
   here */

#ifndef INCLUSION_GUARD_STE_HAL_ANM_AHI_H
#define INCLUSION_GUARD_STE_HAL_ANM_AHI_H


#include <ste_adm_client.h> /* for HW params struct */
#include <stdio.h>
#include <pthread.h>
#include <linux_utils.h>
#include "ste_hal_anm_ap.h"
#include "ste_hal_anm_util.h"
#include <cutils/str_parms.h>
#include <system/audio.h>
#include <hardware/audio.h>
#include <assert.h>

/* Key strings for setParameters() / getParameters() */
#define PARAM_KEY_EC_SUPPORTED      "ec_supported"
#define PARAM_KEY_BTWB              "bt_headset_wideband"
#define PARAM_KEY_BTNREC            "bt_headset_nrec"
#define PARAM_KEY_STREAM_STATE      "ste_stream_state"
#define PARAM_KEY_STREAM_REF        "ste_stream_ref"
#define PARAM_KEY_STREAM_STANDBY    "ste_stream_standby"
#define PARAM_KEY_AP_REF            "ste_ap_ref"
#define PARAM_KEY_TTY_MODE          "tty_mode"
#define PARAM_KEY_SINK_LATENCY      "ste_sink_latency"

#define status_t int
#define MAX_NUMBER_INPUTS 10
#define MAX_NUMBER_OUTPUTS 10


enum status_t_1 {
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
    JPARKS_BROKE_IT     = -EPIPE,
    BAD_INDEX           = -EOVERFLOW,
    NOT_ENOUGH_DATA     = -ENODATA,
    WOULD_BLOCK         = -EWOULDBLOCK,
    TIMED_OUT           = -ETIME,
    UNKNOWN_TRANSACTION = -EBADMSG,
};

/**
 * Common functionality for Input and output interfaces.
 */
struct stream_adm_base
{
    struct audio_stream_in_anm  *stream_in_anm;
    struct audio_stream_out_anm *stream_out_anm;

    int mADMConnectionID;
    /**< File descriptor holding the ADM connection ID */
    struct node *mDeviceList;
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

    /**< Called by open_devices, after a device has been opened. */
    void (*post_open_device)(struct stream_adm_base *adm);
    /**< Called just before closing a device */
    void (*pre_close_device)(struct stream_adm_base *adm);
    /**
     * Calculate wanted buffer size
     */
    size_t (*calc_buffer_size)(struct stream_adm_base *adm);
};
typedef struct stream_adm_base stream_adm_base_t;

/**
 * ANM implementation structure for the output device in AHI.
 * Inherits from audio_stream_out defined in AHI.
 */

struct audio_stream_out_anm
{
    audio_stream_out_t  stream_out;
    stream_adm_base_t   adm_base;
    struct ste_audio_hw_device *dev;

    /** dump the state of the audio input/output device */
    int (*dump)(const struct audio_stream *stream, int fd);

    /**
     * set/get audio stream parameters. The function accepts a list of
     * parameter key value pairs in the form: key1=value1;key2=value2;...
     *
     *
     * If the implementation does not accept a parameter change while
     * the output is active but the parameter is acceptable otherwise, it must
     * return -ENOSYS.
     *
     * The audio flinger will put the stream in standby and then change the
     * parameter value.
     */
    int (*set_parameters)(struct audio_stream *stream, const char *kv_pairs);

    char * (*get_parameters)(const struct audio_stream *stream,
            const char *keys);

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
    audio_mode_t mMode;
}; /* struct audio_stream_out_anm */
typedef struct audio_stream_out_anm audio_stream_out_anm_t;

/**
 * ANM implementation struct for the input device for AHI.
 * Inherits from audio_stream_in defined in AHI.
 */

 #define MAX_PREPROCESSORS 3 /* maximum one AGC + one NS + one AEC per input stream */

 struct ste_preprocessing_info
 {
    /**< Storage of registered PreProcessing algorithms */
    effect_handle_t mPreProcEffects[MAX_PREPROCESSORS];
    /**< Nb of registered effects */
    unsigned int num_preprocessors;
    unsigned int FrameSizeInBytes;
    /**< Number of frame to transfer to preprocessing effect to match with 10ms constraints*/
    unsigned int NbFramesPerProcess;
    FILE * data_beforePreProc;
    FILE * data_afterPreProc;
 };

struct audio_stream_in_anm
{

    audio_stream_in_t  stream_in;
    stream_adm_base_t  adm_base;
    struct ste_audio_hw_device *dev;

    /**
     * Dump the state of the audio input device.
     * @param fd File descriptor to dump state into.
     * @param args @todo
     * @return Android error code.
     */
    int (*dump)(const struct audio_stream *stream, int fd);
    int (*set_parameters)(struct audio_stream *stream, const char *kv_pairs);
    char * (*get_parameters)(const struct audio_stream *stream,
            const char *keys);

    bool mMuted;
    int  mMuteAppVol;
    /**< If the mic is muted or not. */
    audio_mode_t mMode;
    struct ste_preprocessing_info mPreProcInfo;
}; /* audio_stream_in_anm */

/** ANM implementation for AHI. */
struct ste_audio_hw_device
{
    struct audio_hw_device device;
    struct audio_stream_in_anm   *active_input;
    struct audio_stream_out_anm  *active_output;

    int (*set_mode)(struct audio_hw_device *dev, audio_mode_t mode);
    /**
     * Set/Get global audio parameters.
     * The function accepts a list of parameters key value pairs in the
     * form: key1=value1;key2=value2;...
     */
    int    (*set_parameters)(struct audio_hw_device *dev, const char *kv_pairs);
    char * (*get_parameters)(const struct audio_hw_device *dev, const char *keys);

    /**
     * Dump information about HW.
     * @param fd File descriptor to dump state into.
     * @param args @todo
     * @return Android error code.
     */
    int (*dump)(const struct audio_hw_device *dev, int fd);

    /**< List of opened inputs */
    node_audio_stream_t *mInputs;
    node_audio_stream_t *mOutputs;

    struct str_parms *mParms;
    /**< List of opened outputs */
    void *mDLHandle;
    /**< Handle to dynamic library */
    bool mMuted;
    /**< If mic is muted or not  */
    int  mMuteCsVol;
    /**< When mic is muted this contains saved volume for cscall */
    bool mBtWb;
    /**< true if wideband is supported in BT HS */
    bool mBtNrEc;
    /**< true if noise reduction and echo cancelling is supported in BT HS */
    int mAudioPolicyManager;
    /**< audio policy manager reference */
    ste_adm_modem_type_t mModemType;
    /**< modem type */
};
typedef struct ste_audio_hw_device ste_audio_hw_device_t;

/* struct used to pass parameter to ste_audio_stream_out_report_latency_thread in pthread_create
 *  ptr->ptr1 = kvpairs;
 *  ptr->ptr2 = struct audio_stream_out_anm
 */
struct kvpair_stream_ptr {
    void *ptr1;
    void *ptr2;
};
int  admbase_init(stream_adm_base_t *adm);
void admbase_deinit(stream_adm_base_t *adm);
int  ste_audio_stream_in_init(struct audio_stream_in_anm *stream_in);
void ste_audio_stream_in_deinit(struct audio_stream_in* stream_in);
int  ste_audio_stream_out_init(struct audio_stream_out_anm *stream);
void ste_audio_stream_out_deinit(struct audio_stream_out *stream);



/* adm base */
int admbase_close_adm_connection(struct stream_adm_base *adm);
int admbase_setup_adm_connection(struct stream_adm_base *adm);
char* admbase_devices_2_str(struct stream_adm_base *adm, char* buf, audio_devices_t dev);
ste_adm_format_t admbase_get_adm_format(struct stream_adm_base *adm);
status_t admbase_open_devices(struct stream_adm_base *adm);
status_t admbase_set_parameters(struct stream_adm_base *adm, const char *kv_pairs);
char* admbase_get_parameters(struct stream_adm_base *adm, const char *key);
status_t admbase_standby_imp(struct stream_adm_base *adm);

/* Output Stream Functions */
void* ste_audio_stream_out_open_dev_thread(void* param);
void* ste_audio_stream_out_close_dev_thread(void* param);
status_t ste_audio_stream_out_set_parameters(struct audio_stream *stream, const char *kv_pairs);
char * ste_audio_stream_out_get_parameters(const struct audio_stream *stream, const char *key);
uint32_t ste_audio_stream_out_get_sample_rate(const struct audio_stream *stream);
size_t ste_audio_stream_out_calc_buffer_size(struct stream_adm_base *adm_base);
size_t ste_audio_stream_out_get_buffer_size(const struct audio_stream *stream);
audio_channel_mask_t ste_audio_stream_out_get_channels(const struct audio_stream *stream);
audio_format_t ste_audio_stream_out_get_format(const struct audio_stream *stream);
uint32_t ste_audio_stream_out_get_latency(const struct audio_stream_out *stream);
int ste_audio_stream_out_set_volume(struct audio_stream_out *stream, float left, float right);
ssize_t ste_audio_stream_out_write(struct audio_stream_out *stream, const void* buffer, size_t bytes);
void ste_audio_stream_out_pre_close_device(struct stream_adm_base *adm);
int ste_audio_stream_out_dump(const struct audio_stream *stream, int fd);
status_t ste_audio_stream_out_change_device(const struct audio_stream *stream, int mode, uint32_t routes);
status_t ste_audio_stream_out_setup(const struct audio_stream *stream, struct audio_config *config);
status_t ste_audio_stream_out_get_render_position(const struct audio_stream_out *stream, uint32_t *dsp_frames);
int ste_audio_stream_out_get_next_write_timestamp(const struct audio_stream_out *stream, int64_t *timestamp);
void ste_audio_stream_out_post_open_device(struct stream_adm_base *adm);
int ste_audio_stream_out_standby(struct audio_stream *stream);
status_t ste_audio_stream_out_standby_l(struct audio_stream *stream);
void* ste_audio_stream_out_report_latency_thread(void *parms);
void ste_audio_stream_out_notify_sink_latency(const struct audio_stream *stream, audio_io_handle_t output);
void ste_audio_stream_out_refresh_device_list(struct node **v, uint32_t routes, struct audio_stream_out_anm* stream_out);
int ste_audio_stream_out_set_sample_rate(struct audio_stream *stream, uint32_t rate);
int ste_audio_stream_out_set_format(struct audio_stream *stream, audio_format_t format);
int ste_audio_stream_out_add_audio_effect(const struct audio_stream *stream, effect_handle_t effect);
int ste_audio_stream_out_remove_audio_effect(const struct audio_stream *stream, effect_handle_t effect);

/* Input Stream Functions */
void ste_audio_stream_in_refresh_device_list(struct node **v, uint32_t routes, struct audio_stream_in_anm* stream_in);
status_t ste_audio_stream_in_setup(const struct audio_stream *stream, struct audio_config *config);
void ste_audio_stream_in_set_mute(const struct audio_stream *stream, bool state);
int ste_audio_stream_in_get_default_mic_vol(struct audio_stream_in_anm *stream_in);
status_t ste_audio_stream_in_set_parameters(struct audio_stream *stream, const char *kv_pairs);
char* ste_audio_stream_in_get_parameters(const struct audio_stream *stream, const char *keys);
uint32_t ste_audio_stream_in_get_sample_rate(const struct audio_stream *stream);
size_t ste_audio_stream_in_calc_buffer_size(struct stream_adm_base *adm_base);
size_t ste_audio_stream_in_get_buffer_size(const struct audio_stream *stream);
audio_channel_mask_t ste_audio_stream_in_get_channels(const struct audio_stream *stream);
audio_format_t ste_audio_stream_in_get_format(const struct audio_stream *stream);
int ste_audio_stream_in_set_gain(struct audio_stream_in *stream, float gain);
ssize_t ste_audio_stream_in_read(struct audio_stream_in *stream, void* buffer, size_t bytes);
int ste_audio_stream_in_dump(const struct audio_stream *stream, int fd);
void ste_audio_stream_in_post_open_device(struct stream_adm_base *adm);
void ste_audio_stream_in_pre_close_device(struct stream_adm_base *adm);
uint32_t ste_audio_stream_in_get_input_frames_lost(struct audio_stream_in *stream);
int ste_audio_stream_in_add_audio_effect(const struct audio_stream *stream, effect_handle_t effect);
int ste_audio_stream_in_remove_audio_effect(const struct audio_stream *stream, effect_handle_t effect);
status_t ste_audio_stream_in_change_device(const struct audio_stream *stream, int mode, uint32_t routes);
int ste_audio_stream_in_standby(struct audio_stream *stream);
status_t ste_audio_stream_in_standby_l(struct audio_stream *stream);
int ste_audio_stream_in_set_sample_rate(struct audio_stream *stream, uint32_t rate);
int ste_audio_stream_in_set_format(struct audio_stream *stream, audio_format_t format);
audio_devices_t ste_audio_stream_in_get_device(const struct audio_stream *stream);
int ste_audio_stream_in_set_device(struct audio_stream *stream, audio_devices_t device);


/* AHI Implementation*/
void ste_audio_hardware_init(struct  ste_audio_hw_device *ahi);

#endif //INCLUSION_GUARD_STE_HAL_ANM_AHI_H
