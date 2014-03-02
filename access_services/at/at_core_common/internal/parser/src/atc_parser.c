/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


/*************************************************************************
 * Includes
 *************************************************************************/

#include <c_type.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>

#include "atc.h"
#include "atc_command_table.h"
#include "atc_common.h"
#include "atc_connection.h"
#include "atc_log.h"
#include "atc_parser.h"
#include "atc_parser_util.h"
#include "atc_string.h"

#ifdef EXE_USE_ATC_CUSTOMER_EXTENSIONS
#include "atc_extended_parser.h"
#endif


/*************************************************************************
 * Types, constants and external variables
 *************************************************************************/
#define RESPONSE_HEADER_LENGTH 2 /* "<S3><S4>" */
#define RESPONSE_TRAILER_LENGTH 2 /* "<S3><S4>" */
#define CMS_ERROR_RESPONSE_LENGTH 25 /* static chars "+CMS ERROR: " + dynamic value 0..999 */

static char *const CMEE_Errors[] = {
    /* Place the error messages in NUMERICAL order */
    /* 1 General errors */
    /*  0*/ "phone failure",
    /*  1*/ "no connection to phone",
    /*  2*/ "phone adaptor link reserved",
    /*  3*/ "operation not allowed",
    /*  4*/ "operation not supported",
    /*  5*/ "PH SIM PIN required",
    /*  6*/ "PH-FSIM PIN required",
    /*  7*/ "PH-FSIM PUK required",
    /*  8*/ "",
    /*  9*/ "",
    /* 10*/ "SIM not inserted",
    /* 11*/ "SIM PIN required",
    /* 12*/ "SIM PUK required",
    /* 13*/ "SIM failure",
    /* 14*/ "SIM busy",
    /* 15*/ "SIM wrong",
    /* 16*/ "incorrect password",
    /* 17*/ "SIM PIN2 required",
    /* 18*/ "SIM PUK2 required",
    /* 19*/ "",
    /* 20*/ "memory or list full",
    /* 21*/ "invalid index within the list",
    /* 22*/ "item not available or list empty",
    /* 23*/ "memory failure",
    /* 24*/ "text string or parameter too long",
    /* 25*/ "invalid characters in text string",
    /* 26*/ "dial string too long",
    /* 27*/ "invalid characters in dial string",
    /* 28*/ "",
    /* 29*/ "",
    /* 30*/ "no network service",
    /* 31*/ "network timeout",
    /* 32*/ "network not allowed - emergency calls only",
    /* 33*/ "",
    /* 34*/ "",
    /* 35*/ "",
    /* 36*/ "",
    /* 37*/ "",
    /* 38*/ "",
    /* 39*/ "",
    /* 40*/ "network personalization PIN required",
    /* 41*/ "network personalization PUK required",
    /* 42*/ "network subset personalization PIN required",
    /* 43*/ "network subset personalization PUK required",
    /* 44*/ "service provider personalization PIN required",
    /* 45*/ "service provider personalization PUK required",
    /* 46*/ "corporate personalization PIN required",
    /* 47*/ "corporate personalization PUK required",
    /* 48*/ "hidden key required",
    /* 49*/ "EAP method not supported",
    /* 50*/ "incorrect parameters",
    /* 51-60*/ "", "", "", "", "", "", "", "", "", "",
    /* 61-70*/ "", "", "", "", "", "", "", "", "", "",
    /* 71-70*/ "", "", "", "", "", "", "", "", "", "",
    /* 81-90*/ "", "", "", "", "", "", "", "", "", "",
    /* 91-99*/ "", "", "", "", "", "", "", "", "",
    /*100*/ "unknown",
    /* 2 GPRS related errors: */
    /* 2.1 Errors related to failure to perform an Attach */
    /*101*/ "",
    /*102*/ "",
    /*103*/ "Illegal MS",
    /*104*/ "",
    /*105*/ "",
    /*106*/ "Illegal ME",
    /*107*/ "GPRS services not allowed",
    /*108*/ "",
    /*109*/ "",
    /*110*/ "",
    /*111*/ "PLMN not allowed",
    /*112*/ "Location area not allowed",
    /*113*/ "Roaming not allowed in this location area",
    /*114-131*/ "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
    /* 2.2 Errors related to failure to Activate a Context */
    /*132*/ "service option not supported",
    /*133*/ "requested service option not subscribed",
    /*134*/ "service option temorarily out of order",
    /*135-147*/ "", "", "", "", "", "", "", "", "", "", "", "", "",
    /*148*/ "unspecified GPRS error",
    /*149*/ "PDP autentication failure",
    /* 2.3 Other GPRS Errors */
    /*150*/ "invalid mobile class",
    /*151*/    "VBS/VGCS not supported by the network",
    /*152*/    "No service subscription on SIM",
    /*153*/    "No subscription for group ID",
    /*154*/    "Group Id not activated on SIM",
    /*155*/    "No matching notification",
    /*156*/    "VBS/VGCS call already present",
    /*157*/    "Congestion",
    /*158*/    "Network failure",
    /*159*/    "Uplink busy",
    /*160*/    "No access rights for SIM file",
    /*161*/    "No subscription for priority",
    /*162*/    "operation not applicable or not possible",
    /*163-170*/ "", "", "", "", "", "", "", "",
    /*171-170*/ "", "", "", "", "", "", "", "", "", "",
    /*181-190*/ "", "", "", "", "", "", "", "", "", "",
    /*191-199*/ "", "", "", "", "", "", "", "", "",
    /*200*/ "SIM/USIM personalization PIN required",
    /*201-210*/ "", "", "", "", "", "", "", "", "", "",
    /*211-220*/ "", "", "", "", "", "", "", "", "", "",
    /*221-230*/ "", "", "", "", "", "", "", "", "", "",
    /*231-240*/ "", "", "", "", "", "", "", "", "", "",
    /*241-250*/ "", "", "", "", "", "", "", "", "", "",
    /*251-260*/ "", "", "", "", "", "", "", "", "", "",
    /*261*/     "",
    /*262*/     "SIM Blocked",
    /*263-270*/ "", "", "", "", "", "", "", "",
    /*271-280*/ "", "", "", "", "", "", "", "", "", "",
    /*281-290*/ "", "", "", "", "", "", "", "", "", "",
    /*291-299*/ "", "", "", "", "", "", "", "", "",
#ifdef AT_HAVE_BT_SUPPORT
    /* 3 Bluetooth related errors: */
    /*300*/ "link/connection not available",
    /*301*/ "no more links/connections allowed",
    /*302*/ "no more multiplex channels available",
    /*303*/ "invalid parameter or out of range",
    /*304*/ "aplication not responding, aborted",
    /*305*/ "link not intended to be towards app.",
    /*306*/ "device already exists",
#endif
    ""
};

static const size_t Max_CMEE_Errors = sizeof(CMEE_Errors) / sizeof(CMEE_Errors[0]);


/* Define a table entry for AT Command Modes. */
typedef struct { /* Structure for AT command mode entries: */
    AT_CommandMode_e Mode; /* ... enumeration of the command mode. */
    char *String; /* ... mode string. */
} AT_ModeTableRecord_s;

/* Declare the table for all supported AT Command Modes. */
static const AT_ModeTableRecord_s AT_ModeTable[] = { { AT_MODE_TEST, "=?" }, /* String as specified by V25.ter & GSM 07.07. */
    { AT_MODE_SET, "=" }, /* String as specified by V25.ter & GSM 07.07. */
    { AT_MODE_READ, "?" } /* String as specified by V25.ter & GSM 07.07. */
};

/* The number of entries in the AT command modes table is constant. */
static const AT_TableSize_t AT_MODE_TABLE_LENGTH = (sizeof(AT_ModeTable)
        / sizeof(AT_ModeTableRecord_s));

AT_ParserState_s ParserStateTable[MAX_NO_OF_OPEN_CHANNELS];


typedef struct { /* Structure for AT response entries:*/
    AT_Command_e     response; /* ... AT command enumeration. */
    char            *verbose_string; /* ... verbose response string. */
    char            *numeric_string; /* ... verbose response string. */
} atc_response_table_record_s;

/* Due to unproven efficiency is this table sorted in the order of
 * "most often used".
 */
const atc_response_table_record_s atc_response_table[] = {
    { AT_OK,            "OK",           "0" },
    { AT_ERROR,         "ERROR",        "4" },
    { AT_CME_ERROR,     "+CME ERROR",   ""  },
    { AT_CMS_ERROR,     "+CMS ERROR",   ""  },
    { AT_BUSY,          "BUSY",         "7" },
    { AT_NO_ANSWER,     "NO ANSWER",    "8" },
    { AT_NO_CARRIER,    "NO CARRIER",   "3" },
    { AT_NO_DIALTONE,   "NO DIALTONE",  "6" },
    { AT_CONNECT,       "CONNECT",      "1" },
    { RING,             "RING",         "2" },
};

/* The number of entries in the AT response table is constant. */
const AT_TableSize_t AT_RESPONSE_TABLE_LENGTH = (sizeof(atc_response_table)
        / sizeof(atc_response_table_record_s));


typedef enum {
    INTEGER,
    HEXADECIMAL,
    BINARY
} input_digit_t;


static int send_cmd_to_atservice(AT_ParserState_s *parser_p);
static bool Parser_Initiate(AT_ParserState_s *parser_p, AT_CommandLine_t Source,
                            AT_LineLength_t *Length);
static bool Parser_GetNextCommand(AT_ParserState_s *parser_p,
                                  AT_Command_e *ResultCode);
static void Parser_Clear(AT_ParserState_s *parser_p);
static AT_CommandLine_t AT_AllocateCmdBuffer(AT_LineLength_t length,
        unsigned char ClientId);
static AT_CommandLine_t AT_FreeCmdBuffer(unsigned char ClientId,
        AT_CommandLine_t Buffer);
static StrParam_t AT_GetCommand(AT_Command_e Command);
static char *AddByteInDecimalAscii(unsigned char *Pos, char *Format,
                                   unsigned char Value, AT_LineLength_t *MaxDigits);
static bool AT_AddRange_func(AT_CommandLine_t InfoText, char *Format,
                             unsigned char Min, unsigned char Max);
static char *AddValueAsAscii(unsigned char *Pos, char *Format,
                             unsigned long Value, AT_IntegerBase_e Base, AT_LineLength_t *MaxDigits);
static bool AT_AddBasedValue_func(AT_CommandLine_t InfoText, char *Format,
                                  signed long Value, AT_IntegerBase_e Base);
unsigned long Parser_GetLongIntParam(AT_ParserState_s *parser_p,
                                     bool *Error_p, unsigned long *Default_p, unsigned long Limit);
static SignedIntParam_t Parser_GetSignedIntParam(AT_ParserState_s *parser_p,
        bool *Error_p, SignedIntParam_t *Default_p);
static StrParam_t Parser_GetDigitParamAsStr(AT_ParserState_s *parser_p,
        bool *Error_p, input_digit_t input_digit, StrParam_t Default_p);
static IntParam_t Parser_GetHexParam(AT_ParserState_s *parser_p, bool *Error_p,
                                     IntParam_t *Default_p);
static bool Parser_IsParsingCommandLine(AT_ParserState_s *parser_p);
static bool Parser_IsAbortable(AT_ParserState_s *parser_p);
static void AT_RemoveComma(AT_CommandLine_t ParamString);
static AT_Command_e Parser_SendSupportedATCommands(AT_ParserState_s *parser_p);
void Parser_SetCMEE_ErrorType(AT_ParserState_s *parser_p,
                              exe_cmee_error_t CMEE_ErrorType);
static void Parser_PendingFIFO_Put(AT_ParserState_s *parser_p,
                                   AT_CommandLine_t Response);
static AT_CommandLine_t Parser_PendingFIFO_Get(AT_ParserState_s *parser_p);
static bool Parser_PendingFIFO_IsEmpty(AT_ParserState_s *parser_p);
static void Parser_PendingFIFO_Flush(AT_ParserState_s *parser_p);
void ParserStateTable_ResetEntry(unsigned char Entry);
static ContextId_t ParserStateTable_GetContextId(unsigned char Entry);
static void parser_send_command_result(AT_ParserState_s *parser_p, AT_Command_e command_result);
static AT_Command_e Parser_GetSuccessResult(AT_ParserState_s *parser_p);
static IntParam_t Parser_GetGenericIntParam(AT_ParserState_s *parser_p,
        bool *Error_p, IntParam_t *Default_p, char AtParameterSeparator,
        char AtCommandSeparator);
static const char *parser_get_cmee_description(exe_cmee_error_t CMEE_ErrorType);


/************************************************************
 * Initiate the parser states.
 ************************************************************/
void init_parser_states(void)
{
    int i;

    for (i = 0; i < MAX_NO_OF_OPEN_CHANNELS; i++) {
        memset(&ParserStateTable[i], 0, sizeof(AT_ParserState_s));
        Parser_SetDefaultState(&ParserStateTable[i]);
    }
}

/*************************************************************************
 * Declaration of functions
 **************************************************************************/

char Parser_GetS3(AT_ParserState_s *parser_p)
{
    return parser_p->S3;
}

char parser_get_s4(AT_ParserState_s *parser_p)
{
    return parser_p->S4;
}

char parser_get_s5(AT_ParserState_s *parser_p)
{
    return parser_p->S5;
}


void Parser_SetConnectionType(AT_ParserState_s *parser_p,
                              atc_connection_type_t connection_type)
{
    parser_p->connection_type = connection_type;
}

/*
 *===============================================================================
 *
 *   Function: AT_AllocateCmdBuffer
 *
 *   INPUT:   Length - Size of the buffer to allocate.
 *            ClientId - Identification of the allocator.
 *
 *   RETURNS: A pointer to the allocated command line buffer.
 *
 *   Allocates a buffer for an AT command line.
 *
 *   Last changed:   970404 UL       Original version
 *                   020208 QCSPARD  Imported from SB
 *
 *===============================================================================
 */
static AT_CommandLine_t AT_AllocateCmdBuffer(AT_LineLength_t length,
        unsigned char ClientId)
{
    return (uint8_t *) malloc(length);
}

/*
 *===============================================================================
 *
 *   Function: AT_FreeCmdBuffer
 *
 *   INPUT:   ClientId - Identification of the allocator.
 *            Buffer - A pointer to the buffer to free.
 *
 *   RETURNS: NULL.
 *
 *   Frees an allocated AT command line buffer.
 *
 *===============================================================================
 */
static AT_CommandLine_t AT_FreeCmdBuffer(unsigned char ClientId,
        AT_CommandLine_t Buffer)
{
    free((void *) Buffer);
    return NULL;
}

/*
 *===============================================================================
 *
 *   Function: AT_GetCommand
 *
 *   INPUT:  Command      - Enumeration of the command to add.
 *
 *   RETURNS: If successful a pointer to the command string is returned,
 *            otherwise NULL is returned.
 *
 *===============================================================================
 */
