/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * Description: Implementation of request handlers in simpbd
 *
 * Author:  Dag Gullberg <dag.xd.gullberg@stericsson.com>
 *          Mats Bengtsson <mats.m.bengtsson@stericsson.com>
 */

#include <string.h>

#include "log.h"
#include "fdmon.h"

#include "simpb.h"
#include "simpbd.h"
#include "simpbd_common.h"
#include "simpbd_p.h"
#include "simpbd_handlers.h"
#include "simpbd_handler_error.h"
#include "simpbd_record.h"
#include "simpbd_cache.h"

/* constant defines*/
#define SIMPB_ADN_REC_PH_NO_LEN 14

/* local typedefs */
typedef enum {
    SIMPBD_SIM_READ_STATUS_COMPLETE,
    SIMPBD_SIM_READ_STATUS_ERROR
} simpbd_sim_read_status_t;

typedef struct {
    uint16_t file_id;
    char    *file_path;
    uint16_t num_records;
    uint8_t  record_len;
    uint16_t index;
    uint16_t num_udpated;
} simpbd_sync_ef_pbc_t;

typedef struct {
    uint16_t file_id;
    char    *file_path;
    uint16_t num_records;
    uint8_t  record_len;
    uint16_t index;
    uint16_t num_udpated;
} simpbd_update_ef_uid_t;

extern uint16_t simpb_puid_value;
extern uint16_t simpb_cc_value;
extern uint32_t simpb_psc_value;
extern uint16_t simpb_p_cc_value;

ste_simpb_occupied_record *simpbd_cache_occupied_record = NULL;


/* utility prototypes  */
void simpbd_translate_sim_request_status_to_simpbd_code(simpbd_record_t *simpbd_record_p, uicc_request_status_t result);
static ste_simpb_sim_state_t simpbd_translate_sim_state_to_simpbd_state(sim_state_t sim_state);
void simpbd_complete_request(simpbd_record_t *simpbd_record_p, ste_simpb_result_t result, void *data_p, size_t data_size);
void simpbd_send_unsolicited(ste_simpb_cause_t cause, void *data_p, size_t data_size);
void simpbd_unsolicited_pbr_cache_request(simpbd_client_t *simpbd_client_p);
void simpbd_unsolicited_pbr_cache_handler(simpbd_client_t *simpbd_client_p, void *data_p);
static uint8_t simpbd_get_filedescriptor_from_fcp(void *data_p, uint16_t length, uint16_t *index_p);
static uint8_t simpbd_ef_adn_record_occupied(uint8_t record_len, ste_uicc_sim_file_read_record_response_t *rec_data);
static char *simpbd_cache_get_ef_path_phonebook(ste_simpb_phonebook_type_t phonebook, uint8_t app_type);

static void dump_hex(void *data_p, uint16_t length);

/* Callback prototypes */
static int simpbd_sim_cb_read(const int sd, const void *context_p);
static int simpbd_sim_cb_remove(const int sd, const void *context_p);

static void simpbd_sim_cb_receive(int cause,
                                  uintptr_t sim_tag,
                                  void *data_p,
                                  void *user_data_p);

static void on_new_sim_state(simpbd_client_t *simpb_client_p, sim_state_t state);

static void simpbd_ef_pbc_sync(simpbd_client_t *simpbd_client_p);

/* defines */
#define SIMPBD_PBR_UINT8_USIM_LOCAL_PATH {0x3f,0x00,0x7F,0xFF,0x5F,0x3A,0x4F,0x30}
#define SIMPBD_PBR_UINT8_USIM_GLOBAL_PATH {0x3f,0x00,0x7F,0x10,0x5F,0x3A,0x4F,0x30}
#define SIMPBD_PBR_UINT8_USIM_SIZE_OF_PATH 8

static const uint16_t simpbd_ef_cc_id  = 0x4F23;
static const uint16_t simpbd_ef_puid_id  = 0x4F24;
static const uint16_t simpbd_ef_psc_id  = 0x4F22;

static char *simpbd_ef_usim_path = "3F007FFF5F3A";
static char *simpbd_ef_sim_path  = "3F007F105F3A";

#define SIMPBD_GET_FILE_INFORMATION_FCP_FD_TAG_VALUE           0x82    /* this file descriptor tag value */

/* Handler function implementations */

/**
 *  simpbd_handler_startup
 *
 *  This function handles the aspects of starting up the service toward the simd.
 *  It is executed in a chain of several requests to simd, some synchronous and the last
 *  asynchronous.
 *
 *  Create the sim connection and register the closure callback.
 *  Get and register file descriptor used for communication.
 *  Get the current SIM state, and report this information back.
 *
 */
ste_simpb_result_t simpbd_handler_startup(simpbd_record_t *simpbd_record_p)
{
    SIMPB_LOG_D("request_state=%d", simpbd_record_p->request_state);

    switch (simpbd_record_p->request_state) {
    case SIMPBD_REQUEST_STATE_REQUEST: {

        ste_sim_closure_t ste_sim_closure;
        int result;
        int fd;

        /* Create a single threaded representation of the cat/uicc server */
        ste_sim_closure.func = simpbd_sim_cb_receive;
        ste_sim_closure.user_data  = simpbd_record_p->simpbd_client_p;

        simpbd_record_p->simpbd_client_p->ste_sim_p = ste_sim_new_st(&ste_sim_closure);

        if (NULL == simpbd_record_p->simpbd_client_p->ste_sim_p) {
            SIMPB_LOG_E("ste_sim_new, returns NULL!");
            goto error;
        }

        SIMPB_LOG_D("ste_sim_new_st() = %p", simpbd_record_p->simpbd_client_p->ste_sim_p);

        /* Connect to the cat/uicc server. Note that the call is synchronous
         * although there will be a response (ignored) that the disconnect is completed. */
        result = ste_sim_connect((simpbd_record_p->simpbd_client_p)->ste_sim_p, 0);
        SIMPB_LOG_D("ste_sim_connect() = %d", result);

        if (STE_SIM_SUCCESS != result) {
            simpbd_translate_sim_request_status_to_simpbd_code(simpbd_record_p, result);
            SIMPB_LOG_E("ste_sim_connect, result %d", result);
            goto error;
        }

        /* Retrieve the underlying file descriptor */
        fd = ste_sim_fd((simpbd_record_p->simpbd_client_p)->ste_sim_p);

        if (0 > fd) {
            SIMPB_LOG_E("ste_sim_fd(), fd %d", fd);
            goto error;
        }

        (simpbd_record_p->simpbd_client_p)->fd_sim = fd;

        /* Register the file descriptor with the fdmon service */
        result = fdmon_add(fd, simpbd_record_p->simpbd_client_p, &simpbd_sim_cb_read, &simpbd_sim_cb_remove);

        if (0 > result) {
            SIMPB_LOG_E("fdmon_add(%d), result %d", fd, result);
            goto error;
        }

        /* Retrieve the current SIM state.
         * Any future changes to the SIM state will be reported as events */
        result = ste_uicc_sim_get_state((simpbd_record_p->simpbd_client_p)->ste_sim_p,
                                        simpbd_record_get_sim_tag(simpbd_record_p));
        SIMPB_LOG_D("ste_uicc_sim_get_state() = %d", result);

        if (STE_SIM_SUCCESS != result) {
            simpbd_translate_sim_request_status_to_simpbd_code(simpbd_record_p, result);
            SIMPB_LOG_E("ste_uicc_sim_get_state, result %d", result);
            goto error;
        }

        simpbd_record_p->request_state = SIMPBD_REQUEST_STATE_RESPONSE;

        return STE_SIMPB_PENDING;
    }

    case SIMPBD_REQUEST_STATE_RESPONSE: {
        ste_simpb_cb_sim_state_t sim_state;
        ste_uicc_get_sim_state_response_t *response_data_p =
            (ste_uicc_get_sim_state_response_t *)simpbd_record_p->response_data_p;

        if (STE_SIM_SUCCESS != simpbd_record_p->response_result) {
            SIMPB_LOG_E("ste_uicc_sim_get_state, result %d", simpbd_record_p->response_result);
            goto error;
        }

        if (NULL == response_data_p) {
            SIMPB_LOG_E("ste_uicc_sim_get_state, response data is NULL!");
            goto error;
        }

        /* Map SIM state to corresponding SIMPB state */
        sim_state.value = simpbd_translate_sim_state_to_simpbd_state(response_data_p->state);

        simpbd_complete_request(simpbd_record_p, STE_SIMPB_SUCCESS, &sim_state, sizeof(sim_state));
        return STE_SIMPB_SUCCESS;
    }

    default:
        SIMPB_LOG_E("unhandled request_state = %d", simpbd_record_p->request_state);
        goto error;
    }

error:

    simpbd_complete_request(simpbd_record_p, STE_SIMPB_FAILURE, NULL, 0);
    return STE_SIMPB_FAILURE;
}


/**
 *  simpbd_handler_cache_pbr
 *
 *  This function handles determining what type of SIM and reading the PBR structure.
 *  It is executed in a chain of several requests to simd, all asynchronous calls made
 *  to simd being response handled by this function and next step in the chain called.
 *
 *  This handler can only be run when the SIM daemon reports that it is ready.
 *
 *  STEP 1: get app_info to determine type of sim, and thus the type of phone books supported.
 *  STEP 2: Get the PBR structure and cache it.
 *
 */
