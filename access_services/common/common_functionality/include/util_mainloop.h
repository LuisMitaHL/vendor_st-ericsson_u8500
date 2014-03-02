/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef UTIL_MAINLOOP_H
#define UTIL_MAINLOOP_H (1)

#include <stdbool.h>
#include <unistd.h>
#include <util_general.h>

/* Types */

/**
 *  @enum util_mainloop_watch_event_t
 *  @brief Watch event types
 *  @n Defines the different watch event types
 */
typedef enum {
    EVENT_READ = 0,
    EVENT_WRITE,
    EVENT_EXCEPTION
} util_mainloop_watch_event_t;

/**
 * @brief Callback function called when a watch event occurs.
 * @param fd        File descriptor
 * @param context   Context for callback
 * @return          Result code from callback
 *
 */
typedef int util_mainloop_watch_callback_t(const int fd, const void *context);


typedef int util_mainloop_state_callback_t(const void *context);


/**
 * @struct util_mainloop_watch_callback_t
 * @brief Module test hooks
 * @n Set of module test hooks that will be triggered
 *    during module test mode.
 */
typedef struct {
    void (*on_watch_event_triggered)(void);
} util_mainloop_moduletest_hooks_t;

int util_mainloop_prepare(const void *context, util_mainloop_state_callback_t *prepared_cb,
    util_mainloop_state_callback_t *stopped_cb);

void util_mainloop_loop(void) __attribute__((__noreturn__));

int util_mainloop_quit(void);

int util_mainloop_set_moduletest_mode(bool ignore_bad_fds,
                                    util_mainloop_moduletest_hooks_t *hooks);

int util_mainloop_add_watch(const util_mainloop_watch_event_t type, const int fd,
                const void *context, util_mainloop_watch_callback_t *trigger_cb,
                util_mainloop_watch_callback_t *cleanup_cb);

int util_mainloop_remove_watch(const util_mainloop_watch_event_t type, const int fd);




#endif        /* #ifndef UTIL_MAINLOOP_H */
