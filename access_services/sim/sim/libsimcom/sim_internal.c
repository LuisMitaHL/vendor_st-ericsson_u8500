/*                               -*- Mode: C -*-
 * Copyright (C) 2010, ST-Ericsson
 *
 * File name       : sim_internal.c
 * Description     : sim utility functions.
 *
 * Author          : Jessica Nilsson <jessica.j.nilsson@stericsson.com>
 *
 */


#include "sim_internal.h"
#include "client_data.h"

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <fcntl.h>

char                   *sim_enc(char *dst, const void *vsrc, size_t n)
{
    const char             *src = (const char *) vsrc;
    while (n) {
        *dst = *src;
        ++dst;
        ++src;
        --n;
    }
    return dst;
}


const char             *sim_dec(const char *src, void *vdst, size_t n,
                                const char *smax)
{
    char                   *dst = (char *) vdst;
    while (src && dst && n) {
        if (src < smax) {
            *dst = *src;
            ++dst;
            ++src;
            --n;
        } else {
            return 0;
        }
    }
    return src;
}

void ste_sim_i_safe_copy(char *dst, const char *src, size_t n)
{
    strncpy(dst, src, n);
    if (n)
        dst[n - 1] = '\0';
}

static void *sim_send_create_buffer ( uint16_t cmd, const char *payload, size_t n, uintptr_t client_tag, size_t *buffsize )
{
    char       *used_buffer = NULL;
    char       *p = NULL;
    uint16_t    len;
    size_t      bsize;

    // check payload/n consistency
    // NULL,0 is good, as is not-null,not-zero.
    if ( (payload == NULL && n != 0) ||
         (payload != NULL && n == 0) )
        return NULL;

    if (n + sizeof(client_tag) + sizeof(cmd) > 0x0ffff)        /* too large! */
        return NULL;

    len = n + sizeof(client_tag) + sizeof(cmd);
    bsize = len + sizeof(len);

    used_buffer = malloc(bsize);
    if (!used_buffer)
        return NULL;

    p = used_buffer;
    p = sim_enc (p, &len, sizeof(len));
    p = sim_enc (p, &cmd, sizeof(cmd));
    p = sim_enc (p, &client_tag, sizeof(client_tag));
    p = sim_enc (p, payload, n);

    if (bsize != (size_t)(p-used_buffer)) {
        perror("uicc : sim_send_create_buffer, assert failure");
    }

    assert( bsize == (size_t)(p-used_buffer) );

    *buffsize = bsize;
    return used_buffer;
}

static void send_error_func ( int fd, const char *func )
{
    const char *msg = strerror(errno);
    printf("%s failed to write to fd=%d, result=%s\n", func, fd, msg );
}

int sim_send_generic(int fd, uint16_t cmd, const char *payload, size_t n, uintptr_t client_tag)
{
    char                   *used_buffer = NULL;
    ssize_t                 bytes_written;
    size_t                  bytes_to_write;
    int                     ret_val = -1;

    used_buffer = sim_send_create_buffer( cmd, payload, n, client_tag, &bytes_to_write );

    if ( !used_buffer )
        return -1;

    bytes_written = write(fd, used_buffer, bytes_to_write);
    if (bytes_written == -1) {
        ret_val = -1; // TODO: Check errno for EPIPE and handle in simd
        send_error_func( fd, __func__ );
    } else if ((size_t) bytes_written != bytes_to_write) {
        /* FIXME: Error handling! */
        ret_val = -1;
    } else {
        /* Success */
        ret_val = 0;
    }

    free(used_buffer);

    return ret_val;
}

struct buffinfo {
    char   *buff;
    size_t  size;
};

static void send_to_each_client ( const ste_sim_client_data_t *p, void *ud )
{
    ssize_t                 bytes_written;
    int                     error;
    struct buffinfo *info = ud;

    bytes_written = write(p->fd, info->buff, info->size);
    if (bytes_written == -1) {
        error = errno;
        send_error_func( p->fd, __func__ );

        /* In case fd is closed, simd will handle the client data de-registration */
        /* If de-registration is called here, it will cause dead-lock */
        if (error == EPIPE)
        {
            /* FIXME: Some error handling perhaps? */
        }
    } else if ((size_t) bytes_written != info->size) {
        /* FIXME: Error handling! */
    } else {
        /* Success */
    }
}

int sim_send_generic_all(uint16_t cmd, const char *payload, size_t n, uintptr_t client_tag)
{
    char   *used_buffer = NULL;
    size_t  bytes_to_write;

    used_buffer = sim_send_create_buffer( cmd, payload, n, client_tag, &bytes_to_write );
    if ( !used_buffer )
        return -1;

    struct buffinfo info;
    info.buff = used_buffer;
    info.size = bytes_to_write;
    ste_sim_client_data_action( send_to_each_client, &info );

    free(used_buffer);

    return 0;
}




int sim_create_pipe(int *wfs, int *rfs)
{
    int                     rv;
    int                     filedes[2];
    rv = pipe(filedes);
    if (rv == 0) {
        /* Set read end non blocking, leave write end blocking */
        rv = fcntl(filedes[0], F_SETFL, O_NONBLOCK);
        if (rv != 0) {
            close(filedes[0]);
            close(filedes[1]);
        } else {
            *rfs = filedes[0];
            *wfs = filedes[1];
            rv = 0;
        }
    }
    return rv;
}



int sim_launch_thread(pthread_t * tid, int detach_state, void *(*func) (void *), void *arg)
{
    pthread_attr_t          attr;
    int                     i;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, detach_state);

    i = pthread_create(tid, &attr, func, arg);
    if (i < 0) {
        perror("failed to create thread");
    }

    pthread_attr_destroy(&attr);


    return i;
}

//For call control
char* sim_encode_call_control_data(ste_cat_call_control_t * cc_p, size_t * len_p)
{
    char     *buf_p = NULL;
    char     *temp_p = NULL;
    size_t    length = 0;
    uint8_t   text_len;

    if (cc_p == NULL || len_p == NULL)
    {
        return NULL;
    }

    switch (cc_p->cc_type)
    {
        case STE_CAT_CC_CALL_SETUP:
        {
            //necessary NULL check
            if (cc_p->cc_data.call_setup_p == NULL
                || cc_p->cc_data.call_setup_p->address.dialled_string_p == NULL
                || cc_p->cc_data.call_setup_p->address.dialled_string_p->text_p == NULL)
            {
                return NULL;
            }
            length = 1 + 1 + sizeof(cc_p->cc_type); //Tag + length + value for cc_type
            length += 1 + 1 + sizeof(cc_p->cc_data.call_setup_p->address.ton);  //TON
            length += 1 + 1 + sizeof(cc_p->cc_data.call_setup_p->address.npi);  //NPI
            length += 1 + 1 + sizeof(cc_p->cc_data.call_setup_p->address.dialled_string_p->text_coding);  //TEXT CODING
            length += 1 + 1 + sizeof(cc_p->cc_data.call_setup_p->address.dialled_string_p->no_of_characters);  //Number of characters
            length += 1 + 1;  //dial text, TAG + Length

            text_len = sim_get_text_string_length(cc_p->cc_data.call_setup_p->address.dialled_string_p);

            length += text_len;  //value part of dial text
            length += 1 + 1 + cc_p->cc_data.call_setup_p->sub_address.len;  //sub address
            length += 1 + 1 + cc_p->cc_data.call_setup_p->ccp_1.len;  //ccp 1
            length += 1 + 1 + cc_p->cc_data.call_setup_p->ccp_2.len;  //ccp 2
            length += 1 + 1 + sizeof(cc_p->cc_data.call_setup_p->bc_repeat_ind);  //BC repeat ind

            buf_p = malloc(length + 1);
            if (buf_p == NULL)
            {
                return NULL;
            }
            memset(buf_p, 0, length + 1);
            temp_p = buf_p;

            *temp_p++ = SIM_CC_TAG_CC_TYPE;
            *temp_p++ = sizeof(cc_p->cc_type);
             temp_p = sim_enc(temp_p, &cc_p->cc_type, sizeof(cc_p->cc_type));

            *temp_p++ = SIM_CC_TAG_TON;
            *temp_p++ = sizeof(cc_p->cc_data.call_setup_p->address.ton);
             temp_p = sim_enc(temp_p, &cc_p->cc_data.call_setup_p->address.ton, sizeof(cc_p->cc_data.call_setup_p->address.ton));

            *temp_p++ = SIM_CC_TAG_NPI;
            *temp_p++ = sizeof(cc_p->cc_data.call_setup_p->address.npi);
             temp_p = sim_enc(temp_p, &cc_p->cc_data.call_setup_p->address.npi, sizeof(cc_p->cc_data.call_setup_p->address.npi));

            *temp_p++ = SIM_CC_TAG_CODING;
            *temp_p++ = sizeof(cc_p->cc_data.call_setup_p->address.dialled_string_p->text_coding);
             temp_p = sim_enc(temp_p, &cc_p->cc_data.call_setup_p->address.dialled_string_p->text_coding, sizeof(cc_p->cc_data.call_setup_p->address.dialled_string_p->text_coding));

            *temp_p++ = SIM_CC_TAG_NO_OF_CHARACTERS;
            *temp_p++ = sizeof(cc_p->cc_data.call_setup_p->address.dialled_string_p->no_of_characters);
             temp_p = sim_enc(temp_p, &cc_p->cc_data.call_setup_p->address.dialled_string_p->no_of_characters, sizeof(cc_p->cc_data.call_setup_p->address.dialled_string_p->no_of_characters));

            *temp_p++ = SIM_CC_TAG_DIAL_STR_TEXT;
            *temp_p++ = text_len;
             temp_p = sim_enc(temp_p, cc_p->cc_data.call_setup_p->address.dialled_string_p->text_p, text_len);

            *temp_p++ = SIM_CC_TAG_SUB_ADDRESS;
            *temp_p++ = cc_p->cc_data.call_setup_p->sub_address.len;
             temp_p = sim_enc(temp_p, cc_p->cc_data.call_setup_p->sub_address.sub_address_p, cc_p->cc_data.call_setup_p->sub_address.len);

            *temp_p++ = SIM_CC_TAG_CCP_1;
            *temp_p++ = cc_p->cc_data.call_setup_p->ccp_1.len;
             temp_p = sim_enc(temp_p, cc_p->cc_data.call_setup_p->ccp_1.ccp_p, cc_p->cc_data.call_setup_p->ccp_1.len);

            *temp_p++ = SIM_CC_TAG_CCP_2;
            *temp_p++ = cc_p->cc_data.call_setup_p->ccp_2.len;
             temp_p = sim_enc(temp_p, cc_p->cc_data.call_setup_p->ccp_2.ccp_p, cc_p->cc_data.call_setup_p->ccp_2.len);

            *temp_p++ = SIM_CC_TAG_BC_REPEAT_IND;
            *temp_p++ = sizeof(cc_p->cc_data.call_setup_p->bc_repeat_ind);
             temp_p = sim_enc(temp_p, &cc_p->cc_data.call_setup_p->bc_repeat_ind, sizeof(cc_p->cc_data.call_setup_p->bc_repeat_ind));
        }
        break;
        case STE_CAT_CC_SS:
        {
            //necessary NULL check
            if (cc_p->cc_data.ss_p == NULL
                || cc_p->cc_data.ss_p->address.dialled_string_p == NULL
                || cc_p->cc_data.ss_p->address.dialled_string_p->text_p == NULL)
            {
                return NULL;
            }
            length = 1 + 1 + sizeof(cc_p->cc_type); //Tag + length + value for cc_type
            length += 1 + 1 + sizeof(cc_p->cc_data.ss_p->address.ton);  //TON
            length += 1 + 1 + sizeof(cc_p->cc_data.ss_p->address.npi);  //NPI
            length += 1 + 1 + sizeof(cc_p->cc_data.ss_p->address.dialled_string_p->text_coding);  //TEXT CODING
            length += 1 + 1 + sizeof(cc_p->cc_data.ss_p->address.dialled_string_p->no_of_characters);  //Number of characters
            length += 1 + 1;  //dial text, TAG + Length

            text_len = sim_get_text_string_length(cc_p->cc_data.ss_p->address.dialled_string_p);

            length += text_len;  //value part of dial text

            buf_p = malloc(length + 1);
            if (buf_p == NULL)
            {
                return NULL;
            }
            memset(buf_p, 0, length + 1);
            temp_p = buf_p;

            *temp_p++ = SIM_CC_TAG_CC_TYPE;
            *temp_p++ = sizeof(cc_p->cc_type);
             temp_p = sim_enc(temp_p, &cc_p->cc_type, sizeof(cc_p->cc_type));

            *temp_p++ = SIM_CC_TAG_TON;
            *temp_p++ = sizeof(cc_p->cc_data.ss_p->address.ton);
             temp_p = sim_enc(temp_p, &cc_p->cc_data.ss_p->address.ton, sizeof(cc_p->cc_data.ss_p->address.ton));

            *temp_p++ = SIM_CC_TAG_NPI;
            *temp_p++ = sizeof(cc_p->cc_data.ss_p->address.npi);
             temp_p = sim_enc(temp_p, &cc_p->cc_data.ss_p->address.npi, sizeof(cc_p->cc_data.ss_p->address.npi));

            *temp_p++ = SIM_CC_TAG_CODING;
            *temp_p++ = sizeof(cc_p->cc_data.ss_p->address.dialled_string_p->text_coding);
             temp_p = sim_enc(temp_p, &cc_p->cc_data.ss_p->address.dialled_string_p->text_coding, sizeof(cc_p->cc_data.ss_p->address.dialled_string_p->text_coding));

            *temp_p++ = SIM_CC_TAG_NO_OF_CHARACTERS;
            *temp_p++ = sizeof(cc_p->cc_data.ss_p->address.dialled_string_p->no_of_characters);
             temp_p = sim_enc(temp_p, &cc_p->cc_data.ss_p->address.dialled_string_p->no_of_characters, sizeof(cc_p->cc_data.ss_p->address.dialled_string_p->no_of_characters));

            *temp_p++ = SIM_CC_TAG_DIAL_STR_TEXT;
            *temp_p++ = text_len;
             temp_p = sim_enc(temp_p, cc_p->cc_data.ss_p->address.dialled_string_p->text_p, text_len);
        }
        break;
        case STE_CAT_CC_USSD:
        {
            //necessary NULL check
            if (cc_p->cc_data.ussd_p == NULL
                || cc_p->cc_data.ussd_p->ussd_data.dialled_string_p == NULL
                || cc_p->cc_data.ussd_p->ussd_data.dialled_string_p->str_p == NULL)
            {
                return NULL;
            }
            length = 1 + 1 + sizeof(cc_p->cc_type); //Tag + length + value for cc_type
            length += 1 + 1 + sizeof(cc_p->cc_data.ussd_p->ussd_data.dcs);  //DCS
            length += 1 + 1 + sizeof(cc_p->cc_data.ussd_p->ussd_data.dialled_string_p->text_coding);  //TEXT CODING
            length += 1 + 1 + sizeof(cc_p->cc_data.ussd_p->ussd_data.dialled_string_p->no_of_bytes);  //Number of characters
            length += 1 + 1;  //dial text, TAG + Length
            text_len = cc_p->cc_data.ussd_p->ussd_data.dialled_string_p->no_of_bytes;
            length += text_len;  //value part of dial text

            buf_p = malloc(length + 1);
            if (buf_p == NULL)
            {
                return NULL;
            }
            memset(buf_p, 0, length + 1);
            temp_p = buf_p;

            *temp_p++ = SIM_CC_TAG_CC_TYPE;
            *temp_p++ = sizeof(cc_p->cc_type);
             temp_p = sim_enc(temp_p, &cc_p->cc_type, sizeof(cc_p->cc_type));

            *temp_p++ = SIM_CC_TAG_DCS;
            *temp_p++ = sizeof(cc_p->cc_data.ussd_p->ussd_data.dcs);
             temp_p = sim_enc(temp_p, &cc_p->cc_data.ussd_p->ussd_data.dcs, sizeof(cc_p->cc_data.ussd_p->ussd_data.dcs));

            *temp_p++ = SIM_CC_TAG_CODING;
            *temp_p++ = sizeof(cc_p->cc_data.ussd_p->ussd_data.dialled_string_p->text_coding);
             temp_p = sim_enc(temp_p, &cc_p->cc_data.ussd_p->ussd_data.dialled_string_p->text_coding, sizeof(cc_p->cc_data.ussd_p->ussd_data.dialled_string_p->text_coding));

            *temp_p++ = SIM_CC_TAG_NO_OF_BYTES;
            *temp_p++ = sizeof(cc_p->cc_data.ussd_p->ussd_data.dialled_string_p->no_of_bytes);
             temp_p = sim_enc(temp_p, &cc_p->cc_data.ussd_p->ussd_data.dialled_string_p->no_of_bytes, sizeof(cc_p->cc_data.ussd_p->ussd_data.dialled_string_p->no_of_bytes));

            *temp_p++ = SIM_CC_TAG_DIAL_STR_TEXT;
            *temp_p++ = text_len;
             temp_p = sim_enc(temp_p, cc_p->cc_data.ussd_p->ussd_data.dialled_string_p->str_p, text_len);
        }
        break;
        case STE_CAT_CC_PDP:
        {
            //necessary NULL check
            if (cc_p->cc_data.pdp_p == NULL
                || cc_p->cc_data.pdp_p->pdp_context.str_p == NULL)
            {
                return NULL;
            }
            length += 1 + 1 + sizeof(cc_p->cc_type); //Tag + length + value for cc_type
            length += 1 + 1 + sizeof(cc_p->cc_data.pdp_p->pdp_context.text_coding);  //TEXT CODING
            length += 1 + 1 + sizeof(cc_p->cc_data.pdp_p->pdp_context.no_of_bytes);  //Number of bytes
            length += 1 + 1;  //dial text, TAG + Length
            text_len = cc_p->cc_data.pdp_p->pdp_context.no_of_bytes;
            length += text_len;  //value part of dial text
            buf_p = malloc(length + 1);
            if (buf_p == NULL)
            {
                return NULL;
            }
            memset(buf_p, 0, length + 1);
            temp_p = buf_p;
            *temp_p++ = SIM_CC_TAG_CC_TYPE;
            *temp_p++ = sizeof(cc_p->cc_type);
             temp_p = sim_enc(temp_p, &cc_p->cc_type, sizeof(cc_p->cc_type));

            *temp_p++ = SIM_CC_TAG_CODING;
            *temp_p++ = sizeof(cc_p->cc_data.pdp_p->pdp_context.text_coding);
             temp_p = sim_enc(temp_p, &cc_p->cc_data.pdp_p->pdp_context.text_coding, sizeof(cc_p->cc_data.pdp_p->pdp_context.text_coding));

            *temp_p++ = SIM_CC_TAG_NO_OF_BYTES;
            *temp_p++ = sizeof(cc_p->cc_data.pdp_p->pdp_context.no_of_bytes);
             temp_p = sim_enc(temp_p, &cc_p->cc_data.pdp_p->pdp_context.no_of_bytes, sizeof(cc_p->cc_data.pdp_p->pdp_context.no_of_bytes));

            *temp_p++ = SIM_CC_TAG_DIAL_STR_TEXT;
            *temp_p++ = text_len;
             temp_p = sim_enc(temp_p, cc_p->cc_data.pdp_p->pdp_context.str_p, text_len);
        }
        break;
        default:
        {
            return NULL;
        }
    }

    if (length != (size_t)(temp_p-buf_p)) {
        perror("uicc : sim_encode_call_control_data, assert failure");
    }
    assert( length == (size_t)(temp_p-buf_p) );

    *len_p = length;

    return buf_p;
}

