/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

#include "cn_log.h"
#include "cn_macros.h"
#include "cn_data_types.h"
#include "cn_pdc_ecc_list_handling.h"
#include "request_handling.h"
#include "cn_pdc_internal.h"
#include "sim_client.h"

/* TODO: Remove and add path so that it is set through init.rc */

#ifdef ENABLE_MODULE_TEST
/* For test purposes emergency_storage_p must be set (it is usually set through the init.rc */
static char *emergency_storage_p = "/tmp/enl";
#else
static char *emergency_storage_p = "/data/misc/enl";
#endif

/* Array with the pre-defined emergency call numbers as specified in 22.101 chapter 10.1.1. */
static char *predefined_emergency_numbers[] = {"112", "911"};

/* Array with the pre-defined emergency call numbers to use when no sim is available,
 * as specified in 22.101 chapter 10.1.1. */
static char *predefined_emergency_numbers_no_sim[] = {"000", "08", "110", "999", "118", "119"};

/* Array with current mcc received from network */
#define CN_MCC_MAX_LEN 3
static char mcc[CN_MCC_MAX_LEN + 1] = { '\0' };

#define CN_MCC_EQUALS_CACHED(mcc_p) \
    ((mcc[0] == mcc_p[0]) && \
     (mcc[1] == mcc_p[1]) && \
     (mcc[2] == mcc_p[2]))


/******************************************************************************
 *                          Local prototypes                                  *
 ******************************************************************************/
static cn_bool_t cn_pdc_util_read_stored_emergency_numbers(cn_emergency_number_list_internal_t **list_pp);
static cn_bool_t cn_pdc_util_is_emergency_number_in_list(cn_emergency_number_config_internal_t *config_p,
        cn_emergency_number_config_internal_t *list_p,
        cn_uint8_t num_of_emergency_numbers,
        cn_uint8_t *pos_p);
static cn_bool_t cn_pdc_util_write_config_to_file(FILE *file_p, cn_emergency_number_config_internal_t *config_p);
static cn_bool_t cn_pdc_util_get_next_delimiter(char *buffer_p, char **delimiter_pp, cn_uint8_t *length_p);

/******************************************************************************
 *                          TOP-LEVEL FUNCTIONS                               *
 ******************************************************************************/

void cn_pdc_set_emergency_number_filepath(char *filepath_p)
{
    (void) filepath_p;
    /* TODO: Remove old stored value. Might need to set defaults in some other fashion.
        emergency_storage_p = calloc(1, strlen(filepath_p) + 1);
        (void)strcpy(emergency_storage_p, filepath_p);*/
}

