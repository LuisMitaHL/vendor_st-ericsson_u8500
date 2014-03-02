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

static int uiccd_encode_and_send_sim_file_get_format_response(int fd,
                                                               uintptr_t client_tag,
                                                               sim_uicc_status_code_t uicc_status_code,
                                                               sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                                               sim_uicc_status_word_t status_word,
                                                               sim_file_structure_t file_type,
                                                               int file_size,
                                                               int record_len,
                                                               int num_records)
{
    char                    *buf_p = NULL;
    char                    *p = NULL;
    size_t                   buf_len = 0;

    buf_len = sizeof(uicc_status_code);
    buf_len += sizeof(uicc_status_code_fail_details);
    buf_len += sizeof(status_word.sw1) + sizeof(status_word.sw2);
    buf_len += sizeof(file_type)+sizeof(file_size)+sizeof(record_len)+sizeof(num_records);

    buf_p = malloc(buf_len);
    if (buf_p == NULL){
        catd_log_f(SIM_LOGGING_E, "uicc : uiccd_encode_and_send_sim_file_get_format_response, memory allocation failed");
        return -1;
    }
    p = buf_p;

    p = sim_enc(p, &uicc_status_code, sizeof(uicc_status_code));
    p = sim_enc(p, &uicc_status_code_fail_details, sizeof(uicc_status_code_fail_details));
    p = sim_enc(p, &status_word.sw1, sizeof(status_word.sw1));
    p = sim_enc(p, &status_word.sw2, sizeof(status_word.sw2));
    p = sim_enc(p, &file_type, sizeof(file_type));
    p = sim_enc(p, &file_size, sizeof(file_size));
    p = sim_enc(p, &record_len, sizeof(record_len));
    p = sim_enc(p, &num_records, sizeof(num_records));

    if (buf_len != (size_t)(p-buf_p)) {
        free(buf_p);
        return -1;
    }

    sim_send_generic(fd,
                     STE_UICC_RSP_SIM_FILE_GET_FORMAT,
                     buf_p,
                     buf_len,
                     client_tag);

    free(buf_p);

    return 0;
}

int uiccd_main_sim_file_get_format(ste_msg_t * ste_msg)
{


    ste_modem_t            *m;
    uiccd_msg_sim_file_get_format_t *msg = (uiccd_msg_sim_file_get_format_t *) ste_msg;
    ste_sim_ctrl_block_t   * ctrl_p;
    sim_uicc_status_word_t status_word = {0,0};
 // Enter busy state...

    catd_log_f(SIM_LOGGING_I, "uicc : SIM FILE GET FORMAT message received from: %d ",msg->fd);


    m = catd_get_modem();
    if (!m) {
        catd_log_f(SIM_LOGGING_E, "uicc : No modem");
        uiccd_encode_and_send_sim_file_get_format_response(msg->fd,
                                                           msg->client_tag,
                                                           SIM_UICC_STATUS_CODE_FAIL,
                                                           SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                           status_word,
                                                           0, 0, 0, 0);
        return -1;
    } else {
        int                     i;
        const char             *p = msg->data;
        const char             *p_max = msg->data + msg->len;
        int file_id;
        size_t                  path_len;
        char                   *file_path = NULL;

        // split msg_data into separate parameters
        p = sim_dec(p, &file_id, sizeof(file_id), p_max);
        p = sim_dec(p, &path_len, sizeof(path_len), p_max);
        if (path_len > 0) {
          file_path = malloc(path_len);
          if (!file_path) {
            uiccd_encode_and_send_sim_file_get_format_response(msg->fd,
                                                               msg->client_tag,
                                                               SIM_UICC_STATUS_CODE_FAIL,
                                                               SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                               status_word,
                                                               0, 0, 0, 0);
            catd_log_f(SIM_LOGGING_E, "uicc : memory allocation failed");
            return -1;
          }
          p = sim_dec(p, file_path, path_len*sizeof(*file_path), p_max);
          if (!p) {
            uiccd_encode_and_send_sim_file_get_format_response(msg->fd,
                                                               msg->client_tag,
                                                               SIM_UICC_STATUS_CODE_FAIL,
                                                               SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                               status_word,
                                                               0, 0, 0, 0);
            catd_log_f(SIM_LOGGING_E, "uicc : sim_dec failed");
            free(file_path);
            return -1;
          }
        }

        ctrl_p = ste_sim_create_ctrl_block(msg->client_tag, msg->fd, msg->type, NULL, NULL);
        if (!ctrl_p) {
            uiccd_encode_and_send_sim_file_get_format_response(msg->fd,
                                                               msg->client_tag,
                                                               SIM_UICC_STATUS_CODE_FAIL,
                                                               SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                               status_word,
                                                               0, 0, 0, 0);
            free(file_path);
            return -1;
        }

        i = ste_modem_sim_file_get_format(m,
                                          (uintptr_t)ctrl_p,
                                          uicc_get_app_id(),
                                          file_id,
                                          file_path);

        if (i != 0) {
            uiccd_encode_and_send_sim_file_get_format_response(msg->fd,
                                                               msg->client_tag,
                                                               SIM_UICC_STATUS_CODE_FAIL,
                                                               SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                               status_word,
                                                               0, 0, 0, 0);
            catd_log_f(SIM_LOGGING_E, "uicc : "
                       "ste_modem_sim_file_get_format failed.");
            free(file_path);
            free(ctrl_p);
            return -1;
        }
        free(file_path);
    }

    return 0;
}

