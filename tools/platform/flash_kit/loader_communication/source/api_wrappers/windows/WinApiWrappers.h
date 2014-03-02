/*******************************************************************************
*
*    File name: WinApiWrappers.h
*     Language: Visual C++
*  Description: WinAPI encapsulation class declarations
*
*
* Copyright (C) ST-Ericsson SA 2011
* License terms: 3-clause BSD license
*
*******************************************************************************/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                 File WinApiWrappers.h

#ifndef __WINAPIWRAPPERS_H__
#define __WINAPIWRAPPERS_H__

#pragma once

#include <windows.h>
#include <process.h>
#include <assert.h>
#include <vector>
#ifdef __MINGW32__
#include "String_s.h"
#include <stdlib.h>
#endif

using namespace std;

class CWaitableObject
{
public:
    CWaitableObject() {}
    virtual ~CWaitableObject() {}
    virtual DWORD Wait(DWORD dwTimeout = INFINITE) = 0;
private:

};

// Thin encapsulation of WinAPI's CRITICAL_SECTION
class CCriticalSectionObject
{
    friend class CLockCS;
public:
    CCriticalSectionObject() {
        ::InitializeCriticalSection(&m_CriticalSection);
    }
    ~CCriticalSectionObject() {
        ::DeleteCriticalSection(&m_CriticalSection);
    }
private:
    void Enter() {
        ::EnterCriticalSection(&m_CriticalSection);
    }
    void Leave() {
        ::LeaveCriticalSection(&m_CriticalSection);
    }
    CRITICAL_SECTION m_CriticalSection;
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

// Thin encapsulation of WinAPI's Events
class CEventObject : public CWaitableObject
{
public:
    CEventObject() {
        m_Handle = ::CreateEvent(
                       0,      // LPSECURITY_ATTRIBUTES
                       FALSE,  // bManualReset i.e. Use auto-reset functionality
                       FALSE,  // bInitialState i.e. In non-signalled state initially
                       0);
    }   // lpName
    ~CEventObject() {
        ::CloseHandle(m_Handle);
    }
    void SetEvent() {
        ::SetEvent(m_Handle);
    }
    void UnsetEvent() {
        ::ResetEvent(m_Handle);
    }
    DWORD Wait(DWORD milliseconds = INFINITE) {
        return ::WaitForSingleObject(m_Handle, milliseconds);
    }
    HANDLE GetHandle() {
        return m_Handle;
    }
private:
    HANDLE m_Handle;
};

// A thin encapsulation of WinAPI's Thread
class CThreadWrapper
{
public:
    CThreadWrapper(unsigned int (WINAPI *pStartAddress)(void *), void *pArgument) {
        m_Handle = (HANDLE)_beginthreadex(
                       0,                  // LPSECURITY_ATTRIBUTES
                       0,                  // dwStackSize - default
                       pStartAddress,      // lpStartAddress
                       pArgument,          // lpParameter
                       CREATE_SUSPENDED,   // dwCreationFlags
                       0);                 // lpThreadId
    }
    ~CThreadWrapper()                           {
        ::CloseHandle(m_Handle);
    }
    void ResumeThread()                         {
        ::ResumeThread(m_Handle);
    }
    void SuspendThread()                        {
        ::SuspendThread(m_Handle);
    }
    void WaitToDie(DWORD dwMilliseconds = 1000) {
        ::WaitForSingleObject(m_Handle, dwMilliseconds);
    }
private:
    HANDLE m_Handle;
    DWORD  m_ThreadId;
};

//class that encapsulates the OS methods for Object signaling
//it contains functions for registering signal objects.
//the main function is the wait method which interface is platform independent.
class CWaitableObjectCollection
{
public:
    CWaitableObjectCollection() {
        m_objs.clear();
    }

    virtual ~CWaitableObjectCollection() {
        m_objs.clear();
    }

    //adds object to the collection
    void Add(CWaitableObject *obj) {
        m_objs.push_back(obj);
    }

