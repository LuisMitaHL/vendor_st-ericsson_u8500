/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "ctype.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <atc.h>
#include <atc_config.h>
#include <atc_exe_glue.h>
#include <atc_handlers.h>
#include <atc_log.h>
#include <atc_parser.h>
#include <atc_parser_util.h>
#include <exe.h>

#define SIZE_OF_CLIP_RESPONSE (MAX_DIGITS_IN_NUMBER+4+41+4+MAX_DIGITS_IN_NUMBER+2)
#define SIZE_OF_ECAV_RESPONSE (5+4+4+4+4+3+MAX_DIGITS_IN_NUMBER+4+1)
#define ATC_RING_TEXT "RING"
#define ATC_CRING_VOICE_TEXT "+CRING: VOICE"

typedef enum {
    CALL_MODE_SINGLE                    = 0,
    CALL_MODE_ALTERNING_VOICE_FAX       = 1,
    CALL_MODE_ALTERNING_VOICE_DATA      = 2,
    CALL_MODE_VOICE_FOLLOWED_BY_DATA    = 3
} call_mode;


#define ATC_CHECK_GOTO_ERROR(a) if (!(a)) goto error
#define GPRS_SC    99
#define L2P_PPP    1

/*
 *===============================================================================
 *
 *   Function: AT_A_Handle
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
 *
 *   The ATA command answers a call.
 *
 *   DO     - Returns OK
 *
 *===============================================================================
 */
AT_Command_e AT_A_Handle(AT_ParserState_s *parser_p,
                         AT_CommandLine_t info_text,
                         AT_ResponseMessage_t message)
{
    AT_Command_e result = AT_ERROR;
    exe_t *exe_p;
    exe_request_result_t exe_res = EXE_FAILURE;
    ATC_LOG_I("Inside AT_A_Handle");

    switch (parser_p->Mode) {
    case AT_MODE_DO:

        if (!parser_p->IsPending) {
            exe_p = atc_get_exe_handle();
            exe_res = exe_request(exe_p, EXE_A, NULL,
                                  (void *)parser_p, &parser_p->request_handle);

            if (EXE_PENDING == exe_res) {
                result = AT_PENDING;
            }
        } else { /* Pending */
            if (message->exe_result_code == EXE_SUCCESS) {
                result = AT_OK;
            }
        }

        break;
    default:
        result = AT_ERROR;
    }

    return result;
}

/*******************************************************************************
*
*   Function: AT_PLUS_CLCC_Handle
*
*   Input:  parser_p      - Pointer to current parser state.
*           info_text    - Pointer to a string buffer to put information text.
*           Response_p  - Not used.
*
*   Output: parser_p->
*           Pos       - The current position in the command line.
*
*   Returns: A status code for the execution of the command is returned:
*            AT_OK        - The command was successfully handled.
*            AT_ERROR     - There was a general error.
*
*   Description: The command returns a list of current calls in the ME.
*
*                Regarding multiparty calls:
*                Lets say we have three parties A, B and C.
*                A calls B
*                B answers
*                A puts B on hold
*
*                A calls C
*                C answers
*                A joins the calls
*
*                By definition they are now all in a multiparty call.
*                B and C is notified by the network that the current call is a multiparty call.
*                It is this notification that we listen to and thereby change the MPTY_Indicator flag.
*                However A does not get this notification.
*
*******************************************************************************/
AT_Command_e AT_PLUS_CLCC_Handle(AT_ParserState_s *parser_p, AT_CommandLine_t info_text, AT_ResponseMessage_t response_p)
{
    AT_Command_e result = AT_ERROR;
    exe_t *exe_p;
    exe_request_result_t exe_res = EXE_FAILURE;
    ATC_LOG_I("Inside %s", __FUNCTION__);

    switch (parser_p->Mode) {
    case AT_MODE_DO:

        if (!parser_p->IsPending) {
            exe_p = atc_get_exe_handle();
            exe_res = exe_request(exe_p, EXE_CLCC, NULL,
                                  (void *)parser_p, &parser_p->request_handle);
            result = parser_handle_exe_result(exe_res, parser_p);
        } else {
            if (response_p) {
                result = parser_handle_exe_result(response_p->exe_result_code, parser_p);

                if (AT_OK == result &&
                        response_p->data_p) {

                    exe_clcc_response_t *clcc_resp_p = (exe_clcc_response_t *)response_p->data_p;
                    int i;

                    for (i = 0; i < clcc_resp_p->number_of_calls; i++) {
                        /* The response might be in several lines. Clear the string first! */
                        *info_text = END_OF_STRING;
                        /* <idx> */
                        AT_AddValue(info_text, ": 0,", clcc_resp_p->call_info[i].id);
                        /* <dir> */
                        AT_AddValue(info_text, "0,", clcc_resp_p->call_info[i].direction);
                        /* <state> */
                        AT_AddValue(info_text, "0,", clcc_resp_p->call_info[i].state);
                        /* <mode> */
                        AT_AddValue(info_text, "0,", clcc_resp_p->call_info[i].mode);
                        /* <mpty> */
                        AT_AddValue(info_text, "0,", clcc_resp_p->call_info[i].multipart);
                        /* <number> */
                        strcat((char *)info_text, "\"");

                        if (clcc_resp_p->call_info[i].type == 145 ) {
                            /* If it is an international number and '+' is not
                               included as prefix, we need to add it. */
                            if (clcc_resp_p->call_info[i].number[0] != '+' && clcc_resp_p->call_info[i].number[0] != 0) {
                                strcat((char *)info_text, "+");
                            }
                        }

                        strncat((char *)info_text, clcc_resp_p->call_info[i].number,
                                sizeof(clcc_resp_p->call_info[i].number) - 1);
                        strcat((char *)info_text, "\",");
                        /* <type> */
                        AT_AddValue(info_text, "0,", clcc_resp_p->call_info[i].type);
                        /* alpha and priority not supported */
                        strcat((char *)info_text, ",,");
                        /* CLI validity */
                        AT_AddValue(info_text, "0", clcc_resp_p->call_info[i].cli_validity);

                        /* Send response for each call. */
                        Parser_SendMultipleResponses(parser_p, AT_NO_OVERRIDE, info_text, i == clcc_resp_p->number_of_calls - 1);
                    }
                } else {
                    /* Unexpectedly no data pointer ! */
                    Parser_SetCMEE_ErrorType(parser_p, CMEE_NOT_FOUND);
                    result = AT_ERROR;
                }
            } else {
                Parser_SetCMEE_ErrorType(parser_p, CMEE_NOT_FOUND);
                result = AT_ERROR;
            }
        }

        break;

    case AT_MODE_TEST:
        result = AT_OK;
        break;

    default:
        result = AT_ERROR;
        break;
    }

    return result;
} /* End of AT_PLUS_CLCC_Handle */


