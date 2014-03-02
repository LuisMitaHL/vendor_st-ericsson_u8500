#ifndef _VCS_CTMRX_H_
#define _VCS_CTMRX_H_
/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file     vcs_ctmrx.h
 * \brief    Internal interface for CTM
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

  typedef struct vcs_ctmrx_s vcs_ctmrx_t;
 
  vcs_ctmrx_t* vcs_ctmrx_create(vcs_log_t *const log);
  
  void vcs_ctmrx_destroy(vcs_ctmrx_t *tx);
  int vcs_ctmrx_enable(vcs_ctmrx_t *const tx, const vcs_algo_mode_t *const mode);
  int vcs_ctmrx_reset(vcs_ctmrx_t *const tx, const vcs_algo_mode_t *const mode);
  int vcs_ctmrx_configure(vcs_ctmrx_t *const tx, const vcs_algo_mode_t *const mode, const vcs_ctm_config_t *const param);
  void vcs_ctmrx_disable(vcs_ctmrx_t *const tx);
  int vcs_ctmrx_is_enabled(vcs_ctmrx_t *const tx);
  int vcs_ctmrx_is_running(vcs_ctmrx_t *const tx);
  void vcs_ctmrx_execute(vcs_ctmrx_t *const tx, short** input, const int nb_input, short** output, const int nb_output);
  void vcs_ctmrx_export(vcs_ctmrx_t *const ctmrx, vcs_ctmrx_export_t  *const data);
  void vcs_ctmrx_import(vcs_ctmrx_t *const ctmrx, vcs_ctmtx_export_t  *const data);
  void vcs_ctmrx_config_tostring(char *const buffer, const unsigned int len, const vcs_ctm_config_t *const param);
  
#ifdef __cplusplus
}
#endif
#endif
