/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <atc_handlers.h>
#include <atc_config.h>
#include <atc_log.h>
#include <atc_exe_glue.h>
#include <exe.h>
#include <atc_parser.h>
#include <atc.h>
#include <atc_parser_util.h>
#include <c_type.h>

#define SIZE_OF_CGEV_RESPONSE (16+MAX_SIZE_OF_PDP_TYPE+MAX_SIZE_OF_PDP_ADDRESS+4) /* "+CGEV: NW DETACH" + pdp type + pdp address + cid */
/* TODO: max size of pdp address and pdp type? */
#define SIZE_OF_CR_RESPONSE (10) /* "+CR: GPRS" */

static void create_eppsd_xml_response(exe_pscc_connection_response_t *connection_response_p, AT_CommandLine_t InfoText);
static void delete_cgdcont_response_list(exe_cgdcont_response_t *cgdcont_response_p);
static void free_cgact_set_list(exe_cgact_set_request_t *cgact_p);
static void free_cgcmod_list(exe_cgcmod_set_request_t *cgcmod_p);
static void get_active_contexts(exe_cgact_read_response_t **response_data_pp);
static bool copy_cid_from_cgact_to_cgcmod_list(exe_cgact_read_response_t *cgact_p, exe_cgcmod_set_request_cid_t **cgcmod_pp);
static bool match_input_active_cids(exe_cgcmod_set_request_cid_t *, exe_cgact_read_response_t *);
static bool parse_pdp_qos_params(AT_ParserState_s *parser_p, exe_pdp_qos_t *pdp_qos, bool allow_defaults);
static bool parse_pdp_qos_2G_params(AT_ParserState_s *parser_p, exe_pdp_qos_2G_t *pdp_qos_p);
bool add_cgpaddr_cid_to_list(int cid, exe_cgpaddr_cid_t **exe_cgpaddr_cid_pp);
void free_cgpaddr_cid_list(exe_cgpaddr_cid_t *exe_cgpaddr_cid_p);
static void create_pdp_qos_read_response(AT_CommandLine_t info_text,
        exe_pdp_qos_response_t *current_p,
        char *at_cmd,
        bool display_last_params);
static void create_pdp_qos_2G_read_response(AT_CommandLine_t info_text,
        exe_pdp_qos_2G_response_t *current_p,
        char *at_cmd);
static void create_pdp_qos_test_response(AT_CommandLine_t info_text, bool allow_defaults, char* bearer);
static void create_pdp_qos_2G_test_response(AT_CommandLine_t info_text, char* bearer);
bool check_exponential(StrParam_t string, exe_exponential_t *allowed_values, int allowed_values_length);

/*
 *===============================================================================
 *
 *  Function: AT_PLUS_CGACT_Handle
 *
 *  Input:    Parser_p   - Pointer to parser to which AT protocol response
 *                         eventually shall be sent.
 *
 *  Output:   Parser_p   - Pointer to submitted parser, with state
 *                         Pending changed to true.
 *
 *  OUTPUT:  Parser->
 *           Pos        - The current position in the command line.
 *
 *  RETURNS: A status code for the execution of the command is returned:
 *           AT_OK        - The command was successfully handled.
 *           AT_ERROR     - There was a general error.
 *           AT_CME_ERROR - There was a GSM specific error, the error code is
 *                          returned in info_text.
 *
 *===============================================================================
 */
AT_Command_e AT_PLUS_CGACT_Handle(AT_ParserState_s *parser_p,
                                  AT_CommandLine_t info_text,
                                  AT_ResponseMessage_t message)
{
    AT_Command_e result = AT_CME_ERROR;
    exe_request_result_t exe_res = EXE_FAILURE;
    exe_t *exe_p = NULL;
    exe_cgact_set_request_t *cgact_p = NULL;

    switch (parser_p->Mode) {
    case AT_MODE_SET: {
        if (!parser_p->IsPending) {
            bool error_flag = false;
            int state;
            int cid;
            exe_cgact_set_request_cid_t *previous_p = NULL;
            exe_cgact_set_request_cid_t *tmp_p = NULL;

            state = Parser_GetIntParam(parser_p, &error_flag, NULL);

            if (true == error_flag || !(1 == state || 0 == state)) {
                return AT_ERROR;
            }

            cgact_p = (exe_cgact_set_request_t *)calloc(1, sizeof(exe_cgact_set_request_t));

            if (NULL == cgact_p) {
                return AT_ERROR;
            }

            cgact_p->state = state;
            cgact_p->cid_p = NULL;

            if (parser_supports_async_cgact(parser_p)) {
                cgact_p->async_mode = EXE_CGACT_ASYNC_MODE_ASYNCHRONOUS;
            } else {
                cgact_p->async_mode = EXE_CGACT_ASYNC_MODE_SYNCHRONOUS;
            }

            /* add 0 or more cids. No cids means all valid cids */
            while (!parser_p->NoMoreParams) {
                tmp_p = (exe_cgact_set_request_cid_t *)malloc(sizeof(exe_cgact_set_request_cid_t));
                tmp_p->next_p = NULL;

                if (NULL == previous_p) {
                    cgact_p->cid_p = tmp_p;
                    previous_p = cgact_p->cid_p;
                } else {
                    previous_p->next_p = tmp_p;
                }

                tmp_p->cid = Parser_GetIntParam(parser_p, &error_flag, NULL);

                if ((tmp_p->cid < MIN_CID) || (tmp_p->cid > MAX_CID) || error_flag) {
                    Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
                    result = AT_CME_ERROR;
                    goto exit;
                }
            }

            exe_p = atc_get_exe_handle();
            exe_res = exe_request(exe_p, EXE_CGACT, (void *)cgact_p,
                                  (void *)parser_p, &parser_p->request_handle);
        } else {
            /* Pending */
            exe_res = message->exe_result_code;
        }

        result = parser_handle_exe_result(exe_res, parser_p);
        break;
    }
    case AT_MODE_READ:

        if (!parser_p->IsPending) {

            exe_p = atc_get_exe_handle();
            exe_res = exe_request(exe_p, EXE_CGACT_READ, NULL,
                                  (void *)parser_p, &parser_p->request_handle);
        } else if (NULL != message) {
            exe_res = message->exe_result_code;
            sprintf((char *)info_text, ": ");
            exe_cgact_read_response_t *response_data_p = (exe_cgact_read_response_t *)message->data_p;
            if (NULL == response_data_p) {
                /* Return default values */
                AT_AddValue(info_text, "0,", 1);
                AT_AddValue(info_text, "0", 0);
            } else {
                while (NULL != response_data_p) {
                    exe_cgact_read_response_t *next_p = response_data_p->next_p;
                    AT_AddValue(info_text, "0,", response_data_p->cid);
                    AT_AddValue(info_text, "0", response_data_p->state);
                    response_data_p = next_p;

                    if (NULL != response_data_p) {
                        strcat((char *)info_text, "\r\n+CGACT: ");
                    }
                }
            }

            Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        }

        result = parser_handle_exe_result(exe_res, parser_p);

        break;
    case AT_MODE_TEST:
        strcpy((char *) info_text, ": (0-1)");
        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        result = AT_OK;
        break;
    default:
        break; /* avoid LINT info */
    }

exit:
    atc_hand_free_cgact_set_list(cgact_p);

    return result;
}


/*
 *===============================================================================
 *
 *  Function: AT_PLUS_CGATT_Handle
 *
 *  Input:    parser_p   - Pointer to parser to which AT protocol response
 *                         eventually shall be sent.
 *
 *  Output:   parser_p   - Pointer to submitted parser, with state
 *                         Pending changed to true.
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
 *===============================================================================
 */
AT_Command_e AT_PLUS_CGATT_Handle(AT_ParserState_s *parser_p,
                                  AT_CommandLine_t info_text,
                                  AT_ResponseMessage_t message)
{
    AT_Command_e result = AT_ERROR;

    if (!parser_p->IsPending) {
        switch (parser_p->Mode) {
        case AT_MODE_DO: { /* if no parameter is specified the default value is 1 (attached) */
            ATC_LOG_I("Using default +CGATT value 1");
            exe_cgatt_request_data_t data = { 1 }; /* Default GPRS state is Attach (1) */

            exe_request_result_t executor_result = exe_request(atc_get_exe_handle(),
                                                   EXE_CGATT_SET_GPRS_STATE,
                                                   &data,
                                                   (void *)parser_p,
                                                   &parser_p->request_handle);

            if (EXE_PENDING == executor_result) {
                result = AT_PENDING;
            }

            break;
        }

        case AT_MODE_SET: {
            exe_cgatt_request_data_t data = { 1 }; /* Default GPRS state is Attach (1) */
            bool error_flag = false;

            data.state = Parser_GetIntParam(parser_p, &error_flag, NULL);

            if (error_flag) {
                ATC_LOG_E("Parameter parsing failed");
                Parser_SetCMEE_ErrorType(parser_p, CMEE_UNKNOWN);
                result = AT_CME_ERROR;
                break;
            }

            if (!parser_p->NoMoreParams) {
                ATC_LOG_E("Too many parameters");
                Parser_SetCMEE_ErrorType(parser_p,
                                         CMEE_INCORRECT_PARAMETERS);
                result = AT_CME_ERROR;
                break;
            }

            if (data.state > 1) {
                ATC_LOG_E("<state> is not valid");
                Parser_SetCMEE_ErrorType(parser_p,
                                         CMEE_INCORRECT_PARAMETERS);
                result = AT_CME_ERROR;
                break;
            } else {
                exe_request_result_t executor_result = exe_request(atc_get_exe_handle(),
                                                       EXE_CGATT_SET_GPRS_STATE,
                                                       &data,
                                                       (void *)parser_p,
                                                       &parser_p->request_handle);

                if (EXE_PENDING == executor_result) {
                    result = AT_PENDING;
                }
            }

            break;
        }

        case AT_MODE_READ: {
            exe_request_result_t executor_result = exe_request(atc_get_exe_handle(),
                                                   EXE_CGATT_GET_GPRS_STATE,
                                                   NULL,
                                                   (void *)parser_p,
                                                   &parser_p->request_handle);

            if (EXE_PENDING == executor_result) {
                result = AT_PENDING;
            }

            break;
        }

        case AT_MODE_TEST: {
            sprintf((char *) info_text, ": (0-1)");
            Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
            result = AT_OK;
            break;
        }

        default: {
            ATC_LOG_E("unexpected parser mode in request handling");
            break;
        }
        }
    } else { /* Pending */
        switch (parser_p->Mode) {
        case AT_MODE_DO:
        case AT_MODE_SET: {
            if (parser_p->abort) {
                /* Abort the ongoing execution. */
                exe_request_result_t result;

                result = exe_request_abort(atc_get_exe_handle(), parser_p->request_handle);

                if (EXE_SUCCESS != result) {
                    ATC_LOG_E("interrupt_set_GPRS_state: abort ongoing record failed.")        ;
                }
            } else if (NULL != message) {
                if (EXE_SUCCESS == ((AT_ResponseMessage_t) message)->exe_result_code) {
                    result = AT_OK;
                } else {
                    Parser_SetCMEE_ErrorType(parser_p, CMEE_UNSPEC_GPRS_ERROR);
                    result = AT_CME_ERROR;
                }
            } else {
                ATC_LOG_I("message is NULL");
            }

            break;
        }

        case AT_MODE_READ: {
            if (NULL != message) {
                if (EXE_SUCCESS == ((AT_ResponseMessage_t) message)->exe_result_code) {
                    exe_cgatt_response_data_t
                    *data_p = (exe_cgatt_response_data_t *) message->data_p;

                    if (NULL != data_p) {
                        int32_t n = snprintf((char *) info_text,
                                             4, /* ": <state> */
                                             ": %d",
                                             data_p->state);

                        if (n < 0) {
                            ATC_LOG_E("Could not write to response info");
                        } else {
                            if ((uint32_t) n >= 4) {
                                ATC_LOG_I("Warning truncation occurred");
                            }

                            info_text[4] = '\0';
                            Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
                            result = AT_OK;
                        }
                    } else {
                        ATC_LOG_E("no data in message");
                    }
                } else {
                    Parser_SetCMEE_ErrorType(parser_p,
                                             CMEE_UNSPEC_GPRS_ERROR);
                    result = AT_CME_ERROR;
                }
            } else {
                ATC_LOG_I("message is NULL");
            }

            break;
        }

        case AT_MODE_TEST:
        default: {
            ATC_LOG_E("unexpected parser mode in response handling");
            break;
        }
        }
    }

    return result;
}

/* Defines for CGCMOD handler substates:*/
#define CGCMOD_SUBSTATE_CALLED_CGACT 0
#define CGCMOD_SUBSTATE_CALLED_CGCMOD 1
/*
 *===============================================================================
 *
 *  Function: AT_PLUS_CGCMOD_Handle
 *
 *  Input:    parser_p   - Pointer to parser to which AT protocol response
 *                         eventually shall be sent.
 *
 *  Output:   parser_p   - Pointer to submitted parser, with state
 *                         Pending changed to true.
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
 *===============================================================================
 */
