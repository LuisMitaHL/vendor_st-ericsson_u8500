/*                               -*- Mode: C -*-
 * Copyright (C) 2010, ST-Ericsson
 *
 * File name       : sim_trace_util.c
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
#include "mal_uicc.h"
#include "mal_uicc_apdu.h"

sim_logging_t sim_trace_debug_level = SIM_LOGGING_E;

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
    static char error[100];

    for ( i = 0 ; i < tablelen ; i++ ) {
        if ( table[i].value == value ) return table[i].str;
    }
    res = snprintf( error, sizeof(error), "%s: unknown value=%d(0x%x)", typename, value, value );
    if (res < 0 || (size_t)res >= sizeof(error)) {
        return "snprintf failed";
    }

    return error;
}

// fake struct name, as these are #defines
static struct enumToStr ENUMSTR_TABLE(mal_req_opcode) [] = {
    MK_ENUMSTR(MAL_UICC_REQ),
    MK_ENUMSTR(MAL_UICC_CARD_REQ),
    MK_ENUMSTR(MAL_UICC_APPLICATION_REQ),
    MK_ENUMSTR(MAL_UICC_APPL_CMD_REQ),
    MK_ENUMSTR(MAL_UICC_PIN_REQ),
    MK_ENUMSTR(MAL_UICC_CAT_REQ),
    MK_ENUMSTR(MAL_UICC_REFRESH_REQ),
    MK_ENUMSTR(MAL_UICC_APDU_REQ),
};

// fake struct name, as these are #defines
static struct enumToStr ENUMSTR_TABLE(mal_req_servicetype) [] = {
    MK_ENUMSTR(MAL_UICC_STATUS_GET),
    MK_ENUMSTR(MAL_UICC_START_UP_COMPLETE),
    MK_ENUMSTR(MAL_UICC_SHUTTING_DOWN),
    MK_ENUMSTR(MAL_UICC_CARD_DISCONNECTED),
    MK_ENUMSTR(MAL_UICC_CARD_REMOVED),
    MK_ENUMSTR(MAL_UICC_CARD_NOT_PRESENT),
    MK_ENUMSTR(MAL_UICC_CARD_READY),
    MK_ENUMSTR(MAL_UICC_CARD_REJECTED),
    MK_ENUMSTR(MAL_UICC_APPL_LIST),
    MK_ENUMSTR(MAL_UICC_APPL_HOST_ACTIVATE),
    MK_ENUMSTR(MAL_UICC_APPL_START_UP_COMPLETE),
    MK_ENUMSTR(MAL_UICC_APPL_SHUT_DOWN_INITIATED),
    MK_ENUMSTR(MAL_UICC_APPL_STATUS_GET),
    MK_ENUMSTR(MAL_UICC_APPL_HOST_DEACTIVATE),
    MK_ENUMSTR(MAL_UICC_APPL_TERMINATED),
    MK_ENUMSTR(MAL_UICC_APPL_RECOVERED),
    MK_ENUMSTR(MAL_UICC_APPL_ACTIVATED),
    MK_ENUMSTR(MAL_UICC_CARD_STATUS_GET),
    MK_ENUMSTR(MAL_UICC_CARD_INFO_GET),
    MK_ENUMSTR(MAL_UICC_APPL_READ_TRANSPARENT),
    MK_ENUMSTR(MAL_UICC_APPL_UPDATE_TRANSPARENT),
    MK_ENUMSTR(MAL_UICC_APPL_READ_LINEAR_FIXED),
    MK_ENUMSTR(MAL_UICC_APPL_UPDATE_LINEAR_FIXED),
    MK_ENUMSTR(MAL_UICC_APPL_FILE_INFO),
    MK_ENUMSTR(MAL_UICC_APPL_APDU_SEND),
    MK_ENUMSTR(MAL_UICC_APPL_CLEAR_CACHE),
    MK_ENUMSTR(MAL_UICC_APPL_SESSION_START),
    MK_ENUMSTR(MAL_UICC_APPL_SESSION_END),
    MK_ENUMSTR(MAL_UICC_APPL_READ_CYCLIC),
    MK_ENUMSTR(MAL_UICC_APPL_UPDATE_CYCLIC),
    MK_ENUMSTR(MAL_UICC_PIN_VERIFY),
    MK_ENUMSTR(MAL_UICC_PIN_UNBLOCK),
    MK_ENUMSTR(MAL_UICC_PIN_DISABLE),
    MK_ENUMSTR(MAL_UICC_PIN_ENABLE),
    MK_ENUMSTR(MAL_UICC_PIN_CHANGE),
    MK_ENUMSTR(MAL_UICC_PIN_INFO),
    MK_ENUMSTR(MAL_UICC_CAT_ENABLE),
    MK_ENUMSTR(MAL_UICC_CAT_DISABLE),
    MK_ENUMSTR(MAL_UICC_CAT_TERMINAL_PROFILE),
    MK_ENUMSTR(MAL_UICC_CAT_TERMINAL_RESPONSE),
    MK_ENUMSTR(MAL_UICC_CAT_ENVELOPE),
    MK_ENUMSTR(MAL_UICC_CAT_POLLING_SET),
    MK_ENUMSTR(MAL_UICC_CAT_REFRESH),
    MK_ENUMSTR(MAL_UICC_CAT_POLL),
    MK_ENUMSTR(MAL_UICC_REFRESH_STATUS),
    MK_ENUMSTR(MAL_UICC_CAT_FETCHED_CMD),
    MK_ENUMSTR(MAL_UICC_CAT_NOT_SUPPORTED),
    MK_ENUMSTR(MAL_UICC_CAT_REG_FAILED),
    MK_ENUMSTR(MAL_UICC_CAT_REG_OK),
    MK_ENUMSTR(MAL_UICC_REFRESH_PERMISSION),
    MK_ENUMSTR(MAL_UICC_REFRESH_STARTING),
    MK_ENUMSTR(MAL_UICC_REFRESH_CANCELLED),
    MK_ENUMSTR(MAL_UICC_REFRESH_NOW),
    MK_ENUMSTR(MAL_UICC_READY),
    MK_ENUMSTR(MAL_UICC_CONNECT),
    MK_ENUMSTR(MAL_UICC_DISCONNECT),
    MK_ENUMSTR(MAL_UICC_RECONNECT),
    MK_ENUMSTR(MAL_UICC_APDU_SEND),
    MK_ENUMSTR(MAL_UICC_ATR_GET),
    MK_ENUMSTR(MAL_UICC_APDU_CONTROL),
};

// fake struct name, as these are #defines
static struct enumToStr ENUMSTR_TABLE(mal_cardtype) [] = {
    MK_ENUMSTR(MAL_UICC_CARD_TYPE_UNKNOWN),
    MK_ENUMSTR(MAL_UICC_CARD_TYPE_ICC),
    MK_ENUMSTR(MAL_UICC_CARD_TYPE_UICC),
};

// fake struct name, as these are #defines
static struct enumToStr ENUMSTR_TABLE(mal_refreshtype) [] = {
    MK_ENUMSTR(MAL_UICC_REFRESH_NAA_INIT),
    MK_ENUMSTR(MAL_UICC_REFRESH_NAA_FILE_CHANGE),
    MK_ENUMSTR(MAL_UICC_REFRESH_NAA_INIT_FILE_CHANGE),
    MK_ENUMSTR(MAL_UICC_REFRESH_NAA_INIT_FULL_FILE_CHANGE),
    MK_ENUMSTR(MAL_UICC_REFRESH_UICC_RESET),
    MK_ENUMSTR(MAL_UICC_REFRESH_NAA_APPLICATION_RESET),
    MK_ENUMSTR(MAL_UICC_REFRESH_NAA_SESSION_RESET)
};

void sim_trace_print_data(uint8_t* buff, size_t length, void (*log_f)(sim_logging_t, const char *, ...))
{
    char    line[100];
    size_t  i = 0, j = 0;

    if ( buff == NULL ) {
        log_f(2,"Buff is NULL\n");
        return;
    }
    for ( i = 0 ; i < length ; i++ ) {
        if ( j == 0 ) sprintf(line,"%04x: ", i);
        sprintf( &line[j*3+6], "%02x ", buff[i]);
        if ( ++j == 16 ) {
            log_f(2,"%s\n",line);
            j = 0;
        }
    }
    if ( j != 0 ) log_f(2,"%s\n",line);
}

const char *sim_trace_lookup_service_type ( uint8_t service_type )
{
    return LOOKUP(mal_req_servicetype,service_type);
}

const char *sim_trace_lookup_card_type ( uint8_t card_type )
{
    return LOOKUP(mal_cardtype,card_type);
}

const char *sim_trace_lookup_request_opcode ( uint32_t uicc_MsgID )
{
    return LOOKUP(mal_req_opcode,uicc_MsgID);
}

const char *sim_trace_lookup_refresh_type ( uint8_t refresh_type )
{
    return LOOKUP(mal_refreshtype,refresh_type);
}
