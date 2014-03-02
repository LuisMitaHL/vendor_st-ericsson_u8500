/*                               -*- Mode: C -*-
 * Copyright (C) 2010, ST-Ericsson
 *
 * File name       : uicc.c
 * Description     : main functionality for UICC access library
 *
 * Author          : Annwesh Mukherjee <annwesh.xa.mukherjee@stericsson.com>
 *
 */


#include "sim.h"
#include "uicc_internal.h"
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
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <assert.h>



#define uiccd_log_s(A,B) do{fputs(A "\n",stdout);}while(0)
#define uiccd_log_f printf

/*********************************************************************************************************
 **********************Interface method definitions for uicc related methods******************************
 ********************************************************************************************************/

int ste_uicc_register(ste_sim_t * uicc, uintptr_t client_tag)
{
    int                     rv = -1;

    if (uicc == NULL) {
        uiccd_log_f ("ste_uicc_register: Incorrect input parameter, abort");
        return UICC_REQUEST_STATUS_FAILED_PARAMETER;
    }

    if (uicc->state != ste_sim_state_connected) {
        uiccd_log_f ("ste_uicc_register: Incorrect state, no client connected, abort");
        return UICC_REQUEST_STATUS_FAILED_STATE;
    }

    rv = sim_send_generic(uicc->fd,
                          STE_UICC_REQ_REGISTER,
                          0,
                          0,
                          client_tag);
    if (rv < 0) {
        uiccd_log_f ("ste_uicc_register: sim_send_generic for STE_UICC_REQ_REGISTER failed, disconnecting...");
        rv = ste_sim_disconnect(uicc,
                                client_tag);
    }
    return rv;
}



int ste_uicc_pin_verify(ste_sim_t * uicc,
                        uintptr_t client_tag,
                        const sim_pin_id_t pin_id,
                        const char *pin)
{
    int                     rv = -1;
    unsigned                pin_len = 0;
    size_t                  buf_len;
    char                    *p = NULL;
    char                    *buf = NULL;

    if (pin == NULL) {
        uiccd_log_f ("ste_uicc_pin_verify: Improper pin, abort");
        return UICC_REQUEST_STATUS_FAILED_PARAMETER;
    }

    if (uicc == NULL) {
        uiccd_log_f ("ste_uicc_pin_verify: Incorrect input parameter, abort");
        return UICC_REQUEST_STATUS_FAILED_PARAMETER;
    }

    if (uicc->state != ste_sim_state_connected) {
        uiccd_log_f ("ste_uicc_pin_verify: Incorrect state, no client connected, abort");
        return UICC_REQUEST_STATUS_FAILED_STATE;
    }

    pin_len = (strlen(pin));

    if (pin_len <= 0) {
        uiccd_log_f ("ste_uicc_pin_verify: Wrong value of pin entered, abort");
        return UICC_REQUEST_STATUS_FAILED_PARAMETER;
    }

    buf_len = sizeof(char) * pin_len;
    buf_len += sizeof(pin_id) + sizeof(pin_len);

    buf = malloc(buf_len);
    if (buf == NULL) {
        uiccd_log_f ("ste_uicc_pin_verify: memory allocation failed, abort");
        return UICC_REQUEST_STATUS_FAILED_RESOURCE;
    }
    memset(buf, 0, buf_len); // Useful if encoding is wrong

    p = buf;
    p = sim_enc(p, &pin_id, sizeof(pin_id));
    p = sim_enc(p, &pin_len, sizeof(pin_len));
    p = sim_enc(p, pin, sizeof(char) * pin_len);

    if (buf_len != (size_t)(p - buf)) {
        uiccd_log_f("ste_uicc_pin_change, assert failure");
    }

    assert( buf_len == (size_t)(p-buf) );

    rv = sim_send_generic(uicc->fd,
                          STE_UICC_REQ_PIN_VERIFY,
                          buf,
                          buf_len,
                          client_tag);
    if (rv < 0) {
        uiccd_log_f ("ste_uicc_pin_verify: sim_send_generic for STE_UICC_REQ_PIN_VERIFY failed, disconnecting...");
        rv = ste_sim_disconnect((ste_sim_t *)uicc,
                               client_tag);
    }

    free(buf);

    return rv;
}

int ste_uicc_pin_disable(ste_sim_t * uicc, uintptr_t client_tag, const char *buf)
{
    int                     rv = -1;
    unsigned                len;


    if (buf == NULL) {
        uiccd_log_f ("ste_uicc_pin_disable: Incorrect pin, abort");
        return UICC_REQUEST_STATUS_FAILED_PARAMETER;
    }

    if (uicc == NULL) {
        uiccd_log_f ("ste_uicc_pin_disable: Incorrect input parameter, abort");
        return UICC_REQUEST_STATUS_FAILED_PARAMETER;
    }

    if (uicc->state != ste_sim_state_connected) {
        uiccd_log_f ("ste_uicc_pin_disable: Incorrect state, no client connected, abort");
        return UICC_REQUEST_STATUS_FAILED_STATE;
    }

    len = (strlen(buf));

    if (len <= 0) {
        uiccd_log_f ("ste_uicc_pin_disable: Wrong vaue of pin entered, abort");
        return UICC_REQUEST_STATUS_FAILED_PARAMETER;
    }

    rv = sim_send_generic(uicc->fd,
                          STE_UICC_REQ_PIN_DISABLE,
                          buf,
                          len,
                          client_tag);
    if (rv < 0) {
        uiccd_log_f ("ste_uicc_pin_disable: sim_send_generic for STE_UICC_REQ_PIN_DISABLE failed, disconnecting...");
        rv = ste_sim_disconnect((ste_sim_t *)uicc,
                                client_tag);
    }

    return rv;
}

int ste_uicc_pin_enable(ste_sim_t * uicc, uintptr_t client_tag, const char *buf)
{
    int                     rv = -1;
    unsigned                len;


    if (buf == NULL) {
        uiccd_log_f ("ste_uicc_pin_enable: Incorrect pin, abort");
        return UICC_REQUEST_STATUS_FAILED_PARAMETER;
    }

    if (uicc == NULL) {
        uiccd_log_f ("ste_uicc_pin_enable: Incorrect input parameter, abort");
        return UICC_REQUEST_STATUS_FAILED_PARAMETER;
    }

    if (uicc->state != ste_sim_state_connected) {
        uiccd_log_f ("ste_uicc_pin_enable: Incorrect state, no client connected, abort");
        return UICC_REQUEST_STATUS_FAILED_STATE;
    }

    len = (strlen(buf));

    if (len <= 0) {
        uiccd_log_f ("ste_uicc_pin_enable: Wrong vaue of pin entered, abort");
        return UICC_REQUEST_STATUS_FAILED_PARAMETER;
    }

    rv = sim_send_generic(uicc->fd,
                          STE_UICC_REQ_PIN_ENABLE,
                          buf,
                          len,
                          client_tag);
    if (rv < 0) {
        uiccd_log_f ("ste_uicc_pin_enable: sim_send_generic for STE_UICC_REQ_PIN_ENABLE failed, disconnecting...");
        rv = ste_sim_disconnect((ste_sim_t *)uicc,
                                client_tag);
    }
    return rv;
}

int ste_uicc_pin_info(ste_sim_t * uicc, uintptr_t client_tag, sim_pin_puk_id_t pin)
{
    int                     rv = -1;
    char                    *p;
    char                    *buf = NULL;
    size_t                  buf_len;

    if (uicc == NULL) {
        uiccd_log_f ("ste_uicc_pin_info: Incorrect input parameter, abort");
        return UICC_REQUEST_STATUS_FAILED_PARAMETER;
    }
    if (!(pin==SIM_PIN_PIN1 || pin<=SIM_PIN_PUK2)) {
        uiccd_log_f ("ste_uicc_pin_info: pin_id out of range");
        return UICC_REQUEST_STATUS_FAILED_PARAMETER;
    }

    if (uicc && (uicc->state == ste_sim_state_connected)) {
        buf_len = sizeof(pin);
        buf = malloc(buf_len);
        if (buf == NULL) {
            uiccd_log_f ("ste_uicc_pin_info: memory allocation failed, abort");
            return UICC_REQUEST_STATUS_FAILED_RESOURCE;
        }

        p = buf;
        p = sim_enc(p, &pin, sizeof(pin));

        if (buf_len != (size_t)(p - buf)) {
            uiccd_log_f("ste_uicc_pin_info, assert failure");
        }

        assert( buf_len == (size_t)(p-buf) );

        rv = sim_send_generic(uicc->fd,
                              STE_UICC_REQ_PIN_INFO,
                              buf,
                              buf_len,
                              client_tag);
        if (rv < 0) {
            uiccd_log_f ("ste_uicc_pin_info: sim_send_generic for STE_UICC_REQ_PIN_INFO failed, disconnecting...");
            rv = ste_sim_disconnect((ste_sim_t *)uicc,
                                   client_tag);
        }
        free(buf);
    }
    return rv;
}