AT_Command_e AT_PLUS_CGCMOD_Handle(AT_ParserState_s *parser_p,
                                   AT_CommandLine_t InfoText,
                                   AT_ResponseMessage_t Message)
{
    AT_Command_e              result   = AT_ERROR;
    exe_request_result_t      exe_res  = EXE_SUCCESS;
    exe_t                    *exe_p    = NULL;
    exe_cgcmod_set_request_t *cgcmod_p = NULL;

    switch (parser_p->Mode) {
    case AT_MODE_SET: {
        if (!parser_p->IsPending) {
            /* list all connections */
            exe_p = atc_get_exe_handle();

            /* Call the handler for CGACT in order to get all the active pdp contexts. */
            exe_res = exe_request(exe_p, EXE_CGACT_READ, NULL,
                                  (void *)parser_p, &parser_p->request_handle);
            parser_p->sub_state = CGCMOD_SUBSTATE_CALLED_CGACT;

        } else {
            /* Check which call to the executor that has been pending. */
            if (CGCMOD_SUBSTATE_CALLED_CGACT == parser_p->sub_state) {
                /* Handle return values from EXE_CGACT_READ */
                bool error_flag = false;
                exe_cgcmod_set_request_cid_t *tmp_p      = NULL;
                exe_cgcmod_set_request_cid_t *previous_p = NULL;

                exe_cgact_read_response_t *response_data_p = (exe_cgact_read_response_t *)Message->data_p;
                get_active_contexts(&response_data_p);

                if (NULL == response_data_p) {
                    /* No active contexts exists, exit */
                    Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
                    result = AT_CME_ERROR;
                    goto exit;
                }

                /* Get input cids */
                cgcmod_p = (exe_cgcmod_set_request_t *) calloc(1, sizeof(exe_cgcmod_set_request_t));

                if (NULL == cgcmod_p) {
                    ATC_LOG_E("AT+CGCMOD: failed to alloc memory");
                    Parser_SetCMEE_ErrorType(parser_p, CMEE_MEMORY_FAILURE);
                    result = AT_CME_ERROR;
                    goto exit;
                }

                cgcmod_p->cid_p = NULL;

                /* If parser is at end of string, no input parameters exists */
                if (!(END_OF_STRING == *parser_p->Pos || AT_COMMAND_SEPARATOR == *parser_p->Pos)) {
                    while (!parser_p->NoMoreParams) {
                        tmp_p = (exe_cgcmod_set_request_cid_t *) calloc(1, sizeof(exe_cgcmod_set_request_cid_t));
                        tmp_p->next_p = NULL;

                        if (NULL == previous_p) {
                            cgcmod_p->cid_p = tmp_p;
                            previous_p = cgcmod_p->cid_p;
                        } else {
                            previous_p->next_p = tmp_p;
                            previous_p = tmp_p;
                        }

                        tmp_p->cid = Parser_GetIntParam(parser_p, &error_flag, NULL);

                        if ((MIN_CID > tmp_p->cid) || (MAX_CID < tmp_p->cid) || error_flag) {
                            ATC_LOG_E("AT+CGCMOD invalid CID param");
                            Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
                            result = AT_CME_ERROR;
                            goto exit;
                        }
                    }
                }

                if (NULL == cgcmod_p->cid_p) {
                    /* No cids means all active cids */
                    if (!copy_cid_from_cgact_to_cgcmod_list(response_data_p, &(cgcmod_p->cid_p))) {
                        ATC_LOG_E("AT+CGCMOD could not copy active cids");
                        Parser_SetCMEE_ErrorType(parser_p, CMEE_MEMORY_FAILURE);
                        result = AT_CME_ERROR;
                        goto exit;
                    }
                } else {
                    /* Each input cid must exist in active cid list */
                    if (!match_input_active_cids(cgcmod_p->cid_p, response_data_p)) {
                        result = AT_CME_ERROR;
                        Parser_SetCMEE_ErrorType(parser_p, CMEE_OPERATION_NOT_ALLOWED);
                        goto exit;
                    }
                }

                exe_p = atc_get_exe_handle();
                exe_res = exe_request(exe_p,
                                      EXE_CGCMOD,
                                      (void *) cgcmod_p,
                                      (void *) parser_p,
                                      &parser_p->request_handle);
                parser_p->sub_state = CGCMOD_SUBSTATE_CALLED_CGCMOD;
            } else {
                /* Handle return values from EXE_CGCMOD */

                /* Set status code from EXE_CGCMOD call */
                exe_res = Message->exe_result_code;
            }
        }

        break;
    }
    case AT_MODE_TEST: {
        /* AT+CGCMOD=? is almost identical to AT+CGACT? but
         * it lists only the active contexts. */

        if (!parser_p->IsPending) {
            exe_p = atc_get_exe_handle();
            exe_res = exe_request(exe_p, EXE_CGACT_READ, NULL,
                                  (void *) parser_p, &parser_p->request_handle);
        } else if (NULL != Message) {
            bool first_print = true;
            exe_res = Message->exe_result_code;
            sprintf((char *)InfoText, ": ");
            exe_cgact_read_response_t *response_data_p = (exe_cgact_read_response_t *)Message->data_p;

            while (NULL != response_data_p) {
                exe_cgact_read_response_t *next_p = response_data_p->next_p;

                if (1 == response_data_p->state) {
                    if (first_print) {
                        AT_AddValue(InfoText, "(0", response_data_p->cid);
                    } else {
                        AT_AddValue(InfoText, ",0", response_data_p->cid);
                    }

                    first_print = false;
                }

                response_data_p = next_p;
            }

            /* end of printout if any active CIDS exist */
            if (!first_print) {
                strcat((char *)InfoText, ")");
            }

            Parser_SendResponse(parser_p, AT_NO_OVERRIDE, InfoText);
        }

        break;
    }
    default:
        ATC_LOG_E("AT+CGCMOD received invalid AT mode command");
        result = AT_ERROR;
        goto exit;
    }

    result = parser_handle_exe_result(exe_res, parser_p);

exit:
    free_cgcmod_list(cgcmod_p);
    return result;
}

/*
 *===============================================================================
 *
 *  Function: AT_PLUS_CGDCONT_Handle
 *
 *
 *  Input:    Parser_p   - Pointer to parser to which AT protocol response
 *                         eventually shall be sent.
 *
 *  Output:   Parser_p   - Pointer to submitted parser, with state
 *                         Pending changed to true.
 *
 *  OUTPUT:  Parser->
 *           Pos        - The current position in the command line.
 *
 *  RETURNS: A status code for the execution of the command is returned:
 *           AT_OK        - The command was successfully handled.
 *           AT_ERROR     - There was a general error.
 *           AT_CME_ERROR - There was a GSM specific error, the error code is
 *                          returned in info_text.
 *
 *===============================================================================
 */
AT_Command_e AT_PLUS_CGDCONT_Handle(AT_ParserState_s *parser_p,
                                    AT_CommandLine_t info_text,
                                    AT_ResponseMessage_t Message)
{
    AT_Command_e result = AT_ERROR;
    exe_request_result_t exe_res = EXE_SUCCESS;

    switch (parser_p->Mode) {
    case AT_MODE_SET: {
        exe_t *exe = NULL;
        int cid;
        int pdp_type;
        StrParam_t pdp_type_p2;
        StrParam_t apn_p = DEFAULT_APN;
        StrParam_t pdp_addr_p = DEFAULT_PDP_ADDR;
        int d_comp = 0; /* Default */
        int h_comp = 0; /* Default */
        bool error_flag = false;
        exe_cgdcont_t cgdcont;

        /* Just in case */
        memset(&cgdcont, 0x00, sizeof(exe_cgdcont_t));
        cgdcont.pdp_type = EXE_PDP_TYPE_INVALID;

        /* A little strange, but valid, case, no in parameter given */
        if (*parser_p->Pos == END_OF_STRING || *parser_p->Pos == AT_COMMAND_SEPARATOR) {
            parser_p->Pos++;
        } else {
            if (!parser_p->NoMoreParams) {
                cid = Parser_GetIntParam(parser_p, &error_flag, (IntParam_t *)&cid);

                if ((cid < MIN_CID) || (cid > MAX_CID) || error_flag) {
                    /* Invalid cid range */
                    return AT_ERROR;
                }

                cgdcont.cid = cid;
            }

            if (!parser_p->NoMoreParams) {
                pdp_type_p2 = Parser_GetStrParam(parser_p, &error_flag, (StrParam_t)NULL);

                if (error_flag || (pdp_type_p2 == NULL)) {
                    return AT_ERROR;
                }

                if (2 == strlen(pdp_type_p2) &&
                        'I' == to_upper(pdp_type_p2[0]) &&
                        'P' == to_upper(pdp_type_p2[1])) {
                    cgdcont.pdp_type = EXE_PDP_TYPE_IPV4;
                } else if (4 == strlen(pdp_type_p2) &&
                           'I' == to_upper(pdp_type_p2[0]) &&
                           'P' == to_upper(pdp_type_p2[1]) &&
                           'V' == to_upper(pdp_type_p2[2]) &&
                           '6' == pdp_type_p2[3]) {
                    cgdcont.pdp_type = EXE_PDP_TYPE_IPV6;
                } else if (6 == strlen(pdp_type_p2) &&
                           'I' == to_upper(pdp_type_p2[0]) &&
                           'P' == to_upper(pdp_type_p2[1]) &&
                           'V' == to_upper(pdp_type_p2[2]) &&
                           '4' == pdp_type_p2[3] &&
                           'V' == to_upper(pdp_type_p2[4]) &&
                           '6' == pdp_type_p2[5]) {
                    cgdcont.pdp_type = EXE_PDP_TYPE_IPV4V6;
                } else if (4 == strlen(pdp_type_p2) &&
                           'T' == to_upper(pdp_type_p2[0]) &&
                           'E' == to_upper(pdp_type_p2[1]) &&
                           'S' == to_upper(pdp_type_p2[2]) &&
                           'T' == to_upper(pdp_type_p2[3])) {
                    cgdcont.pdp_type = EXE_PDP_TYPE_TEST;
                } else {
                    return AT_ERROR;
                }
            }

            cgdcont.apn_p = Parser_GetStrParam(parser_p, &error_flag, apn_p);

            if (error_flag || (cgdcont.apn_p == NULL)) {
                return AT_ERROR;
            }

            cgdcont.pdp_addr_p = Parser_GetStrParam(parser_p, &error_flag, pdp_addr_p);

            if (error_flag || (cgdcont.pdp_addr_p == NULL)) {
                return AT_ERROR;
            }

            if (((EXE_PDP_TYPE_IPV6 == cgdcont.pdp_type) ||
                (EXE_PDP_TYPE_IPV4V6 == cgdcont.pdp_type)) &&
                0 != strcmp(cgdcont.pdp_addr_p, "")) {
                ATC_LOG_E("AT+CGDCONT PDP_addr is not supported for IPV6 and IPV4V6");
                Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
                return AT_CME_ERROR;
            }

            if (!parser_p->NoMoreParams) {
                d_comp = Parser_GetIntParam(parser_p, &error_flag, (IntParam_t *)&d_comp);

                if (1 < d_comp || error_flag) {
                    /* Invalid d_comp range */
                    return AT_ERROR;
                }

                cgdcont.d_comp = d_comp;
            }

            if (!parser_p->NoMoreParams) {
                h_comp = Parser_GetIntParam(parser_p, &error_flag, (IntParam_t *)&h_comp);

                if (1 < h_comp || error_flag) {
                    /* Invalid h_comp range */
                    return AT_ERROR;
                }

                cgdcont.h_comp = h_comp;
            }

            if (!parser_p->NoMoreParams) {
                /* Too many parameters */
                return AT_ERROR;
            }
        }

        exe = atc_get_exe_handle();
        exe_res = exe_request(exe, EXE_CGDCONT, (void *)&cgdcont, (void *)parser_p, &parser_p->request_handle);
        result = parser_handle_exe_result(exe_res, parser_p);
        break;
    }

    case AT_MODE_READ:

        if (!parser_p->IsPending) {
            exe_t *exe_p;
            exe_p = atc_get_exe_handle();
            exe_res = exe_request(exe_p, EXE_CGDCONT_READ, NULL,
                                  (void *)parser_p, &parser_p->request_handle);
        } else {
            exe_cgdcont_response_t *cgdcont_response_p = (exe_cgdcont_response_t *)Message->data_p;

            if (Message->exe_result_code == EXE_SUCCESS) {
                exe_cgdcont_response_t *current_p = cgdcont_response_p;
                sprintf((char *) info_text, ": ");

                if (NULL == current_p) {
                    /* Return default account */
                    AT_AddValue(info_text, "0,", 1);
                    strcat((char *) info_text, "\"IP\"");
                    strcat((char *) info_text, ",\"");
                    strcat((char *) info_text, "");
                    strcat((char *) info_text, "\",\"");
                    strcat((char *) info_text, "");
                    strcat((char *) info_text, "\",");
                    AT_AddValue(info_text, "0,", 0);
                    AT_AddValue(info_text, "0", 0);
                } else {
                    while (NULL != current_p) {
                        AT_AddValue(info_text, "0,", current_p->cgdcont_p->cid);

                        if (current_p->cgdcont_p->pdp_type == EXE_PDP_TYPE_IPV4) {
                            strcat((char *) info_text, "\"IP\"");
                        }

                        if (current_p->cgdcont_p->pdp_type == EXE_PDP_TYPE_IPV6) {
                            strcat((char *) info_text, "\"IPV6\"");
                        }

                        if (current_p->cgdcont_p->pdp_type == EXE_PDP_TYPE_IPV4V6) {
                            strcat((char *) info_text, "\"IPV4V6\"");
                        }

                        strcat((char *) info_text, ",\"");

                        if (current_p->cgdcont_p->apn_p != NULL) {
                            strcat((char *) info_text,
                                   current_p->cgdcont_p->apn_p);
                        }

                        strcat((char *) info_text, "\",\"");

                        if (current_p->cgdcont_p->pdp_addr_p != NULL) {
                            strcat((char *) info_text,
                                   current_p->cgdcont_p->pdp_addr_p);
                        }

                        if (current_p->cgdcont_p->ipv6_pdp_addr_p != NULL) {
                            if (current_p->cgdcont_p->pdp_addr_p != NULL)
                                strcat((char *) info_text, "\",\"");
                            strcat((char *) info_text,
                                   current_p->cgdcont_p->ipv6_pdp_addr_p);
                        }

                        strcat((char *) info_text, "\",");

                        AT_AddValue(info_text, "0,", current_p->cgdcont_p->d_comp);
                        AT_AddValue(info_text, "0", current_p->cgdcont_p->h_comp);

                        if (current_p->next_p != NULL) {
                            Parser_SendMultipleResponses(parser_p, AT_NO_OVERRIDE, info_text, false);
                            sprintf((char *) info_text, ": ");
                        }

                        current_p = current_p->next_p;
                    }
                }
            } else {
                result = parser_handle_exe_result(Message->exe_result_code, parser_p);
                break;
            }

            Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);

            result = AT_OK;
        }

        result = parser_handle_exe_result(exe_res, parser_p);
        break;

    case AT_MODE_TEST:
        sprintf((char *)info_text, ": (%d-%d),\"IP\",,,(0-1),(0-1)", MIN_CID, MAX_CID);
        Parser_SendMultipleResponses(parser_p, AT_NO_OVERRIDE, info_text, false);
        sprintf((char *)info_text, ": (%d-%d),\"IPV6\",,,(0-1),(0-1)", MIN_CID, MAX_CID);
        Parser_SendMultipleResponses(parser_p, AT_NO_OVERRIDE, info_text, false);
        sprintf((char *)info_text, ": (%d-%d),\"IPV4V6\",,,(0-1),(0-1)", MIN_CID, MAX_CID);
        Parser_SendMultipleResponses(parser_p, AT_NO_OVERRIDE, info_text, true);

        result = AT_OK;
        break;
    default:
        break;
    }

    return result;
}

