/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/* ATC related header files */
#include <atc.h>
#include <atc_command_list.h>
#include <atc_config.h>
#include <atc_exe_glue.h>
#include <atc_handlers.h>
#include <atc_log.h>
#include <atc_parser.h>
#include <atc_parser_util.h>
#include <atc_string.h>

#include <exe.h>

/* Standard C header files */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE_OF_CNAP_NAME      (80)         /* 3GPP 27.007 */
#define SIZE_OF_CNAP_RESPONSE  (10+SIZE_OF_CNAP_NAME+1)
#define STATIC_SIZE_OF_CSSU_RESPONSE (7+2+3+5+1+0+1+1) /* +CSSU: <code2>,<index>,<number>,<type> where <number> is dynamically allocated */
#define SIZE_OF_CSSI_RESPONSE  17
#define SIZE_OF_CCWA_RESPONSE  (7+MAX_DIGITS_IN_NUMBER+1+3+1+3) /* +CCWA: <number>,<type>,<class> */
#define SIZE_OF_CUUS1_RESPONSE (9+1+1+1024) /*+CUUS1x: <message>,<UUIE> */
#define MAX_USSD_LENGTH        512
#define DEFAULT_DCS_FOR_GSM7   15

typedef enum {
    CALL_WAITING_MODE_DISABLED     = 0,
    CALL_WAITING_MODE_ENABLED      = 1,
    CALL_WAITING_MODE_QUERY_STATUS = 2,
    CALL_WAITING_MODE_UNDEFINED    = 255  /* 255 is a value that is not a valid mode. Used later on to see if mode was set */
} call_waiting_mode;

typedef enum {
    CALL_WAITING_N_DISABLED     = 0,
    CALL_WAITING_N_ENABLED      = 1
} call_waiting_n;

typedef enum {
    CSSI_NOTIFICATION_DISABLE  = 0,
    CSSI_NOTIFICATION_ENABLE = 1
} ss_notification_cssi_mode;

typedef enum {
    CSSU_NOTIFICATION_DISABLE  = 0,
    CSSU_NOTIFICATION_ENABLE = 1
} ss_notification_cssu_mode;

typedef enum {
    CSSN_DISABLE  = 0,
    CSSN_ENABLE = 1
} ss_cssn_mode;

typedef enum {
    CODE1_UNCOND_CALL_FORWARD     = 0,
    CODE1__PARTIAL_CALL_FORWARD   = 1,
    CODE1_CALL_FORWARDED          = 2,
    CODE1_CALL_WAITING            = 3,
    CODE1_CUG_CALL                = 4,
    CODE1_OUTGOING_CALLS_BARRED   = 5,
    CODE1_INCOMING_CALLS_BARRED   = 6,
    CODE1_CLIR_SUPPESION_REJECTED = 7,
    CODE1_CALL_DEFLECTED          = 8
} cssn_code1;

typedef enum {
    CODE2_FORWARDED_CALL              = 0,
    CODE2_CUG_CALL                    = 1,
    CODE2_ON_HOLD                     = 2,
    CODE2_RETREIVED                   = 3,
    CODE2_MULTIPARTY                  = 4,
    CODE2_ON_HOLD_RELEASE             = 5,
    CODE2_FORWARD_CHECH_SS_RECEIVED   = 6,
    CODE2_ALERTING_CALL_CONNECTING    = 7,
    CODE2_ALERTING_CALL_CONNECTED     = 8,
    CODE2_CALL_DEFLECTED              = 9,
    CODE2_ADDITIONAL_INC_CALL_FORWARD = 10

} cssn_code2;

/*
 *===============================================================================
 *
 *  Function: AT_PLUS_CCWA_Handle
 *
 *  INPUT:   parser_p   - Pointer to the current parser state.
 *           info_text  - Pointer to a string buffer to put information text.
 *           Response   - Pointer to the requester response
 *
 *  OUTPUT:  parser_p->
 *           Pos        - The current position in the command line.
 *
 *  RETURNS: A status code for the execution of the command is returned:
 *           AT_OK        - The command was successfully handled.
 *           AT_ERROR     - There was a general error.
 *           AT_CME_ERROR - There was a GSM specific error, the error code is
 *                          returned in info_text.
 *
 *  Description: +CCWA allows control of the Call Waiting
 *  supplementary service.
 *  Activation, deactivation and status query are supported.
 *
 *===============================================================================
 */
AT_Command_e AT_PLUS_CCWA_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message)
{
    switch (parser_p->Mode) {
    case AT_MODE_SET: {
        if (!parser_p->IsPending) {
            IntParam_t n;
            IntParam_t mode = CALL_WAITING_MODE_UNDEFINED;
            IntParam_t classes = EXE_CLASSX_DEFAULT;   /* Default value according to spec. */
            bool error_flag = false;

            /* Extract AT command arguments */
            if (!parser_p->NoMoreParams) {
                n = Parser_GetIntParam(parser_p, &error_flag, &n);

                if (error_flag) {
                    Parser_SetCMEE_ErrorType(parser_p, CMEE_UNKNOWN);
                    goto error;
                }

                if (n > 1) {
                    Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
                    goto error;
                }

                parser_p->CCWA_n = n;
            }

            if (!parser_p->NoMoreParams) {
                mode = Parser_GetIntParam(parser_p, &error_flag, &mode);

                if (error_flag) {
                    Parser_SetCMEE_ErrorType(parser_p, CMEE_UNKNOWN);
                    goto error;
                }

                if (mode > CALL_WAITING_MODE_QUERY_STATUS) {
                    Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
                    goto error;
                }
            }

            if (!parser_p->NoMoreParams) {
                classes = Parser_GetIntParam(parser_p, &error_flag, &classes);

                if (error_flag) {
                    Parser_SetCMEE_ErrorType(parser_p, CMEE_UNKNOWN);
                    goto error;
                }
            }

            /* Allowed classes are 1,2,8,16,32,64,128 and the GSM default aggregated class 7. */
            if (EXE_CLASSX_UNDEFINED >= classes) {
                Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
                goto error;
            }

            if (!parser_p->NoMoreParams) {
                Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
                goto error;
            }

            if (CALL_WAITING_MODE_UNDEFINED != mode) {
                exe_request_result_t exe_res = EXE_FAILURE;
                exe_call_waiting_request_t request_data;
                exe_t *exe_p = atc_get_exe_handle();
                request_data.requested_classes = classes;
                request_data.mode = mode;

                switch (mode) {
                case CALL_WAITING_MODE_DISABLED:
                case CALL_WAITING_MODE_ENABLED: {
                    exe_res = exe_request(exe_p,
                                          EXE_CCWA_SET,
                                          &request_data,
                                          (void *)parser_p,
                                          &parser_p->request_handle);
                    break;
                }
                case CALL_WAITING_MODE_QUERY_STATUS: {
                    exe_res = exe_request(exe_p,
                                          EXE_CCWA_QUERY,
                                          &request_data,
                                          (void *)parser_p,
                                          &parser_p->request_handle);
                    break;
                }
                default: {
                    Parser_SetCMEE_ErrorType(parser_p, CMEE_OPERATION_NOT_SUPPORTED);
                    goto error;
                    break;
                }
                }

                switch (exe_res) {
                case EXE_PENDING:
                    return AT_PENDING;
                case EXE_FAILURE: {
                    exe_error_response_t *status = (exe_error_response_t *)message->data_p;
                    if (status) {
                        Parser_SetCMEE_ErrorType(parser_p, status->error);
                    } else {
                        Parser_SetCMEE_ErrorType(parser_p, CMEE_OPERATION_NOT_ALLOWED);
                    }
                    goto error;
                }
                case EXE_NOT_SUPPORTED:
                    Parser_SetCMEE_ErrorType(parser_p, CMEE_OPERATION_NOT_SUPPORTED);
                    goto error;
                case EXE_SUCCESS:
                    ATC_LOG_E("AT_PLUS_CCWA_Handle, executor returned EXE_SUCCESS");
                    goto error;
                default: {
                    Parser_SetCMEE_ErrorType(parser_p, CMEE_OPERATION_NOT_ALLOWED);
                    goto error;
                }
                }
            }

        } else {
            exe_call_waiting_response_t *response_p = NULL;

            if (NULL == message) {
                Parser_SetCMEE_ErrorType(parser_p, CMEE_UNKNOWN);
                goto error;
            }
            if (EXE_SUCCESS != message->exe_result_code) {
                exe_error_response_t *status = (exe_error_response_t *)message->data_p;
                if (status) {
                    Parser_SetCMEE_ErrorType(parser_p, status->error);
                } else {
                    Parser_SetCMEE_ErrorType(parser_p, CMEE_UNKNOWN);
                }
                goto error;
            }
            if (NULL != message->data_p) {
                response_p = (exe_call_waiting_response_t *)(message->data_p);
            }

            if (NULL != response_p && CALL_WAITING_MODE_QUERY_STATUS == response_p->mode) {

                uint8_t classcounter = 1;
                int i = 0;
                int result = 0;
                int response_mask = response_p->requested_classes;

                if (EXE_CLASSX_DATA & response_mask) {
                    response_mask |= EXE_CLASSX_DATA_CIRCUIT_SYNC | EXE_CLASSX_DATA_CIRCUIT_ASYNC | EXE_CLASSX_PACKET_ACCESS | EXE_CLASSX_PAD_ACCESS;
                    response_mask &= ~(EXE_CLASSX_DATA);
                }

                for (i = 1; i <= 8; i++) { /* Step through classcounter 1,2,4,8,16,32,64,128 and print out the status */
                    if (classcounter & response_mask) {
                        if (classcounter & response_p->classes_status) {
                            result = snprintf((char *)info_text, 8, ": %d,%d", 1 , classcounter);

                            if (0 > result) {
                                goto error;
                            }

                            Parser_SendMultipleResponses(parser_p, AT_NO_OVERRIDE, info_text, response_mask < (2 *(int)classcounter));
                        } else if (0 == (response_p->classes_status & response_mask)) {
                            /* The response line for 'not active' case (<status>=0)
                             * should be returned only if service is not active for any <class>.*/
                            result = snprintf((char *)info_text, 8, ": %d,%d", 0 , classcounter);

                            if (0 > result) {
                                goto error;
                            }

                            Parser_SendMultipleResponses(parser_p, AT_NO_OVERRIDE, info_text, response_mask < (2 *(int)classcounter));
                        }
                    }

                    classcounter = classcounter << 1;
                }


            }
        }

        break;
    }

    case AT_MODE_TEST: {
        sprintf((char *) info_text, ": (0,1)");
        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        break;
    }
    case AT_MODE_READ: {
        AT_AddValue(info_text, ": 0", parser_p->CCWA_n);
        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        break;
    }
    default: {
        Parser_SetCMEE_ErrorType(parser_p, CMEE_UNKNOWN);
        goto error;
        break;
    }
    } /* switch */

    return AT_OK;
error:
    return AT_CME_ERROR;
}