static StrParam_t AT_GetCommand(AT_Command_e Command)
{
    unsigned char i = 0;

    while ((i < AT_COMMAND_TABLE_LENGTH) && (Command
            != AT_CommandTable[i].Command)) {
        i++;
    }

    if (i < AT_COMMAND_TABLE_LENGTH) {
        return (StrParam_t) AT_CommandTable[i].String;
    }

    return NULL;
} /* End of AT_GetCommand */

/*
 *===============================================================================
 *
 *   Function: AT_Flag_Handle
 *
 *   INPUT:   parser_p - Pointer to the current parser state.
 *            InfoText - Pointer to a string buffer to put information text.
 *            Flag     - Pointer to a storage for the flag.
 *            Limit    - Upper limit for the flag value (typically 1, max 8).
 *
 *   OUTPUT:  parser_p->
 *              Pos    - The current position in the command line.
 *
 *   RETURNS: A status code for the execution of the command is returned:
 *            AT_OK        - The command was successfully handled.
 *            AT_ERROR     - There where a general error.
 *            AT_CME_ERROR - There where a GSM specific error, the error code is
 *                           returned in InfoText.
 *
 *   This function handles any command that manipulates a flag register.
 *   DO   - Updates the flag with the given parameter; '0' - Limit.
 *   SET  - Updates the flag with the given parameter; '0' - Limit.
 *   READ - Writes the current value to the InfoText string buffer.
 *   TEST - Writes the supported range for the flag register to InfoText.
 *   If the character at the current position is ',', ';' or end-of-string,
 *   thus indicating that the default value shall be used,
 *   the flag is set to 0, when a DO or SET command is executed.
 *
 *
 *===============================================================================
 */
AT_Command_e AT_Flag_Handle(AT_ParserState_s *parser_p,
                            AT_CommandLine_t InfoText, unsigned char *Flag, unsigned char Limit)
{
    return AT_Flag_Handle_Default(parser_p, InfoText, Flag, Limit, NULL);
}

/*
 * See AT_Flag_Handle
 * When in DO an SET mode, a default value can be used.
 *
 *   INPUT:   Default  - Default value for the parameter, to use if no parameter is
 *                       found. If no default is supported set Default to NULL.
 */
AT_Command_e AT_Flag_Handle_Default(AT_ParserState_s *parser_p,
                                    AT_CommandLine_t InfoText,
                                    unsigned char *Flag,
                                    unsigned char Limit,
                                    unsigned char *Default)
{
    /* Flags may only have three valid bits! */
    *Flag &= 0x07;

    switch (parser_p->Mode) {
    case AT_MODE_DO:
    case AT_MODE_SET: {
        bool ErrorFlag = 0;
        unsigned char default_flag;

        if (NULL == Default) {
            default_flag = 0;
        } else {
            default_flag = *Default;
        }

        /* Check if the current position in the AT Command buffer contains
         * a digit between 0 and Limit, if that is the case assign it to Flag,
         * which is returned to the caller.
         */
        if ((*parser_p->Pos >= '0') && (*parser_p->Pos <= '0' + Limit)
                && (!is_digit(*(parser_p->Pos + 1)))) {
            *Flag = *parser_p->Pos - '0';
            (parser_p->Pos)++;
        } else if (*parser_p->Pos == '\0') {
           *Flag = default_flag;
        } else if (*parser_p->Pos == ','  || *parser_p->Pos == ';') {
            *Flag = default_flag;
            (parser_p->Pos)++;
        } else {
            ErrorFlag = 1;
            (parser_p->Pos)++;
        }

        if (ErrorFlag) {
            return AT_ERROR;
        }

        return AT_OK;
    } /* End of case AT_MODE_DO: */
    case AT_MODE_TEST: {
        AT_AddRange(InfoText, ": (0-0)", 0, Limit);
        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, InfoText);
        return AT_OK;
    }
    case AT_MODE_READ: {
        AT_AddValue(InfoText, ": 0", *Flag);
        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, InfoText);
        return AT_OK;
    }
    }

    return AT_ERROR;
}

/*
 *===============================================================================
 *
 *   Function: parser_parse_int_param
 *
 *   INPUT:   parser_p - Pointer to the current parser state.
 *            InfoText - Pointer to a string buffer to put information text.
 *            Value_p  - Pointer to a storage for the value.
 *            LoLim    - Lower limit for the value.
 *            UpLim    - Upper limit for the value (>= LoLim).
 *            Default  - Default for the value.
 *
 *   OUTPUT:  parser_p->
 *              Pos    - The current position in the command line.
 *
 *   RETURNS: A status code for the execution of the command is returned:
 *            AT_OK        - The command was successfully handled.
 *            AT_ERROR     - There where a general error.
 *            AT_CME_ERROR - There where a GSM specific error, the error code is
 *                           returned in InfoText.
 *
 *   This function handles any command that manipulates an IntParam_t value.
 *   DO   - Updates the flag with the given parameter; LoLim - UpLim.
 *   SET  - Updates the flag with the given parameter; LoLim - UpLim.
 *   READ - Writes the current value to the InfoText string buffer.
 *   TEST - Writes the supported range for the flag register to InfoText.
 *   If the character at the current position is ',', ';' or end-of-string,
 *   thus indicating that the default value shall be used,
 *   the flag is set to 0, when a DO or SET command is executed.
 *
 *
 *===============================================================================
 */
AT_Command_e parser_parse_int_param(AT_ParserState_s *parser_p,
                                    AT_CommandLine_t InfoText,
                                    IntParam_t *Value_p,
                                    IntParam_t LoLim,
                                    IntParam_t UpLim,
                                    IntParam_t Default)
{
    /* Input parameter validation */
    if (!parser_p) {
        return AT_ERROR;
    }

    if (!Value_p) {
        return AT_ERROR;
    }

    if (LoLim > UpLim) {
        return AT_ERROR;
    }

    if (Default < LoLim ||
            Default > UpLim) {
        return AT_ERROR;
    }

    switch (parser_p->Mode) {
    case AT_MODE_DO:
    case AT_MODE_SET: {
        bool ErrorFlag = 0;
        int value = -1;


        /* Check if the current position in the AT Command buffer contains
         * at least one digit, if that is the case convert it, compare it with
         * the limits, return result to caller.
         */
        if (is_digit(*(parser_p->Pos))) {
            value = atoi((const char *)parser_p->Pos);

            while (is_digit(*(parser_p->Pos))) {
                (parser_p->Pos)++;
            }
        } else if (*parser_p->Pos != END_OF_STRING && *parser_p->Pos != AT_COMMAND_SEPARATOR) {
            ErrorFlag = 1;
            (parser_p->Pos)++;
        } else {
            *Value_p = (IntParam_t)Default;
            parser_p->NoMoreParams = true; /* This command has no more parameters. */
            return AT_OK;
        }

        if (LoLim > value || UpLim < value) {
            ErrorFlag = 1;
        } else {
            *Value_p = (IntParam_t)value;
        }

        if (ErrorFlag) {
            return AT_ERROR;
        }

        return AT_OK;
    } /* End of case AT_MODE_DO: */

    case AT_MODE_TEST:
        AT_AddRange(InfoText, ": (0-0)", LoLim, UpLim);
        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, InfoText);
        return AT_OK;

    case AT_MODE_READ:
        AT_AddValue(InfoText, "0", *Value_p);
        Parser_SendResponse(parser_p, AT_TEXT_RESPONSE, InfoText);
        return AT_OK;

    } /* switch (parser_p->Mode) */

    return AT_ERROR;
} /* End of AT_Value_Handle */

/*
 *===============================================================================
 *
 *   Function: Parser_DecodeEscapeSequences
 *
 *   INPUT:  StartOfParam  - pointer to a string which needs to be recoded.
 *
 *   OUTPUT: StartOfParam  - pointer to a recoded string.
 *
 *   RETURNS:  TRUE is the recoding was successful.
 *
 *   Recodes string from escape sequences to characters in-place.
 *
 *===============================================================================
 */

bool Parser_DecodeEscapeSequences(AT_CommandLine_t StartOfParam)
{
    unsigned char *Source = StartOfParam;
    unsigned char *Dest = StartOfParam;

    while (*Source != END_OF_STRING) {
        /* If there is a "\xx" sequence in a string, we convert it to ASCII xx. */
        if (*Source == '\\') {
            unsigned char SlashChar; /* Value of "\xx" in string parameter. */
            unsigned char TempChar;
            bool DoubleSlash = false; /* True if we found "\\" */
            Source++;
            TempChar = to_upper(*Source);

            if ((TempChar >= '0') && (TempChar <= '9')) {
                SlashChar = TempChar - '0';
            } else if ((TempChar >= 'A') && (TempChar <= 'F')) {
                SlashChar = 10 + TempChar - 'A';
            } else if (TempChar == '\\') {
                /* SB special feature... "\\" is accepted and converted to "\" */
                SlashChar = '\\'; /* 0x5C */
                DoubleSlash = true;
            } else if (TempChar == '\"') {
                /* SB special feature... ' \" ' is accepted and converted to ' " ' */
                SlashChar = '\"'; /* 0x22 */
                DoubleSlash = true;
            } else {
                *Dest = END_OF_STRING;
                return false;
            }

            if (!DoubleSlash) {
                SlashChar <<= 4;
                Source++;
                TempChar = to_upper(*Source);

                if ((TempChar >= '0') && (TempChar <= '9')) {
                    SlashChar += TempChar - '0';
                } else if ((TempChar >= 'A') && (TempChar <= 'F')) {
                    SlashChar += 10 + TempChar - 'A';
                } else {
                    *Dest = END_OF_STRING;
                    return false;
                }
            } /* !DoubleSlash */

            *Dest = SlashChar;
        } /* Backslash */ else {

            *Dest = *Source;
        }

        Source++;
        Dest++;
    } /* while */

    *Dest = END_OF_STRING;
    return true;
}

/*
 *===============================================================================
 *
 *   Function: parser_get_char_param
 *
 *   INPUT:  parser_p - Pointer to the current parser state.
 *           Error    - Pointer to an error flag.
 *           Default  - Default value for the parameter, to use if no parameter is
 *                      found. If no default is supported set Default to NULL.
 *
 *   OUTPUT: Error    - The error flag is set to TRUE if no default value was given
 *                      and the parameter was left out, or if there was an error in
 *                      the parameter syntax.
 *           parser_p->
 *             Pos    - Points to the first character after the parameter and any
 *                      parameter separator.
 *
 *   RETURNS: The character at pos, or the default value if not present.
 *
 *===============================================================================
 */
char_param_t parser_get_char_param(AT_ParserState_s *parser_p,
                                   bool *error_p, char_param_t *default_p)
{
    char character = '\0';

    /* Since the command line has no white spaces we can presume that the first
       character is either a separator, a command line termination character
       parameter digit (or a syntax error).

       If next character is a command separator or a command line termination
       character the default value shall be used.
    */

    if (parser_p->NoMoreParams || (*parser_p->Pos == AT_COMMAND_SEPARATOR)
            || (*parser_p->Pos == AT_PARAMETER_SEPARATOR) || (*parser_p->Pos == END_OF_STRING)) {
        if (*parser_p->Pos == AT_PARAMETER_SEPARATOR) {
            parser_p->Pos++; /* Skip the parameter separator. */
        } else if (*parser_p->Pos == AT_COMMAND_SEPARATOR) {
            parser_p->Pos++; /* Skip the command separator. */
            parser_p->NoMoreParams = true; /* This command has no more parameters. */
        } else {
            parser_p->NoMoreParams = true; /* This command has no more parameters. */
        }

        if (NULL == default_p) {
            *error_p = true;  /* The parameter list is empty and no default is given. */
            return character; /* we return with '/0' here, otherwise we dereference a NULL pointer */
        }

        return *default_p;
    }

    character = *parser_p->Pos;
    /* Find the first character that is not a valid int parameter digit. */
    parser_p->Pos++;

    /* The character following a parameter must be either a separator or a
       command line termination character.
     */
    if (((*parser_p->Pos != AT_COMMAND_SEPARATOR) && (*parser_p->Pos
            != AT_PARAMETER_SEPARATOR) && (*parser_p->Pos != END_OF_STRING))) {
        *error_p = true; /* No string parameter was found. */
    } else if (*parser_p->Pos == AT_PARAMETER_SEPARATOR) {
        parser_p->Pos++; /* Skip the parameter separator. */
    } else if (*parser_p->Pos == AT_COMMAND_SEPARATOR) {
        parser_p->Pos++; /* Skip the parameter separator. */
        parser_p->NoMoreParams = true; /* This command has no more parameters. */
    } else {
        parser_p->NoMoreParams = true; /* This command has no more parameters. */
    }

    return character;
} /* End of parser_get_char_param */

/*
 *===============================================================================
 *
 *   Function: Parser_GetStrParam
 *
 *   INPUT:  parser_p - Pointer to the current parser state.
 *           Error    - Pointer to an error flag.
 *           Default  - Default value for the parameter, to use if no parameter is
 *                      found. If no default is supported set Default to NULL.
 *
 *   OUTPUT: Error    - The error flag is set to TRUE if no default value was given
 *                      and the parameter was left out, or if there was an error in
 *                      the parameter syntax.
 *           parser_p->
 *             Pos    - Points to the first character after the parameter and any
 *                      parameter separator.
 *
 *   RETURNS: A pointer to the extracted parameter. The pointer is only valid
 *            to use as long as parser_p->CommandLine is still allocated.
 *
 *   Replaces the end bounder of the string parameter with an END_OF_STRING
 *   character and returns a pointer to the first character of the string.
 *
 *===============================================================================
 */
