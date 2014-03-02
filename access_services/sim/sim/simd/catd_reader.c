/*                               -*- Mode: C -*-
 * Copyright (C) 2010, ST-Ericsson
 *
 * File name       : catd_reader.c
 * Description     : Reader thread form catd.
 *
 * Author          : Mats Bergstrom <mats.m.bergstrom@stericsson.com>
 *
 */

// FIXME: rename catd_reader, sim_reader
// FIXME: Move OUT the catd related stuff to a new catd/catd_reader.c
// FIXME: Have the catd/catd_reader.c register event streams.

#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdint.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "simd.h"
#include "catd.h"
#include "uiccd.h"
#include "catd_reader.h"
#include "cat_internal.h"
#include "sim_internal.h"
#include "cat_barrier.h"
#include "simd.h"
#include "func_trace.h"
#include "util_security.h"

#ifndef HAVE_ANDROID_OS
#ifndef HOST_FS_PATH
#define HOST_FS_PATH ""
#endif
static const char       socket_name[] = HOST_FS_PATH "/tmp/catd_a";
#else
static const char       socket_name[] = "/dev/socket/catd_a";
#endif

#define INPUT_BUFFER_SIZE       (4096)


#define STE_CATD_I_CMD_DIE       (0x00dead00)
#define STE_CATD_I_CMD_ADD_ES    (0xadd0a0e5)
#define STE_CATD_I_CMD_REM_ES    (0x580fd000)

struct ste_catd_reader_cmd_s {
    uint32_t                type;
    void                   *vptr;
};
typedef struct ste_catd_reader_cmd_s ste_catd_reader_cmd_t;


#define READER_PIPE_BUFFER_SIZE    (64)


typedef struct {
    size_t                  alen;       /* Allocated size */
    size_t                  ulen;       /* Used size */
    ste_es_t              **a;
} ste_es_array_t;



struct ste_catd_reader_s {
    void                   *queue;
    int                     thread_pipe;        /* -1 if reader thread is not running */
};


typedef struct {
    int                     input_pipe;         /* -1 when thread is dying */
    int                     input_socket;
    ste_es_array_t         *es_array;
    cat_barrier_t          *bar;                /* barrier to be used in shutdown */
} ste_catd_reader_thread_data_t;






ste_es_array_t         *ste_es_array_new(size_t n)
{
    ste_es_array_t         *p = malloc(sizeof(ste_es_array_t));
    if (p) {
        size_t                  i;
        p->alen = n;
        p->ulen = 0;
        p->a = malloc(n * sizeof(ste_es_t *));
        for (i = 0; i < n; ++i)
            p->a[i] = 0;
    }
    return p;
}

void ste_es_array_del(ste_es_array_t * p)
{
    if (p) {
        free(p->a);
        free(p);
    }
}



#define ALEN_INC (4)
void ste_es_array_add(ste_es_array_t * arr, ste_es_t * es)
{
    if (arr->ulen == arr->alen) {
        size_t                  i;
        ste_es_t              **a =
            malloc((arr->alen + ALEN_INC) * sizeof(ste_es_t *));
        for (i = 0; i < arr->alen; ++i) {
            a[i] = arr->a[i];
        }
        free(arr->a);
        arr->a = a;
        arr->alen += ALEN_INC;
        for ( /*EMPTY*/; i < arr->alen; ++i) {  /* clear remainders */
            arr->a[i] = 0;
        }
    }

    arr->a[arr->ulen] = es;
    ++(arr->ulen);
}



ste_es_t               *ste_es_array_rem(ste_es_array_t * arr, int fd)
{
    ste_es_t               *es = 0;
    size_t                  i;
    if (fd == -1 && arr->ulen) {        /* get any if fd == -1 */
        fd = ste_es_fd(arr->a[0]);
    }
    for (i = 0; i < arr->ulen; ++i) {
        if (ste_es_fd(arr->a[i]) == fd) {
            es = arr->a[i];
            arr->a[i] = 0;
            ++i;
            while (i < arr->ulen) {
                arr->a[i - 1] = arr->a[i];
                arr->a[i] = 0;
                ++i;
            }
            --(arr->ulen);
            break;
        }
    }
    return es;
}

