/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*
*  Overview of OMX component connections in voicecall / dictaphone usecase.
*
*  Voicecall without disctaphone activated:
*
*   /--------\                                            /--------\
*   | voice  | --> [ dict_split_dl ] -------------------> | speech | --> speaker chain
*   | srcsnk |                                            | proc   | <-- echo ref.
*   |        | <-- [ dict_split_ul] <-- [dict_mix_ul] <-- |        | <-- mic chain
*   \--------/                                            \--------/
*
*  The voice srcsnk component is always EXECUTING (due to modem problems).
*
*   -----------------------------------------------------------------------
*
*    Dictaphone recording taps the dict_split_dl and dict_split_ul splitters.
*    The dictaphone audio from these taps is mixed in dict_rec_mix_h.
*
*   [dict_split_dl]  -->
*   [dict_split_ul]  --> [ dict_rec_mix_h ] --> [ADM dict rec]
*
*   -----------------------------------------------------------------------
*
*    Dictaphone playback sends data to the dict_mix_ul mixer, as well as
*    to the ADM. A splitter is dedicated to the dictaphone playback
*    usecase; dict_play_split_h.
*
*   [ADM dict play] --> [dict_play_split_h] --> [dict_mix_ul]
*                                           --> [ADM app dict play]
*/

#define ADM_LOG_FILENAME "cscall_omx"

#include "ste_adm_cscall_omx.h"
#include "ste_adm_api_cscall.h"
#include "ste_adm_omx_core.h"
#include "ste_adm_omx_tool.h" // IL-macros etc
#include "ste_adm_dev_omx_util.h" // adm_setup_component TODO: Move elsewhere, to IL tool or general util
#include "ste_adm_dbg.h"
#include "ste_adm_platform_adaptions.h" // IL component names
#include "OMX_IndexExt.h"
#include "OMX_CoreExt.h"
#include "audio_chipset_api_index.h"
#include "audio_render_chipset_api.h"
#include <ste_adm_msg.h> // msg_base_t
#include "ste_adm_client.h"
#include "ste_adm_api_modem.h"
#include "ste_adm_util.h"

#include <string.h>
#include <unistd.h>

#include "OMX_Symbian_AudioExt_Ste.h" // loopback
#include "AFM_Types.h" // loopback
#include "AFM_Index.h" // loopback

#define STE_ADM_CSCALL_SRC_SAMPLERATE (48000)
#define STE_ADM_CSCALL_SRC_NOCHAN (1)
#define STE_ADM_CSCALL_DL_NOBUFS (2)
#define STE_ADM_CSCALL_UL_NOBUFS (2)
#define STE_ADM_CSCALL_VOICE_NOCHAN (1)
#define STE_ADM_CSCALL_VOICE_SINK_INPUT_PORT (2)
#define STE_ADM_VOIP_SPEECHPROC_SAMPLERATE (16000)

// MSP modes defines
#define ADM_MSP_MODE_IIS 0
#define ADM_MSP_MODE_TDM_MASTER_RX_2_SLOTS 1
#define ADM_MSP_MODE_TDM_MASTER_RX_1_SLOT 2
#define ADM_MSP_MODE_TDM_SLAVE_RX_2_SLOTS 3
#define ADM_MSP_MODE_TDM_SLAVE_RX_1_SLOT 4

// Used for debugging; possible to enable smaller scope
// e.g. CFG_NO_DICT
#define CFG_FULL
#define CFG_ENABLE_ECHO_REFERENCE // until flag is removed totally



#define RETURN_IF_FAIL(__f) \
    { \
        SRV_DBG_ASSERT_IS_WORKER_THREAD; \
        ste_adm_res_t r = __f; \
        if (r != STE_ADM_RES_OK) { \
            ALOG_ERR("Error %d in '%s'\n", r, # __f); \
            srv_become_normal_thread(); \
            return r; \
        } \
    }

cscall_omx_state_t g_cscall_omx_state;

static void cscall_adm_context_cb(void* param, il_comp_t hComponent,
        OMX_EVENTEXTTYPE eEvent,
        OMX_U32 Data1, OMX_U32 Data2, OMX_PTR pEventData);

static ste_adm_res_t configure_cscall_comp_slim(cscall_omx_state_t* state_p);
static ste_adm_res_t configure_cscall_comp_fat(cscall_omx_state_t* state_p);
static ste_adm_res_t configure_speechproc_comp(cscall_omx_state_t* state_p, const char* input_name, const char* output_name);
static ste_adm_res_t configure_speechproc_comp_slim(cscall_omx_state_t* state_p, const char* input_name, const char* output_name);
static ste_adm_res_t configure_speechproc_comp_fat(cscall_omx_state_t* state_p, const char* input_name, const char* output_name);

il_comp_t adm_cscall_omx_get_speechproc_handle()
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    return g_cscall_omx_state.speechproc_h;
}

int adm_cscall_omx_get_samplerate()
{
    if (g_cscall_omx_state.voip_enabled) {
        return STE_ADM_VOIP_SPEECHPROC_SAMPLERATE;
    } else {
        return g_cscall_omx_state.cscall_samplerate;
    }
}

int  is_slim_graph_creation(cscall_omx_state_t* state_p)
{
    switch(adm_get_modem_type()){
        // In case of slim or half slim modem , speechproc is always used, so full graph is used
        case ADM_SLIM_MODEM:
        case ADM_HALF_SLIM_MODEM:
            return 1;

        // In case of FAT modem, speech proc is not used in case of CSCALL , but is it used in case of Voip
        case ADM_FAT_MODEM:
            if(state_p->voip_enabled)
                return 1;
            else
                return 0;

        default :
            ALOG_ERR("Bad Modem Type");
            return 0;
    }
    return 0;
}

ste_adm_res_t adm_cscall_create_cscall_cmp_slim(cscall_omx_state_t* state_p)
{

    // NOTE: In case of error during execution of an IL_TOOL_ macro,
    // it will set the 'result' variable below to the resulting error
    // code and then goto cleanup.
    ste_adm_res_t res = STE_ADM_RES_OK;

    // Temporarilly disable the mechanism in IL tool that causes the
    // ADM to reboot when an error occurs.
    il_tool_set_state_transition_timeout(30);

    IL_TOOL_CREATE(&(state_p->voice_srcsnk_h), adaptions_comp_name(ADM_CSCALL_NAME), "cscall");


    // request callbacks when speech codec is enabled/disabled
    OMX_CONFIG_CALLBACKREQUESTTYPE callback_config;
    IL_TOOL_INIT_CONFIG_STRUCT(callback_config);
    callback_config.nPortIndex = OMX_ALL;
    callback_config.bEnable    = OMX_TRUE;
    callback_config.nIndex     = OMX_Symbian_IndexConfigAudioCodecEnabled;
    IL_TOOL_SET_CONFIG(
            state_p->voice_srcsnk_h,
            OMX_IndexConfigCallbackRequest,
            &callback_config);


    callback_config.nIndex = OMX_Symbian_IndexConfigAudioCodecDisabled;
    IL_TOOL_SET_CONFIG(state_p->voice_srcsnk_h, OMX_IndexConfigCallbackRequest, &callback_config);

    il_tool_subscribe_events(state_p->voice_srcsnk_h, cscall_adm_context_cb, NULL, 1);

    // set line priority
    OMX_PARAM_LINEPRIORITYTYPE line_prio;
    IL_TOOL_INIT_CONFIG_STRUCT(line_prio);
    line_prio.nPortIndex = ADM_CSCALL_UL_PORT;
    line_prio.ePortPriority = OMX_PriorityTelephonyUL_2;
    IL_TOOL_SET_PARAM(state_p->voice_srcsnk_h,
        OMX_IndexParamLinePriority, &line_prio);
    line_prio.nPortIndex = ADM_CSCALL_DL_PORT;
    line_prio.ePortPriority = OMX_PriorityTelephonyDL;
    IL_TOOL_SET_PARAM(state_p->voice_srcsnk_h,
        OMX_IndexParamLinePriority, &line_prio);
    IL_TOOL_DISABLE_ALL_PORTS(state_p->voice_srcsnk_h);
    IL_TOOL_ENTER_STATE(OMX_StateIdle,      state_p->voice_srcsnk_h);
    IL_TOOL_ENTER_STATE(OMX_StateExecuting, state_p->voice_srcsnk_h);

cleanup:

    il_tool_set_state_transition_timeout(0);
    return res;
}

ste_adm_res_t adm_cscall_create_cscall_cmp_fat(cscall_omx_state_t* state_p)
{
    // NOTE: In case of error during execution of an IL_TOOL_ macro,
    // it will set the 'result' variable below to the resulting error
    // code and then goto cleanup.
    ste_adm_res_t res = STE_ADM_RES_OK;

    // Temporarilly disable the mechanism in IL tool that causes the
    // ADM to reboot when an error occurs.
    il_tool_set_state_transition_timeout(30);

    IL_TOOL_CREATE(&state_p->voice_srcsnk_h, adaptions_comp_name(ADM_CSCALL_NAME), "cscall");

    AFM_AUDIO_PARAM_CSCALL_MPC cscall_mpc;
    IL_TOOL_INIT_CONFIG_STRUCT(cscall_mpc);
    cscall_mpc.nDuration = 5000.;
    cscall_mpc.nSamplingRate = 16000;
    cscall_mpc.nLoopbackMode = state_p->loopback_enabled;
    // Configure MSP in TDM master mode, 1 slot active on downlink, 4 slots active on uplink
    cscall_mpc.nMspMode = ADM_MSP_MODE_TDM_MASTER_RX_1_SLOT;

    IL_TOOL_SET_PARAM(  state_p->voice_srcsnk_h,
                            AFM_IndexParamCsCallMpc, &cscall_mpc);

    IL_TOOL_DISABLE_ALL_PORTS(state_p->voice_srcsnk_h);
    IL_TOOL_ENTER_STATE(OMX_StateIdle,      state_p->voice_srcsnk_h);
    IL_TOOL_ENTER_STATE(OMX_StateExecuting, state_p->voice_srcsnk_h);

cleanup:

    il_tool_set_state_transition_timeout(0);
    return res;
}


///////////////////////////////////////////////////////////////////////////////
//
//  init_cscall
//
//  Called when the ADM boots. Loads the cscall component, disables the ports,
//  and brings it to executing state, which it never leaves. Due to modem
//  issues, cscall component must always be in executing state.
//
//
///////////////////////////////////////////////////////////////////////////////
ste_adm_res_t adm_cscall_omx_init_cscall(void(*codec_enabled_fp)(),
    void(*codec_disabled_fp)(), void(*cscall_samplerate_fp)())
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    ste_adm_res_t res = STE_ADM_RES_OK;

    ADM_ASSERT(codec_enabled_fp  != NULL);
    ADM_ASSERT(codec_disabled_fp != NULL);
    ADM_ASSERT(cscall_samplerate_fp != NULL);

    g_cscall_omx_state.codec_enabled_fp      = codec_enabled_fp;
    g_cscall_omx_state.codec_disabled_fp     = codec_disabled_fp;
    g_cscall_omx_state.cscall_samplerate_fp  = cscall_samplerate_fp;

    ALOG_INFO("OMX_Symbian_IndexConfigAudioCodecEnabled = %X\n", OMX_Symbian_IndexConfigAudioCodecEnabled);
    ALOG_INFO("OMX_Symbian_IndexConfigAudioCodecDisabled = %X\n", OMX_Symbian_IndexConfigAudioCodecDisabled);

    ALOG_INFO("IL TRANSITIONS - init_cscall()\n");


#ifdef CFG_FULL
    ALOG_STATUS("Speech mode: CFG_FULL\n");
#endif

#ifdef CFG_CSCALL_ONLY
    ALOG_STATUS("Speech mode: CFG_CSCALL_ONLY\n");
#endif

#ifdef CFG_NO_DICT
    ALOG_STATUS("Speech mode: CFG_NO_DICT\n");
#endif

#ifdef CFG_EXPERIMENTAL
    ALOG_STATUS("Speech mode: CFG_EXPERIMENTAL\n");
#endif

    switch(adm_get_modem_type()){
        case ADM_SLIM_MODEM:
            ALOG_STATUS("Speech mode: Slim Modem");
            // In case of Slim Modem, the Cscall component must be activated at the init
            res = adm_cscall_create_cscall_cmp_slim(&g_cscall_omx_state);
            break;
        case ADM_HALF_SLIM_MODEM:
            ALOG_STATUS("Speech mode: Half Slim Modem\n");
            res = modem_init();
            break;
        case ADM_FAT_MODEM:
        case ADM_FULL_FAT_MODEM:
            ALOG_STATUS("Speech mode: Fat Modem\n");
            res = modem_init();
            break;
        case ADM_UNKNOWN_MODEM_TYPE:
            ALOG_STATUS("Speech mode: Unknown Modem\n");
            break;
    }

    return res ;

}

ste_adm_res_t adm_cscall_omx_destroy_cscall()
{

    // NOTE: In case of error during execution of an IL_TOOL_ macro,
    // it will set the 'result' variable below to the resulting error
    // code and then goto cleanup.
    ste_adm_res_t res = STE_ADM_RES_OK;

    // Do not become worker thread; this is done at module exit
    if (g_cscall_omx_state.voice_srcsnk_h) {
    IL_TOOL_ENTER_STATE(OMX_StateIdle,   g_cscall_omx_state.voice_srcsnk_h);
    IL_TOOL_ENTER_STATE(OMX_StateLoaded, g_cscall_omx_state.voice_srcsnk_h);
    IL_TOOL_DESTROY(&g_cscall_omx_state.voice_srcsnk_h);
    }

cleanup:
    return res;
}




