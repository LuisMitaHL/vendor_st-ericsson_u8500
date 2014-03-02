/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>
#include <fcntl.h>
#include <errno.h>

#include <util_log.h>

#define MAX_SIZE_OF_FORMAT_STRING 1000

/* Use the Android log mechanism if available */
#ifdef CFG_USE_ANDROID_LOG
#include <utils/Log.h>
 /* utils/Log.h will set default LOG_TAG to NULL. But this needs   *
  * to be defined in runtime to support several different modules. *
  * So a stack variable with the same name is used for the Android *
  * log macros.                                                    */
#undef LOG_TAG
#endif /* CFG_USE_ANDROID_LOG */

typedef struct {
    char *name_p;
    char *log_tag_p;
} util_module_entry_t;

/* NOTE: Align order of assignment with the util_log_module_t */
static util_module_entry_t util_module_db[] = {
    /* UTIL_LOG_MODULE_UTIL */ { "<UTIL>:", "RIL_UTIL"},
    /* UTIL_LOG_MODULE_CN   */ { "<CN>:",   "RIL_CN"},
    /* UTIL_LOG_MODULE_AT   */ { "<AT>:",   "AT"},
    /* UTIL_LOG_MODULE_SIM  */ { "<SIM>:",  "RIL_SIM"},
    /* UTIL_LOG_MODULE_SMS  */ { "<SMS>:",  "SMS"},
    /* UTIL_LOG_MODULE_MAD  */ { "<MAD>:",  "MAD"},
    /* UTIL_LOG_MODULE_PSCC */ { "<PSCC>:",  "RIL PSCC"},
    /* UTIL_LOG_MODULE_STERC*/ { "<STERC>:",  "RIL STERC"}
};

#ifdef CFG_USE_ANDROID_LOG
/* NOTE: Align order of assignment with the util_log_type_t */
static int android_log_type[] = {
    /* UTIL_LOG_TYPE_UNKNOWN */ ANDROID_LOG_ERROR, /* unknown -> error */
    /* UTIL_LOG_TYPE_ERROR   */ ANDROID_LOG_ERROR,
    /* UTIL_LOG_TYPE_WARNING */ ANDROID_LOG_WARN,
    /* UTIL_LOG_TYPE_INFO    */ ANDROID_LOG_INFO,
    /* UTIL_LOG_TYPE_DEBUG   */ ANDROID_LOG_DEBUG,
    /* UTIL_LOG_TYPE_VERBOSE */ ANDROID_LOG_VERBOSE
};
#else
/* NOTE: Align order of assignment with the util_log_type_t */
static char *log_type_string[] = {
    /* UTIL_LOG_TYPE_UNKNOWN */ "unknown",
    /* UTIL_LOG_TYPE_ERROR   */ "error",
    /* UTIL_LOG_TYPE_WARNING */ "warning",
    /* UTIL_LOG_TYPE_INFO    */ "info",
    /* UTIL_LOG_TYPE_DEBUG   */ "debug",
    /* UTIL_LOG_TYPE_VERBOSE */ "verbose"
};
#endif

int calculate_log_type_index(util_log_type_t log_type);


util_void_t util_print_log(util_log_module_t module, util_log_type_t module_log_level, util_log_type_t log_type, const char *func_name_p, const char *format_p, ...)
{
    va_list arg_list;
    char *new_format_p = NULL;
    char *log_type_p   = NULL;
    char *LOG_TAG      = NULL; /* no _p suffix can be used here */

    /* check if the log type is permitted according to log level */
    if ((log_type & module_log_level) == 0) {
        goto exit;
    }

    new_format_p = (char *) calloc(1, MAX_SIZE_OF_FORMAT_STRING);
    if (!new_format_p) {
        goto exit;
    }

    va_start(arg_list, format_p);

#ifdef CFG_USE_ANDROID_LOG
    (void)log_type_p; /* not used since Android provides this information already */

    /* lookup Android LOG_TAG relevant for the given module */
    if (UTIL_LOG_MODULE_LAST > module) {
        LOG_TAG = (char*)util_module_db[module].log_tag_p;
    }

    if (!LOG_TAG) {
        goto arg_list_cleanup;
    }

    /* prepare format string with all information */
    snprintf(new_format_p, MAX_SIZE_OF_FORMAT_STRING, "%s: %s",
             func_name_p, format_p);

    /* redirect prints to Android's radio log ('RIL' prefix in LOG_TAG) */
    LOG_PRI_VA(android_log_type[calculate_log_type_index(log_type)], LOG_TAG, new_format_p, arg_list);

#else /* CFG_USE_ANDROID_LOG undefined */

    /* prepare format string with all information */
    log_type_p = log_type_string[calculate_log_type_index(log_type)];

    /* print to standard output */
    snprintf(new_format_p, MAX_SIZE_OF_FORMAT_STRING, "%s, %s: %s\n",
             log_type_p, func_name_p, format_p);
    vprintf(new_format_p, arg_list);
#endif

arg_list_cleanup:
    va_end(arg_list);

exit:
    if (new_format_p) {
        free(new_format_p);
    }
}

util_log_type_t util_read_log_file(util_log_module_t module)
{
    char line[80];
    char *param_pos_p = NULL;
    util_log_type_t log_level = UTIL_LOG_TYPE_UNKNOWN;
    FILE *file_p = NULL;

    if (module >= UTIL_LOG_MODULE_LAST) {
        UTIL_LOG_E("Unknown module");
        goto exit;
    }

    file_p = fopen(UTIL_LOG_FILE, "r");

    if (file_p == NULL) {
        UTIL_LOG_E("Error %d when opening %s for read ",
                   errno, UTIL_LOG_FILE);
        goto exit;
    }

    while (fgets(line, sizeof(line), file_p) != NULL) {
        /* Ignore empty or comment lines */
        if (1 > strlen(line) || '#' == line[0]) {
            continue;
        }

        param_pos_p = strstr(line, util_module_db[module].name_p);

        if (param_pos_p) {
            log_level = atoi(param_pos_p + strlen(util_module_db[module].name_p));
            break;
        }
    }

exit:

    if (file_p != NULL) {
        fclose(file_p);
    }

    return log_level;
}

int calculate_log_type_index(util_log_type_t log_type)
{
  unsigned int target = 0;
  unsigned int index = 0;

  if (0 == log_type || UTIL_LOG_TYPE_MAX_VALUE < log_type) {
      goto exit;
  }

  while (target < log_type) {
    target = (int)pow(2, index);
    index++;
  }

exit:
    return index;
}