ste_simpb_result_t simpbd_handler_cache_pbr(simpbd_record_t *simpbd_record_p)
{
    SIMPB_LOG_D("request_state=%d, step_in_chain=%d, response_data_p=%p",
                simpbd_record_p->request_state,
                simpbd_record_p->step_in_chain,
                simpbd_record_p->response_data_p);

    switch (simpbd_record_p->request_state) {
    case SIMPBD_REQUEST_STATE_REQUEST: {
        int result;

        /* clear adn_file_info */
        simpbd_cache_flush_file_info();
        simpbd_send_unsolicited(STE_SIMPB_CAUSE_UNSOLICITED_SIMPB_FILE_IDS, (void *)simpbd_get_adn_file_ids(), sizeof(simpb_file_ids_t));

        /* Get information about the currently active uicc application */
        result = ste_uicc_card_status((simpbd_record_p->simpbd_client_p)->ste_sim_p,
                                      simpbd_record_get_sim_tag(simpbd_record_p));

        SIMPB_LOG_D("ste_uicc_card_status() = %d", result);

        if (UICC_REQUEST_STATUS_OK != result) {
            simpbd_translate_sim_request_status_to_simpbd_code(simpbd_record_p, result);
            SIMPB_LOG_E("ste_uicc_card_status, result %d", result);
            goto error;
        }

        simpbd_record_p->request_state = SIMPBD_REQUEST_STATE_STEP;
        simpbd_record_p->step_in_chain = SIMPBD_REQUEST_CHAIN_STARTUP_DO_UICC_DETERMINE_APPLICATION;

        return STE_SIMPB_PENDING;
    }

    case SIMPBD_REQUEST_STATE_STEP:

        switch (simpbd_record_p->step_in_chain) {
        case SIMPBD_REQUEST_CHAIN_STARTUP_DO_UICC_DETERMINE_APPLICATION: {
            int result;
            ste_uicc_sim_card_status_response_t *sim_card_status_p = NULL;

            /* RESPONSE handling from previous step in chain */
            if (NULL != simpbd_record_p->response_data_p) {
                sim_card_status_p = (ste_uicc_sim_card_status_response_t *)simpbd_record_p->response_data_p;
                simpbd_record_p->simpbd_client_p->card_type = sim_card_status_p->card_type;
                SIMPB_LOG_D(" card_type = %d", sim_card_status_p->card_type);

            } else {
                SIMPB_LOG_E("ste_uicc_card_status, no response data!");
                goto error;
            }

            /* Get information about the currently active uicc application */
            result = ste_uicc_get_app_info((simpbd_record_p->simpbd_client_p)->ste_sim_p,
                                           simpbd_record_get_sim_tag(simpbd_record_p));

            SIMPB_LOG_D("ste_uicc_get_app_info() = %d", result);

            if (UICC_REQUEST_STATUS_OK != result) {
                simpbd_translate_sim_request_status_to_simpbd_code(simpbd_record_p, result);
                SIMPB_LOG_E("ste_uicc_get_app_info, result %d", result);
                goto error;
            }

            if (STE_SIM_SUCCESS != result) {
                SIMPB_LOG_E("simpbd_cache_init %d", result);
                goto error;
            }

            simpbd_record_p->request_state = SIMPBD_REQUEST_STATE_STEP;
            simpbd_record_p->step_in_chain = SIMPBD_REQUEST_CHAIN_STARTUP_DO_UICC_DETERMINE_SUPPORTED_PHONEBOOK_GLOBAL;

            break;
        }

        case SIMPBD_REQUEST_CHAIN_STARTUP_DO_UICC_DETERMINE_SUPPORTED_PHONEBOOK_GLOBAL: {
            uicc_request_status_t result = UICC_REQUEST_STATUS_OK;
            ste_uicc_sim_app_info_response_t *sim_app_info_p = NULL;
            sim_app_type_t app_type = SIM_APP_UNKNOWN;
            sim_card_type_t card_type = simpbd_record_p->simpbd_client_p->card_type;

            /* RESPONSE handling from previous step in chain */
            if (NULL != simpbd_record_p->response_data_p) {
                sim_app_info_p = (ste_uicc_sim_app_info_response_t *)simpbd_record_p->response_data_p;
                app_type = sim_app_info_p->app_type;
                simpbd_record_p->simpbd_client_p->app_type = app_type;
            } else {
                SIMPB_LOG_E("ste_uicc_get_app_info, no response data!");
                goto error;
            }

            /* Handle next request to determine supported phone books */

            if (SIM_CARD_TYPE_ICC == card_type) {
                SIMPB_LOG_D("Request complete: SIM_CARD_TYPE_ICC");
                simpbd_record_p->simpbd_client_p->supported_phonebooks = STE_SIMPB_PHONEBOOK_TYPE_GLOBAL;
                simpbd_record_p->simpbd_client_p->selected_phonebook = STE_SIMPB_PHONEBOOK_TYPE_GLOBAL;

                simpbd_complete_request(simpbd_record_p, STE_SIMPB_SUCCESS, NULL, 0);
                return STE_SIMPB_SUCCESS;
            }

            /* Get the FCP on <MF><DF_Telecom><DF_Phonebook><EF_PBR>. EF_PBR SHALL be present if phone book exists. */
            result = ste_uicc_sim_get_file_information((simpbd_record_p->simpbd_client_p)->ste_sim_p,
                     simpbd_record_get_sim_tag(simpbd_record_p),
                     simpbd_cache_get_fid_from_client_file_id(STE_SIMPB_FILE_TYPE_PBR, 0, 1, STE_SIMPB_PHONEBOOK_TYPE_GLOBAL, app_type),
                     simpbd_cache_get_path_on_selected_phonebook(STE_SIMPB_PHONEBOOK_TYPE_GLOBAL, app_type, card_type, STE_SIMPB_FILE_TYPE_PBR),
                     STE_UICC_SIM_GET_FILE_INFO_TYPE_EF);

            SIMPB_LOG_D("ste_uicc_sim_get_file_information() = %d", result);

            if (UICC_REQUEST_STATUS_OK != result) {
                simpbd_translate_sim_request_status_to_simpbd_code(simpbd_record_p, result);
                SIMPB_LOG_E("ste_uicc_sim_get_file_information, result %d", result);
                goto error;
            }

            if (STE_SIM_SUCCESS != result) {
                SIMPB_LOG_E("simpbd_cache_init %d", result);
                goto error;
            }

            simpbd_record_p->request_state = SIMPBD_REQUEST_STATE_STEP;
            simpbd_record_p->step_in_chain = SIMPBD_REQUEST_CHAIN_STARTUP_DO_UICC_DETERMINE_SUPPORTED_PHONEBOOK_LOCAL;

            break;
        }

        case SIMPBD_REQUEST_CHAIN_STARTUP_DO_UICC_DETERMINE_SUPPORTED_PHONEBOOK_LOCAL: {
            uicc_request_status_t result = UICC_REQUEST_STATUS_OK;
            sim_app_type_t app_type = simpbd_record_p->simpbd_client_p->app_type;
            sim_card_type_t card_type = simpbd_record_p->simpbd_client_p->card_type;
            ste_uicc_get_file_information_response_t *sim_file_information_p = NULL;

            /* RESPONSE handling from previous step in chain */
            if (NULL != simpbd_record_p->response_data_p) {
                sim_file_information_p = (ste_uicc_get_file_information_response_t *)simpbd_record_p->response_data_p;

                if (STE_UICC_STATUS_CODE_OK == sim_file_information_p->uicc_status_code) {
                    simpbd_record_p->simpbd_client_p->supported_phonebooks |= STE_SIMPB_PHONEBOOK_TYPE_GLOBAL;
                }

            } else {
                SIMPB_LOG_E("ste_uicc_sim_get_file_information, no response data!");
                goto error;
            }

            /* Get the FCP on <ADF_USIM><DF_Phonebook> <EF_PBR>. EF_PBR SHALL be present if phone book exists. */
            result = ste_uicc_sim_get_file_information((simpbd_record_p->simpbd_client_p)->ste_sim_p,
                     simpbd_record_get_sim_tag(simpbd_record_p),
                     simpbd_cache_get_fid_from_client_file_id(STE_SIMPB_FILE_TYPE_PBR, 0, 1, STE_SIMPB_PHONEBOOK_TYPE_LOCAL, app_type),
                     simpbd_cache_get_path_on_selected_phonebook(STE_SIMPB_PHONEBOOK_TYPE_LOCAL, app_type, card_type, STE_SIMPB_FILE_TYPE_PBR),
                     STE_UICC_SIM_GET_FILE_INFO_TYPE_EF);

            SIMPB_LOG_D("ste_uicc_sim_get_file_information() = %d", result);

            if (UICC_REQUEST_STATUS_OK != result) {
                simpbd_translate_sim_request_status_to_simpbd_code(simpbd_record_p, result);
                SIMPB_LOG_E("ste_uicc_sim_get_file_information, result %d", result);
                goto error;
            }

            if (STE_SIM_SUCCESS != result) {
                SIMPB_LOG_E("simpbd_cache_init %d", result);
                goto error;
            }

            simpbd_record_p->request_state = SIMPBD_REQUEST_STATE_STEP;
            simpbd_record_p->step_in_chain = SIMPBD_REQUEST_CHAIN_STARTUP_DO_UICC_CACHE_PBR_STRUCTURE;

            break;
        }

        case SIMPBD_REQUEST_CHAIN_STARTUP_DO_UICC_CACHE_PBR_STRUCTURE: {
            uicc_request_status_t result = UICC_REQUEST_STATUS_OK;
            sim_app_type_t app_type = simpbd_record_p->simpbd_client_p->app_type;
            sim_card_type_t card_type = simpbd_record_p->simpbd_client_p->card_type;
            ste_simpb_phonebook_type_t selected_phonebook = STE_SIMPB_PHONEBOOK_TYPE_GLOBAL;
            ste_simpb_phonebook_type_t client_selected_phonebook = simpbd_record_p->simpbd_client_p->client_selected_phonebook;
            /*Forcing to use Local phonebook */
            client_selected_phonebook = STE_SIMPB_PHONEBOOK_TYPE_LOCAL;

            uint8_t supported_phonebooks = simpbd_record_p->simpbd_client_p->supported_phonebooks;
            ste_uicc_get_file_information_response_t *sim_file_information_p = NULL;

            /* RESPONSE handling from previous step in chain */
            if (NULL != simpbd_record_p->response_data_p) {
                sim_file_information_p = (ste_uicc_get_file_information_response_t *)simpbd_record_p->response_data_p;

                if (STE_UICC_STATUS_CODE_OK == sim_file_information_p->uicc_status_code) {
                    supported_phonebooks |= STE_SIMPB_PHONEBOOK_TYPE_LOCAL;
                }

                /*Forcing to use Local phone book, as no Phone book selection possible*/
                selected_phonebook = STE_SIMPB_PHONEBOOK_TYPE_LOCAL;

                if (!((supported_phonebooks & STE_SIMPB_PHONEBOOK_TYPE_LOCAL) && (client_selected_phonebook == STE_SIMPB_PHONEBOOK_TYPE_LOCAL))) {
                    SIMPB_LOG_D("Local phone book Not supported");
                    simpbd_complete_request(simpbd_record_p, STE_SIMPB_SUCCESS, NULL, 0);
                    return STE_SIMPB_SUCCESS;
                }

                simpbd_record_p->simpbd_client_p->supported_phonebooks = supported_phonebooks;
                simpbd_record_p->simpbd_client_p->selected_phonebook = selected_phonebook;
            } else {
                SIMPB_LOG_E("ste_uicc_sim_get_file_information, no response data!");
                goto error;
            }

            SIMPB_LOG_D("get_fid = 0x%04X", simpbd_cache_get_fid_from_client_file_id(STE_SIMPB_FILE_TYPE_PBR, 0, 1, selected_phonebook, app_type));
            SIMPB_LOG_D("get_path = %s", simpbd_cache_get_path_on_selected_phonebook(selected_phonebook, app_type, card_type, STE_SIMPB_FILE_TYPE_PBR));

            result = ste_uicc_sim_file_read_record((simpbd_record_p->simpbd_client_p)->ste_sim_p,
                                                   simpbd_record_get_sim_tag(simpbd_record_p),
                                                   simpbd_cache_get_fid_from_client_file_id(STE_SIMPB_FILE_TYPE_PBR, 0, 1, selected_phonebook, app_type),
                                                   1,
                                                   0,
                                                   simpbd_cache_get_path_on_selected_phonebook(selected_phonebook, app_type, card_type, STE_SIMPB_FILE_TYPE_PBR));

            SIMPB_LOG_D("ste_uicc_sim_file_read_record() = %d", result);

            if (UICC_REQUEST_STATUS_OK != result) {
                /* We must fail this as we could not read any EF_PBR from SIM */
                SIMPB_LOG_E("ste_uicc_sim_file_read_record, result %d", result);
                goto error;
            }

            simpbd_record_p->private_data_p = (void *)1;
            simpbd_record_p->step_in_chain = SIMPBD_REQUEST_CHAIN_STARTUP_DO_UICC_CACHE_PBR_STRUCTURE_ITERATE;

            break;
        }

        case SIMPBD_REQUEST_CHAIN_STARTUP_DO_UICC_CACHE_PBR_STRUCTURE_ITERATE: {
            sim_card_type_t card_type = simpbd_record_p->simpbd_client_p->card_type;
            ste_simpb_result_t result;
            int slice = (int)(simpbd_record_p->private_data_p);

            if (NULL == simpbd_record_p->response_data_p) {
                SIMPB_LOG_E("ste_uicc_sim_file_read_binary, no response data!");
                goto error;
            }

            result = simpbd_create_and_store_pbr_cache(
                         (ste_uicc_sim_file_read_record_response_t *)simpbd_record_p->response_data_p, slice);

            if (STE_SIMPB_SUCCESS == result) {
                sim_app_type_t app_type = simpbd_record_p->simpbd_client_p->app_type;
                ste_simpb_phonebook_type_t selected_phonebook = simpbd_record_p->simpbd_client_p->selected_phonebook;
                uicc_request_status_t result = UICC_REQUEST_STATUS_OK;

                SIMPB_LOG_D("simpbd_record_p->simpbd_client_p->pbc_sync_flag = %d", simpbd_record_p->simpbd_client_p->pbc_sync_flag);

                if (0 == simpbd_record_p->simpbd_client_p->pbc_sync_flag) {
                    SIMPB_LOG_D("READ EF PBC for slice %d", slice);
                    (void)simpbd_execute(simpbd_record_p->simpbd_client_p, STE_SIMPB_INTERNAL_REQUEST_ID_SYNC_EF_PBC, (void *)slice, NULL);
                    simpbd_record_p->simpbd_client_p->pbc_sync_flag = 1;
                }

                slice++;

                SIMPB_LOG_D("fid = 0x%04X", simpbd_cache_get_fid_from_client_file_id(STE_SIMPB_FILE_TYPE_PBR, 0, 1, selected_phonebook, app_type));
                SIMPB_LOG_D("path = %s", simpbd_cache_get_path_on_selected_phonebook(selected_phonebook, app_type, card_type, STE_SIMPB_FILE_TYPE_PBR));
                SIMPB_LOG_D("slice = %d", slice);
                result = ste_uicc_sim_file_read_record((simpbd_record_p->simpbd_client_p)->ste_sim_p,
                                                       simpbd_record_get_sim_tag(simpbd_record_p),
                                                       simpbd_cache_get_fid_from_client_file_id(STE_SIMPB_FILE_TYPE_PBR, 0, 1, selected_phonebook, app_type),
                                                       slice, /* Note that slice in PBR context is the record in PBR */
                                                       0,
                                                       simpbd_cache_get_path_on_selected_phonebook(selected_phonebook, app_type, card_type, STE_SIMPB_FILE_TYPE_PBR));

                SIMPB_LOG_D("ste_uicc_sim_file_read_record() = %d iteration %d", result, slice);

                if (UICC_REQUEST_STATUS_OK != result) {
                    SIMPB_LOG_E("ste_uicc_sim_file_read_record, result %d", result);
                    goto error;
                }

                simpbd_record_p->private_data_p = (void *)slice;

            } else {
                SIMPB_LOG_D("EF PBR read done.");

                /* cache_adn_file_info() */
                /* read and cache adn empty/non empty status */
                result = simpbd_cache_adn_uid_file_info();

                if (result != STE_SIMPB_FAILURE) {
                    /* Read the file format for all ADN files by  ADN by ste_uicc_sim_file_get_format() */
                    /* Store simpbd_record_p->private_data_p, file id of the current file read */

                    result = simpbd_execute(simpbd_record_p->simpbd_client_p, STE_SIMPB_INTERNAL_REQUEST_ID_CACHE_ADN_INFO, (void *)simpbd_get_adn_file_info_head(), NULL);

                    if (STE_SIMPB_FAILURE == result) { /*Cleanup if we did not succeed*/
                        simpbd_cache_flush_file_info();
                    }

                } else {
                    SIMPB_LOG_E("ADN UID Info Caching failed", result);
                    goto error;
                }

                /* if we got an error from trying to add PBR data from previous read, then there are no more records for us */
                simpbd_complete_request(simpbd_record_p, STE_SIMPB_SUCCESS, NULL, 0);
                return STE_SIMPB_SUCCESS;
            }

            break;
        }

        default:
            SIMPB_LOG_E("unhandled step_in_chain = %d", simpbd_record_p->step_in_chain);
            goto error;
        }

        return STE_SIMPB_PENDING;

    default:
        SIMPB_LOG_E("unhandled request_state = %d", simpbd_record_p->request_state);
        goto error;
    }

error:
    simpbd_cache_flush_file_info();
    simpbd_complete_request(simpbd_record_p, STE_SIMPB_FAILURE, NULL, 0);
    return STE_SIMPB_FAILURE;
}

ste_simpb_result_t simpbd_handler_update_psc(simpbd_record_t *simpbd_record_p)
{
    sim_app_type_t app_type = simpbd_record_p->simpbd_client_p->app_type;
    sim_card_type_t card_type = simpbd_record_p->simpbd_client_p->card_type;
    ste_simpb_phonebook_type_t selected_phonebook = simpbd_record_p->simpbd_client_p->selected_phonebook;

    SIMPB_LOG_D("request_state=%d, step_in_chain=%d, response_data_p=%p",
                simpbd_record_p->request_state,
                simpbd_record_p->step_in_chain,
                simpbd_record_p->response_data_p);

    switch (simpbd_record_p->request_state) {
    case SIMPBD_REQUEST_STATE_REQUEST: {
        uicc_request_status_t result = UICC_REQUEST_STATUS_OK;
        /*Read PSC first*/
        result = ste_uicc_sim_file_read_binary((simpbd_record_p->simpbd_client_p)->ste_sim_p,
                                               simpbd_record_get_sim_tag(simpbd_record_p),
                                               simpbd_ef_psc_id,
                                               0,
                                               4,
                                               simpbd_cache_get_ef_path_phonebook(selected_phonebook, app_type));

        if (UICC_REQUEST_STATUS_OK != result) {
            SIMPB_LOG_E("ste_uicc_sim_file_read_binary, result %d", result);
            goto error;
        }

        simpbd_record_p->request_state = SIMPBD_REQUEST_STATE_STEP;
        simpbd_record_p->step_in_chain = SIMPBD_REQUEST_CHAIN_READ_PSC;
        return STE_SIMPB_PENDING;
    }
    break;

    case SIMPBD_REQUEST_STATE_STEP: {
        switch (simpbd_record_p->step_in_chain) {
        case SIMPBD_REQUEST_CHAIN_READ_PSC: {
            uicc_request_status_t result = UICC_REQUEST_STATUS_OK;
            ste_uicc_sim_file_read_binary_response_t *read_binary_resp_p =
                (ste_uicc_sim_file_read_binary_response_t *)simpbd_record_p->response_data_p;
            sim_app_type_t app_type = simpbd_record_p->simpbd_client_p->app_type;
            uint32_t psc_value = 0x00000000;
            uint8_t update_data[4];

            if (NULL == simpbd_record_p->response_data_p) {
                SIMPB_LOG_E("simpbd_record_p->response_data_p NULL");
                goto error;
            }

            if (UICC_REQUEST_STATUS_OK != result) {
                SIMPB_LOG_E("ste_uicc_sim_file_read_binary, result %d", result);
                goto error;
            }

            simpb_psc_value = ((uint32_t) * (read_binary_resp_p->data)) << 24;
            simpb_psc_value = ((uint32_t) * (read_binary_resp_p->data + 1)) << 16 | simpb_psc_value;
            simpb_psc_value = ((uint32_t) * (read_binary_resp_p->data + 2)) << 8 | simpb_psc_value;
            simpb_psc_value = ((uint32_t) * (read_binary_resp_p->data + 3)) | simpb_psc_value;

            SIMPB_LOG_D("EF_PSC current value: 0x%x", psc_value);

            simpb_psc_value = (simpb_psc_value + 1) % 0xFFFFFFFF;
            update_data[0] = ((simpb_psc_value >> 24) & 0xFF);
            update_data[1] = ((simpb_psc_value >> 16) & 0xFF);
            update_data[2] = ((simpb_psc_value >> 8) & 0xFF);
            update_data[3] = ((simpb_psc_value) & 0xFF);

            SIMPB_LOG_D("EF_PSC update to: 0x%x 0x%x 0x%x 0x%x", update_data[0], update_data[1], update_data[2], update_data[3]);
            result = ste_uicc_sim_file_update_binary(simpbd_record_p->simpbd_client_p->ste_sim_p,
                     simpbd_record_get_sim_tag(simpbd_record_p),
                     simpbd_ef_psc_id,
                     0,
                     4,
                     simpbd_cache_get_ef_path_phonebook(selected_phonebook, app_type),
                     update_data);

            if (UICC_REQUEST_STATUS_OK != result) {
                SIMPB_LOG_E("ste_uicc_sim_file_update_binary, result %d", result);
                goto error;
            }

            simpbd_record_p->request_state = SIMPBD_REQUEST_STATE_STEP;
            simpbd_record_p->step_in_chain = SIMPBD_REQUEST_CHAIN_UPDATE_PSC;
            return STE_SIMPB_PENDING;
        }
        break;

        case SIMPBD_REQUEST_CHAIN_UPDATE_PSC: {
            ste_uicc_update_sim_file_binary_response_t *update_binary_resp_p =
                (ste_uicc_update_sim_file_binary_response_t *)simpbd_record_p->response_data_p;

            if (NULL == simpbd_record_p->response_data_p) {
                SIMPB_LOG_E("simpbd_record_p->response_data_p NULL");
                goto error;
            }

            if (STE_UICC_STATUS_CODE_OK != update_binary_resp_p->uicc_status_code) {
                SIMPB_LOG_E("ste_uicc_sim_file_update_binary failed for EF_CC, status code %d", update_binary_resp_p->uicc_status_code);
                simpb_psc_value = (simpb_psc_value - 1) % 0xFFFFFFFF;
                goto error;
            }

            SIMPB_LOG_D("SYNC EF_PSC done.");
            simpbd_complete_request(simpbd_record_p, STE_SIMPB_SUCCESS, NULL, 0);
            return STE_SIMPB_SUCCESS;
        }
        break;

        default:
            SIMPB_LOG_E("unhandled step_in_chain = %d", simpbd_record_p->step_in_chain);
            goto error;
        }

        return STE_SIMPB_PENDING;
    }
    break;

    default:
        SIMPB_LOG_E("unhandled request_state = %d", simpbd_record_p->request_state);
        goto error;
    }

error:

    SIMPB_LOG_E("Update EF PSC return failure.");
    simpbd_complete_request(simpbd_record_p, STE_SIMPB_FAILURE, NULL, 0);
    return STE_SIMPB_FAILURE;
}