///////////////////////////////////////////////////////////////////////////////
//
//  activate_cscall
//
//  Create the speech component and makes all connections.
//
///////////////////////////////////////////////////////////////////////////////
ste_adm_res_t adm_cscall_omx_activate_cscall(const char* in_dev, const char* out_dev)
{
    ALOG_INFO("adm_cscall_omx_activate_cscall\n");
    if(is_slim_graph_creation(&g_cscall_omx_state))
        return adm_cscall_omx_activate_cscall_slim(in_dev,out_dev);
    else
        return adm_cscall_omx_activate_cscall_fat(in_dev,out_dev);
}
ste_adm_res_t adm_cscall_omx_activate_cscall_slim(const char* in_dev, const char* out_dev)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;


    cscall_omx_state_t old_state = g_cscall_omx_state;
    cscall_omx_state_t new_state = old_state;

    // NOTE: In case of error during execution of an IL_TOOL_ macro,
    // it will set the 'result' variable below to the resulting error
    // code and then goto cleanup.
    ste_adm_res_t res = STE_ADM_RES_OK;
    srv_become_worker_thread();

    OMX_AUDIO_PARAM_PCMMODETYPE pcm_mode;
    IL_TOOL_INIT_CONFIG_STRUCT(pcm_mode);

    OMX_AUDIO_PARAM_PCMMODETYPE pcm_mode_voip;
    IL_TOOL_INIT_CONFIG_STRUCT(pcm_mode_voip);

    OMX_PARAM_LINEPRIORITYTYPE line_prio;
    IL_TOOL_INIT_CONFIG_STRUCT(line_prio);

    if (!new_state.voip_enabled) {
        res = configure_cscall_comp_slim(&new_state);
        RETURN_IF_FAIL(res);
    }

    IL_TOOL_CREATE(&new_state.speechproc_h, adaptions_comp_name(ADM_SPEECHPROC_NAME), "speechproc");
    IL_TOOL_DISABLE_PORT(new_state.speechproc_h, ADM_SPEECHPROC_UL_IN_PORT);
    IL_TOOL_DISABLE_PORT(new_state.speechproc_h, ADM_SPEECHPROC_DL_IN_PORT);
    IL_TOOL_DISABLE_PORT(new_state.speechproc_h, ADM_SPEECHPROC_REF_PORT);
    IL_TOOL_DISABLE_PORT(new_state.speechproc_h, ADM_SPEECHPROC_UL_OUT_PORT);
    IL_TOOL_DISABLE_PORT(new_state.speechproc_h, ADM_SPEECHPROC_DL_OUT_PORT);

    RETURN_IF_FAIL(configure_speechproc_comp(&new_state, in_dev, out_dev));

    // set line priority
    line_prio.nPortIndex = ADM_SPEECHPROC_UL_IN_PORT;
    line_prio.ePortPriority = OMX_PriorityNormal; /* DSP port : normal priority */
    IL_TOOL_SET_PARAM(new_state.speechproc_h,
        OMX_IndexParamLinePriority, &line_prio);

    line_prio.nPortIndex = ADM_SPEECHPROC_REF_PORT;
    line_prio.ePortPriority = OMX_PriorityNormal; /* DSP port : normal priority */
    IL_TOOL_SET_PARAM(new_state.speechproc_h,
        OMX_IndexParamLinePriority, &line_prio);

    line_prio.nPortIndex = ADM_SPEECHPROC_DL_OUT_PORT;
    line_prio.ePortPriority = OMX_PriorityNormal; /* DSP port : normal priority */
    IL_TOOL_SET_PARAM(new_state.speechproc_h,
        OMX_IndexParamLinePriority, &line_prio);

    line_prio.nPortIndex = ADM_SPEECHPROC_UL_OUT_PORT;
    line_prio.ePortPriority = OMX_PriorityTelephonyUL; /* ARM port : UL port */
    IL_TOOL_SET_PARAM(new_state.speechproc_h,
        OMX_IndexParamLinePriority, &line_prio);

    line_prio.nPortIndex = ADM_SPEECHPROC_DL_IN_PORT;
    line_prio.ePortPriority = OMX_PriorityTelephonyDL; /* ARM port : DL port */
    IL_TOOL_SET_PARAM(new_state.speechproc_h,
        OMX_IndexParamLinePriority, &line_prio);

    IL_TOOL_ENTER_STATE(OMX_StateIdle,      new_state.speechproc_h);
    IL_TOOL_ENTER_STATE(OMX_StateExecuting, new_state.speechproc_h);

    /* pcm data */
    ALOG_INFO("Setting PCM parameters for speech proc\n");
    pcm_mode.nPortIndex     = ADM_SPEECHPROC_DL_IN_PORT;
    IL_TOOL_GET_PARAM(new_state.speechproc_h,OMX_IndexParamAudioPcm,&pcm_mode);

    pcm_mode.nChannels      = STE_ADM_CSCALL_VOICE_NOCHAN;
    pcm_mode.eEndian        = OMX_EndianLittle;
    pcm_mode.nSamplingRate  = (OMX_U32) new_state.cscall_samplerate;
    IL_TOOL_SET_PARAM(new_state.speechproc_h,OMX_IndexParamAudioPcm,&pcm_mode);

    pcm_mode.nPortIndex     = ADM_SPEECHPROC_UL_IN_PORT;
    IL_TOOL_GET_PARAM(new_state.speechproc_h,OMX_IndexParamAudioPcm,&pcm_mode);

    pcm_mode.nChannels      = STE_ADM_CSCALL_VOICE_NOCHAN;
    pcm_mode.eEndian        = OMX_EndianLittle;
    pcm_mode.nSamplingRate  = (OMX_U32) new_state.cscall_samplerate;
    IL_TOOL_SET_PARAM(new_state.speechproc_h,OMX_IndexParamAudioPcm,&pcm_mode);

    pcm_mode.nPortIndex     = ADM_SPEECHPROC_REF_PORT;
    IL_TOOL_GET_PARAM(new_state.speechproc_h,OMX_IndexParamAudioPcm,&pcm_mode);

    pcm_mode.nChannels      = STE_ADM_CSCALL_VOICE_NOCHAN;
    pcm_mode.eEndian        = OMX_EndianLittle;
    pcm_mode.nSamplingRate  = (OMX_U32) new_state.cscall_samplerate;
    pcm_mode.eChannelMapping[0] = OMX_AUDIO_ChannelCF; //needs to be set for device suspend/resume
    IL_TOOL_SET_PARAM(new_state.speechproc_h,OMX_IndexParamAudioPcm,&pcm_mode);


    pcm_mode.nPortIndex = ADM_SPEECHPROC_UL_OUT_PORT;
    IL_TOOL_GET_PARAM(new_state.speechproc_h,OMX_IndexParamAudioPcm,&pcm_mode);

    pcm_mode.nChannels     = STE_ADM_CSCALL_VOICE_NOCHAN;
    pcm_mode.eEndian       = OMX_EndianLittle;
    pcm_mode.nSamplingRate = (OMX_U32) new_state.cscall_samplerate;
    IL_TOOL_SET_PARAM(new_state.speechproc_h,OMX_IndexParamAudioPcm,&pcm_mode);


    pcm_mode.nPortIndex = ADM_SPEECHPROC_DL_OUT_PORT;
    IL_TOOL_GET_PARAM(new_state.speechproc_h,OMX_IndexParamAudioPcm,&pcm_mode);

    pcm_mode.nChannels     = STE_ADM_CSCALL_VOICE_NOCHAN;
    pcm_mode.eEndian       = OMX_EndianLittle;
    pcm_mode.nSamplingRate = (OMX_U32) new_state.cscall_samplerate;
    IL_TOOL_SET_PARAM(new_state.speechproc_h,OMX_IndexParamAudioPcm,&pcm_mode);


#ifdef CFG_NO_DICT
    if (new_state.voip_enabled) {
        /* Setup VoIP path */
        IL_TOOL_CREATE(&new_state.dict_mix_ul_h, adaptions_comp_name(ADM_VC_UL_MIXER_NAME), "ulmix");

        pcm_mode_voip.nPortIndex = MIXER_OUTPUT_PORT;
        IL_TOOL_GET_PARAM(new_state.dict_mix_ul_h, OMX_IndexParamAudioPcm, &pcm_mode_voip);
        pcm_mode_voip.nSamplingRate = new_state.voip_ul_samplerate;
        pcm_mode_voip.nChannels = new_state.voip_ul_channels;
        if (pcm_mode_voip.nChannels == 1) {
            pcm_mode_voip.eChannelMapping[0] = OMX_AUDIO_ChannelCF;
        } else if (pcm_mode_voip.nChannels == 2) {
            pcm_mode_voip.eChannelMapping[0] = OMX_AUDIO_ChannelLF;
            pcm_mode_voip.eChannelMapping[1] = OMX_AUDIO_ChannelRF;
        }

        ALOG_INFO("VoIP UL, samplerate=%d, channels=%d\n", pcm_mode_voip.nSamplingRate, pcm_mode_voip.nChannels);
        IL_TOOL_SET_PARAM(new_state.dict_mix_ul_h, OMX_IndexParamAudioPcm, &pcm_mode_voip);

        IL_TOOL_DISABLE_ALL_PORTS(new_state.dict_mix_ul_h);
        IL_TOOL_CONNECT_AND_ENABLE_WITH_SRC_CFG(
              new_state.speechproc_h,     ADM_SPEECHPROC_UL_OUT_PORT,
              new_state.dict_mix_ul_h,    MIXER_INPUT_PORT);

        pcm_mode_voip.nPortIndex = MIXER_INPUT_PORT;
        IL_TOOL_GET_PARAM(new_state.voip_mix_dl_h, OMX_IndexParamAudioPcm, &pcm_mode_voip);
        pcm_mode_voip.nSamplingRate = new_state.voip_dl_samplerate;
        pcm_mode_voip.nChannels = new_state.voip_dl_channels;
        if (pcm_mode_voip.nChannels == 1) {
            pcm_mode_voip.eChannelMapping[0] = OMX_AUDIO_ChannelCF;
        } else if (pcm_mode_voip.nChannels == 2) {
            pcm_mode_voip.eChannelMapping[0] = OMX_AUDIO_ChannelLF;
            pcm_mode_voip.eChannelMapping[1] = OMX_AUDIO_ChannelRF;
        }
        ALOG_INFO("VoIP DL, samplerate=%d, channels=%d\n", pcm_mode_voip.nSamplingRate, pcm_mode_voip.nChannels);
        IL_TOOL_SET_PARAM(new_state.voip_mix_dl_h, OMX_IndexParamAudioPcm, &pcm_mode_voip);

        IL_TOOL_DISABLE_ALL_PORTS(new_state.);
        IL_TOOL_CONNECT_AND_ENABLE_WITH_SRC_CFG(
              new_state.voip_mix_dl_h,    MIXER_OUTPUT_PORT,
              new_state.speechproc_h,     ADM_SPEECHPROC_DL_IN_PORT);

        IL_TOOL_ENTER_STATE(OMX_StateIdle,      new_state.dict_mix_ul_h);
        IL_TOOL_ENTER_STATE(OMX_StateExecuting, new_state.dict_mix_ul_h);

        IL_TOOL_ENTER_STATE(OMX_StateIdle,      new_state.voip_mix_dl_h);
        IL_TOOL_ENTER_STATE(OMX_StateExecuting, new_state.voip_mix_dl_h);
    } else {
        IL_TOOL_CONNECT_AND_ENABLE_WITH_SRC_CFG(
            new_state.voice_srcsnk_h,       ADM_CSCALL_DL_PORT,
            new_state.speechproc_h,         ADM_SPEECHPROC_DL_IN_PORT);

        IL_TOOL_CONNECT_AND_ENABLE_WITH_DST_CFG(
            new_state.speechproc_h,         ADM_SPEECHPROC_UL_OUT_PORT,
            new_state.voice_srcsnk_h,       ADM_CSCALL_UL_PORT);
    }
#endif


#ifdef CFG_FULL
    // Connect the full usecase, including both dictaphone paths
    // and speechproc. The cscall component is in executing state,
    // with both ports disabled. The 'remote' (ADM) components that
    // will be connected to the speechproc are also in executing state.

    //
    // Uplink, cscall side
    //
    IL_TOOL_CREATE(&new_state.dict_mix_ul_h,   adaptions_comp_name(ADM_VC_UL_MIXER_NAME), "ulmix");
    IL_TOOL_CREATE(&new_state.dict_split_ul_h, adaptions_comp_name(ADM_VC_UL_SPLITTER_NAME), "ulsplit");

    pcm_mode.nPortIndex = ADM_SPEECHPROC_UL_OUT_PORT;
    IL_TOOL_GET_PARAM(new_state.speechproc_h,OMX_IndexParamAudioPcm,&pcm_mode);

    if (new_state.voip_enabled) {
        /* set sampling rate requested by voip application */
        pcm_mode_voip.nPortIndex = MIXER_OUTPUT_PORT;
        IL_TOOL_GET_PARAM(new_state.dict_mix_ul_h, OMX_IndexParamAudioPcm, &pcm_mode_voip);
        pcm_mode_voip.nSamplingRate = new_state.voip_ul_samplerate;
        pcm_mode_voip.nChannels = new_state.voip_ul_channels;

        if (pcm_mode_voip.nChannels == 1) {
            pcm_mode_voip.eChannelMapping[0] = OMX_AUDIO_ChannelCF;
        } else if (pcm_mode_voip.nChannels == 2) {
            pcm_mode_voip.eChannelMapping[0] = OMX_AUDIO_ChannelLF;
            pcm_mode_voip.eChannelMapping[1] = OMX_AUDIO_ChannelRF;
        }

        ALOG_INFO("VoIP UL, samplerate=%d, channels=%d\n", pcm_mode_voip.nSamplingRate, pcm_mode_voip.nChannels);
        pcm_mode_voip.nPortIndex = SPLITTER_OUTPUT_PORT;
        IL_TOOL_SET_PARAM(new_state.dict_split_ul_h, OMX_IndexParamAudioPcm, &pcm_mode_voip);
        pcm_mode_voip.nPortIndex = SPLITTER_DICT_OUTPUT_PORT;
        IL_TOOL_SET_PARAM(new_state.dict_split_ul_h, OMX_IndexParamAudioPcm, &pcm_mode_voip);
        pcm_mode_voip.nPortIndex = SPLITTER_INPUT_PORT;
        IL_TOOL_SET_PARAM(new_state.dict_split_ul_h, OMX_IndexParamAudioPcm, &pcm_mode_voip);
        pcm_mode_voip.nPortIndex = MIXER_OUTPUT_PORT;
        IL_TOOL_SET_PARAM(new_state.dict_mix_ul_h, OMX_IndexParamAudioPcm, &pcm_mode_voip);
    }
    else
    {
      pcm_mode.nPortIndex = SPLITTER_OUTPUT_PORT;
      IL_TOOL_SET_PARAM(new_state.dict_split_ul_h, OMX_IndexParamAudioPcm, &pcm_mode);
      pcm_mode.nPortIndex = SPLITTER_DICT_OUTPUT_PORT;
      IL_TOOL_SET_PARAM(new_state.dict_split_ul_h, OMX_IndexParamAudioPcm, &pcm_mode);
      pcm_mode.nPortIndex = SPLITTER_INPUT_PORT;
      IL_TOOL_SET_PARAM(new_state.dict_split_ul_h, OMX_IndexParamAudioPcm, &pcm_mode);
      pcm_mode.nPortIndex = MIXER_OUTPUT_PORT;
      IL_TOOL_SET_PARAM(new_state.dict_mix_ul_h, OMX_IndexParamAudioPcm, &pcm_mode);
    }

    pcm_mode.nPortIndex = MIXER_INPUT_PORT;
    IL_TOOL_SET_PARAM(new_state.dict_mix_ul_h, OMX_IndexParamAudioPcm, &pcm_mode);
    pcm_mode.nPortIndex = 2;
    IL_TOOL_SET_PARAM(new_state.dict_mix_ul_h, OMX_IndexParamAudioPcm, &pcm_mode);

    // set line priority
    line_prio.nPortIndex = SPLITTER_INPUT_PORT;
    line_prio.ePortPriority = OMX_PriorityTelephonyUL;
    IL_TOOL_SET_PARAM(new_state.dict_split_ul_h,
        OMX_IndexParamLinePriority, &line_prio);

    line_prio.nPortIndex = SPLITTER_OUTPUT_PORT;
    line_prio.ePortPriority = OMX_PriorityTelephonyUL;
    IL_TOOL_SET_PARAM(new_state.dict_split_ul_h,
        OMX_IndexParamLinePriority, &line_prio);

    line_prio.nPortIndex = MIXER_INPUT_PORT;
    line_prio.ePortPriority = OMX_PriorityTelephonyUL;
    IL_TOOL_SET_PARAM(new_state.dict_mix_ul_h,
        OMX_IndexParamLinePriority, &line_prio);

    line_prio.nPortIndex = MIXER_OUTPUT_PORT;
    line_prio.ePortPriority = OMX_PriorityTelephonyUL;
    IL_TOOL_SET_PARAM(new_state.dict_mix_ul_h,
        OMX_IndexParamLinePriority, &line_prio);


    IL_TOOL_DISABLE_ALL_PORTS(new_state.dict_mix_ul_h);
    IL_TOOL_ENTER_STATE(OMX_StateIdle,      new_state.dict_mix_ul_h);
    IL_TOOL_ENTER_STATE(OMX_StateExecuting, new_state.dict_mix_ul_h);

    IL_TOOL_DISABLE_ALL_PORTS(new_state.dict_split_ul_h);
    IL_TOOL_ENTER_STATE(OMX_StateIdle,      new_state.dict_split_ul_h);
    IL_TOOL_ENTER_STATE(OMX_StateExecuting, new_state.dict_split_ul_h);


    OMX_AUDIO_PARAM_SYNCHRONIZEDTYPE sync_param;
    IL_TOOL_INIT_CONFIG_STRUCT(sync_param);
    sync_param.bIsSynchronized = OMX_TRUE;
    sync_param.nPortIndex = MIXER_INPUT_PORT;
    IL_TOOL_SET_PARAM(new_state.dict_mix_ul_h, OMX_IndexParamAudioSynchronized, &sync_param);


    IL_TOOL_CONNECT_AND_ENABLE_WITH_SRC_CFG(
        new_state.speechproc_h,     ADM_SPEECHPROC_UL_OUT_PORT,
        new_state.dict_mix_ul_h,    MIXER_INPUT_PORT);

    IL_TOOL_CONNECT_AND_ENABLE_WITH_SRC_CFG(
        new_state.dict_mix_ul_h,    MIXER_OUTPUT_PORT,
        new_state.dict_split_ul_h,  SPLITTER_INPUT_PORT);


    if (!new_state.voip_enabled) {
        /* If VoIP is enabled no connection to cscall should be established */
        IL_TOOL_CONNECT_AND_ENABLE_WITH_SRC_CFG(
            new_state.dict_split_ul_h,  SPLITTER_OUTPUT_PORT,
            new_state.voice_srcsnk_h,   ADM_CSCALL_UL_PORT);
    }


    //
    // Downlink, cscall side
    //
    // If VoIP is enabled no connection to cscall should be established

    // A mixer is insered between splitter and speech proc for resampling
    // since the old binary splitter didn't support resampling.

    IL_TOOL_CREATE(&new_state.dict_split_dl_h, adaptions_comp_name(ADM_VC_DL_SPLITTER_NAME), "dlsplit");

    pcm_mode.nPortIndex = ADM_SPEECHPROC_DL_IN_PORT;
    IL_TOOL_GET_PARAM(new_state.speechproc_h,OMX_IndexParamAudioPcm,&pcm_mode);

    if (new_state.voip_enabled) {
        IL_TOOL_CREATE(&new_state.voip_mix_dl_h, adaptions_comp_name(ADM_VC_DL_MIXER_NAME), "dlmix");

        pcm_mode.nPortIndex = MIXER_OUTPUT_PORT;
        IL_TOOL_SET_PARAM(new_state.voip_mix_dl_h, OMX_IndexParamAudioPcm, &pcm_mode);

        pcm_mode_voip.nPortIndex = MIXER_INPUT_PORT;
        IL_TOOL_GET_PARAM(new_state.voip_mix_dl_h, OMX_IndexParamAudioPcm, &pcm_mode_voip);
        pcm_mode_voip.nSamplingRate = new_state.voip_dl_samplerate;
        pcm_mode_voip.nChannels = new_state.voip_dl_channels;

        if (pcm_mode_voip.nChannels == 1) {
            pcm_mode_voip.eChannelMapping[0] = OMX_AUDIO_ChannelCF;
        } else if (pcm_mode_voip.nChannels == 2) {
            pcm_mode_voip.eChannelMapping[0] = OMX_AUDIO_ChannelLF;
            pcm_mode_voip.eChannelMapping[1] = OMX_AUDIO_ChannelRF;
        }

        ALOG_INFO("VoIP DL, samplerate=%d, channels=%d\n", pcm_mode_voip.nSamplingRate, pcm_mode_voip.nChannels);
        pcm_mode_voip.nPortIndex = SPLITTER_INPUT_PORT;
        IL_TOOL_SET_PARAM(new_state.dict_split_dl_h, OMX_IndexParamAudioPcm, &pcm_mode_voip);
        pcm_mode_voip.nPortIndex = SPLITTER_OUTPUT_PORT;
        IL_TOOL_SET_PARAM(new_state.dict_split_dl_h, OMX_IndexParamAudioPcm, &pcm_mode_voip);
        pcm_mode_voip.nPortIndex = SPLITTER_DICT_OUTPUT_PORT;
        IL_TOOL_SET_PARAM(new_state.dict_split_dl_h, OMX_IndexParamAudioPcm, &pcm_mode_voip);
        pcm_mode_voip.nPortIndex = MIXER_INPUT_PORT;
        IL_TOOL_SET_PARAM(new_state.voip_mix_dl_h, OMX_IndexParamAudioPcm, &pcm_mode_voip);

        // set line priority
        line_prio.nPortIndex = MIXER_OUTPUT_PORT;
        line_prio.ePortPriority = OMX_PriorityTelephonyDL;
        IL_TOOL_SET_PARAM(new_state.voip_mix_dl_h,
            OMX_IndexParamLinePriority, &line_prio);
        IL_TOOL_DISABLE_ALL_PORTS(new_state.voip_mix_dl_h);
        line_prio.nPortIndex = MIXER_INPUT_PORT;
        line_prio.ePortPriority = OMX_PriorityTelephonyDL;
        IL_TOOL_SET_PARAM(new_state.voip_mix_dl_h,
            OMX_IndexParamLinePriority, &line_prio);
        IL_TOOL_DISABLE_ALL_PORTS(new_state.voip_mix_dl_h);

        IL_TOOL_ENTER_STATE(OMX_StateIdle,      new_state.voip_mix_dl_h);
        IL_TOOL_ENTER_STATE(OMX_StateExecuting, new_state.voip_mix_dl_h);
    }
    else {
        pcm_mode.nPortIndex = SPLITTER_INPUT_PORT;
        IL_TOOL_SET_PARAM(new_state.dict_split_dl_h, OMX_IndexParamAudioPcm, &pcm_mode);
        pcm_mode.nPortIndex = SPLITTER_OUTPUT_PORT;
        IL_TOOL_SET_PARAM(new_state.dict_split_dl_h, OMX_IndexParamAudioPcm, &pcm_mode);
        pcm_mode.nPortIndex = SPLITTER_DICT_OUTPUT_PORT;
        IL_TOOL_SET_PARAM(new_state.dict_split_dl_h, OMX_IndexParamAudioPcm, &pcm_mode);
    }

    // set line priority
    line_prio.nPortIndex = SPLITTER_INPUT_PORT;
    line_prio.ePortPriority = OMX_PriorityTelephonyDL;
    IL_TOOL_SET_PARAM(new_state.dict_split_dl_h,
        OMX_IndexParamLinePriority, &line_prio);

    line_prio.nPortIndex = SPLITTER_OUTPUT_PORT;
    line_prio.ePortPriority = OMX_PriorityTelephonyDL;
    IL_TOOL_SET_PARAM(new_state.dict_split_dl_h,
        OMX_IndexParamLinePriority, &line_prio);

    IL_TOOL_DISABLE_ALL_PORTS(new_state.dict_split_dl_h);
    IL_TOOL_ENTER_STATE(OMX_StateIdle,      new_state.dict_split_dl_h);
    IL_TOOL_ENTER_STATE(OMX_StateExecuting, new_state.dict_split_dl_h);


    if (new_state.voip_enabled) {
        /* If VoIP is enabled no connection to cscall should be established  */
        IL_TOOL_CONNECT_AND_ENABLE_WITH_SRC_CFG(
            new_state.dict_split_dl_h,  SPLITTER_OUTPUT_PORT,
            new_state.voip_mix_dl_h,    MIXER_INPUT_PORT);
        IL_TOOL_CONNECT_AND_ENABLE_WITH_SRC_CFG(
            new_state.voip_mix_dl_h,    MIXER_OUTPUT_PORT,
            new_state.speechproc_h,     ADM_SPEECHPROC_DL_IN_PORT);
    }
    else {
        IL_TOOL_CONNECT_AND_ENABLE_WITH_SRC_CFG(
            new_state.voice_srcsnk_h,   ADM_CSCALL_DL_PORT,
            new_state.dict_split_dl_h,  SPLITTER_INPUT_PORT);

        IL_TOOL_CONNECT_AND_ENABLE_WITH_SRC_CFG(
            new_state.dict_split_dl_h,  SPLITTER_OUTPUT_PORT,
            new_state.speechproc_h,     ADM_SPEECHPROC_DL_IN_PORT);
    }
