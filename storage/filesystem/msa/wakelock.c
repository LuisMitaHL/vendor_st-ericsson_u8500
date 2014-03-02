/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "wakelock.h"

#include "log.h"
#include "posix.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>

#define WAKE_LOCK_PATH "/sys/power/wake_lock"
#define WAKE_UNLOCK_PATH "/sys/power/wake_unlock"
#define WAKE_LOCK_STRING "msa_rpc"

static bool has_wakelock;
static bool wakelock_activated;

/**
 *   Do the sysfs call to release wakelock.
 *
 *   @return 1 if successful, 0 if unsuccessful.
 */
static int do_wakelock_release(bool quiet)
{
	int fd;
	int errnum;
	int success = 0;

	fd = posix_open(WAKE_UNLOCK_PATH, O_WRONLY, 0);

	if (fd < 0) {
		has_wakelock = false;
		wakelock_activated = false;
		logWARN("%s: disabling wakelocks", __func__);
	} else {
		ssize_t written = posix_write("WAKE-UNLOCK", fd, WAKE_LOCK_STRING, sizeof(WAKE_LOCK_STRING) - 1);

		if (written < 0) {
			errnum = errno;
			if (!quiet)
				logERROR("%s: posix_write() errno=%d (%s)", __func__, errnum, strerror(errnum));
		} else {
			logDBG2("Wake lock released");
			wakelock_activated = false;
			success = 1;
		}

		posix_close("WAKE-UNLOCK", fd);
	}

	return success;
}

/**
 *   Check whether wake locks exist at all, and is writeable, set global indicator.
 *   @return Nothing
 */
void check_wakelock(void)
{
	int errnum;

	if (access(WAKE_LOCK_PATH, W_OK) < 0) {
		errnum = errno;
		logERROR("%s: access(%s, W_OK) errno=%d (%s)", __func__, WAKE_LOCK_PATH, errnum, strerror(errnum));
		logWARN("%s: disabling wakelocks", __func__);
		has_wakelock = false;
	} else {
		has_wakelock = true;
		do_wakelock_release(true);
	}
}


/**
 *   Acquire wakelock to prevent platform from sleeping.
 *
 *   May be called repeatedly without first calling release_wakelock().
 *
 *   @return 1 if successful, 0 if unsuccessful.
 */
int wakelock_acquire(void)
{
	int success = 0;
	int errnum;
	int fd;

	if (!has_wakelock)
		return 1;

	if (wakelock_activated)
		/* Wake lock is already in place */
		return 1;

	fd = posix_open(WAKE_LOCK_PATH, O_WRONLY, 0);

	if (fd < 0) {
		has_wakelock = false;
		logWARN("%s: disabling wakelocks", __func__);
	} else {
		ssize_t written = posix_write("WAKE-LOCK", fd, WAKE_LOCK_STRING, sizeof(WAKE_LOCK_STRING) - 1);

		if (written < 0) {
			errnum = errno;
			logERROR("%s: posix_write() errno=%d (%s)", __func__, errnum, strerror(errnum));
		} else {
			logDBG2("Indefinite wake lock acquired");
			wakelock_activated = true;
			success = 1;
		}

		posix_close("WAKE-LOCK", fd);
	}

	return success;
}


/**
 *   Release wakelock to allow platform to sleep.
 *
 *   @return 1 if successful, 0 if unsuccessful.
 */
int wakelock_release(void)
{

	if (!has_wakelock)
		return 1;

	if (!wakelock_activated)
		return 0;

	return do_wakelock_release(false);
}

/**
 *   Check if we have a wakelock or not
 *
 *   @return 1 if active, 0 if inactive (no wakelock).
 */
int wakelock_active(void)
{
	return wakelock_activated;
}
