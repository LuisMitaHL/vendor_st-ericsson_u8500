/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \brief  PS Routing Control run script
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
#include <assert.h>
#include "stercd.h"
#include "sterc_runscript.h"
#include "sterc_msg.h"

/********************************************************************************
 *
 * Defines & Type definitions
 *
 ********************************************************************************/

/* Runscript paths for PS-DUN */
#define STERC_DUN_RUNSCRIPT_PATH_CONNECT    "/data/misc/psdata/sterc_script_connect_dun"
#define STERC_DUN_RUNSCRIPT_PATH_DISCONNECT "/data/misc/psdata/sterc_script_disconnect_dun"

/* Routing table name for PS-DUN */
#define DUN_RT_TABLE_NAME "PS_DUN"

/********************************************************************************
 *
 * Global variables
 *
 ********************************************************************************/
char* sterc_runscript_path_connect = NULL;
char* sterc_runscript_path_disconnect = NULL;

/********************************************************************************
 *
 * Local variables
 *
 ********************************************************************************/

/********************************************************************************
 *
 * Local variables
 *
 ********************************************************************************/

static const int runscript_parameters[] =
{
  sterc_paramid_lan_own_ip,
  sterc_paramid_lan_device,
  sterc_paramid_wan_ip,
  sterc_paramid_wan_dns,
  sterc_paramid_wan_device
};

static const int runscript_dnsmasq_parameters[] =
{
  sterc_paramid_lan_netmask,
  sterc_paramid_lan_dhcp_range_first,
  sterc_paramid_lan_dhcp_range_last,
  sterc_paramid_lan_dhcp_lease
};


/********************************************************************************
 *
 * Private function prototypes
 *
 ********************************************************************************/
static int exec_script(char *const *argv, char *const *env);
static void *create_env_rt_table(int parentid,
                                 sterc_get_param_fp_t get_param_cb);
static void* create_env_dnsmasq_config_file(int parentid,
                                            sterc_get_param_fp_t get_param_cb);
/*******************************************************************************
 *
 * Public Functions
 *
 *******************************************************************************/
/**
 * sterc_runscript_call()
 **/
