/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "msgq.h"
#include "uiccd_msg.h"
#include "sim_internal.h"
#include "uicc_internal.h"

int uiccd_main_app_status(const ste_msg_t * ste_msg)
{
  uiccd_msg_app_status_t *msg = (uiccd_msg_app_status_t *) ste_msg;

  // Input from uiccd msg
  const char *p = msg->data, *p_max = msg->data + msg->len;
  int fd = msg->fd;
  uintptr_t client_tag = msg->client_tag;
  int app_index;
  // Output from mal
  ste_mal_app_status_data_t md;
  // For encoding
  char *pp = NULL, *buf_p = NULL;
  size_t buf_len;
  // Extra output data not part of mal call
  sim_uicc_status_code_t status = SIM_UICC_STATUS_CODE_FAIL; // For goto.
  ste_sim_app_mark_t mark;
  int i;
  // Decode input data
  catd_log_f(SIM_LOGGING_I, "uiccd_main_app_status: Enter");
  p = sim_dec(p, &app_index, sizeof(app_index), p_max);
  if (!p) goto error;
  // Call mal method
  i = ste_mal_app_status(app_index, &md);
  if (i != 0) goto error;
  status = SIM_UICC_STATUS_CODE_OK;
  // Encode output data.
  buf_len = sizeof(status) + sizeof(md.app_type) + sizeof(md.app_status);
  buf_len += sizeof(md.aid_len) + md.aid_len;
  buf_len += sizeof(md.label_len) + sizeof(char)*(md.label_len+1);
  buf_len += sizeof(mark) + sizeof(md.pin_mode);
  buf_len += sizeof(md.pin_state) + sizeof(md.pin2_state);
  buf_p = malloc(buf_len);
  if (!buf_p) {
    status = SIM_UICC_STATUS_CODE_FAIL;
    goto error;
  }
  pp = buf_p;
  pp = sim_enc(pp, &status, sizeof(status));
  pp = sim_enc(pp, &md.app_type, sizeof(md.app_type));
  pp = sim_enc(pp, &md.app_status, sizeof(md.app_status));
  pp = sim_enc(pp, &md.aid_len, sizeof(md.aid_len));
  pp = sim_enc(pp, md.aid, md.aid_len);
  pp = sim_enc(pp, &md.label_len, sizeof(md.label_len));
  pp = sim_enc(pp, md.label, md.label_len+1);
  // Only little piece of logic needed; mal only knows if app was selected
  if (md.activated_app)
    mark = STE_SIM_APP_TELEKOM_GSM_UMTS_MARKED;
  else
    mark = STE_SIM_APP_TELEKOM_UNMARKED;
  pp = sim_enc(pp, &mark, sizeof(mark));
  pp = sim_enc(pp, &md.pin_mode, sizeof(md.pin_mode));
  pp = sim_enc(pp, &md.pin_state, sizeof(md.pin_state));
  pp = sim_enc(pp, &md.pin2_state, sizeof(md.pin2_state));
  if (buf_len != (size_t)(pp-buf_p)) {
    status = SIM_UICC_STATUS_CODE_FAIL;
    goto error;
  }
  sim_send_generic(fd,
                   STE_UICC_RSP_APP_STATUS,
                   buf_p,
                   buf_len,
                   client_tag);
  free(buf_p);
  catd_log_f(SIM_LOGGING_I, "uiccd_main_app_status: Exit");
  return 0;
 error:
  // Set status right before getting here.
  if (buf_p) free(buf_p);
  sim_send_generic(fd,
                   STE_UICC_RSP_APP_STATUS,
                   (char *)&status,
                   sizeof status,
                   client_tag);
  return -1;
}