size_t ste_es_array_len(const ste_es_array_t * arr)
{
    return arr->ulen;
}


int ste_es_array_set(const ste_es_array_t * arr, fd_set * rset)
{
    int                     max_fd = 0;
    size_t                  i;
    for (i = 0; i < arr->ulen; ++i) {
        ste_es_t               *es = arr->a[i];
        if (es) {
            int                     fd = ste_es_fd(es);
            FD_SET(fd, rset);
            if (fd > max_fd)
                max_fd = fd;
        }
    }
    return max_fd;
}



static int
ste_es_array_read_and_parse(ste_es_array_t * arr, fd_set * rset)
{
    size_t                  i;

    for (i = 0; i < arr->ulen; ++i) {
        ste_es_t               *es = arr->a[i];
        if (es) {
            int                     fd = ste_es_fd(es);
            if (FD_ISSET(fd, rset)) {
                ssize_t                 n;
                n = ste_es_read(es);
                if (n > 0) {
                    n = ste_es_parse(es);
                }

                if (ste_es_state(es) == STE_ES_STATE_RESET) {
                    catd_log_s(SIM_LOGGING_I, "STE_ES_STATE_RESET", 0);
                    return -1;
                }

                if (ste_es_state(es) != STE_ES_STATE_OPEN) {
                    ste_es_t               *p = ste_es_array_rem(arr, fd);
                    if (p) {
                        ste_es_delete(p);
                        --i;
                        close(fd);
                    }
                    catd_log_f(SIM_LOGGING_I, "read : Closed connection on fd=%d.", fd);
                }
            }
        }
    }
    return 0;
}



typedef struct {
    //    int                     is; /* input socket */
    ste_es_array_t         *arr;        /* input array */
} reader_thread_data_t;





static void
ste_sim_reader_i_handle_command(uint16_t cmd,uintptr_t client_tag,
                                const char *buf,uint16_t len,
                                ste_sim_client_context_t * cc)
{
    const char             *bb = buf;
    uint16_t                target;

    buf = bb;
    target = (cmd >> 12) & 0x000f;

    catd_log_f(SIM_LOGGING_I, "read : Handling command: %04x len=%d", cmd, len);

    switch ( target ) {
    case 0:
        ste_simd_handle_command(cmd,client_tag,buf,len,cc);
        break;

    case 1:
        ste_catd_handle_command(cmd,client_tag,buf,len,cc);
        break;

    case 2:
        ste_uiccd_handle_command(cmd,client_tag,buf,len,cc);
        break;

    default:
        catd_log_f(SIM_LOGGING_E, "ste_sim_reader_i_handle_command : BAD COMMAND: %04x len=%d, no handler", cmd, len);
        break;
    }

}


static                  ssize_t
ste_sim_reader_i_parse_func_th(char *buf, char *buf_max, void *ud)
{
    const char             *p = buf;
    uint16_t                len;
    uint16_t                cmd;
    uintptr_t               client_tag;
    ssize_t                 bytes_consumed = 0;
    ste_sim_client_context_t *client_context = ud;

    if (!buf && !buf_max) {
        /* Destructor */
        simd_sig_disconnect(client_context->fd, 0);
        free(client_context);
    } else if (!buf_max) {
        abort();
    } else {
        do {
            catd_log_f(SIM_LOGGING_V,
                       "read : ste_sim_reader_i_parse_func_th: "
                       "%p %p %u",
                       (void*) p, (void*) buf_max,
                       (unsigned) (buf_max - p));

            p = sim_dec(p, &len, sizeof(len), buf_max);
            if (!p)
                break;

            if (buf_max - p < len) // TODO: Remove? (checked by the sim_dec function)
                break;

            p = sim_dec(p, &cmd, sizeof(cmd), buf_max);
            if (!p)
                break;

            p = sim_dec(p, &client_tag, sizeof(client_tag), buf_max);

            if (!p)
                break;

            len -= sizeof(cmd) + sizeof(client_tag);

            ste_sim_reader_i_handle_command(cmd, client_tag, p, len, client_context);

            p += len; // TODO: Why do we do this? (temp pointer not used again in the function, remove?)

            bytes_consumed += sizeof(len) + sizeof(cmd) + sizeof(client_tag) + len;
        } while (0);
    }

    return bytes_consumed;
}



