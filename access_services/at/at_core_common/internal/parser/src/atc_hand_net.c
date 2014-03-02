/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <atc.h>
#include <atc_command_list.h>
#include <atc_config.h>
#include <atc_exe_glue.h>
#include <atc_handlers.h>
#include <atc_log.h>
#include <atc_parser.h>
#include <atc_string.h>
#include <atc_parser_util.h>

#include <exe.h>

#define SET_RESULT_AND_GOTO_EXIT(value) result = (value); goto exit;

#define ATC_CREG_UNSOL_MAX_RESP_LENGTH  26 /* ": 1,"EB2D","0E120805",255"     */
#define ATC_CREG_UNSOL_MIN_RESP_LENGTH  5  /* ": 14" */
#define ATC_EREG_UNSOL_MAX_RESP_LENGTH  26 /* ": 1,"EB2D","0E120805",255"   */
#define ATC_CGREG_UNSOL_MAX_RESP_LENGTH 25 /* ": 1,"EB2D","0E120805",16"   */
#define ATC_CGREG_UNSOL_MIN_RESP_LENGTH 5  /* ": 14" */
#define ATC_ECME_UNSOL_MIN_RESP_LENGTH  14 /* "*ECME: XXX,"""             */

#define ATC_EGNCI_NUMBER_OF_NEIGHBORS 6
#define ATC_EWNCI_NUMBER_OF_NEIGHBORS 6

static exe_creg_reg_info_t creg_data = {EXE_NET_REG_STAT_UNKNOWN, 0, 0, EXE_BASIC_ACT_UNKNOWN, 0};
static exe_cgreg_reg_info_t cgreg_data = {EXE_NET_REG_STAT_UNKNOWN, 0, 0, 0};

static int get_highest_trigger_level();

/*
 *===============================================================================
 *
 *   Function: AT_PLUS_CREG_Handle
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
 *   The +CREG function is used to query the current ME network registration
 *   status and to enable unsolicited result codes when the status is changed.
 *
 *   SET  - Enables ('1') or disables ('0') the unsolicited result codes.
 *   READ - Displays whether the unsolicited result code is enabled or disabled,
 *          the current ME network registration status, local area code and cell
 *          identification.
 *   TEST - Writes the supported range for the enable command.
 *
 *
 *===============================================================================
 */