/*
 *===============================================================================
 *
 *   Function: AT_PLUS_CNAP_Handle
 *
 *   INPUT:   parser_p  - Pointer to the current parser state.
 *            info_text - Pointer to a string buffer to put information text.
 *
 *   OUTPUT:  parser_p->
 *              Pos     - The current position in the command line.
 *
 *   RETURNS: A status code for the execution of the command is returned:
 *            AT_OK        - The command was successfully handled.
 *            AT_ERROR     - There where a general error.
 *            AT_CME_ERROR - There where a GSM specific error, the error code is
 *                           returned in ASCII format in info_text.
 *
 *   The command enables, disables, reads or tests Calling Name Identification
 *   Presentation.
 *
 *===============================================================================
 */
AT_Command_e AT_PLUS_CNAP_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message)
{
    AT_Command_e result = AT_ERROR;

    switch (parser_p->Mode) {
    case AT_MODE_READ:

        if (!parser_p->IsPending) {
            exe_t *exe_p;
            exe_request_result_t exe_res = EXE_FAILURE;
            exe_p = atc_get_exe_handle();

            if (NULL != exe_p) {
                exe_res =
                    exe_request(exe_p, EXE_CNAP_READ, NULL,
                                (void *)parser_p, &parser_p->request_handle);
                result = parser_handle_exe_result(exe_res, parser_p);
            }
        } else {
            if (message) {
                sprintf((char *)info_text, ": %d,%d", parser_p->CNAP,
                        (EXE_SUCCESS == message->exe_result_code) ?
                        *((int *)message->data_p) : 2);
                Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
                result = AT_OK;
            } else {
                Parser_SetCMEE_ErrorType(parser_p,
                                         CMEE_OPERATION_NOT_ALLOWED);
                result = AT_CME_ERROR;
            }
        }

        break;

    case AT_MODE_SET:
    case AT_MODE_TEST:
        result = AT_Flag_Handle(parser_p, info_text, &parser_p->CNAP, 1);
        break;

    default:
        Parser_SetCMEE_ErrorType(parser_p, CMEE_UNKNOWN);
        result = AT_CME_ERROR;
        break;
    }

    return result;
}


/*
 * Handle incoming CNAP event
 */
void handle_incoming_CNAP_event(exe_cnap_response_t *response_p)
{
    AT_ParserState_s *parser_p;
    unsigned char entry;
    AT_CommandLine_t infotext_p = (AT_CommandLine_t)malloc(4 * SIZE_OF_CNAP_RESPONSE);
    size_t dest_length = 0;
    char *dest_p = NULL;
    int conv;

    if (!infotext_p) {
        ATC_LOG_E("infotext allocation failed!");
        return;
    }

    dest_p = (char *)calloc(4, response_p->name_char_length + 1);

    if (!dest_p) {
        ATC_LOG_E("dest_p allocation failed!");
        goto exit;
    }

    if (EXE_CNI_VALIDITY_NA_INTERWORKING != response_p->cni_validity && 0 < response_p->name_char_length) {
        /* Name is always provided in UCS-2 coding scheme. Convert it for presentation to UTF-8. */
        conv = atc_string_convert(ATC_CHARSET_UCS2, (uint8_t *)response_p->name_p,
                                  sizeof(uint16_t) * response_p->name_char_length,
                                  ATC_CHARSET_UTF8, (uint8_t *)dest_p, &dest_length);

        if (0 == conv) {
            ATC_LOG_E("String conversion failed!");
            goto exit;
        }
    }

    /* Build response string */
    sprintf((char *)infotext_p, "+CNAP: \"%s\",%d", dest_p, response_p->cni_validity);

    for (entry = 0; entry < MAX_NO_OF_OPEN_CHANNELS; entry++) {
        parser_p = ParserStateTable_GetParserState_ByEntry(entry);

        if (parser_p->CNAP) {
            parser_send_unsolicited(parser_p, AT_TEXT_RESPONSE, infotext_p);
        }
    }

exit:
    free(dest_p);
    free(infotext_p);
}


/*
 *===============================================================================
 *
 *  Function: CSSN_Set
 *
 *  Input:    parser_p   - Pointer to parser to which AT protocol response
 *                         eventually shall be sent.
 *
 *  Output:   parser_p   - Pointer to submitted parser, with state
 *                         Pending changed to TRUE.
 *
 *===============================================================================
 */
