/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#include <atc_config.h>
#include <atc_exe_glue.h>
#include <atc_handlers.h>
#include <atc_log.h>
#include <atc_parser.h>
#include <atc_parser_util.h>
#include <exe.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define SET_RESULT_AND_GOTO_EXIT(value) result = (value); goto exit;

#define EEMPAGE_FIRST_PAGE          1
#define EEMPAGE_LAST_PAGE           5
#define EEMPAGE_DEFAULT_PAGE        1
#define EEMPAGE_DEFAULT_TIMERVALUE 10


/* Note: The paths are purposely defined as pointers and not as preprocessor macros.
 *       Internal reasons requires this to be assignable.
 */
char *atc_revision_identification_path_p = (char *) "/etc/AT/system_id.cfg";
char *atc_model_identification_path_p = (char *) "/etc/AT/model_id.cfg";
char *atc_manufacturer_identification_path_p = (char *) "/etc/AT/manuf_id.cfg";

/*
 * Note: This is a handler for CFUN sent in from outside the ME.
 *       This code should probably be moved somewhere else. Need
 *       to be able to have more than one command table before
 *       we move the code
 *
 */
AT_Command_e AT_PLUS_CFUN_Handle_external(AT_ParserState_s *parser_p, AT_CommandLine_t info_text, AT_ResponseMessage_t message)
{
    /* Cannot be called in pending state */
    switch (parser_p->Mode) {
    case AT_MODE_SET: {
        bool ErrorFlag = false;
        bool OnlyOneParam = false;
        IntParam_t fun = 0; /* Default is to turn off the ME */
        IntParam_t rst = 0; /* Default is to not restart the ME */
        fun = Parser_GetIntParam(parser_p, &ErrorFlag, &fun);

        if (parser_p->NoMoreParams) {
            OnlyOneParam = true;
        } else {
            rst = Parser_GetIntParam(parser_p, &ErrorFlag, &rst);
        }

        if (ErrorFlag) {
            goto exit;
        }

        /* When received from the outside CFUN
         *  is sent up to application level
         * (atservice in java in the Android case).
         */

        break;
    }

    case AT_MODE_READ:
        break;

    case AT_MODE_TEST:
        break;
    }

    /* handle +CFUN in AtService */
    parser_p->Command = AT_UNKNOWN_COMMAND;

exit:
    return AT_ERROR;

}

/*
 *===============================================================================
 *
 *   Function: AT_PLUS_CFUN_Handle
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
 *   The +CFUN command selects the level of functionality in the ME. Level
 *   'full functionality' is where the highest level of power is drawn.
 *   'Minimum functionality' is where minimum power is drawn.
 *
 *   SET  - Sets the level of functionality
 *   READ - Writes the current functionality level to info_text.
 *   TEST - Writes the supported functionality levels to info_text.
 *
 *===============================================================================
 */
AT_Command_e AT_PLUS_CFUN_Handle(AT_ParserState_s *parser_p, AT_CommandLine_t info_text, AT_ResponseMessage_t message)
{
    AT_Command_e result = AT_OK;
    exe_request_result_t exe_res = EXE_FAILURE;
    exe_cfun_request_t request_data;

    if (parser_p->connection_type == ATC_CONNECTION_TYPE_EXTERNAL) {
        result = AT_PLUS_CFUN_Handle_external(parser_p, info_text, message);
        goto exit;
    }

    switch (parser_p->Mode) {
    case AT_MODE_SET:

        if (!parser_p->IsPending) {
            bool error_flag = false;
            IntParam_t fun = 0; /* Default is to turn off the ME */
            exe_t *exe_p;
            fun = Parser_GetIntParam(parser_p, &error_flag, &fun);
            exe_p = atc_get_exe_handle();

            if (error_flag || !parser_p->NoMoreParams) {
                Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
                result = AT_CME_ERROR;
                goto exit;
            }

            switch (fun) {
            case 99: /* Full functionality, latest setting or modem default */

                if (EXE_PENDING == exe_request(exe_p, EXE_CFUN_RADIO_ON, NULL,
                                               (void *)parser_p, &parser_p->request_handle)) {
                    result = AT_PENDING;
                } else {
                    Parser_SetCMEE_ErrorType(parser_p, CMEE_PHONE_FAILURE);
                    result = AT_CME_ERROR;
                }

                break;

            case 100: /* Prepare modem for shutdown */

                if (EXE_PENDING == exe_request(exe_p, EXE_CFUN_MODEM_SHUTDOWN, NULL,
                                               (void *)parser_p, &parser_p->request_handle)) {
                    result = AT_PENDING;
                } else {
                    Parser_SetCMEE_ErrorType(parser_p, CMEE_PHONE_FAILURE);
                    result = AT_CME_ERROR;
                }

                break;

            case 4: /* Disable the phone's transmit and receive RF circuits */

                if (EXE_PENDING == exe_request(exe_p, EXE_CFUN_RADIO_OFF, NULL,
                                               (void *)parser_p, &parser_p->request_handle)) {
                    result = AT_PENDING;
                } else {
                    Parser_SetCMEE_ErrorType(parser_p, CMEE_PHONE_FAILURE);
                    result = AT_CME_ERROR;
                }

                break;

            case 5:
                /* GSM only (WCDMA radio is off) */
                request_data.network = EXE_NETWORK_GSM_ONLY;
                goto set_network;
            case 6:
                /* WCDMA only (GSM radio is off) */
                request_data.network = EXE_NETWORK_WCDMA_ONLY;
                goto set_network;
            case 7:
                /* WCDMA preferred or GSM */
                request_data.network = EXE_NETWORK_WCDMA_GSM_PREFERRED_WCDMA;
set_network:
                exe_res = exe_request(exe_p, EXE_CFUN_SET_NW, &request_data,
                                      (void *)parser_p, &parser_p->request_handle);

                result = parser_handle_exe_result(exe_res, parser_p);

                if (AT_CME_ERROR == result) {
                    /* Non default CMEE error codes for CFUN */
                    if (EXE_NOT_SUPPORTED == exe_res) {
                        Parser_SetCMEE_ErrorType(parser_p, CMEE_NO_NETWORK_SERVICE);
                    } else {
                        Parser_SetCMEE_ErrorType(parser_p, CMEE_PHONE_FAILURE);
                    }
                }

                break;

            case 0: /* Minimum functionality */
            case 1: /* Not supported */
            case 2: /* Not supported */
            case 3: /* Not supported */
                Parser_SetCMEE_ErrorType(parser_p, CMEE_OPERATION_NOT_SUPPORTED);
                result = AT_CME_ERROR;
                break;

            default:
                result = AT_ERROR;
            }
        } else { /* We have a result...*/
            if (message->exe_result_code != EXE_SUCCESS) {
                Parser_SetCMEE_ErrorType(parser_p, CMEE_OPERATION_NOT_ALLOWED);
                result = AT_CME_ERROR;
            }
        }

        break;

    case AT_MODE_READ:
        result = AT_ERROR; /* For now */
        bool error_flag = false;
        exe_t *exe_p;
        exe_p = atc_get_exe_handle();

        if (!parser_p->IsPending) {
            exe_res = exe_request(exe_p, EXE_CFUN_READ, NULL,
                                  (void *)parser_p, &parser_p->request_handle);

            switch (exe_res) {
                /* CMEE error values different form the ones in parser_handle_exe_result */
            case EXE_PENDING:
                result = AT_PENDING;
                break;

            case EXE_NOT_SUPPORTED:
                Parser_SetCMEE_ErrorType(parser_p, CMEE_NO_NETWORK_SERVICE);
                result = AT_CME_ERROR;
                break;

            default:
                Parser_SetCMEE_ErrorType(parser_p, CMEE_PHONE_FAILURE);
                result = AT_CME_ERROR;
                break;
            }
        } else {  /* We have a result...*/
            exe_cfun_response_t *response_p = (exe_cfun_response_t *) message->data_p;

            if (message->exe_result_code == EXE_SUCCESS && NULL != response_p) {
                int fun = response_p->value;

                if (fun >= 4 && fun <= 7) {
                    sprintf((char *) info_text, ": %d", fun);
                    Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
                    result = AT_OK;
                }
            }

            if (!AT_OK) {
                Parser_SetCMEE_ErrorType(parser_p, CMEE_OPERATION_NOT_ALLOWED);
                result = AT_CME_ERROR;
            }
        }

        break;

    case AT_MODE_TEST:
        sprintf((char *)info_text, ": (4,5,6,7,99)");
        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        break;
    }

exit:
    return result;
}

