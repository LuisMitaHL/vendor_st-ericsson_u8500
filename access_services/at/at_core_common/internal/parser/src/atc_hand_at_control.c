/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>

#include "atc_command_table.h"
#include "atc_config.h"
#include "atc.h"
#include "atc_handlers.h"
#include "atc_log.h"
#include "atc_parser.h"
#include "atc_common.h"
#include "atc_exe_glue.h"
#include "atc_parser_util.h"

static CSCS_CharSetLookupTableRecord_s CSCS_CharSetAlphabetLookupTable[] = {
#ifdef AT_PLUS_CSCS_CHARSET_UTF_8_ENABLED
    {ATC_CHARSET_UTF8, "UTF-8"}
#endif
#ifdef AT_PLUS_CSCS_CHARSET_HEX_ENABLED
    , {ATC_CHARSET_HEX, "HEX"}
#endif
#ifdef AT_PLUS_CSCS_CHARSET_GSM_ENABLED
    , {ATC_CHARSET_GSM7, "GSM"}
#endif
#ifdef AT_PLUS_CSCS_CHARSET_IRA_ENABLED
    , {ATC_CHARSET_IRA, "IRA"}
#endif
#ifdef AT_PLUS_CSCS_CHARSET_8859_1_ENABLED
    , {ATC_CHARSET_8859_1, "8859-1"}
#endif
#ifdef AT_PLUS_CSCS_CHARSET_UCS2_ENABLED
    , {ATC_CHARSET_UCS2, "UCS2"}
#endif
    , {0xff, ""}
};

/*
 *===============================================================================
 *
 *   Function: findCharSetValue
 *
 *   INPUT:  charSetParam_p  - Pointer to string passed as a parameter to the
 *                             CSCS-command.
 *
 *   OUTPUT: charSet_p - The CSCS_CharacterSetAlphabet_e enum value that corresponds
 *                       to the char set sting that is passed in as a parameter
 *                       to the CSCS command (charSetParam_p).
 *
 *   RETURNS: AT_Command_e status value:
 *            AT_OK: A matching CSCS_CharacterSetAlphabet_e enum value could be found.
 *            AT_ERROR: No matching CSCS_CharacterSetAlphabet_e enum value could be found.
 *
 *   Finds the The CSCS_CharacterSetAlphabet_e enum value that corresponds
 *   to the char set sting that is passed in as a parameter to the CSCS command.
 *
 *===============================================================================
 */
AT_Command_e findCharSetValue(StrParam_t charSetParam_p, atc_charset_t *charSet_p)
{
    int i = 0;

    while (strcmp(CSCS_CharSetAlphabetLookupTable[i].cscs_charsetstring_p, "")) {
        if (!strcasecmp(CSCS_CharSetAlphabetLookupTable[i].cscs_charsetstring_p, charSetParam_p)) {
            *charSet_p = (atc_charset_t)CSCS_CharSetAlphabetLookupTable[i].cscs_charsetval;
            return AT_OK;
        }

        i++;
    }

    return AT_ERROR;
}

/*
 *===========================================================================================
 *
 *   Function: findCharSetString
 *
 *   INPUT:  charSet  - The CSCS_CharacterSetAlphabet_e enum value.
 *
 *   OUTPUT: charSetSrting_pp - The character set string that corresponds to the
 *                              CSCS_CharacterSetAlphabet_e enum value that is passed in
 *                              via charSet.
 *
 *   RETURNS: AT_Command_e status value:
 *            AT_OK: A matching character set string could be found.
 *            AT_ERROR: No matching character set string could be found.
 *
 *   Finds a character set string that corresponds to a CSCS_CharacterSetAlphabet_e enum
 *   value passed to this function.
 *
 *===========================================================================================
 */
AT_Command_e findCharSetString(atc_charset_t charSet, StrParam_t *charSetSrting_pp)
{
    int i = 0;

    while (strcmp(CSCS_CharSetAlphabetLookupTable[i].cscs_charsetstring_p, "")) {
        if (CSCS_CharSetAlphabetLookupTable[i].cscs_charsetval == charSet) {
            *charSetSrting_pp = (StrParam_t)CSCS_CharSetAlphabetLookupTable[i].cscs_charsetstring_p;
            return AT_OK;
        }

        i++;
    }

    return AT_ERROR;
}

