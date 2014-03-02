/*
 * Copyright (C) -Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#define LOG_TAG "ADM_OMX"
#define ADM_LOG_FILENAME "omx_util"
#include "ste_adm_dbg.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>

#include "OMX_Component.h"
#include "OMX_Types.h"
#include "OMX_Core.h"
#include "sqlite3.h"



#include "ste_adm_dev_omx.h"
#include "ste_adm_dev_omx_util.h"
#include "ste_adm_db.h"
#include "ste_adm_platform_adaptions.h"


OMX_STATETYPE dev_omx_util_get_omx_state(il_comp_t comp)
{
    OMX_STATETYPE t;
    if (il_tool_GetState(comp, &t) != STE_ADM_RES_OK) {
        ALOG_ERR("il_tool_GetState failed for component %X\n", (unsigned long) il_tool_raw_handle(comp));
        return OMX_StateTestForResources;
    }
    return t;
}


void dev_omx_print_port_def_parameters(
                                    OMX_PARAM_PORTDEFINITIONTYPE *portDefParam)
{
    if (portDefParam == NULL)
    {
        ALOG_ERR("portDefParam is NULL");
        return;
    }
    ALOG_INFO("\tnPortIndex: (OMX_U32)%u\n",
        (unsigned int)portDefParam->nPortIndex);
    if (portDefParam->eDir == OMX_DirInput) {
        ALOG_INFO("\teDir: (OMX_DIRTYPE)OMX_DirInput\n");
    } else {
        ALOG_INFO("\teDir: (OMX_DIRTYPE)OMX_DirOutput\n");
    }
    ALOG_INFO("\tnBufferCountActual: (OMX_U32)%u\n",
        (unsigned int)portDefParam->nBufferCountActual);
    ALOG_INFO("\tnBufferCountMin: (OMX_U32)%u\n",
        (unsigned int)portDefParam->nBufferCountMin);
    ALOG_INFO("\tnBufferSize: (OMX_U32)%u\n",
        (unsigned int)portDefParam->nBufferSize);
    ALOG_INFO("\tbEnabled: (OMX_BOOL)%u\n",
        (unsigned int)portDefParam->bEnabled);
    ALOG_INFO("\tbPopulated: (OMX_BOOL)%u\n",
        (unsigned int)portDefParam->bPopulated);
    ALOG_INFO("\tbBuffersContiguous: (OMX_BOOL)%u\n",
        (unsigned int)portDefParam->bBuffersContiguous);
    ALOG_INFO("\tnBufferAlignment: (OMX_U32)%u\n",
        (unsigned int)portDefParam->nBufferAlignment);
    ALOG_INFO("\teDomain: (OMX_PORTDOMAINTYPE)%u\n",
        (unsigned int)portDefParam->eDomain);
}

char* dev_omx_stateid2statestring(OMX_STATETYPE state_id)
{
    switch (state_id) {
    case OMX_StateInvalid:
        return "OMX_StateInvalid";
    case OMX_StateLoaded:
        return "OMX_StateLoaded";
    case OMX_StateIdle:
        return "OMX_StateIdle";
    case OMX_StateExecuting:
        return "OMX_StateExecuting";
    case OMX_StatePause:
        return "OMX_StatePause";
    case OMX_StateWaitForResources:
        return "OMX_StateWaitForResources";
    default:
        ADM_ASSERT(0);
        return "Unknown State";
    }
}


void dev_omx_util_free_components(dev_omx_graph_t*        graph_p,
                                               int is_vc)
{
    int i = 0;
    unsigned int nbr_of_comp;
    il_comp_t* comp_handles_p;

    /* Free components for the graph corresponding to the specified device id */

    if (!is_vc) {
        nbr_of_comp = graph_p->app.nbr_of_comp;
        graph_p->app.nbr_of_comp = 0;
        comp_handles_p = graph_p->app.comp_handles;
    } else {
        nbr_of_comp = graph_p->voice.nbr_of_comp;
        graph_p->voice.nbr_of_comp = 0;
        comp_handles_p = graph_p->voice.comp_handles;
    }

    for (i = 0; i < (int)nbr_of_comp; i++) {
        if (dev_omx_util_get_omx_state(
                comp_handles_p[i]) != OMX_StateLoaded) {

            ALOG_ERR("IL chain component %X not in StateLoaded! "
                    "Not calling FreeHandle\n",
                    il_tool_raw_handle(comp_handles_p[i]));
        } else {
            il_tool_destroy_component(&comp_handles_p[i]);
        }
    }
}

