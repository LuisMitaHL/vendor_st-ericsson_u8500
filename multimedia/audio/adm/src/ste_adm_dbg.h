/*
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*/
/*! \file ste_adm_dbg.h
    \brief Debugging utilities.

    Print macros etc.
*/
#ifndef STE_ADM_DBG_INCLUSION_GUARD_H
#define STE_ADM_DBG_INCLUSION_GUARD_H

#include <stdio.h>

#ifdef ADM_DBG_X86
    #include <assert.h>
#endif

/**
* Enum containing all available log levels.
*/
enum STE_ADM_LOGLEVEL {
    STE_ADM_LOG_INFO_VERBOSE,
    STE_ADM_LOG_INFO,
    STE_ADM_LOG_D2D_VERBOSE,
    STE_ADM_LOG_D2D,
    STE_ADM_LOG_OMXIL_VERBOSE,
    STE_ADM_LOG_OMXIL,
    STE_ADM_LOG_OMXIL_FLOW,
    STE_ADM_LOG_ADIL,
    STE_ADM_LOG_ADIL_FLOW,
    STE_ADM_LOG_ADIL_INT,
    STE_ADM_LOG_ADIL_DUMP,
    STE_ADM_LOG_DB_VERBOSE,
    STE_ADM_LOG_DB,
    STE_ADM_LOG_STATUS,
    STE_ADM_LOG_WARN,
    STE_ADM_LOG_ERR,
    STE_ADM_LOG_LAST
};