int ste_uicc_pin_change(ste_sim_t * uicc,
                        uintptr_t client_tag,
                        const char *old_pin,
                        const char *new_pin,
                        const sim_pin_id_t pin_id)
{
    int                     rv = -1;
    char                    *p = NULL;
    char                    *buf = NULL;
    size_t                  buf_len;
    unsigned                i, old_pin_len, new_pin_len;

    if (old_pin == NULL) {
        uiccd_log_f ("ste_uicc_pin_change: Incorrect old pin, abort");
        return UICC_REQUEST_STATUS_FAILED_PARAMETER;
    }

    if (new_pin == NULL) {
        uiccd_log_f ("ste_uicc_pin_change: Incorrect new pin, abort");
        return UICC_REQUEST_STATUS_FAILED_PARAMETER;
    }

    //Check that no invalid chars are used.
    for (i = 0; i < strlen(new_pin); i++) {
        if (new_pin[i] < '0' || new_pin[i] > '9') {
            return UICC_REQUEST_STATUS_FAILED_PARAMETER;
        }
    }

    if (uicc == NULL) {
        uiccd_log_f ("ste_uicc_pin_change: Incorrect input parameter, abort");
        return UICC_REQUEST_STATUS_FAILED_PARAMETER;
    }

    if (uicc->state != ste_sim_state_connected) {
        uiccd_log_f ("ste_uicc_pin_change: Incorrect state, no client connected, abort");
        return UICC_REQUEST_STATUS_FAILED_STATE;
    }

    if (!(pin_id==SIM_PIN_ID_PIN1 || pin_id==SIM_PIN_ID_PIN2)) {
        uiccd_log_f ("ste_uicc_pin_info: pin_id out of range");
        return UICC_REQUEST_STATUS_FAILED_PARAMETER;
    }

    old_pin_len = (strlen(old_pin));

    if (old_pin_len <= 0) {
        uiccd_log_f ("ste_uicc_pin_change: Wrong vaue of old pin entered, abort");
        return UICC_REQUEST_STATUS_FAILED_PARAMETER;
    }

    new_pin_len = (strlen(new_pin));

    if (new_pin_len <= 0) {
        uiccd_log_f ("ste_uicc_pin_change: Wrong vaue of new pin entered, abort");
        return UICC_REQUEST_STATUS_FAILED_PARAMETER;
    }

    buf_len = sizeof(char) * (old_pin_len + new_pin_len); //First we calculate the total number of characters for old pin and new pin values.
    buf_len += sizeof(old_pin_len) + sizeof(new_pin_len); //Then we add the total size of the datatypes holding the old pin and the new pin values.
    buf_len += sizeof(pin_id);

    buf = malloc(buf_len);
    if (buf == NULL) {
        uiccd_log_f ("ste_uicc_pin_change: memory allocation failed, abort");
        return UICC_REQUEST_STATUS_FAILED_RESOURCE;
    }
    memset(buf, 0, buf_len); // Useful if encoding is wrong

    p = buf; // The temp variable p is meant to hold intermediate encoded values from the buffer.

    p = sim_enc(p, &old_pin_len, sizeof(old_pin_len));
    p = sim_enc(p, old_pin,      sizeof(char) * (old_pin_len));
    p = sim_enc(p, &new_pin_len, sizeof(new_pin_len));
    p = sim_enc(p, new_pin,      sizeof(char)*(new_pin_len));
    p = sim_enc(p, &pin_id,      sizeof(pin_id));

    if (buf_len != (size_t)(p - buf)) {
        uiccd_log_f("ste_uicc_pin_change, assert failure");
    }
    assert( buf_len == (size_t)(p-buf) );

    rv = sim_send_generic(uicc->fd,
                          STE_UICC_REQ_PIN_CHANGE,
                          buf,
                          buf_len,
                          client_tag);
    if (rv < 0) {
        uiccd_log_f ("ste_uicc_pin_change: sim_send_generic for STE_UICC_REQ_PIN_CHANGE failed, disconnecting...");
        rv = ste_sim_disconnect((ste_sim_t *)uicc,
                                client_tag);
    }

    free( buf );
    return rv;
}


int ste_uicc_pin_unblock(ste_sim_t * uicc,
                         uintptr_t client_tag,
                         const sim_pin_id_t pin_id,
                         const char *pin,
                         const char *puk)
{
    int                     rv = -1;
    char                    *p = NULL;
    char                    *buf = NULL;
    size_t                  buf_len;
    unsigned                i, pin_len, puk_len;

    if (pin == NULL) {
        uiccd_log_f ("ste_uicc_pin_unblock: Incorrect pin, abort");
        return UICC_REQUEST_STATUS_FAILED_PARAMETER;
    }

    //Check that no invalid chars are used.
    for (i = 0; i < strlen(pin); i++) {
        if (pin[i] < '0' || pin[i] > '9') {
            return UICC_REQUEST_STATUS_FAILED_PARAMETER;
        }
    }

    if (puk == NULL) {
        uiccd_log_f ("ste_uicc_pin_unblock: Incorrect puk, abort");
        return UICC_REQUEST_STATUS_FAILED_PARAMETER;
    }

    if (uicc == NULL) {
        uiccd_log_f ("ste_uicc_pin_unblock: Incorrect input parameter, abort");
        return UICC_REQUEST_STATUS_FAILED_PARAMETER;
    }

    if (uicc->state != ste_sim_state_connected) {
        uiccd_log_f ("ste_uicc_pin_unblock: Incorrect state, no client connected, abort");
        return UICC_REQUEST_STATUS_FAILED_STATE;
    }

    pin_len = (strlen(pin));

    if (pin_len <= 0) {
        uiccd_log_f ("ste_uicc_pin_unblock: Wrong vaue of pin entered, abort");
        return UICC_REQUEST_STATUS_FAILED_PARAMETER;
    }

    puk_len = (strlen(puk));

    if (puk_len <= 0) {
        uiccd_log_f ("ste_uicc_pin_unblock: Wrong vaue of puk entered, abort");
        return UICC_REQUEST_STATUS_FAILED_PARAMETER;
    }

    buf_len = sizeof(pin_id);
    buf_len += sizeof(char) * (pin_len + puk_len);
    buf_len += sizeof(pin_len) + sizeof(puk_len);

    buf = malloc(buf_len);
    if (buf == NULL) {
        uiccd_log_f ("ste_uicc_pin_unblock: memory allocation failed, abort");
        return UICC_REQUEST_STATUS_FAILED_RESOURCE;
    }
    memset(buf, 0, buf_len); // Useful if encoding is wrong

    p = buf;

    p = sim_enc(p, &pin_len, sizeof(pin_len));
    p = sim_enc(p, pin,      sizeof(char)*(pin_len));
    p = sim_enc(p, &puk_len, sizeof(puk_len));
    p = sim_enc(p, puk,      sizeof(char)*(puk_len));
    p = sim_enc(p, &pin_id, sizeof(pin_id));

    if (buf_len != (size_t)(p - buf)) {
        uiccd_log_f("ste_uicc_pin_unblock: assert failure");
    }

    assert( buf_len == (size_t)(p-buf) );

    rv = sim_send_generic(uicc->fd,
                          STE_UICC_REQ_PIN_UNBLOCK,
                          buf,
                          buf_len,
                          client_tag);
    if (rv < 0) {
        uiccd_log_f ("ste_uicc_pin_change: sim_send_generic for STE_UICC_REQ_PIN_UNBLOCK failed, disconnecting...");
        rv = ste_sim_disconnect((ste_sim_t *)uicc,
                                client_tag);
    }

    free( buf );
    return rv;
}




uicc_request_status_t ste_uicc_sim_file_get_format(ste_sim_t * uicc,
                                                   uintptr_t client_tag,
                                                   int file_id,
                                                   const char *file_path)
{
    int rv = UICC_REQUEST_STATUS_OK;
    char *p = NULL;
    char *buf = NULL;
    size_t buf_len = 0;
    size_t path_len = 0;

    if (uicc == NULL) {
        uiccd_log_f ("ste_uicc_sim_file_get_format: Incorrect input parameter, abort");
        return UICC_REQUEST_STATUS_FAILED_PARAMETER;
    }

    if (uicc->state != ste_sim_state_connected) {
        uiccd_log_f ("ste_uicc_sim_file_get_format: Incorrect state, no client connected, abort");
        return UICC_REQUEST_STATUS_FAILED_STATE;
    }

    if (file_path) {
        path_len = strlen(file_path) + 1;
    }

    buf_len = sizeof(file_id) + sizeof(path_len) + path_len * sizeof(char);
    buf = malloc(buf_len);
    if (buf == NULL) {
        uiccd_log_f ("ste_uicc_sim_file_update_record: memory allocation failed, abort");
        return UICC_REQUEST_STATUS_FAILED_RESOURCE;
    }
    memset(buf, 0, buf_len); // Useful if encoding is wrong

    p = buf;
    p = sim_enc(p, &file_id, sizeof(file_id));
    p = sim_enc(p, &path_len, sizeof(path_len));
    p = sim_enc(p, file_path, path_len * sizeof(char));

    if (buf_len != (size_t)(p - buf)) {
        uiccd_log_f("ste_uicc_sim_file_get_format, assert failure");
    }

    assert( buf_len == (size_t)(p-buf) );

    rv = sim_send_generic(uicc->fd,
                          STE_UICC_REQ_SIM_FILE_GET_FORMAT,
                          buf,
                          buf_len,
                          client_tag);
    if (rv < 0) {
        uiccd_log_f ("ste_uicc_sim_file_read_record: sim_send_generic returned error, abort");
        rv = ste_sim_disconnect(uicc, client_tag);
        if ( !rv )
            uiccd_log_f ("ste_uicc_sim_file_read_record: ste_sim_disconnect returned error, abort");
    }

    free( buf );
    return rv;
}