ste_simpb_result_t simpbd_handler_adn_uid_update(simpbd_record_t *simpbd_record_p)
{
    sim_app_type_t app_type = simpbd_record_p->simpbd_client_p->app_type;
    sim_card_type_t card_type = simpbd_record_p->simpbd_client_p->card_type;
    ste_simpb_phonebook_type_t selected_phonebook = simpbd_record_p->simpbd_client_p->selected_phonebook;
    uint8_t update_data[2];
    SIMPB_LOG_D("card_type=%d, app_type=%d, seleced pb=%p", card_type, app_type, selected_phonebook);

    switch (simpbd_record_p->request_state) {
    case SIMPBD_REQUEST_STATE_REQUEST: {
        SIMPB_LOG_D("request_state=%d, step_in_chain=%d, response_data_p=%p",
                    simpbd_record_p->request_state,
                    simpbd_record_p->step_in_chain,
                    simpbd_record_p->response_data_p);
        simpb_adn_file_t *adn_file_data_p = NULL;
        simpb_adn_update_data_t *adn_update_cmd_data = (simpb_adn_update_data_t *)simpbd_record_p->request_data_p;
        uicc_request_status_t result = UICC_REQUEST_STATUS_OK;

        if (NULL == adn_update_cmd_data) {
            SIMPB_LOG_E("adn_update_cmd_data is NULL");
            goto error;
        }

        /*get the format info from cache*/
        adn_file_data_p = simpbd_get_adn_file_info(adn_update_cmd_data->file_id);

        if (NULL == adn_file_data_p) {
            SIMPB_LOG_E("Could not get valid file info from cache");
            goto error;
        }

        /*update the record to be read*/
        adn_file_data_p->index = adn_update_cmd_data->record_num;

        /*update request data*/
        simpbd_record_p->request_data_p = (void *)adn_file_data_p;

        SIMPB_LOG_D("EF_ADN file_id = 0x%04X, EF_ADN file_path = %s", adn_file_data_p->fid, adn_file_data_p->file_path);

        /*Read the record*/
        result = ste_uicc_sim_file_read_record((simpbd_record_p->simpbd_client_p)->ste_sim_p,
                                               simpbd_record_get_sim_tag(simpbd_record_p),
                                               adn_file_data_p->fid,
                                               adn_file_data_p->index,
                                               adn_file_data_p->record_len,
                                               adn_file_data_p->file_path);

        if (UICC_REQUEST_STATUS_OK != result) {
            SIMPB_LOG_E("ste_uicc_sim_file_read_record, result %d", result);
            goto error;
        }

        simpbd_record_p->request_state = SIMPBD_REQUEST_STATE_STEP;
        simpbd_record_p->step_in_chain = SIMPBD_REQUEST_CHAIN_UPDATE_UID_ADN_READ_RECORD;

        return STE_SIMPB_PENDING;
    }
    break;

    case SIMPBD_REQUEST_STATE_STEP: {
        simpb_adn_file_t *adn_file_data_p = (simpb_adn_file_t *)simpbd_record_p->request_data_p;
        uint8_t name_len = 0;

        switch (simpbd_record_p->step_in_chain) {

        case SIMPBD_REQUEST_CHAIN_UPDATE_UID_ADN_READ_RECORD: {
            uicc_request_status_t result = UICC_REQUEST_STATUS_OK;
            ste_uicc_sim_file_read_record_response_t *read_record_resp_p =
                (ste_uicc_sim_file_read_record_response_t *)simpbd_record_p->response_data_p;
            uint8_t record_occupied = FALSE;
            uint8_t cache_rec_state = FALSE;

            if (NULL == simpbd_record_p->response_data_p) {
                SIMPB_LOG_E("simpbd_record_p->response_data_p NULL");
                goto error;
            }

            if (NULL == adn_file_data_p) {
                SIMPB_LOG_E("sync_data_p is NULL");
                goto error;
            }

            if (STE_UICC_STATUS_CODE_OK != read_record_resp_p->uicc_status_code) {
                SIMPB_LOG_E("ste_uicc_sim_file_read_record(%d) failed for EF_ADN, status code %d", adn_file_data_p->index, read_record_resp_p->uicc_status_code);
                goto error;
            }

            if (simpbd_ef_adn_record_occupied(adn_file_data_p->record_len, read_record_resp_p)) {
                record_occupied = TRUE;
            }

            simpb_adn_file_t *adn_file = simpbd_get_adn_file_info(adn_file_data_p->fid);

            if (NULL == adn_file) {
                SIMPB_LOG_E("ADN file not found in cache");
                goto error;
            }

            cache_rec_state = adn_file->adn_rec_occupied[adn_file_data_p->index - 1];

            if ((record_occupied == FALSE && TRUE == cache_rec_state) ||
                    (record_occupied == TRUE && TRUE == cache_rec_state)) {
                /*this is a deletion or an update*/
                if (record_occupied == FALSE && TRUE == cache_rec_state) {
                    /*if deletion, update the cache that record is empty*/
                    ste_simpb_result_t res = simpbd_update_cached_adn_record(adn_file_data_p->fid, adn_file_data_p->index - 1, FALSE);

                    if (res == STE_SIMPB_FAILURE) {
                        SIMPB_LOG_E("Could not update record in Cache");
                        goto error;
                    }
                }

                /*we just increment CC*/
                SIMPB_LOG_D("ADN Record %d, in File id %d Updated/Deleted", adn_file_data_p->index, adn_file_data_p->fid);
                SIMPB_LOG_D("CC Value: 0x%x", simpb_cc_value);

                simpb_p_cc_value = simpb_cc_value;
                uint8_t psc_update = FALSE;
                simpb_cc_value++;

                /*check if CC has exceeded*/
                if ((simpb_cc_value == 0xffff) || (simpb_cc_value == 0)) {
                    /*cc will exceed and PSC has to be reset*/
                    simpb_cc_value = 1;
                    (void)simpbd_execute(simpbd_record_p->simpbd_client_p, STE_SIMPB_INTERNAL_REQUEST_ID_UPDATE_PSC, NULL, NULL);
                }

                update_data[0] = ((simpb_cc_value >> 8) & 0xFF);
                update_data[1] = ((simpb_cc_value) & 0xFF);

                SIMPB_LOG_D("EF_CC update to: 0x%x 0x%x", update_data[0], update_data[1]);
                result = ste_uicc_sim_file_update_binary(simpbd_record_p->simpbd_client_p->ste_sim_p,
                         simpbd_record_get_sim_tag(simpbd_record_p),
                         simpbd_ef_cc_id,
                         0,
                         2,
                         simpbd_cache_get_ef_path_phonebook(selected_phonebook, app_type),
                         update_data);

                if (UICC_REQUEST_STATUS_OK != result) {
                    SIMPB_LOG_E("ste_uicc_sim_file_update_binary, result %d", result);
                    goto error;
                }

                simpbd_record_p->request_state = SIMPBD_REQUEST_STATE_STEP;
                simpbd_record_p->step_in_chain = SIMPBD_REQUEST_CHAIN_UPDATE_UID_UPDATE_EF_CC;
                return STE_SIMPB_PENDING;
            }

            else if (cache_rec_state == FALSE && record_occupied == TRUE) {
                /* This is addition, so update the cache first and then update UID*/
                SIMPB_LOG_D("ADN Record %d, File id %d Added", adn_file_data_p->index, adn_file_data_p->fid);
                ste_simpb_result_t res = simpbd_update_cached_adn_record(adn_file_data_p->fid, adn_file_data_p->index - 1, TRUE);

                if (res == STE_SIMPB_FAILURE) {
                    SIMPB_LOG_E("Could not update record in Cache");
                    goto error;
                }

                /*check PUID first, if we have to regenerate UIDs*/
                SIMPB_LOG_D("PUID Value: 0x%x", simpb_puid_value);

                if (simpb_puid_value >= 0xFFFE) {
                    /*update UIDs for occupied ADN records*/
                    SIMPB_LOG_D("Regenerate UIDs");
                    simpbd_cache_flush_occupied_rec_info(); /* first time call, so flush*/

                    if (simpdb_get_cached_occupied_adn_record()) {
                        simpbd_record_p->private_data_p = NULL; /*This is not a lone record update*/
                        simpbd_cache_occupied_record = simpbd_get_occ_rec_holder(); /*get the occupied rec info holder*/

                        /*Occupied record found, reset PUID and update the UID record*/
                        uint8_t update_data[2];
                        simpb_puid_value = 1;
                        update_data[0] = ((simpb_puid_value >> 8) & 0xFF);
                        update_data[1] = ((simpb_puid_value) & 0xFF);
                        SIMPB_LOG_D("Update UID (%d) record %d with %d", simpbd_cache_occupied_record->uid_file->fid,
                                    simpbd_cache_occupied_record->rec_num, simpb_puid_value);
                        result = ste_uicc_sim_file_update_record(simpbd_record_p->simpbd_client_p->ste_sim_p,
                                 simpbd_record_get_sim_tag(simpbd_record_p),
                                 simpbd_cache_occupied_record->uid_file->fid,
                                 simpbd_cache_occupied_record->rec_num,
                                 simpbd_cache_occupied_record->uid_file->record_len,
                                 simpbd_cache_occupied_record->uid_file->file_path,
                                 update_data);

                        if (UICC_REQUEST_STATUS_OK != result) {
                            SIMPB_LOG_E("ste_uicc_sim_file_update_record, result %d", result);
                            goto error;
                        }

                        simpbd_record_p->request_state = SIMPBD_REQUEST_STATE_STEP;
                        simpbd_record_p->step_in_chain = SIMPBD_REQUEST_CHAIN_UPDATE_UID_UPDATE_UID_RECORD;
                        return STE_SIMPB_PENDING;
                    } else {
                        SIMPB_LOG_E("This state is undesirable, no occupied record found");
                        goto error;
                    }
                } else {

                    SIMPB_LOG_D("Update single UID record");

                    /*update the corresponding record in UID, and increment PUID and update */
                    simpb_uid_file_t *uid_file_info = NULL;
                    uid_file_info = simpbd_get_uid_file_info_for_adn_fid(adn_file_data_p->fid);

                    if (NULL == uid_file_info) {
                        SIMPB_LOG_E("Could not find UID file info to update the record");
                        goto error;
                    }

                    simpb_puid_value = (simpb_puid_value + 1) % 0xFFFF;
                    update_data[0] = ((simpb_puid_value >> 8) & 0xFF);
                    update_data[1] = ((simpb_puid_value) & 0xFF);
                    SIMPB_LOG_D("Update single UID record with PUID Value: 0x%x", simpb_puid_value);
                    SIMPB_LOG_D("fid: %d, rec num: %d", uid_file_info->fid, adn_file_data_p->index);
                    result = ste_uicc_sim_file_update_record(simpbd_record_p->simpbd_client_p->ste_sim_p,
                             simpbd_record_get_sim_tag(simpbd_record_p),
                             uid_file_info->fid,
                             adn_file_data_p->index,
                             uid_file_info->record_len,
                             uid_file_info->file_path,
                             update_data);

                    if (UICC_REQUEST_STATUS_OK != result) {
                        SIMPB_LOG_E("ste_uicc_sim_file_update_record, result %d", result);
                        goto error;
                    }

                    /*Just a flag to say that UID record has to be updated after PUID update*/
                    simpbd_record_p->private_data_p = (void *)1;

                    simpbd_record_p->request_state = SIMPBD_REQUEST_STATE_STEP;
                    simpbd_record_p->step_in_chain = SIMPBD_REQUEST_CHAIN_UPDATE_UID_UPDATE_UID_RECORD;

                    return STE_SIMPB_PENDING;
                }
            } else {
                SIMPB_LOG_E("Cannot determine what was done with ADN");
                goto error;
            }
        }
        break;

        case SIMPBD_REQUEST_CHAIN_UPDATE_UID_UPDATE_UID_RECORD: {
            uicc_request_status_t result = UICC_REQUEST_STATUS_OK;
            ste_uicc_update_sim_file_record_response_t *update_record_resp_p =
                (ste_uicc_update_sim_file_record_response_t *)simpbd_record_p->response_data_p;

            if (NULL == simpbd_record_p->response_data_p) {
                SIMPB_LOG_E("simpbd_record_p->response_data_p NULL");
                goto error;
            }

            if (STE_UICC_STATUS_CODE_OK != update_record_resp_p->uicc_status_code) {
                SIMPB_LOG_E("ste_uicc_sim_file_update_record(%d) failed for EF_UID, status code %d, status word 0x%x, 0x%x", simpbd_cache_occupied_record->rec_num, update_record_resp_p->uicc_status_code,
                            update_record_resp_p->status_word.sw1, update_record_resp_p->status_word.sw2);
                goto error;
            }

            /*Check if this call is for updating a lone record, then update puid and the CC and finish*/
            if ((int)simpbd_record_p->private_data_p) {
                SIMPB_LOG_D("Updated single UID record, Now update PUID");
                update_data[0] = ((simpb_puid_value >> 8) & 0xFF);
                update_data[1] = ((simpb_puid_value) & 0xFF);

                result = ste_uicc_sim_file_update_binary(simpbd_record_p->simpbd_client_p->ste_sim_p,
                         simpbd_record_get_sim_tag(simpbd_record_p),
                         simpbd_ef_puid_id,
                         0,
                         2,
                         simpbd_cache_get_ef_path_phonebook(selected_phonebook, app_type),
                         update_data);

                if (UICC_REQUEST_STATUS_OK != result) {
                    SIMPB_LOG_E("ste_uicc_sim_file_update_binary, result %d", result);
                    goto error;
                }

                simpbd_record_p->request_state = SIMPBD_REQUEST_STATE_STEP;
                simpbd_record_p->step_in_chain = SIMPBD_REQUEST_CHAIN_UPDATE_UID_UPDATE_PUID;
                return STE_SIMPB_PENDING;
            }

            if (simpdb_get_cached_occupied_adn_record()) {
                /*Next Occupied record found*/
                simpb_puid_value = (simpb_puid_value + 1) % 0xFFFF;
                update_data[0] = ((simpb_puid_value >> 8) & 0xFF);
                update_data[1] = ((simpb_puid_value) & 0xFF);
                SIMPB_LOG_D("Update UID (%d) record %d with %d", simpbd_cache_occupied_record->uid_file->fid,
                            simpbd_cache_occupied_record->rec_num, simpb_puid_value);
                result = ste_uicc_sim_file_update_record(simpbd_record_p->simpbd_client_p->ste_sim_p,
                         simpbd_record_get_sim_tag(simpbd_record_p),
                         simpbd_cache_occupied_record->uid_file->fid,
                         simpbd_cache_occupied_record->rec_num,
                         simpbd_cache_occupied_record->uid_file->record_len,
                         simpbd_cache_occupied_record->uid_file->file_path,
                         update_data);

                if (UICC_REQUEST_STATUS_OK != result) {
                    SIMPB_LOG_E("ste_uicc_sim_file_update_record, result %d", result);
                    goto error;
                }

                simpbd_record_p->request_state = SIMPBD_REQUEST_STATE_STEP;
                simpbd_record_p->step_in_chain = SIMPBD_REQUEST_CHAIN_UPDATE_UID_UPDATE_UID_RECORD;
                return STE_SIMPB_PENDING;
            }

            /*No more occupied records, Then update PUID*/
            update_data[0] = ((simpb_puid_value >> 8) & 0xFF);
            update_data[1] = ((simpb_puid_value) & 0xFF);
            result = ste_uicc_sim_file_update_binary(simpbd_record_p->simpbd_client_p->ste_sim_p,
                     simpbd_record_get_sim_tag(simpbd_record_p),
                     simpbd_ef_puid_id,
                     0,
                     2,
                     simpbd_cache_get_ef_path_phonebook(selected_phonebook, app_type),
                     update_data);

            if (UICC_REQUEST_STATUS_OK != result) {
                SIMPB_LOG_E("ste_uicc_sim_file_update_binary, result %d", result);
                goto error;
            }

            simpbd_record_p->request_state = SIMPBD_REQUEST_STATE_STEP;
            simpbd_record_p->step_in_chain = SIMPBD_REQUEST_CHAIN_UPDATE_UID_UPDATE_PUID;
            return STE_SIMPB_PENDING;
        }

        break;

        case SIMPBD_REQUEST_CHAIN_UPDATE_UID_UPDATE_PUID: {
            uicc_request_status_t result = UICC_REQUEST_STATUS_OK;
            ste_uicc_update_sim_file_binary_response_t *update_binary_resp_p =
                (ste_uicc_update_sim_file_binary_response_t *)simpbd_record_p->response_data_p;

            uint8_t psc_update = FALSE;

            if (NULL == simpbd_record_p->response_data_p) {
                SIMPB_LOG_E("simpbd_record_p->response_data_p NULL");
                goto error;
            }

            if (STE_UICC_STATUS_CODE_OK != update_binary_resp_p->uicc_status_code) {
                SIMPB_LOG_E("ste_uicc_sim_file_update_binary failed for EF_PUID, status code %d, status word 0x%x, 0x%x", update_binary_resp_p->uicc_status_code,
                            update_binary_resp_p->status_word.sw1, update_binary_resp_p->status_word.sw2);
                goto error;
            }

            SIMPB_LOG_D("Update EF PUID done (%d)", simpb_puid_value);
            /*increment CC after PUID update*/



            /*check if CC has exceeded*/
            simpb_p_cc_value = simpb_cc_value;

            simpb_cc_value++;

            /*check if CC has exceeded*/
            if ((simpb_cc_value == 0xffff) || (simpb_cc_value == 0)) {
                /*cc will exceed and PSC has to be reset*/
                simpb_cc_value = 1;
                (void)simpbd_execute(simpbd_record_p->simpbd_client_p, STE_SIMPB_INTERNAL_REQUEST_ID_UPDATE_PSC, NULL, NULL);
            }

            update_data[0] = ((simpb_cc_value >> 8) & 0xFF);
            update_data[1] = ((simpb_cc_value) & 0xFF);

            SIMPB_LOG_D("EF_CC update to: 0x%x 0x%x", update_data[0], update_data[1]);
            result = ste_uicc_sim_file_update_binary(simpbd_record_p->simpbd_client_p->ste_sim_p,
                     simpbd_record_get_sim_tag(simpbd_record_p),
                     simpbd_ef_cc_id,
                     0,
                     2,
                     simpbd_cache_get_ef_path_phonebook(selected_phonebook, app_type),
                     update_data);

            if (UICC_REQUEST_STATUS_OK != result) {
                SIMPB_LOG_E("ste_uicc_sim_file_update_binary, result %d", result);
                goto error;
            }

            simpbd_record_p->private_data_p = NULL; /*reset it*/

            simpbd_record_p->request_state = SIMPBD_REQUEST_STATE_STEP;
            simpbd_record_p->step_in_chain = SIMPBD_REQUEST_CHAIN_UPDATE_UID_UPDATE_EF_CC;
            return STE_SIMPB_PENDING;
        }
        break;

        case SIMPBD_REQUEST_CHAIN_UPDATE_UID_UPDATE_EF_CC: {
            ste_uicc_update_sim_file_binary_response_t *update_binary_resp_p =
                (ste_uicc_update_sim_file_binary_response_t *)simpbd_record_p->response_data_p;

            if (NULL == simpbd_record_p->response_data_p) {
                SIMPB_LOG_E("simpbd_record_p->response_data_p NULL");
                SIMPB_LOG_D("Update EF CC failed reverting CC value in cache to %d.", simpb_p_cc_value);
                simpb_cc_value = simpb_p_cc_value;
                goto error;
            }

            if (STE_UICC_STATUS_CODE_OK != update_binary_resp_p->uicc_status_code) {
                SIMPB_LOG_E("ste_uicc_sim_file_update_binary failed for EF_CC, status code %d, status word 0x%x, 0x%x", update_binary_resp_p->uicc_status_code,
                            update_binary_resp_p->status_word.sw1, update_binary_resp_p->status_word.sw2);
                SIMPB_LOG_D("Update EF CC failed reverting CC value in cache to %d.", simpb_p_cc_value);
                simpb_cc_value = simpb_p_cc_value;
                goto error;
            }

            SIMPB_LOG_D("Update EF CC done (%d).", simpb_cc_value);
            simpbd_complete_request(simpbd_record_p, STE_SIMPB_SUCCESS, NULL, 0);
            return STE_SIMPB_SUCCESS;
        }
        break;

        default:
            SIMPB_LOG_E("unhandled step_in_chain = %d", simpbd_record_p->step_in_chain);
            goto error;
        }

    }
    break;

    default:
        SIMPB_LOG_E("unhandled request_state = %d", simpbd_record_p->request_state);
        goto error;
    }

error:

    SIMPB_LOG_E("UPDATE EF_UID return failure.");
    simpbd_complete_request(simpbd_record_p, STE_SIMPB_FAILURE, NULL, 0);
    return STE_SIMPB_FAILURE;
}


