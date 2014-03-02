/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <alloca.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

#include "atc_log.h"
#include "atc_selector.h"
#include "exe_p.h"
#include "exe.h"
#include "exe_internal.h"
#include "exe_start.h"
#include "exe_request_record.h"
#include "exe_requests.h"
#include "exe_requests_table.h"
#include "exe_services_table.h"

#ifdef EXE_USE_COPS_SERVICE
#include "exe_cops_client.h"
#include "exe_cops_requests.h"
#endif

#ifdef EXE_USE_CN_SERVICE
#include "exe_cn_client.h"
#include "exe_cn_requests.h"
#endif

#ifdef EXE_USE_SMS_SERVICE
#include "exe_sms_client.h"
#include "exe_sms_requests.h"
#endif

#ifdef EXE_USE_PSCC_SERVICE
#include "exe_pscc_client.h"
#include "exe_pscc_requests.h"
#endif

#ifdef EXE_USE_SIM_SERVICE
#include "exe_sim_client.h"
#include "exe_sim_requests.h"
#endif

#ifdef EXE_USE_SIMPB_SERVICE
#include "exe_simpb_client.h"
#endif

#if defined(EXE_USE_ATC_CUSTOMER_EXTENSIONS) && defined(EXE_USE_SIMPB_SERVICE)
#include "exe_extended_simpb_requests.h"
#endif


/********************************************************************
 * Static variables
 ********************************************************************
 */
static exe_t *s_exe_p = NULL;

static exe_final_response_t    s_callback_final_response = NULL;
static exe_event_response_t    s_callback_event = NULL;


/********************************************************************
 * Private prototypes
 ********************************************************************
 */
static void exe_init(exe_t *exe_p);
exe_request_t exe_get_request(exe_request_id_t request_id);


/********************************************************************
 * Private methods
 ********************************************************************
 */
/*
 * exe_init
 *
 * Initializes the executor object.
 *
 *  exe_p             [in]      Pointer to the executer object.
 *
 *  No return values.
 */
static void exe_init(exe_t *exe_p)
{
    if (NULL == exe_p) {
        ATC_LOG_E("error! exe_p is NULL");
        return;
    }

    /* Initialize client contexts */
#ifdef EXE_USE_COPS_SERVICE
    exe_set_client_context(exe_p, EXE_SERVICE_COPS, NULL);
#endif
#ifdef EXE_USE_CN_SERVICE
    exe_set_client_context(exe_p, EXE_SERVICE_CN, NULL);
    exe_set_dtmf_duration(exe_p, 0L);
#endif
#ifdef EXE_USE_PSCC_SERVICE
    exe_set_client_context(exe_p, EXE_SERVICE_PSCC, NULL);
#endif
#ifdef EXE_USE_SIM_SERVICE
    exe_set_client_context(exe_p, EXE_SERVICE_SIM, NULL);
#endif
#ifdef EXE_USE_SMS_SERVICE
    exe_set_client_context(exe_p, EXE_SERVICE_SMS, NULL);
#endif
#ifdef EXE_USE_SIMPB_SERVICE
    exe_set_client_context(exe_p, EXE_SERVICE_SIMPB, NULL);
#endif
    exe_last_fail_cause_init(&exe_p->last_fail);
}

/*
 * exe_get_request
 *
 * Finds the request/function that is associated with the specified request id.
 *
 *  request_id  [in]    Id of the request to find.
 *
 *  Returns:    Function to pointer to the request.
 *              NULL if no request was found.
 */
exe_request_t exe_get_request(exe_request_id_t request_id)
{
    unsigned int i;

    for (i = 0; i < (sizeof(exe_request_table) / sizeof(exe_request_table_t)); i++) {
        if (request_id == exe_request_table[i].request_id) {
            return exe_request_table[i].request;
        }
    }

    return NULL;
}


/********************************************************************
 * Internal methods
 ********************************************************************
 */
#ifdef EXE_USE_COPS_SERVICE
copsclient_t *exe_get_copsclient(exe_t *exe_p)
{
    if (NULL == exe_p) {
        ATC_LOG_E("error! exe_p is NULL");
        return NULL;
    }

    return exe_p->copsclient_p;
}
#endif


#ifdef EXE_USE_CN_SERVICE
cn_context_t *exe_get_cn_client(exe_t *exe_p)
{
    if (NULL == exe_p) {
        ATC_LOG_E("error! exe_p is NULL");
        return NULL;
    }

    return exe_p->cn_context_p;
}
#endif

#ifdef EXE_USE_PSCC_SERVICE
psccclient_t *exe_get_psccclient(exe_t *exe_p)
{
    if (NULL == exe_p) {
        ATC_LOG_E("error! exe_p is NULL");
        return NULL;
    }

    return exe_p->psccclient_p;
}
#endif

