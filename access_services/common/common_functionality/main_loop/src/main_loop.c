/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdbool.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>


#include <util_general.h>
#include <util_log.h>

#include "util_mainloop.h"


/**
 * @brief Specifies the properties associated with a monitored file descriptor.
 *
 */
struct element_s {
    struct element_s *next;                   /* Pointer to next element in list */
    int               fd;                     /* File descriptor */
    void             *context_p;                /* Callback context */
    util_mainloop_watch_callback_t *event_cb; /* Event callback */
    util_mainloop_watch_callback_t *remove_cb;/* Remove callback */
};
typedef struct element_s element_t;

typedef struct {
    bool is_module_mode;
    bool ignore_bad_fds;

    /* Hooks */
    void (*on_watch_event_triggered)(void);
} module_test_data_t;

typedef int iterator_action_t(element_t *element, element_t *prev_element, const void *context);

/* Messages passed to the main thread. when called from other threads */
typedef enum {
    PIPE_EVENT_STOP = 1,
    PIPE_EVENT_ADD,
    PIPE_EVENT_REMOVE,
    PIPE_EVENT_INVALID /* Must be last */
} pipe_event_t;

/* Basic data needed to syncronize while processing the event.
 * All the argument structs below have this initial layout. */

#define EVENT_BASE_ARGS_MEMBERS \
bool event_processed;  /* Set to true when the main thread has processed the event */ \
int result;            /* The result */                                               \
pthread_mutex_t mutex; /* Mutex used to protect the event_processed variable */      \
pthread_cond_t cond   /* Signalled when event_processed is modified */

typedef struct {
    EVENT_BASE_ARGS_MEMBERS;
} pipe_event_base_args_t;

typedef struct {
    EVENT_BASE_ARGS_MEMBERS;
    element_t **list_pp;
    int fd;
    void *context_p;
    util_mainloop_watch_callback_t *trigger_cb;
    util_mainloop_watch_callback_t *cleanup_cb;
} add_element_args_t;

typedef struct {
    EVENT_BASE_ARGS_MEMBERS;
    element_t **list_pp;
    int fd;
} remove_element_args_t;

/* Iteration action arguments */

typedef struct {
    const int fd;
    element_t **found_element_pp;
    element_t **prev_element_pp;
} action_find_args_t;

typedef struct {
    fd_set *set_p;
    int    *max_fd_p;
} action_populate_fds_args_t;

typedef struct {
    element_t **list_pp;
} action_prune_bad_fd_args_t;

typedef struct {
    fd_set *set_p;
    element_t **list_pp;
} action_find_selected_element_args_t;


/* Static local data */
static int s_read_pipe = -1, s_write_pipe = -1;
static element_t *s_read_list_anchor = NULL;
static element_t *s_write_list_anchor = NULL;
static element_t *s_exception_list_anchor = NULL;
static bool s_stop = false;
static pthread_mutex_t s_pipe_mutex = PTHREAD_MUTEX_INITIALIZER;

static module_test_data_t s_module_test_data = {false, false, NULL};
static util_mainloop_state_callback_t *s_stopped_cb = NULL;
static void *s_stopped_cb_context = NULL;

/* private methods */
static bool is_main_thread() {

#ifndef ANDROID
    return syscall(SYS_gettid) == getpid();
#else
    return gettid() == getpid();
#endif /* ANDROID_OS */
}

static int cleanup(void);

static int send_add_element_event(element_t **list, const int fd, const void *context,
    util_mainloop_watch_callback_t *trigger_cb, util_mainloop_watch_callback_t *cleanup_cb);

static int send_remove_element_event(element_t **list, const int fd);

static int read_from_pipe(const int fd, const void *context);

static int write_to_pipe(const pipe_event_t event, pipe_event_base_args_t *base_args);

static int add_element(element_t **list, const int fd, const void *context,
    util_mainloop_watch_callback_t *trigger_cb, util_mainloop_watch_callback_t *cleanup_cb);

static int remove_element(element_t **list, const int fd);

static int process_next_event(/*element_t **elementp, */long timeout);

static int for_each_element(element_t *list, iterator_action_t *action_cb,
                                                                const void *context);

static int find_element_action(element_t *element, element_t *prev_element, const void *context);

static int populate_fdset_action(element_t *element, element_t *prev_element, const void *context);

