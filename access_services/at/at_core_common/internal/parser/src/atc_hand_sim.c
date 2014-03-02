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
#include <atc_string.h>

#include <exe.h>


#ifdef EXE_USE_ATC_CUSTOMER_EXTENSIONS
#include <atc_extended_handlers.h>
#endif

#define AT_SIM_MAX_FILE_ID 0xFFFF
#define AT_SIM_EFSPN_FILE_ID 0x6F46
#define HIGHEST_VALID_RECORD_NUMBER 254
#define MIN_NUMERIC_STRING_LENGTH 5

/* Define CNUM values */
#define CNUM_ALPHA_LEN 241
#define CNUM_NUMBER_LEN 20
#define CNUM_SUPLUS_SIGNS_LEN 20 /* : "","",<type=1 digit>,<speed=3 digits>,<service=1 digit>,<itc=1 digit><CR><LF>'\0' = 20 chars! */

/* Define CPIN codes.*/
#define CPIN_BLOCKED        "BLOCKED"
#define CPIN_NO_CODE        "Internal error"
#define CPIN_PH_CORP_PIN    "PH-CORP PIN"
#define CPIN_PH_NET_PIN     "PH-NET PIN"
#define CPIN_PH_NETSUB_PIN  "PH-NETSUB PIN"
#define CPIN_PH_SIMLOCK_PIN "PH-SIMLOCK PIN"
#define CPIN_PH_SIM_PIN     "PH-SIM PIN"
#define CPIN_PH_SP_PIN      "PH-SP PIN"
#define CPIN_PH_ESL_PIN     "PH-ESL PIN"
#define CPIN_READY          "READY"
#define CPIN_SIM_PIN2       "SIM PIN2"
#define CPIN_SIM_PIN        "SIM PIN"
#define CPIN_SIM_PUK2       "SIM PUK2"
#define CPIN_SIM_PUK        "SIM PUK"

typedef enum {
    ATC_READ_BINARY    = 176,
    ATC_READ_RECORD    = 178,
    ATC_GET_RESPONSE   = 192,
    ATC_UPDATE_BINARY  = 214,
    ATC_UPDATE_RECORD  = 220,
    ATC_STATUS         = 242
} sim_valid_crsm_command_t;

typedef enum {
    ATC_RECORD_MODE_NEXT     = 2,
    ATC_RECORD_MODE_PREVIOUS = 3,
    ATC_RECORD_MODE_ABSOLUTE = 4
} sim_record_modes_t;

static bool is_hex(const char c)
{
    if (is_digit(c) || (to_upper(c) >= 'A' && to_upper(c) <= 'F')) {
        return true;
    }

    return false;
}

static uint8_t get_hex(const char c)
{
    if (is_digit(c)) {
        return (uint8_t)(c - '0');
    } else if (to_upper(c) >= 'A' && to_upper(c) <= 'F') {
        return (uint8_t)(to_upper(c) - 'A' + 10);
    } else {
        return (uint8_t) 0xff;
    }
}

bool convert_ascii_hex_to_bin_buf(const char *const data_p,
                                  uint8_t *const bin_buf_p,
                                  const uint16_t length /* of bin_buf*/)
{
    uint16_t i;

    char c_1, c_2;

    for (i = 0; i < length; i++) {
        c_1 = (data_p[i * 2]);
        c_2 = (data_p[i * 2 + 1]);

        if (!is_hex(c_1) || !is_hex(c_2)) {
            return false;
        }

        bin_buf_p[i] = (uint8_t)(get_hex(c_1) << 4) + get_hex(c_2);
    }

    return true;
}

bool service_availability_check_required(uint16_t sim_file_id)
{
   if (AT_SIM_EFSPN_FILE_ID == sim_file_id )
   {
      return true;
   }
   return false;
}


AT_Command_e AT_PLUS_CRSM_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message)
{
    sim_valid_crsm_command_t sim_command;
    uint16_t sim_file_id = AT_SIM_MAX_FILE_ID;
    uint8_t p_1 = 0xFF;
    uint8_t p_2 = 0xFF;
    uint8_t p_3 = 0xFF;
    bool error_flag = false;
    uint8_t *update_data_p = NULL;
    StrParam_t sim_data_p;
    StrParam_t path_id_p = NULL;
    exe_cmee_error_t cmee_error_type = CMEE_UNKNOWN;
    exe_t *exe_p = NULL;

    if (!parser_p->IsPending) {
        if (AT_MODE_SET != parser_p->Mode && AT_MODE_TEST != parser_p->Mode) {
            return AT_ERROR;
        }

        if (AT_MODE_TEST == parser_p->Mode) {
            return AT_OK;
        }

        sim_command = (sim_valid_crsm_command_t) Parser_GetLongIntParam(parser_p,
                      &error_flag, NULL, 0xFFFF);
        sim_file_id = (uint16_t) Parser_GetLongIntParam(parser_p, &error_flag, (long unsigned int *)&sim_file_id,
                      AT_SIM_MAX_FILE_ID);
        p_1 = Parser_GetIntParam(parser_p, &error_flag, NULL);
        p_2 = Parser_GetIntParam(parser_p, &error_flag, NULL);
        p_3 = Parser_GetIntParam(parser_p, &error_flag, NULL);

        if ((ATC_STATUS != sim_command) && (AT_SIM_MAX_FILE_ID == sim_file_id)) {
            Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
            return AT_CME_ERROR;
        }

        /* READ_BINARY/RECORD have all five parameters above as mandatory  */
        if ((ATC_READ_BINARY == sim_command) || (ATC_READ_RECORD == sim_command)) {
            if (error_flag) { /* Missing or incorrect parameters */
                Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
                return AT_CME_ERROR;
            }
        }

        /* Get the <data> parameter */
        sim_data_p = Parser_GetStrParam(parser_p, &error_flag, NULL); /* No default */

        if ((ATC_UPDATE_BINARY == sim_command) || (ATC_UPDATE_RECORD == sim_command)) {
            /* <data> is mandatory for UPDATE_BINARY/RECORD */
            if (error_flag || (uint16_t) strlen(sim_data_p) != p_3 * 2) {
                exe_cmee_error_t Code = CMEE_INCORRECT_PARAMETERS;

                if (!error_flag && (uint16_t) strlen(sim_data_p) > p_3 * 2) {
                    Code = CMEE_TEXT_STRING_TOO_LONG;
                }

                Parser_SetCMEE_ErrorType(parser_p, Code);
                return AT_CME_ERROR;
            }
        } else {
            /* <data> should not be specified for other
               commands (i.e. ErrorFlag shall be true here).
               To specify <pathid>, <data> should be skipped,
               i.e. not set to anything.
               Example: AT+CRSM=176,20272,0,0,1,,"7F205F70" */
            if (!error_flag || NULL != sim_data_p) {
                Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
                return AT_CME_ERROR;
            }
        }

        /* Get the optional <pathid> parameter */
        if (parser_p->NoMoreParams) {
            /* <fileid> or <pathid> is mandatory for ATC_STATUS */
            if ((ATC_STATUS == sim_command) && (AT_SIM_MAX_FILE_ID == sim_file_id)) {
                Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
                return AT_CME_ERROR;
            }

            cmee_error_type = CMEE_OK;
        } else {
            error_flag = false;
            path_id_p = Parser_GetStrParam(parser_p, &error_flag, (void *) NULL);

            if (error_flag) {
                Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
                return AT_CME_ERROR;
            }
        }


        /* AT_STRING_PARAM_BOUNDER */
        if (!parser_p->NoMoreParams) {
            /* Don't accept more parameters than up to <pathid> */
            Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
            return AT_CME_ERROR;
        }

        /* Test the <mode> parameter when ATC_UPDATE_RECORD and ATC_READ_RECORD
         * Only ATC_RECORD_MODE_ABSOLUTE mode allowed */
        if ((ATC_READ_RECORD == sim_command) || (ATC_UPDATE_RECORD == sim_command)) {
            if ((ATC_RECORD_MODE_ABSOLUTE != p_2)) {
                Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
                return AT_CME_ERROR;
            }
        }

        /* We need to convert the string to binary data (uint8_t).
         * 2 char signs hex represent one binary value)  */
        if ((ATC_UPDATE_BINARY == sim_command) || (ATC_UPDATE_RECORD == sim_command)) {
            int i = 0;
            update_data_p = alloca(p_3);
            convert_ascii_hex_to_bin_buf((char *)sim_data_p, update_data_p, (uint16_t)p_3);
        }

        /* Check p_3 (length) parameter. If it is 0 then set it to 255  */
        if (0 == p_3) {
            p_3 = 255;
        }

        exe_p = atc_get_exe_handle();
        exe_sim_io_t sim_io_data;
        exe_request_result_t result = EXE_FAILURE;
        exe_request_id_t request_id;

        sim_io_data.file_id = sim_file_id;
        sim_io_data.path_p = (char *)path_id_p;
        sim_io_data.p_1 = p_1;
        sim_io_data.p_2 = p_2;
        sim_io_data.p_3 = p_3;
        sim_io_data.data_p = NULL;
        sim_io_data.info_type = EXE_CRSM_RESPONSE_INFO;

        /* Clear data in the struct that will be returned from exe_p */
        /* sim_io_data.sim_io_response = NULL; */
        sim_io_data.sim_io_response_p = NULL;
        sim_io_data.length = 0;
        sim_io_data.sw_1 = 0;
        sim_io_data.sw_2 = 0;
        sim_io_data.error_codes = CMEE_OK;

        switch (sim_command) {
        case ATC_READ_BINARY: { /* 176   0xB0 */
             request_id = EXE_CRSM_SIM_IO_READ_BINARY;
             if (service_availability_check_required(sim_file_id))
                {
                 request_id = EXE_CRSM_CHECK_SERVICE_SIM_IO_READ_BINARY;
                }
                break;
             }
        case ATC_READ_RECORD: { /* 178   0xB2 */
            request_id = EXE_CRSM_SIM_IO_READ_RECORD;
            break;
        }
        case ATC_UPDATE_BINARY: { /* 214   0xD6 */
            request_id = EXE_CRSM_SIM_IO_UPDATE_BINARY;
            sim_io_data.data_p = (char *)update_data_p;
            break;
        }
        case ATC_UPDATE_RECORD: { /* 220   0xDC */
            request_id = EXE_CRSM_SIM_IO_UPDATE_RECORD;
            sim_io_data.data_p = (char *)update_data_p;
            break;
        }
        case ATC_STATUS: { /* 242   0xF2 */
            request_id = EXE_CRSM_SIM_IO_STATUS;
            sim_io_data.info_type = EXE_CRSM_STATUS_INFO;
            break;
        }
        case ATC_GET_RESPONSE: { /* 192  0xC0 */
            request_id = EXE_CRSM_SIM_IO_GET_RESPONSE;
            sim_io_data.info_type = EXE_CRSM_RESPONSE_INFO;
            break;
        }
        default:
            Parser_SetCMEE_ErrorType(parser_p, CMEE_OPERATION_NOT_ALLOWED);
            return AT_CME_ERROR;
            /* break; */
        }

        result = exe_request(exe_p, request_id, (void *)&sim_io_data,
                             (void *)parser_p, &parser_p->request_handle);

        if (EXE_PENDING == result) {
            return AT_PENDING;
        } else {
            Parser_SetCMEE_ErrorType(parser_p, CMEE_OPERATION_NOT_ALLOWED);
            return AT_CME_ERROR;
        }
    } else { /* IsPending handling*/
        switch (parser_p->Mode) {
        case AT_MODE_SET: {
            exe_sim_io_t *sim_io_data_p = (exe_sim_io_t *)(message->data_p);

            if (NULL == sim_io_data_p) {
                ATC_LOG_E("sim_io_data_p is NULL");
                goto error;
            }

            if (CMEE_OK != sim_io_data_p->error_codes) {
                Parser_SetCMEE_ErrorType(parser_p, CMEE_OPERATION_NOT_ALLOWED);
                return AT_CME_ERROR;
            }

            if (NULL == sim_io_data_p->sim_io_response_p) {
                sprintf((char *) info_text, ": %u,%u", sim_io_data_p->sw_1, sim_io_data_p->sw_2);
                Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
            } else {
                int i;
                char tmp_buffer[20];

                sprintf((char *)info_text, ": %u,%u,\"", sim_io_data_p->sw_1, sim_io_data_p->sw_2);

                if ((strlen((char *)info_text) + (2 * sim_io_data_p->length) + 1) > RESULT_TEXT_SIZE) {
                    ATC_LOG_E("The AT response buffer is too small. Needed length = %lu, bin length = %d",
                              (strlen((char *)info_text) + (2 * sim_io_data_p->length) + 1), sim_io_data_p->length);
                    Parser_SetCMEE_ErrorType(parser_p, CMEE_PHONE_FAILURE);
                    return AT_CME_ERROR;
                }

                for (i = 0; i < sim_io_data_p->length; i++) {
                    tmp_buffer[0] = 0;
                    sprintf((char *)tmp_buffer, "%02X", sim_io_data_p->sim_io_response_p[i]);
                    strcat((char *)info_text, tmp_buffer);
                }

                strcat((char *)info_text, "\"");


                Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
            }

            break;
        }
        default: {
            break;
        }
        }
    }

    return AT_OK;
error:
    return AT_ERROR;
} /* AT_PLUS_CRSM_Handle */

/*
 *===============================================================================
 *
 *   Function: AT_PLUS_CIMI_Handle
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
 *   The CIMI command is used for reading the IMSI number
 *
 *   DO   - Reads the IMSI number
 *   TEST - Confirms that the command is implemented
 *
 *===============================================================================
 */
AT_Command_e AT_PLUS_CIMI_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message)
{
    exe_cmee_error_t cmee_result = CMEE_OK;
    exe_request_result_t result = EXE_FAILURE;
    exe_t *exe_p = NULL;
    exe_p = atc_get_exe_handle();

    if (!parser_p->IsPending) {
        switch (parser_p->Mode) {
        case AT_MODE_DO: {
            result = exe_request(exe_p, EXE_CIMI, NULL, (void *)parser_p, &parser_p->request_handle);

            if (result == EXE_PENDING) {
                return AT_PENDING;
            } else {
                cmee_result = CMEE_SIM_FAILURE;
            }

            break;
        }
        case AT_MODE_TEST:
            break;
        }
    } else {
        switch (parser_p->Mode) {
        case AT_MODE_DO: {
            if (message->exe_result_code == EXE_SUCCESS) {
                if (message->data_p != NULL) {
                    exe_imsi_t *imsi_buf_p = (exe_imsi_t *)(message->data_p);

                    if (CMEE_OK == imsi_buf_p->error_code) {
                        sprintf((char *)info_text, "%s", imsi_buf_p->imsi_p);
                    } else {
                        cmee_result = imsi_buf_p->error_code;  /* Error code from exe SIM parts */
                        break;
                    }
                } else {
                    cmee_result = CMEE_SIM_FAILURE;
                    break;
                }

                Parser_SendResponse(parser_p, AT_TEXT_RESPONSE, info_text);
                return AT_OK;
            } else {
                cmee_result = CMEE_SIM_FAILURE;
            }

            break;
        }
        default:
            cmee_result = CMEE_OPERATION_NOT_ALLOWED;
            break;
        }
    }

    if (cmee_result == CMEE_OK) {
        return AT_OK;
    }

    Parser_SetCMEE_ErrorType(parser_p, cmee_result);
    return AT_CME_ERROR;
} /* End of AT_PLUS_CIMI_Handle */

