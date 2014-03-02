/**
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <string.h>
#include <stdio.h>
/* ATC related header files */
#include "atc_config.h"
#include "atc_exe_glue.h"
#include "atc_log.h"
#include "atc_handlers.h"
#include "atc_parser.h"
#include "atc_parser_util.h"
#include "exe.h"
#include "atc_string.h"

/*
 *===============================================================================
 *
 *   Function: AT_STAR_ECSPSAW_Handle
 *
 *   INPUT:   parser_p  - Pointer to the current parser state.
 *            info_text - Pointer to a string buffer to put information text.
 *
 *   OUTPUT:  parser_p->
 *              Pos    - The current position in the command line.
 *
 *   RETURNS: A status code for the execution of the command is returned:
 *            AT_OK        - The command was successfully handled.
 *            AT_ERROR     - There where a general error.
 *            AT_CME_ERROR - There where a GSM specific error, the error code is
 *                           returned in ASCII format in info_text.
 *            AT_PENDING   - The command is not ready but returns while
 *                           waiting for an asynchronous response.
 *
 *   SET  - Write cspsa data to storage area.
 *
 *   TEST - Test command to show if the command is supported.
 *
 *

 *===============================================================================
 */
AT_Command_e AT_STAR_ECSPSAW_Handle(AT_ParserState_s *parser_p,
                                    AT_CommandLine_t info_text,
                                    AT_ResponseMessage_t message)
{
    exe_cmee_error_t current_cmee = CMEE_OK;
    AT_Command_e result = AT_OK;
    bool error_flag = false;
    exe_request_result_t exe_res = EXE_FAILURE;
    exe_t *exe = NULL;
    StrParam_t value = NULL;
    exe_cspsa_data_t cspsa_data;
    uint32_t binary_size;
    size_t converted_size = 0;

    memset(&cspsa_data,0, sizeof(exe_cspsa_data_t));

    switch (parser_p->Mode) {
    case AT_MODE_SET: {
        cspsa_data.key = Parser_GetLongIntParam(parser_p, &error_flag, NULL, 0XFFFFFFFF);
        if (error_flag) {
            Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
            result = AT_CME_ERROR;
            goto error;
        }
        value = Parser_GetStrParam(parser_p, &error_flag, NULL);
        if (error_flag) {
            current_cmee = CMEE_INCORRECT_PARAMETERS;
            result = AT_CME_ERROR;
            goto error;
        }
        cspsa_data.storage = Parser_GetStrParam(parser_p, &error_flag, NULL);

        binary_size = (strlen(value) + 1) / 2;
        cspsa_data.data_len = binary_size;
        cspsa_data.value = calloc(1, binary_size);
        if(!cspsa_data.value){
            goto error;
        }
        atc_string_convert(ATC_CHARSET_HEX, (uint8_t *)value, strlen((char *) value),
                ATC_CHARSET_UNDEF_BINARY, (uint8_t *)(cspsa_data.value), &converted_size);
        /* If not all was converted, an error occurred  (probably odd number of input data)*/
        if (converted_size < binary_size) {
            current_cmee = CMEE_INCORRECT_PARAMETERS;
            result = AT_CME_ERROR;
            goto error;
        }

        exe = atc_get_exe_handle();
        /* cspsa synchronous call */
        exe_res = exe_request(exe, EXE_CSPSA_WRITE_DATA, (void *) &cspsa_data, (void *) parser_p,
                &parser_p->request_handle);
        free(cspsa_data.value);
        cspsa_data.value = NULL;
        result = parser_handle_exe_result(exe_res, parser_p);
        if (AT_OK != result) {
            goto error;
        }
        break;
    }

    case AT_MODE_TEST:
        result = AT_OK;
        break;

    default:
        current_cmee = CMEE_INCORRECT_PARAMETERS;
        result = AT_CME_ERROR;
        goto error;
        break;

    }
    return result;

error:
    free(cspsa_data.value);
    cspsa_data.value = NULL;
    if (current_cmee != CMEE_OK) {
        Parser_SetCMEE_ErrorType(parser_p, current_cmee);
        result = AT_CME_ERROR;
    }
    return result;
}

