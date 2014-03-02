/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \brief  PS Connection Control message definitions
 */

#ifndef PSCC_MSG_H
#define PSCC_MSG_H

#include <stdbool.h>
#include <stdint.h>
#include "mpl_list.h"
#include "mpl_param.h"
#include "mpl_msg.h"

#ifdef  __cplusplus
//extern "C" {
#endif


#define PSCC_PARAM_SET_ID 1
#define PSCC_PARAMID_PREFIX "pscc"


/**
 * pscc_result_t
 *
 * @pscc_result_ok
 * @pscc_result_failed_operation_not_allowed
 * @pscc_result_failed_object_exists
 * @pscc_result_failed_object_not_found
 * @pscc_result_failed_parameter_not_found
 * @pscc_result_failed_unspecified
 *
 **/
#define PSCC_RESULT_VALUES                                \
  PSCC_RESULT_VALUE_ELEMENT(ok)                           \
  PSCC_RESULT_VALUE_ELEMENT(failed_operation_not_allowed) \
  PSCC_RESULT_VALUE_ELEMENT(failed_object_exists)         \
  PSCC_RESULT_VALUE_ELEMENT(failed_object_not_found)      \
  PSCC_RESULT_VALUE_ELEMENT(failed_parameter_not_found)   \
  PSCC_RESULT_VALUE_ELEMENT(failed_unspecified)

#define PSCC_RESULT_VALUE_ELEMENT(ELEMENT) pscc_result_##ELEMENT,
typedef enum
{
  PSCC_RESULT_VALUES
  pscc_number_of_results
} pscc_result_t;
#undef PSCC_RESULT_VALUE_ELEMENT


/**
 * pscc_msgtype_t
 *
 * @pscc_msgtype_req
 * @pscc_msgtype_resp
 * @pscc_msgtype_event
 *
 **/
#define PSCC_MESSAGE_TYPES \
  PSCC_MESSAGE_TYPE_ELEMENT(req) \
  PSCC_MESSAGE_TYPE_ELEMENT(resp) \
  PSCC_MESSAGE_TYPE_ELEMENT(event)

#define PSCC_MESSAGE_TYPE_ELEMENT(ELEMENT) pscc_msgtype_##ELEMENT = mpl_msg_type_##ELEMENT,
typedef enum
{
  PSCC_MESSAGE_TYPES
  pscc_number_of_msgtype = mpl_number_of_msg_type
} pscc_msgtype_t;
#undef PSCC_MESSAGE_TYPE_ELEMENT


/**
 * pscc_msgid_t
 *
 * @pscc_create
 * @pscc_destroy
 * @pscc_attach
 * @pscc_attach_status
 * @pscc_detach
 * @pscc_connect
 * @pscc_disconnect
 * @pscc_query_attach_mode
 * @pscc_configure_attach_mode
 * @pscc_query_ping_block_mode
 * @pscc_configure_ping_block_mode
 * @pscc_set
 * @pscc_get
 * @pscc_fetch_data_counters
 * @pscc_reset_data_counters
 * @pscc_list_all_connections
 * @pscc_modify
 * @pscc_generate_uplink_data
 * @pscc_init_handler
 * @pscc_event_attached
 * @pscc_event_attach_failed
 * @pscc_event_detached
 * @pscc_event_detach_failed
 * @pscc_event_attach_mode_queried
 * @pscc_event_attach_mode_configured
 * @pscc_event_attach_mode_configure_failed
 * @pscc_event_connecting
 * @pscc_event_connected
 * @pscc_event_disconnecting
 * @pscc_event_disconnected
 * @pscc_event_data_counters_fetched
 * @pscc_event_data_counters_reset
 * @pscc_event_failure
 * @pscc_event_modified
 * @pscc_event_modify_failed
 * @pscc_event_ping_block_mode_queried
 * @pscc_event_ping_block_mode_configured
 * @pscc_event_ping_block_mode_configure_failed
 *
 **/
#define PSCC_MESSAGE_IDS                                      \
  PSCC_MESSAGE_ID_ELEMENT(create)                             \
  PSCC_MESSAGE_ID_ELEMENT(destroy)                            \
  PSCC_MESSAGE_ID_ELEMENT(attach)                             \
  PSCC_MESSAGE_ID_ELEMENT(attach_status)                      \
  PSCC_MESSAGE_ID_ELEMENT(detach)                             \
  PSCC_MESSAGE_ID_ELEMENT(connect)                            \
  PSCC_MESSAGE_ID_ELEMENT(disconnect)                         \
  PSCC_MESSAGE_ID_ELEMENT(query_attach_mode)                  \
  PSCC_MESSAGE_ID_ELEMENT(configure_attach_mode)              \
  PSCC_MESSAGE_ID_ELEMENT(query_ping_block_mode)              \
  PSCC_MESSAGE_ID_ELEMENT(configure_ping_block_mode)          \
  PSCC_MESSAGE_ID_ELEMENT(set)                                \
  PSCC_MESSAGE_ID_ELEMENT(get)                                \
  PSCC_MESSAGE_ID_ELEMENT(fetch_data_counters)                \
  PSCC_MESSAGE_ID_ELEMENT(reset_data_counters)                \
  PSCC_MESSAGE_ID_ELEMENT(list_all_connections)               \
  PSCC_MESSAGE_ID_ELEMENT(modify)                             \
  PSCC_MESSAGE_ID_ELEMENT(generate_uplink_data)               \
  PSCC_MESSAGE_ID_ELEMENT(init_handler)                       \
  PSCC_MESSAGE_ID_ELEMENT(event_attached)                     \
  PSCC_MESSAGE_ID_ELEMENT(event_attach_failed)                \
  PSCC_MESSAGE_ID_ELEMENT(event_detached)                     \
  PSCC_MESSAGE_ID_ELEMENT(event_detach_failed)                \
  PSCC_MESSAGE_ID_ELEMENT(event_attach_mode_queried)          \
  PSCC_MESSAGE_ID_ELEMENT(event_attach_mode_configured)       \
  PSCC_MESSAGE_ID_ELEMENT(event_attach_mode_configure_failed) \
  PSCC_MESSAGE_ID_ELEMENT(event_connecting)                   \
  PSCC_MESSAGE_ID_ELEMENT(event_connected)                    \
  PSCC_MESSAGE_ID_ELEMENT(event_disconnecting)                \
  PSCC_MESSAGE_ID_ELEMENT(event_disconnected)                 \
  PSCC_MESSAGE_ID_ELEMENT(event_data_counters_fetched)        \
  PSCC_MESSAGE_ID_ELEMENT(event_data_counters_reset)          \
  PSCC_MESSAGE_ID_ELEMENT(event_failure)                      \
  PSCC_MESSAGE_ID_ELEMENT(event_modified)                     \
  PSCC_MESSAGE_ID_ELEMENT(event_modify_failed)                \
  PSCC_MESSAGE_ID_ELEMENT(event_ping_block_mode_queried)          \
  PSCC_MESSAGE_ID_ELEMENT(event_ping_block_mode_configured)       \
  PSCC_MESSAGE_ID_ELEMENT(event_ping_block_mode_configure_failed) \

#define PSCC_MESSAGE_ID_ELEMENT(ELEMENT) pscc_##ELEMENT,
typedef enum
{
  PSCC_MESSAGE_IDS
  pscc_number_of_msgids
} pscc_msgid_t;
#undef PSCC_MESSAGE_ID_ELEMENT

