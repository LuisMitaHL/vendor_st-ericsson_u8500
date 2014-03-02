/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * File name       : simd_timer.c
 * Description     : simd timer framework implementation
 *
 * Author          : Leif Simmons <leif.simmons@stericsson.com>
 *
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <pthread.h>

#include "simd.h"
#include "sim_unused.h"
#include "simd_timer.h"

#define SIMD_TIMER_SIGNO (SIGVTALRM) /* Signo used for timers in this implementation */
//#define SIMD_TIMER_SIGNO (SIGALRM) /* Prefered signo for timers, but may be in use in CATd */

typedef uint32_t simd_timer_s;
typedef void ste_simd_timer_callback(uintptr_t timer_event);

typedef struct {
    int read_socket;
    int write_socket;
} simd_timer_pipe_t;

typedef struct {
    uint16_t             tag;     // The tag is used for timer handle validation.
    uint16_t             event;   // The event number is used to prevent stop/timeout racing
    timer_t              timer;   // The system timer id
    simd_timer_callback* func;    // Call-back function provided when the timer was started.
    uintptr_t            data[4]; // User data provided when the timer was started
} simd_timer_info_t;

typedef struct {
    pthread_mutex_t    mutex;
    uint16_t           tag_counter;
    uint16_t           size;
    simd_timer_info_t* storage;
} simd_timer_storage_t;

static simd_timer_storage_t g_simd_timer_storage = { PTHREAD_MUTEX_INITIALIZER, 0, 0, NULL };
static simd_timer_pipe_t    g_simd_timer_pipe = {-1, -1};


/**********************************************************************************
 * Internal utility timer related functions that does not require a mutex
 * lock, but only makes minimal argument checking (if any).
 *
 * In other words:
 * - THE CALLER MUST MAKE SURE THE PARAMETERS ARE SAFE!
 *
 **********************************************************************************/

/*
 * Makes a timer handle from index & tag
 */
inline static uint32_t simd_timer_create_handle(uint16_t index, uint16_t tag)
{
    return ((((uint32_t)index) << 20) | ((uint32_t)tag) | ((uint32_t)0x000F0000));
}

/*
 * Parses the timer handle and extracts the index & tag.
 *
 * No NULL check on index & tag.
 */
inline static void simd_timer_parse_handle(uint32_t handle, uint16_t* index, uint16_t* tag)
{
    *index = (uint16_t)((handle >> 20) & 0x0FFF);
    *tag = (uint16_t)(handle & 0xFFFF);
}

/*
 * Makes a timeout event from index & event.
 *
 * Timeout events are created the same way as handles...
 */
inline static uint32_t simd_timer_create_timeout_event(uint16_t index, uint16_t event)
{
    return simd_timer_create_handle(index, event);
}

/*
 * Parses the timeout event and extracts the index & event.
 *
 * Timeout events are created the same way as handles...
 *
 * No NULL check on index & event.
 */
inline static void simd_timer_parse_timeout_event(uint32_t to_event, uint16_t* index, uint16_t* event)
{
    return simd_timer_parse_handle(to_event, index, event);
}

/*
 * Checks if a timer record is free (for use)
 *
 * No NULL check on t.
 */
inline static int simd_timer_is_timer_free(simd_timer_info_t* t)
{
    return (t->tag == 0);
}

/*
 * Creates and starts asystem timer.
 *
 * @param timeout   Timeout in millisecs
 * @param to_event  The vent numbe rhtat will be written to the timer socket.
 *
 * No NULL check on timer.
 */
static int simd_timer_systimer_create(timer_t* timer, uint32_t timeout, uint32_t to_event)
{
    int rc = -1;
    if (timer && timeout) {
        struct sigevent sev;
        sev.sigev_notify = SIGEV_SIGNAL;
        sev.sigev_signo = SIMD_TIMER_SIGNO;
        sev.sigev_value.sival_ptr = (void*)to_event;
        rc = timer_create(CLOCK_REALTIME, &sev, timer);

        if (!rc) {
            struct itimerspec its;
            its.it_value.tv_sec = (timeout / 1000);
            its.it_value.tv_nsec = ((timeout - (its.it_value.tv_sec * 1000)) * 1000);
            its.it_interval.tv_sec = 0;
            its.it_interval.tv_nsec = 0;

            rc = timer_settime(*timer, 0, &its, NULL);
        }
    }
    return rc;
}

/*
 * Deletes a previously created system timer.
 *
 * If the timer is started, but not expired, then it is canceled.
 */
static void simd_timer_systimer_delete(timer_t* timer)
{
    if (timer && *timer) {
        timer_delete(*timer);
        *timer = 0;
    }
}

