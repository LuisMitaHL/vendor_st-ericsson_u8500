/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/

#include <sys/errno.h>
#include <semaphore.h>
#include <sys/time.h>
#include <fcntl.h>
#include "Types.h"
#include "CSemaphore.h"
#include "OS.h"


CSemaphore::CSemaphore(unsigned int initial_count)
{
#if defined(__APPLE__)
    char sem_name[SEM_NAME_MAX_LENGTH];
    int sem_nr = 1;

    while (sem_nr <= SEM_MAX_NR) {
        snprintf(sem_name, SEM_NAME_MAX_LENGTH, "lcdriversem_%d", sem_nr);

        /* open semaphore with "rw" permissions for everyone - 0666 */
        m_semaphore = sem_open(sem_name, O_CREAT | O_EXCL, 0666 , 0);

        if (m_semaphore != SEM_FAILED) {
            break;
        }

        sem_nr++;
    }

#elif defined(__linux__)
    sem_init(&m_semaphore, 0, initial_count);
#endif
}

CSemaphore::~CSemaphore()
{
#if defined(__APPLE__)
    sem_close(m_semaphore);
#elif defined(__linux__)
    sem_destroy(&m_semaphore);
#endif
}

bool CSemaphore::Release(unsigned int count)
{
    if (!count) {
        return false;
    }

    for (unsigned int i = 0; i < count; ++i) {
#if defined(__APPLE__)

        if (sem_post(m_semaphore)) {
#elif defined(__linux__)

        if (sem_post(&m_semaphore)) {
#endif
            return false;
        }
    }

    return true;
}

DWORD CSemaphore::Wait(DWORD timeout) {
#if defined(__APPLE__)

    if (INFINITE == timeout) {
        sem_wait(m_semaphore);
    } else {
        struct timeval curr_time, start_time;
        DWORD dwTimePassed = 0;
        int ret;

        gettimeofday(&start_time, NULL);

        /* Try to lock the semaphore */
        ret = sem_trywait(m_semaphore);

        if (ret != 0) {
            while (dwTimePassed < timeout) {
                /* Sleep 1ms */
                OS::Sleep(1);

                /* Try to lock the semaphore again*/
                ret = sem_trywait(m_semaphore);

                if (ret == 0) {
                    return WAIT_OBJECT_0;
                }

                gettimeofday(&curr_time, NULL);

                dwTimePassed = 1000 * (curr_time.tv_sec - start_time.tv_sec) + \
                               (curr_time.tv_usec - start_time.tv_usec) / 1000;
            }

            return WAIT_TIMEOUT;
        }
    }

    return WAIT_OBJECT_0;
#elif defined(__linux__)

    if (INFINITE == timeout) {
        sem_wait(&m_semaphore);
    } else {
        timespec absoulute_time = OS::GetAbsoluteTime(timeout);
        int ret;

        /* coverity[returned_null] */
        while (-1 == (ret = sem_timedwait(&m_semaphore, &absoulute_time)) && errno == EINTR);

        if (0 != ret) {
            return WAIT_TIMEOUT;
        }
    }

    return WAIT_OBJECT_0;
#endif
}