uicc_request_status_t ste_uicc_sim_file_read_record(ste_sim_t * uicc,
                                                    uintptr_t client_tag,
                                                    int file_id,
                                                    int record_id,
                                                    int length,
                                                    const char *file_path)
{

    int rv = UICC_REQUEST_STATUS_OK;
    char *p = NULL, *buf = NULL;
    size_t buf_len = 0, path_len = 0;

    if (uicc == NULL) {
        uiccd_log_f ("ste_uicc_sim_file_read_record: Incorrect input parameter, abort");
        return UICC_REQUEST_STATUS_FAILED_PARAMETER;
    }

    if (uicc->state != ste_sim_state_connected) {
        uiccd_log_f ("ste_uicc_sim_file_read_record: Incorrect state, no client connected, abort");
        return UICC_REQUEST_STATUS_FAILED_STATE;
    }
    if (file_path) {
        path_len = strlen(file_path) + 1; // null term included
    }

    buf_len = sizeof(file_id) + sizeof(record_id) + sizeof(length) +
              sizeof(path_len) + path_len * sizeof(char);

    buf = malloc(buf_len);
    if (!buf) {
      uiccd_log_f ("ste_uicc_sim_file_read_record: No memory");
      return UICC_REQUEST_STATUS_FAILED_RESOURCE;
    }
    memset(buf, 0, buf_len); // Useful if encoding is wrong

    p = buf;

    p = sim_enc(p, &file_id, sizeof(file_id));
    p = sim_enc(p, &record_id, sizeof(record_id));
    p = sim_enc(p, &length, sizeof(length));

    p = sim_enc(p, &path_len, sizeof(path_len));
    p = sim_enc(p, file_path, path_len * sizeof(char));

    if (buf_len != (size_t)(p - buf)) {
        uiccd_log_f("ste_uicc_sim_file_read_record, assert failure");
    }

    assert( buf_len == (size_t)(p-buf) );

    rv = sim_send_generic(uicc->fd,
                          STE_UICC_REQ_READ_SIM_FILE_RECORD,
                          buf,
                          buf_len,
                          client_tag);
    if (rv != UICC_REQUEST_STATUS_OK) {
        uiccd_log_f ("ste_uicc_sim_file_read_record: sim_send_generic for STE_UICC_REQ_READ_SIM_FILE_RECORD returned error, abort");
        rv = ste_sim_disconnect(uicc, client_tag);
        if ( !rv )
            uiccd_log_f ("ste_uicc_sim_file_read_record: ste_sim_disconnect returned error, abort");
    }

    free( buf );
    return rv;

}

uicc_request_status_t ste_uicc_sim_file_read_binary(ste_sim_t *uicc,
                                                    uintptr_t client_tag,
                                                    int file_id,
                                                    int offset,
                                                    int length,
                                                    const char *file_path)
{
    int rv = UICC_REQUEST_STATUS_OK;
    char *p = NULL, *buf = NULL;
    size_t buf_len = 0, path_len = 0;

    if (uicc == NULL) {
        uiccd_log_f ("ste_uicc_sim_file_read_binary: Incorrect input parameter, abort");
        return UICC_REQUEST_STATUS_FAILED_PARAMETER;
    }

    if (uicc->state != ste_sim_state_connected) {
        uiccd_log_f ("ste_uicc_sim_file_read_binary: Incorrect state, no client connected, abort");
        return UICC_REQUEST_STATUS_FAILED_STATE;
    }

    if (file_path) {
        path_len = strlen(file_path) + 1; // null term included
    }
    buf_len = sizeof(file_id) + sizeof(offset) + sizeof(length) +
              sizeof(path_len) + path_len * sizeof(char);

    buf = malloc(buf_len);
    if (!buf) {
      uiccd_log_f ("ste_uicc_sim_file_read_record: No memory");
      return UICC_REQUEST_STATUS_FAILED_RESOURCE;
    }
    memset(buf, 0, buf_len); // Useful if encoding is wrong

    p = buf;
    p = sim_enc(p, &file_id, sizeof(file_id));
    p = sim_enc(p, &offset, sizeof(offset));
    p = sim_enc(p, &length, sizeof(length));

    p = sim_enc(p, &path_len, sizeof(path_len));
    p = sim_enc(p, file_path, path_len * sizeof(char));

    if (buf_len != (size_t)(p - buf)) {
        uiccd_log_f("ste_uicc_sim_file_read_binary, assert failure");
    }

    assert( buf_len == (size_t)(p-buf) );

    rv = sim_send_generic(uicc->fd,
                          STE_UICC_REQ_READ_SIM_FILE_BINARY,
                          buf,
                          buf_len,
                          client_tag);
    if (rv != UICC_REQUEST_STATUS_OK) {
        uiccd_log_f ("ste_uicc_sim_file_read_binary: sim_send_generic for STE_UICC_REQ_READ_SIM_FILE_BINARY returned error, abort");
        rv = ste_sim_disconnect(uicc, client_tag);
        if ( !rv )
            uiccd_log_f ("ste_uicc_sim_file_read_binary: ste_sim_disconnect returned error, abort");
    }

    free( buf );
    return rv;

}

uicc_request_status_t ste_uicc_sim_file_update_binary(ste_sim_t *uicc,
                                                      uintptr_t client_tag,
                                                      int file_id,
                                                      int offset,
                                                      int length,
                                                      const char *file_path,
                                                      const uint8_t *data)
{
    int                     rv = UICC_REQUEST_STATUS_OK;
    size_t                  buf_len = 0;
    char                    *buf = NULL;
    char                    *p = NULL;
    size_t                  path_len = 0;

    if (uicc == NULL) {
        uiccd_log_f ("ste_uicc_sim_file_update_binary: Incorrect input parameter, abort");
        return UICC_REQUEST_STATUS_FAILED_PARAMETER;
    }

    if (uicc->state != ste_sim_state_connected) {
        uiccd_log_f ("ste_uicc_sim_file_update_binary: Incorrect state, no client connected, abort");
        return UICC_REQUEST_STATUS_FAILED_STATE;
    }

    if (data == NULL || length == 0) {
        uiccd_log_f ("ste_uicc_sim_file_update_binary: Data to update is NULL, abort");
        return UICC_REQUEST_STATUS_FAILED_PARAMETER;
    }

    if (file_path) {
        path_len = strlen(file_path) + 1; // null term included
    }
    buf_len = sizeof(file_id) + sizeof(offset) + sizeof(length) +
              length * sizeof(uint8_t) + sizeof(path_len) + path_len * sizeof(char);

    buf = malloc(buf_len);
    if (!buf) {
        uiccd_log_f ("ste_uicc_sim_file_update_binary: memory allocation failed, abort");
        return UICC_REQUEST_STATUS_FAILED_RESOURCE;
    }
    memset(buf, 0, buf_len); // Useful if encoding is wrong

    p = buf;
    p = sim_enc(p, &file_id, sizeof(file_id));
    p = sim_enc(p, &offset, sizeof(offset));
    p = sim_enc(p, &length, sizeof(length));
    p = sim_enc(p, data, length * sizeof(uint8_t));

    p = sim_enc(p, &path_len, sizeof(path_len));
    p = sim_enc(p, file_path, path_len * sizeof(char));

    if (buf_len != (size_t)(p - buf)) {
        uiccd_log_f("ste_uicc_sim_file_update_binary, assert failure");
    }

    assert( buf_len == (size_t)(p-buf) );

    rv = sim_send_generic(uicc->fd,
                          STE_UICC_REQ_UPDATE_SIM_FILE_BINARY,
                          buf,
                          buf_len,
                          client_tag);
    if (rv < 0) {
        uiccd_log_f ("ste_uicc_sim_file_update_binary: sim_send_generic for STE_UICC_REQ_UPDATE_SIM_FILE_BINARY returned error, abort");
        rv = ste_sim_disconnect(uicc, client_tag);
        if ( !rv )
            uiccd_log_f ("ste_uicc_sim_file_update_binary: ste_sim_disconnect returned error, abort");
    }

    free(buf);
    return rv;
}

#define SIM_RECORD_ID_IS_VALID(RecordId) \
        ((((RecordId) >= SIM_RECORD_ID_ABSOLUTE_FIRST) && ((RecordId) <= SIM_RECORD_ID_ABSOLUTE_LAST)) || \
         ((RecordId) == SIM_RECORD_ID_PREVIOUS) || \
         ((RecordId) == SIM_RECORD_ID_CURRENT) || \
         ((RecordId) == SIM_RECORD_ID_NEXT))

#define ICC_MAX_RECORD_LENGTH 255

