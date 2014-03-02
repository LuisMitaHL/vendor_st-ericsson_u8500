/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "atc_anchor.h"
#include "atc_log.h"

extern bool atc_connect_external(anchor_connection_t *conn_p);

/* The anchor handles MAX_NBR_CONNECTIONS connections plus one for debugging */
static anchor_connection_t connection[MAX_NBR_CONNECTIONS + 1];

/*--------------------------------------------------------------
 * Handler for SIGCHLD
 * -------------------------------------------------------------*/
static void sigchild_handler(int value)
{
    int i;
    /* TODO: 1. Is this thread safe?
     * TODO: 2. Maybe do waitpid instead in workers to handle more than one handover*/

    for (i = 0; i < MAX_NBR_CONNECTIONS; i++) {
        connection[i].connected_to = ANC_POINT_AT;
    }
}

/*--------------------------------------------------------------
 * Registers a handler for SIGCHLD
 * -------------------------------------------------------------*/
static bool set_sigprocmask(void)
{
    /* sigset_t oldset; */
    /*int status; */
    signal(SIGCHLD, sigchild_handler);
    return true;
}

/*--------------------------------------------------------------
 * Initializes a connection
 * -------------------------------------------------------------*/
void anc_init_conn(anchor_connection_t *conn_p)
{
    conn_p->connected_to = ANC_POINT_NONE;
    conn_p->type = ANC_TYPE_NONE;
    conn_p->fdr = -1;
    conn_p->fdw = -1;
    conn_p->pid = -1;
    conn_p->device_name[0] = 0;
    conn_p->baud = -1;
}

/*--------------------------------------------------------------
 * Initializes the anchor
 * -------------------------------------------------------------*/
void anc_init(void)
{
    int i;

    for (i = 0; i <= MAX_NBR_CONNECTIONS; i++) {
        anc_init_conn(&connection[i]);
    }

    set_sigprocmask();
    return;
}

/*--------------------------------------------------------------
 * Runs in the child after a fork and changes stdin and stdout
 * to the fds for the current connection.
 * -------------------------------------------------------------*/
static void duplicate_fds(anchor_connection_t *conn_p)
{
    /* assume exections as child*/
    int status;
    status = dup2(conn_p->fdr, STDIN_FILENO); /* dup fd to stdin */

    if (status < 0) {
        /* TODO: Find a nice way of handle this...*/
        ATC_LOG_E("dup2() failed :(\n");
        close(conn_p->fdr);
        close(conn_p->fdw);
        conn_p->connected_to = ANC_POINT_NONE;
        exit(1);
    }

    status = dup2(conn_p->fdw, STDOUT_FILENO); /* dup fd to stdout */

    if (status < 0) {
        /* TODO: Find a nice way of handle this...*/
        ATC_LOG_E("dup2() failed :(\n");
        close(conn_p->fdr);
        close(conn_p->fdw);
        conn_p->connected_to = ANC_POINT_NONE;
        exit(1);
    }
}


/*--------------------------------------------------------------
 * Starts the application responsible for the new anchor point.
 * debug mux and dun should be supported according to the DS.
 * -------------------------------------------------------------*/
static bool start_app(anchor_point_t to)
{
    bool result = false;
    int status;

    if (to == ANC_POINT_DBMX) {
        ATC_LOG_I("Calling execl()\n");
        status = execlp("./debugmux", "AT*EDEBUGMUX", NULL);
        /* TODO: The correct path to debugmux should be /sbin/debugmux
         * (should maybe try that first and the fall back to ./debugmux */

        if (status < 0) {
            ATC_LOG_E("execlp() failed :(\n");
        } else {
            result = true; /* This should not happen :)*/
        }
    }

    return result;
}


/*--------------------------------------------------------------
 * Starts a hand over to an other anchor point.
 * -------------------------------------------------------------*/
bool anc_handover(anchor_connection_t *conn_p, anchor_point_t to)
{
    bool result = false;

    /* Check if valid handover. */
    if (to > ANC_POINT_AT && conn_p->connected_to == ANC_POINT_AT) {
        pid_t pid = -1;
        pid = fork();

        if (pid < 0) { /* Fork failed */
            close(conn_p->fdr);
            close(conn_p->fdw);
            conn_p->connected_to = ANC_POINT_NONE;
            /* TODO: return false instead of closing the connection */
        }

        if (pid == 0) { /* child */
            duplicate_fds(conn_p);

            if (start_app(to)) {
                exit(EXIT_SUCCESS);
            } else {
                exit(EXIT_FAILURE);
            }
        } else { /* parent */
            /* Trust our child and assume everything went well*/
            conn_p->connected_to = to;
            conn_p->pid = pid;
            result = true;
        }
    }

    return result;
}


