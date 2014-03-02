#ifndef _VCS_COMFORTNOISE_H_
#define _VCS_COMFORTNOISE_H_
/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file     vcs_comfortnoise.h
 * \brief    Interface file for algorithm comfort noise
 * \author   ST-Ericsson
 */
/*****************************************************************************/
#include "vcs_algo.h"

#ifdef __cplusplus
extern "C"
{
#endif
  
  typedef struct vcs_comfortnoise_s vcs_comfortnoise_t;

  vcs_comfortnoise_t* vcs_comfortnoise_create(vcs_log_t* log);

  void vcs_comfortnoise_destroy(vcs_comfortnoise_t* comfortnoise);
 
  int vcs_comfortnoise_config(vcs_comfortnoise_t* comfortnoise, 
                              const vcs_algo_mode_t *mode, 
                              const vcs_comfortnoise_config_t* config);
                             
  void vcs_comfortnoise_reset(vcs_comfortnoise_t* comfortnoise, 
                             const vcs_algo_mode_t *mode);
                             
  int vcs_comfortnoise_is_running(vcs_comfortnoise_t* comfortnoise);
                             
  void vcs_comfortnoise_execute(vcs_comfortnoise_t* comfortnoise,
                                short** input,
					            int nb_input,
					            short** output,
					            int nb_output);
                             

#ifdef __cplusplus
}
#endif

#endif //_VCS_COMFORTNOISE_H_