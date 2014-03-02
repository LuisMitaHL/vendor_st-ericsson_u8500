/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/

#include <time.h>
#include <errno.h>

#include "Types.h"
#include "CWaitableObjectCollection.h"
#include <assert.h>
#include <sys/time.h>

CWaitableObjectCollection::CWaitableObjectCollection()
{
    m_objs.clear();
}

CWaitableObjectCollection::~CWaitableObjectCollection()
{
    m_objs.clear();
}

void CWaitableObjectCollection::Add(CWaitableObject *obj)
{
    m_objs.push_back(obj);
}

CWaitableObject *CWaitableObjectCollection::Wait(DWORD dwTimeout)
{
    vector<CWaitableObject *>::iterator it;
    DWORD dwTimePassed = 0;
    struct timespec ts;
    struct timeval curr_time, start_time;

    gettimeofday(&start_time, NULL);

    do {
        for (it = m_objs.begin(); it != m_objs.end(); ++it) {
            assert(*it);

            if (WAIT_OBJECT_0 == (*it)->Wait(0)) {
                return (*it);
            }
        }

        ts.tv_sec = 0;
        ts.tv_nsec = 10000000L;  // 10 milliseconds

        // There isn't any possiblity of errno returning NULL, mean is not defined. Even if
        // errno is not defined this is the least thing that we should care for.

        // coverity[returned_null]
        while (-1 == nanosleep(&ts, &ts) && EINTR == errno);

        gettimeofday(&curr_time, NULL);

        dwTimePassed = 1000 * (curr_time.tv_sec - start_time.tv_sec) + \
                       (curr_time.tv_usec - start_time.tv_usec) / 1000;

    } while (dwTimePassed < dwTimeout);

    return NULL;
}
