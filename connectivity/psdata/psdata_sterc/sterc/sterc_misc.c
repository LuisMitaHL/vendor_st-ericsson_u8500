/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \brief  Routing Control
 */

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <unistd.h>
#include "sterc_misc.h"

#ifdef STERC_SW_VARIANT_ANDROID
#define LOG_TAG "RIL STERCD"
#include <cutils/log.h>
#include <time.h>
#include <sys/time.h>
#else
#include <syslog.h>
#endif //STERC_SW_VARIANT_ANDROID

#define PRINT_BUF_SIZE  (512)
extern int debug_mode;

/**
 * exec_script()
 **/
int exec_script(char *const *argv, char *const *env)
{
    pid_t pid;
    int pipefd[2];

    if (pipe(pipefd) < 0) {
        STERC_DBG_TRACE(LOG_ERR, "pipe failed (%s)", strerror(errno));
        return -1;
    }

    switch (pid = vfork()) {
    case -1:
        STERC_DBG_TRACE(LOG_ERR, "vfork: %s\n", strerror(errno));
        close(pipefd[0]);
        close(pipefd[1]);
        break;
    case 0:
        close(pipefd[1]);

        if (pipefd[0] != STDIN_FILENO) {
            if (dup2(pipefd[0], STDIN_FILENO) != STDIN_FILENO) {
                STERC_DBG_TRACE(LOG_ERR, "dup2 failed (%s)", strerror(errno));
                return -1;
            }

            close(pipefd[0]);
        }

        execve(argv[0], argv, env);
        STERC_DBG_TRACE(LOG_DEBUG, "%s: %s\n", argv[0], strerror(errno));
        _exit(127);
        /* NOTREACHED */
    }

    return pid;
}

/**
 * stercd_log_func()
 **/
void stercd_log_func(void *user_p, int level, const char *file, int line, const char *format, ...)
{

    STERC_IDENTIFIER_NOT_USED(user_p);

    /* Use Logcat for android, otherwise default log (syslog) */
#ifdef STERC_SW_VARIANT_ANDROID
    STERC_IDENTIFIER_NOT_USED(file);
    STERC_IDENTIFIER_NOT_USED(line);

    if (debug_mode || level != LOG_DEBUG) {
        char *buf = malloc(PRINT_BUF_SIZE);
        va_list arg;
        va_start(arg, format);

        vsnprintf(buf, PRINT_BUF_SIZE, format, arg);
        buf[PRINT_BUF_SIZE - 1] = '\0';

        switch (level) {
        case LOG_EMERG:
        case LOG_ALERT:
        case LOG_CRIT:
        case LOG_ERR:
            ALOGE("%s", buf);
            break;

        case LOG_WARNING:
            ALOGW("%s", buf);
            break;

        case LOG_NOTICE:
        case LOG_INFO:
            ALOGI("%s", buf);
            break;

        case LOG_DEBUG:
            ALOGD("%s", buf);
            break;
        default:

            break;
        }

        va_end(arg);
        free(buf);
    } else {
        STERC_IDENTIFIER_NOT_USED(format);
    }

#else
    va_list ap;

    if (debug_mode || level != LOG_DEBUG) {
        char *buf = malloc(PRINT_BUF_SIZE);
        va_start(ap, format);
        vsnprintf(buf, PRINT_BUF_SIZE, format, ap);
        buf[PRINT_BUF_SIZE - 1] = '\0';

        switch (level) {

        case LOG_EMERG:
        case LOG_ALERT:
        case LOG_CRIT:
        case LOG_ERR:
            fprintf(stderr, "ERROR [%s:%d]: ", file, line);
            break;

        case LOG_WARNING:
            fprintf(stderr, "WARN  [%s:%d]: ", file, line);
            break;

        case LOG_NOTICE:
        case LOG_INFO:
            fprintf(stderr, "INFO  [%s:%d]: ", file, line);
            break;

        case LOG_DEBUG:
        default:
            fprintf(stderr, "DEBUG [%s:%d]: ", file, line);
            break;
        }

        fprintf(stderr, "%s", buf);
        /* flush printf */
        fflush(stderr);
        free(buf);
        va_end(ap);
    }

#endif //STERC_SW_VARIANT_ANDROID
}


