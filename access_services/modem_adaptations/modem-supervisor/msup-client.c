/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>


#include "dgram-sock.h"
#include "msup-client.h"


int msup_send_notification(msup_flags_t flags)
{
	int n;
    int msg_len;
	char msg[MSUP_NOTIFICATION_WITH_FLAGS_LENGTH];
	void *dgram_p;

	dgram_p = dgram_sock_init(DGRAM_SOCK_CLIENT, MSUP_SERVER_SOCKET_NAME);
	if (dgram_p == NULL) {
	    return -1;
	}

	msg_len = sprintf(msg, MSUP_NOTIFICATION_WITH_FLAGS MSUP_NOTIFICATION_FLAGS_FORMAT, flags);

	n = dgram_sock_send(dgram_p, msg, msg_len + 1);

	dgram_sock_destroy(dgram_p);

	return n;
}
