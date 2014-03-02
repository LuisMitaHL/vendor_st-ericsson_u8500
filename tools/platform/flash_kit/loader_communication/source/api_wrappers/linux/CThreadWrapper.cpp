/*******************************************************************************
*
*  File name: CThreadWrapper.cpp
*  Language: Visual C++
*  Description: CThreadWrapper class definitions
*               The class implements OS independent thread wrapper.
*               The class contains methods for suspending resuming and waiting for threads.
*               The implementations of the methods is linux dependent,
*               but the interface of the functions is same as the appropriate
*               WIN32 implementation.
*
*
* Copyright (C) ST-Ericsson SA 2011
* License terms: 3-clause BSD license
*
*******************************************************************************/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//

#include <string>
#include <signal.h>

#include "Types.h"
#include "CThreadWrapper.h"
#include "CCriticalSection.h"

// ******************************************************************************
// Name:  CThreadWrapper()
// Desc:  CEventObject constructor which initializes the class members
// Ret:
// ******************************************************************************
CThreadWrapper::CThreadWrapper(void *(*pStartAddress)(void *), void *pArgument)
{
    m_pStartAddress = pStartAddress;
    m_pArgument = pArgument;
    m_tid = (pthread_t)0;
    m_ThreadEndedEvt = new CEventObject();
}

// ******************************************************************************
// Name:  ~CThreadWrapper()
// Desc:  CEventObject destructor
// Ret:
// ******************************************************************************
CThreadWrapper::~CThreadWrapper()
{
    delete m_ThreadEndedEvt;
}

// ******************************************************************************
// Name:  Wait()
// Desc:  Waits for the thread to finish its job
// Ret:   WAIT_OBJECT_0 on success and EAIT_TIMEOUT otherwise
// ******************************************************************************
DWORD CThreadWrapper::Wait(DWORD dwMilliseconds)
{
    if (m_tid != 0) {
        return m_ThreadEndedEvt->Wait(dwMilliseconds);
    } else {
        return WAIT_OBJECT_0;
    }
}

// ******************************************************************************
// Name:  ResumeThread()
// Desc:  Resumes the thread. Should be called after the thread is created
// Ret:
// ******************************************************************************
void CThreadWrapper::ResumeThread()
{
    pthread_create(&m_tid, NULL, CThreadWrapper::ThreadFunc, this);
}

// ******************************************************************************
// Name:  ResumeThread()
// Desc:  Suspends the running thread.
// Ret:
// ******************************************************************************
void CThreadWrapper::SuspendThread()
{
    // this will stop all active threads
    pthread_kill(m_tid, SIGSTOP);
}

// ******************************************************************************
// Name:  WaitToDie()
// Desc:  Waits 1000ms for thread to die
// Ret:
// ******************************************************************************
void CThreadWrapper::WaitToDie(DWORD dwMilliseconds)
{
    CThreadWrapper::Wait(dwMilliseconds);
}
// ******************************************************************************
// Name:  ThreadFunc()
// Desc:  Sets event when the thread finishes its job
// Ret:
// ******************************************************************************
void *CThreadWrapper::ThreadFunc(void *arg)
{
    CThreadWrapper *pthis = (CThreadWrapper *)arg;

    pthis->m_pStartAddress(pthis->m_pArgument);

    // Thread has finished, set appropriate event here
    pthis->m_ThreadEndedEvt->SetEvent();
    pthread_detach(pthread_self());
    pthis->m_tid = (pthread_t)0;
    return NULL;
}

// ******************************************************************************
// Name:  GetThreadId()
// Desc:  Returns the thread ID
// Ret:   Thread ID
// ******************************************************************************
DWORD CThreadWrapper::GetThreadId()
{
    return (long long)m_tid;
}
