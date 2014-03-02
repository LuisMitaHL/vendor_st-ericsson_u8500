/*
 * Copyright (C) 2011 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


/*
 * Copyright (C) ST-Ericsson SA 2011
 *
 * Implements STE's AAC decoder in
 * local OMX framework.
 *
 */


#ifndef _STE_SOFT_AAC_DEC_H_

#define _STE_SOFT_AAC_DEC_H_

#include "SimpleSoftOMXComponent.h"

namespace android {

typedef int (*STE_eaacPlus_open_t)(void *itf);
typedef int (*STE_aac_getAudioSpecificConfig_t)(void *hBs, void *p_global, unsigned short ASC_header_size);
typedef void (*STE_eaacPlus_close_t)(void *interface_fe);
typedef void (*STE_eaacPlus_reset_t)(void *interface_fe);
typedef int (*STE_eaacPlus_decode_frame_t)(void *interface_fe);
typedef void (*STE_aac_parse_t)(void *interface_fe);
typedef void (*STE_aac_setoutput_mute_t)(void *interface_fe);

struct STE_SoftAAC_Dec : public SimpleSoftOMXComponent {
    STE_SoftAAC_Dec(const char *name,
            const OMX_CALLBACKTYPE *callbacks,
            OMX_PTR appData,
            OMX_COMPONENTTYPE **component);
protected:

    virtual ~STE_SoftAAC_Dec();

    virtual OMX_ERRORTYPE internalGetParameter(
            OMX_INDEXTYPE index, OMX_PTR params);

    virtual OMX_ERRORTYPE internalSetParameter(
            OMX_INDEXTYPE index, const OMX_PTR params);

    virtual void onQueueFilled(OMX_U32 portIndex);
    virtual void onPortFlushCompleted(OMX_U32 portIndex);
    virtual void onPortEnableCompleted(OMX_U32 portIndex, bool enabled);
private:

    enum {
        kNumBuffers = 4
    };

    bool mIsADTS;
    size_t mInputBufferCount;
    size_t mUpsamplingFactor;
    int64_t mAnchorTimeUs;
    int64_t mNumSamplesOutput;

    bool mSignalledError;

    int64_t mPrevTimeStamp;
    int64_t mNewTimeStamp;
    int64_t mExpectedTimeStamp;
    int64_t mDeltaTimeStamp;
    int64_t mInitialTimeStamp;

    int reset_done;
    int mFramesTobeInserted;
    int mFrameDuration;
    int moutputFrameSize;
    int mfirst_frame;
    bool mEnable_silenceinsertion;

    void  *aac_itf;
    void  *aac_config;
    void  *aac_info_struct;
    void  *mDLHandle;

    STE_eaacPlus_open_t STE_eaacPlus_open;
    STE_aac_getAudioSpecificConfig_t STE_aac_getAudioSpecificConfig;
    STE_eaacPlus_close_t STE_eaacPlus_close;
    STE_eaacPlus_reset_t STE_eaacPlus_reset;
    STE_eaacPlus_decode_frame_t STE_eaacPlus_decode_frame;
    STE_aac_parse_t STE_aac_parse;
    STE_aac_setoutput_mute_t STE_aac_setoutput_mute;

    enum {
        NONE,
        AWAITING_DISABLED,
        AWAITING_ENABLED
    } mOutputPortSettingsChange;

    void initPorts();
    status_t initDecoder();
    bool isConfigured() const;
    void maybeConfigureDownmix() const;

    DISALLOW_EVIL_CONSTRUCTORS(STE_SoftAAC_Dec);
};

}  // namespace android

#endif  // _STE_SOFT_AAC_DEC_H_
