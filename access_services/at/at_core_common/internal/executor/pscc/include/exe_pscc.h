/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef EXE_PSCC_H
#define EXE_PSCC_H 1

/* Add client specific requester states here.
 * Do use client specific ids.
 */
#define EXE_STATE_SETUP_DATA_CALL_CREATE             10
#define EXE_STATE_SETUP_DATA_CALL_SET                11
#define EXE_STATE_SETUP_DATA_CALL_CONNECT            12
#define EXE_STATE_SETUP_DATA_CALL_CONNECTING         13
#define EXE_STATE_SETUP_DATA_CALL_CONNECTED          14
#define EXE_STATE_SETUP_DATA_CALL_GET                15
#define EXE_STATE_SETUP_DATA_CALL_DISCONNECTED       16
#define EXE_STATE_SETUP_DATA_CALL_DESTROY            17
#define EXE_STATE_SETUP_DATA_CALL_WAIT_RECONNECT     18
#define EXE_STATE_SETUP_DATA_CALL_RECONNECT          19
#define EXE_STATE_DEACTIVATE_DATA_CALL_CONNECTING    20
#define EXE_STATE_DEACTIVATE_DATA_CALL_DISCONNECT    21
#define EXE_STATE_DEACTIVATE_DATA_CALL_DISCONNECTING 22
#define EXE_STATE_DEACTIVATE_DATA_CALL_DISCONNECTED  23
#define EXE_STATE_DEACTIVATE_DATA_CALL_DESTROY       24
#define EXE_STATE_QUERY_CONNECTION                   25
#define EXE_STATE_STERC_CREATE                       26
#define EXE_STATE_STERC_DESTROY                      27
#define EXE_STATE_GPRS_GET_STATE                     30
#define EXE_STATE_GPRS_ATTACH                        31
#define EXE_STATE_GPRS_DETACH                        32
#define EXE_STATE_GPRS_ATTACHING                     33
#define EXE_STATE_GPRS_DETACHING                     34
#define EXE_STATE_DISCONNECTED_FAILURE_CAUSE_DESTROY 35
#define EXE_STATE_LIST_CONNECTIONS_REQUEST           40
#define EXE_STATE_LIST_CONNECTIONS_GET               41
#define EXE_STATE_MODIFY_DATA_CALL_REQUEST           45
#define EXE_STATE_MODIFY_DATA_CALL_RESPONSE          46
#define EXE_STATE_SEND_DATA_UPLINK_SET_SIZE          50
#define EXE_STATE_SEND_DATA_UPLINK_SEND_DATA         51
#define EXE_STATE_DUN_PSCC_CREATE                    55
#define EXE_STATE_DUN_PSCC_SET                       56
#define EXE_STATE_DUN_PSCC_DESTROY                   57
#define EXE_STATE_DUN_PSCC_DESTROYED                 58

typedef enum {
    EXE_PSCC_SUCCESS,
    EXE_PSCC_SUCCESS_TERMINATE,
    EXE_PSCC_FAILURE,
    EXE_PSCC_FAILURE_DESTROY
} exe_pscc_result_t;

#endif
