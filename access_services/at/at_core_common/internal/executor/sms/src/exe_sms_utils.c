/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "atc_log.h"

#include "exe_sms_utils.h"


void print_sms_error(const char *prefix_string_p, SMS_Error_t sms_error)
{
    ATC_LOG_E("%s %s", prefix_string_p, str_sms_error(sms_error));
}

const char *str_sms_error(SMS_Error_t sms_error)
{
    /* Instead of looking up a string from a table or similar a switch statement is used. It is useful during an interactive debugging session. */
    switch (sms_error) {
    case SMS_ERROR_RP_UNALLOCATED_NUMBER:
        return "SMS_ERROR_RP_UNALLOCATED_NUMBER";
    case SMS_ERROR_RP_OPERATOR_DETERMINED_BARRING:
        return "SMS_ERROR_RP_OPERATOR_DETERMINED_BARRING";
    case SMS_ERROR_RP_CALL_BARRED:
        return "SMS_ERROR_RP_CALL_BARRED";
    case SMS_ERROR_RP_RESERVED:
        return "SMS_ERROR_RP_RESERVED";
    case SMS_ERROR_RP_SMS_TRANSFER_REJECTED:
        return "SMS_ERROR_RP_SMS_TRANSFER_REJECTED";
    case SMS_ERROR_RP_DESTINATION_OUT_OF_ORDER:
        return "SMS_ERROR_RP_DESTINATION_OUT_OF_ORDER";
    case SMS_ERROR_RP_UNIDENTIFIED_SUBSCRIBER:
        return "SMS_ERROR_RP_UNIDENTIFIED_SUBSCRIBER";
    case SMS_ERROR_RP_FACILITY_REJECTED:
        return "SMS_ERROR_RP_FACILITY_REJECTED";
    case SMS_ERROR_RP_UNKNOWN_SUBSCRIBER:
        return "SMS_ERROR_RP_UNKNOWN_SUBSCRIBER";
    case SMS_ERROR_RP_NETWORK_OUT_OF_ORDER:
        return "SMS_ERROR_RP_NETWORK_OUT_OF_ORDER";
    case SMS_ERROR_RP_TEMPORARY_FAILURE:
        return "SMS_ERROR_RP_TEMPORARY_FAILURE";
    case SMS_ERROR_RP_CONGESTION:
        return "SMS_ERROR_RP_CONGESTION";
    case SMS_ERROR_RP_RESOURCES_UNAVAILABLE_UNSPECIFIED:
        return "SMS_ERROR_RP_RESOURCES_UNAVAILABLE_UNSPECIFIED";
    case SMS_ERROR_RP_REQUESTED_FACILITY_NOT_SUBSCRIBED:
        return "SMS_ERROR_RP_REQUESTED_FACILITY_NOT_SUBSCRIBED";
    case SMS_ERROR_RP_REQUESTED_FACILITY_NOT_IMPLEMENTED:
        return "SMS_ERROR_RP_REQUESTED_FACILITY_NOT_IMPLEMENTED";
    case SMS_ERROR_RP_INVALID_SMS_TRANSFER_REFERENCE_VALUE:
        return "SMS_ERROR_RP_INVALID_SMS_TRANSFER_REFERENCE_VALUE";
    case SMS_ERROR_RP_SEMANTICALLY_INCORRECT_MESSAGE:
        return "SMS_ERROR_RP_SEMANTICALLY_INCORRECT_MESSAGE";
    case SMS_ERROR_RP_INVALID_MANDATORY_INFORMATION:
        return "SMS_ERROR_RP_INVALID_MANDATORY_INFORMATION";
    case SMS_ERROR_RP_MESSAGE_TYPE_NON_EXISTENT_OR_NOT_IMPLEMENTED:
        return "SMS_ERROR_RP_MESSAGE_TYPE_NON_EXISTENT_OR_NOT_IMPLEMENTED";
    case SMS_ERROR_RP_MESSAGE_NOT_COMPATIBLE_WITH_SMS_PROTOCOL_STATE:
        return "SMS_ERROR_RP_MESSAGE_NOT_COMPATIBLE_WITH_SMS_PROTOCOL_STATE";
    case SMS_ERROR_RP_IE_NOT_EXISTENT_OR_NOT_IMPLEMENTED:
        return "SMS_ERROR_RP_IE_NOT_EXISTENT_OR_NOT_IMPLEMENTED";
    case SMS_ERROR_RP_INTERWORKING_UNSPECIFIED_PROBLEM:
        return "SMS_ERROR_RP_INTERWORKING_UNSPECIFIED_PROBLEM";
    case SMS_ERROR_RP_PROTOCOL_ERROR_UNSPECIFIED:
        return "SMS_ERROR_RP_PROTOCOL_ERROR_UNSPECIFIED";
    case SMS_ERROR_RP_MEMORY_CAPACITY_EXCEEDED:
        return "SMS_ERROR_RP_MEMORY_CAPACITY_EXCEEDED";
    case SMS_ERROR_CP_NETWORK_FAILURE:
        return "SMS_ERROR_CP_NETWORK_FAILURE";
    case SMS_ERROR_CP_CONGESTION:
        return "SMS_ERROR_CP_CONGESTION";
    case SMS_ERROR_CP_INVALID_TI_VALUE:
        return "SMS_ERROR_CP_INVALID_TI_VALUE";
    case SMS_ERROR_CP_SEMANTICALLY_INCORRECT_MESSAGE:
        return "SMS_ERROR_CP_SEMANTICALLY_INCORRECT_MESSAGE";
    case SMS_ERROR_CP_INVALID_MANDATORY_INFORMATION:
        return "SMS_ERROR_CP_INVALID_MANDATORY_INFORMATION";
    case SMS_ERROR_CP_MESSAGE_TYPE_NON_EXISTENT_OR_NOT_IMPLEMENTED:
        return "SMS_ERROR_CP_MESSAGE_TYPE_NON_EXISTENT_OR_NOT_IMPLEMENTED";
    case SMS_ERROR_CP_MESSAGE_NOT_COMPATIBLE_WITH_SMS_PROTOCOL_STATE:
        return "SMS_ERROR_CP_MESSAGE_NOT_COMPATIBLE_WITH_SMS_PROTOCOL_STATE";
    case SMS_ERROR_CP_IE_NOT_EXISTENT_OR_NOT_IMPLEMENTED:
        return "SMS_ERROR_CP_IE_NOT_EXISTENT_OR_NOT_IMPLEMENTED";
    case SMS_ERROR_CP_PROTOCOL_ERROR_UNSPECIFIED:
        return "SMS_ERROR_CP_PROTOCOL_ERROR_UNSPECIFIED";
    case SMS_ERROR_RMS_INTERN_CONNECTION_ESTABLISHMENT_FAILED:
        return "SMS_ERROR_RMS_INTERN_CONNECTION_ESTABLISHMENT_FAILED";
    case SMS_ERROR_RMS_INTERN_CONCATENATION_TIMER_EXPIRY:
        return "SMS_ERROR_RMS_INTERN_CONCATENATION_TIMER_EXPIRY";
    case SMS_ERROR_RMS_INTERN_RELAY_LAYER_TIMER_EXPIRY:
        return "SMS_ERROR_RMS_INTERN_RELAY_LAYER_TIMER_EXPIRY";
    case SMS_ERROR_RMS_INTERN_CONTROL_LAYER_TIMER_EXPIRY:
        return "SMS_ERROR_RMS_INTERN_CONTROL_LAYER_TIMER_EXPIRY";
    case SMS_ERROR_RMS_INTERN_CONNECTION_LOST:
        return "SMS_ERROR_RMS_INTERN_CONNECTION_LOST";
    case SMS_ERROR_RMS_INTERN_TIMEOUT:
        return "SMS_ERROR_RMS_INTERN_TIMEOUT";
    case SMS_ERROR_RMS_INTERN_RELEASE_FAILURE:
        return "SMS_ERROR_RMS_INTERN_RELEASE_FAILURE";
    case SMS_ERROR_RMS_INTERN_ACCESS_CLASS_BARRED:
        return "SMS_ERROR_RMS_INTERN_ACCESS_CLASS_BARRED";
    case SMS_ERROR_RMS_INTERN_RADIO_PATH_NOT_AVAILABLE:
        return "SMS_ERROR_RMS_INTERN_RADIO_PATH_NOT_AVAILABLE";
    case SMS_ERROR_RMS_INTERN_NO_ERROR_NO_ACK:
        return "SMS_ERROR_RMS_INTERN_NO_ERROR_NO_ACK";
    case SMS_ERROR_TP_FCS_PID_TELE_INT_NOT_SUPPORTED:
        return "SMS_ERROR_TP_FCS_PID_TELE_INT_NOT_SUPPORTED";
    case SMS_ERROR_TP_FCS_PID_SM_TYPE_0_NOT_SUPPORTED:
        return "SMS_ERROR_TP_FCS_PID_SM_TYPE_0_NOT_SUPPORTED";
    case SMS_ERROR_TP_FCS_PID_CANNOT_REPLACE_SM:
        return "SMS_ERROR_TP_FCS_PID_CANNOT_REPLACE_SM";
    case SMS_ERROR_TP_FCS_PID_UNSPECIFIED_ERROR:
        return "SMS_ERROR_TP_FCS_PID_UNSPECIFIED_ERROR";
    case SMS_ERROR_TP_FCS_DCS_CODING_NOT_SUPPORTED:
        return "SMS_ERROR_TP_FCS_DCS_CODING_NOT_SUPPORTED";
    case SMS_ERROR_TP_FCS_DCS_MESSAGE_CLASS_NOT_SUPPORTED:
        return "SMS_ERROR_TP_FCS_DCS_MESSAGE_CLASS_NOT_SUPPORTED";
    case SMS_ERROR_TP_FCS_DCS_UNSPECIFIED_ERROR:
        return "SMS_ERROR_TP_FCS_DCS_UNSPECIFIED_ERROR";
    case SMS_ERROR_TP_FCS_COMMAND_CANNOT_BE_ACTIONED:
        return "SMS_ERROR_TP_FCS_COMMAND_CANNOT_BE_ACTIONED";
    case SMS_ERROR_TP_FCS_COMMAND_UNSUPPORTED:
        return "SMS_ERROR_TP_FCS_COMMAND_UNSUPPORTED";
    case SMS_ERROR_TP_FCS_COMMAND_UNSPECIFIED_ERROR:
        return "SMS_ERROR_TP_FCS_COMMAND_UNSPECIFIED_ERROR";
    case SMS_ERROR_TP_FCS_TPDU_NOT_SUPPORTED:
        return "SMS_ERROR_TP_FCS_TPDU_NOT_SUPPORTED";
    case SMS_ERROR_TP_FCS_SC_BUSY:
        return "SMS_ERROR_TP_FCS_SC_BUSY";
    case SMS_ERROR_TP_FCS_NO_SC_SUBSCRIPTION:
        return "SMS_ERROR_TP_FCS_NO_SC_SUBSCRIPTION";
    case SMS_ERROR_TP_FCS_SC_SYSTEM_FAILURE:
        return "SMS_ERROR_TP_FCS_SC_SYSTEM_FAILURE";
    case SMS_ERROR_TP_FCS_INVALID_SME_ADDRESS:
        return "SMS_ERROR_TP_FCS_INVALID_SME_ADDRESS";
    case SMS_ERROR_TP_FCS_DESTINATION_SME_BARRED:
        return "SMS_ERROR_TP_FCS_DESTINATION_SME_BARRED";
    case SMS_ERROR_TP_FCS_SM_REJECTED_DUPLICATE_SM:
        return "SMS_ERROR_TP_FCS_SM_REJECTED_DUPLICATE_SM";
    case SMS_ERROR_TP_FCS_TP_VPF_NOT_SUPPORTED:
        return "SMS_ERROR_TP_FCS_TP_VPF_NOT_SUPPORTED";
    case SMS_ERROR_TP_FCS_TP_VP_NOT_SUPPORTED:
        return "SMS_ERROR_TP_FCS_TP_VP_NOT_SUPPORTED";
    case SMS_ERROR_TP_FCS_SIM_SMS_STORAGE_FULL:
        return "SMS_ERROR_TP_FCS_SIM_SMS_STORAGE_FULL";
    case SMS_ERROR_TP_FCS_NO_SMS_STORAGE_CAPABILITY_IN_SIM:
        return "SMS_ERROR_TP_FCS_NO_SMS_STORAGE_CAPABILITY_IN_SIM";
    case SMS_ERROR_TP_FCS_ERROR_IN_MS:
        return "SMS_ERROR_TP_FCS_ERROR_IN_MS";
    case SMS_ERROR_TP_FCS_MEMORY_CAPACITY_EXCEEDED:
        return "SMS_ERROR_TP_FCS_MEMORY_CAPACITY_EXCEEDED";
    case SMS_ERROR_TP_FCS_SIM_APPLICATION_TOOLKIT_BUSY:
        return "SMS_ERROR_TP_FCS_SIM_APPLICATION_TOOLKIT_BUSY";
    case SMS_ERROR_TP_FCS_SIM_DATA_DOWNLOAD_ERROR:
        return "SMS_ERROR_TP_FCS_SIM_DATA_DOWNLOAD_ERROR";
    case SMS_ERROR_TP_FCS_APPLICATION_SPECIFIC_FIRST:
        return "SMS_ERROR_TP_FCS_APPLICATION_SPECIFIC_FIRST";
    case SMS_ERROR_TP_FCS_APPLICATION_SPECIFIC_LAST:
        return "SMS_ERROR_TP_FCS_APPLICATION_SPECIFIC_LAST";
    case SMS_ERROR_TP_FCS_UNSPECIFIED_ERROR_CAUSE:
        return "SMS_ERROR_TP_FCS_UNSPECIFIED_ERROR_CAUSE<F10>";
    case SMS_ERROR_INTERN_NO_ERROR:
        return "SMS_ERROR_INTERN_NO_ERROR";
    case SMS_ERROR_INTERN_INVALID_MESSAGE_ID:
        return "SMS_ERROR_INTERN_INVALID_MESSAGE_ID";
    case SMS_ERROR_INTERN_STORAGE_FULL:
        return "SMS_ERROR_INTERN_STORAGE_FULL";
    case SMS_ERROR_INTERN_MESSAGE_PARTIALLY_STORED:
        return "SMS_ERROR_INTERN_MESSAGE_PARTIALLY_STORED";
    case SMS_ERROR_INTERN_MESSAGE_NOT_FOUND:
        return "SMS_ERROR_INTERN_MESSAGE_NOT_FOUND";
    case SMS_ERROR_INTERN_MESSAGE_ALREADY_IN_SIM:
        return "SMS_ERROR_INTERN_MESSAGE_ALREADY_IN_SIM";
    case SMS_ERROR_INTERN_INVALID_PARAMS_FOR_MT:
        return "SMS_ERROR_INTERN_INVALID_PARAMS_FOR_MT";
    case SMS_ERROR_INTERN_INVALID_PARAMS_FOR_MO:
        return "SMS_ERROR_INTERN_INVALID_PARAMS_FOR_MO";
    case SMS_ERROR_INTERN_NO_AIR_INTERFACE:
        return "SMS_ERROR_INTERN_NO_AIR_INTERFACE";
    case SMS_ERROR_INTERN_COMMAND_NOT_UNDERSTOOD:
        return "SMS_ERROR_INTERN_COMMAND_NOT_UNDERSTOOD";
    case SMS_ERROR_INTERN_UNSPECIFIED_ERROR:
        return "SMS_ERROR_INTERN_UNSPECIFIED_ERROR";
    case SMS_ERROR_INTERN_ERROR_MESSAGE_SYSTEM_TIMEOUT:
        return "SMS_ERROR_INTERN_ERROR_MESSAGE_SYSTEM_TIMEOUT";
    case SMS_ERROR_INTERN_CAN_NOT_CODE_MSG_AS_EMS_RELEASE4:
        return "SMS_ERROR_INTERN_CAN_NOT_CODE_MSG_AS_EMS_RELEASE4";
    case SMS_ERROR_INTERN_CALLING_PROCESS_NOT_RECOGNISED:
        return "SMS_ERROR_INTERN_CALLING_PROCESS_NOT_RECOGNISED";
    case SMS_ERROR_INTERN_CLIENT_CONTROL_DATA_MISSING_OR_WRONG:
        return "SMS_ERROR_INTERN_CLIENT_CONTROL_DATA_MISSING_OR_WRONG";
    case SMS_ERROR_INTERN_INVALID_PID_VALUE:
        return "SMS_ERROR_INTERN_INVALID_PID_VALUE";
    case SMS_ERROR_INTERN_MEMORY_ALLOCATION_FAILED:
        return "SMS_ERROR_INTERN_MEMORY_ALLOCATION_FAILED";
    case SMS_ERROR_INTERN_INVALID_WAIT_MODE:
        return "SMS_ERROR_INTERN_INVALID_WAIT_MODE";
    case SMS_ERROR_INTERN_MESSAGE_NOT_USC2:
        return "SMS_ERROR_INTERN_MESSAGE_NOT_USC2";
    case SMS_ERROR_INTERN_FILE_SYSTEM_ACCESS_FAILED:
        return "SMS_ERROR_INTERN_FILE_SYSTEM_ACCESS_FAILED";
    case SMS_ERROR_INTERN_INVALID_BROWSE_OPTION:
        return "SMS_ERROR_INTERN_INVALID_BROWSE_OPTION";
    case SMS_ERROR_INTERN_INVALID_MESSAGE_TYPE:
        return "SMS_ERROR_INTERN_INVALID_MESSAGE_TYPE";
    case SMS_ERROR_INTERN_NO_STATUSREPORT_FOR_SMS_PART:
        return "SMS_ERROR_INTERN_NO_STATUSREPORT_FOR_SMS_PART";
    case SMS_ERROR_INTERN_NO_SMS_FOR_PART_NUMBER:
        return "SMS_ERROR_INTERN_NO_SMS_FOR_PART_NUMBER";
    case SMS_ERROR_INTERN_SERVER_BUSY:
        return "SMS_ERROR_INTERN_SERVER_BUSY";
    case SMS_ERROR_INTERN_INVALID_VOLUME:
        return "SMS_ERROR_INTERN_INVALID_VOLUME";
    case SMS_ERROR_INTERN_APP_PORT_IN_USE:
        return "SMS_ERROR_INTERN_APP_PORT_IN_USE";
    case SMS_ERROR_INTERN_OPERATION_NOT_SUPPORTED_BY_STORAGE:
        return "SMS_ERROR_INTERN_OPERATION_NOT_SUPPORTED_BY_STORAGE";
    case SMS_ERROR_INTERN_INVALID_SIM_SLOT_NUMBER:
        return "SMS_ERROR_INTERN_INVALID_SIM_SLOT_NUMBER";
    case SMS_ERROR_INTERN_NO_ERROR_NO_ACTION:
        return "SMS_ERROR_INTERN_NO_ERROR_NO_ACTION";
    case SMS_ERR_ROUTE_NOT_AVAILABLE:
        return "SMS_ERR_ROUTE_NOT_AVAILABLE";
    case SMS_ERROR_INTERN_FIXED_DIALLING_NUMBER_RESTRICTED:
        return "SMS_ERROR_INTERN_FIXED_DIALLING_NUMBER_RESTRICTED";
#if 0 /* TODO */
    case SMS_ERR_OK:
        return "SMS_ERR_OK";
    case SMS_ERR_ROUTING_RELEASED:
        return "SMS_ERR_ROUTING_RELEASED";
    case SMS_ERR_INVALID_PARAMETER:
        return "SMS_ERR_INVALID_PARAMETER";
    case SMS_ERR_DEVICE_FAILURE:
        return "SMS_ERR_DEVICE_FAILURE";
    case SMS_ERR_PP_RESERVED:
        return "SMS_ERR_PP_RESERVED";
    case SMS_ERR_ROUTE_NOT_ALLOWED:
        return "SMS_ERR_ROUTE_NOT_ALLOWED";
    case SMS_ERR_SERVICE_RESERVED:
        return "SMS_ERR_SERVICE_RESERVED";
    case SMS_ERR_INVALID_LOCATION:
        return "SMS_ERR_INVALID_LOCATION";
    case SMS_ERR_NO_NETW_RESPONSE:
        return "SMS_ERR_NO_NETW_RESPONSE";
    case SMS_ERR_DEST_ADDR_FDN_RESTRICTED:
        return "SMS_ERR_DEST_ADDR_FDN_RESTRICTED";
    case SMS_ERR_SMSC_ADDR_FDN_RESTRICTED:
        return "SMS_ERR_SMSC_ADDR_FDN_RESTRICTED";
    case SMS_ERR_RESEND_ALREADY_DONE:
        return "SMS_ERR_RESEND_ALREADY_DONE";
    case SMS_ERR_SMSC_ADDR_NOT_AVAILABLE:
        return "SMS_ERR_SMSC_ADDR_NOT_AVAILABLE";
    case SMS_ERR_ROUTING_FAILED:
        return "SMS_ERR_ROUTING_FAILED";
    case SMS_ERR_CS_INACTIVE:
        return "SMS_ERR_CS_INACTIVE";
    case SMS_ERR_SAT_MO_CONTROL_MODIFIED:
        return "SMS_ERR_SAT_MO_CONTROL_MODIFIED";
    case SMS_ERR_SAT_MO_CONTROL_REJECT:
        return "SMS_ERR_SAT_MO_CONTROL_REJECT";
    case SMS_ERR_TRACFONE_FAILED:
        return "SMS_ERR_TRACFONE_FAILED";
    case SMS_ERR_SENDING_ONGOING:
        return "SMS_ERR_SENDING_ONGOING";
    case SMS_ERR_SERVER_NOT_READY:
        return "SMS_ERR_SERVER_NOT_READY";
    case SMS_ERR_NO_TRANSACTION:
        return "SMS_ERR_NO_TRANSACTION";
    case SMS_ERR_REJECTED:
        return "SMS_ERR_REJECTED";
    case SMS_ERR_INVALID_SUBSCRIPTION_NR:
        return "SMS_ERR_INVALID_SUBSCRIPTION_NR";
    case SMS_ERR_RECEPTION_FAILED:
        return "SMS_ERR_RECEPTION_FAILED";
    case SMS_ERR_RC_REJECTED:
        return "SMS_ERR_RC_REJECTED";
    case SMS_ERR_ALL_SUBSCRIPTIONS_ALLOCATED:
        return "SMS_ERR_ALL_SUBSCRIPTIONS_ALLOCATED";
    case SMS_ERR_SUBJECT_COUNT_OVERFLOW:
        return "SMS_ERR_SUBJECT_COUNT_OVERFLOW";
    case SMS_ERR_DCS_COUNT_OVERFLOW:
        return "SMS_ERR_DCS_COUNT_OVERFLOW";
    case SMS_EXT_ERR_UNASSIGNED_NUMBER:
        return "SMS_EXT_ERR_UNASSIGNED_NUMBER";
    case SMS_EXT_ERR_OPER_DETERMINED_BARR:
        return "SMS_EXT_ERR_OPER_DETERMINED_BARR";
    case SMS_EXT_ERR_CALL_BARRED:
        return "SMS_EXT_ERR_CALL_BARRED";
    case SMS_EXT_ERR_RESERVED:
        return "SMS_EXT_ERR_RESERVED";
    case SMS_EXT_ERR_MSG_TRANSFER_REJ:
        return "SMS_EXT_ERR_MSG_TRANSFER_REJ";
    case SMS_EXT_ERR_MEMORY_CAPACITY_EXC:
        return "SMS_EXT_ERR_MEMORY_CAPACITY_EXC";
    case SMS_EXT_ERR_DEST_OUT_OF_ORDER:
        return "SMS_EXT_ERR_DEST_OUT_OF_ORDER";
    case SMS_EXT_ERR_UNDEFINED_SUBSCRIBER:
        return "SMS_EXT_ERR_UNDEFINED_SUBSCRIBER";
    case SMS_EXT_ERR_FACILITY_REJECTED:
        return "SMS_EXT_ERR_FACILITY_REJECTED";
    case SMS_EXT_ERR_UNKNOWN_SUBSCRIBER:
        return "SMS_EXT_ERR_UNKNOWN_SUBSCRIBER";
    case SMS_EXT_ERR_NETW_OUT_OF_ORDER:
        return "SMS_EXT_ERR_NETW_OUT_OF_ORDER";
    case SMS_EXT_ERR_TEMPORARY_FAILURE:
        return "SMS_EXT_ERR_TEMPORARY_FAILURE";
    case SMS_EXT_ERR_CONGESTION:
        return "SMS_EXT_ERR_CONGESTION";
    case SMS_EXT_ERR_RESOURCE_UNAVAILABLE:
        return "SMS_EXT_ERR_RESOURCE_UNAVAILABLE";
    case SMS_EXT_ERR_REQ_FACILITY_NOT_SUB:
        return "SMS_EXT_ERR_REQ_FACILITY_NOT_SUB";
    case SMS_EXT_ERR_REQ_FACILITY_NOT_IMP:
        return "SMS_EXT_ERR_REQ_FACILITY_NOT_IMP";
    case SMS_EXT_ERR_INVALID_REFERENCE:
        return "SMS_EXT_ERR_INVALID_REFERENCE";
    case SMS_EXT_ERR_INCORRECT_MESSAGE:
        return "SMS_EXT_ERR_INCORRECT_MESSAGE";
    case SMS_EXT_ERR_INVALID_MAND_INFO:
        return "SMS_EXT_ERR_INVALID_MAND_INFO";
    case SMS_EXT_ERR_INVALID_MSG_TYPE:
        return "SMS_EXT_ERR_INVALID_MSG_TYPE";
    case SMS_EXT_ERR_MSG_NOT_COMP_WITH_ST:
        return "SMS_EXT_ERR_MSG_NOT_COMP_WITH_ST";
    case SMS_EXT_ERR_INVALID_INFO_ELEMENT:
        return "SMS_EXT_ERR_INVALID_INFO_ELEMENT";
    case SMS_EXT_ERR_PROTOCOL_ERROR:
        return "SMS_EXT_ERR_PROTOCOL_ERROR";
    case SMS_EXT_ERR_INTERWORKING:
        return "SMS_EXT_ERR_INTERWORKING";
    case SMS_EXT_ERR_NO_CAUSE:
        return "SMS_EXT_ERR_NO_CAUSE";
    case SMS_EXT_ERR_IMSI_UNKNOWN_HLR:
        return "SMS_EXT_ERR_IMSI_UNKNOWN_HLR";
    case SMS_EXT_ERR_ILLEGAL_MS:
        return "SMS_EXT_ERR_ILLEGAL_MS";
    case SMS_EXT_ERR_IMSI_UNKNOWN_VLR:
        return "SMS_EXT_ERR_IMSI_UNKNOWN_VLR";
    case SMS_EXT_ERR_IMEI_NOT_ACCEPTED:
        return "SMS_EXT_ERR_IMEI_NOT_ACCEPTED";
    case SMS_EXT_ERR_ILLEGAL_ME:
        return "SMS_EXT_ERR_ILLEGAL_ME";
    case SMS_EXT_ERR_PLMN_NOT_ALLOWED:
        return "SMS_EXT_ERR_PLMN_NOT_ALLOWED";
    case SMS_EXT_ERR_LA_NOT_ALLOWED:
        return "SMS_EXT_ERR_LA_NOT_ALLOWED";
    case SMS_EXT_ERR_ROAM_NOT_ALLOWED_LA:
        return "SMS_EXT_ERR_ROAM_NOT_ALLOWED_LA";
    case SMS_EXT_ERR_NO_SUITABLE_CELLS_LA:
        return "SMS_EXT_ERR_NO_SUITABLE_CELLS_LA";
    case SMS_EXT_ERR_MAC_FAILURE:
        return "SMS_EXT_ERR_MAC_FAILURE";
    case SMS_EXT_ERR_SYNC_FAILURE:
        return "SMS_EXT_ERR_SYNC_FAILURE";
    case SMS_EXT_ERR_LOW_LAYER_CONGESTION:
        return "SMS_EXT_ERR_LOW_LAYER_CONGESTION";
    case SMS_EXT_ERR_AUTH_UNACCEPTABLE:
        return "SMS_EXT_ERR_AUTH_UNACCEPTABLE";
    case SMS_EXT_ERR_SERV_OPT_NOT_SUPPORTED:
        return "SMS_EXT_ERR_SERV_OPT_NOT_SUPPORTED";
    case SMS_EXT_ERR_SERV_OPT_NOT_SUBSCRIBED:
        return "SMS_EXT_ERR_SERV_OPT_NOT_SUBSCRIBED";
    case SMS_EXT_ERR_SERV_OPT_NOT_SUBSCRIBED:
        return "SMS_EXT_ERR_SERV_OPT_NOT_SUBSCRIBED";
    case SMS_EXT_ERR_SERV_OPT_TEMP_OUT_OF_ORDER:
        return "SMS_EXT_ERR_SERV_OPT_TEMP_OUT_OF_ORDER";
    case SMS_EXT_ERR_CALL_CANNOT_BE_IDENTIFIED:
        return "SMS_EXT_ERR_CALL_CANNOT_BE_IDENTIFIED";
    case SMS_EXT_ERR_LOW_LAYER_INVALID_MAND_INFO:
        return "SMS_EXT_ERR_LOW_LAYER_INVALID_MAND_INFO";
    case SMS_EXT_ERR_LOW_LAYER_INVALID_MSG_TYPE:
        return "SMS_EXT_ERR_LOW_LAYER_INVALID_MSG_TYPE";
    case SMS_EXT_ERR_LOW_LAYER_MSG_TYPE_NOT_COMP_WITH_ST:
        return "SMS_EXT_ERR_LOW_LAYER_MSG_TYPE_NOT_COMP_WITH_ST";
    case SMS_EXT_ERR_LOW_LAYER_INVALID_INFO_ELEMENT:
        return "SMS_EXT_ERR_LOW_LAYER_INVALID_INFO_ELEMENT";
    case SMS_EXT_ERR_CONDITIONAL_IE_ERROR:
        return "SMS_EXT_ERR_CONDITIONAL_IE_ERROR";
    case SMS_EXT_ERR_LOW_LAYER_MSG_NOT_COMP_WITH_ST:
        return "SMS_EXT_ERR_LOW_LAYER_MSG_NOT_COMP_WITH_ST";
    case SMS_EXT_ERR_CS_BARRED:
        return "SMS_EXT_ERR_CS_BARRED";
    case SMS_EXT_ERR_LOW_LAYER_PROTOCOL_ERROR:
        return "SMS_EXT_ERR_LOW_LAYER_PROTOCOL_ERROR";
#endif /* TODO */
    default:
        return "Unknown SMS error code!";
    }
}

