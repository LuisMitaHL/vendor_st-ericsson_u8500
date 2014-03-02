/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#define ADM_LOG_FILENAME "omx_log"

#include "OMX_Core.h"
#include "OMX_Component.h"
#include "AFM_Index.h"
#include "AFM_Types.h"
#include "AFM_Core.h"
#include "audio_chipset_api_index.h"
#include "audio_render_chipset_api.h"
#include "audio_codecs_chipset_api.h"
#include "OMX_Audio.h"
#include "omx_ste_speech_proc.h"
#include "audio_sourcesink_chipset_api.h"
#include "OMX_IndexExt.h"
#include "OMX_CoreExt.h"
#include "ste_adm_platform_adaptions.h"
#include "ste_adm_omx_core.h"
#include "OMX_STE_AudioEffectsExt.h"

#ifndef ADM_NO_EFFECT_CONFIG_SUPPORT
    #include "OMX_CNGExt.h"

    #include "OMX_DRCExt.h"
    #include "OMX_STE_VirtualSurroundExt.h"
    #include "omx_ste_noise_reduction.h"
    #include "audio_transducer_chipset_api.h"
#endif

#define DONT_PRINT_VERBOSE 0
#define PRINT_VERBOSE      1
#define FORCE_PRINT        2

static void dump_cfg_or_param(OMX_INDEXTYPE idx, const void* data, int is_verbose);

const char* adm_log_index2str(OMX_INDEXTYPE idx)
{
    switch ((int)idx) {
        case OMX_IndexParamAudioInit: return "AudioInit";
        case OMX_IndexParamVideoInit: return "VideoInit";
        case OMX_IndexParamImageInit: return "ImageInit";
        case OMX_IndexParamOtherInit: return "OtherInit";
        case OMX_IndexParamStandardComponentRole: return "StandardComponentRole";
        case OMX_IndexParamPortDefinition: return "PortDefinition";
        case OMX_IndexParamCompBufferSupplier: return "CompBufferSupplier";
        case OMX_IndexParamAudioPortFormat: return "AudioPortFormat";
        case OMX_IndexParamAudioPcm: return "AudioPcm";
        case OMX_IndexConfigAudioVolume: return "Volume";
        case OMX_IndexConfigAudioBalance: return "Balance";
        case OMX_IndexConfigAudioChannelMute: return "ChannelMute";
        case OMX_IndexConfigAudioMute: return "Mute";
        case OMX_IndexConfigAudioLoudness: return "Loudness";
        case OMX_IndexConfigAudioEchoCancelation: return "EchoCancelation";
        case OMX_IndexConfigAudioNoiseReduction: return "NoiseReduction";
        case OMX_IndexConfigAudioBass: return "Bass";
        case OMX_IndexConfigAudioTreble: return "Treble";
        case OMX_IndexConfigAudioStereoWidening: return "StereoWidening";
        case OMX_IndexConfigAudioChorus: return "Chorus";
        case OMX_IndexConfigAudioEqualizer: return "Equalizer";
        case OMX_IndexConfigAudioChannelVolume: return "ChannelVolume";
        case OMX_IndexConfigAudioLoopControl: return "LoopControl";
        case OMX_IndexConfigAudioLatency: return "Latency";
        case AFM_IndexParamPcm: return "AFM_IndexParamPcm";
        case AFM_IndexParamPcmLayout: return "AFM_IndexParamPcmLayout";
        case AFM_IndexParamSetPriority: return "AFM_IndexParamSetPriority";
        case AFM_IndexParamGetPriority: return "AFM_IndexParamGetPriority";
        case AFM_IndexParamCscall: return "AFM_IndexParamCscall";
        case AFM_IndexParamForceStandardTunneling: return "AFM_IndexParamForceStandardTunneling";
        case AFM_IndexParamMspNbChannel: return "AFM_IndexParamMspNbChannel";
        case OMX_IndexParamAudioSynchronized: return "OMX_IndexParamAudioSynchronized";
        case OMX_IndexConfigALSADevice: return "OMX_IndexConfigALSADevice";
        case AFM_IndexParamDmaChannelType: return "AFM_IndexParamDmaChannelType";
        case OMX_IndexParamLinePriority: return "OMX_IndexParamLinePriority";

        case OMX_Symbian_IndexConfigAudioCodecEnabled: return "OMX_Symbian_IndexConfigAudioCodecEnabled";
        case OMX_Symbian_IndexConfigAudioCodecDisabled: return "OMX_Symbian_IndexConfigAudioCodecDisabled";
        case OMX_Symbian_IndexConfigAudioSampleRate8khz: return "OMX_Symbian_IndexConfigAudioSampleRate8khz";
        case OMX_Symbian_IndexConfigAudioSampleRate16khz: return "OMX_Symbian_IndexConfigAudioSampleRate16khz";
        case ENS_IndexConfigTraceSetting: return "ENS_IndexConfigTraceSetting";
        case AFM_IndexParamHostMpcSync: return "AFM_IndexParamHostMpcSync";
        case AFM_IndexParamBindingMemoryType: return "AFM_IndexParamBindingMemoryType";
        case ADM_IndexConfigLatencySettings: return "ADM_IndexConfigLatencySettings";
        case ADM_IndexConfigStartupJitterMargin: return "ADM_IndexConfigStartupJitterMargin";
        case ADM_IndexParamDictRecMode: return "OMX_IndexParamDictRecMode";
        case AFM_IndexParamCsCallMpc: return "AFM_IndexParamCsCallMpc";
#ifdef ADM_MMPROBE
        case AFM_IndexConfigPcmProbe: return "AFM_IndexConfigPcmProbe";
#endif // ADM_MMPROBE

        case OMX_IndexConfigAudioChannelSwitchBoard: return "OMX_IndexConfigAudioChannelSwitchBoard";

#ifdef ADM_SRC_MODE
        case AFM_IndexParamSrcMode: return "AFM_IndexParamSrcMode";
#endif

#ifndef ADM_NO_EFFECT_CONFIG_SUPPORT
        case OMX_IndexParamAudioDRC: return "OMX_IndexParamAudioDRC";
        case OMX_IndexConfigAudioDRC: return "OMX_IndexConfigAudioDRC";
        case AFM_IndexParamTransducerEqualizer: return "AFM_IndexParamTransducerEqualizer";
        case AFM_IndexConfigTransducerEqualizer: return "AFM_IndexConfigTransducerEqualizer";
        case OMX_STE_IndexConfigAudioRxDRC: return "OMX_STE_IndexConfigAudioRxDRC";
        case OMX_STE_IndexConfigAudioTxDRC: return "OMX_STE_IndexConfigAudioTxDRC";
        case OMX_STE_IndexConfigAudioTxComfortNoise: return "OMX_STE_IndexConfigAudioTxComfortNoise";
        case OMX_STE_IndexConfigAudioRxComfortNoise: return "OMX_STE_IndexConfigAudioRxComfortNoise";
        case OMX_STE_IndexConfigAudioRxSpeechEnhancement: return "OMX_STE_IndexConfigAudioRxSpeechEnhancement";
        case OMX_STE_IndexConfigAudioTxSpeechEnhancement: return "OMX_STE_IndexConfigAudioTxSpeechEnhancement";
        case OMX_STE_IndexConfigAudioRxCTM: return "OMX_STE_IndexConfigAudioRxCTM";
        case OMX_STE_IndexConfigAudioTxCTM: return "OMX_STE_IndexConfigAudioTxCTM";
        case OMX_IndexConfigCallbackRequest: return "OMX_IndexConfigCallbackRequest";
        case AFM_IndexParamSpl: return "AFM_IndexParamSpl";
        case OMX_IndexConfigSpllimit: return "OMX_IndexConfigSpllimit";
        case OMX_Symbian_IndexConfigAudioSpeechLoops: return "OMX_Symbian_IndexConfigAudioSpeechLoops";
        case AFM_IndexConfigCscallSpeechTimingReq: return "AFM_IndexConfigCscallSpeechTimingReq";
        case AFM_IndexConfigCscallSpeechCodecReq: return "AFM_IndexConfigCscallSpeechCodecReq";
        case OMX_STE_IndexConfigAudioVirtualSurround: return "OMX_STE_IndexConfigAudioVirtualSurround";
        case OMX_STE_IndexConfigAudioNoiseReduction: return "OMX_STE_IndexConfigAudioNoiseReduction";
        case OMX_IndexConfigAudioComfortNoise: return "OMX_IndexConfigAudioComfortNoise";
        case AFM_IndexParamMdrc: return "AFM_IndexParamMdrc";
        case AFM_IndexGlobalConfigMdrc: return "AFM_IndexGlobalConfigMdrc";
        case OMX_IndexConfigAudioMdrc: return "OMX_IndexConfigAudioMdrc";

#endif
        case AFM_IndexParamRenderer: return "AFM_IndexParamRenderer";
        case OMX_IndexConfigAudioEffectAddPosition: return "OMX_IndexConfigAudioEffectAddPosition";
        case OMX_IndexConfigAudioEffectRemovePosition: return "OMX_IndexConfigAudioEffectRemovePosition";
        case OMX_IndexConfigAudioEffectCommit: return "OMX_IndexConfigAudioEffectCommit";
        case OMX_IndexConfigAudioEffectConfigure: return "OMX_IndexConfigAudioEffectConfigure";
        default: {  return "<unknown index code>"; }
    }
}

