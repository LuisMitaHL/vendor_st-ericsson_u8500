/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <string.h>

#include "exe_pscc_client.h"
#include "exe_pscc_iadb.h"
#include "exe_internal.h"
#include "exe_last_fail_cause.h"
#include "exe_pscc_requests.h"
#include "exe_pscc_util.h"
#include "exe_request_record_p.h"

#ifdef EXE_USE_ATC_CUSTOMER_EXTENSIONS
#include "exe_extended_pscc_util.h"
#endif

#include "mpl_config.h" /* libmpl */

/********************************************************************************
 *
 * Defines & Type definitions
 *
 ********************************************************************************/
/* add defines here */

/* Defines for the bitrate conversions */
#define cutoff_1_kbps ((uint8_t)0x40)      /* 0b01000000 */
#define cutoff_8_kbps ((uint8_t)0x80)      /* 0b10000000 */
#define cutoff_64_kbps ((uint8_t)0xfe)     /* 0b11111110 */
#define cutoff_1_Mbps ((uint8_t)0x4a)      /* 0b01001010 */
#define cutoff_2_Mbps ((uint8_t)0xba)      /* 0b10111010 */
#define maxLimit ((uint8_t)0xfa)           /* 0b11111010 */

/* Mbps to kbps conversion ration is 1000, not 1024. */
#define Mbps ((uint16_t)1000)

#define base_octet_offset_64 ((uint16_t)64)
#define base_octet_offset_576 ((uint16_t)576)
#define extended_octet_offset_8600 ((uint16_t)8600)
#define extended_octet_offset_16 ((uint16_t)16)
#define extended_octet_offset_128 ((uint16_t)128)
#define extended_octet_maxval ((uint16_t)256)

#define minimum_speed ((uint16_t)1)

/* Defines for SDU error conversions */
#define sdu_mpl_1502 ((uint8_t)0x97)       /* 0b10010111 */
#define sdu_mpl_1510 ((uint8_t)0x98)       /* 0b10011000 */
#define sdu_mpl_1520 ((uint8_t)0x99)       /* 0b10011001 */

/* Defines for transfer delay conversions */
#define cutoff_td_150 ((uint8_t)0x10)      /* 0b10000 */
#define cutoff_td_950 ((uint8_t)0x20)      /* 0b100000 */
#define cutoff_td_max ((uint8_t)0x3e)      /* 0b111110 */
#define offset_td_200 ((uint16_t)200)
#define offset_td_1000 ((uint16_t)1000)



/********************************************************************
 * Public methods
 ********************************************************************
 */
exe_pscc_result_t exe_pscc_send_destroy_request(exe_request_record_t *record_p)
{
    exe_pscc_result_t   result = EXE_PSCC_FAILURE;
    pscc_msg_t          *request_p = NULL;

    request_p = mpl_req_msg_alloc(PSCC_PARAM_SET_ID);
    EXE_CHECK_GOTO_ERROR(NULL != request_p);

    request_p->req_destroy.id = pscc_destroy;
    EXE_CHECK_GOTO_ERROR(psccclient_send_request(record_p, request_p));

    result = EXE_PSCC_SUCCESS;

error:
    mpl_msg_free((mpl_msg_t *)request_p);
    return result;
}

bool create_and_init_psccclient_record_data(exe_request_record_t *record_p, exe_pscc_action_t action, exe_cgact_async_mode_t async_mode)
{
    psccclient_record_data_t *client_data_p = NULL;
    psccclient_t             *psccclient_p  = NULL;
    bool                      result        = false;

    /* for unsolicited event handling we don't have exe available, but have psccclient from previous activity */
    if (EXE_PSCC_ACTION_UNSOLICITED != action) {
        if (NULL == record_p->exe_p) {
            ATC_LOG_E("exe pointer in request record is NULL. Failed");
            goto exit;
        }

        psccclient_p = exe_get_psccclient(record_p->exe_p);

        if (NULL == psccclient_p) {
            ATC_LOG_E("psccclient_p is NULL. Failed.");
            goto exit;
        }
    }

    client_data_p = (psccclient_record_data_t *)record_p->client_data_p;

    /* TODO: do we need to check for old data? */
    if (NULL != client_data_p) {
        ATC_LOG_I("client_data is not NULL on entry. Freeing old data.");
        free(client_data_p);
    }

    client_data_p = (psccclient_record_data_t *)calloc(1, sizeof(psccclient_record_data_t));

    if (NULL == client_data_p) {
        ATC_LOG_E("client_data is NULL. Failed to allocate memory!!!");
        goto exit;
    }

    client_data_p->psccclient_p     = psccclient_p;
    client_data_p->action           = action;
    client_data_p->async_mode       = async_mode;

    record_p->client_data_p = client_data_p;

    result = true;

exit:
    return result;
}


void psccclient_print_pscc_response_error(const char *prefix_string_p, pscc_result_t result)
{
    switch (result) {
    case pscc_result_ok:/* Not an error */
        break;
    case pscc_result_failed_operation_not_allowed:
        ATC_LOG_E("%s pscc_result_failed_operation_not_allowed", prefix_string_p);
        break;
    case pscc_result_failed_object_exists:
        ATC_LOG_E("%s pscc_result_failed_object_exists", prefix_string_p);
        break;
    case pscc_result_failed_object_not_found:
        ATC_LOG_E("%s pscc_result_failed_object_not_found", prefix_string_p);
        break;
    case pscc_result_failed_parameter_not_found:
        ATC_LOG_E("%s pscc_result_failed_parameter_not_found", prefix_string_p);
        break;
    case pscc_result_failed_unspecified:
        ATC_LOG_E("%s pscc_result_failed_unspecified", prefix_string_p);
        break;
    case pscc_number_of_results:
        ATC_LOG_E("%s pscc_result_failed_unspecified (illegal error code)", prefix_string_p);
        break;
    default:
        ATC_LOG_E("%s Unknown pscc result code!", prefix_string_p);
        break;
    }
}


bool get_response_parameter_from_mpl_msg(mpl_msg_resp_t *response_p, mpl_param_element_id_t param_id, char **str_pp)
{
    bool result = false;

    mpl_param_element_t *param_element_p = mpl_param_list_find(param_id, response_p->param_list_p);

    if (NULL != *str_pp) {
        goto exit;
    }

    if (NULL == param_element_p) {
        goto exit;
    }

    if (NULL == param_element_p->value_p) {
        result = true; /* the value string is allowed to be NULL, but does then not require any copying */
        goto exit;
    }

    *str_pp = (char *)malloc(1 + strlen((char *)(param_element_p->value_p)));

    if (NULL == *str_pp) {
        goto exit;
    }

    strcpy(*str_pp, (char *)(param_element_p->value_p));

    result = true;

exit:
    return result;
}

