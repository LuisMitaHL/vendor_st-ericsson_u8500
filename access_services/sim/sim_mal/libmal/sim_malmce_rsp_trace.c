/*                               -*- Mode: C -*-
 * Copyright (C) 2010, ST-Ericsson
 *
 * File name       : sim_malmce_rsp_trace.c
 * Description     : Utility trace
 *
 * Author          : Steve Critchlow <steve.critchlow@stericsson.com>
 *
 */

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "simd.h" // For event stream, logging functions etc.
#include "sim_mal_defs.h"
#include "sim_trace_util.h"

#include "mal_mce.h"
#include "sim_log.h"


#ifndef LOCAL_COMPILE_TEST
#define LOG_TAG "RIL MAL-MCE"   // CallBackDumper
#include <cutils/log.h>
#else
#define LOGI(x)
#define LOGD(x)
#define LOGV(x)
#endif

static void log_f(sim_logging_t level, const char *fmt, ...)
    __attribute__ ((format (printf, 2, 3)));

static void log_f(sim_logging_t level, const char *fmt, ...)
{
    char    buf[1024];
    va_list arg;
    size_t  len;

    if ( level > sim_get_log_level()) return;

    va_start(arg, fmt);
    vsnprintf(buf, sizeof(buf), fmt, arg);
    len = strlen(buf);
    if ( buf[len-1] == '\n' ) buf[len-1] = '\0';    // clip the newline
    switch ( level ) {
    case SIM_LOGGING_E:
        catd_log_s(SIM_LOGGING_E, buf, 0);
        break;
    case SIM_LOGGING_I:
        catd_log_s(SIM_LOGGING_I, buf, 0);
        break;
    case SIM_LOGGING_D:
        catd_log_s(SIM_LOGGING_D, buf, 0);
        break;
    case SIM_LOGGING_V:
        catd_log_s(SIM_LOGGING_V, buf, 0);
        break;
    }
    va_end(arg);
}

struct enumToStr {
    int         value;
    const char  *str;
};
#define MK_ENUMSTR(x)       { x, #x }
#define ENUMSTR_TABLE(x)    x ## _table
#define ASIZE(x)            sizeof(x)/sizeof(x[0])
#define LOOKUP(type,value)  lookup( #type, ENUMSTR_TABLE(type), ASIZE(ENUMSTR_TABLE(type)), value )

static const char *lookup ( const char *typename, struct enumToStr *table, size_t tablelen, int value )
{
    size_t  i;
    int res;

    for ( i = 0 ; i < tablelen ; i++ ) {
        if ( table[i].value == value ) return table[i].str;
    }
    static char error[100];
    res = snprintf( error, sizeof(error), "%s: unknown value=%d(0x%x)", typename, value, value );

    if (res < 0 || (size_t)res >= sizeof(error)) {
       log_f(0,"lookup() failed");
    }

    return error;
}

static struct enumToStr ENUMSTR_TABLE(mal_mce_error_type) [] = {
    MK_ENUMSTR(MAL_MCE_SUCCESS),
    MK_ENUMSTR(MAL_MCE_FAIL),
    MK_ENUMSTR(MAL_MCE_R_GENERAL_ERROR),
    MK_ENUMSTR(MAL_MCE_R_COMPLETED),
    MK_ENUMSTR(MAL_MCE_R_NOT_INITIALIZED),
    MK_ENUMSTR(MAL_MCE_R_REQUEST_NOT_SUPPORTED),
    MK_ENUMSTR(MAL_MCE_R_INCORRECT_PARAMETERS),
    MK_ENUMSTR(MAL_MCE_R_MODEM_TRANSACTION_ONGOING),
    MK_ENUMSTR(MAL_MCE_R_MODEM_RESET),
    MK_ENUMSTR(MAL_MCE_R_BUSY),
    MK_ENUMSTR(MAL_MCE_R_ALREADY_ACTIVE),
    MK_ENUMSTR(MAL_MCE_R_SOCK_SEND_ERROR),
    MK_ENUMSTR(MAL_MCE_R_SOCK_CONN_ERROR),
};

static struct enumToStr ENUMSTR_TABLE(mal_mce_rf_status) [] = {
    MK_ENUMSTR(MAL_MCE_RF_OFF),
    MK_ENUMSTR(MAL_MCE_RF_ON),
    MK_ENUMSTR(MAL_MCE_RF_TRANSITION),
};

static struct enumToStr ENUMSTR_TABLE(mal_mce_status) [] = {
    MK_ENUMSTR(MAL_MCE_OK),
    MK_ENUMSTR(MAL_MCE_REQ_FAIL),
    MK_ENUMSTR(MAL_MCE_NOT_ALLOWED),
    MK_ENUMSTR(MAL_MCE_ALREADY_ACTIVE),
    MK_ENUMSTR(MAL_MCE_TRANSITION_ONGOING),
    MK_ENUMSTR(MAL_MCE_RESET_REQUIRED),
};

// fake struct name, as these are #defines
static struct enumToStr ENUMSTR_TABLE(mal_mce_opcode) [] = {
    MK_ENUMSTR(MAL_MCE_RF_STATE_RESP),
    MK_ENUMSTR(MAL_MCE_RF_STATE_IND),
    MK_ENUMSTR(MAL_MCE_RF_STATE_QUERY_RESP),
};


void sim_malmce_indication_response_trace(int event_id, void *data, mal_mce_error_type error_code, void *UNUSED(client_tag))
{
    log_f(0,"mal_mce_event_cb: event=%s", LOOKUP(mal_mce_opcode,event_id) );
    log_f(0,"mal_mce_event_cb: error=%s", LOOKUP(mal_mce_error_type,error_code) );
    switch ( event_id ) {
    case MAL_MCE_RF_STATE_RESP:
        {
            mal_mce_status   *p = data;
            log_f(1,"mal_mce_event_cb: data=%s", LOOKUP(mal_mce_status,*p) );
        }
    break;
    case MAL_MCE_RF_STATE_IND:
        {
            mal_mce_rf_status   *p = data;
            log_f(1,"mal_mce_event_cb: data=%s", LOOKUP(mal_mce_rf_status,*p) );
        }
    break;
    case MAL_MCE_RF_STATE_QUERY_RESP:
        {
            mal_mce_rf_status   *p = data;
            log_f(1,"mal_mce_event_cb: data=%s", LOOKUP(mal_mce_rf_status,*p) );
        }
    break;
    }
}
