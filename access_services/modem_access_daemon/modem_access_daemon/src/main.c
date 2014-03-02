/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <util_mainloop.h>
#include <util_security.h>
#include "mad_log.h"
#include "mad_dbus.h"

#include "backend/at_handler.h"
#include "backend/fd_handler.h"


/* Static variables */
static int opt_daemonise;

static int stdin_fd = STDIN_FILENO;
static int stdout_fd = STDOUT_FILENO;
static int stderr_fd = STDERR_FILENO;


/* Local function prototypes */

static void main_signal_handler(int);
static void parse_options(int argc, const char **argv);
static void daemonise(void);
static int pre_loop_init(const void*);
static int post_loop_cleanup(const void*);

int main(int argc, const char **argv)
{
    int result;

    /* Set umask before anything else */
    util_set_restricted_umask();

    /* Do basic initialization */
    setbuf(stdout, 0);
    parse_options(argc, argv);

    if (opt_daemonise) {
        MAD_LOG_D("Daemonizing");
        daemonise();
    }

    /* Establish SIGTERM signal handler */
    signal(SIGTERM, main_signal_handler);

    /*Ingore SIGPIPE and avoid crash */
    if (signal((int)SIGPIPE, SIG_IGN) == SIG_ERR) {
        MAD_LOG_E("Modem Access Demon: SIG_ERR when ignoring SIGPIPE\n");
        abort();
    }

    /* prepare main loop */
    result = util_mainloop_prepare(NULL, pre_loop_init, post_loop_cleanup);
    if (result < 0) {
        MAD_LOG_E("Failed to prepare mainloop!");
        (void)post_loop_cleanup(NULL);
        exit(EXIT_FAILURE);
    }

    /* start the loop (will not return) */
    util_mainloop_loop();

    return result;
}

int pre_loop_init(const void* not_used) {
    (void)not_used;
    int result;

    mad_log_init();

    result = mad_dbus_handler_init();
    if (result < 0) {
        MAD_LOG_E("Failed to initialize DBus handler! Aborting");
        goto exit;
    }

    result = mad_at_handler_init();
    if (result < 0) {
        MAD_LOG_E("Failed to initialize AT handler!  Aborting");
        goto exit;
    }

     /* Handle USB Tether Events and Screen States */ 
     mad_fd_handler_init();


exit:
    return result;
}

int post_loop_cleanup(const void* not_used) {
    (void)not_used;

    mad_at_handler_shutdown();
    mad_dbus_handler_shutdown();


    /* Do final shutdown */
    close(stdin_fd);
    close(stdout_fd);
    close(stderr_fd);
    return 0;
}

void main_signal_handler(int signo)
{
    switch (signo) {
        case SIGTERM:
            MAD_LOG_D("Got TERM signal!");
            util_mainloop_quit();        /* Request process termination */
            break;
            /* Add signal handlers here as required */
        default:
            break;
    }

}

static int match(const char *a, const char *b)
{
    size_t                  n = strlen(b);
    size_t                  m = strlen(a);

    return (n == m ? !strncmp(a, b, n) : 0);
}

static void parse_options(int argc, const char **argv)
{
    --argc;
    ++argv;

    while (argc > 0) {
        const char *opt = *argv;
        --argc;
        ++argv;

        if (!opt) {
            continue;
        }

        if (match(opt, "-fg")) {
            opt_daemonise = 0;
        } else {
            UTIL_LOG_E("Bad argument=\"%s\"", opt);
            exit(EXIT_FAILURE);
        }
    }
}

void daemonise()
{
    pid_t pid;
    int fd;
    int i;

    pid = getppid();

    if (pid == 1) {             /* we are already a daemon */
        return;
    }

    pid = fork();

    if (pid < 0) {              /* Fork failed */
        MAD_LOG_E("fork failed");
        abort();
    }

    if (pid > 0) {              /* in the parent process */
        exit(EXIT_SUCCESS);
    }

    pid = setsid();             /* Create a new session id. */

    if (pid < 0) {
        MAD_LOG_E("setsid failed");
        abort();
    }

    i = chdir("/");             /* set CWD to / to unblock other fs */

    if (i < 0) {
        MAD_LOG_E("chdir failed");
        abort();
    }

    /* Redirect all standard I/O to /dev/null */
    fd = open("/dev/null", O_RDONLY);

    if (-1 != fd) {
        stdin_fd = dup2(fd, stdin_fd);
        close(fd);
    } else {
        MAD_LOG_W("unable to redirect stdin to /dev/null!");
    }

    fd = open("/dev/null", O_WRONLY);

    if (-1 != fd) {
        stdout_fd = dup2(fd, stdout_fd);
        stderr_fd = dup2(fd, stderr_fd);
        close(fd);
    } else {
        MAD_LOG_W("unable to redirect stdout and stderr to /dev/null!");
    }

    util_set_restricted_umask(); /* just to be sure... */

}