/*
 *===============================================================================
 *
 *   Function: AT_D_Handle
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
 *   The ATD command starts a voice or data call.
 *
 *   DO     - Returns OK
 *
 *===============================================================================
 */
AT_Command_e AT_D_Handle(AT_ParserState_s *parser_p,
                         AT_CommandLine_t info_text,
                         AT_ResponseMessage_t message)
{
    ATC_LOG_I("Entry");
    static const char *valid_dial_chars_p = "1234567890*#+ABCD,!W@>GgIi";
    unsigned char *end_index_p;
    exe_cmee_error_t cme_err;
    AT_Command_e result = AT_ERROR;
    bool isVoiceCall = false;
    bool isInsideQuote = false;

    switch (parser_p->Mode) {
    case AT_MODE_DO: {
        if (!parser_p->IsPending) {
            exe_atd_data_t atd_data;

            /* Skip any leading 'T' (tone dialling), or 'P' (pulse dialling) modifier. */
            if ('T' == (0xDF & *parser_p->Pos) || 'P' == (0xDF & *parser_p->Pos)) {
                parser_p->Pos++;
            }

            /* If ATD ends with a ; it means voice call, if not it means a data call, so
             * first we need to check what kind it is
             */
            end_index_p = parser_p->Pos;

            while (*end_index_p != AT_COMMAND_SEPARATOR && *end_index_p
                    != END_OF_STRING) {

                /* Track if characters are inside double-quotes (") or not */
                if ('\"' == *end_index_p) {
                    isInsideQuote = !isInsideQuote;
                } else {
                    if (!isInsideQuote) {
                        /* Check that dial string contains only allowed characters.
                         * Except for characters inside double quotes.
                         * A more thorough check will be made later. */
                        char *valid_char_p = strchr(valid_dial_chars_p, (char) * end_index_p);

                        if (NULL == valid_char_p) {
                            ATC_LOG_E("Invalid character ('%c') found in dial string (%s)!", *end_index_p, (char *)parser_p->Pos);
                            Parser_SetCMEE_ErrorType(parser_p, CMEE_INVALID_CHARACTER_IN_DIAL_STRING);
                            result = AT_CME_ERROR;
                            goto exit;
                        }
                    }
                }

                end_index_p++;
            }

            if (isInsideQuote) {
                ATC_LOG_E("Dial string contains an open quote!");
                Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
                result = AT_CME_ERROR;
                goto exit;
            }

            /* ATD can be followed by other AT-commands.
             * Check if voice call is indicated when followed by other command */
            if (*end_index_p == AT_COMMAND_SEPARATOR && *(end_index_p + 1) != END_OF_STRING) {
                if (*(end_index_p + 1) == AT_COMMAND_SEPARATOR) {
                    /* We have a following command and call is voice call */
                    isVoiceCall = true;
                    /* Due to the special ATD case ';' indicating voice call,
                     * increment end_index so that common parser indicates correct command separator */
                    end_index_p++;
                    ATC_LOG_I("Voice call with following command");
                } else {
                    /* We have a following command and call is not voice call */
                    isVoiceCall = false;
                    ATC_LOG_I("Data call with following command");
                }
            } else if (*end_index_p == AT_COMMAND_SEPARATOR && *(end_index_p + 1) == END_OF_STRING) {
                /* Voice call with no following command */
                isVoiceCall = true;
                /* Due to the special ATD case ';' indicating voice call,
                 * increment end_index so that common parser indicates correct command separator */
                end_index_p++;
                ATC_LOG_I("Voice call with NO following command");
            } else {
                /* Data call with no following command */
                isVoiceCall = false;
                ATC_LOG_I("Data call with NO following command");
            }

            if (!isVoiceCall) { /* Data Call */

                /* Syntax: ATD*<GPRS_SC>[[*<called_address>][*<L2P>][*cid]]#
                 * <GPRS_SC>         - expected value = 99. NO CARRIER is returned for other values (e.g. 98 or 97).
                 * <called_address>  - always ignored.
                 * <L2P>             - expected value = 1 (PPP) ERROR returned for other values.
                 * <cid>             - See +CGDCONT for specification.
                 *
                 * Accepted syntax variants:
                 * ATD*99#               - No CID or L2P provided. Let the UE decide what values to use.
                 * ATD*99***<cid>#       - CID provided but no L2P. Let the UE decide which L2P value to use.
                 * ATD*99**<l2p>*<cid>#  - CID and L2P provided.
                 * ATD*99**<l2p>*#       - L2P provided but no CID. Let the UE decide which CID value to use.
                 * ATD*99***#            - No CID or L2P provided. Let the UE decide what values to use.
                 */

                IntParam_t cid = 0;
                IntParam_t l2p = 0;
                bool err = false;
                IntParam_t gprs_sc = 0;
                exe_atd_psdun_t dun;

                ATC_LOG_I("Data call");

                /* Skip the first 'd' character */
                parser_p->Pos++;

                gprs_sc = Parser_GetDIntParam(parser_p, &err, NULL);

                if (gprs_sc != GPRS_SC || err) {
                    Parser_SetCMEE_ErrorType(parser_p, CMEE_OPERATION_NOT_SUPPORTED);
                    result = AT_CME_ERROR;
                    goto exit;
                }

                if (!parser_p->NoMoreParams) {
                    /* Since <called_adress> is ignored, we must jump passed it. */
                    char *substr = strchr((char *)parser_p->Pos, '*');

                    if (substr) {
                        parser_p->Pos = (AT_CommandLine_t)substr;
                    }

                    parser_p->Pos++;
                }

                memset(&dun, 0, sizeof(exe_atd_psdun_t));

                if (!parser_p->NoMoreParams) {
                    l2p = Parser_GetDIntParam(parser_p, &err, NULL);

                    /* either omitted or valid parameter is expected */
                    if (!err && L2P_PPP != l2p) {
                        Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
                        result = AT_CME_ERROR;
                        goto exit;
                    }

                    dun.l2p = l2p;
                }

                if (!parser_p->NoMoreParams) {
                    IntParam_t defaultCidValue = 255;
                    cid = Parser_GetDIntParam(parser_p, &err, &defaultCidValue);

                    if (!err && defaultCidValue == cid) {
                        /* Omitted CID, use whatever is known by the UE. */
                        dun.cid = -1;
                    } else if ((cid < MIN_CID) || (cid > MAX_CID)) {
                        /* Range check CID value*/
                        Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
                        result = AT_CME_ERROR;
                        goto exit;
                    } else {
                        dun.cid = cid;
                    }
                }

                if (!parser_p->NoMoreParams) {
                    /* Too many parameters */
                    Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
                    result = AT_CME_ERROR;
                    goto exit;
                }

                /* Omitted CID, use whatever is known by the UE. */
                if (0 == dun.cid) {
                    dun.cid = -1;
                }

                {
                    exe_request_result_t executor_result = exe_request(atc_get_exe_handle(),
                                                           EXE_PS_DIAL_UP,
                                                           (void *)&dun,
                                                           (void *)parser_p,
                                                           &parser_p->request_handle);
                    result = parser_handle_exe_result(executor_result, parser_p);

                    if (AT_PENDING == result) {
                        parser_p->atd_call_type = ATD_CALL_TYPE_PS_DATA;
                    } else if (AT_OK == result) {
                        result = AT_CONNECT;
                    } else {
                        result = AT_NO_CARRIER;
                    }
                }
            } else {  /* Voice call */
                char *dial_string_p;

                /* Is it external it should be handle by at_service */
                if (ATC_CONNECTION_TYPE_EXTERNAL == parser_p->connection_type) {
                    parser_p->Command = AT_UNKNOWN_COMMAND;
                    result = AT_ERROR;
                    goto exit;
                }

                /* We need to use the heap here as the input string can be a lot longer than
                 * MAX_DIGITS_IN_NUMBER but still valid as long as the extra characters
                 * are to be ignored and the output is within range. */
                if ((dial_string_p = (char *)malloc(strlen((char *)parser_p->Pos) + 1)) == NULL) {
                    ATC_LOG_E("No resources to allocate");
                    Parser_SetCMEE_ErrorType(parser_p, CMEE_MEMORY_FULL);
                    result = AT_CME_ERROR;
                    goto exit;
                }

                strcpy(dial_string_p, (char *)parser_p->Pos);

                /* Creates a valid dial string, checks that length is within range  and
                 * that i/I, if included, are correctly placed */
                if (!non_dial_characters_successfully_removed(dial_string_p, &cme_err)) {
                    Parser_SetCMEE_ErrorType(parser_p, cme_err);
                    result = AT_CME_ERROR;
                    free((void *)dial_string_p);
                    goto exit;
                }

                memset(&atd_data, 0, sizeof(atd_data));
                atd_data.phone_number_p = dial_string_p;

                /* Find CLIR */
                if (strchr((char *)parser_p->Pos, 'I')) {
                    atd_data.clir = CLIR_CMD_INVOKE;
                } else if (strchr((char *)parser_p->Pos, 'i')) {
                    atd_data.clir = CLIR_CMD_SUPPRESS;
                } else {
                    atd_data.clir = CLIR_CMD_DEFAULT;
                }

                if (parser_p->CUUS1_setup_msg.data) {
                    atd_data.cuus1_data_p = &(parser_p->CUUS1_setup_msg);
                }

                /* Note: g/G is not used by mal so it is ignored on purpose here */

                /* Call the executer */
                {
                    exe_request_result_t exe_res = exe_request(atc_get_exe_handle(),
                                                   EXE_DIAL_UP,
                                                   &atd_data,
                                                   parser_p,
                                                   &parser_p->request_handle);
                    result = parser_handle_exe_result(exe_res, parser_p);

                    if (AT_PENDING == result) {
                        parser_p->atd_call_type = ATD_CALL_TYPE_CS_VOICE;
                    }
                }

                /* Due to the special ATD case ';' indicating voice call,
                 * set parser_p->pos to end_index so that common parser indicates correct command separator */
                parser_p->Pos = end_index_p;
                free((void *)dial_string_p);
            }
        } else { /* parser_p->IsPending */
            if (EXE_SUCCESS == message->exe_result_code) {
                if (ATD_CALL_TYPE_CS_VOICE == parser_p->atd_call_type) {
                    parser_p->connecting_voice_call = true;
                    result = AT_OK;
                } else {
                    result = AT_CONNECT;
                }
            } else {
                exe_atd_response_t *response_p = (exe_atd_response_t *)message->data_p;

                if (NULL != response_p && response_p->error_code != CMEE_OPERATION_NOT_ALLOWED) {
                    Parser_SetCMEE_ErrorType(parser_p, response_p->error_code);
                    result = AT_CME_ERROR;
                } else {
                    if (ATD_CALL_TYPE_CS_VOICE == parser_p->atd_call_type) {
                        parser_p->connecting_voice_call = true;
                        /* Outputting "OK" on error is done to comply with ITU
                         * V.250 recommendation and 3GPP 27.007 for voice calls.
                         * The call-state change event handler will see to that
                         * the "OK" is followed by a "NO CARRIER" as required by
                         * the specifications listed above. */
                        result = AT_OK;
                    } else {
                        result = AT_NO_CARRIER;
                    }
                }
            }
        }

        break;
    }
    default:
        result = AT_ERROR;
        break;
    }

exit:
    ATC_LOG_D("Exit, result=%d", result);
    return result;
}

