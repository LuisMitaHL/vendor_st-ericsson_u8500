/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "atc_log.h"

#include "exe.h"
#include "exe_internal.h"
#include "exe_request_record.h"
#include "exe_request_record_p.h"


#define EXE_MAX_NUMBER_CLIENT_TAG   (128)   /* MUST!!! be a power of 2 */


static int records_in_use = 0;              /* Number of request records currently in use */
static int num_failed_allocations = 0;      /* Number of failed consecutive allocations */
static bool print_failed_allocation = true; /* Print 'failed allocation' flag */

static exe_request_record_t *s_record_array[EXE_MAX_NUMBER_CLIENT_TAG] = { NULL };


/********************************************************************
 * Private prototypes
 ********************************************************************
 */
static exe_request_record_t *request_record_allocate(exe_request_id_t request_id);
static int request_record_get_id(exe_request_record_t *record_p);
static exe_request_record_t *request_record_get_from_client_tag(int client_tag);
static void request_record_free(exe_request_record_t *record_p);


/********************************************************************
 * Private methods
 ********************************************************************
 */

/**
 * request_record_allocate()
 *
 * Find an unallocated or free entry in the request record array. Each allocated
 * and free entry is marked as such and are available for use. Each request has a unique
 * 'id' field (request record ID) that can be used to quickly retrieve the request record
 * from the array.
 *
 * Allocation is made in a round-robin fashion so that all array entries are
 * used evenly.
 */
exe_request_record_t *request_record_allocate(exe_request_id_t request_id)
{
    static unsigned int start_id = EXE_MAX_NUMBER_CLIENT_TAG - 1;
    exe_request_record_t *record_p = NULL;
    unsigned int i, id, end_id;

    /*
     * Make a single pass over the array, starting at the last allocated position + 1.
     * Never use record_id's in the range; 0 - (EXE_MAX_NUMBER_CLIENT_TAG - 1).
     * So, when the id counter wraps around, add EXE_MAX_NUMBER_CLIENT_TAG to it.
     */
    for (id = start_id + 1, end_id = start_id + EXE_MAX_NUMBER_CLIENT_TAG;
            id != end_id;
            id++) {

        /* Calculate array number */
        i = id % EXE_MAX_NUMBER_CLIENT_TAG;

        /* Array position never used before, allocate memory for record */
        if (NULL == s_record_array[i]) {
            record_p = calloc(1, sizeof(exe_request_record_t));

            if (!record_p) {
                ATC_LOG_E("calloc failed for record_p!");
                goto error;
            }

            s_record_array[i] = record_p;
        }

        /* Array position is allocated, reuse if it is free */
        if (NULL != s_record_array[i]) {
            record_p = s_record_array[i];

            if (0 == record_p->ref_count) {
                /* Set the current 'id' for the record, but
                 * if it has wrapped around, adjust it first. */
                if (EXE_MAX_NUMBER_CLIENT_TAG > id) {
                    id += EXE_MAX_NUMBER_CLIENT_TAG;
                }

                record_p->id = start_id = id;
                record_p->ref_count++;
                records_in_use++;
                ATC_LOG_D("Allocated request record 0x%X[%d] \"%s\" (%p). "
                          "Record(s) currently in use %d.",
                          id, i, exe_get_request_description(request_id),
                          record_p, records_in_use);
                break;
            }
        }

        record_p = NULL;
    }

    if (!record_p) {
        /*
         * Request record allocation has failed!
         * In order not to drown the log with a potentially huge number of error log
         * entries, produce a print when request table exhaustion is first detected,
         * then count silently. When again allocation succeeds, produce a print with
         * number of consecutively failed request record allocations.
         */
        if (print_failed_allocation) {
            ATC_LOG_E("Request table exhausted!");
            print_failed_allocation = false;
            num_failed_allocations = 1;
            request_record_dump_table();
        } else {
            num_failed_allocations++;
        }
    } else {
        /* Is this the first successful allocation after previous failure(s)? */
        if (!print_failed_allocation) {
            ATC_LOG_I("Request table exhaustion resolved. "
                      "Failed %d consecutive allocation requests.",
                      num_failed_allocations);
            print_failed_allocation = true;
        }
    }

error:
    return record_p;
}


int request_record_get_id(exe_request_record_t *record_p)
{
    if (record_p) {
        return (int)record_p->id;
    } else {
        return 0;
    }
}


exe_request_record_t *request_record_get_from_client_tag(int client_tag)
{
    exe_request_record_t *record_p;
    unsigned int i, id;

    id = (unsigned int)client_tag;
    i = id % EXE_MAX_NUMBER_CLIENT_TAG;
    record_p = s_record_array[i];

    if (NULL == record_p) {
        ATC_LOG_E("Invalid request record id 0x%X[%d]", id, i);
        goto error;
    }

    if (0 == record_p->ref_count) {
        ATC_LOG_E("Invalid state for request record 0x%X[%d] \"%s\" (%p)!",
                  id, i, exe_get_request_description(record_p->request_id),
                  record_p);
        goto error;
    }

    if (id != record_p->id) {
        ATC_LOG_E("Stale request record 0x%X[%d] \"%s\", current value is 0x%X!",
                  id, i, exe_get_request_description(record_p->request_id),
                  record_p->id);
        goto error;
    }

    return record_p;

error:
    return NULL;
}


