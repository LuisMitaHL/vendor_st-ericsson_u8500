/*                               -*- Mode: C -*-
 * Copyright (C) 2010, ST-Ericsson
 *
 * File name       : sim.c
 * Description     : Functionality for connecting to CAT and UICC.
 *
 * Author          : Jessica Nilsson <jessica.j.nilsson@stericsson.com>
 *
 */

#include "sim.h"
#include "sim_unused.h"
#include "cat_internal.h"
#include "uicc_internal.h"
#include "sim_internal.h"
#include "event_stream.h"
#include "func_trace.h"
#include "catd_modem_types.h"

#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/un.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdlib.h>


#ifndef HAVE_ANDROID_OS
#ifndef HOST_FS_PATH
#define HOST_FS_PATH ""
#endif
static const char       socket_name[] = HOST_FS_PATH "/tmp/catd_a";
#else
static const char       socket_name[] = "/dev/socket/catd_a";
#endif


/**
 * @brief Internal to external state mapping structure.
 */
typedef struct {
    const char                   *state_name;         /**< Name of the internal state */
    sim_state_t                  sim_state;           /**< External state delivered to client */
} sim_external_state_t;

/**
 * @brief State table to deliver state events to the clients.
 */
static const sim_external_state_t uicc_external_state_table[] = {
        { "unknown", SIM_STATE_UNKNOWN  },
        { "init", SIM_STATE_NOT_READY  },
        { "pin_needed", SIM_STATE_PIN_NEEDED },
        { "puk_needed", SIM_STATE_PUK_NEEDED },
        { "pin2_needed", SIM_STATE_PIN2_NEEDED },
        { "puk2_needed", SIM_STATE_PUK2_NEEDED },
        { "ready", SIM_STATE_READY  },
        { "no_card", SIM_STATE_SIM_ABSENT },
        { "closed", SIM_STATE_PERMANENTLY_BLOCKED },
        { "card_invalid", SIM_STATE_PERMANENTLY_BLOCKED},
        { "card_sim_locked", SIM_STATE_REJECTED_CARD_SIM_LOCK},
        { "card_sim_consecutive_6f00_lock", SIM_STATE_REJECTED_CARD_CONSECUTIVE_6F00},
        { "card_disconnected", SIM_STATE_DISCONNECTED_CARD},
};

#define STE_SIM_I_CMD_DIE       (0xdead)
#define SIM_READER_INPUT_BUFFER_SIZE (2048)
#define SIM_READER_PIPE_BUFFER_SIZE (64)
#define MAX_ENTRIES_IN_STATE_MAPPING_TABLE (sizeof(uicc_external_state_table) / sizeof(sim_external_state_t))

typedef struct {
    int                     input_pipe; /* set to -1 when thread dies */
    int                     fd;
    ste_sim_closure_t       closure;
    uintptr_t               client_tag;
    ste_sim_t*              sim;                /* Always 0 in threaded mode! */
    int                     is_st;
} ste_sim_thread_data_t;


static void ste_sim_i_handle_command_th(uint16_t cmd, uintptr_t client_tag,
                                        const char *bug,uint16_t len,
                                        ste_sim_thread_data_t * thread_data);
static void ste_sim_i_kill_thread(ste_sim_t * sim, uintptr_t client_tag);

static ssize_t ste_sim_i_reader_pipe_parse_func_th(char *buf, char *buf_max,
                                                   void *ud);
static int ste_sim_es_i_read_and_parse_th(ste_es_t * es);
static void            *ste_sim_i_reader_func_th(void *vdata);
static ssize_t ste_sim_i_reader_parse_func_th(char *buf, char *buf_max,
                                              void *ud);

static int FindStateFromStateTable(const char *internal_state_name)
{
    uint32_t Count = 0;
    for (Count = 0; Count < MAX_ENTRIES_IN_STATE_MAPPING_TABLE; Count++) {
        if (strcmp(internal_state_name, uicc_external_state_table[Count].state_name) == 0) {
            return uicc_external_state_table[Count].sim_state;
      }
    }
    return 0;
}


static int ste_sim_i_connect(const char *path)
{
    struct sockaddr_un      addr;
    int                     i;
    int                     fd;

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    ste_sim_i_safe_copy(addr.sun_path, path, sizeof(addr.sun_path));

    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("failed to create socket");
        return CLOSED_FD;
    }

    i = connect(fd, (struct sockaddr *) &addr, sizeof(addr));
    if (i < 0) {
        perror("failed to connect");
        close(fd);
        return CLOSED_FD;
    }

    i = fcntl(fd, F_SETFL, O_NONBLOCK);
    if (i < 0) {
        perror("failed to set non-blocking");
        close(fd);
        return CLOSED_FD;
    }

    return fd;
}



static void            *ste_sim_i_reader_func_th(void *vdata)
{
    ste_sim_thread_data_t  *thread_data = (ste_sim_thread_data_t *) vdata;
    setThreadName("simi_rd");
    //    simd_log_f("#\tsimi_rd : New Thread 0x%08lx\n",(unsigned long)pthread_self());
    struct timeval          tv;
    ste_es_t               *es_s = 0;
    ste_es_t               *es_p = 0;
    fd_set                  rset;
    int                     max_fd;
    int                     i;
    uintptr_t               client_tag;
    int                     ret_val = 0;
    ste_es_parser_closure_t pc;
    int                     hangup = 0;

    client_tag = thread_data->client_tag;

    /* Set up reading from the control pipe */
    pc.func = ste_sim_i_reader_pipe_parse_func_th;
    pc.user_data = vdata;
    es_p =
        ste_es_new_normal(thread_data->input_pipe, SIM_READER_PIPE_BUFFER_SIZE,
                          &pc);
    if (!es_p) {
        // FIXME:  Call the callback function with cause.

        if (thread_data->input_pipe >= 0)
            close(thread_data->input_pipe);

        if (thread_data->fd >= 0)
            close(thread_data->fd);

        int *pRet = malloc( sizeof *pRet );
        if ( pRet ) *pRet = -1;
        pthread_exit(pRet);
    }

    /* Set up reading from the socket */
    pc.func = ste_sim_i_reader_parse_func_th;
    pc.user_data = vdata;
    es_s =
        ste_es_new_normal(thread_data->fd, SIM_READER_INPUT_BUFFER_SIZE, &pc);
    if (!es_s) {
        ste_es_delete(es_p);
        // FIXME:  Call the callback function with cause.

        if (thread_data->input_pipe >= 0)
            close(thread_data->input_pipe);

        if (thread_data->fd >= 0)
            close(thread_data->fd);

        int *pRet = malloc( sizeof *pRet );
        if ( pRet ) *pRet = -1;
        pthread_exit(pRet);
    }

    max_fd = 1 + (thread_data->fd > thread_data->input_pipe
                  ? thread_data->fd : thread_data->input_pipe);

    /* Call the call back to indicate the connection is up and running */
    thread_data->closure.func(STE_SIM_CAUSE_CONNECT,    /* Cause */
                              thread_data->client_tag,
                              0,        /* No payload */
                              thread_data->closure.user_data);

    for (;;) {
        FD_ZERO(&rset);
        FD_SET(thread_data->fd, &rset);
        FD_SET(thread_data->input_pipe, &rset);

        tv.tv_sec = 20;
        tv.tv_usec = 0;

        i = select(max_fd, &rset, 0, 0, &tv);


        if (i == 0) {           /* timer expired */
        } else if (i < 0) {     /* Error */
            int                     e = errno;
            int                     hangup = 0;

            switch (e)
            {
                case EBADF:
                case EINTR:
                case ENOMEM:
                        hangup = 1;
                        break;
                case EINVAL: // Ignore
                default:
                        hangup = 0;
            }
            break;
        } else {

            if (FD_ISSET(ste_es_fd(es_p), &rset)) {
                i = ste_sim_es_i_read_and_parse_th(es_p);
                if (i != STE_SIM_SUCCESS) {
                    hangup = 1;
                    break;
                }

                // Check if disconnect occured
                if (thread_data->input_pipe < 0) {
                    break;
                }
            }

            if (FD_ISSET(ste_es_fd(es_s), &rset)) {
                i = ste_sim_es_i_read_and_parse_th(es_s);
                if (i != STE_SIM_SUCCESS) {
                    hangup = 1;
                    break;
                }
            }

        }
    }

    if (hangup) {
        // Issue hangup callback
        thread_data->closure.func(STE_SIM_CAUSE_HANGUP,
                                  0,
                                  0,
                                  thread_data->closure.user_data);
    }

    if (thread_data->input_pipe >= 0)
        close(thread_data->input_pipe);
    if (thread_data->fd >= 0)
        close(thread_data->fd);

    if (es_p) {
        // Thread data is released by the closure function through this call
        ste_es_delete(es_p);

        // Remove the thread data from the es_s structure to ensure that
        // the thread data is not released twice by the closure function
        ste_es_set_ud(es_s, 0);
    }

    if (es_s)
        ste_es_delete(es_s);

    int *pRet = malloc( sizeof(int) );
    if ( pRet ) *pRet = ret_val;
        pthread_exit(pRet);

    return 0;
}




static ssize_t ste_sim_i_reader_pipe_parse_func_th(char *buf,
                                                   char *buf_max, void *ud)
{
    const char             *p = buf;
    uint16_t                cmd;
    uintptr_t               client_tag;
    ssize_t                 bytes_consumed = 0;
    ste_sim_thread_data_t  *thread_data = (ste_sim_thread_data_t *) ud;

    if (!buf && !buf_max) {
        /* Destructor */
        //simd_log_f("ste_sim_i_reader_pipe_parse_func_th dtor - freeing thread_data at %p\n", ud);
        free(thread_data);
    } else if (!buf_max) {
        /* marks accept sockets, which we are not. */
        abort();
    } else {

        do {
            // Decode the command.
            p = sim_dec(p, &cmd, sizeof(cmd), buf_max);
            if (!p)
                break;

            p = sim_dec(p, &client_tag, sizeof(client_tag), buf_max);
            if (!p)
                break;

            bytes_consumed += (sizeof(cmd) + sizeof(client_tag));

            if (cmd == STE_SIM_I_CMD_DIE) {
                close(thread_data->input_pipe);
                thread_data->input_pipe = CLOSED_FD;
            } else {
                // FIXME: Handle garbage
            }
        } while (0);
    }

    return bytes_consumed;
}

static int ste_sim_es_i_read_and_parse_th(ste_es_t * es)
{
    ssize_t                 n;
    n = ste_es_read(es);
    if (n > 0) {
        n = ste_es_parse(es);
    }
    if (ste_es_state(es) != STE_ES_STATE_OPEN) {
        return STE_SIM_ERROR_EVENT_STREAM;
    }
    return 0;
}

static ssize_t ste_sim_i_reader_parse_func_th(char *buf,
                                              char *buf_max, void *ud)
{
    const char             *p = buf;
    uint16_t                len;
    uint16_t                cmd;
    uintptr_t               client_tag;
    ssize_t                 bytes_consumed = 0;
    ste_sim_thread_data_t  *thread_data = (ste_sim_thread_data_t *) ud;

    if (!buf && !buf_max) {
        /* Destructor */
        //simd_log_f("ste_sim_i_reader_parse_func_th dtor - NOT!!! freeing thread_data at %p\n", ud);
        /* free(thread_data); Not needed here! Done by the pipe parser func */
        if (thread_data && thread_data->is_st) {
            free(thread_data);
        }
    } else if (!buf_max) {
        /* marks accept sockets, which we are not. */
        abort();
    } else {

        do {
            p = sim_dec(p, &len, sizeof(len), buf_max);
            if (!p)
                break;

            if (buf_max - p < len)      /* FIXME: +1?? Is this check necessary? (Done by sim_dec)  */
                break;

            p = sim_dec(p, &cmd, sizeof(cmd), buf_max);
            if (!p)
                break;

            p = sim_dec(p, &client_tag, sizeof(client_tag), buf_max);
            if (!p)
                break;

            len -= sizeof(cmd) + sizeof(client_tag);


            ste_sim_i_handle_command_th(cmd, client_tag, p, len, thread_data);

            p += len;

            bytes_consumed += sizeof(len) + sizeof(cmd) + sizeof(client_tag) + len;
        } while (0);
    }

    return bytes_consumed;
}

static void ste_sim_i_kill_thread(ste_sim_t * sim, uintptr_t client_tag)
{
    uint16_t                cmd = STE_SIM_I_CMD_DIE;
    char                    buf[sizeof(cmd) + sizeof(client_tag)];
    char                    *p = buf;
    int                     i;
    int                     *ret_val;

    p = sim_enc (p, &cmd, sizeof(cmd));
    p = sim_enc (p, &client_tag, sizeof(client_tag));

    if (sizeof(buf) != (size_t)(p-buf)) {
        printf("sim : ste_sim_i_kill_thread, assert failure");
    }

    assert( sizeof(buf) == (size_t)(p-buf) );

    write(sim->thread_pipe, buf, sizeof(buf));

    // Join the reader thread
    i = pthread_join(sim->rdr_tid, (void**)&ret_val);
    free( ret_val );

    sim->rdr_tid = 0;

    if (i != 0) {
        //        simd_log_f("#\tste_sim_i_kill_thread: pthread_join failed with error code %d\n", i);
    }

    // Issue disconnect callback
    sim->closure.func(STE_SIM_CAUSE_DISCONNECT,
                      client_tag,
                      0,
                      sim->closure.user_data);
}