static int prune_bad_fd_action(element_t *element, element_t *prev_element, const void *context);

static int find_selected_element_action(element_t *element,
                                element_t *prev_element, const void *context);




int util_mainloop_prepare(const void *context, util_mainloop_state_callback_t *prepared_cb,
                                                    util_mainloop_state_callback_t *stopped_cb) {
    int pipefd[2];
    int errnum, flags;

    /* Create a pipe that can be used to wake us up when we are waiting
     * for an event to occur on any of the monitored file descriptors.
     * See the util_mainloop_quit() function.
     */
    if (pipe(pipefd) < 0) {
        UTIL_LOG_E("pipe() failed, errno=%d", errno);
        goto error_return;
    }

    s_read_pipe = pipefd[0];
    flags = fcntl(s_read_pipe, F_GETFD, 0);

    if (flags < 0 || fcntl(s_read_pipe, F_SETFD, flags | FD_CLOEXEC) < 0) {
        errnum = errno;
        UTIL_LOG_E("fcntl error(%d)=\"%s\"", errnum, strerror(errnum));
        goto cleanup_return;
    }

    s_write_pipe = pipefd[1];
    flags = fcntl(s_write_pipe, F_GETFD, FD_CLOEXEC);

    if (flags < 0 || fcntl(s_write_pipe, F_SETFD, flags | FD_CLOEXEC) < 0) {
        errnum = errno;
        UTIL_LOG_E("fcntl error(%d)=\"%s\"", errnum, strerror(errnum));
        goto cleanup_return;
    }

    if (add_element(&s_read_list_anchor, s_read_pipe, NULL, read_from_pipe, NULL) < 0) {
        UTIL_LOG_E("Could not add read pipe to read list");
        goto cleanup_return;
    }

    s_stopped_cb = stopped_cb;
    s_stopped_cb_context = (void *)context;

    if (prepared_cb && prepared_cb(context) < 0) {
        goto cleanup_return;
    }

    return 0;

cleanup_return:
    cleanup();

error_return:
    return -1;
}

void util_mainloop_loop(void) {
    int result = 0;

    if (!is_main_thread()) {
        UTIL_LOG_E("Must be called from the main thread!");
        result = -1;
        goto exit;
    }

    while (!s_stop) {
        UTIL_LOG_D("Before event");
        result = process_next_event(-1);
        UTIL_LOG_D("After event");
        if (result < 0) {
            UTIL_LOG_E("Failed to wait for next event!");
            break;
        }

    }

    /* cleanup lists */
    cleanup();

    /* Notify that the main loop has stopped */
    if (s_stopped_cb != NULL)
        s_stopped_cb(s_stopped_cb_context);

exit:
    if (result < 0)
        exit(EXIT_FAILURE);
    else
        exit(EXIT_SUCCESS);


}

int util_mainloop_quit(void) {

    UTIL_LOG_D("Stopping");

    return write_to_pipe(PIPE_EVENT_STOP, NULL);
}

int util_mainloop_set_moduletest_mode(bool ignore_bad_fds,
                                    util_mainloop_moduletest_hooks_t *hooks) {

    if (!is_main_thread()) {
        UTIL_LOG_E("Must be called from the main thread!");
        return -1;
    }

    s_module_test_data.is_module_mode = true;
    s_module_test_data.ignore_bad_fds = ignore_bad_fds;

    if (hooks != NULL) {
        s_module_test_data.on_watch_event_triggered = hooks->on_watch_event_triggered;
    }

    return 0;

}