AT_Command_e AT_PLUS_CREG_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text, AT_ResponseMessage_t message)
{
    AT_Command_e result = AT_ERROR;

    switch (parser_p->Mode) {
    case AT_MODE_SET: {
        if (!parser_p->IsPending) {
            bool error_flag = false;
            net_reg_ind_type_t IndicationType = NET_REG_IND_DISABLED;
            exe_request_result_t exe_res = EXE_FAILURE;
            int trigger_level = 0;

            /* If parameter not given the default value CREG_DISABLED will be used. */
            IndicationType = (net_reg_ind_type_t) Parser_GetIntParam(parser_p,
                             &error_flag, (unsigned char *) &IndicationType);

            /* ERROR if parameter <n> was not fetched correctly */
            if (error_flag) {
                break;
            }

            /* Do not accept more than one parameter */
            if (!parser_p->NoMoreParams) {
                Parser_SetCMEE_ErrorType(parser_p, CMEE_OPERATION_NOT_SUPPORTED);
                result = AT_CME_ERROR;
                break;
            }

            /* CME ERROR if received value is out of range for parameter <n>*/
            if (IndicationType > NET_REG_IND_LAC_AND_CELL_ID) {
                Parser_SetCMEE_ErrorType(parser_p, CMEE_OPERATION_NOT_SUPPORTED);
                result = AT_CME_ERROR;
                break;
            }

            /* Save parameter <n> */
            parser_p->CREG = IndicationType;

            trigger_level = get_highest_trigger_level();

            exe_res = exe_request(atc_get_exe_handle(),
                                  EXE_CREG_SET,
                                  &trigger_level,
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

    case AT_MODE_READ: {

        if (!parser_p->IsPending) {
            exe_request_result_t exe_res = exe_request(atc_get_exe_handle(),
                                           EXE_CREG_READ,
                                           NULL,
                                           (void *)parser_p,
                                           &parser_p->request_handle);

            if (EXE_PENDING == exe_res) {
                result = AT_PENDING;
            }

        } else {
            if (NULL != message) {
                if (EXE_SUCCESS == ((AT_ResponseMessage_t) message)->exe_result_code) {
                    exe_creg_reg_info_t *data_p = (exe_creg_reg_info_t *) message->data_p;

                    if (NULL != data_p) {

                        if (data_p->cs_attached == 0 && (EXE_NET_REG_STAT_REGISTERED_HOME == data_p->stat ||
                                                         EXE_NET_REG_STAT_REGISTERED_ROAMING == data_p->stat)) {
                            data_p->stat = EXE_NET_REG_STAT_NOT_REGISTERED_SEARCH;
                        }

                        /* Response format "+CREG: <n>,<stat>[,<lac>,<ci>,<Act>]" */
                        /* Append <n>, <stat> */
                        AT_AddValue(info_text, ": 0", parser_p->CREG);
                        AT_AddValue(info_text, ",0", data_p->stat);

                        /* Add [,<lac>,<ci>,<Act>] if both network and location info are requested
                         * and the ME is registered to the network. */
                        if (NET_REG_IND_LAC_AND_CELL_ID == parser_p->CREG &&
                                1 == data_p->cs_attached &&
                                (EXE_NET_REG_STAT_REGISTERED_HOME == data_p->stat ||
                                 EXE_NET_REG_STAT_REGISTERED_ROAMING == data_p->stat)) {
                            AT_AddBasedValue(info_text, ",\"0000\"", data_p->lac, AT_HEX_INT);
                            AT_AddBasedValue(info_text, ",\"00000000\"", data_p->cid, AT_HEX_INT);
                            AT_AddValue(info_text, ",0", data_p->act);
                        }

                        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
                        result = AT_OK;
                    } else {
                        ATC_LOG_E("%s: data_p in message is NULL", __FUNCTION__);
                        Parser_SetCMEE_ErrorType(parser_p, CMEE_UNKNOWN);
                        result = AT_CME_ERROR;
                    }
                } else {
                    ATC_LOG_E("%s: unsuccessful response", __FUNCTION__);
                    Parser_SetCMEE_ErrorType(parser_p, CMEE_OPERATION_NOT_ALLOWED);
                    result = AT_CME_ERROR;
                }
            } else {
                ATC_LOG_E(": message in response is NULL", __FUNCTION__);
                Parser_SetCMEE_ErrorType(parser_p, CMEE_UNKNOWN);
                result = AT_CME_ERROR;
            }
        }

        break;
    } /* case AT_MODE_READ: */

    case AT_MODE_TEST: {
        strcpy((char *)info_text, ": (0-2)");
        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        result = AT_OK;
        break;
    } /* case AT_MODE_TEST: */

    default:
        break;
    } /* switch (parser_p->Mode) */

    return result;
} /* AT_Command_e AT_PLUS_CREG_Handle */

/*
 * Handle CREG events sent from modem. Compose an AT string and send the unsolicited.
 */
void handle_incoming_creg_event(void *data_p)
{
    exe_creg_reg_info_t *creg_data_p = NULL;
    AT_ParserState_s *parser_p = NULL;
    uint8_t i;

    if (NULL == data_p) {
        ATC_LOG_E("handle_incoming_creg_event(): error data_p is NULL");
        return;
    }

    creg_data_p = (exe_creg_reg_info_t *) data_p;

    if (creg_data_p->cs_attached == 0 && (EXE_NET_REG_STAT_REGISTERED_HOME == creg_data_p->stat ||
                                          EXE_NET_REG_STAT_REGISTERED_ROAMING == creg_data_p->stat)) {
        creg_data_p->stat = EXE_NET_REG_STAT_NOT_REGISTERED_SEARCH;
    }

    /* Don't send update if nothing has changed */
    if (creg_data_p->cid == creg_data.cid && creg_data_p->lac ==
            creg_data.lac && creg_data_p->stat == creg_data.stat) {
        return;
    }

    /* Loop through all parser states to find subscriptions. */
    for (i = 0; i < MAX_NO_OF_OPEN_CHANNELS; i++) {

        parser_p = ParserStateTable_GetParserState_ByEntry(i);

        if (NULL != parser_p) {
            if (NET_REG_IND_NETWORK_STATUS == parser_p->CREG) {

                char creg_text[ATC_CREG_UNSOL_MIN_RESP_LENGTH] = { '\0' };

                /* Don't send update if service state has not changed,
                 * only in case of +CREG=1.
                 */
                if (creg_data_p->stat == creg_data.stat) {
                    continue;
                }

                /* Create a ": <stat>" response part */
                (void) sprintf((char *) creg_text, ": %d", creg_data_p->stat);

                /* Send */
                parser_send_unsolicited(parser_p, AT_PLUS_CREG, (AT_CommandLine_t) creg_text);

            } else if (NET_REG_IND_LAC_AND_CELL_ID == parser_p->CREG) {

                int n = 0;
                char creg_text[ATC_CREG_UNSOL_MAX_RESP_LENGTH] = { '\0' };

                /* Create a ": <stat>" response part */
                n = sprintf((char *) creg_text, ": %d", creg_data_p->stat);

                /* Registered? */
                if (1 == creg_data_p->cs_attached && (EXE_NET_REG_STAT_REGISTERED_HOME == creg_data_p->stat ||
                                                      EXE_NET_REG_STAT_REGISTERED_ROAMING == creg_data_p->stat)) {
                    /* Create and append [,<lac>,<ci>] response part */
                    sprintf((char *)(creg_text + n),
                            ",\"%04X\",\"%08X\",%d",
                            creg_data_p->lac, creg_data_p->cid, creg_data_p->act);
                }

                /* Send */
                parser_send_unsolicited(parser_p, AT_PLUS_CREG,
                                        (AT_CommandLine_t) creg_text);
            }
        }
    }

    creg_data.cid = creg_data_p->cid;
    creg_data.lac = creg_data_p->lac;
    creg_data.stat = creg_data_p->stat;
    creg_data.act = creg_data_p->act;
}

/*
 *===============================================================================
 *
 *   Function: AT_PLUS_CGREG_Handle
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
 *   The +CGREG function is used to query the current ME network GPRS
 *   registration status and to enable unsolicited result codes when the status
 *   is changed.
 *
 *   SET  - Enables ('1') or disables ('0') the unsolicited result codes.
 *   READ - Displays whether the unsolicited result code is enabled or disabled,
 *          the current ME network GPRS registration status, local area code and
 *          cell identification.
 *
 *   TEST - Writes the supported range for the enable command.
 *
 *
 *===============================================================================
 */
AT_Command_e AT_PLUS_CGREG_Handle(AT_ParserState_s *parser_p,
                                  AT_CommandLine_t info_text, AT_ResponseMessage_t message)
{
    AT_Command_e result = AT_ERROR;

    switch (parser_p->Mode) {
    case AT_MODE_SET: {
        if (!parser_p->IsPending) {
            bool error_flag = false;
            net_reg_ind_type_t IndicationType = NET_REG_IND_DISABLED;
            exe_request_result_t exe_res = EXE_FAILURE;
            int trigger_level = 0;

            /* If parameter not given the default value CGREG_DISABLED will be used. */
            IndicationType = (net_reg_ind_type_t) Parser_GetIntParam(parser_p,
                             &error_flag, (unsigned char *) &IndicationType);

            /* ERROR if parameter <n> was not fetched correctly */
            if (error_flag) {
                Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
                result = AT_CME_ERROR;
                break;
            }

            /* Do not accept more than one parameter */
            if (!parser_p->NoMoreParams) {
                Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
                result = AT_CME_ERROR;
                break;
            }

            /* CME ERROR if received value is out of range for parameter <n>*/
            if (IndicationType > NET_REG_IND_LAC_AND_CELL_ID) {
                Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
                result = AT_CME_ERROR;
                break;
            }

            /* Save parameter <n> */
            parser_p->CGREG = IndicationType;

            trigger_level = get_highest_trigger_level();

            exe_res = exe_request(atc_get_exe_handle(),
                                  EXE_CGREG_SET,
                                  &trigger_level,
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

    case AT_MODE_READ: {

        if (!parser_p->IsPending) {
            exe_request_result_t exe_res = exe_request(atc_get_exe_handle(),
                                           EXE_CGREG_READ,
                                           NULL,
                                           (void *)parser_p,
                                           &parser_p->request_handle);

            if (EXE_PENDING == exe_res) {
                result = AT_PENDING;
            }

        } else {
            if (NULL != message) {
                if (EXE_SUCCESS == ((AT_ResponseMessage_t) message)->exe_result_code) {
                    exe_cgreg_reg_info_t *data_p = (exe_cgreg_reg_info_t *) message->data_p;

                    if (NULL != data_p) {
                        /* Response format "+CGREG: <n>,<stat>[,<lac>,<ci>]" */
                        /* Append <n>, <stat> */
                        AT_AddValue(info_text, ": 0", parser_p->CGREG);
                        AT_AddValue(info_text, ",0", data_p->stat);

                        /* Add [,<lac>,<ci>,<act>] if both network and location info are requested
                         * and the ME is registered to the network. */
                        if (NET_REG_IND_LAC_AND_CELL_ID == parser_p->CGREG &&
                                (EXE_NET_REG_STAT_REGISTERED_HOME == data_p->stat ||
                                 EXE_NET_REG_STAT_REGISTERED_ROAMING == data_p->stat)) {
                            AT_AddBasedValue(info_text, ",\"0000\"", data_p->lac, AT_HEX_INT);
                            AT_AddBasedValue(info_text, ",\"00000000\"", data_p->ci, AT_HEX_INT);
                            AT_AddValue(info_text, ",00", data_p->act);
                        }

                        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
                        result = AT_OK;
                    } else {
                        ATC_LOG_E("%s: data_p in message is NULL", __FUNCTION__);
                        Parser_SetCMEE_ErrorType(parser_p, CMEE_UNKNOWN);
                        result = AT_CME_ERROR;
                    }
                } else {
                    ATC_LOG_E("%s: unsuccessful response", __FUNCTION__);
                    Parser_SetCMEE_ErrorType(parser_p, CMEE_OPERATION_NOT_ALLOWED);
                    result = AT_CME_ERROR;
                }
            } else {
                ATC_LOG_E("%s: message in response is NULL", __FUNCTION__);
                Parser_SetCMEE_ErrorType(parser_p, CMEE_UNKNOWN);
                result = AT_CME_ERROR;
            }
        }

        break;
    } /* case AT_MODE_READ: */

    case AT_MODE_TEST: {
        strcpy((char *)info_text, ": (0-2)");
        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        result = AT_OK;
        break;
    } /* case AT_MODE_TEST: */

    default:
        break;
    } /* switch (parser_p->Mode) */

    return result;
} /* AT_Command_e AT_PLUS_CGREG_Handle */

/*
 * Handle CGREG events sent from modem. Compose an AT string and send the unsolicited.
 */
void handle_incoming_cgreg_event(void *data_p)
{
    exe_cgreg_reg_info_t *cgreg_data_p = NULL;
    AT_ParserState_s *parser_p = NULL;
    uint8_t i;

    if (NULL == data_p) {
        ATC_LOG_E("handle_incoming_cgreg_event(): error data_p is NULL");
        return;
    }

    cgreg_data_p = (exe_cgreg_reg_info_t *) data_p;

    /* Don't send update if nothing has changed */
    if (cgreg_data_p->ci == cgreg_data.ci && cgreg_data_p->lac ==
            cgreg_data.lac && cgreg_data_p->stat == cgreg_data.stat
            && cgreg_data_p->act == cgreg_data.act) {
        return;
    }

    /* Loop through all parser states to find subscriptions. */
    for (i = 0; i < MAX_NO_OF_OPEN_CHANNELS; i++) {

        parser_p = ParserStateTable_GetParserState_ByEntry(i);

        if (NULL != parser_p) {
            if (NET_REG_IND_NETWORK_STATUS == parser_p->CGREG) {

                char cgreg_text[ATC_CGREG_UNSOL_MIN_RESP_LENGTH] = { '\0' };

                /* Don't send update if service state has not changed,
                 * only in case of +CGREG=1.
                 */
                if (cgreg_data_p->stat == cgreg_data.stat) {
                    continue;
                }

                /* Create a ": <stat>" response part */
                (void) sprintf((char *) cgreg_text, ": %d", cgreg_data_p->stat);

                /* Send */
                parser_send_unsolicited(parser_p, AT_PLUS_CGREG, (AT_CommandLine_t) cgreg_text);

            } else if (NET_REG_IND_LAC_AND_CELL_ID == parser_p->CGREG) {

                int n = 0;
                char cgreg_text[ATC_CGREG_UNSOL_MAX_RESP_LENGTH] = { '\0' };

                /* Create a ": <stat>" response part */
                n = sprintf((char *) cgreg_text, ": %d", cgreg_data_p->stat);

                /* Registered? */
                if (EXE_NET_REG_STAT_REGISTERED_HOME == cgreg_data_p->stat ||
                        EXE_NET_REG_STAT_REGISTERED_ROAMING == cgreg_data_p->stat) {
                    /* Create and append [,<lac>,<ci>,<act>] response part */
                    sprintf((char *)(cgreg_text + n),
                            ",\"%04X\",\"%08X\",%d",
                            cgreg_data_p->lac, cgreg_data_p->ci, cgreg_data_p->act);
                }

                /* Send */
                parser_send_unsolicited(parser_p, AT_PLUS_CGREG, (AT_CommandLine_t) cgreg_text);
            }
        }
    }

    cgreg_data.ci = cgreg_data_p->ci;
    cgreg_data.lac = cgreg_data_p->lac;
    cgreg_data.stat = cgreg_data_p->stat;
    cgreg_data.act = cgreg_data_p->act;
}

/*==============================================================================
 *
 *   Function: AT_PLUS_EPSB_Handle
 *
 *   INPUT:   parser_p       - Pointer to the current parser state.
 *            info_text      - Pointer to a string buffer to put information text.
 *
 *   OUTPUT:  parser_p->Pos  - The current position in the command line.
 *
 *   RETURNS: A status code for the execution of the command is returned:
 *            AT_OK        - The command was successfully handled.
 *            AT_ERROR     - There where a general error.
 *            AT_CME_ERROR - There where a GSM specific error, the error code is
 *                           returned in ASCII format in info_text.
 *
 *   The *EPSB function is introduced in order to have a command and UR code
 *   with the same info as +CPSB
 *
 *   The *EPSB function is used to controls the presentation of an unsolicited
 *   result code *EPSB.
 *
 *   SET  - Enables or disables the unsolicited result codes.
 *   READ - Displays whether the unsolicited result code is enabled or disabled,
 *          the parameter <curr_bearer> is also returned.
 *   TEST - Writes the supported range for the enable command.
 *
 *=============================================================================*/
AT_Command_e AT_STAR_EPSB_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text, AT_ResponseMessage_t message)
{
    /* TODO Consider to align this implementation with ECAM and likes. */
    exe_cmee_error_t current_cmee = CMEE_OK;

    switch (parser_p->Mode) {
    case AT_MODE_SET: {
        bool error_flag = false;
        BitField_t enable = 0;
        /*
         * Fetch parameter <n>
         * If parameter not given, then default value EXE_EREG_DISABLED will be used.
         */
        enable = (BitField_t) Parser_GetIntParam(parser_p, &error_flag, (unsigned char *) &enable);

        if (error_flag) {
            return AT_ERROR;
        }

        if (!parser_p->NoMoreParams) {
            return AT_ERROR;
        }

        if (enable > 1) {
            current_cmee = CMEE_OPERATION_NOT_SUPPORTED;
            break;
        }

        parser_p->EPSB = enable;
        break;
    }
    case AT_MODE_READ: {
        AT_AddValue(info_text, ": 0", parser_p->EPSB); /* Value for <n> is fixed to enabled */
        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        break;
    } /* case AT_MODE_READ: */
    case AT_MODE_TEST: {
        AT_AddRange(info_text, ": (0-0)", 0, 1);
        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        break;
    }
    default:
        break;
    } /* switch (parser_p->Mode) */

    if (current_cmee != CMEE_OK) {
        Parser_SetCMEE_ErrorType(parser_p, current_cmee);
        return AT_CME_ERROR;
    } else {
        return AT_OK;
    }
}


/*==============================================================================
 *
 *   Function: AT_STAR_EREG_Handle
 *
 *   INPUT:   parser_p       - Pointer to the current parser state.
 *            info_text      - Pointer to a string buffer to put information text.
 *
 *   OUTPUT:  parser_p->Pos  - The current position in the command line.
 *
 *   RETURNS: A status code for the execution of the command is returned:
 *            AT_OK        - The command was successfully handled.
 *            AT_ERROR     - There where a general error.
 *            AT_CME_ERROR - There where a GSM specific error, the error code is
 *                           returned in ASCII format in info_text.
 *
 *   The *EREG function is introduced in order to have a command and UR code
 *   with the same info as +CREG that also supports the <AcT> parameter
 *
 *   The *EREG function is used to query the current ME network registration
 *   status and to enable unsolicited result codes when the status is changed.
 *
 *   SET  - Enables or disables the unsolicited result codes.
 *   READ - Displays whether the unsolicited result code is enabled or disabled,
 *          the current ME network registration status, local area code and cell
 *          identification.
 *   TEST - Writes the supported range for the enable command.
 *
 *=============================================================================*/
AT_Command_e AT_STAR_EREG_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text, AT_ResponseMessage_t message)
{
    switch (parser_p->Mode) {
    case AT_MODE_SET: {
        if (!parser_p->IsPending) {
            bool error_flag = false;
            net_reg_ind_type_t indication_type = NET_REG_IND_DISABLED;
            exe_request_result_t exe_res = EXE_FAILURE;
            int trigger_level = 0;

            /* Fetch parameter <n> */
            indication_type = (net_reg_ind_type_t) Parser_GetIntParam(
                                  parser_p, &error_flag, (unsigned char *) &indication_type);

            if (error_flag) {
                Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
                return AT_CME_ERROR;
            }

            if (!parser_p->NoMoreParams) {
                Parser_SetCMEE_ErrorType(parser_p, CMEE_OPERATION_NOT_ALLOWED);
                return AT_CME_ERROR;
            }

            if (indication_type > NET_REG_IND_LAC_AND_CELL_ID) {
                Parser_SetCMEE_ErrorType(parser_p, CMEE_OPERATION_NOT_ALLOWED);
                return AT_CME_ERROR;
            }

            parser_p->EREG = indication_type;

            trigger_level = get_highest_trigger_level();

            exe_res = exe_request(atc_get_exe_handle(),
                                  EXE_EREG_SET,
                                  &trigger_level,
                                  (void *)parser_p,
                                  &parser_p->request_handle);

            if (EXE_PENDING != exe_res) {
                ATC_LOG_E("unsuccessful request response! (%d)", exe_res);
                Parser_SetCMEE_ErrorType(parser_p, CMEE_UNKNOWN);
                return AT_CME_ERROR;
            }

            return AT_PENDING;

        } else { /* IsPending */
            if (EXE_SUCCESS != message->exe_result_code) {
                Parser_SetCMEE_ErrorType(parser_p, CMEE_UNKNOWN);
                return AT_CME_ERROR;
            }
        }

        break;
    }
    case AT_MODE_READ: {
        if (!parser_p->IsPending) {
            exe_request_result_t exe_res = EXE_FAILURE;

            exe_res = exe_request(atc_get_exe_handle(),
                                  EXE_EREG_READ,
                                  NULL,
                                  (void *)parser_p,
                                  &parser_p->request_handle);

            if (EXE_PENDING != exe_res) {
                ATC_LOG_E("unsuccessful request response! (%d)", exe_res);
                Parser_SetCMEE_ErrorType(parser_p, CMEE_UNKNOWN);
                return AT_CME_ERROR;
            }

            return AT_PENDING;
        } else {
            exe_ereg_reg_info_t *response_data_p = NULL;

            if (!message) {
                ATC_LOG_E("AT_STAR_EREG_Handle: message in response is NULL");
                Parser_SetCMEE_ErrorType(parser_p, CMEE_UNKNOWN);
                return AT_CME_ERROR;
            }

            if (EXE_SUCCESS != ((AT_ResponseMessage_t) message)->exe_result_code) {
                ATC_LOG_E("AT_STAR_EREG_Handle: unsuccessful response");
                Parser_SetCMEE_ErrorType(parser_p, CMEE_UNKNOWN);
                return AT_CME_ERROR;
            }

            response_data_p = (exe_ereg_reg_info_t *) message->data_p;

            if (NULL == response_data_p) {
                ATC_LOG_E("AT_STAR_EREG_Handle: response_data_p in message is NULL");
                Parser_SetCMEE_ErrorType(parser_p, CMEE_UNKNOWN);
                return AT_CME_ERROR;
            }

            sprintf((char *)info_text, ": %d,%d", parser_p->EREG, response_data_p->stat);

            if (NET_REG_IND_LAC_AND_CELL_ID == parser_p->EREG &&
                    1 == response_data_p->cs_attached &&
                    (EXE_NET_REG_STAT_REGISTERED_HOME == response_data_p->stat ||
                     EXE_NET_REG_STAT_REGISTERED_ROAMING == response_data_p->stat)) {
                sprintf((char *)(info_text + strlen((char *)info_text)),
                        ",\"%04X\",\"%08X\",%d",
                        response_data_p->lac, response_data_p->cid, response_data_p->act);
            }

            Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
            break;
        }
    }
    case AT_MODE_TEST: {
        strcpy((char *)info_text, ": (0-2)");
        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        break;
    }
    default:
        Parser_SetCMEE_ErrorType(parser_p, CMEE_OPERATION_NOT_ALLOWED);
        return AT_CME_ERROR;
    }

    return AT_OK;
}

/*
 * Handle EREG unsolicited result code triggered by network registration, cell id, lac or act change
 */
void handle_incoming_ereg_event(void *data_p)
{
    exe_ereg_reg_info_t *ereg_data_p = NULL;
    AT_ParserState_s *parser_p = NULL;
    uint8_t i;
    char ereg_unsol_text[ATC_EREG_UNSOL_MAX_RESP_LENGTH] = { '\0' };

    if (NULL == data_p) {
        ATC_LOG_E("%s: data_p is NULL", __FUNCTION__);
        return;
    }

    ereg_data_p = (exe_ereg_reg_info_t *) data_p;

    if (ereg_data_p->cs_attached == 0 && (EXE_NET_REG_STAT_REGISTERED_HOME == ereg_data_p->stat ||
                                          EXE_NET_REG_STAT_REGISTERED_ROAMING == ereg_data_p->stat)) {
        ereg_data_p->stat = EXE_NET_REG_STAT_NOT_REGISTERED_SEARCH;
    }

    /* Loop through all parser states to find subscriptions. */
    for (i = 0; i < MAX_NO_OF_OPEN_CHANNELS; i++) {

        parser_p = ParserStateTable_GetParserState_ByEntry(i);

        if (NULL == parser_p) {
            continue;
        }

        if (NET_REG_IND_DISABLED == parser_p->EREG) {
            continue;
        }

        sprintf(ereg_unsol_text, ": %d", ereg_data_p->stat);

        if (NET_REG_IND_LAC_AND_CELL_ID == parser_p->EREG &&
                1 == ereg_data_p->cs_attached &&
                (EXE_NET_REG_STAT_REGISTERED_HOME == ereg_data_p->stat ||
                 EXE_NET_REG_STAT_REGISTERED_ROAMING == ereg_data_p->stat)) {

            sprintf(ereg_unsol_text + strlen(ereg_unsol_text),
                    ",\"%04X\",\"%08X\",%d",
                    ereg_data_p->lac, ereg_data_p->cid, ereg_data_p->act);
        }

        parser_send_unsolicited(parser_p, AT_STAR_EREG, (AT_CommandLine_t) ereg_unsol_text);
    }
}

/*
 *===============================================================================
 *  Function: AT_PLUS_COPS_Handle
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
 *  Description: +COPS allows control the network operator selection.
 *               Set, Read and Test are supported.
 *
 *===============================================================================
 */
AT_Command_e AT_PLUS_COPS_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message)
{
    AT_Command_e result = AT_ERROR;

    if (!parser_p->IsPending) {
        switch (parser_p->Mode) {
        case AT_MODE_SET: {
            bool error_flag = false;
            IntParam_t default_parameter = 0;
            char *empty_string_p = "";
            IntParam_t set_mode;
            StrParam_t operator;
            IntParam_t act;

            set_mode = Parser_GetIntParam(parser_p, &error_flag, &default_parameter);
            parser_p->at_plus_cops_format = Parser_GetIntParam(parser_p, &error_flag, &default_parameter);
            operator = Parser_GetStrParam(parser_p, &error_flag, empty_string_p);
            default_parameter = EXE_BASIC_ACT_UNKNOWN;
            act = Parser_GetIntParam(parser_p, &error_flag, &default_parameter);

            if (error_flag) {
                ATC_LOG_E("ERROR FLAG");
                break;
            }

            if (!parser_p->NoMoreParams) {
                break;
            }

            if (set_mode > 4) {
                /* Parameter <mode> is out of range [0,4] */
                break;
            }

            if (parser_p->at_plus_cops_format > 2) {
                /* Parameter <format> is out of range [0,2] */
                break;
            }

            if ((1 == set_mode) || (4 == set_mode)) {
                if (!strcmp((char *) operator, empty_string_p)) {
                    /* Manual mode requires an <oper> string */
                    break;
                }

                if (0 != act && 2 != act && act != EXE_BASIC_ACT_UNKNOWN) {
                    /* Parameter <AcT> must be 0 or 2 */
                    ATC_LOG_E("AcT out of bounds: %d", act);
                    break;
                }
            }

            switch (set_mode) {
            case 0: {
                exe_t *executor_p = atc_get_exe_handle();
                exe_request_result_t
                executor_result = exe_request(executor_p,
                                              EXE_COPS_AUTOMATIC_REGISTER,
                                              NULL,
                                              (void *)parser_p,
                                              &parser_p->request_handle);

                if (EXE_PENDING == executor_result) {
                    result = AT_PENDING;
                }

                break;
            }
            case 1: {
                int n;
                exe_t *executor_p = atc_get_exe_handle();
                exe_cops_request_data_t cops_request;
                cops_request.format = parser_p->at_plus_cops_format;
                cops_request.act = act;
                n = snprintf(cops_request.operator,
                             EXE_COPS_OPERATOR_MAX_LONG_LEN + 1, "%s",
                             operator);

                if (n < 0) {
                    ATC_LOG_E("Could not write to operator field");
                    break;
                } else if (n >= EXE_COPS_OPERATOR_MAX_LONG_LEN + 1) {
                    break;
                } else {
                    exe_request_result_t executor_result =  exe_request(executor_p,
                                                            EXE_COPS_MANUAL_REGISTER,
                                                            &cops_request,
                                                            (void *)parser_p,
                                                            &parser_p->request_handle);

                    if (EXE_PENDING == executor_result) {
                        result = AT_PENDING;
                    }
                }

                break;
            }
            case 2: {
                exe_t *executor_p = atc_get_exe_handle();
                exe_request_result_t executor_result;

                executor_result = exe_request(executor_p,
                                              EXE_COPS_DEREGISTER,
                                              NULL,
                                              (void *) parser_p,
                                              &parser_p->request_handle);

                if (EXE_PENDING == executor_result) {
                    result = AT_PENDING;
                }

                break;
            }
            case 3:
                result = AT_OK;
                break;
            case 4: {
                int n;
                exe_t *executor_p = atc_get_exe_handle();

                if (NULL == executor_p) {
                    ATC_LOG_E("executor_p is NULL");
                    break;
                }

                exe_cops_request_data_t cops_request;
                cops_request.format = parser_p->at_plus_cops_format;
                cops_request.act = act;
                n = snprintf(cops_request.operator,
                             EXE_COPS_OPERATOR_MAX_LONG_LEN + 1, "%s",
                             operator);

                if (n < 0) {
                    ATC_LOG_E("Could not write to operator field");
                    break;
                } else if (n >= EXE_COPS_OPERATOR_MAX_LONG_LEN + 1) {
                    ATC_LOG_E("Operator field >= EXE_COPS_OPERATOR_MAX_LONG_LEN + 1");
                    break;
                } else {
                    exe_request_result_t executor_result =  exe_request(executor_p,
                                                            EXE_COPS_MANUAL_REGISTER_AUTO_FALLBACK,
                                                            &cops_request,
                                                            (void *)parser_p,
                                                            &parser_p->request_handle);

                    if (EXE_PENDING == executor_result) {
                        result = AT_PENDING;
                    }
                }

                break;
            }
            default:
                ATC_LOG_E("Unknown <mode>");
                break;
            }

            break;
        }
        case AT_MODE_READ: {
            exe_t *executor_p = atc_get_exe_handle();
            exe_cops_request_data_t cops_request;
            exe_request_result_t executor_result;
            cops_request.format = parser_p->at_plus_cops_format;
            executor_result = exe_request(executor_p,
                                          EXE_COPS_GET_SETTINGS,
                                          &cops_request,
                                          (void *)parser_p,
                                          &parser_p->request_handle);

            if (EXE_PENDING == executor_result) {
                result = AT_PENDING;
            }

            break;
        }
        case AT_MODE_TEST: {
            exe_t *executor_p = atc_get_exe_handle();
            exe_cops_request_data_t cops_request;
            exe_request_result_t executor_result;
            cops_request.format = parser_p->at_plus_cops_format;
            executor_result = exe_request(executor_p,
                                          EXE_COPS_SEARCH_NETWORKS,
                                          &cops_request,
                                          (void *)parser_p,
                                          &parser_p->request_handle);

            if (EXE_PENDING == executor_result) {
                result = AT_PENDING;
            }

            break;
        }
        default:
            ATC_LOG_E("Unknown operation mode");
            break;
        }
    } else {
        switch (parser_p->Mode) {
        case AT_MODE_SET: {
            if (message) {
                if (EXE_SUCCESS == ((AT_ResponseMessage_t) message)->exe_result_code) {
                    result = AT_OK;
                } else {
                    ATC_LOG_E("unsuccessful response");
                    Parser_SetCMEE_ErrorType(parser_p, CMEE_OPERATION_NOT_ALLOWED);
                    result = AT_CME_ERROR;
                }
            } else {
                ATC_LOG_E("message is NULL");
            }
        }
        break;
        case AT_MODE_READ: {
            if (message) {
                if (EXE_SUCCESS == ((AT_ResponseMessage_t) message)->exe_result_code) {
                    exe_cops_response_data_t *data_p = (exe_cops_response_data_t *) message->data_p;

                    if (data_p) {
                        int n;

                        if (data_p->operator_selected) {
                            /* There is an operator selected! */
                            n = snprintf(
                                    (char *) info_text,
                                    12
                                    + EXE_COPS_OPERATOR_MAX_LONG_LEN, /* ": <mode>[,<format>,<oper>[,<AcT>]]" */
                                    ": %d,%d,\"%s\",%d",
                                    data_p->mode,
                                    data_p->format,
                                    data_p->operator,
                                    data_p->act);

                        } else {
                            /* There is no operator selected */
                            n = snprintf((char *) info_text, 4,
                                         ": %d", data_p->mode);
                        }

                        if (n < 0) {
                            ATC_LOG_E("Could not write to out parameter");
                        } else {
                            result = AT_OK;
                            Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
                        }
                    } else {
                        ATC_LOG_E("no data in message");
                    }
                } else {
                    ATC_LOG_E("unsuccessful response");
                    Parser_SetCMEE_ErrorType(parser_p, CMEE_NO_NETWORK_SERVICE);
                    result = AT_CME_ERROR;
                }
            } else {
                ATC_LOG_E("message is NULL");
            }
        }
        break;
        case AT_MODE_TEST: {
            if (parser_p->abort) {
                exe_t *executor_p = atc_get_exe_handle();
                exe_request_result_t er = exe_request_abort(executor_p, parser_p->request_handle);
                return AT_ERROR;
            }

            if (message) {
                if (EXE_SUCCESS
                        == ((AT_ResponseMessage_t) message)->exe_result_code) {
                    exe_cops_network_list_t
                    *data_p = (exe_cops_network_list_t *) message->data_p;

                    if (data_p) {
                        uint32_t i;
                        (void) strncat((char *) info_text, ": ", 2);

                        /* Create string with network list */
                        for (i = 0; i < data_p->num_of_networks; i++) {
                            if (i > 0) {
                                (void) strncat((char *) info_text, ",", 1);
                            }

                            /* Begin new operator info */
                            (void) strncat((char *) info_text, "(", 1);
                            /* Append <stat> */
                            (void) strncat(
                                (char *) info_text,
                                data_p->completelist_p[i].stat,
                                strlen(
                                    data_p->completelist_p[i].stat));
                            /* Append long alphanumeric <oper> */
                            (void) strncat((char *) info_text, ",\"", 2);
                            (void) strncat(
                                (char *) info_text,
                                data_p->completelist_p[i].long_alphanumeric,
                                strlen(
                                    data_p->completelist_p[i].long_alphanumeric));
                            (void) strncat((char *) info_text, "\"", 1);
                            /* Append short alphanumeric <oper> */
                            (void) strncat((char *) info_text, ",\"", 2);
                            (void) strncat(
                                (char *) info_text,
                                data_p->completelist_p[i].short_alphanumeric,
                                strlen(
                                    data_p->completelist_p[i].short_alphanumeric));
                            (void) strncat((char *) info_text, "\"", 1);
                            /* Append numeric <oper> */
                            (void) strncat((char *) info_text, ",\"", 2);
                            (void) strncat(
                                (char *) info_text,
                                data_p->completelist_p[i].numeric,
                                strlen(
                                    data_p->completelist_p[i].numeric));
                            (void) strncat((char *) info_text, "\"", 1);
                            /* Append <AcT> */
                            (void) strncat((char *) info_text, ",", 1);
                            (void) strncat(
                                (char *) info_text,
                                data_p->completelist_p[i].act,
                                strlen(
                                    data_p->completelist_p[i].act));
                            /* End of operator info */
                            (void) strncat((char *) info_text, ")", 1);
                        }

                        result = AT_OK;
                        Parser_SendResponse(parser_p, AT_NO_OVERRIDE,
                                            info_text);
                    } else {
                        ATC_LOG_E("no data in message");
                    }
                } else {
                    ATC_LOG_E("unsuccessful response");

                    exe_cops_response_data_t *response_p = (exe_cops_response_data_t *)message->data_p;

                    if (response_p != NULL) {
                        Parser_SetCMEE_ErrorType(parser_p, response_p->error_code);
                    } else {
                        Parser_SetCMEE_ErrorType(parser_p, CMEE_OPERATION_NOT_ALLOWED);
                    }

                    result = AT_CME_ERROR;
                }
            } else {
                ATC_LOG_E("Message is NULL");
            }
        }
        break;
        default:
            ATC_LOG_E("Unknown operation mode");
            break;
        }
    }

    return result;
}

/*===============================================================================
 *
 *   Function: AT_PLUS_CSQ_Handle
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
 *   The CSQ command is used for quering the current received signal strength.
 *
 *   DO   - Returns the current RSSI:
 *          +CSQ: <rssi>,<ber>
 *            <rssi> signal strength (0 = -113dBm, 1 = -111dBm, ..., 31 = -51dBm).
 *            <ber> channel bit error rate (0-7 according to GSM 05.08).
 *   TEST - Returns the supported range for the RSSI and BER:
 *          +CSQ: (0-31,99),(0-7,99)
 *===============================================================================
 */

AT_Command_e AT_PLUS_CSQ_Handle(AT_ParserState_s *parser_p,
                                AT_CommandLine_t info_text,
                                AT_ResponseMessage_t message)
{
    exe_t      *exe_p      = NULL;
    exe_csq_data_t *csq_data_p = NULL;
    AT_Command_e result    = AT_OK;
    exe_request_result_t exe_result = EXE_SUCCESS;

    if (!parser_p) {
        ATC_LOG_E("AT_PLUS_CSQ_Handle: parser_p is NULL!\n");
        SET_RESULT_AND_GOTO_EXIT(AT_ERROR);
    }

    switch (parser_p->Mode) {
    case AT_MODE_DO:

        if (!parser_p->IsPending) {
            exe_p = atc_get_exe_handle();

            if (!exe_p) {
                ATC_LOG_E("AT_PLUS_CSQ_Handle: unable to acquire executor context!\n");
                SET_RESULT_AND_GOTO_EXIT(AT_ERROR);
            }

            exe_result = exe_request(exe_p, EXE_CSQ, NULL,
                                     (void *)parser_p, &parser_p->request_handle);

            if (exe_result != EXE_PENDING) {
                Parser_SetCMEE_ErrorType(parser_p,
                                         CMEE_OPERATION_NOT_ALLOWED);
                SET_RESULT_AND_GOTO_EXIT(AT_CME_ERROR);
            }

            result = AT_PENDING;
        } /* if (!parser_p->IsPending) */
        else {
            if (message->exe_result_code != EXE_SUCCESS) {
                Parser_SetCMEE_ErrorType(parser_p,
                                         CMEE_OPERATION_NOT_ALLOWED);
                SET_RESULT_AND_GOTO_EXIT(AT_CME_ERROR);
            }

            if (!message->data_p) {
                ATC_LOG_E("AT_PLUS_CSQ_Handle: message.data is NULL!\n");
                SET_RESULT_AND_GOTO_EXIT(AT_ERROR);
            }

            csq_data_p = (exe_csq_data_t *) message->data_p;
            /* Display signal strength values from modem (the executor makes sure that the values are legal) */
            AT_AddValue(info_text, ": 0", csq_data_p->rssi);
            AT_AddValue(info_text, ",0", csq_data_p->ber);
            Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        }

        break;
    case AT_MODE_TEST:
        sprintf((char *) info_text, ": (0-31,99),(0-7,99)");
        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        break;
    default: /* including AT_MODE_READ */
        ATC_LOG_E("AT_PLUS_CSQ_Handle: invalid mode!")
        ;
        result = AT_ERROR;
        break;
    }

exit:
    return result;
}

/*
 *===============================================================================
 *
 *   Function: AT_STAR_ECME_Handle
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
 *   The ECME turns on/off reporting of network related unsolicited reject cause codes.
 *
 *   SET  - Activates/Deactivates unsolicited responses.
 *   READ - Shows the current status for CME error unsolicited error reporting.
 *   TEST - Show list of supported set values.
 *
 *===============================================================================
 */
AT_Command_e AT_STAR_ECME_Handle(AT_ParserState_s *parser_p, AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message)
{
    AT_Command_e result = AT_ERROR;

    switch (parser_p->Mode) {
    case AT_MODE_READ:
        sprintf((char *)info_text, ": %d", parser_p->ECME);
        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        result = AT_OK;
        break;

    case AT_MODE_SET:
    case AT_MODE_TEST:
        result = AT_Flag_Handle(parser_p, info_text, &parser_p->ECME, 1);
        break;

    default:
        Parser_SetCMEE_ErrorType(parser_p, CMEE_UNKNOWN);
        result = AT_CME_ERROR;
        break;
    }

    return result;
}

/*
 * Handle incoming ECME event
 */
void handle_incoming_ecme_event(exe_ecme_response_t *response)
{
    AT_ParserState_s *parser_p = NULL;
    unsigned char entry;
    AT_CommandLine_t infotext = (unsigned char *)calloc(1, ATC_ECME_UNSOL_MIN_RESP_LENGTH + strlen(response->cause_text_p));

    if (!infotext) {
        ATC_LOG_E("%s: infotext is NULL!\n", __FUNCTION__);
        return;
    }

    sprintf((char *)infotext, "*ECME: %d,\"%s\"", response->cause + CME_3GPP_REJECT_CAUSE_OFFSET,
            strlen(response->cause_text_p) == 0 ? "" : response->cause_text_p);

    for (entry = 0; entry < MAX_NO_OF_OPEN_CHANNELS; entry++) {
        parser_p = ParserStateTable_GetParserState_ByEntry(entry);

        if (parser_p->ECME) {
            parser_send_unsolicited(parser_p, AT_TEXT_RESPONSE, infotext);
        }
    }

    free((void *)infotext);
}

void handle_incoming_RSSI_event(void *response_p)
{
#ifdef EXE_USE_ATC_CUSTOMER_EXTENSIONS
    handle_incoming_CCSQ_RSSI_event((exe_ccsq_rssi_data_t *)response_p);
#endif
}

void handle_incoming_ciev_signal_event(void *response_p)
{
    unsigned char           entry;
    AT_ParserState_s        *parser_p  = NULL;
    AT_CommandLine_t        info_text  = NULL;
    AT_Command_e            result     = AT_OK;
    exe_request_result_t    exe_res    = EXE_FAILURE;
    exe_cmer_ciev_data_t    *ciev_data_p = (exe_cmer_ciev_data_t *)response_p;
    ciev_signal_bars_t      signal_quality;

    info_text = (AT_CommandLine_t) calloc(1, AT_MAX_DATA_LENGTH);

    if (NULL == info_text) {
        ATC_LOG_E("info_text failed to calloc");
        goto exit;
    }

    if (NULL == ciev_data_p) {
        ATC_LOG_E("ciev_data_p is NULL");
        goto exit;
    }

    /* Translating the rssi_dbm value into the +CIEV signal value (0-5) */
    if (CIEV_SIGNAL_DBM_BORDER_0 < ciev_data_p->rssi_dbm || CIEV_SIGNAL_DBM_UNDEFINED == ciev_data_p->rssi_dbm) {
        signal_quality = CIEV_0_BARS;
    } else if (CIEV_SIGNAL_DBM_BORDER_1 < ciev_data_p->rssi_dbm) {
        signal_quality = CIEV_1_BAR;
    } else if (CIEV_SIGNAL_DBM_BORDER_2 < ciev_data_p->rssi_dbm) {
        signal_quality = CIEV_2_BARS;
    } else if (CIEV_SIGNAL_DBM_BORDER_3 < ciev_data_p->rssi_dbm) {
        signal_quality = CIEV_3_BARS;
    } else if (CIEV_SIGNAL_DBM_BORDER_4 < ciev_data_p->rssi_dbm) {
        signal_quality = CIEV_4_BARS;
    } else {
        signal_quality = CIEV_5_BARS;
    }

    sprintf((char *)info_text, "+CIEV: 2,%d", signal_quality);

    for (entry = 0; entry < MAX_NO_OF_OPEN_CHANNELS; entry++) {
        parser_p = ParserStateTable_GetParserState_ByEntry(entry);

        if (NULL != parser_p && CMER_MODE_FORWARD == parser_p->CMER_mode && CMER_IND_ACTIVATED == parser_p->CMER_ind) {
            if (signal_quality != parser_p->CIEV_signal) {
                parser_send_unsolicited(parser_p, AT_TEXT_RESPONSE, (AT_CommandLine_t)info_text);
                parser_p->CIEV_signal = signal_quality;
            }
        }
    }

exit:

    free(info_text);
}

/*==============================================================================
 *
 *   Function: AT_STAR_EHSTACT_Handle
 *
 *   INPUT:   parser_p       - Pointer to the current parser state.
 *            info_text      - Pointer to a string buffer to put information text.
 *
 *   OUTPUT:  parser_p->Pos  - The current position in the command line.
 *
 *   RETURNS: A status code for the execution of the command is returned:
 *            AT_OK        - The command was successfully handled.
 *            AT_ERROR     - There where a general error.
 *            AT_CME_ERROR - There where a GSM specific error, the error code is
 *                           returned in ASCII format in info_text.
 *
 *
 *
 *   SET  - Enables or disables the FD feature.
 *   TEST - Writes the supported range for the enable command.
 *
 *=============================================================================
 */
AT_Command_e AT_STAR_EHSTACT_Handle(AT_ParserState_s *parser_p,
                                    AT_CommandLine_t info_text, AT_ResponseMessage_t message)
{
    AT_Command_e result = AT_OK;
    IntParam_t mode = 0; /* unknown mode */
    IntParam_t input;

    switch (parser_p->Mode) {
    case AT_MODE_SET:

        if (!parser_p->IsPending) {

            bool error_flag = false;
            exe_t *exe_p;
            exe_request_result_t exe_res = EXE_FAILURE;

            mode = Parser_GetIntParam(parser_p, &error_flag, NULL);

            if (error_flag || !(parser_p->NoMoreParams)) {
                Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
                return AT_CME_ERROR;
            }

            if (!(1 == mode || 2 == mode)) {
                Parser_SetCMEE_ErrorType(parser_p, CMEE_INCORRECT_PARAMETERS);
                return AT_CME_ERROR;
            }

            exe_p = atc_get_exe_handle();

            if (exe_p == NULL) {
                Parser_SetCMEE_ErrorType(parser_p, CMEE_PHONE_FAILURE);
                return AT_CME_ERROR;
            }

            exe_res = exe_request(exe_p, EXE_EHSTACT,
                                  (void *) &mode, (void *) parser_p, &parser_p->request_handle);
            result = parser_handle_exe_result(exe_res, parser_p);

        } else { /* We have a result...*/
            if (message->exe_result_code != EXE_SUCCESS) {

                Parser_SetCMEE_ErrorType(parser_p, CMEE_OPERATION_NOT_ALLOWED);
                result = AT_CME_ERROR;
            }
        }

        break;

    case AT_MODE_READ :
        result = AT_ERROR;
        break;

    case AT_MODE_TEST:
        sprintf((char *)info_text, ": (1,2)");
        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        break;

    default:
        ATC_LOG_E("AT_STAR_EHSTACT_Handle: Invalid mode!");
        result = AT_ERROR;
        break;
    }

    return result;
}

/*
 *===============================================================================
 *
 *   Function: AT_STAR_EGNCI_Handle
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
 *===============================================================================
 */
AT_Command_e AT_STAR_EGNCI_Handle(AT_ParserState_s *parser_p, AT_CommandLine_t info_text, AT_ResponseMessage_t message)
{
    AT_Command_e result = AT_ERROR;
    int i = 0;
    int32_t vl_rssi = RSSI_VALUE_UNKNOWN;
    unsigned char *pl_temp_p = info_text;
    bool create_string = false;

    switch (parser_p->Mode) {
    case AT_MODE_DO:

        if (!parser_p->IsPending) {

            exe_t *exe_p = NULL;
            exe_request_result_t exe_res = EXE_FAILURE;

            exe_p = atc_get_exe_handle();

            exe_res = exe_request(exe_p, EXE_NEIGH_CELLINFO_2G_GET,
                                  NULL, (void *) parser_p, &parser_p->request_handle);

            result = parser_handle_exe_result(exe_res, parser_p);
        } else { /* parser_p->IsPending */
            if (NULL != message) {
                result = parser_handle_exe_result(message->exe_result_code, parser_p);
            } else {
                result = AT_ERROR;
            }

            if (AT_OK == result && NULL != message->data_p) {
                create_string = true;
            }
        }

        if (create_string) {

            exe_2g_3g_neigh_cell_response_t *data_p = (exe_2g_3g_neigh_cell_response_t *) message->data_p;
            exe_cell_information_response_t *neigh_next_cell_info_p;

            if (data_p->rat_info == EXE_NEIGHBOURING_RAT_TYPE_2G) { /* < 0 - Unknown; 1 - GSM; 2 - UMTS */
                /* Add list of ncell id to the response. we need to add only for six neighboring cells  */
                neigh_next_cell_info_p = data_p->neigh_cell_info_p;

                /* we need to report only best ATC_EGNCI_NUMBER_OF_NEIGHBORS neighbor cells so if more than ATC_EGNCI_NUMBER_OF_NEIGHBORS we
                 * recieved then truncate the number of cell to ATC_EGNCI_NUMBER_OF_NEIGHBORS */
                if (data_p->num_of_cells > ATC_EGNCI_NUMBER_OF_NEIGHBORS) {
                    data_p->num_of_cells = ATC_EGNCI_NUMBER_OF_NEIGHBORS;
                }

                for (i = 0; i < data_p->num_of_cells; i++) {
                    /* Add neighbor PLMN as string to the response  */
                    pl_temp_p +=  snprintf((char *)pl_temp_p, sizeof(char) * 22, ": \"%d%d\"", neigh_next_cell_info_p->gsm_info.mcc,
                                           neigh_next_cell_info_p->gsm_info.mnc);

                    /* Add neighbor LAC in hex format to the response  */
                    pl_temp_p +=  snprintf((char *)pl_temp_p, sizeof(char) * 22, ",\"%04X\"", neigh_next_cell_info_p->gsm_info.lac);

                    /* Add neighbor cell id in hex format to the response  */
                    pl_temp_p +=  snprintf((char *)pl_temp_p, sizeof(char) * 20, ",\"%08X\"", neigh_next_cell_info_p->gsm_info.cid);

                    /* Add neighbor cell Received RSSI to the response  */
                    vl_rssi = neigh_next_cell_info_p->gsm_info.rxlev;

                    if (vl_rssi < 0) {
                        if (vl_rssi < -110) {
                            vl_rssi = -110;
                        }

                        if (vl_rssi > -47) {
                            vl_rssi = -47;
                        }
                        vl_rssi += 110;
                    }

                    pl_temp_p += snprintf((char *)pl_temp_p, sizeof(char) * 7, ",,,%d", vl_rssi);
                    neigh_next_cell_info_p ++;

                    if (i == data_p->num_of_cells - 1) {
                        /* Final response */
                        result = AT_OK;
                        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
                    } else {
                        /* More neighboring cells to come, reset pl_temp_p */
                        pl_temp_p = info_text;
                        Parser_SendMultipleResponses(parser_p, AT_NO_OVERRIDE, info_text, false);
                    }
                }
            } else {
                result = AT_CME_ERROR;
                Parser_SetCMEE_ErrorType(parser_p, CMEE_OPERATION_NOT_ALLOWED);
            }
        }

        break;

    case AT_MODE_READ :
        break;
    case AT_MODE_TEST :
        result = AT_OK;
        break;
    default :
        ATC_LOG_E("AT_STAR_EGNCI_Handle");
        result = AT_ERROR;
        break;

    }

    return result;
}

/*
 *===============================================================================
 *
 *   Function: AT_STAR_EWNCI_Handle
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
 *===============================================================================
 */
AT_Command_e AT_STAR_EWNCI_Handle(AT_ParserState_s *parser_p, AT_CommandLine_t info_text, AT_ResponseMessage_t message)
{
    AT_Command_e result = AT_ERROR;
    int i = 0;
    uint32_t vl_psc = 0;
    bool create_string = false;
    int32_t vl_rscp;
    int8_t vl_report_value;
    unsigned char *pl_temp_p = info_text;

    switch (parser_p->Mode) {
    case AT_MODE_DO:

        if (!parser_p->IsPending) {

            exe_t *exe_p = NULL;
            exe_request_result_t exe_res = EXE_FAILURE;
            exe_p = atc_get_exe_handle();

            exe_res = exe_request(exe_p, EXE_NEIGH_CELLINFO_3G_GET,
                                  NULL, (void *) parser_p, &parser_p->request_handle);
            result = parser_handle_exe_result(exe_res, parser_p);
        } else { /* parser_p->IsPending */
            if (NULL != message) {
                result = parser_handle_exe_result(message->exe_result_code, parser_p);
            } else {
                result = AT_ERROR;
            }

            if (AT_OK == result && NULL != message->data_p) {
                create_string = true;
            }
        }

        if (create_string) {

            exe_2g_3g_neigh_cell_response_t *data_p = (exe_2g_3g_neigh_cell_response_t *) message->data_p;
            exe_cell_information_response_t *neigh_next_cell_info_p;

            if (data_p->rat_info == EXE_NEIGHBOURING_RAT_TYPE_3G) { /**< 0 - Unknown; 1 - GSM; 2 - UMTS */
                neigh_next_cell_info_p = data_p->neigh_cell_info_p;

                /* we need to report only best ATC_EWNCI_NUMBER_OF_NEIGHBORS neighbor cells so if more than ATC_EWNCI_NUMBER_OF_NEIGHBORS we
                 * received then truncate the number of cell to ATC_EWNCI_NUMBER_OF_NEIGHBORS */
                if (data_p->num_of_cells > ATC_EWNCI_NUMBER_OF_NEIGHBORS) {
                    data_p->num_of_cells = ATC_EWNCI_NUMBER_OF_NEIGHBORS;
                }

                /* Add list of ncell id to the response. we need to add only for six neighboring cells  */
                for (i = 0; i < data_p->num_of_cells; i++) {

                    pl_temp_p += snprintf((char *)pl_temp_p, sizeof(char) * 12, ": %d", neigh_next_cell_info_p->umts_info.dl_uarfcn);
                    pl_temp_p += snprintf((char *)pl_temp_p, sizeof(char) * 12, ",%d", neigh_next_cell_info_p->umts_info.psc);

                    /* Add neighbor cell Received RSSI to the response  */
                    vl_rscp = neigh_next_cell_info_p->umts_info.cpich_rscp;
                    /* range of value from CN is -120 to 11 */
                    if (vl_rscp < -120) {
                        /* No value with id -127 is thereby reported as 0 */
                        vl_rscp = -120;
                    } else if (vl_rscp > 11) {
                        vl_rscp = 11;
                    }
                    vl_report_value = (int8_t)((((double)(120 + vl_rscp) / 131)) * 91);

                    pl_temp_p += snprintf((char *)pl_temp_p, sizeof(char) * 12, ",%d", vl_report_value);
                    pl_temp_p += snprintf((char *)pl_temp_p, sizeof(char) * 12, ",%d", neigh_next_cell_info_p->umts_info.cpich_ecno);
                    pl_temp_p += snprintf((char *)pl_temp_p, sizeof(char) * 12, ",%d", neigh_next_cell_info_p->umts_info.cpich_pathloss);

                    neigh_next_cell_info_p ++;

                    if (i == data_p->num_of_cells - 1) {
                        /* Final response */
                        result = AT_OK;
                        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
                    } else {
                        /* More neighboring cells to come, reset pl_temp_p */
                        pl_temp_p = info_text;
                        Parser_SendMultipleResponses(parser_p, AT_NO_OVERRIDE, info_text, false);
                    }
                }
            } else {
                result = AT_CME_ERROR;
                Parser_SetCMEE_ErrorType(parser_p, CMEE_OPERATION_NOT_ALLOWED);
            }
        }

        break;

    case AT_MODE_READ :
        break;

    case AT_MODE_TEST :
        result = AT_OK;
        break;

    default :
        ATC_LOG_E("AT_STAR_EWNCI_Handle");
        result = AT_ERROR;
        break;
    }

    return result;
}

/*
 *===============================================================================
 *
 *   Function: AT_STAR_EGSCI_Handle
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
 *===============================================================================
 */
AT_Command_e AT_STAR_EGSCI_Handle(AT_ParserState_s *parser_p, AT_CommandLine_t info_text, AT_ResponseMessage_t message)
{
    AT_Command_e result = AT_ERROR;
    int i = 0;
    int32_t vl_rssi = RSSI_VALUE_UNKNOWN;
    unsigned char *pl_temp_p = info_text;
    bool create_string = false;

    switch (parser_p->Mode) {
    case AT_MODE_DO:

        if (!parser_p->IsPending) {

            exe_t *exe_p = NULL;
            exe_request_result_t exe_res = EXE_FAILURE;

            exe_p = atc_get_exe_handle();

            exe_res = exe_request(exe_p, EXE_SERVING_CELLINFO_2G_GET,
                                  NULL, (void *) parser_p, &parser_p->request_handle);

            result = parser_handle_exe_result(exe_res, parser_p);
        } else { /* parser_p->IsPending */
            if (NULL != message) {
                result = parser_handle_exe_result(message->exe_result_code, parser_p);
            } else {
                result = AT_ERROR;
            }

            if (AT_OK == result && NULL != message->data_p) {
                create_string = true;
            }
        }

        if (create_string) {

            exe_2g_3g_neigh_cell_response_t *data_p = (exe_2g_3g_neigh_cell_response_t *) message->data_p;
            /*Response Format: *EGSCI: <ARFCN>,<BSIC>,<RxLevel>[,<TimingAdvance>]*/
            if (data_p->rat_info == EXE_NEIGHBOURING_RAT_TYPE_2G) { /* < 0 - Unknown; 1 - GSM; 2 - UMTS */

                /* Add neighbor cell Received RSSI to the response  */
                vl_rssi = data_p->serving_cell_info.gsm_info.rxlev;

                if (vl_rssi < 0) {
                    if (vl_rssi < -110) {
                        vl_rssi = -110;
                    }

                    if (vl_rssi > -47) {
                        vl_rssi = -47;
                    }
                    vl_rssi += 110;
                }
                sprintf((char *)info_text, ": %d,%d,%d,%d",
                        data_p->serving_cell_info.gsm_info.arfcn,
                        data_p->serving_cell_info.gsm_info.bsic,
                        vl_rssi,
                        data_p->serving_cell_info.gsm_info.timing_advance);

                Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
                result = AT_OK;
            } else {
                result = AT_CME_ERROR;
                Parser_SetCMEE_ErrorType(parser_p, CMEE_OPERATION_NOT_ALLOWED);
            }
        }

        break;

    case AT_MODE_READ :
        break;
    case AT_MODE_TEST :
        result = AT_OK;
        break;
    default :
        ATC_LOG_E("AT_STAR_EGSCI_Handle");
        result = AT_ERROR;
        break;

    }

    return result;
}

/*
 *===============================================================================
 *
 *   Function: AT_STAR_EWSCI_Handle
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
 *===============================================================================
 */
AT_Command_e AT_STAR_EWSCI_Handle(AT_ParserState_s *parser_p, AT_CommandLine_t info_text, AT_ResponseMessage_t message)
{
    AT_Command_e result = AT_ERROR;
    int i = 0;
    uint32_t vl_psc = 0;
    bool create_string = false;
    int32_t vl_rscp;
    int8_t vl_report_value;
    unsigned char *pl_temp_p = info_text;

    switch (parser_p->Mode) {
    case AT_MODE_DO:

        if (!parser_p->IsPending) {

            exe_t *exe_p = NULL;
            exe_request_result_t exe_res = EXE_FAILURE;
            exe_p = atc_get_exe_handle();

            exe_res = exe_request(exe_p, EXE_SERVING_CELLINFO_3G_GET,
                                  NULL, (void *) parser_p, &parser_p->request_handle);
            result = parser_handle_exe_result(exe_res, parser_p);
        } else { /* parser_p->IsPending */
            if (NULL != message) {
                result = parser_handle_exe_result(message->exe_result_code, parser_p);
            } else {
                result = AT_ERROR;
            }

            if (AT_OK == result && NULL != message->data_p) {
                create_string = true;
            }
        }

        if (create_string) {

            exe_2g_3g_neigh_cell_response_t *data_p = (exe_2g_3g_neigh_cell_response_t *) message->data_p;
            /*Responce format: *EWSCI: <UARFCN>,<PrimaryScramblingCode>[,<RSCP>,<ECNO>,<PathLoss>]*/
            if (data_p->rat_info == EXE_NEIGHBOURING_RAT_TYPE_3G) { /**< 0 - Unknown; 1 - GSM; 2 - UMTS */
                /* Add neighbor cell Received RSSI to the response  */
                vl_rscp = data_p->serving_cell_info.umts_info.cpich_rscp;
                /* range of value from CN is -120 to 11 */
                if (vl_rscp < -120) {
                    /* No value with id -127 is thereby reported as 0 */
                    vl_rscp = -120;
                } else if (vl_rscp > 11) {
                    vl_rscp = 11;
                }
                vl_report_value = (int8_t)((((double)(120 + vl_rscp) / 131)) * 91);
                sprintf((char *)info_text, ": %d,%d,%d,%d,%d",
                        data_p->serving_cell_info.umts_info.dl_uarfcn,
                        data_p->serving_cell_info.umts_info.psc,
                        vl_report_value,
                        data_p->serving_cell_info.umts_info.cpich_ecno,
                        data_p->serving_cell_info.umts_info.cpich_pathloss);

                Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
                result = AT_OK;
            } else {
                result = AT_CME_ERROR;
                Parser_SetCMEE_ErrorType(parser_p, CMEE_OPERATION_NOT_ALLOWED);
            }
        }

        break;

    case AT_MODE_READ :
        break;

    case AT_MODE_TEST :
        result = AT_OK;
        break;

    default :
        ATC_LOG_E("AT_STAR_EWSCI_Handle");
        result = AT_ERROR;
        break;
    }

    return result;
}

/*
 *===============================================================================
 *
 *   Function: AT_PLUS_ECRAT_Handle
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
 *   The *ECRAT function is used to query the current RAT name and to enable
 *   unsolicited result codes when the RAT name is changed.
 *
 *   SET  - Enables ('1') or disables ('0') the unsolicited result codes.
 *   READ - Displays whether the unsolicited result code is enabled or disabled
 *          and the current RAT name.
 *   TEST - Writes the supported range for the SET command.
 *
 *
 *===============================================================================
 */
AT_Command_e AT_STAR_ECRAT_Handle(AT_ParserState_s *parser_p,
                                  AT_CommandLine_t info_text,
                                  AT_ResponseMessage_t message)
{
    AT_Command_e result = AT_ERROR;

    switch (parser_p->Mode) {
    case AT_MODE_SET: {
        if (!parser_p->IsPending) {
            bool error_flag = false;
            exe_request_result_t exe_res = EXE_FAILURE;
            exe_ecrat_set_t ecrat;

            ecrat.n = Parser_GetIntParam(parser_p, &error_flag, NULL);

            if (error_flag) {
                ATC_LOG_E("could not parse <n> parameter!");
                Parser_SetCMEE_ErrorType(parser_p, CMEE_OPERATION_NOT_SUPPORTED);
                result = AT_CME_ERROR;
                break;
            }

            /* Do not accept more than one parameter */
            if (!parser_p->NoMoreParams) {
                Parser_SetCMEE_ErrorType(parser_p, CMEE_OPERATION_NOT_SUPPORTED);
                result = AT_CME_ERROR;
                break;
            }

            /* check parameter range of <n> */
            if (!(0 == ecrat.n || 1 == ecrat.n)) {
                ATC_LOG_E("bad parameter value!");
                Parser_SetCMEE_ErrorType(parser_p, CMEE_OPERATION_NOT_SUPPORTED);
                result = AT_CME_ERROR;
                break;
            }

            /* Save parameter <n> */
            parser_p->ECRAT = (unsigned char)ecrat.n;

            exe_res = exe_request(atc_get_exe_handle(),
                                  EXE_ECRAT_SET,
                                  &ecrat,
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

    case AT_MODE_READ: {

        if (!parser_p->IsPending) {
            exe_request_result_t exe_res = exe_request(atc_get_exe_handle(),
                                           EXE_ECRAT_READ,
                                           NULL,
                                           (void *)parser_p,
                                           &parser_p->request_handle);

            if (EXE_PENDING == exe_res) {
                result = AT_PENDING;
            }

        } else {
            if (NULL != message) {
                if (EXE_SUCCESS == ((AT_ResponseMessage_t) message)->exe_result_code) {
                    exe_ecrat_read_t *data_p = (exe_ecrat_read_t *) message->data_p;

                    if (NULL != data_p) {
                        /* Prepare info_text with response format "*ECRAT: <n>,<rat>" */
                        AT_AddValue(info_text, ": 0", parser_p->ECRAT);
                        AT_AddValue(info_text, ",0", data_p->rat);

                        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
                        result = AT_OK;
                    } else {
                        ATC_LOG_E("%s: data_p in message is NULL", __FUNCTION__);
                        Parser_SetCMEE_ErrorType(parser_p, CMEE_UNKNOWN);
                        result = AT_CME_ERROR;
                    }
                } else {
                    ATC_LOG_E("%s: unsuccessful response", __FUNCTION__);
                    Parser_SetCMEE_ErrorType(parser_p, CMEE_OPERATION_NOT_ALLOWED);
                    result = AT_CME_ERROR;
                }
            } else {
                ATC_LOG_E(": message in response is NULL", __FUNCTION__);
                Parser_SetCMEE_ErrorType(parser_p, CMEE_UNKNOWN);
                result = AT_CME_ERROR;
            }
        }

        break;
    } /* case AT_MODE_READ: */

    case AT_MODE_TEST: {
        strcpy((char *)info_text, ": (0,1)");
        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        result = AT_OK;
        break;
    } /* case AT_MODE_TEST: */

    default:
        break;
    } /* switch (parser_p->Mode) */

    return result;
} /* AT_Command_e AT_PLUS_CREG_Handle */



AT_Command_e AT_STAR_ENNIR_Handle(AT_ParserState_s *parser_p,
                                  AT_CommandLine_t info_text,
                                  AT_ResponseMessage_t message)
{
    AT_Command_e result = AT_ERROR;

    switch (parser_p->Mode) {
    case AT_MODE_SET: {
        if (!parser_p->IsPending) {
            bool error_flag = false;
            exe_request_result_t exe_res = EXE_FAILURE;
            exe_ennir_set_t ennir;

            ennir.n = Parser_GetIntParam(parser_p, &error_flag, NULL);

            if (error_flag) {
                ATC_LOG_E("could not parse <n> parameter!");
                Parser_SetCMEE_ErrorType(parser_p, CMEE_OPERATION_NOT_SUPPORTED);
                result = AT_CME_ERROR;
                break;
            }

            /* Do not accept more than one parameter */
            if (!parser_p->NoMoreParams) {
                Parser_SetCMEE_ErrorType(parser_p, CMEE_OPERATION_NOT_SUPPORTED);
                result = AT_CME_ERROR;
                break;
            }

            /* check parameter range of <n> */
            if (!(0 == ennir.n || 1 == ennir.n)) {
                ATC_LOG_E("bad parameter value!");
                Parser_SetCMEE_ErrorType(parser_p, CMEE_OPERATION_NOT_SUPPORTED);
                result = AT_CME_ERROR;
                break;
            }

            /* Save parameter <n> */
            parser_p->ENNIR = (unsigned char)ennir.n;

            exe_res = exe_request(atc_get_exe_handle(),
                                  EXE_ENNIR_SET,
                                  &ennir,
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

    case AT_MODE_READ: {

        AT_AddValue(info_text, ": 0", parser_p->ENNIR); /* Value for <n> is fixed to enabled */
        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        result = AT_OK;
        break;
    } /* case AT_MODE_READ: */

    case AT_MODE_TEST: {
        strcpy((char *)info_text, ": (0,1)");
        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        result = AT_OK;
        break;
    } /* case AT_MODE_TEST: */

    default:
        break;
    } /* switch (parser_p->Mode) */

    return result;
} /* AT_Command_e AT_STAR_ENNIR_Handle */




/*
 * Handle incoming ECRAT event
 */
void handle_incoming_ecrat_event(exe_ecrat_ind_t *response_p)
{
    AT_ParserState_s *parser_p = NULL;
    unsigned char entry;
    AT_CommandLine_t infotext = NULL;
    exe_ecrat_ind_t *ecrat_p = (exe_ecrat_ind_t *)response_p;

    if (!response_p) {
        ATC_LOG_E("response_p is NULL!");
        goto exit;
    }

    infotext = (unsigned char *)calloc(1, 20);

    if (!infotext) {
        ATC_LOG_E("%s: infotext is NULL!\n", __FUNCTION__);
        goto exit;
    }

    sprintf((char *)infotext, "*ECRAT: %d", ecrat_p->rat);

    for (entry = 0; entry < MAX_NO_OF_OPEN_CHANNELS; entry++) {
        parser_p = ParserStateTable_GetParserState_ByEntry(entry);

        if (parser_p->ECRAT) {
            parser_send_unsolicited(parser_p, AT_TEXT_RESPONSE, infotext);
        }
    }

    free((void *)infotext);

exit:
    return;
}

/*
 * Handle incoming ENNIR event
 */
void handle_incoming_ennir_event(exe_ennir_ind_t *response_p)
{
    AT_ParserState_s *parser_p = NULL;
    unsigned char entry;
    AT_CommandLine_t infotext = NULL;
    exe_ennir_ind_t *ennir_p = (exe_ennir_ind_t *)response_p;
    char text_long[EXE_COPS_OPERATOR_MAX_LONG_LEN + 1] = { '\0' };
    char text_short[EXE_COPS_OPERATOR_MAX_SHORT_LEN + 1] = { '\0' };
    size_t text_long_length = 0;
    size_t text_short_length = 0;
    int conv;
    atc_charset_t input_charset = ATC_CHARSET_GSM7;

    if (!response_p) {
        ATC_LOG_E("response_p is NULL!");
        goto exit;
    }


    if (0 < response_p->name_char_short_length) {

        input_charset = (EXE_NETWORK_NAME_DCS_GSM7 == response_p->short_dcs) ? ATC_CHARSET_GSM7 : ATC_CHARSET_UCS2;
        ATC_LOG_D("input_charset: %d\n", input_charset);
        conv = atc_string_convert(input_charset, (uint8_t *)response_p->text_short, response_p->name_char_short_length,
                                  ATC_CHARSET_UTF8, (uint8_t *)text_short, &text_short_length);

        if (0 == conv) {
            ATC_LOG_E("String conversion failed!");
            goto exit;
        }
    }

    ATC_LOG_D("Short Name: %s\n", text_short);

    if (0 < response_p->name_char_long_length) {
        input_charset = (EXE_NETWORK_NAME_DCS_GSM7 == response_p->long_dcs) ? ATC_CHARSET_GSM7 : ATC_CHARSET_UCS2;
        ATC_LOG_D("input_charset: %d\n", input_charset);
        conv = atc_string_convert(input_charset, (uint8_t *)response_p->text_long, response_p->name_char_long_length,
                                  ATC_CHARSET_UTF8, (uint8_t *)text_long, &text_long_length);

        if (0 == conv) {
            ATC_LOG_E("String conversion failed!");
            goto exit;
        }
    }

    ATC_LOG_D("Long Name: %s\n", text_long);

    infotext = (unsigned char *)calloc(1, 120);

    if (!infotext) {
        ATC_LOG_E("%s: infotext is NULL!\n", __FUNCTION__);
        goto exit;
    }

    ATC_LOG_D("MCC-MNC: %s\n", response_p->mcc_mnc);

    sprintf((char *)infotext, "*ENNIR: %s ,%s ,%s", text_long, text_short, response_p->mcc_mnc);

    for (entry = 0; entry < MAX_NO_OF_OPEN_CHANNELS; entry++) {
        parser_p = ParserStateTable_GetParserState_ByEntry(entry);

        if (parser_p->ENNIR) {
            parser_send_unsolicited(parser_p, AT_TEXT_RESPONSE, infotext);
        }
    }

    free((void *)infotext);

exit:
    return;
}



/*
 * Get highest trigger level for AT+CREG, AT+CGREG and AT*EREG
 */
static int get_highest_trigger_level()
{
    AT_ParserState_s *parser_p = NULL;
    int level = 0;
    int i = 0;

    /* find the highest subscription level in all parser states for CREG, CGREG and EREG */

    for (i = 0; i < MAX_NO_OF_OPEN_CHANNELS; i++) {
        parser_p = ParserStateTable_GetParserState_ByEntry(i);

        if (NULL != parser_p) {
            if ((int)parser_p->CREG > level) {
                level = parser_p->CREG;
            }

            if ((int)parser_p->CGREG > level) {
                level = parser_p->CGREG;
            }

            if ((int)parser_p->EREG > level) {
                level = parser_p->EREG;
            }
        }
    } /* end of for-loop */

    ATC_LOG_D("level: %d", level);
    return level;
}

