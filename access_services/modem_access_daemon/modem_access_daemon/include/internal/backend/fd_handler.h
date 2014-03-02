/**
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */



typedef enum {
    FD_DISABLE          = 1,
    FD_ENABLE           = 2
} mad_fast_dormancy_values_t;


void mad_fd_handler_init();
void mad_screenstate_monitor_init();
int screen_display_process_callback(const int fd, const void *data_p);
void mad_screen_tether_event();
bool tether_dbus_msg_handler(DBusConnection *conn, DBusMessage *msg);