/*
 *=====================================================================
 *
 *  Function:     cpin_get_code_tag
 *
 *  Input:        buffer  - Pointer to empty string buffer for the result.
 *                code   - Index to the tag.
 *
 *  Output:       buffer  - String tag associated with state.
 *
 *  Description:  The writes a string type tag associated to 'code' into the buffer
 *                pointed at by 'buffer'.
 *
 * ==================================================================
 */
static void cpin_get_code_tag(char *buffer_p, exe_cpin_code_t code)
{

    switch (code) {
    case EXE_CPIN_CODE_READY:
        strcpy(buffer_p, CPIN_READY);
        break;
    case EXE_CPIN_CODE_SIM_PIN:
        strcpy(buffer_p, CPIN_SIM_PIN);
        break;
    case EXE_CPIN_CODE_SIM_PUK:
        strcpy(buffer_p, CPIN_SIM_PUK);
        break;
    case EXE_CPIN_CODE_PH_SIM_PIN:
        strcpy(buffer_p, CPIN_PH_SIM_PIN);
        break;
    case EXE_CPIN_CODE_SIM_PIN2:
        strcpy(buffer_p, CPIN_SIM_PIN2);
        break;
    case EXE_CPIN_CODE_SIM_PUK2:
        strcpy(buffer_p, CPIN_SIM_PUK2);
        break;
    case EXE_CPIN_CODE_PH_NET_PIN:
        strcpy(buffer_p, CPIN_PH_NET_PIN);
        break;
    case EXE_CPIN_CODE_PH_NETSUB_PIN:
        strcpy(buffer_p, CPIN_PH_NETSUB_PIN);
        break;
    case EXE_CPIN_CODE_PH_SP_PIN:
        strcpy(buffer_p, CPIN_PH_SP_PIN);
        break;
    case EXE_CPIN_CODE_PH_CORP_PIN:
        strcpy(buffer_p, CPIN_PH_CORP_PIN);
        break;
    case EXE_CPIN_CODE_PH_SIMLOCK_PIN:
        strcpy(buffer_p, CPIN_PH_SIMLOCK_PIN);
        break;
    case EXE_CPIN_CODE_PH_ESL_PIN:
        strcpy(buffer_p, CPIN_PH_ESL_PIN);
        break;
    case EXE_CPIN_CODE_BLOCKED:
        strcpy(buffer_p, CPIN_BLOCKED);
        break;
    case EXE_CPIN_CODE_NO_CODE:
    default:
        strcpy(buffer_p, CPIN_NO_CODE);
        break;
    }

} /* End of cpin_get_code_tag */

/*
 *=====================================================================
 *
 *  Function:     cpin_get_code_value
 *
 *  Input:        code - Pointer to code string tag.
 *
 *  Returns:      Index to the tag; EXE_CPIN_CODE_NO_CODE if not
 *                found or not supported.
 *
 *  Description:  Returns index value associated with string tag.
 *
 * ==================================================================
 */
static exe_cpin_code_t cpin_get_code_value(const StrParam_t code)
{
    exe_cpin_code_t value = EXE_CPIN_CODE_NO_CODE;

    if (strncmp(code, CPIN_READY, 5) == 0) {
        value = EXE_CPIN_CODE_NO_CODE;
    } else if (strncmp(code, CPIN_PH_SIM_PIN, 10) == 0) {
        value = EXE_CPIN_CODE_PH_SIM_PIN;
    } else if (strncmp(code, CPIN_SIM_PIN2, 8) == 0) {
        value = EXE_CPIN_CODE_SIM_PIN2;
    } else if (strncmp(code, CPIN_SIM_PUK2, 8) == 0) {
        value = EXE_CPIN_CODE_SIM_PUK2;
    } else if (strncmp(code, CPIN_SIM_PIN, 7) == 0) {
        value = EXE_CPIN_CODE_SIM_PIN;
    } else if (strncmp(code, CPIN_SIM_PUK, 7) == 0) {
        value = EXE_CPIN_CODE_SIM_PUK;
    } else if (strncmp(code, CPIN_PH_NET_PIN, 10) == 0) {
        value = EXE_CPIN_CODE_PH_NET_PIN;
    } else if (strncmp(code, CPIN_PH_NETSUB_PIN, 13) == 0) {
        value = EXE_CPIN_CODE_PH_NETSUB_PIN;
    } else if (strncmp(code, CPIN_PH_SP_PIN, 9) == 0) {
        value = EXE_CPIN_CODE_PH_SP_PIN;
    } else if (strncmp(code, CPIN_PH_CORP_PIN, 11) == 0) {
        value = EXE_CPIN_CODE_PH_CORP_PIN;
    } else if (strncmp(code, CPIN_PH_SIMLOCK_PIN, 14) == 0) {
        value = EXE_CPIN_CODE_PH_SIMLOCK_PIN;
    } else if (strncmp(code, CPIN_PH_ESL_PIN, 10) == 0) {
        value = EXE_CPIN_CODE_PH_ESL_PIN;
    } else if (strncmp(code, CPIN_BLOCKED, 7) == 0) {
        value = EXE_CPIN_CODE_NO_CODE;
    }

    ATC_LOG_D("code %d (%s) parsed", value, code);

    return value;
} /* End of cpin_get_code_value */

/*
 *=====================================================================
 *
 *  Function:     ecexpin_query_app_state
 *
 *  Input:        parser_p - Pointer to the current parser state.
 *                exe_p    - Pointer to executor object
 *                app_id   - Pointer to application identifier string.
 *
 *  Returns:      Executor result.
 *
 *  Description:  Executes state request for given sim application id.
 *                If no app_id is present the status for the telecom
 *                application is requested.
 *
 * ==================================================================
 */
exe_request_result_t ecexpin_query_app_state(AT_ParserState_s *parser_p, exe_t *exe_p, StrParam_t app_id)
{
    exe_request_result_t result = EXE_FAILURE;

    if (app_id) {
        /* TODO: Make use of other executor when that patch is delivered. */
        result = exe_request(exe_p, EXE_CPIN_GET_SIM_STATE, app_id,
                             (void *)parser_p, &parser_p->request_handle);
    } else { /* Get telecom app status */
        result = exe_request(exe_p, EXE_CPIN_GET_SIM_STATE, NULL,
                             (void *)parser_p, &parser_p->request_handle);
    }

    return result;
}

/*
 *===============================================================================
 *
 *   Function: AT_PLUS_CPIN_Handle
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
 *   The CPIN command is used for sending passwords to the ME and for checking
 *   the currently required passwords.
 *   The CPIN-handler is extended and used by the ECEXPIN command which is not
 *   state dependent as the CPIN command.
 *
 *   SET  - CPIN:
 *          Sends the currently required password to the ME:
 *          +CPIN=<pin>[,<newpin>]
 *            <pin> and  <newpin> are numeric string type values.
 *            The range for the SIM PIN and the PH-SIM PIN is 4 - 8 digits.
 *            The SIM PUK consists of 8 digits.
 *        - ECEXPIN:
 *          Sends password for wanted facility indicated by <code>.
 *          Not state dependent. Eg. "SIM PUK" can be entered in "READY" state.
 *          *ECEXPIN=<mode>,[<appId>],<code>,<pin>[,<newpin>]
 *            <mode> is an integer value. 1 for set password and 2 for query state.
 *            <appId> is a string identifying an application on sim to target.
 *            <code> identifies which password to set.
 *            <pin> and <newpin> has same spec as CPIN.
 *
 *   READ - Returns the currently required passwords for CPIN. NA for ECEXPIN.
 *          +CPIN: <code>
 *            "READY"      ME has no pending request for any password.
 *            "SIM PIN"    ME requires a SIM PIN to be entered.
 *            "SIM PUK"    ME requires a SIM PUK to be entered.
 *            "SIM PIN2"   ME requires a SIM PIN2 to be entered.
 *            "SIM PUK2"   ME requires a SIM PUK2 to be entered.
 *            "PH-SIM PIN" ME requires a Phone -to-SIM card password to be entered
 *                         (phone lock).
 *            "PH-NET PIN" ME requires a network personalization password
 *                         to be entered.
 *            "PH-NETSUB PIN" ME requires a network subset personalization
 *                         password to be entered.
 *            "PH-SP PIN"  ME requires a service provider personalization
 *                         password to be entered.
 *            "PH-CORP PIN" ME requires a corporate personalization
 *                         password to be entered.
 *            "PH-SIMLOCK PIN" ME requires a SIM lock password
 *                         to be entered.
 *            "PH-ESL PIN" ME requires a extended SIM lock password
 *                         to be entered. Ericsson specific.
 *
 *   TEST - Writes the supported password types to info_text:
 *          +CPIN: <code1>,[<code2,[...]]
 *          +ECEXPIN: <code1>,[<code2,[...]]
 *
 *===============================================================================
 */
