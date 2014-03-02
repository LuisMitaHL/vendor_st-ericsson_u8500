#ifndef _LOG_H
#define _LOG_H

/************************************************************************
 *                                                                      *
 *  Copyright 2010 (C) ST-Ericsson                                      *
 *                                                                      *
 *  Author: Joakim AXELSSON <joakim.axelsson AT stericsson.com>         *
 *  Author: Sebastian RASMUSSEN <sebastian.rasmussen AT stericsson.com> *
 *                                                                      *
 ************************************************************************/

#include "likely.h"

#include <stdbool.h>
#include <stddef.h>

/**********************************************
 * Types
 **********************************************/

typedef enum
{
	LOGTOCONSOLE_DISABLED = 0,
	LOGTOCONSOLE_STDOUT,
	LOGTOCONSOLE_STDERR,
} LogToConsole_t;

typedef enum
{
	LVL_SUCCESS = 0,
	LVL_FAILED,

	LVL_FATAL,
	LVL_ERROR,
	LVL_WARN,
	LVL_INFO,
	LVL_INVALID, //Packet received is invalid
	LVL_REQ,
	LVL_RESP,
	LVL_PATH,
	LVL_DBG1,
	LVL_DBG2,
	LVL_DBG3,
	LVL_HEX,

	LVL_MAX
} LogLevel_t;


/**********************************************
 * Extern variables
 **********************************************/

// Used by macros to not pass a pointer to the config to all functions
//They should ONLY be used by below macros. Not ever directly. Useed the
// functions below to manipulate them.
extern int	log_global_debugLvl;	// Debug level, set by command line


/**********************************************
 * Macros
 **********************************************/

#define logFATAL(fmt, args...) \
	log_msg(LVL_FATAL, fmt, ##args)

#define logERROR(fmt, args...) \
	log_msg(LVL_ERROR, fmt, ##args)

#define logWARN(fmt, args...) \
	log_msg(LVL_WARN, fmt, ##args)

#define logINFO(fmt, args...) \
	log_msg(LVL_INFO, fmt, ##args)

#define logREQ(proc, fmt, args...) \
	do { if ( unlikely(log_global_debugLvl > 0) ) \
		log_msg(LVL_REQ, #proc " " fmt, ##args); \
	} while (0)

#define logRESP(proc, fmt, args...) \
	do { if ( unlikely(log_global_debugLvl > 0) ) \
		log_msg(LVL_RESP, #proc " " fmt, ##args); \
	} while (0)

//Used to log translations between handle and paths
#define logPATH(proc, fmt, args...) \
	do { if ( unlikely(log_global_debugLvl > 0) ) \
		log_msg(LVL_PATH, #proc " " fmt, ##args); \
	} while (0)

#define logINVALID(fmt, args...) \
	log_msg(LVL_INVALID, fmt, ##args)

#define logDBG1(fmt, args...) \
	do \
	{ \
		if (unlikely(log_global_debugLvl > 0)) \
			log_msg(LVL_DBG1, fmt, ##args); \
	} while (0)

#define logDBG2(fmt, args...) \
	do \
	{ \
		if (unlikely(log_global_debugLvl > 1)) \
			log_msg(LVL_DBG2, fmt, ##args); \
	} while (0)

#define logDBG3(fmt, args...) \
	do \
	{ \
		if (unlikely(log_global_debugLvl > 2)) \
			log_msg(LVL_DBG3, fmt, ##args); \
	} while (0)

#define logHEX(fmt, args...) \
	do \
	{ \
		if (unlikely(log_global_debugLvl > 3)) \
			log_msg(LVL_HEX, fmt, ##args); \
	} while (0)

#define logFAIL(fmt, args...) \
	log_msg(LVL_FAILED, fmt, ##args)

#define logERRNO(errno, fmt, args...) \
	logINFO(fmt " errno=%d (%s)", ##args, errno, strerror(errno))


// Need debugLvl 2 in order to see successful operations.
#define logSUCC(fmt, args...) \
	do \
	{ \
		if (unlikely(log_global_debugLvl > 1)) \
			log_msg(LVL_SUCCESS, fmt, ##args); \
	} while (0)


#define logHexDump(data, len) \
	do \
	{ \
		if (unlikely(log_global_debugLvl > 3)) \
			log_hexdump(data, len); \
	} while (0)


/**********************************************
 * Functions
 **********************************************/

void log_setDebugLevel(int debugLvl);

void log_setLogToConsole(LogToConsole_t logToConsole);
void log_setLogToSyslog(bool logToSyslog);
void log_setLogToFile(const char *path);


void log_msg(LogLevel_t lvl, const char *fmt, ...);
void log_hexdump(void *buf, size_t len);

void log_closeAllLog(void);




#endif // _LOG_H