AT_Command_e AT_PLUS_CHLD_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message)
{
    AT_Command_e result = AT_ERROR;

    switch (parser_p->Mode) {
    case AT_MODE_SET:

        if (!parser_p->IsPending) {
            bool error_flag = false;
            IntParam_t n = 0;
            exe_t *exe_p;
            exe_request_result_t exe_res = EXE_FAILURE;
            exe_chld_t exe_req;
            n = Parser_GetIntParam(parser_p, &error_flag, &n);
            exe_p = atc_get_exe_handle();

            if (!error_flag && exe_p && parser_p->NoMoreParams) {
                exe_req.n = n;
                exe_res =
                    exe_request(exe_p, EXE_CHLD, &exe_req,
                                (void *)parser_p, &parser_p->request_handle);
                result = parser_handle_exe_result(exe_res, parser_p);
            }
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
    case AT_MODE_TEST:
        sprintf((char *) info_text, ": (0,1,1x,2,2x,3,4)");
        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        result = AT_OK;
        break;
    default:
        result = AT_ERROR;
    }

    return result;
}

/*
 *===============================================================================
 *
 *   Function: AT_PLUS_CLIP_Handle
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
 *   The command enables, disables, reads or tests Calling Line Identification
 *   Presentation.
 *
 *===============================================================================
 */
AT_Command_e AT_PLUS_CLIP_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message)
{
    ATC_LOG_I("Inside AT_PLUS_CLIP_Handle");
    AT_Command_e result = AT_ERROR;

    switch (parser_p->Mode) {
    case AT_MODE_READ:

        if (!parser_p->IsPending) {
            exe_t *exe_p;
            exe_request_result_t exe_res = EXE_FAILURE;
            exe_p = atc_get_exe_handle();

            if (NULL != exe_p) {
                exe_res =
                    exe_request(exe_p, EXE_CLIP_READ, NULL,
                                (void *)parser_p, &parser_p->request_handle);
                result = parser_handle_exe_result(exe_res, parser_p);
            }
        } else {
            if (message) {
                sprintf((char *)info_text, ": %d,%d", parser_p->CLIP,
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
    default:
        result = AT_Flag_Handle(parser_p, info_text, &parser_p->CLIP, 1);
        break;
    }

    return result;
}

AT_Command_e AT_PLUS_CLIR_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message)
{

    AT_Command_e result = AT_ERROR;

    switch (parser_p->Mode) {
    case AT_MODE_SET:

        if (!parser_p->IsPending) {
            bool error_flag = false;

            IntParam_t n;
            IntParam_t default_val = 0;

            exe_t *exe_p;
            exe_request_result_t exe_res = EXE_FAILURE;
            exe_clir_t exe_req;

            n = Parser_GetIntParam(parser_p, &error_flag, &default_val);

            if (n > 2) {
                Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
                result = AT_CME_ERROR;
            } else {
                exe_p = atc_get_exe_handle();

                if (!error_flag && exe_p && parser_p->NoMoreParams) {
                    exe_req.n = n;
                    exe_res =
                        exe_request(exe_p, EXE_CLIR, &exe_req,
                                    (void *)parser_p, &parser_p->request_handle);
                    result = parser_handle_exe_result(exe_res, parser_p);
                }
            }
        } else {
            if (EXE_SUCCESS == message->exe_result_code) {
                result = AT_OK;
            } else {
                Parser_SetCMEE_ErrorType(parser_p,
                                         CMEE_OPERATION_NOT_ALLOWED);
                result = AT_CME_ERROR;
            }
        }

        break;
    case AT_MODE_READ: {
        exe_request_result_t exe_res = EXE_FAILURE;

        if (!parser_p->IsPending) {
            exe_clir_read_t clir_read_response;
            exe_t *exe_p;
            exe_p = atc_get_exe_handle();
            exe_res = exe_request(exe_p, EXE_CLIR_READ, (void *) &clir_read_response,
                                  (void *)parser_p, &parser_p->request_handle);
            result = parser_handle_exe_result(exe_res, parser_p);
        } else {
            if (NULL != message) {
                result = parser_handle_exe_result(message->exe_result_code, parser_p);

                if ((AT_OK == result) && (NULL != message->data_p)) {
                    sprintf((char *) info_text, ": %d,%d", ((exe_clir_read_t *)message->data_p)->n,
                            ((exe_clir_read_t *)message->data_p)->m);
                    Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
                    result = AT_OK;
                }
            } else {
                Parser_SetCMEE_ErrorType(parser_p, CMEE_NOT_FOUND);
                result = AT_ERROR;
            }
        }

        break;
    }
    case AT_MODE_TEST:
        sprintf((char *) info_text, ": (0-2)");
        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        result = AT_OK;
        break;
    default:
        result = AT_ERROR;
    }

    return result;
}

AT_Command_e AT_H_Handle(AT_ParserState_s *parser_p,
                         AT_CommandLine_t info_text,
                         AT_ResponseMessage_t message)
{
    AT_Command_e result = AT_ERROR;
    ATC_LOG_I("Inside AT_H_Handle");

    switch (parser_p->Mode) {
    case AT_MODE_DO:
    case AT_MODE_SET:

        if (!parser_p->IsPending) {
            bool error_flag = false;
            IntParam_t n = 0;
            exe_t *exe_p;
            exe_request_result_t exe_res = EXE_FAILURE;
            n = Parser_GetIntParam(parser_p, &error_flag, &n);

            exe_p = atc_get_exe_handle();

            if (!error_flag && exe_p && parser_p->NoMoreParams && n == 0) {
                exe_res =
                    exe_request(exe_p, EXE_HOOK, NULL,
                                (void *)parser_p, &parser_p->request_handle);
                result = parser_handle_exe_result(exe_res, parser_p);
            }
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
    case AT_MODE_TEST:
        sprintf((char *) info_text, ": (0)");
        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        result = AT_OK;
        break;
    default:
        result = AT_ERROR;
    }

    return result;
}

/*
*===============================================================================
*
*   Function: AT_PLUS_CMOD_Handle
*
*   INPUT:   parser_p   - Pointer to the current parser state.
*            info_text  - Pointer to a string buffer to put information text.
*            message    - Pointer to the response message used for pending commands.
*
*   OUTPUT:  parser_p->
*              Pos      - The current position in the command line.
*
*   RETURNS: A status code for the execution of the command is returned:
*            AT_OK        - The command was successfully handled.
*            AT_ERROR     - There was a general error.
*            AT_CME_ERROR - Invalid parameters
*
*   The AT+CMOD command is used to set call mode prior to
*   or answering a call. Since the only mode that is supported is CALL_MODE_SINGLE
*   i.e. a "normal" call, this function just returns OK for this mode.
*
*
*   SET  - Set the call mode, only 0 is supported.
*   READ - The current value, always 0.
*   TEST - Returns the supported modes as a compound value.
*
*===============================================================================
*/
AT_Command_e AT_PLUS_CMOD_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message)
{
    AT_Command_e result = AT_ERROR;

    switch (parser_p->Mode) {

    case AT_MODE_SET: {

        bool error_flag = false;

        IntParam_t mode = CALL_MODE_SINGLE;

        mode = Parser_GetIntParam(parser_p, &error_flag, &mode);

        if (!parser_p->NoMoreParams || error_flag) {
            Parser_SetCMEE_ErrorType(parser_p, CMEE_OPERATION_NOT_ALLOWED);
            return AT_CME_ERROR;
        }

        if (mode > CALL_MODE_VOICE_FOLLOWED_BY_DATA) {
            Parser_SetCMEE_ErrorType(parser_p, CMEE_OPERATION_NOT_ALLOWED);
            return AT_CME_ERROR;
        }

        if (mode > CALL_MODE_SINGLE) {
            Parser_SetCMEE_ErrorType(parser_p, CMEE_OPERATION_NOT_SUPPORTED);
            return AT_CME_ERROR;
        }

        /* We only accept mode = 0 (single mode) which is default, so just return OK */
        result = AT_OK;
        break;
    }
    case AT_MODE_READ:
        sprintf((char *) info_text, ": 0");
        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        result = AT_OK;
        break;
    case AT_MODE_TEST:
        sprintf((char *) info_text, ": (0)");
        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        result = AT_OK;
        break;
    default:
        result = AT_ERROR;
        break;
    }

    return result;
}


/*
 *===============================================================================
 *
 *   Function: AT_STAR_ELAT_Handle
 *
 *   INPUT:   parser_p   - Pointer to the current parser state.
 *            info_text  - Pointer to a string buffer to put information text.
 *            message    - Pointer to the response message used for pending commands.
 *
 *   OUTPUT:  parser_p->
 *              Pos      - The current position in the command line.
 *
 *   RETURNS: A status code for the execution of the command is returned:
 *            AT_OK        - The command was successfully handled.
 *            AT_ERROR     - There was a general error.
 *            AT_CME_ERROR - Invalid parameters
 *
 *   The AT*ELAT command is used to enable/disable unsolicited reporting of
 *   local tone generation indications.
 *
 *   SET  - Enable/disable unsolicited reporting
 *   READ - Get the current reporting mode
 *   TEST - Returns the supported modes.
 *
 *===============================================================================
 */
AT_Command_e AT_STAR_ELAT_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message)
{
    AT_Command_e result = AT_ERROR;

    switch (parser_p->Mode) {
    case AT_MODE_SET:
    case AT_MODE_TEST:
        result = AT_Flag_Handle(parser_p, info_text, &parser_p->ELAT, 1);
        break;

    case AT_MODE_READ:
        sprintf((char *)info_text, ": %d", parser_p->ELAT);
        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        result = AT_OK;
        break;

    default:
        Parser_SetCMEE_ErrorType(parser_p, CMEE_UNKNOWN);
        result = AT_CME_ERROR;
        break;
    }

    return result;
}

/*
 *===============================================================================
 *
 *   Function: AT_STAR_ELIN_Handle
 *
 *   INPUT:   parser_p  - Pointer to the current parser state.
 *            info_text - Pointer to a string buffer to put information text.
 *            message   - Used when call back from handling PENDING
 *
 *   OUTPUT:  parser_p->
 *              Pos    - The current position in the command line.
 *
 *   RETURNS: A status code for the execution of the command is returned:
 *            AT_OK        - The command was successfully handled.
 *            AT_ERROR     - There was a general error.
 *
 *   SET  - Set phone line to use for speech
 *   READ - Get which line is currently selected
 *   TEST - Get which lines that are supported
 *
 *===============================================================================
 */
AT_Command_e AT_STAR_ELIN_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message)
{

    AT_Command_e result = AT_ERROR;

    switch (parser_p->Mode) {
    case AT_MODE_SET:

        if (!parser_p->IsPending) {
            bool error_flag = false;

            IntParam_t line = EXE_LINE_NORMAL;

            exe_t *exe_p;
            exe_request_result_t exe_res = EXE_FAILURE;
            exe_line_request_response_data_t exe_req_data;

            line = Parser_GetIntParam(parser_p, &error_flag, &line);
            if (error_flag || !parser_p->NoMoreParams
                    || !(line == EXE_LINE_NORMAL || line == EXE_LINE_TWO)) {
                Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
                result = AT_CME_ERROR;
            } else {
                exe_p = atc_get_exe_handle();

                if (exe_p) {
                    exe_req_data.line = line;
                    exe_res = exe_request(exe_p, EXE_ELIN_SET, &exe_req_data,
                                          (void *)parser_p, &parser_p->request_handle);
                    result = parser_handle_exe_result(exe_res, parser_p);
                }
            }
        } else {
            if (EXE_SUCCESS == message->exe_result_code) {
                result = AT_OK;
            } else {
                Parser_SetCMEE_ErrorType(parser_p,
                                         CMEE_OPERATION_NOT_ALLOWED);
                result = AT_CME_ERROR;
            }
        }

        break;
    case AT_MODE_READ:
        /* INFO: AT_MODE_READ "AT*ELIN?" where answer is *ELIN: <lineid> */
        /* e.g. *ELIN: 1 */

        if (!parser_p->IsPending) {
            exe_line_request_response_data_t elin_read_response;
            exe_t *exe_p;
            exe_request_result_t exe_res = EXE_FAILURE;
            exe_p = atc_get_exe_handle();
            exe_res = exe_request(exe_p, EXE_ELIN_READ, (void *) &elin_read_response,
                                  (void *)parser_p, &parser_p->request_handle);
            result = parser_handle_exe_result(exe_res, parser_p);
        } else {
            if (NULL != message) {
                result = parser_handle_exe_result(message->exe_result_code, parser_p);

                if ((AT_OK == result) && (NULL != message->data_p)) {
                    sprintf((char *) info_text, ": %d",
                            ((exe_line_request_response_data_t *)message->data_p)->current_line);

                    Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
                    result = AT_OK;
                }
            } else {
                Parser_SetCMEE_ErrorType(parser_p, CMEE_NOT_FOUND);
                result = AT_CME_ERROR;
            }
        }

        break;

    case AT_MODE_TEST:
        /* INFO: AT_MODE_TEST "AT*ELIN=?" where answer is *ELIN: (list of supported <lineid>) */
        /* e.g. *ELIN: (1,2) */

        if (!parser_p->IsPending) {
            exe_line_request_response_data_t elin_read_response;
            exe_t *exe_p;
            exe_request_result_t exe_res = EXE_FAILURE;
            exe_p = atc_get_exe_handle();
            exe_res = exe_request(exe_p, EXE_ELIN_READ, (void *) &elin_read_response,
                                  (void *)parser_p, &parser_p->request_handle);
            result = parser_handle_exe_result(exe_res, parser_p);
        } else {
            if (NULL != message) {
                result = parser_handle_exe_result(message->exe_result_code, parser_p);

                if ((AT_OK == result) && (NULL != message->data_p)) {
                    if (((exe_line_request_response_data_t *)message->data_p)->supported_lines == EXE_LINE_SUPPORT) {
                        sprintf((char *) info_text, ": (1,2)");
                    } else {
                        sprintf((char *) info_text, ": (1)");
                    }

                    Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
                }
            } else {
                Parser_SetCMEE_ErrorType(parser_p, CMEE_NOT_FOUND);
                result = AT_CME_ERROR;
            }
        }

        break;

    default:
        result = AT_ERROR;
    }

    return result;
} /* End of AT_STAR_ELIN_Handle */