void request_record_free(exe_request_record_t *record_p)
{
    unsigned int i, id;

    if (NULL == record_p) {
        ATC_LOG_E("Invalid request record pointer!");
        goto error;
    }

    id = record_p->id;
    i = id % EXE_MAX_NUMBER_CLIENT_TAG;

    if (0 < record_p->ref_count) {

        record_p->ref_count--;

        if (0 == record_p->ref_count) {
            records_in_use--;
            ATC_LOG_D("Freed request record 0x%X[%d] \"%s\" (%p). "
                      "Record(s) currently in use %d",
                      id, i, exe_get_request_description(record_p->request_id),
                      record_p, records_in_use);
        }
    } else {
        ATC_LOG_E("Request record 0x%X[%d] \"%s\" (%p) is already freed!",
                  id, i, exe_get_request_description(record_p->request_id),
                  record_p);
    }

error:
    return;
}


/********************************************************************
 * Public methods
 ********************************************************************
 */

exe_request_record_t *request_record_create(exe_request_id_t request_id,
        exe_request_t request,
        exe_t *exe_p,
        void *request_data_p,
        void *caller_data_p,
        int *client_tag_p)
{
    exe_request_record_t *record_p = request_record_allocate(request_id);

    if (NULL != record_p) {
        /* NOTE: Ensure that all request structure members are intialized.
         * The request structure is reused instead of repeatedly freed and allocated.
         */
        record_p->exe_p = exe_p;
        record_p->request_id = request_id;
        record_p->request = request;
        record_p->request_data_p = request_data_p;   /* request_data is allocated/deallocated in requester if applicable */
        record_p->caller_data_p = caller_data_p;
        record_p->state = EXE_STATE_REQUEST;
        record_p->client_result_code = 0;
        record_p->response_data_p = NULL;
        record_p->client_data_p = NULL;
        record_p->abort = false;

        *client_tag_p = request_record_get_client_tag(record_p);
    }

    return record_p;
}


void request_record_destroy(exe_request_record_t *record_p)
{
    request_record_free(record_p);
}


void request_record_destroy_all()
{
    exe_request_record_t *record_p;
    int purged = 0;
    int i;

    /* Dump the request record table to log */
    request_record_dump_table();

    /* Free memory allocated to all request records */
    for (i = 0; i < EXE_MAX_NUMBER_CLIENT_TAG; i++) {
        record_p = s_record_array[i];

        if (NULL != record_p) {
            free(record_p);
            s_record_array[i] = NULL;
        }
    }
}


void request_record_retain(exe_request_record_t *record_p)
{
    if (NULL == record_p) {
        ATC_LOG_E("Invalid request record pointer!");
        return;
    }

    if (1 > record_p->ref_count) {
        unsigned int i, id;

        id = record_p->id;
        i = id % EXE_MAX_NUMBER_CLIENT_TAG;
        ATC_LOG_E("Ref_count is zero for request record 0x%X[%d] \"%s\" (%p)!",
                  id, i, exe_get_request_description(record_p->request_id),
                  record_p);
        return;
    }

    record_p->ref_count++;
}


exe_request_t request_record_get_request(exe_request_record_t *record_p)
{
    if (NULL == record_p) {
        ATC_LOG_E("Invalid request record pointer!");
        return (exe_request_t)NULL;
    }

    return record_p->request;
}


void request_record_set_request(exe_request_record_t *record_p, exe_request_t request)
{
    if (NULL == record_p) {
        ATC_LOG_E("Invalid request record pointer!");
    } else {
        record_p->request = request;
    }
}


void request_record_set_abort(exe_request_record_t *record_p)
{
    if (NULL == record_p) {
        ATC_LOG_E("Invalid request record pointer!");
    } else {
        record_p->abort = true;
    }
}


uintptr_t request_record_get_client_tag(exe_request_record_t *record_p)
{
    if (NULL == record_p) {
        ATC_LOG_E("Invalid request record pointer!");
        return (uintptr_t)NULL;
    }

    return ((uintptr_t)request_record_get_id(record_p));
}


exe_request_record_t *request_record_get_record_from_client_tag(int client_tag)
{
    return request_record_get_from_client_tag(client_tag);
}


void request_record_set_response_data(exe_request_record_t *record_p, void *response_data_p)
{
    if (NULL == record_p) {
        ATC_LOG_E("Invalid request record pointer!");
    } else {
        record_p->response_data_p = response_data_p;
    }
}


void request_record_set_resultcode(exe_request_record_t *record_p, int error_code)
{
    if (NULL == record_p) {
        ATC_LOG_E("Invalid request record pointer!");
    } else {
        record_p->client_result_code = error_code;
    }
}


void *request_record_get_response_data(exe_request_record_t *record_p)
{
    if (NULL == record_p) {
        ATC_LOG_E("Invalid request record pointer!");
        return NULL;
    }

    return record_p->response_data_p;
}


