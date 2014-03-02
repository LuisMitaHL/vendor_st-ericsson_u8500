/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef EXE_REQUEST_RECORD_P_H
#define EXE_REQUEST_RECORD_P_H 1

#include "stdbool.h"


/* Private definition of request record struct. Only to be used by
 * exe_request_record.c and exe_requests_xxx.c.
 */
struct exe_request_record_s {
    unsigned int                        id;                 /* Request record id */
    int                                 ref_count;          /* Reference counter */
    exe_t                              *exe_p;              /* Pointer to the executor object. */
    exe_request_id_t                    request_id;         /* Request ID. */
    exe_request_t                       request;            /* Function pointer to the request handler. */
    int                                 state;              /* Execution state of the record. */
    int                                 client_result_code; /* Client result code */
    void                               *request_data_p;     /* In parameters from the caller, out param when synchronuous */
    void                               *caller_data_p;      /* Client tag from caller */
    void                               *response_data_p;    /* Response data from service */
    void                               *client_data_p;      /* Client specific data needed when next service response is received */
    bool                                abort;              /* Abort the execution of the request if set. */
};

#endif
