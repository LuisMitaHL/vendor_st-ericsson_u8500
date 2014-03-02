/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * Description: Common include file for code in simpb.
 *
 * Author: Dag Gullberg <dag.xd.gullbermg@stericsson.com>
 */

#ifndef _INCLUSION_GUARD_SIMPBD_COMMON_H_
#define _INCLUSION_GUARD_SIMPBD_COMMON_H_

#include <stdint.h>

#include "simpb.h"

typedef struct simpbd_record_s simpbd_record_t;
typedef struct simpbd_client_s simpbd_client_t;
typedef enum {
    /* request id's refer to requests internal to simpbd */
    STE_SIMPB_INTERNAL_REQUEST_ID_SYNC_EF_PBC = STE_SIMPB_REQUEST_ID_END_OF_LIST,
    STE_SIMPB_INTERNAL_REQUEST_ID_CACHE_ADN_INFO,
    STE_SIMPB_INTERNAL_REQUEST_ID_CACHE_UPDATE_UID,
    STE_SIMPB_INTERNAL_REQUEST_ID_UPDATE_CC,
    STE_SIMPB_INTERNAL_REQUEST_ID_UPDATE_PSC,
    STE_SIMPB_INTERNAL_REQUEST_ID_PBR_CACHE_REFRESH,

    STE_SIMPB_INTERNAL_REQUEST_ID_END_OF_LIST
} simpbd_internal_request_id_t;

/* Request chain definitions */
typedef enum {
    SIMPBD_REQUEST_CHAIN_INITIAL_VALUE,
    SIMPBD_REQUEST_CHAIN_STARTUP_DO_UICC_DETERMINE_APPLICATION,
    SIMPBD_REQUEST_CHAIN_STARTUP_DO_UICC_DETERMINE_SUPPORTED_PHONEBOOK_GLOBAL,
    SIMPBD_REQUEST_CHAIN_STARTUP_DO_UICC_DETERMINE_SUPPORTED_PHONEBOOK_LOCAL,
    SIMPBD_REQUEST_CHAIN_STARTUP_DO_UICC_CACHE_PBR_STRUCTURE,
    SIMPBD_REQUEST_CHAIN_STARTUP_DO_UICC_CACHE_PBR_STRUCTURE_ITERATE,
    SIMPBD_REQUEST_CHAIN_GET_FILE_INFORMATION_NUMBER_OF_RECORDS_IN_PBR,
    SIMPBD_REQUEST_CHAIN_GET_FILE_INFORMATION_NUMBER_OF_RECORDS_IN_ADN,
    SIMPBD_REQUEST_CHAIN_USIM_USED_NUMBER_OF_RECORDS_FOR_ADNS,
    SIMPBD_REQUEST_CHAIN_THE_END,
    SIMPBD_REQUEST_CHAIN_MAX_VALUE,
} simbpd_request_chain_t;

/* Request chain definitions for EF PBC sync */
typedef enum {
    SIMPBD_REQUEST_CHAIN_NULL = 0,
    SIMPBD_REQUEST_CHAIN_GET_APP_INFO,
    SIMPBD_REQUEST_CHAIN_GET_FORMAT,
    SIMPBD_REQUEST_CHAIN_READ_EF_PBC,
    SIMPBD_REQUEST_CHAIN_UPDATE_EF_PBC,
    SIMPBD_REQUEST_CHAIN_READ_EF_CC,
    SIMPBD_REQUEST_CHAIN_UPDATE_EF_CC,
    SIMPBD_REQUEST_CHAIN_END,
} simbpd_request_chain_sync_ef_pbc_t;

/* Request chain definitions for EF ADNs read */
typedef enum {
    SIMPBD_REQUEST_CHAIN_INIT = 0,
    SIMPBD_REQUEST_CHAIN_READ_ADN_FILE_FORMAT,
    SIMPBD_REQUEST_CHAIN_ADN_FILE_ITERATE_READ_RECORD,
    SIMPBD_REQUEST_CHAIN_ADN_FILE_READ_EF_CC,
    SIMPBD_REQUEST_CHAIN_ADN_FILE_READ_EF_PUID,
    SIMPBD_REQUEST_CHAIN_ADN_FILE_READ_EF_PSC,
    SIMPBD_REQUEST_CHAIN_ADN_FILE_READ_UID_FILE_FORMAT,
    SIMPBD_REQUEST_CHAIN_ADN_END,
} simbpd_request_chain_cache_adn_t;

