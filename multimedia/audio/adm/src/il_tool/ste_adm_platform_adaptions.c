/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*! \file ste_adm_cmd.h
*   \brief Platform specific adaptions.

    This header exposes functionality for handling platform specific adaptions.
*/

#include <string.h>
#define ADM_LOG_FILENAME "pl_adapt"

#include "ste_adm_platform_adaptions.h"
#include "ste_adm_dbg.h"
#include "AFM_Index.h"
#include "AFM_Types.h"
#include "audio_chipset_api_index.h"
#include "audio_render_chipset_api.h"
#include "omx_ste_speech_proc.h"
#include "ste_adm_omx_tool.h"
#include "OMX_DRCExt.h"
#include "OMX_CoreExt.h"
#include "OMX_IndexExt.h"
#ifndef ADM_DBG_X86
  #include "cutils/properties.h"
#endif

#ifndef ADM_NO_EFFECT_CONFIG_SUPPORT
    #include "OMX_STE_VirtualSurroundExt.h"
    #include "omx_ste_noise_reduction.h"
#endif

void force_standard_tunneling(il_comp_t      comp_h)
{
    static const OMX_INDEXTYPE port_type_index[4] = {
        OMX_IndexParamAudioInit,   OMX_IndexParamImageInit,
        OMX_IndexParamVideoInit,   OMX_IndexParamOtherInit
    };

    ALOG_INFO("Setting all ports on %X to standard tunneling\n", comp_h);

    int type_idx;
    for (type_idx=0 ; type_idx < 4 ; type_idx++) {
        OMX_PORT_PARAM_TYPE port_param;
        port_param.nSize             = sizeof(OMX_PORT_PARAM_TYPE);
        port_param.nVersion.nVersion = OMX_VERSION;

        ste_adm_res_t res = il_tool_GetParameter(comp_h,
                    port_type_index[type_idx], &port_param);
        if (res != STE_ADM_RES_OK) {
            ALOG_ERR("GetParameter failed while setting standard tunneling\n");
            return;
        }

        OMX_U32 first_port = port_param.nStartPortNumber;
        OMX_U32 last_port  = port_param.nStartPortNumber + port_param.nPorts;

        // Go through all ports ([first_port,last_port]) and disable them
        // unless they are in the ports_to_keep_enabled list.
        OMX_U32 port_idx;
        for (port_idx=first_port ; port_idx < last_port ; port_idx++) {
            AFM_PARAM_FORCE_STANDARD_TUNNELING fst;
            IL_TOOL_INIT_CONFIG_STRUCT(fst);
            fst.nPortIndex = port_idx;
            fst.bIsStandardTunneling = OMX_TRUE;
            res = il_tool_SetParameter(comp_h, AFM_IndexParamForceStandardTunneling, &fst);
            if (res != STE_ADM_RES_OK) {
                ALOG_ERR("SetParameter failed while setting standard tunneling\n");
                return;
            }
        }
    }
}