#ifdef AT_PLUS_CPIN_ENABLED
AT_Command_e AT_PLUS_CPIN_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message)
{
    StrParam_t empty_string = ""; /* Default strings must be on the stack. */
    IntParam_t mode = 0; /* Reference to the <mode> parameter from AT*ECEXPIN. */
    StrParam_t app_id = NULL; /* Reference to the <appId> parameter from AT*ECEXPIN. */
    StrParam_t code = empty_string; /* Reference to the <code> parameter from AT*ECEXPIN. */
    StrParam_t pin_code = empty_string; /* Reference to the <pin> parameter. */
    StrParam_t new_pin = empty_string; /* Reference to the <newpin> parameter. */
    bool error_flag = false; /* Flags a parameter parsing error. */
    exe_cops_facility_personalization_data_t cops_data;
    exe_sim_verify_codes_t code_data;
    exe_request_id_t request_id = EXE_LAST;
    exe_t *exe_p;
    exe_request_result_t result = EXE_SUCCESS;
    size_t max_pin_test_length = 17; /* MAX_SIZE_SIMLOCK_CONTROL_KEY + 1 */
    exe_p = atc_get_exe_handle();
    exe_cmee_error_t current_cmee = CMEE_OK; /* GSM specific error code. */
    exe_cpin_sim_get_state_t *sim_state_p = NULL;

    switch (parser_p->Mode) {
    case AT_MODE_SET: {
        if (!parser_p->IsPending) {
            parser_p->sub_state = EXE_CPIN_RESP_STATE_NORMAL;

            if (parser_p->Command == AT_PLUS_CPIN) {
                /* We have to ask for the current state
                 * before we are able to make the correct
                 * checks in AT_MODE_SET
                 */
                result = exe_request(exe_p, EXE_CPIN_GET_SIM_STATE, NULL,
                                     (void *)parser_p, &parser_p->request_handle);

                if (EXE_PENDING != result) {
                    current_cmee = CMEE_SIM_FAILURE;
                    goto end;
                }

                parser_p->cpin_code = EXE_CPIN_CODE_NO_CODE; /* No code has been found yet */
                parser_p->get_sim_state = true; /* Indicate that we have requested sim status */
                return AT_PENDING;
            } else if (parser_p->Command == AT_STAR_ECEXPIN) {
                /* ECEXPIN - bypass statehandling*/
                parser_p->cpin_code = EXE_CPIN_CODE_NO_CODE;
                parser_p->get_sim_state = true;
                parser_p->IsPending = true;
            } else {
                /* Unsupported command */
                return AT_ERROR;
            }
        }

        if (parser_p->IsPending) {
            if (parser_p->get_sim_state) {

                if (parser_p->Command == AT_PLUS_CPIN) {
                    /*
                     * We got a response to get sim state
                     */

                    /* Check that we got a valid response on sim state question */
                    if (EXE_SUCCESS != message->exe_result_code || NULL == message->data_p) {
                        current_cmee = CMEE_SIM_FAILURE;
                        goto end;
                    }

                    sim_state_p = message->data_p;

                    parser_p->cpin_code = sim_state_p->error_code;

                    switch (sim_state_p->error_code) {
                    case CMEE_OK:
                    case CMEE_SIM_PIN_REQUIRED:
                    case CMEE_SIM_PIN2_REQUIRED:
                    case CMEE_SIM_PUK_REQUIRED:
                    case CMEE_SIM_PUK2_REQUIRED:
                    case CMEE_PH_NET_PIN_REQUIRED:
                    case CMEE_PH_NETSUB_PIN_REQUIRED:
                    case CMEE_PH_SP_PIN_REQUIRED:
                    case CMEE_PH_CORP_PIN_REQUIRED:
                    case CMEE_PH_SIMLOCK_PIN_REQUIRED:
                    case CMEE_PH_NETSUB_PUK_REQUIRED:
                    case CMEE_PH_SP_PUK_REQUIRED:
                    case CMEE_PH_CORP_PUK_REQUIRED:
                        current_cmee = CMEE_OK;
                        break;
                    default:
                        current_cmee = sim_state_p->error_code;
                        goto end;
                    }
                } /* End if (parser_p->Command == AT_PLUS_CPIN) */

                /* Indicate that we have not requested/have an answer to sim status, so the next
                 * response must be the reply to verify PIN/PUK.
                 */
                parser_p->get_sim_state = false;

                /* ErrorFlag is set true when:
                   no default value was given and the parameter was left out
                   or if there was an error in the parameter syntax. */
                if (parser_p->Command == AT_PLUS_CPIN) {
                    pin_code = Parser_GetStrParam(parser_p, &error_flag, NULL);

                    if (error_flag) {
                        goto check_error_flag;
                    }

                    new_pin = Parser_GetStrParam(parser_p, &error_flag, empty_string);
                } else { /* AT_STAR_ECEXPIN */
                    mode = Parser_GetIntParam(parser_p, &error_flag, NULL);

                    if (error_flag) {
                        goto check_error_flag;
                    }

                    if (mode != 1 && mode != 2) {
                        ATC_LOG_D("mode param faulty");
                        current_cmee = CMEE_INCORRECT_PARAMETERS;
                        goto end;
                    }

                    app_id = Parser_GetStrParam(parser_p, &error_flag, empty_string);

                    app_id = strcmp(app_id, empty_string) == 0 ? NULL : app_id;

                    if (error_flag) {
                        goto check_error_flag;
                    }

                    if (mode == 2) {
                        /* Query state - no more params after appId */
                        ATC_LOG_D("*ECEXPIN params: mode:%d, app_id:%s", mode, app_id ? app_id : empty_string);
                        goto check_error_flag;
                    }

                    code = Parser_GetStrParam(parser_p, &error_flag, NULL);

                    if (error_flag) {
                        goto check_error_flag;
                    }

                    pin_code = Parser_GetStrParam(parser_p, &error_flag, NULL);

                    if (error_flag) {
                        goto check_error_flag;
                    }

                    new_pin = Parser_GetStrParam(parser_p, &error_flag, empty_string);

                    ATC_LOG_D("*ECEXPIN params: mode:%d, app_id:%s, code:%s, pin_code:%s, new_pin:%s", mode, app_id ? app_id : empty_string, code, pin_code, new_pin);
                }

check_error_flag:

                if (error_flag) {
                    ATC_LOG_D("parser error_flag was set");
                    current_cmee = CMEE_INCORRECT_PARAMETERS;
                    goto end;
                }

                /* Last Parameter received. Check if the user by mistake sends more parameters!
                   Check this! */
                if (!parser_p->NoMoreParams) {
                    ATC_LOG_E("Too many parameters received");
                    return AT_ERROR;
                }

                new_pin = strcmp(new_pin, "(null)") == 0 ? empty_string : new_pin;

                exe_cpin_code_t curr_code = EXE_CPIN_CODE_NO_CODE;

                if (parser_p->Command == AT_PLUS_CPIN) {
                    curr_code = sim_state_p->code;
                } else {
                    if (mode == 2) {
                        ATC_LOG_D("query pin state per application id");

                        /* Query pin state per application id */
                        if (ecexpin_query_app_state(parser_p, exe_p, app_id) != EXE_PENDING) {
                            current_cmee = CMEE_SIM_FAILURE;
                            goto end;
                        }

                        parser_p->sub_state = EXE_CPIN_RESP_STATE_QUERY_APP_STATE;
                        parser_p->get_sim_state = false;
                        return AT_PENDING;
                    }

                    /* Use the code in *ECEXPIN as wanted "state" */
                    curr_code = cpin_get_code_value(code);

                    if (curr_code == EXE_CPIN_CODE_NO_CODE) {
                        current_cmee = CMEE_INCORRECT_PARAMETERS;
                        goto end;
                    }
                }

                /* It is only valid with the newpin parameter when any puk is required. */
                if ((EXE_CPIN_CODE_SIM_PUK == curr_code)
                        || (EXE_CPIN_CODE_SIM_PUK2 == curr_code)) {
                    /* 8 digits are required for the puk code and 4-8 digits are required */
                    /* for the new pin code. */
                    if ((strnlen((char const *) pin_code, max_pin_test_length) != 8)
                            || (strnlen((char const *) new_pin, max_pin_test_length) < 4)
                            || (strnlen((char const *) new_pin, max_pin_test_length) > 8)) {
                        Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PASSWORD);
                        return AT_CME_ERROR;
                    }
                }
                /* It is only valid with the code parameter when any SIMLock control key is required. */
                else if ((EXE_CPIN_CODE_PH_NET_PIN == curr_code)
                         || (EXE_CPIN_CODE_PH_NETSUB_PIN == curr_code)
                         || (EXE_CPIN_CODE_PH_SP_PIN == curr_code)
                         || (EXE_CPIN_CODE_PH_CORP_PIN == curr_code)
                         || (EXE_CPIN_CODE_PH_SIMLOCK_PIN == curr_code)
                         || (EXE_CPIN_CODE_PH_ESL_PIN == curr_code)) {
                    /* Only one code must be given, with 8-16 digits required.
                     MIN_SIZE_SIMLOCK_CONTROL_KEY=8, MAX_SIZE_SIMLOCK_CONTROL_KEY=16
                     */
                    if ((strnlen((char const *) new_pin, max_pin_test_length) != 0)
                            || (strnlen((char const *) pin_code, max_pin_test_length) < 8)
                            || (strnlen((char const *) pin_code, max_pin_test_length) > 16)) {
                        current_cmee = CMEE_INCORRECT_PASSWORD;
                        goto end;
                    }
                }
                /* Enter PUK and sim pin when in enter sim pin state */
                else if ((strnlen((char const *) new_pin, max_pin_test_length) != 0)
                         && (curr_code == EXE_CPIN_CODE_SIM_PIN || curr_code == EXE_CPIN_CODE_READY)
                         && (strnlen((char const *) pin_code, max_pin_test_length) == 8)
                         && (strnlen((char const *) new_pin, max_pin_test_length) >= 4)
                         && (strnlen((char const *) new_pin, max_pin_test_length) <= 8)) {
                    curr_code = EXE_CPIN_CODE_SIM_PUK;
                }
                /* Only one code must be given, with 4-8 digits required. */
                else if ((strnlen((char const *) new_pin, max_pin_test_length) != 0)
                         || (strnlen((char const *) pin_code, max_pin_test_length) < 4)
                         || (strnlen((char const *) pin_code, max_pin_test_length) > 8)) {
                    if (EXE_CPIN_CODE_BLOCKED == curr_code) { /* the SIM card is present but blocked */
                        current_cmee = CMEE_SIM_FAILURE;
                        goto end;
                    }

                    current_cmee = CMEE_INCORRECT_PASSWORD;
                    goto end;
                }

                memset(&cops_data, 0, sizeof(cops_data));
                code_data.pin_id = EXE_SIM_PIN_ID_UNKNOWN;

                switch (curr_code) {
                case EXE_CPIN_CODE_SIM_PIN:
                    code_data.pin_id = (parser_p->Command == AT_STAR_ECEXPIN ? EXE_SIM_PIN_ID_PIN1 : code_data.pin_id);
                    request_id = EXE_CPIN_VERIFY_SIM_PIN;
                    break;
                case EXE_CPIN_CODE_SIM_PUK:
                    code_data.pin_id = (parser_p->Command == AT_STAR_ECEXPIN ? EXE_SIM_PIN_ID_PIN1 : code_data.pin_id);
                    request_id = EXE_CPIN_VERIFY_SIM_PUK;
                    /* To be able to get correct CME error code in answer to a faulty puk enter when in stateless mode */
                    parser_p->cpin_code = (parser_p->Command == AT_STAR_ECEXPIN ? CMEE_SIM_PUK_REQUIRED : parser_p->cpin_code);
                    break;
                case EXE_CPIN_CODE_SIM_PIN2:
                    code_data.pin_id = (parser_p->Command == AT_STAR_ECEXPIN ? EXE_SIM_PIN_ID_PIN2 : code_data.pin_id);
                    request_id = EXE_CPIN_VERIFY_SIM_PIN2;
                    /* To be able to get correct CME error code in answer to a faulty pin2 enter when in stateless mode */
                    parser_p->cpin_code = (parser_p->Command == AT_STAR_ECEXPIN ? CMEE_SIM_PIN2_REQUIRED : parser_p->cpin_code);
                    break;
                case EXE_CPIN_CODE_SIM_PUK2:
                    code_data.pin_id = (parser_p->Command == AT_STAR_ECEXPIN ? EXE_SIM_PIN_ID_PIN2 : code_data.pin_id);
                    request_id = EXE_CPIN_VERIFY_SIM_PUK2;
                    /* To be able to get correct CME error code in answer to a faulty puk2 enter when in stateless mode */
                    parser_p->cpin_code = (parser_p->Command == AT_STAR_ECEXPIN ? CMEE_SIM_PUK2_REQUIRED : parser_p->cpin_code);
                    break;
                case EXE_CPIN_CODE_PH_NET_PIN:
                    cops_data.sim_lock_type = EXE_COPS_SIM_LOCK_TYPE_NETWORK_LOCK;
                    request_id = EXE_COPS_UNLOCK_SIM_LOCK;
                    break;
                case EXE_CPIN_CODE_PH_NETSUB_PIN:
                    cops_data.sim_lock_type = EXE_COPS_SIM_LOCK_TYPE_NETWORK_SUBSET_LOCK;
                    request_id = EXE_COPS_UNLOCK_SIM_LOCK;
                    break;
                case EXE_CPIN_CODE_PH_SP_PIN:
                    cops_data.sim_lock_type = EXE_COPS_SIM_LOCK_TYPE_SERVICE_PROVIDER_LOCK;
                    request_id = EXE_COPS_UNLOCK_SIM_LOCK;
                    break;
                case EXE_CPIN_CODE_PH_CORP_PIN:
                    cops_data.sim_lock_type = EXE_COPS_SIM_LOCK_TYPE_CORPORATE_LOCK;
                    request_id = EXE_COPS_UNLOCK_SIM_LOCK;
                    break;
                case EXE_CPIN_CODE_PH_SIMLOCK_PIN:
                    cops_data.sim_lock_type = EXE_COPS_SIM_LOCK_TYPE_USER_LOCK;
                    request_id = EXE_COPS_UNLOCK_SIM_LOCK;
                    break;
                case EXE_CPIN_CODE_PH_ESL_PIN:
                    cops_data.sim_lock_type = EXE_COPS_SIM_LOCK_TYPE_FLEXIBLE_ESL_LOCK;
                    request_id = EXE_COPS_UNLOCK_SIM_LOCK;
                    break;
                case EXE_CPIN_CODE_PH_SIM_PIN: // Phone lock is not supported
                default:
                    current_cmee = CMEE_OPERATION_NOT_SUPPORTED;
                    break;
                } /* end switch */

                if (CMEE_OK == current_cmee) {
                    if (request_id == EXE_COPS_UNLOCK_SIM_LOCK) {
                        exe_request_result_t exe_res;

                        cops_data.operation = EXE_COPS_FACILITY_PERSONALIZATION_OPERATION_UNLOCK;
                        cops_data.passwd_p = pin_code;

                        /* Synchronous COPS call */
                        result = exe_request(exe_p, request_id, &cops_data,
                                             (void *)parser_p, &parser_p->request_handle);

                        current_cmee = cops_data.error_code;

                        ATC_LOG_D("EXE_COPS_UNLOCK_SIM_LOCK request result: %d and cmee: %d", result, current_cmee);

                        /* SIM-lock successfully unlocked; restart SIM */
                        if (result == EXE_SUCCESS) {
                            result = exe_request(exe_p, EXE_SIM_RESTART, NULL,
                                                 (void *)parser_p, &parser_p->request_handle);

                            ATC_LOG_D("EXE_SIM_RESTART request result: %d", result);

                            if (result != EXE_PENDING) {
                                goto end;
                            }
                            parser_p->sub_state = EXE_CPIN_RESP_STATE_RESTART_SIM;
                            /* Save current cmee to use in response */
                            parser_p->cpin_code = current_cmee;

                            return AT_PENDING;
                        }

                        goto end;
                    } else {
                        ATC_LOG_D("pin_id: %d", code_data.pin_id);
                        code_data.code_p = pin_code;
                        code_data.new_pin_p = new_pin;

                        result = exe_request(exe_p, request_id, &code_data,
                                             (void *)parser_p, &parser_p->request_handle);
                    }

                    if (EXE_SUCCESS == result) { /* CODE FOR STUBBED MODE */
                        return AT_OK; /* CODE FOR STUBBED MODE */
                        /* END STUBBED MODE synchronous */
                    } else if (EXE_PENDING != result) {
                        current_cmee = CMEE_SIM_FAILURE;
                        goto end;
                    }

                    return AT_PENDING;
                }
            } else {
                /*
                 * We got a response from SIM
                 */
                current_cmee = CMEE_UNKNOWN;

                if (EXE_SUCCESS == message->exe_result_code) {
                    if (EXE_CPIN_RESP_STATE_QUERY_APP_STATE == parser_p->sub_state) {
                        /* Response to a query application state AT*ECEXPIN=2,, */
                        if (NULL == message->data_p) {
                            ATC_LOG_E("message->data_p == NULL!");
                            current_cmee = CMEE_SIM_FAILURE;
                            goto end;
                        }

                        exe_cpin_sim_get_state_t *sim_state_data_p = message->data_p;

                        switch (sim_state_data_p->error_code) {
                        case CMEE_OK:
                        case CMEE_SIM_PIN_REQUIRED:
                        case CMEE_SIM_PIN2_REQUIRED:
                        case CMEE_SIM_PUK_REQUIRED:
                        case CMEE_SIM_PUK2_REQUIRED:
                        case CMEE_PH_NET_PIN_REQUIRED:
                        case CMEE_PH_NETSUB_PIN_REQUIRED:
                        case CMEE_PH_SP_PIN_REQUIRED:
                        case CMEE_PH_CORP_PIN_REQUIRED:
                        case CMEE_PH_SIMLOCK_PIN_REQUIRED:
                        case CMEE_PH_NETSUB_PUK_REQUIRED:
                        case CMEE_PH_SP_PUK_REQUIRED:
                        case CMEE_PH_CORP_PUK_REQUIRED:
                            current_cmee = CMEE_OK;
                            break;
                        default:
                            current_cmee = sim_state_data_p->error_code;
                            goto end;
                        }

                        exe_cpin_code_t response_code = sim_state_data_p->code;

                        strcpy((char *) info_text, ": ");
                        cpin_get_code_tag((char *) info_text + strlen((char *) info_text), (unsigned char) response_code);
                        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
                        return AT_OK;

                    } else if (EXE_CPIN_RESP_STATE_RESTART_SIM == parser_p->sub_state) {
                        current_cmee = parser_p->cpin_code;
                        ATC_LOG_D("EXE_CPIN_RESP_STATE_RESTART_SIM current_cmee from cops call: %d", current_cmee);
                    } else {
                        /* PIN verification fail/success is given by the sent data
                         * (0 if successfully verified), not by the EXE_SUCCESS/EXE_FAIL
                         */
                        current_cmee = *((exe_cmee_error_t *)message->data_p);

                        /* The executor can not determine if the operation was a PIN, PIN2 or PUK, PUK2 request */
                        if ((CMEE_SIM_PIN_REQUIRED == current_cmee || CMEE_SIM_PUK_REQUIRED == current_cmee)
                                && CMEE_SIM_PUK2_REQUIRED == parser_p->cpin_code) {
                            current_cmee = CMEE_SIM_PUK2_REQUIRED; /* There are attempts left */
                        } else if ((CMEE_SIM_PIN_REQUIRED == current_cmee || CMEE_SIM_PUK_REQUIRED == current_cmee)
                                   && CMEE_SIM_PUK_REQUIRED == parser_p->cpin_code) {
                            current_cmee = CMEE_SIM_PUK_REQUIRED; /* There are attempts left */
                        } else if (CMEE_SIM_PIN_REQUIRED == current_cmee && CMEE_SIM_PIN2_REQUIRED == parser_p->cpin_code) {
                            current_cmee = CMEE_SIM_PIN2_REQUIRED; /* There are attempts left */
                        } else if (current_cmee == CMEE_SIM_PUK_REQUIRED && CMEE_SIM_PIN2_REQUIRED == parser_p->cpin_code) {
                            current_cmee = CMEE_SIM_PUK2_REQUIRED; /* There are no attempts left */
                        } else if ( current_cmee == CMEE_SIM_BLOCKED ) {
                            if (CMEE_SIM_PIN2_REQUIRED == parser_p->cpin_code) {
                                current_cmee = CMEE_SIM_PUK2_REQUIRED;
                            } else if (CMEE_SIM_PIN_REQUIRED == parser_p->cpin_code) {
                                current_cmee = CMEE_SIM_PUK_REQUIRED;
                            }
                        }
                    }
                } else if (EXE_FAILURE == message->exe_result_code) {
                    if (EXE_CPIN_RESP_STATE_RESTART_SIM == parser_p->sub_state) {
                        current_cmee = parser_p->cpin_code;
                        ATC_LOG_D("EXE_CPIN_RESP_STATE_RESTART_SIM current_cmee from cops call: %d", current_cmee);

                        if (NULL != message->data_p) {
                            current_cmee = *((exe_cmee_error_t *)message->data_p);
                            ATC_LOG_D("EXE_CPIN_RESP_STATE_RESTART_SIM current_cmee from sim restart: %d", current_cmee);
                        }
                    }
                }

                parser_p->cpin_code = EXE_CPIN_CODE_NO_CODE;
                parser_p->sub_state = EXE_CPIN_RESP_STATE_NORMAL;
            }
        }

        break;
    } /* End of AT_MODE_SET: */
    case AT_MODE_TEST: {
        exe_cpin_code_t code = EXE_CPIN_CODE_READY;
        strcpy((char *) info_text, ": (");

        for (code = 0; code < EXE_CPIN_CODE_NO_CODE; code++) {
            if (code != EXE_CPIN_CODE_READY) {
                strcat((char *) info_text, ",");
            }

            if (parser_p->Command == AT_PLUS_CPIN) {
                cpin_get_code_tag((char *) info_text + strlen((char *) info_text), code);
            } else {/* AT_STAR_ECEXPIN */
                strcat((char *) info_text, "\"");
                cpin_get_code_tag((char *) info_text + strlen((char *) info_text), code);
                strcat((char *) info_text, "\"");
            }
        }

        strcat((char *) info_text, ")");
        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        return AT_OK;
    } /* End of AT_MODE_TEST: */
    case AT_MODE_READ: {
        if (!parser_p->IsPending) {
            result = exe_request(exe_p, EXE_CPIN_GET_SIM_STATE, NULL,
                                 (void *)parser_p, &parser_p->request_handle);

            if (EXE_PENDING != result) {
                current_cmee = CMEE_SIM_FAILURE;
                goto end;
            }

            parser_p->get_sim_state = true; /* Indicate that we have requested sim status */
            return AT_PENDING;
        } else { /* Response from sim */

            if (NULL == message->data_p) {
                ATC_LOG_E("message->data_p == NULL!");
                current_cmee = CMEE_SIM_FAILURE;
                goto end;
            }

            exe_cpin_sim_get_state_t *sim_state_data_p = message->data_p;

            switch (sim_state_data_p->error_code) {
            case CMEE_OK:
            case CMEE_SIM_PIN_REQUIRED:
            case CMEE_SIM_PIN2_REQUIRED:
            case CMEE_SIM_PUK_REQUIRED:
            case CMEE_SIM_PUK2_REQUIRED:
            case CMEE_PH_NET_PIN_REQUIRED:
            case CMEE_PH_NETSUB_PIN_REQUIRED:
            case CMEE_PH_SP_PIN_REQUIRED:
            case CMEE_PH_CORP_PIN_REQUIRED:
            case CMEE_PH_SIMLOCK_PIN_REQUIRED:
            case CMEE_PH_NETSUB_PUK_REQUIRED:
            case CMEE_PH_SP_PUK_REQUIRED:
            case CMEE_PH_CORP_PUK_REQUIRED:
                current_cmee = CMEE_OK;
                break;
            default:
                current_cmee = sim_state_data_p->error_code;
                goto end;
            }

            exe_cpin_code_t response_code = sim_state_data_p->code;

            parser_p->get_sim_state = false;
            strcpy((char *) info_text, ": ");
            cpin_get_code_tag((char *) info_text + strlen((char *) info_text), (unsigned char) response_code);
            Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
            return AT_OK;
        }

        break;
    } /* End of AT_MODE_READ: */
    default:
        current_cmee = CMEE_UNKNOWN;
        break;
    } /* end switch */

