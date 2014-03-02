/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <atc_config.h>
#include <atc_context.h>
#include <atc_exe_glue.h>
#include <atc.h>
#include <atc_handlers.h>
#include <atc_log.h>
#include <atc_parser.h>
#include <atc_parser_util.h>
#include <atc_string.h>
#include <exe.h>

#define SET_RESULT_AND_GOTO_EXIT(value) result = (value); goto exit;
#define CPMS_MAX_STORAGE_LENGTH 2
#define STORAGE_AREA_SM "SM" /* (U)SIM storage */
#define STORAGE_AREA_ME "ME" /* ME (EEPROM/FileSystem) storage */
#define STORAGE_AREA_MT "MT" /* Any storage(reserved) */
#define CGSMS_SERVICE_UNKNOWN 0xff
#define CGSMS_SERVICE_MAX_VALUE 3
#define CGML_MAX_PDU_LENGTH 176 /* service address: 10 + tpdu: 164 + 2 extra */

enum {
    SMS_CMGW_STATUS_UNREAD = 0,
    SMS_CMGW_STATUS_READ = 1,
    SMS_CMGW_STATUS_UNSENT = 2,
    SMS_CMGW_STATUS_SENT = 3
} sms_cmgw_status_t;

static bool cpms_parameters_are_valid(StrParam_t mem1, StrParam_t mem2, StrParam_t mem3);
static bool cpms_get_msg_storage_from_string(const char *const memory_string, msg_storage_t *const memory);
static bool cpms_get_string_from_msg_storage(msg_storage_t msg_storage, char *const memory_string);
static void get_msg_storage_from_parser(exe_cpms_msg_storage_area_t *const msg_storage_area, const AT_ParserState_s *const parser_p);
static AT_Command_e cmgd_delete_specified(uint16_t Index, AT_ParserState_s  *parser_p);
static AT_Command_e cmgd_delete_all(exe_msg_status_t status, AT_ParserState_s  *parser_p);
static bool parse_cscb_structure(exe_cscb_data_t *cscb_data_p, char *mids_p, char *dcss_p);

static msg_storage_t msg_storage_mem3 = MSG_STORAGE_SM;

/*
*===============================================================================
*
*   Function: AT_PLUS_CMGD_Handle
*
*   INPUT:   parser_p  - Pointer to the current parser state.
*            info_text - Pointer to a string buffer to put information text.
*            message   - Pointer to the response message used for pending commands.
*
*
*   RETURNS: A status code for the execution of the command is returned:
*            AT_OK        - The command was successfully handled.
*            AT_ERROR     - There was a general error.
*            AT_PENDING   - The parser is exited before AT-command was completed
*            AT_CMS_ERROR - There was an SMS specific error
*
*
*
*   SET  - Delete message
*   TEST - Writes "OK" as a proof of the existence of the command
*
*===============================================================================
*/
AT_Command_e AT_PLUS_CMGD_Handle(AT_ParserState_s *parser_p, AT_CommandLine_t info_text, AT_ResponseMessage_t message)
{
    bool        error_flag   = false;
    AT_Command_e result = AT_OK;

    if (!parser_p) {
        ATC_LOG_E("AT_PLUS_CMGD_Handle: parser_p is NULL!");
        SET_RESULT_AND_GOTO_EXIT(AT_ERROR);
    }

    switch (parser_p->Mode) {
    case AT_MODE_SET: {
        if (!parser_p->IsPending) {
            uint16_t    index;
            uint16_t    delflag = 0;
            /* Read command parameters */
            index = (uint16_t)Parser_GetLongIntParam(parser_p, &error_flag, 0, 0XFFFF);

            if (error_flag) {
                SET_RESULT_AND_GOTO_EXIT(AT_ERROR);
            }

            if (!parser_p->NoMoreParams) {
                delflag = (uint16_t) Parser_GetLongIntParam(parser_p, &error_flag, 0, 0XFFFF);

                if (error_flag || (!parser_p->NoMoreParams)) {
                    SET_RESULT_AND_GOTO_EXIT(AT_ERROR);
                }
            }

            switch (delflag) {
            case 4:
                result = cmgd_delete_all(MSG_STATUS_ANY_STATUS, parser_p);
                goto exit;
                break;
            case 3:
                result = cmgd_delete_all(MSG_STATUS_UNSENT, parser_p);
                goto exit;
                break;
            case 2:
                result = cmgd_delete_all(MSG_STATUS_SENT, parser_p);
                goto exit;
                break;
            case 1:
                result = cmgd_delete_all(MSG_STATUS_READ, parser_p);
                goto exit;
                break;
            case 0:
                result = cmgd_delete_specified(index, parser_p);
                goto exit;
                break;
            default:
                Parser_SetCMS_ErrorType(parser_p, EXE_CMS_OPERATION_NOT_SUPPORTED);
                SET_RESULT_AND_GOTO_EXIT(AT_CMS_ERROR);
            }

            break;
        } else {
            exe_cmgd_response_t *res_data = (exe_cmgd_response_t *)message->data_p;

            /* response from exectuer */
            if (message->exe_result_code != EXE_SUCCESS) {
                Parser_SetCMS_ErrorType(parser_p, res_data->error);
                return AT_CMS_ERROR;
            }

            break;
        }
    }
    case AT_MODE_TEST: {
        if (true) {
            sprintf((char *)info_text, ": (0-%d),(0-4)", 5/*WorkArea1.TotalSpace*/);
        } else {
            strcpy((char *)info_text, ": (),(0-4)");
        }

        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        break;
    }

    default:
        Parser_SetCMS_ErrorType(parser_p, EXE_CMS_OPERATION_NOT_ALLOWED);
        ATC_LOG_E("AT_PLUS_CMGD_Handle: parser mode out of range!");
        SET_RESULT_AND_GOTO_EXIT(AT_CMS_ERROR);
    }

exit:
    return result;
}
/*
*===============================================================================
*
*   Function: AT_PLUS_CMGS_Handle
*
*   INPUT:   parser_p  - Pointer to the current parser state.
*            info_text - Pointer to a string buffer to put information text.
*            message   - Pointer to the response message used for pending commands.
*
*   OUTPUT:  parser_p->
*              Pos     - The current position in the command line.
*
*   RETURNS: A status code for the execution of the command is returned:
*            AT_OK        - The command was successfully handled.
*            AT_ERROR     - There was a general error.
*            AT_PENDING   - The parser is exited before AT-command was completed
*            AT_CMS_ERROR - There was an SMS specific error
*
*   This handle function is used to send SMS messages  with SMS-COMMAND and SMS TPDUs.
*   The SM shall be given in <pdu>-format, which is a GSM 04.11 SC address followed
*   by a GSM 03.40 TPDU.
*
*   Ref: SIR 2.3 Ensemble S11 (GSM SMS and CBS PDU Mode) Rev PC14
*
*   SET  - Sends message
*   TEST - Writes "OK" as a proof of the existence of the command
*
*===============================================================================
*/
AT_Command_e AT_PLUS_CMGS_Handle(AT_ParserState_s *parser_p, AT_CommandLine_t info_text, AT_ResponseMessage_t message)
{
    switch (parser_p->Mode) {
    case AT_MODE_SET: {
        bool                  ErrorFlag   = false;
        exe_t                *exe_p       = NULL;
        exe_request_result_t  result      = EXE_SUCCESS;
        IntParam_t length = 0;
        exe_cmgs_t req_data;
        char *response_p = "\r\n> ";

        if (!parser_p->IsPending) {
            length = Parser_GetIntParam(parser_p, &ErrorFlag, NULL);

            if (ErrorFlag) {
                return AT_ERROR;
            }

            if (CGML_MAX_PDU_LENGTH < length || 1 > length) {
                Parser_SetCMS_ErrorType(parser_p, EXE_CMS_INVALID_PDU_MODE_PARAMETER);
                return AT_CMS_ERROR;
            }

            if (!parser_p->NoMoreParams) {
                Parser_SetCMS_ErrorType(parser_p, EXE_CMS_OPERATION_NOT_SUPPORTED);
                return AT_CMS_ERROR;
            }

            parser_p->PDU_Length = length;
            atc_context_set_transparent(parser_p->ContextID, true);
            /* Transparent mode set send prompt and receive PDU */
            parser_write_formatted_text(parser_p, (unsigned char *)response_p, strlen(response_p));
            return AT_PENDING;
        } else {
            if (atc_context_get_transparent(parser_p->ContextID)) {
                /* We are in transparent mode so we should get the PDU from the parser */
                size_t converted_size   = 0;
                uint8_t  sc_len         = 0;
                uint16_t pdu_offset     = 0;

                req_data.pdu_data_p = calloc((sizeof(unsigned char *)*message->len) / 2 + 1, 1);

                if (NULL == req_data.pdu_data_p) {
                    return AT_ERROR;
                }

                atc_string_convert(ATC_CHARSET_HEX, (uint8_t *)message->data_p, message->len,
                                   ATC_CHARSET_UNDEF_BINARY, (uint8_t *)req_data.pdu_data_p, &converted_size);

                /* Get length of SCA */
                sc_len = req_data.pdu_data_p[0];

                /* Sanity check pdu data based on input value <length> */
                pdu_offset = sc_len + 1;
                if (pdu_offset + parser_p->PDU_Length != converted_size) {
                    Parser_SetCMS_ErrorType(parser_p, EXE_CMS_INVALID_PDU_MODE_PARAMETER);
                    free(req_data.pdu_data_p);
                    return AT_CMS_ERROR;
                }

                req_data.length = parser_p->PDU_Length;
                atc_context_set_transparent(parser_p->ContextID, false);
                exe_p = atc_get_exe_handle();
                result = exe_request(exe_p, EXE_CMGS, &req_data,
                                     (void *)parser_p, &parser_p->request_handle);
                free(req_data.pdu_data_p);

                if (EXE_PENDING != result) {
                    return AT_ERROR;
                }

                return AT_PENDING;
                break;
            } else {
                exe_cmgs_t *res_data = (exe_cmgs_t *)message->data_p;

                /* response from executor */
                if (message->exe_result_code != EXE_SUCCESS) {
                    Parser_SetCMS_ErrorType(parser_p, res_data->error);
                    return AT_CMS_ERROR;
                } else {
                    sprintf((char *)info_text, ": %d", res_data->mr);
                    Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
                    return AT_OK;
                }
            }
        }

        break;
    }
    case AT_MODE_READ: {
        Parser_SetCMS_ErrorType(parser_p, EXE_CMS_OPERATION_NOT_ALLOWED);
        return AT_CMS_ERROR;
        break;
    }
    case AT_MODE_TEST: {
        break;
    }
    default:
        Parser_SetCMS_ErrorType(parser_p, EXE_CMS_OPERATION_NOT_ALLOWED);
        return AT_CMS_ERROR;
        break;
    }

    return AT_OK;
}