#ifdef EXE_USE_SMS_SERVICE
smsclient_t *exe_get_smsclient(exe_t *exe_p)
{
    if (NULL == exe_p) {
        ATC_LOG_E("error! exe_p is NULL");
        return NULL;
    }

    return exe_p->smsclient_p;
}
#endif

#ifdef EXE_USE_SIM_SERVICE
sim_client_t *exe_get_sim_client(exe_t *exe_p)
{
    if (NULL == exe_p) {
        ATC_LOG_E("error! exe_p is NULL");
        return NULL;
    }

    return exe_p->simclient_p;
}
#endif

#ifdef EXE_USE_SIMPB_SERVICE
simpb_client_t *exe_get_simpb_client(exe_t *exe_p)
{
    if (NULL == exe_p) {
        ATC_LOG_E("error! exe_p is NULL");
        return NULL;
    }

    return exe_p->simpbclient_p;
}
#endif

/*
 * exe_request_complete
 *
 * Calls the registered callback with the result and response data
 * for the request together with the caller data. The request record is
 * then destroyed.
 *
 *  request_record_p    [in]    Pointer to the request record.
 *  result              [in]    Result of the execution.
 *  response_p          [in]    Pointer to the response data.
 *
 *  No return values.
 */
void exe_request_complete(exe_request_record_t *request_record_p,
                          exe_request_result_t result, void *response_p)
{
    if (NULL != s_callback_final_response) {
        s_callback_final_response(request_record_get_caller_data(request_record_p),
                                  result, response_p);
        request_record_destroy(request_record_p);
    }
}

/*
 * exe_event
 *
 * Calls the registered callback with the event data.
 *
 *  event       [in]    Id of the event.
 *  response_p  [in]    Pointer to the event data.
 *
 *  No return values.
 */
void exe_event(exe_event_t event, void *event_data_p)
{
    if (s_callback_event != NULL) {
        /* Null check to handle events generated at mal initialization*/
        s_callback_event(event, event_data_p);
    }
}


/********************************************************************
 * Public methods
 ********************************************************************
 */

/*
 * exe_create
 *
 * Creates the executor object and creates all clients that
 * opens all necessary connections.
 *
 *  Returns:    Function to the executor object.
 *              NULL if no object could be created.
 */
exe_t *exe_create(void)
{
    int ret;
    int fd_mfl;
    exe_t *exe_p = NULL;
    ATC_LOG_I("exe_create");
    exe_p = calloc(1, sizeof(*exe_p));

    if (NULL == exe_p) {
        ATC_LOG_E("Memory allocation of %lu bytes failed!", sizeof(*exe_p));
        goto error;
    }

    exe_init(exe_p);

    /* The following call returns only when all critical services have
     * been connected.
     */
    if (!exe_start_init(exe_p)) {
        ATC_LOG_E("exe_start_init failed!");
        goto error;
    }

    /* TODO Selector registration done in client. Move to here */
    /* if exe shall control any restarts or similar. */
    s_exe_p = exe_p;
    return exe_p;

error:
    return NULL;
}


/*
 * exe_destroy
 *
 * Deallocates all resources and destroys the executor object.
 *
 *  exe_p   [in] Pointer to the executer object.
 */
void exe_destroy(exe_t *exe_p)
{
    if (NULL == exe_p) {
        ATC_LOG_E("error! exe_p is NULL");
        return;
    }

    /* Clean up all requests */
    request_record_destroy_all();

#ifdef EXE_USE_CN_SERVICE
    cn_client_close_session(exe_p->cn_context_p);
#endif
#ifdef EXE_USE_COPS_SERVICE
    copsclient_close_session(exe_p->copsclient_p);
#endif
#ifdef EXE_USE_PSCC_SERVICE
    psccclient_close_session(exe_p->psccclient_p);
#endif
#ifdef EXE_USE_SIM_SERVICE
    sim_client_close_session(exe_p->simclient_p);
#endif
#ifdef EXE_USE_SMS_SERVICE
    smsclient_close_session(exe_p->smsclient_p);
#endif
#ifdef EXE_USE_SIMPB_SERVICE
    simpb_client_close_session(exe_p->simpbclient_p);
#endif
    free(exe_p);
    exe_p = NULL;
    s_exe_p = NULL;
}


/*
 * exe_request
 *
 * This function creates a record that holds the data needed to execute the request,
 * it then calls the actual request identified by the request id. The called request
 * will be synchronous or asynchronous; the record is destroyed here if the request
 * returns a final result (i.e. not pending/synchronous).
 *
 *  exe_p             [in]      Pointer to the executer object.
 *  request_id        [in]      Id of the request to be executed.
 *  request_data_p    [in/out]  Pointer to the input parameters.
 *                              Holds the response data for synchronous calls.
 *  caller_data_p     [in]      Pointer to data that shall be returned to the
 *                              caller when terminating the request.
 *  request_handle_p  [out]     Pointer to a id of this request.
 *
 *  Returns:    EXE_NOT_SUPPORTED   The request_id is not supported.
 *              EXE_PENDING         The request has been successfully received, response
 *                                  will be sent to registered callback (asynchronous).
 *              EXE_SUCCESS         The request has been successfully handled
 *                                  and has terminated (synchronous).
 *              EXE_FAILURE         The request handling failed.
 */