ste_simpb_result_t simpbd_handler_cache_adn_info(simpbd_record_t *simpbd_record_p)
{
    sim_app_type_t app_type = simpbd_record_p->simpbd_client_p->app_type;
    sim_card_type_t card_type = simpbd_record_p->simpbd_client_p->card_type;
    ste_simpb_phonebook_type_t selected_phonebook = simpbd_record_p->simpbd_client_p->selected_phonebook;

    simpb_adn_file_t *adn_file_data_p = (simpb_adn_file_t *)simpbd_record_p->request_data_p;

    switch (simpbd_record_p->request_state) {
    case SIMPBD_REQUEST_STATE_REQUEST: {
        SIMPB_LOG_D("request_state=%d, step_in_chain=%d, response_data_p=%p",
                    simpbd_record_p->request_state,
                    simpbd_record_p->step_in_chain,
                    simpbd_record_p->response_data_p);
        SIMPB_LOG_D("card_type=%d, app_type=%d, seleced pb=%p", card_type, app_type, selected_phonebook);

        uicc_request_status_t result = UICC_REQUEST_STATUS_OK;

        adn_file_data_p->file_path = simpbd_cache_get_path_on_selected_phonebook(selected_phonebook, app_type, card_type, STE_SIMPB_FILE_TYPE_ADN);

        adn_file_data_p->num_records = 0;
        adn_file_data_p->index = 0;
        adn_file_data_p->num_udpated = 0;

        SIMPB_LOG_D("EF_ADN file_id = 0x%04X", adn_file_data_p->fid);
        SIMPB_LOG_D("EF_ADN file_path = %s", adn_file_data_p->file_path);

        result = ste_uicc_sim_file_get_format((simpbd_record_p->simpbd_client_p)->ste_sim_p,
                                              simpbd_record_get_sim_tag(simpbd_record_p),
                                              adn_file_data_p->fid,
                                              adn_file_data_p->file_path);

        SIMPB_LOG_D("ste_uicc_sim_file_get_format() = %d", result);

        if (UICC_REQUEST_STATUS_OK != result) {
            SIMPB_LOG_E("ste_uicc_sim_file_get_format, result %d", result);
            goto error;
        }

        simpbd_record_p->request_state = SIMPBD_REQUEST_STATE_STEP;
        simpbd_record_p->step_in_chain = SIMPBD_REQUEST_CHAIN_READ_ADN_FILE_FORMAT;
        return STE_SIMPB_PENDING;
    }
    break;

    case SIMPBD_REQUEST_STATE_STEP: {
        switch (simpbd_record_p->step_in_chain) {
        case SIMPBD_REQUEST_CHAIN_READ_ADN_FILE_FORMAT: {
            uicc_request_status_t result = UICC_REQUEST_STATUS_OK;
            ste_uicc_sim_file_get_format_response_t *file_format_p =
                (ste_uicc_sim_file_get_format_response_t *)simpbd_record_p->response_data_p;

            if (NULL == simpbd_record_p->response_data_p) {
                SIMPB_LOG_E("simpbd_record_p->response_data_p NULL");
                goto error;
            }

            if (NULL == adn_file_data_p) {
                SIMPB_LOG_E("adn_file_data_p is NULL");
                goto error;
            }

            if (STE_UICC_STATUS_CODE_OK != file_format_p->uicc_status_code) {
                SIMPB_LOG_E("ste_uicc_sim_file_get_format failed for EF_PBC, status code %d, status word 0x%x, 0x%x", file_format_p->uicc_status_code,
                            file_format_p->status_word.sw1, file_format_p->status_word.sw2);
                goto error;
            }

            SIMPB_LOG_D("EF_ADN num_records = 0x%04X", file_format_p->num_records);
            SIMPB_LOG_D("EF_ADN record_len = %d", file_format_p->record_len);
            adn_file_data_p->num_records = file_format_p->num_records;
            adn_file_data_p->record_len = file_format_p->record_len;

            if (file_format_p->num_records == 0) {
                SIMPB_LOG_E("EF ADN has no records");
                goto error;
            }

            /*go to next entry in ADN file cache*/
            if (adn_file_data_p->next != NULL) {
                adn_file_data_p = adn_file_data_p->next;
                adn_file_data_p->file_path = simpbd_cache_get_path_on_selected_phonebook(selected_phonebook, app_type, card_type, STE_SIMPB_FILE_TYPE_ADN);
                adn_file_data_p->num_records = 0;
                adn_file_data_p->index = 0;
                adn_file_data_p->num_udpated = 0;
                SIMPB_LOG_D("EF_ADN file_id = 0x%04X", adn_file_data_p->fid);
                SIMPB_LOG_D("EF_ADN file_path = %s", adn_file_data_p->file_path);

                result = ste_uicc_sim_file_get_format((simpbd_record_p->simpbd_client_p)->ste_sim_p,
                                                      simpbd_record_get_sim_tag(simpbd_record_p),
                                                      adn_file_data_p->fid,
                                                      adn_file_data_p->file_path);

                SIMPB_LOG_D("ste_uicc_sim_file_get_format() = %d", result);

                if (UICC_REQUEST_STATUS_OK != result) {
                    SIMPB_LOG_E("ste_uicc_sim_file_get_format, result %d", result);
                    goto error;
                }

                simpbd_record_p->request_data_p = (void *) adn_file_data_p;
                simpbd_record_p->request_state = SIMPBD_REQUEST_STATE_STEP;
                simpbd_record_p->step_in_chain = SIMPBD_REQUEST_CHAIN_READ_ADN_FILE_FORMAT;
            } else {
                /*Read first record*/
                adn_file_data_p = simpbd_get_adn_file_info_head();
                adn_file_data_p->index = 1;
                simpbd_record_p->request_data_p = (void *) simpbd_get_adn_file_info_head();

                SIMPB_LOG_D("EF_ADN read record: %d", adn_file_data_p->index);

                result = ste_uicc_sim_file_read_record((simpbd_record_p->simpbd_client_p)->ste_sim_p,
                                                       simpbd_record_get_sim_tag(simpbd_record_p),
                                                       adn_file_data_p->fid,
                                                       adn_file_data_p->index,
                                                       adn_file_data_p->record_len,
                                                       adn_file_data_p->file_path);

                if (UICC_REQUEST_STATUS_OK != result) {
                    SIMPB_LOG_E("ste_uicc_sim_file_read_record, result %d", result);
                    goto error;
                }

                simpbd_record_p->request_state = SIMPBD_REQUEST_STATE_STEP;
                simpbd_record_p->step_in_chain = SIMPBD_REQUEST_CHAIN_ADN_FILE_ITERATE_READ_RECORD;
            }

            return STE_SIMPB_PENDING;
        }
        break;

        case SIMPBD_REQUEST_CHAIN_ADN_FILE_ITERATE_READ_RECORD: {
            uicc_request_status_t result = UICC_REQUEST_STATUS_OK;
            ste_uicc_sim_file_read_record_response_t *read_record_resp_p =
                (ste_uicc_sim_file_read_record_response_t *)simpbd_record_p->response_data_p;

            if (NULL == simpbd_record_p->response_data_p) {
                SIMPB_LOG_E("simpbd_record_p->response_data_p NULL");
                goto error;
            }

            if (NULL == adn_file_data_p) {
                SIMPB_LOG_E("adn_file_data_p is NULL");
                goto error;
            }

            if (STE_UICC_STATUS_CODE_OK != read_record_resp_p->uicc_status_code) {
                SIMPB_LOG_E("ste_uicc_sim_file_read_record(%d) failed for EF_ADN, status code %d", adn_file_data_p->index, read_record_resp_p->uicc_status_code);
                goto error;
            }

            SIMPB_LOG_D("Adn index =  %d", adn_file_data_p->index);

            if (simpbd_ef_adn_record_occupied(adn_file_data_p->record_len, read_record_resp_p)) {
                adn_file_data_p->adn_rec_occupied[adn_file_data_p->index - 1] = TRUE;
            } else {
                adn_file_data_p->adn_rec_occupied[adn_file_data_p->index - 1] = FALSE;
            }

            /*Read the next record, if last record is read then move to next ADN File*/
            if (adn_file_data_p->index + 1 <= adn_file_data_p->num_records) {
                adn_file_data_p->index++;
            } else if (adn_file_data_p->next != NULL) {
                adn_file_data_p = adn_file_data_p->next;
                adn_file_data_p->index = 1;
                simpbd_record_p->request_data_p = (void *) adn_file_data_p;
            } else {
                SIMPB_LOG_D("SYNC EF ADN READ done.");
                /*Now read and store the CC value*/
                result = ste_uicc_sim_file_read_binary((simpbd_record_p->simpbd_client_p)->ste_sim_p,
                                                       simpbd_record_get_sim_tag(simpbd_record_p),
                                                       simpbd_ef_cc_id,
                                                       0,
                                                       2,
                                                       simpbd_cache_get_ef_path_phonebook(selected_phonebook, app_type));

                if (UICC_REQUEST_STATUS_OK != result) {
                    SIMPB_LOG_E("ste_uicc_sim_file_read_binary, result %d", result);
                    goto error;
                }

                simpbd_record_p->request_state = SIMPBD_REQUEST_STATE_STEP;
                simpbd_record_p->step_in_chain = SIMPBD_REQUEST_CHAIN_ADN_FILE_READ_EF_CC;
                return STE_SIMPB_PENDING;
            }

            result = ste_uicc_sim_file_read_record((simpbd_record_p->simpbd_client_p)->ste_sim_p,
                                                   simpbd_record_get_sim_tag(simpbd_record_p),
                                                   adn_file_data_p->fid,
                                                   adn_file_data_p->index,
                                                   adn_file_data_p->record_len,
                                                   adn_file_data_p->file_path);

            if (UICC_REQUEST_STATUS_OK != result) {
                SIMPB_LOG_E("ste_uicc_sim_file_read_record, result %d", result);
                goto error;
            }

            simpbd_record_p->request_state = SIMPBD_REQUEST_STATE_STEP;
            simpbd_record_p->step_in_chain = SIMPBD_REQUEST_CHAIN_ADN_FILE_ITERATE_READ_RECORD;
            return STE_SIMPB_PENDING;
        }
        break;

        case SIMPBD_REQUEST_CHAIN_ADN_FILE_READ_EF_CC: {
            uicc_request_status_t result = UICC_REQUEST_STATUS_OK;
            ste_uicc_sim_file_read_binary_response_t *read_binary_resp_p =
                (ste_uicc_sim_file_read_binary_response_t *)simpbd_record_p->response_data_p;
            sim_app_type_t app_type = simpbd_record_p->simpbd_client_p->app_type;

            if (NULL == simpbd_record_p->response_data_p) {
                SIMPB_LOG_E("simpbd_record_p->response_data_p NULL");
                goto error;
            }

            if (STE_UICC_STATUS_CODE_OK != read_binary_resp_p->uicc_status_code) {
                SIMPB_LOG_E("ste_uicc_sim_file_read_binary failed for EF_CC, status code %d, status word 0x%x, 0x%x", read_binary_resp_p->uicc_status_code,
                            read_binary_resp_p->status_word.sw1, read_binary_resp_p->status_word.sw2);
                goto error;
            }

            simpb_cc_value = ((uint16_t) * (read_binary_resp_p->data)) << 8;
            simpb_cc_value = ((uint16_t) * (read_binary_resp_p->data + 1)) | simpb_cc_value;

            SIMPB_LOG_D("EF_CC read done, value: 0x%x", simpb_cc_value);
            /*Now read and store the PUID value*/
            result = ste_uicc_sim_file_read_binary((simpbd_record_p->simpbd_client_p)->ste_sim_p,
                                                   simpbd_record_get_sim_tag(simpbd_record_p),
                                                   simpbd_ef_puid_id,
                                                   0,
                                                   2,
                                                   simpbd_cache_get_ef_path_phonebook(selected_phonebook, app_type));

            if (UICC_REQUEST_STATUS_OK != result) {
                SIMPB_LOG_E("ste_uicc_sim_file_read_binary, result %d", result);
                goto error;
            }

            simpbd_record_p->request_state = SIMPBD_REQUEST_STATE_STEP;
            simpbd_record_p->step_in_chain = SIMPBD_REQUEST_CHAIN_ADN_FILE_READ_EF_PUID;
            return STE_SIMPB_PENDING;
        }
        break;

        case SIMPBD_REQUEST_CHAIN_ADN_FILE_READ_EF_PUID: {
            uicc_request_status_t result = UICC_REQUEST_STATUS_OK;
            ste_uicc_sim_file_read_binary_response_t *read_binary_resp_p =
                (ste_uicc_sim_file_read_binary_response_t *)simpbd_record_p->response_data_p;
            sim_app_type_t app_type = simpbd_record_p->simpbd_client_p->app_type;

            if (NULL == simpbd_record_p->response_data_p) {
                SIMPB_LOG_E("simpbd_record_p->response_data_p NULL");
                goto error;
            }

            if (STE_UICC_STATUS_CODE_OK != read_binary_resp_p->uicc_status_code) {
                SIMPB_LOG_E("ste_uicc_sim_file_read_binary failed for EF_PUID, status code %d, status word 0x%x, 0x%x", read_binary_resp_p->uicc_status_code,
                            read_binary_resp_p->status_word.sw1, read_binary_resp_p->status_word.sw2);
                goto error;
            }

            simpb_puid_value = ((uint16_t) * (read_binary_resp_p->data)) << 8;
            simpb_puid_value = ((uint16_t) * (read_binary_resp_p->data + 1)) | simpb_puid_value;
            SIMPB_LOG_D("EF_PUID value: 0x%x", simpb_puid_value);
            SIMPB_LOG_D("SYNC EF PUID read done.");
            /*Now read and store the PSC value*/
            result = ste_uicc_sim_file_read_binary((simpbd_record_p->simpbd_client_p)->ste_sim_p,
                                                   simpbd_record_get_sim_tag(simpbd_record_p),
                                                   simpbd_ef_psc_id,
                                                   0,
                                                   4,
                                                   simpbd_cache_get_ef_path_phonebook(selected_phonebook, app_type));

            if (UICC_REQUEST_STATUS_OK != result) {
                SIMPB_LOG_E("ste_uicc_sim_file_read_binary, result %d", result);
                goto error;
            }

            simpbd_record_p->request_state = SIMPBD_REQUEST_STATE_STEP;
            simpbd_record_p->step_in_chain = SIMPBD_REQUEST_CHAIN_ADN_FILE_READ_EF_PSC;
            return STE_SIMPB_PENDING;
        }
        break;

        case SIMPBD_REQUEST_CHAIN_ADN_FILE_READ_EF_PSC: {
            uicc_request_status_t result = UICC_REQUEST_STATUS_OK;
            ste_uicc_sim_file_read_binary_response_t *read_binary_resp_p =
                (ste_uicc_sim_file_read_binary_response_t *)simpbd_record_p->response_data_p;
            sim_app_type_t app_type = simpbd_record_p->simpbd_client_p->app_type;

            if (NULL == simpbd_record_p->response_data_p) {
                SIMPB_LOG_E("simpbd_record_p->response_data_p NULL");
                goto error;
            }

            if (STE_UICC_STATUS_CODE_OK != read_binary_resp_p->uicc_status_code) {
                SIMPB_LOG_E("ste_uicc_sim_file_read_binary failed for EF_PSC, status code %d, status word 0x%x, 0x%x", read_binary_resp_p->uicc_status_code,
                            read_binary_resp_p->status_word.sw1, read_binary_resp_p->status_word.sw2);
                goto error;
            }

            simpb_psc_value = ((uint32_t) * (read_binary_resp_p->data)) << 24;
            simpb_psc_value = ((uint32_t) * (read_binary_resp_p->data + 1)) << 16 | simpb_psc_value;
            simpb_psc_value = ((uint32_t) * (read_binary_resp_p->data + 2)) << 8 | simpb_psc_value;
            simpb_psc_value = ((uint32_t) * (read_binary_resp_p->data + 3)) | simpb_psc_value;
            SIMPB_LOG_D("SYNC EF PSC read done, EF_PSC value: 0x%x", simpb_psc_value);

            /*read UID info*/

            simpb_uid_file_t *uid_file_data_p = simpbd_get_uid_file_info_head();
            simpbd_record_p->request_data_p = (void *)uid_file_data_p;
            SIMPB_LOG_D("Get EF_uid file_id from EF_PBR...");
            uid_file_data_p->file_path = simpbd_cache_get_path_on_selected_phonebook(selected_phonebook, app_type, card_type, STE_SIMPB_FILE_TYPE_UID);

            uid_file_data_p->num_records = 0;
            uid_file_data_p->index = 0;
            uid_file_data_p->num_udpated = 0;

            SIMPB_LOG_D("EF_uid file_id = 0x%04X, EF_uid file_path = %s", uid_file_data_p->fid, uid_file_data_p->file_path);

            result = ste_uicc_sim_file_get_format((simpbd_record_p->simpbd_client_p)->ste_sim_p,
                                                  simpbd_record_get_sim_tag(simpbd_record_p),
                                                  uid_file_data_p->fid,
                                                  uid_file_data_p->file_path);

            SIMPB_LOG_D("ste_uicc_sim_file_get_format() = %d", result);

            if (UICC_REQUEST_STATUS_OK != result) {
                SIMPB_LOG_E("ste_uicc_sim_file_get_format, result %d", result);
                goto error;
            }

            simpbd_record_p->request_state = SIMPBD_REQUEST_STATE_STEP;
            simpbd_record_p->step_in_chain = SIMPBD_REQUEST_CHAIN_ADN_FILE_READ_UID_FILE_FORMAT;
            return STE_SIMPB_PENDING;
        }
        break;

        case SIMPBD_REQUEST_CHAIN_ADN_FILE_READ_UID_FILE_FORMAT: {
            uicc_request_status_t result = UICC_REQUEST_STATUS_OK;
            ste_uicc_sim_file_get_format_response_t *file_format_p =
                (ste_uicc_sim_file_get_format_response_t *)simpbd_record_p->response_data_p;
            simpb_uid_file_t *uid_file_data_p = (simpb_uid_file_t *)simpbd_record_p->request_data_p;

            if (NULL == simpbd_record_p->response_data_p) {
                SIMPB_LOG_E("simpbd_record_p->response_data_p NULL");
                goto error;
            }

            if (NULL == uid_file_data_p) {
                SIMPB_LOG_E("uid_file_data_p is NULL");
                goto error;
            }

            if (STE_UICC_STATUS_CODE_OK != file_format_p->uicc_status_code) {
                SIMPB_LOG_E("ste_uicc_sim_file_get_format failed for EF_UID, status code %d, status word 0x%x, 0x%x", file_format_p->uicc_status_code,
                            file_format_p->status_word.sw1, file_format_p->status_word.sw2);
                goto error;
            }

            SIMPB_LOG_D("EF_uid num_records = 0x%04X, EF_uid record_len = %d", file_format_p->num_records, file_format_p->record_len);
            uid_file_data_p->num_records = file_format_p->num_records;
            uid_file_data_p->record_len = file_format_p->record_len;

            if (file_format_p->num_records == 0) {
                SIMPB_LOG_D("EF uid has no records");
            }

            /*go to next entry in uid file cache*/
            if (uid_file_data_p->next != NULL) {
                uid_file_data_p = uid_file_data_p->next;
                uid_file_data_p->file_path = simpbd_cache_get_path_on_selected_phonebook(selected_phonebook, app_type, card_type, STE_SIMPB_FILE_TYPE_UID);
                uid_file_data_p->num_records = 0;
                uid_file_data_p->index = 0;
                uid_file_data_p->num_udpated = 0;
                SIMPB_LOG_D("EF_uid file_id = 0x%04X, EF_uid file_path = %s", uid_file_data_p->fid, uid_file_data_p->file_path);

                result = ste_uicc_sim_file_get_format((simpbd_record_p->simpbd_client_p)->ste_sim_p,
                                                      simpbd_record_get_sim_tag(simpbd_record_p),
                                                      uid_file_data_p->fid,
                                                      uid_file_data_p->file_path);

                SIMPB_LOG_D("ste_uicc_sim_file_get_format() = %d", result);

                if (UICC_REQUEST_STATUS_OK != result) {
                    SIMPB_LOG_E("ste_uicc_sim_file_get_format, result %d", result);
                    goto error;
                }

                simpbd_record_p->request_data_p = (void *) uid_file_data_p;
                simpbd_record_p->request_state = SIMPBD_REQUEST_STATE_STEP;
                simpbd_record_p->step_in_chain = SIMPBD_REQUEST_CHAIN_ADN_FILE_READ_UID_FILE_FORMAT;
                return STE_SIMPB_PENDING;
            } else {
                SIMPB_LOG_D("SYNC EF uid READ done.");
                simpbd_send_unsolicited(STE_SIMPB_CAUSE_UNSOLICITED_SIMPB_FILE_IDS, (void *)simpbd_get_adn_file_ids(), sizeof(simpb_file_ids_t));
                SIMPB_LOG_D("SUPPORTED PHONEBOOK = %d", simpbd_record_p->simpbd_client_p->supported_phonebooks);
                SIMPB_LOG_D("SELECTED PHONEBOOK = %d", simpbd_record_p->simpbd_client_p->selected_phonebook);
                SIMPB_LOG_D("CLIENT SELECTED PHONEBOOK = %d", simpbd_record_p->simpbd_client_p->client_selected_phonebook);
                simpbd_complete_request(simpbd_record_p, STE_SIMPB_SUCCESS, NULL, 0);
                return STE_SIMPB_SUCCESS;
            }
        }
        break;

        default:
            SIMPB_LOG_E("unhandled step_in_chain = %d", simpbd_record_p->step_in_chain);
            goto error;
        }
    }
    break;

    default:
        SIMPB_LOG_E("unhandled request_state = %d", simpbd_record_p->request_state);
        goto error;
    }

error:

    SIMPB_LOG_E("SYNC EF_ADN return failure.");
    simpbd_cache_flush_file_info();
    simpbd_complete_request(simpbd_record_p, STE_SIMPB_FAILURE, NULL, 0);
    return STE_SIMPB_FAILURE;
}


