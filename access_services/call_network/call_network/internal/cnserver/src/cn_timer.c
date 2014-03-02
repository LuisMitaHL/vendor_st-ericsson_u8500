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
#include <stdbool.h>
#include <errno.h>
#include <time.h>

#include "cn_log.h"
#include "cn_macros.h"
#include "cn_timer.h"
#include "fdmon.h"

/* Static variables */

typedef struct {
    int timer_read_pipe;
    int timer_write_pipe;
} cn_timer_context_t;

typedef struct timer_info_struct_t {
    timer_t timer_id;
    cn_timer_callback_t timer_callback;
    void *data_p;
    struct timer_info_struct_t *next_p;
} cn_timer_item_t;

static cn_timer_context_t cn_timer_context;

static cn_timer_item_t *cn_timer_item_p = NULL;

/* Local function prototypes */

static int cn_timer_callback(const int fd, const void *context);
static int cn_timer_callback_remove(const int fd, const void *context);


int cn_timer_init(void)
{
    int pipefd[2];
    int errnum, flags;
    int timer_read_pipe, timer_write_pipe;


    if (pipe(pipefd) < 0) {
        CN_LOG_E("pipe() failed, errno=%d", errno);
        goto error_return;
    }

    timer_read_pipe = pipefd[0];
    flags = fcntl(timer_read_pipe, F_GETFD, 0);

    if (flags < 0 || fcntl(timer_read_pipe, F_SETFL, flags | O_NONBLOCK) < 0) {
        errnum = errno;
        CN_LOG_E("fcntl error(%d)=\"%s\"", errnum, strerror(errnum));
        goto cleanup_return;
    }

    timer_write_pipe = pipefd[1];
    flags = fcntl(timer_write_pipe, F_GETFL, 0);

    if (flags < 0 || fcntl(timer_write_pipe, F_SETFL, flags | O_NONBLOCK) < 0) {
        errnum = errno;
        CN_LOG_E("fcntl error(%d)=\"%s\"", errnum, strerror(errnum));
        goto cleanup_return;
    }

    cn_timer_context.timer_read_pipe = timer_read_pipe;
    cn_timer_context.timer_write_pipe = timer_write_pipe;

    fdmon_add(timer_read_pipe, &cn_timer_context, &cn_timer_callback, &cn_timer_callback_remove);

    return 0;

cleanup_return:
    close(pipefd[0]);
    close(pipefd[1]);

error_return:
    return -1;
}

static int cn_timer_get_nr_of_timers()
{
    int nr_of_timers = 0;
    cn_timer_item_t *curr_timer_item_p = cn_timer_item_p;


    while (NULL != curr_timer_item_p) {
        curr_timer_item_p = curr_timer_item_p->next_p;
        nr_of_timers++;
    }

    return nr_of_timers;
}

static void cn_timer_init_item(cn_timer_item_t *timer_item_p)
{
    timer_item_p->timer_id = -1;
    timer_item_p->timer_callback = NULL;
    timer_item_p->data_p = NULL;
    timer_item_p->next_p = NULL;
}

static cn_timer_item_t *cn_timer_create_new_item()
{
    cn_timer_item_t *curr_timer_item_p = cn_timer_item_p;
    cn_timer_item_t *prev_timer_item_p = NULL;

    while (NULL != curr_timer_item_p) {
        prev_timer_item_p = curr_timer_item_p;
        curr_timer_item_p = curr_timer_item_p->next_p;
    }

    curr_timer_item_p = calloc(1, sizeof(cn_timer_item_t));

    if (NULL == curr_timer_item_p) {
        CN_LOG_E("memory allocation failed for new timer item");

        return NULL;
    }

    cn_timer_init_item(curr_timer_item_p);

    if (NULL == cn_timer_item_p) {
        cn_timer_item_p = curr_timer_item_p;
    } else {
        prev_timer_item_p->next_p = curr_timer_item_p;
    }

    return curr_timer_item_p;
}