uicc_request_status_t  ste_uicc_sim_file_update_record(ste_sim_t * uicc,
                                                       uintptr_t client_tag,
                                                       int file_id,
                                                       int record_id,
                                                       int length,
                                                       const char *file_path,
                                                       const uint8_t *data)
{
    int rv = UICC_REQUEST_STATUS_OK;
    size_t buf_len = 0, path_len = 0;
    char *buf = NULL, *p = NULL;

    if (uicc == NULL) {
        uiccd_log_f ("ste_uicc_sim_file_update_record: Incorrect input parameter, abort");
        return UICC_REQUEST_STATUS_FAILED_PARAMETER;
    }

    if (uicc->state != ste_sim_state_connected) {
        uiccd_log_f ("ste_uicc_sim_file_update_record: Incorrect state, no client connected, abort");
        return UICC_REQUEST_STATUS_FAILED_STATE;
    }

    if ((data == NULL) || (length == 0)) {
        uiccd_log_f ("ste_uicc_sim_file_update_record: Data to update is NULL, abort");
        return UICC_REQUEST_STATUS_FAILED_PARAMETER;
    }

    if (length > ICC_MAX_RECORD_LENGTH ||
        !SIM_RECORD_ID_IS_VALID(record_id)) {
        uiccd_log_f ("ste_uicc_sim_file_update_record: Incorrect record info, abort");
        return UICC_REQUEST_STATUS_FAILED_PARAMETER;
    }

    if (file_path) {
        path_len = strlen(file_path) + 1;
    }

    buf_len = sizeof(file_id) + sizeof(record_id) + sizeof(length)+
      length * sizeof(uint8_t)+ sizeof(path_len) + path_len * sizeof(char);

    buf = malloc(buf_len);
    if (buf == NULL) {
        uiccd_log_f ("ste_uicc_sim_file_update_record: memory allocation failed, abort");
        return UICC_REQUEST_STATUS_FAILED_RESOURCE;
    }
    memset(buf, 0, buf_len);

    p = buf;
    p = sim_enc(p, &file_id, sizeof(file_id));
    p = sim_enc(p, &record_id, sizeof(record_id));
    p = sim_enc(p, &length, sizeof(length));
    p = sim_enc(p, data, length * sizeof(uint8_t));

    p = sim_enc(p, &path_len, sizeof(path_len));
    p = sim_enc(p, file_path, path_len * sizeof(char));

    if (buf_len != (size_t)(p - buf)) {
        uiccd_log_f("ste_uicc_sim_file_update_record, assert failure");
    }

    assert( buf_len == (size_t)(p-buf) );

    rv = sim_send_generic(uicc->fd,
                          STE_UICC_REQ_UPDATE_SIM_FILE_RECORD,
                          buf,
                          buf_len,
                          client_tag);
    if (rv < 0) {
        uiccd_log_f ("ste_uicc_sim_file_update_record: sim_send_generic for STE_UICC_REQ_UPDATE_SIM_FILE_RECORD returned error, abort");
        rv = ste_sim_disconnect(uicc, client_tag);
        if ( !rv ) {
            uiccd_log_f ("ste_uicc_sim_file_update_record: ste_sim_disconnect returned error, abort");
        }
    }

    free(buf);
    return rv;
}

uicc_request_status_t ste_uicc_sim_get_file_information(ste_sim_t * uicc,
                                                        uintptr_t client_tag,
                                                        int file_id,
                                                        const char *file_path,
                                                        ste_uicc_sim_get_file_info_type_t type)
{
    int rv = UICC_REQUEST_STATUS_OK;
    char *p = NULL;
    char *buf = NULL;
    size_t buf_len = 0;
    size_t path_len = 0;

    if (!uicc) {
        uiccd_log_f ("ste_uicc_sim_get_file_information: Incorrect input parameter, abort");
        return UICC_REQUEST_STATUS_FAILED_PARAMETER;
    }
    if (uicc->state != ste_sim_state_connected) {
        uiccd_log_f ("ste_uicc_sim_get_file_information: Incorrect state, no client connected, abort");
        return UICC_REQUEST_STATUS_FAILED_STATE;
    }

    if (file_path) path_len = strlen(file_path)+1;
    buf_len = sizeof(file_id)+sizeof(path_len)+path_len*sizeof(*file_path)+sizeof(type);
    buf = malloc(buf_len);
    if (buf == NULL) {
        uiccd_log_f ("ste_uicc_sim_file_update_record: memory allocation failed, abort");
        return UICC_REQUEST_STATUS_FAILED_RESOURCE;
    }
    memset(buf, 0, buf_len); // Useful if encoding is wrong

    p = buf;
    p = sim_enc(p, &file_id, sizeof(file_id));
    p = sim_enc(p, &type, sizeof(type));
    p = sim_enc(p, &path_len, sizeof(path_len));
    p = sim_enc(p, file_path, path_len * sizeof(char));

    if (buf_len != (size_t)(p - buf)) {
        uiccd_log_f("ste_uicc_sim_get_file_information, assert failure");
    }

    assert( buf_len == (size_t)(p-buf) );

    rv = sim_send_generic(uicc->fd,
                          STE_UICC_REQ_GET_FILE_INFORMATION,
                          buf,
                          buf_len,
                          client_tag);
    if (rv < 0) {
      uiccd_log_f ("ste_uicc_sim_get_file_information: sim_send_generic for STE_UICC_REQ_GET_FILE_INFORMATION returned error, abort");
      rv = ste_sim_disconnect(uicc, client_tag);
      if ( !rv ) {
          uiccd_log_f ("ste_uicc_sim_get_file_information: ste_sim_disconnect returned error, abort");
      }
    }
    free(buf);
    return rv;
}

uicc_request_status_t         ste_uicc_sim_get_state(ste_sim_t * uicc,
                                                     uintptr_t client_tag)
{
    int                     rv = UICC_REQUEST_STATUS_OK;

    if (uicc == NULL) {
        uiccd_log_f ("ste_uicc_sim_get_state: Incorrect input parameter, abort");
        return UICC_REQUEST_STATUS_FAILED_PARAMETER;
    }

    if (uicc->state != ste_sim_state_connected) {
        uiccd_log_f ("ste_uicc_sim_get_state: Incorrect state, no client connected, abort");
        return UICC_REQUEST_STATUS_FAILED_STATE;
    }

    rv = sim_send_generic(uicc->fd,
                          STE_UICC_REQ_GET_SIM_STATE,
                          0,
                          0,
                          client_tag);
    if (rv < 0) {
         uiccd_log_f ("ste_uicc_sim_get_state: sim_send_generic for STE_UICC_REQ_GET_SIM_STATE returned error, abort");
         rv = ste_sim_disconnect(uicc, client_tag);
         if ( !rv )
             uiccd_log_f ("ste_uicc_sim_get_state: ste_sim_disconnect returned error, abort");
    }
    return rv;
}

uicc_request_status_t ste_uicc_get_app_info(ste_sim_t * uicc,
                                            uintptr_t client_tag)
{
  int rv = UICC_REQUEST_STATUS_OK;

  if (!uicc) {
    uiccd_log_f ("ste_uicc_sim_file_update_record: Incorrect input parameter, abort");
    return UICC_REQUEST_STATUS_FAILED_PARAMETER;
  }

  if (uicc->state != ste_sim_state_connected) {
    uiccd_log_f ("ste_uicc_sim_file_update_record: Incorrect state, no client connected, abort");
    return UICC_REQUEST_STATUS_FAILED_STATE;
  }
  rv = sim_send_generic(uicc->fd,
                        STE_UICC_REQ_APP_INFO,
                        NULL,
                        0,
                        client_tag);
  if (rv < 0) {
    uiccd_log_f ("ste_uicc_sim_get_file_information: sim_send_generic for STE_UICC_REQ_APP_INFO returned error, abort");
    rv = ste_sim_disconnect(uicc, client_tag);
    if ( !rv ) {
      uiccd_log_f ("ste_uicc_sim_get_file_information: ste_sim_disconnect returned error, abort");
    }
  }

  return rv;
}

uicc_request_status_t ste_uicc_sim_smsc_get_active ( ste_sim_t * uicc,
                                                     uintptr_t client_tag )
{
    int rv = UICC_REQUEST_STATUS_OK;

    if (!uicc) {
        uiccd_log_f ("ste_uicc_sim_smsc_get_active: Incorrect input parameter, abort");
        return UICC_REQUEST_STATUS_FAILED_PARAMETER;
    }

    if (uicc->state != ste_sim_state_connected) {
        uiccd_log_f ("ste_uicc_sim_smsc_get_active: Incorrect state, no client connected, abort");
        return UICC_REQUEST_STATUS_FAILED_STATE;
    }
    rv = sim_send_generic(uicc->fd,
                          STE_UICC_REQ_READ_SMSC,
                          NULL,
                          0,
                          client_tag);
    if (rv < 0) {
        uiccd_log_f ("ste_uicc_sim_smsc_get_active: sim_send_generic for STE_UICC_REQ_READ_SMSC returned error, abort");
        rv = ste_sim_disconnect(uicc,
                                client_tag);
        if ( !rv ) {
            uiccd_log_f ("ste_uicc_sim_smsc_get_active: ste_sim_disconnect returned error, abort");
        }
    }

    return rv;
}



uicc_request_status_t ste_uicc_sim_smsc_set_active ( ste_sim_t * uicc,
                                                     uintptr_t client_tag,
                                                     ste_sim_call_number_t * smsc_p )
{
    int rv = UICC_REQUEST_STATUS_OK;
    char * buf;
    size_t buf_len = 0;

    if (!uicc) {
        uiccd_log_f ("ste_uicc_sim_smsc_set_active: Incorrect input parameter, abort");
        return UICC_REQUEST_STATUS_FAILED_PARAMETER;
    }
    if (uicc->state != ste_sim_state_connected) {
        uiccd_log_f ("ste_uicc_sim_smsc_set_active: Incorrect state, no client connected, abort");
        return UICC_REQUEST_STATUS_FAILED_STATE;
    }
    if (!smsc_p) {
        uiccd_log_f ("ste_uicc_sim_smsc_set_active: Incorrect input parameter, abort");
        return UICC_REQUEST_STATUS_FAILED_PARAMETER;
    }

    buf = sim_encode_smsc_string(smsc_p, &buf_len);
    if (buf == NULL) {
        uiccd_log_f ("ste_uicc_sim_smsc_set_active: encode smsc string failed, abort");
        return UICC_REQUEST_STATUS_FAILED_RESOURCE;
    }

    rv = sim_send_generic(uicc->fd,
                          STE_UICC_REQ_UPDATE_SMSC,
                          buf,
                          buf_len,
                          client_tag);
    if (rv < 0) {
      uiccd_log_f ("ste_uicc_sim_smsc_set_active: sim_send_generic for STE_UICC_REQ_UPDATE_SMSC returned error, abort");
      rv = ste_sim_disconnect(uicc, client_tag);
      if ( !rv )
          uiccd_log_f ("ste_uicc_sim_smsc_set_active: ste_sim_disconnect returned error, abort");
    }

    free(buf);
    return rv;
}