/*
 *===============================================================================
 *
 *   Function: AT_PLUS_GCAP_Handle
 *
 *   INPUT:   parser_p  - Pointer to the current parser state.
 *            info_text - Pointer to a string buffer to put information text.
 *
 *   OUTPUT:  parser_p->
 *              Pos     - The current position in the command line.
 *
 *   RETURNS: A list with the supported capabilities. The only supported capacity is +CGSM.
 *
 *
 *   DO   - Request complete capabilities list.
 *
 *   TEST - Test command to show if the command is supported.
 *
 *===============================================================================
 */
AT_Command_e AT_PLUS_GCAP_Handle(AT_ParserState_s *parser_p, AT_CommandLine_t info_text, AT_ResponseMessage_t message)
{
    AT_Command_e result = AT_ERROR;

    switch (parser_p->Mode) {
    case AT_MODE_DO:
        *info_text = END_OF_STRING;
        sprintf((char *)info_text, ": +CGSM");
        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        result = AT_OK;
        break;
    case AT_MODE_TEST:
        result = AT_OK;
        break;
    }

    return result;
}

/*
 *===============================================================================
 *
 *   Function: handle_read_manufacturer_information
 *
 *   INPUT:   parser_p  - Pointer to the current parser state.
 *            info_text - Pointer to a string buffer to put information text.
 *            message   - Used when call back from handling PENDING.
 *            path_p    - File path to the requested manufacture information.
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
 *   DESCRIPTION: Reads a manufacture identification file.
 *
 *   DO - Read the requested manufacture information file.
 *
 *===============================================================================
 */
