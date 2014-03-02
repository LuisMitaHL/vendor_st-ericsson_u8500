/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "atc_exe_glue.h"
#include "atc_log.h"

#include "exe.h"
#include "exe_internal.h"
#include "exe_request_record_p.h"
#include "exe_requests.h"
#include "sim.h"
#include "cops.h"

#define EXE_STATE_SIM_GET_PROFILES          20
#define EXE_STATE_SIM_SAVE_SMCS             30
#define EXE_STATE_SIM_RESTORE_SMCS          40
#define EXE_STATE_SIM_FULL_STATE            50
#define EXE_STATE_SIM_FULL_REQUEST_APP      60
#define EXE_STATE_CAT_REGISTER_REQ          70
#define EXE_STATE_CAT_ENABLE_REQ            80
#define EXE_STATE_SIM_FULL_REQUEST_APP_RAW  90

/* EXE_STATE:s for read full PIN info */
#define EXE_STATE_FULL_PIN_INFO_PIN1        210
#define EXE_STATE_FULL_PIN_INFO_PIN2        211
#define EXE_STATE_FULL_PIN_INFO_PUK1        212
#define EXE_STATE_FULL_PIN_INFO_PUK2        213

/* EXE_STATE:s for read/write PLMN files */
#define EXE_STATE_SIM_PLMN_RETRY            21

/* EXE_STATE:s for getting FDN availability */
#define EXE_STATE_FDN_AVAILABLE             220

/* EXE_STATE:s for getting SPN availability */
#define EXE_STATE_SPN_AVAILABLE             221

#ifdef EXE_USE_SIMPB_SERVICE
/*EXE_STATE:s for getting phonebook response*/
#define EXE_STATE_PB                        222
#endif


/* EXE_STATE intermediate state for sim restart */
#define EXE_STATE_SIM_RESTART_INTERMEDIATE  2
#define EXE_SIM_NORMAL_COMPLETION           0

/* SMS-C: max digits in number */
#ifdef SMS_MAX_DIGITS_IN_NUMBER
#define SMSC_MAX_LENGTH                     SMS_MAX_DIGITS_IN_NUMBER
#else
#define SMSC_MAX_LENGTH                     (20)
#endif

static char full_sim_xml[1024];

/********************************************************************
 * Private prototypes
 ********************************************************************
 */
static exe_esimsr_state_t exe_convert_sim_state_to_esimsr_state(sim_state_t sim_state);
static bool sim_state_to_cpin_code_or_error_code(exe_cpin_sim_get_state_t *cpin_sim_get_state_p, sim_state_t sim_state);
static ste_sim_type_of_number_t exe_convert_type_of_number(exe_type_of_number_t exe_ton);
static const uint16_t sim_ef_dir_file_id = 0x2F00; /* Identifier for the EFdir file */

/********************************************************************
 * Private methods
 ********************************************************************
 */
exe_cmee_error_t convert_sim_status_codes_to_cmee_error(ste_uicc_status_code_t               uicc_status_code,
        ste_uicc_status_code_fail_details_t  uicc_status_code_fail_details,
        ste_uicc_status_word_t               status_word)
{
    exe_cmee_error_t cmee_error = CMEE_OK;
    ATC_LOG_I("%s: error start status = %d, fail-detail = %d, sw1= %d (0x%2X), sw2=%d (0x%2X)", __FUNCTION__, uicc_status_code,
              uicc_status_code_fail_details, status_word.sw1, status_word.sw1, status_word.sw2, status_word.sw2);

    switch (uicc_status_code) {
    case STE_UICC_STATUS_CODE_OK:
        cmee_error = CMEE_OK;
        break;

    case STE_UICC_STATUS_CODE_NOT_READY:
    case STE_UICC_STATUS_CODE_SHUTTING_DOWN:
    case STE_UICC_STATUS_CODE_CARD_NOT_READY:
        cmee_error = CMEE_SIM_BUSY;
        break;

    case STE_UICC_STATUS_CODE_PIN_ENABLED:
    case STE_UICC_STATUS_CODE_PIN_DISABLED:
        cmee_error = CMEE_OPERATION_NOT_ALLOWED;
        break;

    case STE_UICC_STATUS_CODE_CARD_DISCONNECTED:
    case STE_UICC_STATUS_CODE_CARD_NOT_PRESENT:
    case STE_UICC_STATUS_CODE_CARD_REJECTED:
        cmee_error = CMEE_SIM_NOT_INSERTED;
        break;

    case STE_UICC_STATUS_CODE_FAIL: {
        /* Check details in this case */
        switch (uicc_status_code_fail_details) {
        case STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS:

            if ((0x69 == status_word.sw1) &&
                    (0x83 == status_word.sw2)) {
                /* In this function we can not determine if it is PUK or PUK2 that is wrong.
                 * This must be handled in the AT parser.
                 */
                cmee_error = CMEE_SIM_PUK_REQUIRED;
            } else if (0x69 == status_word.sw1) {
                cmee_error = CMEE_OPERATION_NOT_ALLOWED;
            } else if ((0x98 == status_word.sw1) &&
                       (0x04 == status_word.sw2)) {
                cmee_error = CMEE_INCORRECT_PASSWORD;
            } else if ((0x98 == status_word.sw1) &&
                       (0x40 == status_word.sw2)) {
                /* In this function we can not determine if it is PUK or PUK2 that is wrong.
                 * This must be handled in the AT parser.
                 */
                cmee_error = CMEE_SIM_PUK_REQUIRED;
            } else if ((0x63 == status_word.sw1) &&
                       (0xC1 == status_word.sw2 || 0xC2 == status_word.sw2)) {
                /* In this function we can not determine if it is PIN, PIN2, PUK or PUK2 that is wrong.
                 * This must be handled in the AT parser.
                 */
                cmee_error = CMEE_SIM_PIN_REQUIRED;
            } else if ((0x63 == status_word.sw1) &&
                       ((0xC3 <= status_word.sw2) && (0xC9 >= status_word.sw2))) {
                /* In this function we can not determine if it is PUK or PUK2 that is wrong.
                 * This must be handled in the AT parser.
                 */
                cmee_error = CMEE_SIM_PUK_REQUIRED;
            } else if ((0x63 == status_word.sw1) &&
                       (0xC0 == status_word.sw2)) {
                cmee_error = CMEE_SIM_BLOCKED;
            } else if (0x62 == status_word.sw1) {
                cmee_error = CMEE_OPERATION_NOT_ALLOWED;
            } else {
                cmee_error = CMEE_UNKNOWN;
            }

            break;

        case STE_UICC_STATUS_CODE_FAIL_DETAILS_INVALID_PARAMETERS:
            cmee_error = CMEE_INCORRECT_PARAMETERS;
            break;

        case STE_UICC_STATUS_CODE_FAIL_DETAILS_FILE_NOT_FOUND:
            cmee_error = CMEE_NOT_FOUND;
            break;

        case STE_UICC_STATUS_CODE_FAIL_DETAILS_SECURITY_CONDITIONS_NOT_SATISFIED:
            cmee_error = CMEE_OPERATION_NOT_ALLOWED;
            break;

        case STE_UICC_STATUS_CODE_FAIL_DETAILS_SERVICE_NOT_SUPPORTED:
            cmee_error = CMEE_OPERATION_NOT_SUPPORTED;
            break;

        case STE_UICC_STATUS_CODE_FAIL_DETAILS_CARD_ERROR:
        case STE_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR:
            cmee_error = CMEE_SIM_FAILURE;
            break;

        default:
            ATC_LOG_I("convert default 2222: CMEE_UNKNOWN");
            cmee_error = CMEE_UNKNOWN;
            break;
        }

        break;
    }

    case STE_UICC_STATUS_CODE_UNKNOWN:
    case STE_UICC_STATUC_CODE_CARD_READY:
    case STE_UICC_STATUS_CODE_APPL_ACTIVE:
    case STE_UICC_STATUS_CODE_APPL_NOT_ACTIVE:
    default:
        cmee_error = CMEE_UNKNOWN;
        break;
    }

    return cmee_error;
}

static exe_cms_error_t convert_sim_status_codes_to_cms_error(ste_uicc_status_code_t uicc_status_code,
        ste_uicc_status_code_fail_details_t  uicc_status_code_fail_details,
        ste_uicc_status_word_t               status_word)
{
    exe_cms_error_t cms_error = EXE_CMS_OK;

    ATC_LOG_I("convert to cms: error start status = %d, fail-detail = %d, sw1= %d, sw2=%d", uicc_status_code,
              uicc_status_code_fail_details, status_word.sw1, status_word.sw2);

    switch (uicc_status_code) {
    case STE_UICC_STATUS_CODE_OK:
        cms_error = EXE_CMS_OK;
        break;

    case STE_UICC_STATUS_CODE_NOT_READY:
    case STE_UICC_STATUS_CODE_SHUTTING_DOWN:
    case STE_UICC_STATUS_CODE_CARD_NOT_READY:
        cms_error = EXE_CMS_SIM_BUSY;
        break;

    case STE_UICC_STATUS_CODE_PIN_ENABLED:
    case STE_UICC_STATUS_CODE_PIN_DISABLED:
        cms_error = EXE_CMS_OPERATION_NOT_ALLOWED;
        break;

    case STE_UICC_STATUS_CODE_CARD_DISCONNECTED:
    case STE_UICC_STATUS_CODE_CARD_NOT_PRESENT:
    case STE_UICC_STATUS_CODE_CARD_REJECTED:
        cms_error = EXE_CMS_SIM_NOT_INSERTED;
        break;

    case STE_UICC_STATUS_CODE_FAIL: {
        /* Check details in this case */
        switch (uicc_status_code_fail_details) {
        case STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS:
            cms_error = EXE_CMS_SIM_FAILURE;
            break;

        case STE_UICC_STATUS_CODE_FAIL_DETAILS_INVALID_PARAMETERS:
            cms_error = EXE_CMS_SIM_FAILURE;
            break;

        case STE_UICC_STATUS_CODE_FAIL_DETAILS_FILE_NOT_FOUND:
            cms_error = EXE_CMS_SIM_FAILURE;
            break;

        case STE_UICC_STATUS_CODE_FAIL_DETAILS_SECURITY_CONDITIONS_NOT_SATISFIED:
            cms_error = EXE_CMS_SIM_FAILURE;
            break;

        case STE_UICC_STATUS_CODE_FAIL_DETAILS_SERVICE_NOT_SUPPORTED:
            cms_error = EXE_CMS_OPERATION_NOT_SUPPORTED;
            break;

        case STE_UICC_STATUS_CODE_FAIL_DETAILS_CARD_ERROR:
        case STE_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR:
            cms_error = EXE_CMS_SIM_FAILURE;
            break;

        default:
            ATC_LOG_I("convert default 2222: CMS_UNKNOWN");
            cms_error = EXE_CMS_UNKNOWN_ERROR;
            break;
        }

        break;
    }

    case STE_UICC_STATUS_CODE_UNKNOWN:
    case STE_UICC_STATUC_CODE_CARD_READY:
    case STE_UICC_STATUS_CODE_APPL_ACTIVE:
    case STE_UICC_STATUS_CODE_APPL_NOT_ACTIVE:
    default:
        cms_error = EXE_CMS_UNKNOWN_ERROR;
        break;
    }

    return cms_error;
}

/*
 * sim_state_to_cpin_code_or_error_code()
 * The function convert a sim state received from the modem to the corresponding cpin code or a cmee error.
 * If a illegal sim status is received the function return false and set the phone_locks_p value to an
 * out of range enum value.
 *
 * return: true if successfully converted, false if no match.
 */
static bool sim_state_to_cpin_code_or_error_code(exe_cpin_sim_get_state_t *cpin_sim_get_state_p, sim_state_t sim_state)
{
    bool result = false;

    if (NULL == cpin_sim_get_state_p) {
        return false;
    }

    /*  default values */
    cpin_sim_get_state_p->error_code = CMEE_OK;
    cpin_sim_get_state_p->code = EXE_CPIN_CODE_NO_CODE;

    switch (sim_state) {
    case SIM_STATE_PIN_NEEDED:
        cpin_sim_get_state_p->code = EXE_CPIN_CODE_SIM_PIN;
        cpin_sim_get_state_p->error_code = CMEE_SIM_PIN_REQUIRED;
        result = true;
        break;

    case SIM_STATE_READY:
        cpin_sim_get_state_p->code = EXE_CPIN_CODE_READY;
        result = true;
        break;

    case SIM_STATE_PIN2_NEEDED:
        cpin_sim_get_state_p->code = EXE_CPIN_CODE_SIM_PIN2;
        cpin_sim_get_state_p->error_code = CMEE_SIM_PIN2_REQUIRED;
        result = true;
        break;

    case SIM_STATE_PUK_NEEDED:
        cpin_sim_get_state_p->code = EXE_CPIN_CODE_SIM_PUK;
        cpin_sim_get_state_p->error_code = CMEE_SIM_PUK_REQUIRED;
        result = true;
        break;

    case SIM_STATE_PUK2_NEEDED:
        cpin_sim_get_state_p->code = EXE_CPIN_CODE_SIM_PUK2;
        cpin_sim_get_state_p->error_code = CMEE_SIM_PUK2_REQUIRED;
        result = true;
        break;

    case SIM_STATE_PERMANENTLY_BLOCKED:
        cpin_sim_get_state_p->code = EXE_CPIN_CODE_BLOCKED;
        result = true;
        break;

    case SIM_STATE_REJECTED_CARD_SIM_LOCK: {
        cops_context_id_t *id_p;
        cops_simlock_status_t simlock_status;
        cops_return_code_t cops_result = COPS_RC_OK;
        exe_t *exe_p = atc_get_exe_handle();

        /* Set to permanently blocked if none of the SIM locks below are set */

        cpin_sim_get_state_p->code = EXE_CPIN_CODE_BLOCKED;
        cpin_sim_get_state_p->error_code = CMEE_SIM_FAILURE;
        result = true;

        if (NULL == exe_p) {
            ATC_LOG_E("%s, could not get the executor object", __func__);
            break;
        }

        id_p = (cops_context_id_t *) copsclient_get_id(exe_get_copsclient(exe_p));

        if (NULL == id_p) {
            ATC_LOG_E("%s, could not get copsclient id.", __func__);
            break;
        }

        cops_result = cops_simlock_get_status(id_p, &simlock_status);

        if (COPS_RC_OK != cops_result) {
            ATC_LOG_E("%s, cops_simlock_get_status failed", __func__);
        } else if (simlock_status.nl_status.lock_setting == COPS_SIMLOCK_LOCK_SETTING_LOCKED) {
            cpin_sim_get_state_p->code = EXE_CPIN_CODE_PH_NET_PIN;

            /* When number of attempts reaches 0 a timer is set in COPS
             * that when it expires restores number of attempts to 1. */
            cpin_sim_get_state_p->error_code = CMEE_PH_NET_PIN_REQUIRED;

        } else if (simlock_status.nsl_status.lock_setting == COPS_SIMLOCK_LOCK_SETTING_LOCKED) {
            cpin_sim_get_state_p->code = EXE_CPIN_CODE_PH_NETSUB_PIN;

            if (simlock_status.nsl_status.attempts_left > 0) {
                cpin_sim_get_state_p->error_code = CMEE_PH_NETSUB_PIN_REQUIRED;
            } else {
                cpin_sim_get_state_p->error_code = CMEE_PH_NETSUB_PUK_REQUIRED;
            }

        } else if (simlock_status.spl_status.lock_setting == COPS_SIMLOCK_LOCK_SETTING_LOCKED) {
            cpin_sim_get_state_p->code = EXE_CPIN_CODE_PH_SP_PIN;

            if (simlock_status.spl_status.attempts_left > 0) {
                cpin_sim_get_state_p->error_code = CMEE_PH_SP_PIN_REQUIRED;
            } else {
                cpin_sim_get_state_p->error_code = CMEE_PH_SP_PUK_REQUIRED;
            }

        } else if (simlock_status.cl_status.lock_setting == COPS_SIMLOCK_LOCK_SETTING_LOCKED) {
            cpin_sim_get_state_p->code = EXE_CPIN_CODE_PH_CORP_PIN;

            if (simlock_status.cl_status.attempts_left > 0) {
                cpin_sim_get_state_p->error_code = CMEE_PH_CORP_PIN_REQUIRED;
            } else {
                cpin_sim_get_state_p->error_code = CMEE_PH_CORP_PUK_REQUIRED;
            }

        } else if (simlock_status.siml_status.lock_setting == COPS_SIMLOCK_LOCK_SETTING_LOCKED) {
            cpin_sim_get_state_p->code = EXE_CPIN_CODE_PH_SIMLOCK_PIN;
            cpin_sim_get_state_p->error_code = CMEE_PH_SIMLOCK_PIN_REQUIRED;
        } else if (simlock_status.esll_status.lock_setting == COPS_SIMLOCK_LOCK_SETTING_LOCKED) {
            cpin_sim_get_state_p->code = EXE_CPIN_CODE_PH_ESL_PIN;
            cpin_sim_get_state_p->error_code = CMEE_OK;
        } else {
            cpin_sim_get_state_p->error_code = CMEE_SIM_FAILURE;
            ATC_LOG_I("%s, No active SIM locks.", __func__);
        }
    }
    break;

    case SIM_STATE_NOT_READY:
        cpin_sim_get_state_p->error_code = CMEE_SIM_BUSY;
        result = true;
        break;

    case SIM_STATE_UNKNOWN:
    case SIM_STATE_REJECTED_CARD_INVALID:
        cpin_sim_get_state_p->error_code = CMEE_SIM_FAILURE;
        result = true;
        break;

    case SIM_STATE_REJECTED_CARD_CONSECUTIVE_6F00:
        ATC_LOG_I("SIM card was rejected since it returned 6F00 three times");
        cpin_sim_get_state_p->error_code = CMEE_SIM_WRONG;
        result = true;
        break;

    case SIM_STATE_SAP:
    case SIM_STATE_SIM_ABSENT:
    case SIM_STATE_DISCONNECTED_CARD:
        cpin_sim_get_state_p->error_code = CMEE_SIM_NOT_INSERTED;
        result = true;
        break;

    default:
        cpin_sim_get_state_p->code = CMEE_SIM_FAILURE;
        result = false;
        break;
    }

    return result;
}


static exe_esimsr_state_t exe_convert_sim_state_to_esimsr_state(sim_state_t sim_state)
{
    exe_esimsr_state_t exe_sim_state;

    switch (sim_state) {
    case SIM_STATE_SIM_ABSENT:
        exe_sim_state = EXE_SIM_STATE_POWER_OFF;
        break;

    case SIM_STATE_NOT_READY:
        exe_sim_state = EXE_SIM_STATE_AWAITING_APPLICATION_SELECTION;
        break;

    case SIM_STATE_PIN_NEEDED:
        exe_sim_state = EXE_SIM_STATE_WAIT_FOR_PIN;
        break;

    case SIM_STATE_READY:
        exe_sim_state = EXE_SIM_STATE_ACTIVE;
        break;

    case SIM_STATE_PUK_NEEDED:
    case SIM_STATE_REJECTED_CARD_SIM_LOCK:
        exe_sim_state = EXE_SIM_STATE_BLOCKED;
        break;

    case SIM_STATE_PERMANENTLY_BLOCKED:
        exe_sim_state = EXE_SIM_STATE_BLOCKED_FOREVER;
        break;

    case SIM_STATE_UNKNOWN:
    case SIM_STATE_REJECTED_CARD_INVALID:
    case SIM_STATE_REJECTED_CARD_CONSECUTIVE_6F00:
    case SIM_STATE_DISCONNECTED_CARD:
    case SIM_STATE_PIN2_NEEDED:
    case SIM_STATE_PUK2_NEEDED:
    case SIM_STATE_SAP:
    default:
        exe_sim_state = EXE_SIM_STATE_NULL;
        break;
    }

    return exe_sim_state;
}

static ste_sim_type_of_number_t exe_convert_type_of_number(exe_type_of_number_t exe_ton)
{
    ste_sim_type_of_number_t sim_ton;

    switch (exe_ton) {
    case EXE_TYPE_OF_NUMBER_NATIONAL:
        sim_ton = STE_SIM_TON_NATIONAL;
        break;

    case EXE_TYPE_OF_NUMBER_INTERNATIONAL:
        sim_ton = STE_SIM_TON_INTERNATIONAL;
        break;

    default:
        sim_ton = STE_SIM_TON_UNKNOWN;
        break;
    }

    return sim_ton;
}


/********************************************************************
 * Public methods
 ********************************************************************
 */


/********************************************************************
 * SIM
 ********************************************************************
 */

/*
 * Enable / disable / query sim lock service.
 */
exe_request_result_t request_sim_lock(exe_request_record_t *record_p)
{
    uicc_request_status_t result = UICC_REQUEST_STATUS_OK;
    exe_clck_mode_t *saved_mode_p = NULL;

    if (EXE_STATE_REQUEST == record_p->state) {
        ATC_LOG_I("request_sim_lock -> request");
        exe_clck_t *clck_data_p = NULL;
        sim_client_t *simclient_p = exe_get_sim_client(record_p->exe_p);

        if (NULL == simclient_p) {
            ATC_LOG_E("request_sim_lock, could not get sim client.");
            goto error;
        }

        clck_data_p = (exe_clck_t *) record_p->request_data_p;

        if (NULL == clck_data_p) {
            ATC_LOG_E("request_sim_lock: sim_pin_info_p is null");
            return EXE_FAILURE;
        }

        ste_sim_t *uicc_p = sim_client_get_handle(simclient_p);

        if (NULL == uicc_p) {
            ATC_LOG_E("request_sim_lock, could not get sim handle.");
            goto error;
        }

        if (EXE_CLCK_MODE_LOCK == clck_data_p->mode) {
            result = ste_uicc_pin_enable(uicc_p,
                                         (uintptr_t)request_record_get_client_tag(record_p),
                                         clck_data_p->passwd_p);
        } else if (EXE_CLCK_MODE_UNLOCK == clck_data_p->mode) {
            result = ste_uicc_pin_disable(uicc_p,
                                          (uintptr_t)request_record_get_client_tag(record_p),
                                          clck_data_p->passwd_p);
        } else { /* EXE_CLCK_MODE_QUERY_STATUS */
            result = ste_uicc_pin_info(uicc_p,
                                       (uintptr_t)request_record_get_client_tag(record_p),
                                       SIM_PIN_PIN1);
        }

        if (UICC_REQUEST_STATUS_OK != result) {
            ATC_LOG_E("request_sim_lock, ste_uicc_pin_enable/disable/query failed, error code %d.", result);
            goto error;
        }

        /* Need to save mode for to use when we get the response */
        saved_mode_p = malloc(sizeof(exe_clck_mode_t));

        if (NULL == saved_mode_p) {
            ATC_LOG_E("request_sim_lock: could not allocate memory");
            goto error;
        }

        *saved_mode_p = clck_data_p->mode;
        record_p->client_data_p = saved_mode_p;

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    } else {
        /* Response from SIM */
        ATC_LOG_I("request_sim_lock <- response");
        exe_clck_t clck_data;

        if (true == record_p->abort) {
            ATC_LOG_E("request_sim_lock: Abort request has arrived. Hence returning!");
            /* NOTE: Currently there is no support for aborting this type of request */
            goto error;
        }

        if (record_p->client_result_code) {

            saved_mode_p = (exe_clck_mode_t *)record_p->client_data_p;
            clck_data.mode = *saved_mode_p;
            free(saved_mode_p);  /* Free this data directly as it is saved in mode*/

            clck_data.status = 0;

            if (EXE_CLCK_MODE_LOCK == clck_data.mode) { /* enable pin done */
                ste_uicc_pin_enable_response_t *pin_enable_data_p = NULL;
                pin_enable_data_p = (ste_uicc_pin_enable_response_t *)(record_p->response_data_p);

                if (NULL == pin_enable_data_p) {
                    ATC_LOG_E("request_sim_lock: get_state_data = NULL");
                    goto error;
                }

                clck_data.status = convert_sim_status_codes_to_cmee_error(pin_enable_data_p->uicc_status_code,
                                   pin_enable_data_p->uicc_status_code_fail_details,
                                   pin_enable_data_p->status_word);

            } else if (EXE_CLCK_MODE_UNLOCK == clck_data.mode) { /* disable pin done */
                ste_uicc_pin_disable_response_t *pin_disable_data_p = NULL;
                pin_disable_data_p = (ste_uicc_pin_disable_response_t *)(record_p->response_data_p);

                if (NULL == pin_disable_data_p) {
                    ATC_LOG_E("request_sim_lock: get_state_data = NULL");
                    goto error;
                }

                clck_data.status = convert_sim_status_codes_to_cmee_error(pin_disable_data_p->uicc_status_code,
                                   pin_disable_data_p->uicc_status_code_fail_details,
                                   pin_disable_data_p->status_word);

            } else { /* EXE_CLCK_MODE_QUERY_STATUS */
                ste_uicc_pin_info_response_t *pin_data_p = NULL;
                pin_data_p = (ste_uicc_pin_info_response_t *)(record_p->response_data_p);
                ste_uicc_status_word_t fake_status = {0, 0};

                if (NULL == pin_data_p) {
                    ATC_LOG_E("request_sim_lock: get_state_data = NULL");
                    goto error;
                }

                clck_data.status = convert_sim_status_codes_to_cmee_error(pin_data_p->uicc_status_code,
                                   pin_data_p->uicc_status_code_fail_details,
                                   fake_status);

                /* Convert ste_uicc_pin_status_t to clck_mode_t  */
                switch (pin_data_p->pin_status) {
                case STE_UICC_PIN_STATUS_ENABLED: {
                    clck_data.lock_status = EXE_CLCK_STATUS_ACTIVE;
                    break;
                }

                case STE_UICC_PIN_STATUS_DISABLED: {
                    clck_data.lock_status = EXE_CLCK_STATUS_NOT_ACTIVE;
                    break;
                }

                case STE_UICC_PIN_STATUS_UNKNOWN:
                default: {
                    clck_data.lock_status = EXE_CLCK_STATUS_NOT_AVAILABLE;
                    break;
                }
                }
            }
        } else {
            clck_data.lock_status = EXE_CLCK_STATUS_NOT_AVAILABLE;
            clck_data.status = CMEE_OPERATION_NOT_ALLOWED;
        }

        exe_request_complete(record_p, EXE_SUCCESS, &clck_data);
        return EXE_SUCCESS;
    }

error:

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, NULL);
    }

    return EXE_FAILURE;
}