int translate_pscc_disconnect_cause_code_to_cause(int cause)
{
    int return_value = 0;

    switch (cause) {
        /* 3GPP error codes: return the cause code untouched */
    case ERROR_CAUSE_OPERATOR_DETERMINED_BARRING:
    case ERROR_CAUSE_LLC_OR_SNDCP_FAILURE:
    case ERROR_CAUSE_INSUFFICIENT_RESOURCES:
    case ERROR_CAUSE_MISSING_OR_UNKNOWN_APN:
    case ERROR_CAUSE_UNKNOWN_PDP_ADDRESS_OR_PDP_TYPE:
    case ERROR_CAUSE_USER_AUTHENTICATION_FAILED:
    case ERROR_CAUSE_ACTIVATION_REJECTED_BY_GGSN:
    case ERROR_CAUSE_ACTIVATION_REJECTED_UNSPECIFIED:
    case ERROR_CAUSE_SERVICE_OPTION_NOT_SUPPORTED:
    case ERROR_CAUSE_REQUESTED_SERVICE_OPTION_NOT_SUBSCRIBED:
    case ERROR_CAUSE_SERVICE_OPTION_TEMPORARILY_OUT_OF_ORDER:
    case ERROR_CAUSE_NSAPI_ALREADY_USED:
    case ERROR_CAUSE_REGULAR_DEACTIVATION:
    case ERROR_CAUSE_QOS_NOT_ACCEPTED:
    case ERROR_CAUSE_NETWORK_FAILURE:
    case ERROR_CAUSE_REACTIVATION_REQUIRED:
    case ERROR_CAUSE_FEATURE_NOT_SUPPORTED:
    case ERROR_CAUSE_SEMANTIC_ERROR_IN_THE_TFT_OPERATION:
    case ERROR_CAUSE_SYNTACTICAL_ERROR_IN_THE_TFT_OPERATION:
    case ERROR_CAUSE_UNKNOWN_PDP_CONTEXT:
    case ERROR_CAUSE_PDP_CONTEXT_WITHOUT_TFT_ALREADY_ACTIVATED:
    case ERROR_CAUSE_SEMANTIC_ERRORS_IN_PACKET_FILTER:
    case ERROR_CAUSE_SYNTATICAL_ERRORS_IN_PACKET_FILTER:
    case ERROR_CAUSE_INVALID_TRANSACTION_IDENTIFIER_VALUE:
    case ERROR_CAUSE_SEMANTICALLY_INCORRECT_MESSAGE:
    case ERROR_CAUSE_INVALID_MANDATORY_INFORMATION:
    case ERROR_CAUSE_MESSAGE_TYPE_NON_EXISTENT_OR_NOT_IMPLEMENTED:
    case ERROR_CAUSE_MESSAGE_TYPE_NOT_COMPATIBLE_WITH_THE_PROTOCOL_STATE:
    case ERROR_CAUSE_INFORMATION_ELEMENT_NON_EXISTENT_OR_NOT_IMPLEMENTED:
    case ERROR_CAUSE_CONDITIONAL_INFORMATION_ELEMENT_ERROR:
    case ERROR_CAUSE_MESSAGE_NOT_COMPATIBLE_WITH_THE_PROTOCOL_STATE:
    case ERROR_CAUSE_PROTOCOL_ERROR_UNSPECIFIED:
    case ERROR_CAUSE_APN_RESTRICTION_VALUE_INCOMPATIBLE_WITH_ACTIVE_PDP_CONTEXT:
        return_value = cause;
        break;
    default:
        return_value = 0 /*TODO: check this:PDP_FAIL_ERROR_UNSPECIFIED*/;
        break;
    }

    return return_value;
}


pscc_qos_traffic_class_t convert_AT_to_PSCC_traffic_class(int traffic_class_AT)
{
    if (4 == traffic_class_AT) {
        return pscc_qos_TRAFFIC_CLASS_UNSPECIFIED;
    }

    if (3 >= traffic_class_AT && 0 <= traffic_class_AT) {
        return (pscc_qos_traffic_class_t)traffic_class_AT + 1;
    }

    ATC_LOG_E("Input out of bounds. Returning default value.");
    return pscc_qos_TRAFFIC_CLASS_UNSPECIFIED;
}

int convert_PSCC_to_AT_traffic_class(pscc_qos_traffic_class_t traffic_class_PSCC)
{
    return traffic_class_PSCC - 1;
}

int convert_PSCC_to_AT_bitrate(uint8_t base, uint8_t extended)
{
    int returnvalue = 0;

    if (base < cutoff_1_kbps) {
        returnvalue = base;
    } else if (base < cutoff_8_kbps) {
        returnvalue = ((base - cutoff_1_kbps) * 8 + base_octet_offset_64);
    } else if (base <= cutoff_64_kbps) {
        returnvalue = ((base - cutoff_8_kbps) * 64 + base_octet_offset_576);

        if (base == cutoff_64_kbps) {
            /* handle extended values: */
            if (extended == 0) {
                /* extended byte = 0 means ignore it */
            } else if (extended <= cutoff_1_Mbps) {
                returnvalue = (extended_octet_offset_8600 + extended * 100); /* 100 is a constant from 3GPP spec 24.008 */
            } else if (extended <= cutoff_2_Mbps) {
                returnvalue = ((extended_octet_offset_16 + extended - cutoff_1_Mbps) * Mbps);
            } else if (extended <= maxLimit) {
                returnvalue = ((extended_octet_offset_128 + (extended - cutoff_2_Mbps) * 2) * Mbps);
            } else {
                returnvalue = (extended_octet_maxval * Mbps); /* 256 is a constant from 3GPP spec 24.008 (maximum value) */
            }
        }
    } else {
        returnvalue = minimum_speed; /* base byte 0xff means speed = 0, which is illegal for mal */
    }

    return returnvalue;
}