AT_Command_e handle_manufacturer_information(AT_ParserState_s *parser_p,
        AT_CommandLine_t info_text,
        AT_ResponseMessage_t message,
        char *path_p)
{
    AT_Command_e result = AT_ERROR;

    switch (parser_p->Mode) {

    case AT_MODE_DO: {
        FILE *file_p = NULL;

        if (NULL == path_p) {
            ATC_LOG_E("handle_manufacturer_information(): initial path is NULL");
            break;
        }

        file_p = fopen(path_p, "r");

        sprintf((char *)info_text, "\"");

        if (NULL != file_p) {
            int32_t rc = fread((info_text + 1), 1, RESULT_TEXT_SIZE - 1, file_p);

            if (rc > 0) {
                strcat((char *)info_text, "\"");

                Parser_SendResponse(parser_p, AT_TEXT_RESPONSE, info_text);
                result = AT_OK;
            } else {
                char *reason_p = strerror(errno); /* returns NULL on some systems */

                if (NULL != reason_p) {
                    ATC_LOG_E("handle_manufacturer_information(): fread failed:%s", reason_p);
                } else {
                    ATC_LOG_E("handle_manufacturer_information(): fread failed: unknown reason");
                }

                Parser_SetCMEE_ErrorType(parser_p, CMEE_UNKNOWN);
                result = AT_CME_ERROR;
            }

            fclose(file_p);
        } else {
            char *reason_p = strerror(errno); /* returns NULL on some systems */

            if (NULL != reason_p) {
                ATC_LOG_E("handle_manufacturer_information(): fopen failed:%s", reason_p);
            } else {
                ATC_LOG_E("handle_manufacturer_information(): fopen failed: unknown reason");
            }

            Parser_SetCMEE_ErrorType(parser_p, CMEE_UNKNOWN);
            result = AT_CME_ERROR;
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

/*
 *===============================================================================
 *
 *   Function: AT_PLUS_CGMR_Handle
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
 *   The +CGMR command retrieves MS revision information.
 *
 *   DO   - Request Revision Information
 *          +CGMR
 *
 *   READ - Not applicable
 *
 *   TEST - AT_OK
 *
 *===============================================================================
 */
AT_Command_e AT_PLUS_CGMR_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message)
{
    return handle_manufacturer_information(parser_p,
                                           info_text,
                                           message,
                                           atc_revision_identification_path_p);
}

/*
 *===============================================================================
 *
 *   Function: AT_PLUS_CGMI_Handle
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
 *   DESCRIPTION: The +CGMI command reads the manufacture identification.
 *
 *   DO - Read the manufacture identification.
 *
 *===============================================================================
 */
AT_Command_e AT_PLUS_CGMI_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message)
{
    return handle_manufacturer_information(parser_p,
                                           info_text,
                                           message,
                                           atc_manufacturer_identification_path_p);
}

/*
 *===============================================================================
 *
 *   Function: AT_PLUS_CGMM_Handle
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
 *   DESCRIPTION: The +CGMM command reads the model identification.
 *
 *   DO - Read the model identification.
 *
 *===============================================================================
 */
AT_Command_e AT_PLUS_CGMM_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message)
{
    return handle_manufacturer_information(parser_p,
                                           info_text,
                                           message,
                                           atc_model_identification_path_p);
}

/*
 *===============================================================================
 *
 *   Function: AT_PLUS_CGSN_Handle
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
 *   The +CGSN command retrieves IMEI.
 *
 *   DO   - Request Product Serial Number Identification
 *          +CGSN
 *
 *   READ - Not applicable
 *
 *   TEST - AT_OK
 *
 *===============================================================================
 */
AT_Command_e AT_PLUS_CGSN_Handle(AT_ParserState_s    *parser_p,
                                 AT_CommandLine_t     info_text,
                                 AT_ResponseMessage_t message)
{
    AT_Command_e result = AT_OK;
    exe_t *exe_p;
    int i;
    exe_cgsn_response_t cgsn_res;
    exe_request_result_t exe_res = EXE_FAILURE;

    switch (parser_p->Mode) {
    case AT_MODE_DO:
        exe_p = atc_get_exe_handle();
        exe_res = exe_request(exe_p, EXE_CGSN, (void *) &cgsn_res,
                              (void *)parser_p, &parser_p->request_handle);

        if (EXE_SUCCESS == exe_res) {
            sprintf((char *) info_text, "%s", &cgsn_res.imei[0]);
            result = AT_OK;
            Parser_SendResponse(parser_p, AT_TEXT_RESPONSE, info_text);
        } else {
            result = AT_CME_ERROR;
            Parser_SetCMEE_ErrorType(parser_p, CMEE_PHONE_FAILURE);
        }

        break;

    case AT_MODE_TEST:
        break;
    }

    return result;
}


/*
 *===============================================================================
 *
 *   Function: AT_STAR_ESVN_Handle
 *
 *   INPUT:   parser_p       - Pointer to the current parser state.
 *            info_text      - Pointer to a string buffer to put information text.
 *
 *   OUTPUT:  parser_p->Pos  - The current position in the command line.
 *
 *   RETURNS: A status code for the execution of the command is returned:
 *            AT_OK        - The command was successfully handled.
 *            AT_ERROR     - There was a general error.
 *            AT_CME_ERROR - There was a GSM specific error, the error code is
 *                           returned in ASCII format in info_text.
 *
 *   The ESVN reads the IMEISV value stored in the modem. The set functionality
 *   is not supported.
 *
 *   SET  - Not supported.
 *   READ - Reads IMEISV from the modem.
 *   TEST - NA (Show list of supported <imeisv>s).
 *
 *===============================================================================
 */
AT_Command_e AT_STAR_ESVN_Handle(AT_ParserState_s *parser_p, AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message)
{
    AT_Command_e result = AT_OK;

    switch (parser_p->Mode) {
    case AT_MODE_READ: {
        if (!parser_p->IsPending) {
            exe_request_result_t exe_res = EXE_FAILURE;

            exe_res = exe_request(atc_get_exe_handle(), EXE_ESVN_READ, NULL,
                                  (void *)parser_p, &parser_p->request_handle);

            result = parser_handle_exe_result(exe_res, parser_p);
        } else {
            if (message->exe_result_code == EXE_SUCCESS) {
                exe_esvn_response_t *esvn_response_p = (exe_esvn_response_t *)message->data_p;
                sprintf((char *) info_text, ": %d", esvn_response_p->imeisv);
                result = AT_OK;
                Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
            } else {
                result = AT_CME_ERROR;
                Parser_SetCMEE_ErrorType(parser_p, CMEE_PHONE_FAILURE);
            }
        }

        break;
    }

    default:
        Parser_SetCMEE_ErrorType(parser_p, CMEE_OPERATION_NOT_SUPPORTED);
        result = AT_CME_ERROR;
        break;
    }

    return result;
}


/*
 *===============================================================================
 *
 *   Function: AT_STAR_ECAM_Handle
 *
 *   INPUT:   parser_p       - Pointer to the current parser state.
 *            info_text      - Pointer to a string buffer to put information text.
 *
 *   OUTPUT:  parser_p->Pos  - The current position in the command line.
 *
 *   RETURNS: A status code for the execution of the command is returned:
 *            AT_OK        - The command was successfully handled.
 *            AT_ERROR     - There was a general error.
 *            AT_CME_ERROR - There was a GSM specific error, the error code is
 *                           returned in ASCII format in info_text.
 *
 *   The ECAM turns on/off the extended call log function in the ME
 *   (SIR2.0:S9/C/E PC1).
 *
 *   SET  - Activates/Deactivates the call monitoring function in the ME.
 *   READ - Shows the current status for "Call Monitoring".
 *   TEST - Show list of supported <onoff>s.
 *
 *===============================================================================
 */
AT_Command_e AT_STAR_ECAM_Handle(AT_ParserState_s *parser_p, AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message)
{
    enum {
        DISABLED = 0,
        ENABLED = 1
    };
    AT_Command_e result = AT_OK;

    switch (parser_p->Mode) {
    case AT_MODE_SET: {
        IntParam_t ECAM_OnOff = DISABLED;
        bool ErrorFlag = false; /* Flags a parameter parsing error. */
        ECAM_OnOff = Parser_GetIntParam(parser_p, &ErrorFlag, &ECAM_OnOff);

        if ((!parser_p->NoMoreParams) || ErrorFlag) {
            result = AT_ERROR;
            break;
        }

        /* Check if the value of received parameter is correct,
         e.g. received value is not out of range. */
        if ((ENABLED == ECAM_OnOff) || (DISABLED == ECAM_OnOff)) {
            parser_p->ECAM = ECAM_OnOff;
            /* Note: No *ECAV is sent at the enabling of *ECAM (contrary to
             * the specification's suggested).
             */
        } else {
            Parser_SetCMEE_ErrorType(parser_p, CMEE_INVALID_INDEX);
            result = AT_CME_ERROR;
        }

        break;
    }
    case AT_MODE_READ: {
        IntParam_t ECAM_OnOff;
        ECAM_OnOff = (IntParam_t)parser_p->ECAM;

        if ((ENABLED == ECAM_OnOff) || (DISABLED == ECAM_OnOff)) {
            sprintf((char *)info_text, ": %d", ECAM_OnOff);
            Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        } else {
            result = AT_ERROR;
        }

        break;
    }
    case AT_MODE_TEST:
        /* Send the range of supported values. */
        sprintf((char *)info_text, ": (%d-%d)", DISABLED, ENABLED);
        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        break;
    default:
        result = AT_CME_ERROR;
        break;
    }

    return result;
}

/*
 *===============================================================================
 *
 *   Function: AT_STAR_ERFSTATE_Handle
 *
 *   INPUT:   parser_p       - Pointer to the current parser state.
 *            info_text      - Pointer to a string buffer to put information text.
 *
 *   OUTPUT:  parser_p->Pos  - The current position in the command line.
 *
 *   RETURNS: A status code for the execution of the command is returned:
 *            AT_OK        - The command was successfully handled.
 *            AT_ERROR     - There was a general error.
 *            AT_CME_ERROR - There was a GSM specific error, the error code is
 *                           returned in ASCII format in info_text.
 *
 *   The ERFSTATEE turns on/off reporting of RF state unsolicited events.
 *
 *   SET  - Activates/Deactivates unsolicited responses.
 *   READ - Shows the current status for CME error unsolicited error reporting.
 *   TEST - Show list of supported set values.
 *
 *===============================================================================
 */
AT_Command_e AT_STAR_ERFSTATE_Handle(AT_ParserState_s *parser_p, AT_CommandLine_t info_text,
                                     AT_ResponseMessage_t message)
{
    enum {
        DISABLED = 0,
        ENABLED = 1
    };
    AT_Command_e result = AT_OK;

    switch (parser_p->Mode) {
    case AT_MODE_SET: {
        IntParam_t ERFSTATE_OnOff = ENABLED;
        bool ErrorFlag = false; /* Flags a parameter parsing error. */

        if (!parser_p->NoMoreParams) {
            ERFSTATE_OnOff = Parser_GetIntParam(parser_p, &ErrorFlag, NULL);
        } else {
            goto error;
        }

        if ((!parser_p->NoMoreParams) || ErrorFlag) {
            result = AT_ERROR;
            break;
        }

        /* Check if the value of received parameter is correct,
         * e.g. received value is not out of range. */
        if ((ENABLED == ERFSTATE_OnOff) || (DISABLED == ERFSTATE_OnOff)) {
            parser_p->ERFSTATE = ERFSTATE_OnOff;
        } else {
            Parser_SetCMEE_ErrorType(parser_p, CMEE_INVALID_INDEX);
            result = AT_CME_ERROR;
        }

        break;
    }
    case AT_MODE_READ: {
        IntParam_t ERFSTATE_OnOff;
        ERFSTATE_OnOff = (IntParam_t)parser_p->ERFSTATE;

        if ((ENABLED == ERFSTATE_OnOff) || (DISABLED == ERFSTATE_OnOff)) {
            sprintf((char *)info_text, ": %d", ERFSTATE_OnOff);
            Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        } else {
            result = AT_ERROR;
        }

        break;
    }

    case AT_MODE_TEST:
        /* Send the range of supported values. */
        sprintf((char *)info_text, ": (%d-%d)", DISABLED, ENABLED);
        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        break;
    default:
        result = AT_CME_ERROR;
        break;
    }

    return result;

error:
    return AT_ERROR;
}

/*
 * Handle ERFSTATE events sent from modem. Compose an AT string and send the unsolicited.
 */
void handle_incoming_erfstate_event(void *data_p)
{
    char text[20];
    exe_erfstate_response_t *response_p = (exe_erfstate_response_t *)data_p;
    AT_ParserState_s    *parser_p;
    unsigned char       entry;

    sprintf(text, "*ERFSTATE: %d", response_p->radio_state);

    /* Loop through all parser states and send to all that have ERFSTATE set to true */
    for (entry = 0; entry < MAX_NO_OF_OPEN_CHANNELS; entry++) {
        parser_p = ParserStateTable_GetParserState_ByEntry(entry);

        if (parser_p->ERFSTATE) {
            parser_send_unsolicited(parser_p, AT_TEXT_RESPONSE, (AT_CommandLine_t)text);
        }
    }
}

/*
*===============================================================================
*
*   Function: AT_PLUS_CMEC_Handle
*
*   INPUT:   parser_p   - Pointer to the current parser state.
*            info_text      - Pointer to a string buffer to put information text.
*
*   OUTPUT:  message    - Message to be printed as result
*
*   RETURNS: A status code for the execution of the command is returned:
*            AT_OK        - The command was successfully handled.
*            AT_ERROR     - There was a general error.
*            AT_CME_ERROR - There was a GSM specific error, the error code is
*                           returned in InfoText.
*
*   The CMEC function is used to select whether MT or AT commands (or both)
*   are to control keypad, display and indicators.
*    keypad     : either the MT keypad or +CKPD  (or both)
*    display    : either the MT's internal display functions or +CDIS  (or both)
*    indicators : either MT updates indicators (like battery status etc.)
*                 or they may be set with +CIND (or both)
*
*   Only MT access and CKPD is supported for this implementation.
*
*   SET   - Performs the selection
*   TEST  - Returns supported values  according to GSM 07.10
*
*===============================================================================
*/
AT_Command_e AT_PLUS_CMEC_Handle(AT_ParserState_s *parser_p, AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message)
{
    AT_Command_e result = AT_OK;

    switch (parser_p->Mode) {
    case AT_MODE_SET: {
        IntParam_t keyp = 2;
        IntParam_t disp = 0;
        IntParam_t ind = 0;
        bool error_flag = false; /* Flags a parameter parsing error. */
        keyp = Parser_GetIntParam(parser_p, &error_flag, &keyp);

        if (error_flag) {
            result = AT_ERROR;
            break;
        }

        disp = Parser_GetIntParam(parser_p, &error_flag, &disp);

        if (error_flag) {
            result = AT_ERROR;
            break;
        }

        ind = Parser_GetIntParam(parser_p, &error_flag, &ind);

        if ((!parser_p->NoMoreParams) || error_flag) {
            result = AT_ERROR;
            break;
        }

        if (2 == keyp && 0 == disp && 0 == ind) {
            result = AT_OK;
        } else {
            Parser_SetCMEE_ErrorType(parser_p, CMEE_OPERATION_NOT_SUPPORTED);
            result = AT_CME_ERROR;
        }

        break;
    }
    case AT_MODE_READ: {
        (void)sprintf((char *)info_text, ": 2,0,0");
        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        result = AT_OK;
        break;
    }
    case AT_MODE_TEST: {
        /* Send the range of supported values. */
        (void)sprintf((char *)info_text, ": (2),(0),(0)");
        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        break;
    }
    default: {
        Parser_SetCMEE_ErrorType(parser_p, CMEE_OPERATION_NOT_ALLOWED);
        result = AT_CME_ERROR;
        break;
    }
    }

    return result;
}

/*
 *===============================================================================
 *
 *   Function: AT_PLUS_CMEE_Handle
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
 *   The AT+CMEE command sets the response format.
 *
 *===============================================================================
 */
AT_Command_e AT_PLUS_CMEE_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text, AT_ResponseMessage_t message)
{
    AT_Command_e result = AT_OK; /* Result code from the last executed command.*/
    unsigned char flag; /* Temporary storage for bitfield flag. */

    /* Null terminate the string so that we can use "str" functions. */
    *info_text = END_OF_STRING;

    flag = parser_p->CMEE;
    result = AT_Flag_Handle(parser_p, info_text, &flag, 2);
    parser_p->CMEE = flag;

    return result;
}