int sim_decode_call_control_data(ste_cat_call_control_t * cc_p, uint8_t * buf_p, size_t len)
{
    uint8_t          *p = buf_p;
    uint8_t          *p_max = buf_p + len;
    uint8_t           tag;
    uint8_t           length;

    if (cc_p == NULL || buf_p == NULL)
    {
        return -1;
    }

    //the first byte is CC Type
    p = (uint8_t*)sim_dec((char*)p, &tag, 1, (char*)p_max);
    if (tag != SIM_CC_TAG_CC_TYPE)
    {
        return -1;
    }
    p = (uint8_t*)sim_dec((char*)p, &length, 1, (char*)p_max);
    p = (uint8_t*)sim_dec((char*)p, &cc_p->cc_type, length, (char*)p_max);

    switch (cc_p->cc_type)
    {
        case STE_CAT_CC_CALL_SETUP:
        {
            cc_p->cc_data.call_setup_p = (ste_cat_cc_call_setup_t*)malloc(sizeof(ste_cat_cc_call_setup_t));
            if (cc_p->cc_data.call_setup_p == NULL)
            {
                return -1;
            }
            memset(cc_p->cc_data.call_setup_p, 0, sizeof(ste_cat_cc_call_setup_t));

            cc_p->cc_data.call_setup_p->address.dialled_string_p = (ste_sim_text_t*)malloc(sizeof(ste_sim_text_t));
            if (cc_p->cc_data.call_setup_p->address.dialled_string_p == NULL)
            {
                free(cc_p->cc_data.call_setup_p);
                return -1;
            }
            memset(cc_p->cc_data.call_setup_p->address.dialled_string_p, 0, sizeof(ste_sim_text_t));

            //now ready to decode TON
            p = (uint8_t*)sim_dec((char*)p, &tag, 1, (char*)p_max);
            if (tag != SIM_CC_TAG_TON)
            {
                free(cc_p->cc_data.call_setup_p->address.dialled_string_p);
                free(cc_p->cc_data.call_setup_p);
                return -1;
            }
            p = (uint8_t*)sim_dec((char*)p, &length, 1, (char*)p_max);
            p = (uint8_t*)sim_dec((char*)p, &cc_p->cc_data.call_setup_p->address.ton, length, (char*)p_max);

            //now ready to decode NPI
            p = (uint8_t*)sim_dec((char*)p, &tag, 1, (char*)p_max);
            if (tag != SIM_CC_TAG_NPI)
            {
                free(cc_p->cc_data.call_setup_p->address.dialled_string_p);
                free(cc_p->cc_data.call_setup_p);
                return -1;
            }
            p = (uint8_t*)sim_dec((char*)p, &length, 1, (char*)p_max);
            p = (uint8_t*)sim_dec((char*)p, &cc_p->cc_data.call_setup_p->address.npi, length, (char*)p_max);

            //now ready to decode Text Coding
            p = (uint8_t*)sim_dec((char*)p, &tag, 1, (char*)p_max);
            if (tag != SIM_CC_TAG_CODING)
            {
                free(cc_p->cc_data.call_setup_p->address.dialled_string_p);
                free(cc_p->cc_data.call_setup_p);
                return -1;
            }
            p = (uint8_t*)sim_dec((char*)p, &length, 1, (char*)p_max);
            p = (uint8_t*)sim_dec((char*)p, &cc_p->cc_data.call_setup_p->address.dialled_string_p->text_coding, length, (char*)p_max);

            //now ready to decode Number of characters
            p = (uint8_t*)sim_dec((char*)p, &tag, 1, (char*)p_max);
            if (tag != SIM_CC_TAG_NO_OF_CHARACTERS)
            {
                free(cc_p->cc_data.call_setup_p->address.dialled_string_p);
                free(cc_p->cc_data.call_setup_p);
                return -1;
            }
            p = (uint8_t*)sim_dec((char*)p, &length, 1, (char*)p_max);
            p = (uint8_t*)sim_dec((char*)p, &cc_p->cc_data.call_setup_p->address.dialled_string_p->no_of_characters, length, (char*)p_max);

            //now ready to decode dialled text string
            p = (uint8_t*)sim_dec((char*)p, &tag, 1, (char*)p_max);
            if (tag != SIM_CC_TAG_DIAL_STR_TEXT)
            {
                free(cc_p->cc_data.call_setup_p->address.dialled_string_p);
                free(cc_p->cc_data.call_setup_p);
                return -1;
            }
            p = (uint8_t*)sim_dec((char*)p, &length, 1, (char*)p_max);
            if (length > 0)
            {
                cc_p->cc_data.call_setup_p->address.dialled_string_p->text_p = malloc(length + 1);
                if (cc_p->cc_data.call_setup_p->address.dialled_string_p->text_p == NULL)
                {
                    free(cc_p->cc_data.call_setup_p->address.dialled_string_p);
                    free(cc_p->cc_data.call_setup_p);
                    return -1;
                }
                memset(cc_p->cc_data.call_setup_p->address.dialled_string_p->text_p, 0, length + 1);
            }
            else
            {
                free(cc_p->cc_data.call_setup_p->address.dialled_string_p);
                free(cc_p->cc_data.call_setup_p);
                return -1;
            }
            p = (uint8_t*)sim_dec((char*)p, cc_p->cc_data.call_setup_p->address.dialled_string_p->text_p, length, (char*)p_max);

            //now ready to decode sub address
            p = (uint8_t*)sim_dec((char*)p, &tag, 1, (char*)p_max);
            if (tag != SIM_CC_TAG_SUB_ADDRESS)
            {
                sim_free_call_control_data(cc_p);
                return -1;
            }
            p = (uint8_t*)sim_dec((char*)p, &length, 1, (char*)p_max);
            cc_p->cc_data.call_setup_p->sub_address.len = length;
            if (length > 0)
            {
                cc_p->cc_data.call_setup_p->sub_address.sub_address_p = malloc(length + 1);
                if (cc_p->cc_data.call_setup_p->sub_address.sub_address_p == NULL)
                {
                    sim_free_call_control_data(cc_p);
                    return -1;
                }
                memset(cc_p->cc_data.call_setup_p->sub_address.sub_address_p, 0, length + 1);
                p = (uint8_t*)sim_dec((char*)p, cc_p->cc_data.call_setup_p->sub_address.sub_address_p, length, (char*)p_max);
            }

            //now ready to decode CCP 1
            p = (uint8_t*)sim_dec((char*)p, &tag, 1, (char*)p_max);
            if (tag != SIM_CC_TAG_CCP_1)
            {
                sim_free_call_control_data(cc_p);
                return -1;
            }
            p = (uint8_t*)sim_dec((char*)p, &length, 1, (char*)p_max);
            cc_p->cc_data.call_setup_p->ccp_1.len = length;
            if (length > 0)
            {
                cc_p->cc_data.call_setup_p->ccp_1.ccp_p = malloc(length + 1);
                if (cc_p->cc_data.call_setup_p->ccp_1.ccp_p == NULL)
                {
                    sim_free_call_control_data(cc_p);
                    return -1;
                }
                memset(cc_p->cc_data.call_setup_p->ccp_1.ccp_p, 0, length + 1);
                p = (uint8_t*)sim_dec((char*)p, cc_p->cc_data.call_setup_p->ccp_1.ccp_p, length, (char*)p_max);
            }

            //now ready to decode CCP 2
            p = (uint8_t*)sim_dec((char*)p, &tag, 1, (char*)p_max);
            if (tag != SIM_CC_TAG_CCP_2)
            {
                sim_free_call_control_data(cc_p);
                return -1;
            }
            p = (uint8_t*)sim_dec((char*)p, &length, 1, (char*)p_max);
            cc_p->cc_data.call_setup_p->ccp_2.len = length;
            if (length > 0)
            {
                cc_p->cc_data.call_setup_p->ccp_2.ccp_p = malloc(length + 1);
                if (cc_p->cc_data.call_setup_p->ccp_2.ccp_p == NULL)
                {
                    sim_free_call_control_data(cc_p);
                    return -1;
                }
                memset(cc_p->cc_data.call_setup_p->ccp_2.ccp_p, 0, length + 1);
                p = (uint8_t*)sim_dec((char*)p, cc_p->cc_data.call_setup_p->ccp_2.ccp_p, length, (char*)p_max);

               if (p == NULL) {
                   perror("uicc : sim_decode_call_control_data, assert failure");
               }

               assert(p != NULL);
            }

            //now ready to decode BC repeat IND
            p = (uint8_t*)sim_dec((char*)p, &tag, 1, (char*)p_max);
            if (tag != SIM_CC_TAG_BC_REPEAT_IND)
            {
                sim_free_call_control_data(cc_p);
                return -1;
            }
            p = (uint8_t*)sim_dec((char*)p, &length, 1, (char*)p_max);
            p = (uint8_t*)sim_dec((char*)p, &cc_p->cc_data.call_setup_p->bc_repeat_ind, length, (char*)p_max);
        }
        break;
        case STE_CAT_CC_SS:
        {
            cc_p->cc_data.ss_p = (ste_cat_cc_ss_t*)malloc(sizeof(ste_cat_cc_ss_t));
            if (cc_p->cc_data.ss_p == NULL)
            {
                return -1;
            }
            memset(cc_p->cc_data.ss_p, 0, sizeof(ste_cat_cc_ss_t));

            cc_p->cc_data.ss_p->address.dialled_string_p = (ste_sim_text_t*)malloc(sizeof(ste_sim_text_t));
            if (cc_p->cc_data.ss_p->address.dialled_string_p == NULL)
            {
                free(cc_p->cc_data.ss_p);
                return -1;
            }
            memset(cc_p->cc_data.ss_p->address.dialled_string_p, 0, sizeof(ste_sim_text_t));

            //now ready to decode TON
            p = (uint8_t*)sim_dec((char*)p, &tag, 1, (char*)p_max);
            if (tag != SIM_CC_TAG_TON)
            {
                free(cc_p->cc_data.ss_p->address.dialled_string_p);
                free(cc_p->cc_data.ss_p);
                return -1;
            }
            p = (uint8_t*)sim_dec((char*)p, &length, 1, (char*)p_max);
            p = (uint8_t*)sim_dec((char*)p, &cc_p->cc_data.ss_p->address.ton, length, (char*)p_max);

            //now ready to decode NPI
            p = (uint8_t*)sim_dec((char*)p, &tag, 1, (char*)p_max);
            if (tag != SIM_CC_TAG_NPI)
            {
                free(cc_p->cc_data.ss_p->address.dialled_string_p);
                free(cc_p->cc_data.ss_p);
                return -1;
            }
            p = (uint8_t*)sim_dec((char*)p, &length, 1, (char*)p_max);
            p = (uint8_t*)sim_dec((char*)p, &cc_p->cc_data.ss_p->address.npi, length, (char*)p_max);

            //now ready to decode Text Coding
            p = (uint8_t*)sim_dec((char*)p, &tag, 1, (char*)p_max);
            if (tag != SIM_CC_TAG_CODING)
            {
                free(cc_p->cc_data.ss_p->address.dialled_string_p);
                free(cc_p->cc_data.ss_p);
                return -1;
            }
            p = (uint8_t*)sim_dec((char*)p, &length, 1, (char*)p_max);
            p = (uint8_t*)sim_dec((char*)p, &cc_p->cc_data.ss_p->address.dialled_string_p->text_coding, length, (char*)p_max);

            //now ready to decode Number of characters
            p = (uint8_t*)sim_dec((char*)p, &tag, 1, (char*)p_max);
            if (tag != SIM_CC_TAG_NO_OF_CHARACTERS)
            {
                free(cc_p->cc_data.ss_p->address.dialled_string_p);
                free(cc_p->cc_data.ss_p);
                return -1;
            }
            p = (uint8_t*)sim_dec((char*)p, &length, 1, (char*)p_max);
            p = (uint8_t*)sim_dec((char*)p, &cc_p->cc_data.ss_p->address.dialled_string_p->no_of_characters, length, (char*)p_max);

            //now ready to decode dialled text string
            p = (uint8_t*)sim_dec((char*)p, &tag, 1, (char*)p_max);
            if (tag != SIM_CC_TAG_DIAL_STR_TEXT)
            {
                free(cc_p->cc_data.ss_p->address.dialled_string_p);
                free(cc_p->cc_data.ss_p);
                return -1;
            }
            p = (uint8_t*)sim_dec((char*)p, &length, 1, (char*)p_max);
            if (length > 0)
            {
                cc_p->cc_data.ss_p->address.dialled_string_p->text_p = malloc(length + 1);
                if (cc_p->cc_data.ss_p->address.dialled_string_p->text_p == NULL)
                {
                    free(cc_p->cc_data.ss_p->address.dialled_string_p);
                    free(cc_p->cc_data.ss_p);
                    return -1;
                }
                memset(cc_p->cc_data.ss_p->address.dialled_string_p->text_p, 0, length + 1);
            }
            else
            {
                free(cc_p->cc_data.ss_p->address.dialled_string_p);
                free(cc_p->cc_data.ss_p);
                return -1;
            }
            p = (uint8_t*)sim_dec((char*)p, cc_p->cc_data.ss_p->address.dialled_string_p->text_p, length, (char*)p_max);
        }
        break;
        case STE_CAT_CC_USSD:
        {
            cc_p->cc_data.ussd_p = (ste_cat_cc_ussd_t*)malloc(sizeof(ste_cat_cc_ussd_t));
            if (cc_p->cc_data.ussd_p == NULL)
            {
                return -1;
            }
            memset(cc_p->cc_data.ussd_p, 0, sizeof(ste_cat_cc_ussd_t));

            cc_p->cc_data.ussd_p->ussd_data.dialled_string_p = (ste_sim_string_t*)malloc(sizeof(ste_sim_string_t));
            if (cc_p->cc_data.ussd_p->ussd_data.dialled_string_p == NULL)
            {
                free(cc_p->cc_data.ussd_p);
                return -1;
            }
            memset(cc_p->cc_data.ussd_p->ussd_data.dialled_string_p, 0, sizeof(ste_sim_string_t));

            //now ready to decode DCS
            p = (uint8_t*)sim_dec((char*)p, &tag, 1, (char*)p_max);
            if (tag != SIM_CC_TAG_DCS)
            {
                free(cc_p->cc_data.ussd_p->ussd_data.dialled_string_p);
                free(cc_p->cc_data.ussd_p);
                return -1;
            }
            p = (uint8_t*)sim_dec((char*)p, &length, 1, (char*)p_max);
            p = (uint8_t*)sim_dec((char*)p, &cc_p->cc_data.ussd_p->ussd_data.dcs, length, (char*)p_max);

            //now ready to decode Text Coding
            p = (uint8_t*)sim_dec((char*)p, &tag, 1, (char*)p_max);
            if (tag != SIM_CC_TAG_CODING)
            {
                free(cc_p->cc_data.ussd_p->ussd_data.dialled_string_p);
                free(cc_p->cc_data.ussd_p);
                return -1;
            }
            p = (uint8_t*)sim_dec((char*)p, &length, 1, (char*)p_max);
            p = (uint8_t*)sim_dec((char*)p, &cc_p->cc_data.ussd_p->ussd_data.dialled_string_p->text_coding, length, (char*)p_max);

            //now ready to decode Number of bytes
            p = (uint8_t*)sim_dec((char*)p, &tag, 1, (char*)p_max);
            if (tag != SIM_CC_TAG_NO_OF_BYTES)
            {
                free(cc_p->cc_data.ussd_p->ussd_data.dialled_string_p);
                free(cc_p->cc_data.ussd_p);
                return -1;
            }
            p = (uint8_t*)sim_dec((char*)p, &length, 1, (char*)p_max);
            p = (uint8_t*)sim_dec((char*)p, &cc_p->cc_data.ussd_p->ussd_data.dialled_string_p->no_of_bytes, length, (char*)p_max);

            //now ready to decode dialled text string
            p = (uint8_t*)sim_dec((char*)p, &tag, 1, (char*)p_max);
            if (tag != SIM_CC_TAG_DIAL_STR_TEXT)
            {
                free(cc_p->cc_data.ussd_p->ussd_data.dialled_string_p);
                free(cc_p->cc_data.ussd_p);
                return -1;
            }
            p = (uint8_t*)sim_dec((char*)p, &length, 1, (char*)p_max);
            if (length > 0)
            {
                cc_p->cc_data.ussd_p->ussd_data.dialled_string_p->str_p = malloc(length + 1);
                if (cc_p->cc_data.ussd_p->ussd_data.dialled_string_p->str_p == NULL)
                {
                    free(cc_p->cc_data.ussd_p->ussd_data.dialled_string_p);
                    free(cc_p->cc_data.ussd_p);
                    return -1;
                }
                memset(cc_p->cc_data.ussd_p->ussd_data.dialled_string_p->str_p, 0, length + 1);
            }
            else
            {
                free(cc_p->cc_data.ussd_p->ussd_data.dialled_string_p);
                free(cc_p->cc_data.ussd_p);
                return -1;
            }
            p = (uint8_t*)sim_dec((char*)p, cc_p->cc_data.ussd_p->ussd_data.dialled_string_p->str_p, length, (char*)p_max);
        }
        break;
        case STE_CAT_CC_PDP:
        {
            cc_p->cc_data.pdp_p = (ste_cat_cc_pdp_t*)malloc(sizeof(ste_cat_cc_pdp_t));
            if (cc_p->cc_data.pdp_p == NULL)
            {
                return -1;
            }
            memset(cc_p->cc_data.pdp_p, 0, sizeof(ste_cat_cc_pdp_t));
            //now ready to decode Text Coding
            p = (uint8_t*)sim_dec((char*)p, &tag, 1, (char*)p_max);
            if (tag != SIM_CC_TAG_CODING)
            {
                free(cc_p->cc_data.pdp_p);
                return -1;
            }
            p = (uint8_t*)sim_dec((char*)p, &length, 1, (char*)p_max);
            p = (uint8_t*)sim_dec((char*)p, &cc_p->cc_data.pdp_p->pdp_context.text_coding, length, (char*)p_max);

            //now ready to decode Number of bytes
            p = (uint8_t*)sim_dec((char*)p, &tag, 1, (char*)p_max);
            if (tag != SIM_CC_TAG_NO_OF_BYTES)
            {
                free(cc_p->cc_data.pdp_p);
                return -1;
            }
            p = (uint8_t*)sim_dec((char*)p, &length, 1, (char*)p_max);
            p = (uint8_t*)sim_dec((char*)p, &cc_p->cc_data.pdp_p->pdp_context.no_of_bytes, length, (char*)p_max);

            //now ready to decode dialed text string
            p = (uint8_t*)sim_dec((char*)p, &tag, 1, (char*)p_max);
            if (tag != SIM_CC_TAG_DIAL_STR_TEXT)
            {
                free(cc_p->cc_data.pdp_p);
                return -1;
            }
            p = (uint8_t*)sim_dec((char*)p, &length, 1, (char*)p_max);
            if (length > 0)
            {
                cc_p->cc_data.pdp_p->pdp_context.str_p = malloc(length + 1);
                if (cc_p->cc_data.pdp_p->pdp_context.str_p == NULL)
                {
                    free(cc_p->cc_data.pdp_p);
                    return -1;
                }
                memset(cc_p->cc_data.pdp_p->pdp_context.str_p, 0, length + 1);
            }
            else
            {
                free(cc_p->cc_data.pdp_p);
                return -1;
            }
            p = (uint8_t*)sim_dec((char*)p, cc_p->cc_data.pdp_p->pdp_context.str_p, length, (char*)p_max);
        }
        break;
        default:
        {
            return -1;
        }
    }

    if (p == NULL) {
       perror("sim_decode_call_control_data, assert failure");
    }

    assert(p != NULL);

    return 0;
}

