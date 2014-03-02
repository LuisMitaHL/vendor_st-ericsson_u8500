#ifndef _VCS_SERX_H_
#define _VCS_SERX_H_
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
 * \brief    Internal header file for voice shell speech enhancement
 * downlink(rx)
 * \author   ST-Ericsson
 */
/*****************************************************************************/
#include "vcs.h"
#include "vcs_algo.h"
#ifdef VCSSE
#include "speech_enhancement.h"
#endif

#ifdef __cplusplus
{
#endif

  typedef struct vcs_serx_s vcs_serx_t;
  
  
  vcs_serx_t* vcs_serx_create(vcs_log_t* log);
  
  void vcs_serx_destroy(vcs_serx_t *rx);
  
  int vcs_serx_enable(vcs_serx_t *rx,
					  const vcs_algo_mode_t *mode);
  
  
  int vcs_serx_reset(vcs_serx_t *rx,
					 const vcs_algo_mode_t *mode);
  
  int vcs_serx_configure(vcs_serx_t *rx,
						 const vcs_algo_mode_t *mode,
						 const vcs_serx_config_t* param);

  void vcs_serx_disable(vcs_serx_t *rx);
  
  int vcs_serx_is_enabled(vcs_serx_t *rx);
  int vcs_serx_is_running(vcs_serx_t *rx);
  
  void vcs_serx_execute(vcs_serx_t *rx,
						short** input,
						int nb_input,
						short** output,
						int nb_output);

  void vcs_serx_export(vcs_serx_t *rx,
					   vcs_se_export_t **data);
  
  void vcs_serx_import(vcs_serx_t *rx,
					   vcs_se_export_t **data);

  void vcs_serx_config_tostring(char* buffer,
								unsigned int len,
								const vcs_serx_config_t* param);
								
							
   
  int vcs_serx_probe_is_supported(const vcs_serx_t *rx, vcs_probe_points_t kind);  
   
  vcs_se_probedata_t* vcs_serx_probe_data_alloc(vcs_serx_t *rx, vcs_probe_points_t kind);  
   
  void vcs_serx_probe_data_free(vcs_se_probedata_t* data);  
  
  int vcs_serx_probe_data_get(vcs_serx_t *rx, vcs_se_probedata_t* data); 
  
#ifdef __cplusplus
}
#endif

#endif 
