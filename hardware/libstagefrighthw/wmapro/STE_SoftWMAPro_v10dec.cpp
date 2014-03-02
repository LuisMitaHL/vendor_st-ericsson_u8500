/*
 * Copyright (C) ST-Ericsson SA 2012
 *
 * Implements STE's WMAPRO decoder in
 * local OMX framework.
 *
 */


//#define LOG_NDEBUG 0
#define LOG_TAG "STE_SoftWMAPro_v10dec"

#include <utils/Log.h>
#include "STE_SoftWMAPro_v10dec.h"
#include "common_interface.h"
#include "audiolibs_common.h"
#include "wmaprov10_decoder_interface.h"
#include <media/stagefright/foundation/ADebug.h>
#include <dlfcn.h>

#define STE_WMAPRO_V10_DECODER_LIBRARY "/system/lib/ste_wmapro_v10.so"
#define ONE_MILI_SEC 1000  // this is 1000 micro second

namespace android {

template<class T>
static void InitOMXParams(T *params) {
    params->nSize = sizeof(T);
    params->nVersion.s.nVersionMajor = 1;
    params->nVersion.s.nVersionMinor = 0;
    params->nVersion.s.nRevision = 0;
    params->nVersion.s.nStep = 0;
}

STE_SoftWMAPro_v10dec::STE_SoftWMAPro_v10dec(
        const char *name,
        const OMX_CALLBACKTYPE *callbacks,
        OMX_PTR appData,
        OMX_COMPONENTTYPE **component)
    : SimpleSoftOMXComponent(name, callbacks, appData, component),
      mInputBufferCount(0),
      mUpsamplingFactor(2),
      mAnchorTimeUs(0),
      mNumSamplesOutput(0),
      mNumChannels(2),
      mSamplingRate(44100),
      mSignalledError(false),
      wmapro_v10_itf(new CODEC_INTERFACE_T),
      wmapro_v10_config(new wmaProPlayerCommand),
      wmapro_v10_info_struct(new wmaProSpeakerPCMOutput),
      mDLHandle(NULL),
      mOutputPortSettingsChange(NONE) {

    mDLHandle = dlopen(STE_WMAPRO_V10_DECODER_LIBRARY, RTLD_NOW);

    if (mDLHandle == NULL) {
        ALOGE("Could not open the libste_wmapro_v10\n");
        return;
    }
    STE_wmapro_v10_open = (STE_wmapro_v10_open_t)dlsym(mDLHandle, "wmaprov10_decode_open");
    STE_wmapro_v10_close = (STE_wmapro_v10_close_t)dlsym(mDLHandle, "wmaprov10_decode_close");
    STE_wmapro_v10_reset = (STE_wmapro_v10_reset_t)dlsym(mDLHandle, "wmaprov10_decode_reset");
    STE_wmapro_v10_decode_frame = (STE_wmapro_v10_decode_frame_t)
                                    dlsym(mDLHandle, "wmaprov10_decode_frame");

    STE_wmaprov10_decode_init = (STE_wmaprov10_decode_init_t)
                                    dlsym(mDLHandle, "wmaprov10_decode_init");

    STE_wmaprov10_check_buffer_status = (STE_wmaprov10_check_buffer_status_t)
                                    dlsym(mDLHandle, "wmaprov10_check_buffer_status");

    initPorts();
    CHECK_EQ(initDecoder(), (status_t)OK);

    ALOGI("Successfully allocated ST-Ericsson '%s' decoder through the " \
         "SoftOMXPlugin interface", this->name());
}

STE_SoftWMAPro_v10dec::~STE_SoftWMAPro_v10dec() {

    STE_wmapro_v10_close((CODEC_INTERFACE_T *)wmapro_v10_itf);
    if (wmapro_v10_itf) {
        delete ((CODEC_INTERFACE_T *)wmapro_v10_itf);
    }
    if (wmapro_v10_config) {
        delete (wmaProPlayerCommand *)wmapro_v10_config;
    }
    if (wmapro_v10_info_struct) {
        delete (wmaProSpeakerPCMOutput *)wmapro_v10_info_struct;
    }

    if (mDLHandle != NULL) {
        dlclose(mDLHandle);
    }

    wmapro_v10_itf             = NULL;
    wmapro_v10_config          = NULL;
    wmapro_v10_info_struct     = NULL;
    mDLHandle               = NULL;
    STE_wmapro_v10_open             = NULL;
    STE_wmapro_v10_close            = NULL;
    STE_wmapro_v10_reset            = NULL;
    STE_wmapro_v10_decode_frame     = NULL;

    ALOGI("Deallocated ST-Ericsson '%s' decoder through the SoftOMXPlugin " \
         "interface", this->name());
}

void STE_SoftWMAPro_v10dec::initPorts() {
    OMX_PARAM_PORTDEFINITIONTYPE def;
    InitOMXParams(&def);

    def.nPortIndex = 0;
    def.eDir = OMX_DirInput;
    def.nBufferCountMin = kNumBuffers;
    def.nBufferCountActual = def.nBufferCountMin;
    def.nBufferSize = 8192*2;
    def.bEnabled = OMX_TRUE;
    def.bPopulated = OMX_FALSE;
    def.eDomain = OMX_PortDomainAudio;
    def.bBuffersContiguous = OMX_FALSE;
    def.nBufferAlignment = 1;

    def.format.audio.cMIMEType = const_cast<char *>("audio/wma");
    def.format.audio.pNativeRender = NULL;
    def.format.audio.bFlagErrorConcealment = OMX_FALSE;
    def.format.audio.eEncoding = OMX_AUDIO_CodingWMA;

    addPort(def);

    def.nPortIndex = 1;
    def.eDir = OMX_DirOutput;
    def.nBufferCountMin = kNumBuffers;
    def.nBufferCountActual = def.nBufferCountMin;
    def.nBufferSize = 8192*4*2;
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

status_t STE_SoftWMAPro_v10dec::initDecoder() {

    CODEC_INTERFACE_T *interface_fe = (CODEC_INTERFACE_T *)wmapro_v10_itf;
    wmaProPlayerCommand *p_config      = (wmaProPlayerCommand *)wmapro_v10_config;
    wmaProSpeakerPCMOutput *ptr_info_config = (wmaProSpeakerPCMOutput *)wmapro_v10_info_struct;
    CODEC_INIT_T kk;
    memset((char *)wmapro_v10_itf, 0, sizeof(CODEC_INTERFACE_T));
    memset((char *)p_config, 0, sizeof(wmaProPlayerCommand));
    memset((char *)ptr_info_config, 0, sizeof(wmaProSpeakerPCMOutput));

    interface_fe->codec_config_local_struct = p_config;
    interface_fe->codec_info_local_struct = ptr_info_config;

    kk = (CODEC_INIT_T)STE_wmapro_v10_open(interface_fe);
    if (kk != INIT_OK) {
        ALOGE("Failed to initialize WMAPro audio decoder %d", kk);
        return UNKNOWN_ERROR;
    }
    STE_wmaprov10_decode_init(wmapro_v10_itf);
    return OK;
}

OMX_ERRORTYPE STE_SoftWMAPro_v10dec::internalGetParameter(
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

OMX_ERRORTYPE STE_SoftWMAPro_v10dec::internalSetParameter(
        OMX_INDEXTYPE index, const OMX_PTR params) {
    switch (index) {
        case OMX_IndexParamStandardComponentRole:
        {
            const OMX_PARAM_COMPONENTROLETYPE *roleParams =
                (const OMX_PARAM_COMPONENTROLETYPE *)params;

            if (strncmp((const char *)roleParams->cRole,
                        "audio_decoder.wmapro",
                        OMX_MAX_STRINGNAME_SIZE - 1)) {
                return OMX_ErrorUndefined;
            }

            return OMX_ErrorNone;
        }

        default:
            return SimpleSoftOMXComponent::internalSetParameter(index, params);
    }
}

bool STE_SoftWMAPro_v10dec::isConfigured() const {
    return mInputBufferCount > 0;
}


void STE_SoftWMAPro_v10dec::onQueueFilled(OMX_U32 portIndex) {

    if (mSignalledError || mOutputPortSettingsChange != NONE) {
        return;
    }

    status_t err;
    int status;
    int inbits, sample_freq;
    int prevSamplingRate;
    size_t numOutBytes;
    CODEC_INTERFACE_T *interface_fe = (CODEC_INTERFACE_T *)wmapro_v10_itf;

    wmaProPlayerCommand *ptr_config_local_struct = (wmaProPlayerCommand *)interface_fe->
                                                codec_config_local_struct;

    wmaProSpeakerPCMOutput *ptr_info_config = (wmaProSpeakerPCMOutput *) interface_fe->
                                                codec_info_local_struct;

    List<BufferInfo *> &inQueue = getPortQueue(0);
    List<BufferInfo *> &outQueue = getPortQueue(1);

    ALOGV("NEW ENTRY \n");

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

        }
        prevSamplingRate =  interface_fe->sample_struct.sample_freq;
        ALOGV("prevSamplingRate %d \n", prevSamplingRate);
        ALOGV("checking buffer status \n");
        int inbytes = inHeader->nFilledLen;
        interface_fe->stream_struct.bits_struct.buf_add =
        (unsigned int *)((unsigned char *)inHeader->pBuffer + inHeader->nOffset);

        interface_fe->stream_struct.real_size_frame_in_bit = inbits << 3;
        inHeader->nFilledLen = STE_wmaprov10_check_buffer_status(interface_fe);
        inHeader->nOffset += inHeader->nFilledLen;
        interface_fe->codec_state.remaining_blocks = 1;

        ALOGV("decoding WMAPro FRAME \n");
        status = STE_wmapro_v10_decode_frame(interface_fe);

        sample_freq = interface_fe->sample_struct.sample_freq;

        ALOGV("after outen  %d freq %d\n",
                interface_fe->codec_state.output_enable, sample_freq);
        ALOGV("after ch %d\n", (interface_fe->sample_struct).chans_nb);
        ALOGV("after status %d codecmode %d freq %d\n",
                interface_fe->codec_state.status,
                interface_fe->codec_state.mode, sample_freq);
        ALOGV("after consumed %d ch %d\n",
                interface_fe->stream_struct.real_size_frame_in_bit,
                (interface_fe->sample_struct).chans_nb);
        // Check on the sampling rate to see whether it is changed.
        if (mInputBufferCount == 1) {
            if (sample_freq != prevSamplingRate) {
                ALOGW("Sample rate was %d Hz, but now is %d Hz",
                        prevSamplingRate, sample_freq);

                // We'll hold onto the input buffer and will decode
                // it again once the output port has been reconfigured.

                notify(OMX_EventPortSettingsChanged, 1, 0, NULL);
                mOutputPortSettingsChange = AWAITING_DISABLED;
                return;
            }
        }

        if (interface_fe->codec_state.output_enable == 1) {
            size_t numOutBytes =
                (interface_fe->sample_struct.block_len) * sizeof(int16_t) *
                interface_fe->sample_struct.chans_nb;

            ALOGV("numOutBytes %d\n", numOutBytes);
            outHeader->nFilledLen = numOutBytes;
            outHeader->nFlags = 0;

              /* already time stamp is applied */
             outHeader->nTimeStamp =
                    mAnchorTimeUs
                    + (mNumSamplesOutput * 1000000ll) / sample_freq;

            mNumSamplesOutput += interface_fe->sample_struct.block_len;
            ALOGV("outHeader->nTimeStamp %lld\n", outHeader->nTimeStamp);

        }
        if (interface_fe->codec_state.output_enable == 1) {
            outInfo->mOwnedByUs = false;
            outQueue.erase(outQueue.begin());
            outInfo = NULL;
            notifyFillBufferDone(outHeader);
            outHeader = NULL;
        }

        if (inHeader->nFilledLen == 0) {
            inInfo->mOwnedByUs = false;
            inQueue.erase(inQueue.begin());
            inInfo = NULL;
            notifyEmptyBufferDone(inHeader);
            inHeader = NULL;
        }

    }
}

void STE_SoftWMAPro_v10dec::onPortFlushCompleted(OMX_U32 portIndex) {

    CODEC_INTERFACE_T *interface_fe = (CODEC_INTERFACE_T *)wmapro_v10_itf;

    if (portIndex == 0) {
        // Make sure that the next buffer output does not still
        // depend on fragments from the last one decoded.
        ALOGV("RESET DONE \n");
        interface_fe->codec_state.first_time = TRUE;
        STE_wmapro_v10_reset(interface_fe);
    }

}

void STE_SoftWMAPro_v10dec::onPortEnableCompleted(OMX_U32 portIndex, bool enabled) {
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
    return new android::STE_SoftWMAPro_v10dec(name, callbacks, appData, component);
}
