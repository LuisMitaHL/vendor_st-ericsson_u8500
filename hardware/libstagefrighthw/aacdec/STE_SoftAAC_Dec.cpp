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
 * STE_SoftAAC_Dec
 * ~STE_SoftAAC_Dec
 * initDecoder
 * internalGetParameter
 * onQueueFilled
 * onPortFlushCompleted
 * createSoftOMXComponent
 *
 */


//#define LOG_NDEBUG 0
#define LOG_TAG "STE_SoftAAC_Dec"
#include <utils/Log.h>

#include "STE_SoftAAC_Dec.h"

#include "common_interface.h"
#include "audiolibs_common.h"
#include "aac_local.h"

#define ONE_MILI_SEC 1000  // this is 1000 micro second
int aac_getAudioSpecificConfig(void *hBs, void *p_global, unsigned short ASC_header_size);

#include <cutils/properties.h>
#include <media/stagefright/foundation/ADebug.h>
#include <media/stagefright/foundation/hexdump.h>
#include <media/stagefright/MediaErrors.h>
#include <dlfcn.h>

#define STE_AAC_DECODER_LIBRARY "/system/lib/libstagefright_ste_aacdec.so"

namespace android {

template<class T>
static void InitOMXParams(T *params) {
    params->nSize = sizeof(T);
    params->nVersion.s.nVersionMajor = 1;
    params->nVersion.s.nVersionMinor = 0;
    params->nVersion.s.nRevision = 0;
    params->nVersion.s.nStep = 0;
}

STE_SoftAAC_Dec::STE_SoftAAC_Dec(
        const char *name,
        const OMX_CALLBACKTYPE *callbacks,
        OMX_PTR appData,
        OMX_COMPONENTTYPE **component)
    : SimpleSoftOMXComponent(name, callbacks, appData, component),
      mInputBufferCount(0),
      mUpsamplingFactor(2),
      mAnchorTimeUs(0),
      mNumSamplesOutput(0),
      mSignalledError(false),
      mPrevTimeStamp(0LL),
      mNewTimeStamp(0LL),
      mExpectedTimeStamp(0LL),
      mDeltaTimeStamp(0LL),
      mInitialTimeStamp(0LL),
      reset_done(0),
      mFramesTobeInserted(0),
      mFrameDuration(0),
      moutputFrameSize(0),
      mfirst_frame(0),
      mEnable_silenceinsertion(false),
      aac_itf(new CODEC_INTERFACE_T),
      aac_config(new AAC_DECODER_CONFIG_STRUCT_T),
      aac_info_struct(new AAC_DECODER_INFO_STRUCT_T),
      mDLHandle(NULL),
      mOutputPortSettingsChange(NONE) {

    mDLHandle = dlopen(STE_AAC_DECODER_LIBRARY, RTLD_NOW);

    if (mDLHandle == NULL) {
        ALOGE("Could not open the libstagefright_ste_aacdec library.\n");
        return;
    }
    STE_eaacPlus_open               = (STE_eaacPlus_open_t)dlsym(mDLHandle, "eaacPlus_open");
    STE_aac_getAudioSpecificConfig  = (STE_aac_getAudioSpecificConfig_t)dlsym(mDLHandle,
            "aac_getAudioSpecificConfig");
    STE_eaacPlus_close              = (STE_eaacPlus_close_t)dlsym(mDLHandle, "eaacPlus_close");
    STE_eaacPlus_reset              = (STE_eaacPlus_reset_t)dlsym(mDLHandle, "eaacPlus_reset");
    STE_eaacPlus_decode_frame       = (STE_eaacPlus_decode_frame_t)dlsym(mDLHandle,
            "eaacPlus_decode_frame");
    STE_aac_parse                   = (STE_aac_parse_t)dlsym(mDLHandle, "aac_parse");
    STE_aac_setoutput_mute          = (STE_aac_setoutput_mute_t)dlsym(mDLHandle,
            "setoutput_mute");

    initPorts();
    CHECK_EQ(initDecoder(), (status_t)OK);

    ALOGI("Successfully allocated ST-Ericsson '%s' decoder through the " \
         "SoftOMXPlugin interface", this->name());
}

STE_SoftAAC_Dec::~STE_SoftAAC_Dec() {

    STE_eaacPlus_close((CODEC_INTERFACE_T *)aac_itf);
    if (aac_itf) {
        delete ((CODEC_INTERFACE_T *)aac_itf);
    }
    if (aac_config) {
        delete (AAC_DECODER_CONFIG_STRUCT_T *)aac_config;
    }
    if (aac_info_struct) {
        delete (AAC_DECODER_INFO_STRUCT_T *)aac_info_struct;
    }

    if (mDLHandle != NULL) {
        dlclose(mDLHandle);
    }

    aac_itf         = NULL;
    aac_config      = NULL;
    aac_info_struct = NULL;
    mDLHandle       = NULL;
    STE_eaacPlus_open              = NULL;
    STE_aac_getAudioSpecificConfig = NULL;
    STE_eaacPlus_close             = NULL;
    STE_eaacPlus_reset             = NULL;
    STE_eaacPlus_decode_frame      = NULL;

    ALOGI("Deallocated ST-Ericsson '%s' decoder through the SoftOMXPlugin " \
         "interface", this->name());
}

void STE_SoftAAC_Dec::initPorts() {
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

    def.format.audio.cMIMEType = const_cast<char *>("audio/aac");
    def.format.audio.pNativeRender = NULL;
    def.format.audio.bFlagErrorConcealment = OMX_FALSE;
    def.format.audio.eEncoding = OMX_AUDIO_CodingAAC;

    addPort(def);

    def.nPortIndex = 1;
    def.eDir = OMX_DirOutput;
    def.nBufferCountMin = kNumBuffers;
    def.nBufferCountActual = def.nBufferCountMin;
    def.nBufferSize = 8192*3;
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

status_t STE_SoftAAC_Dec::initDecoder() {

    mPrevTimeStamp = 0;
    mNewTimeStamp = 0;
    mFrameDuration = 0;
    mExpectedTimeStamp = 0;
    mDeltaTimeStamp = 0;
    mFramesTobeInserted = 0;
    moutputFrameSize = 0;
    mfirst_frame = 0;
    mInitialTimeStamp = 0;
    mEnable_silenceinsertion = false;

    AAC_DECODER_CONFIG_STRUCT_T *p_config      = (AAC_DECODER_CONFIG_STRUCT_T *)aac_config;
    AAC_DECODER_INFO_STRUCT_T   *p_info_config = (AAC_DECODER_INFO_STRUCT_T *)aac_info_struct;
    CODEC_INIT_T kk;
    memset((char *)aac_itf, 0, sizeof(CODEC_INTERFACE_T));
    memset((char *)p_config, 0, sizeof(AAC_DECODER_CONFIG_STRUCT_T));
    memset((char *)p_info_config, 0, sizeof(AAC_DECODER_INFO_STRUCT_T));

    ((CODEC_INTERFACE_T *)aac_itf)->codec_config_local_struct = p_config;
    ((CODEC_INTERFACE_T *)aac_itf)->codec_info_local_struct = p_info_config;
    p_config->syntax  = AAC_RAW_SYNTAX;
    p_config->objectType = 2;
    p_config->sample_freq = ESAA_FREQ_32KHZ;
    p_config->bsac_usemaxlayer = 1;
    p_config->concealment_on = 3;
    p_config->crc_ignore = TRUE;
    p_config->aac_ch_nb = 2;
    ((CODEC_INTERFACE_T *)aac_itf)->sample_struct.sample_size = 16;
    ((CODEC_INTERFACE_T *)aac_itf)->codec_state.first_time = TRUE;
    ((CODEC_INTERFACE_T *)aac_itf)->codec_state.mode = AAC_RAW;

    kk = (CODEC_INIT_T)STE_eaacPlus_open((CODEC_INTERFACE_T *)aac_itf);
    if (kk != INIT_OK) {
        ALOGE("Failed to initialize MP4 audio decoder %d", kk);
        return UNKNOWN_ERROR;
    }

    return OK;
}

OMX_ERRORTYPE STE_SoftAAC_Dec::internalGetParameter(
        OMX_INDEXTYPE index, OMX_PTR params) {
    AAC_DECODER_CONFIG_STRUCT_T *p_config = (AAC_DECODER_CONFIG_STRUCT_T *)aac_config;
    switch (index) {
        case OMX_IndexParamAudioAac:
        {
            OMX_AUDIO_PARAM_AACPROFILETYPE *aacParams =
                    (OMX_AUDIO_PARAM_AACPROFILETYPE *)params;

            if (aacParams->nPortIndex != 0) {
                return OMX_ErrorUndefined;
            }

            aacParams->nBitRate = 0;
            aacParams->nAudioBandWidth = 0;
            aacParams->nAACtools = 0;
            aacParams->nAACERtools = 0;
            aacParams->eAACProfile = OMX_AUDIO_AACObjectMain;

            aacParams->eAACStreamFormat =
                mIsADTS
                    ? OMX_AUDIO_AACStreamFormatMP4ADTS
                    : OMX_AUDIO_AACStreamFormatMP4FF;
            aacParams->eChannelMode = OMX_AUDIO_ChannelModeStereo;

            if (!isConfigured()) {
                aacParams->nChannels = 1;
                aacParams->nSampleRate = 44100;
                aacParams->nFrameLength = 0;
            } else {
                aacParams->nChannels = p_config->aac_ch_nb;
                aacParams->nSampleRate = ((CODEC_INTERFACE_T *)aac_itf)->sample_struct.sample_freq;
                aacParams->nFrameLength = ((CODEC_INTERFACE_T *)aac_itf)->sample_struct.block_len;
            }

            return OMX_ErrorNone;
        }

        case OMX_IndexParamAudioPcm:
        {
            OMX_AUDIO_PARAM_PCMMODETYPE *pcmParams =
                    (OMX_AUDIO_PARAM_PCMMODETYPE *)params;

            if (pcmParams->nPortIndex != 1) {
                return OMX_ErrorUndefined;
            }

            pcmParams->eNumData = OMX_NumericalDataSigned;
            pcmParams->eEndian = OMX_EndianBig;
            pcmParams->bInterleaved = OMX_TRUE;
            pcmParams->nBitPerSample = 16;
            pcmParams->ePCMMode = OMX_AUDIO_PCMModeLinear;
            pcmParams->eChannelMapping[0] = OMX_AUDIO_ChannelLF;
            pcmParams->eChannelMapping[1] = OMX_AUDIO_ChannelRF;
            pcmParams->eChannelMapping[2] = OMX_AUDIO_ChannelCF;
            pcmParams->eChannelMapping[3] = OMX_AUDIO_ChannelLFE;
            pcmParams->eChannelMapping[4] = OMX_AUDIO_ChannelLS;
            pcmParams->eChannelMapping[5] = OMX_AUDIO_ChannelRS;


            if (!isConfigured()) {
                pcmParams->nChannels = 1;
                pcmParams->nSamplingRate = 44100;
            } else {
                pcmParams->nChannels = p_config->aac_ch_nb;
                pcmParams->nSamplingRate = ((CODEC_INTERFACE_T *)aac_itf)->
                            sample_struct.sample_freq;
            }

            return OMX_ErrorNone;
        }

        default:
            return SimpleSoftOMXComponent::internalGetParameter(index, params);
    }
}

OMX_ERRORTYPE STE_SoftAAC_Dec::internalSetParameter(
        OMX_INDEXTYPE index, const OMX_PTR params) {
    switch (index) {
        case OMX_IndexParamStandardComponentRole:
        {
            const OMX_PARAM_COMPONENTROLETYPE *roleParams =
                    (const OMX_PARAM_COMPONENTROLETYPE *)params;

            if (strncmp((const char *)roleParams->cRole,
                        "audio_decoder.aac",
                        OMX_MAX_STRINGNAME_SIZE - 1)) {
                return OMX_ErrorUndefined;
            }

            return OMX_ErrorNone;
        }

        case OMX_IndexParamAudioAac:
        {
            const OMX_AUDIO_PARAM_AACPROFILETYPE *aacParams =
                    (const OMX_AUDIO_PARAM_AACPROFILETYPE *)params;

            if (aacParams->nPortIndex != 0) {
                return OMX_ErrorUndefined;
            }

            if (aacParams->eAACStreamFormat == OMX_AUDIO_AACStreamFormatMP4FF) {
                mIsADTS = false;
            } else if (aacParams->eAACStreamFormat
                        == OMX_AUDIO_AACStreamFormatMP4ADTS) {
                mIsADTS = true;
            } else {
                return OMX_ErrorUndefined;
            }
            return OMX_ErrorNone;
        }

        case OMX_IndexParamAudioPcm:
        {
            const OMX_AUDIO_PARAM_PCMMODETYPE *pcmParams =
                (OMX_AUDIO_PARAM_PCMMODETYPE *)params;

            if (pcmParams->nPortIndex != 1) {
                return OMX_ErrorUndefined;
            }

            return OMX_ErrorNone;
        }
        default:
            return SimpleSoftOMXComponent::internalSetParameter(index, params);
    }
}

bool STE_SoftAAC_Dec::isConfigured() const {
    return mInputBufferCount > 0;
}

void STE_SoftAAC_Dec::maybeConfigureDownmix() const {
    char value[PROPERTY_VALUE_MAX];
    if (!(property_get("media.aac_51_output_enabled", value, NULL) &&
                (!strcmp(value, "1") || !strcasecmp(value, "true")))) {
            AAC_DECODER_CONFIG_STRUCT_T *p_config      = (AAC_DECODER_CONFIG_STRUCT_T *)aac_config;

            ALOGI("Downmixing multichannel AAC to stereo");
            p_config->aac_ch_nb = 2;
    }
}

const int sampling_freq_table[] =
{
    96000,
    88200,
    64000,
    48000,
    44100,
    32000,
    24000,
    22050,
    16000,
    12000,
    11025,
    8000
};

void STE_SoftAAC_Dec::onQueueFilled(OMX_U32 portIndex) {

    if (mSignalledError || mOutputPortSettingsChange != NONE) {
        return;
    }

    status_t err;
    int inbits, sample_freq;
    bool apply_silenceinsertion = false;
    int prevSamplingRate;

    AAC_DECODER_STRUCT_T *p_global = (AAC_DECODER_STRUCT_T *)
            ((CODEC_INTERFACE_T *)aac_itf)->codec_local_struct;
    AAC_DECODER_CONFIG_STRUCT_T *ptr_aac_config_local_struct    = (AAC_DECODER_CONFIG_STRUCT_T *)
            ((CODEC_INTERFACE_T *)aac_itf)->codec_config_local_struct;
    AAC_DECODER_INFO_STRUCT_T *p_info_config = (AAC_DECODER_INFO_STRUCT_T *)
            ((CODEC_INTERFACE_T *)aac_itf)->codec_info_local_struct;

    List<BufferInfo *> &inQueue = getPortQueue(0);
    List<BufferInfo *> &outQueue = getPortQueue(1);

    ALOGV("NEW ENTRY \n");

    if (portIndex == 0 && mInputBufferCount == 0) {
        ++mInputBufferCount;

        BufferInfo *info = *inQueue.begin();
        OMX_BUFFERHEADERTYPE *header = info->mHeader;

        BS_STRUCT_T *hBs;
        AAC_DECODER_STRUCT_T *p_global = (AAC_DECODER_STRUCT_T *)((CODEC_INTERFACE_T *)aac_itf)->
                    codec_local_struct;
        AAC_DECODER_CONFIG_STRUCT_T *p_config      = (AAC_DECODER_CONFIG_STRUCT_T *)aac_config;

        ((CODEC_INTERFACE_T *)aac_itf)->stream_struct.bits_struct.buf_add =
                (unsigned int *)(header->pBuffer + header->nOffset);
        ((CODEC_INTERFACE_T *)aac_itf)->stream_struct.bits_struct.pos_ptr =
                ((CODEC_INTERFACE_T *)aac_itf)->stream_struct.bits_struct.buf_add;
        ((CODEC_INTERFACE_T *)aac_itf)->stream_struct.bits_struct.nbit_available = 32;
        ((CODEC_INTERFACE_T *)aac_itf)->stream_struct.bits_struct.buf_size =
                (header->nFilledLen + 3) >> 2;
        ((CODEC_INTERFACE_T *)aac_itf)->stream_struct.bits_struct.buf_end =
                ((CODEC_INTERFACE_T *)aac_itf)->stream_struct.bits_struct.buf_add
                + (int)(((CODEC_INTERFACE_T *)aac_itf)->
                    stream_struct.bits_struct.buf_size);                // in words;
        ((CODEC_INTERFACE_T *)aac_itf)->stream_struct.bits_struct.word_size = 32;
        hBs = &((CODEC_INTERFACE_T *)aac_itf)->stream_struct.bits_struct;

        p_global->AudioSpecificConfigChecked = 1;
        ALOGV("ASC SIZE %d first word %x\n", header->nFilledLen, *((CODEC_INTERFACE_T *)aac_itf)->stream_struct.
                    bits_struct.buf_add);
        ALOGV("before mode %d\n", ((CODEC_INTERFACE_T *)aac_itf)->codec_state.mode);

        if (header->nFilledLen == 0) {
            err = AAC_ILLEGAL_PROGRAM;
            ALOGE("ASCSIZE is ZERO ");
            mSignalledError = true;
            notify(OMX_EventError, OMX_ErrorUndefined, err, NULL);
            return;
        }

        if (STE_aac_getAudioSpecificConfig((void *)hBs, (void *)p_global,
                    header->nFilledLen << 3) != 0) {
            err = AAC_ILLEGAL_PROGRAM;
            ALOGE("Error in STE ASC");
            mSignalledError = true;
            notify(OMX_EventError, OMX_ErrorUndefined, err, NULL);
            return;
        }

        ((CODEC_INTERFACE_T *)aac_itf)->codec_state.mode = AAC_RAW;
        p_config->syntax  = AAC_RAW_SYNTAX;
        p_config->EnableSBR = p_global->EnableSBR;
        p_config->aac_ch_nb = p_global->ch;
        p_global->frames = 1;
        p_config->sample_freq = p_global->aac_mc_info.sampling_rate_idx;
        ((CODEC_INTERFACE_T *)aac_itf)->sample_struct.sample_freq =
                sampling_freq_table[p_global->aac_mc_info.sampling_rate_idx];

        /* if SBR is signalled then double the o/p freq to avoid the port setting change*/
        if (p_global->EnableSBR == 1 && p_global->decim_enbl == 0) {
            ((CODEC_INTERFACE_T *)aac_itf)->sample_struct.sample_freq = 2 *
                    ((CODEC_INTERFACE_T *)aac_itf)->sample_struct.sample_freq;
        }

        ALOGV("afterASC mode %d\n", ((CODEC_INTERFACE_T *)aac_itf)->codec_state.mode);
        ALOGV("ASCdecodech %d \n", p_global->ch);
        ALOGV("ASCdecodeframe %d \n", p_global->frames);
        ALOGV("ASCdecodeSBR %d \n", p_global->EnableSBR);
        ALOGV("ASCdecoderate %d %d \n", p_global->aac_mc_info.sampling_rate_idx,
                ((CODEC_INTERFACE_T *)aac_itf)->sample_struct.sample_freq);

        inQueue.erase(inQueue.begin());
        info->mOwnedByUs = false;
        notifyEmptyBufferDone(header);

        maybeConfigureDownmix();
        notify(OMX_EventPortSettingsChanged, 1, 0, NULL);
        mOutputPortSettingsChange = AWAITING_DISABLED;
        return;
    }

    while (!inQueue.empty() && !outQueue.empty()) {
        BufferInfo *inInfo = *inQueue.begin();
        OMX_BUFFERHEADERTYPE *inHeader = inInfo->mHeader;

        BufferInfo *outInfo = *outQueue.begin();
        OMX_BUFFERHEADERTYPE *outHeader = outInfo->mHeader;

        if (inHeader->nFlags & OMX_BUFFERFLAG_EOS) {
            ALOGV("END OF STREAM\n");
            inQueue.erase(inQueue.begin());
            inInfo->mOwnedByUs = false;
            notifyEmptyBufferDone(inHeader);

            outHeader->nFilledLen = 0;
            outHeader->nFlags = OMX_BUFFERFLAG_EOS;

            outQueue.erase(outQueue.begin());
            outInfo->mOwnedByUs = false;
            notifyFillBufferDone(outHeader);
            return;
        }

        if (inHeader->nOffset == 0) {
            mAnchorTimeUs = inHeader->nTimeStamp;
            mNumSamplesOutput = 0;

            ALOGV("mAnchorTimeUs %lld %lld\n", mAnchorTimeUs, inHeader->nTimeStamp);

            if (reset_done == 1 && mAnchorTimeUs == 0) {
                /* the stream has reset to begining */
                if (((CODEC_INTERFACE_T *)aac_itf)->codec_state.mode == AAC_RAW) {
                    /* ASC is parsed so frame no starts from 1 */
                    p_global->frames = 1;
                } else {
                    /* no ASC is parsed so frame no starts from 0 */
                    p_global->frames = 0;
                }
            }
        }

        if (mfirst_frame == 0) {
            /* Tap the first time stamp */
            mInitialTimeStamp = mAnchorTimeUs;
        }
        if (mInitialTimeStamp == mAnchorTimeUs && reset_done == 1) {
            /* case when streams is played from begining after/during reset*/
            mfirst_frame = 0;
            reset_done = 0;
            mPrevTimeStamp = 0;
            mNewTimeStamp = 0;
            mFrameDuration = 0;
            mExpectedTimeStamp = 0;
            mDeltaTimeStamp = 0;
            mFramesTobeInserted = 0;
            moutputFrameSize = 0;
        }
        size_t adtsHeaderSize = 0;
        if (mIsADTS) {
            // skip 30 bits, aac_frame_length follows.
            // ssssssss ssssiiip ppffffPc ccohCCll llllllll lll?????

            const uint8_t *adtsHeader = inHeader->pBuffer + inHeader->nOffset;

            bool signalError = false;
            if (inHeader->nFilledLen < 7) {
                ALOGE("Audio data too short to contain even the ADTS header. "
                      "Got %ld bytes.", inHeader->nFilledLen);
                hexdump(adtsHeader, inHeader->nFilledLen);
                signalError = true;
            } else {
                bool protectionAbsent = (adtsHeader[1] & 1);

                unsigned aac_frame_length =
                    ((adtsHeader[3] & 3) << 11)
                    | (adtsHeader[4] << 3)
                    | (adtsHeader[5] >> 5);

                if (inHeader->nFilledLen < aac_frame_length) {
                    ALOGE("Not enough audio data for the complete frame. "
                          "Got %ld bytes, frame size according to the ADTS "
                          "header is %u bytes.",
                          inHeader->nFilledLen, aac_frame_length);
                    hexdump(adtsHeader, inHeader->nFilledLen);
                    signalError = true;
                } else {
                    adtsHeaderSize = (protectionAbsent ? 7 : 9);

                    ((CODEC_INTERFACE_T *)aac_itf)->stream_struct.bits_struct.buf_add =
                        (unsigned int *)((unsigned char *)adtsHeader + adtsHeaderSize);

                    inbits =
                        aac_frame_length - adtsHeaderSize;

                    inHeader->nOffset += adtsHeaderSize;
                    inHeader->nFilledLen -= adtsHeaderSize;
                }
            }

            if (signalError) {
                mSignalledError = true;

                notify(OMX_EventError, OMX_ErrorStreamCorrupt,
                       ERROR_MALFORMED, NULL);

                return;
            }
        } else {
            ((CODEC_INTERFACE_T *)aac_itf)->stream_struct.bits_struct.buf_add = (unsigned int *)(inHeader->pBuffer + inHeader->nOffset);
            inbits = inHeader->nFilledLen;
        }

       // inbits = inHeader->nFilledLen;
        ((CODEC_INTERFACE_T *)aac_itf)->stream_struct.real_size_frame_in_bit = inbits << 3;

        /*((CODEC_INTERFACE_T *)aac_itf)->stream_struct.bits_struct.buf_add =
                (unsigned int *)((unsigned char *)inHeader->pBuffer + inHeader->nOffset);*/
        ((CODEC_INTERFACE_T *)aac_itf)->stream_struct.bits_struct.pos_ptr =
                ((CODEC_INTERFACE_T *)aac_itf)->stream_struct.bits_struct.buf_add;
        ((CODEC_INTERFACE_T *)aac_itf)->stream_struct.bits_struct.nbit_available = 32;
        ((CODEC_INTERFACE_T *)aac_itf)->stream_struct.bits_struct.buf_size = (inbits + 3) >> 2;
        ((CODEC_INTERFACE_T *)aac_itf)->stream_struct.bits_struct.buf_end  =
                ((CODEC_INTERFACE_T *)aac_itf)->stream_struct.bits_struct.buf_add
                + (int)(((CODEC_INTERFACE_T *)aac_itf)->stream_struct.bits_struct.buf_size);
        ((CODEC_INTERFACE_T *)aac_itf)->stream_struct.bits_struct.word_size = 32;

        ((CODEC_INTERFACE_T *)aac_itf)->sample_struct.buf_add =
                (Float *)reinterpret_cast<int16_t *>(outHeader->pBuffer + outHeader->nOffset);

        ALOGV("\nInitital Total frames inserted %d mfirst_frame %d inbits %d\n",
                mFramesTobeInserted, mfirst_frame, inbits);
        /* To produce 2 o/p channels */
        ptr_aac_config_local_struct->aac_config_map[0] = AUDIOLIBS_CHANNEL_LEFT;
        ptr_aac_config_local_struct->aac_config_map[1] = AUDIOLIBS_CHANNEL_RIGHT;

        /* parse header for stream type as adts/adif/raw */
        STE_aac_parse((CODEC_INTERFACE_T *)aac_itf);

        /* Silence insertion Functionality starts here */
        if (mEnable_silenceinsertion == true) {
            int sampling_freq;

            /*check for bounds of sample freq table */
            if (ptr_aac_config_local_struct->sample_freq > 12) {
                ptr_aac_config_local_struct->sample_freq = 0;
            }

            sampling_freq = sampling_freq_table[ptr_aac_config_local_struct->sample_freq];

            /* UPdate the time stamps*/
            mPrevTimeStamp = mNewTimeStamp;

            /* to handle multple blocks in ADTS format */
            mNewTimeStamp  = mAnchorTimeUs + ((mNumSamplesOutput * 1000000) / sampling_freq);

            ALOGV("Sample rate %d\n", sampling_freq);
            ALOGV("mPrevTimeStamp %lld mNewTimeStamp %lld\n", mPrevTimeStamp, mNewTimeStamp);

            if (mFramesTobeInserted == 0) {
                if (reset_done == 0) { /* No silence insertion for seek */
                    mFrameDuration =  ((1000000LL * 1024LL) / sampling_freq);
                    /*  to round to 1000's  place */
                    mFrameDuration =  (mFrameDuration / 1000) * 1000;
                    mExpectedTimeStamp = mPrevTimeStamp + mFrameDuration;

                    /* start with time stamp with zero*/
                    if (mfirst_frame == 0) {
                        mExpectedTimeStamp = 0;
                        mfirst_frame = 1;
                    }

                    mDeltaTimeStamp = (mNewTimeStamp - mExpectedTimeStamp);

                    /* Compute no of frames to be inserted */
                    if (mDeltaTimeStamp > (2 * ONE_MILI_SEC)) { // Compute above 2 mili sec difference
                        mFramesTobeInserted = (int)((long long)(mDeltaTimeStamp) / mFrameDuration);
                    }
                }
            }

            /*  Apply silence insertion */
            if (reset_done == 0) {
                /* NO silence insertion for seek */
                ALOGV("Total frames inserted %d mfirst_frame %d\n",
                        mFramesTobeInserted, mfirst_frame);
                if (mFramesTobeInserted > 0) {
                    /* enable the silence insertion to skip the present frame decoding */
                    apply_silenceinsertion = true;

                    /* following  function will set the reset the o/p buffer
                     * and set the o/p parameters as
                     * sampling freq, channel, blocklen */
                    STE_aac_setoutput_mute((CODEC_INTERFACE_T *)aac_itf);

                    mFramesTobeInserted--;

                    p_global->frames++;

                    outHeader->nTimeStamp = mExpectedTimeStamp;

                    mExpectedTimeStamp = mExpectedTimeStamp + mFrameDuration;
                    mNewTimeStamp = mPrevTimeStamp + mFrameDuration; // new time stamp
                }
            } else {
                /*  reset_done set to zero, as
                 *  it should be set to 1 once for every seek
                 *  otherwise it should be zero */
                reset_done = 0;
            }
        }

        mUpsamplingFactor = 1;

        prevSamplingRate =  ((CODEC_INTERFACE_T *)aac_itf)->sample_struct.sample_freq;
        ALOGV("prevSamplingRate %d \n", prevSamplingRate);

        if (apply_silenceinsertion == false) {
            /* skip the present frame decoding for silence insertion enable  */
            ALOGV("decoding AAC FRAME \n");
            int status = STE_eaacPlus_decode_frame((CODEC_INTERFACE_T *)aac_itf);
        }

        sample_freq = ((CODEC_INTERFACE_T *)aac_itf)->sample_struct.sample_freq;

        ALOGV("frameno %d flag %d remainging mFramesTobeInserted %d \n", p_global->frames,
                apply_silenceinsertion, mFramesTobeInserted);
        ALOGV("after outen  %d freq %d\n",
                ((CODEC_INTERFACE_T *)aac_itf)->codec_state.output_enable, sample_freq);
        ALOGV("after ch %d\n", (((CODEC_INTERFACE_T *)aac_itf)->sample_struct).chans_nb);
        ALOGV("after mode %d\n", ((CODEC_INTERFACE_T *)aac_itf)->codec_state.mode);
        ALOGV("after status %d codecmode %d freq %d\n",
                ((CODEC_INTERFACE_T *)aac_itf)->codec_state.status,
                ((CODEC_INTERFACE_T *)aac_itf)->codec_state.mode, sample_freq);
        ALOGV("after consumed %d ch %d\n",
                ((CODEC_INTERFACE_T *)aac_itf)->stream_struct.real_size_frame_in_bit,
                (((CODEC_INTERFACE_T *)aac_itf)->sample_struct).chans_nb);

        /*
         * AAC+/eAAC+ streams can be signalled in two ways: either explicitly
         * or implicitly, according to MPEG4 spec. AAC+/eAAC+ is a dual
         * rate system and the sampling rate in the final output is actually
         * doubled compared with the core AAC decoder sampling rate.
         *
         * Explicit signalling is done by explicitly defining SBR audio object
         * type in the bitstream. Implicit signalling is done by embedding
         * SBR content in AAC extension payload specific to SBR, and hence
         * requires an AAC decoder to perform pre-checks on actual audio frames.
         *
         * Thus, we could not say for sure whether a stream is
         * AAC+/eAAC+ until the first data frame is decoded.
         */

        // Check on the sampling rate to see whether it is changed.
        if (mInputBufferCount == 1) {
            if (sample_freq != prevSamplingRate) {
                ALOGW("Sample rate was %d Hz, but now is %d Hz",
                        prevSamplingRate, sample_freq);

                maybeConfigureDownmix();
                // We'll hold onto the input buffer and will decode
                // it again once the output port has been reconfigured.

                notify(OMX_EventPortSettingsChanged, 1, 0, NULL);
                mOutputPortSettingsChange = AWAITING_DISABLED;
                return;
            }
        }

        size_t numOutBytes =
                (((CODEC_INTERFACE_T *)aac_itf)->sample_struct.block_len) * sizeof(int16_t) *
                ptr_aac_config_local_struct->aac_ch_nb;

        ALOGV("numOutBytes %d\n", numOutBytes);

        if (((CODEC_INTERFACE_T *)aac_itf)->codec_state.output_enable == 0) {
            memset(outHeader->pBuffer + outHeader->nOffset, 0, numOutBytes);
        }

        if (apply_silenceinsertion == false) {
            CHECK_LE(((CODEC_INTERFACE_T *)aac_itf)->stream_struct.real_size_frame_in_bit,
                    inHeader->nFilledLen);

            inHeader->nFilledLen -= ((CODEC_INTERFACE_T *)aac_itf)->
                        stream_struct.real_size_frame_in_bit;
            inHeader->nOffset += ((CODEC_INTERFACE_T *)aac_itf)->
                        stream_struct.real_size_frame_in_bit;
        }

        outHeader->nFilledLen = numOutBytes;
        outHeader->nFlags = 0;

        /* already time stamp is applied */
        if (apply_silenceinsertion == false) {
            outHeader->nTimeStamp =
                    mAnchorTimeUs
                    + (mNumSamplesOutput * 1000000ll) / sample_freq;

            mNumSamplesOutput += ((CODEC_INTERFACE_T *)aac_itf)->sample_struct.block_len;
        }
        ALOGV("outHeader->nTimeStamp %lld\n", outHeader->nTimeStamp);

        outInfo->mOwnedByUs = false;
        outQueue.erase(outQueue.begin());
        outInfo = NULL;
        notifyFillBufferDone(outHeader);
        outHeader = NULL;

        if (inHeader->nFilledLen == 0) {
            inInfo->mOwnedByUs = false;
            inQueue.erase(inQueue.begin());
            inInfo = NULL;
            notifyEmptyBufferDone(inHeader);
            inHeader = NULL;
        }

        if (((CODEC_INTERFACE_T *)aac_itf)->codec_state.status == AAC_OK) {
            ++mInputBufferCount;
        }
    }
}

void STE_SoftAAC_Dec::onPortFlushCompleted(OMX_U32 portIndex) {

    if (portIndex == 0) {
        AAC_DECODER_STRUCT_T *p_global = (AAC_DECODER_STRUCT_T *)((CODEC_INTERFACE_T *)aac_itf)->
                    codec_local_struct;
        // Make sure that the next buffer output does not still
        // depend on fragments from the last one decoded.
        ALOGV("RESET DONE \n");
        /* if some frame is decoded then start the frame counter from 2 so that we can keep track of the frames ..
           Also if frame count is 1 means that only ASC is decoded till now & flush comes then donot set the frame count
           2 as no frame is decoded till now. */
	if(p_global->frames >1)
            p_global->frames = 2;
        ((CODEC_INTERFACE_T *)aac_itf)->codec_state.first_time = TRUE;
        STE_eaacPlus_reset((CODEC_INTERFACE_T *)aac_itf);
        reset_done = 1;
        mFramesTobeInserted = 0;
    }

}

void STE_SoftAAC_Dec::onPortEnableCompleted(OMX_U32 portIndex, bool enabled) {
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
    return new android::STE_SoftAAC_Dec(name, callbacks, appData, component);
}
