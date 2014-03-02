/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \brief  Routing Control
 */

#ifndef _STERC_MISC_H
#define _STERC_MISC_H 1

#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef STERC_SW_VARIANT_ANDROID
#define  LOG_EMERG  0  /* system is unusable */
#define  LOG_ALERT  1  /* action must be taken immediately */
#define  LOG_CRIT  2  /* critical conditions */
#define  LOG_ERR    3  /* error conditions */
#define  LOG_WARNING  4  /* warning conditions */
#define  LOG_NOTICE  5  /* normal but significant condition */
#define  LOG_INFO  6  /* informational */
#define  LOG_DEBUG  7  /* debug-level messages */
#endif
    /*******************************************************************************
     *
     * Functions and macros
     *
     *******************************************************************************/

    /*
     * STERC_COMPILE_TIME_ASSERT
     *
     * Check constants at compile time
     * usage: STERC_COMPILE_TIME_ASSERT(x == y)
     */
#ifndef STERC_COMPILE_TIME_ASSERT
#define STERC_COMPILE_TIME_ASSERT(x) (void)(1/(int)(x))
#endif

    /*
     * Should be used to "touch" a variable or a formal parameter that is intentionally
     * not used and thereby avoid compiler warnings.
     *
     * @Id A symbol that represents a variable that is not used, usually a
     *     formal parameter.
     *
     */
#define STERC_IDENTIFIER_NOT_USED(Id) (void)(Id);

#define STERC_DBG_TRACE(level, format, ...) stercd_log_func(NULL, level, __FILE__, __LINE__, format, ##__VA_ARGS__)
#define STERC_DBG_TRACE_NO_INFO(format, ...) stercd_log_func_no_info(NULL, format, ##__VA_ARGS__)
#define STERC_DBG_TRACE_DEFINE(var) var
    void stercd_log_func(void *user_p, int level, const char *file, int line, const char *format, ...);

    int exec_script(char *const *argv, char *const *env);
    void stercd_log_func(void *user_p, int level, const char *file, int line, const char *format, ...);

#ifdef __cplusplus
}
#endif
#endif