/*
*===============================================================================
*
*   Function: AT_PLUS_CMGW_Handle
*
*   INPUT:   Parser   - Pointer to the current parser state.
*            InfoText - Pointer to a string buffer to put information text.
*            Message  - Pointer to the response message used for pending commands.
*
*   OUTPUT:  Parser->
*              Pos    - The current position in the command line.
*
*   RETURNS: A status code for the execution of the command is returned:
*            AT_OK        - The command was successfully handled.
*            AT_ERROR     - There where a general error.
*            AT_CME_ERROR - There was an GSM specific error
*            AT_CMS_ERROR - There was an SMS specific error
*            AT_PENDING   - The parser is exited before AT-command was completed
*
*   The CMGW command is used to write a PDU in memory "SIM" or "ME" (EEPROM/Filesystem),
*   for subsequent sending by means of the +CMSS command.
*
*   SET   - Writes the PDU to memory.
*   TEST  - Confirms the availability of the +CMGW command
*
*===============================================================================
*/
AT_Command_e AT_PLUS_CMGW_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message)
{
    switch (parser_p->Mode) {
    case AT_MODE_SET: {
        bool                  ErrorFlag   = false;
        exe_t                *exe_p       = NULL;
        exe_request_result_t  result      = EXE_SUCCESS;
        IntParam_t length = 0;
        IntParam_t stat = SMS_CMGW_STATUS_UNSENT;
        exe_cmgw_t req_data;
        char *response_p = "\r\n> ";

        if (!parser_p->IsPending) {
            length = Parser_GetIntParam(parser_p, &ErrorFlag, &length);

            if (ErrorFlag) {
                return AT_ERROR;
            }

            if (CGML_MAX_PDU_LENGTH < length || 1 > length) {
                Parser_SetCMS_ErrorType(parser_p, EXE_CMS_INVALID_PDU_MODE_PARAMETER);
                return AT_CMS_ERROR;
            }

            if (!parser_p->NoMoreParams) {
                stat = Parser_GetIntParam(parser_p, &ErrorFlag, &stat);

                if (ErrorFlag) {
                    return AT_ERROR;
                }

                if (SMS_CMGW_STATUS_READ != stat  && SMS_CMGW_STATUS_UNREAD != stat &&  SMS_CMGW_STATUS_UNSENT != stat && SMS_CMGW_STATUS_SENT != stat) {
                    Parser_SetCMS_ErrorType(parser_p, EXE_CMS_OPERATION_NOT_SUPPORTED);
                    return AT_CMS_ERROR;
                }

                if (!parser_p->NoMoreParams) {
                    Parser_SetCMS_ErrorType(parser_p, EXE_CMS_OPERATION_NOT_SUPPORTED);
                    return AT_CMS_ERROR;
                }
            }

            parser_p->PDU_Length = length;
            parser_p->Stat = stat;
            atc_context_set_transparent(parser_p->ContextID, true);
            /* Transparent mode set send prompt and receive PDU */
            parser_write_formatted_text(parser_p, (unsigned char *)response_p, strlen(response_p));
            return AT_PENDING;
        } else {
            if (atc_context_get_transparent(parser_p->ContextID)) {
                /* We are in transparent mode so we should get the PDU from the parser */
                if (message->len < 1) {
                    atc_context_set_transparent(parser_p->ContextID, false);
                    return AT_ERROR;
                }

                size_t converted_size   = 0;
                uint8_t  sc_len         = 0;
                uint16_t pdu_offset     = 0;

                req_data.pdu_data_p = calloc((sizeof(unsigned char *)*message->len) / 2 + 1, 1);

                if (NULL == req_data.pdu_data_p) {
                    return AT_ERROR;
                }

                atc_string_convert(ATC_CHARSET_HEX, (uint8_t *)message->data_p, message->len,
                                   ATC_CHARSET_UNDEF_BINARY, (uint8_t *)req_data.pdu_data_p, &converted_size);

                if (NULL == req_data.pdu_data_p) {
                    ATC_LOG_E("AT_PLUS_CMGW_Handle: req_data.pdu_data_p is NULL!");
                    return AT_ERROR;
                }
                /* Get length of SCA */
                sc_len = req_data.pdu_data_p[0];

                /* Sanity check pdu data based on input value <length> */
                pdu_offset = sc_len + 1;
                if (pdu_offset + parser_p->PDU_Length != converted_size) {
                    Parser_SetCMS_ErrorType(parser_p, EXE_CMS_INVALID_PDU_MODE_PARAMETER);
                    free(req_data.pdu_data_p);
                    return AT_CMS_ERROR;
                }

                req_data.length = parser_p->PDU_Length;
                req_data.stat = parser_p->Stat;
                req_data.slot.storage = parser_p->cpms_msg_storage.mem2;
                req_data.slot.position = 0; /* first available */

                atc_context_set_transparent(parser_p->ContextID, false);
                exe_p = atc_get_exe_handle();
                result = exe_request(exe_p, EXE_CMGW, &req_data,
                                     (void *)parser_p, &parser_p->request_handle);
                free(req_data.pdu_data_p);

                if (EXE_PENDING != result) {
                    return AT_ERROR;
                }

                return AT_PENDING;
                break;
            } else {
                exe_cmgw_t *res_data = (exe_cmgw_t *)message->data_p;

                /* response from executor */
                if (message->exe_result_code != EXE_SUCCESS) {
                    Parser_SetCMS_ErrorType(parser_p, res_data->error);
                    return AT_CMS_ERROR;
                } else {
                    sprintf((char *)info_text, ": %d", res_data->slot.position);
                    Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
                    return AT_OK;
                }
            }
        }

        break;
    }
    case AT_MODE_READ: {
        Parser_SetCMEE_ErrorType(parser_p, CMEE_OPERATION_NOT_ALLOWED);
        return AT_CME_ERROR;
        break;
    }
    case AT_MODE_TEST: {
        break;
    }
    default:
        Parser_SetCMEE_ErrorType(parser_p, CMEE_OPERATION_NOT_ALLOWED);
        return AT_CME_ERROR;
        break;
    }

    return AT_OK;
}

/*
*===============================================================================
*
*   Function: AT_PLUS_CNMA_Handle
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
*            AT_CMS_ERROR - There was a GSM SMS specific error, the error code is
*                           returned in info_text.
*
*   The +CNMA function acknowledge that we have received the sms..
*

*   SET  - Send ack.
*   READ - Not supported
*   TEST - Returns the supported modes as a compound value.
*
*===============================================================================
*/
AT_Command_e AT_PLUS_CNMA_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message)
{
    exe_request_result_t result = EXE_SUCCESS;
    exe_cnma_t req_data;
    exe_t *exe_p = NULL;

    switch (parser_p->Mode) {
    case AT_MODE_SET:

        if (!parser_p->IsPending) {
            IntParam_t n = 0;
            IntParam_t length = 0;
            bool ErrorFlag = false;
            char *response_p = "\r\n> ";

            /* Read command parameters */
            n = Parser_GetIntParam(parser_p, &ErrorFlag, NULL);

            if (ErrorFlag) {
                return AT_ERROR;
            }

            if (n > 2) {
                Parser_SetCMS_ErrorType(parser_p, EXE_CMS_OPERATION_NOT_ALLOWED);
                return AT_CMS_ERROR;
            }

            parser_p->sms_ack_type = n;

            length = Parser_GetIntParam(parser_p, &ErrorFlag, &length);

            if (ErrorFlag) {
                return AT_ERROR;
            }

            parser_p->PDU_Length = length;

            /* If length is entered, PDU should be read */
            if (length > 0) {
                if (!parser_p->NoMoreParams) {
                    Parser_SetCMS_ErrorType(parser_p, EXE_CMS_OPERATION_NOT_SUPPORTED);
                    return AT_CMS_ERROR;
                }

                atc_context_set_transparent(parser_p->ContextID, true);
                /* Transparent mode set, send prompt and receive PDU */
                parser_write_formatted_text(parser_p, (unsigned char *)response_p, strlen(response_p));
                return AT_PENDING;
            }

            exe_p = atc_get_exe_handle();
            req_data.n = parser_p->sms_ack_type;
            req_data.length = 0;
            req_data.pdu_data_p = NULL;
            result = exe_request(exe_p, EXE_CNMA, &req_data,
                                 (void *)parser_p, &parser_p->request_handle);

            if (EXE_PENDING != result) {
                return AT_ERROR;
            }

            return AT_PENDING;

        } else {
            if (atc_context_get_transparent(parser_p->ContextID)) {
                /* We are in transparent mode so we should get the PDU from the parser */
                size_t converted_size = 0;

                req_data.pdu_data_p = calloc((sizeof(unsigned char *)*message->len) / 2 + 1, 1);

                if (NULL == req_data.pdu_data_p) {
                    return AT_ERROR;
                }

                atc_string_convert(ATC_CHARSET_HEX, (uint8_t *)message->data_p, message->len,
                                   ATC_CHARSET_UNDEF_BINARY, (uint8_t *)req_data.pdu_data_p, &converted_size);

                req_data.n = parser_p->sms_ack_type;
                req_data.length = parser_p->PDU_Length;
                atc_context_set_transparent(parser_p->ContextID, false);
                exe_p = atc_get_exe_handle();
                result = exe_request(exe_p, EXE_CNMA, &req_data,
                                     (void *) parser_p, &parser_p->request_handle);
                free(req_data.pdu_data_p);

                if (EXE_PENDING != result) {
                    return AT_ERROR;
                }

                return AT_PENDING;
                break;
            } else {
                if (message->exe_result_code != EXE_SUCCESS) {
                    Parser_SetCMS_ErrorType(parser_p, EXE_CMS_OPERATION_NOT_ALLOWED);
                    return AT_CMS_ERROR;
                }
            }
        }

        break;
    case AT_MODE_READ:
        Parser_SetCMS_ErrorType(parser_p, EXE_CMS_OPERATION_NOT_ALLOWED);
        return AT_CMS_ERROR;    /* Not supported */
        break;
    case AT_MODE_TEST:
        sprintf((char *) info_text, ": (0-2)");
        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        break;
    default:
        break;
    }

    return AT_OK;
}

/*
*===============================================================================
*
*   Function: AT_PLUS_CMMS_Handle
*
*   INPUT:   parser_p   - Pointer to the current parser state.
*            info_text   - Pointer to a string buffer to put information text.
*            message    - Pointer to the response message used for pending commands.
*
*   RETURNS: A status code for the execution of the command is returned:
*            AT_OK        - The command was successfully handled.
*            AT_ERROR     - There was a general error.
*            AT_CME_ERROR - There was a GSM specific error, the error code is
*                           returned in InfoText.
*
*   Ref: SIR 2.3 Ensemble S15 (GPRS) Rev PA20
*
*   SET   - Set command controls the continuity of SMS relay protocol link. When
*           feature is enabled (and supported by network) multiple messages can be
*           sent much faster as link is kept open.
*   READ  - The read command returns the current setting.
*   TEST  - Test command returns supported values as a compound value.
*===============================================================================
*/
AT_Command_e AT_PLUS_CMMS_Handle(AT_ParserState_s *parser_p, AT_CommandLine_t info_text, AT_ResponseMessage_t message)
{
    exe_request_result_t  result      = EXE_SUCCESS;
    exe_t                *exe_p       = NULL;
    exe_cnma_t            req_data;

    switch (parser_p->Mode) {
    case AT_MODE_SET: {
        if (!parser_p->IsPending) {
            bool                 ErrorFlag    = false;
            IntParam_t           n            = 0;

            n = Parser_GetIntParam(parser_p, &ErrorFlag, &n);

            if (ErrorFlag) {
                return AT_ERROR;
            }

            if (!parser_p->NoMoreParams) {
                Parser_SetCMS_ErrorType(parser_p, EXE_CMS_OPERATION_NOT_SUPPORTED);
                return AT_CMS_ERROR;
            }

            if (n > 2) {
                Parser_SetCMS_ErrorType(parser_p, EXE_CMS_OPERATION_NOT_ALLOWED);
                return AT_CMS_ERROR;
            }

            exe_p = atc_get_exe_handle();
            req_data.n = n;
            result = exe_request(exe_p, EXE_CMMS_SET, &req_data,
                                 (void *)parser_p, &parser_p->request_handle);

            if (EXE_PENDING != result) {
                ATC_LOG_E("AT_PLUS_CMMS_Handle, executer returned error (expected PENDING)");
                Parser_SetCMS_ErrorType(parser_p,
                                        EXE_CMS_UNKNOWN_ERROR);
                return  AT_CMS_ERROR;
            }

            return AT_PENDING;
        } else {

            if (message->exe_result_code != EXE_SUCCESS) {
                ATC_LOG_E("AT_PLUS_CMMS_Handle, executer returned error (expected SUCCESS)");
                Parser_SetCMS_ErrorType(parser_p,
                                        EXE_CMS_UNKNOWN_ERROR);
                return  AT_CMS_ERROR;
            }
        }

        break;
    }

    case AT_MODE_READ: {

        if (!parser_p->IsPending) {
            bool                  ErrorFlag   = false;

            exe_p = atc_get_exe_handle();
            result = exe_request(exe_p, EXE_CMMS_READ, NULL,
                                 (void *)parser_p, &parser_p->request_handle);

            if (EXE_PENDING != result) {
                ATC_LOG_E("AT_PLUS_CMMS_Handle, executer returned error (expected PENDING)");
                Parser_SetCMS_ErrorType(parser_p,
                                        EXE_CMS_UNKNOWN_ERROR);
                return  AT_CMS_ERROR;
            }

            return AT_PENDING;
        } else {
            if (message->exe_result_code != EXE_SUCCESS) {
                Parser_SetCMS_ErrorType(parser_p,
                                        EXE_CMS_UNKNOWN_ERROR);
                return  AT_CMS_ERROR;
            }

            exe_cmms_t *res_data = (exe_cmms_t *)message->data_p;

            sprintf((char *) info_text, ": %d", res_data->n);
            Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
            return AT_OK;
        }

        break;
    }
    case AT_MODE_TEST: {
        sprintf((char *)info_text, ": (0-2)");
        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        break;
    }
    default:
        break;
    }

    return AT_OK;
}

