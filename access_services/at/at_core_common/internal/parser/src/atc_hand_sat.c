/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <c_type.h>

#include <atc_config.h>
#include <atc_exe_glue.h>
#include <atc_handlers.h>
#include <atc_parser.h>
#include <atc_parser_util.h>
#include <atc_log.h>
#include <atc.h>

#include <exe.h>

/********************************************************************
 * Private prototypes
 ********************************************************************
 */
static void broadcast_sat_session_completed();

/*
 *====================================================================================
 *
 *   Function: AT_STAR_STKC_Handle
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
 *                           returned in info_text.
 *            AT_PENDING   - The command is not ready but returns while
 *                           waiting for an asynchronous response.
 *
 *   The STKC command enables/disables the presentation of unsolicited notifications
 *   result codes from TA to the TE. It also sets the Terminal Profile in MODEM/USIM.
 *
 *   SET  - Enables/disables the presentation of unsolicited notifications
 *          & sends down a terminal profile to the SIM:
 *          *STKC= <n>[,<TermProfile >]
 *            <n> and  <TermProfile > are numeric string type values.
 *            The range for <n> is {0,1,2}.
 *            <TermProfile> is a binary hexstring, without quotes of length 19.
 *            Support for Terminal Profile is not implemented at the moment.
 *
 *   READ - Returns the current <n>:
 *          *STKC: (n)
 *            0: Unsolicited notifications result codes are disabled.
 *            1: Unsolicited notifications result codes are enabled.
 *
 *   TEST - Writes the supported types of <n>s to info_text:
 *          *STKC: (list of supported <n>s)
 *
 *====================================================================================
 */