uicc_request_status_t ste_uicc_sim_smsc_get_record_max(ste_sim_t * uicc,
                                                       uintptr_t client_tag)
{
    int rv = UICC_REQUEST_STATUS_OK;

    if (!uicc) {
        uiccd_log_f ("ste_uicc_sim_smsc_get_record_max: Incorrect input parameter, abort");
        return UICC_REQUEST_STATUS_FAILED_PARAMETER;
    }
    if (uicc->state != ste_sim_state_connected) {
        uiccd_log_f ("ste_uicc_sim_smsc_get_record_max: Incorrect state, no client connected, abort");
        return UICC_REQUEST_STATUS_FAILED_STATE;
    }

    rv = sim_send_generic(uicc->fd,
                          STE_UICC_REQ_SMSC_GET_RECORD_MAX,
                          NULL,
                          0,
                          client_tag);
    if (rv < 0) {
      uiccd_log_f ("ste_uicc_sim_smsc_get_record_max: sim_send_generic for STE_UICC_REQ_SMSC_GET_RECORD_MAX returned error, abort");
      rv = ste_sim_disconnect(uicc, client_tag);
      if ( !rv )
          uiccd_log_f ("ste_uicc_sim_smsc_get_record_max: ste_sim_disconnect returned error, abort");
    }

    return rv;
}

uicc_request_status_t ste_uicc_sim_smsc_save_to_record(ste_sim_t * uicc,
                                                       uintptr_t client_tag,
                                                       int record_id)
{
    int rv = UICC_REQUEST_STATUS_OK;
    size_t buf_len;
    char *buf, *p;

    if (!uicc) {
        uiccd_log_f ("ste_uicc_sim_smsc_save_to_record: Incorrect input parameter, abort");
        return UICC_REQUEST_STATUS_FAILED_PARAMETER;
    }
    if (uicc->state != ste_sim_state_connected) {
        uiccd_log_f ("ste_uicc_sim_smsc_save_to_record: Incorrect state, no client connected, abort");
        return UICC_REQUEST_STATUS_FAILED_STATE;
    }

    buf_len = sizeof(record_id);
    buf = malloc(buf_len);

    if (buf == NULL) {
        uiccd_log_f ("ste_uicc_sim_smsc_save_to_record failed, abort");
        return UICC_REQUEST_STATUS_FAILED_RESOURCE;
    }

    p = buf;
    p = sim_enc(p, &record_id, sizeof(record_id));

    if (buf_len != (size_t)(p - buf)) {
        uiccd_log_f("ste_uicc_sim_smsc_save_to_record, assert failure");
    }

    assert( buf_len == (size_t)(p-buf) );

    rv = sim_send_generic(uicc->fd,
                          STE_UICC_REQ_SMSC_SAVE_TO_RECORD,
                          buf,
                          buf_len,
                          client_tag);
    if (rv < 0) {
      uiccd_log_f ("ste_uicc_sim_smsc_save_to_record: sim_send_generic for STE_UICC_REQ_SMSC_SAVE_TO_RECORD returned error, abort");
      rv = ste_sim_disconnect(uicc, client_tag);
      if ( !rv )
          uiccd_log_f ("ste_uicc_sim_smsc_save_to_record: ste_sim_disconnect returned error, abort");
    }
    free(buf);

    return rv;
}

uicc_request_status_t ste_uicc_sim_smsc_restore_from_record(ste_sim_t * uicc,
                                                            uintptr_t client_tag,
                                                            int record_id)
{
    int rv = UICC_REQUEST_STATUS_OK;
    size_t buf_len;
    char *buf, *p;

    if (!uicc) {
        uiccd_log_f ("ste_uicc_sim_smsc_restore_from_record: Incorrect input parameter, abort");
        return UICC_REQUEST_STATUS_FAILED_PARAMETER;
    }
    if (uicc->state != ste_sim_state_connected) {
        uiccd_log_f ("ste_uicc_sim_smsc_restore_from_record: Incorrect state, no client connected, abort");
        return UICC_REQUEST_STATUS_FAILED_STATE;
    }

    buf_len = sizeof(record_id);
    buf = malloc(buf_len);

    if (buf == NULL) {
        uiccd_log_f ("ste_uicc_sim_smsc_restore_from_record failed, abort");
        return UICC_REQUEST_STATUS_FAILED_RESOURCE;
    }

    p = buf;
    p = sim_enc(p, &record_id, sizeof(record_id));

    if (buf_len != (size_t)(p - buf)) {
        uiccd_log_f("ste_uicc_sim_smsc_restore_from_record, assert failure");
    }

    assert( buf_len == (size_t)(p-buf) );

    rv = sim_send_generic(uicc->fd,
                          STE_UICC_REQ_SMSC_RESTORE_FROM_RECORD,
                          buf,
                          buf_len,
                          client_tag);

    if (rv < 0) {
      uiccd_log_f ("ste_uicc_sim_smsc_restore_from_record: sim_send_generic for STE_UICC_REQ_SMSC_RESTORE_FROM_RECORD returned error, abort");
      rv = ste_sim_disconnect(uicc, client_tag);
      if ( !rv )
          uiccd_log_f ("ste_uicc_sim_smsc_restore_from_record: ste_sim_disconnect returned error, abort");
    }

    free(buf);

    return rv;
}

uicc_request_status_t ste_uicc_sim_file_read_generic(ste_sim_t *uicc,
                                                     uintptr_t client_tag,
                                                     int file_id,
                                                     void * file_param,
                                                     int param_len)
{
    int    rv = UICC_REQUEST_STATUS_OK;
    char * p = NULL;
    char * buf_p = NULL;
    size_t buf_len = 0;

    if (uicc == NULL) {
        uiccd_log_f ("ste_uicc_sim_file_read_generic: Incorrect input parameter, abort");
        return UICC_REQUEST_STATUS_FAILED_PARAMETER;
    }

    if (uicc->state != ste_sim_state_connected) {
        uiccd_log_f ("ste_uicc_sim_file_read_generic: Incorrect state, no client connected, abort");
        return UICC_REQUEST_STATUS_FAILED_STATE;
    }

    buf_len = sizeof(file_id) + sizeof(param_len) + param_len;// Same as calculating the size of int and param length

    buf_p = malloc(buf_len);

    if (!buf_p) {
        uiccd_log_f ("ste_uicc_sim_file_read_generic: No memory");
        return UICC_REQUEST_STATUS_FAILED_RESOURCE;
    }
    memset(buf_p, 0, buf_len); // Useful if encoding is wrong

    p = buf_p;
    p = sim_enc(p, &file_id, sizeof(file_id));
    p = sim_enc(p, &param_len, sizeof(param_len));

    if ((param_len > 0) && (file_param != NULL)) {
        p = sim_enc(p, (char*)file_param, param_len);
    }

    if (buf_len != (size_t)(p - buf_p)) {
        uiccd_log_f("ste_uicc_sim_file_read_generic, assert failure");
    }

    assert( buf_len == (size_t)(p - buf_p) );

    rv = sim_send_generic(uicc->fd,
                          STE_UICC_REQ_SIM_FILE_READ_GENERIC,
                          buf_p,
                          buf_len,
                          client_tag);

    if (rv != UICC_REQUEST_STATUS_OK) {
        uiccd_log_f ("ste_uicc_sim_file_read_generic: sim_send_generic for STE_UICC_REQ_SIM_FILE_READ_GENERIC returned error, abort");
        rv = ste_sim_disconnect(uicc,
                                client_tag);
        if ( !rv ) {
            uiccd_log_f ("ste_uicc_sim_file_read_generic: ste_sim_disconnect returned error, abort");
        }
    }

    free( buf_p );
    return rv;
}

uicc_request_status_t ste_uicc_sim_file_read_imsi(ste_sim_t * uicc,
                                                  uintptr_t client_tag )
{
    int rv = UICC_REQUEST_STATUS_OK;

    if (!uicc) {
        uiccd_log_f ("ste_uicc_sim_file_read_imsi: Incorrect input parameter, abort");
        return UICC_REQUEST_STATUS_FAILED_PARAMETER;
    }

    if (uicc->state != ste_sim_state_connected) {
        uiccd_log_f ("ste_uicc_sim_file_read_imsi: Incorrect state, no client connected, abort");
        return UICC_REQUEST_STATUS_FAILED_STATE;
    }

    rv = ste_uicc_sim_file_read_generic(uicc,
                                        client_tag,
                                        SIM_EF_IMSI,
                                        NULL,
                                        0);

    if (rv < 0) {
        uiccd_log_f ("ste_uicc_sim_file_read_imsi: ste_uicc_sim_file_read_generic returned error, abort");
        rv = ste_sim_disconnect(uicc,
                                client_tag);
        if ( !rv ) {
            uiccd_log_f ("ste_uicc_sim_file_read_imsi: ste_sim_disconnect returned error, abort");
        }
    }

    return rv;
}

uicc_request_status_t ste_uicc_sim_file_read_plmn(ste_sim_t *uicc, uintptr_t client_tag, ste_uicc_sim_plmn_file_id_t file_id)
{
    int       rv = UICC_REQUEST_STATUS_OK;
    size_t    buf_len = 0;
    char     *buf_p = NULL;

    if (!uicc) {
        uiccd_log_f("%s: Incorrect input parameter, abort", __func__);
        return UICC_REQUEST_STATUS_FAILED_PARAMETER;
    }

    if (uicc->state != ste_sim_state_connected) {
        uiccd_log_f("%s: Incorrect state, no client connected, abort", __func__);
        return UICC_REQUEST_STATUS_FAILED_STATE;
    }

    //encode data
    buf_p = sim_encode_read_plmn(&file_id, &buf_len);
    rv = sim_send_generic(uicc->fd,
                          STE_UICC_REQ_READ_PLMN,
                          buf_p,
                          buf_len,
                          client_tag);

    if (rv < 0) {
        uiccd_log_f("%s: ste_uicc_sim_file_read_generic returned error, abort", __func__);
        rv = ste_sim_disconnect(uicc, client_tag);
        if (!rv) {
            uiccd_log_f("%s: ste_sim_disconnect returned error, abort", __func__);
        }
    }

    free(buf_p);
    return rv;
}