static void
ste_sim_sync_func(int cause,
                  uintptr_t UNUSED(client_tag),
                  void *data,
                  void *user_data)
{
    ste_sim_thread_data_t * thread_data;

    thread_data = (ste_sim_thread_data_t *)user_data;

    switch ( thread_data->sim->sync_data.expected_response_id ) {
    default:
        abort();
        break;

#if 0
    case STE_SIM_RSP_READ_BINARY:
        // Unpack the data and set
        // FIXME: Do the actual unpacking here...
        abort(); // Just to remember to do it...
        break;
#endif

    case STE_SIM_RSP_PING:
        // Just set the rv to 0.
        // .. There is no payload to handle for the ping.
        thread_data->sim->sync_data.rv = 0;
        break;

    case STE_UICC_RSP_READ_SIM_FILE_RECORD:
    {
        ste_uicc_sim_file_read_record_response_t * result_p = (ste_uicc_sim_file_read_record_response_t *)(thread_data->sim->sync_data.vdata);

        thread_data->sim->sync_data.rv = 0;

        memset(result_p, 0, sizeof(*result_p));

        if (cause == STE_UICC_CAUSE_NOT_READY) {
            result_p->uicc_status_code = STE_UICC_STATUS_CODE_NOT_READY;
            result_p->uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
        } else if (cause == STE_UICC_CAUSE_REQ_READ_SIM_FILE_RECORD) {
            memcpy(result_p, data, sizeof(ste_uicc_sim_file_read_record_response_t));
            if (result_p->length > 0) {
                result_p->data = malloc(result_p->length + 1);
                if (!result_p->data){
                    return;
                }
                memset(result_p->data, 0, result_p->length + 1);
                memcpy(result_p->data, ((ste_uicc_sim_file_read_record_response_t *)data)->data, result_p->length);
            }
        }
    }
    break;
    case STE_UICC_RSP_READ_SIM_FILE_BINARY:
    {
        ste_uicc_sim_file_read_binary_response_t * result_p = (ste_uicc_sim_file_read_binary_response_t *)(thread_data->sim->sync_data.vdata);

        thread_data->sim->sync_data.rv = 0;

        memset(result_p, 0, sizeof(*result_p));

        if (cause == STE_UICC_CAUSE_NOT_READY) {
            result_p->uicc_status_code = STE_UICC_STATUS_CODE_NOT_READY;
            result_p->uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
        } else if (cause == STE_UICC_CAUSE_REQ_READ_SIM_FILE_BINARY) {
            memcpy(result_p, data, sizeof(ste_uicc_sim_file_read_binary_response_t));
            if (result_p->length > 0) {
                result_p->data = malloc(result_p->length + 1);
                if (!result_p->data){
                    return;
                }
                memset(result_p->data, 0, result_p->length + 1);
                memcpy(result_p->data, ((ste_uicc_sim_file_read_binary_response_t *)data)->data, result_p->length);
            }
        }
    }
    break;
    case STE_UICC_RSP_UPDATE_SIM_FILE_RECORD:
    {
        ste_uicc_update_sim_file_record_response_t  * result_p = thread_data->sim->sync_data.vdata;

        thread_data->sim->sync_data.rv = 0;

        memset(result_p, 0, sizeof(*result_p));

        if (cause == STE_UICC_CAUSE_NOT_READY) {
            result_p->uicc_status_code = STE_UICC_STATUS_CODE_NOT_READY;
            result_p->uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
        } else if (cause == STE_UICC_CAUSE_REQ_UPDATE_SIM_FILE_RECORD) {
            memcpy(result_p, data, sizeof(ste_uicc_update_sim_file_record_response_t));
        }

        break;
    }
    case STE_UICC_RSP_UPDATE_SIM_FILE_BINARY:
    {
        ste_uicc_update_sim_file_binary_response_t  * result_p = thread_data->sim->sync_data.vdata;

        thread_data->sim->sync_data.rv = 0;

        memset(result_p, 0, sizeof(*result_p));

        if (cause == STE_UICC_CAUSE_NOT_READY) {
            result_p->uicc_status_code = STE_UICC_STATUS_CODE_NOT_READY;
            result_p->uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
        } else if (cause == STE_UICC_CAUSE_REQ_UPDATE_SIM_FILE_BINARY) {
            memcpy(result_p, data, sizeof(ste_uicc_update_sim_file_binary_response_t));
        }

        break;
    }
    case STE_UICC_RSP_GET_SIM_STATE:
    {
        ste_uicc_get_sim_state_response_t  * result_p = thread_data->sim->sync_data.vdata;

        thread_data->sim->sync_data.rv = 0;

        memset(result_p, 0, sizeof(*result_p));

        if (cause == STE_UICC_CAUSE_NOT_READY) {
            result_p->error_cause = -1;
            result_p->state = SIM_STATE_UNKNOWN;
        } else if (cause == STE_UICC_CAUSE_REQ_GET_SIM_STATE) {
            memcpy(result_p, data, sizeof(ste_uicc_get_sim_state_response_t));
        }

        break;
    }
    case STE_UICC_RSP_SIM_FILE_GET_FORMAT:
    {
        ste_uicc_sim_file_get_format_response_t  * result_p = thread_data->sim->sync_data.vdata;

        thread_data->sim->sync_data.rv = 0;

        memset(result_p, 0, sizeof(*result_p));

        if (cause == STE_UICC_CAUSE_NOT_READY) {
            result_p->uicc_status_code = STE_UICC_STATUS_CODE_NOT_READY;
            result_p->uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
        } else if (cause == STE_UICC_CAUSE_REQ_SIM_FILE_GET_FORMAT) {
            memcpy(result_p, data, sizeof(ste_uicc_sim_file_get_format_response_t));
        }

        break;
    }
    case STE_UICC_RSP_GET_FILE_INFORMATION:
    {
        ste_uicc_get_file_information_response_t * result_p = (ste_uicc_get_file_information_response_t *)(thread_data->sim->sync_data.vdata);

        thread_data->sim->sync_data.rv = 0;

        memset(result_p, 0, sizeof(*result_p));

        if (cause == STE_UICC_CAUSE_NOT_READY) {
            result_p->uicc_status_code = STE_UICC_STATUS_CODE_NOT_READY;
            result_p->uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
        } else if (cause == STE_UICC_CAUSE_REQ_GET_FILE_INFORMATION) {
            memcpy(result_p, data, sizeof(ste_uicc_get_file_information_response_t));
            if (result_p->length > 0) {
                result_p->fcp = malloc(result_p->length + 1);
                if (!result_p->fcp){
                    return;
                }
                memset(result_p->fcp, 0, result_p->length + 1);
                memcpy(result_p->fcp, ((ste_uicc_get_file_information_response_t *)data)->fcp, result_p->length);
            }
        }
    }
    break;
    case STE_UICC_RSP_UPDATE_SMSC:
    {
        ste_uicc_sim_smsc_set_active_response_t  * result_p = thread_data->sim->sync_data.vdata;

        thread_data->sim->sync_data.rv = 0;

        memset(result_p, 0, sizeof(*result_p));

        if (cause == STE_UICC_CAUSE_NOT_READY) {
            result_p->uicc_status_code = STE_UICC_STATUS_CODE_NOT_READY;
            result_p->uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
        } else if (cause == STE_UICC_CAUSE_REQ_SMSC_SET_ACTIVE) {
            memcpy(result_p, data, sizeof(ste_uicc_sim_smsc_set_active_response_t));
        }
    }
    break;
    case STE_UICC_RSP_READ_SMSC:
    {
        ste_uicc_sim_smsc_get_active_response_t * result_p = (ste_uicc_sim_smsc_get_active_response_t *)(thread_data->sim->sync_data.vdata);
        ste_uicc_sim_smsc_get_active_response_t * data_p = (ste_uicc_sim_smsc_get_active_response_t *) data;

        thread_data->sim->sync_data.rv = 0;

        memset(result_p, 0, sizeof(*result_p));

        if (cause == STE_UICC_CAUSE_NOT_READY) {
            result_p->uicc_status_code = STE_UICC_STATUS_CODE_NOT_READY;
            result_p->uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
        } else if (cause == STE_UICC_CAUSE_REQ_SMSC_GET_ACTIVE) {
            memcpy(thread_data->sim->sync_data.vdata, data, sizeof(ste_uicc_sim_smsc_get_active_response_t));

            if (data_p->smsc.num_text.text_p != NULL) {
                size_t len = strlen(data_p->smsc.num_text.text_p);
                result_p->smsc.num_text.text_p = malloc(len + sizeof(char));
                if (result_p->smsc.num_text.text_p == NULL) {
                    return;
                }
                strcpy(result_p->smsc.num_text.text_p, data_p->smsc.num_text.text_p);
            } else {
                data_p->smsc.num_text.text_p = NULL;
            }
        }
    }
    break;

    case STE_UICC_RSP_GET_SERVICE_TABLE:
    {
        ste_uicc_get_service_table_response_t  * result_p = thread_data->sim->sync_data.vdata;

        thread_data->sim->sync_data.rv = 0;

        memset(result_p, 0, sizeof(*result_p));

        if (cause == STE_UICC_CAUSE_NOT_READY) {
            result_p->uicc_status_code = STE_UICC_STATUS_CODE_NOT_READY;
            result_p->uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
        } else if (cause == STE_UICC_CAUSE_REQ_GET_SERVICE_TABLE) {
            memcpy(result_p, data, sizeof(ste_uicc_get_service_table_response_t));
        }

        break;
    }

    case STE_UICC_RSP_GET_SERVICE_AVAILABILITY:
    {
        ste_uicc_get_service_availability_response_t  * result_p = thread_data->sim->sync_data.vdata;

        thread_data->sim->sync_data.rv = 0;

        memset(result_p, 0, sizeof(*result_p));

        if (cause == STE_UICC_CAUSE_NOT_READY) {
            result_p->uicc_status_code = STE_UICC_STATUS_CODE_NOT_READY;
            result_p->uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
        } else if (cause == STE_UICC_CAUSE_REQ_GET_SERVICE_AVAILABILITY) {
            memcpy(result_p, data, sizeof(ste_uicc_get_service_availability_response_t));
        }
        break;
    }
    case STE_UICC_RSP_READ_FDN:
    {
      ste_uicc_sim_fdn_response_t *d = (ste_uicc_sim_fdn_response_t *)(thread_data->sim->sync_data.vdata);
      ste_uicc_sim_fdn_record_t *f, *df;
      size_t f_len;

      thread_data->sim->sync_data.rv = 0;

      memset(d, 0, sizeof(*d));

      if (cause == STE_UICC_CAUSE_NOT_READY) {
          d->uicc_status_code = STE_UICC_STATUS_CODE_NOT_READY;
          d->uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
      } else if (cause == STE_UICC_CAUSE_REQ_SIM_FILE_READ_FDN) {
          memcpy(d, data, sizeof(*d));
          f_len = d->number_of_records;
          f = malloc(sizeof(*f)*f_len);
          df = d->fdn_record_p;
          d->fdn_record_p = f;
          if (f) {
            int i;

            memcpy(f, df, sizeof(*f)*f_len);
            for (i = 0; i< (int)f_len; i++) {
              size_t dc_len = (size_t)df[i].dial_string.no_of_characters;
              uint8_t *c, *dc;
              size_t da_len = (size_t)df[i].alpha_string.no_of_characters;
              uint8_t *a, *da;

              if (dc_len == 0) {
                continue;
              }
              dc = df[i].dial_string.text_p;
              c = malloc(dc_len);
              f[i].dial_string.text_p = c;
              if (!dc) {
                continue;
              }
              memcpy(c, dc, dc_len);

              if (da_len == 0) {
                continue;
              }
              da = df[i].alpha_string.text_p;
              a = malloc(da_len);
              f[i].alpha_string.text_p = a;
              if (!da) {
                continue;
              }
              memcpy(a, da, da_len);
            }
          }
      }
    }
    break;
    case STE_UICC_RSP_READ_ECC:
    {
      // Result_p is allocated by client in the sync call.
      // Data includes some malloced data that needs to be
      ste_uicc_sim_ecc_response_t *d = (ste_uicc_sim_ecc_response_t *)(thread_data->sim->sync_data.vdata);
      ste_uicc_sim_ecc_number_t *e;
      ste_uicc_sim_ecc_number_t *de;
      size_t e_len;

      thread_data->sim->sync_data.rv = 0;

      memset(d, 0, sizeof(*d));

      if (cause == STE_UICC_CAUSE_NOT_READY) {
          d->uicc_status_code = STE_UICC_STATUS_CODE_NOT_READY;
          d->uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
      } else if (cause == STE_UICC_CAUSE_REQ_SIM_FILE_READ_ECC) {
          memcpy(d, data, sizeof(*d));
          e_len = d->number_of_records;
          e = malloc(sizeof(*e)*e_len);
          de = d->ecc_number_p;
          d->ecc_number_p = e;

          if (e) {
            int i;

            memcpy(e, de, sizeof(*e)*e_len);
            for (i=0;i<(int)e_len;i++) {
              size_t da_len = de[i].length;
              uint8_t *a, *da;

              if (da_len == 0) {
                continue;
              }
              da = de[i].alpha;
              a = malloc(da_len);
              e[i].alpha = a;
              if (!da) {
                continue;
              }
              memcpy(a, da, da_len);
            }
          }
      }
    }
    break;
  }  // switch ( cause )
}

static void ste_sim_handle_read_generic_response(ste_sim_closure_t closure,
                                                 uintptr_t client_tag,
                                                 ste_uicc_sim_file_read_generic_response_t * generic_data_p)
{
    int    req_cause;
    void * result_p;
    int dynamic_res_alloc = 0;

    if (!generic_data_p) {
        return;
    }
    switch (generic_data_p->file_id)
    {
        case SIM_EF_IMSI:
        {
            ste_uicc_sim_file_read_imsi_response_t * imsi_result_p;

            imsi_result_p = malloc(sizeof(*imsi_result_p));
            if (!imsi_result_p) {
                return;
            }
            memset(imsi_result_p, 0, sizeof(*imsi_result_p));
            req_cause = STE_UICC_CAUSE_REQ_SIM_FILE_READ_IMSI;
            //copy the result from generic data to imsi result
            sim_create_imsi_response(imsi_result_p, generic_data_p->data_p, generic_data_p->data_len);
            imsi_result_p->uicc_status_code = generic_data_p->uicc_status_code;
            imsi_result_p->uicc_status_code_fail_details = generic_data_p->uicc_status_code_fail_details;
            imsi_result_p->status_word.sw1 = generic_data_p->status_word.sw1;
            imsi_result_p->status_word.sw2 = generic_data_p->status_word.sw2;
            result_p = imsi_result_p;
            dynamic_res_alloc = 1;
        }
        break;
        default:
        {
            req_cause = STE_UICC_CAUSE_REQ_SIM_FILE_READ_GENERIC;
            result_p = generic_data_p;
        }
    }

    closure.func(req_cause, client_tag, result_p, closure.user_data);

    if (dynamic_res_alloc) {
      free(result_p);
    }
}

static void ste_sim_subscriber_number_cleanup(ste_sim_subscriber_number_record_t *record_data_p,
                                              uint16_t number_of_records)
{
    ste_sim_subscriber_number_record_t *rec_data_p = record_data_p;
    int i;

    for(i=0; i < number_of_records; i++){

        free(rec_data_p->alpha_p);
        free(rec_data_p->number_p);
        rec_data_p++;
    }

    free(record_data_p);

}