int sim_copy_call_control_address(ste_cat_cc_dialled_address_t * buf_p, ste_cat_cc_dialled_address_t * data_p)
{
    int                     rv = 0;
    size_t                  text_len;

    buf_p->ton = data_p->ton;
    buf_p->npi = data_p->npi;
    if (data_p->dialled_string_p != NULL)
    {
        buf_p->dialled_string_p = malloc(sizeof(ste_sim_text_t));
        if (buf_p->dialled_string_p == NULL)
        {
            return -1;
        }
        memset(buf_p->dialled_string_p, 0, sizeof(ste_sim_text_t));

        buf_p->dialled_string_p->text_coding      = data_p->dialled_string_p->text_coding;
        buf_p->dialled_string_p->no_of_characters = data_p->dialled_string_p->no_of_characters;

        text_len = sim_get_text_string_length(data_p->dialled_string_p);

        if (data_p->dialled_string_p->no_of_characters > 0 && data_p->dialled_string_p->text_p != NULL)
        {
            buf_p->dialled_string_p->text_p = malloc(text_len + 1);
            if (buf_p->dialled_string_p->text_p == NULL)
            {
                free(buf_p->dialled_string_p);
                buf_p->dialled_string_p = NULL;
                return -1;
            }
            memset(buf_p->dialled_string_p->text_p, 0, text_len + 1);
            memcpy(buf_p->dialled_string_p->text_p, data_p->dialled_string_p->text_p, text_len);
        }
    }
    else
    {
        buf_p->dialled_string_p = NULL;
    }

    return rv;
}

int sim_copy_call_control_data(ste_cat_call_control_t * dest_p, ste_cat_call_control_t * source_p)
{
    int                     rv = 0;

    if (dest_p == NULL || source_p == NULL)
    {
        return -1;
    }

    dest_p->cc_type = source_p->cc_type;

    switch (source_p->cc_type)
    {
        case STE_CAT_CC_CALL_SETUP:
        {
            ste_cat_cc_call_setup_t *data_p;
            ste_cat_cc_call_setup_t *buf_p;

            data_p = (ste_cat_cc_call_setup_t*)(source_p->cc_data.call_setup_p);
            if (data_p == NULL)
            {
                return -1;
            }
            buf_p = malloc(sizeof(ste_cat_cc_call_setup_t));
            if (buf_p == NULL)
            {
                return -1;
            }
            memset(buf_p, 0, sizeof(ste_cat_cc_call_setup_t));
            //copy the address from data_p to buf_p
            rv = sim_copy_call_control_address(&(buf_p->address), &(data_p->address));

            //copy the sub address from data_p to buf_p
            buf_p->sub_address.len = data_p->sub_address.len;
            if (data_p->sub_address.len > 0 && data_p->sub_address.sub_address_p != NULL)
            {
                buf_p->sub_address.sub_address_p = malloc(data_p->sub_address.len + 1);
                if (buf_p->sub_address.sub_address_p == NULL)
                {
                    free(buf_p);
                    buf_p = NULL;
                    return -1;
                }
                memset(buf_p->sub_address.sub_address_p, 0, data_p->sub_address.len + 1);
                memcpy(buf_p->sub_address.sub_address_p, data_p->sub_address.sub_address_p, data_p->sub_address.len);
            }
            else
            {
                buf_p->sub_address.sub_address_p = NULL;
            }

            //copy the ccp_1 from data_p to buf_p
            buf_p->ccp_1.len = data_p->ccp_1.len;
            if (data_p->ccp_1.len > 0 && data_p->ccp_1.ccp_p != NULL)
            {
                buf_p->ccp_1.ccp_p = malloc(data_p->ccp_1.len + 1);
                if (buf_p->ccp_1.ccp_p == NULL)
                {
                    free(buf_p);
                    buf_p = NULL;
                    return -1;
                }
                memset(buf_p->ccp_1.ccp_p, 0, data_p->ccp_1.len + 1);
                memcpy(buf_p->ccp_1.ccp_p, data_p->ccp_1.ccp_p, data_p->ccp_1.len);
            }
            else
            {
                buf_p->ccp_1.ccp_p = NULL;
            }

            //copy the ccp_2 from data_p to buf_p
            buf_p->ccp_2.len = data_p->ccp_2.len;
            if (data_p->ccp_2.len > 0 && data_p->ccp_2.ccp_p != NULL)
            {
                buf_p->ccp_2.ccp_p = malloc(data_p->ccp_2.len + 1);
                if (buf_p->ccp_2.ccp_p == NULL)
                {
                    free(buf_p);
                    buf_p = NULL;
                    return -1;
                }
                memset(buf_p->ccp_2.ccp_p, 0, data_p->ccp_2.len + 1);
                memcpy(buf_p->ccp_2.ccp_p, data_p->ccp_2.ccp_p, data_p->ccp_2.len);
            }
            else
            {
                buf_p->ccp_2.ccp_p = NULL;
            }
            //copy the bc_repeat_ind from data_p to buf_p
            buf_p->bc_repeat_ind = data_p->bc_repeat_ind;

            //complete the copy, set the pointer to destination
            dest_p->cc_data.call_setup_p = buf_p;
        }
        break;
        case STE_CAT_CC_SS:
        {
            ste_cat_cc_ss_t *data_p;
            ste_cat_cc_ss_t *buf_p;

            data_p = (ste_cat_cc_ss_t*)(source_p->cc_data.ss_p);
            if (data_p == NULL)
            {
                return -1;
            }
            buf_p = malloc(sizeof(ste_cat_cc_ss_t));
            if (buf_p == NULL)
            {
                return -1;
            }
            memset(buf_p, 0, sizeof(ste_cat_cc_ss_t));
            //copy the address from data_p to buf_p
            rv = sim_copy_call_control_address(&(buf_p->address), &(data_p->address));

            //complete the copy, set the pointer to destination
            dest_p->cc_data.ss_p = buf_p;
        }
        break;
        case STE_CAT_CC_USSD:
        {
            ste_cat_cc_ussd_t *data_p;
            ste_cat_cc_ussd_t *buf_p;

            data_p = (ste_cat_cc_ussd_t*)(source_p->cc_data.ussd_p);
            if (data_p == NULL)
            {
                return -1;
            }
            buf_p = malloc(sizeof(ste_cat_cc_ussd_t));
            if (buf_p == NULL)
            {
                return -1;
            }
            memset(buf_p, 0, sizeof(ste_cat_cc_ussd_t));

            buf_p->ussd_data.dcs = data_p->ussd_data.dcs;
            //copy the dialled text string
            if (data_p->ussd_data.dialled_string_p != NULL)
            {
                buf_p->ussd_data.dialled_string_p = malloc(sizeof(ste_sim_string_t));
                if (buf_p->ussd_data.dialled_string_p == NULL)
                {
                    free(buf_p);
                    buf_p = NULL;
                    return -1;
                }
                memset(buf_p->ussd_data.dialled_string_p, 0, sizeof(ste_sim_string_t));

                buf_p->ussd_data.dialled_string_p->text_coding      = data_p->ussd_data.dialled_string_p->text_coding;
                buf_p->ussd_data.dialled_string_p->no_of_bytes = data_p->ussd_data.dialled_string_p->no_of_bytes;

                if (data_p->ussd_data.dialled_string_p->no_of_bytes > 0 && data_p->ussd_data.dialled_string_p->str_p != NULL)
                {
                    size_t       text_len;

                    text_len = data_p->ussd_data.dialled_string_p->no_of_bytes;

                    buf_p->ussd_data.dialled_string_p->str_p = malloc(text_len + 1);
                    if (buf_p->ussd_data.dialled_string_p->str_p == NULL)
                    {
                        free(buf_p->ussd_data.dialled_string_p);
                        buf_p->ussd_data.dialled_string_p = NULL;
                        free(buf_p);
                        buf_p = NULL;
                        return -1;
                    }
                    memset(buf_p->ussd_data.dialled_string_p->str_p, 0, text_len + 1);
                    memcpy(buf_p->ussd_data.dialled_string_p->str_p, data_p->ussd_data.dialled_string_p->str_p, text_len);
                }
            }
            else
            {
                buf_p->ussd_data.dialled_string_p = NULL;
            }

            //complete the copy, set the pointer to destination
            dest_p->cc_data.ussd_p = buf_p;
        }
        break;
        case STE_CAT_CC_PDP:
        {
            ste_cat_cc_pdp_t *data_p;
            ste_cat_cc_pdp_t *buf_p;

            data_p = (ste_cat_cc_pdp_t*)(source_p->cc_data.pdp_p);
            if (data_p == NULL)
            {
                return -1;
            }
            buf_p = malloc(sizeof(ste_cat_cc_pdp_t));
            if (buf_p == NULL)
            {
                return -1;
            }
            memset(buf_p, 0, sizeof(ste_cat_cc_pdp_t));

            if (data_p->pdp_context.str_p != NULL && data_p->pdp_context.no_of_bytes > 0)
            {
                size_t       text_len;

                buf_p->pdp_context.text_coding = data_p->pdp_context.text_coding;
                buf_p->pdp_context.no_of_bytes = data_p->pdp_context.no_of_bytes;


                text_len = data_p->pdp_context.no_of_bytes;

                buf_p->pdp_context.str_p = malloc(text_len + 1);
                if (buf_p->pdp_context.str_p == NULL)
                {
                    free(buf_p);
                    buf_p = NULL;
                    return -1;
                }
                memset(buf_p->pdp_context.str_p, 0, text_len + 1);
                memcpy(buf_p->pdp_context.str_p, data_p->pdp_context.str_p, text_len);
            }
            else
            {
                free(buf_p);
                buf_p = NULL;
            }

            //complete the copy, set the pointer to destination
            dest_p->cc_data.pdp_p = buf_p;
        }
        break;

        default:
        {
            return -1;
        }
    }

    return rv;
}

void sim_free_call_control_data(ste_cat_call_control_t * cc_p)
{

    if (cc_p == NULL) {
        return;
    }
    switch (cc_p->cc_type)
    {
        case STE_CAT_CC_CALL_SETUP:
        {
            ste_cat_cc_call_setup_t * call_setup_p = cc_p->cc_data.call_setup_p;           //call setup control
            if (!call_setup_p)
            {
                return;
            }
            if (call_setup_p->address.dialled_string_p)
            {
                if (call_setup_p->address.dialled_string_p->text_p)
                {
                    free(call_setup_p->address.dialled_string_p->text_p);
                }
                free(call_setup_p->address.dialled_string_p);
            }
            if (call_setup_p->sub_address.sub_address_p)
            {
                free(call_setup_p->sub_address.sub_address_p);
            }
            if (call_setup_p->ccp_1.ccp_p)
            {
                free(call_setup_p->ccp_1.ccp_p);
            }
            if (call_setup_p->ccp_2.ccp_p)
            {
                free(call_setup_p->ccp_2.ccp_p);
            }
            free(call_setup_p);
        }
        break;
        case STE_CAT_CC_SS:
        {
            ste_cat_cc_ss_t * ss_p = cc_p->cc_data.ss_p;                   //SS
            if (!ss_p)
            {
                return;
            }
            if (ss_p->address.dialled_string_p)
            {
                if (ss_p->address.dialled_string_p->text_p)
                {
                    free(ss_p->address.dialled_string_p->text_p);
                }
                free(ss_p->address.dialled_string_p);
            }
            free(ss_p);
        }
        break;
        case STE_CAT_CC_USSD:
        {
            ste_cat_cc_ussd_t * ussd_p = cc_p->cc_data.ussd_p;                 //USSD
            if (!ussd_p)
            {
                return;
            }
            if (ussd_p->ussd_data.dialled_string_p)
            {
                if (ussd_p->ussd_data.dialled_string_p->str_p)
                {
                    free(ussd_p->ussd_data.dialled_string_p->str_p);
                }
                free(ussd_p->ussd_data.dialled_string_p);
            }
            free(ussd_p);
        }
        break;
        case STE_CAT_CC_PDP:
        {
            ste_cat_cc_pdp_t * pdp_p = cc_p->cc_data.pdp_p;                 //PDP
            if (!pdp_p)
            {
                return;
            }
            if (pdp_p->pdp_context.str_p)
            {
                free(pdp_p->pdp_context.str_p);
            }
            free(pdp_p);
        }
        break;
        default:
        {
            //should be no data in the CC info structure
        }
    }
}

char * sim_encode_cc_user_indication(ste_sim_string_t * user_ind_p, size_t * result_len_p)
{
    char * buf_p = NULL;
    size_t buf_len = 0;
    char * temp_p = NULL;

    if (!user_ind_p)
    {
        return NULL;
    }
    if (user_ind_p->no_of_bytes == 0 || !user_ind_p->str_p)
    {
        return NULL;
    }
    buf_len = 1 + 1 + sizeof(user_ind_p->text_coding); //Tag + length + value for text_coding
    buf_len += 1 + 1 + sizeof(user_ind_p->no_of_bytes); //Tag + length + value for no of bytes
    buf_len += 1 + 1 + sizeof(char) * (user_ind_p->no_of_bytes); //Tag + length + value for no of bytes
    buf_p = malloc(buf_len);
    if (!buf_p)
    {
        return NULL;
    }
    memset(buf_p, 0, buf_len);
    temp_p = buf_p;

    *temp_p++ = SIM_CC_RSP_TAG_USER_IND_CODING;
    *temp_p++ = sizeof(user_ind_p->text_coding);
    temp_p = sim_enc(temp_p, &user_ind_p->text_coding, sizeof(user_ind_p->text_coding));

    *temp_p++ = SIM_CC_RSP_TAG_USER_IND_NO_OF_BYTES;
    *temp_p++ = sizeof(user_ind_p->no_of_bytes);
    temp_p = sim_enc(temp_p, &user_ind_p->no_of_bytes, sizeof(user_ind_p->no_of_bytes));

    *temp_p++ = SIM_CC_RSP_TAG_USER_IND_STRING;
    *temp_p++ = user_ind_p->no_of_bytes;
    temp_p = sim_enc(temp_p, user_ind_p->str_p, user_ind_p->no_of_bytes);

    if (buf_len != (size_t)(temp_p-buf_p)) {
        perror("simd : sim_encode_cc_user_indication, assert failure");
    }

    assert( buf_len == (size_t)(temp_p-buf_p) );

    *result_len_p = buf_len;
    return buf_p;
}

static void sim_decode_cc_user_indication(ste_sim_string_t * user_ind_p, uint8_t * buf_p, size_t len)
{
    uint8_t          *p = buf_p;
    uint8_t          *p_max = buf_p + len;
    uint8_t           tag;
    uint8_t           length;

    if (user_ind_p == NULL || buf_p == NULL)
    {
        return;
    }

    //the first byte is tag for SIM_CC_RSP_TAG_USER_IND_CODING
    p = (uint8_t*)sim_dec((char*)p, &tag, 1, (char*)p_max);
    if (tag != SIM_CC_RSP_TAG_USER_IND_CODING)
    {
        return;
    }
    p = (uint8_t*)sim_dec((char*)p, &length, 1, (char*)p_max);
    p = (uint8_t*)sim_dec((char*)p, &user_ind_p->text_coding, length, (char*)p_max);

    //then SIM_CC_RSP_TAG_USER_IND_NO_OF_BYTES
    p = (uint8_t*)sim_dec((char*)p, &tag, 1, (char*)p_max);
    if (tag != SIM_CC_RSP_TAG_USER_IND_NO_OF_BYTES)
    {
        return;
    }
    p = (uint8_t*)sim_dec((char*)p, &length, 1, (char*)p_max);
    p = (uint8_t*)sim_dec((char*)p, &user_ind_p->no_of_bytes, length, (char*)p_max);

    //then SIM_CC_RSP_TAG_USER_IND_STRING
    p = (uint8_t*)sim_dec((char*)p, &tag, 1, (char*)p_max);
    if (tag != SIM_CC_RSP_TAG_USER_IND_STRING)
    {
        return;
    }
    p = (uint8_t*)sim_dec((char*)p, &length, 1, (char*)p_max);
    if (length > 0)
    {
        user_ind_p->str_p = malloc(length + 1);
        if (!user_ind_p->str_p)
        {
            return;
        }
        memset(user_ind_p->str_p, 0, length + 1);
        p = (uint8_t*)sim_dec((char*)p, user_ind_p->str_p, length, (char*)p_max);
    }
    if ( !p )
    {
        return;
    }
}

char * sim_encode_cc_result(unsigned cc_result, size_t * result_len_p)
{
    char * buf_p = NULL;
    size_t buf_len = 0;
    char * temp_p = NULL;

    if (!result_len_p)
    {
        return NULL;
    }
    buf_len = 1 + 1 + sizeof(cc_result); //Tag + length + value for cc result
    buf_p = malloc(buf_len);
    if (!buf_p)
    {
        return NULL;
    }
    memset(buf_p, 0, buf_len);
    temp_p = buf_p;

    *temp_p++ = SIM_CC_RSP_TAG_CC_RESULT;
    *temp_p++ = sizeof(cc_result);
    temp_p = sim_enc(temp_p, &cc_result, sizeof(cc_result));

    if (buf_len != (size_t)(temp_p-buf_p)) {
        perror("simd : sim_encode_cc_result, assert failure");
    }

    assert( buf_len == (size_t)(temp_p-buf_p) );

    *result_len_p = buf_len;
    return buf_p;
}

