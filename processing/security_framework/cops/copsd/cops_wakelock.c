/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "cops_common.h"
#include "cops_wakelock.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>

static int cops_do_wakelock_release(void);

#define WAKE_LOCK_PATH "/sys/power/wake_lock"
#define WAKE_UNLOCK_PATH "/sys/power/wake_unlock"
#define WAKE_LOCK_STRING "cops"

/* This will be set to true if the product supports wakelocks */
static bool has_wakelock = false;
/* When the wakelock has been taken, this will be true */
static bool wakelock_activated = false;
/* filedescriptor for the wakelock file */
static int fd_wakelock = -1;
/* filedescriptor for the wakeunlock file */
static int fd_wakeunlock = -1;

/**
 *   Do the sysfs call to release wakelock.
 *
 *   @return 1 if successful, 0 if unsuccessful.
 */
static int cops_do_wakelock_release(void)
{
    int errnum;
    int success = 0;

    if (fd_wakeunlock < 0) {
        has_wakelock = false;
        wakelock_activated = false;
        COPS_LOG(LOG_INFO, "Disabling wakelocks\n");
    } else {
        ssize_t written = write(fd_wakeunlock, WAKE_LOCK_STRING,
                                sizeof(WAKE_LOCK_STRING) - 1);

        if (written < 0) {
            errnum = errno;
            COPS_LOG(LOG_ERROR, "write() errno=%d (%s)\n",
                     errnum, strerror(errnum));
        } else {
            wakelock_activated = false;
            success = 1;
        }
    }

    return success;
}

/**
 *  Check whether wake locks exist at all, and is writeable. Open fd's before
 *  dropping privileges.
 *  @return Nothing
 */
void cops_wakelock_init(void)
{
    int errnum;

    has_wakelock = false;

    if (access(WAKE_LOCK_PATH, W_OK) < 0) {
        errnum = errno;
        COPS_LOG(LOG_ERROR, "Access(%s, W_OK) errno=%d (%s)\n",
                 WAKE_LOCK_PATH, errnum, strerror(errnum));
    } else if (access(WAKE_UNLOCK_PATH, W_OK) < 0) {
        errnum = errno;
        COPS_LOG(LOG_ERROR, "Access(%s, W_OK) errno=%d (%s)\n",
                 WAKE_UNLOCK_PATH, errnum, strerror(errnum));
    } else {
        fd_wakeunlock = open(WAKE_UNLOCK_PATH, O_WRONLY, 0);
        fd_wakelock = open(WAKE_LOCK_PATH, O_WRONLY, 0);
        if (fd_wakelock > 0 && fd_wakeunlock > 0) {
            COPS_LOG(LOG_INFO, "Wakelock initiated\n");
            has_wakelock = true;
            (void)cops_do_wakelock_release();
        }
    }
}

/**
 *  Close and clean up everything.
 *
 *  @return Nothing
 */
void cops_wakelock_final(void)
{
    (void)cops_do_wakelock_release();
    (void)close(fd_wakelock);
    (void)close(fd_wakeunlock);
    has_wakelock = false;
}

/**
 *   Acquire wakelock to prevent platform from sleeping.
 *
 *   May be called repeatedly without first calling release_wakelock().
 *
 *   @return 1 if successful, 0 if unsuccessful.
 */
int cops_wakelock_acquire(void)
{
    int success = 0;
    int errnum;

    if (!has_wakelock) {
        return 1;
    }

    if (wakelock_activated) {
        /* Wake lock is already in place */
        return 1;
    }

    if (fd_wakeunlock < 0) {
        has_wakelock = false;
        COPS_LOG(LOG_INFO, "Disabling wakelocks\n");
    } else {
        ssize_t written = write(fd_wakelock, WAKE_LOCK_STRING,
                                sizeof(WAKE_LOCK_STRING) - 1);

        if (written < 0) {
            errnum = errno;
            COPS_LOG(LOG_ERROR, "write() errno=%d (%s)\n",
                     errnum, strerror(errnum));
        } else {
            wakelock_activated = true;
            success = 1;
        }

    }

    return success;
}


/**
 *   Release wakelock to allow platform to sleep.
 *
 *   @return 1 if successful, 0 if unsuccessful.
 */
int cops_wakelock_release(void)
{

    if (!has_wakelock)
        return 1;

    if (!wakelock_activated)
        return 0;

    return cops_do_wakelock_release();
}

/**
 *   Check if we have a wakelock or not
 *
 *   @return 1 if active, 0 if inactive (no wakelock).
 */
int cops_wakelock_active(void)
{
    return wakelock_activated;
}