/*
 *===============================================================================
 *
 *  Function: AT_PLUS_CGPADDR_Handle
 *
 *  Input:    Parser_p   - Pointer to parser to which AT protocol response
 *                         eventually shall be sent.
 *
 *  Output:   Parser_p   - Pointer to submitted parser, with state
 *                         Pending changed to true.
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
 * DESCRIPTION: Show PDP Address.
 *
 *===============================================================================
 */
AT_Command_e AT_PLUS_CGPADDR_Handle(AT_ParserState_s *parser_p,
                                    AT_CommandLine_t info_text,
                                    AT_ResponseMessage_t message)
{
    AT_Command_e result = AT_ERROR;
    exe_request_result_t exe_res = EXE_SUCCESS;
    exe_t *exe_p = NULL;
    exe_cgpaddr_cid_t *cgpaddr_cid_p = NULL;
    ATC_LOG_I("enter");

    switch (parser_p->Mode) {
    case AT_MODE_SET:

        if (!parser_p->IsPending) {
            bool error_flag = false;

            int cid;

            if (!(END_OF_STRING == *parser_p->Pos || AT_COMMAND_SEPARATOR
                    == *parser_p->Pos)) {
                while (!parser_p->NoMoreParams) {
                    cid = Parser_GetIntParam(parser_p, &error_flag, NULL);

                    if ((cid < MIN_CID) || (cid > MAX_CID) || error_flag) {
                        Parser_SetCMEE_ErrorType(parser_p,
                                                 CMEE_INCORRECT_PARAMETERS);
                        free_cgpaddr_cid_list(cgpaddr_cid_p);
                        return AT_CME_ERROR;
                    }

                    if (!add_cgpaddr_cid_to_list(cid, &cgpaddr_cid_p)) {
                        ATC_LOG_E("Error adding cid to list");
                        free_cgpaddr_cid_list(cgpaddr_cid_p);
                        return AT_ERROR;
                    }
                }
            }

            /* If no inparameter was found, send NULL as request data (cgpaddr_cid_p)
             * to get addresses for all defined cids. */
            exe_p = atc_get_exe_handle();
            exe_res = exe_request(exe_p, EXE_CGPADDR, (void *) cgpaddr_cid_p,
                                  (void *) parser_p, &parser_p->request_handle);
            free_cgpaddr_cid_list(cgpaddr_cid_p);

        } else { /* pending */
            exe_res = message->exe_result_code;
            result = parser_handle_exe_result(exe_res, parser_p);

            if (AT_OK != result) {
                ATC_LOG_E("Executor failed.");
            } else {
                exe_cgpaddr_response_t *response_data_p = (exe_cgpaddr_response_t *) message->data_p;
                exe_cgpaddr_response_t *tmp_p = response_data_p;
                sprintf((char *) info_text, ": ");

                while (NULL != tmp_p) {
                    AT_AddValue(info_text, "0,", tmp_p->cid);

                    if (NULL != tmp_p->pdp_address_p) {
                        AT_AddString(info_text, tmp_p->pdp_address_p);
                    }

                    if ((NULL != tmp_p->ipv6_pdp_address_p)) {
                        if (NULL != tmp_p->pdp_address_p) {
                            strcat((char *) info_text, ",");
                        }
                        AT_AddString(info_text, tmp_p->ipv6_pdp_address_p);
                    }
                    if (NULL != tmp_p->next_p) {
                        strcat((char *) info_text, "\r\n+CGPADDR: ");
                    }

                    tmp_p = tmp_p->next_p;
                }
            }

            Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        }

        break;
    case AT_MODE_TEST:

        /* List all defined cids. Almost identical to AT+CGACT? */
        if (!parser_p->IsPending) {
            exe_p = atc_get_exe_handle();
            exe_res = exe_request(exe_p, EXE_CGACT_READ, NULL,
                                  (void *)parser_p, &parser_p->request_handle);

        } else if (NULL != message) {
            bool first_print = true;
            exe_res = message->exe_result_code;
            sprintf((char *) info_text, ": ");
            exe_cgact_read_response_t *response_data_p = (exe_cgact_read_response_t *) message->data_p;

            while (NULL != response_data_p) {
                exe_cgact_read_response_t *next_p = response_data_p->next_p;

                if (first_print) {
                    AT_AddValue(info_text, "(0", response_data_p->cid);
                } else {
                    AT_AddValue(info_text, ",0", response_data_p->cid);
                }

                first_print = false;
                response_data_p = next_p;
            }

            /* end of printout if any active CIDS exist */
            if (!first_print) {
                strcat((char *) info_text, ")");
            }

            Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        }

        break;
    default:
        result = AT_ERROR;
        ATC_LOG_E("Invalid AT_MODE received");
        break;
    }

    result = parser_handle_exe_result(exe_res, parser_p);
    return result;
}

/*
 *===============================================================================
 *
 *  Function: CGEREP_Set
 *
 *  Input:    Parser_p   - Pointer to parser to which AT protocol response
 *                         eventually shall be sent.
 *
 *  Output:   Parser_p   - Pointer to submitted parser, with state
 *                         Pending changed to TRUE.
 *
 *===============================================================================
 */
AT_Command_e CGEREP_Set(AT_ParserState_s *parser_p)
{
    IntParam_t mode = 0;
    IntParam_t bfr  = 0;
    bool error_flag = false;

    /* Extract AT command arguments */
    mode = Parser_GetIntParam(parser_p, &error_flag, &mode);

    if (error_flag) {
        Parser_SetCMEE_ErrorType(parser_p, CMEE_UNKNOWN);
        return AT_CME_ERROR;
    }

    bfr = Parser_GetIntParam(parser_p, &error_flag, &bfr);
    /* No error check, setting bfr is optional */

    /* Last Parameter received. Check if the user by mistake sends more parameters! */
    if (!parser_p->NoMoreParams) {
        return AT_CME_ERROR;
    }

    if ((mode > 1) || (bfr > 0)) {
        Parser_SetCMEE_ErrorType(parser_p, CMEE_OPERATION_NOT_SUPPORTED);
        return AT_CME_ERROR;
    }

    /* Only brf value 0 is currently supported, so no need to store this value in the parser */
    parser_p->CGEV = mode;

    return AT_OK;
} /* End of CGEREP_Set */


/*
 *  Function: AT_PLUS_CGEREP_Handle
 *
 *  INPUT:   Parser_p   - Pointer to the current parser state.
 *           info_text   - Pointer to a string buffer to put information text.
 *           Response   - Pointer to the requester response
 *
 *  OUTPUT:  Parser->
 *           Pos        - The current position in the command line.
 *
 *  RETURNS: A status code for the execution of the command is returned:
 *           AT_OK        - The command was successfully handled.
 *           AT_ERROR     - There was a general error.
 *           AT_CME_ERROR - There was a GSM specific error, the error code is
 *                          returned in info_text.
 *
 *  Description:  +CGEREP=[<mode>[,<bfr>]]
 *
 * SET  - +CGEREP=[<mode>[,<bfr>]]
 *        Set command enables or disables sending of unsolicited result codes,
 *        +CGEV: XXX from ME to TE in the case of certain events occurring in the GPRS/Packet Domain ME or the network.
 * READ - +CGEREP?
 *        CGEREP: <mode>,<bfr>
 * TEST - +CGEREP=?
 *        +CGEREP: (list of supported <mode>s),(list of supported <bfr>s)
 *
 *  References:
 *
 *
 *===============================================================================
 */
AT_Command_e AT_PLUS_CGEREP_Handle(AT_ParserState_s *parser_p,
                                   AT_CommandLine_t info_text,
                                   AT_ResponseMessage_t Message)
{
    AT_Command_e result = AT_ERROR;

    switch (parser_p->Mode) {
    case AT_MODE_SET:

        if (!parser_p->IsPending) {
            result = CGEREP_Set(parser_p);
        } else {
            result = AT_ERROR;
        }

        break;

    case AT_MODE_READ:
        AT_AddRange(info_text, ": 0,0",
                    (IntParam_t) parser_p->CGEV,
                    (IntParam_t) 0); /* Only brf value 0 is currently supported */

        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);

        result = AT_OK;
        break;

    case AT_MODE_TEST:
        /* Send the range of supported values. */
        AT_AddRange(info_text, ": (0-0)", 0, 1);
        AT_AddValue(info_text, ",(0)", 0); /* Only brf value 0 is currently supported */
        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        result = AT_OK;
        break;

    default:
        Parser_SetCMEE_ErrorType(parser_p, CMEE_UNKNOWN);
        result = AT_CME_ERROR;
        break;
    } /* switch */

    return result;
}

/*
 * Support method for AT+CGQMIN & AT+CGQREQ.
 *
 * Sets info_text to contain the accepted values for the AT commands.
 * */
static void create_pdp_qos_2G_read_response(AT_CommandLine_t info_text,
        exe_pdp_qos_2G_response_t *current_p,
        char *at_cmd)
{
    int n = 0;

    n = sprintf((char *)info_text, ": ");

    while ((current_p != NULL) && (current_p->pdp_qos.cid != 0)) {
        n += sprintf((char *)info_text + n, "%d,", current_p->pdp_qos.cid);
        n += sprintf((char *)info_text + n, "%d,", current_p->pdp_qos.precedence);
        n += sprintf((char *)info_text + n, "%d,", current_p->pdp_qos.delay);
        n += sprintf((char *)info_text + n, "%d,", current_p->pdp_qos.reliability);
        n += sprintf((char *)info_text + n, "%d,", current_p->pdp_qos.peak);
        n += sprintf((char *)info_text + n, "%d", current_p->pdp_qos.mean);

        if (current_p->next_p != NULL) {
            n += sprintf((char *)info_text + n, "%s: ", at_cmd);
        }

        current_p = current_p->next_p;
    }
}

/*
 * Support method for AT+CGQMIN & AT+CGQREQ.
 *
 * Checks input parameters and sets default values for any missing parameters.
 *
 * */
static bool parse_pdp_qos_2G_params(AT_ParserState_s *parser_p, exe_pdp_qos_2G_t *pdp_qos_p)
{
    bool error_flag = false;

    IntParam_t precedence = 0;
    IntParam_t delay = 0;
    IntParam_t reliability = 0;
    IntParam_t peak = 0;
    IntParam_t mean = 0;

    int cid = Parser_GetIntParam(parser_p, &error_flag, NULL);

    if ((MIN_CID > cid) || (MAX_CID < cid) || error_flag) {
        /* Invalid cid range */
        ATC_LOG_E("Invalid cid range... cid=%d  (error_flag:%d)", cid, error_flag);
        return false;
    }

    pdp_qos_p->cid = cid;

    /* Parse precedence */
    if (!parser_p->NoMoreParams) {
        precedence = Parser_GetIntParam(parser_p, &error_flag, &precedence);

        if ((EXE_QOS2G_PROFILE_MAX_PRECEDENCE < precedence) || error_flag) {
            ATC_LOG_E("precedence out of range:%d", precedence);
            return false;
        }
    }

    pdp_qos_p->precedence = precedence;

    /* Parse delay */
    if (!parser_p->NoMoreParams) {
        delay = Parser_GetIntParam(parser_p, &error_flag, &delay);

        if ((EXE_QOS2G_PROFILE_MAX_DELAY < delay) || error_flag) {
            ATC_LOG_E("delay out of range:%d", delay);
            return false;
        }
    }

    pdp_qos_p->delay = delay;

    /* Parse reliability */
    if (!parser_p->NoMoreParams) {
        reliability = Parser_GetIntParam(parser_p, &error_flag, &reliability);

        if ((EXE_QOS2G_PROFILE_MAX_RELIABILITY < reliability) || error_flag) {
            ATC_LOG_E("reliability out of range:%d", reliability);
            return false;
        }
    }

    pdp_qos_p->reliability = reliability;

    /* Parse peak */
    if (!parser_p->NoMoreParams) {
        peak = Parser_GetIntParam(parser_p, &error_flag, &peak);

        if ((EXE_QOS2G_PROFILE_MAX_PEAK < peak) || error_flag) {
            ATC_LOG_E("peak out of range:%d", peak);
            return false;
        }
    }

    pdp_qos_p->peak = peak;

    /* Parse mean */
    if (!parser_p->NoMoreParams) {
        mean = Parser_GetIntParam(parser_p, &error_flag, &mean);

        if ((EXE_QOS2G_PROFILE_MAX_MEAN < mean && EXE_QOS2G_PROFILE_MAX_MEAN_EXTRA != mean) || error_flag) {
            ATC_LOG_E("mean out of range:%d", mean);
            return false;
        }
    }

    pdp_qos_p->mean = mean;

    return true;
}

