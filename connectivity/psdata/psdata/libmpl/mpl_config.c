/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 * 
 * \brief  Message (m) Parameter (p) List (l) library
 */


/********************************************************************************
 *
 * Include files
 *
 ********************************************************************************/
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <syslog.h>
#include "mpl_config.h"
#include "mpl_param.h"

/********************************************************************************
 *
 * Defines & Type definitions
 *
 ********************************************************************************/
#define DBG_PRINTF(format, ...) \
  if (NULL != mpl_config_log_func) \
    (*mpl_config_log_func)(mpl_config_log_user_p, LOG_DEBUG, __FILE__, __LINE__, format, ##__VA_ARGS__)


/********************************************************************************
 *
 * Local variables
 *
 ********************************************************************************/

static mpl_log_fp mpl_config_log_func = NULL;
static void* mpl_config_log_user_p = NULL;
static int mpl_config_param_set_id = -1;

/********************************************************************************
 *
 * Private function prototypes
 *
 ********************************************************************************/

static char * mpl_config_get_line(char *s, int size, FILE *stream, int *line, char **_pos);


/*******************************************************************************
 *
 * Public Functions
 *
 *******************************************************************************/

/**
 * mpl_config_init - Initiate library
 **/
int mpl_config_init(void* user_p, mpl_log_fp log_fp, mpl_config_t *config_p, int param_set_id)
{
  mpl_config_log_user_p = user_p;
  mpl_config_log_func = log_fp;
  mpl_config_param_set_id = param_set_id;
  *config_p = NULL;
  return 0;
}

int mpl_config_read_config(char* config_path,
                           mpl_config_t *config_p,
                           int param_set_id)
{
  FILE *f;
  char buf[256], *pos;
  int line = 0,numargs,i;
  mpl_arg_t *args_p;
  mpl_param_element_t* unpackparam = NULL;

  MPL_IDENTIFIER_NOT_USED(param_set_id);

  DBG_PRINTF("Reading config from file %s\n", config_path);
  f = fopen(config_path, "r");
  if (f == NULL)
  {
    DBG_PRINTF("Open config file error, no config file loaded!\n");
    return -1;
  }
  args_p = malloc(sizeof(mpl_arg_t)*MPL_MAX_ARGS);
  if(NULL == args_p)
  {
    DBG_PRINTF("failed allocating memory!\n");
    fclose(f);
    return -1;
  }
  memset(args_p,0,sizeof(mpl_arg_t)*MPL_MAX_ARGS);
  while (mpl_config_get_line(buf, sizeof(buf), f, &line, &pos))
  {
    numargs = mpl_get_args(args_p, MPL_MAX_ARGS, buf, '=', ';', '\\');
    for(i=0;i<numargs;i++)
    {
      if (mpl_param_unpack_param_set(args_p[i].key_p, args_p[i].value_p, &unpackparam, mpl_config_param_set_id) < 0)
      {
        DBG_PRINTF("mpl_unpack_param() failed at line %d in config\n", line);
        continue;//skip the bad parameters
      }
      if (!mpl_param_allow_config(unpackparam->id))
      {
        DBG_PRINTF("Parameter %s is not configurable, check your config file\n", \
                   mpl_param_id_get_string(unpackparam->id));
        mpl_param_element_destroy(unpackparam);
        continue;//skip the parameters which should not be configured
      }
      mpl_list_add(config_p, &unpackparam->list_entry);
    }
  }
  fclose(f);
  free(args_p);
  return 0;
}

mpl_param_element_t* mpl_config_get_para(mpl_param_element_id_t param_id,
                                         mpl_config_t *config_p)
{
  mpl_param_element_t *res;

  res = mpl_param_list_find(param_id, *config_p);
  if (res == NULL)
  {
    res = mpl_param_element_get_default(param_id);
    if(res == NULL)
      return res;
    mpl_list_add(config_p,&res->list_entry);
  }
  return res;
}

mpl_param_element_t* mpl_config_tuple_key_get_para(mpl_param_element_id_t param_id,
                                                   char *key_p,
                                                   char *wildcard_p,
                                                   mpl_config_t *config_p)
{
  mpl_param_element_t *res;

  res =mpl_param_list_tuple_key_find_wildcard(param_id, key_p, wildcard_p, *config_p);
  if (res == NULL)
  {
    res = mpl_param_element_get_default(param_id);
    if(res == NULL)
      return res;
    mpl_list_add(config_p,&res->list_entry);
  }
  return res;
}


void mpl_config_reset(mpl_config_t *config_p)
{
  if(*config_p!=NULL)
    mpl_param_list_destroy(config_p);

  *config_p = NULL;
}

/*******************************************************************************
 *
 * Private Functions
 *
 *******************************************************************************/
/**
 * mpl_config_get_line()
 **/
static char * mpl_config_get_line(char *s, int size, FILE *stream, int *line, char **_pos)
{
  char *pos, *end;

  while (stream && fgets(s, size, stream)) {
    (*line)++;
    s[size - 1] = '\0';
    pos = s;
    // Skip white space from the beginning of line.
    while(isspace(*pos))
      pos++;
    // Skip comment lines and empty lines
    if(*pos == '#')
      continue;

    // Remove # comments in the end of line
    end = strchr(pos,'#');
    if(end)
      *end-- = '\0';
    else
      end = pos + strlen(pos) - 1;

    // Remove trailing white space.
    while (end > pos && isspace(*end))
      *end-- = '\0';

    if (*pos == '\0')
      continue;

    if (_pos)
      *_pos = pos;
    return pos;
  }

  if (_pos)
    *_pos = NULL;
  return NULL;
}

