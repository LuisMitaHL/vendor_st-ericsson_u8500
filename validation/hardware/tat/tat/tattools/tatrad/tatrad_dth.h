/*****************************************************************************/
/**
*  © ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file	   {@filename}.h
* \brief   DTH registration and utilities for {@module}
* \author  ST-Ericsson
*/
/*****************************************************************************/
#ifndef {@FILENAME}_H__
#define {@FILENAME}_H__

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <syslog.h>

#include <dth_tools.h>
#include <dthsrvhelper/dthsrvhelper.h> /* for dth_element definition */
#include <dthsrvhelper/taterror.h>

/* Path of the DTH elements. To be passed as argument to tatasync */
{@dth_paths}

/* ID of the DTH elements. An ID is passed in the user_data attribute in a
dth_element structure. */
{@dth_ids}

/* Enumeration codes for DTH actions having more than one value */

/* system is unusable */
#define ENUM_{@dthroot}_LOG_EMERG     0
/* action must be taken immediately */
#define ENUM_{@dthroot}_LOG_ALERT     1
/* critical conditions */
#define ENUM_{@dthroot}_LOG_CRIT      2
/* error conditions */
#define ENUM_{@dthroot}_LOG_ERR       3
/* warning conditions */
#define ENUM_{@dthroot}_LOG_WARNING   4
/* normal, but significant, condition */
#define ENUM_{@dthroot}_LOG_NOTICE    5
/* informational message */
#define ENUM_{@dthroot}_LOG_INFO      6
/* debug message */
#define ENUM_{@dthroot}_LOG_DEBUG     7

{@dth_enums}

/*++tatrad:syslog update=create */

int g_{@prefix}syslog_level;
void  {@prefix}syslog(int vp_prio, int vp_output, const char *pp_func,
    const char *pp_file, int vp_line, const char *pp_format, ...);

/* syslog option for TAT */
#define LOG_TAT_OPTION      ( LOG_NDELAY | LOG_PID )

/* syslog facilty for TAT */
#define LOG_TAT_FACILITY    LOG_LOCAL7

/* log outputs flags */
#define {@dthroot}_LOG_OUTPUT         0x0000000F
#define {@dthroot}_LOG_CONSOLE        0x00000001
#define {@dthroot}_LOG_STACK          0x00000002
#define {@dthroot}_LOG_SYSLOG         0x00000004
#define {@dthroot}_LOG_ALL            {@dthroot}_LOG_OUTPUT

#define {@dthroot}_LOG_OPTION         0xFFFF0000
#define {@dthroot}_LOG_TEXTONLY       0x00010000

#define {@dthroot}_SYSLOG_COND ({@prefix}syslog_get_level())

#define {@dthroot}_SYSLOGLOC(prio)                                             \
    {@prefix}syslog((prio), {@dthroot}_LOG_CONSOLE, __func__, __FILE__,    \
        __LINE__, "")

/* prio : level (defined in <syslog.h>)
This determines the importance of the message. The levels are, in order of
decreasing importance: 
LOG_EMERG       system is unusable 
LOG_ALERT       action must be taken immediately 
LOG_CRIT        critical conditions 
LOG_ERR         error conditions 
LOG_WARNING     warning conditions 
LOG_NOTICE      normal, but significant, condition 
LOG_INFO        informational message 
LOG_DEBUG       debug-level message
*/

#define {@dthroot}_SYSLOGSTR(prio, fmt , __args__...) \
    {@prefix}syslog((prio), {@dthroot}_LOG_CONSOLE|{@dthroot}_LOG_TEXTONLY, \
        __func__, __FILE__, __LINE__, fmt, ##__args__)

#define {@dthroot}_SYSLOG(prio, fmt , __args__...)  \
    {@prefix}syslog((prio), {@dthroot}_LOG_ALL, __func__, __FILE__, \
        __LINE__, fmt, ##__args__)

/* Dereference pointer */
#define {@dthroot}_DEREF_PTR(p, t) (*((t *)(p) ))

/* Dereference and assign pointer */
#define {@dthroot}_DEREF_PTR_SET(p, t, v) (*((t *)p) = (v))

/* DTH_GET_SIGNED(<scalar>, struct dth_element*, void*, int&) */
#define {@dthroot}_DTH_GET_SIGNED(vp_value, pp_elem, pp_dth, var_result) \
    do { \
        var_result = {@prefix}get_signed(vp_value, pp_elem, pp_dth); \
    } while (0);

/* DTH_SET_SIGNED(struct dth_elemet*, void*, <scalar>&, int&) */
#define {@dthroot}_DTH_SET_SIGNED(pp_elem, pp_dth, var_value, var_result) \
    do { \
        var_value = {@prefix}set_signed(pp_elem, pp_dth, &var_result); \
    } while (0);

/* DTH_GET_UNSIGNED(<scalar>, struct dth_element*, void*, int&) */
#define {@dthroot}_DTH_GET_UNSIGNED(vp_value, pp_elem, pp_dth, var_result) \
    do { \
        var_result = {@prefix}get_unsigned(vp_value, pp_elem, pp_dth); \
    } while (0);

/* DTH_SET_UNSIGNED(struct dth_elemet*, void*, <scalar>&, int&) */
#define {@dthroot}_DTH_SET_UNSIGNED(pp_elem, pp_dth, var_value, var_result) \
    do { \
        var_value = {@prefix}set_unsigned(pp_elem, pp_dth, &var_result); \
    } while (0);

/* DTH_GET_FLOAT(float, struct dth_element*, void*, int&) */
#define {@dthroot}_DTH_GET_FLOAT(vp_value, pp_elem, pp_dth, var_result) \
    do { \
        var_result = {@prefix}get_float(vp_value, pp_elem, pp_dth); \
    } while (0);

/* DTH_SET_FLOAT(struct dth_elemet*, void*, float&, int&) */
#define {@dthroot}_DTH_SET_FLOAT(pp_elem, pp_dth, var_value, var_result) \
    do { \
        var_value = {@prefix}set_float(pp_elem, pp_dth, &var_result); \
    } while (0);

int {@prefix}get_signed(int64_t value, struct dth_element*, void *pp_value);
int {@prefix}get_unsigned(uint64_t value, struct dth_element*, void *pp_value);
int {@prefix}get_float(float value, struct dth_element*, void *pp_value);
float {@prefix}set_float(struct dth_element*, void *pp_value, int *pp_result);
int64_t {@prefix}set_signed(struct dth_element*, void *pp_value,
    int *pp_result);
uint64_t {@prefix}set_unsigned(struct dth_element*, void *pp_value,
    int *pp_result);

int {@prefix}syslog_get_level(void);
void {@prefix}syslog_set_level(int);

/*--tatrad:syslog */

int {@prefix}register_dth_struct(void);
int {@prefix}unregister_dth_struct(void);

/* handler for get or set that does nothing but return TAT_ERROR_OFF */
int {@prefix}get_set_null(struct dth_element *elem, void *value);
/* exec that does nothing but return TAT_ERROR_OFF */
int {@prefix}exec_null(struct dth_element *elem);

/* default DTH interface for debug */
int {@prefix}get_debug(struct dth_element *elem, void *value);
int {@prefix}set_debug(struct dth_element *elem, void *value);

/* default DTH interface for versioning */
int {@prefix}get_version(struct dth_element *elem, void *value);

#endif
/* END OF FILE */
