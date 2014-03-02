/*************************************************************************
 * Copyright ST-Ericsson 2012
 ************************************************************************/
#ifndef COPS_LOG_H
#define COPS_LOG_H

#include <stdio.h>
#include <stdbool.h>

#ifdef COPS_OSE_ENVIRONMENT
#define fprintf(x, ...) printf(__VA_ARGS__)
#endif

#ifdef COPS_ENABLE_ENV_MODE_ITP
#define COPS_LOG(level, ...)
#else
#define COPS_LOG(level, ...) \
    do { \
        cops_log(level, __FILE__, __func__, __LINE__, __VA_ARGS__); \
    } while (0)
#endif /* COPS_ENABLE_ENV_MODE_ITP*/

enum cops_log_level {
    LOG_ERROR = 0,
    LOG_WARNING,
    LOG_INFO,
};

#ifndef COPS_ENABLE_ENV_MODE_ITP
void cops_log(enum cops_log_level level, const char *file,
              const char *func, int line, const char *format, ...)
__attribute__((format(printf, 5, 6)));
#endif /* COPS_ENABLE_ENV_MODE_ITP */

#endif /* COPS_LOG_H */