OMX_INDEXTYPE adaptions_string_to_index(const char* str)
{
    if (strcmp(str, "AFM_IndexParamMspNbChannel") == 0) {
        return AFM_IndexParamMspNbChannel;
    } else if (strcmp(str, "OMX_IndexConfigAudioVolume") == 0) {
        return OMX_IndexConfigAudioVolume;
    } else if (strcmp(str, "OMX_IndexConfigAudioMute") == 0) {
        return OMX_IndexConfigAudioMute;
    } else if (strcmp(str, "OMX_IndexConfigAudioMdrc") == 0) {
        return OMX_IndexConfigAudioMdrc;
    } else if (strcmp(str, "OMX_IndexParamAudioPcm") == 0) {
        return OMX_IndexParamAudioPcm;
    } else if (strcmp(str, "AFM_IndexParamDmaChannelType") == 0) {
        return AFM_IndexParamDmaChannelType;
    } else if (strcmp(str, "OMX_STE_IndexConfigAudioTxSpeechEnhancement") == 0) {
        return OMX_STE_IndexConfigAudioTxSpeechEnhancement;
    } else if (strcmp(str, "OMX_STE_IndexConfigAudioRxSpeechEnhancement") == 0) {
        return OMX_STE_IndexConfigAudioRxSpeechEnhancement;
    } else if (strcmp(str, "OMX_STE_IndexConfigAudioTxDRC") == 0) {
        return OMX_STE_IndexConfigAudioTxDRC;
    } else if (strcmp(str, "OMX_STE_IndexConfigAudioRxDRC") == 0) {
        return OMX_STE_IndexConfigAudioRxDRC;
    } else if (strcmp(str, "OMX_STE_IndexConfigAudioTxCTM") == 0) {
        return OMX_STE_IndexConfigAudioTxCTM;
    } else if (strcmp(str, "OMX_STE_IndexConfigAudioRxCTM") == 0) {
        return OMX_STE_IndexConfigAudioRxCTM;
    } else if (strcmp(str, "OMX_IndexConfigALSADevice") == 0) {
       return OMX_IndexConfigALSADevice;
    } else if (strcmp(str, "OMX_IndexParamAudioDRC") == 0) {
        return OMX_IndexParamAudioDRC;
    } else if (strcmp(str, "OMX_IndexConfigAudioDRC") == 0) {
        return OMX_IndexConfigAudioDRC;
    } else if (strcmp(str, "OMX_IndexConfigCallbackRequest") == 0) {
        return OMX_IndexConfigCallbackRequest;
    } else if (strcmp(str, "AFM_IndexConfigPcmProbe") == 0) {
        return AFM_IndexConfigPcmProbe;
    }
#ifdef ADM_MMPROBE
    else if (strcmp(str, "AFM_IndexConfigPcmProbe") == 0) {
        return AFM_IndexConfigPcmProbe;
    }
#endif // ADM_MMPROBE
#ifndef ADM_NO_EFFECT_CONFIG_SUPPORT
      else if (strcmp(str, "OMX_STE_IndexConfigAudioTransducerEqualizer") == 0) {
        return OMX_STE_IndexConfigAudioTransducerEqualizer;
    } else if (strcmp(str, "AFM_IndexParamTransducerEqualizer") == 0) {
        return AFM_IndexParamTransducerEqualizer;
    } else if (strcmp(str, "AFM_IndexConfigTransducerEqualizer") == 0) {
        return AFM_IndexConfigTransducerEqualizer;
    } else if (strcmp(str, "AFM_IndexParamSpl") == 0) {
        return AFM_IndexParamSpl;
    } else if (strcmp(str, "OMX_IndexConfigSpllimit") == 0) {
        return OMX_IndexConfigSpllimit;
    } else if (strcmp(str, "OMX_STE_IndexConfigAudioVirtualSurround") == 0) {
        return OMX_STE_IndexConfigAudioVirtualSurround;
    } else if (strcmp(str, "OMX_STE_IndexConfigAudioNoiseReduction") == 0) {
        return OMX_STE_IndexConfigAudioNoiseReduction;
    } else if (strcmp(str, "AFM_IndexParamMdrc") == 0) {
        return AFM_IndexParamMdrc;
    } else if (strcmp(str, "AFM_IndexGlobalConfigMdrc") == 0) {
        return AFM_IndexGlobalConfigMdrc;
    }
#endif

    return OMX_IndexComponentStartUnused;
}

#ifdef ADM_SUPPORT_RC_FILTER
int adm_is_rc_in_board()
{
    static int rc_is_in_board   = -1;
    if(rc_is_in_board == -1)
    {
        FILE * f_rcfilter = fopen("/sys/devices/platform/modem-hwcfg/rcfilter","r");
        if(f_rcfilter){
            if(fscanf(f_rcfilter, "%d", &rc_is_in_board)<=0){
                ALOG_ERR("Unable to read Filter RC information");
                rc_is_in_board = 0;
            }
            fclose(f_rcfilter);
            ALOG_INFO(" Filter RC is present on board : %d" , rc_is_in_board);
        }
        else
        {
            ALOG_INFO(" No file /sys/devices/platform/modem-hwcfg/rcfilter found set rc to 0 ");
            rc_is_in_board = 0;
        }
    }
    return rc_is_in_board;
}
#endif



/**
 *  Creates an OMX component and sets all platform specific configurations required.
 *  Those can be overridden in database if required.
 */