/*
 * This function is used to indicate error reason for a CBS request that fails.
 *
 * @param [in] prefix_string_p   Pointer to string that is added in the beginning of the printout. It is
 *                               intended to contain the name of the calling function.
 *
 * @param [in] cbs_error         The CBS error description that should be printed to the log.
 *
 */
void print_cbs_error(const char *prefix_string_p, CBS_Error_t cbs_error)
{
    ATC_LOG_E("%s %s", prefix_string_p, str_cbs_error(cbs_error));
}

const char *str_cbs_error(CBS_Error_t cbs_error)
{
    switch (cbs_error) {
    case CBS_ERROR_NONE:
        return "CBS_ERROR_NONE";
    case CBS_ERROR_TEMPORARY_FAILURE:
        return "CBS_ERROR_TEMPORARY_FAILURE";
    case CBS_ERROR_MEMORY_ALLOCATION_FAILED:
        return "CBS_ERROR_MEMORY_ALLOCATION_FAILED";
    case CBS_ERROR_TOO_MANY_SUBSCRIBERS:
        return "CBS_ERROR_TOO_MANY_SUBSCRIBERS";
    case CBS_ERROR_ALREADY_SUBSCRIBED:
        return "CBS_ERROR_ALREADY_SUBSCRIBED";
    case CBS_ERROR_SUBSCRIPTION_FAILED:
        return "CBS_ERROR_SUBSCRIPTION_FAILED";
    case CBS_ERROR_INVALID_SUBSCRIPTION:
        return "CBS_ERROR_INVALID_SUBSCRIPTION";
    case CBS_ERROR_CONTROL_DATA_MISSING_OR_WRONG:
        return "CBS_ERROR_CONTROL_DATA_MISSING_OR_WRONG";
    case CBS_ERROR_INVALID_PARAMETER:
        return "CBS_ERROR_INVALID_PARAMETER";
    case CBS_ERROR_REQUESTED_MESSAGE_NO_LONGER_EXISTS:
        return "CBS_ERROR_REQUESTED_MESSAGE_NO_LONGER_EXISTS";
    case CBS_ERROR_RADIO_INACTIVE:
        return "CBS_ERROR_RADIO_INACTIVE";
    case CBS_ERROR_CS_INACTIVE:
        return "CBS_ERROR_CS_INACTIVE";
    case CBS_ERROR_LAST_ERROR_TYPE:
        return "CBS_ERROR_LAST_ERROR_TYPE";
    default:
        return "Unknown CBS error code!";
    }
}

