/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */
/*****************************************************************************/
#include "vcs_plugin.h"
#include <dlfcn.h>
#include <stdlib.h>

struct vcs_plugin_s {
  void *handle; 
  vcs_log_t *log;
};

#define CUSTOMER_LIBRARY "libspeech_proc_customer_plugin.so"

const char* plugin_factories[]= {"speech_proc_rx1_factory", 
"speech_proc_rx2_factory", "speech_proc_tx2_factory", ""};

static void my_dummy_set_config(struct vcs_algorithm_s* algo,
					  const vcs_algorith_mode_t *mode,
                      const vcs_algorithm_config_t* config)
{
}

static void my_dummy_reset(struct vcs_algorithm_s* algo, const vcs_algorith_mode_t *mode)
{
}

static int my_is_running(struct vcs_algorithm_s* algo)
{
  return 0;
}

static void my_dummy_execute(struct vcs_algorithm_s* algo,
				   short **input,
				   int nb_input,
				   short **output,
				   int nb_output)
{
}

static void my_dummy_destroy(struct vcs_algorithm_s* algo)
{
  free(algo);
}  
 
static vcs_algorithm_t* my_dummy_create_factory(void) 
{
  vcs_algorithm_t* plugin = (vcs_algorithm_t*) malloc(sizeof(vcs_algorithm_t));
  if (plugin != 0) {
    plugin->set_config = my_dummy_set_config;
    plugin->reset = my_dummy_reset;
    plugin->execute = my_dummy_execute;
    plugin->destroy = my_dummy_destroy;
    plugin->is_running = my_is_running;
  }
  return plugin;
}

vcs_plugin_t* vcs_plugin_init(vcs_log_t *log)
{
  struct vcs_plugin_s* plugin = (struct vcs_plugin_s*) malloc(sizeof(struct vcs_plugin_s));
  if (plugin != 0) {
    plugin->handle = dlopen(CUSTOMER_LIBRARY, RTLD_LAZY);
    if (plugin->handle != 0) log->log(log, VCS_DEBUG, "customer library %s found", CUSTOMER_LIBRARY);
    else log->log(log, VCS_DEBUG, "customer library %s not found", CUSTOMER_LIBRARY);
    plugin->log = log;
  }
  return plugin;
}

void vcs_plugin_destroy(vcs_plugin_t* plugin)
{
  if (plugin != 0) {
    if (plugin->handle != 0) dlclose(plugin->handle);
    free(plugin);
  }
}

vcs_algorithm_t* vcs_plugin_get(vcs_plugin_t* self, 
                                vcs_plugin_types_t kind)
{
  vcs_algorithm_t* result = 0;
  if (self != 0) {
    vcs_algorithm_factory factory = 0;
    char *error;
    if (self->handle != 0) {
      dlerror();
      factory = dlsym(self->handle, plugin_factories[kind]);
      if ((error = dlerror()) != NULL) {
        self->log->log(self->log, VCS_WARNING, "dlsym error %s", error);
      }
    }
    if (factory == 0) {
       self->log->log(self->log, VCS_DEBUG, "customer %s missing, using default implementation", plugin_factories[kind]);
       result = my_dummy_create_factory();
    } else {
      result = factory();
      self->log->log(self->log, VCS_DEBUG, "customer %s used", plugin_factories[kind]);
    }
  } 
  return result;
}

void vcs_to_algorithm_mode(vcs_algorith_mode_t *algomode,
					       const vcs_mode_t *mode,
					       unsigned int nb_channels,
					       unsigned int interleaved)
{
  algomode->resolution   = mode->resolution;
  algomode->samplerate   = mode->samplerate;
  algomode->framesize    = mode->framesize;
  algomode->interleaved  = interleaved;
  algomode->nb_channels  = nb_channels;
}
                      
                      
