/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <atc_handlers.h>
#include <atc_config.h>
#include <atc_exe_glue.h>
#include <atc_parser.h>
#include <atc_log.h>
#include <exe.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>


/*
 *===============================================================================
 *
 *   Function: AT_PLUS_CMUT_Handle
 *
 *   INPUT:   parser_p  - Pointer to the current parser state.
 *            info_text - Pointer to a string buffer to put information text.
 *            message   - Not used
 *
 *   OUTPUT:  parser_p->
 *              Pos    - The current position in the command line.
 *
 *   RETURNS: A status code for the execution of the command is returned:
 *            AT_OK        - The command was successfully handled.
 *            AT_ERROR     - There was a general error.
 *            AT_CME_ERROR - There was a GSM specific error, the error code is
 *                           returned in info_text.
 *
 *   The CMUT function is used to enable/disable the uplink voice muting during a
 *   voice call.
 *   Note that these calls are synchronous.
 *
 *   SIR 2.3
 *
 *   SET  - Enable or Disable the uplink voice muting
 *   READ - Show the current settings of the uplink voice muting
 *   TEST - Show list of supported parameter.
 *
 *===============================================================================
 */
AT_Command_e AT_PLUS_CMUT_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text, AT_ResponseMessage_t message)
{
    AT_Command_e result = AT_OK;

    switch (parser_p->Mode) {
    case AT_MODE_SET:

        if (!parser_p->IsPending) {
            exe_cmut_response_t cmut_res;
            bool error_flag = false;
            IntParam_t n = 2;
            exe_t *exe_p;
            exe_request_result_t exe_res = EXE_FAILURE;
            n = Parser_GetIntParam(parser_p, &error_flag, &n);
            exe_p = atc_get_exe_handle();

            if (!error_flag && exe_p && parser_p->NoMoreParams) {
                switch (n) {
                case 0:
                case 1: {
                    cmut_res.value = n;
                    exe_res = exe_request(exe_p, EXE_CMUT_SET, (void *) &cmut_res,
                                          (void *)parser_p, &parser_p->request_handle);

                    switch (exe_res) {
                    case EXE_SUCCESS:
                        result = AT_OK;
                        break;
                    case EXE_FAILURE:
                        result = AT_CME_ERROR;
                        Parser_SetCMEE_ErrorType(parser_p,
                                                 CMEE_OPERATION_NOT_ALLOWED);
                        break;
                    case EXE_NOT_SUPPORTED:
                        result = AT_CME_ERROR;
                        Parser_SetCMEE_ErrorType(parser_p,
                                                 CMEE_OPERATION_NOT_SUPPORTED);
                        break;
                    case EXE_PENDING:
                    default:
                        result = AT_CME_ERROR;
                        Parser_SetCMEE_ErrorType(parser_p,
                                                 CMEE_PHONE_FAILURE);
                        break;
                    }
                }
                break;
                default:
                    result = AT_CME_ERROR;
                    Parser_SetCMEE_ErrorType(parser_p,
                                             CMEE_OPERATION_NOT_ALLOWED);
                }
            } else {
                result = AT_ERROR;
            }
        }

        break;
    case AT_MODE_READ:
        result = AT_OK;

        if (!parser_p->IsPending) {
            exe_t *exe_p;
            exe_cmut_response_t cmut_res;
            exe_request_result_t exe_res = EXE_FAILURE;
            IntParam_t n = 0;
            exe_p = atc_get_exe_handle();
            exe_res = exe_request(exe_p, EXE_CMUT_READ, (void *) &cmut_res,
                                  (void *)parser_p, &parser_p->request_handle);

            switch (exe_res) {
            case EXE_SUCCESS:
                AT_AddValue(info_text, ": 0", cmut_res.value);
                Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
                result = AT_OK;
                break;
            case EXE_FAILURE:
                result = AT_CME_ERROR;
                Parser_SetCMEE_ErrorType(parser_p,
                                         CMEE_OPERATION_NOT_ALLOWED);
                break;
            case EXE_NOT_SUPPORTED:
                result = AT_CME_ERROR;
                Parser_SetCMEE_ErrorType(parser_p,
                                         CMEE_OPERATION_NOT_SUPPORTED);
                break;
            case EXE_PENDING:
            default:
                result = AT_PENDING;
                Parser_SetCMEE_ErrorType(parser_p,
                                         CMEE_PHONE_FAILURE);
                break;
            }
        } else { /* We have a result...*/
            IntParam_t n = 0;
            exe_cmut_response_t *cmut_res =
                (exe_cmut_response_t *) message->data_p;
            n = cmut_res->value;

            if (EXE_SUCCESS == message->exe_result_code) {
                sprintf((char *) info_text, ": %d", n);
                Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
            } else {
                Parser_SetCMEE_ErrorType(parser_p, CMEE_OPERATION_NOT_ALLOWED);
                result = AT_CME_ERROR;
            }
        }

        break;
    case AT_MODE_TEST:
        sprintf((char *) info_text, ": (0,1)");
        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        result = AT_OK;
        break;
    }

    return result;
} /* End of AT_PLUS_CMUT_Handle */


