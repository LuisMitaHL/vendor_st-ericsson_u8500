/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <atc_command_list.h>
#include <atc_config.h>
#include <atc_log.h>
#include <atc_parser.h>

/*
 *===============================================================================
 *
 *   Function: AT_CommandHandlerFunction
 *
 *   INPUT:   parser_p    - Pointer to the current parser state.
 *            result_text - Buffer to put result in.
 *            message     - Pointer to a message for any pending command.
 *
 *   OUTPUT:  parser_p->
 *              When the command line is terminated:
 *              IsPending   - Is cleared.
 *              CommandLine - The buffer is deleted.
 *              Pos         - The pointer is set to NIL.
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
 *   to a buffer with infomation about the result (message). The handler for the
 *   pending command will be called with the message pointer. Thereafter the
 *   rest of the commands are parsed.
 *
 *
 *===============================================================================
 */
/*static*/AT_Command_e AT_CommandHandlerFunction(AT_ParserState_s *parser_p,
        AT_CommandLine_t result_text, AT_ResponseMessage_t message)
{
    AT_Command_e result_code = AT_OK; /* Result code from the last executed command.*/
    unsigned char flag; /* Temporary storage for flags. */
    int i;

    /* Fix to avoid compilation warnings */
    flag = 1;

    if (flag == 1) {
        flag++;
    }

    /* Null terminate the string so that we can use "str" functions. */
    *result_text = END_OF_STRING;

    switch (parser_p->Command) {
    case AT_NO_COMMAND:
        result_code = AT_OK;
        break;

    case AT_ERROR:
    case AT_UNKNOWN_COMMAND:
        result_code = AT_ERROR;
        break;

    default: /* The command is identified but not yet implemented. */
        result_code = AT_ERROR;
        break;
    }

    return result_code;
}