/*
 *===============================================================================
 *
 *   Function: AT_STAR_Handle
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
 *   The AT* command lists all supported commands.
 *
 *   DO     - Returns a list of all the supported AT commands
 *             format: ATCommand1<CR><LF>ATCommand2....
 *   TEST     - Test command returns OK to confirm that the command is supported.
 *
 *===============================================================================
 */
AT_Command_e AT_STAR_Handle(AT_ParserState_s *parser_p,
                            AT_CommandLine_t info_text, AT_ResponseMessage_t message)
{
    int i;
    atc_context_t *at_context_p;

    switch (parser_p->Mode) {
    case AT_MODE_DO:
        *info_text = '\0';

        for (i = 0; i < AT_COMMAND_TABLE_LENGTH; i++) {
            if (!(AT_CommandTable[i].Class & AT_CLASS_DEBUG)
                    && (AT_MODE_NONE != AT_CommandTable[i].Mode)) {
                Parser_SendMultipleResponses(parser_p, AT_CommandTable[i].Command, /*AT_BUILD_COMMAND_INFO,*/
                                             NULL, AT_COMMAND_TABLE_LENGTH == (i + 1));
            }
        }

        at_context_p = atc_context_get_by_context_id(parser_p->ContextID);

        if (NULL != at_context_p &&
                ATC_CONNECTION_TYPE_EXTERNAL == at_context_p->at_connection_type) {
            /* Handle in AtService.
               This value trigger sending to AtService. */
            parser_p->Command = AT_UNKNOWN_COMMAND;
            return AT_ERROR;
        } else {
            return AT_OK;
        }

        break;
    case AT_MODE_TEST:
        return AT_OK;
        break;
    default:
        return AT_ERROR;
    }
}

/*
 *===============================================================================
 *
 *   Function: AT_PLUS_CSCS_Handle
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
 *   The AT* command sets the current character set for the parser.
 *
 *===============================================================================
 */
AT_Command_e AT_PLUS_CSCS_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text, AT_ResponseMessage_t message)
{
    AT_Command_e result = AT_OK;
    StrParam_t chset;
    bool error_flag = false;
    atc_charset_t *charSetVal_p;
    char *str_p = NULL;

    switch (parser_p->Mode) {
    case AT_MODE_SET: { /* AT+CSCS=<chset> */

        if (AT_ERROR == findCharSetString(CHARACTER_SET_DEFAULT, &str_p)) {
            ATC_LOG_E("Default Character set %d could not be found!", CHARACTER_SET_DEFAULT);
            result = AT_ERROR;
            goto exit;
        }

        chset = Parser_GetStrParam(parser_p, &error_flag, str_p);

        if ((NULL == chset) || error_flag) {
            result = AT_ERROR;
            goto exit;
        }

        if (AT_ERROR == findCharSetValue(chset, &parser_p->cscs_charset)) {
            result = AT_CME_ERROR;
            Parser_SetCMEE_ErrorType(parser_p,
                                     CMEE_INCORRECT_PARAMETERS);
            goto exit;
        }
    }
    break;
    case AT_MODE_READ: { /* AT+CSCS? */
        /* Here we assumt that the output will fit the output string. */
        if (AT_ERROR != findCharSetString(parser_p->cscs_charset, &str_p)) {
            sprintf((char *)info_text, ": \"%s\"", str_p);
        } else {
            ATC_LOG_E("Character set could not be found!");
            result = AT_ERROR;
            goto exit;
        }

        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
    }
    break;
    case AT_MODE_TEST: { /* AT+CSCS=? */
        uint8_t i;
        char tmp_buffer[20];
        bool first_char_set = true;

        for (i = 0; i < ATC_CHARSET_MAX; i++) {
            if (AT_ERROR != findCharSetString(i, &str_p)) {
                if (first_char_set) {
                    sprintf((char *)tmp_buffer, ": (\"%s\"", str_p);
                    first_char_set = false;
                } else {
                    sprintf((char *)tmp_buffer, ",\"%s\"", str_p);
                }

                if ((strlen((char *)info_text) + strlen(tmp_buffer) + 1) < RESULT_TEXT_SIZE) {
                    strcat((char *)info_text, tmp_buffer);
                } else {
                    break;
                }
            }
        }

        strcat((char *)info_text, ")");

        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
    }
    break;
    default: {
        ATC_LOG_E("Unknown parser mode!");
        result = AT_ERROR;
        goto exit;
    }
    }

exit:
    return result;
}


