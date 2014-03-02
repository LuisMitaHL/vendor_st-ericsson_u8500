/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include "sim_internal.h"
#include "catd_modem_types.h"
#include "uicc_internal.h"
#include "simd.h"
#include "uiccd_msg.h"
#include "uicc_file_smsc_common.h"
#include "uicc_state_machine.h"

//FIXME: Remove handle of data and data_len, not used in this response.
static int uiccd_encode_and_send_update_sim_file_binary_response(int fd,
                                                                 uintptr_t client_tag,
                                                                 sim_uicc_status_code_t uicc_status_code,
                                                                 sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                                                 sim_uicc_status_word_t status_word,
                                                                 const uint8_t  *data,
                                                                 size_t  data_len)
{
    char                    *buf_p = NULL;
    char                    *p = NULL;
    size_t                   buf_len = 0;

    buf_len = sizeof(uicc_status_code);
    buf_len += sizeof(uicc_status_code_fail_details);
    buf_len += sizeof(status_word.sw1) + sizeof(status_word.sw2);
    buf_len += sizeof(uint8_t)*data_len;
    buf_len += sizeof(data_len);

    buf_p = malloc(buf_len);
    if (buf_p == NULL){
        catd_log_f(SIM_LOGGING_E, "uicc : uiccd_encode_and_send_update_sim_file_binary_response, memory allocation failed");
        return -1;
    }
    p = buf_p;

    p = sim_enc(p, &uicc_status_code, sizeof(uicc_status_code));
    p = sim_enc(p, &uicc_status_code_fail_details, sizeof(uicc_status_code_fail_details));
    p = sim_enc(p, &status_word.sw1, sizeof(status_word.sw1));
    p = sim_enc(p, &status_word.sw2, sizeof(status_word.sw2));
    p = sim_enc(p, &data_len, sizeof(data_len));
    p = sim_enc(p, data, sizeof(uint8_t)*(data_len));

    if (buf_len != (size_t)(p-buf_p)) {
        free(buf_p);
        return -1;
    }

    sim_send_generic(fd,
                     STE_UICC_RSP_UPDATE_SIM_FILE_BINARY,
                     buf_p,
                     buf_len,
                     client_tag);

    free(buf_p);

    return 0;
}

int uiccd_main_update_sim_file_binary(ste_msg_t * ste_msg)
{
    ste_modem_t                    *m;
    uiccd_msg_update_sim_file_binary_t   *msg;
    ste_sim_ctrl_block_t   * ctrl_p;
    sim_uicc_status_word_t status_word = {0,0};

    msg = (uiccd_msg_update_sim_file_binary_t *) ste_msg;

    catd_log_f(SIM_LOGGING_I, "uicc : UPDATE EF binary message received");

    m = catd_get_modem();
    if (!m) {
        catd_log_f(SIM_LOGGING_E, "uicc : No modem");
        uiccd_encode_and_send_update_sim_file_binary_response(msg->fd,
                                                              msg->client_tag,
                                                              SIM_UICC_STATUS_CODE_FAIL,
                                                              SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                              status_word,
                                                              NULL,
                                                              0);
    } else {
        int i;
        const char *p = msg->data;
        const char *p_max = msg->data + msg->len;
        int file_id, offset, length;
        char *file_path = NULL;
        uint8_t *data;

        size_t path_len;

        p = sim_dec(p, &file_id, sizeof(file_id), p_max);
        p = sim_dec(p, &offset, sizeof(offset), p_max);
        p = sim_dec(p, &length, sizeof(length), p_max);

        if (length <= 0) {
            uiccd_encode_and_send_update_sim_file_binary_response(msg->fd,
                                                                  msg->client_tag,
                                                                  SIM_UICC_STATUS_CODE_FAIL,
                                                                  SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                                  status_word,
                                                                  NULL,
                                                                  0);
            catd_log_f(SIM_LOGGING_E, "uicc : uiccd_sig_update_sim_file_binary failed");
            return -1;
        }

        data = malloc(length);
        if (!data) {
            uiccd_encode_and_send_update_sim_file_binary_response(msg->fd,
                                                                  msg->client_tag,
                                                                  SIM_UICC_STATUS_CODE_FAIL,
                                                                  SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                                  status_word,
                                                                  NULL,
                                                                  0);
            catd_log_f(SIM_LOGGING_E, "uicc : uiccd_main_update_sim_file_binary, memory allocation failed");
            return -1;
        }
        p = sim_dec(p, data, length*sizeof(*data), p_max);
        p = sim_dec(p, &path_len, sizeof(path_len), p_max);

        if (path_len > 0) {
          file_path = malloc(path_len + 1);
          if (!file_path) {
            free(data);
            uiccd_encode_and_send_update_sim_file_binary_response(msg->fd,
                                                                  msg->client_tag,
                                                                  SIM_UICC_STATUS_CODE_FAIL,
                                                                  SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                                  status_word,
                                                                  NULL,
                                                                  0);
            catd_log_f(SIM_LOGGING_E, "uicc : memory allocation failed");
            return -1;
          }
          p = sim_dec(p, file_path, path_len*sizeof(*file_path), p_max);
          if (!p || file_path[path_len-1] != '\0') {
            uiccd_encode_and_send_update_sim_file_binary_response(msg->fd,
                                                                  msg->client_tag,
                                                                  SIM_UICC_STATUS_CODE_FAIL,
                                                                  SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                                  status_word,
                                                                  NULL,
                                                                  0);
            catd_log_f(SIM_LOGGING_E, "uicc : sim_dec failed");
            free(data);
            free(file_path);
            return -1;
          }
        }
        else
        {
            path_len = 1;
            file_path = malloc(path_len + 1);
            if (!file_path) {
              free(data);
              uiccd_encode_and_send_update_sim_file_binary_response(msg->fd,
                                                                    msg->client_tag,
                                                                    SIM_UICC_STATUS_CODE_FAIL,
                                                                    SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                                    status_word,
                                                                    NULL,
                                                                    0);
              catd_log_f(SIM_LOGGING_E, "uicc : memory allocation failed");
              return -1;
            }
            file_path[0] = '\0';
        }

        ctrl_p = ste_sim_create_ctrl_block(msg->client_tag, msg->fd, msg->type, NULL, NULL);
        if (!ctrl_p) {
            uiccd_encode_and_send_update_sim_file_binary_response(msg->fd,
                                                                  msg->client_tag,
                                                                  SIM_UICC_STATUS_CODE_FAIL,
                                                                  SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                                  status_word,
                                                                  NULL,
                                                                  0);
            free(file_path);
            free(data);
            return -1;
        }

        i = ste_modem_file_update_binary(m,
                                         (uintptr_t)ctrl_p,
                                         uicc_get_app_id(),
                                         file_id,
                                         offset,
                                         length,
                                         file_path,
                                         data);

        if (i != 0) {
            uiccd_encode_and_send_update_sim_file_binary_response(msg->fd,
                                                                  msg->client_tag,
                                                                  SIM_UICC_STATUS_CODE_FAIL,
                                                                  SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                                  status_word,
                                                                  NULL,
                                                                  0);
            catd_log_f(SIM_LOGGING_E, "uicc : uiccd_sig_update_sim_file_binary failed");
            free(data);
            free(file_path);
            free(ctrl_p);
            return -1;
        }
        free(data);
        free(file_path);
    }

    return 0;
}