static void ste_sim_i_handle_command_th(uint16_t cmd, uintptr_t client_tag,
                                        const char *buf, uint16_t len,
                                        ste_sim_thread_data_t * thread_data)
{
    ste_sim_t*              sim;
    ste_sim_closure_t       closure;            /* Used closure. */

    closure = thread_data->closure;
    sim = thread_data->sim;

    // Synchronous mode checking.
    // .. only relevant in single threaded mode.
    if ( sim && sim->is_st ) {
        if ( sim->sync_data.is_sync ) {

            // We are in synchronous mode.
            // For all IND we use the normal closure.
            // For all RSP except the sim->sync_data.expected_response_id
            // we use the normal closure.
            // For RSP equal to sim->sync_data.expected_response_id
            // we use our closure function with thread data as user data.

            if (client_tag == thread_data->sim->sync_data.client_tag &&
                (cmd == thread_data->sim->sync_data.expected_response_id ||
                 cmd == STE_UICC_RSP_NOT_READY)) {
                closure.func = ste_sim_sync_func;
                closure.user_data = thread_data;

                // Set flag that response has been caught!
                sim->sync_data.done = 1;
            }
        }
    }

    switch (cmd) {
    case STE_SIM_RSP_PING:
        closure.func(STE_SIM_CAUSE_PING,   /* Cause */
                                  client_tag,
                                  0,    /* No payload */
                                  closure.user_data);
        break;

    case STE_SIM_RSP_SHUTDOWN:
        closure.func(STE_SIM_CAUSE_SHUTDOWN,       /* Cause */
                                  client_tag,
                                  0,    /* No payload */
                                  closure.user_data);
        break;

    case STE_SIM_RSP_STARTUP:
        closure.func(STE_SIM_CAUSE_STARTUP,       /* Cause */
                                  client_tag,
                                  0,    /* No payload */
                                  closure.user_data);
        break;

    case STE_SIM_RSP_SET_SIM_DEBUG_LEVEL:
        closure.func(STE_SIM_CAUSE_SIM_LOG_LEVEL,       /* Cause */
                                  client_tag,
                                  0,    /* No payload */
                                  closure.user_data);
        break;

    case STE_SIM_RSP_SET_MODEM_DEBUG_LEVEL:
        closure.func(STE_SIM_CAUSE_MODEM_LOG_LEVEL,       /* Cause */
                                  client_tag,
                                  0,    /* No payload */
                                  closure.user_data);
        break;

    case STE_CAT_RSP_REGISTER:
        {
            int reg_status;

            reg_status = -1;
            ste_sim_decode_status_response(&reg_status, buf, len);

            closure.func(STE_CAT_CAUSE_REGISTER,       /* Cause */
                         client_tag,
                         &reg_status,
                         closure.user_data);
        }
        break;

    case STE_CAT_RSP_DEREGISTER:
        {
            int reg_status;

            reg_status = -1;
            ste_sim_decode_status_response(&reg_status, buf, len);

            closure.func(STE_CAT_CAUSE_DEREGISTER,       /* Cause */
                         client_tag,
                         &reg_status,
                         closure.user_data);
        }
        break;

    case STE_CAT_RSP_EC:
        {
            ste_cat_ec_response_t ec_rsp;

            ec_rsp.ec_status = STE_CAT_RESPONSE_UNKNOWN;
            ste_sim_decode_cat_response(&ec_rsp, buf, len);

            closure.func(STE_CAT_CAUSE_EC, /* Cause */
                         client_tag,
                         &ec_rsp,
                         closure.user_data);
            if (ec_rsp.apdu.buf) {
                free(ec_rsp.apdu.buf);
            }
        }
        break;
    case STE_CAT_RSP_TR:
        {
            ste_cat_tr_response_t  data;
            const char             *p = buf;
            const char             *p_max = buf + len;
            ste_uicc_status_word_t status_word;
            int                    status;

            // split msg_data into separate parameters
            p = sim_dec(p, &status, sizeof(status), p_max);
            p = sim_dec(p, &status_word.sw1, sizeof(status_word.sw1), p_max);
            p = sim_dec(p, &status_word.sw2, sizeof(status_word.sw2), p_max);

            if (!p) {
                break;
            }

            data.status = status;
            data.status_word.sw1 = status_word.sw1;
            data.status_word.sw2 = status_word.sw2;

            closure.func(STE_CAT_CAUSE_TR, /* Cause */
                         client_tag,
                         &data,
                         closure.user_data);
        }
        break;

    case STE_CAT_RSP_SET_TERMINAL_PROFILE:
        {
            ste_cat_set_terminal_profile_response_t set_rsp;

            set_rsp.status = -1;
            ste_sim_decode_status_response(&set_rsp.status, buf, len);

            closure.func(STE_CAT_CAUSE_SET_TERMINAL_PROFILE, /* Cause */
                         client_tag,
                         &set_rsp,
                         closure.user_data);
        }
        break;
    case STE_CAT_RSP_GET_TERMINAL_PROFILE:
        {
            ste_cat_get_terminal_profile_response_t get_rsp;
            get_rsp.status = -1;
            get_rsp.profile_len = 0;
            get_rsp.profile_p = NULL;

            if ((buf != NULL && *buf == '\0' && len == 0) || buf == NULL) {
                printf("#\tCATD returned NULL profile.\n");
            }
            else {
                get_rsp.profile_p = ste_sim_decode_length_data((size_t *)&(get_rsp.profile_len), (char*)buf, len);
            }

            if (get_rsp.profile_p) {
                get_rsp.status = 0;
            }
            closure.func(STE_CAT_CAUSE_GET_TERMINAL_PROFILE, /* Cause */
                         client_tag,
                         &get_rsp,
                         closure.user_data);

            if (get_rsp.profile_p) {
                free(get_rsp.profile_p);
            }
        }
        break;
    case STE_CAT_RSP_CALL_CONTROL:
        {
            ste_cat_call_control_response_t cc_rsp;
            int  ec_status;

            cc_rsp.cc_result = STE_CAT_CC_ALLOWED_NO_MODIFICATION;  //by default, we allow the cc
            ste_sim_decode_cc_response(&ec_status, &cc_rsp, buf, len);

            //check the ec_status first
            //if the EC is not executed successfully, should we allow or not? we set for NOT_ALLOWED for now
            if (ec_status != STE_CAT_RESPONSE_OK)
            {
                if ( ec_status == STE_CAT_RESPONSE_TEMP_PROBLEM_IND ) {
                  /* Indicate temporary problem so that the SIM may resend the request */
                  cc_rsp.cc_result = STE_CAT_CC_NOT_ALLOWED_TEMP_PROBLEM;
                } else {
                cc_rsp.cc_result = STE_CAT_CC_NOT_ALLOWED;
                }
            }

            closure.func(STE_CAT_CAUSE_SIM_EC_CALL_CONTROL, /* Cause */
                         client_tag,
                         &cc_rsp,
                         closure.user_data);

            sim_free_call_control_response_data(&cc_rsp);
        }
        break;

    case STE_CAT_RSP_SMS_CONTROL:
        {
            ste_cat_sms_control_response_t sc_rsp;
            int  ec_status;

            sc_rsp.sc_result = STE_CAT_CC_ALLOWED_NO_MODIFICATION;  //by default, we allow the cc
            ste_sim_decode_sc_response(&ec_status, &sc_rsp, buf, len);

            //check the ec_status first
            //if the EC is not executed successfully, should we allow or not? we set for NOT_ALLOWED for now
            if (ec_status != STE_CAT_RESPONSE_OK)
            {
                sc_rsp.sc_result = STE_CAT_CC_NOT_ALLOWED;
            }

            closure.func(STE_CAT_CAUSE_SIM_EC_SMS_CONTROL, /* Cause */
                         client_tag,
                         &sc_rsp,
                         closure.user_data);

            sim_free_sms_control_response_data(&sc_rsp);
        }
        break;

    case STE_CAT_RSP_GET_CAT_STATUS:
        {
            ste_cat_cat_status_t cat_status;

            cat_status.status = -1;
            ste_sim_decode_status_response(&cat_status.status, buf, len);

            closure.func(STE_CAT_CAUSE_GET_CAT_STATUS, /* Cause */
                         client_tag,
                         &cat_status,
                         closure.user_data);
        }
        break;

    case STE_CAT_IND_CAT_STATUS:
        {
            ste_cat_cat_status_t cat_status;

            cat_status.status = -1;
            ste_sim_decode_status_response(&cat_status.status, buf, len);

            closure.func(STE_CAT_CAUSE_IND_CAT_STATUS, /* Cause */
                         client_tag,
                         &cat_status,
                         closure.user_data);
        }
        break;

    case STE_CAT_IND_PC:
        {
            ste_cat_pc_ind_t        pc_data;

            sim_decode_pc_ind(&(pc_data), buf, len);

            closure.func(STE_CAT_CAUSE_PC, /* Cause */
                         client_tag,
                         &pc_data,
                         closure.user_data);

            if (pc_data.apdu.buf) {
                free(pc_data.apdu.buf);
            }
        }
        break;

    case STE_CAT_IND_PC_NOTIFICATION:
        {
            ste_cat_pc_ind_t        pc_data;

            sim_decode_pc_ind(&(pc_data), buf, len);

            closure.func(STE_CAT_CAUSE_PC_NOTIFICATION, /* Cause */
                         client_tag,
                         &pc_data,
                         closure.user_data);

            if (pc_data.apdu.buf) {
                free(pc_data.apdu.buf);
            }
        }
        break;

    case STE_CAT_IND_PC_SETUP_CALL:
        {
            ste_cat_pc_setup_call_ind_t    call_data;
            const char                    *p = buf;
            const char                    *p_max = buf + len;

            p = sim_dec(p, &call_data, sizeof(ste_cat_pc_setup_call_ind_t), p_max);
            if ( !p ) {
                break;
            }

            closure.func(STE_CAT_CAUSE_PC_SETUP_CALL_IND, /* Cause */
                         client_tag,
                         &call_data,
                         closure.user_data);
        }
        break;

    case STE_CAT_RSP_ANSWER_CALL:
        {
            ste_cat_answer_call_response_t call_status;

            call_status.status = -1;
            ste_sim_decode_status_response(&call_status.status, buf, len);

            closure.func(STE_CAT_CAUSE_ANSWER_CALL, /* Cause */
                         client_tag,
                         &call_status,
                         closure.user_data);
        }
        break;

    case STE_CAT_RSP_EVENT_DOWNLOAD:
        {
            ste_cat_event_download_response_t rsp;

            rsp.status = -1;
            ste_sim_decode_status_response(&rsp.status, buf, len);

            closure.func(STE_CAT_CAUSE_EVENT_DOWNLOAD, /* Cause */
                         client_tag,
                         &rsp,
                         closure.user_data);
        }
        break;

    case STE_CAT_IND_PC_REFRESH:
        {
            ste_cat_pc_refresh_ind_t       refresh_data;
            const char                    *p = buf;
            const char                    *p_max = buf + len;

            p = sim_dec(p, &refresh_data.simd_tag, sizeof(refresh_data.simd_tag), p_max);
            p = sim_dec(p, &refresh_data.type, sizeof(refresh_data.type), p_max);
            if ( !p ) {
                break;
            }

            closure.func(STE_CAT_CAUSE_PC_REFRESH_IND, /* Cause */
                         client_tag,
                         &refresh_data,
                         closure.user_data);
        }
        break;
    case STE_CAT_IND_PC_REFRESH_FILE:
        {
            ste_cat_pc_refresh_file_ind_t  refresh_data;
            const char                    *p = buf;
            const char                    *p_max = buf + len;

            p = sim_dec(p, &refresh_data.simd_tag, sizeof(refresh_data.simd_tag), p_max);
            p = sim_dec(p, &refresh_data.path.pathlen, sizeof(refresh_data.path.pathlen), p_max);
            p = sim_dec(p,  refresh_data.path.path, sizeof(refresh_data.path.path), p_max);
            if ( !p ) {
                break;
            }

            closure.func(STE_CAT_CAUSE_PC_REFRESH_FILE_IND, /* Cause */
                         client_tag,
                         &refresh_data,
                         closure.user_data);
        }
        break;
    case STE_CAT_IND_SESSION_END:
        {
            closure.func(STE_CAT_CAUSE_SESSION_END_IND, /* Cause */
                         client_tag,
                         NULL,
                         closure.user_data);
        }
        break;
    case STE_CAT_RSP_ENABLE:
        {
            ste_cat_enable_response_t enable_status;

            enable_status.status = -1;
            ste_sim_decode_status_response(&enable_status.status, buf, len);

            closure.func(STE_CAT_CAUSE_ENABLE, /* Cause */
                         client_tag,
                         &enable_status,
                         closure.user_data);
        }
        break;

    case STE_CAT_IND_CAT_INFO:
        {
            ste_cat_info_ind_t      cat_info;
            int                     status;

            cat_info.info = STE_CAT_INFO_NULL;
            ste_sim_decode_status_response(&status, buf, len);
            cat_info.info = (ste_cat_info_t)status;

            closure.func(STE_CAT_CAUSE_CAT_INFO, /* Cause */
                         client_tag,
                         &cat_info,
                         closure.user_data);
        }
        break;

    case STE_CAT_IND_RAT_SETTING_UPDATED:
        {
            closure.func(STE_CAT_CAUSE_RAT_SETTING_UPDATED, /* Cause */
                         client_tag,
                         NULL,
                         closure.user_data);
        }
        break;

    case STE_UICC_REQ_PIN_NEEDED:
        {
            printf("#\tRequest PIN needed.\n");
            closure.func(STE_UICC_CAUSE_REQ_PIN_NEEDED,      /* Cause */
                         client_tag,
                         0, /* No payload */
                         closure.user_data);
        }
        break;

    case STE_UICC_RSP_PIN_VERIFY:
        {
            ste_uicc_pin_verify_response_t      data;
            const char                         *p = buf;
            const char                         *p_max = buf + len;
            ste_uicc_status_word_t              status_word;
            ste_uicc_status_code_t              uicc_status_code;
            ste_uicc_status_code_fail_details_t uicc_status_code_fail_details;

            // split msg_data into separate parameters
            p = sim_dec(p, &uicc_status_code, sizeof(uicc_status_code), p_max);
            p = sim_dec(p, &uicc_status_code_fail_details, sizeof(uicc_status_code_fail_details), p_max);
            p = sim_dec(p, &status_word.sw1, sizeof(status_word.sw1), p_max);
            p = sim_dec(p, &status_word.sw2, sizeof(status_word.sw2), p_max);

            if (!p) {
                break;
            }

            data.uicc_status_code = uicc_status_code;
            data.uicc_status_code_fail_details = uicc_status_code_fail_details;
            data.status_word.sw1 = status_word.sw1;
            data.status_word.sw2 = status_word.sw2;

            closure.func(STE_UICC_CAUSE_REQ_PIN_VERIFY,
                                      client_tag,
                                      &data,
                                      closure.user_data);
        }
        break;

    case STE_UICC_RSP_GET_SIM_STATE:
        {
            ste_uicc_get_sim_state_response_t data;
            const char             *p = buf;
            const char             *p_max = buf + len;
            char                   *internal_state_name = NULL;
            size_t                  status_len;
            size_t                  data_len;
            uint32_t                status_tmp;

            data.error_cause = -1;
            // split msg_data into separate parameters
            p = sim_dec(p, &status_len, sizeof(status_len), p_max);
            p = sim_dec(p, &status_tmp, sizeof(char)*(status_len), p_max);
            if (status_tmp == 0){
               data.error_cause = 0;
            }
            p = sim_dec(p, &data_len, sizeof(data_len), p_max);
            if ( !p ) {
                break;
            }

            internal_state_name = malloc(data_len);
            p = sim_dec(p, internal_state_name, sizeof(char)*(data_len), p_max);
            if ( !p ) {
                free(internal_state_name);
                break;
            }

            data.state = (sim_state_t)FindStateFromStateTable(internal_state_name);

            free(internal_state_name);

            closure.func(STE_UICC_CAUSE_REQ_GET_SIM_STATE,      /* Cause */
                                      client_tag,
                                      &data,
                                      closure.user_data);
        }
        break;

    case STE_UICC_SIM_STATE_CHANGED:
        {
            ste_uicc_sim_state_changed_t data;
            const char             *p = buf;
            const char             *p_max = buf + len;
            char                   *internal_state_name = NULL;
            size_t                 data_len = 0;

            // split msg_data into separate parameters
            p = sim_dec(p, &data_len, sizeof(data_len), p_max);
            if ( !p ) {
                break;
            }

            internal_state_name = malloc(data_len);
            p = sim_dec(p, internal_state_name, data_len, p_max);
            if ( !p ) {
                free(internal_state_name);
                break;
            }

            data.state = (sim_state_t)FindStateFromStateTable(internal_state_name);

            free(internal_state_name);

            thread_data->closure.func(STE_UICC_CAUSE_SIM_STATE_CHANGED,      /* Cause */
                                      client_tag,
                                      &data,
                                      thread_data->closure.user_data);
        }
        break;

    case STE_UICC_RSP_PIN_CHANGE:
        {
            ste_uicc_pin_change_response_t data;
            const char                    *p = buf;
            const char                    *p_max = buf + len;
            ste_uicc_status_word_t         status_word;
            ste_uicc_status_code_t         uicc_status_code;
            ste_uicc_status_code_fail_details_t uicc_status_code_fail_details;

            // split msg_data into separate parameters
            p = sim_dec(p, &uicc_status_code, sizeof(uicc_status_code), p_max);
            p = sim_dec(p, &uicc_status_code_fail_details, sizeof(uicc_status_code_fail_details), p_max);
            p = sim_dec(p, &status_word.sw1, sizeof(status_word.sw1), p_max);
            p = sim_dec(p, &status_word.sw2, sizeof(status_word.sw2), p_max);

            if (!p) {
                break;
            }

            data.uicc_status_code = uicc_status_code;
            data.uicc_status_code_fail_details = uicc_status_code_fail_details;
            data.status_word.sw1 = status_word.sw1;
            data.status_word.sw2 = status_word.sw2;

            closure.func(STE_UICC_CAUSE_REQ_PIN_CHANGE,
                                      client_tag,
                                      &data,
                                      closure.user_data);
        }
        break;

    case STE_UICC_RSP_PIN_DISABLE:
        {
            ste_uicc_pin_disable_response_t data;
            const char                     *p = buf;
            const char                     *p_max = buf + len;
            ste_uicc_status_word_t          status_word;
            ste_uicc_status_code_t          uicc_status_code;
            ste_uicc_status_code_fail_details_t  uicc_status_code_fail_details;

            // split msg_data into separate parameters
            p = sim_dec(p, &uicc_status_code, sizeof(uicc_status_code), p_max);
            p = sim_dec(p, &uicc_status_code_fail_details, sizeof(uicc_status_code_fail_details), p_max);
            p = sim_dec(p, &status_word.sw1, sizeof(status_word.sw1), p_max);
            p = sim_dec(p, &status_word.sw2, sizeof(status_word.sw2), p_max);

            if (!p) {
                break;
            }

            data.uicc_status_code = uicc_status_code;
            data.uicc_status_code_fail_details = uicc_status_code_fail_details;
            data.status_word.sw1 = status_word.sw1;
            data.status_word.sw2 = status_word.sw2;

            closure.func(STE_UICC_CAUSE_REQ_PIN_DISABLE,
                                      client_tag,
                                      &data,
                                      closure.user_data);
        }
        break;

    case STE_UICC_RSP_PIN_ENABLE:
        {
            ste_uicc_pin_enable_response_t data;
            const char                    *p = buf;
            const char                    *p_max = buf + len;
            ste_uicc_status_word_t         status_word;
            ste_uicc_status_code_t         uicc_status_code;
            ste_uicc_status_code_fail_details_t uicc_status_code_fail_details;

            // split msg_data into separate parameters
            p = sim_dec(p, &uicc_status_code, sizeof(uicc_status_code), p_max);
            p = sim_dec(p, &uicc_status_code_fail_details, sizeof(uicc_status_code_fail_details), p_max);
            p = sim_dec(p, &status_word.sw1, sizeof(status_word.sw1), p_max);
            p = sim_dec(p, &status_word.sw2, sizeof(status_word.sw2), p_max);

            if (!p) {
                break;
            }

            data.uicc_status_code = uicc_status_code;
            data.uicc_status_code_fail_details = uicc_status_code_fail_details;
            data.status_word.sw1 = status_word.sw1;
            data.status_word.sw2 = status_word.sw2;

            closure.func(STE_UICC_CAUSE_REQ_PIN_ENABLE,
                                      client_tag,
                                      &data,
                                      closure.user_data);
        }
        break;

    case STE_UICC_RSP_PIN_INFO:
        {
            ste_uicc_pin_info_response_t   data;
            const char                    *p = buf;
            const char                    *p_max = buf + len;
            ste_sim_pin_status_t           pin_status;
            ste_uicc_status_code_t         uicc_status_code;
            ste_uicc_status_code_fail_details_t uicc_status_code_fail_details;

            // split msg_data into separate parameters
            p = sim_dec(p, &uicc_status_code, sizeof(uicc_status_code), p_max);
            p = sim_dec(p, &uicc_status_code_fail_details, sizeof(uicc_status_code_fail_details), p_max);
            p = sim_dec(p, &pin_status, sizeof(pin_status), p_max);
            p = sim_dec(p, &data.attempts_remaining, sizeof(data.attempts_remaining), p_max);

            if ( !p ) {
                break;
            }

            switch (pin_status) {
                case STE_SIM_PIN_STATUS_ENABLED:
                    data.pin_status = STE_UICC_PIN_STATUS_ENABLED;
                break;
                case STE_SIM_PIN_STATUS_DISABLED:
                    data.pin_status = STE_UICC_PIN_STATUS_DISABLED;
                break;
                case STE_SIM_PIN_STATUS_UNKNOWN:
                    data.pin_status = STE_UICC_PIN_STATUS_UNKNOWN;
                break;
                default:
                    data.pin_status = STE_UICC_PIN_STATUS_UNKNOWN;
                break;
            }

            data.uicc_status_code = uicc_status_code;
            data.uicc_status_code_fail_details = uicc_status_code_fail_details;

            closure.func(STE_UICC_CAUSE_REQ_PIN_INFO,      /* Cause */
                                      client_tag,
                                      &data,
                                      closure.user_data);
        }
        break;


    case STE_UICC_RSP_PIN_UNBLOCK:
        {
            ste_uicc_pin_unblock_response_t data;
            const char                     *p = buf;
            const char                     *p_max = buf + len;
            ste_uicc_status_word_t          status_word;
            ste_uicc_status_code_t          uicc_status_code;
            ste_uicc_status_code_fail_details_t  uicc_status_code_fail_details;

            // split msg_data into separate parameters
            p = sim_dec(p, &uicc_status_code, sizeof(uicc_status_code), p_max);
            p = sim_dec(p, &uicc_status_code_fail_details, sizeof(uicc_status_code_fail_details), p_max);
            p = sim_dec(p, &status_word.sw1, sizeof(status_word.sw1), p_max);
            p = sim_dec(p, &status_word.sw2, sizeof(status_word.sw2), p_max);

            if (!p) {
                break;
            }

            data.uicc_status_code = uicc_status_code;
            data.uicc_status_code_fail_details = uicc_status_code_fail_details;
            data.status_word.sw1 = status_word.sw1;
            data.status_word.sw2 = status_word.sw2;

            closure.func(STE_UICC_CAUSE_REQ_PIN_UNBLOCK,
                                      client_tag,
                                      &data,
                                      closure.user_data);
        }
        break;

    case STE_UICC_RSP_APP_INFO:
        {
            ste_uicc_sim_app_info_response_t data;
            const char *p = buf;
            const char *p_max = buf + len;

            p = sim_dec(p, &data.app_type, sizeof(data.app_type), p_max);
            if ( !p ) {
                break;
            }
            data.status = 0;
            // FIXME: Place assertions on len
            closure.func(STE_UICC_CAUSE_REQ_GET_APP_INFO,
                                      client_tag,
                                      &data,
                                      closure.user_data);
        }
        break;
    case STE_UICC_RSP_READ_SIM_FILE_RECORD:
        {
            ste_uicc_sim_file_read_record_response_t data;
            uint8_t                        *p_data = NULL;
            const char                     *p = buf;
            const char                     *p_max = buf + len;
            size_t                         data_len;
            ste_uicc_status_word_t         status_word;
            ste_uicc_status_code_t         uicc_status_code;
            ste_uicc_status_code_fail_details_t uicc_status_code_fail_details;

            data.data = NULL;

            // split msg_data into separate parameters
            p = sim_dec(p, &uicc_status_code, sizeof(uicc_status_code), p_max);
            p = sim_dec(p, &uicc_status_code_fail_details, sizeof(uicc_status_code_fail_details), p_max);
            p = sim_dec(p, &status_word.sw1, sizeof(status_word.sw1), p_max);
            p = sim_dec(p, &status_word.sw2, sizeof(status_word.sw2), p_max);
            p = sim_dec(p, &data_len, sizeof(data_len), p_max);

            if ( !p ) {
                break;
            }

            if (data_len > 0) {
                data.data = malloc(data_len);
                p_data = data.data;

                p = sim_dec(p, data.data, sizeof(char)*(data_len), p_max);
                if ( !p ) {
                    free(data.data);
                    break;
                }
            }

            data.length = data_len;
            data.status_word.sw1 = status_word.sw1;
            data.status_word.sw2 = status_word.sw2;
            data.uicc_status_code = uicc_status_code;
            data.uicc_status_code_fail_details = uicc_status_code_fail_details;

            closure.func(STE_UICC_CAUSE_REQ_READ_SIM_FILE_RECORD,      /* Cause */
                                      client_tag,
                                      &data,
                                      closure.user_data);
            if (p_data)
                free(p_data);
        }
        break;

    case STE_UICC_RSP_READ_SIM_FILE_BINARY:
        {
            ste_uicc_sim_file_read_binary_response_t data;
            const char                     *p = buf;
            uint8_t                        *p_data = NULL;
            const char                     *p_max = buf + len;
            size_t                         data_len;
            ste_uicc_status_word_t         status_word;
            ste_uicc_status_code_t         uicc_status_code;
            ste_uicc_status_code_fail_details_t uicc_status_code_fail_details;

            data.data = NULL;

            // split msg_data into separate parameters
            p = sim_dec(p, &uicc_status_code, sizeof(uicc_status_code), p_max);
            p = sim_dec(p, &uicc_status_code_fail_details, sizeof(uicc_status_code_fail_details), p_max);
            p = sim_dec(p, &status_word.sw1, sizeof(status_word.sw1), p_max);
            p = sim_dec(p, &status_word.sw2, sizeof(status_word.sw2), p_max);
            p = sim_dec(p, &data_len, sizeof(data_len), p_max);

            if ( !p ) {
                break;
            }

            if (data_len > 0) {
                data.data = malloc(data_len);
                p_data = data.data;
                p = sim_dec(p, data.data, sizeof(char)*(data_len), p_max);
                if ( !p ) {
                    free(data.data);
                    break;
                }
             }

            data.length = data_len;
            data.status_word.sw1 = status_word.sw1;
            data.status_word.sw2 = status_word.sw2;
            data.uicc_status_code = uicc_status_code;
            data.uicc_status_code_fail_details = uicc_status_code_fail_details;

            closure.func(STE_UICC_CAUSE_REQ_READ_SIM_FILE_BINARY,      /* Cause */
                                      client_tag,
                                      &data,
                                      closure.user_data);
            if (p_data)
                free(p_data);
        }
        break;

    case STE_UICC_RSP_SIM_FILE_GET_FORMAT:
        {
            ste_uicc_sim_file_get_format_response_t data;
            const char                     *p = buf;
            const char                     *p_max = buf + len;
            ste_uicc_status_word_t         status_word;
            ste_uicc_status_code_t         uicc_status_code;
            ste_uicc_status_code_fail_details_t uicc_status_code_fail_details;

            // split msg_data into separate parameters
            p = sim_dec(p, &uicc_status_code, sizeof(uicc_status_code), p_max);
            p = sim_dec(p, &uicc_status_code_fail_details, sizeof(uicc_status_code_fail_details), p_max);
            p = sim_dec(p, &status_word.sw1, sizeof(status_word.sw1), p_max);
            p = sim_dec(p, &status_word.sw2, sizeof(status_word.sw2), p_max);
            p = sim_dec(p, &data.file_type, sizeof(data.file_type), p_max);
            p = sim_dec(p, &data.file_size, sizeof(data.file_size), p_max);
            p = sim_dec(p, &data.record_len, sizeof(data.record_len), p_max);
            p = sim_dec(p, &data.num_records, sizeof(data.num_records), p_max);

            if ( !p ) break;

            data.status_word.sw1 = status_word.sw1;
            data.status_word.sw2 = status_word.sw2;
            data.uicc_status_code = uicc_status_code;
            data.uicc_status_code_fail_details = uicc_status_code_fail_details;

            closure.func(STE_UICC_CAUSE_REQ_SIM_FILE_GET_FORMAT,      /* Cause */
                                      client_tag,
                                      &data,
                                      closure.user_data);
        }
        break;

    case STE_UICC_RSP_GET_FILE_INFORMATION:
        {
            ste_uicc_get_file_information_response_t data;
            const char                     *p = buf;
            uint8_t                        *p_data = NULL;
            const char                     *p_max = buf + len;
            ste_uicc_status_word_t         status_word;
            ste_uicc_status_code_t         uicc_status_code;
            ste_uicc_status_code_fail_details_t uicc_status_code_fail_details;

            data.fcp = NULL;

           // split msg_data into separate parameters
            p = sim_dec(p, &uicc_status_code, sizeof(uicc_status_code), p_max);
            p = sim_dec(p, &uicc_status_code_fail_details, sizeof(uicc_status_code_fail_details), p_max);
            p = sim_dec(p, &status_word.sw1, sizeof(status_word.sw1), p_max);
            p = sim_dec(p, &status_word.sw2, sizeof(status_word.sw2), p_max);
            p = sim_dec(p, &data.length, sizeof(data.length), p_max);

            if ( !p ) {
                break;
            }

            if (data.length > 0) {
                data.fcp = malloc(data.length);
                p_data = data.fcp;
                p = sim_dec(p, data.fcp, data.length, p_max);
                if ( !p ) {
                    free(p_data);
                    break;
                }
            }

            data.status_word.sw1 = status_word.sw1;
            data.status_word.sw2 = status_word.sw2;
            data.uicc_status_code = uicc_status_code;
            data.uicc_status_code_fail_details = uicc_status_code_fail_details;

            closure.func(STE_UICC_CAUSE_REQ_GET_FILE_INFORMATION,      /* Cause */
                               client_tag,
                               &data,
                               closure.user_data);
            if (p_data) free(p_data);
        }
        break;

    case STE_UICC_RSP_UPDATE_SIM_FILE_RECORD:
        {
            ste_uicc_update_sim_file_record_response_t data;
            const char             *p = buf;
            const char             *p_max = buf + len;
            ste_uicc_status_word_t         status_word;
            ste_uicc_status_code_t         uicc_status_code;
            ste_uicc_status_code_fail_details_t uicc_status_code_fail_details;

            // split msg_data into separate parameters
            p = sim_dec(p, &uicc_status_code, sizeof(uicc_status_code), p_max);
            p = sim_dec(p, &uicc_status_code_fail_details, sizeof(uicc_status_code_fail_details), p_max);
            p = sim_dec(p, &status_word.sw1, sizeof(status_word.sw1), p_max);
            p = sim_dec(p, &status_word.sw2, sizeof(status_word.sw2), p_max);

            if ( !p ) {
                break;
            }

            data.status_word.sw1 = status_word.sw1;
            data.status_word.sw2 = status_word.sw2;
            data.uicc_status_code = uicc_status_code;
            data.uicc_status_code_fail_details = uicc_status_code_fail_details;

            closure.func(STE_UICC_CAUSE_REQ_UPDATE_SIM_FILE_RECORD,      /* Cause */
                                      client_tag,
                                      &data,
                                      closure.user_data);
        }
        break;

    case STE_UICC_RSP_UPDATE_SIM_FILE_BINARY:
        {
            ste_uicc_update_sim_file_binary_response_t data;
            const char                    *p = buf;
            const char                    *p_max = buf + len;
            ste_uicc_status_word_t         status_word;
            ste_uicc_status_code_t         uicc_status_code;
            ste_uicc_status_code_fail_details_t uicc_status_code_fail_details;

            // split msg_data into separate parameters
            p = sim_dec(p, &uicc_status_code, sizeof(uicc_status_code), p_max);
            p = sim_dec(p, &uicc_status_code_fail_details, sizeof(uicc_status_code_fail_details), p_max);
            p = sim_dec(p, &status_word.sw1, sizeof(status_word.sw1), p_max);
            p = sim_dec(p, &status_word.sw2, sizeof(status_word.sw2), p_max);

            if ( !p ) {
                break;
            }

            data.status_word.sw1 = status_word.sw1;
            data.status_word.sw2 = status_word.sw2;
            data.uicc_status_code = uicc_status_code;
            data.uicc_status_code_fail_details = uicc_status_code_fail_details;

            closure.func(STE_UICC_CAUSE_REQ_UPDATE_SIM_FILE_BINARY,      /* Cause */
                                      client_tag,
                                      &data,
                                      closure.user_data);
        }
        break;

    case STE_UICC_RSP_READ_SMSC:
        {
            ste_uicc_sim_smsc_get_active_response_t set_result;
            const char                             *p = buf;
            const char                             *p_max = buf + len;
            ste_uicc_status_word_t                  status_word;
            ste_uicc_status_code_t                  uicc_status_code;
            ste_uicc_status_code_fail_details_t     uicc_status_code_fail_details;
            size_t                                  status_len = sizeof(uicc_status_code) +
                                                                 sizeof(uicc_status_code_fail_details) +
                                                                 sizeof(status_word.sw1) +
                                                                 sizeof(status_word.sw2);

            // split msg_data into separate parameters
            p = sim_dec(p, &uicc_status_code, sizeof(uicc_status_code), p_max);
            p = sim_dec(p, &uicc_status_code_fail_details, sizeof(uicc_status_code_fail_details), p_max);
            p = sim_dec(p, &status_word.sw1, sizeof(status_word.sw1), p_max);
            p = sim_dec(p, &status_word.sw2, sizeof(status_word.sw2), p_max);

            set_result.smsc.num_text.text_p = NULL;

            if (len > status_len) {
                sim_decode_smsc_string(&(set_result.smsc), p, len - status_len);
            }

            if ( !p ) {
                break;
            }

            set_result.status_word.sw1 = status_word.sw1;
            set_result.status_word.sw2 = status_word.sw2;
            set_result.uicc_status_code = uicc_status_code;
            set_result.uicc_status_code_fail_details = uicc_status_code_fail_details;

            closure.func(STE_UICC_CAUSE_REQ_SMSC_GET_ACTIVE,      /* Cause */
                         client_tag,
                         &set_result,
                         closure.user_data);

            if (set_result.smsc.num_text.text_p) {
                free(set_result.smsc.num_text.text_p);
            }
        }
        break;

    case STE_UICC_RSP_SMSC_GET_RECORD_MAX:
    {
            ste_uicc_sim_smsc_get_record_max_response_t data;
            const char                                 *p = buf;
            const char                                 *p_max = buf + len;
            ste_uicc_status_word_t                      status_word;
            ste_uicc_status_code_t                      uicc_status_code;
            ste_uicc_status_code_fail_details_t         uicc_status_code_fail_details;
            int                                         max_record_id = 0;

            // split msg_data into separate parameters
            p = sim_dec(p, &uicc_status_code, sizeof(uicc_status_code), p_max);
            p = sim_dec(p, &uicc_status_code_fail_details, sizeof(uicc_status_code_fail_details), p_max);
            p = sim_dec(p, &status_word.sw1, sizeof(status_word.sw1), p_max);
            p = sim_dec(p, &status_word.sw2, sizeof(status_word.sw2), p_max);
            p = sim_dec(p, &max_record_id, sizeof(max_record_id), p_max);

            if ( !p ) {
                break;
            }

            data.status_word.sw1 = status_word.sw1;
            data.status_word.sw2 = status_word.sw2;
            data.uicc_status_code = uicc_status_code;
            data.uicc_status_code_fail_details = uicc_status_code_fail_details;
            data.max_record_id = max_record_id;

            closure.func(STE_UICC_CAUSE_REQ_SMSC_GET_RECORD_MAX,      /* Cause */
                                      client_tag,
                                      &data,
                                      closure.user_data);
    }
    break;

    case STE_UICC_RSP_SMSC_RESTORE_FROM_RECORD:
    {
            ste_uicc_sim_smsc_restore_from_record_response_t data;
            const char                                      *p = buf;
            const char                                      *p_max = buf + len;
            ste_uicc_status_word_t                           status_word;
            ste_uicc_status_code_t                           uicc_status_code;
            ste_uicc_status_code_fail_details_t              uicc_status_code_fail_details;

            // split msg_data into separate parameters
            p = sim_dec(p, &uicc_status_code, sizeof(uicc_status_code), p_max);
            p = sim_dec(p, &uicc_status_code_fail_details, sizeof(uicc_status_code_fail_details), p_max);
            p = sim_dec(p, &status_word.sw1, sizeof(status_word.sw1), p_max);
            p = sim_dec(p, &status_word.sw2, sizeof(status_word.sw2), p_max);

            if ( !p ) {
                break;
            }

            data.status_word.sw1 = status_word.sw1;
            data.status_word.sw2 = status_word.sw2;
            data.uicc_status_code = uicc_status_code;
            data.uicc_status_code_fail_details = uicc_status_code_fail_details;

            closure.func(STE_UICC_CAUSE_REQ_SMSC_RESTORE_FROM_RECORD,      /* Cause */
                         client_tag,
                         &data,
                         closure.user_data);
    }
    break;

    case STE_UICC_RSP_SMSC_SAVE_TO_RECORD:
    {
            ste_uicc_sim_smsc_save_to_record_response_t data;
            const char                                 *p = buf;
            const char                                 *p_max = buf + len;
            ste_uicc_status_word_t                      status_word;
            ste_uicc_status_code_t                      uicc_status_code;
            ste_uicc_status_code_fail_details_t         uicc_status_code_fail_details;

            // split msg_data into separate parameters
            p = sim_dec(p, &uicc_status_code, sizeof(uicc_status_code), p_max);
            p = sim_dec(p, &uicc_status_code_fail_details, sizeof(uicc_status_code_fail_details), p_max);
            p = sim_dec(p, &status_word.sw1, sizeof(status_word.sw1), p_max);
            p = sim_dec(p, &status_word.sw2, sizeof(status_word.sw2), p_max);

            if ( !p ) {
                break;
            }

            data.status_word.sw1 = status_word.sw1;
            data.status_word.sw2 = status_word.sw2;
            data.uicc_status_code = uicc_status_code;
            data.uicc_status_code_fail_details = uicc_status_code_fail_details;

            closure.func(STE_UICC_CAUSE_REQ_SMSC_SAVE_TO_RECORD,      /* Cause */
                         client_tag,
                         &data,
                         closure.user_data);
    }
    break;

    case STE_UICC_RSP_APPL_APDU_SEND:
        {
            ste_uicc_appl_apdu_send_response_t data;
            const char                     *p = buf;
            uint8_t                        *p_data = NULL;
            const char                     *p_max = buf + len;
            size_t                         data_len;
            ste_uicc_status_word_t         status_word;
            ste_uicc_status_code_t         uicc_status_code;
            ste_uicc_status_code_fail_details_t uicc_status_code_fail_details;

            data.data = NULL;

            // split msg_data into separate parameters
            p = sim_dec(p, &uicc_status_code, sizeof(uicc_status_code), p_max);
            p = sim_dec(p, &uicc_status_code_fail_details, sizeof(uicc_status_code_fail_details), p_max);
            p = sim_dec(p, &status_word.sw1, sizeof(status_word.sw1), p_max);
            p = sim_dec(p, &status_word.sw2, sizeof(status_word.sw2), p_max);
            p = sim_dec(p, &data_len, sizeof(data_len), p_max);

            if ( !p ) {
                break;
            }

            if (data_len > 0) {
                data.data = malloc(data_len);
                p_data = data.data;
                p = sim_dec(p, data.data, sizeof(char)*(data_len), p_max);
                if ( !p ) {
                    free(data.data);
                    break;
                }
             }

            data.length = data_len;
            data.status_word.sw1 = status_word.sw1;
            data.status_word.sw2 = status_word.sw2;
            data.uicc_status_code = uicc_status_code;
            data.uicc_status_code_fail_details = uicc_status_code_fail_details;

            closure.func(STE_UICC_CAUSE_REQ_APPL_APDU_SEND,      /* Cause */
                                      client_tag,
                                      &data,
                                      closure.user_data);
            if (p_data)
                free(p_data);
        }
        break;

    case STE_UICC_RSP_UPDATE_SMSC:
        {
            ste_uicc_sim_smsc_set_active_response_t set_result;
            const char             *p = buf;
            const char             *p_max = buf + len;
            ste_uicc_status_word_t         status_word;
            ste_uicc_status_code_t         uicc_status_code;
            ste_uicc_status_code_fail_details_t uicc_status_code_fail_details;

            // split msg_data into separate parameters
            p = sim_dec(p, &uicc_status_code, sizeof(uicc_status_code), p_max);
            p = sim_dec(p, &uicc_status_code_fail_details, sizeof(uicc_status_code_fail_details), p_max);
            p = sim_dec(p, &status_word.sw1, sizeof(status_word.sw1), p_max);
            p = sim_dec(p, &status_word.sw2, sizeof(status_word.sw2), p_max);
            if ( !p ) {
                break;
            }

            set_result.uicc_status_code = uicc_status_code;
            set_result.uicc_status_code_fail_details = uicc_status_code_fail_details;
            set_result.status_word.sw1 = status_word.sw1;
            set_result.status_word.sw2 = status_word.sw2;

            closure.func(STE_UICC_CAUSE_REQ_SMSC_SET_ACTIVE,      /* Cause */
                         client_tag,
                         &set_result,
                         closure.user_data);

        }
        break;

    case STE_UICC_RSP_UPDATE_SERVICE_TABLE:
        {
            ste_uicc_update_service_table_response_t update_result;
            const char             *p = buf;
            const char             *p_max = buf + len;
            ste_uicc_status_word_t         status_word;
            ste_uicc_status_code_t         uicc_status_code;
            ste_uicc_status_code_fail_details_t uicc_status_code_fail_details;

            // split msg_data into separate parameters
            p = sim_dec(p, &uicc_status_code, sizeof(uicc_status_code), p_max);
            p = sim_dec(p, &uicc_status_code_fail_details, sizeof(uicc_status_code_fail_details), p_max);
            p = sim_dec(p, &status_word.sw1, sizeof(status_word.sw1), p_max);
            p = sim_dec(p, &status_word.sw2, sizeof(status_word.sw2), p_max);
            if ( !p ) {
                break;
            }

            update_result.uicc_status_code = uicc_status_code;
            update_result.uicc_status_code_fail_details = uicc_status_code_fail_details;
            update_result.status_word.sw1 = status_word.sw1;
            update_result.status_word.sw2 = status_word.sw2;

            closure.func(STE_UICC_CAUSE_REQ_UPDATE_SERVICE_TABLE,      /* Cause */
                         client_tag,
                         &update_result,
                         closure.user_data);

        }
        break;

    case STE_UICC_RSP_GET_SERVICE_TABLE:
        {
            ste_uicc_get_service_table_response_t data;
            const char             *p = buf;
            const char             *p_max = buf + len;
            ste_uicc_status_word_t         status_word;
            ste_uicc_status_code_t         uicc_status_code;
            ste_uicc_status_code_fail_details_t uicc_status_code_fail_details;
            sim_uicc_service_status_t           service_status;

            // split msg_data into separate parameters
            p = sim_dec(p, &uicc_status_code, sizeof(uicc_status_code), p_max);
            p = sim_dec(p, &uicc_status_code_fail_details, sizeof(uicc_status_code_fail_details), p_max);
            p = sim_dec(p, &status_word.sw1, sizeof(status_word.sw1), p_max);
            p = sim_dec(p, &status_word.sw2, sizeof(status_word.sw2), p_max);
            p = sim_dec(p, &service_status, sizeof(service_status), p_max);

            if ( !p ) {
                break;
            }

            switch (service_status) {
                case SIM_UICC_SERVICE_STATUS_ENABLED:
                    data.service_status = STE_UICC_SERVICE_STATUS_ENABLED;
                break;
                case SIM_UICC_SERVICE_STATUS_DISABLED:
                    data.service_status = STE_UICC_SERVICE_STATUS_DISABLED;
                break;
                case SIM_UICC_SERVICE_STATUS_UNKNOWN:
                    data.service_status = STE_UICC_SERVICE_STATUS_UNKNOWN;
                break;
                default:
                    data.service_status = STE_UICC_SERVICE_STATUS_UNKNOWN;
                break;
            }

            data.uicc_status_code = uicc_status_code;
            data.uicc_status_code_fail_details = uicc_status_code_fail_details;
            data.status_word.sw1 = status_word.sw1;
            data.status_word.sw2 = status_word.sw2;

            closure.func(STE_UICC_CAUSE_REQ_GET_SERVICE_TABLE,      /* Cause */
                         client_tag,
                         &data,
                         closure.user_data);

        }
        break;

    case STE_UICC_RSP_GET_SERVICE_AVAILABILITY:
        {
            ste_uicc_get_service_availability_response_t data;
            const char             *p = buf;
            const char             *p_max = buf + len;
            ste_uicc_status_code_t         uicc_status_code;
            ste_uicc_status_code_fail_details_t uicc_status_code_fail_details;
            sim_uicc_service_availability_t     service_availability;

            // split msg_data into separate parameters
            p = sim_dec(p, &uicc_status_code, sizeof(uicc_status_code), p_max);
            p = sim_dec(p, &uicc_status_code_fail_details, sizeof(uicc_status_code_fail_details), p_max);
            p = sim_dec(p, &service_availability, sizeof(service_availability), p_max);

            if ( !p ) {
                break;
            }

            switch (service_availability) {
                case SIM_UICC_SERVICE_AVAILABLE:
                    data.service_availability = STE_UICC_SERVICE_AVAILABLE;
                break;
                case SIM_UICC_SERVICE_NOT_AVAILABLE:
                    data.service_availability = STE_UICC_SERVICE_NOT_AVAILABLE;
                break;
                case SIM_UICC_SERVICE_AVAILABILITY_UNKNOWN:
                    data.service_availability = STE_UICC_SERVICE_AVAILABILITY_UNKNOWN;
                break;
                default:
                    data.service_availability = STE_UICC_SERVICE_AVAILABILITY_UNKNOWN;
                break;
            }

            data.uicc_status_code = uicc_status_code;
            data.uicc_status_code_fail_details = uicc_status_code_fail_details;

            closure.func(STE_UICC_CAUSE_REQ_GET_SERVICE_AVAILABILITY,      /* Cause */
                         client_tag,
                         &data,
                         closure.user_data);

        }
        break;

    case STE_UICC_RSP_SIM_FILE_READ_GENERIC:
        {
            ste_uicc_sim_file_read_generic_response_t read_result;

            if ((buf != NULL && *buf == '\0' && len == 0) || buf == NULL) {
                read_result.file_id = 0;
                read_result.data_len = 0;
                read_result.uicc_status_code = STE_UICC_STATUS_CODE_FAIL;
                read_result.uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR;
                read_result.status_word.sw1 = 0;
                read_result.status_word.sw2 = 0;
                read_result.data_p = NULL;
            }
            else {
                // split msg_data into separate parameters
                read_result.data_p = NULL;

                sim_decode_read_generic_result(&(read_result), buf, len);
            }

            ste_sim_handle_read_generic_response(closure, client_tag, &read_result);

            if (read_result.data_p) {
                free(read_result.data_p);
            }
        }
        break;

        case STE_UICC_RSP_REGISTER:
            closure.func(STE_UICC_CAUSE_REGISTER,      /* Cause */
                                      client_tag,
                                      0,    /* No payload */
                                      closure.user_data);
            break;

        case STE_UICC_RSP_NOT_READY:
        {
            ste_uicc_not_ready_t    data;
            const char             *p = buf;
            const char             *p_max = buf + len;

            p = sim_dec(p, &data.type, sizeof(data.type), p_max);
            if ( !p ) {
                break;
            }

            printf("#\tsim.c handle command, decode:not ready, type=%x\n",data.type);
            closure.func(STE_UICC_CAUSE_NOT_READY,      /* Cause */
                                      client_tag,
                                      &data,
                                      closure.user_data);
        }
            break;
    case STE_UICC_SIM_STATUS: {
      sim_status_t data;
      if (sim_dec(buf, &data.reason, sizeof(data.reason), buf+len))
        closure.func(STE_UICC_CAUSE_SIM_STATUS,
                     client_tag,
                     &data,
                     closure.user_data);
    }
      break;
    case STE_UICC_RSP_SIM_ICON_READ:
        {
            ste_uicc_sim_icon_read_response_t data;
            const char            *p = buf;
            uint8_t               *p_data = NULL;
            const char            *p_max = buf + len;
            size_t                 data_len;
            ste_uicc_status_word_t status_word;
            ste_uicc_status_code_t uicc_status_code;
            ste_uicc_status_code_fail_details_t uicc_status_code_fail_details;

            data.data = NULL;

            // split msg_data into separate parameters
            p = sim_dec(p, &uicc_status_code, sizeof(uicc_status_code), p_max);
            p = sim_dec(p, &uicc_status_code_fail_details, sizeof(uicc_status_code_fail_details), p_max);
            p = sim_dec(p, &status_word.sw1, sizeof(status_word.sw1), p_max);
            p = sim_dec(p, &status_word.sw2, sizeof(status_word.sw2), p_max);
            p = sim_dec(p, &data_len, sizeof(data_len), p_max);

            if ( !p ) {
                break;
            }

            if (data_len > 0) {
                data.data = malloc(data_len);
                p_data = data.data;
                p = sim_dec(p, data.data, sizeof(char)*(data_len), p_max);
                if ( !p ) {
                    free(data.data);
                    break;
                }
             }
            // One could decode directly into this data structure, but it is
            // good to do it this way to make sure data types and sizes are
            // properly handled by doing these assignments with implicit casts.
            data.length = data_len;
            data.status_word.sw1 = status_word.sw1;
            data.status_word.sw2 = status_word.sw2;
            data.uicc_status_code = uicc_status_code;
            data.uicc_status_code_fail_details = uicc_status_code_fail_details;

            closure.func(STE_UICC_CAUSE_REQ_SIM_ICON_READ,      /* Cause */
                                      client_tag,
                                      &data,
                                      closure.user_data);
            if (p_data)
                free(p_data);
        }
        break;

    case STE_UICC_RSP_READ_SUBSCRIBER_NUMBER:
        {
            ste_sim_subscriber_number_response_t data;
            const char            *p = buf;
            const char            *p_max = buf + len;
            ste_uicc_status_word_t status_word = {0,0};
            ste_uicc_status_code_t uicc_status_code = STE_UICC_STATUS_CODE_OK;
            ste_uicc_status_code_fail_details_t uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;

            uint8_t  alpha_id_len = 0;
            uint16_t number_of_records = 0; /* Number of records will not be larger then 256. */
            uint16_t record_length = 0; /* Record length will not be larger then 256. */
            ste_sim_subscriber_number_record_t *record_data_p = NULL;
            ste_sim_subscriber_number_record_t *tmp_rec_data_p = NULL;

            data.uicc_status_code = uicc_status_code;
            data.uicc_status_code_fail_details = uicc_status_code_fail_details;
            data.status_word.sw1 = status_word.sw1;
            data.status_word.sw2 = status_word.sw2;
            data.record_data_p = NULL;

            /* Split msg_data into separate parameters. */
            p = sim_dec(p, &uicc_status_code, sizeof(uicc_status_code), p_max);
            p = sim_dec(p, &uicc_status_code_fail_details, sizeof(uicc_status_code_fail_details), p_max);
            p = sim_dec(p, &status_word.sw1, sizeof(status_word.sw1), p_max);
            p = sim_dec(p, &status_word.sw2, sizeof(status_word.sw2), p_max);
            p = sim_dec(p, &alpha_id_len, sizeof(alpha_id_len), p_max);
            p = sim_dec(p, &number_of_records, sizeof(number_of_records), p_max);
            p = sim_dec(p, &record_length, sizeof(record_length), p_max);

            if ( NULL == p ) {
                goto error;
            }

            data.uicc_status_code = uicc_status_code;
            data.uicc_status_code_fail_details = uicc_status_code_fail_details;
            data.status_word.sw1 = status_word.sw1;
            data.status_word.sw2 = status_word.sw2;
            data.number_of_records = number_of_records;
            data.alpha_id_len = alpha_id_len;
            data.record_length = record_length;

            if (number_of_records > 0) {
                int i = 0;
                uint8_t  alpha_id_actual_len = 0;
                uint8_t  alpha_coding = STE_SIM_CODING_UNKNOWN;
                uint8_t  number_actual_len = 0;
                uint8_t  type = 0;
                uint8_t  speed = 0;
                uint8_t  service = 0;
                uint8_t  itc = 0;

                /* Allocate memory for all records. */
                data.record_data_p = NULL;
                data.record_data_p = malloc(sizeof(ste_sim_subscriber_number_record_t) * number_of_records);
                record_data_p = data.record_data_p;
                tmp_rec_data_p = data.record_data_p;

                if ( NULL == data.record_data_p ) {
                    goto error;
                }

                /* Initialize all pointers. */
                for (i = 0; i < number_of_records; i++) {
                    tmp_rec_data_p->alpha_p = NULL;
                    tmp_rec_data_p->number_p = NULL;
                    tmp_rec_data_p++;
                }

                tmp_rec_data_p = data.record_data_p;

                for(i=0; i<number_of_records; i++){

                    /* Decode Alpha Identifier. */
                    p = sim_dec(p, &alpha_id_actual_len, sizeof(alpha_id_actual_len), p_max);
                    p = sim_dec(p, &alpha_coding, sizeof(alpha_coding), p_max);
                    tmp_rec_data_p->alpha_p = malloc(sizeof(uint8_t) * alpha_id_actual_len);

                    if ( (NULL == p) || (NULL == tmp_rec_data_p->alpha_p) ) {
                        data.record_data_p = NULL;
                        goto error;
                    }

                    tmp_rec_data_p->alpha_id_actual_len = alpha_id_actual_len;
                    tmp_rec_data_p->alpha_coding = alpha_coding;
                    p = sim_dec(p, tmp_rec_data_p->alpha_p, alpha_id_actual_len, p_max);

                    /* Decode Dialling Number. */
                    p = sim_dec(p, &number_actual_len, sizeof(number_actual_len), p_max);
                    tmp_rec_data_p->number_p = malloc(sizeof(uint8_t) * number_actual_len);

                    if ( (NULL == p) || (NULL == tmp_rec_data_p->number_p) ) {
                        data.record_data_p = NULL;
                        goto error;
                    }

                    tmp_rec_data_p->number_actual_len = number_actual_len;
                    p = sim_dec(p, tmp_rec_data_p->number_p, number_actual_len, p_max);

                    /* Decode Type Of Number. */
                    p = sim_dec(p, &type, sizeof(type), p_max);

                    /* Decode Speed. */
                    p = sim_dec(p, &speed, sizeof(speed), p_max);

                    /* Decode Service. */
                    p = sim_dec(p, &service, sizeof(service), p_max);

                    /* Decode ICT (Information Transfer Capability). */
                    p = sim_dec(p, &itc, sizeof(itc), p_max);

                    if (NULL == p) {
                        data.record_data_p = NULL;
                        goto error;
                    }

                    tmp_rec_data_p->type = type;
                    tmp_rec_data_p->speed = speed;
                    tmp_rec_data_p->service = service;
                    tmp_rec_data_p->itc = itc;

                    tmp_rec_data_p++;
                }
             }

            closure.func(STE_UICC_CAUSE_REQ_READ_SUBSCRIBER_NUMBER,      /* Cause */
                         client_tag,
                         &data,
                         closure.user_data);

            error:
            /* Clean up and free all dynamically allocated memory. */
            ste_sim_subscriber_number_cleanup(record_data_p, number_of_records);

        }
        break;

    case STE_UICC_RSP_UPDATE_PLMN:
    {
        ste_uicc_update_sim_file_plmn_response_t update_result;
        const char *p = buf;
        const char *p_max = buf + len;
        ste_uicc_status_word_t status_word = {0,0};
        ste_uicc_status_code_t uicc_status_code;
        ste_uicc_status_code_fail_details_t uicc_status_code_fail_details;

        if ((buf != NULL && *buf == '\0' && len == 0) || buf == NULL) {
            uicc_status_code = STE_UICC_STATUS_CODE_FAIL;
            uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
        } else {
            // split msg_data into separate parameters
            p = sim_dec(p, &uicc_status_code, sizeof(uicc_status_code), p_max);
            p = sim_dec(p, &uicc_status_code_fail_details, sizeof(uicc_status_code_fail_details), p_max);
            p = sim_dec(p, &status_word.sw1, sizeof(status_word.sw1), p_max);
            sim_dec(p, &status_word.sw2, sizeof(status_word.sw2), p_max);
        }



        // One could decode directly into this data structure, but it is
        // good to do it this way to make sure data types and sizes are
        // properly handled by doing these assignments with implicit casts.
        update_result.status_word.sw1 = status_word.sw1;
        update_result.status_word.sw2 = status_word.sw2;
        update_result.uicc_status_code = uicc_status_code;
        update_result.uicc_status_code_fail_details = uicc_status_code_fail_details;

        closure.func(STE_UICC_CAUSE_REQ_SIM_FILE_UPDATE_PLMN, /* Cause */
        client_tag, &update_result, closure.user_data);
    }
        break;
    case STE_UICC_RSP_READ_PLMN: {
        ste_uicc_sim_file_read_plmn_response_t read_result;
        ste_sim_plmn_with_AcT_t               *data_p = NULL;

        if (buf == NULL || len == 0) {
            read_result.uicc_status_code = STE_UICC_STATUS_CODE_FAIL;
            read_result.uicc_status_code_fail_details
                    = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
            read_result.status_word.sw1 = 0;
            read_result.status_word.sw2 = 0;
        } else {
            ste_sim_decode_plmn_response(&read_result, buf, len);
            data_p = read_result.ste_sim_plmn_with_AcT_p;
        }
        closure.func(STE_UICC_CAUSE_REQ_SIM_FILE_READ_PLMN, /* Cause */
        client_tag, &read_result, closure.user_data);
        if (data_p)
            free(data_p);
    }
        break;
    case STE_UICC_RSP_CARD_STATUS:
        {
          ste_uicc_sim_card_status_response_t data;

          const char *p = buf;
          const char *p_max = buf + len;
          memset(&data, 0, sizeof data);
          p = sim_dec(p, &data.status, sizeof(data.status), p_max);
          if (data.status != SIM_UICC_STATUS_CODE_OK) {
            closure.func(STE_UICC_CAUSE_REQ_APP_STATUS,
                         client_tag,
                         &data,
                         closure.user_data);
            break;
          }
          p = sim_dec(p, &data.num_apps, sizeof(data.num_apps), p_max);
          p = sim_dec(p, &data.card_type, sizeof(data.card_type), p_max);
          p = sim_dec(p, &data.card_state, sizeof(data.card_state), p_max);
          p = sim_dec(p, &data.upin_state, sizeof(data.upin_state), p_max);

          if ( !p ) {
            break;
          }
          closure.func(STE_UICC_CAUSE_REQ_CARD_STATUS,
                       client_tag,
                       &data,
                       closure.user_data);
        }
        break;
    case STE_UICC_RSP_APP_STATUS:
        {
          ste_uicc_sim_app_status_response_t data;
          char *p_label = NULL;
          const char *p = buf;
          const char *p_max = buf + len;

          memset(&data, 0, sizeof data);
          p = sim_dec(p, &data.status, sizeof(data.status), p_max);
          if (data.status != SIM_UICC_STATUS_CODE_OK) {
            closure.func(STE_UICC_CAUSE_REQ_APP_STATUS,
                         client_tag,
                         &data,
                         closure.user_data);
            break;
          }
          p = sim_dec(p, &data.app_type, sizeof(data.app_type), p_max);
          p = sim_dec(p, &data.app_state, sizeof(data.app_state), p_max);
          p = sim_dec(p, &data.aid_len, sizeof(data.aid_len), p_max);
          p = sim_dec(p, data.aid, data.aid_len, p_max);
          p = sim_dec(p, &data.label_len, sizeof(data.label_len), p_max);
          // The label is recommended to be max 32 bytes, but that is
          // not a firm requirement, hence malloc that.
          // Data is coded as a string, add 1 for term zero
          // string pointer cannot be null
          p_label = malloc(data.label_len+1);
          if (!p_label) break;
          p = sim_dec(p, p_label, data.label_len+1, p_max);
          data.label = p_label;
          p = sim_dec(p, &data.mark, sizeof(data.mark), p_max);
          p = sim_dec(p, &data.pin_mode, sizeof(data.pin_mode), p_max);
          p = sim_dec(p, &data.pin_state, sizeof(data.pin_state), p_max);
          p = sim_dec(p, &data.pin2_state, sizeof(data.pin2_state), p_max);
          if (!p) break;
          closure.func(STE_UICC_CAUSE_REQ_APP_STATUS,
                       client_tag,
                       &data,
                       closure.user_data);
          if (p_label) free(p_label);
        }
        break;

    case STE_UICC_RSP_SIM_CHANNEL_SEND:
    {
        ste_uicc_sim_channel_send_response_t data;
        const char *p = buf;
        const char *p_max = buf + len;

        memset(&data, 0, sizeof data);
        p = sim_dec(p, &data.uicc_status_code, sizeof(data.uicc_status_code),p_max);
        p = sim_dec(p, &data.uicc_status_code_fail_details, sizeof(data.uicc_status_code_fail_details),p_max);
        p = sim_dec(p, &data.status_word.sw1, sizeof(data.status_word.sw1),p_max);
        p = sim_dec(p, &data.status_word.sw2, sizeof(data.status_word.sw2),p_max);
        p = sim_dec(p, &data.apdu_len, sizeof(data.apdu_len),p_max);

        if (data.apdu_len > 0) {
            data.apdu = malloc(data.apdu_len);
            if (data.apdu == NULL) {
                data.uicc_status_code_fail_details = SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR;
                closure.func(STE_UICC_CAUSE_REQ_SIM_CHANNEL_SEND,
                             client_tag,
                             &data,
                             closure.user_data);
                break;
            }
            p = sim_dec(p, data.apdu, sizeof(uint8_t)*(data.apdu_len),p_max);
        }

        if (!p) {
            free(data.apdu);
            data.apdu = NULL;
            data.uicc_status_code_fail_details = SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR;
            closure.func(STE_UICC_CAUSE_REQ_SIM_CHANNEL_SEND,
                         client_tag,
                         &data,
                         closure.user_data);
            break;
        }
        closure.func(STE_UICC_CAUSE_REQ_SIM_CHANNEL_SEND,
                     client_tag,
                     &data,
                     closure.user_data);
        free(data.apdu);
    }
    break;
    case STE_UICC_RSP_SIM_CHANNEL_OPEN:
    {
        ste_uicc_sim_channel_open_response_t data;
        const char *p = buf;
        const char *p_max = buf + len;

        memset(&data, 0, sizeof data);
        p = sim_dec(p, &data.uicc_status_code, sizeof(data.uicc_status_code),p_max);
        p = sim_dec(p, &data.uicc_status_code_fail_details, sizeof(data.uicc_status_code_fail_details),p_max);
        p = sim_dec(p, &data.status_word.sw1, sizeof(data.status_word.sw1),p_max);
        p = sim_dec(p, &data.status_word.sw2, sizeof(data.status_word.sw2),p_max);
        p = sim_dec(p, &data.session_id, sizeof(data.session_id),p_max);

        if (!p) {
            data.uicc_status_code_fail_details = SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR;
            closure.func(STE_UICC_CAUSE_REQ_SIM_CHANNEL_OPEN,
                         client_tag,
                         &data,
                         closure.user_data);
            break;
        }

        closure.func(STE_UICC_CAUSE_REQ_SIM_CHANNEL_OPEN,
                     client_tag,
                     &data,
                     closure.user_data);
    }
    break;
    case STE_UICC_RSP_SIM_CHANNEL_CLOSE:
    {
        ste_uicc_sim_channel_close_response_t data;
        const char *p = buf;
        const char *p_max = buf + len;

        memset(&data, 0, sizeof data);
        p = sim_dec(p, &data.uicc_status_code, sizeof(data.uicc_status_code),p_max);
        p = sim_dec(p, &data.uicc_status_code_fail_details, sizeof(data.uicc_status_code_fail_details),p_max);
        p = sim_dec(p, &data.status_word.sw1, sizeof(data.status_word.sw1),p_max);
        p = sim_dec(p, &data.status_word.sw2, sizeof(data.status_word.sw2),p_max);

        if (!p) {
            data.uicc_status_code_fail_details = SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR;
            closure.func(STE_UICC_CAUSE_REQ_SIM_CHANNEL_CLOSE,
                         client_tag,
                         &data,
                         closure.user_data);
            break;
        }

        closure.func(STE_UICC_CAUSE_REQ_SIM_CHANNEL_CLOSE,
                     client_tag,
                     &data,
                     closure.user_data);
    }
    break;
    case STE_UICC_RSP_READ_FDN:
      {
        ste_uicc_sim_fdn_response_t data;
        const char *p = buf;
        const char *p_max = buf + len;
        int i, rv = 0;
        size_t fdn_len = 0;
        ste_uicc_sim_fdn_record_t *fdn = NULL;

        memset(&data, 0, sizeof(data));
        // do-while block to break out of to reduce block nesting
        do {
          p = sim_dec(p, &data.uicc_status_code, sizeof(data.uicc_status_code),p_max);
          p = sim_dec(p, &data.uicc_status_code_fail_details, sizeof(data.uicc_status_code_fail_details),p_max);
          p = sim_dec(p, &data.status_word.sw1, sizeof(data.status_word.sw1),p_max);
          p = sim_dec(p, &data.status_word.sw2, sizeof(data.status_word.sw2),p_max);
          if (data.uicc_status_code == SIM_UICC_STATUS_CODE_FAIL) {
            rv = -1;
            break;
          }
          p = sim_dec(p, &fdn_len, sizeof(fdn_len), p_max);
          if (fdn_len == 0) {
            // Successful, but no fdn numbers on card found
            break;
          }
          fdn = (ste_uicc_sim_fdn_record_t *)malloc(fdn_len*sizeof(*fdn));
          if (!fdn) {
            rv = -1;
            break;
          }
          data.fdn_record_p = fdn;
          data.number_of_records = fdn_len;
          for (i = 0; i< (int)fdn_len; i++) {
            size_t code_size;
            size_t alpha_size;
            uint8_t *code;
            uint8_t *alpha;

            // Parse out the dial string
            p = sim_dec(p, &fdn[i].dial_string.text_coding, sizeof(fdn[i].dial_string.text_coding), p_max);
            p = sim_dec(p, &fdn[i].dial_string.no_of_characters, sizeof(fdn[i].dial_string.no_of_characters), p_max);

            p = sim_dec(p, &code_size, sizeof(code_size), p_max);
            if (code_size > 0) {
              code = (uint8_t *)malloc(code_size);
              if (!code) {
                rv = -1;
                break;
              }
              p = sim_dec(p, code, code_size, p_max);
              fdn[i].dial_string.text_p = code;
            }
            else {
               fdn[i].dial_string.text_p = NULL;
            }

            // Parse out the alpha string
            p = sim_dec(p, &fdn[i].alpha_string.text_coding, sizeof(fdn[i].alpha_string.text_coding), p_max);
            p = sim_dec(p, &fdn[i].alpha_string.no_of_characters, sizeof(fdn[i].alpha_string.no_of_characters), p_max);
            p = sim_dec(p, &alpha_size, sizeof(alpha_size), p_max);
            if (alpha_size > 0) {
              alpha = (uint8_t *)malloc(alpha_size);
              if (!alpha) {
                rv = -1;
                break; // do ... while(0)
              }
              p = sim_dec(p, alpha, alpha_size, p_max);
              fdn[i].alpha_string.text_p = alpha;
            }
            else {
              fdn[i].alpha_string.text_p = NULL;
            }

            p = sim_dec(p, &fdn[i].ton, sizeof(fdn[i].ton), p_max);
            p = sim_dec(p, &fdn[i].npi, sizeof(fdn[i].npi), p_max);
          }
          if (!p) {
            rv = -1;
            break;
          }
        } while (0);

        // Check on error
         if (rv < 0) {
          data.uicc_status_code = SIM_UICC_STATUS_CODE_FAIL;
          data.uicc_status_code_fail_details = SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR;
         data.fdn_record_p = NULL;
         data.number_of_records = 0;
        }

        closure.func(STE_UICC_CAUSE_REQ_SIM_FILE_READ_FDN,
                     client_tag,
                     &data,
                     closure.user_data);
        for (i = 0; i<(int)fdn_len; i++) {
          if (fdn[i].dial_string.text_p) {
            free(fdn[i].dial_string.text_p);
          }
          if (fdn[i].alpha_string.text_p) {
            free(fdn[i].alpha_string.text_p);
          }
        }
        free(fdn);
      }
      break;
    case STE_UICC_RSP_READ_ECC:
      {
        ste_uicc_sim_ecc_response_t data;
        const char *p = buf;
        const char *p_max = buf + len;
        int i;
        int rv = 0;
        size_t ecc_len = 0;
        ste_uicc_sim_ecc_number_t *ecc = NULL;

        memset(&data, 0, sizeof(data));
        // do-while block to break out of to reduce block nesting
        do {
          p = sim_dec(p, &data.uicc_status_code, sizeof(data.uicc_status_code),p_max);
          p = sim_dec(p, &data.uicc_status_code_fail_details, sizeof(data.uicc_status_code_fail_details),p_max);
          p = sim_dec(p, &data.status_word.sw1, sizeof(data.status_word.sw1),p_max);
          p = sim_dec(p, &data.status_word.sw2, sizeof(data.status_word.sw2),p_max);
          if (data.uicc_status_code == SIM_UICC_STATUS_CODE_FAIL) {
            rv = -1;
            break;
          }

          p = sim_dec(p, &ecc_len, sizeof(ecc_len), p_max);
          if (ecc_len == 0) {
            // Successful, but no ecc numbers on card found
            break;
          }
          ecc = (ste_uicc_sim_ecc_number_t *)calloc(1, ecc_len*sizeof(*ecc));
          if (!ecc) {
            rv = -1;
            break;
          }

          data.ecc_number_p = ecc;
          data.number_of_records = ecc_len;

          for (i = 0; i< (int)ecc_len; i++) {
            int num_len;
            ste_uicc_ecc_category_t cat;

            // There must always be a number here
            p = sim_dec(p, &num_len, sizeof(num_len), p_max);
            p = sim_dec(p, ecc[i].number, num_len, p_max);
            ecc[i].number[num_len] = 0;
            p = sim_dec(p, &ecc[i].length, sizeof(ecc[i].length), p_max);
            if (ecc[i].length > 0) {
              ecc[i].alpha = (uint8_t *)malloc(ecc[i].length);
              if (!ecc[i].alpha) {
                rv = -1;
                break; // loop
              }
              p = sim_dec(p, ecc[i].alpha, ecc[i].length, p_max);
            }
            else {
              ecc[i].alpha = NULL;
            }

            p = sim_dec(p, &cat, sizeof(cat), p_max);
            ecc[i].category = cat;
          }
          if (!p) {
            rv = -1;
            break;
          }
        } while (0);
        // Check on error
        if (rv < 0) {
          printf("sim.c: ecc fail\n");
          data.uicc_status_code = SIM_UICC_STATUS_CODE_FAIL;
          data.uicc_status_code_fail_details = SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR;
          data.ecc_number_p = NULL;
          data.number_of_records = 0;
        }
        closure.func(STE_UICC_CAUSE_REQ_SIM_FILE_READ_ECC,
                     client_tag,
                     &data,
                     closure.user_data);

        for (i = 0; i<(int)ecc_len; i++) {
          if (ecc[i].alpha) {
            free(ecc[i].alpha);
          }
        }

        free(ecc);
      }
      break;
    case STE_UICC_RSP_RESET:
    {
        ste_uicc_sim_reset_response_t      result;
        const char                        *p = buf;
        const char                        *p_max = buf + len;
        ste_uicc_status_word_t             status_word;
        ste_uicc_status_code_t             uicc_status_code;
        ste_uicc_status_code_fail_details_t uicc_status_code_fail_details;

        // split msg_data into separate parameters
        p = sim_dec(p, &uicc_status_code, sizeof(uicc_status_code), p_max);
        p = sim_dec(p, &uicc_status_code_fail_details, sizeof(uicc_status_code_fail_details), p_max);
        p = sim_dec(p, &status_word.sw1, sizeof(status_word.sw1), p_max);
        p = sim_dec(p, &status_word.sw2, sizeof(status_word.sw2), p_max);
        if ( !p ) {
            break;
        }

        result.uicc_status_code = uicc_status_code;
        result.uicc_status_code_fail_details = uicc_status_code_fail_details;
        result.status_word.sw1 = status_word.sw1;
        result.status_word.sw2 = status_word.sw2;

        closure.func(STE_UICC_CAUSE_REQ_SIM_RESET,      /* Cause */
                     client_tag,
                     &result,
                     closure.user_data);

    }
    break;
    case STE_UICC_RSP_SIM_POWER_ON:
    {
        ste_uicc_sim_power_on_response_t   result;
        const char                        *p = buf;
        const char                        *p_max = buf + len;
        ste_uicc_status_word_t             status_word;
        ste_uicc_status_code_t             uicc_status_code;
        ste_uicc_status_code_fail_details_t uicc_status_code_fail_details;

        // split msg_data into separate parameters
        p = sim_dec(p, &uicc_status_code, sizeof(uicc_status_code), p_max);
        p = sim_dec(p, &uicc_status_code_fail_details, sizeof(uicc_status_code_fail_details), p_max);
        p = sim_dec(p, &status_word.sw1, sizeof(status_word.sw1), p_max);
        p = sim_dec(p, &status_word.sw2, sizeof(status_word.sw2), p_max);
        if ( !p ) {
            break;
        }

        result.uicc_status_code = uicc_status_code;
        result.uicc_status_code_fail_details = uicc_status_code_fail_details;
        result.status_word.sw1 = status_word.sw1;
        result.status_word.sw2 = status_word.sw2;

        closure.func(STE_UICC_CAUSE_REQ_SIM_POWER_ON,      /* Cause */
                     client_tag,
                     &result,
                     closure.user_data);
    }
    break;

    case STE_UICC_RSP_SIM_POWER_OFF:
    {
        ste_uicc_sim_power_off_response_t  result;
        const char                        *p = buf;
        const char                        *p_max = buf + len;
        ste_uicc_status_word_t             status_word;
        ste_uicc_status_code_t             uicc_status_code;
        ste_uicc_status_code_fail_details_t uicc_status_code_fail_details;

        // split msg_data into separate parameters
        p = sim_dec(p, &uicc_status_code, sizeof(uicc_status_code), p_max);
        p = sim_dec(p, &uicc_status_code_fail_details, sizeof(uicc_status_code_fail_details), p_max);
        p = sim_dec(p, &status_word.sw1, sizeof(status_word.sw1), p_max);
        p = sim_dec(p, &status_word.sw2, sizeof(status_word.sw2), p_max);
        if ( !p ) {
            break;
        }

        result.uicc_status_code = uicc_status_code;
        result.uicc_status_code_fail_details = uicc_status_code_fail_details;
        result.status_word.sw1 = status_word.sw1;
        result.status_word.sw2 = status_word.sw2;

        closure.func(STE_UICC_CAUSE_REQ_SIM_POWER_OFF,      /* Cause */
                     client_tag,
                     &result,
                     closure.user_data);

    }
    break;

    case STE_UICC_RSP_SIM_READ_PREFERRED_RAT_SETTING:
    {
        ste_uicc_sim_read_preferred_RAT_setting_response_t  result;
        const char                        *p = buf;
        const char                        *p_max = buf + len;
        ste_uicc_status_code_t             uicc_status_code;
        ste_uicc_status_code_fail_details_t uicc_status_code_fail_details;
        ste_uicc_sim_RAT_setting_t         RAT;

        // split msg_data into separate parameters
        p = sim_dec(p, &uicc_status_code, sizeof(uicc_status_code), p_max);
        p = sim_dec(p, &uicc_status_code_fail_details, sizeof(uicc_status_code_fail_details), p_max);
        p = sim_dec(p, &RAT, sizeof(RAT), p_max);

        if ( !p ) {
            break;
        }

        result.uicc_status_code = uicc_status_code;
        result.uicc_status_code_fail_details = uicc_status_code_fail_details;
        result.RAT = RAT;

        closure.func(STE_UICC_CAUSE_REQ_SIM_READ_PREFERRED_RAT_SETTING,      /* Cause */
                     client_tag,
                     &result,
                     closure.user_data);

    }
    break;

    default:
      printf("ste_cat_i_handle_command_th: BAD COMMAND: cmd=%d(0x%x), len=%d", cmd, cmd, len);
      break;
    }
}