/**
 * pscc_paramid_t
 * Parameter IDs
 *
 * @pscc_paramid_message_type                                  - Message type (pscc_msgtype_t)
 * @pscc_paramid_message                                       - Message (pscc_msgid_t)
 * @pscc_paramid_ct                                            - Client tag (uint32)
 * @pscc_paramid_apn                                           - APN (string)
 * @pscc_paramid_connid                                        - Connection ID (integer)
 * @pscc_paramid_pdp_type                                      - 3GPP PDP context type (pscc_pdp_type_t))
 * @pscc_paramid_static_ip_address                             - static IP address
 * @pscc_paramid_auth_method                                   - User authentication method (pscc_auth_method_t)
 * @pscc_paramid_auth_uid                                      - UID for user authentication (string)
 * @pscc_paramid_auth_pwd                                      - Password for user authentication (string)
 * @pscc_paramid_hcmp                                          - use header compression (bool: true/false)
 * @pscc_paramid_dcmp                                          - use data compression (bool: true/false)
 * @pscc_paramid_aqm                                           - AQM active (bool: true/false)
 * @pscc_paramid_loopback                                      - loopback interface for debug only (bool: true/false)
 * @pscc_paramid_pref_service                                  - Preferred mode of service for PS data (pscc_preferred_service_t)
 * @pscc_paramid_packet_priority                               - packet priority, bitmask
 * @pscc_paramid_qos_type                                      - Quality of Service Type (pscc_qos_type_t)
 * @pscc_paramid_req_qos_precedence_class                      - 3GPP QoS: Requested Precedence Class (pscc_qos_precedence_class_t)
 * @pscc_paramid_req_qos_delay_class                           - 3GPP QoS: Requested Delay Class (pscc_qos_delay_class_t)
 * @pscc_paramid_req_qos_reliability_class                     - 3GPP QoS: Requested Reliability Class (pscc_qos_reliability_class_t)
 * @pscc_paramid_req_qos_peak_throughput_class                 - 3GPP QoS: Requested Peak Throughput Class (pscc_qos_peak_throughput_class_t)
 * @pscc_paramid_req_qos_mean_throughput_class                 - 3GPP QoS: Requested Mean Throughput Class (pscc_qos_mean_throughput_class_t)
 * @pscc_paramid_req_qos_traffic_class                         - 3GPP QoS: Requested Traffic Class (pscc_qos_traffic_class_t)
 * @pscc_paramid_req_qos_delivery_order                        - 3GPP QoS: Requested delivery order (pscc_qos_delivery_order_t)
 * @pscc_paramid_req_qos_delivery_erroneous_sdu                - 3GPP QoS: Requested Delivery erroneous sdu (pscc_qos_delivery_erroneous_sdu_t)
 * @pscc_paramid_req_qos_max_sdu_size                          - 3GPP QoS: Requested Max sdu size
 * @pscc_paramid_req_qos_max_bitrate_uplink                    - 3GPP QoS: Requested max bitrate uplink
 * @pscc_paramid_req_qos_max_bitrate_downlink                  - 3GPP QoS: Requested max bitrate downlink
 * @pscc_paramid_req_qos_residual_ber                          - 3GPP QoS: Requested residual bit error rate (pscc_qos_residual_ber_t)
 * @pscc_paramid_req_qos_sdu_error_ratio                       - 3GPP QoS: Requested sdu error ratio (pscc_qos_sdu_error_ratio_t)
 * @pscc_paramid_req_qos_transfer_delay                        - 3GPP QoS: Requested transfer delay
 * @pscc_paramid_req_qos_traffic_handling_priority             - 3GPP QoS: Requested Traffic handling priority (pscc_qos_traffic_handling_priority_t)
 * @pscc_paramid_req_qos_guaranteed_bitrate_uplink             - 3GPP QoS: Requested guaranteed bitrate uplink
 * @pscc_paramid_req_qos_guaranteed_bitrate_downlink           - 3GPP QoS: Requested guaranteed bitrate downlink
 * @pscc_paramid_req_qos_signalling_indication                 - 3GPP QoS: Requested signalling indication (pscc_qos_signalling_indication_t)
 * @pscc_paramid_req_qos_source_statistics_descriptor          - 3GPP QoS: Requested source statistics descriptor (pscc_qos_source_statistics_descriptor_t)
 * @pscc_paramid_req_qos_extended_max_bitrate_downlink         - 3GPP QoS: Requested Max bitrate downlink (extended)
 * @pscc_paramid_req_qos_extended_max_bitrate_uplink           - 3GPP QoS: Requested Max bitrate uplink (extended)
 * @pscc_paramid_req_qos_extended_guaranteed_bitrate_downlink  - 3GPP QoS: Requested Guaranteed bitrate downlink (extended)
 * @pscc_paramid_req_qos_extended_guaranteed_bitrate_uplink    - 3GPP QoS: Requested Guaranteed bitrate uplink (extended)
 * @pscc_paramid_min_qos_precedence_class                      - 3GPP QoS: Minimum Precedence Class (pscc_qos_precedence_class_t)
 * @pscc_paramid_min_qos_delay_class                           - 3GPP QoS: Minimum Delay Class (pscc_qos_delay_class_t)
 * @pscc_paramid_min_qos_reliability_class                     - 3GPP QoS: Minimum Reliability Class (pscc_qos_reliability_class_t)
 * @pscc_paramid_min_qos_peak_throughput_class                 - 3GPP QoS: Minimum Peak Throughput Class (pscc_qos_peak_throughput_class_t)
 * @pscc_paramid_min_qos_mean_throughput_class                 - 3GPP QoS: Minimum Mean Throughput Class (pscc_qos_mean_throughput_class_t)
 * @pscc_paramid_min_qos_traffic_selector                      - 3GPP QoS: Minimum Traffic Class (pscc_qos_traffic_class_t)
 * @pscc_paramid_min_qos_delivery_order                        - 3GPP QoS: Minimum delivery order (pscc_qos_delivery_order_t)
 * @pscc_paramid_min_qos_delivery_erroneous_sdu                - 3GPP QoS: Minimum Delivery erroneous sdu (pscc_qos_delivery_erroneous_sdu_t)
 * @pscc_paramid_min_qos_max_sdu_size                          - 3GPP QoS: Minimum Max sdu size
 * @pscc_paramid_min_qos_max_bitrate_uplink                    - 3GPP QoS: Minimum max bitrate uplink
 * @pscc_paramid_min_qos_max_bitrate_downlink                  - 3GPP QoS: Minimum max bitrate downlink
 * @pscc_paramid_min_qos_residual_ber                          - 3GPP QoS: Minimum residual bit error rate (pscc_qos_residual_ber_t)
 * @pscc_paramid_min_qos_sdu_error_ratio                       - 3GPP QoS: Minimum sdu error ratio (pscc_qos_sdu_error_ratio_t)
 * @pscc_paramid_min_qos_transfer_delay                        - 3GPP QoS: Minimum transfer delay
 * @pscc_paramid_min_qos_traffic_handling_priority             - 3GPP QoS: Minimum Traffic handling priority (pscc_qos_traffic_handling_priority_t)
 * @pscc_paramid_min_qos_guaranteed_bitrate_uplink             - 3GPP QoS: Minimum guaranteed bitrate uplink
 * @pscc_paramid_min_qos_guaranteed_bitrate_downlink           - 3GPP QoS: Minimum guaranteed bitrate downlink
 * @pscc_paramid_min_qos_signalling_indication                 - 3GPP QoS: Minimum signalling indication (pscc_qos_signalling_indication_t)
 * @pscc_paramid_min_qos_source_statistics_descriptor          - 3GPP QoS: Minimum source statistics descriptor (pscc_qos_source_statistics_descriptor_t)
 * @pscc_paramid_min_qos_extended_max_bitrate_downlink         - 3GPP QoS: Minimum Max bitrate downlink (extended)
 * @pscc_paramid_min_qos_extended_max_bitrate_uplink           - 3GPP QoS: Minimum Max bitrate uplink (extended)
 * @pscc_paramid_min_qos_extended_guaranteed_bitrate_downlink  - 3GPP QoS: Minimum Guaranteed bitrate downlink (extended)
 * @pscc_paramid_min_qos_extended_guaranteed_bitrate_uplink    - 3GPP QoS: Minimum Guaranteed bitrate uplink (extended)
 * @pscc_paramid_neg_qos_traffic_class                         - 3GPP QoS: Negotiated Traffic Class (pscc_qos_traffic_class_t)
 * @pscc_paramid_neg_qos_delivery_order                        - 3GPP QoS: Negotiated delivery order (pscc_qos_delivery_order_t)
 * @pscc_paramid_neg_qos_delivery_erroneous_sdu                - 3GPP QoS: Negotiated Delivery erroneous sdu (pscc_qos_delivery_erroneous_sdu_t)
 * @pscc_paramid_neg_qos_max_sdu_size                          - 3GPP QoS: Negotiated Max sdu size
 * @pscc_paramid_neg_qos_max_bitrate_uplink                    - 3GPP QoS: Negotiated max bitrate uplink
 * @pscc_paramid_neg_qos_max_bitrate_downlink                  - 3GPP QoS: Negotiated max bitrate downlink
 * @pscc_paramid_neg_qos_residual_ber                          - 3GPP QoS: Negotiated residual bit error rate (pscc_qos_residual_ber_t)
 * @pscc_paramid_neg_qos_sdu_error_ratio                       - 3GPP QoS: Negotiated sdu error ratio (pscc_qos_sdu_error_ratio_t)
 * @pscc_paramid_neg_qos_transfer_delay                        - 3GPP QoS: Negotiated transfer delay
 * @pscc_paramid_neg_qos_traffic_handling_priority             - 3GPP QoS: Negotiated Traffic handling priority (pscc_qos_traffic_handling_priority_t)
 * @pscc_paramid_neg_qos_guaranteed_bitrate_uplink             - 3GPP QoS: Negotiated guaranteed bitrate uplink
 * @pscc_paramid_neg_qos_guaranteed_bitrate_downlink           - 3GPP QoS: Negotiated guaranteed bitrate downlink
 * @pscc_paramid_neg_qos_signalling_indication                 - 3GPP QoS: Negotiated signalling indication (pscc_qos_signalling_indication_t)
 * @pscc_paramid_neg_qos_source_statistics_descriptor          - 3GPP QoS: Negotiated source statistics descriptor (pscc_qos_source_statistics_descriptor_t)
 * @pscc_paramid_neg_qos_extended_max_bitrate_downlink         - 3GPP QoS: Negotiated Max bitrate downlink (extended)
 * @pscc_paramid_neg_qos_extended_max_bitrate_uplink           - 3GPP QoS: Negotiated Max bitrate uplink (extended)
 * @pscc_paramid_neg_qos_extended_guaranteed_bitrate_downlink  - 3GPP QoS: Negotiated Guaranteed bitrate downlink (extended)
 * @pscc_paramid_neg_qos_extended_guaranteed_bitrate_uplink    - 3GPP QoS: Negotiated Guaranteed bitrate uplink (extended)
 * @pscc_paramid_result                                        - Message result (pscc_result_t)
 * @pscc_paramid_cause                                         - Cause value, defined in 3GPP 24008 (integer)
 * @pscc_paramid_reason                                        - disconnect reasons (pscc_reason_t)
 * @pscc_paramid_ip_map                                        - IP Map to tell which addresses are valid
 * @pscc_paramid_own_ip_address                                - Own IP address (string)
 * @pscc_paramid_own_ipv6_address                              - Own IPv6 address (string)
 * @pscc_paramid_dns_address                                   - Primary DNS address (string)
 * @pscc_paramid_ipv6_dns_address                              - Primary IPv6 DNS address (string)
 * @pscc_paramid_secondary_dns_address                         - Secondary DNS address (string)
 * @pscc_paramid_ipv6_secondary_dns_address                    - Secondary IPv6 DNS address (string)
 * @pscc_paramid_gw_address                                    - GW address (string)
 * @pscc_paramid_attach_mode                                   - ps attach mode (pscc_attach_mode_t)
 * @pscc_paramid_attach_status                                 - ps attach status (pscc_attach_status_t)
 * @pscc_paramid_connection_status                             - connection status (pscc_connection_status_t)
 * @pscc_paramid_nsapi                                         - Network service access point identifier (int)
 * @pscc_paramid_netdev_name                                   - Net device name (string) Read only
 * @pscc_paramid_netdev_name_prefix                            - Net device prefix name (string)
 * @pscc_paramid_auth_chap_id                                  - CHAP Id, use pre-calculated CHAP id instead of generate own
 * @pscc_paramid_auth_chap_challenge                           - CHAP Challenge, use pre-calculated challenge instead of generate own
 * @pscc_paramid_auth_chap_response                            - CHAP Response,  use pre-calculated resposne instead of generate own
 * @pscc_paramid_uplink_data_size                              - Size of data to be generated on the uplink
 * @pscc_paramid_rx_data_count_lo                              - Number of bytes received (first 32 bits)
 * @pscc_paramid_rx_data_count_hi                              - Number of bytes received (last 32 bits)
 * @pscc_paramid_tx_data_count_lo                              - Number of bytes sent (first 32 bits)
 * @pscc_paramid_tx_data_count_hi                              - Number of bytes sent (last 32 bits)
 * @pscc_paramid_nwif_txqueuelen                               - Length of the txqueue in the network interface
 * @pscc_paramid_debug_mode                                    - Enable debug prints
 * @pscc_paramid_gprs_res_ctrl                                 - Enable/Disable GPRS Resource Control
 * @pscc_paramid_nwif_mtu                                      - The mtu in the network interface
 * @pscc_paramid_aol_mode                                      - If always-online mode should be set
 * @pscc_paramid_ping_block_mode                               - If incoming (mobile terminated) ICMP echo requests (ping) should be discarded by the modem
 *
 **/