ste_adm_res_t adm_adaptions_setup_comp(
    il_comp_t      Handle,
    const char* cComponentName)
{
    ste_adm_res_t res = STE_ADM_RES_OK;
    ALOG_INFO("Enter adm_platform_adaptions_setup\n");

#ifdef STE_ADM_WORKAROUND_STANDARD_TUNNELING
    force_standard_tunneling(Handle);
#endif

    // TODO: Find out what components that require the echo reference port to be disabled
    if (strcmp(cComponentName, "OMX.ST.AFM.NOKIA_AV_SINK") == 0 ||
        strcmp(cComponentName, "OMX.ST.AFM.IHF") == 0 ||
        strcmp(cComponentName, "OMX.ST.AFM.EARPIECE") == 0 ||
        strcmp(cComponentName, "OMX.ST.AFM.BT_MONO_SINK") == 0 ||
        strcmp(cComponentName, "OMX.ST.AFM.alsasink") == 0 )
    {
        // echo reference is port 1
        if (il_tool_change_port_state(Handle, 1, 0, 0)) {
            ALOG_ERR("Failed to disable clock port on component %s\n", cComponentName);
            return STE_ADM_RES_IL_UNKNOWN_ERROR; // TODO: Error from il_tool_change_port_state? Use IL_ macros?
        }
    }
#ifdef ADM_SUPPORT_RC_FILTER
    if (strcmp(cComponentName, "OMX.ST.AFM.NOKIA_AV_SINK") == 0 ||
        strcmp(cComponentName, "OMX.ST.AFM.IHF") == 0 ||
        strcmp(cComponentName, "OMX.ST.AFM.EARPIECE") == 0 ||
        strcmp(cComponentName, "OMX.ST.AFM.NOKIA_AV_SOURCE") == 0 ||
        strcmp(cComponentName, "OMX.ST.AFM.MULTI_MIC") == 0 ||
        strcmp(cComponentName, "OMX.ST.AFM.FM_SINK") == 0 ||
        strcmp(cComponentName, "OMX.ST.AFM.FM_SOURCE") == 0)
    {
        AFM_AUDIO_PARAM_RENDERER param_renderer;
        IL_TOOL_INIT_CONFIG_STRUCT(param_renderer);
        param_renderer.nBoardRC = adm_is_rc_in_board();
        il_tool_SetConfig(  Handle,
                            AFM_IndexParamRenderer , &param_renderer);
    }
#endif
    ALOG_INFO("Leave adm_adaptions_setup_comp\n");
    return res;
}



const char* adaptions_comp_name(comp_name_id_t id)
{
    int intcomps = adaptions_internal_mixsplit();

    switch (id)
    {
        case ADM_DEFAULT_APP_MIXER_NAME:        id = ADM_ARM_MIXER_NAME; break;
        case ADM_DEFAULT_COMMON_MIXER_NAME:     id = ADM_DSP_MIXER_NAME; break;
        case ADM_DEFAULT_SPLITTER_NAME:         id = ADM_DSP_SPLITTER_NAME; break;

        default: break;
    }

    switch (id)
    {
        case ADM_ARM_MIXER_NAME:
            return intcomps ? "ADM.mixer" : "OMX.ST.AFM.mixer_host";

		case ADM_COMMON_MIXER_NAME:
				return "OMX.ST.AFM.mixer_host";

        case ADM_ARM_SPLITTER_NAME:
            return intcomps ? "ADM.splitter" : "OMX.ST.AFM.pcmsplitter_host";

        case ADM_DSP_MIXER_NAME:
            return "OMX.ST.AFM.mixer";

        case ADM_DSP_SPLITTER_NAME:
            return "OMX.ST.AFM.pcmsplitter";

        case ADM_VC_DL_SPLITTER_NAME:
            return "OMX.ST.AFM.pcmsplitter_host";

        case ADM_VC_DL_MIXER_NAME:
           if(adm_get_modem_type()!=ADM_FAT_MODEM)
             return "OMX.ST.AFM.mixer_host";
           else
             return intcomps ? "ADM.mixer" : "OMX.ST.AFM.mixer_host";

        case ADM_VC_UL_SPLITTER_NAME:
            return "OMX.ST.AFM.pcmsplitter_host";

        case ADM_VC_UL_MIXER_NAME:
           if(adm_get_modem_type()!=ADM_FAT_MODEM)
             return "OMX.ST.AFM.mixer_host";
           else
             return intcomps ? "ADM.mixer" : "OMX.ST.AFM.mixer_host";

        case ADM_VC_DICT_SPLITTER_NAME:
            return "OMX.ST.AFM.pcmsplitter_host";

        case ADM_VC_DICT_MIXER_NAME:
           if(adm_get_modem_type()!=ADM_FAT_MODEM)
             return "OMX.ST.AFM.mixer_host";
           else
             return intcomps ? "ADM.mixer" : "OMX.ST.AFM.mixer_host";

        case ADM_CSCALL_NAME:
            if((adm_get_modem_type()==ADM_FAT_MODEM)
               ||(adm_get_modem_type()==ADM_FULL_FAT_MODEM))
                return "OMX.ST.AFM.cscall_mpc";
            else
                return "OMX.ST.AFM.cscall";

        case ADM_MODEM_SOURCE_NAME:
            if(adm_get_modem_type()==ADM_FAT_MODEM)
                return "ADM.modem_src";
            else
                return "<unknown component ID>";
        case ADM_MODEM_SINK_NAME:
            if(adm_get_modem_type()==ADM_FAT_MODEM)
                return "ADM.modem_sink";
            else
                return "<unknown component ID>c";

        case ADM_SPEECHPROC_NAME:
            return "OMX.ST.AFM.speech_proc";

        default: {
            ADM_ASSERT(0);
            return "<unknown component ID>";
        }
    }
}