int ste_sim_fd(const ste_sim_t * sim)
{
    int fd = CLOSED_FD;
    // Only return the fd if we are in single threaded mode.
    if ( sim && sim->is_st )
        fd = sim->fd;
    return fd;
}


int ste_sim_read(ste_sim_t* sim)
{
    ssize_t                 n;
    ste_es_t * es;

    if ( !sim || !(sim->is_st) )
        return -1;

    es = sim->es;

    n = ste_es_read(es);
    if (n > 0) {
        n = ste_es_parse(es);
    }

    if (ste_es_state(es) != STE_ES_STATE_OPEN) {
        return -1;
    }

    return 0;
}

int ste_sim_connect_mt(ste_sim_t * sim, uintptr_t client_tag)
{
    int                     rv = STE_SIM_ERROR_UNKNOWN;
    do {
        int                     fd;
        int                     thread_pipe;
        int                     input_pipe;
        ste_sim_thread_data_t  *tdata;
        pthread_t               tid;

        if (!sim) {
            rv = STE_SIM_ERROR_BAD_HANDLE;
            break;
        }
        if (sim->state == ste_sim_state_connected) {
            rv = STE_SIM_SUCCESS;
            break;
        }


        fd = ste_sim_i_connect(socket_name);
        if (fd == CLOSED_FD) {
            rv = STE_SIM_ERROR_SOCKET;
            break;
        }

        rv = sim_create_pipe(&thread_pipe, &input_pipe);
        if (rv < 0) {
            rv = STE_SIM_ERROR_PIPE;
            close(fd);
            break;
        }

        tdata = malloc(sizeof(ste_sim_thread_data_t));
        if (!tdata) {
            rv = STE_SIM_ERROR_RESOURCE;
            close(fd);
            close(thread_pipe);
            close(input_pipe);
            break;
        }

        tdata->input_pipe = input_pipe;
        tdata->fd = fd;
        tdata->closure = sim->closure;
        tdata->client_tag = client_tag;
        tdata->sim = 0;
        tdata->is_st = 0;

        sim->thread_pipe = thread_pipe;
        sim->fd = fd;

        sim->state = ste_sim_state_connected;

        rv = sim_launch_thread(&tid, PTHREAD_CREATE_JOINABLE, ste_sim_i_reader_func_th, tdata);
        sim->rdr_tid = tid;

        if (rv < 0) {
            rv = STE_SIM_ERROR_RESOURCE;
            close(fd);
            close(thread_pipe);
            close(input_pipe);
            free(tdata);
            sim->rdr_tid = 0;
            sim->state = ste_sim_state_disconnected;
            break;
        }

    } while (0);
    return rv;
}



