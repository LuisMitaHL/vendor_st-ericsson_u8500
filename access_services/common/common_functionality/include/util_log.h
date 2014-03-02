/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __util_log_type_h__
#define __util_log_type_h__ (1)

#include <util_general.h>

#define UTIL_LOG_FILE_NAME "telephony_log_levels"

#ifdef CFG_USE_ANDROID_LOG
#define UTIL_LOG_DIR "/data/misc/"
#else
#define UTIL_LOG_DIR "/tmp/"
#endif

#define UTIL_LOG_FILE UTIL_LOG_DIR UTIL_LOG_FILE_NAME

typedef enum {
    UTIL_LOG_TYPE_UNKNOWN = 0,
    UTIL_LOG_TYPE_ERROR = 1,
    UTIL_LOG_TYPE_WARNING = 2,
    UTIL_LOG_TYPE_INFO = 4,
    UTIL_LOG_TYPE_DEBUG = 8,
    UTIL_LOG_TYPE_VERBOSE = 16
} util_log_type_t;
#define UTIL_LOG_TYPE_MAX_VALUE UTIL_LOG_TYPE_VERBOSE
#define UTIL_LOG_LEVEL (UTIL_LOG_TYPE_ERROR | UTIL_LOG_TYPE_WARNING | UTIL_LOG_TYPE_INFO | UTIL_LOG_TYPE_DEBUG | UTIL_LOG_TYPE_VERBOSE)

typedef enum {
    UTIL_LOG_MODULE_UTIL = 0,
    UTIL_LOG_MODULE_CN,
    UTIL_LOG_MODULE_AT,
    UTIL_LOG_MODULE_SIM,
    UTIL_LOG_MODULE_SMS,
    UTIL_LOG_MODULE_MAD,
    UTIL_LOG_MODULE_PSCC,
    UTIL_LOG_MODULE_STERC,
    UTIL_LOG_MODULE_LAST
} util_log_module_t;

util_void_t util_print_log(util_log_module_t module, util_log_type_t module_log_level, util_log_type_t log_type, const char *func_name_p, const char *format_p, ...);
util_log_type_t util_read_log_file(util_log_module_t module);

/* The log macros below are intended for common_functionality internal prints
 * Each module using util_print_log will have their own macro functions. */
#define  UTIL_LOG_E(...) util_print_log(UTIL_LOG_MODULE_UTIL, UTIL_LOG_LEVEL, UTIL_LOG_TYPE_ERROR, __func__, __VA_ARGS__)
#define  UTIL_LOG_W(...) util_print_log(UTIL_LOG_MODULE_UTIL, UTIL_LOG_LEVEL, UTIL_LOG_TYPE_ERROR, __func__, __VA_ARGS__)
#define  UTIL_LOG_I(...) util_print_log(UTIL_LOG_MODULE_UTIL, UTIL_LOG_LEVEL, UTIL_LOG_TYPE_INFO, __func__, __VA_ARGS__)
#define  UTIL_LOG_D(...) util_print_log(UTIL_LOG_MODULE_UTIL, UTIL_LOG_LEVEL, UTIL_LOG_TYPE_DEBUG, __func__, __VA_ARGS__)
#define  UTIL_LOG_V(...) util_print_log(UTIL_LOG_MODULE_UTIL, UTIL_LOG_LEVEL, UTIL_LOG_TYPE_VERBOSE, __func__, __VA_ARGS__)

#endif /* __util_log_type_h__ */
