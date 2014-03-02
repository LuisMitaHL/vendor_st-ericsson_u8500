/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/

#ifndef _LOGGER_H_
#define _LOGGER_H_

#include "t_basicdefinitions.h"
#ifdef _WIN32
#include "WinApiWrappers.h"
#else
#include "LinuxApiWrappers.h"
#endif

typedef void (*MessageCallback_t)(const void *Communication_p, uint32 MessageLength, const char *MessageText);

class Logger
{
public:
    Logger(const void *communication);

    void log(const char *format, ...);
    void setMessageCallback(MessageCallback_t messageCallback) {
        messageCallback_ = messageCallback;
    }
private:
    CCriticalSectionObject criticalSection_;
    const void *communication_;
    MessageCallback_t messageCallback_;
};

#endif // _LOGGER_H_