static int ste_sim_connect_st(ste_sim_t * sim, uintptr_t client_tag)
{
    int                     rv = STE_SIM_ERROR_UNKNOWN;
    do {
        int                     fd;
        ste_es_t               *es = 0;
        ste_es_parser_closure_t pc;
        ste_sim_thread_data_t  *tdata;

        if (!sim) {
            rv = STE_SIM_ERROR_BAD_HANDLE;
            break;
        }
        if (sim->state == ste_sim_state_connected) {
            rv = STE_SIM_SUCCESS;
            break;
        }

        fd = ste_sim_i_connect(socket_name);
        if (fd == CLOSED_FD) {
            rv = STE_SIM_ERROR_SOCKET;
            break;
        }

        // MUST have thread data, also in st mode!
        tdata = malloc(sizeof(ste_sim_thread_data_t));
        if (!tdata) {
            rv = STE_SIM_ERROR_RESOURCE;
            close(fd);
            break;
        }

        tdata->input_pipe = -1;
        tdata->fd = fd;
        tdata->closure = sim->closure;
        tdata->client_tag = client_tag;
        tdata->sim = sim;
        tdata->is_st = 1;

        pc.func = ste_sim_i_reader_parse_func_th;
        pc.user_data = tdata;

        es = ste_es_new_normal(fd, SIM_READER_INPUT_BUFFER_SIZE, &pc);
        if (!es) {
            free(tdata);
            close(fd);
            break;
        }

        sim->fd = fd;
        sim->es = es;
        sim->state = ste_sim_state_connected;
        rv = STE_SIM_SUCCESS;

        /* Call the call back to indicate the connection is up and running */
        sim->closure.func(STE_SIM_CAUSE_CONNECT,    /* Cause */
                          client_tag,
                          0,        /* No payload */
                          sim->closure.user_data);

    } while (0);
    return rv;
}



