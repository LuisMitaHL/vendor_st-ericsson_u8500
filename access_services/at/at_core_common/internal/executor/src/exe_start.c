/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <alloca.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

#include "atc_log.h"
#include "atc_selector.h"

#include "exe_p.h"
#include "exe.h"
#include "exe_start.h"
#include "exe_internal.h"

#ifdef EXE_USE_CN_SERVICE
#include "exe_cn_client.h"
#endif

#ifdef EXE_USE_COPS_SERVICE
#include "exe_cops_client.h"
#endif

#ifdef EXE_USE_SMS_SERVICE
#include "exe_sms_client.h"
#endif

#ifdef EXE_USE_PSCC_SERVICE
#include "exe_pscc_client.h"
#endif

#ifdef EXE_USE_SIM_SERVICE
#include "exe_sim_client.h"
#endif

#ifdef EXE_USE_SIMPB_SERVICE
#include "exe_simpb_client.h"
#endif

/*
 * Service list
 */
typedef struct {
    char *name_p;
    void *(* open)(void);
    void (* close)(void *);
    exe_service_t exe_service;
    exe_start_status_t status;
    void *service_p;
} exe_service_entry_t;

/* NOTE: the order here must be the same as for exe_start_status_t */
static exe_service_entry_t service_db[] = {
#ifdef EXE_USE_CN_SERVICE
    {"C&N"  , cn_client_open_session   , cn_client_close_session   , EXE_SERVICE_CN,    EXE_START_NOT_CONNECTED, NULL},
#endif
#ifdef EXE_USE_SIM_SERVICE
    {"SIM"  , sim_client_open_session  , sim_client_close_session  , EXE_SERVICE_SIM,   EXE_START_NOT_CONNECTED, NULL},
#endif
#ifdef EXE_USE_COPS_SERVICE
    {"COPS" , copsclient_open_session  , copsclient_close_session  , EXE_SERVICE_COPS,  EXE_START_NOT_CONNECTED, NULL},
#endif
    {"REST" , NULL                     , NULL                      , EXE_SERVICE_NONE,  EXE_START_NOT_CONNECTED, NULL},
#ifdef EXE_USE_SMS_SERVICE
    {"SMS"  , smsclient_open_session   , smsclient_close_session   , EXE_SERVICE_SMS,   EXE_START_NOT_CONNECTED, NULL},
#endif
#ifdef EXE_USE_PSCC_SERVICE
    {"PSCC" , psccclient_open_session  , psccclient_close_session  , EXE_SERVICE_PSCC,  EXE_START_NOT_CONNECTED, NULL},
#endif
#ifdef EXE_USE_SIMPB_SERVICE
    {"SIMPB", simpb_client_open_session, simpb_client_close_session, EXE_SERVICE_SIMPB, EXE_START_NOT_CONNECTED, NULL},
#endif
};


/*
 * Timer information
 */
static bool timer_is_running = false;
static int timer_read_fd = -1;
static int timer_write_fd = -1;
static timer_t timer_id;
static int timer_loop = 0;
static int timer_sleep_sec = 0;
static int timer_time_sec = 0;

/* Critical services flags */
static bool all_critical_connected = false;


/* Function prototypes */
static bool exe_start_selector_callback(int fd, void *data_p);
static void timer_handler(int sig, siginfo_t *si, void *uc);
static bool exe_start_timer(exe_t *exe_p);
static void exe_stop_timer(void);


/*
 *  Call back function triggered when exe_start_timer writes data "."
 *  to the internal pipe.
 */