/*
 *===============================================================================
 *
 *   Function: AT_STAR_EMSR_Handle
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
 *   Description:-The AT command EMSR is for Modem silent reset.
 *
 *===============================================================================
 */
AT_Command_e AT_STAR_EMSR_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text, AT_ResponseMessage_t message)
{
    AT_Command_e result = AT_OK;
    exe_t *exe_p;
    exe_request_result_t exe_result = EXE_FAILURE;

    switch (parser_p->Mode) {
    case AT_MODE_DO:

        if (!parser_p->IsPending) {
            exe_p = atc_get_exe_handle();

            if (!exe_p) {
                ATC_LOG_E("unable to acquire executor context!\n");
                result = AT_ERROR;
                goto error;
            }

            exe_result = exe_request(exe_p, EXE_EMSR, NULL,
                                     (void *)parser_p, &parser_p->request_handle);

            if (exe_result != EXE_PENDING) {
                Parser_SetCMEE_ErrorType(parser_p,
                                         CMEE_UNKNOWN);
                result = AT_CME_ERROR;
                goto error;
            }

            result = AT_PENDING;
        } else {
            if (EXE_SUCCESS == message->exe_result_code) {
                result = AT_OK;
            } else {
                ATC_LOG_E("exe_result %d", exe_result);
                result = AT_ERROR;
            }
        }
        break;
    case AT_MODE_TEST:
        break;
    default:
        Parser_SetCMEE_ErrorType(parser_p, CMEE_OPERATION_NOT_ALLOWED);
        result = AT_CME_ERROR;
        break;
    }

error:
    return result;

}