void atc_set_sms_cbs_subscription(exe_message_subscribe_t *message_subscribe_p)
{
    AT_ParserState_s   *parser_p = NULL;
    unsigned char       entry = MAX_NO_OF_OPEN_CHANNELS;

    if (NULL == message_subscribe_p) {
        ATC_LOG_E("check_sms_cbs_subscription: message_subscribe_p is NULL");
        return;
    }

    message_subscribe_p->sms = false;
    message_subscribe_p->cbs = false;

    /* Loop through all parser states to find subscriptions. */
    for (entry = 0; entry < MAX_NO_OF_OPEN_CHANNELS; entry++) {
        parser_p = ParserStateTable_GetParserState_ByEntry(entry);

        if (parser_p->cnmi_message_indication.mt > 0 ||
                parser_p->cnmi_message_indication.ds > 0 ||
                parser_p->cnmi_message_indication.bm > 0) {

            message_subscribe_p->sms = true;
        }

        if (parser_p->cnmi_message_indication.bm > 0) {
            message_subscribe_p->cbs = true;
        }
    }
}

/*
 *===============================================================================
 *
 *   Function: AT_PLUS_CNMI_Handle
 *
 *   INPUT:   parser_p  - Pointer to the current parser state.
 *            info_text - Pointer to a string buffer to put information text.
 *            message   - Pointer to the response message used for pending commands.
 *
 *   OUTPUT:  parser_p->
 *              Pos     - The current position in the command line.
 *
 *   RETURNS: A status code for the execution of the command is returned:
 *            AT_OK        - The command was successfully handled.
 *            AT_ERROR     - There was a general error.
 *            AT_CMS_ERROR - There was a GSM SMS specific error, the error code is
 *                           returned in info_text.
 *
 *   The +CNMI function selects the procedure, how receiving of new messages
 *   from the network is indicated.
 *
 *   Ref: SIR 2.1 Ensemble S11 (GSM SMS and CBS PDU Mode) Rev pA58.
 *
 *   SET  - Set a new message indication to TE
 *   READ - Shows the current setting
 *   TEST - Returns the supported modes as a compound value.
 *
 *===============================================================================
 */
AT_Command_e AT_PLUS_CNMI_Handle(AT_ParserState_s *parser_p, AT_CommandLine_t info_text, AT_ResponseMessage_t message)
{
    AT_Command_e result = AT_ERROR;

    switch (parser_p->Mode) {
    case AT_MODE_SET: {
        if (!parser_p->IsPending) {
            IntParam_t mode = 2;
            IntParam_t mt = 0;
            IntParam_t bm = 0;
            IntParam_t ds = 0;
            IntParam_t bfr = 0;
            bool       error_flag = false;
            exe_t     *exe_p = NULL;
            exe_request_result_t    exe_result = EXE_SUCCESS;
            exe_message_subscribe_t message_subscribe;

            /* Read command parameters */
            mode = Parser_GetIntParam(parser_p, &error_flag, &mode);
            mt   = Parser_GetIntParam(parser_p, &error_flag, &mt);
            bm   = Parser_GetIntParam(parser_p, &error_flag, &bm);
            ds   = Parser_GetIntParam(parser_p, &error_flag, &ds);
            bfr  = Parser_GetIntParam(parser_p, &error_flag, &bfr);

            if (error_flag) {
                ATC_LOG_E("AT_PLUS_CNMI_Handle: could not acquire parameter values!");
                return AT_ERROR;
            }

            if (!parser_p->NoMoreParams) {
                Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
                return AT_CME_ERROR;
            }

            if ((mode != 1 && mode != 2) || (bm == 1) || (bfr != 0)) {
                Parser_SetCMEE_ErrorType(parser_p, CMEE_OPERATION_NOT_SUPPORTED);
                return AT_CME_ERROR;
            }

            if ((mt > 3) || (bm > 2) || (ds > 2)) {
                Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
                return AT_CME_ERROR;
            }

            parser_p->cnmi_message_indication.mode = mode;
            parser_p->cnmi_message_indication.mt = mt;
            parser_p->cnmi_message_indication.bm = bm;
            parser_p->cnmi_message_indication.ds = ds;
            parser_p->cnmi_message_indication.bfr = bfr;

            atc_set_sms_cbs_subscription(&message_subscribe);

            exe_p = atc_get_exe_handle();
            exe_result = exe_request(exe_p, EXE_CNMI, &message_subscribe ,
                                     (void *)parser_p, &parser_p->request_handle);
            result = parser_handle_exe_result(exe_result, parser_p);
        } else {
            if (message->exe_result_code != EXE_SUCCESS) {
                Parser_SetCMS_ErrorType(parser_p, EXE_CMS_OPERATION_NOT_ALLOWED);
                result = AT_CMS_ERROR;
            }

            result = AT_OK;
        }

        break;
    }
    case AT_MODE_READ: {
        cnmi_message_indication_t cnmi_ind = parser_p->cnmi_message_indication;

        sprintf((char *) info_text, ": %d,%d,%d,%d,%d",
                cnmi_ind.mode, cnmi_ind.mt, cnmi_ind.bm, cnmi_ind.ds, cnmi_ind.bfr);

        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        result = AT_OK;
        break;
    }
    case AT_MODE_TEST: {
        sprintf((char *)info_text, ": (1,2),(0-3),(0,2),(0-2),(0)");
        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        result = AT_OK;
        break;
    }
    default:
        break;
    }

    return result;
} /* End of AT_PLUS_CNMI_Handle */
/*
 *===============================================================================
 *
 *   Function: AT_PLUS_CSMS_Handle
 *
 *   INPUT:   parser_p  - Pointer to the current parser state.
 *            info_text - Pointer to a string buffer to put information text.
 *            message   - Pointer to the response message used for pending commands.
 *
 *   OUTPUT:  parser_p->
 *              Pos     - The current position in the command line.
 *
 *   RETURNS: A status code for the execution of the command is returned:
 *            AT_OK        - The command was successfully handled.
 *            AT_ERROR     - There where a general error.
 *
 *   The +CSMS command is used to select messaging service.
 *
 *   Ref: SIR 2.3 Ensemble S11 (GSM SMS and CBS PDU Mode)
 *
 *   SET   - Selects message Service.
 *   READ  - Shows supported message types along with the current service setting.
 *   TEST  - Returns list of supported services.
 *
 *===============================================================================
 */
AT_Command_e AT_PLUS_CSMS_Handle(AT_ParserState_s *parser_p, AT_CommandLine_t info_text, AT_ResponseMessage_t message)
{

    exe_request_result_t result = EXE_SUCCESS;
    exe_t                *exe_p = NULL;
    exe_request_result_t exe_result = EXE_SUCCESS;

    switch (parser_p->Mode) {
    case AT_MODE_SET:

        if (!parser_p->IsPending) {
            bool ErrorFlag;
            ErrorFlag = false;
            exe_csms_t req_data;
            int Service = 0;

            req_data.n = Parser_GetIntParam(parser_p, &ErrorFlag, NULL);

            if (ErrorFlag) {
                return AT_ERROR;
            }

            if (!parser_p->NoMoreParams) {
                Parser_SetCMS_ErrorType(parser_p, EXE_CMS_OPERATION_NOT_SUPPORTED);
                return AT_CMS_ERROR;
            }

            if (req_data.n > 127) {
                Parser_SetCMS_ErrorType(parser_p, EXE_CMS_OPERATION_NOT_ALLOWED);
                return AT_CMS_ERROR;
            }

            if (req_data.n != 1 && req_data.n != 0) {
                Parser_SetCMS_ErrorType(parser_p, EXE_CMS_OPERATION_NOT_SUPPORTED);
                return AT_CMS_ERROR;
            }

            /* Make call to executor here */
            exe_p = atc_get_exe_handle();

            exe_result = exe_request(exe_p, EXE_CSMS_SET, &req_data,
                                     (void *)parser_p, &parser_p->request_handle);

            if (EXE_PENDING != exe_result) {
                return AT_ERROR;
            }

            return AT_PENDING;
        } else {
            if (message->exe_result_code != EXE_SUCCESS) {
                Parser_SetCMS_ErrorType(parser_p,
                                        EXE_CMS_OPERATION_NOT_ALLOWED);
                return  AT_CMS_ERROR;
            } else {
                sprintf((char *) info_text, ": 1,1,1"); /* Hard coded since no other values will be returned */
                Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
                return AT_OK;
            }
        }

        break;
    case AT_MODE_READ:

        if (!parser_p->IsPending) {
            exe_p = atc_get_exe_handle();
            result = exe_request(exe_p, EXE_CSMS_READ, NULL,
                                 (void *)parser_p, &parser_p->request_handle);

            if (EXE_PENDING != result) {
                ATC_LOG_E("AT_PLUS_CSMS_Handle, executer returned error (expected PENDING)");
                Parser_SetCMS_ErrorType(parser_p,
                                        EXE_CMS_UNKNOWN_ERROR);
                return  AT_CMS_ERROR;
            }

            return AT_PENDING;
        } else {
            if (message->exe_result_code != EXE_SUCCESS) {
                Parser_SetCMS_ErrorType(parser_p,
                                        EXE_CMS_UNKNOWN_ERROR);
                return  AT_CMS_ERROR;
            }

            exe_csms_t *res_data = (exe_csms_t *)message->data_p;

            sprintf((char *) info_text, ": %d,1,1,1", res_data->n);
            Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
            return AT_OK;
        }

        break;
    case AT_MODE_TEST:
        sprintf((char *) info_text, ": (0-1)");
        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        return AT_OK;
        break;
    default:
        break;
    }

    return AT_ERROR;
}
/*
 *===============================================================================
 *
 *   Function: AT_PLUS_CPMS_Handle
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
 *            AT_ERROR     - There where a general error.
 *            AT_CMS_ERROR - There was an SMS specific error
 *
 *   The CPMS command selects memory storage <mem1>, <mem2> and <mem3> to be used
 *   for reading, writing, etc.
 *
 *   Ref: SIR 2.3 Ensemble S11 rev. PC15 (GSM SMS and CBS PDU Mode)
 *
 *   SET   - Selects the memory storage
 *   READ  - Reads the currently selected memory storage
 *   TEST  - Returns the supported values designating memory storage.
 *
 *===============================================================================
 */
