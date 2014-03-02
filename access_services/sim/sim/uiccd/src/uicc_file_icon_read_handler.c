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
#include "uicc_state_machine.h"

// This info needs to be kept in order to generate the right response to client.
// The zero length cases are there to have us prepared if specs should be
// clarified in a new direction

typedef enum {
  ICON_READ_CASE_EF_IMG,
  ICON_READ_CASE_EF_IMG_ZERO_LENGTH,
  ICON_READ_CASE_EF_IIDF,
  ICON_READ_CASE_EF_IIDF_ZERO_LENGTH
} icon_read_case_t;

typedef struct {
  icon_read_case_t ir_case;
} trans_icon_read_data_t;

static void uiccd_main_sim_icon_read_send_response(int fd, uintptr_t client_tag,
         sim_uicc_status_code_t uicc_status_code,
         sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
         sim_uicc_status_word_t status_word,
         uint8_t *data_p,
         size_t length)
{
  char *p = NULL, *buf_p = NULL;
  size_t buf_len;
  unsigned data_len;

  data_len = length;

  buf_len = sizeof(char) * data_len;
  buf_len += sizeof(data_len);
  buf_len += sizeof(uicc_status_code) + sizeof(uicc_status_code_fail_details);
  buf_len += sizeof(status_word.sw1) + sizeof(status_word.sw2);

  buf_p = malloc(buf_len);
  if (!buf_p) {
    catd_log_f(SIM_LOGGING_E, "uicc : memory allocation failed");
    goto error;
  }
  p = buf_p;

  p = sim_enc(p, &uicc_status_code, sizeof(uicc_status_code));
  p = sim_enc(p, &uicc_status_code_fail_details, sizeof(uicc_status_code_fail_details));
  p = sim_enc(p, &status_word.sw1, sizeof(status_word.sw1));
  p = sim_enc(p, &status_word.sw2, sizeof(status_word.sw2));

  p = sim_enc(p, &data_len, sizeof(data_len));
  p = sim_enc(p, data_p, sizeof(char)*(data_len));

  if (buf_len != (size_t)(p - buf_p)) {
    catd_log_f(SIM_LOGGING_E, "uicc: icon_read_send_response assert failure");
  }

  assert( buf_len == (size_t)(p - buf_p) );

  sim_send_generic(fd, STE_UICC_RSP_SIM_ICON_READ,
                   buf_p, buf_len, client_tag);

  free(buf_p);
  return;

  // Common cleanup in case of error
 error:
  sim_send_generic(fd, STE_UICC_RSP_SIM_ICON_READ,
                   NULL, 0, client_tag);
  return;
}

static int uiccd_main_sim_icon_read_response(ste_msg_t *ste_msg)
{
  trans_icon_read_data_t *tdata_p;
  ste_sim_ctrl_block_t   *ctrl_p;

  // Initialize for internal failure so we can easily spot it.
  uint8_t                            *data = NULL;
  size_t                              len = 0;
  sim_uicc_status_code_t              uicc_status_code = SIM_UICC_STATUS_CODE_FAIL;
  sim_uicc_status_code_fail_details_t uicc_status_code_fail_details = SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR;
  sim_uicc_status_word_t              status_word = {0, 0};

  catd_log_f(SIM_LOGGING_I, "uicc : sim icon read response called");

  ctrl_p = (ste_sim_ctrl_block_t *)(ste_msg->client_tag);
  if (!ctrl_p) {  //this will never happen
    catd_log_f(SIM_LOGGING_E, "uicc : Control Block is NULL");
    return 0;  //return 0 because anyway this transaction is done
  }
  // We need to act on file id and file length
  tdata_p = (trans_icon_read_data_t *)(ctrl_p->transaction_data_p);
  if (!tdata_p) {  //this will never happen
    catd_log_f(SIM_LOGGING_E, "uicc : tdata_p is NULL");
    return 0;  //return 0 because anyway this transaction is done
  }
  switch (tdata_p->ir_case) {
  case ICON_READ_CASE_EF_IMG_ZERO_LENGTH:
    // Fall through.
  case ICON_READ_CASE_EF_IMG: {
    catd_log_f(SIM_LOGGING_D, "ICON READ: read ef img record ir-case %d",
               tdata_p->ir_case);
    uiccd_msg_read_sim_file_record_response_t *rr_msg = (uiccd_msg_read_sim_file_record_response_t*)ste_msg;
    uicc_status_code = rr_msg->uicc_status_code;
    uicc_status_code_fail_details = rr_msg->uicc_status_code_fail_details;
    status_word = rr_msg->status_word;
    data = rr_msg->data;
    len = rr_msg->len;
// EF IMG file read
    break;
  }
  case ICON_READ_CASE_EF_IIDF_ZERO_LENGTH:
    // Fall through.
  case ICON_READ_CASE_EF_IIDF: {
    catd_log_f(SIM_LOGGING_D, "ICON READ: read iidf binary ir-case %d\n",
               tdata_p->ir_case);
    uiccd_msg_read_sim_file_binary_response_t *rb_msg = (uiccd_msg_read_sim_file_binary_response_t*)ste_msg;
    // IIDF file read
    uicc_status_code = rb_msg->uicc_status_code;
    uicc_status_code_fail_details = rb_msg->uicc_status_code_fail_details;
    status_word = rb_msg->status_word;
    data = rb_msg->data;
    len = rb_msg->len;
    break;
  }
  default:
    break;
  }
  uiccd_main_sim_icon_read_send_response(ctrl_p->fd, ctrl_p->client_tag,
                                         uicc_status_code,
                                         uicc_status_code_fail_details,
                                         status_word,
                                         data, len);
  free(tdata_p);
  free(ctrl_p);

  return 0;
}