void convert_AT_to_PSCC_bitrate(int bitrate_AT, uint8_t *base, uint8_t *extended)
{
    if (bitrate_AT >= 8700) {
        /* 8700 is a constant from 3GPP spec 24.008 (lower limit for first interval with extended octet)
         bitrate_AT is beyond the base encoding. Must use extended octet */
        *base = cutoff_64_kbps;

        if (bitrate_AT > 256 * Mbps) {
            ATC_LOG_E("AT bitrate input out of bounds. Returning default values.");
            *extended = 0;
            *base = 0;
        } else if (bitrate_AT > extended_octet_offset_128 * Mbps) {
            *extended = bitrate_AT / (2 * Mbps) - 64 + cutoff_2_Mbps;
        } else if (bitrate_AT > extended_octet_offset_16 * Mbps) {
            *extended = bitrate_AT / Mbps - extended_octet_offset_16 + cutoff_1_Mbps;
        } else {
            *extended = (bitrate_AT - extended_octet_offset_8600) / 100;
        }
    } else {
        *extended = 0;

        if (bitrate_AT >= base_octet_offset_576) {
            *base = (bitrate_AT - base_octet_offset_576) / 64 + cutoff_8_kbps;
        } else if (bitrate_AT >= base_octet_offset_64) {
            *base = (bitrate_AT - base_octet_offset_64) / 8 + cutoff_1_kbps;
        } else if (bitrate_AT > 0) {
            *base = bitrate_AT;
        } else { /* NB: A value of "0" means default, i.e. 0. There is no "0kbps", as minimum is 1kbps */
            *base = 0;
        }
    }
}

pscc_qos_delivery_order_t convert_AT_to_PSCC_SDU_delivery_order(int delivery_order)
{
    pscc_qos_delivery_order_t returnval = pscc_qos_DELIVERY_ORDER_UNSPECIFIED;

    switch (delivery_order) {
    case 0:
        returnval = pscc_qos_DELIVERY_ORDER_NO;
        break;
    case 1:
        returnval = pscc_qos_DELIVERY_ORDER_YES;
        break;
    case 2:
        break;
    default:
        ATC_LOG_E("AT SDU delivery order input out of bounds. Returning default value.");
        break;
    }

    return returnval;
}

int convert_PSCC_to_AT_SDU_delivery_order(pscc_qos_delivery_order_t delivery_order)
{
    if (pscc_qos_DELIVERY_ORDER_NO == delivery_order) {
        return 0;
    } else {
        return 1;
    }
}

uint8_t convert_AT_to_PSCC_max_SDU_size(int size)
{
    uint8_t returnvalue = 0;

    if (1520 < size) {
        ATC_LOG_E("AT max SDU size input out of bounds. Returning default value.");
    } else if (size == 1520) {
        returnvalue = sdu_mpl_1520;
    } else if (size == 1510) {
        returnvalue = sdu_mpl_1510;
    } else if (size == 1502) {
        returnvalue = sdu_mpl_1502;
    } else if (size <= 1500) {
        returnvalue = size / 10;
    }

    return returnvalue;
}

int convert_PSCC_to_AT_max_SDU_size(uint8_t size)
{
    int returnvalue = 0;

    if (size < sdu_mpl_1502) {
        returnvalue = size * 10;
    } else if (size == sdu_mpl_1502) {
        returnvalue = 1502;
    } else if (size == sdu_mpl_1510) {
        returnvalue = 1510;
    } else if (size == sdu_mpl_1520) {
        returnvalue = 1520;
    } else {
        ATC_LOG_E("PSCC max SDU size input out of bounds. Returning default value.");
        returnvalue = 1520;
    }

    return returnvalue;
}

pscc_qos_sdu_error_ratio_t convert_AT_to_PSCC_SDU_error_ratio(char *string_SDU_err)
{
    pscc_qos_sdu_error_ratio_t returnvalue = pscc_qos_SDU_ERROR_RATIO_UNSPECIFIED;

    if (string_SDU_err != NULL && '0' < string_SDU_err[0] && 'E' == string_SDU_err[1] && '0' < string_SDU_err[2]) {

        uint8_t mantissa = string_SDU_err[0] - '0';
        uint8_t exponent = string_SDU_err[2] - '0';

        if (1 == mantissa) {
            switch (exponent) {
            case 2:
                returnvalue = pscc_qos_SDU_ERROR_RATIO_1_E_NEG2;
                break;
            case 3:
                returnvalue = pscc_qos_SDU_ERROR_RATIO_1_E_NEG3;
                break;
            case 4:
                returnvalue = pscc_qos_SDU_ERROR_RATIO_1_E_NEG4;
                break;
            case 5:
                returnvalue =  pscc_qos_SDU_ERROR_RATIO_1_E_NEG5;
                break;
            case 6:
                returnvalue = pscc_qos_SDU_ERROR_RATIO_1_E_NEG6;
                break;
            case 1:
                returnvalue = pscc_qos_SDU_ERROR_RATIO_1_E_NEG1;
                break;
            default:
                break;
            }
        } else if (7 == mantissa && 3 == exponent) {
            returnvalue = pscc_qos_SDU_ERROR_RATIO_7_E_NEG3;
        }
    }

    return returnvalue;
}

void convert_PSCC_to_AT_SDU_error_ratio(pscc_qos_sdu_error_ratio_t value, char *convertedValue)
{
    if (NULL != convertedValue) {
        convertedValue[0] = '1';
        convertedValue[1] = 'E';
        convertedValue[2] = '1';

        switch (value) {
        case pscc_qos_SDU_ERROR_RATIO_1_E_NEG2:
            convertedValue[2] = '2';
            break;
        case pscc_qos_SDU_ERROR_RATIO_7_E_NEG3:
            convertedValue[0] = '7';
            convertedValue[2] = '3';
            break;
        case pscc_qos_SDU_ERROR_RATIO_1_E_NEG3:
            convertedValue[2] = '3';
            break;
        case pscc_qos_SDU_ERROR_RATIO_1_E_NEG4:
            convertedValue[2] = '4';
            break;
        case pscc_qos_SDU_ERROR_RATIO_1_E_NEG5:
            convertedValue[2] = '5';
            break;
        case pscc_qos_SDU_ERROR_RATIO_1_E_NEG6:
            convertedValue[2] = '6';
            break;
        case pscc_qos_SDU_ERROR_RATIO_1_E_NEG1:
            break;
        default:
            /* This should not be an alternative since pscc should never return value UNSPECIFIED. However, if
             * this method is used for other purposes than returning parameters from PSCC this is valid. */
            ATC_LOG_E("PSCC SDU error ratio input out of bounds. Returning default value.");
            convertedValue[0] = '0';
            convertedValue[2] = '0';
            break;
        }
    }
}

