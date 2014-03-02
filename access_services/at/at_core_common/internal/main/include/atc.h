/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef ATC_H
#define ATC_H

#include "atc_config.h"
#include "atc_anchor.h"
#include "atc_parser.h"
#include "atc_connection.h"
#include "atc_context.h"

/* Result code for answer OK from ATService. */
#define AT_RESULT_CODE_OK ('0')
/* Result code for answer ERROR from AtService. */
#define AT_RESULT_CODE_ERROR ('1')
/* Result code for answer ERROR from AtService. */
#define AT_RESULT_CODE_CME_ERROR ('2')
/* Size of a header sent from java*/
#define HEADER_SIZE (18)
/* Buffer size, messages received from AtService*/ /* TODO: Change value? */
#define BUFFER_SIZE (255)

#define MAX(A, B) (A)>(B)?(A):(B)
#define MIN(A, B) (A)<(B)?(A):(B)

#define AT_CTRL_Z 0x1A

typedef bool (*callback_function_t)(int, void *); /* TODO: Check if this can be removed */

typedef struct backend_connection_s {
    struct backend_connection_s *next;
    int fd;
    void *data_p;
    callback_function_t callback_function;
} backend_connection_t; /*TODO: Check if this can be removed*/

bool atc_configure_serial(anchor_connection_t *conn_p, bool echo, char veol, char verase);
bool atc_connect_external(anchor_connection_t *conn_p);
atc_context_t *atc_context_get_by_context_id(unsigned char context_id);
int atc_write_to_client(atc_context_t *context, unsigned char *data,
                        unsigned int length);
int atc_read_from_anchor(atc_context_t *context, unsigned char *data,
                         unsigned int length);
int atc_write_to_at_service(unsigned char contextID, unsigned char *cmd_p, size_t size);

bool atc_stop(void);

#define CONTEXTID_TO_CLIENTTAG(ChId)        ( ChId | 0x80 )
#define CLIENTTAG_TO_CONTEXTID(ClientTag)   (ContextId_t)( ClientTag & 0x7F )

#define MAX_DIGITS_IN_NUMBER 25

typedef enum {
    CHARACTER_NONE, /* No modifier characters given */
    CHARACTER_I, /* Modifier character I is given */
    CHARACTER_i, /* Modifier character i is given */
    CHARACTER_G, /* Modifier character G is given */
    CHARACTER_g, /* Modifier character g is given */
    CHARACTER_I_and_G, /* Both I and G is given */
    CHARACTER_I_and_g, /* Both I and g is given */
    CHARACTER_i_and_G, /* Both i and G is given */
    CHARACTER_i_and_g
    /* Both i and g is given */
} PAS_ModifierCharacters_t;

#define AT_DEBUG_PIPE "/dev/socket/atc"

#endif /* ATC_H */