bool exe_start_selector_callback(int fd, void *data_p)
{
    int status = 0;
    exe_start_services_t service = 0;
    int nbr_of_bytes = 0;
    char buf[10];
    struct itimerspec its;
    int sleep_sec;
    bool result = true;
    bool rest_connected = true;
    bool critical_connected = true;
    exe_t *exe_p = (exe_t *)data_p;

    /* Update the timer that ensures we are called regularly */
    timer_time_sec += timer_sleep_sec;

    ATC_LOG_D("%s: called, timer_time_sec = %d", __FUNCTION__, timer_time_sec);

    /* Read the data from the pipe. So it wont trigger again  */
    nbr_of_bytes = read(fd, buf, 1);

    /* Connect to critical services in the correct order */
    /* Connect to non-critical services in any order */
    for (service = EXE_START_CRITICAL + 1;
            critical_connected && service < EXE_START_LAST;
            service++) {

        /* Ignore REST demarcation service */
        if (EXE_START_REST == service) {
            continue;
        }

        /* Connect to service if not connected */
        if (EXE_START_NOT_CONNECTED == service_db[service].status) {
            ATC_LOG_I("Try to connect to service: %s", service_db[service].name_p);
            service_db[service].service_p = service_db[service].open();

            if (NULL != service_db[service].service_p) {
                /* Set client context pointer */
                exe_set_client_context(exe_p,
                                       service_db[service].exe_service,
                                       service_db[service].service_p);
            }
        }

        if (EXE_START_CONNECTED_READY != service_db[service].status) {
            if (service < EXE_START_REST) {
                critical_connected = false;
                break;
            }

            rest_connected = false;
            continue;
        }
    }

    if (timer_time_sec < 10) {
        /* Test every 1 sec the first 10 secs */
        sleep_sec = 1;
    } else if (timer_time_sec < 60) {
        /* Test every 10 sec for 1 min */
        sleep_sec = 10;
    } else if (timer_time_sec < 180) {
        /* Test every 30 sec for 3 min */
        sleep_sec = 30;
    } else {
        /* Test every 60 sec from then on */
        sleep_sec = 60;
    }

    timer_loop++;

    if (timer_sleep_sec != sleep_sec) {
        timer_sleep_sec = sleep_sec;
        its.it_value.tv_sec = sleep_sec;
        its.it_value.tv_nsec = 0;
        its.it_interval.tv_sec = its.it_value.tv_sec;
        its.it_interval.tv_nsec = its.it_value.tv_nsec;
        status = timer_settime(timer_id, 0, &its, NULL);

        if (status < 0) {
            ATC_LOG_E("%s: timer_settime ERROR (%s)!", __FUNCTION__, strerror(errno));
            exe_stop_timer();
            result = false;
            goto exit_func;
        }
    }

    /* Check service flags */
    if (critical_connected) {
        /* Set flag and return false to allow the start-up thread to
         * drop out of the exe_start_init() function */
        if (!all_critical_connected) {
            all_critical_connected = true;
            ATC_LOG_I("Connection with all critical services completed.");
            /* Returning false will break out of the selector_loop() call.
             * NOTE: It will also remove monitoring of the timer file-
             * descriptor. */
            result = false;
        }

        /* If all services are connected, stop the timer */
        if (rest_connected) {
            ATC_LOG_I("Connection with all services completed.");
            exe_stop_timer();
        }
    }

exit_func:
    return result;
}


void timer_handler(int sig, siginfo_t *si, void *uc)
{
    static char sigstr[] = ".";
    int result;

    switch (sig) {
    case SIGALRM:
        result = write(si->si_int, sigstr, 1);

        if (result < 0) {
            ATC_LOG_E("timer_handler: write restult %d on fd %d", result, si->si_int);
            exe_stop_timer();
        }

        break;

    default:
        break;
    }
}


