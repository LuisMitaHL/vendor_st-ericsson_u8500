/*                               -*- Mode: C -*-
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * Description     : Log handling
 *
 * Author          : Johan Norberg <johan.xx.norberg@stericsson.com>
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/types.h>
#include <unistd.h>
#include "log.h"

#define MAX_SIZE_OF_FORMAT_STRING 1000
#define ERROR_STRING "error"
#define DEBUG_STRING "debug"
#define INFO_STRING "info"
#define VERBOSE_STRING "verbose"

void simpb_print_log(simpb_log_type_t log_type, const char *func_name_p, const char *format_p, ...)
{
    va_list arg_list;
    char *new_format_p = NULL;
    char *log_type_p = NULL;
    new_format_p = (char *) calloc(1, MAX_SIZE_OF_FORMAT_STRING);

    if (!new_format_p) {
        goto exit;
    }

    switch (log_type) {
    case SIMPB_LOG_TYPE_ERROR:
        log_type_p = (char *) ERROR_STRING;
        break;
    case SIMPB_LOG_TYPE_DEBUG:
        log_type_p = (char *) DEBUG_STRING;
        break;
    case SIMPB_LOG_TYPE_INFO:
        log_type_p = (char *) INFO_STRING;
        break;
    case SIMPB_LOG_TYPE_VERBOSE:
        log_type_p = (char *) VERBOSE_STRING;
        break;
    default:
        goto exit;
    }

    va_start(arg_list, format_p);
#ifdef CFG_USE_ANDROID_LOG
    snprintf(new_format_p, MAX_SIZE_OF_FORMAT_STRING, "[%d] %s, %s: %s",
             getpid(), log_type_p, func_name_p, format_p);
    LOG_PRI_VA(ANDROID_LOG_INFO, CFG_ANDROID_LOG_TAG, new_format_p, arg_list);
#else
    snprintf(new_format_p, MAX_SIZE_OF_FORMAT_STRING, "[%d] %s, %s: %s\n",
             getpid(), log_type_p, func_name_p, format_p);
    vprintf(new_format_p, arg_list);
#endif
    va_end(arg_list);

exit:

    if (new_format_p) {
        free(new_format_p);
    }

    return;
}