#endif

cleanup:
    srv_become_normal_thread();
    if (res == STE_ADM_RES_OK) {
        g_cscall_omx_state = new_state;
    }

    return res;
}
ste_adm_res_t adm_cscall_omx_activate_cscall_fat(const char* in_dev, const char* out_dev)
{
    (void)in_dev;
    (void)out_dev;
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

    cscall_omx_state_t old_state = g_cscall_omx_state;
    cscall_omx_state_t new_state = old_state;

    // NOTE: In case of error during execution of an IL_TOOL_ macro,
    // it will set the 'result' variable below to the resulting error
    // code and then goto cleanup.

    ste_adm_res_t res = STE_ADM_RES_OK;
    srv_become_worker_thread();

    res = adm_cscall_create_cscall_cmp_fat(&new_state);
    RETURN_IF_FAIL(res);

    res = configure_cscall_comp_fat(&new_state);
    RETURN_IF_FAIL(res);

    srv_become_normal_thread();
    g_cscall_omx_state = new_state;

    return res;
}


ste_adm_res_t adm_cscall_omx_enable_voip_input(int num_bufs, int bufsz)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

    cscall_omx_state_t old_state = g_cscall_omx_state;
    cscall_omx_state_t new_state = old_state;

    // NOTE: In case of error during execution of an IL_TOOL_ macro,
    // it will set the 'result' variable below to the resulting error
    // code and then goto cleanup.
    ste_adm_res_t res = STE_ADM_RES_OK;
    srv_become_worker_thread();

#ifdef CFG_NO_DICT
    if (new_state.voip_enabled) {
        IL_TOOL_SET_PORT_BUFFER_CONFIG(new_state.speechproc_h, ADM_SPEECHPROC_UL_OUT_PORT, num_bufs, bufsz);
        IL_TOOL_ENABLE_PORT_WITH_IO(new_state.speechproc_h, ADM_SPEECHPROC_UL_OUT_PORT);
    }
#endif

#ifdef CFG_FULL
    if (new_state.voip_enabled) {
        IL_TOOL_SET_PORT_BUFFER_CONFIG(new_state.dict_split_ul_h, SPLITTER_OUTPUT_PORT, num_bufs, bufsz);
        IL_TOOL_ENABLE_PORT_WITH_IO(new_state.dict_split_ul_h, SPLITTER_OUTPUT_PORT);
    }
#endif

cleanup:
    srv_become_normal_thread();

    if (res == STE_ADM_RES_OK) {
        g_cscall_omx_state = new_state;
    }
    return res;
}

ste_adm_res_t adm_cscall_omx_disable_voip_input(void)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

    cscall_omx_state_t old_state = g_cscall_omx_state;
    cscall_omx_state_t new_state = old_state;

    // NOTE: In case of error during execution of an IL_TOOL_ macro,
    // it will set the 'result' variable below to the resulting error
    // code and then goto cleanup.
    ste_adm_res_t res = STE_ADM_RES_OK;
    srv_become_worker_thread();

#ifdef CFG_NO_DICT
    if (new_state.voip_enabled) {
        IL_TOOL_DISABLE_PORT_WITH_IO(new_state.speechproc_h, ADM_SPEECHPROC_UL_OUT_PORT);
    }
#endif

#ifdef CFG_FULL
    if (new_state.voip_enabled) {
        IL_TOOL_DISABLE_PORT_WITH_IO(new_state.dict_split_ul_h, SPLITTER_OUTPUT_PORT);
    }
#endif


cleanup:
    srv_become_normal_thread();

    if (res == STE_ADM_RES_OK) {
        g_cscall_omx_state = new_state;
    }
    return res;
}

ste_adm_res_t adm_cscall_omx_enable_voip_output(int num_bufs, int bufsz)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

    cscall_omx_state_t old_state = g_cscall_omx_state;
    cscall_omx_state_t new_state = old_state;

    // NOTE: In case of error during execution of an IL_TOOL_ macro,
    // it will set the 'result' variable below to the resulting error
    // code and then goto cleanup.
    ste_adm_res_t res = STE_ADM_RES_OK;
    srv_become_worker_thread();

#ifdef CFG_NO_DICT
    if (new_state.voip_enabled) {
        IL_TOOL_SET_PORT_BUFFER_CONFIG(new_state.speechproc_h, ADM_SPEECHPROC_DL_IN_PORT, num_bufs, bufsz);
        IL_TOOL_ENABLE_PORT_WITH_IO(new_state.speechproc_h, ADM_SPEECHPROC_DL_IN_PORT);
    }
#endif

#ifdef CFG_FULL
    if (new_state.voip_enabled) {
        IL_TOOL_SET_PORT_BUFFER_CONFIG(new_state.dict_split_dl_h, SPLITTER_INPUT_PORT, num_bufs, bufsz);
        IL_TOOL_ENABLE_PORT_WITH_IO(new_state.dict_split_dl_h, SPLITTER_INPUT_PORT);
    }
#endif

cleanup:
    srv_become_normal_thread();

    if (res == STE_ADM_RES_OK) {
        g_cscall_omx_state = new_state;
    }
    return res;
}

ste_adm_res_t adm_cscall_omx_disable_voip_output(void)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

    cscall_omx_state_t old_state = g_cscall_omx_state;
    cscall_omx_state_t new_state = old_state;

    // NOTE: In case of error during execution of an IL_TOOL_ macro,
    // it will set the 'result' variable below to the resulting error
    // code and then goto cleanup.
    ste_adm_res_t res = STE_ADM_RES_OK;
    srv_become_worker_thread();

#ifdef CFG_NO_DICT
    if (new_state.voip_enabled) {
        IL_TOOL_DISABLE_PORT_WITH_IO(new_state.speechproc_h, ADM_SPEECHPROC_DL_IN_PORT);
    }
#endif

#ifdef CFG_FULL
    if (new_state.voip_enabled) {
        IL_TOOL_DISABLE_PORT_WITH_IO(new_state.dict_split_dl_h, SPLITTER_INPUT_PORT);
    }
#endif


cleanup:
    srv_become_normal_thread();

    if (res == STE_ADM_RES_OK) {
        g_cscall_omx_state = new_state;
    }
    return res;
}

#include "audio_codecs_chipset_api.h"

///////////////////////////////////////////////////////////////////////////////
//
//   adm_cscall_omx_connect_voicecall_input
//
///////////////////////////////////////////////////////////////////////////////
ste_adm_res_t adm_cscall_omx_connect_voicecall_input(il_comp_t capturer_comp, OMX_U32 capturer_port)
{
    ALOG_INFO("adm_cscall_omx_connect_voicecall_input %X.%d \n", capturer_comp, capturer_port);
    if(is_slim_graph_creation(&g_cscall_omx_state))
        return adm_cscall_omx_connect_voicecall_input_slim(capturer_comp,capturer_port);
    else
        return adm_cscall_omx_connect_voicecall_input_fat(capturer_comp,capturer_port);
}
ste_adm_res_t adm_cscall_omx_connect_voicecall_input_slim(il_comp_t capturer_comp, OMX_U32 capturer_port)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

    cscall_omx_state_t old_state = g_cscall_omx_state;
    cscall_omx_state_t new_state = old_state;
    OMX_AUDIO_PARAM_PCMMODETYPE pcm_params;
    IL_TOOL_INIT_CONFIG_STRUCT(pcm_params);

    // NOTE: In case of error during execution of an IL_TOOL_ macro,
    // it will set the 'result' variable below to the resulting error
    // code and then goto cleanup.
    ste_adm_res_t res = STE_ADM_RES_OK;
    srv_become_worker_thread();

    new_state.capturer_port = capturer_port;
    new_state.capturer_h    = capturer_comp;

#ifdef CFG_CSCALL_ONLY
    if (new_state.voip_enabled) {
        /* If VoIP is enabled no connection to cscall should be established */
        IL_TOOL_ENABLE_PORT_WITH_IO(new_state.capturer_h, new_state.capturer_port);
    } else {
        IL_TOOL_CONNECT_AND_ENABLE_WITH_SRC_CFG(
            new_state.capturer_h   ,   new_state.capturer_port,
            new_state.voice_srcsnk_h,  ADM_CSCALL_UL_PORT);
    }
#endif

#ifndef CFG_CSCALL_ONLY
    pcm_params.nPortIndex = new_state.capturer_port;
    res = il_tool_GetParameter(new_state.capturer_h,
    OMX_IndexParamAudioPcm, &pcm_params);
    if (res != STE_ADM_RES_OK)
       return res;

    if (pcm_params.nSamplingRate != g_cscall_omx_state.cscall_samplerate) {
        pcm_params.nSamplingRate = g_cscall_omx_state.cscall_samplerate;

        res = il_tool_SetParameter(new_state.capturer_h, OMX_IndexParamAudioPcm, &pcm_params);
        if (res != STE_ADM_RES_OK)
           return res;
    }

    IL_TOOL_CONNECT_AND_ENABLE_WITH_SRC_CFG(
        new_state.capturer_h,      new_state.capturer_port,
        new_state.speechproc_h,    ADM_SPEECHPROC_UL_IN_PORT);
#endif

cleanup:
    srv_become_normal_thread();
    if (res == STE_ADM_RES_OK) {
        g_cscall_omx_state = new_state;
    }

    return res;
}
ste_adm_res_t adm_cscall_omx_connect_voicecall_input_fat(il_comp_t capturer_comp, OMX_U32 capturer_port)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

    cscall_omx_state_t old_state = g_cscall_omx_state;
    cscall_omx_state_t new_state = old_state;
    OMX_AUDIO_PARAM_PCMMODETYPE pcm_params;
    IL_TOOL_INIT_CONFIG_STRUCT(pcm_params);

    // NOTE: In case of error during execution of an IL_TOOL_ macro,
    // it will set the 'result' variable below to the resulting error
    // code and then goto cleanup.
    ste_adm_res_t res = STE_ADM_RES_OK;
    srv_become_worker_thread();

    new_state.capturer_port = capturer_port;
    new_state.capturer_h    = capturer_comp;

    IL_TOOL_CONNECT_AND_ENABLE_WITH_DST_CFG(
        new_state.capturer_h   ,   new_state.capturer_port,
        new_state.voice_srcsnk_h,  ADM_CSCALL_UL_PORT);