/**
 *  simpbd_handler_shutdown
 *
 *  This function handles the shutdown, intended for silent reset feature.
 *
 *  A successful completion means that the connection to the SIM daemon has
 *  been closed, the cat/uicc object is freed. To reconnect startup has to
 *  be invoked.
 */
ste_simpb_result_t simpbd_handler_shutdown(simpbd_record_t *simpbd_record_p)
{
    SIMPB_LOG_D("entered");

    if (SIMPBD_REQUEST_STATE_REQUEST == simpbd_record_p->request_state) { /* REQUEST PART */
        int result;

        /* Disconnect from the cat/uicc server. Note that the call is synchronous
         * although there will be a response (ignored) that the disconnect is completed. */
        result = ste_sim_disconnect((simpbd_record_p->simpbd_client_p)->ste_sim_p, 0);
        SIMPB_LOG_D("ste_sim_disconnect() = %d", result);

        if (STE_SIM_SUCCESS != result) {
            simpbd_translate_sim_request_status_to_simpbd_code(simpbd_record_p, result);
            SIMPB_LOG_E("ste_sim_disconnect, result %d", result);
            goto error;
        }

        /* Stop listening for messages on simd socket */
        fdmon_del((simpbd_record_p->simpbd_client_p)->fd_sim);
        (simpbd_record_p->simpbd_client_p)->fd_sim = -1;

        /* Delete the single threaded representation of the cat/uicc server */
        ste_sim_delete((simpbd_record_p->simpbd_client_p)->ste_sim_p, 0);
        SIMPB_LOG_D("ste_sim_delete()");
        (simpbd_record_p->simpbd_client_p)->ste_sim_p = NULL;

        simpbd_complete_request(simpbd_record_p, STE_SIMPB_SUCCESS, NULL, 0);
        return STE_SIMPB_SUCCESS;
    }

error:

    simpbd_complete_request(simpbd_record_p, STE_SIMPB_FAILURE, NULL, 0);
    return STE_SIMPB_FAILURE;
}


/**
 *  simpbd_handler_read
 *
 *  This function handles READ_RECORD. The client will get 1 record, if more records exists,
 *  the client will have to use GET_RESPONSE.
 *
 *  The function is called repeatedly until all records resulting from the read request
 *  has been collected and stored in the cache.
 *
 *    Initial read. If all is done (SW flags to indicate) respond to client.
 *    Consecutive reads. If all is done (SW flags to indicate) respond to client.
 *              Note that the uicc request function is called only once, and uicc will employ the
 *              closure call-back for as many record as it find related to the requested read.
 *
 */
ste_simpb_result_t simpbd_handler_read(simpbd_record_t *simpbd_record_p)
{
    ste_simpb_cb_read_data_t read_response_data;
    memset(&read_response_data, 0, sizeof(ste_simpb_cb_read_data_t));

    SIMPB_LOG_D("entered");

    if (SIMPBD_REQUEST_STATE_REQUEST == simpbd_record_p->request_state) { /* REQUEST PART */

        if (NULL != simpbd_record_p->request_data_p) {
            uicc_request_status_t result = UICC_REQUEST_STATUS_OK;
            sim_app_type_t app_type = simpbd_record_p->simpbd_client_p->app_type;
            sim_card_type_t card_type = simpbd_record_p->simpbd_client_p->card_type;
            ste_simpb_phonebook_type_t selected_phonebook = simpbd_record_p->simpbd_client_p->selected_phonebook;
            simpbd_request_read_t *simpbd_request_read_p = (simpbd_request_read_t *)simpbd_record_p->request_data_p;
            uint8_t record_id = 0;
            uint16_t fid = 0;
            char *path_p = NULL;

            simpbd_cache_get_parameters(simpbd_request_read_p->file_id,
                                        simpbd_request_read_p->instance,
                                        simpbd_request_read_p->slice,
                                        simpbd_request_read_p->record_id,
                                        simpbd_request_read_p->p2,
                                        selected_phonebook,
                                        app_type,
                                        card_type,
                                        &fid,
                                        &record_id,
                                        &path_p);

            if (0 == fid) {
                read_response_data.sim_iso_error = STE_SIMPB_ISO_ERROR_6A82_FILE_NOT_FOUND;
                read_response_data.sw1 = 0x6A;
                read_response_data.sw2 = 0x82;
                SIMPB_LOG_E("File id %d not found.", simpbd_request_read_p->file_id);
                simpbd_complete_request(simpbd_record_p, STE_SIMPB_SUCCESS, &read_response_data, sizeof(read_response_data));
                return STE_SIMPB_SUCCESS; /* the successful return is to communicate sw1 and sw2 to the client */
            }

            if (0 == record_id) {
                read_response_data.sim_iso_error = STE_SIMPB_ISO_ERROR_6A83_RECORD_NOT_FOUND;
                read_response_data.sw1 = 0x6A;
                read_response_data.sw2 = 0x83;
                SIMPB_LOG_E("Invalid record id %d", simpbd_request_read_p->record_id);
                simpbd_complete_request(simpbd_record_p, STE_SIMPB_SUCCESS, &read_response_data, sizeof(read_response_data));
                return STE_SIMPB_SUCCESS; /* the successful return is to communicate sw1 and sw2 to the client */
            }

            SIMPB_LOG_D("Calling READ_RECORD with \n ** file_id=0x%04X instance=%d slice=%d req_record_id=%d p2=%d length=%d path_p=%s resolved_record_id=%d",
                        fid,
                        simpbd_request_read_p->instance,
                        simpbd_request_read_p->slice,
                        simpbd_request_read_p->record_id,
                        simpbd_request_read_p->p2,
                        simpbd_request_read_p->length,
                        path_p,
                        record_id);

            result = ste_uicc_sim_file_read_record(simpbd_record_p->simpbd_client_p->ste_sim_p,
                                                   simpbd_record_get_sim_tag(simpbd_record_p),
                                                   fid,
                                                   record_id,
                                                   simpbd_request_read_p->length,
                                                   path_p);

            if (UICC_REQUEST_STATUS_OK != result) {
                simpbd_translate_sim_request_status_to_simpbd_code(simpbd_record_p, result);
                goto error;
            }

            simbpd_cache_set_current_record_id(simpbd_request_read_p->file_id, record_id);

            simpbd_record_p->request_state = SIMPBD_REQUEST_STATE_RESPONSE;
            return STE_SIMPB_PENDING;
        }
    } else {  /* RESPONSE PART */
        if (NULL != simpbd_record_p->response_data_p) {
            ste_simpb_cb_read_data_t *read_data_p = NULL;
            ste_uicc_sim_file_read_record_response_t *sim_read_data_p = (ste_uicc_sim_file_read_record_response_t *)simpbd_record_p->response_data_p;
            uint8_t sw1 = 0;
            uint8_t sw2 = 0;

            read_data_p = (ste_simpb_cb_read_data_t *)calloc(1, sizeof(ste_simpb_cb_read_data_t) + sim_read_data_p->length);

            if (0 != sim_read_data_p->length || NULL != sim_read_data_p->data) {
                memcpy(read_data_p->data_p, sim_read_data_p->data, sim_read_data_p->length);
                read_data_p->data_size = sim_read_data_p->length;
            }

            sw1 = sim_read_data_p->status_word.sw1;
            sw2 = sim_read_data_p->status_word.sw2;

            /* MAL bad status bytes fixup
             * if sw1 adn sw2  == 0 we assume we have "0x6A83 record not found"
             */
            if (sw1 == 0 && sw2 == 0) {
                sw1 = 0x6A;
                sw2 = 0x83;
            }

            read_data_p->sim_iso_error = simpbd_translate_status_bytes_to_iso_error(sw1, sw2);
            read_data_p->sw1 = sw1;
            read_data_p->sw2 = sw2;

            SIMPB_LOG_D("response data: data_size=%d  data_p=0x%08X sw1=0x%02X sw2=0x%02X sim_iso_error=%d\ndata:%s",
                        read_data_p->data_size, read_data_p->data_p, read_data_p->sw1, read_data_p->sw2, read_data_p->sim_iso_error, read_data_p->data_p);

            simpbd_complete_request(simpbd_record_p, STE_SIMPB_SUCCESS, read_data_p, sizeof(*read_data_p) + read_data_p->data_size);
            free(read_data_p);
            return STE_SIMPB_SUCCESS;
        }
    }

error:

    simpbd_complete_request(simpbd_record_p, STE_SIMPB_FAILURE, &read_response_data, sizeof(read_response_data));
    return STE_SIMPB_FAILURE;
}


/**
 *  simpbd_handler_update
 *
 *  This function handles UPDATE_RECORD. One record at a time.
 *
 */
ste_simpb_result_t simpbd_handler_update(simpbd_record_t *simpbd_record_p)
{
    ste_simpb_cb_update_data_t update_response_data;
    memset(&update_response_data, 0, sizeof(ste_simpb_cb_update_data_t));
    update_response_data.sim_iso_error = STE_SIMPB_ISO_ERROR_UNSPECIFIED;
    update_response_data.sw1 = 0x6F;
    update_response_data.sw2 = 0x00;

    SIMPB_LOG_D("entered");

    if (SIMPBD_REQUEST_STATE_REQUEST == simpbd_record_p->request_state) { /* REQUEST PART */
        if (NULL != simpbd_record_p->request_data_p) {
            uicc_request_status_t result = UICC_REQUEST_STATUS_OK;
            simpbd_request_update_t *simpbd_request_update_p =
                (simpbd_request_update_t *)simpbd_record_p->request_data_p;
            sim_app_type_t app_type = simpbd_record_p->simpbd_client_p->app_type;
            sim_card_type_t card_type = simpbd_record_p->simpbd_client_p->card_type;
            ste_simpb_phonebook_type_t selected_phonebook = simpbd_record_p->simpbd_client_p->selected_phonebook;
            uint8_t record_id = 0;
            uint16_t fid = 0;
            char *path_p = NULL;

            simpbd_cache_get_parameters(simpbd_request_update_p->file_id,
                                        simpbd_request_update_p->instance,
                                        simpbd_request_update_p->slice,
                                        simpbd_request_update_p->record_id,
                                        simpbd_request_update_p->p2,
                                        selected_phonebook,
                                        app_type,
                                        card_type,
                                        &fid,
                                        &record_id,
                                        &path_p);

            if (0 == fid) {
                update_response_data.sim_iso_error = STE_SIMPB_ISO_ERROR_6A82_FILE_NOT_FOUND;
                update_response_data.sw1 = 0x6A;
                update_response_data.sw2 = 0x82;
                SIMPB_LOG_E("File id %d not found.", simpbd_request_update_p->file_id);
                simpbd_complete_request(simpbd_record_p, STE_SIMPB_SUCCESS, &update_response_data, sizeof(update_response_data));
                return STE_SIMPB_SUCCESS; /* the successful return is to communicate sw1 and sw2 to the client */
            }

            if (0 == record_id) {
                update_response_data.sim_iso_error = STE_SIMPB_ISO_ERROR_6A83_RECORD_NOT_FOUND;
                update_response_data.sw1 = 0x6A;
                update_response_data.sw2 = 0x83;
                SIMPB_LOG_E("Invalid record id %d", simpbd_request_update_p->record_id);
                simpbd_complete_request(simpbd_record_p, STE_SIMPB_SUCCESS, &update_response_data, sizeof(update_response_data));
                return STE_SIMPB_SUCCESS; /* the successful return is to communicate sw1 and sw2 to the client */
            }

            SIMPB_LOG_D("Calling UPDATE_RECORD with \n ** file_id=0x%04X instance=%d req_record_id=%d p2=%d length=%d path_p=%s resolved_record_id=%d",
                        fid,
                        simpbd_request_update_p->instance,
                        simpbd_request_update_p->record_id,
                        simpbd_request_update_p->p2,
                        simpbd_request_update_p->length,
                        path_p,
                        record_id);

            result = ste_uicc_sim_file_update_record(simpbd_record_p->simpbd_client_p->ste_sim_p,
                     simpbd_record_get_sim_tag(simpbd_record_p),
                     fid,
                     record_id,
                     simpbd_request_update_p->length,
                     path_p,
                     simpbd_request_update_p->data);

            if (UICC_REQUEST_STATUS_OK != result) {
                simpbd_translate_sim_request_status_to_simpbd_code(simpbd_record_p, result);
                goto error;
            }

            simbpd_cache_set_current_record_id(simpbd_request_update_p->file_id, record_id);

            simpbd_record_p->request_state = SIMPBD_REQUEST_STATE_RESPONSE;
            return STE_SIMPB_PENDING;
        }
    } else {  /* RESPONSE PART */
        if (NULL != simpbd_record_p->response_data_p) {
            ste_uicc_update_sim_file_record_response_t *ste_uicc_update_sim_file_record_response_p =
                (ste_uicc_update_sim_file_record_response_t *)simpbd_record_p->response_data_p;

            uint8_t sw1 = ste_uicc_update_sim_file_record_response_p->status_word.sw1;
            uint8_t sw2 = ste_uicc_update_sim_file_record_response_p->status_word.sw2;

            /* MAL bad status bytes fixup
             * if sw1 adn sw2  == 0 we assume we have 0x6F00  "technical problem, no precise diagnosis"
             */
            if (sw1 == 0 && sw2 == 0) {
                sw1 = 0x6F;
                sw2 = 0x00;
            }

            update_response_data.sim_iso_error = simpbd_translate_status_bytes_to_iso_error(sw1, sw2);
            update_response_data.sw1 = sw1;
            update_response_data.sw2 = sw2;

            SIMPB_LOG_D("response data: result_code=%d  ext_code=%d sw1=0x%02X sw2=0x%02X",
                        (int)(ste_uicc_update_sim_file_record_response_p->uicc_status_code),
                        (int)(ste_uicc_update_sim_file_record_response_p->uicc_status_code_fail_details),
                        update_response_data.sw1, update_response_data.sw2);

            simpbd_complete_request(simpbd_record_p, STE_SIMPB_SUCCESS, &update_response_data, sizeof(update_response_data));
            return STE_SIMPB_SUCCESS;
        }
    }

error:

    simpbd_complete_request(simpbd_record_p, STE_SIMPB_FAILURE, &update_response_data, sizeof(update_response_data));
    return STE_SIMPB_FAILURE;
}


