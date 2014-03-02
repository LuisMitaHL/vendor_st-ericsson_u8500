/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*! \file ste_adm_omx.c
    \brief Implementation of OpenMAX functionality.

    @todo Describe
*/

#define LOG_TAG "ADM_OMX"
#define ADM_LOG_FILENAME "dev_omx"
#include "ste_adm_dbg.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <string.h>

#include "OMX_Component.h"
#include "OMX_Types.h"
#include "OMX_Core.h"
#include "sqlite3.h"

#include "AFM_Index.h"
#include "AFM_Types.h"

#include "ste_adm_dev_omx.h"
#include "ste_adm_dev_omx_util.h"
#include "ste_adm_dev_omx_graph.h"
#include "ste_adm_db.h"
#include "ste_adm_api_cscall.h"

#include "audio_chipset_api_index.h"
#include "audio_render_chipset_api.h"
#include "AFM_Index.h"
#include "audio_sourcesink_chipset_api.h"

#include "ste_adm_platform_adaptions.h"


#define RETURN_IF_FAIL(__f) \
    { \
        ste_adm_res_t r = __f; \
        if (r != STE_ADM_RES_OK) { \
            ALOG_ERR("Error %d in '%s'\n", r, # __f); \
            return r; \
        } \
    }


/********************* Declaration of static functions *************************
*
*
*
*******************************************************************************/

static int dev_omx_init_pcm_mode(dev_omx_graph_t*    graph_p,
                              int                     is_vc,
                              const char*             name,
                              int                     samplerate,
                              unsigned int            channels,
                              int                     alloc_buffers,
                              int                     only_common);

static ste_adm_res_t dev_omx_init_buffers(
        dev_omx_graph_t*    graph_p,
        int                 is_vc,
        OMX_U32             port_index,
        unsigned int        num_bufs,
        unsigned int        bufsz,
        int                 shm_fd_to_reuse);

static ste_adm_res_t dev_omx_connect_common_mixer_splitter_to_app_voice(
        dev_omx_graph_t* graph_p, int is_vc);

static ste_adm_res_t dev_omx_state_transition_request(dev_omx_graph_t* graph_p,
                                         int         is_vc,
                                         OMX_U32     port_index,
                                         OMX_STATETYPE      omx_state,
                                         int starting,
                                         unsigned int only_common);

static ste_adm_res_t dev_omx_state_transition_wait(dev_omx_graph_t* graph_p,
                                            int is_vc,
                                      OMX_U32               port_index,
                                      OMX_STATETYPE     omx_state,
                                      int starting,
                                      unsigned int only_common);

static ste_adm_res_t dev_omx_setup_coded_sink(dev_omx_graph_t* graph_p,
                                      const char* name,
                                      struct dev_params* dev_params,
                                      int* logical_handle_p);
static ste_adm_res_t dev_omx_close_coded_sink(dev_omx_graph_t* graph_p);

static ste_adm_res_t dev_omx_connect_effect_chain(
                            dev_omx_graph_t*            graph_p,
                            const char*                 name,
                            const char*                 name2,
                            ste_adm_effect_chain_type_t type,
                            int                         vc_disconnected);

static ste_adm_res_t dev_omx_disconnect_and_destroy_effect_chain(
                            dev_omx_graph_t*            graph_p,
                            const char*                 name,
                            const char*                 name2,
                            ste_adm_effect_chain_type_t type,
                            int                         vc_disconnected);

static const char* chaintype2str(ste_adm_effect_chain_type_t type);

static ste_adm_res_t dev_omx_remove_mixer_effects(
                                    dev_omx_graph_t* graph_p,
                                    ste_adm_effect_chain_type_t chain);

/********************* Defintion of static functions ***************************
*
*
*
*******************************************************************************/

static const char* chaintype2str(ste_adm_effect_chain_type_t type)
{
    switch (type) {
        case STE_ADM_APPLICATION_CHAIN: return "app_chain";
        case STE_ADM_VOICE_CHAIN:       return "voice_chain";
        case STE_ADM_COMMON_CHAIN:      return "common_chain";
        default:                        return "<unknown>";
    }
}

static int dev_omx_connect_components(dev_omx_graph_t*    graph_p,
                                          int                     is_vc,
                                          OMX_U32                 port_index,
                                          int                     only_common)
{
    int                                 result                  = 0;
    unsigned int                        common_graph_created    = 0;
    dev_omx_comp_chain_t            comp_chains;

    ALOG_INFO_VERBOSE("Enter dev_omx_connect_components\n");

    /* Get the App/Voice graph and Common graph OMX components */
    dev_omx_util_get_components(graph_p, is_vc, &comp_chains);

    if (!only_common)
    {
        /* Connect Mixer/Splitter and effects in App/Voice comp chain */
        result = dev_omx_graph_connect_app_voice_graph(
                        graph_p,
                        is_vc);
        STE_ADM_OMX_CHECK_RESULT(result);
    }

    /* Connect the effects in the Common comp chain, if not already connected */
    dev_omx_util_get_common_state(graph_p, &common_graph_created);
    if (!common_graph_created || only_common) {

        result = dev_omx_graph_connect_effects(
                    comp_chains.nbr_of_common_comp,
                    comp_chains.common_comp_handles_p);
        STE_ADM_OMX_CHECK_RESULT(result);

        /* Connect the Sink/Source to the Common graph */
        result = dev_omx_graph_connect_io_to_common(
                    graph_p,
                    comp_chains.nbr_of_common_comp,
                    comp_chains.common_comp_handles_p);
        STE_ADM_OMX_CHECK_RESULT(result);

        /* Connect the Mixer/Splitter to the Common graph */
        result = dev_omx_graph_connect_mixer_splitter_to_common(
                    graph_p,
                    comp_chains.nbr_of_common_comp,
                    comp_chains.common_comp_handles_p);
        STE_ADM_OMX_CHECK_RESULT(result);
    }

    /* Connect the pre effect (if applicable) */
    if (!is_vc && !only_common) {
        if (adm_omx_is_pre_effect(graph_p, port_index)) {
            result = adm_omx_graph_connect_pre_effect_to_app(
                graph_p, port_index);
            STE_ADM_OMX_CHECK_RESULT(result);
        }
    }

cleanup:
    return result;
}

static ste_adm_res_t dev_omx_config_buffers_and_lpa(dev_omx_graph_t* graph_p,
                                                    int is_lpa)
{
    //
    // LPA:
    //    * component between ARM and DSP should have the
    //      AFM_IndexParamHostMpcSync parameter set
    //
    //    * common mixer input and output should be set to
    //      use TCMX memory
    //
    //    * between ARM and DSP, there should be 3 buffers of
    //      32 ms each. 3 buffers (at least) are needed for the
    //      mechanism that synchronizes DSP execution with ARM
    //      execution by keeping one buffer until the DSP wakes up.
    //
    // General:
    //    * the tunnel between ARM and DSP should be set to use
    //      32 ms buffers. AFM default is 4096 bytes, so for mono
    //      this is 43 ms, which is a bit too much.
    //
    // Note that the buffer sizes will be wrong if the sample rate
    // or number of channels is later changed.
    //
    // Effect chains are stored such that comp_handles_p[0] is the component
    // to which data should be sent, and comp_handles_p[N-1] is the component
    // from which data is received, for both input and output devices.
    // e.g.:
    //  input graph
    //     Source --> comp_handles_p[0].EFFECT_PORT_INPUT
    //     comp_handles_p[nbr_of_comp-1].EFFECT_PORT_OUTPUT --> common_splitter.SPLITTER_PORT_INPUT
    //
    //  output graph
    //     common_mixer_comp.MIXER_PORT_OUTPUT --> comp_handles_p[0].EFFECT_PORT_INPUT
    //     comp_handles_p[nbr_of_comp-1].EFFECT_PORT_OUTPUT --> Sink

    // Helper variables
    il_comp_t app_mixer    = graph_p->app.mixer_splitter_handle;
    il_comp_t common_mixer = graph_p->common.mixer_splitter_handle;

    il_comp_t bridge_input_comp  = common_mixer;
    int       bridge_input_port  = MIXER_PORT_APP_INPUT;

    il_comp_t bridge_output_comp = app_mixer;
    int       bridge_output_port = 0;

    if (graph_p->app.nbr_of_comp > 0) {
        // Starting from the common mixer side of the graph,
        // we set bridge_input_comp to be the last component
        // on the DSP.
        int i;
        for (i=graph_p->app.nbr_of_comp-1 ; i>=0 ; i--) {
            int dsp_comp = il_tool_is_dsp_component(graph_p->app.comp_handles[i]);
            if (dsp_comp) {
                bridge_input_comp = graph_p->app.comp_handles[i];
                bridge_input_port = EFFECT_PORT_INPUT;
            } else {
                bridge_output_comp = graph_p->app.comp_handles[i];
                bridge_output_port = EFFECT_PORT_OUTPUT;
                break;
            }
        }
    }

    // The component and port the app mixer delivers data to
    il_comp_t app_mixer_output_peer_comp;
    OMX_U32   app_mixer_output_peer_port;
    if (graph_p->app.nbr_of_comp == 0) {
         // App chain is empty. App mixer delivers data to common mixer
         app_mixer_output_peer_comp = common_mixer;
         app_mixer_output_peer_port = MIXER_PORT_APP_INPUT;
    } else {
         // There are effects in the chain.
         app_mixer_output_peer_comp = graph_p->app.comp_handles[0];
         app_mixer_output_peer_port = EFFECT_PORT_INPUT;
     }


    if (is_lpa) {
        //
        // Component between ARM and DSP should have the
        // AFM_IndexParamHostMpcSync parameter set.
        //
        AFM_PARAM_HOST_MPC_SYNC_TYPE hostsync;
        IL_TOOL_INIT_CONFIG_STRUCT(hostsync);
        hostsync.nPortIndex = bridge_input_port;
        hostsync.enabled = 1;
        RETURN_IF_FAIL(il_tool_SetParameter(bridge_input_comp,
                        AFM_IndexParamHostMpcSync, &hostsync));

        //
        // Common mixer input and output should be set to
        // use TCMX memory
        //
        AFM_PARAM_BINDING_MEMORY_TYPE memtype;
        IL_TOOL_INIT_CONFIG_STRUCT(memtype);
        memtype.memoryType = AFM_MEMORY_TYPE_TCMX;

        // Input of the common mixer. Binding will be shmin used to receive
        // data from the DSP.
        memtype.nPortIndex = MIXER_PORT_APP_INPUT;
        RETURN_IF_FAIL(il_tool_SetParameter(common_mixer,
                       AFM_IndexParamBindingMemoryType, &memtype));
    }

    unsigned int num_bridge_bufs = 0;
    if (is_lpa) {
        // Set number of buffers on ARM->DSP bridge to 3 (needed for LPA).
        num_bridge_bufs = 3;
    }

    // For LPA, we set the number of buffers to 3. For non-lpa, use
    // default (specify 0).
    RETURN_IF_FAIL(il_tool_set_buffer_time(
            bridge_input_comp, bridge_input_port,
            32, num_bridge_bufs));
    RETURN_IF_FAIL(il_tool_set_buffer_time(
            bridge_output_comp, bridge_output_port,
            32, num_bridge_bufs));

    // There will be a standard tunnel between ADM and the first AFM
    // component; smaller buffers than the default are needed.
    if (bridge_input_comp != app_mixer_output_peer_comp) {
        RETURN_IF_FAIL(il_tool_set_buffer_time(
                app_mixer_output_peer_comp, app_mixer_output_peer_port,
                32, 0));
        RETURN_IF_FAIL(il_tool_set_buffer_time(
                app_mixer, 0,
                32, 0));
    }

    return STE_ADM_RES_OK;
}


static int dev_omx_init_pcm_mode(dev_omx_graph_t*    graph_p,
                              int                     is_vc,
                              const char*             name,
                              int                     samplerate,
                              unsigned int            channels,
                              int                     alloc_buffers,
                              int                     only_common)
{
    int                         result                  = 0;
    unsigned int                common_graph_created    = 0;
    unsigned int                portStart               = 0;
    unsigned int                portCount               = 0;
    dev_omx_comp_chain_t        comp_chains;
    il_comp_t                   mixer_splitter_comp;
    il_comp_t                   app_mixer_splitter_comp;
    unsigned int                i;
    OMX_PORT_PARAM_TYPE         param_port;

    IL_TOOL_INIT_CONFIG_STRUCT(param_port);

    ALOG_INFO_VERBOSE("Enter dev_omx_init_pcm_mode\n");

    /* Get the App/Voice graph and Common graph OMX components */
    dev_omx_util_get_components(graph_p, is_vc, &comp_chains);

    /* Init PCM settings for Common comp chain, if not already initialized */
    dev_omx_util_get_common_state(graph_p, &common_graph_created);

    if (!common_graph_created || only_common) {
        /* Init PCM settings for the Common Mixer/Splitter component */
        mixer_splitter_comp = graph_p->common.mixer_splitter_handle;

        for (i=0; i < 3; i++) {
            result = dev_omx_graph_init_pcm_mode_common_mixer_splitter(
                    graph_p, name, mixer_splitter_comp, i,
                    samplerate, channels);
            RETURN_IF_FAIL(result);
        }

        RETURN_IF_FAIL(il_tool_GetParameter(mixer_splitter_comp,
            OMX_IndexParamAudioInit, &param_port));
        portStart = param_port.nStartPortNumber;
        portCount = param_port.nPorts;
        ALOG_INFO("\tCommon Mixer/Splitter 0x%08X has %u ports "
            "starting at index %u\n", (unsigned int)il_tool_raw_handle(mixer_splitter_comp),
            portCount, portStart);

        for (i=(portStart+1); i < portCount  ; i++) {
            RETURN_IF_FAIL(il_tool_SendCommand(mixer_splitter_comp,
                OMX_CommandPortDisable, i, 0));
            ALOG_INFO_VERBOSE("Wait for event semaphore Down\n");
            ADM_SEM_WAIT_WITH_PRINT("PortDisable port %d Common Mixer/Splitter", i);
        }
    } else {
        ALOG_INFO("\tPCM settings already set for Common effect chain!\n");
    }

    if (!is_vc && !only_common) {
        app_mixer_splitter_comp = graph_p->app.mixer_splitter_handle;

        RETURN_IF_FAIL(il_tool_GetParameter(app_mixer_splitter_comp,
            OMX_IndexParamAudioInit, &param_port));
        portStart = param_port.nStartPortNumber;
        portCount = param_port.nPorts;
        ALOG_INFO("\tApp Mixer/Splitter 0x%08X has %u ports "
            "starting at index %u\n", (unsigned int)il_tool_raw_handle(app_mixer_splitter_comp),
            portCount, portStart);

        for (i=portStart; i < portCount; i++) {
            result = dev_omx_graph_init_pcm_mode_app_mixer_splitter(
                graph_p, name, app_mixer_splitter_comp,
                i, samplerate, channels);
            RETURN_IF_FAIL(result);

            /* Set ports in app mixer/splitter as synchronized
              (except for input port of splitter and output port of mixer) */
            if (i != 0) {
                OMX_AUDIO_PARAM_SYNCHRONIZEDTYPE sync_param;
                IL_TOOL_INIT_CONFIG_STRUCT(sync_param);
                sync_param.nPortIndex = i;
                sync_param.bIsSynchronized = OMX_TRUE;
                RETURN_IF_FAIL(il_tool_SetParameter(app_mixer_splitter_comp,
                    (OMX_INDEXTYPE)OMX_IndexParamAudioSynchronized,
                    &sync_param));
            }

        }

        if (!alloc_buffers) {
            /* Disable the unsued first port */
            if (graph_p->is_input) {
                RETURN_IF_FAIL(il_tool_SendCommand(app_mixer_splitter_comp,
                OMX_CommandPortDisable, SPLITTER_PORT_APP_OUTPUT, 0));
            } else {
                RETURN_IF_FAIL(il_tool_SendCommand(app_mixer_splitter_comp,
                OMX_CommandPortDisable, MIXER_PORT_APP_INPUT, 0));
            }

            ALOG_INFO_VERBOSE("Wait for event semaphore Down\n");
            ADM_SEM_WAIT_WITH_PRINT("PortDisable App mixer/splitter, first unused port\n");
            ALOG_INFO_VERBOSE("Event semaphore Down!\n");
        }
        /* Disable the unused Mixer/Splitter ports */
        for (i=(portStart+2) ; i < portCount  ; i++) {
            RETURN_IF_FAIL(il_tool_SendCommand(app_mixer_splitter_comp,
                OMX_CommandPortDisable, i, 0));
            ALOG_INFO_VERBOSE("Wait for event semaphore Down\n");
            ADM_SEM_WAIT_WITH_PRINT("App mixer/splitter PortDisable port %d\n", i);
        }
    }

    int is_lpa = 0;
    if (strcmp(name, STE_ADM_DEVICE_STRING_HSOUT) == 0 ||
        strcmp(name, STE_ADM_DEVICE_STRING_SPEAKER) == 0)
    {
        is_lpa = 1;
        if (!common_graph_created || only_common)
        {
            AFM_PARAM_BINDING_MEMORY_TYPE memtype;
            IL_TOOL_INIT_CONFIG_STRUCT(memtype);
            memtype.memoryType = AFM_MEMORY_TYPE_TCMX;
            memtype.nPortIndex = MIXER_PORT_OUTPUT;
            // Output of the common mixer. Binding will be pcmadapter after the mixer.
            RETURN_IF_FAIL(il_tool_SetParameter(graph_p->common.mixer_splitter_handle,
                            AFM_IndexParamBindingMemoryType, &memtype));
        }
    }

    if (!is_vc && !only_common && !graph_p->is_input) {
        RETURN_IF_FAIL(dev_omx_config_buffers_and_lpa(graph_p, is_lpa));
    }

    return result;
}

static ste_adm_res_t dev_omx_init_buffers(
        dev_omx_graph_t*    graph_p,
        int                 is_vc,
        OMX_U32             port_index,
        unsigned int        num_bufs,
        unsigned int        bufsz,
        int                 shm_fd_to_reuse)
{
    il_comp_t                       buf_comp;
    OMX_U32                         buf_port;
    OMX_PARAM_PORTDEFINITIONTYPE    port_def;
    IL_TOOL_INIT_CONFIG_STRUCT(port_def);

    ALOG_INFO_VERBOSE("Enter dev_omx_init_buffers bufsz=%d\n", bufsz);

    if (!is_vc) {
        /* Get app handle and port for which buffers should be initialized */
        adm_omx_get_buffer_comp(graph_p, port_index, &buf_comp, &buf_port);

        /* Get information from the port about minimum buffer count and size */
        port_def.nPortIndex = buf_port;
        RETURN_IF_FAIL(il_tool_GetParameter(
            buf_comp, OMX_IndexParamPortDefinition, &port_def));

        dev_omx_print_port_def_parameters(&port_def);

        port_def.nBufferSize = bufsz;
        port_def.nBufferCountActual = num_bufs;
        RETURN_IF_FAIL(il_tool_SetParameter(
            buf_comp, OMX_IndexParamPortDefinition, &port_def));

        /* Initialize I/O and allocate buffers */
        ALOG_INFO("Attaching IO to component %X, port %d\n",
            (unsigned long) il_tool_raw_handle(buf_comp), buf_port);
        RETURN_IF_FAIL(il_tool_io_alloc(buf_comp, buf_port, shm_fd_to_reuse));
    } else {
        ALOG_INFO_VERBOSE("\tDo not allocate any buffers for Voice Call graph\n");
    }

    return STE_ADM_RES_OK;
}

static ste_adm_res_t dev_omx_connect_common_mixer_splitter_to_app_voice(
        dev_omx_graph_t* graph_p, int is_vc)
{
    il_comp_t                           common_mixer_splitter_comp;
    il_comp_t                           chain_comp;
    OMX_U32                             common_port_index;
    OMX_U32                             chain_port;
    OMX_PARAM_PORTDEFINITIONTYPE        common_port_def_param;
    OMX_PARAM_PORTDEFINITIONTYPE        chain_port_def_param;
    OMX_AUDIO_PARAM_SYNCHRONIZEDTYPE    audio_synch_param;
    dev_omx_comp_chain_t                comp_chains;

    IL_TOOL_INIT_CONFIG_STRUCT(common_port_def_param);
    IL_TOOL_INIT_CONFIG_STRUCT(chain_port_def_param);
    IL_TOOL_INIT_CONFIG_STRUCT(audio_synch_param);

    ALOG_INFO_VERBOSE("Enter dev_omx_connect_common_mixer_splitter_to_app_voice\n");
    common_mixer_splitter_comp = graph_p->common.mixer_splitter_handle;

    if (!is_vc) {
        if (!graph_p->is_input) {
            common_port_index = MIXER_PORT_APP_INPUT;

            /* Set App Chain port in Common Mixer/Spliter as synchronized */
            audio_synch_param.nPortIndex = common_port_index;
            audio_synch_param.nSize = sizeof(audio_synch_param);
            RETURN_IF_FAIL(il_tool_GetParameter(common_mixer_splitter_comp,
                (OMX_INDEXTYPE)OMX_IndexParamAudioSynchronized,
                &audio_synch_param));

            audio_synch_param.nPortIndex = common_port_index;
            audio_synch_param.bIsSynchronized = OMX_TRUE;
            RETURN_IF_FAIL(il_tool_SetParameter(common_mixer_splitter_comp,
                (OMX_INDEXTYPE)OMX_IndexParamAudioSynchronized,
                &audio_synch_param));
        } else {
            common_port_index = SPLITTER_PORT_APP_OUTPUT;
        }



        /* Set up tunnel and enable the App Chain Mixer/Splitter port and app chain end port*/

        /* Get component and port settigns */
        adm_omx_get_app_end_comp(graph_p, &chain_comp, &chain_port);

        common_port_def_param.nPortIndex = common_port_index;
        RETURN_IF_FAIL(il_tool_GetParameter(common_mixer_splitter_comp,
            OMX_IndexParamPortDefinition, &common_port_def_param));

        chain_port_def_param.nPortIndex = chain_port;
        RETURN_IF_FAIL(il_tool_GetParameter(chain_comp,
            OMX_IndexParamPortDefinition, &chain_port_def_param));



        /* Setup the tunnel */
        if (graph_p->is_input)
        {
            RETURN_IF_FAIL(il_tool_connect_and_enable(common_mixer_splitter_comp, common_port_index, chain_comp, chain_port,
                                                      1, 1, 1, OMX_BufferSupplyUnspecified));
        }
        else
        {
            if (adaptions_internal_mixsplit()) {

                OMX_AUDIO_PARAM_PCMMODETYPE pcm_params;
                IL_TOOL_INIT_CONFIG_STRUCT(pcm_params);
                pcm_params.nPortIndex = chain_port;
                RETURN_IF_FAIL(il_tool_GetParameter(chain_comp, OMX_IndexParamAudioPcm, &pcm_params));

                pcm_params.nPortIndex = common_port_index;
                RETURN_IF_FAIL(il_tool_SetParameter(common_mixer_splitter_comp, OMX_IndexParamAudioPcm, &pcm_params));
            }

            // We want the input port to supply the buffers to avoid a memcpy. If they are provided
            // by the DSP port, they are already mapped in DSP memory.
            // To avoid problems with the order in which components are enabled, do not do this
            // if there are effects in the chain (it wouldn't help LPA anyway).
            OMX_BUFFERSUPPLIERTYPE supplier_preference = OMX_BufferSupplyUnspecified;
            if (graph_p->app.nbr_of_comp == 0) {
                supplier_preference = OMX_BufferSupplyInput;
            }

            RETURN_IF_FAIL(il_tool_connect_and_enable(chain_comp, chain_port, common_mixer_splitter_comp, common_port_index, 1, 0, 0,
                                                      supplier_preference));
        }
    } else {
        ADM_ASSERT(is_vc);
        OMX_AUDIO_PARAM_PCMMODETYPE pcm_params;
        IL_TOOL_INIT_CONFIG_STRUCT(pcm_params);

        dev_omx_util_get_components(graph_p, 1, &comp_chains);
        if (!graph_p->is_input) {
            common_port_index = MIXER_PORT_VOICE_INPUT;

            /* Set Voice Chain port in Common Mixer as synchronized */
            audio_synch_param.nPortIndex = common_port_index;
            RETURN_IF_FAIL(il_tool_GetParameter(common_mixer_splitter_comp,
                (OMX_INDEXTYPE)OMX_IndexParamAudioSynchronized,
                &audio_synch_param));

            audio_synch_param.nPortIndex = common_port_index;
            audio_synch_param.bIsSynchronized = OMX_TRUE; /* Enable Synch */
            RETURN_IF_FAIL(il_tool_SetParameter(common_mixer_splitter_comp,
                (OMX_INDEXTYPE)OMX_IndexParamAudioSynchronized,
                &audio_synch_param));

            /* If there are effects in VC effect chain, they have to be connected to the common mixer,
               and the ports need to be enabled */
            if (comp_chains.nbr_of_comp > 0) {

                /* Propagte PCM settings to common mixer */
                pcm_params.nPortIndex = EFFECT_PORT_OUTPUT;
                RETURN_IF_FAIL(il_tool_GetParameter(comp_chains.comp_handles_p[comp_chains.nbr_of_comp-1], OMX_IndexParamAudioPcm, &pcm_params));

                pcm_params.nPortIndex = common_port_index;
                RETURN_IF_FAIL(il_tool_SetParameter(common_mixer_splitter_comp, OMX_IndexParamAudioPcm, &pcm_params));

                /* Set up tunnel to common splitter. */
                RETURN_IF_FAIL(il_tool_SetupTunnel(comp_chains.comp_handles_p[comp_chains.nbr_of_comp-1],
                                                   EFFECT_PORT_OUTPUT,
                                                   common_mixer_splitter_comp,
                                                   common_port_index));

                RETURN_IF_FAIL(il_tool_SendCommand(common_mixer_splitter_comp,
                    OMX_CommandPortEnable, common_port_index, NULL));

                RETURN_IF_FAIL(il_tool_SendCommand(
                    comp_chains.comp_handles_p[comp_chains.nbr_of_comp-1],
                    OMX_CommandPortEnable, EFFECT_PORT_OUTPUT, NULL));

                /* Wait for common mixer port to be enabled */
                ADM_SEM_WAIT_WITH_PRINT("Wait for common mixer port/port of effect to be enabled, 1/2");

                /* Wait for port of the effect to be enabled */
                ADM_SEM_WAIT_WITH_PRINT("Wait for common mixer port/port of effect to be enabled, 2/2");
            }
        } else {
            ADM_ASSERT(graph_p->is_input);

            common_port_index = SPLITTER_PORT_VOICE_OUTPUT;

            /* If there are effects in VC effect chain, they have to be connected to the common splitter,
               and the ports need to be enabled */
            if (comp_chains.nbr_of_comp > 0) {

                /* Propagte PCM settings to common mixer */
                pcm_params.nPortIndex = EFFECT_PORT_OUTPUT;
                RETURN_IF_FAIL(il_tool_GetParameter(comp_chains.comp_handles_p[0], OMX_IndexParamAudioPcm, &pcm_params));

                pcm_params.nPortIndex = common_port_index;
                RETURN_IF_FAIL(il_tool_SetParameter(common_mixer_splitter_comp, OMX_IndexParamAudioPcm, &pcm_params));

                /* Set up tunnel to common mixer. */
                RETURN_IF_FAIL(il_tool_SetupTunnel(common_mixer_splitter_comp,
                                                   common_port_index,
                                                   comp_chains.comp_handles_p[0],
                                                   EFFECT_PORT_INPUT));
                RETURN_IF_FAIL(il_tool_SendCommand(
                    comp_chains.comp_handles_p[0],
                    OMX_CommandPortEnable, EFFECT_PORT_INPUT, NULL));

                RETURN_IF_FAIL(il_tool_SendCommand(common_mixer_splitter_comp,
                    OMX_CommandPortEnable, common_port_index, NULL));

                /* Wait for common mixer port to be enabled */
                ADM_SEM_WAIT_WITH_PRINT("enable common mixer port/enable effect port 1/2");
                ADM_SEM_WAIT_WITH_PRINT("enable common mixer port/enable effect port 2/2");
            }
        }
    }

    return STE_ADM_RES_OK;
}

static ste_adm_res_t dev_omx_state_transition_request(dev_omx_graph_t* graph_p,
                                         int         is_vc,
                                         OMX_U32     port_index,
                                         OMX_STATETYPE      omx_state,
                                         int starting,
                                         unsigned int only_common)
{
    unsigned int                nbr_of_connected_graphs = 0;
    unsigned int                *ref_cnt_p              = NULL;
    dev_omx_comp_chain_t    comp_chains;
    il_comp_t                   io_comp;
    il_comp_t                   mixer_splitter_comp;
    il_comp_t                   app_mixer_splitter_comp;
    il_comp_t                   pre_effect_comp;
    unsigned int                i;
    int                         stage_index, stage_start, stage_end, stage_inc;
    int                         transit_chain_forward = 0;
    unsigned int                request_app_vc = 0;
    unsigned int                request_common = 0;


    ALOG_INFO_VERBOSE("Enter dev_omx_state_transition_request - %s\n",
        dev_omx_stateid2statestring(omx_state));

    if (adm_sem_get_count(&g_adm_event_sem) != 0) {
        ALOG_WARN("Semaphore count != 0 before dev_omx_state_transition_request starts\n");
    }

    dev_omx_util_get_components(graph_p, is_vc, &comp_chains);
    dev_omx_util_get_common_state(graph_p, &nbr_of_connected_graphs);

    adm_omx_util_get_ref_cnt(graph_p, is_vc, &ref_cnt_p);

    /* Transition stategy:
       All transitions except for Executing -> Idle from non-supplier to supplier (sink -> source)
       Executing -> Idle from supplier to non-supplier (source -> sink)
       The default behaviour from the framework is that the output port is the supplier */
    if (!starting && omx_state == OMX_StateIdle) {
        if (!graph_p->is_input) {
            stage_start = 0;
            stage_end = 5;
            stage_inc = 1;
            transit_chain_forward = 1;
        } else {
            stage_start = 5;
            stage_end = 0;
            stage_inc = -1;
            transit_chain_forward = 1;
        }
    } else {
        if (!graph_p->is_input) {
            stage_start = 5;
            stage_end = 0;
            stage_inc = -1;
            transit_chain_forward = 0;
        } else {
            stage_start = 0;
            stage_end = 5;
            stage_inc = 1;
            transit_chain_forward = 0;
        }
    }

    /* Check if state transition should be requested for app/vc chain */
    if (!only_common && ((starting && (*ref_cnt_p == 0)) ||
        (!starting && (*ref_cnt_p == 1)))) {
        request_app_vc = 1;
    }

    /* Check if state transition should be requested for common chain and io */
    if (request_app_vc) {
        if ((starting && (nbr_of_connected_graphs == 0)) ||
            ((!starting) && (nbr_of_connected_graphs == 1))) {
            request_common = 1;
        }
    }
    else
    {
        request_common = only_common;
    }



    for (stage_index=stage_start; stage_index!=stage_end+stage_inc;
            stage_index += stage_inc) {
         switch (stage_index) {

         case 0: /* pre effect */

            if (!is_vc && !only_common && adm_omx_is_pre_effect(graph_p, port_index)) {
                ALOG_INFO("Transition for pre effect\n");
                ADM_ASSERT((port_index-1) < ADM_OMX_MAX_NBR_OF_PORTS);
                pre_effect_comp = graph_p->app.pre_effect_handle[port_index-1];

                RETURN_IF_FAIL(il_tool_SendCommand(
                    pre_effect_comp, OMX_CommandStateSet,omx_state, 0));
            }
            break;

        case 1: /* app mixer/splitter */
            if (request_app_vc && !is_vc) {
                ALOG_INFO("Transition for app mixer/splitter\n");
                app_mixer_splitter_comp = graph_p->app.mixer_splitter_handle;
                RETURN_IF_FAIL(il_tool_SendCommand(
                    app_mixer_splitter_comp, OMX_CommandStateSet, omx_state, 0));
            }
            break;

        case 2: /* app/vc effects */
            if (request_app_vc) {
                ALOG_INFO("Transition for app/vc effects\n");
                /* Request state trans for all effects in the App/Voice chain */
                for (i = 0; i < comp_chains.nbr_of_comp; i++) {

                    if ( transit_chain_forward == 1 ) {
                        RETURN_IF_FAIL(il_tool_SendCommand(
                            comp_chains.comp_handles_p[i],
                            OMX_CommandStateSet,
                            omx_state, 0));
                    } else {
                        RETURN_IF_FAIL(il_tool_SendCommand(
                            comp_chains.comp_handles_p[
                            comp_chains.nbr_of_comp-1-i],
                            OMX_CommandStateSet,
                            omx_state, 0));
                    }
                }
            }
            break;

        case 3: /* common mixer/splitter */
            if (request_common) {
                ALOG_INFO("Transition for common mixer/splitter\n");
                /* Request state transition for the Common Mixer/Splitter */
                mixer_splitter_comp = graph_p->common.mixer_splitter_handle;
                RETURN_IF_FAIL(il_tool_SendCommand(mixer_splitter_comp,
                    OMX_CommandStateSet, omx_state, 0));
            }
            break;

        case 4: /* common effects */
            if (request_common) {
                ALOG_INFO("Transition for common effects\n");
                for (i = 0; i < comp_chains.nbr_of_common_comp; i++) {
                    if (transit_chain_forward) {
                       RETURN_IF_FAIL(il_tool_SendCommand(
                           comp_chains.common_comp_handles_p[i],
                           OMX_CommandStateSet,
                           omx_state, 0));
                    } else {
                        RETURN_IF_FAIL(il_tool_SendCommand(
                           comp_chains.common_comp_handles_p[comp_chains.nbr_of_common_comp-1-i],
                           OMX_CommandStateSet,
                           omx_state, 0));
                    }
                }
            }
            break;

        case 5: /* IO component */

            if (request_common) {
                ALOG_INFO("Transition for sink/source\n");
                /* Request state trans for the io component, i.e. sink/source */
                io_comp = graph_p->common.io_handle;
                RETURN_IF_FAIL(il_tool_SendCommand(io_comp, OMX_CommandStateSet,
                    omx_state, 0));
            }
            break;
        default:
            ADM_ASSERT(0);
            ALOG_ERR("Internal error\n");
            return STE_ADM_RES_INTERNAL_ERROR;
        }
    }


    return STE_ADM_RES_OK;
}

static ste_adm_res_t dev_omx_state_transition_wait(dev_omx_graph_t* graph_p,
                                            int is_vc,
                                      OMX_U32               port_index,
                                      OMX_STATETYPE     omx_state,
                                      int starting,
                                      unsigned int only_common)
{
    unsigned int                nbr_of_connected_graphs = 0;
    unsigned int                *ref_cnt_p              = NULL;
    dev_omx_comp_chain_t    comp_chains;
    il_comp_t                   io_comp;
    il_comp_t                   mixer_splitter_comp;
    il_comp_t                   app_mixer_splitter_comp;
    il_comp_t                   pre_effect_comp;
    unsigned int                request_app_vc = 0;
    unsigned int                request_common = 0;

    ALOG_INFO_VERBOSE("Enter dev_omx_state_transition_wait - %s\n",
        dev_omx_stateid2statestring(omx_state));

    dev_omx_util_get_common_state(graph_p, &nbr_of_connected_graphs);
    adm_omx_util_get_ref_cnt(graph_p, is_vc, &ref_cnt_p);

    dev_omx_util_get_components(graph_p, is_vc, &comp_chains);

    /* Wait for all effects in App/Voice chain to transition to new state */
    RETURN_IF_FAIL(dev_omx_graph_state_transition_wait(comp_chains.nbr_of_comp));

    /* Check if state transition has been requested for app/vc chain */
    if (!only_common && ((starting && (*ref_cnt_p == 0)) ||
        (!starting && (*ref_cnt_p == 1)))) {
        request_app_vc = 1;
    }

    /* Check if state transition has been requested for common chain and io */
    if (request_app_vc) {
        if ((starting && (nbr_of_connected_graphs == 0)) ||
            ((!starting) && (nbr_of_connected_graphs == 1))) {
            request_common = 1;
        }
    }
    else
    {
        request_common = only_common;
    }

    if (request_common) {
        /* Wait for all effects in Common chain to transition to new state */
        RETURN_IF_FAIL(dev_omx_graph_state_transition_wait(comp_chains.nbr_of_common_comp));

        /* Wait for Sink/Source to transition to new state */
        RETURN_IF_FAIL(dev_omx_graph_state_transition_wait(1));

        /* Wait for Common Mixer/Splitter to transition to new state */
        RETURN_IF_FAIL(dev_omx_graph_state_transition_wait(1));
    }

    /* Wait for App Chain Mixer/Splitter to transition to new state */
    if (request_app_vc && !is_vc) {
        RETURN_IF_FAIL(dev_omx_graph_state_transition_wait(1));
    }

    /* Wait for pre effect to transition to new state */
    if (!is_vc && !only_common && adm_omx_is_pre_effect(graph_p, port_index)) {
        RETURN_IF_FAIL(dev_omx_graph_state_transition_wait(1));
    }


    /* Check that all effects in App/VC chain have transitioned to new state */
    if (request_app_vc) {
        RETURN_IF_FAIL(dev_omx_graph_state_transition_check(
            comp_chains.comp_handles_p,
            comp_chains.nbr_of_comp, omx_state));
    }

    if (request_common) {
        /* Check that all effects in Common chain have transitioned to new state */
        RETURN_IF_FAIL(dev_omx_graph_state_transition_check(
            comp_chains.common_comp_handles_p,
            comp_chains.nbr_of_common_comp, omx_state));

        /* Check that Sink/Source has transitioned to new state */
        io_comp = graph_p->common.io_handle;
        RETURN_IF_FAIL(dev_omx_graph_state_transition_check(&io_comp, 1, omx_state));

        /* Check that Common Mixer/Splitter has transitioned to new state */
        mixer_splitter_comp = graph_p->common.mixer_splitter_handle;
        RETURN_IF_FAIL(dev_omx_graph_state_transition_check(
            &mixer_splitter_comp, 1, omx_state));
    }

    if (request_app_vc && !is_vc) {
        /* Check that App Chain Mixer/Splitter has transitioned to new state */
        app_mixer_splitter_comp = graph_p->app.mixer_splitter_handle;
        RETURN_IF_FAIL(dev_omx_graph_state_transition_check(
            &app_mixer_splitter_comp, 1, omx_state));
    }

    if (!is_vc && !only_common && adm_omx_is_pre_effect(graph_p, port_index)) {
        /* Check that pre effect has transitioned to new state */
        ADM_ASSERT((port_index-1) < ADM_OMX_MAX_NBR_OF_PORTS);
        pre_effect_comp = graph_p->app.pre_effect_handle[port_index-1];

        RETURN_IF_FAIL(dev_omx_graph_state_transition_check(
            &pre_effect_comp, 1, omx_state));
    }

    return STE_ADM_RES_OK;
}

/********************* Defintion of exported functions ************************
*
*
*
*******************************************************************************/




/**
 * Read a chain from the database and configure the components
 */
static int adm_setup_chain(dev_omx_graph_t* graph_p,
                            const char* name,
                            struct dev_params* dev_params,
                            ste_adm_effect_chain_type_t chain)
{
    adm_db_comp_iter_t compit;
    int compit_alloced = 0;
    const char* il_name = NULL;
    adm_db_cfg_iter_t cfgit;
    memset(&cfgit, 0, sizeof(cfgit));
    int cfgit_alloced = 0;
    ste_adm_res_t res = STE_ADM_RES_OK;

    il_comp_t* comp_handle_p = NULL;
    unsigned int *nbr_of_comp = NULL;
    unsigned int *nbr_mixer_effects = NULL;

    ALOG_INFO("Extracting components for chain %d, dev ='%s'\n",
              chain, name);

    /* Create iterator */
    if (chain == STE_ADM_VOICE_CHAIN) {
        comp_handle_p = graph_p->voice.comp_handles;
        nbr_of_comp = &graph_p->voice.nbr_of_comp;
        nbr_mixer_effects = &graph_p->voice.nbr_mixer_effects;

        if (graph_p->is_input) {
            res = adm_db_comp_iter_create(
                name,
                dev_params->name_out,
                chain, dev_params->samplerate, &compit);
        } else {
            res = adm_db_comp_iter_create(
                name,
                dev_params->name_in,
                chain, dev_params->samplerate, &compit);
        }
    } else {
        if (chain == STE_ADM_APPLICATION_CHAIN) {
            comp_handle_p = graph_p->app.comp_handles;
            nbr_of_comp = &graph_p->app.nbr_of_comp;
            nbr_mixer_effects = &graph_p->app.nbr_mixer_effects;
        }
        else {
            comp_handle_p = graph_p->common.comp_handles;
            nbr_of_comp = &graph_p->common.nbr_of_comp;
            nbr_mixer_effects = &graph_p->common.nbr_mixer_effects;
        }
        res = adm_db_comp_iter_create(name,NULL,
                    chain, dev_params->samplerate, &compit);
    }
    if (res != STE_ADM_RES_OK) {
        ALOG_ERR("adm_db_comp_iter_create failed for dev='%s'\n",name);
        goto cleanup;
    } else {
        compit_alloced = 1;
    }

    int position = 0;
    *nbr_mixer_effects = 0;

    /* iterate over components */
    while ((position < ADM_OMX_CHAIN_MAX_LENGTH) && STE_ADM_RES_OK ==
            (res = adm_db_comp_iter_get_next(&compit, &il_name, &cfgit))) {
        /* Variables for config iteration */
        cfgit_alloced = 1;
        ALOG_INFO_VERBOSE("Got a component\n");
        unsigned int i = 0;

        const char *mixer_effect = adaptions_get_mixer_internal_effect_name(il_name);
        if (!graph_p->use_mixer_effects || mixer_effect == NULL) {

            if (*nbr_mixer_effects > 0 && (chain == STE_ADM_APPLICATION_CHAIN || chain == STE_ADM_VOICE_CHAIN)) {
                ALOG_ERR("Non mixer effect found after mixer effect on input port.");
                res = STE_ADM_RES_IL_UNKNOWN_ERROR;
                goto cleanup;
            }

            /* Store component handle in device_p */
            i = (*nbr_of_comp)++;

            res = il_tool_create_component(&(comp_handle_p[i]), il_name, "chain");
            RETURN_IF_FAIL(res);

            ALOG_INFO("adm_create_component returned 0x%08X for %s\n",
                (unsigned int) (il_tool_raw_handle(comp_handle_p[i])), il_name);

            /* Configure component using the config iterator.
               Both SetParameter and SetConfig */
            if (STE_ADM_RES_OK != adm_setup_component(comp_handle_p[i], &cfgit, 1, 1)) {
                ALOG_ERR("Failed to configure component");
                res = STE_ADM_RES_IL_UNKNOWN_ERROR;
                goto cleanup;
            }
        }
        else {
            if (*nbr_of_comp > 0 && chain == STE_ADM_COMMON_CHAIN) {
                ALOG_ERR("mixer effect found after non mixer effect on output port.");
                res = STE_ADM_RES_IL_UNKNOWN_ERROR;
                goto cleanup;
            }

            (*nbr_mixer_effects)++;

            if (STE_ADM_RES_OK != adm_setup_mixer_effect(graph_p, *nbr_mixer_effects, chain, il_name, &cfgit, 1, 1, 1)) {
                ALOG_ERR("Failed to configure mixer effect");
                res = STE_ADM_RES_IL_UNKNOWN_ERROR;
                goto cleanup;
            }
        }



#ifdef ADM_MMPROBE
        /* Configure PCM probe for component */
        pcm_probe_comp_t pcm_probe_comp;
        int mixer_port;
        switch (chain) {
        case STE_ADM_APPLICATION_CHAIN:
            pcm_probe_comp = PCM_PROBE_APP_EFFECT;
            mixer_port = MIXER_PORT_APP_INPUT;
            break;
        case STE_ADM_VOICE_CHAIN:
            pcm_probe_comp = PCM_PROBE_VOICE_EFFECT;
            mixer_port = MIXER_PORT_VOICE_INPUT;
            break;
        case STE_ADM_COMMON_CHAIN:
        default:
            pcm_probe_comp = PCM_PROBE_COMMON_EFFECT;
            mixer_port = MIXER_PORT_OUTPUT;
            break;
        }

        int probe_id;
        int probe_enabled;
        if (dev_get_pcm_probe(name, pcm_probe_comp, position, &probe_id, &probe_enabled) == STE_ADM_RES_OK) {
            AFM_AUDIO_CONFIG_PCM_PROBE pcm_probe_cfg;
            IL_TOOL_INIT_CONFIG_STRUCT(pcm_probe_cfg);
            pcm_probe_cfg.nPortIndex = EFFECT_PORT_OUTPUT;
            pcm_probe_cfg.nProbeIdx = probe_id;
            pcm_probe_cfg.bEnable = probe_enabled;

            if (!graph_p->use_mixer_effects || mixer_effect == NULL) {

                if (il_tool_SetConfig(comp_handle_p[i], AFM_IndexConfigPcmProbe, &pcm_probe_cfg) != STE_ADM_RES_OK) {
                    ALOG_WARN("AFM_IndexConfigPcmProbe not supported by comp=%p (%s, %s, port_index=%d, effect_index=%d)",
                        il_tool_raw_handle(comp_handle_p[i]), name, probe_comp2str(pcm_probe_comp), EFFECT_PORT_OUTPUT, position);
                }
            }
            else {
               if (il_tool_setConfig_mixer_effect(graph_p->common.mixer_splitter_handle ,*nbr_mixer_effects, mixer_port, AFM_IndexConfigPcmProbe, &pcm_probe_cfg) != STE_ADM_RES_OK) {
                    ALOG_WARN("AFM_IndexConfigPcmProbe not supported by comp=%p (%s, %s, port_index=%d, effect_index=%d)",
                        il_tool_raw_handle(graph_p->common.mixer_splitter_handle), name, probe_comp2str(pcm_probe_comp), EFFECT_PORT_OUTPUT, position);
               }
               if (il_tool_commit_mixer_effects(graph_p->common.mixer_splitter_handle, mixer_port) != STE_ADM_RES_OK) {
                    ALOG_WARN("Failed to commit AFM_IndexConfigPcmProbe on comp=%p (%s, %s, port_index=%d, effect_index=%d)",
                        il_tool_raw_handle(graph_p->common.mixer_splitter_handle), name, probe_comp2str(pcm_probe_comp), EFFECT_PORT_OUTPUT, position);
               }
            }
        } else {
            ALOG_ERR("dev_get_pcm_probe failed for %s (%d, %d)", name, (int)pcm_probe_comp, position);
        }

#endif // ADM_MMPROBE

        /* Destroy cfg iterator */
        adm_db_cfg_iter_destroy(&cfgit);
        cfgit_alloced = 0;

        position++;
    }
    if (STE_ADM_RES_OK == res && *nbr_of_comp == ADM_OMX_CHAIN_MAX_LENGTH) {
        ALOG_ERR("Not enough space to store all components in chain");
        res = STE_ADM_RES_CHAIN_TOO_LONG;
    } else if (STE_ADM_RES_NO_MORE != res) {
        ALOG_ERR("Error iterating over components");
        res = STE_ADM_RES_UNKNOWN_DATABASE_ERROR;
        goto cleanup;
    } else {
        ALOG_INFO_VERBOSE("No more components\n");
        res = STE_ADM_RES_OK;
    }

cleanup:
    /* Destroy component iterator */
    if (cfgit_alloced) {
        adm_db_cfg_iter_destroy(&cfgit);
    }
    if (compit_alloced) {
        adm_db_comp_iter_destroy(&compit);
    }
    return res;
}


/**
 * 1. Load component from DB
 * 2. Set parameters / config
 */
//X static int adm_setup_effects(device_t *device_p, ste_adm_devid_t device_id,
//X                       int samplerate)
ste_adm_res_t adm_setup_effects(const char* name, dev_omx_graph_t* graph_p,
                int is_vc, struct dev_params* dev_params, int only_common)
{
    unsigned int common_graph_created;
    dev_omx_util_get_common_state(graph_p, &common_graph_created);
    int is_hdmi = 0;
    if (!common_graph_created || only_common) {
        if (strcmp(name, STE_ADM_DEVICE_STRING_HDMI) == 0) {
            ALOG_INFO("Device is HDMI, enabling special case (multichan app mixer, host common mixer)\n");
            is_hdmi = 1;
        }
        RETURN_IF_FAIL(adm_omx_graph_init_common(graph_p, is_hdmi));
    }

#ifdef ADM_DISABLE_MIXER_EFFECTS
    graph_p->use_mixer_effects = 0;
#else
    if (is_hdmi || graph_p->is_input) {
        graph_p->use_mixer_effects = 0;
    }
    else {
        graph_p->use_mixer_effects = 1;
    }
#endif

    if (!only_common)
    {
        /* Allocate space for effect components and initialize structs */
        RETURN_IF_FAIL(adm_omx_graph_init(graph_p, is_vc));

        if (is_vc) {
            /* Setup VC chain */
            RETURN_IF_FAIL(adm_setup_chain(graph_p, name, dev_params, STE_ADM_VOICE_CHAIN));
        } else {
            /* Setup APP chain */
            RETURN_IF_FAIL(adm_setup_chain(graph_p, name, dev_params, STE_ADM_APPLICATION_CHAIN));
        }
    }

    if (!common_graph_created || only_common) {
        /* Setup COMMON chain */
        RETURN_IF_FAIL(adm_setup_chain(graph_p, name, dev_params, STE_ADM_COMMON_CHAIN));

        /* Setup IO */
        {
            char *io_name = NULL;
            adm_db_cfg_iter_t cfg_iter;
            memset(&cfg_iter, 0, sizeof(cfg_iter));

            ste_adm_res_t res = adm_db_io_cfg_iter_create(name, &io_name, &cfg_iter);
            if (res != STE_ADM_RES_OK) {
                adm_db_cfg_iter_destroy(&cfg_iter);
                return STE_ADM_RES_UNKNOWN_DATABASE_ERROR; // TODO: Better db errors
            }

            ADM_ASSERT(io_name != NULL);
            res = il_tool_create_component(
                &graph_p->common.io_handle, io_name, "io");

            free(io_name);
            if (res != STE_ADM_RES_OK) {
                adm_db_cfg_iter_destroy(&cfg_iter);
                return res; // TODO
            }

            res = adm_setup_component(graph_p->common.io_handle,
                &cfg_iter, 1, 1); //Both SetParameter and SetConfig
            adm_db_cfg_iter_destroy(&cfg_iter);
            RETURN_IF_FAIL(res);

#ifdef ADM_MMPROBE
            /* Configure PCM probe for in/out port of IO component */
            int probe_id;
            int probe_enabled;
            int port_index = graph_p->is_input ? SOURCE_PORT_OUTPUT : SINK_PORT_INPUT;
            if (dev_get_pcm_probe(name, PCM_PROBE_IO, port_index, &probe_id, &probe_enabled) == STE_ADM_RES_OK) {
                AFM_AUDIO_CONFIG_PCM_PROBE pcm_probe_cfg;
                IL_TOOL_INIT_CONFIG_STRUCT(pcm_probe_cfg);
                pcm_probe_cfg.nPortIndex = port_index;
                pcm_probe_cfg.nProbeIdx = probe_id;
                pcm_probe_cfg.bEnable = probe_enabled;
                if (il_tool_SetConfig(graph_p->common.io_handle, AFM_IndexConfigPcmProbe, &pcm_probe_cfg) != STE_ADM_RES_OK) {
                    ALOG_WARN("AFM_IndexConfigPcmProbe not supported by comp=%p (%s, %s, port_index=%d))",
                        il_tool_raw_handle(graph_p->common.io_handle), name, probe_comp2str(PCM_PROBE_IO), port_index);
                }
            } else {
                ALOG_ERR("dev_get_pcm_probe failed for %s (%s, %d)", name, probe_comp2str(PCM_PROBE_IO), port_index);
            }

            /* Configure PCM probe for reference port of IO component */
            il_comp_t io_comp;
            OMX_U32 io_port;
            res = dev_omx_get_endpoint_echoref(graph_p, &io_comp, &io_port);
            if (res == STE_ADM_RES_OK && io_comp) {
                if (dev_get_pcm_probe(name, PCM_PROBE_IO, io_port, &probe_id, &probe_enabled) == STE_ADM_RES_OK) {
                    AFM_AUDIO_CONFIG_PCM_PROBE pcm_probe_cfg;
                    IL_TOOL_INIT_CONFIG_STRUCT(pcm_probe_cfg);
                    pcm_probe_cfg.nPortIndex = io_port;
                    pcm_probe_cfg.nProbeIdx = probe_id;
                    pcm_probe_cfg.bEnable = probe_enabled;
                    if (il_tool_SetConfig(graph_p->common.io_handle, AFM_IndexConfigPcmProbe, &pcm_probe_cfg) != STE_ADM_RES_OK) {
                        ALOG_WARN("AFM_IndexConfigPcmProbe not supported by comp=%p (%s, %s, port_index=%d))",
                            il_tool_raw_handle(graph_p->common.io_handle), name, probe_comp2str(PCM_PROBE_IO), io_port);
                    }
                } else {
                    ALOG_ERR("dev_get_pcm_probe failed for %s (%s, %d)", name, probe_comp2str(PCM_PROBE_IO), io_port);
                }
            }
#endif // ADM_MMPROBE

        }

        OMX_AUDIO_PARAM_PCMMODETYPE pcm_mode;
        IL_TOOL_INIT_CONFIG_STRUCT(pcm_mode);
        pcm_mode.nPortIndex = 0;
        RETURN_IF_FAIL(il_tool_GetParameter(graph_p->common.io_handle,OMX_IndexParamAudioPcm, &pcm_mode));
        graph_p->common.hw_num_channels = (int) pcm_mode.nChannels;
        ALOG_INFO("HW default is %d channels\n", graph_p->common.hw_num_channels);
    }

    return STE_ADM_RES_OK;
}

static ste_adm_res_t adm_setup_pre_effect(dev_omx_graph_t* graph_p, int is_vc, const char* name,
                                OMX_U32 port_index, unsigned int input_sample_rate,
                                unsigned int input_channels, unsigned int *output_channels_p)
{
    ste_adm_res_t res = 0;
    adm_db_cfg_iter_t cfg_iter;
    memset(&cfg_iter, 0, sizeof(cfg_iter));
    /* Check if pre effect is needed, i.e. if the device is stereo or mono and
       the input of the chain is configured to 5.1 */
    *output_channels_p = input_channels;
    ALOG_INFO("adm_setup_pre_effect\n");
    if (!is_vc) {
        OMX_AUDIO_PARAM_PCMMODETYPE pcm_mode;
        IL_TOOL_INIT_CONFIG_STRUCT(pcm_mode);
        pcm_mode.nPortIndex = 0;
        RETURN_IF_FAIL(il_tool_GetParameter(graph_p->common.io_handle,
            OMX_IndexParamAudioPcm, &pcm_mode));

        ALOG_INFO("adm_setup_pre_effect: pcm_mode.nChannels=%d input_channels=%d\n", pcm_mode.nChannels, input_channels);
        if (((pcm_mode.nChannels == 1 || pcm_mode.nChannels == 2) && (input_channels == 6)) ||
           ((pcm_mode.nChannels == 6) && (input_channels == 2))) {
            /* Pre effect is needed in chain input */
            char *pre_effect_name = NULL;
            res = adm_db_pre_effect_cfg_iter_create(
                name,
                &pre_effect_name, &cfg_iter);
            if (res != STE_ADM_RES_OK) {
                adm_db_cfg_iter_destroy(&cfg_iter);
                free(pre_effect_name);
                goto cleanup;
            }

            if (pre_effect_name) {
                ALOG_INFO("Setup pre effect - channels=%d, pre_effect_name=%s",
                    input_channels, pre_effect_name);
                res = il_tool_create_component(
                    &graph_p->app.pre_effect_handle[port_index-1],
                    pre_effect_name, "pre_effect");
                free(pre_effect_name);
                if (res != 0) {
                    adm_db_cfg_iter_destroy(&cfg_iter);
                    goto cleanup;
                }

                res = il_tool_change_port_state(graph_p->app.pre_effect_handle[port_index-1], 1, 0, 0);
                if (res != STE_ADM_RES_OK) {
                    adm_db_cfg_iter_destroy(&cfg_iter);
                    goto cleanup;
                }

                res = il_tool_change_port_state(graph_p->app.pre_effect_handle[port_index-1], 0, 0, 0);
                if (res != STE_ADM_RES_OK) {
                    adm_db_cfg_iter_destroy(&cfg_iter);
                    goto cleanup;
                }

                res = adm_setup_component(
                    graph_p->app.pre_effect_handle[port_index-1],
                    &cfg_iter, 1, 1);
                adm_db_cfg_iter_destroy(&cfg_iter);
                STE_ADM_OMX_CHECK_RESULT(res);

                /* Update sample rate of input of pre-effect */
                pcm_mode.nPortIndex = EFFECT_PORT_INPUT;
                RETURN_IF_FAIL(il_tool_GetParameter(graph_p->app.pre_effect_handle[port_index-1],
                    OMX_IndexParamAudioPcm, &pcm_mode));
                pcm_mode.nSamplingRate = input_sample_rate;
                RETURN_IF_FAIL(il_tool_SetParameter(graph_p->app.pre_effect_handle[port_index-1],
                    OMX_IndexParamAudioPcm, &pcm_mode));

                /* Get number of channels of the pre effect output */
                pcm_mode.nPortIndex = 0;
                RETURN_IF_FAIL(il_tool_GetParameter(graph_p->common.io_handle,
                    OMX_IndexParamAudioPcm, &pcm_mode));
                *output_channels_p = pcm_mode.nChannels;
            } else {
                ALOG_INFO("No pre-effect available for device %s\n",name);
            }
        }
    }
cleanup:
    return res;
}

ste_adm_res_t dev_omx_setup_graph(dev_omx_graph_t* graph_p,
                                      const char* name,
                                      int         is_vc,
                                      struct dev_params* dev_params,
                                      int* logical_handle_p)
{
    SRV_DBG_ASSERT_IS_WORKER_THREAD;

    unsigned int    *ref_cnt_p      = NULL;
    OMX_U32         port_index;

    ALOG_INFO("Enter dev_omx_setup_graph - "
        "device=%s, samplerate=%d, format=%X, alloc_buffers=%d, is_vc=%d, bufsz=%d\n",
        name, dev_params->samplerate, dev_params->format, dev_params->alloc_buffers, is_vc, dev_params->bufsz);

    if (dev_params->format >= STE_ADM_FORMAT_FIRST_CODED ||
        (graph_p->common.io_handle && !graph_p->common.mixer_splitter_handle))
    {
        // io_handle && !common.mixer_splitter_handle --> direct sink connection (e.g. codec HDMI)
        return dev_omx_setup_coded_sink(graph_p, name, dev_params, logical_handle_p);
    }

    ref_cnt_p = is_vc ? (&graph_p->voice.ref_cnt) : (&graph_p->app.ref_cnt);

    if (*ref_cnt_p == 0) {
        /* Read effect from database and set params and config for them */
        RETURN_IF_FAIL(adm_setup_effects(name, graph_p, is_vc, dev_params, 0));

        /* Get port index to be used in the app/voice chain */
        RETURN_IF_FAIL(adm_omx_graph_get_port_index(graph_p, is_vc, &port_index));

        unsigned int external_channels = (unsigned int) dev_params->format;
        if (external_channels == 0) {
            ALOG_INFO("dev_omx_setup_graph: num_channels == 0, defaulting to %d\n", graph_p->common.hw_num_channels);
            external_channels = (unsigned int) graph_p->common.hw_num_channels;
        }

        /* Setup the pre effect effect (if applicable) */
        unsigned int internal_channels;
        RETURN_IF_FAIL(adm_setup_pre_effect(graph_p, is_vc, name, port_index, dev_params->samplerate, external_channels, &internal_channels));

        /* Initialize PCM mode settings for splitter etc */
        RETURN_IF_FAIL(dev_omx_init_pcm_mode(graph_p, is_vc, name,
            dev_params->samplerate, internal_channels, dev_params->alloc_buffers, 0));

        /* Connect the OMX components, i.e. setup the OMX graph */
        RETURN_IF_FAIL(dev_omx_connect_components(graph_p, is_vc, port_index, 0));

        if (is_vc) {
            *logical_handle_p = 0;
        } else {
            *logical_handle_p = (int) port_index;
            ADM_ASSERT(port_index >= 1 && port_index <= 8);
        }

        /* Request transition to OMX_StateIdle for all components */
        RETURN_IF_FAIL(dev_omx_state_transition_request(
            graph_p, is_vc, port_index, OMX_StateIdle, 1 /* starting */, 0));

        if (dev_params->alloc_buffers) {
            /* Initialize buffers in the graph */
            RETURN_IF_FAIL(dev_omx_init_buffers(graph_p, is_vc, port_index,
                (unsigned int) dev_params->num_bufs, (unsigned int) dev_params->bufsz,
                dev_params->shm_fd));
        }
        /* Wait until transitioned to OMX_StateIdle... */
        RETURN_IF_FAIL(dev_omx_state_transition_wait(
            graph_p, is_vc, port_index, OMX_StateIdle, 1 /* starting */, 0));

        /* Request transition to OMX_StateExecuting for all components */
        RETURN_IF_FAIL(dev_omx_state_transition_request(
            graph_p, is_vc, port_index, OMX_StateExecuting, 1 /* starting */, 0));

        /* Wait until transitioned to OMX state Executing... */
        RETURN_IF_FAIL(dev_omx_state_transition_wait(
            graph_p, is_vc, port_index, OMX_StateExecuting, 1 /* starting */, 0));

        /* Enable needed ports in the Common Mixer/Splitter */
        RETURN_IF_FAIL(dev_omx_connect_common_mixer_splitter_to_app_voice(graph_p, is_vc));

    } else { /* ref_cnt > 0 */
        /* Check chain type. More than one voice chain reference is not valid */
        if (is_vc) {
            ALOG_ERR("The reference counter for voice chain is already %u\n",
                *ref_cnt_p);
            return STE_ADM_RES_ALL_IN_USE;
        }

        unsigned int external_channels = (unsigned int) dev_params->format;
        if (external_channels == 0) {
            ALOG_INFO("dev_omx_setup_graph: num_channels == 0, defaulting to %d\n", graph_p->common.hw_num_channels);
            external_channels = (unsigned int) graph_p->common.hw_num_channels;
        }

        /* Get port index to be used in the app chain handle */
        RETURN_IF_FAIL(adm_omx_graph_get_port_index(graph_p, is_vc, &port_index));
        *logical_handle_p = (int) port_index;

        /* Setup the pre effect effect, if needed */
        unsigned int internal_channels;
        RETURN_IF_FAIL(adm_setup_pre_effect(graph_p, is_vc, name, port_index, dev_params->samplerate, external_channels, &internal_channels));

        il_comp_t app_mixer_splitter = graph_p->app.mixer_splitter_handle;

        /* Set PCM settings for port in app mixer/splitter */
        OMX_AUDIO_PARAM_PCMMODETYPE pcm_mode;
        IL_TOOL_INIT_CONFIG_STRUCT(pcm_mode);

        pcm_mode.nPortIndex = port_index;
        RETURN_IF_FAIL(il_tool_GetParameter(app_mixer_splitter,
            OMX_IndexParamAudioPcm, &pcm_mode));

        pcm_mode.nChannels      = (OMX_U32)internal_channels;
        pcm_mode.nSamplingRate  = (OMX_U32)dev_params->samplerate;
        if (pcm_mode.nChannels == 1) {
            pcm_mode.eChannelMapping[0] = OMX_AUDIO_ChannelCF;
        } else if (pcm_mode.nChannels == 2) {
            pcm_mode.eChannelMapping[0] = OMX_AUDIO_ChannelLF;
            pcm_mode.eChannelMapping[1] = OMX_AUDIO_ChannelRF;
        } else if (pcm_mode.nChannels == 6) {
            pcm_mode.eChannelMapping[0] = OMX_AUDIO_ChannelLF;
            pcm_mode.eChannelMapping[1] = OMX_AUDIO_ChannelRF;
            pcm_mode.eChannelMapping[2] = OMX_AUDIO_ChannelLFE;
            pcm_mode.eChannelMapping[3] = OMX_AUDIO_ChannelCF;
            pcm_mode.eChannelMapping[4] = OMX_AUDIO_ChannelLS;
            pcm_mode.eChannelMapping[5] = OMX_AUDIO_ChannelRS;
        }
        RETURN_IF_FAIL(il_tool_SetParameter(app_mixer_splitter,
                                OMX_IndexParamAudioPcm, &pcm_mode));


        if (adm_omx_is_pre_effect(graph_p, port_index)) {
            il_comp_t  buf_comp;
            OMX_U32    buf_port;
            adm_omx_get_buffer_comp(graph_p, port_index, &buf_comp, &buf_port);


            /* Disable the output port index in pre effect */
            RETURN_IF_FAIL(il_tool_SendCommand(
                buf_comp, OMX_CommandPortDisable, 1, 0));

            /* Wait for the port to be disabled */
            ALOG_INFO("Wait for event semaphore Down\n");
            ADM_SEM_WAIT_WITH_PRINT("Port Disable, pre effect");

            /* Request transition to OMX_StateIdle */
            RETURN_IF_FAIL(dev_omx_state_transition_request(
                graph_p, is_vc, port_index, OMX_StateIdle, 1, 0));

            if (dev_params->alloc_buffers) {
                /* Initialize buffers in the graph */
                RETURN_IF_FAIL(dev_omx_init_buffers(graph_p, is_vc,
                    port_index, dev_params->num_bufs, dev_params->bufsz,
                    dev_params->shm_fd));
            }
            /* Wait until transitioned to OMX_StateIdle... */
            RETURN_IF_FAIL(dev_omx_state_transition_wait(
                graph_p, is_vc, port_index, OMX_StateIdle, 1, 0));

            /* Request transition to OMX_StateExecuting for all components */
            RETURN_IF_FAIL(dev_omx_state_transition_request(
                graph_p, is_vc, port_index, OMX_StateExecuting, 1, 0));

            /* Wait until transitioned to OMX state Executing... */
            RETURN_IF_FAIL(dev_omx_state_transition_wait(
                graph_p, is_vc, port_index, OMX_StateExecuting, 1, 0));

            RETURN_IF_FAIL(adm_omx_graph_connect_pre_effect_to_app(
                graph_p, port_index));

            if (dev_params->alloc_buffers) {
                /* Enable the input port index in the app mixer */
                RETURN_IF_FAIL(il_tool_SendCommand(
                    app_mixer_splitter, OMX_CommandPortEnable, port_index, 0));

                /* Enable the output port index in pre effect */
                RETURN_IF_FAIL(il_tool_SendCommand(
                    buf_comp, OMX_CommandPortEnable, 1, 0));

                /* Wait for pre effect output port to be enabled */
                ALOG_INFO("Wait for event semaphore Down\n");
                ADM_SEM_WAIT_WITH_PRINT("Wait for pre effect output port to be enabled");
            }
        } else { /* No pre effect */

            if (!is_vc)
            {
                dev_omx_graph_init_pcm_mode_app_mixer_splitter(
                graph_p, name, graph_p->app.mixer_splitter_handle,
                port_index, dev_params->samplerate, external_channels);
            }

            if (dev_params->alloc_buffers) {
                /* Enable the port index in the app mixer/splitter */
                RETURN_IF_FAIL(il_tool_SendCommand(
                    app_mixer_splitter, OMX_CommandPortEnable, port_index, 0));


                /* Initialize buffers (PortEnable must have been requested) */
                RETURN_IF_FAIL(dev_omx_init_buffers(graph_p, is_vc, port_index,
                    dev_params->num_bufs, dev_params->bufsz, dev_params->shm_fd));

                /* Wait for the port to be enabled */
                ALOG_INFO("Wait for event semaphore Down\n");
                ADM_SEM_WAIT_WITH_PRINT("Wait for the port to be enabled");
            }
        }
    }

    /* Increase reference counter to indicate successful creation */
    (*ref_cnt_p)++;
    ALOG_INFO_VERBOSE("Leave dev_omx_setup_graph - "
        "port_index=%u, ref_cnt=%u %s\n",
        port_index, ref_cnt_p ? *ref_cnt_p : 0, ref_cnt_p ? "" : "(NULL)");

    return STE_ADM_RES_OK;
}

ste_adm_res_t dev_omx_close_graph(dev_omx_graph_t* graph_p,
                                      int logical_handle,
                                      int dealloc_buffers,
                                      int* last_device_closed)
{
    SRV_DBG_ASSERT_IS_WORKER_THREAD;

    unsigned int                nbr_of_connected_graphs = 0;
    unsigned int                *ref_cnt_p              = NULL;
    dev_omx_comp_chain_t    comp_chains;
    il_comp_t                   io_comp;
    il_comp_t                   mixer_splitter_comp;
    il_comp_t                   app_mixer_splitter_comp;
    il_comp_t                   buf_comp;
    OMX_U32                     buf_port;
    unsigned int                i;

    int is_vc = (logical_handle == 0);


    ALOG_INFO("Enter dev_omx_close_graph - "
        "logical_handle=%d  dealloc_buffers=%d\n",
        logical_handle, dealloc_buffers);

    if (graph_p->common.io_handle && !graph_p->common.mixer_splitter_handle) {
        *last_device_closed = 1;
        return dev_omx_close_coded_sink(graph_p);
    }


    adm_omx_util_get_ref_cnt(graph_p, is_vc, &ref_cnt_p);
    ADM_ASSERT(*ref_cnt_p != 0);

    if (!is_vc) {
        OMX_U32 port_index = (OMX_U32) (logical_handle);
        adm_omx_get_buffer_comp(graph_p, port_index, &buf_comp, &buf_port);
    }

    if (!graph_p->is_input && graph_p->app.ref_cnt + graph_p->voice.ref_cnt == 1) {
        ALOG_INFO("Closing last device. Request audiocodec to ramp down audio\n");

        // Tell audiocodec to ramp down
        OMX_AUDIO_CONFIG_MUTETYPE ramp_down_cfg;
        IL_TOOL_INIT_CONFIG_STRUCT(ramp_down_cfg);
        ramp_down_cfg.bMute = OMX_TRUE;
        ramp_down_cfg.nPortIndex = 0;

        // Wait for event from audiocodec, indicating that ramp-down is complete
        ste_adm_res_t res;
        res = il_tool_set_config_and_wait_config_event(graph_p->common.io_handle,
                                                       OMX_IndexConfigAudioMute,
                                                       &ramp_down_cfg,
                                                       OMX_IndexConfigAudioMute);
        if (res != STE_ADM_RES_OK) {
            ALOG_INFO("OMX_IndexConfigAudioMute not supported");
        }
    }

    if (*ref_cnt_p == 1) {
        dev_omx_util_get_components(graph_p, is_vc, &comp_chains);
        OMX_U32 port_index = (OMX_U32) (logical_handle);

        /* If both App Chain and Voice Chain are created, the graph to be
            closed has to be disconnected from the Common chain.
            Also do this generally, since the supplier might have been changed
            to allow buffer sharing ARM<-->DSP, and then the Exec->Idle
            commands will be sent in incorrect order for this connection.
            Problem avoided if disconnecting before state transitions. */
        RETURN_IF_FAIL(dev_omx_graph_disconnect_mixer_splitter_from_app_voice(
                graph_p, is_vc));

        RETURN_IF_FAIL(dev_omx_remove_mixer_effects(graph_p, is_vc?STE_ADM_VOICE_CHAIN:STE_ADM_APPLICATION_CHAIN));


        if (dealloc_buffers) {
           RETURN_IF_FAIL(il_tool_change_port_state(buf_comp, buf_port, 0, 1));
        }
        /* Request transition to OMX_StateIdle for all components */
        RETURN_IF_FAIL(dev_omx_state_transition_request(
            graph_p, is_vc, port_index, OMX_StateIdle, 0 /* stopping */, 0));

        /* Wait until transitioned to OMX_StateIdle... */
        RETURN_IF_FAIL(dev_omx_state_transition_wait(
            graph_p, is_vc, port_index, OMX_StateIdle, 0 /* stopping */, 0));

        /* Request transition to OMX_StateLoaded for all components */
        RETURN_IF_FAIL(dev_omx_state_transition_request(
            graph_p, is_vc, port_index, OMX_StateLoaded, 0 /* stopping */, 0));

        /* Wait until transitioned to OMX_StateLoaded... */
        RETURN_IF_FAIL(dev_omx_state_transition_wait(
            graph_p, is_vc, port_index, OMX_StateLoaded, 0 /* stopping */, 0));

        ALOG_INFO_VERBOSE("Free the OMX handles of the app/voice chain\n");
        /* Free the OMX handles of the App/Voice chain */
        dev_omx_util_free_components(graph_p, is_vc);

        /* Free the OMX handle for the App chain Mixer/Splitter */
        if (!is_vc) {
            ALOG_INFO_VERBOSE("Free the OMX handle for the app chain "
                "mixer/splitter, 0x%08X\n",
                (unsigned int)il_tool_raw_handle(graph_p->app.mixer_splitter_handle));

            if (dev_omx_util_get_omx_state(graph_p->app.mixer_splitter_handle) != OMX_StateLoaded) {
                ALOG_ERR("app_mixer_splitter_comp not in StateLoaded! "
                    "Not calling FreeHandle\n");
            } else {
                il_tool_destroy_component(&(graph_p->app.mixer_splitter_handle));
            }
        }

        dev_omx_util_get_common_state(graph_p, &nbr_of_connected_graphs);

        if (nbr_of_connected_graphs == 1) {
            /* Free OMX handles of the Common chain */
            ALOG_INFO_VERBOSE("Free the OMX handles of the common chain\n");
            for (i = 0; i < comp_chains.nbr_of_common_comp; i++) {
                if (dev_omx_util_get_omx_state(comp_chains.common_comp_handles_p[i]) !=
                        OMX_StateLoaded)
                {
                    ALOG_ERR("comp_chains.common_comp_handles_p[i] "
                        "%X not in StateLoaded, not calling free\n",
                        il_tool_raw_handle(comp_chains.common_comp_handles_p[i]));
                } else {
                    il_tool_destroy_component(&comp_chains.common_comp_handles_p[i]);
                }
            }

            /* Free the OMX handles of the Sink/Source */
            io_comp = graph_p->common.io_handle;

            ALOG_INFO_VERBOSE("Free the OMX handles of the IO component "
                "0x%08X\n", (unsigned int)il_tool_raw_handle(io_comp));

            if (dev_omx_util_get_omx_state(io_comp) != OMX_StateLoaded) {
                ALOG_ERR("io_comp not in StateLoaded, "
                    "not calling FreeHandle\n");
            } else {
                il_tool_destroy_component(&io_comp);
            }

            /* Free the OMX handle of the Common Mixer/Splitter */
            mixer_splitter_comp = graph_p->common.mixer_splitter_handle;
            ALOG_INFO_VERBOSE(
                "Free the OMX handle of the common mixer/splitter 0x%08X\n",
                (unsigned int)il_tool_raw_handle(mixer_splitter_comp));

            if (dev_omx_util_get_omx_state(mixer_splitter_comp) != OMX_StateLoaded) {
                ALOG_ERR("mixer_splitter_comp not in StateLoaded, "
                    "not calling FreeHandle\n");
            } else {
                il_tool_destroy_component(&mixer_splitter_comp);
                ALOG_INFO("All OMX handles freed successfully\n");
            }
        }
    } else { /* ref_cnt > 1 */
        if (!is_vc) {
            OMX_U32 port_index = (OMX_U32) (logical_handle);
            app_mixer_splitter_comp = graph_p->app.mixer_splitter_handle;
            if (adm_omx_is_pre_effect(graph_p, port_index)) {
                /* Disable the ADM data flow port. if not dealloc_buffers is set,
                *  the port has been disabled before this call */
                if (dealloc_buffers) {
                    RETURN_IF_FAIL(il_tool_change_port_state(buf_comp, buf_port, 0, 1));
                }

                /* Request transition to OMX_StateIdle */
                RETURN_IF_FAIL(dev_omx_state_transition_request(
                    graph_p, is_vc, port_index, OMX_StateIdle, 0, 0));

                /* Wait until transitioned to OMX_StateIdle... */
                RETURN_IF_FAIL(dev_omx_state_transition_wait(
                    graph_p, is_vc, port_index, OMX_StateIdle, 0, 0));

                /* Disable the output port index in pre effect */
                RETURN_IF_FAIL(il_tool_SendCommand(buf_comp,
                    OMX_CommandPortDisable, EFFECT_PORT_OUTPUT, 0));

                /* Disable the input port index of the app mixer */
                RETURN_IF_FAIL(il_tool_SendCommand(app_mixer_splitter_comp,
                    OMX_CommandPortDisable, port_index, 0));

                /* Wait until the pre effect output port has been disabled */
                ALOG_INFO_VERBOSE("Wait for event semaphore Down");
                ADM_SEM_WAIT_WITH_PRINT("Wait until the pre effect output port has been disabled");

                /* Wait until the app mixer input port has been disabled */
                ALOG_INFO_VERBOSE("Wait for event semaphore Down");
                ADM_SEM_WAIT_WITH_PRINT("Wait until the app mixer input port has been disabled");

                /* Request transition to OMX_StateLoaded */
                RETURN_IF_FAIL(dev_omx_state_transition_request(
                    graph_p, is_vc, port_index, OMX_StateLoaded, 0, 0));

                /* Wait until transitioned to OMX_StateLoaded... */
                RETURN_IF_FAIL(dev_omx_state_transition_wait(
                    graph_p, is_vc, port_index, OMX_StateLoaded, 0 /* stopping */, 0));

            } else { /* No pre effect */
                /* Disable the port of the app mixer/splitter, and release buffers */
                if (dealloc_buffers) {
                    RETURN_IF_FAIL(il_tool_change_port_state(
                        graph_p->app.mixer_splitter_handle, port_index, 0, 1));
                }
            }
        }
    }

    if (!is_vc) {
        OMX_U32 port_index = (OMX_U32) (logical_handle);

        /* Free the port index which is no longer used.
            Available mixer input ports and splitter output ports start at 1,
            therefore decrement 1 */
        graph_p->app.port_index[port_index-1] = 0;
        /* Free the pre effect if available */
        if (adm_omx_is_pre_effect(graph_p, port_index)) {
            il_comp_t pre_effect_comp;
            ADM_ASSERT((port_index-1) < ADM_OMX_MAX_NBR_OF_PORTS);
            pre_effect_comp = graph_p->app.pre_effect_handle[port_index-1];

            il_tool_destroy_component(&pre_effect_comp);
        }
    }

    /* Decrease reference counter to indicate successful removal */
    (*ref_cnt_p)--;
    ALOG_INFO("Graph closed successfully! ref_cnt = %d\n", *ref_cnt_p);

    if (graph_p->app.ref_cnt == 0 && graph_p->voice.ref_cnt == 0) {
        *last_device_closed = 1;
    } else {
        *last_device_closed = 0;
    }

    return STE_ADM_RES_OK;
}

ste_adm_res_t dev_omx_reconfigure_effects(dev_omx_graph_t* graph_p, const char* name, const char* name2, int vc_disconnected)
{
    SRV_DBG_ASSERT_IS_WORKER_THREAD;
    ALOG_INFO("Enter dev_omx_reconfigure_effects, %s\n", name);
    ADM_ASSERT(graph_p);

    /* Disconnect the current effect chains and free all effect
       components in the effect chains.
       Setup and connect the effect chains again using new cfg from db.
       The mixers/splitters and sink/source are however not freed, and
       only disconnected from the effect chains */
    RETURN_IF_FAIL(dev_omx_disconnect_and_destroy_effect_chain(graph_p, name, name2, STE_ADM_APPLICATION_CHAIN, vc_disconnected));
    RETURN_IF_FAIL(dev_omx_disconnect_and_destroy_effect_chain(graph_p, name, name2, STE_ADM_VOICE_CHAIN, vc_disconnected));
    RETURN_IF_FAIL(dev_omx_disconnect_and_destroy_effect_chain(graph_p, name, name2, STE_ADM_COMMON_CHAIN, vc_disconnected));
    RETURN_IF_FAIL(dev_omx_connect_effect_chain(graph_p, name, name2, STE_ADM_COMMON_CHAIN, vc_disconnected));
    RETURN_IF_FAIL(dev_omx_connect_effect_chain(graph_p, name, name2, STE_ADM_VOICE_CHAIN, vc_disconnected));
    RETURN_IF_FAIL(dev_omx_connect_effect_chain(graph_p, name, name2, STE_ADM_APPLICATION_CHAIN, vc_disconnected));

    ALOG_INFO("Leave dev_omx_reconfigure_effects, %s\n", name);
    return STE_ADM_RES_OK;
}


ste_adm_res_t dev_omx_rescan_effects(
        dev_omx_graph_t* graph_p,
        ste_adm_effect_chain_type_t chain,
        const char* name,
        const char* name2)
{
    ste_adm_res_t res = STE_ADM_RES_OK;
    ALOG_INFO("Enter dev_omx_rescan_effects, %s, %s, %s\n", name, name2, chaintype2str(chain));

    SRV_DBG_ASSERT_IS_WORKER_THREAD;
    ADM_ASSERT(graph_p);

    il_comp_t *comp_handles_p = NULL;
    unsigned int nbr_of_comp;;

    adm_db_comp_iter_t compit;
    int compit_alloced = 0;
    const char* il_name = NULL;
    adm_db_cfg_iter_t cfgit;
    memset(&cfgit, 0, sizeof(cfgit));
    int cfgit_alloced = 0;

    switch (chain) {
    case STE_ADM_COMMON_CHAIN:
        comp_handles_p = graph_p->common.comp_handles;
        nbr_of_comp = graph_p->common.nbr_of_comp;

        res = adm_db_comp_iter_create(name, NULL,
                    chain, 0, &compit);
        break;
    case STE_ADM_VOICE_CHAIN:
        if (graph_p->voice.ref_cnt > 0 && name2[0]) {
            comp_handles_p = graph_p->voice.comp_handles;
            nbr_of_comp = graph_p->voice.nbr_of_comp;

            res = adm_db_comp_iter_create(name, name2,
                chain, csapi_get_samplerate(), &compit);
        }
        else {
            ALOG_INFO("Voice chain not open");
            goto cleanup;
        }
        break;
    case STE_ADM_APPLICATION_CHAIN:
        if (graph_p->app.ref_cnt > 0) {
            comp_handles_p = graph_p->app.comp_handles;
            nbr_of_comp = graph_p->app.nbr_of_comp;

            res = adm_db_comp_iter_create(name, NULL,
                    chain, 0, &compit);
        }
        else {
            ALOG_INFO("App chain not open");
            goto cleanup;
        }
        break;
    default:
        ALOG_ERR("Unknown effect chain type");
        return STE_ADM_RES_INVALID_PARAMETER;
    }

    if (res != STE_ADM_RES_OK) {
        ALOG_ERR("adm_db_comp_iter_create failed for dev='%s'\n",name);
        goto cleanup;
    } else {
        compit_alloced = 1;
    }

    int mixer_effect_position = 0;
    int comp_position = -1;

    /* iterate over components */
    while ((comp_position + 1 < ADM_OMX_CHAIN_MAX_LENGTH) && STE_ADM_RES_OK ==
            (res = adm_db_comp_iter_get_next(&compit, &il_name, &cfgit))) {
        /* Variables for config iteration */
        cfgit_alloced = 1;
        ALOG_INFO_VERBOSE("Got a component\n");

        const char *mixer_effect = adaptions_get_mixer_internal_effect_name(il_name);
        if (!graph_p->use_mixer_effects || mixer_effect == NULL) {
            /* Handle normal effect component */

            if (mixer_effect_position > 0 && (chain == STE_ADM_APPLICATION_CHAIN || chain == STE_ADM_VOICE_CHAIN)) {
                ALOG_ERR("Non mixer effect found after mixer effect on input port.");
                res = STE_ADM_RES_IL_UNKNOWN_ERROR;
                goto cleanup;
            }

            comp_position++;

            if(comp_position >= (int)nbr_of_comp) {
                ALOG_ERR("DB does not match graph. Found %d comps but graph only contains %d.", comp_position + 1, nbr_of_comp);
                res = STE_ADM_RES_CHAIN_TOO_LONG;
                goto cleanup;
            }

            /* Configure component using the config iterator. Only SetConfig */
            if (STE_ADM_RES_OK != adm_setup_component(comp_handles_p[comp_position], &cfgit, 0, 1)) {
                ALOG_ERR("Failed to configure component");
                res = STE_ADM_RES_IL_UNKNOWN_ERROR;
                goto cleanup;
            }
        }
        else {
            /* Handle mixer effect */

            if (comp_position >= 0 && chain == STE_ADM_COMMON_CHAIN) {
                ALOG_ERR("mixer effect found after non mixer effect on output port.");
                res = STE_ADM_RES_IL_UNKNOWN_ERROR;
                goto cleanup;
            }

            mixer_effect_position++;

            /* Configure effect using the config iterator. Only SetConfig */
            if (STE_ADM_RES_OK != adm_setup_mixer_effect(graph_p, mixer_effect_position, chain, il_name, &cfgit, 0, 1, 0)) {
                ALOG_ERR("Failed to configure mixer effect");
                res = STE_ADM_RES_IL_UNKNOWN_ERROR;
                goto cleanup;
            }
        }

                /* Destroy cfg iterator */
        adm_db_cfg_iter_destroy(&cfgit);
        cfgit_alloced = 0;
    }
    if (STE_ADM_RES_OK == res && nbr_of_comp == ADM_OMX_CHAIN_MAX_LENGTH) {
        ALOG_ERR("Not enough space to store all components in chain");
        res = STE_ADM_RES_CHAIN_TOO_LONG;
    } else if (STE_ADM_RES_NO_MORE != res) {
        ALOG_ERR("Error iterating over components");
        res = STE_ADM_RES_UNKNOWN_DATABASE_ERROR;
        goto cleanup;
    } else {
        ALOG_INFO_VERBOSE("No more components\n");
        res = STE_ADM_RES_OK;
    }

cleanup:
    /* Destroy component iterator */
    if (cfgit_alloced) {
        adm_db_cfg_iter_destroy(&cfgit);
    }
    if (compit_alloced) {
        adm_db_comp_iter_destroy(&compit);
    }

    ALOG_INFO("Leave dev_omx_rescan_effects, %s\n", name);
    return res;
}

static ste_adm_res_t dev_omx_remove_mixer_effects(
                                    dev_omx_graph_t* graph_p,
                                    ste_adm_effect_chain_type_t chain)
{
    int port;

    ALOG_INFO("Enter dev_omx_remove_mixer_effects for chain %s", chaintype2str(chain));

    if (!graph_p->is_input && graph_p->use_mixer_effects) {
        switch (chain) {
        case STE_ADM_COMMON_CHAIN:
            port = MIXER_OUTPUT_PORT;
            graph_p->common.nbr_mixer_effects = 0;
            break;
        case STE_ADM_VOICE_CHAIN:
            port = MIXER_PORT_VOICE_INPUT;
            graph_p->voice.nbr_mixer_effects = 0;
            break;
        case STE_ADM_APPLICATION_CHAIN:
            port = MIXER_PORT_APP_INPUT;
            graph_p->app.nbr_mixer_effects = 0;
            break;
        default:
            ALOG_ERR("Unknown effect chain type");
            return STE_ADM_RES_INVALID_PARAMETER;
        }

        RETURN_IF_FAIL(il_tool_remove_mixer_effect(graph_p->common.mixer_splitter_handle, OMX_ALL, port));
        RETURN_IF_FAIL(il_tool_commit_mixer_effects(graph_p->common.mixer_splitter_handle, port));
    }

    ALOG_INFO("Leave dev_omx_remove_mixer_effects");

    return STE_ADM_RES_OK;
}

static ste_adm_res_t dev_omx_disconnect_and_destroy_effect_chain(
                                    dev_omx_graph_t* graph_p,
                                    const char* name,
                                    const char* name2,
                                    ste_adm_effect_chain_type_t type,
                                    int vc_disconnected)
{
    ste_adm_res_t res = STE_ADM_RES_OK;
    il_comp_t *comp_handles_p = NULL;
    unsigned int *nbr_of_comp_p = NULL;


    ALOG_INFO("Enter dev_omx_disconnect_and_destroy_effect_chain, %s, %s\n", name, chaintype2str(type));

    switch (type) {
    case STE_ADM_COMMON_CHAIN:
        comp_handles_p = graph_p->common.comp_handles;
        nbr_of_comp_p = &graph_p->common.nbr_of_comp;
        graph_disconnect_common_effects_from_io(graph_p);
        if (graph_p->common.nbr_of_comp > 0) {
            graph_disconnect_common_effects_from_mix_split(graph_p);
        }
        break;
    case STE_ADM_VOICE_CHAIN:
        if (graph_p->voice.ref_cnt > 0 && name2[0]) {
            comp_handles_p = graph_p->voice.comp_handles;
            nbr_of_comp_p = &graph_p->voice.nbr_of_comp;
            if (!vc_disconnected) {
                graph_disconnect_voice_effects_from_mix_split(graph_p);
            }
        }
        break;
    case STE_ADM_APPLICATION_CHAIN:
        if (graph_p->app.ref_cnt > 0) {
            comp_handles_p = graph_p->app.comp_handles;
            nbr_of_comp_p = &graph_p->app.nbr_of_comp;
            graph_disconnect_app_effects_from_app_mix_split(graph_p);
            if (graph_p->app.nbr_of_comp > 0) {
                graph_disconnect_app_effects_from_common_mix_split(graph_p);
            }
        }
        break;
    default:
        ALOG_ERR("Unknown effect chain type");
        return STE_ADM_RES_INVALID_PARAMETER;
    }

    if (nbr_of_comp_p != NULL) {
        if (*nbr_of_comp_p > 0) {
            /* Transition to OMX_StateIdle */
            /* transit_forward parameter according to transit strategy in
             * dev_omx_state_transition_request() */
            RETURN_IF_FAIL(dev_omx_graph_state_transition_request(comp_handles_p, *nbr_of_comp_p, OMX_StateIdle, 1));
            RETURN_IF_FAIL(dev_omx_graph_state_transition_wait(*nbr_of_comp_p));
            RETURN_IF_FAIL(dev_omx_graph_state_transition_check(comp_handles_p, *nbr_of_comp_p, OMX_StateIdle));
            /* Transition to OMX_StateLoaded */
            RETURN_IF_FAIL(dev_omx_graph_state_transition_request(comp_handles_p, *nbr_of_comp_p, OMX_StateLoaded, 0));
            RETURN_IF_FAIL(dev_omx_graph_state_transition_wait(*nbr_of_comp_p));
            RETURN_IF_FAIL(dev_omx_graph_state_transition_check(comp_handles_p, *nbr_of_comp_p, OMX_StateLoaded));
            /* Destroy all components in effect chain */
            int i;
            for (i = 0; i < (int)*nbr_of_comp_p; i++) {
                il_tool_destroy_component(&comp_handles_p[i]);
            }
            *nbr_of_comp_p = 0;
        }
    }

    /* Remove mixer effects */
    RETURN_IF_FAIL(dev_omx_remove_mixer_effects(graph_p, type));

    ALOG_INFO("Leave dev_omx_disconnect_and_destroy_effect_chain, %s, %s\n", name, chaintype2str(type));
    return res;
}

static ste_adm_res_t dev_omx_connect_effect_chain(
                                    dev_omx_graph_t* graph_p,
                                    const char* name,
                                    const char* name2,
                                    ste_adm_effect_chain_type_t type,
                                    int vc_disconnected)
{
    ste_adm_res_t res = STE_ADM_RES_OK;
    ALOG_INFO("Enter dev_omx_connect_effect_chain, %s, %s\n", name, chaintype2str(type));

    il_comp_t *comp_handles_p = NULL;
    unsigned int *nbr_of_comp_p = NULL;

    struct dev_params param_dev;
    memset(&param_dev, 0, sizeof(param_dev));

    switch (type) {
    case STE_ADM_COMMON_CHAIN:
        comp_handles_p = graph_p->common.comp_handles;
        nbr_of_comp_p = &graph_p->common.nbr_of_comp;
        /* Read common effect chain from db and configure components */
        RETURN_IF_FAIL(adm_setup_chain(graph_p, name, &param_dev, type));
        /* Connect the effects in the common effect chain */
        RETURN_IF_FAIL(dev_omx_graph_connect_effects(
            *nbr_of_comp_p, comp_handles_p));
        break;
    case STE_ADM_VOICE_CHAIN:
        if (graph_p->voice.ref_cnt > 0 && name2[0]) {
            comp_handles_p = graph_p->voice.comp_handles;
            nbr_of_comp_p = &graph_p->voice.nbr_of_comp;
            param_dev.samplerate = csapi_get_samplerate();
            if (graph_p->is_input) {
                param_dev.name_in = name;
                param_dev.name_out = name2;
            } else {
                param_dev.name_in = name2;
                param_dev.name_out = name;
            }

            /* Read vc effect chain from db and configure components */
            RETURN_IF_FAIL(adm_setup_chain(graph_p, name, &param_dev, type));
            /* Connect the effects in the vc effect chain */
            RETURN_IF_FAIL(dev_omx_graph_connect_effects(
                *nbr_of_comp_p, comp_handles_p));
        }
        break;
    case STE_ADM_APPLICATION_CHAIN:
        if (graph_p->app.ref_cnt > 0) {
            comp_handles_p = graph_p->app.comp_handles;
            nbr_of_comp_p = &graph_p->app.nbr_of_comp;
            /* Read app effect chain from db and configure components */
            RETURN_IF_FAIL(adm_setup_chain(graph_p, name, &param_dev, type));
            /* Connect the effects in the app effect chain */
            RETURN_IF_FAIL(dev_omx_graph_connect_effects(
                *nbr_of_comp_p, comp_handles_p));
        }
        break;
    default:
        ALOG_ERR("Unknown effect chain type");
        return STE_ADM_RES_INVALID_PARAMETER;
    }

    if (nbr_of_comp_p != NULL) {
        if (*nbr_of_comp_p > 0) {
            /* Disable end ports of effect chain */
            IL_TOOL_DISABLE_PORT(comp_handles_p[0], EFFECT_PORT_INPUT);
            IL_TOOL_DISABLE_PORT(comp_handles_p[*nbr_of_comp_p-1], EFFECT_PORT_OUTPUT);
            /* Transition to OMX_StateIdle */
            /* transit_forward parameter according to transit strategy in
             * dev_omx_state_transition_request() */
            RETURN_IF_FAIL(dev_omx_graph_state_transition_request(comp_handles_p, *nbr_of_comp_p, OMX_StateIdle, 0));
            RETURN_IF_FAIL(dev_omx_graph_state_transition_wait(*nbr_of_comp_p));
            RETURN_IF_FAIL(dev_omx_graph_state_transition_check(comp_handles_p, *nbr_of_comp_p, OMX_StateIdle));
            /* Transition to OMX_StateExecuting */
            RETURN_IF_FAIL(dev_omx_graph_state_transition_request(comp_handles_p, *nbr_of_comp_p, OMX_StateExecuting, 0));
            RETURN_IF_FAIL(dev_omx_graph_state_transition_wait(*nbr_of_comp_p));
            RETURN_IF_FAIL(dev_omx_graph_state_transition_check(comp_handles_p, *nbr_of_comp_p, OMX_StateExecuting));
        }
    }

    switch (type) {
    case STE_ADM_COMMON_CHAIN:
        /* Connect the common effect chain to source/sink.
           If no effects, the common mixer is connected to io */
        RETURN_IF_FAIL(graph_connect_common_effects_to_io(graph_p));
        if (*nbr_of_comp_p > 0) {
            /* Connect the common effect chain to common mixer/splitter.
               If no effects common mixer is already connected to io */
            RETURN_IF_FAIL(graph_connect_common_effects_to_mix_split(graph_p));
        }
        break;
    case STE_ADM_VOICE_CHAIN:
        if (graph_p->voice.ref_cnt > 0) {
            /* Connect the voice effect chain to common mixer/splitter.
               If no effect, nothing is connected */
            if (!vc_disconnected) {
                RETURN_IF_FAIL(graph_connect_voice_effects_to_mix_split(graph_p));
            }
        }
        break;
    case STE_ADM_APPLICATION_CHAIN:
        if (graph_p->app.ref_cnt > 0) {
            /* Connect the app effect chain to common mixer/splitter.
               If no effects, the common mixer is connected to app mixer/splitter */
            RETURN_IF_FAIL(graph_connect_app_effects_to_common_mix_split(graph_p));
            if (*nbr_of_comp_p > 0) {
                RETURN_IF_FAIL(graph_connect_app_effects_to_app_mix_split(graph_p));
            }
        }
        break;
    default:
        ALOG_ERR("Unknown effect chain type");
        return STE_ADM_RES_INVALID_PARAMETER;
    }

cleanup:
    ALOG_INFO("Leave dev_omx_connect_effect_chain, %s, %s\n", name, chaintype2str(type));
    return res;

}

/**
 *  This is only used when enabling/disabling TTY mode.
 */
static ste_adm_res_t dev_omx_close_common_graph(dev_omx_graph_t* graph_p)
{
    SRV_DBG_ASSERT_IS_WORKER_THREAD;

    il_comp_t               mixer_splitter_comp;


    ALOG_INFO("Enter dev_omx_close_common_graph\n");

    /* Request transition to OMX_StateIdle for all components */
    RETURN_IF_FAIL(dev_omx_state_transition_request(
        graph_p, 0, 0, OMX_StateIdle, 0 /* stopping */, 1 /* Only common*/));

    /* Wait until transitioned to OMX_StateIdle... */
    RETURN_IF_FAIL(dev_omx_state_transition_wait(
        graph_p, 0, 0, OMX_StateIdle, 0 /* stopping */, 1 /* Only common*/));

    /* Request transition to OMX_StateLoaded for all components */
    RETURN_IF_FAIL(dev_omx_state_transition_request(
        graph_p, 0, 0, OMX_StateLoaded, 0 /* stopping */, 1 /* Only common*/));

    /* Wait until transitioned to OMX_StateLoaded... */
    RETURN_IF_FAIL(dev_omx_state_transition_wait(
        graph_p, 0, 0, OMX_StateLoaded, 0 /* stopping */, 1 /* Only common*/));

    ALOG_INFO_VERBOSE("Free the OMX handles of the app/voice chain\n");


    /* Free OMX handles of the Common chain */
    ALOG_INFO_VERBOSE("Free the OMX handles of the common chain\n");
    unsigned int i;
    for (i = 0; i < graph_p->common.nbr_of_comp; i++) {
        if (dev_omx_util_get_omx_state(graph_p->common.comp_handles[i]) !=
                OMX_StateLoaded)
        {
            ALOG_ERR("graph_p->common.comp_handles[i] "
                "%X not in StateLoaded, not calling free\n",
                il_tool_raw_handle(graph_p->common.comp_handles[i]));
        } else {
            il_tool_destroy_component(&(graph_p->common.comp_handles[i]));
        }
    }

    graph_p->common.nbr_of_comp = 0;

    /* Free the OMX handles of the Sink/Source */
    il_comp_t io_comp = graph_p->common.io_handle;

    ALOG_INFO_VERBOSE("Free the OMX handles of the IO component "
        "0x%08X\n", (unsigned int)il_tool_raw_handle(io_comp));

    if (dev_omx_util_get_omx_state(io_comp) != OMX_StateLoaded) {
        ALOG_ERR("io_comp not in StateLoaded, "
            "not calling FreeHandle\n");
    } else {
        il_tool_destroy_component(&io_comp);
        graph_p->common.io_handle = NULL;
    }

    /* Free the OMX handle of the Common Mixer/Splitter */
    mixer_splitter_comp = graph_p->common.mixer_splitter_handle;
    ALOG_INFO_VERBOSE(
        "Free the OMX handle of the common mixer/splitter 0x%08X\n",
        (unsigned int)il_tool_raw_handle(mixer_splitter_comp));

    if (dev_omx_util_get_omx_state(mixer_splitter_comp) != OMX_StateLoaded) {
        ALOG_ERR("mixer_splitter_comp not in StateLoaded, "
            "not calling FreeHandle\n");
    } else {
        il_tool_destroy_component(&mixer_splitter_comp);
        ALOG_INFO("All OMX handles freed successfully\n");
        graph_p->common.mixer_splitter_handle = NULL;
    }

    return STE_ADM_RES_OK;
}

/**
 *  This is only used when enabling/disabling TTY mode.
 */
static ste_adm_res_t dev_omx_open_common_graph(dev_omx_graph_t* graph_p,
                                       const char* name,
                                       struct dev_params* dev_params)
{
    SRV_DBG_ASSERT_IS_WORKER_THREAD;

    unsigned int    channels = (unsigned int) dev_params->format;

    ALOG_INFO("Enter dev_omx_open_common_graph - "
        "device=%s, samplerate=%d, format=%d\n",
        name, dev_params->samplerate, dev_params->format);

    ADM_ASSERT(graph_p->common.io_handle == NULL);
    ADM_ASSERT(graph_p->common.mixer_splitter_handle == NULL);
    ADM_ASSERT(graph_p->common.nbr_of_comp == 0);

    /* Read effect from database and set params and config for them */
    RETURN_IF_FAIL(adm_setup_effects(name, graph_p, 0, dev_params, 1));

    /* Initialize PCM mode settings for splitter etc */
    RETURN_IF_FAIL(dev_omx_init_pcm_mode(graph_p, 0, name,
        dev_params->samplerate, channels, 0, 1));

    /* Connect the OMX components, i.e. setup the OMX graph */
    RETURN_IF_FAIL(dev_omx_connect_components(graph_p, 0, 0, 1));

    /* Request transition to OMX_StateIdle for all components */
    RETURN_IF_FAIL(dev_omx_state_transition_request(
        graph_p, 0, 0, OMX_StateIdle, 1 /* starting */, 1));

    /* Wait until transitioned to OMX_StateIdle... */
    RETURN_IF_FAIL(dev_omx_state_transition_wait(
        graph_p, 0, 0, OMX_StateIdle, 1 /* starting */, 1));

    /* Request transition to OMX_StateExecuting for all components */
    RETURN_IF_FAIL(dev_omx_state_transition_request(
        graph_p, 0, 0, OMX_StateExecuting, 1 /* starting */, 1));

    /* Wait until transitioned to OMX state Executing... */
    RETURN_IF_FAIL(dev_omx_state_transition_wait(
        graph_p, 0, 0, OMX_StateExecuting, 1 /* starting */, 1));

    /* Enable needed ports in the Common Mixer/Splitter */
    RETURN_IF_FAIL(dev_omx_connect_common_mixer_splitter_to_app_voice(graph_p, 0));


    ALOG_INFO_VERBOSE("Leave dev_omx_open_common_graph.\n");

    return STE_ADM_RES_OK;
}


void dev_omx_get_endpoint(dev_omx_graph_t*     graph_p,
                                       int             logical_handle,
                                       il_comp_t*      comp_p,
                                       OMX_U32*        port_p)
{
    int is_vc = (logical_handle == 0);

    if (!is_vc || (graph_p->common.io_handle && !graph_p->common.mixer_splitter_handle)) {
        // io_handle && !common.mixer_splitter_handle --> direct sink connection (e.g. codec HDMI)
        OMX_U32 port_index = (OMX_U32) (logical_handle);
        adm_omx_get_buffer_comp(graph_p, port_index, comp_p, port_p);
    } else {
        adm_omx_get_voice_handle(graph_p, comp_p, port_p);
    }
}

ste_adm_res_t dev_omx_get_endpoint_echoref(const dev_omx_graph_t* graph_p,
                                       il_comp_t*  comp_p,
                                       OMX_U32*    port_p)
{
    if (graph_p->is_input) {
        *comp_p = NULL;
        return STE_ADM_RES_OK;
    }

    OMX_PORT_PARAM_TYPE   param_port;
    IL_TOOL_INIT_CONFIG_STRUCT(param_port);

    RETURN_IF_FAIL(il_tool_GetParameter(graph_p->common.io_handle,
                OMX_IndexParamAudioInit,
                &param_port));

    ALOG_INFO("%d ports on IO handle\n", param_port.nPorts);
    if (param_port.nPorts <= 1) {
        *comp_p = NULL;
    } else {
        *comp_p = graph_p->common.io_handle;
        *port_p = 1; // port 0 is main, 1 should be feedback port
    }

    return STE_ADM_RES_OK;
}

il_comp_t dev_omx_get_component_at(dev_omx_graph_t* graph_p,
                                 ste_adm_effect_chain_type_t chain_type,
                                 const char* il_comp_name,
                                 unsigned int position)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    il_comp_t*    components_p;
    unsigned int  nbr_of_comp;

    switch(chain_type)
    {
    case STE_ADM_APPLICATION_CHAIN:
        nbr_of_comp  = graph_p->app.nbr_of_comp;
        components_p = graph_p->app.comp_handles;
        break;
    case STE_ADM_VOICE_CHAIN:
        nbr_of_comp  = graph_p->voice.nbr_of_comp;
        components_p = graph_p->voice.comp_handles;
        break;
    case STE_ADM_COMMON_CHAIN:
        nbr_of_comp  = graph_p->common.nbr_of_comp;
        components_p = graph_p->common.comp_handles;
        break;
    default:
        ALOG_ERR("dev_omx_get_component_at: Invalid chain type %d\n", chain_type);
        return 0;
    }

    if (position < nbr_of_comp)
    {
        char component_name[OMX_MAX_STRINGNAME_SIZE];
        OMX_VERSIONTYPE componentVersion;
        OMX_VERSIONTYPE specVersion;

        ADM_ASSERT(components_p[position]);
        ste_adm_res_t res = il_tool_GetComponentVersion(components_p[position],
                                          component_name,
                                          &componentVersion,
                                          &specVersion,
                                          NULL);

        if (res == STE_ADM_RES_OK && strcmp(component_name, il_comp_name) == 0)
        {
            return components_p[position];
        }
    }
    else
    {
        ALOG_ERR("dev_omx_get_component_at: Invalid position %d, nbr_of_comp = %d\n", position, nbr_of_comp);
        return 0;
    }

    ALOG_ERR("dev_omx_get_component_at: Component at %d not matching '%s' in chain %d\n", position, il_comp_name, chain_type);
    return 0;
}