bool cn_timer_remove_item(cn_timer_item_t *timer_item_p)
{
    cn_timer_item_t *curr_timer_item_p = cn_timer_item_p;
    cn_timer_item_t *prev_timer_item_p = NULL;


    if (NULL == timer_item_p) {
        CN_LOG_E("timer item to remove is NULL");

        return false;
    }

    if (NULL == cn_timer_item_p) {
        CN_LOG_E("timer doesn't have any items");

        return false;
    }

    while (NULL != curr_timer_item_p) {

        if (curr_timer_item_p == timer_item_p) {
            break;
        } else {
            prev_timer_item_p = curr_timer_item_p;
            curr_timer_item_p = curr_timer_item_p->next_p;
        }

    }

    if (NULL == curr_timer_item_p) {
        CN_LOG_E("timer item for removal not found");

        return false;
    }

    if (NULL == prev_timer_item_p) {
        cn_timer_item_p = curr_timer_item_p->next_p;
    } else {
        prev_timer_item_p->next_p = curr_timer_item_p->next_p;
    }

    free(curr_timer_item_p->data_p);
    free(curr_timer_item_p);

    return true;
}

static cn_timer_item_t *cn_timer_get_item(cn_timer_callback_t timer_callback)
{
    cn_timer_item_t *curr_timer_item_p = cn_timer_item_p;


    if (NULL == timer_callback) {
        CN_LOG_E("timer callback is NULL");
        curr_timer_item_p = NULL;
        goto exit;
    }

    while (NULL != curr_timer_item_p) {

        if (curr_timer_item_p->timer_callback == timer_callback) {
            break;
        } else {
            curr_timer_item_p = curr_timer_item_p->next_p;
        }

    }

exit:
    return curr_timer_item_p;
}

static void cn_timer_handler(int sig, siginfo_t *si_p, void *uc_p)
{
    (void)uc_p;


    switch (sig) {
        case SIGALRM: {
            cn_timer_item_t *timer_item_p;

            timer_item_p = (cn_timer_item_t *)si_p->si_ptr;

            if (NULL == timer_item_p) {
                CN_LOG_E("timer item is NULL");
                return;
            }

            if (cn_timer_context.timer_write_pipe) {
                if (write(cn_timer_context.timer_write_pipe, timer_item_p, sizeof(cn_timer_item_t)) == sizeof(cn_timer_item_t)) {
                    return;
                }
            } else {
                CN_LOG_E("no pipe for writing!");
            }
            break;
        }
        default:
            break;
    }

    return;
}

static timer_t cn_timer_create_new_timer(cn_timer_item_t *timer_item_p)
{
    struct sigaction saction;
    struct sigevent sevent;
    int result;
    timer_t timer_id;


    if (1 == cn_timer_get_nr_of_timers()) {
        /* Establish signal handler */
        memset(&saction, 0, sizeof(saction));
        saction.sa_flags = SA_SIGINFO;
        saction.sa_sigaction = cn_timer_handler;
        sigemptyset(&saction.sa_mask);
        result = sigaction(SIGALRM, &saction, NULL);

        if (result < 0) {
            CN_LOG_E("sigaction ERROR (%s)", strerror(errno));
            goto error;
        }
    }

    /* Create timer */
    sevent.sigev_notify = SIGEV_SIGNAL;
    sevent.sigev_signo = SIGALRM;
    sevent.sigev_value.sival_ptr = timer_item_p;
    result = timer_create(CLOCK_REALTIME, &sevent, &timer_id);

    if (result < 0) {
        CN_LOG_E("timer_create ERROR (%s)", strerror(errno));
        goto cleanup_signal;
    }

    return timer_id;

cleanup_signal:
    signal(SIGALRM, SIG_IGN);

error:
    return -1;
}

