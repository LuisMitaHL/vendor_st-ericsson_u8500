/*************************************************************************
 * Copyright ST-Ericsson 2011
 ************************************************************************/
#include "cops_error.h"

#ifdef COPS_OSE_ENVIRONMENT
#include "cops_psock.h"
#else
#include <errno.h>
#include <string.h>
#endif

int cops_errno(void)
{
#ifdef COPS_OSE_ENVIRONMENT
    return cops_psock_errno();
#else
    return errno;
#endif
}

char *cops_strerror(int errnum)
{
#ifdef COPS_OSE_ENVIRONMENT
    return (char *)cops_psock_strerror(errnum);
#else
    return strerror(errnum);
#endif
}

int cops_error_is_eagain(int errnum)
{
#ifdef COPS_OSE_ENVIRONMENT
    return cops_psock_error_is_eagain(errnum);
#else
    if (EAGAIN == errnum ||
        EWOULDBLOCK == errnum) {
        return 1;
    }
    return 0;
#endif
}
