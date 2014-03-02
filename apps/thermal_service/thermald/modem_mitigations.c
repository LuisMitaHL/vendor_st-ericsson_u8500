/*
 * Thermal Service Manager
 *
 * Communicates with STE's MAD daemon to enable modem based
 * thermal mitigations.
 *
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include <stdbool.h>
#include "log.h"

#include <dbus/dbus.h>
#include <cutils/properties.h>

#include <ste_mad/mad.h>

static int init_dbus(DBusConnection **conn_pp);
static int send_dbus_signal(DBusConnection *conn_p, const char* msg_name_p,
			const char* value);
static int send_thermal_mgr_event(bool activated);

void modem_enablethermal(void) {
	INF("activating modem thermal mitigations\n");

	if (send_thermal_mgr_event(true) != 0) {
		ERR("failed to communicate with MAD\n");
	}
}

void modem_disablethermal(void) {
	INF("disabling modem thermal mitigations\n");

	if (send_thermal_mgr_event(false) != 0) {
		ERR("failed to communicate with MAD\n");
	}
}

static int init_dbus(DBusConnection **conn) {
	DBusError err;
	char dbus_address[PROPERTY_VALUE_MAX];
	int result = 0;

	dbus_error_init(&err);

	/* get STE specific DBUS port */
	memset(dbus_address, 0, sizeof(dbus_address));
	(void) property_get("ste.dbus.bus.address", dbus_address, NULL);

	/* connect to the bus */
	*conn = dbus_connection_open(dbus_address, &err);

	if (*conn == NULL || dbus_error_is_set(&err)) {
		WRN("failed to connect to DBUS: %s\n", err.message);
		result = -1;
		goto exit;
	}

	if (!dbus_bus_get_unique_name(*conn) &&
		!dbus_bus_register(*conn, &err)) {
		ERR("failed to register with DBUS: %s\n", err.message);
		goto exit;
	}

	dbus_connection_ref(*conn);

exit:
	if (*conn) {
		dbus_connection_unref(*conn);
	}

	dbus_error_free(&err);

	return result;
}


static int send_dbus_signal(DBusConnection *conn,
			const char *msg_name,
			const char *value) {
	DBusMessage *msg;
	DBusMessageIter args;
	DBusPendingCall *pending;
	int result = 0;

	/* create a signal and check for errors */
	msg = dbus_message_new_method_call(STE_MAD_DBUS_SERVICE,
					STE_MAD_DBUS_OBJECT_NAME,
					STE_MAD_DBUS_TXBO_IF_NAME,
					msg_name);

	if (msg == NULL) {
		ERR("failed to allocate memory\n");
		return -1;
	}

	/* append arguments onto signal */
	dbus_message_iter_init_append(msg, &args);
	if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &value)) {
		ERR("failed to allocate memory\n");
		result = -1;
		goto exit;
	}

	/* send the message and flush the connection */
	if (!dbus_connection_send_with_reply(conn, msg, &pending, -1)) {
		ERR("DBUS message sending failed\n");
		result = -1;
		goto exit;
	}

	/* send request */
	dbus_connection_flush(conn);
	dbus_message_unref(msg);

	/* wait for ACK from MAD */
	dbus_pending_call_block(pending);
	msg = dbus_pending_call_steal_reply(pending);
	dbus_pending_call_unref(pending);

	if (msg == NULL) {
		ERR("failed to receive MAD's ACK\n");
		goto exit;
	}

	if (dbus_message_get_type(msg) == DBUS_MESSAGE_TYPE_ERROR) {
		DBusError err;

		dbus_error_init(&err);
		dbus_set_error_from_message(&err, msg);
		ERR("MAD ACK failed: %s\n", err.message);
		dbus_error_free(&err);
		goto exit;
	}

exit:
	if (msg) {
		dbus_message_unref(msg);
	}
	return result;
}


static int send_thermal_mgr_event(bool activated) {
	DBusConnection* conn = NULL;
	int result;
	char *msg_value = activated ? STE_MAD_FEATURE_ON : STE_MAD_FEATURE_OFF;

	result = init_dbus(&conn);
	if (result < 0) {
		ERR("failed to init DBUS\n");
		return result;
	}

	result = send_dbus_signal(conn, STE_MAD_TXBO_THERMAL_MANAGER,
				msg_value);
	if (result < 0) {
		ERR("DBUS message failed to send\n");
	}

	if (conn) {
		dbus_connection_unref(conn);
	}

	return result;
}
