/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file     vcs_drc
 * \brief    DRC
 * \author   ST-Ericsson
 */
/*****************************************************************************/
#include "vcs.h"
#include "vcs_drc.h"
#include <string.h>
#include <stdio.h>

struct vcs_drc_s
{
#ifdef DRC
  t_drc_handle               *handle; 
  t_drc_configuration_params config;
  vcs_log_t                  *mylog;
#endif
  int enabled;
  int running;
};

#ifdef DRC
/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
vcs_drc_t* vcs_drc_create(vcs_log_t *log)
{
  vcs_drc_t* drc =(vcs_drc_t*) calloc(1, sizeof(vcs_drc_t));

  if (drc != NULL)
    drc->mylog = log;
  else 
    log->log(log, VCS_ERROR, "Failure creating DRC\n");
  return drc;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
void vcs_drc_destroy(vcs_drc_t* drc)
{
  if (drc != NULL)
  {
    vcs_drc_disable(drc);
    free(drc);
  }
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
static int vcs_drc_enable2(vcs_drc_t *drc, const vcs_algo_mode_t *mode, const vcs_drc_config_t *config)
{
  int result = VCS_OK;
  /* set algorithm as enabled. If not running then create and
	 allocate DRC resosurce. If OK set it as running, otherwise
	 as not running
   */
  drc->enabled = 1;
  if (!drc->running) {
	t_bool drcresult;
	t_drc_allocation_params alloc_param;
	drc->handle = drc_create();
	if (drc->handle != 0) {
	  alloc_param.nof_channels = mode->nb_channels;
	  alloc_param.sample_rate = mode->samplerate;
	  alloc_param.frame_size = mode->framesize;
	  alloc_param.sample_pattern = mode->interleaved;
	  
	  drcresult = drc_allocate(&drc->handle, &alloc_param);
	  if (drcresult == 0) drc->mylog->log(drc->mylog, VCS_ERROR, "Failure allocate DRC\n");
	  else {
		t_drc_param param;
		param.type = DRC_PARAMETER_TYPE_ALL_CONFIG_PARAMS;
		param.param.all_config_params = config == 0 ? drc->config : config->config_param;
		drcresult  = drc_setparam(drc->handle,
								  &param);
		if (drcresult == 0) drc->mylog->log(drc->mylog, VCS_ERROR, "Failure setparam DRC\n");
	  }
	  if (drcresult) {
		drc->running = 1;
		drc_reset(drc->handle);
		result = VCS_OK;
		drc->mylog->log(drc->mylog, VCS_DEBUG, "DRC enabled\n");
	  } else {
		drc_destroy(&drc->handle);
		drc->handle = 0;
		drc->running = 0;
		result = VCS_ERROR_ALG;
	  }
	} else {
	  drc->mylog->log(drc->mylog, VCS_ERROR, "Failure create DRC\n");
	}
  }
  return VCS_OK;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
int vcs_drc_enable(vcs_drc_t *drc, const vcs_algo_mode_t *mode)
{
  return vcs_drc_enable2(drc, mode, 0);
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
void vcs_drc_disable(vcs_drc_t *drc)
{
  if (drc->handle != 0)
  {
    drc_destroy(&drc->handle);
    drc->handle = 0;
  }

  drc->running = 0;
  drc->enabled = 0;
  drc->mylog->log(drc->mylog, VCS_DEBUG, "DRC disabled\n");
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
int vcs_drc_configure(vcs_drc_t *drc, const vcs_algo_mode_t *mode,  const vcs_drc_config_t *config)
{
  int result = VCS_OK;
  if (config->enabled)
  {
    drc->config = config->config_param;
    
    if (!drc->running)
    {
      result = vcs_drc_enable2(drc, mode, config); 
    }
    else
    {
      t_drc_param param;
      param.type = DRC_PARAMETER_TYPE_ALL_CONFIG_PARAMS;
      param.param.all_config_params = drc->config;
      int drcresult  = drc_setparam(drc->handle, &param);
	
      if (!drcresult)
      {
        drc->mylog->log(drc->mylog, VCS_ERROR, "Failure in setparam for DRC\n");
      }
      else
      {
        drc->mylog->log(drc->mylog, VCS_DEBUG, "DRC reconfigured\n");
      }
      result = VCS_OK;
    }
  }
  else
  {
    vcs_drc_disable(drc);
  }
  return result;
}


/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
int vcs_drc_reset(vcs_drc_t *drc, const vcs_algo_mode_t *mode)
{
  if (drc->running)
  {
    vcs_drc_disable(drc);
    return vcs_drc_enable(drc, mode);
  } 
  else 
    return VCS_OK;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
int vcs_drc_is_enabled(vcs_drc_t *drc)
{
  if (drc == 0) 
    return 0;
  else
    return drc->enabled;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
int vcs_drc_is_running(vcs_drc_t *drc)
{
  if (drc == 0) return 0;
  return drc->running;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
void vcs_drc_execute(vcs_drc_t *drc, short** input, int nb_input, short** output, int nb_output)
{
  drc_execute(drc->handle, input[0], output[0], 0);
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
void vcs_drc_config_tostring(char* buffer, unsigned int len, vcs_drc_config_t *config)
 {
   int pos = 0;
   int i;
   pos += snprintf(buffer +pos, len-pos, "%d,drc{{", config->enabled);
   
   for (i = 0; i < DRC_NUMBER_OF_STATIC_CURVE_POINTS; i++) 
   {
     if (i < DRC_NUMBER_OF_STATIC_CURVE_POINTS -1)
       pos += snprintf(buffer +pos, len-pos, "[%d,%d]",
					   config->config_param.static_curve.static_curve_point[i].x,
					   config->config_param.static_curve.static_curve_point[i].y);
    else
     pos += snprintf(buffer +pos, len-pos, "[%d,%d]},",
					   config->config_param.static_curve.static_curve_point[i].x,
					   config->config_param.static_curve.static_curve_point[i].y);
   }
   pos += snprintf(buffer +pos, len-pos, "%d,", config->config_param.static_curve.input_gain);
   pos += snprintf(buffer +pos, len-pos, "%d,", config->config_param.static_curve.offset_gain);
   pos += snprintf(buffer +pos, len-pos, "%d,", config->config_param.time_specifiers.level_detector_attack_time);
   pos += snprintf(buffer +pos, len-pos, "%d,", config->config_param.time_specifiers.level_detector_release_time);
   pos += snprintf(buffer +pos, len-pos, "%d,", config->config_param.time_specifiers.gain_processor_attack_time);
   pos += snprintf(buffer +pos, len-pos, "%d,", config->config_param.time_specifiers.gain_processor_release_time);
   pos += snprintf(buffer +pos, len-pos, "%d,", config->config_param.time_specifiers.acceptance_level); 
}

#else

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
vcs_drc_t* vcs_drc_create( vcs_log_t *log)
{
  vcs_drc_t* drc =(vcs_drc_t*) calloc(1, sizeof(vcs_drc_t));

  if (drc != NULL)
    drc->mylog = log;
  else 
    log->log(log, VCS_ERROR, "Failure creating DRC\n");
  return drc;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
void vcs_drc_destroy(vcs_drc_t* drc)
{
  free(drc);
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
void vcs_drc_param(vcs_drc_t *drc,const vcs_drc_param_t *param)
{
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
int vcs_drc_reset(vcs_drc_t *drc, const vcs_algo_mode_t *mode)
{
  return 0;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
void vcs_drc_disable(vcs_drc_t *drc)
{
  return 0;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
int vcs_drc_is_enabled(vcs_drc_t *drc)
{
  return 0;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
int vcs_drc_is_running(vcs_drc_t *drc)
{
  return 0;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
void vcs_drc_execute(vcs_drc_t *drc, short** input, int nb_input, short** output, int nb_output)
{
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
int vcs_drc_configure(vcs_drc_t *drc, const vcs_algo_mode_t *mode, const vcs_drc_config_t *config)
{
  return 0;
}

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
void vcs_drc_config_tostring(char* buffer, unsigned int len, vcs_drc_config_t *config)
{
  buffer[0]='\0';
}

#endif // DRC