#define PSCC_PARAMETER_IDS                                           \
  /* Parameter description: Name                                         Type         Enum                             MAX,                              Set?   Get?   Config? Default Value */ \
  PSCC_PARAMETER_ID_ELEMENT(message_type,                                enum,        msgtype,                         no_max,                           false, false, false,  no_default) \
  PSCC_PARAMETER_ID_ELEMENT(message,                                     enum,        msgid,                           no_max,                           false, false, false,  no_default) \
  PSCC_PARAMETER_ID_ELEMENT(ct,                                        uint32,      dummy,                           max(UINT32_MAX),                           false, false, false,  no_default) \
  PSCC_PARAMETER_ID_ELEMENT(apn,                                         string,      dummy,                           max(99),                          true,  true,  true,   default("")) \
  PSCC_PARAMETER_ID_ELEMENT(connid,                                      int,         dummy,                           no_max,                           false, false, false,  no_default) \
  PSCC_PARAMETER_ID_ELEMENT(pdp_type,                                    enum,        pdp_type,                        no_max,                           true,  true,  true,   default(pscc_pdp_type_ipv4)) \
  PSCC_PARAMETER_ID_ELEMENT(static_ip_address,                           string,      dummy,                           max(39),                          true,  true,  false,  no_default) \
  PSCC_PARAMETER_ID_ELEMENT(auth_method,                                 enum,        auth_method,                     no_max,                           true,  true,  true,   default(pscc_auth_method_none)) \
  PSCC_PARAMETER_ID_ELEMENT(auth_uid,                                    string,      dummy,                           max(50),                          true,  true,  false,  default("")) \
  PSCC_PARAMETER_ID_ELEMENT(auth_pwd,                                    string,      dummy,                           max(50),                          true,  true,  false,  default("")) \
  PSCC_PARAMETER_ID_ELEMENT(hcmp,                                        bool,        dummy,                           no_max,                           true,  true,  true,   default(false)) \
  PSCC_PARAMETER_ID_ELEMENT(dcmp,                                        bool,        dummy,                           no_max,                           true,  true,  true,   default(false)) \
  PSCC_PARAMETER_ID_ELEMENT(aqm,                                         bool,        dummy,                           no_max,                           true,  true,  true,   default(false)) \
  PSCC_PARAMETER_ID_ELEMENT(loopback,                                    bool,        dummy,                           no_max,                           true,  true,  true,   default(false)) \
  PSCC_PARAMETER_ID_ELEMENT(pref_service,                                enum,        preferred_service,               no_max,                           true,  true,  true,   default(pscc_preferred_service_automatic)) \
  PSCC_PARAMETER_ID_ELEMENT(packet_priority,                             uint32,      dummy,                           max(PSCC_PACKET_PRIORITY_ALL),    true,  true,  true,   default(0)) \
  PSCC_PARAMETER_ID_ELEMENT(qos_type,                                    enum,        qos_type,                        no_max,                           true,  true,  true,   default(pscc_qos_type_rel99))\
  PSCC_PARAMETER_ID_ELEMENT(req_qos_precedence_class,                    enum,        qos_precedence_class,            no_max,                           true,  true,  true,   default(0))\
  PSCC_PARAMETER_ID_ELEMENT(req_qos_delay_class,                         enum,        qos_delay_class,                 no_max,                           true,  true,  true,   default(0))\
  PSCC_PARAMETER_ID_ELEMENT(req_qos_reliability_class,                   enum,        qos_reliability_class,           no_max,                           true,  true,  true,   default(0))\
  PSCC_PARAMETER_ID_ELEMENT(req_qos_peak_throughput_class,               enum,        qos_peak_throughput_class,       no_max,                           true,  true,  true,   default(0))\
  PSCC_PARAMETER_ID_ELEMENT(req_qos_mean_throughput_class,               enum,        qos_mean_throughput_class,       no_max,                           true,  true,  true,   default(0))\
  PSCC_PARAMETER_ID_ELEMENT(req_qos_traffic_class,                       enum,        qos_traffic_class,               no_max,                           true,  true,  true,   default(0))\
  PSCC_PARAMETER_ID_ELEMENT(req_qos_delivery_order,                      enum,        qos_delivery_order,              no_max,                           true,  true,  true,   default(0))\
  PSCC_PARAMETER_ID_ELEMENT(req_qos_delivery_erroneous_sdu,              enum,        qos_delivery_erroneous_sdu,      no_max,                           true,  true,  true,   default(0))\
  PSCC_PARAMETER_ID_ELEMENT(req_qos_max_sdu_size,                        uint8,       dummy,                           no_max,                           true,  true,  true,   default(0))\
  PSCC_PARAMETER_ID_ELEMENT(req_qos_max_bitrate_uplink,                  uint8,       dummy,                           no_max,                           true,  true,  true,   default(0))\
  PSCC_PARAMETER_ID_ELEMENT(req_qos_max_bitrate_downlink,                uint8,       dummy,                           no_max,                           true,  true,  true,   default(0))\
  PSCC_PARAMETER_ID_ELEMENT(req_qos_residual_ber,                        enum,        qos_residual_ber,                no_max,                           true,  true,  true,   default(0))\
  PSCC_PARAMETER_ID_ELEMENT(req_qos_sdu_error_ratio,                     enum,        qos_sdu_error_ratio,             no_max,                           true,  true,  true,   default(0))\
  PSCC_PARAMETER_ID_ELEMENT(req_qos_transfer_delay,                      uint8,       dummy,                           max(63),                          true,  true,  true,   default(0)) \
  PSCC_PARAMETER_ID_ELEMENT(req_qos_traffic_handling_priority,           enum,        qos_traffic_handling_priority,   no_max,                           true,  true,  true,   default(0))\
  PSCC_PARAMETER_ID_ELEMENT(req_qos_guaranteed_bitrate_uplink,           uint8,       dummy,                           no_max,                           true,  true,  true,   default(0))\
  PSCC_PARAMETER_ID_ELEMENT(req_qos_guaranteed_bitrate_downlink,         uint8,       dummy,                           no_max,                           true,  true,  true,   default(0))\
  PSCC_PARAMETER_ID_ELEMENT(req_qos_signalling_indication,               enum,        qos_signalling_indication,       no_max,                           true,  true,  true,   default(0))\
  PSCC_PARAMETER_ID_ELEMENT(req_qos_source_statistics_descriptor,        enum,        qos_source_statistics_descriptor,no_max,                           true,  true,  true,   default(0))\
  PSCC_PARAMETER_ID_ELEMENT(req_qos_extended_max_bitrate_downlink,       uint8,       dummy,                           no_max,                           true,  true,  true,   default(0))\
  PSCC_PARAMETER_ID_ELEMENT(req_qos_extended_max_bitrate_uplink,         uint8,       dummy,                           no_max,                           true,  true,  true,   default(0))\
  PSCC_PARAMETER_ID_ELEMENT(req_qos_extended_guaranteed_bitrate_downlink,uint8,       dummy,                           no_max,                           true,  true,  true,   default(0))\
  PSCC_PARAMETER_ID_ELEMENT(req_qos_extended_guaranteed_bitrate_uplink,  uint8,       dummy,                           no_max,                           true,  true,  true,   default(0))\
  PSCC_PARAMETER_ID_ELEMENT(min_qos_precedence_class,                    enum,        qos_precedence_class,            no_max,                           true,  true,  true,   default(0))\
  PSCC_PARAMETER_ID_ELEMENT(min_qos_delay_class,                         enum,        qos_delay_class,                 no_max,                           true,  true,  true,   default(0))\
  PSCC_PARAMETER_ID_ELEMENT(min_qos_reliability_class,                   enum,        qos_reliability_class,           no_max,                           true,  true,  true,   default(0))\
  PSCC_PARAMETER_ID_ELEMENT(min_qos_peak_throughput_class,               enum,        qos_peak_throughput_class,       no_max,                           true,  true,  true,   default(0))\
  PSCC_PARAMETER_ID_ELEMENT(min_qos_mean_throughput_class,               enum,        qos_mean_throughput_class,       no_max,                           true,  true,  true,   default(0))\
  PSCC_PARAMETER_ID_ELEMENT(min_qos_traffic_class,                       enum,        qos_traffic_class,               no_max,                           true,  true,  true,   default(0))\
  PSCC_PARAMETER_ID_ELEMENT(min_qos_delivery_order,                      enum,        qos_delivery_order,              no_max,                           true,  true,  true,   default(0))\
  PSCC_PARAMETER_ID_ELEMENT(min_qos_delivery_erroneous_sdu,              enum,        qos_delivery_erroneous_sdu,      no_max,                           true,  true,  true,   default(0))\
  PSCC_PARAMETER_ID_ELEMENT(min_qos_max_sdu_size,                        uint8,       dummy,                           no_max,                           true,  true,  true,   default(0))\
  PSCC_PARAMETER_ID_ELEMENT(min_qos_max_bitrate_uplink,                  uint8,       dummy,                           no_max,                           true,  true,  true,   default(0))\
  PSCC_PARAMETER_ID_ELEMENT(min_qos_max_bitrate_downlink,                uint8,       dummy,                           no_max,                           true,  true,  true,   default(0))\
  PSCC_PARAMETER_ID_ELEMENT(min_qos_residual_ber,                        enum,        qos_residual_ber,                no_max,                           true,  true,  true,   default(0))\
  PSCC_PARAMETER_ID_ELEMENT(min_qos_sdu_error_ratio,                     enum,        qos_sdu_error_ratio,             no_max,                           true,  true,  true,   default(0))\
  PSCC_PARAMETER_ID_ELEMENT(min_qos_transfer_delay,                      uint8,       dummy,                           max(63),                          true,  true,  true,   default(0))\
  PSCC_PARAMETER_ID_ELEMENT(min_qos_traffic_handling_priority,           enum,        qos_traffic_handling_priority,   no_max,                           true,  true,  true,   default(0))\
  PSCC_PARAMETER_ID_ELEMENT(min_qos_guaranteed_bitrate_uplink,           uint8,       dummy,                           no_max,                           true,  true,  true,   default(0))\
  PSCC_PARAMETER_ID_ELEMENT(min_qos_guaranteed_bitrate_downlink,         uint8,       dummy,                           no_max,                           true,  true,  true,   default(0))\
  PSCC_PARAMETER_ID_ELEMENT(min_qos_signalling_indication,               enum,        qos_signalling_indication,       no_max,                           true,  true,  true,   default(0))\
  PSCC_PARAMETER_ID_ELEMENT(min_qos_source_statistics_descriptor,        enum,        qos_source_statistics_descriptor,no_max,                           true,  true,  true,   default(0))\
  PSCC_PARAMETER_ID_ELEMENT(min_qos_extended_max_bitrate_downlink,       uint8,       dummy,                           no_max,                           true,  true,  true,   default(0))\
  PSCC_PARAMETER_ID_ELEMENT(min_qos_extended_max_bitrate_uplink,         uint8,       dummy,                           no_max,                           true,  true,  true,   default(0))\
  PSCC_PARAMETER_ID_ELEMENT(min_qos_extended_guaranteed_bitrate_downlink,uint8,       dummy,                           no_max,                           true,  true,  true,   default(0))\
  PSCC_PARAMETER_ID_ELEMENT(min_qos_extended_guaranteed_bitrate_uplink,  uint8,       dummy,                           no_max,                           true,  true,  true,   default(0))\
  PSCC_PARAMETER_ID_ELEMENT(neg_qos_traffic_class,                       enum,        qos_traffic_class,               no_max,                           false, true,  false,  no_default)\
  PSCC_PARAMETER_ID_ELEMENT(neg_qos_delivery_order,                      enum,        qos_delivery_order,              no_max,                           false, true,  false,  no_default)\
  PSCC_PARAMETER_ID_ELEMENT(neg_qos_delivery_erroneous_sdu,              enum,        qos_delivery_erroneous_sdu,      no_max,                           false, true,  false,  no_default)\
  PSCC_PARAMETER_ID_ELEMENT(neg_qos_max_sdu_size,                        uint8,       dummy,                           no_max,                           false, true,  false,  no_default)\
  PSCC_PARAMETER_ID_ELEMENT(neg_qos_max_bitrate_uplink,                  uint8,       dummy,                           no_max,                           false, true,  false,  no_default)\
  PSCC_PARAMETER_ID_ELEMENT(neg_qos_max_bitrate_downlink,                uint8,       dummy,                           no_max,                           false, true,  false,  no_default)\
  PSCC_PARAMETER_ID_ELEMENT(neg_qos_residual_ber,                        enum,        qos_residual_ber,                no_max,                           false, true,  false,  no_default)\
  PSCC_PARAMETER_ID_ELEMENT(neg_qos_sdu_error_ratio,                     enum,        qos_sdu_error_ratio,             no_max,                           false, true,  false,  no_default)\
  PSCC_PARAMETER_ID_ELEMENT(neg_qos_transfer_delay,                      uint8,       dummy,                           max(63),                          false, true,  false,  no_default)\
  PSCC_PARAMETER_ID_ELEMENT(neg_qos_traffic_handling_priority,           enum,        qos_traffic_handling_priority,   no_max,                           false, true,  false,  no_default)\
  PSCC_PARAMETER_ID_ELEMENT(neg_qos_guaranteed_bitrate_uplink,           uint8,       dummy,                           no_max,                           false, true,  false,  no_default)\
  PSCC_PARAMETER_ID_ELEMENT(neg_qos_guaranteed_bitrate_downlink,         uint8,       dummy,                           no_max,                           false, true,  false,  no_default)\
  PSCC_PARAMETER_ID_ELEMENT(neg_qos_signalling_indication,               enum,        qos_signalling_indication,       no_max,                           false, true,  false,  no_default)\
  PSCC_PARAMETER_ID_ELEMENT(neg_qos_source_statistics_descriptor,        enum,        qos_source_statistics_descriptor,no_max,                           false, true,  false,  no_default)\
  PSCC_PARAMETER_ID_ELEMENT(neg_qos_extended_max_bitrate_downlink,       uint8,       dummy,                           no_max,                           false, true,  false,  no_default)\
  PSCC_PARAMETER_ID_ELEMENT(neg_qos_extended_max_bitrate_uplink,         uint8,       dummy,                           no_max,                           false, true,  false,  no_default)\
  PSCC_PARAMETER_ID_ELEMENT(neg_qos_extended_guaranteed_bitrate_downlink,uint8,       dummy,                           no_max,                           false, true,  false,  no_default)\
  PSCC_PARAMETER_ID_ELEMENT(neg_qos_extended_guaranteed_bitrate_uplink,  uint8,       dummy,                           no_max,                           false, true,  false,  no_default)\
  PSCC_PARAMETER_ID_ELEMENT(result,                                      enum,        result,                          no_max,                           false, false, false,  no_default)\
  PSCC_PARAMETER_ID_ELEMENT(cause,                                       int,         dummy,                           max(INT_MAX),                     false, false, false,  no_default)\
  PSCC_PARAMETER_ID_ELEMENT(reason,                                      enum,        reason,                          no_max,                           false, false, false,  no_default)\
  PSCC_PARAMETER_ID_ELEMENT(ip_map,                                      int,         dummy,                           no_max,                           true,  true,  false,  no_default)\
  PSCC_PARAMETER_ID_ELEMENT(own_ip_address,                              string,      dummy,                           max(39),                          false, true,  false,  no_default)\
  PSCC_PARAMETER_ID_ELEMENT(own_ipv6_address,                            string,      dummy,                           max(39),                          false, true,  false,  no_default)\
  PSCC_PARAMETER_ID_ELEMENT(dns_address,                                 string,      dummy,                           max(39),                          false, true,  false,  no_default)\
  PSCC_PARAMETER_ID_ELEMENT(ipv6_dns_address,                            string,      dummy,                           max(39),                          false, true,  false,  no_default)\
  PSCC_PARAMETER_ID_ELEMENT(secondary_dns_address,                       string,      dummy,                           max(39),                          false, true,  false,  no_default)\
  PSCC_PARAMETER_ID_ELEMENT(ipv6_secondary_dns_address,                  string,      dummy,                           max(39),                          false, true,  false,  no_default)\
  PSCC_PARAMETER_ID_ELEMENT(gw_address,                                  string,      dummy,                           max(39),                          false, true,  false,  no_default)\
  PSCC_PARAMETER_ID_ELEMENT(attach_mode,                                 enum,        attach_mode,                     no_max,                           false, false, false,  no_default)\
  PSCC_PARAMETER_ID_ELEMENT(attach_status,                               enum,        attach_status,                   no_max,                           false, false, false,  no_default)\
  PSCC_PARAMETER_ID_ELEMENT(connection_status,                           enum,        connection_status,               no_max,                           false, true,  false,  no_default)\
  PSCC_PARAMETER_ID_ELEMENT(nsapi,                                       int,         dummy,                           no_max,                           true,  true,  false,  no_default)\
  PSCC_PARAMETER_ID_ELEMENT(netdev_name,                                 string,      dummy,                           max(10),                          false, true,  false,  no_default)\
  PSCC_PARAMETER_ID_ELEMENT(netdev_name_prefix,                          string,      dummy,                           max(10),                          true,  true,  true,   default("rmnet"))\
  PSCC_PARAMETER_ID_ELEMENT(auth_chap_id,                                uint8,       dummy,                           max(255),                         true,  true,  false,  no_default) \
  PSCC_PARAMETER_ID_ELEMENT(auth_chap_challenge,                         uint8_array, dummy,                           no_max,                           true,  true,  false,  no_default)\
  PSCC_PARAMETER_ID_ELEMENT(auth_chap_response,                          uint8_array, dummy,                           no_max,                           true,  true,  false,  no_default)\
  PSCC_PARAMETER_ID_ELEMENT(uplink_data_size,                            uint32,      dummy,                           max(UINT32_MAX),                  true,  true,  false,  default(0))\
  PSCC_PARAMETER_ID_ELEMENT(rx_data_count_lo,                            uint32,      dummy,                           max(UINT32_MAX),                  false, false, false,  default(0))\
  PSCC_PARAMETER_ID_ELEMENT(rx_data_count_hi,                            uint32,      dummy,                           max(UINT32_MAX),                  false, false, false,  default(0))\
  PSCC_PARAMETER_ID_ELEMENT(tx_data_count_lo,                            uint32,      dummy,                           max(UINT32_MAX),                  false, false, false,  default(0))\
  PSCC_PARAMETER_ID_ELEMENT(tx_data_count_hi,                            uint32,      dummy,                           max(UINT32_MAX),                  false, false, false,  default(0)) \
  PSCC_PARAMETER_ID_ELEMENT(nwif_txqueuelen,                             uint32,      dummy,                           max(UINT32_MAX),                  true,  true,  true,   default(150)) \
  PSCC_PARAMETER_ID_ELEMENT(debug_mode,                                  bool,        dummy,                           no_max,                           false, false, true,   default(false)) \
  PSCC_PARAMETER_ID_ELEMENT(gprs_res_ctrl,                               bool,        dummy,                           no_max,                           false, false, true,   default(false)) \
  PSCC_PARAMETER_ID_ELEMENT(nwif_mtu,                                    uint32,      dummy,                           max(UINT32_MAX),                  true,  true,  true,   default(1500)) \
  PSCC_PARAMETER_ID_ELEMENT(aol_mode,                                    bool,        dummy,                           no_max,                           false, false, true,   default(false)) \
  PSCC_PARAMETER_ID_ELEMENT(ping_block_mode,                             enum,        ping_block_mode,                 no_max,                           false, false, false,  no_default)