char * ste_sim_encode_cc_response(ste_cat_call_control_result_t     cc_result,
                                  ste_cat_call_control_response_t * cc_rsp_p,
                                  size_t * result_len_p)
{
    char * buf_cc_p = NULL;
    size_t buf_cc_len = 0;
    char * buf_user_ind_p = NULL;
    size_t buf_user_ind_len = 0;
    char * buf_result_p = NULL;
    size_t buf_result_len = 0;
    char * buf_p = NULL;
    size_t buf_len = 0;
    char * temp_p = NULL;


    if (!result_len_p)
    {
        return NULL;
    }
    *result_len_p = 0;

    if (cc_rsp_p)
    {
        //encode the call control info
        buf_cc_p = sim_encode_call_control_data(&(cc_rsp_p->cc_info), &buf_cc_len);

        if (buf_cc_p == NULL) {
            buf_cc_len = 0;
        }
        //encode user indication
        buf_user_ind_p = sim_encode_cc_user_indication(&(cc_rsp_p->user_indication), &buf_user_ind_len);
    }

    //encode the cc result
    buf_result_p = sim_encode_cc_result((unsigned)cc_result, &buf_result_len);
    if (!buf_result_p)
    {
        if (buf_user_ind_p)
        {
            free(buf_user_ind_p);
        }
        return NULL;
    }

    //now concatenate the 3 parts into one binary string
    buf_len = buf_cc_len + buf_user_ind_len + buf_result_len;
    buf_len += 1 + 1 + 1 + 1;  //we add one byte for tag, one byte for length, for CC_INFO, USER_IND respectively

    buf_p = malloc(buf_len + 1);
    if (!buf_p)
    {
        if (buf_cc_p)
        {
            free(buf_cc_p);
        }
        if (buf_user_ind_p)
        {
            free(buf_user_ind_p);
        }
        if (buf_result_p)
        {
            free(buf_result_p);
        }

        return NULL;
    }
    memset(buf_p, 0, buf_len + 1);
    temp_p = buf_p;

    memcpy(temp_p, buf_result_p, buf_result_len);
    free(buf_result_p);

    temp_p += buf_result_len;

    *temp_p++ = SIM_CC_RSP_TAG_CC_INFO;
    *temp_p++ = buf_cc_len;

    if (buf_cc_p)
    {
        memcpy(temp_p, buf_cc_p, buf_cc_len);
        temp_p += buf_cc_len;
        free(buf_cc_p);
    }

    *temp_p++ = SIM_CC_RSP_TAG_USER_INDICATION;
    *temp_p++ = buf_user_ind_len;

    if (buf_user_ind_p)
    {
        memcpy(temp_p, buf_user_ind_p, buf_user_ind_len);
        temp_p += buf_user_ind_len;
        free(buf_user_ind_p);
    }

    *result_len_p = buf_len;

    return buf_p;
}

int  ste_sim_decode_cc_response(int * ec_status_p,
                                ste_cat_call_control_response_t * cc_rsp_p,
                                const char * data_p,
                                size_t data_len)
{
    const char       *p = data_p;
    const char       *p_max = data_p + data_len;
    unsigned          status_len = 0;
    uint8_t           tag;
    uint8_t           length;
    unsigned          remaining_len;

    if (data_p == NULL || data_len == 0 || cc_rsp_p == NULL || ec_status_p == NULL)
    {
        return -1;
    }

    memset(cc_rsp_p, 0, sizeof(ste_cat_call_control_response_t));
    //decode the EC status first
    p = sim_dec(p, &status_len, sizeof(status_len), p_max);
    p = sim_dec(p, ec_status_p, status_len, p_max);
    //get the length of remaining part
    p = sim_dec(p, &remaining_len, sizeof(remaining_len), p_max);

    if (remaining_len == 0)
    {
        //we reach the end of string
        return 0;
    }

    //otherwise, now ready to decode CC result
    p = sim_dec((char*)p, &tag, 1, (char*)p_max);
    if (tag != SIM_CC_RSP_TAG_CC_RESULT)
    {
        return -1;
    }

    p = sim_dec((char*)p, &length, 1, (char*)p_max);
    p = sim_dec((char*)p, &cc_rsp_p->cc_result, length, (char*)p_max);

    //now ready to decode CC INFO
    p = sim_dec((char*)p, &tag, 1, (char*)p_max);
    if (tag != SIM_CC_RSP_TAG_CC_INFO)
    {
        return -1;
    }
    p = sim_dec((char*)p, &length, 1, (char*)p_max);
    if (length > 0)
    {
        //decode the call control info
        if (sim_decode_call_control_data(&(cc_rsp_p->cc_info), (uint8_t*)p, length) != 0)
        {
            return -1;
        }
        p += length;
    }

    if ( !p )
    {
        return -1;
    }
    //now ready to decode USER INDICATION
    p = sim_dec((char*)p, &tag, 1, (char*)p_max);
    if (tag != SIM_CC_RSP_TAG_USER_INDICATION)
    {
        return -1;
    }
    p = sim_dec((char*)p, &length, 1, (char*)p_max);

    if (length > 0)
    {
        //decode user indication
        sim_decode_cc_user_indication(&(cc_rsp_p->user_indication), (uint8_t*)p, length);
    }

    if ( !p )
    {
        return -1;
    }

    return 0;
}

void sim_free_call_control_response_data(ste_cat_call_control_response_t * cc_rsp_p)
{
    if (cc_rsp_p == NULL)
    {
        return;
    }

    sim_free_call_control_data(&(cc_rsp_p->cc_info));

    if (cc_rsp_p->user_indication.str_p)
    {
        free(cc_rsp_p->user_indication.str_p);
    }
}

int sim_encode_ton_npi(ste_sim_type_of_number_t ton, ste_sim_numbering_plan_id_t npi, uint8_t * buf_p)
{
    if (buf_p == NULL) {
        return -1;
    }

    *buf_p = (0x0f & npi) | ((0x07 & ton)<<4) | 0x80;
    return 0;
}

int sim_decode_ton_npi(uint8_t ton_npi, ste_sim_type_of_number_t * ton_p, ste_sim_numbering_plan_id_t * npi_p)
{
    if (ton_p == NULL || npi_p == NULL) {
        return -1;
    }

    *ton_p = (ste_sim_type_of_number_t)((ton_npi >> 4) & 0x07);
    *npi_p = (ste_sim_numbering_plan_id_t)(ton_npi & 0x0F);
    return 0;
}

#define SERVICEPARAMS_RECSIZE 28 /* Known length of a record in EF_SMSP excluding the alpha-identifier */

#define EF_SMSP_OFFSET_TP_DEST_ADDR           0x01  //the offset of destination address in smsp file
#define EF_SMSP_OFFSET_TS_SRV_CENTRE_ADDR     0x0D  //the offset of service center address in smsp file
#define EF_SMSP_OFFSET_TP_PROTOCOL_ID         0x19  //the offset of protocol id in smsp file
#define EF_SMSP_OFFSET_TP_DATA_CODING_SCHEME  0x1A  //the offset of data coding scheme in smsp file
#define EF_SMSP_OFFSET_TP_VALIDITY_PERIOD     0x1B  //the offset of validity period in smsp file


int utility_ASCII8_to_UCS2( ste_sim_text_t  * source_text_p,
                            uint16_t        * UCS2_text_p,
                            uint16_t          max_len,
                            uint16_t        * actual_len_p)
{
    uint16_t          target_counter = 0;
    uint16_t          source_counter = 0;
    uint8_t*          source_data_p;
    uint16_t*         UCS2_p = UCS2_text_p;

    if ((source_text_p == NULL) || (source_text_p->text_p == NULL) ||
        (actual_len_p == NULL) || (UCS2_text_p == NULL))
    {
        return -1;
    }

    *actual_len_p = 0;

    if (source_text_p->text_coding != STE_SIM_ASCII8)
    {
        return -1;
    }

    source_data_p = (uint8_t*) source_text_p->text_p;

    while ((source_counter < source_text_p->no_of_characters) &&
           (target_counter < max_len - 1)) // "-1" as we use 2 target bytes each loop
    {
        *UCS2_p = (uint16_t) *source_data_p;
        source_data_p++;
        UCS2_p++;
        source_counter++;
        target_counter = target_counter + 2;  // Two bytes for each input char
    }

    if (source_counter < source_text_p->no_of_characters)
    {
        return -1;
    }

    *actual_len_p = (uint16_t) target_counter;

    return 0;
}

uint8_t utility_UCS2char_to_BCD (uint16_t UCS2_char)
{
    if ((UCS2_char >= '0') && (UCS2_char <= '9'))
    {
        return (uint8_t) (UCS2_char - 0x30);
    }
    switch(UCS2_char)
    {
        case '*' : return 0x0A;
        case '#' : return 0x0B;
        case 'C' : return 0x0C;
        case 'D' : return 0x0D;
        case 'E' : return 0x0E;

       default:   return (0x0F);
    }
}

uint8_t utility_BCD_to_ASCII8(uint8_t bcd)
{
    uint8_t translate[] = "0123456789*#CDEF";
    return (bcd < 0xF) ? translate[bcd] : translate[0xF];
}

int utility_ASCII8_to_BCD(ste_sim_text_t* source_text_p,
                          uint8_t*        BCD_text_p,
                          uint16_t        max_len,
                          uint16_t*       actual_len_p)
{
    unsigned needed_size;
    uint8_t* src = NULL;
    uint8_t* end = NULL;

    if ((source_text_p == NULL)                        ||
        (source_text_p->text_p == NULL)                ||
        (source_text_p->text_coding != STE_SIM_ASCII8) ||
        (actual_len_p == NULL)                         ||
        (BCD_text_p == NULL)                            ) {
        return -1;
    }

    needed_size = ((unsigned)(source_text_p->no_of_characters + 1)) >> 1;
    if (max_len < needed_size) {
        return -1;
    }

    (*actual_len_p) = 0;
    src = source_text_p->text_p;
    end = src + source_text_p->no_of_characters;
    while (src < end)
    {
        uint8_t lsn, msn;
        lsn = utility_UCS2char_to_BCD((uint16_t)*src);
        src++;
        msn = (src < end) ? utility_UCS2char_to_BCD((uint16_t)*src) : 0x0F;

        *BCD_text_p = ((msn << 4) | lsn);
        BCD_text_p++;
        (*actual_len_p)++;

        src++;
    }
    return 0;
}

int utility_UCS2_to_BCD( ste_sim_text_t  * source_text_p,
                         uint8_t         * BCD_text_p,
                         uint16_t          max_len,
                         uint16_t        * actual_len_p)
{
    uint16_t          target_byte_counter = 0;
    uint16_t          source_counter = 0;
    int               break_flag = FALSE;
    uint16_t*         source_data_p;
    uint8_t*          BCD_p = BCD_text_p;
    uint8_t           MS_nybble;
    uint8_t           LS_nybble;

    if ((source_text_p == NULL) || (source_text_p->text_p == NULL) ||
        (actual_len_p == NULL) || (BCD_text_p == NULL))
    {
        return -1;
    }

    if (source_text_p->text_coding != STE_SIM_UCS2)
    {
        return -1;
    }

    if (((source_text_p->no_of_characters + 1) / 2) > max_len)
    {
        return -1;
    }

    *actual_len_p = 0;
    source_data_p = (uint16_t*) source_text_p->text_p;

    while (TRUE)
    {
        // Low nybble, odd number, new byte.
        if (source_counter == source_text_p->no_of_characters) // Last character
        {
            *actual_len_p = target_byte_counter;
            return 0;
        }
        target_byte_counter++;
        LS_nybble = utility_UCS2char_to_BCD(*source_data_p);
        source_data_p++;
        source_counter++;

        // High nybble, even number, same byte.
        if (source_counter == source_text_p->no_of_characters) // Last character
        {
            MS_nybble = 0x0F;
            break_flag = TRUE;
        }
        else
        {
            MS_nybble = utility_UCS2char_to_BCD(*source_data_p);
            source_data_p++;
            source_counter++;
        }

        *BCD_p = (uint8_t) (LS_nybble | (MS_nybble << 4)) ;
        BCD_p++;

        if (break_flag)
        {
            break;
        }
    }

    *actual_len_p = target_byte_counter;
    return 0;
}

int sim_convert_UCS2_to_BCD(ste_sim_text_t  * source_p,
                            uint16_t          max_len,
                            uint16_t        * actual_len_p,
                            uint8_t         * buf_p)
{
    int rv = -1;

    if (source_p == NULL || buf_p == NULL || actual_len_p == NULL)
    {
        return -1;
    }

    *actual_len_p = 0;     /* Initialise return value */
    if (source_p->text_p == NULL || source_p->text_coding != STE_SIM_UCS2)
    {
        return -1;
    }

    if (source_p->no_of_characters != 0)
    {
        uint16_t converted_bytes = 0;

        if ((source_p->no_of_characters & 0x01) == 0x00)
        { /* The util can coupe with even number of digits */
            rv = utility_UCS2_to_BCD(source_p, buf_p, max_len, &converted_bytes);
        }
        else
        { /* Odd number of BCD which the utils cant cope with, so add on a space char */
            uint16_t* plus_one_p = malloc((source_p->no_of_characters + 1) * sizeof(uint16_t));
            if (plus_one_p)
            { /* mem allocated, so add space */
                ste_sim_text_t plus_one;

                plus_one.text_coding = STE_SIM_UCS2;
                plus_one.no_of_characters = source_p->no_of_characters + 1;
                plus_one.text_p = plus_one_p;
                memcpy(plus_one_p, source_p->text_p, source_p->no_of_characters * sizeof(uint16_t));
                plus_one_p[source_p->no_of_characters] = ' ';
                rv = utility_UCS2_to_BCD(&plus_one, buf_p, max_len, &converted_bytes);
                free(plus_one_p);
            }
            else
            {
                return -1;
            }
        }
        *actual_len_p = converted_bytes;
    }
    else
    { /* There were no characters to convert */
        *actual_len_p = 0;
    }

    return 0;
}

int sim_convert_ASCII8_to_BCD(ste_sim_text_t  * source_p,
                              uint16_t          max_len,
                              uint16_t        * actual_len_p,
                              uint8_t         * buf_p)
{
    int rv = -1;

    if (source_p == NULL || buf_p == NULL || actual_len_p == NULL)
    {
        return -1;
    }

    *actual_len_p = 0;     /* Initialise return value */
    if (source_p->text_p == NULL || source_p->text_coding != STE_SIM_ASCII8)
    {
        return -1;
    }

    if (source_p->no_of_characters != 0)
    {
        uint16_t    converted_bytes = 0;
        uint16_t    UCS2_text_len = source_p->no_of_characters * 2;
        uint16_t*   UCS2_text_p;
        uint16_t    result_len;

        /* Rather then re-invent the wheel, convert to UCS2 first(!) then convert to BCD */
        UCS2_text_p = (uint16_t*)malloc(UCS2_text_len);

        if (!UCS2_text_p)
        {
            printf("uicc : memory allocation failed in sim_convert_ASCII8_to_BCD! \n");
            return -1;
        }

        rv = utility_ASCII8_to_UCS2( source_p, (uint16_t*)UCS2_text_p, UCS2_text_len, &result_len);

        if (rv == 0)
        {
            ste_sim_text_t  UCS2_data;

            UCS2_data.no_of_characters = source_p->no_of_characters;
            UCS2_data.text_coding = STE_SIM_UCS2;
            UCS2_data.text_p = UCS2_text_p;

            rv = sim_convert_UCS2_to_BCD(&UCS2_data, max_len, &converted_bytes, buf_p);
        }
        free(UCS2_text_p);

        *actual_len_p = converted_bytes;
    }
    else
    { /* There were no characters to convert */
        *actual_len_p = 0;
    }

    return 0;
}

int sim_convert_BCD_to_ASCII8(ste_sim_text_t  * source_p,
                              uint16_t          max_len,
                              uint16_t        * actual_len_p,
                              uint8_t         * buf_p)
{
    if (source_p == NULL || buf_p == NULL || actual_len_p == NULL)
    {
        return -1;
    }

    *actual_len_p = 0;     /* Initialise return value */
    if (source_p->text_p == NULL || source_p->text_coding != STE_SIM_BCD)
    {
        return -1;
    }

    if (source_p->no_of_characters != 0)
    {
        uint16_t    result_len = 0;
        uint8_t    *index_p = source_p->text_p;
        uint16_t    char_nr = source_p->no_of_characters;

        while (result_len < max_len)
        {
            *buf_p++ = utility_BCD_to_ASCII8((*index_p) & 0x0F);
            --char_nr;
            ++result_len;
            if (char_nr == 0)
            {
                break;
            }
            *buf_p++ = utility_BCD_to_ASCII8(((*index_p) >> 4) & 0x0F);
            --char_nr;
            ++result_len;
            ++index_p;
            if (char_nr == 0)
            {
                break;
            }
        }

        *actual_len_p = result_len;
    }
    else
    { /* There were no characters to convert */
        *actual_len_p = 0;
    }

    return 0;
}

int sim_undo_create_ril_smsc_str(ste_sim_short_message_address_t * address_p, ste_sim_call_number_t * smsc_p)
{
    int rc = sim_encode_ton_npi(smsc_p->ton, smsc_p->npi, &(address_p->addr_type));
    if (!rc) {
        uint16_t len = 0;
        rc = utility_ASCII8_to_BCD(&(smsc_p->num_text),
                                   address_p->addr_value,
                                   SMR_MAX_SM_ADDRESS_LENGTH,
                                   &len);

        address_p->length = (uint8_t)len;
    }
    return rc;
}

/*
 * Wrapper for sim_convert_BCD_to_ASCII8 to fix handling of dialling strings
 * with odd number of digits.
 */
int sim_convert_BCD_to_ASCII8_handle_odd_bytes(ste_sim_text_t* source_p,
                                               uint16_t        max_len,
                                               uint16_t*       actual_len_p,
                                               uint8_t*        buf_p)
{
    int rc = sim_convert_BCD_to_ASCII8(source_p, max_len, actual_len_p, buf_p);
    if (rc >= 0) {
        /*
         * Check if length is greater than zero and is an even number.
         */
        if (*actual_len_p > 0 && !(*actual_len_p & 0x01)) {
            /*
             * If last digit is an 'F', then it should now have been included.
             */
            if (buf_p[*actual_len_p - 1] == 'F') {
                buf_p[*actual_len_p - 1] = 0; /* Zero terminate on the 'F' position */
                *actual_len_p -= 1; /* Decrease the length */
            }
        }
    }
    return rc;
}

int sim_create_ril_smsc_str(ste_sim_call_number_t * smsc_p, ste_sim_short_message_address_t * address_p)
{
    uint16_t        result_len = 0;
    ste_sim_text_t  tmp;
    int             buffer_allocated_by_us = FALSE;
    int             rc = -1;

    if (smsc_p && address_p) {
        uint8_t len = 2 * MIN(address_p->length, SMR_MAX_SM_ADDRESS_LENGTH);

        /*
         * If no destination text buffer is allocated, then allocate one with the necessary space!
         */
        if (!smsc_p->num_text.text_p) {
            smsc_p->num_text.text_p = calloc(1, len + 1); /* Needed space is 2 * len, but add one for zero termination just in case... */
            buffer_allocated_by_us = TRUE;
        }

        if (smsc_p->num_text.text_p) {
            tmp.no_of_characters = len; /* Correct value is either len or len - 1, but sim_convert_BCD_to_ASCII8 only cares about the
                                           value being non-zero and conversion function will giveus the correct lenght anyway */
            tmp.text_coding = STE_SIM_BCD;
            tmp.text_p = address_p->addr_value;
            rc = sim_convert_BCD_to_ASCII8_handle_odd_bytes(&tmp, len, &result_len, smsc_p->num_text.text_p);
            if (rc >= 0) {
                (void)sim_decode_ton_npi(address_p->addr_type, &smsc_p->ton, &smsc_p->npi); /* Can't fail unless we pass null pointers,
                                                                                               so ignore return value */
                smsc_p->num_text.text_coding = STE_SIM_ASCII8;
                smsc_p->num_text.no_of_characters = result_len;
                rc = 0;
            }
        }

        if (rc < 0) {
            smsc_p->ton = STE_SIM_TON_UNKNOWN;
            smsc_p->npi = STE_SIM_NPI_UNKNOWN;
            smsc_p->num_text.text_coding = STE_SIM_CODING_UNKNOWN;
            smsc_p->num_text.no_of_characters = 0;
            if (smsc_p->num_text.text_p && buffer_allocated_by_us) {
                free(smsc_p->num_text.text_p);
                smsc_p->num_text.text_p = NULL;
            }
        }
    }
    return rc;
}