/*===============================================================================
 *
 *  Function: AT_PLUS_CGQREQ_Handle
 *
 *
 *  INPUT:   Parser_p   - Pointer to the current parser state.
 *                 info_text   - Pointer to a string buffer to put information text.
 *                Response   - Pointer to the requester response
 *
 *
 *  OUTPUT:  Parser->
 *           Pos        - The current position in the command line.
 *
 *  RETURNS: A status code for the execution of the command is returned:
 *           AT_OK        - The command was successfully handled.
 *           AT_ERROR     - There was a general error.
 *
 *===============================================================================
 */
AT_Command_e AT_PLUS_CGQREQ_Handle(AT_ParserState_s *parser_p,
                                   AT_CommandLine_t info_text,
                                   AT_ResponseMessage_t message)
{
    AT_Command_e result = AT_ERROR;
    exe_request_result_t exe_res = EXE_SUCCESS;
    exe_pdp_qos_2G_response_t *response_p;

    switch (parser_p->Mode) {
    case AT_MODE_SET: {
        exe_t *exe = NULL;
        exe_pdp_qos_2G_t pdp_qos;

        memset(&pdp_qos, 0x00, sizeof(exe_pdp_qos_2G_t));

        if (!parse_pdp_qos_2G_params(parser_p, &pdp_qos)) {
            ATC_LOG_E("parsing of input failed.");
            return AT_ERROR;
        }

        exe = atc_get_exe_handle();
        exe_res = exe_request(exe, EXE_CGQREQ_SET, (void *)&pdp_qos, (void *)parser_p, &parser_p->request_handle);
        result = parser_handle_exe_result(exe_res, parser_p);
        break;
    }

    case AT_MODE_READ: {
        exe_t *exe_p;
        exe_pdp_qos_2G_response_t qos_response;
        memset(&qos_response, 0x00, sizeof(exe_pdp_qos_2G_response_t));
        exe_p = atc_get_exe_handle();
        exe_res = exe_request(exe_p, EXE_CGQREQ_GET, (void *)&qos_response,
                              (void *)parser_p, &parser_p->request_handle);

        if (EXE_SUCCESS == exe_res) {
            create_pdp_qos_2G_read_response(info_text,  qos_response.next_p, "\r\n+CGQREQ");
            Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);

            /* Release memory from qos_response*/
            exe_pdp_qos_2G_response_t *current = qos_response.next_p;
            exe_pdp_qos_2G_response_t *element_to_remove = NULL;

            while (NULL != current) {
                element_to_remove = current;
                current = current->next_p;
                free(element_to_remove);
            }
        }

        result = parser_handle_exe_result(exe_res, parser_p);
        break;
    }

    case AT_MODE_TEST: {
        create_pdp_qos_2G_test_response(info_text, "IP");
        Parser_SendMultipleResponses(parser_p, AT_NO_OVERRIDE, info_text, false);
        create_pdp_qos_2G_test_response(info_text, "IPV6");
        Parser_SendMultipleResponses(parser_p, AT_NO_OVERRIDE, info_text, false);
        create_pdp_qos_2G_test_response(info_text, "IPV4V6");
        Parser_SendMultipleResponses(parser_p, AT_NO_OVERRIDE, info_text, true);
        result = AT_OK;
        break;
    }
    default: {
        break;
    }

    }

    return result;
}

/*===============================================================================
 *
 *  Function: AT_PLUS_CGQMIN_Handle
 *
 *
 *  INPUT:   Parser_p   - Pointer to the current parser state.
 *                 info_text   - Pointer to a string buffer to put information text.
 *                Response   - Pointer to the requester response
 *
 *
 *  OUTPUT:  Parser->
 *           Pos        - The current position in the command line.
 *
 *  RETURNS: A status code for the execution of the command is returned:
 *           AT_OK        - The command was successfully handled.
 *           AT_ERROR     - There was a general error.
 *
 *===============================================================================
 */
AT_Command_e AT_PLUS_CGQMIN_Handle(AT_ParserState_s *parser_p,
                                   AT_CommandLine_t info_text,
                                   AT_ResponseMessage_t message)
{
    AT_Command_e result = AT_ERROR;
    exe_request_result_t exe_res = EXE_SUCCESS;
    exe_pdp_qos_2G_response_t *response_p;

    switch (parser_p->Mode) {
    case AT_MODE_SET: {
        exe_t *exe = NULL;
        exe_pdp_qos_2G_t pdp_qos;

        memset(&pdp_qos, 0x00, sizeof(exe_pdp_qos_2G_t));

        if (!parse_pdp_qos_2G_params(parser_p, &pdp_qos)) {
            ATC_LOG_E("parsing of input failed.");
            return AT_ERROR;
        }

        exe = atc_get_exe_handle();
        exe_res = exe_request(exe, EXE_CGQMIN_SET, (void *)&pdp_qos, (void *)parser_p, &parser_p->request_handle);
        result = parser_handle_exe_result(exe_res, parser_p);
        break;
    }

    case AT_MODE_READ: {
        exe_t *exe_p;
        exe_pdp_qos_2G_response_t qos_response;
        memset(&qos_response, 0x00, sizeof(exe_pdp_qos_2G_response_t));
        exe_p = atc_get_exe_handle();
        exe_res = exe_request(exe_p, EXE_CGQMIN_GET, (void *)&qos_response,
                              (void *)parser_p, &parser_p->request_handle);

        if (EXE_SUCCESS == exe_res) {
            create_pdp_qos_2G_read_response(info_text,  qos_response.next_p, "\r\n+CGQMIN");
            Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);

            /* Release memory from qos_response*/
            exe_pdp_qos_2G_response_t *current = qos_response.next_p;
            exe_pdp_qos_2G_response_t *element_to_remove = NULL;

            while (NULL != current) {
                element_to_remove = current;
                current = current->next_p;
                free(element_to_remove);
            }
        }

        result = parser_handle_exe_result(exe_res, parser_p);
        break;
    }

    case AT_MODE_TEST: {
        create_pdp_qos_2G_test_response(info_text, "IP");
        Parser_SendMultipleResponses(parser_p, AT_NO_OVERRIDE, info_text, false);
        create_pdp_qos_2G_test_response(info_text, "IPV6");
        Parser_SendMultipleResponses(parser_p, AT_NO_OVERRIDE, info_text, false);
        create_pdp_qos_2G_test_response(info_text, "IPV4V6");
        Parser_SendMultipleResponses(parser_p, AT_NO_OVERRIDE, info_text, true);
        result = AT_OK;
        break;
    }
    default: {
        break;
    }

    }

    return result;
}

/*
 *===============================================================================
 *
 *  Function: AT_PLUS_DS_Handle
 *
 *
 *  Input:    Parser_p   - Pointer to parser to which AT protocol response
 *                         eventually shall be sent.
 *
 *  Output:   Parser_p   - Pointer to submitted parser, with state
 *                         Pending changed to true.
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
 *           Data compression for online data mode.
 *
 *===============================================================================
 */
AT_Command_e AT_PLUS_DS_Handle(AT_ParserState_s *parser_p,
                               AT_CommandLine_t info_text,
                               AT_ResponseMessage_t message)
{
    AT_Command_e result = AT_ERROR;

    switch (parser_p->Mode) {
    case AT_MODE_SET: {
        /* Do nothing, just return OK */
        IntParam_t param = 0;
        bool error_flag = false;

        /* Direction */
        param = Parser_GetIntParam(parser_p, &error_flag, NULL);
        if (error_flag) {
            Parser_SetCMEE_ErrorType(parser_p, CMEE_UNKNOWN);
            result = AT_CME_ERROR;
            goto exit;
        }

        if (!parser_p->NoMoreParams) {
            /* Compression negotiation */
            param = Parser_GetIntParam(parser_p, &error_flag, NULL);
            if (error_flag) {
                Parser_SetCMEE_ErrorType(parser_p, CMEE_UNKNOWN);
                result = AT_CME_ERROR;
                goto exit;
            }
        }

        if (!parser_p->NoMoreParams) {
            /* Maximum dictionary size */
            param = Parser_GetLongIntParam(parser_p, &error_flag, NULL, EXE_MAX_DICT_SIZE);
            if (error_flag) {
                Parser_SetCMEE_ErrorType(parser_p, CMEE_UNKNOWN);
                result = AT_CME_ERROR;
                goto exit;
            }
        }

        if (!parser_p->NoMoreParams) {
            /* Maximum string length */
            param = Parser_GetIntParam(parser_p, &error_flag, NULL);
            if (error_flag) {
                Parser_SetCMEE_ErrorType(parser_p, CMEE_UNKNOWN);
                result = AT_CME_ERROR;
                goto exit;
            }
        }
        result = AT_OK;
    }
    default:
        break;
    }

exit:
    return result;
}

/*
 *===============================================================================
 *
 *  Function: AT_PLUS_IFC_Handle
 *
 *
 *  Input:    Parser_p   - Pointer to parser to which AT protocol response
 *                         eventually shall be sent.
 *
 *  Output:   Parser_p   - Pointer to submitted parser, with state
 *                         Pending changed to true.
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
 *           Interface local flowcontrol for DTE<->DCE communication.
 *
 *===============================================================================
 */

AT_Command_e AT_PLUS_IFC_Handle(AT_ParserState_s *parser_p,
                                AT_CommandLine_t info_text,
                                AT_ResponseMessage_t message)
{
    AT_Command_e result = AT_ERROR;

    switch (parser_p->Mode) {
    case AT_MODE_SET: {
        /* Do nothing, just return OK */
        IntParam_t param = 0;
        bool error_flag = false;
        /* DTE settings */
        param = Parser_GetIntParam(parser_p, &error_flag, NULL);
        if (error_flag) {
            Parser_SetCMEE_ErrorType(parser_p, CMEE_UNKNOWN);
            result = AT_CME_ERROR;
            goto exit;
        }
        /* DCE settings */
        param = Parser_GetIntParam(parser_p, &error_flag, NULL);
        if (error_flag) {
            Parser_SetCMEE_ErrorType(parser_p, CMEE_UNKNOWN);
            result = AT_CME_ERROR;
            goto exit;
        }
        result = AT_OK;
    }
    default:
        break;
    }

exit:
    return result;
}
/*
 * Support method for AT+CGEQMIN & AT+CGEQREQ.
 *
 * Sets info_text to contain the accepted values for the AT commands.
 * */
static void create_pdp_qos_test_response(AT_CommandLine_t info_text, bool allow_defaults, char* bearer)
{

    int n = 0;
    int rangeoffset = 0;

    if (true == allow_defaults) {
        rangeoffset = 1;
    }

    n += sprintf((char *)info_text + n, ": \"%s\",(0-%d),(0-%d),", bearer, EXE_QOS3G_PROFILE_MAX_TRAFFIC_CLASS + rangeoffset, EXE_QOS3G_PROFILE_MAX_MAX_BITRATE_UL);
    n += sprintf((char *)info_text + n, "(0-%d),(0-%d),(0-%d),(0-%d),(0-%d),(", EXE_QOS3G_PROFILE_MAX_MAX_BITRATE_DL,
                 EXE_QOS3G_PROFILE_MAX_GUARANTEED_BITRATE_UL, EXE_QOS3G_PROFILE_MAX_GUARANTEED_BITRATE_DL, EXE_QOS3G_PROFILE_MAX_DELIVERY_ORDER + rangeoffset,
                 EXE_QOS3G_PROFILE_MAX_SDU_SIZE);

    if (true == allow_defaults) {
        n += sprintf((char *)info_text + n, "\"0E0\",");
    }

    n += sprintf((char *)info_text + n, "\"1E2\",\"7E3\",\"1E3\",\"1E4\","
                 "\"1E5\",\"1E6\",\"1E1\"),(");

    if (true == allow_defaults) {
        n += sprintf((char *)info_text + n, "\"0E0\",");
    }

    n += sprintf((char *)info_text + n, "\"5E2\",\"1E2\",\"5E3\",\"4E3\",\"1E3\",\"1E4\","
                 "\"1E5\",\"1E6\",\"6E8\"),(0-%d),(0-%d),(0-%d),(0-1),(0-1)", EXE_QOS3G_PROFILE_MAX_DELIVERY_ERR_SDU + rangeoffset,
                 EXE_QOS3G_PROFILE_MAX_TRANSFER_DELAY, EXE_QOS3G_PROFILE_MAX_TRAFFIC_HANDLING_PRIO);
}


/*
 * Support method for AT+CGEQMIN & AT+CGEQREQ.
 *
 * If bool allow_defaults is set the default values used by CGEQREQ will be allowed as inputs.
 *
 * This affects range of parameters traffic_class, delivery_order and delivery_err_sdu and makes "0E0" an
 * acceptable input for the string parameters.
 *
 * */