static                  ssize_t
ste_sim_reader_i_accept_func_th(char *buf, char *buf_max, void *ud)
{
    int                     s;
    ste_es_t               *es;
    ste_es_parser_closure_t pc;
    ste_sim_client_context_t *cc;
    ste_catd_reader_thread_data_t *tdata =
        (ste_catd_reader_thread_data_t *) ud;

    if (!buf && !buf_max) {
        /* Destructor */
        /* free(thread_data); Not needed here! Done by the pipe parser func */
    } else if (!buf_max) {
        s = *(int *) buf;

        cc = malloc(sizeof(ste_sim_client_context_t));
        cc->fd = s;
        pc.func = ste_sim_reader_i_parse_func_th;
        pc.user_data = cc;
        es = ste_es_new_normal(s, INPUT_BUFFER_SIZE, &pc);
        if (!es) {
            catd_log_s(SIM_LOGGING_E, "read : Reader ste_es_new() failed", 0);
            close(s);
        } else {
            ste_es_array_add(tdata->es_array, es);
            simd_sig_connect(s, 0);
        }
    } else {
        abort();
    }

    return 1;
}



static                  ssize_t
ste_catd_reader_i_pipe_parse_func_th(char *buf, char *buf_max, void *ud)
{
    const char             *p = buf;
    ste_catd_reader_cmd_t   cmd;
    ssize_t                 bytes_consumed = 0;
    ste_catd_reader_thread_data_t *tdata =
        (ste_catd_reader_thread_data_t *) ud;

    if (!buf && !buf_max) {
        /* Destructor */
        /* EMPTY */
    } else if (!buf_max) {
        /* marks accept sockets, which we are not. */
        abort();
    } else {
        do {
            catd_log_f(SIM_LOGGING_V,
                       "read : from pipe  %p %p %u",
                       (void*) p, (void*) buf_max,
                       (unsigned) (buf_max - p));

            // Decode the command.
            p = sim_dec(p, &cmd, sizeof(cmd), buf_max);
            if (!p)
                break;
            bytes_consumed += sizeof(cmd);

            catd_log_f(SIM_LOGGING_V, "read : from pipe cmd = %08x %p", cmd.type,
                       cmd.vptr);

            if (cmd.type == STE_CATD_I_CMD_DIE) {
                close(tdata->input_pipe);
                tdata->input_pipe = -1;
                tdata->bar = cmd.vptr;
            } else if (cmd.type == STE_CATD_I_CMD_ADD_ES) {
                // Add the event stream to the array
                ste_es_t               *es;
                es = (ste_es_t *) cmd.vptr;
                ste_es_array_add(tdata->es_array, es);
                catd_log_f(SIM_LOGGING_I, "read : Added event stream %p ", es);
            } else if (cmd.type == STE_CATD_I_CMD_REM_ES) {
                // Remove the event stream to the array
                ste_es_t               *p;
                int                     fd = (intptr_t) cmd.vptr;
                p = ste_es_array_rem(tdata->es_array, fd);
                catd_log_f(SIM_LOGGING_I, "read : Removed event stream %p fd=%d ", p,
                           fd);
                if (p)
                    ste_es_delete(p);
            } else {
                // FIXME: Handle garbage
            }
        } while (0);
    }

    return bytes_consumed;
}



