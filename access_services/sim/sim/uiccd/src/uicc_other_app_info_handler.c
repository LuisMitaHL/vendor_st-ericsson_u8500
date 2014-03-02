/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdio.h>
#include "msgq.h"
#include "sim_internal.h"
#include "uicc_internal.h"
#include "uiccd_msg.h"
#include "catd_modem_types.h"
#include "uiccd.h"
#include "uicc_state_machine.h"

// -----------------------------------------------------------------------------
// PIN Data

int uiccd_main_app_info(const ste_msg_t * ste_msg)
{
  uiccd_msg_app_info_t *msg = (uiccd_msg_app_info_t *) ste_msg;
  ste_sim_app_type_t    app_type = uicc_get_app_type();

  sim_send_generic(msg->fd,
                   STE_UICC_RSP_APP_INFO,
                   (char *)&app_type,
                   sizeof(app_type),
                   msg->client_tag);

  return 0;
}