/*
 * Enable / disable / query fixed dialing service.
 */
exe_request_result_t request_fixed_dialing(exe_request_record_t *record_p)
{
    uicc_request_status_t result = UICC_REQUEST_STATUS_OK;
    sim_client_t *simclient_p = exe_get_sim_client(record_p->exe_p);
    ste_sim_t *uicc_p = NULL;

    if (true == record_p->abort) {
        ATC_LOG_E("request_sim_lock: Abort request has arrived. Hence returning!");
        /* NOTE: Currently there is no support for aborting this type of request */
        goto error;
    }

    switch (record_p->state) {
    case EXE_STATE_REQUEST: {
        ATC_LOG_I("service available -> request");

        if (NULL == simclient_p) {
            ATC_LOG_E("could not get sim client.");
            goto error;
        }

        if (NULL == record_p->request_data_p) {
            ATC_LOG_E("sim_pin_info_p is null");
            return EXE_FAILURE;
        }

        record_p->client_data_p = malloc(sizeof(exe_clck_t));

        if (NULL == record_p->client_data_p) {
            ATC_LOG_E("Could not allocate memory");
            return EXE_FAILURE;
        }

        memmove(record_p->client_data_p, record_p->request_data_p,
                sizeof(exe_clck_t));

        uicc_p = sim_client_get_handle(simclient_p);

        if (NULL == uicc_p) {
            ATC_LOG_E("could not get sim handle.");
            goto error;
        }

        result = ste_uicc_get_service_availability(uicc_p,
                 (uintptr_t) request_record_get_client_tag(record_p),
                 SIM_SERVICE_TYPE_FDN);

        if (UICC_REQUEST_STATUS_OK != result) {
            ATC_LOG_E("ste_uicc_pin_enable/disable failed, error code %d.", result);
            goto error;
        }

        record_p->state = EXE_STATE_FDN_AVAILABLE;
        return EXE_PENDING;
    }

    case EXE_STATE_FDN_AVAILABLE: {
        uint8_t enable_service = 1;
        exe_clck_t *clck_data_p = (exe_clck_t *) record_p->client_data_p;
        ste_uicc_get_service_availability_response_t *service_rsp_p = record_p->response_data_p;

        if (STE_UICC_STATUS_CODE_OK != service_rsp_p->uicc_status_code) {
            exe_clck_t clck_data;
            ste_uicc_status_word_t status_word = {0, 0};

            ATC_LOG_E("Get service available failed, status: %d, details: %d",
                      service_rsp_p->uicc_status_code,
                      service_rsp_p->uicc_status_code_fail_details);

            clck_data.lock_status = EXE_CLCK_STATUS_NOT_AVAILABLE;
            clck_data.status = convert_sim_status_codes_to_cmee_error(
                                   service_rsp_p->uicc_status_code,
                                   service_rsp_p->uicc_status_code_fail_details,
                                   status_word);

            free(record_p->client_data_p);
            exe_request_complete(record_p, EXE_FAILURE, &clck_data);
            return EXE_FAILURE;
        }

        if (service_rsp_p->service_availability == STE_UICC_SERVICE_AVAILABLE) {

            uicc_p = sim_client_get_handle(simclient_p);

            if (NULL == uicc_p) {
                ATC_LOG_E("could not get sim handle.");
                goto error;
            }

            if (EXE_CLCK_MODE_QUERY_STATUS == clck_data_p->mode) {
                ATC_LOG_I("get service table -> request");
                result = ste_uicc_get_service_table(uicc_p,
                                                    (uintptr_t) request_record_get_client_tag(record_p),
                                                    SIM_SERVICE_TYPE_FDN);
            } else {
                if (EXE_CLCK_MODE_LOCK == clck_data_p->mode) {
                    enable_service = 1;
                } else {
                    enable_service = 0;
                }

                ATC_LOG_I("update service table -> request");
                result = ste_uicc_update_service_table(uicc_p,
                                                       (uintptr_t)request_record_get_client_tag(record_p),
                                                       clck_data_p->passwd_p,
                                                       SIM_SERVICE_TYPE_FDN,
                                                       enable_service);
            }

            if (UICC_REQUEST_STATUS_OK != result) {
                ATC_LOG_E("service table request failed, error code %d.",
                          result);
                goto error;
            }

        } else {
            exe_clck_t clck_data;
            clck_data.lock_status = EXE_CLCK_STATUS_NOT_AVAILABLE;

            if (EXE_CLCK_MODE_QUERY_STATUS != clck_data_p->mode) {
                clck_data.status = CMEE_OPERATION_NOT_SUPPORTED;
            } else {
                clck_data.status = CMEE_OK;
            }

            free(record_p->client_data_p);
            exe_request_complete(record_p, EXE_SUCCESS, &clck_data);
            return EXE_SUCCESS;
        }

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    }

    case EXE_STATE_RESPONSE: {
        /* Response from SIM */
        exe_clck_t clck_data;
        clck_data.mode = *(exe_clck_mode_t *)record_p->client_data_p;
        free(record_p->client_data_p);
        record_p->client_data_p = NULL;

        clck_data.status = 0;

        ATC_LOG_I("service table <- response");

        if (record_p->client_result_code) {
            if (EXE_CLCK_MODE_QUERY_STATUS != clck_data.mode) {
                ste_uicc_update_service_table_response_t *fixed_dialing_response_p = NULL;
                fixed_dialing_response_p = (ste_uicc_update_service_table_response_t *)(record_p->response_data_p);

                if (NULL == fixed_dialing_response_p) {
                    ATC_LOG_E("get_state_data = NULL");
                    goto error;
                }

                clck_data.status =
                    convert_sim_status_codes_to_cmee_error(
                        fixed_dialing_response_p->uicc_status_code,
                        fixed_dialing_response_p->uicc_status_code_fail_details,
                        fixed_dialing_response_p->status_word);

            } else {
                ste_uicc_get_service_table_response_t *fixed_dialing_response_p = NULL;
                fixed_dialing_response_p =
                    (ste_uicc_get_service_table_response_t *)(record_p->response_data_p);

                if (NULL == fixed_dialing_response_p) {
                    ATC_LOG_E("get_state_data = NULL");
                    goto error;
                }

                clck_data.status =
                    convert_sim_status_codes_to_cmee_error(
                        fixed_dialing_response_p->uicc_status_code,
                        fixed_dialing_response_p->uicc_status_code_fail_details,
                        fixed_dialing_response_p->status_word);

                /* Convert ste_uicc_pin_status_t to clck_mode_t  */
                switch (fixed_dialing_response_p->service_status) {
                case STE_UICC_SERVICE_STATUS_ENABLED: {
                    clck_data.lock_status = EXE_CLCK_STATUS_ACTIVE;
                    break;
                }

                case STE_UICC_SERVICE_STATUS_DISABLED: {
                    clck_data.lock_status = EXE_CLCK_STATUS_NOT_ACTIVE;
                    break;
                }

                case STE_UICC_SERVICE_STATUS_UNKNOWN:
                default: {
                    clck_data.lock_status = EXE_CLCK_STATUS_NOT_AVAILABLE;
                    break;
                }
                }
            }
        } else {
            clck_data.lock_status = EXE_CLCK_STATUS_NOT_AVAILABLE;
            clck_data.status = CMEE_OPERATION_NOT_ALLOWED;
        }

        exe_request_complete(record_p, EXE_SUCCESS, &clck_data);
        return EXE_SUCCESS;
    }

    default:
        ATC_LOG_E("Unhandled message state: %d", record_p->state);
        break;
    }

error:

    free(record_p->client_data_p);

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, NULL);
    }

    return EXE_FAILURE;
}


/* TODO This request shall be merged with the other cpin related requests into one. */
/*
 * Get the current sim state for cpin.
 */
exe_request_result_t request_get_sim_state_cpin(exe_request_record_t *record_p)
{
    uicc_request_status_t result = UICC_REQUEST_STATUS_OK;

    if (EXE_STATE_REQUEST == record_p->state) {
        ATC_LOG_I("request_get_sim_state_cpin -> request");
        sim_client_t *simclient_p = exe_get_sim_client(record_p->exe_p);

        if (NULL == simclient_p) {
            ATC_LOG_E("request_get_sim_state_cpin, could not get sim client.");
            goto error;
        }

        ste_sim_t *uicc_p = sim_client_get_handle(simclient_p);

        if (NULL == uicc_p) {
            ATC_LOG_E("request_get_sim_state_cpin, could not get sim handle.");
            goto error;
        }

        result = ste_uicc_sim_get_state(uicc_p, (uintptr_t)request_record_get_client_tag(record_p));

        if (UICC_REQUEST_STATUS_OK != result) {
            ATC_LOG_E("request_get_sim_state_cpin, ste_uicc_sim_get_state failed, error code %d.", result);
            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    } else {
        /* Response from SIM */
        ste_uicc_get_sim_state_response_t *get_state_data = NULL;
        exe_cpin_sim_get_state_t get_state;
        ATC_LOG_I("request_get_sim_state_cpin <- response");
        get_state_data = (ste_uicc_get_sim_state_response_t *)(record_p->response_data_p);

        if (NULL == get_state_data) {
            ATC_LOG_E("request_get_sim_state_cpin: get_state_data = NULL");
            goto error;
        }

        if (get_state_data->error_cause != EXE_SIM_NORMAL_COMPLETION) {
            goto error;
        }

        EXE_CHECK_GOTO_ERROR(sim_state_to_cpin_code_or_error_code(&get_state, get_state_data->state));
        exe_request_complete(record_p, EXE_SUCCESS, &get_state);
        return EXE_SUCCESS;
    }

error:

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, NULL);
    }

    return EXE_FAILURE;
}

static bool exe_sim_encode_plmn(exe_sim_cpol_info_t *cpolinfo, ste_sim_plmn_with_AcT_t **plmn_pp)
{
    if (strlen(cpolinfo->oper_string) > 0) {
        /*operator is set => parse and set plmn data */
        ste_sim_plmn_with_AcT_t *plmn_p;
        /* Parse the input data into a struct for sending to sim */
        plmn_p = calloc(1, sizeof(ste_sim_plmn_with_AcT_t));

        if (!plmn_p) {
            ATC_LOG_E("%s, memory allocation error!", __func__);
            *plmn_pp = NULL;
            return false;
        }

        plmn_p->AcT = cpolinfo->gsm_act * SIM_ACT_GSM_BITMASK + cpolinfo->gsm_compact_act * SIM_ACT_GSM_COMPACT_BITMASK + cpolinfo->utran_act * SIM_ACT_UTRAN_BITMASK;

        /* First three characters are always MCC1-3 */
        plmn_p->PLMN.MCC1 = cpolinfo->oper_string[0] - 0x30;
        plmn_p->PLMN.MCC2 = cpolinfo->oper_string[1] - 0x30;
        plmn_p->PLMN.MCC3 = cpolinfo->oper_string[2] - 0x30;
        plmn_p->PLMN.MNC1 = cpolinfo->oper_string[3] - 0x30;
        plmn_p->PLMN.MNC2 = cpolinfo->oper_string[4] - 0x30;

        /* MNC format depends on whether string is five or six characters long */
        if (cpolinfo->oper_string[5] < 0x30) {
            /* five characters => MNC is only two characters */
            plmn_p->PLMN.MNC3 = 0xF;
        } else {
            plmn_p->PLMN.MNC3 = cpolinfo->oper_string[5] - 0x30;
        }

        *plmn_pp = plmn_p;
    } else {
        /* no operator string is set => remove the entry with index index */
        *plmn_pp = NULL;
    }

    return true;
}

static bool is_plmn_valid(const ste_uicc_sim_plmn_t *const i_p)
{
    return (
               (i_p->MCC1 <= 9) &&
               (i_p->MCC2 <= 9) &&
               (i_p->MCC3 <= 9) &&
               (i_p->MNC3 <= 9 || i_p->MNC3 == 0xf) &&
               (i_p->MNC1 <= 9) &&
               (i_p->MNC2 <= 9));
}

static void release_cpol_response_list(exe_sim_cpol_info_response_t *response_p)
{
    exe_sim_cpol_info_response_t *tmp = response_p;
    exe_sim_cpol_info_response_t *tmp2 = NULL;

    while (tmp != NULL) {
        tmp2 = tmp->next_p;
        free(tmp);
        tmp = tmp2;
    }

}

exe_request_result_t request_set_cpol_data(exe_request_record_t *record_p)
{
    uicc_request_status_t result = UICC_REQUEST_STATUS_OK;
    exe_cmee_error_t cmee_error_code = CMEE_UNKNOWN;
    ste_sim_plmn_with_AcT_t *plmn_p = NULL;
    ste_sim_t *uicc_p = NULL;
    sim_client_t *simclient_p = NULL;
    exe_sim_cpol_info_t *exe_cpol_data_p = NULL;

    switch (record_p->state) {
    case EXE_STATE_REQUEST: {
        ATC_LOG_I("request_set_cpol_data -> request");
        /* Get sim interface and check validity */
        simclient_p = exe_get_sim_client(record_p->exe_p);

        if (NULL == simclient_p) {
            ATC_LOG_E("request_get_sim_state, could not get sim client.");
            goto error;
        }

        uicc_p = sim_client_get_handle(simclient_p);

        if (NULL == uicc_p) {
            ATC_LOG_E("request_get_sim_state, could not get sim handle.");
            goto error;
        }

        exe_sim_cpol_info_t *cpolinfo_p = (exe_sim_cpol_info_t *) record_p->request_data_p;
        exe_cpol_data_p = calloc(1, sizeof(exe_sim_cpol_info_t));

        if (!exe_cpol_data_p) {
            ATC_LOG_E("%s, memory allocation error!", __func__);
            goto error;
        }

        memcpy(exe_cpol_data_p, cpolinfo_p, sizeof(exe_sim_cpol_info_t));

        if (!exe_sim_encode_plmn(exe_cpol_data_p, &plmn_p)) {
            goto error;
        }

        /* The input cases are checked as far as possible in the parsing implementation.  */
        /* Call assumes that index is valid, or that an invalid index will be rejected by the
         * sim implementation in an orderly fashion. */
        result = ste_uicc_sim_file_update_plmn(uicc_p, (uintptr_t) request_record_get_client_tag(
                record_p), exe_cpol_data_p->index, plmn_p, SIM_FILE_ID_PLMN_WACT);

        record_p->request_data_p = exe_cpol_data_p;

        if (UICC_REQUEST_STATUS_OK != result) {
            ATC_LOG_E("request_set_cpol_data, ste_uicc_sim_file_update_plmn failed, error code %d.", result);
            goto error;
        }

        free(plmn_p);
        plmn_p = NULL;
        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
        break;
    }

    case EXE_STATE_RESPONSE:
    case EXE_STATE_SIM_PLMN_RETRY: {
        ATC_LOG_I("request_set_cpol_data <- response");

        if (record_p->client_result_code) {

            exe_cpol_data_p = (exe_sim_cpol_info_t *) record_p->request_data_p;
            ste_uicc_update_sim_file_plmn_response_t *resp =
                (ste_uicc_update_sim_file_plmn_response_t *)(record_p->response_data_p);

            if (STE_UICC_STATUS_CODE_FAIL == resp->uicc_status_code
                    && STE_UICC_STATUS_CODE_FAIL_DETAILS_FILE_NOT_FOUND
                    == resp->uicc_status_code_fail_details &&
                    EXE_STATE_SIM_PLMN_RETRY != record_p->state) {
                /* Retry using PLMNsel file */

                if (!exe_sim_encode_plmn(exe_cpol_data_p, &plmn_p)) {
                    goto error;
                }

                simclient_p = exe_get_sim_client(record_p->exe_p);
                uicc_p = sim_client_get_handle(simclient_p);
                result = ste_uicc_sim_file_update_plmn(uicc_p,
                                                       (uintptr_t) request_record_get_client_tag(record_p),
                                                       exe_cpol_data_p->index, plmn_p, SIM_FILE_ID_PLMN_SEL);

                if (UICC_REQUEST_STATUS_OK != result) {
                    ATC_LOG_E("request_set_cpol_data, ste_uicc_sim_file_update_plmn failed, error code %d.", result);
                    goto error;
                }

                free(plmn_p);
                plmn_p = NULL;

                record_p->state = EXE_STATE_SIM_PLMN_RETRY;
                return EXE_PENDING;
            }

            cmee_error_code = convert_sim_status_codes_to_cmee_error(resp->uicc_status_code,
                              resp->uicc_status_code_fail_details, resp->status_word);

            if (cmee_error_code == CMEE_OK) {
                free(exe_cpol_data_p);
                exe_request_complete(record_p, EXE_SUCCESS, NULL);
                return EXE_SUCCESS;
            } else {
                ATC_LOG_E("%s: Status=%d, Status details= %d", __FUNCTION__, resp->uicc_status_code, resp->uicc_status_code_fail_details);
                goto error;
            }

        } else {
            cmee_error_code = CMEE_OPERATION_NOT_ALLOWED;
            goto error;
        }
    }

    default:
        ATC_LOG_E("%s: Unknown state: %d", __FUNCTION__, record_p->state);
        goto error;

    }

error:

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, &cmee_error_code);
    }

    free(exe_cpol_data_p);
    free(plmn_p);

    return EXE_FAILURE;
}


/*
 * Get preferred operator list from SIM. (For the AT+CPOL command)
 *
 * Note: does not currently support any other format than the numeric (format=2), and
 * will always return data on numeric format.
 */
exe_request_result_t request_get_cpol_data(exe_request_record_t *record_p)
{
    uicc_request_status_t result = UICC_REQUEST_STATUS_OK;
    exe_sim_cpol_info_response_t *response_p = NULL;
    exe_sim_cpol_info_response_t *last_response_p = NULL;
    exe_cmee_error_t cmee_error_code = CMEE_UNKNOWN;
    sim_client_t *simclient_p = NULL;
    ste_sim_t *uicc_p = NULL;

    switch (record_p->state) {
    case EXE_STATE_REQUEST: {
        ATC_LOG_I("request_get_cpol_data -> request");

        /* Get sim interface and check validity */
        simclient_p = exe_get_sim_client(record_p->exe_p);

        if (NULL == simclient_p) {
            ATC_LOG_E("request_get_sim_state, could not get sim client.");
            goto error;
        }

        uicc_p = sim_client_get_handle(simclient_p);

        if (NULL == uicc_p) {
            ATC_LOG_E("request_get_sim_state, could not get sim handle.");
            goto error;
        }

        /* Make request to sim to get all the CPOL data */
        result = ste_uicc_sim_file_read_plmn(uicc_p,
                                             (uintptr_t) request_record_get_client_tag(record_p), SIM_FILE_ID_PLMN_WACT);

        if (UICC_REQUEST_STATUS_OK != result) {
            ATC_LOG_E("request_get_sim_state, ste_uicc_sim_get_state failed, error code %d.", result);
            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
        break;
    }

    case EXE_STATE_RESPONSE:
    case EXE_STATE_SIM_PLMN_RETRY: {

        /* Response from SIM */
        ATC_LOG_I("request_get_cpol_data <- response");

        if (record_p->client_result_code) {
            int i = 0;
            ste_sim_plmn_with_AcT_t *plmn = NULL;

            /* Get all data and check result of call */
            ste_uicc_sim_file_read_plmn_response_t *get_cpol_data_p =
                (ste_uicc_sim_file_read_plmn_response_t *)(record_p->response_data_p);

            if (NULL == get_cpol_data_p) {
                ATC_LOG_E("request_get_cpol_data: get_cpol_data_p = NULL");
                goto error;
            }

            if (STE_UICC_STATUS_CODE_FAIL == get_cpol_data_p->uicc_status_code &&
                    STE_UICC_STATUS_CODE_FAIL_DETAILS_FILE_NOT_FOUND == get_cpol_data_p->uicc_status_code_fail_details &&
                    EXE_STATE_SIM_PLMN_RETRY != record_p->state) {
                // retry using EFplmnsel
                record_p->state = EXE_STATE_SIM_PLMN_RETRY;
                simclient_p = exe_get_sim_client(record_p->exe_p);
                uicc_p = sim_client_get_handle(simclient_p);
                result = ste_uicc_sim_file_read_plmn(uicc_p,
                                                     (uintptr_t) request_record_get_client_tag(record_p), SIM_FILE_ID_PLMN_SEL);

                if (UICC_REQUEST_STATUS_OK != result) {
                    ATC_LOG_E("request_get_sim_state, ste_uicc_sim_get_state failed, error code %d.", result);
                    goto error;
                }

                return EXE_PENDING;
            }

            cmee_error_code = convert_sim_status_codes_to_cmee_error(
                                  get_cpol_data_p->uicc_status_code,
                                  get_cpol_data_p->uicc_status_code_fail_details, get_cpol_data_p->status_word);

            if (cmee_error_code != CMEE_OK) {
                ATC_LOG_E("%s: Status=%d, Status details= %d", __FUNCTION__,
                          get_cpol_data_p->uicc_status_code, get_cpol_data_p->uicc_status_code_fail_details);
                goto error;
            }

            /* For each returned record :*/
            for (i = 0; i < get_cpol_data_p->nr_of_plmn; i++) {
                plmn = &(get_cpol_data_p->ste_sim_plmn_with_AcT_p)[i];

                /* Only select entries that are actually used */
                if (is_plmn_valid(&(plmn->PLMN))) {

                    /* Create empty object to add */
                    exe_sim_cpol_info_response_t *new_item_p = NULL;
                    new_item_p = (exe_sim_cpol_info_response_t *) calloc(1,
                                 sizeof(exe_sim_cpol_info_response_t));

                    if (NULL == new_item_p) {
                        ATC_LOG_E("request_get_cpol_data: new_item_p is NULL. Failed to allocate memory!!!");
                        goto error;
                    }

                    new_item_p->cpol_info.index = i;
                    new_item_p->cpol_info.format = CPOL_FORMAT_NUMERIC;

                    if (EXE_STATE_SIM_PLMN_RETRY == record_p->state) {
                        new_item_p->valid_act = false;
                    } else {
                        new_item_p->valid_act = true;
                    }

                    /* Convert to proper format. See definition of ste_uicc_sim_file_read_plmn for detailed description of parameter formats. */
                    new_item_p->cpol_info.oper_string[0] = 0x30 + plmn->PLMN.MCC1;
                    new_item_p->cpol_info.oper_string[1] = 0x30 + plmn->PLMN.MCC2;
                    new_item_p->cpol_info.oper_string[2] = 0x30 + plmn->PLMN.MCC3;
                    new_item_p->cpol_info.oper_string[3] = 0x30 + plmn->PLMN.MNC1;
                    new_item_p->cpol_info.oper_string[4] = 0x30 + plmn->PLMN.MNC2;

                    if (9 >= plmn->PLMN.MNC3) {
                        new_item_p->cpol_info.oper_string[5] = 0x30 + plmn->PLMN.MNC3;
                    }

                    new_item_p->cpol_info.gsm_act = (plmn->AcT & SIM_ACT_GSM_BITMASK)
                                                    == SIM_ACT_GSM_BITMASK;
                    new_item_p->cpol_info.gsm_compact_act = (plmn->AcT
                                                            & SIM_ACT_GSM_COMPACT_BITMASK) == SIM_ACT_GSM_COMPACT_BITMASK;
                    new_item_p->cpol_info.utran_act = (plmn->AcT & SIM_ACT_UTRAN_BITMASK)
                                                      == SIM_ACT_UTRAN_BITMASK;
                    new_item_p->cpol_info.oper_string[EXE_OPERATOR_NUMERIC_STRING_MAX_LENGTH] = '\0';

                    /* Add item to the end of the list with return values */
                    if (NULL == response_p) {
                        /* First object to add => set the record_p->response */
                        response_p = new_item_p;
                        last_response_p = new_item_p;
                    } else {
                        /* After first object is added -> add the record to the last one (linked list) */
                        last_response_p->next_p = new_item_p;
                        last_response_p = new_item_p;
                    }
                }
            }
        } else {
            cmee_error_code = CMEE_OPERATION_NOT_ALLOWED;
            goto error;
        }

        exe_request_complete(record_p, EXE_SUCCESS, response_p);
        release_cpol_response_list(response_p);

        return EXE_SUCCESS;
        break;
    }

    default:
        ATC_LOG_E("%s: unknown state", __FUNCTION__);
        break;
    }

error:

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, &cmee_error_code);
    }

    release_cpol_response_list(response_p);

    return EXE_FAILURE;
}