int sim_encode_smsp_record_smsc(ste_sim_call_number_t * smsc_p, uint8_t * buf_p)
{
    int                            rv;
    uint8_t                        ton_npi;
    uint8_t                        length;
    uint16_t                       wlength;
    uint8_t                      * index_p = NULL;

    if (smsc_p == NULL || buf_p == NULL) {
        return -1;
    }

    rv = sim_encode_ton_npi(smsc_p->ton, smsc_p->npi, &ton_npi);

    switch (smsc_p->num_text.text_coding)
    {
        case STE_SIM_ASCII8:
        {
            if (smsc_p->num_text.no_of_characters == 0) {
                smsc_p->num_text.no_of_characters = strlen((char*)smsc_p->num_text.text_p);
            }
            index_p = buf_p + 1 + 1; //move to the value part
            rv = sim_convert_ASCII8_to_BCD(&smsc_p->num_text, SMR_MAX_SM_ADDRESS_LENGTH, &wlength, index_p);
            *buf_p++ = wlength + 1;
            *buf_p++ = ton_npi;
        }
        break;
        case STE_SIM_UCS2:
        {
            index_p = buf_p + 1 + 1; //move to the value part
            rv = sim_convert_UCS2_to_BCD(&smsc_p->num_text, SMR_MAX_SM_ADDRESS_LENGTH, &wlength, index_p);
            *buf_p++ = wlength + 1;
            *buf_p++ = ton_npi;
        }
        break;
        case STE_SIM_BCD:
        {
            //check how many characters are in the BCD string
            if (smsc_p->num_text.no_of_characters == 0) {
                length = strlen((char*)(smsc_p->num_text.text_p));
                if (length > 0) {
                    index_p = (uint8_t*)smsc_p->num_text.text_p + length - 1; //move to the last byte
                    if (((*index_p) & 0xF0) == 0xF0) {  //odd number
                        smsc_p->num_text.no_of_characters = length * 2 - 1;
                    }
                    else {
                        smsc_p->num_text.no_of_characters = length * 2;
                    }
                }
            }
            else {
                length = get_length_in_bytes_for_BCD(smsc_p->num_text.no_of_characters);
            }

            *buf_p++ = length + 1;
            *buf_p++ = ton_npi;
            memcpy(buf_p, (uint8_t*)smsc_p->num_text.text_p, length);
        }
        break;
        default:
        {
            return -1;
        }
    }
    return 0;
}

/*************************************************************************
 * Function:      get_length_in_bytes_for_BCD
 *
 * Description:   Returns the length in bytes for the BCD string.
 *
 * Input Params:  nr_char      The number of characters in the BCD string.
 *
 * Return:        Length in bytes
 *
 *************************************************************************/
uint8_t get_length_in_bytes_for_BCD(uint8_t nr_char)
{
    uint8_t length;

    if ((nr_char & 0x01) == 0) {
        length = nr_char / 2;
    }
    else {
        length = nr_char / 2 + 1;
    }

    return length;
}

/*************************************************************************
 * Function:      get_length_in_digits_for_BCD
 *
 * Description:   Returns the length in digits for the BCD string.
 *                Function assumes the bcd number starts at index 0, and
 *                is ended either by buffer end or by a nibble set to 0xf.
 *
 * Input Params:  Buffer and size of the bcd string
 *
 * Return:        Length in digits (or chars)
 *
 *************************************************************************/
uint8_t get_length_in_digits_for_BCD(uint8_t* buf, size_t len)
{
  int i;
  for(i=0;i<(int)len;i++) {
    if ((buf[i] & 0x0f) == 0x0f)
      return (uint8_t)i*2;
    if ((buf[i] & 0xf0) == 0xf0)
      return (uint8_t)i*2+1;
  }
  // Reached end of buffer with all contents being bcd
  return (uint8_t)i*2;
}

uint8_t* sim_encode_smsp_record(ste_sim_call_number_t* smsc_p, int* result_len_p, ste_sim_smsp_t* smsp_p, uint8_t rec_len)
{
    uint8_t* result_p = NULL;
    uint8_t* index_p;

    if ( (smsc_p != NULL)                                                       && /* Make sure a SMSCA pointer is given*/
         (smsc_p->num_text.no_of_characters <= (2 * SMR_MAX_SM_ADDRESS_LENGTH)) && /* Make sure the SCA number has a valid length */
         (result_len_p != NULL)                                                 && /* Make sure a return buffer length pointer was given */
         (rec_len >= SERVICEPARAMS_RECSIZE))                                    {  /* Make sure the record size is valid */

        *result_len_p = 0;
        result_p = malloc(rec_len);
        if (result_p) {
            uint8_t param_ind = (STE_SIM_SMSP_DEST_ADDRESS_ABSENT  | STE_SIM_SMSP_SC_ADDRESS_ABSENT   | STE_SIM_SMSP_PROTOCOL_ID_ABSENT |
                                 STE_SIM_SMSP_CODING_SCHEME_ABSENT | STE_SIM_SMSP_VALID_PERIOD_ABSENT | STE_SIM_SMSP_BIT_RESERVED       );

            memset(result_p, 0xFF, rec_len);

            /*
             * Copy destination address if present in source
             */
            if (!(smsp_p->indicators & STE_SIM_SMSP_DEST_ADDRESS_ABSENT)) {
                /*
                 * Source parameter indicators indicates that destination address is present,
                 * but we also check that the length says so too.
                 */
                if (smsp_p->destination_address.length > 0) {
                    uint8_t len = smsp_p->destination_address.length;

                    /*
                     * Make sure length is set to a valid value, regardless of what the source indicates.
                     */
                    if (len > SMR_MAX_SM_ADDRESS_LENGTH) {
                        len = SMR_MAX_SM_ADDRESS_LENGTH;
                    }

                    param_ind &= (~STE_SIM_SMSP_DEST_ADDRESS_ABSENT); /* Set indication that destination address is present */
                    index_p    = result_p + rec_len - SERVICEPARAMS_RECSIZE + EF_SMSP_OFFSET_TP_DEST_ADDR;
                    *index_p++ = len + 1; /* Length byte indicates length including the ton/npi byte. */
                    *index_p++ = smsp_p->destination_address.addr_type;
                    memcpy(index_p, smsp_p->destination_address.addr_value, len);
                }
            }

            /*
             * Encode the new  Service Centre Address
             */
            if (smsc_p->num_text.no_of_characters > 0) {
                index_p = result_p + rec_len - SERVICEPARAMS_RECSIZE + EF_SMSP_OFFSET_TS_SRV_CENTRE_ADDR;
                /*
                 * Encode SCA, no need to check max length of source, it was checked above...
                 */
                if (sim_encode_smsp_record_smsc(smsc_p, index_p) >= 0) {
                    param_ind &= (~STE_SIM_SMSP_SC_ADDRESS_ABSENT);
                } else {
                    free(result_p);
                    return NULL;
                }
            }

            /*
             * Copy Protocol Identifier if present in source
             */
            if (!(smsp_p->indicators & STE_SIM_SMSP_PROTOCOL_ID_ABSENT)) {
                param_ind &= (~STE_SIM_SMSP_PROTOCOL_ID_ABSENT);
                index_p    = result_p + rec_len - SERVICEPARAMS_RECSIZE + EF_SMSP_OFFSET_TP_PROTOCOL_ID;
                *index_p   = smsp_p->protocol_id;
            }

            /*
             * Copy Data Coding Scheme if present in the source
             */
            if (!(smsp_p->indicators & STE_SIM_SMSP_CODING_SCHEME_ABSENT)) {
                param_ind &= (~STE_SIM_SMSP_CODING_SCHEME_ABSENT);
                index_p    = result_p + rec_len - SERVICEPARAMS_RECSIZE + EF_SMSP_OFFSET_TP_DATA_CODING_SCHEME;
                *index_p   = smsp_p->coding_scheme;
            }

            /*
             * Copy Validity Period if present in the source
             */
            if (!(smsp_p->indicators & STE_SIM_SMSP_VALID_PERIOD_ABSENT)) {
                param_ind &= (~STE_SIM_SMSP_VALID_PERIOD_ABSENT);
                index_p    = result_p + rec_len - SERVICEPARAMS_RECSIZE + EF_SMSP_OFFSET_TP_VALIDITY_PERIOD;
                *index_p   = smsp_p->validity_period;
            }

            /*
             * Write Parameter Indicators
             */
            index_p = result_p + rec_len - SERVICEPARAMS_RECSIZE;
            *index_p = param_ind;

            *result_len_p = rec_len;
        }
    }
    return result_p;
}

int sim_decode_smsp_record(ste_sim_smsp_t* smsp_p, uint8_t* data_p, size_t data_len)
{
    uint8_t* index_p;
    uint8_t  param_ind;
    uint8_t  len;

    if (!smsp_p || !data_p || data_len < SERVICEPARAMS_RECSIZE) {
        return -1;
    }

    index_p = data_p + data_len - SERVICEPARAMS_RECSIZE;
    param_ind = *index_p;
    memset(smsp_p, 0, sizeof(ste_sim_smsp_t));
    smsp_p->indicators = (STE_SIM_SMSP_DEST_ADDRESS_ABSENT  | STE_SIM_SMSP_SC_ADDRESS_ABSENT   | STE_SIM_SMSP_PROTOCOL_ID_ABSENT |
                          STE_SIM_SMSP_CODING_SCHEME_ABSENT | STE_SIM_SMSP_VALID_PERIOD_ABSENT | STE_SIM_SMSP_BIT_RESERVED       );

    if (!(param_ind & STE_SIM_SMSP_DEST_ADDRESS_ABSENT)) {
        index_p = data_p + data_len - SERVICEPARAMS_RECSIZE + EF_SMSP_OFFSET_TP_DEST_ADDR;
        len = *index_p++; /* length incl. ton/npi */
        if (len > 1) {
            len--; /* Exclude ton/npi from lenght */
            if (len > SMR_MAX_SM_ADDRESS_LENGTH) {
                /*
                 * If we get here, then the destination address data in EFsmsp is bad.
                 * Just set the length to the defined maximum.
                 */
                len = SMR_MAX_SM_ADDRESS_LENGTH;
            }
            smsp_p->indicators &= (~STE_SIM_SMSP_DEST_ADDRESS_ABSENT);
            smsp_p->destination_address.length = len;
            smsp_p->destination_address.addr_type = *index_p++;
            memcpy(smsp_p->destination_address.addr_value, index_p, len);
        }
    }

    if (!(param_ind & STE_SIM_SMSP_SC_ADDRESS_ABSENT)) {
        index_p = data_p + data_len - SERVICEPARAMS_RECSIZE + EF_SMSP_OFFSET_TS_SRV_CENTRE_ADDR;
        len = *index_p++; /* length incl. ton/npi */
        if (len > 1) {
            len--; /* Exclude ton/npi from lenght */
            if (len > SMR_MAX_SM_ADDRESS_LENGTH) {
                /*
                 * If we get here, then the service centre address data in EFsmsp is bad.
                 * Just set the length to the defined maximum.
                 */
                len = SMR_MAX_SM_ADDRESS_LENGTH;
            }
            smsp_p->indicators &= (~STE_SIM_SMSP_SC_ADDRESS_ABSENT);
            smsp_p->service_center_address.length = len;
            smsp_p->service_center_address.addr_type = *index_p++;
            memcpy(smsp_p->service_center_address.addr_value, index_p, len);
        }
    }

    if (!(param_ind & STE_SIM_SMSP_PROTOCOL_ID_ABSENT)) {
        index_p = data_p + data_len - SERVICEPARAMS_RECSIZE + EF_SMSP_OFFSET_TP_PROTOCOL_ID;
        smsp_p->indicators &= (~STE_SIM_SMSP_PROTOCOL_ID_ABSENT);
        smsp_p->protocol_id = *index_p;
    }

    if (!(param_ind & STE_SIM_SMSP_CODING_SCHEME_ABSENT)) {
        index_p = data_p + data_len - SERVICEPARAMS_RECSIZE + EF_SMSP_OFFSET_TP_DATA_CODING_SCHEME;
        smsp_p->indicators &= (~STE_SIM_SMSP_CODING_SCHEME_ABSENT);
        smsp_p->coding_scheme = *index_p;
    }

    if (!(param_ind & STE_SIM_SMSP_VALID_PERIOD_ABSENT)) {
        index_p = data_p + data_len - SERVICEPARAMS_RECSIZE + EF_SMSP_OFFSET_TP_VALIDITY_PERIOD;
        smsp_p->indicators &= (~STE_SIM_SMSP_VALID_PERIOD_ABSENT);
        smsp_p->validity_period = *index_p;
    }

    return 0;
}

/*************************************************************************
 * Function:      sim_get_text_string_length
 *
 * Description:   Returns the size of the text string based on a
 *                particular text coding scheme.
 *
 * Input Params:  source_p      The source text string.
 *
 * Return:        Length of text string
 *
 *************************************************************************/
size_t sim_get_text_string_length(ste_sim_text_t  * source_p)
{
    size_t length = 0;

    if (source_p == NULL) {
        return 0;
    }
    switch (source_p->text_coding)
    {
        case STE_SIM_ASCII8:
        {
            length = source_p->no_of_characters;
        }
        break;
        case STE_SIM_UCS2:
        {
            length = source_p->no_of_characters * 2;
        }
        break;
        case STE_SIM_BCD:
        {
            uint8_t                      * index_p = NULL;

            //check how many characters are in the BCD string
            if (source_p->no_of_characters == 0 && source_p->text_p != NULL) {
                length = strlen((char*)(source_p->text_p));
                index_p = (uint8_t*)source_p->text_p + length - 1; //move to the last byte
                if (((*index_p) & 0xF0) == 0xF0) {  //odd number
                    source_p->no_of_characters = length * 2 - 1;
                }
                else {
                    source_p->no_of_characters = length * 2;
                }
            }
            else {
                length = get_length_in_bytes_for_BCD(source_p->no_of_characters);
            }
        }
        break;
        default:
        {
            return 0;
        }
    }
    return length;
}

char * sim_encode_smsc_string(ste_sim_call_number_t * smsc_p, size_t * result_len_p)
{
    char * p;
    char * buf;
    size_t buf_len = 0;
    size_t text_len = 0;

    if (!smsc_p) {
        return NULL;
    }

    buf_len = sizeof(ste_sim_type_of_number_t) + sizeof(ste_sim_numbering_plan_id_t) + sizeof(ste_sim_text_coding_t) + sizeof(unsigned);
    text_len = sim_get_text_string_length(&(smsc_p->num_text));
    buf_len += sizeof(text_len);
    buf_len += text_len * sizeof(char);

    buf = malloc(buf_len + 1);
    if (buf == NULL) {
        return NULL;
    }
    memset(buf, 0, buf_len + 1); // Useful if encoding is wrong
    *result_len_p = buf_len;

    p = buf;
    p = sim_enc(p, &(smsc_p->ton), sizeof(ste_sim_type_of_number_t));
    p = sim_enc(p, &(smsc_p->npi), sizeof(ste_sim_numbering_plan_id_t));
    p = sim_enc(p, &(smsc_p->num_text.text_coding), sizeof(ste_sim_text_coding_t));
    p = sim_enc(p, &(smsc_p->num_text.no_of_characters), sizeof(unsigned));
    p = sim_enc(p, &text_len, sizeof(text_len));
    p = sim_enc(p, smsc_p->num_text.text_p, text_len);

    if (buf_len != (size_t)(p - buf)) {
        perror("uicc : sim_encode_smsc_string, assert failure");
    }

    assert( buf_len == (size_t)(p - buf) );

    return buf;
}

void sim_decode_smsc_string(ste_sim_call_number_t * smsc_p, const char * data_p, size_t data_len)
{
    const char       *p = data_p;
    const char       *p_max = data_p + data_len;
    size_t            text_len = 0;

    if (smsc_p == NULL || data_p == NULL || data_len == 0) {
        return;
    }

    p = sim_dec(p, &(smsc_p->ton), sizeof(ste_sim_type_of_number_t), p_max);
    p = sim_dec(p, &(smsc_p->npi), sizeof(ste_sim_numbering_plan_id_t), p_max);
    p = sim_dec(p, &(smsc_p->num_text.text_coding), sizeof(ste_sim_text_coding_t), p_max);
    p = sim_dec(p, &(smsc_p->num_text.no_of_characters), sizeof(unsigned), p_max);
    p = sim_dec(p, &text_len, sizeof(text_len), p_max);
    if (text_len > 0) {
        smsc_p->num_text.text_p = malloc(text_len + 1);
        if (smsc_p->num_text.text_p == NULL) {
            return;
        }
        memset(smsc_p->num_text.text_p, 0, text_len + 1);
        p = sim_dec(p, smsc_p->num_text.text_p, text_len, p_max);
        if ( !p ) {
            return;
        }
    } else {
        smsc_p->num_text.text_p = NULL;
    }
}

ste_sim_ctrl_block_t * ste_sim_create_ctrl_block(uintptr_t client_tag,
                                                 int fd,
                                                 uint32_t trans_id,
                                                 ste_sim_transaction_handler_func* handler_func,
                                                 void * data_p)
{
    ste_sim_ctrl_block_t * ctrl_p;

    ctrl_p = malloc(sizeof(ste_sim_ctrl_block_t));
    if (ctrl_p == NULL) {
        return NULL;
    }
    ctrl_p->client_tag = client_tag;
    ctrl_p->fd = fd;
    ctrl_p->transaction_id = trans_id;
    ctrl_p->transaction_handler_func = handler_func;
    ctrl_p->transaction_data_p = data_p;
    //generate an unique tag id for SIM internal usage, when needed. Currently set to NULL
    ctrl_p->sim_tag = 0;

    return ctrl_p;
}


