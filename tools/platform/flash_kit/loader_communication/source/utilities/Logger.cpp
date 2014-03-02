/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/

#include "Logger.h"
using namespace std;

#ifdef _WIN32
#define flockfile _lock_file
#define funlockfile _unlock_file
#else
#include <cstddef>
#include "OS.h"
#endif

Logger::Logger(const void *communication): communication_(communication), messageCallback_(0)
{
}

void Logger::log(const char *format, ...)
{
#ifdef _MESSAGES

    if (0 != messageCallback_) {
        CLockCS lock(criticalSection_);
        va_list args;
        char message[1024];
        va_start(args, format);
        vsprintf_s(message, format, args);
        va_end(args);
        messageCallback_(communication_, strlen(message), message);
    }

#endif
}
