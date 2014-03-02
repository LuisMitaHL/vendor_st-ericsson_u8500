/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \brief  Message definitions for STE RC protocol
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
#include "sterc_msg.h"


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
  if (NULL != sterc_log_func) \
    (*sterc_log_func)(sterc_log_user_p, LOG_DEBUG, __FILE__, __LINE__, format, ##__VA_ARGS__)

#define MESSAGE_DELIMITER ','


/********************************************************************************
 *
 * Global  variables
 *
 ********************************************************************************/
const char sterc_event_failure_buf[] = "message=event_failure";


/********************************************************************************
 *
 * Local variables
 *
 ********************************************************************************/

static mpl_log_fp sterc_log_func = NULL;
static void* sterc_log_user_p = NULL;

#ifndef STERC_COMPILE_TIME_ASSERT
#define STERC_COMPILE_TIME_ASSERT(x) (void)(1/(int)(x))
#endif

#define STERC_MESSAGE_TYPE_ELEMENT(ELEMENT) #ELEMENT,
static const char* sterc_names_msgtype[] =
{
  STERC_MESSAGE_TYPES
};
#undef STERC_MESSAGE_TYPE_ELEMENT

#define STERC_MESSAGE_ID_ELEMENT(ELEMENT) #ELEMENT,
static const char* sterc_names_msgid[] =
{
  STERC_MESSAGE_IDS
};
#undef STERC_MESSAGE_ID_ELEMENT

#define STERC_RESULT_VALUE_ELEMENT(ELEMENT) #ELEMENT,
static const char* sterc_names_result[] =
{
  STERC_RESULT_VALUES
};
#undef STERC_RESULT_VALUE_ELEMENT

#define STERC_WAN_AUTH_METHOD_ELEMENT(LEVEL) #LEVEL,
static const char* sterc_names_wan_auth_method[] =
{
  STERC_WAN_AUTH_METHOD
};
#undef STERC_WAN_AUTH_METHOD_ELEMENT

#define STERC_MODE_ELEMENT(LEVEL) #LEVEL,
static const char* sterc_names_mode[] =
{
  STERC_MODE
};
#undef STERC_MODE_ELEMENT

#define STERC_REASON_ELEMENT(LEVEL) #LEVEL,
static const char* sterc_names_reason[] =
{
  STERC_REASON
};
#undef STERC_REASON_ELEMENT

/********************************************************************************
 *
 * Private function prototypes
 *
 ********************************************************************************/
/* Dummies */
static const char* sterc_names_dummy[] =
{
  ""
};

#define sterc_pre_string char
#define sterc_pre_int    int
#define sterc_pre_uint8  uint8_t
#define sterc_pre_uint32 uint32_t
#define sterc_pre_enum
#define sterc_pre_bool   bool
#define sterc_pre_uint8_array  mpl_uint8_array_t
#define sterc_pre_string_tuple  mpl_string_tuple_t
#define sterc_post_string []
#define sterc_post_int
#define sterc_post_uint8
#define sterc_post_uint32
#define sterc_post_enum
#define sterc_post_bool
#define sterc_post_uint8_array
#define sterc_post_string_tuple
#define sterc_dummy_t

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
#define set_string_tuple_no_default {NULL,NULL}

#define set_max(var) var
#define set_no_max 0

#define STERC_PARAMETER_ID_ELEMENT(ELEMENT, TYPE, EXTRA, MAX, SET, GET, CONFIG, DEFAULT) \
const sterc_pre_##TYPE sterc_ ##EXTRA ##_t  sterc_default_##ELEMENT sterc_post_##TYPE = \
      set_##TYPE ##_ ##DEFAULT; \
const uint64_t sterc_max_##ELEMENT = set_##MAX;
STERC_PARAMETER_IDS
#undef STERC_PARAMETER_ID_ELEMENT


#define is_default(var) true
#define is_no_default false
#define is_max(var) true
#define is_no_max false

#define STERC_PARAMETER_ID_ELEMENT(ELEMENT, TYPE, EXTRA, MAX, SET, GET, CONFIG, DEFAULT) \
  { #ELEMENT,                                                                \
    (SET),                                                                   \
    (GET),                                                                   \
    (CONFIG),                                                                \
    (is_##DEFAULT )?&sterc_default_##ELEMENT :NULL,                          \
    (is_##MAX)?&sterc_max_##ELEMENT : NULL,                                  \
    mpl_pack_param_value_##TYPE,                                         \
    mpl_unpack_param_value_##TYPE,                                       \
    mpl_clone_param_value_##TYPE,                                        \
    mpl_compare_param_value_##TYPE,                                      \
    mpl_free_param_value_##TYPE,                                      \
    sterc_names_##EXTRA,                                                   \
    (sterc_names_##EXTRA==NULL)?0:ARRAY_SIZE(sterc_names_##EXTRA)        \
  },
static mpl_param_descr_t sterc_param_descr[] =
{
  STERC_PARAMETER_IDS
};
#undef STERC_PARAMETER_ID_ELEMENT


MPL_DEFINE_PARAM_DESCR_SET(sterc, STERC);
MPL_DEFINE_MSG_SET_DESCR(sterc);


/*******************************************************************************
 *
 * Public Functions
 *
 *******************************************************************************/

/**
 * sterc_init - Initiate library
 **/
int sterc_init(void* user_p, mpl_log_fp log_fp)
{
  /* Assert that no max values are set too high */
#define STERC_PARAMETER_ID_ELEMENT(ELEMENT, TYPE, EXTRA, MAX, SET, GET, CONFIG, DEFAULT) \
STERC_COMPILE_TIME_ASSERT( set_##MAX <= max_limit_##TYPE);
STERC_PARAMETER_IDS
#undef STERC_PARAMETER_ID_ELEMENT

  sterc_log_user_p = user_p;
  sterc_log_func = log_fp;

  return mpl_msg_init(user_p, log_fp, &sterc_msg_set_descr);
}

/*******************************************************************************
 *
 * Private Functions
 *
 *******************************************************************************/


