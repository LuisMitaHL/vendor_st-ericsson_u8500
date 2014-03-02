/*                               -*- Mode: C -*-
 * Copyright (C) 2010, ST-Ericsson
 *
 * File name       : cat.c
 * Description     : main functionality fo CAT access library
 *
 * Author          : Mats Bergstrom <mats.m.bergstrom@stericsson.com>
 *
 */


#include "sim.h"
#include "cat_internal.h"
#include "sim_internal.h"
#include "event_stream.h"
#include "func_trace.h"

#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <assert.h>


//#define STE_CAT_I_CMD_DIE       (0xdead)

//#define READER_INPUT_BUFFER_SIZE (2048)
//#define READER_PIPE_BUFFER_SIZE (64)


#define catd_log_s(A,B) do{fputs(A "\n",stdout);}while(0)
#define catd_log_f printf

    uint32_t                reg_events;     /* Events that the client is interested in, one bit represent one event */


int ste_cat_register(ste_sim_t * cat, uintptr_t client_tag, uint32_t reg_events)
{
    int                     rv = -1;
    size_t                  buf_len = 0;
    char                    *buf_p = NULL;
    char                    *p = NULL;

    if (!(cat && cat->state == ste_sim_state_connected)) {
        return -1;
    }

    buf_len = sizeof(reg_events);
    buf_p = malloc(buf_len);
    if (!buf_p) {
        return -1;
    }

    p = buf_p;
    p = sim_enc(p, &reg_events, sizeof(reg_events));

    if (buf_len != (size_t)(p - buf_p)) {
        catd_log_f("catd : ste_cat_register, assert failure");
    }

    assert( buf_len == (size_t)(p - buf_p) );

    rv = sim_send_generic(cat->fd, STE_CAT_REQ_REGISTER, buf_p, buf_len, client_tag);
    if (rv < 0) {
        rv = ste_sim_disconnect((ste_sim_t*)cat, client_tag);
    }
    free(buf_p);
    return rv;
}

int ste_cat_deregister(ste_sim_t * cat, uintptr_t client_tag)
{
    int                     rv = -1;

    if (cat && cat->state == ste_sim_state_connected) {
        rv = sim_send_generic(cat->fd, STE_CAT_REQ_DEREGISTER, NULL, 0, client_tag);
        if (rv < 0) {
            rv = ste_sim_disconnect(cat, client_tag);
        }
    }
    return rv;
}


int ste_cat_terminal_response(ste_sim_t * cat, uintptr_t client_tag, const char *buf, unsigned len)
{
    int                     rv = -1;
    if (cat && cat->state == ste_sim_state_connected) {
        rv = sim_send_generic(cat->fd, STE_CAT_REQ_TR, buf, len, client_tag);
        if (rv < 0) {
            rv = ste_sim_disconnect(cat, client_tag);
        }
    }
    return rv;
}



int ste_cat_envelope_command(ste_sim_t * cat, uintptr_t client_tag, const char *buf, unsigned len)
{
    int                     rv = -1;
    if (cat && cat->state == ste_sim_state_connected) {
        rv = sim_send_generic(cat->fd, STE_CAT_REQ_EC, buf, len, client_tag);
        if (rv < 0) {
            rv = ste_sim_disconnect((ste_sim_t *)cat, client_tag);
        }

    }
    return rv;
}


int ste_cat_raw_apdu(ste_sim_t * cat, uintptr_t client_tag, const char *buf, unsigned len)
{
    int                     rv = -1;
    if (cat && cat->state == ste_sim_state_connected) {
        rv = sim_send_generic(cat->fd, STE_CAT_REQ_RAW_APDU, buf, len, client_tag);
        if (rv < 0) {
            rv = ste_sim_disconnect((ste_sim_t *)cat, client_tag);
        }

    }
    return rv;
}

int ste_cat_call_control(ste_sim_t * cat, uintptr_t client_tag, ste_cat_call_control_t * cc_p)
{
    int                     rv = -1;

    if (cat && cat->state == ste_sim_state_connected) {
        char                    *buf_p = NULL;
        size_t                   buf_len = 0;


        if (cc_p == NULL) {
            return -1;
        }
        //calculate the data length and allocate memory for the buffer to hold the binary string
        buf_p = sim_encode_call_control_data(cc_p, &buf_len);

        if (buf_p == NULL) {
            return -1;
        }

        rv = sim_send_generic(cat->fd, STE_CAT_REQ_CALL_CONTROL, buf_p, buf_len, client_tag);
        if (rv < 0) {
            rv = ste_sim_disconnect((ste_sim_t *)cat, client_tag);
        }
        free( buf_p );
    }
    return rv;
}

int                     ste_cat_set_terminal_profile(ste_sim_t * cat,
                                                     uintptr_t client_tag,
                                                     const char *data_p,
                                                     size_t len)
{
    int                     rv = -1;

    if (data_p == NULL || len == 0) {
        return -1;
    }
    if (cat && cat->state == ste_sim_state_connected) {
        char                   * buf_p = NULL;
        size_t                   buf_len = 0;
        char                   * p = NULL;

        buf_len = sizeof(len) + len;
        buf_p = malloc(buf_len);

        if (!buf_p) {
            return -1;
        }

        p = buf_p;
        p = sim_enc(p, &len, sizeof(len));
        p = sim_enc(p, data_p, len);

        if (buf_len != (size_t)(p - buf_p)) {
           catd_log_f("catd : ste_cat_set_terminal_profile, assert failure");
        }

        assert( buf_len == (size_t)(p - buf_p) );

        rv = sim_send_generic(cat->fd, STE_CAT_REQ_SET_TERMINAL_PROFILE, buf_p, buf_len, client_tag);
        if (rv < 0) {
            rv = ste_sim_disconnect((ste_sim_t *)cat, client_tag);
        }
        free( buf_p );
    }
    return rv;
}

