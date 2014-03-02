/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <errno.h>
#include <atc_anchor.h>
#include <atc.h>
#include <atc_debug.h>
#include <atc_log.h>

void print_hex(unsigned char *pointer, unsigned int len)
{
    static const char hex[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8',
                                '9', 'a', 'b', 'c', 'd', 'e', 'f'
                              };

    while (len) {
        putchar(hex[(*pointer >> 4) & 0x0f]);
        putchar(hex[*pointer & 0x0f]);
        putchar(' ');

        pointer++;
        len--;
    }
}

/*--------------------------------------------------------------
 * Creates some fifos in the file system for debugging.
 *
 * -------------------------------------------------------------*/
bool at_debug_mkfifo(void)
{
    int status;
    bool result = false;

    status = mkfifo(AT_DEBUG_PIPE, S_IRUSR | S_IWUSR);

    if (status == 0 || (status == -1 && errno == EEXIST)) {
        result = true;
        ATC_LOG_I("[At] fifo :)\n");
    } else {
        ATC_LOG_I("[At] fifo :( %d\n", status);
    }

    return result;
}

/*--------------------------------------------------------------
 * Prints the content of an anchor connection
 * -------------------------------------------------------------*/
void at_debug_print_conn(anchor_connection_t *conn)
{
    if (conn) {
        switch (conn->connected_to) {
        case ANC_POINT_NONE:
            ATC_LOG_I("\tconnected_to: None\n");
            break;
        case ANC_POINT_AT:
            ATC_LOG_I("\tconnected_to: AT\n");
            break;
        case ANC_POINT_DBMX:
            ATC_LOG_I("\tconnected_to: debugMux\n");
            break;
        case ANC_POINT_DUN:
            ATC_LOG_I("\tconnected_to: DUN\n");
            break;
        default:
            ATC_LOG_I("\tconnected_to: %d", conn->connected_to);
        }

        ATC_LOG_I("\tfdr         : %d\n", (int) conn->fdr);
        ATC_LOG_I("\tfdw         : %d\n", (int) conn->fdw);
        ATC_LOG_I("\tpid         : %d\n", (int) conn->pid);
        ATC_LOG_I("\tdevice_name : %s\n", conn->device_name);
    } else {
        ATC_LOG_I("\tconnection conn == NULL\n");
    }
}

/*--------------------------------------------------------------
 * Sets up a pipe used for debugging.
 * -------------------------------------------------------------*/
int at_debug_init(void)
{
    int fd = -1;

    ATC_LOG_I("[At] create debug fifo\n");

    if (at_debug_mkfifo()) {
        anchor_connection_t *conn;
        anchor_connection_t *debug_conn;

        conn = anc_get_connections();
        debug_conn = &conn[DEBUG_CONNECTION];
        debug_conn->connected_to = ANC_POINT_AT;
        debug_conn->type = ANC_TYPE_DEBUG;
        debug_conn->pid = -1;
        strcpy(debug_conn->device_name, AT_DEBUG_PIPE);

        ATC_LOG_I("[At] debug connection ---\n");
        at_debug_print_conn(debug_conn);

        ATC_LOG_I("[At] open debug fifo\n");

        fd = open(AT_DEBUG_PIPE, O_RDONLY);
        debug_conn->fdr = fd;
        debug_conn->fdw = -1;

        if (debug_conn->fdr >= 0) {
            ATC_LOG_I("Succeed, got debug connection, fdr = %d\n", debug_conn->fdr);
        } else {
            ATC_LOG_I("errno %d\n", errno);
        }
    }

    return fd;
}
