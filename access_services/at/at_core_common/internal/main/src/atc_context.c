/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <string.h>

#include "atc_context.h"
#include "atc_log.h"


static atc_context_t at_context[MAX_NBR_CONNECTIONS];

/* --------------------------------------------------------------
 *
 * --------------------------------------------------------------*/
void atc_context_init(atc_context_t *context)
{
    context->input_size = 0;
    context->input_pos = 0;
    context->input_p = NULL;
    context->conn = NULL;
    context->at_connection_type = ATC_CONNECTION_TYPE_UNUSED;
    context->parser_state = NULL;
    context->context_id = 0xFF;
    context->sockfd = -1;
    context->is_waiting = false;
    context->result_code = AT_OK;
    context->cme_error_code = CMEE_OK;
    context->input_mode = ATC_INPUT_MODE_NEW_LINE;
    context->next_input_mode = ATC_INPUT_MODE_NEW_LINE;
}

/* --------------------------------------------------------------
 * Initiate the at_context with proper start values.
 * --------------------------------------------------------------*/
void atc_context_init_all_contexts(void)
{
    int i;

    for (i = 0; i < MAX_NBR_CONNECTIONS; i++) {
        atc_context_init(&at_context[i]);
    }
}

/* --------------------------------------------------------------
 *
 * --------------------------------------------------------------*/
atc_context_t *atc_context_get_free(void)
{
    int i = 0;
    atc_context_t *result = NULL;

    while (i < MAX_NBR_CONNECTIONS && at_context[i].at_connection_type != ATC_CONNECTION_TYPE_UNUSED) {
        i++;
    }

    if (i < MAX_NBR_CONNECTIONS) {
        result = &at_context[i];
    }

    return result;
}

/* --------------------------------------------------------------
 *
 * --------------------------------------------------------------*/
int atc_context_get_number_of_connections(void)
{
    int count = 0;
    int i;

    for (i = 0; i < MAX_NBR_CONNECTIONS; i++) {
        if (at_context[i].conn != NULL) {
            count++;
        }
    }

    return count;
}

/* --------------------------------------------------------------
 * Returns the context with context_id if it exist, otherwise null.
 * --------------------------------------------------------------*/
atc_context_t *atc_context_get_by_context_id(unsigned char context_id)
{
    atc_context_t *result = NULL;

    if (context_id < MAX_NBR_CONNECTIONS) {
        if (at_context[context_id].parser_state) {
            if (at_context[context_id].parser_state->ContextID
                    == context_id) {
                result = &at_context[context_id];
            }
        } else {
            ATC_LOG_E("not connected.");
        }
    }

    return result;
}


bool atc_context_get_transparent(unsigned char context_id)
{
    atc_context_t *context_p = atc_context_get_by_context_id(context_id);

    if (!context_p) {
        return false;
    }

    return (ATC_INPUT_MODE_TRANSPARENT == context_p->input_mode);
}


void atc_context_set_transparent(unsigned char context_id, bool on)
{
    atc_context_t *context_p = atc_context_get_by_context_id(context_id);

    if (!context_p) {
        ATC_LOG_E("context_p is NULL");
        return;
    }

    if (on) {
        /* If mode is new line, mode handling has already preceded.
         * Change mode to transparent directly */
        if (context_p->input_mode == ATC_INPUT_MODE_NEW_LINE) {
            context_p->input_mode = ATC_INPUT_MODE_TRANSPARENT;
        } else {
            context_p->next_input_mode = ATC_INPUT_MODE_TRANSPARENT;
        }
    } else {
        context_p->input_mode = ATC_INPUT_MODE_WAIT_FOR_NEW_LINE;
    }
}


bool atc_context_connect_to_parser(atc_context_t *context)
{
    unsigned char parser_entry;
    bool result = false;

    parser_entry = ParserStateTable_GetFreeEntry();

    if (parser_entry != PARSER_STATE_TABLE_ENTRY_NOT_FOUND) {
        ContextId_t context_id = parser_entry;
        context->context_id = context_id;
        context->parser_state = ParserStateTable_GetParserState_ByEntry(parser_entry);
        ParserStateTable_ResetEntry(parser_entry);
        ParserStateTable_UpdateEntry(parser_entry, context_id);
        result = true;
    }

    return result;
}

bool atc_context_disconnect_from_parser(atc_context_t *context_p)
{
    if (!context_p) {
        ATC_LOG_E("context_p is NULL");
        return false;
    }

    ParserStateTable_ResetEntry(context_p->context_id);

    /* Free the input buffer, and reset the context */
    free(context_p->input_p);
    atc_context_init(context_p);

    return true;
}
