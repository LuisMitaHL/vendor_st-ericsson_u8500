/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file     vcs_comfortnoise.c
 * \brief    Comfort noise
 * \author   ST-Ericsson
 */
/*****************************************************************************/
#include "vcs_comfortnoise.h"
#ifdef VCSCOMFORTNOISE
#include "comfortnoise.h"


struct vcs_comfortnoise_s {
  t_comfort_noise_handle *handle;
  t_comfort_noise_configuration_params config;
  unsigned int framesize;
  vcs_log_t* mylog;
  unsigned int running;
};

static void vcs_comfortnoise_disable(vcs_comfortnoise_t* comfortnoise);
static int vcs_comfortnoise_enable(vcs_comfortnoise_t* comfortnoise, const vcs_algo_mode_t *mode)
{
   int result = VCS_OK;
   if (!comfortnoise->running) {
     int ok = 0;
     comfortnoise->handle = comfort_noise_create();
     if (comfortnoise->handle != 0) {
	   t_comfort_noise_allocation_params alloc_param;
	   alloc_param.nof_channels = mode->nb_channels;
	   alloc_param.frame_size = mode->framesize;
	   alloc_param.sample_pattern = 0; 
	   ok = comfort_noise_allocate(&comfortnoise->handle, &alloc_param);
	 }
	 if (ok) {
	   ok = comfort_noise_setconfigure(comfortnoise->handle, &comfortnoise->config);
	 }
	 if (ok) {
	   comfort_noise_reset(comfortnoise->handle);
	   comfortnoise->running = 1;
	   comfortnoise->mylog->log(comfortnoise->mylog, VCS_DEBUG, "Comfort noise enabled\n");
	 } else {
	   comfortnoise->mylog->log(comfortnoise->mylog, VCS_ERROR, "Failure create Comfort noise handle\n");
	 }
   } else {
      int ok = comfort_noise_setconfigure(comfortnoise->handle, &comfortnoise->config);
	  if (ok) {
        comfort_noise_reset(comfortnoise->handle);
        comfortnoise->mylog->log(comfortnoise->mylog, VCS_DEBUG, "Comfort noise reconfigured\n");
	  } else {
	    vcs_comfortnoise_disable(comfortnoise);
		comfortnoise->mylog->log(comfortnoise->mylog, VCS_ERROR, "Comfort noise reconfigured failed\n");
	  }
   }
   return result;
}

static void vcs_comfortnoise_disable(vcs_comfortnoise_t* comfortnoise) 
{
  if (comfortnoise->handle != 0) {
        comfort_noise_destroy(&comfortnoise->handle);
        comfortnoise->handle = 0;
        comfortnoise->mylog->log(comfortnoise->mylog, VCS_DEBUG, "Comfort noise disabled\n");
   }
   comfortnoise->running = 0;
}

vcs_comfortnoise_t* vcs_comfortnoise_create(vcs_log_t* log)
{
  struct vcs_comfortnoise_s *instance = (struct vcs_comfortnoise_s*) calloc(1, sizeof(struct vcs_comfortnoise_s));
  if (instance != 0) {
    instance->mylog = log;
    // rest is 0 
  } else {
    log->log(log, VCS_ERROR, "Failure creating Comfort noise\n");
  }
  return instance;
}

void vcs_comfortnoise_destroy(vcs_comfortnoise_t* comfortnoise)
{
   if (comfortnoise != 0) {
     vcs_comfortnoise_disable(comfortnoise);
     free(comfortnoise);
   }
}
 
int vcs_comfortnoise_config(vcs_comfortnoise_t* comfortnoise, 
                              const vcs_algo_mode_t *mode, 
                              const vcs_comfortnoise_config_t* config)
{
  
   comfortnoise->config.level = config->level; 
   comfortnoise->config.enable = config->enabled;
   comfortnoise->framesize =  mode->framesize;
   if (config->enabled) {
     if (comfortnoise->running)  {
	   vcs_comfortnoise_disable(comfortnoise);
	 }
     (void) vcs_comfortnoise_enable(comfortnoise, mode);
      
   }
   return VCS_OK;
}
                             
void vcs_comfortnoise_reset(vcs_comfortnoise_t* comfortnoise, 
                             const vcs_algo_mode_t *mode)
{
   comfortnoise->framesize =  mode->framesize;
   if (comfortnoise->running) {
     vcs_comfortnoise_disable(comfortnoise);
     (void) vcs_comfortnoise_enable(comfortnoise, mode);
   } 
}
                             
void vcs_comfortnoise_execute(vcs_comfortnoise_t* comfortnoise,
                                short** input,
					            int nb_input,
					            short** output,
					            int nb_output)
{
  comfort_noise_process(comfortnoise->handle, input[0], output[0], comfortnoise->framesize);
}
                               
int vcs_comfortnoise_is_running(vcs_comfortnoise_t* comfortnoise)
{
  return (comfortnoise->running);
}
                           
#else

struct vcs_comfortnoise_s {
  vcs_log_t* log;
  int running;
};

vcs_comfortnoise_t* vcs_comfortnoise_create(vcs_log_t* log)
{
  struct vcs_comfortnoise_s *instance = (struct vcs_comfortnoise_s*) calloc(1, sizeof(struct vcs_comfortnoise_s));
  if (instance != 0) {
    instance->log = log;
  }
  return instance;
}

void vcs_comfortnoise_destroy(vcs_comfortnoise_t* comfortnoise)
{
  if (comfortnoise != 0) free(comfortnoise);
}
 
int vcs_comfortnoise_config(vcs_comfortnoise_t* comfortnoise, 
                              const vcs_algo_mode_t *mode, 
                              const vcs_comfortnoise_config_t* config)
{
  return VCS_OK;
}
                             
void vcs_comfortnoise_reset(vcs_comfortnoise_t* comfortnoise, 
                             const vcs_algo_mode_t *mode)
{
}

int vcs_comfortnoise_is_running(vcs_comfortnoise_t* comfortnoise)
{
  return (comfortnoise->running);
}
                             
void vcs_comfortnoise_execute(vcs_comfortnoise_t* comfortnoise,
                                short** input,
					            int nb_input,
					            short** output,
					            int nb_output)
{
}

#endif // VCSCOMFORTNOISE