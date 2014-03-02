/*
 * erasedrv.h
 *
 *  Created on: 25.11.2009
 *      Author: rohacmic
 */

#ifndef ERASEDRV_H_
#define ERASEDRV_H_

#include <windows.h>
#include <winver.h>
#include <tchar.h>
#include <stdlib.h>
#include <stdio.h>
#include <setupapi.h>

#define PRG_NAME "INF File Uninstaller"

#define SWITCH_OPT_GET "/g"
#define SWITCH_OPT_REMOVE "/r"

#define LOG_BUF_LENGTH 1024
TCHAR logBuffer[LOG_BUF_LENGTH];

inline void infoLog(TCHAR * fstring, ...) {
	va_list args;
	va_start(args, fstring);

	if (_vstprintf(logBuffer, fstring, args) < 0) {
		_stprintf(logBuffer, "");
	}
	_tprintf("\n INFO: %s", logBuffer);
	va_end(args);
}

inline void errorLog(TCHAR * fstring, ...) {
	va_list args;
	va_start(args, fstring);
	if (_vstprintf(logBuffer, fstring, args) < 0) {
		_stprintf(logBuffer, "");
	}
	_tprintf("\nERROR: %s", logBuffer);
	va_end(args);
}

inline void warnLog(TCHAR * fstring, ...) {
	va_list args;
	va_start(args, fstring);
	if (_vstprintf(logBuffer, fstring, args) < 0) {
		_stprintf(logBuffer, "");
	}
	_tprintf("\n WARN: %s", logBuffer);
	va_end(args);
}

enum {
	OPT_UNDEF = -1,
	OPT_GET = 1,
	OPT_REMOVE
} PRG_OPTIONS;


#endif /* ERASEDRV_H_ */