static bool parse_pdp_qos_params(AT_ParserState_s *parser_p, exe_pdp_qos_t *pdp_qos_p, bool allow_defaults)
{
    bool error_flag = false;
    int cid = 0;
    int rangeoffset = 0;
    bool result = false;

    IntParam_t traffic_class = EXE_QOS3G_PROFILE_TRAFFIC_CLASS;
    unsigned long max_bitrate_ul = EXE_QOS3G_PROFILE_MAX_BITRATE_UL;
    unsigned long max_bitrate_dl = EXE_QOS3G_PROFILE_MAX_BITRATE_DL;
    unsigned long guaranteed_bitrate_ul = EXE_QOS3G_PROFILE_GUARANTEED_BITRATE_UL;
    unsigned long guaranteed_bitrate_dl = EXE_QOS3G_PROFILE_GUARANTEED_BITRATE_DL;
    IntParam_t delivery_order = EXE_QOS3G_PROFILE_DELIVERY_ORDER;
    unsigned long max_sdu_size = EXE_QOS3G_PROFILE_SDU_SIZE;
    StrParam_t sdu_error_ratio = EXE_QOS3G_PROFILE_SDU_ERROR_RATIO;
    StrParam_t residual_ber = EXE_QOS3G_PROFILE_RESIDUAL_BER;
    IntParam_t delivery_err_sdu = EXE_QOS3G_PROFILE_DELIVERY_ERR_SDU;
    unsigned long transfer_delay = EXE_QOS3G_PROFILE_TRANSFER_DELAY;
    IntParam_t traffic_handling_priority = EXE_QOS3G_PROFILE_TRAFFIC_HANDLING_PRIORITY;
    IntParam_t source_statistics_descriptor = EXE_QOS3G_PROFILE_SOURCE_STATISTICS_DESCRIPTOR;
    IntParam_t signalling_indication = EXE_QOS3G_PROFILE_SIGNALLING_INDICATION;
    exe_exponential_t allowed_SDUs[] = EXE_QOS3G_PROFILE_ACCEPTED_SDU_ERRORS;
    exe_exponential_t allowed_BERs[] = EXE_QOS3G_PROFILE_ACCEPTED_BER_RATIOS;

    if (true == allow_defaults) {
        rangeoffset = 1;
        traffic_class = EXE_QOS3G_PROFILE_TRAFFIC_CLASS_REQ;
        delivery_order = EXE_QOS3G_PROFILE_DELIVERY_ORDER_REQ;
        delivery_err_sdu = EXE_QOS3G_PROFILE_DELIVERY_ERR_SDU_REQ;
    }

    /* A little strange, but valid, case, no in parameter given */
    if (END_OF_STRING == *parser_p->Pos || AT_COMMAND_SEPARATOR == *parser_p->Pos) {
        parser_p->Pos++;
        ATC_LOG_E("no input parameters...");
    }
    /* Loop over the in parameters
     */
    else {
        if (!parser_p->NoMoreParams) {
            cid = Parser_GetIntParam(parser_p, &error_flag, NULL);

            if ((MIN_CID > cid) || (MAX_CID < cid) || error_flag) {
                /* Invalid cid range */
                ATC_LOG_E("Invalid cid range... cid=%d  (error_flag:%d)", cid, error_flag);
                return false;
            }

            pdp_qos_p->cid = cid;
        } else {
            return false;
        }

        /* Parse traffic class */
        if (!parser_p->NoMoreParams) {
            traffic_class = Parser_GetIntParam(parser_p, &error_flag, &traffic_class);

            if ((EXE_QOS3G_PROFILE_MAX_TRAFFIC_CLASS + rangeoffset < traffic_class) || error_flag) {
                ATC_LOG_E("traffic class out of range:%d", traffic_class);
                return false;
            }
        }

        pdp_qos_p->traffic_class = traffic_class;

        /* Parse max bitrate ul */
        if (!parser_p->NoMoreParams) {
            max_bitrate_ul = Parser_GetLongIntParam(parser_p, &error_flag, &max_bitrate_ul , EXE_QOS3G_PROFILE_MAX_MAX_BITRATE_UL);

            if (error_flag) {
                ATC_LOG_E("max bitrate ul out of range:%ld", max_bitrate_ul);
                return false;
            }
        }

        pdp_qos_p->max_bitrate_ul = max_bitrate_ul;

        /* Parse max bitrate dl */
        if (!parser_p->NoMoreParams) {
            max_bitrate_dl = Parser_GetLongIntParam(parser_p, &error_flag, &max_bitrate_dl , EXE_QOS3G_PROFILE_MAX_MAX_BITRATE_DL);

            if (error_flag) {
                ATC_LOG_E("max bitrate dl out of range:%ld", max_bitrate_dl);
                return false;
            }
        }

        pdp_qos_p->max_bitrate_dl = max_bitrate_dl;

        /* Parse guaranteed bitrate ul */
        if (!parser_p->NoMoreParams) {
            guaranteed_bitrate_ul = Parser_GetLongIntParam(parser_p, &error_flag , &guaranteed_bitrate_ul , EXE_QOS3G_PROFILE_MAX_GUARANTEED_BITRATE_UL);

            if (error_flag) {
                ATC_LOG_E("guaranteed bitrate ul out of range:%ld", guaranteed_bitrate_ul);
                return false;
            }
        }

        pdp_qos_p->guaranteed_bitrate_ul = guaranteed_bitrate_ul;

        /* Parse guaranteed bitrate dl */
        if (!parser_p->NoMoreParams) {
            guaranteed_bitrate_dl = Parser_GetLongIntParam(parser_p, &error_flag, &guaranteed_bitrate_dl , EXE_QOS3G_PROFILE_MAX_GUARANTEED_BITRATE_DL);

            if (error_flag) {
                ATC_LOG_E("guaranteed bitrate dl out of range:%ld", guaranteed_bitrate_dl);
                return false;
            }
        }

        pdp_qos_p->guaranteed_bitrate_dl = guaranteed_bitrate_dl;

        /* Parse delivery order */
        if (!parser_p->NoMoreParams) {
            delivery_order = Parser_GetIntParam(parser_p, &error_flag, &delivery_order);

            if (EXE_QOS3G_PROFILE_MAX_DELIVERY_ORDER  + rangeoffset < delivery_order || error_flag) {
                ATC_LOG_E("delivery order out of range:%d", delivery_order);
                return false;
            }
        }

        pdp_qos_p->delivery_order = delivery_order;

        /* Parse max SDU size */
        if (!parser_p->NoMoreParams) {
            max_sdu_size = Parser_GetLongIntParam(parser_p, &error_flag , &max_sdu_size , EXE_QOS3G_PROFILE_MAX_SDU_SIZE);

            if (error_flag) {
                ATC_LOG_E("max SDU size out of range:%ld", max_sdu_size);
                return false;
            }
        }

        pdp_qos_p->max_sdu_size = max_sdu_size;

        /* Parse SDU error ratio */
        if (!parser_p->NoMoreParams) {
            sdu_error_ratio = Parser_GetStrParam(parser_p, &error_flag, (StrParam_t)sdu_error_ratio);

            if (!check_exponential(sdu_error_ratio, allowed_SDUs, sizeof(allowed_SDUs) / sizeof(exe_exponential_t)) || error_flag) {
                ATC_LOG_E("SDU error ratio out of range");
                return false;
            }
        }

        pdp_qos_p->sdu_error_ratio_p = sdu_error_ratio;

        /* Parse residual ber */
        if (!parser_p->NoMoreParams) {
            residual_ber = Parser_GetStrParam(parser_p, &error_flag, (StrParam_t)residual_ber);

            if (!check_exponential(residual_ber, allowed_BERs, sizeof(allowed_BERs) / sizeof(exe_exponential_t)) || error_flag) {
                ATC_LOG_E("residual ber out of range");
                return false;
            }
        }

        pdp_qos_p->residual_ber_p = residual_ber;

        /* Parse delivery err sdu */
        if (!parser_p->NoMoreParams) {
            delivery_err_sdu = Parser_GetIntParam(parser_p, &error_flag, &delivery_err_sdu);

            if (EXE_QOS3G_PROFILE_MAX_DELIVERY_ERR_SDU  + rangeoffset < delivery_err_sdu || error_flag) {
                ATC_LOG_E("delivery err sdu out of range:%d", delivery_err_sdu);
                return false;
            }
        }

        pdp_qos_p->delivery_err_sdu = delivery_err_sdu;

        /* Parse transfer delay */
        if (!parser_p->NoMoreParams) {
            transfer_delay = Parser_GetLongIntParam(parser_p, &error_flag, &transfer_delay , EXE_QOS3G_PROFILE_MAX_TRANSFER_DELAY);

            if (error_flag) {
                ATC_LOG_E("transfer delay out of range:%ld", transfer_delay);
                return false;
            }
        }

        pdp_qos_p->transfer_delay = transfer_delay;

        /* Parse traffic handling priority */
        if (!parser_p->NoMoreParams) {
            traffic_handling_priority = Parser_GetIntParam(parser_p, &error_flag, &traffic_handling_priority);

            if (EXE_QOS3G_PROFILE_MAX_TRAFFIC_HANDLING_PRIO < traffic_handling_priority || error_flag) {
                ATC_LOG_E("traffic handling priority out of range:%d", traffic_handling_priority);
                return false;
            }
        }

        pdp_qos_p->traffic_handling_priority = traffic_handling_priority;

        /* Parse source statistics descriptor*/
        if (!parser_p->NoMoreParams) {
            source_statistics_descriptor = Parser_GetIntParam(parser_p, &error_flag, &source_statistics_descriptor);

            if (EXE_QOS3G_PROFILE_MAX_SOURCE_STATISTICS_DESCRIPTOR < source_statistics_descriptor || error_flag) {
                ATC_LOG_E("source statistics descriptor out of range:%d", source_statistics_descriptor);
                return false;
            }
        }

        pdp_qos_p->source_statistics_descriptor = source_statistics_descriptor;

        /* Parse signalling indication */
        if (!parser_p->NoMoreParams) {
            signalling_indication = Parser_GetIntParam(parser_p, &error_flag, &signalling_indication);

            if (EXE_QOS3G_PROFILE_MAX_SIGNALLING_INDICATION < signalling_indication || error_flag) {
                /* Valid range: 0-1 */
                ATC_LOG_E("signalling indication out of range:%d", signalling_indication);
                return false;
            }
        }

        pdp_qos_p->signalling_indication = signalling_indication;


        /* Last Parameter received. Check if the user by mistake sends more parameters! */
        if (!parser_p->NoMoreParams) {
            ATC_LOG_E("too many input parameters");
            return false;
        }
    }

    return true;
}

/*
 *===============================================================================
 *
 *  Function: AT_PLUS_CGEQMIN_Handle
 *
 *
 *  Input:    parser_p   - Pointer to parser to which AT protocol response
 *                         eventually shall be sent.
 *
 *  Output:   parser_p   - Pointer to submitted parser, with state
 *                         Pending changed to true.
 *
 *  OUTPUT:  Parser->
 *           Pos        - The current position in the command line.
 *
 *  RETURNS: A status code for the execution of the command is returned:
 *           AT_OK        - The command was successfully handled.
 *           AT_ERROR     - There was a general error.
 *           AT_CME_ERROR - There was a GSM specific error, the error code is
 *                          returned in info_text.
 *
 *===============================================================================
 */
AT_Command_e AT_PLUS_CGEQMIN_Handle(AT_ParserState_s *parser_p,
                                    AT_CommandLine_t info_text,
                                    AT_ResponseMessage_t message)
{
    AT_Command_e result = AT_ERROR;
    exe_request_result_t exe_res = EXE_SUCCESS;
    exe_pdp_qos_response_t *response_p;

    switch (parser_p->Mode) {
    case AT_MODE_SET: {
        exe_t *exe = NULL;
        exe_pdp_qos_t pdp_qos;

        memset(&pdp_qos, 0x00, sizeof(exe_pdp_qos_t));

        if (!parse_pdp_qos_params(parser_p, &pdp_qos, false)) {
            ATC_LOG_E("copy of input failed.");
            return AT_ERROR;
        }

        exe = atc_get_exe_handle();
        exe_res = exe_request(exe, EXE_CGEQMIN_SET, (void *)&pdp_qos, (void *)parser_p, &parser_p->request_handle);
        result = parser_handle_exe_result(exe_res, parser_p);
        break;
    }

    case AT_MODE_READ: {
        exe_t *exe_p;
        exe_pdp_qos_response_t qos_response;
        memset(&qos_response, 0x00, sizeof(exe_pdp_qos_response_t));
        exe_p = atc_get_exe_handle();
        exe_res = exe_request(exe_p, EXE_CGEQMIN_GET, (void *)&qos_response,
                              (void *)parser_p, &parser_p->request_handle);

        if (EXE_SUCCESS == exe_res) {
            create_pdp_qos_read_response(info_text,  qos_response.next_p, "\r\n+CGEQMIN", true);
            Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);

            /* Release memory from qos_response*/
            exe_pdp_qos_response_t *current = qos_response.next_p;
            exe_pdp_qos_response_t *element_to_remove = NULL;

            while (NULL != current) {
                element_to_remove = current;
                current = current->next_p;
                free(element_to_remove->pdp_qos.residual_ber_p);
                free(element_to_remove->pdp_qos.sdu_error_ratio_p);
                free(element_to_remove);
            }
        }

        result = parser_handle_exe_result(exe_res, parser_p);
        break;
    }

    case AT_MODE_TEST: {

        create_pdp_qos_test_response(info_text, false, "IP");
        Parser_SendMultipleResponses(parser_p, AT_NO_OVERRIDE, info_text, false);
        create_pdp_qos_test_response(info_text, false, "IPV6");
        Parser_SendMultipleResponses(parser_p, AT_NO_OVERRIDE, info_text, false);
        create_pdp_qos_test_response(info_text, false, "IPV4V6");
        Parser_SendMultipleResponses(parser_p, AT_NO_OVERRIDE, info_text, true);
        result = AT_OK;
        break;
    }
    default: {
        break;
    }

    }

    return result;
}

/*===============================================================================
 *
 *  Function: AT_PLUS_CGEQNEG_Handle
 *
 *
 *  Input:    parser_p   - Pointer to parser to which AT protocol response
 *                         eventually shall be sent.
 *
 *  Output:   parser_p   - Pointer to submitted parser, with state
 *                         Pending changed to true.
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
 *===============================================================================
 */