AT_Command_e AT_STAR_STKC_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message)
{
    bool error_flag = false; /* Flags a parameter parsing error. */
    AT_Command_e result = AT_OK;

    switch (parser_p->Mode) {
    case AT_MODE_SET: {
        if (!parser_p->IsPending) {
            exe_stkc_pc_request_data_t n = {EXE_STKC_PC_DISABLE};
            exe_t *exe_p = NULL;
            exe_request_result_t exe_request_result = EXE_SUCCESS;
            bool call_executor = true;

            /* Extract the <n> parameter. */
            n.mode = (exe_stkc_pc_request_mode_t)Parser_GetIntParam(parser_p, &error_flag, NULL);

            if (error_flag) {
                result = AT_CME_ERROR;
                Parser_SetCMEE_ErrorType(parser_p,
                                         CMEE_INCORRECT_PARAMETERS);
                goto end;
            }

            if (!parser_p->NoMoreParams) {
                char *term_profile_p = NULL;
                term_profile_p = Parser_GetStrParam(parser_p, &error_flag, NULL);

                if ((NULL == term_profile_p) || error_flag) {
                    result = AT_ERROR;
                    goto end;
                }

                if (!parser_p->NoMoreParams) {
                    result = AT_CME_ERROR;
                    Parser_SetCMEE_ErrorType(parser_p,
                                             CMEE_OPERATION_NOT_SUPPORTED);
                    goto end;
                }
            }

            switch (n.mode) {
            case EXE_STKC_PC_DISABLE: /* Check if the <n> parameter zero (0). */
                /* Only one channel at the time is allowed to enable pro-active commands, so
                 * if we are in some other state than disabled we know that we control it
                 * and can safely disable as no other channel can be listening.
                 */
                if (EXE_STKC_PC_DISABLE == parser_p->stkc_pc_enable) {
                    call_executor = false;
                }

                break;
            case EXE_STKC_PC_ENABLE: {
                /* If we are already in ENABLED we know that we are the
                 * controlling channel so no need to check the others, all we need to do is
                 * to see if we should change to or from WITH_ICON.
                 */
                if (EXE_STKC_PC_DISABLE != parser_p->stkc_pc_enable) {
                    if (n.mode == parser_p->stkc_pc_enable) {
                        call_executor = false;
                    }
                } else {
                    uint8_t i = 0;
                    AT_ParserState_s *tmp_parser_p = NULL;

                    /* Loop through all parser states to find subscriptions. */
                    for (i = 0; i < MAX_NO_OF_OPEN_CHANNELS; i++) {
                        tmp_parser_p = ParserStateTable_GetParserState_ByEntry(i);

                        if ((NULL != tmp_parser_p) &&
                                (tmp_parser_p != parser_p) &&
                                (EXE_STKC_PC_ENABLE == tmp_parser_p->stkc_pc_enable)) {
                            /* If there is one parser with the state variable stkc_pc_enable
                             * set to ENABLED we cannot enable it on this channel.
                             */
                            call_executor = false;
                            result = AT_CME_ERROR;
                            Parser_SetCMEE_ErrorType(parser_p,
                                                     CMEE_OPERATION_NOT_ALLOWED);
                            break;
                        }
                    }
                }

                break;
            }
            default:
                result = AT_CME_ERROR;
                Parser_SetCMEE_ErrorType(parser_p,
                                         CMEE_INCORRECT_PARAMETERS);
                goto end;
                break;
            }

            if (call_executor) {
                exe_p = atc_get_exe_handle();

                if (NULL == exe_p) {
                    result = AT_ERROR;
                    ATC_LOG_E("AT_STAR_STKC_Handle: exe_p is NULL");
                    goto end;
                }

                exe_request_result = exe_request(exe_p,   /* Executor handle. */
                                                 EXE_STKC_PC_CONTROL, /* Executor request ID. */
                                                 &n, /* Parameter to STKC form TE. */
                                                 (void *) parser_p,  /* Parser status data. */
                                                 &parser_p->request_handle);

                /* Sort out the answers from the executer. */
                result = parser_handle_exe_result(exe_request_result, parser_p);
            }
        } else {
            if (NULL == message) {
                result = AT_ERROR;
                ATC_LOG_E("AT_STAR_STKC_Handle: message is NULL");
                goto end;
            }

            if (EXE_SUCCESS != message->exe_result_code) {
                result = AT_CME_ERROR;
                Parser_SetCMEE_ErrorType(parser_p,
                                         CMEE_SIM_FAILURE);
                goto end;
            }

            if (NULL != message->data_p) {
                parser_p->stkc_pc_enable = ((exe_stkc_pc_request_data_t *) message->data_p)->mode;
            }
        }
    } /* End of AT_MODE_SET: */
    break;
    case AT_MODE_READ: { /* AT*STKC? */

        if (!parser_p->IsPending) {
            exe_t *exe_p = NULL;
            exe_request_result_t exe_request_result = EXE_SUCCESS;

            exe_p = atc_get_exe_handle();

            if (NULL != exe_p) {
                exe_request_result = exe_request(
                                         exe_p,   /* Executor handle. */
                                         EXE_GET_TERMPROFILE, /* Executor request ID. */
                                         NULL, /* Parameter to STKC form TE. */
                                         (void *) parser_p,  /* Parser status data. */
                                         &parser_p->request_handle);
            } else {
                result = AT_ERROR;
                goto end;
            }

            /* Sort out the answers from the executer. */
            result = parser_handle_exe_result(exe_request_result, parser_p);

            if ((AT_ERROR == result) || (AT_CME_ERROR == result)) {
                result = AT_ERROR;
                goto end;
            }

        } else {
            if (NULL == message) {
                result = AT_ERROR;
                goto end;
            }

            if (EXE_SUCCESS != message->exe_result_code) {
                result = AT_ERROR;
                goto end;
            }

            exe_stkc_get_terminal_profile_response_t *stkc_get_tp_resp_p = (exe_stkc_get_terminal_profile_response_t *)(message->data_p);

            if (NULL == stkc_get_tp_resp_p) {
                result = AT_ERROR;
                goto end;
            } else if (0 != stkc_get_tp_resp_p->status) {
                result = AT_ERROR;
                goto end;
            } else if (NULL == stkc_get_tp_resp_p->profile_p) {
                result = AT_ERROR;
                goto end;
            } else {
                uint8_t i;
                char tmp_buffer[20];
                bool first_char_set = true;

                for (i = 0; i < stkc_get_tp_resp_p->profile_len; i++) {
                    if (first_char_set) {
                        sprintf((char *)tmp_buffer, ": %d,%X", parser_p->stkc_pc_enable, *(stkc_get_tp_resp_p->profile_p + i));
                        first_char_set = false;
                    } else {
                        sprintf((char *)tmp_buffer, "%X", *(stkc_get_tp_resp_p->profile_p + i));
                    }

                    if ((strlen((char *)info_text) + strlen(tmp_buffer) + 1) < RESULT_TEXT_SIZE) {
                        strcat((char *)info_text, tmp_buffer);
                    } else {
                        break;
                    }
                }

                Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
            }
        }
    } /* End of AT_MODE_READ: */
    break;
    case AT_MODE_TEST: { /* AT*STKC=? */
        sprintf((char *) info_text, ": (0,1)");

        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);

        result = AT_OK;
    } /* End of AT_MODE_TEST: */
    break;
    default: {
        result = AT_CME_ERROR;
        Parser_SetCMEE_ErrorType(parser_p,
                                 CMEE_UNKNOWN);
    }
    break;
    }

