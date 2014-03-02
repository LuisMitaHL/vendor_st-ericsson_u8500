/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/

#ifndef _CCRITICALSECTION_H
#define _CCRITICALSECTION_H

#include "pthread.h"
class CCriticalSectionObject
{
    friend class CLockCS;
public:
    CCriticalSectionObject() {
        pthread_mutexattr_init(&recursivem);
        pthread_mutexattr_settype(&recursivem, PTHREAD_MUTEX_RECURSIVE);
        pthread_mutex_init(&m_CriticalSection, &recursivem);
    }
    ~CCriticalSectionObject() {
        pthread_mutexattr_destroy(&recursivem);
        pthread_mutex_destroy(&m_CriticalSection);
    }
    inline void Enter() {
        pthread_mutex_lock(&m_CriticalSection);
    }
    inline void Leave() {
        pthread_mutex_unlock(&m_CriticalSection);
    }
private:
    pthread_mutexattr_t recursivem;
    mutable pthread_mutex_t m_CriticalSection;
};

// Basic lock class used to enter and leave the CRITICAL_SECTION private member of a
// CCriticalSection object. Create a CLock object in scope that needs to be
// synchronized and pass the shared CCriticalSection object used to synchronize the
// resource to protect. The destructor calls the Leave method when leaving scope.
class CLockCS
{
public:
    CLockCS(CCriticalSectionObject &cs) : m_CriticalSectionObject(cs) {
        m_CriticalSectionObject.Enter();
    }
    ~CLockCS() {
        m_CriticalSectionObject.Leave();
    }
private:
    CCriticalSectionObject &m_CriticalSectionObject;
};

#endif /* _CCRITICALSECTION_H */

