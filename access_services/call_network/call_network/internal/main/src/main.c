/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

#include "cn_log.h"
#include "cn_macros.h"
#include "main.h"
#include "fdmon.h"
#include "sockserv.h"
#include "cnserver.h"

#include "util_security.h"

#ifndef ENABLE_MODULE_TEST
#include "cn_timer.h"
#endif

#ifdef ENABLE_MODULE_TEST
#include "module_test_main.h"
#endif

/* Static variables */
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

/* Local function prototypes */

static void main_signal_handler(int);
static int main_loop(void);

static void parse_options(int argc, const char **argv);
static int match(const char *a, const char *b);
static void daemonise(void);

int main(int argc, const char **argv)
{
    int exit_code = 0;


    /* Set umask before anything else */
    util_set_restricted_umask();

    /* Do basic initialization */
    /* NOTE: Uncomment the following line and add func_trace.c if you want instrumentation
     *  setProcName(argv[0]);
     */
    setbuf(stdout, 0);
    parse_options(argc, argv);

    if (opt_daemonise) {
        CN_LOG_D("Daemonizing");
        daemonise();
    }

    /* Establish SIGTERM signal handler */
    signal(SIGTERM, main_signal_handler);

    /*Ingore SIGPIPE and avoid crash */
    if (signal((int)SIGPIPE, SIG_IGN) == SIG_ERR) {
        CN_LOG_E("CNS : SIG_ERR when ignoring SIGPIPE\n");
        abort();
    }

    /* Start module test thread */
#ifdef ENABLE_MODULE_TEST
    {
        int res = 0;
        res = pthread_create(&module_test_thread, NULL, module_test_main, &module_test_data);

        if (0 != res) {
            CN_LOG_E("pthread_create failed for module_test_main!");
            goto exit;
        }

        res = pthread_detach(module_test_thread); /* Set property for automatic reclaim of TLS data when the thread terminates (to avoid memory leak) */

        if (0 != res) {
            CN_LOG_E("pthread_detach failed for module_test_main!");
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

int main_loop()
{
    int exit_code = EXIT_SUCCESS;
    int err;
    int res;


    CN_LOG_D("Call & Network service start-up, initialising...");

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
    num_servers = cnserver_get_num_servers();

    /* Add calls to get count of additional socket servers here */
    if (sockserv_init(num_servers) < 0) {
        exit_code = EXIT_FAILURE;
        goto shutdown_fdmon;
    }

    /* Call and Network server */
    if (cnserver_init() < 0) {
        exit_code = EXIT_FAILURE;
        goto shutdown_sockserv;
    }

#ifndef ENABLE_MODULE_TEST
    /* Call and Network timer */
    if (cn_timer_init() < 0) {
        CN_LOG_E("timer init failed");
        goto shutdown_cnserver;
    }
#endif /* ENABLE_MODULE_TEST */

    /* Add initialisation to additional services here */

    /* Do file descriptor monitoring */
    CN_LOG_D("Initialisation complete, entering monitoring loop.");

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
    CN_LOG_D("Monitor loop exited, beginning shut down ...");

shutdown_cnserver:
    /* Call and Network server */
    CN_LOG_D("cnserver shutdown");
    cnserver_shutdown();

shutdown_sockserv:
    /* Socket server */
    CN_LOG_D("sockserv shutdown");
    sockserv_shutdown();

shutdown_fdmon:
    /* File descriptor monitor */
    CN_LOG_D("fdmon shutdown");
    fdmon_shutdown();

shutdown_complete:
    CN_LOG_D("Shut down complete.");

    return(exit_code);
}


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


void main_shutdown()
{

    /* Set stop flag so that we drop out of main_loop */
    stop = 1;

    /* Make sure we drop out of a wait for file descriptor event call */
    fdmon_stopwait();

}


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
        } else {
            CN_LOG_E("Bad argument=\"%s\"", opt);
            exit(EXIT_FAILURE);
        }
    }

}


static int match(const char *a, const char *b)
{
    size_t                  n = strlen(b);
    size_t                  m = strlen(a);


    return (n == m ? !strncmp(a, b, n) : 0);
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
        CN_LOG_E("fork failed");
        abort();
    }

    if (pid > 0) {              /* in the parent process */
        exit(EXIT_SUCCESS);
    }

    pid = setsid();             /* Create a new session id. */

    if (pid < 0) {
        CN_LOG_E("setsid failed");
        abort();
    }

    i = chdir("/");             /* set CWD to / to unblock other fs */

    if (i < 0) {
        CN_LOG_E("chdir failed");
        abort();
    }

    /* Redirect all standard I/O to /dev/null */
    fd = open("/dev/null", O_RDONLY);

    if (-1 != fd) {
        stdin_fd = dup2(fd, stdin_fd);
        close(fd);
    } else {
        CN_LOG_W("unable to redirect stdin to /dev/null!");
    }

    fd = open("/dev/null", O_WRONLY);

    if (-1 != fd) {
        stdout_fd = dup2(fd, stdout_fd);
        stderr_fd = dup2(fd, stderr_fd);
        close(fd);
    } else {
        CN_LOG_W("unable to redirect stdout and stderr to /dev/null!");
    }

    util_set_restricted_umask(); /* just to be sure... */

}