/*
 * Handle incoming ELAT event
 */
#define ELAT_TEXT_MAX_SIZE 9
void handle_incoming_elat_event(exe_elat_response_t *response_p)
{
    AT_ParserState_s *parser_p = NULL;
    unsigned char entry = 0;
    unsigned char text[] = {0, 0, 0, 0, 0, 0, 0, 0, 0}; /* size: ELAT_TEXT_MAX_SIZE */
    AT_CommandLine_t infotext = (AT_CommandLine_t)text;

    if (!response_p) {
        ATC_LOG_E("%s: response_p is NULL!", __FUNCTION__);
        goto exit;
    }

    if (*response_p < 0 || *response_p > 1) {
        ATC_LOG_E("%s: *response_p has an invalid value! (%d)", __FUNCTION__, *response_p);
        goto exit;
    }

    (void)snprintf((char *)infotext, ELAT_TEXT_MAX_SIZE, "*ELAT: %d", *response_p);

    for (entry = 0; entry < MAX_NO_OF_OPEN_CHANNELS; entry++) {
        parser_p = ParserStateTable_GetParserState_ByEntry(entry);

        if (!parser_p) {
            ATC_LOG_E("%s: parser state entry %d is NULL!", __FUNCTION__, entry);
            continue;
        }

        if (parser_p->ELAT) {
            parser_send_unsolicited(parser_p, AT_TEXT_RESPONSE, infotext);
        }
    }

exit:
    return;
}