static int simd_timers_create_storage(simd_timer_info_t** storage, uint16_t size)
{
    int rc = -1;
    if (storage) {
        if (size && size <= SIMD_TIMER_MAX_TIMERS) {
            *storage = calloc(size, sizeof(simd_timer_info_t));
            if (*storage) {
                int i;
                for (i = 0; i < size; i++) {
                    (*storage)[i].event = (uint16_t)((rand() >> 4) & 0xFFFF);
                }
                rc = 0;
            }

        } else {
            *storage = NULL;
        }
    }
    return rc;
}

static int simd_timers_create_pipe(simd_timer_pipe_t* apipe)
{
    int mypipe[2] = {-1, -1};
    int rc = -1;
    if (apipe) {
        int flags;
        rc = pipe(mypipe);
        if (!rc) {
            flags = fcntl(mypipe[0], F_GETFD, 0);
            if (flags < 0 || fcntl(mypipe[0], F_SETFL, flags | O_NONBLOCK) < 0) {
                rc = -1;
            }
        }
        if (!rc) {
            flags = fcntl(mypipe[1], F_GETFD, 0);
            if (flags < 0 || fcntl(mypipe[1], F_SETFL, flags | O_NONBLOCK) < 0) {
                rc = -1;
            }
        }
    }

    if (!rc) {
        apipe->read_socket = mypipe[0] ;
        apipe->write_socket = mypipe[1];
    } else {
        if (mypipe[0] != -1) {
            close(mypipe[0]);
        }
        if (mypipe[1] != -1) {
            close(mypipe[1]);
        }
    }
    return rc;
}

/**********************************************************************************
 * Internal utility timer related functions that must be called within a mutex
 * lock, but that does not lock the mutex themselves! Also, these functions only
 * makes minimal argument checking, if any.
 *
 * In other words:
 * - THE CALLER HAS TO LOCK/UNLOCK THE MUTEX!
 * - THE CALLER MUST MAKE SURE THE PARAMETERS ARE SAFE!
 *
 **********************************************************************************/

/*
 * Returns the next tag to use (for new timer handles)
 *
 * Assumes that s->mutex is locked. No NULL check on s.
 */
inline static uint16_t simd_timer_get_next_tag(simd_timer_storage_t* s)
{
    if (!(++s->tag_counter)) {
        ++s->tag_counter;
    }
    return s->tag_counter;
}

/*
 * Checks that the timer index is valid and that the event number matches the expected event
 *
 * Assumes that s->mutex is locked. No NULL check on s or index.
 *
 * Returns 1 if the event is valid, 0 if it is not valid
 */
static int simd_timer_validate_timeout(simd_timer_storage_t* s, uint32_t to_event, uint16_t* index)
{
    uint16_t dummy, event;

    if (!index) {
        index = &dummy;
    }

    simd_timer_parse_timeout_event(to_event, index, &event);
    if (s->storage && (*index < s->size)) {
        if (event == s->storage[*index].event) {
            return 1;
        }
    }
    return 0;
}

/*
 * Validates a timer handle and optionally returns the extracted storage index
 *
 * Assumes that s->mutex is locked. No NULL check on s or index.
 */
int simd_timer_validate_handle(simd_timer_storage_t* s, uint32_t handle, uint16_t* index)
{
    uint16_t dummy, tag;

    if (!index) {
        index = &dummy;
    }

    simd_timer_parse_handle(handle, index, &tag);
    if ((s->storage) && (*index < s->size) && (tag == s->storage[*index].tag)) {
        return 0;
    }
    return -1;
}

/*
 * Internal implementation of timer stop.
 *
 * Assumes mutex is locked
 */
static int simd_timer_stop_internal(simd_timer_storage_t* s, uint32_t handle)
{
    uint16_t index;
    int rc = simd_timer_validate_handle(s, handle, &index);
    if (!rc) {
        simd_timer_info_t* t = &s->storage[index];
        simd_timer_systimer_delete(&t->timer);
        t->func = NULL;
        t->data[0] = 0;
        t->data[1] = 0;
        t->data[2] = 0;
        t->data[3] = 0;
        t->event++;
    }
    return rc;
}

/**********************************************************************************
 * Internal functions
 **********************************************************************************/

/*
 * Timeout event dispatcher. Validates the timeout event and invokes the associated
 * call-back function.
 */