end:
    Parser_SetCMEE_ErrorType(parser_p, current_cmee);

    switch (current_cmee) {
    case CMEE_OK:
        return AT_OK;
    default:
        return AT_CME_ERROR;
    }
} /* End of AT_PLUS_CPIN_Handle */
#endif /* AT_PLUS_CPIN_ENABLED */

/*
 *===============================================================================
 *
 *   Function: AT_PLUS_CPOL_Handle
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
 *
 *   Description:
 *          SET command writes a plmn entry to the SIM on position index OR writes a plmn entry
 *          to the first free space on SIM OR erases a plmn entry from the SIM OR attempts to change
 *          the format for the READ command. See 3gpp 27.007 for exact specifications.
 *          READ command returns all valid plmn entries on the SIM card
 *          TEST command returns valid indexes for plmn entries on the SIM as well as the currently
 *          supported formats for the operator string.
 *===============================================================================
 */
AT_Command_e AT_PLUS_CPOL_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message)
{
    AT_Command_e result = AT_ERROR;
    exe_request_result_t exe_result = EXE_FAILURE;

    exe_cmee_error_t current_cmee = CMEE_OK; /* GSM specific error code. */

    switch (parser_p->Mode) {
    case AT_MODE_SET:

        if (!parser_p->IsPending) {
            /* Parse parameters. */

            bool invalid_index = false;
            bool invalid_format = false;
            bool invalid_oper = false;
            bool invalid_act_params = true;
            IntParam_t default_format = CPOL_FORMAT_NUMERIC;
            char *oper_p;

            exe_sim_cpol_info_t cpol_set_data;
            memset(cpol_set_data.oper_string, 0, EXE_OPERATOR_NUMERIC_STRING_MAX_LENGTH + 1);

            cpol_set_data.index = Parser_GetIntParam(parser_p, &invalid_index, NULL);

            /* AT spec issue: Parameter format has default value =2 if it is not present, but if it is not */
            if (!parser_p->NoMoreParams) {
                cpol_set_data.format = Parser_GetIntParam(parser_p, &invalid_format, (IntParam_t *)&default_format);
            } else {
                invalid_format = true;
            }

            oper_p = Parser_GetStrParam(parser_p, &invalid_oper, NULL);


            if (!parser_p->NoMoreParams) {
                /* As long as one parameter is set here, all must be valid */
                invalid_act_params = false;
                cpol_set_data.gsm_act = Parser_GetIntParam(parser_p, &invalid_act_params, NULL);
                cpol_set_data.gsm_compact_act = Parser_GetIntParam(parser_p, &invalid_act_params, NULL);
                cpol_set_data.utran_act = Parser_GetIntParam(parser_p, &invalid_act_params, NULL);

                /* If they exist, they must be correct: */
                if (invalid_act_params || 2 <= cpol_set_data.gsm_act || 2 <= cpol_set_data.gsm_compact_act || 2 <= cpol_set_data.utran_act) {
                    current_cmee = CMEE_INCORRECT_PARAMETERS;
                    goto end;
                }
            }

            /* All parameters parsed. Commence error checks: */
            if (!parser_p->NoMoreParams) {
                /* Too many parameters put in */
                current_cmee = CMEE_INCORRECT_PARAMETERS;
                goto end;
            }

            if (invalid_index && 0 < cpol_set_data.index) {
                /* Index can only be invalid if it is too large. */
                current_cmee = CMEE_INCORRECT_PARAMETERS;
                goto end;
            }

            if (!invalid_format && CPOL_FORMAT_NUMERIC != cpol_set_data.format) {
                current_cmee = CMEE_OPERATION_NOT_SUPPORTED;
                goto end;
            }

            if (!invalid_oper) {
                if (CPOL_FORMAT_NUMERIC == cpol_set_data.format) { /* This check is unnecessary until more than numeric format is supported */
                    /* Check that operator string is on correct numeric format */
                    int i = 0;

                    strncpy(cpol_set_data.oper_string, oper_p, EXE_OPERATOR_NUMERIC_STRING_MAX_LENGTH + 1);

                    for (i = 0; i < MIN_NUMERIC_STRING_LENGTH; i++) {
                        if ('0' > cpol_set_data.oper_string[i] || '9' < cpol_set_data.oper_string[i]) {
                            ATC_LOG_I("AT+CPOL called with string oper being illegal. (element %d had the value %d)", i, cpol_set_data.oper_string[i]);
                            current_cmee = CMEE_INCORRECT_PARAMETERS;
                            goto end;
                        }
                    }

                    if (0 != cpol_set_data.oper_string[MIN_NUMERIC_STRING_LENGTH]) {
                        /* String can be 6 numbers. But then that last figure must be valid and the string must end right after! */
                        if ('0' > cpol_set_data.oper_string[MIN_NUMERIC_STRING_LENGTH] || '9' < cpol_set_data.oper_string[MIN_NUMERIC_STRING_LENGTH] || 0 != cpol_set_data.oper_string[MIN_NUMERIC_STRING_LENGTH + 1]) {
                            ATC_LOG_I("AT+CPOL called with last character being illegal");
                            current_cmee = CMEE_INCORRECT_PARAMETERS;
                            goto end;
                        }
                    }
                }

                if (invalid_act_params) {
                    /* If operator is given, so must AcT parameters */
                    current_cmee = CMEE_INCORRECT_PARAMETERS;
                    goto end;
                }
            }

            if (invalid_index && invalid_oper && !invalid_format) {
                /* Command to change output format. As only one format is supported, this command
                 * will not do anything. */
                goto end;
            }

            if ((!invalid_oper && !invalid_format) ||             /* Command to set operator string */
                    (!invalid_index && invalid_oper)) {                 /* Command to remove operator string */
                if (invalid_index) {
                    /* Command to set operator string on first free index. */
                    cpol_set_data.index = -1;
                }

                /* Make request to executor. */
                result = exe_request(atc_get_exe_handle(), EXE_CPOL_SET, &cpol_set_data,
                                     (void *)parser_p, &parser_p->request_handle);

                if (EXE_PENDING != result) {
                    current_cmee = CMEE_SIM_FAILURE;
                    goto end;
                }

                return AT_PENDING;
            }

            /* All other combinations are illegal */
            current_cmee = CMEE_INCORRECT_PARAMETERS;
        } else {
            /* Check answer from executor. Return data in case of FAILURE is the CMEE error code. */
            if (message->exe_result_code == EXE_SUCCESS) {
                current_cmee = CMEE_OK;
            } else {
                current_cmee = *(exe_cmee_error_t *)(message->data_p);
            }
        }

        break;
    case AT_MODE_READ:

        if (!parser_p->IsPending) {

            result = exe_request(atc_get_exe_handle(), EXE_CPOL_READ, NULL,
                                 (void *)parser_p, &parser_p->request_handle);

            if (EXE_PENDING != result) {
                current_cmee = CMEE_SIM_FAILURE;
                goto end;
            }

            return AT_PENDING;
        } else {
            /* Parse the response to the output */
            if (message->exe_result_code == EXE_SUCCESS) {
                exe_sim_cpol_info_response_t *response_p = (exe_sim_cpol_info_response_t *)(message->data_p);
                int n = 0;
                n = sprintf((char *)info_text, ": ");

                while ((response_p != NULL)) {
                    n += sprintf((char *)info_text + n, "%d,", response_p->cpol_info.index);
                    n += sprintf((char *)info_text + n, "%d,", response_p->cpol_info.format);

                    if (strlen(response_p->cpol_info.oper_string) > 0) {
                        n += sprintf((char *)info_text + n, "\"%s\"", response_p->cpol_info.oper_string);
                    }

                    /* The Act parameters shall only be printed if the information is read from a file supporting AcT. */
                    if (response_p->valid_act) {
                        n += sprintf((char *) info_text + n, ",%d,", response_p->cpol_info.gsm_act);
                        n += sprintf((char *) info_text + n, "%d,", response_p->cpol_info.gsm_compact_act);
                        n += sprintf((char *) info_text + n, "%d", response_p->cpol_info.utran_act);
                    }

                    if (NULL != response_p->next_p) {
                        Parser_SendMultipleResponses(parser_p, AT_NO_OVERRIDE, info_text, false);
                        n = sprintf((char *)info_text, ": ");
                    }

                    response_p = response_p->next_p;
                }

                Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
                current_cmee = CMEE_OK;
            } else {
                current_cmee = *(exe_cmee_error_t *)(message->data_p);
            }
        }

        break;
    case AT_MODE_TEST:

        /* To get the range of valid indexes, a read from the SIM card must be performed */
        if (!parser_p->IsPending) {

            result = exe_request(atc_get_exe_handle(), EXE_CPOL_TEST, NULL,
                                 (void *)parser_p, &parser_p->request_handle);

            if (EXE_PENDING != result) {
                current_cmee = CMEE_SIM_FAILURE;
                goto end;
            }

            return AT_PENDING;
        } else {
            if (message->exe_result_code == EXE_SUCCESS) {
                int noOfValidIndexes = *((int *)(message->data_p));

                /* Put together the output string: */
                int n = 0;
                n = sprintf((char *)info_text, ": ");
                n += sprintf((char *)info_text + n, "(0-%d),(2)", (noOfValidIndexes - 1));

                Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
                current_cmee = CMEE_OK;
            } else {
                current_cmee = *(exe_cmee_error_t *)(message->data_p);
            }
        }

        break;
    }

end:

    if (CMEE_OK != current_cmee) {
        Parser_SetCMEE_ErrorType(parser_p, current_cmee);
        return AT_CME_ERROR;
    }

    return AT_OK;
} /* End of AT_PLUS_CPOL_Handle */

/*
 *===============================================================================
 *
 *   Function: AT_STAR_ESIMSR_Handle
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
 *   The ESIMSR command is used SIM State Reporting
 *
 *   SET   - Used to enable/disable SIM state reporting
 *   READ  - Read the current setting and SIM state
 *
 *===============================================================================
 */