int ste_sim_connect(ste_sim_t * sim, uintptr_t client_tag)
{
    int i = STE_SIM_ERROR_UNKNOWN;
    if ( sim ) {
        if ( sim->is_st ) {
            i = ste_sim_connect_st(sim, client_tag);
        }
        else {

            if (signal((int)SIGPIPE, SIG_IGN) == SIG_ERR) {
                //simd_log_f("#\tste_sim_connect: Could not ignore SIGPIPE");
                i = STE_SIM_ERROR_PIPE;
            } else  {
                i = ste_sim_connect_mt(sim, client_tag);
            }

        }
    }
    return i;
}

int ste_sim_disconnect_mt(ste_sim_t * sim, uintptr_t client_tag)
{
    int                     rv = STE_SIM_ERROR_UNKNOWN;

    do {
        if (!sim) {
            rv = STE_SIM_ERROR_BAD_HANDLE;
            break;
        }
        if (sim->state != ste_sim_state_connected) {
            rv = STE_SIM_ERROR_NOT_CONNECTED;
            break;
        }

        ste_sim_i_kill_thread(sim, client_tag);

        sim->state = ste_sim_state_disconnected;
        sim->fd = CLOSED_FD;
        close(sim->thread_pipe);
        sim->thread_pipe = CLOSED_FD;

        rv = STE_SIM_SUCCESS;
    } while (0);
    return rv;
}