const char* adm_log_state2str(OMX_STATETYPE state)
{
    switch (state) {
        case OMX_StateInvalid: return "StateInvalid";
        case OMX_StateLoaded:  return "StateLoaded";
        case OMX_StateIdle:    return "StateIdle";
        case OMX_StateExecuting: return "StateExecuting";
        case OMX_StatePause:     return "StatePause";
        case OMX_StateWaitForResources: return "StateWaitForResources";
        case OMX_StateTestForResources: return "StateTestForResources";
        default: { return "<unknown event code>"; }
    }
}

const char* adm_log_event2str(OMX_EVENTTYPE event)
{
    switch (event) {
        case OMX_EventCmdComplete: return "CmdComplete";
        case OMX_EventError:       return "Error";
        case OMX_EventMark:        return "Mark";
        case OMX_EventPortSettingsChanged: return "PortSettingsChanged";
        case OMX_EventBufferFlag:  return "BufferFlag";
        case OMX_EventResourcesAcquired: return "ResourcesAcquired";
        case OMX_EventComponentResumed: return "ComponentResumed";
        case OMX_EventDynamicResourcesAvailable: return "DynamicResourcesAvailable";
        case OMX_EventPortFormatDetected: return "PortFormatDetected";
        case OMX_EventIndexSettingChanged: return "IndexSettingChanged";
        default: { return "<unknown event code>"; }
    }
}

const char* adm_log_cmd2str(OMX_COMMANDTYPE cmd)
{
    switch (cmd) {
        case OMX_CommandStateSet: return "StateSet";
        case OMX_CommandFlush:    return "Flush";
        case OMX_CommandPortDisable: return "PortDisable";
        case OMX_CommandPortEnable: return "PortEnable";
        case OMX_CommandMarkBuffer: return "MarkBuffer";
        default: { return "<unknown command code>"; }
    }
}

const char* adm_log_err2str(OMX_ERRORTYPE res)
{
    switch ((int)res) {
        case OMX_ErrorNone: return "OK";
        case OMX_ErrorInsufficientResources: return "OMX_ErrorInsufficientResources";
        case OMX_ErrorUndefined: return "OMX_ErrorUndefined";
        case OMX_ErrorInvalidComponentName: return "OMX_ErrorInvalidComponentName";
        case OMX_ErrorComponentNotFound: return "OMX_ErrorComponentNotFound";
        case OMX_ErrorInvalidComponent: return "OMX_ErrorInvalidComponent";
        case OMX_ErrorBadParameter: return "OMX_ErrorBadParameter";
        case OMX_ErrorNotImplemented: return "OMX_ErrorNotImplemented";
        case OMX_ErrorUnderflow: return "OMX_ErrorUnderflow";
        case OMX_ErrorOverflow: return "OMX_ErrorOverflow";
        case OMX_ErrorHardware: return "OMX_ErrorHardware";
        case OMX_ErrorInvalidState: return "OMX_ErrorInvalidState";
        case OMX_ErrorStreamCorrupt: return "OMX_ErrorStreamCorrupt";
        case OMX_ErrorPortsNotCompatible: return "OMX_ErrorPortsNotCompatible";
        case OMX_ErrorResourcesLost: return "OMX_ErrorResourcesLost";
        case OMX_ErrorNoMore: return "OMX_ErrorNoMore";
        case OMX_ErrorVersionMismatch: return "OMX_ErrorVersionMismatch";
        case OMX_ErrorNotReady: return "OMX_ErrorNotReady";
        case OMX_ErrorTimeout: return "OMX_ErrorTimeout";
        case OMX_ErrorSameState: return "OMX_ErrorSameState";
        case OMX_ErrorResourcesPreempted: return "OMX_ErrorResourcesPreempted";
        case OMX_ErrorPortUnresponsiveDuringAllocation: return "OMX_ErrorPortUnresponsiveDuringAllocation";
        case OMX_ErrorPortUnresponsiveDuringDeallocation: return "OMX_ErrorPortUnresponsiveDuringDeallocation";
        case OMX_ErrorPortUnresponsiveDuringStop: return "OMX_ErrorPortUnresponsiveDuringStop";
        case OMX_ErrorIncorrectStateTransition: return "OMX_ErrorIncorrectStateTransition";
        case OMX_ErrorIncorrectStateOperation: return "OMX_ErrorIncorrectStateOperation";
        case OMX_ErrorUnsupportedSetting: return "OMX_ErrorUnsupportedSetting";
        case OMX_ErrorUnsupportedIndex: return "OMX_ErrorUnsupportedIndex";
        case OMX_ErrorBadPortIndex: return "OMX_ErrorBadPortIndex";
        case OMX_ErrorPortUnpopulated: return "OMX_ErrorPortUnpopulated";
        case OMX_ErrorComponentSuspended: return "OMX_ErrorComponentSuspended";
        case OMX_ErrorDynamicResourcesUnavailable: return "OMX_ErrorDynamicResourcesUnavailable";
        case OMX_ErrorMbErrorsInFrame: return "OMX_ErrorMbErrorsInFrame";
        case OMX_ErrorFormatNotDetected: return "OMX_ErrorFormatNotDetected";
        case OMX_ErrorContentPipeOpenFailed: return "OMX_ErrorContentPipeOpenFailed";
        case OMX_ErrorContentPipeCreationFailed: return "OMX_ErrorContentPipeCreationFailed";
        case OMX_ErrorSeperateTablesUsed: return "OMX_ErrorSeperateTablesUsed";
        case OMX_ErrorTunnelingUnsupported: return "OMX_ErrorTunnelingUnsupported";
        case AFM_ErrorDspPanic: return "AFM_ErrorDspPanic";
        default: { return "<unknown error code>"; }
    }
}


// Make it possible to build parts of this logging 'standalone', so it
// can be used for other code (currently the IL server)
#ifndef ADM_LOG_STANDALONE

#include <stdlib.h>
#include <string.h>
#include "ste_adm_dbg.h"
#include <stdarg.h>
#include "AFM_Index.h"
#include "AFM_Types.h"
#include "audio_chipset_api_index.h"
#include "audio_render_chipset_api.h"
#include "ste_adm_omx_log.h"
#include "OMX_CoreExt.h"
#include "OMX_IndexExt.h"
#include "ste_adm_omx_tool.h"
#include "ste_adm_omx_log.h"



static const char* index2str(OMX_INDEXTYPE idx);

static void ste_adm_debug_logbuf_printf(const char* fmt, ...);



#include "OMX_Audio.h"
#include "omx_ste_speech_proc.h"
#include "audio_sourcesink_chipset_api.h"

// TODO: Discuss with JN how to move to prints
// TODO: vsnprintf
static void log_wrapper(OMX_ERRORTYPE res, const char* fmt, ...)
{
    if ( (res == OMX_ErrorNone && !ste_adm_debug_is_log_enabled(STE_ADM_LOG_OMXIL)) ||
     (res != OMX_ErrorNone && !ste_adm_debug_is_log_enabled(STE_ADM_LOG_ERR))) {
    return;
    }

    char linebuf[220];
    va_list a;
    va_start(a, fmt);
    vsnprintf(linebuf, sizeof(linebuf), fmt, a);
    va_end(a);

    if (res == OMX_ErrorNone) {
        ALOG_OMXIL(linebuf);
    } else {
        ALOG_ERR(linebuf);
    }
}

static void log_wrapper_verbose(OMX_ERRORTYPE res, const char* fmt, ...)
{
    if ( (res == OMX_ErrorNone && !ste_adm_debug_is_log_enabled(STE_ADM_LOG_OMXIL_VERBOSE)) ||
     (res != OMX_ErrorNone && !ste_adm_debug_is_log_enabled(STE_ADM_LOG_ERR))) {
    return;
    }

    char linebuf[220];
    va_list a;
    va_start(a, fmt);
    vsnprintf(linebuf, sizeof(linebuf), fmt, a);
    va_end(a);

    if (res == OMX_ErrorNone) {
        ALOG_OMXIL_VERBOSE(linebuf);
    } else {
        ALOG_ERR(linebuf);
    }
}

static void log_wrapper_cfg(int is_verbose, const char* fmt, ...)
{
    char linebuf[220];
    va_list a;
    va_start(a, fmt);
    vsnprintf(linebuf, sizeof(linebuf), fmt, a);
    va_end(a);

    if (is_verbose) {
        ALOG_OMXIL_VERBOSE(linebuf);
    } else {
        ALOG_OMXIL(linebuf);
    }
}



static void dumpAUDIO_PARAM_PCMMODETYPE(const void* data, int is_verbose)
{
    const OMX_AUDIO_PARAM_PCMMODETYPE* s = (const OMX_AUDIO_PARAM_PCMMODETYPE*) data;
    log_wrapper_cfg(is_verbose, "    port=%u channels=%u eNumData=%u endian=%u interleave=%u bits=%u samplerate=%u\n",
        s->nPortIndex, s->nChannels, s->eNumData, s->eEndian, s->bInterleaved,
        s->nBitPerSample, s->nSamplingRate);
    log_wrapper_cfg(is_verbose, "    pcmMode=%u map[0]=%u map[1]=%u map[2]=%u, map[3]=%u, map[4]=%u, map[5]=%u\n",
        s->ePCMMode, s->eChannelMapping[0], s->eChannelMapping[1], s->eChannelMapping[2], s->eChannelMapping[3],
        s->eChannelMapping[4], s->eChannelMapping[5]);
}