/*
 *===============================================================================
 *
 *   Function: AT_AND_C_Handle
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
 *   The AT* command sets the service reporting control.
 *
 *===============================================================================
 */
AT_Command_e AT_AND_C_Handle(AT_ParserState_s *parser_p,
                             AT_CommandLine_t info_text, AT_ResponseMessage_t message)
{
    return AT_Flag_Handle(parser_p, info_text, &parser_p->AndC, 1);
}


/*
 *===============================================================================
 *
 *   Function: AT_AND_D_Handle
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
 *   The AT* command sets the service reporting control.
 *
 *===============================================================================
 */
AT_Command_e AT_AND_D_Handle(AT_ParserState_s *parser_p,
                             AT_CommandLine_t info_text, AT_ResponseMessage_t message)
{
    return AT_Flag_Handle(parser_p, info_text, &parser_p->AndD, 2);
}


/*
 *===============================================================================
 *
 *   Function: AT_E_Handle
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
 *   The AT* command sets the newline character.
 *
 *===============================================================================
 */
AT_Command_e AT_E_Handle(AT_ParserState_s *parser_p,
                         AT_CommandLine_t info_text, AT_ResponseMessage_t message)
{
    /* support E, E0, E1, E=0, E=1, E?, E=? */

    switch (parser_p->Mode) {
    case AT_MODE_DO:
    case AT_MODE_SET: {
        bool error_flag = false;
        IntParam_t value = 1; /* note: This is the default value. */

        value = Parser_GetIntParam(parser_p, &error_flag, &value);

        if (error_flag || value > 1) {
            ATC_LOG_E("AT_E_Handle: error_flag: %d value: %d", error_flag, value);
            return AT_ERROR;
        }

        atc_context_t *context_p = atc_context_get_by_context_id(parser_p->ContextID);

        if (context_p &&
                context_p->conn &&
                (ATC_CONNECTION_TYPE_EXTERNAL == context_p->at_connection_type)) {
            /* Try to update the external connection */
            if (atc_configure_serial(context_p->conn, (bool)value, parser_p->S3, parser_p->S5)) {
                parser_p->Echo = value;
            } else {
                return AT_ERROR;
            }
        } else {
            parser_p->Echo = value;
        }

        return AT_OK;
    }
    break;
    case AT_MODE_READ: {
        int n = snprintf((char *) info_text, 4, "%d", parser_p->Echo); /* "<xyz>" */

        if (0 >= n || 4 < n) {
            ATC_LOG_E("AT_E_Handle(): error reading echo value");
            return AT_ERROR;
        } else {
            Parser_SendResponse(parser_p, AT_TEXT_RESPONSE, info_text);
            return AT_OK;
        }
    }
    break;
    case AT_MODE_TEST: {
        int n = snprintf((char *) info_text, 8, ": (0-1)"); /* ": <xyz>" */

        if (0 >= n) {
            ATC_LOG_E("AT_E_Handle(): could not build interval string");
            return AT_ERROR;
        }

        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        return AT_OK;
    }
    break;
    default:
        ATC_LOG_E("AT_E_Handle(): unknown parser mode");
        break;
    }

    return AT_OK;
}

/*
 *===============================================================================
 *
 *   Function: AT_AND_F_Handle
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
 *   This command sets a subset of the parameters to default values, which may
 *   take in consideration hardware configuration and other manufacturer-defined
 *   criteria.
 *
 *===============================================================================
 */
AT_Command_e AT_AND_F_Handle(AT_ParserState_s *parser_p,
                             AT_CommandLine_t info_text, AT_ResponseMessage_t message)

