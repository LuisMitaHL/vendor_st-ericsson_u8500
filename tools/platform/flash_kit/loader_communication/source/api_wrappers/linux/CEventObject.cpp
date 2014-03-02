/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/

#include <errno.h>
#include <sys/time.h>
#include <fcntl.h>
#include <time.h>
#include "Types.h"
#include "CEventObject.h"
#include "OS.h"

// ******************************************************************************
// Name:  CEventObject()
// Desc:  CEventObject constructor which initializes the m_sem member
// Ret:
// ******************************************************************************
CEventObject::CEventObject()
{
#if defined(__APPLE__)
    char sem_name[SEM_NAME_MAX_LENGTH];
    int sem_nr = 1;

    while (sem_nr <= SEM_MAX_NR) {
        snprintf(sem_name, SEM_NAME_MAX_LENGTH, "lcdriversem_%d", sem_nr);

        /* open semaphore with "rw" permissions for everyone - 0666 */
        m_sem = sem_open(sem_name, O_CREAT | O_EXCL, 0666 , 0);

        if (m_sem != SEM_FAILED) {
            break;
        }

        sem_nr++;
    }

#elif defined(__linux__)
    sem_init(&m_sem, 0, 0);
#endif
}

// ******************************************************************************
// Name:  ~CEventObject()
// Desc:  CEventObject destructor
// Ret:
// ******************************************************************************
CEventObject::~CEventObject()
{
#if defined(__APPLE__)
    sem_close(m_sem);
#elif defined(__linux__)
    sem_destroy(&m_sem);
#endif
}

// ******************************************************************************
// Name:  SetEvent()
// Desc:  Sets an event by post-ing the member semaphore m_sem
// Ret:
// ******************************************************************************
void CEventObject::SetEvent()
{
#if defined(__APPLE__)
    sem_post(m_sem);
#elif defined(__linux__)
    sem_post(&m_sem);
#endif
}

// ******************************************************************************
// Name:  Wait()
// Desc:  implementation of the pure virtual base class member Wait()
// Ret:   WAIT_OBJECT_0 when event occurs, or WAIT_TIMEOUT on timeout
// ******************************************************************************
DWORD CEventObject::Wait(DWORD dwTimeout)
{
#if defined(__APPLE__)

    if (INFINITE == dwTimeout) {
        sem_wait(m_sem);
    } else {
        struct timeval curr_time, start_time;
        DWORD dwTimePassed = 0;
        int ret;

        gettimeofday(&start_time, NULL);

        /* Try to lock the semaphore */
        ret = sem_trywait(m_sem);

        if (ret != 0) {
            while (dwTimePassed < dwTimeout) {
                /* Sleep 1ms */
                OS::Sleep(1);

                /* Try to lock the semaphore again*/
                ret = sem_trywait(m_sem);

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

    if (INFINITE == dwTimeout) {
        sem_wait(&m_sem);
        return WAIT_OBJECT_0;
    } else {
        timespec absolute_time = OS::GetAbsoluteTime(dwTimeout);
        int ret;

        /* coverity[returned_null] */
        while (-1 == (ret = sem_timedwait(&m_sem, &absolute_time)) && errno == EINTR);

        if (0 == ret) {
            return WAIT_OBJECT_0;
        } else {
            return WAIT_TIMEOUT;
        }
    }

#endif
}