/*
 *===============================================================================
 *
 *   Function: AT_PLUS_VTD_Handle
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
 *            AT_CME_ERROR - There was a GSM specific error, the error code is
 *                           returned in info_text.
 *
 *   The VTD command sets the duration for DTMF tones.
 *
 *   SET  - Set duration of dtmf
 *   READ - Read the duration of dtmf
 *   TEST - Show list of supported parameter.
 *
 *===============================================================================
 */
AT_Command_e AT_PLUS_VTD_Handle(AT_ParserState_s *parser_p,
                                AT_CommandLine_t info_text, AT_ResponseMessage_t message)
{
    AT_Command_e result = AT_ERROR;

    switch (parser_p->Mode) {
    case AT_MODE_SET:

        if (!parser_p->IsPending) {
            if (!parser_p->NoMoreParams) {
                unsigned long inparam = 0;
                bool  error_flag = false;

                inparam = Parser_GetLongIntParam(parser_p, &error_flag, (unsigned long *)NULL, 10000000);

                if (!error_flag) {
                    exe_t               *exe_p;
                    exe_request_result_t exe_res = EXE_FAILURE;

                    exe_p = atc_get_exe_handle();
                    exe_res = exe_request(exe_p, EXE_SET_DTMF_DURATION, (exe_vtd_t *)&inparam,
                                          (void *)parser_p, &parser_p->request_handle);

                    switch (exe_res) {
                    case EXE_PENDING:
                        result = AT_PENDING;
                        break;
                    case EXE_SUCCESS:
                        result = AT_OK;
                        break;
                    case EXE_FAILURE:
                    case EXE_NOT_SUPPORTED:
                    default:
                        break;
                    }
                }
            }
        } else { /* We have a result...*/
            if (EXE_SUCCESS == message->exe_result_code) {
                result = AT_OK;
            }
        }

        break;
    case AT_MODE_READ:
        result = AT_OK;

        if (!parser_p->IsPending) {
            exe_vtd_t n;
            exe_t *exe_p;
            exe_request_result_t exe_res = EXE_FAILURE;
            exe_p = atc_get_exe_handle();
            exe_res = exe_request(exe_p, EXE_GET_DTMF_DURATION, &n,
                                  (void *)parser_p, &parser_p->request_handle);

            switch (exe_res) {
            case EXE_SUCCESS:
                AT_AddValue(info_text, "0", n);
                Parser_SendResponse(parser_p, AT_TEXT_RESPONSE, info_text);
                result = AT_OK;
                break;
            case EXE_PENDING:
            case EXE_FAILURE:
            case EXE_NOT_SUPPORTED:
            default:
                result = AT_ERROR;
                break;
            }
        }

        break;

    case AT_MODE_TEST:
        sprintf((char *) info_text, "(0-10000000)");
        Parser_SendResponse(parser_p, AT_TEXT_RESPONSE, info_text);
        result = AT_OK;
        break;
    }

    return result;
} /* End of AT_PLUS_VTD_Handle */