il_comp_t dev_omx_get_component(dev_omx_graph_t* graph_p,
                              ste_adm_effect_chain_type_t chain_type,
                              const char* il_comp_name)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;
    il_comp_t*    components_p;
    unsigned int  nbr_of_comp;

    switch(chain_type)
    {
    case STE_ADM_APPLICATION_CHAIN:
        nbr_of_comp  = graph_p->app.nbr_of_comp;
        components_p = graph_p->app.comp_handles;
        break;
    case STE_ADM_VOICE_CHAIN:
        nbr_of_comp  = graph_p->voice.nbr_of_comp;
        components_p = graph_p->voice.comp_handles;
        break;
    case STE_ADM_COMMON_CHAIN:
        nbr_of_comp  = graph_p->common.nbr_of_comp;
        components_p = graph_p->common.comp_handles;
        break;
    default:
        ALOG_ERR("dev_omx_get_component: Invalid chain type %d\n", chain_type);
        return 0;
    }

    unsigned int i;
    for (i = 0; i < nbr_of_comp; i++)
    {
        char component_name[OMX_MAX_STRINGNAME_SIZE];
        OMX_VERSIONTYPE componentVersion;
        OMX_VERSIONTYPE specVersion;

        ADM_ASSERT(components_p[i]);
        ste_adm_res_t res = il_tool_GetComponentVersion(components_p[i],
                                          component_name,
                                          &componentVersion,
                                          &specVersion,
                                          NULL);

        if (res == STE_ADM_RES_OK && strcmp(component_name, il_comp_name) == 0)
        {
            return components_p[i];
        }
    }

    ALOG_INFO("ste_adm_cmd_handle_get_set_config: Component not found - '%s' in chain %d\n", il_comp_name, chain_type);
    return 0;
}