AT_Command_e CSSN_Set(AT_ParserState_s *parser_p)
{
    IntParam_t n = 0;
    IntParam_t m = 0;
    bool error_flag = false;
    exe_cssn_request_data_t req_data;
    exe_t *exe_p = NULL;
    AT_Command_e result = AT_OK;
    AT_ParserState_s *parser_entry_p = NULL;
    bool cssn_activated = false;
    unsigned char entry;
    /* Extract AT command arguments */
    n = Parser_GetIntParam(parser_p, &error_flag, &n);

    if (error_flag) {
        Parser_SetCMEE_ErrorType(parser_p, CMEE_UNKNOWN);
        return AT_CME_ERROR;
    }

    m = Parser_GetIntParam(parser_p, &error_flag, &m);

    if (error_flag) {
        Parser_SetCMEE_ErrorType(parser_p, CMEE_UNKNOWN);
        return AT_CME_ERROR;
    }

    /* Last Parameter received. Check if the user by mistake sends more parameters! */
    if (!parser_p->NoMoreParams) {
        return AT_CME_ERROR;
    }

    /* <Reason> and <Mode> are used as array index. */
    /* (Classes > EXE_CLASSX_ALL) will be detected by Parser_GetIntParam, i.e. */
    /* covered by the check for PAS_SS_ErrorFlag. */
    if ((n > 1) || (m > 1)) {
        Parser_SetCMEE_ErrorType(parser_p, CMEE_OPERATION_NOT_SUPPORTED);
        return AT_CME_ERROR;
    }

    if (CSSI_NOTIFICATION_ENABLE == n || CSSU_NOTIFICATION_ENABLE == m) {

        /* Loop through the ParserStateTable and see if this has already been activated in the executor */
        for (entry = 0; entry < MAX_NO_OF_OPEN_CHANNELS; entry++) {
            parser_entry_p = ParserStateTable_GetParserState_ByEntry(entry);

            if (CSSI_NOTIFICATION_ENABLE == parser_entry_p->CSSI || CSSU_NOTIFICATION_ENABLE == parser_entry_p->CSSU) {
                cssn_activated = true;
                break;
            }
        }

        if (!cssn_activated) {
            req_data.mode = CSSN_ENABLE;
            exe_p = atc_get_exe_handle();
            result = exe_request(exe_p, EXE_CSSN, (void *) &req_data, (void *) parser_p, &parser_p->request_handle);

            if (EXE_PENDING == result) {
                result = AT_PENDING;
            } else if (EXE_SUCCESS == result) {
                result = AT_OK;
            } else {
                Parser_SetCMEE_ErrorType(parser_p, CMEE_UNKNOWN);
                return AT_CME_ERROR;
            }
        }

    } else if (CSSI_NOTIFICATION_DISABLE == n && CSSU_NOTIFICATION_DISABLE == m) {
        cssn_activated = false;

        /* Loop through the ParserStateTable and see if any other ParserState has this activated */
        for (entry = 0; entry < MAX_NO_OF_OPEN_CHANNELS; entry++) {
            parser_entry_p = ParserStateTable_GetParserState_ByEntry(entry);

            if (CSSI_NOTIFICATION_ENABLE == parser_entry_p->CSSI || CSSU_NOTIFICATION_ENABLE == parser_entry_p->CSSU) {
                if (parser_entry_p != parser_p) {
                    cssn_activated = true;
                    break;
                }
            }
        }

        if (!cssn_activated) {
            req_data.mode = CSSN_DISABLE;
            exe_p = atc_get_exe_handle();
            result = exe_request(exe_p, EXE_CSSN, (void *) &req_data, (void *) parser_p, &parser_p->request_handle);

            if (EXE_PENDING == result) {
                result = AT_PENDING;
            } else if (EXE_SUCCESS == result) {
                result = AT_OK;
            } else {
                Parser_SetCMEE_ErrorType(parser_p, CMEE_UNKNOWN);
                return AT_CME_ERROR;
            }
        }
    }

    parser_p->CSSI = n;
    parser_p->CSSU = m;

    return result;
} /* End of CSSN_Set */


/*
 *  Function: AT_PLUS_CSSN_Handle
 *
 *  INPUT:   parser_p   - Pointer to the current parser state.
 *           InfoText   - Pointer to a string buffer to put information text.
 *           Response   - Pointer to the requester response
 *
 *  OUTPUT:  Parser->
 *           Pos        - The current position in the command line.
 *
 *  RETURNS: A status code for the execution of the command is returned:
 *           AT_OK        - The command was successfully handled.
 *           AT_ERROR     - There was a general error.
 *           AT_CME_ERROR - There was a GSM specific error, the error code is
 *                          returned in InfoText.
 *
 *  Description:  Supplementary Service Notification. +CSSN allows control of the presentation of notification result codes from TA to TE.
 *
 * SET  - +CSSN=[<n>[,<m>]]
 *        Enables/disables the presentation of notification result codes from TA
 *        to TE.
 * READ - +CSSN?
 *        CSSN: <n>,<m>
 * TEST - +CSSN=?
 *        (List of supported <n>s), (List of supported <m>s).*
 *
 *  References:
 *
 *
 *===============================================================================
 */

AT_Command_e AT_PLUS_CSSN_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t InfoText,
                                 AT_ResponseMessage_t message)
{
    AT_Command_e result = AT_ERROR;

    switch (parser_p->Mode) {
    case AT_MODE_SET: {
        if (!parser_p->IsPending) {
            result = CSSN_Set(parser_p);
        } else {
            if (message->exe_result_code == EXE_SUCCESS) {
                result = AT_OK;
            } else {
                Parser_SetCMEE_ErrorType(parser_p,
                                         CMEE_OPERATION_NOT_ALLOWED);
                result = AT_CME_ERROR;
            }
        }

        break;
    }
    case AT_MODE_READ: {
        AT_AddRange(InfoText, ": 0,0",
                    (IntParam_t) parser_p->CSSI,
                    (IntParam_t) parser_p->CSSU);
        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, InfoText);
        result = AT_OK;
        break;
    }
    case AT_MODE_TEST: {
        /* Send the range of supported values. */
        AT_AddRange(InfoText, ": (0,0)", 0, 1);
        AT_AddRange(InfoText, ",(0,0)", 0, 1);
        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, InfoText);
        result = AT_OK;
        break;
    }
    default: {
        Parser_SetCMEE_ErrorType(parser_p, CMEE_UNKNOWN);
        result = AT_CME_ERROR;
        break;
    }
    } /* switch */

    return result;
}


/*
 * Handle CSSI events sent from modem. Compose an AT string and send the unsolicited.
 */
void handle_incoming_CSSI_event(exe_cssi_response_t *response_p)
{
    AT_ParserState_s *parser_p;
    unsigned char entry;
    AT_CommandLine_t infotext = (unsigned char *) alloca(SIZE_OF_CSSI_RESPONSE);
    int res = 0;

    if (!infotext) {
        return;
    }

    *infotext = END_OF_STRING;

    if (CODE1_CUG_CALL == response_p->code1) {
        /* CUG call */
        res = snprintf((char *)infotext, SIZE_OF_CSSI_RESPONSE, "+CSSI: %d,%d", response_p->code1, response_p->index);
    } else {
        res = snprintf((char *)infotext, SIZE_OF_CSSI_RESPONSE, "+CSSI: %d", response_p->code1);
    }

    if (0 >= res || SIZE_OF_CSSI_RESPONSE <= res) {
        ATC_LOG_E("handle_incoming_CSSI_event: error writing response");
        return;
    }

    for (entry = 0; entry < MAX_NO_OF_OPEN_CHANNELS; entry++) {
        parser_p = ParserStateTable_GetParserState_ByEntry(entry);

        if (parser_p->CSSI) {
            parser_send_unsolicited(parser_p, AT_TEXT_RESPONSE, infotext);
        }
    }
}

/*
 * Handle CSSU events sent from modem. Compose an AT string and send the unsolicited.
 */
void handle_incoming_CSSU_event(exe_cssu_response_t *response_p)
{
    AT_ParserState_s *parser_p;
    unsigned char entry;
    AT_CommandLine_t infotext = NULL;
    int res = 0;
    int number_len = 0;
    int total_len = 0;

    if (response_p->number) {
        number_len = strlen(response_p->number);

        if ((EXE_TYPE_OF_NUMBER_INTERNATIONAL == response_p->address_type)
                && (*response_p->number != '+')) {
            number_len += 1;
        }
    } else {
        response_p->number = (char *)malloc(4);

        if (!response_p->number) {
            goto error;
        }

        strcpy(response_p->number, "");
    }

    total_len = STATIC_SIZE_OF_CSSU_RESPONSE + number_len + 1; /*static length + length for number + null termination */

    infotext = (unsigned char *)alloca(total_len);

    if (!infotext) {
        goto error;
    }

    *infotext = END_OF_STRING;

    if (CODE2_CUG_CALL == response_p->code2) {
        /* CUG call */
        res = snprintf((char *) infotext, total_len, "+CSSU: %d,%d,\"%s%s\",%d",
                       response_p->code2,
                       response_p->index,
                       EXE_TYPE_OF_NUMBER_INTERNATIONAL == response_p->address_type && *response_p->number != '+' && strlen(response_p->number) != 0 ? "+" : "",
                       response_p->number,
                       response_p->address_type);
    } else {
        res = snprintf((char *) infotext, total_len, "+CSSU: %d,,\"%s%s\",%d",
                       response_p->code2,
                       EXE_TYPE_OF_NUMBER_INTERNATIONAL == response_p->address_type && *response_p->number != '+' && strlen(response_p->number) != 0 ? "+" : "",
                       response_p->number,
                       response_p->address_type);
    }

    if (0 >= res || total_len <= res) {
        ATC_LOG_E("error writing response");
        goto error;
    }

    for (entry = 0; entry < MAX_NO_OF_OPEN_CHANNELS; entry++) {
        parser_p = ParserStateTable_GetParserState_ByEntry(entry);

        if (parser_p->CSSU) {
            parser_send_unsolicited(parser_p, AT_TEXT_RESPONSE, infotext);
        }
    }

error:

    if (response_p->number) {
        free(response_p->number);
    }
}

