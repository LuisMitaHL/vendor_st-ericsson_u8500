/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "cn_macros.h"
#include "cn_mal_dial_dtmf_handler.h"

/*
 * The store size should be max number of calls plus additional space.
 * Max number of calls plus one should be sufficient ;)
 */
#define DTMF_STORE_SIZE 9

/*
 * Datatype for an array item in the DTMF storage.
 * Associates a call id with a DTMF string.
 */
typedef struct {
    cn_sint32_t               call_id;
    cn_request_dtmf_string_t* dtmf_p;
} mddh_dtmf_store_item_t;

/*
 * Datatype for the DTMF storage.
 */
typedef struct {
    int                    init_flag; /* Init flag set is zero when storage is initialized */
    mddh_dtmf_store_item_t array[DTMF_STORE_SIZE];
} mddh_dtmf_store_t;

/*
 * Global (file scope) storage variable. Note that the initalizer sets the init_flag to -1
 * to indicate that initialization is needed (will be performed first time the storage is used).
 */
static mddh_dtmf_store_t g_dtmf_store = { -1, { {0, NULL} } };

#ifdef USE_MAL_CS

static void init_dtmf_store_if_needed() {

    if (g_dtmf_store.init_flag < 0) {
        CN_LOG_D("Initialising DTMF store");
        memset(&g_dtmf_store, 0, sizeof(g_dtmf_store));
    }
}

int mddh_associate_dtmf_with_call_id(cn_sint32_t call_id, cn_request_dtmf_string_t* dtmf_p)
{

    CN_LOG_D("call_id = %d, dtmf_p = 0x%X", call_id, dtmf_p);

    init_dtmf_store_if_needed();
    if (dtmf_p) {
        int i;
        for (i = 0; i < DTMF_STORE_SIZE; i++) {
            if (!g_dtmf_store.array[i].dtmf_p) {
                g_dtmf_store.array[i].call_id = call_id;
                g_dtmf_store.array[i].dtmf_p  = dtmf_p;
                return 0;
            }
        }
    }
    CN_LOG_E("NULL pointer input or DTMF storage exhausted.");
    return -1;
}

cn_request_dtmf_string_t* mddh_fetch_call_id_associated_dtmf(cn_sint32_t call_id)
{
    cn_request_dtmf_string_t* dtmf_p = NULL;
    int i;

    CN_LOG_D("call_id = %d", call_id);

    init_dtmf_store_if_needed();
    for (i = 0; i < DTMF_STORE_SIZE; i++) {
        if (g_dtmf_store.array[i].dtmf_p && g_dtmf_store.array[i].call_id == call_id) {
            dtmf_p = g_dtmf_store.array[i].dtmf_p;
            g_dtmf_store.array[i].dtmf_p = NULL;
            CN_LOG_V("Found DTMF instance 0x%X associated with call_id = %d", dtmf_p, call_id);
        }
    }

    return dtmf_p;
}

cn_request_dtmf_string_t* mddh_util_extract_and_remove_dtmf(cn_request_dial_t* dial_p)
{
    cn_request_dtmf_string_t* dtmf_p = NULL;
    char* str_p = NULL;


    CN_LOG_D("dial_p = 0x%X", dial_p);

    if (dial_p) {
        CN_LOG_D("dialing string = %s", dial_p->dial.phone_number);
        for (str_p = dial_p->dial.phone_number; *str_p; str_p++)
        {
            char uc = *str_p & 0xDF; // Simple upper-case conversion.
            if (uc == 'W' || uc == 'P') {
                *str_p = 'p'; /* Make sure first dtmf digit is a 'p' and not a 'w' */

                /*
                 * Extract & Copy DTMF part
                 */
                dtmf_p = calloc(1, sizeof(*dtmf_p));
                if (dtmf_p) {
                    dtmf_p->length = strlen(str_p);
                    if (dtmf_p->length > CN_MAX_STRING_SIZE - 1) {
                        dtmf_p->length = CN_MAX_STRING_SIZE - 1;
                    }
                    memcpy(dtmf_p->dtmf_string, str_p, dtmf_p->length);
                    dtmf_p->dtmf_string[dtmf_p->length] = 0; // NULL terminate to allow easy debug print....

                    dtmf_p->string_type   = CN_DTMF_STRING_TYPE_ASCII;
                    dtmf_p->duration_time = 0; // Use default duration value
                    dtmf_p->pause_time    = 0; // Use default pause timing value

                    CN_LOG_D("dtmf = %s", dtmf_p->dtmf_string);
                }

                *str_p = 0; // Truncate original dialing string to remove DTMF part.
                break;
            }
        }
        CN_LOG_D("number = %s", dial_p->dial.phone_number);
    }

    return dtmf_p;
}