int adaptions_is_dsp_component(const char* name)
{
    static const char* dsp_components[] = {
        "OMX.ST.AFM.BT_MONO_SINK",
        "OMX.ST.AFM.BT_MONO_SOURCE",
        "OMX.ST.AFM.EARPIECE",
        "OMX.ST.AFM.FM_SINK",
        "OMX.ST.AFM.FM_SOURCE",
        "OMX.ST.AFM.IHF",
        "OMX.ST.AFM.mixer",
        "OMX.ST.AFM.MULTI_MIC",
        "OMX.ST.AFM.NOKIA_AV_SINK",
        "OMX.ST.AFM.NOKIA_AV_SOURCE",
        "OMX.ST.AFM.pcmprocessing.audiovirtualizer",
        "OMX.ST.AFM.pcmprocessing.comfortnoise",
        "OMX.ST.AFM.pcmprocessing.equalizer",
        "OMX.ST.AFM.pcmprocessing.mdrc",
        "OMX.ST.AFM.pcmprocessing.reverb",
        "OMX.ST.AFM.pcmprocessing.spl",
        "OMX.ST.AFM.pcmprocessing.transducer_equalizer",
        "OMX.ST.AFM.pcmprocessing.virtualsurround",
        "OMX.ST.AFM.pcmsplitter",
        "OMX.ST.HAPTICS_RENDERER.PCM.VIBL",
        "OMX.ST.HAPTICS_RENDERER.PCM.VIBR",
        NULL
    };

    static const char* arm_components[] = {
        "OMX.ST.AFM.pcmprocessing.audiovisualization",
        "OMX.ST.AFM.pcmprocessing.channelsynthesis",
        "OMX.ST.AFM.pcmprocessing.chorus",
        "OMX.ST.AFM.pcmprocessing.drc",
        "OMX.ST.AFM.pcmprocessing.noise_reduction",
        "OMX.ST.AFM.pcmprocessing.spectrum_analyzer",
        "OMX.ST.AFM.pcmprocessing.timescale",
        NULL
    };

    if (strncmp(name, "ADM.", 4) == 0) {
        return 0;
    }

    const char** cur;
    for (cur = dsp_components; *cur ; cur++) {
        if (strcmp(*cur, name) == 0) {
            return 1;
        }
    }

    for (cur = arm_components; *cur ; cur++) {
        if (strcmp(*cur, name) == 0) {
            return 1;
        }
    }

    ALOG_ERR("Deployment (ARM/DSP) not known for %s\n", name);
    return 0;
}


int adaptions_internal_mixsplit(void)
{
#ifdef ADM_DBG_X86
    return 1;
#else
    char prop[PROPERTY_VALUE_MAX];
    property_get("ste.debug.adm.intmixsplit", prop, "");
    prop[PROPERTY_VALUE_MAX-1]=0;

    if (strcmp(prop, "1") == 0)
        return 1;
    else if (strcmp(prop, "0") == 0)
        return 0;
    else
        return ADM_INTERNAL_MIXSPLIT_DEFAULT_VALUE;
#endif
}

const char* adaptions_get_mixer_internal_effect_name(const char* name)
{
    if (strcmp(name, "OMX.ST.AFM.pcmprocessing.comfortnoise") == 0 ) {
        return "audio_processor.pcm.comfortnoise";
    }
    else if (strcmp(name, "OMX.ST.AFM.pcmprocessing.mdrc") == 0 ) {
        return "audio_processor.pcm.mdrc";
    }
    else if (strcmp(name, "OMX.ST.AFM.pcmprocessing.spl") == 0 ) {
        return "audio_processor.pcm.spl";
    }
    else if (strcmp(name, "OMX.ST.AFM.pcmprocessing.transducer_equalizer") == 0 ) {
        return "audio_processor.pcm.trans_equalizer";
    }

    return NULL;
}