StrParam_t Parser_GetStrParam(AT_ParserState_s *parser_p, bool *Error,
                              StrParam_t Default)
{
    AT_CommandLine_t StartOfParam; /* Pointer to the first character of the string. */
    /* Since the command line has no whitespaces we can presume that the first */
    /* character is either a separator, a command line termination character */
    /* or a string parameter bounder (or a syntax error). */
    /* If next character is a command separator or a command line termination */
    /* character the default value shall be used. But first we check that there */
    /* are any parameters to get. */

    if (parser_p->NoMoreParams || (*parser_p->Pos == AT_COMMAND_SEPARATOR)
            || (*parser_p->Pos == AT_PARAMETER_SEPARATOR) || (*parser_p->Pos
                    == END_OF_STRING)) {
        if (*parser_p->Pos == AT_PARAMETER_SEPARATOR) {
            parser_p->Pos++; /* Skip the parameter separator. */
        } else if (*parser_p->Pos == AT_COMMAND_SEPARATOR) {
            parser_p->Pos++; /* Skip the command separator. */
            parser_p->NoMoreParams = 1; /* This command has no more parameters. */
        } else {
            parser_p->NoMoreParams = 1; /* This command has no more parameters. */
        }

        if (NULL == Default) {
            *Error = 1; /* The parameter list is empty and no default is given. */
        }

        return Default;
    }

    /* All string parameters must start with a string parameter bounder character. */
    if (*parser_p->Pos == AT_STRING_PARAM_BOUNDER) {
        uint8_t Previous_c = 0xFF;
        /* Skip the string parameter bounder. */
        parser_p->Pos++;
        StartOfParam = parser_p->Pos;

        while (*(parser_p->Pos) != '\0') {
            if ((*(parser_p->Pos - 1) == '\\') && (Previous_c == '\\')) {
                Previous_c = 0xFF; /* special case for  e.i "\"\\" */
            } else {
                Previous_c = *(parser_p->Pos - 1);
            }

            if ((*(parser_p->Pos) == '\"') && (Previous_c != '\\')) {
                break;
            }

            parser_p->Pos++;

            if ((size_t)(parser_p->Pos - StartOfParam) > parser_p->CommandLineLength) {
                parser_p->Pos = NULL;
                break;
            }
        }

        if (parser_p->Pos != NULL && *(parser_p->Pos) != END_OF_STRING) {
            /* Null terminate the string parameter and move on. */
            *parser_p->Pos++ = END_OF_STRING;

            /* The character following a parameter must be either a separator or a */
            /* command line termination character. */
            if ((*parser_p->Pos != AT_PARAMETER_SEPARATOR) && (*parser_p->Pos
                    != AT_COMMAND_SEPARATOR) && (*parser_p->Pos != END_OF_STRING)) {
                *Error = 1;
            } else if (*parser_p->Pos == AT_PARAMETER_SEPARATOR) {
                parser_p->Pos++; /* Skip the parameter separator. */
            } else if (*parser_p->Pos == AT_COMMAND_SEPARATOR) {
                parser_p->Pos++; /* Skip the command separator. */
                parser_p->NoMoreParams = 1; /* This command has no more parameters. */
            } else {
                parser_p->NoMoreParams = 1; /* This command has no more parameters. */
            }

            /* Recode the escape sequences */
            if (!Parser_DecodeEscapeSequences(StartOfParam)) {
                *Error = 1;
                return NULL;
            }

            return (StrParam_t) StartOfParam;
        } else {
            /* ASSERT(FALSE); */  /* This shall never happen! */
        }
    } /* endif */

    /* No string parameter was found */
    *Error = 1;
    return NULL;
} /* End of Parser_GetStrParam */


/* TODO: Check where this function was in PAS. Maybe id does not belong in the parser.
 *===============================================================================
 *
 *   Function: Parser_SendResponse
 *
 *   INPUT:  parser_p    - Pointer to the current parser state.
 *           Override    - Response code that overrides the current
 *                         Parse->Command.
 *           BuildMode   - Determines how the response will be built:
 *                         AT_BUILD_COMMAND_INFO - The response string will
 *                           start with the command text followed by the
 *                           ParamString.
 *                         AT_BUILD_RESPONSE - In verbose mode the response
 *                           string will start with the response text followed by
 *                           the ParamString. In non verbose mode the response
 *                           will be numeric.
 *                         AT_BUILD_UNSOLICITED_RESPONSE - The response string
 *                           will always start with the response text followed by
 *                           the ParamString, not concerning if verbose or not.
 *           ParamString - Pointer to the parameter string for the response.
 *                          The pointer may be set to NULL if there is no
 *                          additional text to add.
 *
 *   RETURNS: TRUE if the response text was successfully sent,
 *            otherwise FALSE is returned.
 *
 *   Used Parser_BuildResponse to construct the response string.
 *
 *===============================================================================
 */
void Parser_SendResponse(AT_ParserState_s *parser_p, AT_Command_e Override,
                         AT_CommandLine_t ParamString)
{
    Parser_SendMultipleResponses(parser_p, Override, ParamString, true);
}

/*
 *===============================================================================
 *
 *   Function: Parser_SendMultipleResponses
 *
 *   INPUT:  parser_p    - Pointer to the current parser state.
 *           Override    - Response code that overrides the current
 *                         Parse->Command.
 *           BuildMode   - Determines how the response will be built:
 *                         AT_BUILD_COMMAND_INFO - The response string will
 *                           start with the command text followed by the
 *                           ParamString.
 *                         AT_BUILD_RESPONSE - In verbose mode the response
 *                           string will start with the response text followed by
 *                           the ParamString. In non verbose mode the response
 *                           will be numeric.
 *                         AT_BUILD_UNSOLICITED_RESPONSE - The response string
 *                           will always start with the response text followed by
 *                           the ParamString, not concerning if verbose or not.
 *           ParamString - Pointer to the parameter string for the response.
 *                          The pointer may be set to NULL if there is no
 *                          additional text to add.
 *           LastResponse - TRUE if this the last time function will be called
 *                          in current sequence. This causes a final CRLF to be
 *                          added to the Response to the client.
 *
 *   Builds the response string passed to clients. This function supports AT
 *   commands which may have multiple responses to clients, e.g. +CMGL, which
 *   must not have CRLF at the end of the response string if there is more data
 *   to follow. (The CRLF at the start of the response string is required.)
 *
 *===============================================================================
 */
void Parser_SendMultipleResponses(AT_ParserState_s *parser_p, AT_Command_e Override,
                                  AT_CommandLine_t ParamString, bool LastResponse)
{
    StrParam_t CommandName;
    uint16_t ResponseLength;
    char *cmd;
    int nbr_written;
    size_t len;

    switch (Override) {
    case AT_NO_OVERRIDE:
        CommandName = AT_GetCommand(parser_p->Command);
        break;
    case AT_TEXT_RESPONSE:
        CommandName = "";
        break;
    default:
        CommandName = AT_GetCommand(Override);
    }

    ResponseLength = strlen((char *) CommandName);

    if (ParamString != NULL) {
        ResponseLength += strlen((char *) ParamString);
    }

    cmd = (char *) calloc(1, ResponseLength + 5);

    if (!cmd) {
        return;
    }

    strcpy(cmd, "\r\n");

    strcat(cmd, (char *) CommandName);

    if (ParamString != NULL) {
        strcat(cmd, (char *) ParamString);
    }

    /* Make the output response more readable by inserting an extra
     * pair of S3+S4 characters before the final response code.
     *
     * 3GPP 27.007 (specification) stipulates that an extra pair of S3+S4
     * characters are to be output before the final response code.
     *
     * ITU V.250 (recommendation) stipulates that there should not be
     * an empty line (extra S3+S4) before the final response code.
     *
     * Set ENABLE_READABILITY to unconditionally include an extra pair
     * of S3+S4, which will look like an empty line. Unset ENABLE_READABILITY
     * to suppress the empty line (extra S3+S4). This applies tor ITU V.250
     * commands only.
     */
    if (LastResponse) {
#ifdef ENABLE_READABILITY
        strcat(cmd, "\r\n");
#else
        int index = parser_p->CommandTableIndex;

        if (0 > index || !(AT_CommandTable[index].Class & AT_CLASS_BASIC)) {
            strcat(cmd, "\r\n");
        }

#endif /* ENABLE_READABILITY */
    }

    /* Go through the output to find any quotes that needs to be escaped. */
    atc_escape_quotes((uint8_t **)&cmd, strlen(cmd));

    len = strlen(cmd);
    nbr_written = parser_write_formatted_text(parser_p, (unsigned char *) cmd, len);
    free((void *)cmd);
}


/*******************************************************************************
 * Writes text characters with formatting to the anchor.
 *******************************************************************************/
int parser_write_formatted_text(AT_ParserState_s *parser_p,
                                unsigned char *text_p, unsigned int length)
{
    atc_context_t *context_p = NULL;
    int bytes_written = 0;
    unsigned int i;

    /* Only need to do format if S3 or S4 do not have default values */
    if (parser_p->S3 != '\r' || parser_p->S4 != '\n') {
        for (i = 0; i < length; i++) {
            switch (*(text_p + i)) {
            case '\r':
                *(text_p + i) = (unsigned char)parser_p->S3;
                break;
            case '\n':
                *(text_p + i) = (unsigned char)parser_p->S4;
                break;
            default:
                break;
            }
        }
    }

    context_p = atc_context_get_by_context_id(parser_p->ContextID);

    if (context_p) {
        bytes_written = atc_write_to_client(context_p, text_p, length);
    }

    return bytes_written;
}


/*******************************************************************************
 * Writes text characters without formatting it to the anchor.
 *******************************************************************************/
int parser_write_unformatted_text(AT_ParserState_s *parser_p,
                                  unsigned char *text_p, unsigned int length)
{
    atc_context_t *context_p = atc_context_get_by_context_id(parser_p->ContextID);
    int bytes_written = 0;

    if (context_p) {
        bytes_written = atc_write_to_client(context_p, text_p, length);
    }

    return bytes_written;
}


/*
 *===============================================================================
 *
 *   Function: AddByteInDecimalAscii
 *
 *   INPUT:   Pos       - Pointer to a string buffer to put the ASCII digits.
 *            Format    - Pointer to a string that determines the format of the
 *                        ASCII digits.
 *            Value     - Value of the integer to convert to ASCII digits.
 *            MaxDigits - Maximal number of digits that may be added.
 *
 *   OUTPUT:  MaxDigits - Actual number of digits that have been added.
 *
 *   RETURNS: Current position in the format string.
 *            NULL is returned if Value has more than MaxDigits.
 *
 *   This function adds a formatted integer to a string buffer. The format string
 *   consists of a number of '0'. The output string will at least consist of that
 *   number of digits and at most MaxDigits. E.g if Format is "000" and Value is
 *   42 the string "042" will be copied to the string buffer.
 *
 *===============================================================================
 */
static char *AddByteInDecimalAscii(unsigned char *Pos, char *Format,
                                   unsigned char Value, AT_LineLength_t *MaxDigits)
{
    unsigned char i;
    AT_LineLength_t MinDigits; /* The minimal number of digits to print Value. */
    AT_LineLength_t Digits; /* The actual number of digits printed. */
    /* Count the minimum number of digits. */
    MinDigits = 0;

    while (*Format == '0') {
        MinDigits++;
        Format++;
    }

    /* How many digits do we need? */
    Digits = Value > 99 ? 3 : (Value > 9 ? 2 : 1);
    Digits = MinDigits > Digits ? MinDigits : Digits;

    /* Check that there is space in the buffer for the digits. */
    /* If ok we use MaxDigits to return the actual number of digits. */
    if (Digits > *MaxDigits) {
        return NULL;
    } else {
        *MaxDigits = Digits;
    }

    /* Fill the string with the right number of '0's. */
    for (i = 0; i < Digits; i++) {
        *Pos++ = '0';
    }

    while (Value > 0) {
        if (Pos == NULL) {
            break;
        }

        Pos--;
        *Pos = '0' + (Value % 10);
        Value /= 10;
    }

    return Format;
} /* End of AddByteInDecimalAscii */

/*
 *===============================================================================
 *
 *   Function: AT_AddRange_func
 *
 *   INPUT:   InfoText - Pointer to a string buffer to put the ASCII range.
 *            Format   - Pointer to a string that determines the format of the
 *                       ASCII range.
 *            Min      - Lower limit of the range.
 *            Max      - Upper limit of the range.
 *
 *   RETURNS: TRUE if the range was successfully added to the string buffer,
 *            otherwise FALSE is returned.
 *
 *   This function adds a formatted integer range to a string buffer.
 *   The format string shall be built of two groups of '0' (zero's) and any
 *   other printable characters before, after and between the groups. Typically
 *   Format looks like "(0-0)". With a Min of 17 and a max of 42 the "(17-42)"
 *   string will be concatenated to the InfoText string buffer.
 *
 *   By adding '0':s it is possible to decide the minimal number of digits in
 *   the output string. With the previous example, the format string "(000-000)"
 *   would give the output "(017-042)".
 *
 *   It is guaranteed that no more than RESULT_TEXT_SIZE characters will
 *   be added to the InfoText string buffer.
 *
 *===============================================================================
 */
static bool AT_AddRange_func(AT_CommandLine_t InfoText, char *Format,
                             unsigned char Min, unsigned char Max)
{
    bool Success;
    AT_LineLength_t Index; /* Current index in InfoText. */
    AT_LineLength_t BytesLeft; /* Remaining number of characters in the buffer. */
    /* We are going to concatenate the new information to the string. */
    Index = (AT_LineLength_t) strlen((char *) InfoText);
    /* Until the opposite is proved. */
    Success = false;

    /* Copy any non format determing text. */
    while ((Index < RESULT_TEXT_SIZE) && (*Format != '0') && (*Format
            != END_OF_STRING)) {
        InfoText[Index++] = *Format++;
    }

    if (Index < RESULT_TEXT_SIZE) {
        /* The previous check assures that the last parameter is within range. */
        BytesLeft = RESULT_TEXT_SIZE - Index;
        Format = AddByteInDecimalAscii(&InfoText[Index], Format, Min,
                                       &BytesLeft);

        if (Format != NULL) {
            Index += BytesLeft;

            /* Copy any non format determing text. */
            while ((Index < RESULT_TEXT_SIZE) && (*Format != '0')
                    && (*Format != END_OF_STRING)) {
                InfoText[Index++] = *Format++;
            }

            if (Index < RESULT_TEXT_SIZE) {
                BytesLeft = RESULT_TEXT_SIZE - Index;
                Format = AddByteInDecimalAscii(&InfoText[Index], Format, Max,
                                               &BytesLeft);

                if (Format != NULL) {
                    Index += BytesLeft;

                    /* Copy any non format determing text. */
                    while ((Index < RESULT_TEXT_SIZE) && (*Format
                                                          != END_OF_STRING)) {
                        InfoText[Index++] = *Format++;
                    }

                    if (Index < RESULT_TEXT_SIZE) {
                        Success = true;
                    }
                } /* endif */
            } /* endif */
        } /* endif */
    } /* endif */

    /* Null terminate the string. */
    if (Index >= RESULT_TEXT_SIZE) {
        InfoText[RESULT_TEXT_SIZE] = END_OF_STRING;
    } else {
        InfoText[Index] = END_OF_STRING;
    }

    return Success;
} /* End of AT_AddRange_func */

/*
 *===============================================================================
 *
 *   Function: AddValueAsAscii
 *
 *   INPUT:   Pos       - Pointer to a string buffer to put the ASCII digits.
 *            Format    - Pointer to a string that determines the format of the
 *                        ASCII digits.
 *            Value     - Integer to write as an ASCII string.
 *            Base      - Determines the base of the output string.
 *
 *   OUTPUT:  MaxDigits - Actual number of digits that have been added.
 *
 *   RETURNS: Current position in the format string.
 *            NULL is returned if Value has more than MaxDigits.
 *
 *   This function adds a formatted integer to a string buffer. The format string
 *   consists of a number of '0'. The output string will at least consist of that
 *   number of digits and at most MaxDigits. E.g if Format is "000" and Value is
 *   42 the string "042" will be copied to the string buffer.
 *
 *===============================================================================
 */
