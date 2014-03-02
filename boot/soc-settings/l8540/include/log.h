/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * Author: Joakim Axelsson <joakim.axelsson at stericsson.com>
 *  for ST-Ericsson
 */

#ifndef __LOG_H__
#define __LOG_H__

#include <config.h>
#if !defined(ENABLE_FEATURE_BUILD_HBTS) && !defined(ENABLE_FEATURE_SELFTESTS_IN_LOADER)
#include <uart.h>
#endif
/* Logging macros */
#define logerror(s, ...) \
do { \
	if (CONFIG_UART_ENABLED) \
		_uart_printf(CONFIG_UART_STRING "ERROR: " s "\r\n", \
			##__VA_ARGS__); \
} while (0)

#define logmemerr(s, addr, value, shouldbe, ...) \
do { \
	if (CONFIG_UART_ENABLED) \
		_uart_printf(CONFIG_UART_STRING "MEMORY ERROR: " s \
			" Address=%x Value=%x Should be=%x\r\n" \
			##__VA_ARGS__, addr, value, shouldbe); \
} while (0)

#define logwarn(s, ...) \
do { \
	if (CONFIG_UART_ENABLED) \
		_uart_printf(CONFIG_UART_STRING "WARN: " s "\r\n", \
			##__VA_ARGS__); \
} while (0)

#define loginfo(s, ...) \
do { \
	if (CONFIG_UART_ENABLED) \
		_uart_printf(CONFIG_UART_STRING "INFO: " s "\r\n", \
			##__VA_ARGS__); \
} while (0)

#define logdebug(s, ...) \
do { \
	if (CONFIG_UART_ENABLED && CONFIG_DEBUG_LOG) \
		_uart_printf(CONFIG_UART_STRING "DEBUG: " s "\r\n", \
			##__VA_ARGS__); \
} while (0)

/* More debugging */
#define logdebug2(s, ...) \
do { \
	if (CONFIG_UART_ENABLED && CONFIG_DEBUG_LOG >= 2) \
		_uart_printf(CONFIG_UART_STRING "DEBUG2: " s "\r\n", \
			##__VA_ARGS__); \
} while (0)

/* Extra more debugging, usally only used inside loops */
#define logdebug3(s, ...) \
do { \
	if (CONFIG_UART_ENABLED && CONFIG_DEBUG_LOG >= 3) \
		_uart_printf(CONFIG_UART_STRING "DEBUG3: " s "\r\n", \
			##__VA_ARGS__); \
} while (0)

#endif /* __LOG_H__ */

