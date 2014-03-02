#ifndef _VCS_CTMTX_H_
#define _VCS_CTMTX_H_
/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file     vcs_ctmtx.h
 * \brief    Internal interface for CTM uplink
 * \author   ST-Ericsson
 */
/*****************************************************************************/
#include "vcs.h"
#include "vcs_algo.h"

#ifdef VCSCTM
# include "tty.h"
#endif

#ifdef __cplusplus
extern "C"
{
#endif

  typedef struct vcs_ctmtx_s vcs_ctmtx_t;
  
  vcs_ctmtx_t* vcs_ctmtx_create(vcs_log_t *const log);
  
  void vcs_ctmtx_destroy(vcs_ctmtx_t *tx);
  int vcs_ctmtx_enable(vcs_ctmtx_t *const tx, const vcs_algo_mode_t *const mode);  
  int vcs_ctmtx_reset(vcs_ctmtx_t *const tx, const vcs_algo_mode_t *const mode);
  int vcs_ctmtx_configure(vcs_ctmtx_t *const tx, const vcs_algo_mode_t *const mode, const vcs_ctm_config_t *const param);
  void vcs_ctmtx_disable(vcs_ctmtx_t * const tx);
  int vcs_ctmtx_is_enabled(vcs_ctmtx_t *const tx);
  int vcs_ctmtx_is_running(vcs_ctmtx_t *const tx);
  void vcs_ctmtx_execute(vcs_ctmtx_t *tx, short** input, const int nb_input, short** output, const int nb_output);
  void vcs_ctmtx_export(vcs_ctmtx_t *const tx, vcs_ctmtx_export_t *const data);
  void vcs_ctmtx_import(vcs_ctmtx_t *const tx, vcs_ctmrx_export_t *const data);
  void vcs_ctmtx_config_tostring(char *const buffer, const unsigned int len, const vcs_ctm_config_t *const param);
  
#ifdef __cplusplus
}
#endif

#endif 
