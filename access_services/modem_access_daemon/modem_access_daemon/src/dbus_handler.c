/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <dbus/dbus.h>

#ifdef ANDROID
#include <cutils/properties.h>
#endif

#include <util_mainloop.h>

#include "mad/mad.h"
#include "mad_dbus.h"
#include "mad_log.h"


static DBusConnection *s_conn = NULL;


static dbus_bool_t add_watch(DBusWatch *watch, void *data);
static void remove_watch(DBusWatch *watch, void *data);
static void toggle_watch(DBusWatch *watch, void *data);

static int mad_dbus_writable_event(const int fd, const void *data);
static int mad_dbus_readable_event(const int fd, const void *data);
static int mad_dbus_event(DBusWatch *watch, const unsigned int flags);

static DBusHandlerResult mad_dbus_incoming_msg_handler(DBusConnection *conn, DBusMessage *msg, void *data);

#define MAD_DEFAULT_DBUS_ADDRESS "unix:path=/dev/socket/dbus_ste"

int mad_dbus_handler_init() {
    int res;
    DBusError err;
    DBusConnection *conn;
    unsigned int index;

    /* Initialize the dbus error return value */
    dbus_error_init(&err);

    /* Connect to system dbus */
#ifdef ANDROID
        {
            char dbus_address[PROPERTY_VALUE_MAX];
            int len = 0;
            memset(dbus_address, 0, sizeof(dbus_address));
            len = property_get("ste.dbus.bus.address", dbus_address, NULL);
            if (!len) {
                strcpy(dbus_address, MAD_DEFAULT_DBUS_ADDRESS);
                MAD_LOG_W("Android property ste.dbus.bus.address missing using default");
            }
            conn = dbus_connection_open(dbus_address, &err);
            if (conn && !dbus_error_is_set(&err)
                        && !dbus_bus_register(conn, &err)) {
                MAD_LOG_E("Failed to register with DBus");
            }

        }
#else
        conn = dbus_bus_get(DBUS_BUS_SESSION, &err);
        MAD_LOG_D("got dbus connection: %x", conn);
#endif /* ANDROID */

    if (!conn || dbus_error_is_set(&err)) {
        MAD_LOG_E("connect error %s: %s", err.name, err.message);
        goto error;
    }

    /* Declare MID service */
    res = dbus_bus_request_name(conn, STE_MAD_DBUS_SERVICE,
                                    DBUS_NAME_FLAG_REPLACE_EXISTING, &err);
    if (dbus_error_is_set(&err)) {
        MAD_LOG_E("request name error %s: %s", err.name, err.message);
        goto error;
    }
    if (res != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER) {
        MAD_LOG_E("failed to take primary ownership of connection");
        goto error;
    }

    if (!dbus_connection_set_watch_functions(conn, add_watch, remove_watch, toggle_watch, NULL, NULL)) {
        MAD_LOG_E("failed to add watches");
        goto error;
    }

    /* Adds a match rules to match messages going through the message bus */
    for (index = 0; index < mad_dbus_msg_handlers_size; index++) {
        const char *match_rule = mad_dbus_msg_handlers[index].match_rule;

        dbus_bus_add_match(conn, match_rule, &err);

        if (dbus_error_is_set(&err)) {
            MAD_LOG_E("failed to add match rule %s: %s", err.name, err.message);
            goto error;
        }
    }

    /* Add a message filter to process incoming messages */
    if (!dbus_connection_add_filter(conn,
                (DBusHandleMessageFunction)mad_dbus_incoming_msg_handler, NULL, NULL)) {
        MAD_LOG_E("failed to add signal filter");
        goto error;
    };

    dbus_error_free(&err);

    /* Increment the reference count on the DBusConnection */
    s_conn = conn;

    MAD_LOG_D("DBus init ok!");
    return 0;

error:
    dbus_error_free(&err);
    return -1;
}

void mad_dbus_handler_shutdown() {
    DBusError err;
    unsigned int index;

    if (s_conn == NULL)
        goto exit;

    /* Initialize the dbus error return value */
    dbus_error_init(&err);

    /* Disconnection clean up */
    for (index = 0; index < mad_dbus_msg_handlers_size; index++) {
        const char *match_rule = mad_dbus_msg_handlers[index].match_rule;

        dbus_bus_remove_match(s_conn, match_rule, &err);

        if (dbus_error_is_set(&err)) {
            MAD_LOG_E("failed to add match rule %s: %s", err.name, err.message);
        }
    }

    dbus_connection_remove_filter(s_conn,
        (DBusHandleMessageFunction)mad_dbus_incoming_msg_handler, NULL);

    /* Flush connection buffer */
    dbus_connection_flush(s_conn);

    /* Deregister from the bus */
    dbus_bus_release_name(s_conn, STE_MAD_DBUS_SERVICE, &err);
    if (dbus_error_is_set(&err)) {
        MAD_LOG_E("release error %s: %s", err.name, err.message);
    }

    dbus_connection_unref(s_conn);
    dbus_error_free(&err);
    dbus_shutdown();
exit:
    MAD_LOG_D("DBus disconnected and shutdown");
}