AT_Command_e AT_STAR_ESIMSR_Handle(AT_ParserState_s *parser_p,
                                   AT_CommandLine_t info_text,
                                   AT_ResponseMessage_t message)
{
    exe_request_result_t exe_result = EXE_FAILURE;
    AT_Command_e result = AT_ERROR;
    IntParam_t n = 0; /* Default value */
    exe_t *exe_p = atc_get_exe_handle();
    bool error_flag = false;

    if (!parser_p->IsPending) {
        switch (parser_p->Mode) {
        case AT_MODE_SET:

            if (!parser_p->NoMoreParams) {
                n = Parser_GetIntParam(parser_p, &error_flag, &n);

                if (error_flag) {
                    goto error;
                }

                if (!parser_p->NoMoreParams || n > 1) {
                    goto error;
                }
            }

            parser_p->ESIMSR = n;
            result = AT_OK;
            break;

        case AT_MODE_READ:
            exe_result = exe_request(exe_p, EXE_ESIMSR_GET_STATE, NULL,
                                     (void *)parser_p, &parser_p->request_handle);

            if (EXE_PENDING != exe_result) {
                goto error;
            }

            result = parser_handle_exe_result(exe_result, parser_p);
            break;
        case AT_MODE_TEST:
            sprintf((char *)info_text, ": (0,1)");
            Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
            result = AT_OK;
            break;
        default:
            goto error;
            break;
        }
    } else {
        switch (parser_p->Mode) {
        case AT_MODE_READ: {
            exe_result = message->exe_result_code;

            if (EXE_SUCCESS == message->exe_result_code && NULL != message->data_p) {
                IntParam_t at_sim_state;
                exe_esimsr_sim_state_t *sim_state_p = (exe_esimsr_sim_state_t *)(message->data_p);
                sprintf((char *) info_text, ": ");
                AT_AddValue(info_text, "0", parser_p->ESIMSR);

                if (0 != parser_p->ESIMSR) {
                    AT_AddValue(info_text, ",0", sim_state_p->sim_state);
                }
            } else {
                goto error;
                break;
            }

            Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
            result = parser_handle_exe_result(exe_result, parser_p);
        }
        break;
        default:
            goto error;
            break;
        }
    }

    return result;
error:
    return AT_ERROR;

} /* End of AT_STAR_ESIMSR_Handle */

void handle_incoming_ESIMSR_event(exe_esimsr_sim_state_t *response_p)
{
    char esimsr_text[30]; /*"*ESIMSR: <sim_state>" */
    exe_esimsr_sim_state_t *state_p = (exe_esimsr_sim_state_t *)response_p;
    IntParam_t at_sim_state;
    AT_ParserState_s *parser_p;
    unsigned char entry;
    at_sim_state = state_p->sim_state;

    /* Loop through all parser states to find subscriptions. */
    for (entry = 0; entry < MAX_NO_OF_OPEN_CHANNELS; entry++) {
        parser_p = ParserStateTable_GetParserState_ByEntry(entry);

        if (parser_p->ESIMSR) {
            memset(esimsr_text, '\0', sizeof(esimsr_text));
            strcat(esimsr_text, ": ");
            AT_AddValue((AT_CommandLine_t)esimsr_text, "0", at_sim_state);
            parser_send_unsolicited(parser_p, AT_STAR_ESIMSR, (AT_CommandLine_t)esimsr_text);
        }
    }
}

/*
 *===============================================================================
 *
 *   Function: AT_STAR_ESIMRF_Handle
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
 *   The ESIMRF command is used for SIM Refresh State Reporting
 *
 *   SET   - Used to enable/disable PIN event reporting
 *   READ  - Read the current setting and SIM state
 *   TEST  - Returns supported SET parameters
 *
 *===============================================================================
 */
AT_Command_e AT_STAR_ESIMRF_Handle(AT_ParserState_s *parser_p,
                                   AT_CommandLine_t info_text,
                                   AT_ResponseMessage_t message)
{
    exe_request_result_t exe_result = EXE_FAILURE;
    AT_Command_e result = AT_ERROR;
    IntParam_t n = 0; /* Default value */
    bool error_flag = false;

    if (!parser_p->IsPending) {
        switch (parser_p->Mode) {
        case AT_MODE_SET:

            if (!parser_p->NoMoreParams) {
                n = Parser_GetIntParam(parser_p, &error_flag, NULL);

                if (error_flag) {
                    goto error;
                }

                if (!parser_p->NoMoreParams || n > 1) {
                    goto error;
                }
            }
            else {
               goto error;
            }
            parser_p->ESIMRF = n;
            result = AT_OK;
            break;

        case AT_MODE_READ:
            sprintf((char *)info_text, ": %d", parser_p->ESIMRF);
            Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
            result = AT_OK;
            break;
        case AT_MODE_TEST:
            sprintf((char *)info_text, ": (0,1)");
            Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
            result = AT_OK;
            break;
        default:
            goto error;
            break;
        }
    } else {
        goto error;
    }

    return result;
error:
    return AT_ERROR;
}

void handle_incoming_esimrf_event(exe_esimrf_unsol_t *esimrf_data_p)
{
    const uint32_t esimrf_size = 35; /*"*ESIMRF: <cmdnumber>,<type>[[,< fileid>,<pathid>]" */
    char *esimrf_text_p;
    IntParam_t at_sim_state;
    AT_ParserState_s *parser_p;
    unsigned char entry;

    if (!esimrf_data_p) {
        goto error;
    }

    esimrf_text_p = alloca(esimrf_size);

    if (!esimrf_text_p) {
        goto error;
    }

    memset(esimrf_text_p, '\0', esimrf_size);

    /* Loop through all parser states to find subscriptions. */
    for (entry = 0; entry < MAX_NO_OF_OPEN_CHANNELS; entry++) {
        parser_p = ParserStateTable_GetParserState_ByEntry(entry);

        if (parser_p->ESIMRF) {
            if (esimrf_data_p->file_id == 0) {
                sprintf(esimrf_text_p, ": 0,%d", esimrf_data_p->refresh_type);
            } else if (esimrf_data_p->path_p == NULL) {
                sprintf(esimrf_text_p, ": 0,%d,%d", esimrf_data_p->refresh_type, esimrf_data_p->file_id);
            } else {
                sprintf(esimrf_text_p, ": 0,%d,%d,\"%s\"", esimrf_data_p->refresh_type, esimrf_data_p->file_id, esimrf_data_p->path_p);
            }

            parser_send_unsolicited(parser_p, AT_STAR_ESIMRF, (AT_CommandLine_t)esimrf_text_p);
        }
    }

    return;

error:
    ATC_LOG_E("handle_incoming_esimrf_event(), error!");
}

/*
 *===============================================================================
 *
 *   Function: AT_STAR_EPEE_Handle
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
 *   The EPEE command enable PIN event that pin is verified.
 *
 *   SET   - Used to enable/disable PIN event reporting
 *   READ  - Read the current setting and SIM state
 *
 *===============================================================================
 */
AT_Command_e AT_STAR_EPEE_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message)
{
    exe_request_result_t exe_result = EXE_FAILURE;
    AT_Command_e result = AT_ERROR;
    IntParam_t n = 0; /* Default value */
    bool error_flag = false;

    if (!parser_p->IsPending) {
        switch (parser_p->Mode) {
        case AT_MODE_SET:

            if (!parser_p->NoMoreParams) {
                n = Parser_GetIntParam(parser_p, &error_flag, &n);

                if (error_flag) {
                    goto error;
                }

                if (!parser_p->NoMoreParams || n > 1) {
                    goto error;
                }
            }

            parser_p->EPEE = n;
            result = AT_OK;
            break;

        case AT_MODE_READ:
            sprintf((char *)info_text, ": %d", parser_p->EPEE);
            Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
            result = AT_OK;
            break;
        case AT_MODE_TEST:
            sprintf((char *)info_text, ": (0,1)");
            Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
            result = AT_OK;
            break;
        default:
            goto error;
            break;
        }
    } else {
        goto error;
    }

    return result;
error:
    return AT_ERROR;

} /* End of AT_STAR_EPEE_Handle */

/*
 * generate a *EPEV unsolicited event
 */
void handle_incoming_epev_event()
{
    AT_ParserState_s *parser_p;
    unsigned char entry;

    /* Loop through all parser states to find subscriptions. */
    for (entry = 0; entry < MAX_NO_OF_OPEN_CHANNELS; entry++) {
        parser_p = ParserStateTable_GetParserState_ByEntry(entry);

        if (parser_p->EPEE) {
            parser_send_unsolicited(parser_p, AT_TEXT_RESPONSE, (AT_CommandLine_t)"*EPEV");
        }
    }
}


/*===============================================================================
 *
 *   Function: AT_PLUS_CSCA_Handle
 *
 *
 *   INPUT:   Parser_p   - Pointer to the current parser state.
 *            InfoText_p - Pointer to a string buffer to put information text.
 *            Message  - Pointer to the response message used for pending commands.
 *
 *   OUTPUT:  Parser->
 *              Pos    - The current position in the command line.
 *
 *   RETURNS: A status code for the execution of the command is returned:
 *            AT_OK        - The command was successfully handled.
 *            AT_ERROR     - There was a general error.
 *            AT_CME_ERROR - There was a GSM specific error, the error code is
 *                           returned in text format in InfoText.
 *            AT_CMS_ERROR - There was a GSM SMS specific error, the error code is
 *                           returned in text format in InfoText.
 *
 *   Description:
 *          Set command updates the SMSC address, through which mobile originated SMs are transmitted.
 *          In text mode, setting is used by send (+CMGS) and write (+CMGW) commands. In PDU mode,
 *          setting is used by the same commands, but only when the length of the SMSC address coded
 *          into <pdu> parameter equals zero. Note that a "+" in front of the number in <sca> has
 *          precedence over the <tosca> parameter, so that a number starting with "+" will always be
 *          treated as an international number.
 *
 *   SET     - Set command updates the SMSC address, through which mobile originated SMs are transmitted.
 *   READ    - Show the current setting
 *   TEST    - Show if the commands is supported and returns the supported modes as a compound value
 *
 *===============================================================================
 */
AT_Command_e AT_PLUS_CSCA_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message)
{
    AT_Command_e result = AT_ERROR;

    if (!parser_p->IsPending) {
        switch (parser_p->Mode) {
        case AT_MODE_SET: {
            bool error_flag = false;
            exe_cmee_error_t CMEE_code = CMEE_OK;
            StrParam_t sca = Parser_GetStrParam(parser_p, &error_flag, "");
            IntParam_t tosca = 129;

            tosca = Parser_GetIntParam(parser_p, &error_flag, &tosca);

            if (error_flag) {
                ATC_LOG_E("AT_PLUS_CSCA_Handle(): parsing failed");
                goto error;
            }

            if (!parser_p->NoMoreParams) {
                ATC_LOG_E("AT_PLUS_CSCA_Handle(): too many parameters received");
                goto error;
            }

            if (0 == memcmp(sca, "", 1)) {
                ATC_LOG_E("AT_PLUS_CSCA_Handle(): could not parse <sca>");
                goto error;
            } else {
                uint32_t i;
                uint32_t j;
                exe_type_of_number_t ton = EXE_TYPE_OF_NUMBER_UNKNOWN;
                exe_csca_request_data_t request_data;
                exe_request_result_t executor_result;

                /* Validate and correct <sca> */
                for (i = 0, j = 0; sca[i] != END_OF_STRING; i++) {
                    if ((sca[i] >= '0' && sca[i] <= '9') ||
                            (sca[i] == '*') ||
                            (sca[i] == '#') ||
                            (sca[i] == 'A') ||
                            (sca[i] == 'B') ||
                            (sca[i] == 'C')) {
                        sca[j++] = sca[i];
                    } else if (sca[i] == '+' && i == 0) {
                        ton = EXE_TYPE_OF_NUMBER_INTERNATIONAL;
                    } else {
                        Parser_SetCMEE_ErrorType(parser_p,
                                                 CMEE_INVALID_CHARACTER_IN_DIAL_STRING);
                        result = AT_CME_ERROR;
                        goto error;
                    }
                }

                /* <sca> contains only valid characters and in case of an international call
                 * the "+" has also been removed from <sca>. NUL-terminate <sca> and move on! */
                sca[j] = END_OF_STRING;

                /* A "+" character in the beginning of the <sca> parameter
                 * indicates that the type-of-number(ton) is international
                 * and that has precedence over the ton indication in the
                 * <tosca> parameter.*/
                if (EXE_TYPE_OF_NUMBER_UNKNOWN == ton) {
                    switch (tosca) {
                    case 129: {
                        ton = EXE_TYPE_OF_NUMBER_UNKNOWN;
                        break;
                    }
                    case 145: {
                        ton = EXE_TYPE_OF_NUMBER_INTERNATIONAL;
                        break;
                    }
                    case 161: {
                        ton = EXE_TYPE_OF_NUMBER_NATIONAL;
                        break;
                    }
                    default: {
                        ATC_LOG_E("AT_PLUS_CSCA_Handle(): unsupported <tosca>");
                        Parser_SetCMEE_ErrorType(parser_p,
                                                 CMEE_INVALID_INDEX);
                        result = AT_CME_ERROR;
                        goto error;
                    }
                    } /* <tosca> switch */
                }

                request_data.ton = ton;
                request_data.text_p = sca;

                executor_result = exe_request(atc_get_exe_handle(),
                                              EXE_CSCA_SET_SMSC,
                                              &request_data,
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
                                                   EXE_CSCA_GET_SMSC,
                                                   NULL,
                                                   (void *)parser_p,
                                                   &parser_p->request_handle);

            if (EXE_PENDING == executor_result) {
                result = AT_PENDING;
            }

            break;
        }
        case AT_MODE_TEST: {
            result = AT_OK;
            break;
        }
        default:
            ATC_LOG_E("AT_PLUS_CSCA_Handle(): Unknown operation mode during request");
            break; /* avoid LINT info */
        } /* End switch */
    } else {
        switch (parser_p->Mode) {
        case AT_MODE_SET: {
            if (NULL != message) {
                if (EXE_SUCCESS == message->exe_result_code) {
                    result = AT_OK;
                } else {
                    ATC_LOG_E("AT_PLUS_CSCA_Handle(): unsuccessful response");
                    Parser_SetCMEE_ErrorType(parser_p,
                                             CMEE_OPERATION_NOT_ALLOWED);
                    result = AT_CME_ERROR;
                }
            } else {
                ATC_LOG_I("AT_PLUS_CSCA_Handle(): message is NULL");
            }

            break;
        }
        case AT_MODE_READ: {
            if (NULL != message) {
                if (EXE_SUCCESS == ((AT_ResponseMessage_t) message)->exe_result_code) {
                    exe_csca_response_data_t *data_p = (exe_csca_response_data_t *) message->data_p;

                    if (data_p && data_p->text_p) {
                        int length = strlen(data_p->text_p) + 1;
                        int n = snprintf((char *)info_text,
                                         8 + length, /* ": "<scb>",<tosca>"*/
                                         ": \"%s\",%d",
                                         data_p->text_p,
                                         data_p->tosca);

                        if (n < 0) {
                            ATC_LOG_E("AT_PLUS_CSCA_Handle(): invalid format in request data");
                            goto error;
                        } else if (n >= 8 + length) {
                            ATC_LOG_E("AT_PLUS_CSCA_Handle(): Warning! truncated operator string");
                        }

                        result = AT_OK;
                        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
                    } else {
                        ATC_LOG_E("AT_PLUS_CSCA_Handle(): no record data returned by SIM");
                        Parser_SetCMEE_ErrorType(parser_p, CMEE_NOT_FOUND);
                        result = AT_CME_ERROR;
                    }
                } else {
                    ATC_LOG_E("AT_PLUS_CSCA_Handle(): unsuccessful response");
                    Parser_SetCMEE_ErrorType(parser_p, CMEE_OPERATION_NOT_ALLOWED);
                    result = AT_CME_ERROR;
                }
            } else {
                ATC_LOG_E("AT_PLUS_CSCA_Handle(): message is NULL");
            }

            break;
        }
        default:
            ATC_LOG_E("AT_PLUS_CSCA_Handle(): Unexpected mode during response");
            break;
        }
    }

error:
    return result;
} /* End of AT_PLUS_CSCA_Handle */

#define CSAS_CRES_MAX_PROFILES 252

/*
 *===============================================================================
 *
 *   Function: AT_PLUS_CRES_Handle
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
 *            AT_ERROR     - There were a general error.
 *            AT_CMS_ERROR - There were a GSM specific error, the error code is
 *                           returned in text format in InfoText.
 *
 *   The +CRES function is used to restore an SMS profile
 *
 *   DO - Restores to the default SMS profile
 *   SET - Restores a given SMS profile
 *   TEST - Returns the supported modes as a compound value.
 *
 *   Coders note : SIM seems to use position 1 to store the active profile, and
 *                 2 is for factory default.
 *                 Leading to a valid SIM range of 3..255 (2 less that the spec's),
 *                 i.e. max range (0-252) in test command.
 *
 *===============================================================================
 */
AT_Command_e AT_PLUS_CRES_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message)
{
    AT_Command_e result = AT_ERROR;
    IntParam_t profile = 1; /* First profile that can be restored */
    exe_cms_error_t current_cms_error = EXE_CMS_OK;
    exe_t *exe_p = atc_get_exe_handle();
    exe_request_result_t exe_result = EXE_FAILURE;
    exe_cres_csas_t cres_data;

    if (!parser_p->IsPending) {
        switch (parser_p->Mode) {
        case AT_MODE_DO: {
            cres_data.profile = 2;  /* This is where the default profile
 * is stored in the SIM file */

            /* No input parameter, restore to default profile */
            exe_result = exe_request(exe_p, EXE_CRES, &cres_data,
                                     (void *)parser_p, &parser_p->request_handle);

            if (EXE_PENDING != exe_result) {
                goto error;
            }

            result = parser_handle_exe_result(exe_result, parser_p);
            break;
        }

        case AT_MODE_SET: {
            bool error_flag = false;

            /* Extract AT command arguments. */
            profile = Parser_GetIntParam(parser_p, &error_flag, &profile);

            if (error_flag) {
                return AT_ERROR;
            }

            if (CSAS_CRES_MAX_PROFILES < profile) {
                current_cms_error = EXE_CMS_SIM_FAILURE;
                goto error;
            }

            /* Test that the given profile is valid is done in the executer part.
             * Add 3 to avoid the two reserved slots in the SIM file. the SIM API indexes from 1..x */
            cres_data.profile = profile + 3;

            exe_result = exe_request(exe_p, EXE_CRES, &cres_data,
                                     (void *)parser_p, &parser_p->request_handle);

            if (EXE_PENDING != exe_result) {
                goto error;
            }

            result = parser_handle_exe_result(exe_result, parser_p);
            break;
        }

        case AT_MODE_TEST:
            exe_result = exe_request(exe_p, EXE_CRES_TEST, NULL,
                                     (void *)parser_p, &parser_p->request_handle);

            if (EXE_PENDING != exe_result) {
                goto error;
            }

            result = parser_handle_exe_result(exe_result, parser_p);
            break;
        }
    } else {
        if (NULL == message) {
            ATC_LOG_I("AT_PLUS_CRES_Handle(): message is NULL");
            goto error;
        }

        exe_result = message->exe_result_code;
        result = parser_handle_exe_result(exe_result, parser_p);

        if (AT_OK != result) {
            goto error;
        }

        switch (parser_p->Mode) {
        case AT_MODE_DO:
        case AT_MODE_SET: {
            exe_cres_csas_response_t *cres_response_p = message->data_p;

            if (EXE_CMS_OK != cres_response_p->error) {
                current_cms_error = cres_response_p->error;
                goto error;
            }

            break;
        }
        case AT_MODE_TEST: {
            exe_cres_csas_test_t *cres_profiles_p = (exe_cres_csas_test_t *)message->data_p;

            if (EXE_CMS_OK != cres_profiles_p->error) {
                current_cms_error = cres_profiles_p->error;
                goto error;
            }

            if (2 >= cres_profiles_p->profiles) {
                /* No place in SIM file to store service center addresses */
                current_cms_error = EXE_CMS_SIM_FAILURE;
                goto error;
            } else if (3 >= cres_profiles_p->profiles) {
                sprintf((char *)info_text, ": (0)"); /* Only 1 non active profile available */
            } else {
                /* It should be noted that the first position on SIM is always used
                 * for the active profile, and the second is used for factory default.
                 * So the first position to be saved to/restored
                 * from is position 3.
                 * This means that if SIM returns number of records = 5, then
                 * the test command will return range (0-2) where 0 is position
                 * 3 on the SIM, 1 is position 4 on SIM, etc.
                 * This also means that the max range will be (0-252). */
                sprintf((char *)info_text, ": (0-%d)", cres_profiles_p->profiles - 3);
            }

            Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
            break;
        }
        }
    }

    return result;

error:

    if (EXE_CMS_OK != current_cms_error) {
        Parser_SetCMS_ErrorType(parser_p, current_cms_error);
    }

    return AT_CMS_ERROR;
}