uicc_request_status_t ste_uicc_sim_file_update_plmn(ste_sim_t *uicc,
                                                    uintptr_t client_tag,
                                                    int index,
                                                    ste_sim_plmn_with_AcT_t *plmn_p,
                                                    ste_uicc_sim_plmn_file_id_t file_id)
{
    int                     rv = UICC_REQUEST_STATUS_OK;
    size_t                  buf_len = 0;
    char                   *buf = NULL;

    if (uicc == NULL) {
        uiccd_log_f ("%s: Incorrect input parameter, abort", __func__);
        return UICC_REQUEST_STATUS_FAILED_PARAMETER;
    }

    if (uicc->state != ste_sim_state_connected) {
        uiccd_log_f ("%s: Incorrect state, no client connected, abort", __func__);
        return UICC_REQUEST_STATUS_FAILED_STATE;
    }

    if (NULL == plmn_p && index < 0) {
        // cant remove negative index
        uiccd_log_f("%s: Incorrect input parameter, abort", __func__);
        return UICC_REQUEST_STATUS_FAILED_PARAMETER;
    }
    buf = sim_encode_plmn_element(plmn_p, index, file_id, &buf_len);
    if (NULL == buf) {
        uiccd_log_f("%s: encode plmn string failed, abort", __func__);
        return UICC_REQUEST_STATUS_FAILED_RESOURCE;
    }

    rv = sim_send_generic(uicc->fd,
                          STE_UICC_REQ_UPDATE_PLMN,
                          buf,
                          buf_len,
                          client_tag);
    if (rv < 0) {
        uiccd_log_f ("%s: sim_send_generic for STE_UICC_REQ_UPDATE_PLMN returned error, abort", __func__);
        rv = ste_sim_disconnect(uicc, client_tag);
        if ( !rv )
            uiccd_log_f ("%s: ste_sim_disconnect returned error, abort", __func__);
    }

    free(buf);
    return rv;

}

#define PIN_MAX_LEN     (16)

uicc_request_status_t ste_uicc_update_service_table ( ste_sim_t * uicc,
                                                      uintptr_t client_tag,
                                                      const char *pin,
                                                      sim_service_type_t service_type,
                                                      uint8_t enable_service)
{
    int rv = UICC_REQUEST_STATUS_OK;
    size_t buf_len = 0;
    char *buf = NULL, *p = NULL;
    unsigned pin_len;

    if (pin == NULL) {
       pin_len = 0;
    }
    else {
       pin_len = (strlen(pin));
    }

    if (pin_len > PIN_MAX_LEN) {
        uiccd_log_f ("ste_uicc_update_service_table: Incorrect input parameter - incorrect PIN length, abort");
        return UICC_REQUEST_STATUS_FAILED_PARAMETER;
    }

    if (uicc == NULL) {
        uiccd_log_f ("ste_uicc_update_service_table: Incorrect input parameter, abort");
        return UICC_REQUEST_STATUS_FAILED_PARAMETER;
    }

    if (uicc->state != ste_sim_state_connected) {
        uiccd_log_f ("ste_uicc_update_service_table: Incorrect state, no client connected, abort");
        return UICC_REQUEST_STATUS_FAILED_STATE;
    }

    buf_len = sizeof(char) * (pin_len);  //First calculate the total number of characters for the pin values.
    buf_len += sizeof(pin_len) ;         //Then add the total size of the datatype holding the  pin value.
    buf_len += sizeof(service_type)+sizeof(enable_service);

    buf = malloc(buf_len);
    if (buf == NULL) {
        uiccd_log_f ("ste_uicc_update_service_table: memory allocation failed, abort");
        return UICC_REQUEST_STATUS_FAILED_RESOURCE;
    }
    memset(buf, 0, buf_len);

    p = buf;
    p = sim_enc(p, &pin_len, sizeof(pin_len));

    if (pin_len > 0) {
      p = sim_enc(p, pin,      sizeof(char)*(pin_len));
    }

    p = sim_enc(p, &service_type, sizeof(service_type));
    p = sim_enc(p, &enable_service, sizeof(enable_service));

    if (buf_len != (size_t)(p-buf)) {
        free(buf);
        return -1;
    }

    rv = sim_send_generic(uicc->fd, STE_UICC_REQ_UPDATE_SERVICE_TABLE, buf,
                          buf_len, client_tag);
    if (rv < 0) {
        rv = ste_sim_disconnect(uicc, client_tag);
        if ( !rv )
            uiccd_log_f ("ste_uicc_update_service_table: ste_sim_disconnect returned error, abort");
        uiccd_log_f ("ste_uicc_update_service_table: sim_send_generic returned error, abort");
        rv = UICC_REQUEST_STATUS_FAILED_APPLICATION;
    }

    free(buf);
    return rv;
}


uicc_request_status_t ste_uicc_get_service_table ( ste_sim_t * uicc,
                                                   uintptr_t client_tag,
                                                   sim_service_type_t service_type)
{
    int rv = UICC_REQUEST_STATUS_OK;
    size_t buf_len = 0;
    char *buf = NULL, *p = NULL;

    if (uicc == NULL) {
        uiccd_log_f ("ste_uicc_get_service_table: Incorrect input parameter, abort");
        return UICC_REQUEST_STATUS_FAILED_PARAMETER;
    }

    if (uicc->state != ste_sim_state_connected) {
        uiccd_log_f ("ste_uicc_get_service_table: Incorrect state, no client connected, abort");
        return UICC_REQUEST_STATUS_FAILED_STATE;
    }

    buf_len = sizeof(service_type);

    buf = malloc(buf_len);
    if (buf == NULL) {
        uiccd_log_f ("ste_uicc_get_service_table: memory allocation failed, abort");
        return UICC_REQUEST_STATUS_FAILED_RESOURCE;
    }
    memset(buf, 0, buf_len);

    p = buf;
    p = sim_enc(p, &service_type, sizeof(service_type));

    if (buf_len != (size_t)(p-buf)) {
        free(buf);
        return -1;
    }

    rv = sim_send_generic(uicc->fd, STE_UICC_REQ_GET_SERVICE_TABLE, buf,
                          buf_len, client_tag);
    if (rv < 0) {
        rv = ste_sim_disconnect(uicc, client_tag);
        if ( !rv )
            uiccd_log_f ("ste_uicc_get_service_table: ste_sim_disconnect returned error, abort");
        uiccd_log_f ("ste_uicc_get_service_table: sim_send_generic returned error, abort");
        rv = UICC_REQUEST_STATUS_FAILED_APPLICATION;
    }

    free(buf);
    return rv;
}

uicc_request_status_t ste_uicc_get_service_availability ( ste_sim_t * uicc,
                                                   uintptr_t client_tag,
                                                   sim_service_type_t service_type)
{
    int rv = UICC_REQUEST_STATUS_OK;
    size_t buf_len = 0;
    char *buf = NULL, *p = NULL;

    if (uicc == NULL) {
        uiccd_log_f ("ste_uicc_get_service_availability: Incorrect input parameter, abort");
        return UICC_REQUEST_STATUS_FAILED_PARAMETER;
    }

    if (uicc->state != ste_sim_state_connected) {
        uiccd_log_f ("ste_uicc_get_service_availability: Incorrect state, no client connected, abort");
        return UICC_REQUEST_STATUS_FAILED_STATE;
    }

    buf_len = sizeof(service_type);

    buf = malloc(buf_len);
    if (buf == NULL) {
        uiccd_log_f ("ste_uicc_get_service_availability: memory allocation failed, abort");
        return UICC_REQUEST_STATUS_FAILED_RESOURCE;
    }
    memset(buf, 0, buf_len);

    p = buf;
    p = sim_enc(p, &service_type, sizeof(service_type));

    if (buf_len != (size_t)(p-buf)) {
        free(buf);
        return -1;
    }

    rv = sim_send_generic(uicc->fd, STE_UICC_REQ_GET_SERVICE_AVAILABILITY, buf,
                          buf_len, client_tag);
    if (rv < 0) {
        rv = ste_sim_disconnect(uicc, client_tag);
        if ( !rv )
            uiccd_log_f ("ste_uicc_get_service_availability: ste_sim_disconnect returned error, abort");
        uiccd_log_f ("ste_uicc_get_service_availability: sim_send_generic returned error, abort");
        rv = UICC_REQUEST_STATUS_FAILED_APPLICATION;
    }

    free(buf);
    return rv;
}

