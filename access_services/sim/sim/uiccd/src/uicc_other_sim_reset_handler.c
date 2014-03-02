/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdio.h>
#include <stdint.h>
#include "sim_internal.h"
#include "catd_modem_types.h"
#include "uicc_internal.h"
#include "simd.h"
#include "uiccd_msg.h"
#include "uicc_file_smsc_common.h"
#include <assert.h>

static int uiccd_reset_send_response( int fd,
                                      uintptr_t client_tag,
                                      sim_uicc_status_code_t uicc_status_code,
                                      sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                      sim_uicc_status_word_t status_word)
{
    char                    *buf_p = NULL;
    char                    *p = NULL;
    size_t                   buf_len = 0;

    buf_len = sizeof(uicc_status_code);
    buf_len += sizeof(uicc_status_code_fail_details);
    buf_len += sizeof(status_word.sw1) + sizeof(status_word.sw2);

    buf_p = malloc(buf_len);

    if (buf_p == NULL){
        catd_log_f(SIM_LOGGING_E, "uicc : uiccd_reset_send_response, memory allocation failed");
        return -1;
    }

    p = buf_p;

    p = sim_enc(p, &uicc_status_code, sizeof(uicc_status_code));
    p = sim_enc(p, &uicc_status_code_fail_details, sizeof(uicc_status_code_fail_details));
    p = sim_enc(p, &status_word.sw1, sizeof(status_word.sw1));
    p = sim_enc(p, &status_word.sw2, sizeof(status_word.sw2));

    if (buf_len != (size_t)(p-buf_p)) {
        free(buf_p);
        return -1;
    }

    sim_send_generic(fd,
                     STE_UICC_RSP_RESET,
                     buf_p,
                     buf_len,
                     client_tag);

    free(buf_p);

    return 0;
}

int uiccd_main_sim_reset(const ste_msg_t *msg)
{
    ste_modem_t *m;
    sim_uicc_status_word_t status_word = {0,0};
    sim_uicc_status_code_t status_code = SIM_UICC_STATUS_CODE_OK;
    sim_uicc_status_code_fail_details_t fail_details = SIM_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;

    m = catd_get_modem();

    if (!m) {
        catd_log_f(SIM_LOGGING_E, "uicc : No modem");
        status_code = SIM_UICC_STATUS_CODE_FAIL;
        fail_details = SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR;
        return -1;
    } else {
        ste_modem_reset_backend_state(m, 0); // Reset the backend state
    }

    if (uiccd_reset_send_response(((uiccd_msg_fd_t *)msg)->fd,
                                  msg->client_tag,
                                  status_code,
                                  fail_details,
                                  status_word) != 0)
    {
        catd_log_f(SIM_LOGGING_E, "uiccd_main_sim_reset: uiccd_reset_send_response failed");
    }

    return 0;
}