/*
 *===============================================================================
 *
 *   Function: AT_PLUS_CSAS_Handle
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
 *            AT_ERROR     - There were a general error.
 *            AT_CMS_ERROR - There were a GSM specific error, the error code is
 *                           returned in text format in InfoText.
 *
 *   The +CSAS is used to save an SMS profile
 *
 *   DO   - Saves to the default profile
 *   SET  - Saves a given SMS profile
 *   TEST - Returns the supported modes as a compound value.
 *
 *   Coders note : SIM seems to use position 1 to store the active profile, and
 *                 2 is for factory default.
 *                 Leading to a valid SIM range of 3..255 (2 less that the spec's),
 *                 i.e. max range (0-252) in test command.
 *
 *===============================================================================
 */
AT_Command_e AT_PLUS_CSAS_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message)
{
    AT_Command_e result = AT_ERROR;
    IntParam_t profile = 1; /* First profile that can be restored */
    exe_cms_error_t current_cms_error = EXE_CMS_OK;
    exe_t *exe_p = atc_get_exe_handle();
    exe_request_result_t exe_result = EXE_FAILURE;
    exe_cres_csas_t csas_data;

    if (!parser_p->IsPending) {
        switch (parser_p->Mode) {
        case AT_MODE_DO:
        case AT_MODE_SET: {
            bool error_flag = false;

            if (AT_MODE_DO == parser_p->Mode) {
                csas_data.profile = 3;  /* this is the default profile */
            } else {
                /* Extract AT command arguments. */
                profile = Parser_GetIntParam(parser_p, &error_flag, &profile);

                if (error_flag) {
                    return AT_ERROR;
                }

                if (CSAS_CRES_MAX_PROFILES < profile) {
                    current_cms_error = EXE_CMS_SIM_FAILURE;
                    goto error;
                }

                /* Test that the given profile is valid is done in the executer part.
                 * Add 3 to avoid the two reserved slots in the SIM file. the SIM API indexes from 1..x */
                csas_data.profile = profile + 3;
            }

            exe_result = exe_request(exe_p, EXE_CSAS, &csas_data,
                                     (void *)parser_p, &parser_p->request_handle);

            if (EXE_PENDING != exe_result) {
                goto error;
            }

            result = parser_handle_exe_result(exe_result, parser_p);
            break;
        }

        case AT_MODE_TEST:
            exe_result = exe_request(exe_p, EXE_CSAS_TEST, NULL,
                                     (void *)parser_p, &parser_p->request_handle);

            if (EXE_PENDING != exe_result) {
                goto error;
            }

            result = parser_handle_exe_result(exe_result, parser_p);
            break;
        }
    } else {
        if (NULL == message) {
            ATC_LOG_I("AT_PLUS_CSAS_Handle(): message is NULL");
            goto error;
        }

        exe_result = message->exe_result_code;
        result = parser_handle_exe_result(exe_result, parser_p);

        if (AT_OK != result) {
            goto error;
        }

        switch (parser_p->Mode) {
        case AT_MODE_DO:
        case AT_MODE_SET: {
            exe_cres_csas_response_t *csas_response_p = message->data_p;

            if (EXE_CMS_OK != csas_response_p->error) {
                current_cms_error = csas_response_p->error;
                goto error;
            }

            break;
        }
        case AT_MODE_TEST: {
            exe_cres_csas_test_t *csas_profiles_p = (exe_cres_csas_test_t *)message->data_p;

            if (EXE_CMS_OK != csas_profiles_p->error) {
                current_cms_error = csas_profiles_p->error;
                goto error;
            }

            if (2 >= csas_profiles_p->profiles) {
                /* No place in SIM file to store service center addresses */
                current_cms_error = EXE_CMS_SIM_FAILURE;
                goto error;
            } else if (3 >= csas_profiles_p->profiles) {
                sprintf((char *)info_text, ": (0)"); /* Only 1 non active profile available */
            } else {
                /* It should be noted that the first position on SIM is always used
                 * for the active profile, and the second is used for factory default.
                 * So the first position to be saved to/restored
                 * from is position 3.
                 * This means that if SIM returns number of records = 5, then
                 * the test command will return range (0-2) where 0 is position
                 * 3 on the SIM, 1 is position 4 on SIM, etc.
                 * This also means that the max range will be (0-252). */
                sprintf((char *)info_text, ": (0-%d)", csas_profiles_p->profiles - 3);
            }

            Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
            break;
        }
        }
    }

    return result;

error:

    if (EXE_CMS_OK != current_cms_error) {
        Parser_SetCMS_ErrorType(parser_p, current_cms_error);
    }

    return AT_CMS_ERROR;

} /* End of AT_PLUS_CSAS_Handle */



/*
 *===============================================================================
 *
 *   Function: AT_STAR_EPINR_Handle
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
 *   The EPINR command is used for getting number of remaining retries for
 *   PIN1, PIN2, PUK1 and PUK2
 *
 *   SET  - Gets the currently number of retries for given pin/puk:
 *          +EPINR=<Pin_Id>
 *            <Pin_Id> is a numeric string type values.
 *
 *   TEST - Writes the supported Pin_Id's to info_text:
 *          +EPINR: 0-3
 *
 *===============================================================================
 */

AT_Command_e AT_STAR_EPINR_Handle(AT_ParserState_s *parser_p,
                                  AT_CommandLine_t info_text,
                                  AT_ResponseMessage_t message)
{
    exe_request_result_t exe_result = EXE_FAILURE;
    AT_Command_e result = AT_ERROR;
    exe_cmee_error_t current_cmee = CMEE_UNKNOWN; /* GSM specific error code. */
    IntParam_t id = EXE_PIN_1; /* Default value */
    exe_t *exe_p = atc_get_exe_handle();
    bool error_flag = false;

    if (!parser_p->IsPending) {
        switch (parser_p->Mode) {
        case AT_MODE_SET: {
            exe_sim_pin_info_t exe_sim_pin_info;

            id = Parser_GetIntParam(parser_p, &error_flag, &id);

            if (error_flag) {
                goto error;
            }

            /* Test that Pin_Id is within limits */
            if (id > EXE_PUK_2 || id < EXE_PIN_1) {
                current_cmee = CMEE_INCORRECT_PARAMETERS;
                goto error;
            }

            if (!parser_p->NoMoreParams) {
                current_cmee = CMEE_INCORRECT_PARAMETERS;
                goto error;
            }

            exe_sim_pin_info.pin_puk = id;
            /* Clear response data */
            exe_sim_pin_info.attempts = 0;
            exe_sim_pin_info.pin_status = 0;
            exe_sim_pin_info.error_code = CMEE_OK;

            exe_result = exe_request(exe_p, EXE_EPINR, &exe_sim_pin_info,
                                     (void *) parser_p, &parser_p->request_handle);

            if (EXE_PENDING != exe_result) {
                goto error;
            }

            result = parser_handle_exe_result(exe_result, parser_p);
            break;
        }
        case AT_MODE_TEST:
            sprintf((char *)info_text, ": (%d-%d)", EXE_PIN_1, EXE_PUK_2);
            Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
            result = AT_OK;
            break;
        case AT_MODE_READ: {
            exe_result = exe_request(exe_p, EXE_EPINR_FULL, NULL,
                                     (void *) parser_p, &parser_p->request_handle);

            if (EXE_PENDING != exe_result) {
                goto error;
            }

            result = parser_handle_exe_result(exe_result, parser_p);
            break;
        }

        default:
            ATC_LOG_E("AT_STAR_EPINR_Handle. Invalid AT MODE: %d ", parser_p->Mode);
            goto error;
            break;
        }
    } else {
        switch (parser_p->Mode) {
        case AT_MODE_SET: {
            if (NULL != message) {
                if (EXE_SUCCESS == message->exe_result_code) {
                    exe_sim_pin_info_t *sim_pin_info_p = (exe_sim_pin_info_t *)(message->data_p);

                    if (CMEE_OK == sim_pin_info_p->error_code) {
                        sprintf((char *) info_text, ": %d", sim_pin_info_p->attempts);
                    } else {
                        current_cmee = sim_pin_info_p->error_code;
                        ATC_LOG_E("AT_STAR_EPINR_Handle. SIM fail ");
                        goto error;
                    }
                } else {
                    ATC_LOG_E("AT_STAR_EPINR_Handle. Not EXE_SUCCESS ");
                    goto error;
                    break;
                }
            } else {
                ATC_LOG_E("AT_STAR_EPINR_Handle. message == NULL");
                goto error;
                break;
            }

            Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
            result = parser_handle_exe_result(message->exe_result_code, parser_p);
            break;
        }
        case AT_MODE_READ: {
            if (NULL != message) {
                if (EXE_SUCCESS == message->exe_result_code) {
                    exe_sim_full_pin_info_t *sim_full_pin_info_p = (exe_sim_full_pin_info_t *)(message->data_p);

                    if (CMEE_OK == sim_full_pin_info_p->error_code) {
                        sprintf((char *) info_text, ": %d, %d, %d, %d",
                                sim_full_pin_info_p->pin1_attempts,
                                sim_full_pin_info_p->pin2_attempts,
                                sim_full_pin_info_p->puk1_attempts,
                                sim_full_pin_info_p->puk2_attempts);
                    } else {
                        current_cmee = sim_full_pin_info_p->error_code;
                        ATC_LOG_E("AT_STAR_EPINR_Handle. SIM fail ");
                        goto error;
                    }
                } else {
                    ATC_LOG_E("AT_STAR_EPINR_Handle. Not EXE_SUCCESS ");
                    goto error;
                    break;
                }
            } else {
                ATC_LOG_E("AT_STAR_EPINR_Handle. message == NULL");
                goto error;
                break;
            }

            Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
            result = parser_handle_exe_result(message->exe_result_code, parser_p);
            break;
        }
        default:
            goto error;
            break;
        }
    }

    return result;
error:

    Parser_SetCMEE_ErrorType(parser_p, current_cmee);
    return AT_CME_ERROR;

} /* End of AT_STAR_EPINR_Handle */