{
    /* Support the optional parameter value 0: &F &F0 */
    switch (parser_p->Mode) {
    case AT_MODE_DO:
        /* Intended fall-through */
    case AT_MODE_SET: {
        bool error_flag = false;
        IntParam_t value = 0;
        value = Parser_GetIntParam(parser_p, &error_flag, &value);

        if (error_flag || value != 0) {
            ATC_LOG_E("AT_AND_F_Handle(): Only profile 0 is supported.");
            Parser_SetCMEE_ErrorType(parser_p, CMEE_OPERATION_NOT_SUPPORTED);
            return AT_CME_ERROR;
        }

        Parser_ResetCommandSettings(parser_p);
        return AT_OK;
    }
    case AT_MODE_TEST: {
        int n = snprintf((char *) info_text, 6, ": (0)"); /* ": (0)" */

        if (0 >= n) {
            ATC_LOG_E("AT_AND_F_Handle(): could not build interval string");
            return AT_ERROR;
        }

        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);

        return AT_OK;
    }
    default:
        ATC_LOG_E("AT_AND_F_Handle(): unknown parser mode");
        return AT_ERROR;
    }
}

/*
 *===============================================================================
 *
 *   Function: AT_Q_Handle
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
 *   The AT* command sets the newline character.
 *
 *===============================================================================
 */
AT_Command_e AT_Q_Handle(AT_ParserState_s *parser_p,
                         AT_CommandLine_t info_text, AT_ResponseMessage_t message)
{
    AT_Command_e result = AT_ERROR;
    /* support Q, Q0, Q=0, Q?, Q=? */

    switch (parser_p->Mode) {
    case AT_MODE_DO:
    case AT_MODE_SET: {
        bool error_flag = false;
        IntParam_t value = 0; /* note: This is the default value. */
        value = Parser_GetIntParam(parser_p, &error_flag, &value);

        if (error_flag || value > 0) {
            result = AT_ERROR;
            break;
        }

        parser_p->SuppressResponse = value;
        result = AT_OK;
    }
    break;
    case AT_MODE_READ: {
        int n = snprintf((char *) info_text, 5, "Q: %d", parser_p->SuppressResponse); /* "<xyz>" */

        if (4 != n) {
            ATC_LOG_E("AT_Q_Handle(): error reading SuppressResponse value. n: %d", n);
        } else {
            result = AT_OK;
            Parser_SendResponse(parser_p, AT_TEXT_RESPONSE, info_text);
        }
    }
    break;
    case AT_MODE_TEST: {
        int n = snprintf((char *) info_text, 10, ": (0)"); /* ": <xyz>" */

        if (0 >= n) {
            ATC_LOG_E("AT_Q_Handle(): could not build interval string");
            goto exit;
        }

        result = AT_OK;
        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
    }
    break;
    default:
        ATC_LOG_E("AT_Q_Handle(): unknown parser mode");
        break;
    }

exit:
    return result;
}
/*
 *===============================================================================
 *
 *   Function: AT_PLUS_CR_Handle
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
 *   The AT* command sets the service reporting control.
 *
 *===============================================================================
 */
#ifdef AT_PLUS_CR_ENABLED
AT_Command_e AT_PLUS_CR_Handle(AT_ParserState_s *parser_p,
                               AT_CommandLine_t info_text, AT_ResponseMessage_t message)
{
    return AT_Flag_Handle(parser_p, info_text, &parser_p->CR, 1);
}
#endif /* AT_PLUS_CR_ENABLED */


/*
 *===============================================================================
 *
 *   Function: AT_PLUS_CRC_Handle
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
 *   The AT* command sets the service reporting control.
 *
 *===============================================================================
 */
#ifdef AT_PLUS_CRC_ENABLED
AT_Command_e AT_PLUS_CRC_Handle(AT_ParserState_s *parser_p,
                                AT_CommandLine_t info_text, AT_ResponseMessage_t message)
{
    return AT_Flag_Handle(parser_p, info_text, &parser_p->CRC, 1);
}
#endif /* AT_PLUS_CRC_ENABLED */


/*
 *===============================================================================
 *
 *   Function: AT_S0_Handle
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
 *   The AT* command sets the automatic answer control.
 *
 *===============================================================================
 */