AT_Command_e AT_PLUS_CGEQNEG_Handle(AT_ParserState_s *parser_p,
                                    AT_CommandLine_t info_text,
                                    AT_ResponseMessage_t message)
{
    AT_Command_e result = AT_ERROR;
    exe_request_result_t exe_res = EXE_SUCCESS;
    exe_t                    *exe_p    = NULL;
    ATC_LOG_I("Entry");
    exe_cgpaddr_cid_t *cgpaddr_cid_p = NULL;

    switch (parser_p->Mode) {
    case AT_MODE_SET: {
        if (!parser_p->IsPending) {
            bool error_flag = false;
            int cid = 0;

            while (!parser_p->NoMoreParams) {
                cid = Parser_GetIntParam(parser_p, &error_flag, NULL);

                if ((cid < MIN_CID) || (cid > MAX_CID) || error_flag) {
                    Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
                    free_cgpaddr_cid_list(cgpaddr_cid_p);
                    return AT_CME_ERROR;
                }

                if (!add_cgpaddr_cid_to_list(cid, &cgpaddr_cid_p)) {
                    ATC_LOG_E("Error adding cid to list");
                    free_cgpaddr_cid_list(cgpaddr_cid_p);
                    return AT_ERROR;
                }
            }

            /* If there are no cid's among the parameters, just return OK. */
            if (NULL != cgpaddr_cid_p) {
                exe_p = atc_get_exe_handle();
                exe_res = exe_request(exe_p, EXE_CGEQNEG_SET, (void *) cgpaddr_cid_p,
                                      (void *) parser_p, &parser_p->request_handle);
                free_cgpaddr_cid_list(cgpaddr_cid_p);
            }

        } else {
            exe_pdp_qos_response_t *response_p = (exe_pdp_qos_response_t *)message->data_p;

            if (message->exe_result_code == EXE_SUCCESS) {
                exe_pdp_qos_response_t *current_p = response_p;
                create_pdp_qos_read_response(info_text, current_p, "\r\n+CGEQNEG", false);

                Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
                result = AT_OK;
            }
        }

        result = parser_handle_exe_result(exe_res, parser_p);
        break;
    }

    case AT_MODE_TEST: {
        /* AT+CGCMOD=? is almost identical to AT+CGACT? but
         * it lists only the active contexts. */

        if (!parser_p->IsPending) {
            exe_p = atc_get_exe_handle();
            exe_res = exe_request(exe_p, EXE_CGACT_READ, NULL,
                                  (void *) parser_p, &parser_p->request_handle);
        } else if (NULL != message) {
            bool first_print = true;
            exe_res = message->exe_result_code;
            sprintf((char *)info_text, ": ");
            exe_cgact_read_response_t *response_data_p = (exe_cgact_read_response_t *)message->data_p;

            while (NULL != response_data_p) {
                exe_cgact_read_response_t *next_p = response_data_p->next_p;

                if (1 == response_data_p->state) {
                    if (first_print) {
                        AT_AddValue(info_text, "(0", response_data_p->cid);
                    } else {
                        AT_AddValue(info_text, ",0", response_data_p->cid);
                    }

                    first_print = false;
                }

                response_data_p = next_p;
            }

            /* end of printout if any active CIDS exist */
            if (!first_print) {
                strcat((char *)info_text, ")");
            }

            Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        }

        break;
    }
    default: {
        break;
    }

    }

    result = parser_handle_exe_result(exe_res, parser_p);

exit:
    return result;
}

/*
 *===============================================================================
 *
 *  Function: AT_PLUS_CGEQREQ_Handle
 *
 *
 *  Input:    parser_p   - Pointer to parser to which AT protocol response
 *                         eventually shall be sent.
 *
 *  Output:   parser_p   - Pointer to submitted parser, with state
 *                         Pending changed to true.
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
 *===============================================================================
 */
AT_Command_e AT_PLUS_CGEQREQ_Handle(AT_ParserState_s *parser_p,
                                    AT_CommandLine_t info_text,
                                    AT_ResponseMessage_t message)
{
    AT_Command_e result = AT_ERROR;
    exe_request_result_t exe_res = EXE_SUCCESS;
    exe_pdp_qos_response_t *response_p;

    switch (parser_p->Mode) {
    case AT_MODE_SET: {
        exe_t *exe = NULL;
        exe_pdp_qos_t pdp_qos;

        memset(&pdp_qos, 0x00, sizeof(exe_pdp_qos_t));

        if (!parse_pdp_qos_params(parser_p, &pdp_qos, true)) {
            ATC_LOG_E("copy of input failed.");
            return AT_ERROR;
        }

        exe = atc_get_exe_handle();
        exe_res = exe_request(exe, EXE_CGEQREQ_SET, (void *)&pdp_qos, (void *)parser_p, &parser_p->request_handle);
        result = parser_handle_exe_result(exe_res, parser_p);
        break;
    }

    case AT_MODE_READ: {
        exe_t *exe_p;
        exe_pdp_qos_response_t qos_response;
        memset(&qos_response, 0x00, sizeof(exe_pdp_qos_response_t));
        exe_p = atc_get_exe_handle();
        exe_res = exe_request(exe_p, EXE_CGEQREQ_GET, (void *)&qos_response,
                              (void *)parser_p, &parser_p->request_handle);

        if (EXE_SUCCESS == exe_res) {
            create_pdp_qos_read_response(info_text,  qos_response.next_p, "\r\n+CGEQREQ", true);
            Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);

            /* Release memory from qos_response*/
            exe_pdp_qos_response_t *current = qos_response.next_p;
            exe_pdp_qos_response_t *element_to_remove = NULL;

            while (NULL != current) {
                element_to_remove = current;
                current = current->next_p;

                free(element_to_remove->pdp_qos.residual_ber_p);
                free(element_to_remove->pdp_qos.sdu_error_ratio_p);
                free(element_to_remove);
            }
        }

        result = parser_handle_exe_result(exe_res, parser_p);
        break;
    }

    case AT_MODE_TEST: {

        create_pdp_qos_test_response(info_text, true, "IP");
        Parser_SendMultipleResponses(parser_p, AT_NO_OVERRIDE, info_text, false);
        create_pdp_qos_test_response(info_text, true, "IPV6");
        Parser_SendMultipleResponses(parser_p, AT_NO_OVERRIDE, info_text, false);
        create_pdp_qos_test_response(info_text, true, "IPV4V6");
        Parser_SendMultipleResponses(parser_p, AT_NO_OVERRIDE, info_text, true);
        result = AT_OK;
        break;
    }
    default: {
        break;
    }

    }

    return result;
}


static void create_pdp_qos_read_response(AT_CommandLine_t info_text,
        exe_pdp_qos_response_t *current_p,
        char *at_cmd, bool display_last_params)
{
    int n = 0;

    n = sprintf((char *)info_text, ": ");


    while ((current_p != NULL) && (current_p->pdp_qos.cid != 0)) {
        n += sprintf((char *)info_text + n, "%d,", current_p->pdp_qos.cid);
        n += sprintf((char *)info_text + n, "%d,", current_p->pdp_qos.traffic_class);
        n += sprintf((char *)info_text + n, "%ld,", current_p->pdp_qos.max_bitrate_ul);
        n += sprintf((char *)info_text + n, "%ld,", current_p->pdp_qos.max_bitrate_dl);
        n += sprintf((char *)info_text + n, "%ld,", current_p->pdp_qos.guaranteed_bitrate_ul);
        n += sprintf((char *)info_text + n, "%ld,", current_p->pdp_qos.guaranteed_bitrate_dl);
        n += sprintf((char *)info_text + n, "%d,", current_p->pdp_qos.delivery_order);
        n += sprintf((char *)info_text + n, "%ld,", current_p->pdp_qos.max_sdu_size);

        if (current_p->pdp_qos.sdu_error_ratio_p == NULL) {
            n += sprintf((char *)info_text + n, ",");
        } else {
            n += sprintf((char *)info_text + n, "\"%.3s\",", current_p->pdp_qos.sdu_error_ratio_p);
        }

        if (current_p->pdp_qos.residual_ber_p == NULL) {
            n += sprintf((char *)info_text + n, ",");
        } else {
            n += sprintf((char *)info_text + n, "\"%.3s\",", current_p->pdp_qos.residual_ber_p);
        }

        n += sprintf((char *)info_text + n, "%d,", current_p->pdp_qos.delivery_err_sdu);
        n += sprintf((char *)info_text + n, "%ld,", current_p->pdp_qos.transfer_delay);
        n += sprintf((char *)info_text + n, "%d", current_p->pdp_qos.traffic_handling_priority);

        if (true == display_last_params) {
            n += sprintf((char *)info_text + n, ",%d,", current_p->pdp_qos.source_statistics_descriptor);
            n += sprintf((char *)info_text + n, "%d", current_p->pdp_qos.signalling_indication);
        }

        if (current_p->next_p != NULL) {
            n += sprintf((char *)info_text + n, "%s: ", at_cmd);
        }

        current_p = current_p->next_p;
    }
}

static void create_pdp_qos_2G_test_response(AT_CommandLine_t info_text, char* bearer)
{
    int n = 0;

    n += sprintf((char *)info_text + n, ": \"%s\"", bearer);
    n += sprintf((char *)info_text + n, ",(0-%d)", EXE_QOS2G_PROFILE_MAX_PRECEDENCE);
    n += sprintf((char *)info_text + n, ",(0-%d)", EXE_QOS2G_PROFILE_MAX_DELAY);
    n += sprintf((char *)info_text + n, ",(0-%d)", EXE_QOS2G_PROFILE_MAX_RELIABILITY);
    n += sprintf((char *)info_text + n, ",(0-%d)", EXE_QOS2G_PROFILE_MAX_PEAK);
    n += sprintf((char *)info_text + n, ",(0-%d,%d)", EXE_QOS2G_PROFILE_MAX_MEAN, EXE_QOS2G_PROFILE_MAX_MEAN_EXTRA);
}

/*
 *===============================================================================
 *
 *  Function: AT_STAR_EPPSD_Handle
 *
 *  Input:    parser_p   - Pointer to parser to which AT protocol response
 *                         eventually shall be sent.
 *
 *  Output:   parser_p   - Pointer to submitted parser, with state
 *                         Pending changed to true.
 *
 *  OUTPUT:  Parser->
 *           Pos        - The current position in the command line.
 *
 *  RETURNS: A status code for the execution of the command is returned:
 *           AT_OK        - The command was successfully handled.
 *           AT_ERROR     - There was a general error.
 *           AT_CME_ERROR - There was a GSM specific error, the error code is
 *                          returned in info_text.
 *
 *===============================================================================
 */
AT_Command_e AT_STAR_EPPSD_Handle(AT_ParserState_s *parser_p,
                                  AT_CommandLine_t info_text,
                                  AT_ResponseMessage_t Message)
{
    AT_Command_e result = AT_OK;

    switch (parser_p->Mode) {
    case AT_MODE_SET: {
        if (!parser_p->IsPending) {
            exe_t                 *exe        = NULL;
            exe_request_result_t  exe_res     = EXE_FAILURE;
            bool error_flag = false;
            exe_pscc_state_t state;
            unsigned long connection_id;
            int cid;

            exe_eppsd_t eppsd;
            memset(&eppsd, 0x00, sizeof(exe_eppsd_t));

            state = Parser_GetIntParam(parser_p, &error_flag, NULL);

            if (error_flag || !(EXE_PSCC_STATE_ON == state || EXE_PSCC_STATE_OFF == state)) {
                return AT_ERROR;
            }

            eppsd.state = state;

            connection_id = Parser_GetLongIntParam(parser_p, &error_flag, NULL, MAX_LONG_INT_PARAM);

            if (error_flag) {
                return AT_ERROR;
            }

            eppsd.connection_id = connection_id;

            cid = Parser_GetIntParam(parser_p, &error_flag, NULL);

            if (error_flag || (MIN_CID > cid) || (MAX_CID < cid)) {
                return AT_ERROR;
            }

            eppsd.cid = cid;

            exe = atc_get_exe_handle();
            exe_res = exe_request(exe, EXE_EPPSD, (void *)&eppsd,
                                  (void *)parser_p, &parser_p->request_handle);
            result = parser_handle_exe_result(exe_res, parser_p);
        } else {
            /* Pending */
            result = parser_handle_exe_result(Message->exe_result_code, parser_p);

            if (AT_OK == result) {
                if (NULL != Message->data_p) {
                    exe_pscc_connection_response_t *response_data_p = (exe_pscc_connection_response_t *)Message->data_p;
                    sprintf((char *)info_text, ": ");
                    create_eppsd_xml_response(response_data_p, info_text);
                    Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
                }
            }
        }

        break;
    }

    case AT_MODE_TEST:
        sprintf((char *) info_text, ": (0-1)");
        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        break;
    default:
        result = AT_ERROR;
        break;
    }

    return result;
}

/* Specification example
<?xml version="1.0"?>
<connection_parameters>
    <ip_address>192.168.0.1</ip_address>
    <subnet_netmask>255.255.255.0</subnet_netmask>
    <mtu>1500</mtu>
    <default_gateway>192.168.0.2</default_gateway>
    <dns_server>192.168.0.3</dns_server>
    <dns_server>192.168.0.4</dns_server>
    <p_cscf_server>192.168.0.2</p_cscf_server>
    <p_cscf_server>192.168.0.2</p_cscf_server>
        </connection_parameters> */