static char *AddValueAsAscii(unsigned char *Pos, char *Format,
                             unsigned long Value, AT_IntegerBase_e Base, AT_LineLength_t *MaxDigits)
{
    unsigned char i;
    AT_LineLength_t MinDigits; /* The minimal number of digits to print Value. */
    AT_LineLength_t Digits; /* The actual number of digits printed. */
    unsigned char Buffer[12];
    /* Count the minimum number of digits. */
    MinDigits = 0;

    while (*Format == '0') {
        MinDigits++;
        Format++;
    }

    if (MinDigits > *MaxDigits) {
        return NULL;
    }

    /* How many digits do we need? */
    Digits = 0;

    do {
        if (Base == AT_HEX_INT) {
            if ((Value % 0x10) < 10) {
                Buffer[Digits] = '0' + (unsigned char)(Value % 0x10);
            } else {
                Buffer[Digits] = 'A' + (unsigned char)((Value % 0x10) - 10);
            }
        } else {
            Buffer[Digits] = '0' + (unsigned char)(Value % 10);
        }

        Digits++;
        Value = Value / Base;
    } while (Value > 0);

    Buffer[Digits] = END_OF_STRING;

    /* Check that there is space in the buffer for the digits. */
    /* If ok we use MaxDigits to return the actual number of digits. */
    if (Digits > *MaxDigits) {
        return NULL;
    }

    /* Add '0'�s in advance of the number if required. */
    if (MinDigits > Digits) {
        *MaxDigits = MinDigits;

        for (i = 0; i < (MinDigits - Digits); i++) {
            *Pos++ = '0';
        }
    } else {
        *MaxDigits = Digits;
    }

    while (Digits > 0) {
        Digits--;
        *Pos++ = Buffer[Digits];
    };

    return Format;
} /* End of AddValueAsAscii */

/*
 *===============================================================================
 *
 *   Function: AT_AddBasedValue_func
 *
 *   INPUT:   InfoText - Pointer to a string buffer to put the ASCII range.
 *            Format   - Pointer to a string that determines the format of the
 *                       ASCII range.
 *            Value    - Signed long to write as a string.
 *            Base     - Determines the base of the output string.
 *
 *   RETURNS: TRUE if the value was successfully added to the string buffer,
 *            otherwise FALSE is returned.
 *
 *   This function adds a formatted integer to a string buffer. The Base
 *   parameter determines whether the integer shall be added in hexadecimal
 *   form or in decimal form (AT_HEX_INT or AT_DEC_INT).
 *   The format string shall be built of one group of '0' (zero's) and any
 *   other printable characters before and after. Typically Format looks like
 *   ",000". With a Value of 42 and the Base AT_HEX_INT the ",02A" string will
 *   be concatenated to the InfoText string buffer.
 *
 *   It is guaranteed that no more than RESULT_TEXT_SIZE characters will
 *   be added to the InfoText string buffer.
 *
 *===============================================================================
 */
static bool AT_AddBasedValue_func(AT_CommandLine_t InfoText, char *Format,
                                  signed long Value, AT_IntegerBase_e Base)
{
    bool Success;
    AT_LineLength_t Index = 0; /* Current index in InfoText. */
    AT_LineLength_t BytesLeft; /* Remaining number of characters in the buffer. */
    /* We are going to concatenate the new information to the string. */
    Index = (AT_LineLength_t) strlen((char *) InfoText);
    /* Until the opposite is proved. */
    Success = false;

    /* Copy any non format determing text. */
    while ((Index < RESULT_TEXT_SIZE) && (*Format != '0') && (*Format
            != END_OF_STRING)) {
        InfoText[Index++] = *Format++;
    }

    /* Add sign if the number is below zero */
    if (Base == AT_DEC_INT) {
        if ((Value < 0) && (Index < RESULT_TEXT_SIZE)) {
            InfoText[Index++] = '-'; /* Add the "negative" sign */
            /*
             * Range of signed long = (-2^N) <-> (2^N - 1) where N is the number of bits used
             * Thus if Value == -2^N -> 0 - ( -2^N ) != (2^N) or (2^N-1) but instead
             * equal to the very same value ! i.e. -2^N
             * (ref.Harbison/Steele sec. 5.1.1 signed integer types)
             */

            /* coverity[dead_error_condition] - DEADCODE - LONG_MIN wrongly interpreted by the compiler */
            if (Value == LONG_MIN) { /* Makes the LONG_MIN value invalid */
                Value += 1; /* Achieved: signed long range [-(2^N-1), (2^N-1)] */
            }

            Value *= -1L; /* Negate (i.e. get the positive value) */
        }
    }

    if (Index < RESULT_TEXT_SIZE) {
        /* The previous check assures that the last parameter is within range. */
        BytesLeft = RESULT_TEXT_SIZE - Index;
        Format = AddValueAsAscii(&InfoText[Index], Format, Value, Base,
                                 &BytesLeft);

        if (Format != NULL) {
            Index += BytesLeft;

            /* Copy any non format determing text. */
            while ((Index < RESULT_TEXT_SIZE) && (*Format
                                                  != END_OF_STRING)) {
                InfoText[Index++] = *Format++;
            }

            if (Index < RESULT_TEXT_SIZE) {
                Success = true;
            }
        } /* endif */
    } /* endif */

    /* Null terminate the string. */
    InfoText[Index] = END_OF_STRING;

    return Success;
} /* End of AT_AddBasedValue_func */

/*
 *===============================================================================
 *
 *   Function: AT_AddRange
 *
 *   INPUT:   InfoText Text to which value shall be appended.
 *            Format   Format to use.
 *            Min      Min value to append.
 *            Max      Max value to append.
 *
 *   OUTPUT:  InfoText Text with appended characters.
 *
 *   Appends the interval Min-Max to InfoText AT-style.
 *
 *===============================================================================
 */

void AT_AddRange(AT_CommandLine_t InfoText, char *Format, unsigned char Min,
                 unsigned char Max)
{
    char *FormatBuffer = (char *) malloc(strlen(Format) + 1);
    strcpy(FormatBuffer, Format);
    (void) AT_AddRange_func(InfoText, FormatBuffer, Min, Max);
    free((void *) FormatBuffer);
}

/*
 *===============================================================================
 *
 *   Function: AT_AddBasedValue
 *
 *   INPUT:   InfoText Text to which value shall be appended.
 *            Format   Format to use.
 *            Value    Value to append.
 *            Base     Base to use.
 *
 *   OUTPUT:  InfoText Text with appended characters.
 *
 *   Appends the value Value to the string Text using Format and Base for
 *   formating.
 *
 *===============================================================================
 */

void AT_AddBasedValue(AT_CommandLine_t InfoText, char *Format,
                      signed long Value, AT_IntegerBase_e Base)
{
    char *FormatBuffer = (char *) malloc(strlen(Format) + 1);
    strcpy(FormatBuffer, Format);
    (void) AT_AddBasedValue_func(InfoText, FormatBuffer, Value, Base);
    free(FormatBuffer);
}

/*
 *===============================================================================
 *
 *   Function: AT_AddString
 *
 *   INPUT:   InfoText - Pointer to a string buffer.
 *                   Buffer   - Pointer to a string buffer
 *
 *   RETURNS: TRUE if the buffer was successfully added to the InfoText,
 *            otherwise FALSE is returned.
 *
 *  The function add escape character "\" before every occurrence of (") and (\)
 *  and copy the string buffer to InfoText .
 *  e.i (name"name")  will be copied to InfoText as ("name\"name\"")
 *  It is guaranteed that no more than RESULT_TEXT_SIZE characters will
 *  be added to the InfoText string buffer.
 *
 *===============================================================================
 */
bool AT_AddString(AT_CommandLine_t InfoText, char *buffer_p)
{
    uint16_t index;
    uint16_t i = 0;
    char *tmp_string_p = NULL;
    tmp_string_p = malloc((2 * strlen(buffer_p)) + 1); /* worst case when every char will be preceded with escape character */

    if (tmp_string_p == NULL) {
        goto AT_AddString_ERROR;
    }

    for (index = 0; index < strlen(buffer_p) + 1; index++) {
        if ((buffer_p[index] == '\\') || (buffer_p[index] == '\"')) {
            tmp_string_p[index + i] = '\\';
            i++;
        }

        tmp_string_p[index + i] = buffer_p[index];
    }

    if ((strlen((char *) InfoText) + strlen(tmp_string_p) + 2)
            > RESULT_TEXT_SIZE) {
        goto AT_AddString_ERROR;
    }

    strcat((char *) InfoText, "\"");
    strcat((char *) InfoText, tmp_string_p);
    strcat((char *) InfoText, "\"");

    if (tmp_string_p != NULL) {
        free(tmp_string_p);
    }

    return true;
AT_AddString_ERROR:

    if (tmp_string_p != NULL) {
        free(tmp_string_p);
    }

    return false;
}

/*
 *===============================================================================
 *
 *   Function: Parser_GetIntParam
 *
 *   INPUT:  parser_p - Pointer to the current parser state.
 *           Error    - Pointer to an error flag.
 *           Default  - Default value for the parameter, to use if no parameter is
 *                      found. If no default is supported set Default to NULL.
 *
 *   OUTPUT: Error    - The error flag is set to TRUE if no default value was given
 *                      and the parameter was left out, or if there was an error in
 *                      the parameter syntax.
 *           parser_p->
 *             Pos    - Points to the first character after the parameter and any
 *                      parameter separator.
 *
 *   RETURNS: The value of the extracted parameter.
 *
 *   Converts a string with decimal digits to an integer of the type IntParam_t.
 *
 *
 *===============================================================================
 */
IntParam_t Parser_GetIntParam(AT_ParserState_s *parser_p, bool *Error,
                              IntParam_t *Default)
{
    return Parser_GetGenericIntParam(parser_p, Error, Default, AT_PARAMETER_SEPARATOR, AT_COMMAND_SEPARATOR);
}

/*
 *===============================================================================
 *
 *   Function: Parser_GetDIntParam
 *
 *   INPUT:  parser_p - Pointer to the current parser state.
 *           Error    - Pointer to an error flag.
 *           Default  - Default value for the parameter, to use if no parameter is
 *                      found. If no default is supported set Default to NULL.
 *
 *   OUTPUT: Error    - The error flag is set to TRUE if no default value was given
 *                      and the parameter was left out, or if there was an error in
 *                      the parameter syntax.
 *           parser_p->
 *             Pos    - Points to the first character after the parameter and any
 *                      parameter separator.
 *
 *   RETURNS: The value of the extracted parameter.
 *
 *   Converts a string with decimal digits to an integer of the type IntParam_t,
 *   assuming parameters are separated with '*', rather than ',', and
 *   that commands are separated with '#', rather than ';'·
 *
 *
 *===============================================================================
 */
IntParam_t Parser_GetDIntParam(AT_ParserState_s *parser_p, bool *Error,
                               IntParam_t *Default)
{
    return Parser_GetGenericIntParam(parser_p, Error, Default, AT_D_PARAMETER_SEPARATOR, AT_D_COMMAND_SEPARATOR);
}

static IntParam_t Parser_GetGenericIntParam(AT_ParserState_s *parser_p, bool *Error_p,
        IntParam_t *Default_p, char AtParameterSeparator, char AtCommandSeparator)
{
    unsigned char i;
    unsigned char Current; /* Temp storage the currently parsed character. */
    unsigned long Value; /* Storage for the extracted parameter. */
    AT_LineLength_t MaxDigits; /* Number of characters remaining in command line. */
    Value = 0;
    /* Since the command line has no white spaces we can presume that the first */
    /* character is either a separator, a command line termination character */
    /* (or a syntax error). */
    Current = *parser_p->Pos;
    MaxDigits = (AT_LineLength_t)(parser_p->CommandLineLength -
                                  (parser_p->Pos - parser_p->CommandLine));

    /* If next character is a command separator or a command line termination */
    /* character the default value shall be used. But first we check that there */
    /* are any parameters to get. */
    if (parser_p->NoMoreParams || (Current == AtCommandSeparator) || (Current
            == AtParameterSeparator) || (Current == END_OF_STRING)) {
        if (Current == AtParameterSeparator) {
            parser_p->Pos++; /* Skip the parameter separator. */
        } else if (Current == AtCommandSeparator) {
            parser_p->Pos++; /* Skip the command separator. */
            parser_p->NoMoreParams = true; /* This command has no more parameters. */
        } else {
            parser_p->NoMoreParams = true; /* This command has no more parameters. */
        }

        if (NULL == Default_p) {
            *Error_p = true; /* The parameter list is empty and no default is given. */
            return 0;
        }

        return *Default_p;
    }

    /* Is the first character a non decimal digit? */
    if ((Current < '0') || (Current > '9')) {
        *Error_p = true; /* Command line syntax error. */
        return 0;
    }

    for (i = 0; i < MaxDigits; i++) {
        /* Add the current digit, return when finding the first non-decimal digit. */
        if ((Current >= '0') && (Current <= '9')) {
            Value = (Value * 10) + Current - '0';
        } else {
            break; /* Break the loop. */
        }

        parser_p->Pos++;
        Current = *parser_p->Pos;
    }; /* end for */

    if (*parser_p->Pos == AtParameterSeparator) {
        parser_p->Pos++; /* Skip the parameter separator. */
    } else if (*parser_p->Pos == AtCommandSeparator) {
        parser_p->Pos++; /* Skip the command separator. */
        parser_p->NoMoreParams = true; /* This command has no more parameters. */
    } else {
        parser_p->NoMoreParams = true; /* This command has no more parameters. */
    }

    if (Value > MAX_INT_PARAM) {
        *Error_p = true;
        return MAX_INT_PARAM;
    }

    /* We have successfully converted the specified number of characters! */
    return (IntParam_t) Value;
} /* End of Parser_GetIntParam */

/*
 *===============================================================================
 *
 *   Function: Parser_GetLongIntParam
 *
 *   INPUT:  parser_p - Pointer to the current parser state.
 *           Error    - Pointer to an error flag.
 *           Default  - Default value for the parameter, to use if no parameter is
 *                      found. If no default is supported set Default to NULL.
 *
 *   OUTPUT: Error    - The error flag is set to TRUE if no default value was given
 *                      and the parameter was left out, or if there was an error in
 *                      the parameter syntax.
 *           parser_p->
 *             Pos    - Points to the first character after the parameter and any
 *                      parameter separator.
 *
 *   RETURNS: The value of the extracted parameter.
 *
 *   Converts a string with decimal digits to an integer of the type unsigned long.
 *
 *===============================================================================
 */
