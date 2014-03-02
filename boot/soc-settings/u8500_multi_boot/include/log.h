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
#include <uart.h>

/* Logging macros */
#define logerror(s, ...) \
do { \
	_uart_printf(LOG_ERROR, s, ##__VA_ARGS__); \
} while (0)

#define logmemerr(s, addr, value, shouldbe, ...) \
do { \
	_uart_printf(LOG_MEMERR, s,				\
		     " Address=%x Value=%x Should be=%x",	\
		     ##__VA_ARGS__, addr, value, shouldbe);	\
} while (0)

#define logwarn(s, ...) \
do { \
	_uart_printf(LOG_WARN, s, ##__VA_ARGS__); \
} while (0)

#define loginfo(s, ...) \
do { \
	_uart_printf(LOG_INFO, s, ##__VA_ARGS__); \
} while (0)

#define logdebug(s, ...) \
do { \
	_uart_printf(LOG_DEBUG_LVL1, s, ##__VA_ARGS__); \
} while (0)

/* More debugging */
#define logdebug2(s, ...) \
do { \
	_uart_printf(LOG_DEBUG_LVL2, s, ##__VA_ARGS__); \
} while (0)

/* Extra more debugging, usally only used inside loops */
#define logdebug3(s, ...) \
do { \
	_uart_printf(LOG_DEBUG_LVL3, s, ##__VA_ARGS__); \
} while (0)

#endif /* __LOG_H__ */

