/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef ATC_COMMAND_TABLE_H
#define ATC_COMMAND_TABLE_H
/*************************************************************************
 * Includes
 *************************************************************************/

/* ATC related header files */
#include "atc_parser.h"

/*************************************************************************
 * Types, constants and external variables
 *************************************************************************/

/* Define a table entry for AT Commands. */
typedef struct { /* Structure for AT command entries: */
    AT_Command_e Command; /* ... AT command enumeration. */
    char *String; /* ... AT command string. */
    AT_Command_e(*HandlerFunction)(AT_ParserState_s *, AT_CommandLine_t,
                                   AT_ResponseMessage_t);
    BitField_t Mode; /* ... possible modes for the command. */
    BitField_t Class; /* ... class membership for the command. */
} AT_CommandTableRecord_s;

/* Define a type for the AT command table entry enumeration. */
typedef unsigned char AT_TableSize_t;

extern const AT_TableSize_t AT_COMMAND_TABLE_LENGTH;

extern const AT_CommandTableRecord_s AT_CommandTable[];

/*************************************************************************
 * Declaration of functions
 **************************************************************************/

/*************************************************************************/
#endif /* ATC_COMMAND_TABLE_H */