//FIXME: Remove handle of data and data_len, not used in this response.
int uiccd_main_update_sim_file_binary_response(ste_msg_t * ste_msg)
{
    uiccd_msg_update_sim_file_binary_response_t* msg;
    char                    *p = NULL;
    char                    *buf = NULL;
    size_t                  buf_len = 0;
    unsigned                data_len = 0;

    ste_sim_ctrl_block_t   * ctrl_p;

    msg = (uiccd_msg_update_sim_file_binary_response_t*)ste_msg;

    catd_log_f(SIM_LOGGING_I, "uicc : EF binary update response message received");

    ctrl_p = (ste_sim_ctrl_block_t*)(msg->client_tag);

    if (!ctrl_p) {
        catd_log_f(SIM_LOGGING_E, "uicc : Control Block is NULL");
        return -1;
    }

    data_len = msg->len;

    buf_len = sizeof(char)*data_len;
    buf_len += sizeof(data_len);
    buf_len += sizeof(msg->status_word.sw1) + sizeof(msg->status_word.sw2);
    buf_len += sizeof(msg->uicc_status_code) + sizeof(msg->uicc_status_code_fail_details);

    buf = malloc(buf_len);
    if (buf == NULL){
        catd_log_f(SIM_LOGGING_E, "uicc : EF binary update response, memory allocatin failed");
        return -1;
    }
    p = buf;

    p = sim_enc(p, &msg->uicc_status_code, sizeof(sim_uicc_status_code_t));
    p = sim_enc(p, &msg->uicc_status_code_fail_details, sizeof(sim_uicc_status_code_fail_details_t));
    p = sim_enc(p, &msg->status_word.sw1, sizeof(msg->status_word.sw1));
    p = sim_enc(p, &msg->status_word.sw2, sizeof(msg->status_word.sw2));
    p = sim_enc(p, &data_len, sizeof(data_len));
    p = sim_enc(p, msg->data,      sizeof(char)*(data_len));

    if (buf_len != (size_t)(p-buf)) {
        catd_log_f(SIM_LOGGING_E, "uicc: uiccd_main_update_sim_file_binary_response, assert failure");
    }

    assert( buf_len == (size_t)(p-buf) );

    sim_send_generic(ctrl_p->fd,
                     STE_UICC_RSP_UPDATE_SIM_FILE_BINARY,
                     buf,
                     buf_len,
                     ctrl_p->client_tag);


    free( buf );
    free(ctrl_p);
    return 0;
}