#define PSCC_PARAMETER_ID_ELEMENT(ELEMENT, TYPE, EXTRA, MAX, SET, GET, CONFIG, DEFAULT) \
  pscc_paramid_##ELEMENT,
typedef enum
{
  pscc_paramid_base = MPL_PARAM_SET_ID_TO_PARAMID_BASE(PSCC_PARAM_SET_ID),
  PSCC_PARAMETER_IDS
  pscc_end_of_paramids
} pscc_paramid_t;
#undef PSCC_PARAMETER_ID_ELEMENT

/**
 * pscc_pdp_type_t
 * 3GPP PDP context type
 *
 * @pscc_pdp_type_ipv4
 * @pscc_pdp_type_ipv6
 * @pscc_pdp_type_ipv4v6
 * @pscc_pdp_type_test
 *
 **/
#define PSCC_PDP_TYPE       \
  PSCC_PDP_TYPE_ELEMENT(ipv4) \
  PSCC_PDP_TYPE_ELEMENT(ipv6) \
  PSCC_PDP_TYPE_ELEMENT(ipv4v6) \
  PSCC_PDP_TYPE_ELEMENT(test)

#define PSCC_PDP_TYPE_ELEMENT(LEVEL) pscc_pdp_type_##LEVEL,
typedef enum
{
  PSCC_PDP_TYPE
  pscc_number_of_pdp_type
} pscc_pdp_type_t;
#undef PSCC_PDP_TYPE_ELEMENT

/**
 * pscc_auth_method_t
 * User authentication method
 *
 * @pscc_auth_method_none
 * @pscc_auth_method_pap
 * @pscc_auth_method_chap
 *
 **/
#define PSCC_AUTH_METHOD    \
  PSCC_AUTH_METHOD_ELEMENT(none) \
  PSCC_AUTH_METHOD_ELEMENT(pap)  \
  PSCC_AUTH_METHOD_ELEMENT(chap)

#define PSCC_AUTH_METHOD_ELEMENT(LEVEL) pscc_auth_method_##LEVEL,
typedef enum
{
  PSCC_AUTH_METHOD
  pscc_number_of_auth_method
} pscc_auth_method_t;
#undef PSCC_AUTH_METHOD_ELEMENT

/**
 * pscc_preferred_service_t
 * Preferred mode of service for PS data
 *
 * @pscc_preferred_service_ps_only   - Packet Switched only
 * @pscc_preferred_service_automatic - Allow incoming CS (voice/data)
 *                                        calls during a PS call.
 *
 **/
#define PSCC_PREFERRED_SERVICE    \
  PSCC_PREFERRED_SERVICE_ELEMENT(ps_only) \
  PSCC_PREFERRED_SERVICE_ELEMENT(automatic)

#define PSCC_PREFERRED_SERVICE_ELEMENT(LEVEL) pscc_preferred_service_##LEVEL,
typedef enum
{
  PSCC_PREFERRED_SERVICE
  pscc_number_of_preferred_service
} pscc_preferred_service_t;
#undef PSCC_PREFERRED_SERVICE_ELEMENT

/**
 * pscc_qos_type_t
 * 3GPP QoS Type.
 * Used to specify QoS parameter type for PDP context
 *
 * @pscc_qos_type_rel97
 * @pscc_qos_type_rel99
 *
 **/
#define PSCC_QOS_TYPE          \
  PSCC_QOS_TYPE_ELEMENT(rel97) \
  PSCC_QOS_TYPE_ELEMENT(rel99)

#define PSCC_QOS_TYPE_ELEMENT(LEVEL) pscc_qos_type_##LEVEL,
typedef enum
{
  PSCC_QOS_TYPE
  pscc_number_of_qos_type
} pscc_qos_type_t;
#undef PSCC_QOS_TYPE_ELEMENT

/**
 * pscc_qos_precedence_class_t
 * 3GPP QoS Precedence class
 *
 * @pscc_qos_PRECEDENCE_CLASS_UNSPECIFIED
 * @pscc_qos_PRECEDENCE_CLASS_HIGH
 * @pscc_qos_PRECEDENCE_CLASS_NORMAL
 * @pscc_qos_PRECEDENCE_CLASS_LOW
 *
 **/
#define PSCC_QOS_PRECEDENCE_CLASS          \
  PSCC_QOS_PRECEDENCE_CLASS_ELEMENT(UNSPECIFIED) \
  PSCC_QOS_PRECEDENCE_CLASS_ELEMENT(HIGH) \
  PSCC_QOS_PRECEDENCE_CLASS_ELEMENT(NORMAL) \
  PSCC_QOS_PRECEDENCE_CLASS_ELEMENT(LOW)

#define PSCC_QOS_PRECEDENCE_CLASS_ELEMENT(LEVEL) pscc_qos_PRECEDENCE_CLASS_##LEVEL,
typedef enum
{
  PSCC_QOS_PRECEDENCE_CLASS
  pscc_number_of_qos_PRECEDENCE_CLASS
} pscc_qos_precedence_class_t;
#undef PSCC_QOS_PRECEDENCE_CLASS_ELEMENT


/**
 * pscc_qos_delay_class_t
 * 3GPP QoS Delay class
 *
 * @pscc_qos_DELAY_CLASS_UNSPECIFIED
 * @pscc_qos_DELAY_CLASS_LEVEL_1
 * @pscc_qos_DELAY_CLASS_LEVEL_2
 * @pscc_qos_DELAY_CLASS_LEVEL_3
 * @pscc_qos_DELAY_CLASS_LEVEL_4
 *
 **/
#define PSCC_QOS_DELAY_CLASS          \
  PSCC_QOS_DELAY_CLASS_ELEMENT(UNSPECIFIED) \
  PSCC_QOS_DELAY_CLASS_ELEMENT(LEVEL_1) \
  PSCC_QOS_DELAY_CLASS_ELEMENT(LEVEL_2) \
  PSCC_QOS_DELAY_CLASS_ELEMENT(LEVEL_3) \
  PSCC_QOS_DELAY_CLASS_ELEMENT(LEVEL_4)

#define PSCC_QOS_DELAY_CLASS_ELEMENT(LEVEL) pscc_qos_DELAY_CLASS_##LEVEL,
typedef enum
{
  PSCC_QOS_DELAY_CLASS
  pscc_number_of_qos_DELAY_CLASS
} pscc_qos_delay_class_t;
#undef PSCC_QOS_DELAY_CLASS_ELEMENT

/**
 * pscc_qos_reliability_class_t
 * 3GPP QoS Reliability class
 *
 * @pscc_qos_RELIABILITY_CLASS_UNSPECIFIED
 * @pscc_qos_RELIABILITY_CLASS_LEVEL_1
 * @pscc_qos_RELIABILITY_CLASS_LEVEL_2
 * @pscc_qos_RELIABILITY_CLASS_LEVEL_3
 * @pscc_qos_RELIABILITY_CLASS_LEVEL_4
 * @pscc_qos_RELIABILITY_CLASS_LEVEL_5
 *
 **/
#define PSCC_QOS_RELIABILITY_CLASS          \
  PSCC_QOS_RELIABILITY_CLASS_ELEMENT(UNSPECIFIED) \
  PSCC_QOS_RELIABILITY_CLASS_ELEMENT(LEVEL_1) \
  PSCC_QOS_RELIABILITY_CLASS_ELEMENT(LEVEL_2) \
  PSCC_QOS_RELIABILITY_CLASS_ELEMENT(LEVEL_3) \
  PSCC_QOS_RELIABILITY_CLASS_ELEMENT(LEVEL_4) \
  PSCC_QOS_RELIABILITY_CLASS_ELEMENT(LEVEL_5)

#define PSCC_QOS_RELIABILITY_CLASS_ELEMENT(LEVEL) pscc_qos_RELIABILITY_CLASS_##LEVEL,
typedef enum
{
  PSCC_QOS_RELIABILITY_CLASS
  pscc_number_of_qos_RELIABILITY_CLASS
} pscc_qos_reliability_class_t;
#undef PSCC_QOS_RELIABILITY_CLASS_ELEMENT

/**
 * pscc_qos_peak_throughput_class_t
 * 3GPP QoS Peak Throughput class
 *
 * @pscc_qos_PEAK_THROUGHPUT_CLASS_UNSPECIFIED
 * @pscc_qos_PEAK_THROUGHPUT_CLASS_LEVEL_1      8 kbps
 * @pscc_qos_PEAK_THROUGHPUT_CLASS_LEVEL_2     16 kpbs
 * @pscc_qos_PEAK_THROUGHPUT_CLASS_LEVEL_4     32 kpbs
 * @pscc_qos_PEAK_THROUGHPUT_CLASS_LEVEL_8     64 kpbs
 * @pscc_qos_PEAK_THROUGHPUT_CLASS_LEVEL_16   128 kpbs
 * @pscc_qos_PEAK_THROUGHPUT_CLASS_LEVEL_32   256 kpbs
 * @pscc_qos_PEAK_THROUGHPUT_CLASS_LEVEL_64   512 kpbs
 * @pscc_qos_PEAK_THROUGHPUT_CLASS_LEVEL_128 1024 kpbs
 * @pscc_qos_PEAK_THROUGHPUT_CLASS_LEVEL_256 2048 kpbs
 *
 **/
#define PSCC_QOS_PEAK_THROUGHPUT_CLASS          \
  PSCC_QOS_PEAK_THROUGHPUT_CLASS_ELEMENT(UNSPECIFIED) \
  PSCC_QOS_PEAK_THROUGHPUT_CLASS_ELEMENT(LEVEL_1)     \
  PSCC_QOS_PEAK_THROUGHPUT_CLASS_ELEMENT(LEVEL_2)     \
  PSCC_QOS_PEAK_THROUGHPUT_CLASS_ELEMENT(LEVEL_4)     \
  PSCC_QOS_PEAK_THROUGHPUT_CLASS_ELEMENT(LEVEL_8)     \
  PSCC_QOS_PEAK_THROUGHPUT_CLASS_ELEMENT(LEVEL_16)    \
  PSCC_QOS_PEAK_THROUGHPUT_CLASS_ELEMENT(LEVEL_32)    \
  PSCC_QOS_PEAK_THROUGHPUT_CLASS_ELEMENT(LEVEL_64)    \
  PSCC_QOS_PEAK_THROUGHPUT_CLASS_ELEMENT(LEVEL_128)   \
  PSCC_QOS_PEAK_THROUGHPUT_CLASS_ELEMENT(LEVEL_256)

#define PSCC_QOS_PEAK_THROUGHPUT_CLASS_ELEMENT(LEVEL) pscc_qos_PEAK_THROUGHPUT_CLASS_##LEVEL,
typedef enum
{
  PSCC_QOS_PEAK_THROUGHPUT_CLASS
  pscc_number_of_qos_PEAK_THROUGHPUT_CLASS
} pscc_qos_peak_throughput_class_t;
#undef PSCC_QOS_PEAK_THROUGHPUT_CLASS_ELEMENT

/**
 * pscc_qos_mean_throughput_class_t
 * 3GPP QoS Mean Throughput class
 *
 * @pscc_qos_MEAN_THROUGHPUT_CLASS_UNSPECIFIED
 * @pscc_qos_MEAN_THROUGHPUT_CLASS_LEVEL_1       0.22 bps
 * @pscc_qos_MEAN_THROUGHPUT_CLASS_LEVEL_2       0.44 bps
 * @pscc_qos_MEAN_THROUGHPUT_CLASS_LEVEL_5       1.11 bps
 * @pscc_qos_MEAN_THROUGHPUT_CLASS_LEVEL_10      2.2  bps
 * @pscc_qos_MEAN_THROUGHPUT_CLASS_LEVEL_20      4.4  bps
 * @pscc_qos_MEAN_THROUGHPUT_CLASS_LEVEL_50      11.1 bps
 * @pscc_qos_MEAN_THROUGHPUT_CLASS_LEVEL_100     22   bps
 * @pscc_qos_MEAN_THROUGHPUT_CLASS_LEVEL_200     44   bps
 * @pscc_qos_MEAN_THROUGHPUT_CLASS_LEVEL_500     111  bps
 * @pscc_qos_MEAN_THROUGHPUT_CLASS_LEVEL_1000    0.22 kbps
 * @pscc_qos_MEAN_THROUGHPUT_CLASS_LEVEL_2000    0.44 kbps
 * @pscc_qos_MEAN_THROUGHPUT_CLASS_LEVEL_5000    1.11 kbps
 * @pscc_qos_MEAN_THROUGHPUT_CLASS_LEVEL_10000   2.2  kbps
 * @pscc_qos_MEAN_THROUGHPUT_CLASS_LEVEL_20000   4.4  kbps
 * @pscc_qos_MEAN_THROUGHPUT_CLASS_LEVEL_50000   11.1 kbps
 * @pscc_qos_MEAN_THROUGHPUT_CLASS_LEVEL_100000  22   kbps
 * @pscc_qos_MEAN_THROUGHPUT_CLASS_LEVEL_200000  44   kbps
 * @pscc_qos_MEAN_THROUGHPUT_CLASS_LEVEL_500000  111  kbps
 * @pscc_qos_MEAN_THROUGHPUT_CLASS_BEST_EFFORT
 *
 **/
