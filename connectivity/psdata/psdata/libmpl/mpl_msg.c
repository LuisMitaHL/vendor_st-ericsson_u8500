/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 * 
 * \brief  Generic Message
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
#include "mpl_msg.h"


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
  if (NULL != mpl_msg_log_func) \
    (*mpl_msg_log_func)(mpl_msg_log_user_p, LOG_DEBUG, __FILE__, __LINE__, format, ##__VA_ARGS__)

#define MESSAGE_DELIMITER ','

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

static mpl_list_t *msg_descriptor_set_list_p = NULL;

static mpl_log_fp mpl_msg_log_func = NULL;
static void* mpl_msg_log_user_p = NULL;

static int pack_msg_common(const mpl_msg_t *msg_p, char* buf, size_t *len_p, const mpl_msg_set_descr_t *msg_set_descr_p);
#define pack_msg_req pack_msg_common
#define pack_msg_resp pack_msg_common
#define pack_msg_event pack_msg_common

static mpl_msg_set_descr_t* msgset_find(int param_set_id, char *paramid_prefix);


/*******************************************************************************
 *
 * Public Functions
 *
 *******************************************************************************/


/**
 * mpl_msg_init - Initiate library
 **/
int mpl_msg_init(void* user_p, mpl_log_fp log_fp, mpl_msg_set_descr_t *msg_set_descr_p)
{
  mpl_msg_log_user_p = user_p;
  mpl_msg_log_func = log_fp;

  /* Check that neither id or prefix has been registered before */
  if ((NULL != msgset_find(msg_set_descr_p->param_descr_set_p->param_set_id, NULL)) ||
      (NULL != msgset_find(-1, msg_set_descr_p->param_descr_set_p->paramid_prefix)))  {
    DBG_PRINTF("Message set '%s' already initialized\n", msg_set_descr_p->param_descr_set_p->paramid_prefix);
    return -1;
  }

  mpl_list_add(&msg_descriptor_set_list_p, &msg_set_descr_p->list_entry);

  return mpl_param_init(user_p, log_fp, msg_set_descr_p->param_descr_set_p);
}


/**
 * mpl_msg_alloc_internal - alloc an empty message
 **/
void *mpl_msg_alloc_internal(mpl_msg_type_t type, int param_set_id)
{
  mpl_msg_t *msg;
  mpl_msg_set_descr_t *msg_set_descr_p;

  msg_set_descr_p = msgset_find(param_set_id, NULL);

  if (NULL == msg_set_descr_p)
  {
    DBG_PRINTF("Message set not found\n");
    return (NULL);
  }

  msg = malloc(sizeof(mpl_msg_t));
  if (NULL == msg)
  {
    DBG_PRINTF("Failed allocating memory\n");
    return (NULL);
  }

  msg->common.param_list_p = NULL;
  msg->common.type = type;
  msg->common.message_set_descr_p = msg_set_descr_p;
  return (msg);
}

/**
 * mpl_msg_free - free a buffer previously allocated by
 *                    mpl_msg_alloc()
 **/
void mpl_msg_free(mpl_msg_t *msg)
{
  if (NULL == msg)
    return;

  mpl_param_list_destroy(&msg->common.param_list_p);
  free(msg);
}


/**
 * mpl_msg_pack - pack a message to be sent to psconnctrld 
 **/
int mpl_msg_pack(const mpl_msg_t *msg,
			char **buf,
			size_t *buflen)
{
  char *tmpbuf=NULL;
  size_t templen=0;
  int res=0;
  
  if ((NULL == msg) || (NULL == buf) || (NULL == buflen))
  {
    DBG_PRINTF("msg or buf is NULL\n");
    return -1;
  }

  if (NULL == msg->common.message_set_descr_p)
  {
    DBG_PRINTF("msg->message_set_descr_p is NULL\n");
    return -1;
  }

  /* normally we loop twice before we return a packed request/response/event
   * first round in the loop to calculate the size of the buffer we need to allocate
   * second round in the loop to fill in the data
   */
  while(1)
  {
    /* We use the switch here only to check that the type is legal */
    switch(msg->common.type)
    {
      /* Call appropriate pack_msg_xxx() function for the specified msg ID */
#define LIBMPL_MESSAGE_TYPE_ELEMENT(ELEMENT)          \
      case mpl_msg_type_##ELEMENT:                        \
        res = pack_msg_common(msg, tmpbuf, &templen, msg->common.message_set_descr_p); \
        break;

      /* Note that this is expanded into case statements for all message IDs */
      LIBMPL_MESSAGE_TYPES
#undef LIBMPL_MESSAGE_TYPE_ELEMENT

    default:
      DBG_PRINTF("unknown/unsupported message type %d\n",msg->common.type);
      return (-1);
    }

    if ((res<0) || (NULL != tmpbuf))
      return (res);

    /* we know how big buffer we need */
    templen++; /* add 1 for 0-termination of the string */

    /* buffer already allocated by user? */
    if(NULL != *buf)
    {
      /* check if specified buflen can fit the packed message? */
      if(templen > *buflen)
      {
        DBG_PRINTF("Fatal error: specified buffer is too small\n");
        return (-1);
      }
    }
    else
    {
      *buf = malloc(templen);
      if(NULL == *buf)
      {
        DBG_PRINTF("Failed allocating memory\n");
        *buf=NULL;
        *buflen=0;
        return (-1);
      }
    }
    *buflen = templen;
    tmpbuf = *buf;
    memset(tmpbuf,0,templen);

    /* one more loop to fill in the data */
  }
}


/**
 * mpl_msg_unpack - unpack received messages
 *
 *
 **/
int mpl_msg_unpack(char *buf,
                   size_t buflen,
                   mpl_msg_t *msg)
{
  int msgtype = -1;
  int msgid   = -1;
  int result  = -1;
  bool message_type_present = false;
  bool message_id_present = false;
  bool result_present = false;
  int message_type_paramset_id;
  int message_id_paramset_id;
  int result_paramset_id;
  int i;
  mpl_param_element_t *param_elem_p;
  mpl_list_t *param_list_p=NULL;
  int numargs,res=0;
  mpl_arg_t *args_p;
  mpl_msg_set_descr_t *msg_set_descr_p=NULL;

  MPL_IDENTIFIER_NOT_USED(buflen);

  if ((NULL == msg) || (NULL == buf))
  {
    DBG_PRINTF("msg or buf is NULL\n");
    return -1;
  }

  args_p = malloc(sizeof(mpl_arg_t)*MPL_MAX_ARGS);
  if (NULL == args_p)
  {
    DBG_PRINTF("failed allocating memory\n");
    return -1;
  }
  memset(args_p,0,sizeof(mpl_arg_t)*MPL_MAX_ARGS);
  
  /* Split buffer into array of key, value string pointers */
  numargs = mpl_get_args(args_p, MPL_MAX_ARGS, buf, '=', MESSAGE_DELIMITER, '\\');

  /* Loop over arguments and make list of parameter elements */
  for (i = 0; i < numargs; i++)
  {
    assert(NULL != args_p[i].key_p);

    if (mpl_param_unpack(args_p[i].key_p, args_p[i].value_p, &param_elem_p) < 0)
    {
      DBG_PRINTF("Param unpack failed for param: %s=%s\n", args_p[i].key_p, args_p[i].value_p);
      res = -1;
      goto destroy_and_return;
    }

    msg_set_descr_p = msgset_find(MPL_PARAMID_TO_PARAM_SET_ID(param_elem_p->id), NULL);
    if (NULL == msg_set_descr_p)
    {
      DBG_PRINTF("Message set descriptor not found\n");
      mpl_param_element_destroy(param_elem_p);
      res = -1;
      goto destroy_and_return;
    }

    if (msg_set_descr_p->paramid_msgtype == param_elem_p->id) {
      msgtype = *(int*)param_elem_p->value_p;
      message_type_present = true;
      message_type_paramset_id = MPL_PARAMID_TO_PARAM_SET_ID(param_elem_p->id);
      mpl_param_element_destroy(param_elem_p);
    }
    else if (msg_set_descr_p->paramid_msgid == param_elem_p->id) {
      msgid = *(int*)param_elem_p->value_p;
      message_id_present = true;
      message_id_paramset_id = MPL_PARAMID_TO_PARAM_SET_ID(param_elem_p->id);
      mpl_param_element_destroy(param_elem_p);
    }
    else if (msg_set_descr_p->paramid_result == param_elem_p->id) {
      result = *(int*)param_elem_p->value_p;
      result_present = true;
      result_paramset_id = MPL_PARAMID_TO_PARAM_SET_ID(param_elem_p->id);
      mpl_param_element_destroy(param_elem_p);
    }
    else
      mpl_list_add(&param_list_p, &param_elem_p->list_entry);
  }

  /* Mandatory tags */
  if (false == message_type_present)
  {
    DBG_PRINTF("Message type tag not found!\n");
    res = -1;
    goto destroy_and_return;
  }

  if (false == message_id_present)
  {
    DBG_PRINTF("Message id tag not found!\n");
    res = -1;
    goto destroy_and_return;
  }

  if ((mpl_msg_type_resp == msgtype) && (false == result_present))
  {
    DBG_PRINTF("Result tag not found!\n");
    res = -1;
    goto destroy_and_return;
  }

  if (message_type_paramset_id != message_id_paramset_id)
  {
    DBG_PRINTF("Message set id mismatch (1)!\n");
    res = -1;
    goto destroy_and_return;
  }
  
  if ((mpl_msg_type_resp == msgtype) && (message_type_paramset_id != result_paramset_id))
  {
    DBG_PRINTF("Message set id mismatch (2)!\n");
    res = -1;
    goto destroy_and_return;
  }  

  if(msg_set_descr_p != NULL)
  {
    /* Set message fields */
    msg->common.type = msgtype;
    msg->common.id = msgid;
    msg->common.message_set_descr_p = msgset_find(MPL_PARAMID_TO_PARAM_SET_ID(msg_set_descr_p->paramid_msgtype), NULL);
    msg->common.param_list_p = param_list_p;
    if (mpl_msg_type_resp == msgtype)
    {
      msg->resp.result = result;
    }
    free(args_p);
    return res;
  }

  destroy_and_return:
  mpl_param_list_destroy(&param_list_p);
  free(args_p);
  return res;
}



/*******************************************************************************
 *
 * Private Functions
 *
 *******************************************************************************/

/**
 * msgset_find
 *
 * paramid_set_id <= 0 (illegal) means search using paramid_prefix
 * paramid_prefix == NULL means search using param_set_id
 *
 * If both are defined (>0 and not NULL), both are checked
 *
 * Returns message set descriptor of NULL if not found.
 * 
 */
static mpl_msg_set_descr_t* msgset_find(int param_set_id, char *paramid_prefix)
{
  mpl_list_t *elem_p;
  mpl_msg_set_descr_t *msgset_p = NULL;
  bool id_match = false;
  bool prefix_match = false;

  MPL_LIST_FOR_EACH(msg_descriptor_set_list_p, elem_p)
  {
    msgset_p = MPL_LIST_CONTAINER(elem_p, mpl_msg_set_descr_t, list_entry);

    if ((param_set_id > 0) && (msgset_p->param_descr_set_p->param_set_id == param_set_id))
    {
      id_match = true;
      if (NULL == paramid_prefix)
        return msgset_p;
    }
    if ((NULL != paramid_prefix) && !strncmp(msgset_p->param_descr_set_p->paramid_prefix, paramid_prefix, MPL_PARAMID_PREFIX_MAXLEN))
    {
      prefix_match = true;
      if (param_set_id <= 0)
        return msgset_p;
    }

    if (id_match && prefix_match)
      return msgset_p;

    id_match = false;
    prefix_match = false;
  }

  return NULL;
}


/**
 * pack_msg_common()
 **/
static int pack_msg_common(const mpl_msg_t *msg_p, char* buf, size_t *len_p, const mpl_msg_set_descr_t *msg_set_descr_p)
{
  int total_len = 0;
  int tmplen;
  mpl_list_t *elem_p;
#if 0
  mpl_list_t *list_p;
#endif
  mpl_param_element_t* param_elem_p;
#if 0
  mpl_param_element_t* tmp_elem_p;
#endif
  mpl_list_t *param_list_p = NULL;
  int res;

  assert(NULL != len_p);
  assert(!((*len_p>0) && (NULL == buf)));


#if 0
  /* Copy out all parameters from the message to a separate param list */
  MPL_LIST_FOR_EACH(msg_p->common.param_list_p, list_p)
  {
    param_elem_p = MPL_LIST_CONTAINER(list_p, mpl_param_element_t, list_entry);
    tmp_elem_p = mpl_param_element_clone(param_elem_p);
    if (NULL == tmp_elem_p)
    {
      res = -1;
      goto free_and_return;
    }
    mpl_list_add(&param_list_p, &tmp_elem_p->list_entry);
  }
#endif

  /* Add msgtype to local parameter list */
  res = mpl_add_param_to_list(&param_list_p, msg_set_descr_p->paramid_msgtype, &msg_p->common.type);
  if (res < 0)
  {
    goto free_and_return;
  }

  /* Add msgid to local parameter list */
  res = mpl_add_param_to_list(&param_list_p, msg_set_descr_p->paramid_msgid, &msg_p->common.id);
  if (res < 0)
  {
    goto free_and_return;
  }

  if (mpl_msg_type_resp == msg_p->common.type)
  {
    /* Add result to local parameter list */
    res = mpl_add_param_to_list(&param_list_p, msg_set_descr_p->paramid_result, &msg_p->resp.result);
    if (res < 0) {
      goto free_and_return;
    }
  }


  /* Loop over all parameters in local parameter list and pack them */
  MPL_LIST_FOR_EACH(param_list_p, elem_p)
  {
    /* Add a delimiter between all parameters */
    if (total_len > 0)
    {
      tmplen = snprintf(((size_t)total_len < *len_p)?buf+total_len:NULL,
			((size_t)total_len < *len_p)?*len_p-total_len:0,
			"%c",
			MESSAGE_DELIMITER);
      if (tmplen < 0)
      {
        res = tmplen;
        goto free_and_return;
      }

      total_len += tmplen;
    }

    param_elem_p = MPL_LIST_CONTAINER(elem_p, mpl_param_element_t, list_entry);

    tmplen = mpl_param_pack(param_elem_p,
                            ((size_t)total_len < *len_p)?buf+total_len:NULL,
                            ((size_t)total_len < *len_p)?*len_p-total_len:0);
    if (tmplen < 0)
    {
      res = tmplen;
      goto free_and_return;
    }

    total_len += tmplen;
  }

  /* Loop over all parameters in message parameter list and pack them */
  MPL_LIST_FOR_EACH(msg_p->common.param_list_p, elem_p)
  {
    /* Add a delimiter between all parameters */
    if (total_len > 0)
    {
      tmplen = snprintf(((size_t)total_len < *len_p)?buf+total_len:NULL,
			((size_t)total_len < *len_p)?*len_p-total_len:0,
			"%c",
			MESSAGE_DELIMITER);
      if (tmplen < 0)
      {
        res = tmplen;
        goto free_and_return;
      }

      total_len += tmplen;
    }

    param_elem_p = MPL_LIST_CONTAINER(elem_p, mpl_param_element_t, list_entry);

    tmplen = mpl_param_pack(param_elem_p,
                            ((size_t)total_len < *len_p)?buf+total_len:NULL,
                            ((size_t)total_len < *len_p)?*len_p-total_len:0);
    if (tmplen < 0)
    {
      res = tmplen;
      goto free_and_return;
    }

    total_len += tmplen;
  }

  *len_p = total_len;
  mpl_param_list_destroy(&param_list_p);
  return (0);

  free_and_return:
  mpl_param_list_destroy(&param_list_p);
  return (res);

}