end:
    return result;
} /* End of AT_STAR_STKC_Handle */

/*
 *===============================================================================
 *
 *   Function: AT_STAR_STKE_Handle
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
 *   The STKE command requests to send an envelope command to SIM
 *
 *   SET   - End an envelope command to SIM
 *   TEST  - Query if command is supported
 *
 *===============================================================================
 */
AT_Command_e AT_STAR_STKE_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message)
{
    exe_request_result_t exe_result = EXE_FAILURE;
    AT_Command_e result = AT_ERROR;
    bool error_flag = false;

    if (!parser_p->IsPending) {
        switch (parser_p->Mode) {
        case AT_MODE_SET: {
            exe_t *exe_p;
            exe_request_result_t exe_res = EXE_FAILURE;
            exe_apdu_data_t request_data;
            unsigned length = 0;
            bool error_flag = false;
            uint8_t *binary_data_p = NULL;
            StrParam_t hex_data_p = NULL;

            hex_data_p = Parser_GetStrParam(parser_p, &error_flag, NULL);

            if (!parser_p->NoMoreParams || error_flag || ((strlen(hex_data_p) % 2))) {
                Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
                result = AT_CME_ERROR;
                goto error;
            }

            length = (strlen(hex_data_p) / 2);
            binary_data_p = alloca(length);

            if (NULL == binary_data_p) {
                ATC_LOG_E("AT_STAR_STKE_Handle: Memory allocation failed");
                goto error;
            }

            if (false == convert_ascii_hex_to_bin_buf((char *)hex_data_p, binary_data_p, length)) {
                Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
                result = AT_CME_ERROR;
                goto error;
            }

            request_data.len = length;
            request_data.buf_p = (char *) binary_data_p;
            exe_p = atc_get_exe_handle();

            if (exe_p) {
                exe_res = exe_request(atc_get_exe_handle(),
                                      EXE_STKE,
                                      (void *) &request_data,
                                      (void *) parser_p,
                                      &parser_p->request_handle);

                result = parser_handle_exe_result(exe_res, parser_p);
            }

            break;
        }
        case AT_MODE_TEST:
            result = AT_OK;
            break;
        default:
            goto error;
            break;
        }
    } else {
        switch (parser_p->Mode) {
        case AT_MODE_SET: {
            exe_apdu_data_t *stke_res_p = (exe_apdu_data_t *) message->data_p;

            if (EXE_SUCCESS == message->exe_result_code) {
                unsigned i = 0;

                if (NULL == stke_res_p) {
                    ATC_LOG_E("AT_STAR_STKE_Handle response: stke_res_p == NULL");
                    goto error;
                }

                if (0 != stke_res_p->len) {
                    sprintf((char *)info_text, ": ");
                    info_text = info_text + 2;

                    for (i = 0; i < stke_res_p->len; i++) {
                        sprintf((char *)&(info_text[i * 2]), "%02X",
                                stke_res_p->buf_p[i]);
                    }

                    info_text = info_text - 2;
                }

                Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
                result = AT_OK;
            } else {
                Parser_SetCMEE_ErrorType(parser_p, CMEE_OPERATION_NOT_ALLOWED);
                result = AT_CME_ERROR;
            }

            break;
        }
        default:
            goto error;
            break;
        }
    }

    return result;
error:
    return AT_ERROR;

} /* End of AT_STAR_STKE_Handle */