cleanup:
    srv_become_normal_thread();
    if (res == STE_ADM_RES_OK) {
        g_cscall_omx_state = new_state;
    }

    return res;
}

///////////////////////////////////////////////////////////////////////////////
//
//   adm_cscall_omx_connect_voicecall_output
//
///////////////////////////////////////////////////////////////////////////////
ste_adm_res_t adm_cscall_omx_connect_voicecall_output(
    il_comp_t renderer_comp, OMX_U32 renderer_port,
    il_comp_t reference_comp, OMX_U32 reference_port)
{
    ALOG_INFO("adm_cscall_omx_connect_voicecall_output %X.%d, echo ref %X.%d\n", renderer_comp, renderer_port, reference_comp, reference_port);

    if(is_slim_graph_creation(&g_cscall_omx_state))
        return adm_cscall_omx_connect_voicecall_output_slim(renderer_comp,renderer_port,reference_comp,reference_port);
    else
        return adm_cscall_omx_connect_voicecall_output_fat(renderer_comp,renderer_port,reference_comp,reference_port);
}
ste_adm_res_t adm_cscall_omx_connect_voicecall_output_slim(
    il_comp_t renderer_comp, OMX_U32 renderer_port,
    il_comp_t reference_comp, OMX_U32 reference_port)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

    cscall_omx_state_t old_state = g_cscall_omx_state;
    cscall_omx_state_t new_state = old_state;

    new_state.renderer_h     = renderer_comp;
    new_state.renderer_port  = renderer_port;

    new_state.reference_h    = reference_comp;
    new_state.reference_port = reference_port;

    // NOTE: In case of error during execution of an IL_TOOL_ macro,
    // it will set the 'result' variable below to the resulting error
    // code and then goto cleanup.
    ste_adm_res_t res = STE_ADM_RES_OK;
    srv_become_worker_thread();


#ifdef CFG_CSCALL_ONLY
    if (new_state.voip_enabled) {
        /* If VoIP is enabled no connection to cscall should be established */
        IL_TOOL_ENABLE_PORT_WITH_IO(renderer_comp, renderer_port);
    } else {
        IL_TOOL_CONNECT_AND_ENABLE_WITH_SRC_CFG(
            new_state.voice_srcsnk_h, ADM_CSCALL_DL_PORT,
            renderer_comp,            renderer_port);
    }
#endif


#ifndef CFG_CSCALL_ONLY
    IL_TOOL_CONNECT_AND_ENABLE_WITH_SRC_CFG(
        new_state.speechproc_h,  ADM_SPEECHPROC_DL_OUT_PORT,
        renderer_comp, renderer_port);
#endif


#ifdef CFG_ENABLE_ECHO_REFERENCE
    if (reference_comp != NULL) {
        // if echo reference port is supported (not NULL) then connect and enable
        ALOG_INFO_VERBOSE("adm_cscall_omx_connect_voicecall -- connecting and enable echo reference 0X%x", reference_comp);

        IL_TOOL_CONNECT_AND_ENABLE_WITH_SRC_CFG(
                reference_comp,           reference_port,
                new_state.speechproc_h,   ADM_SPEECHPROC_REF_PORT);
    } else {
        ALOG_INFO("no echo reference to connect device and speech_proc");
    }
#endif

cleanup:
    srv_become_normal_thread();
    if (res == STE_ADM_RES_OK) {
        g_cscall_omx_state = new_state;
    }

    return res;
}
ste_adm_res_t adm_cscall_omx_connect_voicecall_output_fat(
    il_comp_t renderer_comp, OMX_U32 renderer_port,
    il_comp_t reference_comp, OMX_U32 reference_port)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

    cscall_omx_state_t old_state = g_cscall_omx_state;
    cscall_omx_state_t new_state = old_state;

    new_state.renderer_h     = renderer_comp;
    new_state.renderer_port  = renderer_port;

    new_state.reference_h    = reference_comp;
    new_state.reference_port = reference_port;

    // NOTE: In case of error during execution of an IL_TOOL_ macro,
    // it will set the 'result' variable below to the resulting error
    // code and then goto cleanup.
    ste_adm_res_t res = STE_ADM_RES_OK;
    srv_become_worker_thread();

    IL_TOOL_CONNECT_AND_ENABLE_WITH_SRC_CFG(
        new_state.voice_srcsnk_h, ADM_CSCALL_DL_PORT,
        renderer_comp,            renderer_port);

#ifdef CFG_ENABLE_ECHO_REFERENCE
    if (reference_comp != NULL) {
        // if echo reference port is supported (not NULL) then connect and enable
        ALOG_INFO("adm_cscall_omx_connect_voicecall_output_fat -- connecting and enable echo reference 0X%x", reference_comp);

        IL_TOOL_CONNECT_AND_ENABLE_WITH_SRC_CFG(
                reference_comp,           reference_port,
                new_state.voice_srcsnk_h, ADM_CSCALL_MPC_REF_PORT);
    } else {
        ALOG_INFO("no echo reference to connect device and cscall_mpc");
    }
#endif

cleanup:
    srv_become_normal_thread();
    if (res == STE_ADM_RES_OK) {
        g_cscall_omx_state = new_state;
    }

    return res;
}

ste_adm_res_t adm_cscall_omx_disconnect_voicecall_input()
{
    ALOG_INFO("adm_cscall_omx_disconnect_voicecall_input");
    if(is_slim_graph_creation(&g_cscall_omx_state))
        return adm_cscall_omx_disconnect_voicecall_input_slim();
    else
        return adm_cscall_omx_disconnect_voicecall_input_fat();
}
ste_adm_res_t adm_cscall_omx_disconnect_voicecall_input_slim()
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    cscall_omx_state_t old_state = g_cscall_omx_state;

    // NOTE: In case of error during execution of an IL_TOOL_ macro,
    // it will set the 'result' variable below to the resulting error
    // code and then goto cleanup.
    ste_adm_res_t res = STE_ADM_RES_OK;
    srv_become_worker_thread();


#ifdef CFG_CSCALL_ONLY
    if (old_state.voip_enabled) {
        /* If VoIP is enabled no connection to cscall should be established */
        IL_TOOL_DISABLE_PORT_WITH_IO(old_state.capturer_h, old_state.capturer_port);
    } else {
        IL_TOOL_DISCONNECT(
            old_state.capturer_h,      old_state.capturer_port,
            old_state.voice_srcsnk_h,  ADM_CSCALL_UL_PORT);
    }
#endif

#ifndef CFG_CSCALL_ONLY
    IL_TOOL_DISCONNECT(
            old_state.capturer_h,          old_state.capturer_port,
            old_state.speechproc_h,        ADM_SPEECHPROC_UL_IN_PORT);
#endif

cleanup:
    srv_become_normal_thread();

    return res;
}
ste_adm_res_t adm_cscall_omx_disconnect_voicecall_input_fat()
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    cscall_omx_state_t old_state = g_cscall_omx_state;

    // NOTE: In case of error during execution of an IL_TOOL_ macro,
    // it will set the 'result' variable below to the resulting error
    // code and then goto cleanup.
    ste_adm_res_t res = STE_ADM_RES_OK;
    srv_become_worker_thread();

    IL_TOOL_DISCONNECT(
        old_state.capturer_h,      old_state.capturer_port,
        old_state.voice_srcsnk_h,  ADM_CSCALL_UL_PORT);
cleanup:
    srv_become_normal_thread();

    return res;
}

ste_adm_res_t adm_cscall_omx_disconnect_voicecall_output()
{
    ALOG_INFO("adm_cscall_omx_disconnect_voicecall_output");
    if(is_slim_graph_creation(&g_cscall_omx_state))
        return adm_cscall_omx_disconnect_voicecall_output_slim();
    else
        return adm_cscall_omx_disconnect_voicecall_output_fat();
}

ste_adm_res_t adm_cscall_omx_disconnect_voicecall_output_slim()
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

    cscall_omx_state_t old_state = g_cscall_omx_state;

    // NOTE: In case of error during execution of an IL_TOOL_ macro,
    // it will set the 'result' variable below to the resulting error
    // code and then goto cleanup.
    ste_adm_res_t res = STE_ADM_RES_OK;
    srv_become_worker_thread();

#ifdef CFG_CSCALL_ONLY
    if (old_state.voip_enabled) {
        /* If VoIP is enabled no connection to cscall should be established */
        IL_TOOL_DISABLE_PORT_WITH_IO(old_state.renderer_h, old_state.renderer_port);
    } else {
        IL_TOOL_DISCONNECT(
            old_state.renderer_h,      old_state.renderer_port,
            old_state.voice_srcsnk_h,  ADM_CSCALL_DL_PORT);
    }
#endif

#ifndef CFG_CSCALL_ONLY
#ifdef CFG_ENABLE_ECHO_REFERENCE
    if (old_state.reference_h != NULL) {
           // if echo reference port is supported (not NULL) then disconnect
        IL_TOOL_DISCONNECT(old_state.reference_h,  old_state.reference_port,
                           old_state.speechproc_h, ADM_SPEECHPROC_REF_PORT);

        ALOG_INFO("echo reference disconnected from speech_proc\n");
    }  else ALOG_INFO("no echo reference to disconnect");
#endif // CFG_ENABLE_ECHO_REFERENCE

    IL_TOOL_DISCONNECT(old_state.speechproc_h, ADM_SPEECHPROC_DL_OUT_PORT,
                       old_state.renderer_h,   old_state.renderer_port);
#endif // CFG_CSCALL_ONLY
cleanup:
    srv_become_normal_thread();

    return res;
}
ste_adm_res_t adm_cscall_omx_disconnect_voicecall_output_fat()
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

    cscall_omx_state_t old_state = g_cscall_omx_state;

    // NOTE: In case of error during execution of an IL_TOOL_ macro,
    // it will set the 'result' variable below to the resulting error
    // code and then goto cleanup.
    ste_adm_res_t res = STE_ADM_RES_OK;
    srv_become_worker_thread();

#ifdef CFG_ENABLE_ECHO_REFERENCE
        if (old_state.reference_h != NULL) {
            // if echo reference port is supported (not NULL) then disconnect
            IL_TOOL_DISCONNECT(old_state.reference_h,  old_state.reference_port,
                                old_state.voice_srcsnk_h, ADM_CSCALL_MPC_REF_PORT);

            ALOG_INFO("echo reference disconnected cscall_mpc\n");
        }else ALOG_INFO("no echo reference to disconnect");
#endif // CFG_ENABLE_ECHO_REFERENCE

    IL_TOOL_DISCONNECT(
        old_state.voice_srcsnk_h,  ADM_CSCALL_DL_PORT,
        old_state.renderer_h,      old_state.renderer_port);
cleanup:
    srv_become_normal_thread();

    return res;
}


///////////////////////////////////////////////////////////////////////////////
//
//  deactivate_cscall
//
///////////////////////////////////////////////////////////////////////////////
ste_adm_res_t adm_cscall_omx_deactivate_cscall()
{
    ALOG_INFO("IL TRANSITIONS: deactivate_cscall\n");
    if(is_slim_graph_creation(&g_cscall_omx_state))
        return adm_cscall_omx_deactivate_cscall_slim();
    else
        return adm_cscall_omx_deactivate_cscall_fat();
}
ste_adm_res_t adm_cscall_omx_deactivate_cscall_slim()
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

    cscall_omx_state_t old_state = g_cscall_omx_state;
    cscall_omx_state_t new_state = old_state;

    // NOTE: In case of error during execution of an IL_TOOL_ macro,
    // it will set the 'result' variable below to the resulting error
    // code and then goto cleanup.
    ste_adm_res_t res = STE_ADM_RES_OK;
    srv_become_worker_thread();

#ifdef CFG_NO_DICT
    if (new_state.voip_enabled) {
        IL_TOOL_DISABLE_PORT_WITH_IO(dict_mix_ul_h, MIXER_OUTPUT_PORT);
        IL_TOOL_DISCONNECT(
            new_state.speechproc_h,     ADM_SPEECHPROC_UL_OUT_PORT,
            new_state.dict_mix_ul_h,    MIXER_INPUT_PORT);
        IL_TOOL_DISCONNECT(
            new_state.voip_mix_dl_h,    MIXER_OUTPUT_PORT,
            new_state.speechproc_h,     ADM_SPEECHPROC_DL_IN_PORT);
        IL_TOOL_DISABLE_PORT_WITH_IO(dict_mix_dl_h, MIXER_INPUT_PORT);
    } else {
        IL_TOOL_DISCONNECT(
            voice_srcsnk_h,     ADM_CSCALL_DL_PORT,
            speechproc_h,       ADM_SPEECHPROC_DL_IN_PORT);

        IL_TOOL_DISCONNECT(
            speechproc_h,       ADM_SPEECHPROC_UL_OUT_PORT,
            voice_srcsnk_h,     ADM_CSCALL_UL_PORT);
    }

    IL_TOOL_ENTER_STATE(OMX_StateIdle,   speechproc_h);
    IL_TOOL_ENTER_STATE(OMX_StateLoaded, speechproc_h);
    IL_TOOL_DESTROY(&speechproc_h);
#endif

#ifdef CFG_FULL
    //
    // Uplink
    //

    IL_TOOL_DISCONNECT(
        new_state.speechproc_h,     ADM_SPEECHPROC_UL_OUT_PORT,
        new_state.dict_mix_ul_h,    MIXER_INPUT_PORT);

    IL_TOOL_DISCONNECT(
        new_state.dict_mix_ul_h,    MIXER_OUTPUT_PORT,
        new_state.dict_split_ul_h,  SPLITTER_INPUT_PORT);

    if (new_state.voip_enabled) {
        IL_TOOL_DISABLE_PORT_WITH_IO(new_state.dict_split_ul_h, SPLITTER_OUTPUT_PORT);
    } else {
        IL_TOOL_DISCONNECT(
            new_state.dict_split_ul_h,  SPLITTER_OUTPUT_PORT,
            new_state.voice_srcsnk_h,   ADM_CSCALL_UL_PORT);
    }


    IL_TOOL_ENTER_STATE(OMX_StateIdle,   new_state.dict_mix_ul_h);
    IL_TOOL_ENTER_STATE(OMX_StateLoaded, new_state.dict_mix_ul_h);
    IL_TOOL_DESTROY(&new_state.dict_mix_ul_h);

    IL_TOOL_ENTER_STATE(OMX_StateIdle,   new_state.dict_split_ul_h);
    IL_TOOL_ENTER_STATE(OMX_StateLoaded, new_state.dict_split_ul_h);
    IL_TOOL_DESTROY(&new_state.dict_split_ul_h);



    //
    // Downlink
    //

    if (new_state.voip_enabled) {
        IL_TOOL_DISABLE_PORT_WITH_IO(new_state.dict_split_dl_h, SPLITTER_INPUT_PORT);
        IL_TOOL_DISCONNECT(
            g_cscall_omx_state.dict_split_dl_h, SPLITTER_OUTPUT_PORT,
            g_cscall_omx_state.voip_mix_dl_h,   MIXER_INPUT_PORT);
        IL_TOOL_DISCONNECT(
            g_cscall_omx_state.voip_mix_dl_h,   MIXER_OUTPUT_PORT,
            g_cscall_omx_state.speechproc_h,    ADM_SPEECHPROC_DL_IN_PORT);

        IL_TOOL_ENTER_STATE(OMX_StateIdle,   g_cscall_omx_state.voip_mix_dl_h);
        IL_TOOL_ENTER_STATE(OMX_StateLoaded, g_cscall_omx_state.voip_mix_dl_h);
        IL_TOOL_DESTROY(&new_state.voip_mix_dl_h);
    } else {
        IL_TOOL_DISCONNECT(
            new_state.voice_srcsnk_h,   ADM_CSCALL_DL_PORT,
            new_state.dict_split_dl_h,  SPLITTER_INPUT_PORT);
        IL_TOOL_DISCONNECT(
            new_state.dict_split_dl_h,  SPLITTER_OUTPUT_PORT,
            new_state.speechproc_h,     ADM_SPEECHPROC_DL_IN_PORT);
    }

    IL_TOOL_ENTER_STATE(OMX_StateIdle,   new_state.dict_split_dl_h);
    IL_TOOL_ENTER_STATE(OMX_StateLoaded, new_state.dict_split_dl_h);
    IL_TOOL_DESTROY(&new_state.dict_split_dl_h);



    // Assert: voicecall disconnected

    IL_TOOL_ENTER_STATE(OMX_StateIdle,   new_state.speechproc_h);
    IL_TOOL_ENTER_STATE(OMX_StateLoaded, new_state.speechproc_h);
    IL_TOOL_DESTROY(&new_state.speechproc_h);
