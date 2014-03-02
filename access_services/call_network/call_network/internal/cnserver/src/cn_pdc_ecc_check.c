/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "cn_macros.h"
#include "cn_pdc_internal.h"
#include "cn_pdc_ecc_list_handling.h"

/******************************************************************************
 *                          Local prototypes                                  *
 ******************************************************************************/
static cn_ecc_result_t cn_pdc_ecc_check_list(cn_pdc_input_t* pdc_input_p, cn_emergency_number_list_internal_t *e_n_list_p);

/******************************************************************************
 *                          TOP-LEVEL FUNCTIONS                               *
 ******************************************************************************/

cn_ecc_result_t cn_pdc_ecc_check(cn_pdc_t* pdc_p)
{
    if (!pdc_p || !(pdc_p->pdc_input_p)) {
        CN_LOG_E("pdc_input_p is NULL!");
        goto error;
    }

    if (pdc_p->pdc_input_p->service_type != CN_SERVICE_TYPE_VOICE_CALL) {
        CN_LOG_D("pdc_input_p->service_type is not voice call, skipping ECC");
        goto not_relevant;
    }

    CN_LOG_D("Enter, state = %d", pdc_p->pdc_ecc_state);

    cn_ecc_result_t result = CN_ECC_RESULT_UNKNOWN;
    cn_emergency_number_list_internal_t *e_n_list_p = NULL;
    switch (pdc_p->pdc_ecc_state) {
        case CN_PDC_ECC_STATE_INITIAL:
            /* Perform as many checks as possible before consulting any asynchronous lists (i.e. sim) */

            /* Get list with emergency numbers from the default (3gpp) list */
            e_n_list_p = cn_pdc_get_default_emerg_number_list();

            /* Compare all numbers in that list */
            result = cn_pdc_ecc_check_list(pdc_p->pdc_input_p, e_n_list_p);
            free(e_n_list_p);

            if (CN_ECC_RESULT_UNKNOWN == result) {
                /* If result is still unknown, get list with emergency numbers from user-defined list */
                e_n_list_p = cn_pdc_get_stored_emerg_number_list();

                /* Compare all numbers in that list */
                result = cn_pdc_ecc_check_list(pdc_p->pdc_input_p, e_n_list_p);
                free(e_n_list_p);
            }

            if (CN_ECC_RESULT_UNKNOWN == result) {
                /*If number is still not determined to be ECC, get list with emergency numbers from
                * SIM (or SIM replacement list see 22.101 chapter 10.1.1) */
                (void) cn_pdc_get_sim_emerg_number_list(pdc_p);

                if (CN_PDC_ECC_STATE_WAIT_FOR_SIM_LIST == pdc_p->pdc_ecc_state) {
                    return CN_ECC_RESULT_PENDING;
                } else {
                    /* All responses other than that we are waiting for SIM contact are wrong. */
                    /* TODO: What is the response if there is no SIM available? Should the default list be consulted?*/
                    goto error;
                }
            }
            break;
        case CN_PDC_ECC_STATE_WAIT_FOR_SIM_LIST:
            e_n_list_p = cn_pdc_get_sim_emerg_number_list(pdc_p);

            if ((CN_PDC_ECC_STATE_RECEIVED_SIM_LIST == pdc_p->pdc_ecc_state) && (NULL != e_n_list_p)) {
                /* Compare all numbers in that list */
                result = cn_pdc_ecc_check_list(pdc_p->pdc_input_p, e_n_list_p);
                free(e_n_list_p);
            } else {
                CN_LOG_E("Received state %d when expecting state %d.", pdc_p->pdc_ecc_state, CN_PDC_ECC_STATE_RECEIVED_SIM_LIST);
                goto error;
            }
            break;
        case CN_PDC_ECC_STATE_POST_SAT_CC:
             /* Get list with emergency numbers from the default (3gpp) list */
             e_n_list_p = cn_pdc_get_default_emerg_number_list();

             /* Compare all numbers in that list */
             result = cn_pdc_ecc_check_list(pdc_p->pdc_input_p, e_n_list_p);
             free(e_n_list_p);
             break;
        default:
            break;
    }

    if (CN_ECC_RESULT_UNKNOWN == result) {
        /* If ECC status is unknown at this point, it is no emergency call */
        result = CN_ECC_RESULT_NO_EMERGENCY_CALL;
    }

    return result;

not_relevant:
    return CN_ECC_RESULT_CHECK_NOT_RELEVANT;

error:
    return CN_ECC_RESULT_ERROR;
}

static cn_ecc_result_t cn_pdc_ecc_check_list(cn_pdc_input_t* pdc_input_p, cn_emergency_number_list_internal_t *e_n_list_p)
{
    int i = 0;

    if (NULL != e_n_list_p) {
        CN_LOG_D("%d numbers will be checked.", e_n_list_p->num_of_emergency_numbers);
        for (i = 0; i < e_n_list_p->num_of_emergency_numbers; i++) {
            if (cn_pdc_equal_numbers(pdc_input_p->service.voice_call.phone_number, e_n_list_p->emergency_number[i].emergency_number)) {
                CN_LOG_D("Numbers matched!");

                return CN_ECC_RESULT_EMERGENCY_CALL;
            }
        }
    }

    return CN_ECC_RESULT_UNKNOWN;
}