pscc_qos_residual_ber_t convert_AT_to_PSCC_residual_BER(char *string_BER)
{
    pscc_qos_residual_ber_t returnvalue = pscc_qos_RESIDUAL_BER_UNSPECIFIED;

    if (string_BER != NULL && '0' < string_BER[0] && 'E' == string_BER[1] && '0' < string_BER[2]) {

        uint8_t mantissa = string_BER[0] - '0';
        uint8_t exponent = string_BER[2] - '0';

        switch (mantissa) {
        case 1:

            switch (exponent) {
            case 2:
                returnvalue = pscc_qos_RESIDUAL_BER_1_E_NEG2;
                break;
            case 3:
                returnvalue = pscc_qos_RESIDUAL_BER_1_E_NEG3;
                break;
            case 4:
                returnvalue = pscc_qos_RESIDUAL_BER_1_E_NEG4;
                break;
            case 5:
                returnvalue =  pscc_qos_RESIDUAL_BER_1_E_NEG5;
                break;
            case 6:
                returnvalue = pscc_qos_RESIDUAL_BER_1_E_NEG6;
                break;
            default:
                break;
            }

            break;
        case 4:

            if (3 == exponent) {
                returnvalue = pscc_qos_RESIDUAL_BER_4_E_NEG3;
            }

            break;
        case 5:

            if (2 == exponent) {
                returnvalue = pscc_qos_RESIDUAL_BER_5_E_NEG2;
            } else if (3 == exponent) {
                returnvalue = pscc_qos_RESIDUAL_BER_5_E_NEG3;
            }

            break;
        case 6:

            if (8 == exponent) {
                returnvalue = pscc_qos_RESIDUAL_BER_6_E_NEG8;
            }

            break;
        default:
            break;
        }
    }

    return returnvalue;
}

void convert_PSCC_to_AT_residual_BER(pscc_qos_residual_ber_t value, char *convertedValue)
{
    if (NULL != convertedValue) {
        convertedValue[0] = '1';
        convertedValue[1] = 'E';
        convertedValue[2] = '3';

        switch (value) {
        case pscc_qos_RESIDUAL_BER_5_E_NEG2:
            convertedValue[0] = '5';
            convertedValue[2] = '2';
            break;
        case pscc_qos_RESIDUAL_BER_1_E_NEG2:
            convertedValue[0] = '1';
            convertedValue[2] = '2';
            break;
        case pscc_qos_RESIDUAL_BER_5_E_NEG3:
            convertedValue[0] = '5';
            convertedValue[2] = '3';
            break;
        case pscc_qos_RESIDUAL_BER_4_E_NEG3:
            convertedValue[0] = '4';
            convertedValue[2] = '3';
            break;
        case pscc_qos_RESIDUAL_BER_1_E_NEG3:
            convertedValue[0] = '1';
            convertedValue[2] = '3';
            break;
        case pscc_qos_RESIDUAL_BER_1_E_NEG4:
            convertedValue[0] = '1';
            convertedValue[2] = '4';
            break;
        case pscc_qos_RESIDUAL_BER_1_E_NEG5:
            convertedValue[0] = '1';
            convertedValue[2] = '5';
            break;
        case pscc_qos_RESIDUAL_BER_1_E_NEG6:
            convertedValue[0] = '1';
            convertedValue[2] = '6';
            break;
        case pscc_qos_RESIDUAL_BER_6_E_NEG8:
            convertedValue[0] = '6';
            convertedValue[2] = '8';
            break;
        default:
            /* This should not be an alternative since pscc should never return value UNSPECIFIED. However, if
             * this method is used for other purposes than returning parameters from PSCC this is valid. */
            ATC_LOG_E("PSCC SDU error ratio input out of bounds. Returning default value.");
            convertedValue[0] = '0';
            convertedValue[2] = '0';
            break;
        }
    }
}

pscc_qos_delivery_erroneous_sdu_t convert_AT_to_PSCC_delivery_of_erroneous_SDU(int atval)
{
    pscc_qos_delivery_erroneous_sdu_t retval = pscc_qos_DELIVERY_ERRONEOUS_SDU_UNSPECIFIED;

    switch (atval) {
    case 0:
        retval = pscc_qos_DELIVERY_ERRONEOUS_SDU_NO;
        break;
    case 1:
        retval = pscc_qos_DELIVERY_ERRONEOUS_SDU_YES;
        break;
    case 2:
        retval = pscc_qos_DELIVERY_ERRONEOUS_SDU_NO_DETECT;
        break;
    case 3:
        retval = pscc_qos_DELIVERY_ERRONEOUS_SDU_UNSPECIFIED;
        break;
    default:
        ATC_LOG_E("AT deliver of erroneous SDU input out of bounds (val = %d). Returning default value.", atval);
        break;
    }

    return retval;
}

int convert_PSCC_to_AT_delivery_of_erroneous_SDU(pscc_qos_delivery_erroneous_sdu_t psccval)
{
    int retval = 2;

    switch (psccval) {
    case pscc_qos_DELIVERY_ERRONEOUS_SDU_NO_DETECT:
        retval = 2;
        break;
    case pscc_qos_DELIVERY_ERRONEOUS_SDU_YES:
        retval = 1;
        break;
    case pscc_qos_DELIVERY_ERRONEOUS_SDU_NO:
        retval = 0;
        break;
    default:
        ATC_LOG_E("PSCC deliver of erroneous SDU input out of bounds (val = %d). Returning default value.", psccval);
        break;
    }

    return retval;
}

uint8_t convert_AT_to_PSCC_transfer_delay(int at_delay)
{
    uint8_t returnvalue = 0;

    if (at_delay > 4000) {
        returnvalue = cutoff_td_max;
        ATC_LOG_E("AT transfer delay out of bounds (val=%d). Returning default.", at_delay);
    } else if (at_delay >= offset_td_1000) {
        returnvalue = (at_delay - offset_td_1000) / 100 + cutoff_td_950;
    } else if (at_delay >= offset_td_200) {
        returnvalue = (at_delay - offset_td_200) / 50 + cutoff_td_150;
    } else {
        returnvalue = at_delay / 10;
    }

    return returnvalue;
}

int convert_PSCC_to_AT_transfer_delay(uint8_t pscc_delay)
{
    int returnvalue = 0;

    if (pscc_delay < cutoff_td_150) {
        returnvalue = pscc_delay * 10;
    } else if (pscc_delay < cutoff_td_950) {
        returnvalue = ((pscc_delay - cutoff_td_150) * 50 + offset_td_200);
    } else if (pscc_delay <= cutoff_td_max) {
        returnvalue = ((pscc_delay - cutoff_td_950) * 100 + offset_td_1000);
    } else {
        ATC_LOG_E("PSCC transfer delay out of bounds (val=%d). Returning default.", pscc_delay);
    }

    return returnvalue;
}