    //waits for multiple objects
    //this function returns the object that signaled the end of its job
    //or null if dwTimeout occur
    CWaitableObject *Wait(DWORD dwTimeout = INFINITE) {
        vector<CWaitableObject *>::iterator it;
        DWORD dwTimePassed = 0;
        DWORD curr_time, start_time;

        start_time = ::GetTickCount();

        do {
            for (it = m_objs.begin(); it != m_objs.end(); ++it) {
                assert(*it);

                if ((*it)->Wait(0) == 0) {
                    return (*it);
                }
            }

            Sleep(10);

            curr_time = ::GetTickCount();

            dwTimePassed = curr_time - start_time;
        } while (dwTimePassed < dwTimeout);

        return NULL;
    }
private:
    vector<CWaitableObject *>m_objs;
};

enum RemoveResult {
    REMOVE_SUCCESS,
    REMOVE_TIMEOUT,
    REMOVE_CANCEL,
    REMOVE_WAIT_FOR_SEMAPHORE
};

// An object queue implementation using WinAPI's Semaphore.
// For multi-threaded multi-processor applications with multiple producers and multiple consumers
class CSemaphoreQueue
{
public:
    CSemaphoreQueue(unsigned int MaxCount) : m_MaximumCount(MaxCount) {
        m_Handles[0] = ::CreateEvent(NULL, FALSE, FALSE, NULL);
        m_Handles[1] = ::CreateSemaphore(NULL, 0, MaxCount, NULL);
        m_Queue = new void*[MaxCount];
        m_CurrentHead = 0;
        m_CurrentTail = 0;
        m_CurrentCount = 0;
    }

    ~CSemaphoreQueue() {
        ::CloseHandle(m_Handles[0]);
        ::CloseHandle(m_Handles[1]);
        delete m_Queue;
    }

    bool AddTail(void *pObject) {
        bool result;
        CLockCS LocalCSLock(m_CSLock);
        AddToQueueTail(pObject);
        result = ::ReleaseSemaphore(m_Handles[1], 1, 0) ? true : false;

        if (!result) {
            RemoveFromQueueTail(); // Not really necessary but keep, everything's gone pear-shaped anyway
        }

        return result;
    }

    RemoveResult RemoveHead(void **ppObject, int mSecTimeout) {
        switch (::WaitForMultipleObjects(2, m_Handles, FALSE, mSecTimeout)) {
        case WAIT_OBJECT_0 + 0:
            return REMOVE_WAIT_FOR_SEMAPHORE;

        case WAIT_OBJECT_0 + 1: {
            CLockCS LocalCSLock(m_CSLock);
            *ppObject = RemoveFromQueueHead(); // Remove pObject from pObjectQueue head
            return REMOVE_SUCCESS;
        }

        case WAIT_TIMEOUT:
            return REMOVE_TIMEOUT;

        default:
            assert(false);
            return REMOVE_CANCEL;
        }
    }

    void SignalEvent() {
        ::SetEvent(m_Handles[0]);
    }
private:
    HANDLE m_Handles[2];
    CCriticalSectionObject m_CSLock;
    void **m_Queue;
    const unsigned int m_MaximumCount;
    unsigned int m_CurrentCount;
    unsigned int m_CurrentHead;
    unsigned int m_CurrentTail;
    void IncrementHead() {
        ++m_CurrentHead;

        if (m_CurrentHead == m_MaximumCount) {
            m_CurrentHead = 0;
        }
    }
    void IncrementTail() {
        ++m_CurrentTail;

        if (m_CurrentTail == m_MaximumCount) {
            m_CurrentTail = 0;
        }
    }
    void AddToQueueTail(LPVOID pObject) {
        m_Queue[m_CurrentTail] = pObject;
        IncrementTail();
    }
    void *RemoveFromQueueHead() {
        void *Object;
        Object = m_Queue[m_CurrentHead];
        IncrementHead();
        return Object;
    }
    // Next 2 functions not really necessary - for error case only
    void DecrementTail() {
        if (m_CurrentTail == 0) {
            m_CurrentTail = m_MaximumCount - 1;
        } else {
            --m_CurrentTail;
        }
    }
    void RemoveFromQueueTail() {
        DecrementTail();
        m_Queue[m_CurrentTail] = 0;
    }
};

class CSimpleQueue : public CSemaphoreQueue
{
public:
    CSimpleQueue(): CSemaphoreQueue(256) {}
    RemoveResult RemoveRequest(void **ppRequest, unsigned int mSecTimeout = INFINITE) {
        return RemoveHead(ppRequest, mSecTimeout);
    }

    bool AddRequest(void *pRequest) {
        return AddTail(pRequest);
    }
};

class OS
{
public:
    OS();
    virtual ~OS();

    static void Sleep(DWORD dwMilliseconds) {
        ::Sleep(dwMilliseconds);
    }
    static time_t GetSystemTimeInMs() {
        FILETIME systemTime;
        ::GetSystemTimeAsFileTime(&systemTime);
        ULARGE_INTEGER time;
        time.LowPart = systemTime.dwLowDateTime;
        time.HighPart = systemTime.dwHighDateTime;
        // file time is in 100 ns intervals, convert to ms
        return time.QuadPart / 10000;
    }
    static DWORD GetErrorCode() {
        return ::GetLastError();
    }
    static void SetErrorCode(DWORD dwErrorCode) {
        ::SetLastError(dwErrorCode);
    }
};

#endif // !defined(__WINAPIWRAPPERS_H__)

//                                                                                          End of file WinApiWrappers.h
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