/* Get the size of the PLMN file, used by the +CPOL AT command. */
exe_request_result_t request_cpol_data_test(exe_request_record_t *record_p)
{
    uicc_request_status_t result = UICC_REQUEST_STATUS_OK;
    exe_cmee_error_t cmee_error_code = CMEE_UNKNOWN;
    sim_client_t *simclient_p = NULL;
    ste_sim_t *uicc_p = NULL;

    switch (record_p->state) {
    case EXE_STATE_REQUEST: {
        ATC_LOG_I("request_get_cpol_data -> request");
        /* Get sim interface and check validity */
        simclient_p = exe_get_sim_client(record_p->exe_p);

        if (NULL == simclient_p) {
            ATC_LOG_E("request_get_sim_state, could not get sim client.");
            goto error;
        }

        uicc_p = sim_client_get_handle(simclient_p);

        if (NULL == uicc_p) {
            ATC_LOG_E("request_get_sim_state, could not get sim handle.");
            goto error;
        }

        /* Make request to sim to get all the CPOL data */
        result = ste_uicc_sim_file_read_plmn(uicc_p, (uintptr_t) request_record_get_client_tag(record_p), SIM_FILE_ID_PLMN_WACT);

        if (UICC_REQUEST_STATUS_OK != result) {
            ATC_LOG_E("request_get_sim_state, ste_uicc_sim_get_state failed, error code %d.", result);
            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    }

    case EXE_STATE_RESPONSE:
    case EXE_STATE_SIM_PLMN_RETRY: {

        /* Response from SIM */
        ATC_LOG_I("request_get_cpol_data <- response");

        if (record_p->client_result_code) {
            int entries = 0;
            ste_sim_plmn_with_AcT_t *plmn = NULL;

            /* Get all data and check result of call */
            ste_uicc_sim_file_read_plmn_response_t *get_cpol_data_p = (ste_uicc_sim_file_read_plmn_response_t *)(record_p->response_data_p);

            if (NULL == get_cpol_data_p) {
                ATC_LOG_E("request_get_cpol_data: get_cpol_data_p = NULL");
                goto error;
            }

            if (STE_UICC_STATUS_CODE_FAIL == get_cpol_data_p->uicc_status_code &&
                    STE_UICC_STATUS_CODE_FAIL_DETAILS_FILE_NOT_FOUND == get_cpol_data_p->uicc_status_code_fail_details &&
                    EXE_STATE_SIM_PLMN_RETRY != record_p->state) {
                // retry using EFplmnsel
                record_p->state = EXE_STATE_SIM_PLMN_RETRY;
                simclient_p = exe_get_sim_client(record_p->exe_p);
                uicc_p = sim_client_get_handle(simclient_p);
                result = ste_uicc_sim_file_read_plmn(uicc_p,
                                                     (uintptr_t) request_record_get_client_tag(record_p), SIM_FILE_ID_PLMN_SEL);

                if (UICC_REQUEST_STATUS_OK != result) {
                    ATC_LOG_E("request_get_sim_state, ste_uicc_sim_get_state failed, error code %d.", result);
                    goto error;
                }

                return EXE_PENDING;
            }

            cmee_error_code = convert_sim_status_codes_to_cmee_error(get_cpol_data_p->uicc_status_code,
                              get_cpol_data_p->uicc_status_code_fail_details,
                              get_cpol_data_p->status_word);

            if (cmee_error_code != CMEE_OK) {
                ATC_LOG_E("%s: Status=%d, Status details= %d", __FUNCTION__, get_cpol_data_p->uicc_status_code, get_cpol_data_p->uicc_status_code_fail_details);
                goto error;
            }

            entries = get_cpol_data_p->nr_of_plmn;
            exe_request_complete(record_p, EXE_SUCCESS, &entries);
            return EXE_SUCCESS;

        } else {
            cmee_error_code = CMEE_OPERATION_NOT_ALLOWED;
            goto error;
        }
    }

    default:
        ATC_LOG_E("%s: unknown state: %d", __FUNCTION__, record_p->state);
        break;
    }

error:

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, &cmee_error_code);
    }

    return EXE_FAILURE;
}

/*
 * Get the current sim state.
 */
exe_request_result_t request_get_sim_state(exe_request_record_t *record_p)
{
    uicc_request_status_t result = UICC_REQUEST_STATUS_OK;

    if (EXE_STATE_REQUEST == record_p->state) {
        ATC_LOG_I("request_get_sim_state -> request");
        sim_client_t *simclient_p = exe_get_sim_client(record_p->exe_p);

        if (NULL == simclient_p) {
            ATC_LOG_E("request_get_sim_state, could not get sim client.");
            goto error;
        }

        ste_sim_t *uicc_p = sim_client_get_handle(simclient_p);

        if (NULL == uicc_p) {
            ATC_LOG_E("request_get_sim_state, could not get sim handle.");
            goto error;
        }

        result = ste_uicc_sim_get_state(uicc_p, (uintptr_t)request_record_get_client_tag(record_p));

        if (UICC_REQUEST_STATUS_OK != result) {
            ATC_LOG_E("request_get_sim_state, ste_uicc_sim_get_state failed, error code %d.", result);
            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    } else {
        /* Response from SIM */
        ATC_LOG_I("request_get_sim_state <- response");
        exe_esimsr_sim_state_t get_state;
        get_state.sim_state = 0;

        if (record_p->client_result_code) {
            ste_uicc_get_sim_state_response_t *get_state_data_p = NULL;
            get_state_data_p = (ste_uicc_get_sim_state_response_t *)(record_p->response_data_p);

            if (NULL == get_state_data_p) {
                ATC_LOG_E("request_get_sim_state: get_state_data = NULL");
                goto error;
            }

            if (get_state_data_p->error_cause != EXE_SIM_NORMAL_COMPLETION) {
                ATC_LOG_E("%s: error_cause = %d", __FUNCTION__, get_state_data_p->error_cause);
                goto error;
            }

            get_state.sim_state = exe_convert_sim_state_to_esimsr_state(get_state_data_p->state);
        }

        exe_request_complete(record_p, EXE_SUCCESS, &get_state);
        return EXE_SUCCESS;
    }

error:

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, NULL);
    }

    return EXE_FAILURE;
}

/*
 * Get the current SIM pin info.
 */
exe_request_result_t request_get_pin_info(exe_request_record_t *record_p)
{
    uicc_request_status_t result = UICC_REQUEST_STATUS_OK;
    exe_sim_pin_info_t *sim_pin_info_p = NULL;

    if (EXE_STATE_REQUEST == record_p->state) {
        ATC_LOG_I("request_get_pin_info -> request");
        sim_client_t *simclient_p = exe_get_sim_client(record_p->exe_p);
        sim_pin_puk_id_t sim_pin_puk = SIM_PIN_ID_PIN1;
        sim_pin_info_p = (exe_sim_pin_info_t *) record_p->request_data_p;

        if (NULL == sim_pin_info_p) {
            ATC_LOG_E("request_get_pin_info: sim_pin_info_p is null");
            return EXE_FAILURE;
        }

        if (NULL == simclient_p) {
            ATC_LOG_E("request_get_pin_info, could not get sim client.");
            goto error;
        }

        ste_sim_t *uicc_p = sim_client_get_handle(simclient_p);

        if (NULL == uicc_p) {
            ATC_LOG_E("request_get_pin_info, could not get sim handle.");
            goto error;
        }

        /* Convert pin puk type to sim type sim_pinpuk_t */
        switch (sim_pin_info_p->pin_puk) {
        case EXE_PIN_1:
            sim_pin_puk = SIM_PIN_PIN1;
            break;

        case EXE_PIN_2:
            sim_pin_puk = SIM_PIN_PIN2;
            break;

        case EXE_PUK_1:
            sim_pin_puk = SIM_PIN_PUK1;
            break;

        case EXE_PUK_2:
            sim_pin_puk = SIM_PIN_PUK2;
            break;

        default:
            break;
        }

        result = ste_uicc_pin_info(uicc_p,
                                   (uintptr_t)request_record_get_client_tag(record_p),
                                   sim_pin_puk);

        if (UICC_REQUEST_STATUS_OK != result) {
            ATC_LOG_E("request_get_pin_info, ste_uicc_pin_info failed, error code %d.", result);
            goto error;
        }

        record_p->client_data_p = malloc(sizeof(exe_sim_pin_puk_t));

        if (NULL == record_p->client_data_p) {
            ATC_LOG_E("request_get_pin_info: could not allocate memory");
            goto error;
        }

        /* Need to save what pin/puk that was requested to use when we get the response */
        *((exe_sim_pin_puk_t *) record_p->client_data_p) = sim_pin_info_p->pin_puk;

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    } else {
        ATC_LOG_I("request_get_pin_info <- response");
        exe_sim_pin_info_t get_info;
        ste_uicc_status_word_t status_word = {0, 0};
        get_info.pin_puk = *((exe_sim_pin_puk_t *)record_p->client_data_p);  /* Indicate what pin/puk was requested.
                                                                              * This will give upper layers the
                                                                              * possibility to avoid saving states.
                                                                              */
        free(record_p->client_data_p);                                       /* Free this data directly as it is
                                                                              * saved in the return structure.
                                                                              */

        if (record_p->client_result_code) {
            ste_uicc_pin_info_response_t *get_pin_info = NULL;

            get_pin_info = (ste_uicc_pin_info_response_t *)(record_p->response_data_p);

            if (NULL == get_pin_info) {
                ATC_LOG_E("request_get_pin_info: get_pin_info = NULL");
                goto error;
            }

            get_info.error_code = convert_sim_status_codes_to_cmee_error(get_pin_info->uicc_status_code,
                                  get_pin_info->uicc_status_code_fail_details,
                                  status_word);

            get_info.attempts = get_pin_info->attempts_remaining;
        } else {
            get_info.error_code = CMEE_OPERATION_NOT_ALLOWED;
            get_info.attempts = 0;
        }

        exe_request_complete(record_p, EXE_SUCCESS, &get_info);
        return EXE_SUCCESS;
    }

error:

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, NULL);
    }

    return EXE_FAILURE;
}

/*
 * Get the full SIM pin info.
 */
exe_request_result_t request_get_full_pin_info(exe_request_record_t *record_p)
{
    uicc_request_status_t result = UICC_REQUEST_STATUS_OK;
    sim_client_t *simclient_p = exe_get_sim_client(record_p->exe_p);
    ste_sim_t *uicc_p = NULL;
    ste_uicc_pin_info_response_t *get_pin_info_p = NULL;
    ste_uicc_status_word_t status_word = {0, 0};
    exe_sim_full_pin_info_t *full_pin_p = NULL;

    if (NULL == simclient_p) {
        ATC_LOG_E("request_get_full_pin_info, could not get sim client.");
        goto error;
    }

    uicc_p = sim_client_get_handle(simclient_p);

    if (NULL == uicc_p) {
        ATC_LOG_E("request_get_full_pin_info, could not get sim handle.");
        goto error;
    }

    if (EXE_STATE_REQUEST == record_p->state) {
        ATC_LOG_I("request_get_full_pin_info -> PIN1 request");

        result = ste_uicc_pin_info(uicc_p,
                                   (uintptr_t)request_record_get_client_tag(record_p),
                                   SIM_PIN_PIN1);

        if (UICC_REQUEST_STATUS_OK != result) {
            ATC_LOG_E("request_get_full_pin_info, ste_uicc_pin_info failed, error code %d.", result);
            goto error;
        }

        /* Need to use record_p->client_data_p to save values get for each pin info */
        record_p->client_data_p = calloc(1, sizeof(exe_sim_full_pin_info_t));

        if (NULL == record_p->client_data_p) {
            ATC_LOG_E("request_get_full_pin_info: could not allocate memory");
            goto error;
        }

        record_p->state = EXE_STATE_FULL_PIN_INFO_PIN1;
        return EXE_PENDING;
    } else if (EXE_STATE_FULL_PIN_INFO_PIN1 == record_p->state) {
        ATC_LOG_I("request_get_full_pin_info <- PIN1 response");

        if (NULL == record_p->client_data_p) {
            ATC_LOG_E("request_get_full_pin_info, error input parameter.");
            goto error;
        }

        full_pin_p = (exe_sim_full_pin_info_t *)record_p->client_data_p;

        /* Decode PIN1 info*/
        if (record_p->client_result_code) {
            get_pin_info_p = (ste_uicc_pin_info_response_t *)(record_p->response_data_p);

            if (NULL == get_pin_info_p) {
                ATC_LOG_E("request_get_full_pin_info: get_pin_info_p = NULL");
                goto error;
            }

            full_pin_p->error_code = convert_sim_status_codes_to_cmee_error(get_pin_info_p->uicc_status_code,
                                     get_pin_info_p->uicc_status_code_fail_details,
                                     status_word);

            full_pin_p->pin1_attempts = get_pin_info_p->attempts_remaining;
        } else {
            full_pin_p->error_code = CMEE_OPERATION_NOT_ALLOWED;
            full_pin_p->pin1_attempts = 0;
        }

        /* Read PIN2 info*/
        ATC_LOG_I("request_get_full_pin_info -> PIN2 request");
        result = ste_uicc_pin_info(uicc_p,
                                   (uintptr_t)request_record_get_client_tag(record_p),
                                   SIM_PIN_PIN2);

        if (UICC_REQUEST_STATUS_OK != result) {
            ATC_LOG_E("request_get_full_pin_info, ste_uicc_pin_info failed, error code %d.", result);
            goto error;
        }

        record_p->state = EXE_STATE_FULL_PIN_INFO_PIN2;
        return EXE_PENDING;
    } else if (EXE_STATE_FULL_PIN_INFO_PIN2 == record_p->state) {
        ATC_LOG_I("request_get_full_pin_info <- PIN2 response");

        if (NULL == record_p->client_data_p) {
            ATC_LOG_E("request_get_full_pin_info, error input parameter.");
            goto error;
        }

        full_pin_p = (exe_sim_full_pin_info_t *)record_p->client_data_p;

        /* Decode PIN2 info*/
        if (record_p->client_result_code) {
            get_pin_info_p = (ste_uicc_pin_info_response_t *)(record_p->response_data_p);

            if (NULL == get_pin_info_p) {
                ATC_LOG_E("request_get_full_pin_info: get_pin_info_p = NULL");
                goto error;
            }

            full_pin_p->error_code = convert_sim_status_codes_to_cmee_error(get_pin_info_p->uicc_status_code,
                                     get_pin_info_p->uicc_status_code_fail_details,
                                     status_word);

            full_pin_p->pin2_attempts = get_pin_info_p->attempts_remaining;
        } else {
            full_pin_p->error_code = CMEE_OPERATION_NOT_ALLOWED;
            full_pin_p->pin2_attempts = 0;
        }

        /* Read PUK1 info*/
        ATC_LOG_I("request_get_full_pin_info -> PUK1 request");
        result = ste_uicc_pin_info(uicc_p,
                                   (uintptr_t)request_record_get_client_tag(record_p),
                                   SIM_PIN_PUK1);

        if (UICC_REQUEST_STATUS_OK != result) {
            ATC_LOG_E("request_get_full_pin_info, ste_uicc_pin_info failed, error code %d.", result);
            goto error;
        }

        record_p->state = EXE_STATE_FULL_PIN_INFO_PUK1;
        return EXE_PENDING;
    } else if (EXE_STATE_FULL_PIN_INFO_PUK1 == record_p->state) {
        ATC_LOG_I("request_get_full_pin_info <- PUK1 response");

        if (NULL == record_p->client_data_p) {
            ATC_LOG_E("request_get_full_pin_info, error input parameter.");
            goto error;
        }

        full_pin_p = (exe_sim_full_pin_info_t *)record_p->client_data_p;

        /* Decode PUK1 info*/
        if (record_p->client_result_code) {
            get_pin_info_p = (ste_uicc_pin_info_response_t *)(record_p->response_data_p);

            if (NULL == get_pin_info_p) {
                ATC_LOG_E("request_get_full_pin_info: get_pin_info_p = NULL");
                goto error;
            }

            full_pin_p->error_code = convert_sim_status_codes_to_cmee_error(get_pin_info_p->uicc_status_code,
                                     get_pin_info_p->uicc_status_code_fail_details,
                                     status_word);

            full_pin_p->puk1_attempts = get_pin_info_p->attempts_remaining;
        } else {
            full_pin_p->error_code = CMEE_OPERATION_NOT_ALLOWED;
            full_pin_p->puk1_attempts = 0;
        }

        /* Read PUK2 info*/
        ATC_LOG_I("request_get_full_pin_info -> PUK2 request");
        result = ste_uicc_pin_info(uicc_p,
                                   (uintptr_t)request_record_get_client_tag(record_p),
                                   SIM_PIN_PUK2);

        if (UICC_REQUEST_STATUS_OK != result) {
            ATC_LOG_E("request_get_full_pin_info, ste_uicc_pin_info failed, error code %d.", result);
            goto error;
        }

        record_p->state = EXE_STATE_FULL_PIN_INFO_PUK2;
        return EXE_PENDING;
    } else if (EXE_STATE_FULL_PIN_INFO_PUK2 == record_p->state) {
        exe_sim_full_pin_info_t response_pin_info;

        ATC_LOG_I("request_get_full_pin_info <- PUK2 response");

        if (NULL == record_p->client_data_p) {
            ATC_LOG_E("request_get_full_pin_info, error input parameter.");
            goto error;
        }

        full_pin_p = (exe_sim_full_pin_info_t *)record_p->client_data_p;

        /* Decode PUK2 info*/
        if (record_p->client_result_code) {
            get_pin_info_p = (ste_uicc_pin_info_response_t *)(record_p->response_data_p);

            if (NULL == get_pin_info_p) {
                ATC_LOG_E("request_get_full_pin_info: get_pin_info_p = NULL");
                goto error;
            }

            full_pin_p->error_code = convert_sim_status_codes_to_cmee_error(get_pin_info_p->uicc_status_code,
                                     get_pin_info_p->uicc_status_code_fail_details,
                                     status_word);

            full_pin_p->puk2_attempts = get_pin_info_p->attempts_remaining;
        } else {
            full_pin_p->error_code = CMEE_OPERATION_NOT_ALLOWED;
            full_pin_p->puk2_attempts = 0;
        }

        /* ALL info has been got*/
        /* copy full pin info from client data to response data structure */
        memcpy(&response_pin_info, full_pin_p, sizeof(response_pin_info));
        free(record_p->client_data_p);
        record_p->client_data_p = NULL;
        exe_request_complete(record_p, EXE_SUCCESS, &response_pin_info);

        return EXE_SUCCESS;
    }

error:

    free(record_p->client_data_p);
    record_p->client_data_p = NULL;

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, NULL);
    }

    return EXE_FAILURE;
}

/*
 * Get the IMSI code from SIM.
 */
exe_request_result_t request_get_imsi(exe_request_record_t *record_p)
{
    exe_imsi_t *sim_imsi_p = NULL;


    sim_imsi_p = (exe_imsi_t *)record_p->request_data_p;

    if (EXE_STATE_REQUEST == record_p->state) {
        ATC_LOG_I("request_get_imsi -> request");
        sim_client_t *sim_client_p = exe_get_sim_client(record_p->exe_p);
        uicc_request_status_t uicc_request_status = UICC_REQUEST_STATUS_OK;

        if (NULL == sim_client_p) {
            ATC_LOG_E("request_get_imsi: sim_client_p is null");
            goto error;
        }

        uicc_request_status = ste_uicc_sim_file_read_imsi(sim_client_get_handle(sim_client_p),
                              (uintptr_t)request_record_get_client_tag(record_p));

        if (UICC_REQUEST_STATUS_OK != uicc_request_status) {
            ATC_LOG_E("request_get_imsi: ste_uicc_sim_file_read_imsi failed ");
            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    } else {
        ATC_LOG_I("request_get_imsi <- response");
        exe_imsi_t imsi_buf;

        if (record_p->client_result_code) {
            ste_uicc_sim_file_read_imsi_response_t *imsi_p = NULL;

            imsi_p = (ste_uicc_sim_file_read_imsi_response_t *)(record_p->response_data_p);

            if (NULL == imsi_p) {
                ATC_LOG_E("request_get_imsi: wrong responce data from ste_uicc_sim_file_read_imsi");
                goto error;
            }

            imsi_buf.error_code = convert_sim_status_codes_to_cmee_error(imsi_p->uicc_status_code,
                                  imsi_p->uicc_status_code_fail_details,
                                  imsi_p->status_word);

            imsi_buf.imsi_p = imsi_p->imsi;

        } else {
            imsi_buf.error_code = CMEE_OPERATION_NOT_ALLOWED;
            imsi_buf.imsi_p = NULL;
        }

        exe_request_complete(record_p, EXE_SUCCESS, &imsi_buf);
        return EXE_SUCCESS;
    }

error:

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, NULL);
    }

    return EXE_FAILURE;
}