#endif

cleanup:
    srv_become_normal_thread();
    if (res == STE_ADM_RES_OK) {
        g_cscall_omx_state = new_state;
    }

    return res;
}
ste_adm_res_t adm_cscall_omx_deactivate_cscall_fat()
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

    cscall_omx_state_t old_state = g_cscall_omx_state;
    cscall_omx_state_t new_state = old_state;

    // NOTE: In case of error during execution of an IL_TOOL_ macro,
    // it will set the 'result' variable below to the resulting error
    // code and then goto cleanup.
    ste_adm_res_t res = STE_ADM_RES_OK;
    srv_become_worker_thread();

    if (new_state.voice_srcsnk_h) {
        IL_TOOL_ENTER_STATE(OMX_StateIdle,   new_state.voice_srcsnk_h);
        IL_TOOL_ENTER_STATE(OMX_StateLoaded, new_state.voice_srcsnk_h);
        IL_TOOL_DESTROY(&new_state.voice_srcsnk_h);
    }
cleanup:
    srv_become_normal_thread();
    if (res == STE_ADM_RES_OK) {
        g_cscall_omx_state = new_state;
    }

    return res;
}

///////////////////////////////////////////////////////////////////////////////
//
//  enable_dict_rec
//
//  Connect the recording dictaphone. Assumes voicecall is already up and
//  running
//
//
///////////////////////////////////////////////////////////////////////////////
ste_adm_res_t adm_cscall_omx_enable_dict_rec(int num_bufs, int bufsz, int samplerate, int channels)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    ALOG_INFO("IL TRANSITIONS: enable_dict_rec\n");
    if(is_slim_graph_creation(&g_cscall_omx_state))
        return adm_cscall_omx_enable_dict_rec_slim(num_bufs,bufsz,samplerate,channels);
    else
        return adm_cscall_omx_enable_dict_rec_fat(num_bufs,bufsz,samplerate,channels);
}

ste_adm_res_t adm_cscall_omx_enable_dict_rec_slim(int num_bufs, int bufsz, int samplerate, int channels)
{
    cscall_omx_state_t old_state = g_cscall_omx_state;
    cscall_omx_state_t new_state = old_state;

    // NOTE: In case of error during execution of an IL_TOOL_ macro,
    // it will set the 'result' variable below to the resulting error
    // code and then goto cleanup.
    ste_adm_res_t res = STE_ADM_RES_OK;
    srv_become_worker_thread();


    ADM_ASSERT(new_state.speechproc_h   != NULL);
    ADM_ASSERT(new_state.dict_rec_mix_h == NULL);


#ifdef CFG_FULL
    IL_TOOL_CREATE(&new_state.dict_rec_mix_h, adaptions_comp_name(ADM_VC_DICT_MIXER_NAME), "recmix");

    OMX_AUDIO_PARAM_PCMMODETYPE  pcm_mode;
    IL_TOOL_INIT_CONFIG_STRUCT(pcm_mode);
    pcm_mode.nPortIndex = ADM_SPEECHPROC_UL_OUT_PORT;
    IL_TOOL_GET_PARAM(new_state.speechproc_h,OMX_IndexParamAudioPcm,&pcm_mode);

    pcm_mode.nPortIndex = 1;
    IL_TOOL_SET_PARAM(new_state.dict_rec_mix_h, OMX_IndexParamAudioPcm, &pcm_mode);

    if(new_state.dict_mode == STE_ADM_DICTAPHONE_UPLINK_DOWNLINK) {
        pcm_mode.nPortIndex = 2;
        IL_TOOL_SET_PARAM(new_state.dict_rec_mix_h, OMX_IndexParamAudioPcm, &pcm_mode);
    }

    /** The output port on the mixer needs to be configured to match the recording client */
    ALOG_INFO("Dictaphone device setttings: samplerate %d [Hz], channel(s) %d\n", samplerate, channels);

    if(channels == STE_ADM_FORMAT_STEREO)
    {
        pcm_mode.eChannelMapping[0] = OMX_AUDIO_ChannelLF;
        pcm_mode.eChannelMapping[1] = OMX_AUDIO_ChannelRF;
    }

    pcm_mode.nPortIndex = 0;
    pcm_mode.nChannels = channels;
    pcm_mode.nSamplingRate = samplerate;
    IL_TOOL_SET_PARAM(new_state.dict_rec_mix_h, OMX_IndexParamAudioPcm, &pcm_mode);


    OMX_AUDIO_PARAM_SYNCHRONIZEDTYPE sync_param;;
    IL_TOOL_INIT_CONFIG_STRUCT(sync_param);
    sync_param.bIsSynchronized = OMX_TRUE;

    sync_param.nPortIndex = 1;
    IL_TOOL_SET_PARAM(new_state.dict_rec_mix_h, OMX_IndexParamAudioSynchronized, &sync_param);

    if(new_state.dict_mode == STE_ADM_DICTAPHONE_UPLINK_DOWNLINK) {
        sync_param.nPortIndex = 2;
        IL_TOOL_SET_PARAM(new_state.dict_rec_mix_h, OMX_IndexParamAudioSynchronized, &sync_param);
    }

    IL_TOOL_DISABLE_ALL_PORTS(new_state.dict_rec_mix_h);
    IL_TOOL_ENTER_STATE(OMX_StateIdle,      new_state.dict_rec_mix_h);
    IL_TOOL_ENTER_STATE(OMX_StateExecuting, new_state.dict_rec_mix_h);

    if(new_state.dict_mode == STE_ADM_DICTAPHONE_UPLINK_DOWNLINK)
    {
        IL_TOOL_CONNECT_AND_ENABLE_WITH_SRC_CFG(
                new_state.dict_split_dl_h,    2,
                new_state.dict_rec_mix_h,     1);

        IL_TOOL_CONNECT_AND_ENABLE_WITH_SRC_CFG(
                new_state.dict_split_ul_h,    2,
                new_state.dict_rec_mix_h,     2);
    }
    else if(new_state.dict_mode == STE_ADM_DICTAPHONE_DOWNLINK)
    {
        IL_TOOL_CONNECT_AND_ENABLE_WITH_SRC_CFG(
                new_state.dict_split_dl_h,    2,
                new_state.dict_rec_mix_h,     1);
    }
    else if(new_state.dict_mode == STE_ADM_DICTAPHONE_UPLINK)
    {
        IL_TOOL_CONNECT_AND_ENABLE_WITH_SRC_CFG(
                new_state.dict_split_ul_h,    2,
                new_state.dict_rec_mix_h,     1);
    }

    IL_TOOL_SET_PORT_BUFFER_CONFIG(new_state.dict_rec_mix_h, 0, num_bufs, bufsz);
    IL_TOOL_ENABLE_PORT_WITH_IO(new_state.dict_rec_mix_h, 0);
#endif

cleanup:
    srv_become_normal_thread();
    if (res == STE_ADM_RES_OK) {
        g_cscall_omx_state = new_state;
    }

    return res;
}

ste_adm_res_t adm_cscall_omx_enable_dict_rec_fat(int num_bufs, int bufsz, int samplerate, int channels)
{
    ALOG_INFO("enter enable_dict_rec\n");

    cscall_omx_state_t old_state = g_cscall_omx_state;
    cscall_omx_state_t new_state = old_state;

    // NOTE: In case of error during execution of an IL_TOOL_ macro,
    // it will set the 'result' variable below to the resulting error
    // code and then goto cleanup.
    ste_adm_res_t res = STE_ADM_RES_OK;
    srv_become_worker_thread();

    if(adm_get_modem_type()==ADM_FAT_MODEM) {

        ADM_ASSERT(new_state.modem_src_h == NULL);
        ADM_ASSERT(new_state.dict_rec_mix_h == NULL);
        IL_TOOL_CREATE(&new_state.modem_src_h, adaptions_comp_name(ADM_MODEM_SOURCE_NAME), "recsrc");
        IL_TOOL_CREATE(&new_state.dict_rec_mix_h, adaptions_comp_name(ADM_ARM_MIXER_NAME), "recmix");

        OMX_AUDIO_PARAM_PCMMODETYPE  pcm_mode;
        IL_TOOL_INIT_CONFIG_STRUCT(pcm_mode);
        pcm_mode.nPortIndex = 0;

        IL_TOOL_GET_PARAM(new_state.modem_src_h, OMX_IndexParamAudioPcm, &pcm_mode);
        pcm_mode.nSamplingRate = new_state.cscall_samplerate;
        pcm_mode.nChannels = STE_ADM_FORMAT_MONO;
        pcm_mode.eChannelMapping[0] = OMX_AUDIO_ChannelCF;
        pcm_mode.eEndian = OMX_EndianLittle;
        IL_TOOL_SET_PARAM(new_state.modem_src_h, OMX_IndexParamAudioPcm, &pcm_mode);
        IL_TOOL_GET_PARAM(new_state.modem_src_h, OMX_IndexParamAudioPcm, &pcm_mode);

        pcm_mode.nPortIndex = 1;
        IL_TOOL_SET_PARAM(new_state.dict_rec_mix_h, OMX_IndexParamAudioPcm, &pcm_mode);

        /** The output port on the mixer needs to be configured to match the recording client */
        ALOG_INFO("Dictaphone device setttings: samplerate %d [Hz], channels %d\n", samplerate, channels);
        pcm_mode.nPortIndex = 0;
        pcm_mode.nSamplingRate = samplerate;
        pcm_mode.nChannels = channels;
        if(channels == STE_ADM_FORMAT_STEREO)
        {
          pcm_mode.eChannelMapping[0] = OMX_AUDIO_ChannelLF;
          pcm_mode.eChannelMapping[1] = OMX_AUDIO_ChannelRF;
        }
        IL_TOOL_SET_PARAM(new_state.dict_rec_mix_h, OMX_IndexParamAudioPcm, &pcm_mode);

        OMX_AUDIO_PARAM_SYNCHRONIZEDTYPE sync_param;;
        IL_TOOL_INIT_CONFIG_STRUCT(sync_param);

        sync_param.bIsSynchronized = OMX_TRUE;
        sync_param.nPortIndex = 1;
        IL_TOOL_SET_PARAM(new_state.dict_rec_mix_h, OMX_IndexParamAudioSynchronized, &sync_param);

        // set dictaphone mode
        ADM_PARAM_DICTRECMODETYPE  dict_mode;
        IL_TOOL_INIT_CONFIG_STRUCT(dict_mode);
        dict_mode.nMode = new_state.dict_mode;
        IL_TOOL_SET_PARAM(new_state.modem_src_h, ADM_IndexParamDictRecMode, &dict_mode);

        IL_TOOL_DISABLE_ALL_PORTS(new_state.dict_rec_mix_h);
        IL_TOOL_ENTER_STATE(OMX_StateIdle,      new_state.dict_rec_mix_h);
        IL_TOOL_ENTER_STATE(OMX_StateExecuting, new_state.dict_rec_mix_h);

        IL_TOOL_DISABLE_ALL_PORTS(new_state.modem_src_h);
        IL_TOOL_ENTER_STATE(OMX_StateIdle,      new_state.modem_src_h);
        IL_TOOL_ENTER_STATE(OMX_StateExecuting, new_state.modem_src_h);

        IL_TOOL_SET_PORT_BUFFER_CONFIG(new_state.dict_rec_mix_h, 0, num_bufs, bufsz);
        IL_TOOL_ENABLE_PORT_WITH_IO(new_state.dict_rec_mix_h, 0);

        IL_TOOL_CONNECT_AND_ENABLE_WITH_SRC_CFG(
              new_state.modem_src_h,    0,
              new_state.dict_rec_mix_h,     1);
    }

cleanup:
    srv_become_normal_thread();
    if (res == STE_ADM_RES_OK) {
        g_cscall_omx_state = new_state;
    }else{
        if (new_state.modem_src_h != NULL)
            IL_TOOL_DESTROY(&new_state.modem_src_h);
        if (new_state.dict_rec_mix_h != NULL)
            IL_TOOL_DESTROY(&new_state.dict_rec_mix_h);
    }
    ALOG_INFO("leave enable_dict_rec\n");

    return res;
}

///////////////////////////////////////////////////////////////////////////////
//
//  disable_dict_rec
//
//  Disconnect the recording dictaphone. Assumes the dictaphone as well as
//  voicecall is already up and running.
//
//
///////////////////////////////////////////////////////////////////////////////
ste_adm_res_t adm_cscall_omx_disable_dict_rec()
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    ALOG_INFO("IL TRANSITIONS: disable_dict_rec\n");
    if(is_slim_graph_creation(&g_cscall_omx_state))
        return adm_cscall_omx_disable_dict_rec_slim();
    else
        return adm_cscall_omx_disable_dict_rec_fat();

}

ste_adm_res_t adm_cscall_omx_disable_dict_rec_slim()
{
    cscall_omx_state_t old_state = g_cscall_omx_state;
    cscall_omx_state_t new_state = old_state;

    // NOTE: In case of error during execution of an IL_TOOL_ macro,
    // it will set the 'result' variable below to the resulting error
    // code and then goto cleanup.
    ste_adm_res_t res = STE_ADM_RES_OK;
    srv_become_worker_thread();

    ADM_ASSERT(new_state.speechproc_h   != NULL);
    ADM_ASSERT(new_state.dict_rec_mix_h != NULL);

#ifdef CFG_FULL
    if(new_state.dict_mode == STE_ADM_DICTAPHONE_UPLINK_DOWNLINK)
    {
        IL_TOOL_DISCONNECT(new_state.dict_split_dl_h, 2,
                           new_state.dict_rec_mix_h, 1);

        IL_TOOL_DISCONNECT(new_state.dict_split_ul_h, 2,
                           new_state.dict_rec_mix_h, 2);
    }
    else if(new_state.dict_mode == STE_ADM_DICTAPHONE_DOWNLINK)
    {
        IL_TOOL_DISCONNECT(new_state.dict_split_dl_h, 2,
                           new_state.dict_rec_mix_h, 1);
    }
    else if(new_state.dict_mode == STE_ADM_DICTAPHONE_UPLINK)
    {
        IL_TOOL_DISCONNECT(new_state.dict_split_ul_h, 2,
                           new_state.dict_rec_mix_h, 1);
    }

    IL_TOOL_DISABLE_PORT_WITH_IO(new_state.dict_rec_mix_h, 0);

    IL_TOOL_ENTER_STATE(OMX_StateIdle,   new_state.dict_rec_mix_h);
    IL_TOOL_ENTER_STATE(OMX_StateLoaded, new_state.dict_rec_mix_h);
    IL_TOOL_DESTROY(&new_state.dict_rec_mix_h);
#endif

cleanup:
    srv_become_normal_thread();
    if (res == STE_ADM_RES_OK) {
        g_cscall_omx_state = new_state;
    }

    return res;
}

ste_adm_res_t adm_cscall_omx_disable_dict_rec_fat()
{
    cscall_omx_state_t old_state = g_cscall_omx_state;
    cscall_omx_state_t new_state = old_state;

    // NOTE: In case of error during execution of an IL_TOOL_ macro,
    // it will set the 'result' variable below to the resulting error
    // code and then goto cleanup.
    ste_adm_res_t res = STE_ADM_RES_OK;
    srv_become_worker_thread();

    ADM_ASSERT(new_state.modem_src_h    != NULL);
    ADM_ASSERT(new_state.dict_rec_mix_h != NULL);

    if(adm_get_modem_type()==ADM_FAT_MODEM) {

        IL_TOOL_DISCONNECT(new_state.modem_src_h,0,
                           new_state.dict_rec_mix_h,1);

        IL_TOOL_DISABLE_PORT_WITH_IO(new_state.dict_rec_mix_h, 0);

        IL_TOOL_ENTER_STATE(OMX_StateIdle,   new_state.dict_rec_mix_h);
        IL_TOOL_ENTER_STATE(OMX_StateLoaded, new_state.dict_rec_mix_h);
        IL_TOOL_DESTROY(&new_state.dict_rec_mix_h);

        IL_TOOL_ENTER_STATE(OMX_StateIdle,   new_state.modem_src_h);
        IL_TOOL_ENTER_STATE(OMX_StateLoaded, new_state.modem_src_h);
        IL_TOOL_DESTROY(&new_state.modem_src_h);
    }

cleanup:
    srv_become_normal_thread();
    if (res == STE_ADM_RES_OK) {
        g_cscall_omx_state = new_state;
    }

    return res;
}

