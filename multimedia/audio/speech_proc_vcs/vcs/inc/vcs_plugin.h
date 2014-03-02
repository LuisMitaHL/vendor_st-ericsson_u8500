#ifndef _VCS_PLUGIN_H_
#define _VCS_PLUGIN_H_
/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file     vcs_algorithm_gen.h
 * \brief    Header file for algorithm pluginS
 * \author   ST-Ericsson
 */
/*****************************************************************************/
#include "vcs_algorithm_gen.h"
#include "vcs.h"

#ifdef __cplusplus
extern "C"
{
#endif
typedef enum {
  VCS_PLUGIN_RX_FIRST,
  VCS_PLUGIN_RX_LAST,
  VCS_PLUGIN_TX_LAST,
  VCS_PLUGIN_LAST
} vcs_plugin_types_t;

typedef vcs_algorithm_t* (*vcs_algorithm_factory)(void);

struct vcs_plugin_s;
typedef struct vcs_plugin_s vcs_plugin_t;

vcs_plugin_t* vcs_plugin_init(vcs_log_t *log);
void vcs_plugin_destroy(vcs_plugin_t* plugin);


vcs_algorithm_t* vcs_plugin_get(vcs_plugin_t* self, vcs_plugin_types_t kind);

void vcs_to_algorithm_mode(vcs_algorith_mode_t *algomode,
					       const vcs_mode_t *mode,
					       unsigned int nb_channels,
					       unsigned int interleaved);


#ifdef __cplusplus
}
#endif

#endif // _VCS_PLUGIN_H_