AT_Command_e AT_PLUS_CPMS_Handle(AT_ParserState_s *parser_p, AT_CommandLine_t info_text, AT_ResponseMessage_t message)
{
    AT_Command_e result = AT_OK;
    cpms_msg_storage_t *msg_storage_p;
    exe_cpms_msg_storage_area_t msg_storage_area;

    msg_storage_p = &(parser_p->cpms_msg_storage);

    switch (parser_p->Mode) {
    case AT_MODE_SET:

        if (!parser_p->IsPending) {
            bool error_flag = false;
            exe_request_result_t exe_result = EXE_FAILURE;
            StrParam_t mem1 = NULL;
            StrParam_t mem2 = NULL;
            StrParam_t mem3 = NULL;
            exe_t *exe_p = NULL;
            int i = 0;
            /* Parse out storage area parameters */
            mem1 = Parser_GetStrParam(parser_p, &error_flag, STORAGE_AREA_SM);    /* message storage 1 (Memory from which messages are read and deleted) */
            mem2 = Parser_GetStrParam(parser_p, &error_flag, STORAGE_AREA_SM);    /* message storage 2 (Memory to which writing and sending operations are made) */
            mem3 = Parser_GetStrParam(parser_p, &error_flag, STORAGE_AREA_SM);    /* message storage 3 (Memory to which received SMs are preferred to be stored) */
            /* Make sure that all parameters are OK and accept no additional parameters */
            if (error_flag) {
                Parser_SetCMS_ErrorType(parser_p, EXE_CMS_OPERATION_NOT_SUPPORTED);
                result = AT_CMS_ERROR;
                goto exit;
            } else if (!parser_p->NoMoreParams) {
                Parser_SetCMS_ErrorType(parser_p, EXE_CMS_OPERATION_NOT_SUPPORTED);
                result = AT_CMS_ERROR;
                goto exit;
            }

            if (!cpms_parameters_are_valid(mem1, mem2, mem3)) {
                Parser_SetCMS_ErrorType(parser_p, EXE_CMS_OPERATION_NOT_SUPPORTED);
                result = AT_CMS_ERROR;
                goto exit;
            }

            if (!cpms_get_msg_storage_from_string(mem1, &(msg_storage_p->mem1)) ||
                    !cpms_get_msg_storage_from_string(mem2, &(msg_storage_p->mem2)) ||
                    !cpms_get_msg_storage_from_string(mem3, &(msg_storage_mem3))) {

                Parser_SetCMS_ErrorType(parser_p, EXE_CMS_OPERATION_NOT_SUPPORTED);
                result = AT_CMS_ERROR;
                goto exit;
            }

            get_msg_storage_from_parser(&msg_storage_area, parser_p);

            exe_p = atc_get_exe_handle();
            exe_result = exe_request(exe_p, EXE_CPMS, &msg_storage_area,
                                     (void *)parser_p, &parser_p->request_handle);

            if (EXE_PENDING != exe_result) {
                ATC_LOG_E("AT_PLUS_CPMS_Handle: incorrect state of exe_result: %d, expected EXE_PENDING!", exe_result);
                result = AT_ERROR;
                goto exit;
            }

            result = AT_PENDING;
        } else {
            exe_cpms_msg_storage_area_t *resp_data_p = (exe_cpms_msg_storage_area_t *)message->data_p;

            if (message->exe_result_code != EXE_SUCCESS) {
                ATC_LOG_E("AT_PLUS_CPMS_Handle: incorrect state of exe_result: %d, expected EXE_SUCCESS!", message->exe_result_code);
                Parser_SetCMS_ErrorType(parser_p, EXE_CMS_OPERATION_NOT_ALLOWED);
                result = AT_CMS_ERROR;
                goto exit;
            }

            sprintf((char *)info_text, ": %d,%d,%d,%d,%d,%d",
                    resp_data_p->mem1.used_space,
                    resp_data_p->mem1.total_space,
                    resp_data_p->mem2.used_space,
                    resp_data_p->mem2.total_space,
                    resp_data_p->mem3.used_space,
                    resp_data_p->mem3.total_space);

            Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
            result = AT_OK;
        }

        break;
    case AT_MODE_READ: {
        if (!parser_p->IsPending) {
            exe_t                 *exe_p = NULL;
            exe_request_result_t   exe_result = EXE_FAILURE;

            get_msg_storage_from_parser(&msg_storage_area, parser_p);

            exe_p = atc_get_exe_handle();
            exe_result = exe_request(exe_p, EXE_CPMS, &msg_storage_area,
                                     (void *)parser_p, &parser_p->request_handle);

            if (EXE_PENDING != exe_result) {
                result = AT_ERROR;
                goto exit;
            }

            result = AT_PENDING;
        } else {
            char string_mem1[3] = {0};
            char string_mem2[3] = {0};
            char string_mem3[3] = {0};
            exe_cpms_msg_storage_area_t *resp_data_p = (exe_cpms_msg_storage_area_t *)message->data_p;

            if (message->exe_result_code != EXE_SUCCESS) {
                ATC_LOG_E("AT_PLUS_CPMS_Handle: incorrect state of exe_result: %d, expected EXE_SUCCESS!", message->exe_result_code);
                Parser_SetCMS_ErrorType(parser_p, EXE_CMS_OPERATION_NOT_ALLOWED);
                result = AT_CMS_ERROR;
                goto exit;
            }

            cpms_get_string_from_msg_storage(resp_data_p->mem1.memory, string_mem1);
            cpms_get_string_from_msg_storage(resp_data_p->mem2.memory, string_mem2);
            cpms_get_string_from_msg_storage(resp_data_p->mem3.memory, string_mem3);

            sprintf((char *)info_text, ": \"%s\",%d,%d,\"%s\",%d,%d,\"%s\",%d,%d",
                    string_mem1,
                    resp_data_p->mem1.used_space,
                    resp_data_p->mem1.total_space,
                    string_mem2,
                    resp_data_p->mem2.used_space,
                    resp_data_p->mem2.total_space,
                    string_mem3,
                    resp_data_p->mem3.used_space,
                    resp_data_p->mem3.total_space);

            Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
            result = AT_OK;
        }

        break;
    }
    case AT_MODE_TEST:
        sprintf((char *)info_text, ": (\"SM\",\"ME\"),(\"SM\",\"ME\"),(\"SM\",\"ME\")");
        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        result = AT_OK;
        break;
    default:
        break;
    }

exit:
    return result;
}

/*
*===============================================================================
*
*   Function: AT_STAR_ESMSFULL_Handle
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
*            AT_CMS_ERROR - There was a GSM SMS specific error, the error code is
*                           returned in info_text.
*
*   The *ESMSFULL function indicates sms storage full or not
*

*   SET  - Send ack.
*   READ - Not supported
*   TEST - Returns the supported modes as a compound value.
*
*===============================================================================
*/
AT_Command_e AT_STAR_ESMSFULL_Handle(AT_ParserState_s *parser_p,
                                     AT_CommandLine_t info_text,
                                     AT_ResponseMessage_t message)
{
    exe_request_result_t result = EXE_SUCCESS;

    switch (parser_p->Mode) {
    case AT_MODE_SET:

        if (!parser_p->IsPending) {
            bool ErrorFlag = false;
            exe_t *exe_p = NULL;
            IntParam_t full = 0;
            exe_esmsfull_t req_data;
            /* Read command parameter */
            full = Parser_GetIntParam(parser_p, &ErrorFlag, NULL);

            if (ErrorFlag) {
                return AT_ERROR;
            }

            /* Only one parameter supported */
            if (!parser_p->NoMoreParams) {
                Parser_SetCMS_ErrorType(parser_p,
                                        EXE_CMS_OPERATION_NOT_SUPPORTED);
                return AT_CMS_ERROR;
            }
            if (full > 1) {
                Parser_SetCMS_ErrorType(parser_p,
                                        EXE_CMS_OPERATION_NOT_ALLOWED);
                return AT_CMS_ERROR;
            }

            exe_p = atc_get_exe_handle();

            if (NULL == exe_p) {
                return AT_ERROR;
            }

            req_data.full = (bool)full;
            result = exe_request(exe_p, EXE_ESMSFULL, &req_data,
                                 (void *)parser_p, &parser_p->request_handle);

            if (0 == full) {
                if (EXE_PENDING != result) {
                    return AT_ERROR;
                }

                return AT_PENDING;
            } else {
                if (EXE_SUCCESS != result) {
                    return AT_ERROR;
                }

                return AT_OK;
            }
        } else {
            if (message->exe_result_code != EXE_SUCCESS) {
                Parser_SetCMS_ErrorType(parser_p,
                                        EXE_CMS_OPERATION_NOT_ALLOWED);
                return  AT_CMS_ERROR;
            }
        }

        break;

    case AT_MODE_READ:

        if (!parser_p->IsPending) {
            exe_request_result_t exe_res = EXE_FAILURE;
            exe_esmsfull_read_t esmsfull_read_response;
            exe_t *exe_p;
            exe_p = atc_get_exe_handle();
            if (NULL == exe_p) {
                ATC_LOG_E("error exe_p is null \n");
                return AT_ERROR;
            }

            exe_res = exe_request(exe_p, EXE_ESMSFULL_READ, (void *) &esmsfull_read_response,
                                  (void *)parser_p, &parser_p->request_handle);

            if (EXE_PENDING != exe_res) {
                ATC_LOG_E("error exe_res = %d \n",exe_res);
                return AT_ERROR;
            }

            return AT_PENDING;
        } else {
            exe_esmsfull_read_t *resp_data_p = (exe_esmsfull_read_t *)message->data_p;

            if (message->exe_result_code != EXE_SUCCESS) {
                Parser_SetCMS_ErrorType(parser_p,
                                        EXE_CMS_OPERATION_NOT_ALLOWED);
                return  AT_CMS_ERROR;
            }
            sprintf((char *)info_text,": %d",resp_data_p->memfull);

            Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
            return AT_OK;
        }

        break;

    case AT_MODE_TEST:
        strcpy((char *)info_text, ": (0,1)");
        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        result = AT_OK;
        break;

    default:
        break;
    }

    return AT_OK;
}

/*
 * Check type of SMS indication CMT or CMTI depending on +CNMI <mt> value, SMS class and coding group.
 */
atc_sms_indication_t get_sms_indication_type(unsigned char mt, uint8_t sms_coding_group, exe_sms_class_t sms_class)
{
    atc_sms_indication_t sms_indication = ATC_SMS_INDICATION_NONE;

    switch (mt) {
    case 1:
        sms_indication = ATC_SMS_INDICATION_CMTI;
        break;
    case 2:

        if (EXE_SMS_CATEGORY_CLASS_TWO == sms_class ||
                EXE_SMS_CODING_GROUP_MWI_STORE == sms_coding_group ||
                EXE_SMS_CODING_GROUP_MWI_STORE_16BIT == sms_coding_group) {
            sms_indication = ATC_SMS_INDICATION_CMTI;
        } else {
            sms_indication = ATC_SMS_INDICATION_CMT;
        }

        break;
    case 3:

        if (EXE_SMS_CATEGORY_CLASS_THREE == sms_class) {
            sms_indication = ATC_SMS_INDICATION_CMT;
        } else {
            sms_indication = ATC_SMS_INDICATION_CMTI;
        }

        break;
    default:
        break;
    }

    return sms_indication;
}

/*
 * Handle CMT - SMS events sent from modem.
 */
void handle_incoming_cmt_event(exe_sms_mt_t *response_p)
{
    AT_ParserState_s *parser_p   = NULL;
    unsigned char     entry      = MAX_NO_OF_OPEN_CHANNELS;
    char             *cmt_text_p = NULL;
    size_t          converted_size = 0;
    uint8_t          *hex_data_p = NULL;

    if (NULL == response_p) {
        ATC_LOG_E("handle_incoming_cmt_event: response_p is NULL");
        goto exit;
    }

    if (NULL == response_p->pdu_p) {
        ATC_LOG_E("handle_incoming_cmt_event: response_p->pdu_p is NULL");
        goto exit;
    }

    hex_data_p = calloc(2 * response_p->total_length + 1, 1);

    if (NULL == hex_data_p) {
        goto exit;
    }

    atc_string_convert(ATC_CHARSET_UNDEF_BINARY, (uint8_t *)response_p->pdu_p, response_p->total_length,
                       ATC_CHARSET_HEX, (uint8_t *)hex_data_p, &converted_size);


    /* pdu length could be max 3 digits because SMS_SHORT_MESSAGE_TPDU_MAX_LENGTH = 164 */
    /* size = <+CMT: XXX\r\n>(11 chars) + <strlen(response_p->pdu_p)> + <end_of_string>(1 char) */
    cmt_text_p = calloc(1, 12 + converted_size);


    if (NULL == cmt_text_p) {
        ATC_LOG_E("handle_incoming_cmt_event: memory allocation failed!");
        goto exit;
    }

    /* Compose response string. */
    /* Add TPDU length and <CR> <LF> to text information */
    sprintf(cmt_text_p, "+CMT: %d\r\n%s", response_p->tpdu_length, hex_data_p);

    /* Loop through all parser states to find subscriptions. */
    for (entry = 0; entry < MAX_NO_OF_OPEN_CHANNELS; entry++) {
        parser_p = ParserStateTable_GetParserState_ByEntry(entry);

        switch (get_sms_indication_type(parser_p->cnmi_message_indication.mt, response_p->group, response_p->class)) {
        case ATC_SMS_INDICATION_CMT:
            parser_send_unsolicited(parser_p, AT_TEXT_RESPONSE, (AT_CommandLine_t)cmt_text_p);
            break;
        case ATC_SMS_INDICATION_CMTI:

            if (0 == response_p->slot.position) {
                /* failed to store sms, send CMT */
                parser_send_unsolicited(parser_p, AT_TEXT_RESPONSE, (AT_CommandLine_t)cmt_text_p);
                break;
            }

            break;
        default:
            break;
        }
    }

exit:
    free(hex_data_p);
    free(cmt_text_p);
}

/*
 * Handle CMTI - SMS events sent from modem.
 */