///////////////////////////////////////////////////////////////////////////////
//
//  set_dict_mode
//
//  Set the correct paths during the recording dictaphone. Assumes the dictaphone
//  as well as voicecall is already up and running.
//
//
///////////////////////////////////////////////////////////////////////////////
ste_adm_res_t adm_cscall_omx_set_dict_mode(int mode)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

    ALOG_INFO("IL TRANSITIONS: set_dict_mode %s \n", (mode == STE_ADM_DICTAPHONE_UPLINK_DOWNLINK ? "uplink/downlink" : 
                                                     (mode == STE_ADM_DICTAPHONE_UPLINK ? "uplink" : "downlink") ));

    g_cscall_omx_state.dict_mode = mode ;

    return STE_ADM_RES_OK;
}

///////////////////////////////////////////////////////////////////////////////
//
//  set_dict_mute
//
//  Mute or unmute recording dictaphone. Assumes the dictaphone
//  as well as voicecall is already up and running.
//
//
///////////////////////////////////////////////////////////////////////////////
ste_adm_res_t adm_cscall_omx_set_dict_mute(int mute)
{
    cscall_omx_state_t old_state = g_cscall_omx_state;
    cscall_omx_state_t new_state = old_state;

    // NOTE: In case of error during execution of an IL_TOOL_ macro,
    // it will set the 'result' variable below to the resulting error
    // code and then goto cleanup.
    ste_adm_res_t res = STE_ADM_RES_OK;

    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

    if(adm_get_modem_type()==ADM_FAT_MODEM) {
        ALOG_INFO("IL TRANSITIONS: %s dictaphone.\n", (mute == 1) ? "mute" : "unmute");

        ADM_PARAM_DICTRECMUTETYPE  dict_mute;
        IL_TOOL_INIT_CONFIG_STRUCT(dict_mute);
        dict_mute.nMute = mute;
        dict_mute.nPortIndex = 0;
        IL_TOOL_SET_CONFIG(old_state.modem_src_h, ADM_IndexParamDictRecMute, &dict_mute);

        new_state.dict_mute = mute;
    }else{
        ALOG_ERR("adm_cscall_omx_set_dict_mute: mute not available\n");
        res = STE_ADM_RES_IL_ERROR;
    }

cleanup:
    if (res == STE_ADM_RES_OK) {
        g_cscall_omx_state = new_state;
    }

    return res;
}

///////////////////////////////////////////////////////////////////////////////
//
//  get_dict_mute
//
//  returns dictaphone mute status. Assumes the dictaphone
//  as well as voicecall is already up and running.
//
//
///////////////////////////////////////////////////////////////////////////////
ste_adm_res_t adm_cscall_omx_get_dict_mute(int *mute)
{
    // NOTE: In case of error during execution of an IL_TOOL_ macro,
    // it will set the 'result' variable below to the resulting error
    // code and then goto cleanup.
    ste_adm_res_t res = STE_ADM_RES_OK;
    *mute=0;

    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

    if(adm_get_modem_type()==ADM_FAT_MODEM) {

        ADM_PARAM_DICTRECMUTETYPE  dict_mute;
        IL_TOOL_INIT_CONFIG_STRUCT(dict_mute);
        IL_TOOL_GET_CONFIG(g_cscall_omx_state.modem_src_h, ADM_IndexParamDictRecMute, &dict_mute);
        *mute = dict_mute.nMute;
        ALOG_INFO("IL TRANSITIONS: dictaphone is %s \n", (*mute == 1) ? "muted" : "unmuted");
    }else{
        ALOG_ERR("adm_cscall_omx_get_dict_mute: mute not available\n");
        res = STE_ADM_RES_IL_ERROR;
    }

cleanup:

    return res;
}



///////////////////////////////////////////////////////////////////////////////
//
//  enable_dict_play
//
//  Activate the playback dictaphone.
//
///////////////////////////////////////////////////////////////////////////////
ste_adm_res_t adm_cscall_omx_enable_dict_play(int num_bufs, int bufsz, int samplerate, int channels)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    ALOG_INFO("IL TRANSITIONS: enable_dict_play\n");
    if(is_slim_graph_creation(&g_cscall_omx_state))
        return adm_cscall_omx_enable_dict_play_slim(num_bufs,bufsz,samplerate,channels);
    else
        return adm_cscall_omx_enable_dict_play_fat(num_bufs,bufsz,samplerate,channels);
}
ste_adm_res_t adm_cscall_omx_enable_dict_play_slim(int num_bufs, int bufsz, int samplerate, int channels)
{
    (void) samplerate;
    (void) channels;
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

    cscall_omx_state_t old_state = g_cscall_omx_state;
    cscall_omx_state_t new_state = old_state;

    ADM_ASSERT(new_state.dict_mix_ul_h     != NULL);
    ADM_ASSERT(new_state.dict_play_split_h == NULL);
    ADM_ASSERT(new_state.dict_app_comp_h   == NULL);

    // NOTE: In case of error during execution of an IL_TOOL_ macro,
    // it will set the 'result' variable below to the resulting error
    // code and then goto cleanup.
    ste_adm_res_t res = STE_ADM_RES_OK;
    srv_become_worker_thread();

#ifdef CFG_FULL
    IL_TOOL_CREATE(&new_state.dict_play_split_h, adaptions_comp_name(ADM_VC_DICT_SPLITTER_NAME), "playsplit");

    OMX_AUDIO_PARAM_PCMMODETYPE  pcm_mode;
    IL_TOOL_INIT_CONFIG_STRUCT(pcm_mode);

    pcm_mode.nPortIndex = ADM_SPEECHPROC_UL_OUT_PORT;
    IL_TOOL_GET_PARAM(new_state.speechproc_h,OMX_IndexParamAudioPcm,&pcm_mode);

    pcm_mode.nPortIndex = 0;
    IL_TOOL_SET_PARAM(new_state.dict_play_split_h, OMX_IndexParamAudioPcm, &pcm_mode);

    pcm_mode.nPortIndex = 1;
    IL_TOOL_SET_PARAM(new_state.dict_play_split_h, OMX_IndexParamAudioPcm, &pcm_mode);

    pcm_mode.nPortIndex = 2;
    IL_TOOL_SET_PARAM(new_state.dict_play_split_h, OMX_IndexParamAudioPcm, &pcm_mode);

    IL_TOOL_DISABLE_ALL_PORTS(new_state.dict_play_split_h);
    IL_TOOL_ENTER_STATE(OMX_StateIdle,      new_state.dict_play_split_h);
    IL_TOOL_ENTER_STATE(OMX_StateExecuting, new_state.dict_play_split_h);

    IL_TOOL_SET_PORT_BUFFER_CONFIG(new_state.dict_play_split_h, 0, num_bufs, bufsz);
    IL_TOOL_ENABLE_PORT_WITH_IO(new_state.dict_play_split_h, 0);

    IL_TOOL_CONNECT_AND_ENABLE_WITH_SRC_CFG(
            new_state.dict_play_split_h, 1,
            new_state.dict_mix_ul_h,     2);

#endif

cleanup:
    srv_become_normal_thread();
    if (res == STE_ADM_RES_OK) {
        g_cscall_omx_state = new_state;
    }

    return res;
}
ste_adm_res_t adm_cscall_omx_enable_dict_play_fat(int num_bufs, int bufsz, int samplerate, int channels)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    ALOG_INFO("IL TRANSITIONS: enable_dict_play\n");

    cscall_omx_state_t old_state = g_cscall_omx_state;
    cscall_omx_state_t new_state = old_state;

    ste_adm_res_t res = STE_ADM_RES_OK;
    srv_become_worker_thread();

    if(adm_get_modem_type()==ADM_FAT_MODEM) {

        ADM_ASSERT(new_state.modem_sink_h == NULL);
        ADM_ASSERT(new_state.dict_mix_ul_h == NULL);
        ADM_ASSERT(new_state.dict_play_split_h == NULL);
        IL_TOOL_CREATE(&new_state.dict_play_split_h, adaptions_comp_name(ADM_ARM_SPLITTER_NAME), "playsplit");
        IL_TOOL_CREATE(&new_state.dict_mix_ul_h, adaptions_comp_name(ADM_ARM_MIXER_NAME), "playmix");
        IL_TOOL_CREATE(&new_state.modem_sink_h, adaptions_comp_name(ADM_MODEM_SINK_NAME), "playsrc");

        OMX_AUDIO_PARAM_PCMMODETYPE  pcm_mode;
        IL_TOOL_INIT_CONFIG_STRUCT(pcm_mode);
        pcm_mode.nPortIndex = 0;
        IL_TOOL_GET_PARAM(new_state.modem_sink_h, OMX_IndexParamAudioPcm, &pcm_mode);
        pcm_mode.nSamplingRate = new_state.cscall_samplerate;
        pcm_mode.nChannels = 1;
        pcm_mode.eChannelMapping[0] = OMX_AUDIO_ChannelCF;
        IL_TOOL_SET_PARAM(new_state.modem_sink_h, OMX_IndexParamAudioPcm, &pcm_mode);
        IL_TOOL_GET_PARAM(new_state.modem_sink_h, OMX_IndexParamAudioPcm, &pcm_mode);

        pcm_mode.nPortIndex = 0;
        IL_TOOL_SET_PARAM(new_state.dict_mix_ul_h, OMX_IndexParamAudioPcm, &pcm_mode);

        pcm_mode.nPortIndex = 1;
        pcm_mode.nSamplingRate = samplerate;
        pcm_mode.nChannels = channels;
        if(channels == STE_ADM_FORMAT_STEREO)
        {
            pcm_mode.eChannelMapping[0] = OMX_AUDIO_ChannelLF;
            pcm_mode.eChannelMapping[1] = OMX_AUDIO_ChannelRF;
        }
        IL_TOOL_SET_PARAM(new_state.dict_mix_ul_h, OMX_IndexParamAudioPcm, &pcm_mode);


        ALOG_INFO("Dictaphone device setttings: samplerate %d [Hz], channel(s) %d\n", samplerate, channels);
        pcm_mode.nPortIndex = 0;
        IL_TOOL_SET_PARAM(new_state.dict_play_split_h, OMX_IndexParamAudioPcm, &pcm_mode);

        pcm_mode.nPortIndex = 1;
        IL_TOOL_SET_PARAM(new_state.dict_play_split_h, OMX_IndexParamAudioPcm, &pcm_mode);

        pcm_mode.nPortIndex = 2;
        IL_TOOL_SET_PARAM(new_state.dict_play_split_h, OMX_IndexParamAudioPcm, &pcm_mode);

        IL_TOOL_DISABLE_ALL_PORTS(new_state.dict_play_split_h);
        IL_TOOL_ENTER_STATE(OMX_StateIdle,        new_state.dict_play_split_h);
        IL_TOOL_ENTER_STATE(OMX_StateExecuting, new_state.dict_play_split_h);

        IL_TOOL_DISABLE_ALL_PORTS(new_state.dict_mix_ul_h);
        IL_TOOL_ENTER_STATE(OMX_StateIdle,        new_state.dict_mix_ul_h);
        IL_TOOL_ENTER_STATE(OMX_StateExecuting, new_state.dict_mix_ul_h);

        IL_TOOL_DISABLE_ALL_PORTS(new_state.modem_sink_h);
        IL_TOOL_ENTER_STATE(OMX_StateIdle,        new_state.modem_sink_h);
        IL_TOOL_ENTER_STATE(OMX_StateExecuting, new_state.modem_sink_h);

        IL_TOOL_SET_PORT_BUFFER_CONFIG(new_state.dict_play_split_h, 0, num_bufs, bufsz);
        IL_TOOL_ENABLE_PORT_WITH_IO(new_state.dict_play_split_h, 0);

        IL_TOOL_CONNECT_AND_ENABLE_WITH_SRC_CFG(
                new_state.dict_play_split_h, 1,
                new_state.dict_mix_ul_h,     1);

    }

    cleanup:
        srv_become_normal_thread();
        if (res == STE_ADM_RES_OK) {
            g_cscall_omx_state = new_state;
        }

    ALOG_INFO("leave enable_dict_play\n");

    return res;
}

///////////////////////////////////////////////////////////////////////////////
//
//  adm_cscall_omx_disable_dict_play_app
//
//  Deactivate the app mixer part of playback dictaphone.
//
///////////////////////////////////////////////////////////////////////////////
ste_adm_res_t adm_cscall_omx_disable_dict_play_app()
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    ALOG_INFO("IL TRANSITIONS: disable_dict_play_app\n");

    cscall_omx_state_t old_state = g_cscall_omx_state;
    cscall_omx_state_t new_state = old_state;

    // NOTE: In case of error during execution of an IL_TOOL_ macro,
    // it will set the 'result' variable below to the resulting error
    // code and then goto cleanup.
    ste_adm_res_t res = STE_ADM_RES_OK;
    srv_become_worker_thread();

    ADM_ASSERT(new_state.dict_app_comp_h);

#ifdef CFG_FULL
    IL_TOOL_DISCONNECT(new_state.dict_play_split_h,  2,
                       new_state.dict_app_comp_h,    new_state.dict_app_comp_port);
#endif

    new_state.dict_app_comp_h = 0;

cleanup:
    srv_become_normal_thread();
    if (res == STE_ADM_RES_OK) {
        g_cscall_omx_state = new_state;
    }

    return res;
}


///////////////////////////////////////////////////////////////////////////////
//
//  disable_dict_play
//
//  Deactivate the playback dictaphone.
//  adm_cscall_omx_disable_dict_play_app() must be called before this
//  function is called.
//
///////////////////////////////////////////////////////////////////////////////
ste_adm_res_t adm_cscall_omx_disable_dict_play()
{
    ALOG_INFO("IL TRANSITIONS: disable_dict_play\n");
    if(is_slim_graph_creation(&g_cscall_omx_state))
        return adm_cscall_omx_disable_dict_play_slim();
    else
        return adm_cscall_omx_disable_dict_play_fat();
}
ste_adm_res_t adm_cscall_omx_disable_dict_play_slim()
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

    cscall_omx_state_t old_state = g_cscall_omx_state;
    cscall_omx_state_t new_state = old_state;

    ADM_ASSERT(new_state.dict_play_split_h != NULL);
    ADM_ASSERT(new_state.dict_mix_ul_h     != NULL);

    // Must call adm_cscall_omx_disable_dict_play_app first
    ADM_ASSERT(new_state.dict_app_comp_h   == NULL);

    // NOTE: In case of error during execution of an IL_TOOL_ macro,
    // it will set the 'result' variable below to the resulting error
    // code and then goto cleanup.
    ste_adm_res_t res = STE_ADM_RES_OK;
    srv_become_worker_thread();


#ifdef CFG_FULL
    IL_TOOL_DISABLE_PORT_WITH_IO(new_state.dict_play_split_h, 0);

    IL_TOOL_DISCONNECT(new_state.dict_play_split_h, 1,
                       new_state.dict_mix_ul_h,     2);

    IL_TOOL_ENTER_STATE(OMX_StateIdle,   new_state.dict_play_split_h);
    IL_TOOL_ENTER_STATE(OMX_StateLoaded, new_state.dict_play_split_h);
    IL_TOOL_DESTROY(&new_state.dict_play_split_h);
#endif

cleanup:
    srv_become_normal_thread();
    if (res == STE_ADM_RES_OK) {
        g_cscall_omx_state = new_state;
    }

    return res;
}
ste_adm_res_t adm_cscall_omx_disable_dict_play_fat()
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

    ste_adm_res_t res = STE_ADM_RES_OK;
    srv_become_worker_thread();

    return res;
}



ste_adm_res_t adm_cscall_omx_enable_dict_play_app(il_comp_t new_dict_app_comp_h,
        OMX_U32 new_dict_app_comp_port)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    ALOG_INFO("IL TRANSITIONS: enable_dict_play_app\n");

    cscall_omx_state_t old_state = g_cscall_omx_state;
    cscall_omx_state_t new_state = old_state;

    // NOTE: In case of error during execution of an IL_TOOL_ macro,
    // it will set the 'result' variable below to the resulting error
    // code and then goto cleanup.
    ste_adm_res_t res = STE_ADM_RES_OK;
    srv_become_worker_thread();

    ADM_ASSERT(new_state.dict_mix_ul_h     != NULL);
    ADM_ASSERT(new_state.dict_play_split_h != NULL);
    ADM_ASSERT(new_state.dict_app_comp_h   == NULL);

