/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * Description: Private definitions of types used in simpbd
 *
 * Author:  Dag Gullberg <dag.xd.gullberg@stericsson.com>
 *          Mats Bengtsson <mats.m.bengtsson@stericsson.com>
 */

#ifndef _INCLUSION_GUARD_SIMPBD_P_H_
#define _INCLUSION_GUARD_SIMPBD_P_H_

/* Private simpb record type
 * This is a structure that persist during an entire session
 * and is unique to each client and is initiated via simpbd_startup().
 */

#include "sim.h"
#include "simpbd_common.h"

#define TRUE 1
#define FALSE 0

typedef enum {
    SIMPBD_CLIENT_UNKNOWN   = 0x00,
    SIMPBD_CLIENT_REQUEST   = 0x01,
    SIMPBD_CLIENT_EVENT     = 0x02,
    SIMPBD_CLIENT_ALL       = SIMPBD_CLIENT_REQUEST | SIMPBD_CLIENT_EVENT,
} simpbd_client_type_t;

struct simpbd_client_s {
    simpbd_client_type_t        type;        /* Client type */
    int                         instance;
    int                         client;
    int                         fd_sim;
    ste_sim_t                  *ste_sim_p;
    ste_simpb_phonebook_type_t  selected_phonebook;
    ste_simpb_phonebook_type_t  client_selected_phonebook;  /* used to check if STE_SIMPB_PHONEBOOK_TYPE_DEFAULT selected */
    uint8_t                     supported_phonebooks;  /* binary map*/
    /*    uint8_t                     unsolicited_pbr_slice; */
    sim_app_type_t              app_type;
    sim_card_type_t             card_type;
    uint8_t                     pbc_sync_flag;
    ste_simpb_status_t          state;
};

/* Private simpb record type */
struct simpbd_record_s {
    struct simpbd_client_s     *simpbd_client_p;
    uint8_t                     internal_handling;  /* this flag is used when internal handling is using the normal handler */
    simpbd_request_t            request;
    void                       *request_data_p;  /* caller request data */
    void                       *caller_data_p;   /* caller client tag */
    simpbd_request_id_t         simpbd_request_id;
    void                       *private_data_p;
    int                         response_result;
    void                       *response_data_p;
    uintptr_t                   sim_tag;
    simpbd_request_state_t      request_state;
    simbpd_request_chain_t      step_in_chain;
    ste_simpb_result_t          result_code;
    ste_simpb_iso_t             error_code;
};

#endif /* _INCLUSION_GUARD_SIMPBD_P_H_ */
