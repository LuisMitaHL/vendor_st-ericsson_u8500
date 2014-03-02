/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef msup_server_h
#define msup_server_h (1)

#define MSUP_RESET_TIMEOUT_MS               (3000)

#define MSUP_SERVER_SOCKET_NAME             "/dev/socket/msup"

#define MSUP_NOTIFICATION_WITH_FLAGS        "msup_notification: flags="
#define MSUP_NOTIFICATION_FLAGS_FORMAT      "0x%04X"
#define MSUP_NOTIFICATION_WITH_FLAGS_LENGTH (40)

typedef enum {
    MSUP_FLAGS_RESET                        = 0x00,     /* Reset all flags */
    MSUP_FLAG_RESET_IMMINENT                = 0x01,     /* The modem will be reset (by APE) imminently */
    MSUP_FLAG_FORCE_MSR_AFTER_NEXT_RESET    = 0x02,     /* Restart modem silently after next modem reset.
                                                         * Overrides '--try' | '-t' setting.
                                                         */
    MSUP_FLAG_NO_COREDUMP_AFTER_NEXT_RESET  = 0x04,     /* No modem coredump after next reset. */
} msup_flags_t;

void *msup_server_open(int *fd_p);
int msup_server_receive(void *p, msup_flags_t *flags_p);
void msup_server_close(void *p);

#endif /* msup_server_h */