exe_request_result_t exe_request(exe_t *exe_p,
                                 exe_request_id_t request_id,
                                 void *request_data_p,
                                 void *caller_data_p,
                                 int *request_handle_p)
{
    exe_request_t request = exe_get_request(request_id);
    exe_request_record_t *request_record_p = NULL;
    exe_request_result_t result = EXE_FAILURE;

    if (NULL == request) {
        /* The request could not be found and is thus not supported. */
        result = EXE_NOT_SUPPORTED;
        goto exit;
    }

    request_record_p = request_record_create(request_id, request, exe_p, request_data_p, caller_data_p, request_handle_p);

    if (NULL != request_record_p) {
        result = request(request_record_p);

        if (EXE_PENDING != result) {
            /* The request terminated, destroy the record. */
            request_record_destroy(request_record_p);
        }
    }

exit:

    return result;
}


/*
 * exe_request_abort
 *
 * This function calls a request being executed and requests it to abort.
 * The record is destroyed here if the request returns a final result
 * (i.e. not pending/synchronous).
 *
 *  exe_p             [in]      Pointer to the executer object.
 *  request_handle    [in]      Id of the request to abort.
 *
 *  Returns:    EXE_NOT_SUPPORTED   The request_handle is not supported.
 *              EXE_PENDING         The request has been successfully received, response
 *                                  will be sent to registered callback (asynchronous).
 *              EXE_SUCCESS         The request has been successfully handled
 *                                  and has terminated (synchronous).
 *              EXE_FAILURE         The request handling failed.
 */
exe_request_result_t exe_request_abort(exe_t *exe_p, int request_handle)
{
    exe_request_result_t result = EXE_FAILURE;
    exe_request_record_t *request_record_p = request_record_get_record_from_client_tag(request_handle);
    exe_request_t request;

    if (NULL == request_record_p) {
        /* The record could not be found and is thus not supported. */
        result = EXE_NOT_SUPPORTED;
        goto exit;
    }

    /* Set the request record to be aborted. */
    request_record_set_abort(request_record_p);

    request = request_record_get_request(request_record_p);

    if (NULL != request) {
        result = request(request_record_p);

        if (EXE_PENDING != result) {
            /* The request terminated, destroy the record. */
            request_record_destroy(request_record_p);
        }
    }

exit:
    return result;
}


/*
 * exe_register_callback_final_response
 *
 * Register a function to be called when a request has terminated.
 *
 *  exe_p                   [in]    Pointer to the executer object.
 *  final_response_callback [in]    Function to register.
 *
 *  No return values.
 */
void exe_register_callback_final_response(exe_t *exe_p, exe_final_response_t final_response_callback)
{
    s_callback_final_response = final_response_callback;
}


/*
 * exe_register_callback_event
 *
 * Register a function to be called when an unsolicited event occurs.
 *
 *  exe_p                   [in]    Pointer to the executer object.
 *  final_response_callback [in]    Function to register.
 *
 *  No return values.
 */
void exe_register_callback_event(exe_t *exe_p, exe_event_response_t event_callback)
{
    s_callback_event = event_callback;
}

/********************************************************************
 * Public, executor internal methods
 ********************************************************************
 */

/*
 * exe_get_exe
 *
 * Returns a pointer to the executor object.
 *
 */
exe_t *exe_get_exe(void)
{
    return s_exe_p;
}

/*
 * exe_set_last_fail_cause
 *
 * Stores the latest fail cause for network/call that has been detected by the
 * executor clients.
 *
 *  exe_p               [in]    Pointer to the executer object.
 *  last_fail_cause_p   [in]    Pointer to the last fail cause data struct.
 *
 *  No return values.
 */
void exe_set_last_fail_cause(exe_t *exe_p, void *last_fail_cause_p)
{
    if (NULL == exe_p) {
        ATC_LOG_E("error! exe_p is NULL");
        return;
    }

    exe_last_fail_cause_set(last_fail_cause_p, &exe_p->last_fail);
}

/*
 * exe_get_last_fail_cause
 *
 * Reads out a text string that describes the latest fail cause stored
 * with exe_set_last_fail_cause.
 */
void exe_get_last_fail_cause(exe_t *exe_p, char *error_report_p)
{
    if (NULL == exe_p) {
        ATC_LOG_E("error! exe_p is NULL");
        return;
    }

    exe_last_fail_cause_get(&exe_p->last_fail, error_report_p);
}