/*
 * Handle incoming CLIP event
 */
void handle_incoming_CLIP_event(exe_clip_response_t *response)
{
    AT_ParserState_s *parser_p;
    unsigned char entry;
    AT_CommandLine_t infotext = (unsigned char *)malloc(SIZE_OF_CLIP_RESPONSE);

    if (!infotext) {
        return;
    }

    *infotext = END_OF_STRING;
    strcat((char *)infotext, "+CLIP: \"");

    if ((response->type == 145)  /* 145 == INTERNATIONAL */
            && (*response->number_p != '+')) {
        strcat((char *)infotext, "+");
    }

    strcat((char *)infotext, response->number_p);
    AT_AddValue(infotext, "\",000", response->type);

    for (entry = 0; entry < MAX_NO_OF_OPEN_CHANNELS; entry++) {
        parser_p = ParserStateTable_GetParserState_ByEntry(entry);

        if (parser_p->CLIP) {
            parser_send_unsolicited(parser_p, AT_TEXT_RESPONSE, infotext);
        }
    }

    free((void *)infotext);
}


/*
 * Handle incoming ECAV event
 */
void handle_incoming_ECAV_event(exe_ecav_response_t *response)
{
    AT_ParserState_s *parser_p;
    unsigned char entry;
    unsigned char *ECAV_text_p = (unsigned char *)calloc(1, SIZE_OF_ECAV_RESPONSE);

    ECAV_build_report((AT_CommandLine_t)ECAV_text_p,
                      response->ccid,
                      response->call_status,
                      response->call_type,
                      response->process_id,
                      response->exit_cause,
                      (unsigned char *)response->subscriber_number_p,
                      response->subscriber_number_addr_type);

    for (entry = 0; entry < MAX_NO_OF_OPEN_CHANNELS; entry++) {
        parser_p = ParserStateTable_GetParserState_ByEntry(entry);

        if (parser_p->ECAM) {
            parser_send_unsolicited(parser_p, AT_TEXT_RESPONSE, (AT_CommandLine_t)ECAV_text_p);
        }
    }

    free((void *)ECAV_text_p);
}