void sim_decode_read_generic_result(ste_uicc_sim_file_read_generic_response_t * result_p, const char * data_p, size_t data_len)
{
    const char       *p = data_p;
    const char       *p_max = data_p + data_len;
    unsigned          buf_len = 0;
    unsigned          file_id_len;

    if (result_p == NULL || data_p == NULL || data_len == 0) {
        return;
    }
    memset(result_p, 0, sizeof(ste_uicc_sim_file_read_generic_response_t));

    p = sim_dec(p, &result_p->uicc_status_code, sizeof(result_p->uicc_status_code), p_max);
    p = sim_dec(p, &result_p->uicc_status_code_fail_details, sizeof(result_p->uicc_status_code_fail_details), p_max);
    p = sim_dec(p, &result_p->status_word.sw1, sizeof(result_p->status_word.sw1), p_max);
    p = sim_dec(p, &result_p->status_word.sw2, sizeof(result_p->status_word.sw2), p_max);
    p = sim_dec(p, &buf_len, sizeof(buf_len), p_max);
    if ( !p ) {
        return;
    }

    result_p->data_len = buf_len;
    if (buf_len > 0) {
        result_p->data_p = malloc(buf_len + 1);
        memset(result_p->data_p, 0, buf_len + 1);
        p = sim_dec(p, result_p->data_p, sizeof(char)*(buf_len), p_max);
        if ( !p ) {
            free(result_p->data_p);
            result_p->data_len = 0;
            return;
        }
    }
    p = sim_dec(p, &file_id_len, sizeof(file_id_len), p_max);
    p = sim_dec(p, &(result_p->file_id), sizeof(char)*(file_id_len), p_max);
    if ( !p ) {
        return;
    }
}

void ste_sim_decode_status_response(int * status_p, const char * data_p, size_t data_len)
{
    const char       *p = data_p;
    const char       *p_max = data_p + data_len;
    unsigned          status_len = 0;;

    if (status_p == NULL || data_p == NULL || data_len == 0) {
        return;
    }

    //decode the status data
    p = sim_dec(p, &status_len, sizeof(status_len), p_max);
    p = sim_dec(p, status_p, sizeof(char)*(status_len), p_max);
    if ( !p ) {
        return;
    }
}

char * ste_sim_decode_length_data(size_t * buf_len_p, const char * data_p, size_t data_len)
{
    const char       *p = data_p;
    const char       *p_max = data_p + data_len;
    size_t            length = 0;
    char             *result_buf_p = NULL;

    if (data_p == NULL || data_len == 0 || buf_len_p == NULL) {
        return NULL;
    }

    *buf_len_p = 0;
    p = sim_dec(p, &length, sizeof(length), p_max);
    if (length > 0) {
        result_buf_p = malloc(length);
        if (!result_buf_p) {
            return NULL;
        }
        p = sim_dec(p, result_buf_p, sizeof(char)*(length), p_max);
    }
    *buf_len_p = length;
    if ( !p ) {
        free(result_buf_p);
        return NULL;
    }
    return result_buf_p;
}

char * ste_sim_encode_length_data(size_t * buf_len_p, const char * data_p, size_t data_len)
{
    char            * p = NULL;
    size_t            buf_len = 0;
    char            * buf = NULL;

    if (data_p == NULL || data_len == 0 || buf_len_p == NULL) {
        return NULL;
    }

    *buf_len_p = 0;
    buf_len = sizeof(char) * data_len + sizeof(data_len);

    buf = malloc(buf_len);
    if (buf == NULL){
        return NULL;
    }
    p = buf;
    p = sim_enc(p, &data_len, sizeof(data_len));
    p = sim_enc(p, data_p,    sizeof(char) * (data_len));

    if (buf_len != (size_t)(p - buf)) {
        perror("simd : ste_sim_encode_length_data, assert failure");
    }

    assert( buf_len == (size_t)(p - buf) );
    *buf_len_p = buf_len;

    return buf;
}

void sim_decode_pc_ind(ste_cat_pc_ind_t * pc_p, const char * data_p, size_t data_len)
{
    const char       *p = data_p;
    const char       *p_max = data_p + data_len;
    unsigned          apdu_len = 0;

    if (pc_p == NULL || data_p == NULL || data_len == 0) {
        return;
    }
    memset(pc_p, 0, sizeof(ste_cat_pc_ind_t));

    p = sim_dec(p, &apdu_len, sizeof(apdu_len), p_max);
    if (apdu_len > 0) {
        pc_p->apdu.buf = malloc(apdu_len + 1);
        if (pc_p->apdu.buf == NULL) {
            return;
        }
        memset(pc_p->apdu.buf, 0, apdu_len + 1);
        p = sim_dec(p, pc_p->apdu.buf, apdu_len, p_max);
        pc_p->apdu.len = apdu_len;
    }
    if ( !p ) {
        return;
    }
}

void ste_sim_decode_cat_response(ste_cat_ec_response_t * ec_p, const char * data_p, size_t data_len)
{
    const char       *p = data_p;
    const char       *p_max = data_p + data_len;
    unsigned          status_len = 0;
    unsigned          apdu_len = 0;

    if (ec_p == NULL || data_p == NULL || data_len == 0) {
        return;
    }
    memset(ec_p, 0, sizeof(ste_cat_ec_response_t));

    p = sim_dec(p, &status_len, sizeof(status_len), p_max);
    p = sim_dec(p, &(ec_p->ec_status), status_len, p_max);
    p = sim_dec(p, &apdu_len, sizeof(apdu_len), p_max);
    if (apdu_len > 0) {
        ec_p->apdu.buf = malloc(apdu_len + 1);
        if (ec_p->apdu.buf == NULL) {
            return;
        }
        memset(ec_p->apdu.buf, 0, apdu_len + 1);
        p = sim_dec(p, ec_p->apdu.buf, apdu_len, p_max);
        ec_p->apdu.len = apdu_len;
    }
    if ( !p ) {
        return;
    }
}

ste_uicc_sim_plmn_AccessTechnology_t ste_sim_get_accesstechnology(unsigned const char *raw_access_tech_p)
{
  ste_uicc_sim_plmn_AccessTechnology_t access_tech;
  access_tech = raw_access_tech_p[1];
  access_tech = (access_tech  << 8) + raw_access_tech_p[0];

  return (access_tech);
}

void ste_sim_decode_plmn_with_AcT(ste_sim_plmn_with_AcT_t *ste_sim_plmn_withAcT_p, unsigned char *data_index_p)
{
    ste_sim_plmn_withAcT_p->PLMN.MCC1 = *(data_index_p) & SIM_PLMN_LOWNIBBLE_MASK;
    ste_sim_plmn_withAcT_p->PLMN.MCC2 = (*(data_index_p) & SIM_PLMN_HIGHNIBBLE_MASK) >> 4;
    ste_sim_plmn_withAcT_p->PLMN.MCC3 = *(data_index_p + 1) & SIM_PLMN_LOWNIBBLE_MASK;
    ste_sim_plmn_withAcT_p->PLMN.MNC3 = (*(data_index_p + 1) & SIM_PLMN_HIGHNIBBLE_MASK) >> 4;
    ste_sim_plmn_withAcT_p->PLMN.MNC1 = *(data_index_p + 2) & SIM_PLMN_LOWNIBBLE_MASK;
    ste_sim_plmn_withAcT_p->PLMN.MNC2 = (*(data_index_p + 2) & SIM_PLMN_HIGHNIBBLE_MASK) >> 4;
}

void ste_sim_decode_plmn_response(ste_uicc_sim_file_read_plmn_response_t *plmn_p, const char *data_p, size_t data_len)
{
    const char             *p_max = data_p + data_len;
    const char             *p = data_p;
    unsigned char          *raw_plmn_data_p;
    unsigned char          *tmp_raw_data_p;
    unsigned                plmn_data_len = 0;
    unsigned char           raw_data_index = 0;
    int                     index = 0;
    uint8_t                 file_id_offset = 0;
    ste_sim_plmn_with_AcT_t *ste_sim_plmn_withAcT_p;
    ste_sim_plmn_with_AcT_t *tmp_plmn_withAcT_p;
    ste_uicc_sim_plmn_file_id_t file_id;
    size_t                   status_len = sizeof(plmn_p->uicc_status_code) +
                                          sizeof(plmn_p->uicc_status_code_fail_details) +
                                          sizeof(plmn_p->status_word.sw1) +
                                          sizeof(plmn_p->status_word.sw2) +
                                          sizeof(ste_uicc_sim_plmn_file_id_t);

    memset(plmn_p, 0, sizeof(ste_uicc_sim_file_read_plmn_response_t));
    plmn_p->nr_of_plmn = 0;
    plmn_p->ste_sim_plmn_with_AcT_p = NULL;

    p = sim_dec(p, &plmn_p->uicc_status_code, sizeof(plmn_p->uicc_status_code), p_max);
    p = sim_dec(p, &plmn_p->uicc_status_code_fail_details, sizeof(plmn_p->uicc_status_code_fail_details), p_max);
    p = sim_dec(p, &plmn_p->status_word.sw1, sizeof(plmn_p->status_word.sw1), p_max);
    p = sim_dec(p, &plmn_p->status_word.sw2, sizeof(plmn_p->status_word.sw2), p_max);
    p = sim_dec(p, &file_id, sizeof(ste_uicc_sim_plmn_file_id_t), p_max);

    file_id_offset = (file_id == SIM_FILE_ID_PLMN_WACT) ? SIM_PLMN_WITH_ACT_SIZE : SIM_PLMN_SIZE;

    if (data_len > status_len) {
        p = sim_dec(p, &plmn_data_len, sizeof(plmn_data_len), p_max);
        /* The EF PLMNsel file size must be a multiple of the size of each entry */
        if (0 != (plmn_data_len % file_id_offset)) {
            plmn_p->uicc_status_code = STE_UICC_STATUS_CODE_FAIL;
            plmn_p->uicc_status_code_fail_details
                    = STE_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR;
            perror("simd : ste_sim_decode_plmn_sel_response, assert failure");
            return;
        }
        plmn_p->nr_of_plmn = (plmn_data_len / file_id_offset);
        plmn_p->ste_sim_plmn_with_AcT_p = malloc(plmn_p->nr_of_plmn
                * sizeof(ste_sim_plmn_with_AcT_t));
        if (NULL == plmn_p->ste_sim_plmn_with_AcT_p) {
            return;
        }
        tmp_plmn_withAcT_p = plmn_p->ste_sim_plmn_with_AcT_p;

        /* Allocate space for temporary data */
        raw_plmn_data_p = malloc(plmn_data_len);
        if (NULL == raw_plmn_data_p) {
            return;
        }
        tmp_raw_data_p = raw_plmn_data_p;
        p = sim_dec(p, tmp_raw_data_p, plmn_data_len, p_max);
        for (index = 0; index < plmn_p->nr_of_plmn; index++) {
            ste_sim_plmn_withAcT_p = tmp_plmn_withAcT_p;
            ste_sim_decode_plmn_with_AcT(ste_sim_plmn_withAcT_p, tmp_raw_data_p + raw_data_index);
            if (SIM_FILE_ID_PLMN_WACT == file_id) {
                /* Add Access thecnology if file id is PLMNwact*/
                ste_sim_plmn_withAcT_p->AcT = ste_sim_get_accesstechnology(tmp_raw_data_p
                        + raw_data_index + SIM_PLMN_SIZE);
            } else {
                ste_sim_plmn_withAcT_p->AcT = SIM_PLMN_DEFAULT_ACT;
            }
            raw_data_index = (index + 1) * file_id_offset;
            *tmp_plmn_withAcT_p = *ste_sim_plmn_withAcT_p;
            tmp_plmn_withAcT_p++;
        }
        free(raw_plmn_data_p);
}
    if (!p) {
        return;
    }
}

void sim_decode_plmn_string(ste_sim_plmn_with_AcT_t *plmn_p, int *index, ste_uicc_sim_plmn_file_id_t *file_id, int *remove, const char * data_p, size_t data_len)
{
    const char *p = data_p;
    const char *p_max = data_p + data_len;

    if (plmn_p == NULL || data_p == NULL || data_len == 0) {
        return;
    }

    if (data_len == (sizeof(int) + sizeof(ste_uicc_sim_plmn_file_id_t))) {
        // only one int and file_id was sent, this indicate that we should remove the element
        p = sim_dec(p, index, sizeof(int), p_max);
        p = sim_dec(p, file_id, sizeof(ste_uicc_sim_plmn_file_id_t), p_max);
        *remove = 1;
    } else {

        p = sim_dec(p, &(plmn_p->PLMN), sizeof(ste_uicc_sim_plmn_t), p_max);
        p = sim_dec(p, &(plmn_p->AcT), sizeof(ste_uicc_sim_plmn_AccessTechnology_t), p_max);
        p = sim_dec(p, index, sizeof(int), p_max);
        p = sim_dec(p, file_id, sizeof(ste_uicc_sim_plmn_file_id_t), p_max);
        *remove = 0;
    }
    if (!p) {
        return;
    }
}

char *sim_encode_plmn_element(ste_sim_plmn_with_AcT_t *plmn_p, int index, ste_uicc_sim_plmn_file_id_t file_id, size_t *result_len_p)
{
    char *buf = NULL;
    char *p = NULL;
    size_t buf_len = 0;

    if (plmn_p == NULL) {
        // remove element
        buf_len = sizeof(int) + sizeof(ste_uicc_sim_plmn_file_id_t);
        buf = malloc(buf_len);
        if (buf == NULL) {
            return NULL;
        }
        memset(buf, 0, buf_len); // Useful if encoding is wrong
        p = buf;
        p = sim_enc(p, &index, sizeof(int));
        p = sim_enc(p, &file_id, sizeof(ste_uicc_sim_plmn_file_id_t));

    } else {
        buf_len = sizeof(ste_uicc_sim_plmn_t) + sizeof(ste_uicc_sim_plmn_AccessTechnology_t)
                + sizeof(int) + sizeof(ste_uicc_sim_plmn_file_id_t);
        buf = malloc(buf_len);
        if (buf == NULL) {
            return NULL;
        }
        memset(buf, 0, buf_len); // Useful if encoding is wrong

        p = buf;
        p = sim_enc(p, &plmn_p->PLMN, sizeof(ste_uicc_sim_plmn_t));
        p = sim_enc(p, &plmn_p->AcT, sizeof(ste_uicc_sim_plmn_AccessTechnology_t));
        p = sim_enc(p, &index, sizeof(int));
        p = sim_enc(p, &file_id, sizeof(ste_uicc_sim_plmn_file_id_t));
    }

    *result_len_p = buf_len;
    if (buf_len != (size_t)(p - buf)) {
        perror("uicc : sim_encode_plmn_string, assert failure");
    }

    assert(buf_len == (size_t)(p - buf));

    return buf;
}

void sim_decode_read_plmn(ste_uicc_sim_plmn_file_id_t *file_id, const char * data_p, size_t data_len)
{
    const char *p = data_p;
    const char *p_max = data_p + data_len;

    if (!file_id) {
        return;
    }
    p = sim_dec(p, file_id, sizeof(ste_uicc_sim_plmn_file_id_t), p_max);

    if (!p) {
        return;
    }
}

char *sim_encode_read_plmn(ste_uicc_sim_plmn_file_id_t *file_id_p, size_t *result_len_p)
{
    char *buf = NULL;
    char *p = NULL;
    size_t buf_len = 0;

    buf_len = sizeof(ste_uicc_sim_plmn_file_id_t);
    buf = malloc(buf_len);
    if (!buf) {
        return NULL;
    }

    memset(buf, 0, buf_len); // Useful if encoding is wrong
    p = buf;
    p = sim_enc(p, file_id_p, sizeof(ste_uicc_sim_plmn_file_id_t));

    *result_len_p = buf_len;
    if (buf_len != (size_t)(p - buf)) {
        perror("uicc : sim_encode_read_plmn, assert failure");
    }

    assert(buf_len == (size_t)(p - buf));

    return buf;

}

void sim_create_imsi_response(ste_uicc_sim_file_read_imsi_response_t * imsi_p, uint8_t * data_p, size_t data_len)
{
    size_t from = 1;
    size_t to = 0;
    size_t i;

    if (imsi_p == NULL || data_p == NULL || data_len <= 0) {
        return;
    }
    memset(imsi_p, 0, sizeof(ste_uicc_sim_file_read_imsi_response_t));

    memcpy( imsi_p->raw_imsi, data_p, sizeof(imsi_p->raw_imsi) );

    memset(imsi_p->imsi, 0x0F, SIM_EF_IMSI_STRING_LEN); //set to default value
    // Copy the nibbles first.
    // Byte 0 is the length
    // LSN of byte 1 is parity information
    imsi_p->imsi[to++] = ( data_p[from++] >> 4 ) & 0x0F;

    while ( to < SIM_EF_IMSI_STRING_LEN && from < SIM_EF_IMSI_LEN ) {
        imsi_p->imsi[to++] = data_p[from] & 0x0F;
        imsi_p->imsi[to++] = ( data_p[from] >> 4 ) & 0x0F;
        from++;
    }

    // Now convert raw nibbles to printable characters, and set remainder
    // 0xF bytes to \0.
    for ( i = 0 ; i < SIM_EF_IMSI_STRING_LEN ; i++ ) {
        if ( imsi_p->imsi[i] <= 9 ) {
            imsi_p->imsi[i] += '0';
        } else {
            // These should be 0xF if they're not 0 to 9
            imsi_p->imsi[i] = '\0';
        }
    }
}

static char* sim_encode_dialled_address(ste_cat_cc_dialled_address_t * address_p, uint8_t * len_p)
{
    char     *buf_p = NULL;
    char     *temp_p = NULL;
    uint8_t   length = 0;
    uint8_t   text_len;

    if (!address_p || !len_p)
    {
        return NULL;
    }
    *len_p = 0;
    if (address_p->dialled_string_p == NULL)
    {
        return NULL;
    }

    length = 1 + 1 + sizeof(address_p->ton);  //Tag + length + value for TON
    length += 1 + 1 + sizeof(address_p->npi);  //NPI
    length += 1 + 1 + sizeof(address_p->dialled_string_p->text_coding);  //TEXT CODING
    length += 1 + 1 + sizeof(address_p->dialled_string_p->no_of_characters);  //Number of characters
    length += 1 + 1;  //dial text, TAG + Length

    text_len = sim_get_text_string_length(address_p->dialled_string_p);

    length += text_len;  //value part of dial text

    buf_p = malloc(length + 1);
    if (buf_p == NULL)
    {
        return NULL;
    }
    memset(buf_p, 0, length + 1);
    temp_p = buf_p;

    *temp_p++ = SIM_CC_TAG_TON;
    *temp_p++ = sizeof(address_p->ton);
    temp_p = sim_enc(temp_p, &address_p->ton, sizeof(address_p->ton));

    *temp_p++ = SIM_CC_TAG_NPI;
    *temp_p++ = sizeof(address_p->npi);
    temp_p = sim_enc(temp_p, &address_p->npi, sizeof(address_p->npi));

    *temp_p++ = SIM_CC_TAG_CODING;
    *temp_p++ = sizeof(address_p->dialled_string_p->text_coding);
    temp_p = sim_enc(temp_p, &address_p->dialled_string_p->text_coding, sizeof(address_p->dialled_string_p->text_coding));

    *temp_p++ = SIM_CC_TAG_NO_OF_CHARACTERS;
    *temp_p++ = sizeof(address_p->dialled_string_p->no_of_characters);
    temp_p = sim_enc(temp_p, &address_p->dialled_string_p->no_of_characters, sizeof(address_p->dialled_string_p->no_of_characters));

    *temp_p++ = SIM_CC_TAG_DIAL_STR_TEXT;
    *temp_p++ = text_len;
    temp_p = sim_enc(temp_p, address_p->dialled_string_p->text_p, text_len);

    if (length != (size_t)(temp_p - buf_p)) {
        perror("simd : sim_encode_dialled_address, assert failure");
    }

    assert( length == (size_t)(temp_p - buf_p) );

    *len_p = length;

    return buf_p;
}