int uiccd_main_sim_icon_read(ste_msg_t *ste_msg)
{

    ste_modem_t            *m;
    uiccd_msg_sim_icon_read_t *msg = (uiccd_msg_sim_icon_read_t *)ste_msg;
    ste_sim_ctrl_block_t   *ctrl_p = NULL;
    trans_icon_read_data_t *tdata_p = NULL;
    sim_uicc_status_word_t status_word = {0,0};
    sim_uicc_status_code_fail_details_t fail_details = SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR;

    catd_log_f(SIM_LOGGING_I, "uicc : ICON READ binary message received from: %d ",msg->fd);
    m = catd_get_modem();
    if (!m) {
      catd_log_f(SIM_LOGGING_E, "uicc : No modem");
      goto error;
    }
    {
      int i;
      const char *p = msg->data, *p_max = msg->data + msg->len;
      int file_id, record_or_offset, length;
      // Have to supply path since 0x4F20 is also used for ef kc
      const char *icon_path = "3F007F105F50";

      // split msg_data into separate parameters
      p = sim_dec(p, &file_id, sizeof(file_id), p_max);
      p = sim_dec(p, &record_or_offset, sizeof(record_or_offset), p_max);
      p = sim_dec(p, &length, sizeof(length), p_max);
      if (!p)
        goto error;
      tdata_p = malloc(sizeof(*tdata_p));
      if (!tdata_p)
        goto error;
      ctrl_p = ste_sim_create_ctrl_block(msg->client_tag, msg->fd, msg->type,
                                         uiccd_main_sim_icon_read_response,
                                         tdata_p);
      if (!ctrl_p)
        goto error;
      catd_log_f(SIM_LOGGING_D, "ICON READ: file_id=%x, index=%d, length=%d\n", file_id, record_or_offset, length);
      // Logic of the command itself follows here
      // but this fn and others could be called from elsewhere
      // MAL or modem will check more parameters for validity
      if (file_id == 0x4F20) {
        tdata_p->ir_case = ICON_READ_CASE_EF_IMG;
        if (length < 0) {
          fail_details = SIM_UICC_STATUS_CODE_FAIL_DETAILS_INVALID_PARAMETERS;
          goto error;
        } else if (length == 0) {
          tdata_p->ir_case = ICON_READ_CASE_EF_IMG_ZERO_LENGTH;
        } else if (length > 255)
          length = 255;
        catd_log_f(SIM_LOGGING_D, "ICON READ: reading ef img record\n");

        i = ste_modem_file_read_record(m, (uintptr_t)ctrl_p, uicc_get_app_id(),
                                       file_id, record_or_offset, length,
                                       icon_path);
        if (i != 0) {
          catd_log_f(SIM_LOGGING_E, "uicc : ICON READ EF IMG failed.");
          goto error;
        }
      } else if ((file_id >= 0x4F00 && file_id <= 0x4F0F) || //No spec support
                 (file_id >= 0x4F21 && file_id <= 0x4FFF)) {
        // Reads binary ef iidf files. Spec says these conditions hold.
        tdata_p->ir_case = ICON_READ_CASE_EF_IIDF;
        if (length < 0) {
          fail_details = SIM_UICC_STATUS_CODE_FAIL_DETAILS_INVALID_PARAMETERS;
          goto error;
        } else if (length == 0) {
          tdata_p->ir_case = ICON_READ_CASE_EF_IIDF_ZERO_LENGTH;
          length = 256;
        } else if (length > 256)
          length = 256;

        i = ste_modem_file_read_binary(m, (uintptr_t)ctrl_p, uicc_get_app_id(),
                                       file_id, record_or_offset, length,
                                       icon_path);
        if (i != 0) {
          catd_log_f(SIM_LOGGING_E, "uicc : ICON READ EF IMG failed.");
          goto error;
        }
      } else {
        fail_details = SIM_UICC_STATUS_CODE_FAIL_DETAILS_INVALID_PARAMETERS;
        catd_log_f(SIM_LOGGING_E, "uicc : ICON READ bad file id.");
        goto error;
      }
    }
    // Point of successful return
    return 0;
    // Code below is cleanup only if error
 error:
    uiccd_main_sim_icon_read_send_response(msg->fd, msg->client_tag,
                                     SIM_UICC_STATUS_CODE_FAIL,
                                     fail_details,
                                     status_word,
                                     NULL,
                                     0);
    if (ctrl_p) free(ctrl_p);
    if (tdata_p) free(tdata_p);
    return -1;
}