void handle_incoming_cmti_event(exe_sms_mt_t *response_p)
{
    AT_ParserState_s    *parser_p = NULL;
    unsigned char        entry = MAX_NO_OF_OPEN_CHANNELS;
    atc_sms_indication_t indication_type;
    /* info_text length = <+CMTI: > + <memory string (4 chars)> + <,> +  <position (max 4 digits)> + <end of string> */
    char info_text[17] = {0};
    char string_mem[3] = {0}; /* <memory (2 chars) + <end of string> */

    if (NULL == response_p) {
        ATC_LOG_E("handle_incoming_cmti_event: response_p is NULL");
        return;
    }

    if (!cpms_get_string_from_msg_storage(response_p->slot.storage, string_mem)) {
        ATC_LOG_E("atc_broadcast_cmti: invalid message storage!");
        return;
    }

    sprintf(info_text, "+CMTI: \"%s\",%d", string_mem, response_p->slot.position);

    /* Loop through all parser states to find subscriptions. */
    for (entry = 0; entry < MAX_NO_OF_OPEN_CHANNELS; entry++) {
        parser_p = ParserStateTable_GetParserState_ByEntry(entry);
        indication_type = get_sms_indication_type(parser_p->cnmi_message_indication.mt, response_p->group, response_p->class);

        if (ATC_SMS_INDICATION_CMTI == indication_type) {
            parser_send_unsolicited(parser_p, AT_TEXT_RESPONSE, (AT_CommandLine_t)info_text);
        }
    }
}

/*
 * Handle CDS - SMS status report events sent from modem.
 */
void handle_incoming_cds_event(exe_sms_status_report_t *response_p)
{
    AT_ParserState_s *parser_p   = NULL;
    unsigned char     entry      = MAX_NO_OF_OPEN_CHANNELS;
    char             *cds_text_p = NULL;
    size_t          converted_size = 0;
    uint8_t          *hex_data_p = NULL;

    if (NULL == response_p) {
        ATC_LOG_E("handle_incoming_cds_event: response_p is NULL");
        goto exit;
    }

    if (NULL == response_p->pdu_p) {
        ATC_LOG_E("handle_incoming_cds_event: response_p->pdu_p is NULL");
        goto exit;
    }

    hex_data_p = calloc(2 * response_p->total_length + 1, 1);

    if (NULL == hex_data_p) {
        goto exit;
    }

    atc_string_convert(ATC_CHARSET_UNDEF_BINARY, (uint8_t *)response_p->pdu_p, response_p->total_length,
                       ATC_CHARSET_HEX, (uint8_t *)hex_data_p, &converted_size);

    /* pdu length could be max 3 digits because SMS_SHORT_MESSAGE_TPDU_MAX_LENGTH = 164 */
    /* size = <+DS: XXX\r\n>(11 chars) + <strlen(response_p->pdu_p)> + <end_of_string>(1 char) */
    cds_text_p = calloc(1, 12 + converted_size);

    if (NULL == cds_text_p) {
        ATC_LOG_E("handle_incoming_cds_event: memory allocation failed!");
        goto exit;
    }

    /* Compose response string. */
    /* Add TPDU length and <CR> <LF> to text information */
    sprintf(cds_text_p, "+CDS: %d\r\n%s", response_p->tpdu_length, hex_data_p);

    /* Loop through all parser states to find subscriptions. */
    for (entry = 0; entry < MAX_NO_OF_OPEN_CHANNELS; entry++) {
        parser_p = ParserStateTable_GetParserState_ByEntry(entry);

        switch (parser_p->cnmi_message_indication.ds) {
        case 1:
            parser_send_unsolicited(parser_p, AT_TEXT_RESPONSE, (AT_CommandLine_t)cds_text_p);
            break;
        case 2:

            if (0 == response_p->slot.position) {
                /* delivery status report not stored, send CDS */
                parser_send_unsolicited(parser_p, AT_TEXT_RESPONSE, (AT_CommandLine_t)cds_text_p);
            }

            break;
        default:
            break;
        }
    }

exit:
    free(hex_data_p);
    free(cds_text_p);
}

/*
 * Handle CDSI - SMS status report events sent from modem.
 */
void handle_incoming_cdsi_event(exe_sms_status_report_t *response_p)
{
    AT_ParserState_s    *parser_p = NULL;
    unsigned char        entry = MAX_NO_OF_OPEN_CHANNELS;
    /* info_text length = <+CDSI: > + <memory string (4 chars)> + <,> +  <position (max 4 digits)> + <end of string> */
    char info_text[17] = {0};
    char string_mem[3] = {0}; /* <memory (2 chars) + <end of string> */

    if (NULL == response_p) {
        ATC_LOG_E("handle_incoming_cdsi_event: response_p is NULL");
        return;
    }

    if (!cpms_get_string_from_msg_storage(response_p->slot.storage, string_mem)) {
        ATC_LOG_E("handle_incoming_cdsi_event: invalid message storage!");
        return;
    }

    sprintf(info_text, "+CDSI: \"%s\",%d", string_mem, response_p->slot.position);

    /* Loop through all parser states to find subscriptions. */
    for (entry = 0; entry < MAX_NO_OF_OPEN_CHANNELS; entry++) {
        parser_p = ParserStateTable_GetParserState_ByEntry(entry);

        if (2 == parser_p->cnmi_message_indication.ds) {
            parser_send_unsolicited(parser_p, AT_TEXT_RESPONSE, (AT_CommandLine_t)info_text);
        }
    }
}

/*
 * Handle CBM events sent from modem. Compose an AT string and send the unsolicited.
 */
void handle_incoming_cbm_event(exe_cbm_t *response_p)
{
    char                *cbm_text_p = NULL;
    AT_ParserState_s    *parser_p;
    unsigned char        entry;
    size_t             converted_size = 0;
    uint8_t             *hex_data_p = NULL;

    if (NULL == response_p) {
        ATC_LOG_E("handle_incoming_cbm_event: response_p is NULL");
        goto exit;
    }

    hex_data_p = calloc(2 * response_p->length + 1, 1);

    if (NULL == hex_data_p) {
        goto exit;
    }

    atc_string_convert(ATC_CHARSET_UNDEF_BINARY, (uint8_t *)response_p->pdu, response_p->length,
                       ATC_CHARSET_HEX, (uint8_t *)hex_data_p, &converted_size);

    cbm_text_p = calloc(1, 15 + converted_size);

    if (NULL == cbm_text_p) {
        ATC_LOG_E("handle_incoming_cbm_event: memory allocation failed!");
        goto exit;
    }

    /* Compose response string. */
    /* Add TPDU length and <CR> <LF> to text information */
    sprintf(cbm_text_p, "+CBM: %d\r\n%s", response_p->length, hex_data_p);

    /* Loop through all parser states to find subscriptions. */
    for (entry = 0; entry < MAX_NO_OF_OPEN_CHANNELS; entry++) {
        parser_p = ParserStateTable_GetParserState_ByEntry(entry);

        if (parser_p->cnmi_message_indication.bm) {
            parser_send_unsolicited(parser_p, AT_TEXT_RESPONSE, (AT_CommandLine_t)cbm_text_p);
        }
    }

exit:
    free(hex_data_p);
    free(cbm_text_p);
}

/*
 * Handle SMS storage status events sent from modem. Compose an AT string and send the unsolicited.
 */
void handle_incoming_ciev_storage_event(void *response_p)
{
    unsigned char               entry;
    AT_ParserState_s            *parser_p  = NULL;
    AT_CommandLine_t            info_text  = NULL;
    exe_ciev_storage_status_t   *ciev_storage_status_p = (exe_ciev_storage_status_t *)response_p;

    info_text = (AT_CommandLine_t) calloc(1, AT_MAX_DATA_LENGTH);

    if (NULL == info_text) {
        ATC_LOG_E("handle_incoming_ciev_storage_event: info_text failed to calloc");
        goto exit;
    }

    if (NULL == ciev_storage_status_p) {
        ATC_LOG_E("handle_incoming_ciev_storage_event: ciev_storage_status_p is NULL");
        goto exit;
    }

    sprintf((char *)info_text, "+CIEV: 10,%d", ciev_storage_status_p->storage_full_SIM);

    for (entry = 0; entry < MAX_NO_OF_OPEN_CHANNELS; entry++) {
        parser_p = ParserStateTable_GetParserState_ByEntry(entry);

        if (NULL != parser_p && CMER_MODE_FORWARD == parser_p->CMER_mode && CMER_IND_ACTIVATED == parser_p->CMER_ind) {
            parser_send_unsolicited(parser_p, AT_TEXT_RESPONSE, (AT_CommandLine_t)info_text);
        }
    }

exit:

    free(info_text);
}

/*
 *===============================================================================
 *  Function: AT_PLUS_CMGF_Handle
 *
 *  INPUT:   parser_p      - Pointer to the current parser state.
 *           info_text     - Pointer to a string buffer to put information text.
 *           Response      - Pointer to the requester response
 *
 *  OUTPUT:  parser_p->Pos - The current position in the command line.
 *
 *  RETURNS: A status code for the execution of the command is returned:
 *           AT_OK        - The command was successfully handled.
 *           AT_ERROR     - There was a general error.
 *           AT_CME_ERROR - There was a GSM specific error, the error code is
 *                          returned in info_text.
 *
 *  Description: +CMGF Message Format determines the input/output format
 *               of messages used with the commands send, list, read, write
 *               and unsolicited results.
 *
 *===============================================================================
 */
AT_Command_e AT_PLUS_CMGF_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message)
{
    AT_Command_e result = AT_ERROR;

    switch (parser_p->Mode) {
    case AT_MODE_SET: {
        IntParam_t mode = 0;
        bool error_flag = false;

        mode = Parser_GetIntParam(parser_p, &error_flag, &mode);

        if (error_flag) {
            break;
        }

        if (!parser_p->NoMoreParams) {
            /* Don't accept more parameters than <mode>. */
            parser_p->CMS_ErrorType = EXE_CMS_OPERATION_NOT_SUPPORTED;
            result = AT_CMS_ERROR;
            break;
        }

        /* The only supported mode is PDU: 0 */
        if (0 != mode) {
            parser_p->CMS_ErrorType = EXE_CMS_OPERATION_NOT_SUPPORTED;
            result = AT_CMS_ERROR;
            break;
        }

        result = AT_OK;
        break;
    }

    case AT_MODE_READ: {
        (void) strcpy((char *) info_text, ": 0");
        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        result = AT_OK;
        break;
    }

    case AT_MODE_TEST: {
        (void) strcpy((char *) info_text, ": (0)");
        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        result = AT_OK;
        break;
    }

    default: {
        ATC_LOG_E("AT_PLUS_CMGF_Handle(): Unknown operation mode");
        break;
    }

    } /* End switch */

    return result;
}

static bool cpms_parameters_are_valid(StrParam_t mem1, StrParam_t mem2, StrParam_t mem3)
{
    bool is_valid = false;
    msg_storage_t memory = MSG_STORAGE_UNKNOWN;

    if (strlen(mem1) > CPMS_MAX_STORAGE_LENGTH ||
            strlen(mem2) > CPMS_MAX_STORAGE_LENGTH ||
            strlen(mem3) > CPMS_MAX_STORAGE_LENGTH) {
        goto exit;
    }

    if (!cpms_get_msg_storage_from_string(mem1, &memory) ||
            !cpms_get_msg_storage_from_string(mem2, &memory) ||
            !cpms_get_msg_storage_from_string(mem3, &memory)) {
        goto exit;
    }

    is_valid = true;
exit:
    return is_valid;
}

/*
*===============================================================================
*
*   Function: cpms_get_msg_storage_from_string
*
*   Checks a string and if it contains a valid sms storage ("ME" or "SM")
*
*===============================================================================
*/
static bool cpms_get_msg_storage_from_string(const char *const memory_string, msg_storage_t *const memory)
{
    bool result = false;
    char temp_string[3];

    if (NULL ==  memory || NULL == memory_string) {
        return false;
    }

    temp_string[0] = memory_string[0] & ~0x20;
    temp_string[1] = memory_string[1] & ~0x20;
    temp_string[2] = '\0';

    if (strcmp((char *)temp_string, STORAGE_AREA_SM) == 0) {
        *memory = MSG_STORAGE_SM;
        result = true;
    } else if (strcmp((char *)temp_string, STORAGE_AREA_ME) == 0) {
        *memory = MSG_STORAGE_ME;
        result = true;
    }

    return result;
}