//For sms control
char* sim_encode_sms_control_data(ste_cat_sms_control_t * sc_p, size_t * len_p)
{
    char     *buf_p = NULL;
    char     *temp_p = NULL;
    size_t    length = 0;
    uint8_t   smsc_len;
    char     *smsc_p = NULL;
    uint8_t   dest_len;
    char     *dest_p = NULL;

    if (sc_p == NULL || len_p == NULL)
    {
        return NULL;
    }
    *len_p = 0;

    //encode SMSC first
    smsc_p = sim_encode_dialled_address(&(sc_p->smsc), &smsc_len);
    //add tag and length
    length = 1 + 1 + smsc_len;  //Tag + length + value for SMSC
    //encode destination
    dest_p = sim_encode_dialled_address(&(sc_p->dest), &dest_len);
    //add tag and length
    length += 1 + 1 + dest_len;  //Tag + length + value for DESTINATION

    buf_p = malloc(length + 1);
    if (buf_p == NULL)
    {
        free(smsc_p);
        free(dest_p);
        return NULL;
    }
    memset(buf_p, 0, length + 1);
    temp_p = buf_p;

    *temp_p++ = SIM_CC_TAG_SMSC;
    *temp_p++ = smsc_len;
    memcpy(temp_p, smsc_p, smsc_len);
    temp_p += smsc_len;

    *temp_p++ = SIM_CC_TAG_DEST;
    *temp_p++ = dest_len;
    memcpy(temp_p, dest_p, dest_len);
    temp_p += dest_len;

    free(smsc_p);
    free(dest_p);

    *len_p = length;

    return buf_p;
}

static int sim_decode_dialled_address(ste_cat_cc_dialled_address_t * address_p, uint8_t * buf_p, size_t len)
{
    uint8_t          *p = buf_p;
    uint8_t          *p_max = buf_p + len;
    uint8_t           tag;
    uint8_t           length;

    if (address_p == NULL || buf_p == NULL)
    {
        return -1;
    }
    if (address_p->dialled_string_p == NULL)
    {
        return -1;
    }

    //now ready to decode TON
    p = (uint8_t*)sim_dec((char*)p, &tag, 1, (char*)p_max);
    if (tag != SIM_CC_TAG_TON)
    {
        return -1;
    }
    p = (uint8_t*)sim_dec((char*)p, &length, 1, (char*)p_max);
    p = (uint8_t*)sim_dec((char*)p, &address_p->ton, length, (char*)p_max);

    //now ready to decode NPI
    p = (uint8_t*)sim_dec((char*)p, &tag, 1, (char*)p_max);
    if (tag != SIM_CC_TAG_NPI)
    {
        return -1;
    }
    p = (uint8_t*)sim_dec((char*)p, &length, 1, (char*)p_max);
    p = (uint8_t*)sim_dec((char*)p, &address_p->npi, length, (char*)p_max);

    //now ready to decode Text Coding
    p = (uint8_t*)sim_dec((char*)p, &tag, 1, (char*)p_max);
    if (tag != SIM_CC_TAG_CODING)
    {
        return -1;
    }
    p = (uint8_t*)sim_dec((char*)p, &length, 1, (char*)p_max);
    p = (uint8_t*)sim_dec((char*)p, &(address_p->dialled_string_p->text_coding), length, (char*)p_max);

    //now ready to decode Number of characters
    p = (uint8_t*)sim_dec((char*)p, &tag, 1, (char*)p_max);
    if (tag != SIM_CC_TAG_NO_OF_CHARACTERS)
    {
        return -1;
    }
    p = (uint8_t*)sim_dec((char*)p, &length, 1, (char*)p_max);
    p = (uint8_t*)sim_dec((char*)p, &address_p->dialled_string_p->no_of_characters, length, (char*)p_max);

    //now ready to decode dialled text string
    p = (uint8_t*)sim_dec((char*)p, &tag, 1, (char*)p_max);
    if (tag != SIM_CC_TAG_DIAL_STR_TEXT)
    {
        return -1;
    }
    p = (uint8_t*)sim_dec((char*)p, &length, 1, (char*)p_max);
    if (length > 0)
    {
        address_p->dialled_string_p->text_p = malloc(length + 1);
        if (address_p->dialled_string_p->text_p == NULL)
        {
            return -1;
        }
        memset(address_p->dialled_string_p->text_p, 0, length + 1);
        p = (uint8_t*)sim_dec((char*)p, address_p->dialled_string_p->text_p, length, (char*)p_max);
    }

    if (p == NULL) {
        perror("simd : sim_decode_dialled_address, assert failure");
    }

    assert(p != NULL);
    return 0;
}

int sim_decode_sms_control_data(ste_cat_sms_control_t * sc_p, uint8_t * buf_p, size_t len)
{
    uint8_t          *p = buf_p;
    uint8_t          *p_max = buf_p + len;
    uint8_t           tag;
    uint8_t           length;
    int               rv;

    if (sc_p == NULL || buf_p == NULL)
    {
        return -1;
    }

    memset(sc_p, 0, sizeof(ste_cat_sms_control_t));

    //the first byte is SMSC tag
    p = (uint8_t*)sim_dec((char*)p, &tag, 1, (char*)p_max);
    if (tag != SIM_CC_TAG_SMSC)
    {
        return -1;
    }
    p = (uint8_t*)sim_dec((char*)p, &length, 1, (char*)p_max);
    if (length > 0)
    {
        sc_p->smsc.dialled_string_p = malloc(sizeof(ste_sim_text_t));
        if (!sc_p->smsc.dialled_string_p)
        {
            return -1;
        }
        memset(sc_p->smsc.dialled_string_p, 0, sizeof(ste_sim_text_t));
        rv = sim_decode_dialled_address(&(sc_p->smsc), p, length);
        if (rv == -1)
        {
            free(sc_p->smsc.dialled_string_p);
            sc_p->smsc.dialled_string_p = NULL;
            return -1;
        }
        p += length;
    }
    else
    {
        sc_p->smsc.dialled_string_p = NULL;
    }

    //the first byte is DEST tag
    p = (uint8_t*)sim_dec((char*)p, &tag, 1, (char*)p_max);
    if (tag != SIM_CC_TAG_DEST)
    {
        free(sc_p->smsc.dialled_string_p);
        sc_p->smsc.dialled_string_p = NULL;
        return -1;
    }
    p = (uint8_t*)sim_dec((char*)p, &length, 1, (char*)p_max);

    if (length > 0)
    {
        sc_p->dest.dialled_string_p = malloc(sizeof(ste_sim_text_t));
        if (!sc_p->dest.dialled_string_p)
        {
            free(sc_p->smsc.dialled_string_p);
            sc_p->smsc.dialled_string_p = NULL;
            return -1;
        }
        memset(sc_p->dest.dialled_string_p, 0, sizeof(ste_sim_text_t));
        rv = sim_decode_dialled_address(&(sc_p->dest), p, length);
        if (rv == -1)
        {
            free(sc_p->smsc.dialled_string_p);
            free(sc_p->dest.dialled_string_p);
            sc_p->dest.dialled_string_p = NULL;
            sc_p->smsc.dialled_string_p = NULL;
            return -1;
        }
        p += length;
    }
    else
    {
        sc_p->dest.dialled_string_p = NULL;
    }
    assert(p != NULL);

    return 0;
}

void sim_free_sms_control_data(ste_cat_sms_control_t * sc_p)
{

    if (sc_p == NULL) {
        return;
    }
    if (sc_p->smsc.dialled_string_p)
    {
        if (sc_p->smsc.dialled_string_p->text_p)
        {
            free(sc_p->smsc.dialled_string_p->text_p);
        }
        free(sc_p->smsc.dialled_string_p);
    }
    if (sc_p->dest.dialled_string_p)
    {
        if (sc_p->dest.dialled_string_p->text_p)
        {
            free(sc_p->dest.dialled_string_p->text_p);
        }
        free(sc_p->dest.dialled_string_p);
    }
}


char * ste_sim_encode_sc_response(ste_cat_sms_control_result_t     sc_result,
                                  ste_cat_sms_control_response_t * sc_rsp_p,
                                  size_t * result_len_p)
{
    char * buf_sc_p = NULL;
    size_t buf_sc_len = 0;
    char * buf_user_ind_p = NULL;
    size_t buf_user_ind_len = 0;
    char * buf_result_p = NULL;
    size_t buf_result_len = 0;
    char * buf_p = NULL;
    size_t buf_len = 0;
    char * temp_p = NULL;

    //NOTE: reuse the tags for call control
    if (!result_len_p)
    {
        return NULL;
    }
    *result_len_p = 0;

    if (sc_rsp_p)
    {
        //encode the sms control info
        buf_sc_p = sim_encode_sms_control_data(&(sc_rsp_p->sc_info), &buf_sc_len);

        if (buf_sc_p == NULL) {
            return NULL;
        }
        //encode user indication
        buf_user_ind_p = sim_encode_cc_user_indication(&(sc_rsp_p->user_indication), &buf_user_ind_len);
    }

    //encode the sc result
    buf_result_p = sim_encode_cc_result((unsigned)sc_result, &buf_result_len);
    if (!buf_result_p)
    {
        if (buf_user_ind_p)
        {
            free(buf_user_ind_p);
        }
        if (buf_sc_p)
        {
            free(buf_sc_p);
        }
        return NULL;
    }

    //now concatenate the 3 parts into one binary string
    buf_len = buf_sc_len + buf_user_ind_len + buf_result_len;
    buf_len += 1 + 1 + 1 + 1;  //we add one byte for tag, one byte for length, for SC_INFO, USER_IND respectively

    buf_p = malloc(buf_len + 1);
    if (!buf_p)
    {
        if (buf_sc_p)
        {
            free(buf_sc_p);
        }
        if (buf_user_ind_p)
        {
            free(buf_user_ind_p);
        }
        if (buf_result_p)
        {
            free(buf_result_p);
        }

        return NULL;
    }
    memset(buf_p, 0, buf_len + 1);
    temp_p = buf_p;

    memcpy(temp_p, buf_result_p, buf_result_len);
    free(buf_result_p);

    temp_p += buf_result_len;

    *temp_p++ = SIM_CC_RSP_TAG_CC_INFO;
    *temp_p++ = buf_sc_len;

    if (buf_sc_p)
    {
        memcpy(temp_p, buf_sc_p, buf_sc_len);
        temp_p += buf_sc_len;
        free(buf_sc_p);
    }

    *temp_p++ = SIM_CC_RSP_TAG_USER_INDICATION;
    *temp_p++ = buf_user_ind_len;

    if (buf_user_ind_p)
    {
        memcpy(temp_p, buf_user_ind_p, buf_user_ind_len);
        temp_p += buf_user_ind_len;
        free(buf_user_ind_p);
    }

    *result_len_p = buf_len;

    return buf_p;
}

int  ste_sim_decode_sc_response(int * ec_status_p,
                                ste_cat_sms_control_response_t * sc_rsp_p,
                                const char * data_p,
                                size_t data_len)
{
    const char       *p = data_p;
    const char       *p_max = data_p + data_len;
    unsigned          status_len = 0;
    uint8_t           tag;
    uint8_t           length;
    unsigned          remaining_len;

    if (data_p == NULL || data_len == 0 || sc_rsp_p == NULL || ec_status_p == NULL)
    {
        return -1;
    }

    memset(sc_rsp_p, 0, sizeof(ste_cat_sms_control_response_t));
    //decode the EC status first
    p = sim_dec(p, &status_len, sizeof(status_len), p_max);
    p = sim_dec(p, ec_status_p, status_len, p_max);
    //get the length of remaining part
    p = sim_dec(p, &remaining_len, sizeof(remaining_len), p_max);

    if (remaining_len == 0)
    {
        //we reach the end of string
        return 0;
    }

    //otherwise, now ready to decode SC result
    p = sim_dec((char*)p, &tag, 1, (char*)p_max);
    if (tag != SIM_CC_RSP_TAG_CC_RESULT)
    {
        return -1;
    }

    p = sim_dec((char*)p, &length, 1, (char*)p_max);
    p = sim_dec((char*)p, &sc_rsp_p->sc_result, length, (char*)p_max);

    //now ready to decode SC INFO
    p = sim_dec((char*)p, &tag, 1, (char*)p_max);
    if (tag != SIM_CC_RSP_TAG_CC_INFO)
    {
        return -1;
    }
    p = sim_dec((char*)p, &length, 1, (char*)p_max);
    if (length > 0)
    {
        //decode the sms control info
        if (sim_decode_sms_control_data(&(sc_rsp_p->sc_info), (uint8_t*)p, length) != 0)
        {
            return -1;
        }
        p += length;
    }

    if ( !p )
    {
        return -1;
    }
    //now ready to decode USER INDICATION
    p = sim_dec((char*)p, &tag, 1, (char*)p_max);
    if (tag != SIM_CC_RSP_TAG_USER_INDICATION)
    {
        return -1;
    }
    p = sim_dec((char*)p, &length, 1, (char*)p_max);

    if (length > 0)
    {
        //decode user indication
        sim_decode_cc_user_indication(&(sc_rsp_p->user_indication), (uint8_t*)p, length);
    }

    if ( !p )
    {
        return -1;
    }

    return 0;
}

void sim_free_sms_control_response_data(ste_cat_sms_control_response_t * sc_rsp_p)
{
    if (sc_rsp_p == NULL)
    {
        return;
    }

    sim_free_sms_control_data(&(sc_rsp_p->sc_info));

    if (sc_rsp_p->user_indication.str_p)
    {
        free(sc_rsp_p->user_indication.str_p);
    }
}


int sim_copy_sms_control_data(ste_cat_sms_control_t * dest_p, ste_cat_sms_control_t * source_p)
{
    int                     rv = 0;

    if (dest_p == NULL || source_p == NULL)
    {
        return -1;
    }

    memcpy(dest_p, source_p, sizeof(ste_cat_sms_control_t));

    rv = sim_copy_call_control_address(&(dest_p->smsc), &(source_p->smsc));

    rv = sim_copy_call_control_address(&(dest_p->dest), &(source_p->dest));

    return rv;
}

char* sim_encode_event_download_data(uint8_t   event_type,
                                     void    * event_data_p,
                                     size_t  * len_p)
{
    size_t    buf_len = 0;
    char     *buf_p = NULL;
    char     *p = NULL;
    char     *event_p = NULL;
    char     *temp_p = NULL;
    size_t    length = 0;
    uint8_t   text_len;

    if (!len_p)
    {
        return NULL;
    }
    *len_p = 0;

    buf_len = sizeof(event_type);

    switch (event_type)
    {
        case STE_CAT_EVENT_DOWNLOAD_TYPE_MT_CALL:
        {
            ste_cat_event_mt_call_t * mt_call_p = NULL;
            ste_sim_text_t            sim_text;

            mt_call_p = (ste_cat_event_mt_call_t *)event_data_p;

            length = 1 + 1 + sizeof(mt_call_p->transaction_id); //Tag + length + value for TRANSACTION ID
            length += 1 + 1 + sizeof(mt_call_p->address.ton);  //TON
            length += 1 + 1 + sizeof(mt_call_p->address.npi);  //NPI
            length += 1 + 1 + sizeof(mt_call_p->address.text_coding);  //TEXT CODING
            length += 1 + 1 + sizeof(mt_call_p->address.no_of_characters);  //Number of characters
            length += 1 + 1;  //text, TAG + Length

            sim_text.text_coding = mt_call_p->address.text_coding;
            sim_text.no_of_characters = mt_call_p->address.no_of_characters;
            sim_text.text_p = mt_call_p->address.text_p;
            text_len = sim_get_text_string_length(&sim_text);

            length += text_len;  //value part of dial text
            length += 1 + 1 + mt_call_p->sub_address.len;  //sub address

            event_p = malloc(length + 1);
            if (event_p == NULL)
            {
                return NULL;
            }
            memset(event_p, 0, length + 1);
            temp_p = event_p;

            *temp_p++ = SIM_EVENT_TAG_TRANSACTION_ID;
            *temp_p++ = sizeof(mt_call_p->transaction_id);
             temp_p = sim_enc(temp_p, &mt_call_p->transaction_id, sizeof(mt_call_p->transaction_id));

            *temp_p++ = SIM_EVENT_TAG_TON;
            *temp_p++ = sizeof(mt_call_p->address.ton);
             temp_p = sim_enc(temp_p, &mt_call_p->address.ton, sizeof(mt_call_p->address.ton));

            *temp_p++ = SIM_EVENT_TAG_NPI;
            *temp_p++ = sizeof(mt_call_p->address.npi);
             temp_p = sim_enc(temp_p, &mt_call_p->address.npi, sizeof(mt_call_p->address.npi));

            *temp_p++ = SIM_EVENT_TAG_CODING;
            *temp_p++ = sizeof(mt_call_p->address.text_coding);
             temp_p = sim_enc(temp_p, &mt_call_p->address.text_coding, sizeof(mt_call_p->address.text_coding));

            *temp_p++ = SIM_EVENT_TAG_NO_OF_CHARACTERS;
            *temp_p++ = sizeof(mt_call_p->address.no_of_characters);
             temp_p = sim_enc(temp_p, &mt_call_p->address.no_of_characters, sizeof(mt_call_p->address.no_of_characters));

            *temp_p++ = SIM_EVENT_TAG_TEXT;
            *temp_p++ = text_len;
             temp_p = sim_enc(temp_p, mt_call_p->address.text_p, text_len);

            *temp_p++ = SIM_EVENT_TAG_SUB_ADDRESS;
            *temp_p++ = mt_call_p->sub_address.len;
             temp_p = sim_enc(temp_p, mt_call_p->sub_address.sub_address_p, mt_call_p->sub_address.len);

            if (temp_p == NULL) {
                perror("simd : sim_encode_event_download_data, assert failure");
            }

            assert(temp_p != NULL);

            buf_len += length;
        }
        break;
        default:
        {
            return NULL;
        }
        break;
    }


    buf_p = malloc(buf_len);

    if (!buf_p)
    {
        free(event_p);
        return NULL;
    }

    p = buf_p;
    p = sim_enc(p, &event_type, sizeof(event_type));
    if (length > 0)
    {
        p = sim_enc(p, event_p, length);
    }

    if (buf_len != (size_t)(p - buf_p)) {
        perror("simd : sim_encode_event_download_data, assert failure");
    }

    assert( buf_len == (size_t)(p - buf_p) );

    *len_p = buf_len;
    free(event_p);

    return buf_p;
}