/*
 * exe_get_request_service
 *
 * Finds the service responsible for servicing a specific request_id.
 *
 *  request_id  [in]    Id of the request for which the service is to be returned.
 *
 *  Returns:    Id of service.
 */
exe_service_t exe_get_request_service(exe_request_id_t request_id)
{
    unsigned int i;

    for (i = 0; i < (sizeof(exe_request_table) / sizeof(exe_request_table_t)); i++) {
        if (request_id == exe_request_table[i].request_id) {
            return exe_request_table[i].service;
        }
    }

    return EXE_SERVICE_NONE;
}


/*
 * exe_get_request_description
 *
 * Finds a string description of the request corresponding to the specified request id.
 *
 *  request_id  [in]    Id of the string description to find.
 *
 *  Returns:    Pointer to string description for the request.
 *              NULL if no string description was found.
 */
const char *exe_get_request_description(exe_request_id_t request_id)
{
    unsigned int i;

    for (i = 0; i < (sizeof(exe_request_table) / sizeof(exe_request_table_t)); i++) {
        if (request_id == exe_request_table[i].request_id) {
            return exe_request_table[i].description;
        }
    }

    return NULL;
}


/*
 * exe_set_client_context
 *
 * Sets the client-side context pointer for a service.
 *
 * If the client-side context pointer is being reset, then purge
 * all pending requests that are associated with the service.
 *
 *  exe_p       [in]  Pointer to the executer object.
 *  service     [in]  Id of the service.
 *  context_p   [in]  Pointer to client-side context.
 *
 *  No return values.
 */
void exe_set_client_context(exe_t *exe_p, exe_service_t service, void *context_p)
{
    void *old_context_p;

    if (NULL == exe_p) {
        ATC_LOG_E("error! exe_p is NULL");
        return;
    }

    if (0 > service || EXE_SERVICE_LAST < service) {
        ATC_LOG_E("service %d is out of range!", service);
        return;
    }

    ATC_LOG_I("exe_p=%p, service=%d(\"%s\"), context_p=%p",
              exe_p, service, exe_service_table[service].description, context_p);

    switch (service) {
    case EXE_SERVICE_EXE:
        old_context_p = NULL;
        break;

#ifdef EXE_USE_AUDIO_SERVICE
    case EXE_SERVICE_AUDIO:
        old_context_p = NULL;
        break;
#endif

#ifdef EXE_USE_CN_SERVICE
    case EXE_SERVICE_CN:
        old_context_p = exe_p->cn_context_p;
        exe_p->cn_context_p = (cn_context_t *)context_p;
        break;
#endif

#ifdef EXE_USE_COPS_SERVICE
    case EXE_SERVICE_COPS:
        old_context_p = exe_p->copsclient_p;
        exe_p->copsclient_p = (copsclient_t *)context_p;
        break;
#endif

#ifdef EXE_USE_CSPSA_SERVICE
    case EXE_SERVICE_CSPSA:
        old_context_p = NULL;
        break;
#endif

#ifdef EXE_USE_PSCC_SERVICE
    case EXE_SERVICE_PSCC:
        old_context_p = exe_p->psccclient_p;
        exe_p->psccclient_p = (psccclient_t *)context_p;
        break;
#endif

#ifdef EXE_USE_SIM_SERVICE
    case EXE_SERVICE_SIM:
        old_context_p = exe_p->simclient_p;
        exe_p->simclient_p = (sim_client_t *)context_p;
        break;
#endif

#ifdef EXE_USE_SIMPB_SERVICE
    case EXE_SERVICE_SIMPB:
        old_context_p = exe_p->simpbclient_p;
        exe_p->simpbclient_p = (simpb_client_t *)context_p;
        break;
#endif

#ifdef EXE_USE_SMS_SERVICE
    case EXE_SERVICE_SMS:
        old_context_p = exe_p->smsclient_p;
        exe_p->smsclient_p = (smsclient_t *)context_p;
        break;
#endif

    default:
        ATC_LOG_E("No matching service, %d!", service);
        return;
    }

    if (NULL == context_p && NULL != old_context_p) {
        int requests;

        /* Purge all pending requests for this service */
        requests = request_record_service_purge(service);
        ATC_LOG_I("Purged %d requests for service %d \"%s\".",
                  requests, service, exe_service_table[service].description);
    }

    return;
}


#if defined(EXE_USE_CN_SERVICE)
exe_vtd_t exe_get_dtmf_duration(exe_t *exe_p)
{
    if (NULL != exe_p) {
        return exe_p->dtmf_duration;
    }

    return 0;
}

void exe_set_dtmf_duration(exe_t *exe_p, exe_vtd_t dtmf_duration)
{
    if (NULL != exe_p) {
        exe_p->dtmf_duration = dtmf_duration;
    }
}
#endif