int sterc_runscript_call(int parentid,
                         sterc_get_param_fp_t get_param_cb,
                         sterc_handler_state_t state)
{
  char *argv[2] = {NULL,NULL};
  char **env = NULL;
  int elem_size;
  int env_idx = 0;
  sterc_paramid_t id;
  mpl_param_element_t* param_elem_p;
  pid_t pid;
  int status = 0;
  int i;
  int number_of_params;
  int number_of_dnsmasq_params;
  int number_of_envs;
  bool create_routing_table = false;
  bool create_dnsmasq_config_file = false;
  char *lan_device;
  sterc_mode_t mode;

  //First get mode, so that we know what to configure
  param_elem_p = get_param_cb(parentid, sterc_paramid_mode, NULL);
  if ((NULL == param_elem_p) ||
      (NULL == param_elem_p->value_p))
  {
    STERC_DBG_TRACE(LOG_WARNING, "could not fetch mode\n");
    return -1;
  }
  mode = *((sterc_mode_t*)param_elem_p->value_p);

  if (sterc_mode_usb_eth_nap == mode) {
    //NAP
    switch (state)
    {
     case sterc_handler_connected:
      argv[0] = sterc_runscript_path_connect;
      break;
    case sterc_handler_disconnecting:
      argv[0] = sterc_runscript_path_disconnect;
      break;
    case sterc_handler_idle:
    case sterc_handler_created:
    case sterc_handler_connecting:
    default:
      return 0;
      break;
    }

    create_dnsmasq_config_file = true;
  }
  else if (sterc_mode_psdun == mode) {
    //DUN
    switch (state)
    {
     case sterc_handler_connected:
      argv[0] = STERC_DUN_RUNSCRIPT_PATH_CONNECT;
      break;
    case sterc_handler_disconnecting:
      argv[0] = STERC_DUN_RUNSCRIPT_PATH_DISCONNECT;
      break;
    case sterc_handler_idle:
    case sterc_handler_created:
    case sterc_handler_connecting:
    default:
      return 0;
      break;
    }

    create_routing_table = true;
  }
  else {
    STERC_DBG_TRACE(LOG_ERR, "Invalid mode: %d.\n", mode);
    return -1;
  }


  //Just fetch all parameters
  number_of_params = (sizeof(runscript_parameters)/sizeof(int));
  number_of_dnsmasq_params = (sizeof(runscript_dnsmasq_parameters)/sizeof(int));
  number_of_envs = number_of_params + number_of_dnsmasq_params + 5; /* status + ppp_config + rt_table + dnsmasq_config + NULL */
  env = malloc(sizeof(char *) * (number_of_envs));
  if(NULL == env)
  {
    STERC_DBG_TRACE(LOG_ERR, "malloc failed\n");
    return(-1);
  }

  if (create_routing_table)
  {
    /* rt table name environment variable */
    assert(env_idx < number_of_envs);
    env[env_idx] = create_env_rt_table(parentid, get_param_cb);
    if (NULL != env[env_idx])
      env_idx++;
  }

  if (create_dnsmasq_config_file) {
    /* rt table name environment variable */
    assert(env_idx < number_of_envs);
    env[env_idx] = create_env_dnsmasq_config_file(parentid, get_param_cb);
    if (NULL != env[env_idx])
      env_idx++;
  }

  //Then fetch all other parameters
  for(i=0; i<number_of_params; i++)
  {
    id=runscript_parameters[i];
    param_elem_p = (*get_param_cb)(parentid, id,NULL);
    if ((NULL == param_elem_p) ||
        (NULL == param_elem_p->value_p))
    {
      continue;
    }
    if (sterc_paramid_wan_dns == id) {
      mpl_param_element_t* tmp_elem_p = param_elem_p;
      int my_idx = 0;

      elem_size = strlen("dns_addresses=");
      do {
        elem_size += snprintf(NULL, 0, "%s ", mpl_param_value_get_string(id, tmp_elem_p->value_p));
      } while (NULL != (tmp_elem_p = mpl_param_list_find_next(id, tmp_elem_p)));
      elem_size++;
      env[env_idx] = malloc(elem_size);
      if (NULL == env[env_idx]) {
        STERC_DBG_TRACE(LOG_ERR, "malloc failed\n");
        continue;
      }

      tmp_elem_p = param_elem_p;
      strcpy(&env[env_idx][my_idx], "dns_addresses=");
      my_idx += strlen("dns_addresses=");
      do {
        my_idx += snprintf(&env[env_idx][my_idx], elem_size - my_idx, "%s ",
            mpl_param_value_get_string(sterc_paramid_wan_dns, tmp_elem_p->value_p));
      } while (NULL != (tmp_elem_p = mpl_param_list_find_next(sterc_paramid_wan_dns, tmp_elem_p)));
      env[env_idx][my_idx++] = '\0';

    } else {
      elem_size = mpl_param_pack_no_prefix(param_elem_p, NULL, 0);
      if (elem_size < 0) {
        STERC_DBG_TRACE(LOG_ERR, "param_pack failed\n");
        continue;
      }
      elem_size++;

      assert(env_idx < number_of_envs);
      env[env_idx] = malloc(elem_size);
      if (NULL == env[env_idx]) {
        STERC_DBG_TRACE(LOG_ERR, "malloc failed\n");
        continue;
      }

      elem_size = mpl_param_pack_no_prefix(param_elem_p, env[env_idx], elem_size);
      if (elem_size < 0) {
        STERC_DBG_TRACE(LOG_ERR, "param_pack failed\n");
        free(env[env_idx]);
        env[env_idx] = NULL;
        continue;
      }
    }
    env_idx++;
  }

  //Then fetch all dnsmasq parameters
  if (create_dnsmasq_config_file) {
    param_elem_p = (*get_param_cb)(parentid, sterc_paramid_lan_device, NULL);
    if ((NULL == param_elem_p) || (NULL == param_elem_p->value_p)) {
      status = -1;
      env[env_idx] = NULL;
      goto cleanup;
    }
    lan_device = (char*) param_elem_p->value_p;

    for (i = 0; i < number_of_dnsmasq_params; i++) {
      id = runscript_dnsmasq_parameters[i];
      param_elem_p = (*get_param_cb)(parentid, id, lan_device);
      if ((NULL == param_elem_p) || (NULL == param_elem_p->value_p)) {
        continue;
      }

      elem_size = mpl_param_pack_no_prefix(param_elem_p, NULL, 0);
      if (elem_size < 0) {
        STERC_DBG_TRACE(LOG_ERR, "param_pack failed\n");
        continue;
      }
      elem_size++;

      assert(env_idx < number_of_envs);
      env[env_idx] = malloc(elem_size);
      if (NULL == env[env_idx]) {
        STERC_DBG_TRACE(LOG_ERR, "malloc failed\n");
        continue;
      }

      elem_size = mpl_param_pack_no_prefix(param_elem_p, env[env_idx], elem_size);
      if (elem_size < 0) {
        STERC_DBG_TRACE(LOG_ERR, "param_pack failed\n");
        free(env[env_idx]);
        env[env_idx] = NULL;
        continue;
      }

      env_idx++;
    }
  }

  assert(env_idx < number_of_envs);
  env[env_idx] = NULL;

  //Call Runscript
  pid = exec_script(argv, env);
  STERC_DBG_TRACE(LOG_DEBUG, "Pid was %d\n", pid);
  if (pid == -1)
    status = -1;
  else if (pid != 0) {
    /* Wait for the script to finish */
    while (waitpid(pid, &status, 0) == -1) {
      if (errno != EINTR) {
        STERC_DBG_TRACE(LOG_ERR, "waitpid: %s", strerror(errno));
        status = -1;
        break;
      }
    }
  }

cleanup:
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
  int pipefd[2];

  if (pipe(pipefd) < 0) {
    STERC_DBG_TRACE(LOG_ERR, "pipe failed (%s)", strerror(errno));
    return -1;
  }

  switch (pid = vfork()) {
  case -1:
    STERC_DBG_TRACE(LOG_ERR, "vfork: %s\n", strerror(errno));
    close(pipefd[0]);
    close(pipefd[1]);
    break;
  case 0:
    close(pipefd[1]);
    if (pipefd[0] != STDIN_FILENO) {
      if (dup2(pipefd[0], STDIN_FILENO) != STDIN_FILENO) {
        STERC_DBG_TRACE(LOG_ERR, "dup2 failed (%s)", strerror(errno));
        return -1;
      }
      close(pipefd[0]);
    }

    execve(argv[0], argv, env);
    STERC_DBG_TRACE(LOG_DEBUG, "%s: %s\n", argv[0], strerror(errno));
    _exit(127);
    /* NOTREACHED */
  }
  return pid;
}