bool count_dtmf_characters(const char *const ParserCommandLine, uint8_t *NumberOfCharacters_p)
{
    const char *ch_p;
    *NumberOfCharacters_p = 0;

    for (ch_p = ParserCommandLine; *ch_p != '\0'; ch_p++) {
        switch (*ch_p) {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'p':
        case 'w':
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case '#':
        case '*':
            (*NumberOfCharacters_p)++;
            break;

        case '\t':
        case ' ':
        case ',':
        case '"':
            break;

        default:
            return false;
        }
    }

    return true;
}

/*
 * This function assumes that you already have called count_dtmf_characters
 * and pass a buffer big enough to store the result.
 */
static void extract_dtmf_characters(AT_ParserState_s *const parser_p, exe_vts_t *dtmf_characters_p)
{
    const unsigned char *ch_p;
    const unsigned char *parser_pos_p = parser_p->Pos;

    for (ch_p = parser_pos_p; *ch_p != '\0'; ch_p++) {

        switch (*ch_p) {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case '#':
        case '*':
            *dtmf_characters_p++ = *ch_p;
            break;

        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'p':
        case 'w': {
            /* conversion to upper cases */
            *dtmf_characters_p++ = (*ch_p) - 0x20;
            break;
        }

        case '\t':
        case ' ':
        case ',':
        case '"':
            break;

        default:
            /* any errors should have been caught by count_dtmf_characters */
            return;
        }
    }

    *dtmf_characters_p = '\0'; /* end of DTMF string */
    parser_p->Pos = (AT_CommandLine_t)ch_p;
    parser_p->NoMoreParams = true;

}

/*
 *===============================================================================
 *
 *   Function: AT_PLUS_VTS_Handle
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
 *            AT_CME_ERROR - There was a GSM specific error, the error code is
 *                           returned in info_text.
 *
 *   The VTS function is used for sending one DTMF character to the remote
 *
 *   SET  - Send a DTMF
 *   READ - Not applicable
 *   TEST - Show list of supported parameter.
 *
 *===============================================================================
 */
AT_Command_e AT_PLUS_VTS_Handle(AT_ParserState_s *parser_p,
                                AT_CommandLine_t info_text, AT_ResponseMessage_t message)
{
    AT_Command_e result = AT_ERROR;

    switch (parser_p->Mode) {
    case AT_MODE_SET:

        if (!parser_p->IsPending) {
            if (parser_p->NoMoreParams) {
                Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
                return AT_ERROR;
            }

            /* For proper treatment according to spec, quoted and unquoted strings
             * must be supported. Further the DTMF list may be comma separated or
             * without separators */
            if (NULL != parser_p->Pos) {
                exe_vts_t *dtmf_p;
                exe_dtmf_data_t dtmf_data;
                uint8_t  number_of_dtmf_chars;

                if (count_dtmf_characters((char *)parser_p->Pos, &number_of_dtmf_chars)) {
                    if (number_of_dtmf_chars <= 0) {
                        Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
                        return AT_ERROR;
                    }

                    dtmf_p = (exe_vts_t *)malloc(number_of_dtmf_chars + 1);

                    if (NULL == dtmf_p) {
                        ATC_LOG_E("AT+VTS: No resources to allocate");
                        return AT_ERROR;
                    }

                    extract_dtmf_characters(parser_p, dtmf_p);

                    dtmf_data.dtmf_p = dtmf_p;
                    dtmf_data.length = number_of_dtmf_chars;

                    exe_t               *exe_p;
                    exe_request_result_t exe_res = EXE_FAILURE;

                    exe_p = atc_get_exe_handle();
                    exe_res = exe_request(exe_p, EXE_VTS, (void *)&dtmf_data,
                                          (void *)parser_p, &parser_p->request_handle);

                    free((void *)dtmf_p);

                    switch (exe_res) {
                    case EXE_PENDING:
                        result = AT_PENDING;
                        break;
                    case EXE_SUCCESS:
                        result = AT_OK;
                        break;
                    case EXE_FAILURE:
                    case EXE_NOT_SUPPORTED:
                        break;
                    default:
                        break;
                    }

                } else {
                    ATC_LOG_E("AT+VTS: Invalid dtmf string");
                    result = AT_ERROR;
                }
            }

        } else { /* We have a result...*/
            if (EXE_SUCCESS == message->exe_result_code) {
                result = AT_OK;
            }
        }

        break;
    case AT_MODE_TEST:
        result = AT_OK;
        break;
    }

    return result;
} /* End of AT_PLUS_VTS_Handle */

