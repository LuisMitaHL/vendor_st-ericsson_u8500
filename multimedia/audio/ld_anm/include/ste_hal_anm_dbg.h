/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*! \file ste_hal_anm_dbg.h
    \brief Debugging utilities.

    Print macros etc.
    Logs are enabled using setprop.
    Example: setprop ste.debug.anm.log "INFO INFO_V"
*/
#ifndef STE_HAL_ANM_DBG_INCLUSION_GUARD_H
#define STE_HAL_ANM_DBG_INCLUSION_GUARD_H

/**
* LOG TAG used to enable ANM debug prints on target
*/

#include <linux_utils.h>
#include <stdio.h>
#include <string.h>


/**
* Enum containing all available log levels.
*/
enum STE_ANM_LOGLEVEL {
    STE_ANM_LOG_INFO,
    STE_ANM_LOG_INFO_VERBOSE,
    STE_ANM_LOG_INFO_FL,
    STE_ANM_LOG_WARN,
    STE_ANM_LOG_ERR,
    STE_ANM_LOG_LAST
};

/**
* Sets up log file to be used for daemon debug printing. This should not fail.
*
* @return Success: 0 Failure: -1
*/
int ste_anm_debug_setup_log();

/**
* Logs name to output and/or file depending on current settings and level.
* Do not use directly, instead use the appropriate LOG_x macro.
*
* @param level      The loglevel of the message, must be a value from the
*                   enum STE_ANM_LOGLEVEL
* @param file       The filename where the print was triggered
* @param line       Line number where the print was triggered
* @param function   Functation name where the print was triggered
* @param fmt        String to be printed, printf style
*
* @return Success: 0 Failure: -1
*/
int ste_anm_debug_print(enum STE_ANM_LOGLEVEL level, const char *file,
    int line, const char *function, const char *fmt, ...);

/**
* Internal helper macro for logging, do not use!
*/
#ifdef ANM_LOG_FILENAME
#define ANM_LOG_BASE(__LEVEL, ...)                                     \
    do {                          \
        ste_anm_debug_print(__LEVEL, ANM_LOG_FILENAME, __LINE__, __func__, __VA_ARGS__); \
    } while(0)
#else
#define ANM_LOG_BASE(__LEVEL, ...)                                     \
    do {                          \
        ste_anm_debug_print(__LEVEL, __FILE__, __LINE__, __func__, __VA_ARGS__); \
    } while(0)
#endif

/*! \def ALOG_INFO(x)

    Wrapper macro for logging prints, to be used for debugging only.
    Set the flags LOG_INFO to enable. Variadic macro
    is defined according to ISO C99 standard instead of the more readable GCC
    way because of SwAG guidelines. Output goes to stdout.
*/
#define ALOG_INFO(...)                                                      \
        do {                                                                \
            ANM_LOG_BASE(STE_ANM_LOG_INFO, __VA_ARGS__);                    \
        } while (0)

/*! \def ALOG_INFO_VERBOSE_(x)

    Wrapper macro for logging prints, to be used for debugging only.
    Set the flags LOG_INFO_VERBOSE to enable. Variadic macro
    is defined according to ISO C99 standard instead of the more readable GCC
    way because of SwAG guidelines. Output goes to stdout.
*/
#define ALOG_INFO_VERBOSE(...)                                              \
        do {                                                                \
            ANM_LOG_BASE(STE_ANM_LOG_INFO_VERBOSE, __VA_ARGS__);            \
        } while (0)

/*! \def ALOG_INFO_FL_(x)

    Wrapper macro for logging prints, to be used for debugging only.
    Set the flags LOG_INFO_FL to enable. Variadic macro
    is defined according to ISO C99 standard instead of the more readable GCC
    way because of SwAG guidelines. Output goes to stdout.
*/
#define ALOG_INFO_FL(...)                                              \
        do {                                                                \
            ANM_LOG_BASE(STE_ANM_LOG_INFO_FL, __VA_ARGS__);            \
        } while (0)

/*! \def ALOG_WARN(x)

    Wrapper macro for logging prints, displays non-critical errors.
    Set the flags LOG_WARNINGS to enable. Variadic macro
    is defined according to ISO C99 standard instead of the more readable GCC
    way because of SwAG guidelines. Output goes to stdout.
*/
#define ALOG_WARN(...)                                                      \
        do {                                                                \
            ANM_LOG_BASE(STE_ANM_LOG_WARN, __VA_ARGS__);                    \
        } while (0)

/*! \def ALOG_ERR(x)

    Wrapper macro for logging prints, displays errors.
    Set the flags LOG_ERRORS to enable. Variadic macro
    is defined according to ISO C99 standard instead of the more readable GCC
    way because of SwAG guidelines. Output goes to stderr.
*/
#define ALOG_ERR(...)                                                       \
        do {                                                                \
            ANM_LOG_BASE(STE_ANM_LOG_ERR, __VA_ARGS__);                     \
        } while (0)



#endif /* STE_HAL_ANM_DBG_INCLUSION_GUARD_H */
