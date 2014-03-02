/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#define LOG_TAG "ADM_OMX"
#define ADM_LOG_FILENAME "omx_graph"
#include "ste_adm_dbg.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <sys/socket.h>
#include <string.h>

#include "OMX_Component.h"
#include "OMX_Types.h"
#include "OMX_Core.h"
#include "sqlite3.h"

#include "AFM_Index.h"
#include "AFM_Types.h"
#include "audio_codecs_chipset_api.h"
#include "audio_chipset_api_index.h"

#include "ste_adm_dev_omx.h"
#include "ste_adm_dev_omx_util.h"
#include "ste_adm_db.h"
#include "ste_adm_dev_omx_graph.h"
#include "ste_adm_platform_adaptions.h"

#define RETURN_IF_FAIL(__f) \
    { \
        ste_adm_res_t r = __f; \
        if (r != STE_ADM_RES_OK) { \
            ALOG_ERR("Error %d in '%s'\n", r, # __f); \
            return r; \
        } \
    }

#ifdef ADM_SRC_MODE
int  outsamplerate = 0;
#endif

/**
 * Allocate memory for the common graph
 */
ste_adm_res_t adm_omx_graph_init_common(dev_omx_graph_t* graph_p, int is_hdmi)
{
    /* Initialize the Common effect chain */
    graph_p->common.nbr_of_comp   = 0;
    graph_p->common.nbr_mixer_effects = 0;

    if (graph_p->is_input) {
        /* Initialize the Splitter for App and Voice chains */
        RETURN_IF_FAIL(il_tool_create_component(
                &(graph_p->common.mixer_splitter_handle),
                adaptions_comp_name(ADM_DEFAULT_SPLITTER_NAME), "common"));

        ALOG_INFO("il_tool_create_component returned 0x%08X for %s (Common Splitter)\n",
            (unsigned int) il_tool_raw_handle(graph_p->common.mixer_splitter_handle),
            adaptions_comp_name(ADM_DEFAULT_SPLITTER_NAME));

        OMX_SYMBIAN_AUDIO_CONFIG_CHANNELSWITCHBOARDTYPE switchboard_config;
        IL_TOOL_INIT_CONFIG_STRUCT(switchboard_config);
        switchboard_config.nPortIndex = 1;
        switchboard_config.nOutputChannel[0] = 0;
        switchboard_config.nOutputChannel[1] = 1;
        RETURN_IF_FAIL(il_tool_SetConfig(graph_p->common.mixer_splitter_handle,
                OMX_IndexConfigAudioChannelSwitchBoard, &switchboard_config));

        switchboard_config.nPortIndex = 2;
        RETURN_IF_FAIL(il_tool_SetConfig(graph_p->common.mixer_splitter_handle,
                OMX_IndexConfigAudioChannelSwitchBoard, &switchboard_config));
    } else {
        /* Initialize the Mixer for App and Voice chains */
        const char *mixer_type;
        if (is_hdmi) {
            mixer_type = adaptions_comp_name(ADM_DEFAULT_APP_MIXER_NAME); //Must use multi channel mixer on ARM
        } else {
            mixer_type = adaptions_comp_name(ADM_DEFAULT_COMMON_MIXER_NAME);
        }
        RETURN_IF_FAIL(il_tool_create_component(
                &graph_p->common.mixer_splitter_handle,
                mixer_type, "common"));

        ALOG_INFO("il_tool_create_component returned 0x%08X for %s (Common Mixer)\n",
            (unsigned int) il_tool_raw_handle(graph_p->common.mixer_splitter_handle),
            mixer_type);
    }

    return STE_ADM_RES_OK;
}

/**
 * Allocate memory for the app/vc graph
 */
ste_adm_res_t adm_omx_graph_init(dev_omx_graph_t* graph_p, int is_vc)
{
    if (!is_vc)  {
        if (!graph_p->is_input) {
            /* Initialize the App Mixer */
            RETURN_IF_FAIL(il_tool_create_component(
                    &(graph_p->app.mixer_splitter_handle),
                    adaptions_comp_name(ADM_DEFAULT_APP_MIXER_NAME), "app"));

            ALOG_INFO("il_tool_create_component returned 0x%08X for %s (App Mixer)\n",
                (unsigned int) il_tool_raw_handle(graph_p->app.mixer_splitter_handle),
                adaptions_comp_name(ADM_DEFAULT_APP_MIXER_NAME));
        } else {
            /* Initialize the App Splitter */
            RETURN_IF_FAIL(il_tool_create_component(
                    &graph_p->app.mixer_splitter_handle,
                    adaptions_comp_name(ADM_DEFAULT_SPLITTER_NAME), "app"));

            ALOG_INFO("il_tool_create_component returned 0x%08X for %s (App Splitter)\n",
                (unsigned int) il_tool_raw_handle(graph_p->app.mixer_splitter_handle),
                adaptions_comp_name(ADM_DEFAULT_SPLITTER_NAME));
        }
    }

    return STE_ADM_RES_OK;
}

ste_adm_res_t dev_omx_graph_init_pcm_mode_common_mixer_splitter(
                            dev_omx_graph_t*        graph_p,
                            const char*             name,
                            il_comp_t               mixer_splitter_comp,
                            unsigned int           port_index,
                            int                     samplerate,
                            unsigned int            channels)
{
    (void) name;
    int                         result          = 0;
    il_comp_t                   io_comp;
    OMX_AUDIO_PARAM_PCMMODETYPE param_pcm;
    IL_TOOL_INIT_CONFIG_STRUCT(param_pcm);

    ALOG_INFO("Enter dev_omx_graph_init_pcm_mode_common_mixer_splitter - "
        "Component: 0x%08X, Port Index: %u\n",
        (unsigned int)il_tool_raw_handle(mixer_splitter_comp), (unsigned int)port_index);

    // Get IO component
    io_comp = graph_p->common.io_handle;

    // Get the PCM Mode parameters from IO component which is used as a
    //   reference to make sure that the base of PCM params are the same.
    param_pcm.nPortIndex = 0;
    RETURN_IF_FAIL(il_tool_GetParameter(io_comp, OMX_IndexParamAudioPcm, &param_pcm));

    if (!graph_p->is_input) {
        switch (port_index) {
        case MIXER_PORT_VOICE_INPUT:
            param_pcm.nChannels      = (OMX_U32) channels;
            param_pcm.nSamplingRate  = (OMX_U32) samplerate;
            if (param_pcm.nChannels == 1) {
                param_pcm.eChannelMapping[0] = OMX_AUDIO_ChannelCF;
            } else if (param_pcm.nChannels == 2) {
                param_pcm.eChannelMapping[0] = OMX_AUDIO_ChannelLF;
                param_pcm.eChannelMapping[1] = OMX_AUDIO_ChannelRF;
            } else if (param_pcm.nChannels == 6) {
                /* HDMI only has an out device setting this as dummy*/
                param_pcm.eChannelMapping[0] = OMX_AUDIO_ChannelLF;
                param_pcm.eChannelMapping[1] = OMX_AUDIO_ChannelRF;
                param_pcm.eChannelMapping[2] = OMX_AUDIO_ChannelLFE;
                param_pcm.eChannelMapping[3] = OMX_AUDIO_ChannelCF;
                param_pcm.eChannelMapping[4] = OMX_AUDIO_ChannelLS;
                param_pcm.eChannelMapping[5] = OMX_AUDIO_ChannelRS;
            } else {
                ALOG_ERR("%d channels not supported\n", channels);
                return STE_ADM_RES_INVALID_CHANNEL_CONFIG;
            }
            break;
        case MIXER_PORT_OUTPUT:
        case MIXER_PORT_APP_INPUT:
            break;

        default: ADM_ASSERT(0);
        }
    } else {
        switch (port_index) {
        case SPLITTER_PORT_VOICE_OUTPUT:
            // Use nChannels from IO comp
            // param_pcm.nChannels = (OMX_U32) channels;
            param_pcm.nSamplingRate  = (OMX_U32) samplerate;
            if (param_pcm.nChannels == 1) {
                param_pcm.eChannelMapping[0] = OMX_AUDIO_ChannelCF;
            } else {
                ADM_ASSERT(param_pcm.nChannels == 2);
                param_pcm.eChannelMapping[0] = OMX_AUDIO_ChannelLF;
                param_pcm.eChannelMapping[1] = OMX_AUDIO_ChannelRF;
            }
            break;
        case SPLITTER_PORT_APP_OUTPUT:
        case SPLITTER_PORT_INPUT:
        default:
            /* Same as IO component */
            break;
        }
    }


    ALOG_INFO("TODO: Mixer/splitter settings not in DB anymore, using settings on IO component for now\n");

    /* Update PCM Mode parameters in the component */
    param_pcm.nPortIndex = port_index;
    ALOG_INFO("\tSet PCM Mode params on port %u for Common Mixer/Splitter "
        "0x%08X\n", param_pcm.nPortIndex, (unsigned int)il_tool_raw_handle(mixer_splitter_comp));
    RETURN_IF_FAIL(il_tool_SetParameter(
        mixer_splitter_comp, OMX_IndexParamAudioPcm, &param_pcm));

#ifdef ADM_MMPROBE
    /* Configure PCM probe for component */
    int probe_id;
    int probe_enabled;
    if (dev_get_pcm_probe(name, PCM_PROBE_COMMON_MIX_SPLIT, port_index, &probe_id, &probe_enabled) == STE_ADM_RES_OK) {
        AFM_AUDIO_CONFIG_PCM_PROBE pcm_probe_cfg;
        IL_TOOL_INIT_CONFIG_STRUCT(pcm_probe_cfg);
        pcm_probe_cfg.nPortIndex = port_index;
        pcm_probe_cfg.nProbeIdx = probe_id;
        pcm_probe_cfg.bEnable = probe_enabled;
        if (il_tool_SetConfig(mixer_splitter_comp, AFM_IndexConfigPcmProbe, &pcm_probe_cfg) != STE_ADM_RES_OK) {
            ALOG_WARN("AFM_IndexConfigPcmProbe not supported by comp=%p (%s, %s, port_index=%d)", 
                il_tool_raw_handle(mixer_splitter_comp), name, probe_comp2str(PCM_PROBE_COMMON_MIX_SPLIT), port_index);
        }
    } else {
        ALOG_ERR("dev_get_pcm_probe failed for %s (%s, port_index=%d)", name, probe_comp2str(PCM_PROBE_COMMON_MIX_SPLIT), port_index);
    }
#endif // ADM_MMPROBE

    ALOG_INFO("Leave dev_omx_graph_init_pcm_mode_common_mixer_splitter\n");
    return result;
}

ste_adm_res_t dev_omx_graph_init_pcm_mode_app_mixer_splitter(
                            dev_omx_graph_t*    graph_p,
                            const char*             name,
                            il_comp_t               mixer_splitter_comp,
                            unsigned int           port_index,
                            int                     samplerate,
                            unsigned int            channels)
{
    (void) name;
    il_comp_t                   io_comp;
    OMX_AUDIO_PARAM_PCMMODETYPE param_pcm;
    unsigned int nIoCompChannels;

    IL_TOOL_INIT_CONFIG_STRUCT(param_pcm);

    ADM_ASSERT(graph_p != NULL);

    ALOG_INFO("Enter dev_omx_graph_init_pcm_mode_app_mixer_splitter - "
        "Component: 0x%08X, Port Index: %u\n",
        (unsigned int)il_tool_raw_handle(mixer_splitter_comp),
        (unsigned int)port_index);

    // Get IO component
    io_comp = graph_p->common.io_handle;

    // Get the PCM Mode parameters from IO component which is used as a
    //   reference to make sure that the base of PCM params are the same.
    param_pcm.nPortIndex = 0;
    RETURN_IF_FAIL(il_tool_GetParameter(io_comp, OMX_IndexParamAudioPcm, &param_pcm));

    nIoCompChannels = param_pcm.nChannels;

    if (graph_p->is_input) {
        switch (port_index) {
        case SPLITTER_PORT_INPUT:
            /* Same as IO component */
            break;
        default:
            param_pcm.nChannels      = (OMX_U32) channels;
            param_pcm.nSamplingRate  = (OMX_U32) samplerate;

            if (param_pcm.nChannels == 1) {
                param_pcm.eChannelMapping[0] = OMX_AUDIO_ChannelCF;
            } else {
                ADM_ASSERT(param_pcm.nChannels == 2);
                param_pcm.eChannelMapping[0] = OMX_AUDIO_ChannelLF;
                param_pcm.eChannelMapping[1] = OMX_AUDIO_ChannelRF;
            }
            break;
        }
    }
    if (!graph_p->is_input) {
        switch (port_index) {
        case MIXER_PORT_OUTPUT:
            /* Same as IO component or special for HDMI */
            if (param_pcm.nChannels == 6) {
                param_pcm.eChannelMapping[0] = OMX_AUDIO_ChannelLF;
                param_pcm.eChannelMapping[1] = OMX_AUDIO_ChannelRF;
                param_pcm.eChannelMapping[2] = OMX_AUDIO_ChannelLFE;
                param_pcm.eChannelMapping[3] = OMX_AUDIO_ChannelCF;
                param_pcm.eChannelMapping[4] = OMX_AUDIO_ChannelLS;
                param_pcm.eChannelMapping[5] = OMX_AUDIO_ChannelRS;
            }

#ifdef ADM_SRC_MODE
            outsamplerate = (OMX_U32) param_pcm.nSamplingRate;
#endif
            break;
        default:
            param_pcm.nChannels      = (OMX_U32) channels;
            param_pcm.nSamplingRate  = (OMX_U32) samplerate;

            if (param_pcm.nChannels == 1) {
                param_pcm.eChannelMapping[0] = OMX_AUDIO_ChannelCF;
            } else if (param_pcm.nChannels == 2) {
                param_pcm.eChannelMapping[0] = OMX_AUDIO_ChannelLF;
                param_pcm.eChannelMapping[1] = OMX_AUDIO_ChannelRF;
            } else if (param_pcm.nChannels == 6) {
                param_pcm.eChannelMapping[0] = OMX_AUDIO_ChannelLF;
                param_pcm.eChannelMapping[1] = OMX_AUDIO_ChannelRF;
                param_pcm.eChannelMapping[2] = OMX_AUDIO_ChannelLFE;
                param_pcm.eChannelMapping[3] = OMX_AUDIO_ChannelCF;
                param_pcm.eChannelMapping[4] = OMX_AUDIO_ChannelLS;
                param_pcm.eChannelMapping[5] = OMX_AUDIO_ChannelRS;
            }
            break;
        }
    }

    ALOG_INFO("TODO: Mixer/splitter settings not in DB anymore, using settings on IO component for now\n");

    /* Update PCM Mode parameters in the component */
    param_pcm.nPortIndex = port_index;
    ALOG_INFO("\tSet PCM Mode params on port %u for App Mixer/Splitter "
        "0x%08X\n", param_pcm.nPortIndex, (unsigned int)il_tool_raw_handle(mixer_splitter_comp));

    RETURN_IF_FAIL(il_tool_SetParameter(
        mixer_splitter_comp, OMX_IndexParamAudioPcm, &param_pcm));

#ifdef ADM_MMPROBE
    /* Configure PCM probe for component */
    int probe_id;
    int probe_enabled;
    if (dev_get_pcm_probe(name, PCM_PROBE_APP_MIX_SPLIT, port_index, &probe_id, &probe_enabled) == STE_ADM_RES_OK) {
        AFM_AUDIO_CONFIG_PCM_PROBE pcm_probe_cfg;
        IL_TOOL_INIT_CONFIG_STRUCT(pcm_probe_cfg);
        pcm_probe_cfg.nPortIndex = port_index;
        pcm_probe_cfg.nProbeIdx = probe_id;
        pcm_probe_cfg.bEnable = probe_enabled;
        if (il_tool_SetConfig(mixer_splitter_comp, AFM_IndexConfigPcmProbe, &pcm_probe_cfg) != STE_ADM_RES_OK) {
            ALOG_WARN("AFM_IndexConfigPcmProbe not supported by comp=%p (%s, %s, port_index=%d)", 
                il_tool_raw_handle(mixer_splitter_comp), name, probe_comp2str(PCM_PROBE_APP_MIX_SPLIT), port_index);
        }
    } else {
        ALOG_ERR("dev_get_pcm_probe failed for %s (%s, port_index=%d)", name, probe_comp2str(PCM_PROBE_APP_MIX_SPLIT), port_index);
    }
#endif // ADM_MMPROBE
        
#ifdef ADM_SRC_MODE /* Sample rate converter mode change 44100 -> 48000 */

    if (!graph_p->is_input)
    {
        if (!adaptions_internal_mixsplit() && (param_pcm.nPortIndex != 0))
        {
            if(samplerate == 44100 && outsamplerate == 48000)
            {
                AFM_AUDIO_PARAM_SRC_MODE src_mode;

                IL_TOOL_INIT_CONFIG_STRUCT(src_mode);
                src_mode.nPortIndex = port_index;
                RETURN_IF_FAIL(il_tool_GetParameter(
                    mixer_splitter_comp, AFM_IndexParamSrcMode, &src_mode));

                src_mode.nPortIndex = port_index;
                src_mode.nSrcMode   = AFM_SRC_MODE_CUSTOM1;
                RETURN_IF_FAIL(il_tool_SetParameter(
                    mixer_splitter_comp, AFM_IndexParamSrcMode, &src_mode));

                ALOG_INFO_VERBOSE("\tSet SRC Mode on port %u for App Mixer/Splitter "
                    "0x%08X : input sample rate %d  output sample rate %d\n",
                    param_pcm.nPortIndex, (unsigned int)il_tool_raw_handle(mixer_splitter_comp), samplerate, outsamplerate);

            }
        }
    }
#endif

// Configure splitter switch board
    if (graph_p->is_input && port_index != SPLITTER_PORT_INPUT) {
        if (channels != 1 && channels != 2) {
            ALOG_ERR("Splitter only supports mono and stereo\n");
            return STE_ADM_RES_INVALID_PARAMETER;
        }

        OMX_SYMBIAN_AUDIO_CONFIG_CHANNELSWITCHBOARDTYPE switchboard_config;
        IL_TOOL_INIT_CONFIG_STRUCT(switchboard_config);
        switchboard_config.nPortIndex = port_index;
        switchboard_config.nOutputChannel[0] = 0;
        if (nIoCompChannels == 2) {
            switchboard_config.nOutputChannel[1] = 1;
        }
        else {
            switchboard_config.nOutputChannel[1] = 0;
        }
        RETURN_IF_FAIL(il_tool_SetConfig(mixer_splitter_comp,
                OMX_IndexConfigAudioChannelSwitchBoard, &switchboard_config));
    }

    ALOG_INFO("Leave dev_omx_graph_init_pcm_mode_app_mixer_splitter\n");
    return STE_ADM_RES_OK;
}

ste_adm_res_t adm_omx_graph_get_port_index(
                    dev_omx_graph_t*        graph_p,
                    int                     is_vc,
                    OMX_U32                *port_index_p)
{
    unsigned int                max_nbr_of_ports = ADM_OMX_MAX_NBR_OF_PORTS;
    OMX_PORT_PARAM_TYPE         param_port;

    il_comp_t                   app_mixer_splitter_comp;
    dev_omx_comp_chain_t    comp_chains;
    unsigned int                i;

    IL_TOOL_INIT_CONFIG_STRUCT(param_port);

    ALOG_STATUS("Enter adm_omx_graph_get_port_index\n");

    /* Get the App/Voice graph and Common graph OMX components */
    dev_omx_util_get_components(graph_p, is_vc, &comp_chains);

    if (!is_vc) {
        app_mixer_splitter_comp = graph_p->app.mixer_splitter_handle;

        /* Get number of ports in the mixer/splitter */
        RETURN_IF_FAIL(il_tool_GetParameter(app_mixer_splitter_comp,
            OMX_IndexParamAudioInit, &param_port));

        if (param_port.nPorts < ADM_OMX_MAX_NBR_OF_PORTS) {
            /* Subtract 1 since one port is input to splitter/output from mixer */
            max_nbr_of_ports = param_port.nPorts - 1;
        }

        /* Find next available port index */
        for (i = 0; i < max_nbr_of_ports; i++) {
        	ALOG_STATUS("graph_p->is_input:%d\n",graph_p->is_input);
        	ALOG_STATUS("graph_p->app.port_index[%d]:%d\n",i,graph_p->app.port_index[i]);
            if (graph_p->app.port_index[i] == 0) {
                /* Available port index found */
                graph_p->app.port_index[i] = 1;
                /* Add 1 to returned port index, since App Mixer input ports
                   start at 1 and App Splitter output ports start at 1 */
                *port_index_p = (unsigned int) i + 1;
                break;
            }
        }
        if (i == max_nbr_of_ports) {
            /* Error - No available port indexes */
            ALOG_ERR("No available port indexes in App Mixer/Splitter (total is %d)\n", max_nbr_of_ports);
            return STE_ADM_RES_ALL_IN_USE;
        }
    } else {
        if (graph_p->is_input) {
            if (comp_chains.nbr_of_comp > 0) {
                *port_index_p = EFFECT_PORT_OUTPUT;
            } else {
                *port_index_p = SPLITTER_PORT_VOICE_OUTPUT;
            }
        } else {
            if (comp_chains.nbr_of_comp > 0) {
                *port_index_p = EFFECT_PORT_INPUT;
            } else {
                *port_index_p = MIXER_PORT_VOICE_INPUT;
            }
        }
    }

    return STE_ADM_RES_OK;
}

int dev_omx_graph_connect_effects(
                            unsigned int                nbr_of_comp,
                            il_comp_t                   *comp_handles_p)
{
    int result = 0;
    unsigned int k;
    ADM_ASSERT(comp_handles_p != NULL);

    ALOG_INFO("Enter dev_omx_graph_connect_effects\n");

    if (nbr_of_comp > 1) {
        for(k = 0 ; k < (nbr_of_comp - 1); k++) {

            result = il_tool_SetupTunnel(comp_handles_p[k], EFFECT_PORT_OUTPUT,
                                     comp_handles_p[k+1], EFFECT_PORT_INPUT);
            STE_ADM_OMX_CHECK_RESULT(result);

            ALOG_INFO("\tSuccessfully connected component "
                "0x%08X port %u to 0x%08X port %u\n",
                (unsigned int)il_tool_raw_handle(comp_handles_p[k]), EFFECT_PORT_OUTPUT,
                (unsigned int)il_tool_raw_handle(comp_handles_p[k+1]), EFFECT_PORT_INPUT);
        }
    }


cleanup:
    return result;
}

/*
 * Connects all effects and mixer/splitter in app graph and disables all endpoint ports.
 */
static ste_adm_res_t adm_omx_graph_connect_app_graph(
                            dev_omx_graph_t* graph_p)
{
    il_comp_t                app_splitter_comp;
    il_comp_t                app_mixer_comp;

    ALOG_INFO("Enter adm_omx_graph_connect_app_graph\n");

    /* Connect the effects in the App comp chain */
    RETURN_IF_FAIL(dev_omx_graph_connect_effects(
                   graph_p->app.nbr_of_comp,
                   graph_p->app.comp_handles));


    if (graph_p->is_input) {
        app_splitter_comp    = graph_p->app.mixer_splitter_handle;

        if (graph_p->app.nbr_of_comp > 0) {
            RETURN_IF_FAIL(il_tool_connect_and_enable(graph_p->app.comp_handles[graph_p->app.nbr_of_comp - 1],
                                               EFFECT_PORT_OUTPUT,
                                               app_splitter_comp,
                                               SPLITTER_PORT_INPUT,
                                               0, 1, 0, OMX_BufferSupplyUnspecified));
            RETURN_IF_FAIL(il_tool_change_port_state(graph_p->app.comp_handles[0], EFFECT_PORT_INPUT, 0, 0));
        }
        else {
            RETURN_IF_FAIL(il_tool_change_port_state(app_splitter_comp, SPLITTER_PORT_INPUT, 0, 0));
        }
    } else {
        app_mixer_comp    = graph_p->app.mixer_splitter_handle;

        if (graph_p->app.nbr_of_comp > 0) {
            RETURN_IF_FAIL(il_tool_SetupTunnel(app_mixer_comp, MIXER_PORT_OUTPUT, graph_p->app.comp_handles[0], EFFECT_PORT_INPUT));
            RETURN_IF_FAIL(il_tool_change_port_state(graph_p->app.comp_handles[graph_p->app.nbr_of_comp - 1], EFFECT_PORT_OUTPUT, 0, 0));
        }
        else {
            RETURN_IF_FAIL(il_tool_change_port_state(app_mixer_comp, MIXER_PORT_OUTPUT, 0, 0));
        }
    }


    return STE_ADM_RES_OK;
}

/*
 * Connects all effects in voice graph and disables all endpoint ports.
 */
static ste_adm_res_t adm_omx_graph_connect_voice_graph(
                            dev_omx_graph_t* graph_p)
{
    ALOG_INFO("Enter adm_omx_graph_connect_voice_graph\n");
    (void) graph_p;

    /* Connect the effects in the Voice comp chain */
    RETURN_IF_FAIL(dev_omx_graph_connect_effects(
                   graph_p->voice.nbr_of_comp,
                   graph_p->voice.comp_handles));

    if (graph_p->voice.nbr_of_comp == 0) {
        /* No effects in Voice Effect Chain */
        ALOG_INFO("\tNo effects in Voice Effect Chain.\n");
        return 0;
    }

    /* Make sure that the end points of the VC effect chain are disabled */
    RETURN_IF_FAIL(il_tool_change_port_state(
        graph_p->voice.comp_handles[0], EFFECT_PORT_INPUT, 0, 0));

    RETURN_IF_FAIL(il_tool_change_port_state(
        graph_p->voice.comp_handles[graph_p->voice.nbr_of_comp-1], EFFECT_PORT_OUTPUT, 0, 0));

    return STE_ADM_RES_OK;
}

/*
 * Connects all components in app or voice graph and disables all endpoint ports.
 */
ste_adm_res_t dev_omx_graph_connect_app_voice_graph(
                            dev_omx_graph_t*        graph_p,
                            int                         is_vc)
{
    ADM_ASSERT(graph_p != NULL);


    ALOG_INFO_VERBOSE("Enter dev_omx_graph_connect_app_voice_graph\n");

    if (!is_vc) {
        RETURN_IF_FAIL(adm_omx_graph_connect_app_graph(graph_p));
    } else {
        RETURN_IF_FAIL(adm_omx_graph_connect_voice_graph(graph_p));
    }

    return STE_ADM_RES_OK;
}

/*
 * Connect to common mixer/splitter
 */
ste_adm_res_t dev_omx_graph_connect_mixer_splitter_to_app_voice(
                            dev_omx_graph_t*    graph_p,
                            int                 is_vc)
{
    ADM_ASSERT(graph_p != NULL);

    ALOG_INFO_VERBOSE("Enter dev_omx_graph_connect_mixer_splitter_to_app_voice\n");

    if (!is_vc) {
        return graph_connect_app_effects_to_common_mix_split(graph_p);
    } else {
        return graph_connect_voice_effects_to_mix_split(graph_p);
    }
}

ste_adm_res_t dev_omx_graph_disconnect_mixer_splitter_from_app_voice(
                            dev_omx_graph_t*        graph_p,
                            int                         is_vc)
{
    ADM_ASSERT(graph_p != NULL);

    ALOG_INFO_VERBOSE("Enter dev_omx_graph_disconnect_mixer_splitter_from_app_voice\n");

    if (!is_vc) {
        return graph_disconnect_app_effects_from_common_mix_split(graph_p);
    } else {
        return graph_disconnect_voice_effects_from_mix_split(graph_p);
    }
}

ste_adm_res_t dev_omx_graph_connect_io_to_common(
                            dev_omx_graph_t*        graph_p,
                            unsigned int                nbr_of_comp,
                            il_comp_t                   *comp_handles_p)
{
    il_comp_t       from_comp;
    il_comp_t       to_comp;
    OMX_U32         from_index;
    OMX_U32         to_index;
    il_comp_t       io_comp;
    il_comp_t       splitter_comp;
    il_comp_t       mixer_comp;

    ADM_ASSERT(graph_p != NULL);
    ADM_ASSERT(comp_handles_p != NULL);

    ALOG_INFO_VERBOSE("Enter dev_omx_graph_connect_io_to_common\n");

    /* Get graph io_comp, i.e. the source or sink */
    io_comp = graph_p->common.io_handle;

    if (graph_p->is_input) {
        /* Connect the Source to the Common Effect Chain */

        from_comp                   = io_comp;
        from_index                  = SOURCE_PORT_OUTPUT;
        if (nbr_of_comp > 0) {
            to_comp                 = comp_handles_p[0];
            to_index                = EFFECT_PORT_INPUT;
        } else {
            /* No Common effects, connect directly to Common Splitter */
            splitter_comp = graph_p->common.mixer_splitter_handle;

            to_comp                 = splitter_comp;
            to_index                = SPLITTER_PORT_INPUT;
        }
    } else {
        if (nbr_of_comp > 0) {
            from_comp               = comp_handles_p[nbr_of_comp-1];
            from_index              = EFFECT_PORT_OUTPUT;
        } else {
            /* No Common effects, connect directly from Common Mixer */
            mixer_comp = graph_p->common.mixer_splitter_handle;

            from_comp               = mixer_comp;
            from_index              = MIXER_PORT_OUTPUT;
        }
        to_comp                     = io_comp;
        to_index                    = SINK_PORT_INPUT;
    }

    RETURN_IF_FAIL(il_tool_SetupTunnel(from_comp, from_index, to_comp, to_index));

    ALOG_INFO("\tSuccessfully connected component "
        "0x%08X port %u to 0x%08X port %u\n",
        il_tool_raw_handle(from_comp), from_index,
        il_tool_raw_handle(to_comp), to_index);

    return STE_ADM_RES_OK;
}

ste_adm_res_t dev_omx_graph_connect_mixer_splitter_to_common(
                            dev_omx_graph_t*        graph_p,
                            unsigned int                nbr_of_comp,
                            il_comp_t                   *comp_handles_p)
{
    il_comp_t       from_comp;
    il_comp_t       to_comp;
    OMX_U32         from_index;
    OMX_U32         to_index;
    il_comp_t       splitter_comp;
    il_comp_t       mixer_comp;

    ADM_ASSERT(graph_p != NULL);
    ADM_ASSERT(comp_handles_p != NULL);

    ALOG_INFO("dev_omx_graph_connect_mixer_splitter_to_common\n");

    if (nbr_of_comp > 0) {

        if (graph_p->is_input) {
            /* Connect the Common Effect Chain to the Common Splitter */
            splitter_comp = graph_p->common.mixer_splitter_handle;

            from_comp                   = comp_handles_p[nbr_of_comp-1];
            from_index                  = EFFECT_PORT_OUTPUT;

            to_comp                     = splitter_comp;
            to_index                    = SPLITTER_PORT_INPUT;
        } else {
            mixer_comp = graph_p->common.mixer_splitter_handle;

            from_comp                   = mixer_comp;
            from_index                  = MIXER_PORT_OUTPUT;

            to_comp                     = comp_handles_p[0];
            to_index                    = EFFECT_PORT_INPUT;
        }

        RETURN_IF_FAIL(il_tool_SetupTunnel(from_comp, from_index, to_comp, to_index));
        ALOG_INFO("Successfully connected component "
            "0x%08X port %u to 0x%08X port %u\n",
            il_tool_raw_handle(from_comp), from_index,
            il_tool_raw_handle(to_comp), to_index);
    } else {
        ALOG_INFO("No effects in common effect chain. Nothing to connect, "
            "since IO comp is already connected to App/Voice Mixer/Splitter\n");
    }

    return STE_ADM_RES_OK;
}


ste_adm_res_t adm_omx_graph_connect_pre_effect_to_app(dev_omx_graph_t*        graph_p,
                                            OMX_U32                     port_index)
{
    // NOTE: In case of error during execution of an IL_TOOL_ macro,
    // it will set the 'result' variable below to the resulting error
    // code and then goto cleanup.
    ste_adm_res_t res = STE_ADM_RES_OK;

    il_comp_t       app_comp;
    il_comp_t       pre_effect_comp;
    OMX_U32         app_port = (OMX_U32)port_index;
    OMX_U32         pre_effect_port = EFFECT_PORT_OUTPUT;

    ADM_ASSERT(graph_p != NULL);

    ALOG_INFO("Enter adm_omx_graph_connect_pre_effect_to_app\n");

    /* Get the pre effect */
    ADM_ASSERT((port_index-1) < ADM_OMX_MAX_NBR_OF_PORTS);
    pre_effect_comp = graph_p->app.pre_effect_handle[port_index-1];

    /* Get the application mixer */
    app_comp = graph_p->app.mixer_splitter_handle;

    /* Connect the pre effect to the application mixer */
    IL_TOOL_CONNECT_DST_CFG(
            pre_effect_comp, pre_effect_port,
            app_comp,        app_port);

    ALOG_INFO("Successfully connected component "
        "0x%08X port %u to 0x%08X port %u\n",
        il_tool_raw_handle(pre_effect_comp), pre_effect_port,
        il_tool_raw_handle(app_comp), app_port);

cleanup:
    return res;
}

/*
 * Connect common effect chain to the common mixer/splitter.
 * If no effects, the source/sink is connected to common mixer/splitter.
 */
ste_adm_res_t graph_connect_common_effects_to_mix_split(dev_omx_graph_t* graph_p)
{
    ste_adm_res_t res = STE_ADM_RES_OK;
    il_comp_t src_comp;
    OMX_U32 src_port;
    il_comp_t dst_comp;
    OMX_U32 dst_port;

    il_comp_t *comp_handles_p = graph_p->common.comp_handles;
    unsigned int nbr_of_comp = graph_p->common.nbr_of_comp;

    ALOG_INFO("Enter graph_connect_common_effects_to_mix_split\n");

    if (nbr_of_comp > 0) {
        if (graph_p->is_input) {
            src_comp = comp_handles_p[nbr_of_comp-1];
            src_port = EFFECT_PORT_OUTPUT;
            dst_comp = graph_p->common.mixer_splitter_handle;
            dst_port = SPLITTER_PORT_INPUT;
        } else {
            src_comp = graph_p->common.mixer_splitter_handle;
            src_port = MIXER_PORT_OUTPUT;
            dst_comp = comp_handles_p[0];
            dst_port = EFFECT_PORT_INPUT;
        }
    } else {
        if (graph_p->is_input) {
            src_comp = graph_p->common.io_handle;
            src_port = SOURCE_PORT_OUTPUT;
            dst_comp = graph_p->common.mixer_splitter_handle;
            dst_port = SPLITTER_PORT_INPUT;
        } else {
            src_comp = graph_p->common.mixer_splitter_handle;
            src_port = MIXER_PORT_OUTPUT;
            dst_comp = graph_p->common.io_handle;
            dst_port = SINK_PORT_INPUT;
        }
    }

    IL_TOOL_CONNECT_AND_ENABLE_WITH_SRC_CFG(src_comp, src_port, dst_comp, dst_port);

cleanup:
    return res;
}

/*
 * Disconnect common effect chain from the common mixer/splitter.
 * If no effects, the source/sink is disconnected from the common mixer/splitter.
 */
ste_adm_res_t graph_disconnect_common_effects_from_mix_split(dev_omx_graph_t* graph_p)
{
    ste_adm_res_t res = STE_ADM_RES_OK;
    il_comp_t src_comp;
    OMX_U32 src_port;
    il_comp_t dst_comp;
    OMX_U32 dst_port;

    il_comp_t *comp_handles_p = graph_p->common.comp_handles;
    unsigned int nbr_of_comp = graph_p->common.nbr_of_comp;

    ALOG_INFO("Enter graph_disconnect_common_effects_from_mix_split\n");

    if (nbr_of_comp > 0) {
        if (graph_p->is_input) {
            src_comp = comp_handles_p[nbr_of_comp-1];
            src_port = EFFECT_PORT_OUTPUT;
            dst_comp = graph_p->common.mixer_splitter_handle;
            dst_port = SPLITTER_PORT_INPUT;
        } else {
            src_comp = graph_p->common.mixer_splitter_handle;
            src_port = MIXER_PORT_OUTPUT;
            dst_comp = comp_handles_p[0];
            dst_port = EFFECT_PORT_INPUT;
        }
    } else {
        if (graph_p->is_input) {
            src_comp = graph_p->common.io_handle;
            src_port = SOURCE_PORT_OUTPUT;
            dst_comp = graph_p->common.mixer_splitter_handle;
            dst_port = SPLITTER_PORT_INPUT;
        } else {
            src_comp = graph_p->common.mixer_splitter_handle;
            src_port = MIXER_PORT_OUTPUT;
            dst_comp = graph_p->common.io_handle;
            dst_port = SINK_PORT_INPUT;
        }
    }

    IL_TOOL_DISCONNECT(src_comp, src_port, dst_comp, dst_port);

cleanup:
    return res;
}

/*
 * Connect common effect chain to the source/sink.
 * If no effects, the common mixer/splitter is connected to the source/sink.
 */
ste_adm_res_t graph_connect_common_effects_to_io(dev_omx_graph_t* graph_p)
{
    ste_adm_res_t res = STE_ADM_RES_OK;
    il_comp_t src_comp;
    OMX_U32 src_port;
    il_comp_t dst_comp;
    OMX_U32 dst_port;

    il_comp_t *comp_handles_p = graph_p->common.comp_handles;
    unsigned int nbr_of_comp = graph_p->common.nbr_of_comp;

    ALOG_INFO("Enter graph_connect_common_effects_to_io\n");

    if (nbr_of_comp > 0) {
        if (graph_p->is_input) {
            src_comp = graph_p->common.io_handle;
            src_port = SOURCE_PORT_OUTPUT;
            dst_comp = comp_handles_p[0];
            dst_port = EFFECT_PORT_INPUT;
        } else {
            src_comp = comp_handles_p[nbr_of_comp-1];
            src_port = EFFECT_PORT_OUTPUT;
            dst_comp = graph_p->common.io_handle;
            dst_port = SINK_PORT_INPUT;
        }
    } else {
        if (graph_p->is_input) {
            src_comp = graph_p->common.io_handle;
            src_port = SOURCE_PORT_OUTPUT;
            dst_comp = graph_p->common.mixer_splitter_handle;
            dst_port = SPLITTER_INPUT_PORT;
        } else {
            src_comp = graph_p->common.mixer_splitter_handle;
            src_port = MIXER_OUTPUT_PORT;
            dst_comp = graph_p->common.io_handle;
            dst_port = SINK_PORT_INPUT;
        }
    }

    IL_TOOL_CONNECT_AND_ENABLE_WITH_SRC_CFG(src_comp, src_port, dst_comp, dst_port);

cleanup:
    return res;
}

/*
 * Disconnect common effect chain from the source/sink.
 * If no effects, the source/sink is disconnected from the common mixer/splitter
 */
ste_adm_res_t graph_disconnect_common_effects_from_io(dev_omx_graph_t* graph_p)
{
    ste_adm_res_t res = STE_ADM_RES_OK;
    il_comp_t src_comp;
    OMX_U32 src_port;
    il_comp_t dst_comp;
    OMX_U32 dst_port;

    il_comp_t *comp_handles_p = graph_p->common.comp_handles;
    unsigned int nbr_of_comp = graph_p->common.nbr_of_comp;

    ALOG_INFO("Enter graph_disconnect_common_effects_from_io\n");

    if (nbr_of_comp > 0) {
        if (graph_p->is_input) {
            src_comp = graph_p->common.io_handle;
            src_port = SOURCE_PORT_OUTPUT;
            dst_comp = comp_handles_p[0];
            dst_port = EFFECT_PORT_INPUT;
        } else {
            src_comp = comp_handles_p[nbr_of_comp-1];
            src_port = EFFECT_PORT_OUTPUT;
            dst_comp = graph_p->common.io_handle;
            dst_port = SINK_PORT_INPUT;
        }
    } else {
        if (graph_p->is_input) {
            src_comp = graph_p->common.io_handle;
            src_port = SOURCE_PORT_OUTPUT;
            dst_comp = graph_p->common.mixer_splitter_handle;
            dst_port = SPLITTER_INPUT_PORT;
        } else {
            src_comp = graph_p->common.mixer_splitter_handle;
            src_port = MIXER_OUTPUT_PORT;
            dst_comp = graph_p->common.io_handle;
            dst_port = SINK_PORT_INPUT;
        }
    }

    IL_TOOL_DISCONNECT(src_comp, src_port, dst_comp, dst_port);

cleanup:
    return res;
}

/*
 * Connect voice effect chain to the common mixer/splitter.
 * If no effects, nothing is connected.
 */
ste_adm_res_t graph_connect_voice_effects_to_mix_split(dev_omx_graph_t* graph_p)
{
    ste_adm_res_t res = STE_ADM_RES_OK;
    il_comp_t src_comp;
    OMX_U32 src_port;
    il_comp_t dst_comp;
    OMX_U32 dst_port;

    ALOG_INFO("Enter graph_connect_voice_effects_to_mix_split\n");

    il_comp_t *comp_handles_p = graph_p->voice.comp_handles;
    unsigned int nbr_of_comp = graph_p->voice.nbr_of_comp;

    if (nbr_of_comp > 0) {
        if (graph_p->is_input) {
            src_comp = graph_p->common.mixer_splitter_handle;
            src_port = SPLITTER_PORT_VOICE_OUTPUT;
            dst_comp = comp_handles_p[0];
            dst_port = EFFECT_PORT_INPUT;
        } else {
            src_comp = comp_handles_p[nbr_of_comp-1];
            src_port = EFFECT_PORT_OUTPUT;
            dst_comp = graph_p->common.mixer_splitter_handle;
            dst_port = MIXER_PORT_VOICE_INPUT;
        }

        IL_TOOL_CONNECT_AND_ENABLE_WITH_SRC_CFG(src_comp, src_port, dst_comp, dst_port);
    }

cleanup:
    return res;
}

/*
 * Disconnect voice effect chain from the common mixer/splitter.
 * If no effects, nothing is disconnected.
 */
ste_adm_res_t graph_disconnect_voice_effects_from_mix_split(dev_omx_graph_t* graph_p)
{
    ste_adm_res_t res = STE_ADM_RES_OK;
    il_comp_t src_comp;
    OMX_U32 src_port;
    il_comp_t dst_comp;
    OMX_U32 dst_port;

    ALOG_INFO("Enter graph_disconnect_voice_effects_from_mix_split\n");

    il_comp_t *comp_handles_p = graph_p->voice.comp_handles;
    unsigned int nbr_of_comp = graph_p->voice.nbr_of_comp;

    if (nbr_of_comp > 0) {
        if (graph_p->is_input) {
            src_comp = graph_p->common.mixer_splitter_handle;
            src_port = SPLITTER_PORT_VOICE_OUTPUT;
            dst_comp = comp_handles_p[0];
            dst_port = EFFECT_PORT_INPUT;
        } else {
            src_comp = comp_handles_p[nbr_of_comp-1];
            src_port = EFFECT_PORT_OUTPUT;
            dst_comp = graph_p->common.mixer_splitter_handle;
            dst_port = MIXER_PORT_VOICE_INPUT;
        }

        IL_TOOL_DISCONNECT(src_comp, src_port, dst_comp, dst_port);
    }

cleanup:
    return res;
}

/*
 * Connect application effect chain to the common mixer/splitter.
 * If no effects, the app mixer/splitter is connected to common mixer/splitter.
 */
ste_adm_res_t graph_connect_app_effects_to_common_mix_split(dev_omx_graph_t* graph_p)
{
    ste_adm_res_t res = STE_ADM_RES_OK;
    il_comp_t src_comp;
    OMX_U32 src_port;
    il_comp_t dst_comp;
    OMX_U32 dst_port;

    ALOG_INFO("Enter graph_connect_app_effects_to_common_mix_split\n");

    il_comp_t *comp_handles_p = graph_p->app.comp_handles;
    unsigned int nbr_of_comp = graph_p->app.nbr_of_comp;

    if (nbr_of_comp > 0) {
        if (graph_p->is_input) {
            src_comp = graph_p->common.mixer_splitter_handle;
            src_port = SPLITTER_PORT_APP_OUTPUT;
            dst_comp = comp_handles_p[0];
            dst_port = EFFECT_PORT_INPUT;
        } else {
            src_comp = comp_handles_p[nbr_of_comp-1];
            src_port = EFFECT_PORT_OUTPUT;
            dst_comp = graph_p->common.mixer_splitter_handle;
            dst_port = MIXER_PORT_APP_INPUT;
        }
    } else {
        if (graph_p->is_input) {
            src_comp = graph_p->common.mixer_splitter_handle;
            src_port = SPLITTER_PORT_APP_OUTPUT;
            dst_comp = graph_p->app.mixer_splitter_handle;
            dst_port = SPLITTER_INPUT_PORT;
        } else {
            src_comp = graph_p->app.mixer_splitter_handle;
            src_port = MIXER_OUTPUT_PORT;
            dst_comp = graph_p->common.mixer_splitter_handle;
            dst_port = MIXER_PORT_APP_INPUT;
        }
    }

    IL_TOOL_CONNECT_AND_ENABLE_WITH_SRC_CFG(src_comp, src_port, dst_comp, dst_port);


cleanup:
    return res;
}

/*
 * Disconnect application effect chain from the common mixer/splitter.
 * If no effects, the app mixer/splitter is disconnected from common mixer/splitter.
 */
ste_adm_res_t graph_disconnect_app_effects_from_common_mix_split(dev_omx_graph_t* graph_p)
{
    ste_adm_res_t res = STE_ADM_RES_OK;
    il_comp_t src_comp;
    OMX_U32 src_port;
    il_comp_t dst_comp;
    OMX_U32 dst_port;

    ALOG_INFO("Enter graph_disconnect_app_effects_from_common_mix_split\n");

    il_comp_t *comp_handles_p = graph_p->app.comp_handles;
    unsigned int nbr_of_comp = graph_p->app.nbr_of_comp;

    if (nbr_of_comp > 0) {
        if (graph_p->is_input) {
            src_comp = graph_p->common.mixer_splitter_handle;
            src_port = SPLITTER_PORT_APP_OUTPUT;
            dst_comp = comp_handles_p[0];
            dst_port = EFFECT_PORT_INPUT;
        } else {
            src_comp = comp_handles_p[nbr_of_comp-1];
            src_port = EFFECT_PORT_OUTPUT;
            dst_comp = graph_p->common.mixer_splitter_handle;
            dst_port = MIXER_PORT_APP_INPUT;
        }
    } else {
        if (graph_p->is_input) {
            src_comp = graph_p->common.mixer_splitter_handle;
            src_port = SPLITTER_PORT_APP_OUTPUT;
            dst_comp = graph_p->app.mixer_splitter_handle;
            dst_port = SPLITTER_INPUT_PORT;
        } else {
            src_comp = graph_p->app.mixer_splitter_handle;
            src_port = MIXER_OUTPUT_PORT;
            dst_comp = graph_p->common.mixer_splitter_handle;
            dst_port = MIXER_PORT_APP_INPUT;
        }
    }

    IL_TOOL_DISCONNECT(src_comp, src_port, dst_comp, dst_port);

cleanup:
    return res;
}

/*
 * Connect application effect chain to the app mixer/splitter.
 * If no effects, the common mixer/splitter is connected to the app mixer/splitter.
 */
ste_adm_res_t graph_connect_app_effects_to_app_mix_split(dev_omx_graph_t* graph_p)
{
    ste_adm_res_t res = STE_ADM_RES_OK;
    il_comp_t src_comp;
    OMX_U32 src_port;
    il_comp_t dst_comp;
    OMX_U32 dst_port;

    ALOG_INFO("Enter graph_connect_app_effects_to_app_mix_split\n");

    il_comp_t *comp_handles_p = graph_p->app.comp_handles;
    unsigned int nbr_of_comp = graph_p->app.nbr_of_comp;

    if (nbr_of_comp > 0) {
        if (graph_p->is_input) {
            src_comp = comp_handles_p[nbr_of_comp-1];
            src_port = EFFECT_PORT_OUTPUT;
            dst_comp = graph_p->app.mixer_splitter_handle;
            dst_port = SPLITTER_PORT_INPUT;
        } else {
            src_comp = graph_p->app.mixer_splitter_handle;
            src_port = MIXER_PORT_OUTPUT;
            dst_comp = comp_handles_p[0];
            dst_port = EFFECT_PORT_INPUT;
        }
    } else {
        if (graph_p->is_input) {
            src_comp = graph_p->common.mixer_splitter_handle;
            src_port = SPLITTER_OUTPUT_PORT;
            dst_comp = graph_p->app.mixer_splitter_handle;
            dst_port = SPLITTER_PORT_INPUT;
        } else {
            src_comp = graph_p->app.mixer_splitter_handle;
            src_port = MIXER_PORT_OUTPUT;
            dst_comp = graph_p->common.mixer_splitter_handle;
            dst_port = MIXER_PORT_APP_INPUT;
        }
    }

    IL_TOOL_CONNECT_AND_ENABLE_WITH_SRC_CFG(src_comp, src_port, dst_comp, dst_port);

cleanup:
    return res;
}

/*
 * Disconnect application effect chain from the app mixer/splitter.
 * If no effects, the common mixer/splitter is disconnected from the app mixer/splitter.
 */
ste_adm_res_t graph_disconnect_app_effects_from_app_mix_split(dev_omx_graph_t* graph_p)
{
    ste_adm_res_t res = STE_ADM_RES_OK;
    il_comp_t src_comp;
    OMX_U32 src_port;
    il_comp_t dst_comp;
    OMX_U32 dst_port;

    ALOG_INFO("Enter graph_disconnect_app_effects_from_app_mix_split\n");

    il_comp_t *comp_handles_p = graph_p->app.comp_handles;
    unsigned int nbr_of_comp = graph_p->app.nbr_of_comp;

    if (nbr_of_comp > 0) {
        if (graph_p->is_input) {
            src_comp = comp_handles_p[nbr_of_comp-1];
            src_port = EFFECT_PORT_OUTPUT;
            dst_comp = graph_p->app.mixer_splitter_handle;
            dst_port = SPLITTER_PORT_INPUT;
        } else {
            src_comp = graph_p->app.mixer_splitter_handle;
            src_port = MIXER_PORT_OUTPUT;
            dst_comp = comp_handles_p[0];
            dst_port = EFFECT_PORT_INPUT;
        }
    } else {
        if (graph_p->is_input) {
            src_comp = graph_p->common.mixer_splitter_handle;
            src_port = SPLITTER_OUTPUT_PORT;
            dst_comp = graph_p->app.mixer_splitter_handle;
            dst_port = SPLITTER_PORT_INPUT;
        } else {
            src_comp = graph_p->app.mixer_splitter_handle;
            src_port = MIXER_PORT_OUTPUT;
            dst_comp = graph_p->common.mixer_splitter_handle;
            dst_port = MIXER_PORT_APP_INPUT;
        }
    }

    IL_TOOL_DISCONNECT(src_comp, src_port, dst_comp, dst_port);

cleanup:
    return res;
}

ste_adm_res_t dev_omx_graph_state_transition_request(
                            il_comp_t                   *comp_handles_p,
                            unsigned int                nbr_of_comp,
                            OMX_STATETYPE               omx_state,
                            unsigned int                transit_forward)
{
    unsigned int i;

    ADM_ASSERT(comp_handles_p != NULL);

    ALOG_INFO_VERBOSE("Enter dev_omx_graph_state_transition_request, "
        "%s, %u components\n",
        dev_omx_stateid2statestring(omx_state), nbr_of_comp);

    for (i = 0; i < nbr_of_comp; i++) {
        if (transit_forward) {
            RETURN_IF_FAIL(il_tool_SendCommand(
                comp_handles_p[i], OMX_CommandStateSet,
                omx_state, 0));
        } else {
            RETURN_IF_FAIL(il_tool_SendCommand(
                comp_handles_p[nbr_of_comp-1-i], OMX_CommandStateSet,
                omx_state, 0));
        }
    }

    return STE_ADM_RES_OK;
}

ste_adm_res_t dev_omx_graph_state_transition_wait(unsigned int  nbr_of_comp)
{
    unsigned int    i;

    ALOG_INFO_VERBOSE("Enter dev_omx_graph_state_transition_wait - "
        "NbrOfCmp=%u\n", nbr_of_comp);

    for (i = 0; i < nbr_of_comp; i++) {
        ALOG_INFO_VERBOSE("Wait for event semaphore Down\n");
        ADM_SEM_WAIT_WITH_PRINT("state_transition_wait %d/%d", i+1, nbr_of_comp);
    }

    return STE_ADM_RES_OK;
}

ste_adm_res_t dev_omx_graph_state_transition_check(
                            il_comp_t                   *comp_handles_p,
                            unsigned int                nbr_of_comp,
                            OMX_STATETYPE               omx_state)
{
    OMX_STATETYPE   current_omx_state   = OMX_StateInvalid;
    unsigned int    i;

    ADM_ASSERT(comp_handles_p != NULL);

    ALOG_INFO_VERBOSE("Enter dev_omx_graph_state_transition_check - %s, "
        "NbrOfCmp=%u\n",
        dev_omx_stateid2statestring(omx_state), nbr_of_comp);


    for (i = 0; i < nbr_of_comp; i++) {
        RETURN_IF_FAIL(il_tool_GetState(comp_handles_p[i], &current_omx_state));
        if (current_omx_state == omx_state) {
            ALOG_INFO("state transition ok, current = %s, requested = %s, component = 0x%08X\n",
                dev_omx_stateid2statestring(current_omx_state),
                dev_omx_stateid2statestring(omx_state),
                (unsigned int) il_tool_raw_handle(comp_handles_p[i]));
        } else {
            ALOG_INFO("state transition NOK, current = %s, requested = %s, component = 0x%08X\n",
                dev_omx_stateid2statestring(current_omx_state),
                dev_omx_stateid2statestring(omx_state),
                (unsigned int) il_tool_raw_handle(comp_handles_p[i]));
        }
    }

    return STE_ADM_RES_OK;
}