/*
 *===============================================================================
 *
 *   Function: AT_STAR_ESLEEP_Handle
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
 *   Description:-The AT command ESLEEP is for setting the modem in sleep mode.
 *
 *===============================================================================
 */
AT_Command_e AT_STAR_ESLEEP_Handle(AT_ParserState_s *parser_p,
                                   AT_CommandLine_t info_text,
                                   AT_ResponseMessage_t message)
{

    exe_request_result_t result = EXE_SUCCESS;

    switch (parser_p->Mode) {
    case AT_MODE_SET:

        if (!parser_p->IsPending) {
            bool ErrorFlag = false;
            exe_t *exe_p = NULL;
            IntParam_t n = 0;
            exe_modem_sleep_t req_data;
            /* Read command parameter */
            n = Parser_GetIntParam(parser_p, &ErrorFlag, NULL);

            if (ErrorFlag) {
                return AT_ERROR;
            }

            /* Only one parameter supported */
            if (!parser_p->NoMoreParams) {
                Parser_SetCMEE_ErrorType(parser_p,
                                         CMEE_INCORRECT_PARAMETERS);
                return AT_CME_ERROR;
            }

            if (n > 1) {
                Parser_SetCMEE_ErrorType(parser_p,
                                         CMEE_OPERATION_NOT_ALLOWED);
                return AT_CME_ERROR;
            }

            exe_p = atc_get_exe_handle();

            if (NULL == exe_p) {
                return AT_ERROR;
            }

            req_data.n = (bool)n;
            result = exe_request(exe_p, EXE_MODEM_ESLEEP, &req_data,
                                 (void *)parser_p, &parser_p->request_handle);

        } else {
            if (EXE_SUCCESS == message->exe_result_code) {
                result = AT_OK;
            } else {
                Parser_SetCMEE_ErrorType(parser_p,
                                         CMEE_UNKNOWN);
                return  AT_CME_ERROR;
            }
        }

        break;


    case AT_MODE_TEST:
        sprintf((char *) info_text, ": (0-1)");
        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        result = AT_OK;
        break;
    default:
        result = AT_ERROR;
    }

    return AT_OK;
}


/*
 *===============================================================================
 *
*   Function: AT_PLUS_CEER_Handle
 *
 *   INPUT:   parser_p  - Pointer to the current parser state.
 *            info_text - Pointer to a string buffer to put information text.
 *            message   - Used when call back from handling PENDING.
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
 *   DESCRIPTION: Returns the cause most recent call / network failure or disconnection.
 *
 *   DO - Return the cause string
*
*   READ - Not applicable
*
*   TEST - AT_OK
*
*===============================================================================
*/

AT_Command_e AT_PLUS_CEER_Handle(AT_ParserState_s    *parser_p,
                                 AT_CommandLine_t     info_text,
                                 AT_ResponseMessage_t message)
{
    AT_Command_e result = AT_OK;
    exe_t *exe_p;
    int i;
    exe_ceer_request_t ceer_resp;
    exe_request_result_t exe_result = EXE_FAILURE;

    switch (parser_p->Mode) {
    case AT_MODE_DO:
        exe_p = atc_get_exe_handle();
        exe_result = exe_request(exe_p, EXE_CEER, (void *) &ceer_resp,
                                 (void *)parser_p, &parser_p->request_handle);

        if (EXE_SUCCESS == exe_result) {
            sprintf((char *) info_text, ": %s", ceer_resp.ceer_string);
            Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        } else {
            ATC_LOG_E("AT_PLUS_CEER_Handle(): exe_result %d", exe_result);
            result = AT_ERROR;
        }

        break;
    case AT_MODE_TEST:
        break;
    }

    return result;
}

/*
 *===============================================================================
 *
 *   Function: AT_PLUS_CMER_Handle
 *
 *   INPUT:   Parser   - Pointer to the current parser state.
 *            InfoText - Pointer to a string buffer to put information text.
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
 *   The CMER function is used to enable/disable sending of unsolicited result
 *   codes from ME to TE in case of key pressings, display and indicator state
 *   changes.
 *   Key pressings and display state changes are currently not supported.
 *
 *     SET  - Set phone activity status
 *     READ - Read the current setting
 *     TEST - Test if command is supported
 *
 *===============================================================================
 */