/*==============================================================================
 *
 *  Function: AT_PLUS_CUSD_Handle
 *
 *  INPUT:   parser_p   - Pointer to the current parser state.
 *           InfoText   - Pointer to a string buffer to put information text.
 *           Response   - Pointer to the requester response
 *
 *  OUTPUT:  Parser->
 *           Pos        - The current position in the command line.
 *
 *  RETURNS: A status code for the execution of the command is returned:
 *           AT_OK        - The command was successfully handled.
 *           AT_ERROR     - There was a general error.
 *           AT_CME_ERROR - There was a GSM specific error, the error code is
 *                          returned in InfoText.
 *
 *  Description:  Supplementary Service Notification. +CSSN allows control of the presentation of notification result codes from TA to TE.
 *
 * SET  - +CSSN=[<n>[,<m>]]
 *        Enables/disables the presentation of notification result codes from TA
 *        to TE.
 * READ - +CSSN?
 *        CSSN: <n>,<m>
 * TEST - +CSSN=?
 *        (List of supported <n>s), (List of supported <m>s).*
 *
 *  References:
 *
 *============================================================================*/
AT_Command_e AT_PLUS_CUSD_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message)
{
    AT_Command_e result = AT_ERROR;
    unsigned char *ussd_result = NULL;

    switch (parser_p->Mode) {
    case AT_MODE_SET: {
        if (!parser_p->IsPending) {
            bool error_flag = false;
            StrParam_t empty_string = "";
            IntParam_t n = 0;
            StrParam_t str = empty_string;
            IntParam_t dcs = 0;
            atc_charset_t output_charset = ATC_CHARSET_GSM7;
            size_t ussd_length = 0;
            size_t converted_characters = 0;

            /* Extract AT command arguments */
            n = Parser_GetIntParam(parser_p, &error_flag, &n);

            if (error_flag) {
                Parser_SetCMEE_ErrorType(parser_p, CMEE_UNKNOWN);
                return AT_CME_ERROR;
            }

            str = Parser_GetStrParam(parser_p, &error_flag, empty_string);

            if (error_flag) {
                Parser_SetCMEE_ErrorType(parser_p, CMEE_UNKNOWN);
                return AT_CME_ERROR;
            }

            dcs = Parser_GetIntParam(parser_p, &error_flag, &dcs);

            if (error_flag) {
                Parser_SetCMEE_ErrorType(parser_p, CMEE_UNKNOWN);
                return AT_CME_ERROR;
            }

            /* Last Parameter received. Check if the user by mistake sends more parameters! */
            if (!parser_p->NoMoreParams) {
                return AT_CME_ERROR;
            }

            if (n > 2) {
                Parser_SetCMEE_ErrorType(parser_p, CMEE_OPERATION_NOT_SUPPORTED);
                return AT_CME_ERROR;
            }

            /* Cancel USSD dialog? */
            if (n == 2) {
                exe_t *exe_p = atc_get_exe_handle();
                exe_request_result_t exe_result = exe_request(exe_p, EXE_CUSD_CANCEL_USSD, NULL,
                                                  parser_p, &parser_p->request_handle);

                if (exe_result == EXE_PENDING) {
                    result = AT_PENDING;
                }

                goto exit;
            }

            /* Store AT parameter <n> for current parser. */
            parser_p->CUSD = n;

            /* Skip interrogating network? */
            if (NULL == str || empty_string == str) {
                result = AT_OK;
                goto exit;
            }

            /* Use MAX_USSD_LENGTH instead of guessing the converted size. */
            ussd_result = (unsigned char *)calloc(1, MAX_USSD_LENGTH);

            if (NULL == ussd_result) {
                ATC_LOG_E("AT_PLUS_CUSD_Handle: Allocation failed");
                result = AT_ERROR;
                goto exit;
            }

            if (dcs < 16 || !(dcs & 0x0c)) {
                ATC_LOG_I("%s: Text encoding GSM7, dcs: %d", __FUNCTION__, dcs);
                output_charset = ATC_CHARSET_GSM7;
                /* Set the dcs value to default, as the request otherwise may be rejected by the network. */
                dcs = DEFAULT_DCS_FOR_GSM7;
            } else {
                ATC_LOG_E("%s: Unsupported data coding scheme, dcs: %d", __FUNCTION__, dcs);
                result = AT_ERROR;
                goto exit;
            }

            converted_characters = atc_string_convert(parser_p->cscs_charset, (uint8_t *)str, strlen((char *)str),
                                   output_charset, (uint8_t *)ussd_result, &ussd_length);

            if (0 == converted_characters) {
                ATC_LOG_E("%s: String conversion failed", __FUNCTION__);
                result = AT_ERROR;
                goto exit;
            }

            exe_cusd_request_data_t data = { ussd_length, dcs, ussd_result };
            exe_t *exe_p = atc_get_exe_handle();
            exe_request_result_t exe_result = exe_request(exe_p, EXE_CUSD_REQUEST_USSD, &data,
                                              parser_p, &parser_p->request_handle);

            if (EXE_PENDING == exe_result) {
                result = AT_PENDING;
            }

            goto exit;
        } else {
            ATC_LOG_I("AT_PLUS_CUSD_Handle: RESPONSE!");

            if (parser_p->abort) {
                exe_request_result_t er = exe_request_abort(atc_get_exe_handle(), parser_p->request_handle);

                if (EXE_SUCCESS != er && EXE_PENDING != er) {
                    ATC_LOG_E("Abort of request failed!");
                }

                result = AT_OK;
                break;
            }

            if (EXE_SUCCESS == message->exe_result_code) {
                result = AT_OK;
            } else {
                exe_error_response_t *error = (exe_error_response_t *)message->data_p;
                if (!error) {
                    Parser_SetCMEE_ErrorType(parser_p, CMEE_UNKNOWN);
                } else {
                    Parser_SetCMEE_ErrorType(parser_p, error->error);
                }
                result = AT_CME_ERROR;
            }
        }
    }
    break;

    case AT_MODE_READ:
        AT_AddValue(info_text, ": 0", (IntParam_t) parser_p->CUSD);
        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        result = AT_OK;
        break;

    case AT_MODE_TEST:
        strncpy((char *) info_text, ": (0-2)", 8);
        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        result = AT_OK;
        break;

    default:
        ATC_LOG_E("AT_PLUS_CUSD_Handle: Unknown parser state");
        break;
    } /* switch */

exit:
    /* Release memory */
    free((void *)ussd_result);

    return result;
}


void set_uus_setup_message(AT_ParserState_s *parser_p, uint8_t length, uint8_t *data)
{
    if (parser_p->CUUS1_setup_msg.data) {
        free(parser_p->CUUS1_setup_msg.data);
    }

    parser_p->CUUS1_setup_msg.length = length;

    if (length > 0) {
        parser_p->CUUS1_setup_msg.data = calloc(1, length);

        if (parser_p->CUUS1_setup_msg.data == NULL) {
            ATC_LOG_E("Memory allocation failed");
            return;
        }

        memmove(parser_p->CUUS1_setup_msg.data, data, length);
    } else {
        parser_p->CUUS1_setup_msg.dcs = 0;
        parser_p->CUUS1_setup_msg.data = NULL;
    }
}

