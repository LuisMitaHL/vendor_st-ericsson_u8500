/************************************************************************
 *                                                                      *
 *  Copyright (C) 2010 ST-Ericsson                                      *
 *                                                                      *
 *  Author: Joakim AXELSSON <joakim.axelsson AT stericsson.com>         *
 *  Author: Sebastian RASMUSSEN <sebastian.rasmussen AT stericsson.com> *
 *                                                                      *
 ************************************************************************/

#include "log.h"

#include "likely.h"

#include <errno.h>
#include <stdbool.h>
#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <syslog.h>
#include <stdio.h>
#include <limits.h>

#define TAG "[MSA]"

// Used by macros to not be able to save function calls. Global variables, and
// default values
int                    log_global_debugLvl = 0;

// Local log state
static bool            log_toSyslog = false;
static LogToConsole_t  log_toConsole = LOGTOCONSOLE_STDERR;
static FILE           *log_toFile = NULL;

static const char * const lvlStr[] = {
	"Success", "FAILED", "FATAL!", "ERROR", "WARN", "Info", "INVALID",
	"Request", "Response", "Path", "DBG1", "DBG2", "DBG3", "HEX" };

static const int lvlToPriority[] = {
	LOG_DEBUG   | LOG_USER, // LVL_SUCCESS
	LOG_NOTICE  | LOG_USER, // LVL_FAILED
	LOG_ALERT   | LOG_USER, // LVL_FATAL
	LOG_ERR     | LOG_USER, // LVL_ERROR
	LOG_WARNING | LOG_USER, // LVL_WARN
	LOG_INFO    | LOG_USER, // LVL_INFO
	LOG_ERR     | LOG_USER, // LVL_INVALID
	LOG_DEBUG   | LOG_USER, // LVL_REQ
	LOG_DEBUG   | LOG_USER, // LVL_RESPONSE
	LOG_DEBUG   | LOG_USER, // LVL_PATH
	LOG_DEBUG   | LOG_USER, // LVL_DBG1
	LOG_DEBUG   | LOG_USER, // LVL_DBG2
	LOG_DEBUG   | LOG_USER, // LVL_DBG3
	LOG_DEBUG   | LOG_USER, // LVL_HEX
};

void log_setDebugLevel(int debugLvl)
{
	log_global_debugLvl = debugLvl;
}

void log_setLogToConsole(LogToConsole_t logToConsole)
{
	log_toConsole = logToConsole;
}

void log_setLogToSyslog(bool logToSyslog)
{
	//First close the any old syslog usage
	if ( log_toSyslog )
	{
		closelog();
	}

	log_toSyslog = logToSyslog;
	if ( log_toSyslog )
	{
		openlog(TAG, LOG_PID | LOG_NDELAY, LOG_USER);
	}
}

void log_setLogToFile(const char *path)
{
	//First close any former open logfile
	if ( log_toFile )
	{
		fclose(log_toFile);
		log_toFile = NULL;
	}

	if ( path )
	{
		log_toFile = fopen(path, "a");
		if ( log_toFile == NULL )
		{
			logERRNO(errno, "Can't open logfile '%s'.", path);
		}
	}
}

void log_closeAllLog(void)
{
	if ( log_toSyslog )
	{
		closelog();
		log_toSyslog = false;
	}

	log_toConsole = LOGTOCONSOLE_DISABLED;

	if ( log_toFile )
	{
		fclose(log_toFile);
		log_toFile = NULL;
	}
}

void log_msg(LogLevel_t lvl, const char *fmt, ...)
{
	char strBuf[BUFSIZ]; // We allocate a large amount on stack, is FSA_OK in Linux
	int num;
	va_list vl;

	assert(lvl < LVL_MAX);
	assert(fmt);

	// syslog() might change it
	int savedErrno = errno;

	num = snprintf(strBuf, BUFSIZ, "%s ", lvlStr[lvl]);

	va_start(vl, fmt);
	vsnprintf(strBuf + num, BUFSIZ - num, fmt, vl);
	va_end(vl);

	//syslog
	if ( log_toSyslog )
	{
	    syslog(lvlToPriority[lvl], "%s", strBuf);
	}

	//console
	if ( log_toConsole == LOGTOCONSOLE_STDOUT )
	{
		fprintf(stdout, TAG " %s\n", strBuf);
		fflush(stdout);
	}
	else if ( log_toConsole == LOGTOCONSOLE_STDERR )
	{
		fprintf(stderr, TAG " %s\n", strBuf);
		fflush(stderr);
	}

	//logfile
	if ( log_toFile )
	{
		fprintf(log_toFile, "%s\n", strBuf);
		fflush(log_toFile);
	}

	errno = savedErrno;
}


#define MAXHEXDUMP 128
void log_hexdump(void *buf, size_t len)
{
	char  str[80];
	char *ptr;
	uint8_t *data = (uint8_t *) buf;
	size_t i = 0, k;
	int increment;

	assert(buf);

	while (i < len)
	{
		ptr = str;
		ptr += sprintf(ptr, "%07zx: ", i);

		for (k = 0; k < 16; k++)
			if (i + k < len)
				ptr += sprintf(ptr, "%02hhx ", data[k]);
			else
				ptr += sprintf(ptr, "   ");

		ptr += sprintf(ptr, " ");
		for (k = 0; k < 16; k++)
			if (i + k < len && data[k] >= ' ' && data[k] <= '~')
				ptr += sprintf(ptr, "%c", data[k]);
			else if (i + k < len)
				ptr += sprintf(ptr, ".");
			else
				ptr += sprintf(ptr, " ");

		logHEX("%s", str);

		if (SIZE_MAX - i > 16)
			increment = 16;
		else
			increment = SIZE_MAX - i;
		data += increment;
		i += increment;

		//We will only hexdump first MAXHEXDUMP bytes
		// if debugLvl isn't at least 5.
		if ( unlikely(log_global_debugLvl < 5 && i >= MAXHEXDUMP) )
		{
			logHEX("Only hex-dumping %u first bytes. Len was %u bytes.",
				MAXHEXDUMP, len);
			break;
		}
	}
}