/*
*===============================================================================
*
*   Function: cpms_get_string_from_msg_storage
*
*   Copies a string to MemoryString depending on the value given in MSG_Storage
*
*===============================================================================
*/
static bool cpms_get_string_from_msg_storage(msg_storage_t msg_storage, char *const memory_string)
{
    bool result = false;

    if (NULL == memory_string) {
        return false;
    }

    switch (msg_storage) {
    case MSG_STORAGE_SM:
        strncpy(memory_string, STORAGE_AREA_SM, CPMS_MAX_STORAGE_LENGTH);
        memory_string[CPMS_MAX_STORAGE_LENGTH] = '\0';
        result = true;
        break;
    case MSG_STORAGE_ME:
        strncpy(memory_string, STORAGE_AREA_ME, CPMS_MAX_STORAGE_LENGTH);
        memory_string[CPMS_MAX_STORAGE_LENGTH] = '\0';
        result = true;
        break;
    case MSG_STORAGE_MT:
        strncpy(memory_string, STORAGE_AREA_MT, CPMS_MAX_STORAGE_LENGTH);
        memory_string[CPMS_MAX_STORAGE_LENGTH] = '\0';
        result = true;
        break;
    default:
        break;
    }

    return result;
}

/*
*===============================================================================
*
*   Function:get_msg_storage_from_parser
*
*   This function returns the current Message storage location of each type.
*
*===============================================================================
*/
static void get_msg_storage_from_parser(exe_cpms_msg_storage_area_t *const msg_storage_area, const AT_ParserState_s *const parser_p)
{
    if (NULL ==  msg_storage_area || NULL == parser_p) {
        return;
    }

    msg_storage_area->mem1.memory = parser_p->cpms_msg_storage.mem1;
    msg_storage_area->mem2.memory = parser_p->cpms_msg_storage.mem2;
    msg_storage_area->mem3.memory = msg_storage_mem3;
}

/*
*===============================================================================
*
*   Function: cmgd_delete_specified
*
*   Static functions used in AT_PLUS_CMGD_Handle
*
*===============================================================================
*/
static AT_Command_e cmgd_delete_specified(uint16_t Index, AT_ParserState_s *parser_p)
{
    exe_t                *exe_p = NULL;
    exe_request_result_t exe_result = EXE_SUCCESS;
    AT_Command_e         result = AT_ERROR;
    exe_cmgd_del_t       req_data;

    exe_p = atc_get_exe_handle();

    memset(&req_data, 0x00, sizeof(exe_cmgd_del_t));
    req_data.storage = parser_p->cpms_msg_storage.mem1;
    req_data.index = Index;

    exe_result = exe_request(exe_p, EXE_CMGD_DELETE, &req_data,
                             (void *)parser_p, &parser_p->request_handle);

    if (EXE_PENDING == exe_result) {
        result = AT_PENDING;
    }

    return result;
}

/*
*===============================================================================
*
*   Function: cmgd_delete_all
*
*   Static functions used in AT_PLUS_CMGD_Handle
*
*===============================================================================
*/

static AT_Command_e cmgd_delete_all(exe_msg_status_t status, AT_ParserState_s *parser_p)
{
    exe_t                 *exe_p = NULL;
    exe_request_result_t   exe_result = EXE_SUCCESS;
    AT_Command_e           result = AT_ERROR;
    exe_cmgd_del_all_t     req_data;

    exe_p = atc_get_exe_handle();

    memset(&req_data, 0x00, sizeof(exe_cmgd_del_all_t));
    req_data.storage = parser_p->cpms_msg_storage.mem1;
    req_data.status = status;

    exe_result = exe_request(exe_p, EXE_CMGD_DELETE_ALL, &req_data,
                             (void *)parser_p, &parser_p->request_handle);

    if (EXE_PENDING == exe_result) {
        result = AT_PENDING;
    }

    return result;
}

/*
 *===============================================================================
 *
 *   Function: parse_cscb_structure
 *
 *   INTPUT:  num_of_subs         - count of subscription ranges
 *            cscb_subscription_p - pointer to exe_cscb_subscription_t structure
 *
 *   OUTPUT:  mids - Pointer to MIDs string.
 *            dcss - Pointer to DCSs string
 *
 *===============================================================================
 */
static bool parse_cscb_structure(exe_cscb_data_t *cscb_data_p, char *mids_p, char *dcss_p)
{
    char *mid = mids_p;
    char *dcs = dcss_p;
    uint16_t mid1;
    uint16_t mid2;
    uint8_t dcs1;
    uint8_t dcs2;
    exe_cscb_subscription_t *cscb_subscription_p = NULL;
    int i;

    if (NULL == mids_p ||
            NULL == dcss_p ||
            NULL == cscb_data_p) {
        ATC_LOG_E("parse_cscb_structure: incorrect arguments!");
        return false;
    }

    cscb_subscription_p = cscb_data_p->subscription_p;

    for (i = cscb_data_p->num_of_subs - 1; i >= 0; i--) {
        mid1 = cscb_subscription_p[i].first_message_identifier;
        mid2 = cscb_subscription_p[i].last_message_identifier;
        dcs1 = cscb_subscription_p[i].first_data_coding_scheme;
        dcs2 = cscb_subscription_p[i].last_data_coding_scheme;

        /* MID values can be between 0 and 0xFFFF, inclusive. I.e. the complete
         * range of uint16_t variables. This means that no range checking is
         * required. */
        if ('\0' != *mid) {
            strcat(mid, ",");
        }

        sprintf(mid + strlen(mid), "%d", mid1);

        if (mid2 != mid1) {
            strcat(mid, "-");
            sprintf(mid + strlen(mid), "%d", mid2);
        }

        if (0xA0 != dcs1 || 0xA0 != dcs2) {
            if ('\0' != *dcs) {
                strcat(dcs, ",");
            }

            sprintf(dcs + strlen(dcs), "%d", dcs1);

            if (dcs2 != dcs1) {
                strcat(dcs, "-");
                sprintf(dcs + strlen(dcs), "%d", dcs2);
            }
        }
    }

    return true;
}

/*
 *===============================================================================
 *
 *   Function: count_ranges
 *
 *   INPUT:   in_string  - Pointer to MIDs or DCSs string.
 *
 *   RETURNS: counter - number of subscription ranges
 *
 *
 *===============================================================================
 */
int count_ranges(char *string)
{
    uint16_t i = 0;
    int num_of_ranges = 0;
    bool found_digit = false;

    if (NULL == string) {
        return 0;
    }

    /* there are as many ranges as "," plus 1 */
    for (i = 0; i < strlen(string); i++) {
        if (',' == string[i]) {
            num_of_ranges++;
        }

        if (string[i] >= '0' && string[i] <= '9') {
            found_digit = true;
        }
    }

    if (found_digit) {
        num_of_ranges++;
    }

    return num_of_ranges;
}

#define CSCB_MIDS_PARSING true
#define CSCB_DCS_PARSING false
/*
 *===============================================================================
 *
 *   Function: parse_single_string
 *
 *   INPUT:   in_string  - Pointer to MIDs or DCSs string
 *            mids - indication whether MIDs or DCSs string is parsed
 *   OUTPUT:  cscb_subscription_p     -pointer to exe_cscb_subscription_t structure
 *
 *   RETURNS: true/false - parsing went/didn't went well
 *
 *
 *===============================================================================
 */
bool parse_single_string(char *in_string, exe_cscb_subscription_t *cscb_subscription_p, bool current_parsing)
{
    uint16_t i = 0;
    int number = 0;
    int prev_num = -1;
    int sub_no = 0;
    bool dash = false;

    if (0 == strlen(in_string)) {
        return true;
    }

    for (i = 0; i < strlen(in_string); i++) {
        /* ignore all whitespace characters */
        if (' ' == in_string[i] || '\r' == in_string[i]) {
            continue;
            /* create a number */
        } else if (in_string[i] >= 0x30 && in_string[i] <= 0x39) {
            number = number * 10 + in_string[i] - 0x30;
            continue;
        } else if ('-' == in_string[i]) {
            if (dash) {
            return false;
            }

            dash = true;

            if (CSCB_MIDS_PARSING == current_parsing) {
                cscb_subscription_p[sub_no].first_message_identifier = number;
            } else {
                cscb_subscription_p[sub_no].first_data_coding_scheme = number;
            }

            prev_num = number;
            number = 0;
            continue;
        } else if (',' == in_string[i]) {
            if (dash) {
                if( number < prev_num ) return false;
                dash = false;
                if(CSCB_MIDS_PARSING == current_parsing) {
                cscb_subscription_p[sub_no].last_message_identifier = number;
                } else {
                cscb_subscription_p[sub_no].last_data_coding_scheme = number;
                }
            } else {
                if (CSCB_MIDS_PARSING == current_parsing) {
                    cscb_subscription_p[sub_no].first_message_identifier = number;
                    cscb_subscription_p[sub_no].last_message_identifier = number;
                } else {
                    cscb_subscription_p[sub_no].first_data_coding_scheme = number;
                    cscb_subscription_p[sub_no].last_data_coding_scheme = number;
                }
            }

            sub_no++;
            prev_num = number;
            number = 0;
            continue;
        } else {
            return false;
        }
    }
    if (dash) {
        if( number < prev_num ) return false;
        if (CSCB_MIDS_PARSING == current_parsing) {
            cscb_subscription_p[sub_no].last_message_identifier = number;
        } else {
            cscb_subscription_p[sub_no].last_data_coding_scheme = number;
        }
    } else {
        if (CSCB_MIDS_PARSING == current_parsing) {
            cscb_subscription_p[sub_no].first_message_identifier = number;
            cscb_subscription_p[sub_no].last_message_identifier = number;
        } else {
            cscb_subscription_p[sub_no].first_data_coding_scheme = number;
            cscb_subscription_p[sub_no].last_data_coding_scheme = number;
        }
    }

    return true;
}

/*
 *===============================================================================
 *
 *   Function: parse_cscb_strings
 *
 *   INPUT:   mids  - Pointer to MIDs string.
 *            dcss - Pointer to DCSs string
 *
 *   OUTPUT:  num_of_subs     - count of subscription ranges
 *
 *   RETURNS: cscb_subscription_p - pointer to exe_cscb_subscription_t structure
 *
 *
 *===============================================================================
 */
bool parse_cscb_strings(char *mids_p, char *dcss_p, exe_cscb_data_t *cscb_data_p)
{
    exe_cscb_subscription_t *cscb_subscription_p = NULL;
    int i = 0;

    if (NULL == mids_p ||
            NULL == dcss_p ||
            NULL == cscb_data_p) {
        ATC_LOG_E("AT_PLUS_CSCB_Handle: incorrect arguments!");
        return false;
    }

    cscb_subscription_p = cscb_data_p->subscription_p;

    for (i = 0; i < cscb_data_p->num_of_subs; i++) {
        /* reserved values as defaults */
        cscb_subscription_p[i].first_data_coding_scheme = 0x0F;
        cscb_subscription_p[i].last_data_coding_scheme = 0x0F;
        cscb_subscription_p[i].first_message_identifier = 0xFFFF;
        cscb_subscription_p[i].last_message_identifier = 0xFFFF;
    }

    if (cscb_data_p->num_of_subs > 0) {
        if (!parse_single_string(mids_p, cscb_subscription_p, CSCB_MIDS_PARSING)) {
            ATC_LOG_E("AT_PLUS_CSCB_Handle: parse_single_string failed.");
            return false;
        }

        if (!parse_single_string(dcss_p, cscb_subscription_p, CSCB_DCS_PARSING)) {
            ATC_LOG_E("AT_PLUS_CSCB_Handle: parse_single_string failed.");
            return false;
        }
    }

    return true;
}

/*
 *===============================================================================
 *
 *   Function: AT_PLUS_CSCB_Handle
 *
 *   INPUT:   parser_p  - Pointer to the current parser state.
 *            info_text - Pointer to a string buffer to put information text.
 *            message   - Pointer to the response message used for pending commands.
 *
 *   OUTPUT:  parser_p->
 *              Pos     - The current position in the command line.
 *
 *   RETURNS: A status code for the execution of the command is returned:
 *            AT_OK        - The command was successfully handled.
 *            AT_ERROR     - There was a general error.
 *            AT_CMS_ERROR - There was a GSM SMS specific error, the error code is
 *                           returned in info_text.
 *
 *   The +CSCB command selects which types of CBMs are to be received by the ME
 *
 *   SET  - Select cell broadcast message type.
 *   READ - Read the current setting.
 *   TEST - Test command to show if the command is supported.
 *
 *===============================================================================
 */