pscc_qos_traffic_handling_priority_t convert_AT_to_PSCC_traffic_handling_priority(int atprio)
{
    if (pscc_qos_TRAFFIC_HANDLING_PRIORITY_LEVEL_3 >= atprio && 0 <= atprio) {
        return (pscc_qos_traffic_handling_priority_t)atprio;
    } else {
        ATC_LOG_E("AT traffic handling priority out of bounds (val=%d). Returning default.", atprio);
        return pscc_qos_TRAFFIC_HANDLING_PRIORITY_LEVEL_3;
    }
}

int convert_PSCC_to_AT_traffic_handling_priority(pscc_qos_traffic_handling_priority_t psccprio)
{
    return (int)psccprio;
}

pscc_qos_source_statistics_descriptor_t convert_AT_to_PSCC_source_statistics_descriptor(int atssd)
{
    if (1 == atssd) {
        return pscc_qos_SOURCE_STATISTICS_DESCRIPTOR_SPEECH;
    } else {
        return pscc_qos_SOURCE_STATISTICS_DESCRIPTOR_UNKNOWN;
    }
}

int convert_PSCC_to_AT_source_statistics_descriptor(pscc_qos_source_statistics_descriptor_t psccssd)
{
    return (int)psccssd;
}

pscc_qos_signalling_indication_t convert_AT_to_PSCC_signalling_indication(int atsigind)
{
    if (1 == atsigind) {
        return pscc_qos_SIGNALLING_INDICATION_OPTIMIZED_FOR_SIGNALLING_TRAFFIC;
    } else {
        return pscc_qos_SIGNALLING_INDICATION_NOT_OPTIMIZED_FOR_SIGNALLING_TRAFFIC;
    }
}

int convert_PSCC_to_AT_signalling_indication(pscc_qos_signalling_indication_t psccsigind)
{
    return (int) psccsigind;
}

/*
 * Note: PSCC uses a special representation of Mean Throughput Class where the value of Best effort is not 31.
 * Apart from that the entire Rel-97 interface complies to TS24.008 (QoS spec) and TS27.007 (AT spec)
 *
 * No need for conversion of pscc value to AT value, since that is never read back from pscc.
 */
pscc_qos_mean_throughput_class_t convert_AT_to_PSCC_mean_throughput(int mtp)
{
    if (mtp == 31) {
        return pscc_qos_MEAN_THROUGHPUT_CLASS_BEST_EFFORT;
    } else {
        return (pscc_qos_mean_throughput_class_t)mtp;
    }
}

/*
 * Method reads up all QoS rel-97/2G parameters from the iadb, converts them to proper pscc format and and adds them
 * to the mpl message. Takes both minimum and requested values.
 */
bool add_2G_qos_parameters_to_pscc_message(int cid, pscc_msg_t *request_p)
{
    int tmp = 0;

    /* Add flag that 2G/rel-97 parameters should be used */
    pscc_qos_type_t qosType =  pscc_qos_type_rel97;

    if (mpl_add_param_to_list(&request_p->req_set.param_list_p, pscc_paramid_qos_type, &qosType) < 0) {
        ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_qos_type)");
        return false;
    }

    /* Add minimum QoS parameters */
    if (psccclient_iadb_get(cid, psccclient_iadb_field_min_precedence, &tmp)) {
        if (mpl_add_param_to_list(&request_p->req_set.param_list_p, pscc_paramid_min_qos_precedence_class, &tmp) < 0) {
            ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_min_qos_precedence_class)");
            return false;
        }
    }

    if (psccclient_iadb_get(cid, psccclient_iadb_field_min_delay, &tmp)) {
        if (mpl_add_param_to_list(&request_p->req_set.param_list_p, pscc_paramid_min_qos_delay_class, &tmp) < 0) {
            ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_min_qos_delay_class)");
            return false;
        }
    }

    if (psccclient_iadb_get(cid, psccclient_iadb_field_min_reliability, &tmp)) {
        if (mpl_add_param_to_list(&request_p->req_set.param_list_p, pscc_paramid_min_qos_reliability_class, &tmp) < 0) {
            ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_min_qos_reliability_class)");
            return false;
        }
    }

    if (psccclient_iadb_get(cid, psccclient_iadb_field_min_peak, &tmp)) {
        if (mpl_add_param_to_list(&request_p->req_set.param_list_p, pscc_paramid_min_qos_peak_throughput_class, &tmp) < 0) {
            ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_min_qos_peak_throughput_class)");
            return false;
        }
    }

    if (psccclient_iadb_get(cid, psccclient_iadb_field_min_mean, &tmp)) {
        pscc_qos_mean_throughput_class_t pscc_mean_tp = convert_AT_to_PSCC_mean_throughput(tmp);

        if (mpl_add_param_to_list(&request_p->req_set.param_list_p, pscc_paramid_min_qos_mean_throughput_class, &pscc_mean_tp) < 0) {
            ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_min_qos_mean_throughput_class)");
            return false;
        }
    }




    /* Add requested  QoS parameters */
    if (psccclient_iadb_get(cid, psccclient_iadb_field_req_precedence, &tmp)) {
        if (mpl_add_param_to_list(&request_p->req_set.param_list_p, pscc_paramid_req_qos_precedence_class, &tmp) < 0) {
            ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_req_qos_precedence_class)");
            return false;
        }
    }

    if (psccclient_iadb_get(cid, psccclient_iadb_field_req_delay, &tmp)) {
        if (mpl_add_param_to_list(&request_p->req_set.param_list_p, pscc_paramid_req_qos_delay_class, &tmp) < 0) {
            ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_req_qos_delay_class)");
            return false;
        }
    }

    if (psccclient_iadb_get(cid, psccclient_iadb_field_req_reliability, &tmp)) {
        if (mpl_add_param_to_list(&request_p->req_set.param_list_p, pscc_paramid_req_qos_reliability_class, &tmp) < 0) {
            ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_req_qos_reliability_class)");
            return false;
        }
    }

    if (psccclient_iadb_get(cid, psccclient_iadb_field_req_peak, &tmp)) {
        if (mpl_add_param_to_list(&request_p->req_set.param_list_p, pscc_paramid_req_qos_peak_throughput_class, &tmp) < 0) {
            ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_req_qos_peak_throughput_class)");
            return false;
        }
    }

    if (psccclient_iadb_get(cid, psccclient_iadb_field_req_mean, &tmp)) {
        pscc_qos_mean_throughput_class_t pscc_mean_tp = convert_AT_to_PSCC_mean_throughput(tmp);

        if (mpl_add_param_to_list(&request_p->req_set.param_list_p, pscc_paramid_req_qos_mean_throughput_class, &pscc_mean_tp) < 0) {
            ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_req_qos_mean_throughput_class)");
            return false;
        }
    }

    return true;
}

