/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 * 
 * \brief  PS Connection Control Daemon service functions
 */

#ifndef STEPSCCD_H
#define STEPSCCD_H

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

#ifndef PSCC_SW_VARIANT_ANDROID
#include "syslog.h"
#endif //PSCC_SW_VARIANT_ANDROID

/********************************************************************************
 *
 * Defines & Type definitions
 *
 ********************************************************************************/
#ifdef PSCC_MODULE_TEST_ENABLED
#define PSCC_CTRL_PATH "/tmp/pscc_ctrl_server_socket"
#define PSCC_EVENT_PATH "/tmp/pscc_event_server_socket"
#else
#define PSCC_CTRL_PATH "/dev/socket/pscc_ctrl_server"
#define PSCC_EVENT_PATH "/dev/socket/pscc_event_server"
#endif
#define PSCC_CTRL_PORT 13578
#define PSCC_EVENT_PORT 13579
#define PSCCD_BUFSIZ 4096

extern mpl_config_t psccd_config;
extern unsigned short gprs_res_ctrl;

// Use Logcat for android and we need to set log flags (same as syslog)
#ifdef PSCC_SW_VARIANT_ANDROID
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
 * PSCC_COMPILE_TIME_ASSERT
 *
 * Check constants at compile time
 * usage: PSCC_COMPILE_TIME_ASSERT(x == y)
 */
#define PSCC_COMPILE_TIME_ASSERT(x) (void)(1/(int)(x))


/*
 * PSCC_IDENTIFIER_NOT_USED
 *
 * Should be used to "touch" a variable or a formal parameter that is intentionally
 * not used and thereby avoid compiler warnings.
 *
 * @Id A symbol that represents a variable that is not used, usually a
 *     formal parameter.
 *
 */
#define PSCC_IDENTIFIER_NOT_USED(Id) (void)(Id);

/*
 * PSCC_DBG_TRACE*
 *
 * Debug trace
 *
 */
void psccd_log_func(void* user_p, int level, const char* file, int line, const char *format, ...);
void psccd_log_func_no_info(void* user_p, const char *format, ...);
int pscc_log_init();
void pscc_log_close();
int pscc_log_select_callback(const int fd, const void *data_p);
void pscc_set_log_level(util_log_type_t log_level);
util_log_type_t pscc_get_log_level();
int pscc_log_get_log_fd();
#define PSCC_DBG_TRACE(level, ...) psccd_log_func(NULL, level, __FILE__, __LINE__, ##__VA_ARGS__)
#define PSCC_DBG_TRACE_NO_INFO(...) psccd_log_func_no_info(NULL, ##__VA_ARGS__)
#define PSCC_DBG_TRACE_DEFINE(var) var
#define  PSCC_LOG_E(...) util_print_log(UTIL_LOG_MODULE_PSCC, pscc_get_log_level(), UTIL_LOG_TYPE_ERROR, __func__, __VA_ARGS__)
#define  PSCC_LOG_W(...) util_print_log(UTIL_LOG_MODULE_PSCC, pscc_get_log_level(), UTIL_LOG_TYPE_WARNING, __func__, __VA_ARGS__)
#define  PSCC_LOG_I(...) util_print_log(UTIL_LOG_MODULE_PSCC, pscc_get_log_level(), UTIL_LOG_TYPE_INFO, __func__, __VA_ARGS__)
#define  PSCC_LOG_D(...) util_print_log(UTIL_LOG_MODULE_PSCC, pscc_get_log_level(), UTIL_LOG_TYPE_DEBUG, __func__, __VA_ARGS__)
#define  PSCC_LOG_V(...) util_print_log(UTIL_LOG_MODULE_PSCC, pscc_get_log_level(), UTIL_LOG_TYPE_VERBOSE, __func__, __VA_ARGS__)

/*
 * psccd_send_event
 *
 * @event_p
 * @event_len
 *
 * Returns: 0 on success, -1 on errror
 *
 */
int psccd_send_event(char *event_p, int event_len);

/*
 * psccd_send_event
 *
 * @event_p
 * @event_len
 * @to
 * @to_len
 *
 * Returns: 0 on success, -1 on errror
 *
 */
int psccd_sendto_event(char *event_p, int event_len, struct sockaddr *to, socklen_t to_len);

#ifdef  __cplusplus
}
#endif

#endif
