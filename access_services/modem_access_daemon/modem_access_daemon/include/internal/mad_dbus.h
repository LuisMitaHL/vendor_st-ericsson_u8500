/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef MAD_DBUS_H
#define MAD_DBUS_H

#include <stdbool.h>
#include <dbus/dbus.h>

typedef enum {
    INVALID = DBUS_MESSAGE_TYPE_INVALID,
    METHOD_CALL = DBUS_MESSAGE_TYPE_METHOD_CALL,
    METHOD_RETURN = DBUS_MESSAGE_TYPE_METHOD_RETURN,
    ERROR = DBUS_MESSAGE_TYPE_ERROR,
    SIGNAL = DBUS_MESSAGE_TYPE_SIGNAL
} mad_dbus_msg_type_t;

typedef bool mad_dbus_handler_method(DBusConnection *conn, DBusMessage *msg);

typedef struct {
    mad_dbus_msg_type_t type;
    const char *if_name;
    const char *match_rule;
    mad_dbus_handler_method *handler;
} mad_dbus_msg_handler_element_t;



extern const mad_dbus_msg_handler_element_t mad_dbus_msg_handlers[];
extern const size_t mad_dbus_msg_handlers_size;

int mad_dbus_handler_init();
void mad_dbus_handler_shutdown();

#endif        /* #ifndef MAD_DBUS_H */