/**
 *  simpbd_handler_status_get
 *
 *  This function is returning the state of the SIM and the status of the simpbd daemon
 *
 */
ste_simpb_result_t simpbd_handler_status_get(simpbd_record_t *simpbd_record_p)
{
    SIMPB_LOG_D("entered");

    if (SIMPBD_REQUEST_STATE_REQUEST == simpbd_record_p->request_state) { /* REQUEST PART */
        if (NULL != simpbd_record_p->request_data_p) {
            uicc_request_status_t result = UICC_REQUEST_STATUS_OK;

            result = ste_uicc_sim_get_state(simpbd_record_p->simpbd_client_p->ste_sim_p,
                                            simpbd_record_get_sim_tag(simpbd_record_p));

            if (UICC_REQUEST_STATUS_OK != result) {
                goto error;
            }

            simpbd_record_p->request_state = SIMPBD_REQUEST_STATE_RESPONSE;
            return STE_SIMPB_PENDING;
        }
    } else {  /* RESPONSE PART */
        if (NULL != simpbd_record_p->response_data_p) {
            ste_simpb_cb_status_data_t status_data;
            ste_uicc_get_sim_state_response_t *ste_uicc_get_sim_state_response_p =
                (ste_uicc_get_sim_state_response_t *)simpbd_record_p->response_data_p;

            status_data.sim_state = ste_uicc_get_sim_state_response_p->state;
            status_data.status = simpbd_record_p->simpbd_client_p->state;

            simpbd_complete_request(simpbd_record_p, STE_SIMPB_SUCCESS, &status_data, sizeof(status_data));
            return STE_SIMPB_SUCCESS;
        }
    }

error:

    simpbd_complete_request(simpbd_record_p, STE_SIMPB_FAILURE, NULL, 0);
    return STE_SIMPB_FAILURE;
}


/**
 *  simpbd_handler_file_information_get
 *
 *  This function handles GET INFORMATION (STATUS) of a file.
 *
 */
ste_simpb_result_t simpbd_handler_file_information_get(simpbd_record_t *simpbd_record_p)
{
    ste_simpb_cb_file_information_data_t file_information_response_data;
    memset(&file_information_response_data, 0, sizeof(ste_simpb_cb_file_information_data_t));
    file_information_response_data.sim_iso_error = STE_SIMPB_ISO_ERROR_UNSPECIFIED;
    file_information_response_data.sw1 = 0x6F;
    file_information_response_data.sw2 = 0x00;

    SIMPB_LOG_D("entered");

    if (SIMPBD_REQUEST_STATE_REQUEST == simpbd_record_p->request_state) { /* REQUEST PART */
        if (NULL != simpbd_record_p->request_data_p) {
            uicc_request_status_t result = UICC_REQUEST_STATUS_OK;
            simpbd_request_file_information_get_t *request_file_information_p =
                (simpbd_request_file_information_get_t *)simpbd_record_p->request_data_p;

            sim_app_type_t app_type = simpbd_record_p->simpbd_client_p->app_type;
            sim_card_type_t card_type = simpbd_record_p->simpbd_client_p->card_type;
            ste_simpb_phonebook_type_t selected_phonebook = simpbd_record_p->simpbd_client_p->selected_phonebook;
            ste_simpb_file_type_t file_id = request_file_information_p->file_id; /* file_id is the client designation of the file. Fid id simpbd's */

            uint16_t fid = simpbd_cache_get_fid_from_client_file_id(file_id,
                           request_file_information_p->instance,
                           request_file_information_p->slice,
                           selected_phonebook,
                           app_type);

            char *path_p = simpbd_cache_get_path_on_selected_phonebook(selected_phonebook, app_type, card_type, file_id);

            SIMPB_LOG_D("Calling GET FILE INFO with \n ** file_id=0x%04X instance=%d slice=%d path_p=%s",
                        fid, request_file_information_p->instance, request_file_information_p->slice, path_p);

            if (0 == fid) {
                /* we failed to locate the file, return "file not found" but it is not an error */
                file_information_response_data.sim_iso_error = STE_SIMPB_ISO_NORMAL_COMPLETION;
                file_information_response_data.sw1 = 0x6A;
                file_information_response_data.sw2 = 0x82;
                simpbd_complete_request(simpbd_record_p, STE_SIMPB_SUCCESS, &file_information_response_data, sizeof(ste_simpb_cb_file_information_data_t));
                return STE_SIMPB_SUCCESS;
            }

            result = ste_uicc_sim_get_file_information(simpbd_record_p->simpbd_client_p->ste_sim_p,
                     simpbd_record_get_sim_tag(simpbd_record_p),
                     fid,
                     path_p,
                     STE_UICC_SIM_GET_FILE_INFO_TYPE_EF);

            if (UICC_REQUEST_STATUS_OK != result) {
                simpbd_translate_sim_request_status_to_simpbd_code(simpbd_record_p, result);
                goto error;
            }

            simpbd_record_p->request_state = SIMPBD_REQUEST_STATE_RESPONSE;
            return STE_SIMPB_PENDING;
        }
    } else {  /* RESPONSE PART */
        if (NULL != simpbd_record_p->response_data_p) {
            ste_simpb_cb_file_information_data_t *file_information_data_p = NULL;
            ste_uicc_get_file_information_response_t *sim_file_information_p =
                (ste_uicc_get_file_information_response_t *)simpbd_record_p->response_data_p;

            uint8_t sw1 = sim_file_information_p->status_word.sw1;
            uint8_t sw2 = sim_file_information_p->status_word.sw2;

            file_information_data_p = (ste_simpb_cb_file_information_data_t *)calloc(1, sizeof(ste_simpb_cb_file_information_data_t) + sim_file_information_p->length);

            if (0 < sim_file_information_p->length && NULL != sim_file_information_p->fcp) {
                memcpy(file_information_data_p->data_p, sim_file_information_p->fcp, sim_file_information_p->length);
                file_information_data_p->data_size = sim_file_information_p->length;
            }

            file_information_data_p->sim_iso_error = simpbd_translate_status_bytes_to_iso_error(sw1, sw2);
            file_information_data_p->sw1 = sw1;
            file_information_data_p->sw2 = sw2;

            SIMPB_LOG_D("response data: data_size=%d  data_p=0x%08X sw1=0x%02X sw2=0x%02X sim_iso_error=%d\ndata:%s",
                        sim_file_information_p->length, sim_file_information_p->fcp, file_information_data_p->sw1,
                        file_information_data_p->sw2, file_information_data_p->sim_iso_error, sim_file_information_p->fcp);

            simpbd_complete_request(simpbd_record_p, STE_SIMPB_SUCCESS, file_information_data_p,
                                    sizeof(ste_simpb_cb_file_information_data_t) + file_information_data_p->data_size);
            free(file_information_data_p);
            return STE_SIMPB_SUCCESS;
        }
    }

error:

    simpbd_complete_request(simpbd_record_p, STE_SIMPB_FAILURE, &file_information_response_data, sizeof(ste_simpb_cb_file_information_data_t));
    return STE_SIMPB_FAILURE;
}


/**
 *  simpbd_handler_phonebook_select
 *
 *  This function handles PHONEBOOK_SELECT.
 *  STE_SIMPB_PHONEBOOK_TYPE_GLOBAL
 *  STE_SIMPB_PHONEBOOK_TYPE_LOCAL
 *  STE_SIMPB_PHONEBOOK_TYPE_DEFAULT
 *
 */
ste_simpb_result_t simpbd_handler_phonebook_select(simpbd_record_t *simpbd_record_p)
{
    SIMPB_LOG_D("request_state=%d,response_data_p=%p",
                simpbd_record_p->request_state,
                simpbd_record_p->response_data_p);

    switch (simpbd_record_p->request_state) {
    case SIMPBD_REQUEST_STATE_REQUEST: {
        if (NULL != simpbd_record_p->request_data_p) {
            uicc_request_status_t result = UICC_REQUEST_STATUS_OK;
            simpbd_request_phonebook_select_t *simpbd_request_phonebook_select_p =
                (simpbd_request_phonebook_select_t *)simpbd_record_p->request_data_p;
            ste_simpb_phonebook_type_t selected_phonebook = simpbd_request_phonebook_select_p->selected_phonebook;
            uint8_t supported_phonebooks = simpbd_record_p->simpbd_client_p->supported_phonebooks;

            simpbd_record_p->simpbd_client_p->client_selected_phonebook = selected_phonebook;

            if (STE_SIMPB_PHONEBOOK_TYPE_DEFAULT == selected_phonebook) {
                if (supported_phonebooks & STE_SIMPB_PHONEBOOK_TYPE_GLOBAL) {
                    selected_phonebook = STE_SIMPB_PHONEBOOK_TYPE_GLOBAL;
                } else if (supported_phonebooks & STE_SIMPB_PHONEBOOK_TYPE_LOCAL) {
                    selected_phonebook = STE_SIMPB_PHONEBOOK_TYPE_LOCAL;
                } else {/* This is a not a wanted situation! Force app to GSM and set GLOBAL */
                    SIMPB_LOG_E("Have UICC but no DF_Phonebook in DF_Telecom or ADF_USIM. Forcing ICC and GSM type application.");
                    SIMPB_LOG_E("Assume SIM application will have one available soon. AP selected. Supported is set to none.");

                    selected_phonebook = STE_SIMPB_PHONEBOOK_TYPE_LOCAL;
                }
            } else if (!(selected_phonebook & supported_phonebooks)) {
                SIMPB_LOG_E("Phone book not supported. supported_phonebooks=0x%02X selected_phonebook=0x%02X",
                            simpbd_record_p->simpbd_client_p->supported_phonebooks, selected_phonebook);
                goto error;
            }

            if (simpbd_record_p->simpbd_client_p->selected_phonebook == selected_phonebook) {
                simpbd_complete_request(simpbd_record_p, STE_SIMPB_SUCCESS, NULL, 0);
                return STE_SIMPB_SUCCESS;
            } /* If surrent phone book is same as requested, do nothing. */

            simpbd_record_p->simpbd_client_p->selected_phonebook = selected_phonebook;

            SIMPB_LOG_D("Supported_phonebooks=0x%02X selected_phonebook=0x%02X",
                        simpbd_record_p->simpbd_client_p->supported_phonebooks, selected_phonebook);

            sim_app_type_t app_type = simpbd_record_p->simpbd_client_p->app_type;
            sim_card_type_t card_type = simpbd_record_p->simpbd_client_p->card_type;

            simpbd_record_p->simpbd_client_p->selected_phonebook = selected_phonebook;

            SIMPB_LOG_D("get_fid = 0x%04X", simpbd_cache_get_fid_from_client_file_id(STE_SIMPB_FILE_TYPE_PBR, 0, 1, selected_phonebook, app_type));
            SIMPB_LOG_D("get_path = %s", simpbd_cache_get_path_on_selected_phonebook(selected_phonebook, app_type, card_type, STE_SIMPB_FILE_TYPE_PBR));

            if (SIM_APP_GSM != app_type) {
                result = ste_uicc_sim_file_read_record((simpbd_record_p->simpbd_client_p)->ste_sim_p,
                                                       simpbd_record_get_sim_tag(simpbd_record_p),
                                                       simpbd_cache_get_fid_from_client_file_id(STE_SIMPB_FILE_TYPE_PBR, 0, 1, selected_phonebook, app_type),
                                                       1,
                                                       0,
                                                       simpbd_cache_get_path_on_selected_phonebook(selected_phonebook, app_type, card_type, STE_SIMPB_FILE_TYPE_PBR));

                SIMPB_LOG_D("ste_uicc_sim_file_read_record() = %d", result);

                if (UICC_REQUEST_STATUS_OK != result) {
                    SIMPB_LOG_E("ste_uicc_sim_file_read_binary, result %d", result);
                    goto error;
                }

                simpbd_record_p->private_data_p = (void *)1;

                simpbd_record_p->request_state = SIMPBD_REQUEST_STATE_STEP_CLEAR_PBR_CACHE;
                return STE_SIMPB_PENDING;

            } else {
                simpbd_complete_request(simpbd_record_p, STE_SIMPB_SUCCESS, NULL, 0);
                return STE_SIMPB_SUCCESS;
            }
        }

        break;
    }

    case SIMPBD_REQUEST_STATE_STEP_CLEAR_PBR_CACHE:
        simpbd_cache_flush_pbr_data();
        simpbd_cache_flush_file_info();
        simpbd_record_p->request_state = SIMPBD_REQUEST_STATE_STEP;
        /* Fall through intentional */

    case SIMPBD_REQUEST_STATE_STEP: {
        ste_simpb_result_t result;
        int slice = (int)(simpbd_record_p->private_data_p);

        if (NULL == simpbd_record_p->response_data_p) {
            SIMPB_LOG_E("ste_uicc_sim_file_read_record, no response data!");
            goto error;
        }

        result = simpbd_create_and_store_pbr_cache(
                     (ste_uicc_sim_file_read_record_response_t *)simpbd_record_p->response_data_p,
                     slice);

        if (STE_SIMPB_SUCCESS == result) {
            sim_app_type_t app_type = simpbd_record_p->simpbd_client_p->app_type;
            sim_card_type_t card_type = simpbd_record_p->simpbd_client_p->card_type;
            ste_simpb_phonebook_type_t selected_phonebook = simpbd_record_p->simpbd_client_p->selected_phonebook;
            uicc_request_status_t result = UICC_REQUEST_STATUS_OK;

            slice++;

            SIMPB_LOG_D("fid = 0x%04X", simpbd_cache_get_fid_from_client_file_id(STE_SIMPB_FILE_TYPE_PBR, 0, 1, selected_phonebook, app_type));
            SIMPB_LOG_D("path = %s", simpbd_cache_get_path_on_selected_phonebook(selected_phonebook, app_type, card_type, STE_SIMPB_FILE_TYPE_PBR));
            SIMPB_LOG_D("slice = %d", slice);

            result = ste_uicc_sim_file_read_record((simpbd_record_p->simpbd_client_p)->ste_sim_p,
                                                   simpbd_record_get_sim_tag(simpbd_record_p),
                                                   simpbd_cache_get_fid_from_client_file_id(STE_SIMPB_FILE_TYPE_PBR, 0, 1, selected_phonebook, app_type),
                                                   slice, /* Note that slice in PBR context is the record in PBR */
                                                   0,
                                                   simpbd_cache_get_path_on_selected_phonebook(selected_phonebook, app_type, card_type, STE_SIMPB_FILE_TYPE_PBR));

            SIMPB_LOG_D("ste_uicc_sim_file_read_record() = %d iteration %d", result, slice);

            if (UICC_REQUEST_STATUS_OK != result) {
                SIMPB_LOG_E("ste_uicc_sim_file_read_record, result %d", result);
                goto error;
            }

            simpbd_record_p->private_data_p = (void *)slice;
            return STE_SIMPB_PENDING;

        } else {
            /* if we got an error from trying to add PBR data from previous read, then there are no more records for us */
            simpbd_complete_request(simpbd_record_p, STE_SIMPB_SUCCESS, NULL, 0);
            return STE_SIMPB_SUCCESS;
        }

        break;
    }

    default:
        SIMPB_LOG_E("unhandled request_state = %d", simpbd_record_p->request_state);
        goto error;
    }

error:

    simpbd_complete_request(simpbd_record_p, STE_SIMPB_FAILURE, NULL, 0);
    return STE_SIMPB_FAILURE;
}

/**
 *  simpbd_handler_selected_phonebook_get
 *
 *  This function returns the selected phonebook
 *
 */
ste_simpb_result_t simpbd_handler_selected_phonebook_get(simpbd_record_t *simpbd_record_p)
{
    ste_simpb_phonebook_type_t selected_phonebook = simpbd_record_p->simpbd_client_p->selected_phonebook;

    SIMPB_LOG_D("entered");
    ste_simpb_cb_selected_phonebook_data_t selected_phonebook_data;

    selected_phonebook_data.sim_iso_error = STE_SIMPB_ISO_NORMAL_COMPLETION;
    selected_phonebook_data.selected_phonebook = simpbd_record_p->simpbd_client_p->selected_phonebook;

    simpbd_complete_request(simpbd_record_p, STE_SIMPB_SUCCESS, &selected_phonebook_data, sizeof(selected_phonebook_data));
    return STE_SIMPB_SUCCESS;
}

/**
 *  simpbd_handler_supported_phonebooks_get
 *
 *  This function handles UPDATE_RECORD. One record at a time.
 *
 */
ste_simpb_result_t simpbd_handler_supported_phonebooks_get(simpbd_record_t *simpbd_record_p)
{
    SIMPB_LOG_D("entered");

    /*
     * This handler does not involve simd. It set the selected phone book internally in simpbd.
     */
    if (NULL != simpbd_record_p->request_data_p) {
        ste_simpb_cb_supported_phonebooks_data_t supported_phonebooks_data;

        supported_phonebooks_data.sim_iso_error = STE_SIMPB_ISO_NORMAL_COMPLETION;
        supported_phonebooks_data.supported_phonebooks = simpbd_record_p->simpbd_client_p->supported_phonebooks;

        SIMPB_LOG_D("supported_phonebooks=%d, sim_iso_error%d",
                    supported_phonebooks_data.supported_phonebooks, supported_phonebooks_data.sim_iso_error);

        simpbd_complete_request(simpbd_record_p, STE_SIMPB_SUCCESS, &supported_phonebooks_data, sizeof(supported_phonebooks_data));
        return STE_SIMPB_SUCCESS;
    }

    simpbd_complete_request(simpbd_record_p, STE_SIMPB_FAILURE, NULL, 0);
    return STE_SIMPB_FAILURE;
}