/*==============================================================================
 *
 *  Function: AT_PLUS_CUUS1_Handle
 *
 *  INPUT:   parser_p   - Pointer to the current parser state.
 *           InfoText   - Pointer to a string buffer to put information text.
 *           Response   - Pointer to the requester response
 *
 *  OUTPUT:  Parser->
 *           Pos        - The current position in the command line.
 *
 *  RETURNS: A status code for the execution of the command is returned:
 *           AT_OK        - The command was successfully handled.
 *           AT_ERROR     - There was a general error.
 *           AT_CME_ERROR - There was a GSM specific error, the error code is
 *                          returned in InfoText.
 *
 *  Description:  User-to-User Signalling Service 1. +CUUS1 is used to activate/deactivate implicit UUS requests.
 *
 * SET  - +CUUS1=[<n>[,<m>[,<message>[,<UUIE> [,<message>[,<UUIE>[,...]]]]]]]
 *        Control of the User-to-User Signalling Supplementary Service 1 (UUS1) according to 3G 22.087
 *
 * READ - +CUUS1?
 *        +CUUS1: <n>,<m>[,<message>,<UUIE> [,<message>,<UUIE>[,...]]]
 * TEST - +CUUS1=?
 *        +CUUS1: (list of supported <n>s), (list of supported <m>s), (list of supported <message>s), (list of supported <messageI>s),(list of supported <messageU>s)
 *
 *  References: 3GPP 27.007 7.26
 *
 *============================================================================*/
AT_Command_e AT_PLUS_CUUS1_Handle(AT_ParserState_s *parser_p,
                                  AT_CommandLine_t info_text,
                                  AT_ResponseMessage_t message)
{

    AT_Command_e result = AT_ERROR;
    uint8_t *uus_unpacked = NULL;
    unsigned char *temp_str_hex_p = NULL;
    size_t converted_size = 0;

    switch (parser_p->Mode) {
    case AT_MODE_SET: {
        bool error_flag = false;
        StrParam_t empty_string = "";
        IntParam_t n = 0;
        IntParam_t m = 0;
        IntParam_t uus_type = 0;
        StrParam_t uus_packed = empty_string;
        int32_t uus_length = 0;

        /* Extract AT command arguments */
        n = Parser_GetIntParam(parser_p, &error_flag, &n);

        if (error_flag) {
            Parser_SetCMEE_ErrorType(parser_p, CMEE_UNKNOWN);
            return AT_CME_ERROR;
        }

        if (n != 0) {
            Parser_SetCMEE_ErrorType(parser_p, CMEE_OPERATION_NOT_SUPPORTED);
            return AT_CME_ERROR;
        }

        m = Parser_GetIntParam(parser_p, &error_flag, &m);

        if (error_flag) {
            Parser_SetCMEE_ErrorType(parser_p, CMEE_UNKNOWN);
            return AT_CME_ERROR;
        }

        if (m != 0 && m != 1) {
            Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
            return AT_CME_ERROR;
        }

        while (!parser_p->NoMoreParams) {
            uus_type = Parser_GetIntParam(parser_p, &error_flag, &uus_type);

            if (error_flag) {
                Parser_SetCMEE_ErrorType(parser_p, CMEE_UNKNOWN);
                result = AT_CME_ERROR;
                goto exit;
            }

            if ((uus_type != CUUS1_MSG_ANY) && (uus_type != CUUS1_MSG_SETUP)) {
                Parser_SetCMEE_ErrorType(parser_p, CMEE_OPERATION_NOT_SUPPORTED);
                result = AT_CME_ERROR;
                goto exit;
            }

            uus_packed = Parser_GetStrParam(parser_p, &error_flag, empty_string);

            if (!error_flag && strlen(uus_packed) > 0) {
                uus_unpacked = calloc(1, strlen(uus_packed));

                if (uus_unpacked == NULL) {
                    Parser_SetCMEE_ErrorType(parser_p, CMEE_UNKNOWN);
                    result = AT_CME_ERROR;
                    goto exit;
                }

                if (atc_string_convert(ATC_CHARSET_HEX, (uint8_t *)uus_packed, strlen((char *)uus_packed),
                                       ATC_CHARSET_UNDEF_BINARY, (uint8_t *)uus_unpacked, &converted_size)) {
                    /* Data must always begin with IEI and highest bit is not valid */
                    if ((uus_unpacked[0] & 0x7F) != CUUS1_IEI) {
                        Parser_SetCMEE_ErrorType(parser_p, CMEE_UNKNOWN);
                        result = AT_CME_ERROR;
                        goto exit;
                    }

                    /* Second byte is length,
                     * binary length should not be longer than hex coded length */
                    if (uus_unpacked[1] > strlen(uus_packed)) {
                        Parser_SetCMEE_ErrorType(parser_p, CMEE_UNKNOWN);
                        result = AT_CME_ERROR;
                        goto exit;
                    }

                    set_uus_setup_message(parser_p, uus_unpacked[1], (uint8_t *)&uus_unpacked[2]);
                } else {
                    ATC_LOG_E("AT_PLUS_CUUS1_Handle: string conversion failed");
                    result = AT_ERROR;
                    goto exit;
                }

                free(uus_unpacked);
                uus_unpacked = NULL;
            } else {
                set_uus_setup_message(parser_p, 0, NULL);
            }
        }

        parser_p->CUUS1I = n;  /* Currently this is always 0 */
        parser_p->CUUS1U = m;
        result = AT_OK;

    }
    break;

    case AT_MODE_READ: {
        sprintf((char *)info_text, ": %d,%d", parser_p->CUUS1I, parser_p->CUUS1U); /* <n> */ /* <m> */

        if (parser_p->CUUS1_setup_msg.data != NULL) {
            temp_str_hex_p = (unsigned char *)calloc(1, (2 * parser_p->CUUS1_setup_msg.length) + 2 + 2 + 1);
            /* Length converted to hex + CUUS1_IEI + Length in hex + null termination */

            if (!temp_str_hex_p) {
                goto exit;
            }

            sprintf((char *)(info_text + 5), ",%d,", CUUS1_MSG_SETUP);
            sprintf((char *)temp_str_hex_p, "%02X", CUUS1_IEI);

            atc_string_convert(ATC_CHARSET_UNDEF_BINARY, &parser_p->CUUS1_setup_msg.length, 1,
                               ATC_CHARSET_HEX, (uint8_t *)temp_str_hex_p + 2, &converted_size);
            atc_string_convert(ATC_CHARSET_UNDEF_BINARY, parser_p->CUUS1_setup_msg.data, parser_p->CUUS1_setup_msg.length,
                               ATC_CHARSET_HEX, (uint8_t *)temp_str_hex_p + 4, &converted_size);
            sprintf((char *)(info_text + 8), "\"%s\"", (char *)temp_str_hex_p);
        }

        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        result = AT_OK;

    }
    break;

    case AT_MODE_TEST: {
        /* SETUP messages are only supported */

        strcat((char *)info_text, ": (0)");     /* Supported <n>s */
        strcat((char *)info_text, ", (0-1)");   /* Supported <m>s */
        strcat((char *)info_text, ", (0-1)");   /* Supported <message>s */
        strcat((char *)info_text, ", ");        /* Supported <messageI>s (i.e. none)*/
        strcat((char *)info_text, ", (0-1)");   /* Supported <messageU>s */
        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        result = AT_OK;
    }
    break;

    default:
        ATC_LOG_E("AT_PLUS_CUUS1_Handle: Unknown parser state");
        break;
    } /* switch */

exit:
    free(uus_unpacked);
    free(temp_str_hex_p);

    return result;
}


/*
 * Handle incoming CUUS1I event
 */
void handle_incoming_CUUS1I_event(exe_cuus1_response_t *response_p)
{
    /*
     * No support in MAL currently of this functionality.
     * When there is it should be implemented.
     *
     */
}

/*
 * Handle incoming CUUS1U event
 */