#define PSCC_QOS_MEAN_THROUGHPUT_CLASS          \
  PSCC_QOS_MEAN_THROUGHPUT_CLASS_ELEMENT(UNSPECIFIED)  \
  PSCC_QOS_MEAN_THROUGHPUT_CLASS_ELEMENT(LEVEL_1)      \
  PSCC_QOS_MEAN_THROUGHPUT_CLASS_ELEMENT(LEVEL_2)      \
  PSCC_QOS_MEAN_THROUGHPUT_CLASS_ELEMENT(LEVEL_5)      \
  PSCC_QOS_MEAN_THROUGHPUT_CLASS_ELEMENT(LEVEL_10)     \
  PSCC_QOS_MEAN_THROUGHPUT_CLASS_ELEMENT(LEVEL_20)     \
  PSCC_QOS_MEAN_THROUGHPUT_CLASS_ELEMENT(LEVEL_50)     \
  PSCC_QOS_MEAN_THROUGHPUT_CLASS_ELEMENT(LEVEL_100)    \
  PSCC_QOS_MEAN_THROUGHPUT_CLASS_ELEMENT(LEVEL_200)    \
  PSCC_QOS_MEAN_THROUGHPUT_CLASS_ELEMENT(LEVEL_500)    \
  PSCC_QOS_MEAN_THROUGHPUT_CLASS_ELEMENT(LEVEL_1000)   \
  PSCC_QOS_MEAN_THROUGHPUT_CLASS_ELEMENT(LEVEL_2000)   \
  PSCC_QOS_MEAN_THROUGHPUT_CLASS_ELEMENT(LEVEL_5000)   \
  PSCC_QOS_MEAN_THROUGHPUT_CLASS_ELEMENT(LEVEL_10000)  \
  PSCC_QOS_MEAN_THROUGHPUT_CLASS_ELEMENT(LEVEL_20000)  \
  PSCC_QOS_MEAN_THROUGHPUT_CLASS_ELEMENT(LEVEL_50000)  \
  PSCC_QOS_MEAN_THROUGHPUT_CLASS_ELEMENT(LEVEL_100000) \
  PSCC_QOS_MEAN_THROUGHPUT_CLASS_ELEMENT(LEVEL_200000) \
  PSCC_QOS_MEAN_THROUGHPUT_CLASS_ELEMENT(LEVEL_500000) \
  PSCC_QOS_MEAN_THROUGHPUT_CLASS_ELEMENT(BEST_EFFORT)

#define PSCC_QOS_MEAN_THROUGHPUT_CLASS_ELEMENT(LEVEL) pscc_qos_MEAN_THROUGHPUT_CLASS_##LEVEL,
typedef enum
{
  PSCC_QOS_MEAN_THROUGHPUT_CLASS
  pscc_number_of_qos_MEAN_THROUGHPUT_CLASS
} pscc_qos_mean_throughput_class_t;
#undef PSCC_QOS_MEAN_THROUGHPUT_CLASS_ELEMENT

/**
 * pscc_qos_traffic_class_t
 * 3GPP QoS Traffic Class.
 * Used to specify traffic class for PDP context
 *
 * @pscc_qos_TRAFFIC_CLASS_UNSPECIFIED
 * @pscc_qos_TRAFFIC_CLASS_CONVERSATIONAL
 * @pscc_qos_TRAFFIC_CLASS_STREAMING
 * @pscc_qos_TRAFFIC_CLASS_INTERACTIVE
 * @pscc_qos_TRAFFIC_CLASS_BACKGROUND
 *
 **/
#define PSCC_QOS_TRAFFIC_CLASS          \
  PSCC_QOS_TRAFFIC_CLASS_ELEMENT(UNSPECIFIED) \
  PSCC_QOS_TRAFFIC_CLASS_ELEMENT(CONVERSATIONAL) \
  PSCC_QOS_TRAFFIC_CLASS_ELEMENT(STREAMING) \
  PSCC_QOS_TRAFFIC_CLASS_ELEMENT(INTERACTIVE) \
  PSCC_QOS_TRAFFIC_CLASS_ELEMENT(BACKGROUND)

#define PSCC_QOS_TRAFFIC_CLASS_ELEMENT(LEVEL) pscc_qos_TRAFFIC_CLASS_##LEVEL,
typedef enum
{
  PSCC_QOS_TRAFFIC_CLASS
  pscc_number_of_qos_TRAFFIC_CLASS
} pscc_qos_traffic_class_t;
#undef PSCC_QOS_TRAFFIC_CLASS_ELEMENT

/**
 * pscc_qos_delivery_order_t
 * 3GPP QoS Delivery order Class.
 * Used to specify if out-of-sequence SDU's are accepted or not.
 *
 * @pscc_qos_DELIVERY_ORDER_UNSPECIFIED
 * @pscc_qos_DELIVERY_ORDER_YES
 * @pscc_qos_DELIVERY_ORDER_NO
 *
 **/
#define PSCC_QOS_DELIVERY_ORDER          \
  PSCC_QOS_DELIVERY_ORDER_ELEMENT(UNSPECIFIED) \
  PSCC_QOS_DELIVERY_ORDER_ELEMENT(YES) \
  PSCC_QOS_DELIVERY_ORDER_ELEMENT(NO)

#define PSCC_QOS_DELIVERY_ORDER_ELEMENT(LEVEL) pscc_qos_DELIVERY_ORDER_##LEVEL,
typedef enum
{
  PSCC_QOS_DELIVERY_ORDER
  pscc_number_of_qos_DELIVERY_ORDER
} pscc_qos_delivery_order_t;
#undef PSCC_QOS_DELIVERY_ORDER_ELEMENT

/**
 * pscc_qos_delivery_erroneous_sdu_t
 * 3GPP QoS Delivery erroneous sdu Class.
 * Used to specify if erroneous SDU's shall be delivered or not
 *
 * @pscc_qos_DELIVERY_ERRONEOUS_SDU_UNSPECIFIED
 * @pscc_qos_DELIVERY_ERRONEOUS_SDU_NO_DETECT
 * @pscc_qos_DELIVERY_ERRONEOUS_SDU_YES
 * @pscc_qos_DELIVERY_ERRONEOUS_SDU_NO
 *
 **/
#define PSCC_QOS_DELIVERY_ERRONEOUS_SDU          \
  PSCC_QOS_DELIVERY_ERRONEOUS_SDU_ELEMENT(UNSPECIFIED) \
  PSCC_QOS_DELIVERY_ERRONEOUS_SDU_ELEMENT(NO_DETECT) \
  PSCC_QOS_DELIVERY_ERRONEOUS_SDU_ELEMENT(YES) \
  PSCC_QOS_DELIVERY_ERRONEOUS_SDU_ELEMENT(NO)

#define PSCC_QOS_DELIVERY_ERRONEOUS_SDU_ELEMENT(LEVEL) pscc_qos_DELIVERY_ERRONEOUS_SDU_##LEVEL,
typedef enum
{
  PSCC_QOS_DELIVERY_ERRONEOUS_SDU
  pscc_number_of_qos_DELIVERY_ERRONEOUS_SDU
} pscc_qos_delivery_erroneous_sdu_t;
#undef PSCC_QOS_DELIVERY_ERRONEOUS_SDU_ELEMENT

/**
 * pscc_qos_residual_ber_t
 * 3GPP QoS Residual Bit error Ratio Class
 * Used to indicate undetected bit error ratio in the delivered SDU's
 *
 * @pscc_qos_RESIDUAL_BER_UNSPECIFIED
 * @pscc_qos_RESIDUAL_BER_5_E_NEG2
 * @pscc_qos_RESIDUAL_BER_1_E_NEG2
 * @pscc_qos_RESIDUAL_BER_5_E_NEG3
 * @pscc_qos_RESIDUAL_BER_4_E_NEG3
 * @pscc_qos_RESIDUAL_BER_1_E_NEG3
 * @pscc_qos_RESIDUAL_BER_1_E_NEG4
 * @pscc_qos_RESIDUAL_BER_1_E_NEG5
 * @pscc_qos_RESIDUAL_BER_1_E_NEG6
 * @pscc_qos_RESIDUAL_BER_6_E_NEG8
 *
 **/
#define PSCC_QOS_RESIDUAL_BER          \
  PSCC_QOS_RESIDUAL_BER_ELEMENT(UNSPECIFIED) \
  PSCC_QOS_RESIDUAL_BER_ELEMENT(5_E_NEG2) \
  PSCC_QOS_RESIDUAL_BER_ELEMENT(1_E_NEG2) \
  PSCC_QOS_RESIDUAL_BER_ELEMENT(5_E_NEG3) \
  PSCC_QOS_RESIDUAL_BER_ELEMENT(4_E_NEG3) \
  PSCC_QOS_RESIDUAL_BER_ELEMENT(1_E_NEG3) \
  PSCC_QOS_RESIDUAL_BER_ELEMENT(1_E_NEG4) \
  PSCC_QOS_RESIDUAL_BER_ELEMENT(1_E_NEG5) \
  PSCC_QOS_RESIDUAL_BER_ELEMENT(1_E_NEG6) \
  PSCC_QOS_RESIDUAL_BER_ELEMENT(6_E_NEG8)

#define PSCC_QOS_RESIDUAL_BER_ELEMENT(LEVEL) pscc_qos_RESIDUAL_BER_##LEVEL,
typedef enum
{
  PSCC_QOS_RESIDUAL_BER
  pscc_number_of_qos_RESIDUAL_BER
} pscc_qos_residual_ber_t;
#undef PSCC_QOS_RESIDUAL_BER_ELEMENT

/**
 * pscc_qos_sdu_error_ratio_t
 * 3GPP QoS SDU error Ratio Class
 * Used to indicate the fraction of SDU's lost or detected as erroneous
 *
 * @pscc_qos_SDU_ERROR_RATIO_UNSPECIFIED
 * @pscc_qos_SDU_ERROR_RATIO_1_E_NEG2
 * @pscc_qos_SDU_ERROR_RATIO_7_E_NEG3
 * @pscc_qos_SDU_ERROR_RATIO_1_E_NEG3
 * @pscc_qos_SDU_ERROR_RATIO_1_E_NEG4
 * @pscc_qos_SDU_ERROR_RATIO_1_E_NEG5
 * @pscc_qos_SDU_ERROR_RATIO_1_E_NEG6
 * @pscc_qos_SDU_ERROR_RATIO_1_E_NEG1
 *
 **/
#define PSCC_QOS_SDU_ERROR_RATIO       \
  PSCC_QOS_SDU_ERROR_RATIO_ELEMENT(UNSPECIFIED) \
  PSCC_QOS_SDU_ERROR_RATIO_ELEMENT(1_E_NEG2) \
  PSCC_QOS_SDU_ERROR_RATIO_ELEMENT(7_E_NEG3) \
  PSCC_QOS_SDU_ERROR_RATIO_ELEMENT(1_E_NEG3) \
  PSCC_QOS_SDU_ERROR_RATIO_ELEMENT(1_E_NEG4) \
  PSCC_QOS_SDU_ERROR_RATIO_ELEMENT(1_E_NEG5) \
  PSCC_QOS_SDU_ERROR_RATIO_ELEMENT(1_E_NEG6) \
  PSCC_QOS_SDU_ERROR_RATIO_ELEMENT(1_E_NEG1)

#define PSCC_QOS_SDU_ERROR_RATIO_ELEMENT(LEVEL) pscc_qos_SDU_ERROR_RATIO_##LEVEL,
typedef enum
{
  PSCC_QOS_SDU_ERROR_RATIO
  pscc_number_of_qos_SDU_ERROR_RATIO
} pscc_qos_sdu_error_ratio_t;
#undef PSCC_QOS_SDU_ERROR_RATIO_ELEMENT

/**
 * pscc_qos_traffic_handling_priority_t
 * 3GPP QoS Traffic Handling priority Class
 * Used to specify the relative importance for handling all SDU's belonging to the UMTS bearer
 * compared to the SDU's of other bearers.
 *
 * @pscc_qos_TRAFFIC_HANDLING_PRIORITY_UNSPECIFIED
 * @pscc_qos_TRAFFIC_HANDLING_PRIORITY_LEVEL_1
 * @pscc_qos_TRAFFIC_HANDLING_PRIORITY_LEVEL_2
 * @pscc_qos_TRAFFIC_HANDLING_PRIORITY_LEVEL_3
 *
 **/
#define PSCC_QOS_TRAFFIC_HANDLING_PRIORITY       \
  PSCC_QOS_TRAFFIC_HANDLING_PRIORITY_ELEMENT(UNSPECIFIED) \
  PSCC_QOS_TRAFFIC_HANDLING_PRIORITY_ELEMENT(LEVEL_1) \
  PSCC_QOS_TRAFFIC_HANDLING_PRIORITY_ELEMENT(LEVEL_2) \
  PSCC_QOS_TRAFFIC_HANDLING_PRIORITY_ELEMENT(LEVEL_3)

#define PSCC_QOS_TRAFFIC_HANDLING_PRIORITY_ELEMENT(LEVEL) pscc_qos_TRAFFIC_HANDLING_PRIORITY_##LEVEL,
typedef enum
{
  PSCC_QOS_TRAFFIC_HANDLING_PRIORITY
  pscc_number_of_qos_TRAFFIC_HANDLING_PRIORITY
} pscc_qos_traffic_handling_priority_t;
#undef PSCC_QOS_TRAFFIC_HANDLING_PRIORITY_ELEMENT