int util_mainloop_add_watch(const util_mainloop_watch_event_t type, const int fd, const void *context,
        util_mainloop_watch_callback_t *trigger_cb, util_mainloop_watch_callback_t *cleanup_cb) {

    int result = -1;
    element_t **list_pp = NULL;

    /* Test that the file/pipe/socket descriptor is not bogus */
    if (0 > fd || FD_SETSIZE <= fd) {
        UTIL_LOG_E("out-of-range descriptor fd = %d, rejected!", fd);
        goto error_return;
    }

    if (!s_module_test_data.ignore_bad_fds) {
        if (fcntl(fd, F_GETFL, 0) < 0 && errno == EBADF) {
            UTIL_LOG_E("invalid descriptor fd = %d, rejected!", fd);
            goto error_return;
        }
    }

    switch (type) {
    case EVENT_READ:
        UTIL_LOG_D("Adding fd to the read list: %d", fd);
        list_pp = &s_read_list_anchor;
        break;
    case EVENT_WRITE:
        UTIL_LOG_D("Adding fd to the write list: %d", fd);
        list_pp = &s_write_list_anchor;
        break;
    case EVENT_EXCEPTION:
        UTIL_LOG_D("Adding fd to the exception list: %d", fd);
        list_pp = &s_exception_list_anchor;
        break;
    default:
        /* Shouldn't happen */
        UTIL_LOG_E("Tried to add an unknown watch type: %d", fd);
        goto error_return;
    }

    if (is_main_thread()) {
        result = add_element(list_pp, fd, context, trigger_cb, cleanup_cb);
    } else {
        result = send_add_element_event(list_pp, fd, context, trigger_cb, cleanup_cb);
    }

    return result;

error_return:
    return -1;
}

int util_mainloop_remove_watch(const util_mainloop_watch_event_t type, const int fd) {

    int result = -1;
    element_t **list_pp = NULL;

    switch (type) {
    case EVENT_READ:
        UTIL_LOG_D("Removing fd from the read list: %d", fd);
        list_pp = &s_read_list_anchor;
        break;
    case EVENT_WRITE:
        UTIL_LOG_D("Removing fd from the write list: %d", fd);
        list_pp = &s_write_list_anchor;
        break;
    case EVENT_EXCEPTION:
        UTIL_LOG_D("Removing fd from the exception list: %d", fd);
        list_pp = &s_exception_list_anchor;
        break;
    default:
        /* Shouldn't happen */
        UTIL_LOG_E("Tried to remove an unknown watch type: %d", fd);
        goto error_return;
    }

    if (is_main_thread()) {
        result = remove_element(list_pp, fd);
    } else {
        result = send_remove_element_event(list_pp, fd);
    }

    return result;

error_return:
    return -1;

}

/* Private Methods */
static int cleanup(void) {

    while (s_read_list_anchor != NULL)
        remove_element(&s_read_list_anchor, s_read_list_anchor->fd);

    while (s_write_list_anchor != NULL)
        remove_element(&s_write_list_anchor, s_exception_list_anchor->fd);

    while (s_exception_list_anchor != NULL)
        remove_element(&s_exception_list_anchor, s_exception_list_anchor->fd);

    /* Close pipes */
    if (s_read_pipe > 0) {
        close(s_read_pipe);
    }

    if (s_write_pipe > 0) {
        close(s_write_pipe);
    }

    return 0;
}

static int send_add_element_event(element_t **list_pp, const int fd, const void *context,
    util_mainloop_watch_callback_t *trigger_cb, util_mainloop_watch_callback_t *cleanup_cb) {

    int result;
    add_element_args_t args;
    args.list_pp = list_pp;
    args.fd = fd;
    args.context_p = (void *)context;
    args.trigger_cb = trigger_cb;
    args.cleanup_cb = cleanup_cb;

    result = write_to_pipe(PIPE_EVENT_ADD, (pipe_event_base_args_t *)&args);
    if (result < 0) {
        UTIL_LOG_E("Failed to write ADD event to pipe");
    } else {
        result = args.result;
    }

    return result;
}

static int send_remove_element_event(element_t **list_pp, const int fd) {
    int result;
    remove_element_args_t args;
    args.list_pp = list_pp;
    args.fd = fd;

    result = write_to_pipe(PIPE_EVENT_REMOVE, (pipe_event_base_args_t *)&args);
    if (result < 0) {
        UTIL_LOG_E("Failed to write REMOVE event to pipe");
    } else {
        result = args.result;
    }

    return result;
}