/**
 * create_env_rt_table()
 **/
static void *create_env_rt_table(int parentid, sterc_get_param_fp_t get_param_cb)
{
  size_t elem_size;
  char *res;

  elem_size = snprintf(NULL, 0, "rt_table_name=%s", DUN_RT_TABLE_NAME);
  elem_size++;

  res = malloc(elem_size);
  if(NULL == res) {
    STERC_DBG_TRACE(LOG_ERR, "malloc failed\n");
    return NULL;
  }

  elem_size = snprintf(res, elem_size, "rt_table_name=%s", DUN_RT_TABLE_NAME);
  res[elem_size]='\0';

  return res;
}

static void* create_env_dnsmasq_config_file(int parentid,
                                            sterc_get_param_fp_t get_param_cb)
{
  size_t elem_size;
  char *res = NULL;
  mpl_param_element_t* param_elem_p;
  char* lan_device;

  //First fetch the parameters
  param_elem_p = (*get_param_cb)(parentid, sterc_paramid_lan_device,NULL);
  if ((NULL == param_elem_p) ||
      (NULL == param_elem_p->value_p))
  {
    return NULL;
  }
  lan_device = (char*)param_elem_p->value_p;

  elem_size = snprintf(NULL, 0,
                       "dnsmasq_config_file=/data/misc/psdata/dnsmasq.conf");
  elem_size++;

  res = malloc(elem_size);
  if(NULL == res)
  {
    STERC_DBG_TRACE(LOG_ERR, "malloc failed\n");
    return NULL;
  }
  elem_size = snprintf(res, elem_size,"dnsmasq_config_file=/data/misc/psdata/dnsmasq.conf");
  res[elem_size]='\0';

  return res;
}