/* Request chain definitions for EF UIDs read
typedef enum {
    SIMPBD_REQUEST_CHAIN_UID_INIT = 0,
    SIMPBD_REQUEST_CHAIN_GET_UID_FORMAT,
    SIMPBD_REQUEST_CHAIN_READ_UID_FILE_FORMAT,
    SIMPBD_REQUEST_CHAIN_UID_STOP,
} simbpd_request_chain_cache_uid_t;
*/

/* Request chain definitions for EF UIDs update */
typedef enum {
    SIMPBD_REQUEST_CHAIN_UPDATE_UID_INIT = 0,
    SIMPBD_REQUEST_CHAIN_UPDATE_UID_ADN_READ_RECORD,
    SIMPBD_REQUEST_CHAIN_UPDATE_UID_READ_EF_CC,
    SIMPBD_REQUEST_CHAIN_UPDATE_UID_UPDATE_EF_CC,
    SIMPBD_REQUEST_CHAIN_UPDATE_UID_READ_PUID,
    SIMPBD_REQUEST_CHAIN_UPDATE_UID_UPDATE_UID_RECORD,
    SIMPBD_REQUEST_CHAIN_UPDATE_UID_UPDATE_PUID,
    SIMPBD_REQUEST_CHAIN_UPDATE_UID_GET_UID_FILE_FORMAT,
    SIMPBD_REQUEST_CHAIN_UPDATE_UID_READ_RECORD,
    SIMPBD_REQUEST_CHAIN_UPDATE_UID_END,
} simbpd_request_chain_update_uid_t;

/* Request chain definitions for EF PSC read */
typedef enum {
    SIMPBD_REQUEST_CHAIN_PSC_UPDATE_START = 0,
    SIMPBD_REQUEST_CHAIN_READ_PSC,
    SIMPBD_REQUEST_CHAIN_UPDATE_PSC,
    SIMPBD_REQUEST_CHAIN_PSC_UPDATE_STOP,
} simbpd_request_chain_update_psc_t;

/* Request chain definitions for EF CC read
typedef enum {
    SIMPBD_REQUEST_CHAIN_CC_UPDATE_START = 0,
    SIMPBD_REQUEST_CHAIN_READ_CC,
    SIMPBD_REQUEST_CHAIN_UPDATE_CC,
    SIMPBD_REQUEST_CHAIN_CC_UPDATE_STOP,
} simbpd_request_chain_update_cc_t;
*/

/* Function pointer used for handling requests simd */
typedef ste_simpb_result_t (*simpbd_request_t)(simpbd_record_t *simpbd_record_p);

typedef enum {
    SIMPBD_REQUEST_STATE_FREE,
    SIMPBD_REQUEST_STATE_REQUEST,
    SIMPBD_REQUEST_STATE_STEP_CLEAR_PBR_CACHE,
    SIMPBD_REQUEST_STATE_STEP,
    SIMPBD_REQUEST_STATE_RESPONSE,
    SIMPBD_REQUEST_UNKNOWN
} simpbd_request_state_t;

typedef struct {
    uint8_t          request_id;
    simpbd_request_t request;
} simpb_request_table_t;

/* Request function parameter types */

typedef struct {
    uint8_t file_id;
    uint8_t instance;
    uint8_t slice;
    uint8_t record_id;
    uint8_t p2;
    uint8_t length;
} simpbd_request_read_t;

typedef struct {
    uint8_t file_id;
    uint8_t instance;
    uint8_t slice;
    uint8_t record_id;
    uint8_t p2;
    uint8_t length;
    uint16_t data_size;
    uint8_t data[1];
} simpbd_request_update_t;

typedef struct {
    ste_simpb_phonebook_type_t selected_phonebook;
} simpbd_request_phonebook_select_t;

typedef struct {
    uint8_t instance;
    uint8_t slice;
    ste_simpb_file_type_t file_id;
} simpbd_request_file_information_get_t;


/* libsimpb <-> simpbd message structure */
/* The simpb_message_t structure is used for both request, response and event messages */
typedef struct {
    ste_simpb_cause_t       cause;          /* Cause of transmission */
    uintptr_t               client_tag;     /* the client_tag is returned in the response. Has value zero for events messages.  */
    simpbd_request_id_t     request_id;     /* request id. Determines which payload struct to use, if any.                    */
    ste_simpb_result_t      result;         /* result code. given in the response message. Undefined for event messages */
    uint32_t                payload_size;   /* size of message payload.                                                         */
    uint8_t                 payload[1];     /* start of payload (if any).                                                       */
} simpb_message_t;

#define SIMPB_MESSAGE_HEADER_SIZE (sizeof(simpb_message_t))

#endif /* _INCLUSION_GUARD_SIMPBD_COMMON_H_ */