AT_Command_e AT_S0_Handle(AT_ParserState_s *parser_p,
                          AT_CommandLine_t info_text, AT_ResponseMessage_t message)
{
    AT_Command_e result = AT_ERROR;

    switch (parser_p->Mode) {
    case AT_MODE_SET: {
        IntParam_t value = (IntParam_t)parser_p->S0;
        result = parser_parse_int_param(parser_p, info_text, &value, 0, 0, 0);

        if (AT_OK != result) {
            break;
        }

        parser_p->S0 = (unsigned char)value;

        break;
    }

    default: {
        IntParam_t value = (IntParam_t)parser_p->S0;
        result = parser_parse_int_param(parser_p, info_text, &value, 0, 0, 0);
        break;
    }

    } /* switch (parser_p->Mode) */

    return result;
}


/*
 *===============================================================================
 *
 *   Function: AT_S3_Handle
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
 *   The AT* command sets the newline character.
 *
 *===============================================================================
 */
AT_Command_e AT_S3_Handle(AT_ParserState_s *parser_p,
                          AT_CommandLine_t info_text, AT_ResponseMessage_t message)
{
    AT_Command_e result = AT_ERROR;

    switch (parser_p->Mode) {
    case AT_MODE_SET: {
        IntParam_t value = (IntParam_t)parser_p->S3;

        result = parser_parse_int_param(parser_p, info_text, &value, 0, 127, 13);

        if (AT_OK != result) {
            break;
        }

        atc_context_t *context_p = atc_context_get_by_context_id(parser_p->ContextID);

        if (context_p &&
                context_p->conn &&
                (ATC_CONNECTION_TYPE_EXTERNAL == context_p->at_connection_type)) {
            /* Try to update the external connection */
            if (atc_configure_serial(context_p->conn, parser_p->Echo, (char)value, parser_p->S5)) {
                parser_p->S3 = (char)value;
            } else {
                result = AT_ERROR;
            }
        } else {
            parser_p->S3 = (char)value;
        }

        break;
    }

    default: {
        IntParam_t value = (IntParam_t)parser_p->S3;
        result = parser_parse_int_param(parser_p, info_text, &value, 0, 127, 13);
        break;
    }

    } /* switch (parser_p->Mode) */

    return result;
}


/*
 *===============================================================================
 *
 *   Function: AT_S4_Handle
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
 *   The AT* command sets the newline character.
 *
 *===============================================================================
 */
AT_Command_e AT_S4_Handle(AT_ParserState_s *parser_p,
                          AT_CommandLine_t info_text, AT_ResponseMessage_t message)
{
    AT_Command_e result = AT_ERROR;

    switch (parser_p->Mode) {
    case AT_MODE_SET: {
        IntParam_t value = (IntParam_t)parser_p->S4;
        result = parser_parse_int_param(parser_p, info_text, &value, 0, 127, 10);

        if (AT_OK != result) {
            break;
        }

        parser_p->S4 = (char)value;

        break;
    }

    default: {
        IntParam_t value = (IntParam_t)parser_p->S4;
        result = parser_parse_int_param(parser_p, info_text, &value, 0, 127, 10);
        break;
    }

    } /* switch (parser_p->Mode) */

    return result;
}


/*
 *===============================================================================
 *
 *   Function: AT_S5_Handle
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
 *   The AT* command sets the newline character.
 *
 *===============================================================================
 */
AT_Command_e AT_S5_Handle(AT_ParserState_s *parser_p,
                          AT_CommandLine_t info_text, AT_ResponseMessage_t message)
{
    AT_Command_e result = AT_ERROR;

    switch (parser_p->Mode) {
    case AT_MODE_SET: {
        IntParam_t value = (IntParam_t)parser_p->S5;
        result = parser_parse_int_param(parser_p, info_text, &value, 0, 127, 8);

        if (AT_OK != result) {
            break;
        }

        atc_context_t *context_p = atc_context_get_by_context_id(parser_p->ContextID);

        if (context_p &&
                context_p->conn &&
                (ATC_CONNECTION_TYPE_EXTERNAL == context_p->at_connection_type)) {
            /* Try to update the external connection */
            if (atc_configure_serial(context_p->conn, parser_p->Echo, parser_p->S3, (char)value)) {
                parser_p->S5 = (char)value;
            } else {
                result = AT_ERROR;
            }
        } else {
            parser_p->S5 = (char)value;
        }

        break;
    }

    default: {
        IntParam_t value = (IntParam_t)parser_p->S5;
        result = parser_parse_int_param(parser_p, info_text, &value, 0, 127, 8);
        break;
    }

    } /* switch (parser_p->Mode) */

    return result;
}


