/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "sim_internal.h"
#include "msgq.h"
#include "uiccd_msg.h"
#include "uicc_internal.h"
#include "simd.h"

// It is a bit unclear whether it is really needed to place sim status
// into a uiccd msg, and then forward that to the client. It might be possible
// for the uiccd_sig function to write to the client fd directly. But then
// there is a risk of race conditions between the uiccd server thread and
// reader thread (or client thread) when writing to fd.
// To be on the safe side, we do like this even though it might cause a delay
// while the msg is in the uiccd msg queue.
int uiccd_main_sim_status(ste_msg_t * ste_msg)
{
  uiccd_msg_sim_status_t *msg = (uiccd_msg_sim_status_t *)ste_msg;

#ifdef SIMD_SUPPORT_PIN_CACHE_FOR_MSR
  if ((msg->reason == STE_REASON_PIN_NEEDED) && uiccd_msr_get_pin_verifying_state() == 1) {
      catd_log_f(SIM_LOGGING_I, "uicc : uiccd_main_sim_status - No SIM_STATUS sent to clients in MSR pin_verifying_state");
      return 0;
  }
#endif

  catd_log_f(SIM_LOGGING_I, "uicc : Sending SIM_STATUS: %d to all clients", msg->reason);
  sim_send_generic_all(STE_UICC_SIM_STATUS,
                       (char *)&(msg->reason),
                       sizeof(msg->reason),
                       0);
  return 0;
}
