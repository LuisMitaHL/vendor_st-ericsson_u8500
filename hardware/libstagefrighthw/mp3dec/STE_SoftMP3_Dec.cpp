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
 * Functions modified:
 * STE_SoftMP3_Dec
 * ~STE_SoftMP3_Dec
 * initDecoder
 * mp3_decode_reset
 * mp3_decode_getFrameReady
 * mp3_decode_silenceInsertion
 * mp3_decode_processFrame
 * onQueueFilled
 * onPortFlushCompleted
 * createSoftOMXComponent
 */

//#define LOG_NDEBUG 0
#define LOG_TAG "STE_SoftMP3_Dec"
#include <utils/Log.h>

#include "STE_SoftMP3_Dec.h"
#include "STE_Audio_API.h"

#include "mp3.h"
#include "defines.h"
#define ONE_MILI_SEC 1000  // this is 1000 micro second

#include <media/stagefright/foundation/ADebug.h>
#include <media/stagefright/MediaDefs.h>

#include <dlfcn.h>

#define STE_MP3_DECODER_LIBRARY "/system/lib/libstagefright_ste_mp3decode.so"

namespace android {

template<class T>
static void InitOMXParams(T *params) {
    params->nSize = sizeof(T);
    params->nVersion.s.nVersionMajor = 1;
    params->nVersion.s.nVersionMinor = 0;
    params->nVersion.s.nRevision = 0;
    params->nVersion.s.nStep = 0;
}

STE_SoftMP3_Dec::STE_SoftMP3_Dec(
        const char *name,
        const OMX_CALLBACKTYPE *callbacks,
        OMX_PTR appData,
        OMX_COMPONENTTYPE **component)
    : SimpleSoftOMXComponent(name, callbacks, appData, component),
      mDecoderBuf(NULL),
      mAnchorTimeUs(0),
      mNumFramesOutput(0),
      mNumChannels(2),
      mSamplingRate(44100),
      mSignalledError(false),
      mFirstTime(1),
#ifdef PARTIAL_FRAME_HANDLING
      bEOSReceived(false),
      pAudioModuleItf(new AUDIO_MODULE_INTERFACE_T),
#else
      mEnable_silenceinsertion(false),
      mPrevTimeStamp(0LL),
      mNewTimeStamp(0LL),
      mExpectedTimeStamp(0LL),
      mDeltaTimeStamp(0LL),
      mInitialTimeStamp(0LL),
      mFramesTobeInserted(0),
      mFrameDuration(0),
      moutputFrameSize(0),
      mfirst_frame(0),
#endif
      mp3_itf(new CODEC_INTERFACE_T),
      mDLHandle(NULL),
      bSeekDone(false),
      mOutputPortSettingsChange(NONE) {

    mDLHandle = dlopen(STE_MP3_DECODER_LIBRARY, RTLD_NOW);

    if (mDLHandle == NULL) {
        ALOGE("Could not open the libstagefright_ste_mp3decode library.\n");
        return;
    }

    mp3_decode_init_malloc = (mp3_decode_init_malloc_t)dlsym(mDLHandle, "mp3_decode_init_malloc");
    mp3_close              = (mp3_close_t)dlsym(mDLHandle, "mp3_close");
    mp3_reset              = (mp3_reset_t)dlsym(mDLHandle, "mp3_reset");
    mp3_decode_frame       = (mp3_decode_frame_t)dlsym(mDLHandle, "mp3_decode_frame");
    mp3_2_5_parse_header   = (mp3_2_5_parse_header_t)dlsym(mDLHandle, "mp3_2_5_parse_header");

    initPorts();
    initDecoder();
    ALOGI("Successfully allocated ST-Ericsson '%s' decoder through the " \
         "SoftOMXPlugin interface", this->name());
}

STE_SoftMP3_Dec::~STE_SoftMP3_Dec() {

    mp3_close((CODEC_INTERFACE_T *)mp3_itf);
    delete (CODEC_INTERFACE_T *)mp3_itf;
    mp3_itf = NULL;

    if (mDLHandle != NULL) {
        dlclose(mDLHandle);
    }

#ifdef PARTIAL_FRAME_HANDLING
    delete (AUDIO_MODULE_INTERFACE_T *)pAudioModuleItf;
    pAudioModuleItf = NULL;
#endif

    mp3_decode_init_malloc = NULL;
    mp3_close              = NULL;
    mp3_reset              = NULL;
    mp3_decode_frame       = NULL;
    mp3_2_5_parse_header   = NULL;

    ALOGI("Deallocated ST-Ericsson '%s' decoder through the SoftOMXPlugin " \
         "interface", this->name());
}

void STE_SoftMP3_Dec::initPorts() {
    OMX_PARAM_PORTDEFINITIONTYPE def;
    InitOMXParams(&def);

    def.nPortIndex = 0;
    def.eDir = OMX_DirInput;
    def.nBufferCountMin = kNumBuffers;
    def.nBufferCountActual = def.nBufferCountMin;
    def.nBufferSize = 8192;
    def.bEnabled = OMX_TRUE;
    def.bPopulated = OMX_FALSE;
    def.eDomain = OMX_PortDomainAudio;
    def.bBuffersContiguous = OMX_FALSE;
    def.nBufferAlignment = 1;

    def.format.audio.cMIMEType =
            const_cast<char *>(MEDIA_MIMETYPE_AUDIO_MPEG);

    def.format.audio.pNativeRender = NULL;
    def.format.audio.bFlagErrorConcealment = OMX_FALSE;
    def.format.audio.eEncoding = OMX_AUDIO_CodingMP3;

    addPort(def);

    def.nPortIndex = 1;
    def.eDir = OMX_DirOutput;
    def.nBufferCountMin = kNumBuffers;
    def.nBufferCountActual = def.nBufferCountMin;
    def.nBufferSize = kOutputBufferSize;
    def.bEnabled = OMX_TRUE;
    def.bPopulated = OMX_FALSE;
    def.eDomain = OMX_PortDomainAudio;
    def.bBuffersContiguous = OMX_FALSE;
    def.nBufferAlignment = 2;

    def.format.audio.cMIMEType = const_cast<char *>("audio/raw");
    def.format.audio.pNativeRender = NULL;
    def.format.audio.bFlagErrorConcealment = OMX_FALSE;
    def.format.audio.eEncoding = OMX_AUDIO_CodingPCM;

    addPort(def);
}

void STE_SoftMP3_Dec::initDecoder() {
    CODEC_INIT_T memory_assigned_message;

    memory_assigned_message = (CODEC_INIT_T)mp3_decode_init_malloc(mp3_itf);
    if (memory_assigned_message != INIT_OK) {
        ALOGE("Malloc failed");
        return;
    }

    mFirstTime = 1;
    mIsFirst = true;
#ifdef PARTIAL_FRAME_HANDLING
    mp3_decode_reset(pAudioModuleItf);

#else
    mEnable_silenceinsertion = false;
    if (mEnable_silenceinsertion == true) {
        mPrevTimeStamp = 0;
        mNewTimeStamp = 0;
        mFrameDuration = 0;
        mExpectedTimeStamp = 0;
        mDeltaTimeStamp = 0;
        mFramesTobeInserted = 0;
        moutputFrameSize = 0;
        mfirst_frame = 0;
        mInitialTimeStamp = 0;
    }
#endif

    ALOGV("MP3 STARTED\n");
    // to be copeid inside the
    ((CODEC_INTERFACE_T *)mp3_itf)->stream_struct.bits_struct.word_size = 32;
    ((CODEC_INTERFACE_T *)mp3_itf)->codec_state.mode = MP3;
    ((CODEC_INTERFACE_T *)mp3_itf)->sample_struct.samples_interleaved = 1;
#ifdef PARTIAL_FRAME_HANDLING
    ((CODEC_INTERFACE_T *)mp3_itf)->codec_state.eof = 0;
#endif
}

OMX_ERRORTYPE STE_SoftMP3_Dec::internalGetParameter(
        OMX_INDEXTYPE index, OMX_PTR params) {
    switch (index) {
        case OMX_IndexParamAudioPcm:
        {
            OMX_AUDIO_PARAM_PCMMODETYPE *pcmParams =
                    (OMX_AUDIO_PARAM_PCMMODETYPE *)params;

            if (pcmParams->nPortIndex > 1) {
                return OMX_ErrorUndefined;
            }

            pcmParams->eNumData = OMX_NumericalDataSigned;
            pcmParams->eEndian = OMX_EndianBig;
            pcmParams->bInterleaved = OMX_TRUE;
            pcmParams->nBitPerSample = 16;
            pcmParams->ePCMMode = OMX_AUDIO_PCMModeLinear;
            pcmParams->eChannelMapping[0] = OMX_AUDIO_ChannelLF;
            pcmParams->eChannelMapping[1] = OMX_AUDIO_ChannelRF;

            pcmParams->nChannels = mNumChannels;
            pcmParams->nSamplingRate = mSamplingRate;

            return OMX_ErrorNone;
        }

        default:
            return SimpleSoftOMXComponent::internalGetParameter(index, params);
    }
}

OMX_ERRORTYPE STE_SoftMP3_Dec::internalSetParameter(
        OMX_INDEXTYPE index, const OMX_PTR params) {
    switch (index) {
        case OMX_IndexParamStandardComponentRole:
        {
            const OMX_PARAM_COMPONENTROLETYPE *roleParams =
                    (const OMX_PARAM_COMPONENTROLETYPE *)params;

            if (strncmp((const char *)roleParams->cRole,
                        "audio_decoder.mp3",
                        OMX_MAX_STRINGNAME_SIZE - 1)) {
                return OMX_ErrorUndefined;
            }

            return OMX_ErrorNone;
        }

        default:
            return SimpleSoftOMXComponent::internalSetParameter(index, params);
    }
}

/* The new wrappers to implement silence insertion */
#ifdef PARTIAL_FRAME_HANDLING
int STE_SoftMP3_Dec::mp3_decode_reset(void   *pAudioModuleItf) {

    RETURN_STATUS_LEVEL_T errorStatus = RETURN_STATUS_OK;
    AUDIO_MODULE_INTERFACE_T *pAudioModuleInterface = (AUDIO_MODULE_INTERFACE_T *)pAudioModuleItf;

    ALOGV("MP3 Decoder Initialized \n");

    memset((char *)pAudioModuleInterface, 0, sizeof(AUDIO_MODULE_INTERFACE_T));
    pAudioModuleInterface->pAudioCodecInterface = mp3_itf;
    pAudioModuleInterface->audioPlayerCommand.enableSilenceInsertion = false;
    pAudioModuleInterface->audioSilenceInfo.activateSilenceInsertion = false;
    pAudioModuleInterface->audioFrameInfo.mFrameReady = false;
    pAudioModuleInterface->audioFrameInfo.mCompleteFrame = false;
    pAudioModuleInterface->audioFrameInfo.mFirstFrame = false;

    return errorStatus;
}

int STE_SoftMP3_Dec::mp3_decode_getFrameReady(void   *pAudioModuleItf) {

    MP3_DEC_ERROR_ENUM_T error_check;
    int layer_info, version, sample_rate, ch_mode, samples = 0, bitrate, framelen;
    long long mThisFrameTimeStamp, mExpectedTimeStamp = 0;
    long mFrameDuration = 0;
    long long mInputBufferTimeStamp = 0;
    unsigned int mInputSpecificFlag = 0;
    unsigned int nBufferStatusFlag = 0;
    bool bSyncLost;

    AUDIO_MODULE_INTERFACE_T *pAudioModuleInterface = (AUDIO_MODULE_INTERFACE_T *)pAudioModuleItf;
    AUDIO_INPUT_STRUCT_T     *pInputAudioData      = &pAudioModuleInterface->inputDataStruct;
    AUDIO_OUTPUT_STRUCT_T    *pOutputAudioData     = &pAudioModuleInterface->outputDataStruct;
    AUDIO_INPUT_CHUNK_INFO_T *pAudioChunkInfo      = &pAudioModuleInterface->inputAudioChunkInfo;
    AUDIO_TIMESTAMP_INFO_T   *pAudioTimeStampInfo  = &pAudioModuleInterface->audioTimeStampInfo;
    AUDIO_FRAME_INFO_T       *pAudioFrameInfo      = &pAudioModuleInterface->audioFrameInfo;
    AUDIO_SILENCE_INFO_T     *pAudioSilenceInfo    = &pAudioModuleInterface->audioSilenceInfo;
    AUDIO_PLAYER_COMMAND_T   *pAudioPlayerCommand  = &pAudioModuleInterface->audioPlayerCommand;
    CODEC_INTERFACE_T        *interface_fe         = (CODEC_INTERFACE_T *)pAudioModuleInterface->pAudioCodecInterface;

    MP3_DECODER_STRUCT_T *mp3_decoder_struct;
    MP3_DECODER_INFO_STRUCT_T *mp3_param_struct = (MP3_DECODER_INFO_STRUCT_T *)interface_fe->codec_info_local_struct;
    mp3_decoder_struct = (MP3_DECODER_STRUCT_T *)interface_fe->codec_local_struct;

    mInputSpecificFlag = pInputAudioData->nInputSpecificFlags;
    mInputBufferTimeStamp = pInputAudioData->nTimeStamp;
    nBufferStatusFlag = pInputAudioData->nBufferStatusFlags;

    ALOGV("mInputBufferTimeStamp : %lld\n", mInputBufferTimeStamp);

    if (mInputSpecificFlag & STE_AUDIO_INPUT_AFTER_SEEK) {
        pAudioTimeStampInfo->mPrevFrameOutTimeStamp = mInputBufferTimeStamp;
        pAudioTimeStampInfo->mCurrFrameOutTimeStamp = mInputBufferTimeStamp;
        pAudioTimeStampInfo->mNextFrameOutTimeStamp = mInputBufferTimeStamp;
        pAudioChunkInfo->mNextChunkTimeStamp = mInputBufferTimeStamp;
    }

    /* This check must be done after the above test */
    if (mInputSpecificFlag & STE_AUDIO_INPUT_FIRST_BUFFER) {
        ALOGV(" STE_AUDIO_INPUT_FIRST_BUFFER ENABLED\n");
        pAudioTimeStampInfo->mPrevFrameOutTimeStamp = 0;
        pAudioTimeStampInfo->mCurrFrameOutTimeStamp = 0;
        pAudioTimeStampInfo->mNextFrameOutTimeStamp = 0;
        pAudioFrameInfo->mFirstFrame = true;
        pAudioChunkInfo->mNextChunkTimeStamp = mInputBufferTimeStamp;
    }

    interface_fe->codec_state.eof = 0;
    if (pInputAudioData->nFrameworkSpecificFlags & STE_AUDIO_BUFFERFLAG_EOS) {
        interface_fe->codec_state.eof = 1;
    }

    if (mInputSpecificFlag & STE_AUDIO_INPUT_GET_NEW_FRAME) {
        pAudioChunkInfo->mFirstChunkTimeStamp = pAudioChunkInfo->mNextChunkTimeStamp;
        pAudioChunkInfo->mChunksForThisFrame = 0;
    }

    if (mInputSpecificFlag & STE_AUDIO_INPUT_NEW_BUFFER) {
        pAudioChunkInfo->mFramesInThisChunk = 0;
    }

    interface_fe->stream_struct.bits_struct.buf_add = (unsigned int *)(pInputAudioData->pBuffer + pInputAudioData->nOffset);
    interface_fe->stream_struct.real_size_frame_in_bit = pInputAudioData->nFilledLen;     // real_size_frame_in_bit is misnomer, it is actually real frame size in bytes

    error_check = (MP3_DEC_ERROR_ENUM_T)mp3_2_5_parse_header(interface_fe);

    ch_mode =  interface_fe->sample_struct.chans_nb;
    sample_rate = interface_fe->sample_struct.sample_freq;
    samples = interface_fe->sample_struct.block_len;
    bitrate = mp3_param_struct->bitrate;
    framelen = mp3_decoder_struct->mp3_Head.bits_in_frame;
    bSyncLost = interface_fe->codec_state.sync_lost;

    pAudioChunkInfo->mBytesConsumed = interface_fe->stream_struct.real_size_frame_in_bit;

    if (pAudioChunkInfo->mBytesConsumed > pInputAudioData->nFilledLen) {
        /* Normally it should not be here */
        pAudioChunkInfo->mBytesConsumed = pInputAudioData->nFilledLen;
    }

    if (error_check == MP3_NO_ENOUGH_MAIN_DATA_ERROR) {
        ALOGV("Partial Frame \n");
        pAudioFrameInfo->mFrameReady = false;
        pAudioFrameInfo->mCompleteFrame = false;
        pAudioChunkInfo->mChunksForThisFrame++;
        return MP3_NO_ENOUGH_MAIN_DATA_ERROR;
    } else if (error_check == MP3_NEXT_SYNC_NOT_FOUND) {
        ALOGV("Complete Frame \n");
        pAudioFrameInfo->mFrameReady = false;
        pAudioFrameInfo->mCompleteFrame = true;
        pAudioChunkInfo->mChunksForThisFrame++;

        ALOGV("mInputBufferTimeStamp = %lld\n", mInputBufferTimeStamp);
        ALOGV("pAudioChunkInfo->mFirstChunkTimeStamp = %lld\n", pAudioChunkInfo->mFirstChunkTimeStamp);

        if (mInputBufferTimeStamp > pAudioChunkInfo->mFirstChunkTimeStamp) {
            pAudioChunkInfo->mFirstChunkTimeStamp = mInputBufferTimeStamp;
        }

        return MP3_NEXT_SYNC_NOT_FOUND;
    } else {
        ALOGV("Ready Frame \n");
        pAudioFrameInfo->mFrameReady = true;
        pAudioChunkInfo->mFramesInThisChunk++;
        pAudioChunkInfo->mLastChunkTimeStamp = mInputBufferTimeStamp;
    }

    mFrameDuration = ((long long)framelen * 1000000ll) / bitrate;

    mExpectedTimeStamp = pAudioTimeStampInfo->mNextFrameOutTimeStamp;
    mThisFrameTimeStamp = mExpectedTimeStamp;

    if (pAudioPlayerCommand->enableSilenceInsertion == true) {

        if (pAudioFrameInfo->mFirstFrame) {
            pAudioChunkInfo->mLastChunkTimeStamp = pAudioChunkInfo->mFirstChunkTimeStamp;
            bSyncLost = true;
        }

        if (pAudioFrameInfo->mCompleteFrame) {
            mThisFrameTimeStamp = pAudioChunkInfo->mFirstChunkTimeStamp;
        }

        if (bSyncLost) {
            if ((pAudioChunkInfo->mLastChunkTimeStamp - pAudioChunkInfo->mFirstChunkTimeStamp) < (mFrameDuration + ONE_MILI_SEC)) {
                ALOGV("bSyncLost First Check1 \n");
                mInputBufferTimeStamp = pAudioChunkInfo->mFirstChunkTimeStamp;
            } else {
                ALOGV("bSyncLost First Check2 \n");
                mInputBufferTimeStamp = pAudioChunkInfo->mLastChunkTimeStamp;
            }

            if ((mInputBufferTimeStamp - mExpectedTimeStamp)  > (mFrameDuration - ONE_MILI_SEC)) {
                ALOGV("bSyncLost Second Check \n");
                mThisFrameTimeStamp = mInputBufferTimeStamp;
            }
        }
    }

    pAudioFrameInfo->frameType = MPEG1_LAYER3;
    pAudioFrameInfo->nBitRate = bitrate;
    pAudioFrameInfo->nBitsPerSampleRef = 16;
    pAudioFrameInfo->nChannels = ch_mode;
    pAudioFrameInfo->mFrameLength = framelen;
    pAudioFrameInfo->nSamplingFreq = sample_rate;
    pAudioFrameInfo->mFrameDuration =  mFrameDuration;
    pAudioFrameInfo->nThisFrameOutTimeStamp = mThisFrameTimeStamp;

    pAudioChunkInfo->mNextChunkTimeStamp = pAudioFrameInfo->nThisFrameOutTimeStamp + mFrameDuration;

    pAudioSilenceInfo->mChannels = ch_mode;
    pAudioSilenceInfo->mSampleSize = 16;
    pAudioSilenceInfo->mSilenceFrameSize = samples;
    pAudioSilenceInfo->mSilenceFrameDuration = mFrameDuration;

    ALOGV("mFrameLength : %d\n", pAudioFrameInfo->mFrameLength);
    ALOGV("mFrameDuration : %d\n", pAudioFrameInfo->mFrameDuration);
    ALOGV("nBitRate: %d\n", pAudioFrameInfo->nBitRate);
    ALOGV("nSamplingFreq : %d\n", pAudioFrameInfo->nSamplingFreq);
    ALOGV("nChannels : %d\n", pAudioFrameInfo->nChannels);
    ALOGV("mSilenceFrameSize : %d\n", pAudioSilenceInfo->mSilenceFrameSize);

    ALOGV("nThisFrameOutTimeStamp : %lld\n", pAudioFrameInfo->nThisFrameOutTimeStamp);
    ALOGV("pAudioChunkInfo->mNextChunkTimeStamp : %lld\n", pAudioChunkInfo->mNextChunkTimeStamp);

    return error_check;
}       // End of function STE_Audio_Module_getFrameReady

int STE_SoftMP3_Dec::mp3_decode_silenceInsertion(void *pAudioModuleItf) {

    long long mDeltaTimeStamp = 0;

    AUDIO_MODULE_INTERFACE_T *pAudioModuleInterface = (AUDIO_MODULE_INTERFACE_T *)pAudioModuleItf;
    AUDIO_INPUT_STRUCT_T     *pInputAudioData      = &pAudioModuleInterface->inputDataStruct;
    AUDIO_OUTPUT_STRUCT_T    *pOutputAudioData     = &pAudioModuleInterface->outputDataStruct;
    AUDIO_INPUT_CHUNK_INFO_T *pAudioChunkInfo      = &pAudioModuleInterface->inputAudioChunkInfo;
    AUDIO_TIMESTAMP_INFO_T   *pAudioTimeStampInfo  = &pAudioModuleInterface->audioTimeStampInfo;
    AUDIO_FRAME_INFO_T       *pAudioFrameInfo      = &pAudioModuleInterface->audioFrameInfo;
    AUDIO_SILENCE_INFO_T     *pAudioSilenceInfo    = &pAudioModuleInterface->audioSilenceInfo;
    AUDIO_PLAYER_COMMAND_T   *pAudioPlayerCommand  = &pAudioModuleInterface->audioPlayerCommand;

    if (pAudioSilenceInfo->activateSilenceInsertion == false) {

        pAudioSilenceInfo->mFramesTobeInserted = 0;
        mDeltaTimeStamp = pAudioFrameInfo->nThisFrameOutTimeStamp - pAudioTimeStampInfo->mNextFrameOutTimeStamp;

        /* Compute no of frames to be inserted */
        if (mDeltaTimeStamp > (2 * ONE_MILI_SEC)) {  // Compute above 2 mili sec difference
            pAudioSilenceInfo->mFramesTobeInserted = (int)((long long)(mDeltaTimeStamp) / pAudioSilenceInfo->mSilenceFrameDuration);
        }

        ALOGV("Silence mFramesTobeInserted = %d\n", pAudioSilenceInfo->mFramesTobeInserted);
    }

    if (pAudioSilenceInfo->mFramesTobeInserted > 0) {
        pAudioSilenceInfo->activateSilenceInsertion = true;
    } else {
        pAudioSilenceInfo->activateSilenceInsertion = false;
    }

    if (pAudioSilenceInfo->activateSilenceInsertion == true) {

        pOutputAudioData->nOffset = 0;
        pOutputAudioData->nFilledLen = pAudioSilenceInfo->mSilenceFrameSize * pAudioSilenceInfo->mChannels * (pAudioSilenceInfo->mSampleSize >> 3);
        pOutputAudioData->nTimeStamp = pAudioTimeStampInfo->mNextFrameOutTimeStamp;

        memset(pOutputAudioData->pBuffer, 0, pOutputAudioData->nFilledLen);     // 16 bit sample size

        pAudioTimeStampInfo->mPrevFrameOutTimeStamp = pAudioTimeStampInfo->mCurrFrameOutTimeStamp;
        pAudioTimeStampInfo->mCurrFrameOutTimeStamp = pOutputAudioData->nTimeStamp;
        pAudioTimeStampInfo->mNextFrameOutTimeStamp = pAudioTimeStampInfo->mCurrFrameOutTimeStamp + pAudioSilenceInfo->mSilenceFrameDuration;

        pAudioSilenceInfo->mTotalSilenceFrames++;
        pAudioSilenceInfo->mFramesTobeInserted--;

    }

    return RETURN_STATUS_OK;
}

int STE_SoftMP3_Dec::mp3_decode_processFrame(void *pAudioModuleItf) {

    RETURN_STATUS_LEVEL_T errorStatus = RETURN_STATUS_OK;

    AUDIO_MODULE_INTERFACE_T *pAudioModuleInterface = (AUDIO_MODULE_INTERFACE_T *)pAudioModuleItf;
    AUDIO_INPUT_STRUCT_T     *pInputAudioData      = &pAudioModuleInterface->inputDataStruct;
    AUDIO_OUTPUT_STRUCT_T    *pOutputAudioData     = &pAudioModuleInterface->outputDataStruct;
    AUDIO_INPUT_CHUNK_INFO_T *pAudioChunkInfo      = &pAudioModuleInterface->inputAudioChunkInfo;
    AUDIO_TIMESTAMP_INFO_T   *pAudioTimeStampInfo  = &pAudioModuleInterface->audioTimeStampInfo;
    AUDIO_FRAME_INFO_T       *pAudioFrameInfo      = &pAudioModuleInterface->audioFrameInfo;
    AUDIO_SILENCE_INFO_T     *pAudioSilenceInfo    = &pAudioModuleInterface->audioSilenceInfo;
    AUDIO_PLAYER_COMMAND_T   *pAudioPlayerCommand  = &pAudioModuleInterface->audioPlayerCommand;
    CODEC_INTERFACE_T        *interface_fe         = (CODEC_INTERFACE_T *)pAudioModuleInterface->pAudioCodecInterface;

    long long mDeltaTimeStamp = 0;
    unsigned int nBufferStatusFlag = pInputAudioData->nBufferStatusFlags;

    if (pAudioFrameInfo->mFrameReady == false) {
        ALOGV("mp3_decode_processFrame: mFrameReady is False\n");
        return RETURN_STATUS_ERROR;
    }

    interface_fe->sample_struct.buf_add = (Float *)pOutputAudioData->pBuffer;
    interface_fe->codec_state.remaining_blocks  = 0;

    if (pAudioPlayerCommand->enableSilenceInsertion) {

        ALOGV("Call TO Silence Insertion \n");

        errorStatus = (RETURN_STATUS_LEVEL_T)mp3_decode_silenceInsertion((void *)pAudioModuleInterface);

        if (pAudioSilenceInfo->activateSilenceInsertion == true) {
            ALOGV("One silence Frame Inserted \n");
            return RETURN_STATUS_OK;
        }
    }

    ALOGV("Call To mp3_decode_frame\n");

    /*  Call To MP3 Decode Frame */
    errorStatus = (RETURN_STATUS_LEVEL_T)mp3_decode_frame(interface_fe);

    /* Reset some of variables to be used in getFrameReady */
    pAudioFrameInfo->mFrameReady = false;
    pAudioFrameInfo->mFirstFrame = false;
    pAudioFrameInfo->mCompleteFrame = false;

    if (errorStatus > RETURN_STATUS_WARNING) {
        return errorStatus;
    }

    ALOGV("Return from mp3_decode_frame\n");

    if (errorStatus != RETURN_STATUS_OK) {
        ALOGV("errorStatus NOK\n");
        pOutputAudioData->nOffset = 0;
        pOutputAudioData->nFilledLen = pAudioSilenceInfo->mSilenceFrameSize * pAudioSilenceInfo->mChannels * (pAudioSilenceInfo->mSampleSize >> 3);
        memset(pOutputAudioData->pBuffer, 0, pOutputAudioData->nFilledLen);
    } else {
        ALOGV("errorStatus OK\n");
        pOutputAudioData->nOffset = 0;
        pOutputAudioData->nFilledLen = interface_fe->sample_struct.block_len * interface_fe->sample_struct.chans_nb * (interface_fe->sample_struct.sample_size >> 3);
    }

    pOutputAudioData->nTimeStamp = pAudioFrameInfo->nThisFrameOutTimeStamp;

    pAudioTimeStampInfo->mCurrentFrameDuration  = pAudioFrameInfo->mFrameDuration;
    pAudioTimeStampInfo->mPrevFrameOutTimeStamp = pAudioTimeStampInfo->mCurrFrameOutTimeStamp;
    pAudioTimeStampInfo->mCurrFrameOutTimeStamp = pOutputAudioData->nTimeStamp;
    pAudioTimeStampInfo->mNextFrameOutTimeStamp = pAudioTimeStampInfo->mCurrFrameOutTimeStamp + pAudioTimeStampInfo->mCurrentFrameDuration;

    ALOGV("pAudioTimeStampInfo->mPrevFrameOutTimeStamp %lld\n", pAudioTimeStampInfo->mPrevFrameOutTimeStamp);
    ALOGV("pAudioTimeStampInfo->mCurrFrameOutTimeStamp %lld\n", pAudioTimeStampInfo->mCurrFrameOutTimeStamp);
    ALOGV("pAudioTimeStampInfo->mNextFrameOutTimeStamp %lld\n", pAudioTimeStampInfo->mNextFrameOutTimeStamp);

    return errorStatus;
}       // End of function STE_Audio_Module_processFrame

void STE_SoftMP3_Dec::onQueueFilled(OMX_U32 portIndex) {

    if (mSignalledError || mOutputPortSettingsChange != NONE) {
        return;
    }

    List<BufferInfo *> &inQueue = getPortQueue(0);
    List<BufferInfo *> &outQueue = getPortQueue(1);
    CODEC_INTERFACE_T *interface_fe = (CODEC_INTERFACE_T *)mp3_itf;

    ALOGV(" NEW ENTRY \n");

    while (!inQueue.empty() && !outQueue.empty()) {
        BufferInfo *inInfo = *inQueue.begin();
        OMX_BUFFERHEADERTYPE *inHeader = inInfo->mHeader;

        BufferInfo *outInfo = *outQueue.begin();
        OMX_BUFFERHEADERTYPE *outHeader = outInfo->mHeader;

        if (inHeader->nFlags & OMX_BUFFERFLAG_EOS && interface_fe->codec_state.eof == 1) {
            interface_fe->codec_state.eof = 2;
        }

        /* some frames must be decoded before singalling  bEOSReceived =true as might be possible that first frame might be EOS in this case no decoding possible so
         * straightway goto exit ..		*/
        if (inHeader->nFlags & OMX_BUFFERFLAG_EOS && interface_fe->codec_state.eof == 0 && mFirstTime == 0) {
            interface_fe->codec_state.eof = 1;
            bEOSReceived = true;
        }

        if (inHeader->nFlags & OMX_BUFFERFLAG_EOS && interface_fe->codec_state.eof == 0 && mFirstTime == 1) {
            interface_fe->codec_state.eof = 2;
            /* exit the code as EOS is signalled in the first go*/
        }

        ALOGV(" END OF STREAM %d\n", bEOSReceived);

        if (inHeader->nFlags & OMX_BUFFERFLAG_EOS && interface_fe->codec_state.eof == 2) {
            inQueue.erase(inQueue.begin());
            inInfo->mOwnedByUs = false;
            notifyEmptyBufferDone(inHeader);

            outHeader->nFilledLen = 0;
            outHeader->nFlags = OMX_BUFFERFLAG_EOS;

            ALOGV("END OF STREAM \n");

            outQueue.erase(outQueue.begin());
            outInfo->mOwnedByUs = false;
            notifyFillBufferDone(outHeader);
            return;
        }

        MP3_DEC_ERROR_ENUM_T error_check;
        RETURN_STATUS_LEVEL_T errorStatus = RETURN_STATUS_OK;
        RETURN_STATUS_LEVEL_T status = RETURN_STATUS_OK;
        int channels, samplerate;

        AUDIO_MODULE_INTERFACE_T *pAudioModuleInterface = (AUDIO_MODULE_INTERFACE_T *)pAudioModuleItf;
        AUDIO_INPUT_STRUCT_T     *pInputAudioData      = &pAudioModuleInterface->inputDataStruct;
        AUDIO_OUTPUT_STRUCT_T    *pOutputAudioData     = &pAudioModuleInterface->outputDataStruct;
        AUDIO_INPUT_CHUNK_INFO_T *pAudioChunkInfo      = &pAudioModuleInterface->inputAudioChunkInfo;
        AUDIO_TIMESTAMP_INFO_T   *pAudioTimeStampInfo  = &pAudioModuleInterface->audioTimeStampInfo;
        AUDIO_FRAME_INFO_T       *pAudioFrameInfo      = &pAudioModuleInterface->audioFrameInfo;
        AUDIO_SILENCE_INFO_T     *pAudioSilenceInfo    = &pAudioModuleInterface->audioSilenceInfo;
        AUDIO_PLAYER_COMMAND_T   *pAudioPlayerCommand  = &pAudioModuleInterface->audioPlayerCommand;
        CODEC_INTERFACE_T        *interface_fe         = (CODEC_INTERFACE_T *)pAudioModuleInterface->pAudioCodecInterface;

        ALOGV("To Read The New frame \n");

        pInputAudioData->nBufferStatusFlags = 0;
        pInputAudioData->nInputSpecificFlags = 0;
        pInputAudioData->nFrameworkSpecificFlags = 0;
        pAudioChunkInfo->mChunksForThisFrame = 0;

        if (inHeader->nOffset == 0 && bEOSReceived == false) {
            mAnchorTimeUs = inHeader->nTimeStamp;
            mNumFramesOutput = 0;
            if (mAnchorTimeUs == 0) { mFirstTime = 1; }
            pInputAudioData->nInputSpecificFlags |= STE_AUDIO_INPUT_NEW_BUFFER;
        }

        if (mFirstTime == 1) {
            ALOGV("First Frame Decoding\n");
            mp3_decode_reset(pAudioModuleItf);
            pAudioModuleInterface->inputDataStruct.nInputSpecificFlags |= STE_AUDIO_INPUT_FIRST_BUFFER;
            if (bSeekDone) {
                pInputAudioData->nInputSpecificFlags |= STE_AUDIO_INPUT_AFTER_SEEK;
            }

            mFirstTime = 0;
            bEOSReceived = false;
        }

        if (bSeekDone) {
            pInputAudioData->nInputSpecificFlags |= STE_AUDIO_INPUT_AFTER_SEEK;
            /*seek flag to be used only once for every seek  */
            bSeekDone = false;
        }

        pInputAudioData->nInputSpecificFlags |= STE_AUDIO_INPUT_GET_NEW_FRAME;

        if (bEOSReceived == false) {
            pInputAudioData->pBuffer = (char *)inHeader->pBuffer;
            pInputAudioData->nOffset = inHeader->nOffset;
            pInputAudioData->nFilledLen = inHeader->nFilledLen;
            pInputAudioData->nTimeStamp = mAnchorTimeUs;

            ALOGV("pInputAudioData->pBuffer: %p\n", pInputAudioData->pBuffer);
            ALOGV("pInputAudioData->nOffset: %d\n", pInputAudioData->nOffset);
            ALOGV("pInputAudioData->nFilledLen: %d\n", pInputAudioData->nFilledLen);
            ALOGV("pInputAudioData->nTimeStamp: %lld\n", pInputAudioData->nTimeStamp);

            ALOGV("Call to STE_Audio_Module_getFrameReady \n");

            error_check = (MP3_DEC_ERROR_ENUM_T)mp3_decode_getFrameReady((void *)pAudioModuleInterface);

            ALOGV("Return from STE_Audio_Module_getFrameReady \n");

            inHeader->nFilledLen -= pAudioChunkInfo->mBytesConsumed;
            inHeader->nOffset += pAudioChunkInfo->mBytesConsumed;

            if (pAudioFrameInfo->mFrameReady == false) {
                if (inHeader->nFilledLen == 0) {
                    inInfo->mOwnedByUs = false;
                    inQueue.erase(inQueue.begin());
                    inInfo = NULL;
                    notifyEmptyBufferDone(inHeader);
                    inHeader = NULL;
                }
                ALOGV("return for frame not ready\n");
                return;
            }
        }

        pInputAudioData->nBufferStatusFlags = 0;
        pInputAudioData->nInputSpecificFlags = 0;
        pInputAudioData->nFrameworkSpecificFlags = 0;

        pOutputAudioData->pBuffer = (char *)reinterpret_cast<int16_t *>(outHeader->pBuffer);
        pOutputAudioData->nOffset = 0;
        pOutputAudioData->nFilledLen = 0;
        pOutputAudioData->nBufferStatusFlags = 0;

        if (bEOSReceived == true) {

            if (pAudioFrameInfo->mFrameReady == false) {
                ALOGV("Pushing Empty Frame to decoder\n");
                pInputAudioData->pBuffer = (char *)NULL;
                pInputAudioData->nOffset = 0;
                pInputAudioData->nFilledLen = 0;
                pInputAudioData->nTimeStamp = pAudioFrameInfo->nThisFrameOutTimeStamp + pAudioFrameInfo->mFrameDuration;
                pInputAudioData->nFrameworkSpecificFlags |= STE_AUDIO_BUFFERFLAG_EOS;

                error_check = (MP3_DEC_ERROR_ENUM_T)mp3_decode_getFrameReady((void *)pAudioModuleInterface);
            }

            if (pAudioFrameInfo->mFrameReady == false) {
                if (inHeader->nFilledLen == 0) {
                    inInfo->mOwnedByUs = false;
                    inQueue.erase(inQueue.begin());
                    inInfo = NULL;
                    notifyEmptyBufferDone(inHeader);
                    inHeader = NULL;
                }
                ALOGV("END OF STREAM\n");
                return;
            }
        }

        /*mp3 core decoding function */
        ALOGV("To decode an encoded frame \n");

        status  = (RETURN_STATUS_LEVEL_T)mp3_decode_processFrame((void *)pAudioModuleInterface);

        ALOGV("Return from  decoder \n");

        if (status == RETURN_STATUS_WARNING) {
            status = RETURN_STATUS_OK;
        }

        channels = pAudioFrameInfo->nChannels;
        samplerate = pAudioFrameInfo->nSamplingFreq;

        if (status != RETURN_STATUS_OK) {
            if (interface_fe->codec_state.status != MP3_NO_ENOUGH_MAIN_DATA_ERROR) {
                ALOGE("mp3 decoder returned error %d decerr %x", status, interface_fe->codec_state.status);

                notify(OMX_EventError, OMX_ErrorUndefined, status, NULL);
                mSignalledError = true;
                return;
            }
        } else if (samplerate != mSamplingRate
                || channels != mNumChannels) {

            ALOGV("PORT SETTING CHANGED\n");
            ALOGV("interface_fe->sample_struct.sample_freq %d mSamplingRate %d\n", samplerate, mSamplingRate);
            ALOGV("interface_fe->sample_struct.chans_nb %d mNumChannels %d\n", channels, mNumChannels);
            mSamplingRate = samplerate;
            mNumChannels  = channels;

            notify(OMX_EventPortSettingsChanged, 1, 0, NULL);
            mOutputPortSettingsChange = AWAITING_DISABLED;
            return;
        }

        outHeader->nOffset = 0;
        outHeader->nFilledLen = pOutputAudioData->nFilledLen;

        outHeader->nTimeStamp = pOutputAudioData->nTimeStamp;

        //mNumFramesOutput += moutputFrameSize / mNumChannels;

        outHeader->nFlags = 0;

        //  CHECK_GE(inHeader->nFilledLen, mConfig->inputBufferUsedLength);

        // inHeader->nOffset += mConfig->inputBufferUsedLength;
        // inHeader->nFilledLen -= mConfig->inputBufferUsedLength;

        if (bEOSReceived == false) {
            if (inHeader->nFilledLen == 0) {
                inInfo->mOwnedByUs = false;
                inQueue.erase(inQueue.begin());
                inInfo = NULL;
                notifyEmptyBufferDone(inHeader);
                inHeader = NULL;
            }
        }

        outInfo->mOwnedByUs = false;
        outQueue.erase(outQueue.begin());
        outInfo = NULL;
        notifyFillBufferDone(outHeader);
        outHeader = NULL;
    }
}

#else
void STE_SoftMP3_Dec::onQueueFilled(OMX_U32 portIndex) {

    if (mSignalledError || mOutputPortSettingsChange != NONE) {
        return;
    }

    List<BufferInfo *> &inQueue = getPortQueue(0);
    List<BufferInfo *> &outQueue = getPortQueue(1);
    CODEC_INTERFACE_T *interface_fe = (CODEC_INTERFACE_T *)mp3_itf;

    ALOGV(" NEW ENTRY \n");

    while (!inQueue.empty() && !outQueue.empty()) {
        BufferInfo *inInfo = *inQueue.begin();
        OMX_BUFFERHEADERTYPE *inHeader = inInfo->mHeader;

        BufferInfo *outInfo = *outQueue.begin();
        OMX_BUFFERHEADERTYPE *outHeader = outInfo->mHeader;

         if (inHeader->nFlags & OMX_BUFFERFLAG_EOS) {
            inQueue.erase(inQueue.begin());
            inInfo->mOwnedByUs = false;
            notifyEmptyBufferDone(inHeader);

            // pad the end of the stream with 529 samples, since that many samples
            // were trimmed off the beginning when decoding started
            outHeader->nFilledLen = kPVMP3DecoderDelay * mNumChannels * sizeof(int16_t);
            memset(outHeader->pBuffer, 0, outHeader->nFilledLen);
            outHeader->nFlags = OMX_BUFFERFLAG_EOS;

            outQueue.erase(outQueue.begin());
            outInfo->mOwnedByUs = false;
            notifyFillBufferDone(outHeader);
            return;
        }

        BS_STRUCT_T bs_save;
        int kk, mpeg_id;
        unsigned int header, bits_con;
        AUDIO_MODE_T algo;
        MP3_DEC_ERROR_ENUM_T error_check;
        int layer_info, version, sample_rate, ch_mode, samples, bitrate, framelen;
        RETURN_STATUS_LEVEL_T status = RETURN_STATUS_OK;
        bool apply_silenceinsertion = false;

        int moutputFrameSize;

        MP3_DECODER_STRUCT_T *mp3_decoder_struct;
        mp3_decoder_struct = (MP3_DECODER_STRUCT_T *)interface_fe->codec_local_struct;

        if (inHeader->nOffset == 0) {
            mAnchorTimeUs = inHeader->nTimeStamp;
            mNumFramesOutput = 0;
            ALOGV("mAnchorTimeUs %lld\n", mAnchorTimeUs);
        }

        interface_fe->stream_struct.bits_struct.buf_add =
                (unsigned int *)((unsigned char *)inHeader->pBuffer + inHeader->nOffset);

        error_check = (MP3_DEC_ERROR_ENUM_T)mp3_2_5_parse_header(interface_fe);

        if (error_check == MP3_ILLEGAL_MODE) {
            ALOGE("Algo is unknown ");
            notify(OMX_EventError, OMX_ErrorUndefined, error_check, NULL);
            mSignalledError = true;
            return;
        }

        ch_mode =  interface_fe->sample_struct.chans_nb;
        sample_rate = interface_fe->sample_struct.sample_freq;
        samples = interface_fe->sample_struct.block_len;

        moutputFrameSize = samples *  ch_mode;

        if (mEnable_silenceinsertion == true) {

            MP3_DECODER_STRUCT_T *mp3_decoder_struct = (MP3_DECODER_STRUCT_T *)interface_fe->codec_local_struct;
            MP3_DECODER_INFO_STRUCT_T *mp3_param_struct = (MP3_DECODER_INFO_STRUCT_T *)interface_fe->codec_info_local_struct;

            ALOGV("before mPrevTimeStamp %lld mNewTimeStamp %lld\n", mPrevTimeStamp, mNewTimeStamp);

            if (mfirst_frame == 0) {
                mInitialTimeStamp = mAnchorTimeUs;
            }

            if ((mInitialTimeStamp == mAnchorTimeUs) && (bSeekDone == 1)) {
                mfirst_frame = 0;
                mPrevTimeStamp = 0;
                mNewTimeStamp = 0;
                mFrameDuration = 0;
                mExpectedTimeStamp = 0;
                mDeltaTimeStamp = 0;
                mFramesTobeInserted = 0;
                moutputFrameSize = 0;
                bSeekDone = 0;
            }

            mPrevTimeStamp = mNewTimeStamp;
            mNewTimeStamp = mAnchorTimeUs + ((mNumFramesOutput * 1000000) / sample_rate);

            ALOGV("after mPrevTimeStamp %lld mNewTimeStamp %lld\n", mPrevTimeStamp, mNewTimeStamp);

            bitrate = mp3_param_struct->bitrate;
            framelen = mp3_decoder_struct->mp3_Head.bits_in_frame;

            if (mFramesTobeInserted == 0) {
                if (bSeekDone == 0) { /* No silence insertion for seek */
                    //mFrameDuration = ( (long long)samples* 1000000) /( sample_rate) ;
                    mFrameDuration =  ((long long)framelen * 1000000ll) / bitrate;

                    //mFrameDuration = (mFrameDuration/1000)*1000;
                    mExpectedTimeStamp = mPrevTimeStamp + mFrameDuration;

                    /* start with time stamp with zero*/
                    if (mfirst_frame == 0) {
                        mExpectedTimeStamp = 0;
                        //mfirst_frame = 1;
                    }

                    mDeltaTimeStamp = (mNewTimeStamp - mExpectedTimeStamp);

                    /* Compute no of frames to be inserted */
                    if (mDeltaTimeStamp > (2 * ONE_MILI_SEC)) { // Compute above 2 mili sec difference
                        //mFramesTobeInserted = (int)((long long)(mDeltaTimeStamp + (mFrameDuration >>2))/mFrameDuration);
                        mFramesTobeInserted = (int)((long long)(mDeltaTimeStamp) / mFrameDuration);
                    }

                    /* this is done as after reboot the sysytem stream will play from the current position(not from the begining ) . without this
                     * silence insertion frames will be inserted and stream will always start from 0 timestamp which is not desirable. 10 is assumed to be
                     * normal frames to be inserted in the begining if any*/
                    if (mfirst_frame == 0 && mFramesTobeInserted > 10) {
                        mFramesTobeInserted = 0;
                    }
                    if (mfirst_frame == 0) {
                        mfirst_frame = 1;
                    }
                }
            }

            ALOGV("mFramesTobeInserted %d\n", mFramesTobeInserted);
            /*  Apply silence insertion */
            if (bSeekDone == 0) { /* NO silence insertion for seek */
                if (mFramesTobeInserted > 0) {

                    apply_silenceinsertion = true;

                    interface_fe->codec_state.output_enable = 0;

                    mp3_decoder_struct->mp3_Head.frame_cnt++;

                    outHeader->nTimeStamp = mExpectedTimeStamp;

                    mExpectedTimeStamp = mExpectedTimeStamp + mFrameDuration;
                    mNewTimeStamp = mPrevTimeStamp + mFrameDuration; // new time stamp

                    ALOGV("mExpectedTimeStamp %lld  mNewTimeStamp %lld\n\n", mExpectedTimeStamp,
                            mNewTimeStamp);

                    if (mFramesTobeInserted == 1) {
                        /* reset decoder so that next frame o/p is independent of prior frame */
                        mp3_reset((CODEC_INTERFACE_T *)mp3_itf);
                    }

                    mFramesTobeInserted--;
                }
            }
        }

        if (bSeekDone == true) {
            /*  reset_done set to false ; as it should be set to 1 once for every seek otherwise it should be zero */
            bSeekDone = false;
        }

        ALOGV("mFramesTobeInserted %d apply_silenceinsertion %d\n", mFramesTobeInserted, apply_silenceinsertion
            );
        if (apply_silenceinsertion == false) {
            interface_fe->stream_struct.bits_struct.pos_ptr =
                    interface_fe->stream_struct.bits_struct.buf_add;
            interface_fe->stream_struct.bits_struct.nbit_available = 32;
            interface_fe->stream_struct.bits_struct.buf_size = (inHeader->nFilledLen + 3) >> 2;
            interface_fe->stream_struct.bits_struct.buf_end  =
                    interface_fe->stream_struct.bits_struct.buf_add
                    + (int)(interface_fe->stream_struct.bits_struct.buf_size);
            interface_fe->stream_struct.bits_struct.word_size = 32;
            interface_fe->stream_struct.real_size_frame_in_bit = inHeader->nFilledLen;

            ALOGV("available bytes %d \n", ((CODEC_INTERFACE_T *)mp3_itf)->stream_struct.real_size_frame_in_bit);

            interface_fe->sample_struct.buf_add = (Float *)reinterpret_cast<int16_t *>(outHeader->pBuffer);

            ((CODEC_INTERFACE_T *)mp3_itf)->codec_state.remaining_blocks = 0;

            /*mp3 core decoding function */
            status  = (RETURN_STATUS_LEVEL_T)mp3_decode_frame((CODEC_INTERFACE_T *)mp3_itf);

            if (status == RETURN_STATUS_WARNING) {
                status = RETURN_STATUS_OK;
            }

            ALOGV("consumed bytes %d \n", ((CODEC_INTERFACE_T *)mp3_itf)->stream_struct.real_size_frame_in_bit);

        }
        //moutputFrameSize = interface_fe->sample_struct.block_len * interface_fe->sample_struct.chans_nb;

        ALOGV("o/p moutputFrameSize %d frameno %d outenable %d\n", moutputFrameSize,
                mp3_decoder_struct->mp3_Head.frame_cnt, interface_fe->codec_state.output_enable);
        ALOGV("o/p channels %d\n", ch_mode);
        ALOGV("o/p samplerate %d \n", sample_rate);

        /* output is not ready to write out */
        if (interface_fe->codec_state.output_enable == 0) {
            ALOGV("Output mute\n");
            memset(outHeader->pBuffer, 0, moutputFrameSize * sizeof(int16_t));
        }

        if (status != RETURN_STATUS_OK) {
            ALOGE("mp3 decoder returned error %d decerr %x", status, interface_fe->codec_state.status);

            notify(OMX_EventError, OMX_ErrorUndefined, status, NULL);
            mSignalledError = true;
            return;
        } else if (sample_rate != mSamplingRate
                || ch_mode != mNumChannels) {

            ALOGV("PORT SETTING CHANGED\n");
            ALOGV("sample_rate %d mSamplingRate %d\n", sample_rate, mSamplingRate);
            ALOGV("ch_mode %d mNumChannels %d\n", ch_mode, mNumChannels);
            mSamplingRate = sample_rate;
            mNumChannels = ch_mode;

            notify(OMX_EventPortSettingsChanged, 1, 0, NULL);
            mOutputPortSettingsChange = AWAITING_DISABLED;
            return;
        }

        outHeader->nOffset = 0;
        // outHeader->nFilledLen = moutputFrameSize * sizeof(int16_t);
        if (mIsFirst) {
            mIsFirst = false;
            // The decoder delay is 529 samples, so trim that many samples off
            // the start of the first output buffer. This essentially makes this
            // decoder have zero delay, which the rest of the pipeline assumes.
            outHeader->nOffset = kPVMP3DecoderDelay * mNumChannels * sizeof(int16_t);
            outHeader->nFilledLen = moutputFrameSize * sizeof(int16_t) - outHeader->nOffset;
        } else {
            outHeader->nOffset = 0;
            outHeader->nFilledLen = moutputFrameSize * sizeof(int16_t);
        }


        if (apply_silenceinsertion == false) {
            inHeader->nFilledLen -= interface_fe->stream_struct.real_size_frame_in_bit;
            inHeader->nOffset += interface_fe->stream_struct.real_size_frame_in_bit;
            outHeader->nTimeStamp =
                    mAnchorTimeUs
                    + (mNumFramesOutput * 1000000ll) / sample_rate;

            mNumFramesOutput += moutputFrameSize / mNumChannels;
        }

        outHeader->nFlags = 0;

        ALOGV("o/p timestamp %lld\n", outHeader->nTimeStamp);

        //  CHECK_GE(inHeader->nFilledLen, mConfig->inputBufferUsedLength);

        // inHeader->nOffset += mConfig->inputBufferUsedLength;
        // inHeader->nFilledLen -= mConfig->inputBufferUsedLength;

        if (inHeader->nFilledLen == 0) {
            inInfo->mOwnedByUs = false;
            inQueue.erase(inQueue.begin());
            inInfo = NULL;
            notifyEmptyBufferDone(inHeader);
            inHeader = NULL;
        }

        outInfo->mOwnedByUs = false;
        outQueue.erase(outQueue.begin());
        outInfo = NULL;
        notifyFillBufferDone(outHeader);
        outHeader = NULL;

    }

}
#endif
void STE_SoftMP3_Dec::onPortFlushCompleted(OMX_U32 portIndex) {
    if (portIndex == 0) {
        // Make sure that the next buffer output does not still
        // depend on fragments from the last one decoded.
#ifdef PARTIAL_FRAME_HANDLING
        AUDIO_MODULE_INTERFACE_T *pAudioModuleInterface = (AUDIO_MODULE_INTERFACE_T *)pAudioModuleItf;
        AUDIO_INPUT_STRUCT_T     *pInputAudioData      = &pAudioModuleInterface->inputDataStruct;
        ((CODEC_INTERFACE_T *)mp3_itf)->codec_state.eof = 0;
        mp3_decode_reset(pAudioModuleItf);
#endif
        ALOGV("RESET DONE\n");
        mp3_reset((CODEC_INTERFACE_T *)mp3_itf);
        bSeekDone = true;
        mIsFirst = true;
        //bEOSReceived = false;
        // pInputAudioData->nInputSpecificFlags |= STE_AUDIO_INPUT_AFTER_SEEK;
    }
}

void STE_SoftMP3_Dec::onPortEnableCompleted(OMX_U32 portIndex, bool enabled) {
    if (portIndex != 1) {
        return;
    }

    switch (mOutputPortSettingsChange) {
        case NONE:
            break;

        case AWAITING_DISABLED:
        {
            CHECK(!enabled);
            mOutputPortSettingsChange = AWAITING_ENABLED;
            break;
        }

        default:
        {
            CHECK_EQ((int)mOutputPortSettingsChange, (int)AWAITING_ENABLED);
            CHECK(enabled);
            mOutputPortSettingsChange = NONE;
            break;
        }
    }
}

}  // namespace android

android::SoftOMXComponent *createSoftOMXComponent(
    const char *name, const OMX_CALLBACKTYPE *callbacks,
    OMX_PTR appData, OMX_COMPONENTTYPE **component) {
    return new android::STE_SoftMP3_Dec(name, callbacks, appData, component);
}
