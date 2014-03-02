#ifndef INCLUSION_GUARD_SMS_STUB_H
#define INCLUSION_GUARD_SMS_STUB_H
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include <stdint.h>

/**
* Signal Id's for the MAL SMS category.
*/
typedef enum {
    REQ_MAL_SMS_TEST,
    RESP_MAL_SMS_TEST,
    EVENT_MAL_SMS_TEST,

    // Request signal numbering
    REQUEST_MAL_SMS_SEND_TPDU,
    REQUEST_MAL_SMS_SEND_DELIVERREPORT,
    REQUEST_MAL_SMS_SET_MEMORY_CAPACITY_STATUS,
    REQUEST_MAL_SMS_MO_ROUTE_SET,
    REQUEST_MAL_SMS_MO_ROUTE_GET,
    REQUEST_MAL_CBS_REQUEST_ROUTING_SET,
    REQUEST_MAL_CBS_REQUEST_ROUTING_REMOVE,
    REQUEST_MAL_SMS_CBS_CONFIGURE,

    // Response signal numbering
    RESPONSE_MAL_SMS_SEND_TPDU,
    RESPONSE_MAL_SMS_SEND_DELIVERREPORT,
    RESPONSE_MAL_SMS_SET_MEMORY_CAPACITY_STATUS,
    RESPONSE_MAL_SMS_MO_ROUTE_SET,
    RESPONSE_MAL_SMS_MO_ROUTE_GET,
    RESPONSE_MAL_CBS_REQUEST_ROUTING_SET,
    RESPONSE_MAL_CBS_REQUEST_ROUTING_REMOVE,
    RESPONSE_MAL_SMS_CBS_CONFIGURE,

    // Indication signal numbering
    IND_MAL_MEMORY_CAPACITY_EXC,

    // Event signal numbering
    EVENT_MAL_SMS_NEW_MESSAGE,
    EVENT_MAL_CBS_IND
#ifdef SMS_SIM_TEST_PLATFORM
    ,SHUTDOWN_MAL_SMS_TEST
#endif
} MAL_SMS_SignalIdentifiers_t;


typedef struct {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
} mal_request_t;

typedef struct {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    SMS_TP_MessageReference_t msg_ref;  // TP-Message-Reference
    SMS_Error_t err_type;
} mal_response_send_TPDU_t;

typedef struct {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    uint16_t RP_Error;
} mal_request_send_deliver_report_t;

typedef struct {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    SMS_Error_t SMS_Error;
} mal_response_send_deliver_report_t;

typedef struct {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    uint8_t MemoryCapacityAvailable;
} mal_request_set_memory_capacity_status_t;

typedef struct {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    uint8_t sms_route;
} mal_request_route_set_t;

typedef struct {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    SMS_Error_t SMS_Error;
} mal_response_set_memory_capacity_status_t;

typedef struct {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    SMS_Error_t SMS_Error;
} mal_response_sms_cbs_config_t;

typedef struct {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    SMS_Error_t SMS_Error;
} mal_response_mo_route_set_t;

typedef struct {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    MAL_SMS_CBS_Error_t MAL_SMS_Error;
    mal_sms_route_info_t route_info;
} mal_response_mo_route_get_t;

typedef struct {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    SMS_Error_t CBS_Error;
    uint8_t SubscriptionNumber;
} mal_response_cbs_routing_set_t;

typedef struct {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    SMS_Error_t CBS_Error;
} mal_response_cbs_routing_remove_t;

typedef struct {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    SMS_SMSC_Address_TPDU_t SMS_SMSC_Address_TPDU;
} mal_received_sms_event_t;

typedef struct {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
} mal_event_t;

typedef struct {
    uint16_t serial_number;
    uint16_t message_id;
    uint8_t pages;
    uint8_t dcs;
} mal_received_cbm_header_t;

typedef struct {
    mal_received_cbm_header_t header;
    uint8_t useful_data_length;
    uint8_t data[MAL_CBS_PAGE_MAX_LENGTH];
} mal_received_cbm_message_t;

#define MAL_RECEIVED_CBS_ROUTING_EVENT_MESSAGE_DATA_SIZE_MAX 82

/**
 * \struct cbs_routing_ind_t
 * \brief CB SMS data
 *
 *  This structure holds the entire CB message indication
 */
typedef struct {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    uint8_t number_of_messages;
    mal_received_cbm_message_t messages[1];
} mal_received_cbs_routing_event_t;


typedef mal_request_t mal_ind_t;

#define MAL_SMS_SOCK_PATH "/tmp/socket_malsms"

#endif                          // INCLUSION_GUARD_SMS_STUB_H
