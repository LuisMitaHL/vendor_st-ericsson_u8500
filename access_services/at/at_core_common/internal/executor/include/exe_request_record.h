/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef EXE_REQUEST_RECORD_H
#define EXE_REQUEST_RECORD_H 1


#include "exe.h"

typedef struct exe_request_record_s exe_request_record_t;

/* Function pointer used for handling requests towards the modem */
typedef exe_request_result_t (*exe_request_t)(exe_request_record_t *record_p);


/*
 * Creates a request record. Allocates the struct, initializes resource and sets
 * the input values.
 *
 * Returns NULL if creation failed.
 */
exe_request_record_t *request_record_create(exe_request_id_t request_id,
        exe_request_t request,
        exe_t *exe_p,
        void *request_data_p,
        void *caller_data_p,
        int *client_tag_p);

/*
 * Destroys the request record by decrementing the ref_count.
 * Allocated resources must be deallocated prior call.
 */
void request_record_destroy(exe_request_record_t *record_p);

/*
 * Destroys all the request records.
 * Is typically called during shutdown to clean up.
 * Allocated resources must be deallocated prior call.
 */
void request_record_destroy_all(void);

/*
 * Retains the record to the caller even if another user destroys it.
 * The caller of this function must destroy the record when it doesn't
 * need it any more.
 */
void request_record_retain(exe_request_record_t *record_p);

/*
 * Sets the request associated with the record.
 */
void request_record_set_request(exe_request_record_t *record_p, exe_request_t request);

/*
 * Gets the request associated with the record.
 * Returns NULL if failed.
 */
exe_request_t request_record_get_request(exe_request_record_t *record_p);

/*
 * Sets the abort flag that will force an ongoing execution to be canceled
 * when the request is called.
 */
void request_record_set_abort(exe_request_record_t *record_p);

/*
 * Returns an integer alias for the request record that is to be used
 * as client tag if the record needs to be send outside the executor.
 */
uintptr_t request_record_get_client_tag(exe_request_record_t *record_p);

/*
 * Gets the pointer to the request record point from the client tag
 * (integer alias).
 * Returns NULL if no record could be found.
 */
exe_request_record_t *request_record_get_record_from_client_tag(int client_tag);

/*
 * Purges all pending request records associated with the service passed
 * as an argument in the call to this function.
 * Returns number of purged request records.
 */
int request_record_service_purge(exe_service_t service);

/*
 * Dumps the current contents of the request record table to log.
 */
void request_record_dump_table(void);

bool request_record_is_valid(exe_request_record_t *record_p);
void request_record_set_response_data(exe_request_record_t *record_p, void *response_data_p);
void *request_record_get_response_data(exe_request_record_t *record_p);
void request_record_set_request_data(exe_request_record_t *record_p, void *request_data_p);
void *request_record_get_request_data(exe_request_record_t *record_p);
void request_record_set_caller_data(exe_request_record_t *record_p, void *caller_data_p);
void *request_record_get_caller_data(exe_request_record_t *record_p);
void request_record_set_client_data(exe_request_record_t *record_p, void *client_data_p);
void *request_record_get_client_data(exe_request_record_t *record_p);
void request_record_set_resultcode(exe_request_record_t *record_p, int error_code);


#endif