AT_Command_e AT_PLUS_CMER_Handle(AT_ParserState_s *parser_p, AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message)
{
    AT_Command_e result = AT_OK;

    switch (parser_p->Mode) {
    case AT_MODE_SET: {
        if (!parser_p->IsPending) {
            IntParam_t mode = 0;
            IntParam_t keyp = 0;
            IntParam_t disp = 0;
            IntParam_t ind = 0;
            IntParam_t bfr = 0;
            int num_params = 0;
            bool error_flag = false;
            bool cmer_ind_activated = false;
            unsigned char entry = 0;
            exe_request_result_t exe_res = EXE_FAILURE;
            exe_cmer_data_t cmer_data;

            mode = Parser_GetIntParam(parser_p, &error_flag, &mode);
            num_params++;

            if (error_flag) {
                ATC_LOG_E("%s: error reading mode", __func__);
                result = AT_ERROR;
                goto exit;
            }

            if (CMER_MODE_BUFFER != mode && CMER_MODE_FORWARD != mode) {
                result = AT_CME_ERROR;
                Parser_SetCMEE_ErrorType(parser_p,
                                         CMEE_INCORRECT_PARAMETERS);
                goto exit;
            }

            if (!parser_p->NoMoreParams) {
                keyp = Parser_GetIntParam(parser_p, &error_flag, &keyp);
                num_params++;

                if (error_flag) {
                    ATC_LOG_E("%s: error reading keyp", __func__);
                    result = AT_ERROR;
                    goto exit;
                }

                if (0 != keyp) {
                    result = AT_CME_ERROR;
                    Parser_SetCMEE_ErrorType(parser_p,
                                             CMEE_INCORRECT_PARAMETERS);
                    goto exit;
                }
            }

            if (!parser_p->NoMoreParams) {
                disp = Parser_GetIntParam(parser_p, &error_flag, &disp);
                num_params++;

                if (error_flag) {
                    ATC_LOG_E("%s: error reading disp", __func__);
                    result = AT_ERROR;
                    goto exit;
                }

                if (0 != disp) {
                    result = AT_CME_ERROR;
                    Parser_SetCMEE_ErrorType(parser_p,
                                             CMEE_INCORRECT_PARAMETERS);
                    goto exit;
                }
            }

            if (!parser_p->NoMoreParams) {
                ind = Parser_GetIntParam(parser_p, &error_flag, &ind);
                num_params++;

                if (error_flag) {
                    ATC_LOG_E("%s: error reading ind", __func__);
                    result = AT_ERROR;
                    goto exit;
                }

                if (CMER_IND_DEACTIVATED != ind && CMER_IND_ACTIVATED != ind) {
                    result = AT_CME_ERROR;
                    Parser_SetCMEE_ErrorType(parser_p,
                                             CMEE_INCORRECT_PARAMETERS);
                    goto exit;
                }
            }

            if (!parser_p->NoMoreParams) {
                bfr = Parser_GetIntParam(parser_p, &error_flag, &bfr);
                num_params++;

                if (error_flag) {
                    ATC_LOG_E("%s: error reading bfr", __func__);
                    result = AT_ERROR;
                    goto exit;
                }

                if (0 != bfr) {
                    result = AT_CME_ERROR;
                    Parser_SetCMEE_ErrorType(parser_p,
                                             CMEE_INCORRECT_PARAMETERS);
                    goto exit;
                }
            }

            if (!parser_p->NoMoreParams) {
                result = AT_CME_ERROR;
                Parser_SetCMEE_ErrorType(parser_p,
                                         CMEE_INCORRECT_PARAMETERS);
                goto exit;
            }

            /* Parameter validation complete */

            if (num_params > 0) {
                parser_p->CMER_mode = mode;
            }

            if (num_params > 3) {
                /* Loop through the ParserStateTable and see if any other ParserState has indications activated */
                for (entry = 0; entry < MAX_NO_OF_OPEN_CHANNELS; entry++) {
                    AT_ParserState_s *parser_entry_p = ParserStateTable_GetParserState_ByEntry(entry);

                    if (CMER_IND_ACTIVATED == parser_entry_p->CMER_ind) {
                        if (parser_entry_p != parser_p) {
                            cmer_ind_activated = true;
                            break;
                        }
                    }
                }

                /* To force a quick +CIEV: 2,<n> event, reset the last reported
                 * signal quality. If reporting is activated, a URC will follow
                 * as soon as an RSSI update is received from the modem.
                 */
                parser_p->CIEV_signal = CIEV_UNKNOWN_BARS;

                /* If no other Parsers have CMER indications enabled, let this
                 * Parser control enabling or disabling of CMER indications.
                 */
                if (!cmer_ind_activated) {
                    cmer_data.ind = ind;

                    exe_res = exe_request(atc_get_exe_handle(),
                                          EXE_CMER, &cmer_data,
                                          (void *)parser_p, &parser_p->request_handle);

                    result = parser_handle_exe_result(exe_res, parser_p);

                    if (EXE_SUCCESS == exe_res || EXE_PENDING == exe_res) {
                        parser_p->CMER_ind = ind;
                    }

                    break;
                } else {
                    /* CMER Indications already activated  */
                    parser_p->CMER_ind = ind;
                }

            }

            result = AT_OK;

        } else {

            if (EXE_SUCCESS == message->exe_result_code) {
                result = AT_OK;
            } else {
                ATC_LOG_E("%s: exe_result_code error", __FUNCTION__);
                result = AT_ERROR;
            }
        }

        break;
    }
    case AT_MODE_READ: {
        (void)sprintf((char *) info_text, ": %d,0,0,%d,0", parser_p->CMER_mode, parser_p->CMER_ind);
        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        break;
    }
    case AT_MODE_TEST: {
        (void)sprintf((char *)info_text, ": (0,3),(0),(0),(0,1),(0)");
        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        break;
    }
    default:
        Parser_SetCMEE_ErrorType(parser_p, CMEE_OPERATION_NOT_ALLOWED);
        result = AT_CME_ERROR;
        break;
    }

exit:
    return result;
}

/*===============================================================================
 *
 *   Function: AT_PLUS_RADIOVER_Handle
 *
 *   INPUT:   parser_p      - Pointer to the current parser state.
 *            info_text     - Pointer to a string buffer to put information text.
 *            message       - Not used
 *
 *   OUTPUT:  parser_p->Pos - The current position in the command line.
 *
 *   RETURNS: A status code for the execution of the command is returned:
 *            AT_OK        - The command was successfully handled.
 *            AT_ERROR     - There where a general error.
 *            AT_CME_ERROR - There where a GSM specific error, the error code is
 *                           returned in ASCII format in info_text.
 *            AT_PENDING   - The command is not ready but returns while
 *                           waiting for an asynchronous response.
 *
 *   The RADIOVER command allows application process to query radio version information.
 *
 *   READ - Read command returns the radio version information
 *          +RADIOVER: <version_string>

 *===============================================================================
 */