void dev_omx_util_get_components(
                            dev_omx_graph_t*        graph_p,
                            int                         is_vc,
                            dev_omx_comp_chain_t    *comp_chains_p)
{
    ADM_ASSERT(comp_chains_p != NULL);

    if (!is_vc) {
        comp_chains_p->comp_handles_p =
            graph_p->app.comp_handles;
        comp_chains_p->nbr_of_comp =
            graph_p->app.nbr_of_comp;
        comp_chains_p->common_comp_handles_p =
            graph_p->common.comp_handles;
        comp_chains_p->nbr_of_common_comp =
            graph_p->common.nbr_of_comp;
    } else {
        comp_chains_p->comp_handles_p =
            graph_p->voice.comp_handles;
        comp_chains_p->nbr_of_comp =
            graph_p->voice.nbr_of_comp;
        comp_chains_p->common_comp_handles_p =
            graph_p->common.comp_handles;
        comp_chains_p->nbr_of_common_comp =
            graph_p->common.nbr_of_comp;
    }
}





void adm_omx_get_buffer_comp(dev_omx_graph_t*   graph_p,
                            OMX_U32                 port_index,
                            il_comp_t               *buffer_comp_p,
                            OMX_U32                 *buffer_port_p)
{
    ADM_ASSERT(buffer_comp_p != NULL);
    ADM_ASSERT(buffer_port_p != NULL);

    /* The buffer component is the first component in the application chain.
        First it must be checked if a pre effect is utilized. In that case
        the buffer component is the pre effect, otherwise the application
        mixer/splitter is the buffer component */
    if (graph_p->common.io_handle && !graph_p->common.mixer_splitter_handle) {
        // io_handle && !common.mixer_splitter_handle --> direct sink connection (e.g. codec HDMI)
        *buffer_comp_p = graph_p->common.io_handle;
        *buffer_port_p = 0;
    } else if (adm_omx_is_pre_effect(graph_p, port_index)) {
        ADM_ASSERT((port_index-1) < ADM_OMX_MAX_NBR_OF_PORTS);
        ADM_ASSERT(graph_p->app.port_index[port_index-1]); // port in use
        *buffer_comp_p = graph_p->app.pre_effect_handle[port_index-1];
        *buffer_port_p = EFFECT_PORT_INPUT;
    } else {
        ADM_ASSERT((port_index-1) < ADM_OMX_MAX_NBR_OF_PORTS);
        ADM_ASSERT(graph_p->app.port_index[port_index-1]); // port in use
        *buffer_comp_p = graph_p->app.mixer_splitter_handle;
        *buffer_port_p = port_index;
    }
}

void adm_omx_get_app_end_comp(dev_omx_graph_t* graph_p,
                             il_comp_t            *comp_p,
                             OMX_U32              *port_p)
{
    dev_omx_comp_chain_t comp_chains;
    ADM_ASSERT(comp_p != NULL);

    dev_omx_util_get_components(graph_p, 0, &comp_chains);

    if (graph_p->is_input) {
        if (comp_chains.nbr_of_comp > 0) {
            *comp_p = comp_chains.comp_handles_p[0];
            *port_p = EFFECT_PORT_INPUT;
        }
        else
        {
            *comp_p = graph_p->app.mixer_splitter_handle;
            *port_p = SPLITTER_PORT_INPUT;
        }
    } else {
        if (comp_chains.nbr_of_comp > 0) {
            *comp_p = comp_chains.comp_handles_p[comp_chains.nbr_of_comp - 1];
            *port_p = EFFECT_PORT_OUTPUT;
        }
        else
        {
            *comp_p = graph_p->app.mixer_splitter_handle;
            *port_p = MIXER_PORT_OUTPUT;
        }
    }
}

void adm_omx_get_voice_handle(dev_omx_graph_t* graph_p,
                            il_comp_t                   *voice_handle_p,
                            OMX_U32                     *port_index_p)
{
    dev_omx_comp_chain_t comp_chains;

    ADM_ASSERT(graph_p != NULL);
    ADM_ASSERT(voice_handle_p != NULL);
    ADM_ASSERT(port_index_p != NULL);

    dev_omx_util_get_components(graph_p, 1, &comp_chains);

    // Get port index
    if (graph_p->is_input) {
        if (comp_chains.nbr_of_comp > 0) {
            *port_index_p = EFFECT_PORT_OUTPUT;
            *voice_handle_p =
                comp_chains.comp_handles_p[comp_chains.nbr_of_comp-1];
        } else {
            *port_index_p = SPLITTER_PORT_VOICE_OUTPUT;
            *voice_handle_p = graph_p->common.mixer_splitter_handle;
        }
    } else {
        if (comp_chains.nbr_of_comp > 0) {
            *port_index_p = EFFECT_PORT_INPUT;
            *voice_handle_p = comp_chains.comp_handles_p[0];
        } else {
            *port_index_p = MIXER_PORT_VOICE_INPUT;
            *voice_handle_p = graph_p->common.mixer_splitter_handle;
        }
    }
}