void handle_incoming_CUUS1U_event(exe_cuus1_response_t *response_p)
{
    AT_ParserState_s *parser_p;
    unsigned char entry;
    uint8_t *hex_buff_p = NULL;
    AT_CommandLine_t infotext = (unsigned char *)calloc(1, SIZE_OF_CUUS1_RESPONSE);
    size_t converted_size = 0;

    if (!infotext) {
        return;
    }

    if (response_p == NULL) {
        free(infotext);
        return;
    }
    *infotext = END_OF_STRING;
    hex_buff_p = calloc(1, response_p->len * 2 + 2 + 1); /* Length converted to hex + Length in hex + null termination */

    if (hex_buff_p == NULL) {
        ATC_LOG_E("Memory allocation failed");
        free(infotext);
        return;
    }

    atc_string_convert(ATC_CHARSET_UNDEF_BINARY, (uint8_t *)&response_p->len, 1,
                       ATC_CHARSET_HEX, (uint8_t *)hex_buff_p, &converted_size);
    atc_string_convert(ATC_CHARSET_UNDEF_BINARY, (uint8_t *)response_p->buff, response_p->len,
                       ATC_CHARSET_HEX, (uint8_t *)hex_buff_p + 2, &converted_size);
    sprintf((char *)infotext, "+CUUS1U: %d,\"%02X%s\"", response_p->type, CUUS1_IEI, hex_buff_p);

    for (entry = 0; entry < MAX_NO_OF_OPEN_CHANNELS; entry++) {
        parser_p = ParserStateTable_GetParserState_ByEntry(entry);

        if (parser_p->CUUS1U) {
            parser_send_unsolicited(parser_p, AT_TEXT_RESPONSE, infotext);
        }

        ATC_LOG_I("AT_PLUS_CUUS1U: %s", infotext);
    }
    free(infotext);
    free(hex_buff_p);
}



/*
 *===============================================================================
 *
 *   Function: AT_PLUS_COLR_Handle
 *
 *  INPUT:   parser_p   - Pointer to the current parser state.
 *           InfoText   - Pointer to a string buffer to put information text.
 *           Response   - Pointer to the requester response
 *
 *  OUTPUT:  Parser->
 *           Pos        - The current position in the command line.
 *
 *  RETURNS: A status code for the execution of the command is returned:
 *           AT_OK        - The command was successfully handled.
 *           AT_ERROR     - There was a general error.
 *           Note: according to spec there is no CMEEs in the possible responses
 *
 *        This function read the NW setting for Connected Line Identification Restriction
 *        (3GPP TS 22.081). It has no effect on the execution of the supplementary service
 *        COLR in the NW. Spec used for command is 3GPP TS 27.007 9.4.0.
 *
 * SET  - +COLR
 *        returns the  provisioning, i.e. +COLR:0, +COLR:1 or +COLR:2
 *
 * READ - Not applicable.
 *
 * TEST - +COLR=?
 *        No response except "OK".
 *
 *===============================================================================
 */
AT_Command_e AT_PLUS_COLR_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text, AT_ResponseMessage_t message)
{
    AT_Command_e result = AT_ERROR;

    switch (parser_p->Mode) {
    case AT_MODE_DO: {
        if (!parser_p->IsPending) {
            exe_t *exe_p = NULL;
            exe_request_result_t exe_res = EXE_FAILURE;

            exe_p = atc_get_exe_handle();
            exe_res = exe_request(exe_p, EXE_COLR_DO, NULL, (void *) parser_p,
                                  &parser_p->request_handle);

            if (EXE_PENDING == exe_res) {
                result = AT_PENDING;
            }
        } else { /* Pending */
            if (message) {
                sprintf((char *) info_text, ": %d",
                        (EXE_SUCCESS == message->exe_result_code) ? *((int *) message->data_p)
                        : 2);
                Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
                result = AT_OK;
            } else {
                Parser_SetCMEE_ErrorType(parser_p, CMEE_OPERATION_NOT_ALLOWED);
                result = AT_CME_ERROR;
            }
        }

        break;
    }
    case AT_MODE_TEST:
        result = AT_OK;
        break;
    default:
        break;

    }

    return result;
}


#define COLP_PROVISIONING_UNKNOWN 2
/*
 *===============================================================================
 *
 *   Function: AT_PLUS_COLP_Handle
 *
 *  INPUT:   parser_p   - Pointer to the current parser state.
 *           InfoText   - Pointer to a string buffer to put information text.
 *           Response   - Pointer to the requester response
 *
 *  OUTPUT:  Parser->
 *           Pos        - The current position in the command line.
 *
 *  RETURNS: A status code for the execution of the command is returned:
 *           AT_OK        - The command was successfully handled.
 *           AT_ERROR     - There was a general error.
 *           Note: according to spec there is no CMEEs in the possible responses
 *
 *   This function handle the supplementary services functionality
 *   (COLP) connected line identification presentation.
 *   It means that the called party's number and auxiliary information will be received
 *   by the network  if no restrictions are put upon it.  The functionality is currently
 *   limited to actually enable/disable locally if the unsolicited event +COLP: <...> is
 *   to be passed on to the client. The provisioning is set to 2 = Unknown as there is no modem
 *   support currently.
 *
 * SET  - +COLP=[<n>]
 *        Enables/disables the presentation of notification result codes from TA
 *        to TE.
 * READ - +COLP?
 *        COLP: <n>,2
 * TEST - +COLP=?
 *        (List of supported <n>s), (List of supported <m>s).
 *
 *===============================================================================
 */
AT_Command_e AT_PLUS_COLP_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message)
{
    AT_Command_e result = AT_ERROR;

    switch (parser_p->Mode) {
    case AT_MODE_READ: {
        sprintf((char *)info_text, ": %d,%d", (IntParam_t) parser_p->colp_state, COLP_PROVISIONING_UNKNOWN);
        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        result = AT_OK;
        break;
    }

    case AT_MODE_SET: {
        IntParam_t colp_state = COLP_DISABLED;
        bool error_flag = false;

        colp_state = Parser_GetIntParam(parser_p, &error_flag, &colp_state);

        if (error_flag || (COLP_DISABLED != colp_state && COLP_ENABLED != colp_state) || !parser_p->NoMoreParams) {
            goto error;
        }

        parser_p->colp_state = (colp_state_e)colp_state;
        result = AT_OK;
        break;
    }

    case AT_MODE_TEST: {
        strncpy((char *) info_text, ": (0,1)", 8);
        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        result = AT_OK;
        break;
    }
    default:
        break;
    }

error:
    return result;
}

/*
 * Handle COLP events sent from MAL. Compose an AT string and broadcast it to the client.
 *
 * UNSOLICITED:
 *        +COLP: <number><type>[<subaddr><satype>[,<alpha>]]
 *
 *              Parameters received from the network includes:
 *              <number> : Phone number as specified by <type>
 *              <type>   : 129,145,161 -> ISDN numbering. other value in the range 128-..255 as
 *              described in GSM 04.08 section 10.5.4.3
 *
 *              Below parameters not to be supported:
 *              <subaddr>, <satype>
 *              <alpha>  : Optional string with corresponding entry found in PHONE book
 *
 *        TODO: Note that alpha is not implemented here. Alpha is a string corresponding to
 *              matching number in he phone book. As it is currently not understood how this
 *              work, this return value is currently NOT SUPPORTED.
 */
void handle_incoming_COLP_event(exe_cusd_response_t *response_p)
{
    /*
     * No support in MAL currently of this functionality.
     * When there is it should be implemented.
     *
     */
}


#define DEFAULT_TIME_CALL_FORWARDING 20 /*seconds*/
/*
 *===============================================================================
 *
 *   Function: AT_PLUS_CCFC_Handle
 *
 *  INPUT:   parser_p   - Pointer to the current parser state.
 *           InfoText   - Pointer to a string buffer to put information text.
 *           Response   - Pointer to the requester response
 *
 *  OUTPUT:  Parser->
 *           Pos        - The current position in the command line.
 *
 *  RETURNS: A status code for the execution of the command is returned:
 *           AT_OK        - The command was successfully handled.
 *           AT_ERROR     - There was a general error.
 *
 *   This function handle the supplementary services functionality
 *   (CCFC) Call Forwarding Number and Conditions.
 *
 * SET  - +CCFC=[<n>]
 *        Enables/disables the presentation of notification result codes from TA
 *        to TE.
 * READ - +CCFC? NOT APPLICABLE
 *
 * TEST - +CCFC=?
 *        (List of supported reasons)
 *
 *        Relevant specifications: ETSI TS 122 082
 *
 *===============================================================================
 */