/*
 * Request/response handler for sending DTMF tones after dialing
 */
static request_status_t mddh_handle_dtmf_send(void *data_p, request_record_t* record_p)
{
    request_status_t status = REQUEST_STATUS_ERROR;


    REQUIRE_VALID_RECORD(record_p);

    switch (record_p->state) {
    case REQUEST_STATE_REQUEST: {
        cn_request_dtmf_string_t* dtmf_p      = (cn_request_dtmf_string_t*) data_p;
        mal_call_dtmf_info        mal_dtmf;
        cn_uint8_t                mal_call_id = (cn_uint8_t)(record_p->client_id & 0xFF);
        cn_sint32_t               rc;

        CN_ASSERT(dtmf_p);
        CN_ASSERT(dtmf_p->string_type == CN_DTMF_STRING_TYPE_ASCII);

        CN_LOG_D("request: call_id = %d, mid = %d, dtmf = %s", record_p->client_id,
                                                               (int)mal_call_id,
                                                               dtmf_p->dtmf_string);

        mal_dtmf.string_type        = UTF8_STRING;
        mal_dtmf.length             = dtmf_p->length;
        mal_dtmf.dtmf_string        = dtmf_p->dtmf_string;
        mal_dtmf.dtmf_duration_time = dtmf_p->duration_time;
        mal_dtmf.dtmf_pause_time    = dtmf_p->pause_time;

        rc = mal_call_request_dtmf_send(&mal_call_id, &mal_dtmf, request_record_get_modem_tag(record_p));
        if (rc == MAL_SUCCESS) {
            record_p->state = REQUEST_STATE_RESPONSE;
            status = REQUEST_STATUS_PENDING;
        } else {
            CN_LOG_E("%s mal_call_request_dtmf_send returned %d", __func__, rc);
        }
        break;
    }
    case REQUEST_STATE_RESPONSE: {
        CN_LOG_D("response: call_id = %d", record_p->client_id);
        status = REQUEST_STATUS_DONE;
        break;
    }
    default:
        CN_LOG_E("Invalid state: %d", record_p->state);
        break;
    }

    return status;
}

void mddh_dtmf_handle_call_state_change(cn_sint32_t call_id, cn_call_state_t state)
{

    CN_LOG_D("call_id = %d, state = %d", call_id, state);

    /*
     * Only take action if call goes to active or idle state
     */
    if (state == CN_CALL_STATE_ACTIVE || state == CN_CALL_STATE_IDLE) {
        /*
         * Fetch dtmf instance associated with call_id (if any)
         */
        cn_request_dtmf_string_t* dtmf = mddh_fetch_call_id_associated_dtmf(call_id);
        if (dtmf && state == CN_CALL_STATE_ACTIVE) {
            /*
             * If there is a DTMF instance and the call went active --> time to play DTMF!
             */
            request_status_t  status = REQUEST_STATUS_ERROR;
            request_record_t* record = request_record_create(CN_REQUEST_UNKNOWN, 0, (int)call_id);
            if (record) {
                record->request_handler_p = mddh_handle_dtmf_send;
                status = mddh_handle_dtmf_send((void *)dtmf, record); // call_id passed as client_id in record
                if (REQUEST_STATUS_PENDING != status) {
                    request_record_free(record);
                }
            }
        }
        free(dtmf);
    }
}

#endif /* USE_MAL_CS */