static void simd_dispatch_timeout_event(uint32_t to_event)
{
    simd_timer_storage_t* s = &g_simd_timer_storage;
    if (pthread_mutex_lock(&s->mutex) == 0) {
        simd_timer_callback* callback = NULL;
        uint16_t index;
        if (simd_timer_validate_timeout(s, to_event, &index)) {
            simd_timer_info_t* t = &s->storage[index];
            callback = t->func;
        }
        pthread_mutex_unlock(&s->mutex);
        if (callback) {
            callback(to_event);
        }
   }
}

/*
 * Event stream parser. Receives the data from the read-end of the pipe,
 * converts it to a timeout event and then calls the dispatcher.
 */
static ssize_t simd_timeout_parse(char* buf, char *buf_max, void* UNUSED(unused))
{
    uint32_t to_event;
    ssize_t size = sizeof(to_event);
    ssize_t consumed = 0;

    if (buf && buf_max) {           // Normal operation
        if ((buf_max - buf) >= size) {
            memcpy(&to_event, buf, size);
            simd_dispatch_timeout_event(to_event);
            consumed = size;
        }
    } else if (!buf && !buf_max) {  // ES is shutting down (as per docs for ES)
        if (g_simd_timer_pipe.read_socket >= 0) {
            close(g_simd_timer_pipe.read_socket);
            g_simd_timer_pipe.read_socket = -1;
        }
    } else {
        consumed = -1;
    }
    return consumed;
}

/*
 * Signal handler. Writes the timeout event to the write-end of the pipe.
 *
 * If modifying this function, do not invoke call-backs, use mutex, print, or anything like that!
 *
 * If needed,
 */
static void simd_timer_signalhandler(int signo, siginfo_t* info, void* context)
{
    (void)context; // Not used.

    if (signo == SIMD_TIMER_SIGNO) {
        int fd = g_simd_timer_pipe.write_socket;
        if (fd >= 0) {
            uint32_t to_event = (uint32_t)(info->si_value.sival_ptr);
            write(fd, &to_event, sizeof(to_event));
        }
    }
}

/**********************************************************************************
 * Interface implementation, thread-safe functions that LOCK THE MUTEX
 **********************************************************************************/

int simd_timer_initiate_timers(uint16_t size)
{
    simd_timer_storage_t* s = &g_simd_timer_storage;
    int rc = -1;
    time_t t;

    time(&t);
    srand((unsigned int)t);

    if (pthread_mutex_init(&s->mutex, 0) == 0) {
        if (pthread_mutex_lock(&s->mutex) == 0) {

            rc = simd_timers_create_storage(&s->storage, size);
            if (!rc) {
                s->tag_counter = ((rand() >> 4) & 0xFFFF);
                s->size = size;
                rc = simd_timers_create_pipe(&g_simd_timer_pipe);
            }
            if (!rc) {
                ste_es_t* es  = NULL;
                ste_es_parser_closure_t pc;
                pc.func = simd_timeout_parse;
                pc.user_data = NULL;
                es = ste_es_new_normal(g_simd_timer_pipe.read_socket,
                                       (size + 1) * sizeof(uint32_t),
                                       &pc);
                if (!es || catd_add_es(es)) {
                    ste_es_delete(es);
                    rc = -1;
                }
            }
            if (!rc) {
                struct sigaction sa;
                sa.sa_flags = SA_SIGINFO;
                sa.sa_sigaction = simd_timer_signalhandler;
                sigemptyset(&sa.sa_mask);
                rc = sigaction(SIMD_TIMER_SIGNO, &sa, NULL);
            }
            pthread_mutex_unlock(&s->mutex);
        }
    }
    return rc;
}

int simd_timer_shutdown_timers()
{
    simd_timer_storage_t* s = &g_simd_timer_storage;
    int rc = -1;
    if (pthread_mutex_lock(&s->mutex) == 0) {
        if (s->storage) {
            int i;
            for (i = 0; i < s->size; i++) {
                simd_timer_info_t* t = &s->storage[i];
                simd_timer_systimer_delete(&t->timer);
            }
            if (s->size) {
                memset(s->storage, 0, s->size * sizeof(simd_timer_info_t));
            }
            free(s->storage);
            s->storage = NULL;
        }

        signal(SIMD_TIMER_SIGNO, SIG_IGN);

        if (g_simd_timer_pipe.read_socket >= 0) {
            catd_rem_es(g_simd_timer_pipe.read_socket);
        }

        if (g_simd_timer_pipe.write_socket >= 0) {
            close(g_simd_timer_pipe.write_socket);
            g_simd_timer_pipe.write_socket = -1;
        }

        pthread_mutex_unlock(&s->mutex);
        pthread_mutex_destroy(&s->mutex);
        rc = 0;
    }
    return rc;
}