AT_Command_e AT_PLUS_RADIOVER_Handle(AT_ParserState_s *parser_p,
                                     AT_CommandLine_t info_text,
                                     AT_ResponseMessage_t message)
{
    AT_Command_e         result      = AT_OK;
    exe_t                *exe_p      = NULL;
    exe_request_result_t exe_result  = EXE_FAILURE;

    switch (parser_p->Mode) {
    case AT_MODE_READ:

        if (!parser_p->IsPending) {
            exe_p = atc_get_exe_handle();
            exe_result = exe_request(exe_p, EXE_RADIOVER_READ,
                                     NULL, (void *) parser_p, &parser_p->request_handle);

            result = parser_handle_exe_result(exe_result, parser_p);
        } else {
            /* We have a result...*/
            if (EXE_SUCCESS == message->exe_result_code) {
                sprintf((char *) info_text, ": %s", (char *) message->data_p);
                Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
            } else {
                Parser_SetCMEE_ErrorType(parser_p, CMEE_OPERATION_NOT_ALLOWED);
                result = AT_CME_ERROR;
            }
        }

        break;

    case AT_MODE_TEST:
        result = AT_OK;
        break;

    default :
        ATC_LOG_E("AT_PLUS_RADIOVER_Handle: Invalid mode!");
        result = AT_ERROR;
        break;
    }

exit:
    return result;
}

/*
 *===============================================================================
 *
 *   Function: AT_STAR_EPWRRED_Handle
 *
 *   INPUT:   parser_p       - Pointer to the current parser state.
 *            info_text      - Pointer to a string buffer to put information text.
 *
 *   OUTPUT:  parser_p->Pos  - Current position in the command line.
 *
 *   RETURNS: Status code for the execution of the commands:
 *            AT_OK          - Successfully handled command.
 *            AT_ERROR       - General error.
 *            AT_CME_ERROR   - GSM specific error, the error code is
 *                             returned in ASCII format in info_text.
 *
 *   The *EPWRRED function is used to send a TX back off event to the modem. The
 *   Information is used by the modem to determine if the TX power should be
 *   reduced or not.
 *
 *   SET  - Send TX back off event to modem
 *   READ - Not supported
 *   TEST - Describes the supported range for the SET command.
 *
 *
 *===============================================================================
 */
AT_Command_e AT_STAR_EPWRRED_Handle(AT_ParserState_s *parser_p,
                                    AT_CommandLine_t info_text,
                                    AT_ResponseMessage_t message)
{
    AT_Command_e result = AT_ERROR;

    switch (parser_p->Mode) {
    case AT_MODE_SET: {
        if (!parser_p->IsPending) {
            bool error_flag = false;
            exe_request_result_t exe_res = EXE_FAILURE;
            exe_epwrred_t epwrred;
            epwrred.event = EXE_TX_BACK_OFF_EVENT_NO_EVENT;

            epwrred.event = (exe_tx_back_off_event_t)Parser_GetIntParam(parser_p, &error_flag, NULL);

            if (error_flag) {
                ATC_LOG_E("could not parse <event> parameter!");
                Parser_SetCMEE_ErrorType(parser_p, CMEE_OPERATION_NOT_SUPPORTED);
                result = AT_CME_ERROR;
                break;
            }

            /* Do not accept more than one parameter */
            if (!parser_p->NoMoreParams) {
                ATC_LOG_E("more than one parameter is not allowed!");
                Parser_SetCMEE_ErrorType(parser_p, CMEE_OPERATION_NOT_SUPPORTED);
                result = AT_CME_ERROR;
                break;
            }

            /* check parameter range of <event> */
            if ((int)epwrred.event < EXE_TX_BACK_OFF_EVENT_NO_EVENT || epwrred.event > EXE_TX_BACK_OFF_EVENT_AUTOMATIC_THERMAL_CONTROL_ACTIVE) {
                ATC_LOG_E("bad parameter value!");
                Parser_SetCMEE_ErrorType(parser_p, CMEE_OPERATION_NOT_SUPPORTED);
                result = AT_CME_ERROR;
                break;
            }

            exe_res = exe_request(atc_get_exe_handle(),
                                  EXE_EPWRRED_SET,
                                  &epwrred,
                                  (void *)parser_p,
                                  &parser_p->request_handle);

            if (EXE_PENDING != exe_res) {
                ATC_LOG_E("unsuccessful request response! (%d)", exe_res);
                Parser_SetCMEE_ErrorType(parser_p, CMEE_UNKNOWN);
                result = AT_CME_ERROR;
                break;
            }

            result = AT_PENDING;
        } else { /* IsPending */
            if (EXE_SUCCESS == message->exe_result_code) {
                result = AT_OK;
            } else {
                Parser_SetCMEE_ErrorType(parser_p, CMEE_UNKNOWN);
                result = AT_CME_ERROR;
            }
        }

        break;
    } /* case AT_MODE_SET: */

    case AT_MODE_TEST: {
        strcpy((char *)info_text, ": (0-16)");
        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        result = AT_OK;
        break;

    } /* case AT_MODE_TEST: */

    default:
        ATC_LOG_E("unknown AT mode (%d)", parser_p->Mode);
        break;
    } /* switch (parser_p->Mode) */

    return result;
} /* AT_Command_e AT_STAR_EPWRRED_Handle */

/*
 * Handle incoming *EEMPAGE urc from the executor
 * This URC is issued when returning result originating
 * from an AT*EEMPAGE "one-shot" requested or as an indicator of internal results
 */
void handle_incoming_EEMPAGE_event(exe_empage_urc_t *data_p)
{
    /* "*EEMPAGE: <page>,<seqno> where <page> and <seqno> are integer numbers */
    AT_ParserState_s *parser_p   = NULL;
    AT_CommandLine_t  info_text  = NULL;
    unsigned char     entry;

    ATC_LOG_D("Enter");

    info_text = (AT_CommandLine_t) calloc(1, AT_MAX_DATA_LENGTH);

    if (NULL == info_text) {
        ATC_LOG_E("ERROR!!! Memory allocation failed");
        return;
    }

    if (NULL != data_p) {
        sprintf((char *)info_text, "*EEMPAGE: %d,%d", (data_p->page), (data_p->seqnr));

        /* Loop through all parsers */
        for (entry = 0; entry < MAX_NO_OF_OPEN_CHANNELS; entry++) {
            parser_p = ParserStateTable_GetParserState_ByEntry(entry);
            /* This will be sent to all channels */
            parser_send_unsolicited(parser_p, AT_TEXT_RESPONSE, (AT_CommandLine_t)info_text);

            /* NOTE - We use this URC also to indicate to client asynchronous results of activation/deactivation
             * These should not change the internal state !
             * Only indications with a page number while in one-shot mode should set state to idle.
             */
            if (EXE_EMPAGE_STATE_ONESHOT == parser_p->empage_state) {
                parser_p->empage_state = EXE_EMPAGE_STATE_IDLE;
            }
        }
    } else {
        ATC_LOG_E("ERROR!!! data_p is NULL");
    }

    /*Free the allocated memory*/
    free(info_text);
}

