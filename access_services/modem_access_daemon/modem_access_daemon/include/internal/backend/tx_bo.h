/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef TX_BO_H
#define TX_BO_H

#include <stdbool.h>

#include "mad_dbus.h"

typedef struct {
    bool thermal_mgr;
    bool wifi_soft_ap;
} mad_txbo_trigger_states_t;

bool txbo_dbus_msg_handler(DBusConnection *conn, DBusMessage *msg);

void txbo_get_trigger_states(mad_txbo_trigger_states_t *states);

#endif        //  #ifndef TX_BO_H

