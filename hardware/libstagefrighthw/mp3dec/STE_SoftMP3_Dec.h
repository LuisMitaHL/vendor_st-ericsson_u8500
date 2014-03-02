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
 * Implements STE's MP3 decoder in
 * local OMX framework.
 *
 */


#ifndef STE_SOFT_MP3_DEC_H_

#define STE_SOFT_MP3_DEC_H_

#include "SimpleSoftOMXComponent.h"


namespace android {

typedef int (*mp3_decode_init_malloc_t)(void *itf);
typedef int (*mp3_decode_frame_t)(void *interface_fe);
typedef void (*mp3_reset_t)(void *itf);
typedef void (*mp3_close_t)(void *itf);
typedef int (*mp3_2_5_parse_header_t)(void *interface_fe);

struct STE_SoftMP3_Dec : public SimpleSoftOMXComponent {
    STE_SoftMP3_Dec(const char *name,
            const OMX_CALLBACKTYPE *callbacks,
            OMX_PTR appData,
            OMX_COMPONENTTYPE **component);
protected:

    virtual ~STE_SoftMP3_Dec();

    virtual OMX_ERRORTYPE internalGetParameter(
            OMX_INDEXTYPE index, OMX_PTR params);

    virtual OMX_ERRORTYPE internalSetParameter(
            OMX_INDEXTYPE index, const OMX_PTR params);

    virtual void onQueueFilled(OMX_U32 portIndex);
    virtual void onPortFlushCompleted(OMX_U32 portIndex);
    virtual void onPortEnableCompleted(OMX_U32 portIndex, bool enabled);
private:

    enum {
        kNumBuffers = 4,
        kOutputBufferSize = 4608 * 2,
        kPVMP3DecoderDelay = 529 // frames
    };

    void *mDecoderBuf;
    int64_t mAnchorTimeUs;
    int64_t mNumFramesOutput;

    int32_t mNumChannels;
    int32_t mSamplingRate;

    bool mConfigured;
    bool mIsFirst;
    bool mSignalledError;

	int mFirstTime;	
	#ifdef PARTIAL_FRAME_HANDLING
	bool bEOSReceived;
	void *pAudioModuleItf;  // AUDIO_MODULE_INTERFACE_T
	#else
	bool mEnable_silenceinsertion;	
	int64_t mPrevTimeStamp;
    int64_t mNewTimeStamp;
    int64_t mExpectedTimeStamp;
    int64_t mDeltaTimeStamp;
    int64_t mInitialTimeStamp;

    int mFramesTobeInserted;
    int mFrameDuration;
    int moutputFrameSize;    
	int mfirst_frame;
	#endif
    void *mp3_itf;
    void *mDLHandle;
	bool bSeekDone;

    mp3_decode_init_malloc_t mp3_decode_init_malloc;
    mp3_decode_frame_t mp3_decode_frame;
    mp3_reset_t mp3_reset;
    mp3_close_t mp3_close;
    mp3_2_5_parse_header_t mp3_2_5_parse_header;

#ifdef PARTIAL_FRAME_HANDLING
	virtual int    mp3_decode_reset( void   *pAudioModuleItf );
	virtual int    mp3_decode_getFrameReady( void   *pAudioModuleItf );
	virtual int    mp3_decode_silenceInsertion( void * pAudioModuleItf );
	virtual int    mp3_decode_processFrame( void * pAudioModuleItf );
#endif
    enum {
        NONE,
        AWAITING_DISABLED,
        AWAITING_ENABLED
    } mOutputPortSettingsChange;

    void initPorts();
    void initDecoder();

    DISALLOW_EVIL_CONSTRUCTORS(STE_SoftMP3_Dec);
};

}  // namespace android

#endif  // STE_SOFT_MP3_DEC_H_