/*
 * Verify the given PIN code.
 */
exe_request_result_t request_verify_sim_pin(exe_request_record_t *record_p)
{
    int result = 0;

    if (EXE_STATE_REQUEST == record_p->state) {
        sim_client_t *sim_client_p = exe_get_sim_client(record_p->exe_p);
        ste_sim_t *ste_sim_p = NULL;
        ATC_LOG_I("request_verify_sim_pin -> request");

        if (NULL == record_p->request_data_p ||
                NULL == ((exe_sim_verify_codes_t *)record_p->request_data_p)->code_p ||
                NULL == sim_client_p) {
            ATC_LOG_E("request_verify_sim_pin, incorrect in data");
            goto error;
        }

        ste_sim_p = sim_client_get_handle(sim_client_p);

        if (NULL == ste_sim_p) {
            goto error;
        }

        /* The parser checks the state so no need to verify that it is correct here
         * by asking the sim.
         */
        result = ste_uicc_pin_verify(ste_sim_p,
                                     (uintptr_t)request_record_get_client_tag(record_p),
                                     ((exe_sim_verify_codes_t *)record_p->request_data_p)->pin_id,
                                     ((exe_sim_verify_codes_t *)record_p->request_data_p)->code_p);

        if (UICC_REQUEST_STATUS_OK != result) {
            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    } else {
        ATC_LOG_I("request_verify_sim_pin <- response");
        exe_cmee_error_t error_code;

        if (record_p->client_result_code) {
            ste_uicc_pin_verify_response_t *status_p = NULL;
            status_p = (ste_uicc_pin_verify_response_t *)(record_p->response_data_p);

            if (NULL == status_p) {
                ATC_LOG_E("request_verify_sim_pin: status = NULL");
                goto error;
            }

            /* Note that we return exe_success even is the pin verification failed in the
             * sim module, this is stated by the error code parameter so the parser will check
             * that and indicate the failure to above layers.
             */
            error_code = convert_sim_status_codes_to_cmee_error(status_p->uicc_status_code,
                         status_p->uicc_status_code_fail_details,
                         status_p->status_word);
        } else {
            error_code = CMEE_OPERATION_NOT_ALLOWED;
        }

        exe_request_complete(record_p, EXE_SUCCESS, &error_code);
        return EXE_SUCCESS;
    }

error:

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, NULL);
    }

    return EXE_FAILURE;
}

/*
 * Verify the given PUK code and set the new PIN code.
 */
exe_request_result_t request_verify_sim_puk(exe_request_record_t *record_p)
{
    int result = 0;

    if (EXE_STATE_REQUEST == record_p->state) {
        sim_client_t *sim_client_p = exe_get_sim_client(record_p->exe_p);
        ste_sim_t *ste_sim_p = NULL;
        ATC_LOG_I("request_verify_sim_puk -> request");

        if (NULL == record_p->request_data_p ||
                NULL == ((exe_sim_verify_codes_t *)record_p->request_data_p)->code_p ||
                NULL == sim_client_p) {
            ATC_LOG_E("request_verify_sim_puk, incorrect in data");
            goto error;
        }

        ste_sim_p = sim_client_get_handle(sim_client_p);

        if (NULL == ste_sim_p) {
            goto error;
        }

        /* The parser checks the state so no need to verify that it is correct here
         * by asking the sim.
         */
        result = ste_uicc_pin_unblock(ste_sim_p,
                                      (uintptr_t)request_record_get_client_tag(record_p),
                                      ((exe_sim_verify_codes_t *)record_p->request_data_p)->pin_id,
                                      ((exe_sim_verify_codes_t *)record_p->request_data_p)->new_pin_p,
                                      ((exe_sim_verify_codes_t *)record_p->request_data_p)->code_p);

        if (UICC_REQUEST_STATUS_OK != result) {
            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    } else {
        ATC_LOG_I("request_verify_sim_puk <- response");
        exe_cmee_error_t error_code;

        if (record_p->client_result_code) {
            ste_uicc_pin_verify_response_t *status_p = NULL;
            status_p = (ste_uicc_pin_verify_response_t *)(record_p->response_data_p);

            if (NULL == status_p) {
                ATC_LOG_E("request_verify_sim_puk: status = NULL");
                goto error;
            }

            /* Note that we return exe_success even is the puk verification failed in the
             * sim module, this is stated by the error code parameter so the parser will check
             * that and indicate the failure to above layers.
             */
            error_code = convert_sim_status_codes_to_cmee_error(status_p->uicc_status_code,
                         status_p->uicc_status_code_fail_details,
                         status_p->status_word);
        } else {
            error_code = CMEE_OPERATION_NOT_ALLOWED;
        }

        exe_request_complete(record_p, EXE_SUCCESS, &error_code);
        return EXE_SUCCESS;
    }

error:

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, NULL);
    }

    return EXE_FAILURE;
}

/*
 * Verify the given PIN2 code.
 */
exe_request_result_t request_verify_sim_pin2(exe_request_record_t *record_p)
{
    int result = 0;

    if (EXE_STATE_REQUEST == record_p->state) {
        sim_client_t *sim_client_p = exe_get_sim_client(record_p->exe_p);
        ste_sim_t *ste_sim_p = NULL;
        ATC_LOG_I("request_verify_sim_pin2 -> request");

        if (NULL == record_p->request_data_p ||
                NULL == ((exe_sim_verify_codes_t *)record_p->request_data_p)->code_p ||
                NULL == sim_client_p) {
            ATC_LOG_E("request_verify_sim_pin2, incorrect in data");
            goto error;
        }

        ste_sim_p = sim_client_get_handle(sim_client_p);

        if (NULL == ste_sim_p) {
            goto error;
        }

        /* The parser checks the state so no need to verify that it is correct here
         * by asking the sim.
         */
        result = ste_uicc_pin_verify(ste_sim_p,
                                     (uintptr_t)request_record_get_client_tag(record_p),
                                     ((exe_sim_verify_codes_t *)record_p->request_data_p)->pin_id,
                                     ((exe_sim_verify_codes_t *)record_p->request_data_p)->code_p);

        if (UICC_REQUEST_STATUS_OK != result) {
            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    } else {
        ATC_LOG_I("request_verify_sim_pin2 <- response");
        exe_cmee_error_t error_code;

        if (record_p->client_result_code) {
            ste_uicc_pin_verify_response_t *status_p = NULL;
            status_p = (ste_uicc_pin_verify_response_t *)(record_p->response_data_p);

            if (NULL == status_p) {
                ATC_LOG_E("request_verify_sim_pin2: status = NULL");
                goto error;
            }

            /* Note that we return exe_success even is the pin2 verification failed in the
             * sim module, this is stated by the error code parameter so the parser will check
             * that and indicate the failure to above layers.
             */
            error_code = convert_sim_status_codes_to_cmee_error(status_p->uicc_status_code,
                         status_p->uicc_status_code_fail_details,
                         status_p->status_word);
        } else {
            error_code = CMEE_OPERATION_NOT_ALLOWED;
        }

        exe_request_complete(record_p, EXE_SUCCESS, &error_code);
        return EXE_SUCCESS;
    }

error:

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, NULL);
    }

    return EXE_FAILURE;
}

/*
 * Verify the given PUK2 code and set the new PIN2 code.
 */
exe_request_result_t request_verify_sim_puk2(exe_request_record_t *record_p)
{
    int result = 0;

    if (EXE_STATE_REQUEST == record_p->state) {
        sim_client_t *sim_client_p = exe_get_sim_client(record_p->exe_p);
        ste_sim_t *ste_sim_p = NULL;
        ATC_LOG_I("request_verify_sim_puk2 -> request");

        if (NULL == record_p->request_data_p ||
                NULL == ((exe_sim_verify_codes_t *)record_p->request_data_p)->code_p ||
                NULL == sim_client_p) {
            ATC_LOG_E("request_verify_sim_puk2, incorrect in data");
            goto error;
        }

        ste_sim_p = sim_client_get_handle(sim_client_p);

        if (NULL == ste_sim_p) {
            goto error;
        }

        /* The parser checks the state so no need to verify that it is correct here
         * by asking the sim.
         */
        result = ste_uicc_pin_unblock(ste_sim_p,
                                      (uintptr_t)request_record_get_client_tag(record_p),
                                      ((exe_sim_verify_codes_t *)record_p->request_data_p)->pin_id,
                                      ((exe_sim_verify_codes_t *)record_p->request_data_p)->new_pin_p,
                                      ((exe_sim_verify_codes_t *)record_p->request_data_p)->code_p);

        if (UICC_REQUEST_STATUS_OK != result) {
            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    } else {
        ATC_LOG_I("request_verify_sim_pin <- response");
        exe_cmee_error_t error_code;

        if (record_p->client_result_code) {
            ste_uicc_pin_verify_response_t *status_p = NULL;
            status_p = (ste_uicc_pin_verify_response_t *)(record_p->response_data_p);

            if (NULL == status_p) {
                ATC_LOG_E("request_verify_sim_pin: status = NULL");
                goto error;
            }

            /* Note that we return exe_success even is the pin verification failed in the
             * sim module, this is stated by the error code parameter so the parser will check
             * that and indicate the failure to above layers.
             */
            error_code = convert_sim_status_codes_to_cmee_error(status_p->uicc_status_code,
                         status_p->uicc_status_code_fail_details,
                         status_p->status_word);
        } else {
            error_code = CMEE_OPERATION_NOT_ALLOWED;
        }

        exe_request_complete(record_p, EXE_SUCCESS, &error_code);
        return EXE_SUCCESS;
    }

error:

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, NULL);
    }

    return EXE_FAILURE;
}


/*
 * Change the PIN or PIN2 code.
 */
exe_request_result_t request_sim_pin_change(exe_request_record_t *record_p)
{
    int result = 0;

    if (EXE_STATE_REQUEST == record_p->state) {
        sim_client_t *sim_client_p = exe_get_sim_client(record_p->exe_p);
        ste_sim_t *ste_sim_p = NULL;
        ATC_LOG_I("request_sim_pin_change -> request");

        if (NULL == record_p->request_data_p ||
                NULL == ((exe_change_pin_t *) record_p->request_data_p)->new_passwd_p ||
                NULL == ((exe_change_pin_t *) record_p->request_data_p)->old_passwd_p ||
                NULL == sim_client_p) {
            ATC_LOG_E("request_sim_pin_change, incorrect in data or no simd connection");
            goto error;
        }

        ste_sim_p = sim_client_get_handle(sim_client_p);

        if (NULL == ste_sim_p) {
            goto error;
        }

        /* The parser checks the state so no need to verify that it is correct here
         * by asking the sim.
         */
        result = ste_uicc_pin_change(ste_sim_p,
                                     (uintptr_t)request_record_get_client_tag(record_p),
                                     ((exe_change_pin_t *)record_p->request_data_p)->old_passwd_p,
                                     ((exe_change_pin_t *)record_p->request_data_p)->new_passwd_p,
                                     ((exe_change_pin_t *)record_p->request_data_p)->pin_id);

        if (UICC_REQUEST_STATUS_OK != result) {
            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    } else {
        ATC_LOG_I("request_sim_pin_change <- response");
        exe_change_pin_t change_pin_data;

        if (record_p->client_result_code) {
            ste_uicc_pin_change_response_t *status_p = NULL;
            status_p = (ste_uicc_pin_change_response_t *)(record_p->response_data_p);

            if (NULL == status_p) {
                ATC_LOG_E("request_sim_pin_change: status = NULL");
                goto error;
            }

            /* Note that we return exe_success even if is the pin verification failed in the
             * sim module, this is stated in the error code parameter so the parser will check
             * that and indicate the failure to above layers.
             */
            change_pin_data.error_code = convert_sim_status_codes_to_cmee_error(status_p->uicc_status_code,
                                         status_p->uicc_status_code_fail_details,
                                         status_p->status_word);
        } else {
            change_pin_data.error_code = CMEE_OPERATION_NOT_ALLOWED;
        }

        exe_request_complete(record_p, EXE_SUCCESS, &change_pin_data);
        return EXE_SUCCESS;
    }

error:

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, NULL);
    }

    return EXE_FAILURE;
}

/*
 * Make the initial part of the full sim xml response.
 */
static void xml_header_and_state(char *full_sim, ste_uicc_card_state_t state)
{
    strcat(full_sim, "<?xml version=\"1.0\"?>\n<card>\n  <state>");

    switch (state) {
    case STE_UICC_CARD_STATE_UNKNOWN:
    case STE_UICC_CARD_STATE_MISSING:
    case STE_UICC_CARD_STATE_DISCONNECTED: {
        strcat(full_sim, "absent");
        break;
    }

    case STE_UICC_CARD_STATE_READY: {
        strcat(full_sim, "present");
        break;
    }

    case STE_UICC_CARD_STATE_NOT_READY:
    case STE_UICC_CARD_STATE_INVALID:
    default: {
        strcat(full_sim, "error");
        break;
    }
    }

    strcat(full_sim, "</state>");
}

/*
 * Add unified pin to xml response.
 */
static void xml_uni_pin(char *full_sim, ste_uicc_pin_state_t uni_pin)
{
    strcat(full_sim, "\n  <uni_pin_state>");

    switch (uni_pin) {
    case STE_UICC_PIN_STATE_ENABLED_VERIFIED: {
        strcat(full_sim, "enabl_verified");
        break;
    }

    case STE_UICC_PIN_STATE_ENABLED_NOT_VERIFIED: {
        strcat(full_sim, "enabl_not_verified");
        break;
    }

    case STE_UICC_PIN_STATE_DISABLED: {
        strcat(full_sim, "disabled");
        break;
    }

    case STE_UICC_PIN_STATE_ENABLED_BLOCKED: {
        strcat(full_sim, "enabl_blocked");
        break;
    }

    case STE_UICC_PIN_STATE_ENABLED_PERM_BLOCKED: {
        strcat(full_sim, "enabl_perm_blocked");
        break;
    }

    case STE_UICC_PIN_STATE_UNKNOWN:
    case STE_UICC_PIN_STATE_ENABLED:
    default: {
        strcat(full_sim, "unknown");
        break;
    }
    }

    strcat(full_sim, "</uni_pin_state>");
}

/*
 * Add the application id tag to sim xml response.
 */
static void xml_app_id(char *full_sim, uint8_t *app_id, size_t length)
{
    size_t i;
    char tmp_buffer[3]; /* Must have room for one converted byte plus string termination */

    if (length < 1) {
        return;
    }

    strcat(full_sim, "\n    <aid>");

    for (i = 0; i < length; i++) {
        tmp_buffer[0] = 0;
        sprintf((char *)tmp_buffer, "%02X", app_id[i]);
        strcat((char *)full_sim, tmp_buffer);
    }

    strcat(full_sim, "</aid>");
}

/*
 * Add the application label tag in full sim xml response.
 */
static void xml_app_label(char *full_sim, char *app_label, size_t length)
{
    if (length < 1) {
        return;
    }

    strcat(full_sim, "\n    <alabel>");
    strncat(full_sim, app_label, length);
    strcat(full_sim, "</alabel>");
}

/*
 * Add the application type part of the full sim xml response.
 */
static void xml_app_type(char *full_sim, sim_app_type_t app_type)
{
    strcat(full_sim, "\n  <app>\n    <type>");

    switch (app_type) {
    case SIM_APP_GSM: {
        strcat(full_sim, "sim");
        break;
    }

    case SIM_APP_USIM: {
        strcat(full_sim, "usim");
        break;
    }

    case SIM_APP_ISIM: {
        strcat(full_sim, "isim");
        break;
    }

    case SIM_APP_UNKNOWN:
    default: {
        strcat(full_sim, "unknown");
        break;
    }
    }

    strcat(full_sim, "</type>");
}

/*
 * Add the application state part of the full sim xml response.
 */
static void xml_app_state(char *full_sim, ste_uicc_app_state_t app_state)
{
    strcat(full_sim, "\n    <app_state>");

    switch (app_state) {
    case STE_UICC_APP_STATE_ACTIVE: {
        strcat(full_sim, "ready");
        break;
    }

    case STE_UICC_APP_STATE_ACTIVE_PIN: {
        strcat(full_sim, "pin");
        break;
    }

    case STE_UICC_APP_STATE_ACTIVE_PUK: {
        strcat(full_sim, "puk");
        break;
    }

    case STE_UICC_APP_STATE_NOT_ACTIVE: {
        strcat(full_sim, "detected");
        break;
    }

    case STE_UICC_APP_STATE_UNKNOWN:
    default: {
        strcat(full_sim, "unknown");
        break;
    }
    }

    strcat(full_sim, "</app_state>");
}

/*
 * Add the pin1 part of the full sim xml response.
 */
static void xml_pin1(char *full_sim, ste_uicc_pin_state_t pin_state)
{
    strcat(full_sim, "\n    <pin1>");

    switch (pin_state) {
    case STE_UICC_PIN_STATE_ENABLED_VERIFIED: {
        strcat(full_sim, "enabl_verified");
        break;
    }

    case STE_UICC_PIN_STATE_ENABLED_NOT_VERIFIED: {
        strcat(full_sim, "enabl_not_verified");
        break;
    }

    case STE_UICC_PIN_STATE_ENABLED_BLOCKED: {
        strcat(full_sim, "enabled_blocked");
        break;
    }

    case STE_UICC_PIN_STATE_ENABLED_PERM_BLOCKED: {
        strcat(full_sim, "enabl_perm_blocked");
        break;
    }

    case STE_UICC_PIN_STATE_DISABLED: {
        strcat(full_sim, "disabled");
        break;
    }

    case STE_UICC_PIN_STATE_UNKNOWN:
    case STE_UICC_PIN_STATE_ENABLED:
    default: {
        strcat(full_sim, "unknown");
        break;
    }
    }

    strcat(full_sim, "</pin1>");
}

/*
 * Add the pin2 part of the full sim xml response.
 */
static void xml_pin2(char *full_sim, ste_uicc_pin_state_t pin_state)
{
    strcat(full_sim, "\n    <pin2>");

    switch (pin_state) {
    case STE_UICC_PIN_STATE_ENABLED_VERIFIED: {
        strcat(full_sim, "enabl_verified");
        break;
    }

    case STE_UICC_PIN_STATE_ENABLED_NOT_VERIFIED: {
        strcat(full_sim, "enabl_not_verified");
        break;
    }

    case STE_UICC_PIN_STATE_ENABLED_BLOCKED: {
        strcat(full_sim, "enabled_blocked");
        break;
    }

    case STE_UICC_PIN_STATE_ENABLED_PERM_BLOCKED: {
        strcat(full_sim, "enabl_perm_blocked");
        break;
    }

    case STE_UICC_PIN_STATE_DISABLED: {
        strcat(full_sim, "disabled");
        break;
    }

    case STE_UICC_PIN_STATE_UNKNOWN:
    case STE_UICC_PIN_STATE_ENABLED:
    default: {
        strcat(full_sim, "unknown");
        break;
    }
    }

    strcat(full_sim, "</pin2>");
}

/*
 * Add the pin replace part of the full sim xml response.
 */
static void xml_pin_repl(char *full_sim, sim_app_pin_mode_t mode)
{
    strcat(full_sim, "\n    <pin1rep>");

    switch (mode) {
    case SIM_APP_PIN_MODE_NORMAL: {
        strcat(full_sim, "off");
        break;
    }

    case SIM_APP_PIN_MODE_REPLACED: {
        strcat(full_sim, "on");
        break;
    }

    default: {
        strcat(full_sim, "off");
        break;
    }
    }

    strcat(full_sim, "</pin1rep>");
}
/*
 * Add the mark part of the full sim xml response.
 */
static void xml_mark(char *full_sim, sim_app_mark_t mark)
{
    strcat(full_sim, "\n    <telekom>");

    switch (mark) {
    case SIM_APP_MARK_NONE: {
        strcat(full_sim, "unmarked");
        break;
    }

    case SIM_APP_MARK_GSM_UMTS: {
        strcat(full_sim, "gsm_umts");
        break;
    }

    case SIM_APP_MARK_CDMA: {
        strcat(full_sim, "cdma");
        break;
    }

    case SIM_APP_MARK_UNKNOWN:
    default: {
        strcat(full_sim, "unknown");
        break;
    }
    }

    strcat(full_sim, "</telekom>");
}

/*
 * Add the final app tag in full sim xml response.
 */
static void xml_app_end(char *full_sim)
{
    strcat(full_sim, "\n  </app>");
}

/*
 * Add the final card tag in full sim xml response.
 */
static void xml_card_end(char *full_sim)
{
    strcat(full_sim, "\n</card>");
}

/*
 * Get the full sim information.
 */