AT_Command_e AT_PLUS_CCFC_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message)
{
    AT_Command_e result = AT_CME_ERROR;

    switch (parser_p->Mode) {
    case AT_MODE_SET:

        if (!parser_p->IsPending) {
            exe_t               *exe_p        = NULL;
            bool                 error_flag   = false;
            exe_request_result_t exe_res      = EXE_FAILURE;
            IntParam_t           reason       = (IntParam_t)EXE_CCFC_REASON_UNKNOWN;
            IntParam_t           mode         = (IntParam_t)EXE_CCFC_MODE_UNKNOWN;
            StrParam_t           number_p     = "\0"; /* May not be NULL as that would fail the parsing. Parameter can be empty */
            IntParam_t           type         = EXE_CCFC_TYPE_NONE;
            IntParam_t           class        = (IntParam_t)EXE_CLASSX_DEFAULT;
            IntParam_t           time         = 0;

            exe_ccfc_data_t      ccfc_data;

            exe_p = atc_get_exe_handle();

            if (NULL != exe_p) {
                reason = Parser_GetIntParam(parser_p, &error_flag, &reason);

                if (error_flag || EXE_CCFC_REASON_UNKNOWN <= reason) {
                    Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
                    return AT_CME_ERROR;
                }

                mode = Parser_GetIntParam(parser_p, &error_flag, &mode);

                if (error_flag || EXE_CCFC_MODE_UNKNOWN <= mode) {
                    Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
                    return AT_CME_ERROR;
                }

                if (!parser_p->NoMoreParams) {

                    number_p = Parser_GetStrParam(parser_p, &error_flag, number_p);

                    if (error_flag) {
                        Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
                        return AT_CME_ERROR;
                    }
                }

                if (!parser_p->NoMoreParams) {

                    type = Parser_GetIntParam(parser_p, &error_flag, &type);

                    /* NB: IntParam_t is unsigned char. As max type is all 8 bits set = 255
                     * check for a value greater than max value becomes meaningless */
                    if (!strcmp(number_p, "\0")) {
                        type = 0;
                    } else if (error_flag || EXE_CCFC_TYPE_MIN > type) {
                        Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
                        return AT_CME_ERROR;
                    }
                }

                if (!parser_p->NoMoreParams) {
                    class = Parser_GetIntParam(parser_p, &error_flag, &class);

                    /* Allowed classes are 1,2,8,16,32,64,128 and the GSM default aggregated class 7. */
                    if (error_flag) {
                        Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
                        return AT_CME_ERROR;
                    }
                }

                if (EXE_CLASSX_UNDEFINED >= class) {
                    Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
                    return AT_CME_ERROR;
                }

                if (!parser_p->NoMoreParams) {
                    bool       dummy_error_flag;
                    StrParam_t subaddr_p = NULL;
                    (void)Parser_GetStrParam(parser_p, &dummy_error_flag, subaddr_p);
                    /* This parameter is not supported and will hence not be evaluated.
                     * Is parsed for completeness and to add support for future inclusion if need be */
                }

                if (!parser_p->NoMoreParams) {
                    bool dummy_error_flag;
                    IntParam_t satype = 0;
                    (void)Parser_GetIntParam(parser_p, &dummy_error_flag, &satype);
                    /* This parameter is not supported and will hence not be evaluated.
                     * Is parsed for completeness and to add support for future inclusion if need be */
                }

                if (!parser_p->NoMoreParams) {
                    time = Parser_GetIntParam(parser_p, &error_flag, &time);

                    if (error_flag || EXE_CCFC_TIME_MIN > time || EXE_CCFC_TIME_MAX < time) {
                        Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
                        return AT_CME_ERROR;
                    }
                }

                /* Last Parameter received. Check if the user by mistake sends more parameters! */
                if (!parser_p->NoMoreParams) {
                    Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
                    return AT_CME_ERROR;
                }

                /* Some post extraction validation */
                if (NULL != number_p) {
                    StrParam_t tmp_str_p = number_p;
                    IntParam_t implied_type = 0;
                    IntParam_t phone_number_length = 0;
                    char test_str[2];

                    /* Validate type; determine if international phone numbering plan */
                    if ('+' == number_p[0]) {
                        implied_type = EXE_CCFC_NUM_TYPE_PLUS;
                        tmp_str_p++; /* increase pointer for next validation step to skip + character */
                    } else {
                        implied_type = EXE_CCFC_NUM_TYPE_NO_PLUS;
                    }

                    if (EXE_CCFC_MODE_REGISTRATION == mode) {
                        if (EXE_CCFC_TYPE_NONE == type) {
                            type = implied_type;
                        } else if (EXE_CCFC_NUM_TYPE_PLUS == implied_type && implied_type != type) {
                            /* Here we have an international number but the type entered by the client does not match that */
                            Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
                            return AT_CME_ERROR;
                        }
                    }

                    /* Validate length of phone number */
                    phone_number_length = strlen(number_p);

                    if (EXE_CCFC_MAX_NUM_DIGITS < phone_number_length) {
                        Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
                        return AT_CME_ERROR;
                    }

                    /* evaluate phone number so it contain only BCD digits as per spec. */
                    test_str[1] = '\0';

                    while ('\0' != *tmp_str_p) {
                        test_str[0] = *tmp_str_p;

                        if (NULL == strstr(EXE_CCFC_ALLOWED_DIGITS, test_str)) {
                            error_flag = true;
                            break;
                        }

                        tmp_str_p++;
                    }

                    if (error_flag) {
                        /* Here we have an international number but the type entered by the client does not match that */
                        Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
                        return AT_CME_ERROR;
                    }
                } else if (EXE_CCFC_MODE_REGISTRATION == mode) {
                    /* When doing REGISTRATION the phone number must be != NULL, otherwise the registration make no sense.*/
                    Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
                    return AT_CME_ERROR;
                }

                /* If TIME for reason NO REPLY when making REGISTRATION is not set, we must force a default */
                if (EXE_CCFC_MODE_REGISTRATION == mode && EXE_CCFC_REASON_NO_REPLY == reason && 0 == time) {
                    time = EXE_CCFC_TIME_DEFAULT;
                }

                ccfc_data.class = class;
                ccfc_data.mode = mode;

                if (NULL != number_p) {
                    ccfc_data.number_p = (char *)calloc(1, strlen(number_p) + 1);
                    strcpy(ccfc_data.number_p, (char *)number_p);
                } else {
                    ccfc_data.number_p = NULL;
                }

                ccfc_data.reason = reason;
                ccfc_data.time = time;
                ccfc_data.type = type;

                exe_res =  exe_request(exe_p, EXE_CCFC, (void *)&ccfc_data,
                                       (void *)parser_p, &parser_p->request_handle);

                if (EXE_PENDING == exe_res) {
                    result = AT_PENDING;
                } else {
                    Parser_SetCMEE_ErrorType(parser_p, CMEE_UNKNOWN);
                }

                free(ccfc_data.number_p);

            } else {
                Parser_SetCMEE_ErrorType(parser_p, CMEE_UNKNOWN);
            }
        } else {
            exe_ccfc_head_t *ccfc_head_p = ((exe_ccfc_head_t *)message->data_p);

            if (NULL != ccfc_head_p) {
                if (EXE_SUCCESS == message->exe_result_code) {
                    exe_ccfc_data_t *ccfc_data_p = ccfc_head_p->data_p;

                    if (NULL != ccfc_data_p) { /* this means that we get response from +CCFC=x,2 QUERY */
                        bool all_disabled = true;

                        while (NULL != ccfc_data_p) {
                            all_disabled &= (EXE_CCFC_STATUS_DISABLED == ccfc_data_p->status);
                            ccfc_data_p = ccfc_data_p->next_p;
                        }

                        ccfc_data_p = ccfc_head_p->data_p;

                        while (NULL != ccfc_data_p) {
                            if (EXE_CCFC_STATUS_ENABLED == ccfc_data_p->status)  {
                                if (NULL != ccfc_data_p->number_p) {
                                    /*
                                     * TIME is not interesting if there is no
                                     * number registered to the REASON
                                     */
                                    if (EXE_CCFC_REASON_NO_REPLY == ccfc_data_p->reason) {
                                        sprintf((char *)info_text, ": %d,%d,\"%s\",%d,,,%d",
                                                ccfc_data_p->status,
                                                ccfc_data_p->class,
                                                ccfc_data_p->number_p,
                                                ccfc_data_p->type,
                                                ccfc_data_p->time);
                                    } else {
                                        sprintf((char *)info_text, ": %d,%d,\"%s\",%d",
                                                ccfc_data_p->status,
                                                ccfc_data_p->class,
                                                ccfc_data_p->number_p,
                                                ccfc_data_p->type);
                                    }
                                    Parser_SendMultipleResponses(parser_p,
                                            AT_NO_OVERRIDE, info_text,
                                            NULL == ccfc_data_p->next_p);
                                }
                            } else if (all_disabled ||
                                    (NULL != ccfc_data_p->number_p && strlen(ccfc_data_p->number_p) > 0)) {
                                /*
                                 * The response line for 'not active' case
                                 * (<status>=0) should be returned only if
                                 * service is not active for any <class>,
                                 * unless there is a number stored so it can be
                                 * displayed in the UI.
                                 */
                                if (NULL != ccfc_data_p->number_p) {
                                    sprintf((char *)info_text, ": %d,%d,\"%s\",%d",
                                            ccfc_data_p->status,
                                            ccfc_data_p->class,
                                            ccfc_data_p->number_p,
                                            ccfc_data_p->type);
                                } else {
                                    sprintf((char *)info_text, ": %d,%d",
                                            ccfc_data_p->status,
                                            ccfc_data_p->class);
                                }
                                Parser_SendMultipleResponses(parser_p,
                                        AT_NO_OVERRIDE, info_text,
                                        NULL == ccfc_data_p->next_p);
                            }
                            ccfc_data_p = ccfc_data_p->next_p;
                        }
                    } else {
                        Parser_SetCMEE_ErrorType(parser_p, CMEE_UNKNOWN);
                        result = AT_CME_ERROR;
                    }
                    result = AT_OK;
                } else {
                    Parser_SetCMEE_ErrorType(parser_p, ccfc_head_p->error_code);
                    result = AT_CME_ERROR;
                }
            } else {
                Parser_SetCMEE_ErrorType(parser_p, CMEE_UNKNOWN);
                result = AT_CME_ERROR;
            }
        }

        break;

    case AT_MODE_TEST:
        strncpy((char *) info_text, ": (0-5)", 8);
        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        result = AT_OK;
        break;
    default:
        break;
    }

    return result;
}