/*
 *===============================================================================
 *
 *   Function: AT_STAR_STKR_Handle
 *
 *   INPUT:   parser_p  - Pointer to the current parser state.
 *            info_text - Pointer to a string buffer to put information text.
 *            Message   - Pointer to the response message used for pending commands.
 *
 *   OUTPUT:  parser_p->
 *            Pos       - The current position in the command line.
 *
 *   RETURNS: A status code for the execution of the command is returned:
 *            AT_OK              - The command was successfully handled.
 *            AT_ERROR           - There where a general error.
 *            AT_CME_ERROR       - There where a GSM specific error, the error code is
 *                                 returned in the parser state object.
 *            AT_PENDING         - The command is not ready but returns while
 *                                 waiting for an asynchronous response.
 *            AT_UNKNOWN_COMMAND - Unknown AT command.
 *
 *   The STKR command is used for sending SIM application toolkit command response
 *   to the SIM. It must be sent as a reply to a Proactive Command. If unexpected
 *   by the MS or if the SIM is not able to handle the Terminal Response, an
 *   error will be returned.
 *
 *   SET  - If a response string is present, send the response string to the SIM:
 *          *STKR=<TermRsp>
 *            <TermRsp> is a hexadecimal string.
 *
 *   TEST - Writes if STKR is supported or not to info_text:
 *          *STKR=?
 *
 *===============================================================================
 */

AT_Command_e AT_STAR_STKR_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message)
{
    AT_Command_e result = AT_OK;             /* Handler request result. */

    switch (parser_p->Mode) {
    case AT_MODE_SET:

        if (!parser_p->IsPending) {
            bool error_flag = false;               /* Parse error. */
            exe_t *exe_p = NULL;                   /* Pointer to executor handler. */
            exe_apdu_data_t request_data;          /* Response message to the SIM on a proactive command. */
            exe_request_result_t exe_request_result = EXE_SUCCESS; /* Executor request result. */
            unsigned length = 0;
            uint8_t *binary_data_p = NULL;
            StrParam_t hex_data_p = NULL;

            hex_data_p = Parser_GetStrParam(parser_p, &error_flag, NULL);

            if (!parser_p->NoMoreParams || error_flag || ((strlen(hex_data_p) % 2))) {
                Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
                result = AT_CME_ERROR;
                goto end;
            }

            length = (strlen(hex_data_p) / 2);
            binary_data_p = alloca(length);

            if (false == convert_ascii_hex_to_bin_buf((char *) hex_data_p, binary_data_p, length)) {
                Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
                result = AT_CME_ERROR;
                goto end;
            }

            request_data.len = length;
            request_data.buf_p = (char *) binary_data_p;
            exe_p = atc_get_exe_handle();

            if (NULL == exe_p) {
                result = AT_ERROR;
                goto end;
            }

            exe_request_result = exe_request(exe_p, /* Executor handle. */
                                             EXE_PROACTIVE_COMMAND_RESPONSE, /* STKR executor request ID. */
                                             (void *) &request_data, /* Parameter to STKR from TE. */
                                             (void *) parser_p, /* Parser status data. */
                                             &parser_p->request_handle);

            /* Sort out the answer from the executor and set possible CMEE error. */
            result = parser_handle_exe_result(exe_request_result, parser_p);

            goto end;
        } else { /* parser_p->IsPending = true. */
            /* We got a response to our terminal response sent to the SIM. */
            if (NULL == message) {
                result = AT_ERROR;
                goto end;
            }

            /* Check that we got a valid response. */
            if (EXE_SUCCESS != message->exe_result_code) {
                result = AT_ERROR;
                goto end;
            }
        }

        break; /* End of AT_MODE_SET: result = AT_OK. */
    case AT_MODE_TEST:
        break; /* End of AT_MODE_TEST: result = AT_OK. */
    default:
        result = AT_ERROR;
        break;
    } /* End switch. */

end:

    return result;
} /* End of AT_STAR_STKR_Handle. */

/*
 *===============================================================================
 *
 *   Function: AT_PLUS_ESHLVOCR_Handle
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
 *   The ESHLVOCR command is used to respond to a +STKI setup call (*ESHLVOCU) unsolicited event
 *   with either accept or reject
 *
 *   SET   - Tell sim to accept or reject the call
 *   TEST - Confirms that the command is implemented
 *
 *===============================================================================
 */
