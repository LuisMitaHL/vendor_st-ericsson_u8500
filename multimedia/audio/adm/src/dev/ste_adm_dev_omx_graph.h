/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef INCLUSION_GUARD_STE_ADM_OMX_GRAPH_H
#define INCLUSION_GUARD_STE_ADM_OMX_GRAPH_H

#include "OMX_Types.h"
#include "ste_adm_dev_omx_util.h"
#include "ste_adm_client.h"

int adm_omx_graph_get_io_pipe_output();


ste_adm_res_t adm_omx_graph_init_common(dev_omx_graph_t* graph_p, int is_hdmi);
ste_adm_res_t adm_omx_graph_init(dev_omx_graph_t* graph_p, int is_vc);

int dev_omx_graph_connect_effects(
                            unsigned int                nbr_of_comp,
                            il_comp_t                   *comp_handles_p);

ste_adm_res_t dev_omx_graph_state_transition_request(
                            il_comp_t                   *comp_handles_p,
                            unsigned int                nbr_of_comp,
                            OMX_STATETYPE               omx_state,
                            unsigned int                transit_chain_forward);

ste_adm_res_t dev_omx_graph_state_transition_wait(unsigned int                nbr_of_comp);

ste_adm_res_t dev_omx_graph_state_transition_check(
                            il_comp_t                   *comp_handles_p,
                            unsigned int                nbr_of_comp,
                            OMX_STATETYPE               omx_state);

ste_adm_res_t dev_omx_graph_init_pcm_mode_common_mixer_splitter(
                            dev_omx_graph_t*            graph_p,
                            const char*                 name,
                            il_comp_t                   mixer_splitter_comp,
                            OMX_INDEXTYPE               port_index,
                            int                         samplerate,
                            unsigned int                channels);

ste_adm_res_t dev_omx_graph_init_pcm_mode_app_mixer_splitter(
                            dev_omx_graph_t*            graph_p,
                            const char*                 name,
                            il_comp_t                   mixer_splitter_comp,
                            OMX_INDEXTYPE               port_index,
                            int                         samplerate,
                            unsigned int                channels);

ste_adm_res_t dev_omx_graph_connect_io_to_common(
                            dev_omx_graph_t*        graph_p,
                            unsigned int                nbr_of_comp,
                            il_comp_t                   *comp_handles_p);

ste_adm_res_t adm_omx_graph_connect_pre_effect_to_app(
                            dev_omx_graph_t*        graph_p,
                            OMX_U32                     port_index);

ste_adm_res_t dev_omx_graph_connect_mixer_splitter_to_common(
                            dev_omx_graph_t*        graph_p,
                            unsigned int                nbr_of_comp,
                            il_comp_t                   *comp_handles_p);

ste_adm_res_t dev_omx_graph_connect_app_voice_graph(
                            dev_omx_graph_t*        graph_p,
                            int                     is_vc);

ste_adm_res_t dev_omx_graph_connect_mixer_splitter_to_app_voice(
                            dev_omx_graph_t*        graph_p,
                            int                     is_vc);

ste_adm_res_t dev_omx_graph_disconnect_mixer_splitter_from_app_voice(
                            dev_omx_graph_t*        graph_p,
                            int                     is_vc);

ste_adm_res_t adm_omx_graph_get_port_index(
                            dev_omx_graph_t*        graph_p,
                            int                         is_vc,
                            OMX_U32                     *port_index_p);

/**
 * Connect common effect chain to the common mixer/splitter.
 * If no effects, the source/sink is connected to common mixer/splitter.
 * @return STE_ADM_RES_OK on success, negative error code on failure.
 */
ste_adm_res_t graph_connect_common_effects_to_mix_split(dev_omx_graph_t* graph_p);
/**
 * Disconnect common effect chain from the common mixer/splitter.
 * If no effects, the source/sink is disconnected from the common mixer/splitter.
 * @return STE_ADM_RES_OK on success, negative error code on failure.
 */
ste_adm_res_t graph_disconnect_common_effects_from_mix_split(dev_omx_graph_t* graph_p);

/**
 * Connect common effect chain to the source/sink.
 * If no effects, the common mixer/splitter is connected to the source/sink.
 * @return STE_ADM_RES_OK on success, negative error code on failure.
 */
ste_adm_res_t graph_connect_common_effects_to_io(dev_omx_graph_t* graph_p);
/**
 * Disconnect common effect chain from the source/sink.
 * If no effects, the source/sink is disconnected from the common mixer/splitter
 * @return STE_ADM_RES_OK on success, negative error code on failure.
 */
ste_adm_res_t graph_disconnect_common_effects_from_io(dev_omx_graph_t* graph_p);

/**
 * Connect voice effect chain to the common mixer/splitter.
 * If no effects, nothing is connected.
 * @return STE_ADM_RES_OK on success, negative error code on failure.
 */
ste_adm_res_t graph_connect_voice_effects_to_mix_split(dev_omx_graph_t* graph_p);
/**
 * Disconnect voice effect chain from the common mixer/splitter.
 * If no effects, nothing is disconnected.
 * @return STE_ADM_RES_OK on success, negative error code on failure.
 */
ste_adm_res_t graph_disconnect_voice_effects_from_mix_split(dev_omx_graph_t* graph_p);

/**
 * Connect application effect chain to the common mixer/splitter.
 * If no effects, the app mixer/splitter is connected to common mixer/splitter.
 * @return STE_ADM_RES_OK on success, negative error code on failure.
 */
ste_adm_res_t graph_connect_app_effects_to_common_mix_split(dev_omx_graph_t* graph_p);
/**
 * Disconnect application effect chain from the common mixer/splitter.
 * If no effects, the app mixer/splitter is disconnected from common mixer/splitter.
 * @return STE_ADM_RES_OK on success, negative error code on failure.
 */
ste_adm_res_t graph_disconnect_app_effects_from_common_mix_split(dev_omx_graph_t* graph_p);

/**
 * Connect application effect chain to the app mixer/splitter.
 * If no effects, the common mixer/splitter is connected to the app mixer/splitter.
 * @return STE_ADM_RES_OK on success, negative error code on failure.
 */
ste_adm_res_t graph_connect_app_effects_to_app_mix_split(dev_omx_graph_t* graph_p);
/**
 * Disconnect application effect chain from the app mixer/splitter.
 * If no effects, the common mixer/splitter is disconnected from the app mixer/splitter.
 * @return STE_ADM_RES_OK on success, negative error code on failure.
 */
ste_adm_res_t graph_disconnect_app_effects_from_app_mix_split(dev_omx_graph_t* graph_p);

#endif //INCLUSION_GUARD_STE_ADM_OMX_GRAPH_H
