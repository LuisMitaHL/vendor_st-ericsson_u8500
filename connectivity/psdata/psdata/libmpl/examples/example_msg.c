/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \brief  Request, Response and Event functions for service category:
 *         Packet Switched Services (PSS)
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
#include "example_msg.h"


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
  if (NULL != example_log_func) \
    (*example_log_func)(example_log_user_p, LOG_DEBUG, __FILE__, __LINE__, format, ##__VA_ARGS__)

#define MESSAGE_DELIMITER ','


/********************************************************************************
 *
 * Global  variables
 *
 ********************************************************************************/
const char example_event_failure_buf[] = "message=event_failure";


/********************************************************************************
 *
 * Local variables
 *
 ********************************************************************************/

static mpl_log_fp example_log_func = NULL;
static void* example_log_user_p = NULL;

#ifndef EXAMPLE_COMPILE_TIME_ASSERT
#define EXAMPLE_COMPILE_TIME_ASSERT(x) (void)(1/(int)(x))
#endif

#define EXAMPLE_MESSAGE_TYPE_ELEMENT(ELEMENT) #ELEMENT,
static const char *example_names_msgtype[] =
{
  EXAMPLE_MESSAGE_TYPES
};
#undef EXAMPLE_MESSAGE_TYPE_ELEMENT
 
#define EXAMPLE_MESSAGE_ID_ELEMENT(ELEMENT) #ELEMENT,
static const char *example_names_msgid[] =
{
  EXAMPLE_MESSAGE_IDS
};
#undef EXAMPLE_MESSAGE_ID_ELEMENT
 
#define EXAMPLE_RESULT_VALUE_ELEMENT(ELEMENT) #ELEMENT,
static const char *example_names_result[] =
{
  EXAMPLE_RESULT_VALUES
};
#undef EXAMPLE_RESULT_VALUE_ELEMENT
 
#define EXAMPLE_MY_ENUM_ELEMENT(LEVEL) #LEVEL,
static const char *example_names_my_enum[] =
{
  EXAMPLE_MY_ENUM
};
#undef EXAMPLE_MY_ENUM_ELEMENT



/********************************************************************************
 *
 * Private function prototypes
 *
 ********************************************************************************/

/* Dummies */
#define example_names_dummy NULL
#define example_number_of_dummy 0

/* Some magic for default values */
#define example_pre_string char
#define example_pre_int    int
#define example_pre_uint8  uint8_t
#define example_pre_uint32 uint32_t
#define example_pre_enum 
#define example_pre_bool   bool
#define example_post_string []
#define example_post_int    
#define example_post_uint8
#define example_post_uint32
#define example_post_enum 
#define example_post_bool   
#define example_dummy_t

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

#define set_max(var) var
#define set_no_max 0

#define EXAMPLE_PARAMETER_ID_ELEMENT(ELEMENT, TYPE, EXTRA, MAX, SET, GET, CONFIG, DEFAULT) \
const example_pre_##TYPE example_ ##EXTRA ##_t  example_default_##ELEMENT example_post_##TYPE = \
      set_##TYPE ##_ ##DEFAULT; \
const uint64_t example_max_##ELEMENT = set_##MAX;  
EXAMPLE_PARAMETER_IDS
#undef EXAMPLE_PARAMETER_ID_ELEMENT
    

#define is_default(var) true
#define is_no_default false
#define is_max(var) true
#define is_no_max false

/**
 * Macro used to populate the param descriptor array.
 *
 *
 */
  
#define EXAMPLE_PARAMETER_ID_ELEMENT(ELEMENT, TYPE, EXTRA, MAX, SET, GET, CONFIG, DEFAULT) \
  { #ELEMENT,                                                                \
    (SET),                                                                   \
    (GET),                                                                   \
    (CONFIG),                                                                \
    (is_##DEFAULT )?&example_default_##ELEMENT :NULL,                                \
    (is_##MAX)?&example_max_##ELEMENT : NULL,                         \
    mpl_pack_param_value_##TYPE,                                \
    mpl_unpack_param_value_##TYPE,                             \
    mpl_clone_param_value_##TYPE,                                        \
    mpl_compare_param_value_##TYPE,                                      \
    mpl_free_param_value_##TYPE,                                      \
    example_names_##EXTRA,                                   \
    (example_names_##EXTRA==NULL)?0:ARRAY_SIZE(example_names_##EXTRA)        \
  },
  
static const mpl_param_descr_t example_param_descr[] =
{
  EXAMPLE_PARAMETER_IDS
};
#undef EXAMPLE_PARAMETER_ID_ELEMENT

MPL_DEFINE_PARAM_DESCR_SET(example, EXAMPLE);
MPL_DEFINE_MSG_SET_DESCR(example);

/*******************************************************************************
 *
 * Public Functions
 *
 *******************************************************************************/

/**
 * example_init - Initiate library
 **/
int example_init(void* user_p, mpl_log_fp log_fp)
{
  /* Assert that no max values are set too high */
#define EXAMPLE_PARAMETER_ID_ELEMENT(ELEMENT, TYPE, EXTRA, MAX, SET, GET, CONFIG, DEFAULT) \
EXAMPLE_COMPILE_TIME_ASSERT( set_##MAX <= max_limit_##TYPE);
EXAMPLE_PARAMETER_IDS
#undef EXAMPLE_PARAMETER_ID_ELEMENT

  example_log_user_p = user_p;
  example_log_func = log_fp;

  return mpl_msg_init(user_p, log_fp, &example_msg_set_descr);
}



/*******************************************************************************
 *
 * Private Functions
 *
 *******************************************************************************/


