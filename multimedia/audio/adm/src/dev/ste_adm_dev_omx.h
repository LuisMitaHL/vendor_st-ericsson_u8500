/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*! \file ste_adm_omx.h
*   \brief Exports STE ADM OpenMAX functionality.

    This header exports all functionality needed for setting up the OpenMAX
    graph.
*/

#ifndef INCLUSION_GUARD_STE_ADM_OMX_H
#define INCLUSION_GUARD_STE_ADM_OMX_H

#include "OMX_Component.h"
#include "OMX_Types.h"
#include "ste_adm_client.h"
#include "ste_adm_util.h"
#include "ste_adm_dev.h"
#include "ste_adm_omx_tool.h"

#define ADM_OMX_MAX_NBR_OF_PORTS 8

#define ADM_OMX_CHAIN_MAX_LENGTH 8


/**
* dev_omx_graph_app_t defines the application graph of
* OMX components for a certain device.
*/
typedef struct {
    il_comp_t               comp_handles[ADM_OMX_CHAIN_MAX_LENGTH];
    /**< OMX handles to the components in the effect chain */
    unsigned int            nbr_of_comp;
    /**< Nbr of comp in the effect chain (i.e. in comp_handle_pp) */
    unsigned int            nbr_mixer_effects;
    /**< Nbr of mixer effects in the effect chain */
    il_comp_t               mixer_splitter_handle;
    /**< OMX handle to the App mixer/splitter, input/output to graph */
    int                     port_index[ADM_OMX_MAX_NBR_OF_PORTS];
    /**< List of available port indexes in the mixer/splitter */
    il_comp_t               pre_effect_handle[ADM_OMX_MAX_NBR_OF_PORTS];
    /**< OMX handle to the pre effect component */
    unsigned int            ref_cnt;
    /**< Number of clients currently referencing the App Chain of this device */
} dev_omx_graph_app_t;

/**
* dev_omx_graph_voice_t defines the call graph of
* OMX components for a certain device.
*/
typedef struct {
    il_comp_t               comp_handles[ADM_OMX_CHAIN_MAX_LENGTH];
    /**< OMX handles to the components in the effect chain */
    unsigned int            nbr_of_comp;
    /**< Nbr of comp in the effect chain (i.e. in comp_handle_p) */
    unsigned int            nbr_mixer_effects;
    /**< Nbr of mixer effects in the effect chain */
    unsigned int            ref_cnt;
    /**< Number of clients currently referencing the Voice Chain of this device,
        only 1 is supported */
} dev_omx_graph_voice_t;

/**
* dev_omx_graph_common_t defines the common graph (i.e. used by both app
* and voice) of OMX comps for a certain device.
*/
typedef struct {
    il_comp_t           comp_handles[ADM_OMX_CHAIN_MAX_LENGTH];
    /**< OMX handles to the components in the effect chain */
    unsigned int        nbr_of_comp;
    /**< Nbr of comp in the effect chain (i.e. in comp_handle_p) */
    unsigned int        nbr_mixer_effects;
    /**< Nbr of mixer effects in the effect chain */
    il_comp_t           io_handle;
    /**< OMX handle to the sink */
    il_comp_t           mixer_splitter_handle;
    /**< Number of channels the device supports */
    int                 hw_num_channels;
} dev_omx_graph_common_t;

typedef struct dev_omx_graph_s {
    int                     is_input;
    int                     use_mixer_effects;
    dev_omx_graph_app_t     app;
    dev_omx_graph_voice_t   voice;
    dev_omx_graph_common_t  common;
} dev_omx_graph_t;

/**
* dev_omx_graph_setup
* @return STE_ADM_RES_OK on success, negative error code on failure.
*/
ste_adm_res_t dev_omx_setup_graph(dev_omx_graph_t* graph_p,
                                      const char* name,
                                      int         is_vc,
                                      struct dev_params* dev_params,
                                      int* logical_handle);

/**
* dev_omx_graph_close
* @return STE_ADM_RES_OK on success, negative error code on failure.
*/
ste_adm_res_t dev_omx_close_graph(dev_omx_graph_t* graph_p,
                                      int logical_handle,
                                      int dealloc_buffers,
                                      int* last_device_closed);

/**
* dev_omx_reconfigure_effects
* @return STE_ADM_RES_OK on success, negative error code on failure.
*/
ste_adm_res_t dev_omx_reconfigure_effects(dev_omx_graph_t* graph_p,
                                          const char* name,
                                          const char* name2,
                                          int vc_disconnected);
/**
* dev_omx_rescan_effects
* Rescans config for all effects in chain without breaking data flow.
* @return STE_ADM_RES_OK on success, negative error code on failure.
*/
ste_adm_res_t dev_omx_rescan_effects(dev_omx_graph_t* graph_p,
                                    ste_adm_effect_chain_type_t chain,
                                    const char* name,
                                    const char* name2);

void dev_omx_get_endpoint(dev_omx_graph_t* graph_p,
                                       int logical_handle,
                                       il_comp_t*      comp_p,
                                       OMX_U32*        port_p);

ste_adm_res_t dev_omx_get_endpoint_echoref(const dev_omx_graph_t* graph_p,
                                       il_comp_t*      comp_p,
                                       OMX_U32*        port_p);

// TODO: Join with dev_omx_get_endpoint_echoref somehow?
il_comp_t dev_omx_get_component(dev_omx_graph_t* graph_p,
                              ste_adm_effect_chain_type_t chain_type,
                              const char* il_comp_name);

// Get a specific component in a chain
il_comp_t dev_omx_get_component_at(dev_omx_graph_t* graph_p,
                              ste_adm_effect_chain_type_t chain_type,
                              const char* il_comp_name,
                              unsigned int position);

/**
 * Returns component and port for the common mixer or splitter.
 */
il_comp_t dev_omx_get_common_mixer_splitter(dev_omx_graph_t* graph_p,
                                                int is_vc,
                                                OMX_U32* port_p);

int dev_omx_is_nonpcm_device(dev_omx_graph_t* graph_p);

il_comp_t dev_omx_get_source(dev_omx_graph_t* graph_p);

il_comp_t dev_omx_get_sink(dev_omx_graph_t* graph_p);

il_comp_t dev_omx_get_app_mixer_splitter(dev_omx_graph_t* graph_p);

ste_adm_res_t dev_omx_switch_tty_mode_on_app_dev(dev_omx_graph_t* graph_p, ste_adm_tty_mode_t tty_mode);

#endif //INCLUSION_GUARD_STE_ADM_OMX_H