static void dumpAUDIO_CONFIG_VOLUMETYPE(const void* data, int is_verbose)
{
    const OMX_AUDIO_CONFIG_VOLUMETYPE* s = (const OMX_AUDIO_CONFIG_VOLUMETYPE*) data;
    log_wrapper_cfg(is_verbose, "    port=%u linear=%u volume=%d mB\n", s->nPortIndex, s->bLinear, s->sVolume.nValue); // todo max/min?
}

static void dumpAUDIO_CONFIG_MUTETYPE(const void* data, int is_verbose)
{
    const OMX_AUDIO_CONFIG_MUTETYPE* s = (const OMX_AUDIO_CONFIG_MUTETYPE*) data;
    log_wrapper_cfg(is_verbose, "    port=%u mute=%u\n", s->nPortIndex, s->bMute);
}

static void dumpAUDIO_CONFIG_LATENCYTYPE(const void* data, int is_verbose)
{
    const OMX_AUDIO_CONFIG_LATENCYTYPE* s = (const OMX_AUDIO_CONFIG_LATENCYTYPE*) data;
    log_wrapper_cfg(is_verbose, "    latency=%u\n", s->nLatency);
}

static void dumpPARAM_PORTDEFINITIONTYPE(const void* data, int is_verbose)
{
    const OMX_PARAM_PORTDEFINITIONTYPE* s = (const OMX_PARAM_PORTDEFINITIONTYPE*) data;

    log_wrapper_cfg(is_verbose, "    port=%u dir=%u act=%u min=%u sz=%u enabled=%u pop=%u domain=%u bcont=%u align=%u\n",
        s->nPortIndex, s->eDir, s->nBufferCountActual, s->nBufferCountMin, s->nBufferSize,
        s->bEnabled, s->bPopulated, s->eDomain, s->bBuffersContiguous, s->nBufferAlignment);
    if (s->eDomain == OMX_PortDomainAudio) {
        log_wrapper_cfg(is_verbose, "    mime=%s natread=%X conceal=%u encoding=%u\n",
            s->format.audio.cMIMEType, s->format.audio.pNativeRender,
            s->format.audio.bFlagErrorConcealment, s->format.audio.eEncoding);
    }
}

static void dumpAFM_PARAM_FORCE_STANDARD_TUNNELING(const void* data, int is_verbose)
{
    const AFM_PARAM_FORCE_STANDARD_TUNNELING* s = (const AFM_PARAM_FORCE_STANDARD_TUNNELING*) data;
    log_wrapper_cfg(is_verbose, "    port=%u bIsStandardTunneling=%u\n", s->nPortIndex, s->bIsStandardTunneling);
}

static void dumpAUDIO_PARAM_SYNCHRONIZEDTYPE(const void* data, int is_verbose)
{
    const OMX_AUDIO_PARAM_SYNCHRONIZEDTYPE* s = (const OMX_AUDIO_PARAM_SYNCHRONIZEDTYPE*) data;
    log_wrapper_cfg(is_verbose, "    port=%u bIsSynchronized=%u\n", s->nPortIndex, s->bIsSynchronized);
}

static void dumpAFM_AUDIO_PARAM_MSP_NB_CHANNEL(const void* data, int is_verbose)
{
    const AFM_AUDIO_PARAM_MSP_NB_CHANNEL* s = (const AFM_AUDIO_PARAM_MSP_NB_CHANNEL*) data;
    log_wrapper_cfg(is_verbose, "    nNbChannel=%d\n", s->nNbChannel);
}


static void dumpAFM_AUDIO_PARAM_DMA_CHANNEL_TYPE(const void* data, int is_verbose)
{
    const AFM_AUDIO_PARAM_DMA_CHANNEL_TYPE* s = (const AFM_AUDIO_PARAM_DMA_CHANNEL_TYPE*) data;
    log_wrapper_cfg(is_verbose, "    isPhysical=%d\n", s->isPhysical);
}

static void dumpOMX_CONFIG_CALLBACKREQUESTTYPE(const void* data, int is_verbose)
{
    const OMX_CONFIG_CALLBACKREQUESTTYPE* s = (const OMX_CONFIG_CALLBACKREQUESTTYPE*) data;
    log_wrapper_cfg(is_verbose, "    %s\n", index2str(s->nIndex));
}

static void dumpAFM_PARAM_HOST_MPC_SYNC_TYPE(const void* data, int is_verbose)
{
    const AFM_PARAM_HOST_MPC_SYNC_TYPE* s = (const AFM_PARAM_HOST_MPC_SYNC_TYPE*) data;
    log_wrapper_cfg(is_verbose, "    nPortIndex=%d enabled=%d\n", s->nPortIndex, s->enabled);
}

static void dumpAFM_PARAM_BINDING_MEMORY_TYPE(const void* data, int is_verbose)
{
    const AFM_PARAM_BINDING_MEMORY_TYPE* s = (const AFM_PARAM_BINDING_MEMORY_TYPE*) data;
    const char* type = "<invalid>";
    switch (s->memoryType) {
        case AFM_MEMORY_TYPE_SDRAM: type="SDRAM"; break;
        case AFM_MEMORY_TYPE_ESRAM: type="ESRAM"; break;
        case AFM_MEMORY_TYPE_TCMX:  type="TCMX";  break;
        case AFM_MEMORY_TYPE_TCMY:  type="TCMY";  break;
    }

    log_wrapper_cfg(is_verbose, "    nPortIndex=%d memoryType=%s\n", s->nPortIndex, type);
}

#ifndef ADM_NO_EFFECT_CONFIG_SUPPORT
static void dumpOMX_STE_AUDIO_CONFIG_COMFORT_NOISE(const void* data, int is_verbose)
{
    const OMX_STE_AUDIO_CONFIG_COMFORT_NOISE* s = (const OMX_STE_AUDIO_CONFIG_COMFORT_NOISE*) data;
    log_wrapper_cfg(is_verbose, "    nSize=%d nPortIndex=%d bEnable=%d sLevel.nValue=%d\n", s->nSize, s->nPortIndex, s->bEnable, s->sLevel.nValue);
}

#endif


static void dumpOMX_PARAM_BUFFERSUPPLIERTYPE(const void* data, int is_verbose)
{
    const OMX_PARAM_BUFFERSUPPLIERTYPE* s = (const OMX_PARAM_BUFFERSUPPLIERTYPE*) data;
    char* desc = "";
    switch (s->eBufferSupplier) {
        case OMX_BufferSupplyUnspecified: desc = "BufferSupplyUnspecified"; break;
        case OMX_BufferSupplyInput:       desc = "BufferSupplyInput";       break;
        case OMX_BufferSupplyOutput:      desc = "BufferSupplyOutput";      break;
        default:                          desc = "<invalid Buffer Supply>"; break;
    }
    log_wrapper_cfg(is_verbose, "    nPortIndex=%d %s\n", s->nPortIndex, desc);
}


static void dumpADM_CONFIG_LATENCYSETTINGTYPE(const void* data, int is_verbose)
{
    const ADM_CONFIG_LATENCYSETTINGTYPE* s = (const ADM_CONFIG_LATENCYSETTINGTYPE*) data;
    log_wrapper_cfg(is_verbose, "    nPortIndex=%d nbrBuffersToUse=%d bufferFillTime=%d \n",
            s->nPortIndex, s->nbrBuffersToUse, s->bufferFillTime);
}

static void dumpADM_CONFIG_STARTUPJITTERMARGINTYPE(const void* data, int is_verbose)
{
    const ADM_CONFIG_STARTUPJITTERMARGINTYPE* s = (const ADM_CONFIG_STARTUPJITTERMARGINTYPE*) data;
    log_wrapper_cfg(is_verbose, "    nPortIndex=%d startupJitterMargin=%d\n",
            s->nPortIndex, s->startupJitterMargin);
}

static void dumpOMX_PARAM_LINEPRIORITYTYPE(const void* data, int is_verbose)
{
    const OMX_PARAM_LINEPRIORITYTYPE* s = (const OMX_PARAM_LINEPRIORITYTYPE*) data;
    const char* str;
    switch (s->ePortPriority) {
        case OMX_PriorityBackground:    str="Background";     break;
        case OMX_PriorityNormal:        str="Normal";         break;
        case OMX_PriorityUrgent:        str="Urgent";         break;
        case OMX_PriorityTelephonyDL:   str="TelephonyDL";    break;
        case OMX_PriorityTelephonyUL:   str="TelephonyUL";    break;
        case OMX_PriorityTelephonyUL_2: str="TelephonyUL_2";  break;
        default:                        str="<unknown prio>"; break;
    }

    log_wrapper_cfg(is_verbose, "    nPortIndex=%d ePortPriority=%s\n", s->nPortIndex, str);
}