/**
 * pscc_qos_signalling_indication_t
 * 3GPP QoS Signalling indication Class
 * Used to indicate signalling content of submitted SDU's for a PDP context
 *
 * @pscc_qos_SIGNALLING_INDICATION_NOT_OPTIMIZED_FOR_SIGNALLING_TRAFFIC
 * @pscc_qos_SIGNALLING_INDICATION_OPTIMIZED_FOR_SIGNALLING_TRAFFIC
 *
 **/
#define PSCC_QOS_SIGNALLING_INDICATION       \
  PSCC_QOS_SIGNALLING_INDICATION_ELEMENT(NOT_OPTIMIZED_FOR_SIGNALLING_TRAFFIC) \
  PSCC_QOS_SIGNALLING_INDICATION_ELEMENT(OPTIMIZED_FOR_SIGNALLING_TRAFFIC)

#define PSCC_QOS_SIGNALLING_INDICATION_ELEMENT(LEVEL) pscc_qos_SIGNALLING_INDICATION_##LEVEL,
typedef enum
{
  PSCC_QOS_SIGNALLING_INDICATION
  pscc_number_of_qos_SIGNALLING_INDICATION
} pscc_qos_signalling_indication_t;
#undef PSCC_QOS_SIGNALLING_INDICATION_ELEMENT

/**
 * pscc_qos_source_statistics_descriptor_t
 * 3GPP QoS Signalling Source Statistics descriptor
 * Specifies the characteristics of the source of the submitted SDU's
 *
 * @pscc_qos_SOURCE_STATISTICS_DESCRIPTOR_UNKNOWN
 * @pscc_qos_SOURCE_STATISTICS_DESCRIPTOR_SPEECH
 *
 **/
#define PSCC_QOS_SOURCE_STATISTICS_DESCRIPTOR       \
  PSCC_QOS_SOURCE_STATISTICS_DESCRIPTOR_ELEMENT(UNKNOWN) \
  PSCC_QOS_SOURCE_STATISTICS_DESCRIPTOR_ELEMENT(SPEECH)

#define PSCC_QOS_SOURCE_STATISTICS_DESCRIPTOR_ELEMENT(LEVEL) pscc_qos_SOURCE_STATISTICS_DESCRIPTOR_##LEVEL,
typedef enum
{
  PSCC_QOS_SOURCE_STATISTICS_DESCRIPTOR
  pscc_number_of_qos_SOURCE_STATISTICS_DESCRIPTOR
} pscc_qos_source_statistics_descriptor_t;
#undef PSCC_QOS_SOURCE_STATISTICS_DESCRIPTOR_ELEMENT


/**
 * pscc_reason_t
 * disconnect reasons
 *
 * @pscc_reason_pdp_context_activation_failed                - pdp context activation failed, unspecified reason
 * @pscc_reason_pdp_context_activation_rejected              - pdp context activation rejected
 * @pscc_reason_pdp_context_nw_deactivated                   - pdp context deactivated, network initiated
 * @pscc_reason_pdp_context_nw_deactivated_reset             - pdp context deactivated, network initiated and pscc will reset
 * @pscc_reason_pdp_context_ms_deactivated                   - pdp context deactivated, ms requested
 *
 **/
#define PSCC_REASON                                                    \
  PSCC_REASON_ELEMENT(pdp_context_ms_deactivated)                      \
  PSCC_REASON_ELEMENT(pdp_context_activation_failed)                   \
  PSCC_REASON_ELEMENT(pdp_context_activation_rejected)                 \
  PSCC_REASON_ELEMENT(pdp_context_nw_deactivated)                      \
  PSCC_REASON_ELEMENT(pdp_context_nw_deactivated_reset)

#define PSCC_REASON_ELEMENT(LEVEL) pscc_reason_##LEVEL,
typedef enum
{
  PSCC_REASON
  pscc_number_of_reasons
} pscc_reason_t;
#undef PSCC_REASON_ELEMENT

/**
 * pscc_connection_status_t
 * connection status valuesd
 *
 * @pscc_conncetion_status_disconnected
 * @pscc_conncetion_status_connecting
 * @pscc_conncetion_status_connected
 * @pscc_conncetion_status_disconnecting
 *
 **/
#define PSCC_CONNECTION_STATUS                      \
  PSCC_CONNECTION_STATUS_ELEMENT(disconnected)      \
  PSCC_CONNECTION_STATUS_ELEMENT(connecting)        \
  PSCC_CONNECTION_STATUS_ELEMENT(connected)         \
  PSCC_CONNECTION_STATUS_ELEMENT(disconnecting)     \
  PSCC_CONNECTION_STATUS_ELEMENT(modifying)

#define PSCC_CONNECTION_STATUS_ELEMENT(VAL) pscc_connection_status_##VAL,
typedef enum
{
  PSCC_CONNECTION_STATUS
  pscc_number_of_connection_status
} pscc_connection_status_t;
#undef PSCC_CONNECTION_STATUS_ELEMENT

/**
 * pscc_attach_mode_t
 * attach mode values
 *
 * @pscc_attach_mode_manual
 * @pscc_attach_mode_automatic
 *
 **/
#define PSCC_ATTACH_MODE                    \
  PSCC_ATTACH_MODE_ELEMENT(manual)          \
  PSCC_ATTACH_MODE_ELEMENT(automatic)

#define PSCC_ATTACH_MODE_ELEMENT(LEVEL) pscc_attach_mode_##LEVEL,
typedef enum
{
  PSCC_ATTACH_MODE
  pscc_number_of_attach_mode
} pscc_attach_mode_t;
#undef PSCC_ATTACH_MODE_ELEMENT

/**
 * pscc_ping_block_mode_t
 * ping block mode values
 *
 * @pscc_ping_block_mode_enabled
 * @pscc_ping_block_mode_disabled
 *
 **/
#define PSCC_PING_BLOCK_MODE                    \
  PSCC_PING_BLOCK_MODE_ELEMENT(enabled)         \
  PSCC_PING_BLOCK_MODE_ELEMENT(disabled)

#define PSCC_PING_BLOCK_MODE_ELEMENT(LEVEL) pscc_ping_block_mode_##LEVEL,
typedef enum
{
  PSCC_PING_BLOCK_MODE
  pscc_number_of_ping_block_mode
} pscc_ping_block_mode_t;
#undef PSCC_PING_BLOCK_MODE_ELEMENT


/**
 * pscc_attach_status_t
 * attach status values
 *
 * @pscc_attach_status_attached
 * @pscc_attach_status_detached
 *
 **/
#define PSCC_ATTACH_STATUS                      \
  PSCC_ATTACH_STATUS_ELEMENT(attached)          \
  PSCC_ATTACH_STATUS_ELEMENT(detached)

#define PSCC_ATTACH_STATUS_ELEMENT(LEVEL) pscc_attach_status_##LEVEL,
typedef enum
{
  PSCC_ATTACH_STATUS
  pscc_number_of_attach_status
} pscc_attach_status_t;
#undef PSCC_ATTACH_STATUS_ELEMENT

/** Packet Prioritization.
 * The defined values are used to build a bitmask indicating the type
 * of data that shall be prioritized for a connection.
 *
 * @param PSCC_PACKET_PRIORITY_UNSPECIFIED      no prioritization
 * @param PSCC_PACKET_PRIORITY_TCP_ACK          prioritize TCP ACK
 * @param PSCC_PACKET_PRIORITY_TCP_SYN          prioritize TCP SYN
 * @param PSCC_PACKET_PRIORITY_TCP_FIN          prioritize TCP FIN
 * @param PSCC_PACKET_PRIORITY_UDP              prioritize UDP
 * @param PSCC_PACKET_PRIORITY_SERVICE_RTP_RTCP prioritize RTP/RTCP. This priority setting may
 *                                                 be used together with the service type setting
 *                                                 of the FLOWSPEC socket option.
 * @param PSCC_PACKET_PRIORITY_SERVICE_HIDDEN   prioritize packets where the  type of data is
 *                                                 not given. This priority setting may be used
 *                                                 together with the service type setting of
 *                                                 the FLOWSPEC socket option.
 */
#define PSCC_PACKET_PRIORITY_UNSPECIFIED      0x00000000
#define PSCC_PACKET_PRIORITY_TCP_ACK          0x00000001
#define PSCC_PACKET_PRIORITY_TCP_SYN          0x00000002
#define PSCC_PACKET_PRIORITY_TCP_FIN          0x00000004
#define PSCC_PACKET_PRIORITY_UDP              0x00000008
#define PSCC_PACKET_PRIORITY_SERVICE_RTP_RTCP 0x00000010
#define PSCC_PACKET_PRIORITY_SERVICE_HIDDEN   0x00000020
#define PSCC_PACKET_PRIORITY_ALL \
  (PSCC_PACKET_PRIORITY_UNSPECIFIED|         \
   PSCC_PACKET_PRIORITY_TCP_ACK|             \
   PSCC_PACKET_PRIORITY_TCP_SYN|             \
   PSCC_PACKET_PRIORITY_TCP_FIN|             \
   PSCC_PACKET_PRIORITY_UDP|                 \
   PSCC_PACKET_PRIORITY_SERVICE_RTP_RTCP|    \
   PSCC_PACKET_PRIORITY_SERVICE_HIDDEN)


/** IP MAP
 * The defined values are used to build a bitmask indicating the type
 * addresses that are available for a given PDP context.
 *
 * @param PSCC_IPV4_ADDRESS_PRESENT      Ipv4 PDP Address present
 * @param PSCC_IPV6_ADDRESS_PRESENT      IPv6 PDP Address present
 * @param PSCC_IPV4_PDNS_PRESENT         IPv4 Primary DNS present
 * @param PSCC_IPV6_PDNS_PRESENT         IPv6 Primary DNS present
 * @param PSCC_IPV4_SDNS_PRESENT         IPv4 Secondary DNS present
 * @param PSCC_IPV6_SDNS_PRESENT         IPv6 Secondary DNS present

 */
#define PSCC_IPV4_ADDRESS_PRESENT      0x00000001
#define PSCC_IPV6_ADDRESS_PRESENT      0x00000002
#define PSCC_IPV4_PDNS_PRESENT         0x00000004
#define PSCC_IPV6_PDNS_PRESENT         0x00000008
#define PSCC_IPV4_SDNS_PRESENT         0x00000010
#define PSCC_IPV6_SDNS_PRESENT         0x00000020


/**
 * pscc_req_create_t - create a PS connection object
 *
 * @id      message id, set to pscc_req_create
 * @connid  connection id
 *          (0)  indicates that server shall select a connection id
 *          (>0) indicates that server shall try to use the proposed connection id
 * @parameter_list_p No parameters supported
 *
 * Response: steps_resp_create
 * Related event(s): -
 *
 **/
typedef mpl_msg_req_t pscc_req_create_t;

/**
 * pscc_resp_create_t - response to pscc_req_create
 *
 * @id      message id, set to pscc_create
 * @connid  connection id
 * @result  result of operation
 *          Possible result values:
 *           pscc_result_ok
 *           pscc_result_failed_operation_not_allowed
 *           pscc_result_failed_object_exists
 *           pscc_result_failed_unspecified
 * @parameter_list_p No parameters supported
 *
 **/
typedef mpl_msg_resp_t pscc_resp_create_t;

/**
 * pscc_req_destroy_t - destroy a PS connection object
 *
 * Note that the PS connection object must be fully disconnected before
 * the object can be destroyed.
 *
 * @id      message id, set to pscc_req_destroy
 * @connid  connection id
 * @parameter_list_p No parameters supported
 *
 * Response: pscc_resp_destroy
 * Related events: -
 *
 **/
typedef mpl_msg_req_t pscc_req_destroy_t;

/**
 * pscc_resp_destroy_t - response to pscc_req_destroy
 *
 * @id      message id, set to pscc_resp_destroy
 * @connid  connection id
 * @result  result of operation
 *          Possible result values:
 *           pscc_result_ok
 *           pscc_result_failed_operation_not_allowed
 *           pscc_result_failed_object_not_found
 *           pscc_result_failed_unspecified
 * @parameter_list_p No parameters supported
 *
 **/
typedef mpl_msg_resp_t pscc_resp_destroy_t;


/**
 * pscc_req_connect_t - connect a PS connection object
 *
 * This request will activate a PDP context and create a PS
 * net device.
 *
 * @id      message id, set to pscc_req_connect
 * @connid  connection id
 * @parameter_list_p No parameters supported
 *
 * Response: pscc_resp_connect
 * Related events: pscc_event_connecting
 *                 pscc_event_connected,
 *                 pscc_event_disconnecting,
 *                 pscc_event_disconnected,
 *
 **/
typedef mpl_msg_req_t pscc_req_connect_t;


/**
 * pscc_resp_connect_t - response to pscc_req_connect
 *
 * @id      message id, set to pscc_resp_connect
 * @connid  connection id
 * @result  result of operation
 *          Possible result values:
 *           pscc_result_ok
 *           pscc_result_failed_operation_not_allowed
 *           pscc_result_failed_object_not_found
 *           pscc_result_failed_unspecified
 * @parameter_list_p No parameters supported
 *
 **/
typedef mpl_msg_resp_t pscc_resp_connect_t;


/**
 * pscc_req_disconnect_t - disconnect a PS connection object
 *
 * @id      message id, set to pscc_req_disconnect
 * @connid  connection id
 * @parameter_list_p No parameters supported
 *
 * Response: pscc_resp_connect
 * Related events: pscc_event_disconnecting,
 *                 pscc_event_disconnected,
 *
 **/