/*
 *===============================================================================
 *
 *   Function: AT_S6_Handle
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
 *   The AT* command sets the pause before blind dialling.
 *
 *===============================================================================
 */
AT_Command_e AT_S6_Handle(AT_ParserState_s *parser_p,
                          AT_CommandLine_t info_text, AT_ResponseMessage_t message)
{
    AT_Command_e result = AT_ERROR;

    switch (parser_p->Mode) {
    case AT_MODE_SET: {
        IntParam_t value = (IntParam_t)parser_p->S6;
        result = parser_parse_int_param(parser_p, info_text, &value, 2, 10, 2);

        if (AT_OK != result) {
            break;
        }

        parser_p->S6 = (unsigned char)value;

        break;
    }

    default: {
        IntParam_t value = (IntParam_t)parser_p->S6;
        result = parser_parse_int_param(parser_p, info_text, &value, 2, 10, 2);
        break;
    }

    } /* switch (parser_p->Mode) */

    return result;
}


/*
 *===============================================================================
 *
 *   Function: AT_S7_Handle
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
 *   The ATS7 command sets the connection complete timeout.
 *
 *===============================================================================
 */
AT_Command_e AT_S7_Handle(AT_ParserState_s *parser_p,
                          AT_CommandLine_t info_text, AT_ResponseMessage_t message)
{
    AT_Command_e result = AT_ERROR;

    switch (parser_p->Mode) {
    case AT_MODE_SET: {
        IntParam_t value = (IntParam_t)parser_p->S7;
        result = parser_parse_int_param(parser_p, info_text, &value, 1, 255, 50);

        if (AT_OK != result) {
            break;
        }

        parser_p->S7 = (unsigned char)value;

        break;
    }

    default: {
        IntParam_t value = (IntParam_t)parser_p->S7;
        result = parser_parse_int_param(parser_p, info_text, &value, 1, 255, 50);
        break;
    }

    } /* switch (parser_p->Mode) */

    return result;
}


/*
 *===============================================================================
 *
 *   Function: AT_S8_Handle
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
 *   The ATS8 command sets the comma dial modifier time.
 *
 *===============================================================================
 */
AT_Command_e AT_S8_Handle(AT_ParserState_s *parser_p,
                          AT_CommandLine_t info_text, AT_ResponseMessage_t message)
{
    AT_Command_e result = AT_ERROR;

    switch (parser_p->Mode) {
    case AT_MODE_SET: {
        IntParam_t value = (IntParam_t)parser_p->S8;
        result = parser_parse_int_param(parser_p, info_text, &value, 0, 255, 2);

        if (AT_OK != result) {
            break;
        }

        parser_p->S8 = (unsigned char)value;

        break;
    }

    default: {
        IntParam_t value = (IntParam_t)parser_p->S8;
        result = parser_parse_int_param(parser_p, info_text, &value, 0, 255, 2);
        break;
    }

    } /* switch (parser_p->Mode) */

    return result;
}


/*
 *===============================================================================
 *
 *   Function: AT_S10_Handle
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
 *   The ATS10 command sets the automatic disconnect delay.
 *
 *===============================================================================
 */
AT_Command_e AT_S10_Handle(AT_ParserState_s *parser_p,
                           AT_CommandLine_t info_text, AT_ResponseMessage_t message)
{
    AT_Command_e result = AT_ERROR;

    switch (parser_p->Mode) {
    case AT_MODE_SET: {
        IntParam_t value = (IntParam_t)parser_p->S10;
        result = parser_parse_int_param(parser_p, info_text, &value, 1, 254, 2);

        if (AT_OK != result) {
            break;
        }

        parser_p->S10 = (unsigned char)value;

        break;
    }

    default: {
        IntParam_t value = (IntParam_t)parser_p->S10;
        result = parser_parse_int_param(parser_p, info_text, &value, 1, 254, 2);
        break;
    }

    } /* switch (parser_p->Mode) */

    return result;
}


/*
 *===============================================================================
 *
 *   Function: AT_V_Handle
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
 *   The AT* command sets the response format.
 *
 *===============================================================================
 */
