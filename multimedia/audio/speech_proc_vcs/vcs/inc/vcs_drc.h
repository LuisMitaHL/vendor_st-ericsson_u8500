#ifndef VCS_DRC_H_
#define VCS_DRC_H_
/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file     vcs_drc.h
 * \brief    Internal header file for voice shell DRC
 * \author   ST-Ericsson
 */
/*****************************************************************************/
#include "vcs.h"
#include "vcs_algo.h"

#ifdef __cplusplus
extern "C"
{
#endif

  /**
	 DRC instance
   */
  typedef struct vcs_drc_s vcs_drc_t;

  vcs_drc_t* vcs_drc_create(vcs_log_t* log);

  void vcs_drc_destroy(vcs_drc_t* drc);
  
  /**
	 Enables DRC. It is created, allocated and configured.
	 drc->running is 1 on success and 0 on failure. If
	 DRC is enabled and running nothing is done
	 @param drc DRC instance
	 @param mode Mode to be used when allocating DRC
	 @return VCS_OK if OK otherwise error code
   */
  int vcs_drc_enable(vcs_drc_t *drc,
					 const vcs_algo_mode_t *mode);
   /**
	  Configures DRC. If DRC is currently disabled but
	  enabled in configuration then DRC is enabled. I already
	  enabled and running it is only reconfigured with new setting.
	  If configuration is disabling the algolithm then DRC is
	  disabled and destroyed after which both enabled and running
	  will be 0 (false)
	  @param drc DRC instance
	  @param mode Mode to be used if DRC is enabled, otherwise ignored
	  @param config The configuration
	  @return VCS_OK if OK otherwise error code
   */
  
  /**
	 reset parameter with new mode
	  @param drc DRC instance
	  @param mode Mode to be used if DRC is enabled, otherwise ignored
	  @return VCS_OK if OK otherwise error code
   */
  int vcs_drc_reset(vcs_drc_t *drc,
					const vcs_algo_mode_t *mode);

  /**
	 configures drc
	 @param drc DRC instance
	 @param mode mode to use
	 @param config configuration
	 @return VCS_OK if OK otherwise error code
   */
  int vcs_drc_configure(vcs_drc_t *drc,
						const vcs_algo_mode_t *mode, 
						const vcs_drc_config_t *config);
  /**
	 Disabled DRC and destroy the instance. Handle will be NULL afterwards
	 and both enabled and running will be 0 (false)
	 @param drc DRC instance
  */
  void vcs_drc_disable(vcs_drc_t *drc);
  
  int vcs_drc_is_running(vcs_drc_t *drc);
  int vcs_drc_is_enabled(vcs_drc_t *drc);

  void vcs_drc_execute(vcs_drc_t *drc,
					   short** input,
					   int nb_input,
					   short** output,
					   int nb_output);

  void vcs_drc_config_tostring(char* buffer, unsigned int len,
							   vcs_drc_config_t *config);
  
#ifdef __cplusplus
}
#endif

#endif // VCS_ALGO_INTERNAL_H_