il_comp_t dev_omx_get_common_mixer_splitter(dev_omx_graph_t* graph_p,
                                                int is_vc,
                                                OMX_U32* port_p)

{
    il_comp_t *comp_p = &graph_p->common.mixer_splitter_handle;

    if (comp_p == NULL)
    {
        ALOG_ERR("dev_omx_get_common_mixer_splitter: Mixer/Splitter is NULL\n");
        return NULL;
    }

    if (graph_p->is_input && is_vc)
    {
        *port_p = SPLITTER_PORT_VOICE_OUTPUT;
    }
    else if (graph_p->is_input && !is_vc)
    {
        *port_p = SPLITTER_PORT_APP_OUTPUT;
    }
    else if (!graph_p->is_input && is_vc)
    {
        *port_p = MIXER_PORT_VOICE_INPUT;
    }
    else if (!graph_p->is_input && !is_vc)
    {
        *port_p = MIXER_PORT_APP_INPUT;
    }

    return *comp_p;
}

il_comp_t dev_omx_get_source(dev_omx_graph_t* graph_p)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

    // User must provide an input graph
    ADM_ASSERT(graph_p->is_input);

    // As long as the graph exists, the IO handle is valid
    ADM_ASSERT(graph_p->common.io_handle);

    return graph_p->common.io_handle;
}