int simd_timer_create(uint32_t* handle)
{
    simd_timer_storage_t* s = &g_simd_timer_storage;
    int rc = -1;

    if (handle) {
        if (pthread_mutex_lock(&s->mutex) == 0) {
            if (s->storage && s->size > 0) {
                uint16_t index;
                for (index = 0; index < s->size; index++) {
                    simd_timer_info_t* t = &s->storage[index];
                    if (simd_timer_is_timer_free(t)) {
                        t->tag = simd_timer_get_next_tag(s);
                        t->func = NULL;
                        t->data[0] = 0;
                        t->data[1] = 0;
                        t->data[2] = 0;
                        t->data[3] = 0;

                        *handle = simd_timer_create_handle(index, t->tag);
                        rc = 0;
                        break;
                    }
                }

            }
            pthread_mutex_unlock(&s->mutex);
        }
    }
    return rc;
}

int simd_timer_destroy(uint32_t handle)
{
    simd_timer_storage_t* s = &g_simd_timer_storage;
    uint16_t index;
    int rc = -1;

    if (pthread_mutex_lock(&s->mutex) == 0) {
        if (simd_timer_validate_handle(s, handle, &index) == 0) {
            simd_timer_info_t* t = &s->storage[index];
            t->tag = 0;
            t->func = NULL;
            t->data[0] = 0;
            t->data[1] = 0;
            t->data[2] = 0;
            t->data[3] = 0;
            simd_timer_systimer_delete(&t->timer);
            rc = 0;

        }
        pthread_mutex_unlock(&s->mutex);
    }
    return rc;
}


/*
 * Starts the timer associated with the handle.
 *
 * LOCK THE MUTEX
 */
int simd_timer_start(uint32_t handle,
                     uint32_t timeout,
                     simd_timer_callback* callback,
                     uintptr_t userdata_0,
                     uintptr_t userdata_1,
                     uintptr_t userdata_2,
                     uintptr_t userdata_3)
{
    simd_timer_storage_t* s = &g_simd_timer_storage;
    uint16_t index, tag;
    int rc = -1;
    if (timeout && callback) {
        if (pthread_mutex_lock(&s->mutex) == 0) {
            rc = simd_timer_stop_internal(s, handle);
            if (!rc) {
                simd_timer_info_t* t;
                uint32_t to_event;

                simd_timer_parse_handle(handle, &index, &tag); // Can't fail since already validate above

                t = &s->storage[index];
                to_event = simd_timer_create_timeout_event(index, t->event);
                rc = simd_timer_systimer_create(&t->timer, timeout, to_event);
                t->func = callback;
                t->data[0] = userdata_0;
                t->data[1] = userdata_1;
                t->data[2] = userdata_2;
                t->data[3] = userdata_3;
            }
            pthread_mutex_unlock(&s->mutex);
        }
    }

    return rc;
}

int simd_timer_stop(uint32_t handle)
{
    simd_timer_storage_t* s = &g_simd_timer_storage;
    int rc = -1;
    if (pthread_mutex_lock(&s->mutex) == 0) {
        rc = simd_timer_stop_internal(s, handle);
        pthread_mutex_unlock(&s->mutex);
    }
    return rc;
}

int simd_timer_validate_timeout_event(uintptr_t timeout_event)
{
    simd_timer_storage_t* s = &g_simd_timer_storage;
    int rc = -1;
    if (pthread_mutex_lock(&s->mutex) == 0) {
        rc = simd_timer_validate_timeout(s, (uint32_t)timeout_event, NULL);
        rc = (rc == 1) ? 0 : -1;
        pthread_mutex_unlock(&s->mutex);
    }
    return rc;
}

int simd_timer_get_data_from_event(uintptr_t timeout_event,
                                   uintptr_t* userdata_0,
                                   uintptr_t* userdata_1,
                                   uintptr_t* userdata_2,
                                   uintptr_t* userdata_3)
{
    simd_timer_storage_t* s = &g_simd_timer_storage;
    int rc = -1;
    if (pthread_mutex_lock(&s->mutex) == 0) {
        uint16_t index;
        if (simd_timer_validate_timeout(s, (uint32_t)timeout_event, &index)) {
            simd_timer_info_t* t = &s->storage[index];
            if (userdata_0) {
                *userdata_0 = t->data[0];
            }
            if (userdata_1) {
                *userdata_1 = t->data[1];
            }
            if (userdata_2) {
                *userdata_2 = t->data[2];
            }
            if (userdata_3) {
                *userdata_3 = t->data[3];
            }
            rc = 0;
        }
        pthread_mutex_unlock(&s->mutex);
    }
    return rc;
}