static dbus_bool_t add_watch(DBusWatch *watch, void *data) {
    int fd;
    dbus_bool_t res;
    unsigned int flags;

#ifdef ANDROID
    fd = dbus_watch_get_fd(watch);
#else
    fd = dbus_watch_get_unix_fd(watch);
#endif

    MAD_LOG_D("Got add_watch for fd: %d" + fd);

    res = dbus_watch_get_enabled(watch);
    if (res) {
        flags = dbus_watch_get_flags(watch);

        if (flags & DBUS_WATCH_READABLE) {
            if (util_mainloop_add_watch(EVENT_READ, fd, watch, mad_dbus_readable_event, NULL) < 0) {
                MAD_LOG_E("Adding watch to read list failed!");
                goto error;
            }
        }

        if (flags & DBUS_WATCH_WRITABLE) {
            if (util_mainloop_add_watch(EVENT_WRITE, fd, watch, mad_dbus_writable_event, NULL) < 0) {
                MAD_LOG_E("Adding watch to write list failed!");
                goto error;
            }
        }

    } else {
        MAD_LOG_D("Watch was disabled. Removing instead");
        remove_watch(watch, data);
    }

    return TRUE;

error:
    return FALSE;
}



static void remove_watch(DBusWatch *watch, void *data) {
    int fd;
    unsigned int flags;

    (void)data;

#ifdef ANDROID
    fd = dbus_watch_get_fd(watch);
#else
    fd = dbus_watch_get_unix_fd(watch);
#endif

    flags = dbus_watch_get_flags(watch);

    if (flags & DBUS_WATCH_READABLE) {
        if (util_mainloop_remove_watch(EVENT_READ, fd) < 0) {
            MAD_LOG_W("Removing watch from read list failed!");
        }
    }

    if (flags & DBUS_WATCH_WRITABLE) {
        if (util_mainloop_remove_watch(EVENT_WRITE, fd) < 0) {
            MAD_LOG_W("Removing watch from write list failed!");
        }
    }

}

static void toggle_watch(DBusWatch *watch, void *data) {
    MAD_LOG_D("Toggling watch");
    remove_watch(watch, data);
    add_watch(watch, data);
}

static int mad_dbus_writable_event(const int fd, const void *data) {
    (void)fd;
    return mad_dbus_event((DBusWatch *)data, DBUS_WATCH_WRITABLE);
}

static int mad_dbus_readable_event(const int fd, const void *data) {
    (void)fd;
    return mad_dbus_event((DBusWatch *)data, DBUS_WATCH_READABLE);
}

static int mad_dbus_event(DBusWatch *watch, const unsigned int flags) {

    while (!dbus_watch_handle(watch, flags)) {
        MAD_LOG_W("dbus_watch_handle needs more memory. Spinning");
        sleep(1);
    }

    for (;;) {
        int res = dbus_connection_dispatch(s_conn);
        switch (res) {
            case DBUS_DISPATCH_COMPLETE:
                return 0;
            case DBUS_DISPATCH_NEED_MEMORY:
                MAD_LOG_W("dbus_connection_dispatch needs more memory. Spinning.");
                sleep(1);
                break;
            case DBUS_DISPATCH_DATA_REMAINS:
                MAD_LOG_D("remaining data for DBUS operation. Spinning.");
                break;
            default:
                MAD_LOG_E("invalid dispatch value.");
                break;
        }
    }
}

static DBusHandlerResult mad_dbus_incoming_msg_handler(DBusConnection *conn,
                                                        DBusMessage *msg, void *data) {
    mad_dbus_msg_type_t type;
    unsigned int index;
    bool msg_handled = false;

    (void)data;
    type = dbus_message_get_type(msg);

    MAD_LOG_D("Got message if: %s member: %s type %d",
                dbus_message_get_interface(msg),
                dbus_message_get_member(msg), type);

    for (index = 0; index < mad_dbus_msg_handlers_size; index++) {
        if (type == mad_dbus_msg_handlers[index].type
                && dbus_message_has_interface(msg, mad_dbus_msg_handlers[index].if_name)) {

            msg_handled = mad_dbus_msg_handlers[index].handler(conn, msg);

            if (msg_handled) {
                break;
            }
        }
    }

    if (!msg_handled) {
        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    }

    dbus_connection_flush(conn);
    return DBUS_HANDLER_RESULT_HANDLED;
}
