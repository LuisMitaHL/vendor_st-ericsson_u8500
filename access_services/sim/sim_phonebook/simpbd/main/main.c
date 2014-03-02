/*                               -*- Mode: C -*-
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * Description     : SIM Phonebook service server
 *
 * Author          : Mats Bergstrom <mats.m.bergstrom@stericsson.com>
 * Author          : Mats Bengtsson <mats.m.bengtsson@stericsson.com>
 * Author          : Johan Norberg <johan.xx.norberg@stericsson.com>
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/inotify.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

#include "log.h"
#include "main.h"
#include "fdmon.h"
#include "sockserv.h"

#include "simpbd.h"

#ifdef ENABLE_MODULE_TEST
#include "module_test_main.h"
#endif

#define MODEM_FM_POWER_OFF_FILE_SIMPBD  "/data/misc/modemfmpoweredoff_simpbd"
#define INOTIFY_EVENT_SIZE      (sizeof (struct inotify_event))
#define INOTIFY_EVENT_NUM       1
#define INOTIFY_BUF_LEN         (INOTIFY_EVENT_NUM * (INOTIFY_EVENT_SIZE + 16))

// -----------------------------------------------------------------------------
// Static variables
static int opt_daemonise;
static int num_servers;

static int stop = 0;            /* Process terminate flag */

static int stdin_fd = STDIN_FILENO;
static int stdout_fd = STDOUT_FILENO;
static int stderr_fd = STDERR_FILENO;

#ifdef ENABLE_MODULE_TEST
static pthread_t          module_test_thread;
static module_test_data_t module_test_data;
#endif

// -----------------------------------------------------------------------------
// Local function prototypes

static void main_signal_handler(int);
static int main_loop(void);

static void parse_options(int argc, const char **argv);
static int match(const char *a, const char *b);
static void daemonise(void);
static void await_modem_powered_on(void);

// -----------------------------------------------------------------------------

int main(int argc, const char **argv)
{
    int exit_code = 0;
    SIMPB_LOG_D("entered");

    /* Do basic initialization */
    /* FIXME: Uncomment the following line and add func_trace.c if you want instrumentation
     *  setProcName(argv[0]);
     */
    setbuf(stdout, 0);
    parse_options(argc, argv);

    if (opt_daemonise) {
        SIMPB_LOG_V("Daemonizing");
        daemonise();
    }

    /* Establish SIGTERM signal handler */
    signal(SIGTERM, main_signal_handler);
   /*Handle SIGPIPE to Avoid Crash*/
    if (signal((int)SIGPIPE, SIG_IGN) == SIG_ERR) {
        SIMPB_LOG_E("SIM_PB : SIG_ERR when ignoring SIGPIPE");
        abort();
    }

    /* Start module test thread */
#ifdef ENABLE_MODULE_TEST
    {
        int res = 0;
        res = pthread_create(&module_test_thread, NULL, module_test_main, &module_test_data);

        if (0 != res) {
            SIMPB_LOG_E("pthread_create failed for module_test_main!");
            goto exit;
        }

        res = pthread_detach(module_test_thread); /* Set property for automatic reclaim of TLS data when the thread terminates (to avoid memory leak) */

        if (0 != res) {
            SIMPB_LOG_E("pthread_detach failed for module_test_main!");
            goto exit;
        }
    }
#endif /* ENABLE_MODULE_TEST */

    /* Run main loop */
    exit_code = main_loop();

#ifdef ENABLE_MODULE_TEST
exit:
#endif
    /* Do final shutdown */
    close(stdin_fd);
    close(stdout_fd);
    close(stderr_fd);

    exit(exit_code);
}

// -----------------------------------------------------------------------------

int main_loop()
{
    int exit_code = EXIT_SUCCESS;
    int err;
    int res;

    /* Await modem powered on */
    await_modem_powered_on();

    SIMPB_LOG_I("SIM Phonebook service start-up, initialising ...");

    /* Initialise services.
     * If any service needs to use a thread or other resource
     * it is expected to create and initialise them in its
     * init call and then return. Any file-descriptors or
     * sockets that need to be monitored and acted upon should
     * also be registered with the fdmon service in this call.
     */

    /* File descriptor monitor */
    if (fdmon_init() < 0) {
        exit_code = EXIT_FAILURE;
        goto shutdown_complete;
    }

    /* Socket server */
    num_servers = 0;
    num_servers += simpbd_get_num_servers();

    /* Add calls to get count of additional socket servers here */

    /* Initialise socket server */
    if (sockserv_init(num_servers) < 0) {
        exit_code = EXIT_FAILURE;
        goto shutdown_fdmon;
    }

    /* SIM Phonebook service */
    if (simpbd_init() < 0) {
        exit_code = EXIT_FAILURE;
        goto shutdown_sockserv;
    }

    /* Add initialisation to additional services here */

    /* Do file descriptor monitoring */
    SIMPB_LOG_I("Initialisation complete, entering monitoring loop.");

    err = 0;

    while (!err && !stop) {
        res = fdmon_waitevent(-1);

        if (res < 0) {
            /* Error: set err flag */
            err = 0 - res;
            exit_code = EXIT_FAILURE;
        } else if (res > 0) {
            /* Event: event related activity not covered by
             * fdmon callbacks go here */
        } else {
            /* Timeout: regular activity goes here */
            /* service_heartbeat(); */
        }
    }

    /* Shutdown.
     * Shutdown all services in reverse order.
     */
    SIMPB_LOG_I("Monitor loop exited, beginning shut down ...");

    /* SIM Phonebook service */
    simpbd_shutdown();

shutdown_sockserv:
    /* Socket server */
    sockserv_shutdown();

shutdown_fdmon:
    /* File descriptor monitor */
    fdmon_shutdown();

shutdown_complete:
    SIMPB_LOG_I("Shut down complete.");
    return(exit_code);
}


