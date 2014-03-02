/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/

#ifndef _OS_H
#define _OS_H
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include "Types.h"

#include "CWaitableObject.h"

class OS
{
public:
    OS();
    virtual ~OS();

    static void Sleep(DWORD dwMilliseconds);
    static time_t GetSystemTimeInMs();

    static DWORD GetErrorCode() {
        return OS::ErrorCode;
    }
    static void SetErrorCode(DWORD dwErrorCode) {
        OS::ErrorCode = dwErrorCode;
    }

    static timespec GetAbsoluteTime(DWORD dwTimeout);
private:

    static DWORD ErrorCode;
};
#endif /* _OS_H */