static int read_from_pipe(const int fd, const void *context) {

    int result = -1;
    size_t read_bytes = 0;
    pipe_event_t event = PIPE_EVENT_INVALID;
    pipe_event_base_args_t *base_args = NULL;

    (void)context;

    if (fd != s_read_pipe) {
        UTIL_LOG_E("Got wrong fd!");
        return -1;
    }

    /* read the event number */
    while (read_bytes < sizeof(pipe_event_t)) {
        result = read(s_read_pipe, (void*)&event, sizeof(pipe_event_t));
        if (result < 0 && errno != EINTR) {
            UTIL_LOG_E("Fatal error reading event from pipe.  error: %s", strerror(errno));
            goto exit;
        }

        read_bytes = (size_t)result;
    }

    switch (event) {
    case PIPE_EVENT_STOP:
        UTIL_LOG_D("Got stop event!");
        s_stop = true;
        goto exit;
    case PIPE_EVENT_ADD: /* fallthrough */
    case PIPE_EVENT_REMOVE:
        break;
    default:
        UTIL_LOG_E("Unknown event!");
        result = -1;
        goto exit;
    }


    /* read the arguments */
    read_bytes = 0;
    while (read_bytes < (int)sizeof(pipe_event_base_args_t *)) {
        result = read(s_read_pipe, (void*)&base_args, sizeof(pipe_event_base_args_t *));
        if (result < 0 && errno != EINTR) {
            UTIL_LOG_E("Fatal error reading args from pipe.  error: %s", strerror(errno));
            goto exit;
        }
        read_bytes = (size_t)result;
    }

    /* process the event */
    result = pthread_mutex_lock(&base_args->mutex);
    if (result != 0) {
        UTIL_LOG_E("Failed to lock mutex!");
    }

    if (event == PIPE_EVENT_ADD) {
        add_element_args_t *args = (add_element_args_t *)base_args;
        args->result = add_element(args->list_pp, args->fd, args->context_p,
                                    args->trigger_cb, args->cleanup_cb);
    } else {
        remove_element_args_t *args = (remove_element_args_t *)base_args;
        args->result = remove_element(args->list_pp, args->fd);
    }

    base_args->event_processed = true;
    pthread_mutex_unlock(&base_args->mutex);

    result = pthread_cond_signal(&base_args->cond);
    if (result < 0) {
        UTIL_LOG_E("Failed to signal condition variable!");
    }

exit:
    return result;

}

static int write_to_pipe(const pipe_event_t event, pipe_event_base_args_t *base_args) {

    int result = -1;
    size_t written_bytes = 0;

    if (!s_write_pipe) {
        UTIL_LOG_E("No pipe to write to!");
        result = -1;
        goto exit;
    }

    if (event != PIPE_EVENT_STOP && !base_args) {
        UTIL_LOG_E("No arguments passed for event: %d", event);
        result = -1;
        goto exit;
    }

    result = pthread_mutex_lock(&s_pipe_mutex);
    if (result != 0) {
        UTIL_LOG_E("Failed to lock mutex!");
        goto exit;
    }

    /* Start by writing the event number */
    while (written_bytes < (int)sizeof(pipe_event_t)) {
        result = write(s_write_pipe, (void*)&event, sizeof(pipe_event_t));
        if (result < 0 && errno != EINTR) {
            UTIL_LOG_E("Fatal error writing event to pipe.  error: %s", strerror(errno));
            goto unlock_and_exit;
        }
        written_bytes = (size_t)result;
    }

    switch (event) {
    case PIPE_EVENT_STOP:
        result = 0;
        goto unlock_and_exit;
    case PIPE_EVENT_ADD:  /* fallthrough */
    case PIPE_EVENT_REMOVE:
        break;
    default:
        UTIL_LOG_E("Unknown event!");
        result = -1;
        goto unlock_and_exit;
    }

    /* Initialize the base parameters */
    base_args->event_processed = false;
    base_args->result = -1;
    pthread_mutex_init(&base_args->mutex, NULL);
    pthread_cond_init(&base_args->cond, NULL);

    /* Write the arguments to the pipe */
    written_bytes = 0;
    while (written_bytes < (int)sizeof(pipe_event_base_args_t *)) {
        result = write(s_write_pipe, base_args, sizeof(pipe_event_base_args_t *));
        if (result < 0 && errno != EINTR) {
            UTIL_LOG_E("Fatal error writing args to pipe.  error: %s", strerror(errno));
            goto cleanup;
        }
        written_bytes = (size_t)result;
    }

    /* unlock the pipe mutex */
    pthread_mutex_unlock(&s_pipe_mutex);


    /* wait for a response */
    result = pthread_mutex_lock(&base_args->mutex);
    if (result != 0) {
        UTIL_LOG_E("Failed to lock response mutex");
        goto cleanup;
    }

    while (!base_args->event_processed){
        result = pthread_cond_wait(&base_args->cond, &base_args->mutex);
        if (result != 0) {
            UTIL_LOG_E("Wait on condition variable failed");
            break;
        }
    }

    pthread_mutex_unlock(&base_args->mutex);

cleanup:
    pthread_mutex_destroy(&base_args->mutex);
    pthread_cond_destroy(&base_args->cond);
    goto exit;

unlock_and_exit:
    result = pthread_mutex_unlock(&s_pipe_mutex);
    if (result < 0) {
        UTIL_LOG_E("Failed to unlock pipe mutex!");
    }
exit:
    return result;

}