AT_Command_e AT_PLUS_CSCB_Handle(AT_ParserState_s *parser_p, AT_CommandLine_t info_text, AT_ResponseMessage_t message)
{
    AT_Command_e result = AT_ERROR;

    switch (parser_p->Mode) {
    case AT_MODE_SET: {
        if (!parser_p->IsPending) {
            bool error_flag = false;
            IntParam_t mode = 0;
            StrParam_t mids_p;
            StrParam_t dcss_p;
            char *empty_string_p = "";
            exe_t *exe_p = NULL;
            exe_request_result_t  exe_result = EXE_SUCCESS;
            int num_of_subscriptions = 0;
            exe_cscb_subscription_t *cscb_subscription_p = NULL;
            exe_cscb_data_t cscb_data;

            /* Read command parameters */
            mode = Parser_GetIntParam(parser_p, &error_flag, &mode);
            mids_p = Parser_GetStrParam(parser_p, &error_flag, empty_string_p);
            dcss_p = Parser_GetStrParam(parser_p, &error_flag, empty_string_p);

            if (error_flag) {
                ATC_LOG_E("AT_PLUS_CSCB_Handle: could not acquire parameter values!");
                return AT_ERROR;
            }

            if (!parser_p->NoMoreParams) {
                ATC_LOG_E("AT_PLUS_CSCB_Handle: too many parameters specified!");
                return AT_ERROR;
            }

            if (mode != 0) {
                ATC_LOG_E("AT_PLUS_CSCB_Handle: invalid parameters!");
                return AT_ERROR;
            }

            num_of_subscriptions = MAX(count_ranges(mids_p), count_ranges(dcss_p));

            cscb_data.subscription_p = NULL;
            cscb_data.num_of_subs = num_of_subscriptions;

            if (num_of_subscriptions > 0) {
                cscb_subscription_p = calloc(num_of_subscriptions, sizeof(exe_cscb_subscription_t));

                if (NULL == cscb_subscription_p) {
                    ATC_LOG_E("AT_PLUS_CSCB_Handle: cscb_subscription_p allocation failed!");
                    return AT_ERROR;
                }

                cscb_data.subscription_p = cscb_subscription_p;
            }

            if (!parse_cscb_strings(mids_p, dcss_p, &cscb_data)) {
                ATC_LOG_E("AT_PLUS_CSCB_Handle: parse_cscb_strings failed!");
                free(cscb_subscription_p);
                return AT_ERROR;
            }

            exe_p = atc_get_exe_handle();
            exe_result = exe_request(exe_p, EXE_CSCB_SET, (void *)&cscb_data,
                                     (void *)parser_p, &parser_p->request_handle);

            free(cscb_subscription_p);
            result = parser_handle_exe_result(exe_result, parser_p);
        } else { /*end if !parser_p->IsPending */
            if (message->exe_result_code != EXE_SUCCESS) {
                Parser_SetCMS_ErrorType(parser_p, EXE_CMS_OPERATION_NOT_ALLOWED);
                return AT_CMS_ERROR;
            }

            result = AT_OK;
        }

        break;
    }
    case AT_MODE_READ: {
        if (!parser_p->IsPending) {
            exe_t *exe_p = NULL;
            exe_request_result_t exe_result = EXE_SUCCESS;

            exe_p = atc_get_exe_handle();
            exe_result = exe_request(exe_p, EXE_CSCB_READ, NULL,
                                     (void *)parser_p, &parser_p->request_handle);

            result = parser_handle_exe_result(exe_result, parser_p);
        } else {
            exe_cscb_data_t *cscb_data_p = (exe_cscb_data_t *)message->data_p;
            int num_of_subs = 0;
            char *mids_p = NULL;
            char *dcss_p = NULL;

            if (message->exe_result_code != EXE_SUCCESS) {
                Parser_SetCMS_ErrorType(parser_p, EXE_CMS_OPERATION_NOT_ALLOWED);
                return AT_CMS_ERROR;
            }


            if (NULL == cscb_data_p) {
                strcpy((char *)info_text, ": 0,\"\",\"\"");
                Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
                return AT_OK;
            }

            num_of_subs = cscb_data_p->num_of_subs;

            /* 12 characters - worst case scenario for numbers of type uint16 "xxxxx-xxxxx," */
            mids_p = calloc(12 * num_of_subs, sizeof(char));

            if (NULL == mids_p) {
                ATC_LOG_E("AT_PLUS_CSCB_Handle: cscb_subscription_p allocation failed!");
                return AT_ERROR;
            }

            /* 8 characters - worst case scenario for numbers of type uint8 "xxx-xxx," */
            dcss_p = calloc(8 * num_of_subs, sizeof(char));

            if (NULL == dcss_p) {
                ATC_LOG_E("AT_PLUS_CSCB_Handle: cscb_subscription_p allocation failed!");
                free(mids_p);
                return AT_ERROR;
            }

            if (!parse_cscb_structure(cscb_data_p, mids_p, dcss_p)) {
                ATC_LOG_E("AT_PLUS_CSCB_Handle: parse_cscb_structure failed!");
                free(mids_p);
                free(dcss_p);
                return AT_ERROR;
            }

            sprintf((char *) info_text, ": 0,\"%s\",\"%s\"", mids_p, dcss_p);
            Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
            free(mids_p);
            free(dcss_p);

            result = AT_OK;
        }

        break;
    }
    case AT_MODE_TEST: {
        sprintf((char *)info_text, ": (0)");
        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        result = AT_OK;
        break;
    }
    default:
        result = AT_ERROR;
    }

    return result;
} /* End of AT_PLUS_CSCB_Handle */

/*
 *===============================================================================
 *
 *   Function: AT_PLUS_CMGR_Handle
 *
 *   INPUT:   Parser   - Pointer to the current parser state.
 *            InfoText - Pointer to a string buffer to put information text.
 *            Message  - Pointer to the response message used for pending commands.
 *
 *   OUTPUT:  Parser->
 *              Pos    - The current position in the command line.
 *
 *   RETURNS: A status code for the execution of the command is returned:
 *            AT_OK        - The command was successfully handled.
 *            AT_ERROR     - There where a general error.
 *            AT_CME_ERROR - There was an GSM specific error
 *            AT_CMS_ERROR - There was an SMS specific error
 *            AT_PENDING   - The parser is exited before AT-command was completed
 *
 *   The CMGR command is used to read an SMS-PDU from memory
 *
 *   SET   - reads the PDU from memory.
 *   TEST  - Confirms the availability of the +CMGR command
 *
 *===============================================================================
 */
AT_Command_e AT_PLUS_CMGR_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message)
{
    bool ErrorFlag = false;
    exe_t *exe_p = NULL;
    exe_request_result_t  exe_result = EXE_SUCCESS;
    uint16_t index = 0;
    exe_cmgr_t req_data;
    AT_Command_e result = AT_OK;
    exe_cmgr_response_t *res_data_p = NULL;

    switch (parser_p->Mode) {
    case AT_MODE_SET: {

        if (!parser_p->IsPending) {
            index = (uint16_t)Parser_GetLongIntParam(parser_p, &ErrorFlag, NULL, 0xFFFF);

            if (ErrorFlag) {
                SET_RESULT_AND_GOTO_EXIT(AT_ERROR);
            }

            if (!parser_p->NoMoreParams) {
                Parser_SetCMS_ErrorType(parser_p, EXE_CMS_OPERATION_NOT_SUPPORTED);
                SET_RESULT_AND_GOTO_EXIT(AT_CMS_ERROR);
            }

            /*Presently, only SM storage is supported*/
            req_data.storage = MSG_STORAGE_SM;
            req_data.index = index;
            exe_p = atc_get_exe_handle();
            exe_result = exe_request(exe_p, EXE_CMGR, &req_data,
                                     (void *)parser_p, &parser_p->request_handle);

            if (EXE_PENDING != exe_result) {
                Parser_SetCMS_ErrorType(parser_p, EXE_CMS_OPERATION_NOT_ALLOWED);
                SET_RESULT_AND_GOTO_EXIT(AT_CMS_ERROR);
            }

            return AT_PENDING;
        } else {
            size_t converted_size = 0;
            res_data_p = (exe_cmgr_response_t *)message->data_p;

            if (message->exe_result_code != EXE_SUCCESS) {
                Parser_SetCMS_ErrorType(parser_p, res_data_p->error);
                SET_RESULT_AND_GOTO_EXIT(AT_CMS_ERROR);
            } else {
                uint8_t *pdu_p = calloc(1, 2 * res_data_p->total_length + 2);

                if (NULL == pdu_p) {
                    result = AT_ERROR;
                    goto exit;
                }

                atc_string_convert(ATC_CHARSET_UNDEF_BINARY, (uint8_t *)res_data_p->pdu_data_p, res_data_p->total_length,
                                   ATC_CHARSET_HEX, (uint8_t *)pdu_p, &converted_size);

                sprintf((char *)info_text, ": %d,,%d\r\n%s", res_data_p->status, res_data_p->tpdu_length, (char *)pdu_p);
                Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);

                free(pdu_p);
                return AT_OK;
            }
        }
    }
    break;

    case AT_MODE_TEST: {
        result = AT_OK;
        break;
    }

    default:
        Parser_SetCMS_ErrorType(parser_p, EXE_CMS_OPERATION_NOT_ALLOWED);
        SET_RESULT_AND_GOTO_EXIT(AT_CMS_ERROR);
        break;

    }

exit:
    return result;
}

/*
 *===============================================================================
 *
 *   Function: AT_PLUS_CGSMS_Handle
 *
 *   INPUT:   Parser   - Pointer to the current parser state.
 *            InfoText - Pointer to a string buffer to put information text.
 *            Message  - Pointer to the response message used for pending commands.
 *
 *   OUTPUT:  Parser->
 *              Pos    - The current position in the command line.
 *
 *   RETURNS: A status code for the execution of the command is returned:
 *            AT_OK        - The command was successfully handled.
 *            AT_ERROR     - There where a general error.
 *            AT_CME_ERROR - There was an GSM specific error
 *            AT_CMS_ERROR - There was an SMS specific error
 *            AT_PENDING   - The parser is exited before AT-command was completed
 *
 *   The CGSMS command is used to set the route (aka service) of SMS messages
 *
 *   SET   - Set the service.
 *   TEST  - Get the service currently active.
 *
 *===============================================================================
 */
AT_Command_e AT_PLUS_CGSMS_Handle(AT_ParserState_s *parser_p,
                                  AT_CommandLine_t info_text,
                                  AT_ResponseMessage_t message)
{
    bool ErrorFlag = false;
    exe_t *exe_p = NULL;
    exe_request_result_t exe_result = EXE_SUCCESS;
    AT_Command_e result = AT_OK;
    uint8_t service = CGSMS_SERVICE_UNKNOWN;
    exe_cgsms_request_t req_data;

    switch (parser_p->Mode) {
    case AT_MODE_SET: {

        if (!parser_p->IsPending) {
            service = (uint8_t)Parser_GetIntParam(parser_p, &ErrorFlag, &service);

            if (ErrorFlag) {
                SET_RESULT_AND_GOTO_EXIT(AT_ERROR);
            }

            /* reject if outside of range; PS preferred service not supported */
            if (CGSMS_SERVICE_MAX_VALUE < service || 2 == service) {
                SET_RESULT_AND_GOTO_EXIT(AT_ERROR);
            }

            if (!parser_p->NoMoreParams) {
                SET_RESULT_AND_GOTO_EXIT(AT_ERROR);
            }

            req_data.service = (exe_cgsms_service_t)service;

            exe_p = atc_get_exe_handle();
            exe_result = exe_request(exe_p, EXE_CGSMS_SET, &req_data,
                                     (void *)parser_p, &parser_p->request_handle);

            if (EXE_PENDING != exe_result) {
                SET_RESULT_AND_GOTO_EXIT(AT_ERROR);
            }

            return AT_PENDING;
        } else {
            exe_cgsms_response_t *res_data_p;

            res_data_p = (exe_cgsms_response_t *)message->data_p;

            if (message->exe_result_code != EXE_SUCCESS) {
                SET_RESULT_AND_GOTO_EXIT(AT_ERROR);
            }

            return AT_OK;
        }

        break;
    }
    case AT_MODE_READ: {
        if (!parser_p->IsPending) {

            exe_p = atc_get_exe_handle();
            exe_result = exe_request(exe_p, EXE_CGSMS_READ, NULL,
                                     (void *)parser_p, &parser_p->request_handle);

            if (EXE_PENDING != exe_result) {
                SET_RESULT_AND_GOTO_EXIT(AT_ERROR);
            }

            return AT_PENDING;
        } else {
            exe_cgsms_response_t *res_data_p;

            res_data_p = (exe_cgsms_response_t *)message->data_p;

            if (message->exe_result_code != EXE_SUCCESS) {
                SET_RESULT_AND_GOTO_EXIT(AT_ERROR);
            } else {
                sprintf((char *)info_text, ": %d", res_data_p->service);
                Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
                return AT_OK;
            }
        }

        break;
    }

    case AT_MODE_TEST: {
        sprintf((char *)info_text, ": (0,1,3)");
        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        result = AT_OK;
        break;
    }

    default:
        SET_RESULT_AND_GOTO_EXIT(AT_ERROR);
        break;

    }

exit:
    return result;
}

