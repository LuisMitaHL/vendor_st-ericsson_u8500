/*************************************************************************
 * Copyright ST-Ericsson 2012
 ************************************************************************/
#ifndef COPS_COMMON_H
#define COPS_COMMON_H

#include <cops_log.h>

#define XSTR(x) #x
#define STR(x) XSTR(x)

#define COPS_CHK_RC(rc) \
    do { \
        ret_code = (rc); \
        if (ret_code != COPS_RC_OK) { \
            COPS_LOG(LOG_ERROR, "ERROR: ret_code = %d\n", ret_code); \
            goto function_exit; \
        } \
    } while (0)

#define COPS_CHK(cond, failrc) \
    do { \
        if (!(cond)) { \
            ret_code = (failrc); \
            goto function_exit; \
        } \
    } while (0)

#define COPS_SET_RC(rc, ...) \
    do { \
        COPS_LOG(LOG_ERROR, __VA_ARGS__); \
        ret_code = (rc); \
        COPS_LOG(LOG_ERROR, "ERROR: ret_code = %d\n", (rc)); \
        goto function_exit; \
    } while (0)

#define COPS_CHK_ALLOC(ptr) \
    do { \
        if ((ptr) == NULL) { \
            ret_code = COPS_RC_MEMORY_ALLOCATION_ERROR; \
            COPS_LOG(LOG_ERROR, "Memory allocation error\n"); \
            goto function_exit; \
        } \
    } while (0)

/* Macro used to detect coding errors during runtime */
#define COPS_CHK_ASSERTION(cond) \
    do { \
        if (!(cond)) { \
            ret_code = COPS_RC_INTERNAL_ERROR; \
            COPS_LOG(LOG_ERROR, "Assertion error\n"); \
            goto function_exit; \
        } \
    } while (0)

#define COPS_DEBUG_CHK_ASSERTION(cond) COPS_CHK_ASSERTION(cond)

#endif /* COPS_COMMON_H */