bool exe_start_timer(exe_t *exe_p)
{
    int pipe_fd[2];
    struct itimerspec its;
    struct sigaction sa;
    struct sigevent sev;
    sigset_t mask;
    int result;

    /* Create pipe-pair  */
    result = pipe(pipe_fd);

    if (result < 0) {
        ATC_LOG_E("%s: pipe error (%s)", __FUNCTION__, strerror(errno));
        goto error;
    }

    timer_read_fd = pipe_fd[0];
    timer_write_fd = pipe_fd[1];

    /* Set read end non blocking, leave write end blocking */
    result = fcntl(timer_read_fd, F_SETFL, O_NONBLOCK);

    if (result != 0) {
        ATC_LOG_E("%s: pipe fcntl ERROR (%s)", __FUNCTION__, strerror(errno));
        goto cleanup_pipe;
    }

    /* Register the file descriptor */
    if (!selector_register_callback_for_fd(timer_read_fd, exe_start_selector_callback, (void *)exe_p)) {
        ATC_LOG_E("%s: selector_register_callback_for_fd %d failed!", __FUNCTION__, timer_read_fd);
        goto cleanup_pipe;
    }

    /* Establish signal handler */
    memset(&sa, 0, sizeof(sa));
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = timer_handler;
    sigemptyset(&sa.sa_mask);
    result = sigaction(SIGALRM, &sa, NULL);

    if (result < 0) {
        ATC_LOG_E("%s: sigaction ERROR (%s)", __FUNCTION__, strerror(errno));
        goto cleanup_pipe;
    }

    /* Create timer */
    memset(&sev, 0, sizeof(sev));
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = SIGALRM;
    sev.sigev_value.sival_int = timer_write_fd;
    result = timer_create(CLOCK_REALTIME, &sev, &timer_id);

    if (result < 0) {
        ATC_LOG_E("%s: timer_create ERROR (%s)", __FUNCTION__, strerror(errno));
        goto cleanup_signal;
    }

    /* Start the timer, initial delay 3 seconds */
    memset(&its, 0, sizeof(its));
    timer_sleep_sec = 3;
    its.it_value.tv_sec = timer_sleep_sec;
    its.it_value.tv_nsec = 0;
    its.it_interval.tv_sec = its.it_value.tv_sec;
    its.it_interval.tv_nsec = its.it_value.tv_nsec;
    result = timer_settime(timer_id, 0, &its, NULL);

    if (result < 0) {
        ATC_LOG_E("%s: timer_settime ERROR (%s)", __FUNCTION__, strerror(errno));
        goto cleanup_timer;
    }

    timer_is_running = true;
    ATC_LOG_I("Service connect timer started.");

    return true;

cleanup_timer:
    timer_delete(timer_id);

cleanup_signal:
    signal(SIGALRM, SIG_IGN);

cleanup_pipe:
    close(timer_read_fd);
    close(timer_write_fd);

error:
    return false;
}


/*
 * Clean up after service connection has completed
 */
void exe_stop_timer()
{
    /* Delete timer */
    if (timer_id >= 0) {
        timer_delete(timer_id);
        timer_id = -1;
    }

    /* De-register signal handler */
    signal(SIGALRM, SIG_IGN);

    /* De-register and close read-end of pipe */
    if (timer_read_fd >= 0) {
        selector_deregister_callback_for_fd(timer_read_fd);
        close(timer_read_fd);
        timer_read_fd = -1;
    }

    /* Close write end of pipe */
    if (timer_write_fd >= 0) {
        close(timer_write_fd);
        timer_write_fd = -1;
    }

    timer_is_running = false;
    ATC_LOG_I("Service connect timer stopped.");
}


/*
 * Initiate the internal data that is used to secure
 * the connection to all services.
 */
bool exe_start_init(exe_t *exe_p)
{
    int service = 0;

    ATC_LOG_I("Begin connecting to services.");

    /* Start the timer */
    if (!exe_start_timer(exe_p)) {
        ATC_LOG_E("%s: Failed starting timer!", __FUNCTION__);
    }

    /*
     * Wait for critical services to be connected before exiting,
     * or if the timer stops running,
     * or at most a minute,
     * whichever comes first.
     *
     * NOTE: When dropping out of selector_loop, the file descriptor has
     * been removed from monitoring. But if the timer is still active
     * it is necessary to register it again to get the callbacks.
     */
    do {
        selector_loop(selector_default_callback, NULL);

        if (timer_is_running) {
            /* Re-register timer callback */
            selector_register_callback_for_fd(timer_read_fd, exe_start_selector_callback, (void *)exe_p);
        }

    } while (!all_critical_connected &&
             timer_is_running &&
             timer_time_sec < 60);

    if (!all_critical_connected) {
        ATC_LOG_E("%s: Failed connecting to all critical services", __FUNCTION__);
        goto error;
    }

    ATC_LOG_I("Begin connecting to services successfully completed.");

    return true;

error:
    return false;
}