void dev_omx_util_get_common_state(
                            const dev_omx_graph_t  *graph_p,
                            unsigned int               *nbr_of_connected_graphs_p)
{
    unsigned int app_graph_created = 0;
    unsigned int voice_graph_created = 0;

    ADM_ASSERT(nbr_of_connected_graphs_p != NULL);

    /* Get the number of graphs (0,1 or 2) that are connected to Common chain */

    app_graph_created   = graph_p->app.ref_cnt;
    voice_graph_created = graph_p->voice.ref_cnt;

    if (app_graph_created && voice_graph_created) {
        *nbr_of_connected_graphs_p = 2;
    } else if (app_graph_created || voice_graph_created) {
        *nbr_of_connected_graphs_p = 1;
    } else {
        *nbr_of_connected_graphs_p = 0;
    }
}


int adm_omx_is_pre_effect(const dev_omx_graph_t  *graph_p,
                          OMX_U32 port_index)
{
    ADM_ASSERT((port_index-1) < ADM_OMX_MAX_NBR_OF_PORTS);
    return (graph_p->app.pre_effect_handle[port_index-1] != 0);
}

void adm_omx_util_get_ref_cnt(dev_omx_graph_t  *graph_p,
                             int is_vc,
                             unsigned int **ref_cnt_pp)
{
    ADM_ASSERT(graph_p != NULL);
    ADM_ASSERT(ref_cnt_pp != NULL);

    if (!is_vc) {
        *ref_cnt_pp = &graph_p->app.ref_cnt;
    } else { /* voice chain */
        *ref_cnt_pp = &graph_p->voice.ref_cnt;
    }
}

ste_adm_res_t adm_setup_component(il_comp_t comp_handle, adm_db_cfg_iter_t *cfgit, int doSetParameter, int doSetConfig)
{
    ste_adm_res_t ret = STE_ADM_RES_OK;
    ste_adm_res_t res;
    int is_param = 0;
    const char *index_name = NULL;
    const void *config_data = NULL;

    /* Iterate over settings */
    while (STE_ADM_RES_OK == (res=adm_db_cfg_iter_get_next(cfgit, &is_param,
                                    &index_name, &config_data))) {
        OMX_INDEXTYPE omx_index;
        
        if ( strcmp( index_name, "ATT_IndexDesign") == 0 ) {
            /* don't send Audio Tuning Tool design parameters to the effect */
        }
        else { 
            if (index_name[0] == '#') {
                omx_index = atoi(index_name+1);
            } else {
                ALOG_INFO_VERBOSE("NOTE: OMX Index testing ADM-internal lookup\n", index_name);
                omx_index = adaptions_string_to_index(index_name);
                if (omx_index != OMX_IndexComponentStartUnused) {
                    res = STE_ADM_RES_OK;
                } else {
                    res = il_tool_GetExtensionIndex(comp_handle, index_name, (OMX_PTR) &omx_index);
                    if (res == STE_ADM_RES_UNKNOWN_IL_INDEX_NAME || res == STE_ADM_RES_INTERNAL_ERROR) {
                        ALOG_ERR("Index '%s' could not be resolved!!\n", index_name);
                    }
                }
            }
            
            if (res != STE_ADM_RES_OK) {
                return res;
            }
            
            if (is_param && doSetParameter) {
                res = il_tool_SetParameter(comp_handle, omx_index, config_data);
            }
            if (!is_param && doSetConfig) {
                res = il_tool_SetConfig(comp_handle, omx_index, config_data);
                // result = OMX_ErrorNone; // TODO: Temporary
            }
        }
    }
    if (STE_ADM_RES_NO_MORE != res) {
        ALOG_ERR("Error iterating over params/configs");
        ret = -1;
        goto cleanup;
    } else {
        ALOG_INFO_VERBOSE("No more params/configs\n");
    }
cleanup:
    return ret;
}