static void dumpAFM_AUDIO_PARAM_SRC_MODE(const void* data, int is_verbose)
{
    const AFM_AUDIO_PARAM_SRC_MODE* s = (const AFM_AUDIO_PARAM_SRC_MODE*) data;
    const char* str;
    switch (s->nSrcMode) {
        case AFM_SRC_MODE_NORMAL:    str="Normal";         break;
        case AFM_SRC_MODE_CUSTOM1:   str="Custom1";        break;
        case AFM_SRC_MODE_CUSTOM2:   str="Custom2";        break;
        case AFM_SRC_MODE_CUSTOM3:   str="Custom3";        break;
        case AFM_SRC_MODE_CUSTOM4:   str="Custom4";        break;
        case AFM_SRC_MODE_CUSTOM5:   str="Custom5";        break;
        case AFM_SRC_MODE_CUSTOM6:   str="Custom6";        break;
        case AFM_SRC_MODE_CUSTOM7:   str="Custom7";        break;
        case AFM_SRC_MODE_CUSTOM8:   str="Custom8";        break;
        default:                     str="<unknown mode>"; break;
    }

    log_wrapper_cfg(is_verbose, "    nPortIndex=%d nSrcMode=%s\n", s->nPortIndex, str);
}


static void dumpOMX_SYMBIAN_AUDIO_CONFIG_CHANNELSWITCHBOARDTYPE(const void* data, int is_verbose)
{
    const OMX_SYMBIAN_AUDIO_CONFIG_CHANNELSWITCHBOARDTYPE* s = (const OMX_SYMBIAN_AUDIO_CONFIG_CHANNELSWITCHBOARDTYPE*) data;
    log_wrapper_cfg(is_verbose, "    nPortIndex=%d nOutputChannel[0]=%d nOutputChannel[1]=%d\n",
                    s->nPortIndex, s->nOutputChannel[0], s->nOutputChannel[1]);
}

#ifndef ADM_NO_EFFECT_CONFIG_SUPPORT
static void dumpOMX_CONFIG_VIRTUALSURROUNDTYPE(const void* data, int is_verbose)
{
    const OMX_STE_AUDIO_CONFIG_VIRTUALSURROUNDTYPE* s = (const OMX_STE_AUDIO_CONFIG_VIRTUALSURROUNDTYPE*) data;
    log_wrapper_cfg(is_verbose, "    nPortIndex=%u bEnable=%u eVirtualSurroundMode=%u\n",
        s->nPortIndex, s->bEnable, s->eVirtualSurroundMode);

    log_wrapper_cfg(is_verbose, "    bRoomSimulationFrontEnable=%u bRoomSimulationSurroundEnable=%u eRoomSimulationMode=%u eRoomSimulationRoomType=%u\n",
        s->bRoomSimulationFrontEnable, s->bRoomSimulationSurroundEnable, s->eRoomSimulationMode, s->eRoomSimulationRoomType);

    log_wrapper_cfg(is_verbose, "    LeftFrontGain=%u RightFrontGain=%u CenterFrontGain=%u\n",
        s->nVirtualSurroundLeftFrontGain,s->nVirtualSurroundRightFrontGain, s->nVirtualSurroundCenterFrontGain);

    log_wrapper_cfg(is_verbose, "    LeftSurroundGain=%u RightSurroundGain=%u LowFrequencyEffectsGain=%u\n",
        s->nVirtualSurroundLeftSurroundGain,s->nVirtualSurroundRightSurroundGain, s->nVirtualSurroundLowFrequencyEffectsGain);

    log_wrapper_cfg(is_verbose, "    FrontRoomSimulationGain=%u SurroundRoomSimulationGain=%u\n",
        s->nVirtualSurroundFrontRoomSimulationGain, s->nVirtualSurroundSurroundRoomSimulationGain);
}

#endif


static void dumpOMX_PARAM_CSCALL_MPC(const void* data, int is_verbose)
{
    const AFM_AUDIO_PARAM_CSCALL_MPC* s = (const AFM_AUDIO_PARAM_CSCALL_MPC*) data;
    log_wrapper_cfg(is_verbose, "    nDuration=%u nSamplingRate=%u nLoopbackMode=%u nMspMode=%u\n",
        s->nDuration, s->nSamplingRate, s->nLoopbackMode, s->nMspMode);
}

static void dumpAFM_AUDIO_PARAM_RENDERER(const void* data, int is_verbose)
{
    const AFM_AUDIO_PARAM_RENDERER* s = (const AFM_AUDIO_PARAM_RENDERER*) data;
    log_wrapper_cfg(is_verbose, "    nBoardRC=%u \n",s->nBoardRC);
}


static void catf(char* buf, const char* fmt, ...)
{
    va_list a;
    va_start(a, fmt);
    vsprintf(buf+strlen(buf), fmt, a);
    va_end(a);
}

static void hexdump(const char* buf, unsigned int bytes)
{
    unsigned int cur=0;
    char linebuf[100];
    linebuf[0]=0;
    while (cur < bytes)
    {
        if (cur%16 == 0) catf(linebuf, "   %08X: ",cur);
        cur++;
        catf(linebuf,"%02X", 0xFF & *buf++);
        if (cur%16 == 0) { catf(linebuf,"\n");  ALOG_OMXIL(linebuf); linebuf[0]=0; }
        else if (cur%4  == 0) catf(linebuf," ");
    }
    if (cur%16 != 0) { catf(linebuf,"\n");  ALOG_OMXIL(linebuf); }
}

static void dumpOMX_AUDIO_CONFIG_EFFECTADDPOSITIONTYPE(const void* data, int is_verbose)
{
    const OMX_AUDIO_CONFIG_EFFECTADDPOSITIONTYPE* s = (const OMX_AUDIO_CONFIG_EFFECTADDPOSITIONTYPE*) data;
    log_wrapper_cfg(is_verbose, "    port=%u position=%u effect=%s\n",
        s->nPortIndex, s->nPositionIndex, s->cEffectName);
}

static void dumpOMX_AUDIO_CONFIG_EFFECTREMOVEPOSITIONTYPE(const void* data, int is_verbose)
{
    const OMX_AUDIO_CONFIG_EFFECTREMOVEPOSITIONTYPE* s = (const OMX_AUDIO_CONFIG_EFFECTREMOVEPOSITIONTYPE*) data;
    log_wrapper_cfg(is_verbose, "    port=%u position=%u\n",
        s->nPortIndex, s->nPositionIndex);
}

static void dumpOMX_AUDIO_CONFIG_EFFECTCOMMITTYPE(const void* data, int is_verbose)
{
    const OMX_AUDIO_CONFIG_EFFECTCOMMITTYPE* s = (const OMX_AUDIO_CONFIG_EFFECTCOMMITTYPE*) data;
    log_wrapper_cfg(is_verbose, "    nPortIndex=%u\n",
        s->nPortIndex);
}

static void dumpOMX_AUDIO_CONFIG_EFFECTCONFIGURETYPE(const void* data, int is_verbose)
{
    const OMX_AUDIO_CONFIG_EFFECTCONFIGURETYPE* s = (const OMX_AUDIO_CONFIG_EFFECTCONFIGURETYPE*) data;
    log_wrapper_cfg(is_verbose, "    nPortIndex=%u nPositionIndex=%u nEffectSpecificIndex=%s\n",
        s->base.nPortIndex, s->base.nPositionIndex, adm_log_index2str(s->base.nEffectSpecificIndex));
    dump_cfg_or_param(s->base.nEffectSpecificIndex, &s->sConfig, is_verbose);
}



