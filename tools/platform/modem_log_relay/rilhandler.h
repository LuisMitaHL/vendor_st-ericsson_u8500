/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef RILHANDLER_H
#define RILHANDLER_H

#define RIL_MESSAGE_SIZE 50
#define TRIGGER_COMMAND "trace -r"

int ril_port;

int open_ril_server(int port);
int connect_client();
int receive_from_ril(char *message);
int send_to_ril(char *message);
void disconnect_client();
void close_ril_server();

#endif // #ifndef RILHANDLER_H
