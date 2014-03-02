/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \brief  PS Connection Control message definitions
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
#include "pscc_msg.h"


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
  if (NULL != pscc_log_func) \
    (*pscc_log_func)(pscc_log_user_p, LOG_DEBUG, __FILE__, __LINE__, format, ##__VA_ARGS__)

#define MESSAGE_DELIMITER ','


/********************************************************************************
 *
 * Global  variables
 *
 ********************************************************************************/
const char pscc_event_failure_buf[] = "message=event_failure";

#define PSCC_CONNECTION_STATUS_ELEMENT(VAL) #VAL,
const char* pscc_names_connection_status[] =
{
  PSCC_CONNECTION_STATUS
};
#undef PSCC_CONNECTION_STATUS_ELEMENT

/********************************************************************************
 *
 * Local variables
 *
 ********************************************************************************/

static mpl_log_fp pscc_log_func = NULL;
static void* pscc_log_user_p = NULL;

#ifndef PSCC_COMPILE_TIME_ASSERT
#define PSCC_COMPILE_TIME_ASSERT(x) (void)(1/(int)(x))
#endif

#define PSCC_MESSAGE_TYPE_ELEMENT(ELEMENT) #ELEMENT,
static const char* pscc_names_msgtype[] =
{
  PSCC_MESSAGE_TYPES
};
#undef PSCC_MESSAGE_TYPE_ELEMENT

#define PSCC_MESSAGE_ID_ELEMENT(ELEMENT) #ELEMENT,
static const char* pscc_names_msgid[] =
{
  PSCC_MESSAGE_IDS
};
#undef PSCC_MESSAGE_ID_ELEMENT

#define PSCC_RESULT_VALUE_ELEMENT(ELEMENT) #ELEMENT,
static const char* pscc_names_result[] =
{
  PSCC_RESULT_VALUES
};
#undef PSCC_RESULT_VALUE_ELEMENT

#define PSCC_PDP_TYPE_ELEMENT(LEVEL) #LEVEL,
static const char* pscc_names_pdp_type[] =
{
  PSCC_PDP_TYPE
};
#undef PSCC_PDP_TYPE_ELEMENT

#define PSCC_AUTH_METHOD_ELEMENT(LEVEL) #LEVEL,
static const char* pscc_names_auth_method[] =
{
  PSCC_AUTH_METHOD
};
#undef PSCC_AUTH_METHOD_ELEMENT

#define PSCC_PREFERRED_SERVICE_ELEMENT(LEVEL) #LEVEL,
static const char* pscc_names_preferred_service[] =
{
  PSCC_PREFERRED_SERVICE
};
#undef PSCC_PREFERRED_SERVICE_ELEMENT

#define PSCC_QOS_TYPE_ELEMENT(LEVEL) #LEVEL,
static const char* pscc_names_qos_type[] =
{
  PSCC_QOS_TYPE
};
#undef PSCC_QOS_TYPE_ELEMENT

#define PSCC_QOS_PRECEDENCE_CLASS_ELEMENT(LEVEL) #LEVEL,
static const char* pscc_names_qos_precedence_class[] =
{
  PSCC_QOS_PRECEDENCE_CLASS
};
#undef PSCC_QOS_PRECEDENCE_CLASS_ELEMENT

#define PSCC_QOS_DELAY_CLASS_ELEMENT(LEVEL) #LEVEL,
static const char* pscc_names_qos_delay_class[] =
{
  PSCC_QOS_DELAY_CLASS
};
#undef PSCC_QOS_DELAY_CLASS_ELEMENT

#define PSCC_QOS_RELIABILITY_CLASS_ELEMENT(LEVEL) #LEVEL,
static const char* pscc_names_qos_reliability_class[] =
{
  PSCC_QOS_RELIABILITY_CLASS
};
#undef PSCC_QOS_RELIABILITY_CLASS_ELEMENT

#define PSCC_QOS_PEAK_THROUGHPUT_CLASS_ELEMENT(LEVEL) #LEVEL,
static const char* pscc_names_qos_peak_throughput_class[] =
{
  PSCC_QOS_PEAK_THROUGHPUT_CLASS
};
#undef PSCC_QOS_PEAK_THROUGHPUT_CLASS_ELEMENT

#define PSCC_QOS_MEAN_THROUGHPUT_CLASS_ELEMENT(LEVEL) #LEVEL,
static const char* pscc_names_qos_mean_throughput_class[] =
{
  PSCC_QOS_MEAN_THROUGHPUT_CLASS
};
#undef PSCC_QOS_MEAN_THROUGHPUT_CLASS_ELEMENT

#define PSCC_QOS_TRAFFIC_CLASS_ELEMENT(LEVEL) #LEVEL,
static const char* pscc_names_qos_traffic_class[] =
{
  PSCC_QOS_TRAFFIC_CLASS
};
#undef PSCC_QOS_TRAFFIC_CLASS_ELEMENT

#define PSCC_QOS_DELIVERY_ORDER_ELEMENT(LEVEL) #LEVEL,
static const char* pscc_names_qos_delivery_order[] =
{
  PSCC_QOS_DELIVERY_ORDER
};
#undef PSCC_QOS_DELIVERY_ORDER_ELEMENT

#define PSCC_QOS_DELIVERY_ERRONEOUS_SDU_ELEMENT(LEVEL) #LEVEL,
static const char* pscc_names_qos_delivery_erroneous_sdu[] =
{
  PSCC_QOS_DELIVERY_ERRONEOUS_SDU
};
#undef PSCC_QOS_DELIVERY_ERRONEOUS_SDU_ELEMENT

#define PSCC_QOS_RESIDUAL_BER_ELEMENT(LEVEL) #LEVEL,
static const char* pscc_names_qos_residual_ber[] =
{
  PSCC_QOS_RESIDUAL_BER
};
#undef PSCC_QOS_RESIDUAL_BER_ELEMENT

#define PSCC_QOS_SDU_ERROR_RATIO_ELEMENT(LEVEL) #LEVEL,
static const char* pscc_names_qos_sdu_error_ratio[] =
{
  PSCC_QOS_SDU_ERROR_RATIO
};
#undef PSCC_QOS_SDU_ERROR_RATIO_ELEMENT

#define PSCC_QOS_TRAFFIC_HANDLING_PRIORITY_ELEMENT(LEVEL) #LEVEL,
static const char* pscc_names_qos_traffic_handling_priority[] =
{
  PSCC_QOS_TRAFFIC_HANDLING_PRIORITY
};
#undef PSCC_QOS_TRAFFIC_HANDLING_PRIORITY_ELEMENT

#define PSCC_QOS_SIGNALLING_INDICATION_ELEMENT(LEVEL) #LEVEL,
static const char* pscc_names_qos_signalling_indication[] =
{
  PSCC_QOS_SIGNALLING_INDICATION
};
#undef PSCC_QOS_SIGNALLING_INDICATION_ELEMENT

#define PSCC_QOS_SOURCE_STATISTICS_DESCRIPTOR_ELEMENT(LEVEL) #LEVEL,
static const char* pscc_names_qos_source_statistics_descriptor[] =
{
  PSCC_QOS_SOURCE_STATISTICS_DESCRIPTOR
};
#undef PSCC_QOS_SOURCE_STATISTICS_DESCRIPTOR_ELEMENT

#define PSCC_REASON_ELEMENT(LEVEL) #LEVEL,
static const char* pscc_names_reason[] =
{
  PSCC_REASON
};
#undef PSCC_REASON_ELEMENT

#define PSCC_ATTACH_STATUS_ELEMENT(LEVEL) #LEVEL,
static const char* pscc_names_attach_status[] =
{
  PSCC_ATTACH_STATUS
};
#undef PSCC_ATTACH_STATUS_ELEMENT

#define PSCC_ATTACH_MODE_ELEMENT(LEVEL) #LEVEL,
static const char* pscc_names_attach_mode[] =
{
  PSCC_ATTACH_MODE
};
#undef PSCC_ATTACH_MODE_ELEMENT

#define PSCC_PING_BLOCK_MODE_ELEMENT(LEVEL) #LEVEL,
static const char* pscc_names_ping_block_mode[] =
{
  PSCC_PING_BLOCK_MODE
};
#undef PSCC_PING_BLOCK_MODE_ELEMENT

/********************************************************************************
 *
 * Private function prototypes
 *
 ********************************************************************************/


#define pack_req_create(msg, buf, len_p) pack_msg_req(&msg->req, buf, len_p)
#define pack_resp_create(msg, buf,  len_p) pack_msg_resp(&msg->resp, buf, len_p)
#define pack_req_destroy(msg, buf, len_p) pack_msg_req(&msg->req, buf, len_p)
#define pack_resp_destroy(msg, buf,  len_p) pack_msg_resp(&msg->resp, buf, len_p)
#define pack_req_connect(msg, buf, len_p) pack_msg_req(&msg->req, buf, len_p)
#define pack_resp_connect(msg, buf,  len_p) pack_msg_resp(&msg->resp, buf, len_p)
#define pack_req_disconnect(msg, buf, len_p) pack_msg_req(&msg->req, buf, len_p)
#define pack_resp_disconnect(msg, buf,  len_p) pack_msg_resp(&msg->resp, buf, len_p)

#define pack_req_set(msg, buf,  len_p) pack_msg_req(&msg->req, buf, len_p)
#define pack_resp_set(msg, buf,  len_p) pack_msg_resp(&msg->resp, buf, len_p)
#define pack_req_get(msg, buf,  len_p) pack_msg_req(&msg->req, buf, len_p)
#define pack_resp_get(msg, buf,  len_p) pack_msg_resp(&msg->resp, buf, len_p)

#define pack_req_list_all_connections(msg, buf,  len_p) pack_msg_req(&msg->req, buf, len_p)
#define pack_resp_list_all_connections(msg, buf,  len_p) pack_msg_resp(&msg->resp, buf, len_p)

#define pack_event_connecting(msg, buf, len_p) pack_msg_event(&msg->event, buf, len_p)
#define pack_event_connected(msg, buf, len_p) pack_msg_event(&msg->event, buf, len_p)
#define pack_event_disconnecting(msg, buf, len_p) pack_msg_event(&msg->event, buf, len_p)
#define pack_event_disconnected(msg, buf, len_p) pack_msg_event(&msg->event, buf, len_p)
#define pack_event_failure(msg, buf, len_p) pack_msg_event(&msg->event, buf, len_p)

/* Dummies */
static const char* pscc_names_dummy[] =
{
  ""
};

#define pscc_pre_string char
#define pscc_pre_int    int
#define pscc_pre_uint8  uint8_t
#define pscc_pre_uint32 uint32_t
#define pscc_pre_enum
#define pscc_pre_bool   bool
#define pscc_pre_uint8_array  mpl_uint8_array_t
#define pscc_post_string []
#define pscc_post_int
#define pscc_post_uint8
#define pscc_post_uint32
#define pscc_post_enum
#define pscc_post_bool
#define pscc_post_uint8_array
#define pscc_dummy_t

#define set_string_default(var) var
#define set_enum_default(var) var
#define set_int_default(var) var
#define set_uint8_default(var) var
#define set_uint32_default(var) var
#define set_bool_default(var) var

#define set_string_no_default ""
#define set_enum_no_default 0
#define set_int_no_default 0
#define set_uint8_no_default 0
#define set_uint32_no_default 0
#define set_bool_no_default false
#define set_uint8_array_no_default {0,NULL}

#define set_max(var) var
#define set_no_max 0

#define PSCC_PARAMETER_ID_ELEMENT(ELEMENT, TYPE, EXTRA, MAX, SET, GET, CONFIG, DEFAULT) \
const pscc_pre_##TYPE pscc_ ##EXTRA ##_t  pscc_default_##ELEMENT pscc_post_##TYPE = \
      set_##TYPE ##_ ##DEFAULT; \
const uint64_t pscc_max_##ELEMENT = set_##MAX;
PSCC_PARAMETER_IDS
#undef PSCC_PARAMETER_ID_ELEMENT


#define is_default(var) true
#define is_no_default false
#define is_max(var) true
#define is_no_max false

#define PSCC_PARAMETER_ID_ELEMENT(ELEMENT, TYPE, EXTRA, MAX, SET, GET, CONFIG, DEFAULT) \
  { #ELEMENT,                                                                \
    (SET),                                                                   \
    (GET),                                                                   \
    (CONFIG),                                                                \
    (is_##DEFAULT )?&pscc_default_##ELEMENT :NULL,                           \
    (is_##MAX)?&pscc_max_##ELEMENT : NULL,                                   \
    mpl_pack_param_value_##TYPE,                                         \
    mpl_unpack_param_value_##TYPE,                                       \
    mpl_clone_param_value_##TYPE,                                        \
    mpl_compare_param_value_##TYPE,                                      \
    mpl_free_param_value_##TYPE,                                      \
    pscc_names_##EXTRA,                                                   \
    (pscc_names_##EXTRA==NULL)?0:ARRAY_SIZE(pscc_names_##EXTRA)        \
  },
static mpl_param_descr_t pscc_param_descr[] =
{
  PSCC_PARAMETER_IDS
};
#undef PSCC_PARAMETER_ID_ELEMENT


MPL_DEFINE_PARAM_DESCR_SET(pscc, PSCC);
MPL_DEFINE_MSG_SET_DESCR(pscc);


/*******************************************************************************
 *
 * Public Functions
 *
 *******************************************************************************/

/**
 * pscc_init - Initiate library
 **/
int pscc_init(void* user_p, mpl_log_fp log_fp)
{
  /* Assert that no max values are set too high */
#define PSCC_PARAMETER_ID_ELEMENT(ELEMENT, TYPE, EXTRA, MAX, SET, GET, CONFIG, DEFAULT) \
PSCC_COMPILE_TIME_ASSERT( set_##MAX <= max_limit_##TYPE);
PSCC_PARAMETER_IDS
#undef PSCC_PARAMETER_ID_ELEMENT

  pscc_log_user_p = user_p;
  pscc_log_func = log_fp;

  return mpl_msg_init(user_p, log_fp, &pscc_msg_set_descr);
}


/**
 * pscc_msg_id_get_string
 */
const char *
pscc_msg_id_get_string(pscc_msgid_t msg_id)
{
  return mpl_param_value_get_string(pscc_paramid_message, &msg_id);
}


/*******************************************************************************
 *
 * Private Functions
 *
 *******************************************************************************/