typedef mpl_msg_req_t pscc_req_disconnect_t;

/**
 * pscc_resp_disconnect_t - response to pscc_req_disconnect
 *
 * @id      message id, set to pscc_resp_disconnect
 * @connid  connection id
 * @result  result of operation
 *          Possible result values:
 *           pscc_result_ok
 *           pscc_result_failed_operation_not_allowed
 *           pscc_result_failed_object_not_found
 *           pscc_result_failed_unspecified
 * @parameter_list_p No parameters supported
 *
 **/
typedef mpl_msg_resp_t pscc_resp_disconnect_t;

/**
 * pscc_req_query_attach_mode_t - Query PS attach mode
 *
 * @id      message id, set to pscc_req_query_attach_mode
 * @connid  dummy
 * @parameter_list_p No parameters supported
 *
 * Response: pscc_resp_query_attach_mode
 * Related events: pscc_event_attach_mode_queried
 *
 **/
typedef mpl_msg_req_t pscc_req_query_attach_mode_t;

/**
 * pscc_resp_query_attach_mode_t - response to pscc_req_query_attach_mode
 *
 * @id      message id, set to pscc_resp_query_attach_mode
 * @connid  dummy
 * @result  result of operation
 *          Possible result values:
 *           pscc_result_ok
 *           pscc_result_failed_operation_not_allowed
 *           pscc_result_failed_object_exists
 *           pscc_result_failed_unspecified
 * @parameter_list_p No parameters supported
 *
 **/
typedef mpl_msg_resp_t pscc_resp_query_attach_mode_t;

/**
 * pscc_req_configure_attach_mode_t - Configure PS attach mode
 *
 * @id      message id, set to pscc_req_configure_attach_mode
 * @connid  dummy
 * @parameter_list_p Request parameters:
 *                   pscc_paramid_attach_mode
 *
 * Response: pscc_resp_configure_attach_mode
 * Related events: pscc_event_attach_mode_configured,
 *                 pscc_event_attach_mode_configure_failed
 *
 **/
typedef mpl_msg_req_t pscc_req_configure_attach_mode_t;

/**
 * pscc_resp_configure_attach_mode_t - response to pscc_req_configure_attach_mode
 *
 * @id      message id, set to pscc_resp_configure_attach_mode
 * @connid  dummy
 * @result  result of operation
 *          Possible result values:
 *           pscc_result_ok
 *           pscc_result_failed_operation_not_allowed
 *           pscc_result_failed_object_exists
 *           pscc_result_failed_unspecified
 * @parameter_list_p No parameters supported
 *
 **/
typedef mpl_msg_resp_t pscc_resp_configure_attach_mode_t;

/**
 * pscc_req_query_ping_block_mode_t - Query ping block mode
 *
 * @id      message id, set to pscc_req_ping_block_mode
 * @connid  dummy
 * @parameter_list_p No parameters supported
 *
 * Response: pscc_resp_ping_block_mode
 * Related events: pscc_event_ping_block_mode_queried
 *
 **/
typedef mpl_msg_req_t pscc_req_query_ping_block_mode_t;

/**
 * pscc_resp_query_ping_block_mode_t - response to pscc_req_ping_block_mode
 *
 * @id      message id, set to pscc_resp_ping_block_mode
 * @connid  dummy
 * @result  result of operation
 *          Possible result values:
 *           pscc_result_ok
 *           pscc_result_failed_operation_not_allowed
 *           pscc_result_failed_object_exists
 *           pscc_result_failed_unspecified
 * @parameter_list_p No parameters supported
 *
 **/
typedef mpl_msg_resp_t pscc_resp_query_ping_block_mode_t;

/**
 * pscc_req_configure_ping_block_mode_t - Configure ping block mode
 *
 * @id      message id, set to pscc_req_configure_ping_block_mode
 * @connid  dummy
 * @parameter_list_p Request parameters:
 *                   pscc_paramid_ping_block_mode
 *
 * Response: pscc_resp_configure_ping_block_mode
 * Related events: pscc_event_ping_block_mode_configured,
 *                 pscc_event_ping_block_mode_configure_failed
 *
 **/
typedef mpl_msg_req_t pscc_req_configure_ping_block_mode_t;

/**
 * pscc_resp_configure_ping_block_mode_t - response to pscc_req_configure_ping_block_mode
 *
 * @id      message id, set to pscc_resp_configure_ping_block_mode
 * @connid  dummy
 * @result  result of operation
 *          Possible result values:
 *           pscc_result_ok
 *           pscc_result_failed_operation_not_allowed
 *           pscc_result_failed_object_exists
 *           pscc_result_failed_unspecified
 * @parameter_list_p No parameters supported
 *
 **/
typedef mpl_msg_resp_t pscc_resp_configure_ping_block_mode_t;

/**
 * pscc_req_attach_t - attach PS to GPRS service
 *
 * @id      message id, set to pscc_req_attach
 * @parameter_list_p Request parameters:
 *                   pscc_paramid_ct
 *
 * Response: pscc_resp_attach
 * Related events: pscc_event_attached,
 *                 pscc_event_attach_failed,
 *
 **/
typedef mpl_msg_req_t pscc_req_attach_t;

/**
 * pscc_resp_attach_t - response to pscc_req_attach
 *
 * @id      message id, set to pscc_resp_attach
 * @result  result of operation
 *          Possible result values:
 *           pscc_result_ok
 *           pscc_result_failed_operation_not_allowed
 *           pscc_result_failed_unspecified
 * @parameter_list_p Response parameters:
 *                   pscc_paramid_ct
 *
 **/
typedef mpl_msg_resp_t pscc_resp_attach_t;

/**
 * pscc_req_attach_status_t - check attach status
 *
 * @id      message id, set to pscc_req_attach_status
 * @parameter_list_p Request parameters:
 *                   pscc_paramid_ct
 *
 * Response: pscc_resp_attach_status
 *
 **/
typedef mpl_msg_req_t pscc_req_attach_status_t;

/**
 * pscc_resp_attach_status_t - response to pscc_req_attach_status
 *
 * @id      message id, set to pscc_resp_attach_status
 * @result  result of operation
 *          Possible result values:
 *           pscc_result_ok
 *           pscc_result_failed_operation_not_allowed
 *           pscc_result_failed_unspecified
 * @parameter_list_p Response parameters:
 *                   pscc_paramid_attach_status
 *                   pscc_paramid_ct
 *
 **/
typedef mpl_msg_resp_t pscc_resp_attach_status_t;

/**
 * pscc_req_detach_t - detach from GPRS service
 *
 * @id      message id, set to pscc_req_detach
 * @parameter_list_p Request parameters:
 *                   pscc_paramid_ct
 *
 * Response: pscc_resp_detach
 * Related events: pscc_event_detached,
 *                 pscc_event_detach_failed,
 *
 **/
typedef mpl_msg_req_t pscc_req_detach_t;

/**
 * pscc_resp_attach_t - response to pscc_req_detach
 *
 * @id      message id, set to pscc_resp_detach
 * @result  result of operation
 *          Possible result values:
 *           pscc_result_ok
 *           pscc_result_failed_operation_not_allowed
 *           pscc_result_failed_unspecified
 * @parameter_list_p Response parameters:
 *                   pscc_paramid_ct
 *
 **/
typedef mpl_msg_resp_t pscc_resp_detach_t;


/**
 * pscc_req_set_t - Set parameters on PS connection object
 *
 * @id      message id, set to pscc_req_set
 * @connid  connection id
 * @parameter_list_p List of parameters to set
 *
 * Response: pscc_resp_set
 *
 **/
typedef mpl_msg_req_t pscc_req_set_t;

/**
 * pscc_resp_set_t - response to pscc_req_set
 *
 * @id      message id, set to pscc_resp_set
 * @connid  connection id
 * @result  result of operation
 *          Possible result values:
 *           pscc_result_ok
 *           pscc_result_failed_operation_not_allowed
 *           pscc_result_failed_object_exists
 *           pscc_result_failed_unspecified
 * @parameter_list_p No parameters supported
 *
 **/
typedef mpl_msg_resp_t pscc_resp_set_t;

/**
 * pscc_req_get_t - Get parameters on PS connection object
 *
 * @id      message id, set to pscc_req_set
 * @connid  connection id
 * @parameter_list_p List of parameters to retrieve
 *
 * Response: pscc_resp_get
 *
 **/
typedef mpl_msg_req_t pscc_req_get_t;

/**
 * pscc_resp_get_t - response to pscc_req_get
 *
 * @id      message id, set to pscc_resp_get
 * @connid  connection id
 * @result  result of operation
 *          Possible result values:
 *           pscc_result_ok
 *           pscc_result_failed_operation_not_allowed
 *           pscc_result_failed_object_not_found
 *           pscc_result_failed_unspecified
 * @parameter_list_p List of parameters retrieved
 *
 **/
typedef mpl_msg_resp_t pscc_resp_get_t;

/**
 * pscc_req_fetch_data_counters_t - Request to fetch data counters
 *
 * @id      message id, set to pscc_req_fetch_data_counters
 * @connid  connection id
 * @parameter_list_p No parameters supported
 *
 * Response: pscc_resp_fetch_data_counters
 * Related events: pscc_event_data_counters_fetched
 *
 **/
typedef mpl_msg_req_t pscc_req_fetch_data_counters_t;

/**
 * pscc_resp_fetch_data_counters_t - response to pscc_req_fetch_data_counters
 *
 * @id      message id, set to pscc_resp_fetch_data_counters
 * @connid  connection id
 * @result  result of operation
 *          Possible result values:
 *           pscc_result_ok
 *           pscc_result_failed_operation_not_allowed
 *           pscc_result_failed_object_not_found
 *           pscc_result_failed_unspecified
 * @parameter_list_p No parameters supported
 *
 **/
typedef mpl_msg_resp_t pscc_resp_fetch_data_counters_t;

/**
 * pscc_req_reset_data_counters_t - Request to reset data counters
 *
 * @id      message id, set to pscc_req_reste_data_counters
 * @connid  connection id
 * @parameter_list_p No parameters supported
 *
 * Response: pscc_resp_reset_data_counters
 * Related events: pscc_event_data_counters_reset
 *
 **/
typedef mpl_msg_req_t pscc_req_reset_data_counters_t;

/**
 * pscc_resp_reset_data_counters_t - response to pscc_req_reset_data_counters
 *
 * @id      message id, set to pscc_resp_reset_data_counters
 * @connid  connection id
 * @result  result of operation
 *          Possible result values:
 *           pscc_result_ok
 *           pscc_result_failed_operation_not_allowed
 *           pscc_result_failed_object_not_found
 *           pscc_result_failed_unspecified
 * @parameter_list_p No parameters supported
 *
 **/
typedef mpl_msg_resp_t pscc_resp_reset_data_counters_t;

/**
 * pscc_req_list_all_connections_t - Get all connection IDs
 *
 * @id      message id, set to pscc_req_list_all_connections
 * @connid  dummy
 * @parameter_list_p List of parameters to retrieve
 *
 * Response: pscc_resp_list_all_connections
 *
 **/
typedef mpl_msg_req_t pscc_req_list_all_connections_t;

/**
 * pscc_resp_list_all_connections_t - response to pscc_req_list_all_connections
 *
 * @id      message id, set to pscc_resp_list_all_connections
 * @connid  dummy
 * @result  result of operation
 *          Possible result values:
 *           pscc_result_ok
 *           pscc_result_failed_operation_not_allowed
 *           pscc_result_failed_object_not_found
 *           pscc_result_failed_unspecified
 * @parameter_list_p List of parameters retrieved
 *
 **/
typedef mpl_msg_resp_t pscc_resp_list_all_connections_t;
/**
 * pscc_req_modify_t - Modify the PDP context
 *
 * @id      message id, set to pscc_req_modify
 * @connid  dummy
 * @parameter_list_p List of parameters to update
 *
 * Response: pscc_resp_modify
 *
 **/
typedef mpl_msg_req_t pscc_req_modify_t;

/**
 * pscc_resp_modify_t - response to pscc_req_modify
 *
 * @id      message id, set to pscc_resp_modify
 * @connid  connection id
 * @result  result of operation
 *          Possible result values:
 *           pscc_result_ok
 *           pscc_result_failed_operation_not_allowed
 *           pscc_result_failed_object_not_found
 *           pscc_result_failed_unspecified
 * @parameter_list_p No parameters supported
 *
 **/
typedef mpl_msg_resp_t pscc_resp_modify_t;

/**
  * pscc_req_generate_uplink_data_t - Generate uplink data
 *
 * @id      message id, set to pscc_req_generate_uplink_data
 * @connid  connection id
 * @result  result of operation
 *          Possible result values:
 *           pscc_result_ok
 *           pscc_result_failed_operation_not_allowed
 *           pscc_result_failed_object_not_found
 *           pscc_result_failed_parameter_not_found
 *           pscc_result_failed_unspecified
 * @parameter_list_p No parameters supported
 *
 * Response: pscc_resp_generate_uplink_data
 *
 **/
typedef mpl_msg_req_t pscc_req_generate_uplink_data_t;

/**
 * pscc_resp_generate_uplink_data_t - response to pscc_req_generate_uplink_data
 *
 * @id      message id, set to pscc_resp_generate_uplink_data
 * @connid  connection id
 * @result  result of operation
 *          Possible result values:
 *           pscc_result_ok
 *           pscc_result_failed_operation_not_allowed
 *           pscc_result_failed_object_not_found
 *           pscc_result_failed_parameter_not_found
 *           pscc_result_failed_unspecified
 * @parameter_list_p No parameters supported
 *
 **/
