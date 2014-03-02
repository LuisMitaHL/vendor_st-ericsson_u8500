/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __message_handler_h__
#define __message_handler_h__ (1)

#include "cn_message_types.h"

typedef struct _cn_pdc_t cn_pdc_t; /* incomplete structure */

typedef enum {
    REQUEST_STATUS_ERROR   = -1, /* generic error */
    REQUEST_STATUS_UNKNOWN =  0, /* calloc() value */
    REQUEST_STATUS_PENDING =  1, /* awaiting response from modem */
    REQUEST_STATUS_DONE    =  2  /* request handling done (also used for error scenarios) */
} request_status_t;

typedef enum {
    REQUEST_STATE_FREE     = 0,
    REQUEST_STATE_REQUEST  = 1,
    REQUEST_STATE_RESPONSE = 2
    /* Use defines for request specific states */
} request_state_t;

typedef struct record {
    cn_message_type_t message_type; /* originating CN client message */
    cn_client_tag_t client_tag;
    int client_id;
    unsigned int request_id;
    request_status_t (*request_handler_p)(void *data_p, struct record *record_p);
    request_state_t state;
    void *request_data_p; /* CN request implementation specific data */
    void *ss_request_info_p; /* CN ss command specific data */
    cn_pdc_t *pdc_data_p;
    int   response_error_code;
    void *response_data_p;
} request_record_t;

typedef struct
{
    const cn_message_type_t msg_type;
    const char *msg_string_p;
} message_string_entry_t;

#ifdef ENABLE_MODULE_TEST
extern message_string_entry_t message_string_table[];
#endif /* ENABLE_MODULE_TEST */

request_record_t *request_record_create(cn_message_type_t message_type, const cn_client_tag_t client_tag, const int client_id);
void *request_record_get_modem_tag(const request_record_t *record_p);
request_record_t *request_record_from_modem_tag(void *modem_tag);
void request_record_free(request_record_t *record_p);
void request_record_free_all();
void handle_request(const cn_message_t *msg_p, const int client_id);
char* lookup_message_table(cn_message_type_t msg_type);
void print_request_table();

#endif /* __message_handler_h__ */