unsigned long Parser_GetLongIntParam(AT_ParserState_s *parser_p,
                                     bool *Error_p, unsigned long *Default_p, unsigned long Limit)
{
    unsigned char i;
    unsigned char Current; /* Temp storage the currently parsed character. */
    unsigned long Value; /* Storage for the extracted parameter. */
    AT_LineLength_t MaxDigits; /* Number of characters remaining in command line. */
    Value = 0;
    /* Since the command line has no whitespaces we can presume that the first */
    /* character is either a separator, a command line termination character */
    /* (or a syntax error). */
    Current = *parser_p->Pos;
    MaxDigits = (AT_LineLength_t)(parser_p->CommandLineLength -
                                  (parser_p->Pos - parser_p->CommandLine));

    /* If next character is a command separator or a command line termination */
    /* character the default value shall be used. But first we check that there */
    /* are any parameters to get. */
    if (parser_p->NoMoreParams || (Current == AT_COMMAND_SEPARATOR) || (Current
            == AT_PARAMETER_SEPARATOR) || (Current == END_OF_STRING)) {
        if (Current == AT_PARAMETER_SEPARATOR) {
            parser_p->Pos++; /* Skip the parameter separator. */
        } else if (Current == AT_COMMAND_SEPARATOR) {
            parser_p->Pos++; /* Skip the command separator. */
            parser_p->NoMoreParams = true; /* This command has no more parameters. */
        } else {
            parser_p->NoMoreParams = true; /* This command has no more parameters. */
        }

        if (NULL == Default_p) {
            *Error_p = true; /* The parameter list is empty and no default is given. */
            return 0;
        }

        return *Default_p;
    }

    /* Is the first character a non decimal digit? */
    if ((Current < '0') || (Current > '9')) {
        *Error_p = true; /* Command line syntax error. */
        return 0;
    }

    for (i = 0; i < MaxDigits; i++) {
        /* Add the current digit, return when finding the first non-decimal digit. */
        if ((Current >= '0') && (Current <= '9')) {
            Value = (Value * 10) + Current - '0';
        } else {
            break; /* Break the loop. */
        }

        parser_p->Pos++;
        Current = *parser_p->Pos;
    }; /* end for */

    /* The character following a parameter must be either a separator or a */
    /* command line termination character. */
    if ((Current != AT_PARAMETER_SEPARATOR)
            && (Current != AT_COMMAND_SEPARATOR) && (Current != END_OF_STRING)) {
        *Error_p = true;
    } else if (*parser_p->Pos == AT_PARAMETER_SEPARATOR) {
        parser_p->Pos++; /* Skip the parameter separator. */
    } else if (*parser_p->Pos == AT_COMMAND_SEPARATOR) {
        parser_p->Pos++; /* Skip the command separator. */
        parser_p->NoMoreParams = true; /* This command has no more parameters. */
    } else {
        parser_p->NoMoreParams = true; /* This command has no more parameters. */
    }

    if (Value > Limit) {
        *Error_p = true;
        return Limit;
    }

    /* We have successfully converted the specified number of characters! */
    return Value;
} /* End of Parser_GetLongIntParam */

/*
 *===============================================================================
 *
 *   Function: Parser_GetSignedIntParam
 *
 *   INPUT:  parser_p - Pointer to the current parser state.
 *           Error    - Pointer to an error flag.
 *           Default  - Default value for the parameter, to use if no parameter is
 *                      found. If no default is supported set Default to NULL.
 *
 *   OUTPUT: Error    - The error flag is set to TRUE if no default value was given
 *                      and the parameter was left out, or if there was an error in
 *                      the parameter syntax.
 *           parser_p->
 *             Pos    - Points to the first character after the parameter and any
 *                      parameter separator.
 *
 *   RETURNS: The value of the extracted parameter.
 *
 *   Converts a string with decimal digits to a signed integer of the type SignedIntParam_t.
 *
 *===============================================================================
 */
static SignedIntParam_t Parser_GetSignedIntParam(AT_ParserState_s *parser_p,
        bool *Error_p, SignedIntParam_t *Default_p)
{
    unsigned char i;
    unsigned char Current; /* Temp storage the currently parsed character. */
    signed long Value; /* Storage for the extracted parameter. */
    unsigned long TempValue; /* Temporary Storage for the extracted parameter. */
    AT_LineLength_t MaxDigits; /* Number of characters remaining in command line. */
    bool Negative = false; /* Negative number */
    TempValue = 0;
    /* Since the command line has no whitespaces we can presume that the first */
    /* character is either a separator, a command line termination character */
    /* (or a syntax error). */
    Current = *parser_p->Pos;
    MaxDigits = (AT_LineLength_t)(parser_p->CommandLineLength -
                                  (parser_p->Pos - parser_p->CommandLine));

    /* If next character is a command separator or a command line termination */
    /* character the default value shall be used. But first we check that there */
    /* are any parameters to get. */
    if (parser_p->NoMoreParams || (Current == AT_COMMAND_SEPARATOR) || (Current
            == AT_PARAMETER_SEPARATOR) || (Current == END_OF_STRING)) {
        if (Current == AT_PARAMETER_SEPARATOR) {
            parser_p->Pos++; /* Skip the parameter separator. */
        } else if (Current == AT_COMMAND_SEPARATOR) {
            parser_p->Pos++; /* Skip the command separator. */
            parser_p->NoMoreParams = true; /* This command has no more parameters. */
        } else {
            parser_p->NoMoreParams = true; /* This command has no more parameters. */
        }

        if (NULL == Default_p) {
            *Error_p = true; /* The parameter list is empty and no default is given. */
            return 0;
        }

        return *Default_p;
    }

    /* Is the first character a minus? */
    if (Current == '-') {
        Negative = true;
        parser_p->Pos++;
        Current = *parser_p->Pos;
        MaxDigits--;
    }

    /* Is the first character a non decimal digit? */
    if ((Current < '0') || (Current > '9')) {
        *Error_p = true; /* Command line syntax error. */
        return 0;
    }

    for (i = 0; i < MaxDigits; i++) {
        /* Add the current digit, return when finding the first non-decimal digit. */
        if ((Current >= '0') && (Current <= '9')) {
            TempValue = (TempValue * 10) + Current - '0';
        } else {
            break; /* Break the loop. */
        }

        parser_p->Pos++;
        Current = *parser_p->Pos;
    }; /* end for */

    /* The character following a parameter must be either a separator or a */
    /* command line termination character. */
    if ((Current != AT_PARAMETER_SEPARATOR)
            && (Current != AT_COMMAND_SEPARATOR) && (Current != END_OF_STRING)) {
        *Error_p = true;
    } else if (*parser_p->Pos == AT_PARAMETER_SEPARATOR) {
        parser_p->Pos++; /* Skip the parameter separator. */
    } else if (*parser_p->Pos == AT_COMMAND_SEPARATOR) {
        parser_p->Pos++; /* Skip the command separator. */
        parser_p->NoMoreParams = true; /* This command has no more parameters. */
    } else {
        parser_p->NoMoreParams = true; /* This command has no more parameters. */
    }

    if (TempValue > MAX_SIGNED_INT_PARAM) {
        *Error_p = true;
        Value = MAX_SIGNED_INT_PARAM;
    } else {
        Value = (signed) TempValue;
    }

    if (Negative) {
        Value = (-1) * Value;
    }

    /* We have successfully converted the specified number of characters! */
    return (SignedIntParam_t) Value;
} /* End of Parser_GetSignedIntParam */

/*
 *===============================================================================
 *
 *   Function: Parser_GetHexParam
 *
 *   INPUT:  parser_p - Pointer to the current parser state.
 *           Error    - Pointer to an error flag.
 *           Default  - Default value for the parameter, to use if no parameter is
 *                      found. If no default is supported set Default to NULL.
 *
 *   OUTPUT: Error    - The error flag is set to TRUE if no default value was given
 *                      and the parameter was left out, or if there was an error in
 *                      the parameter syntax.
 *           parser_p->
 *             Pos    - Points to the first character after the parameter and any
 *                      parameter separator.
 *
 *   RETURNS: The value of the extracted parameter.
 *
 *   Converts a string with hexadecimal digits to an integer of the type IntParam_t.
 *
 *===============================================================================
 */
static IntParam_t Parser_GetHexParam(AT_ParserState_s *parser_p, bool *Error_p,
                                     IntParam_t *Default_p)
{
    unsigned char i;
    unsigned char Current; /* Temp storage the currently parsed character. */
    unsigned long Value; /* Storage for the extracted parameter. */
    AT_LineLength_t MaxDigits; /* Number of characters remaining in command line.*/
    Value = 0;
    /* Since the command line has no whitespaces we can presume that the first */
    /* character is either a separator, a command line termination character */
    /* (or a syntax error). */
    Current = *parser_p->Pos;
    MaxDigits = (AT_LineLength_t)(parser_p->CommandLineLength -
                                  (parser_p->Pos - parser_p->CommandLine));

    /* If next character is a command separator or a command line termination */
    /* character is either a separator, a command line termination character */
    /* (or a syntax error). */
    if (parser_p->NoMoreParams || (Current == AT_COMMAND_SEPARATOR) || (Current
            == AT_PARAMETER_SEPARATOR) || (Current == END_OF_STRING)) {
        if (Current == AT_PARAMETER_SEPARATOR) {
            parser_p->Pos++; /* Skip the parameter separator. */
        } else if (Current == AT_COMMAND_SEPARATOR) {
            parser_p->Pos++; /* Skip the command separator. */
            parser_p->NoMoreParams = true; /* This command has no more parameters. */
        } else {
            parser_p->NoMoreParams = true; /* This command has no more parameters. */
        }

        if (NULL == Default_p) {
            *Error_p = true; /* The parameter list is empty and no default is given. */
            return 0;
        }

        return *Default_p;
    }

    /* Is the first character a non hexadecimal digit? */
    if ((Current < '0') || ((Current > '9') && (Current < 'A')) || (Current
            > 'F')) {
        *Error_p = true; /* Command line syntax error. */
        return 0;
    }

    for (i = 0; i < MaxDigits; i++) {
        /* Add the current digit, return when finding the first non-decimal digit.*/
        if ((Current >= '0') && (Current <= '9')) {
            Value = (Value << 4) + Current - '0';
        } else if ((Current >= 'A') && (Current <= 'F')) {
            Value = (Value << 4) + 10 + Current - 'A';
        } else {
            break; /* Break the loop. */
        }

        parser_p->Pos++;
        Current = *parser_p->Pos;
    }; /* end for */

    /* The character following a parameter must be either a separator or a */
    /* command line termination character. */
    if ((Current != AT_PARAMETER_SEPARATOR)
            && (Current != AT_COMMAND_SEPARATOR) && (Current != END_OF_STRING)) {
        *Error_p = true;
    } else if (*parser_p->Pos == AT_PARAMETER_SEPARATOR) {
        parser_p->Pos++; /* Skip the parameter separator. */
    } else if (*parser_p->Pos == AT_COMMAND_SEPARATOR) {
        parser_p->Pos++; /* Skip the command separator. */
        parser_p->NoMoreParams = true; /* This command has no more parameters. */
    } else {
        parser_p->NoMoreParams = true; /* This command has no more parameters. */
    }

    if (Value > MAX_INT_PARAM) {
        *Error_p = true;
        return MAX_INT_PARAM;
    }

    /* We have successfully converted the specified number of characters! */
    return (IntParam_t) Value;
} /* End of Parser_GetHexParam */

static StrParam_t Parser_GetIntParamAsStr(AT_ParserState_s *parser_p,
        bool *Error_p, StrParam_t Default_p)
{
    return Parser_GetDigitParamAsStr(parser_p, Error_p, INTEGER, Default_p);
}

StrParam_t Parser_GetHexParamAsStr(AT_ParserState_s *parser_p,
                                   bool *Error_p, StrParam_t Default_p)
{
    return Parser_GetDigitParamAsStr(parser_p, Error_p, HEXADECIMAL, Default_p);
}
StrParam_t Parser_GetBinParamAsStr(AT_ParserState_s *parser_p,
                                   bool *Error_p, StrParam_t Default_p)
{
    return Parser_GetDigitParamAsStr(parser_p, Error_p, BINARY, Default_p);
}

/*
 *===============================================================================
 *
 *   Function: Parser_GetDigitParamAsStr
 *
 *   INPUT:  parser_p - Pointer to the current parser state.
 *           Error    - Pointer to an error flag.
 *           Default  - Default value for the parameter, to use if no parameter is
 *                      found. If no default is supported set Default to NULL.
 *
 *   OUTPUT: Error    - The error flag is set to true if no default value was given
 *                      and the parameter was left out, or if there was an error in
 *                      the parameter syntax.
 *           parser_p->
 *             Pos    - Points to the first character after the parameter and any
 *                      parameter separator.
 *
 *   RETURNS: A pointer to the extracted parameter. The pointer is only valid
 *            to use as long as parser_p->CommandLine is still allocated.
 *
 *   This function returns an integer parameter as a string.
 *
 *   Replaces the separator after the integer parameter with an END_OF_STRING
 *   character and returns a pointer to the first character of the integer
 *   parameter.
 *
 *===============================================================================
 */
static StrParam_t Parser_GetDigitParamAsStr(AT_ParserState_s *parser_p,
        bool *Error_p, input_digit_t input_digit, StrParam_t Default_p)
{
    AT_CommandLine_t StartOfParam; /* Pointer to the first character of the param.*/
#define PARSER_VALID_INT_PARAMETER_DIGITS "0123456789"
#define PARSER_VALID_HEX_PARAMETER_DIGITS "0123456789ABCDEF"
#define PARSER_VALID_BIN_PARAMETER_DIGITS "01"
    /* Since the command line has no whitespaces we can presume that the first */
    /* character is either a separator, a command line termination character */
    /* or a integer parameter digit (or a syntax error). */

    /* If next character is a command separator or a command line termination */
    /* character the default value shall be used. */
    if (parser_p->NoMoreParams || (*parser_p->Pos == AT_COMMAND_SEPARATOR)
            || (*parser_p->Pos == AT_PARAMETER_SEPARATOR) || (*parser_p->Pos
                    == END_OF_STRING)) {
        if (*parser_p->Pos == AT_PARAMETER_SEPARATOR) {
            parser_p->Pos++; /* Skip the parameter separator. */
        } else if (*parser_p->Pos == AT_COMMAND_SEPARATOR) {
            parser_p->Pos++; /* Skip the command separator. */
            parser_p->NoMoreParams = true; /* This command has no more parameters. */
        } else {
            parser_p->NoMoreParams = true; /* This command has no more parameters. */
        }

        if (NULL == Default_p) {
            *Error_p = true; /* The parameter list is empty and no default is given. */
        }

        return Default_p;
    }

    StartOfParam = parser_p->Pos;

    /* Find the first character that is not a valid int parameter digit. */
    if (INTEGER == input_digit) {
        parser_p->Pos += strspn((char *) StartOfParam,
                                PARSER_VALID_INT_PARAMETER_DIGITS);
    } else if (HEXADECIMAL == input_digit) {
        parser_p->Pos += strspn((char *) StartOfParam,
                                PARSER_VALID_HEX_PARAMETER_DIGITS);
    } else if (BINARY ==   input_digit) {
        parser_p->Pos += strspn((char *) StartOfParam,
                                PARSER_VALID_BIN_PARAMETER_DIGITS);
    } else {
        if (NULL == Default_p) {
            *Error_p = true; /* The parameter list is empty and no default is given. */
        }

        return Default_p;
    }

    /* The character following a parameter must be either a separator or a */
    /* command line termination character. */
    if ((parser_p->Pos == StartOfParam)
            || ((*parser_p->Pos != AT_COMMAND_SEPARATOR) && (*parser_p->Pos
                    != AT_PARAMETER_SEPARATOR) && (*parser_p->Pos
                            != END_OF_STRING))) {
        *Error_p = true; /* No string parameter was found. */
    } else if (*parser_p->Pos == AT_PARAMETER_SEPARATOR) {
        /* Replace the charcterer after the parameter with an end of string */
        /* character so that a null terminated string is returned. */
        *parser_p->Pos = END_OF_STRING;
        parser_p->Pos++; /* Skip the parameter separator. */
    } else if (*parser_p->Pos == AT_COMMAND_SEPARATOR) {
        *parser_p->Pos = END_OF_STRING;
        parser_p->Pos++; /* Skip the parameter separator. */
        parser_p->NoMoreParams = true; /* This command has no more parameters. */
    } else {
        parser_p->NoMoreParams = true; /* This command has no more parameters. */
    }

    return (StrParam_t) StartOfParam;
} /* End of Parser_GetIntParamAsStr */

