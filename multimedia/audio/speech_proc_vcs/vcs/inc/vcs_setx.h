#ifndef _VCS_SETX_H_
#define _VCS_SETX_H_
/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file     vcs_setx.h
 * \brief    Internal header file for voice shell speech enhancement uplink (tx)
 * \author   ST-Ericsson
 */
/*****************************************************************************/
#include "vcs.h"
#include "vcs_algo.h"
#ifdef VCSSE
#include "speech_enhancement.h"
#endif

#ifdef __cplusplus
extern "C"
{
#endif

  //typedef struct vcs_serx_s vcs_serx_t;
  typedef struct vcs_setx_s vcs_setx_t;
  
  
  vcs_setx_t* vcs_setx_create(vcs_log_t* log);
  
  void vcs_setx_destroy(vcs_setx_t *tx);
  
  int vcs_setx_enable(vcs_setx_t *tx,
					  const vcs_algo_mode_t *mode);
  
  
  int vcs_setx_reset(vcs_setx_t *tx,
					 const vcs_algo_mode_t *mode);
  
  int vcs_setx_configure(vcs_setx_t *tx,
						 const vcs_algo_mode_t *mode,
						 const vcs_setx_config_t* param);
  void vcs_setx_disable(vcs_setx_t *tx);
  
  int vcs_setx_is_enabled(vcs_setx_t *tx);
  int vcs_setx_is_running(vcs_setx_t *tx);
  
  void vcs_setx_execute(vcs_setx_t *tx,
						short** input,
						int nb_input,
						short** ecref,
						int nb_ecref,
						short** output,
						int nb_output,
						short** linoutput,
						int nb_linoutput);

  void vcs_setx_export(vcs_setx_t *tx,
					   vcs_se_export_t  **data);
  
  void vcs_setx_import(vcs_setx_t *tx,
					   vcs_se_export_t  **data);
  
  void vcs_setx_config_tostring(char* buffer,
						 unsigned int len,
						 const vcs_setx_config_t* param);
		

  int vcs_setx_probe_is_supported(const vcs_setx_t *tx, vcs_probe_points_t kind);
  
  vcs_se_probedata_t* vcs_setx_probe_data_alloc(vcs_setx_t *tx, vcs_probe_points_t kind);  
   
  void vcs_setx_probe_data_free(vcs_se_probedata_t* data);
  
  int vcs_setx_probe_data_get(vcs_setx_t *tx, vcs_se_probedata_t* data); 
  
  
#ifdef __cplusplus
}
#endif

#endif 