/*
 * Method reads up all QoS rel-99/3G parameters from the iadb, converts them to proper pscc format and and adds them
 * to the mpl message. Takes both minimum and requested values.
 * */
bool add_qos_parameters_to_pscc_message(int cid, pscc_msg_t *request_p)
{
    int                  traffic_class;
    unsigned long        max_bitrate_ul;
    unsigned long        max_bitrate_dl;
    unsigned long        guaranteed_bitrate_ul;
    unsigned long        guaranteed_bitrate_dl;
    int                  delivery_order;
    unsigned long        max_sdu_size;
    char                *sdu_error_ratio_p = NULL;
    char                *residual_ber_p    = NULL;
    int                  delivery_err_sdu;
    unsigned long        transfer_delay;
    int                  traffic_handling_priority;
    int                  source_statistics_descriptor;
    int                  signalling_indication;

    /* Add flag that 3G/rel-99 parameters should be used */
    pscc_qos_type_t qosType =  pscc_qos_type_rel99;

    if (mpl_add_param_to_list(&request_p->req_set.param_list_p, pscc_paramid_qos_type, &qosType) < 0) {
        ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_qos_type)");
        return false;
    }

    /* Add minimum QoS parameters */
    if (psccclient_iadb_get(cid, psccclient_iadb_field_min_traffic_class, &traffic_class)) {
        pscc_qos_traffic_class_t val = convert_AT_to_PSCC_traffic_class(traffic_class);

        if (mpl_add_param_to_list(&request_p->req_set.param_list_p, pscc_paramid_min_qos_traffic_class, &val) < 0) {
            ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_min_qos_traffic_class)");
            return false;
        }
    }

    if (psccclient_iadb_get(cid, psccclient_iadb_field_min_max_bitrate_ul, &max_bitrate_ul)) {
        uint8_t base = 0;
        uint8_t extended = 0;
        convert_AT_to_PSCC_bitrate(max_bitrate_ul, &base, &extended);

        if (mpl_add_param_to_list(&request_p->req_set.param_list_p, pscc_paramid_min_qos_max_bitrate_uplink, &base) < 0) {
            ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_min_qos_max_bitrate_uplink)");
            return false;
        }

        if (mpl_add_param_to_list(&request_p->req_set.param_list_p, pscc_paramid_min_qos_extended_max_bitrate_uplink, &extended) < 0) {
            ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_min_qos_extended_max_bitrate_uplink)");
            return false;
        }
    }

    if (psccclient_iadb_get(cid, psccclient_iadb_field_min_max_bitrate_dl, &max_bitrate_dl)) {
        uint8_t base = 0;
        uint8_t extended = 0;
        convert_AT_to_PSCC_bitrate(max_bitrate_dl, &base, &extended);

        if (mpl_add_param_to_list(&request_p->req_set.param_list_p, pscc_paramid_min_qos_max_bitrate_downlink, &base) < 0) {
            ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_min_qos_max_bitrate_downlink)");
            return false;
        }

        if (mpl_add_param_to_list(&request_p->req_set.param_list_p, pscc_paramid_min_qos_extended_max_bitrate_downlink, &extended) < 0) {
            ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_min_qos_extended_max_bitrate_downlink)");
            return false;
        }
    }

    if (psccclient_iadb_get(cid, psccclient_iadb_field_min_guaranteed_bitrate_ul, &guaranteed_bitrate_ul)) {
        uint8_t base = 0;
        uint8_t extended = 0;
        convert_AT_to_PSCC_bitrate(guaranteed_bitrate_ul, &base, &extended);

        if (mpl_add_param_to_list(&request_p->req_set.param_list_p, pscc_paramid_min_qos_guaranteed_bitrate_uplink, &base) < 0) {
            ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_min_qos_guaranteed_bitrate_uplink)");
            return false;
        }

        if (mpl_add_param_to_list(&request_p->req_set.param_list_p, pscc_paramid_min_qos_extended_guaranteed_bitrate_uplink, &extended) < 0) {
            ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_min_qos_extended_guaranteed_bitrate_uplink)");
            return false;
        }
    }

    if (psccclient_iadb_get(cid, psccclient_iadb_field_min_guaranteed_bitrate_dl, &guaranteed_bitrate_dl)) {
        uint8_t base = 0;
        uint8_t extended = 0;
        convert_AT_to_PSCC_bitrate(guaranteed_bitrate_dl, &base, &extended);

        if (mpl_add_param_to_list(&request_p->req_set.param_list_p, pscc_paramid_min_qos_guaranteed_bitrate_downlink, &base) < 0) {
            ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_min_qos_guaranteed_bitrate_downlink)");
            return false;
        }

        if (mpl_add_param_to_list(&request_p->req_set.param_list_p, pscc_paramid_min_qos_extended_guaranteed_bitrate_downlink, &extended) < 0) {
            ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_min_qos_guaranteed_bitrate_downlink)");
            return false;
        }
    }

    if (psccclient_iadb_get(cid, psccclient_iadb_field_min_delivery_order, &delivery_order)) {
        pscc_qos_delivery_order_t pscc_do = convert_AT_to_PSCC_SDU_delivery_order(delivery_order);

        if (mpl_add_param_to_list(&request_p->req_set.param_list_p, pscc_paramid_min_qos_delivery_order, &pscc_do) < 0) {
            ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_min_qos_delivery_order)");
            return false;
        }
    }

    if (psccclient_iadb_get(cid, psccclient_iadb_field_min_max_sdu_size, &max_sdu_size)) {
        uint8_t pscc_msdu = convert_AT_to_PSCC_max_SDU_size(max_sdu_size);

        if (mpl_add_param_to_list(&request_p->req_set.param_list_p, pscc_paramid_min_qos_max_sdu_size, &pscc_msdu) < 0) {
            ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_min_qos_max_sdu_size)");
            return false;
        }
    }

    if (psccclient_iadb_get(cid, psccclient_iadb_field_min_sdu_error_ratio_p, &sdu_error_ratio_p)) {
        pscc_qos_sdu_error_ratio_t pscc_sdu_err = convert_AT_to_PSCC_SDU_error_ratio(sdu_error_ratio_p);

        if (mpl_add_param_to_list(&request_p->req_set.param_list_p, pscc_paramid_min_qos_sdu_error_ratio, &pscc_sdu_err) < 0) {
            ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_min_qos_sdu_error_ratio_p)");
            return false;
        }
    }

    if (psccclient_iadb_get(cid, psccclient_iadb_field_min_residual_ber_p, &residual_ber_p)) {
        pscc_qos_residual_ber_t pscc_resber = convert_AT_to_PSCC_residual_BER(residual_ber_p);

        if (mpl_add_param_to_list(&request_p->req_set.param_list_p, pscc_paramid_min_qos_residual_ber, &pscc_resber) < 0) {
            ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_min_qos_residual_ber_p)");
            return false;
        }
    }

    if (psccclient_iadb_get(cid, psccclient_iadb_field_min_delivery_err_sdu, &delivery_err_sdu)) {
        pscc_qos_delivery_erroneous_sdu_t pscc_del_err_sdu = convert_AT_to_PSCC_delivery_of_erroneous_SDU(delivery_err_sdu);

        if (mpl_add_param_to_list(&request_p->req_set.param_list_p, pscc_paramid_min_qos_delivery_erroneous_sdu, &pscc_del_err_sdu) < 0) {
            ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_min_qos_delivery_erroneous_sdu)");
            return false;
        }
    }

    if (psccclient_iadb_get(cid, psccclient_iadb_field_min_transfer_delay, &transfer_delay)) {
        uint8_t pscc_td = convert_AT_to_PSCC_transfer_delay(transfer_delay);

        if (mpl_add_param_to_list(&request_p->req_set.param_list_p, pscc_paramid_min_qos_transfer_delay, &pscc_td) < 0) {
            ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_min_qos_transfer_delay)");
            return false;
        }
    }

    if (psccclient_iadb_get(cid, psccclient_iadb_field_min_traffic_handling_priority, &traffic_handling_priority)) {
        pscc_qos_traffic_handling_priority_t pscc_thprio = convert_PSCC_to_AT_traffic_handling_priority(traffic_handling_priority);

        if (mpl_add_param_to_list(&request_p->req_set.param_list_p, pscc_paramid_min_qos_traffic_handling_priority, &pscc_thprio) < 0) {
            ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_min_qos_traffic_handling_priority)");
            return false;
        }
    }

    if (psccclient_iadb_get(cid, psccclient_iadb_field_min_source_statistics_descriptor, &source_statistics_descriptor)) {
        pscc_qos_source_statistics_descriptor_t pscc_ssd = convert_PSCC_to_AT_source_statistics_descriptor(source_statistics_descriptor);

        if (mpl_add_param_to_list(&request_p->req_set.param_list_p, pscc_paramid_min_qos_source_statistics_descriptor, &pscc_ssd) < 0) {
            ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_min_qos_source_statistics_descriptor)");
            return false;
        }
    }

    if (psccclient_iadb_get(cid, psccclient_iadb_field_min_signalling_indication, &signalling_indication)) {
        pscc_qos_signalling_indication_t pscc_si = convert_PSCC_to_AT_signalling_indication(signalling_indication);

        if (mpl_add_param_to_list(&request_p->req_set.param_list_p, pscc_paramid_min_qos_signalling_indication, &pscc_si) < 0) {
            ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_min_qos_signalling_indication)");
            return false;
        }
    }

    /* Add requested QoS parameters */
    if (psccclient_iadb_get(cid, psccclient_iadb_field_req_traffic_class, &traffic_class)) {
        pscc_qos_traffic_class_t pscc_tc = convert_AT_to_PSCC_traffic_class(traffic_class);

        if (mpl_add_param_to_list(&request_p->req_set.param_list_p, pscc_paramid_req_qos_traffic_class, &pscc_tc) < 0) {
            ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_req_qos_traffic_class)");
            return false;
        }
    }

    if (psccclient_iadb_get(cid, psccclient_iadb_field_req_max_bitrate_ul, &max_bitrate_ul)) {
        uint8_t base = 0;
        uint8_t extended = 0;
        convert_AT_to_PSCC_bitrate(max_bitrate_ul, &base, &extended);

        if (mpl_add_param_to_list(&request_p->req_set.param_list_p, pscc_paramid_req_qos_max_bitrate_uplink, &base) < 0) {
            ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_req_qos_max_bitrate_uplink)");
            return false;
        }

        if (mpl_add_param_to_list(&request_p->req_set.param_list_p, pscc_paramid_req_qos_extended_max_bitrate_uplink, &extended) < 0) {
            ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_req_qos_extended_max_bitrate_uplink)");
            return false;
        }
    }

    if (psccclient_iadb_get(cid, psccclient_iadb_field_req_max_bitrate_dl, &max_bitrate_dl)) {
        uint8_t base = 0;
        uint8_t extended = 0;
        convert_AT_to_PSCC_bitrate(max_bitrate_dl, &base, &extended);

        if (mpl_add_param_to_list(&request_p->req_set.param_list_p, pscc_paramid_req_qos_max_bitrate_downlink, &base) < 0) {
            ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_req_qos_max_bitrate_downlink)");
            return false;
        }

        if (mpl_add_param_to_list(&request_p->req_set.param_list_p, pscc_paramid_req_qos_extended_max_bitrate_downlink, &extended) < 0) {
            ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_req_qos_extended_max_bitrate_downlink)");
            return false;
        }
    }

    if (psccclient_iadb_get(cid, psccclient_iadb_field_req_guaranteed_bitrate_ul, &guaranteed_bitrate_ul)) {
        uint8_t base = 0;
        uint8_t extended = 0;
        convert_AT_to_PSCC_bitrate(guaranteed_bitrate_ul, &base, &extended);

        if (mpl_add_param_to_list(&request_p->req_set.param_list_p, pscc_paramid_req_qos_guaranteed_bitrate_uplink, &base) < 0) {
            ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_req_qos_guaranteed_bitrate_uplink)");
            return false;
        }

        if (mpl_add_param_to_list(&request_p->req_set.param_list_p, pscc_paramid_req_qos_extended_guaranteed_bitrate_uplink, &extended) < 0) {
            ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_req_qos_extended_guaranteed_bitrate_uplink)");
            return false;
        }
    }

    if (psccclient_iadb_get(cid, psccclient_iadb_field_req_guaranteed_bitrate_dl, &guaranteed_bitrate_dl)) {
        uint8_t base = 0;
        uint8_t extended = 0;
        convert_AT_to_PSCC_bitrate(guaranteed_bitrate_dl, &base, &extended);

        if (mpl_add_param_to_list(&request_p->req_set.param_list_p, pscc_paramid_req_qos_guaranteed_bitrate_downlink, &base) < 0) {
            ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_req_qos_guaranteed_bitrate_downlink)");
            return false;
        }

        if (mpl_add_param_to_list(&request_p->req_set.param_list_p, pscc_paramid_req_qos_extended_guaranteed_bitrate_downlink, &extended) < 0) {
            ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_req_qos_extended_guaranteed_bitrate_downlink)");
            return false;
        }
    }

    if (psccclient_iadb_get(cid, psccclient_iadb_field_req_delivery_order, &delivery_order)) {
        pscc_qos_delivery_order_t pscc_do = convert_AT_to_PSCC_SDU_delivery_order(delivery_order);

        if (mpl_add_param_to_list(&request_p->req_set.param_list_p, pscc_paramid_req_qos_delivery_order, &pscc_do) < 0) {
            ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_req_qos_delivery_order)");
            return false;
        }
    }

    if (psccclient_iadb_get(cid, psccclient_iadb_field_req_max_sdu_size, &max_sdu_size)) {
        uint8_t pscc_msdu = convert_AT_to_PSCC_max_SDU_size(max_sdu_size);

        if (mpl_add_param_to_list(&request_p->req_set.param_list_p, pscc_paramid_req_qos_max_sdu_size, &pscc_msdu) < 0) {
            ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_req_qos_max_sdu_size)");
            return false;
        }
    }

    if (psccclient_iadb_get(cid, psccclient_iadb_field_req_sdu_error_ratio_p, &sdu_error_ratio_p)) {
        pscc_qos_sdu_error_ratio_t pscc_sdu_err = convert_AT_to_PSCC_SDU_error_ratio(sdu_error_ratio_p);

        if (mpl_add_param_to_list(&request_p->req_set.param_list_p, pscc_paramid_req_qos_sdu_error_ratio, &pscc_sdu_err) < 0) {
            ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_req_qos_sdu_error_ratio_p)");
            return false;
        }
    }

    if (psccclient_iadb_get(cid, psccclient_iadb_field_req_residual_ber_p, &residual_ber_p)) {
        pscc_qos_residual_ber_t pscc_resber = convert_AT_to_PSCC_residual_BER(residual_ber_p);

        if (mpl_add_param_to_list(&request_p->req_set.param_list_p, pscc_paramid_req_qos_residual_ber, &pscc_resber) < 0) {
            ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_req_qos_residual_ber_p)");
            return false;
        }
    }

    if (psccclient_iadb_get(cid, psccclient_iadb_field_req_delivery_err_sdu, &delivery_err_sdu)) {
        pscc_qos_delivery_erroneous_sdu_t pscc_del_err_sdu = convert_AT_to_PSCC_delivery_of_erroneous_SDU(delivery_err_sdu);

        if (mpl_add_param_to_list(&request_p->req_set.param_list_p, pscc_paramid_req_qos_delivery_erroneous_sdu, &pscc_del_err_sdu) < 0) {
            ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_req_qos_delivery_erroneous_sdu)");
            return false;
        }
    }

    if (psccclient_iadb_get(cid, psccclient_iadb_field_req_transfer_delay, &transfer_delay)) {
        uint8_t pscc_td = convert_AT_to_PSCC_transfer_delay(transfer_delay);

        if (mpl_add_param_to_list(&request_p->req_set.param_list_p, pscc_paramid_req_qos_transfer_delay, &pscc_td) < 0) {
            ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_req_qos_transfer_delay)");
            return false;
        }
    }

    if (psccclient_iadb_get(cid, psccclient_iadb_field_req_traffic_handling_priority, &traffic_handling_priority)) {
        pscc_qos_traffic_handling_priority_t pscc_thprio = convert_PSCC_to_AT_traffic_handling_priority(traffic_handling_priority);

        if (mpl_add_param_to_list(&request_p->req_set.param_list_p, pscc_paramid_req_qos_traffic_handling_priority, &pscc_thprio) < 0) {
            ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_req_qos_traffic_handling_priority)");
            return false;
        }
    }

    if (psccclient_iadb_get(cid, psccclient_iadb_field_req_source_statistics_descriptor, &source_statistics_descriptor)) {
        pscc_qos_source_statistics_descriptor_t pscc_ssd = convert_PSCC_to_AT_source_statistics_descriptor(source_statistics_descriptor);

        if (mpl_add_param_to_list(&request_p->req_set.param_list_p, pscc_paramid_req_qos_source_statistics_descriptor, &pscc_ssd) < 0) {
            ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_req_qos_source_statistics_descriptor)");
            return false;
        }
    }

    if (psccclient_iadb_get(cid, psccclient_iadb_field_req_signalling_indication, &signalling_indication)) {
        pscc_qos_signalling_indication_t pscc_si = convert_PSCC_to_AT_signalling_indication(signalling_indication);

        if (mpl_add_param_to_list(&request_p->req_set.param_list_p, pscc_paramid_req_qos_signalling_indication, &pscc_si) < 0) {
            ATC_LOG_E("mpl_add_param_to_list failed! (pscc_paramid_req_qos_signalling_indication)");
            return false;
        }
    }

    return true;
}