/*
 *===============================================================================
 *
 *   Function: Parser_IsParsingCommandLine
 *
 *   INPUT:   parser_p - Pointer to the current parser state.
 *
 *   OUTPUT:  None
 *
 *   RETURNS: true if a command line is beeing parsed, FALSE if the parser is
 *            ready for a new command line.
 *
 *===============================================================================
 */
static bool Parser_IsParsingCommandLine(AT_ParserState_s *parser_p)
{
    /* Pos will always point somewhere in the command line while it is executed.*/
    return (parser_p->Pos != NULL);
}

/*
 *===============================================================================
 *
 *   Function: Parser_IsAbortable
 *
 *   INPUT:    parser_p - Pointer to the current parser state.
 *
 *   OUTPUT:   None
 *
 *   RETURNS:  true if the current command is abortable, otherwise FALSE.
 *
 *===============================================================================
 */
static bool Parser_IsAbortable(AT_ParserState_s *parser_p)
{
    unsigned char i;

    if (parser_p->EnabledCommandClasses & AT_CLASS_ABORTABLE) {
        for (i = 0; i < AT_COMMAND_TABLE_LENGTH; i++) {
            if ((AT_CommandTable[i].Command == parser_p->Command)
                    && (AT_CommandTable[i].Class & AT_CLASS_ABORTABLE)) {
                /* Return true if abort is enabled for the parser
                 * and the current command is abortable. */
                return true;
            }
        }
    }

    return false;
}

/*
 *=====================================================================
 *
 *  Function: AT_RemoveComma
 *
 *  Input:  ParamString     - buffer with command response parameters
 *
 *  Output:
 *
 *  The function removes from end of response buffer empty commas
 *
 * ==================================================================
 */
static void AT_RemoveComma(AT_CommandLine_t ParamString)
{
    uint16_t BufferLen;

    if (ParamString == NULL) {
        return;
    }

    BufferLen = strlen((char *) ParamString) - 1;

    while (ParamString[BufferLen] == ',' && BufferLen > 0) {
        ParamString[BufferLen] = '\0';
        BufferLen--;
    }
}

/*
 *===============================================================================
 *
 *   Function: Parser_SendSupportedATCommands
 *
 *   INPUT:   parser_p - Pointer to the current parser state.
 *
 *   OUTPUT:  None
 *
 *   RETURNS: A status code for the execution of the command is returned:
 *            AT_OK        - The command was successfully handled.
 *            AT_ERROR     - There where a general error.
 *            AT_CME_ERROR - There where a GSM specific error, the error code is
 *                           returned in ASCII format in InfoText.
 *            AT_PENDING   - The command is not ready but returns while
 *                           waiting for an asynchronous response.
 *
 *   This function lists all supported at commands.
 *
 *===============================================================================
 */
static AT_Command_e Parser_SendSupportedATCommands(AT_ParserState_s *parser_p)
{
    uint16_t i;
    bool VerboseResponse = parser_p->VerboseResponse;

    if (*(parser_p->Pos) != '\0') {
        /* The command was NOT AT* !!! */
        return AT_ERROR;
    }

    parser_p->VerboseResponse = false;

    for (i = 0; i < AT_COMMAND_TABLE_LENGTH; i++) {
        if (!(AT_CommandTable[i].Class & AT_CLASS_DEBUG)
                && (AT_CommandTable[i].Mode != AT_MODE_NONE)) {
            Parser_SendResponse(parser_p, AT_CommandTable[i].Command, /*AT_BUILD_COMMAND_INFO,*/
                                NULL);
        }
    }

    parser_p->VerboseResponse = VerboseResponse;
    return AT_OK;
}

/*
 *===============================================================================
 *
 *   Function: Parser_SetCMEE_ErrorType
 *
 *   INPUT:   parser_p - Pointer to the current parser state.
 *            CMEE_ErrorType - New value for the error type.
 *
 *   Access function for the CMEE_ErrorType entry in AT_ParserState_s.
 *
 *===============================================================================
 */
void Parser_SetCMEE_ErrorType(AT_ParserState_s *parser_p,
                              exe_cmee_error_t CMEE_ErrorType)
{
    parser_p->CMEE_ErrorType = CMEE_ErrorType;
}

/*
 *===============================================================================
 *
 *   Function: Parser_SetCMS_ErrorType
 *
 *   INPUT:   parser_p - Pointer to the current parser state.
 *            CMS_ErrorType - New value for the error type.
 *
 *   Access function for the CMS_ErrorType entry in AT_ParserState_s.
 *
 *===============================================================================
 */
void Parser_SetCMS_ErrorType(AT_ParserState_s *parser_p,
                             unsigned int CMS_ErrorType)
{
    parser_p->CMS_ErrorType = CMS_ErrorType;
}


/*
 *===============================================================================
 *
 *   Function: Parser_ResetCommandSettings
 *
 *   INPUT:  parser_p - Pointer to the current parser state.
 *
 *   OUTPUT: parser_p - All state varibles modifiable by AT commands are reset.
 *
 *   Sets default values for all parser state variables.
 *
 *===============================================================================
 */
void Parser_ResetCommandSettings(AT_ParserState_s *parser_p)
{
    parser_p->VerboseResponse = true; /* Use verbose response codes. */
    parser_p->SuppressResponse = false; /* Do not suppress response codes. */
    parser_p->CMEE = false; /* Do not return GSM specific error codes. */
    parser_p->ECAM = 0; /* Extended call monitoring. */
    parser_p->ECME = 0; /* CME unsolicited gprs error cause code reporting. */
    parser_p->ERFSTATE = 1;/* RF state unsolicited reporting, enable and disable */
    parser_p->ESIMSR = 0; /* Ericsson SIM State Reporting */
    parser_p->ESIMRF = 0; /* Ericsson SIM Refresh State Reporting */
    parser_p->CGEV = 0;  /* PS data information */
    parser_p->CIEV_signal = CIEV_UNKNOWN_BARS; /* Last reported signal quality value */
    parser_p->CMER_mode = CMER_MODE_BUFFER; /* Presentation or hiding of Mobile Equipment Event Reporting. Values 0 and 3 are supported.  */
    parser_p->CMER_ind = CMER_IND_DEACTIVATED; /* Presentation or hiding of the +CIEV unsolicited event. 1 is enabled. */
    parser_p->CSSI = 0; /* Supplementary Service Intermediate */
    parser_p->CCWA_n = 0;       /* AT+CCWA - Call Waiting Notification, N. Set to 0 on default */
    parser_p->CSSU = 0; /* Supplementary Service Unsolicited */
    parser_p->CR = 0; /* Service reporting control +CR */
    parser_p->CRC  = 0; /* Cellular Result Codes */
    parser_p->CLIP = 0; /* Calling Line Identification Presentation */
    parser_p->CNAP = 0; /* Calling Name Identification Presentation */
    parser_p->CUSD = 0; /* Unstructured Supplementary Service Unsolicited */
    parser_p->abort = false;
    parser_p->request_handle = 0;  /* request handle (used in abort scenarios) */
    parser_p->Echo = true; /* Echo characters */
    parser_p->S0 = 0;       /* According to M570 */
    parser_p->S3 = 13;      /* According to 3GPP. 13 = CR */
    parser_p->S4 = 10;      /* According to 3GPP. 10 = LF */
    parser_p->S5 = 8;       /* According to 3GPP. 8 = BS */
    parser_p->S6 = 2;       /* Pause Before Blind Dialling */
    parser_p->S7 = 50;      /* Connection Completion Timeout */
    parser_p->S8 = 2;       /* Comma Dial Modifier Time (dummy) */
    parser_p->S10 = 2;      /* Disconnect Delay Control */
    parser_p->X = 4;        /* Call progress monitoring control */
    parser_p->CGREG = NET_REG_IND_DISABLED; /* Disable network registration unsolicited result code */
    parser_p->CREG = NET_REG_IND_DISABLED;  /* Disable network registration unsolicited result code */
    parser_p->EREG = NET_REG_IND_DISABLED;  /* Disable network registration unsolicited result code */
    parser_p->ETZR = ETZR_DISABLED;
    parser_p->EPEE = 0;
    parser_p->cscs_charset = CHARACTER_SET_DEFAULT; /* Setting default character set value.*/
    parser_p->CUUS1I = 0;   /* Disable CUUS1 intermediate events */
    parser_p->CUUS1U = 0;   /* Disable CUUS1 unsolicited events */
    parser_p->CUUS1_setup_msg.length = 0;
    parser_p->CUUS1_setup_msg.dcs = 0;
    free(parser_p->CUUS1_setup_msg.data);
    parser_p->CUUS1_setup_msg.data = NULL;
    parser_p->stkc_pc_enable = EXE_STKC_PC_DISABLE;
    parser_p->facility_code = ATC_FAC_ERROR;
    parser_p->clck_mode = 0;
    parser_p->clck_class = 0;
    parser_p->cnmi_message_indication.mode = 2;  /* Directly forward messages to TE */
    parser_p->cnmi_message_indication.mt = 0;  /* No SMS-DELIVER indications */
    parser_p->cnmi_message_indication.bm = 0;  /* No CBM indications */
    parser_p->cnmi_message_indication.ds = 0;  /* No SMS-STATUS-REPORTs */
    parser_p->cnmi_message_indication.bfr = 0;  /* Do not clear the buffer of UR codes defined within CNMI command*/
    parser_p->cpms_msg_storage.mem1 = MSG_STORAGE_SM; /* Memory from which messages are read and deleted */
    parser_p->cpms_msg_storage.mem2 = MSG_STORAGE_SM; /* Memory to which messages are written and sent */
    parser_p->connecting_voice_call = false;  /* MO voice call not initiated */
    parser_p->empage_state = EXE_EMPAGE_STATE_IDLE;
#ifdef EXE_USE_ATC_CUSTOMER_EXTENSIONS
    Atc_Extended_Parser_ResetCommandSettings(parser_p);
#endif
}

/*
 *===============================================================================
 *
 *   Function: Parser_SetDefaultState
 *
 *   INPUT:  parser_p - Pointer to the current parser state.
 *
 *   OUTPUT: parser_p - All state variables are set.
 *
 *   Sets default values for all parser state variables.
 *
 *===============================================================================
 */
void Parser_SetDefaultState(AT_ParserState_s *parser_p)
{
    /* TODO: Clean up this function */
    parser_p->IsPending = false; /* No AT commands is pending. */
    parser_p->ContextID = 255; /* As default the parser is assigned no context id. */
    parser_p->Command = AT_NO_COMMAND; /* No command is yet parsed. */
    parser_p->CommandTableIndex = -1; /* No command is yet parsed. */
    parser_p->Mode = AT_MODE_UNDEFINED; /* No command mode is yet parsed. */
    parser_p->EnabledCommandClasses = AT_CLASS_ALL; /* Enabled all command classes. */
    parser_p->Pos = NULL; /* Pos is not defined. */
    parser_p->CommandLine = NULL; /* No Command Line is allocated. */
    parser_p->CommandLineLength = 0; /* No Command Line is allocated. */
    parser_p->CMEE_ErrorType        = CMEE_OK;           /* No CME error has yet ocurred. */
    /* TODO:    parser_p->CMS_ErrorType         = CMS_OK; */           /* No CMS error has yet ocurred. */
    parser_p->connection_type = ATC_CONNECTION_TYPE_UNUSED;
#ifdef AT_PLUS_CPIN_ENABLED
    parser_p->get_sim_state = false;
#endif
    parser_p->pending_queue_p = NULL;
    Parser_ResetCommandSettings(parser_p);
} /* Parser_SetDefaultState */

/*
 *=====================================================================
 *
 * Function: ParserStateTable_ResetEntry
 *
 * INPUT:  Entry - Requested Entry in ParserStateTable
 * OUTPUT: None
 *
 * This function marks an entry in the ParserStateTable free and resets
 * all elements to their default values.
 *
 *======================================================================
 */

void ParserStateTable_ResetEntry(unsigned char Entry)
{
    if (Entry < MAX_NO_OF_OPEN_CHANNELS) {
        AT_ParserState_s *parser_p = &ParserStateTable[Entry];
        /* SS_ClearPending(); */
        /* SBMISC_AbortPending(parser_p); */
        /* SB_SMS_ClearPending(parser_p); */
        /* TODO import Parser_SetDefaultState(parser_p); from R13 PAS */
        Parser_SetDefaultState(parser_p);
    }
} /* End of ParserStateTable_ResetEntry */

/*
 *=====================================================================
 *
 * Function: ParserStateTable_GetFreeEntry
 *
 * INPUT:  NONE
 * OUTPUT: Returned  - first free entry number in ParserState Table
 *
 * This function returns the the first free entry number in the ParserState Table.
 *
 *======================================================================
 */

unsigned char ParserStateTable_GetFreeEntry(void)
{
    unsigned char Entry;

    for (Entry = 0; Entry < MAX_NO_OF_OPEN_CHANNELS; Entry++) {
        if (ParserStateTable[Entry].ContextID == 255) {
            return Entry;
        }
    }

    return PARSER_STATE_TABLE_ENTRY_NOT_FOUND;
} /* End of ParserStateTable_GetFreeEntry */

