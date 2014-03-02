/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


/* TODO: The long term goal must be to move these defines and types
 *       to other files and remove this file */

#ifndef ATC_COMMON_H
#define ATC_COMMON_H

/*************************************************************************
 * Includes
 *************************************************************************/
#include <stdint.h>

#ifdef EXE_USE_ATC_CUSTOMER_EXTENSIONS
#include <atc_extended_common.h>
#endif
/*************************************************************************
 * Types, constants and external variables
 *************************************************************************/

typedef unsigned char ContextId_t;

/* Define a general type for bit fields. */
typedef unsigned char BitField_t;

/* Define a general type for string parameters. */
typedef char *StrParam_t;

#define END_OF_STRING (0x00) /* End of string character. */

/* Define a general type for integer parameters. */
typedef unsigned char IntParam_t;
typedef char SignedIntParam_t;
typedef char char_param_t;

#define MIN_INT_PARAM (0x00)
#define MAX_INT_PARAM (0xFF)
#define MAX_LONG_INT_PARAM (0xFFFFFFFF)
#define MAX_SIGNED_INT_PARAM (127)

#define AT_MAX_DATA_LENGTH (1500)

/*TODO: Need to merge in more types from R13 here...*/

/*************************************************************************
 * Declaration of functions
 **************************************************************************/

typedef uint16_t AT_DataLength_t;

/* Define a type for the AT command line length: */
typedef AT_DataLength_t AT_LineLength_t;

typedef enum {
    AT_DATA_COMMAND, AT_DATA_ABORT, AT_DATA_TRANSPARENT
} AT_DataType_t;


/*************************************************************************/
#endif /* ATC_COMMON_H */
