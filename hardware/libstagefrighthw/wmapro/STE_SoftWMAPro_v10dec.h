/*
 * Copyright (C) ST-Ericsson SA 2012
 *
 * Implements STE's WMAPro_v10 decoder in
 * local OMX framework.
 *
 */


#ifndef _STE_SOFT_WMAPRO_V10_DEC_H_

#define _STE_SOFT_WMAPRO_V10_DEC_H_

#include "SimpleSoftOMXComponent.h"

namespace android {

typedef int (*STE_wmapro_v10_open_t)(void *itf);
typedef void (*STE_wmapro_v10_close_t)(void *interface_fe);
typedef void (*STE_wmapro_v10_reset_t)(void *interface_fe);
typedef int (*STE_wmapro_v10_decode_frame_t)(void *interface_fe);
typedef int (*STE_wmaprov10_decode_init_t)(void *interface_fe);
typedef int (*STE_wmaprov10_check_buffer_status_t)(void *interface_fe);

struct STE_SoftWMAPro_v10dec : public SimpleSoftOMXComponent {
    STE_SoftWMAPro_v10dec(const char *name,
            const OMX_CALLBACKTYPE *callbacks,
            OMX_PTR appData,
            OMX_COMPONENTTYPE **component);
protected:

    static const int kNumBuffers = 4;

    virtual ~STE_SoftWMAPro_v10dec();

    virtual OMX_ERRORTYPE internalGetParameter(
            OMX_INDEXTYPE index, OMX_PTR params);

    virtual OMX_ERRORTYPE internalSetParameter(
            OMX_INDEXTYPE index, const OMX_PTR params);

    virtual void onQueueFilled(OMX_U32 portIndex);
    virtual void onPortFlushCompleted(OMX_U32 portIndex);
    virtual void onPortEnableCompleted(OMX_U32 portIndex, bool enabled);

private:

    size_t mInputBufferCount;
    size_t mUpsamplingFactor;
    int64_t mAnchorTimeUs;
    int64_t mNumSamplesOutput;

    int32_t mNumChannels;
    int32_t mSamplingRate;
    bool mSignalledError;

    void  *wmapro_v10_itf;
    void  *wmapro_v10_config;
    void  *wmapro_v10_info_struct;
    void  *mDLHandle;


    STE_wmapro_v10_open_t STE_wmapro_v10_open;
    STE_wmapro_v10_close_t STE_wmapro_v10_close;
    STE_wmapro_v10_reset_t STE_wmapro_v10_reset;
    STE_wmapro_v10_decode_frame_t STE_wmapro_v10_decode_frame;
    STE_wmaprov10_decode_init_t STE_wmaprov10_decode_init;
    STE_wmaprov10_check_buffer_status_t STE_wmaprov10_check_buffer_status;

    enum {
        NONE,
        AWAITING_DISABLED,
        AWAITING_ENABLED
    } mOutputPortSettingsChange;

    void initPorts();
    status_t initDecoder();
    bool isConfigured() const;

    DISALLOW_EVIL_CONSTRUCTORS(STE_SoftWMAPro_v10dec);
};

}  // namespace android

#endif  // _STE_SOFT_WMAPRO_V10_DEC_H_