int sim_decode_event_download_data(uint8_t * event_type_p,
                                   void   ** event_data_pp,
                                   uint8_t * buf_p,
                                   size_t    len)
{
    uint8_t          *p = buf_p;
    uint8_t          *p_max = buf_p + len;
    uint8_t           tag;
    uint8_t           length;
    uint8_t           event_type;

    if (event_type_p == NULL || buf_p == NULL)
    {
        return -1;
    }

    //decode the first byte, which is event_type
    //the first byte is event type
    p = (uint8_t*)sim_dec((char*)p, &event_type, sizeof(event_type), (char*)p_max);
    *event_type_p = event_type;

    //based on the event_type, decode the following data
    switch (event_type)
    {
        case STE_CAT_EVENT_DOWNLOAD_TYPE_MT_CALL:
        {
            ste_cat_event_mt_call_t * mt_call_p = NULL;

            mt_call_p = malloc(sizeof(ste_cat_event_mt_call_t));
            if (!mt_call_p)
            {
                return -1;
            }
            memset(mt_call_p, 0, sizeof(ste_cat_event_mt_call_t));
            //now ready to decode transaction id
            p = (uint8_t*)sim_dec((char*)p, &tag, 1, (char*)p_max);
            assert( tag == SIM_EVENT_TAG_TRANSACTION_ID );
            p = (uint8_t*)sim_dec((char*)p, &length, 1, (char*)p_max);
            p = (uint8_t*)sim_dec((char*)p, &mt_call_p->transaction_id, length, (char*)p_max);
            //now ready to decode TON
            p = (uint8_t*)sim_dec((char*)p, &tag, 1, (char*)p_max);
            assert( tag == SIM_EVENT_TAG_TON );
            p = (uint8_t*)sim_dec((char*)p, &length, 1, (char*)p_max);
            p = (uint8_t*)sim_dec((char*)p, &mt_call_p->address.ton, length, (char*)p_max);
            //now ready to decode NPI
            p = (uint8_t*)sim_dec((char*)p, &tag, 1, (char*)p_max);
            assert( tag == SIM_EVENT_TAG_NPI );
            p = (uint8_t*)sim_dec((char*)p, &length, 1, (char*)p_max);
            p = (uint8_t*)sim_dec((char*)p, &mt_call_p->address.npi, length, (char*)p_max);
            //now ready to decode text coding
            p = (uint8_t*)sim_dec((char*)p, &tag, 1, (char*)p_max);
            assert( tag == SIM_EVENT_TAG_CODING );
            p = (uint8_t*)sim_dec((char*)p, &length, 1, (char*)p_max);
            p = (uint8_t*)sim_dec((char*)p, &mt_call_p->address.text_coding, length, (char*)p_max);
            //now ready to decode no of characters
            p = (uint8_t*)sim_dec((char*)p, &tag, 1, (char*)p_max);
            assert( tag == SIM_EVENT_TAG_NO_OF_CHARACTERS );
            p = (uint8_t*)sim_dec((char*)p, &length, 1, (char*)p_max);
            p = (uint8_t*)sim_dec((char*)p, &mt_call_p->address.no_of_characters, length, (char*)p_max);
            //now ready to decode text string
            p = (uint8_t*)sim_dec((char*)p, &tag, 1, (char*)p_max);
            assert( tag == SIM_EVENT_TAG_TEXT );
            p = (uint8_t*)sim_dec((char*)p, &length, 1, (char*)p_max);
            if (length > 0)
            {
                mt_call_p->address.text_p = malloc(length + 1);
                if (mt_call_p->address.text_p == NULL)
                {
                    free(mt_call_p);
                    return -1;
                }
                memset(mt_call_p->address.text_p, 0, length + 1);
                p = (uint8_t*)sim_dec((char*)p, mt_call_p->address.text_p, length, (char*)p_max);

                if (p == NULL) {
                    perror("simd : sim_decode_event_download_data, assert failure");
                }

                assert(p != NULL);
            }
            //now ready to decode sub address
            p = (uint8_t*)sim_dec((char*)p, &tag, 1, (char*)p_max);
            assert( tag == SIM_EVENT_TAG_SUB_ADDRESS );
            p = (uint8_t*)sim_dec((char*)p, &length, 1, (char*)p_max);
            mt_call_p->sub_address.len = length;
            if (length > 0)
            {
                mt_call_p->sub_address.sub_address_p = malloc(length + 1);
                if (mt_call_p->sub_address.sub_address_p == NULL)
                {
                    free(mt_call_p->address.text_p);
                    free(mt_call_p);
                    return -1;
                }
                memset(mt_call_p->sub_address.sub_address_p, 0, length + 1);
                p = (uint8_t*)sim_dec((char*)p, mt_call_p->sub_address.sub_address_p, length, (char*)p_max);

                if (p == NULL) {
                    perror("sim_decode_event_download_data, assert failure");
                }

                assert(p != NULL);
            }

            //DONE
            *event_data_pp = mt_call_p;
        }
        break;
        default:
        {
            return -1;
        }
        break;
    }

    return 0;
}

void sim_free_event_download_data(uint8_t event_type, void * event_data_p)
{
    //based on the event_type, free the related data structure
    if (!event_data_p)
    {
        return;
    }

    switch (event_type)
    {
        case STE_CAT_EVENT_DOWNLOAD_TYPE_MT_CALL:
        {
            //free the members inside the data structure for event MT CALL
            ste_cat_event_mt_call_t * mt_call_p = NULL;

            mt_call_p = (ste_cat_event_mt_call_t *)event_data_p;
            if (mt_call_p->address.text_p)
            {
                free(mt_call_p->address.text_p);
            }
            if (mt_call_p->sub_address.sub_address_p)
            {
                free(mt_call_p->sub_address.sub_address_p);
            }
        }
        break;
        default:
        {
            return;
        }
        break;
    }
}

uint8_t ste_apdu_decode_bcd(uint8_t x)
{
    return ((x & 0x0F) * 10) + ((x >> 4) & 0x0F);
}

uint8_t ste_apdu_encode_bcd(uint8_t x)
{
    if (x == 0) {
        return x;
    } else {
        return (((x % 10) << 4) | (x / 10));
    }
}

int min(int a, int b)
{
  return a<b?a:b;
}

#define SIM_UCS2_FORMAT                    (0x80)
#define SIM_UCS2_81_PREFIX_FORMAT          (0x81)
#define SIM_UCS2_82_PREFIX_FORMAT          (0x82)

/*************************************************************************
 * Function:      sim_get_coding_of_alpha_id
 *
 * Description:   Obtains the data coding scheme from the first byte of
 *                a data object supplied by the SIM in EF ADN format.
 *                Ref. GSM 11.11 Annex B, Coding of Alpha Fields in the SIM
 *                for UCS2.
 *
 * Input Params:  first_byte              First byte of data object
 *
 * Output Params: None
 *
 * Return:        Text coding scheme
 *
 *
 *************************************************************************/
ste_sim_text_coding_t  sim_get_coding_of_alpha_id(const uint8_t first_byte)
{
    ste_sim_text_coding_t alpha_coding;

    switch (first_byte)
    {
        case SIM_UCS2_FORMAT:
            alpha_coding = STE_SIM_UCS2;
            break;

        case SIM_UCS2_81_PREFIX_FORMAT:
            alpha_coding = STE_SIM_UCS2_81Prefix;
            break;

        case SIM_UCS2_82_PREFIX_FORMAT:
            alpha_coding = STE_SIM_UCS2_82Prefix;
            break;

        default:
            alpha_coding = STE_SIM_GSM_Default;
            break;
    }

    return (alpha_coding);
}

/*************************************************************************
 * Function:      sim_decode_alpha_id
 *
 * Description:   At entry alpha_id_p is the start of an unpacked text.
 *                The function removes 0xFF's from the end of text
 *                (which is not in UCS2 format) as EF ADN alpha ID's may
 *                have FF pad bytes.
 *                For UCS2 data, the string will have the 80/81/82 prefix
 *                removed and the data coding scheme will be specified in
 *                ste_sim_text_coding_t.
 *
 * Input Params:  alpha_id_p               original alpha id text
 *                total_alpha_len          original alpha id length
 *
 * Output Params: alpha_coding_p           alpha id coding scheme
 *                dest_p                   buffer to hold the result
 *
 * Return:        Length of actual alpha id
 *
 * Notes: It should have already been checked that the text length > 0.
 *
 *************************************************************************/
uint8_t sim_decode_alpha_id(const uint8_t*               alpha_id_p,
                            const uint8_t                total_alpha_len,
                            uint8_t*               const alpha_coding_p,
                            uint8_t*               const dest_p)
{
    uint8_t actual_alpha_len = total_alpha_len;

    if (!alpha_id_p || !alpha_coding_p || !dest_p || total_alpha_len == 0)
    {
        return 0;
    }

    /* Obtain the coding scheme. This is only possible if actual_alpha_len > 0. */
    if (actual_alpha_len > 0)
    {/* Determine coding type */
        *alpha_coding_p = (uint8_t)sim_get_coding_of_alpha_id(*alpha_id_p);
    }
    else
    { /* Set Coding scheme to invalid */
        *alpha_coding_p = (uint8_t)STE_SIM_CODING_UNKNOWN;
    }

    if (actual_alpha_len != 0)
    { /* Check if the text ends with 0xFF characters only if not USC2 coded: */

        switch (*alpha_id_p)                /* check first byte */
        {
            case SIM_UCS2_FORMAT:
            case SIM_UCS2_81_PREFIX_FORMAT:
            case SIM_UCS2_82_PREFIX_FORMAT:
            {
                /* Copy text from next byte as the UCS2 format byte is not required in the string */
                alpha_id_p++;
                actual_alpha_len--;
            }
            break;

            default:
            {
                uint8_t  index;

                for (index = 0; index < actual_alpha_len; index++)
                {
                    if (*(alpha_id_p + index) == 0xFF) /* 0xFF to indicate end of valid data */
                    {
                        actual_alpha_len = index;
                        break;
                    }
                }
            }
            break;
        } /* end switch */
    } /* end Check if the text ends with 0xFF characters only if not USC2 coded: */

    if (actual_alpha_len > 0)
    { /* Copy text string */
        memcpy(dest_p, alpha_id_p, actual_alpha_len);
    }
    else
    { /* Set Coding scheme to invalid */
        *alpha_coding_p = (uint8_t)STE_SIM_CODING_UNKNOWN;
    }

    return(actual_alpha_len);

}  /* end sim_decode_alpha_id */


/****************************************************************************
 * Function:    sim_convert_gsm_default_char_to_ucs2
 * Description: Converts a character in the GSM default alphabet to UCS2 coded
 * On Error:    Output is always well defined.
 * Note:        This function assumes "Default" means "Not Extended".
 ****************************************************************************/
uint16_t sim_convert_gsm_default_char_to_ucs2 (const uint8_t gsm_char)
{
    uint16_t return_value;

    switch (gsm_char)
    {
        case 0x00:
            return_value = 0x40;  // Commercial at
            break;
        case 0x01:
            return_value = 0xA3;  // Pound sign
            break;
        case 0x02:
            return_value = 0x24;  // Dollar sign
            break;
        case 0x03:
            return_value = 0xA5;  // Yen sign
            break;
        case 0x04:
            return_value = 0xE8;  // Latin small letter e with grave
            break;
        case 0x05:
            return_value = 0xE9;  // Latin small letter e with acute
            break;
        case 0x06:
            return_value = 0xF9;  // Latin small letter u with grave
            break;
        case 0x07:
            return_value = 0xEC;  // Latin small letter i with grave
            break;
        case 0x08:
            return_value = 0xF2;  // Latin small letter o with grave
            break;
        case 0x09:
            return_value = 0xC7;  // Latin capital letter C with cedilla
            break;
        case 0x0B:
            return_value = 0xD8;  // Latin capital letter O with stroke
            break;
        case 0x0C:
            return_value = 0xF8;  // Latin small letter O with stroke
            break;
        case 0x0E:
            return_value = 0xC5;  // Latin capital letter A with ring above
            break;
        case 0x0F:
            return_value = 0xE5;  // Latin small letter A with ring above
            break;
        case 0x10:
            return_value = 0x394; // Greek capital letter delta
            break;
        case 0x11:
            return_value = 0x5F;  // Low line, underscore
            break;
        case 0x12:
            return_value = 0x3A6; // Greek capital letter phi
            break;
        case 0x13:
            return_value = 0x393; // Greek capital letter gamma
            break;
        case 0x14:
            return_value = 0x39B; // Greek capital letter lambda
            break;
        case 0x15:
            return_value = 0x3A9; // Greek capital letter omega
            break;
        case 0x16:
            return_value = 0x3A0; // Greek capital letter pi
            break;
        case 0x17:
            return_value = 0x3A8; // Greek capital letter psi
            break;
        case 0x18:
            return_value = 0x3A3; // Greek capital letter sigma
            break;
        case 0x19:
            return_value = 0x398; // Greek capital letter theta
            break;
        case 0x1A:
            return_value = 0x39E; // Greek capital letter xi
            break;
        case 0x1B:
            return_value = 0x20;  // Shall not be used, but displays as space
            break;
        case 0x1C:
            return_value = 0xC6;  // Latin capital letter ae
            break;
        case 0x1D:
            return_value = 0xE6;  // Latin small letter ae
            break;
        case 0x1E:
            return_value = 0xDF;  // Latin small letter sharp s
            break;
        case 0x1F:
            return_value = 0xC9;  // Latin capital letter e with acute
            break;
        case 0x24:
            return_value = 0xA4;  // Currency sign
            break;
        case 0x40:
            return_value = 0xA1;  // Inverted exclamation mark;
            break;
        case 0x5B:
            return_value = 0xC4;  // Latin capital letter a with diaeresis
            break;
        case 0x5C:
            return_value = 0xD6;  // Latin capital letter o with diaeresis
            break;
        case 0x5D:
            return_value = 0xD1;  // Latin capital letter N with tilde
            break;
        case 0x5E:
            return_value = 0xDC;  // Latin capital letter u with diaeresis
            break;
        case 0x5F:
            return_value = 0xA7;  // Section sign
            break;
        case 0x60:
            return_value = 0xBF;  // Inverted question mark
            break;
        case 0x7B:
            return_value = 0xE4;  // Latin small letter a with diaeresis
            break;
        case 0x7C:
            return_value = 0xF6;  // Latin small letter o with diaeresis
            break;
        case 0x7D:
            return_value = 0xF1;  // Latin small letter N with tilde
            break;
        case 0x7E:
            return_value = 0xFC;  // Latin small letter u with diaeresis
            break;
        case 0x7F:
            return_value = 0xE0;  // Latin small letter a with grave
            break;
        default:
            return_value = (wchar_t) gsm_char;
            break;
    }

    return return_value;
}

/****************************************************************************
 * Function:    sim_convert_ucs2_81prefix_to_ucs2
 * Description: This function converts the packed UCS2 format to "standard"
 *              UCS2. Note that the length is embedded in the text, and is max 255.
 *              The format byte 81H is not expected in the source_p.
 *              But it is also ok if with it
 ****************************************************************************/
int sim_convert_ucs2_81prefix_to_ucs2(uint8_t   * source_p,
                                      uint16_t    max_len,
                                      uint16_t  * actual_len_p,
                                      uint16_t  * dest_p)
{
    uint16_t   result_counter;
    uint16_t   source_counter;
    uint8_t*   data_p;
    uint8_t    nr_of_char;
    uint8_t    base_bit_15to8;
    uint16_t*  result_p = dest_p;

    // Reject Null Pointers
    if ((source_p == NULL) || (actual_len_p == NULL) || (dest_p == NULL))
    {
        perror("sim_convert_ucs2_81prefix_to_ucs2, incorrect input params");
        return -1;
    }

    result_counter = 0;
    source_counter = 0;
    *actual_len_p = 0;

    data_p = source_p; // point to the first byte to check if it is '81H'

    if (*data_p == 0x81)
    {
        data_p++; // Point to the length;
    }
    nr_of_char = *data_p;

    if ((nr_of_char * sizeof(uint16_t)) > max_len )
    {
        perror("sim_convert_ucs2_81prefix_to_ucs2, not enough space to convert");
        return -1;
    }

    data_p++; // Point to the base;
    base_bit_15to8 = *data_p;

    data_p++; // Point to the text itself;

    while ((source_counter < nr_of_char) && (result_counter < max_len - 1)) // "-1" as each target character requires 2 bytes
    {
        if (*data_p < 0x80) // From the GSM Default alphabet
        {
            *result_p = sim_convert_gsm_default_char_to_ucs2(*data_p);
        }
        else // Use the base to compute a value
        {
            *result_p = (base_bit_15to8 << 7) + (*data_p & 0x7F);
        }
        data_p++;
        result_p++;
        source_counter++;
        result_counter = result_counter + 2; // Two bytes for each input char
    }

    *actual_len_p = result_counter;

    return 0;
}

/****************************************************************************
 * Function:    sim_convert_ucs2_82prefix_to_ucs2
 * Description: This function converts the packed UCS2 format to "standard"
 *              UCS2. Note that the length is embedded in the text, and is max 255.
 *              The format byte 82H is not expected in the source_p.
 *              But it is also ok if with it
 ****************************************************************************/
int sim_convert_ucs2_82prefix_to_ucs2(uint8_t   * source_p,
                                      uint16_t    max_len,
                                      uint16_t  * actual_len_p,
                                      uint16_t  * dest_p)
{
    uint16_t  result_counter;
    uint16_t  source_counter;
    uint8_t*  data_p;
    uint8_t   nr_of_char;
    uint16_t  base_value;
    uint16_t* result_p = dest_p;

    // Reject Null Pointers
    if ((source_p == NULL) || (actual_len_p == NULL) || (dest_p == NULL))
    {
        perror("sim_convert_ucs2_82prefix_to_ucs2, incorrect input params");
        return -1;
    }

    result_counter = 0;
    source_counter = 0;
    *actual_len_p = 0;

    data_p = source_p; // point to the first byte to check if it is '82H'

    if (*data_p == 0x82)
    {
        data_p++; // Point to the length;
    }

    nr_of_char = *data_p;

    if (nr_of_char > (max_len * sizeof(uint16_t)))
    {
        perror("sim_convert_ucs2_82prefix_to_ucs2, not enough space to convert");
        return -1;
    }

    data_p++; // Point to the base, first byte;
    base_value = *data_p << 8;

    data_p++; // Point to the base, second byte;
    base_value = base_value + *data_p;

    data_p++; // Point to the text itself;

    // source_counter is really 4 here, but we start at the data instead
    while ((source_counter < nr_of_char) && (result_counter < max_len - 1)) // "-1" as each target character requires 2 bytes
    {
        if (*data_p < 0x80) // From the GSM Default alphabet
        {
            *result_p = sim_convert_gsm_default_char_to_ucs2(*data_p);
        }
        else // Use the base to compute a value
        {
            *result_p = base_value + ((*data_p) & 0x7F);
        }
        data_p++;
        result_p++;
        source_counter++;
        result_counter = result_counter + 2; // Two bytes for each input char
    }

    *actual_len_p = result_counter;

    return 0;
}