il_comp_t dev_omx_get_sink(dev_omx_graph_t* graph_p)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

    // User must provide an output graph
    if (graph_p->is_input) {
        ALOG_WARN("dev_omx_get_sink: input graph found\n");
        return NULL;
    }

    // As long as the graph exists, the IO handle is valid
    ADM_ASSERT(graph_p->common.io_handle);

    return graph_p->common.io_handle;
}

il_comp_t dev_omx_get_app_mixer_splitter(dev_omx_graph_t* graph_p)
{
    SRV_DBG_ASSERT_IS_NOT_WORKER_THREAD;

    // As long as the graph exists, the IO handle is valid
    // for pcm devices

    return graph_p->app.mixer_splitter_handle;
}

int dev_omx_is_nonpcm_device(dev_omx_graph_t* graph_p)
{
    if (graph_p->common.io_handle && !graph_p->common.mixer_splitter_handle) {
        return 1;
    }
    return 0;
}

ste_adm_res_t dev_omx_switch_tty_mode_on_app_dev(dev_omx_graph_t* graph_p, ste_adm_tty_mode_t tty_mode)
{
    // Choose correct device name.
    const char *device_name;
    if (graph_p->is_input)
    {
        switch (tty_mode)
        {
            case STE_ADM_TTY_MODE_OFF:
            case STE_ADM_TTY_MODE_VCO:
                device_name = STE_ADM_DEVICE_STRING_HSIN;
                break;
            case STE_ADM_TTY_MODE_HCO:
            case STE_ADM_TTY_MODE_FULL:
                device_name = STE_ADM_DEVICE_STRING_TTYIN;
                break;
            default:
                return STE_ADM_RES_INVALID_PARAMETER;

        }
    }
    else
    {
        switch (tty_mode)
        {
            case STE_ADM_TTY_MODE_OFF:
            case STE_ADM_TTY_MODE_HCO:
                device_name = STE_ADM_DEVICE_STRING_HSOUT;
                break;
            case STE_ADM_TTY_MODE_VCO:
            case STE_ADM_TTY_MODE_FULL:
                device_name = STE_ADM_DEVICE_STRING_TTYOUT;
                break;
            default:
                return STE_ADM_RES_INVALID_PARAMETER;

        }
    }

    // Get samplerate and channels from current device.
    OMX_U32 port = 0;
    il_comp_t comp = dev_omx_get_common_mixer_splitter(graph_p, 0, &port);

    if (NULL == comp)
    {
        ALOG_ERR("dev_omx_switch_tty_mode_on_app_dev: no common mixer/splitter found\n");
        return STE_ADM_RES_INCORRECT_STATE;
    }

    OMX_AUDIO_PARAM_PCMMODETYPE pcm_params;
    IL_TOOL_INIT_CONFIG_STRUCT(pcm_params);

    pcm_params.nPortIndex = port;

    ste_adm_res_t res = il_tool_GetParameter(comp, OMX_IndexParamAudioPcm, &pcm_params);

    if (res != STE_ADM_RES_OK)
    {
        ALOG_ERR("dev_omx_switch_tty_mode_on_app_dev: il_tool_GetParameter failed: %d\n", res);
        return res;
    }

    struct dev_params dev_params;
    dev_params.name_in = NULL;
    dev_params.name_out = NULL;
    dev_params.alloc_buffers = 0;
    dev_params.samplerate = (int)pcm_params.nSamplingRate;
    dev_params.format = (ste_adm_format_t) pcm_params.nChannels;
    dev_params.shm_fd = -1;

    // Change the common graph.

    RETURN_IF_FAIL(dev_omx_graph_disconnect_mixer_splitter_from_app_voice(graph_p, 0));
    RETURN_IF_FAIL(dev_omx_close_common_graph(graph_p));
    RETURN_IF_FAIL(dev_omx_open_common_graph(graph_p,device_name, &dev_params));

    return STE_ADM_RES_OK;
}