int uiccd_main_sim_file_get_format_response(ste_msg_t * ste_msg)
{
    uiccd_msg_sim_file_get_format_response_t* msg;
    char *p, *buf;
    size_t buf_len;
    ste_sim_ctrl_block_t   * ctrl_p;

    msg = (uiccd_msg_sim_file_get_format_response_t*)ste_msg;

    catd_log_f(SIM_LOGGING_I, "uicc : SIM File Get Format Response message received");

    ctrl_p = (ste_sim_ctrl_block_t*)(msg->client_tag);

    if (!ctrl_p) {
        catd_log_f(SIM_LOGGING_E, "uicc : Control Block is NULL");
        return -1;
    }
    // Since this is a kind of marshalling, it is possible to auto-generate
    // the encoding here, and corresponding decode at the clients end out of a
    // common description. Similar to IDL and stuff.
    // TODO: Try to write to the fd directly without using a malloc'ed buffer
    // intermediate storage.
    buf_len = sizeof(msg->file_type)+sizeof(msg->file_size)+
      sizeof(msg->record_len)+sizeof(msg->num_records);
    buf_len += sizeof(msg->status_word.sw1) + sizeof(msg->status_word.sw2);
    buf_len += sizeof(msg->uicc_status_code) + sizeof(msg->uicc_status_code_fail_details);

    buf = malloc(buf_len);
    if (!buf) {
      catd_log_f(SIM_LOGGING_E, "uicc : Could not allocate memory in uiccd_main_sim_file_get_format_response");
      return -1;
    }
    p = buf;

    p = sim_enc(p, &msg->uicc_status_code, sizeof(msg->uicc_status_code));
    p = sim_enc(p, &msg->uicc_status_code_fail_details, sizeof(msg->uicc_status_code_fail_details));
    p = sim_enc(p, &msg->status_word.sw1, sizeof(msg->status_word.sw1));
    p = sim_enc(p, &msg->status_word.sw2, sizeof(msg->status_word.sw2));
    p = sim_enc(p, &msg->file_type, sizeof(msg->file_type));
    p = sim_enc(p, &msg->file_size, sizeof(msg->file_size));
    p = sim_enc(p, &msg->record_len, sizeof(msg->record_len));
    p = sim_enc(p, &msg->num_records, sizeof(msg->num_records));

    if (buf_len != (size_t)(p-buf)) {
        catd_log_f(SIM_LOGGING_E, "uicc: uiccd_main_sim_file_get_format_response, assert failure");
    }

    assert( buf_len == (size_t)(p-buf) );


    sim_send_generic(ctrl_p->fd,
                     STE_UICC_RSP_SIM_FILE_GET_FORMAT,
                     buf,
                     buf_len,
                     ctrl_p->client_tag);

    free(ctrl_p);
    free( buf );
    return 0;
}