request_status_t handle_request_modify_emergency_number_list(void *data_p, request_record_t *record_p)
{
    request_status_t status = REQUEST_STATUS_ERROR;

    REQUIRE_VALID_RECORD(record_p);

    switch (record_p->state) {
    case REQUEST_STATE_REQUEST: {
        cn_bool_t result = FALSE;

        REQUIRE_VALID_REQUEST_DATA(data_p);

        cn_request_modify_emergency_number_list_t *request_data_p = (cn_request_modify_emergency_number_list_t *) data_p;

        switch (request_data_p->operation) {
        case CN_EMERGENCY_NUMBER_OPERATION_CLEAR_LIST: {
            CN_LOG_I("CN_EMERGENCY_NUMBER_OPERATION_CLEAR_LIST");
            result = cn_pdc_util_clear_emergency_numbers_by_origin(CN_EMERGENCY_NUMBER_ORIGIN_CLIENT, FALSE);
            break;
        }
        case CN_EMERGENCY_NUMBER_OPERATION_ADD: {
            cn_emergency_number_list_internal_t list = { 1, {{
                        request_data_p->config.service_type,
                        CN_EMERGENCY_NUMBER_ORIGIN_CLIENT,
                        {'\0'}, {'\0'}, {'\0'}
                    }
                }
            };

            CN_LOG_I("CN_EMERGENCY_NUMBER_OPERATION_ADD");

            (void) memmove(list.emergency_number[0].emergency_number, request_data_p->config.emergency_number, CN_EMERGENCY_NUMBER_STRING_LENGTH);
            list.emergency_number[0].emergency_number[CN_EMERGENCY_NUMBER_STRING_LENGTH] = '\0';
            (void) memmove(list.emergency_number[0].mcc, request_data_p->config.mcc, CN_MCC_STRING_LENGTH);
            list.emergency_number[0].mcc[CN_MCC_STRING_LENGTH] = '\0';
            (void) memmove(list.emergency_number[0].mcc_range, request_data_p->config.mcc_range, CN_MCC_RANGE_STRING_LENGTH);
            list.emergency_number[0].mcc_range[CN_MCC_RANGE_STRING_LENGTH] = '\0';

            result = cn_pdc_util_add_emergency_numbers_to_list(&list); /* synchronous operation */
            break;
        }
        /* NOTE: Remove function is currently not used by any client. If to be used, then
         *       general "remove single element from emergency number list" tests
         *       has to be written. */
        case CN_EMERGENCY_NUMBER_OPERATION_REMOVE: {
            cn_emergency_number_config_internal_t config = { request_data_p->config.service_type,
                                                  CN_EMERGENCY_NUMBER_ORIGIN_CLIENT,
            {'\0'}, {'\0'}, {'\0'}
                                                           };

            CN_LOG_I("CN_EMERGENCY_NUMBER_OPERATION_REM");

            (void) memmove(config.emergency_number, request_data_p->config.emergency_number, CN_EMERGENCY_NUMBER_STRING_LENGTH);
            config.emergency_number[CN_EMERGENCY_NUMBER_STRING_LENGTH] = '\0';
            (void) memmove(config.mcc, request_data_p->config.mcc, CN_MCC_STRING_LENGTH);
            config.mcc[CN_MCC_STRING_LENGTH] = '\0';
            (void) memmove(config.mcc_range, request_data_p->config.mcc_range, CN_MCC_RANGE_STRING_LENGTH);
            config.mcc_range[CN_MCC_RANGE_STRING_LENGTH] = '\0';

            result = cn_pdc_util_rem_emergency_number_from_list(&config);
            break;
        }
        default: {
            CN_LOG_E("unknown emergency number operation! (%d)", request_data_p->operation);
            break;
        }
        }

        if (!result) {
            CN_LOG_E("operation failed! (%d)", request_data_p->operation);
            send_response(record_p->client_id, CN_RESPONSE_MODIFY_EMERGENCY_NUMBER_LIST, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        send_response(record_p->client_id, CN_RESPONSE_MODIFY_EMERGENCY_NUMBER_LIST, CN_SUCCESS, record_p->client_tag, 0, NULL);
        return REQUEST_STATUS_DONE;
    }
    default: {
        CN_LOG_W("record_p->state has an unknown value!");
        send_response(record_p->client_id, CN_RESPONSE_MODIFY_EMERGENCY_NUMBER_LIST, CN_FAILURE, record_p->client_tag, 0, NULL);
        goto error;
    }
    }

error:
    return status;
}

request_status_t handle_request_get_emergency_number_list(void *data_p, request_record_t *record_p)
{

    request_status_t status = REQUEST_STATUS_ERROR;

    REQUIRE_VALID_RECORD(record_p);

    switch (record_p->state) {
    case REQUEST_STATE_REQUEST: {
        cn_emergency_number_list_internal_t *cn_emergency_number_list_internal_p = NULL;
        cn_emergency_number_list_t *cn_emergency_number_list_p = NULL;
        size_t size = 0;
        REQUIRE_VALID_REQUEST_DATA(data_p);

        CN_LOG_D("CN_REQUEST_GET_EMERGENCY_NUMBER_LIST");

        /* Attempt to retrieve the saved list:*/
        cn_emergency_number_list_internal_p = cn_pdc_get_stored_emerg_number_list();

        if (NULL == cn_emergency_number_list_internal_p) {
            /* no entries in list. Make an empty list to return. */
            cn_emergency_number_list_p = (cn_emergency_number_list_t *) calloc(1, sizeof(cn_emergency_number_list_t));
            size = sizeof(cn_emergency_number_list_t);
        } else {
            /* A list was returned. Take the entries and convert them into the stripped format that the API requires: */
            int i = 0;
            int j = 0;
            int num_entries = 0;

            for (i = 0; i < cn_emergency_number_list_internal_p->num_of_emergency_numbers; i++) {
                if (CN_EMERGENCY_NUMBER_ORIGIN_CLIENT == cn_emergency_number_list_internal_p->emergency_number[i].origin) {
                    num_entries++;
                }
            }

            if (0 < num_entries) {
                size = sizeof(cn_emergency_number_list_t) + (num_entries - 1) * sizeof(cn_emergency_number_t);
            } else {
                size = sizeof(cn_emergency_number_list_t);
            }

            cn_emergency_number_list_p = (cn_emergency_number_list_t *) calloc(1, size);

            if (NULL == cn_emergency_number_list_p) {
                CN_LOG_E("cn_emergency_number_list_p memory allocation failed");
                free(cn_emergency_number_list_internal_p);
                goto error;
            }

            for (i = 0, j = 0; i < cn_emergency_number_list_internal_p->num_of_emergency_numbers; i++) {
                if (CN_EMERGENCY_NUMBER_ORIGIN_CLIENT == cn_emergency_number_list_internal_p->emergency_number[i].origin) {
                    cn_emergency_number_config_internal_t *source_p = &cn_emergency_number_list_internal_p->emergency_number[i];
                    cn_emergency_number_t                 *target_p = &cn_emergency_number_list_p->emergency_number[j];
                    int n = CN_EMERGENCY_NUMBER_STRING_LENGTH < strlen((char *) source_p->emergency_number) ?
                    CN_EMERGENCY_NUMBER_STRING_LENGTH : strlen((char *) source_p->emergency_number);

                    (void) memmove(target_p->emergency_number, source_p->emergency_number, n);
                    target_p->emergency_number[n] = '\0';
                    target_p->service_type = source_p->service_type;
                    j++;
                }
            }

            cn_emergency_number_list_p->num_of_emergency_numbers = j;

            /* Size of data is non-constant as the number of emergency numbers vary, and a 'dynamic' data structure is used. */
            size = sizeof(cn_emergency_number_list_t) + (num_entries - 1) * sizeof(cn_emergency_number_t);
            free(cn_emergency_number_list_internal_p);
        }

        send_response(record_p->client_id, CN_RESPONSE_GET_EMERGENCY_NUMBER_LIST, CN_SUCCESS, record_p->client_tag, size, cn_emergency_number_list_p);
        free(cn_emergency_number_list_p);
        return REQUEST_STATUS_DONE;
    }
    default: {
        CN_LOG_E("record_p->state has an unknown value!");
        send_response(record_p->client_id, CN_RESPONSE_GET_EMERGENCY_NUMBER_LIST, CN_FAILURE, record_p->client_tag, 0, NULL);
        goto exit;
    }

    }

exit:
    return status;

error:
    send_response(record_p->client_id, CN_RESPONSE_MODIFY_EMERGENCY_NUMBER_LIST, CN_FAILURE, record_p->client_tag, 0, NULL);
    return status;
}

cn_emergency_number_list_internal_t *cn_pdc_get_default_emerg_number_list(void)
{
    int i = 0;
    int num_numbers = sizeof(predefined_emergency_numbers) / sizeof(predefined_emergency_numbers[0]);

    cn_emergency_number_list_internal_t *list_p = (cn_emergency_number_list_internal_t *) malloc(
        sizeof(cn_emergency_number_list_internal_t) + (num_numbers - 1)
        * sizeof(cn_emergency_number_config_internal_t));
    list_p->num_of_emergency_numbers = num_numbers;

    for (i = 0; i < num_numbers; i++) {
        (void) strcpy(list_p->emergency_number[i].emergency_number, predefined_emergency_numbers[i]);
    }

    CN_LOG_D("Returning list with %d elements.", num_numbers);

    return list_p;
}

cn_emergency_number_list_internal_t *cn_pdc_get_sim_emerg_number_list(cn_pdc_t *pdc_p)
{
    ste_sim_t *sim_p = NULL;
    uicc_request_status_t result = 0;
    int i = 0;
    int num_numbers = 0;
    ste_uicc_sim_ecc_response_t *response = NULL;
    cn_emergency_number_list_internal_t *list_p = NULL;

    CN_LOG_D("State = %d.", pdc_p->pdc_ecc_state);

    switch (pdc_p->pdc_ecc_state) {
    case CN_PDC_ECC_STATE_INITIAL:
        /* Send call to sim and check if it is available.*/

        sim_p = sim_client_get_handle();

        if (!sim_p) {
            CN_LOG_E("sim_client_get_handle failed!");
            goto error;
        }

        result = ste_uicc_sim_file_read_ecc(sim_p, (uintptr_t)request_record_get_modem_tag(pdc_p->record_p));

        if ((UICC_REQUEST_STATUS_PENDING != result) && (UICC_REQUEST_STATUS_OK != result)) {
            CN_LOG_E("ste_uicc_sim_file_read_ecc failed!");
            goto error;
        }

        /* Set ECC substate*/
        pdc_p->pdc_ecc_state = CN_PDC_ECC_STATE_WAIT_FOR_SIM_LIST;

        return NULL;
        break;
    case CN_PDC_ECC_STATE_WAIT_FOR_SIM_LIST:

        /* Check response from SIM (which should be available by now */
        if (!(pdc_p->record_p)) {
            CN_LOG_E("SIM response error");
            goto error;
        }

        if (pdc_p->record_p->response_error_code == STE_UICC_CAUSE_NOT_READY) {
            /* This is what SIM replies if the sim card is not available. Use the hard-coded SIM replacement list! */
            CN_LOG_W("SIM called failed, no SIM card available.");
            /* Put the contents of the default no-sim-list into the response: */
            num_numbers = sizeof(predefined_emergency_numbers_no_sim) / sizeof(predefined_emergency_numbers_no_sim[0]);
            list_p = (cn_emergency_number_list_internal_t *) calloc(1, sizeof(cn_emergency_number_list_internal_t) + (num_numbers - 1)
            * sizeof(cn_emergency_number_config_internal_t));

            if (NULL == list_p) {
                CN_LOG_E("Calloc failed.");
                goto error;
            }

            list_p->num_of_emergency_numbers = num_numbers;

            for (i = 0; i < num_numbers; i++) {
                strcpy(list_p->emergency_number[i].emergency_number, predefined_emergency_numbers_no_sim[i]);
            }

            CN_LOG_D("Returning list with ECC numbers No SIM. %d entries.", num_numbers);
            /* Set ECC substate*/
            pdc_p->pdc_ecc_state = CN_PDC_ECC_STATE_RECEIVED_SIM_LIST;

            return list_p;

        }

        if (!(pdc_p->record_p->response_data_p)) {
            CN_LOG_E("SIM response error, response_data_p is NULL");
            goto error;
        }

        response = (ste_uicc_sim_ecc_response_t *)(pdc_p->record_p->response_data_p);

        if (STE_UICC_STATUS_CODE_OK == response->uicc_status_code) {
            /* Put the contents of the list into a cn_emergency_number_list_internal_t* */
            CN_LOG_D("ECC list received from SIM. %d entries.", response->number_of_records);
            num_numbers = response->number_of_records;
            list_p = (cn_emergency_number_list_internal_t *) calloc(1, sizeof(cn_emergency_number_list_internal_t) +
            (num_numbers - 1) * sizeof(cn_emergency_number_config_internal_t));

            if (NULL == list_p) {
                CN_LOG_E("Calloc failed");
                goto error;
            }

            list_p->num_of_emergency_numbers = num_numbers;

            for (i = 0; i < num_numbers; i++) {
                strcpy(list_p->emergency_number[i].emergency_number, response->ecc_number_p[i].number);
            }
        } else {
            CN_LOG_E("SIM present, but reading of ECC list failed.");
            goto error;
        }

        CN_LOG_D("Returning list with ECC numbers. %d entries.", num_numbers);
        /* Set ECC substate*/
        pdc_p->pdc_ecc_state = CN_PDC_ECC_STATE_RECEIVED_SIM_LIST;

        return list_p;
        break;

    case CN_PDC_ECC_STATE_RECEIVED_SIM_LIST:
    case CN_PDC_ECC_STATE_ERROR:
    default:
        CN_LOG_W("Unknown/unhandled state %d.\n", pdc_p->pdc_ecc_state);
        goto error;
        break;
    }

    return NULL;

error:
    pdc_p->pdc_ecc_state = CN_PDC_ECC_STATE_ERROR;
    return NULL;
}

cn_emergency_number_list_internal_t *cn_pdc_get_stored_emerg_number_list(void)
{
    cn_emergency_number_list_internal_t *cn_emergency_number_list_p = NULL;
    cn_bool_t is_success = FALSE;

    is_success = cn_pdc_util_read_stored_emergency_numbers(&cn_emergency_number_list_p);

    if (!is_success) {
        CN_LOG_E("Failed to read emergency numbers from file!");

        return NULL;
    }

    CN_LOG_D("Returning list. %d entries.", NULL == cn_emergency_number_list_p ? 0 : cn_emergency_number_list_p->num_of_emergency_numbers);

    return cn_emergency_number_list_p;
}


/**
 * \fn cn_pdc_set_current_mcc()
 * \brief Set the current mcc.
 *
 * This method sets the current mcc and returns if the mcc
 * has been changed or is unknown (i.e. set to NULL).
 *
 * \return  \ref cn_bool_t, TRUE if mcc has changed or is lost, FALSE if not.
 *
 */
cn_bool_t cn_pdc_set_current_mcc(char *mcc_p)
{
    cn_bool_t mcc_changed_or_lost = false;

    if (NULL != mcc_p && 0 != strlen(mcc_p)) {
        if (!CN_MCC_EQUALS_CACHED(mcc_p)) { /* New network (mcc)? */
            (void) memmove(mcc, mcc_p, CN_MCC_MAX_LEN);
            mcc[CN_MCC_MAX_LEN] = '\0';
            mcc_changed_or_lost = true;
        }
    } else { /* lost network? */
        (void) memset(mcc, '\0', CN_MCC_MAX_LEN + 1);
        mcc_changed_or_lost = true;
    }

    return mcc_changed_or_lost;
}

/**
 * \fn cn_pdc_util_clear_emergency_number_list()
 * \brief Clear emergency number list.
 *
 * This method will clear the emergency number list.
 *
 * \return  \ref cn_bool_t, TRUE is returned if successful, else FALSE.
 *
 */
cn_bool_t cn_pdc_util_clear_emergency_number_list(void)
{
    FILE *file_p = NULL;
    cn_bool_t result = false;

    CN_LOG_I("clearing emergency number list");

    file_p = fopen(emergency_storage_p, "w");

    if (NULL == file_p) {
        CN_LOG_E("error, couldn't open:%s", emergency_storage_p);
        goto error;
    }

    if (0 != fclose(file_p)) {
        CN_LOG_E("error closing file");
    }

    result = true;

error:
    return result;
}

/**
 * \fn cn_pdc_util_clear_emergency_number_list()
 * \brief Clear emergency number list by origin.
 *
 * This method will removes the specified origin sublist from the emergency number list.
 *
 * When this method is called from registration indication/request handling, the parameter
 * compare_mcc should be set to TRUE. This enables checking if the emergency numbers belongs
 * to the current mcc or if they should be removed from the emergency number list.
 * The reason for this is to avoid possible race conditions between emergency number indications
 * and registration events/requests.
 *
 * \return  \ref cn_bool_t, TRUE is returned if successful, else FALSE.
 *
 */
cn_bool_t cn_pdc_util_clear_emergency_numbers_by_origin(uint16_t origins, cn_bool_t compare_mcc)
{
    cn_emergency_number_list_internal_t *list_p = NULL;
    FILE *file_p = NULL;
    cn_uint8_t i;
    cn_bool_t result = false;

    CN_LOG_I("clearing emergency number list by origin: 0x%x", origins);

    if (!cn_pdc_util_read_stored_emergency_numbers(&list_p)) {
        CN_LOG_E("could not read stored emergency numbers");
        goto error;
    }

    if (NULL != list_p) {
        file_p = fopen(emergency_storage_p, "w");

        if (NULL == file_p) {
            CN_LOG_E("error, couldn't open:%s", emergency_storage_p);
            goto error;
        }

        for (i = 0; i < list_p->num_of_emergency_numbers; i++) {
            /* restore emergency numbers with an origin that is not to be removed */

            if (!(list_p->emergency_number[i].origin & origins)) {
                if (!cn_pdc_util_write_config_to_file(file_p, &list_p->emergency_number[i])) {
                    CN_LOG_E("could not write config to file:%s", emergency_storage_p);
                    goto error;
                }
            } else {
                /* if applicable, restore the emergency numbers that correspond
                 * to the currently cached mcc.*/
                if (TRUE == compare_mcc &&
                CN_EMERGENCY_NUMBER_ORIGIN_NETWORK == list_p->emergency_number[i].origin &&
                CN_MCC_EQUALS_CACHED(list_p->emergency_number[i].mcc)) {
                    if (!cn_pdc_util_write_config_to_file(file_p, &list_p->emergency_number[i])) {
                        CN_LOG_E("could not write config to file:%s", emergency_storage_p);
                        goto error;
                    }
                }
            }
        }
    }

    result = true;

error:

    free(list_p);

    if (NULL != file_p) {
        if (0 != fclose(file_p)) {
            CN_LOG_E("error closing file");
        }
    }

    return result;
}

/**
 * \fn cn_pdc_util_add_emergency_numbers_to_list(cn_emergency_number_list_internal_t *sublist_p)
 * \brief Add a emergency number sublist to the emergency number list.
 *
 * This method will add a emergency number sublist to the emergency number list, and save the list to
 * the file system.
 *
 * \return  \ref cn_bool_t, TRUE is returned if successful, else FALSE.
 *
 */
cn_bool_t cn_pdc_util_add_emergency_numbers_to_list(cn_emergency_number_list_internal_t *sublist_p)
{
    cn_emergency_number_list_internal_t *list_p = NULL;
    FILE *file_p = NULL;
    cn_bool_t result = false;
    cn_uint8_t i = 0;
    cn_uint8_t n = 0;

    if (NULL == sublist_p) {
        CN_LOG_E("null parameter received");
        goto error;
    }

    /* read list from file */
    if (!cn_pdc_util_read_stored_emergency_numbers(&list_p)) {
        CN_LOG_E("could not read stored emergency numbers");
        goto error;
    }

    file_p = fopen(emergency_storage_p, "w");

    if (NULL == file_p) {
        CN_LOG_E("error, couldn't open:%s", emergency_storage_p);
        goto error;
    }

    /* restore previous contents? */
    if (NULL != list_p) {
        for (i = 0; i < list_p->num_of_emergency_numbers; i++) {
            if (!cn_pdc_util_write_config_to_file(file_p, &list_p->emergency_number[i])) {
                CN_LOG_E("could not write config to file:%s", emergency_storage_p);
                goto error;
            }
        }
    }

    /* append non-duplicates from sublist */
    for (i = 0; i < sublist_p->num_of_emergency_numbers; i++) {

        if (NULL != list_p) {
            if (cn_pdc_util_is_emergency_number_in_list(&sublist_p->emergency_number[i], &list_p->emergency_number[0], list_p->num_of_emergency_numbers, NULL)) {
                CN_LOG_I("duplicated emergency number:%s;%d;%s;%s;%d not added", sublist_p->emergency_number[i].emergency_number,
                sublist_p->emergency_number[i].service_type,
                sublist_p->emergency_number[i].mcc,
                sublist_p->emergency_number[i].mcc_range,
                sublist_p->emergency_number[i].origin);
                continue;
            }
        }

        n = sublist_p->num_of_emergency_numbers - 1;

        if (i < n) {
            if (cn_pdc_util_is_emergency_number_in_list(&sublist_p->emergency_number[i], &sublist_p->emergency_number[i + 1], n - i, NULL)) {
                CN_LOG_I("duplicated emergency number:%s;%d;%s;%s;%d not added", sublist_p->emergency_number[i].emergency_number,
                sublist_p->emergency_number[i].service_type,
                sublist_p->emergency_number[i].mcc,
                sublist_p->emergency_number[i].mcc_range,
                sublist_p->emergency_number[i].origin);
                continue;
            }
        }

        if (!cn_pdc_util_write_config_to_file(file_p, &sublist_p->emergency_number[i])) {
            CN_LOG_E("could not write config to file:%s", emergency_storage_p);
            goto error;
        }
    }

    result = true;

error:

    (void) free(list_p);

    if (NULL != file_p) {

        if (0 != fclose(file_p)) {
            CN_LOG_E("error closing file:%s", emergency_storage_p);
        }
    }

    return result;
}

/* NOTE: Remove function is currently not used by any client. If to be used, then
 *       general "remove single element from emergency number list" tests
 *       has to be written. */
cn_bool_t cn_pdc_util_rem_emergency_number_from_list(cn_emergency_number_config_internal_t *config_p)
{
    cn_emergency_number_list_internal_t *list_p = NULL;
    FILE *file_p = NULL;
    cn_bool_t result = false;
    cn_uint8_t i, pos = 0;

    /* read list from file */
    if (!cn_pdc_util_read_stored_emergency_numbers(&list_p)) {
        CN_LOG_E("could not read stored emergency numbers");
        goto error;
    }

    if (NULL == list_p) {
        CN_LOG_E("emergency list is NULL");
        goto error;
    }

    if (!cn_pdc_util_is_emergency_number_in_list(config_p, list_p->emergency_number, list_p->num_of_emergency_numbers, &pos)) {
        CN_LOG_I("emergency number not found");
        goto exit;
    }

    file_p = fopen(emergency_storage_p, "w");

    if (NULL == file_p) {
        CN_LOG_E("error, couldn't open:%s", emergency_storage_p);
        goto error;
    }

    /* restore previous contents */
    for (i = 0; i < list_p->num_of_emergency_numbers; i++) {
        if (i != pos && !cn_pdc_util_write_config_to_file(file_p, &list_p->emergency_number[i])) {
            CN_LOG_E("could not write config to file:%s", emergency_storage_p);
            goto error;
        }
    }

exit:
    result = true;

error:

    (void) free(list_p);

    if (NULL != file_p) {
        if (0 != fclose(file_p)) {
            CN_LOG_E("error closing file:%s", emergency_storage_p);
        }
    }

    return result;
}


/******************************************************************************
 *                            UTILITY FUNCTIONS                               *
 ******************************************************************************/

/**
 * \fn cn_pdc_util_get_next_delimiter(char *buffer_p, char **delimiter_pp, cn_uint8_t *length_p)
 *
 * \brief Search next delimiter.
 *
 * This method is used when getting the next token from an emergency number entry.
 * It searches buffer for the next intermediate ';' or the final '\n' characters
 * and updates the length (offset between delimiter and start of buffer).
 *
 * \return  \ref cn_bool_t, TRUE is returned if emergency number is written to file, else FALSE.
 *
 */
static cn_bool_t cn_pdc_util_get_next_delimiter(char *buffer_p, char **delimiter_pp, cn_uint8_t *length_p)
{
    cn_bool_t result = false;

    if (NULL != buffer_p && NULL != delimiter_pp && NULL != length_p) {
        *length_p = 0;
        *delimiter_pp = strchr(buffer_p, ';');

        if (NULL == *delimiter_pp) {
            *delimiter_pp = strchr(buffer_p, '\n');

            if (NULL != *delimiter_pp) {
                *length_p = *delimiter_pp - buffer_p;
                result = true;
            }
        } else {
            *length_p = *delimiter_pp - buffer_p;
            result = true;
        }
    } else {
        CN_LOG_E("null parameters received");
    }

    return result;
}

/**
 * \fn cn_pdc_util_write_config_to_file(FILE *file_p, cn_emergency_number_config_internal_t *config_p)
 *
 * \brief Writes an emergency number to a file.
 *
 * This method builds an emergency number entry according to this format
 * <emergency-number>;<service-type>;<mcc>;<mcc-range>;<origin> and writes it to
 * a file.
 *
 * \return  \ref cn_bool_t, TRUE is returned if emergency number is written to file, else FALSE.
 *
 */
static cn_bool_t cn_pdc_util_write_config_to_file(FILE *file_p, cn_emergency_number_config_internal_t *config_p)
{
    cn_bool_t result = false;

    if (NULL != file_p && NULL != config_p) {
        char buffer[CN_ENL_BUFFER_SIZE] = { '\0' };

        cn_sint16_t n = snprintf(buffer, CN_ENL_BUFFER_SIZE,
        "%s;%d;%s;%s;%d\n",
        config_p->emergency_number,
        config_p->service_type,
        config_p->mcc,
        config_p->mcc_range,
        config_p->origin);

        if (n < 0) {
            CN_LOG_E("couldn't write config into buffer");
        } else if (n >= CN_ENL_BUFFER_SIZE) {
            CN_LOG_E("truncation occured");
        } else {
            if (1 != fwrite(buffer, n, 1, file_p)) {
                CN_LOG_E("fwrite failed");
            }

            result = true;
        }
    } else {
        CN_LOG_E("null parameters received");
    }

    return result;
}

/**
 * \fn cn_pdc_util_is_emergency_number_in_list(cn_emergency_number_config_internal_t *config_p,
 *                                             cn_emergency_number_config_internal_t *list_p,
 *                                             cn_uint8_t num_of_emergency_numbers,
 *                                             cn_uint8_t *pos_p)
 *
 * \brief Checks if and where an emergency number exist in a list.
 *
 * This method checks if an emergency number exist in a list. If a number
 * is found the position in list is optionally set. If the position is not
 * to be set used, then set pos_p to NULL.
 *
 * \return  \ref cn_bool_t, TRUE is returned if emergency number is found, else FALSE.
 *
 */
static cn_bool_t cn_pdc_util_is_emergency_number_in_list(cn_emergency_number_config_internal_t *config_p,
cn_emergency_number_config_internal_t *list_p,
cn_uint8_t num_of_emergency_numbers,
cn_uint8_t *pos_p)
{
    cn_bool_t result = false;
    cn_uint8_t i;

    if (NULL != list_p && NULL != config_p) {
        for (i = 0; i < num_of_emergency_numbers; i++) {
            if (!strcmp(list_p[i].emergency_number, config_p->emergency_number) &&
            !strcmp(list_p[i].mcc, config_p->mcc) &&
            !strcmp(list_p[i].mcc_range, config_p->mcc_range) &&
            (list_p[i].origin == config_p->origin) &&
            (list_p[i].service_type == config_p->service_type)) {
                if (NULL != pos_p) {
                    *pos_p = i;
                }

                result = true;
                break;
            }
        }
    } else {
        CN_LOG_E("null parameters received");
    }

    return result;
}

/*
 * cn_pdc_util_read_stored_emergency_numbers - Reads a list of emergency numbers from file_p
 * Caller must deallocate list_p.
 */
static cn_bool_t cn_pdc_util_read_stored_emergency_numbers(
    cn_emergency_number_list_internal_t **list_pp)
{
    cn_emergency_number_list_internal_t *list_p = NULL;
    FILE *file_p = NULL;
    cn_uint8_t i = 0, num_of_emergency_numbers = 0;
    cn_uint32_t size = 0;
    char buffer[CN_ENL_BUFFER_SIZE] = { 0 };
    cn_bool_t result = false;

    if (NULL == list_pp) {
        CN_LOG_E("error, list_pp is NULL");
        goto error;
    }

    file_p = fopen(emergency_storage_p, "rb");

    if (NULL == file_p) {
        CN_LOG_I("couldn't open:%s, list is considered to be empty", emergency_storage_p);
        result = true;
        goto exit;
    }

    /* get number of entries */
    while (NULL != fgets(buffer, sizeof(buffer), file_p)) {
        num_of_emergency_numbers++;
    }

    (void) fseek(file_p, 0L, SEEK_SET);

    size = sizeof(cn_emergency_number_list_internal_t) + (num_of_emergency_numbers * sizeof(cn_emergency_number_config_internal_t));

    list_p = (cn_emergency_number_list_internal_t *) calloc(1, size);

    if (NULL == list_p) {
        CN_LOG_E("error, couldn't allocate list");
        goto error;
    }

    list_p->num_of_emergency_numbers = num_of_emergency_numbers;

    for (; NULL != fgets(buffer, sizeof(buffer), file_p); i++) {
        char *remainder_p = buffer;
        char *delimiter_p = NULL;
        cn_uint8_t length = 0;

        /* emergency number */
        if (cn_pdc_util_get_next_delimiter(remainder_p, &delimiter_p, &length)) {
            uint8_t n = (CN_EMERGENCY_NUMBER_STRING_LENGTH < length) ? CN_EMERGENCY_NUMBER_STRING_LENGTH : length;
            (void) memmove(list_p->emergency_number[i].emergency_number, remainder_p, n);
            list_p->emergency_number[i].emergency_number[n] = '\0';
            remainder_p = delimiter_p + 1;
        } else {
            CN_LOG_E("format error reading emergency number");
            goto error;
        }

        /* service type */
        if (cn_pdc_util_get_next_delimiter(remainder_p, &delimiter_p, &length)) {
            list_p->emergency_number[i].service_type = atoi(remainder_p);
            remainder_p = delimiter_p + 1;
        } else {
            CN_LOG_E("format error reading service type");
            goto error;
        }

        /* mcc */
        if (cn_pdc_util_get_next_delimiter(remainder_p, &delimiter_p, &length)) {
            uint8_t n = (CN_MCC_STRING_LENGTH < length) ? CN_MCC_STRING_LENGTH : length;
            (void) memmove(list_p->emergency_number[i].mcc, remainder_p, n);
            list_p->emergency_number[i].mcc[n] = '\0';
            remainder_p = delimiter_p + 1;
        } else {
            CN_LOG_E("format error mcc");
            goto error;
        }

        /* mcc-range */
        if (cn_pdc_util_get_next_delimiter(remainder_p, &delimiter_p, &length)) {
            uint8_t n = (CN_MCC_RANGE_STRING_LENGTH < length) ? CN_MCC_RANGE_STRING_LENGTH : length;
            (void) memmove(list_p->emergency_number[i].mcc_range, remainder_p, n);
            list_p->emergency_number[i].mcc_range[n] = '\0';
            remainder_p = delimiter_p + 1;
        } else {
            CN_LOG_E("format error mcc_range");
            goto error;
        }

        /* origin */
        if (cn_pdc_util_get_next_delimiter(remainder_p, &delimiter_p, &length)) {
            list_p->emergency_number[i].origin = atoi(remainder_p);
            remainder_p = delimiter_p + 1;
        } else {
            CN_LOG_E("format error reading origin");
            goto error;
        }
    }

    *list_pp = list_p;
    result = true;

exit:

    if (NULL != file_p) {
        if (0 != fclose(file_p)) {
            CN_LOG_E("error closing file");
        }
    }

    return result;

error:
    free(list_p);
    goto exit;
}

/***** Number comparison methods. Move to another file if they are to be reused by FDN/BDN/Other. (probably) *****/

/* Compares a phone number versus an FDN entry, returns true if there is a match, and false otherwise.
 *
 * Comparison is done according to 3gpp 22.101, section A 25. The cryptical requirements are interpreted as:
 *
 * a) If TON is INTERNATIONAL, first character of phone number under test must be '+' unless it is SS string
 * b) For every character in the FDN number, the phone number must match according to:
 *      0-9, #, * => exact match
 *      0xC, i.e. 'DTMF Control digit separator' => 'p' or 'w' (pause/wait)
 *      0xD, i.e. 'Wild value' => any character (NB: No character is not allowed!)
 * c) Any character in the FDN outside of the values defined above is considered illegal.
 * d) If all characters in FDN are matched, comparison is considered to be a success, even if there are characters
 *      left in the compared phone number.
 *
 * */
cn_bool_t cn_pdc_fdn_equal_numbers(char *phone_number_p, char *fdn_number_p, int fdn_number_length, int fdn_ton)
{
    char *pnr_p = phone_number_p;
    int i = 0;
    int is_ss_str = 0;

    if (fdn_ton == STE_SIM_TON_INTERNATIONAL) {
        if (*pnr_p != '+' && *pnr_p != '*') {
            CN_LOG_W("Comparison failure. FDN entry has TON=international, phone_number does not start with '+' or '*'");
            return false;
        }

        if (*pnr_p == '+') {
            CN_LOG_D("first char of phone number is '+', skipped.");
            pnr_p++;
        } else if (*pnr_p == '*') {
            CN_LOG_D("phone number starts with a '*', assuming SS string.");
            is_ss_str = 1;
        }
    }

    for (i = 0; i < fdn_number_length; i++) {
        switch (fdn_number_p[i]) {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '*':
        case '#':
            CN_LOG_V("fdn_number_p[%d]:%c *pnr_p:%c", i, fdn_number_p[i], *pnr_p);

            if (fdn_number_p[i] != *pnr_p) {
                if (is_ss_str && fdn_ton == STE_SIM_TON_INTERNATIONAL && (*pnr_p == '+')) {
                    CN_LOG_D("SS string with '+' in the middle, skipping character");
                    pnr_p++;

                    if (fdn_number_p[i] == *pnr_p) {
                        CN_LOG_D("current character match FDN position. continue with comparison.");
                        pnr_p++;
                        continue;
                    }
                }

                CN_LOG_E("Comparison failure. FDN character %d is %c, phone number has %c.", i, fdn_number_p[i], *pnr_p);
                return false;
            }

            break;
        case 'C':

            if ('p' != *pnr_p && 'w' != *pnr_p) {
                CN_LOG_E("Comparison failure. FDN character %d is DTMF control, phone number has %c (should be p or w).", i, *pnr_p);
                return false;
            }

            break;
        case 'D':

            if (0 == *pnr_p) {
                CN_LOG_E("Comparison failure. FDN character %d is a wild character, phone number has no digit in that place.", i);
                return false;
            }

            break;
        default:
            CN_LOG_E("FDN character %d has an illegal value (ascii value %d)", i, fdn_number_p[i]);
            return false;
            break;
        }

        pnr_p++;
    }

    CN_LOG_D("Number %s found to match!", phone_number_p);

    return true;
}

/* Return true if judged as equal numbers. Should in the future ignore differences as country codes etc. */
cn_bool_t cn_pdc_equal_numbers(char *number_p, char *reference_p)
{

    /* TODO: Improve. This is only the most basic comparison.*/
    /* TODO: Implement support for dialstring+dtmf for emergency numbers */
    if (NULL == number_p || NULL == reference_p) {
        CN_LOG_E("Null parameter in cn_pdc_equal_numbers!");
        return false;
    }

    CN_LOG_D("Matching %s vs. %s", number_p, reference_p);

    if (0 == strcmp(number_p, reference_p)) {
        return true;
    } else {
        return false;
    }
}

