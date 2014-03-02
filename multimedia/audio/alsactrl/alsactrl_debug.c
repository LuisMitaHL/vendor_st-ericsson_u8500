/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */
/*****************************************************************************/

#include <stdio.h>
#include <string.h>

#include <cutils/log.h>
#include <cutils/properties.h>

#include "alsactrl.h"
#include "alsactrl_debug.h"

#define LOG_TAG "ALSACTRL"
#define BUFFER_LENGTH 2048
#define PRINT_FORMAT "%s: %s%s"

/**
* File stream pointer used for logging to file.
*/
unsigned int g_alsactrl_debug_mask;

char *log_prefix[ALSACTRL_LOG_LAST] = {
	"INFO",
	"WARN",
	"ERR",
};

#ifndef ALSACTRL_LOG_TO_FILE
#define LOG_TO_FILE_DEFINED 0
#else
#define LOG_TO_FILE_DEFINED 1
#endif

/*! \def LOG_INFO_DEFINED

    Helper macro to avoid \#ifdef usage for conditional compilation
*/
#ifndef ALSACTRL_LOG_INFO
#define LOG_INFO_DEFINED 0
#else
#define LOG_INFO_DEFINED 1
#endif

/*! \def LOG_WARN_DEFINED

    Helper macro to avoid \#ifdef usage for conditional compilation
*/
#ifndef ADM_LOG_WARNINGS
#define LOG_WARN_DEFINED 0
#else
#define LOG_WARN_DEFINED 1
#endif

/*! \def LOG_ERR_DEFINED

    Helper macro to avoid \#ifdef usage for conditional compilation
*/
#ifndef ALSACTRL_LOG_ERRORS
#define LOG_ERR_DEFINED 0
#else
#define LOG_ERR_DEFINED 1
#endif

static const char* get_prop(const char* key)
{
	static char prop_static_buf[PROPERTY_VALUE_MAX];

	property_get(key, prop_static_buf, "");
	prop_static_buf[PROPERTY_VALUE_MAX-1] = 0; // not sure if needed for property_get

	return prop_static_buf;
}

static void set_logging_defaults(void)
{
	g_alsactrl_debug_mask =
		(LOG_INFO_DEFINED << ALSACTRL_LOG_INFO) |
		(LOG_WARN_DEFINED << ALSACTRL_LOG_WARN) |
		(LOG_ERR_DEFINED << ALSACTRL_LOG_ERR);
}

void alsactrl_dbg_set_logs_from_properties()
{
	set_logging_defaults();
	const char* name_start = get_prop("ste.debug.alsactrl.log");

	while (*name_start) {
		int log_type;
		const char* name_end = name_start;

		while (*name_end && *name_end != ' ') name_end++;
		for (log_type = 0; log_type < ALSACTRL_LOG_LAST; log_type++) {
			if ((size_t)(name_end-name_start) == strlen(log_prefix[log_type]) &&
				strncasecmp(log_prefix[log_type], name_start, (size_t)(name_end-name_start)) == 0)
			{
				g_alsactrl_debug_mask |= 1U << log_type;
			}
		}

		name_start = name_end;
		while (*name_start && *name_start == ' ') name_start++;
	}
}

int alsactrl_debug_is_log_enabled(enum ALSACTRL_LOGLEVEL level)
{
	return (g_alsactrl_debug_mask & (1U << level)) ? 1 : 0;
}

int alsactrl_debug_print(enum ALSACTRL_LOGLEVEL level, const char *file, int line, const char *function, const char *fmt, ...)
{
	va_list ap;
	char buffer[BUFFER_LENGTH];
	char insert_newline[2] = {0, 0};

	UNUSED_PAR(file);
	UNUSED_PAR(line);

	va_start(ap, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, ap);
	va_end(ap);

	if (buffer[strlen(buffer)-1] != '\n')
		insert_newline[0] = '\n';

	switch (level) {
	case ALSACTRL_LOG_ERR:
		ALOGE(PRINT_FORMAT, function, buffer, insert_newline);
	break;
	case ALSACTRL_LOG_WARN:
	case ALSACTRL_LOG_INFO:
	default:
		ALOGD(PRINT_FORMAT, function, buffer, insert_newline);
	break;
	}

	return 0;
}