void request_record_set_request_data(exe_request_record_t *record_p, void *request_data_p)
{
    if (NULL == record_p) {
        ATC_LOG_E("Invalid request record pointer!");
    } else {
        record_p->request_data_p = request_data_p;
    }
}


void *request_record_get_request_data(exe_request_record_t *record_p)
{
    if (NULL == record_p) {
        ATC_LOG_E("Invalid request record pointer!");
        return NULL;
    }

    return record_p->request_data_p;
}


void request_record_set_caller_data(exe_request_record_t *record_p, void *caller_data_p)
{
    if (NULL == record_p) {
        ATC_LOG_E("Invalid request record pointer!");
    } else {
        record_p->caller_data_p = caller_data_p;
    }
}


void *request_record_get_caller_data(exe_request_record_t *record_p)
{
    if (NULL == record_p) {
        ATC_LOG_E("Invalid request record pointer!");
        return NULL;
    }

    return record_p->caller_data_p;
}


void request_record_set_client_data(exe_request_record_t *record_p, void *client_data_p)
{
    if (NULL == record_p) {
        ATC_LOG_E("Invalid request record pointer!");
    } else {
        record_p->client_data_p = client_data_p;
    }
}


void *request_record_get_client_data(exe_request_record_t *record_p)
{
    if (NULL == record_p) {
        ATC_LOG_E("Invalid request record pointer!");
        return NULL;
    }

    return record_p->client_data_p;
}


bool request_record_is_valid(exe_request_record_t *record_p)
{
    bool found = false;
    unsigned int i;

    if (NULL == record_p) {
        ATC_LOG_E("Invalid request record pointer!");
        return false;
    }

    for (i = 0; i < EXE_MAX_NUMBER_CLIENT_TAG; i++) {

        if (record_p == s_record_array[i] && 0 < record_p->ref_count) {
            found = true;
            break;
        }
    }

    return found;
}


/**
 * request_record_service_purge()
 *
 * Find all request records for a specific service and terminate them all.
 *
 * This function is typically called when a service disconnects. The purpose is to
 * clean up so that pending requests are terminated properly, and the request record
 * slot becomes available for use again.
 */
int request_record_service_purge(exe_service_t service)
{
    exe_request_record_t *record_p;
    int purged = 0;
    int i;

    for (i = 0; i < EXE_MAX_NUMBER_CLIENT_TAG; i++) {
        record_p = s_record_array[i];

        if (NULL != record_p && 0 != record_p->ref_count) {
            if (service == exe_get_request_service(record_p->request_id)) {
                /*
                 * Request matching the service found. Terminate it.
                 *
                 * NOTE: This implementation simply destroys the request
                 * record. Ideally the request handler should be called
                 * with a code indicating that the request needs to be
                 * terminated and the handler should do whatever necessary
                 * to clean up and terminate at its end. Unfortunately
                 * there is currently no consistent way of doing this
                 * even for request handlers that support aborting a
                 * command. This will have to be addressed by a future
                 * change.
                 */
                while (0 != record_p->ref_count) {
                    request_record_destroy(record_p);
                }

                purged++;
            }
        }
    }

    return purged;
}


void request_record_dump_table()
{
    int i = 0;
    char *msg_string_p = NULL;
    char *state_strings[] = {"REQUEST", "RESPONSE", "State:%d"};
    char *state_string_p = NULL;
    exe_request_record_t *record_p;
    char state_message[30];
    char error_message[30];
    memset(&error_message, 0, sizeof(error_message));

    if (0 == records_in_use) {
        ATC_LOG_I("No request records currently in use.");
        return;
    }

    ATC_LOG_E("");
    ATC_LOG_E("Dump of Request Table: Request records currently in use: %d", records_in_use);
    ATC_LOG_E("+-------+------------+----------+------------------------------------------------------------------+");
    ATC_LOG_E("| Index | Request Id | State    | Description                                                      |");
    ATC_LOG_E("+-------+------------+----------+------------------------------------------------------------------+");

    for (i = 0; i < EXE_MAX_NUMBER_CLIENT_TAG; i++) {
        record_p = s_record_array[i];

        if (NULL != record_p && 0 != record_p->ref_count) {
            if (1 < record_p->state) {
                sprintf(state_message, state_strings[2], record_p->state);
                state_string_p = state_message;
            } else {
                state_string_p = state_strings[record_p->state];
            }

            msg_string_p   = (0 == record_p->ref_count) ? "" : (char *)exe_get_request_description(record_p->request_id);

            if (!msg_string_p) {
                sprintf(error_message, "UNKNOWN REQUEST! (id=%d)", record_p->request_id);
                msg_string_p = error_message;
            }

            ATC_LOG_E("| %-5d | 0x%08X | %-8s | %-64s |", i, record_p->id, state_string_p, msg_string_p);
        }
    } /* end of for loop */

    ATC_LOG_E("+-------+------------+----------+------------------------------------------------------------------+");
    ATC_LOG_E("");
}