AT_Command_e AT_V_Handle(AT_ParserState_s *parser_p,
                         AT_CommandLine_t info_text, AT_ResponseMessage_t message)
{
    AT_Command_e result = AT_ERROR;
    /* support V, V0, V=0, V1, V=1, V?, V=? */

    switch (parser_p->Mode) {
    case AT_MODE_DO:
    case AT_MODE_SET: {
        bool error_flag = false;
        IntParam_t value = 1; /* note: This is the default value. */
        value = Parser_GetIntParam(parser_p, &error_flag, &value);

        if (error_flag || value > 1) {
            result = AT_ERROR;
            break;
        }

        parser_p->VerboseResponse = value;
        result = AT_OK;
    }
    break;
    case AT_MODE_READ: {
        int n = snprintf((char *) info_text, 5, "V: %d", parser_p->VerboseResponse); /* "<xyz>" */

        if (4 != n) {
            ATC_LOG_E("AT_V_Handle(): error reading VerboseResponse value");
        } else {
            result = AT_OK;
            Parser_SendResponse(parser_p, AT_TEXT_RESPONSE, info_text);
        }
    }
    break;
    case AT_MODE_TEST: {
        int n = snprintf((char *) info_text, 8, ": (0-1)"); /* ": <xyz>" */

        if (0 >= n) {
            ATC_LOG_E("AT_V_Handle(): could not build interval string");
            goto exit;
        }

        result = AT_OK;
        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
    }
    break;
    default:
        ATC_LOG_E("AT_V_Handle(): unknown parser mode");
        break;
    }

exit:
    return result;
}


/*
 *===============================================================================
 *
 *   Function: AT_X_Handle
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
 *   The AT* command sets the call progress monitoring control.
 *
 *===============================================================================
 */
AT_Command_e AT_X_Handle(AT_ParserState_s *parser_p,
                         AT_CommandLine_t info_text, AT_ResponseMessage_t message)
{
    unsigned char default_value = 4;
    return AT_Flag_Handle_Default(parser_p, info_text, &parser_p->X, 4, &default_value);
}


/*
 *===============================================================================
 *
 *   Function: AT_Z_Handle
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
 *===============================================================================
 */
AT_Command_e AT_Z_Handle(AT_ParserState_s *parser_p,
                         AT_CommandLine_t info_text, AT_ResponseMessage_t message)
{
    AT_Command_e result = AT_OK;
    bool error_flag = false;

    switch (parser_p->Mode) {
    case AT_MODE_SET:
    case AT_MODE_DO: {
        if (!parser_p->IsPending) {
            IntParam_t n = 0;
            exe_t *exe_p;
            exe_request_result_t exe_res = EXE_FAILURE;

            n = Parser_GetIntParam(parser_p, &error_flag, &n);
            /* Don't check error_flag. If no intParam found just continue. */
            /* Don't check parser_p->NoMoreParams. There could be more at commands in the same line */

            if (n != 0) {
                result = AT_ERROR;
                goto exit;
            }

            exe_p = atc_get_exe_handle();

            if (NULL == exe_p) {
                ATC_LOG_E("AT_Z_Handle NULL == exe_p");
                result = AT_ERROR;
                goto exit;
            }

            exe_res = exe_request(exe_p, EXE_HOOK_ALL, NULL,
                                  (void *)parser_p, &parser_p->request_handle);

            result = parser_handle_exe_result(exe_res, parser_p);
        } else {
            if (EXE_SUCCESS == message->exe_result_code) {
                result = AT_OK;
            } else {
                ATC_LOG_E("AT_Z_Handle(): exe_result_code error");
                result = AT_ERROR;
            }

            Parser_ResetCommandSettings(parser_p);
        }

        break;
    }
    case AT_MODE_TEST: {
        int n = snprintf((char *) info_text, RESULT_TEXT_SIZE, ": (0)");

        if (5 != n) {
            ATC_LOG_E("AT_Z_Handle(): could not build interval string");
            result = AT_ERROR;
        }

        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        break;
    }
    default:
        ATC_LOG_E("AT_Z_Handle default error");
        result = AT_ERROR;
        break;
    }

exit:
    return result;
}