uicc_request_status_t ste_uicc_sim_icon_read(ste_sim_t *uicc,
                                             uintptr_t client_tag,
                                             int file_id,
                                             int record_or_offset,
                                             int length)
{
    int rv = UICC_REQUEST_STATUS_OK;
    char *p = NULL, *buf = NULL;
    size_t buf_len = 0;

    if (!uicc) {
        uiccd_log_f ("ste_uicc_sim_icon_read: Incorrect input parameter, abort");
        return UICC_REQUEST_STATUS_FAILED_PARAMETER;
    }

    if (uicc->state != ste_sim_state_connected) {
        uiccd_log_f ("ste_uicc_sim_icon_read: Incorrect state, no client connected, abort");
        return UICC_REQUEST_STATUS_FAILED_STATE;
    }

    buf_len = sizeof(file_id)+sizeof(record_or_offset)+sizeof(length);

    buf = malloc(buf_len);
    if (!buf) {
      uiccd_log_f ("ste_uicc_sim_icon_read: No memory");
      return UICC_REQUEST_STATUS_FAILED_RESOURCE;
    }
    memset(buf, 0, buf_len); // Useful if encoding is wrong

    p = buf;
    p = sim_enc(p, &file_id, sizeof(file_id));
    p = sim_enc(p, &record_or_offset, sizeof(record_or_offset));
    p = sim_enc(p, &length, sizeof(length));

    if (buf_len != (size_t)(p - buf)) {
        uiccd_log_f("ste_uicc_sim_icon_read: assert failure");
    }

    assert( buf_len == (size_t)(p-buf) );

    rv = sim_send_generic(uicc->fd,
                          STE_UICC_REQ_SIM_ICON_READ,
                          buf,
                          buf_len,
                          client_tag);
    if (rv != UICC_REQUEST_STATUS_OK) {
        uiccd_log_f ("ste_uicc_sim_icon_read: sim_send_generic for STE_UICC_REQ_SIM_ICON_READ returned error, abort");
        rv = ste_sim_disconnect(uicc, client_tag);
        if (!rv)
            uiccd_log_f ("ste_uicc_sim_icon_read: ste_sim_disconnect returned error, abort");
    }

    free(buf);
    return rv;

}

uicc_request_status_t ste_uicc_get_subscriber_number(ste_sim_t *uicc,
                                                     uintptr_t client_tag)
{
    int rv = UICC_REQUEST_STATUS_OK;

    printf("ste_uicc_get_subscriber_number (%d)", __LINE__);

    if (uicc == NULL) {
        uiccd_log_f ("ste_uicc_get_subscriber_number: Incorrect input parameter, abort");
        return UICC_REQUEST_STATUS_FAILED_PARAMETER;
    }

    if (uicc->state != ste_sim_state_connected) {
        uiccd_log_f ("ste_uicc_get_subscriber_number: Incorrect state, no client connected, abort");
        return UICC_REQUEST_STATUS_FAILED_STATE;
    }

    rv = sim_send_generic(uicc->fd,
                          STE_UICC_REQ_READ_SUBSCRIBER_NUMBER,
                          NULL,
                          0,
                          client_tag);

    if (rv != UICC_REQUEST_STATUS_OK) {
        uiccd_log_f ("ste_uicc_get_subscriber_number: sim_send_generic for STE_UICC_REQ_SIM_SUBSCRIBER_NUM_READ returned error, abort");
        rv = ste_sim_disconnect(uicc, client_tag);

        if (!rv)
            uiccd_log_f ("ste_uicc_get_subscriber_number: ste_sim_disconnect returned error, abort");
    }

    return rv;
}

uicc_request_status_t ste_uicc_card_status(ste_sim_t * uicc,
                                            uintptr_t client_tag)
{
  int rv = UICC_REQUEST_STATUS_OK;

  if (!uicc) {
    uiccd_log_f ("ste_uicc_card_status: Incorrect input parameter, abort");
    return UICC_REQUEST_STATUS_FAILED_PARAMETER;
  }

  if (uicc->state != ste_sim_state_connected) {
    uiccd_log_f ("ste_uicc_card_status: Incorrect state, no client connected, abort");
    return UICC_REQUEST_STATUS_FAILED_STATE;
  }
  rv = sim_send_generic(uicc->fd,
                        STE_UICC_REQ_CARD_STATUS,
                        NULL,
                        0,
                        client_tag);
  if (rv < 0) {
    uiccd_log_f ("ste_uicc_card_status: sim_send_generic for STE_UICC_REQ_CARD_STATUS returned error, abort");
    rv = ste_sim_disconnect(uicc, client_tag);
    if ( !rv ) {
      uiccd_log_f ("ste_uicc_card_status: ste_sim_disconnect returned error, abort");
    }
  }

  return rv;
}

uicc_request_status_t ste_uicc_app_status(ste_sim_t * uicc,
                                          uintptr_t client_tag,
                                          int app_index)
{
  int rv = UICC_REQUEST_STATUS_OK;
  size_t buf_len;
  char *buf;
  if (!uicc || app_index < 0 || app_index >= UICC_MAX_APPS) {
    // No further check of
    uiccd_log_f ("ste_uicc_app_status: Incorrect input parameter, abort");
    return UICC_REQUEST_STATUS_FAILED_PARAMETER;
  }

  if (uicc->state != ste_sim_state_connected) {
    uiccd_log_f ("ste_uicc_card_status: Incorrect state, no client connected, abort");
    return UICC_REQUEST_STATUS_FAILED_STATE;
  }
  buf_len = sizeof(app_index);
  buf = (char *)&app_index;
  rv = sim_send_generic(uicc->fd,
                        STE_UICC_REQ_APP_STATUS,
                        buf,
                        buf_len,
                        client_tag);
  if (rv < 0) {
    uiccd_log_f ("ste_uicc_card_status: sim_send_generic for STE_UICC_REQ_CARD_STATUS returned error, abort");
    rv = ste_sim_disconnect(uicc, client_tag);
    if ( !rv ) {
      uiccd_log_f ("ste_uicc_card_status: ste_sim_disconnect returned error, abort");
    }
  }
  return rv;
}

uicc_request_status_t ste_uicc_sim_channel_send(ste_sim_t   *uicc,
                                                uintptr_t   client_tag,
                                                uint16_t    session_id,
                                                uint32_t    apdu_len,
                                                uint8_t     *apdu)
{
    int rv = UICC_REQUEST_STATUS_OK;
    char *p = NULL, *buf = NULL;
    size_t buf_len = 0;

    if (!uicc) {
        uiccd_log_f ("ste_uicc_sim_channel_send: Incorrect input parameter, abort");
        return UICC_REQUEST_STATUS_FAILED_PARAMETER;
    }

    if (uicc->state != ste_sim_state_connected) {
        uiccd_log_f ("ste_uicc_sim_channel_send: Incorrect state, no client connected, abort");
        return UICC_REQUEST_STATUS_FAILED_STATE;
    }

    buf_len = sizeof(session_id)+sizeof(apdu_len)+apdu_len;

    buf = malloc(buf_len);
    if (!buf) {
      uiccd_log_f ("ste_uicc_sim_channel_send: No memory");
      return UICC_REQUEST_STATUS_FAILED_RESOURCE;
    }
    memset(buf, 0, buf_len); // Useful if encoding is wrong

    p = buf;
    p = sim_enc(p, &session_id, sizeof(session_id));
    p = sim_enc(p, &apdu_len, sizeof(apdu_len));
    p = sim_enc(p, apdu, apdu_len);

    if (buf_len != (size_t)(p - buf)) {
        uiccd_log_f("ste_uicc_sim_channel_send: assert failure");
    }

    assert( buf_len == (size_t)(p-buf) );

    rv = sim_send_generic(uicc->fd,
                          STE_UICC_REQ_SIM_CHANNEL_SEND,
                          buf,
                          buf_len,
                          client_tag);
    if (rv != UICC_REQUEST_STATUS_OK) {
        uiccd_log_f ("ste_uicc_sim_channel_send: sim_send_generic for STE_UICC_REQ_SIM_CHANNEL_SEND returned error, abort");
        rv = ste_sim_disconnect(uicc, client_tag);
        if (!rv)
            uiccd_log_f ("ste_uicc_sim_channel_send: ste_sim_disconnect returned error, abort");
    }

    free(buf);
    return rv;
}

uicc_request_status_t ste_uicc_sim_channel_open(ste_sim_t   *uicc,
                                                uintptr_t   client_tag,
                                                uint32_t    aid_len,
                                                uint8_t     *aid)
{
    int rv = UICC_REQUEST_STATUS_OK;
    char *p = NULL, *buf = NULL;
    size_t buf_len = 0;

    if (!uicc) {
        uiccd_log_f ("ste_uicc_sim_channel_open: Incorrect input parameter, abort");
        return UICC_REQUEST_STATUS_FAILED_PARAMETER;
    }

    if (uicc->state != ste_sim_state_connected) {
        uiccd_log_f ("ste_uicc_sim_channel_open: Incorrect state, no client connected, abort");
        return UICC_REQUEST_STATUS_FAILED_STATE;
    }

    if ( aid == NULL || aid_len == 0 ) {
        uiccd_log_f ("ste_uicc_sim_channel_open: NULL/0 parameters not allowed");
        return UICC_REQUEST_STATUS_FAILED_PARAMETER;
    }

    buf_len = sizeof(aid_len)+aid_len;

    buf = malloc(buf_len);
    if (!buf) {
      uiccd_log_f ("ste_uicc_sim_channel_open: No memory");
      return UICC_REQUEST_STATUS_FAILED_RESOURCE;
    }
    memset(buf, 0, buf_len); // Useful if encoding is wrong

    p = buf;
    p = sim_enc(p, &aid_len, sizeof(aid_len));
    p = sim_enc(p, aid, aid_len);

    if (buf_len != (size_t)(p - buf)) {
        uiccd_log_f("ste_uicc_sim_channel_open: assert failure");
    }

    assert( buf_len == (size_t)(p-buf) );

    rv = sim_send_generic(uicc->fd,
                          STE_UICC_REQ_SIM_CHANNEL_OPEN,
                          buf,
                          buf_len,
                          client_tag);
    if (rv != UICC_REQUEST_STATUS_OK) {
        uiccd_log_f ("ste_uicc_sim_channel_open: sim_send_generic for STE_UICC_REQ_SIM_CHANNEL_OPEN returned error, abort");
        rv = ste_sim_disconnect(uicc, client_tag);
        if (!rv)
            uiccd_log_f ("ste_uicc_sim_channel_open: ste_sim_disconnect returned error, abort");
    }

    free(buf);
    return rv;
}

