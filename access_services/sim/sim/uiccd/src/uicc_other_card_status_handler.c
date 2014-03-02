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

/**
 * This function makes a mal call to get the current card data.
 * Quite a lot of data is needed
 * Then this data is formatted onto the file descriptor.
 */
int uiccd_main_card_status(ste_msg_t * ste_msg)
{
  uiccd_msg_card_status_t *msg = (uiccd_msg_card_status_t *) ste_msg;
  // Variables got from uiccd msg
  uintptr_t client_tag = msg->client_tag;
  int fd = msg->fd;
  // Variables for output parameters of ste_mal_card_status call
  ste_mal_card_status_data_t md;

  // For encoding to the fd
  char *pp = NULL;
  char *buf_p = NULL;
  size_t buf_len;
  // Extra local data not part of mal call follows here
  sim_uicc_status_code_t status = SIM_UICC_STATUS_CODE_FAIL;
  ste_sim_pin_status_t upin_status = STE_SIM_PIN_STATUS_UNKNOWN;
  int i;

  i = ste_mal_card_status(&md);

  if (i != 0) goto error;
  upin_status = STE_SIM_PIN_MODE_NORMAL;
  status = SIM_UICC_STATUS_CODE_OK;
  // Encode output data.
  buf_len = sizeof status;
  buf_len += sizeof md.num_apps;
  buf_len += sizeof md.card_type;
  buf_len += sizeof md.card_status;
  buf_len += sizeof upin_status;
  buf_p = malloc(buf_len);
  if (!buf_p) {
    status = SIM_UICC_STATUS_CODE_FAIL;
    goto error;
  }
  pp = buf_p;
  pp = sim_enc(pp, &status, sizeof(status));
  pp = sim_enc(pp, &md.num_apps, sizeof(md.num_apps));
  pp = sim_enc(pp, &md.card_type, sizeof(md.card_type));
  pp = sim_enc(pp, &md.card_status, sizeof(md.card_status));
  pp = sim_enc(pp, &upin_status, sizeof(upin_status));
  if (buf_len != (size_t)(pp-buf_p)) {
    status = SIM_UICC_STATUS_CODE_FAIL;
    goto error;
  }
  sim_send_generic(fd,
                   STE_UICC_RSP_CARD_STATUS,
                   buf_p,
                   buf_len,
                   client_tag);
  free(buf_p);
  return 0;
 error:
  if (buf_p) free(buf_p);
  sim_send_generic(fd,
                   STE_UICC_RSP_CARD_STATUS,
                   (char *)&status,
                   sizeof(status),
                   client_tag);
  return -1;
}