static void create_eppsd_xml_response(exe_pscc_connection_response_t *connection_response_p, AT_CommandLine_t info_text)
{
    strcat((char *)info_text, "<?xml version=\"1.0\"?><connection_parameters>");

    if (connection_response_p->ip_address_p != NULL && connection_response_p->ipv6_address_p != NULL) {
        /* Dual-mode */
        strcat((char *)info_text, "<ip_address>");
        strcat((char *)info_text, connection_response_p->ip_address_p);
        strcat((char *)info_text, "</ip_address>");
        strcat((char *)info_text, "<ipv6_address>");
        strcat((char *)info_text, connection_response_p->ipv6_address_p);
        strcat((char *)info_text, "</ipv6_address>");
    } else if (connection_response_p->ip_address_p != NULL) {
        strcat((char *)info_text, "<ip_address>");
        strcat((char *)info_text, connection_response_p->ip_address_p);
        strcat((char *)info_text, "</ip_address>");
    } else if (connection_response_p->ipv6_address_p != NULL)  {
        strcat((char *)info_text, "<ip_address>");
        strcat((char *)info_text, connection_response_p->ipv6_address_p);
        strcat((char *)info_text, "</ip_address>");
    }

    if (connection_response_p->subnet_netmask_p != NULL) {
        strcat((char *)info_text, "<subnet_mask>");
        strcat((char *)info_text, connection_response_p->subnet_netmask_p);
        strcat((char *)info_text, "</subnet_mask>");
    }

    AT_AddValue(info_text, "<mtu>0</mtu>", connection_response_p->mtu);

    if (connection_response_p->default_gateway_p != NULL) {
        strcat((char *)info_text, "<default_gateway>");
        strcat((char *)info_text, connection_response_p->default_gateway_p);
        strcat((char *)info_text, "</default_gateway>");
    }

    if (connection_response_p->dns_server_p != NULL && connection_response_p->ipv6_dns_server_p != NULL) {
        /* Dualmode */
        strcat((char *)info_text, "<dns_server>");
        strcat((char *)info_text, connection_response_p->dns_server_p);
        strcat((char *)info_text, "</dns_server>");
        strcat((char *)info_text, "<ipv6_dns_server>");
        strcat((char *)info_text, connection_response_p->ipv6_dns_server_p);
        strcat((char *)info_text, "</ipv6_dns_server>");
    } else if (connection_response_p->dns_server_p != NULL) {
        strcat((char *)info_text, "<dns_server>");
        strcat((char *)info_text, connection_response_p->dns_server_p);
        strcat((char *)info_text, "</dns_server>");
    } else if (connection_response_p->ipv6_dns_server_p != NULL) {
        strcat((char *)info_text, "<dns_server>");
        strcat((char *)info_text, connection_response_p->ipv6_dns_server_p);
        strcat((char *)info_text, "</dns_server>");
    }

    if (connection_response_p->dns_server_2_p != NULL && connection_response_p->ipv6_dns_server_2_p != NULL) {
        /* Dualmode */
        strcat((char *)info_text, "<dns_server>");
        strcat((char *)info_text, connection_response_p->dns_server_2_p);
        strcat((char *)info_text, "</dns_server>");
        strcat((char *)info_text, "<ipv6_dns_server>");
        strcat((char *)info_text, connection_response_p->ipv6_dns_server_2_p);
        strcat((char *)info_text, "</ipv6_dns_server>");
    } else if (connection_response_p->dns_server_2_p != NULL) {
        strcat((char *)info_text, "<dns_server>");
        strcat((char *)info_text, connection_response_p->dns_server_2_p);
        strcat((char *)info_text, "</dns_server>");
    } else if (connection_response_p->ipv6_dns_server_2_p != NULL) {
        strcat((char *)info_text, "<dns_server>");
        strcat((char *)info_text, connection_response_p->ipv6_dns_server_2_p);
        strcat((char *)info_text, "</dns_server>");
    }

    if (connection_response_p->p_cscf_server_p != NULL) {
        strcat((char *)info_text, "<p_cscf_server>");
        strcat((char *)info_text, connection_response_p->p_cscf_server_p);
        strcat((char *)info_text, "</p_cscf_server>");
    }

    if (connection_response_p->p_cscf_server_2_p != NULL) {
        strcat((char *)info_text, "<p_cscf_server>");
        strcat((char *)info_text, connection_response_p->p_cscf_server_2_p);
        strcat((char *)info_text, "</p_cscf_server>");
    }
    AT_AddValue(info_text, "<activation_cause>0</activation_cause>", connection_response_p->activation_cause);

    strcat((char *)info_text, "</connection_parameters>");
}

/*
 *=====================================================================
 *
 *  Function: CGEV_build_report
 *
 *  Input:              info_text -      Empty character buffer.
 *                      response -      a CGEV_response_t containing
 *                                          response_type
 *                                          cid (not needed for response types which doesn't need cid, see specs.)
 *                                          pdp_type (not needed for response types which doesn't need pdp_type, see specs.)
 *                                          pdp_addr_p (not needed for response types which doesn't need pdp_addr_p, see specs.)
 *                                          cid (not needed for response types which doesn't need cid, see specs.)
 *
 *  Output:             info_text - Character buffer with CGEV indication text.
 *
 *      The function assembles a call status indication from the values of
 *      the input parameters.
 *
 *======================================================================
 */
void CGEV_build_report(AT_CommandLine_t info_text,
                       exe_cgev_response_t *response_p)
{
    *info_text = END_OF_STRING;

    if (NULL != response_p) {
        strcat((char *)info_text, "+CGEV: ");

        switch (response_p->response_type) {
        case CGEV_TYPE_ME_ACT:
            /* +CGEV: ME ACT <PDP_type>, <PDP_addr>, [<cid>] */
            strcat((char *) info_text, "ME ACT ");

            switch(response_p->pdp_type) {
            case 1:
                strcat((char *) info_text, "\"IPV6\"");
                break;
            case 2:
                strcat((char *) info_text, "\"IPV4V6\"");
                break;
            default:
                strcat((char *) info_text, "\"IP\"");
                break;
            }

            if (NULL != response_p->pdp_addr_p) {
                strcat((char *) info_text, ", ");
                strcat((char *) info_text, response_p->pdp_addr_p);
            } else {
                strcat((char *) info_text, ",");
            }

            if (response_p->cid > MIN_CID && response_p->cid < MAX_CID) {
                AT_AddValue(info_text, ", 0", response_p->cid);
            }

            break;

        case CGEV_TYPE_NW_DEACT:
            /* +CGEV: NW DEACT <PDP_type>, <PDP_addr>, [<cid>] */
            strcat((char *) info_text, "NW DEACT ");

            switch(response_p->pdp_type) {
            case 1:
                strcat((char *) info_text, "\"IPV6\"");
                break;
            case 2:
                strcat((char *) info_text, "\"IPV4V6\"");
                break;
            default:
                strcat((char *) info_text, "\"IP\"");
                break;
            }

            strcat((char *) info_text, ", ");

            if (NULL != response_p->pdp_addr_p) {
                strcat((char *) info_text, response_p->pdp_addr_p);
            }

            if (response_p->cid > MIN_CID && response_p->cid < MAX_CID) {
                AT_AddValue(info_text, ", 0", response_p->cid);
            }

            break;
        case CGEV_TYPE_ME_DEACT:
            /* +CGEV: ME DEACT <PDP_type>, <PDP_addr>, [<cid>] */
            strcat((char *) info_text, "ME DEACT ");

            switch(response_p->pdp_type) {
            case 1:
                strcat((char *) info_text, "\"IPV6\"");
                break;
            case 2:
                strcat((char *) info_text, "\"IPV4V6\"");
                break;
            default:
                strcat((char *) info_text, "\"IP\"");
                break;
            }

            strcat((char *) info_text, ", ");

            if (NULL != response_p->pdp_addr_p) {
                strcat((char *) info_text, response_p->pdp_addr_p);
            }

            if (response_p->cid > MIN_CID && response_p->cid < MAX_CID) {
                AT_AddValue(info_text, ", 0", response_p->cid);
            }

            break;
        case CGEV_TYPE_NW_DETACH:
            /* +CGEV: NW DETACH */
            strcat((char *) info_text, "NW DETACH");
            break;
        case CGEV_TYPE_ME_DETACH:
            /* +CGEV: ME DETACH */
            strcat((char *) info_text, "ME DETACH");
            break;
        case CGEV_TYPE_UNDEFINED:
        default:
            ATC_LOG_E("CGEV response type is not set or is set to CGEV_TYPE_UNDEFINED");
            return;
            break;
        }

        ATC_LOG_I("\nresponse_type = %d, cid = %d, pdp_type = %s, pdp_address = %s",
                  response_p->response_type,
                  response_p->cid,
                  response_p->pdp_type == 2 ? "\"IPV4V6\"" : (response_p->pdp_type == 1 ? "\"IPV6\"" : "\"IP\""),
                  response_p->pdp_addr_p == NULL ? "NULL" : response_p->pdp_addr_p);
    } else {
        ATC_LOG_E("failed, response is empty");
    }

    return;
}

/*
 * Handle incoming CGEV event from the executor
 *
 */
void handle_incoming_CGEV_event(exe_cgev_response_t *response_p)
{
    unsigned char      *cgev_text_p = (unsigned char *)malloc(SIZE_OF_CGEV_RESPONSE);
    AT_ParserState_s    *parser_p;
    unsigned char       entry;

    if (NULL == cgev_text_p) {
        ATC_LOG_E("cgev_text_p failed to initialize");
        goto exit;
    }

    CGEV_build_report((AT_CommandLine_t)cgev_text_p, response_p);

    /* Loop through all parser states to find subscriptions. */
    for (entry = 0; entry < MAX_NO_OF_OPEN_CHANNELS; entry++) {
        parser_p = ParserStateTable_GetParserState_ByEntry(entry);

        if (parser_p->CGEV) {
            parser_send_unsolicited(parser_p, AT_TEXT_RESPONSE, (AT_CommandLine_t)cgev_text_p);
        }
    }

exit:
    free((void *)cgev_text_p);
}

/*
 * Handle CR event from the executor
 *
 */
void handle_incoming_CR_event(void)
{
    AT_ParserState_s    *parser_p;
    unsigned char       entry;
    AT_CommandLine_t    ct_infotext_p = (unsigned char *)malloc(SIZE_OF_CR_RESPONSE);

    if (NULL == ct_infotext_p) {
        ATC_LOG_E("malloc failed, ct_infotext_p is NULL");
        return;
    }

    *ct_infotext_p = END_OF_STRING;
    strcat((char *)ct_infotext_p, "+CR: GPRS");

    /* Loop through all parser states to find subscriptions. */
    for (entry = 0; entry < MAX_NO_OF_OPEN_CHANNELS; entry++) {
        parser_p = ParserStateTable_GetParserState_ByEntry(entry);

        if (parser_p->CR) {
            parser_send_unsolicited(parser_p, AT_TEXT_RESPONSE, ct_infotext_p);
        }
    }

    free((void *)ct_infotext_p);
}


void atc_hand_free_cgact_set_list(exe_cgact_set_request_t *cgact_p)
{
    if (NULL != cgact_p) {
        exe_cgact_set_request_cid_t *next_p = NULL;
        exe_cgact_set_request_cid_t *current_p = cgact_p->cid_p;

        while (NULL != current_p) {
            next_p = current_p->next_p;
            free((void *)current_p);
            current_p = next_p;
        }

        cgact_p->cid_p = NULL;
        free(cgact_p);
    }
}

/*
 *===============================================================================
 *
 *  Function: AT_STAR_EIAAUW_Handle
 *
 *  This function writes authentication parameters to a given <cid>.
 *
 *  Input:    parser_p   - Pointer to parser to which AT protocol response
 *                         eventually shall be sent.
 *
 *  Output:   parser_p   - Pointer to submitted parser, with state
 *                         Pending changed to true.
 *
 *  OUTPUT:  Parser->
 *           Pos        - The current position in the command line.
 *
 *  RETURNS: A status code for the execution of the command is returned:
 *           AT_OK        - The command was successfully handled.
 *           AT_ERROR     - There was a general error.
 *           AT_CME_ERROR - There was a GSM specific error, the error code is
 *                          returned in info_text.
 *
 *===============================================================================
 */
AT_Command_e AT_STAR_EIAAUW_Handle(AT_ParserState_s *parser_p,
                                   AT_CommandLine_t info_text,
                                   AT_ResponseMessage_t message)
{
    AT_Command_e result = AT_OK;
    exe_request_result_t exe_res = EXE_FAILURE;
    bool error_flag = false;

    switch (parser_p->Mode) {
    case AT_MODE_SET: {
        exe_eiaauw_t  eiaauw;
        exe_t        *exe = NULL;
        StrParam_t    auth;
        int           i;

        memset(&eiaauw, 0, sizeof(exe_eiaauw_t));

        eiaauw.cid = Parser_GetIntParam(parser_p, &error_flag, NULL);

        if ((MIN_CID > eiaauw.cid) || (MAX_CID < eiaauw.cid) || error_flag) {
            Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
            result = AT_CME_ERROR;
            goto error;
        }

        eiaauw.bearer_type = (exe_pscc_bearer_type_t)Parser_GetIntParam(parser_p, &error_flag, NULL);

        /* Check if bearer is valid. Only PS is supported */
        if (EXE_PSCC_BEARER_TYPE_PS != eiaauw.bearer_type || error_flag) {
            Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
            result = AT_CME_ERROR;
            goto error;
        }

        eiaauw.userid_p = Parser_GetStrParam(parser_p, &error_flag, NULL);

        if ((NULL == eiaauw.userid_p) || error_flag) {
            Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
            result = AT_CME_ERROR;
            goto error;
        }

        eiaauw.password_p = Parser_GetStrParam(parser_p, &error_flag, NULL);

        if ((NULL == eiaauw.password_p) || error_flag) {
            Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
            result = AT_CME_ERROR;
            goto error;
        }

        auth = Parser_GetBinParamAsStr(parser_p, &error_flag, NULL);

        if ((NULL == auth) || error_flag) {
            Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
            result = AT_CME_ERROR;
            goto error;
        }

        if ((strcmp(auth, "00001") == 0) || ((strcmp(auth, "001") == 0))) { /*PAP and CHAP is never performed.*/
            eiaauw.auth_prot = EXE_PSCC_AUTH_NONE;
        } else if ((strcmp(auth, "00011") == 0) || ((strcmp(auth, "011") == 0))) { /*PAP may be performed; CHAP is never performed.*/
            eiaauw.auth_prot = EXE_PSCC_AUTH_PAP;
        } else if ((strcmp(auth, "00101") == 0) || (strcmp(auth, "101") == 0)) { /*CHAP may be performed; PAP is never performed.*/
            eiaauw.auth_prot = EXE_PSCC_AUTH_CHAP;
        } else if ((strcmp(auth, "00111") == 0) || (strcmp(auth, "111") == 0)) { /*PAP / CHAP may be performed - baseband dependent.*/
            eiaauw.auth_prot = EXE_PSCC_AUTH_PAP_OR_CHAP;
        } else {
            Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
            result = AT_CME_ERROR;
            goto error;
        }

        eiaauw.ask4pwd = (bool)Parser_GetIntParam(parser_p, &error_flag, NULL);

        /* If MMI asked for user id and password. Only OFF=0 is currently supported */
        if (eiaauw.ask4pwd || error_flag) {
            Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
            result = AT_CME_ERROR;
            goto error;
        }

        exe = atc_get_exe_handle();
        exe_res = exe_request(exe, EXE_EIAAUW, (void *)&eiaauw, (void *)parser_p, &parser_p->request_handle);
        result = parser_handle_exe_result(exe_res, parser_p);
        break;
    }
    case AT_MODE_TEST: {
        sprintf((char *) info_text, ": (1-25),(1-4),\"\",\"\",(00001-11111),(0-1)");
        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        break;
    }
    default:
        result = AT_ERROR;
        break;
    }

    return result;

error:
    return result;
}