/*
 *=====================================================================
 *
 *  Function: ECAV_build_report
 *
 *  Input:              info_text -      Empty character buffer.
 *                              ccid
 *                              ECAM_status
 *                              call_type
 *                              process_id
 *                              exit_cause
 *                              subscriber_number_p
 *                              subscriber_number_addr_type
 *
 *  Output:             infotext - Character buffer with ECAV indication text.
 *
 *      The function assembles a call status indication from the values of
 *      the input parameters.
 *
 *======================================================================
 */
void ECAV_build_report(AT_CommandLine_t infotext, unsigned char ccid,
                       exe_call_status_t call_status, exe_call_type_t call_type,
                       unsigned char process_id, unsigned char exit_cause,
                       unsigned char *subscriber_number_p, int subscriber_number_addr_type)
{
    *infotext = END_OF_STRING;

    /* Note: call id for ECAV is from 0 to 6 and thus different for all other handling. */
    sprintf((char *)infotext, "*ECAV: %d,%d,%d", ccid - 1, call_status, call_type);

    if (process_id) {
        AT_AddValue(infotext, ",00", process_id);
    } else {
        /* If process_id doesn't exist then print ',' */
        strcat((char *) infotext, ",");
    }

    if (exit_cause) {
        AT_AddValue(infotext, ",000", exit_cause);
    } else {
        /* If exit_cause doesn't exist then print ',' */
        strcat((char *) infotext, ",");
    }

    if (subscriber_number_p != NULL)  {
        strcat((char *) infotext, ",\"");

        if ((subscriber_number_addr_type == 145)  /* 145 == INTERNATIONAL */
                && (subscriber_number_p[0] != '+' && subscriber_number_p[0] != 0)) {
            strcat((char *) infotext, "+");
        }

        strncat((char *) infotext, (char const *) subscriber_number_p, (size_t)MAX_DIGITS_IN_NUMBER);
        strcat((char *) infotext, "\"");
        AT_AddValue(infotext, ",000", (unsigned char) subscriber_number_addr_type);
    }

    return;
}