static int ste_sim_disconnect_st(ste_sim_t * sim, uintptr_t client_tag)
{
    int                     rv = STE_SIM_ERROR_UNKNOWN;
    do {
        if (!sim) {
            rv = STE_SIM_ERROR_BAD_HANDLE;
            break;
        }
        if (sim->state != ste_sim_state_connected) {
            rv = STE_SIM_ERROR_NOT_CONNECTED;
            break;
        }

        close( sim->fd );
        sim->state = ste_sim_state_disconnected;
        sim->fd = CLOSED_FD;

        rv = STE_SIM_SUCCESS;

        /* Call the call back to indicate the connection is up and running */
        sim->closure.func(STE_SIM_CAUSE_DISCONNECT,    /* Cause */
                          client_tag,
                          0,        /* No payload */
                          sim->closure.user_data);

    } while (0);
    return rv;
}

int ste_sim_disconnect(ste_sim_t * sim, uintptr_t client_tag)
{
    int i = STE_SIM_ERROR_UNKNOWN;
    if ( sim ) {
        if ( sim->is_st ) {
            i = ste_sim_disconnect_st(sim, client_tag);
        }
        else {
            i = ste_sim_disconnect_mt(sim, client_tag);
        }
    } else {
        i = STE_SIM_ERROR_BAD_HANDLE;
    }
    return i;
}