static void            *ste_catd_reader_i_func_th(void *vdata)
{
    ste_catd_reader_thread_data_t *tdata =
        (ste_catd_reader_thread_data_t *) vdata;
    setThreadName("catd_rd");
    struct timeval          tv;
    ste_es_t               *es_p = 0;
    ste_es_t               *es_s = 0;
    fd_set                  rset;
    int                     max_fd;
    int                     i;
    int                     ret = 0;

    ste_es_parser_closure_t pc;

    tdata->es_array = ste_es_array_new(4);
    if (!tdata->es_array) {
        // FIXME:  Call the callback function with cause.
        // FIXME: Cleanup
        return 0;
    }


    /* Set up reading from the control pipe */
    pc.func = ste_catd_reader_i_pipe_parse_func_th;
    pc.user_data = vdata;
    es_p =
        ste_es_new_normal(tdata->input_pipe, READER_PIPE_BUFFER_SIZE, &pc);
    if (!es_p) {
        // FIXME:  Call the callback function with cause.
        // FIXME: Cleanup
        return 0;
    }



    /* Set up accepting from the socket */
    pc.func = ste_sim_reader_i_accept_func_th;
    pc.user_data = vdata;
    es_s = ste_es_new_accept(tdata->input_socket, &pc);
    if (!es_s) {
        // FIXME:  Call the callback function with cause.
        // FIXME: Cleanup
        ste_es_delete( es_p );
        return 0;
    }

    ste_es_array_add(tdata->es_array, es_p);
    ste_es_array_add(tdata->es_array, es_s);



    // FIXME: Signal reader started.

#define catd_log_s(a,b,c)  printf("%s\n",b)
    for (;;) {
        FD_ZERO(&rset);
        max_fd = ste_es_array_set(tdata->es_array, &rset);

        tv.tv_sec = 60;
        tv.tv_usec = 0;

        {
            static int count = 5;
            if ( count && count-- ) {   // suppress after so many iterations
                catd_log_s(3, "read : Reader waiting on select", 0);
            }
        }
        i = select(max_fd + 1, &rset, 0, 0, &tv);

        if (i == 0) {           /* timer expired */
            static int count = 5;
            if ( count && count-- ) {   // suppress after so many iterations
                catd_log_s(3, "read : Reader timer expired", 0);
            }
        } else if (i < 0) {     /* Error */
            int                     e = errno;
            catd_log_s(3, "read : Reader select failed!", 0);
            if (e == EBADF) {
                catd_log_s(1, "read : Reader select failed EBADF", 0);
                break;
            } else if (e == EINTR) {
                catd_log_s(1, "read : Reader select failed EINTR", 0);
                /* ignore */
            } else if (e == EINVAL) {
                catd_log_s(1, "read : Reader select failed EINVAL", 0);
                break;
            } else if (e == ENOMEM) {
                catd_log_s(1, "read : Reader select failed ENOMEM", 0);
                break;
            } else {
                catd_log_s(1, "read : Reader select failed UNKNOWN", 0);
                break;
            }
        } else {
            ret = ste_es_array_read_and_parse(tdata->es_array, &rset);
            if ( ret < 0) {
                catd_log_s(SIM_LOGGING_I, "read : Sending shutdown signal!", 0);
                simd_sig_shutdown(-1, 0);
                break;
        }
            if (tdata->input_pipe < 0)
                break;
        }
    }
    catd_log_s(1, "read : exited for(;;) loop", 0);


    // FIXME: Signal reader died.

    //#if 0   /*FIXME: this cleanup presently segfaults */
    /* Cleanup. */
    while (ste_es_array_len(tdata->es_array)) {
        ste_es_t               *es = ste_es_array_rem(tdata->es_array, -1);
        if (es) {
            int                     fd = ste_es_fd(es);
            if (fd >= 0)
                close(fd);
            ste_es_delete(es);
            catd_log_s(1, "read : deleted an ES", 0);
        }
    }
    ste_es_array_del(tdata->es_array);
    catd_log_s(1, "read : deleted ES vector", 0);

    //#endif
    close(tdata->input_socket);

    cat_barrier_t   *bar = tdata->bar;
    free(tdata);

    if (bar) {
        cat_barrier_release(bar, 0);
    }

    return 0;
#undef catd_log_s
}



static void ste_catd_reader_i_kill_thread(ste_catd_reader_t * rdr)
{
    cat_barrier_t          *bar = 0;        /* Barrier to sync shutdown */
    ste_catd_reader_cmd_t   data = {
      .type = STE_CATD_I_CMD_DIE,
      .vptr = 0
    };

    bar = cat_barrier_new();
    if (!bar)
        abort();
    cat_barrier_set(bar);

    data.vptr = bar;
    write(rdr->thread_pipe, &data, sizeof(data));

    if ( cat_barrier_wait(bar, 0) == 0 ) {   /* Wait for thread to die */
        cat_barrier_delete(bar);
        bar = 0;
    }
}



