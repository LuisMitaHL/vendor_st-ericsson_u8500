/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdbool.h>
#include <stdlib.h>
#include <sys/select.h>
#include <errno.h>

#include "atc.h"
#include "atc_log.h"
#include "atc_selector.h"

static selector_item_t selector_head;

static const int INVALID_FD = -1;
static const int MARKED_FOR_DELETION = -2;

/* --------------------------------------------------------------
 * Initialize the selector.
 *
 * This function creates and initializes the head for the list
 * (actually a ring) used to store all the registered callbacks
 * and FDs.
 *
 * +----+
 * |HEAD|---+ next
 * +----+   |
 *    ^     |
 *    |     |
 *    +-----+
 *
 * --------------------------------------------------------------*/
bool selector_init(void)
{
    selector_head.callback = NULL;
    selector_head.fd = INVALID_FD;
    selector_head.data_p = NULL;
    selector_head.next = &selector_head;
    return true;
}

/* --------------------------------------------------------------
 * Return true if the fd exist in the ring (list).
 * --------------------------------------------------------------*/
static bool selector_fd_exists(int fd)
{
    bool found = false;
    selector_item_t *item_p = &selector_head;

    do {
        if (item_p->fd == fd) {
            found = true;
            break;
        }

        item_p = item_p->next;
    } while (!found && item_p != &selector_head);

    return found;
}

/* --------------------------------------------------------------
 * Adds an fd, callback, data triplet to the selector ring.
 *
 * +----+ next  +----+
 * |HEAD|------>| fd |---+ next
 * +----+       +----+   |
 *    ^                  |
 *    |                  |
 *    +------------------+
 *
 * Will return false if a callback is already registered for the
 * fd.
 * --------------------------------------------------------------*/
bool selector_register_callback_for_fd(int fd, callback_t callback, void *data_p)
{
    selector_item_t *item_p;

    if (NULL == callback) {
        ATC_LOG_E("No callback provided!");
        return false;
    }

    if (selector_fd_exists(fd)) {
        ATC_LOG_E("File descriptor %d already registered!", fd);
        return false;
    }

    if (0 > fd || FD_SETSIZE <= fd) {
        ATC_LOG_E("File descriptor %d is out of range (0-%d)!", fd, FD_SETSIZE - 1);
        return false;
    }

    item_p = malloc(sizeof(selector_item_t));

    if (NULL == item_p) {
        ATC_LOG_E("malloc failed!");
        return false;
    }

    item_p->callback = callback;
    item_p->fd = fd;
    item_p->data_p = data_p;
    item_p->next = selector_head.next;
    selector_head.next = item_p;

    ATC_LOG_D("fd = %d", fd);

    return true;
}

/* --------------------------------------------------------------
 * Marks an fd, callback, data triplet for deletion.
 *
 * +----+ next  +----+       +----+       +----+       +----+
 * |HEAD|------>| 03 |------>| -2 |------>| 07 |------>| 04 |---+
 * +----+       +----+       +----+       +----+       +----+   |
 *    ^                      To Be                              |
 *    |                      Deleted                            |
 *    +---------------------------------------------------------+
 *
 * --------------------------------------------------------------*/
bool selector_deregister_callback_for_fd(int fd)
{
    bool result = false;
    selector_item_t *item_p = &selector_head;

    if (fd > INVALID_FD) {
        do {
            if (item_p->fd == fd) {
                item_p->fd = MARKED_FOR_DELETION;
                result = true;
            }

            item_p = item_p->next;
        } while (!result && item_p != &selector_head);
    }

    ATC_LOG_D("fd = %d, result = %d", fd, result);

    return result;
}

/* --------------------------------------------------------------
 * Delete all elements marked for deletion in the ring.
 * --------------------------------------------------------------*/
static void selector_cleanup(void)
{
    selector_item_t *item_p = &selector_head;
    selector_item_t *to_be_freed = NULL;

    do {
        if (MARKED_FOR_DELETION == item_p->next->fd) {
            to_be_freed = item_p->next;
            item_p->next = to_be_freed->next;
            free(to_be_freed);
        }

        item_p = item_p->next;
    } while (item_p != &selector_head);
}

/* --------------------------------------------------------------
 * Build a list of FDs for select()
 * --------------------------------------------------------------*/
static int selector_build_list(fd_set *set)
{
    selector_item_t *item_p = &selector_head;
    int max_fd = INVALID_FD;
    FD_ZERO(set);

    do {
        if (item_p->fd > INVALID_FD) {
            FD_SET(item_p->fd, set);
            max_fd = MAX(max_fd, item_p->fd);
        }

        item_p = item_p->next;
    } while (item_p != &selector_head);

    return max_fd;
}

/* --------------------------------------------------------------
 * Selector main loop
 *
 * This function waits on select and calls the registered callback
 * for each fd that is  marked in the set after a call to select.
 *
 * It will do so until one of the callbacks returns false.
 * If false is returned the associated file-descriptor will
 * be marked for deletion.
 *
 * If any of the callbacks have called
 * selector_deregister_callback_for_fd() the corresponding entry
 * will be freed after all callbacks have been called (to avoid
 * modifying the ring while it is used).
 * --------------------------------------------------------------*/
void selector_loop(callback_t default_callback, void *data_p)
{
    bool cont = true;
    selector_item_t *item_p;
    fd_set read_fds;
    int nbr_of_fds;
    int max_fd;

    while (cont) {
        max_fd = selector_build_list(&read_fds);

        if (max_fd > INVALID_FD) {
            nbr_of_fds = select(max_fd + 1, &read_fds, NULL, NULL, NULL);

            if (nbr_of_fds >= 0) {
                item_p = &selector_head;

                do {
                    if (item_p->fd > INVALID_FD && FD_ISSET(item_p->fd, &read_fds)) {
                        cont = item_p->callback(item_p->fd, item_p->data_p);

                        if (!cont) {
                            /* Mark item for deletion */
                            item_p->fd = MARKED_FOR_DELETION;
                            break;
                        }
                    }

                    item_p = item_p->next;
                } while (item_p != &selector_head);
            } else {
                int errnum = errno;

                switch (errnum) {
                case EBADF:
                    ATC_LOG_E("EBADF - Bad file descriptor!");
                    break;
                case EINTR:
                    /* A child probably died */
                    default_callback(-1, data_p);
                    break;
                case EINVAL:
                    ATC_LOG_E("EINVAL - Invalid arguments!");
                    break;
                case ENOMEM:
                    ATC_LOG_E("ENOMEM - Out of memory!");
                    break;
                default:
                    ATC_LOG_E("%d - %s", errnum, strerror(errnum));
                    break;
                }
            }
        } else {
            cont = false;
        }

        selector_cleanup();
    }
}


/*--------------------------------------------------------------
 * Default callback for the selector.
 * This function is called when select returns, but no file
 * descriptor can be found. The variable fd is -1.
 * -------------------------------------------------------------*/
bool selector_default_callback(int fd, void *data_p)
{
    /* A child probably died */
    return false;
}