/**
 *  simpbd_handler_sync_ef_pbc
 *
 *  This function is to sync the EF PBC file and also update EF CC file if needed
 *
 */
ste_simpb_result_t simpbd_handler_sync_ef_pbc(simpbd_record_t *simpbd_record_p)
{
    sim_app_type_t app_type = simpbd_record_p->simpbd_client_p->app_type;
    sim_card_type_t card_type = simpbd_record_p->simpbd_client_p->card_type;
    ste_simpb_phonebook_type_t selected_phonebook = simpbd_record_p->simpbd_client_p->selected_phonebook;
    uint8_t slice = (uint8_t)(0x000000ff & (uint32_t)simpbd_record_p->request_data_p); /* get current slice in PBR */
    SIMPB_LOG_D("card_type=%d, app_type=%d, seleced pb=%p", card_type, app_type, selected_phonebook);

    //SIMPB_LOG_D("request_state=%d,response_data_p=%p",
    //            simpbd_record_p->request_state,
    //            simpbd_record_p->response_data_p);

    switch (simpbd_record_p->request_state) {
    case SIMPBD_REQUEST_STATE_REQUEST: {
        simpbd_sync_ef_pbc_t *sync_data_p = NULL;
        uicc_request_status_t result = UICC_REQUEST_STATUS_OK;
        sync_data_p = (simpbd_sync_ef_pbc_t *)calloc(1, sizeof(simpbd_sync_ef_pbc_t));

        if (sync_data_p == NULL) {
            SIMPB_LOG_E("memory allocation failed.");
            goto error;
        }

        simpbd_record_p->request_data_p = sync_data_p;
        SIMPB_LOG_D("Get EF_PBC file_id from EF_PBR...");
        sync_data_p->file_id = simpbd_cache_get_fid_from_client_file_id(STE_SIMPB_FILE_TYPE_PBC, 0, slice, selected_phonebook, app_type);
        sync_data_p->file_path = simpbd_cache_get_path_on_selected_phonebook(selected_phonebook, app_type, card_type, STE_SIMPB_FILE_TYPE_PBC);

        sync_data_p->num_records = 0;
        sync_data_p->index = 0;
        sync_data_p->num_udpated = 0;

        SIMPB_LOG_D("EF_PBC file_id = 0x%04X", sync_data_p->file_id);
        SIMPB_LOG_D("EF_PBC file_path = %s", sync_data_p->file_path);

        result = ste_uicc_sim_file_get_format((simpbd_record_p->simpbd_client_p)->ste_sim_p,
                                              simpbd_record_get_sim_tag(simpbd_record_p),
                                              sync_data_p->file_id,
                                              sync_data_p->file_path);

        SIMPB_LOG_D("ste_uicc_sim_file_get_format() = %d", result);

        if (UICC_REQUEST_STATUS_OK != result) {
            SIMPB_LOG_E("ste_uicc_sim_file_get_format, result %d", result);
            goto error;
        }

        simpbd_record_p->request_state = SIMPBD_REQUEST_STATE_STEP;
        simpbd_record_p->step_in_chain = SIMPBD_REQUEST_CHAIN_GET_FORMAT;
        return STE_SIMPB_PENDING;
    }
    break;

    case SIMPBD_REQUEST_STATE_STEP: {
        switch (simpbd_record_p->step_in_chain) {
        case SIMPBD_REQUEST_CHAIN_GET_FORMAT: {
            simpbd_sync_ef_pbc_t *sync_data_p = (simpbd_sync_ef_pbc_t *)(simpbd_record_p->request_data_p);
            uicc_request_status_t result = UICC_REQUEST_STATUS_OK;
            ste_uicc_sim_file_get_format_response_t *file_format_p =
                (ste_uicc_sim_file_get_format_response_t *)simpbd_record_p->response_data_p;

            if (NULL == simpbd_record_p->response_data_p) {
                SIMPB_LOG_E("simpbd_record_p->response_data_p NULL");
                goto error;
            }

            if (NULL == sync_data_p) {
                SIMPB_LOG_E("sync_data_p is NULL");
                goto error;
            }

            if (STE_UICC_STATUS_CODE_OK != file_format_p->uicc_status_code) {
                SIMPB_LOG_E("ste_uicc_sim_file_get_format failed for EF_PBC, status code %d, status word 0x%x, 0x%x", file_format_p->uicc_status_code,
                            file_format_p->status_word.sw1, file_format_p->status_word.sw2);
                goto error;
            }

            SIMPB_LOG_D("EF_PBC num_records = 0x%04X", file_format_p->num_records);
            SIMPB_LOG_D("EF_PBC record_len = %d", file_format_p->record_len);
            sync_data_p->num_records = file_format_p->num_records;
            sync_data_p->record_len = file_format_p->record_len;

            if (file_format_p->num_records == 0) {
                //SIMPB_LOG_D("EF PBC has no records, sync is not needed.");

                if (simpbd_record_p->request_data_p) {
                    free(simpbd_record_p->request_data_p);
                    simpbd_record_p->request_data_p = NULL;
                }

                simpbd_complete_request(simpbd_record_p, STE_SIMPB_SUCCESS, NULL, 0);
                return STE_SIMPB_SUCCESS;
            }

            // read first record
            sync_data_p->index = 1;
            SIMPB_LOG_D("EF_PBC read record: %d", sync_data_p->index);

            result = ste_uicc_sim_file_read_record((simpbd_record_p->simpbd_client_p)->ste_sim_p,
                                                   simpbd_record_get_sim_tag(simpbd_record_p),
                                                   sync_data_p->file_id,
                                                   sync_data_p->index,
                                                   sync_data_p->record_len,
                                                   sync_data_p->file_path);

            if (UICC_REQUEST_STATUS_OK != result) {
                SIMPB_LOG_E("ste_uicc_sim_file_read_record, result %d", result);
                goto error;
            }

            simpbd_record_p->request_state = SIMPBD_REQUEST_STATE_STEP;
            simpbd_record_p->step_in_chain = SIMPBD_REQUEST_CHAIN_READ_EF_PBC;
            return STE_SIMPB_PENDING;
        }
        break;

        case SIMPBD_REQUEST_CHAIN_READ_EF_PBC: {
            simpbd_sync_ef_pbc_t *sync_data_p = (simpbd_sync_ef_pbc_t *)(simpbd_record_p->request_data_p);
            uicc_request_status_t result = UICC_REQUEST_STATUS_OK;
            ste_uicc_sim_file_read_record_response_t *read_record_resp_p =
                (ste_uicc_sim_file_read_record_response_t *)simpbd_record_p->response_data_p;

            if (NULL == simpbd_record_p->response_data_p) {
                SIMPB_LOG_E("simpbd_record_p->response_data_p NULL");
                goto error;
            }

            if (NULL == sync_data_p) {
                SIMPB_LOG_E("sync_data_p is NULL");
                goto error;
            }

            if (STE_UICC_STATUS_CODE_OK != read_record_resp_p->uicc_status_code) {
                SIMPB_LOG_E("ste_uicc_sim_file_read_record(%d) failed for EF_PBC, status code %d, status word 0x%x, 0x%x", sync_data_p->index, read_record_resp_p->uicc_status_code,
                            read_record_resp_p->status_word.sw1, read_record_resp_p->status_word.sw2);
                goto error;
            }

            SIMPB_LOG_D("EF_PBC record[%d] = 0x%x", sync_data_p->index, *(read_record_resp_p->data));
            SIMPB_LOG_D("EF_PBC number of records to be updated: %d", sync_data_p->num_udpated);

            //check if the value has been updated
            if (*(read_record_resp_p->data) == 0x01) {
                uint8_t update_data[2];

                //update this record
                sync_data_p->num_udpated++;
                update_data[0] = 0x00;
                update_data[1] = *(read_record_resp_p->data + 1);
                SIMPB_LOG_D("EF_PBC update record: %d", sync_data_p->index);

                result = ste_uicc_sim_file_update_record(simpbd_record_p->simpbd_client_p->ste_sim_p,
                         simpbd_record_get_sim_tag(simpbd_record_p),
                         sync_data_p->file_id,
                         sync_data_p->index,
                         sync_data_p->record_len,
                         sync_data_p->file_path,
                         update_data);

                if (UICC_REQUEST_STATUS_OK != result) {
                    SIMPB_LOG_E("ste_uicc_sim_file_update_record, result %d", result);
                    goto error;
                }

                simpbd_record_p->request_state = SIMPBD_REQUEST_STATE_STEP;
                simpbd_record_p->step_in_chain = SIMPBD_REQUEST_CHAIN_UPDATE_EF_PBC;
                return STE_SIMPB_PENDING;
            } else {
                //read next record
                if (sync_data_p->num_records > sync_data_p->index) {
                    sync_data_p->index++;
                    SIMPB_LOG_E("EF_PBC read record: %d", sync_data_p->index);

                    result = ste_uicc_sim_file_read_record((simpbd_record_p->simpbd_client_p)->ste_sim_p,
                                                           simpbd_record_get_sim_tag(simpbd_record_p),
                                                           sync_data_p->file_id,
                                                           sync_data_p->index,
                                                           sync_data_p->record_len,
                                                           sync_data_p->file_path);

                    if (UICC_REQUEST_STATUS_OK != result) {
                        SIMPB_LOG_E("ste_uicc_sim_file_read_record, result %d", result);
                        goto error;
                    }

                    simpbd_record_p->request_state = SIMPBD_REQUEST_STATE_STEP;
                    simpbd_record_p->step_in_chain = SIMPBD_REQUEST_CHAIN_READ_EF_PBC;
                    return STE_SIMPB_PENDING;
                } else {
                    sim_app_type_t app_type = simpbd_record_p->simpbd_client_p->app_type;

                    //reach the end of file, check if update EF CC is needed
                    if (sync_data_p->num_udpated == 0) {
                        SIMPB_LOG_D("NO NEED to update EF_CC.");

                        if (simpbd_record_p->request_data_p) {
                            free(simpbd_record_p->request_data_p);
                            simpbd_record_p->request_data_p = NULL;
                        }

                        simpbd_complete_request(simpbd_record_p, STE_SIMPB_SUCCESS, NULL, 0);
                        return STE_SIMPB_SUCCESS;
                    }

                    //now update EF CC, read data from EF CC first
                    result = ste_uicc_sim_file_read_binary((simpbd_record_p->simpbd_client_p)->ste_sim_p,
                                                           simpbd_record_get_sim_tag(simpbd_record_p),
                                                           simpbd_ef_cc_id,
                                                           0,
                                                           2,
                                                           simpbd_cache_get_ef_path_phonebook(selected_phonebook, app_type));

                    if (UICC_REQUEST_STATUS_OK != result) {
                        SIMPB_LOG_E("ste_uicc_sim_file_read_binary, result %d", result);
                        goto error;
                    }

                    simpbd_record_p->request_state = SIMPBD_REQUEST_STATE_STEP;
                    simpbd_record_p->step_in_chain = SIMPBD_REQUEST_CHAIN_READ_EF_CC;
                    return STE_SIMPB_PENDING;
                }
            }
        }
        break;

        case SIMPBD_REQUEST_CHAIN_UPDATE_EF_PBC: {
            simpbd_sync_ef_pbc_t *sync_data_p = (simpbd_sync_ef_pbc_t *)(simpbd_record_p->request_data_p);
            uicc_request_status_t result = UICC_REQUEST_STATUS_OK;
            ste_uicc_update_sim_file_record_response_t *update_record_resp_p =
                (ste_uicc_update_sim_file_record_response_t *)simpbd_record_p->response_data_p;

            if (NULL == simpbd_record_p->response_data_p) {
                SIMPB_LOG_E("simpbd_record_p->response_data_p NULL");
                goto error;
            }

            if (NULL == sync_data_p) {
                SIMPB_LOG_E("sync_data_p is NULL");
                goto error;
            }

            if (STE_UICC_STATUS_CODE_OK != update_record_resp_p->uicc_status_code) {
                SIMPB_LOG_E("ste_uicc_sim_file_update_record(%d) failed for EF_PBC, status code %d, status word 0x%x, 0x%x", sync_data_p->index, update_record_resp_p->uicc_status_code,
                            update_record_resp_p->status_word.sw1, update_record_resp_p->status_word.sw2);
                goto error;
            }

            SIMPB_LOG_D("Selected App Type = %d", app_type);
            SIMPB_LOG_D("Selected PB Type = %d", simpbd_record_p->simpbd_client_p->supported_phonebooks);
            SIMPB_LOG_D("Selected Path = %s", simpbd_cache_get_ef_path_phonebook(selected_phonebook, app_type));

            //read next record
            if (sync_data_p->num_records > sync_data_p->index) {
                sync_data_p->index++;
                //SIMPB_LOG_D("EF_PBC read record: %d", sync_data_p->index);

                result = ste_uicc_sim_file_read_record((simpbd_record_p->simpbd_client_p)->ste_sim_p,
                                                       simpbd_record_get_sim_tag(simpbd_record_p),
                                                       sync_data_p->file_id,
                                                       sync_data_p->index,
                                                       sync_data_p->record_len,
                                                       sync_data_p->file_path);

                if (UICC_REQUEST_STATUS_OK != result) {
                    SIMPB_LOG_E("ste_uicc_sim_file_read_record, result %d", result);
                    goto error;
                }

                simpbd_record_p->request_state = SIMPBD_REQUEST_STATE_STEP;
                simpbd_record_p->step_in_chain = SIMPBD_REQUEST_CHAIN_READ_EF_PBC;
                return STE_SIMPB_PENDING;
            } else {
                sim_app_type_t app_type = simpbd_record_p->simpbd_client_p->app_type;

                //reach the end of file, check if update EF CC is needed
                //now update EF CC, read data from EF CC first

                result = ste_uicc_sim_file_read_binary((simpbd_record_p->simpbd_client_p)->ste_sim_p,
                                                       simpbd_record_get_sim_tag(simpbd_record_p),
                                                       simpbd_ef_cc_id,
                                                       0,
                                                       2,
                                                       simpbd_cache_get_ef_path_phonebook(selected_phonebook, app_type));

                if (UICC_REQUEST_STATUS_OK != result) {
                    SIMPB_LOG_E("ste_uicc_sim_file_read_binary, result %d", result);
                    goto error;
                }

                simpbd_record_p->request_state = SIMPBD_REQUEST_STATE_STEP;
                simpbd_record_p->step_in_chain = SIMPBD_REQUEST_CHAIN_READ_EF_CC;
                return STE_SIMPB_PENDING;
            }
        }
        break;

        case SIMPBD_REQUEST_CHAIN_READ_EF_CC: {
            simpbd_sync_ef_pbc_t *sync_data_p = (simpbd_sync_ef_pbc_t *)(simpbd_record_p->request_data_p);
            uicc_request_status_t result = UICC_REQUEST_STATUS_OK;
            ste_uicc_sim_file_read_binary_response_t *read_binary_resp_p =
                (ste_uicc_sim_file_read_binary_response_t *)simpbd_record_p->response_data_p;
            sim_app_type_t app_type = simpbd_record_p->simpbd_client_p->app_type;
            uint8_t update_data[2];

            if (NULL == simpbd_record_p->response_data_p) {
                SIMPB_LOG_E("simpbd_record_p->response_data_p NULL");
                goto error;
            }

            if (NULL == sync_data_p) {
                SIMPB_LOG_E("sync_data_p is NULL");
                goto error;
            }

            if (STE_UICC_STATUS_CODE_OK != read_binary_resp_p->uicc_status_code) {
                SIMPB_LOG_E("ste_uicc_sim_file_read_binary failed for EF_CC, status code %d, status word 0x%x, 0x%x", read_binary_resp_p->uicc_status_code,
                            read_binary_resp_p->status_word.sw1, read_binary_resp_p->status_word.sw2);
                goto error;
            }

            simpb_cc_value = ((uint16_t) * (read_binary_resp_p->data)) << 8;
            simpb_cc_value = ((uint16_t) * (read_binary_resp_p->data + 1)) | simpb_cc_value;

            //simpb_cc_value = *(read_binary_resp_p->data) * 256 + *(read_binary_resp_p->data + 1);

            SIMPB_LOG_D("EF_CC value: 0x%x", simpb_cc_value);

            //simpb_cc_value = simpb_cc_value + sync_data_p->num_udpated;
            simpb_p_cc_value = simpb_cc_value;
            uint8_t psc_update = FALSE;

            if ((0xFFFE - simpb_cc_value) >= sync_data_p->num_udpated) {
                simpb_cc_value = simpb_cc_value + sync_data_p->num_udpated;
                psc_update = FALSE;
            } else {
                simpb_cc_value  = sync_data_p->num_udpated - (0xFFFE - simpb_cc_value);
                psc_update = TRUE;
            }

            SIMPB_LOG_D("EF_CC value updated: 0x%x", simpb_cc_value);

            if (psc_update == TRUE) {
                (void)simpbd_execute(simpbd_record_p->simpbd_client_p, STE_SIMPB_INTERNAL_REQUEST_ID_UPDATE_PSC, NULL, NULL);
            }

            update_data[0] = ((simpb_cc_value >> 8) & 0xFF);
            update_data[1] = ((simpb_cc_value) & 0xFF);
            SIMPB_LOG_D("EF_CC udpate to: 0x%x 0x%x", update_data[0], update_data[1]);

            result = ste_uicc_sim_file_update_binary(simpbd_record_p->simpbd_client_p->ste_sim_p,
                     simpbd_record_get_sim_tag(simpbd_record_p),
                     simpbd_ef_cc_id,
                     0,
                     2,
                     simpbd_cache_get_ef_path_phonebook(selected_phonebook, app_type),
                     update_data);

            if (UICC_REQUEST_STATUS_OK != result) {
                SIMPB_LOG_E("ste_uicc_sim_file_update_binary, result %d", result);
                goto error;
            }

            simpbd_record_p->request_state = SIMPBD_REQUEST_STATE_STEP;
            simpbd_record_p->step_in_chain = SIMPBD_REQUEST_CHAIN_UPDATE_EF_CC;
            return STE_SIMPB_PENDING;
        }
        break;

        case SIMPBD_REQUEST_CHAIN_UPDATE_EF_CC: {
            ste_uicc_update_sim_file_binary_response_t *update_binary_resp_p =
                (ste_uicc_update_sim_file_binary_response_t *)simpbd_record_p->response_data_p;

            if (NULL == simpbd_record_p->response_data_p) {
                SIMPB_LOG_E("simpbd_record_p->response_data_p NULL");
                simpb_cc_value = simpb_p_cc_value;
                goto error;
            }

            if (STE_UICC_STATUS_CODE_OK != update_binary_resp_p->uicc_status_code) {
                SIMPB_LOG_E("ste_uicc_sim_file_update_binary failed for EF_CC, status code %d", update_binary_resp_p->uicc_status_code);
                simpb_cc_value = simpb_p_cc_value;
                goto error;
            }

            if (simpbd_record_p->request_data_p) {
                free(simpbd_record_p->request_data_p);
                simpbd_record_p->request_data_p = NULL;
            }

            SIMPB_LOG_D("SYNC EF_PBC done.");
            simpbd_complete_request(simpbd_record_p, STE_SIMPB_SUCCESS, NULL, 0);
            return STE_SIMPB_SUCCESS;
        }
        break;

        default:
            SIMPB_LOG_E("unhandled step_in_chain = %d", simpbd_record_p->step_in_chain);
            goto error;
        }

        return STE_SIMPB_PENDING;
    }
    break;

    default:
        SIMPB_LOG_E("unhandled request_state = %d", simpbd_record_p->request_state);
        goto error;
    }

error:

    if (simpbd_record_p->request_data_p) {
        free(simpbd_record_p->request_data_p);
        simpbd_record_p->request_data_p = NULL;
    }

    SIMPB_LOG_E("SYNC EF_PBC return failure.");
    simpbd_complete_request(simpbd_record_p, STE_SIMPB_FAILURE, NULL, 0);
    return STE_SIMPB_FAILURE;
}