int ste_sim_ping(ste_sim_t * sim, uintptr_t client_tag)
{
    int                     rv = STE_SIM_ERROR_UNKNOWN;
    if (sim && sim->state == ste_sim_state_connected) {
        rv = sim_send_generic(sim->fd, STE_SIM_REQ_PING, 0, 0, client_tag);
        if (rv < 0) {
            rv = ste_sim_disconnect(sim, client_tag);
        }
    }
    return rv;
}




int ste_sim_shutdown(ste_sim_t * cat, uintptr_t client_tag)
{
    int                     rv = STE_SIM_ERROR_UNKNOWN;
    if (cat && cat->state == ste_sim_state_connected) {
        rv = sim_send_generic(cat->fd, STE_SIM_REQ_SHUTDOWN, 0, 0, client_tag);
        if (rv < 0) {
            rv = ste_sim_disconnect(cat, client_tag);
        }
    }
    return rv;
}

int ste_sim_startup(ste_sim_t * cat, uintptr_t client_tag)
{
    int                     rv = -1;
    if (cat && cat->state == ste_sim_state_connected) {
        rv = sim_send_generic(cat->fd, STE_SIM_REQ_STARTUP, 0, 0, client_tag);
        if (rv < 0) {
            printf("ste_sim_startup: Error!\n");
            rv = ste_sim_disconnect(cat, client_tag);
        }
    }
    return rv;
}

int ste_sim_set_sim_log_level(ste_sim_t * cat,
                              uintptr_t client_tag,
                              int level)
{
    int                     rv = -1;
    if (cat && cat->state == ste_sim_state_connected) {
        rv = sim_send_generic(cat->fd, STE_SIM_REQ_SET_SIM_DEBUG_LEVEL, (char*)&level, sizeof(level), client_tag);
        if (rv < 0) {
            printf("ste_sim_set_sim_log_level: Error!\n");
            rv = ste_sim_disconnect(cat, client_tag);
        }
    }
    return rv;
}

int ste_sim_set_modem_log_level(ste_sim_t * cat,
                                uintptr_t client_tag,
                                int level)
{
    int                     rv = -1;
    if (cat && cat->state == ste_sim_state_connected) {
        rv = sim_send_generic(cat->fd, STE_SIM_REQ_SET_MODEM_DEBUG_LEVEL, (char*)&level, sizeof(level), client_tag);
        if (rv < 0) {
            printf("ste_sim_set_modem_log_level: Error!\n");
            rv = ste_sim_disconnect(cat, client_tag);
        }
    }
    return rv;
}




ste_sim_t              *ste_sim_new(const ste_sim_closure_t * closure)
{
    ste_sim_t              *me = malloc(sizeof(ste_sim_t));
    if (me) {
        me->state = ste_sim_state_disconnected;
        me->closure = *closure;
        me->thread_pipe = -1;
        me->fd = -1;
        me->rdr_tid = -1;
        me->is_st = 0;
        me->es = NULL;
    }
    return me;
}

ste_sim_t* ste_sim_new_st(const ste_sim_closure_t * closure)
{
    ste_sim_t              *me = malloc(sizeof(ste_sim_t));
    if (me) {
        me->state = ste_sim_state_disconnected;
        me->closure = *closure;
        me->thread_pipe = -1;
        me->fd = -1;
        me->rdr_tid = -1;
        me->is_st = 1;
        me->es = NULL;
    }
    return me;
}

void ste_sim_delete(ste_sim_t * sim, uintptr_t client_tag)
{
    if (sim) {
        ste_es_t *es = sim->es;
        int is_st = sim->is_st;

        sim->es = 0;

        if ( sim->state != ste_sim_state_disconnected ) {
            int rv = ste_sim_disconnect((ste_sim_t *)sim, client_tag);
            if ( rv ) {
                printf("ste_sim_delete: disconnect fail\n");
            }
        }

        free(sim);

        if (is_st && es) {
            ste_es_delete(es);
        }
    }
}