/*
 *====================================================================================
 *
 *   Function: AT_PLUS_CNUM_Handle
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
 *   The CNUM command returns the MSISDNs related to the subscriber (this information can
 *   be stored in the SIM or in the ME). If subscriber has different MSISDN for different
 *   services, each MSISDN is returned in a separate line.
 *
 *   SET  - Request subscriber MSISDN number.
 *
 *   READ - Not applicable.
 *
 *   TEST - Test command to show if the command is supported.
 *
 *====================================================================================
 */
AT_Command_e AT_PLUS_CNUM_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message)
{
    bool error_flag = false; /* Flags a parameter parsing error. */
    AT_Command_e result = AT_OK;
    int rv = 0;

    switch (parser_p->Mode) {
    case AT_MODE_DO: {
        if (!parser_p->IsPending) {
            exe_t *exe_p = atc_get_exe_handle();
            exe_request_result_t exe_request_result = EXE_SUCCESS;
            bool call_executor = true;

            exe_request_result = exe_request(exe_p,   /* Executor handle. */
                                             EXE_CNUM, /* Executor request ID. */
                                             NULL,
                                             (void *) parser_p,  /* Parser status data. */
                                             &parser_p->request_handle);

            /* Sort out the answers from the executer. */
            result = parser_handle_exe_result(exe_request_result, parser_p);

        } else {
            exe_cnum_response_data_t *cnum_data_p = NULL;
            exe_cnum_record_data_t *tmp_rec_data_p = NULL;
            uint8_t i = 0;
            uint8_t j = 0;
            char tmp_buffer[20];

            if (NULL == message) {
                result = AT_ERROR;
                ATC_LOG_E("message is NULL");
                goto end;
            }

            if (EXE_SUCCESS != message->exe_result_code) {
                result = AT_CME_ERROR;
                Parser_SetCMEE_ErrorType(parser_p,
                                         CMEE_SIM_FAILURE);
                goto end;
            }

            cnum_data_p = (exe_cnum_response_data_t *) message->data_p;

            if (NULL == cnum_data_p) {
                result = AT_ERROR;
                ATC_LOG_E("cnum_data_p is NULL");
                goto end;
            }

            if (CMEE_OK != cnum_data_p->cmee_error_code) {
                result = AT_CME_ERROR;
                Parser_SetCMEE_ErrorType(parser_p,
                                         cnum_data_p->cmee_error_code);
                goto end;
            }

            tmp_rec_data_p = cnum_data_p->record_data_p;

            for (i = 0; i < cnum_data_p->number_of_records; i++) {

                char comma_buffer[3];
                uint16_t converted_characters = 0;
                uint8_t *alpha_result_p = NULL;
                size_t alpha_len = 0;
                atc_charset_t alpha_coding;

                memset(comma_buffer, 0, sizeof(comma_buffer));

                if (tmp_rec_data_p->alpha_id_actual_len > 0) {
                    /* Add alpha and number to output */
                    alpha_result_p = (uint8_t *)malloc(tmp_rec_data_p->alpha_id_actual_len * 2 + 1);

                    if (NULL == alpha_result_p) {
                        ATC_LOG_E("Memory allocation failed");
                        result = AT_ERROR;
                        goto end;
                    }

                    memset(alpha_result_p, 0, tmp_rec_data_p->alpha_id_actual_len * 2 + 1);

                    /* Alpha id needs to be converted to coding scheme set by AT+CSCS */
                    switch (tmp_rec_data_p->alpha_coding) {
                    case EXE_CHARSET_UCS2:
                        alpha_coding = ATC_CHARSET_UCS2;
                        break;
                    case EXE_CHARSET_GSM8:

                        if (parser_p->cscs_charset == ATC_CHARSET_HEX) {
                            alpha_coding = ATC_CHARSET_UNDEF_BINARY;
                        } else {
                            alpha_coding = ATC_CHARSET_GSM8;
                        }

                        break;
                    default:
                        ATC_LOG_E("Unsupported coding format");
                        result = AT_ERROR;
                        free(alpha_result_p);
                        goto end;
                    }

                    converted_characters = atc_string_convert(alpha_coding, tmp_rec_data_p->alpha_p,
                                           tmp_rec_data_p->alpha_id_actual_len,
                                           parser_p->cscs_charset, alpha_result_p, &alpha_len);

                    if (0 == converted_characters) {
                        ATC_LOG_E("String conversion failed");
                        result = AT_ERROR;
                        free(alpha_result_p);
                        goto end;
                    }
                } else {
                    /* Set an null string to print */
                    alpha_result_p = (uint8_t *)malloc(1);
                    *alpha_result_p = '\0';
                }

                rv = snprintf((char *)info_text,
                              RESULT_TEXT_SIZE,
                              ": \"%s\",\"%s\",",
                              alpha_result_p,
                              tmp_rec_data_p->number_p);

                free(alpha_result_p);
                alpha_result_p = NULL;

                /* Add type to output */
                sprintf((char *) tmp_buffer, "%d", tmp_rec_data_p->type);

                /* Check that the output string still fit the command line. */
                if ((strlen((char *)info_text) + strlen(tmp_buffer) + 1) < RESULT_TEXT_SIZE) {
                    strcat((char *)info_text, tmp_buffer);
                } else {
                    break;
                }

                /* Add speed to output */
                if (tmp_rec_data_p->speed <= 134) {
                    sprintf((char *)tmp_buffer, ",%d", tmp_rec_data_p->speed);

                    /* Check that the output string still fit the command line. */
                    if ((strlen((char *)info_text) + strlen(tmp_buffer) + 1) < RESULT_TEXT_SIZE) {
                        strcat((char *)info_text, tmp_buffer);
                    } else {
                        break;
                    }
                } else {
                    sprintf(comma_buffer, ",");
                }

                /* Add service to output */
                if (tmp_rec_data_p->service <= 5) {
                    sprintf((char *)tmp_buffer, "%s,%d", comma_buffer, tmp_rec_data_p->service);
                    memset(comma_buffer, 0, sizeof(comma_buffer));

                    /* Check that the output string still fit the command line. */
                    if ((strlen((char *)info_text) + strlen(tmp_buffer) + 1) < RESULT_TEXT_SIZE) {
                        strcat((char *)info_text, tmp_buffer);
                    } else {
                        break;
                    }
                } else {
                    sprintf(comma_buffer, ",");
                }

                /* Add itc to output */
                if (tmp_rec_data_p->itc <= 1) {
                    sprintf((char *)tmp_buffer, "%s,%d", comma_buffer, tmp_rec_data_p->itc);
                    memset(comma_buffer, 0, sizeof(comma_buffer));

                    /* Check that the output string still fit the command line. */
                    if ((strlen((char *)info_text) + strlen(tmp_buffer) + 1) < RESULT_TEXT_SIZE) {
                        strcat((char *)info_text, tmp_buffer);
                    } else {
                        break;
                    }
                }

                Parser_SendMultipleResponses(parser_p, AT_NO_OVERRIDE, info_text, (i + 1) == cnum_data_p->number_of_records);
                info_text[0] = 0;
                tmp_rec_data_p++;
            }
        }
    } /* End of AT_MODE_DO: */
    break;
    case AT_MODE_TEST: { /* AT+CNUM=? */
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
} /* End of AT_PLUS_CNUM_Handle */

/*
 *===============================================================================
 *
 *   Function: AT_PLUS_CCHO_Handle
 *
 *   INPUT:   parser_p  - Pointer to the current parser state.
 *            info_text - Pointer to a string buffer to put information text.
 *
 *   OUTPUT:  parser_p->
 *            Pos     - The current position in the command line.
 *
 *   RETURNS: A status code for the execution of the command is returned:
 *            AT_OK        - The command was successfully handled.
 *            AT_ERROR     - There where a general error.
 *            AT_CME_ERROR - There where a GSM specific error, the error code is
 *                           returned in info_text.
 *            AT_PENDING   - The command is not ready but returns while
 *                           waiting for an asynchronous response.
 *
 *   The CCHO command is used get a logical channel to the UICC card
 *
 *   SET  - open channel to UICC :
 *          +CCHO=<dfname>
 *            <dfname> is a string with a selectable application.
 *
 *   TEST - Gives OK if the command is supported
 *
 *===============================================================================
 */

AT_Command_e AT_PLUS_CCHO_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message)
{
    exe_request_result_t exe_result = EXE_FAILURE;
    AT_Command_e result = AT_ERROR;
    exe_cmee_error_t current_cmee = CMEE_UNKNOWN; /* GSM specific error code. */
    StrParam_t df_name_p = NULL;
    exe_t *exe_p = atc_get_exe_handle();
    bool error_flag = false;

    if (!parser_p->IsPending) {
        switch (parser_p->Mode) {
        case AT_MODE_SET: {
            exe_ccho_t exe_ccho_data;

            df_name_p = Parser_GetStrParam(parser_p, &error_flag, NULL);

            if (error_flag) {
                goto error;
            }

            if (!parser_p->NoMoreParams) {
                current_cmee = CMEE_INCORRECT_PARAMETERS;
                goto error;
            }

            exe_ccho_data.application_id_length = (strlen(df_name_p) / 2);
            exe_ccho_data.application_id_p
            = alloca(exe_ccho_data.application_id_length);

            /* Convert and check that the application id is ASCII in hex format  */
            if (false == convert_ascii_hex_to_bin_buf((char *) df_name_p,
                    exe_ccho_data.application_id_p,
                    exe_ccho_data.application_id_length)) {
                Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
                result = AT_CME_ERROR;
                goto error;
            }

            exe_result = exe_request(exe_p, EXE_CCHO, &exe_ccho_data,
                                     (void *) parser_p,
                                     &parser_p->request_handle);

            result = parser_handle_exe_result(exe_result, parser_p);
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
        if (NULL == message) {
            ATC_LOG_E("AT_PLUS_CCHO_Handle. message == NULL");
            goto error;
        }

        result = parser_handle_exe_result(message->exe_result_code, parser_p);

        if (AT_OK != result) {
            ATC_LOG_E("AT_PLUS_CCHO_Handle. Not EXE_SUCCESS ");
            goto error;
        }

        exe_ccho_response_t *channel_data_p = (exe_ccho_response_t *)(message->data_p);

        if (CMEE_OK == channel_data_p->error) {
            sprintf((char *) info_text, ": %d", channel_data_p->session_id);
        } else {
            current_cmee = channel_data_p->error;
            goto error;
        }

        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
    }

    return result;

error:

    Parser_SetCMEE_ErrorType(parser_p, current_cmee);
    return AT_CME_ERROR;

} /* End of AT_PLUS_CCHO_Handle */

#define ATC_SIM_IO_MAX_SESSION_ID 20
#define ATC_SIM_IO_MIN_SESSION_ID 1
#define ATC_SIM_IO_MIN_CGLA_SESSION_ID 0

/*
 *===============================================================================
 *
 *   Function: AT_PLUS_CCHC_Handle
 *
 *   INPUT:   parser_p  - Pointer to the current parser state.
 *            info_text - Pointer to a string buffer to put information text.
 *
 *   OUTPUT:  parser_p->
 *            Pos     - The current position in the command line.
 *
 *   RETURNS: A status code for the execution of the command is returned:
 *            AT_OK        - The command was successfully handled.
 *            AT_ERROR     - There where a general error.
 *            AT_CME_ERROR - There where a GSM specific error, the error code is
 *                           returned in info_text.
 *            AT_PENDING   - The command is not ready but returns while
 *                           waiting for an asynchronous response.
 *
 *   The CCHC command closes a logical channel to the UICC card
 *
 *   SET  - close channel to UICC :
 *          +CCHC=<sessionid>
 *            <sessionid> is a integer.
 *
 *   TEST - Gives OK if the command is supported
 *
 *===============================================================================
 */

AT_Command_e AT_PLUS_CCHC_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message)
{
    exe_request_result_t exe_result = EXE_FAILURE;
    AT_Command_e result = AT_ERROR;
    exe_cmee_error_t current_cmee = CMEE_UNKNOWN; /* GSM specific error code. */
    unsigned long default_session_id = 0;
    exe_t *exe_p = atc_get_exe_handle();
    bool error_flag = false;

    if (!parser_p->IsPending) {
        switch (parser_p->Mode) {
        case AT_MODE_SET: {
            exe_cchc_t exe_cchc_data;

            exe_cchc_data.session_id = (uint16_t) Parser_GetLongIntParam(parser_p,
                                       &error_flag,
                                       &default_session_id,
                                       0XFFFF);

            if (error_flag) {
                goto error;
            }

            if ((ATC_SIM_IO_MIN_SESSION_ID > exe_cchc_data.session_id)
                    || (ATC_SIM_IO_MAX_SESSION_ID < exe_cchc_data.session_id)) {
                current_cmee = CMEE_INCORRECT_PARAMETERS;
                goto error;
            }

            if (!parser_p->NoMoreParams) {
                current_cmee = CMEE_INCORRECT_PARAMETERS;
                goto error;
            }

            exe_result = exe_request(exe_p, EXE_CCHC, &exe_cchc_data,
                                     (void *) parser_p,
                                     &parser_p->request_handle);

            result = parser_handle_exe_result(exe_result, parser_p);
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
        if (NULL == message) {
            ATC_LOG_E("AT_PLUS_CCHC_Handle. message == NULL");
            goto error;
        }

        result = parser_handle_exe_result(message->exe_result_code, parser_p);

        if (AT_OK != result) {
            ATC_LOG_E("AT_PLUS_CCHC_Handle. Not EXE_SUCCESS ");
            goto error;
        }

        exe_cchc_response_t *channel_data_p = (exe_cchc_response_t *)(message->data_p);

        if (CMEE_OK != channel_data_p->error) {
            current_cmee = channel_data_p->error;
            goto error;
        }
    }

    return result;
error:

    Parser_SetCMEE_ErrorType(parser_p, current_cmee);
    return AT_CME_ERROR;

} /* End of AT_PLUS_CCHC_Handle */

/*
 *===============================================================================
 *
 *  Function: AT_PLUS_CGLA_Handle
 *
 *  INPUT: parser_p  - Pointer to the current Parser_p state.
 *         info_text  - Pointer to a string buffer to put information text.
 *         message   - Pointer to request response
 *
 *  OUTPUT:  parser_p->Pos  - The current position in the command line.
 *
 *  RETURNS: A status code for the execution of the command is returned:
 *           AT_OK     - The command was successfully handled.
 *           AT_ERROR  - There was a general error.
 *
 *  SET  - Call entered command on SIM Module
 *  TEST - Test if the command is supported
 *
 *===============================================================================
 */

AT_Command_e AT_PLUS_CGLA_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message)
{
    exe_request_result_t exe_result = EXE_FAILURE;
    AT_Command_e result = AT_ERROR;
    exe_cmee_error_t current_cmee = CMEE_UNKNOWN; /* GSM specific error code. */

    switch (parser_p->Mode) {
    case AT_MODE_SET: {
        if (!parser_p->IsPending) {
            const unsigned long min_length = 2;
            const unsigned long max_length = 522;
            StrParam_t command; /* Command from parser */
            IntParam_t session_id = 0; /* session id */
            unsigned long length; /* Length parsed from AT command - long type depends on Parser function */
            uint16_t cmd_length; /* Length of CGLA command - uint16 type depends on SIM Module function */
            bool error_flag = false;
            exe_t *exe_p = atc_get_exe_handle();
            exe_generic_sim_io_t exe_crla;

            session_id = Parser_GetIntParam(parser_p, &error_flag, NULL);

            if (error_flag) {
                current_cmee = CMEE_INCORRECT_PARAMETERS;
                goto error;
            }

            if ((ATC_SIM_IO_MIN_CGLA_SESSION_ID > session_id)
                    || (ATC_SIM_IO_MAX_SESSION_ID < session_id)) {
                current_cmee = CMEE_INCORRECT_PARAMETERS;
                goto error;
            }

            length = Parser_GetLongIntParam(parser_p, &error_flag, NULL,
                                            max_length);

            if (error_flag) {
                current_cmee = CMEE_INCORRECT_PARAMETERS;
                goto error;
            }

            if ((length < min_length) || (length > max_length)) {
                current_cmee = CMEE_INCORRECT_PARAMETERS;
                goto error;
            }

            cmd_length = (uint16_t) length;

            command = Parser_GetStrParam(parser_p, &error_flag, NULL);

            if (error_flag || !command || !strlen(command)) {
                current_cmee = CMEE_INCORRECT_PARAMETERS;
                goto error;
            }

            if (strlen(command) != cmd_length) {
                current_cmee = CMEE_INCORRECT_PARAMETERS;
                goto error;
            }

            if (!parser_p->NoMoreParams) {
                current_cmee = CMEE_INCORRECT_PARAMETERS;
                goto error;
            }

            exe_crla.apdu_length = (strlen(command) / 2);
            exe_crla.apdu_p = alloca(exe_crla.apdu_length);
            exe_crla.session_id = session_id;

            if (NULL == exe_crla.apdu_p) {
                ATC_LOG_E("Memory allocation failed");
                goto error;
            }

            if (false == convert_ascii_hex_to_bin_buf((char *)command, exe_crla.apdu_p, exe_crla.apdu_length)) {
                current_cmee = CMEE_INCORRECT_PARAMETERS;
                goto error;
            }

            exe_result = exe_request(exe_p, EXE_CGLA, &exe_crla,
                                     (void *) parser_p,
                                     &parser_p->request_handle);

            if (EXE_PENDING != exe_result) {
                goto error;
            }

            result = parser_handle_exe_result(exe_result, parser_p);
            break;

        } else { /* parser_p->IsPending */
            if (NULL == message) {
                ATC_LOG_E("message == NULL");
                goto error;
            }

            result = parser_handle_exe_result(message->exe_result_code, parser_p);

            if (AT_OK != result) {
                ATC_LOG_E("Not EXE_SUCCESS ");
                goto error;
            }

            exe_generic_sim_io_response_t *cgla_data_p = (exe_generic_sim_io_response_t *)(message->data_p);

            if (CMEE_OK != cgla_data_p->error) {
                current_cmee = cgla_data_p->error;
                goto error;
            }

            uint32_t i;
            char tmp_buffer[20];

            /* Lengths from SIM is number of bytes but we display the number of characters in the response */
            sprintf((char *) info_text, ": %d,\"", cgla_data_p->apdu_data_length * 2);

            if ((strlen((char *) info_text) + (2 * cgla_data_p->apdu_data_length) + 1) > RESULT_TEXT_SIZE) {
                ATC_LOG_E("The At command buffer is too small, needed length = %lu, bin length = %d",
                          (strlen((char *)info_text) + (2 * cgla_data_p->apdu_data_length) + 1),
                          cgla_data_p->apdu_data_length);
                Parser_SetCMEE_ErrorType(parser_p, CMEE_PHONE_FAILURE);
                return AT_CME_ERROR;
            }

            for (i = 0; i < cgla_data_p->apdu_data_length; i++) {
                tmp_buffer[0] = 0;
                sprintf((char *) tmp_buffer, "%02X", cgla_data_p->apdu_data_p[i]);
                strcat((char *) info_text, tmp_buffer);
            }

            strcat((char *) info_text, "\"");

            Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        }

        break;
    }
    case AT_MODE_TEST: {
        result = AT_OK;
        break;
    }
    default:
        result = AT_ERROR;
        break;
    }

    return result;

error:

    Parser_SetCMEE_ErrorType(parser_p, current_cmee);
    return AT_CME_ERROR;

}

#define ATC_CSIM_SESSION_ID 0
/*
*===============================================================================
*
*  Function: AT_PLUS_CSIM_Handle
*
*  INPUT: parser_p  - Pointer to the current Parser_p state.
*         info_text  - Pointer to a string buffer to put information text.
*         message   - Pointer to request response
*
*  OUTPUT:  parser_p->Pos  - The current position in the command line.
*
*  RETURNS: A status code for the execution of the command is returned:
*           AT_OK     - The command was successfully handled.
*           AT_ERROR  - There was a general error.
*
*  The CSIM provides generic SIM Access
*
*  SET  - Call entered command on SIM Module
*  TEST - Test if the command is supported
*
*===============================================================================
*/

AT_Command_e AT_PLUS_CSIM_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message)
{
    exe_request_result_t exe_result = EXE_FAILURE;
    AT_Command_e result = AT_ERROR;
    exe_cmee_error_t current_cmee = CMEE_UNKNOWN; /* GSM specific error code. */
    bool continue_command = false;
    exe_t *exe_p = atc_get_exe_handle();

    switch (parser_p->Mode) {
    case AT_MODE_SET: {
        if (!parser_p->IsPending) {
            const unsigned long min_length = 2;
            const unsigned long max_length = 522;
            StrParam_t command; /* Command from parser */
            unsigned long length; /* Length parsed from AT command - long type depends on PAS Parser function */
            uint16_t cmd_length; /* Length of CSIM command - uint16 type depends on SIM Module function */
            bool error_flag = false;
            exe_generic_sim_io_t exe_csim;

            length = Parser_GetLongIntParam(parser_p, &error_flag, NULL,
                                            max_length);

            if (error_flag) {
                current_cmee = CMEE_INCORRECT_PARAMETERS;
                goto error;
            }

            if ((length < min_length) || (length > max_length)) {
                current_cmee = CMEE_INCORRECT_PARAMETERS;
                goto error;
            }

            cmd_length = (uint16_t) length;
            command = Parser_GetStrParam(parser_p, &error_flag, NULL);

            if (error_flag || !command || !strlen(command)) {
                current_cmee = CMEE_INCORRECT_PARAMETERS;
                goto error;
            }

            if (strlen(command) != cmd_length) {
                current_cmee = CMEE_INCORRECT_PARAMETERS;
                goto error;
            }

            if (!parser_p->NoMoreParams) {
                current_cmee = CMEE_INCORRECT_PARAMETERS;
                goto error;
            }

            exe_csim.apdu_length = (strlen(command) / 2);
            exe_csim.apdu_p = alloca(exe_csim.apdu_length);
            exe_csim.session_id = ATC_CSIM_SESSION_ID;

            if (NULL == exe_csim.apdu_p) {
                ATC_LOG_E("Memory allocation failed");
                goto error;
            }

            if (false == convert_ascii_hex_to_bin_buf((char *)command, exe_csim.apdu_p, exe_csim.apdu_length)) {
                current_cmee = CMEE_INCORRECT_PARAMETERS;
                goto error;
            }

            exe_result = exe_request(exe_p, EXE_CSIM, &exe_csim,
                                     (void *) parser_p,
                                     &parser_p->request_handle);

            if (EXE_PENDING != exe_result) {
                goto error;
            }

            result = parser_handle_exe_result(exe_result, parser_p);
            break;

        } else { /* parser_p->IsPending */
            switch (parser_p->Mode) {
            case AT_MODE_SET: {
                if (EXE_SUCCESS != message->exe_result_code) {
                    ATC_LOG_E("Not EXE_SUCCESS ");
                    goto error;
                    break;
                }

                exe_generic_sim_io_response_t *csim_data_p = (exe_generic_sim_io_response_t *)(message->data_p);

                if (CMEE_OK == csim_data_p->error) {
                    uint32_t i;
                    char tmp_buffer[20];

                    /* Lengths from SIM is number of bytes but we display the number of characters in the response */
                    sprintf((char *)info_text, ": %d,\"", csim_data_p->apdu_data_length * 2);

                    if ((strlen((char *)info_text) + (2 * csim_data_p->apdu_data_length) + 1) > RESULT_TEXT_SIZE) {
                        ATC_LOG_E("The At command buffer is too small needed length = %lu, bin length = %d",
                                  (strlen((char *)info_text) + (2 * csim_data_p->apdu_data_length) + 1),
                                  csim_data_p->apdu_data_length);
                        Parser_SetCMEE_ErrorType(parser_p, CMEE_PHONE_FAILURE);
                        return AT_CME_ERROR;
                    }

                    for (i = 0; i < csim_data_p->apdu_data_length; i++) {
                        tmp_buffer[0] = 0;
                        sprintf((char *)tmp_buffer, "%02X", csim_data_p->apdu_data_p[i]);
                        strcat((char *)info_text, tmp_buffer);
                    }

                    strcat((char *)info_text, "\"");

                } else {
                    current_cmee = csim_data_p->error;
                    ATC_LOG_E("SIM fail ");
                    goto error;
                }

                Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
                result = parser_handle_exe_result(message->exe_result_code, parser_p);
                break;
            }
            default:
                goto error;
                break;
            }
        }

        break;
    }
    case AT_MODE_TEST: {
        result = AT_OK;
        break;
    }
    default:
        result = AT_ERROR;
        break;
    }

    return result;

error:

    Parser_SetCMEE_ErrorType(parser_p, current_cmee);
    return AT_CME_ERROR;

}

/*
 *===============================================================================
 *
 *   Function: AT_STAR_EFSIM_Handle
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
 *   The EFSIM command is used for retrieving status / content of the current SIM.
 *
 *   READ - Returns information about the current SIM in XML format:
 *
 *   TEST - returns OK if the command is supported
 *
 *===============================================================================
 */

AT_Command_e AT_STAR_EFSIM_Handle(AT_ParserState_s *parser_p,
                                  AT_CommandLine_t info_text,
                                  AT_ResponseMessage_t message)
{
    exe_t *exe_p;
    exe_request_result_t result = EXE_SUCCESS;
    exe_p = atc_get_exe_handle();
    exe_cmee_error_t current_cmee = CMEE_OK; /* GSM specific error code. */

    switch (parser_p->Mode) {
    case AT_MODE_READ: {
        if (!parser_p->IsPending) {
            result = exe_request(exe_p, EXE_EFSIM, NULL,
                                 (void *)parser_p, &parser_p->request_handle);

            if (EXE_PENDING != result) {
                current_cmee = CMEE_SIM_FAILURE;
                goto error;
            }

            return AT_PENDING;
        } else { /* IsPending */

            if (NULL == message->data_p) {
                current_cmee = CMEE_SIM_FAILURE;
                goto error;
            }

            exe_full_sim_t *full_sim_data_p = message->data_p;

            sprintf((char *) info_text, "%s", full_sim_data_p->state_content_p);
            Parser_SendResponse(parser_p, AT_TEXT_RESPONSE, info_text);
            return AT_OK;
        }

        break;
    }
    case AT_MODE_TEST: {
        return AT_OK;
    }
    default:
        current_cmee = CMEE_UNKNOWN;
        break;
    }

error:

    if (CMEE_OK != current_cmee) {
        Parser_SetCMEE_ErrorType(parser_p, current_cmee);
    }

    return AT_CME_ERROR;
}

/*
*===============================================================================
*
*   Function: AT_PLUS_CUAD_Handle
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
*
*
*   CUAD = UICC Application Discovery
*
*   This command asks the MT to discover what applications are available for
*   selection on the UICC.
*   Each record contains the AID and optionally application parameters of one of
*   the applications available on the UICC.
*
*===============================================================================
*/
AT_Command_e AT_PLUS_CUAD_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message)
{
    exe_t *exe_p;
    exe_request_result_t result = EXE_SUCCESS;

    switch (parser_p->Mode) {
    case AT_MODE_DO: {
        if (!parser_p->IsPending) {
            exe_p = atc_get_exe_handle();

            result = exe_request(exe_p, EXE_CUAD_APPS, NULL, (void *) parser_p,
                                 &parser_p->request_handle);

            if (EXE_PENDING == result) {
                return AT_PENDING;
            } else {
                Parser_SetCMEE_ErrorType(parser_p, CMEE_OPERATION_NOT_ALLOWED);
                return AT_CME_ERROR;
            }

        } else { /* IsPending handling */
            exe_cuad_response_t *cuad_data_p = (exe_cuad_response_t *)(message->data_p);
            int i = 0;

            if (NULL == cuad_data_p) {
                ATC_LOG_E("cuad_data_p is NULL");
                return AT_ERROR;
            }

            if (CMEE_OK != cuad_data_p->error) {
                Parser_SetCMEE_ErrorType(parser_p, CMEE_OPERATION_NOT_ALLOWED);
                return AT_CME_ERROR;
            }

            if (0 == cuad_data_p->number_of_apps) {
                return AT_OK;
            }

            for (i = 0; i < cuad_data_p->number_of_apps; ++i) {
                int j;
                char tmp_buffer[20];

                sprintf((char *) info_text, ": \"");

                if ((strlen((char *) info_text) + (2 * cuad_data_p->apps[i]->length) + 1)
                        > RESULT_TEXT_SIZE) {
                    ATC_LOG_E("ERROR The At command buffer is to small."
                              " Needed length = %d, bin length =%d",
                              (int)(strlen((char *)info_text) + (2 * cuad_data_p->apps[i]->length) + 1),
                              cuad_data_p->apps[i]->length);
                    Parser_SetCMEE_ErrorType(parser_p, CMEE_PHONE_FAILURE);
                    return AT_CME_ERROR;
                }

                for (j = 0; j < cuad_data_p->apps[i]->length; j++) {
                    tmp_buffer[0] = 0;
                    sprintf((char *) tmp_buffer, "%02X", cuad_data_p->apps[i]->sim_io_response_p[j]);
                    strcat((char *) info_text, tmp_buffer);
                }

                strcat((char *) info_text, "\"");

                Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
            }

            break;
        }
    }
    case AT_MODE_TEST: {
        return AT_OK;
        break;
    }
    default:
        return AT_ERROR;
    }

    return AT_OK;
}