AT_Command_e AT_STAR_ESHLVOCR_Handle(AT_ParserState_s *parser_p,
                                     AT_CommandLine_t info_text,
                                     AT_ResponseMessage_t message)
{
    exe_cmee_error_t cmee_result = CMEE_OK;
    exe_request_result_t result = EXE_FAILURE;
    exe_t *exe_p = NULL;
    exe_setup_call_answer_data_t answer_data = {2}; /* out of range initial value */
    bool error_flag = false;
    exe_p = atc_get_exe_handle();

    switch (parser_p->Mode) {
    case AT_MODE_SET: {
        if (!parser_p->IsPending) {
            answer_data.answer = (int)Parser_GetIntParam(parser_p, &error_flag, NULL);

            if (error_flag || !parser_p->NoMoreParams || answer_data.answer > 1) {
                cmee_result = CMEE_INCORRECT_PARAMETERS;
                goto error;
            }

            result = exe_request(exe_p, EXE_ESHLVOCR, &answer_data, (void *)parser_p, &parser_p->request_handle);

            if (EXE_PENDING == result) {
                return AT_PENDING;
            } else {
                cmee_result = CMEE_SIM_FAILURE;
                goto error;
            }
        } else {
            if (EXE_SUCCESS == message->exe_result_code) {
                return AT_OK;
            } else {
                cmee_result = CMEE_SIM_FAILURE;
                goto error;
            }
        }

        break;
    }
    case AT_MODE_TEST:
        sprintf((char *) info_text, ": (0,1)");
        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        break;
    default:
        cmee_result = CMEE_OPERATION_NOT_ALLOWED;
        break;
    }

    if (cmee_result == CMEE_OK) {
        return AT_OK;
    }

error:
    Parser_SetCMEE_ErrorType(parser_p, cmee_result);
    return AT_CME_ERROR;
} /* End of AT_PLUS_ESHLVOCR_Handle */

#define ATC_STKEND_COMMAND "*STKEND"
void handle_incoming_stkend_event()
{
    AT_ParserState_s *parser_p;
    unsigned char entry;
    int i = 0;

    /* Loop through all parser states to find subscriptions. */
    for (entry = 0; entry < MAX_NO_OF_OPEN_CHANNELS; entry++) {
        parser_p = ParserStateTable_GetParserState_ByEntry(entry);

        /* Only the channel that enabled pro-active commands are to receive them. */
        if (EXE_STKC_PC_DISABLE != parser_p->stkc_pc_enable) {
            parser_send_unsolicited(parser_p, AT_TEXT_RESPONSE, (AT_CommandLine_t)ATC_STKEND_COMMAND);
        }
    }

#ifdef EXE_USE_ATC_CUSTOMER_EXTENSIONS
    atc_extended_broadcast_stk_session_completed();
#endif
}

static void pack_and_broadcast_apdu_data(AT_Command_e command, exe_apdu_data_t *response_p)
{
    AT_ParserState_s *parser_p;
    unsigned char entry;
    char *info_text_p = NULL;
    int length = 0;
    int command_length = 7;
    int i = 0;

    if (NULL == response_p) {
        return;
    }

    /*  2 * is because each char in buff contains 2 hex numbers 1
     * is for the null character appended at the end */
    length = command_length + (2 * response_p->len) + 1;

    if (NULL == (info_text_p = malloc(length))) {
        goto end;
    }

    switch (command) {
    case AT_STAR_STKI:
        sprintf(info_text_p, "*STKI: ");
        break;
    case AT_STAR_STKN:
        sprintf(info_text_p, "*STKN: ");
        break;
    default:
        goto end;
    }

    info_text_p += command_length;

    for (i = 0; i < (int)(response_p->len); i++) {
        sprintf((char *) &(info_text_p[i * 2]), "%02X",
                response_p->buf_p[i]);
    }

    info_text_p -= command_length;

    /* Loop through all parser states to find subscriptions. */
    for (entry = 0; entry < MAX_NO_OF_OPEN_CHANNELS; entry++) {
        parser_p = ParserStateTable_GetParserState_ByEntry(entry);

        /* Only the channel that enabled pro-active commands are to receive them. */
        if (EXE_STKC_PC_DISABLE != parser_p->stkc_pc_enable) {
            parser_send_unsolicited(parser_p, AT_TEXT_RESPONSE, (AT_CommandLine_t)info_text_p);
        }
    }

end:
    free(info_text_p);
}

void handle_incoming_stkn_event(exe_apdu_data_t *response_p)
{
    pack_and_broadcast_apdu_data(AT_STAR_STKN, response_p);
#ifdef EXE_USE_ATC_CUSTOMER_EXTENSIONS
    atc_extended_pack_and_broadcast_apdu_data(AT_STAR_STKN, response_p);
#endif
}

void handle_incoming_stki_event(exe_apdu_data_t *response_p)
{
    pack_and_broadcast_apdu_data(AT_STAR_STKI, response_p);
#ifdef EXE_USE_ATC_CUSTOMER_EXTENSIONS
    atc_extended_pack_and_broadcast_apdu_data(AT_STAR_STKI, response_p);
#endif
}
