/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/* ATC related header files */
#include <atc_command_list.h>
#include <atc_config.h>
#include <atc_log.h>
#include <atc_parser.h>


/*
 *===============================================================================
 *
 *   Function: AT_STAR_EACS_Handle
 *
 *   INPUT:   parser_p           - Pointer to the current parser state.
 *            InfoText           - Pointer to a string buffer to put information text.
 *            message            - Not used.
 *
 *   OUTPUT:  parser_p->Pos      - The current position in the command line.
 *
 *
 *   RETURNS: A status code for the execution of the command is returned:
 *            AT_OK        - The command was successfully handled.
 *            AT_ERROR     - There was a general error.
 *            AT_CME_ERROR - There was a GSM specific error, the error code is
 *                           returned in InfoText.
 *
 *   The *EACS function is used by accessories to identify themselves
 *   Ensemble SIR 2.3 C26/C/E rev. PC16
 *
 *   SET  - Identify an accessory.
 *   READ - Show current status for all accessories.
 *   TEST - Test if command is supported and also show possible settings.
 *
 *===============================================================================
 */
AT_Command_e AT_STAR_EACS_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t InfoText, AT_ResponseMessage_t message)
{
    ATC_LOG_I("\nInside the EACS handle\n");
    return AT_OK;
} /* AT_Command_e AT_STAR_EACS_Handle */