/*
 *=====================================================================
 *
 * Function: ParserStateTable_UpdateEntry
 *
 * INPUT:  Entry          - Entry in ParserStateTable to be updated
 *                 ContextID      -    Value to set for ContextId
 *
 * OUTPUT: NONE
 *
 * This function updates the Logical address and modem signals for a ParserStateTable entry.
 *
 *======================================================================
 */
void ParserStateTable_UpdateEntry(unsigned char Entry, ContextId_t ContextID)
{
    ParserStateTable[Entry].ContextID = ContextID;
} /* End of ParserStateTable_UpdateEntry */

/*
 *=====================================================================
 *
 * Function: ParserStateTable_GetParserState_ByEntry
 *
 * INPUT:  Entry          - Entry in ParserStateTable
 * OUTPUT: Returned       - ParserState for entry in ParserStateTable
 *
 * This function returns the state of the AT protocol parser for a given
 * ParserStateTable entry.
 *
 *======================================================================
 */

AT_ParserState_s *ParserStateTable_GetParserState_ByEntry(unsigned char Entry)
{
    if (Entry < MAX_NO_OF_OPEN_CHANNELS) {
        return &ParserStateTable[Entry];
    } else {
        return NULL;
    }
}

/*
 *=====================================================================
 *
 * Function: ParserStateTable_GetContextId
 *
 * INPUT:  Entry     - Entry in ParserStateTable
 * OUTPUT: Returned  - Context Id for entry in ParserStateTable
 *
 * This function returns the logical address for a given entry in ParserStateTable.
 *
 *======================================================================
 */

static ContextId_t ParserStateTable_GetContextId(unsigned char Entry)
{
    if (Entry < MAX_NO_OF_OPEN_CHANNELS) {
        return ParserStateTable[Entry].ContextID;
    } else {
        return 0;
    }
} /* End of ParserStateTable_GetContextId */

/*
 *===============================================================================
 *
 *   Function: Parser_GetNextCommand
 *
 *   INPUT:   parser_p - Pointer to the current parser state.
 *
 *   OUTPUT:  parser_p->
 *              Command - Enumeration of the command found.
 *                        AT_UNKNOWN_COMMAND - if the command was not supported.
 *                        AT_NO_COMMAND - if no command was found.
 *                        AT_ERROR - if the command mode was not supported.
 *              Mode    - Enumeration of the mode for the command.
 *                        AT_MODE_UNDEFINED - if no command was found.
 *              Pos     - If a command was found Pos points to the first
 *                        parameter for the command. Otherwise Pos points at the
 *                        position where the function was terminated.
 *              NoMoreParams - Set to FALSE if a command was found.
 *
 *   RETURNS: true if a command was found, otherwise FALSE.
 *
 *   This function presumes that the command line is pre-formatted so that all
 *   characters are upper case and that there are no redundant white spaces.
 *   The function tries to identify the AT command at the current position in the
 *   command line. If successful the command mode is also extracted and verified.
 *
 *   If the identified command is of a class that is not supported error is
 *   returned.
 *
 *===============================================================================
 */
static bool Parser_GetNextCommand(AT_ParserState_s *parser_p,
                                  AT_Command_e *ResultCode)
{
    static unsigned int FirstStep = 0;
    int Index = 0;
    int last_matching_index;
    unsigned char i;
    int Result; /* Result of the last string comparison. */
    size_t str_len = 0;
    char *temp_command_p = NULL;

    /* Initiate the return parameters with their appropriate default values. */
    parser_p->Command = AT_NO_COMMAND;
    parser_p->CommandTableIndex = -1;
    parser_p->Mode = AT_MODE_UNDEFINED;
    *ResultCode = AT_OK;

    /* If there is an command separator, let's skip it! */
    if (*(parser_p->Pos) == AT_COMMAND_SEPARATOR) {
        parser_p->Pos++;
    }

    /* Return if we have reached the line termination character. */
    if (*(parser_p->Pos) == END_OF_STRING) {
        return false;
    }

    while (Index < (AT_COMMAND_TABLE_LENGTH - 1)) {

        if (Index >= 0 && Index < AT_COMMAND_TABLE_LENGTH) {
            str_len = strlen(AT_CommandTable[Index].String);
            temp_command_p = (char *)malloc(str_len + 1);

            if (!temp_command_p) {
                ATC_LOG_E("%s: error allocating memory!",
                          __FUNCTION__);
                return false;
            }

            strncpy(temp_command_p, (char *)parser_p->Pos, str_len);

            for (i = 0; i < str_len; i++) {
                temp_command_p[i] = to_upper(temp_command_p[i]);
            }

            Result = memcmp(AT_CommandTable[Index].String, temp_command_p, str_len);
            free(temp_command_p);
        } else {
            /* This should never happen! Code added to make Coverity happy.*/
            if (Index < 0) {
                Result = -1; /* The index was to low. */
            } else {
                Result = 1; /* The index was to high. */
            }
        }

        if (Result != 0) { /* The index was to low. */
            Index++;
        } else { /* Result == 0; we have found a matching command! */
            last_matching_index = Index;

            /* We have found one match but there could be a better if more characters match it is better
             * so we save current match length in str_len
             */
            while (Index < (AT_COMMAND_TABLE_LENGTH - 1)) {
                Index++;
                size_t new_str_len = strlen(AT_CommandTable[Index].String);
                temp_command_p = (char *)malloc(new_str_len + 1);

                if (!temp_command_p) {
                    ATC_LOG_E("%s: error allocating memory!",
                              __FUNCTION__);
                    return false;
                }

                strncpy(temp_command_p, (char *)parser_p->Pos, new_str_len);

                for (i = 0; i < new_str_len; i++) {
                    temp_command_p[i] = to_upper(temp_command_p[i]);
                }

                if (0 == memcmp(AT_CommandTable[Index].String, temp_command_p, new_str_len) && new_str_len > str_len) {
                    /* This command matches with more characters, then the initial one */
                    last_matching_index = Index;
                    str_len = new_str_len;
                }

                free(temp_command_p);
            }

            Index = last_matching_index;

            /*AT_STAR may not be combined with other commands.
             * Allowed variants of AT* are AT* and AT*=?.
             * */
            if (AT_STAR == AT_CommandTable[Index].Command) {
                size_t pos_len = strlen((char *) parser_p->Pos);
                const size_t at_star_test_len = 3; /* The length of "*=?" */

                if (0 != memcmp(AT_CommandTable[Index].String, parser_p->Pos, pos_len)) {
                    /* Return OK if it is the test command */
                    if ((pos_len == at_star_test_len) && (0 == memcmp(parser_p->Pos, "*=?", pos_len))) {
                        parser_p->Command = AT_OK;
                        *ResultCode = AT_OK;
                    } else {
                        /* Send other star commands to at-service for validation */
                        parser_p->Command = AT_UNKNOWN_COMMAND;
                        *ResultCode = AT_ERROR;
                    }

                    return false;
                }

            }

            parser_p->Pos += strlen(AT_CommandTable[Index].String);

            for (i = 0; i < AT_MODE_TABLE_LENGTH; i++) {
                if (memcmp(AT_ModeTable[i].String, parser_p->Pos, strlen(
                               AT_ModeTable[i].String)) == 0) {
                    break;
                }
            }

            if (i == AT_MODE_TABLE_LENGTH) {
                parser_p->Mode = AT_MODE_DO;
            } else {
                /* Store the mode and move past the mode token. */
                parser_p->Mode = AT_ModeTable[i].Mode;
                parser_p->Pos += strlen(AT_ModeTable[i].String);
            }

            /* Check that the mode is supported for this command. */
            if (parser_p->Mode & AT_CommandTable[Index].Mode) {
                /* TODO: R13 PAS checked Command_Allowed() here (should not allow all commands before PIN!!!) */
                parser_p->Command = AT_CommandTable[Index].Command;
                parser_p->CommandTableIndex = Index;
                parser_p->HandlerFunction = AT_CommandTable[Index].HandlerFunction;

                /* Set NoMoreParams so that it can be trusted on from start */
                if ((AT_COMMAND_SEPARATOR == *parser_p->Pos) || (END_OF_STRING == *parser_p->Pos)) {
                    parser_p->NoMoreParams = true;
                } else {
                    parser_p->NoMoreParams = false;
                }

                return true;
            }

            parser_p->Command = AT_ERROR;
            *ResultCode = AT_ERROR;
            return false;
        } /*else result =o */
    }

    parser_p->Command = AT_UNKNOWN_COMMAND;
    *ResultCode = AT_ERROR;
    return false;
} /* End of Parser_GetNextCommand */

/*
 *===============================================================================
 *
 *   Function: Parser_Clear
 *
 *   INPUT:  parser_p - Pointer to the current parser state.
 *
 *   OUTPUT: parser_p->
 *             CommandLine - NULL.
 *             Pos         - NULL.
 *
 *   Deallocates the command line buffer (parser_p->CommandLine).
 *
 *===============================================================================
 */
static void Parser_Clear(AT_ParserState_s *parser_p)
{
#ifdef STATIC_COMMAND_LINE_BUFFER
    parser_p->CommandLine[0] = END_OF_STRING;
#else
    /* Free the command line buffer. */
    parser_p->CommandLine = AT_FreeCmdBuffer(parser_p->ContextID,
                            parser_p->CommandLine);
#endif
    parser_p->Pos = NULL;
} /* End of Parser_Clear */

/*
 *===============================================================================
 *
 *   Function: Parser_ParseCommandLine
 *
 *   INPUT:   parser_p - Pointer to the current parser state.
 *            Message  - Pointer to a message for any pending command.
 *
 *   OUTPUT:  parser_p->
 *              When the command line is terminated:
 *              IsPending   - Is cleared.
 *              CommandLine - The buffer is deleted.
 *              Pos         - The pointer is set to NULL.
 *
 *   RETURNS: FALSE if the command line buffer is not allocated, otherwise TRUE.
 *
 *   This function parses a command line by extracting the commands one by one
 *   and calling the appropriate command handlers. If any command is asynchronous
 *   the function will return immediately.
 *
 *   To finish the execution of an asynchronous command this function must be
 *   called by the instance that receives the result of the asynchronous command.
 *   That instance shall call this function with the current parser and a pointer
 *   to a buffer with information about the result (Message). The handler for the
 *   pending command will be called with the Message pointer. Thereafter the
 *   rest of the commands are parsed.
 *
 *===============================================================================
 */

bool Parser_ParseCommandLine(AT_ParserState_s *parser_p,
                             AT_ResponseMessage_t Message)
{
    AT_CommandLine_t ResultText;          /* Buffer for any result text. */
    bool ContinueParsing;                 /* Flags termination of the command line. */
    AT_Command_e ResultCode = AT_OK;      /* Result code from the last executed command. */
    atc_context_t *at_context;            /* */
    int nbr;                              /* The number of bytes sent to AtService */
    bool send_to_AtService = false;       /* */
    AT_CommandLine_t initial_pos;         /* Used for commands handled in both AtService and at_core */
    /* Flags a transition for Phone_GlobalLocks to LOCKS_READY. */
    bool Extended = false;

    /* Is there any parser and does it have any command line to parse? */
    if ((parser_p == NULL) || (parser_p->CommandLine == NULL) ||
            (parser_p->Pos == NULL)) {
        return false;
    }

    /* This printout will occur both when a command is received */
    /* and when we receive a signal for a pending command. */
    ContinueParsing = true;
    /* Must allocate space for the worst case scenario of +CGMR. */
    ResultText = (AT_CommandLine_t) calloc(1, RESULT_TEXT_SIZE + 1);

    if (NULL == ResultText) {
        return false;
    }

    do {
        /* If there is no pending AT command we shall look for another command. */
        *ResultText = END_OF_STRING;

        if (!parser_p->IsPending && !parser_p->isWaiting) {
            if (Extended && !(parser_p->Mode == AT_MODE_SET)
                    && !((*(parser_p->Pos) == AT_COMMAND_SEPARATOR)
                         || (*(parser_p->Pos) == END_OF_STRING))) {
                ResultCode = AT_ERROR;
            } else { /* TODO: R13 PAS checked parser_p->SupplementaryServiceThroughDialString here. */
                Extended = ((*(parser_p->Pos) == '*') || (*(parser_p->Pos) == '+') ||
                            (strncmp((char *) parser_p->Pos, ";*", 2) == 0) ||
                            (strncmp((char *) parser_p->Pos, ";+", 2) == 0));

                /* Print out the received command string for the external channel. */
                if (parser_p->connection_type == ATC_CONNECTION_TYPE_EXTERNAL &&
                        parser_p->Pos == parser_p->CommandLine &&
                        strlen((char *)parser_p->Pos) > 0) {
                    at_context = atc_context_get_by_context_id(parser_p->ContextID);
                    ATC_LOG_I("AT(%s)> %s",
                              (at_context != NULL && at_context->conn != NULL) ? at_context->conn->device_name : NULL,
                              parser_p->Pos);
                }

                /* Save the initial parser->Pos so we can restore it if sending to AtService. */
                initial_pos = parser_p->Pos;

                ContinueParsing = Parser_GetNextCommand(parser_p, &ResultCode);

                if (ContinueParsing) {
                    ResultCode = (parser_p->HandlerFunction)(parser_p, ResultText, Message);
                }

                /* Two cases either not recognized in Parser_GetNextCommand or
                 * handler function wish to send the command on to the AT Service.
                 * This is only allowed if the command came on an external connection.*/
                if (parser_p->Command == AT_UNKNOWN_COMMAND &&
                        parser_p->connection_type == ATC_CONNECTION_TYPE_EXTERNAL) {
                    send_to_AtService = true;
                }
            }
        } else if (parser_p->IsPending) {
            if (parser_p->Command == AT_NO_COMMAND) {
                ResultCode = AT_OK;
                ContinueParsing = false;
            } else {
                ResultCode = parser_p->HandlerFunction(parser_p, ResultText,
                                                       Message);
            }

            parser_p->isWaiting = false;
        } else if (parser_p->isWaiting) {
            /* waiting for response from AtService */
            at_context = atc_context_get_by_context_id(parser_p->ContextID);

            if (at_context != NULL) {
                if (at_context->is_waiting) {
                    /* still waiting */
                    free(ResultText);
                    return true;
                } else {
                    /* no longer waiting, use the result */
                    ResultCode = at_context->result_code;
                    parser_p->CMEE_ErrorType = at_context->cme_error_code;
                    parser_p->isWaiting = false;
                }
            } else {
                ResultCode = AT_ERROR;
            }
        }

#ifdef AT_SERVICE

        if (send_to_AtService) {
            /* could be a command handled by AtService
             * parser_p->Pos points to a character other than \0 and ;
             * because Parser_GetNextCommand has been executed
             * ResultCode is AT_ERROR */

            /* Restore parser_p->Pos value to the initial value to allow parsing in AtService. */
            parser_p->Pos = initial_pos;
            send_to_AtService = false; /* will send it now */
            nbr = send_cmd_to_atservice(parser_p);

            if (nbr > 0) {
                if (ResultCode == AT_PENDING) {
                    parser_p->IsPending = true;
                }

                parser_p->isWaiting = true;
                free(ResultText);
                return true;
            }

            /* ResultCode is already set to AT_ERROR by Parser_GetNextCommand */
        }

#endif /* AT_SERVICE */

        /* If the response for the last AT command is asynchronous the status has
         * been set to pending. Then this function shall return. The parsing will
         * resume when the response is handled and this function is called again! */
        if (ResultCode == AT_PENDING) {
            parser_p->IsPending = true;
            free(ResultText);
            return true;
        }

        parser_p->IsPending = false;
    } while (ContinueParsing && (ResultCode == AT_OK));

    /* Parser_SendResponse( parser_p, ResultCode, AT_BUILD_RESPONSE, ResultText ); */
    if (AT_OK == ResultCode) {
        ResultCode = Parser_GetSuccessResult(parser_p);
    }

    parser_send_command_result(parser_p, ResultCode);

    Parser_SetSuccessResult(parser_p, AT_OK);

    /* Flush the pending queue for any unsent unsoliciteds. */
    parser_pending_queue_flush();

    /* Deallocate the dynamic command line buffer. */
    Parser_Clear(parser_p);
    /* TODO: R13 PAS had introduced handling of PIN2 & PUK2 here. */
    free(ResultText);
    return true;
} /* End of Parser_ParseCommandLine() */