typedef mpl_msg_resp_t pscc_resp_generate_uplink_data_t;

/**
  * pscc_req_init_handler_t - init PSCC handler functionality
 *
 * @id      message id, set to pscc_req_init
 * @result  result of operation
 *          Possible result values:
 *           pscc_result_ok
 *           pscc_result_failed_unspecified
 * @parameter_list_p No parameters supported
 *
 * Response: pscc_resp_init
 *
 **/
typedef mpl_msg_req_t pscc_req_init_handler_t;

/**
 * pscc_resp_init_t - response to pscc_req_init
 *
 * @id      message id, set to pscc_resp_init
 * @result  result of operation
 *          Possible result values:
 *           pscc_result_ok
 * @parameter_list_p No parameters supported
 *
 **/
typedef mpl_msg_resp_t pscc_resp_init_handler_t;
/**
 * pscc_event_connecting_t
 *
 * @id      message id, set to pscc_event_connecting
 * @connid  connection id
 * @parameter_list_p No parameters supported
 *
 **/
typedef mpl_msg_event_t pscc_event_connecting_t;

/**
 * pscc_event_connected_t
 *
 * @id      message id, set to pscc_event_connected
 * @connid  connection id
 * @parameter_list_p No parameters supported
 *
 **/
typedef mpl_msg_event_t pscc_event_connected_t;

/**
 * pscc_event_disconnecting_t
 *
 * @id      message id, set to pscc_event_disconnecting
 * @connid  connection id
 * @parameter_list_p No parameters supported
 *
 **/
typedef mpl_msg_event_t pscc_event_disconnecting_t;

/**
 * pscc_event_disconnected_t
 *
 * @id      message id, set to pscc_event_disconnected
 * @connid  connection id
 * @parameter_list_p Event parameters:
 *                   pscc_paramid_cause
 *                   pscc_paramid_reason
 *                   pscc_paramid_own_ip_address
 *                   pscc_paramid_pdp_type
 *
 **/
typedef mpl_msg_event_t pscc_event_disconnected_t;

/**
 * pscc_event_data_counters_fetched_t
 *
 * @id      message id, set to pscc_event_data_counters_fetched
 * @connid  connection id
 * @parameter_list_p Event parameters:
 *                   pscc_paramid_rx_data_count
 *                   pscc_paramid_tx_data_count
 *
 **/
typedef mpl_msg_event_t pscc_event_data_counters_fetched_t;

/**
 * pscc_event_data_counters_reset_t
 *
 * @id      message id, set to pscc_event_data_counters_reset
 * @connid  connection id
 * @parameter_list_p No parameters supported
 *
 **/
typedef mpl_msg_event_t pscc_event_data_counters_reset_t;

/**
 * pscc_event_modified_t
 *
 * @id      message id, set to pscc_event_modified
 * @connid  connection id (always set to 0)
 * @parameter_list_p No parameters supported
 *
 **/
typedef mpl_msg_event_t pscc_event_modified_t;

/**
 * pscc_event_modify_failed_t
 *
 * @id      message id, set to pscc_event_modify_failed
 * @connid  connection id (always set to 0)
 * @parameter_list_p Event parameters:
 *                   pscc_paramid_cause
 *                   pscc_paramid_reason
 *
 **/
typedef mpl_msg_event_t pscc_event_modify_failed_t;

/**
 * pscc_event_failure_t
 *
 * @id      message id, set to pscc_event_failure
 * @connid  connection id (always set to 0)
 * @parameter_list_p No parameters supported
 *
 **/
typedef mpl_msg_event_t pscc_event_failure_t;

/**
 * pscc_event_attached_t
 *
 * @id      message id, set to pscc_event_attached
 * @parameter_list_p Event parameters:
 *                   pscc_paramid_ct
 *
 **/
typedef mpl_msg_event_t pscc_event_attached_t;

/**
 * pscc_event_attach_failed_t
 *
 * @id      message id, set to pscc_event_attach_failed
 * @parameter_list_p Event parameters:
 *                   pscc_paramid_cause
 *                   pscc_paramid_ct
 *
 **/
typedef mpl_msg_event_t pscc_event_attach_failed_t;

/**
 * pscc_event_detached_t
 *
 * @id      message id, set to pscc_event_detached
 * @parameter_list_p Event parameters:
 *                   pscc_paramid_cause (optional)
 *                   pscc_paramid_ct
 **/
typedef mpl_msg_event_t pscc_event_detached_t;

/**
 * pscc_event_detach_failed_t
 *
 * @id      message id, set to pscc_event_detach_failed
 * @parameter_list_p Event parameters:
 *                   pscc_paramid_cause
 *                   pscc_paramid_ct
 *
 **/
typedef mpl_msg_event_t pscc_event_detach_failed_t;

/**
 * pscc_event_attach_mode_queried_t
 *
 * @id      message id, set to pscc_event_attach_mode_queried
 * @parameter_list_p Event parameters:
 *                   pscc_paramid_attach_mode
 *                   pscc_paramid_ct
 *
 **/
typedef mpl_msg_event_t pscc_event_attach_mode_queried_t;

/**
 * pscc_event_attach_mode_configured_t
 *
 * @id      message id, set to pscc_event_attach_mode_configured
 * @parameter_list_p Event parameters:
 *                   pscc_paramid_ct
 *
 **/
typedef mpl_msg_event_t pscc_event_attach_mode_configured_t;

/**
 * pscc_event_attach_mode_configure_failed_t
 *
 * @id      message id, set to pscc_event_attach_mode_configure_failed
 * @parameter_list_p Event parameters:
 *                   pscc_paramid_cause
 *                   pscc_paramid_ct
 *
 **/
typedef mpl_msg_event_t pscc_event_attach_mode_configure_failed_t;

/**
 * pscc_event_ping_block_mode_queried_t
 *
 * @id      message id, set to pscc_event_ping_block_mode_queried
 * @parameter_list_p Event parameters:
 *                   pscc_paramid_ping_block_mode
 *                   pscc_paramid_ct
 *
 **/
typedef mpl_msg_event_t pscc_event_ping_block_mode_queried_t;

/**
 * pscc_event_ping_block_mode_configured_t
 *
 * @id      message id, set to pscc_event_ping_block_mode_configured
 * @parameter_list_p Event parameters:
 *                   pscc_paramid_ct
 *
 **/
typedef mpl_msg_event_t pscc_event_ping_block_mode_configured_t;

/**
 * pscc_event_ping_block_mode_configure_failed_t
 *
 * @id      message id, set to pscc_event_ping_block_mode_configure_failed
 * @parameter_list_p Event parameters:
 *                   pscc_paramid_cause
 *                   pscc_paramid_ct
 *
 **/
typedef mpl_msg_event_t pscc_event_ping_block_mode_configure_failed_t;

/**
 * pscc_msg_t - union holding all message types that can be packed/unpacked
 *
 * @header              message type and id (common for all messages), this field is used
 *                      to indicate which message field in the union that is
 *                      valid, e.g., if set to pscc_req_connect the
 *                      req_connect message field is the field to access.
 * @common              contains fields common for all messages
 * @req                 generic request message
 * @resp                generic response message
 * @event               generic event message
 *
 * Messages:
 * @req_create          create request message
 * @resp_create         create response message
 * @req_destroy         destroy request message
 * @resp_destroy        destroy response message
 * @req_connect         connect request message
 * @resp_connect        connect response message
 * @req_disconnect      disconnect request message
 * @resp_disconnect     disconnect response message
 * @req_set             set request message
 * @resp_set            set response message
 * @req_get             get request message
 * @resp_get            get response message
 * @req_generate_uplink_data      generate uplink data request
 * @resp_generate_uplink_data     generate uplink data response
 * @req_query_ping_block_mode               query ping block mode request
 * @resp_query_ping_block_mode              query ping block mode response
 * @req_configure_ping_block_mode           configure ping block mode request
 * @resp_configure_ping_block_mode          configure ping block mode response
 * @event_connecting                        connecting event message
 * @event_connected                         connected event message
 * @event_disconnecting                     disconnecting event message
 * @event_disconnected                      disconnected event message
 * @event_ping_block_mode_queried           ping block mode queried event message
 * @event_ping_block_mode_configured        ping block mode configured event message
 * @event_ping_block_mode_configure_failed  ping block mode configuration failed event message
 *
 **/
typedef union
{
  mpl_msg_common_t       common;
  mpl_msg_req_t          req;
  mpl_msg_resp_t         resp;
  mpl_msg_event_t        event;
  pscc_req_create_t                 req_create;
  pscc_resp_create_t                  resp_create;
  pscc_req_destroy_t                  req_destroy;
  pscc_resp_destroy_t                 resp_destroy;
  pscc_req_attach_t                   req_attach;
  pscc_resp_attach_t                  resp_attach;
  pscc_req_attach_status_t            req_attach_status;
  pscc_resp_attach_status_t           resp_attach_status;
  pscc_req_detach_t                   req_detach;
  pscc_resp_detach_t                  resp_detach;
  pscc_req_connect_t                  req_connect;
  pscc_resp_connect_t                 resp_connect;
  pscc_req_disconnect_t               req_disconnect;
  pscc_resp_disconnect_t              resp_disconnect;
  pscc_req_fetch_data_counters_t      req_fetch_data_counters;
  pscc_resp_fetch_data_counters_t     resp_fetch_data_counters;
  pscc_req_reset_data_counters_t      req_reset_data_counters;
  pscc_resp_reset_data_counters_t     resp_reset_data_counters;
  pscc_req_query_attach_mode_t        req_query_attach_mode;
  pscc_resp_query_attach_mode_t       resp_query_attach_mode;
  pscc_req_configure_attach_mode_t    req_configure_attach_mode;
  pscc_resp_configure_attach_mode_t   resp_configure_attach_mode;
  pscc_req_set_t                      req_set;
  pscc_resp_set_t                     resp_set;
  pscc_req_get_t                      req_get;
  pscc_resp_get_t                     resp_get;
  pscc_req_list_all_connections_t     req_list_all_connections;
  pscc_resp_list_all_connections_t    resp_list_all_connections;
  pscc_req_modify_t                   req_modify;
  pscc_resp_modify_t                  resp_modify;
  pscc_req_generate_uplink_data_t             req_generate_uplink_data;
  pscc_resp_generate_uplink_data_t            resp_generate_uplink_data;
  pscc_req_init_handler_t                     req_init_handler;
  pscc_resp_init_handler_t                    resp_init_handler;
  pscc_req_query_ping_block_mode_t            req_query_ping_block_mode;
  pscc_resp_query_ping_block_mode_t           resp_query_ping_block_mode;
  pscc_req_configure_ping_block_mode_t        req_configure_ping_block_mode;
  pscc_resp_configure_ping_block_mode_t       resp_configure_ping_block_mode;
  pscc_event_attached_t                       event_attached;
  pscc_event_attach_failed_t                  event_attach_failed;
  pscc_event_detached_t                       event_detached;
  pscc_event_detach_failed_t                  event_detach_failed;
  pscc_event_attach_mode_queried_t            event_attach_mode_queried;
  pscc_event_attach_mode_configured_t         event_attach_mode_configured;
  pscc_event_attach_mode_configure_failed_t   event_attach_mode_configure_failed;
  pscc_event_connecting_t                     event_connecting;
  pscc_event_connected_t                      event_connected;
  pscc_event_disconnecting_t                  event_disconnecting;
  pscc_event_disconnected_t                   event_disconnected;
  pscc_event_data_counters_fetched_t          event_data_counters_fetched;
  pscc_event_data_counters_reset_t            event_data_counters_reset;
  pscc_event_modified_t                       event_modified;
  pscc_event_modify_failed_t                  event_modify_failed;
  pscc_event_failure_t                        event_failure;
  pscc_event_ping_block_mode_queried_t          event_ping_block_mode_queried;
  pscc_event_ping_block_mode_configured_t       event_ping_block_mode_configured;
  pscc_event_ping_block_mode_configure_failed_t event_ping_block_mode_configure_failed;
} pscc_msg_t;

#define PSCC_CONNID_PRESENT(msg_p) MPL_MSG_PARAM_PRESENT(msg_p, pscc_paramid_connid)
#define PSCC_GET_CONNID(msg_p) MPL_MSG_GET_PARAM_VALUE(msg_p, int, pscc_paramid_connid)
#define PSCC_CT_PRESENT(msg_p) MPL_MSG_PARAM_PRESENT(msg_p, pscc_paramid_ct)
#define PSCC_GET_CT(msg_p) MPL_MSG_GET_PARAM_VALUE(msg_p, uint32_t, pscc_paramid_ct)


/**
 * pscc_event_failure_buf - Pre-packet message buffer for failure event
 */
extern const char pscc_event_failure_buf[];

/**
 * pscc_msg_init - Initiate library
 *
 * @user_p  User pointer
 * @log_fp  Logging function
 *
 * Returns:  0 on success, -1 on failure
 *
 **/
int pscc_init(void* user_p, mpl_log_fp log_fp);

/**
 * pscc_msg_id_get_string
 *
 * Description: Get string corresponding to the message ID
 *
 * Parameters:
 *     msg_id:     Message ID
 *
 * Return Values : String
 *
 */
const char *
pscc_msg_id_get_string(pscc_msgid_t msg_id);

#ifdef  __cplusplus
}
#endif

#endif /* PSCC_MSG_H */