/*--------------------------------------------------------------
 * Returns the array of connections
 * -------------------------------------------------------------*/
anchor_connection_t *anc_get_connections(void)
{
    return connection;
}


/*--------------------------------------------------------------
 * Returns the connection that reads from the fd fd.
 * -------------------------------------------------------------*/
anchor_connection_t *anc_get_conn_by_fd(int fd)
{
    int i;

    for (i = 0; i <= MAX_NBR_CONNECTIONS; i++) {
        if (connection[i].fdr == fd) {
            return &connection[i];
        }
    }

    return NULL;
}


/*--------------------------------------------------------------
 *
 * -------------------------------------------------------------*/
static bool anc_add_connection(anchor_point_t connected_to, anchor_type_t type, int fdr, int fdw, char *name, long baud)
{
    bool result = false;
    int i;

    for (i = 0; i < MAX_NBR_CONNECTIONS; i++) {
        if (connection[i].connected_to == ANC_POINT_NONE) {
            connection[i].connected_to = connected_to;
            connection[i].type = type;
            connection[i].fdr = fdr;
            connection[i].fdw = fdw;
            strcpy(connection[i].device_name, name);
            connection[i].baud = baud;
            atc_connect_external(&connection[i]);
            result = true;
            break;
        }
    }

    return result;
}


/*--------------------------------------------------------------
 *
 * -------------------------------------------------------------*/
int anc_connect(char *name, long baud)
{
    int fd = -1;
    bool result;
    char devname[SERIAL_DEVICE_MAX_LENGTH + 6];
    int res;

    res = snprintf(devname, sizeof(devname), "/dev/%s", name);

    if ((int)sizeof(devname) <= res) {
        ATC_LOG_E("Device name \"%s\" truncated!", devname);
        return -1;
    }

    fd = open(devname, O_RDWR | O_NONBLOCK | O_NOCTTY);

    if (0 > fd) {
        int errnum = errno;
        ATC_LOG_E("Open of device \"%s\" failed, errno=%d (%s)", devname, errnum, strerror(errnum));
        return -1;
    }

    ATC_LOG_I("Opened port \"%s\" as AT channel (fd=%d).", name, fd);

    result = anc_add_connection(ANC_POINT_AT, ANC_TYPE_SERIAL, fd, fd, name, baud);

    if (!result) {
        ATC_LOG_E("anc_add_connection for \"%s\" failed!", name);
        close(fd);
        return -1;
    }

    return fd;
}


/*--------------------------------------------------------------
 *
 * -------------------------------------------------------------*/
bool anc_disconnect(anchor_connection_t *conn_p)
{
    bool result = false;

    if (conn_p->connected_to == ANC_POINT_AT) {
        /* Close the connection */
        if (close(conn_p->fdr) >= 0) {
            ATC_LOG_I("Closed port \"%s\" as AT channel (fd=%d).", conn_p->device_name, conn_p->fdr);
            /* close the other connection if we are debugging with pipes */
            if (conn_p->fdr != conn_p->fdw) {
                close(conn_p->fdw);
            }

            anc_init_conn(conn_p);
            result = true;
        }
    }

    return result;
}


/* debug code below --------------------------------------------*/
int test_connect(char *name)    /* used until we have USB */
{
    int bidirectional = -1;
    int fdr = -1;
    int fdw = -1;
    int i = 0;
    char dummy[30];
    int last_pos;
    ATC_LOG_I("[anchor] Connect to %s\n", name);
    fdr = open(name, O_RDONLY);
    ATC_LOG_I("[anchor] opened fdr=%d\n", fdr);

    if (fdr >= 0) {
        if ((bidirectional = isatty(fdr)) >= 0) {
            if (bidirectional) {
                fdw = fdr;
            } else {
                /* todo: clean up this code */
                strcpy(dummy, name);
                last_pos = strlen(dummy) - 1;
                dummy[last_pos] = dummy[last_pos] + 1;
                ATC_LOG_I("[anchor] and writing to %s\n", dummy);
                fdw = open(dummy, O_WRONLY);
                ATC_LOG_I("[anchor] opened fdw=%d\n", fdw);
            }

            anc_add_connection(ANC_POINT_AT, ANC_TYPE_SERIAL, fdr, fdw, name, 0);
        }
    }

    return bidirectional;
}


int test_disconnect(char *name_p)
{
    /*TODO add stuff*/
    return -1;
}