int cn_timer_start(int timeout, cn_timer_callback_t timer_callback, cn_timer_callback_data_t *callback_data_p)
{
    struct itimerspec its;
    timer_t timer_id;
    int tv_sec = 0;
    int tv_nsec = 0;
    cn_timer_item_t *timer_item_p = NULL;
    int result = 0;


    timer_item_p = cn_timer_get_item(timer_callback);

    if (NULL != timer_item_p) {
        CN_LOG_W("timer callback already registered");
        goto error;
    }

    timer_item_p = cn_timer_create_new_item();

    if (NULL == timer_item_p) {
        CN_LOG_E("new timer item can't be created");
        goto error;
    }

    timer_id = cn_timer_create_new_timer(timer_item_p);

    if (timer_id < 0) {
        CN_LOG_E("new timer can't be started");
        goto remove_timer_item;
    }

    timer_item_p->timer_id = timer_id;
    timer_item_p->timer_callback = timer_callback;

    if (NULL == callback_data_p || NULL == callback_data_p->data_p) {
        timer_item_p->data_p = NULL;
    } else if (NULL != callback_data_p->data_p && 0 == callback_data_p->size_of_data) {
        CN_LOG_E("invalid timer callback data");
        goto cleanup_timer;
    } else {
        timer_item_p->data_p = calloc(1, callback_data_p->size_of_data);

        if (NULL == timer_item_p->data_p) {
            CN_LOG_E("memory allocation failed for timer item data");
            goto cleanup_timer;
        }

        memmove(timer_item_p->data_p, callback_data_p->data_p, callback_data_p->size_of_data);
    }

    /* Start the timer */
    if (timeout >= 1000) {
        tv_sec = timeout / 1000;
        tv_nsec = (timeout - (tv_sec * 1000)) * 1000;
    } else {
        tv_sec = 0;
        tv_nsec = timeout * 1000;
    }

    its.it_value.tv_sec = tv_sec;
    its.it_value.tv_nsec = tv_nsec;
    its.it_interval.tv_sec = tv_sec;
    its.it_interval.tv_nsec = tv_nsec;
    result = timer_settime(timer_item_p->timer_id, 0, &its, NULL);

    if (result < 0) {
        CN_LOG_E("timer_settime ERROR (%s)", strerror(errno));
        goto cleanup_timer;
    }

    return 0;

cleanup_timer:
    timer_delete(timer_item_p->timer_id);

    if (1 == cn_timer_get_nr_of_timers()) {
        signal(SIGALRM, SIG_IGN);
    }

remove_timer_item:
    cn_timer_remove_item(timer_item_p);

error:
    return -1;
}

int cn_timer_stop(cn_timer_callback_t timer_callback)
{
    cn_timer_item_t *timer_item_p = NULL;


    if (NULL == timer_callback) {
        CN_LOG_E("timer callback is NULL");
        goto error;
    }

    timer_item_p = cn_timer_get_item(timer_callback);

    if (NULL == timer_item_p) {
        CN_LOG_E("no timer is running for given callback");
        goto error;
    }

    if (timer_item_p->timer_id >= 0) {
        timer_delete(timer_item_p->timer_id);
        timer_item_p->timer_id = -1;
    }

    if (!cn_timer_remove_item(timer_item_p)) {
        CN_LOG_E("can't remove timer item");
    }

    if (0 == cn_timer_get_nr_of_timers()) {
        signal(SIGALRM, SIG_IGN);
    }

    return 0;

error:
    return -1;
}

static int cn_timer_callback(const int fd, const void *context)
{
    int read_size = 0;
    char read_buf[sizeof(cn_timer_item_t)];
    cn_timer_context_t *cn_timer_context_p = (cn_timer_context_t *)context;


    if (NULL == cn_timer_context_p) {
        CN_LOG_E("context is NULL");
        return -1;
    }

    read_size = read(fd, read_buf, sizeof(cn_timer_item_t));

    if (read_size != sizeof(cn_timer_item_t)) {
        CN_LOG_E("read failed");
        return -1;
    }

    cn_timer_item_t *timer_item_p = (cn_timer_item_t *)read_buf;

    if (NULL == timer_item_p->timer_callback) {
        CN_LOG_E("timer callback is NULL");
        return -1;
    }

    (void)timer_item_p->timer_callback(timer_item_p->data_p);

    return TRUE;
}

static int cn_timer_callback_remove(const int fd, const void *context)
{
    cn_timer_context_t *cn_timer_context_p = (cn_timer_context_t *)context;
    (void)fd;

    if (NULL == cn_timer_context_p) {
        CN_LOG_E("context is NULL");
        return -1;
    }

    while (NULL != cn_timer_item_p) {
         if (cn_timer_stop(cn_timer_item_p->timer_callback) < 0) {
            CN_LOG_E("stop timer error in cn_timer_callback_remove");
         }
    }
    /* Close pipes */
    if (cn_timer_context_p->timer_read_pipe) {
        close(cn_timer_context_p->timer_read_pipe);
    }

    if (cn_timer_context_p->timer_write_pipe) {
        close(cn_timer_context_p->timer_write_pipe);
    }

    return TRUE;
}

