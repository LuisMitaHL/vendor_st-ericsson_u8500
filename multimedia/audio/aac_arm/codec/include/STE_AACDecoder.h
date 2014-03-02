/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef STE_AACDECODER_H_

#define STE_AACDECODER_H_

#include <media/stagefright/MediaSource.h>

typedef int  (*STE_eaacPlus_open_t)(void *itf);
typedef int  (*STE_aac_getAudioSpecificConfig_t)(void *hBs, void *p_global, unsigned short ASC_header_size);
typedef void (*STE_eaacPlus_close_t)(void *interface_fe);
typedef void (*STE_eaacPlus_reset_t)(void *interface_fe);
typedef int  (*STE_eaacPlus_decode_frame_t)(void *interface_fe);
typedef void (*STE_aac_parse_t)(void *interface_fe);
typedef void (*STE_aac_setoutput_mute_t)(void *interface_fe);

namespace android {

struct MediaBufferGroup;
struct MetaData;

struct STE_AACDecoder : public MediaSource {
    STE_AACDecoder(const sp<MediaSource> &source);

    virtual status_t start(MetaData *params);
    virtual status_t stop();

    virtual sp<MetaData> getFormat();

    virtual status_t read(
            MediaBuffer **buffer, const ReadOptions *options);

protected:
    virtual ~STE_AACDecoder();

private:
    sp<MetaData>    mMeta;
    sp<MediaSource> mSource;
    bool mStarted;

    MediaBufferGroup *mBufferGroup;

    void *mDecoderBuf;
    int64_t mAnchorTimeUs;
    int64_t mNumSamplesOutput;
    status_t mInitCheck;
    int64_t  mNumDecodedBuffers;
    int32_t  mUpsamplingFactor;

    MediaBuffer *mInputBuffer;

    status_t initCheck();


    STE_eaacPlus_open_t                 STE_eaacPlus_open;
    STE_aac_getAudioSpecificConfig_t    STE_aac_getAudioSpecificConfig;
    STE_eaacPlus_close_t                STE_eaacPlus_close;
    STE_eaacPlus_reset_t                STE_eaacPlus_reset;
    STE_eaacPlus_decode_frame_t         STE_eaacPlus_decode_frame;
	STE_aac_parse_t                     STE_aac_parse;
	STE_aac_setoutput_mute_t            STE_aac_setoutput_mute;

	int64_t mPrevTimeStamp;
    int64_t mNewTimeStamp;
    int64_t mExpectedTimeStamp;
    int64_t mDeltaTimeStamp;
    int64_t mInitialTimeStamp;

    int mFramesTobeInserted;
    int mFrameDuration;
    int moutputFrameSize;
    int mfirst_frame;	
	bool mEnable_silenceinsertion;
	
    void  *aac_itf;
    void  *aac_config;
    void  *aac_info_struct;
    void  *mDLHandle;

    STE_AACDecoder(const STE_AACDecoder &);
    STE_AACDecoder &operator=(const STE_AACDecoder &);
};

}// namespace android

#endif  // STE_AACDECODER_H_