ste_catd_reader_t      *ste_catd_reader_new(void *queue)
{
    ste_catd_reader_t      *me = malloc(sizeof(ste_catd_reader_t));
    if (me) {
        me->queue = queue;
        me->thread_pipe = -1;
    }
    return me;
}


void ste_catd_reader_delete(ste_catd_reader_t * rdr)
{
    if (rdr) {
        ste_catd_reader_stop(rdr);
        free(rdr);
    }
}




static int sim_create_unix_socket(const char *path)
{
    int                     fd;
    struct sockaddr_un      addr;
    int                     i;

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    ste_sim_i_safe_copy(addr.sun_path, path,
                                sizeof(addr.sun_path));


    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("failed to create socket");
        return -1;
    }

    unlink(addr.sun_path);      /* Remove old socket */

    i = bind(fd, (struct sockaddr *) &addr, sizeof(addr));
    if (i < 0) {
        perror("fail to bind socket");
        close(fd);
        return -1;
    }

    i = listen(fd, 3);          /* Allow 3 pending connections */
    if (i < 0) {
        perror("failed to listen");
        close(fd);
        return -1;
    }

    i = fcntl(fd, F_SETFL, O_NONBLOCK);
    if (i < 0) {
        perror("failed to set non-blocking");
        close(fd);
        return -1;
    }

    return fd;
}







int ste_catd_reader_start(ste_catd_reader_t * rdr)
{
    int                     rv = -1;
    do {
        int                     thread_pipe;
        int                     input_pipe;
        int                     input_socket;
        ste_catd_reader_thread_data_t *tdata;
        pthread_t               tid;

        if (!rdr) {
            break;
        }
        if (rdr->thread_pipe != -1) {
            break;
        }


        rv = sim_create_pipe(&thread_pipe, &input_pipe);
        if (rv < 0) {
            break;
        }

        input_socket = sim_create_unix_socket(socket_name);
        if ( input_socket < 0 ) {
            rv = -1;
            break;
        }

        tdata = malloc(sizeof(ste_catd_reader_thread_data_t));
        if (!tdata) {
            rv = -1;
            close(thread_pipe);
            close(input_pipe);
            close(input_socket);
            break;
        }

        tdata->input_pipe = input_pipe;
        tdata->input_socket = input_socket;
        tdata->es_array = 0;
        tdata->bar = 0;

        rv = sim_launch_thread(&tid, PTHREAD_CREATE_DETACHED, ste_catd_reader_i_func_th, tdata);
        if (rv < 0) {
            close(thread_pipe);
            close(input_pipe);
            close(input_socket);
            free(tdata);
            break;
        }

        rdr->thread_pipe = thread_pipe;

        // Super user access right not needed anymore
        util_continue_as_non_privileged_user();
    } while (0);
    return rv;
}


int ste_catd_reader_stop(ste_catd_reader_t * rdr)
{
    int                     rv = -1;
    do {
        if (!rdr)
            break;
        if (rdr->thread_pipe == -1)
            break;

        ste_catd_reader_i_kill_thread(rdr);

        close(rdr->thread_pipe);
        rdr->thread_pipe = -1;

        rv = 0;
    } while (0);
    return rv;
}



int ste_catd_reader_add_es(ste_catd_reader_t * rdr, ste_es_t * es)
{
    ssize_t                 n;
    ste_catd_reader_cmd_t   data = {
        .type = STE_CATD_I_CMD_ADD_ES,
        .vptr = es
    };

    n = write(rdr->thread_pipe, &data, sizeof(data));
    if (n != sizeof(data))
        return -1;
    return 0;
}



int ste_catd_reader_rem_es(ste_catd_reader_t * rdr, int fd)
{
    ssize_t                 n;
    ste_catd_reader_cmd_t   data = {
        .type = STE_CATD_I_CMD_REM_ES,
        .vptr = (void *) (intptr_t) fd
    };

    n = write(rdr->thread_pipe, &data, sizeof(data));
    if (n != sizeof(data))
        return -1;
    return 0;
}