static void dump_cfg_or_param(OMX_INDEXTYPE idx, const void* data, int is_verbose)
{
    if (is_verbose !=  FORCE_PRINT){
        if ( ( (is_verbose==PRINT_VERBOSE)  && !ste_adm_debug_is_log_enabled(STE_ADM_LOG_OMXIL_VERBOSE)) ||
             ( (is_verbose==DONT_PRINT_VERBOSE) && !ste_adm_debug_is_log_enabled(STE_ADM_LOG_OMXIL))) {
            return;
        }
    }

    switch ((int)idx) {
        case OMX_IndexParamAudioPcm: dumpAUDIO_PARAM_PCMMODETYPE(data, is_verbose); break;
        case OMX_IndexConfigAudioVolume: dumpAUDIO_CONFIG_VOLUMETYPE(data, is_verbose); break;
        case OMX_IndexConfigAudioMute: dumpAUDIO_CONFIG_MUTETYPE(data, is_verbose); break;
        case OMX_IndexConfigAudioLatency: dumpAUDIO_CONFIG_LATENCYTYPE(data, is_verbose); break;
        case OMX_IndexParamPortDefinition: dumpPARAM_PORTDEFINITIONTYPE(data, is_verbose); break;
        case AFM_IndexParamForceStandardTunneling: dumpAFM_PARAM_FORCE_STANDARD_TUNNELING(data, is_verbose); break;
        case OMX_IndexParamAudioSynchronized: dumpAUDIO_PARAM_SYNCHRONIZEDTYPE(data, is_verbose); break;
        case AFM_IndexParamMspNbChannel: dumpAFM_AUDIO_PARAM_MSP_NB_CHANNEL(data, is_verbose); break;
        case AFM_IndexParamDmaChannelType: dumpAFM_AUDIO_PARAM_DMA_CHANNEL_TYPE(data, is_verbose); break;
        case OMX_IndexConfigCallbackRequest: dumpOMX_CONFIG_CALLBACKREQUESTTYPE(data, is_verbose); break;
        case AFM_IndexParamHostMpcSync: dumpAFM_PARAM_HOST_MPC_SYNC_TYPE(data, is_verbose); break;
        case AFM_IndexParamBindingMemoryType: dumpAFM_PARAM_BINDING_MEMORY_TYPE(data, is_verbose); break;
        case OMX_IndexParamCompBufferSupplier: dumpOMX_PARAM_BUFFERSUPPLIERTYPE(data, is_verbose); break;
        case ADM_IndexConfigLatencySettings: dumpADM_CONFIG_LATENCYSETTINGTYPE(data, is_verbose); break;
        case ADM_IndexConfigStartupJitterMargin: dumpADM_CONFIG_STARTUPJITTERMARGINTYPE(data, is_verbose); break;
        case OMX_IndexParamLinePriority: dumpOMX_PARAM_LINEPRIORITYTYPE(data, is_verbose); break;
        case AFM_IndexParamSrcMode: dumpAFM_AUDIO_PARAM_SRC_MODE(data, is_verbose); break;
        case OMX_IndexConfigAudioChannelSwitchBoard: dumpOMX_SYMBIAN_AUDIO_CONFIG_CHANNELSWITCHBOARDTYPE(data, is_verbose); break;

#ifndef ADM_NO_EFFECT_CONFIG_SUPPORT
        case OMX_STE_IndexConfigAudioVirtualSurround: dumpOMX_CONFIG_VIRTUALSURROUNDTYPE(data, is_verbose); break;
        case OMX_STE_IndexConfigAudioTxComfortNoise: dumpOMX_STE_AUDIO_CONFIG_COMFORT_NOISE(data, is_verbose); break;
        case OMX_STE_IndexConfigAudioRxComfortNoise: dumpOMX_STE_AUDIO_CONFIG_COMFORT_NOISE(data, is_verbose); break;
#endif
        case AFM_IndexParamCsCallMpc: dumpOMX_PARAM_CSCALL_MPC(data, is_verbose); break;
        case AFM_IndexParamRenderer: dumpAFM_AUDIO_PARAM_RENDERER(data, is_verbose); break;
        case OMX_IndexConfigAudioEffectAddPosition: dumpOMX_AUDIO_CONFIG_EFFECTADDPOSITIONTYPE(data, is_verbose); break;
        case OMX_IndexConfigAudioEffectRemovePosition: dumpOMX_AUDIO_CONFIG_EFFECTREMOVEPOSITIONTYPE(data, is_verbose); break;
        case OMX_IndexConfigAudioEffectCommit: dumpOMX_AUDIO_CONFIG_EFFECTCOMMITTYPE(data, is_verbose); break;
        case OMX_IndexConfigAudioEffectConfigure: dumpOMX_AUDIO_CONFIG_EFFECTCONFIGURETYPE(data, is_verbose); break;

        default:
            if (is_verbose) {
                hexdump(data, *((const unsigned long*) data));
            }
        break;
    }
}



static const char* index2str(OMX_INDEXTYPE idx)
{
    const char* str = adm_log_index2str(idx);
    if (*str == '<') {
        if (idx == (OMX_INDEXTYPE) -1) {
            return "OMX_ALL";
        }
        ALOG_OMXIL("Unknown index (not all mapped): 0x%X\n", idx);
    }
    return str;
}





static const char *last_part(const char* buf, size_t max_len)
{
   size_t len = strlen(buf);
   if (len <= max_len) return buf;
   return buf + (len - max_len);
}



static const char* state2str(OMX_STATETYPE state)
{
    const char* str = adm_log_state2str(state);
    if (*str == '<') {
        ALOG_WARN("Unknown state code: %d\n", state);
    }
    return str;
}





static const char* event2str(OMX_EVENTTYPE event)
{
    const char* str = adm_log_event2str(event);
    if (*str == '<') {
        ALOG_WARN("Unknown event code: %d\n", event);
    }
    return str;
}


static const char* cmd2str(OMX_COMMANDTYPE cmd)
{
    const char* str = adm_log_cmd2str(cmd);
    if (*str == '<') {
        ALOG_WARN("Unknown command code: %d\n", cmd);
    }
    return str;
}




static const char* err2str(OMX_ERRORTYPE res)
{
    const char* str = adm_log_err2str(res);
    if (*str == '<') {
        ALOG_WARN("Unknown error code: %X\n", res);
    }
    return str;
}


#include <pthread.h>
static pthread_mutex_t il_log_mutex;
static int log_seq=1;
#define MAX_DATALOG 16
struct datalog
{
    FILE* f;
    OMX_HANDLETYPE comp;
    OMX_U32 port;
} datalog[MAX_DATALOG];


OMX_API OMX_ERRORTYPE OMX_APIENTRY il_tool_log_Init(void)
{
    OMX_ERRORTYPE res = OMX_Init();
    log_wrapper(res, "Init --> %s\n", err2str(res));
    pthread_mutex_init(&il_log_mutex, 0);
    memset(datalog, 0, sizeof(datalog));
    return res;
}



static void log_data(OMX_HANDLETYPE comp, OMX_BUFFERHEADERTYPE* pBuffer, OMX_U32 port)
{
    return;
    pthread_mutex_lock(&il_log_mutex);
    int i;
    for (i=0 ; i < MAX_DATALOG ; i++) {
        if (datalog[i].f && datalog[i].comp == comp && datalog[i].port == port)
            break;
    }

    if (i == MAX_DATALOG) {
        for (i=0 ; i < MAX_DATALOG ; i++)
            if (!datalog[i].f) {
                char name[80];
                sprintf(name, "/admpcmlog-%d-%X.%d", log_seq++, (int) comp, (int) port);
                printf("adm: log_data opening file %s\n", name);
                datalog[i].f = fopen(name, "wb");
                datalog[i].comp = comp;
                datalog[i].port = port;
                break;
            }
    }

    if (datalog[i].f) {
        fwrite(pBuffer->pBuffer, 1, pBuffer->nFilledLen, datalog[i].f);
    }

    pthread_mutex_unlock(&il_log_mutex);
}

static void log_data_close(OMX_HANDLETYPE comp, OMX_U32 port)
{
    return;
    pthread_mutex_lock(&il_log_mutex);
    int i;
    for (i=0 ; i < MAX_DATALOG ; i++) {
        if (datalog[i].f && datalog[i].comp == comp && ((port == 0xFFFFFFFF || port == datalog[i].port))) {
            fclose(datalog[i].f);
            datalog[i].f = 0;
        }
    }
    pthread_mutex_unlock(&il_log_mutex);
}



static    OMX_ERRORTYPE log_EventHandler(
        OMX_IN OMX_HANDLETYPE hComponent,
        OMX_IN OMX_PTR pAppData,
        OMX_IN OMX_EVENTTYPE eEvent,
        OMX_IN OMX_U32 nData1,
        OMX_IN OMX_U32 nData2,
        OMX_IN OMX_PTR pEventData)
{
    ADM_ASSERT(hComponent != NULL);
    ADM_ASSERT(pAppData != NULL);


    // Do not access pAddData for these spurious errors
    if (eEvent == OMX_EventError) {
        if (nData1 == (OMX_U32)OMX_ErrorOverflow || nData1 == (OMX_U32)OMX_ErrorPortUnpopulated) {
            ALOG_WARN("Error %s received from component %X\n", err2str(nData1), hComponent);
            return OMX_ErrorNone;
        }
        if (nData1 == (OMX_U32)OMX_ErrorUnderflow) {
            ALOG_WARN("Underflow %X. Usually: if sporadic (except at start/stop), problem in ADM or below. Large bursts; audioflinger or above\n", hComponent);
            return OMX_ErrorNone;
        }
    }


    const char* name_p = ((adm_il_appdata_t*) pAppData)->callback_state.component_name;

    if (eEvent == OMX_EventCmdComplete) {
        if (nData1 == OMX_CommandStateSet) {
            log_wrapper(OMX_ErrorNone, "%X [%-12s]: CB CmdComplete StateSet %s\n",
                hComponent, name_p, state2str(nData2));
            ste_adm_debug_logbuf_printf("%X [%-12s]: CB CmdComplete StateSet %s\n",
                hComponent, name_p, state2str(nData2));
        } else  if (nData1 == OMX_CommandPortDisable) {
            log_wrapper(OMX_ErrorNone, "%X [%-12s]: CB CmdComplete PortDisable %d\n",
                hComponent, name_p, nData2);
            ste_adm_debug_logbuf_printf("%X [%-12s]: CB CmdComplete PortDisable %d\n",
                hComponent, name_p, nData2);
        } else  if (nData1 == OMX_CommandPortEnable) {
            log_wrapper(OMX_ErrorNone, "%X [%-12s]: CB CmdComplete PortEnable %d\n",
                hComponent, name_p, nData2);
            ste_adm_debug_logbuf_printf("%X [%-12s]: CB CmdComplete PortEnable %d\n",
                hComponent, name_p, nData2);
        } else {
            log_wrapper(OMX_ErrorNone, "%X [%-12s]: CB CmdComplete %s nData2=%X pEventData=%X\n", hComponent, name_p, cmd2str(nData1), nData2, pEventData);
            ste_adm_debug_logbuf_printf("%X [%-12s]: CB CmdComplete %s nData2=%X pEventData=%X\n", hComponent, name_p, cmd2str(nData1), nData2, pEventData);
        }
    } else if (eEvent == OMX_EventError) {
        ALOG_ERR("%X [%-12s]: CB EventError %s nData2=%d\n", hComponent, name_p, err2str(nData1), nData2);
        ste_adm_debug_logbuf_printf("%X [%-12s]: CB EventError %s nData2=%d\n", hComponent, name_p, err2str(nData1), nData2);
    } else if ((OMX_EVENTEXTTYPE)eEvent == OMX_EventIndexSettingChanged) {
        log_wrapper(OMX_ErrorNone, "%X [%-12s]: CB EventIndexSettingChanged port=%d index=%s\n", hComponent, name_p, nData2, index2str(nData1));
        log_wrapper(OMX_ErrorNone, "%X [%-12s]: CB EventIndexSettingChanged port=%d index=%s\n", hComponent, name_p, nData1, index2str(nData2));
    } else {
        log_wrapper(OMX_ErrorNone, "%X [%-12s]: CB Event %s nData1=%X nData2=%X pEventData=%X\n", hComponent, name_p, event2str(eEvent), nData1, nData2, pEventData);
    }

    il_tool_event_cb((adm_il_appdata_t*) pAppData, eEvent, nData1, nData2, pEventData);
    return OMX_ErrorNone;
}



