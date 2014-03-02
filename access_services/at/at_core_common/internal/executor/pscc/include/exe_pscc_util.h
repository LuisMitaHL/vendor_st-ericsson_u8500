/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef EXE_PSCC_UTIL_H_
#define EXE_PSCC_UTIL_H_

#include "exe_request_record.h"
#include "exe_pscc_iadb.h"
#include "exe_pscc.h"

typedef enum { /* 3GPP error codes */
    ERROR_CAUSE_OPERATOR_DETERMINED_BARRING = 8,
    ERROR_CAUSE_LLC_OR_SNDCP_FAILURE = 25,
    ERROR_CAUSE_INSUFFICIENT_RESOURCES = 26,
    ERROR_CAUSE_MISSING_OR_UNKNOWN_APN = 27,
    ERROR_CAUSE_UNKNOWN_PDP_ADDRESS_OR_PDP_TYPE = 28,
    ERROR_CAUSE_USER_AUTHENTICATION_FAILED = 29,
    ERROR_CAUSE_ACTIVATION_REJECTED_BY_GGSN = 30,
    ERROR_CAUSE_ACTIVATION_REJECTED_UNSPECIFIED = 31,
    ERROR_CAUSE_SERVICE_OPTION_NOT_SUPPORTED = 32,
    ERROR_CAUSE_REQUESTED_SERVICE_OPTION_NOT_SUBSCRIBED = 33,
    ERROR_CAUSE_SERVICE_OPTION_TEMPORARILY_OUT_OF_ORDER = 34,
    ERROR_CAUSE_NSAPI_ALREADY_USED = 35,
    ERROR_CAUSE_REGULAR_DEACTIVATION = 36,
    ERROR_CAUSE_QOS_NOT_ACCEPTED = 37,
    ERROR_CAUSE_NETWORK_FAILURE = 38,
    ERROR_CAUSE_REACTIVATION_REQUIRED = 39,
    ERROR_CAUSE_FEATURE_NOT_SUPPORTED = 40,
    ERROR_CAUSE_SEMANTIC_ERROR_IN_THE_TFT_OPERATION = 41,
    ERROR_CAUSE_SYNTACTICAL_ERROR_IN_THE_TFT_OPERATION = 42,
    ERROR_CAUSE_UNKNOWN_PDP_CONTEXT = 43,
    ERROR_CAUSE_PDP_CONTEXT_WITHOUT_TFT_ALREADY_ACTIVATED = 46,
    ERROR_CAUSE_SEMANTIC_ERRORS_IN_PACKET_FILTER = 44,
    ERROR_CAUSE_SYNTATICAL_ERRORS_IN_PACKET_FILTER = 45,
    ERROR_CAUSE_INVALID_TRANSACTION_IDENTIFIER_VALUE = 81,
    ERROR_CAUSE_SEMANTICALLY_INCORRECT_MESSAGE = 95,
    ERROR_CAUSE_INVALID_MANDATORY_INFORMATION = 96,
    ERROR_CAUSE_MESSAGE_TYPE_NON_EXISTENT_OR_NOT_IMPLEMENTED = 97,
    ERROR_CAUSE_MESSAGE_TYPE_NOT_COMPATIBLE_WITH_THE_PROTOCOL_STATE = 98,
    ERROR_CAUSE_INFORMATION_ELEMENT_NON_EXISTENT_OR_NOT_IMPLEMENTED = 99,
    ERROR_CAUSE_CONDITIONAL_INFORMATION_ELEMENT_ERROR = 100,
    ERROR_CAUSE_MESSAGE_NOT_COMPATIBLE_WITH_THE_PROTOCOL_STATE = 101,
    ERROR_CAUSE_PROTOCOL_ERROR_UNSPECIFIED = 111,
    ERROR_CAUSE_APN_RESTRICTION_VALUE_INCOMPATIBLE_WITH_ACTIVE_PDP_CONTEXT = 112
} error_cause_t;

