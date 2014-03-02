/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include "uicc_internal.h"
#include "state_machine.h"
#include "sim_internal.h"
#include "state_machine.h"
#include "simd.h"
#include "uiccd.h"
#include "uicc_state_machine.h"

void uiccd_main_send_state_change_event(const ste_sim_state_t  *state, ste_uicc_status_t uicc_status)
{

  char *p = NULL;
  char *buf = NULL;
  size_t buf_len = 0;
  size_t data_len = 0;
  const char *state_name;

  if (!strcmp(state->ss_name, "init")) {
      state_name = "init";
  } else if (!strcmp(state->ss_name, "disconnect")) {
      state_name = "card_disconnected";
  } else if (!strcmp(state->ss_name, "sap")) {
      state_name = "card_disconnected";
  } else if (!strcmp(state->ss_name, "closed")) {
      switch (uicc_status) {
          case STE_UICC_STATUS_REJECTED_CARD_SIM_LOCK:
              state_name = "card_sim_locked";
              break;
          case STE_UICC_STATUS_REJECTED_CARD_CONSECUTIVE_6F00:
              state_name = "card_sim_consecutive_6f00_lock";
              break;
          case STE_UICC_STATUS_REMOVED_CARD:
          case STE_UICC_STATUS_NO_CARD:
              state_name = "no_card";
              break;
          case STE_UICC_STATUS_DISCONNECTED_CARD:
              state_name = "card_disconnected";
              break;
          case STE_UICC_STATUS_REJECTED_CARD_INVALID:
              state_name = "card_invalid";
              break;
          case STE_UICC_STATUS_CLOSED:
          default:
              state_name = "closed";
              break;
      }
  } else if (!strcmp(state->ss_name, "idle")) {
      switch (uicc_status) {
          case STE_UICC_STATUS_PIN_NEEDED:
              state_name = "pin_needed";
              break;
          case STE_UICC_STATUS_PUK_NEEDED:
              state_name = "puk_needed";
              break;
          case STE_UICC_STATUS_PIN2_NEEDED:
              state_name = "pin2_needed";
              break;
          case STE_UICC_STATUS_PUK2_NEEDED:
              state_name = "puk2_needed";
              break;
          case STE_UICC_STATUS_PIN_VERIFIED:
          case STE_UICC_STATUS_PIN2_VERIFIED:
          case STE_UICC_STATUS_READY:
          default:
              state_name = "ready";
              break;
      }
  } else {
      state_name = "unknown";
  }

  data_len = strlen(state_name) + 1;

  buf_len = sizeof(char) * (data_len);
  buf_len += sizeof(data_len);

  buf = malloc(buf_len);
  if (!buf) {
    catd_log_f(SIM_LOGGING_E, "uicc : Could not allocate memory in uiccd_main_send_state_change_event");
    return;
  }
  p = buf;

  p = sim_enc(p, &data_len, sizeof(data_len));
  p = sim_enc(p, state_name,      sizeof(char)*(data_len));

  if (buf_len != (size_t)(p-buf)) {
      catd_log_f(SIM_LOGGING_E, "uicc: uiccd_main_send_state_change_event, assert failure");
  }

  assert( buf_len == (size_t)(p-buf) );

  //Here we need to send the unsolicited event to all the connected clients.
  catd_log_f(SIM_LOGGING_I, "uicc : Sending STE_UICC_SIM_STATE_CHANGED to all clients");
  sim_send_generic_all(STE_UICC_SIM_STATE_CHANGED,
                       buf,
                       buf_len,
                       0);

  free( buf );
  return;
}
