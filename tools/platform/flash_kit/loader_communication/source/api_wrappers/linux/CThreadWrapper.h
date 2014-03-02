/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/

#ifndef _CTHREADWRAPPER_H
#define _CTHREADWRAPPER_H

#include "CWaitableObject.h"
#include "CEventObject.h"
#include <pthread.h>


typedef void *(*StartAddress_t)(void *);

class CThreadWrapper : public CWaitableObject
{
public:
    CThreadWrapper(void *(* pStartAddress)(void *), void *pArgument);
    ~CThreadWrapper();
    void ResumeThread();
    void SuspendThread();
    void WaitToDie(DWORD dwMilliseconds = 1000);
    DWORD GetThreadId();
    DWORD Wait(DWORD dwMilliseconds = INFINITE);
private:
    pthread_t m_tid;
    StartAddress_t m_pStartAddress;
    void *m_pArgument;
    CEventObject *m_ThreadEndedEvt;

protected:
    static void *ThreadFunc(void *Arg);
};

#endif /* _CTHREADWRAPPER_H */