/*
*===============================================================================
*
*   Function: AT_STAR_ETTY_Handle
*
*   INPUT:   Parser   - Pointer to the current parser state.
*            InfoText - Pointer to a string buffer to put information text.
*            Message  - Not used
*
*   OUTPUT:  Parser->
*              Pos    - The current position in the command line.
*
*   RETURNS: A status code for the execution of the command is returned:
*            AT_OK        - The command was successfully handled.
*            AT_ERROR     - There was a general error.
*            AT_CME_ERROR - There was a GSM specific error, the error code is
*                           returned in InfoText.
*
*   The ETTY function is used to set tty mode and change  APF file
*
*   SIR 2.3
*
*   SET  - set apf file and change tty mode
*   READ - Show the current tty mode
*   TEST - Show list of supported parameters.
*
*===============================================================================
*/
AT_Command_e AT_STAR_ETTY_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message)
{
    switch (parser_p->Mode) {
        exe_request_result_t result = EXE_FAILURE;
        exe_t *exe_p = NULL;

    case AT_MODE_SET: {
        if (!parser_p->IsPending) {
            exe_etty_t req_data;
            bool error_flag = false;
            IntParam_t n = 0;

            n = Parser_GetIntParam(parser_p, &error_flag, &n);

            if (error_flag) {
                return AT_ERROR;
            }

            /* Only n supported at the moment. */
            if (!parser_p->NoMoreParams) {
                Parser_SetCMEE_ErrorType(parser_p,
                                         CMEE_OPERATION_NOT_ALLOWED);
                return AT_CME_ERROR;
            }

            if (n > 3) {
                Parser_SetCMEE_ErrorType(parser_p,
                                         CMEE_OPERATION_NOT_ALLOWED);
                return AT_CME_ERROR;
            }

            exe_p = atc_get_exe_handle();

            if (NULL == exe_p) {
                ATC_LOG_E("AT_STAR_ETTY_Handle, atc_get_exe_handle returned NULL");
                return AT_ERROR;
            }

            req_data.n = n;
            result = exe_request(exe_p, EXE_ETTY_SET, &req_data,
                                 (void *)parser_p, &parser_p->request_handle);

            if (EXE_PENDING != result) {
                ATC_LOG_E("AT_STAR_ETTY_Handle, executer did not return pending");
                return AT_ERROR;
            }

            return AT_PENDING;
            break;

        } else {

            if (EXE_SUCCESS != message->exe_result_code) {
                ATC_LOG_E("AT_STAR_ETTY_Handle, executer did not return SUCCESS");
                return AT_ERROR;
            }

            break;
        }
    }

    case AT_MODE_READ: {
        exe_etty_t res_data;
        exe_p = atc_get_exe_handle();

        res_data.n = -1;

        if (NULL == exe_p) {
            ATC_LOG_E("AT_STAR_ETTY_Handle, atc_get_exe_handle returned NULL");
            return AT_ERROR;
        }

        result = exe_request(exe_p, EXE_ETTY_GET, &res_data,
                             (void *)parser_p, &parser_p->request_handle);

        if (EXE_SUCCESS != result) {
            return AT_ERROR;
        }

        sprintf((char *)info_text, ": %d", res_data.n);
        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        break;
    }

    case AT_MODE_TEST: {
        sprintf((char *) info_text, ": (0-3)");
        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        result = AT_OK;
        break;
    }
    default: {
        Parser_SetCMEE_ErrorType(parser_p, CMEE_OPERATION_NOT_ALLOWED);
        result = AT_CME_ERROR;
        break;
    }
    }

    return AT_OK;
} /* End of AT_STAR_ETTY_Handle */