/*
 * Handle RING events sent from modem.
 */
void handle_incoming_RING_event(exe_call_type_t *call_type_p)
{
    AT_ParserState_s    *parser_p;
    unsigned char       entry;
    AT_CommandLine_t    text;

    for (entry = 0; entry < MAX_NO_OF_OPEN_CHANNELS; entry++) {
        parser_p = ParserStateTable_GetParserState_ByEntry(entry);

        /* The RING UR code is always available and can not be switched off. */
        if (parser_p->CRC) {
            switch (*call_type_p) {
            case CALL_SPEECH:
            case CALL_ALS:
                text = (AT_CommandLine_t)ATC_CRING_VOICE_TEXT;
                break;

            case CALL_DATA:
            default:
                text = (AT_CommandLine_t)"";
                ATC_LOG_E("handle_incoming_RING_event: unsupported call type ( %d)", *call_type_p);
                break;
            }
        } else {
            text = (AT_CommandLine_t)atc_get_response(RING, parser_p->VerboseResponse);;
        }

        parser_send_unsolicited(parser_p, AT_TEXT_RESPONSE, text);
    }
}

void handle_incoming_call_response_event(exe_call_response_t *call_response_p)
{
    AT_ParserState_s    *parser_p = NULL;
    unsigned char       entry;

    for (entry = 0; entry < MAX_NO_OF_OPEN_CHANNELS; entry++) {
        parser_p = ParserStateTable_GetParserState_ByEntry(entry);

        if (parser_p->connecting_voice_call) {
            if (EXE_CALL_IDLE == call_response_p->call_state) {
                AT_Command_e response = AT_ERROR;
                AT_CommandLine_t response_str_p = NULL;

                switch (call_response_p->exit_response) {
                case EXE_CALL_RESPONSE_NO_DIALTONE:
                    response = AT_NO_DIALTONE;
                    break;
                case EXE_CALL_RESPONSE_BUSY:
                    response = AT_BUSY;
                    break;
                case EXE_CALL_RESPONSE_NO_ANSWER:
                    response = AT_NO_ANSWER;
                    break;
                case EXE_CALL_RESPONSE_NO_CARRIER:
                    response = AT_NO_CARRIER;
                    break;
                default:
                    response = AT_ERROR;
                    break;
                }

                response_str_p = (AT_CommandLine_t)atc_get_response(response, parser_p->VerboseResponse);
                parser_send_unsolicited(parser_p, AT_TEXT_RESPONSE, response_str_p);
                parser_p->connecting_voice_call = false;

                /* If external channel is connected, send unsolicited on that one as well */
                for (entry = 0; entry < MAX_NO_OF_OPEN_CHANNELS; entry++) {
                    parser_p = ParserStateTable_GetParserState_ByEntry(entry);

                    if (ATC_CONNECTION_TYPE_EXTERNAL == parser_p->connection_type) {
                        /* Display verbose or numeric result codes over the external AT channel. */
                        response_str_p = (AT_CommandLine_t)atc_get_response(response, parser_p->VerboseResponse);
                        parser_send_unsolicited(parser_p, AT_TEXT_RESPONSE, response_str_p);
                        break;
                    }
                }
            } else if (EXE_CALL_ACTIVE == call_response_p->call_state) {
                parser_p->connecting_voice_call = false;
            }

            break;  /* TODO Remove this if we allow more than one active call. */
        }
    }
}

void handle_incoming_ATD_event(void)
{
    AT_ParserState_s    *parser_p;
    unsigned char       entry;
    AT_CommandLine_t    ct_infotext_p = NULL;

    /* Loop through all parser states to find subscriptions. */
    for (entry = 0; entry < MAX_NO_OF_OPEN_CHANNELS; entry++) {
        parser_p = ParserStateTable_GetParserState_ByEntry(entry);

        if (ATD_CALL_TYPE_PS_DATA == parser_p->atd_call_type) {
            ct_infotext_p = (AT_CommandLine_t)atc_get_response(AT_NO_CARRIER, parser_p->VerboseResponse);
            parser_send_unsolicited(parser_p, AT_TEXT_RESPONSE, ct_infotext_p);
        }
    }
}

