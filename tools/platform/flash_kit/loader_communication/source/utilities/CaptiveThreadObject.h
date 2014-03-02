/*******************************************************************************
*
*    File name: CaptiveThreadObject.h
*     Language: Visual C++
*  Description: Captive Thread Object class declarations
*
*
* Copyright (C) ST-Ericsson SA 2011
* License terms: 3-clause BSD license
*
*******************************************************************************/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                            File CaptiveThreadObject.h

#ifndef __CAPTIVE_THREAD_OBJECT_H__
#define __CAPTIVE_THREAD_OBJECT_H__

#pragma once

#if defined(WIN32)
#include <windows.h>
#include "WinApiWrappers.h"
#elif (defined(__linux__) || defined(__APPLE__))
#include "LinuxApiWrappers.h"
#else
#error "Unknown target"
#endif

// CCaptiveThreadObject owns the captive thread
class CCaptiveThreadObject
{
public:
    CCaptiveThreadObject();
    virtual ~CCaptiveThreadObject();
    void EndCaptiveThread();
    int ThreadIsDying() {
        return IsDying;
    }
protected:
    virtual void InitializeCaptiveThreadObject() = 0;
    virtual void MainExecutionLoop() = 0;

    virtual void SignalDeath() = 0;
#ifdef _WIN32
    static unsigned int WINAPI ThreadEntry(void *arg);
#else
    static void *ThreadEntry(void *arg);
#endif
    int IsDying;
    CThreadWrapper  Thread;
};

#endif

//                                                                                     End of file CaptiveThreadObject.h
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