static  OMX_ERRORTYPE log_EmptyBufferDone(
        OMX_IN OMX_HANDLETYPE hComponent,
        OMX_IN OMX_PTR pAppData,
        OMX_IN OMX_BUFFERHEADERTYPE* pBuffer)
{
    ADM_ASSERT(hComponent != NULL);
    ADM_ASSERT(pAppData != NULL);

    const char* name_p = ((adm_il_appdata_t*) pAppData)->callback_state.component_name;

    ALOG_OMXIL_FLOW("%X [%-12s]: CB EmptyBufferDone port=%d pBuf=%X pBuf->pBuf=%X A=%u F=%u O=%u\n", hComponent, name_p, pBuffer->nInputPortIndex,   pBuffer, pBuffer->pBuffer, pBuffer->nAllocLen, pBuffer->nFilledLen, pBuffer->nOffset);
    il_tool_cb_empty_buffer_done((adm_il_appdata_t*) pAppData, pBuffer);
    return OMX_ErrorNone;
}

static OMX_ERRORTYPE log_FillBufferDone(
        OMX_OUT OMX_HANDLETYPE hComponent,
        OMX_OUT OMX_PTR pAppData,
        OMX_OUT OMX_BUFFERHEADERTYPE* pBuffer)
{
    ADM_ASSERT(hComponent != NULL);
    ADM_ASSERT(pAppData != NULL);

    const char* name_p = ((adm_il_appdata_t*) pAppData)->callback_state.component_name;

    ALOG_OMXIL_FLOW("%X [%-12s]: CB FillBufferDone port=%d pBuf=%X pBuf->pBuf=%X A=%u F=%u O=%u\n", hComponent, name_p, pBuffer->nOutputPortIndex, pBuffer, pBuffer->pBuffer, pBuffer->nAllocLen, pBuffer->nFilledLen, pBuffer->nOffset);
    log_data(hComponent, pBuffer, pBuffer->nOutputPortIndex);
    il_tool_cb_fill_buffer_done((adm_il_appdata_t*) pAppData, pBuffer);
    return OMX_ErrorNone;
}


static OMX_CALLBACKTYPE log_callbacks = {
    log_EventHandler,
    log_EmptyBufferDone,
    log_FillBufferDone
    };

#include "ste_adm_omx_core.h"
OMX_ERRORTYPE il_tool_log_CreateComponent(const char* name, adm_il_appdata_t* il_data)
{
    // The 'const' attribute is missing in the OMX_GetHandle call
    // in the official IL headers. We'll have to fix it here until
    // it is (hypothetically) fixed in the prober place. Ugly cast to avoid
    // gcc warning.
    ADM_ASSERT(il_data != NULL);
    char* const_cast_name = (char*) (long int) name;

    OMX_ERRORTYPE res = OMX_ErrorComponentNotFound;

    if(!strcmp(const_cast_name,adaptions_comp_name(ADM_MODEM_SOURCE_NAME))){
        if(adm_get_modem_type()==ADM_FAT_MODEM){
            res = adm_omxcore_OMX_GetHandle(&il_data->callback_state.omx_handle,
                    const_cast_name, il_data, &log_callbacks);
        }
    }

    if ((res == OMX_ErrorComponentNotFound) && adaptions_internal_mixsplit()) {
        res = adm_omxcore_OMX_GetHandle(&il_data->callback_state.omx_handle,
                    const_cast_name, il_data, &log_callbacks);
    }

    if (res == OMX_ErrorComponentNotFound) {
        res = OMX_GetHandle(&il_data->callback_state.omx_handle,
                const_cast_name, il_data, &log_callbacks);
    }

    if (res != OMX_ErrorNone) {
        log_wrapper(res, "GetHandle '%s' --> %s\n", name, err2str(res));
        return res;
    }


    // For debug convenience, save component name
    strncpy(il_data->callback_state.component_name, last_part(name,12), 79);
    il_data->callback_state.component_name[79] = 0;

    log_wrapper(res, "%X: GetHandle '%s' pAD=%X pCB=%X %s\n",
            il_data->callback_state.omx_handle, name, il_data, &log_callbacks, err2str(res));

    return res;
}


OMX_ERRORTYPE il_tool_log_GetExtensionIndex(
            il_comp_t comp_h,
            const char* cParameterName,
            OMX_OUT OMX_INDEXTYPE* pIndexType)
{
    ADM_ASSERT(comp_h != NULL);
    ADM_ASSERT(pIndexType != NULL);
    ADM_ASSERT(cParameterName != NULL);

    char* const_cast_name = (char*) (long int) cParameterName;

    // The 'const' attribute is missing in the OMX_GetExtensionIndex call
    // in the official IL headers. We'll have to fix it here until
    // it is (hypothetically) fixed in the prober place. Ugly cast to avoid
    // gcc warning.
    const char* name_p = comp_h->callback_state.component_name;

    OMX_ERRORTYPE res = OMX_GetExtensionIndex(comp_h->callback_state.omx_handle, const_cast_name, pIndexType);
    if (ste_adm_debug_is_log_enabled(STE_ADM_LOG_OMXIL_VERBOSE)) {
        if (res == OMX_ErrorNone) {
            log_wrapper_verbose(res, "%X [%-12s]: GetExtensionIndex '%s' --> %d %s\n", comp_h->callback_state.omx_handle, name_p, cParameterName, *pIndexType, err2str(res));
        } else {
            log_wrapper_verbose(res, "%X [%-12s]: GetExtensionIndex '%s' --> %s\n", comp_h->callback_state.omx_handle, name_p, cParameterName, err2str(res));
        }
    }
    return res;
}


OMX_ERRORTYPE il_tool_log_SetParameter(
            il_comp_t comp_h,
            OMX_IN  OMX_INDEXTYPE nIndex,
            const void* pComponentParameterStructure)
{
    ADM_ASSERT(comp_h != NULL);
    ADM_ASSERT(pComponentParameterStructure != NULL);

    // The 'const' attribute is missing for the pComponentParameterStructure
    // parameter in the official IL headers. We'll have to fix it here until
    // it is (hypothetically) fixed in the prober place. Ugly cast to avoid
    // gcc warning.
    void* const_cast_param = (void*) (long int) pComponentParameterStructure;
    ALOG_OMXIL("%X [%-12s]: SetParameter idx=%s\n", comp_h->callback_state.omx_handle, comp_h->callback_state.component_name, index2str(nIndex));
    dump_cfg_or_param(nIndex, pComponentParameterStructure, DONT_PRINT_VERBOSE);
    OMX_ERRORTYPE res = OMX_SetParameter(comp_h->callback_state.omx_handle, nIndex, const_cast_param);
    // hexdump(pComponentParameterStructure, *((unsigned long*) pComponentParameterStructure));

    if (res != OMX_ErrorNone) {
        ALOG_ERR("%X [%-12s]: last SetParameter idx=%s failed, err=%s\n", comp_h->callback_state.omx_handle, comp_h->callback_state.component_name, index2str(nIndex), adm_log_err2str(res));
        dump_cfg_or_param(nIndex, pComponentParameterStructure, FORCE_PRINT);
    }

    return res;
}

OMX_ERRORTYPE il_tool_log_SetConfig(
            il_comp_t comp_h,
            OMX_IN  OMX_INDEXTYPE nIndex,
            const void* pComponentConfigStructure)
{
    ADM_ASSERT(comp_h != NULL);
    ADM_ASSERT(pComponentConfigStructure != NULL);

    // The 'const' attribute is missing for the pComponentParameterStructure
    // parameter in the official IL headers. We'll have to fix it here until
    // it is (hypothetically) fixed in the prober place. Ugly cast to avoid
    // gcc warning.
    void* const_cast_param = (void*) (long int) pComponentConfigStructure;

    ALOG_OMXIL("%X [%-12s]: SetConfig idx=%s\n", comp_h->callback_state.omx_handle, comp_h->callback_state.component_name, index2str(nIndex));
    dump_cfg_or_param(nIndex, pComponentConfigStructure, DONT_PRINT_VERBOSE);

    OMX_ERRORTYPE res = OMX_SetConfig(comp_h->callback_state.omx_handle, nIndex, const_cast_param);
    if (res != OMX_ErrorNone) {
        ALOG_ERR("%X [%-12s]: last SetConfig idx=%s failed, err=%s\n", comp_h->callback_state.omx_handle, comp_h->callback_state.component_name, index2str(nIndex), adm_log_err2str(res));
        dump_cfg_or_param(nIndex, pComponentConfigStructure, FORCE_PRINT);
    }

    // hexdump(pComponentConfigStructure, *((unsigned long*) pComponentConfigStructure));
    return res;
}