/*
 *===============================================================================
 *
 *   Function: AT_PLUS_CMSS_Handle
 *
 *   INPUT:   Parser   - Pointer to the current parser state.
 *            InfoText - Pointer to a string buffer to put information text.
 *            Message  - Pointer to the response message used for pending commands.
 *
 *   OUTPUT:  Parser->
 *              Pos    - The current position in the command line.
 *
 *   RETURNS: A status code for the execution of the command is returned:
 *            AT_OK        - The command was successfully handled.
 *            AT_ERROR     - There where a general error.
 *            AT_CME_ERROR - There was an GSM specific error
 *            AT_CMS_ERROR - There was an SMS specific error
 *            AT_PENDING   - The parser is exited before AT-command was completed
 *
 *   The CMSS command is used to send an SMS-PDU from memory
 *
 *   SET   - transmits a SMS from memory for given slot. If a new recipient is given
 *           the recipient in the SMS will be altered before transmission.
 *   TEST  - Confirms the availability of the +CMSS command
 *
 *===============================================================================
 */

AT_Command_e AT_PLUS_CMSS_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message)
{
    AT_Command_e result = AT_OK;

    switch (parser_p->Mode) {
    case AT_MODE_SET: {

        bool error_flag = false;
        exe_t *exe_p = NULL;
        exe_request_result_t  exe_result = EXE_SUCCESS;
        uint16_t index = 0;
        char *da_p = NULL;
        uint8_t toda = 0;
        exe_cmss_request_t req_data;
        exe_cmss_response_t *res_data_p = NULL;

        if (!parser_p->IsPending) {
            index = (uint16_t)Parser_GetLongIntParam(parser_p, &error_flag, NULL, 0xFFFF);

            if (error_flag) {
                Parser_SetCMS_ErrorType(parser_p, EXE_CMS_OPERATION_NOT_SUPPORTED);
                SET_RESULT_AND_GOTO_EXIT(AT_CMS_ERROR);
            }

            if (!parser_p->NoMoreParams) {

                da_p = (char *)Parser_GetStrParam(parser_p, &error_flag, NULL);

                if (error_flag) {
                    Parser_SetCMS_ErrorType(parser_p, EXE_CMS_OPERATION_NOT_SUPPORTED);
                    SET_RESULT_AND_GOTO_EXIT(AT_CMS_ERROR);
                }

                /* establish toda defaults as described in 3gpp 27.005 */
                if (da_p[0] == '+') {
                    toda = 145;
                } else {
                    toda = 129;
                }

                if (!parser_p->NoMoreParams) {
                    toda = Parser_GetIntParam(parser_p, &error_flag, &toda);

                    if (error_flag) {
                        Parser_SetCMS_ErrorType(parser_p, EXE_CMS_OPERATION_NOT_SUPPORTED);
                        SET_RESULT_AND_GOTO_EXIT(AT_CMS_ERROR);
                    }

                    if (!parser_p->NoMoreParams) {
                        Parser_SetCMS_ErrorType(parser_p, EXE_CMS_OPERATION_NOT_SUPPORTED);
                        SET_RESULT_AND_GOTO_EXIT(AT_CMS_ERROR);
                    }
                }
            }

            memset(&req_data, 0x00, sizeof(req_data));

            if (MSG_STORAGE_UNKNOWN != parser_p->cpms_msg_storage.mem2) {
                req_data.storage = parser_p->cpms_msg_storage.mem2;
            } else {
                req_data.storage = MSG_STORAGE_SM;
            }

            req_data.index = index;

            if (NULL != da_p) {
                req_data.da_p = da_p;
                req_data.toda = toda;
            }

            exe_p = atc_get_exe_handle();
            exe_result = exe_request(exe_p, EXE_CMSS, &req_data,
                                     (void *)parser_p, &parser_p->request_handle);

            if (EXE_PENDING != exe_result) {
                Parser_SetCMS_ErrorType(parser_p, EXE_CMS_OPERATION_NOT_ALLOWED);
                SET_RESULT_AND_GOTO_EXIT(AT_CMS_ERROR);
            }

            return AT_PENDING;
        } else {

            if (parser_p->abort) {
                exe_t *executor_p = atc_get_exe_handle();
                exe_request_result_t er = exe_request_abort(executor_p, parser_p->request_handle);
                return AT_ERROR;
            }

            if (NULL == message->data_p) {
                Parser_SetCMS_ErrorType(parser_p, EXE_CMS_UNKNOWN_ERROR);
                SET_RESULT_AND_GOTO_EXIT(AT_CMS_ERROR);
            }

            res_data_p = (exe_cmss_response_t *)message->data_p;

            if (EXE_SUCCESS != message->exe_result_code) {
                Parser_SetCMS_ErrorType(parser_p, res_data_p->error);
                SET_RESULT_AND_GOTO_EXIT(AT_CMS_ERROR);
            } else {
                sprintf((char *)info_text, ": %d", res_data_p->mr);

                Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
                return AT_OK;
            }
        }
    }
    break;

    case AT_MODE_TEST: {
        result = AT_OK;
        break;
    }

    default:
        Parser_SetCMS_ErrorType(parser_p, EXE_CMS_OPERATION_NOT_ALLOWED);
        SET_RESULT_AND_GOTO_EXIT(AT_CMS_ERROR);
        break;
    }

exit:
    return result;
}

/*
 *===============================================================================
 *
 *   Function: AT_PLUS_CMGL_Handle
 *
 *   INPUT:   Parser   - Pointer to the current parser state.
 *            InfoText - Pointer to a string buffer to put information text.
 *            Message  - Pointer to the response message used for pending commands.
 *
 *   OUTPUT:  Parser->
 *              Pos    - The current position in the command line.
 *
 *   RETURNS: A status code for the execution of the command is returned:
 *            AT_OK        - The command was successfully handled.
 *            AT_ERROR     - There where a general error.
 *            AT_CME_ERROR - There was an GSM specific error
 *            AT_CMS_ERROR - There was an SMS specific error
 *            AT_PENDING   - The parser is exited before AT-command was completed
 *
 *   The CMGL command is used to return a list of SMS-PDUs from memory
 *
 *   SET   - reads the PDU from memory.
 *   TEST  - Confirms the availability of the +CMGR command
 *
 *===============================================================================
 */
AT_Command_e AT_PLUS_CMGL_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message)
{
    bool ErrorFlag = false;
    exe_t *exe_p = NULL;
    exe_request_result_t  exe_result = EXE_SUCCESS;
    IntParam_t stat = EXE_CMGL_STATUS_REC_UNREAD;
    exe_cmgl_request_t req_data;
    AT_Command_e result = AT_OK;
    exe_cmgl_response_t *res_data_p = NULL;

    switch (parser_p->Mode) {
    case AT_MODE_DO:
    case AT_MODE_SET: {
        if (!parser_p->IsPending) {
            if (!parser_p->NoMoreParams) {
                stat = Parser_GetIntParam(parser_p, &ErrorFlag, &stat);

                if (ErrorFlag) {
                    SET_RESULT_AND_GOTO_EXIT(AT_ERROR);
                }

                if ((EXE_CMGL_STATUS_REC_UNREAD != stat) &&
                        (EXE_CMGL_STATUS_REC_READ != stat) &&
                        (EXE_CMGL_STATUS_STO_UNSENT != stat) &&
                        (EXE_CMGL_STATUS_STO_SENT != stat) &&
                        (EXE_CMGL_STATUS_ALL != stat)) {
                    ATC_LOG_E("AT_PLUS_CMGL_Handle: Invalid parameter value");
                    SET_RESULT_AND_GOTO_EXIT(AT_ERROR);
                }

                if (!parser_p->NoMoreParams) {
                    ATC_LOG_E("AT_PLUS_CMGL_Handle: Invalid parameter");
                    SET_RESULT_AND_GOTO_EXIT(AT_ERROR);
                }
            }

            req_data.storage = parser_p->cpms_msg_storage.mem1;
            req_data.position = EXE_MSG_POSITION_INVALID;
            req_data.browse = MSG_FIRST;
            req_data.status = stat;

            exe_p = atc_get_exe_handle();
            exe_result = exe_request(exe_p, EXE_CMGL, &req_data,
                                     (void *)parser_p, &parser_p->request_handle);

            if (EXE_PENDING != exe_result) {
                Parser_SetCMS_ErrorType(parser_p, EXE_CMS_OPERATION_NOT_ALLOWED);
                SET_RESULT_AND_GOTO_EXIT(AT_CMS_ERROR);
            }

            return AT_PENDING;
        } else {
            res_data_p = (exe_cmgl_response_t *)message->data_p;

            if (res_data_p != NULL) {

                /* Return AT_OK if no SMS is found. See 3GPP TS 27.005 chapter 3.2.5. */
                if (res_data_p->num_pdus == 0 && res_data_p->pdu_data_p == NULL) {
                    SET_RESULT_AND_GOTO_EXIT(AT_OK);
                }

                if (message->exe_result_code == EXE_SUCCESS && res_data_p->num_pdus > 0 && res_data_p->pdu_data_p != NULL) {
                    uint16_t count = 0;
                    exe_cmgl_response_pdu_data_t *pdu_data_p = res_data_p->pdu_data_p;
                    size_t converted_size = 0;
                    uint8_t *pdu_p = alloca(2 * CGML_MAX_PDU_LENGTH); /* Use max size to be able to handle all responses. */

                    if (NULL == pdu_p) {
                        result = AT_ERROR;
                        goto exit;
                    }

                    do {
                        count++;
                        memset(pdu_p, 0, 2 * CGML_MAX_PDU_LENGTH);
                        atc_string_convert(ATC_CHARSET_UNDEF_BINARY, (uint8_t *)pdu_data_p->pdu_p, pdu_data_p->total_length,
                                           ATC_CHARSET_HEX, (uint8_t *)pdu_p, &converted_size);

                        sprintf((char *)info_text, ": %d,%d,,%d\r\n%s",
                                pdu_data_p->position,
                                pdu_data_p->status,
                                pdu_data_p->tpdu_length,
                                (char *)pdu_p);

                        /* 3GPP TS 27.005 section 4.1 (List Messages +CMGL) states that there should
                         * be a CRLF before the second, and any subsequent, PDUs. The function
                         * Parser_SendMultipleResponses() automatically inserts CRLF, if required,
                         * so we do not it here. */
                        Parser_SendMultipleResponses(parser_p, AT_NO_OVERRIDE, info_text, (count == res_data_p->num_pdus) ? true : false);
                        pdu_data_p = pdu_data_p->next_p;
                    } while (pdu_data_p != NULL);

                    SET_RESULT_AND_GOTO_EXIT(AT_OK);
                } else {
                    Parser_SetCMS_ErrorType(parser_p, res_data_p->error);
                    SET_RESULT_AND_GOTO_EXIT(AT_CMS_ERROR);
                }
            } else {
                Parser_SetCMS_ErrorType(parser_p, EXE_CMS_MEMORY_FAILURE);
                SET_RESULT_AND_GOTO_EXIT(AT_CMS_ERROR);
            }
        }

        break;
    }

    case AT_MODE_TEST:
        sprintf((char *)info_text, ": (0,1,2,3,4)");
        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        result = AT_OK;
        break;

    default:
        Parser_SetCMS_ErrorType(parser_p, EXE_CMS_OPERATION_NOT_ALLOWED);
        SET_RESULT_AND_GOTO_EXIT(AT_CMS_ERROR);
        break;
    }

exit:
    return result;
}