exe_request_result_t request_get_full_sim_info(exe_request_record_t *record_p)
{
    uicc_request_status_t result = UICC_REQUEST_STATUS_OK;
    sim_client_t *sim_client_p = exe_get_sim_client(record_p->exe_p);
    exe_full_sim_t full_sim;
    full_sim.error_code = 0;
    int *app_count_p = NULL;

    if (NULL == sim_client_p) {
        ATC_LOG_E("request_get_full_sim_info, could not get sim client.");
        goto error;
    }

    ste_sim_t *uicc_p = sim_client_get_handle(sim_client_p);

    if (NULL == uicc_p) {
        ATC_LOG_E("request_get_full_sim_info, could not get sim handle.");
        goto error;
    }

    if (EXE_STATE_REQUEST == record_p->state) {
        /*
         * Step one is to ask for general SIM status
         */
        ATC_LOG_I("request_get_full_sim_info -> request");
        full_sim_xml[0] = 0; /* Clere the xml buffer */

        result = ste_uicc_card_status(uicc_p, (uintptr_t)request_record_get_client_tag(record_p));

        if (UICC_REQUEST_STATUS_OK != result) {
            ATC_LOG_E("request_get_full_sim_info, ste_uicc_card_status failed, error code %d.", result);
            goto error;
        }

        record_p->state = EXE_STATE_SIM_FULL_STATE;
        return EXE_PENDING;

    } else if (EXE_STATE_SIM_FULL_STATE == record_p->state) {
        /* Response from SIM on first request */
        ATC_LOG_I("request_get_full_sim_info <- response");

        if (record_p->client_result_code) {
            ste_uicc_sim_card_status_response_t *card_status_data_p = NULL;
            card_status_data_p = (ste_uicc_sim_card_status_response_t *)(record_p->response_data_p);
            int local_app_count = 0;

            if (NULL == card_status_data_p) {
                ATC_LOG_E("request_get_full_sim_info: get_state_data = NULL");
                goto error;
            }

            if (card_status_data_p->status != EXE_SIM_NORMAL_COMPLETION) {
                ATC_LOG_E("request_get_full_sim_info: card_status_data_p->status not SIM_ISO_NORMAL_COMPLETION");
                goto error;
            }

            /* make xml answer for state*/
            xml_header_and_state((char *)full_sim_xml, card_status_data_p->card_state);
            xml_uni_pin((char *)full_sim_xml, card_status_data_p->upin_state);

            local_app_count = card_status_data_p->num_apps;

            if (local_app_count > 0) {
                /*
                 * Step two is to ask for information about the first application
                 */
                result = ste_uicc_app_status(uicc_p,
                                             (uintptr_t)request_record_get_client_tag(record_p),
                                             local_app_count - 1);

                if (UICC_REQUEST_STATUS_OK != result) {
                    ATC_LOG_E("request_get_full_sim_info, ste_uicc_app_status failed, error code %d.", result);
                    goto error;
                }

                /* Need to save mode for to use when we get the response */
                app_count_p = malloc(sizeof(int));

                if (NULL == app_count_p) {
                    ATC_LOG_E("request_get_full_sim_info: could not allocate memory");
                    goto error;
                }

                *app_count_p = local_app_count;
                record_p->client_data_p = app_count_p;

                record_p->state = EXE_STATE_SIM_FULL_REQUEST_APP;
                return EXE_PENDING;
            } else {  /* No apps on the card. end the xml responce and finish execution */
                full_sim.state_content_p = (char *)full_sim_xml;
                xml_card_end(full_sim.state_content_p);

                exe_request_complete(record_p, EXE_SUCCESS, &full_sim);
                return EXE_SUCCESS;
            }
        } else {
            full_sim.error_code = CMEE_OPERATION_NOT_ALLOWED;
            full_sim.state_content_p = NULL;
            exe_request_complete(record_p, EXE_SUCCESS, &full_sim);
            return EXE_SUCCESS;
        }
    } else if (EXE_STATE_SIM_FULL_REQUEST_APP == record_p->state) {
        /* Response from SIM on app request */
        ste_uicc_sim_app_status_response_t *app_status_data_p = NULL;
        int local_app_count = 0;
        app_count_p = (int *)record_p->client_data_p;
        local_app_count = *app_count_p;
        free(app_count_p);  /* Free this data directly as it is saved in mode*/

        app_status_data_p = (ste_uicc_sim_app_status_response_t *)(record_p->response_data_p);

        if (NULL == app_status_data_p) {
            ATC_LOG_E("request_get_full_sim_info: app_status_data = NULL");
            goto error;
        }

        if (app_status_data_p->status != EXE_SIM_NORMAL_COMPLETION) {
            ATC_LOG_E("request_get_full_sim_info: app_status_data_p->status not SIM_ISO_NORMAL_COMPLETION");
            goto error;
        }

        /* make xml answer */
        xml_app_type((char *)full_sim_xml, app_status_data_p->app_type);
        xml_app_state((char *)full_sim_xml, app_status_data_p->app_state);
        xml_app_id((char *)full_sim_xml, app_status_data_p->aid, app_status_data_p->aid_len);
        xml_app_label((char *)full_sim_xml, app_status_data_p->label, app_status_data_p->label_len);
        xml_pin_repl((char *)full_sim_xml, app_status_data_p->pin_mode);
        xml_pin1((char *)full_sim_xml, app_status_data_p->pin_state);
        xml_pin2((char *)full_sim_xml, app_status_data_p->pin2_state);
        xml_mark((char *)full_sim_xml, app_status_data_p->mark);

        local_app_count--;  /* Decrease remaining number of apps */

        if (local_app_count > 0) {
            /*
             * Step three is to ask for information about the other application
             */
            result = ste_uicc_app_status(uicc_p,
                                         (uintptr_t)request_record_get_client_tag(record_p),
                                         local_app_count - 1);

            if (UICC_REQUEST_STATUS_OK != result) {
                ATC_LOG_E("request_get_full_sim_info, ste_uicc_app_status failed, error code %d.", result);
                goto error;
            }

            /* Need to save mode for to use when we get the response */
            app_count_p = malloc(sizeof(int));

            if (NULL == app_count_p) {
                ATC_LOG_E("request_get_full_sim_info: could not allocate memory");
                goto error;
            }

            *app_count_p = local_app_count;
            record_p->client_data_p = app_count_p;

            /* End the previous app tag */
            xml_app_end((char *)full_sim_xml);

            record_p->state = EXE_STATE_SIM_FULL_REQUEST_APP;
            return EXE_PENDING;

        } else {  /*  No more apps. Terminate xml responce and exit */
            full_sim.state_content_p = (char *)full_sim_xml;
            xml_app_end(full_sim.state_content_p);
            xml_card_end(full_sim.state_content_p);

            exe_request_complete(record_p, EXE_SUCCESS, &full_sim);
            return EXE_SUCCESS;
        }
    }

error:

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, NULL);
    }

    return EXE_FAILURE;
}

/*
 * Free data used in request_get_sim_apps_info
 */
static void free_request_get_sim_apps_info_data(exe_cuad_response_t *data_p)
{
    int i = 0;

    for (i = 0; i < MAX_SIM_APPS; ++i) {
        if (data_p->apps[i] != NULL) {
            exe_cuad_app_response_t *app_data_p = data_p->apps[i];
            free(app_data_p->sim_io_response_p);
            free(app_data_p);
            data_p->apps[i] = NULL;
        }
    }

    free(data_p);
}

/*
 * Get the app info in raw format.
 */
exe_request_result_t request_get_sim_apps_info(exe_request_record_t *record_p)
{
    uicc_request_status_t result = UICC_REQUEST_STATUS_OK;
    sim_client_t *sim_client_p = exe_get_sim_client(record_p->exe_p);
    exe_cuad_response_t *response_apps_p = NULL;
    int *app_count_p = NULL;

    if (NULL == sim_client_p) {
        ATC_LOG_E("request_get_sim_apps_info, could not get sim client.");
        goto error;
    }

    ste_sim_t *uicc_p = sim_client_get_handle(sim_client_p);

    if (NULL == uicc_p) {
        ATC_LOG_E("request_get_sim_apps_info, could not get sim handle.");
        goto error;
    }

    switch (record_p->state) {

    case EXE_STATE_REQUEST: {
        /*
         * Step one is to ask for general SIM status
         */
        ATC_LOG_I("request_get_sim_apps_info -> request");

        result = ste_uicc_card_status(uicc_p, (uintptr_t) request_record_get_client_tag(record_p));

        if (UICC_REQUEST_STATUS_OK != result) {
            ATC_LOG_E("request_get_sim_apps_info, ste_uicc_card_status failed, error code %d.", result);
            goto error;
        }

        record_p->state = EXE_STATE_SIM_FULL_STATE;
        return EXE_PENDING;
        break;
    }

    case EXE_STATE_SIM_FULL_STATE: {
        /* Response from SIM on first request */
        ATC_LOG_I("request_get_sim_apps_info <- response number of apps");

        if (record_p->client_result_code) {
            ste_uicc_sim_card_status_response_t *card_status_data_p = NULL;
            card_status_data_p
                = (ste_uicc_sim_card_status_response_t *)(record_p->response_data_p);
            int local_app_count = 0;

            if (NULL == card_status_data_p) {
                ATC_LOG_E("request_get_sim_apps_info: get_state_data = NULL");
                goto error;
            }

            if (card_status_data_p->status != EXE_SIM_NORMAL_COMPLETION) {
                ATC_LOG_E("request_get_sim_apps_info: card_status_data_p->status %d", card_status_data_p->status);
                goto error;
            }

            local_app_count = card_status_data_p->num_apps;

            if ((local_app_count > 0) && (card_status_data_p->card_type == SIM_CARD_TYPE_UICC)) {
                /*
                 * Step two is to ask for information about the first application
                 */
                int i = 0;

                result = ste_uicc_sim_file_read_record(uicc_p,
                                                       (uintptr_t) request_record_get_client_tag(record_p), sim_ef_dir_file_id, 1,
                                                       0, /* 0 means read until end of record */
                                                       NULL);

                if (UICC_REQUEST_STATUS_OK != result) {
                    ATC_LOG_E("request_get_sim_apps_info, ste_uicc_app_status failed, error code %d.", result);
                    goto error;
                }

                /* Need to save data for to use when we get the response */
                response_apps_p = calloc(1, sizeof(exe_cuad_response_t));

                if (NULL == response_apps_p) {
                    ATC_LOG_E("request_get_sim_apps_info: could not allocate memory");
                    goto error;
                }

                response_apps_p->number_of_apps = local_app_count;
                response_apps_p->count = 1;

                for (i = 0; i < MAX_SIM_APPS; ++i) {
                    response_apps_p->apps[i] = NULL;
                }

                record_p->client_data_p = response_apps_p;

                record_p->state = EXE_STATE_SIM_FULL_REQUEST_APP_RAW;
                return EXE_PENDING;
            } else { /* No apps on the card. Finish execution */
                exe_cuad_response_t response_apps;

                response_apps.error = CMEE_OK;
                response_apps.number_of_apps = 0;

                exe_request_complete(record_p, EXE_SUCCESS, &response_apps);
                return EXE_SUCCESS;
            }
        } else {
            exe_cuad_response_t response_apps;

            response_apps.error = CMEE_OPERATION_NOT_ALLOWED;
            response_apps.number_of_apps = 0;
            exe_request_complete(record_p, EXE_SUCCESS, &response_apps);
            return EXE_SUCCESS;
        }

        break;
    }

    case EXE_STATE_SIM_FULL_REQUEST_APP_RAW: {

        /* Response from SIM on app request */
        ATC_LOG_I("request_get_sim_apps_info <- response one app");
        ste_uicc_sim_file_read_record_response_t *record_data_p = NULL;
        response_apps_p = (exe_cuad_response_t *) record_p->client_data_p;

        if (!response_apps_p) {
            ATC_LOG_E("request_get_sim_apps_info: response_apps_p is NULL");
            goto error;
        }

        record_data_p = (ste_uicc_sim_file_read_record_response_t *)(record_p->response_data_p);

        if (NULL == record_data_p) {
            ATC_LOG_E("request_get_sim_apps_info: record_data_p is NULL");
            /* Need to free data */

            goto error;
        }

        response_apps_p->error = convert_sim_status_codes_to_cmee_error(
                                     record_data_p->uicc_status_code, record_data_p->uicc_status_code_fail_details,
                                     record_data_p->status_word);

        if (response_apps_p->error == CMEE_OK) {
            exe_cuad_app_response_t *app_data_p = NULL;
            char *data_p = NULL;

            /* Add the data for the app in the response */
            app_data_p = calloc(1, sizeof(exe_cuad_app_response_t));

            if (NULL == app_data_p) {
                ATC_LOG_E("request_get_sim_apps_info: could not allocate memory");
                goto error;
            }

            response_apps_p->apps[response_apps_p->count - 1] = app_data_p;
            app_data_p->length = record_data_p->length;
            data_p = malloc(app_data_p->length);

            if (NULL == data_p) {
                ATC_LOG_E("request_get_sim_apps_info: could not allocate memory");
                goto error;
            }

            memmove(data_p, record_data_p->data, record_data_p->length);
            app_data_p->sim_io_response_p = data_p;

            response_apps_p->count++; /* Increase app count */

            if (response_apps_p->count <= response_apps_p->number_of_apps) {
                /*
                 * Step three is to ask for information about the other application
                 */
                result = ste_uicc_sim_file_read_record(uicc_p,
                                                       (uintptr_t) request_record_get_client_tag(record_p), sim_ef_dir_file_id,
                                                       response_apps_p->count, 0, /* 0 means read until end of record */
                                                       NULL);

                if (UICC_REQUEST_STATUS_OK != result) {
                    ATC_LOG_E("request_get_sim_apps_info, ste_uicc_app_status failed, error code %d.", result);
                    goto error;
                }

                record_p->state = EXE_STATE_SIM_FULL_REQUEST_APP_RAW;
                return EXE_PENDING;

            } else { /*  No more apps. Terminate  */

                exe_request_complete(record_p, EXE_SUCCESS, response_apps_p);
                /* Free data */
                free_request_get_sim_apps_info_data(response_apps_p);
                return EXE_SUCCESS;
            }
        }

        break;
    }

    default:
        goto error;
        break;
    }

error:

    if (response_apps_p) {
        free_request_get_sim_apps_info_data(response_apps_p);
    }

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, NULL);
    }

    return EXE_FAILURE;
}

exe_request_result_t request_sim_set_smsc(exe_request_record_t *record_p)
{
    if (EXE_STATE_REQUEST == record_p->state) {
        sim_client_t *sim_client_p = NULL;
        ste_sim_call_number_t smsc;

        ATC_LOG_I("request_sim_set_smsc -> request");

        if (NULL == record_p->request_data_p) {
            ATC_LOG_E("request_sim_set_smsc: request data is NULL");
            goto error;
        }

        sim_client_p = exe_get_sim_client(record_p->exe_p);

        if (NULL == sim_client_p) {
            ATC_LOG_E("request_sim_set_smsc: sim client is NULL");
            goto error;
        }

        smsc.num_text.text_p = ((exe_csca_request_data_t *) record_p->request_data_p)->text_p;
        smsc.num_text.text_coding = STE_SIM_ASCII8; /* Android uses ASCII */
        smsc.num_text.no_of_characters = strlen(((exe_csca_request_data_t *) record_p->request_data_p)->text_p);
        smsc.ton = exe_convert_type_of_number(((exe_csca_request_data_t *) record_p->request_data_p)->ton);
        smsc.npi = STE_SIM_NPI_ISDN;

        if (smsc.num_text.no_of_characters > SMSC_MAX_LENGTH) {
            ATC_LOG_E("request_sim_set_smsc: smsc address too long");
            goto error;
        }

        if (UICC_REQUEST_STATUS_OK
                != ste_uicc_sim_smsc_set_active(sim_client_get_handle(sim_client_p),
                                                (uintptr_t)request_record_get_client_tag(record_p),
                                                &smsc)) {
            ATC_LOG_E("request_sim_set_smsc: request error");
            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    } else {
        ATC_LOG_I("request_sim_set_smsc <- response");
        exe_cmee_error_t error_code;

        if (record_p->client_result_code) {
            ste_uicc_sim_smsc_set_active_response_t *smsc_response_p = NULL;

            if (NULL == record_p->response_data_p) {
                ATC_LOG_E("request_sim_set_smsc(): response data is NULL");
                goto error;
            }

            smsc_response_p = (ste_uicc_sim_smsc_set_active_response_t *) record_p->response_data_p;

            error_code = convert_sim_status_codes_to_cmee_error(smsc_response_p->uicc_status_code,
                         smsc_response_p->uicc_status_code_fail_details,
                         smsc_response_p->status_word);
        } else {
            error_code = CMEE_OPERATION_NOT_ALLOWED;
        }

        exe_request_complete(record_p, EXE_SUCCESS, NULL);
        return EXE_SUCCESS;
    }

error:

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, NULL);
    }

    return EXE_FAILURE;
}


exe_request_result_t request_sim_get_smsc(exe_request_record_t *record_p)
{
    if (EXE_STATE_REQUEST == record_p->state) {
        sim_client_t *sim_client_p = exe_get_sim_client(record_p->exe_p);

        ATC_LOG_I("request_sim_get_smsc -> request");

        if (NULL == sim_client_p) {
            ATC_LOG_E("request_sim_get_smsc: sim client is NULL");
            goto error;
        }

        if (UICC_REQUEST_STATUS_OK
                != ste_uicc_sim_smsc_get_active(sim_client_get_handle(sim_client_p),
                                                (uintptr_t) request_record_get_client_tag(record_p))) {
            ATC_LOG_E("request_sim_get_smsc: sim client is NULL");
            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    } else {
        ATC_LOG_I("request_sim_get_smsc <- response");
        exe_csca_response_data_t response = { NULL, 0, CMEE_OK };

        if (record_p->client_result_code) {
            ste_uicc_sim_smsc_get_active_response_t *smsc_response_p = NULL;
            ste_uicc_status_word_t status_word = {0, 0};
            uint32_t size;
            int32_t n;

            if (NULL == record_p->response_data_p) {
                ATC_LOG_E("request_sim_get_smsc: response data is NULL");
                goto error;
            }

            smsc_response_p = (ste_uicc_sim_smsc_get_active_response_t *)record_p->response_data_p;

            response.error_codes = convert_sim_status_codes_to_cmee_error(smsc_response_p->uicc_status_code,
                                   smsc_response_p->uicc_status_code_fail_details,
                                   status_word);

            /* In case the record is not found by SIM then SIM_ISO_NORMAL_COMPLETION
             * is returned and text_p is set to NULL. This is not treated as an error in
             * SIM, it means no record was found (but the AT command fails).
             */
            if (NULL != smsc_response_p->smsc.num_text.text_p) {

                if (STE_SIM_NPI_ISDN == smsc_response_p->smsc.npi) {
                    switch (smsc_response_p->smsc.ton) {
                    case STE_SIM_TON_UNKNOWN: {
                        response.tosca = EXE_TYPE_OF_NUMBER_UNKNOWN;
                        break;
                    }

                    case STE_SIM_TON_INTERNATIONAL: {
                        response.tosca = EXE_TYPE_OF_NUMBER_INTERNATIONAL;
                        break;
                    }

                    case STE_SIM_TON_NATIONAL: {
                        response.tosca = EXE_TYPE_OF_NUMBER_NATIONAL;
                        break;
                    }

                    default: {
                        ATC_LOG_E("request_sim_get_smsc: unsupported ton");
                        goto error;
                    }
                    }
                } else {
                    ATC_LOG_E("request_sim_get_smsc: unsupported npi");
                    goto error;
                }

                /* SIM is expected to return the text in ASCII encoding,
                 * the code below depends on that.
                 */
                if (STE_SIM_ASCII8 != smsc_response_p->smsc.num_text.text_coding) {
                    ATC_LOG_E("request_sim_get_smsc: unexpected encoding");
                    goto error;
                }

                size = smsc_response_p->smsc.num_text.no_of_characters;

                if (STE_SIM_TON_INTERNATIONAL == smsc_response_p->smsc.ton) {
                    size++; /* Since it is international, a "+" should be added */
                }

                size++; /* NULL termination */

                response.text_p = (char *) alloca(size);

                if (NULL == response.text_p) {
                    ATC_LOG_E("request_sim_get_smsc: could not allocate response.text_p");
                    goto error;
                }

                if (STE_SIM_TON_INTERNATIONAL == smsc_response_p->smsc.ton) {
                    /* Add "+" infront */
                    n = snprintf(response.text_p,
                                 size,
                                 "+%s",
                                 (char *) smsc_response_p->smsc.num_text.text_p);
                } else {
                    n = snprintf(response.text_p,
                                 size,
                                 "%s",
                                 (char *) smsc_response_p->smsc.num_text.text_p);

                }

                if (n < 0) {
                    ATC_LOG_E("request_sim_get_smsc: invalid format in request data");
                    goto error;
                } else if ((uint32_t) n >= size) {
                    ATC_LOG_I("request_sim_get_smsc: Warning! truncated number string");
                }
            }
        } else {
            response.error_codes = CMEE_OPERATION_NOT_ALLOWED;
            response.text_p = NULL;
            response.tosca = 0;
        }

        exe_request_complete(record_p, EXE_SUCCESS, &response);
        return EXE_SUCCESS;
    }

error:

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, NULL);
    }

    return EXE_FAILURE;
}

/*
 * Get the number of existing smsc profiles on the SIM.
 */
exe_request_result_t request_smsc_profiles_max(exe_request_record_t *record_p)
{
    uicc_request_status_t result = UICC_REQUEST_STATUS_OK;
    exe_cres_csas_test_t smcs_profiles_response;
    sim_client_t *sim_client_p = exe_get_sim_client(record_p->exe_p);
    ste_sim_t *uicc_p = sim_client_get_handle(sim_client_p);

    if (NULL == sim_client_p) {
        ATC_LOG_E("request_smcs_pofiles, could not get sim client.");
        goto error;
    }

    if (NULL == uicc_p) {
        ATC_LOG_E("request_smcs_pofiles, could not get sim handle.");
        goto error;
    }

    if (EXE_STATE_REQUEST == record_p->state) {
        ATC_LOG_I("request_smcs_pofiles -> request");

        result = ste_uicc_sim_smsc_get_record_max(uicc_p,
                 (uintptr_t) request_record_get_client_tag(record_p));

        if (UICC_REQUEST_STATUS_OK != result) {
            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    } else {
        ATC_LOG_I("request_smcs_pofiles <- response");

        if (record_p->client_result_code) {
            ste_uicc_sim_smsc_get_record_max_response_t *profiles_response_p = NULL;
            profiles_response_p = (ste_uicc_sim_smsc_get_record_max_response_t *)(record_p->response_data_p);

            if (NULL == profiles_response_p) {
                ATC_LOG_E("request_smcs_pofiles: open_response_p = NULL");
                goto error;
            }

            smcs_profiles_response.error = convert_sim_status_codes_to_cms_error(profiles_response_p->uicc_status_code,
                                           profiles_response_p->uicc_status_code_fail_details,
                                           profiles_response_p->status_word);

            if (EXE_CMS_OK != smcs_profiles_response.error) {
                smcs_profiles_response.profiles = 0;
            } else {
                smcs_profiles_response.profiles = profiles_response_p->max_record_id;
            }

        } else {
            smcs_profiles_response.error = EXE_CMS_OPERATION_NOT_ALLOWED;
            smcs_profiles_response.profiles = 0;
        }

        exe_request_complete(record_p, EXE_SUCCESS, &smcs_profiles_response);
        return EXE_SUCCESS;
    }

error:

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, NULL);
    }

    return EXE_FAILURE;
}

/*
 * Save the smcs profiles on the SIM.
 */
exe_request_result_t request_save_smcs_to_profile(exe_request_record_t *record_p)
{
    uicc_request_status_t result = UICC_REQUEST_STATUS_OK;
    uint8_t *profile_p = NULL;
    exe_cres_csas_response_t save_response;
    sim_client_t *sim_client_p = exe_get_sim_client(record_p->exe_p);
    ste_sim_t *uicc_p = sim_client_get_handle(sim_client_p);

    if (NULL == sim_client_p) {
        ATC_LOG_E("request_save_smcs_to_profile, could not get sim client.");
        goto error;
    }

    if (NULL == uicc_p) {
        ATC_LOG_E("request_save_smcs_to_profile, could not get sim handle.");
        goto error;
    }

    if (EXE_STATE_REQUEST == record_p->state) {
        ATC_LOG_I("request_save_smcs_to_profile -> request");

        if (NULL == record_p->request_data_p) {
            ATC_LOG_E("request_save_smcs_to_profile, incorrect in data");
            goto error;
        }

        result = ste_uicc_sim_smsc_get_record_max(uicc_p,
                 (uintptr_t) request_record_get_client_tag(record_p));

        if (UICC_REQUEST_STATUS_OK != result) {
            goto error;
        }

        /* Need to save profile for to use when we get the response */
        exe_cres_csas_t *cres_data_p = record_p->request_data_p;
        profile_p = malloc(sizeof(uint8_t));

        if (NULL == profile_p) {
            ATC_LOG_E("request_save_smcs_to_profile: could not allocate memory");
            goto error;
        }

        *profile_p = cres_data_p->profile;
        record_p->client_data_p = profile_p;

        record_p->state = EXE_STATE_SIM_GET_PROFILES;
        return EXE_PENDING;

    } else if (EXE_STATE_SIM_GET_PROFILES == record_p->state) {

        ATC_LOG_I("request_save_smcs_to_profile <- response");
        uint8_t profile = 0;
        profile_p = (uint8_t *) record_p->client_data_p;
        profile = *profile_p;
        free(profile_p); /* Free this data directly as it is saved in profile*/

        if (record_p->client_result_code) {
            ste_uicc_sim_smsc_get_record_max_response_t *profiles_response_p = NULL;
            profiles_response_p = (ste_uicc_sim_smsc_get_record_max_response_t *)(record_p->response_data_p);

            if (NULL == profiles_response_p) {
                ATC_LOG_E("request_save_smcs_to_profile: open_response_p = NULL");
                goto error;
            }

            save_response.error = convert_sim_status_codes_to_cms_error(profiles_response_p->uicc_status_code,
                                  profiles_response_p->uicc_status_code_fail_details,
                                  profiles_response_p->status_word);

            if (EXE_CMS_OK == save_response.error) {
                if (profile > profiles_response_p->max_record_id) {
                    save_response.error = EXE_CMS_SIM_FAILURE;
                } else {
                    result = ste_uicc_sim_smsc_save_to_record(uicc_p,
                             (uintptr_t) request_record_get_client_tag(record_p),
                             profile);

                    if (UICC_REQUEST_STATUS_OK != result) {
                        goto error;
                    }

                    record_p->state = EXE_STATE_SIM_SAVE_SMCS;
                    return EXE_PENDING;
                }
            }

        } else {
            save_response.error = EXE_CMS_OPERATION_NOT_ALLOWED;
        }

        exe_request_complete(record_p, EXE_SUCCESS, &save_response);
        return EXE_SUCCESS;
    } else {
        ste_uicc_sim_smsc_save_to_record_response_t *save_response_p = NULL;
        save_response_p = (ste_uicc_sim_smsc_save_to_record_response_t *)(record_p->response_data_p);

        if (NULL == save_response_p) {
            ATC_LOG_E("request_save_smcs_to_profile: open_response_p = NULL");
            goto error;
        }

        save_response.error = convert_sim_status_codes_to_cms_error(save_response_p->uicc_status_code,
                              save_response_p->uicc_status_code_fail_details,
                              save_response_p->status_word);
        exe_request_complete(record_p, EXE_SUCCESS, &save_response);
        return EXE_SUCCESS;
    }

error:

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, NULL);
    }

    return EXE_FAILURE;
}

