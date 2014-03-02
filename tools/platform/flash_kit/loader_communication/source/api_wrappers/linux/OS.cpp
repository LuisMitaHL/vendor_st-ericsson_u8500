/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
#include <time.h>
#include <errno.h>
#include <sys/time.h>
#include "Types.h"
#include "OS.h"

DWORD OS::ErrorCode = 0;

OS::OS()
{
}

OS::~OS()
{
}

void OS::Sleep(DWORD dwMilliseconds)
{
    struct timespec ts;

    ts.tv_sec = dwMilliseconds / 1000;
    ts.tv_nsec = (dwMilliseconds % 1000) * 1000000L;

    /* coverity[returned_null] */
    while (-1 == nanosleep(&ts, &ts) && errno == EINTR);
}

time_t OS::GetSystemTimeInMs()
{
    struct timeval current_time;

    gettimeofday(&current_time, NULL);

    return (current_time.tv_sec * 1000) + (current_time.tv_usec / 1000);
}

timespec OS::GetAbsoluteTime(DWORD dwTimeout)
{
    timespec absolute_time;
    timeval current_time;
    long timeout_nsec;

    gettimeofday(&current_time, NULL);

    absolute_time.tv_sec = current_time.tv_sec + (dwTimeout / 1000);
    timeout_nsec = (dwTimeout % 1000) * 1000000L;

    if ((1000000000 - current_time.tv_usec * 1000) < timeout_nsec) {
        // overflow will occur!
        absolute_time.tv_sec++;
    }

    absolute_time.tv_nsec = (current_time.tv_usec * 1000 + timeout_nsec) % 1000000000;

    return absolute_time;
}