/*
 *===============================================================================
 *
 *   Function: AT_STAR_EEMPAGE_Handle
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
 *   SET  - Engineer mode request
 *          AT*EEMPAGE=<cmd>[,<page>[,<timer>]]
 *
 *   READ - Returns ok.
 *   TEST - Show list of supported parameters.
 *
 *===============================================================================
 */
AT_Command_e AT_STAR_EEMPAGE_Handle(AT_ParserState_s *parser_p,
                                    AT_CommandLine_t info_text,
                                    AT_ResponseMessage_t message)
{
    AT_Command_e         result        = AT_OK;
    AT_Command_e         current_cmee  = CMEE_OK;
    exe_t                *exe_p        = NULL;
    exe_request_result_t exe_result    = EXE_FAILURE;
    IntParam_t           cmd           = 0;
    IntParam_t           page          = EEMPAGE_DEFAULT_PAGE;
    IntParam_t           timervalue    = EEMPAGE_DEFAULT_TIMERVALUE;
    bool                 error_flag    = false;
    bool                 try_getpage   = false;
    bool                 try_gettime   = false;
    exe_empage_t         empage_data;

    switch (parser_p->Mode) {
    case AT_MODE_SET:

        if (!parser_p->IsPending) {
            cmd = Parser_GetIntParam(parser_p, &error_flag, NULL);

            if (error_flag) {
                ATC_LOG_E("Parsing of <cmd> failed");
                current_cmee = CMEE_INCORRECT_PARAMETERS;
                result       = AT_CME_ERROR;
                goto exit;
            }

            switch ((exe_empage_cmd_mode_t)cmd) {
            case EXE_EMPAGE_CMD_ONESHOT:
                /* May have <page> and if so then possibly also <timer> */
                try_getpage = true;
                try_gettime = true;
                break;

            case EXE_EMPAGE_CMD_START_PERIODIC:
                /* May have <page> */
                try_getpage = true;
                break;

            case EXE_EMPAGE_CMD_STOP_PERIODIC:
            case EXE_EMPAGE_CMD_RESET_MEASUREMENTS:
                break;

            default:
                ATC_LOG_E("Erroneous <cmd>");
                current_cmee = CMEE_INCORRECT_PARAMETERS;
                result       = AT_CME_ERROR;
                goto exit;
            }

            if (try_getpage && !parser_p->NoMoreParams) {
                page = Parser_GetIntParam(parser_p, &error_flag, &page);

                if (error_flag) {
                    ATC_LOG_E("Parsing of <page> failed");
                    current_cmee = CMEE_INCORRECT_PARAMETERS;
                    result       = AT_CME_ERROR;
                    goto exit;
                }
            }

            if (try_gettime && !parser_p->NoMoreParams) {
                timervalue = Parser_GetIntParam(parser_p, &error_flag, &timervalue);

                if (error_flag) {
                    ATC_LOG_E("Parsing of <timer> failed");
                    current_cmee = CMEE_INCORRECT_PARAMETERS;
                    result       = AT_CME_ERROR;
                    goto exit;
                }
            }

            /* No further parameters allowed */
            if (!parser_p->NoMoreParams) {
                ATC_LOG_E("Too many parameters received for given <cmd>");
                current_cmee = CMEE_INCORRECT_PARAMETERS;
                result       = AT_CME_ERROR;
                goto exit;
            }

            if ((page < EEMPAGE_FIRST_PAGE) ||
                    (page > EEMPAGE_LAST_PAGE)) {
                ATC_LOG_E("Given page is out of bounds");
                current_cmee = CMEE_INCORRECT_PARAMETERS;
                result       = AT_CME_ERROR;
                goto exit;
            }

            if ((timervalue <= 0) || (timervalue > 60)) {
                ATC_LOG_E("Given timer value is out of bounds");
                current_cmee = CMEE_INCORRECT_PARAMETERS;
                result       = AT_CME_ERROR;
                goto exit;
            }

            empage_data.mode  = cmd;
            empage_data.page  = page;
            empage_data.timer = timervalue;

            /* Validate state and update internal state supervision */
            switch ((exe_empage_cmd_mode_t)cmd) {
            case EXE_EMPAGE_CMD_ONESHOT:

                if (EXE_EMPAGE_STATE_IDLE == parser_p->empage_state) {
                    parser_p->empage_state = EXE_EMPAGE_STATE_ONESHOT;
                } else {
                    error_flag = true;
                }

                break;

            case EXE_EMPAGE_CMD_START_PERIODIC:

                if (EXE_EMPAGE_STATE_IDLE == parser_p->empage_state) {
                    parser_p->empage_state = EXE_EMPAGE_STATE_PERIODIC;
                } else {
                    error_flag = true;
                }

                break;

            case EXE_EMPAGE_CMD_RESET_MEASUREMENTS:
                break;

            case EXE_EMPAGE_CMD_STOP_PERIODIC:

                if (EXE_EMPAGE_STATE_PERIODIC == parser_p->empage_state) {
                    parser_p->empage_state = EXE_EMPAGE_STATE_IDLE;
                } else {
                    error_flag = true;
                }

                break;
            }

            if (error_flag) {
                ATC_LOG_E("Operation aborted, wrong state");
                current_cmee = CMEE_OPERATION_NOT_ALLOWED;
                result       = AT_CME_ERROR;
                goto exit;
            }

            exe_p = atc_get_exe_handle();

            if (exe_p) {
                exe_result = exe_request(exe_p, EXE_EMPAGE_SET,
                                         (void *) &empage_data,
                                         (void *) parser_p,
                                         &parser_p->request_handle);
                result     = parser_handle_exe_result(exe_result, parser_p);
            } else {
                ATC_LOG_E("atc_get_exe_handle failed ");
                result       = AT_ERROR;
                goto exit;
            }

        } else { /* We have a result...*/
            if (message->exe_result_code != EXE_SUCCESS) {
                ATC_LOG_E("Execution of request failed");
                current_cmee = CMEE_OPERATION_NOT_ALLOWED;
                result       = AT_CME_ERROR;
                goto exit;
            }
        }

        break;

    case AT_MODE_TEST:
        sprintf((char *) info_text, ": (0-3),(1-5),(1-60)");
        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        break;

    default :
        ATC_LOG_E("Invalid mode!");
        result = AT_ERROR;
        break;
    }

exit:

    if (CMEE_OK != current_cmee) {
        Parser_SetCMEE_ErrorType(parser_p, current_cmee);
    }

    return result;
}