int                     ste_cat_get_terminal_profile(ste_sim_t * cat,
                                                     uintptr_t client_tag)
{
    int                     rv = -1;

    if (cat && cat->state == ste_sim_state_connected) {
        rv = sim_send_generic(cat->fd, STE_CAT_REQ_GET_TERMINAL_PROFILE, NULL, 0, client_tag);
        if (rv < 0) {
            rv = ste_sim_disconnect((ste_sim_t *)cat, client_tag);
        }
    }
    return rv;
}

int                     ste_cat_get_cat_status(ste_sim_t * cat,
                                               uintptr_t client_tag)
{
    int                     rv = -1;

    if (cat && cat->state == ste_sim_state_connected) {
        rv = sim_send_generic(cat->fd, STE_CAT_REQ_GET_CAT_STATUS, NULL, 0, client_tag);
        if (rv < 0) {
            rv = ste_sim_disconnect((ste_sim_t *)cat, client_tag);
        }
    }
    return rv;
}

int ste_cat_answer_call(ste_sim_t   * cat,
                        uintptr_t     client_tag,
                        int           answer)
{
    int                     rv = -1;
    size_t                  buf_len = 0;
    char                   *buf_p = NULL;
    char                   *p = NULL;

    if (!(cat && cat->state == ste_sim_state_connected)) {
        return -1;
    }

    buf_len = sizeof(answer);
    buf_p = malloc(buf_len);
    if (!buf_p) {
        return -1;
    }

    p = buf_p;
    p = sim_enc(p, &answer, sizeof(answer));

    if (buf_len != (size_t)(p - buf_p)) {
        catd_log_f("catd : ste_cat_set_terminal_profile, assert failure");
    }

    assert( buf_len == (size_t)(p - buf_p) );

    rv = sim_send_generic(cat->fd, STE_CAT_REQ_ANSWER_CALL, buf_p, buf_len, client_tag);
    if (rv < 0) {
        rv = ste_sim_disconnect((ste_sim_t*)cat, client_tag);
    }
    free(buf_p);
    return rv;
}

int ste_cat_event_download(ste_sim_t                     * cat,
                           uintptr_t                       client_tag,
                           ste_cat_event_download_type_t   event_type,
                           void                          * event_data_p)
{
    int                     rv = -1;
    size_t                  buf_len = 0;
    char                   *buf_p = NULL;

    if (!(cat && cat->state == ste_sim_state_connected)) {
        return -1;
    }

    //calculate the data length and allocate memory for the buffer to hold the binary string
    buf_p = sim_encode_event_download_data(event_type, event_data_p, &buf_len);

    if (buf_p == NULL) {
        return -1;
    }

    rv = sim_send_generic(cat->fd, STE_CAT_REQ_EVENT_DOWNLOAD, buf_p, buf_len, client_tag);
    if (rv < 0) {
        rv = ste_sim_disconnect((ste_sim_t*)cat, client_tag);
    }
    free(buf_p);
    return rv;
}

int                     ste_cat_sms_control(ste_sim_t             * cat,
                                            uintptr_t               client_tag,
                                            ste_cat_sms_control_t * sc_p)
{
    int                     rv = -1;

    if (cat && cat->state == ste_sim_state_connected) {
        char                    *buf_p = NULL;
        size_t                   buf_len = 0;

        if (sc_p == NULL) {
            return -1;
        }
        //calculate the data length and allocate memory for the buffer to hold the binary string
        buf_p = sim_encode_sms_control_data(sc_p, &buf_len);

        if (buf_p == NULL) {
            return -1;
        }

        rv = sim_send_generic(cat->fd, STE_CAT_REQ_SMS_CONTROL, buf_p, buf_len, client_tag);
        if (rv < 0) {
            rv = ste_sim_disconnect((ste_sim_t *)cat, client_tag);
        }
        free( buf_p );
    }
    return rv;
}

int                     ste_cat_enable(ste_sim_t             * cat,
                                       uintptr_t               client_tag,
                                       ste_cat_enable_type_t   enable_type)
{
    int                     rv = -1;
    size_t                  buf_len = 0;
    char                   *buf_p = NULL;
    char                   *p = NULL;

    if (!(cat && cat->state == ste_sim_state_connected)) {
        return -1;
    }

    buf_len = sizeof(enable_type);
    buf_p = malloc(buf_len);
    if (!buf_p) {
        return -1;
    }

    p = buf_p;
    p = sim_enc(p, &enable_type, sizeof(enable_type));

    if (buf_len != (size_t)(p - buf_p)) {
        catd_log_f("catd : ste_cat_enable, assert failure");
    }

    assert( buf_len == (size_t)(p - buf_p) );

    rv = sim_send_generic(cat->fd, STE_CAT_REQ_ENABLE, buf_p, buf_len, client_tag);
    if (rv < 0) {
        rv = ste_sim_disconnect((ste_sim_t*)cat, client_tag);
    }
    free(buf_p);
    return rv;
}