/*
 * This function is used to convert a SMS_Error_t to a MSG_Error.
 *
 *
 * @param [in]  SMS_Error_t      The SMS error code
 *
 * @param [out] CMS_ErrorCodes_t Converted CMS error code
 *
 *
 */
exe_cms_error_t SMS_ErrorToCMS(SMS_Error_t SMS_Error)
{
    switch (SMS_Error) {
    case SMS_ERROR_RP_UNALLOCATED_NUMBER: {
        return EXE_CMS_UNASSIGNED_NUMBER;
    }
    case SMS_ERROR_RP_OPERATOR_DETERMINED_BARRING: {
        return EXE_CMS_OPERATOR_DETERMINED_BARRING;
    }
    case SMS_ERROR_RP_CALL_BARRED: {
        return EXE_CMS_CALL_BARRED;
    }
    case SMS_ERROR_RP_RESERVED: {
        return EXE_CMS_UNKNOWN_ERROR;
    }
    case SMS_ERROR_RP_SMS_TRANSFER_REJECTED: {
        return EXE_CMS_SHORT_MESSAGE_TRANSFER_REJECTED;
    }
    case SMS_ERROR_RP_DESTINATION_OUT_OF_ORDER: {
        return EXE_CMS_DESTINATION_OUT_OF_SERVICE;
    }
    case SMS_ERROR_RP_UNIDENTIFIED_SUBSCRIBER: {
        return EXE_CMS_UNIDENTIFIED_SUBSCRIBER;
    }
    case SMS_ERROR_RP_FACILITY_REJECTED: {
        return EXE_CMS_FACILITY_REJECTED;
    }
    case SMS_ERROR_RP_UNKNOWN_SUBSCRIBER: {
        return EXE_CMS_UNKNOWN_SUBSCRIBER;
    }
    case SMS_ERROR_RP_NETWORK_OUT_OF_ORDER: {
        return EXE_CMS_NETWORK_OUT_OF_ORDER;
    }
    case SMS_ERROR_RP_TEMPORARY_FAILURE: {
        return EXE_CMS_TEMPORARY_FAILURE;
    }
    case SMS_ERROR_RP_CONGESTION: {
        return EXE_CMS_CONGESTION;
    }
    case SMS_ERROR_RP_RESOURCES_UNAVAILABLE_UNSPECIFIED: {
        return EXE_CMS_RESOURCES_UNAVAILABLE_UNSPECIFIED;
    }
    case SMS_ERROR_RP_REQUESTED_FACILITY_NOT_SUBSCRIBED: {
        return EXE_CMS_REQUESTED_FACILITY_NOT_SUBSCRIBED;
    }
    case SMS_ERROR_RP_REQUESTED_FACILITY_NOT_IMPLEMENTED: {
        return EXE_CMS_REQUESTED_FACILITY_NOT_IMPLEMENTED;
    }
    case SMS_ERROR_RP_INVALID_SMS_TRANSFER_REFERENCE_VALUE: {
        return EXE_CMS_INVALID_SHORT_MESSAGE_TRANSFER_REFERENCE_VALUE;
    }
    case SMS_ERROR_RP_SEMANTICALLY_INCORRECT_MESSAGE: {
        return EXE_CMS_INVALID_MESSAGE_UNSPECIFIED;
    }
    case SMS_ERROR_RP_INVALID_MANDATORY_INFORMATION: {
        return EXE_CMS_INVALID_MANDATORY_INFORMATION;
    }
    case SMS_ERROR_RP_MESSAGE_TYPE_NON_EXISTENT_OR_NOT_IMPLEMENTED: {
        return EXE_CMS_MESSAGE_TYPE_NON_EXISTENT_OR_NOT_IMPLEMENTED;
    }
    case SMS_ERROR_RP_MESSAGE_NOT_COMPATIBLE_WITH_SMS_PROTOCOL_STATE: {
        return EXE_CMS_MESSAGE_NOT_COMPATIBLE_WITH_SHORT_MESSAGE_PROTOCOL_STATE;
    }
    case SMS_ERROR_RP_IE_NOT_EXISTENT_OR_NOT_IMPLEMENTED: {
        return EXE_CMS_INFORMATION_ELEMENT_NON_EXISTENT_OR_NOT_IMPLEMENTED;
    }
    case SMS_ERROR_RP_INTERWORKING_UNSPECIFIED_PROBLEM: {
        return EXE_CMS_INTERWORKING_UNSPECIFIED;
    }
    case SMS_ERROR_RP_PROTOCOL_ERROR_UNSPECIFIED: {
        return EXE_CMS_PROTOCOL_ERROR_UNSPECIFIED;
    }
    case SMS_ERROR_RP_MEMORY_CAPACITY_EXCEEDED: {
        return EXE_CMS_MEMORY_FULL;
    }
    case SMS_ERROR_CP_NETWORK_FAILURE: {
        return EXE_CMS_NO_NETWORK_SERVICE;
    }
    case SMS_ERROR_CP_CONGESTION: {
        return EXE_CMS_CONGESTION;
    }
    case SMS_ERROR_CP_INVALID_TI_VALUE: {
        return EXE_CMS_UNKNOWN_ERROR;
    }
    case SMS_ERROR_CP_SEMANTICALLY_INCORRECT_MESSAGE: {
        return EXE_CMS_INVALID_MESSAGE_UNSPECIFIED;
    }
    case SMS_ERROR_CP_INVALID_MANDATORY_INFORMATION: {
        return EXE_CMS_INVALID_MANDATORY_INFORMATION;
    }
    case SMS_ERROR_CP_MESSAGE_TYPE_NON_EXISTENT_OR_NOT_IMPLEMENTED: {
        return EXE_CMS_MESSAGE_TYPE_NON_EXISTENT_OR_NOT_IMPLEMENTED;
    }
    case SMS_ERROR_CP_MESSAGE_NOT_COMPATIBLE_WITH_SMS_PROTOCOL_STATE: {
        return EXE_CMS_MESSAGE_NOT_COMPATIBLE_WITH_SHORT_MESSAGE_PROTOCOL_STATE;
    }
    case SMS_ERROR_CP_IE_NOT_EXISTENT_OR_NOT_IMPLEMENTED: {
        return EXE_CMS_INFORMATION_ELEMENT_NON_EXISTENT_OR_NOT_IMPLEMENTED;
    }
    case SMS_ERROR_CP_PROTOCOL_ERROR_UNSPECIFIED: {
        return EXE_CMS_PROTOCOL_ERROR_UNSPECIFIED;
    }
    case SMS_ERROR_RMS_INTERN_CONNECTION_ESTABLISHMENT_FAILED:
    case SMS_ERROR_RMS_INTERN_CONCATENATION_TIMER_EXPIRY:
    case SMS_ERROR_RMS_INTERN_RELAY_LAYER_TIMER_EXPIRY:
    case SMS_ERROR_RMS_INTERN_CONTROL_LAYER_TIMER_EXPIRY:
    case SMS_ERROR_RMS_INTERN_RADIO_PATH_NOT_AVAILABLE: {
        return EXE_CMS_UNKNOWN_ERROR;
    }
    case SMS_ERROR_TP_FCS_PID_TELE_INT_NOT_SUPPORTED: {
        return EXE_CMS_TELEMATIC_INTERWORKING_NOT_SUPPORTED;
    }
    case SMS_ERROR_TP_FCS_PID_SM_TYPE_0_NOT_SUPPORTED: {
        return EXE_CMS_SHORT_MESSAGE_TYPE_0_NOT_SUPPORTED;
    }
    case SMS_ERROR_TP_FCS_PID_CANNOT_REPLACE_SM: {
        return EXE_CMS_CANNOT_REPLACE_SHORT_MESSAGE;
    }
    case SMS_ERROR_TP_FCS_PID_UNSPECIFIED_ERROR: {
        return EXE_CMS_UNSPECIFIED_TP_PID_ERROR;
    }
    case SMS_ERROR_TP_FCS_DCS_CODING_NOT_SUPPORTED: {
        return EXE_CMS_DATA_CODING_SCHEME_NOT_SUPPORTED;
    }
    case SMS_ERROR_TP_FCS_DCS_MESSAGE_CLASS_NOT_SUPPORTED: {
        return EXE_CMS_MESSAGE_CLASS_NOT_SUPPORTED;
    }
    case SMS_ERROR_TP_FCS_DCS_UNSPECIFIED_ERROR: {
        return EXE_CMS_UNSPECIFIED_TP_DCS_ERROR;
    }
    case SMS_ERROR_TP_FCS_COMMAND_CANNOT_BE_ACTIONED: {
        return EXE_CMS_COMMAND_CANNOT_BE_ACTIONED;
    }
    case SMS_ERROR_TP_FCS_COMMAND_UNSUPPORTED: {
        return EXE_CMS_COMMAND_UNSUPPORTED;
    }
    case SMS_ERROR_TP_FCS_COMMAND_UNSPECIFIED_ERROR: {
        return EXE_CMS_UNSPECIFIED_TP_COMMAND_ERROR;
    }
    case SMS_ERROR_TP_FCS_TPDU_NOT_SUPPORTED: {
        return EXE_CMS_TPDU_NOT_SUPPORTED;
    }
    case SMS_ERROR_TP_FCS_SC_BUSY: {
        return EXE_CMS_SC_BUSY;
    }
    case SMS_ERROR_TP_FCS_NO_SC_SUBSCRIPTION: {
        return EXE_CMS_NO_SC_SUBSCRIPTION;
    }
    case SMS_ERROR_TP_FCS_SC_SYSTEM_FAILURE: {
        return EXE_CMS_SC_SYSTEM_FAILURE;
    }
    case SMS_ERROR_TP_FCS_INVALID_SME_ADDRESS: {
        return EXE_CMS_INVALID_SME_ADDRESS;
    }
    case SMS_ERROR_TP_FCS_DESTINATION_SME_BARRED: {
        return EXE_CMS_DESTINATION_SME_BARRED;
    }
    case SMS_ERROR_TP_FCS_SM_REJECTED_DUPLICATE_SM: {
        return EXE_CMS_SM_REJECTED_DUPLICATE_SM;
    }
    case SMS_ERROR_TP_FCS_TP_VPF_NOT_SUPPORTED:
    case SMS_ERROR_TP_FCS_TP_VP_NOT_SUPPORTED: {
        return EXE_CMS_OPERATION_NOT_SUPPORTED;
    }
    case SMS_ERROR_TP_FCS_SIM_SMS_STORAGE_FULL: {
        return EXE_CMS_SIM_SMS_STORAGE_FULL;
    }
    case SMS_ERROR_TP_FCS_NO_SMS_STORAGE_CAPABILITY_IN_SIM: {
        return EXE_CMS_NO_SMS_STORAGE_CAPABILITY_IN_SIM;
    }
    case SMS_ERROR_TP_FCS_ERROR_IN_MS: {
        return EXE_CMS_ERROR_IN_MS;
    }
    case SMS_ERROR_TP_FCS_MEMORY_CAPACITY_EXCEEDED: {
        return EXE_CMS_MEMORY_CAPACITY_EXCEEDED;
    }
    case SMS_ERROR_TP_FCS_SIM_APPLICATION_TOOLKIT_BUSY: {
        return EXE_CMS_SIM_BUSY;
    }
    case SMS_ERROR_TP_FCS_SIM_DATA_DOWNLOAD_ERROR: {
        return EXE_CMS_SIM_FAILURE;
    }
    case SMS_ERROR_TP_FCS_UNSPECIFIED_ERROR_CAUSE: {
        return EXE_CMS_UNSPECIFIED_ERROR_CAUSE;
    }
    case SMS_ERROR_INTERN_NO_ERROR: {
        return EXE_CMS_OK;
    }
    case SMS_ERROR_INTERN_INVALID_MESSAGE_ID: {
        return EXE_CMS_INVALID_MEMORY_INDEX;
    }
    case SMS_ERROR_INTERN_STORAGE_FULL: {
        return EXE_CMS_MEMORY_FULL;
    }
    case SMS_ERROR_INTERN_MESSAGE_PARTIALLY_STORED:
    case SMS_ERROR_INTERN_MESSAGE_NOT_FOUND:
    case SMS_ERROR_INTERN_MESSAGE_ALREADY_IN_SIM:
    case SMS_ERROR_INTERN_INVALID_PARAMS_FOR_MT:
    case SMS_ERROR_INTERN_INVALID_PARAMS_FOR_MO:
    case SMS_ERROR_INTERN_NO_AIR_INTERFACE:
    case SMS_ERROR_INTERN_COMMAND_NOT_UNDERSTOOD: {
        return EXE_CMS_OPERATION_NOT_SUPPORTED;
    }
    case SMS_ERROR_INTERN_UNSPECIFIED_ERROR: {
        return EXE_CMS_UNKNOWN_ERROR;
    }
    case SMS_ERROR_INTERN_ERROR_MESSAGE_SYSTEM_TIMEOUT:
    case SMS_ERROR_INTERN_CAN_NOT_CODE_MSG_AS_EMS_RELEASE4: {
        return EXE_CMS_OPERATION_NOT_SUPPORTED;
    }
    case SMS_ERROR_INTERN_CALLING_PROCESS_NOT_RECOGNISED:
    case SMS_ERROR_INTERN_CLIENT_CONTROL_DATA_MISSING_OR_WRONG:
    case SMS_ERROR_INTERN_INVALID_PID_VALUE:
    case SMS_ERROR_INTERN_MEMORY_ALLOCATION_FAILED: {
        return EXE_CMS_MEMORY_FAILURE;
    }
    case SMS_ERROR_INTERN_MO_SMS_CONTROL_BY_USIM_REJECTED: {
        return EXE_CMS_OPERATION_NOT_ALLOWED;
    }
    case SMS_ERR_ROUTE_NOT_AVAILABLE: {
        return EXE_CMS_TEMPORARY_FAILURE;
    }
    case SMS_ERROR_INTERN_INVALID_WAIT_MODE:
    case SMS_ERROR_INTERN_MESSAGE_NOT_USC2:
    case SMS_ERROR_INTERN_FILE_SYSTEM_ACCESS_FAILED:
    case SMS_ERROR_INTERN_INVALID_BROWSE_OPTION:
    case SMS_ERROR_INTERN_INVALID_MESSAGE_TYPE: {
        return EXE_CMS_UNKNOWN_ERROR;
    }
    case SMS_ERROR_INTERN_FIXED_DIALLING_NUMBER_RESTRICTED:
        return EXE_CMS_FIXED_DIALLING_NUMBER_CHECK_FAILURE;
    default: {
        return EXE_CMS_UNKNOWN_ERROR;
    }
    }
}

uint8_t smsutil_sms_to_pdu(SMS_SMSC_Address_TPDU_t *sms_address_p, uint8_t *pdu_p)
{
    uint8_t index = 0;
    /* Initialize PDU buffer */
    memset(pdu_p, 0x0, sizeof(SMS_SMSC_Address_TPDU_t));

    if (sms_address_p->ServiceCenterAddress.Length > 0) {
        /* Add one to the Service Center Address length to include the TON/NPI byte */
        /* TON = TypeOfNumber, NPI = NumberingPlanId */
        sms_address_p->ServiceCenterAddress.Length++;
        /* Copy service center address including length field, if length == 0 only copy length byte */
        memmove(pdu_p, &(sms_address_p->ServiceCenterAddress),
                sms_address_p->ServiceCenterAddress.Length + 1);
        /* Advance pdu_p buffer pointer the number of copied bytes */
        index += (sms_address_p->ServiceCenterAddress.Length
                  + 1);
    } else {
        *pdu_p = 0x0;
        index++;
    }

    /* Copy TPDU field */
    memcpy(pdu_p + index, &(sms_address_p->TPDU.Data),
           sms_address_p->TPDU.Length);
    index += (sms_address_p->TPDU.Length);
    return index;
}
