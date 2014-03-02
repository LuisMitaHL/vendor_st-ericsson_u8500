/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \brief  Routing Control Daemon service functions
 */

#ifndef STERCD_H
#define STERCD_H

#ifdef  __cplusplus
extern "C" {
#endif

/********************************************************************************
 *
 * Include files
 *
 ********************************************************************************/
#include <sys/socket.h>
#include "mpl_config.h"
#include "util_log.h"

#ifndef STERC_SW_VARIANT_ANDROID
#include <syslog.h>
#endif

/********************************************************************************
 *
 * Defines & Type definitions
 *
 ********************************************************************************/
#ifdef STERC_MODULE_TEST_ENABLED
#define STERC_CTRL_PATH "/tmp/sterc_ctrl_server_socket"
#define STERC_EVENT_PATH "/tmp/sterc_event_server_socket"
#else
#define STERC_CTRL_PATH "/dev/socket/sterc_ctrl_server"
#define STERC_EVENT_PATH "/dev/socket/sterc_event_server"
#endif
#define STERC_CTRL_PORT 13588
#define STERC_EVENT_PORT 13589
#define STERCD_BUFSIZ 4096

typedef enum {
  tt_unix,
  tt_ip,
  tt_unknown
}sterc_transport_type_t;

extern mpl_config_t stercd_config;

#ifdef STERC_SW_VARIANT_ANDROID
#define  LOG_EMERG  UTIL_LOG_TYPE_ERROR  /* system is unusable */
#define  LOG_ALERT  UTIL_LOG_TYPE_ERROR  /* action must be taken immediately */
#define  LOG_CRIT  UTIL_LOG_TYPE_ERROR  /* critical conditions */
#define  LOG_ERR    UTIL_LOG_TYPE_ERROR  /* error conditions */
#define  LOG_WARNING  UTIL_LOG_TYPE_WARNING  /* warning conditions */
#define  LOG_NOTICE  UTIL_LOG_TYPE_WARNING  /* normal but significant condition */
#define  LOG_INFO  UTIL_LOG_TYPE_WARNING  /* informational */
#define  LOG_DEBUG  UTIL_LOG_TYPE_DEBUG  /* debug-level messages */
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
#define  STERC_LOG_E(...) util_print_log(UTIL_LOG_MODULE_STERC, sterc_get_log_level(), UTIL_LOG_TYPE_ERROR, __func__, __VA_ARGS__)
#define  STERC_LOG_W(...) util_print_log(UTIL_LOG_MODULE_STERC, sterc_get_log_level(), UTIL_LOG_TYPE_WARNING, __func__, __VA_ARGS__)
#define  STERC_LOG_I(...) util_print_log(UTIL_LOG_MODULE_STERC, sterc_get_log_level(), UTIL_LOG_TYPE_INFO, __func__, __VA_ARGS__)
#define  STERC_LOG_D(...) util_print_log(UTIL_LOG_MODULE_STERC, sterc_get_log_level(), UTIL_LOG_TYPE_DEBUG, __func__, __VA_ARGS__)
#define  STERC_LOG_V(...) util_print_log(UTIL_LOG_MODULE_STERC, sterc_get_log_level(), UTIL_LOG_TYPE_VERBOSE, __func__, __VA_ARGS__)
void stercd_log_func(void* user_p, int level, const char* file, int line, const char *format, ...);
int sterc_log_init();
void sterc_log_close();
int sterc_log_select_callback(const int fd, const void *data_p);
void sterc_set_log_level(util_log_type_t log_level);
util_log_type_t sterc_get_log_level();

/*
 * stercd_send_event
 *
 * @event_p
 * @event_len
 *
 * Returns: 0 on success, -1 on errror
 *
 */
int stercd_send_event(char *event_p, int event_len);

#ifdef  __cplusplus
}
#endif

#endif