uicc_request_status_t ste_uicc_sim_channel_close(ste_sim_t   *uicc,
                                                 uintptr_t   client_tag,
                                                 uint16_t    session_id)
{
    int rv = UICC_REQUEST_STATUS_OK;
    char *p = NULL, *buf = NULL;
    size_t buf_len = 0;

    if (!uicc) {
        uiccd_log_f ("ste_uicc_sim_channel_close: Incorrect input parameter, abort");
        return UICC_REQUEST_STATUS_FAILED_PARAMETER;
    }

    if (uicc->state != ste_sim_state_connected) {
        uiccd_log_f ("ste_uicc_sim_channel_close: Incorrect state, no client connected, abort");
        return UICC_REQUEST_STATUS_FAILED_STATE;
    }

    buf_len = sizeof(session_id);

    buf = malloc(buf_len);
    if (!buf) {
      uiccd_log_f ("ste_uicc_sim_channel_close: No memory");
      return UICC_REQUEST_STATUS_FAILED_RESOURCE;
    }
    memset(buf, 0, buf_len); // Useful if encoding is wrong

    p = buf;
    p = sim_enc(p, &session_id, sizeof(session_id));

    if (buf_len != (size_t)(p - buf)) {
        uiccd_log_f("ste_uicc_sim_channel_close: assert failure");
    }

    assert( buf_len == (size_t)(p-buf) );

    rv = sim_send_generic(uicc->fd,
                          STE_UICC_REQ_SIM_CHANNEL_CLOSE,
                          buf,
                          buf_len,
                          client_tag);
    if (rv != UICC_REQUEST_STATUS_OK) {
        uiccd_log_f ("ste_uicc_sim_channel_close: sim_send_generic for STE_UICC_REQ_SIM_CHANNEL_CLOSE returned error, abort");
        rv = ste_sim_disconnect(uicc, client_tag);
        if (!rv)
            uiccd_log_f ("ste_uicc_sim_channel_close: ste_sim_disconnect returned error, abort");
    }

    free(buf);
    return rv;
}

uicc_request_status_t ste_uicc_sim_file_read_fdn(ste_sim_t *uicc,
                                                     uintptr_t client_tag)
{
    int rv = UICC_REQUEST_STATUS_OK;

    if (!uicc) {
        uiccd_log_f ("ste_uicc_sim_file_read_fdn: Incorrect input parameter, abort");
        return UICC_REQUEST_STATUS_FAILED_PARAMETER;
    }

    if (uicc->state != ste_sim_state_connected) {
        uiccd_log_f ("ste_uicc_sim_file_read_fdn: Incorrect state, no client connected, abort");
        return UICC_REQUEST_STATUS_FAILED_STATE;
    }

    rv = sim_send_generic(uicc->fd,
                          STE_UICC_REQ_READ_FDN,
                          NULL,
                          0,
                          client_tag);

    if (rv != UICC_REQUEST_STATUS_OK) {
        uiccd_log_f ("ste_uicc_sim_file_read_fdn: sim_send_generic for STE_UICC_CAUSE_REQ_SIM_FILE_READ_FDN returned error, abort");
        rv = ste_sim_disconnect(uicc, client_tag);

        if (!rv)
            uiccd_log_f ("ste_uicc_sim_file_read_fdn: ste_sim_disconnect returned error, abort");
    }

    return rv;
}

uicc_request_status_t ste_uicc_sim_file_read_ecc(ste_sim_t *uicc,
                                                 uintptr_t client_tag)
{
  int rv = UICC_REQUEST_STATUS_OK;

  if (!uicc) {
    uiccd_log_f ("ste_uicc_sim_file_read_ecc: Incorrect input parameter, abort");
    return UICC_REQUEST_STATUS_FAILED_PARAMETER;
  }

  if (uicc->state != ste_sim_state_connected) {
    uiccd_log_f ("ste_uicc_sim_file_read_ecc: Incorrect state, no client connected, abort");
    return UICC_REQUEST_STATUS_FAILED_STATE;
  }
  rv = sim_send_generic(uicc->fd,
                        STE_UICC_REQ_READ_ECC,
                        NULL,
                        0,
                        client_tag);
  if (rv < 0) {
    uiccd_log_f ("ste_uicc_sim_file_read_ecc: sim_send_generic for STE_UICC_REQ_CARD_STATUS returned error, abort");
    rv = ste_sim_disconnect(uicc, client_tag);
    if ( !rv ) {
      uiccd_log_f ("ste_uicc_card_status: ste_sim_disconnect returned error, abort");
    }
  }

  return rv;
}

int ste_uicc_sim_reset(ste_sim_t *uicc,
                       uintptr_t client_tag)
{
  int rv = UICC_REQUEST_STATUS_OK;

  if (!uicc) {
    uiccd_log_f ("ste_uicc_sim_reset: Incorrect input parameter, abort");
    return UICC_REQUEST_STATUS_FAILED_PARAMETER;
  }

  if (uicc->state != ste_sim_state_connected) {
    uiccd_log_f ("ste_uicc_sim_reset: Incorrect state, no client connected, abort");
    return UICC_REQUEST_STATUS_FAILED_STATE;
  }

  rv = sim_send_generic(uicc->fd,
                        STE_UICC_REQ_RESET,
                        NULL,
                        0,
                        client_tag);
  if (rv < 0) {
    uiccd_log_f ("ste_uicc_sim_reset: sim_send_generic for STE_UICC_REQ_RESET returned error, abort");
    rv = ste_sim_disconnect(uicc, client_tag);
    if ( !rv ) {
      uiccd_log_f ("ste_uicc_sim_reset: ste_sim_disconnect returned error, abort");
    }
  }

  return rv;

}

uicc_request_status_t ste_uicc_sim_power_on(ste_sim_t * uicc,
                                            uintptr_t client_tag)
{
  int rv = UICC_REQUEST_STATUS_OK;

  if (!uicc) {
    // No further check of
    uiccd_log_f ("ste_uicc_app_status: Incorrect input parameter, abort");
    return UICC_REQUEST_STATUS_FAILED_PARAMETER;
  }

  if (uicc->state != ste_sim_state_connected) {
    uiccd_log_f ("ste_uicc_card_status: Incorrect state, no client connected, abort");
    return UICC_REQUEST_STATUS_FAILED_STATE;
  }

  rv = sim_send_generic(uicc->fd,
                        STE_UICC_REQ_SIM_POWER_ON,
                        NULL,
                        0,
                        client_tag);
  if (rv < 0) {
    uiccd_log_f ("ste_uicc_card_status: sim_send_generic for STE_UICC_REQ_SIM_POWER_ON returned error, abort");
    rv = ste_sim_disconnect(uicc, client_tag);
    if ( !rv ) {
      uiccd_log_f ("ste_uicc_card_status: ste_sim_disconnect returned error, abort");
    }
  }
  return rv;
}

uicc_request_status_t ste_uicc_sim_power_off(ste_sim_t * uicc,
                                             uintptr_t client_tag)
{
  int rv = UICC_REQUEST_STATUS_OK;

  if (!uicc) {
    // No further check of
    uiccd_log_f ("ste_uicc_app_status: Incorrect input parameter, abort");
    return UICC_REQUEST_STATUS_FAILED_PARAMETER;
  }

  if (uicc->state != ste_sim_state_connected) {
    uiccd_log_f ("ste_uicc_card_status: Incorrect state, no client connected, abort");
    return UICC_REQUEST_STATUS_FAILED_STATE;
  }

  rv = sim_send_generic(uicc->fd,
                        STE_UICC_REQ_SIM_POWER_OFF,
                        NULL,
                        0,
                        client_tag);
  if (rv < 0) {
    uiccd_log_f ("ste_uicc_card_status: sim_send_generic for STE_UICC_REQ_SIM_POWER_OFF returned error, abort");
    rv = ste_sim_disconnect(uicc, client_tag);
    if ( !rv ) {
      uiccd_log_f ("ste_uicc_card_status: ste_sim_disconnect returned error, abort");
    }
  }
  return rv;
}

uicc_request_status_t ste_uicc_sim_read_preferred_RAT_setting(ste_sim_t * uicc,
                                                              uintptr_t client_tag)
{
  int rv = UICC_REQUEST_STATUS_OK;

  if (!uicc) {
    uiccd_log_f ("ste_uicc_app_status: Incorrect input parameter, abort");
    return UICC_REQUEST_STATUS_FAILED_PARAMETER;
  }

  if (uicc->state != ste_sim_state_connected) {
    uiccd_log_f ("ste_uicc_card_status: Incorrect state, no client connected, abort");
    return UICC_REQUEST_STATUS_FAILED_STATE;
  }

  rv = sim_send_generic(uicc->fd,
                        STE_UICC_REQ_SIM_READ_PREFERRED_RAT_SETTING,
                        NULL,
                        0,
                        client_tag);
  if (rv < 0) {
    uiccd_log_f ("ste_uicc_card_status: sim_send_generic for STE_UICC_REQ_SIM_READ_PREFERRED_RAT_SETTING returned error, abort");
    rv = ste_sim_disconnect(uicc, client_tag);
    if ( !rv ) {
      uiccd_log_f ("ste_uicc_card_status: ste_sim_disconnect returned error, abort");
    }
  }
  return rv;
}