/*
 * Handle CUSD events sent from modem. Compose an AT string and broadcast it to the clients.
 */
void handle_incoming_CUSD_event(exe_cusd_response_t *response_p)
{
    AT_ParserState_s *parser_p;
    unsigned char *ussd_result = NULL;
    size_t ussd_length = 0;
    AT_CommandLine_t info_text = NULL;
    unsigned char entry;
    atc_charset_t input_charset = ATC_CHARSET_GSM7;

    if (NULL == response_p || NULL == response_p->ussd_str_p) {
        ATC_LOG_E("%s: received NULL parameters", __FUNCTION__);
        goto exit;
    }

    ATC_LOG_I("%s: ussd event type: %d", __FUNCTION__, response_p->type);
    ATC_LOG_I("%s: ussd received type: %d and suppress_ussd_on_end: %d", __FUNCTION__, response_p->received_type, response_p->suppress_ussd_on_end);

    /* Interpretation of dcs, Data Coding Scheme:
     *   bits 7..4 == 0000 -> GSM7
     *   bits 7..4 != 0000: bit 2 == 0 -> GSM7
     *                      bit 2 == 1 -> GSM8
     *                      bit 3 == 1 -> UCS2
     */
    if (response_p->dcs < 16 || !(response_p->dcs & 0x0c)) {
        ATC_LOG_I("%s: Text encoding: GSM7, dcs: %d", __FUNCTION__, response_p->dcs);
        input_charset = ATC_CHARSET_GSM7;
    } else if (response_p->dcs & 0x04) {
        ATC_LOG_I("%s: Text encoding: GSM8, dcs: %d", __FUNCTION__, response_p->dcs);
        input_charset = ATC_CHARSET_GSM8;
    } else if (response_p->dcs & 0x08) {
        ATC_LOG_I("%s: Text encoding: UCS2, dcs: %d", __FUNCTION__, response_p->dcs);
        input_charset = ATC_CHARSET_UCS2;
    } else {
        ATC_LOG_E("%s: Unsupported data coding scheme, dcs: %d", __FUNCTION__, response_p->dcs);
        goto exit;
    }

    /* Use MAX_USSD_LENGTH instead of guessing the converted size. */
    info_text = (AT_CommandLine_t)calloc(1, MAX_USSD_LENGTH);
    ussd_result = (unsigned char *)calloc(1, MAX_USSD_LENGTH);

    if (NULL == info_text || NULL == ussd_result) {
        goto exit;
    }

    /* Loop through all parser states to send on any subscribed channels. */
    for (entry = 0; entry < MAX_NO_OF_OPEN_CHANNELS; entry++) {
        int n = 0;
        parser_p = ParserStateTable_GetParserState_ByEntry(entry);

        if (parser_p->CUSD) {
            ussd_length = 0;
            memset(ussd_result, 0, MAX_USSD_LENGTH);
            memset(info_text, 0, MAX_USSD_LENGTH);

            if (response_p->length > 0) {

                n = atc_string_convert(input_charset, (uint8_t *)response_p->ussd_str_p, response_p->length,
                                       parser_p->cscs_charset, (uint8_t *)ussd_result, &ussd_length);

                if (0 == n) {
                    ATC_LOG_E("String conversion failed");
                    goto exit;
                }

                n = snprintf((char *)info_text, 10 + ussd_length +  5,  /* <m>,<str>,<dcs> */
                             "+CUSD: %d,\"%s\",%d",
                             response_p->type,
                             ussd_result,
                             response_p->dcs);
            } else {
                n = snprintf((char *)info_text, 10,  /* <m> */
                             "+CUSD: %d",
                             response_p->type);
            }

            if (n < 0) {
                ATC_LOG_E("%s: Error building info text", __FUNCTION__);
                goto exit;
            }

            if (n >= MAX_USSD_LENGTH) {
                ATC_LOG_I("%s: Warning truncated string", __FUNCTION__);
            }

            /* Check if me initiated and type is USSD_END, which is 2*/
            if ((response_p->me_initiated == 1) && (response_p->type == EXE_USSD_TYPE_USSD_SESSION_TERMINATED)) {
                /* ME initiated the cancel_ussd then dont send a unsol_ussd. */
                ATC_LOG_D("ME initiated the USSD abort, no unsolicited CUSD is sent");
            } else if ((response_p->suppress_ussd_on_end == TRUE) && (response_p->received_type == EXE_USSD_RECEIVED_TYPE_GSM_USSD_END)){
                /* For SAT initiated USSD request when the receive type in USSD Received Indication is SS_GSM_USSD_END then dont send a unsol_ussd . */
                ATC_LOG_D("SAT initiated and the receive type is SS_GSM_USSD_END , no unsolicited CUSD is sent");
            } else {
                ATC_LOG_D("USSD abort was initiated by network, unsolicited CUSD is sent");
                parser_send_unsolicited(parser_p, AT_TEXT_RESPONSE, info_text);
            }
        }
    }

exit:
    free(info_text);
    free(ussd_result);
}


/*
 * Handle incoming CCWA event
 */
void handle_incoming_CCWA_event(exe_ccwa_response_t *response)
{
    AT_ParserState_s *parser_p;
    unsigned char entry;
    AT_CommandLine_t infotext = (unsigned char *)alloca(SIZE_OF_CCWA_RESPONSE);

    if (!infotext) {
        return;
    }

    memset(infotext, 0, SIZE_OF_CCWA_RESPONSE);
    *infotext = END_OF_STRING;
    strcat((char *)infotext, "+CCWA: \"");

    if ((145 == response->type)  /* 145 == INTERNATIONAL */
            && (*response->number_p != '+')) {
        strcat((char *)infotext, "+");
    }

    strcat((char *)infotext, response->number_p);
    AT_AddValue(infotext, "\",000", response->type);
    AT_AddValue(infotext, ",0", response->class);

    for (entry = 0; entry < MAX_NO_OF_OPEN_CHANNELS; entry++) {
        parser_p = ParserStateTable_GetParserState_ByEntry(entry);

        if (parser_p->CCWA_n) {
            parser_send_unsolicited(parser_p, AT_TEXT_RESPONSE, infotext);
        }
    }
}
