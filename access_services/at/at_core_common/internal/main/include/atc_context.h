/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef ATC_CONTEXT_H_
#define ATC_CONTEXT_H_

#include <stdbool.h>
#include <stdint.h>

#include "atc_config.h"
#include "atc_connection.h"
#include "atc_anchor.h"
#include "atc_parser.h"
#include "atc_common.h"
#include "atc_command_list.h"
#include "exe.h"

typedef struct {
    size_t input_size;                          /* Size of input buffer */
    size_t input_pos;                           /* Position in input buffer where next data should be inserted */
    uint8_t *input_p;                           /* Input buffer that receives command strings */
    atc_connection_type_t at_connection_type;
    anchor_connection_t *conn; /* The connection to the anchor (if any) */
    AT_ParserState_s *parser_state;
    ContextId_t context_id; /*The context id*/
    int sockfd; /* The id of file descriptor belonging to it's socket */
    bool is_waiting; /* Indicates waiting for response from AtService */
    AT_Command_e result_code; /* The final result from sending the command to AtService */
    exe_cmee_error_t cme_error_code;
    atc_input_mode_t input_mode;
    atc_input_mode_t next_input_mode;
} atc_context_t;

void atc_context_init(atc_context_t *context);
void atc_context_init_all_contexts(void);
atc_context_t *atc_context_get_free(void);
int atc_context_get_number_of_connections(void);
atc_context_t *atc_context_get_by_context_id(unsigned char context_id);
bool atc_context_connect_to_parser(atc_context_t *context);
bool atc_context_disconnect_from_parser(atc_context_t *context);
void atc_context_set_transparent(unsigned char context_id, bool on);
bool atc_context_get_transparent(unsigned char context_id);

#endif /* ATC_CONTEXT_H_ */