static int add_element(element_t **list, const int fd, const void *context,
    util_mainloop_watch_callback_t *trigger_cb, util_mainloop_watch_callback_t *cleanup_cb) {

    element_t *element;
    element_t *new_element;

    /* Find end of list */
    element = *list;

    if (element) {
        while (element->next != NULL) {
            element = element->next;
        }
    }

    /* Allocate new element and chain it in */
    new_element = malloc(sizeof(element_t));

    if (new_element) {
        new_element->next = NULL;
        new_element->fd = fd;
        new_element->context_p = (void *)context;
        new_element->event_cb = trigger_cb;
        new_element->remove_cb = cleanup_cb;

        if (element) {
            element->next = new_element;
        } else {
            *list = new_element;
        }

        UTIL_LOG_D("registering fd = %d, new_element = %p", fd, new_element);

        return 0;
    }

    return 1;
}

static int remove_element(element_t **list, const int fd) {
    element_t *element = NULL;
    element_t *prev_element = NULL;
    int result;

    action_find_args_t find_args = {fd, &element, &prev_element};
    result = for_each_element(*list, find_element_action, (void *)&find_args);
    if (result < 0 || element == NULL) {
        UTIL_LOG_E("Failed to find element in list.  Unable to remove");
        return result;
    }

    /* unlink this item */
    if (prev_element != NULL) {
        prev_element->next = element->next;
    } else {
        /* first element is being removed */
        *list = element->next;
    }

    if (element->remove_cb != NULL) {
        element->remove_cb(element->fd, element->context_p);
    }

    /* Release the memory, and then we are done */
    free(element);
    return 0;

}

static int process_next_event(/*element_t **elementp, */long timeout) {
    struct timeval tv, *tvp;
    int result;

     /* Set up timeout structure */
    if (timeout < 0) {
        tvp = NULL;
    } else {
        tv.tv_sec = timeout / 1000;
        tv.tv_usec = (timeout % 1000) * 1000;
        tvp = &tv;
    }

    for (;;) {
        UTIL_LOG_D("Starting to populate fdsets");
        int max_fd = 0;
        fd_set readfds, writefds, errfds;
        fd_set *readfdsp = NULL, *writefdsp = NULL, *errfdsp = NULL;

        if (s_read_list_anchor) {
            FD_ZERO(&readfds);
            action_populate_fds_args_t args = {&readfds, &max_fd};
            result = for_each_element(s_read_list_anchor, populate_fdset_action, (void *)&args);
            if (result < 0) {
                UTIL_LOG_E("Failed to populate read fds");
            } else {
                readfdsp = &readfds;
            }
        }

        if (s_write_list_anchor) {
            FD_ZERO(&writefds);
            action_populate_fds_args_t args = {&writefds, &max_fd};
            result = for_each_element(s_write_list_anchor, populate_fdset_action, (void *)&args);
            if (result < 0) {
                UTIL_LOG_E("Failed to populate write fds");
            } else {
                writefdsp = &writefds;
            }
        }

        if (s_exception_list_anchor) {
            FD_ZERO(&errfds);
            action_populate_fds_args_t args = {&errfds, &max_fd};
            result = for_each_element(s_exception_list_anchor, populate_fdset_action, (void *)&args);
            if (result < 0) {
                UTIL_LOG_E("Failed to populate exception fds");
            } else {
                errfdsp = &errfds;
            }
        }

        /* Perform the select */
        UTIL_LOG_D("Select...");
        result = select(max_fd + 1, readfdsp, writefdsp, errfdsp, tvp);
        UTIL_LOG_D("Select returned");

        if (result > 0) {

            /* Find the selected element */
            if (s_read_list_anchor) {
                action_find_selected_element_args_t args = {readfdsp, &s_read_list_anchor};
                for_each_element(s_read_list_anchor, find_selected_element_action, (void *)&args);
            }

            if (s_write_list_anchor) {
                action_find_selected_element_args_t args = {writefdsp, &s_write_list_anchor};
                for_each_element(s_write_list_anchor, find_selected_element_action, (void *)&args);
            }

            if (s_exception_list_anchor) {
                action_find_selected_element_args_t args = {errfdsp, &s_exception_list_anchor};
                for_each_element(s_exception_list_anchor, find_selected_element_action, (void *)&args);
            }

            if (s_module_test_data.is_module_mode && s_module_test_data.on_watch_event_triggered) {
                s_module_test_data.on_watch_event_triggered();
            }

            break;
        } else if (result < 0) {
            if (errno == EINTR) {
                continue;
            } else if (errno == EBADF) {
                UTIL_LOG_E("select returns: EBADF");

                /* Try to recover by identifying and purging any and all invalid fd's */
                if (s_read_list_anchor) {
                    action_prune_bad_fd_args_t args = {&s_read_list_anchor};
                    for_each_element(s_read_list_anchor, prune_bad_fd_action, &args);
                }

                if (s_write_list_anchor) {
                    action_prune_bad_fd_args_t args = {&s_write_list_anchor};
                    for_each_element(s_write_list_anchor, prune_bad_fd_action, &args);
                }

                if (s_exception_list_anchor) {
                    action_prune_bad_fd_args_t args = {&s_exception_list_anchor};
                    for_each_element(s_exception_list_anchor, prune_bad_fd_action, &args);
                }

                break;
            } else {
                /* Treat all other errors as fatal */
                int errnum = errno;
                UTIL_LOG_E("errno(%d)=\"%s\"", errnum, strerror(errnum));
                goto error_return;
            }

        } else {
            /* This should only happen if there's a timeout */
            break;
        }

    }

    return 0;

error_return:
    return -1;
}