/*
 *===============================================================================
 *
 *  Function: AT_STAR_ENAP_Handle
 *
 *  Input:    parser_p   - Pointer to parser to which AT protocol response
 *                         eventually shall be sent.
 *
 *  Output:   parser_p   - Pointer to submitted parser, with state
 *                         Pending changed to true.
 *
 *  OUTPUT:  Parser->
 *           Pos        - The current position in the command line.
 *
 *  RETURNS: A status code for the execution of the command is returned:
 *           AT_OK        - The command was successfully handled.
 *           AT_ERROR     - There was a general error.
 *           AT_CME_ERROR - There was a GSM specific error, the error code is
 *                          returned in info_text.
 *
 *===============================================================================
 */

AT_Command_e AT_STAR_ENAP_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message)
{
    AT_Command_e result = AT_ERROR;
    exe_request_result_t exe_res = EXE_FAILURE;
    bool error_flag = false;

    switch (parser_p->Mode) {
    case AT_MODE_SET: {
        ATC_LOG_I("AT*ENAP started...");

        if ((!parser_p->IsPending)) {
            int state = 0;        /* 0 = Disconnect, 1 = Connect */
            exe_enap_t enap_request;
            exe_t *exe_p;
            int cid = 0;
            int bearer_type = 1; /* 1 = PS */

            /* Parse the input parameters... */
            state = Parser_GetIntParam(parser_p, &error_flag, NULL);

            if (state > 1 || error_flag) {
                ATC_LOG_E("incorrect state.");
                return AT_ERROR;
            }

            cid = Parser_GetIntParam(parser_p, &error_flag, NULL);

            if ((cid < MIN_CID) || (cid > MAX_CID) || error_flag) {
                ATC_LOG_E("incorrect cid.");
                return AT_ERROR;
            }

            bearer_type = Parser_GetIntParam(parser_p, &error_flag, (IntParam_t *)&bearer_type);

            if ((bearer_type != 1) || error_flag) {
                /* Only PS = 1 is supported */
                ATC_LOG_E("incorrect bearer type.");
                return AT_ERROR;
            }

            if (!parser_p->NoMoreParams) {
                ATC_LOG_E("parameter not expected.");
                return AT_ERROR;
            }

            exe_p = atc_get_exe_handle();
            enap_request.cid = cid;

            /* Setup a NAP connection:*/
            switch (state) {
            case 0:
                exe_res = exe_request(exe_p, EXE_ENAP_DISCONNECT, (void *)&enap_request,
                                      (void *)parser_p, &parser_p->request_handle);
                break;
            case 1:
                exe_res = exe_request(exe_p, EXE_ENAP_CONNECT, (void *)&enap_request,
                                      (void *)parser_p, &parser_p->request_handle);
                break;
            default:
                break;
            }
        } else {
            /* Handle response from executer */
            exe_res = message->exe_result_code;
        }

        result = parser_handle_exe_result(exe_res, parser_p);
        break;
    }
    case AT_MODE_READ: {
        if ((!parser_p->IsPending)) {
            exe_enap_t enap_request;
            exe_t *exe_p = NULL;

            exe_p = atc_get_exe_handle();
            exe_res = exe_request(exe_p, EXE_ENAP_READ, (void *)&enap_request,
                                  (void *)parser_p, &parser_p->request_handle);

            if (exe_res == EXE_SUCCESS) {
                /* case when sterc_handle is not assigned to cid */
                AT_AddValue(info_text, ": 0", 0);
                Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
            }
        } else {
            /* Handle response from executer */
            exe_enap_t *resp_data_p = (exe_enap_t *)message->data_p;
            exe_res = message->exe_result_code;

            if (exe_res == EXE_SUCCESS) {
                if (NULL == resp_data_p) {
                    ATC_LOG_E("resp_data_p is NULL");
                    break;
                }

                AT_AddValue(info_text, ": 0", resp_data_p->state);
                Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
            }
        }

        result = parser_handle_exe_result(exe_res, parser_p);
        break;
    }
    case AT_MODE_TEST: {
        sprintf((char *) info_text, ": (0-1),(1-99),1");
        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        result = AT_OK;
        break;
    }
    default:
        result = AT_ERROR;
        break;
    }

    return result;
}

/*
 *===============================================================================
 *
 *  Function: AT_STAR_EUPLINK_Handle
 *
 *  This function sends <bytes> of data over the activated PDP context corresponding to
 *  <cid>. Note that the PDP context must be activated prior to this AT command being issued.
 *
 *  Input:    parser_p   - Pointer to parser to which AT protocol response
 *                         eventually shall be sent.
 *
 *  Output:   parser_p   - Pointer to submitted parser, with state
 *                         Pending changed to true.
 *
 *  OUTPUT:  Parser->
 *           Pos        - The current position in the command line.
 *
 *  RETURNS: A status code for the execution of the command is returned:
 *           AT_OK        - The command was successfully handled.
 *           AT_ERROR     - There was a general error.
 *           AT_CME_ERROR - There was a GSM specific error, the error code is
 *                          returned in info_text.
 *
 *
 *===============================================================================
 */
AT_Command_e AT_STAR_EUPLINK_Handle(AT_ParserState_s *parser_p,
                                    AT_CommandLine_t info_text,
                                    AT_ResponseMessage_t message)
{
    AT_Command_e result  = AT_OK;
    exe_request_result_t exe_res = EXE_FAILURE;

    switch (parser_p->Mode) {
    case AT_MODE_SET: {
        if (!parser_p->IsPending) {
            exe_euplink_t  uplink;
            bool           error_flag = false;
            exe_t         *exe_p      = NULL;

            uplink.cid = 0; /* Invalid CID value */
            uplink.size = EXE_PSCC_DEFAULT_UPLINK_DATA_SIZE;

            uplink.cid = Parser_GetIntParam(parser_p, &error_flag, NULL);

            if ((uplink.cid < MIN_CID) || (uplink.cid > MAX_CID) || error_flag) {
                /* Invalid cid range */
                Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
                result = AT_CME_ERROR;
            }

            if (!parser_p->NoMoreParams) {
                uplink.size = Parser_GetLongIntParam(parser_p, &error_flag, NULL, EXE_PSCC_UPLINK_DATA_MAX_SIZE);

                if ((uplink.size < 0) || (uplink.size > EXE_PSCC_UPLINK_DATA_MAX_SIZE) || error_flag) {
                    /* Invalid size range */
                    Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
                    result = AT_CME_ERROR;
                }

            }

            if (!parser_p->NoMoreParams) {
                result = AT_ERROR;
            }

            if (AT_OK == result) {
                exe_p = atc_get_exe_handle();
                exe_res = exe_request(exe_p, EXE_EUPLINK, (void *)&uplink,
                                      (void *)parser_p, &parser_p->request_handle);
            }
        } else {
            /* Handle response from executer */
            exe_res = message->exe_result_code;
        }

        if (AT_OK == result) {
            result = parser_handle_exe_result(exe_res, parser_p);
        }

        break;
    }
    default:
        result = AT_ERROR;
        break;
    }

    return result;
}

static void free_cgcmod_list(exe_cgcmod_set_request_t *cgcmod_p)
{
    if (NULL != cgcmod_p) {
        exe_cgcmod_set_request_cid_t *next_p = NULL;
        exe_cgcmod_set_request_cid_t *current_p = cgcmod_p->cid_p;

        while (NULL != current_p) {
            next_p = current_p->next_p;
            free((void *)current_p);
            current_p = next_p;
        }

        free(cgcmod_p);
    }
}

/*
 * Copies all active cids from an cgact read response to an cgcmod request
 */
static bool copy_cid_from_cgact_to_cgcmod_list(exe_cgact_read_response_t *cgact_p, exe_cgcmod_set_request_cid_t **cgcmod_pp)
{
    bool first = true;
    exe_cgcmod_set_request_cid_t *prev_p = NULL;

    while (NULL != cgact_p) {
        exe_cgcmod_set_request_cid_t *new_p = (exe_cgcmod_set_request_cid_t *) calloc(1,
                                              sizeof(exe_cgcmod_set_request_cid_t));

        if (NULL == new_p) {
            return false;
        }

        if (NULL != prev_p) {
            prev_p->next_p = new_p;
        }

        if (first) {
            *cgcmod_pp = new_p;
            first = false;
        }

        prev_p = new_p;
        new_p->cid = cgact_p->cid;
        cgact_p = cgact_p->next_p;
    }

    return true;
}

bool add_cgpaddr_cid_to_list(int cid, exe_cgpaddr_cid_t **exe_cgpaddr_cid_pp)
{
    exe_cgpaddr_cid_t *new_p = NULL;
    bool result = false;
    new_p = (exe_cgpaddr_cid_t *) malloc(sizeof(exe_cgpaddr_cid_t));

    if (NULL == new_p) {
        ATC_LOG_E("Failed to alloc memory");
        result = false;
        return result;
    }

    new_p->cid = cid;
    new_p->next_p = NULL;

    if (NULL == *exe_cgpaddr_cid_pp) {
        result = true;
        *exe_cgpaddr_cid_pp = new_p;
    } else {
        exe_cgpaddr_cid_t *tmp_p = *exe_cgpaddr_cid_pp;

        while (tmp_p->next_p != NULL) {
            tmp_p = tmp_p->next_p;
        }

        tmp_p->next_p = new_p;
        result = true;
    }

    return result;
}

void free_cgpaddr_cid_list(exe_cgpaddr_cid_t *exe_cgpaddr_cid_p)
{
    exe_cgpaddr_cid_t *next_p = NULL;

    while (NULL != exe_cgpaddr_cid_p) {
        next_p = exe_cgpaddr_cid_p->next_p;
        free((void *) exe_cgpaddr_cid_p);
        exe_cgpaddr_cid_p = next_p;
    }
}

/*
 * Removes all cids that are not active from a cgact read response.
 */
static void get_active_contexts(exe_cgact_read_response_t **response_data_pp)
{
    exe_cgact_read_response_t *prev_p = NULL;
    exe_cgact_read_response_t *iterator_p = *response_data_pp;
    exe_cgact_read_response_t *tmp_p = NULL;

    while (NULL != iterator_p) {
        if (0 == iterator_p->state) {
            /* state inactive, remove this entry */
            if (NULL == prev_p) {
                /* first in list*/
                *response_data_pp = iterator_p->next_p;
            } else {
                prev_p->next_p = iterator_p->next_p;
            }

            tmp_p = iterator_p;
            iterator_p = iterator_p->next_p;
            free(tmp_p);
        } else {
            prev_p = iterator_p;
            iterator_p = iterator_p->next_p;
        }
    }
}
/*
 * Returns true if all input cids exist among active cids.
 */
static bool match_input_active_cids(exe_cgcmod_set_request_cid_t *input_cids_p,
                                    exe_cgact_read_response_t *active_cid_p)
{
    bool found;
    exe_cgact_read_response_t *first_p = active_cid_p;

    while (NULL != input_cids_p) {
        found = false;
        active_cid_p = first_p;

        while (NULL != active_cid_p) {
            if (input_cids_p->cid == active_cid_p->cid) {
                found = true;
                break;
            }

            active_cid_p = active_cid_p->next_p;
        }

        if (!found) {
            ATC_LOG_E("AT+CGCMOD cid: %d not active", input_cids_p->cid);
            return false;
        }

        input_cids_p = input_cids_p->next_p;
    }

    return true;
}

/*
 * Returns true if the exponential value of the string finds its match in the vector of allowed_values.
 *
 * (An empty string is also accepted.)
 *
 * Match is considered correct if there is one value-pair in the allowed_values struct where the first character
 * in the string is a number with the value of mantissa, the second character is
 * the ascii character "E",  the third character is a number with the value of the exponent, and the fourth character
 * is the string terminator ('\0').
 * */
bool check_exponential(StrParam_t string, exe_exponential_t *allowed_values, int allowed_values_length)
{
    if (string != NULL && '\0' == string[0]) {
        return true;
    }

    if (string != NULL &&
            '0' <= string[0] &&
            '9' >= string[0] &&
            'E' == string[1] &&
            '0' <= string[2] &&
            '9' >= string[2] &&
            '\0' == string[3]) {
        int i = 0;
        uint8_t mantissa = string[0] - '0';
        uint8_t exponent = string[2] - '0';

        while (i < allowed_values_length) {
            if (allowed_values[i].mantissa == mantissa && allowed_values[i].exponent == exponent) {
                return true;
            }

            i++;
        }

    }

    return false;
}