exe_ppp_auth_prot_t auth_mode_pscc2generic(pscc_auth_method_t pscc_mode)
{
    switch (pscc_mode) {
    case pscc_auth_method_none:
        return PPP_AUTH_PROTOCOL_NONE;
        break;
    case pscc_auth_method_pap:
        return PPP_AUTH_PROTOCOL_PAP;
        break;
    case pscc_auth_method_chap:
        return PPP_AUTH_PROTOCOL_CHAP;
        break;
    default:
        ATC_LOG_E("Received invalid auth mode Will return PPP_AUTH_PROTOCOL_NONE");
        return PPP_AUTH_PROTOCOL_NONE;
    }
}

void free_pscc_connection_response(exe_pscc_connection_response_t *response_data_p)
{
    if (NULL == response_data_p) {
        return;
    }

    free(response_data_p->netdev_name_p);
    free(response_data_p->ip_address_p);
    free(response_data_p->ipv6_address_p);
    free(response_data_p->subnet_netmask_p);
    free(response_data_p->default_gateway_p);
    free(response_data_p->dns_server_p);
    free(response_data_p->dns_server_2_p);
    free(response_data_p->ipv6_dns_server_p);
    free(response_data_p->ipv6_dns_server_2_p);
    free(response_data_p->p_cscf_server_p);
    free(response_data_p->p_cscf_server_2_p);
    free(response_data_p);
}

void exe_pscc_send_fail_cause_event(int32_t error_code)
{
    exe_last_fail_cause_t fail_response;

    fail_response.class = EXE_LAST_FAIL_CAUSE_CLASS_NET_PSCC;
    fail_response.cause = (int32_t)error_code;
    /* Register fail data in executor */
    exe_set_last_fail_cause(exe_get_exe(), &fail_response);

#ifdef EXE_USE_ATC_CUSTOMER_EXTENSIONS
    exe_ext_pscc_send_fail_cause_event(error_code);
#endif
}