/* Global variable */
extern int s_last_data_call_fail_cause;


/********************************************************************************************************************************************************************
 * UTILITY FUNCTIONS                                                                                                                                                *
 ********************************************************************************************************************************************************************/


exe_pscc_result_t exe_pscc_send_destroy_request(exe_request_record_t *record_p);
bool create_and_init_psccclient_record_data(exe_request_record_t *record_p, exe_pscc_action_t action, exe_cgact_async_mode_t async_mode);

bool get_response_parameter_from_mpl_msg(mpl_msg_resp_t *response_p, mpl_param_element_id_t param_id, char **str_pp);

void psccclient_print_pscc_response_error(const char *prefix_string_p, pscc_result_t result);
int translate_pscc_disconnect_cause_code_to_cause(int cause);


/*
 * Conversion methods that will translate between the format used by the AT commands (TS 27.007)
 * and the PSCC interface (described by TS 24.008)
 */
pscc_qos_traffic_class_t convert_AT_to_PSCC_traffic_class(int traffic_class_AT);
int convert_PSCC_to_AT_traffic_class(pscc_qos_traffic_class_t traffic_class_PSCC);
void convert_AT_to_PSCC_bitrate(int bitrate_AT, uint8_t *base, uint8_t *extended);
int convert_PSCC_to_AT_bitrate(uint8_t base, uint8_t extended);
pscc_qos_delivery_order_t convert_AT_to_PSCC_SDU_delivery_order(int delivery_order);
int convert_PSCC_to_AT_SDU_delivery_order(pscc_qos_delivery_order_t delivery_order);
uint8_t convert_AT_to_PSCC_max_SDU_size(int size);
int convert_PSCC_to_AT_max_SDU_size(uint8_t size);
pscc_qos_sdu_error_ratio_t convert_AT_to_PSCC_SDU_error_ratio(char *string_SDU_err);
void convert_PSCC_to_AT_SDU_error_ratio(pscc_qos_sdu_error_ratio_t value, char *convertedValue);
pscc_qos_residual_ber_t convert_AT_to_PSCC_residual_BER(char *string_BER);
void convert_PSCC_to_AT_residual_BER(pscc_qos_residual_ber_t value, char *convertedValue);
pscc_qos_delivery_erroneous_sdu_t convert_AT_to_PSCC_delivery_of_erroneous_SDU(int atval);
int convert_PSCC_to_AT_delivery_of_erroneous_SDU(pscc_qos_delivery_erroneous_sdu_t psccval);
pscc_qos_traffic_handling_priority_t convert_AT_to_PSCC_traffic_handling_priority(int atprio);
int convert_PSCC_to_AT_traffic_handling_priority(pscc_qos_traffic_handling_priority_t psccprio);
uint8_t convert_AT_to_PSCC_transfer_delay(int at_delay);
int convert_PSCC_to_AT_transfer_delay(uint8_t pscc_delay);
pscc_qos_source_statistics_descriptor_t convert_AT_to_PSCC_source_statistics_descriptor(int atssd);
int convert_PSCC_to_AT_source_statistics_descriptor(pscc_qos_source_statistics_descriptor_t psccssd);
pscc_qos_signalling_indication_t convert_to_AT_to_PSCC_signalling_indication(int atsigind);
int convert_PSCC_to_AT_signalling_indication(pscc_qos_signalling_indication_t psccsigind);
pscc_qos_mean_throughput_class_t convert_AT_to_PSCC_mean_throughput(int mtp);
bool add_qos_parameters_to_pscc_message(int cid, pscc_msg_t *request_p);
bool add_2G_qos_parameters_to_pscc_message(int cid, pscc_msg_t *request_p);
exe_ppp_auth_prot_t auth_mode_pscc2generic(pscc_auth_method_t pscc_mode);
void free_pscc_connection_response(exe_pscc_connection_response_t *response_data_p);
void exe_pscc_send_fail_cause_event(int32_t error_code);
#endif /* EXE_PSCC_UTIL_H_ */
