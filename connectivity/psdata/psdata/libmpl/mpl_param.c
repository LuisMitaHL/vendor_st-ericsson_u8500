/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 * 
 * \brief  Generic parameters
 */


/********************************************************************************
 *
 * Include files
 *
 ********************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <stdarg.h>
#include <syslog.h>
#include <ctype.h>
#include <stdint.h>
#include <inttypes.h>
#include <limits.h>
#include <string.h>
#include "mpl_param.h"


/********************************************************************************
 *
 * Defines & Type definitions
 *
 ********************************************************************************/
    /* So far we only use level LOG_DEBUG from this library.
     * The arguments are the following:
     * - It is caller of the lib that has to find out if a failure is really critical or not
     *   I.e. has ta add another 'level' of logging
     * - We do not want to populate the log file with lots of errors/warnings
     *   that is really unimportant.
     */
#define DBG_PRINTF(format, ...) \
  if (NULL != mpl_param_log_func) \
    (*mpl_param_log_func)(mpl_param_log_user_p, LOG_DEBUG, __FILE__, __LINE__, format, ##__VA_ARGS__)

#define PARAM_SET_SIZE(param_descr_p)                                   \
  (param_descr_p)->paramid_end - MPL_PARAM_SET_ID_TO_PARAMID_BASE((param_descr_p)->param_set_id) - 1

#define PARAMID_TO_INDEX(paramid) (((paramid) & 0x0000ffff) - 1)
#define PARAMID_OK(paramid,param_descr_p) ((PARAMID_TO_INDEX(paramid) >= 0) && ((paramid) < (param_descr_p)->paramid_end))
#define INDEX_TO_PARAMID(index,param_descr_p) ((index) + MPL_PARAM_SET_ID_TO_PARAMID_BASE((param_descr_p)->param_set_id) + 1)

/********************************************************************************
 *
 * Global  variables
 *
 ********************************************************************************/


/********************************************************************************
 *
 * Local variables
 *
 ********************************************************************************/

static mpl_list_t *param_descriptor_set_list_p = NULL;

static mpl_log_fp mpl_param_log_func = NULL;

static void* mpl_param_log_user_p = NULL;

static char value_get_string[max_limit_string+1];


static const char* mpl_names_bool[] =
{
  "false",
  "true"
};

/**
 * paramset_find
 *
 * param_set_id <= 0 means search using paramid_prefix
 * paramid_prefix == NULL means search using param_set_id
 *
 * If both are defined (<= 0 and not NULL), both are checked
 *
 * Returns parameter descriptor set of NULL if not found.
 *
 */
static mpl_param_descr_set_t* paramset_find(int param_set_id, char *paramid_prefix);




/**
 * strchr_escape
 *
 * Description: Like strchr(), but supporting escape. Finds first non-escaped character.
 */
static char *strchr_escape(char *s, char c, char escape);

static int convert_int(const char* value_str, int *value_p);
static int convert_stringarr_to_int(const char* value_str, int *value_p, const char* stringarr[], int stringarr_size);


/*******************************************************************************
 *
 * Public Functions
 *
 *******************************************************************************/

/**
 * mpl_param_init - Initiate library
 **/
int mpl_param_init(void* user_p, mpl_log_fp log_fp, mpl_param_descr_set_t *param_descr_p)
{
  mpl_param_log_user_p = user_p;
  mpl_param_log_func = log_fp;

  /* Check that neither id or prefix has been registered before */
  if ((NULL != paramset_find(param_descr_p->param_set_id, NULL)) ||
      (NULL != paramset_find(-1, param_descr_p->paramid_prefix)))  {
    DBG_PRINTF("Parameter set '%s' already initialized\n", param_descr_p->paramid_prefix);
    return -1;
  }

  mpl_list_add(&param_descriptor_set_list_p, &param_descr_p->list_entry);

  return 0;
}


/**
 * mpl_param_pack - pack a parameter to be sent to psccd (PS Connection
 *                          Control Daemon)
 */
int
mpl_param_pack_internal(const mpl_param_element_t* element_p,
                        char *buf,
                        size_t buflen,
                        bool no_prefix)
{
  int len;
  int tmp_len;
  mpl_param_descr_set_t *param_descr_p;

  if (NULL == element_p) {
    DBG_PRINTF("element_p is NULL\n");
    return (-1);
  }

  param_descr_p = paramset_find(MPL_PARAMID_TO_PARAM_SET_ID(element_p->id), NULL);

  if (NULL == param_descr_p)
  {
    DBG_PRINTF("Parameter set is NULL\n");
    return (-1);
  }

  if (!PARAMID_OK(element_p->id, param_descr_p))
  {
    DBG_PRINTF("Illegal element id: %d\n", element_p->id);
    return (-1);
  }

  if (no_prefix)
    len = snprintf(buf, buflen, "%s", param_descr_p->array[PARAMID_TO_INDEX(element_p->id)].name);
  else
    len = snprintf(buf, buflen, "%s.%s", param_descr_p->paramid_prefix, param_descr_p->array[PARAMID_TO_INDEX(element_p->id)].name);

  if (len < 0)
  {
    DBG_PRINTF("snprintf failed\n");
    return (len);
  }

  /* We also accept no value (used by e.g. 'get' command) */
  if (NULL == element_p->value_p)
    return (len);

  if (len >= (int)buflen)
    tmp_len = (*param_descr_p->array[PARAMID_TO_INDEX(element_p->id)].pack_func)(element_p->value_p,
                                                                                 NULL,
                                                                                 0,
                                                                                 param_descr_p->array[PARAMID_TO_INDEX(element_p->id)].stringarr,
                                                                                 param_descr_p->array[PARAMID_TO_INDEX(element_p->id)].stringarr_size);
  else
    tmp_len = (*param_descr_p->array[PARAMID_TO_INDEX(element_p->id)].pack_func)(element_p->value_p,
                                                                                 buf+len,
                                                                                 buflen-len,
                                                                                 param_descr_p->array[PARAMID_TO_INDEX(element_p->id)].stringarr,
                                                                                 param_descr_p->array[PARAMID_TO_INDEX(element_p->id)].stringarr_size);
  if (tmp_len < 0)
  {
    DBG_PRINTF("Param value pack failed for %s\n",mpl_param_id_get_string(element_p->id));
    return (tmp_len);
  }

  len += tmp_len;

  return (len);
}

/**
 * mpl_param_unpack - unpack received parameter strings
 */
int
mpl_param_unpack_internal(const char* id_str,
			      const char* value_str,
			      mpl_param_element_t** element_pp,
			      int param_set_id)
{
  int id, res;
  mpl_param_element_t* tmp_p;
  mpl_param_descr_set_t *param_descr_p = NULL;
  mpl_list_t *elem_p;
  int prefix_len;
  int size;

  if (NULL == element_pp)
  {
    return (-1);
  }

  MPL_LIST_FOR_EACH(param_descriptor_set_list_p, elem_p)
  {
    param_descr_p = MPL_LIST_CONTAINER(elem_p, mpl_param_descr_set_t, list_entry);
    prefix_len = strlen(param_descr_p->paramid_prefix);
    if (!strncmp(param_descr_p->paramid_prefix, id_str, prefix_len) &&
        ('.' == id_str[prefix_len]))
    {
      id_str += (prefix_len + 1);
      break;
    }
    param_descr_p = NULL;
  }

  if (NULL == param_descr_p)
  {
    /* No prefix perhaps? */
    if (param_set_id > 0)
    {
      param_descr_p = paramset_find(param_set_id, NULL);
    }
  }

  if (NULL == param_descr_p)
  {
    DBG_PRINTF("Parameter set is NULL\n");
    return (-1);
  }

  size = PARAM_SET_SIZE(param_descr_p);

  for (id=0; id < size; id++)
  {
    if (0 == strcmp(param_descr_p->array[id].name, id_str))
    {
      tmp_p = mpl_param_element_create_empty(INDEX_TO_PARAMID(id,param_descr_p));
      if (NULL == tmp_p)
      {
        return (-1);
      }

      /* We also accept no value (used by 'get' command) */
      if (NULL == value_str)
      {
        *element_pp = tmp_p;
        return (0);
      }

      assert(NULL != param_descr_p->array[id].unpack_func);
      
      res = (*param_descr_p->array[id].unpack_func)(value_str,
						    &tmp_p->value_p,
						    param_descr_p->array[id].stringarr,
						    param_descr_p->array[id].stringarr_size,
                                                    param_descr_p->array[id].max_p);

      if (res < 0)
      {
        DBG_PRINTF("Param value unpack failed for %s=%s\n",
                   id_str, value_str);
        mpl_param_element_destroy(tmp_p);
        return (res);
      }

      *element_pp = tmp_p;
      return (0);
    }
  }

  return (-1);
}

/**
 * mpl_param_id_get_string
 */
const char *
mpl_param_id_get_string(mpl_param_element_id_t param_id)
{
  mpl_param_descr_set_t *param_descr_p;

  param_descr_p = paramset_find(MPL_PARAMID_TO_PARAM_SET_ID(param_id), NULL);

  if (NULL == param_descr_p) {
    DBG_PRINTF("Parameter set is NULL\n");
    return "<Error: no parameter set>";
  }

  if (!PARAMID_OK(param_id, param_descr_p))
    return "<unknown param>";

  return param_descr_p->array[PARAMID_TO_INDEX(param_id)].name;
}

/**
 * mpl_param_value_get_string
 *
 * Description: Get string corresponding to the parameter value
 *
 * Parameters:
 *     param_id:     Param ID
 *     value_p:      Param value
 *
 * Return Values : String
 *
 */
const char *
mpl_param_value_get_string(mpl_param_element_id_t param_id,
                               void* value_p)
{
  int res;
  mpl_param_descr_set_t *param_descr_p;

  param_descr_p = paramset_find(MPL_PARAMID_TO_PARAM_SET_ID(param_id), NULL);


  if (NULL == param_descr_p) {
    DBG_PRINTF("Parameter set is NULL\n");
    return "<Error: no parameter set>";
  }

  if (!PARAMID_OK(param_id, param_descr_p))
    return "<unknown param>";


  /* We also accept no value */
  if (NULL == value_p)
    return "<no value>";

  res = (*param_descr_p->array[PARAMID_TO_INDEX(param_id)].pack_func)(value_p,
                                                                      value_get_string,
                                                                      sizeof(value_get_string),
                                                                      param_descr_p->array[PARAMID_TO_INDEX(param_id)].stringarr,
                                                                      param_descr_p->array[PARAMID_TO_INDEX(param_id)].stringarr_size);
  if (res <= 0)
    return "<unknown value>";

  /* Remove '=' character!!! */
  return &value_get_string[1];
}

/**
 * mpl_param_allow_get
 */
bool
mpl_param_allow_get(mpl_param_element_id_t param_id)
{
  mpl_param_descr_set_t *param_descr_p;

  param_descr_p = paramset_find(MPL_PARAMID_TO_PARAM_SET_ID(param_id), NULL);

  if (NULL == param_descr_p) {
    DBG_PRINTF("Parameter set is NULL\n");
    return false;
  }

  if (!PARAMID_OK(param_id, param_descr_p))
    return false;

  return param_descr_p->array[PARAMID_TO_INDEX(param_id)].allow_get;
}

/**
 * mpl_param_allow_set
 */
bool
mpl_param_allow_set(mpl_param_element_id_t param_id)
{
  mpl_param_descr_set_t *param_descr_p;

  param_descr_p = paramset_find(MPL_PARAMID_TO_PARAM_SET_ID(param_id), NULL);

  if (NULL == param_descr_p) {
    DBG_PRINTF("Parameter set is NULL\n");
    return false;
  }

  if (!PARAMID_OK(param_id, param_descr_p))
    return false;

  return param_descr_p->array[PARAMID_TO_INDEX(param_id)].allow_set;
}

/**
 * mpl_param_allow_config
 */
bool
mpl_param_allow_config(mpl_param_element_id_t param_id)
{
  mpl_param_descr_set_t *param_descr_p;

  param_descr_p = paramset_find(MPL_PARAMID_TO_PARAM_SET_ID(param_id), NULL);

  if (NULL == param_descr_p) {
    DBG_PRINTF("Parameter set is NULL\n");
    return false;
  }

  if (!PARAMID_OK(param_id, param_descr_p))
    return false;

  return param_descr_p->array[PARAMID_TO_INDEX(param_id)].allow_config;
}

/**
 * mpl_param_element_create_stringn
 *
 */
mpl_param_element_t*
  mpl_param_element_create_stringn(mpl_param_element_id_t param_id,
                                   const char* value,
                                   size_t len)
{
  mpl_param_element_t* element_p;
  size_t size;

  size = len + 1;

  element_p = mpl_param_element_create_empty(param_id);
  if (NULL == element_p)
  {
    return NULL;
  }

  element_p->value_p = malloc(size);
  if (NULL == element_p->value_p)
  {
    DBG_PRINTF("Failed allocating memory\n");
    mpl_param_element_destroy(element_p);
    return NULL;
  }

  strncpy((char*)element_p->value_p, value, size);

  return (element_p);
}

/**
 * mpl_param_element_create_empty
 *
 */
mpl_param_element_t*
mpl_param_element_create_empty(mpl_param_element_id_t param_id)
{
  mpl_param_element_t* element_p;
  mpl_param_descr_set_t *param_descr_p;

  param_descr_p = paramset_find(MPL_PARAMID_TO_PARAM_SET_ID(param_id), NULL);

  if (NULL == param_descr_p) {
    DBG_PRINTF("Parameter set is NULL\n");
    return NULL;
  }

  if (!PARAMID_OK(param_id, param_descr_p))
  {
    DBG_PRINTF("Unknown parameter ID: %x\n", param_id);
    return (NULL);
  }

  element_p = malloc(sizeof(mpl_param_element_t));
  if (NULL == element_p)
  {
    DBG_PRINTF("Failed allocating memory\n");
    return NULL;
  }

  element_p->id = param_id;
  element_p->value_p = NULL;
  element_p->list_entry.next_p = NULL;

  return (element_p);
}

/**
 * mpl_param_element_create
 */
mpl_param_element_t*
mpl_param_element_create(mpl_param_element_id_t param_id,
                             const void* value_p)
{
  mpl_param_element_t* element_p;
  mpl_param_descr_set_t *param_descr_p;

  param_descr_p = paramset_find(MPL_PARAMID_TO_PARAM_SET_ID(param_id), NULL);

  if (NULL == param_descr_p)
  {
    DBG_PRINTF("Parameter set is NULL\n");
    return NULL;
  }

  element_p = mpl_param_element_create_empty(param_id);
  if (NULL == element_p)
  {
    return NULL;
  }

  if (NULL != value_p)
  {
    int res;

    res = (*param_descr_p->array[PARAMID_TO_INDEX(param_id)].clone_func)(&element_p->value_p,
                                                                         value_p);
    if (res < 0)
    {
      DBG_PRINTF("Param value clone failed for %s\n",
                 mpl_param_id_get_string(param_id));
      mpl_param_element_destroy(element_p);
      return NULL;
    }
  }

  return (element_p);
}


/**
 * mpl_param_element_clone
 */
mpl_param_element_t*
  mpl_param_element_clone(const mpl_param_element_t* element_p)
{
  mpl_param_element_t* new_element_p;
  int res;
  mpl_param_descr_set_t *param_descr_p;

  if (NULL == element_p)
    return NULL;

  param_descr_p = paramset_find(MPL_PARAMID_TO_PARAM_SET_ID(element_p->id), NULL);

  if (NULL == param_descr_p)
  {
    DBG_PRINTF("Parameter set is NULL\n");
    return NULL;
  }

  new_element_p = mpl_param_element_create_empty(element_p->id);
  if (NULL == new_element_p)
  {
    return NULL;
  }

  if (NULL != element_p->value_p)
  {
    res = (*param_descr_p->array[PARAMID_TO_INDEX(element_p->id)].clone_func)(&new_element_p->value_p,
							    element_p->value_p);
    if (res < 0)
    {
      DBG_PRINTF("Param value clone failed for %s\n",
                 mpl_param_id_get_string(element_p->id));
      mpl_param_element_destroy(new_element_p);
      return NULL;
    }
  }

  return new_element_p;
}


/**
 * mpl_param_element_compare
 */
int
mpl_param_element_compare(const mpl_param_element_t* element1_p,
                          const mpl_param_element_t* element2_p)
{
  mpl_param_descr_set_t *param_descr_p;

  if (NULL == element1_p)
    return -1;

  if (NULL == element2_p)
    return -1;

  if (element1_p->id != element2_p->id)
    return -1;

  param_descr_p = paramset_find(MPL_PARAMID_TO_PARAM_SET_ID(element1_p->id), NULL);

  if (NULL == param_descr_p)
  {
    DBG_PRINTF("Parameter set is NULL\n");
    return -1;
  }

  return (*param_descr_p->array[PARAMID_TO_INDEX(element1_p->id)].compare_func)(element1_p->value_p, element2_p->value_p);
}


/**
 * mpl_param_element_get_default
 */
mpl_param_element_t*
mpl_param_element_get_default(mpl_param_element_id_t param_id)
{
  mpl_param_element_t* element_p;
  int res;
  mpl_param_descr_set_t *param_descr_p;

  param_descr_p = paramset_find(MPL_PARAMID_TO_PARAM_SET_ID(param_id), NULL);

  if (NULL == param_descr_p)
  {
    DBG_PRINTF("Parameter set is NULL\n");
    return NULL;
  }

  if (!PARAMID_OK(param_id, param_descr_p))
  {
    DBG_PRINTF("Unknown parameter ID: %d\n", param_id);
    return (NULL);
  }

  /* Do we have a default value? */
  if (param_descr_p->array[PARAMID_TO_INDEX(param_id)].default_value_p == NULL)
  {
    /* There is no default value for parameter */
    return NULL;
  }

  element_p = mpl_param_element_create_empty(param_id);
  if (NULL == element_p)
  {
    return NULL;
  }

  res = (*param_descr_p->array[PARAMID_TO_INDEX(element_p->id)].clone_func)(&element_p->value_p,
                                                                            param_descr_p->array[PARAMID_TO_INDEX(param_id)].default_value_p);
  if (res < 0)
  {
    DBG_PRINTF("Param value clone failed for %s\n",
               mpl_param_id_get_string(param_id));
    mpl_param_element_destroy(element_p);
    return NULL;
  }

  return element_p;
}

/**
 * mpl_param_element_destroy
 */
void
mpl_param_element_destroy(mpl_param_element_t* element_p)
{
  if (NULL == element_p)
    return;

  if (NULL != element_p->value_p)
  {
    mpl_param_descr_set_t *param_descr_p;

    param_descr_p = paramset_find(MPL_PARAMID_TO_PARAM_SET_ID(element_p->id), NULL);

    if (NULL != param_descr_p)
    {
      if (PARAMID_OK(element_p->id, param_descr_p))
      {
        (*param_descr_p->array[PARAMID_TO_INDEX(element_p->id)].free_func)(element_p->value_p);
      }
      else
        DBG_PRINTF("param id not ok, element not freed\n");
    }
    else
      DBG_PRINTF("param descr set not found, element not freed\n");
  }

  free(element_p);
}

/**
 * mpl_param_list_destroy
 *
 */
void mpl_param_list_destroy( mpl_list_t **param_list_pp )
{
  mpl_list_t *elem_p, *tmp_p;

  if (NULL == param_list_pp)
    return;

  MPL_LIST_FOR_EACH_SAFE(*param_list_pp, elem_p, tmp_p)
  {
    (void)mpl_list_remove(param_list_pp, elem_p);
    mpl_param_element_destroy(MPL_LIST_CONTAINER(elem_p,
                                                 mpl_param_element_t,
                                                 list_entry));
  }
}

/**
 * mpl_param_list_find
 * 
 */
mpl_param_element_t*
mpl_param_list_find( mpl_param_element_id_t param_id, mpl_list_t *param_list_p )
{
  mpl_list_t *elem_p;
  mpl_param_element_t *res;

  MPL_LIST_FOR_EACH(param_list_p, elem_p)
  {
    res = MPL_LIST_CONTAINER(elem_p, mpl_param_element_t, list_entry);
    if (res->id == param_id)
      return res;
  }

  return NULL;
}

/**
 * mpl_param_list_tuple_key_find
 * 
 */
mpl_param_element_t*
  mpl_param_list_tuple_key_find( mpl_param_element_id_t param_id, char *key_p, mpl_list_t *param_list_p )
{
  mpl_param_element_t *param_elem_p;
  mpl_string_tuple_t *st_p;

  if ((NULL == key_p) || (0 == strlen(key_p)))
  {
    DBG_PRINTF("Invalid key_p\n");
    return NULL;
  }
  
  param_elem_p = mpl_param_list_find(param_id, param_list_p);
  while (NULL != param_elem_p)
  {
    st_p = MPL_GET_VALUE_REF_FROM_PARAM_ELEMENT(mpl_string_tuple_t*, param_elem_p);
    if ((NULL != st_p) && (NULL != st_p->key_p) && !strncmp(st_p->key_p, key_p, strlen(key_p)))
      return param_elem_p;
    
    param_elem_p = mpl_param_list_find_next(param_id, param_elem_p);
  }
  return NULL;
}


/**
 * mpl_param_list_tuple_key_find_wildcard
 * 
 */
mpl_param_element_t*
  mpl_param_list_tuple_key_find_wildcard( mpl_param_element_id_t param_id, char *key_p, char *wildcard_p, mpl_list_t *param_list_p )
{
  mpl_param_element_t *param_elem_p;

  param_elem_p = mpl_param_list_tuple_key_find(param_id, key_p, param_list_p);
  if (NULL != param_elem_p)
    return param_elem_p;

  return mpl_param_list_tuple_key_find(param_id, wildcard_p, param_list_p);
}


/**
 * mpl_pack_param_value_string()
 **/
int mpl_pack_param_value_string(void* param_value_p, char *buf, size_t buflen, const char** stringarr, int stringarr_size)
{
  assert(NULL != param_value_p);
  MPL_IDENTIFIER_NOT_USED(stringarr_size);
  MPL_IDENTIFIER_NOT_USED(stringarr)
  return snprintf(buf, buflen, "=%s", (char*)param_value_p);
}

/**
 * mpl_unpack_param_value_string()
 **/
int
mpl_unpack_param_value_string(const char* value_str, void **value_pp,
                                  const char** stringarr, int stringarr_size,
                                  const uint64_t *max_p)
{
  char* p;
  size_t size;

  MPL_IDENTIFIER_NOT_USED(stringarr);
  MPL_IDENTIFIER_NOT_USED(stringarr_size);

  if ((max_p != NULL) &&
      (strlen(value_str) > *max_p))
  {
    DBG_PRINTF("Unpack string failed on max length check: %d > %" PRIu64 "\n", strlen(value_str), *max_p);
    return (-1);
  }

  size = strlen(value_str)+1;
  p = malloc(size);
  if (NULL == p)
  {
    DBG_PRINTF("Failed allocating memory\n");
    return (-1);
  }

  strncpy(p, value_str, size);
  *value_pp = p;

  return (0);
}

/**
 * mpl_clone_param_value_string()
 **/
int mpl_clone_param_value_string(void **new_value_pp, const void* old_value_p)
{
  size_t size = strlen((char*)old_value_p) + 1;
  char* p = malloc(size);
  if (NULL == p)
  {
    DBG_PRINTF("Failed allocating memory\n");
    return (-1);
  }

  strncpy(p, (char*)old_value_p, size);
  *new_value_pp = p;

  return (0);
}

/**
 * mpl_compare_param_value_string()
 **/
int mpl_compare_param_value_string(const void *value1_p, const void* value2_p)
{
  return strcmp(value1_p, value2_p);
}


/**
 * mpl_pack_param_value_int()
 **/
int mpl_pack_param_value_int(void* param_value_p, char *buf, size_t buflen, const char** stringarr, int stringarr_size)
{
  assert(NULL != param_value_p);
  MPL_IDENTIFIER_NOT_USED(stringarr);
  MPL_IDENTIFIER_NOT_USED(stringarr_size);
  return snprintf(buf, buflen, "=%d", *(int*)param_value_p);
}

/**
 * mpl_unpack_param_value_int()
 **/
int
mpl_unpack_param_value_int(const char* value_str, void **value_pp,
                               const char** stringarr, int stringarr_size,
                               const uint64_t *max_p)
{
  int* p;

  MPL_IDENTIFIER_NOT_USED(stringarr);
  MPL_IDENTIFIER_NOT_USED(stringarr_size);

  p = malloc(sizeof(int));
  if (NULL == p)
  {
    DBG_PRINTF("Failed allocating memory\n");
    return (-1);
  }

  if (convert_int(value_str, p) < 0)
  {
    free(p);
    return (-1);
  }

  if ((max_p != NULL) &&
      ((uint64_t)*p > *max_p))
  {
    DBG_PRINTF("Unpack int failed on max check: %d > %" PRIu64 "\n", *p, *max_p);
    free(p);
    return (-1);
  }

  assert(NULL != value_pp);
  *value_pp = p;

  return (0);
}

/**
 * mpl_clone_param_value_int()
 **/
int
mpl_clone_param_value_int(void **new_value_pp, const void* old_value_p)
{
  int* p = malloc(sizeof(int));
  if (NULL == p)
  {
    DBG_PRINTF("Failed allocating memory\n");
    return (-1);
  }

  *p = *(int*)old_value_p;
  *new_value_pp = p;

  return (0);
}

/**
 * mpl_compare_param_value_int()
 **/
int mpl_compare_param_value_int(const void *value1_p, const void* value2_p)
{
  return *((int*) value1_p) != *((int*) value2_p);
}


/**
 * mpl_pack_param_value_uint8()
 **/
int mpl_pack_param_value_uint8(void* param_value_p, char *buf, size_t buflen, const char** stringarr, int stringarr_size)
{
  assert(NULL != param_value_p);
  MPL_IDENTIFIER_NOT_USED(stringarr);
  MPL_IDENTIFIER_NOT_USED(stringarr_size);
  return snprintf(buf, buflen, "=0x%x", *(uint8_t*)param_value_p);
}

/**
 * mpl_unpack_param_value_uint8()
 **/
int
mpl_unpack_param_value_uint8(const char* value_str, void **value_pp,
                                 const char** stringarr, int stringarr_size,
                                 const uint64_t *max_p)
{
  int temp;
  uint8_t* p;

  MPL_IDENTIFIER_NOT_USED(stringarr);
  MPL_IDENTIFIER_NOT_USED(stringarr_size);

  p = malloc(sizeof(uint8_t));
  if (NULL == p)
  {
    DBG_PRINTF("Failed allocating memory\n");
    return (-1);
  }

  if (convert_int(value_str, &temp) < 0)
  {
    free(p);
    return (-1);
  }

  if ((temp < 0) ||
      (temp > (int)UINT8_MAX))
  {
    DBG_PRINTF("Unpack uint8 failed on range check: %d\n", temp);
    free(p);
    return (-1);
  }

  if ((max_p != NULL) &&
      ((uint64_t)temp > *max_p))
  {
    DBG_PRINTF("Unpack uint8 failed on max check: %d > %" PRIu64 "\n", temp, *max_p);
    free(p);
    return (-1);
  }

  *p = (uint8_t)temp;

  assert(NULL != value_pp);
  *value_pp = p;

  return (0);
}

/**
 * mpl_clone_param_value_uint8()
 **/
int
mpl_clone_param_value_uint8(void **new_value_pp, const void* old_value_p)
{
  uint8_t* p = malloc(sizeof(uint8_t));
  if (NULL == p)
  {
    DBG_PRINTF("Failed allocating memory\n");
    return (-1);
  }

  *p = *(uint8_t*)old_value_p;
  *new_value_pp = p;

  return (0);
}

/**
 * mpl_compare_param_value_uint8()
 **/
int mpl_compare_param_value_uint8(const void *value1_p, const void* value2_p)
{
  return *((uint8_t*) value1_p) != *((uint8_t*) value2_p);
}


/**
 * mpl_pack_param_value_uint32()
 **/
int mpl_pack_param_value_uint32(void* param_value_p, char *buf, size_t buflen, const char** stringarr, int stringarr_size)
{
  assert(NULL != param_value_p);
  MPL_IDENTIFIER_NOT_USED(stringarr);
  MPL_IDENTIFIER_NOT_USED(stringarr_size);
  return snprintf(buf, buflen, "=0x%" PRIx32, *(uint32_t*)param_value_p);
}

/**
 * mpl_unpack_param_value_uint32()
 **/
int
mpl_unpack_param_value_uint32(const char* value_str, void **value_pp,
                                  const char** stringarr, int stringarr_size,
                                  const uint64_t *max_p)
{
  uint64_t temp;
  uint32_t* p;
  int res;

  MPL_IDENTIFIER_NOT_USED(stringarr);
  MPL_IDENTIFIER_NOT_USED(stringarr_size);

  p = malloc(sizeof(uint32_t));
  if (NULL == p)
  {
    DBG_PRINTF("Failed allocating memory\n");
    return (-1);
  }

  if ((NULL != strstr(value_str, "0x")) ||
      (NULL != strstr(value_str, "0X")))
    res = sscanf(value_str, "0x%" SCNx64, &temp);
  else
    res = sscanf(value_str, "%" SCNu64, &temp);

  if (res <= 0)
  {
    free(p);
    return (-1);
  }

  if (temp > UINT32_MAX)
  {
    DBG_PRINTF("Unpack uint32 failed on range check: 0x%" PRIu64 "\n", temp);
    free(p);
    return (-1);
  }

  if ((max_p != NULL) &&
      (temp > *max_p))
  {
    DBG_PRINTF("Unpack uint32 failed on max check: %" PRIu64 " > %" PRIu64 "\n", temp, *max_p);
    free(p);
    return (-1);
  }

  *p = (uint32_t)temp;

  assert(NULL != value_pp);
  *value_pp = p;

  return (0);
}

/**
 * mpl_clone_param_value_uint32()
 **/
int
mpl_clone_param_value_uint32(void **new_value_pp, const void* old_value_p)
{
  uint32_t* p = malloc(sizeof(uint32_t));
  if (NULL == p)
  {
    DBG_PRINTF("Failed allocating memory\n");
    return (-1);
  }

  *p = *(uint32_t*)old_value_p;
  *new_value_pp = p;

  return (0);
}

/**
 * mpl_compare_param_value_uint32()
 **/
int mpl_compare_param_value_uint32(const void *value1_p, const void* value2_p)
{
  return *((uint32_t*) value1_p) != *((uint32_t*) value2_p);
}



/**
 * mpl_pack_param_value_enum()
 **/
int mpl_pack_param_value_enum(void* param_value_p, char *buf, size_t buflen, const char** stringarr, int stringarr_size)
{
  int index;

  assert(NULL != param_value_p);
  assert(NULL != stringarr);

  index = *(int*)param_value_p;
  if (index >= stringarr_size)
  {
    DBG_PRINTF("Pack enum failed on range check: %d >= %d\n",
               index, stringarr_size);
    return (-1);
  }

  assert(NULL != stringarr[index]);

  return snprintf(buf, buflen, "=%s", stringarr[index]);
}

/**
 * mpl_unpack_param_value_enum()
 **/
int
mpl_unpack_param_value_enum(const char* value_str, void **value_pp,
                                const char* stringarr[], int stringarr_size,
                                const uint64_t *max_p)
{
  int* p = malloc(sizeof(int));

  MPL_IDENTIFIER_NOT_USED(max_p);

  if (NULL == p)
  {
    DBG_PRINTF("Failed allocating memory\n");
    return (-1);
  }

  if (convert_stringarr_to_int(value_str, p, stringarr, stringarr_size) < 0)
  {
    if (convert_int(value_str, p) < 0)
    {
      free(p);
      return (-1);
    }

    if ((*p < 0) ||
	(*p >= stringarr_size))
    {
      DBG_PRINTF("Unpack enum failed on range check: %d : %d\n",
                 *p, stringarr_size);
      free(p);
      return (-1);
    }
  }

  assert(NULL != value_pp);
  *value_pp = p;

  return (0);
}

/**
 * mpl_pack_param_value_bool()
 **/
int mpl_pack_param_value_bool(void* param_value_p, char *buf, size_t buflen, const char** stringarr, int stringarr_size)
{
  int index;

  MPL_IDENTIFIER_NOT_USED(stringarr);
  MPL_IDENTIFIER_NOT_USED(stringarr_size);

  assert(NULL != param_value_p);

  index = *(bool*)param_value_p;
  if (index >= (int)ARRAY_SIZE(mpl_names_bool))
  {
    DBG_PRINTF("Pack bool failed on range check: %d >= %d\n",
               index, ARRAY_SIZE(mpl_names_bool));
    return -1;
  }

  return snprintf(buf, buflen, "=%s", mpl_names_bool[index]);
}

/**
 * mpl_unpack_param_value_bool()
 **/
int mpl_unpack_param_value_bool(const char* value_str, void **value_pp,
                                    const char** stringarr, int stringarr_size,
                                    const uint64_t *max_p)
{
  int temp;
  bool* p = malloc(sizeof(bool));

  MPL_IDENTIFIER_NOT_USED(stringarr);
  MPL_IDENTIFIER_NOT_USED(stringarr_size);
  MPL_IDENTIFIER_NOT_USED(max_p);

  if (NULL == p)
  {
    DBG_PRINTF("Failed allocating memory\n");
    return (-1);
  }

  if (convert_stringarr_to_int(value_str, &temp,
                               mpl_names_bool, ARRAY_SIZE(mpl_names_bool)) < 0)
  {
    if (convert_int(value_str, &temp) < 0)
    {
      free(p);
      return (-1);
    }
  }

  *p = (bool)temp;

  assert(NULL != value_pp);
  *value_pp = p;

  return (0);
}

int mpl_clone_param_value_bool(void **new_value_pp, const void* old_value_p)
{
  bool* p = malloc(sizeof(bool));
  if (NULL == p)
  {
    DBG_PRINTF("Failed allocating memory\n");
    return (-1);
  }

  *p = *(uint8_t*)old_value_p;
  *new_value_pp = p;

  return (0);
}

/**
 * mpl_compare_param_value_bool()
 **/
int mpl_compare_param_value_bool(const void *value1_p, const void* value2_p)
{
  return *((bool*) value1_p) != *((bool*) value2_p);
}



/**
 * mpl_pack_param_value_uint8_array()
 **/
int mpl_pack_param_value_uint8_array(void* param_value_p, char *buf, size_t buflen, const char** stringarr, int stringarr_size)
{
  int i;
  mpl_uint8_array_t *a_p;
  int len;
  int total_len;

  assert(NULL != param_value_p);
  MPL_IDENTIFIER_NOT_USED(stringarr);
  MPL_IDENTIFIER_NOT_USED(stringarr_size);

  a_p = param_value_p;

  len = snprintf(buf, buflen, "=%08x", a_p->len);
  if ((int)buflen > len)
    buf += len;
  total_len = len;

  for (i = 0; i < a_p->len; i++)
  {
    len = snprintf(buf, buflen, "%02x", a_p->arr_p[i]);
    if ((int)buflen > len)
      buf += len;
    total_len += len;
  }

  return total_len;
}


/**
 * mpl_unpack_param_value_uint8_array()
 **/
int
mpl_unpack_param_value_uint8_array(const char* value_str, void **value_pp,
                                   const char** stringarr, int stringarr_size,
                                   const uint64_t *max_p)
{
  int len;
  int i;
  unsigned int val;
  mpl_uint8_array_t *a_p;
  uint8_t* p;

  MPL_IDENTIFIER_NOT_USED(stringarr);
  MPL_IDENTIFIER_NOT_USED(stringarr_size);
  MPL_IDENTIFIER_NOT_USED(max_p);

  a_p = malloc(sizeof(mpl_uint8_array_t));
  if (NULL == a_p)
  {
    DBG_PRINTF("Failed allocating memory\n");
    return (-1);
  }

  if (sscanf(value_str, "%8x", &len) <= 0)
  {
    free(a_p);
    return (-1);
  }

  a_p->arr_p = malloc(len * sizeof(uint8_t));

  if (NULL == a_p->arr_p)
  {
    DBG_PRINTF("Failed allocating memory\n");
    free(a_p);
    return (-1);
  }

  p = (uint8_t *) value_str + 8;
  for (i = 0; i < len; i++)
  {
    if (sscanf((char*) p, "%2x", &val) <= 0)
    {
      free(a_p->arr_p);
      free(a_p);
      return -1;
    }
    a_p->arr_p[i] = (uint8_t) val;
    p += 2;
  }

  a_p->len = len;

  assert(NULL != value_pp);
  *value_pp = a_p;

  return (0);
}

/**
 * mpl_clone_param_value_uint8_array()
 **/
int
mpl_clone_param_value_uint8_array(void **new_value_pp, const void* old_value_p)
{
  mpl_uint8_array_t *a_p;
  const mpl_uint8_array_t *old_a_p;

  a_p = malloc(sizeof(mpl_uint8_array_t));
  if (NULL == a_p)
  {
    DBG_PRINTF("Failed allocating memory\n");
    return (-1);
  }

  old_a_p = old_value_p;

  a_p->arr_p = malloc(old_a_p->len * sizeof(uint8_t));
  if (NULL == a_p->arr_p)
  {
    DBG_PRINTF("Failed allocating memory\n");
    free(a_p);
    return (-1);
  }

  a_p->len = old_a_p->len;
  memcpy(a_p->arr_p, old_a_p->arr_p, a_p->len);

  *new_value_pp = a_p;

  return (0);
}

/**
 * mpl_compare_param_value_uint8_array()
 **/
int mpl_compare_param_value_uint8_array(const void *value1_p, const void* value2_p)
{
  const mpl_uint8_array_t *a1_p;
  const mpl_uint8_array_t *a2_p;

  a1_p = value1_p;
  a2_p = value2_p;

  if (a1_p->len != a2_p->len)
    return -1;

  return memcmp(a1_p->arr_p, a2_p->arr_p, a1_p->len);
}

/**
 * mpl_free_param_value_uint8_array()
 **/
void mpl_free_param_value_uint8_array(void *value_p)
{
  mpl_uint8_array_t *a_p;

  a_p = value_p;
  if (NULL != a_p)
  {
    if (NULL != a_p->arr_p)
    {
      free(a_p->arr_p);
    }
    free(a_p);
  }
}


/**
 * mpl_pack_param_value_string_tuple()
 **/
int mpl_pack_param_value_string_tuple(void* param_value_p, char *buf, size_t buflen, const char** stringarr, int stringarr_size)
{
  mpl_string_tuple_t *st_p;

  assert(NULL != param_value_p);
  MPL_IDENTIFIER_NOT_USED(stringarr);
  MPL_IDENTIFIER_NOT_USED(stringarr_size);

  st_p = param_value_p;
  assert(NULL != st_p->key_p);
  
  if (NULL != st_p->value_p)
    return snprintf(buf, buflen, "=%s:%s", st_p->key_p, st_p->value_p);
  else
    return snprintf(buf, buflen, "=%s:", st_p->key_p);
}


/**
 * mpl_unpack_param_value_string_tuple()
 **/
int
mpl_unpack_param_value_string_tuple(const char* value_str, void **value_pp,
                                    const char** stringarr, int stringarr_size,
                                    const uint64_t *max_p)
{
  mpl_string_tuple_t *st_p;
  char* p;
  size_t slen;

  MPL_IDENTIFIER_NOT_USED(stringarr);
  MPL_IDENTIFIER_NOT_USED(stringarr_size);

  if ((max_p != NULL) &&
      (strlen(value_str) > *max_p))
  {
    DBG_PRINTF("Unpack string_tuple failed on max length check: %d > %" PRIu64 "\n", strlen(value_str), *max_p);
    return (-1);
  }

  p = strchr(value_str, ':');
  if (NULL == p)
  {
    DBG_PRINTF("Unpack string_tuple failed, no delimiter: %s\n", value_str);
    return (-1);
  }

  st_p = malloc(sizeof(mpl_string_tuple_t));
  if (NULL == st_p)
  {
    DBG_PRINTF("Failed allocating memory\n");
    return (-1);
  }

  slen = p - value_str;
  st_p->key_p = malloc(slen + 1);
  if (NULL == st_p->key_p)
  {
    DBG_PRINTF("Failed allocating memory\n");
    free(st_p);
    return (-1);
  }
  strncpy(st_p->key_p, value_str, slen);
  st_p->key_p[slen] = 0;

  p++;
  slen = strlen(p);
  st_p->value_p = malloc(slen + 1);
  if (NULL == st_p->value_p)
  {
    DBG_PRINTF("Failed allocating memory\n");
    free(st_p->key_p);
    free(st_p);
    return (-1);
  }

  strncpy(st_p->value_p, p, slen);
  st_p->value_p[slen] = 0;
  *value_pp = st_p;

  return (0);
}

/**
 * mpl_clone_param_value_string_tuple()
 **/
int
mpl_clone_param_value_string_tuple(void **new_value_pp, const void* old_value_p)
{
  mpl_string_tuple_t *st_p;
  const mpl_string_tuple_t *old_st_p;

  st_p = malloc(sizeof(mpl_string_tuple_t));
  if (NULL == st_p)
  {
    DBG_PRINTF("Failed allocating memory\n");
    return (-1);
  }

  old_st_p = old_value_p;

  st_p->key_p = malloc(strlen(old_st_p->key_p) + 1);
  if (NULL == st_p->key_p)
  {
    DBG_PRINTF("Failed allocating memory\n");
    free(st_p);
    return (-1);
  }
 
  st_p->value_p = malloc(strlen(old_st_p->value_p) + 1);
  if (NULL == st_p->value_p)
  {
    DBG_PRINTF("Failed allocating memory\n");
    free(st_p->key_p);
    free(st_p);
    return (-1);
  }

  strcpy(st_p->key_p, old_st_p->key_p);
  strcpy(st_p->value_p, old_st_p->value_p);

  *new_value_pp = st_p;

  return (0);
}

/**
 * mpl_compare_param_value_string_tuple()
 **/
int mpl_compare_param_value_string_tuple(const void *value1_p, const void* value2_p)
{
  const mpl_string_tuple_t *st1_p;
  const mpl_string_tuple_t *st2_p;

  st1_p = value1_p;
  st2_p = value2_p;

  if ((NULL == st1_p->key_p) || (NULL == st2_p->key_p))
  {
    if (st1_p->key_p != st2_p->key_p)
      return -1;
  }
  else if (strcmp(st1_p->key_p, st2_p->key_p))
    return -1;

  if ((NULL == st1_p->value_p) || (NULL == st2_p->value_p))
  {
    if (st1_p->value_p != st2_p->value_p)
      return -1;
  }
  else if (strcmp(st1_p->value_p, st2_p->value_p))
    return -1;
  
  return 0;
}

/**
 * mpl_free_param_value_string_tuple()
 **/
void mpl_free_param_value_string_tuple(void *value_p)
{
  mpl_string_tuple_t *st_p;

  st_p = value_p;
  if (NULL != st_p)
  {
    if (NULL != st_p->key_p)
    {
      free(st_p->key_p);
    }
    if (NULL != st_p->value_p)
    {
      free(st_p->value_p);
    }
    free(st_p);
  }
}


/**
 * mpl_get_args()
 **/
int mpl_get_args(mpl_arg_t *args, int args_len, char *buf, char equal, char delimiter, char escape)
{
  char *p;
  char *kp;
  char *vp;
  int i = 0;
  int buflen;

  assert(NULL != args);

  p = mpl_trimstring(buf, escape);

  buflen = strlen(p);

  while (((p - buf) < buflen) && (i < args_len))
  {
    char *mid;
    char *end;

    end = strchr_escape( p, delimiter, escape);
    if (NULL == end)
    {
      end = p + strlen(p);
    }
    else
    {
      *end = '\0';
    }

    mid = strchr_escape( p, equal, escape );
    if (NULL != mid)
    {
      *mid = '\0';
      kp = mpl_trimstring(p, escape);
      args[i].key_p = kp;
      vp = mpl_trimstring(mid + 1, escape);
      args[i].value_p = vp;
    }
    else
    {
      args[i].key_p = mpl_trimstring(p, escape);
      args[i].value_p = NULL;
    }

    while ((((end+1) - buf) < buflen) &&
           *(end+1) == delimiter)
      end++;

    p = end + 1;
    i++;
  }

  if ((p - buf) < buflen)
    return -1;
  else
    return i;
}


/**
 * mpl_add_param_to_list - add parameter to param list
 *
 */
int mpl_add_param_to_list(mpl_list_t **param_list_pp,
                          mpl_param_element_id_t param_id,
                          const void *value_p)
{
  mpl_param_element_t* param_elem_p;

  /* Add msgtype to the parameter list */
  param_elem_p = mpl_param_element_create(param_id, value_p);
  if (NULL == param_elem_p)
  {
    return -1;
  }
  
  mpl_list_add(param_list_pp, &param_elem_p->list_entry);
  return 0;
}


/**
 * mpl_trimstring()
 **/
char *mpl_trimstring(char *s, char escape)
{
  char *p = s;
  char *e;

  // Trim start
  while (strlen(s) && isspace(s[0]))
    s++;

  // Trim end, taking escape into account
  p = s + (strlen(s) - 1);

  while ((p > s) && isspace(*p)) {
    // did we find an escaped character?
    e = p;
    while (e && (e > s) && (*(e-1) == escape))
      e--;

    // An odd number of escapes in a row? (then it is escaped)
    if (1 == ((p - e) % 2))
      break;

    *p = '\0';
    p--;
  }

  return s;
}


/*******************************************************************************
 *
 * Private Functions
 *
 *******************************************************************************/
/**
 * paramset_find
 *
 *
 */
static mpl_param_descr_set_t* paramset_find(int param_set_id, char *paramid_prefix)
{
  mpl_list_t *elem_p;
  mpl_param_descr_set_t *paramset_p = NULL;
  bool id_match = false;
  bool prefix_match = false;

  MPL_LIST_FOR_EACH(param_descriptor_set_list_p, elem_p)
  {
    paramset_p = MPL_LIST_CONTAINER(elem_p, mpl_param_descr_set_t, list_entry);

    if ((param_set_id > 0) && (paramset_p->param_set_id == param_set_id))
    {
      id_match = true;
      if (NULL == paramid_prefix)
        return paramset_p;
    }

    if ((NULL != paramid_prefix) && !strncmp(paramset_p->paramid_prefix, paramid_prefix, MPL_PARAMID_PREFIX_MAXLEN))
    {
      prefix_match = true;
      if (param_set_id <= 0)
        return paramset_p;
    }
    
    if (id_match && prefix_match)
      return paramset_p;

    id_match = false;
    prefix_match = false;
  }

  return NULL;
}



/**
 * strchr_escape()
 **/
static char *strchr_escape(char *s, char c, char escape)
{
  char *p = s;
  char *e;

  while (p && strlen(p)) {
    p = strchr(p, c);
    // did we find an escaped character?
    e = p;
    while (e && (e > s) && (*(e-1) == escape))
      e--;
    if (1 == ((p - e) % 2)) {
      // An odd number of escapes in a row: yes
      p++;
      continue;
    }
    else
      break;
  }
  return p;
}

/**
 * convert_int()
 **/
static int
convert_int(const char* value_str, int *value_p)
{
  char* endp;

  assert(NULL != value_p);
  assert(NULL != value_str);

  /* Empty strings are not allowed */
  if (0 == strlen(value_str))
  {
    DBG_PRINTF("empty string is not an acceptable integer value\n");
    return -1;
  }

  *value_p = strtol( value_str, &endp, 0 );

  /* The whole string should be a number */
  if (*endp != '\0')
  {
    DBG_PRINTF("Not an acceptable integer value: %s\n", value_str);
    return (-1);
  }

  return (0);
}

/**
 * convert_stringarr_to_int()
 **/
static int
convert_stringarr_to_int(const char* value_str, int *value_p, const char* stringarr[], int stringarr_size)
{
  int index;

  assert(NULL != value_p);
  assert(NULL != value_str);
  assert(NULL != stringarr);

  for (index=0; index<stringarr_size; index++)
  {
    assert(NULL != stringarr[index]);
    if (0 == strcmp( value_str, stringarr[index]))
    {
      *value_p = index;
      return (0);
    }
  }

  return (-1);
}

