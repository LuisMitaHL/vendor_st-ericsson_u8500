/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdlib.h>
#include <string.h>
#include "cops.h"
#include "atc_log.h"
#include "whitebox.h"

static int bind_call_set;

cops_return_code_t cops_context_create(
    cops_context_id_t           **ctxpp_pp,
    const cops_event_callbacks_t *ev_cbs, void *aux_p)
{
    if (NULL == ctxpp_pp) {
        return COPS_RC_ARGUMENT_ERROR;
    }

    *ctxpp_pp = malloc(1);
    return COPS_RC_OK;
}

void cops_context_destroy(cops_context_id_t **ctxpp_pp)
{
    if (NULL != ctxpp_pp && NULL != *ctxpp_pp) {
        free(*ctxpp_pp);
    }
}

cops_return_code_t cops_read_imei(cops_context_id_t *ctxp_p, cops_imei_t *imei_p)
{
    imei_p->digits[0]  = 0x0;
    imei_p->digits[1]  = 0x1;
    imei_p->digits[2]  = 0x2;
    imei_p->digits[3]  = 0x3;
    imei_p->digits[4]  = 0x4;
    imei_p->digits[5]  = 0x5;
    imei_p->digits[6]  = 0x6;
    imei_p->digits[7]  = 0x7;
    imei_p->digits[8]  = 0x8;
    imei_p->digits[9]  = 0x9;
    imei_p->digits[10] = 0x8;
    imei_p->digits[11] = 0x7;
    imei_p->digits[12] = 0x6;
    imei_p->digits[13] = 0x5;
    imei_p->digits[14] = 0x4;
    return COPS_RC_OK;
}

cops_return_code_t cops_simlock_get_status(cops_context_id_t *ctxp,
                                           cops_simlock_status_t *simlock_status)
{
    ATC_LOG_E("%s: entered", __func__);
    cops_simlock_lock_status_t lock_status;
    lock_status.lock_setting = COPS_SIMLOCK_LOCK_SETTING_LOCKED;

    simlock_status->cl_status = lock_status;
    simlock_status->esll_status = lock_status;
    simlock_status->nl_status = lock_status;
    simlock_status->nsl_status = lock_status;
    simlock_status->sim_card_status = COPS_SIMLOCK_CARD_STATUS_APPROVED;
    simlock_status->spl_status = lock_status;
    simlock_status->siml_status = lock_status;

    return COPS_RC_OK;
}

cops_return_code_t cops_simlock_unlock(cops_context_id_t *ctxp,
                                       cops_simlock_lock_type_t lock_type,
                                       cops_simlock_control_key_t *control_key)
{
    return COPS_RC_OK;
}

cops_return_code_t cops_get_challenge(cops_context_id_t *ctxp,
                                      cops_auth_type_t auth_type,
                                      uint8_t *buf, size_t *blen)
{
    uint8_t response_buf[4] = {0xDE, 0xAD, 0xBE, 0xEF};
    if (NULL != buf) {
        memcpy(buf, &response_buf, 4);
    }
    *blen = 4;
    return COPS_RC_OK;
}

cops_return_code_t cops_authenticate(cops_context_id_t *ctx_p, bool permanently,
                                     cops_auth_data_t *auth_data_p)
{
    uint8_t rsa_cmpdata[4] = {0xDE, 0xAD, 0xBE, 0xEF};

    switch (CURRENT_TEST_CASE) {
    case wb_testcase_cops_rsa_authenticate:
        if (0 != memcmp(auth_data_p->data, rsa_cmpdata, 4)) {
            return COPS_RC_INTERNAL_ERROR;
        }
        break;
    }
    return COPS_RC_OK;
}

cops_return_code_t cops_deauthenticate(cops_context_id_t *ctxp,
                                       bool permanently)
{
    return COPS_RC_OK;
}