#ifdef CFG_FULL
    IL_TOOL_CONNECT_AND_ENABLE_WITH_SRC_CFG(
            new_state.dict_play_split_h, 2,
            new_dict_app_comp_h,         new_dict_app_comp_port);
#endif

    new_state.dict_app_comp_h    = new_dict_app_comp_h;
    new_state.dict_app_comp_port = new_dict_app_comp_port;


cleanup:
    srv_become_normal_thread();
    if (res == STE_ADM_RES_OK) {
        g_cscall_omx_state = new_state;
    }

    return res;
}


///////////////////////////////////////////////////////////////////////////////
//
//  adm_cscall_get_dict_rec_comp
//
///////////////////////////////////////////////////////////////////////////////
int adm_cscall_get_dict_rec_comp(il_comp_t* comp_p, OMX_U32* port_p)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    ALOG_INFO("IL TRANSITIONS: disable_dict_play\n");
    if(is_slim_graph_creation(&g_cscall_omx_state))
        return adm_cscall_get_dict_rec_comp_slim(comp_p,port_p);
    else
        return adm_cscall_get_dict_rec_comp_fat(comp_p,port_p);
}
int adm_cscall_get_dict_rec_comp_slim(il_comp_t* comp_p, OMX_U32* port_p)
{
    if (g_cscall_omx_state.dict_rec_mix_h == 0) {
        return 0;
    } else {
        *comp_p = g_cscall_omx_state.dict_rec_mix_h;
        *port_p = 0;
        return 1;
    }
}
int adm_cscall_get_dict_rec_comp_fat(il_comp_t* comp_p, OMX_U32* port_p)
{
     if (g_cscall_omx_state.dict_rec_mix_h == 0) {
         return 0;
     } else {
         *comp_p = g_cscall_omx_state.dict_rec_mix_h;
         *port_p = 0;
         return 1;
     }

}

///////////////////////////////////////////////////////////////////////////////
//
//  adm_cscall_get_dict_play_comp
//
///////////////////////////////////////////////////////////////////////////////
int adm_cscall_get_dict_play_comp(il_comp_t* comp_p, OMX_U32* port_p)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    ALOG_INFO("IL TRANSITIONS: disable_dict_play\n");
    if(is_slim_graph_creation(&g_cscall_omx_state))
        return adm_cscall_get_dict_play_comp_slim(comp_p,port_p);
    else
        return adm_cscall_get_dict_play_comp_fat(comp_p,port_p);
}
int adm_cscall_get_dict_play_comp_slim(il_comp_t* comp_p, OMX_U32* port_p)
{
    if (g_cscall_omx_state.dict_play_split_h == 0) {
        return 0;
    } else {
        *comp_p = g_cscall_omx_state.dict_play_split_h;
        *port_p = 0;
        return 1;
    }
}
int adm_cscall_get_dict_play_comp_fat(il_comp_t* comp_p, OMX_U32* port_p)
{
   if (g_cscall_omx_state.dict_play_split_h == 0) {
        return 0;
    } else {
        *comp_p = g_cscall_omx_state.dict_play_split_h;
        *port_p = 0;
        return 1;
    }
}

///////////////////////////////////////////////////////////////////////////////
//
//  adm_cscall_omx_enable_voip
//
///////////////////////////////////////////////////////////////////////////////
ste_adm_res_t adm_cscall_omx_enable_voip(int in_samplerate, int in_channels,
    int out_samplerate, int out_channels)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    ste_adm_res_t res = STE_ADM_RES_OK;
    ALOG_INFO("Enable VoIP. Input (UL) [%d, %d], Output (DL) [%d, %d]\n",
        in_samplerate,in_channels, out_samplerate,out_channels);
    g_cscall_omx_state.voip_enabled = 1;
    g_cscall_omx_state.voip_ul_samplerate = in_samplerate;
    g_cscall_omx_state.voip_ul_channels = in_channels;
    g_cscall_omx_state.voip_dl_samplerate = out_samplerate;
    g_cscall_omx_state.voip_dl_channels = out_channels;
    g_cscall_omx_state.cscall_samplerate = STE_ADM_VOIP_SPEECHPROC_SAMPLERATE;
    ALOG_INFO("Speech proc samplerate = %d\n", g_cscall_omx_state.cscall_samplerate);
    return res;
}

///////////////////////////////////////////////////////////////////////////////
//
//  adm_cscall_omx_disable_voip
//
///////////////////////////////////////////////////////////////////////////////
ste_adm_res_t adm_cscall_omx_disable_voip()
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    ste_adm_res_t res = STE_ADM_RES_OK;
    ALOG_INFO("Disable VoIP\n");
    g_cscall_omx_state.voip_enabled = 0;
    g_cscall_omx_state.voip_ul_samplerate = 0;
    g_cscall_omx_state.voip_ul_channels = 0;
    g_cscall_omx_state.voip_dl_samplerate = 0;
    g_cscall_omx_state.voip_dl_channels = 0;
    return res;
}

///////////////////////////////////////////////////////////////////////////////
//
//  adm_cscall_get_voip_in_comp
//
///////////////////////////////////////////////////////////////////////////////
int adm_cscall_get_voip_in_comp(il_comp_t* comp_p, OMX_U32* port_p)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

#ifdef CFG_NO_DICT
    if (g_cscall_omx_state.dict_mix_ul_h == 0) {
        return 0;
    } else {
        *comp_p = g_cscall_omx_state.dict_mix_ul_h;
        *port_p = MIXER_OUTPUT_PORT;
        return 1;
    }
#endif

#ifdef CFG_CSCALL_ONLY
    if (g_cscall_omx_state.capturer_h == 0) {
        return 0;
    } else {
        *comp_p = g_cscall_omx_state.capturer_h;
        *port_p = g_cscall_omx_state.capturer_port;
        return 1;
    }
#endif

#ifdef CFG_FULL
    if (g_cscall_omx_state.dict_split_ul_h == 0) {
        return 0;
    } else {
        *comp_p = g_cscall_omx_state.dict_split_ul_h;
        *port_p = SPLITTER_OUTPUT_PORT;
        return 1;
    }
#endif

    return 0;
}

///////////////////////////////////////////////////////////////////////////////
//
//  adm_cscall_get_voip_out_comp
//
///////////////////////////////////////////////////////////////////////////////
int adm_cscall_get_voip_out_comp(il_comp_t* comp_p, OMX_U32* port_p)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

#ifdef CFG_NO_DICT
    if (g_cscall_omx_state.voip_mix_dl_h == 0) {
        return 0;
    } else {
        *comp_p = g_cscall_omx_state.voip_mix_dl_h;
        *port_p = MIXER_INPUT_PORT;
        return 1;
    }
#endif

#ifdef CFG_CSCALL_ONLY
    if (g_cscall_omx_state.renderer_h == 0) {
        return 0;
    } else {
        *comp_p = g_cscall_omx_state.renderer_h;
        *port_p = g_cscall_omx_state.renderer_port;
        return 1;
    }
#endif

#ifdef CFG_FULL
    if (g_cscall_omx_state.dict_split_dl_h == 0) {
        return 0;
    } else {
        *comp_p = g_cscall_omx_state.dict_split_dl_h;
        *port_p = SPLITTER_INPUT_PORT;
        return 1;
    }
#endif

    return 0;
}

///////////////////////////////////////////////////////////////////////////////
//
//  configure_cscall_comp_slim
//
//  Download settings to cscall component.
//  Caller is responsible for restoring state on failure
//
///////////////////////////////////////////////////////////////////////////////
static ste_adm_res_t configure_cscall_comp_slim(cscall_omx_state_t* state_p)
{
    SRV_DBG_ASSERT_IS_WORKER_THREAD;
    ALOG_INFO("Called configure for cscall slim\n");

    // NOTE: In case of error during execution of an IL_TOOL_ macro,
    // it will set the 'result' variable below to the resulting error
    // code and then goto cleanup.
    ste_adm_res_t res = STE_ADM_RES_OK;


    /* port settings */
    OMX_PARAM_PORTDEFINITIONTYPE port_def;
    IL_TOOL_INIT_CONFIG_STRUCT(port_def);

    ALOG_INFO("Setting port parameters for cscall component ports (output:0,input:1)\n");
    port_def.nPortIndex = ADM_CSCALL_DL_PORT;
    IL_TOOL_GET_PARAM(state_p->voice_srcsnk_h, OMX_IndexParamPortDefinition, &port_def);

    port_def.nBufferCountActual = STE_ADM_CSCALL_DL_NOBUFS;
    IL_TOOL_SET_PARAM(state_p->voice_srcsnk_h, OMX_IndexParamPortDefinition, &port_def);


    port_def.nPortIndex = ADM_CSCALL_UL_PORT;
    IL_TOOL_GET_PARAM(state_p->voice_srcsnk_h, OMX_IndexParamPortDefinition, &port_def);

    port_def.nBufferCountActual = STE_ADM_CSCALL_UL_NOBUFS;
    IL_TOOL_SET_PARAM(state_p->voice_srcsnk_h, OMX_IndexParamPortDefinition, &port_def);



    /* pcm data */
    OMX_AUDIO_PARAM_PCMMODETYPE  pcm_mode;
    IL_TOOL_INIT_CONFIG_STRUCT(pcm_mode);

    ALOG_INFO("Setting PCM parameters for voice src/snk ports (output:0,input:1)\n");
    pcm_mode.nPortIndex = ADM_CSCALL_DL_PORT;
    IL_TOOL_GET_PARAM(state_p->voice_srcsnk_h, OMX_IndexParamAudioPcm, &pcm_mode);

    pcm_mode.nChannels     = STE_ADM_CSCALL_VOICE_NOCHAN;
    pcm_mode.eEndian       = OMX_EndianLittle;
    pcm_mode.nSamplingRate = (OMX_U32)state_p->cscall_samplerate;
    pcm_mode.eChannelMapping[0]= OMX_AUDIO_ChannelCF;
    IL_TOOL_SET_PARAM(state_p->voice_srcsnk_h, OMX_IndexParamAudioPcm, &pcm_mode);


    pcm_mode.nPortIndex = ADM_CSCALL_UL_PORT;
    IL_TOOL_GET_PARAM(state_p->voice_srcsnk_h, OMX_IndexParamAudioPcm, &pcm_mode);

    pcm_mode.nChannels     = STE_ADM_CSCALL_VOICE_NOCHAN;
    pcm_mode.eEndian       = OMX_EndianLittle;
    pcm_mode.nSamplingRate = (OMX_U32) state_p->cscall_samplerate;
    pcm_mode.eChannelMapping[0]= OMX_AUDIO_ChannelCF;
    IL_TOOL_SET_PARAM(state_p->voice_srcsnk_h, OMX_IndexParamAudioPcm, &pcm_mode);

cleanup:
    return res;
}

///////////////////////////////////////////////////////////////////////////////
//
//  configure_cscall_comp_fat
//
//  Download settings to cscall component.
//  Caller is responsible for restoring state on failure
//
///////////////////////////////////////////////////////////////////////////////
static ste_adm_res_t configure_cscall_comp_fat(cscall_omx_state_t* state_p)
{
    SRV_DBG_ASSERT_IS_WORKER_THREAD;
    ALOG_INFO("Called configure for cscall fat\n");

    // NOTE: In case of error during execution of an IL_TOOL_ macro,
    // it will set the 'result' variable below to the resulting error
    // code and then goto cleanup.
    ste_adm_res_t res = STE_ADM_RES_OK;


    /* port settings */
    OMX_PARAM_PORTDEFINITIONTYPE port_def;
    IL_TOOL_INIT_CONFIG_STRUCT(port_def);

    ALOG_INFO("Setting port parameters for cscall component ports (output:0,input:1)\n");
    port_def.nPortIndex = ADM_CSCALL_DL_PORT;
    IL_TOOL_GET_PARAM(state_p->voice_srcsnk_h, OMX_IndexParamPortDefinition, &port_def);

    port_def.nBufferCountActual = STE_ADM_CSCALL_DL_NOBUFS;
    IL_TOOL_SET_PARAM(state_p->voice_srcsnk_h, OMX_IndexParamPortDefinition, &port_def);


    port_def.nPortIndex = ADM_CSCALL_UL_PORT;
    IL_TOOL_GET_PARAM(state_p->voice_srcsnk_h, OMX_IndexParamPortDefinition, &port_def);

    port_def.nBufferCountActual = STE_ADM_CSCALL_UL_NOBUFS;
    IL_TOOL_SET_PARAM(state_p->voice_srcsnk_h, OMX_IndexParamPortDefinition, &port_def);



    /* pcm data */
    OMX_AUDIO_PARAM_PCMMODETYPE  pcm_mode;
    IL_TOOL_INIT_CONFIG_STRUCT(pcm_mode);

    ALOG_INFO("Setting PCM parameters for voice src/snk ports (output:0,input:1)\n");
    pcm_mode.nPortIndex = ADM_CSCALL_DL_PORT;
    IL_TOOL_GET_PARAM(state_p->voice_srcsnk_h, OMX_IndexParamAudioPcm, &pcm_mode);

    pcm_mode.nChannels     = STE_ADM_CSCALL_VOICE_NOCHAN;
    pcm_mode.eEndian       = OMX_EndianLittle;
    pcm_mode.nSamplingRate = (OMX_U32)state_p->cscall_samplerate;
    pcm_mode.eChannelMapping[0]= OMX_AUDIO_ChannelCF;
    IL_TOOL_SET_PARAM(state_p->voice_srcsnk_h, OMX_IndexParamAudioPcm, &pcm_mode);


    pcm_mode.nPortIndex = ADM_CSCALL_UL_PORT;
    IL_TOOL_GET_PARAM(state_p->voice_srcsnk_h, OMX_IndexParamAudioPcm, &pcm_mode);

    pcm_mode.nChannels     = STE_ADM_CSCALL_VOICE_NOCHAN;
    pcm_mode.eEndian       = OMX_EndianLittle;
    pcm_mode.nSamplingRate = (OMX_U32) state_p->cscall_samplerate;
    pcm_mode.eChannelMapping[0]= OMX_AUDIO_ChannelCF;
    IL_TOOL_SET_PARAM(state_p->voice_srcsnk_h, OMX_IndexParamAudioPcm, &pcm_mode);

#ifdef CFG_ENABLE_ECHO_REFERENCE
    pcm_mode.nPortIndex = ADM_CSCALL_MPC_REF_PORT;
    IL_TOOL_GET_PARAM(state_p->voice_srcsnk_h, OMX_IndexParamAudioPcm, &pcm_mode);


    pcm_mode.nChannels     = STE_ADM_CSCALL_VOICE_NOCHAN;
    pcm_mode.eEndian       = OMX_EndianLittle;
    pcm_mode.nSamplingRate = (OMX_U32) state_p->cscall_samplerate;
    pcm_mode.eChannelMapping[0]= OMX_AUDIO_ChannelCF;
    IL_TOOL_SET_PARAM(state_p->voice_srcsnk_h, OMX_IndexParamAudioPcm, &pcm_mode);
#endif // CFG_ENABLE_ECHO_REFERENCE

cleanup:
    return res;
}