/*
 * restore the smcs profiles on the SIM.
 */
exe_request_result_t request_restore_smsc_profile(exe_request_record_t *record_p)
{
    uicc_request_status_t result = UICC_REQUEST_STATUS_OK;
    uint8_t *profile_p = NULL;
    exe_cres_csas_response_t restore_response;
    sim_client_t *sim_client_p = exe_get_sim_client(record_p->exe_p);
    ste_sim_t *uicc_p = sim_client_get_handle(sim_client_p);

    if (NULL == sim_client_p) {
        ATC_LOG_E("request_restore_smsc_profile, could not get sim client.");
        goto error;
    }

    if (NULL == uicc_p) {
        ATC_LOG_E("request_restore_smsc_profile, could not get sim handle.");
        goto error;
    }

    if (EXE_STATE_REQUEST == record_p->state) {
        ATC_LOG_I("request_restore_smsc_profile -> request");

        if (NULL == record_p->request_data_p) {
            ATC_LOG_E("request_restore_smsc_profile, incorrect in data");
            goto error;
        }

        result = ste_uicc_sim_smsc_get_record_max(uicc_p,
                 (uintptr_t) request_record_get_client_tag(record_p));

        if (UICC_REQUEST_STATUS_OK != result) {
            goto error;
        }

        /* Need to save profile for to use when we get the response */
        exe_cres_csas_t *cres_data_p = record_p->request_data_p;
        profile_p = malloc(sizeof(uint8_t));

        if (NULL == profile_p) {
            ATC_LOG_E("request_restore_smsc_profile: could not allocate memory");
            goto error;
        }

        *profile_p = cres_data_p->profile;
        record_p->client_data_p = profile_p;

        record_p->state = EXE_STATE_SIM_GET_PROFILES;
        return EXE_PENDING;

    } else if (EXE_STATE_SIM_GET_PROFILES == record_p->state) {

        ATC_LOG_I("request_save_smcs_to_profile <- response");
        uint8_t profile = 0;
        profile_p = (uint8_t *) record_p->client_data_p;
        profile = *profile_p;
        free(profile_p); /* Free this data directly as it is saved in profile*/

        if (record_p->client_result_code) {
            ste_uicc_sim_smsc_get_record_max_response_t *profiles_response_p = NULL;
            profiles_response_p = (ste_uicc_sim_smsc_get_record_max_response_t *)(record_p->response_data_p);

            if (NULL == profiles_response_p) {
                ATC_LOG_E("request_restore_smsc_profile: open_response_p = NULL");
                goto error;
            }

            restore_response.error = convert_sim_status_codes_to_cms_error(profiles_response_p->uicc_status_code,
                                     profiles_response_p->uicc_status_code_fail_details,
                                     profiles_response_p->status_word);

            if (EXE_CMS_OK == restore_response.error) {
                if (profile > profiles_response_p->max_record_id) {
                    restore_response.error = EXE_CMS_SIM_FAILURE;
                } else {
                    result = ste_uicc_sim_smsc_restore_from_record(uicc_p,
                             (uintptr_t) request_record_get_client_tag(record_p),
                             profile);

                    if (UICC_REQUEST_STATUS_OK != result) {
                        goto error;
                    }

                    record_p->state = EXE_STATE_SIM_RESTORE_SMCS;
                    return EXE_PENDING;
                }
            }

        } else {
            restore_response.error = EXE_CMS_OPERATION_NOT_ALLOWED;
        }

        exe_request_complete(record_p, EXE_SUCCESS, &restore_response);
        return EXE_SUCCESS;
    } else {
        ste_uicc_sim_smsc_restore_from_record_response_t *save_response_p = NULL;
        save_response_p = (ste_uicc_sim_smsc_restore_from_record_response_t *)(record_p->response_data_p);

        if (NULL == save_response_p) {
            ATC_LOG_E("request_restore_smsc_profile: open_response_p = NULL");
            goto error;
        }

        restore_response.error = convert_sim_status_codes_to_cms_error(save_response_p->uicc_status_code,
                                 save_response_p->uicc_status_code_fail_details,
                                 save_response_p->status_word);
        exe_request_complete(record_p, EXE_SUCCESS, &restore_response);
        return EXE_SUCCESS;
    }

error:

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, NULL);
    }

    return EXE_FAILURE;
}

/*
 * Generic sim-io.
 */
exe_request_result_t request_generic_sim_access(exe_request_record_t *record_p)
{
    uicc_request_status_t result = UICC_REQUEST_STATUS_OK;

    if (EXE_STATE_REQUEST == record_p->state) {
        sim_client_t *sim_client_p = exe_get_sim_client(record_p->exe_p);
        ste_sim_t *ste_sim_p = NULL;
        ATC_LOG_I("request_generic_sim_io -> request");

        if (NULL == record_p->request_data_p ||
                NULL == ((exe_generic_sim_io_t *)record_p->request_data_p)->apdu_p ||
                NULL == sim_client_p) {
            ATC_LOG_E("request_generic_sim_io, incorrect in data");
            goto error;
        }

        exe_generic_sim_io_t *generic_sim_io_p = record_p->request_data_p;

        ste_sim_p = sim_client_get_handle(sim_client_p);

        if (NULL == ste_sim_p) {
            goto error;
        }

        result = ste_uicc_sim_channel_send(ste_sim_p,
                                           (uintptr_t)request_record_get_client_tag(record_p),
                                           generic_sim_io_p->session_id,
                                           generic_sim_io_p->apdu_length,
                                           generic_sim_io_p->apdu_p);

        if (UICC_REQUEST_STATUS_OK != result) {
            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    } else {
        ATC_LOG_I("request_generic_sim_io <- response");
        exe_generic_sim_io_response_t generic_sim_io_response;

        if (record_p->client_result_code) {
            ste_uicc_sim_channel_send_response_t *channel_send_data_p = NULL;
            channel_send_data_p = (ste_uicc_sim_channel_send_response_t *)(record_p->response_data_p);

            if (NULL == channel_send_data_p) {
                ATC_LOG_E("request_verify_sim_pin: status = NULL");
                goto error;
            }

            generic_sim_io_response.error = convert_sim_status_codes_to_cmee_error(channel_send_data_p->uicc_status_code,
                                            channel_send_data_p->uicc_status_code_fail_details,
                                            channel_send_data_p->status_word);

            generic_sim_io_response.apdu_data_length = channel_send_data_p->apdu_len;
            generic_sim_io_response.apdu_data_p = channel_send_data_p->apdu;

        } else {
            generic_sim_io_response.error = CMEE_OPERATION_NOT_ALLOWED;
            generic_sim_io_response.apdu_data_length = 0;
            generic_sim_io_response.apdu_data_p = NULL;
        }

        exe_request_complete(record_p, EXE_SUCCESS, &generic_sim_io_response);
        return EXE_SUCCESS;
    }

error:

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, NULL);
    }

    return EXE_FAILURE;
}

/********************************************************************
 * SIM-IO
 ********************************************************************
 */

/**
 * Request SIM I/O operation. This is used for AT+CRSM with command set to 192, i.e. GET RESPONSE.
 */
exe_request_result_t request_sim_io_file_information_command(exe_request_record_t *record_p)
{
    if (EXE_STATE_REQUEST == record_p->state) {
        ATC_LOG_I("request_sim_io_file_information_command -> request");
        exe_sim_io_t *sim_io_data_p = record_p->request_data_p;
        sim_client_t *sim_client_p = exe_get_sim_client(record_p->exe_p);
        uicc_request_status_t  uicc_request_status = UICC_REQUEST_STATUS_OK;
        ste_uicc_sim_get_file_info_type_t info_type = STE_UICC_SIM_GET_FILE_INFO_TYPE_EF;

        if (NULL == sim_client_p) {
            ATC_LOG_E("sim_client_p is null");
            goto error;
        }

        /* Convert to sim info type */
        switch (sim_io_data_p->info_type) {
        case EXE_CRSM_RESPONSE_INFO:
            info_type = STE_UICC_SIM_GET_FILE_INFO_TYPE_EF;
            break;

        case EXE_CRSM_STATUS_INFO:
            info_type = STE_UICC_SIM_GET_FILE_INFO_TYPE_DF;
            break;

        default:
            info_type = STE_UICC_SIM_GET_FILE_INFO_TYPE_EF;
            break;
        }

        uicc_request_status = ste_uicc_sim_get_file_information(sim_client_get_handle(sim_client_p),
                              (uintptr_t)request_record_get_client_tag(record_p),
                              (int)(sim_io_data_p->file_id),
                              sim_io_data_p->path_p,
                              info_type);

        if (UICC_REQUEST_STATUS_OK != uicc_request_status) {
            ATC_LOG_E("request_sim_io_file_information_command, ste_uicc_sim_get_file_information failed = %d" , uicc_request_status);
            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    } else {
        ATC_LOG_I("request_sim_io_file_information_command <- response");
        exe_sim_io_t sim_io_response_data;

        if (record_p->client_result_code) {
            ste_uicc_get_file_information_response_t *file_information_p = NULL;
            exe_cmee_error_t error_code;

            if (NULL == record_p->response_data_p) {
                ATC_LOG_E("request_sim_io_file_information_command, record_p->response_data_p is NULL");
                goto error;
            }

            file_information_p = (ste_uicc_get_file_information_response_t *)(record_p->response_data_p);

            error_code = convert_sim_status_codes_to_cmee_error(file_information_p->uicc_status_code,
                         file_information_p->uicc_status_code_fail_details,
                         file_information_p->status_word);

            sim_io_response_data.sw_1 = file_information_p->status_word.sw1;
            sim_io_response_data.sw_2 = file_information_p->status_word.sw2;

            /* TODO test on specific error codes for different files */
            if (CMEE_OK != error_code) {
                if ((0 == file_information_p->status_word.sw1) && (0 == file_information_p->status_word.sw2)) {
                    sim_io_response_data.sw_1 = 0x94;    /* Not provided from this modem version. Hardcoded 148 = no file found*/
                    sim_io_response_data.sw_2 = 0x04;    /* Not provided from this modem version. Hardcoded 4 = no file found*/
                }
            }

            /* Due to different behavior among modem releases
             * We need to set a value if we get sw1 and sw2 to zero and the operation has succeded.  */
            if ((0 == sim_io_response_data.sw_1) && (0 == sim_io_response_data.sw_2)) {
                /* Set values for OK response */
                sim_io_response_data.sw_1 = 0x90;  /* Hardcoded 90 = OK  */
                sim_io_response_data.sw_2 = 0x00;  /* Hardcoded 0 = OK  */
            }

            sim_io_response_data.error_codes = CMEE_OK;
            sim_io_response_data.sim_io_response_p = (char *)file_information_p->fcp;
            sim_io_response_data.length = file_information_p->length;
        } else { /* SIM daemon did not handle the request but gave a error code.*/
            sim_io_response_data.error_codes = CMEE_OPERATION_NOT_ALLOWED;
            sim_io_response_data.sw_1 = 0;
            sim_io_response_data.sw_2 = 0;
            sim_io_response_data.sim_io_response_p = NULL;
            sim_io_response_data.length = 0;
        }

        exe_request_complete(record_p, EXE_SUCCESS, &sim_io_response_data);
        return EXE_SUCCESS;
    }

error:

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, NULL);
    }

    return EXE_FAILURE;
}

/**
 * Request SIM I/O operation. This is used for AT+CRSM with command set to 176, i.e. READ_BINARY.
 */
exe_request_result_t request_sim_io_read_binary(exe_request_record_t *record_p)
{
    if (EXE_STATE_REQUEST == record_p->state) {
        ATC_LOG_I("request_sim_io_read_binary -> request");
        exe_sim_io_t *sim_io_data_p = record_p->request_data_p;
        sim_client_t *sim_client_p = exe_get_sim_client(record_p->exe_p);
        uicc_request_status_t  uicc_request_status = UICC_REQUEST_STATUS_OK;
        int offset = 0;

        if (NULL == record_p->request_data_p ||
                NULL == sim_client_p) {
            ATC_LOG_E("request_sim_io_read_binary, incorrect in data");
            goto error;
        }

        /* p_1 = offset high and p_2 = offset low  */
        offset = (((int)sim_io_data_p->p_1 << 8) | sim_io_data_p->p_2);

        uicc_request_status = ste_uicc_sim_file_read_binary(sim_client_get_handle(sim_client_p),
                              (uintptr_t)request_record_get_client_tag(record_p),
                              (int)(sim_io_data_p->file_id),
                              offset,
                              (int)(sim_io_data_p->p_3),
                              sim_io_data_p->path_p);

        if (UICC_REQUEST_STATUS_OK != uicc_request_status) {
            ATC_LOG_E("request_sim_io_read_binary ste_uicc_sim_file_read_binary failed = %d" , uicc_request_status);
            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    } else {
        ATC_LOG_I("request_sim_io_read_binary <- response");
        exe_sim_io_t sim_io_response_data;

        if (record_p->client_result_code) {
            ste_uicc_sim_file_read_binary_response_t *binary_data_p = NULL;
            exe_cmee_error_t error_code;

            if (NULL == record_p->response_data_p) {
                ATC_LOG_E("request_sim_io_read_binary: record_p->response_data_p = NULL");
                goto error;
            }

            binary_data_p = (ste_uicc_sim_file_read_binary_response_t *)(record_p->response_data_p);

            sim_io_response_data.sw_1 = binary_data_p->status_word.sw1;
            sim_io_response_data.sw_2 = binary_data_p->status_word.sw2;

            error_code = convert_sim_status_codes_to_cmee_error(binary_data_p->uicc_status_code,
                         binary_data_p->uicc_status_code_fail_details,
                         binary_data_p->status_word);

            /* TODO test on specific error codes for different files */
            if (CMEE_OK != error_code) {
                if ((0 == binary_data_p->status_word.sw1) && (0 == binary_data_p->status_word.sw2)) {
                    sim_io_response_data.sw_1 = 0x94;    /* Not provided from this modem version. Hardcoded 148 = no file found*/
                    sim_io_response_data.sw_2 = 0x04;    /* Not provided from this modem version. Hardcoded 4 = no file found*/
                }
            }

            /* Due to different behavior among modem releases
             * We need to set a value if we get sw1 and sw2 to zero and the operation has succeded.  */
            if ((0 == sim_io_response_data.sw_1) && (0 == sim_io_response_data.sw_2)) {
                /* Set values for OK response */
                sim_io_response_data.sw_1 = 0x90;  /* Hardcoded 90 = OK  */
                sim_io_response_data.sw_2 = 0x00;  /* Hardcoded 0 = OK  */
            }

            sim_io_response_data.error_codes = CMEE_OK;
            sim_io_response_data.sim_io_response_p = (char *)binary_data_p->data;
            sim_io_response_data.length = binary_data_p->length;
        } else { /* SIM daemon did not handle the request but gave a error code.*/
            sim_io_response_data.error_codes = CMEE_OPERATION_NOT_ALLOWED;
            sim_io_response_data.sw_1 = 0;
            sim_io_response_data.sw_2 = 0;
            sim_io_response_data.sim_io_response_p = NULL;
            sim_io_response_data.length = 0;
        }

        exe_request_complete(record_p, EXE_SUCCESS, &sim_io_response_data);
        return EXE_SUCCESS;
    }

error:

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, NULL);
    }

    return EXE_FAILURE;
}

/**
 * Request SIM I/O operation. This is used for AT+CRSM with command set to 176, i.e. READ_BINARY for EFSPN.
 */
exe_request_result_t request_sim_io_service_availability_read_binary(exe_request_record_t *record_p)
{
    uicc_request_status_t result = UICC_REQUEST_STATUS_OK;
    exe_sim_io_t *sim_io_data_p = record_p->request_data_p;
    sim_client_t *simclient_p = exe_get_sim_client(record_p->exe_p);
    ste_sim_t *uicc_p = NULL;

    switch (record_p->state) {
    case EXE_STATE_REQUEST: {
        exe_sim_io_t *sim_io_data_p = (exe_sim_io_t *) record_p->request_data_p;
        ATC_LOG_I("request_sim_io_service_availability_read_binary -> request");

        if (NULL == simclient_p) {
            ATC_LOG_E("could not get sim client.");
            goto exit;
        }

        if (NULL == record_p->request_data_p) {
            ATC_LOG_E("sim_io_data_p is null");
            return EXE_FAILURE;
        }

        uicc_p = sim_client_get_handle(simclient_p);

        if (NULL == uicc_p) {
            ATC_LOG_E("could not get sim handle.");
            goto exit;
        }

        record_p->request_data_p = (exe_sim_io_t *)calloc(1, sizeof(exe_sim_io_t));

        if (NULL == record_p->request_data_p) {
            ATC_LOG_E("Could not allocate memory for client_data_p");
            goto error;
        }

        ((exe_sim_io_t *) record_p->request_data_p)->file_id = sim_io_data_p->file_id;
        ((exe_sim_io_t *) record_p->request_data_p)->p_1 = sim_io_data_p->p_1;
        ((exe_sim_io_t *) record_p->request_data_p)->p_2 = sim_io_data_p->p_2;
        ((exe_sim_io_t *) record_p->request_data_p)->p_3 = sim_io_data_p->p_3;

        if (NULL != sim_io_data_p->path_p) {
            ((exe_sim_io_t *) record_p->request_data_p)->path_p = strdup(sim_io_data_p->path_p);
        } else {
            ((exe_sim_io_t *) record_p->request_data_p)->path_p = NULL;
        }

        result = ste_uicc_get_service_availability(uicc_p,
                 (uintptr_t) request_record_get_client_tag(record_p),
                 SIM_SERVICE_TYPE_SPN);

        if (UICC_REQUEST_STATUS_OK != result) {
            ATC_LOG_E("SIM call ste_uicc_get_service_availability failed!, error code %d.", result);
            goto error;
        }

        record_p->state = EXE_STATE_SPN_AVAILABLE;
        return EXE_PENDING;
    }

    case EXE_STATE_SPN_AVAILABLE: {
        int offset = 0;
        exe_sim_io_t *sim_io_data_p = (exe_sim_io_t *) record_p->request_data_p;
        ste_uicc_get_service_availability_response_t *service_rsp_p = record_p->response_data_p;

        if (STE_UICC_STATUS_CODE_OK != service_rsp_p->uicc_status_code) {

            ATC_LOG_E("Get service available failed, status: %d, details: %d",
                      service_rsp_p->uicc_status_code,
                      service_rsp_p->uicc_status_code_fail_details);

            goto error;
        }

        if (service_rsp_p->service_availability == STE_UICC_SERVICE_AVAILABLE) {

            uicc_p = sim_client_get_handle(simclient_p);

            if (NULL == uicc_p) {
                ATC_LOG_E("could not get sim handle.");
                goto error;
            }

            offset = (((int)sim_io_data_p->p_1 << 8) | sim_io_data_p->p_2);

            result = ste_uicc_sim_file_read_binary(uicc_p,
                                                   (uintptr_t)request_record_get_client_tag(record_p),
                                                   (int)(sim_io_data_p->file_id),
                                                   offset,
                                                   (int)(sim_io_data_p->p_3),
                                                   sim_io_data_p->path_p);

            if (UICC_REQUEST_STATUS_OK != result) {
                ATC_LOG_E("request_sim_io_read_binary ste_uicc_sim_file_read_binary failed = %d" , result);
                goto error;
            }

            record_p->state = EXE_STATE_RESPONSE;
            return EXE_PENDING;
        } else {
            goto error;
        }
    }

    case EXE_STATE_RESPONSE: {
        /* Response from SIM */
        ATC_LOG_I("request_sim_io_service_availability_read_binary <- response");
        exe_sim_io_t sim_io_response_data;


        if (record_p->client_result_code) {
            ste_uicc_sim_file_read_binary_response_t *binary_data_p = NULL;
            exe_cmee_error_t error_code;

            if (NULL == record_p->response_data_p) {
                ATC_LOG_E("request_sim_io_read_binary: record_p->response_data_p = NULL");
                goto error;
            }

            binary_data_p = (ste_uicc_sim_file_read_binary_response_t *)(record_p->response_data_p);

            sim_io_response_data.sw_1 = binary_data_p->status_word.sw1;
            sim_io_response_data.sw_2 = binary_data_p->status_word.sw2;

            error_code = convert_sim_status_codes_to_cmee_error(binary_data_p->uicc_status_code,
                         binary_data_p->uicc_status_code_fail_details,
                         binary_data_p->status_word);

            /* TODO test on specific error codes for different files */
            if (CMEE_OK != error_code) {
                if ((0 == binary_data_p->status_word.sw1) && (0 == binary_data_p->status_word.sw2)) {
                    sim_io_response_data.sw_1 = 0x94;    /* Not provided from this modem version. Hardcoded 148 = no file found*/
                    sim_io_response_data.sw_2 = 0x04;    /* Not provided from this modem version. Hardcoded 4 = no file found*/
                }
            }

            /* Due to different behavior among modem releases
             * We need to set a value if we get sw1 and sw2 to zero and the operation has succeded.  */
            if ((0 == sim_io_response_data.sw_1) && (0 == sim_io_response_data.sw_2)) {
                /* Set values for OK response */
                sim_io_response_data.sw_1 = 0x90;  /* Hardcoded 90 = OK  */
                sim_io_response_data.sw_2 = 0x00;  /* Hardcoded 0 = OK  */
            }

            sim_io_response_data.error_codes = CMEE_OK;
            sim_io_response_data.sim_io_response_p = (char *)binary_data_p->data;
            sim_io_response_data.length = binary_data_p->length;
        } else { /* SIM daemon did not handle the request but gave a error code.*/
            sim_io_response_data.error_codes = CMEE_OPERATION_NOT_ALLOWED;
            sim_io_response_data.sw_1 = 0;
            sim_io_response_data.sw_2 = 0;
            sim_io_response_data.sim_io_response_p = NULL;
            sim_io_response_data.length = 0;
        }
        if (NULL !=  record_p->request_data_p)

        {
            free(((exe_sim_io_t *) record_p->request_data_p)->path_p);
            free(record_p->request_data_p);
        }
        exe_request_complete(record_p, EXE_SUCCESS, &sim_io_response_data);
        return EXE_SUCCESS;
    }

    default:
        ATC_LOG_E("Unhandled message state: %d", record_p->state);
        break;
    }

error:

    if (NULL !=  record_p->request_data_p)

    {
        free(((exe_sim_io_t *) record_p->request_data_p)->path_p);
        free(record_p->request_data_p);
    }

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, NULL);
    }