ste_adm_res_t adm_setup_mixer_effect(dev_omx_graph_t* graph_p,
        int position,
        ste_adm_effect_chain_type_t chain,
        const char *effect_name,
        adm_db_cfg_iter_t *cfgit,
        int doSetParameter,
        int doSetConfig,
        int createEffect)
{
    ste_adm_res_t ret = STE_ADM_RES_OK;
    ste_adm_res_t res;
    int is_param = 0;
    const char *index_name = NULL;
    const void *config_data = NULL;

    int mixer_port;
    switch(chain){
    case STE_ADM_VOICE_CHAIN:
        mixer_port = MIXER_PORT_VOICE_INPUT;
        break;
    case STE_ADM_APPLICATION_CHAIN:
        mixer_port = MIXER_PORT_APP_INPUT;
        break;
    case STE_ADM_COMMON_CHAIN:
        mixer_port = MIXER_PORT_OUTPUT;
        break;
    default:
        return STE_ADM_RES_IL_UNKNOWN_ERROR;
    }


    if (createEffect) {
        res = il_tool_add_mixer_effect(
                graph_p->common.mixer_splitter_handle,
                position,
                mixer_port,
                effect_name);

        if (res != STE_ADM_RES_OK) {
            return res;
        }
    }

    /* Iterate over settings */
    while (STE_ADM_RES_OK == (res=adm_db_cfg_iter_get_next(cfgit, &is_param,
                                    &index_name, &config_data))) {
        OMX_INDEXTYPE omx_index;

        if ( strcmp( index_name, "ATT_IndexDesign") == 0 ) {
            /* don't send Audio Tuning Tool design parameters to the effect */
        }
        else if ( strcmp( index_name, "OMX_IndexParamAudioPcm") == 0 ) {
            /* Ignore port settings for mixer effects */
        }
        else {
            if (index_name[0] == '#') {
                omx_index = atoi(index_name+1);
            } else {
                ALOG_INFO_VERBOSE("NOTE: OMX Index testing ADM-internal lookup\n", index_name);
                omx_index = adaptions_string_to_index(index_name);
                if (omx_index != OMX_IndexComponentStartUnused) {
                    res = STE_ADM_RES_OK;
                } else {
                    res = il_tool_GetExtensionIndex(graph_p->common.mixer_splitter_handle, index_name, (OMX_PTR) &omx_index);
                    if (res == STE_ADM_RES_UNKNOWN_IL_INDEX_NAME || res == STE_ADM_RES_INTERNAL_ERROR) {
                        ALOG_ERR("Index '%s' could not be resolved!!\n", index_name);
                    }
                }
            }

            if (res != STE_ADM_RES_OK) {
                return res;
            }

            if ((is_param && doSetParameter) || (!is_param && doSetConfig)) {
                res = il_tool_setConfig_mixer_effect(graph_p->common.mixer_splitter_handle, position, mixer_port, omx_index, config_data);
            }

        }
    }


    if (STE_ADM_RES_NO_MORE != res) {
        ALOG_ERR("Error iterating over params/configs");
        ret = STE_ADM_RES_DB_FATAL;
        goto cleanup;
    } else {
        ALOG_INFO_VERBOSE("No more params/configs\n");
    }

    ret = il_tool_commit_mixer_effects(graph_p->common.mixer_splitter_handle, mixer_port);
cleanup:
    return ret;
}


ste_adm_res_t adm_omx_util_get_db_AudioPcmSettings(const char* name, OMX_AUDIO_PARAM_PCMMODETYPE *settings_p)
{
    int result;
    ste_adm_res_t res;
    int found = 0;
    char *io_name = NULL;
    adm_db_cfg_iter_t cfg_iter;
    int is_param = 0;
    const char *index_name = NULL;
    const void *config_data = NULL;

    memset(&cfg_iter, 0, sizeof(cfg_iter));

    ADM_ASSERT(settings_p);

    result = adm_db_io_cfg_iter_create(name, &io_name, &cfg_iter);
    if (result != STE_ADM_RES_OK) {
        adm_db_cfg_iter_destroy(&cfg_iter);
        goto cleanup;
    }
    free(io_name);

    /* Iterate over settings */
    while (STE_ADM_RES_OK == (res =adm_db_cfg_iter_get_next(&cfg_iter, &is_param,
                                    (const char**)&index_name, (const void**)&config_data))) {
        OMX_INDEXTYPE omx_index;
        if (index_name[0] == '#') {
            omx_index = atoi(index_name+1);
        } else {
            omx_index = adaptions_string_to_index(index_name);
            if (omx_index == OMX_IndexComponentStartUnused) {
                ALOG_ERR("Index '%s' could not be resolved!!\n", index_name);
                result = -1;
                adm_db_cfg_iter_destroy(&cfg_iter);
                goto cleanup;
            }
        }

        if (omx_index == OMX_IndexParamAudioPcm)
        {
            found = 1;
            memcpy(settings_p, config_data, sizeof(OMX_AUDIO_PARAM_PCMMODETYPE));
            break;
        }

    }
    adm_db_cfg_iter_destroy(&cfg_iter);
    if (found ==0) {
        result = -1;
    }
cleanup:
    return result;
}
