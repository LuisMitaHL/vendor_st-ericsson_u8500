/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \brief  Message definitions for STE RC protocol
 */

#ifndef STERC_MSG_H
#define STERC_MSG_H

#include <stdbool.h>
#include <stdint.h>
#include "mpl_list.h"
#include "mpl_param.h"
#include "mpl_msg.h"

#ifdef  __cplusplus
extern "C" {
#endif


#define STERC_PARAM_SET_ID 3
#define STERC_PARAMID_PREFIX "sterc"


/**
 * sterc_event_failure_buf - Pre-packet message buffer for failure event
 */
extern const char sterc_event_failure_buf[];

/**
 * sterc_msgtype_t (MANDATORY)
 *
 * This enum identifies the message types in the protocol
 *
 * Note: Three of these enum values (suffixes req, resp, event) are mandatory and must be set equal to
 *       the library enum values for message types: mpl_msg_type_req, mpl_msg_type_resp, mpl_msg_type_event.
 *
 * @sterc_msgtype_req
 * @sterc_msgtype_resp
 * @sterc_msgtype_event
 *
 **/
#define STERC_MESSAGE_TYPES           \
  STERC_MESSAGE_TYPE_ELEMENT(req)     \
  STERC_MESSAGE_TYPE_ELEMENT(resp)    \
  STERC_MESSAGE_TYPE_ELEMENT(event)

#define STERC_MESSAGE_TYPE_ELEMENT(ELEMENT) sterc_msgtype_##ELEMENT = mpl_msg_type_##ELEMENT,
typedef enum
{
  STERC_MESSAGE_TYPES
  sterc_number_of_msgtype = mpl_number_of_msg_type
} sterc_msgtype_t;
#undef STERC_MESSAGE_TYPE_ELEMENT


/**
 * sterc_msgid_t (MANDATORY)
 *
 * This enum lists the different messages in the protocol. Together with the message type
 * this enum will identify the semantics (meaning) of a particular message.
 *
 * @sterc_start
 * @sterc_stop
 * @sterc_set
 * @sterc_get
 * @sterc_connect_wan
 * @sterc_event_ppp_up
 * @sterc_event_ppp_down
 * @sterc_event_disconnected
 *
 **/
#define STERC_MESSAGE_IDS                  \
  STERC_MESSAGE_ID_ELEMENT(create)         \
  STERC_MESSAGE_ID_ELEMENT(destroy)        \
  STERC_MESSAGE_ID_ELEMENT(set)            \
  STERC_MESSAGE_ID_ELEMENT(get)            \
  STERC_MESSAGE_ID_ELEMENT(connect_wan)    \
  STERC_MESSAGE_ID_ELEMENT(event_ppp_up)   \
  STERC_MESSAGE_ID_ELEMENT(event_ppp_down) \
  STERC_MESSAGE_ID_ELEMENT(event_disconnected)

#define STERC_MESSAGE_ID_ELEMENT(ELEMENT) sterc_##ELEMENT,
typedef enum
{
  STERC_MESSAGE_IDS
  sterc_number_of_msgids
} sterc_msgid_t;
#undef STERC_MESSAGE_ID_ELEMENT

/**
 * sterc_result_t (MANDATORY)
 *
 * This enum is used in messages of type "resp" to indicate the status of a previously sent request (type "req").
 *
 * @sterc_result_ok
 * @sterc_result_failed_parameter_not_found
 *
 **/
#define STERC_RESULT_VALUES                                \
  STERC_RESULT_VALUE_ELEMENT(ok)                           \
  STERC_RESULT_VALUE_ELEMENT(failed_operation_not_allowed) \
  STERC_RESULT_VALUE_ELEMENT(failed_object_exists)         \
  STERC_RESULT_VALUE_ELEMENT(failed_object_not_found)      \
  STERC_RESULT_VALUE_ELEMENT(failed_parameter_not_found)   \
  STERC_RESULT_VALUE_ELEMENT(failed_authentication_error)  \
  STERC_RESULT_VALUE_ELEMENT(failed_unspecified)           \

#define STERC_RESULT_VALUE_ELEMENT(ELEMENT) sterc_result_##ELEMENT,
typedef enum
{
  STERC_RESULT_VALUES
  sterc_number_of_results
} sterc_result_t;
#undef STERC_RESULT_VALUE_ELEMENT


/**
 * sterc_paramid_t (MANDATORY)
 *
 * This enum lists all parameters in the protocol.
 *
 * Note: Three parameters are mandatory: message type, message id and result. The names of these parameters
 *       are optional, but the types are not. They must be the same as the ones defined above.
 *
 * Parameter IDs
 *
 * @sterc_paramid_message_type                                  - Message type (sterc_msgtype_t)  (MANDATORY)
 * @sterc_paramid_message                                       - Message (sterc_msgid_t)         (MANDATORY)
 * @sterc_paramid_result                                        - Message result (sterc_result_t) (MANDATORY)
 * @sterc_paramid_pppd_pid                                      - Process ID of the ppp daemon
 * @sterc_paramid_handle                                        - sterc handle
 * @sterc_paramid_ct                                            - Client tag (uint32)
 * @sterc_paramid_reason                                        - Disconnect reason (sterc_reason_t)
 * @sterc_paramid_lan_own_ip                                    - own ip address on LAN side
 * @sterc_paramid_lan_peer_ip                                   - peer ip address on LAN side
 * @sterc_paramid_lan_dns                                       - lan dns address
 * @sterc_paramid_lan_device                                    - lan device name
 * @sterc_paramid_wan_static_ip                                 - wan static ip address
 * @sterc_paramid_wan_ip                                        - wan ip address
 * @sterc_paramid_wan_dns                                       - wan DNS address(es)
 * @sterc_paramid_wan_device                                    - wan device name
 * @sterc_paramid_tty                                           - tty
 * @sterc_paramid_cid                                           - Caller Id
 * @sterc_paramid_wan_auth_method                               - authentication method
 * @sterc_paramid_wan_auth_uid                                  - username
 * @sterc_paramid_wan_auth_pwd                                  - password
 * @sterc_paramid_wan_auth_chap_id                              - CHAP Id, use pre-calculated CHAP id instead of generate own
 * @sterc_paramid_wan_auth_chap_challenge                       - CHAP Challenge, use pre-calculated challenge instead of generate own
 * @sterc_paramid_wan_auth_chap_response                        - CHAP Response,  use pre-calculated resposne instead of generate own
 * @sterc_paramid_socket_addr                                   - Socket address
 * @sterc_paramid_mode                                          - sterc mode
 * @sterc_paramid_lan_netmask                                   - Net mask of LAN device
 * @sterc_paramid_lan_dhcp_range_first                          - First address of dhcp range on LAN
 * @sterc_paramid_lan_dhcp_range_last                           - Last address of dhcp range on LAN
 * @sterc_paramid_lan_dhcp_lease                                - Lease time of DHCP server
 * @sterc_paramid_debug_mode                                    - Enable debug prints
 *
 **/
#define STERC_PARAMETER_IDS                                             \
  /* Parameter description:  Name                     Type         Enum             MAX,             Set?   Get?   Config? Default Value */ \
  STERC_PARAMETER_ID_ELEMENT(message_type,            enum,        msgtype,         no_max,          false, false, false,  no_default) \
  STERC_PARAMETER_ID_ELEMENT(message,                 enum,        msgid,           no_max,          false, false, false,  no_default) \
  STERC_PARAMETER_ID_ELEMENT(result,                  enum,        result,          no_max,          false, false, false,  no_default) \
  STERC_PARAMETER_ID_ELEMENT(pppd_pid,                int,         dummy,           no_max,          true,  true,  false,  no_default) \
  STERC_PARAMETER_ID_ELEMENT(handle,                  int,         dummy,           no_max,          true,  true,  false,  no_default) \
  STERC_PARAMETER_ID_ELEMENT(ct,                      uint32,      dummy,           max(UINT32_MAX), false, false, false,  no_default) \
  STERC_PARAMETER_ID_ELEMENT(reason,                  enum,        reason,          no_max,          false, false, false,  no_default) \
  STERC_PARAMETER_ID_ELEMENT(lan_own_ip,              string,      dummy,           max(39),         true,  true,  true,   default("10.0.0.1")) \
  STERC_PARAMETER_ID_ELEMENT(lan_peer_ip,             string,      dummy,           max(39),         true,  true,  false,  no_default) \
  STERC_PARAMETER_ID_ELEMENT(lan_dns,                 string,      dummy,           max(39),         true,  true,  false,  no_default) \
  STERC_PARAMETER_ID_ELEMENT(lan_device,              string,      dummy,           max(10),         true,  true,  false,  no_default) \
  STERC_PARAMETER_ID_ELEMENT(wan_static_ip,           string,      dummy,           max(39),         true,  true,  false,  no_default) \
  STERC_PARAMETER_ID_ELEMENT(wan_ip,                  string,      dummy,           max(39),         true,  true,  false,  no_default) \
  STERC_PARAMETER_ID_ELEMENT(wan_dns,                 string,      dummy,           max(39),         true,  true,  false,  no_default) \
  STERC_PARAMETER_ID_ELEMENT(wan_device,              string,      dummy,           max(10),         false, true,  false,  no_default) \
  STERC_PARAMETER_ID_ELEMENT(tty,                     string,      dummy,           max(255),        true,  true,  false,  default("/dev/ttyGS0")) \
  STERC_PARAMETER_ID_ELEMENT(cid,                     int,         dummy,           no_max,          true,  true,  false,  no_default) \
  STERC_PARAMETER_ID_ELEMENT(wan_auth_method,         enum,        wan_auth_method, no_max,          true,  true,  true,   default(sterc_wan_auth_method_none)) \
  STERC_PARAMETER_ID_ELEMENT(wan_auth_uid,            string,      dummy,           max(50),         true,  true,  false,  default("")) \
  STERC_PARAMETER_ID_ELEMENT(wan_auth_pwd,            string,      dummy,           max(50),         true,  true,  false,  default("")) \
  STERC_PARAMETER_ID_ELEMENT(wan_auth_chap_id,        uint8,       dummy,           max(255),        true,  true,  false,  no_default) \
  STERC_PARAMETER_ID_ELEMENT(wan_auth_chap_challenge, uint8_array, dummy,           no_max,          true,  true,  false,  no_default) \
  STERC_PARAMETER_ID_ELEMENT(wan_auth_chap_response,  uint8_array, dummy,           no_max,          true,  true,  false,  no_default) \
  STERC_PARAMETER_ID_ELEMENT(socket_addr,             string,      dummy,           max(255),        true,  true,  false,  no_default) \
  STERC_PARAMETER_ID_ELEMENT(mode,                    enum,        mode,            no_max,          true,  true,  false,  no_default) \
  STERC_PARAMETER_ID_ELEMENT(lan_netmask,             string_tuple,dummy,           no_max,          true,  true,  true,   no_default) \
  STERC_PARAMETER_ID_ELEMENT(lan_dhcp_range_first,    string_tuple,dummy,           no_max,          true,  true,  true,   no_default) \
  STERC_PARAMETER_ID_ELEMENT(lan_dhcp_range_last,     string_tuple,dummy,           no_max,          true,  true,  true,   no_default) \
  STERC_PARAMETER_ID_ELEMENT(lan_dhcp_lease,          string_tuple,dummy,           no_max,          true,  true,  true,   no_default) \
  STERC_PARAMETER_ID_ELEMENT(debug_mode,              bool,        dummy,           no_max,          false, false, true,   default(false)) \

#define STERC_PARAMETER_ID_ELEMENT(ELEMENT, TYPE, EXTRA, MAX, SET, GET, CONFIG, DEFAULT) \
  sterc_paramid_##ELEMENT,
typedef enum
{
  sterc_paramid_base = MPL_PARAM_SET_ID_TO_PARAMID_BASE(STERC_PARAM_SET_ID),
  STERC_PARAMETER_IDS
  sterc_end_of_paramids
} sterc_paramid_t;
#undef STERC_PARAMETER_ID_ELEMENT


#define STERC_CID_PRESENT(msg_p) MPL_MSG_PARAM_PRESENT(msg_p, sterc_paramid_cid)
#define STERC_GET_CID(msg_p) MPL_MSG_GET_PARAM_VALUE(msg_p, int, sterc_paramid_cid)

#define STERC_HANDLE_PRESENT(msg_p) MPL_MSG_PARAM_PRESENT(msg_p, sterc_paramid_handle)
#define STERC_GET_HANDLE(msg_p) MPL_MSG_GET_PARAM_VALUE(msg_p, int, sterc_paramid_handle)

#define STERC_CT_PRESENT(msg_p) MPL_MSG_PARAM_PRESENT(msg_p, sterc_paramid_ct)
#define STERC_GET_CT(msg_p) MPL_MSG_GET_PARAM_VALUE(msg_p, uint32_t, sterc_paramid_ct)

/**
 * sterc_wan_auth_method_t
 * User authentication method
 *
 * @sterc_wan_auth_method_none
 * @sterc_wan_auth_method_pap
 * @sterc_wan_auth_method_chap
 *
 **/
#define STERC_WAN_AUTH_METHOD    \
  STERC_WAN_AUTH_METHOD_ELEMENT(none) \
  STERC_WAN_AUTH_METHOD_ELEMENT(pap)  \
  STERC_WAN_AUTH_METHOD_ELEMENT(chap)

#define STERC_WAN_AUTH_METHOD_ELEMENT(LEVEL) sterc_wan_auth_method_##LEVEL,
typedef enum
{
  STERC_WAN_AUTH_METHOD
  sterc_wan_number_of_auth_method
} sterc_wan_auth_method_t;
#undef STERC_WAN_AUTH_METHOD_ELEMENT

/**
 * sterc_mode_t
 * Usecase
 *
 * @sterc_mode_usb_eth_nap
 * @sterc_mode_psdun
 *
 **/
#define STERC_MODE    \
  STERC_MODE_ELEMENT(usb_eth_nap)  \
  STERC_MODE_ELEMENT(psdun)

#define STERC_MODE_ELEMENT(LEVEL) sterc_mode_##LEVEL,
typedef enum
{
  STERC_MODE
  sterc_number_of_mode
} sterc_mode_t;
#undef STERC_MODE_ELEMENT

/**
 * sterc_reason_t
 * Disconnect reasons
 *
 * @sterc_reason_disconnected_by_dce   - disconnected by DCE
 * @sterc_reason_disconnected_by_nw    - disconnected by PS network
 *
 **/
#define STERC_REASON                          \
  STERC_REASON_ELEMENT(disconnected_by_dce)   \
  STERC_REASON_ELEMENT(disconnected_by_nw)

#define STERC_REASON_ELEMENT(LEVEL) sterc_reason_##LEVEL,
typedef enum
{
  STERC_REASON
  sterc_number_of_reasons
} sterc_reason_t;
#undef STERC_REASON_ELEMENT

/**
 * sterc_msg_init - Initiate library
 *
 * @user_p  User pointer
 * @log_fp  Logging function
 *
 * Returns:  0 on success, -1 on failure
 *
 **/
int sterc_init(void* user_p, mpl_log_fp log_fp);

#ifdef  __cplusplus
}
#endif

#endif /* STERC_MSG_H */