ste_adm_res_t adm_cscall_omx_rescan_speech_proc(const char* input_name, const char* output_name)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

    srv_become_worker_thread();
    ste_adm_res_t res = configure_speechproc_comp(&g_cscall_omx_state, input_name, output_name);
    srv_become_normal_thread();
    return res;
}
static ste_adm_res_t configure_speechproc_comp(cscall_omx_state_t* state_p, const char* input_name, const char* output_name)
{
    if(is_slim_graph_creation(&g_cscall_omx_state))
        return configure_speechproc_comp_slim(state_p,input_name,output_name);
    else
        return configure_speechproc_comp_fat(state_p,input_name,output_name);
}
static ste_adm_res_t configure_speechproc_comp_slim(cscall_omx_state_t* state_p, const char* input_name, const char* output_name)
{
    SRV_DBG_ASSERT_IS_WORKER_THREAD;
    ALOG_INFO("Configuring speech proc from database [%s,%s]\n", input_name, output_name);


    adm_db_cfg_iter_t db_iterator;
    memset(&db_iterator, 0, sizeof(db_iterator));

    ste_adm_res_t res = adm_db_speech_cfg_iter_create(input_name, output_name,
            state_p->cscall_samplerate, &db_iterator);
    if (res != STE_ADM_RES_OK) {
        ALOG_ERR("Failed to extract config iterator for speech proc db\n");
        goto cleanup;
    }

    res = adm_setup_component(state_p->speechproc_h, &db_iterator, 0, 1);
    if (res != STE_ADM_RES_OK) {
        ALOG_ERR("Failed to setup speech proc component from database\n");
        adm_db_cfg_iter_destroy(&db_iterator);
        goto cleanup;
    }
    adm_db_cfg_iter_destroy(&db_iterator);

cleanup:
    ALOG_INFO("speech proc configuration done, result = %d\n", res);
    return res;
}
static ste_adm_res_t configure_speechproc_comp_fat(cscall_omx_state_t* state_p, const char* input_name, const char* output_name)
{
    SRV_DBG_ASSERT_IS_WORKER_THREAD;

    // If the internal Speech proc is not used, it is the modem speech proc that it is used.
    // Send An At command to update the algorithm
    return modem_configure_speech_proc(state_p->cscall_samplerate,input_name,output_name);


}
ste_adm_res_t start_async_configure_speechproc_comp(const char* input_name, const char* output_name, int *start_async_done)
{
    ALOG_INFO("start_async_configure_speechproc_comp");
    // In case of FAT modem
    if(is_slim_graph_creation(&g_cscall_omx_state))
    {
        if(start_async_done!=NULL)
            *start_async_done = 0;
        return STE_ADM_RES_OK;
    }
    else
    {
        if(start_async_done!=NULL)
            *start_async_done = 1;
        return adm_cscall_omx_rescan_speech_proc( input_name,output_name);
    }
}

ste_adm_res_t wait_end_of_async_configure_speechproc_comp(void)
{
    ALOG_INFO("wait_end_of_async_configure_speechproc_comp");
    if(is_slim_graph_creation(&g_cscall_omx_state))
        return STE_ADM_RES_OK;
    else {
        srv_become_worker_thread();
        ste_adm_res_t res = modem_wait_end_of_configure_speechproc_comp();
        srv_become_normal_thread();
        return res;
    }
}

ste_adm_res_t cscall_omx_set_loopback_mode(int loopback_enabled, int codec_type)
{
    ste_adm_res_t res = STE_ADM_RES_OK;
    int type = adm_get_modem_type();
    switch(type) {
        case ADM_FAT_MODEM:
            res = cscall_omx_set_loopback_mode_fat(loopback_enabled,codec_type);
            ALOG_INFO("Loopback on CSCALL MPC\n");
            break;
        case ADM_FULL_FAT_MODEM:
            res = STE_ADM_RES_OK;
            ALOG_ERR("Loopback currently not supported for Full Fat\n");
            break;
        case ADM_SLIM_MODEM:
        case ADM_HALF_SLIM_MODEM:
            res = cscall_omx_set_loopback_mode_slim(loopback_enabled,codec_type);
            ALOG_INFO("Loopback on CSCALL slim");
            break;
        case ADM_UNKNOWN_MODEM_TYPE:
            res = STE_ADM_RES_INTERNAL_ERROR;
            ALOG_ERR("Unknown Modem Type");
            break;
    }

    return res;
}

ste_adm_res_t cscall_omx_set_loopback_mode_slim(int loopback_enabled, int codec_type)
{
    int  n_try;
    ste_adm_res_t res = STE_ADM_RES_OK;
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    ALOG_INFO("Called ste_adm_cscall_omx_set_loopback_mode : %d, codec_type : %d\n", loopback_enabled, codec_type);

    if  (g_cscall_omx_state.voice_srcsnk_h ==NULL)
    {
        ALOG_INFO("ste_adm_cscall_omx_set_loopback_mode Modem Not started => initialize cscall component");
        /*Modem is not working  so load cscall component  in order to active CSLoopBack.*/
        res = adm_cscall_create_cscall_cmp_slim(&g_cscall_omx_state); 
        if (res !=  STE_ADM_RES_OK)
        {
            ALOG_ERR("Impossible to Load cscall component\n");
            return res;
        }
    }

    cscall_omx_state_t old_state = g_cscall_omx_state;
    cscall_omx_state_t new_state = old_state;

    ADM_ASSERT(old_state.voice_srcsnk_h != NULL);

    // NOTE: In case of error during execution of an IL_TOOL_ macro,
    // it will set the 'result' variable below to the resulting error
    // code and then goto cleanup.

    srv_become_worker_thread();

    {
        OMX_STATETYPE state;
        ADM_ASSERT(il_tool_GetState(old_state.voice_srcsnk_h, &state) == STE_ADM_RES_OK && state == OMX_StateExecuting);
        (void)state;
    }


    OMX_AUDIO_CODINGTYPE      codingType;
    OMX_AUDIO_AMRBANDMODETYPE BandModeType = 0;

    if (loopback_enabled) {
        switch (codec_type) {
            case 10: codingType=OMX_AUDIO_CodingGSMFR;  break;
            case 9:  codingType=OMX_AUDIO_CodingGSMHR;  break;
            case 8:  codingType=OMX_AUDIO_CodingGSMEFR; break;
            case 7:  codingType=OMX_AUDIO_CodingAMR; BandModeType=OMX_AUDIO_AMRBandModeNB7; break;
            case 6:  codingType=OMX_AUDIO_CodingAMR; BandModeType=OMX_AUDIO_AMRBandModeNB6; break;
            case 5:  codingType=OMX_AUDIO_CodingAMR; BandModeType=OMX_AUDIO_AMRBandModeNB5; break;
            case 4:  codingType=OMX_AUDIO_CodingAMR; BandModeType=OMX_AUDIO_AMRBandModeNB4; break;
            case 3:  codingType=OMX_AUDIO_CodingAMR; BandModeType=OMX_AUDIO_AMRBandModeNB3; break;
            case 2:  codingType=OMX_AUDIO_CodingAMR; BandModeType=OMX_AUDIO_AMRBandModeNB2; break;
            case 1:  codingType=OMX_AUDIO_CodingAMR; BandModeType=OMX_AUDIO_AMRBandModeNB1; break;
            case 0:  codingType=OMX_AUDIO_CodingAMR; BandModeType=OMX_AUDIO_AMRBandModeNB0; break;
            default:
                ALOG_ERR("Unknown codec type %d\n", codec_type);
                res = STE_ADM_RES_INVALID_PARAMETER;
                goto cleanup;
        }

        IL_TOOL_ENTER_STATE(OMX_StateIdle,   old_state.voice_srcsnk_h);
        IL_TOOL_ENTER_STATE(OMX_StateLoaded, old_state.voice_srcsnk_h);


        //
        // set loopback mode
        //
        OMX_SYMBIAN_AUDIO_CONFIG_SPEECHLOOPSTYPE cb_config;
        IL_TOOL_INIT_CONFIG_STRUCT(cb_config);
        cb_config.bLoopDownlinkUplink    = OMX_FALSE;
        cb_config.bLoopDownlinkUplinkAlg = OMX_FALSE;
        cb_config.bLoopUplinkDownlink    = OMX_FALSE;
        cb_config.bLoopUplinkDownlinkAlg = OMX_TRUE;
        IL_TOOL_SET_CONFIG(old_state.voice_srcsnk_h,
                           OMX_Symbian_IndexConfigAudioSpeechLoops, &cb_config);

        IL_TOOL_ENTER_STATE(OMX_StateIdle,      old_state.voice_srcsnk_h);
        IL_TOOL_ENTER_STATE(OMX_StateExecuting, old_state.voice_srcsnk_h);


        for(n_try =4; n_try != 0; n_try--)
        {
            OMX_STATETYPE state;
            il_tool_GetState(old_state.voice_srcsnk_h, &state);
            if(state == OMX_StateExecuting) {
                break;
            }
            else {
                usleep(5*1000);
            }
        }

        if(n_try == 0) {
          ALOG_ERR("CScall component not in executing mode as expected");
          res = STE_ADM_RES_INCORRECT_STATE;
          goto cleanup;
        }

        //
        // set timing
        //
        AFM_AUDIO_CONFIG_CSCALL_UPLINK_TIMING_REQ timing_config;
        IL_TOOL_INIT_CONFIG_STRUCT(timing_config);
        timing_config.nDeliveryTime        = 30000;
        timing_config.nModemProcessingTime = 0;
        timing_config.nCause               = 2;
        IL_TOOL_SET_CONFIG(old_state.voice_srcsnk_h,
                           AFM_IndexConfigCscallSpeechTimingReq, &timing_config);

        //
        // set codec type
        //
        AFM_AUDIO_CONFIG_CSCALL_SPEECH_CODEC_REQ codec_config;
        IL_TOOL_INIT_CONFIG_STRUCT(codec_config);
        codec_config.nNetworkType                = AFM_AUDIO_NETWORK_GSM;
        codec_config.nCodingType                 = codingType;
        codec_config.encoderconfig.bAAC          = OMX_FALSE;
        codec_config.encoderconfig.bNsynch       = OMX_FALSE;
        codec_config.encoderconfig.nSFN          = 0;
        codec_config.encoderconfig.nBandModeType = BandModeType;
        codec_config.encoderconfig.bDTX          = OMX_FALSE;
        codec_config.nBandModeTypeDecoder        = BandModeType;
        codec_config.codecInitialization         = OMX_TRUE;
        codec_config.bPCMcodecIn16K              = OMX_FALSE;
        IL_TOOL_SET_CONFIG(old_state.voice_srcsnk_h,
                           AFM_IndexConfigCscallSpeechCodecReq, &codec_config);

        ALOG_STATUS("loopback enabled\n");
    } else {
        //
        // set codec type to none
        //
        AFM_AUDIO_CONFIG_CSCALL_SPEECH_CODEC_REQ codec_config;
        IL_TOOL_INIT_CONFIG_STRUCT(codec_config);
        codec_config.nCodingType = OMX_AUDIO_CodingUnused;
        IL_TOOL_SET_CONFIG(old_state.voice_srcsnk_h,
                           AFM_IndexConfigCscallSpeechCodecReq, &codec_config);

        IL_TOOL_ENTER_STATE(OMX_StateIdle,   old_state.voice_srcsnk_h);
        IL_TOOL_ENTER_STATE(OMX_StateLoaded, old_state.voice_srcsnk_h);


        //
        // set algorithm type to none
        //
        OMX_SYMBIAN_AUDIO_CONFIG_SPEECHLOOPSTYPE cb_config;
        IL_TOOL_INIT_CONFIG_STRUCT(cb_config);
        cb_config.bLoopDownlinkUplink    = OMX_FALSE;
        cb_config.bLoopDownlinkUplinkAlg = OMX_FALSE;
        cb_config.bLoopUplinkDownlink    = OMX_FALSE;
        cb_config.bLoopUplinkDownlinkAlg = OMX_FALSE;
        IL_TOOL_SET_CONFIG(old_state.voice_srcsnk_h,
                           OMX_Symbian_IndexConfigAudioSpeechLoops, &cb_config);


        IL_TOOL_ENTER_STATE(OMX_StateIdle,      old_state.voice_srcsnk_h);
        IL_TOOL_ENTER_STATE(OMX_StateExecuting, old_state.voice_srcsnk_h);

        ALOG_STATUS("loopback disabled\n");
    }

cleanup:
    srv_become_normal_thread();
    if (res == STE_ADM_RES_OK) {
        g_cscall_omx_state = new_state;
    }
    return res;
}

ste_adm_res_t cscall_omx_set_loopback_mode_fat(int loopback_type, int codec_type)
{
    static int current_loopback_type = 0;

    ALOG_INFO("cscall_omx_set_loopback_mode_fat(%d,%d) with current_loopback_type = %d\n", loopback_type,  codec_type, current_loopback_type);

    if((loopback_type!=current_loopback_type) && (loopback_type!=0) && (current_loopback_type!=0) ) {
         ALOG_INFO("Deactivate previous loop back => %d ", current_loopback_type);
        /*If we change loop back without deactivated the old one -> deactivate the old one before to activate the new one*/
        if(current_loopback_type>2) {
            modem_set_modem_loop(current_loopback_type, 0,0);
        }
        else {
            g_cscall_omx_state.loopback_enabled = 0;
            g_cscall_omx_state.codec_type = 0 ;
        }
    }

    /*If loop is disable, check old loop used*/
    if(loopback_type==0){
        ALOG_INFO("Deactivate loop back => %d ", loopback_type);
        if(current_loopback_type>2){
            modem_set_modem_loop(current_loopback_type, 0,0);
        }
        else{
            g_cscall_omx_state.loopback_enabled = 0;
            g_cscall_omx_state.codec_type = 0 ;
        }
    }
    else {
        ALOG_INFO("Activate loop back => %d ", loopback_type);
        if(loopback_type>2){
            modem_set_modem_loop(loopback_type, 1,codec_type);
        }
        else {
            g_cscall_omx_state.loopback_enabled = loopback_type;
            g_cscall_omx_state.codec_type = 0 ;
        }
    }
    current_loopback_type = loopback_type;
    return STE_ADM_RES_OK;
}


///////////////////////////////////////////////////////////////////////////////
//
//  codec_enabled_callback
//
//  This callback is intended to handle the OMX_Symbian_IndexConfigAudioCodecEnabled
//  and OMX_Symbian_IndexConfigAudioCodecDisabled callbacks.
//
///////////////////////////////////////////////////////////////////////////////
static void cscall_adm_context_cb(void* param, il_comp_t hComponent,
        OMX_EVENTEXTTYPE eEvent,
        OMX_U32 Data1, OMX_U32 Data2, OMX_PTR pEventData)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

    ALOG_INFO("cscall_adm_context_cb received\n");
    ADM_ASSERT(hComponent == g_cscall_omx_state.voice_srcsnk_h);
    (void) param;
    (void) pEventData;
    (void) hComponent;
    if (eEvent == OMX_EventIndexSettingChanged) {
        if (Data1 == OMX_ALL && Data2 == OMX_Symbian_IndexConfigAudioCodecEnabled) {
            if (g_cscall_omx_state.cscall_codec_active) {
                ALOG_WARN("OMX_Symbian_IndexConfigAudioCodecEnabled received when codec already active; ignoring\n");
            } else {
                ALOG_INFO("OMX_Symbian_IndexConfigAudioCodecEnabled processed\n");
                ADM_ASSERT(g_cscall_omx_state.codec_enabled_fp != NULL);
                g_cscall_omx_state.cscall_codec_active = 1;
                g_cscall_omx_state.codec_enabled_fp();
            }
        }

        if (Data1 == OMX_ALL && Data2 == OMX_Symbian_IndexConfigAudioCodecDisabled) {
            if (g_cscall_omx_state.cscall_codec_active) {
                ALOG_INFO("OMX_Symbian_IndexConfigAudioCodecDisabled processed\n");
                g_cscall_omx_state.cscall_codec_active = 0;
                ADM_ASSERT(g_cscall_omx_state.codec_disabled_fp != NULL);
                g_cscall_omx_state.codec_disabled_fp();

            } else {
                ALOG_WARN("OMX_Symbian_IndexConfigAudioCodecDisabled received when codec already disabled; ignoring\n");
            }
        }

        if (Data1 == OMX_ALL && Data2 == OMX_Symbian_IndexConfigAudioSampleRate8khz) {
            if (csapi_cscall_active())
            {
                ALOG_INFO("OMX_Symbian_IndexConfigAudioSampleRate8khz ignored. Active Call\n");
            } else {
                ALOG_INFO("OMX_Symbian_IndexConfigAudioSampleRate8khz processed\n");
                ADM_ASSERT(g_cscall_omx_state.cscall_samplerate_fp != NULL);
                g_cscall_omx_state.cscall_samplerate = 8000;
                g_cscall_omx_state.cscall_samplerate_fp();
            }
        }

        if (Data1 == OMX_ALL && Data2 == OMX_Symbian_IndexConfigAudioSampleRate16khz) {
            if (csapi_cscall_active())
            {
                ALOG_INFO("OMX_Symbian_IndexConfigAudioSampleRate16khz ignored. Active Call\n");
            } else {
                ALOG_INFO("OMX_Symbian_IndexConfigAudioSampleRate16khz processed\n");
                ADM_ASSERT(g_cscall_omx_state.cscall_samplerate_fp != NULL);
                g_cscall_omx_state.cscall_samplerate = 16000;
                g_cscall_omx_state.cscall_samplerate_fp();
            }
        }
    }
}
