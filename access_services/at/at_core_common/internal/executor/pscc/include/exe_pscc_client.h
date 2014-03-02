/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef EXE_PSCC_CLIENT_H
#define EXE_PSCC_CLIENT_H 1

#include "atc_log.h"
#include "atc_selector.h"
#include "exe_request_record.h"
#include "mpl_config.h"/* libmpl */
#include "pscc_msg.h"
#include "sterc_msg.h"

typedef struct psccclient_s psccclient_t;

typedef enum {
    EXE_PSCC_ACTION_UNDEFINED,
    EXE_PSCC_ACTION_DATA_LIST_CGACT,
    EXE_PSCC_ACTION_DATA_LIST_AUTH,
    EXE_PSCC_ACTION_DATA_LIST_CGDCONT,
    EXE_PSCC_ACTION_DATA_LIST_NEG_QOS,
    EXE_PSCC_ACTION_MODIFY_QOS,
    EXE_PSCC_ACTION_DATA_LIST_PDP_ADDRESS,
    EXE_PSCC_ACTION_UNSOLICITED,
} exe_pscc_action_t;


typedef struct psccclient_record_data_s {
    psccclient_t                *psccclient_p;
    mpl_msg_event_t             *pscc_event_p;
    mpl_msg_resp_t              *pscc_response_p;
    exe_pscc_action_t            action;
    int                          fault_cause; /* this is set when receiving disconnected events*/
    int                          gprs_status;
    exe_cgact_set_request_t     *cgact_p; /* list with cids to connect or disconnect with cgact */
    exe_cgcmod_set_request_t    *cgcmod_p; /* list with cids to modify using cgcmod */
    exe_cgpaddr_cid_t           *cgpaddr_p; /* list with cids to get pdp addresses for. */
    int                         *connection_ids_data_list_p;
    int                          connection_ids_length;
    int                          connection_ids_current_index;
    int                          cid;
    exe_cgact_async_mode_t       async_mode;
} psccclient_record_data_t;

void *psccclient_open_session(void);
void psccclient_close_session(void *service_p);

/* Functions needed for request implementation */
bool psccclient_send_request(exe_request_record_t *record_p, pscc_msg_t *pscc_request_p);

bool psccclient_set_pscc_event_p(exe_request_record_t *record_p, mpl_msg_event_t *event_p);
exe_pscc_action_t psccclient_get_action(exe_request_record_t *record_p);
mpl_msg_resp_t *psccclient_get_pscc_response_p(exe_request_record_t *record_p);


#endif