// -----------------------------------------------------------------------------

void main_signal_handler(int signo)
{
    switch (signo) {
    case SIGTERM:
        main_shutdown();        /* Request process termination */
        break;
        /* Add signal handlers here as required */
    default:
        break;
    }
}


// -----------------------------------------------------------------------------

void main_shutdown()
{
    SIMPB_LOG_D("entered");
    /* Set stop flag so that we drop out of main_loop */
    stop = 1;

    /* Make sure we drop out of a wait for file descriptor event call */
    fdmon_stopwait();
}


// -----------------------------------------------------------------------------

void parse_options(int argc, const char **argv)
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
        }
        /*
                else if (match(opt, "-v")) {
                    ++opt_v;
                }

                else if (match(opt, "-vv")) {
                    opt_v += 2;
                }

                else if (match(opt, "-vvv")) {
                    opt_v += 3;
                }

                else if (match(opt, "-vvvv")) {
                    opt_v += 4;
                }
        */
        /*
                else if (match(opt, "--need-pin")) {
                    pin_required_flag = 1;
                    SIMPB_LOG_V("pin_required_flag set to %d", pin_required_flag);
                }
        */
        else {
            SIMPB_LOG_E("Bad argument=\"%s\"", opt);
            exit(EXIT_FAILURE);
        }

    };

}


// -----------------------------------------------------------------------------

static int match(const char *a, const char *b)
{
    size_t                  n = strlen(b);
    size_t                  m = strlen(a);

    return (n == m ? !strncmp(a, b, n) : 0);
}


// -----------------------------------------------------------------------------

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
        SIMPB_LOG_E("fork failed");
        abort();
    }

    if (pid > 0) {              /* in the parent process */
        exit(EXIT_SUCCESS);
    }

    pid = setsid();             /* Create a new session id. */

    if (pid < 0) {
        SIMPB_LOG_E("setsid failed");
        abort();
    }

    i = chdir("/");             /* set CWD to / to unblock other fs */

    if (i < 0) {
        SIMPB_LOG_E("chdir failed");
        abort();
    }

    /* Redirect all standard I/O to /dev/null */
    fd = open("/dev/null", O_RDONLY);

    if (-1 != fd) {
        stdin_fd = dup2(fd, stdin_fd);
        close(fd);
    } else {
        SIMPB_LOG_E("unable to redirect stdin to /dev/null!");
    }

    fd = open("/dev/null", O_WRONLY);

    if (-1 != fd) {
        stdout_fd = dup2(fd, stdout_fd);
        stderr_fd = dup2(fd, stderr_fd);
        close(fd);
    } else {
        SIMPB_LOG_E("unable to redirect stdout and stderr to /dev/null!");
    }

    umask(0);                   /* just to be sure... */
}

static void await_modem_powered_on()
{
    int len,fd, wd;
    char buf[INOTIFY_BUF_LEN];
    struct inotify_event *event;

    fd = inotify_init();
    if (fd < 0) {
        SIMPB_LOG_E("inotify_init error!");
        return;
    }

    /*
     * As we want to make this daemon silent and relaxed we just await the deletion
     * of the file ...
    */
    while(1) {
        SIMPB_LOG_I("Wait to be triggered...");

        wd = inotify_add_watch(fd, MODEM_FM_POWER_OFF_FILE_SIMPBD, IN_DELETE | IN_ONESHOT | IN_ACCESS | IN_DELETE_SELF);
        if (wd < 0) {
            SIMPB_LOG_I("SIM_PHONEBOOK Normal startup...");
            /* Can't set watch so file actually don't exist */
            goto exit;
        }

        len = read (fd, buf, INOTIFY_BUF_LEN);
        if (len > 0) {
            event = (struct inotify_event *) &buf[0];
            if (event->mask & IN_DELETE) {
                SIMPB_LOG_I("SIM_PHONEBOOK startup in flight mode...");
                goto exit;
                /* NOTE : (void)inotify_rm_watch (fd, wd); No need as long as IN_ONESHOT trigger is used */
            }
        }
    }
exit:
    SIMPB_LOG_I("triggered");
    close(fd);
}
