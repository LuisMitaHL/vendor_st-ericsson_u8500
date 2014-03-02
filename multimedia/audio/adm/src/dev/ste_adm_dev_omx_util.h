/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef INCLUSION_GUARD_STE_ADM_OMX_UTIL_H
#define INCLUSION_GUARD_STE_ADM_OMX_UTIL_H

#include "OMX_Types.h"
#include "OMX_Audio.h"
#include "ste_adm_omx_io.h"
#include "ste_adm_dev_omx.h"
#include "ste_adm_db.h"


#define STE_ADM_OMX_CHECK_RESULT(_result_) if (_result_ != 0) { \
        ALOG_ERR("ERROR! %s@%d - Result Code: 0x%08X \n", \
            __FILE__, __LINE__, (unsigned int)_result_); \
        goto cleanup; \
    }; \

#define STE_ADM_OMX_CHECK_ALLOC(_ptr_) if (_ptr_ == NULL) { \
        ALOG_ERR("ERROR! Out of memory!\n"); \
        result = -ENOMEM; \
        goto cleanup; \
    };

#define EFFECT_PORT_INPUT           0
#define EFFECT_PORT_OUTPUT          1

#define SINK_PORT_INPUT             0
#define SOURCE_PORT_OUTPUT          0
#define MIXER_PORT_OUTPUT           0
#define MIXER_PORT_APP_INPUT        1
#define MIXER_PORT_VOICE_INPUT      2

#define SPLITTER_PORT_INPUT         0
#define SPLITTER_PORT_APP_OUTPUT    1
#define SPLITTER_PORT_VOICE_OUTPUT  2

typedef struct {
    unsigned int            nbr_of_effects;
    unsigned int            nbr_of_common_effects;
} dev_omx_effect_chain_t;

typedef struct {
    unsigned int            nbr_of_comp;
    il_comp_t               *comp_handles_p;
    unsigned int            nbr_of_common_comp;
    il_comp_t               *common_comp_handles_p;
} dev_omx_comp_chain_t;

OMX_STATETYPE dev_omx_util_get_omx_state(il_comp_t      comp);

void dev_omx_print_port_def_parameters(
                            OMX_PARAM_PORTDEFINITIONTYPE *portDefParam);

char* dev_omx_stateid2statestring(OMX_STATETYPE state_id);

void dev_omx_util_free_components(dev_omx_graph_t* graph_p,
                                      int                  is_vc);

void dev_omx_util_get_components(
                            dev_omx_graph_t         *graph_p,
                            int                         is_vc,
                            dev_omx_comp_chain_t    *comp_chains_p);

int dev_omx_util_get_io_comp(
                            dev_omx_graph_t         *graph_p,
                            int                         is_vc,
                            il_comp_t                   *io_comp_p);

int adm_omx_get_pre_effect_comp(
                            dev_omx_graph_t         *graph_p,
                            OMX_U32                     port_index,
                            il_comp_t                   *pre_effect_comp_p);

int dev_omx_util_get_common_mixer_splitter(
                            dev_omx_graph_t         *graph_p,
                            int                         is_vc,
                            il_comp_t                   *omx_comp_p);

int dev_omx_util_get_app_mixer_splitter(
                            dev_omx_graph_t         *graph_p,
                            int                         is_vc,
                            il_comp_t                   *omx_comp_p);


void adm_omx_get_buffer_comp(dev_omx_graph_t*        graph_p,
                            OMX_U32                 port_index,
                            il_comp_t           *buffer_comp_p,
                            OMX_U32                 *buffer_port_p);

void adm_omx_get_app_end_comp(dev_omx_graph_t* graph_p,
                             il_comp_t            *comp_p,
                             OMX_U32              *port_p);

void adm_omx_get_voice_handle(
                            dev_omx_graph_t         *graph_p,
                            il_comp_t                   *voice_handle_p,
                            OMX_U32                     *port_index_p);

void dev_omx_util_get_common_state(
                            const dev_omx_graph_t  *graph_p,
                            unsigned int               *nbr_of_connected_graphs_p);

int adm_omx_is_pre_effect(const dev_omx_graph_t  *graph_p,
                          OMX_U32 port_index);

void adm_omx_util_get_ref_cnt(dev_omx_graph_t  *graph_p,
                             int is_vc,
                             unsigned int **ref_cnt_pp);

ste_adm_res_t adm_omx_util_get_db_AudioPcmSettings(const char* name,
                                        OMX_AUDIO_PARAM_PCMMODETYPE *settings_p);


// Get the VC output port from the master splitter (the port receiving data from the VC chain)
int dev_omx_get_splitter_vc_port(il_comp_t* out_handle, OMX_INDEXTYPE* out_index);

// Get the VC input port from the master mixer (the port feeding the VC chain)
int dev_omx_get_mixer_vc_port(il_comp_t* out_handle, OMX_INDEXTYPE* out_index);

/**
* adm_setup_component
* @return 0 on success, negative error code on failure.
*/
ste_adm_res_t adm_setup_component(il_comp_t comp_handle, adm_db_cfg_iter_t *cfgit, int doSetParameter, int doSetConfig);

/**
* adm_setup_mixer_effect
* Adds effect to the common mixer and configures it.
* @return 0 on success, negative error code on failure.
*/
ste_adm_res_t adm_setup_mixer_effect(dev_omx_graph_t* graph_p,
        int position,
        ste_adm_effect_chain_type_t chain,
        const char *effect_name,
        adm_db_cfg_iter_t *cfgit,
        int doSetParameter,
        int doSetConfig,
        int createEffect);

#endif //INCLUSION_GUARD_STE_ADM_OMX_UTIL_H