exit:

    return EXE_FAILURE;
}


/**
 * Request SIM I/O operation. This is used for AT+CRSM with command set to 178, i.e. READ_RECORD.
 */
exe_request_result_t request_sim_io_read_record(exe_request_record_t *record_p)
{
    if (EXE_STATE_REQUEST == record_p->state) {
        ATC_LOG_I("request_sim_io_read_record -> request");
        exe_sim_io_t *sim_io_data_p = record_p->request_data_p;
        sim_client_t *sim_client_p = exe_get_sim_client(record_p->exe_p);
        uicc_request_status_t  uicc_request_status = UICC_REQUEST_STATUS_OK;

        if (NULL == record_p->request_data_p ||
                NULL == sim_client_p) {
            ATC_LOG_E("request_sim_io_read_record, incorrect in data");
            goto error;
        }

        uicc_request_status = ste_uicc_sim_file_read_record(sim_client_get_handle(sim_client_p),
                              (uintptr_t)request_record_get_client_tag(record_p),
                              (int)(sim_io_data_p->file_id),
                              (int)(sim_io_data_p->p_1),
                              (int)(sim_io_data_p->p_3),
                              sim_io_data_p->path_p);

        if (UICC_REQUEST_STATUS_OK != uicc_request_status) {
            ATC_LOG_E("request_sim_io_read_record ste_uicc_sim_file_read_record failed = %d" , uicc_request_status);
            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    } else {
        ATC_LOG_I("request_sim_io_read_record <- response");
        exe_sim_io_t sim_io_response_data;

        if (record_p->client_result_code) {
            ste_uicc_sim_file_read_record_response_t *record_data_p = NULL;
            exe_cmee_error_t error_code;

            if (NULL == record_p->response_data_p) {
                ATC_LOG_E("request_sim_io_read_record: binary_data_p = NULL");
                goto error;
            }

            record_data_p = (ste_uicc_sim_file_read_record_response_t *)(record_p->response_data_p);

            sim_io_response_data.sw_1 = record_data_p->status_word.sw1;
            sim_io_response_data.sw_2 = record_data_p->status_word.sw2;

            error_code = convert_sim_status_codes_to_cmee_error(record_data_p->uicc_status_code,
                         record_data_p->uicc_status_code_fail_details,
                         record_data_p->status_word);

            /* TODO test on specific error codes for different files */
            if (CMEE_OK != error_code) {
                if ((0 == record_data_p->status_word.sw1) && (0 == record_data_p->status_word.sw2)) {
                    sim_io_response_data.sw_1 = 0x94;    /* Not provided from this modem version. Hardcoded 148 = no file found*/
                    sim_io_response_data.sw_2 = 0x04;    /* Not provided from this modem version. Hardcoded 4 = no file found*/
                }
            }

            /* Due to different behavior among modem releases
             * We need to set a value if we get sw1 and sw2 to zero and the operation has succeded.  */
            if ((0 == sim_io_response_data.sw_1) && (0 == sim_io_response_data.sw_2)) {
                /* Set values for OK response */
                sim_io_response_data.sw_1 = 0x90;  /* Hardcoded 90 = OK  */
                sim_io_response_data.sw_2 = 0x00;  /* Hardcoded 0 = OK  */
            }

            sim_io_response_data.error_codes = CMEE_OK;
            sim_io_response_data.sim_io_response_p = (char *)record_data_p->data;
            sim_io_response_data.length = record_data_p->length;
        } else { /* SIM daemon did not handle the request but gave a error code.*/
            sim_io_response_data.error_codes = CMEE_OPERATION_NOT_ALLOWED;
            sim_io_response_data.sw_1 = 0;
            sim_io_response_data.sw_2 = 0;
            sim_io_response_data.sim_io_response_p = NULL;
            sim_io_response_data.length = 0;
        }

        exe_request_complete(record_p, EXE_SUCCESS, &sim_io_response_data);
        return EXE_SUCCESS;
    }

error:

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, NULL);
    }

    return EXE_FAILURE;
}

/**
 * Request SIM I/O operation. This is used for AT+CRSM with command set to 214, i.e. UPDATE_BINARY.
 */
exe_request_result_t request_sim_io_update_binary(exe_request_record_t *record_p)
{
    if ((EXE_STATE_REQUEST == record_p->state)) {
        ATC_LOG_I("request_sim_io_update_binary -> request");
        exe_sim_io_t *sim_io_data_p = record_p->request_data_p;
        sim_client_t *sim_client_p = exe_get_sim_client(record_p->exe_p);
        uicc_request_status_t  uicc_request_status = UICC_REQUEST_STATUS_OK;
        int offset = 0;

        if (NULL == record_p->request_data_p ||
                NULL == sim_client_p) {
            ATC_LOG_E("request_sim_io_update_binary, incorrect in data");
            goto error;
        }

        /* p_1 = offset high and p_2 = offset low  */
        offset = (((int)sim_io_data_p->p_1 << 8) | sim_io_data_p->p_2);

        uicc_request_status = ste_uicc_sim_file_update_binary(sim_client_get_handle(sim_client_p),
                              (uintptr_t)request_record_get_client_tag(record_p),
                              (int)(sim_io_data_p->file_id),
                              offset,
                              (int)(sim_io_data_p->p_3),
                              sim_io_data_p->path_p,
                              (uint8_t *)sim_io_data_p->data_p);

        if (UICC_REQUEST_STATUS_OK != uicc_request_status) {
            ATC_LOG_E("request_sim_io_update_binary ste_uicc_sim_file_read_binary failed = %d" , uicc_request_status);
            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    } else {
        ATC_LOG_I("request_sim_io_update_binary <- response");
        exe_sim_io_t sim_io_response_data;

        if (record_p->client_result_code) {
            ste_uicc_update_sim_file_binary_response_t *update_binary_p = NULL;
            exe_cmee_error_t error_code;

            if (NULL == record_p->response_data_p) {
                ATC_LOG_E("request_sim_io_update_binary: record_p->response_data_p = NULL");
                goto error;
            }

            update_binary_p = (ste_uicc_update_sim_file_binary_response_t *)(record_p->response_data_p);

            sim_io_response_data.sw_1 = update_binary_p->status_word.sw1;
            sim_io_response_data.sw_2 = update_binary_p->status_word.sw2;

            error_code = convert_sim_status_codes_to_cmee_error(update_binary_p->uicc_status_code,
                         update_binary_p->uicc_status_code_fail_details,
                         update_binary_p->status_word);

            /* TODO test on specific error codes for different files */
            if (CMEE_OK != error_code) {
                if ((0 == update_binary_p->status_word.sw1) && (0 == update_binary_p->status_word.sw2)) {
                    sim_io_response_data.sw_1 = 0x94;    /* Not provided from this modem version. Hardcoded 148 = no file found*/
                    sim_io_response_data.sw_2 = 0x04;    /* Not provided from this modem version. Hardcoded 4 = no file found*/
                }
            }

            /* Due to different behavior among modem releases
             * We need to set a value if we get sw1 and sw2 to zero and the operation has succeded.  */
            if ((0 == sim_io_response_data.sw_1) && (0 == sim_io_response_data.sw_2)) {
                /* Set values for OK response */
                sim_io_response_data.sw_1 = 0x90;  /* Hardcoded 90 = OK  */
                sim_io_response_data.sw_2 = 0x00;  /* Hardcoded 0 = OK  */
            }

            sim_io_response_data.error_codes = CMEE_OK;
            sim_io_response_data.sim_io_response_p = NULL;
            sim_io_response_data.length = 0;
        } else { /* SIM daemon did not handle the request but gave a error code.*/
            sim_io_response_data.error_codes = CMEE_OPERATION_NOT_ALLOWED;
            sim_io_response_data.sw_1 = 0;
            sim_io_response_data.sw_2 = 0;
            sim_io_response_data.sim_io_response_p = NULL;
            sim_io_response_data.length = 0;
        }

        exe_request_complete(record_p, EXE_SUCCESS, &sim_io_response_data);
        return EXE_SUCCESS;
    }

error:

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, NULL);
    }

    return EXE_FAILURE;
}


/**
 * Request SIM I/O operation. This is used for AT+CRSM with command set to 220, i.e. UPDATE_RECORD.
 */
exe_request_result_t request_sim_io_update_record(exe_request_record_t *record_p)
{
    if (EXE_STATE_REQUEST == record_p->state) {
        ATC_LOG_I("request_sim_io_update_record -> request");
        exe_sim_io_t *sim_io_data_p = record_p->request_data_p;
        sim_client_t *sim_client_p = exe_get_sim_client(record_p->exe_p);
        uicc_request_status_t  uicc_request_status = UICC_REQUEST_STATUS_OK;

        if (NULL == record_p->request_data_p ||
                NULL == sim_client_p) {
            ATC_LOG_E("request_sim_io_update_record, incorrect in data");
            goto error;
        }

#ifdef EXE_USE_SIMPB_SERVICE
        /*Storing the request data, as this will be required for calling the SIMPB API*/
        record_p->request_data_p = (exe_sim_io_t *)calloc(1, sizeof(exe_sim_io_t));

        if (NULL == record_p->request_data_p) {
            ATC_LOG_E("Could not allocate memory for client_data_p");
            goto error;
        }

        ((exe_sim_io_t *) record_p->request_data_p)->file_id = sim_io_data_p->file_id;
        ((exe_sim_io_t *) record_p->request_data_p)->p_1 = sim_io_data_p->p_1;
        ((exe_sim_io_t *) record_p->request_data_p)->p_2 = sim_io_data_p->p_2;
        ((exe_sim_io_t *) record_p->request_data_p)->p_3 = sim_io_data_p->p_3;

        if (NULL != sim_io_data_p->path_p) {
            ((exe_sim_io_t *) record_p->request_data_p)->path_p = strdup(sim_io_data_p->path_p);
        }

#endif
        uicc_request_status = ste_uicc_sim_file_update_record(sim_client_get_handle(sim_client_p),
                              (uintptr_t)request_record_get_client_tag(record_p),
                              (int)(sim_io_data_p->file_id),
                              (int)(sim_io_data_p->p_1),
                              (int)(sim_io_data_p->p_3),
                              sim_io_data_p->path_p,
                              (uint8_t *)sim_io_data_p->data_p);

        if (UICC_REQUEST_STATUS_OK != uicc_request_status) {
            ATC_LOG_E("request_sim_io_update_record ste_uicc_sim_file_read_binary failed = %d" , uicc_request_status);
#ifdef EXE_USE_SIMPB_SERVICE

            if (NULL != ((exe_sim_io_t *) record_p->request_data_p)->path_p) {
                free(((exe_sim_io_t *) record_p->request_data_p)->path_p);
            }

            free(record_p->request_data_p);  /*free the stored data*/
#endif
            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    } else if (EXE_STATE_RESPONSE == record_p->state) {
        ATC_LOG_I("request_sim_io_update_record <- response");
        exe_sim_io_t sim_io_response_data;
        exe_cmee_error_t error_code = CMEE_PHONE_FAILURE; /* for initializing*/
#ifdef EXE_USE_SIMPB_SERVICE
        exe_sim_io_t *sim_io_data_p = (exe_sim_io_t *)record_p->request_data_p;
        simpb_client_t *simpb_client_p = exe_get_simpb_client(record_p->exe_p);
        bool res = FALSE;
        ste_simpb_result_t result ;
#endif
        ste_uicc_update_sim_file_record_response_t *update_record_p = NULL;

        if (record_p->client_result_code) {
            if (NULL == record_p->response_data_p) {
                ATC_LOG_E("request_sim_io_update_record: record_p->response_data_p = NULL");
#ifdef EXE_USE_SIMPB_SERVICE

                if (NULL != sim_io_data_p->path_p) {
                    free(sim_io_data_p->path_p);
                }

                free(sim_io_data_p);  /*free the stored data*/
#endif
                goto error;
            }

            update_record_p = (ste_uicc_update_sim_file_record_response_t *)(record_p->response_data_p);

            sim_io_response_data.sw_1 = update_record_p->status_word.sw1;
            sim_io_response_data.sw_2 = update_record_p->status_word.sw2;

            error_code = convert_sim_status_codes_to_cmee_error(update_record_p->uicc_status_code,
                         update_record_p->uicc_status_code_fail_details,
                         update_record_p->status_word);

            /* TODO test on specific error codes for different files */
            if (CMEE_OK != error_code) {
                if ((0 == update_record_p->status_word.sw1) && (0 == update_record_p->status_word.sw2)) {
                    sim_io_response_data.sw_1 = 0x94;    /* Not provided from this modem version. Hardcoded 148 = no file found*/
                    sim_io_response_data.sw_2 = 0x04;    /* Not provided from this modem version. Hardcoded 4 = no file found*/
                }
            }

            /* Due to different behavior among modem releases
             * We need to set a value if we get sw1 and sw2 to zero and the operation has succeded.  */
            if ((0 == sim_io_response_data.sw_1) && (0 == sim_io_response_data.sw_2)) {
                /* Set values for OK response */
                sim_io_response_data.sw_1 = 0x90;  /* Hardcoded 90 = OK  */
                sim_io_response_data.sw_2 = 0x00;  /* Hardcoded 0 = OK  */
            }

            sim_io_response_data.error_codes = CMEE_OK;
            sim_io_response_data.sim_io_response_p = NULL;
            sim_io_response_data.length = 0;
        } else { /* SIM daemon did not handle the request but gave a error code.*/
            sim_io_response_data.error_codes = CMEE_OPERATION_NOT_ALLOWED;
            sim_io_response_data.sw_1 = 0;
            sim_io_response_data.sw_2 = 0;
            sim_io_response_data.sim_io_response_p = NULL;
            sim_io_response_data.length = 0;
        }

#ifdef EXE_USE_SIMPB_SERVICE

        res = simpb_client_is_adn_file(sim_io_data_p->file_id);

        if (res && (CMEE_OK == error_code)) { /*call SIMPB if SIMD has completed successfully*/
            record_p->request_data_p = (exe_sim_io_t *)calloc(1, sizeof(exe_sim_io_t));
            memcpy(record_p->request_data_p, &sim_io_response_data, sizeof(exe_sim_io_t));

            result = ste_simpb_update_uids(simpb_client_get_handle(simpb_client_p),
                                           (uintptr_t)request_record_get_client_tag(record_p),
                                           sim_io_data_p->file_id,
                                           sim_io_data_p->p_1,
                                           sim_io_data_p->path_p);

            if (NULL != sim_io_data_p->path_p) {
                free(sim_io_data_p->path_p);
            }

            free(sim_io_data_p);

            if (STE_SIMPB_SUCCESS != result) {
                ATC_LOG_E("sim_phonebook failed to update UID");
                exe_request_complete(record_p, EXE_SUCCESS, &sim_io_response_data);
                return EXE_SUCCESS;
            }

            record_p->request_data_p = (exe_sim_io_t *)calloc(1, sizeof(exe_sim_io_t));
            memcpy(record_p->request_data_p, &sim_io_response_data, sizeof(exe_sim_io_t));

            record_p->state = EXE_STATE_PB;
            return EXE_PENDING;
        } else {
            if (NULL != sim_io_data_p->path_p) {
                free(sim_io_data_p->path_p);
            }

            free(sim_io_data_p);
#endif
            exe_request_complete(record_p, EXE_SUCCESS, &sim_io_response_data);
            return EXE_SUCCESS;

        }

#ifdef EXE_USE_SIMPB_SERVICE

    } else if (EXE_STATE_PB == record_p->state) {
        exe_request_complete(record_p, EXE_SUCCESS, record_p->request_data_p);
        free(record_p->request_data_p);
        return EXE_SUCCESS;
    }

#endif

error:

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, NULL);
    }

    return EXE_FAILURE;
}

/*
 * Open a channel to a UICC application.
 */
exe_request_result_t request_open_channel(exe_request_record_t *record_p)
{
    uicc_request_status_t result = UICC_REQUEST_STATUS_OK;
    uint16_t *session_id_p = NULL;
    exe_ccho_response_t ccho_response;
    sim_client_t *sim_client_p = exe_get_sim_client(record_p->exe_p);

    if (NULL == sim_client_p) {
        ATC_LOG_E("request_open_channel, could not get sim client.");
        goto error;
    }

    ste_sim_t *uicc_p = sim_client_get_handle(sim_client_p);

    if (NULL == uicc_p) {
        ATC_LOG_E("request_open_channel, could not get sim handle.");
        goto error;
    }

    if (EXE_STATE_REQUEST == record_p->state) {
        ATC_LOG_I("request_open_channel -> request");

        /*  application_id_p == NULL is a valid paramater and therefore not checked. */
        if (NULL == record_p->request_data_p) {
            ATC_LOG_E("request_open_channel, incorrect in data");
            goto error;
        }

        exe_ccho_t *ccho_data_p = record_p->request_data_p;

        result = ste_uicc_sim_channel_open(uicc_p,
                                           (uintptr_t) request_record_get_client_tag(record_p),
                                           ccho_data_p->application_id_length,
                                           ccho_data_p->application_id_p);

        if (UICC_REQUEST_STATUS_OK != result) {
            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    } else {
        ATC_LOG_I("request_open_channel <- response");

        if (record_p->client_result_code) {
            ste_uicc_sim_channel_open_response_t *open_response_p = NULL;
            open_response_p = (ste_uicc_sim_channel_open_response_t *)(record_p->response_data_p);

            if (NULL == open_response_p) {
                ATC_LOG_E("request_open_channel: open_response_p = NULL");
                goto error;
            }

            ccho_response.error = convert_sim_status_codes_to_cmee_error(open_response_p->uicc_status_code,
                                  open_response_p->uicc_status_code_fail_details,
                                  open_response_p->status_word);

            if (CMEE_OK != ccho_response.error) {
                ccho_response.session_id = 0;
            } else {
                ccho_response.session_id = open_response_p->session_id;
            }

        } else {
            ccho_response.error = CMEE_OPERATION_NOT_ALLOWED;
            ccho_response.session_id = 0;
        }

        exe_request_complete(record_p, EXE_SUCCESS, &ccho_response);
        return EXE_SUCCESS;
    }

error:

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, NULL);
    }

    return EXE_FAILURE;
}

/*
 * Close a channel to a UICC application.
 */
exe_request_result_t request_close_channel(exe_request_record_t *record_p)
{
    uicc_request_status_t result = UICC_REQUEST_STATUS_OK;
    uint16_t *session_id_p = NULL;
    exe_cchc_response_t cchc_response;
    sim_client_t *sim_client_p = exe_get_sim_client(record_p->exe_p);

    if (NULL == sim_client_p) {
        ATC_LOG_E("request_get_full_sim_info, could not get sim client.");
        goto error;
    }

    ste_sim_t *uicc_p = sim_client_get_handle(sim_client_p);

    if (NULL == uicc_p) {
        ATC_LOG_E("request_get_full_sim_info, could not get sim handle.");
        goto error;
    }


    if (EXE_STATE_REQUEST == record_p->state) {
        ATC_LOG_I("request_close_channel -> request");

        if (NULL == record_p->request_data_p) {
            ATC_LOG_E("request_close_channel, incorrect in data");
            goto error;
        }

        exe_cchc_t *cchc_data_p = record_p->request_data_p;

        result = ste_uicc_sim_channel_close(uicc_p,
                                            (uintptr_t)request_record_get_client_tag(record_p),
                                            cchc_data_p->session_id);

        if (UICC_REQUEST_STATUS_OK != result) {
            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    } else {
        ATC_LOG_I("request_close_channel <- response");

        if (record_p->client_result_code) {
            ste_uicc_sim_channel_close_response_t *close_response_p = NULL;
            close_response_p = (ste_uicc_sim_channel_close_response_t *)(record_p->response_data_p);

            if (NULL == close_response_p) {
                ATC_LOG_E("request_close_channel: open_response_p = NULL");
                goto error;
            }

            cchc_response.error = convert_sim_status_codes_to_cmee_error(close_response_p->uicc_status_code,
                                  close_response_p->uicc_status_code_fail_details,
                                  close_response_p->status_word);
        } else {
            cchc_response.error = CMEE_OPERATION_NOT_ALLOWED;
        }

        exe_request_complete(record_p, EXE_SUCCESS, &cchc_response);
        return EXE_SUCCESS;
    }

error:

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, NULL);
    }

    return EXE_FAILURE;
}

/**
 * Executor function for registering and unregistering for receiving proactive commands.
 */
exe_request_result_t request_stkc_pc_control(exe_request_record_t *record_p)
{
    exe_request_result_t exe_result = EXE_FAILURE;
    exe_stkc_pc_request_data_t *request_data_p = (exe_stkc_pc_request_data_t *)record_p->request_data_p;
    exe_stkc_pc_request_data_t *registration_type_p = NULL;
    sim_client_t *sim_client_p = NULL;
    sim_client_p = exe_get_sim_client(record_p->exe_p);

    if (NULL == sim_client_p) {
        goto exit;
    }

    ste_sim_t *ste_cat_p = NULL;
    ste_cat_p = sim_client_get_handle(sim_client_p);

    if (NULL == ste_cat_p) {
        goto exit;
    }

    uintptr_t client_tag = (uintptr_t)request_record_get_client_tag(record_p);

    if (EXE_STATE_REQUEST == record_p->state) {
        ATC_LOG_I("request_stkc_pc_control -> request");

        registration_type_p = malloc(sizeof(exe_stkc_pc_request_data_t));

        if (NULL == registration_type_p) {
            ATC_LOG_E("request_stkc_pc_control: malloc failed");
            goto exit;
        }

        registration_type_p->mode = request_data_p->mode;

        if (EXE_STKC_PC_ENABLE == request_data_p->mode) {
            uint32_t reg_events = (STE_CAT_CLIENT_REG_EVENTS_CAT_STATUS        |
                                   STE_CAT_CLIENT_REG_EVENTS_CARD_STATUS       |
                                   STE_CAT_CLIENT_REG_EVENTS_GUI_CAPABLE_CLIENT |
                                   STE_CAT_CLIENT_REG_EVENTS_PC                |
                                   STE_CAT_CLIENT_REG_EVENTS_PC_NOTIFICATION   |
                                   STE_CAT_CLIENT_REG_EVENTS_PC_REFRESH        |
                                   STE_CAT_CLIENT_REG_EVENTS_PC_SETUP_CALL_IND |
                                   STE_CAT_CLIENT_REG_EVENTS_SESSION_END_IND   |
                                   STE_CAT_CLIENT_REG_EVENTS_CAT_INFO);

            if (0 == ste_cat_register(ste_cat_p, client_tag, reg_events)) {
                exe_result = EXE_PENDING;
            }
        } else { /* EXE_STKC_PC_DISABLE */

            if (0 == ste_cat_deregister(ste_cat_p, client_tag)) {
                exe_result = EXE_PENDING;
            }
        }

        if (EXE_PENDING == exe_result) {
            record_p->request_data_p = registration_type_p;
            record_p->state = EXE_STATE_CAT_REGISTER_REQ;
        } else {
            free(registration_type_p);
        }

    } else if (record_p->state == EXE_STATE_CAT_REGISTER_REQ) {
        ATC_LOG_I("request_stkc_pc_control <- response cat register");
        exe_stkc_pc_request_data_t at_response_data = {0};
        at_response_data.mode = ((exe_stkc_pc_request_data_t *) record_p->request_data_p)->mode; /* If all goes well we will keep
                                                                                                  * request_data_p->mode until we get
                                                                                                  * a response on the ste_cat_enable request.
                                                                                                  */
        bool failure_and_free_data = true;

        if (record_p->client_result_code) {
            int sim_response_data = *((int *)record_p->response_data_p);

            /* 0 == response_data -> SAT request when well. */
            if (0 == sim_response_data) {
                ste_cat_enable_type_t cat_enable_type = STE_CAT_ENABLE_TYPE_NONE;

                /* First part of cat register went OK now
                 * a ste_cat_enable must be done.
                 */
                /* Convert at_response_data.mode to ste_cat_enable_type_t */
                if (at_response_data.mode == EXE_STKC_PC_ENABLE) {
                    cat_enable_type = STE_CAT_ENABLE_TYPE_ENABLE;
                } else {
                    cat_enable_type = EXE_STKC_PC_DISABLE;
                }

                if (0 == ste_cat_enable(ste_cat_p, client_tag, cat_enable_type)) {
                    exe_result = EXE_PENDING;
                    record_p->state = EXE_STATE_CAT_ENABLE_REQ;
                    failure_and_free_data = false;
                }
            }
        }

        if (failure_and_free_data) {
            free(record_p->request_data_p);
            exe_request_complete(record_p, EXE_FAILURE, NULL);
            exe_result = EXE_FAILURE;
        }
    } else if (record_p->state == EXE_STATE_CAT_ENABLE_REQ) {
        ATC_LOG_I("request_stkc_pc_control <- response cat ready");
        exe_stkc_pc_request_data_t at_response_data = {0};
        at_response_data.mode = ((exe_stkc_pc_request_data_t *) record_p->request_data_p)->mode;
        free(record_p->request_data_p);

        exe_request_complete(record_p, EXE_SUCCESS, &at_response_data);
        exe_result = EXE_SUCCESS;
    }

exit:
    return exe_result;
}