#include <unistd.h>

#include <sys/time.h>
#include <sys/resource.h>



OMX_ERRORTYPE il_tool_log_SendCommand(
            il_comp_t comp_h,
            OMX_IN  OMX_COMMANDTYPE Cmd,
            OMX_IN  OMX_U32 nParam1,
            OMX_IN  OMX_PTR pCmdData)
{
    ADM_ASSERT(comp_h != NULL);

#if 0
    {
        struct sched_param sched_param;
        int    sched_policy;
        int    nice_prio;

        pthread_getschedparam(pthread_self(), &sched_policy, &sched_param);
        ALOG_STATUS("SCHED_OTHER=%d, SCHED_FIFO=%d, SCHED_RR=%d\n", SCHED_OTHER, SCHED_FIFO, SCHED_RR);
        ALOG_STATUS("il_tool_log_SendCommand thread %d info: getpriority=%d pthread.policy=%d pthread.sched_priority=%d\n",gettid(),
                    getpriority(PRIO_PROCESS, gettid()), sched_policy, sched_param.sched_priority);
    }
#endif

    if (Cmd == OMX_CommandStateSet) {
        ALOG_OMXIL("%X [%-12s]: SendCommand StateSet %s\n", comp_h->callback_state.omx_handle, comp_h->callback_state.component_name, state2str(nParam1));
        ste_adm_debug_logbuf_printf("%X [%-12s]: SendCommand StateSet %s\n", comp_h->callback_state.omx_handle, comp_h->callback_state.component_name, state2str(nParam1));
    } else {
        ALOG_OMXIL("%X [%-12s]: SendCommand %s %X\n", comp_h->callback_state.omx_handle, comp_h->callback_state.component_name, cmd2str(Cmd), nParam1);
        ste_adm_debug_logbuf_printf("%X [%-12s]: SendCommand %s %X\n", comp_h->callback_state.omx_handle, comp_h->callback_state.component_name, cmd2str(Cmd), nParam1);
    }
    OMX_ERRORTYPE res = OMX_SendCommand(comp_h->callback_state.omx_handle, Cmd, nParam1, pCmdData);
    if (res != OMX_ErrorNone) {
        if (Cmd == OMX_CommandStateSet) {
            ALOG_ERR("%X [%-12s]: prev cmd SendCommand StateSet %s failed, err %s\n", comp_h->callback_state.omx_handle, comp_h->callback_state.component_name, state2str(nParam1), adm_log_err2str(res));
            ste_adm_debug_logbuf_printf("%X [%-12s]: prev cmd SendCommand StateSet %s failed, err %s\n", comp_h->callback_state.omx_handle, comp_h->callback_state.component_name, state2str(nParam1), adm_log_err2str(res));
        } else {
            ALOG_ERR("%X [%-12s]: prev cmd SendCommand %s %X failed, err %s\n", comp_h->callback_state.omx_handle, comp_h->callback_state.component_name, cmd2str(Cmd), nParam1, adm_log_err2str(res));
            ste_adm_debug_logbuf_printf("%X [%-12s]: prev cmd SendCommand %s %X failed, err %s\n", comp_h->callback_state.omx_handle, comp_h->callback_state.component_name, cmd2str(Cmd), nParam1, adm_log_err2str(res));
        }
        if(res == OMX_ErrorTimeout)
        {
            ste_adm_debug_reboot();
        }
    }

    if (Cmd == OMX_CommandStateSet && nParam1 == OMX_StateLoaded) {
        log_data_close(comp_h->callback_state.omx_handle, 0xFFFFFFFF);
    } else if (Cmd == OMX_CommandPortDisable) {
        log_data_close(comp_h->callback_state.omx_handle, nParam1);
    }

    if (pCmdData) hexdump(pCmdData, *((unsigned long*) pCmdData));
    return res;
}

OMX_ERRORTYPE il_tool_log_EmptyThisBuffer(
            il_comp_t comp_h,
            OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer)
{
    ADM_ASSERT(comp_h != NULL);

#if 0
    {
        struct sched_param sched_param;
        int    sched_policy;
        int    nice_prio;

        pthread_getschedparam(pthread_self(), &sched_policy, &sched_param);
        ALOG_STATUS("SCHED_OTHER=%d, SCHED_FIFO=%d, SCHED_RR=%d\n", SCHED_OTHER, SCHED_FIFO, SCHED_RR);
        ALOG_STATUS("il_tool_log_EmptyThisBuffer thread %d info: getpriority=%d pthread.policy=%d pthread.sched_priority=%d\n",gettid(),
                    getpriority(PRIO_PROCESS, gettid()), sched_policy, sched_param.sched_priority);
    }
#endif

    /* Copy values to be printed from pBuffer in case the values have
       been modified before returning from OMX_EmptyThisBuffer */
    OMX_U32 filled_len = pBuffer->nFilledLen;
    OMX_U32 alloc_len = pBuffer->nAllocLen;
    OMX_U32 offset = pBuffer->nOffset;

    log_data(comp_h->callback_state.omx_handle, pBuffer, pBuffer->nInputPortIndex);

    ALOG_OMXIL_FLOW("%X [%-12s]: EmptyThisBuffer pBuf=%X pBuf->pBuf=%X nALen=%u nFLen=%u Offs=%u %s\n",
                    comp_h->callback_state.omx_handle, comp_h->callback_state.component_name,
                    pBuffer, pBuffer->pBuffer, alloc_len, filled_len, offset, pBuffer->nFlags & OMX_BUFFERFLAG_EOS ? "EOS" : "");
    OMX_ERRORTYPE res = OMX_EmptyThisBuffer(comp_h->callback_state.omx_handle, pBuffer);
    if (res != OMX_ErrorNone) {
        ALOG_ERR("%X [%-12s]: last EmptyThisBuffer failed, err=%s pBuf=%X pBuf->pBuf=%X nALen=%u nFLen=%u Offs=%u\n", comp_h->callback_state.omx_handle, comp_h->callback_state.component_name, adm_log_err2str(res), pBuffer, pBuffer->pBuffer, alloc_len, filled_len, offset);
    }
    return res;
}

OMX_ERRORTYPE il_tool_log_FillThisBuffer(
            il_comp_t comp_h,
            OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer)
{
    ADM_ASSERT(comp_h != NULL);
    /* Copy values to be printed from pBuffer in case the values have
       been modified before returning from OMX_FillThisBuffer */
    OMX_U32 filled_len = pBuffer->nFilledLen;
    OMX_U32 alloc_len = pBuffer->nAllocLen;
    OMX_U32 offset = pBuffer->nOffset;

    ALOG_OMXIL_FLOW("%X [%-12s]: FillThisBuffer pBuf=%X pBuf->pBuf=%X nALen=%u nFLen=%u Offs=%u\n", comp_h->callback_state.omx_handle, comp_h->callback_state.component_name, pBuffer, pBuffer->pBuffer, alloc_len, filled_len, offset);
    OMX_ERRORTYPE res = OMX_FillThisBuffer(comp_h->callback_state.omx_handle, pBuffer);
    if (res != OMX_ErrorNone) {
        ALOG_ERR("%X [%-12s]: last FillThisBuffer failed, err=%s pBuf=%X pBuf->pBuf=%X nALen=%u nFLen=%u Offs=%u\n", comp_h->callback_state.omx_handle, comp_h->callback_state.component_name, adm_log_err2str(res), pBuffer, pBuffer->pBuffer, alloc_len, filled_len, offset, adm_log_err2str(res));
    }

    return res;
}



OMX_ERRORTYPE il_tool_log_GetParameter(
            il_comp_t comp_h,
            OMX_IN  OMX_INDEXTYPE nParamIndex,
            OMX_INOUT OMX_PTR pComponentParameterStructure)
{
    ADM_ASSERT(comp_h != NULL);
    ADM_ASSERT(pComponentParameterStructure != NULL);

    OMX_ERRORTYPE res = OMX_GetParameter(comp_h->callback_state.omx_handle, nParamIndex, pComponentParameterStructure);
    log_wrapper_verbose(res, "%X [%-12s]: GetParameter %s --> %s\n", comp_h->callback_state.omx_handle, comp_h->callback_state.component_name, index2str(nParamIndex), err2str(res));
    if (res == OMX_ErrorNone){
        dump_cfg_or_param(nParamIndex, pComponentParameterStructure, PRINT_VERBOSE);
    }
    else{
        dump_cfg_or_param(nParamIndex, pComponentParameterStructure, FORCE_PRINT);
    }
    return res;
}

