/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 * 
 * \brief  PS Connection Control run script
 */


/********************************************************************************
 *
 * Include files
 *
 ********************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include "psccd.h"
#include "pscc_object.h"
#include "pscc_handler.h"
#include "pscc_runscript.h"

/********************************************************************************
 *
 * Defines & Type definitions
 *
 ********************************************************************************/

/********************************************************************************
 *
 * Global variables
 *
 ********************************************************************************/
char* pscc_runscript_path = NULL;

/********************************************************************************
 *
 * Local variables
 *
 ********************************************************************************/

static const int pscc_runscript_parameters[] =
{
  pscc_paramid_dns_address,
  pscc_paramid_own_ip_address,
  pscc_paramid_gw_address,
  pscc_paramid_netdev_name
};
  
/********************************************************************************
 *
 * Private function prototypes
 *
 ********************************************************************************/
static int exec_script(char *const *argv, char *const *env);

/*******************************************************************************
 *
 * Public Functions
 *
 *******************************************************************************/
/**
 * pscc_runscript_call()
 **/
int pscc_runscript_call(pscc_object_t*obj)
{
  char *const argv[2] = {pscc_runscript_path,
                         NULL };
  char **env = NULL;
  int elem_size;
  int env_idx = 0;
  pscc_paramid_t id;
  mpl_param_element_t* param_elem_p;
  pid_t pid;
  int status = 0;
  int i,number_of_params;
  int number_of_envs;
  

  if (NULL == pscc_runscript_path)
    return 0;

  number_of_params = (sizeof(pscc_runscript_parameters)/sizeof(int));
  number_of_envs = (number_of_params + 2);//2 here means the first element is for connection status
                                          // and the last one is for "/0"

  env = malloc(sizeof(char *) * number_of_envs);
  if(NULL == env)
  {
    PSCC_DBG_TRACE(LOG_ERR, "malloc failed\n");
    return(-1);
  }

  elem_size = strlen("connection_status") + strlen(pscc_names_connection_status[pscc_obj_get_state(obj)]) + 2;
  env[env_idx] = malloc(elem_size);
  if(NULL == env[env_idx])
  {
    PSCC_DBG_TRACE(LOG_ERR, "malloc failed\n");
    free(env);
    return(-1);
  }

  snprintf(env[env_idx], elem_size, "connection_status=%s", pscc_names_connection_status[pscc_obj_get_state(obj)]);
  env_idx++;

  for(i=0;i<number_of_params;i++)
  {
    id=pscc_runscript_parameters[i];
    param_elem_p = pscc_get_param(obj->connid, id);
    if (NULL == param_elem_p)
    {
      continue;
    }

    switch (id)
    {
    case pscc_paramid_dns_address:
    {
      mpl_param_element_t* tmp_elem_p = param_elem_p;
      int my_idx = 0;
        
      elem_size = strlen("dns_addresses=");
      do 
      {
        elem_size += snprintf(NULL, 0, "%s ",
                              mpl_param_value_get_string(id, tmp_elem_p->value_p));
      } while (NULL != (tmp_elem_p = mpl_param_list_find_next(id, tmp_elem_p)));
      elem_size++;
      env[env_idx] = malloc(elem_size);
      if(NULL == env[env_idx])
      {
        PSCC_DBG_TRACE(LOG_ERR, "malloc failed\n");
        continue;
      }
      
      tmp_elem_p = param_elem_p;
      strcpy(&env[env_idx][my_idx], "dns_addresses=");
      my_idx += strlen("dns_addresses=");
      do 
      {
        my_idx += snprintf(&env[env_idx][my_idx], elem_size-my_idx, "%s ",
                           mpl_param_value_get_string(pscc_paramid_dns_address,
                                                      tmp_elem_p->value_p));
      } while (NULL != (tmp_elem_p = mpl_param_list_find_next(pscc_paramid_dns_address, tmp_elem_p)));
      env[env_idx][my_idx++]='\0';
      break;
    }

    default:
      elem_size = mpl_param_pack(param_elem_p, NULL, 0);
      if (elem_size < 0)
      {
        PSCC_DBG_TRACE(LOG_ERR, "param_pack failed\n");
        continue;
      }
      elem_size++;
      
      env[env_idx] = malloc(elem_size);
      if(NULL == env[env_idx])
      {
        PSCC_DBG_TRACE(LOG_ERR, "malloc failed\n");
        continue;
      }

      elem_size = mpl_param_pack_no_prefix(param_elem_p, env[env_idx], elem_size);
      if (elem_size < 0)
      {
        PSCC_DBG_TRACE(LOG_ERR, "param_pack failed\n");
        free(env[env_idx]);
        env[env_idx] = NULL;
continue;
      }
      break;
    }

    env_idx++;
  }

  env[env_idx] = NULL;

  pid = exec_script(argv, env);
  if (pid == -1)
    status = -1;
  else if (pid != 0) {
    /* Wait for the script to finish */
    while (waitpid(pid, &status, 0) == -1) {
      if (errno != EINTR) {
        PSCC_DBG_TRACE(LOG_ERR, "waitpid: %s", strerror(errno));
        status = -1;
        break;
      }
    }
  }

  /* Cleanup */
  env_idx = 0;
  while (env[env_idx] != NULL)
    free(env[env_idx++]);
  free(env);
  return status;
}

/*******************************************************************************
 *
 * Private Functions
 *
 *******************************************************************************/
/**
 * exec_script()
 **/
static int exec_script(char *const *argv, char *const *env)
{
  pid_t pid;

  switch (pid = vfork()) {
  case -1:
    PSCC_DBG_TRACE(LOG_ERR, "vfork: %s\n", strerror(errno));
    break;
  case 0:
    execve(argv[0], argv, env);
    PSCC_DBG_TRACE(LOG_ERR, "%s: %s\n", argv[0], strerror(errno));
    _exit(127);
    /* NOTREACHED */
  }
  return pid;
}