/**
 * Executor function for fetching the terminal profile from SAT.
 */
exe_request_result_t request_get_terminal_profile(exe_request_record_t *record_p)
{
    exe_request_result_t exe_result = EXE_FAILURE;

    if (EXE_STATE_REQUEST == record_p->state) {
        sim_client_t *sim_client_p = NULL;
        sim_client_p = exe_get_sim_client(record_p->exe_p);

        ATC_LOG_I("request_get_terminal_profile -> request");

        if (NULL == sim_client_p) {
            ATC_LOG_E("request_get_terminal_profile: Failed getting sim client!");
            goto exit;
        }

        ste_sim_t *ste_cat_p = NULL;
        ste_cat_p = sim_client_get_handle(sim_client_p);

        if (NULL == ste_cat_p) {
            ATC_LOG_E("request_get_terminal_profile: Failed getting sim client handle!");
            goto exit;
        }

        uintptr_t client_tag = (uintptr_t)request_record_get_client_tag(record_p);

        if (0 == ste_cat_get_terminal_profile(ste_cat_p, client_tag)) {
            exe_result = EXE_PENDING;
        }

        record_p->state = EXE_STATE_RESPONSE;

    } else {
        ATC_LOG_I("request_get_terminal_profile <- response");

        if (record_p->client_result_code) {
            exe_stkc_get_terminal_profile_response_t *response_data_p = (exe_stkc_get_terminal_profile_response_t *)record_p->response_data_p;

            if (0 == response_data_p->status) {
                exe_request_complete(record_p, EXE_SUCCESS, response_data_p);
                exe_result = EXE_SUCCESS;
            } else {
                ATC_LOG_E("request_get_terminal_profile: Failed getting terminal profile!");
                exe_request_complete(record_p, EXE_FAILURE, response_data_p);
                exe_result = EXE_FAILURE;
            }
        } else {
            exe_request_complete(record_p, EXE_FAILURE, NULL);
            exe_result = EXE_FAILURE;
        }
    }

exit:
    return exe_result;
}

/**
 * Executor function for fetching the SIM Type from SIM.
 */
exe_request_result_t request_get_sim_app_type(exe_request_record_t *record_p)
{
    ste_uicc_sim_app_info_response_t *response_data_p = NULL;
    exe_request_result_t exe_result = EXE_FAILURE;

    if (EXE_STATE_REQUEST == record_p->state) {
        sim_client_t *sim_client_p = NULL;
        sim_client_p = exe_get_sim_client(record_p->exe_p);

        ATC_LOG_I("request_get_sim_app_type -> request");

        if (NULL == sim_client_p) {
            ATC_LOG_E("request_get_sim_app_type: Failed getting sim client!");
            goto error_exit;
        }

        ste_sim_t *ste_sim_p = NULL;
        ste_sim_p = sim_client_get_handle(sim_client_p);

        if (NULL == ste_sim_p) {
            ATC_LOG_E("request_get_sim_app_type: Failed getting sim client handle!");
            goto error_exit;
        }

        uintptr_t client_tag = (uintptr_t)request_record_get_client_tag(record_p);

        if (UICC_REQUEST_STATUS_OK != ste_uicc_get_app_info(ste_sim_p, client_tag)) {
            goto error_exit;
        }

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;

    } else {
        ATC_LOG_I("request_get_sim_app_type <- response");

        if (record_p->client_result_code) {
            response_data_p = (ste_uicc_sim_app_info_response_t *)record_p->response_data_p;

            if (NULL == response_data_p) {
                ATC_LOG_E("request_get_sim_app_type: SIM response pointer is NULL!");
                goto error_exit;
            } else if (0 != response_data_p->status) {
                ATC_LOG_E("request_get_sim_app_type: Failed getting SIM App Type!");
                goto error_exit;
            } else if (SIM_APP_ISIM == response_data_p->app_type) {
                ATC_LOG_E("request_get_sim_app_type: SIM responsed app type SIM_APP_ISIM!");
                goto error_exit;
            } else {
                exe_request_complete(record_p, EXE_SUCCESS, response_data_p);
                return EXE_SUCCESS;
            }
        } else {
            goto error_exit;
        }
    }

error_exit:

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, response_data_p);
    }

    return EXE_FAILURE;
}


/********************************************************************
 * STK
 ********************************************************************
 */
exe_request_result_t request_sat_envelope_command(exe_request_record_t *record_p)
{
    if (EXE_STATE_REQUEST == record_p->state) {
        ste_sim_t *ste_sim_p = NULL;
        sim_client_t *sim_client_p = exe_get_sim_client(record_p->exe_p);

        ATC_LOG_I("request_sat_envelope_command -> request");

        if (NULL == sim_client_p ||
                NULL == record_p->request_data_p ||
                NULL == ((exe_apdu_data_t *)(record_p->request_data_p))->buf_p ||
                0 == ((exe_apdu_data_t *)(record_p->request_data_p))->len) {
            ATC_LOG_E("request_sat_envelope_command: incorrect data");
            goto error;
        }

        ste_sim_p = sim_client_get_handle(sim_client_p);

        if (NULL == ste_sim_p) {
            goto error;
        }

        /* return an int: 0 on success, -1 on failure */
        if (0 != ste_cat_envelope_command(ste_sim_p,
                                          (uintptr_t)request_record_get_client_tag(record_p),
                                          ((exe_apdu_data_t *)(record_p->request_data_p))->buf_p,
                                          ((exe_apdu_data_t *)(record_p->request_data_p))->len)) {
            ATC_LOG_E("request_sat_envelope_command: ste_cat_envelope_command failed");
            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    } else {
        ATC_LOG_I("request_sat_envelope_command <- response");

        if (record_p->client_result_code) {
            exe_apdu_data_t *stke_data_p = NULL;
            ste_cat_ec_response_t *response_data_p = (ste_cat_ec_response_t *) record_p->response_data_p;

            if (NULL == response_data_p) {
                ATC_LOG_E("request_sat_envelope_command: response data is NULL");
                goto error;
            }

            /* STE_CAT_RESPONSE_OK == success, all else is fail */
            if (STE_CAT_RESPONSE_OK != response_data_p->ec_status) {
                ATC_LOG_E("request_sat_envelope_command: cat response != STE_CAT_RESPONSE_OK");
                goto error;
            }

            stke_data_p = (exe_apdu_data_t *)(&(response_data_p->apdu));

            if (NULL == stke_data_p->buf_p ||
                    0 == stke_data_p->len) {
                ATC_LOG_E("request_sat_envelope_command: invalid response data");
                goto error;
            }

            exe_request_complete(record_p, EXE_SUCCESS, stke_data_p);
            return EXE_SUCCESS;
        }
    }

error:

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, NULL);
    }

    return EXE_FAILURE;
}

/**
 * Send a terminal response as a reply to a previous proactive command.
 */
exe_request_result_t request_send_terminal_response(exe_request_record_t *record_p)
{
    exe_request_result_t exe_result = EXE_FAILURE;
    uicc_request_status_t result;


    if (EXE_STATE_REQUEST == record_p->state) {
        ste_sim_t *ste_cat_p = NULL;
        uintptr_t client_tag = 0;
        sim_client_t *sim_client_p = exe_get_sim_client(record_p->exe_p);

        ATC_LOG_I("request_send_terminal_response -> request");

        if (NULL == sim_client_p) {
            ATC_LOG_E("request_send_terminal_response, could not get sim client.");
            goto exit;
        }

        ste_cat_p = sim_client_get_handle(sim_client_p);

        if (NULL == ste_cat_p) {
            ATC_LOG_E("request_send_terminal_response, could not get sim handle.");
            goto exit;
        }

        client_tag = (uintptr_t)request_record_get_client_tag(record_p);

        if (0 == client_tag) {
            ATC_LOG_E("request_send_terminal_response, could not get client tag.");
            goto exit;
        }

        result = ste_cat_terminal_response(ste_cat_p, client_tag, ((exe_apdu_data_t *)(record_p->request_data_p))->buf_p, ((exe_apdu_data_t *)(record_p->request_data_p))->len);

        if (0 != result) {
            ATC_LOG_E("request_send_terminal_response, ste_cat_terminal_response failed.");
            goto exit;
        }

        record_p->state = EXE_STATE_RESPONSE;
        exe_result =  EXE_PENDING;
    } else {
        /* Response from SIM. */
        ATC_LOG_I("request_send_terminal_response <- response");

        if (record_p->client_result_code) {
            ste_cat_tr_response_t *get_status_data_p = NULL;

            get_status_data_p = (ste_cat_tr_response_t *)(record_p->response_data_p);

            if (NULL == get_status_data_p) {
                ATC_LOG_E("ste_cat_terminal_response: get_status_data_p == NULL");
                exe_request_complete(record_p, EXE_FAILURE, NULL);
                goto exit;
            }

            /* 0 == success, everything else is fail */
            if (0 != get_status_data_p->status) {
                ATC_LOG_E("ste_cat_terminal_response: get_status_data_p->status=%d", get_status_data_p->status);
                exe_request_complete(record_p, EXE_FAILURE, NULL);
                goto exit;
            }

        } else {
            exe_request_complete(record_p, EXE_FAILURE, NULL);
            goto exit;
        }

        exe_request_complete(record_p, EXE_SUCCESS, NULL);
        exe_result = EXE_SUCCESS;
    }

exit:

    return exe_result;
}

static void cnum_subscriber_number_cleanup(exe_cnum_record_data_t *record_data_p,
        uint16_t number_of_records)
{
    exe_cnum_record_data_t *rec_data_p = record_data_p;
    int i;

    for (i = 0; i < number_of_records; i++) {

        free(rec_data_p->alpha_p);
        free(rec_data_p->number_p);
        rec_data_p++;
    }

    free(record_data_p);
}



exe_request_result_t request_cnum_subscriber_number(exe_request_record_t *record_p)
{
    exe_request_result_t exe_result = EXE_FAILURE;
    exe_cnum_response_data_t exe_response_data;
    uicc_request_status_t sim_result;
    uint16_t number_of_records = 0;

    exe_response_data.record_data_p = NULL;

    if (EXE_STATE_REQUEST == record_p->state) {
        sim_client_t *sim_client_p = NULL;
        ste_sim_t *ste_uicc_p = NULL;
        uintptr_t client_tag = 0;
        sim_client_p = exe_get_sim_client(record_p->exe_p);

        ATC_LOG_I("request_cnum_subscriber_number -> request");

        if (NULL == sim_client_p) {
            ATC_LOG_E("request_cnum_subscriber_number, could not get sim client.");
            goto exit;
        }

        ste_uicc_p = sim_client_get_handle(sim_client_p);

        if (NULL == ste_uicc_p) {
            ATC_LOG_E("request_cnum_subscriber_number, could not get sim handle.");
            goto exit;
        }

        client_tag = (uintptr_t)request_record_get_client_tag(record_p);

        ATC_LOG_I("request_cnum_subscriber_number (%d): Calling ste_uicc_get_subscriber_number.", __LINE__);
        sim_result = ste_uicc_get_subscriber_number(ste_uicc_p, client_tag);

        if (0 != sim_result) {
            ATC_LOG_E("request_cnum_subscriber_number, ste_uicc_get_subscriber_number failed.");
            goto exit;
        }

        record_p->state = EXE_STATE_RESPONSE;
        exe_result =  EXE_PENDING;

    } else {
        ste_sim_subscriber_number_response_t *response_data_p = (ste_sim_subscriber_number_response_t *)(record_p->response_data_p);
        ste_sim_subscriber_number_record_t *src_rec_data_p = NULL;
        exe_cnum_record_data_t *dest_rec_data_p = NULL;
        exe_cmee_error_t error_code;
        exe_result = EXE_SUCCESS;
        int i = 0;
        int j = 0;
        uint8_t *src_p = NULL;
        uint8_t *dest_p = NULL;
        uint16_t number_of_records = 0;

        ATC_LOG_I("request_cnum_subscriber_number <- response");

        if (NULL == response_data_p) {
            ATC_LOG_E("request_cnum_subscriber_number: response data is NULL");
            exe_result = EXE_FAILURE;
            goto error;
        }

        error_code = convert_sim_status_codes_to_cmee_error(response_data_p->uicc_status_code,
                     response_data_p->uicc_status_code_fail_details,
                     response_data_p->status_word);

        exe_response_data.number_of_records = response_data_p->number_of_records;
        number_of_records = response_data_p->number_of_records;
        exe_response_data.cmee_error_code = error_code;
        src_rec_data_p = response_data_p->record_data_p;


        /* Allocate memory for all records. */
        exe_response_data.record_data_p = (exe_cnum_record_data_t *)malloc(sizeof(exe_cnum_record_data_t) * exe_response_data.number_of_records);

        if (NULL == exe_response_data.record_data_p) {
            ATC_LOG_E("request_cnum_subscriber_number: Memory allocation of record data failed!");
            exe_result = EXE_FAILURE;
            goto error;
        }

        dest_rec_data_p = exe_response_data.record_data_p;

        for (i = 0; i < number_of_records; i++) {
            dest_rec_data_p->alpha_p = NULL;
            dest_rec_data_p->number_p = NULL;
            dest_rec_data_p++;
        }

        dest_rec_data_p = exe_response_data.record_data_p;

        for (i = 0; i < number_of_records; i++) {
            dest_rec_data_p->alpha_id_actual_len = src_rec_data_p->alpha_id_actual_len;
            dest_rec_data_p->number_actual_len = src_rec_data_p->number_actual_len;

            switch (src_rec_data_p->type) {
            case STE_SIM_TON_INTERNATIONAL:
                dest_rec_data_p->type = EXE_TYPE_OF_NUMBER_INTERNATIONAL;
                break;

            case STE_SIM_TON_NATIONAL:
                dest_rec_data_p->type = EXE_TYPE_OF_NUMBER_NATIONAL;
                break;

            default:
                dest_rec_data_p->type = EXE_TYPE_OF_NUMBER_UNKNOWN;
                break;
            }

            dest_rec_data_p->speed = src_rec_data_p->speed;
            dest_rec_data_p->service = src_rec_data_p->service;
            dest_rec_data_p->itc = src_rec_data_p->itc;

            /* Copy alpha. */
            if (src_rec_data_p->alpha_id_actual_len > 0) {
                switch (src_rec_data_p->alpha_coding) {
                case STE_SIM_UCS2:
                    dest_rec_data_p->alpha_coding = (uint8_t)EXE_CHARSET_UCS2;
                    break;

                case STE_SIM_GSM_Default:
                    dest_rec_data_p->alpha_coding = (uint8_t)EXE_CHARSET_GSM8;
                    break;

                default:
                    ATC_LOG_E("request_cnum_subscriber_number: Unsupported coding format for alpha id!");
                    exe_result = EXE_FAILURE;
                    goto error;
                }

                dest_rec_data_p->alpha_p = (uint8_t *)malloc(sizeof(uint8_t) * dest_rec_data_p->alpha_id_actual_len);

                if (NULL == dest_rec_data_p->alpha_p) {
                    ATC_LOG_E("request_cnum_subscriber_number: Memory allocation of alpha failed!");
                    exe_result = EXE_FAILURE;
                    goto error;
                }

                memset(dest_rec_data_p->alpha_p, 0, dest_rec_data_p->alpha_id_actual_len);

                dest_p = dest_rec_data_p->alpha_p;
                src_p = src_rec_data_p->alpha_p;

                for (j = 0; j < dest_rec_data_p->alpha_id_actual_len; j++) {
                    *dest_p = *src_p;
                    dest_p++;
                    src_p++;
                }
            }

            /* Copy number. */
            dest_rec_data_p->number_p = (uint8_t *)malloc(sizeof(uint8_t) * dest_rec_data_p->number_actual_len);

            if (NULL == dest_rec_data_p->number_p) {
                ATC_LOG_E("request_cnum_subscriber_number: Memory allocation of number failed!");
                exe_result = EXE_FAILURE;
                goto error;
            }

            memset(dest_rec_data_p->number_p, 0, dest_rec_data_p->number_actual_len);

            dest_p = dest_rec_data_p->number_p;
            src_p = src_rec_data_p->number_p;

            for (j = 0; j < dest_rec_data_p->number_actual_len; j++) {
                *dest_p = *src_p;
                dest_p++;
                src_p++;
            }

            dest_rec_data_p++;
            src_rec_data_p++;
        }

        exe_request_complete(record_p, EXE_SUCCESS, &exe_response_data);
        /* Clean up and free all memory. */
        cnum_subscriber_number_cleanup(exe_response_data.record_data_p, number_of_records);
    }

exit:
    return exe_result;

error:
    exe_request_complete(record_p, EXE_FAILURE, NULL);
    /* Clean up and free all memory. */
    cnum_subscriber_number_cleanup(exe_response_data.record_data_p, number_of_records);
    return exe_result;

}

exe_request_result_t request_answer_setup_call(exe_request_record_t *record_p)
{
    if (EXE_STATE_REQUEST == record_p->state) {
        sim_client_t *sim_client_p = exe_get_sim_client(record_p->exe_p);
        ste_sim_t *ste_sim_p = NULL;
        exe_setup_call_answer_data_t *answer_data_p = NULL;

        ATC_LOG_I("request_call_initiated_by_sim_response -> request");

        if (NULL == sim_client_p ||
                NULL == record_p->request_data_p) {
            ATC_LOG_E("request_sim_call_initiated_by_sim_response: incorrect data");
            goto error;
        }

        answer_data_p = (exe_setup_call_answer_data_t *) record_p->request_data_p;

        ste_sim_p = sim_client_get_handle(sim_client_p);

        if (NULL == ste_sim_p) {
            goto error;
        }

        /* Return: 0 on success, -1 on failure */
        if (0 != ste_cat_answer_call(ste_sim_p,
                                     (uintptr_t)request_record_get_client_tag(record_p),
                                     answer_data_p->answer)) {
            ATC_LOG_E("ste_cat_answer_call failed");
            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    } else {
        ATC_LOG_I("request_call_initiated_by_sim_response <- response");

        if (record_p->client_result_code) {
            ste_cat_answer_call_response_t *response_data_p = (ste_cat_answer_call_response_t *) record_p->response_data_p;

            if (NULL == response_data_p) {
                ATC_LOG_E("request_call_initiated_by_sim_response: response data is NULL");
                goto error;
            }

            /* 0 == success */
            if (0 != response_data_p->status) {
                ATC_LOG_E("request_call_initiated_by_sim_response: cat response != 0");
                goto error;
            }

            exe_request_complete(record_p, EXE_SUCCESS, NULL);
            return EXE_SUCCESS;
        }
    }

error:

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, NULL);
    }

    return EXE_FAILURE;
}


exe_request_result_t request_sim_restart(exe_request_record_t *record_p)
{
    uicc_request_status_t result = UICC_REQUEST_STATUS_OK;
    exe_cmee_error_t cmee = CMEE_OK;

    if (EXE_STATE_REQUEST == record_p->state) {
        sim_client_t *sim_client_p = exe_get_sim_client(record_p->exe_p);
        ste_sim_t *ste_sim_p = NULL;

        ATC_LOG_I("-> request");

        if (NULL == sim_client_p) {
            ATC_LOG_E("sim_client_p is NULL");
            goto error;
        }

        ste_sim_p = sim_client_get_handle(sim_client_p);

        if (NULL == ste_sim_p) {
            ATC_LOG_E("ste_sim_p is NULL");
            goto error;
        }

        result = ste_uicc_sim_power_off(ste_sim_p, (uintptr_t)request_record_get_client_tag(record_p));

        if (UICC_REQUEST_STATUS_OK != result) {
            ATC_LOG_E("ste_uicc_sim_power_off call failed, error code %d.", result);
            goto error;
        }

        record_p->state = EXE_STATE_SIM_RESTART_INTERMEDIATE;
        return EXE_PENDING;
    } else if (EXE_STATE_SIM_RESTART_INTERMEDIATE == record_p->state) {
        ste_uicc_sim_power_off_response_t *power_off_response = NULL;
        sim_client_t *sim_client_p = exe_get_sim_client(record_p->exe_p);
        ste_sim_t *ste_sim_p = NULL;
        ATC_LOG_I("<- intermediate response");

        power_off_response = (ste_uicc_sim_power_off_response_t *)(record_p->response_data_p);

        if (NULL == power_off_response) {
            ATC_LOG_E("power_off_response is NULL");
            goto error;
        }

        if (STE_UICC_STATUS_CODE_OK != power_off_response->uicc_status_code) {
            cmee = convert_sim_status_codes_to_cmee_error(power_off_response->uicc_status_code,
                    power_off_response->uicc_status_code_fail_details,
                    power_off_response->status_word);

            ATC_LOG_E("power_off_response converted cmee is: %d", cmee);
            goto error;
        }

        if (NULL == sim_client_p) {
            ATC_LOG_E("sim_client_p is NULL");
            goto error;
        }

        ste_sim_p = sim_client_get_handle(sim_client_p);

        if (NULL == ste_sim_p) {
            ATC_LOG_E("ste_sim_p is NULL");
            goto error;
        }

        result = ste_uicc_sim_power_on(ste_sim_p, (uintptr_t)request_record_get_client_tag(record_p));

        if (UICC_REQUEST_STATUS_OK != result) {
            ATC_LOG_E("ste_uicc_sim_power_off call failed, error code %d.", result);
            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    } else { /* EXE_STATE_RESPONSE */
        ste_uicc_sim_power_on_response_t *power_on_response = NULL;
        ATC_LOG_I("<- response");

        power_on_response = (ste_uicc_sim_power_on_response_t *)(record_p->response_data_p);

        if (NULL == power_on_response) {
            ATC_LOG_E("power_on_response is NULL");
            goto error;
        }

        if (STE_UICC_STATUS_CODE_OK != power_on_response->uicc_status_code) {
            cmee = convert_sim_status_codes_to_cmee_error(power_on_response->uicc_status_code,
                    power_on_response->uicc_status_code_fail_details,
                    power_on_response->status_word);

            ATC_LOG_E("power_on_response converted cmee is: %d", cmee);
            goto error;
        }

        exe_request_complete(record_p, EXE_SUCCESS, NULL);
        return EXE_SUCCESS;
    }

error:

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, &cmee);
    }

    return EXE_FAILURE;
}

