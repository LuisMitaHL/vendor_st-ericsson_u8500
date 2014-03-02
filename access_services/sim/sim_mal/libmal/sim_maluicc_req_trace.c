/*                               -*- Mode: C -*-
 * Copyright (C) 2010, ST-Ericsson
 *
 * File name       : sim_maluicc_req_trace.c
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
#include "mal_uicc.h"
#include "mal_uicc_apdu.h"
#include "sim_trace_util.h"
#include "sim_log.h"



#define LOG_NDEBUG 0
#ifndef LOCAL_COMPILE_TEST
#define LOG_TAG "RIL MAL-REQ"   // MAL-Request
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

    if ( level >  sim_get_log_level()) return;

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

static void req_message(void *p_data)
{
    const char *fn = __func__;
    if ( p_data != NULL ) {
        uicc_req_t   *p = p_data;
        log_f(1,"%s: service_type=%s", fn, sim_trace_lookup_service_type(p->service_type) );
        switch ( p->service_type ) {
        case MAL_UICC_STATUS_GET:
        {
            // This has no additional data
            break;
        }
        default:
            log_f(0,"%s: bad service type", fn );
            break;
        }
    } else {
        log_f(0,"%s: NULL data pointer error", fn );
    }
}

static void card_req_message(void *p_data)
{
    const char *fn = __func__;
    if ( p_data != NULL ) {
        uicc_card_req_t   *p = p_data;
        log_f(1,"%s: service_type=%s, card_type=%s",
                fn,
                sim_trace_lookup_service_type(p->service_type),
                sim_trace_lookup_card_type(p->card_type));
        switch ( p->service_type ) {
        case MAL_UICC_CARD_STATUS_GET:
        {
            // This has no additional data
            break;
        }
        case MAL_UICC_CARD_INFO_GET:
        {
            // This has no additional data
            break;
        }
        default:
            log_f(0,"%s: bad service type", fn );
            break;
        }
    } else {
        log_f(0,"%s: NULL data pointer error", fn );
    }
}

static void application_req_message(void *p_data)
{
    const char *fn = __func__;
    if ( p_data != NULL ) {
        mal_uicc_appln_req_t   *p = p_data;
        log_f(1,"%s: service_type=%s, activate_last_appln=%s",
                fn,
                sim_trace_lookup_service_type(p->service_type),
                p->activate_last_appln ? "Yes" : "No" );
        switch ( p->service_type ) {
        case MAL_UICC_APPL_LIST:
        {
            // This has no additional data
            break;
        }
        case MAL_UICC_APPL_HOST_ACTIVATE:
        {
            uicc_appln_host_activate_t      *f = &p->sub_block.appln_host_activate;
            log_f(1,"%s: App: type=%d, id=%d",
                    fn, f->sb_appln.application_type, f->sb_appln.appl_id );
            log_f(1,"%s: AppInfo: startup type=%d",
                    fn, f->sb_appl_info.strat_up_type );
            log_f(1,"%s: AID: str_len=%d, str=%p",
                    fn, f->uicc_sb_aid.str_len, f->uicc_sb_aid.str );
            sim_trace_print_data(f->uicc_sb_aid.str, f->uicc_sb_aid.str_len,log_f);
            break;
        }
        case MAL_UICC_APPL_SHUT_DOWN_INITIATED:
        {
            //uicc_appl_shut_down_initiated_t *f = &p->sub_block.uicc_appl_shut_down_initiated;
            log_f(1,"%s: TODO MAL_UICC_APPL_SHUT_DOWN_INITIATED", fn );
            break;
        }
        case MAL_UICC_APPL_STATUS_GET:
        {
            //uicc_appl_status_get_t          *f = &p->sub_block.appl_status_get;
            log_f(1,"%s: TODO MAL_UICC_APPL_STATUS_GET", fn );
            break;
        }
        case MAL_UICC_APPL_HOST_DEACTIVATE:
        {
            uicc_appl_host_deactivate_t     *f = &p->sub_block.uicc_appl_host_deactivate;

            log_f(1,"%s: App: type=%d, id=%d",
                    fn, f->sb_appln.application_type, f->sb_appln.appl_id );
            break;
        }
        default:
            log_f(0,"%s: bad service type", fn );
            break;
        }
    } else {
        log_f(0,"%s: NULL data pointer error", fn );
    }
}

// Formats the EF file path as a hex string, to a maximum of pathlen of 16 bytes
// A "more" indication is shown if the pathlen is very long
static void format_path ( char *buff, size_t size, uint8_t *path, uint8_t pathlen )
{
#define MAX_FORMAT_PATH_LEN 16
    uint8_t i;
    int res;

    for ( i = 0 ; i < pathlen && i < MAX_FORMAT_PATH_LEN ; i++ ) {
        int len = snprintf( buff, size, "%02X", path[i] );
        if ( len > 0 ) {
            buff += len;
            size -= len;
        } else {
            break;
        }
    }
    if ( pathlen > MAX_FORMAT_PATH_LEN ) {
        res = snprintf( buff, size, "(%d more)", pathlen - MAX_FORMAT_PATH_LEN );
        if (res < 0 || (size_t)res >= size) {
            log_f(0,"format_path: snprintf failed");
        }
    }
}
// Formats a line of hex data into a buffer, prefixed by an address offset.
static void format_data_line ( uint8_t *data, size_t datalength, char *line, size_t *offset )
{
#define MAX_DATALINE_LEN 16
    size_t j;
    for ( j = 0 ; *offset < datalength && j < MAX_DATALINE_LEN ; j++, (*offset)++ ) {
        if ( j == 0 ) sprintf( line, "%04x: ", *offset );   // 6 chars generated
        sprintf( &line[j*3+6], "%02x ", data[*offset] );    // 3 chars per displayed byte
    }
}
static void appl_cmd_req_message(void *p_data)
{
    char    buff[100];
    const char *fn = __func__;
    memset(buff, 0x0, 100);
    if ( p_data != NULL ) {
        uicc_appl_cmd_req_t   *p = p_data;
        log_f(1,"%s: service_type=%s, app_id=%d",
                fn,
                sim_trace_lookup_service_type(p->service_type), p->appl_id );
        switch ( p->service_type ) {
        case MAL_UICC_APPL_READ_TRANSPARENT:
        {
            read_transparent_t      *f = &p->sub_block_t.ru_transp;
            format_path( buff, sizeof buff, f->path, f->path_length );
            log_f(1,"%s: EF=0x%04x, SFI=0x%02x, path=%s, offset=%d, amount=%d",
                    fn,
                    f->ef, f->sfi, buff, f->offset, f->amount );
            break;
        }
        case MAL_UICC_APPL_READ_LINEAR_FIXED:
        {
            read_linearfixed_t      *f = &p->sub_block_t.sb_linearfixed;
            format_path( buff, sizeof buff, f->path, f->path_length );
            log_f(1,"%s: EF=0x%04x, SFI=0x%02x, path=%s, record=%d, offset=%d, amount=%d",
                    fn,
                    f->ef, f->sfi, buff, f->record, f->offset, f->amount );
            break;
        }
        case MAL_UICC_APPL_FILE_INFO:
        {
            uicc_appl_file_info_t   *f = &p->sub_block_t.uicc_appl_file_info;
            format_path( buff, sizeof buff, f->path, f->path_length );
            log_f(1,"%s: EF=0x%04x, SFI=0x%02x, path=%s",
                    fn,
                    f->ef, f->sfi, buff );
            break;
        }
        case MAL_UICC_APPL_UPDATE_TRANSPARENT:
        {
            update_transparent_t    *f = &p->sub_block_t.update_transp;
            size_t                  count = 0;
            format_path( buff, sizeof buff,
                    f->read_transp.path, f->read_transp.path_length );
            log_f(1,"%s: EF=0x%04x, SFI=0x%02x, path=%s, offset=%d, amount=%d",
                    fn,
                    f->read_transp.ef, f->read_transp.sfi, buff,
                    f->read_transp.offset, f->read_transp.amount );
            log_f(1,"%s: Update Length=%d",
                    fn,
                    f->data.data_length );
            while ( count < (size_t)f->data.data_length && count < 256 ) {
                format_data_line( f->data.data, (size_t)f->data.data_length, buff, &count );
                log_f(2,"%s: %s", fn, buff );
            }
            break;
        }
        case MAL_UICC_APPL_UPDATE_LINEAR_FIXED:
        {
            update_linearfixed_t    *f = &p->sub_block_t.update_linearfixed;
            size_t                  count = 0;
            format_path( buff, sizeof buff,
                    f->read_linearfixed.path, f->read_linearfixed.path_length );
            log_f(1,"%s: EF=0x%04x, SFI=0x%02x, path=%s, record=%d, offset=%d, amount=%d",
                    fn,
                    f->read_linearfixed.ef, f->read_linearfixed.sfi, buff,
                    f->read_linearfixed.record,
                    f->read_linearfixed.offset, f->read_linearfixed.amount );
            log_f(1,"%s: Update Length=%d",
                    fn,
                    f->data.data_length );
            while ( count < (size_t)f->data.data_length && count < 256 ) {
                format_data_line( f->data.data, (size_t)f->data.data_length, buff, &count );
                log_f(2,"%s: %s", fn, buff );
            }
            break;
        }
        case MAL_UICC_APPL_APDU_SEND:
        {
            appl_apdu_send_req_t    *f = &p->sub_block_t.appl_apdu_send_req;
            size_t                  count = 0;
            format_path( buff, sizeof buff,
                    f->uicc_sb_appl_path.path, f->uicc_sb_appl_path.path_length );
            log_f(1,"%s: EF=0x%04x, SFI=0x%02x, path=%s",
                    fn,
                    f->uicc_sb_appl_path.ef, f->uicc_sb_appl_path.sfi, buff );
            log_f(1,"%s: client=%d\n", fn, f->uicc_sb_client.client_id );
            log_f(1,"%s: APDU Force=%d, Length=%d", fn,
                    f->uicc_sb_apdu.cmd_force, f->uicc_sb_apdu.apdu_length );
            while ( count < (size_t)f->uicc_sb_apdu.apdu_length && count < 256 ) {
                format_data_line( f->uicc_sb_apdu.apdu, (size_t)f->uicc_sb_apdu.apdu_length, buff, &count );
                log_f(2,"%s: %s", fn, buff );
            }
            break;
        }
        default:
            log_f(0,"%s: bad service type", fn );
            break;
        }
    } else {
        log_f(0,"%s: NULL data pointer error", fn );
    }
}

static void format_uicc_sb_pin_t ( char *buff, size_t size, uicc_sb_pin_t *pin )
{
    int res = snprintf( buff, size, "ID=%d, qualifier=%d, length=%d, pin=%.*s",
                        pin->pin_id, pin->pin_qualifier, pin->length, pin->length, pin->pin_code );
    if (res < 0 || (size_t)res >= size) {
       log_f(0,"lookup() failed");
    }
}
static void format_uicc_sb_puk_t ( char *buff, size_t size, uicc_sb_puk_t *puk )
{
    int res = snprintf( buff, size, "ID=%d, length=%d, puk=%.*s",
                        puk->pin_id, puk->puk_length, puk->puk_length, puk->puk_code );
    if (res < 0 || (size_t)res >= size) {
       log_f(0,"lookup() failed");
    }
}
static void pin_req_message(void *p_data)
{
    char    buff[100];
    const char *fn = __func__;
    if ( p_data != NULL ) {
        uicc_pin_req_t   *p = p_data;
        log_f(1,"%s: service_type=%s, app_id=%d",
                fn,
                sim_trace_lookup_service_type(p->service_type), p->appl_id );
        switch ( p->service_type ) {
        case MAL_UICC_PIN_ENABLE:
        {
            uicc_pin_enable_req_t   *sb = &p->sub_block_t.uicc_pin_enable_req;
            uicc_sb_pin_t           *pin = &sb->pin_uicc_sb_pin;
            format_uicc_sb_pin_t( buff, sizeof(buff), pin );
            log_f(1,"%s: PIN=%s", fn, buff );
            break;
        }
        case MAL_UICC_PIN_DISABLE:
        {
            uicc_pin_disable_req_t  *sb = &p->sub_block_t.uicc_pin_disable_req;
            uicc_sb_pin_t           *pin = &sb->pin_uicc_sb_pin;
            format_uicc_sb_pin_t( buff, sizeof(buff), pin );
            log_f(1,"%s: PIN=%s", fn, buff );
            break;
        }
        case MAL_UICC_PIN_VERIFY:
        {
            uicc_pin_verify_req_t   *sb = &p->sub_block_t.uicc_pin_verify_req;
            uicc_sb_pin_t           *pin = &sb->pin_uicc_sb_pin;
            format_uicc_sb_pin_t( buff, sizeof(buff), pin );
            log_f(1,"%s: PIN=%s", fn, buff );
            break;
        }
        case MAL_UICC_PIN_CHANGE:
        {
            uicc_pin_change_req_t   *sb = &p->sub_block_t.uicc_pin_change_req;
            uicc_sb_pin_t           *old_pin = &sb->pin_uicc_sb_pin_old;
            uicc_sb_pin_t           *new_pin = &sb->pin_uicc_sb_pin_new;
            format_uicc_sb_pin_t( buff, sizeof(buff), old_pin );
            log_f(1,"%s: OLD_PIN=%s", fn, buff );
            format_uicc_sb_pin_t( buff, sizeof(buff), new_pin );
            log_f(1,"%s: NEW_PIN=%s", fn, buff );
            break;
        }
        case MAL_UICC_PIN_INFO:
        {
            uicc_pin_info_req_t     *sb = &p->sub_block_t.uicc_pin_info_req;
            uicc_sb_pin_ref_t       *pinref = &sb->pin_uicc_sb_ref;
            log_f(1,"%s: ID=%d", fn, pinref->pin_id );
            break;
        }
        case MAL_UICC_PIN_UNBLOCK:
        {
            uicc_pin_unblock_req_t  *sb = &p->sub_block_t.uicc_pin_unblock_req;
            uicc_sb_pin_t           *pin = &sb->pin_uicc_sb_pin;
            uicc_sb_puk_t           *puk = &sb->pin_uicc_sb_puk;
            format_uicc_sb_pin_t( buff, sizeof(buff), pin );
            log_f(1,"%s: PIN=%s", fn, buff );
            format_uicc_sb_puk_t( buff, sizeof(buff), puk );
            log_f(1,"%s: PIN=%s", fn, buff );
            break;
        }
        default:
            log_f(0,"%s: bad service type", fn );
            break;
        }
    } else {
        log_f(0,"%s: NULL data pointer error", fn );
    }
}

static void cat_req_message(void *p_data)
{
    const char *fn = __func__;
    if ( p_data != NULL ) {
        uicc_cat_req_t   *p = p_data;
        log_f(1,"%s: service_type=%s", fn, sim_trace_lookup_service_type(p->service_type) );
        switch ( p->service_type ) {
        case MAL_UICC_CAT_TERMINAL_PROFILE:
        {
            log_f(1,"%s: TODO MAL_UICC_CAT_TERMINAL_PROFILE", fn );
            break;
        }
        case MAL_UICC_CAT_ENABLE:
        {
            log_f(1,"%s: TODO MAL_UICC_CAT_ENABLE", fn );
            break;
        }
        case MAL_UICC_CAT_DISABLE:
        {
            log_f(1,"%s: TODO MAL_UICC_CAT_DISABLE", fn );
            break;
        }
        case MAL_UICC_CAT_TERMINAL_RESPONSE:
        {
            log_f(1,"%s: TODO MAL_UICC_CAT_TERMINAL_RESPONSE", fn );
            break;
        }
        case MAL_UICC_CAT_ENVELOPE:
        {
            log_f(1,"%s: TODO MAL_UICC_CAT_ENVELOPE", fn );
            break;
        }
        case MAL_UICC_CAT_POLLING_SET:
        {
            log_f(1,"%s: TODO MAL_UICC_CAT_POLLING_SET", fn );
            break;
        }
        default:
            log_f(0,"%s: bad service type", fn );
            break;
        }
    } else {
        log_f(0,"%s: NULL data pointer error", fn );
    }
}

static void apdu_req_message(void *p_data)
{
    const char *fn = __func__;
    if ( p_data != NULL ) {
        uicc_apdu_req_t   *p = p_data;
        log_f(1,"%s: service_type=%s", fn, sim_trace_lookup_service_type(p->service_type) );
        switch ( p->service_type ) {
        case MAL_UICC_APDU_SEND:
        {
            char                    buff[100];
            uicc_apdu_send_req_t    *f = &p->sub_block_t.uicc_apdu_send_req;
            size_t                  count = 0;
            log_f(1,"%s: APDU Force=%d, Length=%d", fn,
                    f->uicc_sb_apdu.cmd_force, f->uicc_sb_apdu.apdu_length );
            while ( count < (size_t)f->uicc_sb_apdu.apdu_length && count < 256 ) {
                format_data_line( f->uicc_sb_apdu.apdu, (size_t)f->uicc_sb_apdu.apdu_length, buff, &count );
                log_f(2,"%s: %s", fn, buff );
            }
            break;
        }
        default:
            log_f(0,"%s: bad service type", fn );
            break;
        }
    } else {
        log_f(0,"%s: NULL data pointer error", fn );
    }
}

/**
 * @brief This function traces MAL requests in varying levels of verbosity
 */
void sim_maluicc_request_trace ( uint32_t uicc_MsgID, void* p_data, void *client_tag )
{
    const char *fn = __func__;
    log_f(0,"%s: cmd=%s, tag=%p", fn, sim_trace_lookup_request_opcode(uicc_MsgID), client_tag );
    switch ( uicc_MsgID )
    {
    case MAL_UICC_REQ:
        req_message(p_data);
        break;
    case MAL_UICC_CARD_REQ:
        card_req_message(p_data);
        break;
    case MAL_UICC_APPLICATION_REQ:
        application_req_message(p_data);
        break;
    case MAL_UICC_APPL_CMD_REQ:
        appl_cmd_req_message(p_data);
        break;
    case MAL_UICC_PIN_REQ:
        pin_req_message(p_data);
        break;
    case MAL_UICC_CAT_REQ:
        cat_req_message(p_data);
        break;
    case MAL_UICC_APDU_REQ:
        apdu_req_message(p_data);
        break;
    }
}