exe_start_status_t exe_start_get_status(exe_start_services_t service)
{
    exe_start_status_t status = EXE_START_UNKNOWN;

    if (service > EXE_START_CRITICAL && service < EXE_START_LAST) {
        status = service_db[service].status;
    }

    return status;
}


void exe_start_set_status(exe_start_services_t service, exe_start_status_t new_status)
{
    bool start_timer_thread = false;
    exe_start_status_t old_status;

    if (!(service > EXE_START_CRITICAL) || !(service < EXE_START_LAST)) {
        ATC_LOG_E("%s: service = %d, new_status = %d, invalid service!", __FUNCTION__, service, new_status);
        return;
    }

    /* Get previous (old) status */
    old_status = service_db[service].status;

    ATC_LOG_D("%s: service = %d, old_status = %d, new_status = %d",
              __FUNCTION__, service, old_status, new_status);

    if (new_status == old_status) {
        return;
    }

    /* Set new status */
    service_db[service].status = new_status;

    switch (new_status) {
    case EXE_START_NOT_CONNECTED: {
        /* Reset client context pointer */
        exe_set_client_context(exe_get_exe(), service_db[service].exe_service, NULL);

        /* Update the all_critical_connected flag for critical services */
        if (service < EXE_START_REST) {
            all_critical_connected = false;
        }

        /* Make sure the timer is running to allow the service(s) to be reconnected */
        if (!timer_is_running) {
            exe_start_timer(exe_get_exe());
        }

        break;
    }

    case EXE_START_CONNECTED_WAITING: {
        break;
    }

    case EXE_START_CONNECTED_READY: {
        struct itimerspec its;
        int remaining_sec;
        int result;

        /* If timer is not running, there's no reason to try to update it */
        if (!timer_is_running) {
            break;
        }

        /*
         * If status went directly from not-connected to connected-ready, then
         * there's no need to update the timer. Timer callback is already active.
         */
        if (EXE_START_NOT_CONNECTED == old_status) {
            break;
        }

        /* Adjust the timer to get a quick response to service connection updates */
        result = timer_gettime(timer_id, &its);

        if (0 > result) {
            ATC_LOG_E("%s: timer_gettime ERROR (%s)!", __FUNCTION__, strerror(errno));
            break;
        }

        /* Calculate how much time has passed */
        remaining_sec = its.it_value.tv_sec + ((its.it_value.tv_nsec + 500000000) / 1000000000);

        its.it_value.tv_sec = 0;
        its.it_value.tv_nsec = 1000000; /* Expire in one (1) millisecond */
        its.it_interval.tv_sec = 0;
        its.it_interval.tv_nsec = 0;
        result = timer_settime(timer_id, 0, &its, NULL);

        if (0 > result) {
            ATC_LOG_E("%s: timer_settime ERROR (%s)!", __FUNCTION__, strerror(errno));
        } else {
            /* Update the time passed, and force the timer to be set again */
            ATC_LOG_D("%s: timer_time_sec(%d) += timer_sleep_sec(%d) - remaining_sec(%d)",
                      __FUNCTION__, timer_time_sec, timer_sleep_sec, remaining_sec);
            timer_time_sec += timer_sleep_sec - remaining_sec;
            timer_sleep_sec = 0;
        }

        break;
    }

    default:
        ATC_LOG_E("%s: service = %d, new_status = %d, invalid status!", __FUNCTION__, service, new_status);
        break;
    }

}