OMX_ERRORTYPE il_tool_log_GetConfig(
            il_comp_t comp_h,
            OMX_IN  OMX_INDEXTYPE nIndex,
            OMX_INOUT OMX_PTR pComponentConfigStructure)
{
    ADM_ASSERT(comp_h != NULL);
    ADM_ASSERT(pComponentConfigStructure != NULL);

    OMX_ERRORTYPE res = OMX_GetConfig(comp_h->callback_state.omx_handle, nIndex, pComponentConfigStructure);
    log_wrapper_verbose(res, "%X [%-12s]: GetConfig %s --> %s\n", comp_h->callback_state.omx_handle, comp_h->callback_state.component_name, index2str(nIndex), err2str(res));
    if (res == OMX_ErrorNone){
        dump_cfg_or_param(nIndex, pComponentConfigStructure, PRINT_VERBOSE);
    }
    else{
        dump_cfg_or_param(nIndex, pComponentConfigStructure, FORCE_PRINT);
    }
    return res;
}

OMX_ERRORTYPE il_tool_log_AllocateBuffer(
            il_comp_t comp_h,
            OMX_INOUT OMX_BUFFERHEADERTYPE** ppBuffer,
            OMX_IN OMX_U32 nPortIndex,
            OMX_IN OMX_PTR pAppPrivate,
            OMX_IN OMX_U32 nSizeBytes)
{
    ADM_ASSERT(comp_h != NULL);

    ALOG_OMXIL("%X [%-12s]: AllocBuf hdr=%X port=%d priv=%X bytes=%d", comp_h->callback_state.omx_handle,
        comp_h->callback_state.component_name, *ppBuffer, nPortIndex, pAppPrivate, nSizeBytes);
    OMX_ERRORTYPE res = OMX_AllocateBuffer(comp_h->callback_state.omx_handle, ppBuffer, nPortIndex, pAppPrivate, nSizeBytes);
    if (res != OMX_ErrorNone) {
        ALOG_ERR("%X [%-12s]: AllocBuf hdr=%X port=%d priv=%X bytes=%d %s\n", comp_h->callback_state.omx_handle,
            comp_h->callback_state.component_name, *ppBuffer, nPortIndex, pAppPrivate, nSizeBytes, err2str(res));
    }
    return res;
}


OMX_ERRORTYPE il_tool_log_FreeBuffer(
            il_comp_t comp_h,
            OMX_IN  OMX_U32 nPortIndex,
            OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer)
{
    ADM_ASSERT(comp_h != NULL);

    ALOG_OMXIL("%X [%-12s]: FreeBuf port=%d pBuf=%X\n", comp_h->callback_state.omx_handle, comp_h->callback_state.component_name, nPortIndex, pBuffer);
    OMX_ERRORTYPE res = OMX_FreeBuffer(comp_h->callback_state.omx_handle, nPortIndex, pBuffer);
    if (res != OMX_ErrorNone) {
        ALOG_ERR("%X [%-12s]: FreeBuf port=%d pBuf=%X %s\n", comp_h->callback_state.omx_handle, comp_h->callback_state.component_name, nPortIndex, pBuffer, err2str(res));
    }

    return res;
}
    OMX_ERRORTYPE il_tool_log_GetComponentVersion(
            il_comp_t comp_h,
            OMX_OUT OMX_STRING pComponentName,
            OMX_OUT OMX_VERSIONTYPE* pComponentVersion,
            OMX_OUT OMX_VERSIONTYPE* pSpecVersion,
            OMX_OUT OMX_UUIDTYPE* pComponentUUID)
{
    ADM_ASSERT(comp_h != NULL);

    OMX_ERRORTYPE res = OMX_GetComponentVersion(comp_h->callback_state.omx_handle,
        pComponentName, pComponentVersion, pSpecVersion, pComponentUUID);

    log_wrapper_verbose(res, "%X [%-12s]: GetComponentVersion --> %s\n", comp_h->callback_state.omx_handle, comp_h->callback_state.component_name, err2str(res));
    return res;
}

    OMX_ERRORTYPE il_tool_log_GetState(
            il_comp_t comp_h,
            OMX_OUT OMX_STATETYPE* pState)
{
    ADM_ASSERT(comp_h != NULL);
    ADM_ASSERT(pState != NULL);

    OMX_ERRORTYPE res = OMX_GetState(comp_h->callback_state.omx_handle,pState);
    log_wrapper_verbose(res, "%X [%-12s]: GetState --> %s, %s\n", comp_h->callback_state.omx_handle, comp_h->callback_state.component_name, state2str(*pState), err2str(res));
    return res;
}

    OMX_ERRORTYPE il_tool_log_UseBuffer(
            il_comp_t comp_h,
            OMX_INOUT OMX_BUFFERHEADERTYPE** ppBufferHdr,
            OMX_IN OMX_U32 nPortIndex,
            OMX_IN OMX_PTR pAppPrivate,
            OMX_IN OMX_U32 nSizeBytes,
            OMX_IN OMX_U8* pBuffer)
{
    ADM_ASSERT(comp_h != NULL);

    ALOG_OMXIL("%X [%-12s]: UseBuffer hdr=%X port=%d priv=%X bytes=%d buf=%X", comp_h->callback_state.omx_handle,
        comp_h->callback_state.component_name, *ppBufferHdr, nPortIndex, pAppPrivate, nSizeBytes, (int) pBuffer);
    OMX_ERRORTYPE res = OMX_UseBuffer(comp_h->callback_state.omx_handle,
        ppBufferHdr, nPortIndex, pAppPrivate, nSizeBytes, pBuffer);
    if (res != OMX_ErrorNone) {
        ALOG_ERR("%X [%-12s]: UseBuffer hdr=%X port=%d priv=%X bytes=%d buf=%X %s", comp_h->callback_state.omx_handle,
            comp_h->callback_state.component_name, *ppBufferHdr, nPortIndex, pAppPrivate, nSizeBytes, (int) pBuffer, err2str(res));
    }

    return res;
}




OMX_API OMX_ERRORTYPE OMX_APIENTRY il_tool_log_FreeHandle(il_comp_t comp_h)
{
    log_wrapper(OMX_ErrorNone, "%X [%-12s]: FreeHandle\n", comp_h->callback_state.omx_handle, comp_h->callback_state.component_name);
    OMX_ERRORTYPE res = OMX_ErrorBadParameter;

    if (adaptions_internal_mixsplit()) {
        res = adm_omxcore_OMX_FreeHandle(comp_h->callback_state.omx_handle);
    }

    if (res == OMX_ErrorBadParameter) {
        res = OMX_FreeHandle(comp_h->callback_state.omx_handle);
    }

    return res;
}

OMX_API OMX_ERRORTYPE OMX_APIENTRY il_tool_log_SetupTunnel(
    il_comp_t out_comp_h,
    OMX_IN  OMX_U32 nPortOutput,
    il_comp_t in_comp_h,
    OMX_IN  OMX_U32 nPortInput)
{
    ADM_ASSERT(out_comp_h != NULL);
    ADM_ASSERT(in_comp_h != NULL);

    const char* in_name_p  = in_comp_h->callback_state.component_name;
    const char* out_name_p = out_comp_h->callback_state.component_name;

    OMX_ERRORTYPE res = OMX_SetupTunnel(out_comp_h->callback_state.omx_handle,
                                      nPortOutput,
                                      in_comp_h->callback_state.omx_handle,
                                      nPortInput);

    log_wrapper(res, "SetupTunnel %X[%s].%u -> %X[%s].%u --> %s\n",
        out_comp_h->callback_state.omx_handle,
        out_name_p,
        nPortOutput,
        in_comp_h->callback_state.omx_handle,
        in_name_p,
        nPortInput,
        err2str(res));
    return res;
}


#define LOGBUF_ENTRIES  64
#define LOGBUF_LINESIZE 150
static pthread_mutex_t logbuf_mutex;
static int logbuf_curpos;
static char logbuf_buf[LOGBUF_ENTRIES][LOGBUF_LINESIZE];

void ste_adm_debug_logbuf_init()
{
    pthread_mutex_init(&logbuf_mutex, NULL);
    logbuf_curpos = 0;
    memset(logbuf_buf, 0, LOGBUF_ENTRIES * LOGBUF_LINESIZE);
}

static void ste_adm_debug_logbuf_printf(const char* fmt, ...)
{
    pthread_mutex_lock(&logbuf_mutex);

    struct timeval tv;
    gettimeofday(&tv, 0);
    time_t tt = tv.tv_sec;
    struct tm tm;
    localtime_r(&tt, &tm);
    int offs=sprintf(logbuf_buf[logbuf_curpos], "[%02d-%02d %02d:%02d:%02d.%03d] ", tm.tm_mon+1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, (int) (tv.tv_usec / 1000));

    va_list ap;
    va_start(ap, fmt);
    vsnprintf(logbuf_buf[logbuf_curpos]+offs, sizeof(logbuf_buf[logbuf_curpos])-offs, fmt, ap);
    va_end(ap);
    logbuf_curpos = (logbuf_curpos+1) % LOGBUF_ENTRIES;
    pthread_mutex_unlock(&logbuf_mutex);
}

void ste_adm_debug_logbuf_show()
{
    pthread_mutex_lock(&logbuf_mutex);
    int i;
    for (i=0 ; i < LOGBUF_ENTRIES ; i++) {
        int j = (i + logbuf_curpos) % LOGBUF_ENTRIES;
        if (logbuf_buf[j][0]) {
            ALOG_WARN("Saved %s", logbuf_buf[j]); // \n already in logbuf_buf
        }
    }
    pthread_mutex_unlock(&logbuf_mutex);
}




#endif // ADM_LOG_STANDALONE