#ifdef AT_SERVICE
/*
 *===============================================================================
 *
 *   Function: send_cmd_to_atservice
 *
 *   INPUT:  parser_p    - Pointer to the current parser state.
 *
 *   OUTPUT: parser_p->
 *             Pos         - Points to the first character after after the command
 *                             sent to AtService
 *
 *   RETURNS: Returns the actual number of bytes sent to the AtService and -1
 *            if it failed to send the command.
 *
 *===============================================================================
 */
static int send_cmd_to_atservice(AT_ParserState_s *parser_p)
{
    int count;
    unsigned char *first_index;
    bool in_string;

    /* If there is a leading command separator, skip it! */
    if (*(parser_p->Pos) == AT_COMMAND_SEPARATOR) {
        parser_p->Pos++;
    }

    /* AtService can only process a single command at a time.
     * Locate where the first command ends, and send it. This function will
     * be called again if there remains commands to process.
     */
    for (count = 0, in_string = false, first_index = (unsigned char *)parser_p->Pos;
            (in_string ||
             (*parser_p->Pos != AT_COMMAND_SEPARATOR && *parser_p->Pos != END_OF_STRING));
            parser_p->Pos++, count++) {

        if (*(parser_p->Pos) == '"') {
            in_string = !in_string;
        }
    }

    count = atc_write_to_at_service(parser_p->ContextID, first_index, count);

    return count;
}
#endif /* AT_SERVICE */


/*
 *===============================================================================
 *
 *   Function: Parser_Initiate
 *
 *   INPUT:  parser_p - Pointer to the current parser state.
 *           Source   - Pointer to the command line.
 *           Lengh    - The length of the source command line.
 *
 *   OUTPUT: Length   - Returns the actual number of characters in the formatted
 *                      command line.
 *           parser_p->
 *             CommandLine - Points to a dynamically allocated buffer with the
 *                           formatted command line.
 *             Pos         - Points to the first character of CommandLine.
 *             Command     - AT_NO_COMMAND if the command line was ok,
 *                           AT_ERROR otherwise.
 *             IsPending   - false.
 *
 *   RETURNS: TRUE if the command line was syntactically correct and no SW
 *            error detected, otherwise false.
 *
 *   Removes all white space characters outside string parameters (' ' & '\t').
 *   Converts all "\xx" sequences in string parameters to the character xx.
 *   Converts double backslash ("\\") to single backslash ("\")
 *
 *===============================================================================
 */
static bool Parser_Initiate(AT_ParserState_s *parser_p, AT_CommandLine_t Source,
                            AT_LineLength_t *Length)
{
    unsigned char Current; /* Temp storage the currently parsed character. */
    unsigned char Previous = 0; /* Temp storage the previous parsed character. */
    AT_CommandLine_t Dest; /* Current position in the destination buffer. */
    bool InStringParam; /* Flags when parsing a string parameter. */
    /* Reset the parser status. */
    parser_p->Command = AT_ERROR;

    Parser_SetSuccessResult(parser_p, AT_OK);
    parser_p->IsPending = false;
#ifdef STATIC_COMMAND_LINE_BUFFER
    /* Null terminate the command line buffer. */
    parser_p->CommandLine[0] = END_OF_STRING;
#else
    /* The formatted command line will be stored in a dynamic buffer owned by */
    /* the parser. */
    parser_p->CommandLine = AT_AllocateCmdBuffer((AT_LineLength_t)(*Length + 1),
                            parser_p->ContextID);
#endif
    Dest = parser_p->CommandLine;
    InStringParam = 0;

    do {
        Current = *Source++;

        /* Only handle characters that are not string parameters. */
        if (!InStringParam) {
            if (Current == AT_STRING_PARAM_BOUNDER) {
                *Dest++ = Current;
                InStringParam = true;
                Previous = 0xFF;
            } else {
                /* Skip all whitespaces. */
                if ((Current != ' ') && (Current != '\t')) {
                    *Dest++ = Current;
                }
            }
        } else {
            /* inside quotes */
            if ((*(Source - 2) == '\\') && (Previous == '\\')) { /*Current = *(Source-1)*/
                Previous = 0xFF; /* special case for  e.i "\"\\" */
            } else {
                Previous = *(Source - 2);
            }

            if ((Current == AT_STRING_PARAM_BOUNDER) && (Previous != '\\')) {
                InStringParam = false;
            }

            *Dest++ = Current;
        }
    } while ((Current != parser_p->S3)
             && ((Dest - parser_p->CommandLine) < *Length));

    /* Remove any line termination characters, from now on all strings shall */
    /* be null terminated! */
    if (Current == parser_p->S3) {
        Dest--;
    }

    /* Terminate the string with an END_OF_STRING character. */
    *Length = (AT_LineLength_t)(Dest - parser_p->CommandLine);
    *Dest = END_OF_STRING;

    /* If the command line is terminated before the current string parameter */
    /* is ended there is a syntax error. */
    if (InStringParam) {
        return false;
    }

    /* The parsing of the command line may now start. */
    parser_p->Command = AT_NO_COMMAND;
    parser_p->CommandTableIndex = -1;
    parser_p->Pos = parser_p->CommandLine;
    parser_p->CommandLineLength = *Length;

    return true;
} /* End of Parser_Initiate */

/*
 *===============================================================================
 *
 *   Function: Parser_AT
 *
 *   INPUT: MsgData_p          - pointer to data
 *          MsgDataLength      - length of the data
 *          RoutingTable_Entry - Entry in Routing Table for used connection
 *
 *   OUTPUT: None
 *
 *   This function initiates parsing of the command line and calls the actual
 *   parser function that can handle asynchronous AT commands.
 *
 *===============================================================================
 */
void Parser_AT(uint8_t *MsgData_p, AT_LineLength_t MsgDataLength,
               AT_DataType_t TypeOfData, AT_ParserState_s *parser_p)
{
    bool InitOK; /* Flags the success of initiation. */
    AT_LineLength_t CommandLineLength; /* Length of the command line. */
    CommandLineLength = MsgDataLength;

    /* Reject the new command line if the last one is not yet finished. */
    if (Parser_IsParsingCommandLine(parser_p)) {
        if (Parser_IsAbortable(parser_p)) {
            /* Abort execution of the current command. */
            parser_p->abort = true;
            Parser_ParseCommandLine(parser_p, NULL);
            parser_p->abort = false;
        }

        return;
    }

    if (0 == CommandLineLength) {
        if (AT_DATA_COMMAND == TypeOfData) {
            /* Command with empty message, assume AT<S3> */
            parser_send_command_result(parser_p, AT_OK);
        }

        return;
    }

    /* Check the origin of the command before parsing. */
    if (parser_p->connection_type == ATC_CONNECTION_TYPE_UNUSED) {
        ATC_LOG_E("parser_p received command from unknown connection\n");
        return;
    }

    /* Initiate the parser with the new command line and start parsing. */
    InitOK = Parser_Initiate(parser_p, MsgData_p, &CommandLineLength);

    if (InitOK) {
        Parser_ParseCommandLine(parser_p, NULL);
    } else {
        /* Clear the parser status and deallocate any dynamic buffers. */
        Parser_Clear(parser_p);
        parser_send_command_result(parser_p, AT_ERROR);
    }

} /* End of Parser_AT */

/*
 *===============================================================================
 *
 *   Function: atc_get_response
 *
 *   INPUT:  response     - Enumeration of the response to add.
 *           verbose      - Flags whether the response should be verbose or numeric.
 *
 *   RETURNS: If successful a pointer to the response string is returned,
 *            otherwise NULL is returned.
 *
 *===============================================================================
 */
char *atc_get_response(AT_Command_e response, bool verbose)
{
    unsigned char i = 0;
    char *response_p = NULL;

    while ((i < AT_RESPONSE_TABLE_LENGTH) &&
            (response != atc_response_table[i].response)) {
        i++;
    }

    if (i < AT_RESPONSE_TABLE_LENGTH) {
        if (verbose) {
            return atc_response_table[i].verbose_string;
        } else {
            return atc_response_table[i].numeric_string;
        }
    }

    return NULL;
}


/*************************************************************************
 *
 * Name: parser_send_command_result
 *
 * Parameters:  parser_p      [IN] Current parser_p.
 *
 *              CommandResult  [IN] Command result.
 *
 *
 * Returns:     -
 *
 * Description: Sends command result to AT
 *              Very little fault handling at the moment...
 *
 *
 **************************************************************************/
static void parser_send_command_result(AT_ParserState_s *parser_p, AT_Command_e command_result)
{
    char *cmd;
    size_t len;
    int nbr_written;
    char *response_p = NULL;

    /* Allocate enough memory without optimizing; longest responses are CMEE errors (16 +
     * error text length) or CMS errors (RESPONSE_HEADER_LENGTH + CMS_ERROR_RESPONSE_LENGTH +
     * RESPONSE_TRAILER_LENGTH + 1).
     */
    cmd = (char *)calloc(1, RESPONSE_HEADER_LENGTH + CMS_ERROR_RESPONSE_LENGTH +
                         RESPONSE_TRAILER_LENGTH + 1 +
                         strlen(parser_get_cmee_description(parser_p->CMEE_ErrorType)));

    if (parser_p->VerboseResponse == 1) {
        switch (command_result) {
        case AT_OK:
        case AT_CONNECT:
        case AT_ERROR:
        case AT_NO_CARRIER:
            response_p = atc_get_response(command_result, parser_p->VerboseResponse);
            sprintf(cmd, "\r\n%s\r\n", response_p);
            break;
        case AT_CME_ERROR:

            switch (parser_p->CMEE) {
            case 1:
                response_p = atc_get_response(command_result, parser_p->VerboseResponse);
                sprintf(cmd, "\r\n%s: %u\r\n", response_p, parser_p->CMEE_ErrorType);
                break;
            case 2:
                response_p = atc_get_response(command_result, parser_p->VerboseResponse);
                sprintf(cmd, "\r\n%s: %s\r\n", response_p, parser_get_cmee_description(parser_p->CMEE_ErrorType));
                break;
            case 0: /* Deliberate fall-through */
            default:
                response_p = atc_get_response(AT_ERROR, parser_p->VerboseResponse);
                sprintf(cmd, "\r\n%s\r\n", response_p);
            }

            break;
        case AT_CMS_ERROR:
            response_p = atc_get_response(command_result, parser_p->VerboseResponse);
            sprintf(cmd, "\r\n%s: %d\r\n", response_p, parser_p->CMS_ErrorType);
            break;
        default:
            response_p = atc_get_response(AT_ERROR, parser_p->VerboseResponse);
            sprintf(cmd, "\r\n%s\r\n", response_p);
        }
    } else {
        switch (command_result) {
        case AT_OK:
        case AT_CONNECT:
        case AT_NO_CARRIER:
        case AT_ERROR:
        case AT_NO_DIALTONE:
        case AT_BUSY:
        case AT_NO_ANSWER:
            response_p = atc_get_response(command_result, parser_p->VerboseResponse);
            break;
        default:
            response_p = atc_get_response(AT_ERROR, parser_p->VerboseResponse);
        }

        sprintf(cmd, "\r\n%s\r\n", response_p);
    }

    len = strlen(cmd);
    nbr_written = parser_write_formatted_text(parser_p, (unsigned char *) cmd, len);
    free((void *)cmd);
}


/*************************************************************************
 *
 * Name: parser_get_cmee_description
 *
 * Parameters:  CME error code
 *
 * Returns:     Pointer to string description corresponding with error code
 *
 **************************************************************************/
const char *parser_get_cmee_description(exe_cmee_error_t CMEE_ErrorType)
{
    if (Max_CMEE_Errors > CMEE_ErrorType) {
        return CMEE_Errors[CMEE_ErrorType];
    } else {
        ATC_LOG_W("No description available for CME error code: %d!", CMEE_ErrorType);
        /* The last string in the array is always empty */
        return CMEE_Errors[Max_CMEE_Errors - 1];
    }
}


/*************************************************************************
 *
 * Name: Parser_GetSuccessResult
 *
 * Parameters:  parser_p      [IN] Current parser_p.
 *
 * Returns:     Desired AT_Command_e response to use upon a successful
 *              command execution
 *
 * Description: Gets the desired AT_Command_e response to use upon a successful
 *              command execution
 *
 *
 **************************************************************************/
static AT_Command_e Parser_GetSuccessResult(AT_ParserState_s *parser_p)
{
    return parser_p->SuccessResult;
}

/*************************************************************************
 *
 * Name: Parser_SetSuccessResult
 *
 * Parameters:  parser_p      [IN] Current parser_p.
 *              result        [IN] Desired AT_Command_e response to use upon
 *              a successful command execution, instead of AT_OK.
 *              Allowed values are
 *                  AT_OK (default)
 *                  AT_CONNECT
 *
 * Description: Sets the desired AT_Command_e response to use upon a successful
 *              command execution, instead of AT_OK.
 *              The setting is valid for the duration of the current command,
 *              and is automatically reset to AT_OK.
 *
 **************************************************************************/
void Parser_SetSuccessResult(AT_ParserState_s *parser_p,  AT_Command_e result)
{
    parser_p->SuccessResult = result;
}


#ifndef EXE_USE_ATC_CUSTOMER_EXTENSIONS
bool parser_supports_async_cgact(AT_ParserState_s *parser_p)
{
    return false;
}
#endif