/*
 *===============================================================================
 *
 *   Function: AT_STAR_ECSPSAR_Handle
 *
 *   INPUT:   parser_p  - Pointer to the current parser state.
 *            info_text - Pointer to a string buffer to put information text.
 *
 *   OUTPUT:  parser_p->
 *              Pos    - The current position in the command line.
 *
 *   RETURNS: A status code for the execution of the command is returned:
 *            AT_OK        - The command was successfully handled.
 *            AT_ERROR     - There where a general error.
 *            AT_CME_ERROR - There where a GSM specific error, the error code is
 *                           returned in ASCII format in info_text.
 *            AT_PENDING   - The command is not ready but returns while
 *                           waiting for an asynchronous response.
 *
 *   SET  - Read cspsa data from storage area.
 *
 *   TEST - Test command to show if the command is supported.
 *
 *

 *===============================================================================
 */
AT_Command_e AT_STAR_ECSPSAR_Handle(AT_ParserState_s *parser_p,
                                    AT_CommandLine_t info_text,
                                    AT_ResponseMessage_t message)
{
    exe_cmee_error_t current_cmee = CMEE_OK;
    AT_Command_e result = AT_OK;
    bool error_flag = false;
    exe_request_result_t exe_res = EXE_FAILURE;
    exe_t *exe = NULL;
    StrParam_t value = NULL;
    exe_cspsa_data_t cspsa_data;
    char *hex_str_p = NULL;
    char *hex_tmp_p = NULL;
    char *format_str_p = NULL;
    uint16_t total_hex_str_len;
    uint16_t max_rsp_buf_size;
    uint16_t remaining_hex_len;
    size_t converted_size = 0;

    memset(&cspsa_data,0, sizeof(exe_cspsa_data_t));

    switch (parser_p->Mode) {
    case AT_MODE_SET: {
        cspsa_data.key = Parser_GetLongIntParam(parser_p, &error_flag, NULL, 0XFFFFFFFF);
        if (error_flag) {
            Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
            result = AT_CME_ERROR;
            goto error;
        }
        cspsa_data.storage = Parser_GetStrParam(parser_p, &error_flag, NULL);

        exe = atc_get_exe_handle();
        /* cspsa synchronous call */
        exe_res = exe_request(exe, EXE_CSPSA_READ_DATA, (void *) &cspsa_data, (void *) parser_p,
                &parser_p->request_handle);
        result = parser_handle_exe_result(exe_res, parser_p);
        if (AT_OK != result) {
            goto error;
        }

        /* Allocate the double size for ASCII format plus 1 for null termination */
        total_hex_str_len = cspsa_data.data_len * 2 + 1;
        hex_str_p = (char *) calloc(1, total_hex_str_len);
        if(!hex_str_p){
            ATC_LOG_E("memory allocation error!");
            goto error;
        }
        atc_string_convert(ATC_CHARSET_UNDEF_BINARY, cspsa_data.value, cspsa_data.data_len,
                           ATC_CHARSET_HEX, (uint8_t *)hex_str_p, &converted_size);

        /* Since the maximum response buffer is limited, the resulting hex string will
         *  have to be split up in several prints if it is larger that the maximum buffer */
        hex_tmp_p = hex_str_p;
        /* Max buffer is total buffer - length of "*ECSPSAR: " + null termination */
        max_rsp_buf_size = RESULT_TEXT_SIZE - (10 + 1);
        remaining_hex_len = total_hex_str_len;
        /* Construct a format string to specify maximum number of characters to be printed */
        asprintf(&format_str_p, ": %%.%ds", max_rsp_buf_size);
        while (remaining_hex_len > max_rsp_buf_size) {
            sprintf((char *) info_text, format_str_p, hex_tmp_p);
            hex_tmp_p += max_rsp_buf_size;
            Parser_SendMultipleResponses(parser_p, AT_NO_OVERRIDE, info_text, false);
            remaining_hex_len -= max_rsp_buf_size;
        }
        sprintf((char *) info_text, ": %s", hex_tmp_p);
        Parser_SendMultipleResponses(parser_p, AT_NO_OVERRIDE, info_text, true);
        free(format_str_p);
        free(hex_str_p);
        break;
    }

    case AT_MODE_TEST:
        result = AT_OK;
        break;

    default:
        Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
        result = AT_CME_ERROR;
        goto error;
        break;

    }

error:
    free(cspsa_data.value);
    return result;
}