/* utilities */
static void dump_hex(void *data_p, uint16_t length)
{
    uint16_t i = 0;
    uint16_t j = 1;
    char string[100];
    char tmp[4];
    uint8_t *ptr_p = (uint8_t *)data_p;

    memset(string, 0, 100);

    for (i = 0; i < length; i++) {
        sprintf(tmp, " %02X", ptr_p[i]);
        strcat(string, tmp);

        if (!(j % 16)) {
            SIMPB_LOG_D("HEX_DUMP: %s", string);
            memset(string, 0, 100);
        }

        j++;
    }

    SIMPB_LOG_D("HEX_DUMP: %s", string);
}


static uint8_t simpbd_get_filedescriptor_from_fcp(void *data_p, uint16_t length, uint16_t *index_p)
{
    uint8_t result = 0;
    uint16_t i = 0;
    uint8_t *ptr_p = (uint8_t *)data_p;

    if (NULL != index_p) {
        for (i = 0; i < length; i++) {
            if (SIMPBD_GET_FILE_INFORMATION_FCP_FD_TAG_VALUE == ptr_p[i]) {
                *index_p = i;
                result = -1;
                break;
            }
        }
    }

    return result;
}

void simpbd_complete_request(simpbd_record_t *simpbd_record_p, ste_simpb_result_t result, void *data_p, size_t data_size)
{
    SIMPB_LOG_D("simpbd_record_p=%p, result=%d, data_size=%d", simpbd_record_p, result, data_size);

    if (!simpbd_record_p->internal_handling) {
        simpbd_handle_response(simpbd_record_p->simpbd_client_p,
                               simpbd_record_p->caller_data_p,
                               simpbd_record_p->simpbd_request_id,
                               result,
                               data_p,
                               data_size);
    }
}


void simpbd_send_unsolicited(ste_simpb_cause_t cause, void *data_p, size_t data_size)
{
    SIMPB_LOG_D("cause=%d, data_size=%d", cause, data_size);

    simpbd_handle_event(cause, data_p, data_size);
}


void simpbd_translate_sim_request_status_to_simpbd_code(simpbd_record_t *simpbd_record_p, uicc_request_status_t result)
{
    SIMPB_LOG_D("entered");
    ste_simpb_result_t simpb_result;

    switch (result) {
    case UICC_REQUEST_STATUS_NOT_AVAILABLE:
        simpb_result = STE_SIMPB_NOT_SUPPORTED;
        break;

    case UICC_REQUEST_STATUS_FAILED_RESOURCE:
        simpb_result = STE_SIMPB_ERROR_RESOURCE;
        break;

    case UICC_REQUEST_STATUS_FAILED:
        simpb_result = STE_SIMPB_FAILURE;
        break;

    case UICC_REQUEST_STATUS_TIME_OUT:
        simpb_result = STE_SIMBP_ERROR_NOT_CONNECTED;
        break;

    case UICC_REQUEST_STATUS_FAILED_PARAMETER:
        simpb_result = STE_SIMPB_ERROR_PARAM;
        break;

    case UICC_REQUEST_STATUS_FAILED_APPLICATION: /* Fall through intentional */
    case UICC_REQUEST_STATUS_FAILED_UNPACKING:
    case UICC_REQUEST_STATUS_FAILED_STATE:
    case UICC_REQUEST_STATUS_OK:
    default:
        simpb_result = STE_SIMPB_ERROR_UNKNOWN;
        break;
    }

    simpbd_record_p->result_code = simpb_result;
}


ste_simpb_sim_state_t simpbd_translate_sim_state_to_simpbd_state(sim_state_t sim_state)
{
    ste_simpb_sim_state_t simpbd_state;

    switch (sim_state) {
    case SIM_STATE_SIM_ABSENT:
        simpbd_state = STE_SIMPB_SIM_STATE_SIM_ABSENT;
        break;

    case SIM_STATE_NOT_READY:
        simpbd_state = STE_SIMPB_SIM_STATE_NOT_READY;
        break;

    case SIM_STATE_READY:
        simpbd_state = STE_SIMPB_SIM_STATE_READY;
        break;

    default:
        simpbd_state = STE_SIMPB_SIM_STATE_UNKNOWN;
        break;
    }

    return simpbd_state;
}


/*  SIM call-back */
int simpbd_sim_cb_read(const int fd, const void *context_p)
{
    simpbd_client_t *simpbd_client_p = (simpbd_client_t *)context_p;
    int result;
    SIMPB_LOG_D("entered");

    if (!simpbd_client_p || !simpbd_client_p->ste_sim_p) {
        SIMPB_LOG_E("simpbd_client_p(%p) or simpbd_client_p->ste_sim_p is NULL!", simpbd_client_p);
        goto error;
    }

    result = ste_sim_read(simpbd_client_p->ste_sim_p);

    if (0 > result) {
        SIMPB_LOG_E("ste_sim_read(), result %d", result);
        goto error;
    }

    return 0;

error:
    return -1;
}

static uint8_t simpbd_ef_adn_record_occupied(uint8_t record_len, ste_uicc_sim_file_read_record_response_t *rec_data)
{
    uint8_t name_len = record_len - SIMPB_ADN_REC_PH_NO_LEN;

    if (
        (
            !(rec_data->data[0] & 0x80)
            ||
            ((rec_data->data[0] == 0x80) && ((rec_data->data[1] != 0xFF) || (rec_data->data[2] != 0xFF)))
            ||
            ((rec_data->data[0] == 0x81) && (rec_data->data[1] != 0x00))
            ||
            ((rec_data->data[0] == 0x82) && (rec_data->data[1] != 0x00))
        )
        ||
        (
            (*(rec_data->data + name_len) != 0xFF) && (*(rec_data->data + name_len) != 0)
            &&
            /*Additional check to declare an entry as free if number length is greater than 11
            or if number length is 1 and begins with 0xFF*/
            (*(rec_data->data + name_len) <= 11) && (!((*(rec_data->data + name_len) == 1) &&
                    (*(rec_data->data + name_len + 1) == 0xFF)))
        )
    ) {
        return TRUE;
    } else {
        return FALSE;
    }

}

int simpbd_sim_cb_remove(const int fd, const void *context_p)
{
    simpbd_client_t *simpbd_client_p = (simpbd_client_t *)context_p;
    SIMPB_LOG_D("entered");

    /* Socket to sim has been closed, clean up */
    simpbd_cache_flush_pbr_data();
    simpbd_cache_flush_file_info();
    ste_sim_delete(simpbd_client_p->ste_sim_p, 0);

    return -1;
}


void on_new_sim_state(simpbd_client_t *simpb_client_p, sim_state_t state)
{
    ste_simpb_cb_sim_state_t sim_state;
    SIMPB_LOG_D("simpb_client_p=%p, state=%d", simpb_client_p, state);

    /* Map the SIM states to the ones we are interested in tracking */
    sim_state.value = simpbd_translate_sim_state_to_simpbd_state(state);

    if (sim_state.value != simpb_client_p->state) {
        simpbd_send_unsolicited(STE_SIMPB_CAUSE_UNSOLICITED_SIM_STATE, &sim_state, sizeof(ste_simpb_cb_sim_state_t));

        /* Cache the SIM state */
        simpb_client_p->state = sim_state.value;
    }

    return;
}

static void simpbd_ef_pbc_sync(simpbd_client_t *simpbd_client_p)
{
    ste_simpb_result_t result;

    SIMPB_LOG_D("simpbd sync EF_PBC");

    result = simpbd_execute(simpbd_client_p, STE_SIMPB_INTERNAL_REQUEST_ID_SYNC_EF_PBC, NULL, NULL);

    return;
}

void simpbd_sim_cb_receive(int cause,
                           uintptr_t sim_tag,
                           void *data_p,
                           void *user_data_p)
{
    simpbd_client_t *simpbd_client_p = (simpbd_client_t *)user_data_p;
    simpbd_record_t *simpbd_record_p;
    SIMPB_LOG_D("cause=%d, sim_tag=%d, data_p=%p, user_data_p=%p", cause, sim_tag, data_p, user_data_p);

    if (NULL == simpbd_client_p) {
        SIMPB_LOG_E("simpbd_client_p is NULL!");
        goto error;
    }

    if (sim_tag) {
        simpbd_record_p = simpbd_record_from_sim_tag(sim_tag);

        if (simpbd_record_p) {
            simpbd_record_p->response_data_p = data_p;
        } else {
            SIMPB_LOG_E("Unknown or corrupt sim_tag %d!", sim_tag);
            goto error;
        }
    } else {
        simpbd_record_p = NULL;
    }

    switch (cause) {
    case STE_SIM_CAUSE_NOOP:
    case STE_SIM_CAUSE_CONNECT:
        break;

    case STE_SIM_CAUSE_DISCONNECT: {
        simpbd_record_t *record_p = simpbd_record_get_first_non_free();

        /* Make a clean break with simpbd clients, terminating their pending requests */
        while (NULL != record_p) {
            simpbd_complete_request(record_p, STE_SIMPB_FAILURE, NULL, 0);
            simpbd_record_free(record_p);
            record_p = simpbd_record_get_first_non_free();
        }

        /* and letting them know SIM is not READY */
        ste_simpb_cb_sim_state_t state;
        state.value = STE_SIMPB_SIM_STATE_NOT_READY;
        /* since simpbd will be shutdown and so do the other modules, due to the silent reset
         * it is not necessary to send out this unsolicited event because this will cause some
         * problem due to the closed sockets by other modules. */
        //simpbd_send_unsolicited(STE_SIMPB_CAUSE_UNSOLICITED_SIM_STATE, &state, sizeof(ste_simpb_cb_sim_state_t));

        SIMPB_LOG_E("**************************************************************************");
        SIMPB_LOG_E("*** SIM PHONEBOOK DAEMON EXITING DUE TO LOST CONNECTION TO SIM DAEMON! ***");
        SIMPB_LOG_E("**************************************************************************");

        exit(-1);
        /* As SIMd is down, we need to exit to allow OS to restart simpbd, and clients to re-connect. */
    }

    case STE_UICC_CAUSE_REQ_READ_SIM_FILE_RECORD:
    case STE_UICC_CAUSE_REQ_UPDATE_SIM_FILE_RECORD:
    case STE_UICC_CAUSE_REQ_READ_SIM_FILE_BINARY:
    case STE_UICC_CAUSE_REQ_UPDATE_SIM_FILE_BINARY:
    case STE_UICC_CAUSE_REGISTER:
    case STE_UICC_CAUSE_REQ_GET_APP_INFO:
    case STE_UICC_CAUSE_REQ_GET_FILE_INFORMATION:
    case STE_UICC_CAUSE_REQ_SIM_FILE_GET_FORMAT:
    case STE_UICC_CAUSE_REQ_CARD_STATUS:
    case STE_UICC_CAUSE_REQ_GET_SIM_STATE: {
        if (NULL != simpbd_record_p && NULL != simpbd_record_p->request) {
            simpbd_record_p->response_result = STE_SIM_SUCCESS; /* Successful transaction */
            simpbd_record_p->result_code = simpbd_record_p->request(simpbd_record_p);

            if (STE_SIMPB_PENDING != simpbd_record_p->result_code) {
                /* The request terminated, free the record. */
                simpbd_record_free(simpbd_record_p);
            }
        }

        break;
    }

    case STE_UICC_CAUSE_NOT_READY: {
        /* If this happens because of a request it cannot be completed because SIM is not ready to process it */
        if (NULL != simpbd_record_p && NULL != simpbd_record_p->request) {
            simpbd_record_p->response_result = STE_UICC_CAUSE_NOT_READY; /* Failed transaction */
            simpbd_record_p->result_code = simpbd_record_p->request(simpbd_record_p);

            if (STE_SIMPB_PENDING != simpbd_record_p->result_code) {
                /* The request terminated, free the record. */
                simpbd_record_free(simpbd_record_p);
            }
        }

        break;
    }

    case STE_SIM_CAUSE_SHUTDOWN:
    case STE_SIM_CAUSE_HANGUP:
        /* NOT EXPLAINED IN SIM API */
        break;

    case STE_UICC_CAUSE_SIM_STATE_CHANGED: {
        ste_uicc_sim_state_changed_t *uicc_data_p = (ste_uicc_sim_state_changed_t *)data_p;

        if (NULL != uicc_data_p) {
            SIMPB_LOG_E("Got STE_UICC_CAUSE_SIM_STATE_CHANGED, state = %d", uicc_data_p->state);
            on_new_sim_state(simpbd_client_p, uicc_data_p->state);
        }

        break;
    }

    case STE_UICC_CAUSE_SIM_STATUS: {
        sim_status_t *sim_status_p = (sim_status_t *)data_p;

        if (NULL != sim_status_p) {
            sim_state_t state;

            /* Map reason to SIM state */
            switch (sim_status_p->reason) {
            case SIM_REASON_DISCONNECTED_CARD: /* fall through intentional */
            case SIM_REASON_NO_CARD:
                state = SIM_STATE_SIM_ABSENT;
                break;

            case SIM_REASON_STARTUP_DONE:
                state = SIM_STATE_READY;
                break;

            default:
                state = SIM_STATE_UNKNOWN;
                break;
            }

            on_new_sim_state(simpbd_client_p, state);
        }

        break;
    }

    case STE_CAT_CAUSE_PC_REFRESH_IND: {
        ste_cat_pc_refresh_ind_t *ste_cat_pc_refresh_ind_p = (ste_cat_pc_refresh_ind_t *)data_p;

        if (NULL != ste_cat_pc_refresh_ind_p) {
            SIMPB_LOG_D("STE_CAT_CAUSE_PC_REFRESH_IND type=%d", ste_cat_pc_refresh_ind_p->type);
            simpbd_cache_flush_pbr_data();
            simpbd_cache_flush_file_info();
            (void)simpbd_execute(simpbd_client_p, STE_SIMPB_INTERNAL_REQUEST_ID_PBR_CACHE_REFRESH, NULL, NULL);
        }

        break;
    }

    case STE_CAT_CAUSE_PC_REFRESH_FILE_IND: {
        ste_cat_pc_refresh_file_ind_t *ste_cat_pc_refresh_file_ind_p = (ste_cat_pc_refresh_file_ind_t *)data_p;
        sim_path_t path = ste_cat_pc_refresh_file_ind_p->path;
        uint8_t pbr_fid_local[] = SIMPBD_PBR_UINT8_USIM_LOCAL_PATH;
        uint8_t pbr_fid_global[] = SIMPBD_PBR_UINT8_USIM_GLOBAL_PATH;
        ste_simpb_phonebook_type_t selected_phonebook = simpbd_client_p->selected_phonebook;

        if (NULL != data_p && SIMPBD_PBR_UINT8_USIM_SIZE_OF_PATH == path.pathlen) {
            if ((STE_SIMPB_PHONEBOOK_TYPE_LOCAL == selected_phonebook &&
                    !memcmp(&path.path[0], pbr_fid_local, SIMPBD_PBR_UINT8_USIM_SIZE_OF_PATH))  ||
                    (STE_SIMPB_PHONEBOOK_TYPE_GLOBAL == selected_phonebook &&
                     !memcmp(&path.path, pbr_fid_global, SIMPBD_PBR_UINT8_USIM_SIZE_OF_PATH))) {
                simpbd_cache_flush_pbr_data();
                (void)simpbd_execute(simpbd_client_p, STE_SIMPB_INTERNAL_REQUEST_ID_PBR_CACHE_REFRESH, NULL, NULL);
            }
        } else {
            SIMPB_LOG_D("data_p is NULL");
        }

        break;
    }

    default:
        SIMPB_LOG_I("Unhandled cause %d", cause);
        break;
    }

error:

    return;
}


static char *simpbd_cache_get_ef_path_phonebook(ste_simpb_phonebook_type_t phonebook, uint8_t app_type)
{
    char *result = NULL;

    result = ((app_type == SIM_APP_GSM) || (phonebook == STE_SIMPB_PHONEBOOK_TYPE_GLOBAL)) ? simpbd_ef_sim_path : simpbd_ef_usim_path;

    return result;
}




