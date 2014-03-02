/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 * 
 * \brief  Generic Message API
 */


#ifndef LIBMPL_MSG_H
#define LIBMPL_MSG_H

/********************************************************************************
 *
 * Includes
 *
 ********************************************************************************/
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "mpl_list.h"
#include "mpl_param.h"

#ifdef  __cplusplus
extern "C" {
#endif


/********************************************************************************
 *
 * Defines & Type definitions
 *
 ********************************************************************************/

/**
 * This macro can be used to define the structure 'msgsetname_msg_set_descr'
 *
 */
#define MPL_DEFINE_MSG_SET_DESCR(msgsetname) \
static mpl_msg_set_descr_t msgsetname ##_msg_set_descr =  \
{                                                             \
    msgsetname ##_paramid_message_type,                     \
    msgsetname ##_paramid_message,                          \
    msgsetname ##_paramid_result,                           \
    &msgsetname ##_param_descr_set,                      \
    {                                                           \
      NULL                                                      \
    }                                                           \
}


/**
 * mpl_msg_type_t
 *
 * @mpl_msg_type_req
 * @mpl_msg_type_resp
 * @mpl_msg_type_event
 *
 **/
#define LIBMPL_MESSAGE_TYPES		  \
  LIBMPL_MESSAGE_TYPE_ELEMENT(req)		  \
  LIBMPL_MESSAGE_TYPE_ELEMENT(resp)		  \
  LIBMPL_MESSAGE_TYPE_ELEMENT(event)

#define LIBMPL_MESSAGE_TYPE_ELEMENT(ELEMENT) mpl_msg_type_##ELEMENT,

typedef enum
{
  LIBMPL_MESSAGE_TYPES
  mpl_number_of_msg_type
} mpl_msg_type_t;
#undef LIBMPL_MESSAGE_TYPE_ELEMENT

/**
 *
 * Structure describing the message parameters
 *
 * @paramid_msgtype   the parameter id (enum) for the message type tag (in this message set)
 * @paramid_msgid     the parameter id (enum) for the message id tag (in this message set)
 * @paramid_result    the parameter id (enum) for the result tag (in this message set)
 * @param_descr_set_p pointer to the parameter set used
 *                    Note that all the three id's above must be part
 *                    of this parameter set
 **/
typedef struct
{
  int paramid_msgtype;
  int paramid_msgid;
  int paramid_result;
  mpl_param_descr_set_t *param_descr_set_p;
  mpl_list_t list_entry;
} mpl_msg_set_descr_t;

/**
 * mpl_msg_common_t
 *
 * @type                  message type
 * @id                    message id
 * @message_set_descr_p   pointer to message set descriptor
 * @param_list_p          list of parameters
 *
 **/
typedef struct
{
  mpl_msg_type_t type;
  int                   id;
  mpl_msg_set_descr_t *message_set_descr_p;
  mpl_list_t        *param_list_p;
} mpl_msg_common_t;

/**
 * mpl_msg_req_t - Request message
 *
 * @type                  message type
 * @id                    message id
 * @message_set_descr_p   pointer to message set descriptor
 * @param_list_p          List of parameters
 *
 **/
typedef struct
{
  mpl_msg_type_t type;
  int                   id;
  mpl_msg_set_descr_t *message_set_descr_p;
  mpl_list_t        *param_list_p;
} mpl_msg_req_t;

/**
 * mpl_msg_resp_t - Response message
 *
 * @type                  message type
 * @id                    message id
 * @message_set_descr_p   pointer to message set descriptor
 * @param_list_p          List of parameters
 * @result                result of operation
 *
 **/
typedef struct
{
  mpl_msg_type_t type;
  int                   id;
  mpl_msg_set_descr_t *message_set_descr_p;
  mpl_list_t           *param_list_p;
  int                   result;
} mpl_msg_resp_t;

/**
 * mpl_msg_event_t - Event message
 *
 * @type                  message type
 * @id                    message id
 * @message_set_descr_p   pointer to message set descriptor
 * @param_list_p          List of parameters
 *
 **/
typedef struct
{
  mpl_msg_type_t type;
  int                   id;
  mpl_msg_set_descr_t *message_set_descr_p;
  mpl_list_t        *param_list_p;
} mpl_msg_event_t;


/**
 * mpl_msg_t - union holding all message types that can be packed/unpacked
 *
 * @common              message header with param list
 *                      (common for all messages).
 * @req                 Req msg (common for all requests).
 * @resp                Resp msg (common for all responses).
 * @event               Event msg (common for all events).
 *
 **/
typedef union
{
  mpl_msg_common_t          common;
  mpl_msg_req_t             req;
  mpl_msg_resp_t            resp;
  mpl_msg_event_t           event;
} mpl_msg_t;



/*******************************************************************************
 *
 * Functions and macros
 *
 *******************************************************************************/

/**
 * mpl_init - Initiate library
 *
 * @user_p             User pointer
 * @log_fp             Logging function
 * @msg_set_descr_p    pointer to message set descriptor struct
 *
 * Returns:  0 on success, -1 on failure
 *
 **/
int mpl_msg_init(void* user_p, mpl_log_fp log_fp, mpl_msg_set_descr_t *msg_set_descr_p);


/**
 * mpl_msg_buf_free - free a buffer previously allocated by
 *                    mpl_msg_pack() 
 *
 * @buf      memory to be freed
 *
 * Returns:  -
 *
 **/
#define mpl_msg_buf_free(BUF) free((BUF))

/**
 * mpl_req_msg_alloc - alloc an empty request message
 *
 * Returns:  Allocated message or NULL if allocation failed
 *
 **/
#define mpl_req_msg_alloc(param_set_id) mpl_msg_alloc_internal(mpl_msg_type_req, (param_set_id))

/**
 * mpl_resp_msg_alloc - alloc an empty response message
 *
 * Returns:  Allocated message or NULL if allocation failed
 *
 **/
#define mpl_resp_msg_alloc(param_set_id) mpl_msg_alloc_internal(mpl_msg_type_resp, (param_set_id))

/**
 * mpl_event_msg_alloc - alloc an empty event message
 *
 * Returns:  Allocated message or NULL if allocation failed
 *
 **/
#define mpl_event_msg_alloc(param_set_id) mpl_msg_alloc_internal(mpl_msg_type_event, (param_set_id))

/**
 * mpl_msg_alloc - alloc an empty generic message (NOTE: user must set the type)
 *
 * Returns:  Allocated message or NULL if allocation failed
 *
 **/
#define mpl_msg_alloc(param_set_id) mpl_msg_alloc_internal(mpl_msg_type_req, (param_set_id))

/**
 * mpl_msg_alloc_internal - alloc an empty message (only to be used internally)
 *
 * @type          message type (req, resp, event)
 * @param_set_id  parameter set identifier
 * Returns:  Allocated message or NULL if allocation failed
 *
 **/
  void *mpl_msg_alloc_internal(mpl_msg_type_t type, int param_set_id);

/**
 * mpl_msg_free - free a buffer previously allocated by
 *                    mpl_msg_alloc() 
 *
 * @msg      memory to be freed
 *
 * Returns:  -
 *
 **/
void mpl_msg_free(mpl_msg_t *msg);

/**
 * mpl_msg_pack - pack a message
 *
 * @msg     message to be packed
 * @buf     buffer to store packed message
 *          if set to NULL the buffer is allocated by the pack function and must
 *          be released by the caller by calling mpl_msg_buf_free()
 *
 *          Otherwise the specified buffer will be used. Packing will fail
 *          if the specified buffer length is too small.
 *
 * @buflen  length of packed buffer
 * Returns:  (0) on success, (-1) on error
 *
 **/
int mpl_msg_pack(const mpl_msg_t *msg,
			char **buf,
			size_t *buflen);

/**
 * mpl_msg_unpack - unpack received message
 *
 * @buf      message to be unpacked
 * @buflen   length of the message to be unpacked
 * @msg      unpacked message
 *
 * Returns: (0) on success, (-1) on error
 *
 **/
int mpl_msg_unpack(char *buf,
		       size_t buflen,
		       mpl_msg_t *msg);


/**
 * mpl_msg_add_param - add parameter to message
 *
 * @msg_p    pointer to message
 * @paramid  id of parameter
 * @value_p  pointer to value of parameter
 *
 * Returns: (0) on success, (-1) on error
 *
 **/
#define mpl_msg_add_param(msg_p, paramid, value_p) mpl_add_param_to_list(&(msg_p)->common.param_list_p, (paramid), (value_p))


#define MPL_MSG_PARAM_PRESENT(msg_p, paramid) \
  MPL_PARAM_PRESENT_IN_LIST(paramid,(msg_p)->common.param_list_p)
#define MPL_MSG_GET_PARAM_VALUE(msg_p, type, paramid) \
  MPL_GET_PARAM_VALUE_FROM_LIST(type,paramid,(msg_p)->common.param_list_p)
#define MPL_MSG_GET_PARAM_VALUE_REF(msg_p, reftype, paramid) \
  MPL_GET_PARAM_VALUE_REF_FROM_LIST(reftype,paramid,(msg_p)->common.param_list_p)
#define MPL_MSG_GET_PARAM_VALUE_STRING(msg_p, type, paramid) \
  mpl_param_value_get_string((paramid), MPL_MSG_GET_PARAM_VALUE_REF((msg_p), type*, (paramid)))

#ifdef  __cplusplus
}
#endif

#endif /* LIBMPL_MSG_H */