cops_return_code_t cops_bind_properties(cops_context_id_t *ctxp,
                                        cops_bind_properties_arg_t *arg)
{
    uint8_t imei_cmpdata[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    char *cmp_simlock_key = "11";
    uint8_t simlock_cmpdata[16] = { 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF };
    uint8_t simlock_cmpdata2[] = {0x1, 0x2};
    cops_simlock_control_keys_t *keys_p;
    cops_return_code_t result = COPS_RC_OK;

    switch (CURRENT_TEST_CASE) {
    case wb_testcase_cops_imei:
        if (0 != memcmp(arg->imei->digits, imei_cmpdata, 8)) {
            result = COPS_RC_INTERNAL_ERROR;
        }
        break;
    case wb_testcase_cops_simlock:
        /* Make a spot check on some values to see if its ok */
        keys_p = (cops_simlock_control_keys_t*) arg->auth_data->data;
        if (0 != memcmp(keys_p->nl_key.value, cmp_simlock_key, 2)) {
            result = COPS_RC_INTERNAL_ERROR;
        }
        if (0 != memcmp(keys_p->cl_key.value, cmp_simlock_key, 2)) {
            result = COPS_RC_INTERNAL_ERROR;
        }
        if(arg->imei != NULL){
            result = COPS_RC_INTERNAL_ERROR;
        }
        return result;
        break;
    case wb_testcase_cops_simlockdata:{
        if (0 != memcmp(arg->cops_data, simlock_cmpdata, 16)) {
            result = COPS_RC_INTERNAL_ERROR;
        }
        break;
    }
    case wb_testcase_cops_simlockdata2: {
        if (0 != memcmp(arg->cops_data, simlock_cmpdata2, 2)) {
            result = COPS_RC_INTERNAL_ERROR;
        }
        break;
    }
    default: {
        bind_call_set++;

        if (bind_call_set != 2) {
            return COPS_RC_NOT_AUTHENTICATED_ERROR;
        } else if (bind_call_set == 2) {
            return COPS_RC_OK;
        } else {
            return COPS_RC_OK;
        }
        break;
    }
    }
    return result;
}

cops_return_code_t cops_read_data(cops_context_id_t *ctxp,
                                  uint8_t *buf_p, size_t *blen_p)
{

    uint8_t response_buf[] = { 0xA, 0xB, 0xC, 0xD, 0xE, 0xF };


    switch (CURRENT_TEST_CASE) {
    case wb_testcase_cops_simlockdata_read: {
        if (NULL != buf_p) {
            memcpy(buf_p, &response_buf, 6);
        }
        *blen_p = 6;
        break;
    }
    default: {
        *blen_p = 100;
        if (buf_p != NULL) {
            *(buf_p + 0) = 0x00;
            *(buf_p + 0) = 0x01;
            *(buf_p + 0) = 0x02;
            *(buf_p + 0) = 0x03;
            *(buf_p + 0) = 0x04;
            *(buf_p + 0) = 0x05;
            *(buf_p + 0) = 0x06;
            *(buf_p + 0) = 0x07;
            *(buf_p + 0) = 0x08;
            *(buf_p + 0) = 0x09;
            *(buf_p + 0) = 0x00;
            *(buf_p + 0) = 0x01;
            *(buf_p + 0) = 0x02;
            *(buf_p + 0) = 0x03;
            *(buf_p + 0) = 0x04;
            *(buf_p + 0) = 0x05;
            *(buf_p + 0) = 0x06;
            *(buf_p + 0) = 0x07;
            *(buf_p + 0) = 0x08;
            *(buf_p + 0) = 0x09;
            *(buf_p + 0) = 0x00;
            *(buf_p + 0) = 0x01;
            *(buf_p + 0) = 0x02;
            *(buf_p + 0) = 0x03;
            *(buf_p + 0) = 0x04;
            *(buf_p + 0) = 0x05;
            *(buf_p + 0) = 0x06;
            *(buf_p + 0) = 0x07;
            *(buf_p + 0) = 0x08;
            *(buf_p + 0) = 0x09;
        }
        break;
    }
    }
    return COPS_RC_OK;
}

cops_return_code_t cops_lock_bootpartition(cops_context_id_t *ctxp)
{
    return COPS_RC_OK;
}
