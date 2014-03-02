/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef EXAMPLE_MSG_H
#define EXAMPLE_MSG_H

#include <stdbool.h>
#include <stdint.h>
#include "mpl_list.h"
#include "mpl_param.h"
#include "mpl_msg.h"

#ifdef  __cplusplus
extern "C" {
#endif

/**
 * These values must be unique among all parameter sets.
 *
 * - *_PARAM_SET_ID is a unique, positive, small integer number
 * - *_PARAMID_PREFIX is a name reflecting the message set
 *
 */
#define EXAMPLE_PARAM_SET_ID 1
#define EXAMPLE_PARAMID_PREFIX "example"

/**
 * example_event_failure_buf - Pre-packed message buffer for failure event
 */
extern const char example_event_failure_buf[];

/**
 * example_msgtype_t (MANDATORY)
 *
 * This enum identifies the message types in the protocol
 *
 * Note: These enum values (suffixes 'req', 'resp', 'event') are mandatory and must be set equal to
 *       the library enum values for message types: mpl_msg_type_req, mpl_msg_type_resp, mpl_msg_type_event.
 *
 * @example_msgtype_req
 * @example_msgtype_resp
 * @example_msgtype_event
 *
 **/
#define EXAMPLE_MESSAGE_TYPES		  \
  EXAMPLE_MESSAGE_TYPE_ELEMENT(req)		  \
  EXAMPLE_MESSAGE_TYPE_ELEMENT(resp)		  \
  EXAMPLE_MESSAGE_TYPE_ELEMENT(event)

#define EXAMPLE_MESSAGE_TYPE_ELEMENT(ELEMENT) example_msgtype_##ELEMENT = mpl_msg_type_##ELEMENT,
typedef enum
{
  EXAMPLE_MESSAGE_TYPES
  example_number_of_msgtype = mpl_number_of_msg_type
} example_msgtype_t;
#undef EXAMPLE_MESSAGE_TYPE_ELEMENT


/**
 * example_msgid_t (MANDATORY)
 *
 * This enum lists the different messages in the protocol. Together with the message type
 * this enum will identify the semantics (meaning) of a particular message.
 *
 * @example_mycommand
 * @example_myevent
 *
 **/
#define EXAMPLE_MESSAGE_IDS                       \
  EXAMPLE_MESSAGE_ID_ELEMENT(mycommand)		  \
  EXAMPLE_MESSAGE_ID_ELEMENT(myevent)

#define EXAMPLE_MESSAGE_ID_ELEMENT(ELEMENT) example_##ELEMENT,
typedef enum 
{
  EXAMPLE_MESSAGE_IDS
  example_number_of_msgids
} example_msgid_t;
#undef EXAMPLE_MESSAGE_ID_ELEMENT
  
/**
 * example_result_t (MANDATORY)
 *
 * This enum is used in messages of type "resp" to indicate the status of a previously sent request (type "req").
 *
 * @example_result_ok
 * @example_result_failed_for_a_reason
 *
 **/
#define EXAMPLE_RESULT_VALUES                                \
  EXAMPLE_RESULT_VALUE_ELEMENT(ok)                           \
  EXAMPLE_RESULT_VALUE_ELEMENT(failed_for_a_reason) \

#define EXAMPLE_RESULT_VALUE_ELEMENT(ELEMENT) example_result_##ELEMENT,
typedef enum 
{
  EXAMPLE_RESULT_VALUES
  example_number_of_results
} example_result_t;
#undef EXAMPLE_RESULT_VALUE_ELEMENT


/**
 * example_paramid_t (MANDATORY)
 *
 * This enum lists all parameters in the protocol.
 *
 * Note: Three parameters are mandatory: message type, message id and result. The names of these parameters
 *       can be chosen freely, but the types can not. They must be the same as the ones defined above.
 *
 * Parameter IDs
 *
 * @example_paramid_message_type                                  - Message type (example_msgtype_t)  (MANDATORY)
 * @example_paramid_message                                       - Message (example_msgid_t)         (MANDATORY)
 * @example_paramid_result                                        - Message result (example_result_t) (MANDATORY)
 * @example_paramid_mystring                                      - String parameter
 * @example_paramid_myint                                         - Integer parameter
 * @example_paramid_myenum                                        - Enum parameter
 * @example_paramid_mybool                                        - Bool parameter
 * @example_paramid_myuint8                                       - Unsigned byte parameter
 * @example_paramid_myuint32                                      - Unsigned integer parameter
 *
 **/
#define EXAMPLE_PARAMETER_IDS                                           \
  /* Parameter description:     Name           Type    Enum      MAX,       Set?   Get?   Config? Default Value */ \
  EXAMPLE_PARAMETER_ID_ELEMENT(message_type,  enum,   msgtype,  no_max,    false, false, false,  no_default) \
  EXAMPLE_PARAMETER_ID_ELEMENT(message,       enum,   msgid,    no_max,    false, false, false,  no_default) \
  EXAMPLE_PARAMETER_ID_ELEMENT(result,        enum,   result,   no_max,    false, false, false,  no_default)\
  EXAMPLE_PARAMETER_ID_ELEMENT(mystring,      string, dummy,    max(99),   true,  true,  true,   default("")) \
  EXAMPLE_PARAMETER_ID_ELEMENT(myint,         int,    dummy,    max(1000), false, false, false,  default(0)) \
  EXAMPLE_PARAMETER_ID_ELEMENT(myenum,        enum,   my_enum,  no_max,    false, false, false,  no_default) \
  EXAMPLE_PARAMETER_ID_ELEMENT(mybool,        bool,   dummy,    no_max,    false, false, false,  no_default) \
  EXAMPLE_PARAMETER_ID_ELEMENT(myuint8,       uint8,  dummy,    no_max,    false, false, false,  no_default) \
  EXAMPLE_PARAMETER_ID_ELEMENT(myuint32,      uint32, dummy,    no_max,    false, false, false,  no_default) \
  
#define EXAMPLE_PARAMETER_ID_ELEMENT(ELEMENT, TYPE, EXTRA, MAX, SET, GET, CONFIG, DEFAULT) \
  example_paramid_##ELEMENT,

/**
 * @example_paramid_base     - This marks the base of the paramid numbers of this parameter set (not
 *                              itself a valid paramid. The value is generated from the param-set-id
 *                              using the macro MPL_PARAM_SET_ID_TO_PARAMID_BASE().
 * @EXAMPLE_PARAMETER_IDS    - This expands to all the paramid names.
 * @example_end_of_paramids  - Marks the end of the paramid numbers (not itself a valid paramid).
 *
 */
typedef enum
{
  example_paramid_base = MPL_PARAM_SET_ID_TO_PARAMID_BASE(EXAMPLE_PARAM_SET_ID),
  EXAMPLE_PARAMETER_IDS
  example_end_of_paramids
} example_paramid_t;
#undef EXAMPLE_PARAMETER_ID_ELEMENT


/**
 * example_my_enum_t
 *
 * Note: If this type is used as enum type in the parameter definition (above), then
 *       the naming must be on this format (starting with 'example_' and 'my_enum' in the
 *       middle. The type name must end with '_t'.
 *
 * @example_my_enum_val1
 * @example_my_enum_val2
 *
 **/
#define EXAMPLE_MY_ENUM       \
  EXAMPLE_MY_ENUM_ELEMENT(val1) \
  EXAMPLE_MY_ENUM_ELEMENT(val2)

#define EXAMPLE_MY_ENUM_ELEMENT(LEVEL) example_my_enum_##LEVEL,
typedef enum
{
  EXAMPLE_MY_ENUM
  example_number_of_my_enum
} example_my_enum_t;
#undef EXAMPLE_MY_ENUM_ELEMENT


  
/**
 * example_init - Initiate libraries
 *
 * @user_p  User pointer
 * @log_fp  Logging function
 *
 * Returns:  0 on success, -1 on failure
 *
 **/
int example_init(void* user_p, mpl_log_fp log_fp);


#ifdef  __cplusplus
}
#endif

#endif /* EXAMPLE_MSG_H */