#ifdef __cplusplus
  extern "C" {
#endif


/**
* Rescans "ste.debug.adm.log".
*
* @return Success: 0 Failure: -1
*/
void dbg_set_logs_from_properties();

/**
* Return 1 if a certain type of log is enabled, 0 otherwise
*/
int ste_adm_debug_is_log_enabled(enum STE_ADM_LOGLEVEL level);


/**
* Return != 0 if OST traces should be enabled for component of type name,
* used as specified type (e.g. 'common' or 'app' for the mixer).
*/
int ste_adm_debug_omx_trace_enabled(const char* name, const char* type);

/**
* Reboots ADM
*/
void ste_adm_debug_reboot(void);

void ste_adm_debug_print_callstack(void);

/**
* Logs data to output and/or file depending on current settings and level.
* Do not use directly, instead use the appropriate LOG_x macro.
*
* @param level The loglevel of the message, must be a value from the enum STE_ADM_LOGLEVEL
* @param file The filename where the print was triggered
* @param line Line number where the print was triggered
* @param function Functation name where the print was triggered
* @param fmt String to be printed, printf style
*
* @return Success: 0 Failure: -1
*/
int ste_adm_debug_print(enum STE_ADM_LOGLEVEL level, const char *file, int line, const char *function, const char *fmt, ...);

/**
* Checks the read en of the pipe where external prints from ie. ALSA are written,
* outputs data to the standard logging facilities when a newline is encountered.
*/
void ste_adm_check_extern_print_buff(void);

/**
* Return an fd that can be used in select() to receive events when there is data
* available to be read. No actual reading should be done from this fd as this is handled
* in the ste_adm_check_extern_print_buff(void) function.
*
* @param [out] fd A duplicate of the fd on the read end of the pipe
* @return Success: 0 Failure: -1
*/
int ste_adm_get_debug_listen_fd(int *fd);

/**
* This function will print various ADM debug information to logcat
* for example:
*   ADIL state
*   DMA / MSP registers
*   AB8500 registers
*   ADM circular buffer (last commands)
*/
void dbg_print_debug_information(void);


#define DUMP_RAW_DEFINED 0

#ifdef __GNUC__
  #define unlikely(x) __builtin_expect((x), 0)
  #define VISIBILITY_HIDDEN __attribute__ ((visibility("hidden")))
#else
  #define unlikely(x) (x)
  #define VISIBILITY_HIDDEN
#endif

// Read by all dynamic prints (ADM_LOG_BASE macro). We want low
// overhead for prints when they are disabled.
//
// visibility=hidden is important, since otherwise accesses to
// g_ste_adm_debug_mask are more indirect via a GOT table (since
// it becomes a shared symbol that might become overriden during dynamic
// linking)
//
// __builtin_expect((x), 0) is important, since we otherwise get branching
// each time a print is skipped.
//
// obviosly, the volatile attribute would slow things down. In practice, the
// new debug setting propagates essentially instantly anyway.
extern unsigned int g_ste_adm_debug_mask VISIBILITY_HIDDEN;

/**
* Internal helper macro for logging, do not use!
*/
#ifdef ADM_LOG_FILENAME
#define ADM_LOG_BASE(__LEVEL, ...)                                     \
    do {                          \
        if (unlikely(g_ste_adm_debug_mask & (1U<< (unsigned int) __LEVEL))) { \
          ste_adm_debug_print(__LEVEL, ADM_LOG_FILENAME, __LINE__, __func__, __VA_ARGS__); \
        } \
    } while(0)
#else
#define ADM_LOG_BASE(__LEVEL, ...)                                     \
    do {                          \
        if (unlikely(g_ste_adm_debug_mask & (1U<< (unsigned int) __LEVEL))) { \
          ste_adm_debug_print(__LEVEL, __FILE__, __LINE__, __func__, __VA_ARGS__); \
       } \
    } while(0)
#endif



// #define ADM_DEBUG
// Due to performance reasons, compile away this print completely unless
// ADM_DEBUG is set.
#ifdef ADM_DEBUG
    #define ALOG_INFO_VERBOSE(...)                                   \
        do {                                                        \
            ADM_LOG_BASE(STE_ADM_LOG_INFO_VERBOSE, __VA_ARGS__);    \
        } while (0)
#else
    #define ALOG_INFO_VERBOSE(...)
#endif

#define ALOG_INFO(...)                                \
    do {                                             \
        ADM_LOG_BASE(STE_ADM_LOG_INFO, __VA_ARGS__); \
    } while (0)


#define ALOG_D2D_VERBOSE(...)                                  \
    do {                                                      \
        ADM_LOG_BASE(STE_ADM_LOG_D2D_VERBOSE, __VA_ARGS__);   \
    } while (0)


#define ALOG_D2D(...)                                  \
    do {                                              \
        ADM_LOG_BASE(STE_ADM_LOG_D2D, __VA_ARGS__);   \
    } while (0)


#define ALOG_OMXIL_VERBOSE(...)                                  \
    do {                                                        \
        ADM_LOG_BASE(STE_ADM_LOG_OMXIL_VERBOSE, __VA_ARGS__);   \
    } while (0)


#define ALOG_OMXIL(...)                                 \
    do {                                               \
        ADM_LOG_BASE(STE_ADM_LOG_OMXIL, __VA_ARGS__);  \
    } while (0)


#define ALOG_OMXIL_FLOW(...)                                \
    do {                                                    \
        ADM_LOG_BASE(STE_ADM_LOG_OMXIL_FLOW, __VA_ARGS__);  \
    } while (0)


#define ALOG_ADIL(...)                                      \
    do {                                                    \
        ADM_LOG_BASE(STE_ADM_LOG_ADIL, __VA_ARGS__);  \
    } while (0)


#define ALOG_ADIL_FLOW(...)                                      \
    do {                                                    \
        ADM_LOG_BASE(STE_ADM_LOG_ADIL_FLOW, __VA_ARGS__);  \
    } while (0)


#define ALOG_ADIL_INT(...)                                      \
    do {                                                    \
        ADM_LOG_BASE(STE_ADM_LOG_ADIL_INT, __VA_ARGS__);  \
    } while (0)

#define ALOG_ADIL_DUMP(...)                                      \
    do {                                                    \
        ADM_LOG_BASE(STE_ADM_LOG_ADIL_DUMP, __VA_ARGS__);  \
    } while (0)

#define ALOG_DB_VERBOSE(...)                                 \
    do {                                                    \
        ADM_LOG_BASE(STE_ADM_LOG_DB_VERBOSE, __VA_ARGS__);  \
    } while (0)


#define ALOG_DB(...)                                 \
    do {                                            \
        ADM_LOG_BASE(STE_ADM_LOG_DB, __VA_ARGS__);  \
    } while (0)


#define ALOG_STATUS(...)                                 \
    do {                                                \
        ADM_LOG_BASE(STE_ADM_LOG_STATUS, __VA_ARGS__);  \
    } while (0)


#define ALOG_WARN(...)                                   \
    do {                                                \
        ADM_LOG_BASE(STE_ADM_LOG_WARN, __VA_ARGS__);    \
    } while (0)


#define ALOG_ERR(...)                                                       \
    do {        \
        ADM_LOG_BASE(STE_ADM_LOG_ERR, __VA_ARGS__); \
    } while (0)


#ifdef __cplusplus
  }
#endif

#ifdef ADM_DBG_X86
    #define ADM_ASSERT assert
#else
    #define ADM_ASSERT(__adm_assert_x) \
        do { if( !(__adm_assert_x)) { ALOG_ERR("ASSERT ERROR '%s'\n", # __adm_assert_x); ste_adm_debug_print_callstack();} } while(0)
#endif


#endif /* STE_ADM_DBG_INCLUSION_GUARD_H */