static ste_adm_res_t dev_omx_setup_coded_sink(dev_omx_graph_t* graph_p,
                                      const char* name,
                                      struct dev_params* dev_params,
                                      int* logical_handle_p)
{
    if (graph_p->common.io_handle) {
        ALOG_ERR("Coded data only supports one input\n");
        return STE_ADM_RES_DEV_PER_SESSION_LIMIT_REACHED;
    }

    *logical_handle_p = 0;

    char *io_name = NULL;
    adm_db_cfg_iter_t cfg_iter;
    memset(&cfg_iter, 0, sizeof(cfg_iter));

    ste_adm_res_t res = adm_db_io_cfg_iter_create(name, &io_name, &cfg_iter);
    if (res != STE_ADM_RES_OK) {
        adm_db_cfg_iter_destroy(&cfg_iter);
        ALOG_ERR("adm_db_io_cfg_iter_create failed\n");
        return STE_ADM_RES_UNKNOWN_DATABASE_ERROR;
    }

    il_tool_save_state();
    ADM_ASSERT(io_name != NULL);
    res = il_tool_create_component(&graph_p->common.io_handle, io_name, "io");
    if (res != STE_ADM_RES_OK) {
        ALOG_ERR("il_tool_create_component failed\n");
    }

    if (res == STE_ADM_RES_OK) {
        res = adm_setup_component(graph_p->common.io_handle, &cfg_iter, 1, 1); //Both SetParameter and SetConfig
        if (res != STE_ADM_RES_OK) {
            ALOG_ERR("adm_setup_component failed\n");
        }
    }


    OMX_AUDIO_PARAM_PCMMODETYPE pcm;
    IL_TOOL_INIT_CONFIG_STRUCT(pcm);

    pcm.nPortIndex = 0;
    pcm.nChannels = 2;
    pcm.eNumData = OMX_NumericalDataSigned;
    pcm.eEndian = OMX_EndianLittle;
    pcm.bInterleaved = OMX_TRUE;
    pcm.nBitPerSample = 16;
    pcm.nSamplingRate = 48000;
    pcm.ePCMMode = OMX_AUDIO_PCMModeLinear;
    pcm.eChannelMapping[0] = OMX_AUDIO_ChannelLF;
    pcm.eChannelMapping[1] = OMX_AUDIO_ChannelRF;
    IL_TOOL_SET_PARAM(graph_p->common.io_handle, OMX_IndexParamAudioPcm, &pcm);



    OMX_AUDIO_PARAM_ALSASINK param_alsasink;
    IL_TOOL_INIT_CONFIG_STRUCT(param_alsasink);
    switch (dev_params->format) {
        case STE_ADM_FORMAT_AC3:     param_alsasink.coding_type = OMX_AUDIO_ALSASINK_CT_AAC;           break;
        case STE_ADM_FORMAT_MPEG1:   param_alsasink.coding_type = OMX_AUDIO_ALSASINK_CT_MPEG1;         break;
        case STE_ADM_FORMAT_MP3:     param_alsasink.coding_type = OMX_AUDIO_ALSASINK_CT_MP3;           break;
        case STE_ADM_FORMAT_MPEG2:   param_alsasink.coding_type = OMX_AUDIO_ALSASINK_CT_MPEG2;         break;
        case STE_ADM_FORMAT_AAC:     param_alsasink.coding_type = OMX_AUDIO_ALSASINK_CT_AAC;           break;
        case STE_ADM_FORMAT_DTS:     param_alsasink.coding_type = OMX_AUDIO_ALSASINK_CT_DTS;           break;
        case STE_ADM_FORMAT_ATRAC:   param_alsasink.coding_type = OMX_AUDIO_ALSASINK_CT_ATRAC;         break;
        case STE_ADM_FORMAT_OBA:     param_alsasink.coding_type = OMX_AUDIO_ALSASINK_CT_ONE_BIT_AUDIO; break;
        case STE_ADM_FORMAT_DDPLUS:  param_alsasink.coding_type = OMX_AUDIO_ALSASINK_CT_DOLBY_DIGITAL; break;
        case STE_ADM_FORMAT_DTS_HD:  param_alsasink.coding_type = OMX_AUDIO_ALSASINK_CT_DTS_HD;        break;
        case STE_ADM_FORMAT_MAT:     param_alsasink.coding_type = OMX_AUDIO_ALSASINK_CT_MAT;           break;
        case STE_ADM_FORMAT_DST:     param_alsasink.coding_type = OMX_AUDIO_ALSASINK_CT_DST;           break;
        case STE_ADM_FORMAT_WMA_PRO: param_alsasink.coding_type = OMX_AUDIO_ALSASINK_CT_WMA_PRO;       break;
        default:
        {
            ALOG_ERR("Invalid format %X\n", dev_params->format);
            res = STE_ADM_RES_INVALID_PARAMETER;
            goto cleanup;
        }
    }

    param_alsasink.nPortIndex = 0;
    IL_TOOL_SET_PARAM(graph_p->common.io_handle, OMX_IndexParamAlsasink, &param_alsasink);

    IL_TOOL_DISABLE_PORT(graph_p->common.io_handle, 0);
    IL_TOOL_ENTER_STATE(OMX_StateIdle,      graph_p->common.io_handle);
    IL_TOOL_ENTER_STATE(OMX_StateExecuting, graph_p->common.io_handle);
    IL_TOOL_SET_PORT_BUFFER_CONFIG(graph_p->common.io_handle, 0, dev_params->num_bufs, dev_params->bufsz);
    IL_TOOL_ENABLE_PORT_WITH_IO(graph_p->common.io_handle, 0);

cleanup:
    if (res != STE_ADM_RES_OK) {
        il_tool_revert();
    }

    adm_db_cfg_iter_destroy(&cfg_iter);
    free(io_name);

    return res;
}

static ste_adm_res_t dev_omx_close_coded_sink(dev_omx_graph_t* graph_p)
{
    ste_adm_res_t res = STE_ADM_RES_OK;
    il_tool_save_state();

    IL_TOOL_DISABLE_PORT_WITH_IO(graph_p->common.io_handle, 0);
    IL_TOOL_ENTER_STATE(OMX_StateIdle,   graph_p->common.io_handle);
    IL_TOOL_ENTER_STATE(OMX_StateLoaded, graph_p->common.io_handle);
    IL_TOOL_DESTROY(&graph_p->common.io_handle);

cleanup:
    if (res != STE_ADM_RES_OK) {
        il_tool_revert();
    }

    return res;
}

