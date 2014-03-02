/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <dbus/dbus.h>

#include "mad/mad.h"
#include "mad_log.h"
#include "backend/tx_bo.h"
#include "backend/at_handler.h"


static mad_txbo_trigger_states_t s_states = {false, false};

static void update_state(bool *state, bool value);

bool txbo_dbus_msg_handler(DBusConnection *conn, DBusMessage *msg) {
    DBusMessage *msg_reply;
    DBusMessageIter args;
    bool *state = NULL;
    char *state_string = NULL;

    if (!msg) {
        MAD_LOG_E("msg was null");
        return false;
    }

    if (dbus_message_is_method_call(msg, STE_MAD_DBUS_TXBO_IF_NAME, STE_MAD_TXBO_WIFI_AP)) {
        state = &s_states.wifi_soft_ap;
        MAD_LOG_D("Wifi Soft AP state received");
    } else if (dbus_message_is_method_call(msg, STE_MAD_DBUS_TXBO_IF_NAME,
                                            STE_MAD_TXBO_THERMAL_MANAGER)) {
        state = &s_states.thermal_mgr;
        MAD_LOG_D("Thermal Manager state received");
    } else {
        MAD_LOG_W("Unknown message received on if: %s - msg: %s", STE_MAD_DBUS_TXBO_IF_NAME,
                                                                    dbus_message_get_member(msg));
        goto not_handled;
    }

    if (!dbus_message_iter_init(msg, &args)) {
        MAD_LOG_W("Message has no arguments!");
        goto not_handled;
    }

    if (DBUS_TYPE_STRING != dbus_message_iter_get_arg_type(&args)) {
        MAD_LOG_W("Argument is not string!");
        goto not_handled;
    }

    dbus_message_iter_get_basic(&args, &state_string);
    if (!strncmp(state_string, STE_MAD_FEATURE_ON, strlen(STE_MAD_FEATURE_ON))) {
        MAD_LOG_D("State is: %s", STE_MAD_FEATURE_ON);
        update_state(state, true);
    } else if (!strncmp(state_string, STE_MAD_FEATURE_OFF, strlen(STE_MAD_FEATURE_OFF))) {
        MAD_LOG_D("State is: %s", STE_MAD_FEATURE_OFF);
        update_state(state, false);
    } else {
        MAD_LOG_W("Message contained unknown state value:");
        goto not_handled;
    }

    /* Send an empty reply back */
    msg_reply = dbus_message_new_method_return(msg);

    if (!msg_reply) {
       MAD_LOG_E("Failed to create a reply message");
       goto not_handled;
    }

    if (!dbus_connection_send(conn, msg_reply, NULL)) {
        MAD_LOG_E("Unable to reply to received msg");
    }
    dbus_message_unref(msg_reply);

    return true;

not_handled:
    return false;
}

void txbo_get_trigger_states(mad_txbo_trigger_states_t *states) {
    if (!states) {
        MAD_LOG_E("states was NULL!");
        return;
    }

    memcpy(states, &s_states, sizeof(mad_txbo_trigger_states_t));
}


static void update_state(bool *state, bool value) {
    mad_epwrred_event_values_t event = NO_EVENT;
    bool *thermal_state = &s_states.thermal_mgr;
    bool *wifi_ap_state = &s_states.wifi_soft_ap;

    if (*state == value) {
        return;  /* unchanged */
    }

    *state = value;

    if (thermal_state == state) {
        event = value ? THERMAL_MANAGEMENT_ACTIVE : THERMAL_MANAGEMENT_INACTIVE;
    } else if (wifi_ap_state == state) {
        event = value ? WLAN_HOTSPOT_ACTIVE : WLAN_HOTSPOT_INACTIVE;
    } else {
        MAD_LOG_E("Got unknown state");
    }

    if (event != NO_EVENT) {
        if (mad_at_handler_send_txbo(event) < 0) {
            MAD_LOG_E("Failed to send Tx Backoff command");
        }
    } else {
        MAD_LOG_D("State was unchanged!");
    }

    return;
}