static int for_each_element(element_t *list, iterator_action_t *action_cb, const void *context) {
    int result = 0;

    if (list) {
        element_t *next = list;
        element_t *prev = NULL;
        do {
            element_t *current = next;
            next = current->next;

            result = action_cb(current, prev, context);
            prev = current;
        } while (result > 0 && next);
    }

    return result;
}

static int find_element_action(element_t *element, element_t *prev_element, const void *context) {
    action_find_args_t *args = (action_find_args_t *)context;

    if (!element || !args->found_element_pp) {
        return -1;
    }

    if (element->fd == args->fd) {
        /* Found it */
        *args->found_element_pp = element;
        if(args->prev_element_pp) {
            *args->prev_element_pp = prev_element;
        }
        return 0; /* success  - stop the iterator */
    }

    return 1; /* continue the iterator */
}

static int populate_fdset_action(element_t *element, element_t *prev_element, const void *context) {
    action_populate_fds_args_t *args = (action_populate_fds_args_t *)context;

    (void)prev_element;

    if (!element || !args->set_p || !args->max_fd_p) {
        return -1;
    }

    FD_SET(element->fd, args->set_p);

    if (element->fd > *args->max_fd_p) {
        *args->max_fd_p = element->fd;
    }

    return 1; /* continue */
}

static int prune_bad_fd_action(element_t *element, element_t *prev_element, const void *context) {
    action_prune_bad_fd_args_t *args = (action_prune_bad_fd_args_t *)context;

    (void)prev_element;

    if (!element || !args->list_pp) {
        return -1;
    }

    if (fcntl(element->fd, F_GETFL, 0) < 0) {
        if (errno == EBADF) {
            UTIL_LOG_E("fd = %d is invalid, removing", element->fd);
            remove_element(args->list_pp, element->fd);
        }
    }

    return 1; /* continue */
}

static int find_selected_element_action(element_t *element,
                                element_t *prev_element, const void *context) {
    action_find_selected_element_args_t *args = (action_find_selected_element_args_t *)context;
    (void) prev_element;

    if (!element || !args || !args->set_p || !args->list_pp) {
        return -1;
    }


    if (FD_ISSET(element->fd, args->set_p)) {
        if (element->event_cb) {
            if (element->event_cb(element->fd, element->context_p) < 0) {
                /* Fatal error on descriptor, remove it */
                UTIL_LOG_E("element->event_cb() failed for element->fd = %d", element->fd);
                remove_element(args->list_pp, element->fd);
            }
        }
    }

    return 1; /* continue */
}
