/*************************************************************************
 * Copyright ST-Ericsson 2011
 ************************************************************************/
#include <cops_fd.h>
#ifdef COPS_OSE_ENVIRONMENT
#include <cops_psock.h>
#include <stdlib.h>
#else
#ifdef COPS_WAKELOCK
#include <cops_wakelock.h>
#endif /* COPS_WAKELOCK */
#include <fcntl.h>
#include <unistd.h>
#endif /* COPS_OSE_ENVIRONMENT */

int cops_fcntl(int fd, int cmd, int arg)
{
#ifdef COPS_OSE_ENVIRONMENT
    return 0;
#else
    return fcntl(fd, cmd, arg);
#endif
}

int cops_select(int nfds, fd_set *readfds, fd_set *writefds,
                fd_set *exceptfds, struct timeval *timeout)
{
#ifdef COPS_OSE_ENVIRONMENT
    return cops_psock_select(nfds, readfds, writefds, exceptfds, timeout);
#else
#ifdef COPS_WAKELOCK
    struct timeval tv;
    int retval;

    tv.tv_sec = COPS_WAKELOCK_TIMEOUT_MS / 1000;
    tv.tv_usec = (COPS_WAKELOCK_TIMEOUT_MS % 1000) * 1000;

    retval = select(nfds, readfds, writefds, exceptfds,
                    cops_wakelock_active() ? &tv : timeout);

    if (retval > 0) {
        /* Data is available for reading.
           Acquire a wakelock to force platform to stay awake
           while the received request is being processed. */
        (void)cops_wakelock_acquire();
    } else {
        /* Timed out.
           Release wake lock so platform is free to sleep. */
        (void)cops_wakelock_release();
    }
   return retval;
#else
    return select(nfds, readfds, writefds, exceptfds, timeout);
#endif /* COPS_WAKLOCK */
#endif /* COPS_OSE_ENVIRONMENT */
}

int cops_close(int fd)
{
#ifdef COPS_OSE_ENVIRONMENT
    return cops_psock_close(fd);
#else
    return close(fd);
#endif
}

ssize_t cops_write(int fd, const void *buf, size_t count)
{
#ifdef COPS_OSE_ENVIRONMENT
    return cops_psock_write(fd, buf, count);
#else
    return write(fd, buf, count);
#endif
}

ssize_t cops_read(int fd, void *buf, size_t count)
{
#ifdef COPS_OSE_ENVIRONMENT
    return cops_psock_read(fd, buf, count);
#else
    return read(fd, buf, count);
#endif
}
